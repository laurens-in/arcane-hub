// ARCANE function codes
#define FUNC_NMT     0b0000
#define FUNC_MIDI0   0b0001
#define FUNC_MIDI1   0b0010
#define FUNC_MIDI2   0b0011
#define FUNC_CFGW    0b0100
#define FUNC_CFGR    0b0101

#define NMT_OFF      0b0000      
#define NMT_ON       0b0001

uint8_t get_func_code(uint32_t id, uint8_t ext);
