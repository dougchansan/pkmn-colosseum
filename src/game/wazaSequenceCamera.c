/**
 * @file wazaSequenceCamera.c
 * @brief wazaSequenceCamera / battleCamera: camera control during move animation
 * playback (front block is battleCamera code -- kept in this file per
 * split spec, not sub-split further).
 *
 * Split from the former game/battle/battle_waza.c CodeCandidate bucket
 * (0x801D1470-0x801DE698); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit. Shared typedefs and cross-TU
 * forward declarations live in include/game/battle/battle_waza_types.h.
 */

#include "game/battle/battle_waza_types.h"

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
 * wazaSequenceCameraGetPattern__Fbi - Waza multi-hit advance.
 * Address: 0x801D2B4C | Size: 0x120
 */
void* wazaSequenceCameraGetPattern__Fbi(u8 shortTable, s32 flags) {
    typedef struct WazaCameraPattern {
        f32 duration;
        u8 data[0x48];
    } WazaCameraPattern;
    extern f32 fn_800E0BE4(u8, s32);
    extern u32 _fadeEffectGetRandom__FUl(u32);
    extern WazaCameraPattern lbl_80371F60[];
    extern WazaCameraPattern lbl_803721C0[];
    WazaCameraPattern* table;
    s32 count;
    s32 i;
    f32 frame = fn_800E0BE4(shortTable, flags);
    f32 end = 0.0f;

    if (shortTable != 0) {
        table = lbl_80371F60;
        count = 8;
        if (flags & 0x20) {
            return &table[4];
        }
        if (flags & 0x40) {
            return &table[5];
        }
        if (flags & 0x80) {
            return &table[6];
        }
    } else {
        table = lbl_803721C0;
        count = 13;
        if (flags & 0x20) {
            return &table[9];
        }
        if (flags & 0x40) {
            return &table[10];
        }
        if (flags & 0x80) {
            return &table[11];
        }
    }

    for (i = 0; i < count; i++) {
        end += table[i].duration;
        if (frame < end) {
            return &table[i];
        }
    }

    if (shortTable != 0) {
        return &lbl_80371F60[_fadeEffectGetRandom__FUl(8)];
    }
    return &lbl_803721C0[_fadeEffectGetRandom__FUl(13)];
}

/**
 * fn_801D2C6C - Waza get global state from SDA.
 * Address: 0x801D2C6C | Size: 0x8
 */
extern void* lbl_8047B3EC;
extern void* lbl_8047B3F0;
extern s32 lbl_8047B410;

extern u8 lbl_80467CC0[];
extern void fn_801DD158(void* obj);
extern void fn_801DF1D0(void* obj);
extern void fn_801DD3E4(void* obj);
extern void fn_801DD23C(void* obj);
extern void _threadSwitch(void);
extern s32 fn_8017B2CC(s32 id);
extern void fn_800F915C(s32 id);
extern void fn_8017B1CC(s32 id);
void* fn_801D2C6C(void) {
    return lbl_8047B3EC;
}

/* =========================================================================
 * WAZA ANIMATION STATE MACHINES (0x801D2C74 - 0x801D7230)
 *
 * Large state machines that drive multi-step move animations.
 * These contain extensive float math and switch statements.
 * ========================================================================= */

/**
 * fn_801D2C74 - Waza animation pre-check.
 * Address: 0x801D2C74 | Size: 0xB4
 */
void fn_801D2C74(void* owner) {
    extern void GSscene_SetMode(s32 arg);
    extern void cameraStopAnime(void* arg);
    extern void fn_801765F4(s32 arg);
    extern s32  fn_800057A8(void);
    extern u8   lbl_8047B3F4;

    void* obj;

    if (lbl_8047B3F4 != 0) {
        obj = lbl_8047B3F0;
        if (obj == NULL) {
            if (lbl_8047B3EC != NULL) {
                if (obj == NULL) {
                    GSscene_SetMode(8);
                } else {
                    if (*(u32*)((u8*)obj + 0x18) != 0 && *(u32*)((u8*)obj + 0x20) != 0) {
                        cameraStopAnime(obj);
                    }
                    lbl_8047B3F0 = NULL;
                }
                fn_801765F4(0);
                lbl_8047B3EC = NULL;
                if (fn_800057A8() == 2) {
                    GSscene_SetMode(2);
                }
            }
            battleCameraStartWaza(owner, NULL);
        }
    }
}

/**
 * fn_801D2D28 - Waza animation setup from move data.
 * Address: 0x801D2D28 | Size: 0x26C
 */
void fn_801D2D28(void) {
    /* TODO: Waza animation setup (0x26C bytes) */
}

/**
 * fn_801D2F94 - Waza animation teardown.
 * Address: 0x801D2F94 | Size: 0x88
 */
extern void GSscene_SetMode(s32 arg);
extern void cameraStopAnime(void* arg);
extern void fn_801765F4(s32 arg);
extern s32  fn_800057A8(void);
void fn_801D2F94(void) {
    void* obj;
    if (lbl_8047B3EC != NULL) {
        obj = lbl_8047B3F0;
        if (obj == NULL) {
            GSscene_SetMode(8);
        } else {
            if (*(u32*)((u8*)obj + 0x18) != 0 && *(u32*)((u8*)obj + 0x20) != 0) {
                cameraStopAnime(obj);
            }
            lbl_8047B3F0 = NULL;
        }
        fn_801765F4(0);
        lbl_8047B3EC = NULL;
        if (fn_800057A8() == 2) {
            GSscene_SetMode(2);
        }
    }
}

/**
 * fn_801D301C - Waza animation reset: set lbl_8047B3F4=1, lbl_8047B3EC=0, lbl_8047B3F0=0.
 * Address: 0x801D301C | Size: 0x18
 */
extern u8  lbl_8047B3F4;
void fn_801D301C(void) {
    lbl_8047B3F4 = 1;
    lbl_8047B3EC = NULL;
    lbl_8047B3F0 = NULL;
}

/**
 * fn_801D3034 - Waza animation frame step.
 * Address: 0x801D3034 | Size: 0x88
 */
void fn_801D3034(void* state) {
    void* obj;
    if (state == lbl_8047B3EC) {
        obj = lbl_8047B3F0;
        if (obj == NULL) {
            GSscene_SetMode(8);
        } else {
            if (*(u32*)((u8*)obj + 0x18) != 0 && *(u32*)((u8*)obj + 0x20) != 0) {
                cameraStopAnime(obj);
            }
            lbl_8047B3F0 = NULL;
        }
        fn_801765F4(0);
        lbl_8047B3EC = NULL;
        if (fn_800057A8() == 2) {
            GSscene_SetMode(2);
        }
    }
}

/**
 * battleCameraStartWaza - Waza animation state machine dispatcher.
 * Address: 0x801D30BC | Size: 0x3E0
 */
void battleCameraStartWaza(void* owner, void* sequence) {
    /* TODO: Waza animation state dispatcher (0x3E0 bytes)
     * Dispatches to type-specific animation handlers based on move type.
     */
}

/**
 * _wazaSequenceCameraDoPosition__FP13ModelSequenceP24wazaSequenceCameraParamsfb - Move animation state machine A.
 * Address: 0x801D349C | Size: 0xAE0
 * Massive state machine (~2.8KB) for a class of move animations.
 * Likely handles physical/contact move animations.
 */
void _wazaSequenceCameraDoPosition__FP13ModelSequenceP24wazaSequenceCameraParamsfb(void) {
    /* TODO: Move animation state machine A (0xAE0 bytes) */
}

/**
 * _wazaSequenceCameraDoDollyPosition__FP21TemplateExpFileHeaderP24wazaSequenceCameraParamsfb - Move animation state machine B.
 * Address: 0x801D3F7C | Size: 0x548
 * State machine for beam/projectile move animations.
 */
void _wazaSequenceCameraDoDollyPosition__FP21TemplateExpFileHeaderP24wazaSequenceCameraParamsfb(void) {
    /* TODO: Move animation state machine B (0x548 bytes) */
}

/**
 * _wazaSequenceCameraDoFOV__FP13ModelSequenceP24wazaSequenceCameraParamsif - Move animation state machine C.
 * Address: 0x801D44C4 | Size: 0x514
 * State machine for status/field effect move animations.
 */
void _wazaSequenceCameraDoFOV__FP13ModelSequenceP24wazaSequenceCameraParamsif(void) {
    /* TODO: Move animation state machine C (0x514 bytes) */
}

/**
 * _wazaSequenceCameraSelectDuration__FUcPff - Move animation state machine D.
 * Address: 0x801D49D8 | Size: 0x3C8
 * State machine for spread/multi-target move animations.
 */
void _wazaSequenceCameraSelectDuration__FUcPff(void) {
    /* TODO: Move animation state machine D (0x3C8 bytes) */
}

/**
 * _wazaSequenceCameraSelectMotion__FP13ModelSequenceP12WazaSequenceP24wazaSequenceCameraParams - Move animation helper: particle burst.
 * Address: 0x801D4DA0 | Size: 0x218
 */
void _wazaSequenceCameraSelectMotion__FP13ModelSequenceP12WazaSequenceP24wazaSequenceCameraParams(s32 effectID, s32 slot) {
    /* TODO: Particle burst helper (0x218 bytes) */
}

/**
 * _wazaSequenceCameraCalculateParams__FP13ModelSequenceiP24wazaSequenceCameraParams - Move animation helper: model projectile.
 * Address: 0x801D4FB8 | Size: 0x370
 */
void _wazaSequenceCameraCalculateParams__FP13ModelSequenceiP24wazaSequenceCameraParams(s32 modelID, s32 attackerSlot, s32 targetSlot) {
    /* TODO: Model projectile helper (0x370 bytes) */
}
