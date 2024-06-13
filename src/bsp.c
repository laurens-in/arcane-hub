#include "bsp.h"
#include "tasks.h"

#include "bsp/board_api.h"
#include "tusb.h"

void usb_init(void);
void button_init(void);

void init(void) {
  if (nrfx_gpiote_is_init() == false) nrfx_gpiote_init(3);
  usb_init();
  button_init();
}

void usb_init(void) {
  // configure UART etc.
  board_init();

  // initialize tinyUSB
  tusb_init();
}

void button_init(void) {
  nrf_gpio_cfg_input(BUTTON_PIN, NRF_GPIO_PIN_PULLUP);

  nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
  config.pull = NRF_GPIO_PIN_PULLUP;

  nrfx_gpiote_in_init(BUTTON_PIN, &config, button_irq_handler);
  nrfx_gpiote_trigger_enable(BUTTON_PIN, true);
}

void led_write(bool state) {
  nrf_gpio_pin_write(LED_PIN, state);
}