// ARCANE function codes
#define FUNC_NMT     0b00000000000
#define FUNC_MIDI0   0b00010000000
#define FUNC_MIDI1   0b00100000000
#define FUNC_MIDI2   0b00110000000
#define FUNC_CFGW    0b01000000000
#define FUNC_CFGR    0b01010000000

#define NMT_OFF      0b0000      
#define NMT_ON       0b0001

uint8_t get_func_code(uint32_t id, uint8_t ext);
