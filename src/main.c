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

#include "nrfx_gpiote.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "bsp/board_api.h"
#include "tusb.h"

#include "mcp2515/mcp_can.h"

/* This MIDI example send sequence of note (on/off) repeatedly. To test on PC,
 * you need to install synth software and midi connection management software.
 * On
 * - Linux (Ubuntu): install qsynth, qjackctl. Then connect TinyUSB output port
 * to FLUID Synth input port
 * - Windows: install MIDI-OX
 * - MacOS: SimpleSynth
 */

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

#define _PINNUM(port, pin) ((port) * 32 + (pin))
#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 02)

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

static uint8_t led_status = 0;

void led_blinking_task(void);

static void can_task(void *params);
StaticTask_t can_task_taskdef;
StackType_t can_task_stack[2048];

void midi_task(void);
static void idle_task(void *params);
StackType_t idle_task_stack[128];
StaticTask_t idle_task_taskdef;

void gpiote_irq_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

/*------------- MAIN -------------*/
int main(void) {

  board_init();

  nrfx_gpiote_init(1);

  nrf_gpio_cfg_input(BUTTON_PIN, NRF_GPIO_PIN_PULLUP);

  nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
  config.pull = NRF_GPIO_PIN_PULLUP;

  NVIC_SetPriority(GPIOTE_IRQn, 3);

  // nrfx_gpiote_in_init allocates a channel
  nrfx_err_t err_code =
      nrfx_gpiote_in_init(BUTTON_PIN, &config, gpiote_irq_handler);
  nrfx_gpiote_trigger_enable(BUTTON_PIN, true);

  // init device stack on configured roothub port
  tud_init(BOARD_TUD_RHPORT);

  // init SPI / CAN

  mcp_spi_init();
  mcp_can_begin(CAN_500KBPS, MCP_8MHz);

  // create task
  xTaskCreateStatic(idle_task, "idle", 128, NULL, configMAX_PRIORITIES - 10,
                    idle_task_stack, &idle_task_taskdef);

  xTaskCreateStatic(can_task, "can", 2048, NULL, configMAX_PRIORITIES-1,
                    can_task_stack, &can_task_taskdef);

  vTaskStartScheduler();

  // scheduler started, this is never reached
  while (1) {
    __WFE(); // Wait for event (low power idle)
  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) { blink_interval_ms = BLINK_MOUNTED; }

// Invoked when device is unmounted
void tud_umount_cb(void) { blink_interval_ms = BLINK_NOT_MOUNTED; }

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
  (void)remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

void led_blinking_task(void) {
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // Blink every interval ms
  if (board_millis() - start_ms < blink_interval_ms)
    return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}

void can_task(void *param) {
  while (1) {
    uint8_t tmp_buffer[4] = {0x11, 0x22, 0x33, 0x44};

    mcp_can_send_msg(0x1, 0, 4, &tmp_buffer);

    if (CAN_MSGAVAIL == mcp_can_check_receive()) {
      uint32_t can_id;
      uint8_t buf[16];
      uint8_t len;
      mcp_can_read_msg(&can_id, &len, buf);
    }
  }
}

void idle_task(void *param) {
  (void)param;

  // RTOS forever loop
  while (1) {
    __WFE(); // Wait for event (low power idle)
  }
}

void gpiote_irq_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
  // Toggle LED when the button is pressed
  led_status = led_status == 0 ? 1 : 0;
  board_led_write(led_status);
}