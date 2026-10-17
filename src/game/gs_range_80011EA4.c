/**
 * @file gs_range_80011EA4.c
 * @brief gs-engine code, 0x80011EA4 - 0x80012D20 (4 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). The range name stays honest until internal
 * TU structure is proven.
 */
#include "dolphin/types.h"

/* 0x80011EA4 | 0x9B4 -- sprite-driven window gauges and labels */
extern void winSpriteSetDisp(void*, s32);
extern void GSmsgPrintColor();
extern u8 lbl_80314E08[];
extern void* windowGetAllocPtr(void*);
extern void* windowGetFreeWork(void*);

typedef struct WindowSprite {
    u8 pad_00[6];
    s16 kind;
    u8 pad_08[0x4C];
    s16 x;
    s16 y;
} WindowSprite;

typedef struct WindowDisplayWork {
    u8 pad_00[0x16];
    u8 mode;
    u8 value_17;
    s16 value_18;
    s16 value_1A;
    union {
        u32 u;
        f32 f;
    } max_1C;
    union {
        u32 u;
        f32 f;
    } value_20;
    u16 sprite_24;
    u16 sprite_26;
    u8 kind_28;
    u8 flag;
} WindowDisplayWork;

typedef struct WindowDisplayAnim {
    s16 start;
    s16 duration;
    s16 time;
    u16 value_06;
    union {
        u32 u;
        f32 f;
    } start_08;
    s16 duration_0C;
    s16 time_0E;
} WindowDisplayAnim;

/*
 * fn_80012858 (0x80012858) is linked from gs_range_candidate_80012858.c.
 * fn_80011EA4 inlines it, so this unit keeps the body as an inline
 * definition that is not emitted here.
 */
#if defined(GS_RANGE_CANDIDATE_80012858)
#define GS_RANGE_80012858_LINKAGE
#else
#define GS_RANGE_80012858_LINKAGE inline
#endif
#if !defined(GS_RANGE_CANDIDATE_80012858)
#endif
GS_RANGE_80012858_LINKAGE void fn_80012858(void* window, WindowSprite* sprite)
{
    void* windowGetAllocPtr(void* window);
    WindowDisplayWork* work = windowGetAllocPtr(window);
    s32 kind = sprite->kind;
    s32 display = 1;
    u8 mode = work->mode;

    switch (kind) {
    case 0x9B:
    case 0x9C:
    case 0xA6:
    case 0xA7:
    case 0x534:
    case 0x535:
    case 0x53C:
    case 0x53D:
        if (work->flag != 0) {
            display = 0;
        } else {
            display = 1;
        }
        break;
    case 0x99:
    case 0x9D:
    case 0x9E:
    case 0xA4:
    case 0xA8:
    case 0xA9:
    case 0x536:
    case 0x537:
    case 0x53E:
    case 0x53F:
        if (work->flag != 0) {
            display = 1;
        } else {
            display = 0;
        }
        break;
    }

    switch (kind) {
    case 0x99:
    case 0xA4:
    case 0x534:
    case 0x535:
    case 0x536:
    case 0x537:
    case 0x53C:
    case 0x53D:
    case 0x53E:
    case 0x53F:
        if (mode == 1) {
            display = 0;
        }
        break;
    }

    winSpriteSetDisp(sprite, display);
}
#if !defined(GS_RANGE_CANDIDATE_80012858)
#endif

#if !defined(GS_RANGE_CANDIDATE_80012858)
/*
 * Not referenced by the game, so the linker strips it. Its constants still
 * lead this TU's .sdata2 pool (0.0f at 0x8047B718, 100.0f at 0x8047B71C),
 * ahead of fn_80011EA4's 1200.0f, as in retail.
 */
/* RULE-EXCEPTION(user-approved): reconstructed linker-stripped helper; its body is inferred from the .sdata2 pool layout only and exists to place 0.0f/100.0f first — see docs/RULE_EXCEPTIONS.md */
f32 gsGaugeGetPercent(f32 current, f32 max)
{
    if (max <= 0.0f) {
        return 0.0f;
    }
    return current * 100.0f / max;
}

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_80011EA4(u8* window, WindowSprite* sprite)
{
    extern void fn_8001DACC(void*, WindowSprite*, u16, f32);
    extern void fn_8010B9E8(void*, WindowSprite*, u16);
    extern s32 fightFloorGetStatus(s32, s32, s32, s32);
    extern void GSmsgPrintColor(s32, s32, u32, s32);
    extern void msgctrlSetValue(s32, s32);
    extern u32 GSmsgGetRect(s32);
    extern s32 GSmsgGetGSchar(u32);
    extern void windowDrawSprite2(s32, s32, s32, s32, u32, void*, u16, s32);
    extern void gfxSetFlags(s32);
    extern void gfxClearFlags(s32);
    extern void gfxSetPrimitive(s32);
    extern void gfxSetVtxDesc(void*);
    extern void gfxBegin(s32);
    extern void gfxVtxPos2s16(s32, s32);
    extern void gfxVtxColor1u32(s32, u32);
    extern void gfxEnd(void);
    extern u32 __cvt_fp2unsigned(f32);
    extern u8 winSpriteGetDisp(WindowSprite*);
    WindowDisplayWork* work;
    WindowDisplayAnim* anim;
    u32 color;

    work = windowGetAllocPtr(window);
    anim = windowGetFreeWork(window);
    fn_80012858(window, sprite);
    if (winSpriteGetDisp(sprite) == 0) {
        return;
    }

    color = window[0x8B] | -0x100;
    switch (sprite->kind) {
    case 0x9D:
    case 0x9E:
    case 0xA8:
    case 0xA9:
        if (work->flag == 2) {
            fn_8001DACC(window, sprite, anim->value_06,
                        (f32)anim->value_06 / 1200.0f);
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x9A:
    case 0xA5:
        fn_8010B9E8(window, sprite, work->sprite_26);
        break;
    case 0xA1:
    case 0xAC: {
        s16 value;
        f32 current;

        if (fightFloorGetStatus(0, 0, 0x32, 0) != 0 && work->mode != 0) {
            break;
        }
        if (anim->duration != 0) {
            current = (f32)anim->time / (f32)anim->duration *
                          (f32)(work->value_1A - anim->start) +
                      (f32)anim->start;
            value = current;
            if (value == 0 && current > 0.0f) {
                value = 1;
            }
        } else {
            value = work->value_1A;
        }
        GSmsgPrintColor(0x20, -2, color, 0x195);
        msgctrlSetValue(0x34, value);
        GSmsgPrintColor((s16)(0x18 - (s16)(GSmsgGetRect(0xCB) >> 16)), -1, color,
                    0xCB);
        msgctrlSetValue(0x34, work->value_18);
        GSmsgPrintColor((s16)(sprite->x - (s16)(GSmsgGetRect(0xCB) >> 16)), -1,
                    color, 0xCB);
        break;
    }
    case 0xA2:
    case 0xAD:
        msgctrlSetValue(0x34, work->value_17);
        GSmsgPrintColor((s16)(sprite->x - (s16)(GSmsgGetRect(0xCB) >> 16)), -1,
                    window[0x8B] | -0x100, 0xCB);
        break;
    case 0xA3:
    case 0xAE: {
        s16 width;
        u32 message;

        msgctrlSetValue(0x37, (s32)work);
        GSmsgPrintColor(0, -1, color, 0xE9);
        width = GSmsgGetRect(0xE9) >> 16;
        switch (work->kind_28) {
        case 0:
            message = 0xD67;
            break;
        case 1:
            message = 0xD68;
            break;
        case 2:
        default:
            message = 0;
            break;
        }
        if (message != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(message));
            GSmsgPrintColor(width, -1, color, 0xD0);
        }
        break;
    }
    case 0x532:
    case 0x53A:
        windowDrawSprite2(0, 0, sprite->x, sprite->y, color, window,
                          (u16)work->sprite_24, 0);
        break;
    case 0x533:
    case 0x53B: {
        f32 current;
        f32 max;
        u16 id;
        s32 width;

        if (anim->duration != 0) {
            current = (f32)anim->time / (f32)anim->duration *
                          (f32)(work->value_1A - anim->start) +
                      (f32)anim->start;
        } else {
            current = work->value_1A;
        }
        max = work->value_18;
        if (anim->duration != 0) {
            s32 start = anim->start;
            s32 x;
            u32 bar;

            if (start <= 0) {
                bar = 0;
            } else if ((f32)start <= 20.0f * max / 100.0f) {
                bar = 0x80000000;
            } else if ((f32)start <= 50.0f * max / 100.0f) {
                bar = 0x64640000;
            } else {
                bar = 0x00800000;
            }
            bar |= window[0x8B];
            x = ((max - 1.0f) + (f32)(start * sprite->x)) / max;
            gfxSetFlags(1);
            gfxClearFlags(6);
            gfxSetPrimitive(7);
            gfxSetVtxDesc(lbl_80314E08);
            gfxBegin(2);
            gfxVtxPos2s16(0, 0);
            gfxVtxColor1u32(0, bar);
            gfxVtxPos2s16(x, sprite->y);
            gfxVtxColor1u32(0, bar);
            gfxEnd();
        }
        if (current <= 0.0f) {
            id = 0;
        } else if (current <= 20.0f * max / 100.0f) {
            id = 0x1AD;
        } else if (current <= 50.0f * max / 100.0f) {
            id = 0x1B0;
        } else {
            id = 0x1B1;
        }
        width = (current * (f32)sprite->x + (max - 1.0f)) / max;
        if (id != 0) {
            windowDrawSprite2(0, 0, width, sprite->y, color, window, id, 0);
        }
        break;
    }
    case 0x535:
    case 0x53D: {
        u32 current;
        u32 max;

        if (anim->duration_0C != 0) {
            current = __cvt_fp2unsigned(
                (f32)anim->time_0E / (f32)anim->duration_0C *
                    (f32)(work->value_20.u - anim->start_08.u) +
                (f32)anim->start_08.u);
        } else {
            current = work->value_20.u;
        }
        max = work->max_1C.u;
        if (current > max) {
            current = max;
        }
        if (max != 0) {
            windowDrawSprite2(0, 0,
                              (s16)((max - 1 + current * sprite->x) / max),
                              sprite->y, color, window, 0x1AC, 0);
        }
        break;
    }
    case 0x537:
    case 0x53F: {
        f32 current;
        f32 max;
        s32 width;

        if (anim->duration_0C != 0) {
            current = (f32)anim->time_0E / (f32)anim->duration_0C *
                          (work->value_20.f - anim->start_08.f) +
                      anim->start_08.f;
        } else {
            current = work->value_20.f;
        }
        max = work->max_1C.f;
        if (current > max) {
            current = max;
        }
        width = current * (f32)sprite->x / max;
        if (current > 0.0f && (s16)width == 0) {
            width = 1;
        }
        windowDrawSprite2(0, 0, width, sprite->y, color, window, 0x1AB, 0);
        break;
    }
    case 0x53C:
    case 0x53E:
        break;
    }
}
#pragma pop
#endif /* !GS_RANGE_CANDIDATE_80012858 */
