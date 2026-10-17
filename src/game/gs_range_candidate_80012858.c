#define GS_RANGE_CANDIDATE_80012858
#include "src/game/gs_range_80011EA4.c"

/*
 * Score-isolation candidate: the inherited semantic recovery lived in the
 * broad gs_event_exec scratch TU, while this object owns the retail range.
 */
extern void* windowGetFreeWork(void*);
extern void* windowAllocMemory(void*, s32);
extern void* windowGetAllocPtr(void*);
extern void* windowSearchID(s32);
extern void fn_80103F74(void*, s32, s32);
extern void soundStopSE(s32);

s32 fn_800129A8(u8* ctx)
{
    u8* work;
    s32 kind2;
    s32 id2;
    void* window2;
    s32 id;
    s32 kind;
    void* buffer;
    void* window;

    work = windowGetFreeWork(ctx);
    if ((s8)ctx[1] == 0) {
        buffer = windowAllocMemory(ctx, 0x30);
        if (buffer != NULL) {
            memcpy(buffer, *(void**)(ctx + 0x60), 0x30);
        }
    }
    windowGetAllocPtr(ctx);
    id = *(s32*)(ctx + 4);
    if ((s8)ctx[1] == 0) {
        kind = 0;
        window = windowSearchID(id);
        if (window != NULL) {
            switch (id) {
            case 0x45:
            case 0x46:
            case 0x49:
                kind = 0x538;
                break;
            case 0x47:
            case 0x48:
            case 0x4A:
                kind = 0x540;
                break;
            }
            fn_80103F74(window, kind, 0);
        }

        id2 = *(s32*)(ctx + 4);
        kind2 = 0;
        window2 = windowSearchID(id2);
        if (window2 != NULL) {
            switch (id2) {
            case 0x45:
            case 0x46:
            case 0x49:
                kind2 = 0x539;
                break;
            case 0x47:
            case 0x48:
            case 0x4A:
                kind2 = 0x541;
                break;
            }
            fn_80103F74(window2, kind2, 0);
        }
    }

    if (*(s16*)(work + 2) != 0) {
        (*(s16*)(work + 4))++;
        if (*(s16*)(work + 4) > *(s16*)(work + 2)) {
            *(s16*)(work + 2) = 0;
        }
    }
    if (*(s16*)(work + 0xC) != 0) {
        (*(s16*)(work + 0xE))++;
        if (*(s16*)(work + 0xE) > *(s16*)(work + 0xC)) {
            *(s16*)(work + 0xC) = 0;
            soundStopSE(0x4D0);
        }
    }
    (*(u16*)(work + 6))++;
    *(u16*)(work + 6) %= 1200;
    return 0;
}

s32 fn_80012B94(u8* ctx)
{
    extern s32 windowGetParam(u8*, s32);
    extern u8 menuDataBiosGetType(s32);
    extern s32 GSmsgGetRect(s32);
    extern void windowDrawPanel(s32, s32, s32, s32);
    extern void fn_8001E644(s32, s32, s32, s32, u8);
    extern void GSmsgPrintColor(s32, s32, s32, s32);
    extern void windowDrawSprite(s32, s16, u8*, s32, s32);
    u8* iter;
    u8* cursor;
    s32 delay;
    s32 j;
    u8* values;
    s32 i;
    s32 range;
    s32 count;
    u8 command;
    s32 totalWidth;
    s32 maxHeight;
    s32 position;
    s32 capacity;

    totalWidth = 0;
    maxHeight = 0;
    command = (u8)windowGetParam(ctx, 0);
    values = windowGetAllocPtr(ctx);
    count = (s8)windowGetParam(ctx, 2);
    capacity = menuDataBiosGetType(*(s32*)(ctx + 4));
    if (count > capacity) {
        count = capacity;
    }

    iter = values;
    for (i = 0; i < count; iter += 4, i++) {
        range = GSmsgGetRect(*(s32*)iter);
        if (maxHeight < (s32)((u32)range >> 16)) {
            maxHeight = (u32)range >> 16;
        }
        totalWidth += (range & 0xFFFF) + 2;
    }

    switch (command) {
    case 0:
    case 1:
        fn_8001E644(0, 0, maxHeight + 0x20, totalWidth, ctx[0x8B]);
        break;
    case 0x7F:
        windowDrawPanel(0, 0, maxHeight + 0x20, totalWidth);
        break;
    }

    position = 1;
    cursor = values;
    for (j = 0; j < count; cursor += 4, j++) {
        if (*(u32*)cursor != 0) {
            range = GSmsgGetRect(*(s32*)cursor);
            delay = (range & 0xFFFF) + 2;
            GSmsgPrintColor(0x20, position, -1, *(s32*)cursor);
        } else {
            delay = 0x14;
        }
        if ((s8)ctx[0x95] == j) {
            windowDrawSprite(0x20, (s16)position, ctx, 0x157, 0);
        }
        position += delay;
    }
    return 0;
}
