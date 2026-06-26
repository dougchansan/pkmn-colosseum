/**
 * @file colosseum_event.c
 * @brief Event handlers, scene setup, and colosseum initialization.
 *
 * =========================================================================
 * SUBSYSTEM ANALYSIS
 * =========================================================================
 *
 * Address range: 0x80201800 - 0x80212000
 * Total functions: ~100
 * Total code size: ~40KB
 *
 * This module handles the event/scene management layer that sits between
 * the low-level data accessors (pokemon.c, trainer.c) and the high-level
 * story scripting (colosseum_script.c). It manages:
 *
 *   - Scene transitions into/out of Colosseum areas
 *   - Event flag management for story progression
 *   - Pre-battle dialogue and cutscene triggers
 *   - Colosseum registration and entry validation
 *
 * KEY FUNCTIONS:
 *
 *   fn_802026E4 (CheckEventFlag) - 291 calls, checks boolean flags
 *     Takes (context, flagId) and returns TRUE/FALSE. This is the
 *     primary predicate used throughout the script system to gate
 *     story progression.
 *
 *   fn_8020248C (SetEventState) - 67 calls, sets event state
 *   fn_802025B8 (GetEventState) - 59 calls, gets event state
 *     These manage multi-valued event states (not just booleans).
 *     Used for tracking things like "which round of the Colosseum"
 *     or "which rival encounter".
 *
 *   fn_80211B94 (TriggerEvent) - 121 calls
 *     Invokes an event handler on a scene/trainer context. The event
 *     data pointer (r4) comes from rodata tables like lbl_8027A00C
 *     which contain function pointer pairs (init, update).
 *
 *   fn_80211E18 (EventCallback) - post-battle result processing
 *     Called after story battles to handle rewards, flag updates, etc.
 *
 *   fn_80212D6C - Colosseum round transition handler
 *     Called to advance from one Colosseum round to the next.
 *
 * SCENE-LEVEL FUNCTIONS (0x80201800-0x80206000):
 *
 *   fn_80203620 (0x5C bytes): Navigate trainer context to a specific
 *     data table. Calls fn_8012640C twice: first with field 0xCC,
 *     then with field 0x79. This resolves trainer -> Pokemon -> extended data.
 *
 *   fn_8020367C (0x58 bytes): Similar navigation but takes an extra
 *     parameter and calls fn_80125424 to write data.
 *
 *   fn_802036D4 (0x84 bytes): Three-hop navigation: field 0xD6, then
 *     conditional check, then another hop. Used for complex trainer data.
 *
 *   These utility functions are the "glue" between the trainer system
 *   and the event/scripting system, translating between different data
 *   table reference formats.
 *
 * EVENT FLAG SYSTEM (0x80202000-0x80203000):
 *   The event flag system maps flag IDs to bits in a large array stored
 *   in the save data. CheckEventFlag (fn_802026E4) resolves the flag ID
 *   to an array index + bit position, then reads the corresponding bit.
 *
 *   Flag ID ranges appear to be:
 *     0x00-0x0F : System flags (game completion, save state)
 *     0x10-0x3D : Story progression flags
 *     0x3E+     : Colosseum/optional content flags
 *
 * COLOSSEUM ENTRY (0x80206000-0x80212000):
 *   This section handles the mechanics of entering a Colosseum:
 *   - Checking if the player's team is valid
 *   - Registering for a tournament
 *   - Setting up opponent brackets
 *   - Initializing the battle sequence
 *
 *   fn_8020A000-0x8020B000 region: Dense cluster of 76 functions that
 *   appear to be Colosseum-specific event handlers, one per game event
 *   or dialogue trigger.
 *
 * RODATA TABLES:
 *   lbl_80279C28-lbl_80279D08: Constant pairs (possibly text/message IDs)
 *   lbl_8027A00C: Function pointer table for event dispatching
 *   lbl_80279E7C: Constant data (0x00007693) - possibly model/animation ID
 *   lbl_80279F7C: Byte table {0x01,0xC8,0x04,0x96,...} - timing/difficulty curve
 *   lbl_80279FA0: Constant pairs - possibly UI element IDs
 *
 * =========================================================================
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

extern void* fn_8012640C();
extern u32   fn_801254B4();
extern void  fn_80125424();
extern u32   itemGetStatus();
extern void  fn_80119ED0(void);
extern void  fn_80121ADC(void);
extern void  fn_8011B67C(void);
extern void  fn_801230E0(void);
extern void* fn_801F0928(void* p);
extern void  fn_8011BEB4(void);

/* SDA table pointers for event data arrays */
extern u32 lbl_80478D38;   /* Event table count */
extern u8  lbl_80478D30[];   /* Event table base (6 bytes per entry) */

/* Forward declarations for converted functions */

/* =========================================================================
 * fn_80203620 - ResolveTrainerExtendedData
 *
 * Navigate from a trainer context through two data table hops to reach
 * extended Pokemon/trainer data.
 *
 * Hop 1: fn_8012640C(ctx, 0, 0xCC, 0) -> intermediate pointer
 * Hop 2: fn_8012640C(intermediate, 0, 0x79, 0) -> extended data
 *
 * If either hop returns NULL, the function returns NULL.
 *
 * @param context  Trainer/party context
 * @return         Extended data pointer, or NULL
 * ========================================================================= */
void* ResolveTrainerExtendedData(void* context) {
    void* intermediate;
    if (context == NULL) {
        return NULL;
    }

    intermediate = fn_8012640C(context, 0, 0xCC, 0);
    if (intermediate == NULL) {
        return NULL;
    }

    return fn_8012640C(intermediate, 0, 0x79, 0);
}

/* =========================================================================
 * fn_8020367C - WriteTrainerExtendedData
 *
 * Similar two-hop navigation, but the second call writes data via
 * fn_80125424 instead of reading it.
 *
 * @param context  Trainer/party context
 * @param value    Value to write
 * ========================================================================= */
void WriteTrainerExtendedData(void* context, u32 value) {
    void* intermediate;
    if (context == NULL) {
        return;
    }

    intermediate = fn_8012640C(context, 0, 0xCC, 0);
    if (intermediate == NULL) {
        return;
    }

    fn_80125424(intermediate, value);
}

/* =========================================================================
 * fn_802036D4 - ResolveTrainerExtendedDataThreeHop
 *
 * Three-hop data navigation for complex trainer structures.
 * Hop 1: field 0xD6 (party list)
 * Conditional check on result
 * Hop 2: if valid, field 0xD2 (secondary reference)
 *
 * @param context  Trainer context
 * @return         Resolved data pointer, or NULL
 * ========================================================================= */
/* TODO: Decompile fn_802036D4 (0x84 bytes) */

/* =========================================================================
 * fn_802026E4 - CheckEventFlag
 *
 * Checks a boolean event flag. This is the gating predicate used by
 * the story script system -- virtually every branching decision in the
 * script interpreter calls this to check whether a condition is met.
 *
 * 291 call sites.
 *
 * @param context  Context pointer (trainer, scene, or NULL for global)
 * @param flagId   Flag identifier to check
 * @return         TRUE (1) if flag is set, FALSE (0) otherwise
 * ========================================================================= */
/* TODO: Decompile fn_802026E4 */

/* =========================================================================
 * fn_802025B8 - GetEventState
 *
 * Gets a multi-valued event state (not just boolean).
 * 59 call sites. Returns a u8 value.
 * ========================================================================= */
/* TODO: Decompile fn_802025B8 */

/* =========================================================================
 * fn_8020248C - SetEventState
 *
 * Sets a multi-valued event state.
 * 67 call sites.
 * ========================================================================= */
/* TODO: Decompile fn_8020248C */

/* =========================================================================
 * fn_80211B94 - TriggerEvent
 *
 * Triggers an event on a scene/trainer context. Takes an event data
 * pointer that comes from rodata function pointer tables.
 *
 * The event data pointer (e.g., lbl_8027A00C) contains pairs of
 * function pointers: the first is an init function, the second is
 * an update/tick function. TriggerEvent calls the init function
 * and registers the update function for per-frame callbacks.
 *
 * 121 call sites.
 * ========================================================================= */
/* TODO: Decompile fn_80211B94 */

/* =========================================================================
 * fn_80211E18 - EventCallback
 *
 * Post-event callback processor. Called after battles and other events
 * complete to process results (give rewards, set flags, advance story).
 * ========================================================================= */
/* TODO: Decompile fn_80211E18 */

/* =========================================================================
 * Dense event handler block (0x8020A000 - 0x8020B000)
 *
 * 76 functions in 4KB. These are individual event handlers, likely
 * one per game event type. Each follows a similar pattern:
 *   1. Call PokemonGet/TrainerDataGet to check conditions
 *   2. Call SetEventState or TrainerDataSet to update state
 *   3. Return a status code
 *
 * The functions are called via function pointer tables set up by
 * the colosseum initialization code.
 * ========================================================================= */

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 140 functions matched
 * =================================================================== */

/* Address: 0x8020A068 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020A068(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* Address: 0x8020A0A4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A0A4(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xA9]) = val;
}

/* Address: 0x8020A0B4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A0B4(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xA8]) = val;
}

/* Address: 0x8020A0C4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A0C4(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0xA6]) = val;
}

/* Address: 0x8020A0D4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A0D4(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0xA4]) = val;
}

/* Address: 0x8020A0E4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A0E4(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0xA0]) = val;
}

/* Address: 0x8020A0F4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A0F4(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x9C]) = val;
}

/* Address: 0x8020A104 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A104(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x99]) = val;
}

/* Address: 0x8020A114 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A114(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x98]) = val;
}

/* Address: 0x8020A124 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A124(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x6]) = val;
}

/* Address: 0x8020A134 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A134(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x4]) = val;
}

/* Address: 0x8020A144 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A144(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x2]) = val;
}

/* Address: 0x8020A154 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A154(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x0]) = val;
}

/* Address: 0x8020A164 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020A164(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xA9]);
}

/* Address: 0x8020A17C | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020A17C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xA8]);
}

/* Address: 0x8020A194 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020A194(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0xA6]);
}

/* Address: 0x8020A1AC | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020A1AC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0xA4]);
}

/* Address: 0x8020A1C4 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020A1C4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0xA0]);
}

/* Address: 0x8020A1DC | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020A1DC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x9C]);
}

/* Address: 0x8020A1F4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020A1F4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x99]);
}

/* Address: 0x8020A20C | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020A20C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x98]);
}

/* Address: 0x8020A258 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020A258(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x6]);
}

/* Address: 0x8020A270 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020A270(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x4]);
}

/* Address: 0x8020A288 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020A288(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x2]);
}

/* Address: 0x8020A2A0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020A2A0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* Address: 0x8020A2F8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A2F8(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x8]) = val;
}

/* Address: 0x8020A308 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A308(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x4]) = val;
}

/* Address: 0x8020A318 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A318(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x2]) = val;
}

/* Address: 0x8020A328 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020A328(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x0]) = val;
}

/* Address: 0x8020A338 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020A338(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x8]);
}

/* Address: 0x8020A350 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020A350(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x8020A368 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020A368(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x2]);
}

/* Address: 0x8020A380 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020A380(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x0]);
}

/* Address: 0x8020AE30 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE30(void) { return 1; }

/* Address: 0x8020AE38 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE38(void) { return 1; }

/* Address: 0x8020AE40 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE40(void) { return 1; }

/* Address: 0x8020AE48 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE48(void) { return 1; }

/* Address: 0x8020AE50 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE50(void) { return 1; }

/* Address: 0x8020AE58 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE58(void) { return 1; }

/* Address: 0x8020AE60 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE60(void) { return 1; }

/* Address: 0x8020AE68 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE68(void) { return 1; }

/* Address: 0x8020AE70 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE70(void) { return 1; }

/* Address: 0x8020AE78 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE78(void) { return 1; }

/* Address: 0x8020AE80 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE80(void) { return 1; }

/* Address: 0x8020AE88 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE88(void) { return 1; }

/* Address: 0x8020AE90 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE90(void) { return 1; }

/* Address: 0x8020AE98 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AE98(void) { return 1; }

/* Address: 0x8020AEA0 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AEA0(void) { return 1; }

/* Address: 0x8020AEA8 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AEA8(void) { return 1; }

/* Address: 0x8020AEB0 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AEB0(void) { return 1; }

/* Address: 0x8020AEB8 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AEB8(void) { return 1; }

/* Address: 0x8020AEC0 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AEC0(void) { return 1; }

/* Address: 0x8020AEC8 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020AEC8(void) { return 1; }

/* Address: 0x8020B050 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020B050(void) { return 1; }

/* Address: 0x8020D784 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020D784(void) { return 1; }

/* Address: 0x8020D78C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020D78C(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x1C]) = val;
}

/* Address: 0x8020D79C | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020D79C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x8]);
}

/* Address: 0x8020D7B4 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020D7B4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x8020D814 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020D814(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x8020D82C | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020D82C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x0]);
}

/* Address: 0x8020D868 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020D868(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x18]) = val;
}

/* Address: 0x8020D878 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020D878(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x10]) = val;
}

/* Address: 0x8020D888 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020D888(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0xC]) = val;
}

/* Address: 0x8020D898 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020D898(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x14]) = val;
}

/* Address: 0x8020D8A8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020D8A8(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x8]) = val;
}

/* Address: 0x8020D8B8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020D8B8(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x4]) = val;
}

/* Address: 0x8020D8C8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020D8C8(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x0]) = val;
}

/* Address: 0x8020D8D8 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020D8D8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x10]);
}

/* Address: 0x8020D8F0 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020D8F0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0xC]);
}

/* Address: 0x8020D908 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020D908(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x14]);
}

/* Address: 0x8020D920 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020D920(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x8]);
}

/* Address: 0x8020D938 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020D938(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x8020D950 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020D950(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x0]);
}

/* Address: 0x8020D9A0 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020D9A0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x4]);
}

/* Address: 0x8020D9B8 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020D9B8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x2]);
}

/* Address: 0x8020D9D0 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020D9D0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x0]);
}

/* Address: 0x8020DE50 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020DE50(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x2]);
}

/* Address: 0x8020DE80 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020DE80(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* Address: 0x8020DE98 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020DE98(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x8]);
}

/* Address: 0x8020DED8 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020DED8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x8]);
}

/* Address: 0x8020DEF0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020DEF0(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x14]) = val;
}

/* Address: 0x8020DF00 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020DF00(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0xC]) = val;
}

/* Address: 0x8020DF90 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020DF90(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x4]) = val;
}

/* Address: 0x8020DFA0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020DFA0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x1]) = val;
}

/* Address: 0x8020DFB0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_8020DFB0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x0]) = val;
}

/* Address: 0x8020DFC0 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020DFC0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x14]);
}

/* Address: 0x8020DFD8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020DFD8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x2]);
}

/* Address: 0x8020DFF0 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020DFF0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x10]);
}

/* Address: 0x8020E008 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020E008(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0xC]);
}

/* Address: 0x8020E0B0 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_8020E0B0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x4]);
}

/* Address: 0x8020E0C8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E0C8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x1]);
}

/* Address: 0x8020E0E0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E0E0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* Address: 0x8020E1A4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E1A4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x2]);
}

/* Address: 0x8020E1BC | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E1BC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x1]);
}

/* Address: 0x8020E1D4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E1D4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* Address: 0x8020E1EC | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020E1EC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x8020E230 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E230(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x18]);
}

/* Address: 0x8020E248 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_8020E248(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x1C]);
}

/* Address: 0x8020E260 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E260(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x17]);
}

/* Address: 0x8020E278 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E278(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x16]);
}

/* Address: 0x8020E290 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E290(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x15]);
}

/* Address: 0x8020E2A8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E2A8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x14]);
}

/* Address: 0x8020E2C0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E2C0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x13]);
}

/* Address: 0x8020E2D8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E2D8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x12]);
}

/* Address: 0x8020E2F0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E2F0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x11]);
}

/* Address: 0x8020E308 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E308(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x10]);
}

/* Address: 0x8020E320 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E320(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xF]);
}

/* Address: 0x8020E338 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E338(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xE]);
}

/* Address: 0x8020E350 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E350(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xD]);
}

/* Address: 0x8020E368 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E368(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xC]);
}

/* Address: 0x8020E380 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E380(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xB]);
}

/* Address: 0x8020E398 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E398(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x9]);
}

/* Address: 0x8020E3B0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E3B0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xA]);
}

/* Address: 0x8020E3C8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E3C8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x8]);
}

/* Address: 0x8020E3E0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E3E0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x7]);
}

/* Address: 0x8020E3F8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E3F8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x6]);
}

/* Address: 0x8020E410 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E410(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x5]);
}

/* Address: 0x8020E428 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E428(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x4]);
}

/* Address: 0x8020E440 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E440(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x3]);
}

/* Address: 0x8020E458 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E458(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x2]);
}

/* Address: 0x8020E470 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E470(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x1]);
}

/* Address: 0x8020E4B4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_8020E4B4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* Address: 0x8020F100 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020F100(void) { return 0; }

/* Address: 0x8020F230 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020F230(void) { return 0; }

/* Address: 0x8020F360 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020F360(void) { return 0; }

/* Address: 0x8020F3E8 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020F3E8(void) { return 0; }

/* Address: 0x8020F7B0 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020F7B0(void) { return 0; }

/* Address: 0x8020F8CC | Size: 0x8 | Pattern: return_constant */
u32 fn_8020F8CC(void) { return 0; }

/* Address: 0x8020FB30 | Size: 0x8 | Pattern: return_constant */
u32 fn_8020FB30(void) { return 0; }

/* Address: 0x8020FBFC | Size: 0x8 | Pattern: return_constant */
u32 fn_8020FBFC(void) { return 0; }

/* Address: 0x80210880 | Size: 0x8 | Pattern: return_constant */
u32 fn_80210880(void) { return 0; }

/* Address: 0x80210990 | Size: 0x8 | Pattern: return_constant */
u32 fn_80210990(void) { return 0; }

/* Address: 0x80210B00 | Size: 0x8 | Pattern: return_constant */
u32 fn_80210B00(void) { return 0; }

/* Address: 0x80210BF0 | Size: 0x8 | Pattern: return_constant */
u32 fn_80210BF0(void) { return 0; }

/* Address: 0x80210CFC | Size: 0x8 | Pattern: return_constant */
u32 fn_80210CFC(void) { return 0; }

/* Address: 0x80210E54 | Size: 0x8 | Pattern: return_constant */
u32 fn_80210E54(void) { return 0; }

/* Address: 0x80210EC0 | Size: 0x8 | Pattern: return_constant */
u32 fn_80210EC0(void) { return 0; }

/* Address: 0x80211038 | Size: 0x8 | Pattern: return_constant */
u32 fn_80211038(void) { return 0; }

/* Address: 0x8021115C | Size: 0x8 | Pattern: return_constant */
u32 fn_8021115C(void) { return 0; }

/* Address: 0x80211168 | Size: 0x8 | Pattern: return_constant */
u32 fn_80211168(void) { return 0; }

/* #######################################################################
 * COVERAGE STUBS: Colosseum event system (0x80201800 - 0x80212000)
 * 256 functions remaining for full coverage of colosseum_event.c TU.
 *
 * Key functions in this range:
 *   fn_802026E4 (CheckEventFlag)  - 291 calls, primary story gate
 *   fn_8020248C (SetEventState)   - 67 calls, multi-valued state setter
 *   fn_802025B8 (GetEventState)   - 59 calls, multi-valued state getter
 *   fn_80211B94 (TriggerEvent)    - 121 calls, event dispatcher
 *   fn_80211E18 (EventCallback)   - post-battle result processing
 *   fn_80212D6C (ColosseumRoundTransition) - round advancement
 *   fn_8020C840 (BattleSystemInit) - battle subsystem initialization
 *   fn_80205B8C (GetTrainerPokemonPtr) - 668 calls, trainer->pokemon nav
 * ####################################################################### */

/* 0x80201890 | size: 0x12C */
void fn_80201890(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern void fn_8011A3E4();
    extern void fn_80121574();
    u16 typeId;
    void* resolved;
    typeId = fn_80119ED0(typeObj);
    if (typeId == 0x7C || typeId == 0xC8 || typeId == 0xCD) {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        typeId = fn_80119ED0(typeObj);
        if (typeId == 0x7C || typeId == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = fn_8012640C(resolved, 0, 0xCC, 0);
            }
            fn_80121574(resolved, typeObj);
        } else if (typeId == 0xCD) {
            fn_8011A3E4(resolved, typeObj);
        }
    } else if (typeId == 0xD8) {
        fn_8011A3E4(ctx, typeObj);
    }
}

/* 0x802019BC | size: 0x170 */
void fn_802019BC(void* ctx, void* src, void* typeObj) {
    extern u16 fn_80119ED0();
    extern void fn_8011A0A8();
    extern void fn_80121484();
    u16 typeId;
    void* srcResolved;
    void* ctxResolved;
    typeId = fn_80119ED0(typeObj);
    if (typeId == 0x7C || typeId == 0xC8 || typeId == 0xCD) {
        srcResolved = fn_8012640C(src, 0, 0xD6, 0);
        ctxResolved = fn_8012640C(ctx, 0, 0xD6, 0);
        typeId = fn_80119ED0(typeObj);
        if (typeId == 0x7C || typeId == 0xC8) {
            if (ctxResolved == NULL) {
                ctxResolved = NULL;
            } else {
                ctxResolved = fn_8012640C(ctxResolved, 0, 0xCC, 0);
            }
            if (srcResolved == NULL) {
                srcResolved = NULL;
            } else {
                srcResolved = fn_8012640C(srcResolved, 0, 0xCC, 0);
            }
            fn_80121484(ctxResolved, srcResolved, typeObj);
        } else if (typeId == 0xCD) {
            fn_8011A0A8(ctxResolved, srcResolved, typeObj);
        }
    } else if (typeId == 0xD8) {
        fn_8011A0A8(ctx, src, typeObj);
    }
}

/* 0x80201B2C | size: 0x12C */
void fn_80201B2C(void* ctx, void* typeObj, u32 param) {
    extern u16 fn_80119ED0();
    extern void fn_8011A570();
    extern void fn_801215E4();
    u16 typeId;
    void* resolved;
    typeId = fn_80119ED0(typeObj);
    if (typeId == 0x7C || typeId == 0xC8 || typeId == 0xCD) {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        typeId = fn_80119ED0(typeObj);
        if (typeId == 0x7C || typeId == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = fn_8012640C(resolved, 0, 0xCC, 0);
            }
            fn_801215E4(resolved, typeObj, param);
        } else if (typeId == 0xCD) {
            fn_8011A570(resolved, typeObj, param);
        }
    } else if (typeId == 0xD8) {
        fn_8011A570(ctx, typeObj, param);
    }
}

/* 0x80201C58 | size: 0x12C */
void fn_80201C58(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern void fn_8011A6D4();
    extern void fn_8012165C();
    u16 typeId;
    void* resolved;
    typeId = fn_80119ED0(typeObj);
    if (typeId == 0x7C || typeId == 0xC8 || typeId == 0xCD) {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        typeId = fn_80119ED0(typeObj);
        if (typeId == 0x7C || typeId == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = fn_8012640C(resolved, 0, 0xCC, 0);
            }
            fn_8012165C(resolved, typeObj);
        } else if (typeId == 0xCD) {
            fn_8011A6D4(resolved, typeObj);
        }
    } else if (typeId == 0xD8) {
        fn_8011A6D4(ctx, typeObj);
    }
}

/* 0x80201D84 | size: 0x12C */
void fn_80201D84(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern void fn_8011A860();
    extern void fn_801216CC();
    u16 typeId;
    void* resolved;
    typeId = fn_80119ED0(typeObj);
    if (typeId == 0x7C || typeId == 0xC8 || typeId == 0xCD) {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        typeId = fn_80119ED0(typeObj);
        if (typeId == 0x7C || typeId == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = fn_8012640C(resolved, 0, 0xCC, 0);
            }
            fn_801216CC(resolved, typeObj);
        } else if (typeId == 0xCD) {
            fn_8011A860(resolved, typeObj);
        }
    } else if (typeId == 0xD8) {
        fn_8011A860(ctx, typeObj);
    }
}

/* 0x80201EB0 | size: 0x12C */
void fn_80201EB0(void* ctx, void* typeObj, u32 param) {
    extern u16 fn_80119ED0();
    extern void fn_8011A9EC();
    extern void fn_8012173C();
    u16 typeId;
    void* resolved;
    typeId = fn_80119ED0(typeObj);
    if (typeId == 0x7C || typeId == 0xC8 || typeId == 0xCD) {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        typeId = fn_80119ED0(typeObj);
        if (typeId == 0x7C || typeId == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = fn_8012640C(resolved, 0, 0xCC, 0);
            }
            fn_8012173C(resolved, typeObj, param);
        } else if (typeId == 0xCD) {
            fn_8011A9EC(resolved, typeObj, param);
        }
    } else if (typeId == 0xD8) {
        fn_8011A9EC(ctx, typeObj, param);
    }
}

/* 0x80201FDC | size: 0x12C */
void fn_80201FDC(void* ctx, void* typeObj, u32 param) {
    extern u16 fn_80119ED0();
    extern void fn_8011AB50();
    extern void fn_801217B4();
    u16 typeId;
    void* resolved;
    typeId = fn_80119ED0(typeObj);
    if (typeId == 0x7C || typeId == 0xC8 || typeId == 0xCD) {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        typeId = fn_80119ED0(typeObj);
        if (typeId == 0x7C || typeId == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = fn_8012640C(resolved, 0, 0xCC, 0);
            }
            fn_801217B4(resolved, typeObj, param);
        } else if (typeId == 0xCD) {
            fn_8011AB50(resolved, typeObj, param);
        }
    } else if (typeId == 0xD8) {
        fn_8011AB50(ctx, typeObj, param);
    }
}

/* 0x80202108 | size: 0x12C */
void fn_80202108(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern void fn_8011ACB4();
    extern void fn_8012182C();
    u16 typeId;
    void* resolved;
    typeId = fn_80119ED0(typeObj);
    if (typeId == 0x7C || typeId == 0xC8 || typeId == 0xCD) {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        typeId = fn_80119ED0(typeObj);
        if (typeId == 0x7C || typeId == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = fn_8012640C(resolved, 0, 0xCC, 0);
            }
            fn_8012182C(resolved, typeObj);
        } else if (typeId == 0xCD) {
            fn_8011ACB4(resolved, typeObj);
        }
    } else if (typeId == 0xD8) {
        fn_8011ACB4(ctx, typeObj);
    }
}

/* 0x80202234 | size: 0x12C */
void fn_80202234(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern void fn_8011AE40();
    extern void fn_8012189C();
    u16 typeId;
    void* resolved;
    typeId = fn_80119ED0(typeObj);
    if (typeId == 0x7C || typeId == 0xC8 || typeId == 0xCD) {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        typeId = fn_80119ED0(typeObj);
        if (typeId == 0x7C || typeId == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = fn_8012640C(resolved, 0, 0xCC, 0);
            }
            fn_8012189C(resolved, typeObj);
        } else if (typeId == 0xCD) {
            fn_8011AE40(resolved, typeObj);
        }
    } else if (typeId == 0xD8) {
        fn_8011AE40(ctx, typeObj);
    }
}

/* 0x80202360 | size: 0x12C */
void fn_80202360(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern void fn_8011B130();
    extern void fn_80121984();
    u16 typeId;
    void* resolved;
    typeId = fn_80119ED0(typeObj);
    if (typeId == 0x7C || typeId == 0xC8 || typeId == 0xCD) {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        typeId = fn_80119ED0(typeObj);
        if (typeId == 0x7C || typeId == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = fn_8012640C(resolved, 0, 0xCC, 0);
            }
            fn_80121984(resolved, typeObj);
        } else if (typeId == 0xCD) {
            fn_8011B130(resolved, typeObj);
        }
    } else if (typeId == 0xD8) {
        fn_8011B130(ctx, typeObj);
    }
}

/* 0x8020248C | size: 0x12C */
void fn_8020248C(void* ctx, void* typeObj, u32 param) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    extern void fn_801219F4();
    u16 typeId;
    void* resolved;
    typeId = fn_80119ED0(typeObj);
    if (typeId == 0x7C || typeId == 0xC8 || typeId == 0xCD) {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        typeId = fn_80119ED0(typeObj);
        if (typeId == 0x7C || typeId == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = fn_8012640C(resolved, 0, 0xCC, 0);
            }
            fn_801219F4(resolved, typeObj, param);
        } else if (typeId == 0xCD) {
            fn_8011B2C0(resolved, typeObj, param);
        }
    } else if (typeId == 0xD8) {
        fn_8011B2C0(ctx, typeObj, param);
    }
}

/* 0x802025B8 | size: 0x12C */
void fn_802025B8(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern void fn_8011B444();
    extern void fn_80121A6C();
    u16 typeId;
    void* resolved;
    typeId = fn_80119ED0(typeObj);
    if (typeId == 0x7C || typeId == 0xC8 || typeId == 0xCD) {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        typeId = fn_80119ED0(typeObj);
        if (typeId == 0x7C || typeId == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = fn_8012640C(resolved, 0, 0xCC, 0);
            }
            fn_80121A6C(resolved, typeObj);
        } else if (typeId == 0xCD) {
            fn_8011B444(resolved, typeObj);
        }
    } else if (typeId == 0xD8) {
        fn_8011B444(ctx, typeObj);
    }
}

/* 0x802026E4 | size: 0x12C */
void fn_802026E4(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern void fn_8011B67C();
    extern void fn_80121ADC();
    u16 typeId;
    void* resolved;
    typeId = fn_80119ED0(typeObj);
    if (typeId == 0x7C || typeId == 0xC8 || typeId == 0xCD) {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        typeId = fn_80119ED0(typeObj);
        if (typeId == 0x7C || typeId == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = fn_8012640C(resolved, 0, 0xCC, 0);
            }
            fn_80121ADC(resolved, typeObj);
        } else if (typeId == 0xCD) {
            fn_8011B67C(resolved, typeObj);
        }
    } else if (typeId == 0xD8) {
        fn_8011B67C(ctx, typeObj);
    }
}

/* 0x80202810 | size: 0x188 | medium */
/* 0x80202810 | size: 0x188 */
void fn_80202810(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern void fn_8011B788();
    extern void fn_80121B4C();
    extern void fn_801DA36C();
    void* eeData;

    eeData = fn_8012640C(ctx, 0, 0xEE, 0);
    if ((u16)(u32)typeObj == 0) {
        if (eeData != NULL) {
            fn_801DA36C(eeData, 1);
            fn_801DA36C(eeData, 2);
        }
    } else {
        if (eeData != NULL) {
            if ((u16)(u32)typeObj == 8) {
                fn_801DA36C(eeData, 1);
            }
            if ((u16)(u32)typeObj == 7) {
                fn_801DA36C(eeData, 2);
            }
        }
    }
    if (fn_80119ED0(typeObj) == 0x7C || fn_80119ED0(typeObj) == 0xC8 || fn_80119ED0(typeObj) == 0xCD) {
        eeData = fn_8012640C(ctx, 0, 0xD6, 0);
        if (fn_80119ED0(typeObj) == 0x7C || fn_80119ED0(typeObj) == 0xC8) {
            if (eeData == NULL) {
                eeData = NULL;
            } else {
                eeData = fn_8012640C(eeData, 0, 0xCC, 0);
            }
            fn_80121B4C(eeData, typeObj);
        } else if (fn_80119ED0(typeObj) == 0xCD) {
            fn_8011B788(eeData, typeObj);
        }
    } else if (fn_80119ED0(typeObj) == 0xD8) {
        fn_8011B788(ctx, typeObj);
    }
}

/* 0x80202998 | size: 0x94 */
void fn_80202998(void* ctx, u16 mode) {
    extern void fn_801DA36C();
    void* obj;
    u16 modeVal;
    obj = fn_8012640C(ctx, 0, 0xEE, 0);
    modeVal = mode;
    if (modeVal == 0) {
        if (obj != NULL) {
            fn_801DA36C(obj, 1);
            fn_801DA36C(obj, 2);
        }
    } else {
        if (obj != NULL) {
            if (modeVal == 8) {
                fn_801DA36C(obj, 1);
            }
            modeVal = mode;
            if (modeVal == 7) {
                fn_801DA36C(obj, 2);
            }
        }
    }
}

/* 0x80202A2C | size: 0xB0 */
void fn_80202A2C(void* ctx, void* typeObj, u32 param) {
    extern u16 fn_80119ED0();
    extern void fn_8011AFCC();
    extern void fn_8012190C();
    void* resolved;
    if (fn_80119ED0(typeObj) == 0x7C || fn_80119ED0(typeObj) == 0xC8) {
        if (ctx == NULL) {
            resolved = NULL;
        } else {
            resolved = fn_8012640C(ctx, 0, 0xCC, 0);
        }
        fn_8012190C(resolved, typeObj, param);
    } else if (fn_80119ED0(typeObj) == 0xCD) {
        fn_8011AFCC(ctx, typeObj, param);
    }
}

/* 0x80202ADC | size: 0xAC */
u32 fn_80202ADC(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern u32 fn_8011B67C();
    extern u32 fn_80121ADC();
    void* resolved;
    u32 result;
    if (fn_80119ED0(typeObj) == 0x7C || fn_80119ED0(typeObj) == 0xC8) {
        if (ctx == NULL) {
            resolved = NULL;
        } else {
            resolved = fn_8012640C(ctx, 0, 0xCC, 0);
        }
        result = fn_80121ADC(resolved, typeObj);
    } else if (fn_80119ED0(typeObj) != 0xCD) {
        result = 0;
    } else {
        result = fn_8011B67C(ctx, typeObj);
    }
    return result;
}

/* 0x80202B88 | size: 0x94 */
u32 fn_80202B88(void* obj1, void* obj2) {
    extern u32 fn_801F02AC();
    extern u16 fn_801F54A4();
    u16 tableId;
    u32 val1;
    u32 val2;
    tableId = 0xFFFF & fn_801F54A4(NULL, 0, 0x14, 0);
    if (obj1 == NULL) {
        return 0;
    }
    if (obj2 == NULL) {
        return 0;
    }
    val1 = fn_801F02AC(2, obj1, tableId);
    val2 = fn_801F02AC(2, obj2, tableId);
    return (u8)(val1 == val2);
}

/* Address: 0x80202C1C | Size: 0x57c | Ghidra import */


void fn_80202C1C(int r3,u32 r4)

{
    extern s8 fn_801233F4();
    extern s8 fn_80123FBC();
    extern u16 fn_801EF634();
    extern u32 fn_801F54A4();
    extern u32 fightSideGetStatus();
    extern s8 fn_801FA634();
    extern int fn_801FB1C0();
    extern int fn_8020E57C();
    extern s8 fn_8020E614();
    extern void fn_8020E640();
  u32 bVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u8 cVar9;
  int iVar5;
  u16 sVar8;
  int iVar6;
  int iVar7;
  u16 uVar10;
  u32 uVar11;
  u32 uVar12;
  
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801F54A4(0,0,0x16,0);
  uVar3 = fn_801F54A4(0,0,0x18,0);
  for (uVar12 = 0; (uVar12 & 0xffff) < (uVar2 & 0xffff); uVar12 = uVar12 + 1) {
    uVar4 = fightSideGetStatus(r4,0,7,uVar12);
    cVar9 = fn_801FA634();
    if (cVar9 != 0) {
      for (uVar11 = 0; (uVar11 & 0xffff) < (uVar3 & 0xffff); uVar11 = uVar11 + 1) {
        iVar5 = fn_801FB1C0(uVar4,0,0x46,uVar11);
        if (iVar5 == 0) {
          bVar1 = 0;
        }
        else {
          sVar8 = fn_801EF634();
          if (sVar8 == 1) {
            bVar1 = 0;
          }
          else {
            iVar6 = (int)fn_8012640C(iVar5,0,0xd6,0);
            if (iVar6 == 0) {
              bVar1 = 0;
            }
            else {
              sVar8 = fn_801EF634();
              if (sVar8 == 1) {
                bVar1 = 0;
              }
              else {
                iVar7 = (int)fn_8012640C(iVar6,0,0xcb,0);
                if (iVar7 == 0) {
                  bVar1 = 0;
                }
                else {
                  cVar9 = fn_80123FBC();
                  if (cVar9 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    if (iVar6 == 0) {
                      iVar7 = 0;
                    }
                    else {
                      iVar7 = (int)fn_8012640C(iVar6,0,0xcc,0);
                    }
                    if (iVar7 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      cVar9 = fn_80123FBC();
                      if (cVar9 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        iVar6 = (int)fn_8012640C(iVar6,0,0xce,0);
                        if (iVar6 < 0) {
                          bVar1 = 0;
                        }
                        else {
                          bVar1 = 1;
                        }
                      }
                    }
                  }
                }
              }
              if (bVar1) {
                bVar1 = 1;
              }
              else {
                bVar1 = 0;
              }
            }
          }
          if (bVar1) {
            iVar6 = (int)fn_8012640C(iVar5,0,0x120,0);
            if (iVar6 == 1) {
              bVar1 = 0;
            }
            else {
              iVar6 = (int)fn_8012640C(iVar5,0,0xd6,0);
              if (iVar6 == 0) {
                bVar1 = 0;
              }
              else {
                sVar8 = fn_801EF634();
                if (sVar8 == 1) {
                  bVar1 = 0;
                }
                else {
                  iVar7 = (int)fn_8012640C(iVar6,0,0xcb,0);
                  if (iVar7 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    cVar9 = fn_80123FBC();
                    if (cVar9 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      if (iVar6 == 0) {
                        iVar7 = 0;
                      }
                      else {
                        iVar7 = (int)fn_8012640C(iVar6,0,0xcc,0);
                      }
                      if (iVar7 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        cVar9 = fn_80123FBC();
                        if (cVar9 == 0) {
                          bVar1 = 0;
                        }
                        else {
                          iVar7 = (int)fn_8012640C(iVar6,0,0xce,0);
                          if (iVar7 < 0) {
                            bVar1 = 0;
                          }
                          else {
                            bVar1 = 1;
                          }
                        }
                      }
                    }
                  }
                }
                if (bVar1) {
                  iVar7 = (int)fn_8012640C(iVar6,0,0xd2,0);
                  if (iVar7 == 1) {
                    bVar1 = 0;
                  }
                  else {
                    if (iVar6 == 0) {
                      iVar6 = 0;
                    }
                    else {
                      iVar6 = (int)fn_8012640C(iVar6,0,0xcc,0);
                    }
                    if (iVar6 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      cVar9 = fn_801233F4();
                      if (cVar9 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        bVar1 = 1;
                      }
                    }
                  }
                }
                else {
                  bVar1 = 0;
                }
              }
              if (bVar1) {
                bVar1 = 1;
              }
              else {
                bVar1 = 0;
              }
            }
          }
          else {
            bVar1 = 0;
          }
        }
        if ((bVar1) && (r3 != 0)) {
          if (iVar5 == 0) {
            bVar1 = 0;
          }
          else {
            sVar8 = fn_801EF634();
            if (sVar8 == 1) {
              bVar1 = 0;
            }
            else {
              iVar6 = (int)fn_8012640C(iVar5,0,0xd6,0);
              if (iVar6 == 0) {
                bVar1 = 0;
              }
              else {
                sVar8 = fn_801EF634();
                if (sVar8 == 1) {
                  bVar1 = 0;
                }
                else {
                  iVar7 = (int)fn_8012640C(iVar6,0,0xcb,0);
                  if (iVar7 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    cVar9 = fn_80123FBC();
                    if (cVar9 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      if (iVar6 == 0) {
                        iVar7 = 0;
                      }
                      else {
                        iVar7 = (int)fn_8012640C(iVar6,0,0xcc,0);
                      }
                      if (iVar7 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        cVar9 = fn_80123FBC();
                        if (cVar9 == 0) {
                          bVar1 = 0;
                        }
                        else {
                          iVar6 = (int)fn_8012640C(iVar6,0,0xce,0);
                          if (iVar6 < 0) {
                            bVar1 = 0;
                          }
                          else {
                            bVar1 = 1;
                          }
                        }
                      }
                    }
                  }
                }
                if (bVar1) {
                  bVar1 = 1;
                }
                else {
                  bVar1 = 0;
                }
              }
            }
          }
          if (bVar1) {
            iVar6 = (int)fn_8012640C(r3,0,0x122,0);
            iVar7 = fn_8020E57C(iVar6,4,iVar5);
            if (iVar7 == 0) {
              for (uVar10 = 0; uVar10 < 4; uVar10 = uVar10 + 1) {
                iVar7 = iVar6 + (u32)uVar10 * 0xc;
                cVar9 = fn_8020E614(iVar7);
                if (cVar9 == 0) {
                  fn_8020E640(iVar7,iVar5);
                  break;
                }
              }
            }
          }
        }
      }
    }
  }
}
/* 0x80203198 | size: 0x14C | medium */
/* 0x80203198 | size: 0x14C */
void fn_80203198(void* ctx, u32 param) {
    extern u16 fn_801FD0EC();
    extern void* fn_8020E57C();
    extern u8 fn_8020E614();
    extern void fn_8020E758();
    void* tableData;
    void* entry;
    void* entryPtr;
    u16 species;
    u8 count;
    u8 i;

    if (ctx == NULL) { return; }
    tableData = fn_8012640C(ctx, 0, 0x122, 0);
    entry = fn_8020E57C(tableData, 4, param);
    if (entry == NULL) { return; }
    species = fn_801FD0EC(entry);
    fn_8020E758(entry);
    if (species == 0 || species == 0x165 || species == 0xFFFF) { return; }
    if ((s32)(u32)fn_8012640C(ctx, 0, 0xF7, 0) != 0) { return; }
    if (ctx != NULL) {
        entryPtr = fn_8012640C(ctx, 0, 0x122, 0);
        for (i = 0; i < 4; i++) {}
        count = 0;
        for (i = 0; i < 4; i++) {
            entry = (void*)((u32)entryPtr + i * 0xC);
            if ((u8)fn_8020E614(entry) == 4) { continue; }
            species = fn_801FD0EC(entry);
            if (species == 4 || species == 0x165) { continue; }
            count++;
        }
    } else {
        count = 0;
    }
    if (count == 4) {
        fn_801254B4(ctx, 0, 0xF7, 0, (u32)species);
    }
}

/* 0x802032E4 | size: 0x138 */
#pragma push
#pragma scheduling off
void fn_802032E4(void* ctx, u32 param) {
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
    extern u8 fn_80121ADC();
    extern void fn_80122370();
    extern u32 fn_80123090();
    void* ccData;
    void* ccCtx;
    u8 result;
    u32 value;

    if (ctx == 0) { ccData = 0; } else { ccData = fn_8012640C(ctx, 0, 0xCC, 0); }
    if (ccData == NULL) { return; }
    if (ctx == 0) { ccCtx = 0; } else { ccCtx = fn_8012640C(ctx, 0, 0xCC, 0); }
    if (ccCtx == NULL) {
        value = 0;
    } else {
        if (fn_80119ED0(0x3D) == 0x7C || fn_80119ED0(0x3D) == 0xC8) {
            void* tmp;
            if (ctx == 0) { tmp = 0; } else { tmp = fn_8012640C(ctx, 0, 0xCC, 0); }
            result = fn_80121ADC(tmp, 0x3D);
        } else {
            if (fn_80119ED0(0x3D) != 0xCD) {
                result = 0;
            } else {
                result = fn_8011B67C(ctx, 0x3D);
            }
        }
        if (result == 1) {
            value = 0;
        } else {
            value = fn_80123090(ccCtx);
        }
    }
    fn_80122370(ccData, value, param);
}
#pragma pop

/* 0x8020341C | size: 0x140 */
#pragma push
#pragma scheduling off
void fn_8020341C(void* ctx, u32 param1, u32 param2) {
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
    extern u8 fn_80121ADC();
    extern void fn_801226D0();
    extern u32 fn_80123090();
    void* ccData;
    void* ccCtx;
    u8 result;
    u32 value;

    if (ctx == 0) { ccData = 0; } else { ccData = fn_8012640C(ctx, 0, 0xCC, 0); }
    if (ccData == NULL) { return; }
    if (ctx == 0) { ccCtx = 0; } else { ccCtx = fn_8012640C(ctx, 0, 0xCC, 0); }
    if (ccCtx == NULL) {
        value = 0;
    } else {
        if (fn_80119ED0(0x3D) == 0x7C || fn_80119ED0(0x3D) == 0xC8) {
            void* tmp;
            if (ctx == 0) { tmp = 0; } else { tmp = fn_8012640C(ctx, 0, 0xCC, 0); }
            result = fn_80121ADC(tmp, 0x3D);
        } else {
            if (fn_80119ED0(0x3D) != 0xCD) {
                result = 0;
            } else {
                result = fn_8011B67C(ctx, 0x3D);
            }
        }
        if (result == 1) {
            value = 0;
        } else {
            value = fn_80123090(ccCtx);
        }
    }
    fn_801226D0(ccData, value, param1, param2);
}
#pragma pop

/* 0x8020355C | size: 0x60 */
u32 fn_8020355C(u32 obj, u32 param) {
    extern u32 fn_801229F4();
    extern u32 fn_8012640C();
    u32 result;
    if (obj == 0) {
        result = 0;
    } else {
        result = fn_8012640C(obj, 0, 0xCC, 0);
    }
    if (result == 0) {
        return 0;
    }
    return fn_801229F4(result, param);
}

/* 0x802035BC | size: 0x64 */
void fn_802035BC(void* obj, u32 value) {
    void* intermediate;
    if (obj == NULL) {
        intermediate = NULL;
    } else {
        intermediate = fn_8012640C(obj, 0, 0xCC, 0);
    }
    if (intermediate != NULL) {
        fn_801254B4(intermediate, 0, 0x79, 0, value);
    }
}

/* fn_80203620 and fn_8020367C are implemented above as
   ResolveTrainerExtendedData and WriteTrainerExtendedData */

/* 0x802036D4 | size: 0x84 */
u32 fn_802036D4(void* ctx) {
    void* resolved;
    u16 species;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = fn_8012640C(resolved, 0, 0xCC, 0);
        }
    }
    species = (u16)(u32)fn_8012640C(resolved, 0, 0x6E, 0);
    return (u16)(u32)fn_8012640C(NULL, species, 0x61, 0);
}

/* 0x80203758 | size: 0x84 */
u32 fn_80203758(void* ctx) {
    extern u32 fn_800FA280();
    void* resolved;
    u16 species;
    resolved = fn_8012640C(ctx, 0, 0xD6, 0);
    if (resolved == NULL) {
        resolved = NULL;
    } else {
        resolved = fn_8012640C(resolved, 0, 0xCC, 0);
    }
    if (resolved == NULL) {
        return 0;
    }
    species = (u16)(u32)fn_8012640C(resolved, 0, 0x6E, 0);
    resolved = fn_8012640C(NULL, species, 0x01, 0);
    return fn_800FA280(resolved);
}

/* 0x802037DC | size: 0x6C */
void* fn_802037DC(void* ctx) {
    void* resolved;
    resolved = fn_8012640C(ctx, 0, 0xD6, 0);
    if (resolved == NULL) {
        resolved = NULL;
    } else {
        resolved = fn_8012640C(resolved, 0, 0xCC, 0);
    }
    if (resolved == NULL) {
        return NULL;
    }
    return fn_8012640C(resolved, 0, 0x77, 0);
}

/* 0x80203848 | size: 0x5C | small */
u32 fn_80203848(void* param_1) {
    void* iVar1;
    u32 uVar2;

    if (param_1 == NULL) {
        iVar1 = NULL;
    } else {
        iVar1 = fn_8012640C(param_1, 0, 0xCC, 0);
    }
    if (iVar1 == NULL) {
        uVar2 = 0;
    } else {
        uVar2 = (u32)fn_8012640C(iVar1, 0, 0x77, 0);
    }
    return uVar2;
}

/* Address: 0x802038A4 | Size: 0x1c8 | Ghidra import */

u8 fn_802038A4(int r3)

{
    extern s8 fn_80123FBC();
    extern short fn_801EF634();
  u8 bVar1;
  u16 sVar4;
  u32 iVar2;
  u8 cVar5;
  u32 iVar3;
  u8 uVar6;
  
  if (r3 == 0) {
    bVar1 = 0;
  }
  else {
    sVar4 = fn_801EF634();
    if (sVar4 == 1) {
      bVar1 = 0;
    }
    else {
      iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
      if (iVar3 == 0) {
        bVar1 = 0;
      }
      else {
        if (iVar3 == 0) {
          bVar1 = 0;
        }
        else {
        sVar4 = fn_801EF634();
        if (sVar4 == 1) {
          bVar1 = 0;
        }
        else {
          iVar2 = (int)fn_8012640C(iVar3,0,0xcb,0);
          if (iVar2 == 0) {
            bVar1 = 0;
          }
          else {
            cVar5 = fn_80123FBC();
            if (cVar5 == 0) {
              bVar1 = 0;
            }
            else {
              if (iVar3 == 0) {
                iVar2 = 0;
              }
              else {
                iVar2 = (int)fn_8012640C(iVar3,0,0xcc,0);
              }
              if (iVar2 == 0) {
                bVar1 = 0;
              }
              else {
                cVar5 = fn_80123FBC();
                if (cVar5 == 0) {
                  bVar1 = 0;
                }
                else {
                  iVar3 = (int)fn_8012640C(iVar3,0,0xce,0);
                  if ((int)iVar3 < 0) {
                    bVar1 = 0;
                  }
                  else {
                    bVar1 = 1;
                  }
                }
              }
            }
          }
        }
        }
        if (bVar1 == 0) {
          bVar1 = 0;
        }
        else {
          bVar1 = 1;
        }
      }
    }
  }
  if (bVar1 == 0) {
    uVar6 = 1;
  }
  else {
    if ((u32)r3 == 0) {
      iVar3 = 0;
    }
    else {
      iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
      if (iVar3 == 0) {
        iVar3 = 0;
      }
      else {
        iVar3 = (int)fn_8012640C(iVar3,0,0xcc,0);
      }
    }
    if (iVar3 == 0) {
      uVar6 = 1;
    }
    else {
      uVar6 = (int)fn_8012640C(iVar3,0,0x7b,0);
    }
  }
  return uVar6;
}
/* 0x80203A6C | size: 0x70 */
u32 fn_80203A6C(void* ctx) {
    extern u32 fn_80122A70();
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = fn_8012640C(resolved, 0, 0xCC, 0);
        }
    }
    if (resolved == NULL) {
        return 0;
    }
    return fn_80122A70(resolved);
}

/* 0x80203ADC | size: 0x80 */
u32 fn_80203ADC(void* ctx, u32 param) {
    extern u32 fn_80122AE0();
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = fn_8012640C(resolved, 0, 0xCC, 0);
        }
    }
    if (resolved == NULL) {
        return 0;
    }
    return fn_80122AE0(resolved, param);
}

/* 0x80203B5C | size: 0x80 */
u32 fn_80203B5C(void* ctx, u32 param) {
    extern u32 fn_80122B50();
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = fn_8012640C(resolved, 0, 0xCC, 0);
        }
    }
    if (resolved == NULL) {
        return 0;
    }
    return fn_80122B50(resolved, param);
}

/* 0x80203BDC | size: 0x80 */
u32 fn_80203BDC(void* ctx, u32 param) {
    extern u32 fn_80122BC0();
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = fn_8012640C(resolved, 0, 0xCC, 0);
        }
    }
    if (resolved == NULL) {
        return 0;
    }
    return fn_80122BC0(resolved, param);
}

/* 0x80203C5C | size: 0x70 */
u32 fn_80203C5C(void* ctx) {
    extern u32 fn_80122C64();
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = fn_8012640C(resolved, 0, 0xCC, 0);
        }
    }
    if (resolved == NULL) {
        return 0;
    }
    return fn_80122C64(resolved);
}

/* 0x80203CCC | size: 0x70 */
u32 fn_80203CCC(void* ctx) {
    extern u32 fn_80122DDC();
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = fn_8012640C(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = fn_8012640C(resolved, 0, 0xCC, 0);
        }
    }
    if (resolved == NULL) {
        return 0;
    }
    return fn_80122DDC(resolved);
}

/* 0x80203D3C | size: 0x70 */
u16 fn_80203D3C(void* ctx) {
    void* resolved;
    resolved = fn_8012640C(ctx, 0, 0xD6, 0);
    if (resolved == NULL) {
        resolved = NULL;
    } else {
        resolved = fn_8012640C(resolved, 0, 0xCC, 0);
    }
    if (resolved == NULL) {
        return 0;
    }
    return (u16)(u32)fn_8012640C(resolved, 0, 0x6E, 0);
}

/* 0x80203DAC | size: 0x60 */
u16 fn_80203DAC(void* ctx) {
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = fn_8012640C(ctx, 0, 0xCC, 0);
    }
    if (resolved == NULL) {
        return 0;
    }
    return (u16)(u32)fn_8012640C(resolved, 0, 0x6E, 0);
}

/* 0x80203E0C | size: 0x70 */
u8 fn_80203E0C(void* ctx) {
    void* resolved;
    resolved = fn_8012640C(ctx, 0, 0xD6, 0);
    if (resolved == NULL) {
        resolved = NULL;
    } else {
        resolved = fn_8012640C(resolved, 0, 0xCC, 0);
    }
    if (resolved == NULL) {
        return 0;
    }
    return (u8)(u32)fn_8012640C(resolved, 0, 0x7A, 0);
}

/* 0x80203E7C | size: 0x60 */
u32 fn_80203E7C(u32 obj) {
    extern u32 fn_8012640C();
    u32 result;
    if (obj == 0) {
        result = 0;
    } else {
        result = fn_8012640C(obj, 0, 0xCC, 0);
    }
    if (result == 0) {
        return 0;
    }
    return fn_8012640C(result, 0, 0x7A, 0) & 0xFF;
}

/* 0x80203EDC | size: 0x108 */
u16 fn_80203EDC(void* ctx) {
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
    extern u8 fn_80121ADC();
    extern u16 fn_80122FF4();
    void* d6Data;
    void* ccData;
    u16 typeId;
    u8 result;

    d6Data = fn_8012640C(ctx, 0, 0xD6, 0);
    ccData = !d6Data ? NULL : fn_8012640C(d6Data, 0, 0xCC, 0);
    if (ccData == NULL) { return 0; }
    typeId = fn_80119ED0(0x3D);
    if (typeId == 0x7C || typeId == 0xC8) {
        result = fn_80121ADC(!d6Data ? NULL : fn_8012640C(d6Data, 0, 0xCC, 0), 0x3D);
    } else if (fn_80119ED0(0x3D) == 0xCD) {
        result = fn_8011B67C(d6Data, 0x3D);
    } else {
        result = 0;
    }
    if (result == 1) { return 0; }
    return fn_80122FF4(ccData);
}

/* 0x80203FE4 | size: 0x104 */
u32 fn_80203FE4(void* ctx) {
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
    extern u8 fn_80121ADC();
    extern u32 fn_80123090();
    void* d6Data;
    void* ccData;
    u16 typeId;
    u8 result;

    d6Data = fn_8012640C(ctx, 0, 0xD6, 0);
    ccData = !d6Data ? NULL : fn_8012640C(d6Data, 0, 0xCC, 0);
    if (ccData == NULL) { return 0; }
    typeId = fn_80119ED0(0x3D);
    if (typeId == 0x7C || typeId == 0xC8) {
        result = fn_80121ADC(!d6Data ? NULL : fn_8012640C(d6Data, 0, 0xCC, 0), 0x3D);
    } else if (fn_80119ED0(0x3D) == 0xCD) {
        result = fn_8011B67C(d6Data, 0x3D);
    } else {
        result = 0;
    }
    if (result == 1) { return 0; }
    return fn_80123090(ccData);
}

/* 0x802041EC | size: 0xF4 | medium */
u32 fn_802041EC(void* param_1) {
    extern s16 fn_80119ED0(u32);
    extern s8 fn_8011B67C(void*, u32);
    extern s8 fn_80121ADC(void*, u32);
    extern u32 fn_80123090(void*);
    u32 uVar1;
    s16 sVar2;
    s8 cVar3;
    void* iVar4;

    if (param_1 == NULL) {
        iVar4 = NULL;
    } else {
        iVar4 = fn_8012640C(param_1, 0, 0xCC, 0);
    }
    if (iVar4 == NULL) {
        uVar1 = 0;
    } else {
        sVar2 = fn_80119ED0(0x3D);
        if ((sVar2 == 0x7C) || (sVar2 = fn_80119ED0(0x3D), sVar2 == 200)) {
            if (param_1 == NULL) {
                uVar1 = 0;
            } else {
                uVar1 = (u32)fn_8012640C(param_1, 0, 0xCC, 0);
            }
            cVar3 = fn_80121ADC((void*)uVar1, 0x3D);
        } else {
            sVar2 = fn_80119ED0(0x3D);
            if (sVar2 == 0xCD) {
                cVar3 = fn_8011B67C(param_1, 0x3D);
            } else {
                cVar3 = 0;
            }
        }
        if (cVar3 == 1) {
            uVar1 = 0;
        } else {
            uVar1 = fn_80123090(iVar4);
        }
    }
    return uVar1;
}

/* 0x802042E0 | size: 0xF4 | medium */
u32 fn_802042E0(void* param_1) {
    extern s16 fn_80119ED0(u32);
    extern s8 fn_8011B67C(void*, u32);
    extern s8 fn_80121ADC(void*, u32);
    extern u32 fn_801230E0(void*);
    u32 uVar1;
    s16 sVar2;
    s8 cVar3;
    void* iVar4;

    if (param_1 == NULL) {
        iVar4 = NULL;
    } else {
        iVar4 = fn_8012640C(param_1, 0, 0xCC, 0);
    }
    if (iVar4 == NULL) {
        uVar1 = 0;
    } else {
        sVar2 = fn_80119ED0(0x3D);
        if ((sVar2 == 0x7C) || (sVar2 = fn_80119ED0(0x3D), sVar2 == 200)) {
            if (param_1 == NULL) {
                uVar1 = 0;
            } else {
                uVar1 = (u32)fn_8012640C(param_1, 0, 0xCC, 0);
            }
            cVar3 = fn_80121ADC((void*)uVar1, 0x3D);
        } else {
            sVar2 = fn_80119ED0(0x3D);
            if (sVar2 == 0xCD) {
                cVar3 = fn_8011B67C(param_1, 0x3D);
            } else {
                cVar3 = 0;
            }
        }
        if (cVar3 == 1) {
            uVar1 = 0;
        } else {
            uVar1 = fn_801230E0(iVar4);
        }
    }
    return uVar1;
}

/* Address: 0x802043D4 | Size: 0x480 | Ghidra import */
u32 fn_802043D4(void)

{
    int r3;
    char r4;
    char r5;
    u32 r6;
    int r7;

    extern short fn_80119ED0();
    extern s8 fn_8011B67C();
    extern s8 fn_80121ADC();
    extern u32 fn_80122FF4();
    extern short fn_80123090();
    extern s8 fn_8012A5B0();
    extern int fn_8020E4E8();
  u32 uVar1;
  short sVar4;
  int iVar2;
  short sVar5;
  short sVar6;
  u8 uVar7;
  s8 cVar8;
  u32 uVar3;
  int iVar9;
  u32 uVar10;
  int iVar11;
  
  if (r3 == 0) {
    iVar11 = 0;
  }
  else {
    iVar11 = (int)fn_8012640C(r3,0,0xd6,0);
    if (iVar11 == 0) {
      iVar11 = 0;
    }
    else {
      iVar11 = (int)fn_8012640C(iVar11,0,0xcc,0);
    }
  }
  if (iVar11 == 0) {
    uVar1 = 0;
  }
  else {
    sVar4 = (int)fn_8012640C(r3,0,0x100,0);
    iVar2 = (int)fn_8012640C(r3,0,0xd6,0);
    if (iVar2 == 0) {
      iVar9 = 0;
    }
    else {
      iVar9 = (int)fn_8012640C(iVar2,0,0xcc,0);
    }
    if (iVar9 == 0) {
      sVar5 = 0;
    }
    else {
      sVar5 = fn_80119ED0(0x3d);
      if ((sVar5 == 0x7c) || (sVar5 = fn_80119ED0(0x3d), sVar5 == 200)) {
        if (iVar2 == 0) {
          uVar3 = 0;
        }
        else {
          uVar3 = (int)fn_8012640C(iVar2,0,0xcc,0);
        }
        cVar8 = fn_80121ADC(uVar3,0x3d);
      }
      else {
        sVar5 = fn_80119ED0(0x3d);
        if (sVar5 == 0xcd) {
          cVar8 = fn_8011B67C(iVar2,0x3d);
        }
        else {
          cVar8 = 0;
        }
      }
      if (cVar8 == 1) {
        sVar5 = 0;
      }
      else {
        sVar5 = fn_80123090(iVar9);
      }
    }
    iVar2 = (int)fn_8012640C(r3,0,0xd6,0);
    if (iVar2 == 0) {
      iVar9 = 0;
    }
    else {
      iVar9 = (int)fn_8012640C(iVar2,0,0xcc,0);
    }
    if (iVar9 == 0) {
      uVar10 = 0;
    }
    else {
      sVar6 = fn_80119ED0(0x3d);
      if ((sVar6 == 0x7c) || (sVar6 = fn_80119ED0(0x3d), sVar6 == 200)) {
        if (iVar2 == 0) {
          uVar3 = 0;
        }
        else {
          uVar3 = (int)fn_8012640C(iVar2,0,0xcc,0);
        }
        cVar8 = fn_80121ADC(uVar3,0x3d);
      }
      else {
        sVar6 = fn_80119ED0(0x3d);
        if (sVar6 == 0xcd) {
          cVar8 = fn_8011B67C(iVar2,0x3d);
        }
        else {
          cVar8 = 0;
        }
      }
      if (cVar8 == 1) {
        uVar10 = 0;
      }
      else {
        uVar10 = fn_80122FF4(iVar9);
        uVar10 = uVar10 & 0xffff;
      }
    }
    uVar7 = (int)fn_8012640C(r3,0,0xea,0);
    if (r7 == 0) {
      cVar8 = 0;
    }
    else {
      cVar8 = fn_8012A5B0(r7,0x11,0);
    }
    uVar1 = (int)fn_8012640C(iVar11,0,0x8c,0);
    uVar1 = uVar1 & 0xffff;
    if ((sVar4 == 0x21) && (r5 == 2)) {
      uVar1 = uVar1 << 1;
    }
    else if ((sVar4 == 0x22) && (r5 == 1)) {
      uVar1 = uVar1 << 1;
    }
    uVar1 = fn_8020E4E8(uVar7,uVar1);
    if ((r4 == 1) && (cVar8 == 1)) {
      uVar1 = (uVar1 * 0x6e) / 100;
    }
    if (sVar5 == 0x18) {
      uVar1 = uVar1 >> 1;
    }
    sVar4 = fn_80119ED0(5);
    if (((sVar4 == 0x7c) || (sVar4 = fn_80119ED0(5), sVar4 == 200)) ||
       (sVar4 = fn_80119ED0(5), sVar4 == 0xcd)) {
      iVar11 = (int)fn_8012640C(r3,0,0xd6,0);
      sVar4 = fn_80119ED0(5);
      if ((sVar4 == 0x7c) || (sVar4 = fn_80119ED0(5), sVar4 == 200)) {
        if (iVar11 == 0) {
          uVar3 = 0;
        }
        else {
          uVar3 = (int)fn_8012640C(iVar11,0,0xcc,0);
        }
        cVar8 = fn_80121ADC(uVar3,5);
      }
      else {
        sVar4 = fn_80119ED0(5);
        if (sVar4 == 0xcd) {
          cVar8 = fn_8011B67C(iVar11,5);
        }
        else {
          cVar8 = 0;
        }
      }
    }
    else {
      sVar4 = fn_80119ED0(5);
      if (sVar4 == 0xd8) {
        cVar8 = fn_8011B67C(r3,5);
      }
      else {
        cVar8 = 0;
      }
    }
    if (cVar8 == 1) {
      uVar1 = uVar1 >> 2;
    }
    if ((sVar5 == 0x1a) && ((int)(r6 & 0xffff) < (int)(uVar10 * 0xffff) / 100)) {
      uVar1 = 0xffffffff;
    }
  }
  return uVar1;
}
/* 0x80204854 | size: 0xD4 | medium */
u32 fn_80204854(void* param_1, void* param_2) {
    extern u8 fn_801F1170(void*);
    extern s16 fn_8020D8D8(void*);
    extern u16 fn_8020D950(void*);
    void* iVar2;
    s16 sVar3;
    u32 uVar1;
    s16 sVar4;
    u8 cVar5;

    sVar3 = (s16)(u32)fn_8012640C(param_2, 0, 0xCE, 0);
    if (sVar3 < 0) {
        uVar1 = 0;
    } else {
        sVar4 = (s16)(u32)fn_8012640C(param_1, 0, 0x121, 0);
        if (sVar3 == sVar4) {
            uVar1 = 1;
        } else {
            iVar2 = fn_8012640C(param_1, 0, 0xFE, 0);
            if ((((iVar2 != NULL) && (cVar5 = fn_801F1170(iVar2), cVar5 == 1)) &&
                ((u16)fn_8020D950(iVar2) == 9)) &&
               (sVar4 = fn_8020D8D8(iVar2), sVar3 == sVar4)) {
                uVar1 = 1;
            } else {
                uVar1 = 0;
            }
        }
    }
    return uVar1;
}

/* 0x80204928 | size: 0x48 | small */
#pragma push
#pragma peephole on
u8 fn_80204928(u32 expected, void* ctx) {
    u32 result = (u32)fn_8012640C(ctx, 0, 0xd5, 0);
    return (result == expected) ? 1 : 0;
}
#pragma pop

/* Address: 0x80204970 | Size: 0xa0 | Ghidra import */
void fn_80204970(void)

{
    int r3;
    int r4;

  u32 *puVar1;
  u32 *puVar2;
  u32 uVar3;
  u32 *puVar4;
  u32 *puVar5;
  int iVar6;
  u32 uStack_15c;
  u32 local_158 [86];
  
  if ((r3 != 0) && (r4 != 0)) {
    iVar6 = 0x2a;
    puVar1 = (u32 *)(r3 + -4);
    puVar2 = &uStack_15c;
    do {
      puVar5 = puVar2;
      puVar4 = puVar1;
      uVar3 = puVar4[2];
      puVar5[1] = puVar4[1];
      puVar5[2] = uVar3;
      iVar6 = iVar6 + -1;
      puVar1 = puVar4 + 2;
      puVar2 = puVar5 + 2;
    } while (iVar6 != 0);
    puVar5[3] = puVar4[3];
    iVar6 = 0x2a;
    puVar1 = (u32 *)(r4 + -4);
    puVar2 = (u32 *)(r3 + -4);
    do {
      puVar5 = puVar2;
      puVar4 = puVar1;
      uVar3 = puVar4[2];
      puVar5[1] = puVar4[1];
      puVar5[2] = uVar3;
      iVar6 = iVar6 + -1;
      puVar1 = puVar4 + 2;
      puVar2 = puVar5 + 2;
    } while (iVar6 != 0);
    puVar5[3] = puVar4[3];
    iVar6 = 0x2a;
    puVar1 = &uStack_15c;
    puVar2 = (u32 *)(r4 + -4);
    do {
      puVar5 = puVar2;
      puVar4 = puVar1;
      uVar3 = puVar4[2];
      puVar5[1] = puVar4[1];
      puVar5[2] = uVar3;
      iVar6 = iVar6 + -1;
      puVar1 = puVar4 + 2;
      puVar2 = puVar5 + 2;
    } while (iVar6 != 0);
    puVar5[3] = puVar4[3];
  }
  return;
}
#pragma peephole off
void* fn_802040E8(void) {
    extern void* fn_8012640C();
    extern u32 fn_80119ED0();
    extern u32 fn_80121ADC();
    extern void* fn_8011B67C();
    extern void* fn_801230E0();
    void* alloc2;
    void* alloc1;
    u32 r0;

    if ((alloc1 = fn_8012640C(0, 0, 0xD6, 0)) != 0) {
        alloc2 = fn_8012640C(0, 0, 0xCC, 0);
    } else {
        alloc2 = 0;
    }
    if (alloc2 == 0) {
        return 0;
    }
    if ((u16)(u32)fn_80119ED0(0x3D) == 0x7C ||
        (u16)(u32)fn_80119ED0(0x3D) == 0xC8) {
        r0 = fn_80121ADC(alloc1 ? fn_8012640C(alloc1, 0, 0xCC, 0) : 0, 0x3D);
    } else {
        r0 = fn_80119ED0(0x3D);
        if ((u16)r0 != 0xCD) {
            return 0;
        }
        r0 = (u32)fn_8011B67C(alloc1, 0x3D);
    }
    if ((u8)r0 == 1) {
        return 0;
    }
    return fn_801230E0(alloc2);
}

/* fn_80204A10 | Size: 0x4C | Check if trainer slot is active */
#pragma push
#pragma peephole on
u8 fn_80204A10(u32 slotId) {
    extern void* fn_801F4354(u32 context, u32 slot);
    extern u8 fn_801FB8F8(void* trainer);
    void* trainer = fn_801F4354(0, slotId);
    if (trainer == NULL) {
        return 0;
    }
    return fn_801FB8F8(trainer) == 1;
}
#pragma pop

/* 0x80204A5C | size: 0x1AC | medium */
/* 0x80204A5C | size: 0x1AC */
#pragma push
#pragma peephole on
u32 fn_80204A5C(void* ctx, u8 targetSlot, u8 mode) {
    extern u32 lbl_80478BD8;
    extern u8 fn_80142984();
    extern u16 fn_801F0898();
    extern u8 fn_801F1170();
    extern void fn_801F54A4();
    u16 field1E;
    u16 field1F;
    int e5Data;
    int feData;
    u8 valid;
    u32 i;

    for (i = 0; (u16)i < lbl_80478BD8; i++) {
        if ((u8)fn_80142984(i) == 0) { continue; }
        if (mode == 1) {
            if (targetSlot != (u8)itemGetStatus(0, i, 0x2, 0)) { continue; }
        } else {
            if (targetSlot == (u8)itemGetStatus(0, i, 0x2, 0)) { continue; }
        }
        fn_801F54A4(0, 0, 0x14, 0);
        if (ctx == NULL) { valid = 0; }
        else {
            feData = (int)fn_8012640C(ctx, 0, 0xFE, 0);
            if (feData == 0) { valid = 0; }
            else if ((u8)fn_801F1170(feData) == 0) { valid = 0; }
            else if (fn_801F0898(feData) != 0x12) { valid = 0; }
            else {
                e5Data = (int)fn_8012640C(ctx, 0, 0xE5, 0);
                if (e5Data == 0) { valid = 0; }
                else {
                    field1E = (u16)itemGetStatus(e5Data, 0, 0x1E, 0);
                    field1F = (u16)itemGetStatus(e5Data, 0, 0x1F, 0);
                    if ((u16)i != 0 && field1E != (u16)i) {
                        valid = 0;
                    } else {
                        valid = 1;
                    }
                }
            }
        }
        if (valid == 1) { return 1; }
    }
    return 0;
}
#pragma pop

/* Address: 0x80204C08 | Size: 0xd8 | Ghidra import */
u16 fn_80204C08(void)

{
    int r3;

    extern short fn_801F0898();
    extern s8 fn_801F1170();
    extern void fn_801F54A4();
  int iVar1;
  s8 cVar4;
  short sVar2;
  u16 uVar3;
  
  fn_801F54A4(0,0,0x14,0);
  if (r3 == 0) {
    uVar3 = 0;
  }
  else {
    iVar1 = (int)fn_8012640C(r3,0,0xfe,0);
    if (iVar1 == 0) {
      uVar3 = 0;
    }
    else {
      cVar4 = fn_801F1170();
      if (cVar4 == 0) {
        uVar3 = 0;
      }
      else {
        sVar2 = fn_801F0898(iVar1);
        if (sVar2 == 0x12) {
          iVar1 = (int)fn_8012640C(r3,0,0xe5,0);
          if (iVar1 == 0) {
            uVar3 = 0;
          }
          else {
            uVar3 = itemGetStatus(iVar1,0,0x1e,0);
          }
        }
        else {
          uVar3 = 0;
        }
      }
    }
  }
  return uVar3;
}
/* 0x80204CE0 | size: 0x104 */
void* fn_80204CE0(void* ctx, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8, u8 p9) {
    extern void fn_80142B24();
    extern u8 fn_801F11CC();
    extern void fn_8020A398();
    extern void fn_8020D878();
    void* e5Data;
    void* feData;

    e5Data = fn_8012640C(ctx, 0, 0xE5, 0);
    if (e5Data == NULL) { return NULL; }
    fn_8020A398(e5Data, (u16)p6, p7, p8);
    fn_80142B24(e5Data, 0, 0x21, 0, (u32)p9);
    feData = fn_8012640C(ctx, 0, 0xFE, 0);
    if (feData == NULL) { feData = NULL; }
    else {
        if ((u8)fn_801F11CC(feData, p2, ctx, p3, p4, p5) == 1) {
            fn_8020D878(feData, p6);
        } else {
            feData = NULL;
        }
    }
    return feData;
}

/* 0x80204DE4 | size: 0x188 */
#pragma push
#pragma peephole on
u32 fn_80204DE4(void* ctx, u16 slotId, void* tablePtr) {
    extern u16 fn_8011BEB4();
    extern void* fn_801F0134();
    extern void* fn_801F025C();
    extern u16 fn_801F0898();
    extern u8 fn_801F1170();
    extern u16 fn_801F54A4();
    void* feData;
    void* d9Data;
    u16 partyCount;
    void* savedEntry;
    u16 field27;
    u16 field09;
    u32 field29;

    partyCount = (u16)fn_801F54A4(0, 0, 0x14, 0);
    if (ctx == NULL) { return 0; }
    savedEntry = !tablePtr ? NULL : fn_801F0134(tablePtr, partyCount);
    feData = fn_8012640C(ctx, 0, 0xFE, 0);
    if (feData == NULL) { return 0; }
    if ((u8)fn_801F1170(feData) == 0) { return 0; }
    if (fn_801F0898(feData) != 0x13) { return 0; }
    d9Data = fn_8012640C(ctx, 0, 0xD9, 0);
    if (d9Data == NULL) { return 0; }
    field27 = (u16)fn_8011BEB4(d9Data, 0, 0x27, 0);
    field09 = (u16)fn_8011BEB4(0, field27, 0x9, 0);
    if (slotId != 0 && field27 != slotId) { return 0; }
    field29 = (u32)fn_8011BEB4(d9Data, 0, 0x29, 0);
    if (field09 == 0xB0) {
        field29 = (u32)fn_801F0134(fn_801F025C(0xE, ctx), partyCount);
    }
    if ((u16)(u32)savedEntry != 0 && (u16)field29 != (u16)(u32)savedEntry) { return 0; }
    return 1;
}
#pragma pop

/* Address: 0x80204F6C | Size: 0xf0 | Ghidra import */
int fn_80204F6C(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;
    u32 r8;
    u32 r9;
    u32 r10;
    u8 param_9;

    extern s8 fn_801F11CC();
    extern void fn_802099AC();
    extern void fn_8020D878();
  int iVar1;
  s8 cVar2;
  
  iVar1 = (int)fn_8012640C(r3,0,0xd9,0);
  if (iVar1 == 0) {
    iVar1 = 0;
  }
  else {
    fn_802099AC(iVar1,r10,r8 & 0xffff,r9,param_9);
    iVar1 = (int)fn_8012640C(r3,0,0xfe,0);
    if (iVar1 == 0) {
      iVar1 = 0;
    }
    else {
      cVar2 = fn_801F11CC(iVar1,r4,r3,r5,r6,r7);
      if (cVar2 == 1) {
        fn_8020D878(iVar1,r8);
      }
      else {
        iVar1 = 0;
      }
    }
    if (iVar1 == 0) {
      iVar1 = 0;
    }
  }
  return iVar1;
}
/* Address: 0x8020505C | Size: 0x98 | Ghidra import */
int fn_8020505C(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;
    u32 r8;

    extern s8 fn_801F11CC();
    extern void fn_8020D878();
  int iVar1;
  s8 cVar2;
  
  iVar1 = (int)fn_8012640C(r3,0,0xfe,0);
  if (iVar1 == 0) {
    iVar1 = 0;
  }
  else {
    cVar2 = fn_801F11CC(iVar1,r4,r3,r5,r6,r7);
    if (cVar2 == 1) {
      fn_8020D878(iVar1,r8);
    }
    else {
      iVar1 = 0;
    }
  }
  return iVar1;
}
#pragma push
#pragma peephole on
#if 0
asm void fn_802050F4(void) {
#include "src/game/colosseum_event_fn_802050F4.inc"
}
#else
void* fn_802050F4(void* ctx) {
    void* p;
    p = fn_8012640C(ctx, 0, 0xFE, 0);
    if (p == NULL) {
        p = (void*)-0x80;
    } else {
        p = fn_801F0928(p);
    }
    return p;
}
#endif
#pragma pop

/* fn_80205134 | Size: 0x50 | Get field 0x30 from resolved 0xD9, default 9 */
#pragma push
#pragma peephole on
u16 fn_80205134(void* ctx) {
    extern u32 fn_8011BEB4();
    void* resolved = fn_8012640C(ctx, 0, 0xD9, 0);
    if (resolved == NULL) {
        return 9;
    }
    return (u16)fn_8011BEB4(resolved, 0, 0x30, 0);
}

u16 fn_80205184(void* ctx) {
    extern void* fn_8012640C();
    extern u32 fn_8011BEB4();
    void* resolved;
    resolved = fn_8012640C(ctx, 0, 0xD9, 0);
    if (resolved == 0) {
        return 0;
    }
    return (u16)fn_8011BEB4(resolved, 0, 0x28, 0);
}

/* fn_802051D4 | Size: 0x50 | Get field 0x27 from resolved 0xD9, default 0 */
u16 fn_802051D4(void* ctx) {
    extern u32 fn_8011BEB4();
    void* resolved = fn_8012640C(ctx, 0, 0xD9, 0);
    if (resolved == NULL) {
        return 0;
    }
    return (u16)fn_8011BEB4(resolved, 0, 0x27, 0);
}

/* fn_80205224 | Size: 0x50 | Get field 0x27 from resolved 0xD9, default 0 */
u16 fn_80205224(void* ctx) {
    extern u32 fn_8011BEB4();
    void* resolved = fn_8012640C(ctx, 0, 0xD9, 0);
    if (resolved == NULL) {
        return 0;
    }
    return (u16)fn_8011BEB4(resolved, 0, 0x27, 0);
}
#pragma pop

/* Address: 0x80205274 | Size: 0x690 | Ghidra import */


void fn_80205274(int r3,u32 r4)

{
    extern s8 fn_801233F4();
    extern s8 fn_80123FBC();
    extern short fn_801EF634();
    extern u32 fn_801F54A4();
    extern u32 fightSideGetStatus();
    extern s8 fn_801FA634();
    extern int fn_801FB1C0();
  u32 bVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u8 cVar10;
  int iVar5;
  int iVar6;
  int iVar7;
  short sVar8;
  short sVar9;
  u8 bVar11;
  u32 uVar12;
  u32 uVar13;
  
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801F54A4(0,0,0x16,0);
  uVar3 = fn_801F54A4(0,0,0x18,0);
  uVar12 = 0;
  do {
    if ((uVar2 & 0xffff) <= (uVar12 & 0xffff)) {
      return;
    }
    uVar4 = fightSideGetStatus(r4,0,7,uVar12);
    cVar10 = fn_801FA634();
    if (cVar10 != 0) {
      for (uVar13 = 0; (uVar13 & 0xffff) < (uVar3 & 0xffff); uVar13 = uVar13 + 1) {
        iVar5 = fn_801FB1C0(uVar4,0,0x46,uVar13);
        if (iVar5 == 0) {
          bVar1 = 0;
        }
        else {
          sVar8 = fn_801EF634();
          if (sVar8 == 1) {
            bVar1 = 0;
          }
          else {
            iVar7 = (int)fn_8012640C(iVar5,0,0xd6,0);
            if (iVar7 == 0) {
              bVar1 = 0;
            }
            else {
              sVar8 = fn_801EF634();
              if (sVar8 == 1) {
                bVar1 = 0;
              }
              else {
                iVar6 = (int)fn_8012640C(iVar7,0,0xcb,0);
                if (iVar6 == 0) {
                  bVar1 = 0;
                }
                else {
                  cVar10 = fn_80123FBC();
                  if (cVar10 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    if (iVar7 == 0) {
                      iVar6 = 0;
                    }
                    else {
                      iVar6 = (int)fn_8012640C(iVar7,0,0xcc,0);
                    }
                    if (iVar6 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      cVar10 = fn_80123FBC();
                      if (cVar10 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        iVar7 = (int)fn_8012640C(iVar7,0,0xce,0);
                        if (iVar7 < 0) {
                          bVar1 = 0;
                        }
                        else {
                          bVar1 = 1;
                        }
                      }
                    }
                  }
                }
              }
              if (bVar1) {
                bVar1 = 1;
              }
              else {
                bVar1 = 0;
              }
            }
          }
          if (bVar1) {
            iVar7 = (int)fn_8012640C(iVar5,0,0x120,0);
            if (iVar7 == 1) {
              bVar1 = 0;
            }
            else {
              iVar7 = (int)fn_8012640C(iVar5,0,0xd6,0);
              if (iVar7 == 0) {
                bVar1 = 0;
              }
              else {
                sVar8 = fn_801EF634();
                if (sVar8 == 1) {
                  bVar1 = 0;
                }
                else {
                  iVar6 = (int)fn_8012640C(iVar7,0,0xcb,0);
                  if (iVar6 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    cVar10 = fn_80123FBC();
                    if (cVar10 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      if (iVar7 == 0) {
                        iVar6 = 0;
                      }
                      else {
                        iVar6 = (int)fn_8012640C(iVar7,0,0xcc,0);
                      }
                      if (iVar6 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        cVar10 = fn_80123FBC();
                        if (cVar10 == 0) {
                          bVar1 = 0;
                        }
                        else {
                          iVar6 = (int)fn_8012640C(iVar7,0,0xce,0);
                          if (iVar6 < 0) {
                            bVar1 = 0;
                          }
                          else {
                            bVar1 = 1;
                          }
                        }
                      }
                    }
                  }
                }
                if (bVar1) {
                  iVar6 = (int)fn_8012640C(iVar7,0,0xd2,0);
                  if (iVar6 == 1) {
                    bVar1 = 0;
                  }
                  else {
                    if (iVar7 == 0) {
                      iVar7 = 0;
                    }
                    else {
                      iVar7 = (int)fn_8012640C(iVar7,0,0xcc,0);
                    }
                    if (iVar7 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      cVar10 = fn_801233F4();
                      if (cVar10 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        bVar1 = 1;
                      }
                    }
                  }
                }
                else {
                  bVar1 = 0;
                }
              }
              if (bVar1) {
                bVar1 = 1;
              }
              else {
                bVar1 = 0;
              }
            }
          }
          else {
            bVar1 = 0;
          }
        }
        if ((bVar1) && (iVar5 = (int)fn_8012640C(iVar5,0,0xd5,0), r3 != 0)) {
          if (iVar5 == 0) {
            bVar1 = 0;
          }
          else {
            sVar8 = fn_801EF634();
            if (sVar8 == 1) {
              bVar1 = 0;
            }
            else {
              iVar7 = (int)fn_8012640C(iVar5,0,0xcb,0);
              if (iVar7 == 0) {
                bVar1 = 0;
              }
              else {
                cVar10 = fn_80123FBC();
                if (cVar10 == 0) {
                  bVar1 = 0;
                }
                else {
                  if (iVar5 == 0) {
                    iVar7 = 0;
                  }
                  else {
                    iVar7 = (int)fn_8012640C(iVar5,0,0xcc,0);
                  }
                  if (iVar7 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    cVar10 = fn_80123FBC();
                    if (cVar10 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      iVar7 = (int)fn_8012640C(iVar5,0,0xce,0);
                      if (iVar7 < 0) {
                        bVar1 = 0;
                      }
                      else {
                        bVar1 = 1;
                      }
                    }
                  }
                }
              }
            }
          }
          if (bVar1) {
            if (r3 == 0) {
              bVar1 = 0;
            }
            else {
              if (iVar5 == 0) {
                bVar1 = 0;
              }
              else {
                sVar8 = fn_801EF634();
                if (sVar8 == 1) {
                  bVar1 = 0;
                }
                else {
                  iVar7 = (int)fn_8012640C(iVar5,0,0xcb,0);
                  if (iVar7 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    cVar10 = fn_80123FBC();
                    if (cVar10 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      if (iVar5 == 0) {
                        iVar7 = 0;
                      }
                      else {
                        iVar7 = (int)fn_8012640C(iVar5,0,0xcc,0);
                      }
                      if (iVar7 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        cVar10 = fn_80123FBC();
                        if (cVar10 == 0) {
                          bVar1 = 0;
                        }
                        else {
                          iVar7 = (int)fn_8012640C(iVar5,0,0xce,0);
                          if (iVar7 < 0) {
                            bVar1 = 0;
                          }
                          else {
                            bVar1 = 1;
                          }
                        }
                      }
                    }
                  }
                }
              }
              if (bVar1) {
                sVar8 = (int)fn_8012640C(iVar5,0,0xce,0);
                for (bVar11 = 0; bVar11 < 0xc; bVar11 = bVar11 + 1) {
                  sVar9 = (int)fn_8012640C(r3,0,0xfd,bVar11);
                  if ((-1 < sVar9) && (sVar9 == sVar8)) {
                    bVar1 = 1;
                    goto LAB_00202858;
                  }
                }
                bVar1 = 0;
              }
              else {
                bVar1 = 0;
              }
            }
LAB_00202858:
            if (bVar1 == 0) {
              sVar8 = (int)fn_8012640C(iVar5,0,0xce,0);
              for (bVar11 = 0; bVar11 < 0xc; bVar11 = bVar11 + 1) {
                sVar9 = (int)fn_8012640C(r3,0,0xfd,bVar11);
                if (sVar9 < 0) {
                  fn_801254B4(r3,0,0xfd,bVar11,(int)sVar8);
                  break;
                }
              }
            }
          }
        }
      }
    }
    uVar12 = uVar12 + 1;
  } while (1);
}
/* Address: 0x80205904 | Size: 0x178 | Ghidra import */
u32 fn_80205904(void)

{
    int r3;
    int r4;

    extern s8 fn_80123FBC();
    extern short fn_801EF634();
  u32 bVar1;
  int iVar2;
  s8 cVar5;
  short sVar3;
  short sVar4;
  u8 bVar6;
  
  if (r3 != 0) {
    if (r4 == 0) {
      bVar1 = 0;
    }
    else {
      sVar3 = fn_801EF634();
      if (sVar3 == 1) {
        bVar1 = 0;
      }
      else {
        iVar2 = (int)fn_8012640C(r4,0,0xcb,0);
        if (iVar2 == 0) {
          bVar1 = 0;
        }
        else {
          cVar5 = fn_80123FBC();
          if (cVar5 == 0) {
            bVar1 = 0;
          }
          else {
            if (r4 == 0) {
              iVar2 = 0;
            }
            else {
              iVar2 = (int)fn_8012640C(r4,0,0xcc,0);
            }
            if (iVar2 == 0) {
              bVar1 = 0;
            }
            else {
              cVar5 = fn_80123FBC();
              if (cVar5 == 0) {
                bVar1 = 0;
              }
              else {
                iVar2 = (int)fn_8012640C(r4,0,0xce,0);
                if (iVar2 < 0) {
                  bVar1 = 0;
                }
                else {
                  bVar1 = 1;
                }
              }
            }
          }
        }
      }
    }
    if (bVar1) {
      sVar3 = (int)fn_8012640C(r4,0,0xce,0);
      for (bVar6 = 0; bVar6 < 0xc; bVar6 = bVar6 + 1) {
        sVar4 = (int)fn_8012640C(r3,0,0xfd,bVar6);
        if ((-1 < sVar4) && (sVar4 == sVar3)) {
          return 1;
        }
      }
    }
  }
  return 0;
}
/* fn_80205A7C | Size: 0x58 | Two-hop resolve and call fn_801232E0 */
#pragma push
#pragma peephole on
void fn_80205A7C(void* ctx, u32 param) {
    extern void fn_801232E0(void* obj, u32 param);
    if (ctx == NULL) {
        return;
    }
    ctx = fn_8012640C(ctx, 0, 0xD5, 0);
    ctx = fn_8012640C(ctx, 0, 0xCB, 0);
    fn_801232E0(ctx, param);
}
#pragma pop

/* fn_80205AD4 | Size: 0x58 | Two-hop resolve and call fn_80123368 */
#pragma push
#pragma peephole on
void fn_80205AD4(void* ctx, u32 param) {
    extern void fn_80123368(void* obj, u32 param);
    if (ctx == NULL) {
        return;
    }
    ctx = fn_8012640C(ctx, 0, 0xD5, 0);
    ctx = fn_8012640C(ctx, 0, 0xCB, 0);
    fn_80123368(ctx, param);
}
#pragma pop

/* fn_80205B2C | Size: 0x60 | Two-hop resolve (0xD5 -> 0xCE), return s16 or -1 */
#pragma push
#pragma peephole on
s16 fn_80205B2C(void* ctx) {
    void* hop1;
    if (ctx == NULL) {
        return -1;
    }
    hop1 = fn_8012640C(ctx, 0, 0xD5, 0);
    if (hop1 == NULL) {
        return -1;
    }
    return (s16)(u32)fn_8012640C(hop1, 0, 0xCE, 0);
}
#pragma pop

#pragma push
#pragma peephole on
void* fn_80205B8C(void* ctx) {
    extern void* fn_8012640C();
    if (ctx == 0) {
        return 0;
    }
    ctx = fn_8012640C(ctx, 0, 0xD6, 0);
    if (ctx == 0) {
        return 0;
    }
    return fn_8012640C(ctx, 0, 0xCC, 0);
}
#pragma pop

/* 0x80205BE8 | size: 0x3C | small */
#pragma push
#pragma peephole on
void* fn_80205BE8(void* ctx) {
    if (ctx == 0) return 0;
    return fn_8012640C(ctx, 0, 0xcc, 0);
}
#pragma pop

/* Address: 0x80205C24 | Size: 0x684 | Ghidra import */


u32 fn_80205C24(int r3,char r4)

{
    extern short fn_80119ED0();
    extern s8 fn_8011B67C();
    extern u32 fn_8011BEB4();
    extern s8 fn_80121ADC();
    extern s8 fn_801233F4();
    extern s8 fn_80123FBC();
    extern short fn_801EF634();
    extern u32 fn_801F0134();
    extern s8 fn_801F11CC();
    extern u32 fn_801F54A4();
    extern void fn_802099AC();
    extern int fn_80209CB4();
    extern void fn_8020D878();
    extern u32 fn_8022B2CC();
  u32 bVar1;
  u16 uVar5;
  int iVar2;
  short sVar6;
  int iVar3;
  u32 uVar4;
  s8 cVar8;
  u16 uVar7;
  
  uVar5 = fn_801F54A4(0,0,0x14,0);
  if (r3 != 0) {
    sVar6 = fn_801EF634();
    if (sVar6 == 1) {
      bVar1 = 0;
    }
    else {
      iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
      if (iVar3 == 0) {
        bVar1 = 0;
      }
      else {
        sVar6 = fn_801EF634();
        if (sVar6 == 1) {
          bVar1 = 0;
        }
        else {
          iVar2 = (int)fn_8012640C(iVar3,0,0xcb,0);
          if (iVar2 == 0) {
            bVar1 = 0;
          }
          else {
            cVar8 = fn_80123FBC();
            if (cVar8 == 0) {
              bVar1 = 0;
            }
            else {
              if (iVar3 == 0) {
                iVar2 = 0;
              }
              else {
                iVar2 = (int)fn_8012640C(iVar3,0,0xcc,0);
              }
              if (iVar2 == 0) {
                bVar1 = 0;
              }
              else {
                cVar8 = fn_80123FBC();
                if (cVar8 == 0) {
                  bVar1 = 0;
                }
                else {
                  iVar3 = (int)fn_8012640C(iVar3,0,0xce,0);
                  if (iVar3 < 0) {
                    bVar1 = 0;
                  }
                  else {
                    bVar1 = 1;
                  }
                }
              }
            }
          }
        }
        if (bVar1) {
          bVar1 = 1;
        }
        else {
          bVar1 = 0;
        }
      }
    }
    if (bVar1) {
      iVar3 = (int)fn_8012640C(r3,0,0x120,0);
      if (iVar3 == 1) {
        bVar1 = 0;
      }
      else {
        iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
        if (iVar3 == 0) {
          bVar1 = 0;
        }
        else {
          sVar6 = fn_801EF634();
          if (sVar6 == 1) {
            bVar1 = 0;
          }
          else {
            iVar2 = (int)fn_8012640C(iVar3,0,0xcb,0);
            if (iVar2 == 0) {
              bVar1 = 0;
            }
            else {
              cVar8 = fn_80123FBC();
              if (cVar8 == 0) {
                bVar1 = 0;
              }
              else {
                if (iVar3 == 0) {
                  iVar2 = 0;
                }
                else {
                  iVar2 = (int)fn_8012640C(iVar3,0,0xcc,0);
                }
                if (iVar2 == 0) {
                  bVar1 = 0;
                }
                else {
                  cVar8 = fn_80123FBC();
                  if (cVar8 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    iVar2 = (int)fn_8012640C(iVar3,0,0xce,0);
                    if (iVar2 < 0) {
                      bVar1 = 0;
                    }
                    else {
                      bVar1 = 1;
                    }
                  }
                }
              }
            }
          }
          if (bVar1) {
            iVar2 = (int)fn_8012640C(iVar3,0,0xd2,0);
            if (iVar2 == 1) {
              bVar1 = 0;
            }
            else {
              if (iVar3 == 0) {
                iVar3 = 0;
              }
              else {
                iVar3 = (int)fn_8012640C(iVar3,0,0xcc,0);
              }
              if (iVar3 == 0) {
                bVar1 = 0;
              }
              else {
                cVar8 = fn_801233F4();
                if (cVar8 == 0) {
                  bVar1 = 0;
                }
                else {
                  bVar1 = 1;
                }
              }
            }
          }
          else {
            bVar1 = 0;
          }
        }
        if (bVar1) {
          bVar1 = 1;
        }
        else {
          bVar1 = 0;
        }
      }
    }
    else {
      bVar1 = 0;
    }
    if (bVar1) {
      sVar6 = fn_80119ED0(0x12);
      if (((sVar6 == 0x7c) || (sVar6 = fn_80119ED0(0x12), sVar6 == 200)) ||
         (sVar6 = fn_80119ED0(0x12), sVar6 == 0xcd)) {
        iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
        sVar6 = fn_80119ED0(0x12);
        if ((sVar6 == 0x7c) || (sVar6 = fn_80119ED0(0x12), sVar6 == 200)) {
          if (iVar3 == 0) {
            uVar4 = 0;
          }
          else {
            uVar4 = (int)fn_8012640C(iVar3,0,0xcc,0);
          }
          cVar8 = fn_80121ADC(uVar4,0x12);
        }
        else {
          sVar6 = fn_80119ED0(0x12);
          if (sVar6 == 0xcd) {
            cVar8 = fn_8011B67C(iVar3,0x12);
          }
          else {
            cVar8 = 0;
          }
        }
      }
      else {
        sVar6 = fn_80119ED0(0x12);
        if (sVar6 == 0xd8) {
          cVar8 = fn_8011B67C(r3,0x12);
        }
        else {
          cVar8 = 0;
        }
      }
      if (cVar8 != 1) {
        sVar6 = fn_80119ED0(0x22);
        if (((sVar6 == 0x7c) || (sVar6 = fn_80119ED0(0x22), sVar6 == 200)) ||
           (sVar6 = fn_80119ED0(0x22), sVar6 == 0xcd)) {
          iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
          sVar6 = fn_80119ED0(0x22);
          if ((sVar6 == 0x7c) || (sVar6 = fn_80119ED0(0x22), sVar6 == 200)) {
            if (iVar3 == 0) {
              uVar4 = 0;
            }
            else {
              uVar4 = (int)fn_8012640C(iVar3,0,0xcc,0);
            }
            cVar8 = fn_80121ADC(uVar4,0x22);
          }
          else {
            sVar6 = fn_80119ED0(0x22);
            if (sVar6 == 0xcd) {
              cVar8 = fn_8011B67C(iVar3,0x22);
            }
            else {
              cVar8 = 0;
            }
          }
        }
        else {
          sVar6 = fn_80119ED0(0x22);
          if (sVar6 == 0xd8) {
            cVar8 = fn_8011B67C(r3,0x22);
          }
          else {
            cVar8 = 0;
          }
        }
        if (cVar8 != 1) {
          return 1;
        }
      }
      if (r4 != 0) {
        uVar4 = (int)fn_8012640C(r3,0,0xf8,0);
        cVar8 = fn_80209CB4();
        if (cVar8 != 0) {
          uVar7 = fn_8011BEB4(uVar4,0,0x28,0);
          cVar8 = fn_8011BEB4(uVar4,0,0x26,0);
          uVar4 = fn_8022B2CC(r3,uVar7,uVar5,0x802062a8,1,0, (void*)0xffffffff);
          uVar4 = fn_801F0134(uVar4,uVar5);
          iVar3 = (int)fn_8012640C(r3,0,0xd9,0);
          if (iVar3 != 0) {
            fn_802099AC(iVar3,(int)cVar8,uVar7,uVar4,1);
            iVar3 = (int)fn_8012640C(r3,0,0xfe,0);
            if ((iVar3 != 0) &&
               (cVar8 = fn_801F11CC(iVar3,0,r3,0x13,0,0x80375ca8), cVar8 == 1)) {
              fn_8020D878(iVar3,uVar7);
            }
          }
        }
      }
    }
  }
  return 0;
}
/* 0x802062A8 | size: 0x54 | small */
#pragma push
#pragma peephole on
void fn_802062A8(void* param_1, u32 param_2, u32 param_3) {
    extern u32 fn_8011BEB4();
    extern void fn_801F00D0();
    void* uVar1;
    u16 uVar2;

    uVar1 = fn_8012640C(param_1, 0, 0xF8, 0);
    uVar2 = (u16)fn_8011BEB4(uVar1, 0, 0x29, 0);
    fn_801F00D0(uVar2, param_3);
}
#pragma pop

/* Address: 0x802062FC | Size: 0x30c | Ghidra import */


u8 fn_802062FC(int r3)

{
    extern s8 fn_801233F4();
    extern s8 fn_80123FBC();
    extern short fn_801EF634();
  u32 bVar1;
  int iVar2;
  short sVar4;
  int iVar3;
  s8 cVar5;
  u8 uVar6;
  
  if (r3 == 0) {
    uVar6 = 0;
  }
  else {
    sVar4 = fn_801EF634();
    if (sVar4 == 1) {
      bVar1 = 0;
    }
    else {
      iVar2 = (int)fn_8012640C(r3,0,0xd6,0);
      if (iVar2 == 0) {
        bVar1 = 0;
      }
      else {
        sVar4 = fn_801EF634();
        if (sVar4 == 1) {
          bVar1 = 0;
        }
        else {
          iVar3 = (int)fn_8012640C(iVar2,0,0xcb,0);
          if (iVar3 == 0) {
            bVar1 = 0;
          }
          else {
            cVar5 = fn_80123FBC();
            if (cVar5 == 0) {
              bVar1 = 0;
            }
            else {
              if (iVar2 == 0) {
                iVar3 = 0;
              }
              else {
                iVar3 = (int)fn_8012640C(iVar2,0,0xcc,0);
              }
              if (iVar3 == 0) {
                bVar1 = 0;
              }
              else {
                cVar5 = fn_80123FBC();
                if (cVar5 == 0) {
                  bVar1 = 0;
                }
                else {
                  iVar2 = (int)fn_8012640C(iVar2,0,0xce,0);
                  if (iVar2 < 0) {
                    bVar1 = 0;
                  }
                  else {
                    bVar1 = 1;
                  }
                }
              }
            }
          }
        }
        if (bVar1) {
          bVar1 = 1;
        }
        else {
          bVar1 = 0;
        }
      }
    }
    if (bVar1) {
      iVar2 = (int)fn_8012640C(r3,0,0x120,0);
      if (iVar2 == 1) {
        uVar6 = 0;
      }
      else {
        iVar2 = (int)fn_8012640C(r3,0,0xd6,0);
        if (iVar2 == 0) {
          uVar6 = 0;
        }
        else {
          sVar4 = fn_801EF634();
          if (sVar4 == 1) {
            bVar1 = 0;
          }
          else {
            iVar3 = (int)fn_8012640C(iVar2,0,0xcb,0);
            if (iVar3 == 0) {
              bVar1 = 0;
            }
            else {
              cVar5 = fn_80123FBC();
              if (cVar5 == 0) {
                bVar1 = 0;
              }
              else {
                if (iVar2 == 0) {
                  iVar3 = 0;
                }
                else {
                  iVar3 = (int)fn_8012640C(iVar2,0,0xcc,0);
                }
                if (iVar3 == 0) {
                  bVar1 = 0;
                }
                else {
                  cVar5 = fn_80123FBC();
                  if (cVar5 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    iVar3 = (int)fn_8012640C(iVar2,0,0xce,0);
                    if (iVar3 < 0) {
                      bVar1 = 0;
                    }
                    else {
                      bVar1 = 1;
                    }
                  }
                }
              }
            }
          }
          if (bVar1) {
            iVar3 = (int)fn_8012640C(iVar2,0,0xd2,0);
            if (iVar3 == 1) {
              uVar6 = 0;
            }
            else {
              if (iVar2 == 0) {
                iVar2 = 0;
              }
              else {
                iVar2 = (int)fn_8012640C(iVar2,0,0xcc,0);
              }
              if (iVar2 == 0) {
                uVar6 = 0;
              }
              else {
                cVar5 = fn_801233F4();
                if (cVar5 == 0) {
                  uVar6 = 0;
                }
                else {
                  uVar6 = 1;
                }
              }
            }
          }
          else {
            uVar6 = 0;
          }
        }
      }
    }
    else {
      uVar6 = 0;
    }
  }
  return uVar6;
}
/* Address: 0x80206608 | Size: 0x178 | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_80206608(void* r3)

{
    extern u8 fn_801233F4(void*);
    extern u8 fn_80123FBC();
    extern u16 fn_801EF634();
  u16 sVar2;
  void* iVar1;
  u8 cVar3;
  u8 bVar4;
  
  if (r3 == 0) {
    return 0;
  }
  if (r3 == 0) {
    bVar4 = 0;
  }
  else {
    sVar2 = fn_801EF634();
    if (sVar2 == 1) {
      bVar4 = 0;
    }
    else {
      iVar1 = fn_8012640C(r3,0,0xcb,0);
      if (iVar1 == 0) {
        bVar4 = 0;
      }
      else {
        cVar3 = fn_80123FBC();
        if (cVar3 == 0) {
          bVar4 = 0;
        }
        else {
          if (r3 == 0) {
            iVar1 = 0;
          }
          else {
            iVar1 = fn_8012640C(r3,0,0xcc,0);
          }
          if (iVar1 == 0) {
            bVar4 = 0;
          }
          else {
            cVar3 = fn_80123FBC();
            if (cVar3 == 0) {
              bVar4 = 0;
            }
            else {
              iVar1 = fn_8012640C(r3,0,0xce,0);
              if ((s32)iVar1 < 0) {
                bVar4 = 0;
              }
              else {
                bVar4 = 1;
              }
            }
          }
        }
      }
    }
  }
  if (bVar4 == 0) {
    return 0;
  }
  iVar1 = fn_8012640C(r3,0,0xd2,0);
  if ((s32)iVar1 == 1) {
    return 0;
  }
  if (r3 == 0) {
    iVar1 = 0;
  }
  else {
    iVar1 = fn_8012640C(r3,0,0xcc,0);
  }
  if (iVar1 == 0) {
    return 0;
  }
  return (u8)fn_801233F4(iVar1) != 0;
}
#pragma pop
/* Address: 0x80206780 | Size: 0x148 | Ghidra import */
#pragma push
#pragma peephole on
u8 fn_80206780(void* p1) {
    extern u8 fn_80123FBC();
    extern u16 fn_801EF634();
    u16 sVar3;
    void* iVar1;
    void* iVar2;
    u8 cVar4;
    u8 uVar5;

    if (p1 == 0) {
        uVar5 = 0;
    } else {
        sVar3 = fn_801EF634();
        if (sVar3 == 1) {
            uVar5 = 0;
        } else {
            iVar1 = fn_8012640C(p1, 0, 0xd6, 0);
            if (iVar1 == 0) {
                uVar5 = 0;
            } else {
                sVar3 = fn_801EF634();
                if (sVar3 == 1) {
                    uVar5 = 0;
                } else {
                    iVar2 = fn_8012640C(iVar1, 0, 0xcb, 0);
                    if (iVar2 == 0) {
                        uVar5 = 0;
                    } else {
                        cVar4 = fn_80123FBC();
                        if (cVar4 == 0) {
                            uVar5 = 0;
                        } else {
                            if (iVar1 == 0) {
                                iVar2 = 0;
                            } else {
                                iVar2 = fn_8012640C(iVar1, 0, 0xcc, 0);
                            }
                            if (iVar2 == 0) {
                                uVar5 = 0;
                            } else {
                                cVar4 = fn_80123FBC();
                                if (cVar4 == 0) {
                                    uVar5 = 0;
                                } else {
                                    iVar1 = fn_8012640C(iVar1, 0, 0xce, 0);
                                    if ((s32)iVar1 < 0) {
                                        uVar5 = 0;
                                    } else {
                                        uVar5 = 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return uVar5;
}
#pragma pop
/* Address: 0x802068C8 | Size: 0x13c | Ghidra import */

void fn_802068C8(int r3,int r4,int r5)

{
    extern u32 fn_801248C4();
    extern void fn_801F198C();
    extern void fn_80206C94();
  u32 uVar1;
  u16 uVar2;
  u16 uVar3;
  u32 uVar4;
  
  if ((r3 != 0) && (r4 != 0)) {
    uVar1 = (int)fn_8012640C(r4,0,0xcc,0);
    fn_80206C94(r3);
    fn_801254B4(r3,0,0xd5,0,r4);
    fn_801254B4(r3,0,0xd6,0,r4);
    if (r5 != 0) {
      fn_801254B4(r3,0,0xee,0,r5);
      fn_801F198C();
    }
    uVar2 = (int)fn_8012640C(uVar1,0,0x6e,0);
    for (uVar4 = 0; (uVar4 & 0xffff) < 2; uVar4 = uVar4 + 1) {
      uVar3 = (int)fn_8012640C(0,uVar2,0x16,uVar4);
      fn_801254B4(r3,0,0xff,uVar4 & 0xff,uVar3);
    }
    uVar2 = fn_801248C4(uVar1);
    fn_801254B4(r3,0,0x100,0,uVar2);
  }
  return;
}
/* Address: 0x80206A04 | Size: 0xe8 | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_80206A04(void* ctx) {
    extern u8 fn_80123FBC();
    extern u16 fn_801EF634();
    u32 uVar1;
    u16 sVar3;
    void* iVar2;
    u8 cVar4;
    if (ctx == 0) { return 0; }
    sVar3 = fn_801EF634();
    if (sVar3 == 1) { return 0; }
    iVar2 = fn_8012640C(ctx, 0, 0xcb, 0);
    if (iVar2 == 0) { return 0; }
    cVar4 = fn_80123FBC();
    if (cVar4 == 0) { return 0; }
    if (ctx == 0) {
        iVar2 = 0;
    } else {
        iVar2 = fn_8012640C(ctx, 0, 0xcc, 0);
    }
    if (iVar2 == 0) { return 0; }
    cVar4 = fn_80123FBC();
    if (cVar4 == 0) { return 0; }
    uVar1 = (u32)fn_8012640C(ctx, 0, 0xce, 0);
    return uVar1 >> 0x1f ^ 1;
}
#pragma pop
/* Address: 0x80206AEC | Size: 0x150 | Ghidra import */
#pragma push
#pragma peephole on
void fn_80206AEC(void* p1, void* p2, s16 p3) {
    extern void fn_8011B950();
    extern void fn_8011F5FC();
    extern void fn_80124A60();
    u32 uVar1;

    if (p1 != 0 && p2 != 0) {
        if (p1 != 0) {
            fn_801254B4(p1, 0, 0xcb, 0, 0);
            fn_8012640C(p1, 0, 0xcc, 0);
            fn_80124A60();
            uVar1 = (u32)fn_8012640C(p1, 0, 0xcd, 0);
            fn_8011B950(uVar1, 1);
            fn_801254B4(p1, 0, 0xce, 0, (void*)0xffffffff);
            fn_801254B4(p1, 0, 0xcf, 0, 0);
            fn_801254B4(p1, 0, 0xd0, 0, 0);
            fn_801254B4(p1, 0, 0xd1, 0, 0);
            fn_801254B4(p1, 0, 0xd2, 0, 0);
        }
        fn_801254B4(p1, 0, 0xcb, 0, p2);
        uVar1 = (u32)fn_8012640C(p1, 0, 0xcc, 0);
        fn_8011F5FC(uVar1, p2);
        fn_801254B4(p1, 0, 0xce, 0, (s32)p3);
    }
}
#pragma pop
/* 0x80206C3C | size: 0x58 | small */
#pragma push
#pragma peephole on
void fn_80206C3C(u32 param_1, u16 param_2) {
    extern void fn_80206C94(u32);
    u32 n;
    int iVar1;

    if (param_1 != 0) {
        iVar1 = 0;
        n = param_2;
        for (; (u16)iVar1 < n; iVar1 = iVar1 + 1) {
            fn_80206C94(param_1 + (u32)(u16)iVar1 * 0x6E0);
        }
    }
}
#pragma pop

/* Address: 0x80206C94 | Size: 0x72c | Ghidra import */
#pragma push
#pragma peephole on
#pragma optimization_level 1
void fn_80206C94(int r3)

{
    extern u32 _DAT_80279c60;
    extern u32 _DAT_80279c64;
    extern u32 _DAT_80279c68;
    extern u32 _DAT_80279c6c;
    extern void fn_8011B950();
    extern void fn_80124A60();
    extern void fightActionInit();
    extern void fn_801FD830();
    extern void fn_80209D90();
    extern void fn_8020A478();
    extern void fn_8020E6D4();
  int iVar1;
  u32 uVar2;
  u32 *puVar3;
  u8 bVar4;
  u32 local_28;
  u32 local_24;
  u32 local_20;
  u16 local_1c;
  
  if (r3 != 0) {
    fn_801254B4(r3,0,0xd5,0,0);
    fn_801254B4(r3,0,0xd6,0,0);
    iVar1 = (int)fn_8012640C(r3,0,0xd7,0);
    if (iVar1 != 0) {
      fn_801254B4(iVar1,0,0xcb,0,0);
      fn_8012640C(iVar1,0,0xcc,0);
      fn_80124A60();
      uVar2 = (int)fn_8012640C(iVar1,0,0xcd,0);
      fn_8011B950(uVar2,1);
      fn_801254B4(iVar1,0,0xce,0, (void*)0xffffffff);
      fn_801254B4(iVar1,0,0xcf,0,0);
      fn_801254B4(iVar1,0,0xd0,0,0);
      fn_801254B4(iVar1,0,0xd1,0,0);
      fn_801254B4(iVar1,0,0xd2,0,0);
    }
    uVar2 = (int)fn_8012640C(r3,0,0xd8,0);
    fn_8011B950(uVar2,0x34);
    local_28 = _DAT_80279c60;
    local_24 = _DAT_80279c64;
    local_20 = _DAT_80279c68;
    local_1c = _DAT_80279c6c;
    for (bVar4 = 0; bVar4 < 7; bVar4 = bVar4 + 1) {
      fn_801254B4(r3,0,*(u16 *)((int)&local_28 + (u32)bVar4 * 2),0,6);
    }
    fn_801FD830(r3,0);
    fn_801254B4(r3,0,0xed,0,2);
    fn_801254B4(r3,0,0xee,0,0);
    for (bVar4 = 0; bVar4 < 0xc; bVar4 = bVar4 + 1) {
      fn_801254B4(r3,0,0xfd,bVar4, (void*)0xffffffff);
    }
    iVar1 = (int)fn_8012640C(r3,0,0xfe,0);
    if (iVar1 != 0) {
      fightActionInit();
      fn_8012640C(r3,0,0xd9,0);
      fn_80209D90();
      fn_8012640C(r3,0,0xe5,0);
      fn_8020A478();
    }
    fn_8012640C(r3,0,0xf8,0);
    fn_80209D90();
    for (bVar4 = 0; bVar4 < 2; bVar4 = bVar4 + 1) {
      fn_801254B4(r3,0,0xff,bVar4,9);
    }
    fn_801254B4(r3,0,0x100,0,0);
    puVar3 = (u32 *)fn_8012640C(r3,0,0x101,0);
    if (puVar3 != (void *)0) {
      *puVar3 = 0;
    }
    fn_801254B4(r3,0,0xef,0,0);
    fn_801254B4(r3,0,0xf0,0,0);
    fn_801254B4(r3,0,0xf1,0,0);
    fn_801254B4(r3,0,0xf2,0,0);
    fn_801254B4(r3,0,0xf3,0,0);
    fn_801254B4(r3,0,0xf4,0,9);
    fn_801254B4(r3,0,0xf5,0,0);
    fn_801254B4(r3,0,0xf6,0,0);
    fn_801254B4(r3,0,0xf7,0,0);
    fn_801254B4(r3,0,0xf9,0,0);
    fn_801254B4(r3,0,0xfc,0,0);
    fn_801254B4(r3,0,0xfb,0,0);
    fn_801254B4(r3,0,0x102,0,0);
    fn_801254B4(r3,0,0x103,0,0);
    fn_801254B4(r3,0,0x104,0,0);
    fn_801254B4(r3,0,0x105,0,0);
    fn_801254B4(r3,0,0x106,0,0);
    fn_801254B4(r3,0,0x107,0,0);
    fn_801254B4(r3,0,0x108,0,0);
    fn_801254B4(r3,0,0x109,0,0);
    fn_801254B4(r3,0,0x10a,0,0);
    fn_801254B4(r3,0,0x10b,0,0);
    fn_801254B4(r3,0,0x10c,0,0);
    fn_801254B4(r3,0,0x10d,0,0);
    fn_801254B4(r3,0,0x10e,0,0);
    fn_801254B4(r3,0,0x10f,0,0);
    fn_801254B4(r3,0,0x110,0,0);
    fn_801254B4(r3,0,0x111,0,0);
    fn_801254B4(r3,0,0x112,0,0);
    fn_801254B4(r3,0,0x113,0,0);
    fn_801254B4(r3,0,0x114,0,0);
    fn_801254B4(r3,0,0x115,0,0);
    fn_801254B4(r3,0,0x116,0,0);
    fn_801254B4(r3,0,0x117,0,0);
    fn_801254B4(r3,0,0x118,0,0);
    fn_801254B4(r3,0,0x119,0,0);
    fn_801254B4(r3,0,0x11a,0,0);
    fn_801254B4(r3,0,0x11b,0,0);
    fn_801254B4(r3,0,0x11c,0,0);
    fn_801254B4(r3,0,0x11d,0,0);
    fn_801254B4(r3,0,0x11e,0,0);
    fn_801254B4(r3,0,0x11f,0,0);
    fn_801254B4(r3,0,0x120,0,0);
    fn_801254B4(r3,0,0x121,0, (void*)0xffffffff);
    uVar2 = (int)fn_8012640C(r3,0,0x122,0);
    fn_8020E6D4(uVar2,4);
  }
  return;
}
#pragma pop
/* Address: 0x802073C0 | Size: 0x88 | Ghidra import */

void fn_802073C0(u32 r3)

{
    extern u32 _DAT_80279c60;
    extern u32 _DAT_80279c64;
    extern u32 _DAT_80279c68;
    extern u32 _DAT_80279c6c;
  u8 bVar1;
  u32 local_28;
  u32 local_24;
  u32 local_20;
  u16 local_1c;
  
  local_28 = _DAT_80279c60;
  local_24 = _DAT_80279c64;
  local_20 = _DAT_80279c68;
  local_1c = _DAT_80279c6c;
  for (bVar1 = 0; bVar1 < 7; bVar1 = bVar1 + 1) {
    fn_801254B4(r3,0,*(u16 *)((int)&local_28 + (u32)bVar1 * 2),0,6);
  }
  return;
}
/* 0x80207448 | size: 0x15C | medium */
#pragma push
#pragma peephole on
void fn_80207448(void* param_1) {
    fn_801254B4(param_1, 0, 0x113, 0, 0);
    fn_801254B4(param_1, 0, 0x114, 0, 0);
    fn_801254B4(param_1, 0, 0x115, 0, 0);
    fn_801254B4(param_1, 0, 0x116, 0, 0);
    fn_801254B4(param_1, 0, 0x117, 0, 0);
    fn_801254B4(param_1, 0, 0x118, 0, 0);
    fn_801254B4(param_1, 0, 0x119, 0, 0);
    fn_801254B4(param_1, 0, 0x11A, 0, 0);
    fn_801254B4(param_1, 0, 0x11B, 0, 0);
    fn_801254B4(param_1, 0, 0x11C, 0, 0);
    fn_801254B4(param_1, 0, 0x11D, 0, 0);
    fn_801254B4(param_1, 0, 0x11E, 0, 0);
    fn_801254B4(param_1, 0, 0x11F, 0, 0);
}
#pragma pop

/* 0x802075A4 | size: 0x1BC | medium */
#pragma push
#pragma peephole on
void fn_802075A4(void* param_1) {
    fn_801254B4(param_1, 0, 0x102, 0, 0);
    fn_801254B4(param_1, 0, 0x103, 0, 0);
    fn_801254B4(param_1, 0, 0x104, 0, 0);
    fn_801254B4(param_1, 0, 0x105, 0, 0);
    fn_801254B4(param_1, 0, 0x106, 0, 0);
    fn_801254B4(param_1, 0, 0x107, 0, 0);
    fn_801254B4(param_1, 0, 0x108, 0, 0);
    fn_801254B4(param_1, 0, 0x109, 0, 0);
    fn_801254B4(param_1, 0, 0x10A, 0, 0);
    fn_801254B4(param_1, 0, 0x10B, 0, 0);
    fn_801254B4(param_1, 0, 0x10C, 0, 0);
    fn_801254B4(param_1, 0, 0x10D, 0, 0);
    fn_801254B4(param_1, 0, 0x10E, 0, 0);
    fn_801254B4(param_1, 0, 0x10F, 0, 0);
    fn_801254B4(param_1, 0, 0x110, 0, 0);
    fn_801254B4(param_1, 0, 0x111, 0, 0);
    fn_801254B4(param_1, 0, 0x112, 0, 0);
}
#pragma pop

/* 0x80207760 | size: 0x74 | small */
void fn_80207760(void* param_1) {
    extern void fightActionInit(void*);
    extern void fn_80209D90(void*);
    extern void fn_8020A478(void*);
    void* iVar1;

    iVar1 = fn_8012640C(param_1, 0, 0xFE, 0);
    if (iVar1 != NULL) {
        fightActionInit(iVar1);
        fn_8012640C(param_1, 0, 0xD9, 0);
        fn_80209D90(iVar1);
        fn_8012640C(param_1, 0, 0xE5, 0);
        fn_8020A478(iVar1);
    }
}

/* 0x802077D4 | size: 0x11C */
#pragma push
#pragma peephole on
void fn_802077D4(void* basePtr, u16 count) {
    extern void fn_8011B950();
    extern void fn_80124A60();
    void* entry;
    u16 i;

    if (basePtr == NULL) { return; }
    for (i = 0; i < count; i++) {
        entry = (void*)((u32)basePtr + i * 0x154);
        if (entry == NULL) { continue; }
        fn_801254B4(entry, 0, 0xCB, 0, 0);
        fn_80124A60(fn_8012640C(entry, 0, 0xCC, 0));
        fn_8011B950(fn_8012640C(entry, 0, 0xCD, 0), 1);
        fn_801254B4(entry, 0, 0xCE, 0, (u32)-1);
        fn_801254B4(entry, 0, 0xCF, 0, 0);
        fn_801254B4(entry, 0, 0xD0, 0, 0);
        fn_801254B4(entry, 0, 0xD1, 0, 0);
        fn_801254B4(entry, 0, 0xD2, 0, 0);
    }
}
#pragma pop

/* Address: 0x802078F0 | Size: 0xec | Ghidra import */
#pragma push
#pragma peephole on
void fn_802078F0(void* r3)
{
    extern void fn_8011B950();
    extern void fn_80124A60();
    void* ctx;
    u32 uVar1;

    if ((ctx = r3) != NULL) {
        fn_801254B4(ctx, 0, 0xcb, 0, 0);
        fn_8012640C(ctx, 0, 0xcc, 0);
        fn_80124A60();
        uVar1 = (u32)fn_8012640C(ctx, 0, 0xcd, 0);
        fn_8011B950(uVar1, 1);
        fn_801254B4(ctx, 0, 0xce, 0, (void*)0xffffffff);
        fn_801254B4(ctx, 0, 0xcf, 0, 0);
        fn_801254B4(ctx, 0, 0xd0, 0, 0);
        fn_801254B4(ctx, 0, 0xd1, 0, 0);
        fn_801254B4(ctx, 0, 0xd2, 0, 0);
    }
}
#pragma pop
/* 0x802079DC | size: 0x104 */
#pragma push
#pragma peephole on
u32 fn_802079DC(void* ctx, void* battleCtx, u32* outSlots) {
    extern u16 fn_8010C74C(void*, u16);
    u16 i;
    int outCount;
    u16 slot0;
    u16 slot1;
    u16 result;
    u8 isPlayerSlot;

    for (i = 0; i < 0x12; i++) {
        outSlots[i] = (u32)-1;
    }
    outCount = 0;
    for (i = 0; i < 0x12; i++) {
        if (i == (u16)(u32)fn_8012640C(ctx, 0, 0xFF, 0)) {
            goto _set1;
        }
        if (i == (u16)(u32)fn_8012640C(ctx, 0, 0xFF, 1)) {
        _set1:
            isPlayerSlot = 1;
        } else {
            isPlayerSlot = 0;
        }
        if (isPlayerSlot == 1) { continue; }
        result = fn_8010C74C(battleCtx, i);
        if (result == 0x42 || result == 0x43) {
            outSlots[(u16)outCount] = i;
            outCount++;
        }
    }
    return outCount;
}
#pragma pop

/* Address: 0x80207AE0 | Size: 0x7c | Ghidra import */
u32 fn_80207AE0(void)

{
    u32 r3;
    short r4;

  short sVar2;
  u32 uVar1;
  
  sVar2 = (int)fn_8012640C(r3,0,0xff,0);
  if ((r4 == sVar2) || (sVar2 = (int)fn_8012640C(r3,0,0xff,1), r4 == sVar2)) {
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}
/* 0x80207B5C | size: 0x30 */
#pragma scheduling on
#pragma peephole on
u32 fn_80207B5C(void* context, u8 flags, u16 value) {
    return fn_801254B4(context, 0, 0xFF, flags, value);
}
#pragma peephole reset
#pragma scheduling reset

/* 0x80207B8C | size: 0x34 */
u16 fn_80207B8C(void* context, u8 field) {
    return (u16)(u32)fn_8012640C(context, 0, 0xFF, field);
}

/* 0x80207BC0 | size: 0x34 */
u32 fn_80207BC0(void* context, u16 value) {
    return fn_801254B4(context, 0, 0x100, 0, value);
}

/* 0x80207C24 | size: 0x48 | small */
void fn_80207C24(void* ctx, u32 param) {
    extern void fn_801DA5AC();
    void* obj = fn_8012640C(ctx, 0, 0xee, 0);
    if (obj != 0) {
        fn_801DA5AC(obj, param);
    }
}

/* Address: 0x80207C6C | Size: 0x2f0 | Ghidra import */
u32 fn_80207C6C(void)

{
    int r3;
    u16 r4;

    extern short fn_80119ED0();
    extern s8 fn_8011B67C();
    extern void fn_8011F5FC();
    extern s8 fn_80121ADC();
    extern void fn_80122040();
    extern u32 fn_80125390();
    extern u32 fn_801DE190();
    extern void fn_801FDB78();
  u32 uVar1;
  short sVar5;
  u32 uVar2;
  int iVar3;
  s8 cVar6;
  u32 uVar4;
  u8 uVar7;
  u32 local_158;
  u32 local_154;
  u8 auStack_150 [320];
  
  if (r3 == 0) {
    uVar1 = 0;
  }
  else {
    iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
    if (iVar3 == 0) {
      uVar1 = 0;
    }
    else {
      uVar1 = (int)fn_8012640C(iVar3,0,0xcc,0);
    }
  }
  fn_8011F5FC(auStack_150,uVar1);
  sVar5 = (int)fn_8012640C(auStack_150,0,0x6e,0);
  uVar2 = (int)fn_8012640C(0,sVar5,0x66,0);
  if (sVar5 != 0x181) goto LAB_00204d50;
  if (r4 != 3) {
    if (r4 < 3) {
      if (r4 == 1) {
        uVar2 = 0x19f;
        goto LAB_00204d50;
      }
      if (r4 != 0) {
        uVar2 = 0x19e;
        goto LAB_00204d50;
      }
    }
    else if (r4 < 5) {
      uVar2 = 0x1a0;
      goto LAB_00204d50;
    }
  }
  uVar2 = 0x181;
LAB_00204d50:
  sVar5 = fn_80119ED0(0x14);
  if (((sVar5 == 0x7c) || (sVar5 = fn_80119ED0(0x14), sVar5 == 200)) ||
     (sVar5 = fn_80119ED0(0x14), sVar5 == 0xcd)) {
    iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
    sVar5 = fn_80119ED0(0x14);
    if ((sVar5 == 0x7c) || (sVar5 = fn_80119ED0(0x14), sVar5 == 200)) {
      if (iVar3 == 0) {
        uVar1 = 0;
      }
      else {
        uVar1 = (int)fn_8012640C(iVar3,0,0xcc,0);
      }
      cVar6 = fn_80121ADC(uVar1,0x14);
    }
    else {
      sVar5 = fn_80119ED0(0x14);
      if (sVar5 == 0xcd) {
        cVar6 = fn_8011B67C(iVar3,0x14);
      }
      else {
        cVar6 = 0;
      }
    }
  }
  else {
    sVar5 = fn_80119ED0(0x14);
    if (sVar5 == 0xd8) {
      cVar6 = fn_8011B67C(r3,0x14);
    }
    else {
      cVar6 = 0;
    }
  }
  if (cVar6 == 1) {
    uVar2 = 0x19d;
  }
  if (uVar2 == 0) {
    uVar1 = 0;
  }
  else {
    fn_801FDB78(r3,&local_154,&local_158);
    fn_801254B4(auStack_150,0,0x6f,0,local_154);
    fn_801254B4(auStack_150,0,0x75,0,local_158);
    uVar1 = fn_80125390(auStack_150);
    uVar1 = fn_801DE190(uVar2 & 0xffff,local_154,uVar1);
    fn_80122040(auStack_150,uVar1);
    if (r3 == 0) {
      uVar4 = 0;
    }
    else {
      iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
      if (iVar3 == 0) {
        uVar4 = 0;
      }
      else {
        uVar4 = (int)fn_8012640C(iVar3,0,0xcc,0);
      }
    }
    uVar7 = (int)fn_8012640C(uVar4,0,0x73,0);
    itemGetStatus(0,uVar7,0x10,0);
  }
  return uVar1;
}
/* Address: 0x80207F5C | Size: 0xcc | Ghidra import */
u32 fn_80207F5C(void)

{
    int r3;
    u32 r4;
    int *r5;

  u32 uVar1;
  int iVar2;
  u8 uVar3;
  int iVar4;
  
  iVar4 = *r5;
  if (r3 == 0) {
    uVar1 = 0;
  }
  else {
    iVar2 = (int)fn_8012640C(r3,0,0xd6,0);
    if (iVar2 == 0) {
      uVar1 = 0;
    }
    else {
      uVar1 = (int)fn_8012640C(iVar2,0,0xcc,0);
    }
  }
  if (r3 != iVar4) {
    uVar3 = (int)fn_8012640C(uVar1,0,0x73,0);
    iVar4 = itemGetStatus(0,uVar3,0x10,0);
    if ((iVar4 != 0) && (r5[1] == iVar4)) {
      r5[2] = r5[2] + 1;
    }
  }
  return 1;
}
/* 0x80208028 | size: 0x80 | small */
#pragma push
#pragma peephole on
void fn_80208028(void* param_1) {
    void* uVar1;
    void* iVar2;
    u8 uVar3;

    if (param_1 == NULL) {
        uVar1 = NULL;
    } else {
        iVar2 = fn_8012640C(param_1, 0, 0xD6, 0);
        if (iVar2 == NULL) {
            uVar1 = NULL;
        } else {
            uVar1 = fn_8012640C(iVar2, 0, 0xCC, 0);
        }
    }
    uVar3 = (u8)(u32)fn_8012640C(uVar1, 0, 0x73, 0);
    itemGetStatus(0, uVar3, 0x10, 0);
}
#pragma pop

/* Address: 0x802080A8 | Size: 0x35c | Ghidra import */
void fn_802080A8(void)

{
    int r3;
    char r4;
    char r5;
    u32 r6;
    char r7;

    extern void _threadSwitch();
    extern void fn_80166A50();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DDD28();
    extern u32 fn_801F54A4();
    extern void fn_802624CC();
    extern void fn_802653FC();
    u32 saved_r25 = 0;
  u16 uVar4;
  int iVar1;
  u32 uVar2;
  u16 uVar5;
  s8 cVar6;
  int iVar3;

  uVar4 = fn_801F54A4(0,0,0x14,0);
  iVar1 = (int)fn_8012640C(r3,0,0xee,0);
  if (iVar1 != 0) {
    if (r7 == 0) {
      if (r4 == 1) {
        fn_801DDD28(iVar1,0xa3,4,0);
      }
      if (r5 == 1) {
        fn_801DDD28(iVar1,0x9f,4,0);
      }
      if ((r4 == 0) && (r5 == 0)) {
        fn_801DDD28(iVar1,0x57,4,0);
      }
    }
    else if (r7 == 1) {
      if (r4 == 1) {
        fn_801DA9E8(iVar1,0xa3,4);
        if (r3 == 0) {
          uVar2 = 0;
        }
        else {
          iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
          if (iVar3 == 0) {
            uVar2 = 0;
          }
          else {
            uVar2 = (int)fn_8012640C(iVar3,0,0xcc,0);
          }
        }
        uVar5 = (int)fn_8012640C(uVar2,0,0x6e,0);
        uVar5 = (int)fn_8012640C(0,uVar5,0x61,0);
        fn_80166A50(uVar5,0,0xff,0);
        fn_802624CC(r6);
        if (r5 == 0) {
          fn_802653FC(r3,uVar4,1);
        }
      }
      if (r5 == 1) {
        if (r4 == 1) {
          while (1) {
            cVar6 = fn_801DA94C(iVar1,0xa3,4);
            if (cVar6 == 0) break;
            _threadSwitch();
          }
        }
        fn_801DA9E8(iVar1,0x9f,4);
        if (r4 == 0) {
          if (r3 == 0) {
            uVar2 = 0;
          }
          else {
            iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
            if (iVar3 == 0) {
              uVar2 = 0;
            }
            else {
              uVar2 = (int)fn_8012640C(iVar3,0,0xcc,0);
            }
          }
          uVar5 = (int)fn_8012640C(uVar2,0,0x6e,0);
          uVar5 = (int)fn_8012640C(0,uVar5,0x61,0);
          fn_80166A50(uVar5,0,0xff,0);
          fn_802624CC(r6);
        }
        fn_802653FC(r3,uVar4,1);
      }
      if ((r4 == 0) && (r5 == 0)) {
        fn_801DA9E8(iVar1,0x57,4);
        fn_802624CC(r6);
      }
    }
    else if (r7 == 2) {
      if (r4 == 1) {
        saved_r25 = 0xa3;
      }
      if (r5 == 1) {
        saved_r25 = 0x9f;
      }
      if ((r4 == 0) && (r5 == 0)) {
        saved_r25 = 0x57;
      }
      while (1) {
        cVar6 = fn_801DA94C(iVar1,saved_r25,4);
        if (cVar6 == 0) break;
        _threadSwitch();
      }
    }
    else if (r7 == 3) {
      if (r4 == 1) {
        fn_801DA8C4(iVar1,0xa3,4);
      }
      if (r5 == 1) {
        fn_801DA8C4(iVar1,0x9f,4);
      }
      if ((r4 == 0) && (r5 == 0)) {
        fn_801DA8C4(iVar1,0x57,4);
      }
    }
  }
  return;
}
/* Address: 0x80208404 | Size: 0x150 | Ghidra import */
#pragma push
#pragma peephole on
void fn_80208404(void* ctx, u8 p4, u8 p5, u8 p6)
{
    extern void _threadSwitch();
    extern void fn_801DA8C4();
    extern u8 fn_801DA94C();
    extern void fn_801DA9B4();
    extern void fn_801DA9E8();
    extern void fn_801DDD28();
    extern u32 fn_801F54A4();
    extern void fn_80265598();
    void* iVar1;
    u16 uVar2;
    u8 cVar3;
    u32 uVar4;

    uVar2 = fn_801F54A4(0, 0, 0x14, 0);
    iVar1 = fn_8012640C(ctx, 0, 0xee, 0);
    if (iVar1 != 0) {
        if (p5 == 0) {
            uVar4 = 0x3a;
        } else if (p5 == 1) {
            uVar4 = 0x88;
        } else if (p5 == 2) {
            uVar4 = 0x57;
        } else {
            uVar4 = 0xd9;
        }
        if (p6 == 0) {
            fn_801DDD28(iVar1, uVar4, 4, 0);
        } else if (p6 == 1) {
            fn_801DA9E8(iVar1, uVar4, 4);
            if (p4 == 1) {
                fn_80265598(ctx, uVar2, 1);
            }
        } else if (p6 == 2) {
            while (1) {
                cVar3 = fn_801DA94C(iVar1, uVar4, 4);
                if (cVar3 == 0) break;
                _threadSwitch();
            }
        } else if (p6 == 3) {
            fn_801DA8C4(iVar1, uVar4, 4);
        } else if (p6 == 4) {
            fn_801DA9B4(iVar1, uVar4, 4);
        }
    }
}
#pragma pop
/* Address: 0x80208554 | Size: 0x70 | Ghidra import */
void fn_80208554(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;

    extern void _threadSwitch();
    extern s8 fn_801DA698();
  int iVar1;
  s8 cVar2;
  
  iVar1 = (int)fn_8012640C(r3,0,0xee,0);
  if (iVar1 != 0) {
    while (cVar2 = fn_801DA698(iVar1,r4,r5,r6), cVar2 != 1) {
      _threadSwitch();
    }
  }
  return;
}
/* Address: 0x802085C4 | Size: 0xec | Ghidra import */
#pragma push
#pragma peephole on
void fn_802085C4(u32 r3, u32 r4, u32 r5, u32 r6, int r7)

{
    extern void fn_80102568();
    extern void fn_801026A4(int, ...);
    extern u32 fn_801F54A4();
    extern void fn_801FE168();
    extern void fn_802094CC();
    extern int fn_802656AC();
  int iVar1;
  u16 uVar2;
  u8 auStack_58 [44];

  uVar2 = fn_801F54A4(0,0,0x14,0);
  iVar1 = (int)fn_8012640C(r3,0,0xee,0);
  if (iVar1 != 0) {
    fn_801FE168(r3,auStack_58);
    if (r7 >= 0) {
      r7 = fn_802656AC(r3,uVar2,1);
      fn_801026A4(r7,0,0,0,0,1,auStack_58);
    }
    fn_802094CC(iVar1,r4,r5,r6);
    if (((r6 & 0xff) == 1) && (r7 >= 0)) {
      fn_80102568(r7,0,0);
    }
  }
  return;
}
#pragma pop
/* 0x802086B0 | size: 0x38 | small */
#pragma push
#pragma peephole on
void fn_802086B0(void* ctx) {
    extern void fn_801DA83C();
    void* obj = fn_8012640C(ctx, 0, 0xee, 0);
    if (obj != 0) {
        fn_801DA83C(obj);
    }
}
#pragma pop

/* Address: 0x802086E8 | Size: 0x68 | Ghidra import */
void fn_802086E8(void)

{
    u32 r3;
    u32 r4;
    u32 r5;

    extern u32 fn_8011BEB4();
    extern void fn_801DA8C4();
  u16 uVar2;
  int iVar1;
  
  uVar2 = fn_8011BEB4(0,r4,0x1f,0);
  iVar1 = (int)fn_8012640C(r3,0,0xee,0);
  if (iVar1 != 0) {
    fn_801DA8C4(iVar1,uVar2,r5);
  }
  return;
}
/* 0x80208750 | size: 0x70 | small */
void fn_80208750(void* param_1, u32 param_2, u32 param_3, u32 param_4) {
    extern u32 fn_8011BEB4(void*, u32, u16, u32);
    extern void fn_801DDD28(void*, u16, u32, u32);
    u32 uVar2;
    void* iVar1;

    uVar2 = fn_8011BEB4(NULL, param_2, 0x1F, 0);
    iVar1 = fn_8012640C(param_1, 0, 0xEE, 0);
    if (iVar1 != NULL) {
        fn_801DDD28(iVar1, (u16)uVar2, param_3, param_4);
    }
}

/* Address: 0x802087C0 | Size: 0x458 | Ghidra import */
void fn_802087C0(void)

{
    int r3;
    u8 r4;
    u32 r5;
    char r6;
    u8 *r7;

    extern void _threadSwitch();
    extern void fn_801C3C98();
    extern void fn_801DA224();
    extern void fn_801DA2C4();
    extern u32 fn_801DA354();
    extern void fn_801DA4E8();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DB100();
    extern void fn_801DDD28();
    extern void fn_801F37B0();
    extern u32 fn_801F54A4();
    extern void fn_8026532C();
  int iVar1;
  u16 uVar4;
  u16 uVar5;
  u16 uVar6;
  u16 uVar7;
  u16 uVar8;
  u8 uVar9;
  s8 cVar10;
  u32 uVar2;
  int iVar3;
  u8 bVar11;
  int local_38;
  int local_34;
  u32 local_30;
  
  iVar1 = (int)fn_8012640C(r3,0,0xee,0);
  if (iVar1 != 0) {
    uVar4 = itemGetStatus(0,r5,0x17,0);
    uVar5 = itemGetStatus(0,r5,0x13,0);
    uVar6 = itemGetStatus(0,r5,0x16,0);
    uVar7 = itemGetStatus(0,r5,0x14,0);
    uVar8 = itemGetStatus(0,r5,0x15,0);
    if (r6 == 0) {
      fn_801DDD28(iVar1,uVar4,4,0);
      fn_801DDD28(iVar1,uVar5,4,0);
      fn_801DDD28(iVar1,uVar6,4,0);
      fn_801DDD28(iVar1,uVar7,4,0);
      fn_801DDD28(iVar1,uVar8,4,0);
      if (r7 != (void *)0) {
        uVar9 = fn_801DA354(iVar1);
        *r7 = uVar9;
        fn_801DA2C4(iVar1);
      }
    }
    else if (r6 == 1) {
      fn_801DA9E8(iVar1,uVar4,4);
      while (1) {
        cVar10 = fn_801DA94C(iVar1,uVar4,4);
        if (cVar10 == 0) break;
        _threadSwitch();
      }
      fn_801DA9E8(iVar1,uVar5,4);
      while (1) {
        cVar10 = fn_801DA94C(iVar1,uVar5,4);
        if (cVar10 == 0) break;
        _threadSwitch();
      }
      bVar11 = 0;
      do {
        fn_801DA9E8(iVar1,uVar6,4);
        while (1) {
          cVar10 = fn_801DA94C(iVar1,uVar6,4);
          if (cVar10 == 0) break;
          _threadSwitch();
        }
        bVar11 = bVar11 + 1;
      } while ((bVar11 < 3) && (bVar11 < r4));
      if (r4 < 4) {
        fn_801DA9E8(iVar1,uVar7,4);
        while (1) {
          cVar10 = fn_801DA94C(iVar1,uVar7,4);
          if (cVar10 == 0) break;
          _threadSwitch();
        }
      }
    }
    else if (r6 == 2) {
      if (r4 < 4) {
        fn_801DA9E8(iVar1,uVar8,4);
      }
    }
    else if (r6 == 3) {
      if (r4 < 4) {
        if (r7 != (void *)0) {
          fn_801DA224(iVar1,*r7);
        }
        while (1) {
          cVar10 = fn_801DA94C(iVar1,uVar8,4);
          if (cVar10 == 0) break;
          _threadSwitch();
        }
      }
    }
    else if (r6 == 4) {
      fn_801DA8C4(iVar1,uVar4,4);
      fn_801DA8C4(iVar1,uVar5,4);
      fn_801DA8C4(iVar1,uVar6,4);
      fn_801DA8C4(iVar1,uVar7,4);
      fn_801DA8C4(iVar1,uVar8,4);
      if (r4 < 4) {
        if (r7 != (void *)0) {
          fn_801DA224(iVar1,*r7);
        }
      }
      else {
        if (r3 == 0) {
          uVar2 = 0;
        }
        else {
          iVar1 = (int)fn_8012640C(r3,0,0xd6,0);
          if (iVar1 == 0) {
            uVar2 = 0;
          }
          else {
            uVar2 = (int)fn_8012640C(iVar1,0,0xcc,0);
          }
        }
        uVar9 = (int)fn_8012640C(uVar2,0,0x73,0);
        iVar1 = itemGetStatus(0,uVar9,0x10,0);
        if (iVar1 != 0) {
          local_30 = 0;
          local_38 = r3;
          local_34 = iVar1;
          fn_801F37B0(0,0x80207f5c,&local_38,0);
        }
        iVar1 = (int)fn_8012640C(r3,0,0xee,0);
        if (iVar1 != 0) {
          iVar3 = (int)fn_8012640C(r3,0,0xee,0);
          if (iVar3 != 0) {
            fn_801DA4E8(iVar3,0);
          }
          fn_801254B4(r3,0,0xee,0,0);
          fn_801C3C98(iVar1);
          fn_801DB100(iVar1);
        }
        uVar4 = fn_801F54A4(0,0,0x14,0);
        fn_8026532C(r3,uVar4,1);
      }
    }
  }
  return;
}
/* Address: 0x80208C18 | Size: 0x2b8 | Ghidra import */
void fn_80208C18(void)

{
    int r3;
    char r4;

    extern void _threadSwitch();
    extern s8 fn_80125390();
    extern void fn_80166A50();
    extern s8 fn_801DA5C4();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DDD28();
  int iVar1;
  u8 uVar6;
  u16 uVar4;
  u16 uVar5;
  s8 cVar7;
  u32 uVar2;
  int iVar3;
  u32 uVar8;
  
  iVar1 = (int)fn_8012640C(r3,0,0xee,0);
  if (iVar1 != 0) {
    if (r3 == 0) {
      uVar8 = 0;
    }
    else {
      iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
      if (iVar3 == 0) {
        uVar8 = 0;
      }
      else {
        uVar8 = (int)fn_8012640C(iVar3,0,0xcc,0);
      }
    }
    uVar6 = (int)fn_8012640C(uVar8,0,0x73,0);
    uVar4 = itemGetStatus(0,uVar6,0xe,0);
    uVar5 = itemGetStatus(0,uVar6,0xf,0);
    if (r4 == 0) {
      fn_801DDD28(iVar1,uVar4,4,0);
      fn_801DDD28(iVar1,uVar5,4,0);
      fn_801DDD28(iVar1,0x67,4,0);
    }
    else if (r4 == 1) {
      fn_801DA9E8(iVar1,uVar4,4);
    }
    else if (r4 == 2) {
      while (cVar7 = fn_801DA94C(iVar1,uVar4,4), cVar7 != 0) {
        _threadSwitch();
      }
    }
    else if (r4 == 3) {
      fn_801DA9E8(iVar1,uVar5,4);
    }
    else if (r4 == 4) {
      while (cVar7 = fn_801DA5C4(0), cVar7 != 1) {
        _threadSwitch();
      }
      if (r3 == 0) {
        uVar2 = 0;
      }
      else {
        iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
        if (iVar3 == 0) {
          uVar2 = 0;
        }
        else {
          uVar2 = (int)fn_8012640C(iVar3,0,0xcc,0);
        }
      }
      uVar4 = (int)fn_8012640C(uVar2,0,0x6e,0);
      uVar4 = (int)fn_8012640C(0,uVar4,0x61,0);
      fn_80166A50(uVar4,0,0xff,0);
      while (cVar7 = fn_801DA94C(iVar1,uVar5,4), cVar7 != 0) {
        _threadSwitch();
      }
      cVar7 = fn_80125390(uVar8);
      if (cVar7 == 1) {
        fn_801DA9E8(iVar1,0x67,4);
        while (cVar7 = fn_801DA94C(iVar1,0x67,4), cVar7 != 0) {
          _threadSwitch();
        }
      }
    }
    else if (r4 == 5) {
      fn_801DA8C4(iVar1,uVar4,4);
      fn_801DA8C4(iVar1,uVar5,4);
      fn_801DA8C4(iVar1,0x67,4);
    }
  }
  return;
}
/* Address: 0x80208ED0 | Size: 0x25c | Ghidra import */
void fn_80208ED0(void)

{
    int r3;
    char r4;

    extern void _threadSwitch();
    extern void fn_801C3C98();
    extern void fn_801DA4E8();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DB100();
    extern void fn_801DDD28();
    extern void fn_801F37B0();
    extern u32 fn_801F54A4();
    extern void fn_8026532C();
  int iVar1;
  u32 uVar2;
  u8 uVar5;
  u16 uVar4;
  s8 cVar6;
  int iVar3;
  int local_28;
  int local_24;
  u32 local_20;
  
  iVar1 = (int)fn_8012640C(r3,0,0xee,0);
  if (iVar1 != 0) {
    if (r3 == 0) {
      uVar2 = 0;
    }
    else {
      iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
      if (iVar3 == 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = (int)fn_8012640C(iVar3,0,0xcc,0);
      }
    }
    uVar5 = (int)fn_8012640C(uVar2,0,0x73,0);
    uVar4 = itemGetStatus(0,uVar5,0xd,0);
    if (r4 == 0) {
      fn_801DDD28(iVar1,uVar4,4,0);
    }
    else if (r4 == 1) {
      fn_801DA9E8(iVar1,uVar4,4);
    }
    else if (r4 == 2) {
      while (1) {
        cVar6 = fn_801DA94C(iVar1,uVar4,4);
        if (cVar6 == 0) break;
        _threadSwitch();
      }
    }
    else if (r4 == 3) {
      fn_801DA8C4(iVar1,uVar4,4);
    }
    else if (r4 == 4) {
      if (r3 == 0) {
        uVar2 = 0;
      }
      else {
        iVar1 = (int)fn_8012640C(r3,0,0xd6,0);
        if (iVar1 == 0) {
          uVar2 = 0;
        }
        else {
          uVar2 = (int)fn_8012640C(iVar1,0,0xcc,0);
        }
      }
      uVar5 = (int)fn_8012640C(uVar2,0,0x73,0);
      iVar1 = itemGetStatus(0,uVar5,0x10,0);
      if (iVar1 != 0) {
        local_20 = 0;
        local_28 = r3;
        local_24 = iVar1;
        fn_801F37B0(0,0x80207f5c,&local_28,0);
      }
      iVar1 = (int)fn_8012640C(r3,0,0xee,0);
      if (iVar1 != 0) {
        iVar3 = (int)fn_8012640C(r3,0,0xee,0);
        if (iVar3 != 0) {
          fn_801DA4E8(iVar3,0);
        }
        fn_801254B4(r3,0,0xee,0,0);
        fn_801C3C98(iVar1);
        fn_801DB100(iVar1);
      }
      uVar4 = fn_801F54A4(0,0,0x14,0);
      fn_8026532C(r3,uVar4,1);
    }
  }
  return;
}
/* Address: 0x8020912C | Size: 0x254 | Ghidra import */
void fn_8020912C(void)

{
    int r3;
    char r4;

    extern void _threadSwitch();
    extern void fn_801C3C98();
    extern void fn_801DA4E8();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DB100();
    extern void fn_801DDD28();
    extern void fn_801F37B0();
    extern u32 fn_801F54A4();
    extern void fn_8026532C();
  int iVar1;
  u32 uVar2;
  u8 uVar5;
  u16 uVar4;
  s8 cVar6;
  int iVar3;
  int local_28;
  int local_24;
  u32 local_20;
  
  iVar1 = (int)fn_8012640C(r3,0,0xee,0);
  if (iVar1 != 0) {
    if (r3 == 0) {
      uVar2 = 0;
    }
    else {
      iVar3 = (int)fn_8012640C(r3,0,0xd6,0);
      if (iVar3 == 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = (int)fn_8012640C(iVar3,0,0xcc,0);
      }
    }
    uVar5 = (int)fn_8012640C(uVar2,0,0x73,0);
    uVar4 = itemGetStatus(0,uVar5,0x10,0);
    if (r4 == 0) {
      fn_801DDD28(iVar1,uVar4,4,0);
    }
    else if (r4 == 1) {
      fn_801DA9E8(iVar1,uVar4,4);
    }
    else if (r4 == 2) {
      while (1) {
        cVar6 = fn_801DA94C(iVar1,uVar4,4);
        if (cVar6 == 0) break;
        _threadSwitch();
      }
      fn_801DA8C4(iVar1,uVar4,4);
      uVar4 = fn_801F54A4(0,0,0x14,0);
      fn_8026532C(r3,uVar4,1);
    }
    else if (r4 == 3) {
      if (r3 == 0) {
        uVar2 = 0;
      }
      else {
        iVar1 = (int)fn_8012640C(r3,0,0xd6,0);
        if (iVar1 == 0) {
          uVar2 = 0;
        }
        else {
          uVar2 = (int)fn_8012640C(iVar1,0,0xcc,0);
        }
      }
      uVar5 = (int)fn_8012640C(uVar2,0,0x73,0);
      iVar1 = itemGetStatus(0,uVar5,0x10,0);
      if (iVar1 != 0) {
        local_20 = 0;
        local_28 = r3;
        local_24 = iVar1;
        fn_801F37B0(0,0x80207f5c,&local_28,0);
      }
      iVar1 = (int)fn_8012640C(r3,0,0xee,0);
      if (iVar1 != 0) {
        iVar3 = (int)fn_8012640C(r3,0,0xee,0);
        if (iVar3 != 0) {
          fn_801DA4E8(iVar3,0);
        }
        fn_801254B4(r3,0,0xee,0,0);
        fn_801C3C98(iVar1);
        fn_801DB100(iVar1);
      }
    }
  }
  return;
}
/* 0x80209380 | size: 0x104 */
#pragma push
#pragma peephole on
void fn_80209380(void* ctx) {
    extern u8 fn_800E3D08();
    extern void fn_800E4014();
    extern void fn_801DA4E8();
    extern void* fn_801DAC3C();
    extern void fn_801F000C();
    void* eeData;
    void* resolved;
    u8 i;

    eeData = fn_8012640C(ctx, 0, 0xEE, 0);
    resolved = !eeData ? NULL : fn_801DAC3C(eeData);
    if (resolved == NULL) { return; }
    if ((u8)fn_800E3D08(resolved) == 0) { return; }
    for (i = 0; i < 8; i++) {
        eeData = fn_8012640C(ctx, 0, 0xEE, 0);
        if (eeData != NULL) {
            fn_801DA4E8(eeData, 1);
        }
        fn_800E4014(resolved, 1);
        fn_801F000C(3);
        eeData = fn_8012640C(ctx, 0, 0xEE, 0);
        if (eeData != NULL) {
            fn_801DA4E8(eeData, 0);
        }
        fn_801F000C(2);
    }
    eeData = fn_8012640C(ctx, 0, 0xEE, 0);
    if (eeData != NULL) {
        fn_801DA4E8(eeData, 1);
    }
}
#pragma pop

/* 0x80209484 | size: 0x48 | small */
#pragma push
#pragma peephole on
void fn_80209484(void* ctx, u32 param) {
    extern void fn_801DA4E8();
    void* obj = fn_8012640C(ctx, 0, 0xee, 0);
    if (obj != 0) {
        fn_801DA4E8(obj, param);
    }
}
#pragma pop

/* 0x802094CC | size: 0x90 | medium */
void fn_802094CC(u32 param_1, u32 param_2, u32 param_3, s8 param_4) {
    extern void _threadSwitch(void);
    extern u16 fn_8011BEB4(void*, u32, u16, u32);
    extern void fn_801DA8C4(u32, u16, u32);
    extern s8 fn_801DA94C(u32, u16, u32);
    extern void fn_801DA9E8(u32, u16, u32);
    u16 uVar1;
    s8 cVar2;

    uVar1 = fn_8011BEB4(NULL, param_2, 0x1F, 0);
    fn_801DA9E8(param_1, uVar1, param_3);
    if (param_4 == 1) {
        while (1) {
            cVar2 = fn_801DA94C(param_1, uVar1, param_3);
            if (cVar2 == 0) break;
            _threadSwitch();
        }
        fn_801DA8C4(param_1, uVar1, param_3);
    }
}

/* Address: 0x8020955C | Size: 0xbc | Ghidra import */
#pragma push
#pragma peephole on
void fn_8020955C(u32 p1, u32 p2, u32 p3, u32 p4)
{
    extern int fn_8011BEB4();
    extern void fn_80211164();
    extern u32 fn_80211168();
    void* pcVar1;
    void* pcVar2;
    u32 uVar3;

    pcVar1 = (void*)fn_8011BEB4(0, p1, 0x20, 0);
    if (pcVar1 == NULL) {
        pcVar1 = (void*)&fn_80211164;
    }
    pcVar2 = (void*)fn_8011BEB4(0, p1, 0x21, 0);
    if (pcVar2 == NULL) {
        pcVar2 = (void*)&fn_80211168;
    }
    uVar3 = ((u32 (*)(u32, u32, u32, u32))pcVar2)(p1, p2, p3, p4);
    ((void (*)(u32, u32, u32, u32, u32))pcVar1)(p1, p2, p3, p4, uVar3);
}
#pragma pop
/* Address: 0x80209618 | Size: 0xd0 | Ghidra import */

char fn_80209618(u32 r3)

{
    extern u32 _DAT_80279d08;
    extern u32 _DAT_80279d0c;
    extern u32 _DAT_80279d10;
    extern u32 _DAT_80279d14;
    extern short fn_80119ED0();
    extern s8 fn_8011B67C();
  u16 uVar1;
  u16 uVar2;
  short sVar3;
  s8 cVar4;
  u16 uVar5;
  u32 local_28;
  u32 local_24;
  u32 local_20;
  u16 local_1c;
  
  uVar2 = 0;
  local_28 = _DAT_80279d08;
  local_24 = _DAT_80279d0c;
  local_20 = _DAT_80279d10;
  local_1c = _DAT_80279d14;
  for (uVar5 = 0; uVar5 < 7; uVar5 = uVar5 + 1) {
    uVar1 = *(u16 *)((int)&local_28 + (u32)uVar5 * 2);
    sVar3 = fn_80119ED0(uVar1);
    if (sVar3 == 0x2a) {
      cVar4 = fn_8011B67C(r3,uVar1);
    }
    else {
      cVar4 = 0;
    }
    if (cVar4 == 1) {
      uVar2 = uVar2 + 1;
    }
  }
  return -((uVar2 < 2) + -1);
}
/* Address: 0x802096E8 | Size: 0xe0 | Ghidra import */
#pragma push
#pragma peephole off
u32 fn_802096E8(void* ctx)
{
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
  u16 sVar2;
  u32 uVar1;
  u8 cVar3;

  sVar2 = fn_80119ED0(0x40);
  if (sVar2 != 0x2a) {
    cVar3 = 0;
  }
  else {
    cVar3 = fn_8011B67C(ctx,0x40);
  }
  if (cVar3 == 1) {
    uVar1 = 0;
  }
  else {
    sVar2 = fn_80119ED0(0x43);
    if (sVar2 != 0x2a) {
      cVar3 = 0;
    }
    else {
      cVar3 = fn_8011B67C(ctx,0x43);
    }
    if (cVar3 == 1) {
      uVar1 = 0;
    }
    else {
      sVar2 = fn_80119ED0(0x45);
      if (sVar2 != 0x2a) {
        cVar3 = 0;
      }
      else {
        cVar3 = fn_8011B67C(ctx,0x45);
      }
      if (cVar3 == 1) {
        uVar1 = 0;
      }
      else {
        uVar1 = 1;
      }
    }
  }
  return uVar1;
}
#pragma pop
/* 0x802097C8 | size: 0x54 | small */
#pragma push
#pragma peephole on
void fn_802097C8(u32 param_1, u32 param_2, u32 param_3) {
    extern u32 fn_80119ED0(u32);
    extern void fn_8011B2C0(u32, u32, u32);
    if ((fn_80119ED0(param_2) & 0xFFFF) == 0x2A) {
        fn_8011B2C0(param_1, param_2, param_3);
    }
}
#pragma pop

/* Address: 0x8020981C | Size: 0x54 | Ghidra import */
u32 fn_8020981C(void)

{
    u32 r3;
    u32 r4;

    extern short fn_80119ED0();
    extern u32 fn_8011B444();
  short sVar2;
  u32 uVar1;
  
  sVar2 = fn_80119ED0(r4);
  if (sVar2 == 0x2a) {
    uVar1 = fn_8011B444(r3,r4);
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}
/* Address: 0x80209870 | Size: 0x9c | Ghidra import */
u32 fn_80209870(void)

{
    u32 r3;

    extern short fn_80119ED0();
    extern s8 fn_8011B67C();
  short sVar1;
  s8 cVar2;
  
  sVar1 = fn_80119ED0(0x41);
  if (sVar1 == 0x2a) {
    cVar2 = fn_8011B67C(r3,0x41);
  }
  else {
    cVar2 = 0;
  }
  if (cVar2 == 1) {
    sVar1 = fn_80119ED0(0x42);
    if (sVar1 == 0x2a) {
      cVar2 = fn_8011B67C(r3,0x42);
    }
    else {
      cVar2 = 0;
    }
    if (cVar2 == 1) {
      return 1;
    }
  }
  return 0;
}
/* 0x8020990C | size: 0x54 */
u32 fn_8020990C(void* ctx, u32 param) {
    extern u32 fn_80119ED0();
    extern u32 fn_8011B67C();
    if ((fn_80119ED0(param) & 0xFFFF) == 0x2A) {
        return fn_8011B67C(ctx, param);
    }
    return 0;
}

/* 0x80209960 | size: 0x4C | small */
void fn_80209960(void* ctx, u32 param) {
    extern u32 fn_80119ED0();
    extern void fn_8011B788();
    if ((fn_80119ED0(param) & 0xFFFF) == 0x2a) {
        fn_8011B788(ctx, param);
    }
}

/* Address: 0x802099AC | Size: 0x270 | Ghidra import */
#pragma push
#pragma peephole on
void fn_802099AC(void* p1, s8 p2, u32 p3, u16 p4, u8 p5) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    extern void fn_8011B950();
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    u16 sVar2;

    if (p1 != 0) {
        if (p1) {
            fn_8011BBD8(p1, 0, 0x26, 0, (void*)0xffffffff);
            fn_8011BBD8(p1, 0, 0x27, 0, 0);
            fn_8011BBD8(p1, 0, 0x28, 0, 0);
            fn_8011BBD8(p1, 0, 0x29, 0, 0);
            fn_8011B950(fn_8011BEB4(p1, 0, 0x2a, 0), 9);
            sVar2 = fn_80119ED0(0x3f);
            if (sVar2 == 0x2a) {
                fn_8011B2C0(p1, 0x3f, 0);
            }
            fn_8011BBD8(p1, 0, 0x2b, 0, 1);
            fn_8011BBD8(p1, 0, 0x2c, 0, 1);
            fn_8011BBD8(p1, 0, 0x2d, 0, 0);
            fn_8011BBD8(p1, 0, 0x2e, 0, 0);
            fn_8011BBD8(p1, 0, 0x2f, 0, 0);
            fn_8011BBD8(p1, 0, 0x30, 0, 9);
            fn_8011BBD8(p1, 0, 0x31, 0, 0);
            fn_8011BBD8(p1, 0, 0x32, 0, 0);
        }
        fn_8011BBD8(p1, 0, 0x26, 0, (s32)p2);
        fn_8011BBD8(p1, 0, 0x29, 0, p4);
        fn_8011BBD8(p1, 0, 0x27, 0, p3 & 0xffff);
        fn_8011BBD8(p1, 0, 0x28, 0, p3 & 0xffff);
        fn_8011BBD8(p1, 0, 0x2f, 0, (u16)fn_8011BEB4(0, p3, 7, 0));
        fn_8011BBD8(p1, 0, 0x30, 0, (u16)fn_8011BEB4(0, p3, 3, 0));
        fn_8011BBD8(p1, 0, 0x32, 0, p5);
    }
}
#pragma pop
/* Address: 0x80209C1C | Size: 0x98 | Ghidra import */
void fn_80209C1C(void)

{
    u32 r3;
    u32 r4;

    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
  u16 uVar1;
  
  fn_8011BBD8(r3,0,0x28,0,r4 & 0xffff);
  uVar1 = fn_8011BEB4(0,r4,7,0);
  fn_8011BBD8(r3,0,0x2f,0,uVar1);
  uVar1 = fn_8011BEB4(0,r4,3,0);
  fn_8011BBD8(r3,0,0x30,0,uVar1);
  return;
}
/* Address: 0x80209CB4 | Size: 0xdc | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_80209CB4(void* ctx) {
    extern s32 fn_8011BEB4(void* ctx, u32 p1, u32 p2, u32 p3);
    s32 iVar1;
    if (ctx == 0) {
        return 0;
    }
    iVar1 = fn_8011BEB4(ctx, 0, 0x27, 0);
    if (iVar1 == 0) {
        return 0;
    }
    iVar1 = fn_8011BEB4(ctx, 0, 0x27, 0);
    if (iVar1 == 0x163) {
        return 0;
    }
    iVar1 = fn_8011BEB4(ctx, 0, 0x28, 0);
    if (iVar1 == 0) {
        return 0;
    }
    iVar1 = fn_8011BEB4(ctx, 0, 0x28, 0);
    if (iVar1 == 0x163) {
        return 0;
    }
    iVar1 = fn_8011BEB4(ctx, 0, 0x29, 0);
    return iVar1 != 0;
}
#pragma pop
/* 0x80209D90 | size: 0x188 */
#pragma push
#pragma peephole on
void fn_80209D90(void* r3) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    extern void fn_8011B950();
    extern void fn_8011BBD8();
    extern void* fn_8011BEB4();
    void* ctx;

    if ((ctx = r3) == NULL) { return; }
    fn_8011BBD8(ctx, 0, 0x26, 0, (u32)-1);
    fn_8011BBD8(ctx, 0, 0x27, 0, 0);
    fn_8011BBD8(ctx, 0, 0x28, 0, 0);
    fn_8011BBD8(ctx, 0, 0x29, 0, 0);
    fn_8011B950(fn_8011BEB4(ctx, 0, 0x2A, 0), 9);
    if (fn_80119ED0(0x3F) == 0x2A) {
        fn_8011B2C0(ctx, 0x3F, 0);
    }
    fn_8011BBD8(ctx, 0, 0x2B, 0, 1);
    fn_8011BBD8(ctx, 0, 0x2C, 0, 1);
    fn_8011BBD8(ctx, 0, 0x2D, 0, 0);
    fn_8011BBD8(ctx, 0, 0x2E, 0, 0);
    fn_8011BBD8(ctx, 0, 0x2F, 0, 0);
    fn_8011BBD8(ctx, 0, 0x30, 0, 9);
    fn_8011BBD8(ctx, 0, 0x31, 0, 0);
    fn_8011BBD8(ctx, 0, 0x32, 0, 0);
}
#pragma pop

/* 0x80209F18 | size: 0xa8 */
#pragma push
#pragma peephole on
void fn_80209F18(void* ctx) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    extern void fn_8011B950();
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    u32 val;
    u16 typeId;
    val = fn_8011BEB4(ctx, 0, 0x2a, 0);
    fn_8011B950(val, 9);
    typeId = fn_80119ED0(0x3f);
    if (typeId == 0x2a) {
        fn_8011B2C0(ctx, 0x3f, 0);
    }
    fn_8011BBD8(ctx, 0, 0x2b, 0, 1);
    fn_8011BBD8(ctx, 0, 0x2c, 0, 1);
}
#pragma pop
/* 0x80209FAC | size: 0x64 */
#pragma push
#pragma scheduling on
#pragma peephole on
void fn_80209FAC(void* ctx) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    extern u32 fn_8011B950();
    extern u32 fn_8011BEB4();
    u32 val = fn_8011BEB4(ctx, 0, 0x2A, 0);
    fn_8011B950(val, 9);
    if (fn_80119ED0(0x3F) == 0x2A) {
        fn_8011B2C0(ctx, 0x3F, 0);
    }
}
#pragma pop

/* 0x8020A010 | size: 0x18 */
u32 fn_8020A010(u8* ptr) {
    if (ptr == NULL) { return 1; }
    return ptr[0x1];
}

/* 0x8020A028 | size: 0x18 */
u32 fn_8020A028(u8* ptr) {
    if (ptr == NULL) { return 1; }
    return ptr[0x0];
}

/* fn_8020A040 | Size: 0x28 | Look up 2-byte entry in table */
#pragma push
#pragma peephole on
u16* fn_8020A040(u16 index) {
    extern u8 lbl_80375DD0[];
    extern u32 lbl_80478D70;
    u16* result = (u16*)&lbl_80375DD0[index * 2];
    if (index < lbl_80478D70) {
        return result;
    }
    return NULL;
}
#pragma pop

/* fn_8020A080 | Size: 0x24 | Look up byte in table with bounds check */
#pragma push
#pragma peephole on
u8* fn_8020A080(u16 index) {
    extern u8 lbl_80478D58[];
    extern u32 lbl_80478D60;
    u8* result = &lbl_80478D58[index];
    if (index < lbl_80478D60) {
        return result;
    }
    return NULL;
}
#pragma pop

/* 0x8020A224 | size: 0x34 | small */
#pragma push
#pragma peephole on
void* fn_8020A224(void* base, u16 index) {
    if (base == 0) return 0;
    if (index >= 9) return 0;
    return (u8*)base + 0x8 + index * 16;
}
#pragma pop

/* 0x8020A2B8 | size: 0x40 -- copy 0xAC bytes (43 u32s) */
void fn_8020A2B8(u32* dst, u32* src) {
    struct CopyBlk8020A2B8 { u32 data[43]; };
    if (dst == 0) return;
    if (src == 0) return;
    *(struct CopyBlk8020A2B8*)dst = *(struct CopyBlk8020A2B8*)src;
}

/* Address: 0x8020A398 | Size: 0xe0 | Ghidra import */
void fn_8020A398(void)

{
    int r3;
    u16 r4;
    u16 r5;
    u32 r6;

    extern void fn_80142B24();
  if (r3 != 0) {
    fn_80142B24(r3,0,0x1e,0,0);
    fn_80142B24(r3,0,0x1f,0,0);
    fn_80142B24(r3,0,0x20,0, (void*)0xffffffff);
    fn_80142B24(r3,0,0x21,0,0);
    fn_80142B24(r3,0,0x1e,0,r4);
    fn_80142B24(r3,0,0x1f,0,r5);
    fn_80142B24(r3,0,0x20,0,r6);
  }
  return;
}
/* Address: 0x8020A478 | Size: 0x88 | Ghidra import */
#pragma push
#pragma peephole on
void fn_8020A478(void* r3)
{
    extern void fn_80142B24();
    void* ctx;
    if ((ctx = r3) != NULL) {
        fn_80142B24(ctx, 0, 0x1e, 0, 0);
        fn_80142B24(ctx, 0, 0x1f, 0, 0);
        fn_80142B24(ctx, 0, 0x20, 0, (void*)0xffffffff);
        fn_80142B24(ctx, 0, 0x21, 0, 0);
    }
}
#pragma pop
/* 0x8020A500 | size: 0x40 */
u32 fn_8020A500(u16 idx) {
    u8* entry;
    idx = idx;
    if (idx >= lbl_80478D38) {
        entry = NULL;
    } else {
        entry = lbl_80478D30 + idx * 6;
    }
    if (entry == NULL) { return 0; }
    return *(u16*)(entry + 4);
}

/* 0x8020A540 | size: 0x40 */
u32 fn_8020A540(u16 idx) {
    u8* entry;
    idx = idx;
    if (idx >= lbl_80478D38) {
        entry = NULL;
    } else {
        entry = lbl_80478D30 + idx * 6;
    }
    if (entry == NULL) { return 0; }
    return *(u16*)(entry + 2);
}

/* 0x8020A580 | size: 0x40 */
u32 fn_8020A580(u16 idx) {
    u8* entry;
    idx = idx;
    if (idx >= lbl_80478D38) {
        entry = NULL;
    } else {
        entry = lbl_80478D30 + idx * 6;
    }
    if (entry == NULL) { return 0; }
    return entry[0];
}

/* 0x8020A5C0 | size: 0x70 */
s16 fn_8020A5C0(u16 index, u16 slot) {
    extern u8 lbl_80375A08[];
    extern u32 lbl_80478D28;
    u8* entry;
    u8* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index * 0x18];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = entry + (slot * 0xA) + 0x4;
    }
    if (sub == NULL) {
        return 0;
    }
    return *(s16*)(sub + 0x4);
}

/* 0x8020A630 | size: 0x70 */
s16 fn_8020A630(u16 index, u16 slot) {
    extern u8 lbl_80375A08[];
    extern u32 lbl_80478D28;
    u8* entry;
    u8* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index * 0x18];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = entry + (slot * 0xA) + 0x4;
    }
    if (sub == NULL) {
        return 0;
    }
    return *(s16*)(sub + 0x2);
}

/* 0x8020A6A0 | size: 0x70 */
u8 fn_8020A6A0(u16 index, u16 slot) {
    extern u8 lbl_80375A08[];
    extern u32 lbl_80478D28;
    u8* entry;
    u8* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index * 0x18];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = entry + (slot * 0xA) + 0x4;
    }
    if (sub == NULL) {
        return 0;
    }
    return sub[1];
}

/* 0x8020A710 | size: 0x70 */
u16 fn_8020A710(u16 index, u16 slot) {
    extern u8 lbl_80375A08[];
    extern u32 lbl_80478D28;
    u8* entry;
    u8* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index * 0x18];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = entry + (slot * 0xA) + 0x4;
    }
    if (sub == NULL) {
        return 0;
    }
    return *(u16*)(sub + 0x8);
}

/* 0x8020A780 | size: 0x70 */
u16 fn_8020A780(u16 index, u16 slot) {
    extern u8 lbl_80375A08[];
    extern u32 lbl_80478D28;
    u8* entry;
    u8* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index * 0x18];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = entry + (slot * 0xA) + 0x4;
    }
    if (sub == NULL) {
        return 0;
    }
    return *(u16*)(sub + 0x6);
}

/* 0x8020A7F0 | size: 0x70 */
u8 fn_8020A7F0(u16 index, u16 slot) {
    extern u8 lbl_80375A08[];
    extern u32 lbl_80478D28;
    u8* entry;
    u8* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index * 0x18];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = entry + (slot * 0xA) + 0x4;
    }
    if (sub == NULL) {
        return 0;
    }
    return sub[0];
}

/* fn_8020A860 | Size: 0x40 | Look up u16 field at offset 2 in 0x18-byte table */
u16 fn_8020A860(u16 index) {
    extern u8 lbl_80375A08[];
    extern u32 lbl_80478D28;
    u8* entry;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index * 0x18];
    }
    if (entry == NULL) {
        return 0;
    }
    return *(u16*)(entry + 0x2);
}

/* fn_8020A8A0 | Size: 0x40 | Look up u8 field at offset 0 in 0x18-byte table */
u8 fn_8020A8A0(u16 index) {
    extern u8 lbl_80375A08[];
    extern u32 lbl_80478D28;
    u8* entry;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index * 0x18];
    }
    if (entry == NULL) {
        return 0;
    }
    return entry[0];
}

/* Address: 0x8020A8E0 | Size: 0x424 | Ghidra import */


int fn_8020A8E0(u32 r3,u32 r4)

{
    extern int fn_800E0C04();
    extern u32 fn_80135E44();
    extern u32 fn_801F021C();
    extern void fn_801F0234();
    extern int fn_801F02AC();
    extern u32 fn_801F54A4();
    extern u32 fn_8020A500();
    extern u32 fn_8020A540();
    extern u32 fn_8020A580();
    extern s16 fn_8020A5C0();
    extern s16 fn_8020A630();
    extern u8 fn_8020A6A0();
    extern u16 fn_8020A710();
    extern u16 fn_8020A780();
    extern u8 fn_8020A7F0();
    extern u16 fn_8020A860();
    extern u8 fn_8020A8A0();
  u32 uVar1;
  int iVar2;
  u32 uVar3;
  u8 bVar12;
  u32 uVar4;
  u32 uVar5;
  short sVar9;
  short sVar10;
  s8 cVar13;
  u16 uVar11;
  int iVar6;
  u32 uVar7;
  u32 uVar8;
  u8 bVar14;
  u32 local_48 [5];
  
  bVar14 = 0;
  do {
    if (1 < bVar14) {
      uVar1 = fn_8020A8A0(r3);
      if ((uVar1 & 0xff) < 7) {

        iVar2 = ((int (*)(void))**(void ***)((uVar1 & 0xff) * 4 + -0x7fc8a6ac))();
        return iVar2;
      }
      iVar2 = 0;
      uVar1 = fn_8020A860(r3);
      if ((uVar1 & 0xffff) == 0) {
        iVar2 = 0;
      }
      else {
        do {
          uVar3 = fn_8020A540(uVar1);
          for (bVar14 = 0; bVar14 < 2; bVar14 = bVar14 + 1) {
            uVar8 = 0;
            bVar12 = fn_8020A7F0(uVar3,bVar14);
            uVar4 = fn_8020A780(uVar3,bVar14);
            uVar5 = fn_8020A710(uVar3,bVar14);
            sVar9 = fn_8020A630(uVar3,bVar14);
            sVar10 = fn_8020A5C0(uVar3,bVar14);
            cVar13 = fn_8020A6A0(uVar3,bVar14);
            if (bVar12 == 2) {
              iVar6 = fn_800E0C04((uVar5 & 0xffff) - (uVar4 & 0xffff));
              uVar8 = (uVar4 & 0xffff) + iVar6;
LAB_00207c9c:
              if ((cVar13 == 1) && (uVar8 = uVar8 * (int)sVar9, sVar10 != 0)) {
                uVar8 = (int)uVar8 / (int)sVar10;
              }
            }
            else {
              if (bVar12 < 2) {
                if (bVar12 != 0) {
                  uVar8 = uVar4 & 0xffff;
                }
                goto LAB_00207c9c;
              }
              if (3 < bVar12) goto LAB_00207c9c;
              uVar11 = fn_801F54A4(0,0,0x14,0);
              iVar6 = fn_801F02AC(uVar4,r4,uVar11);
              if (iVar6 != 0) {
                fn_801F0234(uVar4);
                uVar7 = fn_801F021C();
                if (cVar13 == 0) {
                  uVar8 = fn_80135E44(uVar7,iVar6,sVar9,uVar5,sVar10);
                }
                else {
                  uVar8 = fn_80135E44(uVar7,iVar6,0,uVar5,0);
                }
                goto LAB_00207c9c;
              }
              uVar8 = 0;
            }
            local_48[bVar14] = uVar8;
          }
          uVar8 = fn_8020A8A0(uVar3);
          if ((uVar8 & 0xff) < 7) {

            iVar2 = ((int (*)(void))**(void ***)((uVar8 & 0xff) * 4 + -0x7fc8a6c8))();
            return iVar2;
          }
          bVar14 = fn_8020A580(uVar3);
          if (bVar14 == 2) {
            iVar2 = 0;
          }
          else if ((bVar14 < 2) && (bVar14 != 0)) {
            if (iVar2 == 0) {
              iVar2 = 0;
            }
            else {
              iVar2 = 1;
            }
          }
          uVar1 = fn_8020A500(uVar1);
        } while ((uVar1 & 0xffff) != 0);
      }
      return iVar2;
    }
    uVar1 = 0;
    bVar12 = fn_8020A7F0(r3,bVar14);
    uVar8 = fn_8020A780(r3,bVar14);
    uVar4 = fn_8020A710(r3,bVar14);
    sVar9 = fn_8020A630(r3,bVar14);
    sVar10 = fn_8020A5C0(r3,bVar14);
    cVar13 = fn_8020A6A0(r3,bVar14);
    if (bVar12 == 2) {
      iVar2 = fn_800E0C04((uVar4 & 0xffff) - (uVar8 & 0xffff));
      uVar1 = (uVar8 & 0xffff) + iVar2;
LAB_00207a34:
      if ((cVar13 == 1) && (uVar1 = uVar1 * (int)sVar9, sVar10 != 0)) {
        uVar1 = (int)uVar1 / (int)sVar10;
      }
    }
    else {
      if (bVar12 < 2) {
        if (bVar12 != 0) {
          uVar1 = uVar8 & 0xffff;
        }
        goto LAB_00207a34;
      }
      if (3 < bVar12) goto LAB_00207a34;
      uVar11 = fn_801F54A4(0,0,0x14,0);
      iVar2 = fn_801F02AC(uVar8,r4,uVar11);
      if (iVar2 != 0) {
        fn_801F0234(uVar8);
        uVar3 = fn_801F021C();
        if (cVar13 == 0) {
          uVar1 = fn_80135E44(uVar3,iVar2,sVar9,uVar4,sVar10);
        }
        else {
          uVar1 = fn_80135E44(uVar3,iVar2,0,uVar4,0);
        }
        goto LAB_00207a34;
      }
      uVar1 = 0;
    }
    uVar8 = (u32)bVar14;
    bVar14 = bVar14 + 1;
    local_48[uVar8 + 2] = uVar1;
  } while (1);
}
/* 0x8020AED0 | size: 0x60 */
#pragma push
#pragma peephole on
u32 fn_8020AED0(void* ctx) {
    extern void fn_801F4C14();
    extern u32 fn_8020D8F0();
    extern u32 fn_8020D908();
    extern void fn_80211B94();
    fn_801F4C14(0, 0, 0x36, 0, fn_8020D908(ctx));
    fn_80211B94(ctx, fn_8020D8F0(ctx), 0);
    return 1;
}
#pragma pop

/* Address: 0x8020AF30 | Size: 0xc4 | Ghidra import */
u32 fn_8020AF30(void)

{
    u32 r3;

    extern u32 fn_80136368();
    extern void fn_801F37B0();
    extern s8 fn_801F453C();
    extern void fn_801F4C14();
    extern u32 fn_8020D8F0();
    extern void fn_8020DA14();
    extern void fn_80211B94();
  s8 cVar2;
  u32 uVar1;
  u32 local_18 [4];
  
  fn_8020DA14(0,1);
  cVar2 = fn_801F453C(0,0);
  local_18[0] = 0;
  fn_801F37B0(0,0x8020aff4,local_18,0);
  if (cVar2 != 0) {
    fn_801F4C14(0,0,0x36,0,local_18[0]);
    uVar1 = fn_80136368(cVar2);
    fn_801F4C14(0,0,0x50,0,uVar1);
    uVar1 = fn_8020D8F0(r3);
    fn_80211B94(r3,uVar1,0);
  }
  return 1;
}
/* Address: 0x8020AFF4 | Size: 0x5c | Ghidra import */
u32 fn_8020AFF4(void)

{
    u32 r3;
    u32 r4;
    u32 *r5;

  int iVar1;
  u32 uVar2;
  
  iVar1 = (int)fn_8012640C(r3,0,0xee,0);
  if (iVar1 == 0) {
    uVar2 = 1;
  }
  else {
    if (r5 != (void *)0) {
      *r5 = r3;
    }
    uVar2 = 0;
  }
  return uVar2;
}
/* Address: 0x8020B058 | Size: 0x2d8 | Ghidra import */
u32 fn_8020B058(void)

{
    extern int fn_8006B0F8();
    extern s8 fn_8006B57C();
    extern s8 fn_8011FC74();
    extern s8 fn_801233F4();
    extern void fn_8012805C();
    extern u32 fn_80128A64();
    extern int fn_80129280();
    extern void fn_80129840();
    extern u32 fn_8012A5B0();
    extern void fn_8012AC64();
    extern short fn_801EF634();
    extern void fn_801EFFC4();
    extern s8 fn_801F1DBC();
    extern int fn_801F2A7C();
    extern int fn_801F47B4();
    extern u32 fn_801F54A4();
    extern int fn_801F7258();
    extern void fn_801F86C0();
    extern s8 fn_801F9034();
    extern int fn_801F9930();
    extern int fn_801FB1C0();
    extern u32 fn_80206608();
  u32 uVar1;
  short sVar6;
  int iVar2;
  int iVar3;
  u32 uVar4;
  s8 cVar7;
  int iVar5;
  u16 uVar9;
  u32 uVar8;
  u32 uVar10;
  u16 local_28 [2];
  u8 auStack_24 [8];
  
  uVar1 = fn_801F54A4(0,0,0x16,0);
  sVar6 = fn_801EF634();
  if (sVar6 != 1) {
    iVar2 = fn_801F2A7C(0);
    if ((iVar2 != 0) && (iVar3 = fn_801FB1C0(iVar2,0,0x44,0), iVar3 != 0)) {
      fn_801F86C0(iVar2,0);
      uVar4 = fn_801EF634();
      cVar7 = fn_801F1DBC(0,uVar4);
      if (cVar7 == 1) {
        cVar7 = fn_801F54A4(0,0,0x24,0);
        if ((cVar7 == 1) && (cVar7 = fn_801F9034(iVar2), cVar7 == 1)) {
          for (uVar9 = 0; uVar9 < 6; uVar9 = uVar9 + 1) {
            uVar4 = fn_8012A5B0(iVar3,3,uVar9);
            cVar7 = fn_801233F4();
            if (((((cVar7 != 0) && (cVar7 = fn_8011FC74(uVar4), cVar7 != 1)) &&
                 (iVar5 = fn_801F9930(iVar2,uVar4), iVar5 != 0)) &&
                ((cVar7 = fn_80206608(), cVar7 != 0 &&
                 (cVar7 = (int)fn_8012640C(iVar5,0,0xd0,0), cVar7 != 0)))) &&
               (uVar8 = fn_80128A64(uVar4,0,0,local_28,auStack_24), (uVar8 & 0xffff) != 0)) {
              fn_8012805C(uVar4,uVar8,local_28[0],auStack_24,iVar3,1,1,0);
              fn_801EFFC4(10);
            }
          }
        }
        cVar7 = fn_801F54A4(0,0,0x30,0);
        if (cVar7 == 1) {
          fn_80129840(iVar3);
        }
        fn_801F54A4(0,0,0x28,0);
      }
      cVar7 = fn_801F54A4(0,0,0x1c,0);
      if ((cVar7 == 1) && (iVar2 = fn_80129280(0,2), iVar2 != 0)) {
        fn_8012AC64(iVar2,iVar3);
      }
    }
    cVar7 = fn_8006B57C();
    if (cVar7 == 1) {
      for (uVar8 = 0; (uVar8 & 0xffff) < 2; uVar8 = uVar8 + 1) {
        iVar2 = fn_801F47B4(0,uVar8);
        if (iVar2 != 0) {
          for (uVar10 = 0; (uVar10 & 0xffff) < (uVar1 & 0xffff); uVar10 = uVar10 + 1) {
            iVar3 = fn_801F7258(iVar2,uVar10);
            if (iVar3 != 0) {
              fn_801F86C0(iVar3,0);
              iVar5 = fn_8006B0F8(uVar10 + (uVar8 & 0xffff) * (uVar1 & 0xffff) & 0xff);
              if ((iVar5 != 0) && (iVar3 = fn_801FB1C0(iVar3,0,0x44,0), iVar3 != 0)) {
                fn_8012AC64(iVar5);
              }
            }
          }
        }
      }
    }
  }
  return 1;
}
/* Address: 0x8020B330 | Size: 0x3a4 | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_8020B330(void* ctx)
{
    extern u32 fn_800896B8();
    extern u32 fn_800896C0();
    extern void _threadSwitch();
    extern void fn_80132A38();
    extern void fn_80165668();
    extern u32 fn_801C2D54();
    extern void fn_801DA8C4();
    extern u8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DDD28();
    extern void fn_801EF2D4();
    extern u32 fn_801EF634();
    extern void fn_801EF8F4();
    extern void fn_801F000C();
    extern u32 fn_801F025C();
    extern u8 fn_801F1DBC();
    extern u8 fn_801F54A4();
    extern u32 fn_801F8000();
    extern u32 fn_801F8100();
    extern u32 fn_801FB1C0();
    extern s16 fn_8020D814();
    extern void fn_8020D920();
    extern void fn_80211B94();
    extern u8 lbl_80378801[];
    extern u8 lbl_8037880F[];
    extern void fn_8026246C();
    extern void fn_80262490();
    extern void fn_802624CC();
    u16 sVar9;
    u32 uVar1;
    u32 uVar2;
    u32 uVar4;
    u32 iVar5;
    u16 uVar3;
    u8 cVar10;
    u32 saved_r27;
    u32 iVar7;

    fn_8020D920();
    sVar9 = fn_8020D814();
    uVar1 = fn_801F025C(0xb, 0);
    uVar2 = fn_801F025C(9, uVar1);
    uVar3 = fn_801FB1C0(uVar2, 0, 0x43, 0);
    uVar4 = fn_801FB1C0(uVar2, 0, 0x4c, 0);
    iVar5 = fn_801FB1C0(0, uVar3, 8, 1);
    if (uVar3 == fn_800896B8()) {
        iVar7 = fn_800896C0();
        if (iVar7 == 0) {
            iVar7 = 0;
        } else {
            fn_80132A38(0x24, iVar7);
            iVar7 = 0x7531;
        }
    } else {
        if ((s32)fn_801FB1C0(uVar2, 0, 0x4a, 0) == 0) {
            iVar7 = fn_801FB1C0(0, uVar3, 8, 2);
        } else {
            iVar7 = fn_801FB1C0(0, uVar3, 8, 3);
            if (iVar7 == 0) {
                iVar7 = fn_801FB1C0(0, uVar3, 8, 2);
            }
        }
    }
    fn_80132A38(0x22, fn_801F8000(uVar2));
    fn_80132A38(0x23, fn_801F8100(uVar2));
    fn_80132A38(0x13, fn_801F8100(uVar1));
    fn_80132A38(0x25, fn_801F8100(uVar2));
    cVar10 = fn_801F54A4(0, 0, 0x33, 0);
    if (cVar10 == 1) {
        if (sVar9 == 2) {
            if (iVar7 != 0) {
                fn_801DDD28(uVar4, 0x5a, 4, 0);
                saved_r27 = fn_801C2D54();
            }
            fn_80165668(0x3f5, 0, 0xff);
            fn_80132A38(0x5d, 0);
            fn_802624CC(0x766c);
            fn_8026246C();
            if (iVar7 != 0) {
                fn_801DA9E8(uVar4, 0x5a, 4);
                fn_80262490(iVar7);
                while (1) {
                    cVar10 = fn_801DA94C(uVar4, 0x5a, 4);
                    if (cVar10 == 0) break;
                    _threadSwitch();
                }
                fn_801EF8F4(saved_r27);
                fn_8026246C();
                fn_801DA8C4(uVar4, 0x5a, 4);
            }
        } else if (sVar9 == 3) {
            if (iVar5 != 0) {
                fn_801DDD28(uVar4, 0x59, 4, 0);
                saved_r27 = fn_801C2D54();
            }
            fn_802624CC(0x7547);
            fn_8026246C();
            if (iVar5 != 0) {
                fn_801DA9E8(uVar4, 0x59, 4);
                fn_80262490(iVar5);
                while (1) {
                    cVar10 = fn_801DA94C(uVar4, 0x59, 4);
                    if (cVar10 == 0) break;
                    _threadSwitch();
                }
                fn_801EF8F4(saved_r27);
                fn_8026246C();
                fn_801DA8C4(uVar4, 0x59, 4);
            }
            fn_802624CC(0x7548);
            fn_8026246C();
        } else if ((1 < (u16)(sVar9 - 4U)) && ((sVar9 == 7 || (sVar9 == 6)))) {
            fn_802624CC(0x7640);
            fn_801F000C(0x40);
            fn_8026246C();
        }
    }
    uVar1 = fn_801EF634();
    cVar10 = fn_801F1DBC(0, uVar1);
    if ((cVar10 == 1) && (cVar10 = fn_801F54A4(0, 0, 0x25, 0), cVar10 == 1)) {
        fn_80211B94(ctx, (u32)lbl_80378801, 0);
        fn_80211B94(ctx, (u32)lbl_8037880F, 0);
    }
    fn_801EF2D4();
    return 1;
}
#pragma pop
/* Address: 0x8020B6D4 | Size: 0x58 | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_8020B6D4(void)
{
    extern void fn_8016597C();
    extern void fn_801F000C();
    extern u32 fn_801F54A4();
    u32 uVar1;

    uVar1 = fn_801F54A4(0, 0, 0x12, 0);
    if (uVar1 != 0) {
        fn_8016597C(1, 1000, 1000, 0xff);
        fn_801F000C(0x3c);
    }
    return 1;
}
#pragma pop
/* Address: 0x8020B72C | Size: 0x1e4 | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_8020B72C(void* ctx)
{
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_801F00D0();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u16 fn_80205224();
    extern u32 fn_80205B8C();
    extern void fn_80209C1C();
    extern u8 fn_80209CB4();
    extern u32 fn_8020D908();
    extern void fn_802128D0();
    extern u32 fn_8022B2CC();
    u16 uVar6;
    u32 uVar1;
    u32 uVar2;
    u8 cVar8;
    u16 uVar7;
    u32 uVar3;
    u16 uVar4;
    s8 uVar9;
    u32 uVar5;

    uVar6 = fn_801F54A4(0, 0, 0x14, 0);
    uVar1 = fn_8020D908(ctx);
    uVar2 = (u32)fn_8012640C((void*)uVar1, 0, 0xd9, 0);
    cVar8 = fn_80209CB4();
    if (cVar8 == 0) {
        uVar1 = 0;
    } else {
        uVar7 = fn_8011BEB4((void*)uVar2, 0, 0x29, 0);
        uVar2 = fn_801F00D0(uVar7, uVar6);
        fn_801F4C14(0, 0, 0x36, 0, uVar1);
        fn_801F4C14(0, 0, 0x42, 0, uVar2);
        uVar2 = fn_80205B8C(uVar1);
        uVar3 = (u32)fn_8012640C((void*)uVar1, 0, 0xd9, 0);
        uVar4 = fn_80205224(uVar1);
        uVar9 = fn_8011BEB4((void*)uVar3, 0, 0x26, 0);
        cVar8 = fn_8011BEB4((void*)uVar3, 0, 0x32, 0);
        if (cVar8 == 0) {
            uVar5 = (u32)fn_8012640C((void*)uVar2, 0, 0x7f, (u8)uVar9);
            if ((uVar4 & 0xffff) != (uVar5 & 0xffff)) {
                uVar4 = (u16)(u32)fn_8012640C((void*)uVar2, 0, 0x7f, (u8)uVar9);
                fn_8011BBD8((void*)uVar3, 0, 0x27, 0, uVar4);
                fn_80209C1C((void*)uVar3, uVar4);
                uVar1 = fn_8022B2CC(uVar1, uVar4, uVar6, 0, 1, 0, (void*)0xffffffff);
                fn_801F4C14(0, 0, 0x43, 0, uVar1);
            }
        }
        fn_802128D0(ctx, uVar4);
        uVar1 = 1;
    }
    return uVar1;
}
#pragma pop
/* 0x8020B910 | size: 0x104 */
#pragma push
#pragma peephole on
u32 fn_8020B910(void* ctx) {
    extern u32 fn_801F00D0();
    extern void fn_801F4C14();
    extern u16 fn_801F54A4();
    extern u32 fn_8020D908();
    extern void fn_80211E18();
    u32 d908val;
    void* e5Data;
    u16 field1E;
    u32 partyCount;
    u8 slotType;
    u32 finalVal;
    partyCount = fn_801F54A4(0, 0, 0x14, 0);
    d908val = fn_8020D908(ctx);
    fn_801F4C14(0, 0, 0x36, 0, d908val);
    e5Data = fn_8012640C((void*)d908val, 0, 0xE5, 0);
    field1E = (u16)itemGetStatus((u32)e5Data, 0, 0x1E, 0);
    slotType = (u8)itemGetStatus(0, field1E, 0x2, 0);
    if (slotType == 1) {
        finalVal = (u32)fn_801F00D0((u16)itemGetStatus((u32)e5Data, 0, 0x1F, 0), partyCount);
    } else {
        finalVal = d908val;
    }
    fn_801F4C14(0, 0, 0x42, 0, finalVal);
    fn_80211E18(ctx, field1E);
    return 1;
}
#pragma pop

/* 0x8020BA14 | size: 0x6c */
#pragma push
#pragma peephole on
u32 fn_8020BA14(void* ctx) {
    extern void fn_801F4C14();
    extern void fn_801F54A4();
    extern u32 fn_8020D908();
    extern void fn_80212D6C();
    u32 d908val;
    fn_801F54A4(0, 0, 0x14, 0);
    d908val = fn_8020D908(ctx);
    fn_801F4C14(0, 0, 0x36, 0, d908val);
    fn_80212D6C(ctx);
    return 1;
}
#pragma pop
/* Address: 0x8020BA80 | Size: 0x78 | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_8020BA80(void* ctx)
{
    extern void fn_801F4C14();
    extern u32 fn_8020D8D8();
    extern u32 fn_8020D908();
    extern void fn_80213158();
  u32 uVar1;
  short sVar2;

  uVar1 = fn_8020D908();
  fn_801F4C14(0,0,0x45,0,uVar1);
  sVar2 = fn_8020D8D8(ctx);
  fn_801254B4(uVar1,0,0x121,0,(int)sVar2);
  fn_80213158(ctx);
  return 1;
}
#pragma pop
/* 0x8020BAF8 | size: 0xAC */
void fn_8020BAF8(void* ctx) {
    extern u8 fn_801F0058();
    extern u8 fn_801F3984();
    extern void fn_801F4C14();
    extern u16 fn_801F54A4();
    extern u32 fn_8020D908();
    extern void fn_80212840();
    u16 tableId;
    u32 obj;
    u8 result;
    tableId = fn_801F54A4(NULL, 0, 0x14, 0);
    obj = fn_8020D908(ctx);
    fn_80212840(obj);
    if (fn_801F0058(obj, tableId) == 1) {
        result = fn_801F3984(0, 4);
    } else {
        result = fn_801F3984(0, 5);
    }
    if (result == 1) {
        fn_801F4C14(0, 0, 0x44, 0, obj);
    }
}

/* Address: 0x8020BBA4 | Size: 0x58 | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_8020BBA4(void* ctx) {
    extern u16 fn_801EF634();
    extern void fn_801F000C();
    extern void fn_801F4AC0();
    extern void fn_802119D4();
    u16 sVar1;
    sVar1 = fn_801EF634();
    if (sVar1 != 0) { return 1; }
    fn_802119D4(ctx);
    fn_801F000C(5);
    fn_801F4AC0(0);
    return 1;
}
#pragma pop
/* 0x8020BBFC | size: 0x98 */
#pragma push
#pragma peephole on
u32 fn_8020BBFC(void* ctx) {
    extern u16 fn_801EF634();
    extern void fn_801F3B24();
    extern void fn_801F4718();
    extern u32 fn_8020BC94();
    extern void fn_80211A00();
    u32 uVar1;
    u16 sVar3;
    u32 uVar2;
    fn_801F4718(0);
    fn_801F3B24(0, 1);
    uVar1 = fn_8020BC94(0, 0);
    if ((u8)uVar1 != 1) { return uVar1; }
    sVar3 = fn_801EF634();
    if (sVar3 != 0) { return 1; }
    fn_80211A00(ctx);
    uVar2 = fn_8020BC94(0, 1);
    uVar1 = 1;
    if ((u8)uVar2 != 1) {
        uVar1 = uVar2;
    }
    return uVar1;
}
#pragma pop
/* Address: 0x8020BC94 | Size: 0x1a4 | Ghidra import */
u32 fn_8020BC94(void)

{
    u32 r3;
    char r4;

    extern short fn_801EF634();
    extern short fn_801F0898();
    extern void fn_801F0F04();
    extern s8 fn_801F1170();
    extern void fn_801F4AC0();
    extern int fn_801F54A4();
    extern u8 fn_802062FC();
  u32 *puVar1;
  u32 uVar2;
  int iVar3;
  s8 cVar7;
  int iVar4;
  short sVar6;
  int iVar5;
  u32 *puVar8;
  u32 *puVar9;
  u32 uVar10;
  u32 uStack_4c;
  u32 local_48 [13];
  
  uVar10 = 0;
  do {
    if (7 < (uVar10 & 0xffff)) {
      return 1;
    }
    iVar3 = fn_801F54A4(r3,0,0x59,uVar10);
    if (iVar3 != 0) {
      cVar7 = fn_802062FC();
      if (cVar7 == 0) {
        fn_801254B4(iVar3,0,0x112,0,1);
      }
      else {
        iVar4 = (int)fn_8012640C(iVar3,0,0xfe,0);
        if (iVar4 == 0) {
          fn_801254B4(iVar3,0,0x112,0,1);
        }
        else {
          cVar7 = fn_801F1170();
          if (cVar7 == 0) {
            fn_801254B4(iVar3,0,0x112,0,1);
          }
          else if (r4 == 0) {
            sVar6 = fn_801F0898(iVar4);
            if (sVar6 == 8) {
LAB_00208d8c:
              iVar5 = (int)fn_8012640C(iVar3,0,0x112,0);
              if (iVar5 != 1) {
                fn_801254B4(iVar3,0,0x112,0,1);
                puVar9 = &uStack_4c;
                puVar8 = (u32 *)(iVar4 + -4);
                iVar3 = 6;
                do {
                  puVar1 = puVar8 + 1;
                  puVar8 = puVar8 + 2;
                  uVar2 = *puVar8;
                  puVar9[1] = *puVar1;
                  puVar9 = puVar9 + 2;
                  *puVar9 = uVar2;
                  iVar3 = iVar3 + -1;
                } while (iVar3 != 0);
                fn_801F0F04(local_48);
                if (r4 != 0) {
                  fn_801F4AC0(0);
                  sVar6 = fn_801EF634();
                  if (sVar6 != 0) {
                    return 1;
                  }
                }
              }
            }
          }
          else {
            sVar6 = fn_801F0898(iVar4);
            if (sVar6 != 8) goto LAB_00208d8c;
          }
        }
      }
    }
    uVar10 = uVar10 + 1;
  } while (1);
}
/* 0x8020BE38 | size: 0x108 */
u32 fn_8020BE38(void) {
    extern u8 fn_80008174();
    extern void fn_801F2B5C();
    extern void* fn_801F47B4();
    extern u16 fn_801F54A4();
    extern void* fn_801F7258();
    extern u8 fn_8026316C();
    extern u32 fn_8020BF40();
    u8 checkResult;
    u16 partyCount;
    u16 slotCount;
    u16 i;
    u16 j;
    void* slotData;
    void* entry;

    checkResult = fn_80008174();
    if (checkResult != 1) {
        fn_801F2B5C(0, (u32)fn_8020BF40, 0, 1);
    } else {
        partyCount = fn_801F54A4(0, 0, 0x14, 0);
        slotCount = fn_801F54A4(0, 0, 0x16, 0);
        for (i = 0; i < 2; i++) {
            slotData = fn_801F47B4(0, i);
            if (slotData == NULL) { continue; }
            for (j = 0; j < slotCount; j++) {
                entry = fn_801F7258(slotData, j);
                if (entry == NULL) { continue; }
                if ((u8)fn_8026316C(entry, partyCount, checkResult) != 0) { continue; }
                if (i == 0) { continue; }
                i--;
                break;
            }
        }
    }
    return 1;
}

/* 0x8020BF40 | size: 0x60 */
#pragma push
#pragma peephole on
u32 fn_8020BF40(void* ctx, u32 param) {
    extern u16 fn_801EF634();
    extern void fn_801F150C();
    extern u8 fn_801F923C();
    if (fn_801EF634() != 0) {
        return 1;
    }
    if (fn_801F923C(ctx, param) == 0) {
        fn_801F150C(0);
    }
    return 1;
}
#pragma pop

/* 0x8020BFA0 | size: 0x120 */
#pragma push
#pragma peephole on
#pragma optimization_level 1
u32 fn_8020BFA0(void* ctx) {
    extern u8 lbl_80375CC8[];
    extern u8 lbl_80378AA0[];
    extern u16 fn_800E0C54();
    extern void fn_801DA7AC();
    extern void fn_801F2F3C();
    extern void fn_801F3074();
    extern void fn_801F3178();
    extern void fn_801F37B0();
    extern void fn_801F3B24();
    extern void fn_801F4718();
    extern void fn_801F4C14();
    extern u32 fn_8020D920();
    extern void fn_80211830();
    extern void fn_80211948();
    extern void fn_8022E1C4();
    extern void fn_8022E314();
    extern s32 fn_8020C0C0();
    extern s32 fn_8020C0E4();
    extern s32 fn_8020C108();
    u8 localBuf[0x10];

    fn_801F4718(0);
    fn_801F3B24(0, 0);
    fn_80211830();
    localBuf[0] = 0;
    fn_801F37B0(0, (u32)fn_8020C108, &localBuf[0], 0);
    fn_80211948(fn_8020D920(ctx), 0, 6, 0, lbl_80375CC8, lbl_80378AA0);
    fn_801F37B0(0, (u32)fn_8020C0E4, 0, 1);
    fn_8022E314(1);
    fn_8022E1C4();
    fn_801F37B0(0, (u32)fn_8020C0C0, 0, 1);
    localBuf[0] = 1;
    fn_801F37B0(0, (u32)fn_8020C108, &localBuf[0], 0);
    fn_801F3178(0);
    fn_801F3074(0);
    fn_801F2F3C(0);
    fn_801F4C14(0, 0, 0x5B, 0, (u32)fn_800E0C54());
    fn_801DA7AC();
    return 1;
}
#pragma pop

/* 0x8020C0C0 | size: 0x24 | small */
/* fn_8020C0C0 | Size: 0x24 | Call fn_8022D084 and return 1 */
#pragma push
#pragma peephole on
s32 fn_8020C0C0(void) {
    extern void fn_8022D084(void);
    fn_8022D084();
    return 1;
}
#pragma pop

/* 0x8020C0E4 | size: 0x24 | small */
/* fn_8020C0E4 | Size: 0x24 | Call fn_8022E410 and return 1 */
#pragma push
#pragma peephole on
s32 fn_8020C0E4(void) {
    extern void fn_8022E410(void);
    fn_8022E410();
    return 1;
}
#pragma pop

/* fn_8020C108 | Size: 0x54 | Apply effect with optional data parameter */
s32 fn_8020C108(void* ctx, u32 unused, u8* data) {
    extern void fn_8022E6F0(void* ctx, u32 value);
    if (data != NULL) {
        fn_8022E6F0(ctx, data[0]);
    } else {
        fn_8022E6F0(ctx, 0);
        fn_8022E6F0(ctx, 1);
    }
    return 1;
}

/* Address: 0x8020C15C | Size: 0x6e4 | Ghidra import */
u32 fn_8020C15C(void)

{
    extern void _threadSwitch();
    extern void fn_80103BA8();
    extern void fn_80132A38();
    extern void fn_80165A20();
    extern s8 fn_801C40F0();
    extern void fn_801C41C8();
    extern void fn_801DA4E8();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9B4();
    extern void fn_801DA9E8();
    extern void fn_801EF7C4();
    extern u32 fn_801F025C();
    extern s8 fn_801F1888();
    extern u32 fn_801F54A4();
    extern u32 fn_801F8000();
    extern u32 fn_801F8100();
    extern u32 fn_801FB1C0();
    extern u32 fn_8020DFC0();
    extern void* fn_8020E0F8();
    extern void fn_8026246C();
    extern void fn_80262490();
    extern u16 lbl_8047B5F8;
    extern f32 lbl_8047E520;
  u32 bVar1;
  u32 bVar2;

  u16 uVar12;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u16 uVar13;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  int iVar9;
  u8 cVar14;
  int iVar10;
  u32 uVar11;

  u16 local_74 [14];
  u16 local_58 [14];
  u16 local_3c [14];
  
  bVar2 = 0;
  uVar12 = fn_801F54A4(0,0,0xe,0);
  uVar3 = fn_801F025C(0xb,0);
  uVar4 = fn_801FB1C0(uVar3,0,0x4c,0);
  uVar3 = fn_801F025C(9,uVar3);
  uVar5 = fn_801FB1C0(uVar3,0,0x4c,0);
  uVar13 = fn_801FB1C0(uVar3,0,0x43,0);
  uVar6 = fn_801F54A4(0,0,0x10,0);
  uVar12 = fn_801F54A4(0,uVar12,0xd,0);
  fn_8020E0F8(uVar12);
  uVar7 = fn_8020DFC0();
  uVar8 = fn_801FB1C0(uVar3,uVar13,7,0);
  if (uVar8 == 0) {
    uVar8 = 0x5f;
  }
  iVar9 = fn_801FB1C0(0,uVar13,8,0);
  cVar14 = fn_801F1888(0);
  if (cVar14 == 0) {
    if (uVar6 != 0) {
      if (uVar7 != 0) {
        fn_801DA9E8(uVar5,uVar7 & 0xffff,4);
      }
      fn_801DA9E8(uVar5,uVar6 & 0xffff,4);
      while (1) {
        if (0) {
          bVar1 = 0;
        }
        else {
          fn_80103BA8(local_3c,1);
          cVar14 = fn_801C40F0(0);
          bVar1 = bVar2;
          if ((cVar14 == 0) && ((local_3c[0] & 0x20) != 0)) {
            bVar1 = 1;
            bVar2 = bVar1;
          }
        }
        if (bVar1) goto LAB_00209430;
        cVar14 = fn_801DA94C(uVar5,uVar6 & 0xffff,4);
        if (cVar14 == 0) break;
        _threadSwitch();
      }
      cVar14 = fn_801F54A4(0,0,0x33,0);
      if (cVar14 == 1) {
        fn_801DA9E8(uVar5,lbl_8047B5F8,4);
        while (1) {
          if (0) {
            bVar1 = 0;
          }
          else {
            fn_80103BA8(local_58,1);
            cVar14 = fn_801C40F0(0);
            bVar1 = bVar2;
            if ((cVar14 == 0) && ((local_58[0] & 0x20) != 0)) {
              bVar1 = 1;
              bVar2 = bVar1;
            }
          }
          if (bVar1) goto LAB_00209430;
          cVar14 = fn_801DA94C(uVar5,lbl_8047B5F8,4);
          if (cVar14 == 0) break;
          _threadSwitch();
        }
      }
      if (uVar7 != 0) {
        while (1) {
          if (0) {
            bVar1 = 0;
          }
          else {
            fn_80103BA8(local_74,1);
            cVar14 = fn_801C40F0(0);
            bVar1 = bVar2;
            if ((cVar14 == 0) && ((local_74[0] & 0x20) != 0)) {
              bVar1 = 1;
              bVar2 = bVar1;
            }
          }
          if ((bVar1) || (cVar14 = fn_801DA94C(uVar5,uVar7 & 0xffff,4), cVar14 == 0)) break;
          _threadSwitch();
        }
      }
    }
LAB_00209430:
    if (bVar2) {
      fn_801C41C8((double)lbl_8047E520,3);
      fn_801C40F0(1);
      if (uVar7 != 0) {
        fn_801DA9B4(uVar5,uVar7 & 0xffff,4);
      }
      fn_801DA9B4(uVar5,uVar6 & 0xffff,4);
      cVar14 = fn_801F54A4(0,0,0x33,0);
      if (cVar14 == 1) {
        fn_801DA9B4(uVar5,lbl_8047B5F8,4);
      }
    }
    if (iVar9 != 0) {
      fn_801DA9E8(uVar5,0x5f,4);
      if (bVar2) {
        fn_801C41C8((double)lbl_8047E520,2);
        bVar2 = 0;
      }
      fn_80262490(iVar9);
      while (cVar14 = fn_801DA94C(uVar5,0x5f,4), cVar14 != 0) {
        _threadSwitch();
      }
      fn_8026246C();
    }
    iVar10 = fn_801F54A4(0,0,0x11,0);
    if (iVar10 != 0) {
      fn_80165A20(iVar10,0,0xff);
    }
    cVar14 = fn_801F54A4(0,0,0x33,0);
    if (cVar14 == 1) {
      uVar4 = fn_801F8000(uVar3);
      fn_80132A38(0x22,uVar4);
      uVar3 = fn_801F8100(uVar3);
      fn_80132A38(0x23,uVar3);
      fn_801DA9E8(uVar5,uVar8 & 0xffff,4);
      if (bVar2) {
        fn_801C41C8((double)lbl_8047E520,2);
        bVar2 = 0;
      }
      fn_80262490(0x766d);
      while (cVar14 = fn_801DA94C(uVar5,uVar8 & 0xffff,4), cVar14 != 0) {
        _threadSwitch();
      }
      fn_8026246C();
    }
    if (bVar2) {
      fn_801C41C8((double)lbl_8047E520,2);
    }
    if (uVar6 != 0) {
      if (uVar7 != 0) {
        fn_801DA8C4(uVar5,uVar7 & 0xffff,4);
      }
      fn_801DA8C4(uVar5,uVar6 & 0xffff,4);
      cVar14 = fn_801F54A4(0,0,0x33,0);
      if (cVar14 == 1) {
        fn_801DA8C4(uVar5,lbl_8047B5F8,4);
      }
    }
    if (iVar9 != 0) {
      fn_801DA8C4(uVar5,0x5f,4);
    }
    cVar14 = fn_801F54A4(0,0,0x33,0);
    if (cVar14 == 1) {
      fn_801DA8C4(uVar5,uVar8 & 0xffff,4);
    }
  }
  else {
    fn_801EF7C4(0);
    fn_801DA4E8(uVar4,1);
    fn_801DA9E8(uVar4,0x54,4);
    while (cVar14 = fn_801DA94C(uVar4,0x54,4), cVar14 != 0) {
      _threadSwitch();
    }
    fn_801EF7C4(0);
    fn_801DA4E8(uVar5,1);
    fn_801DA9E8(uVar5,0x55,4);
    cVar14 = fn_801F54A4(0,0,0x33,0);
    if (cVar14 == 1) {
      uVar11 = fn_801F8000(uVar3);
      fn_80132A38(0x22,uVar11);
      uVar3 = fn_801F8100(uVar3);
      fn_80132A38(0x23,uVar3);
      fn_80262490(0x766d);
    }
    while (cVar14 = fn_801DA94C(uVar5,0x55,4), cVar14 != 0) {
      _threadSwitch();
    }
    cVar14 = fn_801F54A4(0,0,0x33,0);
    if (cVar14 == 1) {
      fn_8026246C();
    }
    fn_801EF7C4(1);
    fn_801DA9E8(uVar4,0x56,4);
    while (cVar14 = fn_801DA94C(uVar4,0x56,4), cVar14 != 0) {
      _threadSwitch();
    }
    fn_801DA8C4(uVar4,0x54,4);
    fn_801DA8C4(uVar5,0x55,4);
    fn_801DA8C4(uVar4,0x56,4);
    iVar9 = fn_801F54A4(0,0,0x11,0);
    if (iVar9 != 0) {
      fn_80165A20(iVar9,0,0xff);
    }
  }
  return 1;
}
/* Address: 0x8020CA98 | Size: 0x548 | Ghidra import */
u32 fn_8020CA98(void)

{
    extern void fn_8010AE2C();
    extern u32 fn_80121C18();
    extern void fn_80132A38();
    extern void fn_801C3430();
    extern void fn_801C3E3C();
    extern u32 fn_801F02AC();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern int fn_801F7258();
    extern u32 fn_801F7388();
    extern s8 fn_801F7404();
    extern u32 fn_801F8000();
    extern u32 fn_801F8100();
    extern int fn_801F981C();
    extern u32 fn_801F98BC();
    extern int fn_801FB1C0();
    extern void fn_801FBC20();
    extern s8 fn_80204A10();
    extern void fn_80205A7C();
    extern void fn_80205AD4();
    extern void* fn_80205BE8();
    extern u32 fn_80206608();
    extern void fn_802068C8();
    extern void fn_80208028();
    extern void fn_80208C18();
    extern void fn_8020CFE0();
    extern u32 fn_8020D814();
    extern u32 fn_8020D920();
    extern void fn_8026246C();
    extern void fn_8026532C();
    extern void fn_80265598();
    u32 saved_r26 = 0;
  u16 uVar8;
  u16 uVar9;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  s8 cVar10;
  u8 uVar11;
  int iVar4;
  u32 uVar5;
  u8 uVar12;
  u32 uVar6;
  int iVar7;
  u32 uVar13;
  u32 uVar14;
  u32 uVar15;

  fn_8020D920();
  uVar8 = fn_8020D814();
  uVar9 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F54A4(0,0,0x18,0);
  uVar1 = uVar1 & 0xffff;
  uVar2 = fn_801F54A4(0,0,0x16,0);
  uVar2 = uVar2 & 0xffff;
  uVar3 = fn_801F02AC(uVar8,0,uVar9);
  cVar10 = fn_801F7404();
  if (cVar10 == 0) {
    uVar3 = 0;
  }
  else {
    uVar11 = fn_801F7388(uVar3);
    uVar13 = 0;
    while (1) {
      if (uVar2 <= (uVar13 & 0xffff)) break;
      iVar7 = fn_801F7258(uVar3,uVar13);
      if ((iVar7 != 0) && (iVar4 = fn_801FB1C0(iVar7,0,0x4c,0), iVar4 != 0)) {
        fn_801F98BC(iVar7);
        uVar15 = 0;
        uVar14 = 0;
        while ((((uVar14 & 0xffff) < 6 && ((uVar15 & 0xffff) < uVar1)) && ((uVar15 & 0xffff) < 2)))
        {
          uVar6 = fn_801FB1C0(iVar7,0,0x45,uVar14);
          cVar10 = fn_80206608();
          if (cVar10 != 0) {
            fn_8010AE2C(uVar6,0,0);
            fn_80205BE8(uVar6);
            uVar5 = fn_80121C18();
            saved_r26 = fn_801FB1C0(iVar7,0,0x46,uVar14);
            fn_802068C8(saved_r26,uVar6,uVar5);
            uVar15 = uVar15 + 1;
            fn_80208028(saved_r26);
            cVar10 = fn_801F54A4(0,0,0x1e,0);
            if ((cVar10 == 1) && (cVar10 = fn_80204A10(saved_r26), cVar10 == 0)) {
              fn_80205AD4(saved_r26,0);
              fn_80205A7C(saved_r26,0);
            }
            fn_801C3E3C(iVar4,uVar5);
          }
          uVar14 = uVar14 + 1;
        }
      }
      uVar13 = uVar13 + 1;
    }
    uVar13 = 0;
    while (1) {
      if (uVar2 <= (uVar13 & 0xffff)) break;
      iVar7 = fn_801F7258(uVar3,uVar13);
      if (iVar7 != 0) {
        uVar14 = 0;
        while (1) {
          if ((uVar1 <= (uVar14 & 0xffff)) ||
             (saved_r26 = fn_801F981C(iVar7,uVar14), saved_r26 != 0)) break;
          uVar14 = uVar14 + 1;
        }
        fn_801FBC20(iVar7,saved_r26,0);
        uVar14 = 0;
        while (1) {
          if (uVar1 <= (uVar14 & 0xffff)) break;
          saved_r26 = fn_801F981C(iVar7,uVar14);
          if (saved_r26 != 0) {
            fn_80208C18(saved_r26,0);
          }
          uVar14 = uVar14 + 1;
        }
      }
      uVar13 = uVar13 + 1;
    }
    uVar13 = 0;
    while (1) {
      if (uVar2 <= (uVar13 & 0xffff)) break;
      iVar7 = fn_801F7258(uVar3,uVar13);
      if (iVar7 != 0) {
        uVar12 = fn_801F98BC();
        uVar14 = 0;
        while (1) {
          if (uVar1 <= (uVar14 & 0xffff)) break;
          saved_r26 = fn_801F981C(iVar7,uVar14);
          if (saved_r26 != 0) {
            fn_8020CFE0(iVar7,saved_r26,uVar11,uVar12,uVar13,uVar14,0);
          }
          uVar14 = uVar14 + 1;
        }
        uVar14 = 0;
        while (1) {
          if ((uVar1 <= (uVar14 & 0xffff)) ||
             (saved_r26 = fn_801F981C(iVar7,uVar14), saved_r26 != 0)) break;
          uVar14 = uVar14 + 1;
        }
        fn_801C3430();
        fn_801FBC20(iVar7,saved_r26,1);
        uVar6 = fn_801F8000(iVar7);
        fn_80132A38(0x22,uVar6);
        uVar6 = fn_801F8100(iVar7);
        fn_80132A38(0x23,uVar6);
        uVar6 = fn_801F8100(iVar7);
        fn_80132A38(0x25,uVar6);
        fn_8020CFE0(iVar7,saved_r26,uVar11,uVar12,uVar13,uVar14,1);
        fn_801FBC20(iVar7,saved_r26,2);
        uVar14 = 0;
        while (1) {
          if (uVar1 <= (uVar14 & 0xffff)) break;
          saved_r26 = fn_801F981C(iVar7,uVar14);
          if (saved_r26 != 0) {
            uVar6 = fn_801F54A4(0,0,0x36,0);
            fn_801F4C14(0,0,0x36,0,saved_r26);
            fn_80208C18(saved_r26,1);
            cVar10 = fn_80204A10(saved_r26);
            if (cVar10 == 0) {
              fn_80265598(saved_r26,uVar9,0);
            }
            else {
              fn_80265598(saved_r26,uVar9,1);
            }
            fn_80208C18(saved_r26,2);
            fn_80208C18(saved_r26,3);
            fn_80208C18(saved_r26,4);
            fn_8026532C(saved_r26,uVar9,0);
            fn_801F4C14(0,0,0x36,0,uVar6);
          }
          uVar14 = uVar14 + 1;
        }
      }
      uVar13 = uVar13 + 1;
    }
    fn_8026246C();
    uVar13 = 0;
    while (1) {
      if (uVar2 <= (uVar13 & 0xffff)) break;
      iVar7 = fn_801F7258(uVar3,uVar13);
      if (iVar7 != 0) {
        uVar14 = 0;
        while (1) {
          if ((uVar1 <= (uVar14 & 0xffff)) ||
             (saved_r26 = fn_801F981C(iVar7,uVar14), saved_r26 != 0)) break;
          uVar14 = uVar14 + 1;
        }
        fn_801FBC20(iVar7,saved_r26,3);
        uVar14 = 0;
        while (1) {
          if (uVar1 <= (uVar14 & 0xffff)) break;
          saved_r26 = fn_801F981C(iVar7,uVar14);
          if (saved_r26 != 0) {
            fn_80208C18(saved_r26,5);
          }
          uVar14 = uVar14 + 1;
        }
      }
      uVar13 = uVar13 + 1;
    }
    uVar3 = 1;
  }
  return uVar3;
}
/* Address: 0x8020CFE0 | Size: 0x21c | Ghidra import */
void fn_8020CFE0(void)

{
    u32 r3;
    u32 r4;
    u16 r5;
    u16 r6;
    u32 r7;
    short r8;
    char r9;

    extern void fn_80132A38();
    extern u32 fn_801F18DC();
    extern int fn_801F8000();
    extern u32 fn_80204A10();
    extern u32 fn_80205B8C();
    extern void fn_802624CC();
  u32 uVar1;
  u32 uVar2;
  int iVar3;
  u32 uVar4;
  
  uVar1 = fn_80204A10(r4);
  uVar1 = __cntlzw(1 - (uVar1 & 0xff));
  uVar1 = uVar1 >> 5;
  uVar2 = fn_801F18DC(0);
  uVar2 = __cntlzw(1 - (uVar2 & 0xff));
  uVar2 = uVar2 >> 5;
  iVar3 = fn_801F8000(r3);
  if ((iVar3 == 0) && ((uVar1 & 0xff) == 0)) {
    uVar2 = 1;
  }
  uVar4 = fn_80205B8C(r4);
  uVar4 = (int)fn_8012640C(uVar4,0,0x77,0);
  if (r9 == 0) {
    if ((uVar2 & 0xff) == 1) {
      if (r8 == 0) {
        fn_80132A38(0x14,uVar4);
        fn_80132A38(0x16,uVar4);
      }
      else {
        fn_80132A38(0x15,uVar4);
        fn_80132A38(0x17,uVar4);
      }
    }
    else if ((uVar1 & 0xff) == 1) {
      if (r8 == 0) {
        fn_80132A38(0x15,uVar4);
        fn_80132A38(0x17,uVar4);
      }
      else {
        fn_80132A38(0x14,uVar4);
        fn_80132A38(0x16,uVar4);
      }
    }
    else if (r8 == 0) {
      fn_80132A38(0x14,uVar4);
      fn_80132A38(0x16,uVar4);
    }
    else {
      fn_80132A38(0x15,uVar4);
      fn_80132A38(0x17,uVar4);
    }
  }
  else if (r9 == 1) {
    if ((r5 < 2) && (1 < r6)) {
      if ((uVar2 & 0xff) == 1) {
        uVar4 = 0x7674;
      }
      else if ((uVar1 & 0xff) == 1) {
        uVar4 = 0x7679;
      }
      else {
        uVar4 = 0x7671;
      }
    }
    else {
      fn_80132A38(0x14,uVar4);
      fn_80132A38(0x16,uVar4);
      if ((uVar2 & 0xff) == 1) {
        uVar4 = 0x7673;
      }
      else if ((uVar1 & 0xff) == 1) {
        uVar4 = 0x7678;
      }
      else {
        uVar4 = 0x7670;
      }
    }
    fn_802624CC(uVar4);
  }
  return;
}
/* Address: 0x8020D1FC | Size: 0x49c | Ghidra import */
u32 fn_8020D1FC(void)

{
    extern u32 fn_8006B0F8();
    extern s8 fn_8006B57C();
    extern s8 fn_801233F4();
    extern s8 fn_80123FBC();
    extern u32 fn_8012A5B0();
    extern void fn_8012AC64();
    extern void fn_801C3430();
    extern void fn_801C3FBC();
    extern void fn_801DA4E8();
    extern u32 fn_801F02AC();
    extern u32 fn_801F4804();
    extern u32 fn_801F54A4();
    extern int fn_801F7258();
    extern void fn_801F72B0();
    extern u32 fn_801F7388();
    extern s8 fn_801F7404();
    extern u32 fightSideGetStatus();
    extern u32 fn_801F8FD8();
    extern int fn_801F9930();
    extern void fn_801F99C8();
    extern void fn_801F9CBC();
    extern s8 fn_801FA4B4();
    extern s8 fn_801FA634();
    extern void fn_801FA6D8();
    extern u32 fn_801FB1C0();
    extern s8 fn_801FB8F8();
    extern void fn_802032E4();
    extern void fn_80206AEC();
    extern u32 fn_8020D814();
    extern u32 fn_8020D920();
    extern u32 fn_8020E020();
    extern u16 fn_8020E068();
    extern u32 fn_8020E0F8();
  u32 uVar1;
  u16 uVar14;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  s8 cVar15;
  u32 uVar7;
  u32 uVar8;
  u32 uVar9;
  u32 uVar10;
  s8 cVar16;
  int iVar11;
  u32 uVar12;
  u8 uVar17;
  int iVar13;
  int iVar18;
  u32 uVar19;
  u8 local_b58;
  u8 local_b57 [3];
  u8 auStack_b54 [2844];
  
  fn_8020D920();
  uVar1 = fn_8020D814();
  uVar14 = fn_801F54A4(0,0,0xd,0);
  uVar2 = fn_8020E0F8(uVar14);
  uVar14 = fn_801F54A4(0,0,0x14,0);
  uVar3 = fn_801F54A4(0,0,0x16,0);
  uVar3 = uVar3 & 0xffff;
  uVar4 = fn_801F54A4(0,0,0x17,0);
  uVar4 = uVar4 & 0xffff;
  uVar5 = fn_801F54A4(0,0,0x18,0);
  uVar6 = fn_801F02AC(uVar1 & 0xffff,0,uVar14);
  cVar15 = fn_801F7404();
  if (cVar15 == 0) {
    uVar2 = 0;
  }
  else {
    iVar13 = -(uVar1 & 0xffff);
    iVar18 = iVar13 + 4;
    uVar14 = fightSideGetStatus(uVar6,0,5,0);
    uVar1 = 0;
    while (1) {
      if (uVar3 <= (uVar1 & 0xffff)) break;
      uVar7 = fightSideGetStatus(uVar6,0,7,uVar1);
      uVar19 = uVar1 + (iVar18 - ((u32)(iVar18 == 0) + iVar13 + 3) & 0xffff) * uVar3 & 0xff;
      uVar8 = fn_8020E068(uVar2,uVar19);
      uVar9 = fn_8020E020(uVar2,uVar19);
      cVar15 = fn_801FA4B4(uVar8,uVar9);
      if (cVar15 != 0) {
        cVar15 = fn_8006B57C();
        if (cVar15 == 1) {
          uVar10 = fn_8006B0F8(uVar19);
          fn_8012AC64(auStack_b54,uVar10);
        }
        else {
          fn_801F9CBC(uVar8,uVar9,auStack_b54);
        }
        uVar10 = fn_801F8FD8(uVar8);
        fn_801FA6D8(uVar7,auStack_b54,uVar8,uVar9,uVar10);
        uVar8 = fn_801FB1C0(uVar7,0,0x44,0);
        cVar15 = fn_801FA634(uVar7);
        if (cVar15 != 0) {
          fn_801F99C8(uVar7,uVar4,uVar5 & 0xffff);
          cVar15 = 0;
          uVar19 = 0;
          while (((((uVar19 & 0xffff) < 6 && (iVar11 = (int)cVar15, iVar11 < (int)(uVar5 & 0xffff)))
                  && (iVar11 < (int)uVar4)) && (iVar11 < 6))) {
            uVar9 = fn_8012A5B0(uVar8,3,uVar19);
            cVar16 = fn_801233F4();
            if ((cVar16 != 0) && (iVar11 = fn_801F9930(uVar7,uVar9), iVar11 == 0)) {
              uVar10 = fn_801FB1C0(uVar7,0,0x45,(int)cVar15);
              uVar12 = fn_801F4804(0);
              fn_80206AEC(uVar10,uVar9,uVar12);
              cVar16 = fn_801F54A4(0,0,0x27,0);
              if ((cVar16 == 1) &&
                 ((cVar16 = fn_801F54A4(0,0,0x2e,0), cVar16 == 1 &&
                  (cVar16 = fn_801FB8F8(uVar7), cVar16 == 1)))) {
                fn_802032E4(uVar10,3);
              }
              cVar15 = cVar15 + 1;
            }
            uVar19 = uVar19 + 1;
          }
          uVar19 = 0;
          while ((((uVar19 & 0xffff) < 6 && ((int)cVar15 < (int)uVar4)) && (cVar15 < 6))) {
            uVar9 = fn_8012A5B0(uVar8,3,uVar19);
            cVar16 = fn_80123FBC();
            if ((cVar16 != 0) && (iVar11 = fn_801F9930(uVar7,uVar9), iVar11 == 0)) {
              uVar10 = fn_801FB1C0(uVar7,0,0x45,(int)cVar15);
              uVar12 = fn_801F4804(0);
              fn_80206AEC(uVar10,uVar9,uVar12);
              cVar16 = fn_801F54A4(0,0,0x27,0);
              if ((cVar16 == 1) &&
                 ((cVar16 = fn_801F54A4(0,0,0x2e,0), cVar16 == 1 &&
                  (cVar16 = fn_801FB8F8(uVar7), cVar16 == 1)))) {
                fn_802032E4(uVar10,3);
              }
              cVar15 = cVar15 + 1;
            }
            uVar19 = uVar19 + 1;
          }
        }
      }
      uVar1 = uVar1 + 1;
    }
    uVar17 = fn_801F7388(uVar6);
    for (uVar4 = 0; (uVar4 & 0xffff) < uVar3; uVar4 = uVar4 + 1) {
      iVar13 = fn_801F7258(uVar6,uVar4);
      if ((iVar13 != 0) && (iVar13 = fn_801FB1C0(iVar13,0,0x4c,0), iVar13 != 0)) {
        fn_801F72B0(uVar14,uVar17,uVar4,local_b57,&local_b58);
        fn_801C3FBC(iVar13,local_b57[0],local_b58);
        fn_801C3430();
        fn_801DA4E8(iVar13,1);
      }
    }
    uVar2 = 1;
  }
  return uVar2;
}
/* Address: 0x8020D698 | Size: 0xec | Ghidra import */
u32 fn_8020D698(void)

{
    extern u32 fn_801EF624();
    extern u32 fn_801F02AC();
    extern void fn_801F17B0();
    extern void fn_801F4860();
    extern u32 fn_801F54A4();
    extern void fn_801F7480();
    extern u16 fn_8020E0B0();
    extern u32 fn_8020E0F8();
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u16 uVar4;
  u16 uVar5;
  
  uVar1 = fn_801EF624();
  uVar2 = fn_8020E0F8();
  uVar3 = fn_801F54A4(0,0,0,0);
  fn_801F4860(uVar3,uVar1);
  fn_801F17B0(0);
  uVar4 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_8020E0B0(uVar2);
  uVar2 = fn_801F02AC(4,0,uVar4);
  uVar5 = fn_801F54A4(0,uVar1,3,0);
  fn_801F7480(uVar2,uVar5);
  uVar2 = fn_801F02AC(5,0,uVar4);
  uVar4 = fn_801F54A4(0,uVar1,3,1);
  fn_801F7480(uVar2,uVar4);
  return 1;
}
/* fn_8020D7CC | Size: 0x1C | Read signed byte, return -128 if NULL */
s32 fn_8020D7CC(u8* ptr) {
    if (ptr == NULL) {
        return -128;
    }
    return (s8)ptr[0];
}

/* fn_8020D7E8 | Size: 0x2C | Look up entry in 12-byte table (u16 index) */
void* fn_8020D7E8(u16 index) {
    extern u8 lbl_80375BB8[];
    extern u32 lbl_80478D48;
    if (index >= lbl_80478D48) {
        return NULL;
    }
    return &lbl_80375BB8[index * 12];
}

/* fn_8020D844 | Size: 0x24 | Store value at indexed slot (max 4) */
#pragma push
#pragma peephole on
void fn_8020D844(u8* ptr, u16 index, u32 value) {
    if (ptr == NULL) {
        return;
    }
    if (index >= 4) {
        return;
    }
    ptr += index * 4;
    *(u32*)(ptr + 0x20) = value;
}
#pragma pop

/* 0x8020D968 | size: 0x38 | small */
void fn_8020D968(void* dst, void* src) {
    struct CopyBlk8020D968 { u32 data[12]; };
    if (dst == 0) return;
    if (src == 0) return;
    *(struct CopyBlk8020D968*)dst = *(struct CopyBlk8020D968*)src;
}

/* fn_8020D9E8 | Size: 0x2C | Look up entry in 6-byte table (u16 index) */
void* fn_8020D9E8(u16 index) {
    extern u8 lbl_80375CB8[];
    extern u32 lbl_80478D50;
    if (index >= lbl_80478D50) {
        return NULL;
    }
    return &lbl_80375CB8[index * 6];
}

/* Address: 0x8020DA14 | Size: 0xbc | Ghidra import */
u32 fn_8020DA14(void)

{
    u32 r3;
    u32 r4;

    extern void fn_80136078();
    extern u32 fn_801F02AC();
    extern u32 fn_801F54A4();
    extern int fn_8020A8E0();
    extern u16 fn_8020D9A0();
    extern u16 fn_8020D9B8();
    extern u16 fn_8020D9D0();
    extern void* fn_8020D9E8();
  u32 uVar1;
  u32 uVar2;
  u16 uVar3;
  u16 uVar4;
  s8 cVar5;
  
  fn_8020D9E8(r4);
  uVar1 = fn_8020D9D0();
  fn_8020D9E8(r4);
  uVar2 = fn_8020D9B8();
  fn_8020D9E8(r4);
  uVar3 = fn_8020D9A0();
  uVar4 = fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801F02AC(uVar2,r3,uVar4);
  cVar5 = fn_8020A8E0(uVar1,r3);
  if (cVar5 == 1) {
    fn_80136078(uVar3,uVar2,r3,0);
  }
  return cVar5 == 1;
}
/* Address: 0x8020DAD0 | Size: 0x274 | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_8020DAD0(u32 p1) {
    extern void _threadSwitch();
    extern u32 fn_800FF56C();
    extern void fn_800FF730();
    extern void fn_80112700();
    extern void fn_8011288C();
    extern void fn_8011395C();
    extern void fn_80113FE8();
    extern void fn_801140C8();
    extern void fn_80129474();
    extern u32 fn_8012A5B0();
    extern void fn_80132A38();
    extern void fn_801657D0();
    extern void fn_80165A20();
    extern void fn_80166AB8();
    extern void fn_8018DA88();
    extern u8 fn_801902E0();
    extern void fn_801903B0();
    extern void fn_80190528();
    extern void fn_801C40F0();
    extern void fn_801C4164(f32, f32, u32, u32, u32);
    extern void fn_801C41C8();
    extern void fn_801D0AFC();
    extern void fn_801D23C0();
    extern void fn_801EF61C();
    extern void fn_801EF62C();
    extern u32 fn_801EF634();
    extern void fn_801EF7B4();
    extern u8 fn_801F1DBC();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u32 fn_801FCC7C();
    extern u32 fn_801FCCC4();
    extern u16 fn_8020DE50();
    extern f32 fn_8020DE68();
    extern u32 fn_8020DE80();
    extern u32 fn_8020DE98();
    extern u32 fn_8020DEB0();
    extern u8 fn_8020DFD8();
    extern void fn_8020DFF0();
    extern u16 fn_8020E068();
    extern u32 fn_8020E0B0();
    extern u8 fn_8020E0E0();
    extern u32 fn_8020E0F8();
    extern u8 fn_8020E260();
    extern u32 fn_8020E488();
    extern u16 fn_80261388();
    extern u16 charNameBiosGetHearFlag();
    extern f32 lbl_8047E528;
    extern f32 lbl_8047E52C;

    u32 uVar1;
    u32 uVar2;
    u16 uVar7;
    u8 uVar9;
    u32 iVar3;
    u8 cVar10;
    u16 sVar8;
    u32 uVar4;
    u32 uVar5;
    u32 uVar6;

    if ((p1 & 0xffff) == 0) {
        uVar1 = 0;
    } else {
        uVar1 = fn_8020E0F8();
        fn_801EF62C(0);
        fn_801903B0(0x9b0);
        fn_801EF61C(p1);
        uVar2 = fn_800FF56C();
        fn_801F4C14(0, 0, 0x4a, 0, uVar2);
        uVar2 = fn_8020E0B0(uVar1);
        uVar7 = fn_801F54A4(0, uVar2, 2, 0);
        fn_801D23C0();
        uVar9 = fn_8020E0E0(uVar1);
        iVar3 = fn_8020E488(uVar9);
        if ((iVar3 != 0) && (cVar10 = fn_8020E260(), cVar10 != 0)) {
            fn_8020E068(uVar1, 1);
            iVar3 = fn_801FCCC4();
            if (iVar3 != 0) {
                uVar2 = fn_801FCC7C();
                sVar8 = fn_80261388();
                if ((sVar8 != 0) && (sVar8 = charNameBiosGetHearFlag(), sVar8 != 0)) {
                    fn_80190528();
                }
                fn_80132A38(0x59, uVar2);
            }
        }
        fn_80165A20(1, 1000, 0xff);
        fn_801657D0(1000);
        fn_8020DFF0(uVar1);
        uVar2 = fn_8020DEB0();
        uVar4 = fn_8020DE80();
        uVar5 = fn_8020DE98(uVar2);
        fn_801C4164(lbl_8047E528, fn_8020DE68(uVar2), 9, uVar5, uVar4);
        sVar8 = fn_8020DE50(uVar2);
        if (sVar8 != 0) {
            fn_80166AB8(sVar8, 0, 0);
        }
        fn_801EF7B4();
        fn_800FF730(uVar7);
        fn_8011288C(0, 0);
        _threadSwitch();
        fn_8011395C(uVar7);
        cVar10 = fn_8020DFD8(uVar1);
        if (cVar10 != 0) {
            uVar1 = fn_801EF634();
            cVar10 = fn_801F1DBC(0, uVar1);
            if (cVar10 == 0) {
                fn_801EF61C(0);
                fn_801903B0(0xe05);
                uVar6 = fn_8012A5B0(0, 0xc, 0);
                fn_80129474(0, ((s32)uVar6 >> 1) + (((s32)uVar6 < 0) & (uVar6 & 1)));
                fn_801D0AFC(1);
                fn_8018DA88();
                fn_80113FE8();
                fn_8011288C(0, 0x5960008);
                _threadSwitch();
                uVar1 = fn_801EF634();
                return uVar1;
            }
        }
        fn_80190528(0x9b0);
        fn_80112700();
        fn_801140C8();
        cVar10 = fn_801902E0(0xe05);
        if (cVar10 == 0) {
            fn_801C41C8(lbl_8047E52C, 2);
            fn_801C40F0(1);
        }
        fn_801EF61C(0);
        uVar1 = fn_801EF634();
    }
    return uVar1;
}
#pragma pop
/* 0x8020DD44 | size: 0x3C | small */
void fn_8020DD44(void) {
    extern void fn_801F54A4();
    extern u32 fn_8020E0B0();
    extern void fn_8020E0F8();
    u32 val;
    fn_8020E0F8();
    val = fn_8020E0B0();
    fn_801F54A4(0, val, 0x7, 0);
}

/* Address: 0x8020DD80 | Size: 0xd0 | Ghidra import */
int fn_8020DD80(void)

{
    u32 r3;

    extern int fn_801F54A4();
    extern short fn_801FB1C0();
    extern int fn_801FBD10();
    extern void fn_801FBD58();
    extern u32 fn_8020E008();
    extern u16 fn_8020E068();
    extern u16 fn_8020E0B0();
    extern u32 fn_8020E0F8();
  u32 uVar1;
  int iVar2;
  u32 uVar3;
  u32 uVar4;
  short sVar5;
  u16 uVar6;
  
  uVar1 = fn_8020E0F8();
  iVar2 = fn_8020E008();
  if (iVar2 == 0) {
    fn_8020E0F8(r3);
    uVar3 = fn_8020E0B0();
    iVar2 = fn_801F54A4(0,uVar3,6,0);
    if (iVar2 == 0) {
      for (uVar6 = 0; uVar6 < 4; uVar6 = uVar6 + 1) {
        uVar4 = fn_8020E068(uVar1,uVar6 & 0xff);
        if (((uVar4 & 0xffff) != 0) && (sVar5 = fn_801FB1C0(0,uVar4,4,0), sVar5 != 0)) {
          fn_801FBD58();
          iVar2 = fn_801FBD10();
          if (iVar2 != 0) {
            return iVar2;
          }
        }
      }
      iVar2 = 1;
    }
  }
  return iVar2;
}
/* fn_8020DE68 | Size: 0x18 | Get float from ptr+4, or default if NULL */
f32 fn_8020DE68(u8* ptr) {
    extern f32 lbl_8047E530;
    if (ptr == NULL) {
        return lbl_8047E530;
    }
    return *(f32*)(ptr + 0x4);
}

/* fn_8020DEB0 | Size: 0x28 | Look up entry in 12-byte table */
void* fn_8020DEB0(u32 index) {
    extern u8 lbl_80375980[];
    extern u32 lbl_80478D20;
    if (index >= lbl_80478D20) {
        return NULL;
    }
    return &lbl_80375980[index * 12];
}

/* fn_8020DF10 | Size: 0x40 | Write u32 to slot in 8-byte array at offset 0x18 */
#pragma push
#pragma peephole on
void fn_8020DF10(u8* base, u8 slot, u32 value) {
    u8* entry;
    if (base == NULL) {
        entry = NULL;
    } else if (slot >= 4) {
        entry = NULL;
    } else {
        entry = base + slot * 8 + 0x18;
    }
    if (entry == NULL) {
        return;
    }
    *(u32*)(entry + 0x4) = value;
}
#pragma pop

/* fn_8020DF50 | Size: 0x40 | Write u16 to slot in 8-byte array at offset 0x18 */
#pragma push
#pragma peephole on
void fn_8020DF50(u8* base, u8 slot, u16 value) {
    u8* entry;
    if (base == NULL) {
        entry = NULL;
    } else if (slot >= 4) {
        entry = NULL;
    } else {
        entry = base + slot * 8 + 0x18;
    }
    if (entry == NULL) {
        return;
    }
    *(u16*)(entry) = value;
}
#pragma pop

/* fn_8020E020 | Size: 0x48 | Read u32 from slot in 8-byte array at offset 0x18 */
#pragma push
#pragma peephole on
u32 fn_8020E020(u8* base, u8 slot) {
    u8* entry;
    if (base == NULL) {
        entry = NULL;
    } else if (slot >= 4) {
        entry = NULL;
    } else {
        entry = base + slot * 8 + 0x18;
    }
    if (entry == NULL) {
        return 0;
    }
    return *(u32*)(entry + 0x4);
}
#pragma pop

/* fn_8020E068 | Size: 0x48 | Read u16 from slot in 8-byte array at offset 0x18 */
#pragma push
#pragma peephole on
u16 fn_8020E068(u8* base, u8 slot) {
    u8* entry;
    if (base == NULL) {
        entry = NULL;
    } else if (slot >= 4) {
        entry = NULL;
    } else {
        entry = base + slot * 8 + 0x18;
    }
    if (entry == NULL) {
        return 0;
    }
    return *(u16*)(entry);
}
#pragma pop

/* fn_8020E0F8 | Size: 0x2C | Look up entry in 0x38-byte table (indirect) */
void* fn_8020E0F8(u16 index) {
    extern u32* lbl_80478F50;
    extern u8* lbl_80478F54;
    if (index >= *lbl_80478F50) {
        return NULL;
    }
    return lbl_80478F54 + index * 0x38;
}

/* 0x8020E124 | size: 0x80 | small */
void fn_8020E124(void) {
    extern u32 lbl_80478F00;
    extern u32 lbl_80478F04;
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;

    r4 = lbl_80478F00;
    r7 = r3 & 0xFFFF;
    r6 = *(u32*)((u8*)r4 + 0x0);
    if (r7 > r6) {
        r4 = 0x0;
    } else {

        r4 = lbl_80478F04;
        /* clrlslwi r0, r3, 16, 3 */;
        r4 = r4 + r0;
    }
    if (r4 == (u32)0x0) {
        r5 = 0x0;
    } else {

        r5 = *(u8*)((u8*)r4 + 0x0);
    }
    if (r7 > r6) {
        r4 = 0x0;
    } else {

        r4 = lbl_80478F04;
        /* clrlslwi r0, r3, 16, 3 */;
        r4 = r4 + r0;
    }
    r3 = r5 & 0xFF;
    if (r4 == (u32)0x0) {
        r0 = 0x0;
    } else {

        r0 = *(u8*)((u8*)r4 + 0x2);
    }
    r0 = r0 & 0xFF;
    r0 = r3 * r0;
    r3 = r0 & 0xFFFF;
    return;
}

/* fn_8020E204 | Size: 0x2C | Look up entry in 8-byte table (indirect) */
void* fn_8020E204(u16 index) {
    extern u32 lbl_80478F00;
    extern u32 lbl_80478F04;
    u32* countPtr = (u32*)lbl_80478F00;
    if (index > *countPtr) {
        return NULL;
    }
    return (u8*)lbl_80478F04 + (u32)index * 8;
}

/* fn_8020E488 | Size: 0x2C | Look up entry in 32-byte table (indirect) */
void* fn_8020E488(u16 index) {
    extern u32 lbl_80478F40;
    extern u32 lbl_80478F44;
    u32* countPtr = (u32*)lbl_80478F40;
    if (index > *countPtr) {
        return NULL;
    }
    return (u8*)lbl_80478F44 + (u32)index * 32;
}

/* 0x8020E4CC | size: 0x1C | tiny */
/* fn_8020E4CC | Size: 0x1C | Clamp value to [0, 12] */
s32 fn_8020E4CC(s32 value) {
    if (value < 0) {
        value = 0;
    }
    if (value > 12) {
        value = 12;
    }
    return value;
}

/* Address: 0x8020E4E8 | Size: 0x94 | Ghidra import */


u32 fn_8020E4E8(u32 r3,int r4)

{
    extern u32 lbl_80478D68;
  u32 uVar1;
  u8 *pbVar2;
  int iVar3;
  u32 uVar4;

  pbVar2 = (u8 *)((r3 & 0xffff) * 2 + -0x7fc8a2f0);
  if (lbl_80478D68 <= (r3 & 0xffff)) {
    pbVar2 = (u8 *)0x0;
  }
  if (pbVar2 == (void *)0) {
    uVar4 = 0;
  }
  else {
    uVar4 = (u32)*pbVar2;
  }
  iVar3 = (r3 & 0xffff) * 2 + -0x7fc8a2f0;
  if (lbl_80478D68 <= (r3 & 0xffff)) {
    iVar3 = 0;
  }
  if (iVar3 == 0) {
    uVar1 = 1;
  }
  else {
    uVar1 = (u32)*(u8 *)(iVar3 + 1);
  }
  return (r4 * uVar4) / uVar1;
}
/* Address: 0x8020E57C | Size: 0x98 | Ghidra import */
int fn_8020E57C(void)

{
    int r3;
    u16 r4;
    int r5;

    extern int fn_801FD104();
  int iVar1;
  u16 uVar2;
  int iVar3;
  
  if (r3 != 0) {
    for (uVar2 = 0; uVar2 < r4; uVar2 = uVar2 + 1) {
      iVar3 = r3 + (u32)uVar2 * 0xc;
      iVar1 = fn_801FD104(iVar3);
      if ((iVar1 != 0) && (iVar1 = fn_801FD104(iVar3), iVar1 == r5)) {
        return iVar3;
      }
    }
  }
  return 0;
}
/* fn_8020E614 | Size: 0x2C | Check if fn_801FD104 returns non-zero */
BOOL fn_8020E614(void) {
    extern s32 fn_801FD104(void);
    return -fn_801FD104() != 0;
}

/* Address: 0x8020E640 | Size: 0x94 | Ghidra import */
void fn_8020E640(void)

{
    int r3;
    int r4;

    extern void fn_801FD07C();
    extern void fn_801FD08C();
    extern void fn_801FD09C();
    extern void fn_801FD0AC();
    extern u32 fn_80205B8C();
  u32 uVar1;
  u16 uVar2;
  
  if ((r3 != 0) && (r4 != 0)) {
    fn_801FD0AC(r3,0);
    fn_801FD09C(r3,0);
    fn_801FD08C(r3,0);
    fn_801FD07C(r3,0);
    fn_801FD0AC(r3,r4);
    uVar1 = fn_80205B8C(r4);
    uVar2 = (int)fn_8012640C(uVar1,0,0x83,0);
    fn_801FD08C(r3,uVar2);
  }
  return;
}
/* Address: 0x8020E6D4 | Size: 0x84 | Ghidra import */
void fn_8020E6D4(void)

{
    int r3;
    u16 r4;

    extern void fn_801FD07C();
    extern void fn_801FD08C();
    extern void fn_801FD09C();
    extern void fn_801FD0AC();
  u16 uVar1;
  int iVar2;
  
  if (r3 != 0) {
    for (uVar1 = 0; uVar1 < r4; uVar1 = uVar1 + 1) {
      iVar2 = r3 + (u32)uVar1 * 0xc;
      fn_801FD0AC(iVar2,0);
      fn_801FD09C(iVar2,0);
      fn_801FD08C(iVar2,0);
      fn_801FD07C(iVar2,0);
    }
  }
  return;
}
/* Address: 0x8020E758 | Size: 0x54 | Ghidra import */
#pragma push
#pragma peephole on
void fn_8020E758(u32 r3)
{
    extern void fn_801FD07C();
    extern void fn_801FD08C();
    extern void fn_801FD09C();
    extern void fn_801FD0AC();
    fn_801FD0AC(r3, 0);
    fn_801FD09C(r3, 0);
    fn_801FD08C(r3, 0);
    fn_801FD07C(r3, 0);
}
#pragma pop
/* Address: 0x8020E7AC | Size: 0x1b0 | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_8020E7AC(void* ctx, u16 count, short matchVal)
{
    extern void fn_801FBD84();
    extern void fn_801FBD94();
    extern void fn_801FBDA4();
    extern void fn_801FBDB4();
    extern void fn_801FBDC4();
    extern void fn_801FBDD4();
    extern void fn_801FBDE4();
    extern void fn_801FBDF4();
    extern void fn_801FBE18();
    extern short fn_801FBF04();
  u32 bVar1;
  u32 uVar2;
  short sVar3;
  u16 uVar6;
  void* iVar4;
  u32 uVar5;

  if (ctx == NULL) {
    uVar2 = 0;
  }
  else if (matchVal < 0) {
    uVar2 = 0;
  }
  else {
    if (ctx == NULL) {
      iVar4 = NULL;
    }
    else {
      for (uVar6 = 0; uVar6 < count; uVar6 = uVar6 + 1) {
        iVar4 = (void*)((u32)ctx + (u32)uVar6 * 0x14);
        if (matchVal < 0) {
          if (iVar4 == NULL) {
            bVar1 = 0;
          }
          else {
            sVar3 = fn_801FBF04(iVar4);
            if (sVar3 < 0) {
              bVar1 = 0;
            }
            else {
              bVar1 = 1;
            }
          }
          if (bVar1 == 0) goto LAB_0020b8ac;
        }
        else {
          if (iVar4 == NULL) {
            bVar1 = 0;
          }
          else {
            sVar3 = fn_801FBF04(iVar4);
            if (sVar3 < 0) {
              bVar1 = 0;
            }
            else {
              bVar1 = 1;
            }
          }
          if ((bVar1) && (sVar3 = fn_801FBF04(iVar4), matchVal == sVar3)) goto LAB_0020b8ac;
        }
      }
      iVar4 = NULL;
    }
LAB_0020b8ac:
    if (iVar4 == NULL) {
      uVar2 = 0;
    }
    else {
      fn_801FBE18(iVar4, (void*)0xffffffff);
      for (uVar5 = 0; (uVar5 & 0xff) < 4; uVar5 = uVar5 + 1) {
        fn_801FBDF4(iVar4,uVar5,0);
      }
      fn_801FBDE4(iVar4,0);
      fn_801FBDD4(iVar4,0);
      fn_801FBDC4(iVar4,0);
      fn_801FBDB4(iVar4,0);
      fn_801FBDA4(iVar4,0);
      fn_801FBD94(iVar4,0);
      fn_801FBD84(iVar4,0);
      uVar2 = 1;
    }
  }
  return uVar2;
}
#pragma pop
/* Address: 0x8020E95C | Size: 0x24c | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_8020E95C(void* ctx, u16 count, u32 matchVal)
{
    extern void fn_801FBD84();
    extern void fn_801FBD94();
    extern void fn_801FBDA4();
    extern void fn_801FBDB4();
    extern void fn_801FBDC4();
    extern void fn_801FBDD4();
    extern void fn_801FBDE4();
    extern void fn_801FBDF4();
    extern void fn_801FBE18();
    extern short fn_801FBF04();
  u32 bVar1;
  u32 uVar2;
  short sVar3;
  void* iVar4;
  u16 uVar6;
  u32 uVar5;

  if (ctx == NULL) {
    uVar2 = 0;
  }
  else {
    if ((short)matchVal < 0) {
      uVar2 = 0;
    }
    else {
      if (ctx == NULL) {
        iVar4 = NULL;
      }
      else {
        for (uVar6 = 0; uVar6 < count; uVar6 = uVar6 + 1) {
          iVar4 = (void*)((u32)ctx + (u32)uVar6 * 0x14);
          if ((short)matchVal < 0) {
            if (iVar4 == NULL) {
              bVar1 = 0;
            }
            else {
              sVar3 = fn_801FBF04(iVar4);
              if (sVar3 < 0) {
                bVar1 = 0;
              }
              else {
                bVar1 = 1;
              }
            }
            if (bVar1 == 0) goto LAB_0020ba60;
          }
          else {
            if (iVar4 == NULL) {
              bVar1 = 0;
            }
            else {
              sVar3 = fn_801FBF04(iVar4);
              if (sVar3 < 0) {
                bVar1 = 0;
              }
              else {
                bVar1 = 1;
              }
            }
            if ((bVar1) && (sVar3 = fn_801FBF04(iVar4), (short)matchVal == sVar3)) goto LAB_0020ba60;
          }
        }
        iVar4 = NULL;
      }
LAB_0020ba60:
      if (iVar4 == NULL) {
        if (ctx == NULL) {
          iVar4 = NULL;
        }
        else {
          for (uVar6 = 0; uVar6 < count; uVar6 = uVar6 + 1) {
            iVar4 = (void*)((u32)ctx + (u32)uVar6 * 0x14);
            if (iVar4 == NULL) {
              bVar1 = 0;
            }
            else {
              sVar3 = fn_801FBF04(iVar4);
              if (sVar3 < 0) {
                bVar1 = 0;
              }
              else {
                bVar1 = 1;
              }
            }
            if (bVar1 == 0) goto LAB_0020bae0;
          }
          iVar4 = NULL;
        }
LAB_0020bae0:
        if (iVar4 == NULL) {
          uVar2 = 0;
        }
        else {
          if (-1 < (short)matchVal) {
            fn_801FBE18(iVar4, (void*)0xffffffff);
            for (uVar5 = 0; (uVar5 & 0xff) < 4; uVar5 = uVar5 + 1) {
              fn_801FBDF4(iVar4,uVar5,0);
            }
            fn_801FBDE4(iVar4,0);
            fn_801FBDD4(iVar4,0);
            fn_801FBDC4(iVar4,0);
            fn_801FBDB4(iVar4,0);
            fn_801FBDA4(iVar4,0);
            fn_801FBD94(iVar4,0);
            fn_801FBD84(iVar4,0);
            fn_801FBE18(iVar4,matchVal);
          }
          uVar2 = 1;
        }
      }
      else {
        uVar2 = 0;
      }
    }
  }
  return uVar2;
}
#pragma pop
/* Address: 0x8020EBA8 | Size: 0xfc | Ghidra import */
#pragma push
#pragma peephole on
int fn_8020EBA8(void* p1, u16 p2, s16 p3) {
    extern s16 fn_801FBF04();
    u8 bVar1;
    s16 sVar2;
    void* iVar4;
    u16 uVar3;

    if (p1 == 0) return 0;
    for (uVar3 = 0; uVar3 < p2; uVar3++) {
        iVar4 = (void*)((u32)p1 + (u32)uVar3 * 0x14);
        if (p3 < 0) {
            if (iVar4 == 0) {
                bVar1 = 0;
            } else {
                sVar2 = fn_801FBF04(iVar4);
                if (sVar2 < 0) {
                    bVar1 = 0;
                } else {
                    bVar1 = 1;
                }
            }
            if (bVar1 == 0) {
                return (s32)iVar4;
            }
        } else {
            if (iVar4 == 0) {
                bVar1 = 0;
            } else {
                sVar2 = fn_801FBF04(iVar4);
                if (sVar2 < 0) {
                    bVar1 = 0;
                } else {
                    bVar1 = 1;
                }
            }
            if (bVar1 && p3 == (sVar2 = fn_801FBF04(iVar4))) {
                return (s32)iVar4;
            }
        }
    }
    return 0;
}
#pragma pop
/* 0x8020ECA4 | size: 0x3C | small */
#pragma push
#pragma peephole on
u32 fn_8020ECA4(void* obj) {
    extern s16 fn_801FBF04();
    s16 val;
    if (obj == 0) return 0;
    val = fn_801FBF04(obj);
    return (val >= 0) ? 1 : 0;
}
#pragma pop

/* Address: 0x8020ECE0 | Size: 0xdc | Ghidra import */
#pragma push
#pragma peephole on
void fn_8020ECE0(void* ctx, u16 count) {
    extern void fn_801FBD84();
    extern void fn_801FBD94();
    extern void fn_801FBDA4();
    extern void fn_801FBDB4();
    extern void fn_801FBDC4();
    extern void fn_801FBDD4();
    extern void fn_801FBDE4();
    extern void fn_801FBDF4(void* entry, u8 idx, u32 zero);
    extern void fn_801FBE18();
    u8 j;
    void* entry;
    u16 i;
    if (ctx == NULL) { return; }
    for (i = 0; i < count; i++) {
        entry = (void*)((u32)ctx + (u32)i * 0x14);
        fn_801FBE18(entry, (void*)0xffffffff);
        for (j = 0; j < 4; j++) {
            fn_801FBDF4(entry, j, 0);
        }
        fn_801FBDE4(entry, 0);
        fn_801FBDD4(entry, 0);
        fn_801FBDC4(entry, 0);
        fn_801FBDB4(entry, 0);
        fn_801FBDA4(entry, 0);
        fn_801FBD94(entry, 0);
        fn_801FBD84(entry, 0);
    }
}
#pragma pop
/* 0x8020EDBC | size: 0x60 */
#pragma push
#pragma peephole on
void fn_8020EDBC(void* ctx) {
    extern void fn_801FBD84();
    extern void fn_801FBD94();
    extern void fn_801FBDA4();
    extern void fn_801FBDB4();
    extern void fn_801FBDC4();
    fn_801FBDC4(ctx, 0);
    fn_801FBDB4(ctx, 0);
    fn_801FBDA4(ctx, 0);
    fn_801FBD94(ctx, 0);
    fn_801FBD84(ctx, 0);
}
#pragma pop

/* Address: 0x8020EE1C | Size: 0xa4 | Ghidra import */
void fn_8020EE1C(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;
  
  fn_80208750(r4,r3,1,0);
  fn_80208750(r5,r3,2,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,1,1,uVar1);
  fn_801F0234(0x12);
  uVar1 = fn_801F0204();
  fn_802085C4(r5,r3,2,0,uVar1);
  return;
}
/* 0x8020EEC0 | size: 0x14 | tiny */
#pragma push
#pragma peephole on
extern u8 lbl_8047B600;
u8 fn_8020EEC0(void) { return (u8)(lbl_8047B600 == 1); }
#pragma pop

/* Address: 0x8020EED4 | Size: 0x22c | Ghidra import */
#pragma push
#pragma peephole on
void fn_8020EED4(void* ctx1, void* ctx2, u32 p5, u32 p6, void* p7)
{
    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern u32 fn_801F02AC();
    extern u32 fn_801F54A4();
    extern u8 fn_802062FC();
    extern void fn_802085C4();
    extern void fn_80208750();
    u16 limit;
    void* uVar2;
    void* uVar1;
    u8 cVar5;

    limit = fn_801F54A4(0, 0, 0x14, 0);
    uVar1 = (void*)fn_801F02AC(0xf, ctx2, limit);
    uVar2 = (void*)fn_801F02AC(0x10, ctx2, limit);
    fn_80208750(ctx2, ctx1, 1, p7);
    fn_80208750(uVar1, ctx1, 2, p7);
    fn_80208750(uVar2, ctx1, 2, p7);
    cVar5 = fn_802062FC(uVar1);
    if ((cVar5 == 1) && (cVar5 = fn_802062FC(uVar2), cVar5 == 1)) {
        fn_801F0234(0x11);
        fn_802085C4(ctx2, ctx1, 1, 1, fn_801F0204());
        fn_801F0234(0x12);
        fn_802085C4(uVar1, ctx1, 2, 1, fn_801F0204());
        fn_801F0234(0x12);
        fn_802085C4(uVar2, ctx1, 2, 0, fn_801F0204());
    } else {
        cVar5 = fn_802062FC(uVar2);
        if (cVar5 == 1) {
            fn_801F0234(0x11);
            fn_802085C4(ctx2, ctx1, 1, 1, fn_801F0204());
            fn_801F0234(0x12);
            fn_802085C4(uVar2, ctx1, 2, 0, fn_801F0204());
        } else {
            cVar5 = fn_802062FC(uVar1);
            if (cVar5 == 1) {
                fn_801F0234(0x11);
                fn_802085C4(ctx2, ctx1, 1, 1, fn_801F0204());
                fn_801F0234(0x12);
                fn_802085C4(uVar1, ctx1, 2, 0, fn_801F0204());
            } else {
                fn_801F0234(0x11);
                fn_802085C4(ctx2, ctx1, 1, 1, fn_801F0204());
            }
        }
    }
}
#pragma pop
/* 0x8020F108 | size: 0x128 */
void fn_8020F108(void* battleCtx, void* ctx) {
    extern void fn_8011BEB4();
    extern void fn_801C3430();
    extern void battleGridReplacePokemon();
    extern void fn_801DB100();
    extern u8 fn_801F453C();
    extern void fn_801FCEC4();
    extern void* fn_80207C6C();
    extern void fn_802085C4();
    extern void fn_80208750();
    u8 localBuf[0x6E8];
    void* eeData;
    void* resolved;
    u8 partySlot;

    partySlot = (u8)fn_801F453C(0, 1);
    fn_8011BEB4(0, battleCtx, 0x1F, 0);
    eeData = fn_8012640C(ctx, 0, 0xEE, 0);
    resolved = fn_80207C6C(ctx, partySlot);
    fn_801FCEC4(localBuf, ctx);
    fn_801254B4(localBuf, 0, 0xEE, 0, (u32)resolved);
    fn_80208750(ctx, battleCtx, 1, 0);
    fn_80208750(localBuf, battleCtx, 3, 0);
    fn_802085C4(ctx, battleCtx, 1, 1, 0);
    battleGridReplacePokemon(eeData, resolved);
    fn_801C3430();
    fn_801254B4(ctx, 0, 0xEE, 0, (u32)resolved);
    fn_802085C4(ctx, battleCtx, 3, 0, 0);
    fn_801DB100(eeData);
}

/* 0x8020F238 | size: 0x128 */
void fn_8020F238(void* battleCtx, void* ctx) {
    extern void fn_8011BEB4();
    extern void fn_801C3430();
    extern void battleGridReplacePokemon();
    extern void fn_801DB100();
    extern u8 fn_801F453C();
    extern void fn_801FCEC4();
    extern void* fn_80207C6C();
    extern void fn_802085C4();
    extern void fn_80208750();
    u8 localBuf[0x6E8];
    void* eeData;
    void* resolved;
    u8 partySlot;

    partySlot = (u8)fn_801F453C(0, 1);
    fn_8011BEB4(0, battleCtx, 0x1F, 0);
    eeData = fn_8012640C(ctx, 0, 0xEE, 0);
    resolved = fn_80207C6C(ctx, partySlot);
    fn_801FCEC4(localBuf, ctx);
    fn_801254B4(localBuf, 0, 0xEE, 0, (u32)resolved);
    fn_80208750(ctx, battleCtx, 3, 0);
    fn_80208750(localBuf, battleCtx, 3, 1);
    fn_802085C4(ctx, battleCtx, 3, 1, 0);
    battleGridReplacePokemon(eeData, resolved);
    fn_801C3430();
    fn_801254B4(ctx, 0, 0xEE, 0, (u32)resolved);
    fn_802085C4(ctx, battleCtx, 3, 0, 0);
    fn_801DB100(eeData);
}

/* Address: 0x8020F368 | Size: 0x80 | Ghidra import */
void fn_8020F368(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern int fn_802026E4();
    extern void fn_802085C4();
    extern void fn_80208750();
  s8 cVar2;
  u32 uVar1;
  
  fn_80208750(r4,r3,1,r7);
  cVar2 = fn_802026E4(r5,0x37);
  if (cVar2 == 1) {
    fn_801F0234(0x11);
    uVar1 = fn_801F0204();
    fn_802085C4(r4,r3,1,0,uVar1);
  }
  return;
}
/* Address: 0x8020F3F0 | Size: 0xa4 | Ghidra import */
void fn_8020F3F0(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;

  fn_80208750(r4,r3,1,r7);
  fn_80208750(r5,r3,2,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,1,1,uVar1);
  fn_801F0234(0x12);
  uVar1 = fn_801F0204();
  fn_802085C4(r5,r3,2,0,uVar1);
  return;
}
/* Address: 0x8020F494 | Size: 0x84 | Ghidra import */

u32 fn_8020F494(void)

{
    extern int fn_801F453C();
  u8 bVar2;
  u32 uVar1;
  
  bVar2 = fn_801F453C(0,1);
  if (bVar2 == 2) {
    uVar1 = 1;
  }
  else if (bVar2 < 2) {
    if (bVar2 == 0) {
      uVar1 = 0;
    }
    else {
      uVar1 = 3;
    }
  }
  else if (bVar2 == 4) {
    uVar1 = 2;
  }
  else if (bVar2 < 4) {
    uVar1 = 4;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}
/* Address: 0x8020F518 | Size: 0xa4 | Ghidra import */
void fn_8020F518(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;
  
  fn_80208750(r4,r3,1,r7);
  fn_80208750(r5,r3,2,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,1,1,uVar1);
  fn_801F0234(0x12);
  uVar1 = fn_801F0204();
  fn_802085C4(r5,r3,2,0,uVar1);
  return;
}
/* Address: 0x8020F5BC | Size: 0x8c | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_8020F5BC(u32 unused, void* typeObj)

{
    extern int fn_80202360();
  short sVar1;

  sVar1 = fn_80202360(typeObj,0x2f);
  switch (sVar1) {
  case 1:
    return 0;
  case 2:
    return 1;
  case 3:
    return 2;
  case 4:
    return 3;
  case -1:
    return 4;
  }
  return 0;
}
#pragma pop
/* Address: 0x8020F648 | Size: 0xa4 | Ghidra import */
void fn_8020F648(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;

  fn_80208750(r4,r3,1,0);
  fn_80208750(r5,r3,2,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,1,1,uVar1);
  fn_801F0234(0x12);
  uVar1 = fn_801F0204();
  fn_802085C4(r5,r3,2,0,uVar1);
  return;
}
/* 0x8020F6EC | size: 0x60 | small */
s32 fn_8020F6EC(void) {
    extern u8 lbl_80379F58[];
    u32 val;
    val = *(u8*)((u8*)lbl_80379F58 + (0x1 << 16) + 0x6002);
    switch ((s32)val) {
    case 0: return 3;
    case 1: return 0;
    case 2: return 1;
    case 3: return 2;
    default: return 0;
    }
}

/* 0x8020F74C | size: 0x64 */
void fn_8020F74C(void* p1, void* p2, u32 unused1, u32 unused2, u32 p3) {
    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
    u32 result;
    fn_80208750(p2, p1, 1, p3);
    fn_801F0234(0x11);
    result = fn_801F0204();
    fn_802085C4(p2, p1, 1, 0, result);
}

/* 0x8020F7B8 | size: 0x114 */
void fn_8020F7B8(void* p1, void* p2, u32 p3, u32 p4) {
    extern u16 fn_8011BEB4();
    extern void fn_801F00D0();
    extern void* fn_801F0204();
    extern void* fn_801F0234();
    extern void* fn_801F02AC();
    extern u16 fn_801F54A4();
    extern u8 fn_802062FC();
    extern void fn_802085C4();
    extern void fn_80208750();
    u16 partyCount;
    void* d9Data;
    u16 field29;
    void* resolved;
    void* tablePtr;

    partyCount = fn_801F54A4(0, 0, 0x14, 0);
    d9Data = fn_8012640C(p2, 0, 0xD9, 0);
    field29 = fn_8011BEB4(d9Data, 0, 0x29, 0);
    fn_801F00D0(field29, partyCount);
    resolved = fn_801F02AC(0xE, p2, partyCount);
    fn_80208750(p2, p1, 1, p4);
    fn_80208750(resolved, p1, 3, p4);
    tablePtr = fn_801F0204(fn_801F0234(0x11));
    fn_802085C4(p2, p1, 1, 1, tablePtr);
    if ((u8)fn_802062FC(resolved) == 1) {
        tablePtr = fn_801F0204(fn_801F0234(0x11));
        fn_802085C4(resolved, p1, 3, 0, tablePtr);
    }
}

/* Address: 0x8020F8D4 | Size: 0x64 | Ghidra import */
void fn_8020F8D4(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;
  
  fn_80208750(r4,r3,3,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,3,0,uVar1);
  return;
}
/* Address: 0x8020F938 | Size: 0x64 | Ghidra import */
void fn_8020F938(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;
  
  fn_80208750(r4,r3,3,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,3,0,uVar1);
  return;
}
/* fn_8020F99C | Size: 0x40 | Check if byte at fixed address equals 3 */
BOOL fn_8020F99C(void) {
    extern u8 lbl_80379F58[];
    u8 val = *(u8*)((u8*)lbl_80379F58 + 0x16002);
    switch (val) {
        case 1:
        case 2:
            return FALSE;
        case 3:
            return TRUE;
        default:
            return FALSE;
    }
}

/* Address: 0x8020F9DC | Size: 0xa4 | Ghidra import */
void fn_8020F9DC(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;

  fn_80208750(r4,r3,1,r7);
  fn_80208750(r5,r3,2,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,1,1,uVar1);
  fn_801F0234(0x12);
  uVar1 = fn_801F0204();
  fn_802085C4(r5,r3,2,0,uVar1);
  return;
}
/* fn_8020FA80 | Size: 0x40 | Check if byte at fixed address equals 3 */
BOOL fn_8020FA80(void) {
    extern u8 lbl_80379F58[];
    u8 val = *(u8*)((u8*)lbl_80379F58 + 0x16002);
    switch (val) {
        case 1:
        case 2:
            return FALSE;
        case 3:
            return TRUE;
        default:
            return FALSE;
    }
}

/* 0x8020FAC0 | size: 0x70 */
#pragma push
#pragma peephole on
void fn_8020FAC0(void* p1, void* p2, u32 unused1, u16 flag, u32 p4) {
    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
    fn_80208750(p2, p1, 1, p4);
    if (flag == 0) {
        fn_801F0234(0x11);
        fn_802085C4(p2, p1, 1, 0, fn_801F0204());
    }
}
#pragma pop

/* Address: 0x8020FB38 | Size: 0xc4 | Ghidra import */
void fn_8020FB38(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    short r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;
  
  if (r6 == 0) {
    fn_80208750(r4,r3,1,r7);
    fn_80208750(r5,r3,2,r7);
    fn_801F0234(0x11);
    uVar1 = fn_801F0204();
    fn_802085C4(r4,r3,1,0,uVar1);
  }
  else {
    fn_80208750(r5,r3,2,r7);
    fn_801F0234(0x12);
    uVar1 = fn_801F0204();
    fn_802085C4(r5,r3,2,0,uVar1);
  }
  return;
}
/* Address: 0x8020FC04 | Size: 0x6c | Ghidra import */

u32 fn_8020FC04(void)

{
    extern int fn_801F453C();
  u8 bVar2;
  u32 uVar1;
  
  bVar2 = fn_801F453C(0,1);
  if (bVar2 == 1) {
    uVar1 = 0;
  }
  else if (bVar2 == 0) {
    uVar1 = 1;
  }
  else if (bVar2 < 5) {
    uVar1 = 2;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}
/* 0x8020FC70 | size: 0x11C */
#pragma push
#pragma peephole on
void fn_8020FC70(void* p1, void* p2, void* p3, u16 mode, u32 p5) {
    extern u8 fn_8011BEB4();
    extern void* fn_801F0204();
    extern void* fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
    u8 battleType;

    battleType = (u8)fn_8011BEB4(0, p1, 5, 0);
    if ((battleType == 4 || battleType == 6 || battleType == 1) && mode != 0) {
        fn_80208750(p3, p1, 2, p5);
        fn_802085C4(p3, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)));
    } else {
        fn_80208750(p2, p1, 1, 0);
        fn_80208750(p3, p1, 2, p5);
        fn_802085C4(p2, p1, 1, 1, fn_801F0204(fn_801F0234(0x11)));
        fn_802085C4(p3, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)));
    }
}
#pragma pop

/* Address: 0x8020FD8C | Size: 0xb4 | Ghidra import */
u32 fn_8020FD8C(void)

{
    u32 r3;
    u32 r4;

    extern u32 fn_8011BEB4();
  u32 uVar1;
  u16 uVar2;
  
  uVar1 = (int)fn_8012640C(r4,0,0xd9,0);
  uVar2 = fn_8011BEB4(uVar1,0,0x2f,0);
  if (uVar2 == 0x46) {
    return 1;
  }
  if (uVar2 < 0x46) {
    if (uVar2 == 0x1e) {
      return 0;
    }
    if (uVar2 < 0x1e) {
      if (uVar2 == 10) {
        return 0;
      }
    }
    else if (uVar2 == 0x32) {
      return 1;
    }
  }
  else {
    if (uVar2 == 0x6e) {
      return 2;
    }
    if (uVar2 < 0x6e) {
      if (uVar2 == 0x5a) {
        return 1;
      }
    }
    else if (uVar2 == 0x96) {
      return 2;
    }
  }
  return 0;
}
/* Address: 0x8020FE40 | Size: 0xa4 | Ghidra import */
void fn_8020FE40(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;
  
  fn_80208750(r4,r3,1,0);
  fn_80208750(r5,r3,2,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,1,1,uVar1);
  fn_801F0234(0x12);
  uVar1 = fn_801F0204();
  fn_802085C4(r5,r3,2,0,uVar1);
  return;
}
/* Address: 0x8020FEE4 | Size: 0x78 | Ghidra import */
char fn_8020FEE4()

{
    u32 r3;
    u32 r4;

    extern u32 fn_8011BEB4();
  s8 cVar1;
  u32 uVar2;
  u16 uVar3;
  
  uVar2 = (int)fn_8012640C(r4,0,0xd9,0);
  uVar3 = fn_8011BEB4(uVar2,0,0x2f,0);
  if (uVar3 < 0x5a) {
    if (uVar3 < 0x3e) {
      cVar1 = -((uVar3 < 0x16) + -1);
    }
    else {
      cVar1 = 2;
    }
  }
  else {
    cVar1 = 3;
  }
  return cVar1;
}
/* Address: 0x8020FF5C | Size: 0xa4 | Ghidra import */
void fn_8020FF5C(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;
  
  fn_80208750(r4,r3,1,0);
  fn_80208750(r5,r3,2,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,1,1,uVar1);
  fn_801F0234(0x12);
  uVar1 = fn_801F0204();
  fn_802085C4(r5,r3,2,0,uVar1);
  return;
}
/* Address: 0x80210000 | Size: 0x74 | Ghidra import */
u32 fn_80210000(void)

{
    u32 r3;
    u32 r4;

    extern u32 fn_8011BEB4();
  u32 uVar1;
  u16 uVar2;
  
  uVar1 = (int)fn_8012640C(r4,0,0xd9,0);
  uVar2 = fn_8011BEB4(uVar1,0,0x2f,0);
  if (uVar2 == 0x50) {
LAB_0020d058:
    uVar1 = 0;
  }
  else {
    if (uVar2 < 0x50) {
      if (uVar2 == 0x28) goto LAB_0020d058;
    }
    else if (uVar2 == 0x78) goto LAB_0020d058;
    uVar1 = 1;
  }
  return uVar1;
}
/* Address: 0x80210074 | Size: 0xa4 | Ghidra import */
void fn_80210074(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;
  
  fn_80208750(r4,r3,1,0);
  fn_80208750(r5,r3,2,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,1,1,uVar1);
  fn_801F0234(0x12);
  uVar1 = fn_801F0204();
  fn_802085C4(r5,r3,2,0,uVar1);
  return;
}
/* Address: 0x80210118 | Size: 0x78 | Ghidra import */
int fn_80210118(void)

{
    u32 r3;
    u32 r4;

    extern u32 fn_8011BEB4();
  u32 uVar1;
  u16 uVar3;
  int iVar2;
  
  uVar1 = (int)fn_8012640C(r4,0,0xd9,0);
  uVar3 = fn_8011BEB4(uVar1,0,0x2f,0);
  if (uVar3 <= 0x18) {
    iVar2 = 0;
  }
  else if (uVar3 <= 0x24) {
    iVar2 = 1;
  }
  else {
    iVar2 = 3 - (u32)(uVar3 < 0x51);
  }
  return iVar2;
}
/* 0x80210190 | size: 0xA4 */
void fn_80210190(void* p1, void* p2, void* p3, u32 unused, u32 p4) {
    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
    u32 result;
    fn_80208750(p2, p1, 1, p4);
    fn_80208750(p3, p1, 2, p4);
    fn_801F0234(0x11);
    result = fn_801F0204();
    fn_802085C4(p2, p1, 1, 1, result);
    fn_801F0234(0x12);
    result = fn_801F0204();
    fn_802085C4(p3, p1, 2, 0, result);
}

/* 0x80210234 | size: 0x84 */
#pragma push
#pragma peephole on
u32 fn_80210234(u32 unused, void* typeObj) {
    extern s16 fn_80202360();
    s16 val;
    val = fn_80202360(typeObj, 0x2E);
    switch (val) {
        case 1: return 0;
        case 2: return 1;
        case 3: return 2;
        case 4: return 3;
        case 5: return 4;
        default: return 0;
    }
}
#pragma pop

/* Address: 0x802102B8 | Size: 0xa4 | Ghidra import */
void fn_802102B8(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;
  
  fn_80208750(r4,r3,1,0);
  fn_80208750(r5,r3,2,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,1,1,uVar1);
  fn_801F0234(0x12);
  uVar1 = fn_801F0204();
  fn_802085C4(r5,r3,2,0,uVar1);
  return;
}
/* Address: 0x8021035C | Size: 0x8c | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_8021035C(u32 unused, void* typeObj)

{
    extern int fn_80202360();
  short sVar1;

  sVar1 = fn_80202360(typeObj,0x2f);
  switch (sVar1) {
  case 1:
    return 0;
  case 2:
    return 1;
  case 3:
    return 2;
  case 4:
    return 3;
  case -1:
    return 4;
  }
  return 0;
}
#pragma pop
/* 0x802103E8 | size: 0x100 */
#pragma push
#pragma peephole on
void fn_802103E8(void* p1, void* p2, void* p3, u16 mode, u32 p5) {
    extern u8 fn_8011BEB4();
    extern void* fn_801F0204();
    extern void* fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();

    fn_8011BEB4(0, p1, 5, 0);
    if (mode != 0) {
        fn_80208750(p3, p1, 2, p5);
        fn_802085C4(p3, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)));
    } else {
        fn_80208750(p2, p1, 1, p5);
        fn_80208750(p3, p1, 2, p5);
        fn_802085C4(p2, p1, 1, 1, fn_801F0204(fn_801F0234(0x11)));
        fn_802085C4(p3, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)));
    }
}
#pragma pop

/* Address: 0x802104E8 | Size: 0x84 | Ghidra import */
u32 fn_802104E8(void)

{
    u32 r3;
    u32 r4;

    extern u32 fn_8011BEB4();
  u32 uVar1;
  u16 uVar2;
  
  uVar1 = (int)fn_8012640C(r4,0,0xd9,0);
  uVar2 = fn_8011BEB4(uVar1,0,0x2f,0);
  if (uVar2 == 0x14) {
    uVar1 = 1;
  }
  else {
    if (uVar2 < 0x14) {
      if (uVar2 == 10) {
        return 0;
      }
    }
    else if (uVar2 == 0x1e) {
      return 2;
    }
    uVar1 = 0;
  }
  return uVar1;
}
/* Address: 0x8021056C | Size: 0xa4 | Ghidra import */
void fn_8021056C(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;
  
  fn_80208750(r4,r3,1,0);
  fn_80208750(r5,r3,2,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,1,1,uVar1);
  fn_801F0234(0x12);
  uVar1 = fn_801F0204();
  fn_802085C4(r5,r3,2,0,uVar1);
  return;
}
/* fn_80210610 | Size: 0x48 | Get level category from fn_80203E0C result */
u32 fn_80210610(void* unused, void* param) {
    extern u8 fn_80203E0C(void* param);
    u8 val = fn_80203E0C(param);
    if (val < 0x21) {
        return 0;
    }
    if (val >= 0x42) {
        return 2;
    }
    return 1;
}

/* Address: 0x80210658 | Size: 0xa4 | Ghidra import */
void fn_80210658(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;
  
  fn_80208750(r4,r3,1,r7);
  fn_80208750(r5,r3,2,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,1,1,uVar1);
  fn_801F0234(0x12);
  uVar1 = fn_801F0204();
  fn_802085C4(r5,r3,2,0,uVar1);
  return;
}
/* Address: 0x802106FC | Size: 0xc0 | Ghidra import */
u32 fn_802106FC(void)

{
    u32 r3;
    u32 r4;

    extern u32 fn_8011BEB4();
  u32 uVar1;
  u16 uVar2;
  
  uVar1 = (int)fn_8012640C(r4,0,0xd9,0);
  uVar2 = fn_8011BEB4(uVar1,0,0x2f,0);
  if (uVar2 == 0x50) {
    return 3;
  }
  if (uVar2 < 0x50) {
    if (uVar2 == 0x28) {
      return 1;
    }
    if (uVar2 < 0x28) {
      if (uVar2 == 0x14) {
        return 0;
      }
    }
    else if (uVar2 == 0x3c) {
      return 2;
    }
  }
  else {
    if (uVar2 == 0x78) {
      return 5;
    }
    if ((uVar2 < 0x78) && (uVar2 == 100)) {
      return 4;
    }
  }
  return 0;
}
/* 0x802107BC | size: 0xC4 */
#pragma push
#pragma peephole on
void fn_802107BC(void* p1, void* p2, void* p3, u16 mode, u32 p5) {
    extern void* fn_801F0204();
    extern void* fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();

    if (mode == 0) {
        fn_80208750(p2, p1, 1, p5);
        fn_80208750(p3, p1, 2, p5);
        fn_802085C4(p2, p1, 1, 0, fn_801F0204(fn_801F0234(0x11)));
    } else {
        fn_80208750(p3, p1, 2, p5);
        fn_802085C4(p3, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)));
    }
}
#pragma pop

/* 0x80210888 | size: 0x108 */
#pragma push
#pragma peephole on
void fn_80210888(void* p1, void* p2, u32 p3, u32 unused, u32 p4) {
    extern void* fn_801F0204();
    extern void* fn_801F0234();
    extern void* fn_801F02AC();
    extern u16 fn_801F54A4();
    extern u8 fn_802062FC();
    extern void fn_802085C4();
    extern void fn_80208750();
    u16 partyCount;
    void* resolved;

    partyCount = fn_801F54A4(0, 0, 0x14, 0);
    resolved = fn_801F02AC(0xE, p2, partyCount);
    fn_80208750(p2, p1, 3, p4);
    fn_80208750(resolved, p1, 3, p4);
    if ((u8)fn_802062FC(resolved) == 1) {
        fn_802085C4(p2, p1, 3, 1, fn_801F0204(fn_801F0234(0x11)));
        fn_802085C4(resolved, p1, 3, 0, fn_801F0204(fn_801F0234(0x11)));
    } else {
        fn_802085C4(p2, p1, 3, 0, fn_801F0204(fn_801F0234(0x11)));
    }
}
#pragma pop

/* 0x80210998 | size: 0x168 */
#pragma push
#pragma peephole on
void fn_80210998(void* p1, void* p2, void* p3, u32 unused, u32 p4) {
    extern void* fn_801F0204();
    extern void* fn_801F0234();
    extern void* fn_801F02AC();
    extern u16 fn_801F54A4();
    extern u8 fn_802062FC();
    extern void fn_802085C4();
    extern void fn_80208750();
    u16 partyCount;
    void* resolved;
    void* tablePtr;

    partyCount = fn_801F54A4(0, 0, 0x14, 0);
    resolved = fn_801F02AC(0x10, p2, partyCount);
    fn_80208750(p2, p1, 1, p4);
    fn_80208750(p3, p1, 2, p4);
    fn_80208750(resolved, p1, 2, p4);
    if ((u8)fn_802062FC(resolved) == 1) {
        fn_802085C4(p2, p1, 1, 1, fn_801F0204(fn_801F0234(0x11)));
        fn_802085C4(p3, p1, 2, 1, fn_801F0204(fn_801F0234(0x12)));
        fn_802085C4(resolved, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)));
    } else {
        fn_802085C4(p2, p1, 1, 1, fn_801F0204(fn_801F0234(0x11)));
        fn_802085C4(p3, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)));
    }
}
#pragma pop

/* 0x80210B08 | size: 0xE8 */
#pragma push
#pragma peephole on
void fn_80210B08(void* p1, void* p2, void* p3, u16 mode, u32 p5) {
    extern void* fn_801F0204();
    extern void* fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
    void* tablePtr;

    if (mode != 0) {
        fn_80208750(p3, p1, 2, p5);
        fn_802085C4(p3, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)));
    } else {
        fn_80208750(p2, p1, 1, p5);
        fn_80208750(p3, p1, 2, p5);
        fn_802085C4(p2, p1, 1, 1, fn_801F0204(fn_801F0234(0x11)));
        fn_802085C4(p3, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)));
    }
}
#pragma pop

/* 0x80210BF8 | size: 0x104 */
#pragma push
#pragma peephole on
void fn_80210BF8(void* p1, void* p2, void* p3, u16 mode, u32 p5) {
    extern void* fn_801F0204();
    extern void* fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();

    if (mode == 0) {
        fn_80208750(p2, p1, 1, p5);
        fn_80208750(p3, p1, 2, p5);
        fn_80208750(p2, p1, 3, p5);
        fn_802085C4(p2, p1, 1, 1, fn_801F0204(fn_801F0234(0x11)));
        fn_802085C4(p3, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)));
    } else if (mode == 1) {
        fn_80208750(p2, p1, 3, p5);
        fn_802085C4(p2, p1, 3, 0, fn_801F0204(fn_801F0234(0x11)));
    }
}
#pragma pop

/* 0x80210D04 | size: 0x150 */
#pragma push
#pragma peephole on
void fn_80210D04(void* p1, void* p2, void* p3, u16 mode, u32 p5) {
    extern s32 fn_8011BEB4();
    extern void* fn_801F0204();
    extern void* fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
    extern void fn_802221EC();
    void* d9Data;
    s32 field2D;

    d9Data = fn_8012640C(p2, 0, 0xD9, 0);
    field2D = fn_8011BEB4(d9Data, 0, 0x2D, 0);
    if (mode == 0) {
        fn_80208750(p2, p1, 1, p5);
        fn_80208750(p3, p1, 2, p5);
        fn_80208750(p2, p1, 3, p5);
        fn_802085C4(p2, p1, 1, 1, fn_801F0204(fn_801F0234(0x11)));
        fn_802085C4(p3, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)));
    } else if (mode == 1) {
        fn_80208750(p2, p1, 3, p5);
        if (field2D > 0) {
            fn_802221EC(0x32, p2, 0, 1);
        } else {
            fn_802085C4(p2, p1, 3, 0, fn_801F0204(fn_801F0234(0x11)));
        }
    }
}
#pragma pop

/* Address: 0x80210E5C | Size: 0x64 | Ghidra import */
void fn_80210E5C(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;

    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  u32 uVar1;
  
  fn_80208750(r4,r3,3,r7);
  fn_801F0234(0x11);
  uVar1 = fn_801F0204();
  fn_802085C4(r4,r3,3,0,uVar1);
  return;
}
/* Address: 0x80210EC8 | Size: 0x170 | Ghidra import */
void fn_80210EC8(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    short r6;
    u32 r7;

    extern u32 DAT_8038ff5a;
    extern s8 fn_8011BEB4();
    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
  s8 cVar2;
  u32 uVar1;
  
  if (DAT_8038ff5a == 1) {
    cVar2 = fn_8011BEB4(0,r3,5,0);
    if ((((cVar2 == 4) || (cVar2 == 6)) || (cVar2 == 1)) && (r6 != 0)) {
      fn_80208750(r5,r3,2,r7);
      fn_801F0234(0x12);
      uVar1 = fn_801F0204();
      fn_802085C4(r5,r3,2,0,uVar1);
    }
    else {
      fn_80208750(r4,r3,1,r7);
      fn_80208750(r5,r3,2,r7);
      fn_801F0234(0x11);
      uVar1 = fn_801F0204();
      fn_802085C4(r4,r3,1,1,uVar1);
      fn_801F0234(0x12);
      uVar1 = fn_801F0204();
      fn_802085C4(r5,r3,2,0,uVar1);
    }
  }
  else {
    fn_80208750(r4,r3,3,r7);
    fn_801F0234(0x11);
    uVar1 = fn_801F0204();
    fn_802085C4(r4,r3,3,0,uVar1);
  }
  return;
}
/* 0x80211040 | size: 0x11C */
#pragma push
#pragma peephole on
void fn_80211040(void* p1, void* p2, void* p3, u16 mode, u32 p5) {
    extern u8 fn_8011BEB4();
    extern void* fn_801F0204();
    extern void* fn_801F0234();
    extern void fn_802085C4();
    extern void fn_80208750();
    u8 battleType;

    battleType = (u8)fn_8011BEB4(0, p1, 5, 0);
    if ((battleType == 4 || battleType == 6 || battleType == 1) && mode != 0) {
        fn_80208750(p3, p1, 2, p5);
        fn_802085C4(p3, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)));
    } else {
        fn_80208750(p2, p1, 1, p5);
        fn_80208750(p3, p1, 2, p5);
        fn_802085C4(p2, p1, 1, 1, fn_801F0204(fn_801F0234(0x11)));
        fn_802085C4(p3, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)));
    }
}
#pragma pop

/* 0x80211164 | size: 0x4 | trivial */
void fn_80211164(void) { return; }

/* Address: 0x80211170 | Size: 0x68c | Ghidra import */
u32
fn_80211170(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    char r7;
    char r8;
    void *r9;
    void *r10;

    extern u32 fn_800E0C54();
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern void fn_801FCEC4();
    extern int fn_802026E4();
    extern u16 fn_80203D3C();
    extern u32 fn_80203FE4();
    extern u32 fn_80205184();
    extern short fn_80207BF4();
    extern u32 fn_802096E8();
    extern void fn_802099AC();
    extern void fn_80209D90();
    extern u8 fn_8020A068();
    extern void fn_8020A080();
    extern void fn_8020A2B8();
    extern void fn_802271E0();
    extern void fn_802274F0();
    extern int fn_80232110();
    extern u32 lbl_80478D60;
    extern u8 lbl_80478D78[1];
    extern u32 lbl_8047B610;
    extern u32 lbl_8047B618;
  u8 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u16 uVar15;
  u32 uVar5;
  u32 uVar6;
  short sVar16;
  u32 uVar7;
  u32 uVar8;
  u32 uVar9;
  u32 uVar10;
  u32 uVar11;
  u32 uVar12;
  s8 cVar18;
  u32 uVar13;
  u32 uVar14;
  u16 uVar17;
  u8 *puVar19;
  u8 *puVar20;
  u32 uVar21;
  u32 uVar22;

  u32 uVar23;
  int iVar24;
  int iVar25;
  u8 local_ec8 [8];
  u8 auStack_ec0 [172];
  u8 auStack_e14 [1760];
  u8 auStack_734 [1760];
  u32 local_54;
  u32 local_50;
  
  local_54 = fn_801F54A4(0,0,0x36,0);
  uVar2 = fn_801F54A4(0,0,0x42,0);
  local_50 = lbl_8047B610;
  puVar20 = (u8 *)(lbl_80478D78);
  puVar19 = local_ec8;
  uVar23 = lbl_8047B618;
  iVar25 = 8;
  do {
    uVar1 = *puVar20;
    puVar20 = puVar20 + 1;
    *puVar19 = uVar1;
    puVar19 = puVar19 + 1;
    iVar25 = iVar25 + -1;
  } while (iVar25 != 0);
  fn_801FCEC4(auStack_734,r5);
  fn_801FCEC4(auStack_e14,r6);
  fn_801F4C14(0,0,0x36,0,r5);
  fn_801F4C14(0,0,0x42,0,r6);
  uVar3 = (int)fn_8012640C(r5,0,0xd9,0);
  fn_8020A2B8(auStack_ec0,uVar3);
  fn_80209D90(uVar3);
  fn_802099AC(uVar3,0,r4,0,0);
  puVar19 = (u8 *)(lbl_80478D78);
  lbl_8047B618 = 0;
  iVar25 = 8;
  do {
    *puVar19 = 0;
    puVar19 = puVar19 + 1;
    iVar25 = iVar25 + -1;
  } while (iVar25 != 0);
  if (r9 != (void *)0) {
    ((void (*)())r9)(r3,r4,r5,r6);
  }
  if (r7 != 1) goto LAB_0020e58c;
  uVar4 = fn_801F025C(0x11,0);
  fn_80207BF4();
  uVar15 = fn_80203FE4(uVar4);
  uVar5 = (int)fn_8012640C(uVar4,0,0xd9,0);
  uVar6 = fn_80205184(uVar4);
  sVar16 = fn_80203D3C(uVar4);
  uVar7 = fn_801F025C(0x12,0);
  iVar25 = 0;
  if ((uVar15 == 0x3f) && (sVar16 == 0x71)) {
    iVar25 = 1;
  }
  iVar24 = 0;
  if ((uVar15 == 0x42) && (sVar16 == 0x53)) {
    iVar24 = 1;
  }
  uVar8 = fn_802026E4(uVar4,0xf);
  uVar8 = __cntlzw(1 - (uVar8 & 0xff));
  uVar9 = fn_8011BEB4(0,uVar6,9,0);
  uVar9 = __cntlzw(0x2b - (uVar9 & 0xffff));
  uVar10 = fn_8011BEB4(0,uVar6,9,0);
  uVar10 = __cntlzw(0x4b - (uVar10 & 0xffff));
  uVar11 = fn_8011BEB4(0,uVar6,9,0);
  uVar11 = __cntlzw(200 - (uVar11 & 0xffff));
  uVar12 = fn_8011BEB4(0,uVar6,9,0);
  uVar21 = __cntlzw(0xd1 - (uVar12 & 0xffff));
  uVar22 = (int)lbl_80478D60 - 1;
  uVar12 = __cntlzw(0x29 - (u32)uVar15);
  uVar8 = (uVar9 >> 5) +
          (uVar8 >> 4 & 0xffffffe) + (uVar10 >> 5) + (uVar11 >> 5) + (uVar21 >> 5) + (uVar12 >> 5) +
          iVar25 * 2 + iVar24 * 2 & 0xffff;
  if (uVar22 < uVar8) {
    uVar8 = uVar22 & 0xffff;
  }
  sVar16 = fn_80207BF4(uVar7);
  if ((sVar16 == 4) || (sVar16 = fn_80207BF4(uVar7), sVar16 == 0x4b)) {
LAB_0020e568:
    fn_8011BBD8(uVar5,0,0x2b,0,1);
  }
  else {
    cVar18 = fn_801F54A4(0,0,0x29,0);
    if (cVar18 != 1) goto LAB_0020e568;
    fn_8020A080(uVar8);
    uVar8 = fn_8020A068();
    uVar9 = fn_800E0C54();
    if (((uVar9 & 0xffff) == ((uVar9 & 0xffff) / (uVar8 & 0xff)) * (uVar8 & 0xff)) ||
       (((cVar18 = fn_802026E4(uVar4,0x3e), cVar18 == 1 && ((uVar6 & 0xffff) == 0x164)) &&
        (uVar6 = fn_800E0C54(), (uVar6 & 0xffff) % 100 < 0x5a)))) {
      fn_8011BBD8(uVar5,0,0x2b,0,2);
    }
    else {
      fn_8011BBD8(uVar5,0,0x2b,0,1);
    }
  }
  lbl_8047B610 = (int)lbl_8047B610 + 1;
LAB_0020e58c:
  uVar4 = fn_801F025C(0x11,0);
  uVar5 = fn_801F025C(0x12,0);
  uVar7 = fn_801F025C(2,uVar5);
  uVar13 = fn_80205184(uVar4);
  uVar14 = (int)fn_8012640C(uVar4,0,0xd9,0);
  uVar17 = fn_8011BEB4(uVar14,0,0x2f,0);
  sVar16 = fn_8011BEB4(uVar14,0,0x30,0);
  iVar25 = fn_80232110(uVar4,uVar5,uVar7,uVar13,uVar17,sVar16);
  uVar6 = fn_8011BEB4(uVar14,0,0x2b,0);
  uVar8 = fn_8011BEB4(uVar14,0,0x2c,0);
  iVar25 = iVar25 * (uVar6 & 0xff) * (uVar8 & 0xff);
  cVar18 = fn_802026E4(uVar4,0x24);
  if ((cVar18 == 1) && (sVar16 == 0xd)) {
    iVar25 = iVar25 * 2;
  }
  cVar18 = fn_802026E4(uVar4,0x32);
  if (cVar18 == 1) {
    iVar25 = (iVar25 * 0xf) / 10;
  }
  fn_8011BBD8(uVar14,0,0x2d,0,iVar25);
  lbl_8047B610 = (int)lbl_8047B610 + 1;
  fn_802274F0(1,1,1,0);
  if (r8 == 1) {
    fn_802271E0(1,1);
    lbl_8047B610 = (int)lbl_8047B610 + 1;
  }
  if (r10 != (void *)0) {
    ((void (*)())r10)(r3,r4,r5,r6);
  }
  cVar18 = fn_802096E8(uVar3);
  if (cVar18 == 1) {
    uVar4 = fn_8011BEB4(uVar3,0,0x2d,0);
  }
  else {
    uVar4 = 0;
  }
  fn_801F4C14(0,0,0x36,0,local_54);
  fn_801F4C14(0,0,0x42,0,uVar2);
  fn_8020A2B8(uVar3,auStack_ec0);
  puVar19 = (u8 *)(lbl_80478D78);
  puVar20 = local_ec8;
  lbl_8047B610 = local_50;
  lbl_8047B618 = uVar23;
  iVar25 = 8;
  do {
    uVar1 = *puVar20;
    puVar20 = puVar20 + 1;
    *puVar19 = uVar1;
    puVar19 = puVar19 + 1;
    iVar25 = iVar25 + -1;
  } while (iVar25 != 0);
  fn_801FCEC4(r5,auStack_734);
  fn_801FCEC4(r6,auStack_e14);
  return uVar4;
}
/* 0x802117FC | size: 0x14 | tiny */
u32 fn_802117FC(void) {
    extern u32 lbl_8047B618;
    return !(lbl_8047B618 & 0x80);
}

/* fn_80211810 | Size: 0x20 | Set/clear bit 0x80 in flags */
void fn_80211810(u8 enable) {
    extern u32 lbl_8047B618;
    u32 val;
    u32 result;
    enable = (u8)enable;
    val = lbl_8047B618;
    result = val | 0x80u;
    if (enable == 1) {
        result = val & ~0x80u;
    }
    lbl_8047B618 = result;
}

/* 0x80211830 | size: 0xCC */
void fn_80211830(void) {
    extern u8 lbl_80478D78[1];
    extern u32 lbl_8047B62C;
    extern void fn_801F37B0();
    extern void* fn_801F47B4();
    extern void fn_801F6EEC();
    extern s32 fn_802118FC();
    extern s32 fn_80213558();
    extern s32 fn_802136A4();
    u8 localBuf[0x10];
    u16 i;
    void* slotData;

    localBuf[0] = 0;
    fn_801F37B0(0, (u32)fn_80213558, &localBuf[0], 0);
    for (i = 0; i < 2; i++) {
        slotData = fn_801F47B4(0, i);
        if (slotData != NULL) {
            fn_801F6EEC(slotData, 0x4D);
        }
    }
    fn_801F37B0(0, (u32)fn_802136A4, 0, 0);
    for (i = 0; i < 8; i++) {
        lbl_80478D78[i] = 0;
    }
    lbl_8047B62C = 0;
    fn_801F37B0(0, (u32)fn_802118FC, 0, 0);
}

/* fn_802118FC | Size: 0x4C | Check state and optionally trigger event 0x11 */
#pragma push
#pragma peephole on
s32 fn_802118FC(void* ctx) {
    extern u8 fn_802062FC(void);
    extern void fn_80202810(void* ctx, u32 eventId);
    if (!fn_802062FC()) {
        return 1;
    }
    fn_80202810(ctx, 0x11);
    return 1;
}
#pragma pop

/* Address: 0x80211948 | Size: 0x8c | Ghidra import */
void fn_80211948(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;
    u32 r8;

    extern void fn_801F0F04();
    extern s8 fn_801F11CC();
    extern void fn_8020D888();
  s8 cVar1;
  u8 auStack_38 [52];
  
  cVar1 = fn_801F11CC(auStack_38,r3,r4,r5,r6,r7);
  if (cVar1 == 1) {
    fn_8020D888(auStack_38,r8);
    cVar1 = 1;
  }
  if (cVar1 == 1) {
    fn_801F0F04(auStack_38);
  }
  return;
}
/* fn_802119D4 | Size: 0x2C | Clear bit 20 in flags and call fn_80213270 */
void fn_802119D4(void) {
    extern u32 lbl_8047B618;
    extern void fn_80213270(void);
    lbl_8047B618 &= ~0x00100000u;
    fn_80213270();
}

/* Address: 0x80211A00 | Size: 0x78 | Ghidra import */
void fn_80211A00(void)

{
    extern u32 DAT_8038ff5a;
    extern u32 DAT_8038fff9;
    extern void fn_801DA7AC();
    extern void fn_801F37B0();
    extern u8 lbl_80478D7B;
    extern u8 lbl_80478D7C;
    extern u8 lbl_8047B625;
    extern void fn_80211A78();
    extern void fn_8022FE20();

  fn_801F37B0(0,(u32)fn_80211A78,0,1);
  fn_801F37B0(0,(u32)fn_8022FE20,0,1);
  lbl_80478D7B = 0;
  lbl_8047B625 = 0;
  lbl_80478D7C = 0;
  DAT_8038ff5a = 0;
  DAT_8038fff9 = 0;
  fn_801DA7AC();
  return;
}
/* 0x80211A78 | size: 0x11C */
#pragma push
#pragma peephole on
u32 fn_80211A78(void* ctx) {
    extern u8 lbl_80375D30[];
    extern u8 lbl_803791FE[];
    extern u32 lbl_8047B62C;
    extern void fn_801F0F04();
    extern u8 fn_801F1170();
    extern u8 fn_801F11CC();
    extern u8 fn_802026E4();
    extern u16 fn_80205224();
    extern u8 fn_802062FC();
    extern void fn_8020D888();
    extern u32 fn_8020D920();
    u8 localBuf[0x30];
    void* feData;
    u32 d920val;
    u8 result;

    if ((u8)fn_802062FC(ctx) == 0) { return 1; }
    feData = fn_8012640C(ctx, 0, 0xFE, 0);
    if (feData == NULL) { return 1; }
    if ((u8)fn_801F1170(feData) == 0) { return 1; }
    if (fn_80205224(ctx) != 0x108) { goto done; }
    if ((u8)fn_802026E4(ctx, 8) != 0) { goto done; }
    if ((u8)(u32)fn_8012640C(ctx, 0, 0xF9, 0) != 0) { goto done; }
    d920val = fn_8020D920(lbl_8047B62C);
    result = fn_801F11CC(localBuf, d920val, ctx, 0xC, 0, lbl_80375D30);
    if (result == 1) {
        fn_8020D888(localBuf, (u32)lbl_803791FE);
        result = 1;
    }
    if (result == 1) {
        fn_801F0F04(localBuf);
    }
done:
    return 1;
}
#pragma pop

/* Address: 0x80211B94 | Size: 0x284 | Ghidra import */
void fn_80211B94(void)

{
    u32 r3;
    u32 r4;
    char r5;

    extern u32 DAT_80378798;
    extern u32 DAT_8038ff5a;
    extern u32 DAT_8038fff9;
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern void fn_801F37B0();
    extern u8 lbl_80478D7B;
    extern u8 lbl_80478D7C;
    extern u32 lbl_8047B610;
    extern u8 lbl_8047B614;
    extern u32 lbl_8047B618;
    extern u8 lbl_8047B625;
    extern u32 lbl_8047B62C;
  u8 uVar1;
  u8 uVar2;
  u32 uVar3;

  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  u32 uVar7;
  u8 local_38;
  u8 local_37 [15];
  
  uVar4 = lbl_8047B610;
  uVar5 = lbl_8047B62C;
  lbl_8047B610 = r4;
  uVar1 = lbl_8047B614;
  if (r5 == 0) {
    lbl_8047B614 = 0;
  }
  lbl_8047B62C = r3;
  do {
    while (((int (*)())*(u32 *)((u32)*(u8*)lbl_8047B610 * 4 + -0x7fd85ff4))(),
          r5 == 0) {
      if ((lbl_8047B614 == 1) || (lbl_8047B614 == 2))
      goto LAB_0020eddc;
    }
    if (lbl_8047B614 == 1) {
      fn_801F37B0(0,0x8023011c,0,0);
      uVar6 = lbl_8047B62C;
      uVar2 = lbl_8047B614;
      uVar7 = lbl_8047B610;
      lbl_8047B614 = 0;
      lbl_8047B610 = (u32)(u8 *)&DAT_80378798;
      lbl_8047B62C = uVar6;
      do {
        ((int (*)())*(u32 *)((u32)*(u8*)lbl_8047B610 * 4 + -0x7fd85ff4))();
        if (lbl_8047B614 == 1) break;
      } while ((char)lbl_8047B614 != 2);
      lbl_8047B62C = uVar6;
      lbl_8047B614 = uVar2;
      lbl_8047B610 = uVar7;
      local_37[0] = 1;
      fn_801F37B0(0,0x8022e34c,local_37,0);
      fn_801F37B0(0,0x8022e1f8,0,0);
      fn_801F37B0(0,0x80230088,0,0);
      local_38 = 0;
      fn_801F37B0(0,0x8022eb9c,&local_38,0);
      lbl_8047B614 = 2;
    }
  } while ((char)lbl_8047B614 != 2);
  uVar6 = fn_801F025C(0x11,0);
  uVar7 = (int)fn_8012640C(uVar6,0,0xd9,0);
  fn_801F37B0(0,0x802136a4,0,0);
  lbl_80478D7B = 0;
  lbl_8047B618 = lbl_8047B618 & 0xf1e892af;
  lbl_8047B625 = 0;
  lbl_80478D7C = 0;
  DAT_8038ff5a = 0;
  DAT_8038fff9 = 0;
  fn_801254B4(uVar6,0,0xf3,0,0);
  fn_801254B4(uVar6,0,0xf4,0,9);
  fn_8011BBD8(uVar7,0,0x2d,0,0);
LAB_0020eddc:
  if (r5 != 0) {
    uVar3 = lbl_8047B618;
    lbl_8047B618 = uVar3 & 0xfffffdff;
    lbl_8047B618 = uVar3 & 0xfff7fdff;
  }
  lbl_8047B62C = uVar5;
  lbl_8047B614 = uVar1;
  lbl_8047B610 = uVar4;
  return;
}
/* Address: 0x80211E18 | Size: 0x8ac | Ghidra import */


void fn_80211E18(u32 r3,u32 r4)

{
    extern u32 DAT_80375e24;
    extern u32 DAT_8038ff76;
    extern u32 DAT_8038fffc;
    extern u32 _DAT_80279e80;
    extern u32 _DAT_80279e84;
    extern u32 _DAT_80279e88;
    extern u32 _DAT_80279e8c;
    extern u32 _DAT_80279e94;
    extern u32 _DAT_80375e24;
    extern u32 _DAT_80375e44;
    extern u32 fn_800FA280();
    extern void fn_801299C8();
    extern void fn_80132A38();
    extern s8 fn_801437E0();
    extern s8 fn_80143878();
    extern s8 fn_801438A0();
    extern s8 fn_801438C8();
    extern s8 fn_801438F0();
    extern s8 fn_80143918();
    extern s8 fn_80143940();
    extern s8 fn_80143990();
    extern s8 fn_801439B8();
    extern s8 fn_801439D4();
    extern s8 fn_801439F0();
    extern s8 fn_80143A0C();
    extern s8 fn_80143A28();
    extern s8 fn_80143A44();
    extern u32 fn_80143A94();
    extern void fn_80143DFC();
    extern void fn_801440A0();
    extern void fn_801DA7AC();
    extern void fn_801EF8F4();
    extern u32 fn_801F025C();
    extern u32 fn_801F4354();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u32 fn_801F8000();
    extern u32 fn_801F8100();
    extern u32 fn_801FB1C0();
    extern int fn_802026E4();
    extern void fn_80202810();
    extern void fn_80211B94();
    extern void fn_80261B68();
    extern void fn_80261E7C();
    extern void fn_8026246C();
    extern void fn_80265598();
    extern u8 lbl_80478D7D;
    extern u8 lbl_8047B614;
  u8 bVar1;
  s8 cVar2;
  u16 uVar9;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  int iVar7;
  short sVar10;
  s8 cVar12;
  u32 uVar8;
  s8 cVar13;
  u16 uVar11;

  u32 uVar14;
  u32 uVar15;
  
  uVar9 = fn_801F54A4(0,0,0x14,0);
  uVar3 = fn_801F025C(0x11,0);
  uVar4 = fn_801F025C(0x12,0);
  uVar5 = fn_801F4354(0,uVar3);
  uVar6 = fn_801FB1C0(uVar5,0,0x44,0);
  iVar7 = (int)fn_8012640C(uVar3,0,0xe5,0);
  if (iVar7 != 0) {
    itemGetStatus(iVar7,0,0x1f,0);
    sVar10 = itemGetStatus(iVar7,0,0x20,0);
    cVar12 = itemGetStatus(iVar7,0,0x21,0);
    fn_801440A0(r4);
    fn_80143DFC();
    uVar8 = fn_80143A94();
    cVar2 = DAT_8038fffc;
    bVar1 = DAT_8038ff76;
    cVar13 = fn_802026E4(uVar3,0x2e);
    if (cVar13 == 1) {
      fn_80202810(uVar3,0x2e);
    }
    cVar13 = fn_802026E4(uVar3,0x15);
    if (cVar13 == 1) {
      fn_80202810(uVar3,0x15);
    }
    cVar13 = fn_802026E4(uVar3,0x28);
    if (cVar13 == 1) {
      fn_80202810(uVar3,0x28);
    }
    uVar14 = _DAT_80375e24;
    if (cVar12 != 1) {
      cVar13 = itemGetStatus(0,r4,2,0);
      if (cVar13 == 1) {
        uVar14 = *(u32 *)((r4 & 0xffff) * 4 + -0x7fc8a210);
      }
      else {
        uVar14 = _DAT_80375e44;
        if (((r4 & 0xffff) != 0x50) && ((r4 & 0xffff) != 0x51)) {
          fn_801440A0(r4);
          fn_80143DFC();
          iVar7 = fn_80143A94();
          if (iVar7 == 0) {
            uVar15 = 7;
          }
          else if ((r4 & 0xffff) == 0x13) {
            uVar15 = 1;
          }
          else {
            cVar13 = fn_801437E0();
            if (cVar13 == 0) {
              cVar13 = fn_80143940(iVar7);
              if ((((cVar13 == 1) || (cVar13 = fn_80143918(iVar7), cVar13 == 1)) ||
                  (cVar13 = fn_801438F0(iVar7), cVar13 == 1)) ||
                 (((cVar13 = fn_801438C8(iVar7), cVar13 == 1 ||
                   (cVar13 = fn_801438A0(iVar7), cVar13 == 1)) ||
                  (cVar13 = fn_80143878(iVar7), cVar13 == 1)))) {
                uVar15 = 3;
              }
              else {
                cVar13 = fn_80143A44(iVar7);
                if (cVar13 == 1) {
                  uVar15 = 4;
                }
                else {
                  cVar13 = fn_80143A28(iVar7);
                  if (((cVar13 == 0) && (cVar13 = fn_80143A0C(iVar7), cVar13 == 0)) &&
                     ((cVar13 = fn_801439F0(iVar7), cVar13 == 0 &&
                      ((cVar13 = fn_801439D4(iVar7), cVar13 == 0 &&
                       (cVar13 = fn_801439B8(iVar7), cVar13 == 0)))))) {
                    cVar13 = fn_80143990(iVar7);
                    if (cVar13 == 1) {
                      uVar15 = 6;
                    }
                    else {
                      uVar15 = 7;
                    }
                  }
                  else {
                    uVar15 = 5;
                  }
                }
              }
            }
            else {
              uVar15 = 2;
            }
          }
          uVar14 = _DAT_80375e24;
          if (uVar15 != 7) {
            fn_801F4C14(0,0,0x4b,0,uVar3);
            lbl_80478D7D = 0;
            if (uVar15 == 4) {
              lbl_80478D7D = 5;
            }
            else if (uVar15 < 4) {
              if (2 < uVar15) {
                cVar13 = fn_80143940(uVar8);
                if ((((cVar13 == 1) && (cVar13 = fn_80143918(uVar8), cVar13 == 1)) &&
                    (cVar13 = fn_801438F0(uVar8), cVar13 == 1)) &&
                   ((cVar13 = fn_801438C8(uVar8), cVar13 == 1 &&
                    (cVar13 = fn_801438A0(uVar8), cVar13 == 1)))) {
                  cVar13 = fn_802026E4(uVar3,8);
                  if (cVar13 == 1) {
                    lbl_80478D7D = 5;
                  }
                  else {
                    cVar13 = fn_802026E4(uVar3,3);
                    if (cVar13 == 1) {
                      lbl_80478D7D = 4;
                    }
                    else {
                      cVar13 = fn_802026E4(uVar3,4);
                      if (cVar13 == 1) {
                        lbl_80478D7D = 4;
                      }
                      else {
                        cVar13 = fn_802026E4(uVar3,6);
                        if (cVar13 == 1) {
                          lbl_80478D7D = 3;
                        }
                        else {
                          cVar13 = fn_802026E4(uVar3,7);
                          if (cVar13 == 1) {
                            lbl_80478D7D = 2;
                          }
                          else {
                            cVar13 = fn_802026E4(uVar3,5);
                            if (cVar13 == 1) {
                              lbl_80478D7D = 1;
                            }
                          }
                        }
                      }
                    }
                  }
                }
                else {
                  cVar13 = fn_80143940(uVar8);
                  if (cVar13 == 1) {
                    lbl_80478D7D = 5;
                  }
                  else {
                    cVar13 = fn_80143918(uVar8);
                    if (cVar13 == 1) {
                      lbl_80478D7D = 4;
                    }
                    else {
                      cVar13 = fn_801438F0(uVar8);
                      if (cVar13 == 1) {
                        lbl_80478D7D = 3;
                      }
                      else {
                        cVar13 = fn_801438C8(uVar8);
                        if (cVar13 == 1) {
                          lbl_80478D7D = 2;
                        }
                        else {
                          cVar13 = fn_801438A0(uVar8);
                          if (cVar13 == 1) {
                            lbl_80478D7D = 1;
                          }
                          else {
                            cVar13 = fn_80143878(uVar8);
                            if (cVar13 == 1) {
                              lbl_80478D7D = 0;
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
            else if ((uVar15 != 6) && (uVar15 < 6)) {
              lbl_80478D7D = 4;
              uVar11 = fn_80143A28(uVar8);
              uVar11 = uVar11 & 0xff;
              if (uVar11 == 0) {
                uVar11 = fn_80143A0C(uVar8);
                uVar11 = uVar11 & 0xff;
                if (uVar11 == 0) {
                  uVar11 = fn_801439F0(uVar8);
                  uVar11 = uVar11 & 0xff;
                  if (uVar11 == 0) {
                    uVar11 = fn_801439D4(uVar8);
                    uVar11 = uVar11 & 0xff;
                    if (uVar11 == 0) {
                      uVar11 = fn_801439B8(uVar8);
                      uVar11 = uVar11 & 0xff;
                      if (uVar11 != 0) {
                        uVar3 = fn_800FA280(_DAT_80279e8c);
                        fn_80132A38(0xd,uVar3);
                        DAT_8038ff76 = 4;
                      }
                    }
                    else {
                      uVar3 = fn_800FA280(_DAT_80279e94);
                      fn_80132A38(0xd,uVar3);
                      DAT_8038ff76 = 6;
                    }
                  }
                  else {
                    uVar3 = fn_800FA280(_DAT_80279e88);
                    fn_80132A38(0xd,uVar3);
                    DAT_8038ff76 = 3;
                  }
                }
                else {
                  uVar3 = fn_800FA280(_DAT_80279e84);
                  fn_80132A38(0xd,uVar3);
                  DAT_8038ff76 = 2;
                }
              }
              else {
                uVar3 = fn_800FA280(_DAT_80279e80);
                fn_80132A38(0xd,uVar3);
                DAT_8038ff76 = 1;
              }
              if ((uVar11 == 1) || (uVar11 == 0xffff)) {
                uVar3 = fn_800FA280(0x76bd);
                fn_80132A38(0xe,uVar3);
                DAT_8038fffc = (DAT_8038ff76 & 0xf) + 0xe;
              }
              else {
                uVar3 = fn_800FA280(0x7626);
                fn_80132A38(0xe,uVar3);
                DAT_8038fffc = (DAT_8038ff76 & 0xf) + 0x26;
              }
              uVar3 = fn_800FA280(0x7627);
              fn_80132A38(0x41,uVar3);
            }
            uVar14 = *(u32 *)(&DAT_80375e24 + uVar15 * 4);
          }
        }
      }
    }
    lbl_8047B614 = 0;
    uVar3 = fn_801F8000(uVar5);
    fn_80132A38(0x22,uVar3);
    uVar3 = fn_801F8100(uVar5);
    fn_80132A38(0x23,uVar3);
    uVar3 = fn_801F8100(uVar5);
    fn_80132A38(0x13,uVar3);
    itemGetStatus(0,r4 & 0xffff,1,0);
    uVar3 = fn_800FA280();
    fn_80132A38(0x29,uVar3);
    fn_801EF8F4(1);
    if (cVar12 == 0) {
      fn_80265598(uVar4,uVar9,1);
    }
    fn_80211B94(r3,uVar14,1);
    if (cVar12 == 0) {
      fn_801299C8(uVar6,r4,1,(int)sVar10);
    }
    DAT_8038ff76 = bVar1;
    DAT_8038fffc = cVar2;
    fn_801DA7AC();
    fn_80261B68(0);
    fn_80261E7C(0);
    fn_8026246C();
  }
  return;
}
