/** Exact standalone owner for 0x80163104 - 0x80163188. */
#include "dolphin/types.h"

typedef struct MusyXAramCommand {
    u32 field_00;
    u32 packed_size;
} MusyXAramCommand;

extern void fn_80163BCC(u8* dest, u32 size);

void fn_80163104(u8* source, u8* dest) {
    MusyXAramCommand* command = (MusyXAramCommand*)source;
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
    fn_80163BCC(dest, size);
}
