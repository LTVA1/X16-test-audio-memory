
#define PSG_CHN_REGISTERS 4
#define PSG_NUM_CHANNELS 16

#include <stdint.h>

typedef struct
{
    uint8_t registers[PSG_NUM_CHANNELS * PSG_CHN_REGISTERS];
} VERA_psg;

VERA_psg VERA_PSG;