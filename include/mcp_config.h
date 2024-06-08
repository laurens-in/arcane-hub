#include "nrfx_gpiote.h"

#define SPI_SS_PIN					NRF_GPIO_PIN_MAP(1,8) // TODO: remove maybe
#define SPI_SCK_PIN					NRF_GPIO_PIN_MAP(0,14)
#define SPI_MOSI_PIN				NRF_GPIO_PIN_MAP(0,13)
#define SPI_MISO_PIN				NRF_GPIO_PIN_MAP(0,15)
#define BSP_LED_3 					NRF_GPIO_PIN_MAP(1,15)
#define MCP2515_PIN_INT			NRF_GPIO_PIN_MAP(0,7)