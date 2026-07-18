/**
 * @file gs_gfx_range_800DB098.c
 * @brief Candidate GS render-engine prefix, 0x800DB098-0x800DB890.
 */

#include "dolphin/types.h"

extern void DCFlushRange(void* addr, u32 size);
extern u32 lbl_8047AA80;
extern void fn_800D6A80(u16, s32, u32*, u32*);
extern void jumptable_80315364(void);

void fn_800DB098(void) {
    u32 state;
    u32 capture;
    u8* cursor;
    u8* start;
    u32 used;

    state = lbl_8047AA80;
    capture = *(u32*)(state + 0x480);
    if (capture == 0) {
        return;
    }

    cursor = *(u8**)(state + 0x484);
    start = *(u8**)(capture + 0x4);
    if (cursor == 0 || start == 0) {
        return;
    }

    used = (u32)(cursor - start);
    if (used + 0x68 > *(u32*)(capture + 0x8)) {
        *(u8*)(capture + 0x1) = 1;
        return;
    }

    *(u32*)(state + 0x484) = (u32)cursor;
    *(u32*)(capture + 0x14) = used;
    DCFlushRange(start, used);
}

void fn_800DB758(u16 vertCount)
{
    u32 state;
    u32 obj;
    u8* p;
    u16* p16;

    state = lbl_8047AA80;
    if (*(s32*)(state + 0x488) == 7) {
        vertCount = (vertCount & 0x7FFF) << 1;
    }

    obj = *(u32*)(state + 0x480);
    fn_800D6A80(vertCount, *(u32*)(state + 0x488),
                (u32*)(obj + 0x10), (u32*)(obj + 0x14));

    state = lbl_8047AA80;
    switch (*(u32*)(state + 0x488)) {
        case 0:
            *(u8*)*(u32*)(state + 0x484) = 0xB8;
            break;
        case 1:
            *(u8*)*(u32*)(state + 0x484) = 0xA8;
            break;
        case 2:
            *(u8*)*(u32*)(state + 0x484) = 0xB0;
            break;
        case 3:
            *(u8*)*(u32*)(state + 0x484) = 0x90;
            break;
        case 4:
            *(u8*)*(u32*)(state + 0x484) = 0x98;
            break;
        case 5:
            *(u8*)*(u32*)(state + 0x484) = 0xA0;
            break;
        case 6:
            *(u8*)*(u32*)(state + 0x484) = 0x80;
            break;
        case 7:
            *(u8*)*(u32*)(state + 0x484) = 0x80;
            break;
    }

    state = lbl_8047AA80;
    p = *(u8**)(state + 0x484);
    *(u32*)(state + 0x484) = (u32)(p + 1);
    obj = *(u32*)(state + 0x480);
    *p = (u8)(*p | *(u32*)(*(u32*)(obj + 0xC) + 4));

    state = lbl_8047AA80;
    p16 = *(u16**)(state + 0x484);
    *p16++ = vertCount;
    state = lbl_8047AA80;
    *(u32*)(state + 0x484) = (u32)p16;
}
