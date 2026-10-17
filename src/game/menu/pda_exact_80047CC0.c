/* RULE-EXCEPTION(user-approved): constant_import (temporary) — remove when this file is merged back into one unit — see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_exact_80047CC0.c
 * @brief PDA single-model camera setup, 0x80047CC0 - 0x800484A4.
 *
 * The body is exact. Its compiler-owned signed int-to-float bias is redirected
 * by the temporary constant_import build step to the named lbl_8047BCB0
 * definition in its canonical data object (sdata2_8047BCA0.c).
 */
#include "dolphin/types.h"

typedef struct PdaVec3 {
    f32 x;
    f32 y;
    f32 z;
} PdaVec3;

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0xC];
    s32 field_10;
    u8 pad14[0x14];
    s32 field_28;
    u8 pad2C[0x14];
    f32 angle;
    u8 pad44[8];
    f32 alphaScale;
} PdaSceneWork;

extern PdaSceneWork lbl_803A6818;
extern u32 lbl_8047A4E0;
extern u16* lbl_8047A4E4;

extern f32 lbl_80478AC0[];
extern f32 lbl_8047BC94;
extern f32 lbl_8047BC98;
extern f32 lbl_8047BC9C;
extern f32 lbl_8047BCBC;
extern f32 lbl_8047BCC0;
extern f32 lbl_8047BD18;
extern f32 lbl_8047BD30;
extern f32 lbl_8047BD38;
extern f32 lbl_8047BD3C;
extern f32 lbl_8047BD40;
extern f32 lbl_8047BD44;
extern f32 lbl_8047BD48;
extern f32 lbl_8047BD4C;
extern const f64 lbl_8047BD50;
extern const f64 lbl_8047BD58;
extern f64 lbl_8047BD60;
extern volatile f32 lbl_8047BD68;

extern u8 lbl_80267180[];
extern u8 lbl_802E540C[];
extern u8 lbl_802E5418[];

extern f64 __frsqrte(f64 value);
extern f64 tan(f64 x);
extern f64 atan(f64 x);
extern f64 sin(f64 x);
extern void* memcpy(void* dst, const void* src, u32 size);

extern u32 gamedataGetStatus(s32 a, s32 b);
extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern u32 memoDataGetPokemonRndFromID(s32 a, u32 id);
extern u32 memoDataGetPokemonTrainerRndFromID(s32 a, u32 id);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetCatchTrainerRnd(u32 work, u32 rnd);
extern u32 pokemonBiosGetPokemonDataId(u32 work);
extern void memoGetScaleAngle(u32 id, f32* scale, f32* angle);

extern u8 menuModelCheck(void* work, s32 index);
extern void* wazaSequenceOwnerGetModel(void* h);
extern s32 wazaSequenceOwnerGetSizeClass(void* h);
extern void GSscene_SetMode(s32 mode);
extern void* GSmodelGetBound(void* model);
extern void ObjInfoInit(void* bound, void* out);
extern void GSmodelGetPosition(void* model, void* out);
extern void GSmodelSetMatrix(void* model, void* mtx);
extern void GSmodelCenterNull(void* model);
extern void modelRemoveCenterNull(void* model);
extern s32 GSmodelWalkJObjs(void* model);
extern void* GSmodelGetPart(void* model, s32 index);
extern void GSpartGetTransform(void* part, void* out, s32 a, s32 b);
extern void GSpartFree(void* part);
extern void fn_800E064C(f32* mtx);
extern void fn_800E03B4(f32* mtx, void* vec);
extern void GSmtxRotateY(f32* mtx, f32 angle);

extern void GScameraGetPerspective(void* cam, f32* a, f32* b, f32* c, f32* d);
extern void GScameraSetPerspective(void* cam, f32 a, f32 b, f32 c, f32 d);
extern void GScameraSetPosition(void* cam, void* pos);
extern void GScameraSetRotation(void* cam, void* rot);
extern void GScameraLookAt(void* cam, void* a, void* b);
extern void set__5GSvecFfff(void* vec, f32 x, f32 y, f32 z);
extern void GSlightSetType(void* light, s32 type);
extern void GSlightSetColor(void* light, void* color);
extern void GSlightSetPosition(void* light, void* pos);
extern void GSlightSetTarget(void* light, void* target);
extern void GSlightSetActive(void* light, s32 active);

static inline u32 pdaLoadPokemon(s32 index)
{
    u32 work = lbl_8047A4E0;
    u32 rnd;
    u32 species;
    u32 trainerRnd;

    if (work != 0) {
        species = lbl_8047A4E4[index];
        if (species >= 0x8000) {
            species = species & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = memoDataGetPokemonRndFromID(0, species);
        trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

static inline s32 pdaFpClassifyF(f32 value)
{
    switch (*(s32*)&value & 0x7F800000) {
    case 0x7F800000:
        if (*(s32*)&value & 0x007FFFFF) {
            return 1;
        }
        return 2;
    case 0:
        if (*(s32*)&value & 0x007FFFFF) {
            return 5;
        }
        return 3;
    }
    return 4;
}

static inline f32 pdaSqrtf(f32 x)
{
    if (x > lbl_8047BC94) {
        f64 xd = x;
        f64 guess = __frsqrte(xd);
        f64 half = lbl_8047BD50;
        f64 three = lbl_8047BD58;
        guess = half * guess * (three - guess * guess * xd);
        guess = half * guess * (three - guess * guess * xd);
        guess = half * guess * (three - guess * guess * xd);
        return (f32)(xd * guess);
    } else if (x < lbl_8047BD60) {
        return lbl_80478AC0[0];
    } else if (pdaFpClassifyF(x) == 1) {
        return lbl_80478AC0[0];
    }
    return x;
}

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
u8 fn_80047CC0(u8* work)
{
    f32 mtx[12];
    PdaVec3 bound;
    PdaVec3 target;
    PdaVec3 camPos;
    PdaVec3 xform;
    PdaVec3 modelPos;
    PdaVec3 lightPos;
    PdaVec3 color;
    f32 persp0;
    f32 persp1;
    f32 persp2;
    f32 persp3;
    f32 angle;
    f32 scale;
    void* model;
    void* part;
    f32 spread;
    f32 root;
    register f32 radians;
    register f32 halfAngle;
    register f32 fov;
    register f32 scaledFov;
    f32 zoom;
    f32 pitch;

    zoom = lbl_8047BCC0;
    angle = lbl_8047BD38;
    scale = lbl_8047BCBC;
    memoGetScaleAngle(
        pokemonBiosGetPokemonDataId(pdaLoadPokemon(lbl_803A6818.currentIndex)),
        &scale, &angle);
    zoom = zoom * scale;
    if (work == NULL) {
        return 0;
    }
    if (*(void**)(work + 0x34) == NULL) {
        return 0;
    }
    if (menuModelCheck(work, 0) == 1) {
        return 0;
    }
    if (work[0x14] != 0) {
        model = wazaSequenceOwnerGetModel(*(void**)(work + 0x24));
        if (model == NULL) {
            return 0;
        }
        switch (wazaSequenceOwnerGetSizeClass(*(void**)(work + 0x24))) {
        case -2:
            spread = lbl_8047BD3C;
            break;
        case -1:
            spread = lbl_8047BD40;
            break;
        case 0:
            spread = lbl_8047BD44;
            break;
        case 1:
            spread = lbl_8047BD44;
            break;
        case 2:
            spread = lbl_8047BD48;
            break;
        case 3:
            spread = lbl_8047BD4C;
            break;
        }
    } else {
        model = *(void**)(work + 0x24);
    }
    GSscene_SetMode(3);
    GScameraGetPerspective(*(void**)(work + 0x38), &persp0, &persp1, &persp2,
                           &persp3);
    ObjInfoInit(GSmodelGetBound(model), &bound);
    {
        f32 temp_persp1;
        temp_persp1 = (f32)*(s32*)(work + 0x2c) / (f32)*(s32*)(work + 0x30);
        persp0 = lbl_8047BD30;
        persp1 = temp_persp1;
    }
    GSmodelGetPosition(model, &modelPos);
    root = pdaSqrtf(bound.y * bound.y + bound.x * bound.x);
    radians = lbl_8047BD68;
    halfAngle = lbl_8047BD18;
    fov = persp0;
    scaledFov = radians * fov;
    spread = zoom * (root / spread) /
             (f32)tan(scaledFov * halfAngle);
    spread = spread * *(f32*)((u8*)&lbl_803A6818 + 0x68);
    GScameraSetPerspective(*(void**)(work + 0x38), persp0, persp1, persp2,
                           persp3);
    {
        f32* v = (f32*)((u8*)&lbl_803A6818 + 0x218);
        pitch = (f32)atan(
            pdaSqrtf((v[4] - v[1]) * (v[4] - v[1])) /
            pdaSqrtf((v[3] - v[0]) * (v[3] - v[0]) +
                     (v[5] - v[2]) * (v[5] - v[2])));
    }
    GSmodelGetPosition(model, &modelPos);
    target.x = lbl_8047BC94;
    target.z = lbl_8047BC94;
    target.y = lbl_8047BC94;
    fn_800E064C(mtx);
    fn_800E03B4(mtx, &target);
    GSmtxRotateY(mtx, angle);
    GSmodelSetMatrix(model, mtx);
    GSmodelCenterNull(model);
    part = GSmodelGetPart(model, GSmodelWalkJObjs(model) - 1);
    if (part != NULL) {
        GSpartGetTransform(part, &xform, 0, 0);
        GSpartFree(part);
    }
    modelRemoveCenterNull(model);
    {
        f32 sin_val = (f32)sin(pitch);
        f32 term = spread * sin_val;
        set__5GSvecFfff(&camPos, lbl_8047BC94,
                        term + (lbl_8047BC94 + xform.y), spread);
    }
    GScameraSetPosition(*(void**)(work + 0x38), &camPos);
    *(f32*)(lbl_802E5418 + 0) = lbl_8047BC94;
    *(f32*)(lbl_802E5418 + 4) = xform.y;
    *(f32*)(lbl_802E5418 + 8) = xform.z;
    GScameraLookAt(*(void**)(work + 0x38), lbl_802E540C, lbl_802E5418);
    color = *(PdaVec3*)lbl_80267180;
    memcpy(&lightPos, &camPos, 12);
    lightPos.x = lightPos.x - lbl_8047BC98;
    lightPos.y = lightPos.y + lbl_8047BC9C;
    GSlightSetType(*(void**)(work + 0x44), 2);
    GSlightSetColor(*(void**)(work + 0x44), &color);
    GSlightSetPosition(*(void**)(work + 0x44), &lightPos);
    GSlightSetTarget(*(void**)(work + 0x44), &target);
    GSlightSetActive(*(void**)(work + 0x44), 1);
    work = *(u8**)((u8*)&lbl_803A6818 + 0x114);
    if (work != NULL) {
        GScameraSetPosition(work, (u8*)&lbl_803A6818 + 0x118);
        GScameraSetRotation(work, (u8*)&lbl_803A6818 + 0x124);
        GScameraSetPerspective(work, *(f32*)((u8*)&lbl_803A6818 + 0x13c),
                               *(f32*)((u8*)&lbl_803A6818 + 0x140),
                               *(f32*)((u8*)&lbl_803A6818 + 0x144),
                               *(f32*)((u8*)&lbl_803A6818 + 0x148));
    }
    GSscene_SetMode(3);
    GSscene_SetMode(4);
    return 1;
}
#pragma peephole reset
