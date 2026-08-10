/* Score instrumentation only; not evidence of a retail TU boundary. */
#include "src/dolphin/thp/THP_range_801E1B54.c"

BOOL fn_801E449C(u8 *buffer)
{
    u8 *base = (u8 *)lbl_8046AC60;
    u8 *work = buffer;
    u32 component;
    u32 videoSize;
    u32 roundedLuma;
    u32 roundedChroma;
    extern void DCInvalidateRange(void *addr, u32 nBytes);

    if (!*(BOOL *)(base + 0xA0) || base[0xA4] != 0) {
        return FALSE;
    }

    if (*(BOOL *)(base + 0xB0)) {
        *(u8 **)(base + 0xB4) = buffer;
        work += *(u32 *)(base + 0x58);
    } else {
        for (component = 0; component < 10; component++) {
            u8 *entry = base + 0xF0 + component * 0xC;
            *(u8 **)entry = work;
            work += OSRoundUp32B(*(u32 *)(base + 0x44));
        }
    }

    videoSize = *(u32 *)(base + 0x80) * *(u32 *)(base + 0x84);
    roundedLuma = OSRoundUp32B(videoSize);
    roundedChroma = OSRoundUp32B(videoSize >> 2);

    for (component = 0; component < 3; component++) {
        u8 *slot = base + 0x168 + component * 0x10;
        *(u8 **)slot = work;
        DCInvalidateRange(work, roundedLuma);
        work += roundedLuma;

        *((u8 **)slot + 1) = work;
        DCInvalidateRange(work, roundedChroma);
        work += roundedChroma;

        *((u8 **)slot + 2) = work;
        DCInvalidateRange(work, roundedChroma);
        work += roundedChroma;
    }

    if (base[0xA7]) {
        u32 sampleStride;

        for (component = 0; component < 3; component++) {
            u8 *slot = base + 0x198 + component * 0xC;
            *(u8 **)slot = work;
            *((u8 **)slot + 1) = work;
            *(u32 *)(slot + 8) = 0;
            work += OSRoundUp32B(*(u32 *)(base + 0x48) << 2);
        }

        sampleStride = OSRoundUp32B(*(u32 *)(base + 0x90) * 40 / 500);
        lbl_8047B470 = (s16 *)work;
        work += sampleStride;
        if (*(u32 *)(base + 0x8C) == 2) {
            lbl_8047B474 = (s16 *)work;
            work += sampleStride;
        }
    }

    *(u8 **)(base + 0x9C) = work;
    return TRUE;
}
