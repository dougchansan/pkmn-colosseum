/**
 * @file gs_task.c
 * @brief GStask -- Scene/FSYS task management and archive loading callbacks.
 *
 * Address range: 0x80006630 - 0x80009178 (~45 functions)
 *
 * This module manages the task system that sits between the main loop and
 * the individual game subsystems. It handles:
 *   - FSYS archive loading via task callbacks (fn_801FB1C0)
 *   - Scene transition sequencing with fn_801026A4 dispatch
 *   - Archive completion callbacks with scene ID routing
 *   - Resource group management (models, textures, scripts)
 *
 * The functions follow a clear pattern:
 *   1. Load an SDA global (lbl_8047A288) for the current scene/map ID
 *   2. Call fn_801FB1C0 to initiate an archive load with a priority level
 *   3. On success, call fn_80106394 and fn_80106080 to activate the loaded data
 *   4. Return 1 on success, -1 on failure
 *
 * Key functions:
 *   fn_80006630  GStask_InitCamera         -- calls GSscene_CameraSetPosition
 *   fn_80006654  GStask_LoadTopMenu         -- loads archive slot 6 (topmenu)
 *   fn_800066C4  GStask_SetSceneType        -- dispatches to GSscene_SetMode based on mode
 *   fn_80006724  GStask_LoadPDAMenu         -- loads archive slot 6, priority 3 (pda_menu)
 *   fn_8000677C  GStask_LoadPocketMenu      -- loads archive slot 6, priority 2 (pocket_menu)
 *   fn_800067D4  GStask_LoadPCBoxMenu       -- loads archive slot 6, priority 1 (pcbox_menu)
 *   fn_8000682C  GStask_LoadBattleMenu      -- loads archive slot 6, priority 0 (battle)
 *   fn_80006884  GStask_SelectRandomNPC     -- random NPC selection from scene data
 *   fn_80006908  GStask_ProcessSceneEvent   -- 0x6A4 bytes, large scene event dispatcher
 *   fn_80006FAC  GStask_ProcessEventResult  -- handles event return codes
 *   fn_80007088  GStask_GetField1           -- struct field accessor (offset 0x00)
 *   fn_800070CC  GStask_GetField2           -- struct field accessor (offset 0x04)
 *   fn_80007110  GStask_GetField3           -- struct field accessor (offset 0x08)
 *   fn_80007154  GStask_LoadSlot0           -- archive load helper, slot 0
 *   fn_800071AC  GStask_LoadSlot1           -- archive load helper, slot 1
 *   fn_80007204  GStask_LoadSlot2           -- archive load helper, slot 2
 *   fn_8000725C  GStask_LoadSlot3           -- archive load helper, slot 3
 *   fn_800072B4  GStask_LoadSlot4           -- archive load helper, slot 4
 *   fn_8000730C  GStask_LoadSlot5           -- archive load helper, slot 5
 *   _dbgMenuFightFightPokemonSelectSub  GStask_InitSceneResources  -- 0x2F8 bytes, resource init
 *   fn_8000765C  GStask_ShutdownResources   -- resource cleanup
 *   fn_80007708  GStask_UpdateLoadState     -- resource loading state machine
 *   fn_80007778  GStask_GetLoadFlag0        -- small accessor (0x20 bytes)
 *   fn_80007798  GStask_GetLoadFlag1        -- small accessor
 *   fn_800077B8  GStask_GetLoadFlag2        -- small accessor
 *   fn_800077D8  GStask_GetLoadFlag3        -- small accessor
 *   fn_800077F8  GStask_ReturnZero          -- stub, returns 0
 *   fn_80007800  GStask_GetLoadFlag4        -- small accessor
 *   fn_80007820  GStask_GetLoadFlag5        -- small accessor
 *   fn_80007840  GStask_ReturnZero2         -- stub, returns 0
 *   fn_80007848  GStask_ValidateScene       -- validates scene index bounds
 *   fn_800078EC  GStask_ArchiveCB0          -- archive completion callback
 *   fn_80007944  GStask_ArchiveCB1          -- archive completion callback
 *   fn_8000799C  GStask_ArchiveCB2          -- archive completion callback
 *   fn_800079F4  GStask_ArchiveCB3          -- archive completion callback
 *   fn_80007A4C  GStask_ArchiveCB4          -- archive completion callback
 *   fn_80007A84  GStask_DispatchArchiveCBs  -- routes to the above callbacks
 *   fn_80007B30  GStask_MainUpdate          -- 0x4AC bytes, main task state machine
 *   fn_80007FDC  GStask_GetSceneCount       -- return scene table size
 *   fn_80008014  GStask_GetEventCount       -- return event table size
 *   fn_8000804C  GStask_SetupTransition     -- scene transition setup (0xF8 bytes)
 *   fn_80008144  GStask_Accessor0           -- 8-byte accessor (lwz/blr)
 *   fn_8000814C  GStask_Accessor1           -- 8-byte accessor
 *   fn_80008154  GStask_Accessor2           -- 8-byte accessor
 *   fn_8000815C  GStask_Accessor3           -- 8-byte accessor
 *   fn_80008164  GStask_Accessor4           -- 8-byte accessor
 *   fn_8000816C  GStask_Accessor5           -- 8-byte accessor
 *   fn_80008174  GStask_Accessor6           -- 8-byte accessor
 *   fn_8000817C  GStask_Accessor7           -- 8-byte accessor
 *   fn_80008184  GStask_FinalizeLoad        -- 0xC0 bytes, finalize resource load
 *   _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub  GStask_BuildResourceList   -- 0x14C bytes, compile resource table
 *
 * Code patterns:
 *   - SDA globals lbl_8047A288 (current scene halfword), lbl_80478F50 (scene table ptr)
 *   - fn_801026A4 is the main event dispatch (takes slot, params, flags)
 *   - fn_80102568 is the event cancel/cleanup function
 *   - fn_8001E304 is a bounded random selection helper
 *   - fn_8001E200 is a random result commit function
 */

#include "dolphin/types.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

/* Scene system */
extern void  fn_801794F0(void);                   /* GSscene_CameraSetPosition */
extern void  GSscene_SetMode(s32 mode);               /* GSscene_SetMode */

/* FSYS archive loading */
extern void* fn_801FB1C0(s32 slot, u16 sceneId, s32 priority, s32 group);
extern s32   fn_80106394(s32 slot, s32 active);
extern s32   fn_80106080(s32 slot);

/* Event dispatch */
extern u8    fn_80102620(s32 slot);               /* Check if event slot busy */
extern s32   fn_80102510(s32 slot);               /* Cancel active event */
extern s32   fn_801026A4(s32 slot, ...);           /* Dispatch scene event */
extern void  fn_80102568(s32 slot, s32 p1, s32 p2);  /* Event cleanup */
extern s32   fn_801022B8(s32 slot);               /* Get event result code */
extern s32   fn_8010264C(s32 slot, s32 p1);       /* Query event state */
extern void  fn_80102868(s32 slot, s32 p1, s32 p2);  /* Set event params */

/* Resource management */
extern void* fn_8020E0F8(void);                   /* Get scene resource table */
extern void  fn_8020DFB0(void* tbl, u8 idx);      /* Set resource field 0 */
extern void  fn_8020DFA0(void* tbl, u8 idx);      /* Set resource field 1 */
extern void  fn_8020DF90(void* tbl, u16 val);     /* Set resource field 2 */
extern void  fn_8020E068(void* tbl, s32 slot);    /* Get resource subfield */
extern void  fn_8020DF50(void* tbl, s32 slot, u16 val);  /* Set resource subfield */
extern void  fn_8020DF10(void* tbl, s32 slot, s32 val);  /* Set resource property */
extern u8    fn_8020E0E0(void* tbl);              /* Get resource type */
extern u8    fn_8020E0C8(void* tbl);              /* Get resource subtype */
extern u16   fn_8020E0B0(void* tbl);              /* Get resource size */
extern u16   fn_8020E020(void* tbl, s32 slot);    /* Get resource slot value */

/* Random selection helpers */
extern u8    fn_8001E304(u16 count, u32* outResult, void* validationFn);
extern void  fn_8001E200(void);

/* Callback function pointers used in validation */
extern void* fn_80008744(u32 id);   /* Validate NPC index (type byte) */
extern void* fn_800086EC(u32 id);   /* Validate NPC index (type byte) */
extern void* fn_8000879C(u32 id);   /* Validate NPC index (type u16) */
extern void* fn_8000868C(u32 id);   /* Validate scene item index */
extern void* fn_800085D8(s32 difficulty);   /* Validate encounter difficulty */

/* =========================================================================
 * SDA global variables
 * ========================================================================= */

extern u16  gCurrentSceneId;     /* lbl_8047A288 : .sbss -- current scene/map halfword */
extern void* gSceneTable;       /* lbl_80478F50 : .sbss -- scene definition table ptr */
extern void* gSceneItemTable;   /* lbl_80478F20 : .sbss -- scene item lookup table ptr */
extern void* gNPCIndexTable;    /* lbl_80478F40 : .sbss -- NPC type lookup table ptr */
extern void* gNPCSubTable;      /* lbl_80478F48 : .sbss -- NPC subtype lookup table ptr */
extern void* gEncounterTable;   /* lbl_80478F00 : .sbss -- encounter difficulty table ptr */

/* =========================================================================
 * Rodata references
 * ========================================================================= */

/* lbl_802666B0: Shift-JIS encoded scene name string (unknown use) */
/* lbl_802666E0: Scene type mapping table (8 entries, 2x u32 each) */
/* lbl_80266700: Scene resource ID mapping table (14 entries) */
/* jumptable_802E28D0: Jump table for fn_80006908 scene event dispatch */

/* =========================================================================
 * Function: GStask_InitCamera
 * Address:  0x80006630
 * Size:     0x24
 *
 * Simple wrapper that calls GSscene_CameraSetPosition to reset the
 * camera, then returns 0. Called at the start of scene transitions.
 * ========================================================================= */
s32 GStask_InitCamera(void) {
    fn_801794F0();
    return 0;
}

/* =========================================================================
 * Function: GStask_LoadTopMenu
 * Address:  0x80006654
 * Size:     0x70
 *
 * Checks if archive slot 6 is busy; if so, cancels it and returns 0.
 * Otherwise, dispatches a new archive load for the top menu FSYS with
 * default parameters and sets up the viewport at (0x14, 0x104).
 * ========================================================================= */
s32 GStask_LoadTopMenu(void) {
    if (fn_80102620(6) != 0) {
        fn_80102510(6);
    } else {
        fn_801026A4(6, 0, 0, 0, 1, 0);
        fn_80102868(6, 0x14, 0x104);
    }
    return 0;
}

/* =========================================================================
 * Function: GStask_SetSceneType
 * Address:  0x800066C4
 * Size:     0x60
 *
 * Given a mode parameter in r4, dispatches to GSscene_SetMode with the
 * appropriate scene type constant:
 *   mode 0 or >=3: type 0 (normal overworld)
 *   mode 1:        type 5 (battle arena)
 *   mode 2:        type 6 (cutscene)
 * Always returns 0.
 * ========================================================================= */
s32 GStask_SetSceneType(void* unused, s32 mode) {
    if (mode == 1) {
        GSscene_SetMode(5);
    } else if (mode == 2) {
        GSscene_SetMode(6);
    } else {
        GSscene_SetMode(0);
    }
    return 0;
}

/* =========================================================================
 * Function: GStask_LoadPDAMenu
 * Address:  0x80006724
 * Size:     0x58
 *
 * Loads the PDA menu archive (priority 3) into the FSYS system.
 * Uses the current scene ID from lbl_8047A288 as a context parameter.
 * On success, activates the archive and returns 1; on failure returns -1.
 * ========================================================================= */
s32 GStask_LoadPDAMenu(void) {
    void* result;

    result = fn_801FB1C0(0, gCurrentSceneId, 8, 3);
    if (result == NULL) {
        return -1;
    }
    fn_80106394(1, 1);
    fn_80106080(1);
    return 1;
}

/* =========================================================================
 * Function: GStask_LoadPocketMenu
 * Address:  0x8000677C
 * Size:     0x58
 *
 * Loads the pocket/bag menu archive (priority 2).
 * Same structure as GStask_LoadPDAMenu but with priority 2.
 * ========================================================================= */
s32 GStask_LoadPocketMenu(void) {
    void* result;

    result = fn_801FB1C0(0, gCurrentSceneId, 8, 2);
    if (result == NULL) {
        return -1;
    }
    fn_80106394(1, 1);
    fn_80106080(1);
    return 1;
}

/* =========================================================================
 * Function: GStask_LoadPCBoxMenu
 * Address:  0x800067D4
 * Size:     0x58
 *
 * Loads the PC box menu archive (priority 1).
 * ========================================================================= */
s32 GStask_LoadPCBoxMenu(void) {
    void* result;

    result = fn_801FB1C0(0, gCurrentSceneId, 8, 1);
    if (result == NULL) {
        return -1;
    }
    fn_80106394(1, 1);
    fn_80106080(1);
    return 1;
}

/* =========================================================================
 * Function: GStask_LoadBattleMenu
 * Address:  0x8000682C
 * Size:     0x58
 *
 * Loads the battle menu archive (priority 0 -- highest).
 * ========================================================================= */
s32 GStask_LoadBattleMenu(void) {
    void* result;

    result = fn_801FB1C0(0, gCurrentSceneId, 8, 0);
    if (result == NULL) {
        return -1;
    }
    fn_80106394(1, 1);
    fn_80106080(1);
    return 1;
}

/* =========================================================================
 * Remaining functions in this module (0x80006884 - 0x80009178) are not
 * fully decompiled here. They follow the same patterns:
 *
 * fn_80006884 (GStask_SelectRandomNPC):
 *   Uses fn_8001E304 to randomly select an NPC from the scene table,
 *   bounds-checks against lbl_80478F20, stores result in lbl_8047A288.
 *
 * fn_80006908 (GStask_ProcessSceneEvent):
 *   The largest function (0x6A4 bytes) - a massive switch statement that
 *   dispatches scene events based on event codes returned by fn_801022B8.
 *   Uses jumptable_802E28D0 for the initial dispatch, then performs
 *   NPC selection, resource loading, and encounter setup based on the
 *   event type. Each case in the switch:
 *     - Gets a resource from the scene table via fn_8020E0E0/fn_8020E0C8
 *     - Validates the result using fn_8001E304 with a callback
 *     - Commits the selection via fn_8001E200
 *     - Updates the resource table via fn_8020DFB0/fn_8020DFA0/etc.
 *
 * fn_80006FAC - fn_80007110 (accessor cluster):
 *   Small field accessors for the scene resource structure.
 *
 * fn_80007154 - fn_8000730C (archive load helpers):
 *   Six nearly identical functions that load archives for slots 0-5,
 *   each following the same load/activate/commit pattern.
 *
 * _dbgMenuFightFightPokemonSelectSub (GStask_InitSceneResources):
 *   0x2F8 bytes. Initializes the resource table for a new scene,
 *   setting up all model/texture/script entries.
 *
 * fn_80007B30 (GStask_MainUpdate):
 *   0x4AC bytes. The main task update state machine that coordinates
 *   archive loading, scene transitions, and event processing.
 *
 * fn_80008184 - _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub (finalization):
 *   Finalize loaded resources and build the active resource list.
 * ========================================================================= */

/* Accessor functions (0x80008390 - 0x80009178) follow a repeating pattern:
 *
 * fn_80008390 (size 0x6C) - Get party slot 0 species
 * fn_800083FC (size 0x64) - Get party slot 0 level
 * fn_80008460 (size 0x60) - Get party slot 0 HP
 * fn_800084C0 (size 0x58) - Get party slot 0 status
 * fn_80008518 (size 0x64) - Get party slot 1 species
 * ...and so on for each party slot property
 *
 * These are thin wrappers that index into the party data structure,
 * extract a field, and return it. They are likely used as callback
 * function pointers passed to the UI rendering system.
 */

/* ===================================================================
 * Lookup/accessor functions
 * =================================================================== */

/* External functions used by lookup helpers */
extern void* fn_800FA280(u32 id);
extern void* fn_8010C4D4(u16 index);
extern void* fn_8020DED8(void* ptr);

/* Global state references */
extern u32 lbl_80478B38;

/* =======================================================================
 * fn_8000857C -- GStask_LookupResourceById
 *
 * Looks up a resource by ID. If ID is 9, uses a fixed constant.
 * If ID >= the global count (lbl_80478B38), also uses the constant.
 * Otherwise, converts via fn_8010C4D4 and passes to fn_800FA280.
 *
 * Address: 0x8000857C  Size: 0x5C (92 bytes)
 * ======================================================================= */
#pragma push
#pragma peephole off
void* fn_8000857C(u32 id)
{
    if (id == 9) {
        return fn_800FA280(0x0000EB63);
    }
    if (id >= lbl_80478B38) {
        return fn_800FA280(0x0000EB63);
    }
    {
        u32 idx;
        idx = (u16)id;
        return fn_800FA280((u32)fn_8010C4D4(idx));
    }
}
#pragma pop

/* =======================================================================
 * fn_800087FC -- GStask_LookupEventById
 *
 * Looks up an event resource by ID. Returns NULL-equivalent constant
 * on invalid input or out-of-range. Otherwise, chains through
 * fn_8020E0F8 -> fn_8020DED8 -> fn_800FA280.
 *
 * Address: 0x800087FC  Size: 0x6C (108 bytes)
 * ======================================================================= */
#pragma push
#pragma scheduling off
void* fn_800087FC(u32 id)
{
    extern void* fn_8020E0F8(u16 id);
    extern u32 lbl_80478F50;
    u32 result;

    if (id == 0) {
        result = 0x0000EB63;
    } else if (id >= *(u32*)lbl_80478F50) {
        result = 0x0000EB63;
    } else {
        result = (u32)fn_8020DED8(fn_8020E0F8((u16)id));
    }
    if (result == 0) {
        result = 0x0000EB63;
    }

    return fn_800FA280(result);
}
#pragma pop

extern u8 lbl_8047882E;
extern u8 lbl_8047A271;
extern u8 lbl_8047A280;
extern u16 lbl_8047A282;
extern u8 lbl_8047A284;
extern u8 lbl_8047A285;
extern u8 lbl_8047A286;

/* Address: 0x800077F8 | Size: 0x8 | Pattern: return_constant */
u32 fn_800077F8(void) { return 1; }

/* Address: 0x80007840 | Size: 0x8 | Pattern: return_constant */
u32 fn_80007840(void) { return 1; }

/* Address: 0x80008144 | Size: 0x8 | Pattern: return_constant */
u32 fn_80008144(void) { return 1; }

/* Address: 0x8000814C | Size: 0x8 | Pattern: sda_getter */
u16 fn_8000814C(void) {
    return lbl_8047A282;
}

/* Address: 0x80008154 | Size: 0x8 | Pattern: sda_getter */
u8 fn_80008154(void) {
    return lbl_8047A280;
}

/* Address: 0x8000815C | Size: 0x8 | Pattern: sda_getter */
u8 fn_8000815C(void) {
    return lbl_8047882E;
}

/* Address: 0x80008164 | Size: 0x8 | Pattern: sda_getter */
u8 fn_80008164(void) {
    return lbl_8047A286;
}

/* Address: 0x8000816C | Size: 0x8 | Pattern: sda_getter */
u8 fn_8000816C(void) {
    return lbl_8047A285;
}

/* Address: 0x80008174 | Size: 0x8 | Pattern: sda_getter */
u8 fn_80008174(void) {
    return lbl_8047A284;
}

/* Address: 0x8000817C | Size: 0x8 | Pattern: sda_getter */
u8 fn_8000817C(void) {
    return lbl_8047A271;
}

/* 0x80008868 | 0x3D8 */
extern void fn_8005D9E4(void);
extern void fn_8001EA98(void);
extern void fn_8001E4B4(void);
extern void fn_800C8520(void);
extern void fn_800FAEF8(void);
extern void fn_801906A0(void);
extern u32 lbl_80478F98;
extern u8 lbl_80478838[8];
extern u8 lbl_803A19C8[];
extern u8 lbl_80266688[];
extern u32 lbl_80478F9C;
extern u8 lbl_8047B6C8[3];
#if 1
asm void fn_80008868(void) {
#include "src/game/gs_task_fn_80008868.inc"
}
#else
void fn_80008868(void) {
    extern u8 lbl_80266688[];
    extern u8 lbl_803A19C8[];
    extern u8 lbl_80478838[8];
    extern u32 lbl_80478F98;
    extern u32 lbl_80478F9C;
    extern u8 lbl_8047B6C8[3];
    extern void fn_8001E4B4();
    extern void fn_8001EA98();
    extern void fn_8005D9E4();
    extern void fn_800C8520();
    extern void fn_800FAEF8();
    extern void fn_801906A0();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r3 = *(u32*)((u8*)r3 + 0x4);
    r25 = 0x28;
    fn_8005D9E4();
    r4 = lbl_80478F98;
    tmp = 0x0;
    *(u32*)(sp + 0x8) = tmp;
    tmp = 0x1e0;
    r4 = *(u32*)((u8*)r4 + 0x0);
    r24 = r3 & 0xFF;
    *(u32*)(sp + 0x8) = tmp;
    r26 = r4;
    if ((s32)r4 > (s32)r24) {
        r26 = r24;
    }
    r27 = r26 * 0xd;
    r3 = 0x1e;
    r5 = r4 + 0x19;
    r6 = r27;
    r4 = 0x28;
    fn_8001EA98();
    /* lha tmp, lbl_80478838@sda21(tmp) */;
    if ((s32)tmp > 0) {
        r4 = 0x21;
        r6 = 0x21;
        r8 = 0x17;
        tmp = (u32)r3 >> 31;
        tmp = tmp + r3;
        r7 = (s32)tmp >> 1;
        r3 = r7 + 0x14;
        r5 = r7 + 0x28;
        r7 = r7 + 0x1e;
        fn_8001E4B4();
    }
    r3 = lbl_80478F98;
    /* lha r4, lbl_80478838@sda21(tmp) */;
    tmp = *(u32*)((u8*)r3 + 0x0);
    tmp = tmp - r4;
    if ((s32)tmp > (s32)r24) {
        r4 = r27 + 0x2f;
        r6 = r4;
        r8 = r27 + 0x39;
        tmp = (u32)r3 >> 31;
        tmp = tmp + r3;
        r7 = (s32)tmp >> 1;
        r3 = r7 + 0x14;
        r5 = r7 + 0x28;
        r7 = r7 + 0x1e;
        fn_8001E4B4();
    }
    r27 = 0x0;
    r28 = (u32)lbl_80478838;
    r4 = (u32)lbl_803A19C8;
    r3 = (u32)lbl_80266688;
    r29 = (u32)lbl_803A19C8;
    r30 = (u32)lbl_80266688;
    while ((s32)r27 < (s32)r26) {
    /* L_80008968 */
    tmp = *(s16*)((u8*)r28 + 0x2);
    if ((s32)tmp == (s32)r27) {
        r3 = 0xFF000000;
        r24 = r3 + 0xff;
    } else {
        r24 = -0x1;
    }
    /* lha tmp, lbl_80478838@sda21(tmp) */;
    r5 = r27 + tmp;
    if ((s32)r5 < 0) {
        tmp = 0x0;
    } else {
        r3 = lbl_80478F98;
        tmp = *(u32*)((u8*)r3 + 0x0);
        if (r5 >= tmp) {
            tmp = 0x0;
        } else {
    r7 = *(s16*)((u8*)r3 + 0x4);
    if ((s32)r5 != 0) {
        r3 = 0x0;
        if ((s32)tmp > 0) {
            if ((s32)tmp > 8) {
                r4 = lbl_80478F9C;
                tmp = r6 + 0x7;
                tmp = (u32)tmp >> 3;
                ctr_fn = (void(*)(void))tmp;
                if ((s32)r6 > 0) {
                    do {
                        tmp = (s16)r7;
                        r3 = r3 + 0x8;
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r4 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r4 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r4 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r4 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r4 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r4 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r4 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        r7 = *(s16*)(r4 + tmp);
                    } while (--ctr != 0);
            }
            }
            r6 = lbl_80478F9C;
            tmp = r4 - r3;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r3 < (s32)r4) {
                do {
                    tmp = (s16)r7;
                    r3 = r3 + 0x1;
                    r4 = tmp << 3;
                    tmp = r4 + 0x6;
                    r7 = *(s16*)(r6 + tmp);
                } while (--ctr != 0);
        }
        }
        tmp = (s16)r7;
        r4 = lbl_80478F9C;
        r3 = tmp << 3;
        tmp = r3 + 0x6;
        r7 = *(s16*)(r4 + tmp);
    }
    tmp = r7 & 0xFFFF;
        }
    }
    /* L_80008AA8 */
    r3 = r29;
    r4 = r30;
    r6 = tmp & 0xFFFF;
    fn_800C8520();
    r4 = r25;
    r5 = r24;
    r6 = r29;
    r3 = 0x1e;
    fn_800FAEF8();
    /* lha tmp, lbl_80478838@sda21(tmp) */;
    r4 = r27 + tmp;
    if ((s32)r4 < 0) {
        tmp = 0x0;
    } else {
        r3 = lbl_80478F98;
        tmp = *(u32*)((u8*)r3 + 0x0);
        if (r4 >= tmp) {
            tmp = 0x0;
        } else {
    r7 = *(s16*)((u8*)r3 + 0x4);
    if ((s32)r4 != 0) {
        r3 = 0x0;
        if ((s32)tmp > 0) {
            if ((s32)tmp > 8) {
                r5 = lbl_80478F9C;
                tmp = r6 + 0x7;
                tmp = (u32)tmp >> 3;
                ctr_fn = (void(*)(void))tmp;
                if ((s32)r6 > 0) {
                    do {
                        tmp = (s16)r7;
                        r3 = r3 + 0x8;
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r5 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r5 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r5 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r5 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r5 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r5 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        tmp = *(s16*)(r5 + tmp);
                        r6 = tmp << 3;
                        tmp = r6 + 0x6;
                        r7 = *(s16*)(r5 + tmp);
                    } while (--ctr != 0);
            }
            }
            r5 = lbl_80478F9C;
            tmp = r4 - r3;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r3 < (s32)r4) {
                do {
                    tmp = (s16)r7;
                    r3 = r3 + 0x1;
                    r4 = tmp << 3;
                    tmp = r4 + 0x6;
                    r7 = *(s16*)(r5 + tmp);
                } while (--ctr != 0);
        }
        }
        tmp = (s16)r7;
        r4 = lbl_80478F9C;
        r3 = tmp << 3;
        tmp = r3 + 0x6;
        r7 = *(s16*)(r4 + tmp);
    }
    tmp = r7 & 0xFFFF;
        }
    }
    /* L_80008BF8 */
    r3 = tmp & 0xFFFF;
    fn_801906A0();
    r7 = r3;
    r4 = r25;
    r5 = r24;
    r3 = r31 + 0x1e;
    r6 = (u32)lbl_8047B6C8;
    fn_800FAEF8();
    r25 = r25 + 0xd;
    r27 = r27 + 0x1;
    } /* end while loop */
    return;
}
#endif

/* 0x80008C40 | 0x538 */
extern void fn_80105624(void);
extern void fn_80190528(void);
extern void fn_801903B0(void);
extern u32 lbl_80478F98;
extern u32 lbl_80478F9C;
#if 1
asm void fn_80008C40(void) {
#include "src/game/gs_task_fn_80008C40.inc"
}
#else
void fn_80008C40(void) {
    extern u32 lbl_80478838;
    extern u32 lbl_80478F98;
    extern u32 lbl_80478F9C;
    extern void fn_8005D9E4();
    extern void fn_80105624();
    extern void fn_801903B0();
    extern void fn_80190528();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r28 = r3;
    fn_80105624();
    r27 = *(u16*)((u8*)r3 + 0x6);
    fn_80105624();
    r4 = lbl_80478F98;
    r31 = *(u16*)((u8*)r3 + 0x0);
    tmp = *(u32*)((u8*)r4 + 0x0);
    r3 = *(u32*)((u8*)r28 + 0x4);
    r30 = (s16)tmp;
    fn_8005D9E4();
    r29 = r3 & 0xFF;
    tmp = (s16)r29;
    if ((s32)r30 < (s32)tmp) {
        r29 = r30;
    }
    r4 = r27 & 0xFFFF;
    r3 = lbl_80478838;
    tmp = r4 & 0x1;
    if ((s32)tmp != 0) {
        r3 = *(s16*)((u8*)(u32)sp + 0xA);
        *(u16*)(sp + 0xA) = tmp;
    } else {
        /* L_80008CB4 */
        tmp = r4 & 0x00000002;
        if ((s32)tmp != 0) {
            r3 = *(s16*)((u8*)(u32)sp + 0xA);
            tmp = r3 + 0x1;
            *(u16*)(sp + 0xA) = tmp;
        }
    }
    /* L_80008CCC */
    tmp = r4 & 0x00000004;
    if ((s32)tmp != 0) {
        r3 = *(u32*)((u8*)r28 + 0x4);
        fn_8005D9E4();
        r3 = r3 & 0xFF;
        tmp = *(s16*)((u8*)(u32)sp + 0xA);
        tmp = tmp - r3;
        tmp = (s16)tmp;
        *(u16*)(sp + 0xA) = tmp;
    } else {
        /* L_80008CFC */
        tmp = r4 & 0x00000008;
        if ((s32)tmp != 0) {
            r3 = *(u32*)((u8*)r28 + 0x4);
            fn_8005D9E4();
            tmp = *(s16*)((u8*)(u32)sp + 0xA);
            r3 = r3 & 0xFF;
            r3 = r3 + tmp;
            tmp = (s16)tmp;
            *(u16*)(sp + 0xA) = tmp;
        }
    }
    /* L_80008D28 */
    r4 = *(s16*)((u8*)(u32)sp + 0xA);
    if ((s32)r4 < 0) {
        r3 = *(s16*)((u8*)(u32)sp + 0x8);
        tmp = 0x0;
        *(u16*)(sp + 0xA) = tmp;
        r3 = r3 + r4;
        tmp = (s16)r3;
        *(u16*)(sp + 0x8) = r3;
        if ((s32)tmp < 0) {
            r4 = (s16)r29;
            tmp = r30 - r4;
            r3 = (s16)r3;
            tmp = (s16)tmp;
            *(u16*)(sp + 0xA) = r3;
            *(u16*)(sp + 0x8) = tmp;
        }
    } else {
        /* L_80008D74 */
        r3 = (s16)r29;
        if ((s32)r4 >= (s32)r3) {
            r3 = *(s16*)((u8*)(u32)sp + 0x8);
            tmp = r4 - r5;
            r3 = r3 + tmp;
            tmp = (s16)r5;
            r3 = (s16)r3;
            *(u16*)(sp + 0xA) = tmp;
            tmp = r3 + tmp;
            *(u16*)(sp + 0x8) = r3;
            if ((s32)tmp >= (s32)r30) {
                tmp = 0x0;
                *(u16*)(sp + 0x8) = tmp;
                *(u16*)(sp + 0xA) = tmp;
            }
        }
    }
    /* L_80008DB8 */
    r3 = r31 & 0xFFFF;
    tmp = r3 & 0x00000080;
    if ((s32)tmp != 0) {
    r3 = *(s16*)((u8*)(u32)sp + 0x8);
    tmp = *(s16*)((u8*)(u32)sp + 0xA);
    r5 = r3 + tmp;
    if ((s32)r5 < 0) {
        tmp = 0x0;
    } else {
        r3 = lbl_80478F98;
        tmp = *(u32*)((u8*)r3 + 0x0);
        if (r5 >= tmp) {
            tmp = 0x0;
        } else {
    r7 = *(s16*)((u8*)r3 + 0x4);
    if ((s32)r5 != 0) {
        r4 = 0x0;
        if ((s32)tmp > 0) {
            if ((s32)tmp > 8) {
                r6 = lbl_80478F9C;
                tmp = r3 + 0x7;
                tmp = (u32)tmp >> 3;
                ctr_fn = (void(*)(void))tmp;
                if ((s32)r3 > 0) {
                    do {
                        tmp = (s16)r7;
                        r4 = r4 + 0x8;
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        r7 = *(s16*)(r6 + tmp);
                    } while (--ctr != 0);
            }
            }
            r6 = lbl_80478F9C;
            tmp = r3 - r4;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r4 < (s32)r3) {
                do {
                    tmp = (s16)r7;
                    r3 = tmp << 3;
                    tmp = r3 + 0x6;
                    r7 = *(s16*)(r6 + tmp);
                } while (--ctr != 0);
        }
        }
        tmp = (s16)r7;
        r3 = lbl_80478F9C;
        tmp = tmp << 3;
        r3 = r3 + tmp;
        r7 = *(s16*)((u8*)r3 + 0x6);
    }
    tmp = r7 & 0xFFFF;
        }
    }
    /* L_80008EEC */
    r3 = tmp & 0xFFFF;
    fn_80190528();
    } else {
    /* L_80008EF8 */
    tmp = r3 & 0x00000040;
    if ((s32)tmp != 0) {
    r3 = *(s16*)((u8*)(u32)sp + 0x8);
    tmp = *(s16*)((u8*)(u32)sp + 0xA);
    r5 = r3 + tmp;
    if ((s32)r5 < 0) {
        tmp = 0x0;
    } else {
        r3 = lbl_80478F98;
        tmp = *(u32*)((u8*)r3 + 0x0);
        if (r5 >= tmp) {
            tmp = 0x0;
        } else {
    r7 = *(s16*)((u8*)r3 + 0x4);
    if ((s32)r5 != 0) {
        r4 = 0x0;
        if ((s32)tmp > 0) {
            if ((s32)tmp > 8) {
                r6 = lbl_80478F9C;
                tmp = r3 + 0x7;
                tmp = (u32)tmp >> 3;
                ctr_fn = (void(*)(void))tmp;
                if ((s32)r3 > 0) {
                    do {
                        tmp = (s16)r7;
                        r4 = r4 + 0x8;
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        r7 = *(s16*)(r6 + tmp);
                    } while (--ctr != 0);
            }
            }
            r6 = lbl_80478F9C;
            tmp = r3 - r4;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r4 < (s32)r3) {
                do {
                    tmp = (s16)r7;
                    r3 = tmp << 3;
                    tmp = r3 + 0x6;
                    r7 = *(s16*)(r6 + tmp);
                } while (--ctr != 0);
        }
        }
        tmp = (s16)r7;
        r3 = lbl_80478F9C;
        tmp = tmp << 3;
        r3 = r3 + tmp;
        r7 = *(s16*)((u8*)r3 + 0x6);
    }
    tmp = r7 & 0xFFFF;
        }
    }
    /* L_80009028 */
    r3 = tmp & 0xFFFF;
    fn_801903B0();
    }
    }
    /* L_80009030 */
    r3 = *(s16*)((u8*)(u32)sp + 0x8);
    tmp = *(s16*)((u8*)(u32)sp + 0xA);
    r5 = r3 + tmp;
    lbl_80478838 = r4;
    if ((s32)r5 < 0) {
        tmp = 0x0;
    } else {
        r3 = lbl_80478F98;
        tmp = *(u32*)((u8*)r3 + 0x0);
        if (r5 >= tmp) {
            tmp = 0x0;
        } else {
    r7 = *(s16*)((u8*)r3 + 0x4);
    if ((s32)r5 != 0) {
        r4 = 0x0;
        if ((s32)tmp > 0) {
            if ((s32)tmp > 8) {
                r6 = lbl_80478F9C;
                tmp = r3 + 0x7;
                tmp = (u32)tmp >> 3;
                ctr_fn = (void(*)(void))tmp;
                if ((s32)r3 > 0) {
                    do {
                        tmp = (s16)r7;
                        r4 = r4 + 0x8;
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        tmp = *(s16*)(r6 + tmp);
                        r3 = tmp << 3;
                        tmp = r3 + 0x6;
                        r7 = *(s16*)(r6 + tmp);
                    } while (--ctr != 0);
            }
            }
            r6 = lbl_80478F9C;
            tmp = r3 - r4;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r4 < (s32)r3) {
                do {
                    tmp = (s16)r7;
                    r3 = tmp << 3;
                    tmp = r3 + 0x6;
                    r7 = *(s16*)(r6 + tmp);
                } while (--ctr != 0);
        }
        }
        tmp = (s16)r7;
        r3 = lbl_80478F9C;
        tmp = tmp << 3;
        r3 = r3 + tmp;
        r7 = *(s16*)((u8*)r3 + 0x6);
    }
    tmp = r7 & 0xFFFF;
        }
    }
    /* L_8000915C */
    tmp = tmp & 0xFFFF;
    *(u32*)((u8*)r28 + 0x80) = tmp;
    return;
}
#endif

/* ===== Phase 2 recovery stubs ===== */

/* fn_80006908 - 0x80006908 | size: 0x6a4 */
extern void jumptable_802E28D0();
extern u32 lbl_80478F50;
extern u32 lbl_80478F40;
extern u32 lbl_80478F00;
extern u32 lbl_80478F48;
extern u32 lbl_80478F20;
#if 1
asm s32 fn_80006908(u16 id) {
#include "src/game/gs_task_fn_80006908.inc"
}
#else
s32 fn_80006908(u16 id) { /* TODO */ return 0; }
#endif

/* fn_80006FAC - 0x80006FAC | size: 0xdc */
extern u32 fn_800F7AF0(s32 port);
extern u32 fn_800F7BC4(s32 port);
#if 0
asm void fn_80006FAC(void) {
#include "src/game/gs_task_fn_80006FAC.inc"
}
#else
#pragma peephole off
void fn_80006FAC(u8* ctx) {
    u32 buttons;
    u32 held;

    if (ctx == 0) return;

    buttons = fn_800F7AF0(1);
    held = fn_800F7BC4(1);
    if ((held & buttons) & 0x100) {
        *(u8*)(ctx + 0x98) = 1;
        *(u32*)(ctx + 0x80) = fn_801022B8(*(u32*)(ctx + 0x4));
    }

    buttons = fn_800F7AF0(1);
    held = fn_800F7BC4(1);
    if ((held & buttons) & 0x1000) {
        *(u8*)(ctx + 0x98) = 1;
        *(s32*)(ctx + 0x80) = -2;
    }

    buttons = fn_800F7AF0(1);
    held = fn_800F7BC4(1);
    if ((held & buttons) & 0x200) {
        *(u8*)(ctx + 0x98) = 1;
        *(u8*)(ctx + 0x99) = 1;
        *(s32*)(ctx + 0x80) = -1;
    }
}
#pragma peephole on
#endif

/* fn_80007088 - 0x80007088 | size: 0x44 */
extern u8   fn_801EF63C(void);
extern void fn_801F1588(s32 mode);
extern void fn_801337E4(void);
#if 0
asm void fn_80007088(void) {
#include "src/game/gs_task_fn_80007088.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80007088(void) {
    if (fn_801EF63C() == 0) {
        return -1;
    }
    fn_801F1588(0);
    fn_801337E4();
    return 0;
}
#pragma pop
#endif

/* fn_800070CC - 0x800070CC | size: 0x44 */
extern void fn_801EF62C(s32 mode);
#if 0
asm void fn_800070CC(void) {
#include "src/game/gs_task_fn_800070CC.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_800070CC(void) {
    if (fn_801EF63C() == 0) {
        return -1;
    }
    fn_801EF62C(3);
    fn_801337E4();
    return 0;
}
#pragma pop
#endif

/* fn_80007110 - 0x80007110 | size: 0x44 */
#if 0
asm void fn_80007110(void) {
#include "src/game/gs_task_fn_80007110.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80007110(void) {
    if (fn_801EF63C() == 0) {
        return -1;
    }
    fn_801EF62C(2);
    fn_801337E4();
    return 0;
}
#pragma pop
#endif

/* fn_80007154 - 0x80007154 | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl _dbgMenuFightFightPokemonSelectSub vs bl _dbgMenuFightFightPokemonSelectSub (same addr) */
extern s32  _dbgMenuFightFightPokemonSelectSub(void);
extern u32  fn_801F986C(u32 ptr, s32 slot);
extern u32  lbl_8047A278;
extern u32  lbl_8047A27C;
#if 0
asm void fn_80007154(void) {
#include "src/game/gs_task_fn_80007154.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80007154(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A27C = fn_801F986C(lbl_8047A278, 5);
    if (lbl_8047A27C == 0) return -1;
    return _dbgMenuFightFightPokemonSelectSub();
}
#pragma pop
#endif

/* fn_800071AC - 0x800071AC | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl _dbgMenuFightFightPokemonSelectSub vs bl _dbgMenuFightFightPokemonSelectSub (same addr) */
#if 0
asm void fn_800071AC(void) {
#include "src/game/gs_task_fn_800071AC.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_800071AC(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A27C = fn_801F986C(lbl_8047A278, 4);
    if (lbl_8047A27C == 0) return -1;
    return _dbgMenuFightFightPokemonSelectSub();
}
#pragma pop
#endif

/* fn_80007204 - 0x80007204 | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl _dbgMenuFightFightPokemonSelectSub vs bl _dbgMenuFightFightPokemonSelectSub (same addr) */
#if 0
asm void fn_80007204(void) {
#include "src/game/gs_task_fn_80007204.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80007204(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A27C = fn_801F986C(lbl_8047A278, 3);
    if (lbl_8047A27C == 0) return -1;
    return _dbgMenuFightFightPokemonSelectSub();
}
#pragma pop
#endif

/* fn_8000725C - 0x8000725C | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl _dbgMenuFightFightPokemonSelectSub vs bl _dbgMenuFightFightPokemonSelectSub (same addr) */
#if 0
asm void fn_8000725C(void) {
#include "src/game/gs_task_fn_8000725C.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_8000725C(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A27C = fn_801F986C(lbl_8047A278, 2);
    if (lbl_8047A27C == 0) return -1;
    return _dbgMenuFightFightPokemonSelectSub();
}
#pragma pop
#endif

/* fn_800072B4 - 0x800072B4 | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl _dbgMenuFightFightPokemonSelectSub vs bl _dbgMenuFightFightPokemonSelectSub (same addr) */
#if 0
asm void fn_800072B4(void) {
#include "src/game/gs_task_fn_800072B4.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_800072B4(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A27C = fn_801F986C(lbl_8047A278, 1);
    if (lbl_8047A27C == 0) return -1;
    return _dbgMenuFightFightPokemonSelectSub();
}
#pragma pop
#endif

/* fn_8000730C - 0x8000730C | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl _dbgMenuFightFightPokemonSelectSub vs bl _dbgMenuFightFightPokemonSelectSub (same addr) */
#if 0
asm void fn_8000730C(void) {
#include "src/game/gs_task_fn_8000730C.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_8000730C(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A27C = fn_801F986C(lbl_8047A278, 0);
    if (lbl_8047A27C == 0) return -1;
    return _dbgMenuFightFightPokemonSelectSub();
}
#pragma pop
#endif

/* _dbgMenuFightFightPokemonSelectSub - 0x80007364 | size: 0x2f8 */
extern void* fn_801F54A4(s32 a, u16 b, s32 c, s32 d);
extern void* fn_80205BE8(u32 ctx);
extern void* fn_801F4460(s32 a, u32 ctx);
extern u8   fn_80203E7C(u32 ctx);
extern u8   fn_801F8C00(void* ptr, u32 ctx);
extern void* fn_801F8D80(void* ptr, u32 ctx);
extern u8   fn_801FECD4(void);
extern void fn_80207BF4(void* ptr);
extern void fn_80207B8C(void* ptr, s32 a);
extern s32  fn_800096B4(void* ptr, s32 a, u8* b, u8* c, u8* d, u8* e);
extern u16  fn_8012640C(s32 a, u16 b, s32 c, s32 d);
extern void fn_8010B01C(void* ptr, s32 a, s32 b);
extern void _threadSwitch(void);
extern void fn_8010BBB8(void* ptr);
extern s8   fn_8010BCE4(void);
extern void* fn_80121C18(void* ptr);
extern void battleGridReplacePokemon(void* ptr, void* src);
extern void fn_801C3430(void);
extern void fn_801254B4(void* ptr, s32 a, s32 b, s32 c, s32 d);
extern void fn_801DB100(void* ptr);
extern void fn_801DA4E8(void* ptr, s32 a);
extern void fn_80202810(void* ptr, s32 a);
extern void fn_802653FC(void* ptr, u16 a, s32 b);
extern void fn_80122040(void* a, u16 b);
extern void* fn_801248C4(void* ptr);
extern void fn_80207BC0(void* ptr, void* a);
extern void fn_80207B5C(void* ptr, u8 a, u16 b);
extern void fn_80265754(void* ptr, u16 a);
#if 1
asm s32 _dbgMenuFightFightPokemonSelectSub(void) {
#include "src/game/gs_task__dbgMenuFightFightPokemonSelectSub.inc"
}
#else
#pragma peephole off
s32 _dbgMenuFightFightPokemonSelectSub(u32 ctx) {
    u32 savedId;
    u8 prevLevel;
    void* archive;
    void* scene;
    s32 result;
    void* encounter;
    u16 itemId;
    u16 subItem;
    u16 loopIdx;
    u8 sp_b;
    u8 sp_a;
    u8 sp_9;
    u8 sp_8;
    sp_b = 0;
    sp_a = 0;
    sp_9 = 0;
    sp_8 = 0;

    savedId = (u16)(u32)fn_801F54A4(0, 0, 0x14, 0);
    scene = fn_80205BE8(ctx);
    if (scene == 0) {
        return -1;
    }

    archive = fn_801F4460(0, ctx);
    if (archive == 0) {
        return -1;
    }

    prevLevel = fn_80203E7C(ctx);

    if (fn_801F8C00(archive, ctx) == 2) {
        encounter = fn_801F8D80(archive, ctx);
        if (fn_801FECD4() == 1) {
            return -1;
        }

        if (encounter != 0) {
            fn_80207BF4(encounter);
            for (loopIdx = 0; (u16)loopIdx < 2; loopIdx++) {
                fn_80207B8C(encounter, 0);
            }
        }

        result = fn_800096B4(scene, 1, &sp_b, &sp_a, &sp_9, &sp_8);
        if (result == 1) {
            itemId = fn_8012640C(0, (u16)(u32)scene, 0x6e, 0);
            if (encounter != 0) {
                subItem = fn_8012640C(0, (u16)(u32)encounter, 0xee, 0);
                if (subItem != 0) {
                    void* animPtr;
                    fn_8010B01C(scene, 0, 0);
                    do {
                        fn_8010BBB8(scene);
                    } while (fn_8010BCE4() == 0);
                    animPtr = fn_80121C18(scene);
                    battleGridReplacePokemon((void*)(u32)subItem, animPtr);
                    fn_801C3430();
                    fn_801254B4(encounter, 0, 0xee, 0, (s32)animPtr);
                    fn_801DB100((void*)(u32)subItem);
                    fn_801DA4E8(animPtr, 1);
                    fn_80202810(encounter, 0x14);
                }
                {
                    u16 newId;
                    newId = (u16)(u32)fn_801F54A4(0, 0, 0x14, 0);
                    fn_802653FC(encounter, newId, 1);
                }
                {
                    u16 eeItem;
                    eeItem = fn_8012640C(0, (u16)(u32)encounter, 0xee, 0);
                    fn_80122040(scene, eeItem);
                }
                fn_80207BC0(encounter, fn_801248C4(scene));
                {
                    u16 i;
                    for (i = 0; (u16)i < 2; i++) {
                        u16 val;
                        val = fn_8012640C(0, itemId, 0x16, i);
                        fn_80207B5C(encounter, (u8)i, val);
                    }
                }
            }
        }
    } else {
        result = fn_800096B4(scene, 1, 0, 0, 0, 0);
    }

    if ((u8)fn_80203E7C(ctx) > (u8)prevLevel) {
        fn_801254B4((void*)ctx, 0, 0xd0, 0, 1);
    }

    fn_80265754(archive, savedId);
    return result;
}
#pragma peephole on
#endif

/* fn_8000765C - 0x8000765C | size: 0xac */
extern void* fn_80008390(u16 id);
extern void fn_80051710(u16 id);
extern u16 lbl_80478830;
extern u32 lbl_80478F08;
#if 0
asm void fn_8000765C(void) {
#include "src/game/gs_task_fn_8000765C.inc"
}
#else
#pragma peephole off
s32 fn_8000765C(void) {
    u32 result;
    s32 val;

    if (fn_801EF63C() == 0) {
        return -1;
    }

    for (;;) {
        if (fn_8001E304(lbl_80478830, &result, fn_80008390) == 0) {
            val = -1;
        } else {
            if (result >= *(u32*)lbl_80478F08) {
                result = *(u32*)lbl_80478F08 - 1;
            }
            fn_8001E200();
            val = (s32)result;
        }
        if (val < 0) {
            return 1;
        }
        lbl_80478830 = (u16)val;
        fn_80051710((u16)val);
    }
}
#pragma peephole on
#endif

/* fn_80007708 - 0x80007708 | size: 0x70 */
extern s32  fn_80051E38(u32 slot);
#if 0
asm void fn_80007708(void) {
#include "src/game/gs_task_fn_80007708.inc"
}
#else
#pragma peephole off
s32 fn_80007708(void) {
    u16 tmp;
    u16 slot;
    tmp  = (u16)(u32)fn_801FB1C0((s32)lbl_8047A278, 0, 0x43, 0);
    slot = (u16)(u32)fn_801FB1C0(0, tmp, 2, 0);
    if (fn_801EF63C() == 0) return -1;
    return fn_80051E38(slot);
}
#pragma peephole on
#endif

/* fn_80007778 - 0x80007778 | size: 0x20 */
extern void fn_8004EADC(void);
#if 0
asm void fn_80007778(void) {
#include "src/game/gs_task_fn_80007778.inc"
}
#else
void fn_80007778(void) {
    fn_8004EADC();
}
#endif

/* fn_80007798 - 0x80007798 | size: 0x20 */
extern void fn_8004EC54(void);
#if 0
asm void fn_80007798(void) {
#include "src/game/gs_task_fn_80007798.inc"
}
#else
void fn_80007798(void) {
    fn_8004EC54();
}
#endif

/* fn_800077B8 - 0x800077B8 | size: 0x20 */
extern void fn_8004EDCC(void);
#if 0
asm void fn_800077B8(void) {
#include "src/game/gs_task_fn_800077B8.inc"
}
#else
void fn_800077B8(void) {
    fn_8004EDCC();
}
#endif

/* fn_800077D8 - 0x800077D8 | size: 0x20 */
extern void fn_8004F860(void);
#if 0
asm void fn_800077D8(void) {
#include "src/game/gs_task_fn_800077D8.inc"
}
#else
void fn_800077D8(void) {
    fn_8004F860();
}
#endif

/* fn_80007800 - 0x80007800 | size: 0x20 */
extern void fn_8004FE3C(void);
#if 0
asm void fn_80007800(void) {
#include "src/game/gs_task_fn_80007800.inc"
}
#else
void fn_80007800(void) {
    fn_8004FE3C();
}
#endif

/* fn_80007820 - 0x80007820 | size: 0x20 */
extern void fn_80050844(void);
#if 0
asm void fn_80007820(void) {
#include "src/game/gs_task_fn_80007820.inc"
}
#else
void fn_80007820(void) {
    fn_80050844();
}
#endif

/* fn_80007848 - 0x80007848 | size: 0xa4 */
extern void dbgMenuFightTrainerDataStatusInputDigit(u16 id, s32 a, s32 b, s32 c, s32 d);
extern u16 lbl_8047A28A;
extern u32 lbl_80478F28;
extern void* _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub(u32 id);
#if 0
asm void fn_80007848(void) {
#include "src/game/gs_task_fn_80007848.inc"
}
#else
#pragma peephole off
s32 fn_80007848(void) {
    u32 result;
    s32 val;

    for (;;) {
        if (fn_8001E304(lbl_8047A28A, &result, _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub) == 0) {
            val = -1;
        } else {
            if (result > *(u32*)lbl_80478F28) {
                result = *(u32*)lbl_80478F28 - 1;
            }
            fn_8001E200();
            val = (s32)result;
        }
        if (val < 0) {
            return 1;
        }
        lbl_8047A28A = (u16)val;
        dbgMenuFightTrainerDataStatusInputDigit((u16)val, 0x3e, 0, 0xc8, -0xc8);
    }
}
#pragma peephole on
#endif

/* fn_800078EC - 0x800078EC | size: 0x58 */
extern u32  fn_801F7258(u32 ptr, s32 mode);
extern u32  lbl_8047A274;
#if 0
asm void fn_800078EC(void) {
#include "src/game/gs_task_fn_800078EC.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_800078EC(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A278 = fn_801F7258(lbl_8047A274, 1);
    if (lbl_8047A278 == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* fn_80007944 - 0x80007944 | size: 0x58 */
#if 0
asm void fn_80007944(void) {
#include "src/game/gs_task_fn_80007944.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80007944(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A278 = fn_801F7258(lbl_8047A274, 0);
    if (lbl_8047A278 == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* fn_8000799C - 0x8000799C | size: 0x58 */
extern u32  fn_801F025C(s32 a, s32 b);
#if 0
asm void fn_8000799C(void) {
#include "src/game/gs_task_fn_8000799C.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_8000799C(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A274 = fn_801F025C(5, 0);
    if (lbl_8047A274 == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* fn_800079F4 - 0x800079F4 | size: 0x58 */
#if 0
asm void fn_800079F4(void) {
#include "src/game/gs_task_fn_800079F4.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_800079F4(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A274 = fn_801F025C(4, 0);
    if (lbl_8047A274 == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* fn_80007A4C - 0x80007A4C | size: 0x38 */
#if 0
asm void fn_80007A4C(void) {
#include "src/game/gs_task_fn_80007A4C.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80007A4C(void) {
    if (fn_801EF63C() == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* fn_80007A84 - 0x80007A84 | size: 0xac | SYMBOL-NAME WALL 97.67%: bl dbgMenuFightWazaEditSub vs bl dbgMenuFightWazaEditSub (same addr) */
extern void dbgMenuFightWazaEditSub(u16 id);
extern u16 lbl_8047882A;
extern void* fn_800083FC(u32 id);
#if 0
asm void fn_80007A84(void) {
#include "src/game/gs_task_fn_80007A84.inc"
}
#else
#pragma peephole off
s32 fn_80007A84(void) {
    u32 result;
    s32 val;

    if (fn_801EF63C() == 0) {
        return -1;
    }

    for (;;) {
        if (fn_8001E304(lbl_8047882A, &result, fn_800083FC) == 0) {
            val = -1;
        } else {
            if (result >= 0x163) {
                result = 0x162;
            }
            fn_8001E200();
            val = (s32)result;
        }
        if (val < 0) {
            return 1;
        }
        lbl_8047882A = (u16)val;
        dbgMenuFightWazaEditSub((u16)val);
    }
}
#pragma peephole on
#endif

/* fn_80007B30 - 0x80007B30 | size: 0x4ac */
extern u8 fn_802117FC(void);
extern void fn_80211810(u8 val);
extern s8 menuSubOpenYesNo(s32 max, s32 a, s32 b, s32 initial);
extern u8 fn_8001E224(u8 val, u32* out, s32 a, s32 b, s32 c, s32 d);
extern u8 fn_80219FE4(u16 id);
extern u8 lbl_80478828;
extern u8 lbl_8047A271;
extern u8 lbl_8047A270;
extern u8 lbl_8047A284;
extern u8 lbl_8047A285;
extern u8 lbl_8047A286;
extern u8 lbl_8047A280;
extern u16 lbl_8047A282;
extern u8 lbl_8047882E;
#if 0
asm void fn_80007B30(void) {
#include "src/game/gs_task_fn_80007B30.inc"
}
#else
#pragma peephole off
s32 fn_80007B30(void) {
    u8 save_sfx;
    u8 save_speed;
    u8 save_rumble;
    u8 save_cam;
    u16 save_species;
    u8 save_nickname;
    u8 save_vol;
    u8 save_battle;
    u8 save_field;
    u8 save_stereo;
    s32 evt;
    u32 tmp;

    save_vol = lbl_80478828;
    save_stereo = lbl_8047A271;
    save_field = lbl_8047A270;
    save_battle = lbl_8047A284;
    save_speed = lbl_8047A285;
    save_sfx = fn_802117FC();
    save_rumble = lbl_8047A286;
    save_cam = lbl_8047A280;
    save_species = lbl_8047A282;
    save_nickname = lbl_8047882E;

    for (;;) {
        evt = fn_801026A4(0xe, 0, 0, 0, 1, 0, 0);
        if (evt == -1) {
            fn_80102568(0xe, 0, 1);
            if (save_vol < 1) save_vol = 1;
            if ((u8)save_vol > 9) save_vol = 9;
            lbl_80478828 = save_vol;
            lbl_8047A271 = save_stereo;
            lbl_8047A270 = save_field;
            lbl_8047A284 = save_battle;
            lbl_8047A285 = save_speed;
            fn_80211810(save_sfx);
            lbl_8047A286 = save_rumble;
            lbl_8047A280 = save_cam;
            lbl_8047A282 = save_species;
            lbl_8047882E = save_nickname;
            return -1;
        }
        if (evt == -2) {
            if (fn_8010264C(0x44, 1) != 0) {
                fn_80102568(0x44, 0, 1);
                continue;
            }
            fn_80102568(0x44, 0, 1);
            break;
        }

        if (evt == 0x57) {
            s8 r;
            r = menuSubOpenYesNo(0x7f, -1, -1, lbl_8047A271 == 0);
            if (r == 0) {
                lbl_8047A271 = 1;
            } else if (r == 1) {
                lbl_8047A271 = 0;
            }
            continue;
        }

        if (evt == 0x59) {
            u8 ret;
            ret = fn_8001E224(lbl_80478828, &tmp, 1, 0x32, 0x32, 0);
            if (ret == 0) {
                fn_8001E200();
                continue;
            }
            {
                u8 v;
                v = (u8)tmp;
                if ((u8)v < 1) v = 1;
                if ((u8)v > 9) v = 9;
                lbl_80478828 = v;
            }
            fn_8001E200();
            continue;
        }

        if (evt == 0x5b) {
            s8 r;
            r = menuSubOpenYesNo(0x7f, -1, -1, lbl_8047A270 == 0);
            if (r == 0) {
                lbl_8047A270 = 1;
            } else if (r == 1) {
                lbl_8047A270 = 0;
            }
            continue;
        }

        if (evt == 0x5d) {
            u8 curVal;
            s8 r;
            curVal = fn_802117FC();
            r = menuSubOpenYesNo(0x7f, -1, -1, curVal == 0);
            if (r == 0) {
                fn_80211810(1);
            } else if (r == 1) {
                fn_80211810(0);
            }
            continue;
        }

        if (evt == 0x5f) {
            s8 r;
            r = menuSubOpenYesNo(0x7f, -1, -1, lbl_8047A284 == 0);
            if (r == 0) {
                lbl_8047A284 = 1;
            } else if (r == 1) {
                lbl_8047A284 = 0;
            }
            continue;
        }

        if (evt == 0x6de) {
            s8 r;
            r = menuSubOpenYesNo(0x7f, -1, -1, lbl_8047A285 == 0);
            if (r == 0) {
                lbl_8047A285 = 1;
            } else if (r == 1) {
                lbl_8047A285 = 0;
            }
            continue;
        }

        if (evt == 0x6f9) {
            s8 r;
            r = menuSubOpenYesNo(0x7f, -1, -1, lbl_8047A286 == 0);
            if (r == 0) {
                lbl_8047A286 = 1;
            } else if (r == 1) {
                lbl_8047A286 = 0;
            }
            continue;
        }

        if (evt == 0x778) {
            s8 r;
            r = menuSubOpenYesNo(0x7f, -1, -1, lbl_8047A280 == 0);
            if (r == 0) {
                lbl_8047A280 = 1;
            } else if (r == 1) {
                lbl_8047A280 = 0;
            }
            continue;
        }

        if (evt == 0x77a) {
            if (fn_801EF63C() != 0) {
                s16 sid;
                s32 maxVal;
                sid = (s16)lbl_8047A282;
                maxVal = 0x162;
                for (;;) {
                    if (fn_8001E304((u16)sid, &tmp, fn_800083FC) == 0) {
                        sid = -1;
                    } else {
                        if (tmp >= 0x163) {
                            tmp = (u32)maxVal;
                        }
                        fn_8001E200();
                        sid = (s16)tmp;
                    }
                    if (sid < 0) break;
                    if (fn_80219FE4((u16)sid) == 1) {
                        lbl_8047A282 = (u16)sid;
                        break;
                    }
                }
            }
            continue;
        }

        if (evt == 0x1198) {
            s8 r;
            r = menuSubOpenYesNo(0x7f, -1, -1, lbl_8047882E == 0);
            if (r == 0) {
                lbl_8047882E = 1;
            } else if (r == 1) {
                lbl_8047882E = 0;
            }
            continue;
        }
    }

    fn_80102568(0xe, 0, 1);
    return 1;
}
#pragma peephole on
#endif

/* fn_80007FDC - 0x80007FDC | size: 0x38 */
#if 0
asm void fn_80007FDC(void) {
#include "src/game/gs_task_fn_80007FDC.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80007FDC(void) {
    if (fn_801EF63C() == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* fn_80008014 - 0x80008014 | size: 0x38 */
#if 0
asm void fn_80008014(void) {
#include "src/game/gs_task_fn_80008014.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80008014(void) {
    if (fn_801EF63C() == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* fn_8000804C - 0x8000804C | size: 0xf8 */
extern void* fn_800FF560(void);
extern void fn_8020DAD0(void);
extern void* GSthreadCreate(s32 a, void* b, s32 c, s32 d, s32 e, void* f);
extern void fn_800F0654(void* task, s32 a, ...);
extern u16 lbl_8047882C;
extern u32 lbl_80478F50;
#if 0
asm void fn_8000804C(void) {
#include "src/game/gs_task_fn_8000804C.inc"
}
#else
#pragma peephole off
s32 fn_8000804C(void) {
    u32 result;
    s32 val;

    for (;;) {
        if (fn_8001E304(lbl_8047882C, &result, fn_800087FC) == 0) {
            val = -1;
        } else {
            if (result >= *(u32*)(void*)lbl_80478F50) {
                result = *(u32*)(void*)lbl_80478F50 - 1;
            }
            fn_8001E200();
            val = (s32)result;
        }
        if (val < 0) {
            break;
        }
        lbl_8047882C = (u16)val;
        if ((u16)lbl_8047882C == 0) {
            continue;
        }
        if (fn_80006908((u16)val) < 0) {
            continue;
        }
        if (fn_801EF63C() != 0) {
            break;
        }
        {
            u16 saved;
            void* taskPtr;
            saved = lbl_8047882C;
            taskPtr = GSthreadCreate(1, fn_800FF560(), 0x4000, 1, 1, (void*)fn_8020DAD0);
            if (taskPtr != 0) {
                fn_800F0654(taskPtr, 1, saved);
            }
            return 0;
        }
    }
    return -1;
}
#pragma peephole on
#endif

/* fn_80008184 - 0x80008184 | size: 0xc0 */
extern u8 fn_80106160(void);
extern u8 lbl_80266678[];
#if 0
asm void fn_80008184(void) {
#include "src/game/gs_task_fn_80008184.inc"
}
#else
#pragma peephole off
u32 fn_80008184(u32 value) {
    u32 buttons;
    u32 held;
    u8 idx;
    u8 buf[10];

    idx = lbl_80478828;
    *(u32*)&buf[0] = *(u32*)&lbl_80266678[0];
    *(u32*)&buf[4] = *(u32*)&lbl_80266678[4];
    *(u16*)&buf[8] = *(u16*)&lbl_80266678[8];

    if (fn_80106160() == 1) {
        value = (value * buf[idx]) / 100;
        if (lbl_8047A270 == 1) {
            value = 0;
            for (;;) {
                buttons = fn_800F7AF0(1);
                held = fn_800F7BC4(1);
                if ((held & buttons) & 0x100) {
                    break;
                }
                _threadSwitch();
            }
        }
    }

    return value;
}
#pragma peephole on
#endif

/* _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub - 0x80008244 | size: 0x14c */
extern void fn_80132A38(s32 slot, void* ptr);
extern u32 lbl_80478F28;
#if 0
asm void _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub(void) {
#include "src/game/gs_task_fn_80008244.inc"
}
#else
#pragma peephole off
void* _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub(u32 id) {
    void* r31;
    void* r30;
    void* r29;
    void* r28;
    if (id >= *(u32*)(void*)lbl_80478F28) {
        r28 = 0;
        r31 = (void*)0xEB63;
        r30 = r31;
        r29 = r31;
    } else {
        r31 = fn_801FB1C0(0, (u16)id, 0x40, 0);
        r30 = fn_801FB1C0(0, (u16)id, 0x41, 0);
        r29 = fn_801FB1C0(0, (u16)id, 0x3f, 0);
        r28 = fn_801FB1C0(0, (u16)id, 0x3e, 0);
    }
    if (r29 == 0) r29 = (void*)0xEB63;
    if (r31 == 0) r31 = (void*)0xEB63;
    if (r30 == 0) r30 = (void*)0xEB63;
    fn_80132A38(0xd,  fn_800FA280((u32)r31));
    fn_80132A38(0xe,  fn_800FA280((u32)r30));
    fn_80132A38(0x4d, fn_800FA280((u32)r29));
    fn_80132A38(0x2f, r28);
    return fn_800FA280(0xF159);
}
#pragma peephole on
#endif

/* fn_800083FC - 0x800083FC | size: 0x64 */
extern void* fn_8011BEB4(s32 a, u16 b, s32 c, s32 d);
#if 0
asm void fn_800083FC(void) {
#include "src/game/gs_task_fn_800083FC.inc"
}
#else
#pragma push
#pragma peephole off
void* fn_800083FC(u32 id) {
    if (id == 0) return fn_800FA280(0xEB63);
    if (id >= 0x163) return fn_800FA280(0xEB63);
    return fn_800FA280((u32)fn_8011BEB4(0, (u16)id, 1, 0));
}
#pragma pop
#endif

/* fn_80008460 - 0x80008460 | size: 0x60 */
extern void* fn_800084C0(u32 id);   /* forward decl for callback */
#if 0
asm void fn_80008460(void) {
#include "src/game/gs_task_fn_80008460.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80008460(s32 id) {
    u32 result;
    if (fn_8001E304((u8)id, &result, fn_800084C0) == 0) return -1;
    if (result > 0xb) result = 0xb;
    fn_8001E200();
    return (s32)result;
}
#pragma pop
#endif

/* fn_800084C0 - 0x800084C0 | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl wazaGetWazaTypeIdName vs bl wazaGetWazaTypeIdName (same addr) */
extern void* wazaGetWazaTypeIdName(u8 idx);
#if 0
asm void fn_800084C0(void) {
#include "src/game/gs_task_fn_800084C0.inc"
}
#else
#pragma push
#pragma peephole off
void* fn_800084C0(u32 id) {
    if (id == 0) return fn_800FA280(0xEB63);
    if (id > 0xb) return fn_800FA280(0xEB63);
    return fn_800FA280((u32)wazaGetWazaTypeIdName((u8)id));
}
#pragma pop
#endif

/* fn_80008518 - 0x80008518 | size: 0x64 */
#if 0
asm void fn_80008518(void) {
#include "src/game/gs_task_fn_80008518.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80008518(u32 id) {
    u32 result;
    if (fn_8001E304((u16)id, &result, fn_8000857C) == 0) return -1;
    if (result >= lbl_80478B38) result = lbl_80478B38 - 1;
    fn_8001E200();
    return (s32)result;
}
#pragma pop
#endif

/* fn_800085D8 - 0x800085D8 | size: 0xb4 */
#if 0
asm void fn_800085D8(void) {
#include "src/game/gs_task_fn_800085D8.inc"
}
#else
#pragma push
#pragma scheduling off
void* fn_800085D8(s32 difficulty) {
    u32 id;
    if (difficulty >= 5) {
        id = 0xEB63;
    } else {
        u32 v;
        if (difficulty == 0)      v = 0x0000ED04;
        else if (difficulty == 1) v = 0x0000ED05;
        else if (difficulty == 2) v = 0x0000ED06;
        else if (difficulty == 3) v = 0x0000ED07;
        else if (difficulty == 4) v = 0x0000ED08;
        else                      v = 0x0000ED04;
        id = v;
    }
    if (id == 0) id = 0xEB63;
    return fn_800FA280(id);
}
#pragma pop
#endif

/* fn_8000868C - 0x8000868C | size: 0x60 */
#if 0
asm void fn_8000868C(void) {
#include "src/game/gs_task_fn_8000868C.inc"
}
#else
void* fn_8000868C(u32 id) {
    u32 result;
    if (id >= *(u32*)lbl_80478F20) {
        result = 0xEB63;
    } else {
        result = (u32)fn_801FB1C0(0, (u16)id, 3, 0);
    }
    if (result == 0) result = 0xEB63;
    return fn_800FA280(result);
}
#endif

/* fn_800086EC - 0x800086EC | size: 0x58 */
extern void* fn_8020E204(u16 id);
extern void* fn_8020E1EC(void* ptr);
#if 0
asm void fn_800086EC(void) {
#include "src/game/gs_task_fn_800086EC.inc"
}
#else
void* fn_800086EC(u32 id) {
    u32 result;
    if (id >= *(u32*)lbl_80478F00) {
        result = 0xEB63;
    } else {
        result = (u32)fn_8020E1EC(fn_8020E204((u16)id));
    }
    if (result == 0) result = 0xEB63;
    return fn_800FA280(result);
}
#endif

/* fn_80008744 - 0x80008744 | size: 0x58 */
extern void* fn_8020E488(u16 id);
extern void* fn_8020E248(void* ptr);
#if 0
asm void fn_80008744(void) {
#include "src/game/gs_task_fn_80008744.inc"
}
#else
void* fn_80008744(u32 id) {
    u32 result;
    if (id >= *(u32*)lbl_80478F40) {
        result = 0xEB63;
    } else {
        result = (u32)fn_8020E248(fn_8020E488((u16)id));
    }
    if (result == 0) result = 0xEB63;
    return fn_800FA280(result);
}
#endif

/* fn_8000879C - 0x8000879C | size: 0x60 */
#if 0
asm void fn_8000879C(void) {
#include "src/game/gs_task_fn_8000879C.inc"
}
#else
void* fn_8000879C(u32 id) {
    u32 result;
    if (id >= *(u32*)lbl_80478F48) {
        result = 0xEB63;
    } else {
        result = (u32)fn_801F54A4(0, (u16)id, 1, 0);
    }
    if (result == 0) result = 0xEB63;
    return fn_800FA280(result);
}
#endif

