/** Exact standalone owner for 0x80163050 - 0x801630E4. */
#include "dolphin/types.h"

typedef struct MusyXAramCommand {
    u32 field_00;
    u32 packed_size;
} MusyXAramCommand;

extern u32 fn_80163810(void* dest, u32 size);

void fn_80163050(u32** source, u32* out) {
    MusyXAramCommand* command = (MusyXAramCommand*)*source;
    u32 packed = command->packed_size;
    u32 type = packed >> 24;
    u32 size = packed & 0xFFFFFF;

    switch (type) {
    case 0:
    case 1:
    case 4:
    case 5:
        size = ((size + 13) / 7 * 4) & ~7u;
        break;
    case 2:
        size <<= 1;
        break;
    }
    *out = fn_80163810((void*)*out, size);
}
