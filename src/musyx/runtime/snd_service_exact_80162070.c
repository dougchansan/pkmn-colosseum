/**
 * @file snd_service_exact_80162070.c
 * @brief MusyX runtime utility functions.
 */

#include "dolphin/types.h"
#include "game/people/people.h"

extern u32 lbl_80478BF0;
extern s16 lbl_80369D20[];
extern u32 synthGetTicksPerSecond(u32 voice);

typedef s32 (*PeopleCmpFn)(u8* a, u8* b);

/* sndRand */
u16 sndRand(void)
{
    lbl_80478BF0 *= 0xA8351D63;
    return lbl_80478BF0 >> 6;
}

s16 sndSin(u16 angle)
{
    angle &= 0xFFF;
    if (angle < 0x400) {
        return lbl_80369D20[angle];
    }
    if (angle < 0x800) {
        return lbl_80369D20[0x3FF - (angle & 0x3FF)];
    }
    if (angle < 0xC00) {
        return -lbl_80369D20[angle & 0x3FF];
    }
    return -lbl_80369D20[0x3FF - (angle & 0x3FF)];
}

void* sndBSearch(u8* key, u8* base, s32 count, u32 size, PeopleCmpFn cmp)
{
    s32 lo;
    s32 hi;
    s32 mid;
    s32 result;
    void* element;

    if (count != 0) {
        lo = 1;
        hi = count;
        do {
            mid = (lo + hi) >> 1;
            element = (u8*)base + size * (mid - 1);
            result = cmp(key, element);
            if (result == 0) {
                return element;
            }
            if (result < 0) {
                hi = mid - 1;
            } else {
                lo = mid + 1;
            }
        } while (lo <= hi);
    }
    return NULL;
}

/* sndConvertMs */
void sndConvertMs(u32* time)
{
    *time *= 0x100;
}

void sndConvertTicks(u32* time, u32 voice)
{
    *time = ((*time << 16) / synthGetTicksPerSecond(voice) * 0x3E8) >> 5;
}

/* sndConvert2Ms */
u32 sndConvert2Ms(u32 time)
{
    return time / 0x100;
}
