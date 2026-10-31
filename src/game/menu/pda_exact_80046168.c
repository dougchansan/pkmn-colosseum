/* RULE-EXCEPTION(user-approved): constant_import (temporary) - remove when this file is merged back into one unit — see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_exact_80046168.c
 * @brief PDA cry-playback thread, 0x80046168 - 0x800472CC.
 *
 * Standalone copy of pdaStrategyMemoUpdate from pda_range_80037158.c together with the
 * inline helpers the shared file inlines into it. Unit flag -opt nopeephole
 * replaces the shared file's local "peephole off".
 */
#include "dolphin/types.h"

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
    u8 pad50[0x1F8];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */

typedef struct PdaOrbitPoint {
    f32 angle;
    f32 pad04[6];
    f32 alpha;
    f32 pad20;
} PdaOrbitPoint;

typedef struct PdaCryTrail {
    void* data;
    f32 speed;
    u32 voice;
    f32 field_0c;
    s32 offset;
    f32 field_14;
    s32 active;
    f32 alpha;
    f32 field_20;
} PdaCryTrail;

extern PdaSceneWork lbl_803A6818;
extern PdaOrbitPoint lbl_802E52C8[8];
extern u32 lbl_8047A4E0;
extern u16* lbl_8047A4E4;
extern u32 lbl_8047A4EC;
extern f32 lbl_8047BC94;
extern f32 lbl_8047BCA0;
extern f32 lbl_8047BCBC;
extern f32 lbl_8047BCFC;
extern f32 lbl_8047BD00;
extern f32 lbl_8047BD18;
extern f32 lbl_8047BD1C;

extern u32 menuCloseCustom(u32 a, u32 b, u32 c);
extern s32 menuOpen(s32 id, s32 flag);
extern u32 soundStartRaw(u32, u32, u32);
extern s32 GSsndGetStatus(u32 voice);
extern u32 GSinputButtonsChanged(s32 chan);
extern u32 GSinputGetButtons(s32 chan);
extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetCatchTrainerRnd(u32 work, u32 rnd);
extern u32 gamedataGetStatus(s32 a, s32 b);
extern u32 memoDataGetPokemonRndFromID(s32 a, u32 id);
extern u32 memoDataGetPokemonTrainerRndFromID(s32 a, u32 id);
extern u16 pokemonBiosGetPokemonDataId(u32 work);
extern void* pokemonDataBiosGetPtr(u16 id);
extern u16 pokemonDataBiosGetVoice(void* data);
extern void* pokemonNakigoeDataBiosGetDataAddress(void* data);

/* Reload the scratch Pokemon slot from a memo entry ID. */
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

/* fn_800439BC's pdaFabs: retail reloads the 0.0f literal at every compare. */
static inline f32 pda_800439BC_Fabs(f32 v)
{
    if (v > 0.0f) {
        return v;
    }
    return -v;
}

/* Cry voice of the highlighted entry, 0xffff when it cannot be resolved. */
static inline u16 pdaCurrentVoice(s32 index)
{
    u32 work = lbl_8047A4E0;
    void* data;

    if (work != 0) {
        work = pdaLoadPokemon(index);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonDataBiosGetVoice(data);
            }
            return 0xffff;
        }
        return 0xffff;
    }
    return 0xffff;
}

/* Cry sample of the highlighted entry, NULL when it cannot be resolved. */
static inline void* pdaCurrentCry(s32 index)
{
    u32 work = lbl_8047A4E0;
    void* data;

    if (work != 0) {
        work = pdaLoadPokemon(index);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonNakigoeDataBiosGetDataAddress(data);
            }
            return NULL;
        }
        return NULL;
    }
    return NULL;
}

/* Trainer-ID roll of a memo entry. */
/* RULE-EXCEPTION(user-approved): single-use inline helper; inlining the trainer roll one level deeper numbers its result after the loader's locals, which gives retail's registers in the case-1 cry reload - see docs/RULE_EXCEPTIONS.md */
static inline u32 pda_80046168_TrainerRnd(u32 species)
{
    return memoDataGetPokemonTrainerRndFromID(0, species);
}

/* pdaLoadPokemon for the case-1 cry reload, with the trainer roll in its helper. */
/* RULE-EXCEPTION(user-approved): single-use inline helper; a second copy of pdaLoadPokemon whose trainer roll goes through pda_80046168_TrainerRnd - see docs/RULE_EXCEPTIONS.md */
static inline u32 pda_80046168_LoadPokemon(s32 index)
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
        trainerRnd = pda_80046168_TrainerRnd(species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

/* pdaCurrentCry for the case-1 cry reload. */
/* RULE-EXCEPTION(user-approved): single-use inline helper; a second copy of pdaCurrentCry that reloads through pda_80046168_LoadPokemon - see docs/RULE_EXCEPTIONS.md */
static inline void* pda_80046168_CurrentCry(s32 index)
{
    u32 work = lbl_8047A4E0;
    void* data;

    if (work != 0) {
        work = pda_80046168_LoadPokemon(index);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonNakigoeDataBiosGetDataAddress(data);
            }
            return NULL;
        }
        return NULL;
    }
    return NULL;
}

/* Push a cry onto the ring of trails and age every older entry. */
#define PDA_CRY(n) (((PdaCryTrail*)lbl_802E52C8)[n])
static inline void pdaPushCry(void* data, u32 voice)
{
    s32 i;
    s32 j;
    u32 head;

    lbl_8047A4EC = lbl_8047A4EC + 1;
    if (lbl_8047A4EC >= 8) {
        lbl_8047A4EC = 0;
    }
    i = lbl_8047A4EC;
    PDA_CRY(i).data = data;
    PDA_CRY(i).voice = voice;
    PDA_CRY(i).field_0c = lbl_8047BC94;
    PDA_CRY(i).field_14 = lbl_8047BC94;
    PDA_CRY(i).offset = 0;
    PDA_CRY(i).active = 1;
    PDA_CRY(i).alpha = lbl_8047BCA0;
    PDA_CRY(i).field_20 = lbl_8047BD18;
    for (j = i - 1; j >= 0; j--) {
        PDA_CRY(j).offset = (s32)((f32)PDA_CRY(j).offset + PDA_CRY(j).speed);
        PDA_CRY(j).field_20 += lbl_8047BD1C;
    }
    head = lbl_8047A4EC;
    for (j = 7; j > head; j--) {
        PDA_CRY(j).offset = (s32)((f32)PDA_CRY(j).offset + PDA_CRY(j).speed);
        PDA_CRY(j).field_20 += lbl_8047BD1C;
    }
}

/* Fill the cry ring with empty trails. */
/* RULE-EXCEPTION(user-approved): single-use inline helper; its loop counter is an inline local, numbered after the trail push's locals, which gives retail's registers for the ring reset - see docs/RULE_EXCEPTIONS.md */
static inline void pda_80046168_ClearCries(void)
{
    s32 k;

    for (k = 0; k < 8; k++) {
        pdaPushCry(NULL, 0);
    }
}

/* 1 while any trail's cry is still playing. */
/* RULE-EXCEPTION(user-approved): single-use inline helper; returning the flag gives retail's shared zero for the flag and the loop counter - see docs/RULE_EXCEPTIONS.md */
static inline u8 pda_80046168_AnyPlaying(void)
{
    PdaCryTrail* t;
    s32 i;
    s32 playing;

    t = (PdaCryTrail*)lbl_802E52C8;
    playing = 0;
    for (i = 0; i < 8; i++) {
        if (t[i].data != NULL) {
            switch (GSsndGetStatus(t[i].voice)) {
            case 0:
                break;
            case 1:
                break;
            case 2:
                playing = 1;
                break;
            case 3:
                break;
            }
        }
    }
    return playing;
}

/* 1 when the stick or the cancel buttons leave the cry screen. */
/* RULE-EXCEPTION(user-approved): single-use inline helper; its early returns give retail's shared join for the cancel test - see docs/RULE_EXCEPTIONS.md */
static inline u8 pda_80046168_StickCancel(void)
{
    /* Retail converts the stick values to single precision (fsubs) for this
     * call, so this caller saw atan2 declared with float parameters. */
    extern f64 atan2(f32 y, f32 x);
    extern s8 GSinputGetLeftStickYData(s32 chan, s32 index);
    extern s8 GSinputGetLeftStickXData(s32 chan, s32 index);
    s8 x;
    s8 y;
    f32 ang;

    if ((GSinputGetButtons(1) & GSinputButtonsChanged(1) & 0x8) != 0) {
        return 1;
    }
    if ((GSinputGetButtons(1) & GSinputButtonsChanged(1) & 0x4) != 0) {
        return 1;
    }
    x = GSinputGetLeftStickYData(1, 0);
    y = GSinputGetLeftStickXData(1, 0);
    if ((y < 0 ? -y : y) > 0x20 || (x < 0 ? -x : x) > 0x20) {
        ang = atan2(y, x);
        if (pda_800439BC_Fabs(ang) < lbl_8047BCFC) {
            return 1;
        }
        if (pda_800439BC_Fabs(ang) > lbl_8047BD00) {
            return 1;
        }
    }
    return 0;
}

/* Cry-playback thread for the PDA's Pokemon entry. */
void pdaStrategyMemoUpdate(void)
{
    extern void _threadSwitch(void);
    u8* S;
    s32 running;
    u16 voice;
    void* cry;

    running = 1;
    pda_80046168_ClearCries();
    S = (u8*)&lbl_803A6818;
    while (1) {
        switch (*(s32*)(S + 0x20)) {
        case 0:
            menuOpen(0x3c, 0);
            menuOpen(0x3f, 0);
            menuOpen(0x3d, 0);
            menuOpen(0x9f, 0);
            menuOpen(0x96, 0);
            if (menuOpen(0xa2, 1) >= 0) {
                if (pdaEntrySeen() != 0) {
                    menuOpen(0x3e, 0);
                    voice = pdaCurrentVoice(lbl_803A6818.currentIndex);
                    soundStartRaw(voice, 0, 0);
                    pdaPushCry(pdaCurrentCry(lbl_803A6818.currentIndex), voice);
                    *(s32*)(S + 0x20) = 1;
                }
            } else {
                if (*(s32*)(S + 0x1c) == 2) {
                    *(s32*)(S + 0x1c) = 8;
                    *(f32*)(S + 0x50) = lbl_8047BC94;
                    *(f32*)(S + 0x58) = lbl_8047BCBC;
                }
                *(s32*)(S + 0x14) = 0;
                running = 0;
            }
            break;
        case 1:
            if (pda_80046168_AnyPlaying() == 0 || pda_80046168_StickCancel() == 1) {
                menuCloseCustom(0x3e, 0, 1);
                *(s32*)(S + 0x20) = 0;
            } else {
                if ((GSinputGetButtons(1) & GSinputButtonsChanged(1) & 0x100) != 0) {
                    voice = pdaCurrentVoice(lbl_803A6818.currentIndex);
                    soundStartRaw(voice, 0, 0);
                    cry = pda_80046168_CurrentCry(lbl_803A6818.currentIndex);
                    pdaPushCry(cry, voice);
                } else if ((GSinputGetButtons(1) & GSinputButtonsChanged(1) & 0x200) != 0) {
                    if (*(s32*)(S + 0x1c) == 2) {
                        *(s32*)(S + 0x1c) = 8;
                        menuCloseCustom(0x3e, 0, 1);
                        menuCloseCustom(0x3c, 0, 1);
                        *(f32*)(S + 0x50) = lbl_8047BC94;
                        *(f32*)(S + 0x58) = lbl_8047BCBC;
                    }
                    running = 0;
                }
            }
            _threadSwitch();
            break;
        default:
            break;
        }
        if (running == 0) {
            break;
        }
    }
    *(s32*)(S + 0x20) = 0;
}
