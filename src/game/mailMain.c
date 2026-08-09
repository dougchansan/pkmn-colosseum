/**
 * @file mailMain.c
 * @brief Mail system: mailMain* prefixed dispatch/init/receive functions.
 *
 * Split from the former game/battle/battle_waza.c CodeCandidate bucket
 * (0x801D1470-0x801DE698); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit. Shared typedefs and cross-TU
 * forward declarations live in include/game/battle/battle_waza_types.h.
 */

#include "game/battle/battle_waza_types.h"


/**
 * mailMainSendByScrpt - Waza effect alpha fade.
 * Address: 0x801D2080 | Size: 0xEC
 */
void mailMainSendByScrpt(s32 seqHandle, f32 alpha, f32 speed) {
    extern u32 fn_8016557C(void);
    extern void* GSresAllocResourceAlign(u32, u32, u32, u32, u32);
    extern u32 fn_801654E0(u32, void*, u32);
    extern u32 fn_80166B3C(u32, u32, u32);
    extern u32 fn_80166A50(u32, u32, u32, u32);
    u32 soundId;
    u32 workId;

    seqHandle = (u16)seqHandle;
    if (mailGetReceiveNumber(seqHandle) >= 0) {
        return;
    }
    mailAddMailbox(seqHandle);
    if (lbl_80467390[0] != 0) {
        return;
    }
    soundId = fn_801D1650(fn_801D16C4());
    if (soundId != 0) {
        workId = fn_8016557C();
        fn_801654E0(soundId,
                    GSresAllocResourceAlign(0x10000, 0x20, 0, 0x408, 0),
                    0x10000);
        fn_80166B3C(soundId, 0, 0x408);
        fn_80166A50(soundId, 0, 0xFF, 0);
    } else {
        workId = 0;
    }
    lbl_80467390[0] = 1;
    lbl_80467390[1] = 0;
    lbl_80467390[2] = soundId;
    lbl_80467390[3] = workId;
}

/**
 * mailMainSendAllMail - Waza effect scale animation.
 * Address: 0x801D216C | Size: 0x120
 */
void mailMainSendAllMail(s32 seqHandle, f32 targetScale, f32 speed) {
    extern f32 lbl_8047E1B0;
    extern u32 fn_8016557C(void);
    extern void* GSresAllocResourceAlign(u32, u32, u32, u32, u32);
    extern u32 fn_801654E0(u32, void*, u32);
    extern u32 fn_80166B3C(u32, u32, u32);
    extern u32 fn_80166A50(u32, u32, u32, u32);
    u32 soundId;
    u32 workId;
    s32 count;
    s32 i;
    s32 sent;

    count = mailGetNbMailData();
    sent = 0;
    for (i = 0; i < count; i++) {
        if (chkMailSend(i, lbl_8047E1B0)) {
            mailAddMailbox(i);
            sent = 1;
        }
    }
    if (sent && lbl_80467390[0] == 0) {
        soundId = fn_801D1650(fn_801D16C4());
        if (soundId != 0) {
            workId = fn_8016557C();
            fn_801654E0(soundId,
                        GSresAllocResourceAlign(0x10000, 0x20, 0, 0x408, 0),
                        0x10000);
            fn_80166B3C(soundId, 0, 0x408);
            fn_80166A50(soundId, 0, 0xFF, 0);
        } else {
            workId = 0;
        }
        lbl_80467390[0] = 1;
        lbl_80467390[1] = 0;
        lbl_80467390[2] = soundId;
        lbl_80467390[3] = workId;
    }
}

/**
 * fn_801D228C - Waza effect rotation animation.
 * Address: 0x801D228C | Size: 0x134
 */
void fn_801D228C(s32 seqHandle) {
    extern void fn_80190528(s32);
    extern void fn_801EED30(s32, s32);
    extern u16 fn_801EE614(s32);
    extern void fn_801EE67C(s32);
    extern void fn_800F7434(s32, s32, ...);
    extern void heroMoveAddAutoEvent(s32, u16, s32, s32, s32);
    u16 handle;
    u16 count;
    u16 i;
    u32 object;
    u32 limit;

    handle = (u16)seqHandle;

    object = fn_801D1504(handle);
    if (object != -1 && object != 0) {
        fn_80190528(object);
    }

    i = (u16)fn_801D139C(handle);
    if (i != 0xFFFF) {
        ((void (*)(s32))fn_801D1364)(i);
    }

    count = (u16)fn_801D19A4(handle);
    if (count != 0xFFFF) {
        for (i = 0; i < count; i++) {
            object = ((s32 (*)(s32, s32))fn_801D1864)(handle, i);
            fn_801EED30(object, 1);
            limit = ((s32 (*)(s32, s32))fn_801D1734)(handle, i);
            if (fn_801EE614(object) < (u8)limit) {
                fn_801EE67C(object);
            }
        }
    }

    object = fn_801D14C0(handle);
    if (object != -1 && object != 0) {
        fn_800F7434(object, 0);
    }

    object = fn_801D147C(handle);
    if (object != -1 && object != 0) {
        heroMoveAddAutoEvent(object, seqHandle, 0, 0, 0);
    }
}

/**
 * mailMainReceiveTerminate - Waza reset callback timer and stop active handle.
 * Address: 0x801D23C0 | Size: 0x44
 */
extern s32 lbl_80467390[];
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
void mailMainReceiveTerminate(void) {
    u32 handle;
    lbl_80467390[1] = 0x258;
    handle = lbl_80467390[2];
    if (handle != 0) {
        fn_801669E4(handle, 0, 0);
    }
}
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on

/**
 * fn_801D2404 - Waza effect complex transform.
 * Address: 0x801D2404 | Size: 0x288
 */
void fn_801D2404(void) {
    extern void* fn_800F92D4(u32);
    extern s32 GStextureGetXsize(void*);
    extern s32 GStextureGetYsize(void*);
    extern void fn_800FE6D0(s32, s32);
    extern void fn_800FE35C(void);
    extern void spriteSetEnv(void);
    extern void fn_800D9ED8(s32);
    extern void fn_800D7820(void*);
    extern void fn_800D88DC(s32);
    extern void fn_800D888C(s32);
    extern void fn_800D85D4(s32, void*);
    extern void fn_800D6A00(s32);
    extern void fn_800D67BC(s32);
    extern void fn_800D61E4(s16, s16);
    extern void fn_800D5CB8(s32, u8, u8, u8, u8);
    extern void fn_800D59B8(s32, f32, f32);
    extern void fn_800D6728(void);
    extern s32 gamedatasaveGetStatus(s32, s32);
    extern void fn_800F78A4(s32, s32, s32, s32, s32);
    extern s32 fn_800D3088(void);
    extern s32 fn_801666BC(u32);
    extern void fn_800F9210(s32, s32);
    extern void fn_80166B18(u32);
    extern void fn_801667D8(u32, s32, s32);
    extern u8 lbl_80314F98[];
    extern f32 lbl_8047E1B4, lbl_8047E1B8, lbl_8047E1BC;
    void* texture;
    s32 width;
    s32 height;
    s32 visible;
    u32 sound;
    u32 work;

    if (lbl_80467390[0] == 0) {
        return;
    }

    visible = 0;
    if (lbl_80467390[1] < 0x258 &&
        lbl_80467390[1] % 60 < 30) {
        visible = 1;
    }
    if (visible) {
        texture = fn_800F92D4(0x0C881200);
        width = GStextureGetXsize(texture);
        height = GStextureGetYsize(texture);
        fn_800FE6D0(0, 0);
        fn_800FE35C();
        spriteSetEnv();
        fn_800D9ED8(1);
        fn_800D7820(lbl_80314F98);
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D85D4(0, texture);
        fn_800D6A00(7);
        fn_800D67BC(2);
        fn_800D61E4(0x15, 0x25);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047E1B4, lbl_8047E1B4);
        fn_800D61E4((s16)(width + 0x15), (s16)(height + 0x25));
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047E1B8, lbl_8047E1B8);
        fn_800D6728();
        fn_800D9ED8(0);
        if (gamedatasaveGetStatus(0, 9) == 0 &&
            (f32)lbl_80467390[1] < lbl_8047E1BC) {
            fn_800F78A4(1, 0, 0xFF, 0x1E, 0);
        }
    }

    lbl_80467390[1] += fn_800D3088();
    sound = lbl_80467390[2];
    if (sound != 0 && fn_801666BC(sound) != 2) {
        fn_800F9210(0, 0x408);
        fn_80166B18(sound);
        work = lbl_80467390[3];
        if (work != 0 && work != 0xFFFFFFFF) {
            fn_801667D8(work, 0, 0);
        }
        lbl_80467390[2] = 0;
    }
    if (lbl_80467390[1] >= 0x258 && lbl_80467390[2] == 0) {
        lbl_80467390[0] = 0;
    }
}

/**
 * mailMainReceiveStart - Waza effect attach to bone.
 * Address: 0x801D268C | Size: 0xD8
 */
void mailMainReceiveStart(s32 seqHandle, s32 slot, s32 boneIdx) {
    extern u32 fn_8016557C(void);
    extern void* GSresAllocResourceAlign(u32, u32, u32, u32, u32);
    extern u32 fn_801654E0(u32, void*, u32);
    extern u32 fn_80166B3C(u32, u32, u32);
    extern u32 fn_80166A50(u32, u32, u32, u32);
    u32 workId;
    u32 soundId;
    void* buffer;

    {
        s32* state = lbl_80467390;
        if (state[0] != 0) {
            return;
        }
    }
    soundId = fn_801D1650(fn_801D16C4());
    if (soundId != 0) {
        workId = fn_8016557C();
        buffer = GSresAllocResourceAlign(0x10000, 0x20, 0, 0x408, 0);
        fn_801654E0(soundId, buffer, 0x10000);
        fn_80166B3C(soundId, 0, 0x408);
        fn_80166A50(soundId, 0, 0xFF, 0);
    } else {
        workId = 0;
    }
    {
        s32* state = lbl_80467390;
        state[0] = 1;
        state[1] = 0;
        state[2] = soundId;
        state[3] = workId;
    }
}

/**
 * chkMailSend - Waza effect detach and fly.
 * Address: 0x801D2764 | Size: 0x274
 */
s32 chkMailSend(s32 seqHandle, f32 step) {
    extern u16 mailGetSendRate(s32);
    extern u8 fn_801D142C(s32);
    extern u16 fn_801D13E4(s32);
    extern u16 fn_801D1338(u16);
    extern u8 mailGetSendCondType(s32);
    extern u32 mailGetSendCondition(s32);
    extern void* fn_801906A0(u32);
    extern s32 fn_800F7434(u32, s32, ...);
    extern f32 lbl_8047E1C8, lbl_8047E1CC;
    u16 rate;
    u16 expected;
    u16 actual;
    u8 comparison;
    u8 conditionType;
    u32 condition;
    s32 eligible;

    if (mailGetReceiveNumber(seqHandle) >= 0) {
        return FALSE;
    }
    rate = mailGetSendRate(seqHandle);
    if (rate == 0xFFFF) {
        return FALSE;
    }
    if (step >= (lbl_8047E1C8 * (f32)rate) / lbl_8047E1CC) {
        return FALSE;
    }

    comparison = fn_801D142C(seqHandle);
    if (comparison == 0xFF) {
        return FALSE;
    }
    expected = fn_801D13E4(seqHandle);
    if (expected == 0xFFFF) {
        return FALSE;
    }
    actual = fn_801D1338(expected);
    eligible = FALSE;
    switch (comparison) {
    case 0:
        eligible = actual == expected;
        break;
    case 1:
        eligible = actual > expected;
        break;
    case 2:
        eligible = actual < expected;
        break;
    case 3:
        eligible = actual >= expected;
        break;
    case 4:
        eligible = actual <= expected;
        break;
    case 5:
        eligible = actual != expected;
        break;
    case 6:
        eligible = TRUE;
        break;
    }
    if (!eligible) {
        return FALSE;
    }

    conditionType = mailGetSendCondType(seqHandle);
    if (conditionType == 0xFF) {
        return FALSE;
    }
    eligible = FALSE;
    if (conditionType == 0) {
        eligible = TRUE;
    } else if (conditionType == 1) {
        condition = mailGetSendCondition(seqHandle);
        if (condition != 0xFFFFFFFF &&
            fn_801906A0(condition) != NULL) {
            eligible = TRUE;
        }
    } else if (conditionType == 2) {
        condition = mailGetSendCondition(seqHandle);
        if (condition != 0xFFFFFFFF && condition != 0 &&
            fn_800F7434(condition, 0) != 0) {
            eligible = TRUE;
        }
    }
    return eligible;
}

/**
 * cbStep - Waza multi-hit effect setup.
 * Address: 0x801D29D8 | Size: 0x130
 */
void cbStep(s32 moveID, s32 hitCount) {
    extern f32 fn_800E0BE4(void);
    extern u32 fn_8016557C(void);
    extern void* GSresAllocResourceAlign(u32, u32, u32, u32, u32);
    extern u32 fn_801654E0(u32, void*, u32);
    extern u32 fn_80166B3C(u32, u32, u32);
    extern u32 fn_80166A50(u32, u32, u32, u32);
    u32 soundId;
    u32 workId;
    void* buffer;
    f32 step;
    s32 count;
    s32 sent;
    s32 i;

    step = fn_800E0BE4();
    count = mailGetNbMailData();
    sent = 0;
    for (i = 0; i < count; i++) {
        if (chkMailSend(i, step)) {
            mailAddMailbox(i);
            sent = 1;
        }
    }
    if (sent && lbl_80467390[0] == 0) {
        soundId = fn_801D1650(fn_801D16C4());
        if (soundId != 0) {
            workId = fn_8016557C();
            buffer = GSresAllocResourceAlign(0x10000, 0x20, 0, 0x408, 0);
            fn_801654E0(soundId, buffer, 0x10000);
            fn_80166B3C(soundId, 0, 0x408);
            fn_80166A50(soundId, 0, 0xFF, 0);
        } else {
            workId = 0;
        }
        lbl_80467390[0] = 1;
        lbl_80467390[1] = 0;
        lbl_80467390[2] = soundId;
        lbl_80467390[3] = workId;
    }
}

/**
 * mailMainInit - Waza register multi-hit callback and clear state.
 * Address: 0x801D2B08 | Size: 0x44
 */
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
void mailMainInit(void) {
    s32* state;

    heroMoveAddStepCallback(cbStep, 0);
    state = lbl_80467390;
    state[0] = 0;
    state[2] = 0;
    state[3] = 0;
}
