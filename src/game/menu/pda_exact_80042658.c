/* RULE-EXCEPTION(user-approved): constant_import (temporary) - remove when this file is merged back into one unit - see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_exact_80042658.c
 * @brief PDA model-page per-frame update, 0x80042658 - 0x80043728.
 *
 * fn_80042658 is copied from pda_range_80037158.c. Its float literals are
 * redirected by the temporary constant_import build step to the shared
 * lbl_8047BCxx pool.
 */
#include "dolphin/types.h"

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0x244];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */

extern PdaSceneWork lbl_803A6818;
extern u8 lbl_803A67FC[];
extern u8 lbl_804788C4;
extern s32 lbl_804788C0;
extern u32 lbl_8047A4E0;
extern u16* lbl_8047A4E4;
extern u8 lbl_802E543C[];
extern u8 lbl_802E5448[];
extern u8 lbl_80267180[];
extern u32 soundStartRaw(u32, u32, u32);
extern void menuCloseCustom(s32 slot, s32 arg1, s32 arg2);
extern void* wazaSequenceOwnerGetModel(void* h);
extern void GSmodelGetPosition(void* model, void* out);
extern void GSmodelSetPosition(void* model, void* pos);
extern u32 GSinputGetButtons(s32 chan);
extern void GSscene_SetMode(s32 mode);
extern void* memcpy(void* dst, const void* src, u32 size);
extern u32 gamedataGetStatus(s32 a, s32 b);
extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern u32 memoDataGetPokemonRndFromID(s32 a, u32 id);
extern u32 memoDataGetPokemonTrainerRndFromID(s32 a, u32 id);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetCatchTrainerRnd(u32 work, u32 rnd);
extern u8 pdaModelOrbitCamera(u8* work);
extern u32 pdaMemoListKey(u32 unused, s32 mode, u16 buttons);

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

/* Whether the highlighted memo entry has actually been seen. */
static inline u8 pdaEntrySeen(void)
{
    return (lbl_8047A4E4[lbl_803A6818.currentIndex] & 0x8000) ? 0 : 1;
}

typedef struct PdaVec3 {
    f32 x;
    f32 y;
    f32 z;
} PdaVec3;

extern void GSvecCopy(void* dst, void* src);
extern void* GSmodelGetBound(void* model);
extern void ObjInfoInit(void* bound, void* out);
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
extern u8 pdaModelFrameSingle(u8* work);
extern u8 pdaModelFrameCamera(void* work, void* sub);


/* Bring the PDA model/camera/light rig online for the summary screen. */
static inline u8 fn_8003D1FC_setup(u8* sub)
{
    void* model;
    void* cam;
    f32 persp0;
    f32 persp1;
    f32 persp2;
    f32 persp3;
    PdaVec3 color;
    PdaVec3 lightPos;
    PdaVec3 lightTarget;
    PdaVec3 pos;
    PdaVec3 bound;
    f32 dist;

    if (sub == NULL) {
        return 0;
    }
    model = *(void**)((u8*)&lbl_803A6818 + 0xe8);
    if (model == NULL) {
        *sub = 0;
        return 0;
    }
    GSvecCopy(&pos, (u8*)&lbl_803A6818 + 0x1e8);
    GSvecCopy(lbl_802E5448, (u8*)&lbl_803A6818 + 0x1f4);
    persp0 = *(f32*)((u8*)&lbl_803A6818 + 0x200);
    persp1 = *(f32*)((u8*)&lbl_803A6818 + 0x204);
    persp2 = *(f32*)((u8*)&lbl_803A6818 + 0x208);
    persp3 = *(f32*)((u8*)&lbl_803A6818 + 0x20c);
    dist = *(f32*)((u8*)&lbl_803A6818 + 0x210);
    ObjInfoInit(GSmodelGetBound(model), &bound);
    GSscene_SetMode(3);
    GScameraGetPerspective(*(void**)((u8*)&lbl_803A6818 + 0xfc), &persp0, &persp1, &persp2,
                           &persp3);
    set__5GSvecFfff(&pos, 0.0f, 0.0f, dist);
    GScameraSetPosition(*(void**)((u8*)&lbl_803A6818 + 0xfc), &pos);
    GScameraSetPerspective(*(void**)((u8*)&lbl_803A6818 + 0xfc), persp0, persp1, persp2,
                           persp3);
    GScameraLookAt(*(void**)((u8*)&lbl_803A6818 + 0xfc), lbl_802E543C, lbl_802E5448);
    color = *(PdaVec3*)lbl_80267180;
    memcpy(&lightPos, &pos, 12);
    lightPos.x = lightPos.x - 20.0f;
    lightPos.y += 40.0f;
    GSlightSetType(*(void**)((u8*)&lbl_803A6818 + 0x108), 2);
    GSlightSetColor(*(void**)((u8*)&lbl_803A6818 + 0x108), &color);
    GSlightSetPosition(*(void**)((u8*)&lbl_803A6818 + 0x108), &lightPos);
    GSlightSetTarget(*(void**)((u8*)&lbl_803A6818 + 0x108), &lightTarget);
    GSlightSetActive(*(void**)((u8*)&lbl_803A6818 + 0x108), 1);
    cam = *(void**)((u8*)&lbl_803A6818 + 0x114);
    if (cam != NULL) {
        GScameraSetPosition(cam, (u8*)&lbl_803A6818 + 0x118);
        GScameraSetRotation(cam, (u8*)&lbl_803A6818 + 0x124);
        GScameraSetPerspective(cam, *(f32*)((u8*)&lbl_803A6818 + 0x13c), *(f32*)((u8*)&lbl_803A6818 + 0x140),
                               *(f32*)((u8*)&lbl_803A6818 + 0x144), *(f32*)((u8*)&lbl_803A6818 + 0x148));
    }
    GSscene_SetMode(3);
    GSscene_SetMode(4);
    return 1;
}

/* Ease a value toward its target, snapping once a step would overshoot. */
static inline void pdaEase(f32* cur, f32* dstp)
{
    f32 c;
    f32 dst;
    f32 delta;
    f32 rate;
    f32 moved;
    f32 next;
    s8 mode;

    c = *cur;
    dst = *dstp;
    if (c == dst) {
        return;
    }
    delta = dst - c;
    rate = 10.0f * *(f32*)((u8*)&lbl_803A6818 + 0x3c);
    if ((delta > 0.0f ? delta : -delta) >= 243.0f) {
        *cur = dst;
        return;
    }
    mode = *(s8*)((u8*)&lbl_803A6818 + 0x48);
    if (mode == 1) {
        rate = rate * (5.0f / (f32)(s8)lbl_804788C4);
    }
    if (mode == 2) {
        rate = rate * (5.0f / (f32)(s8)lbl_804788C4);
    }
    moved = delta * rate;
    next = c + moved;
    /* RULE-EXCEPTION(user-approved): scalar reuse - the dead rate holds the remaining distance (fn_80042658) - see docs/RULE_EXCEPTIONS.md */
    rate = dst - next;
    *cur = next;
    if ((rate > 0.0f ? rate : -rate) <= (moved > 0.0f ? moved : -moved)) {
        *cur = dst;
    }
}

extern s8 GSinputGetRightStickXData(s32 chan, s32 index);
extern s8 GSinputGetRightStickYData(s32 chan, s32 index);
extern void GSvecAdd(void* dst, void* a, void* b);
extern u8 menuPokemonModelIsLoaded(void* work, u32 pokemon);
extern void menuModelFree(void* work);
extern void menuPokemonModelRequest(void* work, u32 pokemon);
extern void GSmodelDestroyLinkedParticles(void* model);

/* Step the page down one level, tearing the current model's particles down. */
static inline void pdaPopLevel(u8* S)
{
    void* model;

    *(f32*)(S + 0x6c) = 0.0f;
    *(f32*)(S + 0x70) = 0.0f;
    *(f32*)(S + 0x74) = 0.0f;
    *(f32*)(S + 0x64) = 1.5f;
    switch (*(s32*)(S + 0x28)) {
    case 0:
        *(s32*)(S + 0x1c) = 0xd;
        *(s32*)(S + 0x24) = 2;
        *(f32*)(S + 0x60) = 0.0f;
        model = wazaSequenceOwnerGetModel(*(void**)(S + 0xa0));
        if (model != NULL) {
            GSmodelDestroyLinkedParticles(model);
        }
        break;
    case 1:
        *(s32*)(S + 0x1c) = 0xd;
        *(s32*)(S + 0x24) = 4;
        *(f32*)(S + 0x60) = 0.0f;
        model = wazaSequenceOwnerGetModel(*(void**)(S + 0xa0));
        if (model != NULL) {
            GSmodelDestroyLinkedParticles(model);
        }
        *(f32*)(S + 0x64) = 2.0f;
        break;
    case 2:
        *(s32*)(S + 0x1c) = 0xd;
        *(s32*)(S + 0x24) = 5;
        *(f32*)(S + 0x60) = 0.0f;
        model = wazaSequenceOwnerGetModel(*(void**)(S + 0xa0));
        if (model != NULL) {
            GSmodelDestroyLinkedParticles(model);
        }
        break;
    default:
        break;
    }
}

/* Close the windows belonging to the level we are leaving. */
static inline void pdaCloseLevelWindows(u8* S)
{
    switch (*(s32*)(S + 0x28)) {
    case 0:
        menuCloseCustom(0x3c, 0, 0);
        menuCloseCustom(0x96, 0, 0);
        break;
    case 1:
        menuCloseCustom(0xa1, 0, 0);
        menuCloseCustom(0x96, 0, 0);
        break;
    case 2:
        menuCloseCustom(0x96, 0, 0);
        break;
    default:
        break;
    }
}

#define G(off) (*(f32*)((u8*)&lbl_803A6818 + (off)))

static inline f32 pdaAbsF(f32 x)
{
    return x > 0.0f ? x : -x;
}

/* RULE-EXCEPTION(user-approved): reconstructed inline helpers pdaAbsF/pdaApproach (fn_80040018, pdaMemoUpdateScroll, fn_80044630) - see docs/RULE_EXCEPTIONS.md */
static inline void pdaApproach(f32* current, f32* target, f32 delta)
{
    f32 step;
    f32 distance;
    f32 scaled;

    step = *target - *current;
    scaled = 10.0f * step;
    step = scaled * delta;
    if (step > 20.0f) {
        step = 20.0f;
    }
    if (step <= -20.0f) {
        step = -20.0f;
    }
    distance = *target - (*current += step);
    if (pdaAbsF(distance) <= pdaAbsF(step) || pdaAbsF(distance) < 1.0f) {
        *current = *target;
    }
}

/* Model page: stick-driven rotation and trigger zoom of the summary model. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_80042658_Spin (fn_80042658) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_80042658_Spin(void)
{
    u8* modelWork = (u8*)&lbl_803A6818 + 0x7c;
    PdaVec3 spin;
    s8 stickY;
    s8 stickX;
    PdaVec3 pos;
    void* model;
    f32* rot;

    ((u8*)&lbl_803A6818)[0x214] = pdaModelOrbitCamera(modelWork);
    spin.x = 0.0f;
    spin.y = 0.0f;
    spin.z = 0.0f;
    model = wazaSequenceOwnerGetModel(*(void**)(modelWork + 0x24));
    if (model == NULL) {
        return;
    }
    GSmodelGetBound(model);
    stickY = GSinputGetRightStickXData(1, 1);
    stickX = GSinputGetRightStickYData(1, 1);
    spin.x = *(f32*)((u8*)&lbl_803A6818 + 0x3c) *
             (3.1415927f * (f32)stickX / 128.0f);
    spin.y = *(f32*)((u8*)&lbl_803A6818 + 0x3c) *
             (3.1415927f * (f32)stickY / 128.0f);
    rot = (f32*)((u8*)&lbl_803A6818 + 0x6c);
    GSvecAdd(rot, rot, &spin);
    if (*rot >= 1.5707964f) {
        *rot = 1.5707964f - spin.x;
    }
    if (*rot <= -1.5707964f) {
        *rot = -1.5707964f - spin.x;
    }
    GSmodelGetPosition(model, &pos);
    pos.x = 0.0f;
    GSmodelSetPosition(model, &pos);
    if ((GSinputGetButtons(1) & 0x40) != 0) {
        if (*(f32*)((u8*)&lbl_803A6818 + 0x64) <= 3.0f) {
            *(f32*)((u8*)&lbl_803A6818 + 0x64) = *(f32*)((u8*)&lbl_803A6818 + 0x64) + *(f32*)((u8*)&lbl_803A6818 + 0x3c);
        }
    } else if ((GSinputGetButtons(1) & 0x20) != 0) {
        if (*(f32*)((u8*)&lbl_803A6818 + 0x64) > 1.0f) {
            *(f32*)((u8*)&lbl_803A6818 + 0x64) = *(f32*)((u8*)&lbl_803A6818 + 0x64) - *(f32*)((u8*)&lbl_803A6818 + 0x3c);
        }
    }
}

/* Per-frame scene update for the PDA's model page. */
s32 pdaModelViewerControl(void* work, s32 mode)
{
    extern u32 GSgetFrameStep(void);
    extern s32 GSgfxGetVsyncRate(void);
    u8* K = lbl_803A67FC;
    u16 keys;
    f32 dt;
    f32 target;
    f32 current;
    f32 step;
    f32 next;
    u32 pokemon;
    s32 held;
    s32 prevIndex;

    held = 0;
    prevIndex = *(s32*)&lbl_803A6818;
    if (*(s8*)((u8*)&lbl_803A6818 + 0x48) == 1) {
        lbl_804788C4 = 3;
        /* RULE-EXCEPTION(user-approved): codegen-only cast - (u16) on the u16 key read gives retail's lhz r0 + mr copy (fn_80042658) - see docs/RULE_EXCEPTIONS.md */
        keys = (u16)*(u16*)(K + 6);
    } else if (*(s8*)((u8*)&lbl_803A6818 + 0x48) == 2) {
        lbl_804788C4 = 2;
        /* RULE-EXCEPTION(user-approved): codegen-only cast - (u16) on the u16 key read gives retail's lhz r0 + mr copy (fn_80042658) - see docs/RULE_EXCEPTIONS.md */
        keys = (u16)*(u16*)(K + 6);
    } else {
        lbl_804788C4 = 5;
        /* RULE-EXCEPTION(user-approved): codegen-only cast - (u16) on the u16 key read gives retail's lhz r0 + mr copy (fn_80042658) - see docs/RULE_EXCEPTIONS.md */
        keys = (u16)*(u16*)(K + 6);
    }
    if ((*(u16*)K & 3) != 0) {
        held = 1;
    }
    dt = (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
    *(f32*)((u8*)&lbl_803A6818 + 0x3c) = dt;
    *(f32*)((u8*)&lbl_803A6818 + 0x40) = *(f32*)((u8*)&lbl_803A6818 + 0x40) + dt;
    if (*(f32*)((u8*)&lbl_803A6818 + 0x40) >= 1.0f) {
        *(f32*)((u8*)&lbl_803A6818 + 0x40) = 0.0f;
    }
    pdaEase((f32*)((u8*)&lbl_803A6818 + 0x2c),
            (f32*)((u8*)&lbl_803A6818 + 0x34));
    pdaEase((f32*)((u8*)&lbl_803A6818 + 0x30),
            (f32*)((u8*)&lbl_803A6818 + 0x38));
    target = G(0x50);
    current = G(0x4C);
    if (target != current) {
        step = G(0x3C) / 0.25f;
        if (target > current) {
            next = current + step;
            G(0x4C) = next;
            if (next > 1.0f) {
                G(0x4C) = 1.0f;
            }
        } else {
            next = current - step;
            G(0x4C) = next;
            if (next < 0.0f) {
                G(0x4C) = 0.0f;
            }
        }
    }

    target = G(0x58);
    current = G(0x54);
    if (target != current) {
        step = G(0x3C) / 0.25f;
        if (target > current) {
            next = current + step;
            G(0x54) = next;
            if (next > 1.0f) {
                G(0x54) = 1.0f;
            }
        } else {
            next = current - step;
            G(0x54) = next;
            if (next < 0.0f) {
                G(0x54) = 0.0f;
            }
        }
    }

    target = G(0x60);
    current = G(0x5C);
    if (target != current) {
        step = G(0x3C) / 0.8f;
        if (target > current) {
            next = current + step;
            G(0x5C) = next;
            if (next > 1.0f) {
                G(0x5C) = 1.0f;
            }
        } else {
            next = current - step;
            G(0x5C) = next;
            if (next < 0.0f) {
                G(0x5C) = 0.0f;
            }
        }
    }

    if (G(0x1DC) != G(0x1E0)) {
        pdaApproach(&G(0x1DC), &G(0x1E0), G(0x3C));
    }

    switch (*(s32*)((u8*)&lbl_803A6818 + 0x1c)) {
    case 1:
    case 2:
    case 3:
        ((u8*)&lbl_803A6818)[0x214] = pdaModelFrameSingle((u8*)&lbl_803A6818 + 0x7c);
        pdaMemoListKey((u32)work, mode, keys);
        break;
    case 4:
        pda_80042658_Spin();
        break;
    case 5:
        ((u8*)&lbl_803A6818)[0x214] = pdaModelFrameCamera((u8*)&lbl_803A6818 + 0x7c, (u8*)&lbl_803A6818 + 0xc4);
        ((u8*)&lbl_803A6818)[0x214] = fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
        break;
    default:
        break;
    }

    if (held != 0) {
        if (*(s8*)((u8*)&lbl_803A6818 + 0x48) < 2) {
            *(f32*)((u8*)&lbl_803A6818 + 0x44) = *(f32*)((u8*)&lbl_803A6818 + 0x44) + *(f32*)((u8*)&lbl_803A6818 + 0x3c);
            if (*(f32*)((u8*)&lbl_803A6818 + 0x44) >= 1.0f) {
                *(s8*)((u8*)&lbl_803A6818 + 0x48) = 1;
            }
            if (*(f32*)((u8*)&lbl_803A6818 + 0x44) >= 2.0f) {
                *(s8*)((u8*)&lbl_803A6818 + 0x48) = 2;
            }
        }
    } else {
        *(f32*)((u8*)&lbl_803A6818 + 0x44) = 0.0f;
        *(s8*)((u8*)&lbl_803A6818 + 0x48) = 0;
    }

    if (mode != 0) {
        if (*(f32*)((u8*)&lbl_803A6818 + 0x58) ==
            *(f32*)((u8*)&lbl_803A6818 + 0x54)) {
            if ((*(u16*)(K + 6) & 0x4) != 0) {
                if (*(s32*)((u8*)&lbl_803A6818 + 0x28) > 0) {
                    pdaCloseLevelWindows((u8*)&lbl_803A6818);
                    *(s32*)((u8*)&lbl_803A6818 + 0x28) =
                        *(s32*)((u8*)&lbl_803A6818 + 0x28) - 1;
                    soundStartRaw(0x23, 0, 0);
                    pdaPopLevel((u8*)&lbl_803A6818);
                }
            }
            if ((*(u16*)(K + 6) & 0x8) != 0) {
                if (*(s32*)((u8*)&lbl_803A6818 + 0x28) < 2 &&
                    pdaEntrySeen() != 0 &&
                    *(s32*)((u8*)&lbl_803A6818 + 0x20) != 1) {
                    pdaCloseLevelWindows((u8*)&lbl_803A6818);
                    *(s32*)((u8*)&lbl_803A6818 + 0x28) =
                        *(s32*)((u8*)&lbl_803A6818 + 0x28) + 1;
                    soundStartRaw(0x23, 0, 0);
                    pdaPopLevel((u8*)&lbl_803A6818);
                }
            }
            *(f32*)((u8*)&lbl_803A6818 + 0x30) =
                *(f32*)((u8*)&lbl_803A6818 + 0x38);
        }
    }

    if (prevIndex != lbl_803A6818.currentIndex && lbl_804788C0 != 0) {
        pokemon = pdaLoadPokemon(lbl_803A6818.currentIndex);
        if (menuPokemonModelIsLoaded((u8*)&lbl_803A6818 + 0x7c, pokemon) == 0) {
            menuModelFree((u8*)&lbl_803A6818 + 0x7c);
            menuPokemonModelRequest((u8*)&lbl_803A6818 + 0x7c, pokemon);
        }
        *((u8*)&lbl_803A6818 + 0x214) = 0;
        switch (*(s32*)((u8*)&lbl_803A6818 + 0x1c)) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6:
        case 7:
        case 8:
        case 0xb:
        case 0xc:
            *((u8*)&lbl_803A6818 + 0x214) =
                pdaModelFrameSingle((u8*)&lbl_803A6818 + 0x7c);
            break;
        case 5:
            *((u8*)&lbl_803A6818 + 0x214) = pdaModelFrameCamera(
                (u8*)&lbl_803A6818 + 0x7c, (u8*)&lbl_803A6818 + 0xc4);
            *((u8*)&lbl_803A6818 + 0x214) =
                fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
            break;
        default:
            break;
        }
    }
    return 0;
}
