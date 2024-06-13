#ifndef _MCP2515_H_
#define _MCP2515_H_

#include "mcp_can_dfs.h"
#include "mcp_config.h"
#include "nrfx_gpiote.h"

#define MAX_CHAR_IN_MESSAGE 8

/**
 * @brief Struct representing a CAN message.
 */
typedef struct {
  uint8_t m_ext_flag;                  /**< Flag indicating if the message uses 11-bit (standard) or 29-bit (extended) identifier. */
  uint32_t m_id;                       /**< CAN identifier for the message. */
  uint8_t m_len;                       /**< Length of the data in the message. */
  uint8_t m_data[MAX_CHAR_IN_MESSAGE]; /**< Data payload of the message. */
  uint8_t m_cs;                        /**< Chip select pin for the CAN controller. */
} mcp_can_t;

/**
 * @brief Set the chip select pin for the CAN controller.
 *
 * @param cs The chip select pin to use.
 */
void mcp_can_setcs(uint8_t cs);

/**
 * @brief Initialize the SPI interface for the CAN controller.
 */
void mcp_spi_init(void);

/**
 * @brief Initialize the CAN controller with specified speed and clock settings.
 *
 * @param speedset The speed setting for the CAN controller.
 * @param clockset The clock setting for the CAN controller.
 * @return Status code indicating success or failure of the operation.
 */
uint8_t mcp_can_begin(uint8_t speedset, const uint8_t clockset);

/**
 * @brief Send a CAN message.
 *
 * @param id The identifier for the CAN message.
 * @param ext Flag indicating if the message uses an extended identifier.
 * @param len The length of the data in the message.
 * @param buf The data payload of the message.
 * @return Status code indicating success or failure of the operation.
 */
uint8_t mcp_can_send_msg(uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf);

/**
 * @brief Read a CAN message.
 *
 * @param id Pointer to store the identifier of the received CAN message.
 * @param ext Pointer to store the flag indicating if the message uses an extended identifier.
 * @param len Pointer to store the length of the data in the received message.
 * @param buf Buffer to store the data payload of the received message.
 * @return Status code indicating success or failure of the operation.
 */
uint8_t mcp_can_read_msg(uint32_t *id, uint8_t *ext, uint8_t *len, uint8_t buf[8]);

/**
 * @brief Check if a CAN message has been received.
 *
 * @return Status code indicating if a message has been received.
 */
uint8_t mcp_can_check_receive(void);

/**
 * @brief Check for errors in the CAN controller.
 *
 * @return Status code indicating if an error has occurred.
 */
uint8_t mcp_can_check_error(void);

#endif