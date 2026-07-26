#include "src/game/gs_range_80011EA4.c"

/*
 * Score-isolation candidate: the inherited semantic recovery lived in the
 * broad gs_event_exec scratch TU, while this object owns the retail range.
 */
extern u8* windowGetFreeWork(void);
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

    work = windowGetFreeWork();
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
