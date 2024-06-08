#ifndef NRFX_CONFIG_H__
#define NRFX_CONFIG_H__

#define NRFX_POWER_ENABLED   1
#define NRFX_POWER_DEFAULT_CONFIG_IRQ_PRIORITY  7

#define NRFX_CLOCK_ENABLED   0
#define NRFX_GPIOTE_ENABLED  1

#define NRFX_UARTE_ENABLED   1
#define NRFX_UARTE0_ENABLED  1

#define NRFX_SPIM_ENABLED    1
#define NRFX_SPIM1_ENABLED   1 // use SPI1 since nrf5340 share uart with spi

#define NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY 2

#define NRFX_PRS_ENABLED     0
#define NRFX_USBREG_ENABLED  1

#include <templates/nrfx_config_nrf52840.h>

#endif // NRFX_CONFIG_H__
