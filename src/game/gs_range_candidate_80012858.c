#include "src/game/gs_range_80011EA4.c"

/*
 * Score-isolation candidate: the inherited semantic recovery lived in the
 * broad gs_event_exec scratch TU, while this object owns the retail range.
 */
extern void* windowGetFreeWork(void*);
extern void* windowAllocMemory(void*, s32);
extern void* windowGetAllocPtr(void*);
extern u8 windowSearchID(s32);
extern void fn_80103F74(s32, s32, s32);
extern void fn_801669BC(s32);

s32 fn_800129A8(u8* ctx)
{
    u8* work;
    s32 id;
    s32 kind;
    void* buffer;

    work = windowGetFreeWork(ctx);
    if ((s8)ctx[1] == 0) {
        buffer = windowAllocMemory(ctx, 0x30);
        if (buffer != NULL) {
            memcpy(buffer, *(void**)(ctx + 0x60), 0x30);
        }
    }
    windowGetAllocPtr(ctx);
    if ((s8)ctx[1] == 0) {
        id = *(s32*)(ctx + 4);
        kind = 0;
        if (windowSearchID(id) != 0) {
            if (id == 0x49) {
                kind = 0x538;
            } else if (id >= 0x49) {
                if (id < 0x4B) {
                    kind = 0x540;
                }
            } else if (id >= 0x47) {
                kind = 0x540;
            } else if (id >= 0x45) {
                kind = 0x538;
            }
            fn_80103F74(id, kind, 0);
        }

        id = *(s32*)(ctx + 4);
        kind = 0;
        if (windowSearchID(id) != 0) {
            if (id == 0x49) {
                kind = 0x539;
            } else if (id >= 0x49) {
                if (id < 0x4B) {
                    kind = 0x541;
                }
            } else if (id >= 0x47) {
                kind = 0x541;
            } else if (id >= 0x45) {
                kind = 0x539;
            }
            fn_80103F74(id, kind, 0);
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
            fn_801669BC(0x4D0);
        }
    }
    (*(u16*)(work + 6))++;
    *(u16*)(work + 6) %= 1200;
    return 0;
}

s32 fn_80012B94(u8* ctx)
{
    extern s32 fn_801040D0(u8*, s32);
    extern void* fn_80103FE4(u8*);
    extern u8 fn_8005D9E4(s32);
    extern s32 fn_800FA444(s32);
    extern void fn_8001EA98(s32, s32, s32, s32);
    extern void fn_8001E644(s32, s32, s32, s32, u8);
    extern void fn_800FB680(s32, s32, s32, s32);
    extern void fn_801040F0(s32, s16, u8*, s32, s32);
    u8* values;
    u8* iter;
    s32 count;
    s32 capacity;
    s32 maxHeight;
    s32 totalWidth;
    s32 range;
    s32 delay;
    s32 position;
    s32 i;
    u8 command;

    maxHeight = 0;
    totalWidth = 0;
    command = (u8)fn_801040D0(ctx, 0);
    values = fn_80103FE4(ctx);
    count = (s8)fn_801040D0(ctx, 2);
    capacity = fn_8005D9E4(*(s32*)(ctx + 4));
    if (count > capacity) {
        count = capacity;
    }

    iter = values;
    for (i = 0; i < count; i++, iter += 4) {
        range = fn_800FA444(*(s32*)iter);
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
        fn_8001EA98(0, 0, maxHeight + 0x20, totalWidth);
        break;
    }

    position = 1;
    iter = values;
    for (i = 0; i < count; i++, iter += 4) {
        if (*(u32*)iter != 0) {
            range = fn_800FA444(*(s32*)iter);
            delay = (range & 0xFFFF) + 2;
            fn_800FB680(0x20, position, -1, *(s32*)iter);
        } else {
            delay = 0x14;
        }
        if ((s8)ctx[0x95] == i) {
            fn_801040F0(0x20, (s16)position, ctx, 0x157, 0);
        }
        position += delay;
    }
    return 0;
}
