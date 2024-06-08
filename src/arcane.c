#include "arcane.h"

uint8_t get_func_code(uint32_t id, uint8_t ext) {
  uint8_t code;
    
  if (ext == 0)
  {
    uint32_t shifted = id >> 7; // Shift right by 8 to bring bits 11-8 to bits 3-0
    code = shifted & 0xF;           // Mask with 0xF (binary 1111) to isolate bits 3-0
  } else {
    uint32_t shifted = id >> 25; // Shift right by 8 to bring bits 11-8 to bits 3-0
    code = shifted & 0xF;           // Mask with 0xF (binary 1111) to isolate bits 3-0
  }

  return code;

}