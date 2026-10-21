/**
 * @file mailMain.c
 * @brief Mail system: mailMain* prefixed dispatch/init/receive functions.
 *
 * Split from the former game/battle/battle_waza.c CodeCandidate bucket
 * (0x801D1470-0x801DE698); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit. Shared typedefs and cross-TU
 * forward declarations live in include/game/battle/battle_waza_types.h.
 */

/* The mail getters below are declared with the types their callers here
 * compile against (u8 handle index, u16 mode/count returns, the raw u32
 * object id); battle_waza_types.h keeps older prototypes for its other
 * users, so they are renamed out of the way while it is included. */
#define mailGetEntryField0 fn_801D1650_hdr
#define mailBiosSetCounter fn_801D1364_hdr
#define mailDataBiosGet08 fn_801D139C_hdr
#define shadowGroupGetEntryId fn_801D1864_hdr
#define shadowGroupGetEntryByte fn_801D1734_hdr
#define shadowGroupGetEntryCount fn_801D19A4_hdr
#include "game/battle/battle_waza_types.h"
#undef mailGetEntryField0
#undef mailBiosSetCounter
#undef mailDataBiosGet08
#undef shadowGroupGetEntryId
#undef shadowGroupGetEntryByte
#undef shadowGroupGetEntryCount
extern u32 mailGetEntryField0(u8 idx);
extern void mailBiosSetCounter(u16 mode);
extern u16 mailDataBiosGet08(s32 idx);
extern u32 shadowGroupGetEntryId(s32 sequence, s32 index);
extern u8 shadowGroupGetEntryByte(s32 sequence, s32 index);
extern u16 shadowGroupGetEntryCount(s32 idx);


/**
 * mailMainSendByScrpt - Waza effect alpha fade.
 * Address: 0x801D2080 | Size: 0xEC
 */
static inline void mailMainStartSe(void) {
    extern u32 soundPauseMusic(void);
    extern void* GSresAllocResourceAlign(u32, u32, u32, u32, u32);
    extern u32 soundLoadGroupForSlot(u32, void*, u32);
    extern u32 soundAttachWork(u32, u32, u32);
    extern u32 soundStart(u32, u32, u32, u32);
    u32 workId;
    u32 soundId;

    soundId = mailGetEntryField0(mailGetCurrentIndex());
    if (soundId != 0) {
        workId = soundPauseMusic();
        soundLoadGroupForSlot(soundId,
                    GSresAllocResourceAlign(0x10000, 0x20, 0, 0x408, 0),
                    0x10000);
        soundAttachWork(soundId, 0, 0x408);
        soundStart(soundId, 0, 0xFF, 0);
    } else {
        workId = 0;
    }
    lbl_80467390[0] = 1;
    lbl_80467390[1] = 0;
    lbl_80467390[2] = soundId;
    lbl_80467390[3] = workId;
}

void mailMainSendByScrpt(s32 seqHandle, f32 alpha, f32 speed) {
    u16 mailId = seqHandle;

    if (mailGetReceiveNumber(mailId) >= 0) {
        return;
    }
    mailAddMailbox(mailId);
    if (lbl_80467390[0] != 0) {
        return;
    }
    mailMainStartSe();
}

/**
 * mailMainSendAllMail - Waza effect scale animation.
 * Address: 0x801D216C | Size: 0x120
 */
void mailMainSendAllMail(s32 seqHandle, f32 targetScale, f32 speed) {
    s32 count;
    s32 i;
    s32 sent;

    count = mailGetNbMailData();
    sent = 0;
    for (i = 0; i < count; i++) {
        if (chkMailSend(i, -1.0f)) {
            mailAddMailbox(i);
            sent = 1;
        }
    }
    if (sent && lbl_80467390[0] == 0) {
        mailMainStartSe();
    }
}

/**
 * mailApplyReward - Waza effect rotation animation.
 * Address: 0x801D228C | Size: 0x134
 *
 * fn_801EE67C takes (id, value) (its prologue saves r4); passing the limit
 * as its second argument is what puts the limit's u8->int conversion in r4
 * at the level compare.
 */
void mailApplyReward(s32 seqHandle) {
    extern void std078_flagSetFalse(s32);
    extern void darkPokemonSetFlag0E(u32, s32);
    extern u16 darkPokemonGetFlag1C(u32);
    extern void std243_shadowPokemonSetState(u32, u32);
    extern void cmsRunSync(s32, s32, ...);
    extern void heroMoveAddAutoEvent(s32, u16, s32, s32, s32);
    u16 handle;
    u16 count;
    u16 i;
    u32 object;
    u8 limit;
    u16 mode;

    handle = seqHandle;

    object = mailDataBiosGet20(handle);
    if (object != -1 && object != 0) {
        std078_flagSetFalse(object);
    }

    mode = mailDataBiosGet08(handle);
    if (mode != 0xFFFF) {
        mailBiosSetCounter(mode);
    }

    count = shadowGroupGetEntryCount(handle);
    if (count != 0xFFFF) {
        for (i = 0; i < count; i++) {
            object = shadowGroupGetEntryId(handle, i);
            darkPokemonSetFlag0E(object, 1);
            limit = shadowGroupGetEntryByte(handle, i);
            if (darkPokemonGetFlag1C(object) < limit) {
                std243_shadowPokemonSetState(object, limit);
            }
        }
    }

    object = mailDataBiosGet24(handle);
    if (object != -1 && object != 0) {
        cmsRunSync(object, 0);
    }

    object = mailDataBiosGet28(handle);
    if (object != -1 && object != 0) {
        heroMoveAddAutoEvent(object, seqHandle, 0, 0, 0);
    }
}

/**
 * mailMainReceiveTerminate - Waza reset callback timer and stop active handle.
 * Address: 0x801D23C0 | Size: 0x44
 */
extern s32 lbl_80467390[];
void mailMainReceiveTerminate(void) {
    u32 handle;
    lbl_80467390[1] = 0x258;
    handle = lbl_80467390[2];
    if (handle != 0) {
        soundStopRaw(handle, 0, 0);
    }
}

/**
 * menuDrawBlinkIcon - Waza effect complex transform.
 * Address: 0x801D2404 | Size: 0x288
 */
void menuDrawBlinkIcon(void) {
    extern void* GSresFindById(u32);
    extern s32 GStextureGetXsize(void*);
    extern s32 GStextureGetYsize(void*);
    extern void spriteSetOrigin(s32, s32);
    extern void fn_800FE35C(void);
    extern void spriteSetEnv(void);
    extern void gfxSetFlag1A(s32);
    extern void gfxSetVtxDesc(void*);
    extern void gfxSetFlags(s32);
    extern void gfxClearFlags(s32);
    extern void gfxLoadTexture(s32, void*);
    extern void gfxSetPrimitive(s32);
    extern void gfxBegin(s32);
    extern void gfxVtxPos2s16(s16, s16);
    extern void gfxVtxColor4u8(s32, u8, u8, u8, u8);
    extern void gfxVtxTexCoord(s32, f32, f32);
    extern void gfxEnd(void);
    extern s32 gamedatasaveGetStatus(s32, s32);
    extern void GSinputMotorStart(s32, s32, s32, s32, s32);
    extern s32 GSgetFrameStep(void);
    extern s32 GSsndGetStatus(u32);
    extern void GSresReleaseOwnerItem(s32, s32);
    extern void soundKill(u32);
    extern void soundResumeSong(u32, s32, s32);
    extern u8 lbl_80314F98[];
    void* texture;
    s32 width;
    s32 height;
    s32 visible;
    u32 sound;
    u32 work;

    visible = 0;
    if (lbl_80467390[0] == 0) {
        return;
    }

    if (lbl_80467390[1] < 0x258 &&
        lbl_80467390[1] % 60 < 30) {
        visible = 1;
    }
    if (visible) {
        texture = GSresFindById(0x0C881200);
        width = GStextureGetXsize(texture);
        height = GStextureGetYsize(texture);
        spriteSetOrigin(0, 0);
        fn_800FE35C();
        spriteSetEnv();
        gfxSetFlag1A(1);
        gfxSetVtxDesc(lbl_80314F98);
        gfxSetFlags(3);
        gfxClearFlags(4);
        gfxLoadTexture(0, texture);
        gfxSetPrimitive(7);
        gfxBegin(2);
        gfxVtxPos2s16(0x15, 0x25);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxTexCoord(0, 0.0f, 0.0f);
        gfxVtxPos2s16((s16)(width + 0x15), (s16)(height + 0x25));
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxTexCoord(0, 1.0f, 1.0f);
        gfxEnd();
        gfxSetFlag1A(0);
        if (gamedatasaveGetStatus(0, 9) == 0 &&
            (f32)lbl_80467390[1] < 180.0f) {
            GSinputMotorStart(1, 0, 0xFF, 0x1E, 0);
        }
    }

    lbl_80467390[1] += GSgetFrameStep();
    sound = lbl_80467390[2];
    if (sound != 0 && GSsndGetStatus(sound) != 2) {
        GSresReleaseOwnerItem(0, 0x408);
        soundKill(sound);
        work = lbl_80467390[3];
        if (work != 0 && work != 0xFFFFFFFF) {
            soundResumeSong(work, 0, 0);
        }
        lbl_80467390[2] = 0;
    }
    if (lbl_80467390[1] >= 0x258 && (u32)lbl_80467390[2] == 0) {
        lbl_80467390[0] = 0;
    }
}

/**
 * mailMainReceiveStart - Waza effect attach to bone.
 * Address: 0x801D268C | Size: 0xD8
 */
void mailMainReceiveStart(s32 seqHandle, s32 slot, s32 boneIdx) {
    extern u32 soundPauseMusic(void);
    extern void* GSresAllocResourceAlign(u32, u32, u32, u32, u32);
    extern u32 soundLoadGroupForSlot(u32, void*, u32);
    extern u32 soundAttachWork(u32, u32, u32);
    extern u32 soundStart(u32, u32, u32, u32);
    u32 workId;
    u32 soundId;
    void* buffer;

    {
        s32* state = lbl_80467390;
        if (state[0] != 0) {
            return;
        }
    }
    soundId = mailGetEntryField0(mailGetCurrentIndex());
    if (soundId != 0) {
        workId = soundPauseMusic();
        buffer = GSresAllocResourceAlign(0x10000, 0x20, 0, 0x408, 0);
        soundLoadGroupForSlot(soundId, buffer, 0x10000);
        soundAttachWork(soundId, 0, 0x408);
        soundStart(soundId, 0, 0xFF, 0);
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
    extern u32 mailDataBiosGet01(s32);
    extern u32 mailDataBiosGet04(s32);
    extern u16 mailBiosGetCounter(void);
    extern u8 mailGetSendCondType(s32);
    extern u32 mailGetSendCondition(s32);
    extern void* std082_flagGetValue(u32);
    extern s32 cmsRunSync(u32, s32, ...);
    u16 rate;
    u32 comparison;
    u32 expected;
    u16 actual;
    s32 eligible;
    u8 type;
    u32 condition;

    if (mailGetReceiveNumber(seqHandle) >= 0) {
        return FALSE;
    }
    rate = mailGetSendRate(seqHandle);
    if (rate == 0xFFFF) {
        return FALSE;
    }
    if (step >= (0.015625f * (f32)rate) / 1000.0f) {
        return FALSE;
    }

    comparison = mailDataBiosGet01(seqHandle);
    if ((u8)comparison == 0xFF) {
        return FALSE;
    }
    expected = mailDataBiosGet04(seqHandle);
    if ((u16)expected == 0xFFFF) {
        return FALSE;
    }
    actual = mailBiosGetCounter();
    eligible = FALSE;
    switch ((u8)comparison) {
    case 0:
        if (actual == (u16)expected) {
            eligible = TRUE;
        }
        break;
    case 1:
        if (actual > (u16)expected) {
            eligible = TRUE;
        }
        break;
    case 2:
        if (actual < (u16)expected) {
            eligible = TRUE;
        }
        break;
    case 3:
        if (actual >= (u16)expected) {
            eligible = TRUE;
        }
        break;
    case 4:
        if (actual <= (u16)expected) {
            eligible = TRUE;
        }
        break;
    case 5:
        if (actual != (u16)expected) {
            eligible = TRUE;
        }
        break;
    case 6:
        eligible = TRUE;
        break;
    }
    if (!eligible) {
        return FALSE;
    }

    type = mailGetSendCondType(seqHandle);
    if (type == 0xFF) {
        return FALSE;
    }
    eligible = FALSE;
    switch (type) {
    case 0:
        eligible = TRUE;
        break;
    case 1:
        condition = mailGetSendCondition(seqHandle);
        if (condition != 0xFFFFFFFF && std082_flagGetValue(condition) != NULL) {
            eligible = TRUE;
        }
        break;
    case 2:
        condition = mailGetSendCondition(seqHandle);
        if (condition != 0xFFFFFFFF && condition != 0 &&
            cmsRunSync(condition, 0) != 0) {
            eligible = TRUE;
        }
        break;
    }
    return eligible;
}

/**
 * cbStep - Waza multi-hit effect setup.
 * Address: 0x801D29D8 | Size: 0x130
 */
void cbStep(s32 moveID, s32 hitCount) {
    extern f32 GSrandf(void);
    f32 step;
    s32 count;
    s32 i;
    s32 sent;

    step = GSrandf();
    count = mailGetNbMailData();
    sent = 0;
    for (i = 0; i < count; i++) {
        if (chkMailSend(i, step)) {
            mailAddMailbox(i);
            sent = 1;
        }
    }
    if (sent && lbl_80467390[0] == 0) {
        mailMainStartSe();
    }
}

/**
 * mailMainInit - Waza register multi-hit callback and clear state.
 * Address: 0x801D2B08 | Size: 0x44
 */
void mailMainInit(void) {
    s32* state;

    heroMoveAddStepCallback(cbStep, 0);
    state = lbl_80467390;
    state[0] = 0;
    state[2] = 0;
    state[3] = 0;
}
