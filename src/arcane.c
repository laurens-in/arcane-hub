#include "arcane.h"

uint8_t get_func_code(uint32_t id) {
  uint32_t shifted = id >> 7; // Shift right to bring to bits 3-0
  uint8_t code = shifted & 0xF; // Mask with 0xF to isolate bits 3-0

  return code;
}

uint16_t get_arcane_id(uint8_t func_code, uint8_t node_id) {
  func_code &= 0x0F; // Ensure func_code is within the range 0-15 (4 bits)
  node_id &= 0x7F; // Ensure node_id is within the range 0-127 (7 bits)

  uint16_t arcane_id = (func_code << 7) | node_id; // Combine func_code and node_id into an 11-bit value

  return arcane_id;
}