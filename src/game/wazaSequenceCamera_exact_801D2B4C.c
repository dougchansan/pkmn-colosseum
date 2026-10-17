/**
 * @file wazaSequenceCamera_exact_801D2B4C.c
 * @brief wazaSequenceCameraGetPattern, 0x801D2B4C - 0x801D2C6C, with its
 * pooled 0.0f (.sdata2 0x8047E1D8).
 *
 * Function-boundary carve of the waza camera TU (see wazaSequenceCamera.c):
 * pick a camera pattern from the short (8-entry) or long (13-entry) table.
 * Flag bits 0x20/0x40/0x80 select fixed entries; otherwise a random frame
 * is placed on the tables' cumulative durations, and past the end a random
 * entry is taken. GC/1.3 -O4,p like the TU, no pragmas; the body is the
 * TU's.
 *
 * Data: the pattern tables lbl_80371F60/lbl_803721C0 stay extern. The
 * function's only pooled constant is the 0.0f the duration sum starts from,
 * the first literal of the TU's pool at 0x8047E1D8; no other function
 * references it (0x8047E1DC-0x8047E1E0 is zero padding before the next
 * pool, left as a gap). It was the head of the linked data unit
 * battle_sdata2_8047E190.c, which now ends at 0x8047E1D8; the rest of that
 * pool (0x8047E1E0-0x8047E2E0) belongs to wazaSequenceCamera_file_801D2D28.c.
 */
#include "dolphin/types.h"

typedef struct WazaCameraPattern {
    f32 duration;
    u8 data[0x48];
} WazaCameraPattern;

extern f32 GSrandf(void);
extern u32 _fadeEffectGetRandom__FUl(u32);
extern WazaCameraPattern lbl_80371F60[];
extern WazaCameraPattern lbl_803721C0[];

void* wazaSequenceCameraGetPattern__Fbi(u8 shortTable, s32 flags)
{
    WazaCameraPattern* table;
    s32 count;
    s32 i;
    f32 frame = GSrandf();
    f32 end = 0.0f;

    if (shortTable != 0) {
        table = lbl_80371F60;
        count = 8;
        if (flags & 0x20) {
            return &table[4];
        }
        if (flags & 0x40) {
            return &table[5];
        }
        if (flags & 0x80) {
            return &table[6];
        }
    } else {
        table = lbl_803721C0;
        count = 13;
        if (flags & 0x20) {
            return &table[9];
        }
        if (flags & 0x40) {
            return &table[10];
        }
        if (flags & 0x80) {
            return &table[11];
        }
    }

    for (i = 0; i < count; i++, table++) {
        end += table->duration;
        if (frame < end) {
            return table;
        }
    }

    if (shortTable != 0) {
        return &lbl_80371F60[_fadeEffectGetRandom__FUl(8)];
    }
    return &lbl_803721C0[_fadeEffectGetRandom__FUl(13)];
}
