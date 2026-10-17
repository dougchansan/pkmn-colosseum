#include "dolphin/types.h"

extern u8 wsCmd48[];

void* jumptable_8039A0F0[8] = {
    (void*)(wsCmd48 + 0x244),
    (void*)(wsCmd48 + 0x20C),
    (void*)(wsCmd48 + 0x214),
    (void*)(wsCmd48 + 0x21C),
    (void*)(wsCmd48 + 0x224),
    (void*)(wsCmd48 + 0x22C),
    (void*)(wsCmd48 + 0x234),
    (void*)(wsCmd48 + 0x23C),
};

void* jumptable_8039A110[8] = {
    (void*)(wsCmd48 + 0xEC),
    (void*)(wsCmd48 + 0xB4),
    (void*)(wsCmd48 + 0xBC),
    (void*)(wsCmd48 + 0xC4),
    (void*)(wsCmd48 + 0xCC),
    (void*)(wsCmd48 + 0xD4),
    (void*)(wsCmd48 + 0xDC),
    (void*)(wsCmd48 + 0xE4),
};
