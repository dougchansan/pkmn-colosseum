/* RULE-EXCEPTION(user-approved): constant_import (temporary) - remove when this file is merged back into one unit - see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_exact_80040308.c
 * @brief PDA filter/sort menu confirm and cancel handling, 0x80040308 - 0x80041114.
 *
 * pdaMemoMainKey is copied from pda_range_80037158.c. Its float literals are
 * redirected by the temporary constant_import build step to the shared
 * lbl_8047BCxx pool. Built with GC/2.0 -O3 and a unit-level -opt nopeephole.
 * fn_8003D818 is an external call here, as in retail.
 */
#include "dolphin/types.h"

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0x244];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */

extern PdaSceneWork lbl_803A6818;
extern u8 lbl_804788C4;
extern s32 lbl_804788C0;
extern u32 lbl_8047A4E0;
extern u16* lbl_8047A4E4;
extern u16 lbl_8047A4E8;
extern u8 lbl_804788D4[4];
extern u32 soundStartRaw(u32, u32, u32);
extern void* wazaSequenceOwnerGetModel(void* h);
extern void GSscene_SetMode(s32 mode);
extern void* memcpy(void* dst, const void* src, u32 size);
extern u32 gamedataGetStatus(s32 a, s32 b);
extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern u32 memoDataGetPokemonRndFromID(s32 a, u32 id);
extern u32 memoDataGetPokemonTrainerRndFromID(s32 a, u32 id);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetCatchTrainerRnd(u32 work, u32 rnd);
extern u8 menuPokemonModelIsLoaded(void* work, u32 pokemon);
extern void menuModelFree(void* work);
extern void menuPokemonModelRequest(void* work, u32 pokemon);
extern void* menuDataBiosGetPtr(s32 id);
extern void* windowGetKeyInfo(void* window);
extern void menuPlaySe(s32 id, s32 se);
extern u16 pdaMemoLoadPokemon(void);
extern u16 pdaMemoBuildList(void);


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
extern u8 lbl_802E543C[];
extern u8 lbl_802E5448[];
extern u8 lbl_80267180[];
extern u8 pdaModelFrameSingle(u8* work);
extern u8 pdaModelFrameCamera(void* work, void* sub);


/* Point the PDA's light at the camera position, offset up and to the left. */
/* RULE-EXCEPTION(user-approved): reconstructed single-use inline helper pda_80040308_SetupLight (stack layout of pdaMemoMainKey) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_80040308_SetupLight(PdaVec3* pos, PdaVec3* target)
{
    PdaVec3 color;
    PdaVec3 lightPos;

    color = *(PdaVec3*)lbl_80267180;
    memcpy(&lightPos, pos, 12);
    lightPos.x = lightPos.x - 20.0f;
    lightPos.y += 40.0f;
    GSlightSetType(*(void**)((u8*)&lbl_803A6818 + 0x108), 2);
    GSlightSetColor(*(void**)((u8*)&lbl_803A6818 + 0x108), &color);
    GSlightSetPosition(*(void**)((u8*)&lbl_803A6818 + 0x108), &lightPos);
    GSlightSetTarget(*(void**)((u8*)&lbl_803A6818 + 0x108), target);
    GSlightSetActive(*(void**)((u8*)&lbl_803A6818 + 0x108), 1);
}

/* Bring the PDA model/camera/light rig online for the summary screen. */
static inline u8 fn_8003D1FC_setup(u8* sub)
{
    void* model;
    void* cam;
    f32 persp0;
    f32 persp1;
    f32 persp2;
    f32 persp3;
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
    pda_80040308_SetupLight(&pos, &lightTarget);
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

static inline s32 pdaViewSlot(u8* L)
{
    s32 slot = 0;

    switch (*(s8*)(L + 9)) {
    case 0:
        slot = 0;
        break;
    case 1:
        slot = 1;
        break;
    case 2:
        if (*(s8*)(L + 0xa) == 0) {
            slot = 2;
        } else {
            slot = 3;
        }
        break;
    case 3:
        slot = 4;
        break;
    default:
        break;
    }
    return slot;
}

static inline void pdaResetCarousel(u16 total)
{
    *(s32*)((u8*)&lbl_803A6818 + 0x0) = 0;
    *(f32*)((u8*)&lbl_803A6818 + 0x2c) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x30) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x34) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x38) = 0.0f;
    *(s32*)((u8*)&lbl_803A6818 + 0x8) = -5;
    if (total >= 5) {
        *(s32*)((u8*)&lbl_803A6818 + 0xc) = 5;
    } else {
        *(s32*)((u8*)&lbl_803A6818 + 0xc) = total;
    }
    *(s32*)((u8*)&lbl_803A6818 + 0x10) = total;
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


void pdaMemoMainKey(void* menu)
{
    u8* S = (u8*)&lbl_803A6818;
    u16 keys;
    u8* L = (u8*)&lbl_803A6818 + 0x154;
    u16 total;
    s8 page;
    s32 slot;
    u8* view;
    f32 target;
    f32 current;
    f32 step;
    f32 next;

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
    if (*(f32*)(L + 0x88) != *(f32*)(L + 0x8c)) {
        pdaApproach((f32*)(L + 0x88), (f32*)(L + 0x8c), G(0x3C));
    }
    if (*(f32*)(S + 0x50) != *(f32*)(S + 0x4c)) {
        return;
    }
    if (menu == NULL) {
        return;
    }
    keys = *(u16*)((u8*)windowGetKeyInfo(
                       menuDataBiosGetPtr(*(s32*)((u8*)menu + 4))) +
                   4);
    if ((keys & 0x10) != 0) {
        soundStartRaw(0x24, 0, 0);
        page = *(s8*)(L + 9);
        if (page == 4) {
            switch (*(s32*)(L + 0x90)) {
            case 0:
                if (pdaMemoLoadPokemon() != 0) {
                    total = lbl_8047A4E8;
                    *(s32*)(L + 0x90) = 1;
                    pdaResetCarousel(total);
                } else {
                    *(s32*)(L + 0x90) = 2;
                }
                break;
            case 1:
                *(s8*)((u8*)menu + 0x98) = 1;
                *(s8*)((u8*)menu + 0x99) = 1;
                if (lbl_804788C0 != 0) {
                    {
                        u32 pokemon =
                            pdaLoadPokemon(lbl_803A6818.currentIndex);
                        if (menuPokemonModelIsLoaded((u8*)&lbl_803A6818 + 0x7c, pokemon) ==
                            0) {
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
                            *((u8*)&lbl_803A6818 + 0x214) =
                                pdaModelFrameCamera((u8*)&lbl_803A6818 + 0x7c,
                                            (u8*)&lbl_803A6818 + 0xc4);
                            *((u8*)&lbl_803A6818 + 0x214) =
                                fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
                            break;
                        default:
                            break;
                        }
                    }
                }
                break;
            case 2:
                *(s32*)(L + 0x90) = 0;
                break;
            default:
                break;
            }
        } else {
            switch (*(s32*)L) {
            case 0:
                if (300.0f == *(f32*)(L + 0x88)) {
                    *(s32*)L = 1;
                    *(f32*)(L + 0x8c) = 0.0f;
                    *(s8*)(L + 0xc) = lbl_804788D4[page];
                    switch (page) {
                    case 0:
                        L[0xb] = L[4];
                        break;
                    case 1:
                        L[0xb] = L[5];
                        break;
                    case 2:
                        L[0xb] = (L + *(s8*)(L + 0xa))[6];
                        break;
                    case 3:
                        L[0xb] = L[8];
                        break;
                    default:
                        break;
                    }
                    if (*(s8*)(L + 0xc) >= 10) {
                        if ((s8)(L[0xb] - 9) < 0) {
                            L[0x74] = 0;
                        }
                        L[0x75] = 10;
                        *(s8*)(L + 0x75) = *(s8*)(L + 0x74) + 10;
                        *(f32*)(L + 0x84) = *(f32*)(L + 0x7c) = (f32)(*(s8*)(L + 0x74) * -0x1d);
                    } else {
                        L[0x74] = 0;
                        L[0x75] = L[0xc];
                        *(f32*)(L + 0x7c) = 0.0f;
                        *(f32*)(L + 0x84) = 0.0f;
                    }
                    slot = pdaViewSlot(L);
                    view = L + slot * 0x14 + 0x10;
                    if (*(s8*)view != 0) {
                        L[0x74] = view[1];
                        L[0x75] = view[2];
                        *(f32*)(L + 0x78) = *(f32*)(view + 4);
                        *(f32*)(L + 0x7c) = *(f32*)(view + 8);
                        *(f32*)(L + 0x80) = *(f32*)(view + 0xc);
                        *(f32*)(L + 0x84) = *(f32*)(view + 0x10);
                    }
                    menuPlaySe(*(s32*)((u8*)menu + 4), 2);
                }
                break;
            case 1:
                if (0.0f == *(f32*)(L + 0x88)) {
                    switch (page) {
                    case 0:
                        L[4] = L[0xb];
                        break;
                    case 1:
                        L[5] = L[0xb];
                        break;
                    case 2:
                        (L + *(s8*)(L + 0xa))[6] = L[0xb];
                        break;
                    case 3:
                        L[8] = L[0xb];
                        break;
                    default:
                        break;
                    }
                    slot = pdaViewSlot(L);
                    view = L + slot * 0x14 + 0x10;
                    view[0] = 1;
                    view[1] = L[0x74];
                    view[2] = L[0x75];
                    *(f32*)(view + 4) = *(f32*)(L + 0x78);
                    *(f32*)(view + 8) = *(f32*)(L + 0x7c);
                    *(f32*)(view + 0xc) = *(f32*)(L + 0x80);
                    *(f32*)(view + 0x10) = *(f32*)(L + 0x84);
                    *(s32*)L = 0;
                    *(f32*)(L + 0x8c) = 300.0f;
                    menuPlaySe(*(s32*)((u8*)menu + 4), 2);
                }
                break;
            default:
                break;
            }
        }
    }
    if ((keys & 0x20) != 0) {
        soundStartRaw(0x25, 0, 0);
        switch (*(s32*)L) {
        case 0:
            if (300.0f == *(f32*)(L + 0x88)) {
                if (*(s32*)(L + 0x90) != 1) {
                    pdaMemoBuildList();
                    total = lbl_8047A4E8;
                    pdaResetCarousel(total);
                    if (lbl_804788C0 != 0) {
                        {
                        u32 pokemon =
                            pdaLoadPokemon(lbl_803A6818.currentIndex);
                        if (menuPokemonModelIsLoaded((u8*)&lbl_803A6818 + 0x7c, pokemon) ==
                            0) {
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
                            *((u8*)&lbl_803A6818 + 0x214) =
                                pdaModelFrameCamera((u8*)&lbl_803A6818 + 0x7c,
                                            (u8*)&lbl_803A6818 + 0xc4);
                            *((u8*)&lbl_803A6818 + 0x214) =
                                fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
                            break;
                        default:
                            break;
                        }
                    }
                    }
                    *((u8*)&lbl_803A6818 + 0x214) =
                        pdaModelFrameSingle((u8*)&lbl_803A6818 + 0x7c);
                    *(s8*)((u8*)menu + 0x98) = 1;
                    *(s8*)((u8*)menu + 0x99) = 1;
                    menuPlaySe(*(s32*)((u8*)menu + 4), 3);
                }
            }
            break;
        case 1:
            if (0.0f == *(f32*)(L + 0x88)) {
                *(s32*)L = 0;
                *(f32*)(L + 0x8c) = 300.0f;
                menuPlaySe(*(s32*)((u8*)menu + 4), 2);
            }
            break;
        default:
            break;
        }
    }
}
