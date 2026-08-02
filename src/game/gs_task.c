/**
 * @file gs_task.c
 * @brief GStask -- Scene/FSYS task management and archive loading callbacks.
 *
 * Address range: 0x80006630 - 0x80009178 (~45 functions)
 *
 * This module manages the task system that sits between the main loop and
 * the individual game subsystems. It handles:
 *   - FSYS archive loading via task callbacks (fightTrainerGetStatus)
 *   - Scene transition sequencing with menuOpenCustom dispatch
 *   - Archive completion callbacks with scene ID routing
 *   - Resource group management (models, textures, scripts)
 *
 * The functions follow a clear pattern:
 *   1. Load an SDA global (lbl_8047A288) for the current scene/map ID
 *   2. Call fightTrainerGetStatus to initiate an archive load with a priority level
 *   3. On success, call winMsgOpenFight and winMsgCloseFight to activate the loaded data
 *   4. Return 1 on success, -1 on failure
 *
 * Key functions:
 *   dbgMenuCameraResetFloor  (calls GSscene_CameraSetPosition; decompiled under its real name below)
 *   dbgMenuCameraChangeDisp  (loads archive slot 6 / topmenu; decompiled under its real name below)
 *   dbgMenuCameraSetType  (dispatches to GSscene_SetMode based on mode; decompiled below --
 *                          retail symbol name is a reused debug-menu name)
 *   fn_80006724  (loads archive slot 6, priority 3 / pda_menu; decompiled below)
 *   fn_8000677C  (loads archive slot 6, priority 2 / pocket_menu; decompiled below)
 *   fn_800067D4  (loads archive slot 6, priority 1 / pcbox_menu; decompiled below)
 *   fn_8000682C  (loads archive slot 6, priority 0 / battle; decompiled below)
 *   fn_80006884  GStask_SelectRandomNPC     -- random NPC selection from scene data
 *   fn_80006908  GStask_ProcessSceneEvent   -- 0x6A4 bytes, large scene event dispatcher
 *   fn_80006FAC  GStask_ProcessEventResult  -- handles event return codes
 *   dbgMenuFightStopTimeOut  GStask_GetField1           -- struct field accessor (offset 0x00)
 *   dbgMenuFightStopHostLose  GStask_GetField2           -- struct field accessor (offset 0x04)
 *   dbgMenuFightStopHostWin  GStask_GetField3           -- struct field accessor (offset 0x08)
 *   dbgMenuFightFightPokemonSelect5  GStask_LoadSlot0           -- archive load helper, slot 0
 *   dbgMenuFightFightPokemonSelect4  GStask_LoadSlot1           -- archive load helper, slot 1
 *   dbgMenuFightFightPokemonSelect3  GStask_LoadSlot2           -- archive load helper, slot 2
 *   dbgMenuFightFightPokemonSelect2  GStask_LoadSlot3           -- archive load helper, slot 3
 *   dbgMenuFightFightPokemonSelect1  GStask_LoadSlot4           -- archive load helper, slot 4
 *   dbgMenuFightFightPokemonSelect0  GStask_LoadSlot5           -- archive load helper, slot 5
 *   menuFightPokemonSelectSub  GStask_InitSceneResources  -- 0x2F8 bytes, resource init
 *   dbgMenuFightFightTrainerPokemonPartDataEdit  GStask_ShutdownResources   -- resource cleanup
 *   dbgMenuFightFightTrainerAiDataEdit  GStask_UpdateLoadState     -- resource loading state machine
 *   fn_80007778  GStask_GetLoadFlag0        -- small accessor (0x20 bytes)
 *   fn_80007798  GStask_GetLoadFlag1        -- small accessor
 *   fn_800077B8  GStask_GetLoadFlag2        -- small accessor
 *   dbgMenuFightFightTrainerAiAddsubWazaDefpokemon  GStask_GetLoadFlag3        -- small accessor
 *   fn_800077F8  GStask_ReturnZero          -- stub, returns 0
 *   dbgMenuFightFightTrainerAiAddsubIrekaeDasu  GStask_GetLoadFlag4        -- small accessor
 *   fn_80007820  GStask_GetLoadFlag5        -- small accessor
 *   fn_80007840  GStask_ReturnZero2         -- stub, returns 0
 *   fn_80007848  GStask_ValidateScene       -- validates scene index bounds
 *   dbgMenuFightFightTrainerSelect1  GStask_ArchiveCB0          -- archive completion callback
 *   dbgMenuFightFightTrainerSelect0  GStask_ArchiveCB1          -- archive completion callback
 *   dbgMenuFightFightSideSelectHostEnemy  GStask_ArchiveCB2          -- archive completion callback
 *   dbgMenuFightFightSideSelectHost  GStask_ArchiveCB3          -- archive completion callback
 *   dbgMenuFightFightFloorEditTenkou  GStask_ArchiveCB4          -- archive completion callback
 *   dbgMenuFightWazaEdit  GStask_DispatchArchiveCBs  -- routes to the above callbacks
 *   fn_80007B30  GStask_MainUpdate          -- 0x4AC bytes, main task state machine
 *   dbgMenuFightStop  GStask_GetSceneCount       -- return scene table size
 *   dbgMenuFightFightFloorEdit  GStask_GetEventCount       -- return event table size
 *   fn_8000804C  GStask_SetupTransition     -- scene transition setup (0xF8 bytes)
 *   fn_80008144  GStask_Accessor0           -- 8-byte accessor (lwz/blr)
 *   fn_8000814C  GStask_Accessor1           -- 8-byte accessor
 *   fn_80008154  GStask_Accessor2           -- 8-byte accessor
 *   fn_8000815C  GStask_Accessor3           -- 8-byte accessor
 *   fn_80008164  GStask_Accessor4           -- 8-byte accessor
 *   fn_8000816C  GStask_Accessor5           -- 8-byte accessor
 *   fn_80008174  GStask_Accessor6           -- 8-byte accessor
 *   fn_8000817C  GStask_Accessor7           -- 8-byte accessor
 *   dbgMenuFightGetMsgSpeedToFrame  GStask_FinalizeLoad        -- 0xC0 bytes, finalize resource load
 *   _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub  GStask_BuildResourceList   -- 0x14C bytes, compile resource table
 *
 * Code patterns:
 *   - SDA globals lbl_8047A288 (current scene halfword), lbl_80478F50 (scene table ptr)
 *   - menuOpenCustom is the main event dispatch (takes slot, params, flags)
 *   - menuCloseCustom is the event cancel/cleanup function
 *   - menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs is a bounded random selection helper
 *   - menuSubCloseNumberInput is a random result commit function
 */

#include "dolphin/types.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

/* Scene system */
extern void  cameraResetFloor(void);                   /* GSscene_CameraSetPosition */
extern void  GSscene_SetMode(s32 mode);               /* GSscene_SetMode */

/* FSYS archive loading */
extern void* fightTrainerGetStatus(s32 slot, u16 sceneId, s32 priority, s32 group);
extern s32   winMsgOpenFight(void* handle, s32 a, s32 b);
extern s32   winMsgCloseFight(s32 slot);

/* Event dispatch */
extern u8    menuIsCheck(s32 slot);               /* Check if event slot busy */
extern s32   menuClose(s32 slot);               /* Cancel active event */
extern s32   menuOpenCustom(s32 slot, ...);           /* Dispatch scene event */
extern void  menuCloseCustom(s32 slot, s32 p1, s32 p2);  /* Event cleanup */
extern s32   menuGetCursorItemID(s32 slot);               /* Get event result code */
extern s32   menuOpen(s32 slot, s32 p1);       /* Query event state */
extern void  menuSetPosition(s32 slot, s32 p1, s32 p2);  /* Set event params */

/* Resource management */
extern void* fightEncountDataBiosGetPtr(u16 id);                   /* Get scene resource table */
extern void  fightEncountDataBiosSetFightKind(void* tbl, u8 idx);      /* Set resource field 0 */
extern void  fightEncountDataBiosSetTrainer(void* tbl, u8 idx);      /* Set resource field 1 */
extern void  fightEncountDataBiosSetFightFloorDataId(void* tbl, u16 val);     /* Set resource field 2 */
extern u16   fightEncountDataBiosGetFightTrainerDataId(void* tbl, s32 slot);    /* Get resource subfield */
extern void  fightEncountDataBiosSetFightTrainerDataId(void* tbl, s32 slot, u16 val);  /* Set resource subfield */
extern void  fightEncountDataBiosSetGSInputDevice(void* tbl, s32 slot, s32 val);  /* Set resource property */
extern u8    fightEncountDataBiosGetFightKind(void* tbl);              /* Get resource type */
extern u8    fightEncountDataBiosGetTrainer(void* tbl);              /* Get resource subtype */
extern u16   fightEncountDataBiosGetFightFloorDataId(void* tbl);              /* Get resource size */
extern u16   fightEncountDataBiosGetGSInputDevice(void* tbl, s32 slot);    /* Get resource slot value */

/* Random selection helpers */
extern u8    menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(volatile unsigned int count, u32* outResult, void* validationFn);
extern void  menuSubCloseNumberInput(void);

/* Callback function pointers used in validation */
extern void* _dbgMenuFightGetFightKindDataIdSub(u32 id);   /* Validate NPC index (type byte) */
extern void* fn_800086EC(u32 id);   /* Validate NPC index (type byte) */
extern void* _dbgMenuFightGetFightFloorDataIdSub(u32 id);   /* Validate NPC index (type u16) */
extern void* fn_8000868C(u32 id);   /* Validate scene item index */
extern void* fn_800085D8(s32 difficulty);   /* Validate encounter difficulty */
extern void* _dbgMenuFightGetZokuseiDataIdSub(u32 id);
extern void* fn_800087FC(u32 id);
extern u8 fn_801EF63C(void);
extern u32 lbl_8047A278;
extern u32 lbl_80478F00;
extern u32 lbl_80478F40;
extern u32 lbl_80478F48;
s32 fn_80006908(u16 id);

/* =========================================================================
 * SDA global variables
 * ========================================================================= */

extern u16  gCurrentSceneId;     /* lbl_8047A288 : .sbss -- current scene/map halfword */
extern u16  lbl_8047A288;
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

#if !defined(GS_TASK_RANGE_SPLIT) || defined(GS_TASK_RANGE_EXACT_80006630)
/* =========================================================================
 * Function: dbgMenuCameraResetFloor (orphan fiction "GStask_InitCamera" renamed --
 *           address 0x80006630 / size 0x24 matches config/GC6E01/symbols.txt
 *           exactly, and this was the only code in the tree covering that
 *           address; the unmatched target had no other candidate)
 * Address:  0x80006630
 * Size:     0x24
 *
 * Simple wrapper that calls GSscene_CameraSetPosition to reset the
 * camera, then returns 0. Called at the start of scene transitions.
 * ========================================================================= */
#pragma push
#pragma scheduling off
s32 dbgMenuCameraResetFloor(void) {
    cameraResetFloor();
    return 0;
}
#pragma pop

/* =========================================================================
 * Function: dbgMenuCameraChangeDisp (orphan fiction "GStask_LoadTopMenu" renamed --
 *           address/size match symbols.txt exactly)
 * Address:  0x80006654
 * Size:     0x70
 *
 * Checks if archive slot 6 is busy; if so, cancels it and returns 0.
 * Otherwise, dispatches a new archive load for the top menu FSYS with
 * default parameters and sets up the viewport at (0x14, 0x104).
 * ========================================================================= */
#pragma push
#pragma scheduling off
#pragma peephole off
s32 dbgMenuCameraChangeDisp(void) {
    if (menuIsCheck(6) != 0) {
        menuClose(6);
    } else {
        menuOpenCustom(6, 0, 0, 0, 1, 0);
        menuSetPosition(6, 0x14, 0x104);
    }
    return 0;
}
#pragma pop

/* =========================================================================
 * Function: dbgMenuCameraSetType (orphan fiction "GStask_SetSceneType"
 *           renamed -- address 0x800066C4 / size 0x60 matches
 *           config/GC6E01/symbols.txt exactly; the retail symbol name is a
 *           reused debug-menu name, not this function's real purpose)
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
#pragma push
#pragma scheduling off
#pragma peephole off
s32 dbgMenuCameraSetType(void* unused, s32 mode) {
    switch (mode) {
    case 0:
    default:
        GSscene_SetMode(0);
        break;
    case 1:
        GSscene_SetMode(5);
        break;
    case 2:
        GSscene_SetMode(6);
        break;
    }
    return 0;
}
#pragma pop

#endif /* GS_TASK_RANGE_EXACT_80006630 */

#if !defined(GS_TASK_RANGE_SPLIT) || defined(GS_TASK_RANGE_RESIDUAL_80006724)
/* =========================================================================
 * Function: fn_80006724 (orphan fiction "GStask_LoadPDAMenu" renamed --
 *           address/size match symbols.txt exactly)
 * Address:  0x80006724
 * Size:     0x58
 *
 * Loads the PDA menu archive (priority 3) into the FSYS system.
 * Uses the current scene ID from lbl_8047A288 as a context parameter.
 * On success, activates the archive and returns 1; on failure returns -1.
 * ========================================================================= */
#pragma push
#pragma scheduling off
s32 fn_80006724(void) {
    void* result;

    result = fightTrainerGetStatus(0, gCurrentSceneId, 8, 3);
    if (result == NULL) {
        return -1;
    }
    winMsgOpenFight(result, 1, 1);
    winMsgCloseFight(1);
    return 1;
}

/* =========================================================================
 * Function: fn_8000677C (orphan fiction "GStask_LoadPocketMenu" renamed --
 *           address/size match symbols.txt exactly)
 * Address:  0x8000677C
 * Size:     0x58
 *
 * Loads the pocket/bag menu archive (priority 2).
 * Same structure as fn_80006724 but with priority 2.
 * ========================================================================= */
s32 fn_8000677C(void) {
    void* result;

    result = fightTrainerGetStatus(0, gCurrentSceneId, 8, 2);
    if (result == NULL) {
        return -1;
    }
    winMsgOpenFight(result, 1, 1);
    winMsgCloseFight(1);
    return 1;
}

/* =========================================================================
 * Function: fn_800067D4 (orphan fiction "GStask_LoadPCBoxMenu" renamed --
 *           address/size match symbols.txt exactly)
 * Address:  0x800067D4
 * Size:     0x58
 *
 * Loads the PC box menu archive (priority 1).
 * ========================================================================= */
s32 fn_800067D4(void) {
    void* result;

    result = fightTrainerGetStatus(0, gCurrentSceneId, 8, 1);
    if (result == NULL) {
        return -1;
    }
    winMsgOpenFight(result, 1, 1);
    winMsgCloseFight(1);
    return 1;
}

/* =========================================================================
 * Function: fn_8000682C (orphan fiction "GStask_LoadBattleMenu" renamed --
 *           address/size match symbols.txt exactly)
 * Address:  0x8000682C
 * Size:     0x58
 *
 * Loads the battle menu archive (priority 0 -- highest).
 * ========================================================================= */
s32 fn_8000682C(void) {
    void* result;

    result = fightTrainerGetStatus(0, gCurrentSceneId, 8, 0);
    if (result == NULL) {
        return -1;
    }
    winMsgOpenFight(result, 1, 1);
    winMsgCloseFight(1);
    return 1;
}
#pragma pop

#endif /* GS_TASK_RANGE_RESIDUAL_80006724 */

typedef struct GsTaskCountTable {
    u32 count;
} GsTaskCountTable;

extern GsTaskCountTable* lbl_80478F20;

#if !defined(GS_TASK_RANGE_SPLIT) || defined(GS_TASK_RANGE_EXACT_80006884)
#pragma push
#pragma peephole off
s32 fn_80006884(void) {
    u32 result;
    s32 val;
    s32 ret;
    u16 selected;

    if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(lbl_8047A288, &result, fn_8000868C) == 0) {
        val = -1;
    } else {
        if (result >= lbl_80478F20->count) {
            result = lbl_80478F20->count - 1;
        }
        menuSubCloseNumberInput();
        val = (s32)result;
    }

    if (val < 0) {
        return -1;
    }

    selected = (u16)val;
    ret = 1;
    lbl_8047A288 = selected;
    return ret;
}
#pragma pop
#endif /* GS_TASK_RANGE_EXACT_80006884 */

/* =========================================================================
 * Remaining functions in this module (0x80006884 - 0x80009178) are not
 * fully decompiled here. They follow the same patterns:
 *
 * fn_80006884 (GStask_SelectRandomNPC):
 *   Uses menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs to randomly select an NPC from the scene table,
 *   bounds-checks against lbl_80478F20, stores result in lbl_8047A288.
 *
 * fn_80006908 (GStask_ProcessSceneEvent):
 *   The largest function (0x6A4 bytes) - a massive switch statement that
 *   dispatches scene events based on event codes returned by menuGetCursorItemID.
 *   Uses jumptable_802E28D0 for the initial dispatch, then performs
 *   NPC selection, resource loading, and encounter setup based on the
 *   event type. Each case in the switch:
 *     - Gets a resource from the scene table via fightEncountDataBiosGetFightKind/fightEncountDataBiosGetTrainer
 *     - Validates the result using menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs with a callback
 *     - Commits the selection via menuSubCloseNumberInput
 *     - Updates the resource table via fightEncountDataBiosSetFightKind/fightEncountDataBiosSetTrainer/etc.
 *
 * fn_80006FAC - dbgMenuFightStopHostWin (accessor cluster):
 *   Small field accessors for the scene resource structure.
 *
 * dbgMenuFightFightPokemonSelect5 - dbgMenuFightFightPokemonSelect0 (archive load helpers):
 *   Six nearly identical functions that load archives for slots 0-5,
 *   each following the same load/activate/commit pattern.
 *
 * menuFightPokemonSelectSub (GStask_InitSceneResources):
 *   0x2F8 bytes. Initializes the resource table for a new scene,
 *   setting up all model/texture/script entries.
 *
 * fn_80007B30 (GStask_MainUpdate):
 *   0x4AC bytes. The main task update state machine that coordinates
 *   archive loading, scene transitions, and event processing.
 *
 * dbgMenuFightGetMsgSpeedToFrame - _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub (finalization):
 *   Finalize loaded resources and build the active resource list.
 * ========================================================================= */

/* Accessor functions (0x80008390 - 0x80009178) follow a repeating pattern:
 *
 * _dbgMenuFightGetFightTrainerPokemonPartDataIdSub (size 0x6C) - Get party slot 0 species
 * _dbgMenuFightGetWazaDataIdSub (size 0x64) - Get party slot 0 level
 * dbgMenuFightGetWazaTypeId (size 0x60) - Get party slot 0 HP
 * _dbgMenuFightGetWazaTypeIdSub (size 0x58) - Get party slot 0 status
 * dbgMenuFightGetZokuseiDataId (size 0x64) - Get party slot 1 species
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
extern void* GSmsgGetGSchar(u32 id);
extern void* fn_8010C4D4(u16 index);
extern void* fightEncountDataBiosGetFightName(void* ptr);

/* Global state references */
extern u32 lbl_80478B38;

extern u8 lbl_8047882E;
extern u8 lbl_8047A271;
extern u8 lbl_8047A280;
extern u16 lbl_8047A282;
extern u8 lbl_8047A284;
extern u8 lbl_8047A285;
extern u8 lbl_8047A286;

/* 0x80008868 | 0x3D8 */
#if !defined(GS_TASK_RANGE_SPLIT) || defined(GS_TASK_RANGE_RESIDUAL_80008868)
extern u32 menuDataBiosGetType(u32);
extern void fn_8001EA98(s32, s32, s32, s32);
extern void fn_8001E4B4(s32, s32, s32, s32, s32, s32);
extern s32 sprintf(char*, const char*, ...);
extern s32 fn_800FAEF8(s32, s32, s32, ...);
extern u32 fn_801906A0(u32);
extern u32 lbl_80478F98;
extern u8 lbl_80478838[8];
extern u8 lbl_803A19C8[];
extern u8 lbl_80266688[];
extern u32 lbl_80478F9C;
extern u8 lbl_8047B6C8[3];
typedef struct GsTaskFlagConfig {
    u32 count;
    s16 first;
} GsTaskFlagConfig;

typedef struct GsTaskFlagDef {
    u8 fields[6];
    s16 next;
} GsTaskFlagDef;

void fn_80008868(u8* window)
{
    s32 width;
    s32 pageSize;
    s32 visibleCount;
    s32 rowHeight;
    s32 row;
    s32 rowY;
    s32 color;
    s32 flagIndex;
    s32 linkIndex;
    s32 flagCount;
    s32 center;
    s32 right;
    s16 flagId;
    GsTaskFlagConfig* config;
    GsTaskFlagDef* flags;
    u32 menuId;

    menuId = *(u32*)(window + 4);
    rowY = 40;
    pageSize = (u8)menuDataBiosGetType(menuId);
    width = 480;
    config = (GsTaskFlagConfig*)lbl_80478F98;
    flagCount = config->count;
    visibleCount = flagCount;
    if (flagCount > pageSize) {
        visibleCount = pageSize;
    }

    rowHeight = visibleCount * 13;
    fn_8001EA98(30, 40, width + 25, rowHeight);

    if (*(s16*)lbl_80478838 > 0) {
        center = width / 2;
        fn_8001E4B4(center + 20, 33, center + 40, 33, center + 30, 23);
    }

    config = (GsTaskFlagConfig*)lbl_80478F98;
    if ((s32)config->count - *(s16*)lbl_80478838 > pageSize) {
        center = width / 2;
        fn_8001E4B4(center + 20, rowHeight + 47, center + 40,
                    rowHeight + 47, center + 30, rowHeight + 57);
    }

    row = 0;
    right = width;
    while (row < visibleCount) {
        if (*(s16*)(lbl_80478838 + 2) == row) {
            color = 0xFF0000FF;
        } else {
            color = -1;
        }

        flagIndex = row + *(s16*)lbl_80478838;
        if (flagIndex < 0) {
            flagId = 0;
        } else {
            config = (GsTaskFlagConfig*)lbl_80478F98;
            if ((u32)flagIndex >= config->count) {
                flagId = 0;
            } else {
                flagId = config->first;
                flags = (GsTaskFlagDef*)lbl_80478F9C;
                linkIndex = 0;
                for (; linkIndex < flagIndex; linkIndex++) {
                    flagId = flags[flagId].next;
                }
            }
        }

        sprintf((char*)lbl_803A19C8, (const char*)lbl_80266688, flagIndex,
                (u16)flagId);
        fn_800FAEF8(30, rowY, color, lbl_803A19C8);

        flagIndex = row + *(s16*)lbl_80478838;
        if (flagIndex < 0) {
            flagId = 0;
        } else {
            config = (GsTaskFlagConfig*)lbl_80478F98;
            if ((u32)flagIndex >= config->count) {
                flagId = 0;
            } else {
                flagId = config->first;
                flags = (GsTaskFlagDef*)lbl_80478F9C;
                linkIndex = 0;
                for (; linkIndex < flagIndex; linkIndex++) {
                    flagId = flags[flagId].next;
                }
            }
        }

        fn_800FAEF8(right + 30, rowY, color, lbl_8047B6C8,
                    fn_801906A0((u16)flagId));
        rowY += 13;
        row++;
    }
}

/* 0x80008C40 | 0x538 */
extern u8* windowGetKeyInfo(void);
extern void fn_80190528(u32);
extern void fn_801903B0(u32);
extern u32 lbl_80478F98;
extern u32 lbl_80478F9C;
static u16 GStaskGetLinkedEntryId(s32 index) {
    u8* list;
    s16 node;
    s32 i;

    if (index < 0) {
        return 0;
    }
    list = (u8*)lbl_80478F98;
    if ((u32)index >= *(u32*)list) {
        return 0;
    }

    node = *(s16*)(list + 4);
    for (i = 0; i < index; i++) {
        node = *(s16*)((u8*)lbl_80478F9C + node * 8 + 6);
    }
    if (index != 0) {
        node = *(s16*)((u8*)lbl_80478F9C + node * 8 + 6);
    }
    return (u16)node;
}

void fn_80008C40(u8* window) {
    u16 repeat;
    u16 trigger;
    s16 top;
    s16 cursor;
    s16 count;
    s16 page;
    s32 value;
    u32 state;

    repeat = *(u16*)(windowGetKeyInfo() + 6);
    trigger = *(u16*)(windowGetKeyInfo() + 0);
    count = (s16)*(u32*)((u8*)lbl_80478F98 + 0);
    page = (u8)menuDataBiosGetType(*(u32*)(window + 4));
    if (count < (s16)page) {
        page = count;
    }

    state = *(u32*)lbl_80478838;
    top = (s16)(state >> 16);
    cursor = (s16)state;

    if (repeat & 1) {
        cursor--;
    } else if (repeat & 2) {
        cursor++;
    }

    if (repeat & 4) {
        cursor = (s16)(cursor - ((u8)menuDataBiosGetType(*(u32*)(window + 4)) - 1));
    } else if (repeat & 8) {
        cursor = (s16)(cursor + ((u8)menuDataBiosGetType(*(u32*)(window + 4)) - 1));
    }

    if (cursor < 0) {
        top = (s16)(top + cursor);
        cursor = 0;
        if (top < 0) {
            cursor = (s16)(page - 1);
            top = (s16)(count - page);
        }
    } else if (cursor >= page) {
        value = page - 1;
        top = (s16)(top + (cursor - value));
        cursor = (s16)value;
        if (top + cursor >= count) {
            top = 0;
            cursor = 0;
        }
    }

    value = top + cursor;
    if (trigger & 0x80) {
        fn_80190528(GStaskGetLinkedEntryId(value));
    } else if (trigger & 0x40) {
        fn_801903B0(GStaskGetLinkedEntryId(value));
    }

    *(u32*)lbl_80478838 = ((u16)top << 16) | (u16)cursor;
    *(u32*)(window + 0x80) = GStaskGetLinkedEntryId(top + cursor);
}
#endif /* GS_TASK_RANGE_RESIDUAL_80008868 */

#if !defined(GS_TASK_RANGE_SPLIT) || defined(GS_TASK_RANGE_RESIDUAL_80006908)
/* ===== Phase 2 recovery stubs ===== */

/* fn_80006908 - 0x80006908 | size: 0x6a4 */
extern u32 lbl_80478F50;
extern u32 lbl_80478F40;
extern u32 lbl_80478F00;
extern u32 lbl_80478F48;
typedef struct GsTaskEncounterBackup {
    u32 words[14];
} GsTaskEncounterBackup;

#pragma push
#pragma peephole off
s32 fn_80006908(u16 id)
{
    void* encounter;
    GsTaskEncounterBackup backup;
    u32 fightKind;
    u32 trainer;
    u32 floor;
    u32 trainer0;
    u32 device0;
    u32 trainer1;
    u32 device1;
    u32 trainer2;
    u32 device2;
    u32 trainer3;
    u32 device3;
    s32 event;
    s32 value;

    if (id == 0) {
        return 1;
    }
    if (id >= *(u32*)(void*)lbl_80478F50) {
        return 1;
    }

    encounter = fightEncountDataBiosGetPtr(id);
    backup = *(GsTaskEncounterBackup*)encounter;

    for (;;) {
        value = menuOpenCustom(0xCD, 0, 0, 0, 1, 1, id);
        if (value == -1) {
            menuCloseCustom(0xCD, 0, 1);
            *(GsTaskEncounterBackup*)encounter = backup;
            return -1;
        }
        if (value == -2) {
            if (menuOpen(0x44, 1) != 0) {
                menuCloseCustom(0x44, 0, 1);
                continue;
            }
            menuCloseCustom(0x44, 0, 1);
            break;
        }

        event = menuGetCursorItemID(0xCD);
        switch (event) {
        case 0xD3F:
            if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(
                    fightEncountDataBiosGetFightKind(encounter), &fightKind,
                    _dbgMenuFightGetFightKindDataIdSub) == 0) {
                value = -1;
            } else {
                if (fightKind >= *(u32*)(void*)lbl_80478F40) {
                    fightKind = *(u32*)(void*)lbl_80478F40 - 1;
                }
                menuSubCloseNumberInput();
                value = fightKind;
            }
            if (value >= 0) {
                fightEncountDataBiosSetFightKind(encounter, (u8)value);
            }
            break;

        case 0xD40:
            if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(
                    fightEncountDataBiosGetTrainer(encounter), &trainer,
                    fn_800086EC) == 0) {
                value = -1;
            } else {
                if (trainer >= *(u32*)(void*)lbl_80478F00) {
                    trainer = *(u32*)(void*)lbl_80478F00 - 1;
                }
                menuSubCloseNumberInput();
                value = trainer;
            }
            if (value >= 0) {
                fightEncountDataBiosSetTrainer(encounter, (u8)value);
            }
            break;

        case 0xD41:
            if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(
                    fightEncountDataBiosGetFightFloorDataId(encounter), &floor,
                    _dbgMenuFightGetFightFloorDataIdSub) == 0) {
                value = -1;
            } else {
                if (floor >= *(u32*)(void*)lbl_80478F48) {
                    floor = *(u32*)(void*)lbl_80478F48 - 1;
                }
                menuSubCloseNumberInput();
                value = floor;
            }
            if (value >= 0) {
                fightEncountDataBiosSetFightFloorDataId(encounter, (u16)value);
            }
            break;

        case 0xD42:
            if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(
                    fightEncountDataBiosGetFightTrainerDataId(encounter, 0),
                    &trainer0, fn_8000868C) == 0) {
                value = -1;
            } else {
                if (trainer0 >= lbl_80478F20->count) {
                    trainer0 = lbl_80478F20->count - 1;
                }
                menuSubCloseNumberInput();
                value = trainer0;
            }
            if (value < 0) {
                break;
            }
            fightEncountDataBiosSetFightTrainerDataId(encounter, 0, (u16)value);

            if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(
                    fightEncountDataBiosGetGSInputDevice(encounter, 0), &device0,
                    fn_800085D8) == 0) {
                value = -1;
            } else {
                if (device0 >= 5) {
                    device0 = 4;
                }
                menuSubCloseNumberInput();
                value = device0;
            }
            if (value >= 0) {
                fightEncountDataBiosSetGSInputDevice(encounter, 0, value);
            }
            break;

        case 0xD43:
            if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(
                    fightEncountDataBiosGetFightTrainerDataId(encounter, 1),
                    &trainer1, fn_8000868C) == 0) {
                value = -1;
            } else {
                if (trainer1 >= lbl_80478F20->count) {
                    trainer1 = lbl_80478F20->count - 1;
                }
                menuSubCloseNumberInput();
                value = trainer1;
            }
            if (value < 0) {
                break;
            }
            fightEncountDataBiosSetFightTrainerDataId(encounter, 1, (u16)value);

            if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(
                    fightEncountDataBiosGetGSInputDevice(encounter, 1), &device1,
                    fn_800085D8) == 0) {
                value = -1;
            } else {
                if (device1 >= 5) {
                    device1 = 4;
                }
                menuSubCloseNumberInput();
                value = device1;
            }
            if (value >= 0) {
                fightEncountDataBiosSetGSInputDevice(encounter, 1, value);
            }
            break;

        case 0xD44:
            if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(
                    fightEncountDataBiosGetFightTrainerDataId(encounter, 2),
                    &trainer2, fn_8000868C) == 0) {
                value = -1;
            } else {
                if (trainer2 >= lbl_80478F20->count) {
                    trainer2 = lbl_80478F20->count - 1;
                }
                menuSubCloseNumberInput();
                value = trainer2;
            }
            if (value < 0) {
                break;
            }
            fightEncountDataBiosSetFightTrainerDataId(encounter, 2, (u16)value);

            if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(
                    fightEncountDataBiosGetGSInputDevice(encounter, 2), &device2,
                    fn_800085D8) == 0) {
                value = -1;
            } else {
                if (device2 >= 5) {
                    device2 = 4;
                }
                menuSubCloseNumberInput();
                value = device2;
            }
            if (value >= 0) {
                fightEncountDataBiosSetGSInputDevice(encounter, 2, value);
            }
            break;

        case 0xD45:
            if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(
                    fightEncountDataBiosGetFightTrainerDataId(encounter, 3),
                    &trainer3, fn_8000868C) == 0) {
                value = -1;
            } else {
                if (trainer3 >= lbl_80478F20->count) {
                    trainer3 = lbl_80478F20->count - 1;
                }
                menuSubCloseNumberInput();
                value = trainer3;
            }
            if (value < 0) {
                break;
            }
            fightEncountDataBiosSetFightTrainerDataId(encounter, 3, (u16)value);

            if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(
                    fightEncountDataBiosGetGSInputDevice(encounter, 3), &device3,
                    fn_800085D8) == 0) {
                value = -1;
            } else {
                if (device3 >= 5) {
                    device3 = 4;
                }
                menuSubCloseNumberInput();
                value = device3;
            }
            if (value >= 0) {
                fightEncountDataBiosSetGSInputDevice(encounter, 3, value);
            }
            break;
        }
    }

    menuCloseCustom(0xCD, 0, 1);
    return 1;
}
#pragma pop
#endif /* GS_TASK_RANGE_RESIDUAL_80006908 */

#if !defined(GS_TASK_RANGE_SPLIT) || defined(GS_TASK_RANGE_EXACT_80006FAC)
/* fn_80006FAC - 0x80006FAC | size: 0xdc */
extern u32 fn_800F7AF0(s32 port);
extern u32 fn_800F7BC4(s32 port);
#if 1
#pragma peephole off
void menuFightButtonNormal(u8* ctx) {
    u32 buttons;
    u32 held;

    if (ctx == 0) return;

    buttons = fn_800F7AF0(1);
    held = fn_800F7BC4(1);
    if ((held & buttons) & 0x100) {
        *(u8*)(ctx + 0x98) = 1;
        *(u32*)(ctx + 0x80) = menuGetCursorItemID(*(u32*)(ctx + 0x4));
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

/* dbgMenuFightStopTimeOut - 0x80007088 | size: 0x44 */
extern u8   fn_801EF63C(void);
extern void fightFloorSetTimeOutAllFightResult(s32 mode);
extern void dbgMenuClose(void);
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightStopTimeOut(void) {
    if (fn_801EF63C() == 0) {
        return -1;
    }
    fightFloorSetTimeOutAllFightResult(0);
    dbgMenuClose();
    return 0;
}
#pragma pop
#endif

/* dbgMenuFightStopHostLose - 0x800070CC | size: 0x44 */
extern void fn_801EF62C(s32 mode);
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightStopHostLose(void) {
    if (fn_801EF63C() == 0) {
        return -1;
    }
    fn_801EF62C(3);
    dbgMenuClose();
    return 0;
}
#pragma pop
#endif

/* dbgMenuFightStopHostWin - 0x80007110 | size: 0x44 */
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightStopHostWin(void) {
    if (fn_801EF63C() == 0) {
        return -1;
    }
    fn_801EF62C(2);
    dbgMenuClose();
    return 0;
}
#pragma pop
#endif

/* dbgMenuFightFightPokemonSelect5 - 0x80007154 | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl menuFightPokemonSelectSub vs bl menuFightPokemonSelectSub (same addr) */
extern s32  menuFightPokemonSelectSub();
extern u32  fightTrainerGetValidFightPokemonPtr(u32 ptr, s32 slot);
extern u32  lbl_8047A278;
extern u32  lbl_8047A27C;
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightFightPokemonSelect5(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A27C = fightTrainerGetValidFightPokemonPtr(lbl_8047A278, 5);
    if (lbl_8047A27C == 0) return -1;
    return menuFightPokemonSelectSub();
}
#pragma pop
#endif

/* dbgMenuFightFightPokemonSelect4 - 0x800071AC | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl menuFightPokemonSelectSub vs bl menuFightPokemonSelectSub (same addr) */
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightFightPokemonSelect4(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A27C = fightTrainerGetValidFightPokemonPtr(lbl_8047A278, 4);
    if (lbl_8047A27C == 0) return -1;
    return menuFightPokemonSelectSub();
}
#pragma pop
#endif

/* dbgMenuFightFightPokemonSelect3 - 0x80007204 | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl menuFightPokemonSelectSub vs bl menuFightPokemonSelectSub (same addr) */
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightFightPokemonSelect3(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A27C = fightTrainerGetValidFightPokemonPtr(lbl_8047A278, 3);
    if (lbl_8047A27C == 0) return -1;
    return menuFightPokemonSelectSub();
}
#pragma pop
#endif

/* dbgMenuFightFightPokemonSelect2 - 0x8000725C | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl menuFightPokemonSelectSub vs bl menuFightPokemonSelectSub (same addr) */
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightFightPokemonSelect2(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A27C = fightTrainerGetValidFightPokemonPtr(lbl_8047A278, 2);
    if (lbl_8047A27C == 0) return -1;
    return menuFightPokemonSelectSub();
}
#pragma pop
#endif

/* dbgMenuFightFightPokemonSelect1 - 0x800072B4 | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl menuFightPokemonSelectSub vs bl menuFightPokemonSelectSub (same addr) */
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightFightPokemonSelect1(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A27C = fightTrainerGetValidFightPokemonPtr(lbl_8047A278, 1);
    if (lbl_8047A27C == 0) return -1;
    return menuFightPokemonSelectSub();
}
#pragma pop
#endif

/* dbgMenuFightFightPokemonSelect0 - 0x8000730C | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl menuFightPokemonSelectSub vs bl menuFightPokemonSelectSub (same addr) */
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightFightPokemonSelect0(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A27C = fightTrainerGetValidFightPokemonPtr(lbl_8047A278, 0);
    if (lbl_8047A27C == 0) return -1;
    return menuFightPokemonSelectSub();
}
#pragma pop
#endif
#endif /* GS_TASK_RANGE_EXACT_80006FAC */

/* menuFightPokemonSelectSub - 0x80007364 | size: 0x2f8 */
extern void* fightFloorGetStatus(s32 a, u16 b, s32 c, s32 d);
extern void* fightPokemonGetPokemonPtr(u32 ctx);
extern void* fightFloorGetFightPokemonPtrToFightTrainerPtr(s32 a, u32 ctx);
extern u8   figthPokemonGetLevel(u32 ctx);
extern u8   fightTrainerCheckCanIrekaeFightPokemon(void* ptr, u32 ctx);
extern void* fightTrainerGetFightPokemonPtrToFightOutPokemonPtr(void* ptr, u32 ctx);
extern u8   fightOutPokemonIsUseHensinBuff(void);
extern void fightOutPokemonGetTokuseiDataId(void* ptr);
extern void fightOutPokemonGetZokuseiDataId(void* ptr, s32 a);
extern s32  fn_800096B4(void* ptr, s32 a, u8* b, u8* c, u8* d, u8* e);
extern u32  pokemonGetStatus(void* ptr, u16 b, s32 c, s32 d);
extern void fn_8010B01C(void* ptr, s32 a, s32 b);
extern void _threadSwitch(void);
extern void fn_8010BBB8(void* ptr);
extern s8   fn_8010BCE4(void);
extern void* pokemonCreateSequence(void* ptr);
extern void battleGridReplacePokemon(void* ptr, void* src);
extern void battleGridUpdate(void);
extern void pokemonSetStatus(void* ptr, s32 a, s32 b, s32 c, s32 d);
extern void fn_801DB100(void* ptr);
extern void fn_801DA4E8(void* ptr, s32 a);
extern void fightOutPokemonWriteJoutaiDataId(void* ptr, s32 a);
extern void fightMenuFightOutPokemonRenewStatusMenu(void* ptr, u16 a, s32 b);
extern void pokemonSetSequenceStatus(void* a, u32 b);
extern void* pokemonGetTokuseiDataId(void* ptr);
extern void fightOutPokemonSetTokuseiDataId(void* ptr, void* a);
extern void fightOutPokemonSetZokuseiDataId(void* ptr, u8 a, u16 b);
extern void fightMenuFightTrainerRenewStatusMenu(void* ptr, u16 a);
#if !defined(GS_TASK_RANGE_SPLIT) || defined(GS_TASK_RANGE_RESIDUAL_80007364)
s32 menuFightPokemonSelectSub(u32 ctx) {
    s32 result;
    u8 prevLevel;
    u16 savedId;
    void* encounter;
    void* archive;
    void* scene;
    u16 itemId;
    u32 subItem;
    u8 sp_b;
    u8 sp_a;
    u8 sp_9;
    u8 sp_8;
    u16 loopIdx;

    sp_b = 0;
    sp_a = 0;
    sp_9 = 0;
    sp_8 = 0;

    savedId = (u16)(u32)fightFloorGetStatus(0, 0, 0x14, 0);
    scene = fightPokemonGetPokemonPtr(ctx);
    if (scene == 0) {
        return -1;
    }

    archive = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, ctx);
    if (archive == 0) {
        return -1;
    }

    prevLevel = figthPokemonGetLevel(ctx);

    if (fightTrainerCheckCanIrekaeFightPokemon(archive, ctx) == 2) {
        encounter = fightTrainerGetFightPokemonPtrToFightOutPokemonPtr(archive, ctx);
        if (fightOutPokemonIsUseHensinBuff() == 1) {
            return -1;
        }

        if (encounter != 0) {
            fightOutPokemonGetTokuseiDataId(encounter);
            for (loopIdx = 0; (u16)loopIdx < 2; loopIdx++) {
                fightOutPokemonGetZokuseiDataId(encounter, 0);
            }
        }

        result = fn_800096B4(scene, 1, &sp_b, &sp_a, &sp_9, &sp_8);
        if (result == 1) {
            itemId = pokemonGetStatus(scene, 0, 0x6e, 0);
            if (encounter != 0) {
                subItem = pokemonGetStatus(encounter, 0, 0xee, 0);
                if (subItem != 0) {
                    void* animPtr;

                    fn_8010B01C(scene, 0, 0);
                    while ((fn_8010BBB8(scene), fn_8010BCE4()) == 0) {
                        _threadSwitch();
                    }
                    animPtr = pokemonCreateSequence(scene);
                    battleGridReplacePokemon((void*)(u32)subItem, animPtr);
                    battleGridUpdate();
                    pokemonSetStatus(encounter, 0, 0xee, 0, (s32)animPtr);
                    fn_801DB100((void*)(u32)subItem);
                    fn_801DA4E8(animPtr, 1);
                    fightOutPokemonWriteJoutaiDataId(encounter, 0x14);
                }
                {
                    u16 newId;

                    newId = (u16)(u32)fightFloorGetStatus(0, 0, 0x14, 0);
                    fightMenuFightOutPokemonRenewStatusMenu(encounter, newId, 1);
                }
                pokemonSetSequenceStatus(
                    scene, pokemonGetStatus(encounter, 0, 0xee, 0));
                fightOutPokemonSetTokuseiDataId(
                    encounter, pokemonGetTokuseiDataId(scene));
                {
                    u32 i;

                    for (i = 0; (u16)i < 2; i++) {
                        u16 val;

                        val = pokemonGetStatus(0, itemId, 0x16, i);
                        fightOutPokemonSetZokuseiDataId(encounter, (u8)i, val);
                    }
                }
            }
        }
    } else {
        result = fn_800096B4(scene, 1, 0, 0, 0, 0);
    }

    if ((u8)figthPokemonGetLevel(ctx) > (u8)prevLevel) {
        pokemonSetStatus((void*)ctx, 0, 0xd0, 0, 1);
    }

    fightMenuFightTrainerRenewStatusMenu(archive, savedId);
    return result;
}
#endif /* GS_TASK_RANGE_RESIDUAL_80007364 */

#if !defined(GS_TASK_RANGE_SPLIT) || defined(GS_TASK_RANGE_EXACT_8000765C)
/* dbgMenuFightFightTrainerPokemonPartDataEdit - 0x8000765C | size: 0xac */
extern void* _dbgMenuFightGetFightTrainerPokemonPartDataIdSub(u32 id);
extern void fn_80051710(u16 id);
extern u16 lbl_80478830;
extern u32 lbl_80478F08;
#if 1
#pragma peephole off
s32 dbgMenuFightFightTrainerPokemonPartDataEdit(void) {
    u32 result;
    s32 val;

    if (fn_801EF63C() == 0) {
        return -1;
    }

    for (;;) {
        if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(lbl_80478830, &result, _dbgMenuFightGetFightTrainerPokemonPartDataIdSub) == 0) {
            val = -1;
        } else {
            if (result >= *(u32*)lbl_80478F08) {
                result = *(u32*)lbl_80478F08 - 1;
            }
            menuSubCloseNumberInput();
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

/* dbgMenuFightFightTrainerAiDataEdit - 0x80007708 | size: 0x70 */
extern s32  fn_80051E38(u32 slot);
#if 1
#pragma peephole off
s32 dbgMenuFightFightTrainerAiDataEdit(void) {
    u16 tmp;
    u16 slot;
    tmp  = (u16)(u32)fightTrainerGetStatus((s32)lbl_8047A278, 0, 0x43, 0);
    slot = (u16)(u32)fightTrainerGetStatus(0, tmp, 2, 0);
    if (fn_801EF63C() == 0) return -1;
    return fn_80051E38(slot);
}
#pragma peephole on
#endif

/* fn_80007778 - 0x80007778 | size: 0x20 */
extern void fn_8004EADC(void);
#if 1
void fn_80007778(void) {
    fn_8004EADC();
}
#endif

/* fn_80007798 - 0x80007798 | size: 0x20 */
extern void fn_8004EC54(void);
#if 1
void fn_80007798(void) {
    fn_8004EC54();
}
#endif

/* fn_800077B8 - 0x800077B8 | size: 0x20 */
extern void fn_8004EDCC(void);
#if 1
void fn_800077B8(void) {
    fn_8004EDCC();
}
#endif

/* dbgMenuFightFightTrainerAiAddsubWazaDefpokemon - 0x800077D8 | size: 0x20 */
extern void fn_8004F860(void);
#if 1
void dbgMenuFightFightTrainerAiAddsubWazaDefpokemon(void) {
    fn_8004F860();
}
#endif

/* Address: 0x800077F8 | Size: 0x8 | Pattern: return_constant */
u32 fn_800077F8(void) { return 1; }

/* dbgMenuFightFightTrainerAiAddsubIrekaeDasu - 0x80007800 | size: 0x20 */
extern void fn_8004FE3C(void);
#if 1
void dbgMenuFightFightTrainerAiAddsubIrekaeDasu(void) {
    fn_8004FE3C();
}
#endif

/* fn_80007820 - 0x80007820 | size: 0x20 */
extern void fn_80050844(void);
#if 1
void fn_80007820(void) {
    fn_80050844();
}
#endif

/* Address: 0x80007840 | Size: 0x8 | Pattern: return_constant */
u32 fn_80007840(void) { return 1; }

/* fn_80007848 - 0x80007848 | size: 0xa4 */
extern void dbgMenuFightTrainerDataStatusInputDigit(u16 id, s32 a, s32 b, s32 c, s32 d);
extern u16 lbl_8047A28A;
extern u32 lbl_80478F28;
extern void* _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub(u32 id);
#if 1
#pragma peephole off
s32 fn_80007848(void) {
    u32 result;
    s32 val;

    for (;;) {
        if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(lbl_8047A28A, &result, _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub) == 0) {
            val = -1;
        } else {
            if (result > *(u32*)lbl_80478F28) {
                result = *(u32*)lbl_80478F28 - 1;
            }
            menuSubCloseNumberInput();
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

/* dbgMenuFightFightTrainerSelect1 - 0x800078EC | size: 0x58 */
extern u32  fightSideGetValidFightTrainerPtr(u32 ptr, s32 mode);
extern u32  lbl_8047A274;
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightFightTrainerSelect1(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A278 = fightSideGetValidFightTrainerPtr(lbl_8047A274, 1);
    if (lbl_8047A278 == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* dbgMenuFightFightTrainerSelect0 - 0x80007944 | size: 0x58 */
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightFightTrainerSelect0(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A278 = fightSideGetValidFightTrainerPtr(lbl_8047A274, 0);
    if (lbl_8047A278 == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* dbgMenuFightFightSideSelectHostEnemy - 0x8000799C | size: 0x58 */
extern u32  fightTargetGetPtrAsNowFightType(s32 a, s32 b);
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightFightSideSelectHostEnemy(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A274 = fightTargetGetPtrAsNowFightType(5, 0);
    if (lbl_8047A274 == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* dbgMenuFightFightSideSelectHost - 0x800079F4 | size: 0x58 */
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightFightSideSelectHost(void) {
    if (fn_801EF63C() == 0) return -1;
    lbl_8047A274 = fightTargetGetPtrAsNowFightType(4, 0);
    if (lbl_8047A274 == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* dbgMenuFightFightFloorEditTenkou - 0x80007A4C | size: 0x38 */
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightFightFloorEditTenkou(void) {
    if (fn_801EF63C() == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* dbgMenuFightWazaEdit - 0x80007A84 | size: 0xac | SYMBOL-NAME WALL 97.67%: bl dbgMenuFightWazaEditSub vs bl dbgMenuFightWazaEditSub (same addr) */
extern void dbgMenuFightWazaEditSub(u16 id);
extern u16 lbl_8047882A;
extern void* _dbgMenuFightGetWazaDataIdSub(u32 id);
#if 1
#pragma peephole off
s32 dbgMenuFightWazaEdit(void) {
    u32 result;
    s32 val;

    if (fn_801EF63C() == 0) {
        return -1;
    }

    for (;;) {
        if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(lbl_8047882A, &result, _dbgMenuFightGetWazaDataIdSub) == 0) {
            val = -1;
        } else {
            if (result >= 0x163) {
                result = 0x162;
            }
            menuSubCloseNumberInput();
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
extern u8 fightSeqGetEffectAminFlag(void);
extern void fightSeqSetEffectAminFlag(u8 val);
extern s8 menuSubOpenYesNo(s32 max, s32 a, s32 b, s32 initial);
extern u8 fn_8001E224(u8 val, u32* out, s32 a, s32 b, s32 c, s32 d);
extern u8 fightSeqCheckYubiwohuruWazaDataId(u16 id);
extern u8 lbl_80478828;
extern u8 lbl_8047A271;
extern u8 lbl_8047A270;
extern u8 lbl_8047A284;
extern u8 lbl_8047A285;
extern u8 lbl_8047A286;
extern u8 lbl_8047A280;
extern u16 lbl_8047A282;
extern u8 lbl_8047882E;
#if 1
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

    save_vol = lbl_80478828;
    save_stereo = lbl_8047A271;
    save_field = lbl_8047A270;
    save_battle = lbl_8047A284;
    save_speed = lbl_8047A285;
    save_sfx = fightSeqGetEffectAminFlag();
    save_rumble = lbl_8047A286;
    save_cam = lbl_8047A280;
    save_species = lbl_8047A282;
    save_nickname = lbl_8047882E;

    for (;;) {
        evt = menuOpenCustom(0xe, 0, 0, 0, 1, 0, 0);
        if (evt == -1) {
            menuCloseCustom(0xe, 0, 1);
            if (save_vol < 1) save_vol = 1;
            if ((u8)save_vol > 9) save_vol = 9;
            lbl_80478828 = save_vol;
            lbl_8047A271 = save_stereo;
            lbl_8047A270 = save_field;
            lbl_8047A284 = save_battle;
            lbl_8047A285 = save_speed;
            fightSeqSetEffectAminFlag(save_sfx);
            lbl_8047A286 = save_rumble;
            lbl_8047A280 = save_cam;
            lbl_8047A282 = save_species;
            lbl_8047882E = save_nickname;
            return -1;
        }
        if (evt == -2) {
            if (menuOpen(0x44, 1) != 0) {
                menuCloseCustom(0x44, 0, 1);
                continue;
            }
            menuCloseCustom(0x44, 0, 1);
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
            u32 tmp;
            ret = fn_8001E224(lbl_80478828, &tmp, 1, 0x32, 0x32, 0);
            if (ret == 0) {
                menuSubCloseNumberInput();
                continue;
            }
            {
                u8 v;
                v = (u8)tmp;
                if ((u8)v < 1) v = 1;
                if ((u8)v > 9) v = 9;
                lbl_80478828 = v;
            }
            menuSubCloseNumberInput();
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
            s8 r;
            r = menuSubOpenYesNo(0x7f, -1, -1, fightSeqGetEffectAminFlag() == 0);
            if (r == 0) {
                fightSeqSetEffectAminFlag(1);
            } else if (r == 1) {
                fightSeqSetEffectAminFlag(0);
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
                s32 maxVal;
                s32 sid;
                u32 tmp;
                maxVal = 0x162;
                sid = lbl_8047A282;
                for (;;) {
                    if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs((u16)sid, &tmp, _dbgMenuFightGetWazaDataIdSub) == 0) {
                        sid = -1;
                    } else {
                        if (tmp >= 0x163) {
                            tmp = (u32)maxVal;
                        }
                        menuSubCloseNumberInput();
                        sid = tmp;
                    }
                    if (sid < 0) break;
                    if (fightSeqCheckYubiwohuruWazaDataId((u16)sid) == 1) {
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

    menuCloseCustom(0xe, 0, 1);
    return 1;
}
#pragma peephole on
#endif

/* dbgMenuFightStop - 0x80007FDC | size: 0x38 */
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightStop(void) {
    if (fn_801EF63C() == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* dbgMenuFightFightFloorEdit - 0x80008014 | size: 0x38 */
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightFightFloorEdit(void) {
    if (fn_801EF63C() == 0) return -1;
    return 1;
}
#pragma pop
#endif

/* fn_8000804C - 0x8000804C | size: 0xf8 */
extern void* fn_800FF560(void);
extern void fn_8020DAD0(void);
extern void* GSthreadCreate(s32 a, void* b, s32 c, s32 d, s32 e, void* f);
extern void GSthreadSetArgs(void* task, s32 a, ...);
extern u16 lbl_8047882C;
extern u32 lbl_80478F50;
#if 1
#pragma peephole off
s32 fn_8000804C(void) {
    u32 result;
    s32 val;

    for (;;) {
        if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(lbl_8047882C, &result, fn_800087FC) == 0) {
            val = -1;
        } else {
            if (result >= *(u32*)(void*)lbl_80478F50) {
                result = *(u32*)(void*)lbl_80478F50 - 1;
            }
            menuSubCloseNumberInput();
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
                GSthreadSetArgs(taskPtr, 1, saved);
            }
            return 0;
        }
    }
    return -1;
}
#pragma peephole on
#endif

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

/* dbgMenuFightGetMsgSpeedToFrame - 0x80008184 | size: 0xc0 */
extern u8 winMsgCloseCheckFight(void);
extern u8 lbl_80266678[];
#if 1
#pragma peephole off
u32 dbgMenuFightGetMsgSpeedToFrame(u32 value) {
    u32 buttons;
    u32 held;
    u8 idx;
    u8 buf[10];

    idx = lbl_80478828;
    *(u32*)&buf[0] = *(u32*)&lbl_80266678[0];
    *(u32*)&buf[4] = *(u32*)&lbl_80266678[4];
    *(u16*)&buf[8] = *(u16*)&lbl_80266678[8];

    if (winMsgCloseCheckFight() == 1) {
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
extern void msgctrlSetValue(s32 slot, void* ptr);
extern u32 lbl_80478F28;
#if 1
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
        r31 = fightTrainerGetStatus(0, (u16)id, 0x40, 0);
        r30 = fightTrainerGetStatus(0, (u16)id, 0x41, 0);
        r29 = fightTrainerGetStatus(0, (u16)id, 0x3f, 0);
        r28 = fightTrainerGetStatus(0, (u16)id, 0x3e, 0);
    }
    if (r29 == 0) r29 = (void*)0xEB63;
    if (r31 == 0) r31 = (void*)0xEB63;
    if (r30 == 0) r30 = (void*)0xEB63;
    msgctrlSetValue(0xd,  GSmsgGetGSchar((u32)r31));
    msgctrlSetValue(0xe,  GSmsgGetGSchar((u32)r30));
    msgctrlSetValue(0x4d, GSmsgGetGSchar((u32)r29));
    msgctrlSetValue(0x2f, r28);
    return GSmsgGetGSchar(0xF159);
}
#pragma peephole on
#endif

/* _dbgMenuFightGetFightTrainerPokemonPartDataIdSub - 0x80008390 | size: 0x6c */
extern u32 lbl_80478F08;
#pragma push
#pragma peephole off
void* _dbgMenuFightGetFightTrainerPokemonPartDataIdSub(u32 id) {
    if (id == 0) return GSmsgGetGSchar(0xEB63);
    if (id >= *(u32*)(void*)lbl_80478F08) return GSmsgGetGSchar(0xEB63);
    return GSmsgGetGSchar((u32)fightTrainerGetStatus(0, (u16)id, 0xb, 0));
}
#pragma pop

/* _dbgMenuFightGetWazaDataIdSub - 0x800083FC | size: 0x64 */
extern void* wazaGetStatus(s32 a, u16 b, s32 c, s32 d);
#if 1
#pragma push
#pragma peephole off
void* _dbgMenuFightGetWazaDataIdSub(u32 id) {
    if (id == 0) return GSmsgGetGSchar(0xEB63);
    if (id >= 0x163) return GSmsgGetGSchar(0xEB63);
    return GSmsgGetGSchar((u32)wazaGetStatus(0, (u16)id, 1, 0));
}
#pragma pop
#endif

/* dbgMenuFightGetWazaTypeId - 0x80008460 | size: 0x60 */
extern void* _dbgMenuFightGetWazaTypeIdSub(u32 id);   /* forward decl for callback */
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightGetWazaTypeId(s32 id) {
    u32 result;
    if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs((u8)id, &result, _dbgMenuFightGetWazaTypeIdSub) == 0) return -1;
    if (result > 0xb) result = 0xb;
    menuSubCloseNumberInput();
    return (s32)result;
}
#pragma pop
#endif

/* _dbgMenuFightGetWazaTypeIdSub - 0x800084C0 | size: 0x58 | SYMBOL-NAME WALL 95.45%: bl wazaGetWazaTypeIdName vs bl wazaGetWazaTypeIdName (same addr) */
extern void* wazaGetWazaTypeIdName(u8 idx);
#if 1
#pragma push
#pragma peephole off
void* _dbgMenuFightGetWazaTypeIdSub(u32 id) {
    if (id == 0) return GSmsgGetGSchar(0xEB63);
    if (id > 0xb) return GSmsgGetGSchar(0xEB63);
    return GSmsgGetGSchar((u32)wazaGetWazaTypeIdName((u8)id));
}
#pragma pop
#endif

/* dbgMenuFightGetZokuseiDataId - 0x80008518 | size: 0x64 */
#if 1
#pragma push
#pragma peephole off
s32 dbgMenuFightGetZokuseiDataId(u32 id) {
    u32 result;
    if (menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs((u16)id, &result, _dbgMenuFightGetZokuseiDataIdSub) == 0) return -1;
    if (result >= lbl_80478B38) result = lbl_80478B38 - 1;
    menuSubCloseNumberInput();
    return (s32)result;
}
#pragma pop
#endif

/* =======================================================================
 * _dbgMenuFightGetZokuseiDataIdSub -- GStask_LookupResourceById
 *
 * Looks up a resource by ID. If ID is 9, uses a fixed constant.
 * If ID >= the global count (lbl_80478B38), also uses the constant.
 * Otherwise, converts via fn_8010C4D4 and passes to GSmsgGetGSchar.
 *
 * Address: 0x8000857C  Size: 0x5C (92 bytes)
 * ======================================================================= */
#pragma push
#pragma peephole off
void* _dbgMenuFightGetZokuseiDataIdSub(u32 id)
{
    if (id == 9) {
        return GSmsgGetGSchar(0x0000EB63);
    }
    if (id >= lbl_80478B38) {
        return GSmsgGetGSchar(0x0000EB63);
    }
    {
        u32 idx;
        idx = (u16)id;
        return GSmsgGetGSchar((u32)fn_8010C4D4(idx));
    }
}
#pragma pop

/* fn_800085D8 - 0x800085D8 | size: 0xb4 */
#if 1
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
    return GSmsgGetGSchar(id);
}
#pragma pop
#endif

/* fn_8000868C - 0x8000868C | size: 0x60 */
#if 1
void* fn_8000868C(u32 id) {
    u32 result;
    if (id >= *(u32*)lbl_80478F20) {
        result = 0xEB63;
    } else {
        result = (u32)fightTrainerGetStatus(0, (u16)id, 3, 0);
    }
    if (result == 0) result = 0xEB63;
    return GSmsgGetGSchar(result);
}
#endif

/* fn_800086EC - 0x800086EC | size: 0x58 */
extern void* fightTypeDataBiosGetPtr(u16 id);
extern void* fightTypeDataBiosGetName(void* ptr);
#if 1
void* fn_800086EC(u32 id) {
    u32 result;
    if (id >= *(u32*)lbl_80478F00) {
        result = 0xEB63;
    } else {
        result = (u32)fightTypeDataBiosGetName(fightTypeDataBiosGetPtr((u16)id));
    }
    if (result == 0) result = 0xEB63;
    return GSmsgGetGSchar(result);
}
#endif

/* _dbgMenuFightGetFightKindDataIdSub - 0x80008744 | size: 0x58 */
extern void* fightKindDataBiosGetPtr(u16 id);
extern void* fightKindDataBiosGetName(void* ptr);
#if 1
void* _dbgMenuFightGetFightKindDataIdSub(u32 id) {
    u32 result;
    if (id >= *(u32*)lbl_80478F40) {
        result = 0xEB63;
    } else {
        result = (u32)fightKindDataBiosGetName(fightKindDataBiosGetPtr((u16)id));
    }
    if (result == 0) result = 0xEB63;
    return GSmsgGetGSchar(result);
}
#endif

/* _dbgMenuFightGetFightFloorDataIdSub - 0x8000879C | size: 0x60 */
#if 1
void* _dbgMenuFightGetFightFloorDataIdSub(u32 id) {
    u32 result;
    if (id >= *(u32*)lbl_80478F48) {
        result = 0xEB63;
    } else {
        result = (u32)fightFloorGetStatus(0, (u16)id, 1, 0);
    }
    if (result == 0) result = 0xEB63;
    return GSmsgGetGSchar(result);
}
#endif

/* =======================================================================
 * fn_800087FC -- GStask_LookupEventById
 *
 * Looks up an event resource by ID. Returns NULL-equivalent constant
 * on invalid input or out-of-range. Otherwise, chains through
 * fightEncountDataBiosGetPtr -> fightEncountDataBiosGetFightName -> GSmsgGetGSchar.
 *
 * Address: 0x800087FC  Size: 0x6C (108 bytes)
 * ======================================================================= */
#pragma push
#pragma scheduling off
void* fn_800087FC(u32 id)
{
    extern void* fightEncountDataBiosGetPtr(u16 id);
    extern u32 lbl_80478F50;
    u32 result;

    if (id == 0) {
        result = 0x0000EB63;
    } else if (id >= *(u32*)lbl_80478F50) {
        result = 0x0000EB63;
    } else {
        result = (u32)fightEncountDataBiosGetFightName(fightEncountDataBiosGetPtr((u16)id));
    }
    if (result == 0) {
        result = 0x0000EB63;
    }

    return GSmsgGetGSchar(result);
}
#pragma pop
#endif /* GS_TASK_RANGE_EXACT_8000765C */
