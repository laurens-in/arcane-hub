#ifndef MCP_CONFIG_H
#define MCP_CONFIG_H

#include "nrfx_gpiote.h"

#define SPI_SS_PIN					NRF_GPIO_PIN_MAP(1,8)
#define SPI_SCK_PIN					NRF_GPIO_PIN_MAP(0,14)
#define SPI_MOSI_PIN				NRF_GPIO_PIN_MAP(0,13)
#define SPI_MISO_PIN				NRF_GPIO_PIN_MAP(0,15)
#define MCP_LED_INT					NRF_GPIO_PIN_MAP(1,15)
#define MCP_PIN_INT			    NRF_GPIO_PIN_MAP(0,7)

#endif /* MCP_CONFIG_H */