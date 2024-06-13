#ifndef BSP_H
#define BSP_H

#include "nrfx_gpiote.h"

/** @brief Pin number for the user button */
#define BUTTON_PIN      NRF_GPIO_PIN_MAP(1, 02)

/** @brief Pin number for the SPI sub select */
#define SPI_SS_PIN      NRF_GPIO_PIN_MAP(1, 8)

/** @brief Pin number for the SPI clock */
#define SPI_SCK_PIN     NRF_GPIO_PIN_MAP(0, 14)

/** @brief Pin number for the SPI main out sub in */
#define SPI_MOSI_PIN    NRF_GPIO_PIN_MAP(0, 13)

/** @brief Pin number for the SPI main in sub out */
#define SPI_MISO_PIN    NRF_GPIO_PIN_MAP(0, 15)

/** @brief Pin number for the user LED */
#define LED_PIN         NRF_GPIO_PIN_MAP(1, 15)

/** @brief Pin number for the MCP interrupt */
#define MCP_INT         NRF_GPIO_PIN_MAP(0, 7)

/**
 * @brief General initialization function for the board support package.
 *
 * This function initializes the GPIOTE module if not already initialized,
 * and also calls the initialization routines for USB and the user button.
 */
void init(void);

/**
 * @brief Write a value to the user LED.
 *
 * @param state Boolean value indicating the desired state of the LED.
 *              - true: Turn the LED on.
 *              - false: Turn the LED off.
 */
void led_write(bool state);

#endif /* BSP_H */