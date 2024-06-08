/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */


#include "nrfx_gpiote.h"

// #include "FreeRTOS.h"
// #include "queue.h"
// #include "semphr.h"
// #include "task.h"
// #include "timers.h"


#include "bsp/board_api.h" // TODO: remove
#include "tusb.h"

#include "tasks.h"

#define _PINNUM(port, pin) ((port) * 32 + (pin))
#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 02)

#define STACK_SIZE configMINIMAL_STACK_SIZE * 2

StackType_t can_read_task_stack[STACK_SIZE];
StaticTask_t can_read_task_taskdef;

StackType_t can_write_task_stack[STACK_SIZE];
StaticTask_t can_write_task_taskdef;
TaskHandle_t can_write_task_handle;

StackType_t cdc_task_stack[STACK_SIZE];
StaticTask_t cdc_task_taskdef;

StackType_t usbd_stack[STACK_SIZE];
StaticTask_t usbd_taskdef;

StackType_t idle_task_stack[128];
StaticTask_t idle_task_taskdef;



void gpiote_irq_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

/*------------- MAIN -------------*/
int main(void) {

  board_init();
  tusb_init();
  uint8_t code = 1;

  nrfx_gpiote_init(1);

  nrf_gpio_cfg_input(BUTTON_PIN, NRF_GPIO_PIN_PULLUP);

  nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
  config.pull = NRF_GPIO_PIN_PULLUP;

  NVIC_SetPriority(GPIOTE_IRQn, 3);

  // nrfx_gpiote_in_init allocates a channel
  nrfx_err_t err_code =
      nrfx_gpiote_in_init(BUTTON_PIN, &config, gpiote_irq_handler);
  nrfx_gpiote_trigger_enable(BUTTON_PIN, true);


  // create task
  xTaskCreateStatic(idle_task, "idle", 128, NULL, configMAX_PRIORITIES - 10,
                    idle_task_stack, &idle_task_taskdef);

  xTaskCreateStatic(can_read_task, "can_read", STACK_SIZE, NULL, configMAX_PRIORITIES-2,
                    can_read_task_stack, &can_read_task_taskdef);

  can_write_task_handle = xTaskCreateStatic(can_write_task, "can_write", STACK_SIZE, NULL, configMAX_PRIORITIES-2,
                    can_write_task_stack, &can_write_task_taskdef);

  xTaskCreateStatic(cdc_task, "cdc", STACK_SIZE, NULL,
                    configMAX_PRIORITIES - 2, cdc_task_stack, &cdc_task_taskdef);
                    
  xTaskCreateStatic(usbd_task, "usbd", STACK_SIZE, NULL,
                    configMAX_PRIORITIES - 1, usbd_stack, &usbd_taskdef);



  vTaskStartScheduler();

  // scheduler started, this is never reached
  while (1) {
    __WFE(); // Wait for event (low power idle)
  }
}


void gpiote_irq_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  
  xTaskNotifyFromISR(can_write_task_handle, 0, eNoAction,
                &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}