/**
 * @file gs_npc_event.c
 * @brief GSnpcEvent -- NPC event callbacks and overworld sprite rendering.
 *
 * Address range: 0x80030170 - 0x80033278 (~30 functions)
 *
 * This module provides the NPC event callback functions that are invoked
 * when specific NPC interactions trigger game events. It also handles
 * NPC overworld model/sprite rendering during event sequences.
 *
 * The functions in this module are registered as callbacks in the event
 * system and are called when the player interacts with specific NPCs
 * or triggers location-based events.
 *
 * Key functions:
 *   fn_80030170  GSnpcEvt_PopEventQueue      -- 0x38 bytes, dequeue next event
 *   fn_800301A8  GSnpcEvt_NopA               -- 4 bytes, blr stub
 *   fn_800301AC  GSnpcEvt_NopB               -- 4 bytes, blr stub
 *   fn_800301B0  GSnpcEvt_ProcessTrainerSprite -- 0x120 bytes, trainer sprite display
 *   fn_800302D0  GSnpcEvt_ProcessOverworldNPC -- 0xA0 bytes, overworld NPC event
 *   fn_80030370  GSnpcEvt_SetModelA          -- 0xC bytes, set model ID 0x43E3
 *   fn_8003037C  GSnpcEvt_SetModelB          -- 0xC bytes, set model ID 0x43E2
 *   fn_80030388  GSnpcEvt_ProcessMirorB      -- 0xA0 bytes, Miror B. encounter event
 *   fn_80030428  GSnpcEvt_ReturnZero         -- 4 bytes, li r3,0 + blr
 *   fn_8003042C  GSnpcEvt_ShadowEncounterCB  -- 0x148 bytes, Shadow Pokemon event CB
 *   fn_80030574  GSnpcEvt_ColosseumePreBattle -- 0x234 bytes, pre-battle setup
 *   fn_800307A8  GSnpcEvt_StoryBattleCB      -- 0x12C bytes, story battle callback
 *   fn_800308D4  GSnpcEvt_QuestCompleteCB    -- 0x170 bytes, quest completion handler
 *   fn_80030A44  GSnpcEvt_ItemPickupCB       -- 0x1D0 bytes, overworld item pickup
 *   fn_80030C14  GSnpcEvt_HiddenItemCB       -- 0x120 bytes, hidden item event
 *   fn_80030D34  GSnpcEvt_TreasureChestCB    -- 0x1D8 bytes, treasure chest interaction
 *   fn_80030F0C  GSnpcEvt_FinalChunk         -- remaining functions to 0x80033278
 *
 * fn_800301B0 (GSnpcEvt_ProcessTrainerSprite):
 *   Handles the display of a trainer sprite during battle transitions.
 *   Checks the NPC event type at offset +0x06 (lha) and dispatches:
 *     0x10CE: Load trainer model from lbl_803A3230
 *     0x10CF: Load trainer model from lbl_803A31E8
 *   Then sets up the renderer:
 *     - fn_800D88DC: Set render layer 3
 *     - fn_800D888C: Set render layer 4
 *     - fn_800D6A00: Set render mode 7
 *     - fn_800D7820: Load model resource
 *     - fn_800D85D4: Bind model to slot 0
 *     - fn_800D67BC: Set blend mode 2
 *     - fn_800D61E4: Set position (x, y)
 *     - fn_800D5CB8: Set color RGBA (255, 255, 255, 255)
 *     - fn_800D59B8: Set scale (1.0, 1.0)
 *     - fn_800D6728: Commit render state
 *
 * fn_800302D0 (GSnpcEvt_ProcessOverworldNPC):
 *   Handles overworld NPC events based on the NPC's event type ID:
 *     0x0FBC: Generic NPC dialog event
 *     0x0FBD: Special NPC battle trigger
 *   For battle triggers, looks up the trainer data from lbl_803A2688,
 *   sets the battle parameter via msgctrlSetValue (param 0x4D), then
 *   initiates a trainer model render via fn_800FB680.
 *
 * fn_80030388 (GSnpcEvt_ProcessMirorB):
 *   Same structure as fn_800302D0 but for Miror B. encounters:
 *     0x10CA: Miror B. pre-battle
 *     0x10CB: Miror B. post-battle
 *   Uses model ID 0x4412 for the standard render and 0x4413 for
 *   the alt render.
 *
 * SDA globals:
 *   lbl_8047B9D4: Model scale factor (float, sdata2)
 *   lbl_8047B9F0: Model Y offset (float, sdata2)
 *   lbl_8047B834: Float constant for render setup (sdata2)
 *   lbl_8047B850: Float constant for render setup (sdata2)
 *
 * BSS globals:
 *   lbl_803A3230: Trainer model data A (loaded from FSYS)
 *   lbl_803A31E8: Trainer model data B (loaded from FSYS)
 *   lbl_803A2688: Party/trainer working data
 *   lbl_80314F98: Default model resource pointer
 */

#include "dolphin/types.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

/* Model/Rendering system */
extern void  fn_800D88DC(s32 layer);              /* Set render layer */
extern void  fn_800D888C(s32 layer);              /* Set secondary layer */
extern void  fn_800D6A00(s32 mode);               /* Set render mode */
extern void  fn_800D7820(void* resource);          /* Load model resource */
extern void  fn_800D85D4(s32 slot, void* model);  /* Bind model to slot */
extern void  fn_800D67BC(s32 blendMode);           /* Set blend mode */
extern void  fn_800D61E4(s32 x, s32 y);           /* Set position */
extern void  fn_800D5CB8(s32 slot, s32 r, s32 g, s32 b, s32 a); /* Set color */
extern void  fn_800D59B8(s32 slot, f32 scaleX, f32 scaleY);     /* Set scale */
extern void  fn_800D6728(void);                    /* Commit render state */

/* NPC/interaction */
extern void* menuModelRender(void* data);              /* Get model from data */
extern s32   heroGetStatus(void* partyData, s32 slot, s32 p3);
extern void  msgctrlSetValue(s32 paramId, s32 value);
extern void  fn_800FB680(s32, s32, u32, u16);

/* Event queue */
/* The event queue is a simple array-based queue where fn_80030170 pops
 * the next pending event. Events are enqueued by the scene system when
 * NPC interactions or location triggers fire. */

/* =========================================================================
 * Function: GSnpcEvt_PopEventQueue
 * Address:  0x80030170
 * Size:     0x38
 *
 * Dequeues the next event from the event queue. Returns NULL if the
 * queue is empty (field at +0x34 >= 0xD indicates end of queue).
 * The queue is stored as an array of pointers at the start of the
 * context structure, with a count/index at offset +0x34.
 * ========================================================================= */

/* =========================================================================
 * Function: GSnpcEvt_ProcessTrainerSprite
 * Address:  0x800301B0
 * Size:     0x120
 *
 * Renders a trainer sprite/model during battle transition sequences.
 * Dispatches on the event type ID from the NPC data structure.
 * ========================================================================= */

/* =========================================================================
 * Function: GSnpcEvt_ProcessOverworldNPC
 * Address:  0x800302D0
 * Size:     0xA0
 *
 * Processes an overworld NPC event. Checks the NPC's event flags and
 * model type to determine the appropriate rendering and behavior.
 * ========================================================================= */

/* =========================================================================
 * Stubs for remaining GSnpcEvt functions (0x80031188-0x800330B8)
 * ========================================================================= */

/* 0x80031188 | 0xA0 */
extern u8 lbl_803A2688[];
#if 0
asm void fn_80031188(void) {
#include "src/game/gs_npc_event_fn_80031188.inc"
}
#else
#pragma peephole off
void fn_80031188(u8* r3, u8* r4) {
    u32 flags = *(u32*)(r4 + 0x64);
    s32 evtype = *(s16*)(r4 + 0x6);
    u32 combined = (flags & 0xa1400000) | *(u8*)(r3 + 0x8b);
    switch (evtype) {
    case 0xF6B:
        fn_800FB680(0, 0, combined, 0x4412);
        break;
    case 0xF6A:
        {
            s32 val = heroGetStatus((void*)lbl_803A2688, 1, 0);
            msgctrlSetValue(0x4D, val);
            fn_800FB680(0, 0, combined, 0x4413);
        }
        break;
    }
}
#pragma peephole on
#endif

/* 0x80031228 | 0x1DC */
extern void pokemonBiosGetFuseiFlag(void);
extern void fn_8010B718(void);
extern void winSpriteSetDisp(void*, s32);
extern void pokemonCheckValid(void);
extern void menuCBRule_CheckPokemonEventFlag(void);
extern u8 lbl_80266E90[];
#if 0
asm void fn_80031228(void) {
#include "src/game/gs_npc_event_fn_80031228.inc"
}
#else
#pragma push
#pragma peephole off
void fn_80031228(u8* arg0, u8* arg1) {
    u8* table;
    s32 kind;
    s32 arg;
    s32 i;
    void* obj;

    table = lbl_80266E90;
    kind = 0;
    arg = 0;
    for (i = 0; i < 12; i++) {
        if (*(s16*)(arg1 + 0x6) == *(u16*)(table + 0x2)) {
            kind = *(u8*)(table + 0x0);
            arg = *(u8*)(table + 0x1);
        }
        table += 0x12;
    }

    obj = 0;
    if (kind == 1) {
        obj = (void*)heroGetStatus(0, 3, (u16)arg);
    } else if (kind == 2) {
        obj = (void*)heroGetStatus(lbl_803A2688, 3, (u16)arg);
    }

    if (((u8 (*)(void*))pokemonBiosGetFuseiFlag)(obj) != 0) {
        ((void (*)(u8*, u8*, void*))fn_8010B718)(arg0, arg1, obj);
        winSpriteSetDisp(arg1, 1);
    } else if (((u8 (*)(void*))pokemonCheckValid)(obj) != 0 && ((u8 (*)(void))menuCBRule_CheckPokemonEventFlag)() == 1) {
        ((void (*)(u8*, u8*, void*))fn_8010B718)(arg0, arg1, obj);
        winSpriteSetDisp(arg1, 1);
    } else {
        winSpriteSetDisp(arg1, 0);
    }
}
#pragma pop
#endif

/* 0x80031404 | 0x244 */
extern void menuSubGetPokemonSexForDisp(void);
extern void pokemonBiosGetTamagoFlag(void);
extern void GSmsgGetGSchar(void);
#if 0
asm void fn_80031404(void) {
#include "src/game/gs_npc_event_fn_80031404.inc"
}
#else
#pragma push
#pragma peephole off
void fn_80031404(u8* arg0, u8* arg1) {
    u8* table;
    u32 combined;
    s32 kind;
    s32 arg;
    s32 i;
    void* obj;
    s32 msg;

    combined = (*(u32*)(arg1 + 0x64) & ~0xFF) | *(u8*)(arg0 + 0x8B);
    table = lbl_80266E90;
    kind = 0;
    arg = 0;
    for (i = 0; i < 12; i++) {
        if (*(s16*)(arg1 + 0x6) == *(u16*)(table + 0xC)) {
            kind = *(u8*)(table + 0x0);
            arg = *(u8*)(table + 0x1);
        }
        table += 0x12;
    }

    obj = 0;
    switch (kind) {
    case 1:
        obj = (void*)heroGetStatus(0, 3, (u16)arg);
        break;
    case 2:
        obj = (void*)heroGetStatus(lbl_803A2688, 3, (u16)arg);
        break;
    }

    if (((u8 (*)(void*))pokemonCheckValid)(obj) != 0 &&
        ((u8 (*)(void))menuCBRule_CheckPokemonEventFlag)() == 1) {
        msg = ((u8 (*)(void*))menuSubGetPokemonSexForDisp)(obj);
        switch (msg) {
        case 0:
            msg = 0xD67;
            break;
        case 1:
            msg = 0xD68;
            break;
        default:
            msg = 0;
            break;
        }
        if (((u8 (*)(void*))pokemonBiosGetTamagoFlag)(obj) == 1) {
            msg = 0;
        }
        if (msg != 0) {
            msgctrlSetValue(0x37, ((s32 (*)(s32))GSmsgGetGSchar)(msg));
            fn_800FB680(2, 0, combined, 0xCF);
            winSpriteSetDisp(arg1, 1);
        } else {
            winSpriteSetDisp(arg1, 0);
        }
    } else {
        winSpriteSetDisp(arg1, 0);
    }
}
#pragma pop
#endif

/* 0x80031648 | 0x290 */
extern void GSmsgGetRect(void);
extern void pokemonBiosGetNicknamePtr(void);
#if 0
asm void fn_80031648(void) {
#include "src/game/gs_npc_event_fn_80031648.inc"
}
#else
#pragma push
#pragma peephole off
void fn_80031648(u8* arg0, u8* arg1) {
    u8* table;
    s32 group;
    s32 arg;
    s32 i;
    void* obj;
    s32 x;
    s32 width;

    table = lbl_80266E90;
    group = 0;
    arg = 0;
    for (i = 0; i < 12; i++) {
        if (*(s16*)(arg1 + 0x6) == *(u16*)(table + 0xA)) {
            group = *(u8*)(table + 0x0);
            arg = *(u8*)(table + 0x1);
        }
        table += 0x12;
    }

    obj = 0;
    if (group == 2) goto grp2;
    if (group >= 2) goto grp_done;
    if (group >= 1) goto grp1;
    goto grp_done;
grp1:
    obj = (void*)heroGetStatus(0, 3, (u16)arg);
    goto grp_done;
grp2:
    obj = (void*)heroGetStatus(lbl_803A2688, 3, (u16)arg);
grp_done:
    ;

    if (((u8 (*)(void*))pokemonBiosGetFuseiFlag)(obj) != 0) {
        msgctrlSetValue(0x37, ((s32 (*)(s32))GSmsgGetGSchar)(0x56C));
    } else if (((u8 (*)(void*))pokemonCheckValid)(obj) != 0 &&
               ((u8 (*)(void*))menuCBRule_CheckPokemonEventFlag)(obj) == 1) {
        if (((u8 (*)(void*))pokemonBiosGetTamagoFlag)(obj) != 0) {
            msgctrlSetValue(0x37, ((s32 (*)(s32))GSmsgGetGSchar)(0x56B));
        } else {
            msgctrlSetValue(0x37, ((s32 (*)(void*))pokemonBiosGetNicknamePtr)(obj));
        }
    } else {
        winSpriteSetDisp(arg1, 0);
        return;
    }

    width = (s16)(((u32 (*)(s32))GSmsgGetRect)(0xE7) >> 16);
    x = (*(s16*)(arg1 + 0x54) / 2) - (width / 2);
    fn_800FB680((s16)x, 0, -1, 0xE7);
    winSpriteSetDisp(arg1, 1);
}
#pragma pop
#endif

/* 0x800318D8 | 0x144 -- scan 12-entry table for matching species, dispatch to winSpriteSetDisp */
extern u32 lbl_8047A424;
extern u32 lbl_8047A420;
extern u8  lbl_80266E90[];
#if 0
asm void fn_800318D8(void) {
#include "src/game/gs_npc_event_fn_800318D8.inc"
}
#else
void fn_800318D8(s32 _unused, u8* tgt) {
    u8* p = lbl_80266E90;
    s32 group = 0;
    s32 sub   = 0;
    s16 key   = *(s16*)(tgt + 6);
    s32 i;
    for (i = 0; i < 2; i++) {
        s32 j;
        for (j = 0; j < 6; j++) {
            if ((s32)key == (s32)*(u16*)(p + 0xe)) {
                group = p[0];
                sub   = p[1];
            }
            p += 0x12;
        }
    }
    if (group == 1 && (s32)lbl_8047A424 == sub) {
        winSpriteSetDisp(tgt, 1);
        return;
    }
    if (group == 2 && (s32)lbl_8047A420 == sub) {
        winSpriteSetDisp(tgt, 1);
        return;
    }
    winSpriteSetDisp(tgt, 0);
}
#endif

/* 0x54 | fn_80031A1C | generic */
/* fn_80031A1C - 0x80031A1C | size: 0x54 */
extern u32 lbl_8047A42C;
#if 0
asm void fn_80031A1C(void) {
#include "src/game/gs_npc_event_fn_80031A1C.inc"
}
#else
#pragma peephole off
void fn_80031A1C(void* r3, void* r4) {
    s32 val = (s32)lbl_8047A42C;
    switch (val) {
    case 7:
    case 2:
        winSpriteSetDisp(r4, 1);
        break;
    default:
        winSpriteSetDisp(r4, 0);
        break;
    }
}
#pragma peephole on
#endif

/* 0x50 | fn_80031A70 | generic */
/* fn_80031A70 - 0x80031A70 | size: 0x50 */
extern u32 lbl_8047A42C;
#if 0
asm void fn_80031A70(void) {
#include "src/game/gs_npc_event_fn_80031A70.inc"
}
#else
#pragma peephole off
void fn_80031A70(void* r3, void* r4) {
    s32 val = (s32)lbl_8047A42C;
    switch (val) {
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        winSpriteSetDisp(r4, 1);
        break;
    default:
        winSpriteSetDisp(r4, 0);
        break;
    }
}
#pragma peephole on
#endif

/* 0x50 | fn_80031AC0 | generic */
/* fn_80031AC0 - 0x80031AC0 | size: 0x50 */
extern u32 lbl_8047A42C;
#if 0
asm void fn_80031AC0(void) {
#include "src/game/gs_npc_event_fn_80031AC0.inc"
}
#else
#pragma peephole off
void fn_80031AC0(void* r3, void* r4) {
    s32 val = (s32)lbl_8047A42C;
    switch (val) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        winSpriteSetDisp(r4, 1);
        break;
    default:
        winSpriteSetDisp(r4, 0);
        break;
    }
}
#pragma peephole on
#endif

/* 0x80031B10 | 0x3C */
extern void _threadSwitch(void);
extern u32 fn_8010B560(void);
extern void fn_801CB9D8(u32);
extern u32 lbl_8047A418;
#if 0
asm void fn_80031B10(void) {
#include "src/game/gs_npc_event_fn_80031B10.inc"
}
#else
#pragma peephole off
void fn_80031B10(void) {
    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    fn_801CB9D8(lbl_8047A418);
}
#pragma peephole on
#endif

/* 0x80031B4C | 0x954 */
extern void fn_801D036C(void);
extern void fn_800849B4(void);
extern void fn_8002FC58(void);
extern void fn_801021F8(void);
extern void windowGetActiveID(void);
extern void menuOpenCustom(void);
extern void windowSearchID(void);
extern void windowSearchItemID(void);
extern void windowCheckCursor(void);
extern void windowGetValue(void);
extern void menuGetCursor(void);
extern void menuClose(void);
extern void fadeSet(void);
extern void fadeCheck(void);
extern void GSmodelSetVisibility(void);
extern void menuSetDisp(void);
extern void fn_80097CD0(void);
extern void fn_8002F79C(void);
extern void fn_8002F284(void);
extern void fn_8002EE74(void);
extern void menuSetEnablePort(void);
extern s32 menuGetLastError(void);
extern void winMsgOpen(s32, s32, s32, s32);
extern void fn_8002EA5C(void);
extern void fn_8002E460(void);
extern void menuOpen(void);
extern void menuModelInit(void);
extern void fn_80109C88(void);
extern void winMsgClose(s32);
extern void stateFunctionExChangeMain(void);
extern void fn_8010A420(void);
extern void pokemonBiosSetFriend(void);
extern void fn_8025FF9C(void);
extern void stateFunctionEvolution(void);
extern void stateFunctionSaveReport(void);
extern void fn_8001E074(void);
extern void fn_801D0314(void);
extern void fn_801024E8(void);
extern void fn_800FF52C(void);
extern void fn_800FF660(void);
extern void floorGetPrevFloorID(void);
extern void floorLink(void);
extern void jumptable_802E4F90();
extern u8 lbl_8047A409;
extern u8 lbl_803A2518[];
extern u8 lbl_804788B0;
extern u8 lbl_80266F90[];
extern u8 lbl_8047A40A;
extern u32 lbl_8047A42C;
extern f32 lbl_8047B9D0;
extern u32 lbl_8047A414;
extern f32 lbl_8047B9D4;
extern u32 lbl_8047A428;
extern u8 lbl_8047A410;
extern u32 lbl_8047A424;
extern u32 lbl_8047A420;
extern void menuSubOpenYesNo();
extern void memoDataSet();
#if 0
asm void fn_80031B4C(void) {
#include "src/game/gs_npc_event_fn_80031B4C.inc"
}
#else
#pragma push
#pragma optimization_level 2
void fn_80031B4C(void) {
    extern u8 lbl_80266F90[];
    extern u8 lbl_803A2518[];
    extern u8 lbl_804788B0;
    extern u8 lbl_8047A409;
    extern u8 lbl_8047A40A;
    extern u8 lbl_8047A410;
    extern u32 lbl_8047A414;
    extern u32 lbl_8047A420;
    extern u32 lbl_8047A424;
    extern u32 lbl_8047A428;
    extern u32 lbl_8047A42C;
    extern f32 lbl_8047B9D0;
    extern f32 lbl_8047B9D4;
    extern void fn_8001E074();
    extern void stateFunctionSaveReport();
    extern void stateFunctionEvolution();
    extern void stateFunctionExChangeMain();
    extern void fn_8002E460();
    extern void fn_8002EA5C();
    extern void fn_8002EE74();
    extern void fn_8002F284();
    extern void fn_8002F79C();
    extern void fn_8002FC58();
    extern s32 fn_800849B4(s32 mode, s32 command, void* input, void* output);
    extern void fn_80097CD0();
    extern void GSmodelSetVisibility();
    extern void fn_800FF52C();
    extern void fn_800FF660();
    extern s32 menuGetLastError();
    extern void fn_801021F8();
    extern void menuSetDisp();
    extern u32 menuGetCursor();
    extern void fn_801024E8();
    extern void menuClose();
    extern void menuOpen();
    extern void menuOpenCustom();
    extern void menuSetEnablePort();
    extern u32 windowGetValue();
    extern void windowCheckCursor();
    extern u32 windowGetActiveID();
    extern u32 windowSearchItemID();
    extern u32 windowSearchID();
    extern void winMsgClose();
    extern void winMsgOpen();
    extern void winSpriteSetDisp();
    extern void fn_80109C88();
    extern void fn_8010A420();
    extern void menuModelInit();
    extern void floorLink();
    extern void floorGetPrevFloorID();
    extern void pokemonBiosSetFriend();
    extern void pokemonBiosGetNicknamePtr();
    extern void fadeCheck();
    extern void fadeSet();
    extern void fn_801D0314();
    extern void fn_801D036C();
    extern void fn_8025FF9C();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    void (*ctr_fn)(void) = 0;

    tmp = lbl_8047A409;
    r3 = 0x1;
    r4 = (u32)lbl_803A2518;
    lbl_804788B0 = r3;
    r31 = (u32)lbl_803A2518;
    if (tmp != 1) {
        fn_801D036C();
        r30 = r3;
        goto L_800323E4;
    do {
            if (tmp <= 0x13) {
                switch (tmp) {
                case 1:
                    fn_8002FC58();
                    goto L_800323E4;
                case 2:
                    fn_801021F8(0xd9, 0);
                    *(u32*)(sp + 0xC) = 1;
                    r28 = windowGetActiveID();
                    menuOpenCustom(0xe3, r28, sp + 0xC, 0, 0, 0);

                    r28 = windowSearchID(0xe3);
                    r29 = windowSearchItemID(r28, 0x102a);
                    if ((r28 != 0) && (r29 != 0)) {
                        winSpriteSetDisp(r29, 1);
                        *(u32*)((u8*)r29 + 0x4C) = 0x43e4;
                    }

                    r28 = windowSearchID(0xe3);
                    r29 = windowSearchItemID(r28, 0x1029);
                    if ((r28 != 0) && (r29 != 0)) {
                        winSpriteSetDisp(r29, 1);
                        *(u32*)((u8*)r29 + 0x4C) = 0x43e5;
                    }

                    windowCheckCursor(0xe3, 1);
                    r29 = windowGetValue(0xe3);
                    r26 = menuGetCursor(0xe3);
                    menuClose(0xe3);
                    if ((s32)r29 == -1) {
                        r26 = -1;
                    }
                    if ((s32)r26 == 0) {
                        lbl_8047A42C = 6;
                    } else if ((s32)r26 < 0) {
                        if ((s32)r26 >= -1) {
                            lbl_8047A42C = 2;
                        }
                    } else if ((s32)r26 < 2) {
                        lbl_8047A42C = 5;
                    }
                    goto L_800323E4;
                case 3:
                    fn_801021F8(0xd9, 0);
                    r28 = windowSearchID(0xd9);
                    r29 = windowSearchItemID(r28, 0x10b2);
                    if ((r28 != 0) && (r29 != 0)) {
                        *(u32*)((u8*)r29 + 0x4C) = 0;
                        winSpriteSetDisp(r29, 0);
                    }

                    *(u32*)(sp + 8) = 1;
                    r28 = windowGetActiveID();
                    menuOpenCustom(0xe3, r28, sp + 8, 0, 0, 0);

                    r28 = windowSearchID(0xe3);
                    r29 = windowSearchItemID(r28, 0x102a);
                    if ((r28 != 0) && (r29 != 0)) {
                        winSpriteSetDisp(r29, 1);
                        *(u32*)((u8*)r29 + 0x4C) = 0x43d4;
                    }

                    r28 = windowSearchID(0xe3);
                    r29 = windowSearchItemID(r28, 0x1029);
                    if ((r28 != 0) && (r29 != 0)) {
                        winSpriteSetDisp(r29, 1);
                        *(u32*)((u8*)r29 + 0x4C) = 0x43d5;
                    }

                    windowCheckCursor(0xe3, 1);
                    r29 = windowGetValue(0xe3);
                    r26 = menuGetCursor(0xe3);
                    menuClose(0xe3);
                    if ((s32)r29 == -1) {
                        r26 = -1;
                    }

                    r28 = windowSearchID(0xd9);
                    r29 = windowSearchItemID(r28, 0x10b2);
                    if ((r28 != 0) && (r29 != 0)) {
                        *(u32*)((u8*)r29 + 0x4C) = 0;
                        winSpriteSetDisp(r29, 0);
                    }

                    if ((s32)r26 == 0) {
                        lbl_8047A42C = 2;
                    } else if ((s32)r26 < 0) {
                        if ((s32)r26 >= -1) {
                            lbl_8047A42C = 2;
                        }
                    } else if ((s32)r26 < 2) {
                        lbl_8047A42C = 0;
                    }
                    goto L_800323E4;
                case 4:
                    fadeSet(3, lbl_8047B9D0);
                    fadeCheck(1);
                    GSmodelSetVisibility(lbl_8047A414, 0);
                    menuSetDisp(0xd9, 0);
                    fadeSet(2, lbl_8047B9D4);
                    fadeCheck(1);
                    r29 = heroGetStatus(0, 3, (u16)lbl_8047A428);
                    fn_80097CD0(r29, 0, 0);
                    fadeSet(3, lbl_8047B9D4);
                    fadeCheck(1);
                    GSmodelSetVisibility(lbl_8047A414, 1);
                    menuSetDisp(0xd9, 1);
                    fadeSet(2, lbl_8047B9D0);
                    fadeCheck(1);
                    lbl_8047A42C = 2;
                    goto L_800323E4;
                case 10:
                    fadeSet(3, lbl_8047B9D0);
                    fadeCheck(1);
                    GSmodelSetVisibility(lbl_8047A414, 0);
                    menuSetDisp(0xdc, 0);
                    menuSetDisp(0xd9, 0);
                    fadeSet(2, lbl_8047B9D4);
                    fadeCheck(1);
                    r29 = heroGetStatus(lbl_803A2518 + 0x170, 3,
                                        (u16)lbl_8047A428);
                    menuSetEnablePort(2);
                    fn_80097CD0(r29, 0, 0);
                    menuSetEnablePort(1);
                    fadeSet(3, lbl_8047B9D4);
                    fadeCheck(1);
                    GSmodelSetVisibility(lbl_8047A414, 1);
                    menuSetDisp(0xd9, 1);
                    fadeSet(2, lbl_8047B9D0);
                    fadeCheck(1);
                    if (menuGetLastError() == 1) {
                        winMsgOpen(2, 0x4448, 1, 0);
                        menuClose(0xd9);
                        lbl_8047A42C = 0;
                    } else {
                        lbl_8047A42C = 7;
                    }
                    goto L_800323E4;
                default:
                    break;
                }
                r3 = (u32)jumptable_802E4F90;
                tmp = tmp << 2;
                r3 = (u32)jumptable_802E4F90;
                tmp = *(u32*)(r3 + tmp);
                ctr_fn = (void(*)(void))tmp;
                r3 = (u32)lbl_80266F90;
                r5 = (u32)sp + 0x10;
                r10 = (u32)lbl_80266F90;
                r3 = 0x0;
                r9 = *(u32*)((u8*)r10 + 0x0);
                r4 = 0x1;
                r8 = *(u32*)((u8*)r10 + 0x4);
                r6 = 0x0;
                r7 = *(u32*)((u8*)r10 + 0x8);
                tmp = *(u32*)((u8*)r10 + 0xC);
                *(u32*)(sp + 0x1C) = tmp;
                r3 = fn_800849B4(0, 1, sp + 0x10, 0);
                if ((s32)r3 < 0) {
                    tmp = 0x0;
                    lbl_8047A40A = tmp;
                    lbl_8047A42C = tmp;
                    goto L_800323E4;
                }
                tmp = 0x2;
                lbl_8047A42C = tmp;
                goto L_800323E4;


                *(u32*)(sp + 0xC) = tmp;
                windowGetActiveID();
                r4 = r3;
                r5 = (u32)sp + 0xc;
                r3 = 0xe3;
                r6 = 0x0;
                r7 = 0x0;
                r8 = 0x0;
                menuOpenCustom();
                r3 = 0xe3;
                windowSearchID();
                r29 = r3;
                r4 = 0x102a;
                windowSearchItemID();
                r29 = r3;
                if ((r29 != 0) && (r29 != 0)) {

                    r4 = 0x1;
                    winSpriteSetDisp();
                    tmp = 0x43e4;
                    *(u32*)((u8*)r29 + 0x4C) = tmp;
                }
                r3 = 0xe3;
                windowSearchID();
                r29 = r3;
                r4 = 0x1029;
                windowSearchItemID();
                r29 = r3;
                if ((r29 != 0) && (r29 != 0)) {

                    r4 = 0x1;
                    winSpriteSetDisp();
                    tmp = 0x43e5;
                    *(u32*)((u8*)r29 + 0x4C) = tmp;
                }
                r3 = 0xe3;
                r4 = 0x1;
                windowCheckCursor();
                r3 = 0xe3;
                windowGetValue();
                r29 = r3;
                r3 = 0xe3;
                menuGetCursor();
                r26 = r3;
                r3 = 0xe3;
                menuClose();
                if ((s32)r29 == (s32)-0x1) {
                    r26 = -0x1;
                }
                if ((s32)r26 != 0) {
                    if ((s32)r26 < 0) {
                        if ((s32)r26 < (s32)-0x1) {
                            goto L_800323E4;
                        }
                        if ((s32)r26 < 2) {
                            tmp = 0x5;
                            lbl_8047A42C = tmp;
                        }
                        goto L_800323E4;
                    }
                    tmp = 0x6;
                    lbl_8047A42C = tmp;
                    goto L_800323E4;
                        }
                tmp = 0x2;
                lbl_8047A42C = tmp;
                goto L_800323E4;


                if ((r29 != 0) && (r29 != 0)) {

                    r4 = 0x1;
                    winSpriteSetDisp();
                    tmp = 0x43db;
                    *(u32*)((u8*)r29 + 0x4C) = tmp;
                }
                tmp = 0x1;
                *(u32*)(sp + 0x8) = tmp;
                windowGetActiveID();
                r4 = r3;
                r5 = (u32)sp + 0x8;
                r3 = 0xe3;
                r6 = 0x0;
                r7 = 0x0;
                r8 = 0x0;
                menuOpenCustom();
                r3 = 0xe3;
                windowSearchID();
                r29 = r3;
                r4 = 0x102a;
                windowSearchItemID();
                r29 = r3;
                if ((r29 != 0) && (r29 != 0)) {

                    r4 = 0x1;
                    winSpriteSetDisp();
                    tmp = 0x43d4;
                    *(u32*)((u8*)r29 + 0x4C) = tmp;
                }
                r3 = 0xe3;
                windowSearchID();
                r29 = r3;
                r4 = 0x1029;
                windowSearchItemID();
                r29 = r3;
                if ((r29 != 0) && (r29 != 0)) {

                    r4 = 0x1;
                    winSpriteSetDisp();
                    tmp = 0x43d5;
                    *(u32*)((u8*)r29 + 0x4C) = tmp;
                }
                r3 = 0xe3;
                r4 = 0x1;
                windowCheckCursor();
                r3 = 0xe3;
                windowGetValue();
                r29 = r3;
                r3 = 0xe3;
                menuGetCursor();
                r26 = r3;
                r3 = 0xe3;
                menuClose();
                if ((s32)r29 == (s32)-0x1) {
                    r26 = -0x1;
                }
                r3 = 0xd9;
                windowSearchID();
                r29 = r3;
                r4 = 0x10b2;
                windowSearchItemID();
                if ((r29 != 0) && (r3 != 0)) {

                    tmp = 0x0;
                    r4 = 0x0;
                    *(u32*)((u8*)r3 + 0x4C) = tmp;
                    winSpriteSetDisp();
                }
                if ((s32)r26 != 0) {
                    if ((s32)r26 < 0) {
                        if ((s32)r26 < (s32)-0x1) {
                            goto L_800323E4;
                        }
                        if ((s32)r26 < 2) {

                        } else {
                            tmp = 0x2;
                            lbl_8047A42C = tmp;
                        }
                        goto L_800323E4;
                    }
                    tmp = 0x0;
                    lbl_8047A42C = tmp;
                    goto L_800323E4;
                        }
                tmp = 0x2;
                lbl_8047A42C = tmp;
                goto L_800323E4;


                lbl_8047A42C = tmp;
                goto L_800323E4;


                lbl_8047A410 = r4;
                lbl_8047A424 = r3;
                lbl_8047A42C = tmp;
                goto L_800323E4;


                if ((s32)r3 == 1) {
                    r3 = 0x2;
                    r4 = 0x4448;
                    r5 = 0x1;
                    r6 = 0x0;
                    winMsgOpen();
                    r3 = 0xd9;
                    menuClose();
                    tmp = 0x0;
                    lbl_8047A42C = tmp;
                    goto L_800323E4;
                }
                tmp = 0x7;
                lbl_8047A42C = tmp;
                goto L_800323E4;


                lbl_8047A42C = tmp;
                goto L_800323E4;


                lbl_8047A42C = tmp;
                goto L_800323E4;


                lbl_8047A42C = tmp;
                goto L_800323E4;


                if ((s32)tmp != 0) {
                    if ((s32)tmp < 0) {
                        if ((s32)tmp < (s32)-0x1) {
                            goto L_800323E4;
                        }
                        if ((s32)tmp < 2) {

                        } else {
                            r3 = 0xde;
                            menuClose();
                            tmp = -0x1;
                            r3 = r31 + 0xd18;
                            lbl_8047A428 = tmp;
                            lbl_8047A424 = tmp;
                            lbl_8047A420 = tmp;
                            fn_8010A420();
                            r3 = r31 + 0xcd0;
                            fn_8010A420();
                            r3 = 0x1;
                            tmp = 0x2;
                            lbl_8047A410 = r3;
                            lbl_8047A42C = tmp;
                        }
                        goto L_800323E4;
                    }
                    r3 = 0xde;
                    menuClose();
                    r3 = r31 + 0xd18;
                    fn_8010A420();
                    r3 = r31 + 0xcd0;
                    fn_8010A420();
                    tmp = 0x0;
                    lbl_8047A42C = tmp;
                    goto L_800323E4;
                        }
                r3 = 0xde;
                menuClose();
                r3 = r31 + 0xd18;
                fn_8010A420();
                r3 = r31 + 0xcd0;
                fn_8010A420();
                tmp = 0x0;
                lbl_8047A42C = tmp;
            }
        L_800323E4:
            tmp = lbl_8047A42C;
    } while ((s32)tmp > 0);
        r3 = r30;
        fn_801D0314();
        tmp = lbl_8047A40A;
        if (tmp != 0) {
            tmp = lbl_804788B0;
            if (tmp != 0) {
                r3 = 0x2;
                r4 = 0x44d0;
                r5 = 0x1;
                r6 = 0x0;
                winMsgOpen();
                r3 = 0x1;
                winMsgClose();
                goto L_80032458;
            }
            r3 = 0x2;
            r4 = 0x44f1;
            r5 = 0x1;
            r6 = 0x0;
            winMsgOpen();
            r3 = 0x1;
            winMsgClose();

        } else {
            tmp = 0x1;
            lbl_8047A40A = tmp;
        }
    L_80032458:
        r3 = 0x1;
        fn_801024E8();
        fn_800FF52C();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            fn_800FF660();
            goto L_80032484;
        }
        floorGetPrevFloorID();
        r4 = 0x0;
        floorLink();
    }
L_80032484:
    tmp = 0x1;
    lbl_8047A409 = tmp;
    return;
}
#pragma pop
#endif

/* 0x800324A0 | 0xC4 */
extern void fn_80112260(s32);
extern u32  fn_80113F48(void);
extern u32  fn_801CBA0C(u32);
extern u32  GSresGetResource(u32);
extern void cameraPlayAnime(s32, u32, s32, s32);
extern void GSscene_SetMode(s32);
extern void fn_801CB7C4(u32);
extern u8 lbl_8047A408;
extern u32 lbl_8047A42C;
extern u8 lbl_8047A409;
extern u32 lbl_8047A428;
extern u32 lbl_8047A424;
extern u32 lbl_8047A420;
extern u8 lbl_8047A410;
extern u8 lbl_8047A41C;
extern u8 lbl_8047A40A;
extern u32 lbl_8047A418;
extern u32 lbl_8047A414;
#if 0
asm void fn_800324A0(void) {
#include "src/game/gs_npc_event_fn_800324A0.inc"
}
#else
#pragma push
#pragma peephole off
void fn_800324A0(void) {
    u32 value;
    u32 handle;
    s32 minus_one;

    fn_80112260(0);
    if (lbl_8047A408 != 0) {
        lbl_8047A408 = 0;
    } else {
        lbl_8047A409 = 0;
        minus_one = -1;
        lbl_8047A42C = 1;
        lbl_8047A428 = minus_one;
        lbl_8047A424 = minus_one;
        lbl_8047A420 = minus_one;
        lbl_8047A410 = 1;
        lbl_8047A41C = 1;
        lbl_8047A40A = 1;
        handle = fn_80113F48();
        value = fn_801CBA0C(0x0FFE1000);
        lbl_8047A418 = value;
        lbl_8047A414 = GSresGetResource(handle);
        cameraPlayAnime(0x37C, 0x0FFF1800, 0, 1);
        GSscene_SetMode(4);
        fn_801CB7C4(0x10B11000);
    }
}
#pragma pop
#endif

/* 0x80032564 | 0x28 */
extern void fn_800FF730(s32);
#if 0
asm void fn_80032564(void) {
#include "src/game/gs_npc_event_fn_80032564.inc"
}
#else
#pragma peephole off
void fn_80032564(void) {
    fn_800FF730(0x394);
    _threadSwitch();
}
#pragma peephole on
#endif

/* 0x8003258C | 0x270 */
extern void fn_80166AB8(s32, s32, s32);
extern void heroAddPokecoupon(s32, s32);
extern void jumptable_802E4FE0();
extern u32 lbl_8047A450;
extern u32 lbl_8047A458;
extern u8 lbl_8047A44A;
extern u8 lbl_803A3278[];
extern u32 lbl_8047A43C;
#if 0
asm void fn_8003258C(void) {
#include "src/game/gs_npc_event_fn_8003258C.inc"
}
#else
#pragma push
#pragma peephole off
void fn_8003258C(void) {
    s32 ctr;
    s32 r31;

    if ((s32)lbl_8047A450 != -1) {
        lbl_8047A450 = lbl_8047A450 + 1;
    }
    ctr = (s32)lbl_8047A450;
    if ((u32)(ctr + 1) % 6U == 0) {
        lbl_8047A458 = 0xe;
        return;
    }
    if (lbl_8047A44A != 0) {
        lbl_8047A44A = 0;
        if ((u32)ctr < 0x1e) {
            switch (ctr - 6) {
            case 3:
                r31 = 0x4b0;
                break;
            case 12:
                r31 = 0x960;
                break;
            case 18:
                r31 = 0x12c0;
                break;
            default:
                r31 = 0x258;
                break;
            }
            fn_80166AB8(0x3cc, 0, 0);
            heroAddPokecoupon(0, r31);
            msgctrlSetValue(0x2f, r31);
            winMsgOpen(8, 0x3b60, 1, 0);
            winMsgClose(1);
        } else {
            switch (ctr) {
            case 0x1e:
                fn_80166AB8(0x3cc, 0, 0);
                heroAddPokecoupon(0, 0x2580);
                msgctrlSetValue(0x2f, 0x2580);
                winMsgOpen(8, 0x3b8c, 1, 0);
                winMsgClose(1);
                break;
            case 0x3c:
                winMsgOpen(8, 0x3b8f, 1, 0);
                winMsgClose(1);
                break;
            case 0x5a:
                winMsgOpen(8, 0x3b53, 1, 0);
                winMsgClose(1);
                break;
            case 0x78:
                winMsgOpen(8, 0x3b56, 1, 0);
                winMsgClose(1);
                break;
            case 0x96:
                winMsgOpen(8, 0x3b59, 1, 0);
                winMsgClose(1);
                break;
            }
        }
        lbl_8047A43C = 0;
        lbl_803A3278[0] = 0;
        lbl_803A3278[1] = 0;
        lbl_803A3278[2] = 0;
        lbl_803A3278[3] = 0;
        lbl_803A3278[4] = 0;
        lbl_803A3278[5] = 0;
        lbl_803A3278[6] = 0;
        lbl_803A3278[7] = 0;
        lbl_803A3278[8] = 0;
        lbl_803A3278[9] = 0;
        lbl_803A3278[0xa] = 0;
        lbl_803A3278[0xb] = 0;
        lbl_803A3278[0xc] = 0;
        lbl_803A3278[0xd] = 0;
        lbl_803A3278[0xe] = 0;
    }
    lbl_8047A458 = 5;
}
#pragma pop
#endif

/* 0x800327FC | 0x6DC */
extern u32 _fadeEffectGetRandom__FUl(u32 range);
extern void GScharCpy(void* dst, const void* src);
extern u32 pokemonGetStatus(void*, u32, u32, u32);
extern void fn_80082EA4(u32, s32, u8, u8);
extern u32 menuIsCheck(u32);
extern s32 fn_800D37CC(void);
extern u32 fn_800D3088(void);
extern u8 lbl_8047A44A;
extern u32 lbl_8047A430;
extern u32 lbl_8047A43C;
extern u8 lbl_803A3334[];
extern u32 lbl_8047A434;
extern u8 lbl_8047A448;
extern f32 lbl_8047BA00;
extern f32 lbl_8047B9F8;
extern u8 lbl_8047A449;
extern f64 lbl_8047BA08;
extern f64 lbl_8047BA10;
extern f32 lbl_8047BA18;
extern u32 lbl_8047A458;
extern u8* fn_80032ED8(s32, s32, u8*);
#if 0
asm void fn_800327FC(void) {
#include "src/game/gs_npc_event_fn_800327FC.inc"
}


#endif

typedef struct NpcEventRosterEntry {
    u8 data[0xCA];
} NpcEventRosterEntry;

typedef struct NpcEventDisplayData {
    u8 data[0xD0];
} NpcEventDisplayData;

typedef struct NpcEventEncounterData {
    u8 pad_000[0x24];
    u8 group;
    u8 pad_025;
    u8 variant;
    u8 pad_027[0x31];
    s8 candidate_count;
    u8 pad_059[2];
    s8 candidate_slots[6];
} NpcEventEncounterData;

void fn_800327FC(void)
{
    NpcEventDisplayData display;
    s32 result;
    u32 i;

    if (lbl_8047A44A != 0) {
        NpcEventRosterEntry selected_record;
        NpcEventRosterEntry* roster;
        NpcEventRosterEntry* selected;
        u32 selected_index;

        roster = (NpcEventRosterEntry*)(lbl_8047A430 + 0x41E4);
        selected = &roster[9];
        for (selected_index = 0; selected_index < 10; selected_index++) {
            NpcEventRosterEntry* candidate = &roster[selected_index];
            u16* status = (u16*)&candidate->data[6];

            if (status[0] != 0 || status[1] != 0) {
                selected_index += _fadeEffectGetRandom__FUl(10 - selected_index);
                selected = &roster[selected_index];
                break;
            }
        }

        selected_record = *selected;
        GScharCpy(&display, &selected_record.data[6]);

        display.data[0x0C] = selected_record.data[0x12];
        display.data[0x0D] = 0;
        display.data[0x0E] = 0;
        display.data[0x0F] = 0;
        display.data[0x10] = 0;
        *(u16*)&display.data[0x12] = *(u16*)&selected_record.data[0x14];
        *(u16*)&display.data[0x14] = *(u16*)&selected_record.data[0x16];
        *(u16*)&display.data[0x16] = *(u16*)&selected_record.data[0x18];
        *(u16*)&display.data[0x18] = *(u16*)&selected_record.data[0x1A];
        *(u32*)&display.data[0x1C] = selected_record.data[0x1C];
        *(u16*)&display.data[0x20] = *(u16*)&selected_record.data[0x1E];
        *(u16*)&display.data[0x22] = 0;
        display.data[0x24] = selected_record.data[0x20];

        *(u32*)&display.data[0x28] = *(u32*)&selected_record.data[0x22];
        *(u32*)&display.data[0x2C] = *(u32*)&selected_record.data[0x26];
        *(u32*)&display.data[0x30] = *(u32*)&selected_record.data[0x2A];
        *(u32*)&display.data[0x34] = *(u32*)&selected_record.data[0x2E];
        *(u32*)&display.data[0x38] = *(u32*)&selected_record.data[0x32];
        *(u32*)&display.data[0x3C] = *(u32*)&selected_record.data[0x36];
        *(u32*)&display.data[0x40] = *(u32*)&selected_record.data[0x3A];
        *(u32*)&display.data[0x44] = *(u32*)&selected_record.data[0x3E];
        *(u32*)&display.data[0x48] = *(u32*)&selected_record.data[0x42];
        *(u32*)&display.data[0x4C] = *(u32*)&selected_record.data[0x46];
        *(u32*)&display.data[0x50] = *(u32*)&selected_record.data[0x4A];
        *(u32*)&display.data[0x54] = *(u32*)&selected_record.data[0x4E];
        *(u32*)&display.data[0x58] = *(u32*)&selected_record.data[0x52];
        *(u32*)&display.data[0x5C] = *(u32*)&selected_record.data[0x56];
        *(u32*)&display.data[0x60] = *(u32*)&selected_record.data[0x5A];
        *(u32*)&display.data[0x64] = *(u32*)&selected_record.data[0x5E];
        *(u32*)&display.data[0x6A] = *(u32*)&selected_record.data[0x64];
        *(u32*)&display.data[0x6E] = *(u32*)&selected_record.data[0x68];
        *(u32*)&display.data[0x72] = *(u32*)&selected_record.data[0x6C];
        *(u32*)&display.data[0x76] = *(u32*)&selected_record.data[0x70];
        *(u32*)&display.data[0x7A] = *(u32*)&selected_record.data[0x74];
        *(u32*)&display.data[0x7E] = *(u32*)&selected_record.data[0x78];
        *(u32*)&display.data[0x82] = *(u32*)&selected_record.data[0x7C];
        *(u32*)&display.data[0x86] = *(u32*)&selected_record.data[0x80];
        *(u32*)&display.data[0x8A] = *(u32*)&selected_record.data[0x84];
        *(u32*)&display.data[0x8E] = *(u32*)&selected_record.data[0x88];
        *(u32*)&display.data[0x92] = *(u32*)&selected_record.data[0x8C];
        *(u32*)&display.data[0x96] = *(u32*)&selected_record.data[0x90];
        *(u32*)&display.data[0x9A] = *(u32*)&selected_record.data[0x94];
        *(u32*)&display.data[0x9E] = *(u32*)&selected_record.data[0x98];
        *(u32*)&display.data[0xA2] = *(u32*)&selected_record.data[0x9C];
        *(u32*)&display.data[0xA6] = *(u32*)&selected_record.data[0xA0];
        *(u32*)&display.data[0xAA] = *(u32*)&selected_record.data[0xA4];
        *(u32*)&display.data[0xAE] = *(u32*)&selected_record.data[0xA8];
        *(u32*)&display.data[0xB2] = *(u32*)&selected_record.data[0xAC];
        *(u32*)&display.data[0xB6] = *(u32*)&selected_record.data[0xB0];
        *(u32*)&display.data[0xBA] = *(u32*)&selected_record.data[0xB4];
        *(u32*)&display.data[0xBE] = *(u32*)&selected_record.data[0xB8];
        *(u32*)&display.data[0xC2] = *(u32*)&selected_record.data[0xBC];
        *(u32*)&display.data[0xC6] = *(u32*)&selected_record.data[0xC0];
        *(u32*)&display.data[0xCA] = *(u32*)&selected_record.data[0xC4];
        *(u16*)&display.data[0xCE] = *(u16*)&selected_record.data[0xC8];

        lbl_8047A43C = 0;
        lbl_803A3278[0] = 0;
        lbl_803A3278[1] = 0;
        lbl_803A3278[2] = 0;
        lbl_803A3278[3] = 0;
        lbl_803A3278[4] = 0;
        lbl_803A3278[5] = 0;
        lbl_803A3278[6] = 0;
        lbl_803A3278[7] = 0;
        lbl_803A3278[8] = 0;
        lbl_803A3278[9] = 0;
        lbl_803A3278[10] = 0;
        lbl_803A3278[11] = 0;
        lbl_803A3278[12] = 0;
        lbl_803A3278[13] = 0;
        lbl_803A3278[14] = 0;
        result = (s32)fn_80032ED8((s32)&display, 0x209,
                                  &display.data[0x28]);
    } else {
        NpcEventEncounterData* encounter;
        u32 scores[6];
        u32 highest_dark_level;
        s8 selected_index;
        u32 tie_count;
        s32 candidate_index;

        encounter = (NpcEventEncounterData*)lbl_803A3334;
        highest_dark_level = 1;
        for (i = 0; i < 6; i++) {
            void* pokemon = (void*)heroGetStatus(0, 3, i);

            if (((u32 (*)(void*))pokemonCheckValid)(pokemon) == 1) {
                u32 dark_level = pokemonGetStatus(pokemon, 0, 0x7A, 0) & 0xFF;
                if (highest_dark_level < dark_level) {
                    highest_dark_level = dark_level;
                }
            }
        }

        selected_index = 0;
        tie_count = 1;
        for (candidate_index = 0;
             candidate_index < encounter->candidate_count;
             candidate_index++) {
            s8 roster_slot;
            u8* roster_data;
            u32 highest_enemy_level;
            u32 member;

            fn_80082EA4(lbl_8047A434, candidate_index, encounter->group,
                        encounter->variant);
            roster_slot = encounter->candidate_slots[candidate_index];
            roster_data = lbl_803A3334 + roster_slot * 0x28;
            highest_enemy_level = 0;
            for (member = 0; member < 4; member++) {
                s8 pokemon_index = *(s8*)&roster_data[0x3B9 + member];
                if (pokemon_index >= 0) {
                    u8 level = lbl_803A3334[pokemon_index * 0x2A + 0x517];
                    if (highest_enemy_level < level) {
                        highest_enemy_level = level;
                    }
                }
            }

            if (highest_dark_level < highest_enemy_level) {
                scores[candidate_index] = highest_enemy_level - highest_dark_level;
            } else {
                scores[candidate_index] = highest_dark_level - highest_enemy_level;
            }
            if (scores[candidate_index] < scores[selected_index]) {
                selected_index = candidate_index;
                tie_count = 1;
            } else if (scores[candidate_index] == scores[selected_index]) {
                tie_count++;
                if (_fadeEffectGetRandom__FUl(tie_count) == 0) {
                    selected_index = candidate_index;
                }
            }
        }

        lbl_8047A448 = selected_index;
        {
            s8 roster_slot = encounter->candidate_slots[selected_index];
            u32* source = (u32*)(lbl_803A3334 + roster_slot * 0x28 + 0x3AC);
            typedef struct NpcEventSetupData {
                u32 words[10];
            } NpcEventSetupData;

            *(NpcEventSetupData*)&display = *(const NpcEventSetupData*)source;
        }
        result = (s32)fn_80032ED8((s32)&display, 0x209, 0);
    }

    if ((menuIsCheck(0xA5) & 0xFF) != 0) {
        ((void (*)(u32))menuClose)(0xA5);
    }
    ((void (*)(u32, f32))fadeSet)(2, lbl_8047BA00);
    ((s32 (*)(u32, u32))menuOpen)(0xA5, 1);
    ((void (*)(u32))fadeCheck)(1);
    lbl_8047A449 = 1;

    {
        f32 elapsed = lbl_8047B9F8;
        while (elapsed < lbl_8047BA18) {
            _threadSwitch();
            elapsed += (f32)fn_800D3088() / (f32)fn_800D37CC();
        }
    }

    lbl_8047A458 = result == 2 ? 0xD : 0xC;
}



/* 0x80032ED8 | 0x1E0 */
extern u8*  fightEncountDataBiosGetPtr(s32);
extern void fightEncountDataBiosSetTrainer(u8*, s32);
extern void fightEncountDataBiosSetFightFloorDataId(u8*, u16);
extern s32  fightTrainerDataBiosGetPtr(s32);
extern void fn_801FCB94(s32, u8);
extern void fightTrainerDataBiosSetKindDataId(s32, s32);
extern void fn_801FCB84(s32, u16);
extern void fn_801FCAFC(s32, u8);
extern void fn_800896E0(s32);
extern void fn_800896D8(s32);
extern void fn_800896D0(s32);
extern void fn_801FCB40(s32, u8, u16);
extern void fn_801FCC3C(s32);
extern u8*  fightTrainerPokemonDataBiosGetPtr(void);
extern void fightTrainerPokemonDataBiosSetPokemonDataId(s32, u16);
extern u8*  fn_801CA5C4(s32, s32, s32);
extern u32  fn_801653C4(void);
extern void fn_80165A20(s32, s32, s32);
extern u32 lbl_8047A444;
extern u8 lbl_8047A439;
extern void fn_800330B8(s32, u8*);
#if 0
asm void fn_80032ED8(void) {
#include "src/game/gs_npc_event_fn_80032ED8.inc"
}
#else
u8* fn_80032ED8(s32 arg0, s32 arg1, u8* arg2) {
    extern u8 lbl_803A3334[];
    s32 ctx;
    u8* node;
    u8* base;
    u8* slot;
    u8* tbl;
    u8* result;
    u32 sfx_id;
    s32 i;
    u8* walk;
    u8* arg2_iter;

    node = fightEncountDataBiosGetPtr(arg1);
    if (*(u32*)(arg0 + 0x1C) == 0) {
        fightEncountDataBiosSetTrainer(node, 0);
    } else {
        fightEncountDataBiosSetTrainer(node, 1);
    }
    fightEncountDataBiosSetFightFloorDataId(node, (u16)lbl_8047A444);

    ctx = fightTrainerDataBiosGetPtr(9);
    fn_801FCB94(ctx, *(u8*)(arg0 + 0xC));
    fightTrainerDataBiosSetKindDataId(ctx, 0x26);
    fn_801FCB84(ctx, *(u16*)(arg0 + 0x20));
    fn_801FCAFC(ctx, *(u8*)(arg0 + 0x24));

    fn_800896E0(arg0);
    fn_800896D8(0);
    fn_800896D0(9);

    walk = (u8*)arg0;
    for (i = 0; i < 4; i++) {
        fn_801FCB40(ctx, (u8)i, *(u16*)(walk + 0x12));
        walk += 2;
    }
    fn_801FCC3C(ctx);

    base = fightTrainerPokemonDataBiosGetPtr();
    arg2_iter = arg2;
    slot = base;
    tbl = lbl_803A3334;
    for (i = 0; i < 4; i++) {
        if (arg2 != 0) {
            fn_800330B8((s32)slot, arg2_iter);
        } else {
            fn_800330B8((s32)slot, tbl + ((s8)*(u8*)(arg0 + i + 0xD) * 0x2A + 0x514));
        }
        arg2_iter += 0x2A;
        slot += 0x50;
    }
    for (; i < 6; i++) {
        fightTrainerPokemonDataBiosSetPokemonDataId((s32)(base + i * 0x50), 0);
    }

    result = fn_801CA5C4(arg1, 1, 0);
    cameraPlayAnime((s32)fn_80113F48(), 0x11171800, 0, 0);

    if (lbl_8047A439 != 0) {
        sfx_id = 0x446;
    } else {
        sfx_id = 0x4CD;
    }
    if (sfx_id != fn_801653C4()) {
        fn_80165A20(sfx_id, 0, 0x7F);
    }
    return result;
}
#endif

#if 0
void fn_80032ED8_legacy_disabled(void) {
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r29 = r3;
    r30 = r4;
    r31 = r5;
    r3 = r30;
    fightEncountDataBiosGetPtr();
    tmp = *(u32*)((u8*)r29 + 0x1C);
    r26 = r3;
    if ((s32)tmp == 0) {
        r4 = 0x0;
        fightEncountDataBiosSetTrainer();
    } else {

        r4 = 0x1;
        fightEncountDataBiosSetTrainer();
    }
    tmp = lbl_8047A444;
    r3 = r26;
    r4 = tmp & 0xFFFF;
    fightEncountDataBiosSetFightFloorDataId();
    r3 = 0x9;
    fightTrainerDataBiosGetPtr();
    r4 = *(u8*)((u8*)r29 + 0xC);
    r27 = r3;
    fn_801FCB94();
    r3 = r27;
    r4 = 0x26;
    fightTrainerDataBiosSetKindDataId();
    r4 = *(u16*)((u8*)r29 + 0x20);
    r3 = r27;
    fn_801FCB84();
    r4 = *(u8*)((u8*)r29 + 0x24);
    r3 = r27;
    fn_801FCAFC();
    r3 = r29;
    fn_800896E0();
    r3 = 0x0;
    fn_800896D8();
    r3 = 0x9;
    fn_800896D0();
    r28 = r29;
    r26 = 0x0;
    do {
        r5 = *(u16*)((u8*)r28 + 0x12);
        r3 = r27;
        r4 = r26 & 0xFF;
        fn_801FCB40();
        r28 = r28 + 0x2;
        r26 = r26 + 0x1;
    } while ((s32)r26 < 4);
    r3 = r27;
    fn_801FCC3C();
    fightTrainerPokemonDataBiosGetPtr();
    r25 = r3;
    r27 = r31;
    r24 = 0x0;
    r26 = r25;
    r3 = (u32)lbl_803A3334;
    r28 = (u32)lbl_803A3334;
    do {
        if (r31 != 0) {
            r3 = r26;
            r4 = r27;
            fn_800330B8();
        } else {

            tmp = r24 + 0xd;
            r3 = r26;
            tmp = *(u8*)(r29 + tmp);
            tmp = (s8)tmp;
            tmp = tmp * 0x2a;
            r4 = r28 + tmp;
            r4 = r4 + 0x514;
            fn_800330B8();
        }
        r27 = r27 + 0x2a;
        r26 = r26 + 0x50;
        r24 = r24 + 0x1;
    } while ((s32)r24 < 4);
    tmp = r24 * 0x50;
    r26 = r25 + tmp;
    while ((s32)r24 < 6) {

        r3 = r26;
        r4 = 0x0;
        fightTrainerPokemonDataBiosSetPokemonDataId();
        r26 = r26 + 0x50;
        r24 = r24 + 0x1;

    }
    r3 = r30;
    r4 = 0x1;
    r5 = 0x0;
    fn_801CA5C4();
    r26 = r3;
    fn_80113F48();
    r4 = 0x11170000;
    r5 = 0x0;
    r4 = r4 + 0x1800;
    r6 = 0x0;
    cameraPlayAnime();
    tmp = lbl_8047A439;
    if (tmp != 0) {
        r28 = 0x446;
    } else {

        r28 = 0x4cd;
    }
    fn_801653C4();
    if (r28 != r3) {
        r3 = r28;
        r4 = 0x0;
        r5 = 0x7f;
        fn_80165A20();
    }
    r3 = r26;
    return;
}
#endif

/* 0x800330B8 | 0x1C0 | WALL 98.2%: loop scheduling - mr r3 placement */
extern void fightTrainerPokemonDataBiosSetNickname(s32, s32);
extern void fightTrainerPokemonDataBiosSetDarkPokemonFlag(s32, u8);
extern void fn_801EEE6C(u8, u8);
extern void fightTrainerPokemonDataBiosSetLevel(s32, u8);
extern void fightTrainerPokemonDataBiosSetWazaDataId(s32, u32, u16);
extern void fightTrainerPokemonDataBiosSetItemDataId(s32, u16);
extern void fightTrainerPokemonDataBiosSetTokuseiFlag(s32, u8);
extern void fightTrainerPokemonDataBiosSetStatusRnd(s32, s32, u8);
extern void fightTrainerPokemonDataBiosSetStatusEffort(s32, s32, s16);
extern void fightTrainerPokemonDataBiosSetFriend(s32, s16);
extern void fightTrainerPokemonDataBiosSetSexDataId(s32, u8);
extern void fightTrainerPokemonDataBiosSetSeikakuDataId(s32, u8);
extern void fightTrainerPokemonDataBiosSetKeyPlayerFlag(s32, u8);
extern void fightTrainerPokemonDataBiosSetPartDataId(s32, u8);
#if 0
asm void fn_800330B8(void) {
#include "src/game/gs_npc_event_fn_800330B8.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void fn_800330B8(s32 arg0, u8* arg1) {
    u8* walk;
    s32 i;

    fightTrainerPokemonDataBiosSetPokemonDataId(arg0, *(u16*)(arg1 + 0x0));
    fightTrainerPokemonDataBiosSetNickname(arg0, 0);
    fightTrainerPokemonDataBiosSetDarkPokemonFlag(arg0, *(u8*)(arg1 + 0x2));
    if (*(u8*)(arg1 + 0x2) != 0) {
        fn_801EEE6C(*(u8*)(arg1 + 0x2), *(u8*)(arg1 + 0x28));
    }
    fightTrainerPokemonDataBiosSetLevel(arg0, *(u8*)(arg1 + 0x3));
    walk = arg1;
    for (i = 0; i < 4; i++) {
        fightTrainerPokemonDataBiosSetWazaDataId(arg0, i & 0xFF, *(u16*)(walk + 0x4));
        walk += 2;
    }
    fightTrainerPokemonDataBiosSetItemDataId(arg0, *(u16*)(arg1 + 0xC));
    fightTrainerPokemonDataBiosSetTokuseiFlag(arg0, *(u8*)(arg1 + 0xE));
    fightTrainerPokemonDataBiosSetStatusRnd(arg0, 0, *(u8*)(arg1 + 0xF));
    fightTrainerPokemonDataBiosSetStatusRnd(arg0, 1, *(u8*)(arg1 + 0x10));
    fightTrainerPokemonDataBiosSetStatusRnd(arg0, 2, *(u8*)(arg1 + 0x11));
    fightTrainerPokemonDataBiosSetStatusRnd(arg0, 3, *(u8*)(arg1 + 0x12));
    fightTrainerPokemonDataBiosSetStatusRnd(arg0, 4, *(u8*)(arg1 + 0x13));
    fightTrainerPokemonDataBiosSetStatusRnd(arg0, 5, *(u8*)(arg1 + 0x14));
    fightTrainerPokemonDataBiosSetStatusEffort(arg0, 0, *(s16*)(arg1 + 0x16));
    fightTrainerPokemonDataBiosSetStatusEffort(arg0, 1, *(s16*)(arg1 + 0x18));
    fightTrainerPokemonDataBiosSetStatusEffort(arg0, 2, *(s16*)(arg1 + 0x1A));
    fightTrainerPokemonDataBiosSetStatusEffort(arg0, 3, *(s16*)(arg1 + 0x1C));
    fightTrainerPokemonDataBiosSetStatusEffort(arg0, 4, *(s16*)(arg1 + 0x1E));
    fightTrainerPokemonDataBiosSetStatusEffort(arg0, 5, *(s16*)(arg1 + 0x20));
    fightTrainerPokemonDataBiosSetFriend(arg0, *(s16*)(arg1 + 0x22));
    fightTrainerPokemonDataBiosSetSexDataId(arg0, *(u8*)(arg1 + 0x24));
    fightTrainerPokemonDataBiosSetSeikakuDataId(arg0, *(u8*)(arg1 + 0x25));
    fightTrainerPokemonDataBiosSetKeyPlayerFlag(arg0, *(u8*)(arg1 + 0x26));
    fightTrainerPokemonDataBiosSetPartDataId(arg0, *(u8*)(arg1 + 0x27));
    return;
}
#endif

/* ===== Phase 2 recovery stubs ===== */

/* fn_80030170 - 0x80030170 | size: 0x38 */
typedef struct GSnpcEventQueue {
    void* items[13];
    u32 idx;
} GSnpcEventQueue;

void* fn_80030170(GSnpcEventQueue* q) {
    u32 idx;
    if (q == NULL) {
        return NULL;
    }
    idx = q->idx;
    if (idx >= 13) {
        return NULL;
    }
    q->idx = idx + 1;
    return q->items[idx];
}

/* fn_800301A8 - 0x800301A8 | size: 0x4 */
#if 0
asm void fn_800301A8(void) {
#include "src/game/gs_npc_event_fn_800301A8.inc"
}
#else
void fn_800301A8(void) { }
#endif

/* fn_800301AC - 0x800301AC | size: 0x4 */
#if 0
asm void fn_800301AC(void) {
#include "src/game/gs_npc_event_fn_800301AC.inc"
}
#else
void fn_800301AC(void) { }
#endif

/* fn_800302D0 - 0x800302D0 | size: 0xa0 */
#pragma peephole off
void fn_800302D0(u8* r3, u8* r4) {
    u32 flags = *(u32*)(r4 + 0x64);
    s32 evtype = *(s16*)(r4 + 0x6);
    u32 combined = (flags & 0xa1400000) | *(u8*)(r3 + 0x8b);
    switch (evtype) {
    case 0xFBD:
        fn_800FB680(0, 0, combined, 0x4412);
        break;
    case 0xFBC:
        {
            s32 val = heroGetStatus((void*)lbl_803A2688, 1, 0);
            msgctrlSetValue(0x4D, val);
            fn_800FB680(0, 0, combined, 0x4413);
        }
        break;
    }
}
#pragma peephole on

/* fn_80030370 - 0x80030370 | size: 0xc */
#if 0
asm void fn_80030370(void) {
#include "src/game/gs_npc_event_fn_80030370.inc"
}
#else
void fn_80030370(u32 r3, u32* r4) {
    r4[0x4C / 4] = 0x43E3;
}
#endif

/* fn_8003037C - 0x8003037C | size: 0xc */
#if 0
asm void fn_8003037C(void) {
#include "src/game/gs_npc_event_fn_8003037C.inc"
}
#else
void fn_8003037C(u32 r3, u32* r4) {
    r4[0x4C / 4] = 0x43E2;
}
#endif

/* fn_80030388 - 0x80030388 | size: 0xa0 */
#if 0
asm void fn_80030388(void) {
#include "src/game/gs_npc_event_fn_80030388.inc"
}
#else
#pragma peephole off
void fn_80030388(u8* r3, u8* r4) {
    u32 flags = *(u32*)(r4 + 0x64);
    s32 evtype = *(s16*)(r4 + 0x6);
    u32 combined = (flags & 0xa1400000) | *(u8*)(r3 + 0x8b);
    switch (evtype) {
    case 0x10CB:
        fn_800FB680(0, 0, combined, 0x4412);
        break;
    case 0x10CA:
        {
            s32 val = heroGetStatus((void*)lbl_803A2688, 1, 0);
            msgctrlSetValue(0x4D, val);
            fn_800FB680(0, 0, combined, 0x4413);
        }
        break;
    }
}
#pragma peephole on
#endif

/* fn_80030428 - 0x80030428 | size: 0x4 */
#if 0
asm void fn_80030428(void) {
#include "src/game/gs_npc_event_fn_80030428.inc"
}
#else
void fn_80030428(void) { }
#endif

/* fn_8003042C - 0x8003042C | size: 0x148 */
extern void pokemonGetSoubiItemDataId(void);
extern void itemDataBiosGetPtr(void);
extern void itemDataBiosGetName(void);
extern u8 lbl_80266F68[];
extern u32 lbl_8047A424;
extern u32 lbl_8047A420;
#if 0
asm void fn_8003042C(void) {
#include "src/game/gs_npc_event_fn_8003042C.inc"
}
#else
#pragma push
#pragma peephole off
void fn_8003042C(u8* arg0, u8* arg1) {
    u32 combined;
    s32 kind;
    void* obj;

    combined = (*(u32*)(arg1 + 0x64) & ~0xFF) | *(u8*)(arg0 + 0x8B);
    kind = 0;
    if (*(s16*)(arg1 + 0x6) == *(u16*)(lbl_80266F68 + 0x12)) {
        kind = *(u16*)(lbl_80266F68 + 0x0);
    }
    if (*(s16*)(arg1 + 0x6) == *(u16*)(lbl_80266F68 + 0x26)) {
        kind = *(u16*)(lbl_80266F68 + 0x14);
    }

    obj = 0;
    if (kind == 1) {
        obj = (void*)heroGetStatus(0, 3, (u16)lbl_8047A424);
    } else if (kind == 2) {
        obj = (void*)heroGetStatus(lbl_803A2688, 3, (u16)lbl_8047A420);
    }

    if (obj != 0) {
        if (((u16 (*)(void*))pokemonGetSoubiItemDataId)(obj) != 0) {
            itemDataBiosGetPtr();
            itemDataBiosGetName();
            msgctrlSetValue(0x37, ((s32 (*)(void))GSmsgGetGSchar)());
            fn_800FB680(0, 0, combined, 0xE7);
            *(u32*)(arg1 + 0x4C) = 0;
            winSpriteSetDisp(arg1, 1);
        } else {
            winSpriteSetDisp(arg1, 0);
        }
    } else {
        winSpriteSetDisp(arg1, 0);
    }
}
#pragma pop
#endif

/* fn_80030574 - 0x80030574 | size: 0x234 */
extern void pokemonWazaCheckValid(void);
extern void wazaGetStatus(void);
extern void fn_800FBB34(s32, s32, s32, s32, u32, u16);
extern u32 lbl_8047A424;
extern u32 lbl_8047A420;
#if 0
asm void fn_80030574(void) {
#include "src/game/gs_npc_event_fn_80030574.inc"
}
#else
#pragma push
#pragma peephole off
void fn_80030574(u8* arg0, u8* arg1) {
    u8* table;
    u32 combined;
    s32 group;
    s32 slot;
    s32 key;
    u32 value;
    void* obj;

    combined = (*(u32*)(arg1 + 0x64) & ~0xFF) | *(u8*)(arg0 + 0x8B);
    table = lbl_80266F68;
    group = 0;
    slot = 0;
    key = *(s16*)(arg1 + 0x6);

    {
        s32 i;
        for (i = 0; i < 2; i++) {
            key = *(s16*)(arg1 + 0x6);
            if (key == *(u16*)(table + 0x8)) {
                group = *(u16*)(table + 0x0);
                slot = 0;
            }
            if (key == *(u16*)(table + 0xA)) {
                group = *(u16*)(table + 0x0);
                slot = 1;
            }
            if (key == *(u16*)(table + 0xC)) {
                group = *(u16*)(table + 0x0);
                slot = 2;
            }
            if (key == *(u16*)(table + 0xE)) {
                group = *(u16*)(table + 0x0);
                slot = 3;
            }
            table += 0x14;
        }
    }

    obj = 0;
    if (group == 2) goto grp2;
    if (group >= 2) goto grp_done;
    if (group >= 1) goto grp1;
    goto grp_done;
grp1:
    obj = (void*)heroGetStatus(0, 3, (u16)lbl_8047A424);
    goto grp_done;
grp2:
    obj = (void*)heroGetStatus(lbl_803A2688, 3, (u16)lbl_8047A420);
grp_done:
    ;

    /* Retail groups all three winSpriteSetDisp calls at the end: the obj == 0
     * body is out of line, after the if/else, not inline at the test. */
    if (obj == 0) {
        goto no_object;
    }

    value = ((u16 (*)(void*, s32, s32, s32))pokemonGetStatus)(obj, 0, 0x7F, (u16)slot);
    if (((u8 (*)(void*, s32))pokemonWazaCheckValid)(obj, (u16)slot) == 0) {
        value = 0;
    }

    value = (u16)value;
    switch (value) {
    case 0xFFFF:
        value = 0x933;
        break;
    case 0xFFFE:
        value = 0x934;
        break;
    case 0:
        break;
    default:
        value = ((u32 (*)(s32, u32, s32, s32))wazaGetStatus)(0, value, 1, 0);
        break;
    }

    if (value != 0) {
        msgctrlSetValue(0x37, ((s32 (*)(u32))GSmsgGetGSchar)(value));
        fn_800FBB34(0, 0, *(s16*)(arg1 + 0x54), *(s16*)(arg1 + 0x56), combined, 0xE7);
        winSpriteSetDisp(arg1, 1);
    } else {
        winSpriteSetDisp(arg1, 0);
    }
    return;

no_object:
    winSpriteSetDisp(arg1, 0);
}
#pragma pop
#endif

/* fn_800307A8 - 0x800307A8 | size: 0x12c */
extern u32 lbl_8047A424;
extern u32 lbl_8047A420;
#if 0
asm void fn_800307A8(void) {
#include "src/game/gs_npc_event_fn_800307A8.inc"
}
#else
#pragma push
#pragma peephole off
void fn_800307A8(u8* arg0, u8* arg1) {
    u32 combined;
    s32 kind;
    void* obj;

    combined = (*(u32*)(arg1 + 0x64) & ~0xFF) | *(u8*)(arg0 + 0x8B);
    kind = 0;
    if (*(s16*)(arg1 + 0x6) == *(u16*)(lbl_80266F68 + 0x2)) {
        kind = *(u16*)(lbl_80266F68 + 0x0);
    }
    if (*(s16*)(arg1 + 0x6) == *(u16*)(lbl_80266F68 + 0x16)) {
        kind = *(u16*)(lbl_80266F68 + 0x14);
    }

    obj = 0;
    if (kind == 1) {
        obj = (void*)heroGetStatus(0, 3, (u16)lbl_8047A424);
    } else if (kind == 2) {
        obj = (void*)heroGetStatus(lbl_803A2688, 3, (u16)lbl_8047A420);
    }

    if (obj != 0) {
        msgctrlSetValue(0x2F, ((u8 (*)(void*, s32, s32, s32))pokemonGetStatus)(obj, 0, 0x7A, 0));
        fn_800FBB34(0, 0, *(s16*)(arg1 + 0x54), *(s16*)(arg1 + 0x56), combined, 0x4414);
        winSpriteSetDisp(arg1, 1);
    } else {
        winSpriteSetDisp(arg1, 0);
    }
}
#pragma pop
#endif

/* fn_800308D4 - 0x800308D4 | size: 0x170 */
extern void pokemonDataBiosGetPtr(void);
extern void pokemonDataBiosGetName(void);
extern u32 lbl_8047A424;
extern u32 lbl_8047A420;
#if 0
asm void fn_800308D4(void) {
#include "src/game/gs_npc_event_fn_800308D4.inc"
}
#else
#pragma push
#pragma peephole off
void fn_800308D4(u8* arg0, u8* arg1) {
    extern u16 pokemonGetStatus(void* obj, s32 a, s32 b, s32 c);
    extern s32 pokemonDataBiosGetPtr(s32 v);
    extern s32 pokemonDataBiosGetName(s32 v);
    extern s32 GSmsgGetRect(s32 m);
    extern s32 GSmsgGetGSchar(s32 m);
    s32 hi;
    s32 kind;
    u32 combined;
    void* obj;
    u8* tbl;

    obj = 0;
    kind = 0;
    combined = (*(u32*)(arg1 + 0x64) & ~0xFF) | *(u8*)(arg0 + 0x8B);
    tbl = lbl_80266F68;
    if (*(s16*)(arg1 + 0x6) == *(u16*)(tbl + 0x6)) {
        kind = *(u16*)(tbl + 0x0);
    }
    tbl += 0x14;
    if (*(s16*)(arg1 + 0x6) == *(u16*)(tbl + 0x6)) {
        kind = *(u16*)(tbl + 0x0);
    }

    switch (kind) {
    case 1:
        obj = (void*)heroGetStatus(0, 3, (u16)lbl_8047A424);
        break;
    case 2:
        obj = (void*)heroGetStatus(lbl_803A2688, 3, (u16)lbl_8047A420);
        break;
    }

    if (obj != 0) {
        hi = (s16)((u32)GSmsgGetRect(0x2BD4) >> 16);
        fn_800FB680(0, 0, combined, 0x2BD4);
        msgctrlSetValue(0x37, GSmsgGetGSchar(pokemonDataBiosGetName(pokemonDataBiosGetPtr(pokemonGetStatus(obj, 0, 0x6E, 0)))));
        fn_800FB680(hi, 0, combined, 0xE7);
        *(u32*)(arg1 + 0x4C) = 0;
        winSpriteSetDisp(arg1, 1);
    } else {
        winSpriteSetDisp(arg1, 0);
    }
}
#pragma pop
#endif

/* fn_80030A44 - 0x80030A44 | size: 0x1d0 */
extern u32 lbl_8047A424;
extern u32 lbl_8047A420;
#if 0
asm void fn_80030A44(void) {
#include "src/game/gs_npc_event_fn_80030A44.inc"
}
#else
#pragma push
#pragma peephole off
void fn_80030A44(u8* arg0, u8* arg1) {
    u8* table;
    u32 combined;
    s32 group;
    s32 msg;
    s32 x;
    void* obj;

    combined = (*(u32*)(arg1 + 0x64) & ~0xFF) | *(u8*)(arg0 + 0x8B);
    table = lbl_80266F68;
    group = 0;

    if (*(s16*)(arg1 + 0x6) == *(u16*)(table + 0x4)) {
        group = *(u16*)(table + 0x0);
    }
    table += 0x14;
    if (*(s16*)(arg1 + 0x6) == *(u16*)(table + 0x4)) {
        group = *(u16*)(table + 0x0);
    }

    obj = 0;
    if (group == 2) goto grp2;
    if (group >= 2) goto grp_done;
    if (group >= 1) goto grp1;
    goto grp_done;
grp1:
    obj = (void*)heroGetStatus(0, 3, (u16)lbl_8047A424);
    goto grp_done;
grp2:
    obj = (void*)heroGetStatus(lbl_803A2688, 3, (u16)lbl_8047A420);
grp_done:
    ;

    /* As in fn_80030574: retail puts the obj == 0 body last, out of line. */
    if (obj == 0) {
        goto no_object_30A44;
    }

    msgctrlSetValue(0x37, ((s32 (*)(void*))pokemonBiosGetNicknamePtr)(obj));
    fn_800FB680(0, 0, combined, 0xE7);
    winSpriteSetDisp(arg1, 1);

    if (((u8 (*)(void*))pokemonCheckValid)(obj) != 0 &&
        ((u8 (*)(void*))menuCBRule_CheckPokemonEventFlag)(obj) == 1) {
        switch (((u8 (*)(void*))menuSubGetPokemonSexForDisp)(obj)) {
        case 0:
            msg = 0xD67;
            break;
        case 1:
            msg = 0xD68;
            break;
        default:
            msg = 0;
            break;
        }

        if (msg != 0) {
            x = (s16)(((u32 (*)(s32))GSmsgGetRect)(0xE7) >> 16);
            msgctrlSetValue(0x37, ((s32 (*)(s32))GSmsgGetGSchar)(msg));
            fn_800FB680((u16)x + 2, 0, combined, 0xCF);
        }
    }
    return;

no_object_30A44:
    winSpriteSetDisp(arg1, 0);
}
#pragma pop
#endif

/* fn_80030C14 - 0x80030C14 | size: 0x120 */
extern u8 lbl_803A3230[];
extern u8 lbl_803A31E8[];
extern u8 lbl_80314F98[];
extern f32 lbl_8047B9D4;
extern f32 lbl_8047B9F0;
#if 0
asm void fn_80030C14(void) {
#include "src/game/gs_npc_event_fn_80030C14.inc"
}
#else
#pragma peephole off
void fn_80030C14(void* r3, u8* r4) {
    void* model = NULL;
    s32 evtype = *(s16*)(r4 + 0x6);
    switch (evtype) {
    case 0x10CC:
        model = menuModelRender(lbl_803A3230);
        break;
    case 0x10CD:
        model = menuModelRender(lbl_803A31E8);
        break;
    }
    if (model != NULL) {
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D6A00(7);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, model);
        fn_800D67BC(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047B9D4, lbl_8047B9D4);
        fn_800D61E4(*(s16*)(r4 + 0x54), *(s16*)(r4 + 0x56));
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047B9F0, lbl_8047B9F0);
        fn_800D6728();
    }
}
#pragma peephole on
#endif

/* fn_80030D34 - 0x80030D34 | size: 0x1d8 */
extern void pokemonBiosGetItemDataId(void);
#if 0
asm void fn_80030D34(void) {
#include "src/game/gs_npc_event_fn_80030D34.inc"
}
#else
#pragma push
#pragma peephole off
void fn_80030D34(u8* arg0, u8* arg1) {
    u8* table;
    s32 kind;
    s32 arg;
    s32 i;
    s32 side;
    void* obj;

    table = lbl_80266E90;
    kind = 0;
    arg = 0;
    for (i = 0; i < 12; i++) {
        if (*(s16*)(arg1 + 0x6) == *(u16*)(table + 0x8)) {
            kind = *(u8*)(table + 0x0);
            arg = *(u8*)(table + 0x1);
        }
        table += 0x12;
    }

    obj = 0;
    if (kind == 1) {
        obj = (void*)heroGetStatus(0, 3, (u16)arg);
    } else if (kind == 2) {
        obj = (void*)heroGetStatus(lbl_803A2688, 3, (u16)arg);
    }

    if (((u8 (*)(void*))pokemonBiosGetFuseiFlag)(obj) != 0) {
        side = ((u16 (*)(void*))pokemonBiosGetItemDataId)(obj);
        if (side != 0) {
            winSpriteSetDisp(arg1, 1);
        } else {
            winSpriteSetDisp(arg1, 0);
        }
    } else if (((u8 (*)(void*))pokemonCheckValid)(obj) != 0 &&
               ((u8 (*)(void))menuCBRule_CheckPokemonEventFlag)() == 1) {
        side = ((u16 (*)(void*))pokemonBiosGetItemDataId)(obj);
        if (side != 0) {
            winSpriteSetDisp(arg1, 1);
        } else {
            winSpriteSetDisp(arg1, 0);
        }
    } else {
        winSpriteSetDisp(arg1, 0);
    }
}
#pragma pop
#endif

/* fn_80030F0C - 0x80030F0C | size: 0x27c */
extern void pokemonBiosGetDarkFlag(void);
#if 0
asm void fn_80030F0C(void) {
#include "src/game/gs_npc_event_fn_80030F0C.inc"
}
#else
#pragma push
#pragma peephole off
void fn_80030F0C(u8* arg0, u8* arg1) {
    u8* table;
    s32 group;
    s32 arg;
    s32 side;
    s32 i;
    void* obj;

    table = lbl_80266E90;
    group = 0;
    arg = 0;
    side = 0;
    for (i = 0; i < 12; i++) {
        if (*(s16*)(arg1 + 0x6) == *(u16*)(table + 0x4)) {
            group = *(u8*)(table + 0x0);
            arg = *(u8*)(table + 0x1);
            side = 1;
        } else if (*(s16*)(arg1 + 0x6) == *(u16*)(table + 0x6)) {
            group = *(u8*)(table + 0x0);
            arg = *(u8*)(table + 0x1);
            side = 2;
        }
        table += 0x12;
    }

    obj = 0;
    if (group == 1) {
        obj = (void*)heroGetStatus(0, 3, (u16)arg);
    } else if (group == 2) {
        obj = (void*)heroGetStatus(lbl_803A2688, 3, (u16)arg);
    }

    if (((u8 (*)(void*))pokemonBiosGetFuseiFlag)(obj) != 0) {
        if (((u8 (*)(void*))pokemonBiosGetDarkFlag)(obj) == 1) {
            winSpriteSetDisp(arg1, side == 2);
        } else {
            winSpriteSetDisp(arg1, side == 1);
        }
    } else if (((u8 (*)(void*))pokemonCheckValid)(obj) != 0 &&
               ((u8 (*)(void*))menuCBRule_CheckPokemonEventFlag)(obj) == 1) {
        if (((u8 (*)(void*))pokemonBiosGetDarkFlag)(obj) == 1) {
            winSpriteSetDisp(arg1, side == 2);
        } else {
            winSpriteSetDisp(arg1, side == 1);
        }
    } else {
        winSpriteSetDisp(arg1, 0);
    }
}
#pragma pop
#endif

/* fn_800347B8 - 0x800347B8 | size: 0xc */
extern u32 lbl_8047A440;
#if 0
asm void fn_800347B8(void) {
#include "src/game/gs_npc_event_fn_800347B8.inc"
}
#else
void fn_800347B8(void) {
    *(u8*)&lbl_8047A440 = 1;
}
#endif

/* fn_800347C4 - 0x800347C4 | size: 0x24 */
extern void fn_80166A28(s32);
#if 0
asm void fn_800347C4(void) {
#include "src/game/gs_npc_event_fn_800347C4.inc"
}
#else
#pragma push
#pragma scheduling off
void fn_800347C4(void) {
    fn_80166A28(0x26);
}
#pragma pop
#endif

/* fn_800347E8 - 0x800347E8 | size: 0x24 */
#if 0
asm void fn_800347E8(void) {
#include "src/game/gs_npc_event_fn_800347E8.inc"
}
#else
#pragma push
#pragma scheduling off
void fn_800347E8(void) {
    fn_80166A28(0x26);
}
#pragma pop
#endif

/* fn_8003480C - 0x8003480C | size: 0x24 */
#if 0
asm void fn_8003480C(void) {
#include "src/game/gs_npc_event_fn_8003480C.inc"
}
#else
#pragma push
#pragma scheduling off
void fn_8003480C(void) {
    fn_80166A28(0x26);
}
#pragma pop
#endif

/* fn_80034DC0 - 0x80034DC0 | size: 0x78 | WALL 80%: oris+ori constant build pattern */
extern u32 lbl_8047A44C;
#if 0
asm void fn_80034DC0(void) {
#include "src/game/gs_npc_event_fn_80034DC0.inc"
}
#else
#pragma push
#pragma peephole off
void fn_80034DC0(u8* arg0, u8* arg1) {
    u32 byte;
    s32 mask;
    s32 combined;

    byte = *(u8*)(arg0 + 0x8B);
    mask = -0x100;
    combined = byte | mask;
    fn_800FB680(0, 0, combined, 0x3CC8);
    msgctrlSetValue(0x34, lbl_8047A44C);
    fn_800FBB34(0, 0, *(s16*)(arg1 + 0x54), *(s16*)(arg1 + 0x56), combined, 0x3CC9);
}
#pragma pop
#endif

/* fn_800301B0 - 0x800301B0 | size: 0x120 */
void fn_800301B0(void* r3, u8* r4) {
    void* model = NULL;
    s32 evtype = *(s16*)(r4 + 0x6);
    switch (evtype) {
    case 0x10CE:
        model = menuModelRender(lbl_803A3230);
        break;
    case 0x10CF:
        model = menuModelRender(lbl_803A31E8);
        break;
    }
    if (model != NULL) {
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D6A00(7);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, model);
        fn_800D67BC(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047B9D4, lbl_8047B9D4);
        fn_800D61E4(*(s16*)(r4 + 0x54), *(s16*)(r4 + 0x56));
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047B9F0, lbl_8047B9F0);
        fn_800D6728();
    }
}
