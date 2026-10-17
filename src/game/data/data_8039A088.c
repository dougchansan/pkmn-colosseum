#include "dolphin/types.h"

extern u8 wsCmd76[];

void* jumptable_8039A088[8] = {
    (void*)(wsCmd76 + 0x6C),
    (void*)(wsCmd76 + 0x78),
    (void*)(wsCmd76 + 0x25C),
    (void*)(wsCmd76 + 0x154),
    (void*)(wsCmd76 + 0x194),
    (void*)(wsCmd76 + 0x25C),
    (void*)(wsCmd76 + 0x1CC),
    (void*)(wsCmd76 + 0x24C),
};
