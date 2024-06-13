#include "tusb.h"

/**
 * @brief Callback invoked when the CDC line state changes (e.g., connected/disconnected).
 *
 * This function is called automatically when the line state of the CDC interface
 * changes, such as when the device is connected or disconnected. When connected,
 * it sends a welcome message to the connected device.
 *
 */
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
  (void)itf;

  // connected
  if (dtr && rts) {
    // print initial message when connected
    tud_cdc_write_str("\r\nWelcome to the ARCANE cli interface!\r\n");
  }
}

/**
 * @brief Callback invoked when the CDC interface receives data from the host.
 *
 * This function is called automatically when data is received from the host
 * on the CDC interface. The interface number is provided as a parameter.
 *
 */
void tud_cdc_rx_cb(uint8_t itf) { (void)itf; }