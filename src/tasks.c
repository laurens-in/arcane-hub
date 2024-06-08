#include <limits.h>

#include "tusb.h"

// #include "FreeRTOS.h"
// #include "queue.h"
// #include "semphr.h"
// #include "task.h"
// #include "timers.h"

#include "mcp_can.h"
#include "arcane.h"
#include "tasks.h"

#include "bsp/board_api.h" // TODO: remove

static uint8_t led_status = 0; // TODO: remove

void cdc_task(void *params)
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
      
      }
    }
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


void usbd_task(void *pvParameters) {

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
