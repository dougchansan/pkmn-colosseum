/**
 * @file fade_range_801C4CB8.c
 * @brief fade effect system, 0x801C4CB8 - 0x801C766C.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) -- mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 *
 * Boundary bug fix (battle_grid.c 5-way split, 2026-07-07): fn_801C4CB8
 * (0x704 bytes) was physically sitting in the old monolithic
 * game/battle/battle_grid.c despite being outside that file's own
 * declared splits.txt range (which ended at 0x801C4CB8, exclusive) --
 * i.e. it always belonged to this unit's range, just misplaced in
 * source. Relocated here so this unit scores real progress instead of
 * 0%. Registered by game/effect/fade_effect.c's
 * fadeEffectHookFunction_trainer_Init hook stub.
 */
#include "dolphin/types.h"

#ifndef FADE_EFFECT_TU
typedef struct GSvec {
    f32 x;
    f32 y;
    f32 z;
} GSvec;
#endif

typedef struct GSvec2 {
    f32 x;
    f32 y;
} GSvec2;

typedef struct FadeCameraWork {
    void* tex0;
    void* tex1;
    u16 frame;
    u16 unk0A;
    u16 unk0C;
    u16 unk0E;
    f32 step;
    f32 value;
    f32 target;
    u8 pad_1C[4];
} FadeCameraWork;

typedef struct FadeFluidWork {
    u32 columns;
    u32 rows;
    f32 accel;
    f32 damping;
    f32 neighbor;
    f32 limit;
    f32 timeStep;
    f32 cellSize;
    f32 xScale;
    f32 yScale;
    GSvec* heightPage[2];
    GSvec* velocityX;
    GSvec* velocityY;
    GSvec2* texCoord;
    u8 pad_3C[4];
} FadeFluidWork;

typedef struct FadeTrailPoint {
    f32 x;
    f32 y;
    f32 z;
    f32 angle;
    s32 alpha;
} FadeTrailPoint;

typedef struct FadeTrailWork {
    FadeTrailPoint point[12];
} FadeTrailWork;

extern void fn_801C6688(f32 t);
extern void fn_801C63C0(void* tex, GSvec* pos, f32 scale, f32 offset, f32 t, f32 alpha);
extern void fn_801C5B60(FadeTrailPoint* point, s32 alpha, f32 scale, f32 angle);
extern void fn_801C5D60(void);
extern void fn_801C673C(void);
extern void fn_801C680C(void* texture);
extern void fn_801C53BC(void* texture);
extern void gfxVtxDescFree(void* ptr);
extern void fn_800D3074(u32 enable);
extern void* GSresFindById(u32 resource);
extern void GSgfxBeginBackFBCapture(void* texture, u32 (*callback)(void), u32 arg);
extern void* gfxVtxDescAlloc(void);
extern void gfxVtxDescSetAttr(void* object, u32 arg1, u32 arg2, u32 arg3, u32 arg4,
                        u32 arg5, u32 arg6, u32 arg7);
extern void fn_800D9ED8(u32 enable);
extern void gfxSetFlags(u32 mask);
extern void gfxClearFlags(u32 mask);
extern void gfxSetOrtho(f32 left, f32 top, f32 right, f32 bottom);
extern void gfxSetBlendMode(u32 arg0, u32 arg1, u32 arg2);
extern void gfxSetUpdates(u32 arg0, u32 arg1, u32 arg2);
extern void gfxSetZMode(u32 arg0, u32 arg1, u32 arg2);
extern void gfxSetAlphaCompare(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
extern void gfxSetCullMode(u32 arg0);
extern void gfxSetPrimitive(u32 arg0);
extern void gfxSetVtxDesc(void* ptr);
extern void gfxLoadTexture(u32 arg0, void* ptr);
extern void gfxLoadTexMatrix(u32 arg0, u32 arg1, u32 arg2, void* ptr);
extern void fn_800DC1D4(u32 arg0);
extern void gfxSetTevStage(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4);
extern void fn_800DC14C(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
extern void fn_800DC0D4(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4);
extern void fn_800DC04C(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
extern void fn_800DBFD4(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4);
extern void fn_800E042C(void* mtx, GSvec* scale);
extern void GSmtxRotateZ(void* mtx, f32 angle);
extern void fn_800E03B4(void* mtx, GSvec* trans);
extern void GSvecTransform(GSvec* dst, void* mtx, GSvec* src);
extern void gfxBegin(u32 arg0);
extern void gfxVtxPos(f32 x, f32 y, f32 z);
#ifndef FADE_EFFECT_TU
extern void gfxVtxColor4u8(u32 arg0, u8 r, u8 g, u8 b, u8 a);
#endif
extern void gfxVtxTexCoord(u32 arg0, f32 s, f32 t);
extern void gfxVtxNormal3f32(f32 x, f32 y, f32 z);
extern void gfxEnd(void);
extern void spriteSetEnv(void);
#ifndef FADE_EFFECT_TU
extern void GSlerpGetLinearInterpolationVector(GSvec* out, GSvec* from,
                                               GSvec* to, f32 t);
#endif
extern void GSgfxEndBackFBCapture(void* texture);
extern u32 _fadeEffectGetRandom__FUl(u32 range);
extern u32 GSmemGetHandleByPtr(void* ptr);
extern void fn_800E24B0(u32 handle);
extern void GSmemFree(u32 handle);
extern u32 GSmemAlloc(u32 size, u32 align);
extern void* GSmemLock(u32 handle);

extern u8 lbl_80467030[0x20];
extern u8 lbl_80467050[0x40];
extern u8 lbl_80466E50[0x1E0];
extern f32 lbl_80478AC0[];
extern u8 lbl_80314AE8[];
extern u8 lbl_80315128[];
extern u8 lbl_8047B3B0;
extern void* lbl_8047B3B4;
extern u32 lbl_8047B3B8;

typedef union FadeFloatShape {
    f32 value;
    s32 bits;
} FadeFloatShape;

/* MSL <math.h> inline sqrtf used by this translation unit. */
static inline f32 fadeSqrtf(f32 value)
{
    FadeFloatShape shape;
    f64 estimate;
    s32 exponent;
    s32 fpclass;

    if (value > 0.0f) {
        const f64 half = 0.5;
        const f64 three = 3.0;

        estimate = __frsqrte(value);
        estimate = half * estimate * (three - value * (estimate * estimate));
        estimate = half * estimate * (three - value * (estimate * estimate));
        estimate = half * estimate * (three - value * (estimate * estimate));
        return (f32)(value * estimate);
    }
    if ((f64)value < 0.0) {
        return lbl_80478AC0[0];
    }

    shape.value = value;
    exponent = shape.bits & 0x7F800000;
    switch (exponent) {
    case 0x7F800000:
        fpclass = (shape.bits & 0x007FFFFF) != 0 ? 1 : 2;
        break;
    case 0:
        fpclass = (shape.bits & 0x007FFFFF) != 0 ? 5 : 3;
        break;
    default:
        fpclass = 4;
        break;
    }
    if (fpclass == 1) {
        return lbl_80478AC0[0];
    }
    return value;
}

u32 fn_801C63B8(void);
void fadeFluidQuit(void);
void fadeFluidEvaluate(void);
void fadeFluidInit(u32 columns, u32 rows, f32 cellSize, f32 calcStep,
                   f32 waveLimit, f32 timeStep);
void fadeFluidSetShock(GSvec* position, f32 strength);
void _fadeEffect_AdjustParms__Fv(void);
u32 fn_801C6908(u32 range);
void fn_801C5748(void);
void fn_801C6688(f32 t);
void fn_801C6934(void* texture, f32 progress, f32 alpha);
u32 fn_801C6008(u32 finish, void* texture, f32 frame, f32 duration,
                f32 angle, f32 angleDuration);
void _fadeEffectFunction_UDLR_FirstInit__FP9GStextureUs(void* texture,
                                                        u16 mode);

/* fade_effect.o functions: fade_range_801C6934.c (FADE_FLUID_TU) builds
 * only the fade_fluid.o range below. */
#ifndef FADE_FLUID_TU
u32 fn_801C4CB8(u32 finish, void* texture, f32 frame, f32 duration)
{
    FadeCameraWork* camera;
    f32 textureMatrix[3][4];
    f32 matrix[3][4];
    GSvec position;
    GSvec point;
    GSvec transformed;
    f32 scaleValue;
    f32 progress;
    f32 left;
    f32 right;
    f32 top;
    f32 bottom;

    if (texture == NULL) {
        return finish;
    }

    if (lbl_8047B3B0 == 1) {
        lbl_8047B3B0 = 0;
        fn_801C53BC(texture);
    }

    if (lbl_8047B3B0 == 0 && (u8)finish == 0 && lbl_8047B3B4 != NULL) {
        gfxVtxDescFree(lbl_8047B3B4);
        lbl_8047B3B4 = NULL;
    }

    progress = frame / duration;
    camera = (FadeCameraWork*)lbl_80467030;
    camera->frame++;
    camera->value += camera->step;
    if (camera->step >= 0.0f) {
        camera->step += 0.5f * progress;
        if (camera->value >= camera->target) {
            camera->value = camera->target;
        }
    } else {
        camera->step -= 0.5f * progress;
        if (camera->value <= camera->target) {
            camera->value = camera->target;
        }
    }

    position.x = 320.0f;
    position.y = 240.0f;
    position.z = 0.0f;
    fn_801C63C0(texture, &position, 1.0f, 0.0f, progress,
                0.0f);

    position.x = 320.0f + camera->value;
    position.y = 240.0f;
    position.z = 0.0f;
    fn_801C63C0(texture, &position, 1.0f, 0.0f, progress,
                0.5f);

    position.x = 320.0f - camera->value;
    position.y = 240.0f;
    position.z = 0.0f;
    fn_801C63C0(texture, &position, 1.0f, 0.0f, progress,
                0.5f);

    if (lbl_8047B3B4 != NULL) {
        fn_800D9ED8(1);
        gfxSetFlags(0x80000003);
        gfxClearFlags(4);
        gfxSetOrtho(0.0f, 0.0f, 640.0f, 480.0f);
        gfxSetBlendMode(1, 6, 7);
        gfxSetUpdates(1, 1, 0);
        gfxSetZMode(0, 1, 1);
        gfxSetAlphaCompare(0, 7, 0, 1, 7, 0);
        gfxSetCullMode(0);
        gfxLoadTexMatrix(0, 0, 4, textureMatrix);
        gfxLoadTexMatrix(1, 0, 5, textureMatrix);
        fn_800DC1D4(2);
        gfxLoadTexture(0, ((FadeCameraWork*)lbl_80467030)->tex1);
        gfxSetTevStage(0, 0, 0, 0, 0);
        fn_800DC14C(0, 8, 0, 0, 0, 1);
        fn_800DC0D4(0, 9, 13, 12, 15);
        fn_800DC04C(0, 0, 0, 0, 0, 1);
        fn_800DBFD4(0, 7, 4, 5, 7);
        gfxLoadTexture(1, texture);
        gfxSetTevStage(1, 0, 1, 1, 0);
        fn_800DC14C(1, 0, 0, 0, 0, 0);
        fn_800DC0D4(1, 15, 8, 2, 15);
        fn_800DC04C(1, 0, 0, 0, 0, 0);
        fn_800DBFD4(1, 7, 7, 7, 1);
        gfxSetVtxDesc(lbl_8047B3B4);
        gfxSetPrimitive(4);

        position.x = 320.0f;
        position.y = 240.0f;
        position.z = 0.0f;
        scaleValue = 1.0f + ((f32)camera->frame / 150.0f);
        point.x = scaleValue;
        point.y = scaleValue;
        point.z = scaleValue;
        progress = 255.0f * (1.0f - progress);
        fn_800E042C(matrix, &point);
        GSmtxRotateZ(matrix, 0.0f);
        fn_800E03B4(matrix, &position);

        point.x = 128.0f;
        point.y = 128.0f;
        point.z = 0.0f;
        GSvecTransform(&transformed, matrix, &point);
        transformed.x -= 320.0f;
        transformed.y -= 240.0f;
        left = (320.0f - transformed.x) / 640.0f;
        right = (320.0f + transformed.x) / 640.0f;
        top = (240.0f - transformed.y) / 480.0f;
        bottom = (240.0f + transformed.y) / 480.0f;
        gfxBegin(4);

        point.x = (-128.0f);
        point.y = (-128.0f);
        point.z = 0.0f;
        GSvecTransform(&transformed, matrix, &point);
        gfxVtxPos(transformed.x, transformed.y, transformed.z);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, (u8)progress);
        gfxVtxTexCoord(0, 0.0f, 0.0f);
        gfxVtxTexCoord(1, left, top);

        point.x = 128.0f;
        point.y = (-128.0f);
        point.z = 0.0f;
        GSvecTransform(&transformed, matrix, &point);
        gfxVtxPos(transformed.x, transformed.y, transformed.z);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, (u8)progress);
        gfxVtxTexCoord(0, 1.0f, 0.0f);
        gfxVtxTexCoord(1, right, top);

        point.x = (-128.0f);
        point.y = 128.0f;
        point.z = 0.0f;
        GSvecTransform(&transformed, matrix, &point);
        gfxVtxPos(transformed.x, transformed.y, transformed.z);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, (u8)progress);
        gfxVtxTexCoord(0, 0.0f, 1.0f);
        gfxVtxTexCoord(1, left, bottom);

        point.x = 128.0f;
        point.y = 128.0f;
        point.z = 0.0f;
        GSvecTransform(&transformed, matrix, &point);
        gfxVtxPos(transformed.x, transformed.y, transformed.z);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, (u8)progress);
        gfxVtxTexCoord(0, 1.0f, 1.0f);
        gfxVtxTexCoord(1, right, bottom);
        gfxEnd();
        fn_800DC1D4(1);
        gfxClearFlags(0x80000000);
        fn_800D9ED8(0);
    }

    if (lbl_8047B3B0 == 0 && (u8)finish == 0) {
        GSgfxEndBackFBCapture(texture);
    }
    return finish;
}

void fn_801C53BC(void* texture)
{
    fn_800D3074(1);
    ((FadeCameraWork*)lbl_80467030)->tex0 = GSresFindById(0x0F861200);
    ((FadeCameraWork*)lbl_80467030)->tex1 = GSresFindById(0x0F871200);
    {
    FadeCameraWork* camera = (FadeCameraWork*)lbl_80467030;
    camera->frame = 0;
    camera->unk0A = 0;
    camera->unk0C = 0;
    camera->unk0E = 0;
    camera->step = 0.1f;
    camera->target = 16.0f;
    camera->value = 0.0f;
    }
    GSgfxBeginBackFBCapture(texture, fn_801C63B8, 0);

    lbl_8047B3B4 = gfxVtxDescAlloc();
    if (lbl_8047B3B4 != NULL) {
        gfxVtxDescSetAttr(lbl_8047B3B4, 1, 0, 1, 4, 0, 0, 0);
        gfxVtxDescSetAttr(lbl_8047B3B4, 4, 0, 6, 10, 0, 0, 0);
        gfxVtxDescSetAttr(lbl_8047B3B4, 6, 0, 8, 4, 0, 0, 0);
        gfxVtxDescSetAttr(lbl_8047B3B4, 7, 0, 8, 4, 0, 0, 0);
    }
    _fadeEffect_AdjustParms__Fv();
}

u32 fn_801C54FC(u32 arg0, f32 frame, f32 duration) {
    fn_801C6688(frame / duration);
    return arg0;
}

u32 fn_801C5530(u32 arg0, void* texture, f32 frame, f32 duration, f32 angle, f32 angleDuration) {
    u32 result;
    void* tex;
    f32 t;
    f32 rot;

    result = arg0;
    tex = texture;
    t = frame / duration;
    rot = angle / angleDuration;

    fn_801C6688(t);
    if (tex == NULL) {
        return result;
    }

    {
        GSvec pos;

        pos.x = 320.0f;
        pos.y = 240.0f;
        pos.z = 0.0f;
        fn_801C63C0(tex, &pos, 1.0f, 0.0f, t, rot);
    }
    return result;
}

u32 fn_801C55D8(u32 finish, void* texture, f32 frame, f32 duration,
                f32 angle, f32 angleDuration)
{
    /* RULE-EXCEPTION(title-path): parameter copies whose only effect is the
     * prologue's copy order (texture last, as in fn_801C5ED0) - see
     * docs/RULE_EXCEPTIONS.md */
    f32 frameLocal = frame;
    u32 result = finish;
    f32 durationLocal = duration;
    f32 angleLocal = angle;
    f32 angleDurationLocal = angleDuration;
    void* const tex = texture;
    FadeCameraWork* camera;
    f32 progress;
    f32 alpha;

    if (tex == NULL) {
        return result;
    }
    if (lbl_8047B3B0 == 1) {
        lbl_8047B3B0 = 0;
        fn_801C5748();
    }
    if (lbl_8047B3B0 == 0 && (u8)result == 0) {
        fadeFluidQuit();
    }

    progress = frameLocal / durationLocal;
    alpha = angleLocal / angleDurationLocal;
    camera = (FadeCameraWork*)lbl_80467030;
    camera->frame++;
    camera->value += camera->step;
    if (camera->step >= 0.0f) {
        camera->step += 0.5f * progress;
        if (camera->value >= camera->target) {
            camera->value = camera->target;
        }
    } else {
        camera->step -= 0.5f * progress;
        if (camera->value <= camera->target) {
            camera->value = camera->target;
        }
    }

    fn_801C6688(progress);
    if ((u8)result == 1) {
        fadeFluidEvaluate();
        fn_801C6934(tex, progress, alpha);
    }
    return result;
}

void fn_801C5748(void)
{
    GSvec position;

    fn_800D3074(1);
    ((FadeCameraWork*)lbl_80467030)->tex0 = GSresFindById(0x0F861200);
    ((FadeCameraWork*)lbl_80467030)->tex1 = GSresFindById(0x0F871200);
    ((FadeCameraWork*)lbl_80467030)->frame = 0;
    ((FadeCameraWork*)lbl_80467030)->unk0A = 0;
    ((FadeCameraWork*)lbl_80467030)->unk0C = 0;
    ((FadeCameraWork*)lbl_80467030)->unk0E = 0;
    ((FadeCameraWork*)lbl_80467030)->step = 0.25f;
    ((FadeCameraWork*)lbl_80467030)->target = 32.0f;
    ((FadeCameraWork*)lbl_80467030)->value = 0.0f;

    fadeFluidInit(40, 30, 1.0f, 1.0f, 0.7f,
                  0.001f);

    position.x = 20.0f;
    position.y = 15.0f;
    position.z = 0.0f;
    fadeFluidSetShock(&position, 16.0f);
    position.x = 6.25f;
    position.y = 6.25f;
    position.z = 0.0f;
    fadeFluidSetShock(&position, 8.0f);
    position.x = 33.75f;
    position.y = 23.75f;
    position.z = 0.0f;
    fadeFluidSetShock(&position, 8.0f);
    position.x = 33.75f;
    position.y = 6.25f;
    position.z = 0.0f;
    fadeFluidSetShock(&position, 8.0f);
    position.x = 6.25f;
    position.y = 23.75f;
    position.z = 0.0f;
    fadeFluidSetShock(&position, 8.0f);
    _fadeEffect_AdjustParms__Fv();
}

/* RULE-EXCEPTION(title-path): pool constants written as one-element const
 * arrays so MWCC loads them instead of folding them - see
 * docs/RULE_EXCEPTIONS.md. As literals, 896.0f becomes a hoisted temporary
 * and the four saved FPRs rotate; as loads, fn_801C5898 matches and the
 * four land at 0x8047E04C - 0x8047E05C in the TU pool, where retail has
 * them. */
static const f32 lbl_8047E04C[1] = { 360.0f };
static const f32 lbl_8047E050[1] = { 804.24774f };
static const f32 lbl_8047E054[1] = { 896.0f };
static const f32 lbl_8047E058[1] = { -256.0f };

u32 fn_801C5898(u32 arg0, void* texture, f32 frame, f32 duration,
                f32 angle, f32 angleDuration)
{
    FadeCameraWork* camera;
    FadeTrailWork* trail;
    GSvec position;
    f32 positionStep[2];
    f32 angleStep[2];
    f32 alphaScale;
    f32 maxPosition;
    f32 progress;
    f32 angleProgress;
    f32 value;
    s32 i;
    s32 j;

    if (lbl_8047B3B0 == 1) {
        lbl_8047B3B0 = 0;
        fn_801C5D60();
    }

    progress = frame / duration;
    angleProgress = angle / angleDuration;
    camera = (FadeCameraWork*)lbl_80467030;
    camera->frame++;
    camera->value += camera->step;
    if (camera->value >= camera->target) {
        camera->value = camera->target;
    }
    fn_801C6688(progress);

    if (texture == NULL) {
        return arg0;
    }

    position.x = 320.0f;
    position.y = 240.0f;
    position.z = 0.0f;
    fn_801C63C0(texture, &position, 1.0f, 0.0f,
                progress, angleProgress);
    fn_801C680C(((FadeCameraWork*)lbl_80467030)->tex0);

    value = camera->value;
    positionStep[0] = value;
    positionStep[1] = -value;
    angleStep[0] = lbl_8047E04C[0] * (value / lbl_8047E050[0]);
    angleStep[1] = -angleStep[0];
    alphaScale = 1.0f - progress;

    trail = (FadeTrailWork*)lbl_80466E50;
    for (i = 0; i < 2; i++) {
        for (j = 11; j > 0; j--) {
            FadeTrailPoint* point = &trail[i].point[j];

            if ((camera->frame & 3) == 0) {
                point->x = point[-1].x;
                point->y = point[-1].y;
                point->z = point[-1].z;
                point->angle = point[-1].angle;
            }
            fn_801C5B60(point,
                        (s32)(alphaScale * (f32)point->alpha),
                        1.0f, point->angle);
        }

        value = trail[i].point[0].angle + angleStep[i];
        if (value <= 0.0f) {
            value += lbl_8047E04C[0];
        } else if (value >= lbl_8047E04C[0]) {
            value -= lbl_8047E04C[0];
        }
        trail[i].point[0].angle = value;

        maxPosition = lbl_8047E054[0];
        value = trail[i].point[0].x + positionStep[i];
        if (value > maxPosition) {
            value = maxPosition;
        } else if (value < lbl_8047E058[0]) {
            value = lbl_8047E058[0];
        }
        trail[i].point[0].x = value;

        fn_801C5B60(&trail[i].point[0],
                    (s32)(alphaScale * (f32)trail[i].point[0].alpha),
                    1.0f, trail[i].point[0].angle);
    }
    fn_801C673C();
    return arg0;
}

void fn_801C5B60(FadeTrailPoint* position, s32 alpha, f32 scale, f32 angle)
{
    f32 matrix[3][4];
    GSvec point;
    GSvec transformed;

    point.x = scale;
    point.y = scale;
    point.z = scale;
    fn_800E042C(matrix, &point);
    GSmtxRotateZ(matrix, 0.017453292f * angle);
    fn_800E03B4(matrix, (GSvec*)position);
    gfxBegin(4);

    point.x = (-128.0f);
    point.y = (-128.0f);
    point.z = 0.0f;
    GSvecTransform(&transformed, matrix, &point);
    gfxVtxPos(transformed.x, transformed.y, transformed.z);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
    gfxVtxTexCoord(0, 0.0f, 0.0f);

    point.x = 128.0f;
    point.y = (-128.0f);
    point.z = 0.0f;
    GSvecTransform(&transformed, matrix, &point);
    gfxVtxPos(transformed.x, transformed.y, transformed.z);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
    gfxVtxTexCoord(0, 1.0f, 0.0f);

    point.x = (-128.0f);
    point.y = 128.0f;
    point.z = 0.0f;
    GSvecTransform(&transformed, matrix, &point);
    gfxVtxPos(transformed.x, transformed.y, transformed.z);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
    gfxVtxTexCoord(0, 0.0f, 1.0f);

    point.x = 128.0f;
    point.y = 128.0f;
    point.z = 0.0f;
    GSvecTransform(&transformed, matrix, &point);
    gfxVtxPos(transformed.x, transformed.y, transformed.z);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
    gfxVtxTexCoord(0, 1.0f, 1.0f);
    gfxEnd();
}

void fn_801C5D60(void)
{
    FadeTrailWork* trails = (FadeTrailWork*)lbl_80466E50;
    GSvec start[2];
    s32 side;
    s32 point;

    fn_800D3074(1);
    ((FadeCameraWork*)lbl_80467030)->tex0 = GSresFindById(0x0F861200);
    ((FadeCameraWork*)lbl_80467030)->tex1 = GSresFindById(0x0F871200);
    ((FadeCameraWork*)lbl_80467030)->frame = 0;
    ((FadeCameraWork*)lbl_80467030)->unk0A = 0;
    ((FadeCameraWork*)lbl_80467030)->unk0C = 0;
    ((FadeCameraWork*)lbl_80467030)->unk0E = 0;
    ((FadeCameraWork*)lbl_80467030)->step = 4.0f;
    ((FadeCameraWork*)lbl_80467030)->value = 0.0f;
    ((FadeCameraWork*)lbl_80467030)->target = 48.0f;

    start[0].x = (-128.0f);
    start[0].y = 364.0f;
    start[0].z = 0.0f;
    start[1].x = 768.0f;
    start[1].y = 116.0f;
    start[1].z = 0.0f;

    for (side = 0; side < 2; side++) {
        for (point = 0; point < 12; point++) {
            trails[side].point[point].x = start[side].x;
            trails[side].point[point].y = start[side].y;
            trails[side].point[point].z = start[side].z;
            trails[side].point[point].angle = 0.0f;
            trails[side].point[point].alpha = 0xFF - point * 0x15;
        }
    }
    _fadeEffect_AdjustParms__Fv();
}

u32 fn_801C5ED0(u32 arg0, void* texture, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    f32 arg2Local = arg2;
    u32 arg0Local = arg0;
    f32 arg3Local = arg3;
    f32 arg4Local = arg4;
    f32 arg5Local = arg5;
    void* const tex = texture;

    if (tex == NULL) {
        return arg0Local;
    }

    if (lbl_8047B3B0 == 1) {
        lbl_8047B3B0 = 0;
        _fadeEffectFunction_UDLR_FirstInit__FP9GStextureUs(tex, 8);
    }

    return fn_801C6008(arg0Local, tex, arg2Local, arg3Local, arg4Local, arg5Local);
}

u32 fn_801C5F6C(u32 arg0, void* texture, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    f32 arg2Local = arg2;
    u32 arg0Local = arg0;
    f32 arg3Local = arg3;
    f32 arg4Local = arg4;
    f32 arg5Local = arg5;
    void* const tex = texture;

    if (tex == NULL) {
        return arg0Local;
    }

    if (lbl_8047B3B0 == 1) {
        lbl_8047B3B0 = 0;
        _fadeEffectFunction_UDLR_FirstInit__FP9GStextureUs(tex, 2);
    }

    return fn_801C6008(arg0Local, tex, arg2Local, arg3Local, arg4Local, arg5Local);
}

/* RULE-EXCEPTION(title-path): pool constant declared here and defined after
 * fn_801C6008 - see docs/RULE_EXCEPTIONS.md. As a literal, MWCC puts -1.5f
 * first in the multiply (retail multiplies pulse by it); declared ahead and
 * defined after the function, it is loaded, and it lands at 0x8047E088
 * between fn_801C6008's literals and UDLR_FirstInit's. */
extern const f32 lbl_8047E088[1];

u32 fn_801C6008(u32 finish, void* texture, f32 frame, f32 duration,
                f32 angle, f32 angleDuration)
{
    GSvec position;
    f32 progress = frame / duration;
    f32 pulse;

    ((FadeCameraWork*)lbl_80467030)->frame++;
    ((FadeCameraWork*)lbl_80467030)->value += ((FadeCameraWork*)lbl_80467030)->step;
    if (((FadeCameraWork*)lbl_80467030)->step >= 0.0f) {
        ((FadeCameraWork*)lbl_80467030)->step += 0.5f * progress;
        if (((FadeCameraWork*)lbl_80467030)->value >= ((FadeCameraWork*)lbl_80467030)->target) {
            ((FadeCameraWork*)lbl_80467030)->value = ((FadeCameraWork*)lbl_80467030)->target;
        }
    } else {
        ((FadeCameraWork*)lbl_80467030)->step -= 0.5f * progress;
        if (((FadeCameraWork*)lbl_80467030)->value <= ((FadeCameraWork*)lbl_80467030)->target) {
            ((FadeCameraWork*)lbl_80467030)->value = ((FadeCameraWork*)lbl_80467030)->target;
        }
    }

    position.x = 320.0f;
    position.y = 240.0f;
    position.z = 0.0f;
    fn_801C63C0(texture, &position, 1.0f, 0.0f,
                progress, 0.0f);

    switch (((FadeCameraWork*)lbl_80467030)->unk0C) {
    case 1:
    case 2:
        position.x = 320.0f + ((FadeCameraWork*)lbl_80467030)->value;
        position.y = 240.0f;
        break;
    case 4:
    case 8:
        position.x = 320.0f;
        position.y = 240.0f + ((FadeCameraWork*)lbl_80467030)->value;
        break;
    }

    position.z = 0.0f;
    pulse = (f32)((FadeCameraWork*)lbl_80467030)->frame /
            (45.0f * (1.0f - progress) + 1.0f);
    fn_801C63C0(texture, &position, 1.0f, pulse, progress,
                0.4f);

    switch (((FadeCameraWork*)lbl_80467030)->unk0C) {
    case 1:
    case 2:
        position.x = 320.0f + 1.5f * ((FadeCameraWork*)lbl_80467030)->value;
        break;
    case 4:
    case 8:
        position.y = 240.0f + 1.5f * ((FadeCameraWork*)lbl_80467030)->value;
        break;
    }
    fn_801C63C0(texture, &position, 1.0f,
                pulse * lbl_8047E088[0], progress, 0.4f);

    if (lbl_8047B3B0 == 0 && (u8)finish == 0) {
        GSgfxEndBackFBCapture(texture);
    }
    return finish;
}

__declspec(section ".sdata2") const f32 lbl_8047E088[1] = { -1.5f };

void _fadeEffectFunction_UDLR_FirstInit__FP9GStextureUs(void* texture, u16 mode)
{
    FadeCameraWork* camera;

    fn_800D3074(1);
    ((FadeCameraWork*)lbl_80467030)->tex0 = GSresFindById(0x0F861200);
    ((FadeCameraWork*)lbl_80467030)->tex1 = GSresFindById(0x0F871200);
    camera = (FadeCameraWork*)lbl_80467030;
    camera->frame = 0;
    camera->unk0A = 0;
    camera->unk0C = mode;
    camera->unk0E = 0;
    camera->step = 0.3f;
    camera->target = 128.0f;
    camera->value = 0.0f;

    if (fn_801C6908(2) == 0) {
        camera->step *= (-1.0f);
        camera->target *= (-1.0f);
        switch (mode) {
        case 1:
            camera->unk0C = 2;
            break;
        case 2:
            camera->unk0C = 1;
            break;
        case 4:
            camera->unk0C = 8;
            break;
        case 8:
            camera->unk0C = 4;
            break;
        }
    }
    _fadeEffect_AdjustParms__Fv();
    GSgfxBeginBackFBCapture(texture, fn_801C63B8, 0);
}

u32 fn_801C63B8(void) {
    return 1;
}

void fn_801C63C0(void* texture, GSvec* position, f32 scale, f32 angle,
                 f32 fade, f32 blend)
{
    GSvec interpolated;
    GSvec vec;
    GSvec out;
    f32 matrix[3][4];
    s32 alpha;

    fn_801C680C(texture);
    vec.x = 1.0f;
    vec.y = 1.0f;
    vec.z = 1.0f;
    out.x = 0.0f;
    out.y = 0.0f;
    out.z = 0.0f;
    GSlerpGetLinearInterpolationVector(&interpolated, &vec, &out, blend);
    alpha = (s32)(255.0f * interpolated.x);
    alpha = (s32)((f32)alpha * (1.0f - fade));
    vec.x = scale;
    vec.y = scale;
    vec.z = scale;
    fn_800E042C(matrix, &vec);
    GSmtxRotateZ(matrix, 0.017453292f * angle);
    fn_800E03B4(matrix, position);
    gfxBegin(4);

    vec.x = (-320.0f);
    vec.y = (-240.0f);
    vec.z = 0.0f;
    GSvecTransform(&out, matrix, &vec);
    gfxVtxPos(out.x, out.y, out.z);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, (u8)alpha);
    gfxVtxTexCoord(0, 0.0f, 0.0f);

    vec.x = 321.0f;
    vec.y = (-240.0f);
    vec.z = 0.0f;
    GSvecTransform(&out, matrix, &vec);
    gfxVtxPos(out.x, out.y, out.z);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, (u8)alpha);
    gfxVtxTexCoord(0, 1.0f, 0.0f);

    vec.x = (-320.0f);
    vec.y = 241.0f;
    vec.z = 0.0f;
    GSvecTransform(&out, matrix, &vec);
    gfxVtxPos(out.x, out.y, out.z);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, (u8)alpha);
    gfxVtxTexCoord(0, 0.0f, 1.0f);

    vec.x = 321.0f;
    vec.y = 241.0f;
    vec.z = 0.0f;
    GSvecTransform(&out, matrix, &vec);
    gfxVtxPos(out.x, out.y, out.z);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, (u8)alpha);
    gfxVtxTexCoord(0, 1.0f, 1.0f);
    gfxEnd();
    fn_801C673C();
}

void fn_801C6688(f32 t) {
    extern void fn_801C673C(void);
    extern void fn_801C6760(void);
    s32 alpha;

    fn_801C6760();
    alpha = 0xFF - (s32)(255.0f * t);
    gfxBegin(2);
    gfxVtxPos(0.0f, 0.0f, 0.0f);
    gfxVtxColor4u8(0, 0, 0, 0, alpha);
    gfxVtxPos(640.0f, 480.0f, 0.0f);
    gfxVtxColor4u8(0, 0, 0, 0, alpha);
    gfxEnd();
    fn_801C673C();
}

void fn_801C673C(void) {
    fn_800D9ED8(0);
}

void fn_801C6760(void) {
    fn_800D9ED8(1);
    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetOrtho(0.0f, 0.0f, 640.0f, 480.0f);
    gfxSetBlendMode(1, 6, 7);
    gfxSetUpdates(1, 1, 0);
    gfxSetZMode(0, 1, 1);
    gfxSetAlphaCompare(0, 7, 0, 1, 7, 0);
    gfxSetCullMode(0);
    gfxSetPrimitive(7);
    gfxSetVtxDesc(NULL);
}

void fn_801C680C(void* texture) {
    fn_800D9ED8(1);
    gfxSetFlags(3);
    gfxClearFlags(4);
    gfxSetOrtho(0.0f, 0.0f, 640.0f, 480.0f);
    gfxSetBlendMode(1, 6, 7);
    gfxSetUpdates(1, 1, 0);
    gfxSetZMode(0, 1, 1);
    gfxSetAlphaCompare(0, 7, 0, 1, 7, 0);
    gfxSetCullMode(0);
    gfxSetPrimitive(4);
    gfxSetVtxDesc(lbl_80314AE8);
    gfxLoadTexture(0, texture);
}

void _fadeEffect_AdjustParms__Fv(void) {
    FadeCameraWork* cam = (FadeCameraWork*)lbl_80467030;
    f32 scale = 3.0f;

    cam->step = cam->step / scale;
    cam->value = cam->value / scale;
    cam->target = cam->target / scale;
}

u32 fn_801C6908(u32 range) {
    return _fadeEffectGetRandom__FUl(range);
}

void fn_801C6928(void) {
    lbl_8047B3B0 = 1;
}

#endif /* !FADE_FLUID_TU */

#ifndef FADE_EFFECT_TU
/* fade_fluid.o (XD fade_fluid.cpp): 0x801C6934 - 0x801C766C, its own TU
 * with its own .sdata2 pool from 0x8047E0A8. */
void fn_801C6AE8(u32 x, u32 y, u8 alpha);

void fn_801C6934(void* texture, f32 progress, f32 blend)
{
    FadeFluidWork* fluid;
    GSvec interpolated;
    GSvec to;
    GSvec from;
    s32 alpha;
    u32 x;
    u32 y;

    to.z = to.y = to.x = 1.0f;
    from.z = from.y = from.x = 0.0f;
    GSlerpGetLinearInterpolationVector(&interpolated, &to, &from, blend);
    alpha = (s32)(255.0f * interpolated.x);
    alpha = (s32)((f32)alpha * (1.0f - progress));

    fn_800D9ED8(1);
    gfxSetFlags(3);
    gfxClearFlags(4);
    spriteSetEnv();
    gfxSetBlendMode(1, 6, 7);
    gfxSetUpdates(1, 1, 0);
    gfxSetZMode(0, 1, 1);
    gfxSetAlphaCompare(0, 7, 0, 1, 7, 0);
    gfxSetCullMode(0);
    gfxSetPrimitive(4);
    gfxSetVtxDesc(lbl_80315128);
    gfxLoadTexture(0, texture);
    fluid = (FadeFluidWork*)lbl_80467050;

    for (y = 0; y < fluid->rows; y++) {
        for (x = 0; x < fluid->columns; x++) {
            fn_801C6AE8(x, y, (u8)alpha);
        }
    }
    fn_800D9ED8(0);
}

void fn_801C6AE8(u32 x, u32 y, u8 alpha) {
    FadeFluidWork* dimensions;
    FadeFluidWork* fluid;
    GSvec** heightPage;
    GSvec* position;
    GSvec* velocity;
    GSvec2* texCoord;
    u32 nextX;
    u32 stride;
    u32 index;
    f32 xScale;
    f32 yScale;

    dimensions = (FadeFluidWork*)&lbl_80467050;
    stride = dimensions->columns + 1;
    gfxBegin(4);

    fluid = (FadeFluidWork*)&lbl_80467050;
    xScale = fluid->xScale;
    yScale = fluid->yScale;
    heightPage = fluid->heightPage;
    index = x + y * stride;
    position = &heightPage[lbl_8047B3B8][index];
    velocity = &fluid->velocityX[index];
    texCoord = &fluid->texCoord[index];
    gfxVtxPos(position->x * xScale,
                position->y * yScale,
                position->z * xScale);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
    gfxVtxTexCoord(0, texCoord->x, texCoord->y);
    gfxVtxNormal3f32(velocity->x, velocity->y, velocity->z);

    nextX = x + 1;
    index = nextX + y * stride;
    position = &heightPage[lbl_8047B3B8][index];
    velocity = &fluid->velocityX[index];
    texCoord = &fluid->texCoord[index];
    gfxVtxPos(position->x * xScale,
                position->y * yScale,
                position->z * xScale);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
    gfxVtxTexCoord(0, texCoord->x, texCoord->y);
    gfxVtxNormal3f32(velocity->x, velocity->y, velocity->z);

    {
        GSvec* cornerPosition;
        u32 nextY = y + 1;

        cornerPosition = &heightPage[lbl_8047B3B8][x + nextY * stride];
        velocity = &fluid->velocityX[x + nextY * stride];
        index = x + nextY * stride;
        texCoord = &fluid->texCoord[index];
        gfxVtxPos(cornerPosition->x * xScale,
                    cornerPosition->y * yScale,
                    cornerPosition->z * xScale);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
        gfxVtxTexCoord(0, texCoord->x, texCoord->y);
        gfxVtxNormal3f32(velocity->x, velocity->y, velocity->z);

        index = nextX + nextY * stride;
        position = &heightPage[lbl_8047B3B8][index];
        velocity = &fluid->velocityX[index];
        texCoord = &fluid->texCoord[index];
        gfxVtxPos(position->x * xScale,
                    position->y * yScale,
                    position->z * xScale);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
        gfxVtxTexCoord(0, texCoord->x, texCoord->y);
        gfxVtxNormal3f32(velocity->x, velocity->y, velocity->z);
    }

    gfxEnd();
}

void _fadeFluidSetShockSub__FUlUlf(u32 x, u32 y, f32 strength);

void fadeFluidSetShock(GSvec* position, f32 strength) {
    FadeFluidWork* fluid = (FadeFluidWork*)lbl_80467050;
    f32 cellSize = fluid->cellSize;
    u32 x = position->x / cellSize;
    u32 y = position->y / cellSize;

    _fadeFluidSetShockSub__FUlUlf(x, y, strength);
    strength *= 0.5f;
    _fadeFluidSetShockSub__FUlUlf(x - 1, y - 1, strength);
    _fadeFluidSetShockSub__FUlUlf(x, y - 1, strength);
    _fadeFluidSetShockSub__FUlUlf(x + 1, y - 1, strength);
    _fadeFluidSetShockSub__FUlUlf(x - 1, y, strength);
    _fadeFluidSetShockSub__FUlUlf(x + 1, y, strength);
    _fadeFluidSetShockSub__FUlUlf(x - 1, y + 1, strength);
    _fadeFluidSetShockSub__FUlUlf(x, y + 1, strength);
    _fadeFluidSetShockSub__FUlUlf(x + 1, y + 1, strength);
}

void _fadeFluidSetShockSub__FUlUlf(u32 x, u32 y, f32 strength) {
    FadeFluidWork* fluid = (FadeFluidWork*)lbl_80467050;
    u32 columns = fluid->columns;

    if (x > columns) {
        return;
    }
    if (y > fluid->rows) {
        return;
    }

    ((f32*)fluid->heightPage[lbl_8047B3B8])
        [3 * (x + (y * (columns + 1))) + 2] -= strength;
}

void fadeFluidEvaluate(void) {
    FadeFluidWork* fluid = (FadeFluidWork*)lbl_80467050;
    FadeFluidWork* work;
    u32 rowStride = fluid->columns + 1;
    GSvec* source;
    s32 x;
    s32 y;
    f32 accel = fluid->accel;
    f32 damping = fluid->damping;
    f32 neighbor = fluid->neighbor;

    for (y = 1; y < fluid->rows; y++) {
        u32 row = y * rowStride;
        GSvec* destination;
        source = ((FadeFluidWork*)lbl_80467050)->heightPage[lbl_8047B3B8] +
                 row;
        destination =
            ((FadeFluidWork*)lbl_80467050)->heightPage[1 - lbl_8047B3B8] + row;

        for (x = 1; x < fluid->columns; x++) {
            destination[x].z =
                accel * source[x].z + damping * destination[x].z + neighbor *
                    (source[x + 1].z + source[x - 1].z +
                     source[x + rowStride].z + source[x - rowStride].z);
        }
    }

    lbl_8047B3B8 = 1 - lbl_8047B3B8;
    work = (FadeFluidWork*)lbl_80467050;
    for (y = 1; y < fluid->rows; y++) {
        u32 row = y * rowStride;
        GSvec* height =
            ((FadeFluidWork*)lbl_80467050)->heightPage[lbl_8047B3B8] + row;
        GSvec* velocityX = work->velocityX + row;
        GSvec* velocityY = work->velocityY + row;

        for (x = 1; x < work->columns; x++) {
            velocityX[x].x = height[x - 1].z - height[x + 1].z;
            velocityX[x].y =
                height[x - rowStride].z - height[x + rowStride].z;
            velocityY[x].z = height[x + 1].z - height[x - 1].z;
        }
    }
}


/*
 * Wave-equation coefficients for one step dt (the usual height-field fluid
 * scheme): f1 = c^2 dt^2 / d^2, f2 = 1 / (mu dt + 2), then
 * accel = (4 - 8 f1) f2, damping = (mu dt - 2) f2, neighbor = 2 f1 f2.
 */
void fadeFluidCalcParms(f32 dt) {
    FadeFluidWork* fluid = (FadeFluidWork*)lbl_80467050;
    f32 c = fluid->limit;
    f32 d = fluid->cellSize;
    f32 c2 = c * c;
    f32 d2 = d * d;
    f32 mt = fluid->timeStep * dt;
    f32 ct = dt * c2;
    f32 f1 = (dt * ct) / d2;
    f32 f2 = 1.0f / (2.0f + mt);
    f32 k1;
    f32 k2;
    f32 k3;

    k1 = 4.0f - 8.0f * f1;
    fluid->accel = k1 * f2;
    k2 = mt - 2.0f;
    fluid->damping = f2 * k2;
    k3 = 2.0f * f1;
    fluid->neighbor = k3 * f2;
}


void fadeFluidQuit(void) {
    extern void fn_801C75EC(void* ptr);
    FadeFluidWork* fluid0 = (FadeFluidWork*)lbl_80467050;
    FadeFluidWork* fluid1;
    FadeFluidWork* fluid2;
    FadeFluidWork* fluid3;
    FadeFluidWork* fluid4;

    if (fluid0->heightPage[0] != NULL) {
        fn_801C75EC(fluid0->heightPage[0]);
    }

    fluid1 = (FadeFluidWork*)lbl_80467050;
    if (fluid1->heightPage[1] != NULL) {
        fn_801C75EC(fluid1->heightPage[1]);
    }

    fluid2 = (FadeFluidWork*)lbl_80467050;
    if (fluid2->velocityX != NULL) {
        fn_801C75EC(fluid2->velocityX);
    }

    fluid3 = (FadeFluidWork*)lbl_80467050;
    if (fluid3->velocityY != NULL) {
        fn_801C75EC(fluid3->velocityY);
    }

    fluid4 = (FadeFluidWork*)lbl_80467050;
    if (fluid4->texCoord != NULL) {
        fn_801C75EC(fluid4->texCoord);
    }

    fluid0->heightPage[0] = NULL;
    fluid1->heightPage[1] = NULL;
    fluid2->velocityX = NULL;
    fluid3->velocityY = NULL;
    fluid4->texCoord = NULL;
}

void fadeFluidInit(u32 columns, u32 rows, f32 cellSize, f32 calcStep,
                   f32 waveLimit, f32 timeStep)
{
    extern void* fn_801C7630(u32 size);
    extern void set__5GSvecFfff(GSvec*, f32, f32, f32);
    extern void OSReport(const char*, ...);
    extern char lbl_80275860[];

    FadeFluidWork* fluid = (FadeFluidWork*)lbl_80467050;
    u32 pointCount;
    u32 vectorBytes;
    s32 x;
    s32 y;
    u32 index;
    f32 velocity;
    f32 texStepY;
    f32 texStepX;
    f32 maximumLimit;

    fluid->xScale = 640.0f / (f32)columns;
    pointCount = (columns + 1) * (rows + 1);
    vectorBytes = pointCount * sizeof(GSvec);
    fluid->yScale = 480.0f / (f32)rows;
    fluid->columns = columns;
    fluid->rows = rows;
    ((FadeFluidWork*)lbl_80467050)->heightPage[0] = fn_801C7630(vectorBytes);
    ((FadeFluidWork*)lbl_80467050)->heightPage[1] = fn_801C7630(vectorBytes);
    lbl_8047B3B8 = 0;
    ((FadeFluidWork*)lbl_80467050)->velocityX = fn_801C7630(vectorBytes);
    ((FadeFluidWork*)lbl_80467050)->velocityY = fn_801C7630(vectorBytes);
    ((FadeFluidWork*)lbl_80467050)->texCoord = fn_801C7630(pointCount * sizeof(GSvec2));

    maximumLimit = (cellSize / (2.0f * calcStep)) *
                   fadeSqrtf(2.0f + timeStep);
    if (!(0.0f < waveLimit && waveLimit < maximumLimit)) {
        OSReport(lbl_80275860, waveLimit, maximumLimit);
        waveLimit = maximumLimit - 1.0e-6f;
    }
    ((FadeFluidWork*)lbl_80467050)->cellSize = cellSize;
    ((FadeFluidWork*)lbl_80467050)->limit = waveLimit;
    ((FadeFluidWork*)lbl_80467050)->timeStep = timeStep;
    fadeFluidCalcParms(calcStep);

    velocity = 2.0f * cellSize;
    texStepX = 1.0f / (f32)((FadeFluidWork*)lbl_80467050)->columns;
    texStepY = 1.0f / (f32)fluid->rows;
    index = 0;
    for (y = 0; y <= rows; y++) {
        for (x = 0; x <= columns; x++) {
            set__5GSvecFfff(&((FadeFluidWork*)lbl_80467050)->heightPage[0][index],
                            cellSize * (f32)x, cellSize * (f32)y, 0.0f);
            GSvecCopy(&((FadeFluidWork*)lbl_80467050)->heightPage[1][index],
                      &((FadeFluidWork*)lbl_80467050)->heightPage[0][index]);
            set__5GSvecFfff(&((FadeFluidWork*)lbl_80467050)->velocityX[index],
                            0.0f, 0.0f, velocity);
            set__5GSvecFfff(&((FadeFluidWork*)lbl_80467050)->velocityY[index],
                            velocity, 0.0f, 0.0f);
            ((FadeFluidWork*)lbl_80467050)->texCoord[index].x = texStepX * (f32)x;
            ((FadeFluidWork*)lbl_80467050)->texCoord[index].y = texStepY * (f32)y;
            index++;
        }
    }
}

void fn_801C75EC(void* ptr) {
    u32 handle = GSmemGetHandleByPtr(ptr);
    u32 masked = handle & 0xFFFF;

    if (masked != 0) {
        fn_800E24B0(handle);
        GSmemFree(handle);
    }
}

void* fn_801C7630(u32 size) {
    u32 handle = GSmemAlloc(size, 0x20);
    u32 masked = handle & 0xFFFF;

    if (masked != 0) {
        return GSmemLock(handle);
    }
    return NULL;
}
#endif /* !FADE_EFFECT_TU */
