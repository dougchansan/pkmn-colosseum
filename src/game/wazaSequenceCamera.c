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
#include "dolphin/mtx.h"

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

    for (i = 0; i < count; i++, table++) {
        end += table->duration;
        if (frame < end) {
            return table;
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
extern u32 lbl_8047B3E8;
extern s32 lbl_8047B410;
extern f32 lbl_80478CDC;

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
    typedef struct CameraFovKey {
        f32 start;
        f32 end;
        u32 startFrame;
        u32 endFrame;
    } CameraFovKey;
    extern CameraFovKey lbl_804673D4[];
    extern u8 lbl_8047B3F4;
    extern u32 fn_800D3088(void);
    extern void* GSmodelGetPart(void*, u32);
    extern void GSpartGetTransform(void*, Vec*, void*, void*);
    extern void GSpartFree(void*);
    extern void GSmodelGetPosition(void*, Vec*);
    extern void GSscene_GetCameraPositionVector(Vec*);
    extern void fn_800E0168(Vec*, Vec*, Vec*);
    extern void cameraMoveTargetOfs(s32, Vec*, f32);
    extern f32 GSlerpGetLinearInterpolationFloat(f32, f32, f32);
    extern void cameraSetFov(f32);
    extern BOOL cameraMoveEndCheckSpecial(s32);
    extern f32 lbl_8047E1E0;
    u8* sequence;
    u8* model;
    CameraFovKey* key;
    Vec target;
    Vec origin;
    void* part;
    f32 t;
    u32 frame;
    u32 partId;
    u32 i;

    if (lbl_8047B3F4 == 0 || lbl_8047B3EC == NULL) {
        return;
    }

    if ((u8*)lbl_8047B3F0 != NULL && *(u32*)((u8*)lbl_8047B3F0 + 0x18) != 0 &&
        *(u32*)((u8*)lbl_8047B3F0 + 0x20) != 0) {
        return;
    }

    frame = fn_800D3088();
    sequence = lbl_8047B3EC;
    model = *(u8**)(sequence + 0x24);
    key = (CameraFovKey*)(*(u8**)(sequence + 0x2C) +
                          *(u16*)(sequence + 0x32) * 0xD4);
    if ((u8*)lbl_8047B3F0 != NULL) {
        partId = *(u32*)((u8*)key + 0x4C + ((u8*)lbl_8047B3F0)[0x17] * 4);
    } else {
        partId = *(u32*)((u8*)key + 0x54);
    }
    part = GSmodelGetPart(model, partId);
    if (part != NULL) {
        GSpartGetTransform(part, &target, NULL, NULL);
        if ((u8*)lbl_8047B3F0 != NULL && *(u16*)((u8*)lbl_8047B3F0 + 0x2E) == 3 &&
            (*(u16*)((u8*)lbl_8047B3F0 + 0x2C) == 0x154 ||
             *(u16*)((u8*)lbl_8047B3F0 + 0x2C) == 0x6E)) {
            GSscene_GetCameraPositionVector(&origin);
        } else {
            GSmodelGetPosition(*(u8**)((u8*)lbl_8047B3EC + 0x24), &origin);
        }
        fn_800E0168(&target, &target, &origin);
        cameraMoveTargetOfs(7, &target, lbl_8047E1E0);
        GSpartFree(part);
    }

    lbl_8047B3E8 += frame;
    key = lbl_804673D4;
    for (i = 0; i < 2; i++, key++) {
        if (lbl_8047B3E8 <= key->startFrame) {
            lbl_80478CDC = key->start;
            break;
        }
        if (lbl_8047B3E8 <= key->endFrame) {
            t = (f32)(lbl_8047B3E8 - key->startFrame) /
                (f32)(key->endFrame - key->startFrame);
            lbl_80478CDC =
                GSlerpGetLinearInterpolationFloat(key->start, key->end, t);
            break;
        }
        lbl_80478CDC = key->end;
    }
    cameraSetFov(lbl_80478CDC);

    if (lbl_8047B3F0 == NULL && !cameraMoveEndCheckSpecial(0)) {
        GSscene_SetMode(8);
        fn_801765F4(0);
        lbl_8047B3EC = NULL;
        if (fn_800057A8() == 2) {
            GSscene_SetMode(2);
        }
    }
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
void _wazaSequenceCameraDoDollyPosition__FP21TemplateExpFileHeaderP24wazaSequenceCameraParamsfb(
    void* header, void* params, s32 shift, u8 reverse)
{
    typedef struct WazaCameraParams {
        u8 pad_00[0x0C];
        f32 rotationBase;
        f32 rotationRange;
        f32 rotationOffset;
        f32 heightMin;
        f32 heightMax;
        f32 distanceMin;
        f32 distanceMax;
        f32 nearDistance;
        f32 middleDistance;
        f32 farDistance;
        f32 nearLength;
        f32 middleLength;
        f32 farLength;
    } WazaCameraParams;
    extern f32 fn_800E0BE4();
    extern u32 _fadeEffectGetRandom__FUl(u32);
    extern void cameraSetDistance(f32);
    extern void cameraSetHeight(f32);
    extern void cameraSetRotY(f32);
    extern void cameraUpdate(void);
    extern void GSscene_GetCameraDirectionVector(Vec*);
    extern void GSscene_GetCameraPositionVector(Vec*);
    extern void fn_800E0168(Vec*, Vec*, Vec*);
    extern void fn_800E013C(Vec*, Vec*, f32);
    extern void GSvecAdd(Vec*, Vec*, Vec*);
    extern s32 fn_800D37CC(void);
    extern void cameraMovePosition(s32, Vec*, f32);
    extern f32 sqrtf(f32);
    extern f32 lbl_8047E1F8;
    extern f32 lbl_8047E1FC;
    extern f32 lbl_8047E260;
    extern f32 lbl_8047E264;
    extern f32 lbl_8047E268;
    extern f32 lbl_8047E26C;
    u8* file = header;
    WazaCameraParams* camera = params;
    Vec direction;
    Vec position;
    s32 duration;
    s32 delay;
    s32 randomDelay;
    f32 threshold0 = lbl_8047E1F8;
    f32 threshold1 = lbl_8047E260;
    f32 distance0;
    f32 distance1;
    f32 nearDistance;
    f32 farDistance;
    f32 height;
    f32 rotation;
    BOOL alternate;

    duration = *(s32*)(file + (*(s32*)file << 2) + 4);
    if (duration == 0) {
        duration = *(s32*)(file + (*(s32*)file << 2) + 8);
    }
    duration <<= shift;

    if (fn_800E0BE4() <= lbl_8047E264) {
        threshold0 = lbl_8047E268;
        threshold1 = lbl_8047E26C;
        alternate = TRUE;
    } else {
        alternate = FALSE;
    }

    if (fn_800E0BE4() < threshold0) {
        randomDelay = 0;
    } else if (fn_800E0BE4() < threshold1) {
        s32 minimumDuration = duration >> 1;

        randomDelay = 0;
        duration = _fadeEffectGetRandom__FUl(duration);
        if (duration < minimumDuration) {
            duration = minimumDuration;
        }
    } else {
        randomDelay = _fadeEffectGetRandom__FUl(duration);
        if (randomDelay > (duration >> 1)) {
            randomDelay = duration >> 1;
        }
    }

    distance0 = camera->nearDistance +
        (camera->middleDistance - camera->nearDistance) * fn_800E0BE4();
    distance1 = camera->nearDistance +
        (camera->middleDistance - camera->nearDistance) * fn_800E0BE4();
    nearDistance = distance0;
    farDistance = distance1;
    if (alternate) {
        if (distance0 < distance1) {
            nearDistance = distance1;
            farDistance = distance0;
        }
    } else if (distance0 > distance1) {
        nearDistance = distance1;
        farDistance = distance0;
    }

    height = camera->heightMin +
        (camera->heightMax - camera->heightMin) * fn_800E0BE4();
    if (reverse) {
        rotation = -((camera->rotationRange - camera->rotationBase) *
                     fn_800E0BE4() -
                     (camera->rotationOffset - camera->rotationBase));
    } else {
        rotation = (camera->rotationRange - camera->rotationBase) *
                       fn_800E0BE4() +
                   (camera->rotationOffset + camera->rotationBase);
    }

    cameraSetDistance(nearDistance);
    cameraSetHeight(height);
    cameraSetRotY(rotation);
    cameraUpdate();
    GSscene_GetCameraDirectionVector(&direction);
    GSscene_GetCameraPositionVector(&position);
    fn_800E0168(&direction, &direction, &position);
    direction.z = lbl_8047E1FC;
    fn_800E013C(&direction, &direction, farDistance / nearDistance);
    direction.z = height;
    GSvecAdd(&direction, &direction, &position);
    delay = duration - randomDelay;
    cameraMovePosition(7, &direction, (f32)delay / (f32)fn_800D37CC());

    camera->nearLength = sqrtf(nearDistance * nearDistance + height * height);
    distance0 = lbl_8047E1F8 * (nearDistance + farDistance);
    camera->middleLength = sqrtf(distance0 * distance0 + height * height);
    camera->farLength = sqrtf(farDistance * farDistance + height * height);
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
s32 _wazaSequenceCameraSelectDuration__FUcPff(
    u8 mode, f32* thresholds, s32 duration)
{
    extern f32 fn_800E0BE4();
    f32 random;

    if (mode == 1) {
        return duration < 12 ? duration : 12;
    }
    if (mode == 2) {
        return duration < 20 ? duration : 20;
    }
    if (mode == 4) {
        return duration < 45 ? duration : 45;
    }
    if (mode == 8) {
        return duration;
    }
    if (mode == 3) {
        random = fn_800E0BE4();
        return random < thresholds[0]
                   ? (duration < 12 ? duration : 12)
                   : (duration < 20 ? duration : 20);
    }
    if (mode == 5) {
        random = fn_800E0BE4();
        return random < thresholds[0]
                   ? (duration < 12 ? duration : 12)
                   : (duration < 45 ? duration : 45);
    }
    if (mode == 9) {
        random = fn_800E0BE4();
        return random < thresholds[0]
                   ? (duration < 12 ? duration : 12)
                   : duration;
    }
    if (mode == 6) {
        random = fn_800E0BE4();
        return random < thresholds[1]
                   ? (duration < 20 ? duration : 20)
                   : (duration < 45 ? duration : 45);
    }
    if (mode == 10) {
        random = fn_800E0BE4();
        return random < thresholds[1]
                   ? (duration < 20 ? duration : 20)
                   : duration;
    }
    if (mode == 12) {
        random = fn_800E0BE4();
        return random < thresholds[2]
                   ? (duration < 45 ? duration : 45)
                   : duration;
    }
    if (mode == 7) {
        random = fn_800E0BE4();
        if (random < thresholds[0]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[1]) {
            return duration < 20 ? duration : 20;
        }
        return duration < 45 ? duration : 45;
    }
    if (mode == 11) {
        random = fn_800E0BE4();
        if (random < thresholds[0]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[1]) {
            return duration < 20 ? duration : 20;
        }
        return duration;
    }
    if (mode == 13) {
        random = fn_800E0BE4();
        if (random < thresholds[0]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[2]) {
            return duration < 45 ? duration : 45;
        }
        return duration;
    }
    if (mode == 14) {
        random = fn_800E0BE4();
        if (random < thresholds[1]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[2]) {
            return duration < 20 ? duration : 20;
        }
        return duration;
    }
    if (mode == 15) {
        random = fn_800E0BE4();
        if (random < thresholds[0]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[1]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[2]) {
            return duration < 20 ? duration : 20;
        }
    }
    return duration;
}

/**
 * _wazaSequenceCameraSelectMotion__FP13ModelSequenceP12WazaSequenceP24wazaSequenceCameraParams - Move animation helper: particle burst.
 * Address: 0x801D4DA0 | Size: 0x218
 */
void _wazaSequenceCameraSelectMotion__FP13ModelSequenceP12WazaSequenceP24wazaSequenceCameraParams(
    void* modelSequence, void* wazaSequence, void* cameraParams)
{
    extern f32 fn_800E0BE4(u8, s32);
    extern s32 lbl_80478CD8;
    extern const f32 lbl_8047E1F4;
    u32 flags;
    u8 option0;
    u8 option1;
    u8 option2;
    u8 option3;
    s32 count;
    s32* motion;
    f32 interval;

    motion = cameraParams;
    if (*(u16*)((u8*)modelSequence + 0x70) == 0x13A) {
        *motion = 4;
        return;
    }

    count = 0;
    option0 = option1 = option2 = option3 = 0;
    if (wazaSequence != NULL) {
        flags = *(u32*)((u8*)wazaSequence + 8);
        if ((flags & 1) != 0) {
            *motion = 5;
            return;
        }
        option0 = (u8)((flags >> 3) & 1);
        if (option0 != 0) {
            count++;
        }
        option1 = (u8)((flags >> 4) & 1);
        if (option1 != 0) {
            count++;
        }
        option2 = (u8)((flags >> 5) & 1);
        if (option2 != 0) {
            count++;
        }
        option3 = (u8)((flags >> 6) & 1);
        if (option3 != 0) {
            count++;
        }
    }

    if (count == 0) {
        count = 4;
        option0 = option1 = option2 = option3 = 1;
    } else if (count == 1) {
        if (option0) {
            *motion = 3;
        }
        if (option1) {
            *motion = 0;
        }
        if (option2) {
            *motion = 1;
        }
        if (option3) {
            *motion = 2;
        }
        lbl_80478CD8 = *motion;
        return;
    }

    interval = lbl_8047E1F4 / (f32)count;
    for (;;) {
        f32 random;
        f32 limit;

        random = fn_800E0BE4(0, 0);
        limit = interval;
        if (option0) {
            if (random < limit) {
                *motion = 3;
                if (lbl_80478CD8 != *motion) {
                    break;
                }
            }
            limit += interval;
        }
        if (option1) {
            if (random < limit) {
                *motion = 0;
                if (lbl_80478CD8 != *motion) {
                    break;
                }
            }
            limit += interval;
        }
        if (option2) {
            if (random < limit) {
                *motion = 1;
                if (lbl_80478CD8 != *motion) {
                    break;
                }
            }
            limit += interval;
        }
        if (option3 && random < limit) {
            *motion = 2;
            if (lbl_80478CD8 != *motion) {
                break;
            }
        }
    }
    lbl_80478CD8 = *motion;
}

/**
 * _wazaSequenceCameraCalculateParams__FP13ModelSequenceiP24wazaSequenceCameraParams - Move animation helper: model projectile.
 * Address: 0x801D4FB8 | Size: 0x370
 */
void _wazaSequenceCameraCalculateParams__FP13ModelSequenceiP24wazaSequenceCameraParams(
    void* modelSequence, s32 flags, void* cameraParams)
{
    extern void* GSmodelGetBound(void*);
    extern void GSmodelGetRotation(void*, Vec*);
    extern f32 fn_800E008C(Vec*);
    extern f32 lbl_8047E1F4, lbl_8047E1FC, lbl_8047E200, lbl_8047E20C;
    extern f32 lbl_8047E240, lbl_8047E244, lbl_8047E248, lbl_8047E24C;
    extern f32 lbl_8047E250, lbl_8047E288, lbl_8047E28C, lbl_8047E290;
    extern f32 lbl_8047E294, lbl_8047E298, lbl_8047E29C, lbl_8047E2A0;
    extern f32 lbl_8047E2A4, lbl_8047E2A8, lbl_8047E2AC, lbl_8047E2B0;
    extern f32 lbl_8047E2B4, lbl_8047E2B8, lbl_8047E2BC, lbl_8047E2C0;
    extern f32 lbl_8047E2C4, lbl_8047E2C8, lbl_8047E2CC, lbl_8047E2D0;
    extern f32 lbl_8047E2D4, lbl_8047E2D8;
    u8* sequence = modelSequence;
    u8* params = cameraParams;
    void* model = *(void**)(sequence + 0x24);
    u8* bound = GSmodelGetBound(model);
    Vec rotation;
    f32 distanceScale;
    f32 sizeScale;
    f32 lower;
    f32 upper;
    f32 magnitude;
    s32 mode;

    GSmodelGetRotation(model, &rotation);
    *(f32*)(params + 0x14) = rotation.y;
    mode = *(s32*)(sequence + 0x10);
    switch (mode) {
    case -2:
        sizeScale = lbl_8047E288;
        distanceScale = lbl_8047E240;
        break;
    case -1:
        sizeScale = lbl_8047E288;
        distanceScale = lbl_8047E240;
        break;
    case 1:
        sizeScale = lbl_8047E28C;
        distanceScale = lbl_8047E244;
        break;
    case 2:
        sizeScale = lbl_8047E290;
        distanceScale = lbl_8047E248;
        break;
    case 3:
        sizeScale = lbl_8047E294;
        distanceScale = lbl_8047E24C;
        break;
    default:
        sizeScale = lbl_8047E298;
        distanceScale = lbl_8047E1F4;
        break;
    }

    if (flags & 1) {
        *(f32*)(params + 0x0C) = lbl_8047E1FC;
        *(f32*)(params + 0x10) = lbl_8047E29C;
    } else if (flags & 2) {
        *(f32*)(params + 0x0C) = lbl_8047E2A0;
        *(f32*)(params + 0x10) = lbl_8047E2A4;
    } else if (flags & 4) {
        if (mode > 0) {
            *(f32*)(params + 0x0C) = lbl_8047E2A0;
            *(f32*)(params + 0x10) = lbl_8047E2A4;
        } else {
            *(f32*)(params + 0x0C) = lbl_8047E2A4;
            *(f32*)(params + 0x10) = lbl_8047E2A8;
        }
    } else if (flags & 8) {
        if (mode > 0) {
            *(f32*)(params + 0x0C) = lbl_8047E2A4;
            *(f32*)(params + 0x10) = lbl_8047E2A8;
        } else {
            *(f32*)(params + 0x0C) = lbl_8047E2A8;
            *(f32*)(params + 0x10) = lbl_8047E2AC;
        }
    } else if (flags & 0x10) {
        *(f32*)(params + 0x0C) = lbl_8047E2AC;
        *(f32*)(params + 0x10) = lbl_8047E2B0;
    } else if (mode > 0) {
        *(f32*)(params + 0x0C) = lbl_8047E2A0;
        *(f32*)(params + 0x10) = lbl_8047E2A8;
    } else {
        *(f32*)(params + 0x0C) = lbl_8047E2A0;
        *(f32*)(params + 0x10) = lbl_8047E2B4;
    }

    if (flags & 0x20) {
        *(f32*)(params + 0x20) = lbl_8047E200;
        *(f32*)(params + 0x24) = lbl_8047E2C0;
        lower = lbl_8047E2B8;
        upper = lbl_8047E2BC;
    } else if (flags & 0x40) {
        *(f32*)(params + 0x20) = lbl_8047E2C0;
        *(f32*)(params + 0x24) = lbl_8047E250;
        lower = lbl_8047E2B8;
        upper = lbl_8047E2C4;
    } else if (flags & 0x80) {
        *(f32*)(params + 0x20) = lbl_8047E250;
        *(f32*)(params + 0x24) = lbl_8047E2C8;
        lower = lbl_8047E2B8;
        upper = lbl_8047E20C;
    } else {
        *(f32*)(params + 0x20) = lbl_8047E20C;
        *(f32*)(params + 0x24) = lbl_8047E2C8;
        lower = lbl_8047E2B8;
        upper = lbl_8047E20C;
    }

    *(f32*)(params + 0x20) *= distanceScale;
    *(f32*)(params + 0x24) *= distanceScale;
    magnitude = fn_800E008C((Vec*)(bound + 0x28));
    *(f32*)(params + 0x04) = lbl_8047E2CC * magnitude;
    *(f32*)(params + 0x08) = *(f32*)(bound + 0x2C);
    *(f32*)(params + 0x18) = *(f32*)(bound + 0x14);
    *(f32*)(params + 0x1C) = *(f32*)(bound + 0x20);

    if (*(f32*)(params + 0x18) < lower) {
        *(f32*)(params + 0x18) = lower;
        if (*(f32*)(params + 0x1C) < lower) {
            *(f32*)(params + 0x1C) = lbl_8047E294 * lower;
        }
    }
    if (*(f32*)(params + 0x1C) > upper) {
        *(f32*)(params + 0x1C) = upper;
        if (*(f32*)(params + 0x18) > upper) {
            *(f32*)(params + 0x18) = lbl_8047E2D0 * upper;
        }
    }
    *(f32*)(params + 0x04) *= sizeScale;
    *(f32*)(params + 0x08) *= sizeScale;
    if (*(f32*)(params + 0x20) < lbl_8047E20C) {
        *(f32*)(params + 0x20) = lbl_8047E20C;
    }
    if (*(f32*)(params + 0x24) < lbl_8047E2D4) {
        *(f32*)(params + 0x24) = lbl_8047E2D4;
    }
    if (*(f32*)(params + 0x20) > lbl_8047E2D8) {
        *(f32*)(params + 0x20) = lbl_8047E2D8;
    }
    if (*(f32*)(params + 0x24) > lbl_8047E2C8) {
        *(f32*)(params + 0x24) = lbl_8047E2C8;
    }
}
