#ifndef ARCANE_H
#define ARCANE_H

#include <stdint.h>

/// @def FUNC_NMT
/// @brief Function code for NMT.
#define FUNC_NMT     0
/// @def FUNC_MIDI0
/// @brief Function code for MIDI0.
#define FUNC_MIDI0   1
/// @def FUNC_RAW0
/// @brief Function code for RAW0.
#define FUNC_RAW0    2
/// @def FUNC_MIDI1
/// @brief Function code for MIDI1.
#define FUNC_MIDI1   3
/// @def FUNC_RAW1
/// @brief Function code for RAW1.
#define FUNC_RAW1    4
/// @def FUNC_MIDI2
/// @brief Function code for MIDI2.
#define FUNC_MIDI2   5
/// @def FUNC_RAW2
/// @brief Function code for RAW2.
#define FUNC_RAW2    6
/// @def FUNC_CFGW
/// @brief Function code for CFGW.
#define FUNC_CFGW    7
/// @def FUNC_CFGR
/// @brief Function code for CFGR.
#define FUNC_CFGR    8

/// @def NMT_OFF
/// @brief NMT state off.
#define NMT_OFF      0     
/// @def NMT_ON
/// @brief NMT state on.
#define NMT_ON       1

/**
 * @brief Get the function code from a given ID.
 * 
 * This function extracts the function code from the provided ARCANE id.
 *
 * @param id The ARCANE id.
 * @return The extracted function code (0-15).
 */
uint8_t get_func_code(uint32_t id);

/**
 * @brief Get the Arcane ID from a function code and node ID.
 * 
 * This function combines a function code and a node ID into an 11-bit ARCANE id.
 *
 * @param func_code The function code (0-15).
 * @param node_id The node ID (0-127).
 * @return The combined Arcane ID.
 */
uint16_t get_arcane_id(uint8_t func_code, uint8_t node_id);

#endif /* ARCANE_H */