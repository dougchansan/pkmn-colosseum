/* Score-only text partition; not evidence of a retail TU boundary. */
#include "dolphin/types.h"

extern void* GSresGetResource(u32 archive, u32 kind);

u32 dbgMenuColisionDisp(s32 key)
{
    u8* flags = GSresGetResource(0, 2);

    if (flags == NULL) {
        return 0;
    }

    switch (key) {
    case 0xB8:
        if (flags[0] != 0) {
            flags[0] = 0;
        } else {
            flags[0] = 1;
        }
        break;
    case 0xB9:
        if (flags[1] != 0) {
            flags[1] = 0;
        } else {
            flags[1] = 1;
        }
        break;
    }
    return 0;
}
