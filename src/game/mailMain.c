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
    void* buffer;

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

/**
 * mailMainSendAllMail - Waza effect scale animation.
 * Address: 0x801D216C | Size: 0x120
 */
void mailMainSendAllMail(s32 seqHandle, f32 targetScale, f32 speed) {
    /* TODO: Effect scale animation (0x120 bytes) */
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
    u16 handle = (u16)seqHandle;
    u16 count;
    u16 i;
    s32 object;
    u8 limit;

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
            limit = (u8)((s32 (*)(s32, s32))fn_801D1734)(handle, i);
            if (fn_801EE614(object) < limit) {
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
        heroMoveAddAutoEvent(object, handle, 0, 0, 0);
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
void fn_801D2404(s32 seqHandle, f32 x, f32 y, f32 z, f32 scale, f32 rot) {
    /* TODO: Effect complex transform (0x288 bytes) */
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
    u32 soundId;
    u32 workId;
    void* buffer;

    if (lbl_80467390[0] != 0) {
        return;
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
    lbl_80467390[0] = 1;
    lbl_80467390[1] = 0;
    lbl_80467390[2] = soundId;
    lbl_80467390[3] = workId;
}

/**
 * chkMailSend - Waza effect detach and fly.
 * Address: 0x801D2764 | Size: 0x274
 */
void chkMailSend(s32 seqHandle, f32 velX, f32 velY, f32 velZ) {
    /* TODO: Effect detach and fly (0x274 bytes) */
}

/**
 * cbStep - Waza multi-hit effect setup.
 * Address: 0x801D29D8 | Size: 0x130
 */
void cbStep(s32 moveID, s32 hitCount) {
    /* TODO: Multi-hit effect setup (0x130 bytes) */
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
