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
#if 0
asm void stateFunctionSaveReport(void) {
#include "src/game/gs_worldmap_fn_8002DD24.inc"
}
#else
/*
 * stateFunctionSaveReport  GSmap_CheckStoryState  0x8002DD24 | size: 0x1EC
 *
 * Checks whether a GBA encounter/trade handshake is ready, runs the
 * associated scene UI loop, and either commits the save-data block (fast
 * path) or plays a short delay sequence (slow path / confirmed trade).
 *
 * Parameters
 *   arg  - pointer to a caller-owned save-data buffer that is block-copied
 *           into the game's live save region on the fast path.
 */
void stateFunctionSaveReport(void *arg)
{
    /* ---- block-scope externs (TU convention) ---- */
    extern u8  lbl_803A2518[];          /* base of live save-data array */
    extern u32 lbl_8047A424;            /* slot-A index (u16 used) */
    extern u32 lbl_8047A420;            /* slot-B index (u16 used) */
    extern u32 lbl_8047A40C;            /* encounter flags */
    extern u32 lbl_804788B0;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A42C;            /* worldmap state machine token */

    /* Interaction getter: heroGetStatus(u8* base, u32 selector, u32 idx) */
    extern void *heroGetStatus(u8 *ptr, u32 selector, u32 idx);

    /* Message/dialog trigger:
       winMsgOpen(s32 slot, s32 msgId, s32 p3, s32 p4)             */
    extern void winMsgOpen(s32 slot, s32 msgId, s32 p3, s32 p4);

    /* GBA encounter / state checkers */
    extern s32  fn_80089E20(s32 mode, void *pkm, u32 slotB, u32 flags);
    extern s32  fn_80089D98(s32 slot);

    /* Scene model-pool helpers */
    extern void fn_801D055C(s32 a, s32 b, s32 c); /* batch update / open */
    extern u8 fn_801D04D0(void);                    /* pool poll */
    extern void fn_801D046C(s32 flag);              /* pool set-count flag  */
    extern s32 memcardGetTaskResult(void);           /* pool event result */
    extern void fn_801D039C(void);                  /* pool step update */

    /* Threading / render helpers */
    extern void _threadSwitch(void);   /* GSthread yield (one frame) */
    extern void gbaCommandSetKeyState(s32 mode, s32 flag); /* abort / cancel scene */

    /* Save-data helpers */
    extern void *savedataGetStatus(s32 side, s32 slotType); /* get party/save ptr */
    extern void  menuSubKeyWait(void);                    /* save commit flush  */
    extern void  winMsgClose(s32 slot);                /* save slot finalize */
    extern void  fn_8010A420(u8 *ptr);                 /* archive release    */

    /* Timer helper: _fadeEffectGetRandom__FUl(s32 frames) -> u32 countdown value */
    extern u32   _fadeEffectGetRandom__FUl(s32 frames);

    /* ---- locals ---- */
    u8   *save_base;   /* r30: base of live save-data */
    void *pkm_b;       /* r31: Pokémon-B object pointer */
    u8    did_action;  /* r29: flag – GBA action was processed this loop */
    u8    confirmed;   /* r28: flag – trade/save was confirmed (result==4) */
    s32   event_result;/* r31 reused: event poll return value */
    u32   timer;       /* r28 reused: countdown for delay path */

    save_base = lbl_803A2518;

    /* Warm up the interaction table for slot-A (result not used here) */
    heroGetStatus(NULL, 3, (u32)(u16)lbl_8047A424);

    /* Get the Pokémon object for slot-B */
    pkm_b = heroGetStatus(save_base + 0x170, 3, (u32)(u16)lbl_8047A420);

    did_action = 0;
    confirmed  = 0;

    /* Notify the message system that a new encounter dialog is starting */
    winMsgOpen(2, 0x44d7, 1, 1);

    /* Check whether the GBA encounter is already ready */
    if (fn_80089E20(2, pkm_b, lbl_8047A420, lbl_8047A40C) != 0) {
        /* Encounter already in progress – jump straight to the path
           decision (same as falling off the loop with did_action==0,
           confirmed==0).  We skip the scene open and loop entirely. */
        goto L_path_decision;
    }

    /* Open the waiting scene */
    fn_801D055C(8, 2, 0);

    /* ---- yield loop: wait for the GBA event result ---- */
L_loop:
    if (!did_action) {
        /* Poll scene pool for a pending entry */
        s32 pool_entry = fn_801D04D0();
        if (pool_entry != 0) {
            /* Check GBA slot-2 state */
            s32 gba_state = fn_80089D98(2);
            if (gba_state >= 0) {
                if (gba_state == 0) {
                    fn_801D046C(0);
                } else {
                    fn_801D046C(1);
                }
                did_action = 1;
            }
        }
    }

    /* Yield one frame, then poll the scene for an event result */
    _threadSwitch();
    event_result = memcardGetTaskResult();
    if (event_result == 0) {
        goto L_loop;
    }

    /* Scene is done – tear down and check the result code */
    gbaCommandSetKeyState(2, 1);
    fn_801D039C();

    if (event_result == 4) {
        confirmed = 1;
    }

L_path_decision:
    /* If the GBA action was NOT processed (did_action==0) OR if it was
       processed AND confirmed (did_action!=0 && confirmed!=0): copy save. */
    if (!did_action || confirmed) {
        goto L_save_copy;
    }
    /* did_action && !confirmed → delay path */
    goto L_delay;

L_save_copy:
    {
        /* Block-copy the caller's buffer into the live save region.
         * savedataGetStatus(0,0) returns a pointer to the destination; the
         * source is the 'arg' parameter passed to this function.
         * The CW loop copies 2 words × 15354 iterations = 122832 bytes.
         * We use a plain memcpy equivalent for x86 semantics.
         */
        u8 *dst = (u8 *)savedataGetStatus(0, 0);
        u8 *src = (u8 *)arg;
        u32 i;
        for (i = 0; i < 15354; i++) {
            /* Each bdnz iteration copies two words (8 bytes) */
            ((u32 *)dst)[0] = ((u32 *)src)[0];
            ((u32 *)dst)[1] = ((u32 *)src)[1];
            dst += 8;
            src += 8;
        }

        winMsgOpen(2, 0x44d6, 1, 0);
        menuSubKeyWait();
        winMsgClose(1);
        fn_8010A420(save_base + 0xd18);
        fn_8010A420(save_base + 0xcd0);
        (*(u8*)&lbl_804788B0) = 0;
        lbl_8047A42C = 0;
        return;
    }

L_delay:
    {
        /* Play a ~60-frame delay sequence then signal a different outcome */
        timer = _fadeEffectGetRandom__FUl(0x3c);
        while (timer != 0) {
            _threadSwitch();
            timer--;
        }
        winMsgOpen(2, 0x44d5, 1, 0);
        menuSubKeyWait();
        winMsgClose(1);
        lbl_8047A42C = 0x13;
    }
}
#endif

/* stateFunctionEvolution - 0x8002DF10 | size: 0x35c */
extern void pokemonEvolutionCheck(void);
extern void fn_801CB9D8(void);
extern void fn_80112260(void);
extern void pokemonEvolutionAll(void);
extern void cameraPlayAnime(void);
extern void fn_80113F48(void);
extern void fn_801CBA0C(void);
extern void GSresGetResource(void);
extern void GSscene_SetMode(void);
extern void GSmodelSetVisibility(void);
extern u32 lbl_8047A424;
extern u32 lbl_8047A40C;
extern u32 lbl_8047A420;
extern f32 lbl_8047B9D0;
extern u32 lbl_8047A41C;
extern u32 lbl_8047A418;
extern f32 lbl_8047B9D4;
extern u32 lbl_8047A408;
extern u32 lbl_8047A414;
extern u32 lbl_8047A42C;
#if 0
asm void stateFunctionEvolution(void) {
#include "src/game/gs_worldmap_fn_8002DF10.inc"
}
#else

/* Build and run the Pokemon-change selection menu. */
void fn_8002FC58(void)
{
    typedef struct PokemonChangeMenuEntry {
        u8 _00;
        u8 slot;
        u8 _02[0x0E];
        u16 itemId;
    } PokemonChangeMenuEntry;
    extern void* savedataGetStatus();
    extern void* heroBiosGetPokemonPtr();
    extern u8 pokemonCheckValid();
    extern u8 menuCBRule_CheckPokemonEventFlag();
    extern void menuItemBiosSetSelectFlag();
    extern void fn_80030170();
    extern void fn_8010B01C();
    extern s32 menuGetCursor();
    extern s32 menuGetCursorFromItemID();
    extern void fn_801021F8();
    extern s32 windowGetActiveID();
    extern void menuOpenCustom(s32, ...);
    extern void* windowSearchID();
    extern void* windowSearchItemID();
    extern void winSpriteSetDisp();
    extern void windowCheckCursor();
    extern s32 windowGetValue();
    extern s32 menuGetCursorItemID();
    extern u8 lbl_803A2650[];
    extern u8 lbl_803A2688[];
    extern PokemonChangeMenuEntry lbl_80266E90[];
    extern u8 lbl_8047A410;
    extern u32 lbl_8047A428;
    extern u32 lbl_8047A42C;

    void* party;
    void* pokemon;
    void* window;
    void* sprite;
    u32* eligible;
    s32 cursor;
    s32 value;
    s32 selected;
    s32 count;
    s32 i;
    u8 enabled;

    cursor = 0;
    count = 0;
    party = savedataGetStatus(0, 2);
    eligible = (u32*)lbl_803A2650;
    for (i = 0; i < 6; i++) {
        pokemon = heroBiosGetPokemonPtr(party, (u16)i);
        if (pokemonCheckValid(pokemon) != 0 &&
            menuCBRule_CheckPokemonEventFlag(pokemon) == 1) {
            eligible[count++] = (u32)pokemon;
        }
    }

    party = lbl_803A2688;
    for (i = 0; i < 6; i++) {
        pokemon = heroBiosGetPokemonPtr(party, (u16)i);
        if (pokemonCheckValid(pokemon) != 0 &&
            menuCBRule_CheckPokemonEventFlag(pokemon) == 1) {
            eligible[count++] = (u32)pokemon;
        }
    }
    eligible[count] = 0;
    eligible[13] = 0;

    fn_8010B01C(0, fn_80030170);
    party = savedataGetStatus(0, 2);

#define SET_PARTY_ITEM(slotIndex, item)                                      \
    do {                                                                     \
        pokemon = heroBiosGetPokemonPtr(party, (slotIndex));                 \
        enabled = 0;                                                         \
        if (pokemonCheckValid(pokemon) != 0 &&                               \
            menuCBRule_CheckPokemonEventFlag(pokemon) == 1) {                \
            enabled = 1;                                                     \
        }                                                                    \
        menuItemBiosSetSelectFlag((item), enabled);                          \
    } while (0)

    SET_PARTY_ITEM(0, 0x1005);
    SET_PARTY_ITEM(1, 0x1002);
    SET_PARTY_ITEM(2, 0x1004);
    SET_PARTY_ITEM(3, 0x1001);
    SET_PARTY_ITEM(4, 0x1003);
    SET_PARTY_ITEM(5, 0x1000);
#undef SET_PARTY_ITEM

    menuItemBiosSetSelectFlag(0x0FFF, 0);
    menuItemBiosSetSelectFlag(0x0FFC, 0);
    menuItemBiosSetSelectFlag(0x0FFE, 0);
    menuItemBiosSetSelectFlag(0x0FFB, 0);
    menuItemBiosSetSelectFlag(0x0FFD, 0);
    menuItemBiosSetSelectFlag(0x0FFA, 0);

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

    window = windowSearchID(0xD9);
    sprite = windowSearchItemID(window, 0x10B2);
    if (window != 0 && sprite != 0) {
        winSpriteSetDisp(sprite, 1);
        *(u32*)((u8*)sprite + 0x4C) = 0x43D9;
    }

    windowCheckCursor(0xD9, 1);
    value = windowGetValue(0xD9);
    selected = 0;
    cursor = menuGetCursorItemID(0xD9);
    for (i = 0; i < 12; i++) {
        if (cursor == lbl_80266E90[i].itemId) {
            selected = lbl_80266E90[i].slot;
        }
    }
    if (menuGetCursorItemID(0xD9) == 0x0FF9) {
        selected = 1000;
    }
    if (value == -1) {
        selected = -1;
    }

    lbl_8047A428 = -1;
    switch (selected) {
    case -1:
    case 1000:
        lbl_8047A42C = 4;
        break;
    default:
        lbl_8047A428 = selected;
        lbl_8047A42C = 3;
        break;
    }
}

/*
 * stateFunctionEvolution  GSmap_UpdateAvailability  0x8002DF10  size:0x35c
 *
 * No-arg world-map state machine: checks whether the two active NPC slots
 * (lbl_8047A424, lbl_8047A420) are in a valid encounter state, and if so
 * runs the full encounter setup sequence:
 *   1. Fade/audio transition (fadeSet/fadeCheck)
 *   2. Hide marker objects (fn_8010A420)
 *   3. Wait for trainer anim (fn_801CB9D8)
 *   4. Yield one frame (_threadSwitch)
 *   5. Spawn/configure encounter objects (pokemonEvolutionAll)
 *   6. Re-anchor NPC handles (fn_80109C88, menuModelInit)
 *   7. Scene-load BGM/scene (cameraPlayAnime, fn_801CBA0C, GSresGetResource)
 *   8. Restore people state (GSscene_SetMode)
 *   9. Mark availability flag and advance state (lbl_8047A42C = 0x12)
 */
#pragma peephole off
void stateFunctionEvolution(void)
{
    extern u32   lbl_8047A424;
    extern u32   lbl_8047A420;
    extern u32   lbl_8047A40C;
    extern u8    lbl_803A2518[];
    extern f32   lbl_8047B9D0;
    extern f32   lbl_8047B9D4;
    extern u32 lbl_8047A41C;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A408;  /* canonical; per-site reinterpret cast */
    extern u32   lbl_8047A418;
    extern u32   lbl_8047A414;
    extern u32   lbl_8047A42C;

    /* heroGetStatus - interaction getter:  (u8* base, u32 mode, u16 idx) -> u8* NPC handle */
    extern u8*  heroGetStatus(u8 *ptr, u32 selector, u32 idx);
    /* pokemonEvolutionCheck - get NPC key/type at location:
         (u8* world, u32 mode, u16 key, u16* key_out, u8* type_out) -> u32 npc_handle (0/0xffff=invalid) */
    extern u32  pokemonEvolutionCheck(u8 *arg0, u32 arg1, u16 arg2, u16 *arg3, u8 *arg4);
    /* pokemonBiosGetPokemonDataId - get Pokemon location / status field from NPC ptr */
    extern u16  pokemonBiosGetPokemonDataId(void *ptr);
    /* fadeSet - BGM fade  (f32 vol, s32 mode) */
    extern void fadeSet(f32 vol, s32 mode);
    /* fadeCheck - BGM enable  (s32 flag) */
    extern void fadeCheck(s32 flag);
    /* menuClose - menu show  (s32 id) */
    extern void menuClose(s32 id);
    /* fn_8010A420 - model marker hide  (u8* obj) */
    extern void fn_8010A420(u8 *obj);
    /* fn_801CB9D8 - scene trainer anim state  (u32 handle) */
    extern void fn_801CB9D8(u32 handle);
    /* fn_80112260 - field collision query reset  (s32 flag) */
    extern void fn_80112260(s32 flag);
    /* _threadSwitch - GS vsync yield */
    extern void _threadSwitch(void);
    /* pokemonEvolutionAll - encounter trigger dispatcher */
    extern s32 pokemonEvolutionAll(u8 *world, u32 npc, u16 key, u8 *type_out, u8 *team, s32 memo, s32 arg6, s32 audio);
    /* menuModelInit - model set bounds (u8* obj, s32 w, s32 h) */
    extern void menuModelInit(u8 *obj, s32 w, s32 h);
    /* fn_80109C88 - model set NPC handle (u8* obj, u8* npc_handle) */
    extern void fn_80109C88(u8 *obj, u8 *npc_handle);
    /* cameraPlayAnime - scene render/BGM start (s32 scene_id, u32 color_key, s32 a, s32 b) */
    extern void cameraPlayAnime(s32 scene_id, u32 color_key, s32 a, s32 b);
    /* fn_80113F48 - get current scene/resource handle */
    extern u32  fn_80113F48(void);
    /* fn_801CBA0C - scene transition fade-out: (u32 color_key) -> u32 fade_handle */
    extern u32  fn_801CBA0C(u32 color_key);
    /* GSresGetResource - resolve resource pointer from handle (u32 handle) -> void* */
    extern void* GSresGetResource(u32 handle);
    /* GSscene_SetMode - restore people state (s32 mode) */
    extern void GSscene_SetMode(s32 mode);
    /* GSmodelSetVisibility - enable/disable field object (void* obj, s32 flag) */
    extern void GSmodelSetVisibility(void *obj, s32 flag);
    /* menuOpen - menu close sync (s32 id, s32 flag) -> s32 */
    extern s32  menuOpen(s32 id, s32 flag);

    u8  *base;          /* r31: lbl_803A2518 base pointer */
    u8  *npc_a;         /* r28: NPC handle for slot A (lbl_8047A424) */
    u8  *npc_b;         /* r30: NPC handle for slot B (lbl_8047A420) */
    u8   need_update;   /* r29: set to 1 if either slot is valid */
    u32  scene_handle;  /* r28 reused: result of fn_80113F48 */
    u32  npc_result;    /* return of pokemonEvolutionCheck (cast to u16 for validity check) */
    s32  enc_result;    /* return of pokemonEvolutionAll */

    /* Stack temporaries for pokemonEvolutionCheck and pokemonEvolutionAll output buffers */
    u16  key_a;         /* sp+0xc: key output for initial A-slot check */
    u8   type_a;        /* sp+0x18: type output for initial A-slot check */
    u16  key_b;         /* -- sp+0xc reused for B-slot check */
    /* sp+0xa / sp+0x14 for encounter call buffers */
    u16  enc_key_a;     /* sp+0xa */
    u8   enc_type_a;    /* sp+0x14 */
    u16  enc_key_b;     /* sp+0x8 */
    u8   enc_type_b;    /* sp+0x10 */

    base = lbl_803A2518;
    lbl_8047A40C = 0;
    need_update = 0;

    /* Check slot A (lbl_8047A424): get NPC handle at index 3 */
    npc_a = heroGetStatus(NULL, 3, (u16)lbl_8047A424);
    /* Check slot B (lbl_8047A420): get NPC handle at index 3 from base+0x170 */
    npc_b = heroGetStatus(base + 0x170, 3, (u16)lbl_8047A420);

    /* Check slot A availability */
    npc_result = pokemonEvolutionCheck(npc_a, 2, 0, &key_a, &type_a);
    if ((u16)npc_result != 0 && (u16)npc_result != 0xffff) {
        need_update = 1;
    }

    /* Check slot B availability */
    npc_result = pokemonEvolutionCheck(npc_b, 2, 0, &key_a, &type_a); /* reuses same stack slots */
    if ((u16)npc_result != 0 && (u16)npc_result != 0xffff) {
        need_update = 1;
        /* Also update lbl_8047A40C from slot B's status field */
        lbl_8047A40C = (u16)pokemonBiosGetPokemonDataId(npc_b);
    }

    if (need_update != 1) {
        /* No valid encounters -- just advance state counter */
        lbl_8047A42C = 0x12;
        return;
    }

    /* --- Valid encounter found: run full setup sequence --- */

    (*(u8*)&lbl_8047A41C) = 0;

    fadeSet(lbl_8047B9D0, 3);
    fadeCheck(1);

    menuClose(0xde);

    fn_8010A420(base + 0xd18);
    fn_8010A420(base + 0xcd0);

    fn_801CB9D8(lbl_8047A418);

    fn_80112260(0);
    _threadSwitch();

    fadeSet(lbl_8047B9D4, 2);
    fadeCheck(1);

    /* --- Slot A encounter trigger --- */
    npc_result = pokemonEvolutionCheck(npc_a, 2, 0, &enc_key_a, &enc_type_a);
    if ((u16)npc_result != 0 && (u16)npc_result != 0xffff) {
        (*(u8*)&lbl_8047A408) = 1;
        enc_result = pokemonEvolutionAll(npc_a, (u32)(u16)npc_result, 0,
                                  &enc_type_a, NULL, 1, 0, 1);
        if (enc_result == 2) {
            (*(u8*)&lbl_8047A408) = 0;
        }
        /* Unconditional clear (compiler-emitted redundant store) */
        (*(u8*)&lbl_8047A408) = 0;
    }

    /* --- Slot B encounter trigger --- */
    npc_result = pokemonEvolutionCheck(npc_b, 2, 0, &enc_key_b, &enc_type_b);
    if ((u16)npc_result != 0 && (u16)npc_result != 0xffff) {
        (*(u8*)&lbl_8047A408) = 1;
        enc_result = pokemonEvolutionAll(npc_b, (u32)(u16)npc_result, 0,
                                  &enc_type_b, NULL, 0, 0, 1);
        if (enc_result == 2) {
            (*(u8*)&lbl_8047A408) = 0;
        }
        /* Unconditional clear */
        (*(u8*)&lbl_8047A408) = 0;
    }

    /* --- Post-encounter: fade back in and re-anchor scene objects --- */

    fadeSet(lbl_8047B9D4, 3);
    fadeCheck(1);

    /* Refresh NPC handles after encounter (slots may have changed) */
    npc_b = heroGetStatus(NULL, 3, (u16)lbl_8047A424);          /* r30 */
    /* FUNCTIONAL-TODO: asm re-uses r29 for slot-B after refresh */
    {
        u8 *npc_b2 = heroGetStatus(base + 0x170, 3, (u16)lbl_8047A420); /* r29 */

        menuModelInit(base + 0xd18, 0xe8, 0x11c);
        menuModelInit(base + 0xcd0, 0xe8, 0x11c);

        fn_80109C88(base + 0xd18, npc_b);
        fn_80109C88(base + 0xcd0, npc_b2);
    }

    cameraPlayAnime(0x37c, 0x0fff1800, 0, 1);

    scene_handle = fn_80113F48();
    lbl_8047A418 = fn_801CBA0C(0x0ffe1000);
    lbl_8047A414 = (u32)GSresGetResource(scene_handle);

    cameraPlayAnime(0x37c, 0x0fff1800, 0, 1);

    GSscene_SetMode(4);

    GSmodelSetVisibility((void*)lbl_8047A414, 1);

    fn_80112260(0);

    menuOpen(0xde, 1);

    fadeSet(lbl_8047B9D0, 2);
    fadeCheck(1);

    (*(u8*)&lbl_8047A41C) = 1;

    /* Fall through to state advance */
    lbl_8047A42C = 0x12;
}
#pragma peephole reset
#endif

/* stateFunctionExChangeMain - 0x8002E26C | size: 0x1f4 */
extern void pokemonInit(void);
extern void pokemonBiosCopy(void);
extern void fn_801024E8(void);
extern void fn_801CB834(void);
extern void fn_80176B48(void);
extern u32 lbl_8047A424;
extern u32 lbl_8047A420;
extern u32 lbl_8047A414;
extern u32 lbl_8047A41C;
extern f32 lbl_8047B9D0;
extern f32 lbl_8047B9D8;
extern u32 lbl_8047A42C;
#if 0
asm void stateFunctionExChangeMain(void) {
#include "src/game/gs_worldmap_fn_8002E26C.inc"
}
#else
/*
 * stateFunctionExChangeMain - GSmap_RefreshDisplay
 * 0x8002E26C | size: 0x1f4
 *
 * Refreshes the world-map display: sets up interaction objects, plays UI
 * sounds, initiates camera animations, shows/hides the map UI layer, loads
 * the trainer model, and writes the next state-machine step into lbl_8047A42C.
 *
 * No parameters (no r3..r10 reads before first write).
 */
void stateFunctionExChangeMain(void)
{
    /* --- block-scope externs (TU convention) --- */
    extern u8  lbl_803A2518[];            /* worldmap context base (BSS)          */
    extern u32 lbl_8047A424;              /* slot/index A (r13-relative)           */
    extern u32 lbl_8047A420;              /* slot/index B (r13-relative)           */
    extern u32 lbl_8047A414;             /* render handle (r13-relative)          */
    extern u32 lbl_8047A41C;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A42C;             /* state-machine step (r13-relative)     */
    extern f32 lbl_8047B9D0;             /* camera duration constant 0 (r2-rel)   */
    extern f32 lbl_8047B9D8;             /* camera duration constant 1 (r2-rel)   */

    /* interaction/sound helpers */
    extern void pokemonInit(u8 *ctx);
    extern void pokemonBiosCopy(void *a, void *b);
    extern void menuClose(s32 sound_id);
    extern void fn_8010A420(void *widget);
    extern void GSmodelSetVisibility(u32 handle, s32 flag);
    extern void fadeSet(f32 duration, s32 mode);
    extern void fadeCheck(s32 flag);
    extern void fn_801024E8(s32 arg);
    extern u32  fn_80113F48(void);
    extern void cameraPlayAnime(s32 handle, u32 flags, s32 a3, s32 a4);
    extern void _threadSwitch(void);
    extern void fn_80166AB8(s32 a1, s32 a2, s32 a3);
    extern void fn_80112260(s32 flag);
    extern void fn_801CB834(u32 a1, s32 a2, s32 a3, s32 a4);
    extern void cameraWaitSyncAnime(s32 arg);
    extern void menuModelInit(void *widget, s32 x, s32 y);
    extern void fn_80109C88(void *widget, void *obj);
    extern void menuOpen(s32 id, s32 flag);
    extern u32  heroGetStatus(u8 *ptr, u32 selector, u32 idx);

    u8  *base  = lbl_803A2518;
    void *obj_a;   /* r30: result of first  heroGetStatus */
    void *obj_b;   /* r29: result of second heroGetStatus */
    u32   handle;  /* r3 after fn_80113F48              */

    /* --- Initialise interaction objects --- */
    pokemonInit(base);

    obj_a = (void *)heroGetStatus((u8 *)0, 3, (u16)lbl_8047A424);
    obj_b = (void *)heroGetStatus(base + 0x170, 3, (u16)lbl_8047A420);

    /* Cross-link the three objects */
    pokemonBiosCopy(base,  obj_b);
    pokemonBiosCopy(obj_b, obj_a);
    pokemonBiosCopy(obj_a, base);

    /* --- Play worldmap entry sound, hide UI widgets --- */
    menuClose(0xde);
    fn_8010A420(base + 0xd18);
    fn_8010A420(base + 0xcd0);

    /* --- Clear display-active flag, disable render handle, start camera --- */
    (*(u8*)&lbl_8047A41C) = 0;
    GSmodelSetVisibility(lbl_8047A414, 0);

    fadeSet(lbl_8047B9D0, 3);
    fadeCheck(1);
    fn_801024E8(1);

    /* --- Set up field model and audio, yield one VBlank --- */
    handle = fn_80113F48();
    cameraPlayAnime((s32)handle, 0x10b61800, 0, 0);

    _threadSwitch();   /* vsync yield */

    fn_80166AB8(0x4c8, 0, 0);

    /* --- Show UI layer, load trainer model --- */
    fn_80112260(1);
    fn_801CB834(0x10b11000, 0, 0, 0);

    /* --- Second camera fade, wait for sync, third camera fade --- */
    fadeSet(lbl_8047B9D0, 2);
    fadeCheck(1);
    cameraWaitSyncAnime(1);

    fadeSet(lbl_8047B9D8, 3);
    fadeCheck(1);
    fn_80112260(0);

    /* --- Position and bind the UI widget slots --- */
    menuModelInit(base + 0xd18, 0xe8, 0x11c);
    menuModelInit(base + 0xcd0, 0xe8, 0x11c);

    fn_80109C88(base + 0xd18, obj_a);
    fn_80109C88(base + 0xcd0, obj_b);

    /* --- Re-enable sound and render handle --- */
    cameraPlayAnime(0x37c, 0x0fff1800, 0, 1);

    GSmodelSetVisibility(lbl_8047A414, 1);
    menuOpen(0xde, 1);

    /* --- Final camera fade and set display-active --- */
    (*(u8*)&lbl_8047A41C) = 1;

    fadeSet(lbl_8047B9D8, 2);
    fadeCheck(1);

    /* --- Advance state machine --- */
    lbl_8047A42C = 0x10;
}
#endif

/* fn_8002E460 - 0x8002E460 | size: 0x5fc */
extern void windowSearchID(void);
extern void windowSearchItemID(void);
extern void winSpriteSetDisp(void);
extern void fn_80073A44(void);
extern void fn_8017B1AC(void);
extern u32 lbl_8047A424;
extern u32 lbl_8047A420;
extern f32 lbl_8047B9D0;
extern u32 lbl_8047A42C;
extern u32 lbl_8047A428;
extern u32 lbl_8047A410;
#if 0
asm void fn_8002E460(void) {
#include "src/game/gs_worldmap_fn_8002E460.inc"
}
#else
/*
 * fn_8002E460 / GSmap_DrawFullFrame (0x8002E460, size 0x5FC)
 *
 * Two-Pokemon "swap / register" worldmap sub-flow. Resolves two storage-box
 * Pokemon handles from the current map-state selectors (lbl_8047A424 / A420),
 * binds them into two list widgets (state+0xD18, state+0xCD0), primes a fixed
 * set of UI menu fields (object 0xDB), then runs a frame-driven loop that polls
 * directional input (fn_80073A44) and a status query (fn_8017B1AC). Each tick it
 * toggles two confirmation flags (r30/r29) on newly-pressed input bits, plays a
 * select/cancel SE, and refreshes the menu fields. The loop ends when both flags
 * are set (commit), when a cancel bit is hit (abort), or when the input query
 * returns a hard-cancel (status==0 path). On commit it tears down the widgets,
 * deep-copies the staged record returned by savedataGetStatus(0,0) into the caller's
 * mapCtx buffer (0x3BFA word-pairs = 0x1DFD0 bytes), advances the map state
 * machine to 0xD, and re-shows the dialog camera. On abort it resets the three
 * selector globals to -1 and advances to state 2.
 *
 * Real param: r3 (=r27) is the caller's destination record buffer (mapCtx),
 * passed from the dispatcher in gs_npc_event.c (return of fn_801D036C).
 */
void fn_8002E460(void* mapCtx)
{
    extern u8  lbl_803A2518[];          /* worldmap UI/state block base */
    extern u32 lbl_8047A424;            /* selector A (16-bit index in low half) */
    extern u32 lbl_8047A420;            /* selector B (16-bit index in low half) */
    extern u32 lbl_8047A428;            /* selector C (reset on abort) */
    extern u32 lbl_8047A42C;            /* map state-machine state */
    extern u32 lbl_8047A410;  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047B9D0;            /* camera transition param (0.0f) */

    extern u8*  heroGetStatus(void* obj, u32 selector, u32 idx);   /* interaction getter -> handle */
    extern void menuModelInit(void* widget, s32 a, s32 b);         /* list-widget init */
    extern void fn_80109C88(void* widget, void* item);          /* bind item to widget */
    extern void fn_8010A420(void* widget);                      /* destroy widget */
    extern void menuOpen(s32 menuId, s32 flag);              /* menu open */
    extern void menuClose(s32 menuId);                        /* menu close */
    extern u8*  windowSearchID(s32 menuId);                        /* select menu object */
    extern u8*  windowSearchItemID(void* menuObj, s32 fieldId);        /* select field -> handle */
    extern void winSpriteSetDisp(void* fieldHandle, s32 value);     /* set field value */
    extern void fadeSet(f32 value, s32 mode);               /* camera anim init */
    extern void fadeCheck(s32 flag);                          /* camera anim start */
    extern u8*  windowGetKeyInfo(void);                              /* current input/state base */
    extern s32  fn_80073A44(s32 mode, u16* outFlags);          /* poll input -> status, writes flags */
    extern s32  fn_8017B1AC(void);                              /* input-mode status query */
    extern void fn_80166AB8(s32 soundId, s32 p2, s32 p3);       /* play SE */
    extern void winMsgOpen(s32 a, s32 b, s32 c, s32 d);        /* dialog/sound event */
    extern void _threadSwitch(void);                             /* vsync / scheduler yield */
    extern void* savedataGetStatus(s32 a, s32 sel);                  /* staged-record getter */

    u8* state = lbl_803A2518;
    u8* handleA;
    u8* handleB;
    s32 doneA;          /* r30: first selection committed */
    s32 doneB;          /* r29: second selection committed */
    u16 prevFlags;      /* r28: previous raw input-flag word */
    u16 rawFlags;       /* r26: per-tick raw flags from windowGetKeyInfo */
    u16 newPress;       /* r25: newly set bits (rawFlags & ~prevFlags) */
    u16 pollFlags;      /* sp+0x8: flag word written by fn_80073A44 */
    s32 status;
    s32 i;

    /* field-id groups refreshed every tick: index 0 carries the flag itself,
       the remaining four carry its logical negation (lit/unlit state). */
    static const s32 idsA[5] = { 0x11A8, 0x0F9B, 0x0F9A, 0x0FA3, 0x0FA5 };
    static const s32 idsB[5] = { 0x11A9, 0x0F99, 0x0F98, 0x0FA4, 0x0FA6 };

    handleA = heroGetStatus((void*)0, 3, lbl_8047A424 & 0xFFFF);
    handleB = heroGetStatus(state + 0x170, 3, lbl_8047A420 & 0xFFFF);

    menuModelInit(state + 0xD18, 0xE4, 0x8F);
    menuModelInit(state + 0xCD0, 0xE4, 0x8F);
    fn_80109C88(state + 0xD18, handleA);
    fn_80109C88(state + 0xCD0, handleB);

    menuOpen(0xDB, 0);

    /* initial menu-field priming: first of each group = 0, the rest = 1 */
    {
        u8* mo; u8* fh;
        mo = windowSearchID(0xDB); fh = windowSearchItemID(mo, idsA[0]); winSpriteSetDisp(fh, 0);
        for (i = 1; i < 5; i++) { mo = windowSearchID(0xDB); fh = windowSearchItemID(mo, idsA[i]); winSpriteSetDisp(fh, 1); }
        mo = windowSearchID(0xDB); fh = windowSearchItemID(mo, idsB[0]); winSpriteSetDisp(fh, 0);
        for (i = 1; i < 5; i++) { mo = windowSearchID(0xDB); fh = windowSearchItemID(mo, idsB[i]); winSpriteSetDisp(fh, 1); }
    }

    fadeSet(lbl_8047B9D0, 2);
    fadeCheck(1);

    doneA = 0;
    doneB = 0;
    prevFlags = 0;

    /* main confirmation loop: runs while NOT (doneA && doneB) */
    for (;;) {
        s32 abortLoop = 0;

        if ((doneA & 0xFF) != 0 && (doneB & 0xFF) != 0) {
            break;  /* both confirmed -> finalize */
        }

        /* ---- loop body (L_8002E604) ---- */
        rawFlags = *(u16*)(windowGetKeyInfo() + 0x4);

        status = fn_80073A44(1, &pollFlags);
        if (status != 0) {
            /* hard cancel via input system */
            winMsgOpen(2, 0x4448, 1, 0);
            menuClose(0xDB);
            fn_8010A420(state + 0xD18);
            fn_8010A420(state + 0xCD0);
            lbl_8047A42C = 0;
            return;
        }

        if (fn_8017B1AC() != 5) {
            newPress = (u16)(pollFlags & ~prevFlags);
            prevFlags = pollFlags;

            if ((rawFlags & 0x10) != 0) {
                /* select bit for slot A */
                if ((doneA & 0xFF) == 0) {
                    fn_80166AB8(0x24, 0, 0);
                }
                doneA = 1;
            } else if ((rawFlags & 0x20) != 0) {
                /* cancel bit for slot A */
                fn_80166AB8(0x25, 0, 0);
                if ((doneA & 0xFF) == 0) {
                    doneA = 0;
                    abortLoop = 1;   /* L_8002E6D4 -> finalize */
                } else {
                    doneA = 0;
                }
            }

            if (!abortLoop) {
                /* L_8002E6EC: react to newly pressed bits for slot B */
                if ((newPress & 0x1) != 0) {
                    if ((doneB & 0xFF) == 0) {
                        fn_80166AB8(0x24, 0, 0);
                    }
                    doneB = 1;
                } else if ((newPress & 0x2) != 0) {
                    fn_80166AB8(0x25, 0, 0);
                    if ((doneB & 0xFF) == 0) {
                        doneB = 0;
                        abortLoop = 1;   /* L_8002E74C -> finalize */
                    } else {
                        doneB = 0;
                    }
                }
            }
        }

        if (abortLoop) {
            break;  /* jump straight to finalize block (L_8002E888) */
        }

        /* ---- L_8002E764: vsync then refresh menu fields with live flags ---- */
        _threadSwitch();
        {
            u8* mo; u8* fh;
            s32 notA = ((doneA & 0xFF) == 0) ? 1 : 0;
            s32 notB = ((doneB & 0xFF) == 0) ? 1 : 0;
            mo = windowSearchID(0xDB); fh = windowSearchItemID(mo, idsA[0]); winSpriteSetDisp(fh, doneA);
            for (i = 1; i < 5; i++) { mo = windowSearchID(0xDB); fh = windowSearchItemID(mo, idsA[i]); winSpriteSetDisp(fh, notA); }
            mo = windowSearchID(0xDB); fh = windowSearchItemID(mo, idsB[0]); winSpriteSetDisp(fh, doneB);
            for (i = 1; i < 5; i++) { mo = windowSearchID(0xDB); fh = windowSearchItemID(mo, idsB[i]); winSpriteSetDisp(fh, notB); }
        }
    }

    /* ---- finalize (L_8002E888): one last field refresh ---- */
    {
        u8* mo; u8* fh;
        s32 notA = ((doneA & 0xFF) == 0) ? 1 : 0;
        s32 notB = ((doneB & 0xFF) == 0) ? 1 : 0;
        mo = windowSearchID(0xDB); fh = windowSearchItemID(mo, idsA[0]); winSpriteSetDisp(fh, doneA);
        for (i = 1; i < 5; i++) { mo = windowSearchID(0xDB); fh = windowSearchItemID(mo, idsA[i]); winSpriteSetDisp(fh, notA); }
        mo = windowSearchID(0xDB); fh = windowSearchItemID(mo, idsB[0]); winSpriteSetDisp(fh, doneB);
        for (i = 1; i < 5; i++) { mo = windowSearchID(0xDB); fh = windowSearchItemID(mo, idsB[i]); winSpriteSetDisp(fh, notB); }
    }

    if ((doneA & 0xFF) != 0 && (doneB & 0xFF) != 0) {
        /* ---- commit path (L_8002E9E4) ---- */
        u32* src;
        u32* dst;
        u32 lo;
        u32 hi;

        fn_8010A420(state + 0xD18);
        fn_8010A420(state + 0xCD0);

        src = (u32*)savedataGetStatus(0, 0);
        dst = (u32*)mapCtx;
        /* deep-copy 0x3BFA word-pairs (0x1DFD0 bytes) from staged record into caller buffer */
        for (i = 0; i < 0x3BFA; i++) {
            lo = src[0];
            hi = src[1];
            dst[0] = lo;
            dst[1] = hi;
            src += 2;
            dst += 2;
        }

        lbl_8047A42C = 0xD;
        fadeSet(lbl_8047B9D0, 3);
        fadeCheck(1);
        menuClose(0xDB);
    } else {
        /* ---- abort path (L_8002E9A8) ---- */
        menuClose(0xDB);
        lbl_8047A428 = (u32)-1;
        lbl_8047A424 = (u32)-1;
        lbl_8047A420 = (u32)-1;
        fn_8010A420(state + 0xD18);
        fn_8010A420(state + 0xCD0);
        (*(u8*)&lbl_8047A410) = 1;
        lbl_8047A42C = 2;
    }
}
#endif

/* fn_8002EA5C - 0x8002EA5C | size: 0x418 */
extern void pokemonBiosGetItemDataId(void);
extern void itemDataBiosCheckImportable(void);
extern void fn_801021F8(void);
extern void heroBiosGetPokemonPtr(void);
extern void pokemonBiosGetFuseiFlag(void);
extern void menuCBRule_CheckPokemonEventFlag(void);
extern void pokemonBiosGetTamagoFlag(void);
extern void pokemonGetStatus(void);
extern u32 lbl_8047A428;
extern u8 lbl_803A2688[];
extern f32 lbl_8047B9D4;
extern f64 lbl_8047B9E0;
extern f64 lbl_8047B9E8;
extern f32 lbl_8047B9DC;
extern u32 lbl_8047A42C;
extern u32 lbl_8047A420;
extern f32 lbl_8047B9D0;
#if 0
asm void fn_8002EA5C(void) {
#include "src/game/gs_worldmap_fn_8002EA5C.inc"
}
#else
/*
 * fn_8002EA5C  (GSmap_HandleSceneChange) - 0x8002EA5C, size 0x418
 *
 * Worldmap scene-change handler. Takes no parameters (reads the small-data
 * global lbl_8047A428 = the current/selected map id before touching any
 * argument register). Returns void.
 *
 * Flow:
 *   1. Query the selected map entry; gate on availability (pokemonBiosGetItemDataId +
 *      itemDataBiosCheckImportable). If unavailable, run the "blocked" timeout: flash the
 *      0xD9 list element (marker 0x43DD), spin a frame-wait, clear it, then
 *      set state lbl_8047A42C = 7 and bail.
 *   2. Otherwise scan party-member slots 0..5 (skipping the active map slot)
 *      for a member that satisfies the carry/usable predicate chain and owns
 *      species property 0x83. If none qualifies, run the "no-match" timeout
 *      (identical shape, marker 0x44E8) and set state 7.
 *   3. If a member qualifies, commit the destination (lbl_8047A420 =
 *      lbl_8047A428), run the transition frame-wait, kick the field
 *      camera/fade (fadeSet/fadeCheck) and sound (menuClose), and
 *      advance state lbl_8047A42C = 0xC.
 *
 * The triple frame-wait loop is the engine's time-integration idiom:
 *   accum += (f32)fn_800D3088() / (f32)(s32)fn_800D37CC()  per vsync yield
 * (the 0x43300000 double-word int->float magic in the asm is normalized to
 * plain casts here). fn_800D3088 = elapsed frame ticks (unsigned numerator),
 * fn_800D37CC = ticks-per-unit (signed denominator).
 */
#pragma peephole off
void fn_8002EA5C(void)
{
    /* small-data globals */
    extern u32 lbl_8047A428;     /* selected/source map id            */
    extern u32 lbl_8047A420;     /* committed destination map id      */
    extern u32 lbl_8047A42C;     /* worldmap state machine selector   */
    extern u8  lbl_803A2688[];   /* worldmap object/context base      */
    /* read-only float consts (r2-relative) */
    extern f32 lbl_8047B9D4;     /* frame-wait accumulator init (0.0) */
    extern f32 lbl_8047B9DC;     /* frame-wait limit, timeout loops   */
    extern f32 lbl_8047B9D0;     /* frame-wait limit, transition loop */

    /* cross-TU callees (block-scope typed externs, TU convention) */
    extern u32  heroGetStatus(u8* ptr, u32 selector, u32 idx);   /* interaction getter   */
    extern u16  pokemonBiosGetItemDataId(u8* ptr);                          /* map availability     */
    extern void itemDataBiosGetPtr(void);                             /* effect/handle helper */
    extern u8   itemDataBiosCheckImportable(void);                             /* gate result          */
    extern void fn_801021F8(u32 id, u32 flag);                 /* list show/hide       */
    extern s32  pokemonBiosGetNicknamePtr(u32 ref);                          /* get species          */
    extern void msgctrlSetValue(u32 prop, u32 value);              /* set display property */
    extern u8*  windowSearchID(s32 key);                          /* find list/window     */
    extern u8*  windowSearchItemID(u8* head, s32 key);                /* find child element   */
    extern void winSpriteSetDisp(u8* elem, u32 flag);              /* set element flag      */
    extern void fn_80166AB8(s32 soundId, s32 p2, s32 p3);     /* play SE              */
    extern void _threadSwitch(void);                             /* vsync yield          */
    extern u32  fn_800D3088(void);                             /* elapsed frame ticks  */
    extern s32  fn_800D37CC(void);                             /* ticks per unit       */
    extern u8*  heroBiosGetPokemonPtr(u8* base, u32 idx);                /* party slot getter    */
    extern u8   pokemonBiosGetFuseiFlag(u8* obj);                          /* slot predicate A     */
    extern u8   pokemonCheckValid(u8* obj);                          /* slot predicate B     */
    extern u8   menuCBRule_CheckPokemonEventFlag(u8* obj);                          /* slot predicate C     */
    extern u8   pokemonBiosGetTamagoFlag(u8* obj);                          /* slot predicate D     */
    extern u32  pokemonGetStatus(u8* obj, u32 id, u32 selector, u32 d); /* property getter  */
    extern void fadeSet(f32 target, s32 mode);            /* camera/fade target   */
    extern void fadeCheck(s32 flag);                        /* camera/fade enable   */
    extern void menuClose(u32 id);                          /* sound/window kick    */

    u8*  mapRef;
    u8*  window;
    u8*  element;
    f32  accum;
    f32  num;
    f32  den;
    u32  i;
    u32  mapId;
    u8   available;
    u8   found;

    mapId  = lbl_8047A428;
    mapRef = (u8*)(u32)heroGetStatus(lbl_803A2688, 3, (u16)mapId);

    if ((u16)pokemonBiosGetItemDataId((u8*)mapRef) != 0) {
        itemDataBiosGetPtr();
        available = itemDataBiosCheckImportable();
    } else {
        available = 1;
    }

    if ((u8)available == 0) {
        /* ---- blocked: map not selectable ---------------------------- */
        fn_801021F8(0xD9, 0);
        msgctrlSetValue(0x32, (u32)pokemonBiosGetNicknamePtr((u32)mapRef));

        window  = windowSearchID(0xD9);
        element = windowSearchItemID(window, 0x10B2);
        if (window != 0 && element != 0) {
            winSpriteSetDisp(element, 1);
            *(u32*)(element + 0x4C) = 0x43DD;
        }

        fn_80166AB8(0x26, 0, 0);

        accum = lbl_8047B9D4;
        while (accum < lbl_8047B9DC) {
            _threadSwitch();
            den = (f32)(s32)fn_800D37CC();
            num = (f32)(u32)fn_800D3088();
            accum = accum + num / den;
        }

        window  = windowSearchID(0xD9);
        element = windowSearchItemID(window, 0x10B2);
        if (window != 0 && element != 0) {
            *(u32*)(element + 0x4C) = 0;
            winSpriteSetDisp(element, 0);
        }

        fn_801021F8(0xD9, 1);
        lbl_8047A42C = 7;
        return;
    }

    /* ---- map selectable: scan party slots for an eligible member ---- */
    found = 0;
    for (i = 0; (u16)i < 6; i++) {
        u8* slot;

        if ((u16)i == (u16)mapId) {
            continue;
        }
        slot = heroBiosGetPokemonPtr(lbl_803A2688, i);
        if ((u8)pokemonBiosGetFuseiFlag(slot) != 0) {
            continue;
        }
        if ((u8)pokemonCheckValid(slot) == 0) {
            continue;
        }
        if ((u8)menuCBRule_CheckPokemonEventFlag(slot) != 1) {
            continue;
        }
        if ((u8)pokemonBiosGetTamagoFlag(slot) != 0) {
            continue;
        }
        if ((u16)pokemonGetStatus(slot, 0, 0x83, 0) == 0) {
            continue;
        }
        found = 1;
    }

    if ((u8)found == 0) {
        /* ---- no eligible member: timeout (marker 0x44E8) ------------ */
        fn_801021F8(0xD9, 0);
        msgctrlSetValue(0x32, (u32)pokemonBiosGetNicknamePtr((u32)mapRef));

        window  = windowSearchID(0xD9);
        element = windowSearchItemID(window, 0x10B2);
        if (window != 0 && element != 0) {
            winSpriteSetDisp(element, 1);
            *(u32*)(element + 0x4C) = 0x44E8;
        }

        fn_80166AB8(0x26, 0, 0);

        accum = lbl_8047B9D4;
        while (accum < lbl_8047B9DC) {
            _threadSwitch();
            den = (f32)(s32)fn_800D37CC();
            num = (f32)(u32)fn_800D3088();
            accum = accum + num / den;
        }

        window  = windowSearchID(0xD9);
        element = windowSearchItemID(window, 0x10B2);
        if (window != 0 && element != 0) {
            *(u32*)(element + 0x4C) = 0;
            winSpriteSetDisp(element, 0);
        }

        fn_801021F8(0xD9, 1);
        lbl_8047A42C = 7;
        return;
    }

    /* ---- success: commit destination and begin transition ---------- */
    lbl_8047A420 = lbl_8047A428;

    accum = lbl_8047B9D4;
    while (accum < lbl_8047B9D0) {
        _threadSwitch();
        den = (f32)(s32)fn_800D37CC();
        num = (f32)(u32)fn_800D3088();
        accum = accum + num / den;
    }

    fadeSet(lbl_8047B9D0, 3);
    fadeCheck(1);
    menuClose(0xD9);
    lbl_8047A42C = 0xC;
}
#pragma peephole reset
#endif

/* fn_8002EE74 - 0x8002EE74 | size: 0x410 */
extern void menuSetEnablePort(void);
extern void windowCheckCursor(void);
extern void windowGetValue(void);
extern void menuGetCursor(void);
extern void menuGetLastError(void);
extern u32 lbl_8047A428;
extern f32 lbl_8047B9D4;
extern f64 lbl_8047B9E0;
extern f64 lbl_8047B9E8;
extern f32 lbl_8047B9DC;
extern u32 lbl_8047A42C;
#if 0
asm void fn_8002EE74(void) {
#include "src/game/gs_worldmap_fn_8002EE74.inc"
}
#else
/* fn_8002EE74 - GSmap_TransitionToScene (0x8002EE74, size 0x410)
 *
 * Worldmap scene-transition driver. Looks up the current map object via the
 * interaction getter, then takes one of three paths:
 *   (A) object is present + entering-state==1 + flag set  -> play a "depart"
 *       SE node animation (anim id 0x43e1) on scene-model 0xd9, frame-pace a
 *       short delay via the host timer, then stop the anim; next state = 7.
 *   (B) object's alt-state flag set -> same as (A) but anim id 0x44be; state=7.
 *   (C) otherwise -> normal scene swap: pump model 0xe3, kick anims 0x43e4/
 *       0x43e5, query the destination slot/branch result and dispatch the next
 *       worldmap state (0xa/0xb/7) or, on the "joint-count==1" special case,
 *       fire effect 0x4448 and go to state 0.
 *
 * EABI: void(void); all inputs come from r13-relative SDA state globals.
 * The 0x4330_0000 double-word int->float magic in the timer loops is
 * normalized to plain (f64)(s32) casts.  *  ENDIAN-QA * 
 */
void fn_8002EE74(void)
{
    /* --- cross-TU callee decls (block-scope, TU convention) --- */
    extern u32   heroGetStatus(u8* ptr, u32 selector, u32 idx);   /* interaction getter */
    extern void  fn_801021F8(void* p, u32 val);                 /* enable/disable node subtree */
    extern u8    pokemonCheckValid(void* obj);
    extern u8    menuCBRule_CheckPokemonEventFlag(void* obj);
    extern u8    pokemonBiosGetTamagoFlag(void* obj);
    extern u8    pokemonBiosGetFuseiFlag(void* obj);
    extern void* windowSearchID(s32 key);                          /* scene node by slot id */
    extern void* windowSearchItemID(void* head, s32 subkey);           /* child node by sub-key */
    extern void  winSpriteSetDisp(void* node, u32 enable);           /* enable/disable a node */
    extern void  fn_80166AB8(s32 soundId, s32 p2, s32 p3);      /* play SE */
    extern void  _threadSwitch(void);                             /* host vsync yield (GSthreadYield) */
    extern s32   fn_800D37CC(void);                             /* timer read A */
    extern u32   fn_800D3088(void);                             /* timer read B (tick) */
    extern void  menuSetEnablePort(s32 mode);
    extern void  menuOpenCustom(void* p, u32 r4, s32 r5, s32 r6, void* r7, s32 r8, ...);
    extern u32   windowGetActiveID(void);
    extern void  windowCheckCursor(void* p, u8 flags);
    extern s32   windowGetValue(s32 key);
    extern s32   menuGetCursor(void* p);
    extern void  menuClose(s32 p);
    extern u32   menuGetLastError(void);
    extern void  winMsgOpen(s32 a, s32 b, s32 c, s32 d);

    /* --- SDA / sdata2 data (block-scope typed externs) --- */
    extern u32 lbl_8047A428;     /* current map index / state */
    extern u32 lbl_8047A42C;     /* next worldmap state machine value */
    extern u8  lbl_803A2688[];   /* interaction context base */
    extern f32 lbl_8047B9D4;     /* timer accumulator start value */
    extern f32 lbl_8047B9DC;     /* timer accumulator threshold */
    extern f64 lbl_8047B9E0;     /* timer-A baseline offset */
    extern f64 lbl_8047B9E8;     /* timer-B baseline offset */

    void* obj;
    void* node;
    void* child;
    f32   acc;

    obj = (void*)heroGetStatus(lbl_803A2688, 3, (u16)lbl_8047A428);
    fn_801021F8((void*)0xd9, 0);

    if (pokemonCheckValid(obj) != 0 &&
        menuCBRule_CheckPokemonEventFlag(obj) == 1 &&
        pokemonBiosGetTamagoFlag(obj) != 0) {
        /* ---- Branch A: depart-animation transition ---- */
        node  = windowSearchID(0xd9);
        child = windowSearchItemID(node, 0x10b2);
        if (node != (void*)0 && child != (void*)0) {
            winSpriteSetDisp(child, 1);
            *(u32*)((u8*)child + 0x4c) = 0x43e1;
        }
        fn_80166AB8(0x26, 0, 0);

        acc = lbl_8047B9D4;
        while (acc < lbl_8047B9DC) {
            f64 dtA, dtB;
            _threadSwitch();
            dtA = (f64)(s32)fn_800D37CC() - lbl_8047B9E0;   /* ENDIAN-QA */
            dtB = (f64)(s32)fn_800D3088() - lbl_8047B9E8;   /* ENDIAN-QA */
            acc = acc + (f32)(dtB / dtA);
        }

        node  = windowSearchID(0xd9);
        child = windowSearchItemID(node, 0x10b2);
        if (node != (void*)0 && child != (void*)0) {
            *(u32*)((u8*)child + 0x4c) = 0;
            winSpriteSetDisp(child, 0);
        }
        lbl_8047A42C = 7;
        return;
    }

    if (pokemonBiosGetFuseiFlag(obj) != 0) {
        /* ---- Branch B: alternate depart-animation transition ---- */
        node  = windowSearchID(0xd9);
        child = windowSearchItemID(node, 0x10b2);
        if (node != (void*)0 && child != (void*)0) {
            winSpriteSetDisp(child, 1);
            *(u32*)((u8*)child + 0x4c) = 0x44be;
        }
        fn_80166AB8(0x26, 0, 0);

        acc = lbl_8047B9D4;
        while (acc < lbl_8047B9DC) {
            f64 dtA, dtB;
            _threadSwitch();
            dtA = (f64)(s32)fn_800D37CC() - lbl_8047B9E0;   /* ENDIAN-QA */
            dtB = (f64)(s32)fn_800D3088() - lbl_8047B9E8;   /* ENDIAN-QA */
            acc = acc + (f32)(dtB / dtA);
        }

        node  = windowSearchID(0xd9);
        child = windowSearchItemID(node, 0x10b2);
        if (node != (void*)0 && child != (void*)0) {
            *(u32*)((u8*)child + 0x4c) = 0;
            winSpriteSetDisp(child, 0);
        }
        lbl_8047A42C = 7;
        return;
    }

    /* ---- Branch C: normal destination swap ---- */
    {
        s32 branchResult;
        s32 destResult;
        s32 flag = 1;

        menuSetEnablePort(2);
        menuOpenCustom((void*)0xe3, windowGetActiveID(), (s32)&flag, 0, (void*)0, 0);

        node  = windowSearchID(0xe3);
        child = windowSearchItemID(node, 0x102a);
        if (node != (void*)0 && child != (void*)0) {
            winSpriteSetDisp(child, 1);
            *(u32*)((u8*)child + 0x4c) = 0x43e4;
        }

        node  = windowSearchID(0xe3);
        child = windowSearchItemID(node, 0x1029);
        if (node != (void*)0 && child != (void*)0) {
            winSpriteSetDisp(child, 1);
            *(u32*)((u8*)child + 0x4c) = 0x43e5;
        }

        windowCheckCursor((void*)0xe3, 1);
        branchResult = windowGetValue(0xe3);
        destResult   = menuGetCursor((void*)0xe3);
        menuClose(0xe3);
        if (branchResult == -1) {
            destResult = -1;
        }

        menuSetEnablePort(1);
        if ((s32)menuGetLastError() == 1) {
            winMsgOpen(2, 0x4448, 1, 0);
            menuClose(0xd9);
            lbl_8047A42C = 0;
            return;
        }

        if (destResult == 0) {
            lbl_8047A42C = 0xb;
        } else if (destResult > 0) {
            if (destResult < 2) {       /* destResult == 1 */
                lbl_8047A42C = 0xa;
            }
            /* destResult >= 2: leave state unchanged */
        } else {                        /* destResult < 0 */
            if (destResult == -1) {
                lbl_8047A42C = 7;
            }
            /* destResult < -1: leave state unchanged */
        }
    }
}
#endif

/* fn_8002F284 - 0x8002F284 | size: 0x518 */
extern void menuItemBiosSetSelectFlag(void);
extern void menuGetCursorFromItemID(void);
extern void menuGetCursorItemID(void);
extern u32 lbl_8047A410;
extern u32 lbl_8047A42C;
extern const u8 lbl_80266E90[];
extern u32 lbl_8047A428;
#if 1
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

/* fn_8002F79C - 0x8002F79C | size: 0x4bc */
extern void itemDataBiosCheckExportable(void);
extern void pokemonBiosGetDarkFlag(void);
extern u32 lbl_8047A428;
extern f32 lbl_8047B9D4;
extern f64 lbl_8047B9E0;
extern f64 lbl_8047B9E8;
extern f32 lbl_8047B9DC;
extern u32 lbl_8047A42C;
extern u32 lbl_8047A410;
extern u32 lbl_8047A424;
#if 0
asm void fn_8002F79C(void) {
#include "src/game/gs_worldmap_fn_8002F79C.inc"
}
#else
/* fn_8002F79C - GSmap_PrepareArrival (0x8002F79C, 0x4BC)
 *
 * Overworld "prepare arrival / start-encounter check" step of the worldmap
 * state machine. Takes no parameters (CW: r3/r4 are loaded with literals
 * before any read). Drives the SDA state vars:
 *   lbl_8047A428 (u32) = current map/area index (input)
 *   lbl_8047A42C (u32) = worldmap step/state (output: 2 = abort/redo, 7 = arrive)
 *   (*(u8*)&lbl_8047A410) (u8)  = "arrival ready" flag (output)
 *   lbl_8047A424 (u32) = committed/arrival area index (output)
 *
 * Three outcomes:
 *   (A) party not ready            -> play abort cue, spin a frame-timed delay, state=2
 *   (B) interaction reports busy   -> suppress UI, play abort cue, spin delay, state=2
 *   (C) at least one party member is a valid wild/usable mon at a different
 *       slot than the current area -> normal "loading" cue, spin delay, state=2
 *   (D) otherwise commit the arrival: latch the area index and set state=7.
 *
 * The CW 0x43300000 store/lfd/fsub sequences are the standard big-endian
 * int->double conversion magic; normalized here to plain casts. The inner
 * loop is a vsync-paced timing spin that accumulates
 *   acc += (f64)(s32)GSgfx_tick() / (f64)(u32)GSrandom_Get()
 * until it crosses an f32 threshold, yielding to the scheduler each frame.
 */
void fn_8002F79C(void) {
    /* ---- cross-TU callees (block-scope typed externs, TU convention) ---- */
    extern u8*  savedataGetStatus(s32 side, s32 slotType);      /* get party/group handle */
    extern u32  heroGetStatus(u8* ptr, u32 selector, u32 idx); /* interaction getter */
    extern u16  pokemonBiosGetItemDataId(u8* obj);                     /* read interaction field */
    extern u8   itemDataBiosGetPtr(u16 handle);                  /* effect/UI helper */
    extern u8   itemDataBiosCheckExportable(void);                        /* arrival-ready query */
    extern s32  pokemonBiosGetNicknamePtr(s32 pokemon);                 /* get species/id */
    extern void msgctrlSetValue(s32 msgType, s32 species);    /* show message */
    extern void* windowSearchID(s32 key);                    /* lookup effect object */
    extern void* windowSearchItemID(void* obj, s32 sub);         /* sub-object lookup */
    extern void winSpriteSetDisp(void* elem, u32 flag);        /* activate effect element */
    extern void fn_80166AB8(u32 a, u32 b, u32 c);         /* play sound/cue */
    extern void fn_801021F8(s32 id, s32 flag);            /* set UI visibility */
    extern u32  pokemonBiosGetDarkFlag(u8* obj);                     /* interaction busy query */
    extern u8*  heroBiosGetPokemonPtr(u8* party, u32 slot);         /* get party member at slot */
    extern u8   pokemonBiosGetFuseiFlag(u8* mon);                     /* flag query */
    extern u8   pokemonCheckValid(u8* mon);                     /* validity check */
    extern u8   menuCBRule_CheckPokemonEventFlag(u8* mon);                     /* usable-state query */
    extern u8   pokemonBiosGetTamagoFlag(u8* mon);                     /* flag query */
    extern u32  pokemonGetStatus(u8* obj, u32 id, u32 selector, u32 d); /* mon prop getter */
    extern void _threadSwitch(void);                        /* vsync / scheduler yield */
    extern u32  fn_800D37CC(void);                        /* GSrandom_Get */
    extern s32  fn_800D3088(void);                        /* GSgfx tick */

    /* ---- SDA state globals (block-scope typed externs) ---- */
    extern u32 lbl_8047A428;   /* current map/area index */
    extern u32 lbl_8047A42C;   /* worldmap step/state */
    extern u32 lbl_8047A410;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A424;   /* committed area index */

    /* ---- timing-loop constants (sdata2). f32 accumulator start/threshold ---- */
    extern f32 lbl_8047B9D4;   /* loop accumulator start */
    extern f32 lbl_8047B9DC;   /* loop accumulator threshold */

    u8* party;
    u8* interact;
    u16 field;
    u8  ready;
    s32 species;
    void* effRoot;
    void* effElem;
    f32 acc;
    u32 slot;
    u8  foundWild;

    party = savedataGetStatus(0, 2);
    interact = (u8*)heroGetStatus(0, 3, (u16)lbl_8047A428);

    field = pokemonBiosGetItemDataId(interact);
    if (field != 0) {
        itemDataBiosGetPtr(field);
        ready = itemDataBiosCheckExportable();
    } else {
        ready = 1;
    }

    if (ready == 0) {
        /* ---- (A) party not ready: abort cue + delay, redo this step ---- */
        species = pokemonBiosGetNicknamePtr((s32)interact);
        msgctrlSetValue(0x32, species);

        effRoot = windowSearchID(0xD9);
        effElem = windowSearchItemID(effRoot, 0x10B2);
        if (effRoot != 0 && effElem != 0) {
            winSpriteSetDisp(effElem, 1);
            *(u32*)((u8*)effElem + 0x4C) = 0x43DD;
        }

        fn_80166AB8(0x26, 0, 0);

        acc = lbl_8047B9D4;
        while (acc < lbl_8047B9DC) {
            f64 r;
            _threadSwitch();
            r = (f64)(u32)fn_800D37CC();          /* ENDIAN-QA: unsigned int->double */
            acc += (f32)((f64)(s32)fn_800D3088() / r); /* ENDIAN-QA: signed int->double */
        }

        effRoot = windowSearchID(0xD9);
        effElem = windowSearchItemID(effRoot, 0x10B2);
        if (effRoot != 0 && effElem != 0) {
            *(u32*)((u8*)effElem + 0x4C) = 0;
            winSpriteSetDisp(effElem, 0);
        }

        lbl_8047A42C = 2;
        return;
    }

    /* ---- ready != 0 ---- */
    if ((u8)pokemonBiosGetDarkFlag(interact) == 1) {
        /* ---- (B) interaction busy: hide UI, abort cue + delay, redo ---- */
        fn_801021F8(0xD9, 0);

        species = pokemonBiosGetNicknamePtr((s32)interact);
        msgctrlSetValue(0x32, species);

        effRoot = windowSearchID(0xD9);
        effElem = windowSearchItemID(effRoot, 0x10B2);
        if (effRoot != 0 && effElem != 0) {
            winSpriteSetDisp(effElem, 1);
            *(u32*)((u8*)effElem + 0x4C) = 0x43DF;
        }

        fn_80166AB8(0x26, 0, 0);

        acc = lbl_8047B9D4;
        while (acc < lbl_8047B9DC) {
            f64 r;
            _threadSwitch();
            r = (f64)(u32)fn_800D37CC();
            acc += (f32)((f64)(s32)fn_800D3088() / r);
        }

        effRoot = windowSearchID(0xD9);
        effElem = windowSearchItemID(effRoot, 0x10B2);
        if (effRoot != 0 && effElem != 0) {
            *(u32*)((u8*)effElem + 0x4C) = 0;
            winSpriteSetDisp(effElem, 0);
        }

        fn_801021F8(0xD9, 1);
        lbl_8047A42C = 2;
        return;
    }

    /* ---- scan the 6 party slots for a valid wild/usable mon at a slot
     *      other than the current area index ---- */
    foundWild = 0;
    for (slot = 0; slot < 6; slot++) {
        u8* mon;
        if ((u16)slot == (u16)lbl_8047A428) {
            continue;
        }
        mon = heroBiosGetPokemonPtr(party, slot);
        if (pokemonBiosGetFuseiFlag(mon) != 0) {
            continue;
        }
        if (pokemonCheckValid(mon) == 0) {
            continue;
        }
        if (menuCBRule_CheckPokemonEventFlag(mon) != 1) {
            continue;
        }
        if (pokemonBiosGetTamagoFlag(mon) != 0) {
            continue;
        }
        if ((u16)pokemonGetStatus(mon, 0, 0x83, 0) != 0) {
            foundWild = 1;
        }
    }

    if (foundWild == 0) {
        /* ---- (C) no usable wild mon: loading cue + delay, redo ---- */
        fn_801021F8(0xD9, 0);

        species = pokemonBiosGetNicknamePtr((s32)interact);
        msgctrlSetValue(0x32, species);

        effRoot = windowSearchID(0xD9);
        effElem = windowSearchItemID(effRoot, 0x10B2);
        if (effRoot != 0 && effElem != 0) {
            winSpriteSetDisp(effElem, 1);
            *(u32*)((u8*)effElem + 0x4C) = 0x44E8;
        }

        fn_80166AB8(0x26, 0, 0);

        acc = lbl_8047B9D4;
        while (acc < lbl_8047B9DC) {
            f64 r;
            _threadSwitch();
            r = (f64)(u32)fn_800D37CC();
            acc += (f32)((f64)(s32)fn_800D3088() / r);
        }

        effRoot = windowSearchID(0xD9);
        effElem = windowSearchItemID(effRoot, 0x10B2);
        if (effRoot != 0 && effElem != 0) {
            *(u32*)((u8*)effElem + 0x4C) = 0;
            winSpriteSetDisp(effElem, 0);
        }

        fn_801021F8(0xD9, 1);
        lbl_8047A42C = 2;
        return;
    }

    /* ---- (D) commit arrival ---- */
    (*(u8*)&lbl_8047A410) = 1;
    lbl_8047A424 = lbl_8047A428;
    lbl_8047A42C = 7;
}
#endif
