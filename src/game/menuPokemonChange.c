/**
 * @file menuPokemonChange.c
 * @brief Pokemon-change menu: story-state/save-report checks, evolution
 *        state handler, and the head of the ExChange state machine.
 *
 * Split from the former game/gs_worldmap.c CodeCandidate bucket
 * (0x80026370-0x80030170); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit (0x8002DD24-0x80030170). This
 * range was originally mislabeled as world-map code; it is actually the
 * head of the XD-era menuPokemonChange.cpp translation unit -- the
 * dispatcher fn_80031B4C and the remainder of that TU live in the next
 * unit, game/gs_npc_event.c (0x80030170-0x80033278).
 */

#include "dolphin/types.h"

/* Menu work area at lbl_803A2518: a scratch Pokemon bios used to swap the
 * traded Pokemon, the partner's hero (party) record at 0x170, and the two
 * menu models of the traded Pokemon. */
typedef struct {
    u32 bios[0x4E];
    u8 pad138[0x38];
    u8 hero[0xB60];
    u8 modelB[0x48];
    u8 modelA[0x48];
} MenuWork;

/* The 0x1DFD0-byte save image block-copied by the trade flow. */
typedef struct { u8 data[0x1DFD0]; } SaveDataImage;

/* Waits `seconds` of real time, one frame at a time. */
static inline void menuPokemonChangeWait(f32 seconds) {
    extern const f32 lbl_8047B9D4;
    extern void _threadSwitch(void);
    extern u32 GSgetFrameStep(void);
    extern s32 fn_800D37CC(void);
    f32 elapsed = lbl_8047B9D4;

    while (elapsed < seconds) {
        _threadSwitch();
        elapsed += (f32)GSgetFrameStep() / (f32)fn_800D37CC();
    }
}

/* Shows message `msgId` in item `itemId` of window `windowId`. */
static inline void menuPokemonChangeSetMessage(s32 windowId, s32 itemId, u32 msgId) {
    extern void* windowSearchID(s32 id);
    extern void* windowSearchItemID(void* window, s32 itemId);
    extern void winSpriteSetDisp(void* sprite, u8 disp);
    void* window;
    void* item;

    window = windowSearchID(windowId);
    item = windowSearchItemID(window, itemId);
    if (window != NULL && item != NULL) {
        winSpriteSetDisp(item, 1);
        *(u32*)((u8*)item + 0x4C) = msgId;
    }
}

/* Hides the message item again. */
static inline void menuPokemonChangeClearMessage(s32 windowId, s32 itemId) {
    extern void* windowSearchID(s32 id);
    extern void* windowSearchItemID(void* window, s32 itemId);
    extern void winSpriteSetDisp(void* sprite, u8 disp);
    void* window;
    void* item;

    window = windowSearchID(windowId);
    item = windowSearchItemID(window, itemId);
    if (window != NULL && item != NULL) {
        *(u32*)((u8*)item + 0x4C) = 0;
        winSpriteSetDisp(item, 0);
    }
}

/* Shows `msgId` in the change menu (0xD9) with a buzzer for 1.5 s. */
static inline void menuPokemonChangeAlert(u32 msgId) {
    extern const f32 lbl_8047B9DC;
    extern void soundStartRaw(s32 se, s32 a, s32 b);
    menuPokemonChangeSetMessage(0xD9, 0x10B2, msgId);
    soundStartRaw(0x26, 0, 0);
    menuPokemonChangeWait(lbl_8047B9DC);
    menuPokemonChangeClearMessage(0xD9, 0x10B2);
}

#if defined(MENU_POKEMON_CHANGE_EXACT_8002DD24_ONLY) || defined(MENU_POKEMON_CHANGE_EXACT_8002DF10_ONLY) || \
    defined(MENU_POKEMON_CHANGE_EXACT_8002E460_ONLY) || defined(MENU_POKEMON_CHANGE_EXACT_8002F284_ONLY) || \
    defined(MENU_POKEMON_CHANGE_EXACT_8002EE74_ONLY) || defined(MENU_POKEMON_CHANGE_EXACT_8002F79C_ONLY) || \
    defined(MENU_POKEMON_CHANGE_EXACT_8002EA5C_ONLY)
/* A wrapper unit that links one exact function of this file. */
#define MENU_POKEMON_CHANGE_EXACT_ISLAND
#endif

#if !defined(MENU_POKEMON_CHANGE_EXACT_ISLAND) || defined(MENU_POKEMON_CHANGE_EXACT_8002DD24_ONLY)

/* stateFunctionSaveReport - 0x8002DD24 | size: 0x1ec */
extern void fn_80089E20(void);
extern void fn_801D055C(void);
extern u8 fn_801D04D0(void);
extern void fn_80089D98(void);
extern void fn_801D046C(void);
extern s32 memcardGetTaskResult(void);
extern void gbaCommandSetKeyState(void);
extern void fn_801D039C(void);
extern void menuSubKeyWait(void);
extern void _fadeEffectGetRandom__FUl(void);
extern u32 lbl_8047A424;
extern u8 lbl_803A2518[];
extern u32 lbl_8047A420;
extern u32 lbl_8047A40C;
extern u32 lbl_804788B0;
extern u32 lbl_8047A42C;

/* 0x8002DD24 | size: 0x1EC
 * Waits for the GBA side of a trade/report handshake, then either copies the
 * caller's 0x1DFD0-byte save image over the live save data and reports
 * success, or waits a random delay and reports failure. */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void stateFunctionSaveReport(void* src) {
    extern u8 lbl_803A2518[];
    extern u32 lbl_8047A424;
    extern u32 lbl_8047A420;
    extern u32 lbl_8047A40C;
    extern u32 lbl_8047A42C;
    extern void* heroGetStatus(void* hero, u32 selector, u16 index);
    extern void winMsgOpen(s32 slot, s32 msgId, s32 p3, s32 p4);
    extern s32 fn_80089E20(s32 mode, void* pkm, u32 slotB, u32 flags);
    extern s32 fn_80089D98(s32 slot);
    extern void fn_801D055C(s32 a, s32 b, s32 c);
    extern u8 fn_801D04D0(void);
    extern void fn_801D046C(s32 flag);
    extern s32 memcardGetTaskResult(void);
    extern void fn_801D039C(void);
    extern void _threadSwitch(void);
    extern void gbaCommandSetKeyState(s32 mode, s32 flag);
    extern void* savedataGetStatus(s32 side, s32 slotType);
    extern void menuSubKeyWait(void);
    extern void winMsgClose(s32 slot);
    extern void fn_8010A420(u8* ptr);
    extern u32 _fadeEffectGetRandom__FUl(s32 frames);
    void* pokemon;
    MenuWork* base;
    u8 sent;
    u8 confirmed;
    s32 result;
    s32 state;
    u32 timer;

    base = (MenuWork*)lbl_803A2518;
    heroGetStatus(NULL, 3, lbl_8047A424);
    pokemon = heroGetStatus(base->hero, 3, lbl_8047A420);
    confirmed = 0;
    sent = 0;
    winMsgOpen(2, 0x44D7, 1, 1);
    if (fn_80089E20(2, pokemon, lbl_8047A420, lbl_8047A40C) == 0) {
        fn_801D055C(8, 2, 0);
        do {
            if (sent == 0 && fn_801D04D0() != 0) {
                state = fn_80089D98(2);
                if (state >= 0) {
                    if (state != 0) {
                        fn_801D046C(0);
                    } else {
                        fn_801D046C(1);
                    }
                    sent = 1;
                }
            }
            _threadSwitch();
            result = memcardGetTaskResult();
        } while (result == 0);
        gbaCommandSetKeyState(2, 1);
        fn_801D039C();
        if (result == 4) {
            confirmed = 1;
        }
    }
    if (sent == 0 || confirmed == 0) {
        *(SaveDataImage*)savedataGetStatus(0, 0) = *(SaveDataImage*)src;
        winMsgOpen(2, 0x44D6, 1, 0);
        menuSubKeyWait();
        winMsgClose(1);
        fn_8010A420(base->modelA);
        fn_8010A420(base->modelB);
        *(u8*)&lbl_804788B0 = 0;
        lbl_8047A42C = 0;
    } else {
        timer = _fadeEffectGetRandom__FUl(0x3C);
        while (timer-- != 0) {
            _threadSwitch();
        }
        winMsgOpen(2, 0x44D5, 1, 0);
        menuSubKeyWait();
        winMsgClose(1);
        lbl_8047A42C = 0x13;
    }
}
#pragma pop

#endif

#if !defined(MENU_POKEMON_CHANGE_EXACT_ISLAND) || defined(MENU_POKEMON_CHANGE_EXACT_8002DF10_ONLY)

/* 0x8002DF10 | size: 0x35C
 * Runs any pending evolution of the two traded Pokemon (hero slot A and the
 * partner record's slot B), then rebuilds their menu models. */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#if !defined(MENU_POKEMON_CHANGE_EXACT_8002DF10_ONLY)
#endif
#if !defined(MENU_POKEMON_CHANGE_EXACT_8002DF10_ONLY)
#endif
void stateFunctionEvolution(void) {
    extern u8 lbl_803A2518[];
    extern u32 lbl_8047A424;
    extern u32 lbl_8047A420;
    extern u32 lbl_8047A40C;
    extern f32 lbl_8047B9D0;
    extern f32 lbl_8047B9D4;
    extern u8 lbl_8047A41C;
    extern volatile u8 lbl_8047A408;
    extern u32 lbl_8047A418;
    extern u32 lbl_8047A414;
    extern u32 lbl_8047A42C;
    extern void* heroGetStatus(void* hero, u32 selector, u16 index);
    extern u16 pokemonEvolutionCheck(void* pokemon, u32 mode, u16 arg, u16* keyOut, u32* typeOut);
    extern u16 pokemonBiosGetPokemonDataId(void* pokemon);
    extern void fadeSet(f32 vol, s32 mode);
    extern void fadeCheck(s32 flag);
    extern void menuClose(s32 id);
    extern void fn_8010A420(void* model);
    extern void std113_mapObjectUnk_8018db68(u32 handle);
    extern void floorSetMapVisibility(s32 flag);
    extern void _threadSwitch(void);
    extern s32 pokemonEvolutionAll(void* pokemon, u16 species, u16 arg, u32* type, void* team, s32 a5, s32 a6, s32 a7);
    extern void menuModelInit(void* model, s32 w, s32 h);
    extern void fn_80109C88(void* model, void* pokemon);
    extern void cameraPlayAnime(s32 id, u32 color, s32 a, s32 b);
    extern u32 std093_getCurrentGroupID(void);
    extern u32 std112_modelGetResourceID(u32 color);
    extern void* GSresGetResource(u32 group, u32 id);
    extern void GSscene_SetMode(s32 mode);
    extern void GSmodelSetVisibility(void* obj, s32 flag);
    extern s32 menuOpen(s32 id, s32 flag);
    MenuWork* work;
    u8 evolve;
    void* pokemonB;
    void* pokemonA;
    u32 resource;
    u16 species;
    u16 key;
    u32 type;
    u16 keyA;
    u32 typeA;
    u16 keyB;
    u32 typeB;

    work = (MenuWork*)lbl_803A2518;
    lbl_8047A40C = 0;
    evolve = 0;
    pokemonA = heroGetStatus(NULL, 3, lbl_8047A424);
    pokemonB = heroGetStatus(work->hero, 3, lbl_8047A420);
    species = pokemonEvolutionCheck(pokemonA, 2, 0, &key, &type);
    if (species != 0 && species != 0xFFFF) {
        evolve = 1;
    }
    species = pokemonEvolutionCheck(pokemonB, 2, 0, &key, &type);
    if (species != 0 && species != 0xFFFF) {
        evolve = 1;
        lbl_8047A40C = pokemonBiosGetPokemonDataId(pokemonB);
    }
    if (evolve == 1) {
        lbl_8047A41C = 0;
        fadeSet(lbl_8047B9D0, 3);
        fadeCheck(1);
        menuClose(0xDE);
        fn_8010A420(work->modelA);
        fn_8010A420(work->modelB);
        std113_mapObjectUnk_8018db68(lbl_8047A418);
        floorSetMapVisibility(0);
        _threadSwitch();
        fadeSet(lbl_8047B9D4, 2);
        fadeCheck(1);
        species = pokemonEvolutionCheck(pokemonA, 2, 0, &keyA, &typeA);
        if (species != 0 && species != 0xFFFF) {
            lbl_8047A408 = 1;
            if (pokemonEvolutionAll(pokemonA, species, 0, &typeA, NULL, 1, 0, 1) == 2) {
                lbl_8047A408 = 0;
            }
            lbl_8047A408 = 0;
        }
        species = pokemonEvolutionCheck(pokemonB, 2, 0, &keyB, &typeB);
        if (species != 0 && species != 0xFFFF) {
            lbl_8047A408 = 1;
            if (pokemonEvolutionAll(pokemonB, species, 0, &typeB, NULL, 0, 0, 1) == 2) {
                lbl_8047A408 = 0;
            }
            lbl_8047A408 = 0;
        }
        fadeSet(lbl_8047B9D4, 3);
        fadeCheck(1);
        pokemonA = heroGetStatus(NULL, 3, lbl_8047A424);
        pokemonB = heroGetStatus(work->hero, 3, lbl_8047A420);
        menuModelInit(work->modelA, 0xE8, 0x11C);
        menuModelInit(work->modelB, 0xE8, 0x11C);
        fn_80109C88(work->modelA, pokemonA);
        fn_80109C88(work->modelB, pokemonB);
        cameraPlayAnime(0x37C, 0x0FFF1800, 0, 1);
        resource = std093_getCurrentGroupID();
        lbl_8047A418 = std112_modelGetResourceID(0x0FFE1000);
        lbl_8047A414 = (u32)GSresGetResource(resource, lbl_8047A418);
        cameraPlayAnime(0x37C, 0x0FFF1800, 0, 1);
        GSscene_SetMode(4);
        GSmodelSetVisibility((void*)lbl_8047A414, 1);
        floorSetMapVisibility(0);
        menuOpen(0xDE, 1);
        fadeSet(lbl_8047B9D0, 2);
        fadeCheck(1);
        lbl_8047A41C = 1;
    }
    lbl_8047A42C = 0x12;
}
#if !defined(MENU_POKEMON_CHANGE_EXACT_8002DF10_ONLY)
#endif

#endif /* guard split */
#if !defined(MENU_POKEMON_CHANGE_EXACT_ISLAND)
/* 0x8002E26C | size: 0x1F4
 * Swaps the two traded Pokemon through the scratch bios at the start of the
 * work area, then replays the trade camera and rebuilds both menu models. */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void stateFunctionExChangeMain(void) {
    extern u8 lbl_803A2518[];
    extern u32 lbl_8047A424;
    extern u32 lbl_8047A420;
    extern f32 lbl_8047B9D0;
    extern f32 lbl_8047B9D8;
    extern u8 lbl_8047A41C;
    extern u32 lbl_8047A414;
    extern u32 lbl_8047A42C;
    extern void pokemonInit(void* bios);
    extern void* heroGetStatus(void* hero, u32 selector, u16 index);
    extern void pokemonBiosCopy(u32* dst, u32* src);
    extern void menuClose(s32 id);
    extern void fn_8010A420(void* model);
    extern void GSmodelSetVisibility(void* obj, s32 flag);
    extern void fadeSet(f32 vol, s32 mode);
    extern void fadeCheck(s32 flag);
    extern void fn_801024E8(s32 flag);
    extern u32 std093_getCurrentGroupID(void);
    extern void cameraPlayAnime(s32 id, u32 color, s32 a, s32 b);
    extern void _threadSwitch(void);
    extern void soundStartRaw(s32 a, s32 b, s32 c);
    extern void floorSetMapVisibility(s32 flag);
    extern void std115_mapObjectPlayAnimation(u32 color, s32 a, s32 b, s32 c);
    extern void cameraWaitSyncAnime(s32 flag);
    extern void menuModelInit(void* model, s32 w, s32 h);
    extern void fn_80109C88(void* model, void* pokemon);
    extern s32 menuOpen(s32 id, s32 flag);
    MenuWork* work;
    void* pokemonA;
    void* pokemonB;

    work = (MenuWork*)lbl_803A2518;
    pokemonInit(work->bios);
    pokemonA = heroGetStatus(NULL, 3, lbl_8047A424);
    pokemonB = heroGetStatus(work->hero, 3, lbl_8047A420);
    pokemonBiosCopy(work->bios, pokemonB);
    pokemonBiosCopy(pokemonB, pokemonA);
    pokemonBiosCopy(pokemonA, work->bios);
    menuClose(0xDE);
    fn_8010A420(work->modelA);
    fn_8010A420(work->modelB);
    lbl_8047A41C = 0;
    GSmodelSetVisibility((void*)lbl_8047A414, 0);
    fadeSet(lbl_8047B9D0, 3);
    fadeCheck(1);
    fn_801024E8(1);
    cameraPlayAnime(std093_getCurrentGroupID(), 0x10B61800, 0, 0);
    _threadSwitch();
    soundStartRaw(0x4C8, 0, 0);
    floorSetMapVisibility(1);
    std115_mapObjectPlayAnimation(0x10B11000, 0, 0, 0);
    fadeSet(lbl_8047B9D0, 2);
    fadeCheck(1);
    cameraWaitSyncAnime(1);
    fadeSet(lbl_8047B9D8, 3);
    fadeCheck(1);
    floorSetMapVisibility(0);
    menuModelInit(work->modelA, 0xE8, 0x11C);
    menuModelInit(work->modelB, 0xE8, 0x11C);
    fn_80109C88(work->modelA, pokemonA);
    fn_80109C88(work->modelB, pokemonB);
    cameraPlayAnime(0x37C, 0x0FFF1800, 0, 1);
    GSmodelSetVisibility((void*)lbl_8047A414, 1);
    menuOpen(0xDE, 1);
    lbl_8047A41C = 1;
    fadeSet(lbl_8047B9D8, 2);
    fadeCheck(1);
    lbl_8047A42C = 0x10;
}
#pragma pop

#endif

#if !defined(MENU_POKEMON_CHANGE_EXACT_ISLAND)

/* One row of the change menu's item table (lbl_80266E90). */
typedef struct PokemonChangeMenuEntry {
    u8 _00;
    u8 slot;
    u8 _02[0x0E];
    u16 itemId;
} PokemonChangeMenuEntry;

/* Whether a party Pokemon may be picked for a trade. */
static inline u8 menuPokemonChangeCanSelect(void* pokemon) {
    extern u8 pokemonCheckValid(void* pokemon);
    extern u8 menuCBRule_CheckPokemonEventFlag(void* pokemon);
    u8 selectable = 0;

    if (pokemonCheckValid(pokemon) != 0 && menuCBRule_CheckPokemonEventFlag(pokemon) == 1) {
        selectable = 1;
    }
    return selectable;
}

/* 0x8002FC58 | size: 0x518
 * Builds and runs the Pokemon-change selection menu (0xD9). */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_8002FC58(void) {
    extern void* savedataGetStatus(s32 side, s32 slotType);
    extern void* heroBiosGetPokemonPtr(void* hero, u16 index);
    extern u8 pokemonCheckValid(void* pokemon);
    extern u8 menuCBRule_CheckPokemonEventFlag(void* pokemon);
    extern void menuItemBiosSetSelectFlag(s32 itemId, s32 flag);
    extern void fn_80030170(void);
    extern void fn_8010B01C(s32 a, void (*cb)(void));
    extern s32 menuGetCursor(s32 id);
    extern s32 menuGetCursorFromItemID(s32 id, s32 itemId);
    extern void fn_801021F8(s32 id, s32 flag);
    extern s32 windowGetActiveID(void);
    extern void menuOpenCustom(s32 id, s32 parent, ...);
    extern void windowCheckCursor(s32 id, s32 flag);
    extern s32 windowGetValue(s32 id);
    extern s32 menuGetCursorItemID(s32 id);
    extern void* lbl_803A2650[];
    extern u8 lbl_803A2688[];
    extern PokemonChangeMenuEntry lbl_80266E90[];
    extern u8 lbl_8047A410;
    extern u32 lbl_8047A428;
    extern u32 lbl_8047A42C;
    void* save;
    void* party;
    s32 i;
    void* pokemon;
    s32 count;
    s32 cursor;
    s32 value;
    s32 selected;
    s32 result;

    cursor = count = 0;
    party = savedataGetStatus(0, 2);
    for (i = 0; i < 6; i++) {
        pokemon = heroBiosGetPokemonPtr(party, i);
        if (pokemonCheckValid(pokemon) != 0 && menuCBRule_CheckPokemonEventFlag(pokemon) == 1) {
            lbl_803A2650[count++] = pokemon;
        }
    }
    party = lbl_803A2688;
    for (i = 0; i < 6; i++) {
        pokemon = heroBiosGetPokemonPtr(party, i);
        if (pokemonCheckValid(pokemon) != 0 && menuCBRule_CheckPokemonEventFlag(pokemon) == 1) {
            lbl_803A2650[count++] = pokemon;
        }
    }
    lbl_803A2650[count] = NULL;
    lbl_803A2650[13] = NULL;
    fn_8010B01C(0, fn_80030170);
    save = savedataGetStatus(0, 2);
    menuItemBiosSetSelectFlag(0x1005, menuPokemonChangeCanSelect(heroBiosGetPokemonPtr(save, 0)));
    menuItemBiosSetSelectFlag(0x1002, menuPokemonChangeCanSelect(heroBiosGetPokemonPtr(save, 1)));
    menuItemBiosSetSelectFlag(0x1004, menuPokemonChangeCanSelect(heroBiosGetPokemonPtr(save, 2)));
    menuItemBiosSetSelectFlag(0x1001, menuPokemonChangeCanSelect(heroBiosGetPokemonPtr(save, 3)));
    menuItemBiosSetSelectFlag(0x1003, menuPokemonChangeCanSelect(heroBiosGetPokemonPtr(save, 4)));
    menuItemBiosSetSelectFlag(0x1000, menuPokemonChangeCanSelect(heroBiosGetPokemonPtr(save, 5)));
    menuItemBiosSetSelectFlag(0xFFF, 0);
    menuItemBiosSetSelectFlag(0xFFC, 0);
    menuItemBiosSetSelectFlag(0xFFE, 0);
    menuItemBiosSetSelectFlag(0xFFB, 0);
    menuItemBiosSetSelectFlag(0xFFD, 0);
    menuItemBiosSetSelectFlag(0xFFA, 0);
    if (lbl_8047A410 != 0 || menuGetCursor(0xD9) == 0) {
        cursor = menuGetCursorFromItemID(0xD9, 0x1005);
        lbl_8047A410 = 0;
    }
    fn_801021F8(0xD9, 1);
    if (cursor != 0) {
        menuOpenCustom(0xD9, windowGetActiveID(), &cursor, 0, 0, 0);
    } else {
        menuOpenCustom(0xD9, windowGetActiveID(), 0, 0, 0, 0);
    }
    menuPokemonChangeSetMessage(0xD9, 0x10B2, 0x43D9);
    windowCheckCursor(0xD9, 1);
    value = windowGetValue(0xD9);
    selected = 0;
    cursor = menuGetCursorItemID(0xD9);
    for (i = 0; i < 12; i++) {
        if (cursor == lbl_80266E90[i].itemId) {
            selected = lbl_80266E90[i].slot;
        }
    }
    if (menuGetCursorItemID(0xD9) == 0xFF9) {
        selected = 1000;
    }
    result = selected;
    if (value == -1) {
        result = -1;
    }
    lbl_8047A428 = -1;
    switch (result) {
    case -1:
        lbl_8047A42C = 4;
        break;
    case 1000:
        lbl_8047A42C = 4;
        break;
    default:
        lbl_8047A428 = result;
        lbl_8047A42C = 3;
        break;
    }
}
#pragma pop


#endif

#if !defined(MENU_POKEMON_CHANGE_EXACT_ISLAND) || defined(MENU_POKEMON_CHANGE_EXACT_8002E460_ONLY)

/* Shows or hides item `itemId` of the trade-confirm window (0xDB). */
#define MENU_POKEMON_CHANGE_SET_DISP(itemId, disp) \
    winSpriteSetDisp(windowSearchItemID(windowSearchID(0xDB), (itemId)), (disp))

/* 0x8002E460 | size: 0x5FC
 * Trade confirmation: shows both Pokemon and waits until each side has
 * confirmed (A on this side, the link flags for the partner). Either side
 * backing out of an unconfirmed state cancels the trade; on success the
 * saved image is copied into `dst`. */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_8002E460(void* dst) {
    extern u8 lbl_803A2518[];
    extern u32 lbl_8047A424;
    extern u32 lbl_8047A420;
    extern u32 lbl_8047A428;
    extern u32 lbl_8047A42C;
    extern u8 lbl_8047A410;
    extern f32 lbl_8047B9D0;
    extern void* heroGetStatus(void* hero, u32 selector, u16 index);
    extern void menuModelInit(void* model, s32 w, s32 h);
    extern void fn_80109C88(void* model, void* pokemon);
    extern void fn_8010A420(void* model);
    extern s32 menuOpen(s32 id, s32 flag);
    extern void* windowSearchID(s32 id);
    extern void* windowSearchItemID(void* window, s32 itemId);
    extern void winSpriteSetDisp(void* sprite, u8 disp);
    extern void menuClose(s32 id);
    extern void fadeSet(f32 vol, s32 mode);
    extern void fadeCheck(s32 flag);
    extern u8* windowGetKeyInfo(void);
    extern s32 fn_80073A44(s32 mode, u16* flags);
    extern s32 fn_8017B1AC(void);
    extern void soundStartRaw(s32 se, s32 a, s32 b);
    extern void winMsgOpen(s32 slot, s32 msgId, s32 p3, s32 p4);
    extern void _threadSwitch(void);
    extern void* savedataGetStatus(s32 side, s32 slotType);
    MenuWork* work;
    void* pokemonA;
    void* pokemonB;
    u8 readyA;
    u8 readyB;
    u32 held;
    u16 prev;
    u16 pressed;
    u16 flags;

    work = (MenuWork*)lbl_803A2518;
    pokemonA = heroGetStatus(NULL, 3, lbl_8047A424);
    pokemonB = heroGetStatus(work->hero, 3, lbl_8047A420);
    menuModelInit(work->modelA, 0xE4, 0x8F);
    menuModelInit(work->modelB, 0xE4, 0x8F);
    fn_80109C88(work->modelA, pokemonA);
    fn_80109C88(work->modelB, pokemonB);
    menuOpen(0xDB, 0);
    MENU_POKEMON_CHANGE_SET_DISP(0x11A8, 0);
    MENU_POKEMON_CHANGE_SET_DISP(0xF9B, 1);
    MENU_POKEMON_CHANGE_SET_DISP(0xF9A, 1);
    MENU_POKEMON_CHANGE_SET_DISP(0xFA3, 1);
    MENU_POKEMON_CHANGE_SET_DISP(0xFA5, 1);
    MENU_POKEMON_CHANGE_SET_DISP(0x11A9, 0);
    MENU_POKEMON_CHANGE_SET_DISP(0xF99, 1);
    MENU_POKEMON_CHANGE_SET_DISP(0xF98, 1);
    MENU_POKEMON_CHANGE_SET_DISP(0xFA4, 1);
    MENU_POKEMON_CHANGE_SET_DISP(0xFA6, 1);
    fadeSet(lbl_8047B9D0, 2);
    fadeCheck(1);
    readyA = 0;
    readyB = 0;
    while (!readyA || !readyB) {
        held = *(u16*)(windowGetKeyInfo() + 4);
        if (fn_80073A44(1, &flags) != 0) {
            winMsgOpen(2, 0x4448, 1, 0);
            menuClose(0xDB);
            fn_8010A420(work->modelA);
            fn_8010A420(work->modelB);
            lbl_8047A42C = 0;
            return;
        }
        if (fn_8017B1AC() != 5) {
            pressed = flags & ~prev;
            prev = flags;
            if (held & 0x10) {
                if (!readyA) {
                    soundStartRaw(0x24, 0, 0);
                }
                readyA = 1;
            } else if (held & 0x20) {
                if (readyA) {
                    soundStartRaw(0x25, 0, 0);
                    readyA = 0;
                } else {
                    soundStartRaw(0x25, 0, 0);
                    readyA = 0;
                    break;
                }
            }
            if (pressed & 1) {
                if (!readyB) {
                    soundStartRaw(0x24, 0, 0);
                }
                readyB = 1;
            } else if (pressed & 2) {
                if (readyB) {
                    soundStartRaw(0x25, 0, 0);
                    readyB = 0;
                } else {
                    soundStartRaw(0x25, 0, 0);
                    readyB = 0;
                    break;
                }
            }
        }
        _threadSwitch();
        MENU_POKEMON_CHANGE_SET_DISP(0x11A8, readyA);
        MENU_POKEMON_CHANGE_SET_DISP(0xF9B, !readyA);
        MENU_POKEMON_CHANGE_SET_DISP(0xF9A, !readyA);
        MENU_POKEMON_CHANGE_SET_DISP(0xFA3, !readyA);
        MENU_POKEMON_CHANGE_SET_DISP(0xFA5, !readyA);
        MENU_POKEMON_CHANGE_SET_DISP(0x11A9, readyB);
        MENU_POKEMON_CHANGE_SET_DISP(0xF99, !readyB);
        MENU_POKEMON_CHANGE_SET_DISP(0xF98, !readyB);
        MENU_POKEMON_CHANGE_SET_DISP(0xFA4, !readyB);
        MENU_POKEMON_CHANGE_SET_DISP(0xFA6, !readyB);
    }
    MENU_POKEMON_CHANGE_SET_DISP(0x11A8, readyA);
    MENU_POKEMON_CHANGE_SET_DISP(0xF9B, !readyA);
    MENU_POKEMON_CHANGE_SET_DISP(0xF9A, !readyA);
    MENU_POKEMON_CHANGE_SET_DISP(0xFA3, !readyA);
    MENU_POKEMON_CHANGE_SET_DISP(0xFA5, !readyA);
    MENU_POKEMON_CHANGE_SET_DISP(0x11A9, readyB);
    MENU_POKEMON_CHANGE_SET_DISP(0xF99, !readyB);
    MENU_POKEMON_CHANGE_SET_DISP(0xF98, !readyB);
    MENU_POKEMON_CHANGE_SET_DISP(0xFA4, !readyB);
    MENU_POKEMON_CHANGE_SET_DISP(0xFA6, !readyB);
    if (!readyA || !readyB) {
        menuClose(0xDB);
        lbl_8047A428 = -1;
        lbl_8047A424 = -1;
        lbl_8047A420 = -1;
        fn_8010A420(work->modelA);
        fn_8010A420(work->modelB);
        lbl_8047A410 = 1;
        lbl_8047A42C = 2;
    } else {
        fn_8010A420(work->modelA);
        fn_8010A420(work->modelB);
        *(SaveDataImage*)dst = *(SaveDataImage*)savedataGetStatus(0, 0);
        lbl_8047A42C = 0xD;
        fadeSet(lbl_8047B9D0, 3);
        fadeCheck(1);
        menuClose(0xDB);
    }
}
#pragma pop

#endif

#if !defined(MENU_POKEMON_CHANGE_EXACT_ISLAND) || defined(MENU_POKEMON_CHANGE_EXACT_8002F79C_ONLY) || \
    defined(MENU_POKEMON_CHANGE_EXACT_8002EA5C_ONLY)

/* Whether `party` has another Pokemon, besides slot `exclude`, that can
 * stay behind (healthy, not an egg, event flag set, nonzero 0x83). */
static inline u8 menuPokemonChangeHasOther(void* hero, s32 exclude) {
    extern void* heroBiosGetPokemonPtr(void* hero, u16 index);
    extern u8 pokemonBiosGetFuseiFlag(void* pokemon);
    extern u8 pokemonCheckValid(void* pokemon);
    extern u8 menuCBRule_CheckPokemonEventFlag(void* pokemon);
    extern u8 pokemonBiosGetTamagoFlag(void* pokemon);
    extern void* pokemonGetStatus(void* pokemon, u32 slot, u16 tableId, u32 flags);
    void* other;
    u16 i;
    u8 found;

    for (i = found = 0; i < 6; i++) {
        if (i == exclude) {
            continue;
        }
        other = heroBiosGetPokemonPtr(hero, i);
        if (pokemonBiosGetFuseiFlag(other) == 0 && pokemonCheckValid(other) != 0 &&
            menuCBRule_CheckPokemonEventFlag(other) == 1 && pokemonBiosGetTamagoFlag(other) == 0 &&
            (u16)(u32)pokemonGetStatus(other, 0, 0x83, 0) != 0) {
            found = 1;
        }
    }
    return found;
}

#endif /* guard split */
#if !defined(MENU_POKEMON_CHANGE_EXACT_ISLAND) || defined(MENU_POKEMON_CHANGE_EXACT_8002EA5C_ONLY)
/* 0x8002EA5C | size: 0x418
 * Picks the selected party Pokemon as the one to trade, refusing it when its
 * held item cannot be traded or when no other Pokemon could stay behind. */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
/* Retail does not share the repeated subexpressions here: common-subexpression
 * elimination stays off for this function (see docs/RULE_EXCEPTIONS.md). */
#pragma push
#pragma peephole off
#pragma opt_common_subs off
void fn_8002EA5C(void) {
    extern u8 lbl_803A2688[];
    extern u32 lbl_8047A428;
    extern u32 lbl_8047A420;
    extern u32 lbl_8047A42C;
    extern const f32 lbl_8047B9D0;
    extern void* heroGetStatus(void* hero, u32 selector, u16 index);
    extern u16 pokemonBiosGetItemDataId(void* pokemon);
    extern void* itemDataBiosGetPtr(u16 id);
    extern u8 itemDataBiosCheckImportable(void* item);
    extern void fadeSet(f32 vol, s32 mode);
    extern void fadeCheck(s32 flag);
    extern void menuClose(s32 id);
    extern void fn_801021F8(s32 id, s32 flag);
    extern void* pokemonBiosGetNicknamePtr(void* pokemon);
    extern void msgctrlSetValue(s32 id, void* value);
    void* pokemon;
    u8* hero;
    u8 importable;
    void* name;
    u16 itemId;

    hero = lbl_803A2688;
    pokemon = heroGetStatus(hero, 3, lbl_8047A428);
    itemId = pokemonBiosGetItemDataId(pokemon);
    if (itemId != 0) {
        importable = itemDataBiosCheckImportable(itemDataBiosGetPtr(itemId));
    } else {
        importable = 1;
    }
    if (importable == 0) {
        fn_801021F8(0xD9, 0);
        name = pokemonBiosGetNicknamePtr(pokemon);
        msgctrlSetValue(0x32, name);
        menuPokemonChangeAlert(0x43DD);
        fn_801021F8(0xD9, 1);
        lbl_8047A42C = 7;
        return;
    }
    if (menuPokemonChangeHasOther(hero, lbl_8047A428) == 0) {
        fn_801021F8(0xD9, 0);
        msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(pokemon));
        menuPokemonChangeAlert(0x44E8);
        fn_801021F8(0xD9, 1);
        lbl_8047A42C = 7;
        return;
    }
    lbl_8047A420 = lbl_8047A428;
    menuPokemonChangeWait(lbl_8047B9D0);
    fadeSet(lbl_8047B9D0, 3);
    fadeCheck(1);
    menuClose(0xD9);
    lbl_8047A42C = 0xC;
}
#pragma pop

#endif /* guard split */
#if !defined(MENU_POKEMON_CHANGE_EXACT_ISLAND) || defined(MENU_POKEMON_CHANGE_EXACT_8002EE74_ONLY)
/* Waits for a choice in menu `id`, closes it, and returns the cursor
 * (-1 when the menu was cancelled). */
/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_8002EE74) — see docs/RULE_EXCEPTIONS.md */
static inline s32 menuPokemonChangeGetChoice(s32 id) {
    extern void windowCheckCursor(s32 id, s32 flag);
    extern s32 windowGetValue(s32 id);
    extern s32 menuGetCursor(s32 id);
    extern void menuClose(s32 id);
    s32 value;
    s32 cursor;

    windowCheckCursor(id, 1);
    value = windowGetValue(id);
    cursor = menuGetCursor(id);
    menuClose(id);
    if (value == -1) {
        cursor = -1;
    }
    return cursor;
}

/* 0x8002EE74 | size: 0x410
 * Summary-screen check for the selected party Pokemon: refuses eggs and
 * fainted Pokemon, otherwise asks what to do with it. */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_8002EE74(void) {
    extern u8 lbl_803A2688[];
    extern u32 lbl_8047A428;
    extern u32 lbl_8047A42C;
    extern void* heroGetStatus(void* hero, u32 selector, u16 index);
    extern void fn_801021F8(s32 id, s32 flag);
    extern u8 pokemonCheckValid(void* pokemon);
    extern u8 menuCBRule_CheckPokemonEventFlag(void* pokemon);
    extern u8 pokemonBiosGetTamagoFlag(void* pokemon);
    extern u8 pokemonBiosGetFuseiFlag(void* pokemon);
    extern void menuSetEnablePort(s32 port);
    extern s32 windowGetActiveID(void);
    extern void menuOpenCustom(s32 id, s32 parent, ...);
    extern void menuClose(s32 id);
    extern s32 menuGetLastError(void);
    extern void winMsgOpen(s32 slot, s32 msgId, s32 p3, s32 p4);
    s32 cursor;
    void* pokemon;
    s32 initial;

    pokemon = heroGetStatus(lbl_803A2688, 3, lbl_8047A428);
    fn_801021F8(0xD9, 0);
    if (pokemonCheckValid(pokemon) != 0 && menuCBRule_CheckPokemonEventFlag(pokemon) == 1 &&
        pokemonBiosGetTamagoFlag(pokemon) != 0) {
        menuPokemonChangeAlert(0x43E1);
        lbl_8047A42C = 7;
        return;
    }
    if (pokemonBiosGetFuseiFlag(pokemon) != 0) {
        menuPokemonChangeAlert(0x44BE);
        lbl_8047A42C = 7;
        return;
    }
    menuSetEnablePort(2);
    initial = 1;
    menuOpenCustom(0xE3, windowGetActiveID(), &initial, 0, 0, 0);
    menuPokemonChangeSetMessage(0xE3, 0x102A, 0x43E4);
    menuPokemonChangeSetMessage(0xE3, 0x1029, 0x43E5);
    cursor = menuPokemonChangeGetChoice(0xE3);
    menuSetEnablePort(1);
    if (menuGetLastError() == 1) {
        winMsgOpen(2, 0x4448, 1, 0);
        menuClose(0xD9);
        lbl_8047A42C = 0;
        return;
    }
    switch (cursor) {
    case 1:
        lbl_8047A42C = 0xA;
        break;
    case 0:
        lbl_8047A42C = 0xB;
        break;
    case -1:
        lbl_8047A42C = 7;
        break;
    }
}
#pragma pop

#endif /* !MENU_POKEMON_CHANGE_EXACT_ISLAND */

#if !defined(MENU_POKEMON_CHANGE_EXACT_ISLAND) || defined(MENU_POKEMON_CHANGE_EXACT_8002F284_ONLY)
/* fn_8002F284 - 0x8002F284 | size: 0x518 */
extern void menuItemBiosSetSelectFlag(void);
extern void menuGetCursorFromItemID(void);
extern void menuGetCursorItemID(void);
extern u32 lbl_8047A410;
extern u32 lbl_8047A42C;
extern const u8 lbl_80266E90[];
extern u32 lbl_8047A428;
#if 1
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void fn_8002F284(void)
{
    /* --- UI item-enable dispatcher: menuItemBiosSetSelectFlag(u32 elementId, u32 val) --- */
    extern void menuItemBiosSetSelectFlag(u32 id, u32 val);
    /* --- party-collection accessor + per-member predicates --- */
    extern void* heroBiosGetPokemonPtr(u8* base, u16 idx);   /* idx-th party member object */
    extern u32   pokemonBiosGetFuseiFlag(u8* mon);             /* eligibility predicate A */
    extern u32   pokemonCheckValid(u8* mon);             /* eligibility predicate B */
    extern u32   menuCBRule_CheckPokemonEventFlag(u8* mon);             /* global-state gate (==1) */
    /* --- scene/object (id 0xD9) management (gs_model.c family) --- */
    extern s32   menuGetCursor(void* p);             /* present? (>=0) / -1 absent */
    extern s32   menuGetCursorFromItemID(void* p, u32 param);  /* lazy load -> handle/result */
    extern void  fn_801021F8(void* p, u32 val);    /* set visibility on subtree */
    extern u8    menuSetEnablePort(u8 mode);             /* push render mode, ret old */
    extern u32   windowGetActiveID(void);                /* current context handle */
    extern void  menuOpenCustom(void* p, u32 a, ...); /* submit/build */
    extern void* windowSearchID(s32 p);               /* resolve node by id */
    extern void* windowSearchItemID(void* head, s32 key); /* find child node by key */
    extern void  winSpriteSetDisp(void* node, u32 enable); /* enable flag on node */
    extern void  windowCheckCursor(void* p, u8 flags);   /* show/commit object */
    extern s32   menuGetLastError(void);                /* arrival/joint-count query */
    extern void  winMsgOpen(s32 a, s32 b, s32 c, s32 d); /* trigger arrival fx */
    extern void  menuClose(s32 p);               /* unload/release object */
    extern s32   windowGetValue(s32 param);           /* dest id */
    extern s32   menuGetCursorItemID(s32 p);               /* map key */

    /* --- small-data globals --- */
    extern u32 lbl_8047A410;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A428;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A42C;  /* canonical; per-site reinterpret cast */
    /* --- party base + destination table --- */
    extern u8  lbl_803A2688[]; /* party / context base block */
    extern const u8 lbl_80266E90[]; /* destination table, 12 * 0x12-byte entries */

    u8*  partyBase;
    void* mon;
    void* lastMon;
    s32  eligible;
    s32  local8;          /* frame local at 0x8(sp); menuGetCursorFromItemID result / flag */
    void* node;
    void* child;
    s32  destId;          /* windowGetValue result (treated as s32) */
    s32  mapKey;          /* menuGetCursorItemID result (table lookup key) */
    s32  resolvedMapId;   /* table-scan result, default 0 */
    s32  stateValue;
    u32  predicate;
    register const u8* ent;
    s32  e;

    local8 = 0;
    partyBase = lbl_803A2688;

    /* Re-enable the six fixed world-map menu element IDs. */
    menuItemBiosSetSelectFlag(0x1005, 0);
    menuItemBiosSetSelectFlag(0x1002, 0);
    menuItemBiosSetSelectFlag(0x1004, 0);
    menuItemBiosSetSelectFlag(0x1001, 0);
    menuItemBiosSetSelectFlag(0x1003, 0);
    menuItemBiosSetSelectFlag(0x1000, 0);

    /* Slot 0: eligibility -> menu element 0xFFF. */
    mon = heroBiosGetPokemonPtr(partyBase, 0);
    predicate = (u8)pokemonBiosGetFuseiFlag((u8*)mon);
    if (predicate > 0U) {
        eligible = 1;
    } else {
        predicate = (u8)pokemonCheckValid((u8*)mon);
        if (predicate > 0U && (u8)menuCBRule_CheckPokemonEventFlag((u8*)mon) == 1) {
            eligible = 1;
        } else {
            eligible = 0;
        }
    }
    menuItemBiosSetSelectFlag(0xFFF, (u32)eligible);

    /* Slot 1 -> menu element 0xFFC. */
    mon = heroBiosGetPokemonPtr(partyBase, 1);
    predicate = (u8)pokemonBiosGetFuseiFlag((u8*)mon);
    if (predicate > 0U) {
        eligible = 1;
    } else {
        predicate = (u8)pokemonCheckValid((u8*)mon);
        if (predicate > 0U && (u8)menuCBRule_CheckPokemonEventFlag((u8*)mon) == 1) {
            eligible = 1;
        } else {
            eligible = 0;
        }
    }
    menuItemBiosSetSelectFlag(0xFFC, (u32)eligible);

    /* Slot 2 -> menu element 0xFFE. */
    mon = heroBiosGetPokemonPtr(partyBase, 2);
    predicate = (u8)pokemonBiosGetFuseiFlag((u8*)mon);
    if (predicate > 0U) {
        eligible = 1;
    } else {
        predicate = (u8)pokemonCheckValid((u8*)mon);
        if (predicate > 0U && (u8)menuCBRule_CheckPokemonEventFlag((u8*)mon) == 1) {
            eligible = 1;
        } else {
            eligible = 0;
        }
    }
    menuItemBiosSetSelectFlag(0xFFE, (u32)eligible);

    /* Slot 3 -> menu element 0xFFB. */
    mon = heroBiosGetPokemonPtr(partyBase, 3);
    predicate = (u8)pokemonBiosGetFuseiFlag((u8*)mon);
    if (predicate > 0U) {
        eligible = 1;
    } else {
        predicate = (u8)pokemonCheckValid((u8*)mon);
        if (predicate > 0U && (u8)menuCBRule_CheckPokemonEventFlag((u8*)mon) == 1) {
            eligible = 1;
        } else {
            eligible = 0;
        }
    }
    menuItemBiosSetSelectFlag(0xFFB, (u32)eligible);

    /* Slot 4 -> menu element 0xFFD. */
    mon = heroBiosGetPokemonPtr(partyBase, 4);
    predicate = (u8)pokemonBiosGetFuseiFlag((u8*)mon);
    if (predicate > 0U) {
        eligible = 1;
    } else {
        predicate = (u8)pokemonCheckValid((u8*)mon);
        if (predicate > 0U && (u8)menuCBRule_CheckPokemonEventFlag((u8*)mon) == 1) {
            eligible = 1;
        } else {
            eligible = 0;
        }
    }
    menuItemBiosSetSelectFlag(0xFFD, (u32)eligible);

    /* Slot 5 -> menu element 0xFFA. */
    lastMon = heroBiosGetPokemonPtr(partyBase, 5);
    predicate = (u8)pokemonBiosGetFuseiFlag((u8*)lastMon);
    if (predicate > 0U) {
        eligible = 1;
    } else {
        predicate = (u8)pokemonCheckValid((u8*)lastMon);
        if (predicate > 0U && (u8)menuCBRule_CheckPokemonEventFlag((u8*)lastMon) == 1) {
            eligible = 1;
        } else {
            eligible = 0;
        }
    }
    menuItemBiosSetSelectFlag(0xFFA, (u32)eligible);

    /* Ensure destination object 0xD9 is loaded.  Lazy-load when either the
     * "already initialized" flag is set, or the object is not yet present. */
    if ((*(u8*)&lbl_8047A410) != 0 || menuGetCursor((void*)0xD9) == 0) {
        local8 = menuGetCursorFromItemID((void*)0xD9, 0xFFF);
        (*(u8*)&lbl_8047A410) = 0;
    }

    fn_801021F8((void*)0xD9, 1);
    menuSetEnablePort(2);

    /* Build/submit the object; the nonzero-local path passes &local8. */
    if (local8 != 0) {
        menuOpenCustom((void*)0xD9, windowGetActiveID(), &local8, 0, (void*)0, 0);
    } else {
        menuOpenCustom((void*)0xD9, windowGetActiveID(), (void*)0, 0, (void*)0, 0);
    }

    /* Resolve the object node and poke its child (key 0x10B2). */
    node = windowSearchID(0xD9);
    child = windowSearchItemID(node, 0x10B2);
    if (node != (void*)0 && child != (void*)0) {
        winSpriteSetDisp(child, 1);
        *(u32*)((u8*)child + 0x4C) = 0x43D9;
    }

    windowCheckCursor((void*)0xD9, 1);
    menuSetEnablePort(1);

    /* Early "already arrived" branch. */
    if (menuGetLastError() == 1) {
        winMsgOpen(2, 0x4448, 1, 0);
        menuClose(0xD9);
        (*(s32*)&lbl_8047A42C) = 0;
        return;
    }

    /* Otherwise resolve the chosen destination id. */
    destId = windowGetValue(0xD9);
    resolvedMapId = 0;

    /* Scan the 12-entry destination table for the map key returned by
     * menuGetCursorItemID(0xD9).  Each entry is 0x12 bytes: key halfword at +0x10,
     * resolved map id byte at +0x01. */
    mapKey = menuGetCursorItemID(0xD9);
    for (e = 0; e < 12; e++) {
        if (mapKey == (s32)*(u16*)(lbl_80266E90 + (e * 0x12) + 0x10)) {  /* ENDIAN-QA */
            resolvedMapId = (s32)*(u8*)(lbl_80266E90 + (e * 0x12) + 0x01);
        }
    }

    /* Special-case: travel key 0xFF9 maps to internal id 0x3E8. */
    if (menuGetCursorItemID(0xD9) == 0xFF9) {
        resolvedMapId = 0x3E8;
    }

    /* destId == -1 forces the "invalid" sentinel result. */
    stateValue = resolvedMapId;
    if (destId == -1) {
        stateValue = -1;
    }

    (*(s32*)&lbl_8047A428) = -1;

    if (stateValue == 0x3E8) {
        goto high_sentinel;
    }
    if (stateValue >= 0x3E8) {
        goto valid_destination;
    }
    switch (stateValue) {
    case -1:
        goto invalid_sentinel;
    default:
        break;
    }
    goto valid_destination;

invalid_sentinel:
    (*(s32*)&lbl_8047A42C) = 9;
    return;

high_sentinel:
    (*(s32*)&lbl_8047A42C) = 9;
    return;

valid_destination:
    (*(s32*)&lbl_8047A428) = stateValue;
    (*(s32*)&lbl_8047A42C) = 8;
}
#pragma peephole on
#else
/* fn_8002F284 - GSmap_LoadDestination (0x8002F284, 0x518 bytes)
 *
 * World-map "load destination" handler. Re-enables the six map menu item
 * slots, then re-evaluates each of the six party-member portrait slots
 * (slots 0..5) deciding whether each is grayed out, by running the same
 * 3-predicate eligibility test used in gs_npc_event.c:208-216:
 *     eligible = pokemonBiosGetFuseiFlag(mon) ||
 *                (pokemonCheckValid(mon) && menuCBRule_CheckPokemonEventFlag() == 1)
 * The per-slot result (0/1) is fed to UI dispatcher menuItemBiosSetSelectFlag under the
 * corresponding menu element ID.
 *
 * It then ensures scene/object 0xD9 is loaded (lazy-load via menuGetCursorFromItemID if
 * not already present), shows it, looks up child node 0x10B2 and pokes a tag,
 * and finally resolves the chosen travel destination either through an early
 * "already arrived" path (menuGetLastError()==1 -> sets state (*(s32*)&lbl_8047A42C)=0) or by
 * scanning the 12-entry destination table lbl_80266E90 (stride 0x12, key at
 * +0x10, map id at +0x01) and writing the resolved id / UI state code into the
 * (*(s32*)&lbl_8047A428) / (*(s32*)&lbl_8047A42C) small-data globals.
 *
 * Wrapper reads no incoming registers before first write -> takes no params.
 */
void fn_8002F284(void)
{
    /* --- UI item-enable dispatcher: menuItemBiosSetSelectFlag(u32 elementId, u32 val) --- */
    extern void menuItemBiosSetSelectFlag(u32 id, u32 val);
    /* --- party-collection accessor + per-member predicates --- */
    extern void* heroBiosGetPokemonPtr(u8* base, u16 idx);   /* idx-th party member object */
    extern u8    pokemonBiosGetFuseiFlag(u8* mon);             /* eligibility predicate A */
    extern u32   pokemonCheckValid(u8* mon);             /* eligibility predicate B */
    extern u8    menuCBRule_CheckPokemonEventFlag(void);                /* global-state gate (==1) */
    /* --- scene/object (id 0xD9) management (gs_model.c family) --- */
    extern s32   menuGetCursor(void* p);             /* present? (>=0) / -1 absent */
    extern s32   menuGetCursorFromItemID(void* p, u32 param);  /* lazy load -> handle/result */
    extern void  fn_801021F8(void* p, u32 val);    /* set visibility on subtree */
    extern u8    menuSetEnablePort(u8 mode);             /* push render mode, ret old */
    extern u32   windowGetActiveID(void);                /* current context handle */
    extern void  menuOpenCustom(void* p, u32 a, void* b, s32 c, void* d, s32 e); /* submit/build */
    extern void* windowSearchID(s32 p);               /* resolve node by id */
    extern void* windowSearchItemID(void* head, s32 key); /* find child node by key */
    extern void  winSpriteSetDisp(void* node, u32 enable); /* enable flag on node */
    extern void  windowCheckCursor(void* p, u8 flags);   /* show/commit object */
    extern s32   menuGetLastError(void);                /* arrival/joint-count query */
    extern void  winMsgOpen(s32 a, s32 b, s32 c, s32 d); /* trigger arrival fx */
    extern void  menuClose(s32 p);               /* unload/release object */
    extern void* windowGetValue(s32 param);           /* (here used as s32 dest id) */
    extern void* menuGetCursorItemID(void* p, u32 target); /* (here used as s32 map key) */

    /* --- small-data globals --- */
    extern u32 lbl_8047A410;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A428;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A42C;  /* canonical; per-site reinterpret cast */
    /* --- party base + destination table --- */
    extern u8  lbl_803A2688[]; /* party / context base block */
    extern u8  lbl_80266E90[]; /* destination table, 12 * 0x12-byte entries */

    u8*  partyBase;
    void* mon;
    s32  eligible;
    s32  i;
    s32  local8;          /* frame local at 0x8(sp); menuGetCursorFromItemID result / flag */
    void* node;
    void* child;
    s32  destId;          /* windowGetValue result (treated as s32) */
    s32  mapKey;          /* menuGetCursorItemID result (table lookup key) */
    s32  resolvedMapId;   /* table-scan result, default 0 */
    u8*  ent;
    s32  e;

    partyBase = lbl_803A2688;
    local8 = 0;

    /* Re-enable the six fixed world-map menu element IDs. */
    menuItemBiosSetSelectFlag(0x1005, 0);
    menuItemBiosSetSelectFlag(0x1002, 0);
    menuItemBiosSetSelectFlag(0x1004, 0);
    menuItemBiosSetSelectFlag(0x1001, 0);
    menuItemBiosSetSelectFlag(0x1003, 0);
    menuItemBiosSetSelectFlag(0x1000, 0);

    /* Slot 0: eligibility -> menu element 0xFFF. */
    mon = heroBiosGetPokemonPtr(partyBase, 0);
    if (pokemonBiosGetFuseiFlag((u8*)mon) != 0) {
        eligible = 1;
    } else if (pokemonCheckValid((u8*)mon) != 0 && menuCBRule_CheckPokemonEventFlag() == 1) {
        eligible = 1;
    } else {
        eligible = 0;
    }
    menuItemBiosSetSelectFlag(0xFFF, (u32)eligible);

    /* Slot 1 -> menu element 0xFFC. */
    mon = heroBiosGetPokemonPtr(partyBase, 1);
    if (pokemonBiosGetFuseiFlag((u8*)mon) != 0) {
        eligible = 1;
    } else if (pokemonCheckValid((u8*)mon) != 0 && menuCBRule_CheckPokemonEventFlag() == 1) {
        eligible = 1;
    } else {
        eligible = 0;
    }
    menuItemBiosSetSelectFlag(0xFFC, (u32)eligible);

    /* Slot 2 -> menu element 0xFFE. */
    mon = heroBiosGetPokemonPtr(partyBase, 2);
    if (pokemonBiosGetFuseiFlag((u8*)mon) != 0) {
        eligible = 1;
    } else if (pokemonCheckValid((u8*)mon) != 0 && menuCBRule_CheckPokemonEventFlag() == 1) {
        eligible = 1;
    } else {
        eligible = 0;
    }
    menuItemBiosSetSelectFlag(0xFFE, (u32)eligible);

    /* Slot 3 -> menu element 0xFFB. */
    mon = heroBiosGetPokemonPtr(partyBase, 3);
    if (pokemonBiosGetFuseiFlag((u8*)mon) != 0) {
        eligible = 1;
    } else if (pokemonCheckValid((u8*)mon) != 0 && menuCBRule_CheckPokemonEventFlag() == 1) {
        eligible = 1;
    } else {
        eligible = 0;
    }
    menuItemBiosSetSelectFlag(0xFFB, (u32)eligible);

    /* Slot 4 -> menu element 0xFFD. */
    mon = heroBiosGetPokemonPtr(partyBase, 4);
    if (pokemonBiosGetFuseiFlag((u8*)mon) != 0) {
        eligible = 1;
    } else if (pokemonCheckValid((u8*)mon) != 0 && menuCBRule_CheckPokemonEventFlag() == 1) {
        eligible = 1;
    } else {
        eligible = 0;
    }
    menuItemBiosSetSelectFlag(0xFFD, (u32)eligible);

    /* Slot 5 -> menu element 0xFFA. */
    mon = heroBiosGetPokemonPtr(partyBase, 5);
    if (pokemonBiosGetFuseiFlag((u8*)mon) != 0) {
        eligible = 1;
    } else if (pokemonCheckValid((u8*)mon) != 0 && menuCBRule_CheckPokemonEventFlag() == 1) {
        eligible = 1;
    } else {
        eligible = 0;
    }
    menuItemBiosSetSelectFlag(0xFFA, (u32)eligible);

    /* Ensure destination object 0xD9 is loaded.  Lazy-load when either the
     * "already initialized" flag is set, or the object is not yet present. */
    if ((*(u8*)&lbl_8047A410) != 0 || menuGetCursor((void*)0xD9) == 0) {
        local8 = menuGetCursorFromItemID((void*)0xD9, 0xFFF);
        (*(u8*)&lbl_8047A410) = 0;
    }

    fn_801021F8((void*)0xD9, 1);
    menuSetEnablePort(2);

    /* Build/submit the object; the nonzero-local path passes &local8. */
    if (local8 != 0) {
        menuOpenCustom((void*)0xD9, windowGetActiveID(), &local8, 0, (void*)0, 0);
    } else {
        menuOpenCustom((void*)0xD9, windowGetActiveID(), (void*)0, 0, (void*)0, 0);
    }

    /* Resolve the object node and poke its child (key 0x10B2). */
    node = windowSearchID(0xD9);
    child = windowSearchItemID(node, 0x10B2);
    if (node != (void*)0 && child != (void*)0) {
        winSpriteSetDisp(child, 1);
        *(u32*)((u8*)child + 0x4C) = 0x43D9;
    }

    windowCheckCursor((void*)0xD9, 1);
    menuSetEnablePort(1);

    /* Early "already arrived" branch. */
    if (menuGetLastError() == 1) {
        winMsgOpen(2, 0x4448, 1, 0);
        menuClose(0xD9);
        (*(s32*)&lbl_8047A42C) = 0;
        return;
    }

    /* Otherwise resolve the chosen destination id. */
    destId = (s32)windowGetValue(0xD9);
    resolvedMapId = 0;

    /* Scan the 12-entry destination table for the map key returned by
     * menuGetCursorItemID(0xD9).  Each entry is 0x12 bytes: key halfword at +0x10,
     * resolved map id byte at +0x01. */
    mapKey = (s32)menuGetCursorItemID((void*)0xD9, 0);  /* selector 0 (li r5,0) */
    ent = lbl_80266E90;
    for (e = 0; e < 12; e++) {
        if (mapKey == (s32)*(s16*)(ent + 0x10)) {  /* ENDIAN-QA */
            resolvedMapId = (s32)*(u8*)(ent + 0x01);
        }
        ent += 0x12;
    }

    /* Special-case: travel key 0xFF9 maps to internal id 0x3E8. */
    if (menuGetCursorItemID((void*)0xD9, 0) == (void*)0xFF9) {
        resolvedMapId = 0x3E8;
    }

    /* destId == -1 forces the "invalid" sentinel result. */
    if (destId == -1) {
        resolvedMapId = -1;
    }

    (*(s32*)&lbl_8047A428) = -1;

    if (resolvedMapId == 0x3E8) {
        /* High sentinel: "arrived elsewhere" UI state. */
        (*(s32*)&lbl_8047A42C) = 9;
    } else if (resolvedMapId > 0x3E8) {
        /* Out-of-range high -> normal destination state, store id. */
        (*(s32*)&lbl_8047A428) = resolvedMapId;
        (*(s32*)&lbl_8047A42C) = 8;
    } else if (resolvedMapId == -1) {
        /* Invalid -> same UI state as 0x3E8 sentinel. */
        (*(s32*)&lbl_8047A42C) = 9;
    } else {
        /* Valid in-range destination -> store id, normal state. */
        (*(s32*)&lbl_8047A428) = resolvedMapId;
        (*(s32*)&lbl_8047A42C) = 8;
    }
}
#endif
#endif

#if !defined(MENU_POKEMON_CHANGE_EXACT_ISLAND) || defined(MENU_POKEMON_CHANGE_EXACT_8002F79C_ONLY)

/* 0x8002F79C | size: 0x4BC
 * Picks the selected Pokemon of the partner's party (the save's party,
 * slot lbl_8047A428) to receive, refusing it when its held item cannot be
 * traded, when it is a Shadow Pokemon, or when no other Pokemon would stay
 * behind. */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_8002F79C(void) {
    extern u32 lbl_8047A428;
    extern u32 lbl_8047A42C;
    extern u32 lbl_8047A410;
    extern u32 lbl_8047A424;
    extern void* savedataGetStatus(s32 side, s32 slotType);
    extern void* heroGetStatus(void* hero, u32 selector, u16 index);
    extern u16 pokemonBiosGetItemDataId(void* pokemon);
    extern void* itemDataBiosGetPtr(u16 id);
    extern u8 itemDataBiosCheckExportable(void* item);
    extern void* pokemonBiosGetNicknamePtr(void* pokemon);
    extern void msgctrlSetValue(s32 id, void* value);
    extern void fn_801021F8(s32 id, s32 flag);
    extern u8 pokemonBiosGetDarkFlag(void* pokemon);
    void* pokemon;
    void* party;
    u16 itemId;
    u8 exportable;
    void* name;

    party = savedataGetStatus(0, 2);
    pokemon = heroGetStatus(NULL, 3, lbl_8047A428);
    itemId = pokemonBiosGetItemDataId(pokemon);
    if (itemId != 0) {
        exportable = itemDataBiosCheckExportable(itemDataBiosGetPtr(itemId));
    } else {
        exportable = 1;
    }
    if (exportable == 0) {
        name = pokemonBiosGetNicknamePtr(pokemon);
        msgctrlSetValue(0x32, name);
        menuPokemonChangeAlert(0x43DD);
        lbl_8047A42C = 2;
        return;
    }
    if (pokemonBiosGetDarkFlag(pokemon) == 1) {
        fn_801021F8(0xD9, 0);
        msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(pokemon));
        menuPokemonChangeAlert(0x43DF);
        fn_801021F8(0xD9, 1);
        lbl_8047A42C = 2;
        return;
    }
    if (menuPokemonChangeHasOther(party, lbl_8047A428) == 0) {
        fn_801021F8(0xD9, 0);
        msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(pokemon));
        menuPokemonChangeAlert(0x44E8);
        fn_801021F8(0xD9, 1);
        lbl_8047A42C = 2;
        return;
    }
    lbl_8047A424 = lbl_8047A428;
    *(u8*)&lbl_8047A410 = 1;
    lbl_8047A42C = 7;
}
#pragma pop

#endif /* !MENU_POKEMON_CHANGE_EXACT_ISLAND */
