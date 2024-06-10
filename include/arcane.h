#ifndef ARCANE_H
#define ARCANE_H

#include <stdint.h>

#define FUNC_NMT     0
#define FUNC_MIDI0   1
#define FUNC_RAW0    2
#define FUNC_MIDI1   3
#define FUNC_RAW1    4
#define FUNC_MIDI2   5
#define FUNC_RAW2    6
#define FUNC_CFGW    7
#define FUNC_CFGR    8

#define NMT_OFF      0     
#define NMT_ON       1

uint8_t get_func_code(uint32_t id);
uint16_t get_arcane_id(uint8_t func_code, uint8_t node_id);

#endif /* ARCANE_H */