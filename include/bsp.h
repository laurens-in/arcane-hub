#ifndef BSP_H
#define BSP_H

/**
 * @defgroup bsp bsp.h
 * @{
 * @ingroup arcane
 *
 * @brief This file contains pin mappings and initialization functions.
 */

#include "nrfx_gpiote.h"

#define BUTTON_PIN      NRF_GPIO_PIN_MAP(1, 02)
#define SPI_SS_PIN			NRF_GPIO_PIN_MAP(1,8)
#define SPI_SCK_PIN			NRF_GPIO_PIN_MAP(0,14)
#define SPI_MOSI_PIN		NRF_GPIO_PIN_MAP(0,13)
#define SPI_MISO_PIN		NRF_GPIO_PIN_MAP(0,15)
#define LED_PIN					NRF_GPIO_PIN_MAP(1,15)
#define MCP_INT			    NRF_GPIO_PIN_MAP(0,7)

void init(void);
void led_write(bool state);

#endif /* BSP_H */