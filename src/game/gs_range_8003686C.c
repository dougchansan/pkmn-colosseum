/**
 * @file gs_range_8003686C.c
 * @brief Post-movie boot fragment, 0x8003686C - 0x80037158.
 *
 * Called from movie.c right after menuOpen(0x85, ...); spawns
 * _menuSoundReadWaveThread and runs filesystem init (_fsysInitTOC).
 * Identified during the PDA decomposition (2026-07-03); the range
 * name stays honest until the TU identity is proven.
 */
#include "dolphin/types.h"

typedef struct WindowCursorWork {
    u8 pad[0x95];
    u8 cursor;
} WindowCursorWork;

extern s32 lbl_804788B8;
extern u32 lbl_8047A460;
extern u8 lbl_803A3E58[];
extern u8 lbl_80267050[];
extern f32 lbl_8047BA30;
extern f32 lbl_8047BA34;
extern f32 lbl_8047BA48;
extern void GSlogWrite(void* fmt, ...);
extern s32 OSGetResetCode(void);
extern u32 OSGetProgressiveMode(void);
extern u32 VIGetDTVStatus(void);
extern void _threadSwitch(void);
extern void fadeCheck(u32 arg);
extern void fn_800A0FC8(u32 arg);
extern u32 fn_800D3088(void);
extern s32 fn_800D37CC(void);
extern s32 fn_800F7BC4(u32 arg);
extern void fn_800F7F64(u32 arg);
extern void fn_80165F84(void);
extern void fn_80166E88(u32 a, u32 b, u32 c, u32 d, u32 e);
extern s32 fn_8017B2CC(u32 arg);
extern s32 menuOpenCustom(u32 id, s32 active, s32* out, u32 a, u32 b, u32 c);
extern void menuCloseCustom(u32 id, u32 a, u32 b);
extern void menuSetPosition(u32 id, u32 x, u32 y);
extern void winMsgClose(u32 arg);
extern void winMsgOpen(u32 a, u32 b, u32 c, u32 d);
extern u8 windowCheckCursor(u32 id, u32 arg);
extern s32 windowGetActiveID(void);
extern s8 windowGetValue(u32 id);
extern WindowCursorWork* windowSearchID(u32 id);
extern s32 OSCreateThread(void* thread, void* func, void* arg, void* stack, u32 stackSize, s32 priority, u16 detached);
extern void OSResumeThread(void* thread);
extern s32 _menuSoundReadWaveThread__FPv(u32* arg);

void fn_8003686C(void) {
    s32 selection;
    s32 pending;
    s32 cursorOutA;
    s32 cursorOutB;
    s8 lastCursor;
    f32 timer;
    f32 limit;
    s8 chosen;
    u8* base;
    u32* waveArgs;

    lbl_804788B8 = -1;

    if ((OSGetResetCode() + 0x80000000U) != 0U) {
        fadeCheck(1);
        timer = lbl_8047BA30;
        limit = lbl_8047BA34;
        while (timer < limit) {
            _threadSwitch();
            timer += (f32)fn_800D3088() / (f32)fn_800D37CC();
        }
        goto boot_done;
    }

    if (VIGetDTVStatus() == 1U) {
        fn_800F7F64(1);
        if ((fn_800F7BC4(1) & 0x200) != 0) {
            winMsgOpen(1, 0x3B50, 1, 1);
            cursorOutA = 0;
            menuOpenCustom(0x11, windowGetActiveID(), &cursorOutA, 0, 0, 0);
            menuSetPosition(0x11, 0x2D, 0xBE);
            timer = lbl_8047BA30;
            limit = lbl_8047BA48;
            lastCursor = -1;
            chosen = -1;
            while (timer < limit) {
                if (windowCheckCursor(0x11, 0) == 0) {
                    chosen = windowGetValue(0x11);
                    break;
                }
                if (lastCursor != (s8)windowSearchID(0x11)->cursor) {
                    timer = lbl_8047BA30;
                    lastCursor = (s8)windowSearchID(0x11)->cursor;
                }
                _threadSwitch();
                timer += (f32)fn_800D3088() / (f32)fn_800D37CC();
            }
            if (!(timer < limit)) {
                chosen = (s8)windowSearchID(0x11)->cursor;
            }
            menuCloseCustom(0x11, 0, 1);

            if (chosen == 0) {
                if (((OSGetResetCode() + 0x80000000U) == 0U && OSGetProgressiveMode() == 0U) ||
                    ((OSGetResetCode() + 0x80000000U) != 0U)) {
                    lbl_804788B8 = 1;
                }
                winMsgOpen(1, 0x3B51, 1, 1);
                timer = lbl_8047BA30;
                limit = lbl_8047BA34;
                while (timer < limit) {
                    _threadSwitch();
                    timer += (f32)fn_800D3088() / (f32)fn_800D37CC();
                }
            } else {
                if (((OSGetResetCode() + 0x80000000U) == 0U && OSGetProgressiveMode() == 1U) ||
                    ((OSGetResetCode() + 0x80000000U) != 0U)) {
                    lbl_804788B8 = 0;
                }
                winMsgOpen(1, 0x3B52, 1, 1);
                timer = lbl_8047BA30;
                limit = lbl_8047BA34;
                while (timer < limit) {
                    _threadSwitch();
                    timer += (f32)fn_800D3088() / (f32)fn_800D37CC();
                }
            }
            winMsgClose(1);
            goto boot_done;
        }

        if (OSGetProgressiveMode() == 1U) {
            winMsgOpen(1, 0x3B50, 1, 1);
            cursorOutB = 0;
            menuOpenCustom(0x11, windowGetActiveID(), &cursorOutB, 0, 0, 0);
            menuSetPosition(0x11, 0x2D, 0xBE);
            timer = lbl_8047BA30;
            limit = lbl_8047BA48;
            lastCursor = -1;
            chosen = -1;
            while (timer < limit) {
                if (windowCheckCursor(0x11, 0) == 0) {
                    chosen = windowGetValue(0x11);
                    break;
                }
                if (lastCursor != (s8)windowSearchID(0x11)->cursor) {
                    timer = lbl_8047BA30;
                    lastCursor = (s8)windowSearchID(0x11)->cursor;
                }
                _threadSwitch();
                timer += (f32)fn_800D3088() / (f32)fn_800D37CC();
            }
            if (!(timer < limit)) {
                chosen = (s8)windowSearchID(0x11)->cursor;
            }
            menuCloseCustom(0x11, 0, 1);

            if (chosen == 0) {
                if (((OSGetResetCode() + 0x80000000U) == 0U && OSGetProgressiveMode() == 0U) ||
                    ((OSGetResetCode() + 0x80000000U) != 0U)) {
                    lbl_804788B8 = 1;
                }
                winMsgOpen(1, 0x3B51, 1, 1);
            } else {
                if (((OSGetResetCode() + 0x80000000U) == 0U && OSGetProgressiveMode() == 1U) ||
                    ((OSGetResetCode() + 0x80000000U) != 0U)) {
                    lbl_804788B8 = 0;
                }
                winMsgOpen(1, 0x3B52, 1, 1);
            }
            timer = lbl_8047BA30;
            limit = lbl_8047BA34;
            while (timer < limit) {
                _threadSwitch();
                timer += (f32)fn_800D3088() / (f32)fn_800D37CC();
            }
            winMsgClose(1);
            goto boot_done;
        }
    }

    fadeCheck(1);
    timer = lbl_8047BA30;
    limit = lbl_8047BA34;
    while (timer < limit) {
        _threadSwitch();
        timer += (f32)fn_800D3088() / (f32)fn_800D37CC();
    }

boot_done:
    for (;;) {
        pending = fn_8017B2CC(0xA);
        if (pending < 0) {
            GSlogWrite(lbl_80267050);
        }
        if (pending == 0) {
            break;
        }
        _threadSwitch();
    }

    fn_80166E88(2, 1, 0x20, 1, 0x10);
    fn_80165F84();

    base = lbl_803A3E58;
    waveArgs = (u32*)(base + 0x1318);
    waveArgs[0] = 1;
    lbl_8047A460++;
    waveArgs[1] = 0;
    waveArgs[2] = 0;
    waveArgs[3] = 0;
    OSCreateThread(base + 0x1328, _menuSoundReadWaveThread__FPv, waveArgs, base + 0x263C, 0x1000, 0x10, 1);
    OSResumeThread(base + 0x1328);

    while ((s32)lbl_8047A460 != 0) {
        _threadSwitch();
    }
}

#pragma push
#pragma peephole off
s32 fn_8003708C(void)
{
    extern s32 _fsysInitTOC(u32 numSlots, u32 param2, u32 param3, u32 param4);
    extern u32 GSgappCreate(s32 state, u8 priority, void* param, void* func);
    extern void fn_8017AAA4(void);
    extern u32 lbl_8047A464;

    _fsysInitTOC(0x40, 0, 0, 0);
    GSgappCreate(1, 0x14, 0, fn_8017AAA4);
    lbl_8047A464 = 1;
    return 0;
}
#pragma pop

#pragma push
#pragma peephole off
s32 _menuSoundReadWaveThread__FPv(u32* arg)
{
    extern void fn_801655D4(u32 index);
    extern u32 lbl_8047A460;

    if (arg[0] != 0) {
        fn_801655D4(arg[0]);
    }
    if (arg[1] != 0) {
        fn_801655D4(arg[1]);
    }
    if (arg[2] != 0) {
        fn_801655D4(arg[2]);
    }
    if (arg[3] != 0) {
        fn_801655D4(arg[3]);
    }
    lbl_8047A460--;
    return 0;
}
#pragma pop
