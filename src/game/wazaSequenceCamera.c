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
#include "crt/math_ppc.h"
#include "dolphin/mtx.h"
#include "game/battle/waza_camera_stop.h"

/* Camera parameters computed per move (lbl_804673A0, 0x34 bytes). */
typedef struct WazaSequenceCameraParams {
    s32 motion;         /* 0x00 */
    f32 size;           /* 0x04 */
    f32 height;         /* 0x08 */
    f32 rotationMin;    /* 0x0C */
    f32 rotationMax;    /* 0x10 */
    f32 rotationBase;   /* 0x14 */
    f32 heightMin;      /* 0x18 */
    f32 heightMax;      /* 0x1C */
    f32 distanceMin;    /* 0x20 */
    f32 distanceMax;    /* 0x24 */
    f32 nearLength;     /* 0x28 */
    f32 middleLength;   /* 0x2C */
    f32 farLength;      /* 0x30 */
} WazaSequenceCameraParams;


/**
 * wazaSequenceCameraGetPattern__Fbi - Waza multi-hit advance.
 * Address: 0x801D2B4C | Size: 0x120
 */
#ifndef WAZA_CAMERA_FILE_ONLY
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
#endif

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
#ifndef WAZA_CAMERA_FILE_ONLY
void* fn_801D2C6C(void) {
    return lbl_8047B3EC;
}
#endif

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
#ifndef WAZA_CAMERA_FILE_ONLY
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
#endif

/**
 * fn_801D2D28 - Waza animation setup from move data.
 * Address: 0x801D2D28 | Size: 0x26C
 */
void fn_801D2D28(void)
{
    typedef struct CameraFovKey {
        f32 start;
        f32 end;
        u32 startFrame;
        u32 endFrame;
    } CameraFovKey;
    typedef struct WazaCameraMove {
        u8 pad00[0x4C];
        u32 targetParts[3];
        u8 pad58[0x7C];
    } WazaCameraMove;
    typedef struct WazaCameraSequence {
        u8 pad00[0x24];
        void* model;
        u8 pad28[4];
        WazaCameraMove* moves;
        u16 pad30;
        u16 moveIndex;
    } WazaCameraSequence;
    extern CameraFovKey lbl_804673D4[];
    extern u8 lbl_8047B3F4;
    extern u32 lbl_8047B3E8;
    extern f32 lbl_80478CDC;
    extern u32 GSgetFrameStep(void);
    extern void* GSmodelGetPart(void*, u32);
    extern void GSpartGetTransform(void*, Vec*, void*, void*);
    extern void GSpartFree(void*);
    extern void GSmodelGetPosition(void*, Vec*);
    extern void GSscene_GetCameraPositionVector(Vec*);
    extern void GSvecSub(Vec*, Vec*, Vec*);
    extern void cameraMoveTargetOfs(s32, Vec*, f32);
    extern f32 GSlerpGetLinearInterpolationFloat(f32, f32, f32);
    extern void cameraSetFov(f32);
    extern u32 cameraMoveEndCheckSpecial(u8);
    CameraFovKey* key = lbl_804673D4;
    WazaCameraMove* move;
    void* model;
    Vec target;
    Vec origin;
    void* part;
    f32 t;
    u32 frame;
    u32 partId;
    u32 i;

    if (lbl_8047B3F4 != 0 && lbl_8047B3EC != NULL &&
        (lbl_8047B3F0 == NULL || *(u32*)((u8*)lbl_8047B3F0 + 0x18) == 0 ||
         *(u32*)((u8*)lbl_8047B3F0 + 0x20) == 0)) {
        frame = GSgetFrameStep();
        move = &((WazaCameraSequence*)lbl_8047B3EC)->moves[
            ((WazaCameraSequence*)lbl_8047B3EC)->moveIndex];
        model = ((WazaCameraSequence*)lbl_8047B3EC)->model;
        if (lbl_8047B3F0 != NULL) {
            partId = move->targetParts[((u8*)lbl_8047B3F0)[0x17]];
        } else {
            partId = move->targetParts[2];
        }
        part = GSmodelGetPart(model, partId);
        if (part != NULL) {
            GSpartGetTransform(part, &target, NULL, NULL);
            if (lbl_8047B3F0 != NULL && *(u16*)((u8*)lbl_8047B3F0 + 0x2E) == 3 &&
                (*(u16*)((u8*)lbl_8047B3F0 + 0x2C) == 0x154 ||
                 *(u16*)((u8*)lbl_8047B3F0 + 0x2C) == 0x6E)) {
                GSscene_GetCameraPositionVector(&origin);
            } else {
                GSmodelGetPosition(*(void**)((u8*)lbl_8047B3EC + 0x24), &origin);
            }
            GSvecSub(&target, &target, &origin);
            cameraMoveTargetOfs(7, &target, 0.2f);
            GSpartFree(part);
        }

        lbl_8047B3E8 += frame;
        for (i = 0; i < 2; i++, key++) {
            if (lbl_8047B3E8 <= key->startFrame) {
                lbl_80478CDC = key->start;
                break;
            }
            if (lbl_8047B3E8 <= key->endFrame) {
                t = (f32)(lbl_8047B3E8 - key->startFrame) /
                    (f32)(key->endFrame - key->startFrame);
                lbl_80478CDC = GSlerpGetLinearInterpolationFloat(key->start, key->end, t);
                break;
            }
            lbl_80478CDC = key->end;
        }
        cameraSetFov(lbl_80478CDC);

        if (lbl_8047B3F0 == NULL && (u8)cameraMoveEndCheckSpecial(0) == 0) {
            /* RULE-EXCEPTION(title-path): redundant status temporary preserves
             * retail's li/cmpwi/bne; see docs/RULE_EXCEPTIONS.md. */
            s32 moving = 0;
            if (moving == 0) {
                wazaCameraStop();
            }
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
    extern void* GSresGetResource(u32 group, u32 resource);
    extern void clear__5GSvecFv(void* vec);
    extern void GSlerpGetLinearInterpolationVector(void* dst, void* src,
                                                   void* target, f32 t);
    extern u8 lbl_804673A0[];
    extern u16 battleGridGetNumPokemonsForTrainer(u32 id);
    extern void cameraSetTargetExt(u32 a, u32 b, u32 c);
    extern void cameraRefreshTargetPos(void);
    extern void cameraMoveStop(void);
    extern void cameraSetOffsetPosition(void* src);
    extern void cameraSetOffsetRotation(void* src);
    extern void cameraSetOffsetScale(void* src);
    extern void cameraPlayOffsetAnime(u32 groupId, u32 animationId,
                                      s32 frame, u8 loop);
    extern void* GSmodelGetBound(void* model);
    extern void GSmodelGetPosition(void* model, Vec* out);
    extern void GSmodelGetRotation(void* model, Vec* out);
    extern void fn_800D1070(s32 mode);
    u8* ownerBytes = owner;
    u8* sequenceBytes = sequence;
    Vec offsetPosition;
    Vec offsetRotation;
    Vec offsetScale;
    Vec center;
    Vec rootPosition;
    Vec ownerPosition;
    void* model;
    u8* bound;
    u8 reverse = FALSE;
    s32 paramsFlags = 0;
    s32 shift = 0;
    /* RULE-EXCEPTION(user-approved): separate shift for the offset-anime path, chosen for register allocation - see docs/RULE_EXCEPTIONS.md */
    s32 animShift;
    f32 scaleValue;

    if (lbl_8047B3F4 == 0) {
        return;
    }

    if (*(s8*)(ownerBytes + 0x76) < 0 &&
        (sequenceBytes == NULL || ((*(s32*)(sequenceBytes + 0x08) & 0x80) != 0x80))) {
        reverse = TRUE;
    }

    lbl_8047B3EC = owner;
    lbl_8047B3F0 = sequence;
    if (sequenceBytes != NULL) {
        battleCameraDisable();
        if (*(u32*)(sequenceBytes + 0x18) != 0 &&
            *(u32*)(sequenceBytes + 0x20) != 0 &&
            GSresGetResource(*(u32*)(sequenceBytes + 0x18),
                             *(u32*)(sequenceBytes + 0x20)) != NULL) {
            model = *(void**)(ownerBytes + 0x24);
            animShift = 0;
            if (reverse) {
                animShift = 4;
            }

            cameraPlayOffsetAnime(
                *(u32*)(sequenceBytes + 0x18), *(u32*)(sequenceBytes + 0x20),
                0, 0);

            if (*(u32*)(sequenceBytes + 0x08) & 0x4) {
                clear__5GSvecFv(&offsetPosition);
                clear__5GSvecFv(&offsetRotation);
                if (*(s8*)(ownerBytes + 0x76) < 0 &&
                    (*(u32*)(sequenceBytes + 0x08) & 0x02000000) == 0x02000000) {
                    offsetRotation.y = 3.1415927f;
                }
                if ((*(u32*)(sequenceBytes + 0x08) & 0x00800000) == 0x00800000) {
                    battleGridGetNormalisedScale((f32*)&offsetScale);
                    if ((*(u32*)(sequenceBytes + 0x08) & 0x01000000) == 0x01000000) {
                        offsetScale.y = 1.0f;
                    }
                } else {
                    set__5GSvecFfff((f32*)&offsetScale, 1.0f,
                                    1.0f, 1.0f);
                }
            } else {
                GSmodelGetPosition(model, &offsetPosition);
                if (*(u32*)(sequenceBytes + 0x08) & 0x00004000) {
                    bound = GSmodelGetBound(model);
                    GSlerpGetLinearInterpolationVector(
                        &center, bound + 0x10, bound + 0x1C, 0.5f);
                    GSvecAdd(&offsetPosition, &offsetPosition, &center);
                }
                if (*(u16*)(sequenceBytes + 0x2E) == 2 &&
                    (ownerBytes[0x18] & 2) == 2 &&
                    (u8)GSmodelIsRootNullAdded(model) != 0) {
                    GSmodelGetRootPosition(model, (GSvec*)&rootPosition);
                    offsetPosition.y += rootPosition.y;
                }
                GSmodelGetRotation(model, &offsetRotation);
                scaleValue = fn_801DABAC(owner);
                set__5GSvecFfff(
                    (f32*)&offsetScale, scaleValue, scaleValue, scaleValue);
            }

            fn_801765F4(animShift);
            cameraMoveStop();
            cameraSetOffsetPosition(&offsetPosition);
            cameraSetOffsetRotation(&offsetRotation);
            cameraSetOffsetScale(&offsetScale);
            fn_800D1070(0);
            cameraUpdate();
            return;
        }

        if (*(u32*)(sequenceBytes + 0x08) & 0x00200000) {
            paramsFlags |= 2;
        } else if (*(u32*)(sequenceBytes + 0x08) & 0x00000200) {
            paramsFlags |= 4;
        } else if (*(u32*)(sequenceBytes + 0x08) & 0x00400000) {
            GSmodelGetPosition(*(void**)(ownerBytes + 0x24), &ownerPosition);
            if (ownerPosition.z < 0.0f) {
                paramsFlags |= 4;
            } else {
                paramsFlags |= 8;
            }
        }

        if (*(u32*)(sequenceBytes + 0x08) & 0x00000400) {
            paramsFlags |= 0x20;
        } else if (*(u32*)(sequenceBytes + 0x08) & 0x00000800) {
            paramsFlags |= 0x40;
        } else if (*(u32*)(sequenceBytes + 0x08) & 0x00001000) {
            paramsFlags |= 0x80;
        }
    } else {
        paramsFlags = 4;
    }

    if (ownerBytes[0x75] == 0) {
        u16 count = battleGridGetNumPokemonsForTrainer((u32)owner);

        if (count == 1) {
            paramsFlags &= ~0x1F;
            paramsFlags |= 0x10;
        } else if (count > 1) {
            paramsFlags &= ~0x1F;
            paramsFlags |= 1;
        }
    }

    GSscene_SetMode(7);
    cameraMoveStop();
    {
        u8* activeOwner = lbl_8047B3EC;
        u8* activeSequence = lbl_8047B3F0;
        u8* cameraParams = *(u8**)(activeOwner + 0x2C) +
                           *(u16*)(activeOwner + 0x32) * 0xD4;
        u32 targetId;

        if (activeSequence != NULL) {
            targetId = ((u32*)(cameraParams + 0x4C))[activeSequence[0x17]];
        } else {
            targetId = *(u32*)(cameraParams + 0x54);
        }
        cameraSetTargetExt(*(u32*)(ownerBytes + 0x00),
                           *(u32*)(ownerBytes + 0x04), targetId);
        cameraRefreshTargetPos();
        cameraSetTargetExt(*(u32*)(ownerBytes + 0x00),
                           *(u32*)(ownerBytes + 0x04), (u32)-1);
    }

    if (ownerBytes[0x75] == 0) {
        shift = 1;
    }

    _wazaSequenceCameraCalculateParams__FP13ModelSequenceiP24wazaSequenceCameraParams(
        owner, paramsFlags, lbl_804673A0);
    _wazaSequenceCameraSelectMotion__FP13ModelSequenceP12WazaSequenceP24wazaSequenceCameraParams(
        owner, sequence, lbl_804673A0);
    _wazaSequenceCameraDoPosition__FP13ModelSequenceP24wazaSequenceCameraParamsfb(
        owner, lbl_804673A0, shift, reverse);
    _wazaSequenceCameraDoFOV__FP13ModelSequenceP24wazaSequenceCameraParamsif(
        owner, lbl_804673A0, paramsFlags, shift);
    cameraUpdate();
}

/**
 * _wazaSequenceCameraDoPosition__FP13ModelSequenceP24wazaSequenceCameraParamsfb - Move animation state machine A.
 * Address: 0x801D349C | Size: 0xAE0
 * Massive state machine (~2.8KB) for a class of move animations.
 * Likely handles physical/contact move animations.
 */
void _wazaSequenceCameraDoPosition__FP13ModelSequenceP24wazaSequenceCameraParamsfb(
    void* modelSequence, void* cameraParams, s32 shift, u8 reverse)
{
    typedef struct WazaSequenceCameraPattern {
        u32 duration_index;
        u8 pad_04[0xD4 - 4];
    } WazaSequenceCameraPattern;
    typedef struct WazaSequenceCameraParamsLocal {
        s32 mode;
        u8 pad_04[8];
        f32 rotation_min;
        f32 rotation_max;
        f32 rotation_base;
        f32 height_min;
        f32 height_max;
        f32 distance_min;
        f32 distance_max;
        f32 out_near;
        f32 out_mid;
        f32 out_far;
    } WazaSequenceCameraParamsLocal;
    extern f32 fn_800E0BE4(void);
    extern s32 fn_800D37CC(void);
    extern void cameraSetDistance(f32);
    extern void cameraSetHeight(f32);
    extern void cameraSetRotY(f32);
    extern void cameraUpdate(void);
    extern void cameraMovePosition(s32, Vec*, f32);
    /* RULE-EXCEPTION(user-approved): codegen-only prototype with a leading int, reproducing retail's li r3,7 before the call - see docs/RULE_EXCEPTIONS.md */
    extern void cameraMoveRotationXYZ(s32, f32, f32, f32, f32);
    extern void GSscene_GetCameraDirectionVector(Vec*);
    extern void GSscene_SetCameraDirectionVector(Vec*);
    u8* sequence = modelSequence;
    WazaSequenceCameraParamsLocal* params = cameraParams;
    WazaSequenceCameraPattern* pattern;
    s32 duration;

    pattern = (WazaSequenceCameraPattern*)
        (*(u8**)(sequence + 0x2C) + *(u16*)(sequence + 0x32) * 0xD4);

    switch (params->mode) {
    case 0:
        _wazaSequenceCameraDoDollyPosition__FP21TemplateExpFileHeaderP24wazaSequenceCameraParamsfb(
            pattern, params, shift, reverse);
        break;

    case 1: {
        Vec direction;
        /* RULE-EXCEPTION(user-approved): declaration order chosen for register allocation - see docs/RULE_EXCEPTIONS.md */
        f32 distance;
        f32 height;
        f32 offset;
        f32 square;
        u8* durationPtr = (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(durationPtr + 4);
        if (duration == 0) {
            duration = *(s32*)(durationPtr + 8);
        }
        duration <<= shift;

        offset = 10.0f * fn_800E0BE4() + 25.0f;
        height = 9.0f * fn_800E0BE4() + 1.0f;
        distance = 30.0f * fn_800E0BE4() + 20.0f;

        cameraSetDistance(distance);
        cameraSetHeight(height);
        cameraSetRotY(0.0f);
        cameraUpdate();

        GSscene_GetCameraDirectionVector(&direction);
        if (reverse) {
            direction.x += 10.0f;
        } else {
            direction.x -= 10.0f;
        }
        GSscene_SetCameraDirectionVector(&direction);
        if (reverse) {
            direction.x += offset;
        } else {
            direction.x -= offset;
        }
        cameraMovePosition(7, &direction, (f32)duration / (f32)fn_800D37CC());

        square = offset * offset + height * height;
        params->out_near = sqrtf(square);
        params->out_far = sqrtf(distance * distance + square);
        params->out_mid = 0.5f * (params->out_near + params->out_far);
        break;
    }

    case 2: {
        f32 distance;
        f32 height;
        f32 rotA;
        f32 rotB;
        f32 length;
        u8* durationPtr = (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(durationPtr + 4);
        if (duration == 0) {
            duration = *(s32*)(durationPtr + 8);
        }
        duration <<= shift;

        /* RULE-EXCEPTION(user-approved): distance_min staged in the reused scalar length for retail's f28 - see docs/RULE_EXCEPTIONS.md */
        length = params->distance_min;
        distance = length + (params->distance_max - length) * fn_800E0BE4();
        height = params->height_min +
            (params->height_max - params->height_min) * fn_800E0BE4();
        if (reverse) {
            /* RULE-EXCEPTION(user-approved): rotation_min staged in rotA (reused scalar) for allocation - see docs/RULE_EXCEPTIONS.md */
            rotA = params->rotation_min;
            rotA = (params->rotation_base - rotA) -
                   (params->rotation_max - rotA) * fn_800E0BE4();
            rotB = (params->rotation_base - params->rotation_min) -
                   (params->rotation_max - params->rotation_min) * fn_800E0BE4();
        } else {
            /* RULE-EXCEPTION(user-approved): rotation_min staged in rotA (reused scalar) for allocation - see docs/RULE_EXCEPTIONS.md */
            rotA = params->rotation_min;
            rotA = (params->rotation_max - rotA) * fn_800E0BE4() +
                   (params->rotation_base + rotA);
            rotB = (params->rotation_max - params->rotation_min) * fn_800E0BE4() +
                   (params->rotation_base + params->rotation_min);
        }
        if (rotA > rotB) {
            f32 swap = rotA;
            rotA = rotB;
            rotB = swap;
        }

        cameraSetDistance(distance);
        cameraSetHeight(height);
        cameraSetRotY(rotA);
        cameraUpdate();
        cameraMoveRotationXYZ(7, 0.0f, rotB, 0.0f, (f32)duration / (f32)fn_800D37CC());

        length = sqrtf(distance * distance + height * height);
        params->out_far = length;
        params->out_mid = length;
        params->out_near = length;
        break;
    }

    case 3: {
        Vec direction;
        f32 distance;
        f32 rotation;
        f32 length;
        u8* durationPtr = (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(durationPtr + 4);
        if (duration == 0) {
            duration = *(s32*)(durationPtr + 8);
        }
        duration <<= shift;

        /* RULE-EXCEPTION(user-approved): distance_min staged in the reused scalar length for retail's f28 - see docs/RULE_EXCEPTIONS.md */
        length = params->distance_min;
        distance = length + (params->distance_max - length) * fn_800E0BE4();
        /* RULE-EXCEPTION(user-approved): the height is kept in the reused scalar length for retail's f28 - see docs/RULE_EXCEPTIONS.md */
        length = params->height_min +
            (params->height_max - params->height_min) * fn_800E0BE4();
        if (reverse) {
            /* RULE-EXCEPTION(user-approved): rotation_min staged in rotation (reused scalar) for allocation - see docs/RULE_EXCEPTIONS.md */
            rotation = params->rotation_min;
            rotation = (params->rotation_base - rotation) -
                       (params->rotation_max - rotation) * fn_800E0BE4();
        } else {
            /* RULE-EXCEPTION(user-approved): rotation_min staged in rotation (reused scalar) for allocation - see docs/RULE_EXCEPTIONS.md */
            rotation = params->rotation_min;
            rotation = (params->rotation_max - rotation) * fn_800E0BE4() +
                       (params->rotation_base + rotation);
        }

        cameraSetDistance(distance);
        cameraSetHeight(length);
        cameraSetRotY(rotation);
        cameraUpdate();
        GSscene_GetCameraDirectionVector(&direction);
        cameraMovePosition(7, &direction, (f32)duration / (f32)fn_800D37CC());

        length = sqrtf(distance * distance + length * length);
        params->out_far = length;
        params->out_mid = length;
        params->out_near = length;
        break;
    }

    case 4: {
        Vec direction;
        f32 rotation;
        f32 length;
        u8* durationPtr = (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(durationPtr + 4);
        if (duration == 0) {
            duration = *(s32*)(durationPtr + 8);
        }
        /* Retail does not scale this case's duration by shift. */

        if (reverse) {
            rotation = params->rotation_base - 0.47123894f;
        } else {
            rotation = 0.47123894f + params->rotation_base;
        }

        cameraSetDistance(110.0f);
        cameraSetHeight(25.0f);
        cameraSetRotY(rotation);
        cameraUpdate();
        GSscene_GetCameraDirectionVector(&direction);
        cameraMovePosition(7, &direction, (f32)duration / (f32)fn_800D37CC());

        length = sqrtf(110.0f * 110.0f + 25.0f * 25.0f);
        params->out_far = length;
        params->out_mid = length;
        params->out_near = length;
        break;
    }

    case 5: {
        Vec direction;
        f32 scale;
        f32 distance;
        f32 height;
        f32 rotation;
        f32 length;
        u8* durationPtr = (u8*)pattern + (*(u32*)pattern << 2);
        /* RULE-EXCEPTION(user-approved): mode declared after durationPtr for register allocation - see docs/RULE_EXCEPTIONS.md */
        s32 mode = *(s32*)(sequence + 0x10);

        duration = *(s32*)(durationPtr + 4);
        if (duration == 0) {
            duration = *(s32*)(durationPtr + 8);
        }
        duration <<= shift;

        switch (mode) {
        case -2:
        case -1:
            scale = 0.875f;
            break;
        case 1:
            scale = 1.4f;
            break;
        case 2:
            scale = 1.8f;
            break;
        case 3:
            scale = 3.0f;
            break;
        default:
            scale = 1.0f;
            break;
        }

        distance = 50.0f * scale;
        /* RULE-EXCEPTION(user-approved): height_min staged in the reused scalar rotation for allocation - see docs/RULE_EXCEPTIONS.md */
        rotation = params->height_min;
        height = rotation + (params->height_max - rotation) * fn_800E0BE4();
        if (reverse) {
            rotation = params->rotation_base - 0.7853982f;
        } else {
            rotation = 0.7853982f + params->rotation_base;
        }

        cameraSetDistance(distance);
        cameraSetHeight(height);
        cameraSetRotY(rotation);
        cameraUpdate();
        GSscene_GetCameraDirectionVector(&direction);
        cameraMovePosition(7, &direction, (f32)duration / (f32)fn_800D37CC());

        length = sqrtf(distance * distance + height * height);
        params->out_far = length;
        params->out_mid = length;
        params->out_near = length;
        break;
    }
    }
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
    extern void GSvecSub(Vec*, Vec*, Vec*);
    extern void vecScale(Vec*, Vec*, f32);
    extern void GSvecAdd(Vec*, Vec*, Vec*);
    extern s32 fn_800D37CC(void);
    extern void cameraMovePosition(s32, Vec*, f32);
    u8* file = header;
    WazaCameraParams* camera = params;
    Vec direction;
    Vec position;
    s32 randomDelay;
    s32 duration;
    s32 delay;
    f32 threshold0 = 0.5f;
    f32 threshold1 = 0.75f;
    f32 distance0;
    f32 distance1;
    f32 nearDistance;
    f32 farDistance;
    f32 height;
    f32 rotation;
    f32 chance;
    u8 alternate;

    duration = *(s32*)(file + (*(s32*)file << 2) + 4);
    if (duration == 0) {
        duration = *(s32*)(file + (*(s32*)file << 2) + 8);
    }
    duration <<= shift;

    if (fn_800E0BE4() <= 0.7f) {
        threshold0 = 0.3f;
        threshold1 = 0.65f;
        alternate = TRUE;
    } else {
        alternate = FALSE;
    }

    chance = fn_800E0BE4();
    if (chance < threshold0) {
        randomDelay = 0;
    } else if (chance < threshold1) {
        s32 fullDuration;

        randomDelay = 0;
        fullDuration = duration;
        duration = _fadeEffectGetRandom__FUl(duration);
        if (duration < (fullDuration >> 1)) {
            duration = fullDuration >> 1;
        }
    } else {
        randomDelay = _fadeEffectGetRandom__FUl(duration);
        if (randomDelay > (duration >> 1)) {
            randomDelay = duration >> 1;
        }
    }

    distance0 = camera->distanceMin +
        (camera->distanceMax - camera->distanceMin) * fn_800E0BE4();
    nearDistance = distance0;
    distance1 = camera->distanceMin +
        (camera->distanceMax - camera->distanceMin) * fn_800E0BE4();
    farDistance = distance1;
    /* RULE-EXCEPTION(user-approved): the height is staged in distance1 (reused scalar) as retail's f0 temporary - see docs/RULE_EXCEPTIONS.md */
    if (alternate) {
        if (distance0 < distance1) {
            farDistance = distance0;
            nearDistance = distance1;
        }
        distance1 = camera->heightMin +
            (camera->heightMax - camera->heightMin) * fn_800E0BE4();
        height = distance1;
    } else {
        if (distance0 > distance1) {
            farDistance = distance0;
            nearDistance = distance1;
        }
        distance1 = camera->heightMin +
            (camera->heightMax - camera->heightMin) * fn_800E0BE4();
        height = distance1;
    }

    if (reverse) {
        rotation = (camera->rotationOffset - camera->rotationBase) -
                   (camera->rotationRange - camera->rotationBase) * fn_800E0BE4();
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
    GSvecSub(&direction, &direction, &position);
    direction.y = 0.0f;
    vecScale(&direction, &direction, farDistance / nearDistance);
    direction.y = height;
    GSvecAdd(&direction, &direction, &position);
    delay = duration - randomDelay;
    cameraMovePosition(7, &direction, (f32)delay / (f32)fn_800D37CC());

    camera->nearDistance =
        sqrtf(nearDistance * nearDistance + height * height);
    distance0 = 0.5f * (nearDistance + farDistance);
    camera->middleDistance =
        sqrtf(distance0 * distance0 + height * height);
    camera->farDistance =
        sqrtf(farDistance * farDistance + height * height);
}

typedef struct WazaSequenceCameraParamsFov {
    s32 mode;
    f32 scaleMin;
    f32 scaleMax;
    u8 pad_0C[0x1C];
    f32 ranges[3];
} WazaSequenceCameraParamsFov;

extern f64 atan2(f64, f64);

/* RULE-EXCEPTION(user-approved): inline helper reconstructed for codegen; each fov is staged in its own output before the degree scaling - see docs/RULE_EXCEPTIONS.md */
static inline void _wazaSequenceCameraFovRange(
    WazaSequenceCameraParamsFov* params, f32 radius, f32* lowFov, f32* highFov)
{
    f32 span = 0.75f * params->scaleMin;
    if (params->scaleMax > span) {
        span = params->scaleMax;
    }
    *lowFov = (f32)atan2(0.5f * span, radius);
    *lowFov = 57.29578f * (2.0f * *lowFov);
    *highFov = (f32)atan2(2.0f * span, radius);
    *highFov = 57.29578f * (2.0f * *highFov);
}

/* RULE-EXCEPTION(user-approved): inline helper reconstructed for codegen; hasSecond is copied from hasFirst to keep retail's mr - see docs/RULE_EXCEPTIONS.md */
static inline void _wazaSequenceCameraFovMix(s32 choice, f32* mixStart, f32* mixEnd)
{
    u8 hasFirst;
    u8 hasSecond;

    hasFirst = FALSE;
    hasSecond = hasFirst;
    if ((choice & 1) == 1) {
        *mixStart = 0.0f;
        hasFirst = TRUE;
        *mixEnd = 0.2f;
    }
    if ((choice & 2) == 2) {
        if (!hasFirst) {
            *mixStart = 0.35f;
        }
        *mixEnd = 0.6f;
        hasSecond = TRUE;
    }
    if ((choice & 4) == 4) {
        if (!hasSecond) {
            *mixStart = 0.75f;
        }
        *mixEnd = 1.0f;
    }
}

/**
 * _wazaSequenceCameraDoFOV__FP13ModelSequenceP24wazaSequenceCameraParamsif - Move animation state machine C.
 * Address: 0x801D44C4 | Size: 0x514
 * State machine for status/field effect move animations.
 */
void _wazaSequenceCameraDoFOV__FP13ModelSequenceP24wazaSequenceCameraParamsif(
    void* modelSequence, void* cameraParams, s32 flags, s32 shift)
{
    typedef struct CameraFovKey {
        f32 start;
        f32 end;
        u32 startFrame;
        u32 endFrame;
    } CameraFovKey;
    typedef struct WazaSequenceCameraFovTiming {
        s32 count;
        u8 pad_04[8];
        s32 frames[3];
    } WazaSequenceCameraFovTiming;
    typedef struct WazaSequenceCameraFovPattern {
        s32 mode;
        s32 durationMode;
        f32 thresholds[4];
        u32 initialFlags;
        u32 flags;
        s32 timingMode;
    } WazaSequenceCameraFovPattern;
    extern f64 atan2(f64, f64);
    extern f32 fn_800E0BE4(void);
    extern void battleCameraDisable(void);
    extern void cameraSetFov(f32);
    extern CameraFovKey lbl_804673D4[];
    extern f32 lbl_80478CDC;
    extern u32 lbl_8047B3E8;
    u8* sequence = modelSequence;
    /* RULE-EXCEPTION(user-approved): declaration order chosen for register allocation - see docs/RULE_EXCEPTIONS.md */
    WazaSequenceCameraFovTiming* timing;
    s32 count;
    s32 prevFrame;
    s32 nextFrame;
    s32 i;
    WazaSequenceCameraFovPattern* pattern;
    s32 duration;
    s32 startFrame;
    s32 endFrame;
    s32 sequenceId;
    s32 choice;
    f32 mixStart;
    f32 mixEnd;
    f32 currentFov;
    f32 lowFov;
    f32 highFov;
    f32 fov;

    sequenceId = *(u16*)(sequence + 0x32);
    timing = (WazaSequenceCameraFovTiming*)
        (*(u8**)(sequence + 0x2C) + sequenceId * 0xD4);
    count = timing->count;
    mixStart = 0.0f;
    mixEnd = 1.0f;
    _wazaSequenceCameraFovRange(cameraParams, ((WazaSequenceCameraParamsFov*)cameraParams)->ranges[0], &lowFov, &highFov);
    if (lowFov < 15.0f) {
        lowFov = 15.0f;
    }
    if (highFov < 15.0f) {
        highFov = 15.0f;
    }
    if (lowFov > 85.0f) {
        lowFov = 85.0f;
    }
    if (highFov > 85.0f) {
        highFov = 85.0f;
    }

    switch (((WazaSequenceCameraParamsFov*)cameraParams)->mode) {
    case 0:
    case 4:
    case 5:
        if (flags & 0x20) {
            choice = 1;
        } else if (flags & 0x80) {
            choice = 4;
        } else {
            choice = 2;
        }
        _wazaSequenceCameraFovMix(choice, &mixStart, &mixEnd);
        fov = mixStart + (mixEnd - mixStart) * fn_800E0BE4();
        fov = fov * (highFov - lowFov) + lowFov;
        lbl_804673D4[1].end = fov;
        lbl_804673D4[1].start = fov;
        lbl_804673D4[0].end = fov;
        lbl_804673D4[0].start = fov;
        lbl_80478CDC = fov;
        lbl_804673D4[1].endFrame = timing->frames[0] << shift;
        lbl_804673D4[1].startFrame = timing->frames[0] << shift;
        lbl_804673D4[0].endFrame = timing->frames[0] << shift;
        lbl_804673D4[0].startFrame = timing->frames[0] << shift;
        lbl_8047B3E8 = timing->frames[0] << shift;
        cameraSetFov(fov);
        return;
    }

    pattern = (WazaSequenceCameraFovPattern*)((u8*)wazaSequenceCameraGetPattern__Fbi(
        sequenceId != 8 && sequenceId != 9, flags) + 4);

    _wazaSequenceCameraFovMix(pattern->initialFlags, &mixStart, &mixEnd);
    fov = mixStart + (mixEnd - mixStart) * fn_800E0BE4();
    fov = fov * (highFov - lowFov) + lowFov;
    lbl_804673D4[1].end = fov;
    lbl_804673D4[1].start = fov;
    lbl_804673D4[0].end = fov;
    lbl_804673D4[0].start = fov;
    lbl_80478CDC = fov;
    lbl_804673D4[1].endFrame = timing->frames[0] << shift;
    lbl_804673D4[1].startFrame = timing->frames[0] << shift;
    lbl_804673D4[0].endFrame = timing->frames[0] << shift;
    lbl_804673D4[0].startFrame = timing->frames[0] << shift;
    lbl_8047B3E8 = timing->frames[0] << shift;
    cameraSetFov(fov);

    if (count <= 2) {
        return;
    }

    nextFrame = timing->frames[0];
    currentFov = fov;
    for (i = 0; i < 2; i++, pattern++) {
        prevFrame = nextFrame;
        nextFrame = timing->frames[i + 1];
        if (prevFrame == nextFrame) {
            lbl_804673D4[i].startFrame = prevFrame << shift;
            lbl_804673D4[i].endFrame = prevFrame << shift;
            lbl_804673D4[i].start = currentFov;
            lbl_804673D4[i].end = currentFov;
        } else switch (pattern->mode) {
        case 3:
        case 4: {
            duration = _wazaSequenceCameraSelectDuration__FUcPff(
                pattern->durationMode, pattern->thresholds, nextFrame - prevFrame);
            _wazaSequenceCameraFovRange(cameraParams,
                ((WazaSequenceCameraParamsFov*)cameraParams)->ranges[i + 1], &lowFov, &highFov);
            if (lowFov < 15.0f) {
                lowFov = 15.0f;
            }
            if (highFov < 15.0f) {
                highFov = 15.0f;
            }
            if (lowFov > 85.0f) {
                lowFov = 85.0f;
            }
            if (highFov > 85.0f) {
                highFov = 85.0f;
            }
            _wazaSequenceCameraFovMix(pattern->flags, &mixStart, &mixEnd);
            fov = mixStart + (mixEnd - mixStart) * fn_800E0BE4();
            fov = fov * (highFov - lowFov) + lowFov;
            if (pattern->timingMode == 2) {
                endFrame = nextFrame;
                startFrame = nextFrame - duration;
            } else {
                startFrame = prevFrame;
                endFrame = prevFrame + duration;
            }
            lbl_804673D4[i].startFrame = startFrame << shift;
            lbl_804673D4[i].endFrame = endFrame << shift;
            lbl_804673D4[i].start = currentFov;
            currentFov = fov;
            lbl_804673D4[i].end = fov;
            break;
        }
        /* RULE-EXCEPTION(user-approved): explicit labels for modes 0-2 shape retail's compare tree - see docs/RULE_EXCEPTIONS.md */
        case 0:
        case 1:
        case 2:
        default:
            lbl_804673D4[i].startFrame = prevFrame << shift;
            lbl_804673D4[i].endFrame = nextFrame << shift;
            lbl_804673D4[i].start = currentFov;
            lbl_804673D4[i].end = currentFov;
            break;
        }
    }
}

/**
 * _wazaSequenceCameraSelectDuration__FUcPff - Move animation state machine D.
 * Address: 0x801D49D8 | Size: 0x3C8
 * State machine for spread/multi-target move animations.
 */
s32 _wazaSequenceCameraSelectDuration__FUcPff(
    s32 mode, f32* thresholds, s32 duration)
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
        return duration;
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
    extern f32 fn_800E0BE4(void);
    extern s32 lbl_80478CD8;
    u32 flags;
    u8 option0;
    u8 option1;
    u8 option2;
    u8 option3;
    s32 count = 0;
    s32* motion;
    f32 interval;

    motion = cameraParams;
    if (*(u16*)((u8*)modelSequence + 0x70) == 0x13A) {
        *motion = 4;
        return;
    }

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
        option0 = 1;
        option1 = 1;
        option2 = 1;
        option3 = 1;
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

    interval = 1.0f / (f32)count;
    for (;;) {
        f32 random;
        f32 limit;

        random = fn_800E0BE4();
        limit = interval;
        if (option0) {
            if (limit > random) {
                *motion = 3;
                if (lbl_80478CD8 != *motion) {
                    break;
                }
            }
            limit += interval;
        }
        if (option1) {
            if (limit > random) {
                *motion = 0;
                if (lbl_80478CD8 != *motion) {
                    break;
                }
            }
            limit += interval;
        }
        if (option2) {
            if (limit > random) {
                *motion = 1;
                if (lbl_80478CD8 != *motion) {
                    break;
                }
            }
            limit += interval;
        }
        if (option3 && limit > random) {
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
    typedef struct WazaCameraBound {
        u8 pad_00[0x14];
        f32 minY;
        u8 pad_18[0x08];
        f32 maxY;
        u8 pad_24[0x04];
        Vec size;
    } WazaCameraBound;
    extern void* GSmodelGetBound(void*);
    extern void GSmodelGetRotation(void*, Vec*);
    extern f32 vecMag(Vec*);
    u8* sequence = modelSequence;
    WazaSequenceCameraParams* params = cameraParams;
    void* model = *(void**)(sequence + 0x24);
    WazaCameraBound* bound = GSmodelGetBound(model);
    Vec* extent;
    Vec rotation;
    /* RULE-EXCEPTION(user-approved): declaration order chosen for register allocation - see docs/RULE_EXCEPTIONS.md */
    f32 sizeScale;
    f32 lower;
    f32 upper;
    f32 distanceScale;
    s32 mode;

    GSmodelGetRotation(model, &rotation);
    params->rotationBase = rotation.y;
    mode = *(s32*)(sequence + 0x10);
    switch (mode) {
    case -2:
        sizeScale = 1.1f;
        distanceScale = 0.875f;
        break;
    case -1:
        sizeScale = 1.1f;
        distanceScale = 0.875f;
        break;
    case 1:
        sizeScale = 1.25f;
        distanceScale = 1.4f;
        break;
    case 2:
        sizeScale = 1.3f;
        distanceScale = 1.8f;
        break;
    case 3:
        sizeScale = 1.5f;
        distanceScale = 3.0f;
        break;
    default:
        sizeScale = 1.2f;
        distanceScale = 1.0f;
        break;
    }

    if (flags & 1) {
        params->rotationMin = 0.0f;
        params->rotationMax = 0.5237035f;
    } else if (flags & 2) {
        params->rotationMin = 0.31415927f;
        params->rotationMax = 0.62831855f;
    } else if (flags & 4) {
        if (mode > 0) {
            params->rotationMin = 0.31415927f;
            params->rotationMax = 0.62831855f;
        } else {
            params->rotationMin = 0.62831855f;
            params->rotationMax = 0.9424779f;
        }
    } else if (flags & 8) {
        if (mode > 0) {
            params->rotationMin = 0.62831855f;
            params->rotationMax = 0.9424779f;
        } else {
            params->rotationMin = 0.9424779f;
            params->rotationMax = 1.2566371f;
        }
    } else if (flags & 0x10) {
        params->rotationMin = 1.2566371f;
        params->rotationMax = 1.5707964f;
    } else if (mode > 0) {
        params->rotationMin = 0.31415927f;
        params->rotationMax = 0.9424779f;
    } else {
        params->rotationMin = 0.31415927f;
        params->rotationMax = 1.0995574f;
    }

    if (flags & 0x20) {
        lower = 6.0f;
        upper = 8.0f;
        params->distanceMin = 25.0f;
        params->distanceMax = 35.0f;
    } else if (flags & 0x40) {
        params->distanceMin = 35.0f;
        params->distanceMax = 50.0f;
        lower = 6.0f;
        upper = 11.0f;
    } else if (flags & 0x80) {
        params->distanceMin = 50.0f;
        params->distanceMax = 60.0f;
        lower = 6.0f;
        upper = 20.0f;
    } else {
        /* RULE-EXCEPTION(user-approved): chained assignment so distanceMin is stored from upper's register, as retail - see docs/RULE_EXCEPTIONS.md */
        params->distanceMin = upper = 20.0f;
        params->distanceMax = 60.0f;
        lower = 6.0f;
    }

    extent = &bound->size;
    params->distanceMin *= distanceScale;
    params->distanceMax *= distanceScale;
    /* RULE-EXCEPTION(user-approved): size and height read into block temporaries before the stores, so extent->y loads before the size store as in retail - see docs/RULE_EXCEPTIONS.md */
    {
        f32 size = 0.57735026f * vecMag(extent);
        f32 height = extent->y;
        params->size = size;
        params->height = height;
    }
    params->heightMin = bound->minY;
    params->heightMax = bound->maxY;

    if (params->heightMin < lower) {
        params->heightMin = lower;
        if (params->heightMax < lower) {
            params->heightMax = 1.5f * lower;
        }
    }
    if (params->heightMax > upper) {
        params->heightMax = upper;
        if (params->heightMin > upper) {
            params->heightMin = 0.8f * upper;
        }
    }
    params->size *= sizeScale;
    params->height *= sizeScale;
    if (params->distanceMin < 20.0f) {
        params->distanceMin = 20.0f;
    }
    if (params->distanceMax < 40.0f) {
        params->distanceMax = 40.0f;
    }
    if (params->distanceMin > 48.0f) {
        params->distanceMin = 48.0f;
    }
    if (params->distanceMax > 60.0f) {
        params->distanceMax = 60.0f;
    }
}
