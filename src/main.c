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

#include "FreeRTOS.h"
#include "task.h"

#include "bsp.h"
#include "tasks.h"

#define STACK_SIZE configMINIMAL_STACK_SIZE * 2

StackType_t can_read_task_stack[STACK_SIZE];
StaticTask_t can_read_task_taskdef;

StackType_t cycle_config_task_stack[STACK_SIZE];
StaticTask_t cycle_config_task_taskdef;
TaskHandle_t cycle_config_task_handle;

StackType_t cdc_task_stack[STACK_SIZE];
StaticTask_t cdc_task_taskdef;

StackType_t usbd_stack[STACK_SIZE];
StaticTask_t usbd_taskdef;

StackType_t idle_task_stack[128];
StaticTask_t idle_task_taskdef;

/*------------- MAIN -------------*/
int main(void) {

  init();

  xTaskCreateStatic(idle_task, "idle", 128, NULL, configMAX_PRIORITIES - 10,
                    idle_task_stack, &idle_task_taskdef);

  xTaskCreateStatic(can_read_task, "can_read", STACK_SIZE, NULL,
                    configMAX_PRIORITIES - 2, can_read_task_stack,
                    &can_read_task_taskdef);

  cycle_config_task_handle = xTaskCreateStatic(
      cycle_config_task, "cycle_config", STACK_SIZE, NULL, configMAX_PRIORITIES - 2,
      cycle_config_task_stack, &cycle_config_task_taskdef);

  xTaskCreateStatic(cdc_task, "cdc", STACK_SIZE, NULL, configMAX_PRIORITIES - 2,
                    cdc_task_stack, &cdc_task_taskdef);

  xTaskCreateStatic(usbd_task, "usbd", STACK_SIZE, NULL,
                    configMAX_PRIORITIES - 1, usbd_stack, &usbd_taskdef);


  vTaskStartScheduler();

  // scheduler started, this is never reached
  while (1) {
    __WFE(); // Wait for event (low power idle)
  }
}