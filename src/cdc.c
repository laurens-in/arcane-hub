#include "tusb.h"

// send characters to CDC
void echo_all(uint8_t buf[], uint32_t count) {
  // echo to cdc
  if (tud_cdc_connected()) {
    for (uint32_t i = 0; i < count; i++) {
      tud_cdc_write_char(buf[i]);

      if (buf[i] == '\r')
        tud_cdc_write_char('\n');
    }
    tud_cdc_write_flush();
  }
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
  (void)itf;

  // connected
  if (dtr && rts) {
    // print initial message when connected
    tud_cdc_write_str("\r\nWelcome to the ARCANE cli interface!\r\n");
  }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf) { (void)itf; }