/**
 * @file menuNameEntry_exact_800265C0.c
 * @brief Name-entry state visibility callbacks.
 */
#include "dolphin/types.h"

extern u8 lbl_80266DD8[];

s32 menuNameEntryDrawItem5(void* owner, u8* draw)
{
    void* context;
    u8* base;
    s32* entry;

    context = *(void**)((u8*)owner + 0x60);
    base = lbl_80266DD8;
    base += *(s32*)((u8*)context + 0x1C) << 4;
    entry = (s32*)base;
    if (entry[1] != 7) {
        draw[0x67] = 0;
    } else {
        draw[0x67] = 0xFF;
    }
    return 0;
}

s32 menuNameEntryDrawItem6(void* owner, u8* draw)
{
    void* context;
    u8* base;
    s32* entry;

    context = *(void**)((u8*)owner + 0x60);
    base = lbl_80266DD8;
    base += *(s32*)((u8*)context + 0x1C) << 4;
    entry = (s32*)base;
    if (entry[1] != 8) {
        draw[0x67] = 0;
    } else {
        draw[0x67] = 0xFF;
    }
    return 0;
}
