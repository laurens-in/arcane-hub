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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "nrfx_gpiote.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "bsp/board_api.h"
#include "tusb.h"

#include "mcp2515/mcp_can.h"
#include "arcane/arcane.h"

enum {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

#define _PINNUM(port, pin) ((port) * 32 + (pin))
#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 02)

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

static uint8_t led_status = 0;

#define STACK_SIZE configMINIMAL_STACK_SIZE * 2

void led_blinking_task(void);

static void can_read_task(void *params);
StackType_t can_read_task_stack[STACK_SIZE];
StaticTask_t can_read_task_taskdef;

static void can_write_task(void *params);
StackType_t can_write_task_stack[STACK_SIZE];
StaticTask_t can_write_task_taskdef;
TaskHandle_t can_write_task_handle;

static void cdc_task(void *params);
StackType_t cdc_task_stack[STACK_SIZE];
StaticTask_t cdc_task_taskdef;

static void usbd_task(void *params);
StackType_t usbd_stack[STACK_SIZE];
StaticTask_t usbd_taskdef;

static void idle_task(void *params);
StackType_t idle_task_stack[128];
StaticTask_t idle_task_taskdef;

void gpiote_irq_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

/*------------- MAIN -------------*/
int main(void) {

  board_init();
  tusb_init();

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

void can_read_task(void *param) {

  // must be done after scheduler is started
  mcp_spi_init();
  mcp_can_begin(CAN_1000KBPS, MCP_16MHz);

  for (;;) {
    if (CAN_MSGAVAIL == mcp_can_check_receive()) {
      uint32_t can_id;
      uint8_t ext;
      uint8_t len;
      uint8_t buf[8];

      mcp_can_read_msg(&can_id, &ext, &len, buf);

      uint8_t code = get_func_code(can_id, ext);

      if (code == FUNC_MIDI0 || code == FUNC_MIDI1 || code == FUNC_MIDI2) {
        uint8_t note[3] = { buf[0] , buf[1], buf[2] };
        tud_midi_stream_write(0, note, 3);
      }
    }
  }
}

void can_write_task(void *param) {
  for (;;) {
    xTaskNotifyWait(pdFALSE, ULONG_MAX, NULL, portMAX_DELAY);
      // Toggle LED when the button is pressed
    led_status = led_status == 0 ? 1 : 0;
    uint8_t tmp_buffer[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t code = mcp_can_send_msg(FUNC_CFGW || 0x01, 0, 4, tmp_buffer);
    board_led_write(led_status);

    vTaskDelay(20);
  }
}


static void usbd_task(void *pvParameters) {

    // init device stack on configured roothub port
    // This should be called after scheduler/kernel is started.
    // Otherwise it could cause kernel issue since USB IRQ handler does use RTOS queue API.
    tud_init(BOARD_TUD_RHPORT);

    // RTOS forever loop
    for (;;) {
        // put this thread to waiting state until there is new events
        tud_task();
    }
}

void idle_task(void *param) {
  (void)param;

  // RTOS forever loop
  while (1) {
    // __WFE(); // Wait for event (low power idle)
    vTaskDelay(20);
  }
}

void gpiote_irq_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  
  xTaskNotifyFromISR(can_write_task_handle, 0, eNoAction,
                &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


// CDC

// send characters to both CDC and WebUSB
void echo_all(uint8_t buf[], uint32_t count)
{
  // echo to cdc
  if ( tud_cdc_connected() )
  {
    for(uint32_t i=0; i<count; i++)
    {
      tud_cdc_write_char(buf[i]);

      if ( buf[i] == '\r' ) tud_cdc_write_char('\n');
    }
    tud_cdc_write_flush();
  }
}


static void cdc_task(void *params)
{
  for(;;){
    if ( tud_cdc_connected() )
    {
      // connected and there are data available
      if ( tud_cdc_available() )
      {
        // [read/write, node_id, param_id, length, data 0-8]
        uint8_t buf[12];

        uint32_t count = tud_cdc_read(buf, sizeof(buf));

        uint8_t data[buf[4]];

        memcpy(data, buf, buf[4]);

        uint8_t code = mcp_can_send_msg(FUNC_CFGW || 0x01, 0, buf[4], data);
        

        // echo back to both cdc
        // echo_all(buf, count);
      }
    }
  }
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
  (void) itf;

  // connected
  if ( dtr && rts )
  {
    // print initial message when connected
    tud_cdc_write_str("\r\nWelcome to the ARCANE cli interface!\r\n");
  }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf)
{
  (void) itf;
}