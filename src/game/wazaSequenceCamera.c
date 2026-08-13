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
    extern void* GSresGetResource(u32 group, u32 resource);
    extern void clear__5GSvecFv(void* vec);
    extern void GSlerpGetLinearInterpolationVector(void* dst, void* src,
                                                   void* target, f32 t);
    extern f32 lbl_8047E1F0;
    extern f32 lbl_8047E1F4;
    extern f32 lbl_8047E1F8;
    extern f32 lbl_8047E1FC;
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
    void* model;
    u32 flags = 0;
    s32 paramsFlags = 0;
    f32 scaleValue;
    s32 shift = 0;
    u8 reverse = FALSE;

    if (lbl_8047B3F4 == 0) {
        return;
    }

    if (*(s8*)(ownerBytes + 0x76) < 0 &&
        (sequenceBytes == NULL || ((*(u32*)(sequenceBytes + 0x08) & 0x80) == 0))) {
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
            if (reverse) {
                shift = 4;
            }

            cameraPlayOffsetAnime(
                *(u32*)(sequenceBytes + 0x18), *(u32*)(sequenceBytes + 0x20),
                0, 0);

            flags = *(u32*)(sequenceBytes + 0x08);
            if (flags & 0x4) {
                clear__5GSvecFv(&offsetPosition);
                clear__5GSvecFv(&offsetRotation);
                if (*(s8*)(ownerBytes + 0x76) < 0 &&
                    (flags & 0x02000000)) {
                    offsetRotation.y = lbl_8047E1F0;
                }
                if (flags & 0x00800000) {
                    battleGridGetNormalisedScale((f32*)&offsetScale);
                    if (flags & 0x01000000) {
                        offsetScale.y = lbl_8047E1F4;
                    }
                } else {
                    set__5GSvecFfff((f32*)&offsetScale, lbl_8047E1F4,
                                    lbl_8047E1F4, lbl_8047E1F4);
                }
            } else {
                GSmodelGetPosition(model, &offsetPosition);
                if (flags & 0x00004000) {
                    GSlerpGetLinearInterpolationVector(
                        &center, (u8*)GSmodelGetBound(model) + 0x10,
                        (u8*)GSmodelGetBound(model) + 0x1C, lbl_8047E1F8);
                    GSvecAdd(&offsetPosition, &offsetPosition, &center);
                }
                if (*(u16*)(sequenceBytes + 0x2E) == 2 &&
                    (ownerBytes[0x18] & 2) != 0 &&
                    GSmodelIsRootNullAdded(model) != 0) {
                    GSmodelGetRootPosition(model, (GSvec*)&rootPosition);
                    offsetPosition.y += rootPosition.y;
                }
                GSmodelGetRotation(model, &offsetRotation);
                scaleValue = fn_801DABAC(owner);
                set__5GSvecFfff(
                    (f32*)&offsetScale, scaleValue, scaleValue, scaleValue);
            }

            fn_801765F4(shift);
            cameraMoveStop();
            cameraSetOffsetPosition(&offsetPosition);
            cameraSetOffsetRotation(&offsetRotation);
            cameraSetOffsetScale(&offsetScale);
            fn_800D1070(0);
            cameraUpdate();
            return;
        }

        flags = *(u32*)(sequenceBytes + 0x08);
        if (flags & 0x00200000) {
            paramsFlags |= 2;
        } else if (flags & 0x00000200) {
            paramsFlags |= 4;
        } else if (flags & 0x00400000) {
            GSmodelGetPosition(*(void**)(ownerBytes + 0x24), &rootPosition);
            if (rootPosition.z < lbl_8047E1FC) {
                paramsFlags |= 4;
            } else {
                paramsFlags |= 8;
            }
        }

        if (flags & 0x00000400) {
            paramsFlags |= 0x20;
        } else if (flags & 0x00000800) {
            paramsFlags |= 0x40;
        } else if (flags & 0x00001000) {
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
            targetId = *(u32*)(cameraParams + 0x4C + activeSequence[0x17] * 4);
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
    extern void cameraMoveRotationXYZ(f32, f32, f32, f32);
    extern void GSscene_GetCameraDirectionVector(Vec*);
    extern void GSscene_SetCameraDirectionVector(Vec*);
    extern f32 sqrtf(f32);
    extern f32 lbl_8047E1F4;
    extern f32 lbl_8047E1F8;
    extern f32 lbl_8047E1FC;
    extern f32 lbl_8047E200;
    extern f32 lbl_8047E204;
    extern f32 lbl_8047E208;
    extern f32 lbl_8047E20C;
    extern f32 lbl_8047E210;
    extern f64 lbl_8047E218;
    extern f64 lbl_8047E220;
    extern f64 lbl_8047E228;
    extern f32 lbl_8047E230;
    extern f32 lbl_8047E234;
    extern f64 lbl_8047E238;
    extern f32 lbl_8047E240;
    extern f32 lbl_8047E244;
    extern f32 lbl_8047E248;
    extern f32 lbl_8047E24C;
    extern f32 lbl_8047E250;
    extern f32 lbl_8047E254;
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
        f32 offset_distance;
        f32 height;
        f32 distance;
        f32 duration_scale;
        f32 len0;
        f32 len2;
        u8* duration_ptr =
            (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(duration_ptr + 4);
        if (duration == 0) {
            duration = *(s32*)(duration_ptr + 8);
        }
        duration <<= shift;

        offset_distance = lbl_8047E200 + lbl_8047E204 * fn_800E0BE4();
        height = lbl_8047E1F4 + lbl_8047E208 * fn_800E0BE4();
        distance = lbl_8047E20C + lbl_8047E210 * fn_800E0BE4();

        cameraSetDistance(distance);
        cameraSetHeight(height);
        cameraSetRotY(lbl_8047E1FC);
        cameraUpdate();

        GSscene_GetCameraDirectionVector(&direction);
        if (reverse) {
            direction.x += lbl_8047E204;
        } else {
            direction.x -= lbl_8047E204;
        }
        GSscene_SetCameraDirectionVector(&direction);
        if (reverse) {
            direction.x += offset_distance;
        } else {
            direction.x -= offset_distance;
        }

        duration_scale = (f32)duration / (f32)fn_800D37CC();
        cameraMovePosition(7, &direction, duration_scale);

        len0 = sqrtf(offset_distance * offset_distance + height * height);
        len2 = sqrtf(distance * distance + len0 * len0);
        params->out_near = len0;
        params->out_far = len2;
        params->out_mid = lbl_8047E1F8 * (len0 + len2);
        break;
    }

    case 2: {
        f32 distance;
        f32 height;
        f32 rot_a;
        f32 rot_b;
        f32 duration_scale;
        f32 len0;
        f32 len2;
        u8* duration_ptr =
            (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(duration_ptr + 4);
        if (duration == 0) {
            duration = *(s32*)(duration_ptr + 8);
        }
        duration <<= shift;

        distance = params->distance_min +
            (params->distance_max - params->distance_min) * fn_800E0BE4();
        height = params->height_min +
            (params->height_max - params->height_min) * fn_800E0BE4();

        if (reverse) {
            rot_a = (params->rotation_min - params->rotation_max) * fn_800E0BE4() +
                    (params->rotation_min - params->rotation_base);
            rot_b = (params->rotation_min - params->rotation_max) * fn_800E0BE4() +
                    (params->rotation_min - params->rotation_base);
        } else {
            rot_a = (params->rotation_max - params->rotation_min) * fn_800E0BE4() +
                    (params->rotation_base + params->rotation_min);
            rot_b = (params->rotation_max - params->rotation_min) * fn_800E0BE4() +
                    (params->rotation_base + params->rotation_min);
        }
        if (rot_a > rot_b) {
            f32 tmp = rot_a;
            rot_a = rot_b;
            rot_b = tmp;
        }

        cameraSetDistance(distance);
        cameraSetHeight(height);
        cameraSetRotY(rot_a);
        cameraUpdate();

        duration_scale = (f32)duration / (f32)fn_800D37CC();
        cameraMoveRotationXYZ(lbl_8047E1FC, rot_b, lbl_8047E1FC, duration_scale);

        len0 = sqrtf(distance * distance + height * height);
        len2 = sqrtf(rot_b * rot_b + len0 * len0);
        params->out_near = len0;
        params->out_far = len2;
        params->out_mid = lbl_8047E1F8 * (len0 + len2);
        break;
    }

    case 3: {
        Vec direction;
        f32 distance;
        f32 height;
        f32 rotation;
        f32 duration_scale;
        f32 length;
        u8* duration_ptr =
            (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(duration_ptr + 4);
        if (duration == 0) {
            duration = *(s32*)(duration_ptr + 8);
        }
        duration <<= shift;

        distance = params->distance_min +
            (params->distance_max - params->distance_min) * fn_800E0BE4();
        height = params->height_min +
            (params->height_max - params->height_min) * fn_800E0BE4();
        if (reverse) {
            rotation = (params->rotation_base - params->rotation_min) * fn_800E0BE4() -
                       (params->rotation_base - params->rotation_min);
        } else {
            rotation = (params->rotation_max - params->rotation_min) * fn_800E0BE4() +
                       (params->rotation_base + params->rotation_min);
        }

        cameraSetDistance(distance);
        cameraSetHeight(height);
        cameraSetRotY(rotation);
        cameraUpdate();
        GSscene_GetCameraDirectionVector(&direction);

        duration_scale = (f32)duration / (f32)fn_800D37CC();
        cameraMovePosition(7, &direction, duration_scale);

        length = sqrtf(distance * distance + height * height + rotation * rotation);
        params->out_far = length;
        params->out_mid = length;
        params->out_near = length;
        break;
    }

    case 4: {
        Vec direction;
        f32 rotation;
        f32 duration_scale;
        f32 length;
        u8* duration_ptr =
            (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(duration_ptr + 4);
        if (duration == 0) {
            duration = *(s32*)(duration_ptr + 8);
        }
        duration <<= shift;

        if (reverse) {
            rotation = params->rotation_base - lbl_8047E230;
        } else {
            rotation = lbl_8047E230 + params->rotation_base;
        }

        cameraSetDistance(lbl_8047E234);
        cameraSetHeight(lbl_8047E200);
        cameraSetRotY(rotation);
        cameraUpdate();
        GSscene_GetCameraDirectionVector(&direction);

        duration_scale = (f32)duration / (f32)fn_800D37CC();
        cameraMovePosition(7, &direction, duration_scale);

        length = sqrtf((f32)lbl_8047E238);
        params->out_far = length;
        params->out_mid = length;
        params->out_near = length;
        break;
    }

    case 5: {
        f32 scale_factor;
        f32 distance;
        f32 height;
        f32 rotation;
        f32 duration_scale;
        f32 length;
        s32 mode = *(s32*)(sequence + 0x10);
        u8* duration_ptr =
            (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(duration_ptr + 4);
        if (duration == 0) {
            duration = *(s32*)(duration_ptr + 8);
        }
        duration <<= shift;

        switch (mode) {
        case -2:
        case -1:
            scale_factor = lbl_8047E240;
            break;
        case 1:
            scale_factor = lbl_8047E244;
            break;
        case 2:
            scale_factor = lbl_8047E248;
            break;
        case 3:
            scale_factor = lbl_8047E24C;
            break;
        default:
            scale_factor = lbl_8047E1F4;
            break;
        }

        distance = lbl_8047E250 * scale_factor;
        height = params->height_min +
            (params->height_max - params->height_min) * fn_800E0BE4();
        if (reverse) {
            rotation = params->rotation_base - lbl_8047E254;
        } else {
            rotation = lbl_8047E254 + params->rotation_base;
        }

        cameraSetDistance(distance);
        cameraSetHeight(height);
        cameraSetRotY(rotation);
        cameraUpdate();

        {
            Vec direction;
            GSscene_GetCameraDirectionVector(&direction);
            duration_scale = (f32)duration / (f32)fn_800D37CC();
            cameraMovePosition(7, &direction, duration_scale);
        }

        length = sqrtf(distance * distance + height * height +
                       lbl_8047E254 * lbl_8047E254);
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
void _wazaSequenceCameraDoFOV__FP13ModelSequenceP24wazaSequenceCameraParamsif(
    void* modelSequence, void* cameraParams, s32 flags, s32 shift)
{
    typedef struct CameraFovKey {
        f32 start;
        f32 end;
        u32 startFrame;
        u32 endFrame;
    } CameraFovKey;
    typedef struct WazaSequenceCameraParamsFov {
        s32 mode;
        f32 scaleMin;
        f32 scaleMax;
        u8 pad_0C[0x1C];
        f32 range0;
        f32 range1;
        f32 range2;
    } WazaSequenceCameraParamsFov;
    typedef struct WazaSequenceCameraFovTiming {
        s32 count;
        u8 pad_04[8];
        s32 frame0;
        s32 frame1;
        s32 frame2;
    } WazaSequenceCameraFovTiming;
    typedef struct WazaSequenceCameraFovPattern {
        s32 mode;
        s32 durationMode;
        f32 thresholds[4];
        u32 initialFlags;
        u32 flags;
        s32 timingMode;
    } WazaSequenceCameraFovPattern;
    extern f32 atan2(f32, f32);
    extern f32 fn_800E0BE4(void);
    extern void battleCameraDisable(void);
    extern void cameraSetFov(f32);
    extern CameraFovKey lbl_804673D4[];
    extern f32 lbl_80478CDC;
    extern u32 lbl_8047B3E8;
    extern f32 lbl_8047E1E0;
    extern f32 lbl_8047E1F4;
    extern f32 lbl_8047E1F8;
    extern f32 lbl_8047E1FC;
    extern f32 lbl_8047E260;
    extern f32 lbl_8047E270;
    extern f32 lbl_8047E274;
    extern f32 lbl_8047E278;
    extern f32 lbl_8047E27C;
    extern f32 lbl_8047E280;
    extern f32 lbl_8047E284;
    u8* sequence = modelSequence;
    WazaSequenceCameraParamsFov* params = cameraParams;
    WazaSequenceCameraFovTiming* timing;
    WazaSequenceCameraFovPattern* pattern;
    CameraFovKey* key;
    u8* timingCursor;
    s32 prevFrame;
    s32 nextFrame;
    s32 count;
    s32 i;
    u32 choice;
    u8 hasFirst;
    u8 hasSecond;
    f32 currentFov;
    f32 lowFov;
    f32 highFov;
    f32 span;

    timing = (WazaSequenceCameraFovTiming*)
        (*(u8**)(sequence + 0x2C) + *(u16*)(sequence + 0x32) * 0xD4);
    count = timing->count;
    span = lbl_8047E260 * params->scaleMin;
    if (params->scaleMax > span) {
        span = params->scaleMax;
    }

    lowFov = lbl_8047E270 *
        (lbl_8047E274 * atan2(lbl_8047E1F8 * span, params->range0));
    highFov = lbl_8047E270 *
        (lbl_8047E274 * atan2(lbl_8047E274 * span, params->range0));

    if (lowFov < lbl_8047E278) {
        lowFov = lbl_8047E278;
    }
    if (highFov < lbl_8047E278) {
        highFov = lbl_8047E278;
    }
    if (lowFov > lbl_8047E27C) {
        lowFov = lbl_8047E27C;
    }
    if (highFov > lbl_8047E27C) {
        highFov = lbl_8047E27C;
    }

    if (params->mode == 0 || (params->mode >= 4 && params->mode < 6)) {
        if (flags & 0x20) {
            choice = 1;
        } else if (flags & 0x80) {
            choice = 4;
        } else {
            choice = 2;
        }

        currentFov = lbl_8047E1FC;
        span = lbl_8047E1F4;
        hasFirst = FALSE;
        hasSecond = FALSE;
        if (choice & 1) {
            currentFov = lbl_8047E1FC;
            span = lbl_8047E1E0;
            hasFirst = TRUE;
        }
        if (choice & 2) {
            if (!hasFirst) {
                currentFov = lbl_8047E280;
            }
            span = lbl_8047E284;
            hasSecond = TRUE;
        }
        if (choice & 4) {
            if (!hasSecond) {
                currentFov = lbl_8047E260;
            }
            span = lbl_8047E1F4;
        }

        currentFov = lowFov + (highFov - lowFov) *
            (currentFov + (span - currentFov) * fn_800E0BE4());
        lbl_804673D4[0].start = currentFov;
        lbl_804673D4[0].end = currentFov;
        lbl_804673D4[1].start = currentFov;
        lbl_804673D4[1].end = currentFov;
        lbl_80478CDC = currentFov;
        lbl_804673D4[0].startFrame = timing->frame0 << shift;
        lbl_804673D4[0].endFrame = timing->frame0 << shift;
        lbl_804673D4[1].startFrame = timing->frame0 << shift;
        lbl_804673D4[1].endFrame = timing->frame0 << shift;
        lbl_8047B3E8 = timing->frame0 << shift;
        cameraSetFov(currentFov);
        return;
    }

    pattern = (WazaSequenceCameraFovPattern*)((u8*)wazaSequenceCameraGetPattern__Fbi(
        (*(u16*)(sequence + 0x32) != 8 && *(u16*)(sequence + 0x32) != 9), flags) + 4);

    currentFov = lbl_8047E1FC;
    span = lbl_8047E1F4;
    hasFirst = FALSE;
    hasSecond = FALSE;
    if (pattern->initialFlags & 1) {
        currentFov = lbl_8047E1FC;
        span = lbl_8047E1E0;
        hasFirst = TRUE;
    }
    if (pattern->initialFlags & 2) {
        if (!hasFirst) {
            currentFov = lbl_8047E280;
        }
        span = lbl_8047E284;
        hasSecond = TRUE;
    }
    if (pattern->initialFlags & 4) {
        if (!hasSecond) {
            currentFov = lbl_8047E260;
        }
        span = lbl_8047E1F4;
    }

    currentFov = lowFov + (highFov - lowFov) *
        (currentFov + (span - currentFov) * fn_800E0BE4());
    lbl_804673D4[0].start = currentFov;
    lbl_804673D4[0].end = currentFov;
    lbl_804673D4[1].start = currentFov;
    lbl_804673D4[1].end = currentFov;
    lbl_80478CDC = currentFov;
    lbl_804673D4[0].startFrame = timing->frame0 << shift;
    lbl_804673D4[0].endFrame = timing->frame0 << shift;
    lbl_804673D4[1].startFrame = timing->frame0 << shift;
    lbl_804673D4[1].endFrame = timing->frame0 << shift;
    lbl_8047B3E8 = timing->frame0 << shift;
    cameraSetFov(currentFov);

    if (count <= 2) {
        return;
    }

    prevFrame = timing->frame0;
    key = lbl_804673D4;
    timingCursor = (u8*)timing;
    for (i = 0; i < 2; i++, key++, pattern++, timingCursor += 4, currentFov = key->end) {
        nextFrame = *(s32*)(timingCursor + 0x10);
        if (prevFrame == nextFrame) {
            key->startFrame = prevFrame << shift;
            key->endFrame = prevFrame << shift;
            key->start = currentFov;
            key->end = currentFov;
            continue;
        }

        if (pattern->mode >= 3 && pattern->mode < 5) {
            s32 duration =
                _wazaSequenceCameraSelectDuration__FUcPff(
                    pattern->durationMode, pattern->thresholds,
                    nextFrame - prevFrame);
            f32 radius = *(f32*)((u8*)params + 0x2C);

            span = lbl_8047E260 * params->scaleMin;
            if (params->scaleMax > span) {
                span = params->scaleMax;
            }

            lowFov = lbl_8047E270 *
                (lbl_8047E274 * atan2(lbl_8047E1F8 * span, radius));
            highFov = lbl_8047E270 *
                (lbl_8047E274 * atan2(lbl_8047E274 * span, radius));

            if (lowFov < lbl_8047E278) {
                lowFov = lbl_8047E278;
            }
            if (highFov < lbl_8047E278) {
                highFov = lbl_8047E278;
            }
            if (lowFov > lbl_8047E27C) {
                lowFov = lbl_8047E27C;
            }
            if (highFov > lbl_8047E27C) {
                highFov = lbl_8047E27C;
            }

            span = lbl_8047E1F4;
            lowFov = currentFov;
            hasFirst = FALSE;
            hasSecond = FALSE;
            if (pattern->flags & 1) {
                currentFov = lbl_8047E1FC;
                span = lbl_8047E1E0;
                hasFirst = TRUE;
            }
            if (pattern->flags & 2) {
                if (!hasFirst) {
                    currentFov = lbl_8047E280;
                }
                span = lbl_8047E284;
                hasSecond = TRUE;
            }
            if (pattern->flags & 4) {
                if (!hasSecond) {
                    currentFov = lbl_8047E260;
                }
                span = lbl_8047E1F4;
            }

            key->end = highFov + (lowFov - highFov) *
                (currentFov + (span - currentFov) * fn_800E0BE4());
            if (pattern->timingMode == 2) {
                key->endFrame = nextFrame << shift;
                key->startFrame = (nextFrame - duration) << shift;
            } else {
                key->startFrame = prevFrame << shift;
                key->endFrame = (prevFrame + duration) << shift;
            }
            key->start = lowFov;
        } else {
            key->startFrame = prevFrame << shift;
            key->endFrame = nextFrame << shift;
            key->start = currentFov;
            key->end = currentFov;
        }

        params = (WazaSequenceCameraParamsFov*)((u8*)params + 4);
        prevFrame = nextFrame;
    }
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
