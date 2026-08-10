/* Score instrumentation only; not evidence of a retail TU boundary. */
#include "src/dolphin/thp/THP_range_801E1B54.c"

#define THP_PLAYER_DATA ((u8 *)lbl_8046AC60)

BOOL fn_801E449C(u8 *buffer)
{
    u8 *work;
    u8 *slot;
    u32 component;
    u32 videoSize;
    u32 roundedLuma;
    u32 roundedChroma;
    u32 offset;
    extern void DCInvalidateRange(void *addr, u32 nBytes);

    if (*(BOOL *)(THP_PLAYER_DATA + 0xA0) && THP_PLAYER_DATA[0xA4] == 0) {
    work = buffer;
    if (*(BOOL *)(THP_PLAYER_DATA + 0xB0)) {
        *(u8 **)(THP_PLAYER_DATA + 0xB4) = buffer;
        work += *(u32 *)(THP_PLAYER_DATA + 0x58);
    } else {
        for (component = 0, offset = 0; component < 10;
             component++, offset += 0xC) {
            u8 *entry = THP_PLAYER_DATA + offset;
            *(u8 **)(entry + 0xF0) = work;
            work += OSRoundUp32B(*(u32 *)(THP_PLAYER_DATA + 0x44));
        }
    }

    videoSize = *(u32 *)(THP_PLAYER_DATA + 0x80) *
                *(u32 *)(THP_PLAYER_DATA + 0x84);
    roundedLuma = OSRoundUp32B(videoSize);
    roundedChroma = OSRoundUp32B(videoSize >> 2);

    for (component = 0; component < 3; component++) {
        slot = THP_PLAYER_DATA + component * 0x10;
        *(u8 **)(slot + 0x168) = work;
        DCInvalidateRange(work, roundedLuma);
        work += roundedLuma;

        *(u8 **)(slot + 0x16C) = work;
        DCInvalidateRange(work, roundedChroma);
        work += roundedChroma;

        *(u8 **)(slot + 0x170) = work;
        DCInvalidateRange(work, roundedChroma);
        work += roundedChroma;
    }

    if (THP_PLAYER_DATA[0xA7]) {
        u32 sampleStride;

        for (component = 0, offset = 0; component < 3;
             component++, offset += 0xC) {
            u8 *slot = THP_PLAYER_DATA + offset;
            *(u8 **)(slot + 0x198) = work;
            *(u8 **)(slot + 0x19C) = work;
            *(u32 *)(slot + 0x1A0) = 0;
            work += OSRoundUp32B(*(u32 *)(THP_PLAYER_DATA + 0x48) << 2);
        }

        sampleStride = OSRoundUp32B(
            *(u32 *)(THP_PLAYER_DATA + 0x90) * 40 / 500);
        lbl_8047B470 = (s16 *)work;
        work += sampleStride;
        if (*(u32 *)(THP_PLAYER_DATA + 0x8C) == 2) {
            lbl_8047B474 = (s16 *)work;
            work += sampleStride;
        }
    }

    *(u8 **)(THP_PLAYER_DATA + 0x9C) = work;
    return TRUE;
    }
    return FALSE;
}

#undef THP_PLAYER_DATA
