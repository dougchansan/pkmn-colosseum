/**
 * @file trainer.c
 * @brief Trainer data access and party management interface.
 *
 * =========================================================================
 * SUBSYSTEM ANALYSIS
 * =========================================================================
 *
 * Address range: 0x801F7F80 - 0x80201800
 * Total functions: ~220 (including the dense accessor block at 0x801FC000-0x801FD000)
 * Total code size: ~38KB
 *
 * This file implements the trainer data access layer. The core function
 * TrainerDataGet (fn_801FB1C0) is the most frequently called function in
 * this range with 883 call sites, making it one of the most critical
 * interfaces in the entire game.
 *
 * KEY FUNCTIONS:
 *
 *   fn_801FB1C0 (TrainerDataGet) - 883 calls, 0x724 bytes (0x801FB1C0-0x801FB8E4)
 *     Two-phase dispatch:
 *     Phase 1 - Category resolution (r5 = field ID):
 *       0x01-0x09: Battle trainer (fn_801FCCC4)
 *       0x0A-0x0C: Party configuration (fn_801FCAD0)
 *       0x0D-0x1D: Team roster (fn_801FCA2C)
 *       0x1E-0x3C: Story/event data (fn_801FC658)
 *       0x3D-0x41: Misc attributes (fn_801FBFBC)
 *     Phase 2 - Field dispatch via jumptable_803757D8 (86 entries)
 *
 *   fn_801FAA58 (TrainerDataSet) - 169 calls
 *     Mirror of TrainerDataGet for writing values.
 *
 *   fn_80205B8C (GetTrainerPokemonPtr) - 668 calls, 0x58 bytes
 *     Navigates trainer -> party -> Pokemon by calling fn_8012640C twice:
 *       First call:  field=0xD6 (get party list)
 *       Second call: field=0xCC (get Pokemon from party slot)
 *     Returns the Pokemon data pointer, or NULL on any failure.
 *
 *   fn_80205BE8 (GetTrainerPokemonPtrSingle) - related helper
 *     Single-step version: just calls fn_8012640C with field=0xCC.
 *
 * TRAINER CATEGORY SUB-DISPATCHERS:
 *   fn_801FCCC4: Resolves battle trainer context. Likely reads from the
 *                active battle's trainer slot data.
 *   fn_801FCAD0: Resolves party configuration. Returns a sub-struct
 *                pointer used to access party layout info.
 *   fn_801FCA2C: Resolves team roster. Used for fields related to the
 *                full team of 6 Pokemon.
 *   fn_801FC658: Resolves story/event data. This bridges the trainer
 *                system to the story progression flags.
 *   fn_801FBFBC: Resolves miscellaneous trainer attributes (name, class,
 *                AI flags, etc.).
 *
 * DENSE ACCESSOR BLOCK (0x801FC000-0x801FD000, ~285 functions):
 *   This region contains 149+136 tiny functions that are individual field
 *   accessors for the trainer/party structure. They follow the same
 *   patterns as the Pokemon field accessors (null-check, offset load).
 *   The sheer density (285 functions in 8KB) indicates these are
 *   compiler-generated struct member access functions.
 *
 * EVENT INTEGRATION (0x801FE000-0x80200A8C):
 *   fn_801FE7EC (49 calls): Set event/story state on a trainer
 *   fn_801FECD4 (59 calls): Check event/story state on a trainer
 *   fn_801FE710 (5 calls):  Clear event state
 *
 * COMMONLY CALLED EXTERNAL FUNCTIONS:
 *   fn_8012640C (1769 total calls): Master data table resolver.
 *     This is THE core data access primitive for the entire game.
 *     It takes (pointer, slot, tableID, flags) and resolves a pointer
 *     into the common_rel data tables. Table IDs seen:
 *       0xCC = Pokemon in party slot
 *       0xD6 = Party/trainer data
 *       0x79 = Secondary Pokemon data
 *       0xE5 = Extended trainer info
 *
 *   fn_801254B4 (544 total calls): Data table write accessor.
 *     Mirror of fn_8012640C for writing values.
 *
 * BSS STATE:
 *   lbl_8047B610 : u32, trainer system program counter / script position
 *     This is read/written very frequently (lwz/stw with addi +1/+5)
 *     suggesting it's a script or event sequence counter.
 *
 * =========================================================================
 */

#include "game/trainer.h"
#include "game/pokemon.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

/* fn_8012640C - Master data table resolver
 * The most-called function in the entire game (1769 calls).
 * Takes a context pointer, slot index, table ID, and flags.
 * Returns a pointer to the resolved data, or NULL. */
extern void* fn_8012640C(void* context, u32 slot, u16 tableId, u32 flags);

/* fn_801254B4 - Master data table writer (544 calls) */
extern u32 fn_801254B4(void* context, u32 slot, u16 tableId, u32 flags, u32 value);

/* fn_80125424 - Data table auxiliary writer */
extern void fn_80125424(void* context, u32 value);

/* itemGetStatus - Secondary data accessor (169 calls) */
extern u32 itemGetStatus(u32 context, u32 param, u16 field, u32 flags);

/* fn_801F02AC - PokemonSlotLookup (89 calls) */
extern u32 fn_801F02AC(u32 type, void* ptr, u32 param);

/* Category resolution sub-dispatchers */
extern void* fn_801FCCC4(u32 slot); /* Battle trainer */
extern void* fn_801FCAD0(u32 slot); /* Party config */
extern void* fn_801FCA2C(u32 slot); /* Team roster */
extern void* fn_801FC658(u32 slot); /* Story/event data */
extern void* fn_801FBFBC(u32 slot); /* Misc attributes */

/* Event integration */
extern void fn_801FE7EC(void* trainer, u32 eventId, u32 param1, u32 param2);
extern u8   fn_801FECD4(void* trainer);
extern void fn_801FE710(void* trainer, u32 eventId, u32 param);

/* SDA table pointers for trainer data arrays */
extern u32* lbl_80478F08;  /* Party config header */
extern u8*  lbl_80478F0C;  /* Party config data */
extern u32* lbl_80478F10;  /* Team roster header */
extern u8*  lbl_80478F14;  /* Team roster data */
extern u32* lbl_80478F18;  /* Slot data header */
extern u8*  lbl_80478F1C;  /* Slot data */
extern u32* lbl_80478F20;  /* Battle trainer header */
extern u8*  lbl_80478F24;  /* Battle trainer data */
extern u32* lbl_80478F28;  /* Misc attributes header */
extern u8*  lbl_80478F2C;  /* Misc attributes data */
extern u32* lbl_80478F30;  /* Story/event header */
extern u8*  lbl_80478F34;  /* Story/event data */

/* ===== Index lookup globals ===== */
/* lbl_80478F18 already declared above as u32* */

/* Forward declarations for converted functions */
u16 fn_801FCC94(u8* ptr);
u8 fn_801FCCAC(u8* ptr);
void fn_801FAA58(void);
void fn_801FB1C0(void);

/* =========================================================================
 * fn_80205B8C - GetTrainerPokemonPtr
 *
 * Navigate from a trainer/party context to a Pokemon data pointer.
 * This is the third most-called function in the range (668 calls).
 *
 * The function performs two hops through the data table system:
 *   1. context -> party list (table 0xD6)
 *   2. party list -> specific Pokemon (table 0xCC)
 *
 * @param context  Trainer or party context pointer
 * @return         Pokemon data pointer, or NULL if either hop fails
 * ========================================================================= */
void* GetTrainerPokemonPtr(void* context) {
    void* partyList;
    if (context == 0) {
        return NULL;
    }

    /* First hop: get party list from trainer context */
    partyList = fn_8012640C(context, 0, 0xD6, 0);
    if (partyList == 0) {
        return NULL;
    }

    /* Second hop: get Pokemon from party list */
    return fn_8012640C(partyList, 0, 0xCC, 0);
}

/* =========================================================================
 * fn_80205BE8 - GetTrainerPokemonPtrSingle
 *
 * Single-hop version of GetTrainerPokemonPtr. Only does the CC lookup.
 *
 * @param context  Party context pointer (already resolved to party level)
 * @return         Pokemon data pointer, or NULL
 * ========================================================================= */
void* GetTrainerPokemonPtrSingle(void* context) {
    if (context == 0) {
        return NULL;
    }
    return fn_8012640C(context, 0, 0xCC, 0);
}

/* =========================================================================
 * fn_801FB1C0 - TrainerDataGet
 *
 * Core trainer data dispatch. This function is called 883 times and is
 * the primary interface for reading any trainer-related data.
 *
 * The two-phase dispatch:
 *
 * Phase 1 - Determine which sub-object to access:
 *   if (field == 0 || field >= 0x5B) return 0;
 *   if (field < 0x0A) ptr = fn_801FCCC4(slot); // battle trainer
 *   else if (field < 0x0D) ptr = fn_801FCAD0(slot); // party config
 *   else if (field < 0x1E) ptr = fn_801FCA2C(slot); // team roster
 *   else if (field < 0x3D) ptr = fn_801FC658(slot); // story/event
 *   else if (field < 0x42) ptr = fn_801FBFBC(slot); // misc
 *   if (ptr == NULL) return 0;
 *
 * Phase 2 - Dispatch on field ID through jumptable_803757D8 (86 entries):
 *   Each entry calls the appropriate getter. Example cases:
 *     Case 0x00: fn_801FCCAC (get trainer is-valid flag, u8)
 *     Case 0x01: fn_801FCC94 (get trainer name ptr, u16)
 *     Case 0x02-0x09: Various battle trainer properties
 *     ...
 *     Case 0x42-0x56: Team/party aggregate queries
 *
 * [Full decompilation requires analysis of 86 jumptable entries]
 * ========================================================================= */
/* TODO: Decompile fn_801FB1C0 (0x724 bytes, jumptable_803757D8) */

/* =========================================================================
 * fn_801FAA58 - TrainerDataSet
 *
 * Write-side counterpart to TrainerDataGet. Same category dispatch logic,
 * but uses a setter jumptable. 169 call sites.
 *
 * [Assembly stub - structure mirrors TrainerDataGet]
 * ========================================================================= */
/* TODO: Decompile fn_801FAA58 */

/* =========================================================================
 * fn_80236BFC - CheckTrainerPokemonFlag
 *
 * A higher-level helper that chains three TrainerDataGet calls:
 *   1. TrainerDataGet(r3, 0, 0x43, 0) -> Get Pokemon ptr from trainer
 *   2. TrainerDataGet(0, result, 0x02, 0) -> Get species or similar
 *   3. TrainerDataGet(0, result, 0x24, 0) -> Check shadow/special flag
 * If the shadow check returns 1, calls fn_802026E4 for the actual flag test.
 *
 * This pattern (resolve trainer -> resolve pokemon -> check property)
 * is the standard idiom throughout the script/event system.
 *
 * 272 call sites, making this one of the most important utility functions.
 * ========================================================================= */
/* TODO: Decompile fn_80236BFC (0x80 bytes) */

/* =========================================================================
 * Trainer accessor functions (0x801FC000 - 0x801FD000)
 *
 * This region contains 285 tiny functions that are individual field
 * getters/setters for various trainer sub-structures. They are
 * organized in interleaved get/set pairs.
 *
 * Due to the extreme density and repetitive nature, these are best
 * decompiled by identifying the structure layout first, then
 * generating the accessors from the struct definition.
 *
 * Notable sub-structures accessed:
 *   - Battle trainer data (via fn_801FCCC4)
 *   - Party configuration (via fn_801FCAD0)
 *   - Team roster entries (via fn_801FCA2C)
 *   - Story/event flags (via fn_801FC658)
 *   - Misc attributes (via fn_801FBFBC)
 * ========================================================================= */

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 256 functions matched
 * =================================================================== */

/* Address: 0x801FBD10 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FBD10(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x8]);
}

/* Address: 0x801FBD28 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FBD28(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x801FBD40 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FBD40(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x0]);
}

/* Address: 0x801FBD84 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBD84(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x12]) = val;
}

/* Address: 0x801FBD94 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBD94(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x11]) = val;
}

/* Address: 0x801FBDA4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBDA4(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x10]) = val;
}

/* Address: 0x801FBDB4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBDB4(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xF]) = val;
}

/* Address: 0x801FBDC4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBDC4(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xE]) = val;
}

/* Address: 0x801FBDD4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBDD4(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0xC]) = val;
}

/* Address: 0x801FBDE4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBDE4(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0xA]) = val;
}

/* Address: 0x801FBE18 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBE18(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x0]) = val;
}

/* Address: 0x801FBE28 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FBE28(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x12]);
}

/* Address: 0x801FBE40 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FBE40(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x11]);
}

/* Address: 0x801FBE58 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FBE58(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x10]);
}

/* Address: 0x801FBE70 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FBE70(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xF]);
}

/* Address: 0x801FBE88 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FBE88(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xE]);
}

/* Address: 0x801FBEA0 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FBEA0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0xC]);
}

/* Address: 0x801FBEB8 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FBEB8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0xA]);
}

/* Address: 0x801FBF04 | Size: 0x18 | Pattern: nullcheck_getter */
s16 fn_801FBF04(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(s16*)(&ptr[0x0]);
}

/* Address: 0x801FBF1C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBF1C(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x8]) = val;
}

/* Address: 0x801FBF2C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBF2C(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x4]) = val;
}

/* Address: 0x801FBF3C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBF3C(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0xC]) = val;
}

/* Address: 0x801FBF4C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBF4C(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x0]) = val;
}

/* Address: 0x801FBF5C | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FBF5C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x8]);
}

/* Address: 0x801FBF74 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FBF74(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x801FBF8C | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FBF8C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0xC]);
}

/* Address: 0x801FBFA4 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FBFA4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x0]);
}

/* Address: 0x801FBFE8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBFE8(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x19]) = val;
}

/* Address: 0x801FBFF8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FBFF8(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x18]) = val;
}

/* Address: 0x801FC008 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC008(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x17]) = val;
}

/* Address: 0x801FC018 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC018(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x16]) = val;
}

/* Address: 0x801FC028 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC028(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x15]) = val;
}

/* Address: 0x801FC0C0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC0C0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x14]) = val;
}

/* Address: 0x801FC0D0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC0D0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x13]) = val;
}

/* Address: 0x801FC0E0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC0E0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x12]) = val;
}

/* Address: 0x801FC0F0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC0F0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x11]) = val;
}

/* Address: 0x801FC100 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC100(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x10]) = val;
}

/* Address: 0x801FC110 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC110(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xF]) = val;
}

/* Address: 0x801FC120 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC120(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xE]) = val;
}

/* Address: 0x801FC130 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC130(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xD]) = val;
}

/* Address: 0x801FC140 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC140(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xC]) = val;
}

/* Address: 0x801FC1D8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC1D8(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xB]) = val;
}

/* Address: 0x801FC1E8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC1E8(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xA]) = val;
}

/* Address: 0x801FC1F8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC1F8(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x9]) = val;
}

/* Address: 0x801FC208 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC208(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x8]) = val;
}

/* Address: 0x801FC218 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC218(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x7]) = val;
}

/* Address: 0x801FC228 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC228(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x6]) = val;
}

/* Address: 0x801FC238 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC238(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x5]) = val;
}

/* Address: 0x801FC248 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC248(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x4]) = val;
}

/* Address: 0x801FC258 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC258(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x3]) = val;
}

/* Address: 0x801FC268 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC268(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x2]) = val;
}

/* Address: 0x801FC278 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC278(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x1]) = val;
}

/* Address: 0x801FC288 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC288(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x0]) = val;
}

/* Address: 0x801FC298 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC298(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x19]);
}

/* Address: 0x801FC2B0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC2B0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x18]);
}

/* Address: 0x801FC2C8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC2C8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x17]);
}

/* Address: 0x801FC2E0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC2E0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x16]);
}

/* Address: 0x801FC2F8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC2F8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x15]);
}

/* Address: 0x801FC3B8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC3B8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x14]);
}

/* Address: 0x801FC3D0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC3D0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x13]);
}

/* Address: 0x801FC3E8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC3E8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x12]);
}

/* Address: 0x801FC400 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC400(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x11]);
}

/* Address: 0x801FC418 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC418(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x10]);
}

/* Address: 0x801FC430 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC430(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xF]);
}

/* Address: 0x801FC448 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC448(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xE]);
}

/* Address: 0x801FC460 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC460(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xD]);
}

/* Address: 0x801FC478 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC478(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xC]);
}

/* Address: 0x801FC538 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC538(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xB]);
}

/* Address: 0x801FC550 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC550(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xA]);
}

/* Address: 0x801FC568 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC568(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x9]);
}

/* Address: 0x801FC580 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC580(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x8]);
}

/* Address: 0x801FC598 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC598(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x7]);
}

/* Address: 0x801FC5B0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC5B0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x6]);
}

/* Address: 0x801FC5C8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC5C8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x5]);
}

/* Address: 0x801FC5E0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC5E0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x4]);
}

/* Address: 0x801FC5F8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC5F8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x3]);
}

/* Address: 0x801FC610 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC610(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x2]);
}

/* Address: 0x801FC628 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC628(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x1]);
}

/* Address: 0x801FC640 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC640(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* Address: 0x801FC684 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC684(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x6]) = val;
}

/* Address: 0x801FC694 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC694(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x5]) = val;
}

/* Address: 0x801FC6A4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC6A4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x6]);
}

/* Address: 0x801FC6BC | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC6BC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x5]);
}

/* Address: 0x801FC6D4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC6D4(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x2]) = val;
}

/* Address: 0x801FC6E4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC6E4(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x1]) = val;
}

/* Address: 0x801FC6F4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC6F4(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x8]) = val;
}

/* Address: 0x801FC784 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC784(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x10]) = val;
}

/* Address: 0x801FC794 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC794(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0xA]) = val;
}

/* Address: 0x801FC7A4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC7A4(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x0]) = val;
}

/* Address: 0x801FC7B4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC7B4(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x3]) = val;
}

/* Address: 0x801FC7C4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC7C4(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0xC]) = val;
}

/* Address: 0x801FC7D4 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC7D4(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x4]) = val;
}

/* Address: 0x801FC828 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FC828(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x14]) = val;
}

/* Address: 0x801FC930 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FC930(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0xA]);
}

/* Address: 0x801FC964 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC964(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x3]);
}

/* Address: 0x801FC97C | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FC97C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0xC]);
}

/* Address: 0x801FC994 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FC994(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x4]);
}

/* Address: 0x801FCA14 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FCA14(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x14]);
}

/* Address: 0x801FCA78 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCA78(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x0]) = val;
}

/* Address: 0x801FCAB8 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FCAB8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x0]);
}

/* Address: 0x801FCAFC | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCAFC(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x10]) = val;
}

/* Address: 0x801FCB30 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCB30(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0xC]) = val;
}

/* Address: 0x801FCB64 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCB64(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x4]) = val;
}

/* Address: 0x801FCB74 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCB74(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x8]) = val;
}

/* Address: 0x801FCB84 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCB84(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x6]) = val;
}

/* Address: 0x801FCB94 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCB94(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x0]) = val;
}

/* Address: 0x801FCBA4 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FCBA4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x10]);
}

/* Address: 0x801FCBF0 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FCBF0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0xC]);
}

/* Address: 0x801FCC3C | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FCC3C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x4]);
}

/* Address: 0x801FCC54 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCC54(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x2]) = val;
}

/* Address: 0x801FCC64 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FCC64(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x2]);
}

/* Address: 0x801FCC7C | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FCC7C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x8]);
}

/* Address: 0x801FCD08 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCD08(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x27BC]) = val;
}

/* Address: 0x801FCD18 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCD18(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x27B5]) = val;
}

/* Address: 0x801FCD28 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCD28(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x27B8]) = val;
}

/* Address: 0x801FCD38 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCD38(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x27B4]) = val;
}

/* Address: 0x801FCD48 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCD48(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x0]) = val;
}

/* Address: 0x801FCD8C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCD8C(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x27C0]) = val;
}

/* Address: 0x801FCD9C | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FCD9C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x27C0]);
}

/* Address: 0x801FCDB4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FCDB4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x27BC]);
}

/* Address: 0x801FCDCC | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FCDCC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x27B5]);
}

/* Address: 0x801FCDE4 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FCDE4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x27B8]);
}

/* Address: 0x801FCDFC | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FCDFC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x27B4]);
}

/* Address: 0x801FCE94 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FCE94(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x0]);
}

/* Address: 0x801FCEFC | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCEFC(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x678]) = val;
}

/* Address: 0x801FCF0C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCF0C(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x676]) = val;
}

/* Address: 0x801FCF1C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCF1C(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x674]) = val;
}

/* Address: 0x801FCF2C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCF2C(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x672]) = val;
}

/* Address: 0x801FCF3C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCF3C(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x670]) = val;
}

/* Address: 0x801FCF4C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCF4C(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x66E]) = val;
}

/* Address: 0x801FCF5C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCF5C(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x66C]) = val;
}

/* Address: 0x801FCF6C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCF6C(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x66A]) = val;
}

/* Address: 0x801FCF7C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FCF7C(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x668]) = val;
}

/* Address: 0x801FCFA4 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FCFA4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x678]);
}

/* Address: 0x801FCFBC | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FCFBC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x676]);
}

/* Address: 0x801FCFD4 | Size: 0x18 | Pattern: nullcheck_getter */
s16 fn_801FCFD4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(s16*)(&ptr[0x674]);
}

/* Address: 0x801FD004 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD004(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x670]);
}

/* Address: 0x801FD01C | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD01C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x66E]);
}

/* Address: 0x801FD034 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD034(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x66C]);
}

/* Address: 0x801FD04C | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD04C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x66A]);
}

/* Address: 0x801FD064 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD064(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x668]);
}

/* Address: 0x801FD07C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD07C(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x8]) = val;
}

/* Address: 0x801FD08C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD08C(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x6]) = val;
}

/* Address: 0x801FD09C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD09C(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x4]) = val;
}

/* Address: 0x801FD0AC | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD0AC(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x0]) = val;
}

/* Address: 0x801FD0BC | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD0BC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x8]);
}

/* Address: 0x801FD0D4 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD0D4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x6]);
}

/* Address: 0x801FD0EC | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD0EC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x4]);
}

/* Address: 0x801FD104 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FD104(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x0]);
}

/* Address: 0x801FD150 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD150(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x6AE]) = val;
}

/* Address: 0x801FD178 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD178(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x6AC]) = val;
}

/* Address: 0x801FD188 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD188(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x6AC]);
}

/* Address: 0x801FD1A0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD1A0(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x6AA]) = val;
}

/* Address: 0x801FD1B0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD1B0(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x6A8]) = val;
}

/* Address: 0x801FD1C0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD1C0(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x6A6]) = val;
}

/* Address: 0x801FD1D0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD1D0(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x6A4]) = val;
}

/* Address: 0x801FD1E0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD1E0(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x6A0]) = val;
}

/* Address: 0x801FD1F0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD1F0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x69E]) = val;
}

/* Address: 0x801FD200 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD200(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x69D]) = val;
}

/* Address: 0x801FD210 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD210(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x69C]) = val;
}

/* Address: 0x801FD220 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD220(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x69B]) = val;
}

/* Address: 0x801FD230 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD230(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x69A]) = val;
}

/* Address: 0x801FD240 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD240(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x699]) = val;
}

/* Address: 0x801FD250 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD250(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x698]) = val;
}

/* Address: 0x801FD260 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD260(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x697]) = val;
}

/* Address: 0x801FD270 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD270(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x696]) = val;
}

/* Address: 0x801FD280 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD280(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x695]) = val;
}

/* Address: 0x801FD290 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD290(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x694]) = val;
}

/* Address: 0x801FD2A0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD2A0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x693]) = val;
}

/* Address: 0x801FD2B0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD2B0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x692]) = val;
}

/* Address: 0x801FD2C0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD2C0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x691]) = val;
}

/* Address: 0x801FD2D0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD2D0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x690]) = val;
}

/* Address: 0x801FD2E0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD2E0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x68F]) = val;
}

/* Address: 0x801FD2F0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD2F0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x68E]) = val;
}

/* Address: 0x801FD300 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD300(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x68D]) = val;
}

/* Address: 0x801FD310 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD310(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x68C]) = val;
}

/* Address: 0x801FD320 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD320(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x68B]) = val;
}

/* Address: 0x801FD330 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD330(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x68A]) = val;
}

/* Address: 0x801FD340 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD340(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x6AA]);
}

/* Address: 0x801FD358 | Size: 0x18 | Pattern: nullcheck_getter */
s16 fn_801FD358(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(s16*)(&ptr[0x6A8]);
}

/* Address: 0x801FD370 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD370(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x6A6]);
}

/* Address: 0x801FD388 | Size: 0x18 | Pattern: nullcheck_getter */
s16 fn_801FD388(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(s16*)(&ptr[0x6A4]);
}

/* Address: 0x801FD3A0 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FD3A0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x6A0]);
}

/* Address: 0x801FD3B8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD3B8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x69E]);
}

/* Address: 0x801FD3D0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD3D0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x69D]);
}

/* Address: 0x801FD3E8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD3E8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x69C]);
}

/* Address: 0x801FD400 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD400(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x69B]);
}

/* Address: 0x801FD418 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD418(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x69A]);
}

/* Address: 0x801FD430 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD430(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x699]);
}

/* Address: 0x801FD448 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD448(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x698]);
}

/* Address: 0x801FD460 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD460(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x697]);
}

/* Address: 0x801FD478 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD478(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x696]);
}

/* Address: 0x801FD490 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD490(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x695]);
}

/* Address: 0x801FD4A8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD4A8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x694]);
}

/* Address: 0x801FD4C0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD4C0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x693]);
}

/* Address: 0x801FD4D8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD4D8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x692]);
}

/* Address: 0x801FD4F0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD4F0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x691]);
}

/* Address: 0x801FD508 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD508(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x690]);
}

/* Address: 0x801FD520 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD520(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x68F]);
}

/* Address: 0x801FD538 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD538(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x68E]);
}

/* Address: 0x801FD550 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD550(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x68D]);
}

/* Address: 0x801FD568 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD568(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x68C]);
}

/* Address: 0x801FD580 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD580(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x68B]);
}

/* Address: 0x801FD598 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD598(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x68A]);
}

/* Address: 0x801FD5C8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD5C8(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x660]) = val;
}

/* Address: 0x801FD5D8 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD5D8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x660]);
}

/* Address: 0x801FD6B8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD6B8(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x688]) = val;
}

/* Address: 0x801FD6C8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD6C8(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x686]) = val;
}

/* Address: 0x801FD6D8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD6D8(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x684]) = val;
}

/* Address: 0x801FD6E8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD6E8(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x682]) = val;
}

/* Address: 0x801FD6F8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD6F8(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x67E]) = val;
}

/* Address: 0x801FD708 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD708(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x680]) = val;
}

/* Address: 0x801FD718 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD718(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x67C]) = val;
}

/* Address: 0x801FD728 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD728(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x67A]) = val;
}

/* Address: 0x801FD738 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD738(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x688]);
}

/* Address: 0x801FD750 | Size: 0x18 | Pattern: nullcheck_getter */
s16 fn_801FD750(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(s16*)(&ptr[0x686]);
}

/* Address: 0x801FD768 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD768(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x684]);
}

/* Address: 0x801FD780 | Size: 0x18 | Pattern: nullcheck_getter */
s16 fn_801FD780(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(s16*)(&ptr[0x682]);
}

/* Address: 0x801FD798 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD798(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x67E]);
}

/* Address: 0x801FD7B0 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD7B0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x680]);
}

/* Address: 0x801FD7C8 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD7C8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x67C]);
}

/* Address: 0x801FD7E0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD7E0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x67A]);
}

/* Address: 0x801FD7F8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD7F8(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x600]) = val;
}

/* Address: 0x801FD808 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FD808(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x600]);
}

/* Address: 0x801FD820 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD820(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x610]) = val;
}

/* Address: 0x801FD830 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD830(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x60C]) = val;
}

/* Address: 0x801FD840 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD840(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x60A]) = val;
}

/* Address: 0x801FD850 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD850(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x609]) = val;
}

/* Address: 0x801FD860 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD860(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x608]) = val;
}

/* Address: 0x801FD870 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD870(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x607]) = val;
}

/* Address: 0x801FD880 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD880(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x606]) = val;
}

/* Address: 0x801FD890 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD890(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x605]) = val;
}

/* Address: 0x801FD8A0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD8A0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x604]) = val;
}

/* Address: 0x801FD8B0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD8B0(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x4]) = val;
}

/* Address: 0x801FD8C0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD8C0(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x0]) = val;
}

/* Address: 0x801FD8D0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD8D0(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x151]) = val;
}

/* Address: 0x801FD8E0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD8E0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x151]);
}

/* Address: 0x801FD8F8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD8F8(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x150]) = val;
}

/* Address: 0x801FD908 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD908(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x14F]) = val;
}

/* Address: 0x801FD918 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD918(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x14E]) = val;
}

/* Address: 0x801FD928 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD928(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x14C]) = val;
}

/* Address: 0x801FD938 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801FD938(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x0]) = val;
}

/* Address: 0x801FD948 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FD948(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x610]);
}

/* Address: 0x801FD960 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD960(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x60A]);
}

/* Address: 0x801FD978 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD978(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x609]);
}

/* Address: 0x801FD990 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD990(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x608]);
}

/* Address: 0x801FD9A8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD9A8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x607]);
}

/* Address: 0x801FD9C0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD9C0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x606]);
}

/* Address: 0x801FD9D8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD9D8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x605]);
}

/* Address: 0x801FD9F0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FD9F0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x604]);
}

/* Address: 0x801FDA84 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FDA84(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x801FDA9C | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FDA9C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x0]);
}

/* Address: 0x801FDAB4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FDAB4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x150]);
}

/* Address: 0x801FDACC | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FDACC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x14F]);
}

/* Address: 0x801FDAE4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FDAE4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x14E]);
}

/* Address: 0x801FDB60 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801FDB60(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x0]);
}

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 2 functions matched
 * =================================================================== */

/* Address: 0x801FCC94 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801FCC94(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x6]);
}

/* Address: 0x801FCCAC | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801FCCAC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* #######################################################################
 * COVERAGE STUBS: Trainer data access (0x801F7F80 - 0x80201800)
 * 123 functions remaining for full coverage of trainer.c TU.
 *
 * Key functions in this range:
 *   fn_801FB1C0 (TrainerDataGet) - 883 calls, 0x724 bytes
 *   fn_801FAA58 (TrainerDataSet) - 169 calls, 0x768 bytes
 *   fn_801FCCC4 (BattleTrainerResolve) - category dispatcher
 *   fn_801FCAD0 (PartyConfigResolve) - party config sub-dispatch
 *   fn_801FCA2C (TeamRosterResolve) - team roster sub-dispatch
 *   fn_801FC658 (StoryDataResolve) - story/event sub-dispatch
 *   fn_801FBFBC (MiscAttrResolve) - misc attribute sub-dispatch
 *   fn_801FE7EC (SetTrainerEventState) - 49 calls
 *   fn_801FECD4 (CheckTrainerEventState) - 59 calls
 * ####################################################################### */

/* 0x801F7F80 | size: 0x80 */
void* fn_801F7F80(void* context, u16 count) {
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 index);
    extern u8 fn_80206A04(void* ptr);
    int i;
    void* result;

    if (context == NULL) {
        return NULL;
    }
    for (i = 0; (u16)i < count; i++) {
        result = fn_801FB1C0(context, 0, 0x45, i);
        if ((u8)fn_80206A04(result) == 0) {
            return result;
        }
    }
    return NULL;
}

/* 0x801F8000 | size: 0x100 */
u32 fn_801F8000(void* context) {
    extern u16 fn_801EF634(void* ctx);
    extern u8 fn_8012A130(void* ptr);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 index);
    extern u8* fn_801FBD58(u16 val);
    extern u32 fn_801FBD28(u8* ptr);
    extern u32 fn_800FA280(u32 val);
    int valid;
    void* result;
    u16 slot;

    if (context == NULL) {
        return 0;
    }
    if (context == NULL) {
        valid = 0;
    } else {
        if ((u16)fn_801EF634(context) == 1) {
            valid = 0;
        } else if ((s32)fn_801FB1C0(context, 0, 0x43, 0) == 0) {
            valid = 0;
        } else {
            result = fn_801FB1C0(context, 0, 0x44, 0);
            if (result == NULL) {
                valid = 0;
            } else if ((u8)fn_8012A130(result) == 0) {
                valid = 0;
            } else {
                valid = 1;
            }
        }
    }
    if ((u8)valid == 0) {
        return 0;
    }
    slot = (u16)(u32)fn_801FB1C0(context, 0, 0x43, 0);
    result = fn_801FB1C0(0, slot, 0x4, 0);
    result = fn_801FBD58((u16)(u32)result);
    return fn_800FA280(fn_801FBD28((u8*)result));
}

/* 0x801F8100 | size: 0xF8 */
void* fn_801F8100(void* context) {
    extern u16 fn_801EF634(void* ctx);
    extern u8 fn_8012A130(void* ptr);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 index);
    extern void* fn_8012A5B0(void* ptr, u32 param1, u32 param2);
    int valid;
    void* result;

    if (context == NULL) {
        return NULL;
    }
    if (context == NULL) {
        valid = 0;
    } else {
        if ((u16)fn_801EF634(context) == 1) {
            valid = 0;
        } else if ((s32)fn_801FB1C0(context, 0, 0x43, 0) == 0) {
            valid = 0;
        } else {
            result = fn_801FB1C0(context, 0, 0x44, 0);
            if (result == NULL) {
                valid = 0;
            } else if ((u8)fn_8012A130(result) == 0) {
                valid = 0;
            } else {
                valid = 1;
            }
        }
    }
    if ((u8)valid == 0) {
        return NULL;
    }
    result = fn_801FB1C0(context, 0, 0x44, 0);
    if (result == NULL) {
        return NULL;
    }
    return fn_8012A5B0(result, 1, 0);
}

/* 0x801F81F8 | size: 0x22C | large */
void fn_801F81F8(void) {
    extern void fn_801FB1C0();
    extern void fn_80204854();
    extern void fn_80204928();
    extern void fn_80206608();
    extern void fn_80206780();
    extern void fn_80206A04();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r26, r3 */;
    r27 = r5;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        return;
    }
    r25 = r4 & 0xFFFF;
    r29 = 0x0;
    r28 = 0x0;
    r30 = 0x0;
    while (1) {
        r0 = r30 & 0xFFFF;
        if (r0 >= (u32)r25) break;
        r3 = r26;
        r6 = r30;
        r4 = 0x0;
        r5 = 0x45;
        fn_801FB1C0();
        r31 = r3;
        fn_80206A04();
        r0 = r3 & 0xFF;
        if ((s32)r0 == (s32)0) {
            r31 = 0x0;
        }
        do {
        if (r31 == (u32)0x0) break;
        r3 = r31;
        fn_80206608();
        r0 = r3 & 0xFF;
        if (r31 == (u32)0x0) {
            r0 = 0x1;

        } else {
            if (r26 == (u32)0x0) {
                r23 = 0x0;

            } else if (r31 == (u32)0x0) {
                r23 = 0x0;

            }
            r24 = 0x0;
            while (1) {
                r0 = r24 & 0xFFFF;
                if (r0 >= (u32)0x2) break;
                r3 = r26;
                r6 = r24;
                r4 = 0x0;
                r5 = 0x46;
                fn_801FB1C0();
                r23 = r3;
                fn_80206780();
                r0 = r3 & 0xFF;
                if (r31 == (u32)0x0) {
                    r23 = 0x0;
                }
                if (r23 != (u32)0x0) {
                    r3 = r31;
                    r4 = r23;
                    fn_80204928();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x1) {
                        break;
                }
                }
                r24 = r24 + 0x1;

            }
            r23 = 0x0;

            if (r23 != (u32)0x0) {
                r0 = 0x2;
                goto L_801F83B4;
            }
            if (r26 == (u32)0x0) {
                r0 = 0x0;

            } else if (r31 == (u32)0x0) {
                r0 = 0x0;

            }
            r24 = 0x0;
            while (1) {
                r0 = r24 & 0xFFFF;
                if (r0 >= (u32)0x2) break;
                r3 = r26;
                r6 = r24;
                r4 = 0x0;
                r5 = 0x46;
                fn_801FB1C0();
                r23 = r3;
                fn_80206780();
                r0 = r3 & 0xFF;
                if (r31 == (u32)0x0) {
                    r23 = 0x0;
                }
                if (r23 != (u32)0x0) {
                    r3 = r23;
                    r4 = r31;
                    fn_80204854();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x1) {
                        r0 = 0x1;
                        break;
                }
                }
                r24 = r24 + 0x1;

            }
            r0 = 0x0;

            r0 = r0 & 0xFF;
            if (r0 == (u32)0x1) {
                r0 = 0x3;

            } else {
                r0 = 0x0;
            }
        }
        L_801F83B4: ;
        r0 = r0 & 0xFF;
        if (r0 == (u32)0x1) break;
        if (r0 == (u32)0x2) {
            r28 = r28 + 0x1;
            break;
        }
        r29 = r29 + 0x1;
        } while (0);
        r30 = r30 + 0x1;

    }
    r3 = r27 & 0xFFFF;
    while (1) {
        r0 = r28 & 0xFFFF;
        if (r0 >= (u32)r3) { r3 = r29; return; }
        r0 = r29 & 0xFFFF;
        if (r0 < (u32)0x1) { r3 = r29; return; }
        /* subi r29, r29, 0x1 */;
        r28 = r28 + 0x1;
    }

    r3 = r29;

    return;
}

/* 0x801F8424 | size: 0x78 */
BOOL fn_801F8424(void* arg0, void* arg1, u32 arg2) {
    u32 val0;

    if (arg0 == NULL) {
        return FALSE;
    }
    if (arg1 == NULL) {
        return FALSE;
    }
    val0 = fn_801F02AC(2, arg0, arg2);
    return (u8)(fn_801F02AC(2, arg1, arg2) - val0 == 0);
}

/* 0x801F849C | size: 0x7C */
void fn_801F849C(void* arg0, u32 arg1) {
    extern void* fn_801FB1C0(void*, u32, u32, u32);
    extern u32 fn_8020ECA4(void*);
    extern void fn_8020EDBC(void*);
    u32 mode;
    void* pokemon;
    u8 i;

    if (arg0 == NULL) {
        return;
    }
    mode = (u8)arg1;
    i = 0;
    while (i < 12) {
        pokemon = fn_801FB1C0(arg0, 0, 0x4e, i);
        if (mode != 1 || (u8)fn_8020ECA4(pokemon)) {
            fn_8020EDBC(pokemon);
        }
        i++;
    }
}

/* 0x801F8518 | size: 0x98 */
void* fn_801F8518(void* context, u32 index) {
    extern u8 fn_80206780(void* ptr);
    extern u8 fn_801F1170(void* ptr);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    void* pokemon;
    void* sub;

    pokemon = fn_801FB1C0(context, 0, 0x46, index);
    if ((u8)fn_80206780(pokemon) == 0) {
        pokemon = NULL;
    }
    if (pokemon == NULL) {
        return NULL;
    }
    sub = fn_8012640C(pokemon, 0, 0xFE, 0);
    if (sub == NULL) {
        return NULL;
    }
    if ((u8)fn_801F1170(sub) == 1) {
        return NULL;
    }
    return pokemon;
}

/* 0x801F85B0 | size: 0x88 */
s32 fn_801F85B0(void* context, void* target) {
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u16 idx);
    int i;

    if (context == NULL) {
        return -1;
    }
    if (target == NULL) {
        return -1;
    }
    for (i = 0; (s16)i < 2; i++) {
        if (target == fn_801FB1C0(context, 0, 0x46, i)) {
            return i;
        }
    }
    return -1;
}

/* 0x801F8638 | size: 0x88 */
s32 fn_801F8638(void* context, void* target) {
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u16 idx);
    int i;

    if (context == NULL) {
        return -1;
    }
    if (target == NULL) {
        return -1;
    }
    for (i = 0; (s16)i < 6; i++) {
        if (target == fn_801FB1C0(context, 0, 0x45, i)) {
            return i;
        }
    }
    return -1;
}

/* 0x801F86C0 | size: 0x10C */
void fn_801F86C0(void* context, u8 flag) {
    extern void fn_8011F5FC(void* ptr, void* pokemon);
    extern void fn_8012190C(void* ptr, u32 field, u32 val);
    extern u8 fn_80121ADC(void* ptr, u32 field);
    extern u16 fn_801230E0(void* pokemon);
    extern u8 fn_80206A04(void* ptr);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u16 idx);
    void* slot;
    u16 i;
    void* pokemon;
    void* moveData;
    u16 speciesId;

    if (context == NULL) {
        return;
    }
    for (i = 0; i < 6; i++) {
        slot = fn_801FB1C0(context, 0, 0x45, i);
        if ((u8)fn_80206A04(slot) == 0) {
            slot = NULL;
        }
        if (slot != NULL) {
            if ((pokemon = fn_8012640C(slot, 0, 0xCC, 0)) != NULL) {
                speciesId = fn_801230E0(pokemon);
                if ((moveData = fn_8012640C(slot, 0, 0xCB, 0)) != NULL) {
                    fn_8011F5FC(moveData, pokemon);
                    if ((u8)fn_80121ADC(moveData, 4) == 1) {
                        fn_8012190C(moveData, 4, 1);
                    }
                    if (flag == 1) {
                        fn_801254B4(moveData, 0, 0x82, 0, speciesId);
                    }
                }
            }
        }
    }
}

/* 0x801F87CC | size: 0x24C | large */
void fn_801F87CC(void) {
    extern void fn_801FB1C0();
    extern void fn_80204854();
    extern void fn_80204928();
    extern void fn_80206608();
    extern void fn_80206780();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r5 = 0x0;
    r28 = r3;
    r29 = r4;
    *(u16*)(sp + 0x8) = r5;
    while (1) {
        r0 = *(u16*)(sp + 0x8);
        if (r0 >= (u32)0x6) break;
        r3 = *(u16*)(sp + 0x8);
        /* clrlslwi r0, r0, 16, 2 */;
        *(u32*)(r29 + r0) = r5;
        r0 = r3 + 0x1;
        *(u16*)(sp + 0x8) = r0;

    }
    r0 = 0x0;
    r30 = 0x0;
    *(u16*)(sp + 0x8) = r0;
    while (1) {
        if (r28 == (u32)0x0) {
            r4 = 0x0;

        } else {
            /* addic. r0, (u32)sp, 0x8 */;
            if (r28 == (u32)0x0) {
                r4 = 0x0;
                goto L_801F89DC;
            }
            goto L_801F89B8;
            do {
                r3 = r28;
                r4 = 0x0;
                r5 = 0x45;
                fn_801FB1C0();
                r31 = r3;
                fn_80206608();
                r0 = r3 & 0xFF;
                if (r28 == (u32)0x0) {
                    r0 = 0x1;

                } else {
                    if (r28 == (u32)0x0) {
                        r26 = 0x0;

                    } else if (r31 == (u32)0x0) {
                        r26 = 0x0;

                    }
                    r27 = 0x0;
                    while (1) {
                        r0 = r27 & 0xFFFF;
                        if (r0 >= (u32)0x2) break;
                        r3 = r28;
                        r6 = r27;
                        r4 = 0x0;
                        r5 = 0x46;
                        fn_801FB1C0();
                        r26 = r3;
                        fn_80206780();
                        r0 = r3 & 0xFF;
                        if (r31 == (u32)0x0) {
                            r26 = 0x0;
                        }
                        if (r26 != (u32)0x0) {
                            r3 = r31;
                            r4 = r26;
                            fn_80204928();
                            r0 = r3 & 0xFF;
                            if (r0 == (u32)0x1) {
                                break;
                        }
                        }
                        r27 = r27 + 0x1;

                    }
                    r26 = 0x0;

                    if (r26 != (u32)0x0) {
                        r0 = 0x2;
                        goto L_801F89A4;
                    }
                    if (r28 == (u32)0x0) {
                        r0 = 0x0;

                    } else if (r31 == (u32)0x0) {
                        r0 = 0x0;

                    }
                    r27 = 0x0;
                    while (1) {
                        r0 = r27 & 0xFFFF;
                        if (r0 >= (u32)0x2) break;
                        r3 = r28;
                        r6 = r27;
                        r4 = 0x0;
                        r5 = 0x46;
                        fn_801FB1C0();
                        r26 = r3;
                        fn_80206780();
                        r0 = r3 & 0xFF;
                        if (r31 == (u32)0x0) {
                            r26 = 0x0;
                        }
                        if (r26 != (u32)0x0) {
                            r3 = r26;
                            r4 = r31;
                            fn_80204854();
                            r0 = r3 & 0xFF;
                            if (r0 == (u32)0x1) {
                                r0 = 0x1;
                                break;
                        }
                        }
                        r27 = r27 + 0x1;

                    }
                    r0 = 0x0;

                    r0 = r0 & 0xFF;
                    if (r0 == (u32)0x1) {
                        r0 = 0x3;

                    } else {
                        r0 = 0x0;
                    }
                }
                L_801F89A4: ;
                r0 = r0 & 0xFF;
                if (r0 == (u32)0x1) break;
                r3 = *(u16*)(sp + 0x8);
                r0 = r3 + 0x1;
                *(u16*)(sp + 0x8) = r0;
                L_801F89B8: ;
                r6 = *(u16*)(sp + 0x8);
            } while (r6 < (u32)0x6);

            r0 = *(u16*)(sp + 0x8);
            if (r0 >= (u32)0x6) {
                r4 = 0x0;

            } else {
                r4 = r31;
            }
        }
        L_801F89DC: ;
        if (r4 == (u32)0x0) { r3 = r30; return; }
        r3 = *(u16*)(sp + 0x8);
        /* clrlslwi r0, r30, 16, 2 */;
        *(u32*)(r29 + r0) = r4;
        r30 = r30 + 0x1;
        r0 = r3 + 0x1;
        *(u16*)(sp + 0x8) = r0;
    }

    r3 = r30;
    return;
}

/* 0x801F8A18 | size: 0x1E8 | medium */
void fn_801F8A18(void) {
    extern void fn_801FB1C0();
    extern void fn_80204854();
    extern void fn_80204928();
    extern void fn_80206608();
    extern void fn_80206780();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r29, r3 */;
    r30 = r4;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        return;
    }
    if (r30 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    goto L_801F8BC8;
    do {
        r3 = r29;
        r4 = 0x0;
        r5 = 0x45;
        fn_801FB1C0();
        r31 = r3;
        fn_80206608();
        r0 = r3 & 0xFF;
        if (r30 == (u32)0x0) {
            r0 = 0x1;

        } else {
            if (r29 == (u32)0x0) {
                r27 = 0x0;

            } else if (r31 == (u32)0x0) {
                r27 = 0x0;

            }
            r28 = 0x0;
            while (1) {
                r0 = r28 & 0xFFFF;
                if (r0 >= (u32)0x2) break;
                r3 = r29;
                r6 = r28;
                r4 = 0x0;
                r5 = 0x46;
                fn_801FB1C0();
                r27 = r3;
                fn_80206780();
                r0 = r3 & 0xFF;
                if (r31 == (u32)0x0) {
                    r27 = 0x0;
                }
                if (r27 != (u32)0x0) {
                    r3 = r31;
                    r4 = r27;
                    fn_80204928();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x1) {
                        break;
                }
                }
                r28 = r28 + 0x1;

            }
            r27 = 0x0;

            if (r27 != (u32)0x0) {
                r0 = 0x2;
                goto L_801F8BB4;
            }
            if (r29 == (u32)0x0) {
                r0 = 0x0;

            } else if (r31 == (u32)0x0) {
                r0 = 0x0;

            }
            r28 = 0x0;
            while (1) {
                r0 = r28 & 0xFFFF;
                if (r0 >= (u32)0x2) break;
                r3 = r29;
                r6 = r28;
                r4 = 0x0;
                r5 = 0x46;
                fn_801FB1C0();
                r27 = r3;
                fn_80206780();
                r0 = r3 & 0xFF;
                if (r31 == (u32)0x0) {
                    r27 = 0x0;
                }
                if (r27 != (u32)0x0) {
                    r3 = r27;
                    r4 = r31;
                    fn_80204854();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x1) {
                        r0 = 0x1;
                        break;
                }
                }
                r28 = r28 + 0x1;

            }
            r0 = 0x0;

            r0 = r0 & 0xFF;
            if (r0 == (u32)0x1) {
                r0 = 0x3;

            } else {
                r0 = 0x0;
            }
        }
        L_801F8BB4: ;
        r0 = r0 & 0xFF;
        if (r0 == (u32)0x1) break;
        r3 = *(u16*)((u8*)r30 + 0x0);
        r0 = r3 + 0x1;
        *(u16*)((u8*)r30 + 0x0) = r0;
        L_801F8BC8: ;
        r6 = *(u16*)((u8*)r30 + 0x0);
    } while (r6 < (u32)0x6);

    r0 = *(u16*)((u8*)r30 + 0x0);
    if (r0 >= (u32)0x6) {
        r3 = 0x0;
        return;
    }
    r3 = r31;

    return;
}

/* 0x801F8C00 | size: 0x180 */
u32 fn_801F8C00(void* context, void* filter) {
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    extern u8 fn_80204854(void* pokemon, void* filter);
    extern u8 fn_80204928(void* filter, void* pokemon);
    extern u8 fn_80206608(void* ptr);
    extern u8 fn_80206780(void* ptr);
    void* pokemon;
    u8 found;
    u16 i;

    if ((u8)fn_80206608(filter) == 0) {
        return 1;
    }
    pokemon = NULL;
    if (context == NULL) {
        pokemon = NULL;
    } else if (filter == NULL) {
        pokemon = NULL;
    } else {
        for (i = 0; i < 2; i++) {
            pokemon = fn_801FB1C0(context, 0, 0x46, i);
            if ((u8)fn_80206780(pokemon) == 0) {
                pokemon = NULL;
            }
            if (pokemon != NULL) {
                if ((u8)fn_80204928(filter, pokemon) == 1) {
                    break;
                }
            }
        }
        pokemon = NULL;
    }
    if (pokemon != NULL) {
        return 2;
    }
    found = 0;
    if (context == NULL) {
        found = 0;
    } else if (filter == NULL) {
        found = 0;
    } else {
        for (i = 0; i < 2; i++) {
            pokemon = fn_801FB1C0(context, 0, 0x46, i);
            if ((u8)fn_80206780(pokemon) == 0) {
                pokemon = NULL;
            }
            if (pokemon != NULL) {
                if ((u8)fn_80204854(pokemon, filter) == 1) {
                    found = 1;
                    break;
                }
            }
        }
        found = 0;
    }
    return found != 0 ? 3 : 0;
}

/* 0x801F8D80 | size: 0xB4 */
void* fn_801F8D80(void* context, void* filter) {
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u16 idx);
    extern u8 fn_80204928(void* filter, void* pokemon);
    extern u8 fn_80206780(void* ptr);
    void* pokemon;
    u16 i;

    if (context == NULL) {
        return NULL;
    }
    if (filter == NULL) {
        return NULL;
    }
    for (i = 0; i < 2; i++) {
        pokemon = fn_801FB1C0(context, 0, 0x46, i);
        if ((u8)fn_80206780(pokemon) == 0) {
            pokemon = NULL;
        }
        if (pokemon != NULL) {
            if ((u8)fn_80204928(filter, pokemon) == 1) {
                return pokemon;
            }
        }
    }
    return NULL;
}

/* 0x801F8E34 | size: 0xF0 */
void* fn_801F8E34(void* context, void* filter) {
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    extern u8 fn_80204928(void* filter, void* pokemon);
    extern u8 fn_802062FC(void* ptr);
    extern u8 fn_80206780(void* ptr);
    void* pokemon;
    u16 i;

    if (context == NULL) {
        return NULL;
    }
    if (filter == NULL) {
        return NULL;
    }
    pokemon = NULL;
    if (context == NULL) {
        pokemon = NULL;
    } else if (filter == NULL) {
        pokemon = NULL;
    } else {
        for (i = 0; i < 2; i++) {
            pokemon = fn_801FB1C0(context, 0, 0x46, i);
            if ((u8)fn_80206780(pokemon) == 0) {
                pokemon = NULL;
            }
            if (pokemon != NULL) {
                if ((u8)fn_80204928(filter, pokemon) == 1) {
                    break;
                }
            }
        }
        pokemon = NULL;
    }
    if (pokemon != NULL) {
        if ((u8)fn_802062FC(pokemon) == 0) {
            pokemon = NULL;
        }
    }
    return pokemon;
}

/* 0x801F8F24 | size: 0xB4 */
void* fn_801F8F24(void* context, s16 speciesId) {
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    extern u8 fn_80206A04(void* ptr);
    void* pokemon;
    u16 i;

    if ((s16)speciesId < 0) {
        return NULL;
    }
    for (i = 0; i < 6; i++) {
        pokemon = fn_801FB1C0(context, 0, 0x45, i);
        if ((u8)fn_80206A04(pokemon) == 0) {
            pokemon = NULL;
        }
        if (pokemon != NULL) {
            if ((s16)(s32)fn_8012640C(pokemon, 0, 0xCE, 0) == (s16)speciesId) {
                return pokemon;
            }
        }
    }
    return NULL;
}

/* 0x801F8FD8 | size: 0x5C */
void* fn_801F8FD8(void* arg0) {
    extern void* fn_801DA4E8(void*, u32);
    extern void* fn_801DE418(u32);
    extern void* fn_801FB1C0(void*, void*, u32, u32);
    void* result;

    result = fn_801DE418((u16)(u32)fn_801FB1C0(0, arg0, 9, 0));
    if (result == NULL) {
        return NULL;
    }
    fn_801DA4E8(result, 0);
    return result;
}

/* 0x801F9034 | size: 0x90 */
u32 fn_801F9034(void* context) {
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    u16 slot;
    u16 type;
    u8 val;

    slot = (u16)(u32)fn_801FB1C0(context, 0, 0x43, 0);
    type = (u16)(u32)fn_801FB1C0(0, slot, 0x4, 0);
    if (type == 1) {
        val = 0;
    } else if (type == 2 || type == 3) {
        val = 1;
    } else {
        val = 2;
    }
    return val == 0;
}

/* 0x801F90C4 | size: 0x6C */
void* fn_801F90C4(void* arg0, void* arg1) {
    extern void* heroIsMinePokemon(void*, void*);
    extern void* fn_801FB1C0(void*, u32, u32, u32);
    void* pokemon;
    void* trainerData;

    extern void* fn_80205BE8(void*);
    pokemon = fn_80205BE8(arg1);
    if (pokemon == NULL) {
        return NULL;
    }
    trainerData = fn_801FB1C0(arg0, 0, 0x44, 0);
    if (trainerData == NULL) {
        return NULL;
    }
    return heroIsMinePokemon(trainerData, pokemon);
}

/* 0x801F9130 | size: 0x10C */
#pragma push
#pragma optimization_level 2
void fn_801F9130(void* unused, void* trainer, void* pokemon) {
    extern u8 lbl_80375CA8[];
    extern u32 fn_801F0134(void* ptr, void* pokemon);
    extern u8 fn_801FF1BC(void* trainer, u32 mode);
    extern u8 fn_801FFEC8(void* trainer, s32 slot, u32 field, u32 flags);
    extern void fn_80204F6C(void* trainer, u32 p1, u32 p2, u32 p3, void* table, u16 moveId, u32 nameId, u32 slotIdx);
    extern void* fn_80205B8C(void* trainer);
    extern void fn_8022B2CC(void* trainer, u16 moveId, void* pokemon, u32 p3, u32 p4, u32 p5, s32 p6);
    u32 sp8;
    s8 i;
    u16 moveId;
    s32 slot;

    if ((u8)fn_801FF1BC(trainer, 1) != 0) {
        return;
    }
    for (i = 0; (s8)i < 4; i++) {
        moveId = (u16)(u32)fn_8012640C(fn_80205B8C(trainer), 0, 0x7F, 0);
        slot = (s8)i;
        if ((u8)fn_801FFEC8(trainer, slot, 1, 0) == 0) {
            moveId = (u16)(u32)fn_8012640C(fn_80205B8C(trainer), 0, 0x7F, slot);
            break;
        }
    }
    fn_8022B2CC(trainer, moveId, pokemon, 0, 1, 0, -1);
    sp8 = 0;
    fn_80204F6C(trainer, 0, 0x13, 0, (void*)lbl_80375CA8, moveId, fn_801F0134(trainer, pokemon), (u32)i);
}
#pragma pop

/* 0x801F923C | size: 0x1BC */
u32 fn_801F923C(void* context, void* param) {
    extern u16 fn_801EF634(void* ctx);
    extern void fightActionInit(void* ptr);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    extern void fn_80207760(void* ptr);
    extern void fn_802342CC(void* ctx, void* param);
    extern u8 fn_80262508(void* ctx, void* param);
    extern void fn_8026316C(void* ctx, void* param, u32 flags);
    void* result;
    u16 slot;
    u16 type;
    u8 battleType;
    u16 i;

    result = fn_801FB1C0(context, 0, 0x4D, 0);
    if (result != NULL) {
        fightActionInit(result);
    }
    for (i = 0; i < 2; i++) {
        fn_80207760(fn_801FB1C0(context, 0, 0x46, i));
    }
    fn_80207760(fn_801FB1C0(context, 0, 0x47, 0));
    if ((u16)fn_801EF634(context) == 1) {
        return 0;
    }
    slot = (u16)(u32)fn_801FB1C0(context, 0, 0x43, 0);
    if ((u16)(u32)fn_801FB1C0(0, slot, 0x2, 0) != 0) {
        fn_802342CC(context, param);
        return 1;
    }
    type = (u16)(u32)fn_801FB1C0(0, slot, 0x4, 0);
    if (type == 1) {
        battleType = 0;
    } else if (type == 2 || type == 3) {
        battleType = 1;
    } else {
        battleType = 2;
    }
    if (battleType == 0) {
        fn_8026316C(context, param, 0);
        return 1;
    }
    type = (u16)(u32)fn_801FB1C0(0, slot, 0x4, 0);
    if (type == 1) {
        battleType = 0;
    } else if (type == 2 || type == 3) {
        battleType = 1;
    } else {
        battleType = 2;
    }
    if (battleType != 1) {
        return 1;
    }
    if ((u8)fn_80262508(context, param) == 0) {
        return 0;
    }
    return 1;
}

/* 0x801F93F8 | size: 0x208 | large */
void fn_801F93F8(void) {
    extern void fn_801FB1C0();
    extern void fn_80204854();
    extern void fn_80204928();
    extern void fn_80206608();
    extern void fn_80206780();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r30, r3 */;
    r3 = 0x0;
    *(u16*)(sp + 0x8) = r3;
    if ((s32)r0 == (s32)0) {

    } else {
        /* addic. r0, (u32)sp, 0x8 */;
        if ((s32)r0 == (s32)0) {
            goto L_801F95C8;
        }
        goto L_801F95A4;
        do {
            r3 = r30;
            r4 = 0x0;
            r5 = 0x45;
            fn_801FB1C0();
            r31 = r3;
            fn_80206608();
            r0 = r3 & 0xFF;
            if ((s32)r0 == (s32)0) {
                r0 = 0x1;

            } else {
                if (r30 == (u32)0x0) {
                    r28 = 0x0;

                } else if (r31 == (u32)0x0) {
                    r28 = 0x0;

                }
                r29 = 0x0;
                while (1) {
                    r0 = r29 & 0xFFFF;
                    if (r0 >= (u32)0x2) break;
                    r3 = r30;
                    r6 = r29;
                    r4 = 0x0;
                    r5 = 0x46;
                    fn_801FB1C0();
                    r28 = r3;
                    fn_80206780();
                    r0 = r3 & 0xFF;
                    if (r31 == (u32)0x0) {
                        r28 = 0x0;
                    }
                    if (r28 != (u32)0x0) {
                        r3 = r31;
                        r4 = r28;
                        fn_80204928();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            break;
                    }
                    }
                    r29 = r29 + 0x1;

                }
                r28 = 0x0;

                if (r28 != (u32)0x0) {
                    r0 = 0x2;
                    goto L_801F9590;
                }
                if (r30 == (u32)0x0) {
                    r0 = 0x0;

                } else if (r31 == (u32)0x0) {
                    r0 = 0x0;

                }
                r29 = 0x0;
                while (1) {
                    r0 = r29 & 0xFFFF;
                    if (r0 >= (u32)0x2) break;
                    r3 = r30;
                    r6 = r29;
                    r4 = 0x0;
                    r5 = 0x46;
                    fn_801FB1C0();
                    r28 = r3;
                    fn_80206780();
                    r0 = r3 & 0xFF;
                    if (r31 == (u32)0x0) {
                        r28 = 0x0;
                    }
                    if (r28 != (u32)0x0) {
                        r3 = r28;
                        r4 = r31;
                        fn_80204854();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r0 = 0x1;
                            break;
                    }
                    }
                    r29 = r29 + 0x1;

                }
                r0 = 0x0;

                r0 = r0 & 0xFF;
                if (r0 == (u32)0x1) {
                    r0 = 0x3;

                } else {
                    r0 = 0x0;
                }
            }
            L_801F9590: ;
            r0 = r0 & 0xFF;
            if (r0 == (u32)0x1) break;
            r3 = *(u16*)(sp + 0x8);
            r0 = r3 + 0x1;
            *(u16*)(sp + 0x8) = r0;
            L_801F95A4: ;
            r6 = *(u16*)(sp + 0x8);
        } while (r6 < (u32)0x6);

        r0 = *(u16*)(sp + 0x8);
        if (r0 >= (u32)0x6) {
            r3 = 0x0;

        } else {
            r3 = r31;
        }
    }
    L_801F95C8: ;
    if (r3 != (u32)0x0) {
        r4 = 0x0;
        r5 = 0xce;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r3 = (s16)r3;
    } else {

        r3 = -0x1;
    }
    return;
}

/* 0x801F9600 | size: 0x190 */
s32 fn_801F9600(void* context, void* p1, void* p2, void* p3) {
    extern u8 fn_80008174(void);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    extern s32 fn_8024E690(void* ctx, void* p1, void* p2, void* p3);
    extern s32 fn_80262D3C(void* ctx, void* p1, void* p2, void* p3);
    extern s32 fn_80263DE4(void* ctx, u32 zero, void* p1, void* p2, u32 flags);
    s32 result;
    u16 slot;
    u16 species;
    u16 type;
    u8 battleType;

    result = -1;
    slot = (u16)(u32)fn_801FB1C0(context, 0, 0x43, 0);
    species = (u16)(u32)fn_801FB1C0(0, slot, 0x2, 0);
    if ((u8)fn_80008174() == 1) {
        return fn_80263DE4(context, 0, p1, p2, 0);
    }
    if (species != 0) {
        return fn_8024E690(context, p1, p2, p3);
    }
    type = (u16)(u32)fn_801FB1C0(0, slot, 0x4, 0);
    if (type == 1) {
        battleType = 0;
    } else if (type == 2 || type == 3) {
        battleType = 1;
    } else {
        battleType = 2;
    }
    if (battleType == 0) {
        return fn_80263DE4(context, 0, p1, p2, 0);
    }
    type = (u16)(u32)fn_801FB1C0(0, slot, 0x4, 0);
    if (type == 1) {
        battleType = 0;
    } else if (type == 2 || type == 3) {
        battleType = 1;
    } else {
        battleType = 2;
    }
    if (battleType != 1) {
        return result;
    }
    return fn_80262D3C(context, p1, p2, p3);
}

/* 0x801F9790 | size: 0x8C */
void fn_801F9790(void* context) {
    extern void fightActionInit(void* ptr);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u16 idx);
    extern void fn_80207760(void* ptr);
    void* result;
    u16 i;

    result = fn_801FB1C0(context, 0, 0x4D, 0);
    if (result != NULL) {
        fightActionInit(result);
    }
    for (i = 0; i < 2; i++) {
        fn_80207760(fn_801FB1C0(context, 0, 0x46, i));
    }
    fn_80207760(fn_801FB1C0(context, 0, 0x47, 0));
}

/* 0x801F981C | size: 0x50 */
void* fn_801F981C(void* arg1, u32 arg2) {
    extern void* fn_801FB1C0(void*, u32, u32, u32);
    extern u32 fn_80206780(void*);
    void* result = fn_801FB1C0(arg1, 0, 0x46, arg2);
    if ((u8)fn_80206780(result) == 0) {
        return NULL;
    }
    return result;
    return;
}

/* 0x801F986C | size: 0x50 */
void* fn_801F986C(void* arg1, u32 arg2) {
    extern void* fn_801FB1C0(void*, u32, u32, u32);
    extern u32 fn_80206A04(void*);
    void* result = fn_801FB1C0(arg1, 0, 0x45, arg2);
    if ((u8)fn_80206A04(result) == 0) {
        return NULL;
    }
    return result;
}

/* 0x801F98BC | size: 0x74 */
u32 fn_801F98BC(void* arg0) {
    extern void* fn_801FB1C0(void*, u32, u32, u32);
    extern u32 fn_802062FC(void*);
    u8 i;
    u32 count;

    count = 0;
    for (i = 0; i < 2; i++) {
        if ((u8)fn_802062FC(fn_801FB1C0(arg0, 0, 0x46, i))) {
            count = ((count & 0xFF) + 1) & 0xFF;
        }
    }
    return count;
}

/* 0x801F9930 | size: 0x98 */
void* fn_801F9930(void* context, void* moveData) {
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    extern u8 fn_80206A04(void* ptr);
    int i;
    void* pokemon;
    void* data;

    for (i = 0; (u16)i < 6; i++) {
        pokemon = fn_801FB1C0(context, 0, 0x45, i);
        if ((u8)fn_80206A04(pokemon) != 0) {
            data = fn_8012640C(pokemon, 0, 0xCB, 0);
            if (data != NULL) {
                if (moveData == data) {
                    return pokemon;
                }
            }
        }
    }
    return NULL;
}

/* 0x801F99C8 | size: 0x2F4 | large */
void fn_801F99C8(void) {
    extern void fn_80123FBC();
    extern void fn_80124A60();
    extern void fn_8012A130();
    extern void fn_8012A5B0();
    extern void fn_801EF634();
    extern void fn_801FB1C0();
    extern void fn_802331F4();
    u8 sp[0x790];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    /* mr. r28, r3 */;
    r29 = r4;
    r30 = r5;
    if ((s32)r0 == (s32)0) return;
    if ((s32)r0 != (s32)0) {

    fn_801EF634();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x1) {

        r3 = r28;
        r4 = 0x0;
        r5 = 0x43;
        r6 = 0x0;
        fn_801FB1C0();
    if ((s32)r3 != (s32)0x0) {

            r3 = r28;
            r4 = 0x0;
            r5 = 0x44;
            r6 = 0x0;
            fn_801FB1C0();
    if (r3 != (u32)0x0) {

                fn_8012A130();
                r0 = r3 & 0xFF;
    if (r3 != (u32)0x0) {

                    r0 = 0x1;
    }
    }
    }
    }
    }
    r0 = r0 & 0xFF;
    if (r3 == (u32)0x0) return;
    r3 = r28;
    r4 = 0x0;
    r5 = 0x43;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = r3 & 0xFFFF;
    r3 = 0x0;
    r4 = r26;
    r5 = 0x2;
    r6 = 0x0;
    fn_801FB1C0();
    r27 = r3 & 0xFFFF;
    r3 = r28;
    r4 = 0x0;
    r5 = 0x44;
    r6 = 0x0;
    fn_801FB1C0();
    r31 = r3;
    if (r27 == (u32)0x0) return;
    r4 = r26;
    r3 = 0x0;
    r5 = 0x4;
    r6 = 0x0;
    fn_801FB1C0();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;

    } else {

        if (r0 == (u32)0x2 || r0 == (u32)0x3) {

            r0 = 0x1;

        } else {
            r0 = 0x2;
        }
    }
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x3) return;
    r4 = r26;
    r3 = 0x0;
    r5 = 0x4;
    r6 = 0x0;
    fn_801FB1C0();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;

    } else {

        if (r0 == (u32)0x2 || r0 == (u32)0x3) {

            r0 = 0x1;

        } else {
            r0 = 0x2;
        }
    }
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        return;
    }
    r27 = (u32)sp + 0x8;
    r25 = 0x0;
    r26 = 0x0;
    while (1) {
        r0 = r25 & 0xFF;
        if (r0 >= (u32)0x6) break;
        r0 = r25 & 0xFF;
        /* clrlslwi r4, r25, 24, 2 */;
        r0 = r0 * 0x138;
        r3 = (u32)sp + 0x20;
        *(u32*)(r27 + r4) = r26;
        r3 = r3 + r0;
        fn_80124A60();
        r25 = r25 + 0x1;

    }
    r0 = r29 & 0xFFFF;
    if (r0 > (u32)0x6) {
        r29 = 0x6;
    }
    r3 = r28;
    r4 = r31;
    r6 = r29;
    r7 = r30;
    r5 = (u32)sp + 0x8;
    fn_802331F4();
    r26 = r29 & 0xFFFF;
    r28 = (u32)sp + 0x8;
    r25 = 0x0;
    r27 = 0x0;
    while (1) {
        r0 = r27 & 0xFF;
        if ((s32)r0 >= (s32)r26) break;
        /* clrlslwi r0, r27, 24, 2 */;
        r29 = *(u32*)(r28 + r0);
        if (r29 != (u32)0x0) {
            r3 = r29;
            fn_80123FBC();
            r0 = r3 & 0xFF;
            if (r29 != (u32)0x0) {
                r0 = r25 & 0xFF;
                r3 = (u32)sp + 0x20;
                r5 = r0 * 0x138;
                r0 = 0x27;
                /* subi r4, r29, 0x4 */;
                r5 = r3 + r5;
                ctr_fn = (void(*)(void))r0;
                /* subi r5, r5, 0x4 */;
                do {
                    r3 = *(u32*)((u8*)r4 + 0x4);
                    r0 = *(u32*)((u8*)r4 + 0x8);
                    *(u32*)((u8*)r5 + 0x4) = r3;
                    r5 += 8; *(u32*)r5 = r0;
                } while (--ctr != 0);
                r25 = r25 + 0x1;
        }
        }
        r27 = r27 + 0x1;

    }
    r29 = (u32)sp + 0x20;
    r26 = 0x0;
    r28 = 0x27;
    while (1) {
        r0 = r26 & 0xFF;
        if (r0 >= (u32)0x6) break;
        r3 = r31;
        r5 = r26 & 0xFF;
        r4 = 0x3;
        fn_8012A5B0();
        if (r3 != (u32)0x0) {
            r0 = r26 & 0xFF;
            /* subi r5, r3, 0x4 */;
            r0 = r0 * 0x138;
            r4 = r29 + r0;
            ctr_fn = (void(*)(void))r28;
            /* subi r4, r4, 0x4 */;
            do {
                r3 = *(u32*)((u8*)r4 + 0x4);
                r0 = *(u32*)((u8*)r4 + 0x8);
                *(u32*)((u8*)r5 + 0x4) = r3;
                r5 += 8; *(u32*)r5 = r0;
            } while (--ctr != 0);
        }
        r26 = r26 + 0x1;

    }

    return;
}

/* 0x801F9CBC | size: 0x2BC | large */
void fn_801F9CBC(void) {
    extern void fn_800896B8();
    extern void fn_800896C8();
    extern void fn_800FA280();
    extern void fn_80129280();
    extern void fn_80129A78();
    extern void fn_80129F20();
    extern void fn_8012A1A4();
    extern void fn_8012A450();
    extern void fn_8012A5B0();
    extern void fn_8012AC64();
    extern void fn_801F9F78();
    extern void fn_801FB1C0();
    u8 sp[0x150];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r31, r5 */;
    r30 = r3;
    r28 = r4;
    if ((s32)r0 == (s32)0) return;
    r4 = r30;
    r3 = 0x0;
    r5 = 0x2;
    r6 = 0x0;
    fn_801FB1C0();
    r0 = r30 & 0xFFFF;
    r3 = r3 & 0xFFFF;
    if ((s32)r0 != (s32)0) {

    if (r3 != (u32)0x0 || (s32)r28 != (s32)0x0) {

        r0 = 0x1;
    }
    }
    r0 = r0 & 0xFF;
    if ((s32)r28 == (s32)0x0) return;
    r4 = r30;
    r3 = 0x0;
    r5 = 0x4;
    r6 = 0x0;
    fn_801FB1C0();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;

    } else {

        if (r0 == (u32)0x2 || r0 == (u32)0x3) {

            r0 = 0x1;

        } else {
            r0 = 0x2;
        }
    }
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x3) {
        r3 = 0x0;
        r4 = 0x2;
        fn_80129280();
        /* mr. r4, r3 */;
        if (r0 == (u32)0x3) return;
        r3 = r31;
        fn_8012AC64();
        return;
    }
    r4 = r30;
    r3 = 0x0;
    r5 = 0x4;
    r6 = 0x0;
    fn_801FB1C0();
    r4 = r30;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x0;
    fn_801FB1C0();
    r29 = r3 & 0xFF;
    r4 = r30;
    r3 = 0x0;
    r5 = 0x3;
    r6 = 0x0;
    fn_801FB1C0();
    fn_800FA280();
    r28 = r3;
    fn_800896B8();
    r0 = r30 & 0xFFFF;
    if (r0 == (u32)r3) {
        fn_800896C8();
        r28 = r3;
    }
    r3 = r31;
    r4 = r28;
    r5 = r29;
    fn_8012A1A4();
    r4 = r30;
    r3 = 0x0;
    r5 = 0x5;
    r6 = 0x0;
    fn_801FB1C0();
    r29 = r3 & 0xFFFF;
    while (1) {
        r3 = r31;
        r4 = 0x2;
        r5 = 0x0;
        fn_8012A5B0();
        r5 = r3;
        r3 = r29;
        r4 = (u32)sp + 0x8;
        fn_801F9F78();
        r0 = r3 & 0xFF;
        if (r0 != (u32)r3) {
            r3 = (u32)sp + 0x8;
            r7 = r29 & 0xFFFF;
            r4 = 0x0;
            r5 = 0xc9;
            r6 = 0x0;
            ((void(*)(void))fn_801254B4)();
            r4 = r29;
            r3 = 0x0;
            r5 = 0x12;
            r6 = 0x0;
            fn_801FB1C0();
            r6 = r3 & 0xFFFF;
            r3 = r31;
            r4 = (u32)sp + 0x8;
            r5 = 0x0;
            r7 = 0x0;
            fn_80129F20();
            r0 = (s16)r3;
        }
        if (r0 >= (u32)r3) {
            r29 = r29 + 0x1;
    }
    }
    r29 = 0x0;
    while (1) {
        r0 = r29 & 0xFFFF;
        if (r0 >= (u32)0x8) break;
        r4 = r30;
        r6 = r29;
        r3 = 0x0;
        r5 = 0x6;
        fn_801FB1C0();
        r4 = r3 & 0xFFFF;
        if (r0 != (u32)r3) {
            r3 = r31;
            r5 = 0x1;
            r6 = -0x1;
            fn_80129A78();
            if ((s32)r3 < (s32)0x0) break;
        }
        r29 = r29 + 0x1;

    }

    r3 = r31;
    r4 = 0xf;
    r5 = 0x0;
    fn_8012A450();
    r3 = r31;
    r4 = 0x10;
    r5 = 0x0;
    fn_8012A450();
    r3 = r31;
    r4 = 0x11;
    r5 = 0x0;
    fn_8012A450();
    r3 = r31;
    r4 = 0x12;
    r5 = 0x0;
    fn_8012A450();
    r3 = r31;
    r4 = 0x13;
    r5 = 0x0;
    fn_8012A450();
    r3 = r31;
    r4 = 0x14;
    r5 = 0x0;
    fn_8012A450();
    r3 = r31;
    r4 = 0x15;
    r5 = 0x0;
    fn_8012A450();
    r3 = r31;
    r4 = 0x16;
    r5 = 0x0;
    fn_8012A450();

    return;
}

/* 0x801F9F78 | size: 0x53C | large */
void fn_801F9F78(void) {
    extern u8 lbl_80279C48[];
    extern u8 lbl_80279C54[];
    extern void fn_800FA280();
    extern void fn_8011FCA4();
    extern void fn_80123110();
    extern void fn_80123D58();
    extern void fn_801240C4();
    extern void fn_80124410();
    extern void fn_80124978();
    extern void fn_80125314();
    extern void fn_8012546C();
    extern void fn_80135938();
    extern void fn_801EE750();
    extern void fn_801EE7BC();
    extern void fn_801EE824();
    extern void fn_801EE894();
    extern void fn_801EE8F4();
    extern void fn_801FB1C0();
    u8 sp[0x80];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r7 = (u32)lbl_80279C48;
    r6 = (u32)lbl_80279C54;
    r28 = r3;
    r31 = r4;
    r25 = r5;
    r4 = r28;
    r3 = 0x0;
    r5 = 0x15;
    r11 = *(u32*)lbl_80279C48;
    r8 = *(u32*)lbl_80279C54;
    r10 = *(u32*)((u8*)r7 + 0x4);
    r9 = *(u32*)((u8*)r7 + 0x8);
    r7 = *(u32*)((u8*)r6 + 0x4);
    r0 = *(u32*)((u8*)r6 + 0x8);
    r6 = 0x0;
    *(u32*)(sp + 0x1C) = r0;
    fn_801FB1C0();
    r23 = r3 & 0xFFFF;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        return;
    }
    r4 = r28;
    r3 = 0x0;
    r5 = 0x13;
    r6 = 0x0;
    fn_801FB1C0();
    r30 = r3 & 0xFF;
    r4 = r28;
    r3 = 0x0;
    r5 = 0x11;
    r6 = 0x0;
    fn_801FB1C0();
    r21 = r3 & 0xFF;
    r4 = r28;
    r3 = 0x0;
    r5 = 0xe;
    r6 = 0x0;
    fn_801FB1C0();
    r24 = (u32)sp + 0xc;
    r22 = r3;
    r19 = 0x0;
    while (1) {
        r0 = r19 & 0xFF;
        if (r0 >= (u32)0x6) break;
        r4 = r28;
        r6 = r19 & 0xFF;
        r3 = 0x0;
        r5 = 0xf;
        fn_801FB1C0();
        r0 = r19 & 0xFF;
        r19 = r19 + 0x1;
        *(u8*)(r24 + r0) = r3;

    }
    r24 = (u32)sp + 0x3c;
    r19 = 0x0;
    while (1) {
        r0 = r19 & 0xFF;
        if (r0 >= (u32)0x6) break;
        r4 = r28;
        r6 = r19 & 0xFF;
        r3 = 0x0;
        r5 = 0x10;
        fn_801FB1C0();
        /* clrlslwi r0, r19, 24, 1 */;
        r19 = r19 + 0x1;
        *(u16*)(r24 + r0) = r3;

    }
    r4 = r28;
    r3 = 0x0;
    r5 = 0x14;
    r6 = 0x0;
    fn_801FB1C0();
    r27 = (s8)r3;
    r4 = r28;
    r3 = 0x0;
    r5 = 0x16;
    r6 = 0x0;
    fn_801FB1C0();
    r24 = r3;
    r29 = (u32)sp + 0x2c;
    r26 = (u32)sp + 0x8;
    r19 = 0x0;
    while (1) {
        r0 = r19 & 0xFF;
        if (r0 >= (u32)0x4) break;
        r20 = r19 & 0xFF;
        r4 = r28;
        r6 = r20;
        r3 = 0x0;
        r5 = 0x17;
        fn_801FB1C0();
        /* clrlslwi r0, r19, 24, 2 */;
        r4 = r28;
        *(u32*)(r29 + r0) = r3;
        r6 = r20;
        r3 = 0x0;
        r5 = 0x18;
        fn_801FB1C0();
        *(u8*)(r26 + r20) = r3;
        r19 = r19 + 0x1;

    }
    r4 = r28;
    r3 = 0x0;
    r5 = 0x19;
    r6 = 0x0;
    fn_801FB1C0();
    r26 = (s16)r3;
    r4 = r28;
    r3 = 0x0;
    r5 = 0x1a;
    r6 = 0x0;
    fn_801FB1C0();
    r29 = (s8)r3;
    r4 = r28;
    r3 = 0x0;
    r5 = 0x1b;
    r6 = 0x0;
    fn_801FB1C0();
    r28 = (s8)r3;
    r3 = 0x0;
    r4 = 0x1;
    fn_80135938();
    r6 = r3;
    r3 = r31;
    r4 = r23;
    r5 = r21;
    fn_801240C4();
    if (r22 != (u32)0x0) {
        r3 = r22;
        fn_800FA280();
        r7 = r3;
        r3 = r31;
        r4 = 0x0;
        r5 = 0x77;
        r6 = 0x0;
        ((void(*)(void))fn_801254B4)();
    }
    r23 = r30;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFF;
        if (r0 >= (u32)0x6) break;
        if (r30 != (u32)0x0) {
            r3 = r30;
            fn_801EE8F4();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r30;
                r4 = r22 & 0xFF;
                fn_801EE824();
                r5 = (s8)r3;
                /* clrlslwi r0, r22, 24, 1 */;
                r4 = (u32)sp + 0x20;
                r3 = r31;
                r7 = r5 & 0xFFFF;
                r5 = *(u16*)(r4 + r0);
                r4 = 0x0;
                r6 = 0x0;
                ((void(*)(void))fn_801254B4)();
                goto L_801FA250;
        }
        }
        r5 = r22 & 0xFF;
        r3 = (u32)sp + 0xc;
        r4 = *(u8*)(r3 + r5);
        r0 = (s8)r4;
        if (r0 >= (u32)0x1) {
            r0 = r5 << 1;
            r3 = (u32)sp + 0x20;
            r5 = *(u16*)(r3 + r0);
            r3 = r31;
            r7 = (s8)r4;
            r4 = 0x0;
            r6 = 0x0;
            ((void(*)(void))fn_801254B4)();
        }
        L_801FA250: ;
        r22 = r22 + 0x1;

    }
    r22 = (u32)sp + 0x3c;
    r21 = (u32)sp + 0x14;
    r20 = 0x0;
    while (1) {
        r0 = r20 & 0xFF;
        if (r0 >= (u32)0x6) break;
        /* clrlslwi r3, r20, 24, 1 */;
        r7 = *(s16*)(r22 + r3);
        r0 = (s16)r7;
        if (r0 >= (u32)0x6) {
            r5 = *(u16*)(r21 + r3);
            r3 = r31;
            r4 = 0x0;
            r6 = 0x0;
            ((void(*)(void))fn_801254B4)();
        }
        r20 = r20 + 0x1;

    }
    r0 = (s8)r27;
    if (r0 >= (u32)0x6) {
        r3 = r31;
        r4 = r27 & 0xFF;
        fn_80124978();
    }
    if ((s32)r24 >= (s32)0x0) {
        if ((s32)r24 == (s32)0x0) {
            r3 = r31;
            r4 = 0x0;
            r5 = 0x0;
            fn_80123110();
            goto L_801FA2E8;
        }
        r3 = r31;
        r4 = r24 & 0xFFFF;
        r5 = 0x1;
        fn_80123110();
    }
    L_801FA2E8: ;
    r20 = (u32)sp + 0x2c;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFF;
        if (r0 >= (u32)0x4) break;
        /* clrlslwi r0, r24, 24, 2 */;
        r21 = r24 & 0xFF;
        r3 = *(u32*)(r20 + r0);
        if ((s32)r3 >= (s32)0x0) {
            if ((s32)r3 == (s32)0x0 && (s32)r3 == (s32)0x164 && (s32)r3 == (s32)0x165 && (s32)r3 == (s32)0x163) {

                r3 = r31;
                r4 = r21;
                fn_80125314();
                goto L_801FA368;
            }
            r0 = *(u32*)(r20 + r0);
            r22 = r24 & 0xFF;
            r3 = r31;
            r4 = r22;
            r5 = r0 & 0xFFFF;
            fn_80123D58();
            r4 = (u32)sp + 0x8;
            r3 = r31;
            r7 = *(u8*)(r4 + r21);
            r6 = r22;
            r4 = 0x0;
            r5 = 0x81;
            ((void(*)(void))fn_801254B4)();
        }
        L_801FA368: ;
        r24 = r24 + 0x1;

    }
    r0 = (s16)r26;
    if (r0 >= (u32)0x4) {
        r3 = r31;
        r7 = r26;
        r4 = 0x0;
        r5 = 0x99;
        r6 = 0x0;
        ((void(*)(void))fn_801254B4)();
    }
    if (r30 != (u32)0x0) {
        r3 = r23;
        fn_801EE8F4();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r23;
            fn_801EE750();
            r7 = r3;
            r3 = r31;
            r4 = 0x0;
            r5 = 0x6f;
            r6 = 0x0;
            ((void(*)(void))fn_801254B4)();
            goto L_801FA408;
    }
    }
    r3 = r31;
    r4 = r29;
    r5 = r28;
    r7 = r25;
    r6 = 0x0;
    fn_80124410();
    r7 = r3;
    r3 = r31;
    r4 = 0x0;
    r5 = 0x6f;
    r6 = 0x0;
    ((void(*)(void))fn_801254B4)();
    L_801FA408: ;
    if (r30 != (u32)0x0) {
        r3 = r31;
        r4 = r23;
        fn_8011FCA4();
        r3 = r23;
        fn_801EE8F4();
        r0 = r3 & 0xFF;
        if (r30 == (u32)0x0) {
            r3 = r31;
            r4 = 0x0;
            r5 = 0x6f;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r4 = r3;
            r3 = r23;
            fn_801EE7BC();
            r21 = (u32)sp + 0x20;
            r20 = 0x0;
            while (1) {
                r0 = r20 & 0xFF;
                if (r0 >= (u32)0x6) break;
                /* clrlslwi r0, r20, 24, 1 */;
                r3 = r31;
                r5 = *(u16*)(r21 + r0);
                r4 = 0x0;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
                r0 = r3 & 0xFFFF;
                r3 = r23;
                r4 = r20 & 0xFF;
                r5 = (s8)r0;
                fn_801EE894();
                r20 = r20 + 0x1;

            }
    }
    }
    r3 = r31;
    fn_8012546C();
    r3 = 0x1;

    return;
}

/* 0x801FA4B4 | size: 0x70 */
BOOL fn_801FA4B4(u32 arg0, s32 arg1) {
    extern u32 fn_801FB1C0(void*, u32, u32, u32);
    u32 result;

    result = (u16)fn_801FB1C0(0, arg0, 2, 0);
    if ((u16)arg0 == 0) {
        return FALSE;
    }
    if (result == 0 && arg1 == 0) {
        return FALSE;
    }
    return TRUE;
}

/* 0x801FA524 | size: 0x110 */
u32 fn_801FA524(void* context) {
    extern u16 fn_801EF634(void* ctx);
    extern u8 fn_8012A130(void* ptr);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    extern u8 fn_80206608(void* ptr);
    u8 valid;
    u8 count;
    u8 i;

    valid = 0;
    if (context != NULL) {
        if ((u16)fn_801EF634(context) != 1) {
            if ((s32)fn_801FB1C0(context, 0, 0x43, 0) != 0) {
                if (fn_801FB1C0(context, 0, 0x44, 0) != NULL) {
                    if ((u8)fn_8012A130(fn_801FB1C0(context, 0, 0x44, 0)) != 0) {
                        valid = 1;
                    }
                }
            }
        }
    }
    if (valid == 0) {
        return 0;
    }
    count = 0;
    for (i = count; i < 6; i++) {
        if ((u8)fn_80206608(fn_801FB1C0(context, 0, 0x45, i)) != 0) {
            count = (u8)(count + 1);
        }
    }
    return count != 0;
}

/* 0x801FA634 | size: 0xA4 */
u32 fn_801FA634(void* context) {
    extern u16 fn_801EF634(void* ctx);
    extern u8 fn_8012A130(void* ptr);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    void* result;

    if (context == NULL) {
        return 0;
    }
    if ((u16)fn_801EF634(context) == 1) {
        return 0;
    }
    if ((s32)fn_801FB1C0(context, 0, 0x43, 0) == 0) {
        return 0;
    }
    result = fn_801FB1C0(context, 0, 0x44, 0);
    if (result == NULL) {
        return 0;
    }
    return (u8)fn_8012A130(result) != 0;
}

/* 0x801FA6D8 | size: 0x1F4 | medium */
void fn_801FA6D8(void* trainer, void* arg1, u16 arg2, u32 arg3, u32 arg4) {
    extern void fn_8012A248(void* ptr);
    extern void fn_8012AC64(void* ptr, void* arg);
    extern void fightActionInit(void* ptr);
    extern void fn_801F198C(void);
    extern void fn_801FAA58(void* ctx, u32 slot, u32 field, u32 idx, u32 val);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    extern void fn_80206C3C(void* ptr, u32 count);
    extern void fn_80206C94(void* ptr);
    extern void fn_802077D4(void* ptr, u32 count);
    extern void fn_8020ECE0(void* ptr, u32 count);
    void* result;

    if (trainer != NULL && arg1 != NULL) {
        if (trainer != NULL) {
            fn_8012A248(fn_801FB1C0(trainer, 0, 0x44, 0));
            fn_801FAA58(trainer, 0, 0x43, 0, 0);
            fn_802077D4(fn_801FB1C0(trainer, 0, 0x45, 0), 6);
            fn_80206C3C(fn_801FB1C0(trainer, 0, 0x46, 0), 2);
            fn_80206C94(fn_801FB1C0(trainer, 0, 0x47, 0));
            fn_801FAA58(trainer, 0, 0x48, 0, 1);
            fn_801FAA58(trainer, 0, 0x49, 0, 0);
            fn_801FAA58(trainer, 0, 0x4A, 0, 0);
            fn_801FAA58(trainer, 0, 0x4B, 0, 0);
            fn_801FAA58(trainer, 0, 0x4C, 0, 0);
            result = fn_801FB1C0(trainer, 0, 0x4D, 0);
            if (result != NULL) {
                fightActionInit(result);
            }
            fn_8020ECE0(fn_801FB1C0(trainer, 0, 0x4E, 0), 0xC);
        }
        result = fn_801FB1C0(trainer, 0, 0x44, 0);
        if (result != NULL) {
            fn_8012AC64(result, arg1);
            fn_801FAA58(trainer, 0, 0x43, 0, arg2);
            fn_801FAA58(trainer, 0, 0x4B, 0, arg3);
            if (arg4 != 0) {
                fn_801FAA58(trainer, 0, 0x4C, 0, arg4);
                fn_801F198C();
            }
        }
    }
}

/* 0x801FA8CC | size: 0x18C */
void fn_801FA8CC(void* base, u16 count) {
    extern void fn_8012A248(void* ptr);
    extern void fightActionInit(void* ptr);
    extern void fn_801FAA58(void* ctx, u32 slot, u32 field, u32 idx, u32 val);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    extern void fn_80206C3C(void* ptr, u32 count);
    extern void fn_80206C94(void* ptr);
    extern void fn_802077D4(void* ptr, u32 count);
    extern void fn_8020ECE0(void* ptr, u32 count);
    u8* trainer;
    void* result;
    u16 i;

    if (base == NULL) {
        return;
    }
    for (i = 0; i < count; i++) {
        trainer = (u8*)base + i * 0x28E4;
        if (trainer != NULL) {
            fn_8012A248(fn_801FB1C0(trainer, 0, 0x44, 0));
            fn_801FAA58(trainer, 0, 0x43, 0, 0);
            fn_802077D4(fn_801FB1C0(trainer, 0, 0x45, 0), 6);
            fn_80206C3C(fn_801FB1C0(trainer, 0, 0x46, 0), 2);
            fn_80206C94(fn_801FB1C0(trainer, 0, 0x47, 0));
            fn_801FAA58(trainer, 0, 0x48, 0, 1);
            fn_801FAA58(trainer, 0, 0x49, 0, 0);
            fn_801FAA58(trainer, 0, 0x4A, 0, 0);
            fn_801FAA58(trainer, 0, 0x4B, 0, 0);
            fn_801FAA58(trainer, 0, 0x4C, 0, 0);
            result = fn_801FB1C0(trainer, 0, 0x4D, 0);
            if (result != NULL) {
                fightActionInit(result);
            }
            fn_8020ECE0(fn_801FB1C0(trainer, 0, 0x4E, 0), 0xC);
        }
    }
}

/* 0x801FAA58 | size: 0x768 | large */
void fn_801FAA58(void) {
    extern void fn_801FB1C0();
    extern void fn_801FBD84();
    extern void fn_801FBD94();
    extern void fn_801FBDA4();
    extern void fn_801FBDB4();
    extern void fn_801FBDC4();
    extern void fn_801FBDD4();
    extern void fn_801FBDE4();
    extern void fn_801FBDF4();
    extern void fn_801FBF1C();
    extern void fn_801FBF2C();
    extern void fn_801FBF3C();
    extern void fn_801FBF4C();
    extern void fn_801FBFE8();
    extern void fn_801FBFF8();
    extern void fn_801FC008();
    extern void fn_801FC018();
    extern void fn_801FC028();
    extern void fn_801FC038();
    extern void fn_801FC07C();
    extern void fn_801FC0C0();
    extern void fn_801FC0D0();
    extern void fn_801FC0E0();
    extern void fn_801FC0F0();
    extern void fn_801FC100();
    extern void fn_801FC110();
    extern void fn_801FC120();
    extern void fn_801FC130();
    extern void fn_801FC140();
    extern void fn_801FC150();
    extern void fn_801FC194();
    extern void fn_801FC1D8();
    extern void fn_801FC1E8();
    extern void fn_801FC1F8();
    extern void fn_801FC208();
    extern void fn_801FC218();
    extern void fn_801FC228();
    extern void fn_801FC238();
    extern void fn_801FC248();
    extern void fn_801FC258();
    extern void fn_801FC268();
    extern void fn_801FC278();
    extern void fn_801FC288();
    extern void fn_801FC684();
    extern void fn_801FC694();
    extern void fn_801FC6D4();
    extern void fn_801FC6E4();
    extern void fn_801FC6F4();
    extern void fn_801FC704();
    extern void fn_801FC744();
    extern void fn_801FC784();
    extern void fn_801FC794();
    extern void fn_801FC7A4();
    extern void fn_801FC7B4();
    extern void fn_801FC7C4();
    extern void fn_801FC7D4();
    extern void fn_801FC7E4();
    extern void fn_801FC808();
    extern void fn_801FC828();
    extern void fn_801FCA58();
    extern void fn_801FCA78();
    extern void fn_801FCAFC();
    extern void fn_801FCB0C();
    extern void fn_801FCB30();
    extern void fn_801FCB40();
    extern void fn_801FCB64();
    extern void fn_801FCB74();
    extern void fn_801FCB84();
    extern void fn_801FCB94();
    extern void fn_801FCC54();
    extern void fn_801FCD08();
    extern void fn_801FCD18();
    extern void fn_801FCD28();
    extern void fn_801FCD38();
    extern void fn_801FCD48();
    extern void fn_801FCD8C();
    extern void fn_8020E7AC();
    extern void fn_8020E95C();
    extern void fn_8020EBA8();
    extern void fn_8020EDBC();
    extern u8 jumptable_80375670[];
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r0 = r5 & 0xFFFF;
    r30 = r5;
    r28 = r4;
    r29 = r6;
    r31 = r7;
    if ((s32)r0 == (s32)0) return;
    if (r0 >= (u32)0x5b) {
        return;
    }
    if (r0 < (u32)0xa) {
        r3 = r28;
        ((void(*)(void))fn_801FCCC4)();
        if (r3 == (u32)0x0) return;

    }
    if (r0 < (u32)0xd) {
        r3 = r28;
        ((void(*)(void))fn_801FCAD0)();
        if (r3 == (u32)0x0) return;

    }
    if (r0 < (u32)0x1e) {
        r3 = r28;
        ((void(*)(void))fn_801FCA2C)();
        if (r3 == (u32)0x0) return;

    }
    if (r0 < (u32)0x3d) {
        r3 = r28;
        ((void(*)(void))fn_801FC658)();
        if (r3 == (u32)0x0) return;

    }
    if (r0 < (u32)0x42) {
        r3 = r28;
        ((void(*)(void))fn_801FBFBC)();
        if (r3 == (u32)0x0) return;
    }
    if (r3 == (u32)0x0) return;
    r0 = r30 & 0xFFFF;
    if (r0 > (u32)0x59) return;
    r4 = (u32)jumptable_80375670;
    r0 = r0 << 2;
    r4 = (u32)jumptable_80375670;
    r0 = *(u32*)(r4 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    r4 = r31 & 0xFF;
    fn_801FCB94();
    return;
    r4 = r31 & 0xFFFF;
    fn_801FCB84();
    return;
    r4 = r31;
    fn_801FCB74();
    return;
    r4 = r31 & 0xFFFF;
    fn_801FCC54();
    return;
    r4 = r31 & 0xFFFF;
    fn_801FCB64();
    return;
    r4 = r29 & 0xFF;
    r5 = r31 & 0xFFFF;
    fn_801FCB40();
    return;
    r4 = r31;
    fn_801FCB30();
    return;
    r5 = r31;
    r4 = r29 & 0xFF;
    fn_801FCB0C();
    return;
    r4 = r31;
    fn_801FCAFC();
    return;
    r4 = r31;
    fn_801FCA78();
    return;
    r4 = r29 & 0xFF;
    r5 = r31 & 0xFF;
    fn_801FCA58();
    return;
    r4 = r31;
    fn_801FC828();
    return;
    r4 = r29 & 0xFF;
    r5 = (s8)r31;
    fn_801FC808();
    return;
    r4 = r29 & 0xFF;
    r5 = (s16)r31;
    fn_801FC7E4();
    return;
    r4 = r31 & 0xFF;
    fn_801FC7D4();
    return;
    r4 = r31 & 0xFFFF;
    fn_801FC7C4();
    return;
    r4 = r31 & 0xFF;
    fn_801FC7B4();
    return;
    r4 = (s8)r31;
    fn_801FC7A4();
    return;
    r4 = r31 & 0xFFFF;
    fn_801FC794();
    return;
    r4 = r31;
    fn_801FC784();
    return;
    r5 = r31;
    r4 = r29 & 0xFF;
    fn_801FC744();
    return;
    r4 = r29 & 0xFF;
    r5 = r31 & 0xFF;
    fn_801FC704();
    return;
    r4 = (s16)r31;
    fn_801FC6F4();
    return;
    r4 = (s8)r31;
    fn_801FC6E4();
    return;
    r4 = (s8)r31;
    fn_801FC6D4();
    return;
    r4 = r31 & 0xFF;
    fn_801FC694();
    return;
    r4 = r31 & 0xFF;
    fn_801FC684();
    return;
    r4 = r31 & 0xFF;
    fn_801FC288();
    return;
    r4 = r31 & 0xFF;
    fn_801FC278();
    return;
    r4 = r31 & 0xFF;
    fn_801FC268();
    return;
    r4 = r31 & 0xFF;
    fn_801FC258();
    return;
    r4 = r31 & 0xFF;
    fn_801FC248();
    return;
    r4 = r31 & 0xFF;
    fn_801FC238();
    return;
    r4 = r31 & 0xFF;
    fn_801FC228();
    return;
    r4 = r31 & 0xFF;
    fn_801FC218();
    return;
    r4 = r31 & 0xFF;
    fn_801FC208();
    return;
    r4 = r31 & 0xFF;
    fn_801FC1F8();
    return;
    r4 = r31 & 0xFF;
    fn_801FC1E8();
    return;
    r4 = r31 & 0xFF;
    fn_801FC1D8();
    return;
    r4 = r31 & 0xFF;
    fn_801FC140();
    return;
    r4 = r31 & 0xFF;
    fn_801FC130();
    return;
    r4 = r31 & 0xFF;
    fn_801FC120();
    return;
    r4 = r31 & 0xFF;
    fn_801FC110();
    return;
    r4 = r31 & 0xFF;
    fn_801FC100();
    return;
    r4 = r31 & 0xFF;
    fn_801FC0F0();
    return;
    r4 = r31 & 0xFF;
    fn_801FC0E0();
    return;
    r4 = r31 & 0xFF;
    fn_801FC0D0();
    return;
    r4 = r31 & 0xFF;
    fn_801FC0C0();
    return;
    r4 = r31 & 0xFF;
    fn_801FC028();
    return;
    r4 = r31 & 0xFF;
    fn_801FC018();
    return;
    r4 = r31 & 0xFF;
    fn_801FC008();
    return;
    r4 = r31 & 0xFF;
    fn_801FBFF8();
    return;
    r4 = r31 & 0xFF;
    fn_801FBFE8();
    return;
    r4 = r29;
    r5 = r31 & 0xFF;
    fn_801FC194();
    return;
    r4 = r29;
    r5 = r31 & 0xFF;
    fn_801FC150();
    return;
    r4 = r29;
    r5 = r31 & 0xFF;
    fn_801FC07C();
    return;
    r4 = r29;
    r5 = r31 & 0xFF;
    fn_801FC038();
    return;
    r4 = r31;
    fn_801FBF4C();
    return;
    r4 = r31;
    fn_801FBF3C();
    return;
    r4 = r31;
    fn_801FBF2C();
    return;
    r4 = r31;
    fn_801FBF1C();
    return;
    r4 = r31 & 0xFFFF;
    fn_801FCD48();
    return;
    r4 = r31 & 0xFF;
    fn_801FCD38();
    return;
    r4 = r31;
    fn_801FCD28();
    return;
    r4 = r31 & 0xFF;
    fn_801FCD18();
    return;
    r4 = r31 & 0xFF;
    fn_801FCD08();
    return;
    r4 = r31;
    fn_801FCD8C();
    return;
    r30 = (s16)r28;
    if (r0 >= (u32)0x59) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r29 & 0xFF;
    r5 = r31 & 0xFFFF;
    fn_801FBDF4();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFFFF;
    fn_801FBDE4();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFFFF;
    fn_801FBDD4();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fn_801FBDC4();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fn_801FBDB4();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fn_801FBDA4();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fn_801FBD94();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fn_801FBD84();
    return;
    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 == (u32)0x0) return;
    r5 = (s16)r31;
    r4 = 0xc;
    fn_8020E95C();
    return;
    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 == (u32)0x0) return;
    r5 = (s16)r31;
    r4 = 0xc;
    fn_8020E7AC();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) return;
    fn_8020EDBC();

    return;
}

/* 0x801FB1C0 | size: 0x738 | large */
void fn_801FB1C0(void) {
    extern void fn_801FB1C0();
    extern void fn_801FBE28();
    extern void fn_801FBE40();
    extern void fn_801FBE58();
    extern void fn_801FBE70();
    extern void fn_801FBE88();
    extern void fn_801FBEA0();
    extern void fn_801FBEB8();
    extern void fn_801FBED0();
    extern void fn_801FBF5C();
    extern void fn_801FBF74();
    extern void fn_801FBF8C();
    extern void fn_801FBFA4();
    extern void fn_801FC298();
    extern void fn_801FC2B0();
    extern void fn_801FC2C8();
    extern void fn_801FC2E0();
    extern void fn_801FC2F8();
    extern void fn_801FC310();
    extern void fn_801FC364();
    extern void fn_801FC3B8();
    extern void fn_801FC3D0();
    extern void fn_801FC3E8();
    extern void fn_801FC400();
    extern void fn_801FC418();
    extern void fn_801FC430();
    extern void fn_801FC448();
    extern void fn_801FC460();
    extern void fn_801FC478();
    extern void fn_801FC490();
    extern void fn_801FC4E4();
    extern void fn_801FC538();
    extern void fn_801FC550();
    extern void fn_801FC568();
    extern void fn_801FC580();
    extern void fn_801FC598();
    extern void fn_801FC5B0();
    extern void fn_801FC5C8();
    extern void fn_801FC5E0();
    extern void fn_801FC5F8();
    extern void fn_801FC610();
    extern void fn_801FC628();
    extern void fn_801FC640();
    extern void fn_801FC6A4();
    extern void fn_801FC6BC();
    extern void fn_801FC838();
    extern void fn_801FC854();
    extern void fn_801FC870();
    extern void fn_801FC888();
    extern void fn_801FC8D0();
    extern void fn_801FC918();
    extern void fn_801FC930();
    extern void fn_801FC948();
    extern void fn_801FC964();
    extern void fn_801FC97C();
    extern void fn_801FC994();
    extern void fn_801FC9AC();
    extern void fn_801FC9E0();
    extern void fn_801FCA14();
    extern void fn_801FCA88();
    extern void fn_801FCAB8();
    extern void fn_801FCBA4();
    extern void fn_801FCBBC();
    extern void fn_801FCBF0();
    extern void fn_801FCC08();
    extern void fn_801FCC3C();
    extern void fn_801FCC64();
    extern void fn_801FCC7C();
    extern void fn_801FCC94();
    extern void fn_801FCCAC();
    extern void fn_801FCCF0();
    extern void fn_801FCD58();
    extern void fn_801FCD9C();
    extern void fn_801FCDB4();
    extern void fn_801FCDCC();
    extern void fn_801FCDE4();
    extern void fn_801FCDFC();
    extern void fn_801FCE14();
    extern void fn_801FCE2C();
    extern void fn_801FCE60();
    extern void fn_801FCE94();
    extern void fn_801FCEAC();
    extern void fn_8020EBA8();
    extern u8 jumptable_803757D8[];
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r0 = r5 & 0xFFFF;
    r31 = r5;
    r29 = r4;
    r30 = r6;
    if ((s32)r0 == (s32)0) { r3 = 0x0; return; }
    if (r0 >= (u32)0x5b) {

        r3 = 0x0;
        return;
    }
    if (r0 < (u32)0xa) {
        r3 = r29;
        ((void(*)(void))fn_801FCCC4)();
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            return;
        }
        if (r0 < (u32)0xd) {
            r3 = r29;
            ((void(*)(void))fn_801FCAD0)();
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                return;
            }
            if (r0 < (u32)0x1e) {
                r3 = r29;
                ((void(*)(void))fn_801FCA2C)();
                if (r3 == (u32)0x0) {
                    r3 = 0x0;
                    return;
                }
                if (r0 < (u32)0x3d) {
                    r3 = r29;
                    ((void(*)(void))fn_801FC658)();
        }
        }
        }
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            return;
        }
        if (r0 < (u32)0x42) {
            r3 = r29;
            ((void(*)(void))fn_801FBFBC)();
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                return;
        }
        }
        }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    r0 = r31 & 0xFFFF;
    if (r0 > (u32)0x56) { r3 = 0x0; return; }
    r4 = (u32)jumptable_803757D8;
    r0 = r0 << 2;
    r4 = (u32)jumptable_803757D8;
    r0 = *(u32*)(r4 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    fn_801FCCAC();
    r3 = r3 & 0xFF;
    return;
    fn_801FCC94();
    r3 = r3 & 0xFFFF;
    return;
    fn_801FCC7C();
    return;
    fn_801FCC64();
    r3 = r3 & 0xFFFF;
    return;
    fn_801FCC3C();
    r3 = r3 & 0xFFFF;
    return;
    r4 = r30 & 0xFF;
    fn_801FCC08();
    r3 = r3 & 0xFFFF;
    return;
    fn_801FCBF0();
    return;
    r4 = r30 & 0xFF;
    fn_801FCBBC();
    return;
    fn_801FCBA4();
    return;
    fn_801FCAB8();
    return;
    r4 = r30 & 0xFF;
    fn_801FCA88();
    r3 = r3 & 0xFF;
    return;
    fn_801FCA14();
    return;
    r4 = r30 & 0xFF;
    fn_801FC9E0();
    r3 = (s8)r3;
    return;
    r4 = r30 & 0xFF;
    fn_801FC9AC();
    r3 = (s16)r3;
    return;
    fn_801FC994();
    r3 = r3 & 0xFF;
    return;
    fn_801FC97C();
    r3 = r3 & 0xFFFF;
    return;
    fn_801FC964();
    r3 = r3 & 0xFF;
    return;
    fn_801FC948();
    r3 = (s8)r3;
    return;
    fn_801FC930();
    r3 = r3 & 0xFFFF;
    return;
    fn_801FC918();
    return;
    r4 = r30 & 0xFF;
    fn_801FC8D0();
    return;
    r4 = r30 & 0xFF;
    fn_801FC888();
    r3 = r3 & 0xFF;
    return;
    fn_801FC870();
    r3 = (s16)r3;
    return;
    fn_801FC854();
    r3 = (s8)r3;
    return;
    fn_801FC838();
    r3 = (s8)r3;
    return;
    fn_801FC6BC();
    r3 = r3 & 0xFF;
    return;
    fn_801FC6A4();
    r3 = r3 & 0xFF;
    return;
    fn_801FC640();
    r3 = r3 & 0xFF;
    return;
    fn_801FC628();
    r3 = r3 & 0xFF;
    return;
    fn_801FC610();
    r3 = r3 & 0xFF;
    return;
    fn_801FC5F8();
    r3 = r3 & 0xFF;
    return;
    fn_801FC5E0();
    r3 = r3 & 0xFF;
    return;
    fn_801FC5C8();
    r3 = r3 & 0xFF;
    return;
    fn_801FC5B0();
    r3 = r3 & 0xFF;
    return;
    fn_801FC598();
    r3 = r3 & 0xFF;
    return;
    fn_801FC580();
    r3 = r3 & 0xFF;
    return;
    fn_801FC568();
    r3 = r3 & 0xFF;
    return;
    fn_801FC550();
    r3 = r3 & 0xFF;
    return;
    fn_801FC538();
    r3 = r3 & 0xFF;
    return;
    fn_801FC478();
    r3 = r3 & 0xFF;
    return;
    fn_801FC460();
    r3 = r3 & 0xFF;
    return;
    fn_801FC448();
    r3 = r3 & 0xFF;
    return;
    fn_801FC430();
    r3 = r3 & 0xFF;
    return;
    fn_801FC418();
    r3 = r3 & 0xFF;
    return;
    fn_801FC400();
    r3 = r3 & 0xFF;
    return;
    fn_801FC3E8();
    r3 = r3 & 0xFF;
    return;
    fn_801FC3D0();
    r3 = r3 & 0xFF;
    return;
    fn_801FC3B8();
    r3 = r3 & 0xFF;
    return;
    fn_801FC2F8();
    r3 = r3 & 0xFF;
    return;
    fn_801FC2E0();
    r3 = r3 & 0xFF;
    return;
    fn_801FC2C8();
    r3 = r3 & 0xFF;
    return;
    fn_801FC2B0();
    r3 = r3 & 0xFF;
    return;
    fn_801FC298();
    r3 = r3 & 0xFF;
    return;
    r4 = r30;
    fn_801FC4E4();
    r3 = r3 & 0xFF;
    return;
    r4 = r30;
    fn_801FC490();
    r3 = r3 & 0xFF;
    return;
    r4 = r30;
    fn_801FC364();
    r3 = r3 & 0xFF;
    return;
    r4 = r30;
    fn_801FC310();
    r3 = r3 & 0xFF;
    return;
    fn_801FBFA4();
    return;
    fn_801FBF8C();
    return;
    fn_801FBF74();
    return;
    fn_801FBF5C();
    return;
    fn_801FCEAC();
    return;
    fn_801FCE94();
    r3 = r3 & 0xFFFF;
    return;
    r4 = r30;
    fn_801FCE60();
    return;
    r4 = r30;
    fn_801FCE2C();
    return;
    fn_801FCE14();
    return;
    fn_801FCDFC();
    r3 = r3 & 0xFF;
    return;
    fn_801FCDE4();
    return;
    fn_801FCDCC();
    r3 = r3 & 0xFF;
    return;
    fn_801FCDB4();
    r3 = r3 & 0xFF;
    return;
    fn_801FCCF0();
    return;
    fn_801FCD9C();
    return;
    r4 = r30 & 0xFF;
    fn_801FCD58();
    return;
    r31 = (s16)r29;
    if (r0 >= (u32)0x56) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    r4 = r30 & 0xFF;
    fn_801FBED0();
    r3 = r3 & 0xFFFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fn_801FBEB8();
    r3 = r3 & 0xFFFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fn_801FBEA0();
    r3 = r3 & 0xFFFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fn_801FBE88();
    r3 = r3 & 0xFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fn_801FBE70();
    r3 = r3 & 0xFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fn_801FBE58();
    r3 = r3 & 0xFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fn_801FBE40();
    r3 = r3 & 0xFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fn_801FB1C0();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fn_8020EBA8();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fn_801FBE28();
    r3 = r3 & 0xFF;
    return;

    r3 = 0x0;

    return;
}

/* 0x801FB8F8 | size: 0x7C */
u8 fn_801FB8F8(void* arg0) {
    extern u32 fn_801FB1C0(void*, u32, u32, u32);
    u32 val;
    u32 category;

    val = (u16)fn_801FB1C0(arg0, 0, 0x43, 0);
    val = (u16)fn_801FB1C0(0, val, 4, 0);
    if (val == 1) {
        category = 0;
    } else if (val == 2 || val == 3) {
        category = 1;
    } else {
        category = 2;
    }
    return (u8)category == 0;
}

/* 0x801FB974 | size: 0xB0 */
void fn_801FB974(void* context, u8 mode) {
    extern void _threadSwitch(void);
    extern void fn_801DA8C4(void* ptr, u32 field, u32 size);
    extern u8 fn_801DA94C(void* ptr, u32 field, u32 size);
    extern void fn_801DA9E8(void* ptr, u32 field, u32 size);
    extern void fn_801DDD28(void* ptr, u32 field, u32 size, u32 flags);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    void* data;

    if ((data = fn_801FB1C0(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    if (mode == 0) {
        fn_801DDD28(data, 0x57, 4, 0);
    } else if (mode == 1) {
        fn_801DA9E8(data, 0x57, 4);
    } else if (mode == 2) {
        do {
            if ((u8)fn_801DA94C(data, 0x57, 4) == 0) {
                return;
            }
            _threadSwitch();
        } while (1);
    } else if (mode == 3) {
        fn_801DA8C4(data, 0x57, 4);
    }
}

/* 0x801FBA24 | size: 0xB0 */
void fn_801FBA24(void* context, u8 mode) {
    extern void _threadSwitch(void);
    extern void fn_801DA8C4(void* ptr, u32 field, u32 size);
    extern u8 fn_801DA94C(void* ptr, u32 field, u32 size);
    extern void fn_801DA9E8(void* ptr, u32 field, u32 size);
    extern void fn_801DDD28(void* ptr, u32 field, u32 size, u32 flags);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    void* data;

    if ((data = fn_801FB1C0(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    if (mode == 0) {
        fn_801DDD28(data, 0x74, 4, 0);
    } else if (mode == 1) {
        fn_801DA9E8(data, 0x74, 4);
    } else if (mode == 2) {
        do {
            if ((u8)fn_801DA94C(data, 0x74, 4) == 0) {
                return;
            }
            _threadSwitch();
        } while (1);
    } else if (mode == 3) {
        fn_801DA8C4(data, 0x74, 4);
    }
}

/* 0x801FBAD4 | size: 0x14C */
void fn_801FBAD4(void* context, u32 param, u8 mode) {
    extern void _threadSwitch(void* ptr);
    extern void fn_801DA8C4(void* ptr, u32 field, u32 size);
    extern u8 fn_801DA94C(void* ptr, u32 field, u32 size);
    extern void fn_801DA9E8(void* ptr, u32 field, u32 size);
    extern void fn_801DDD28(void* ptr, u32 field, u32 size, u32 flags);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    void* data;
    u16 animId;

    if ((data = fn_801FB1C0(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    animId = (u16)itemGetStatus(0, param, 0x12, 0);
    if (mode == 0) {
        fn_801DDD28(data, animId, 4, 0);
    } else if (mode == 1) {
        fn_801DA9E8(data, animId, 4);
        do {
            if ((u8)fn_801DA94C(data, animId, 4) == 0) {
                return;
            }
            _threadSwitch(data);
        } while (1);
    } else if (mode == 2) {
        fn_801DA8C4(data, animId, 4);
    } else if (mode == 3) {
        fn_801DDD28(data, 0x57, 4, 0);
    } else if (mode == 4) {
        fn_801DA9E8(data, 0x57, 4);
    } else if (mode == 5) {
        do {
            if ((u8)fn_801DA94C(data, 0x57, 4) == 0) {
                return;
            }
            _threadSwitch(data);
        } while (1);
    } else if (mode == 6) {
        fn_801DA8C4(data, 0x57, 4);
    }
}

/* 0x801FBC20 | size: 0xF0 */
void fn_801FBC20(void* context, void* trainerCtx, u8 mode) {
    extern void _threadSwitch(void* ptr);
    extern void fn_801DA8C4(void* ptr, u32 field, u32 size);
    extern u8 fn_801DA94C(void* ptr, u32 field, u32 size);
    extern void fn_801DA9E8(void* ptr, u32 field, u32 size);
    extern void fn_801DDD28(void* ptr, u32 field, u32 size, u32 flags);
    extern void* fn_801FB1C0(void* ctx, u32 slot, u32 field, u32 idx);
    extern void* fn_80205B8C(void* ctx);
    void* data;
    u8 typeId;
    u16 animId;

    if ((data = fn_801FB1C0(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    typeId = (u8)(u32)fn_8012640C(fn_80205B8C(trainerCtx), 0, 0x73, 0);
    animId = (u16)itemGetStatus(0, typeId, 0x11, 0);
    if (mode == 0) {
        fn_801DDD28(data, animId, 4, 0);
    } else if (mode == 1) {
        fn_801DA9E8(data, animId, 4);
    } else if (mode == 2) {
        do {
            if ((u8)fn_801DA94C(data, animId, 4) == 0) {
                return;
            }
            _threadSwitch(data);
        } while (1);
    } else if (mode == 3) {
        fn_801DA8C4(data, animId, 4);
    }
}

/* 0x801FBD58 | size: 0x2C */
u8* fn_801FBD58(u16 idx) {
    idx = idx;
    if (idx >= lbl_80478F18[0]) { return NULL; }
    return lbl_80478F1C + idx * 0xC;
}

/* 0x801FBDF4 | size: 0x24 */
void fn_801FBDF4(u8* ptr, u8 idx, u16 val) {
    if (ptr == NULL) { return; }
    if (idx > 4) { return; }
    ptr += idx * 2;
    *(u16*)(ptr + 0x2) = val;
}

/* 0x801FBED0 | size: 0x34 */
u16 fn_801FBED0(u8* ptr, u8 idx) {
    if (ptr == NULL) { return 0; }
    if (idx > 4) { return 0; }
    ptr += idx * 2;
    return *(u16*)(ptr + 0x2);
}

/* 0x801FC038 | size: 0x44 */
void fn_801FC038(u8* ptr, u16 idx, u8 val) {
    u8* entry;
    if (ptr == NULL) return;
    if (ptr == NULL) {
        entry = NULL;
    } else if (idx >= 2) {
        entry = NULL;
    } else {
        entry = ptr + idx * 2 + 0x24;
    }
    if (entry == NULL) return;
    entry[1] = val;
}

/* 0x801FC07C | size: 0x44 */
void fn_801FC07C(u8* ptr, u16 idx, u8 val) {
    u8* entry;
    if (ptr == NULL) return;
    if (ptr == NULL) {
        entry = NULL;
    } else if (idx >= 2) {
        entry = NULL;
    } else {
        entry = ptr + idx * 2 + 0x24;
    }
    if (entry == NULL) return;
    entry[0] = val;
}

/* 0x801FC150 | size: 0x44 */
void fn_801FC150(u8* ptr, u16 idx, u8 val) {
    u8* entry;
    if (ptr == NULL) return;
    if (ptr == NULL) {
        entry = NULL;
    } else if (idx >= 2) {
        entry = NULL;
    } else {
        entry = ptr + idx * 2 + 0x20;
    }
    if (entry == NULL) return;
    entry[1] = val;
}

/* 0x801FC194 | size: 0x44 */
void fn_801FC194(u8* ptr, u16 idx, u8 val) {
    u8* entry;
    if (ptr == NULL) return;
    if (ptr == NULL) {
        entry = NULL;
    } else if (idx >= 2) {
        entry = NULL;
    } else {
        entry = ptr + idx * 2 + 0x20;
    }
    if (entry == NULL) return;
    entry[0] = val;
}

/* 0x801FC310 | size: 0x54 */
u32 fn_801FC310(u8* ptr, u16 idx) {
    u8* entry;
    if (ptr == NULL) {
        return 0;
    }
    if (ptr == NULL) {
        entry = NULL;
    } else if (idx >= 2) {
        entry = NULL;
    } else {
        entry = ptr + idx * 2 + 0x24;
    }
    if (entry == NULL) {
        return 0;
    }
    return entry[1];
}

/* 0x801FC364 | size: 0x54 */
u32 fn_801FC364(u8* ptr, u16 idx) {
    u8* entry;
    if (ptr == NULL) {
        return 0;
    }
    if (ptr == NULL) {
        entry = NULL;
    } else if (idx >= 2) {
        entry = NULL;
    } else {
        entry = ptr + idx * 2 + 0x24;
    }
    if (entry == NULL) {
        return 0;
    }
    return entry[0];
}

/* 0x801FC490 | size: 0x54 */
u32 fn_801FC490(u8* ptr, u16 idx) {
    u8* entry;
    if (ptr == NULL) {
        return 0;
    }
    if (ptr == NULL) {
        entry = NULL;
    } else if (idx >= 2) {
        entry = NULL;
    } else {
        entry = ptr + idx * 2 + 0x20;
    }
    if (entry == NULL) {
        return 0;
    }
    return entry[1];
}

/* 0x801FC4E4 | size: 0x54 */
u32 fn_801FC4E4(u8* ptr, u16 idx) {
    u8* entry;
    if (ptr == NULL) {
        return 0;
    }
    if (ptr == NULL) {
        entry = NULL;
    } else if (idx >= 2) {
        entry = NULL;
    } else {
        entry = ptr + idx * 2 + 0x20;
    }
    if (entry == NULL) {
        return 0;
    }
    return entry[0];
}

/* 0x801FC704 | size: 0x40 */
void fn_801FC704(u8* ptr, u8 idx, u8 val) {
    u8* entry;
    if (ptr == NULL) {
        entry = NULL;
    } else if (idx >= 4) {
        entry = NULL;
    } else {
        entry = ptr + idx * 8 + 0x30;
    }
    if (entry == 0) { return; }
    entry[0] = val;
}

/* 0x801FC744 | size: 0x40 */
void fn_801FC744(u8* ptr, u8 idx, u32 val) {
    u8* entry;
    if (ptr == NULL) {
        entry = NULL;
    } else if (idx >= 4) {
        entry = NULL;
    } else {
        entry = ptr + idx * 8 + 0x30;
    }
    if (entry == 0) { return; }
    *(u32*)(entry + 4) = val;
}

/* 0x801FC7E4 | size: 0x24 */
void fn_801FC7E4(u8* ptr, u8 idx, u16 val) {
    if (ptr == NULL) { return; }
    if (idx >= 6) { return; }
    ptr += idx * 2;
    *(u16*)(ptr + 0x22) = val;
}

/* 0x801FC808 | size: 0x20 */
void fn_801FC808(u8* ptr, u8 idx, u8 val) {
    if (ptr == NULL) { return; }
    if (idx >= 6) { return; }
    ptr += idx;
    *(ptr + 0x1C) = val;
}

/* 0x801FC838 | size: 0x1C */
s32 fn_801FC838(u8* ptr) {
    if (ptr == NULL) { return -1; }
    return (s8)ptr[0x2];
}

/* 0x801FC854 | size: 0x1C */
s32 fn_801FC854(u8* ptr) {
    if (ptr == NULL) { return -1; }
    return (s8)ptr[0x1];
}

/* 0x801FC870 | size: 0x18 */
s32 fn_801FC870(u8* ptr) {
    if (ptr == NULL) { return -1; }
    return *(s16*)(ptr + 0x8);
}

/* 0x801FC888 | size: 0x48 */
u32 fn_801FC888(u8* ptr, u8 idx) {
    u8* entry;
    if (ptr == NULL) {
        entry = NULL;
    } else if (idx >= 4) {
        entry = NULL;
    } else {
        entry = ptr + idx * 8 + 0x30;
    }
    if (entry == NULL) {
        return 0;
    }
    return entry[0];
}

/* 0x801FC8D0 | size: 0x48 */
s32 fn_801FC8D0(u8* ptr, u8 idx) {
    u8* entry;
    if (ptr == NULL) {
        entry = NULL;
    } else if (idx >= 4) {
        entry = NULL;
    } else {
        entry = ptr + idx * 8 + 0x30;
    }
    if (entry == NULL) {
        return -1;
    }
    return *(s32*)(entry + 4);
}

/* 0x801FC918 | size: 0x18 */
s32 fn_801FC918(u8* ptr) {
    if (ptr == NULL) { return -1; }
    return *(s32*)(ptr + 0x10);
}

/* 0x801FC948 | size: 0x1C */
s32 fn_801FC948(u8* ptr) {
    if (ptr == NULL) { return -1; }
    return (s8)ptr[0x0];
}

/* 0x801FC9AC | size: 0x34 */
s32 fn_801FC9AC(u8* ptr, u8 idx) {
    if (ptr == NULL) { return -1; }
    if (idx >= 6) { return -1; }
    ptr += idx * 2;
    return *(s16*)(ptr + 0x22);
}

/* 0x801FC9E0 | size: 0x34 */
s32 fn_801FC9E0(u8* ptr, u8 idx) {
    if (ptr == NULL) { return -1; }
    if (idx >= 6) { return -1; }
    ptr += idx;
    return (s8)*(ptr + 0x1C);
}

/* 0x801FCA58 | size: 0x20 */
void fn_801FCA58(u8* ptr, u8 idx, u8 val) {
    if (ptr == NULL) { return; }
    if (idx >= 0xC) { return; }
    ptr += idx;
    *(ptr + 0x8) = val;
}

/* 0x801FCA88 | size: 0x30 */
u32 fn_801FCA88(u8* ptr, u8 idx) {
    if (ptr == NULL) { return 0; }
    if (idx >= 0xC) { return 0; }
    ptr += idx;
    return *(ptr + 0x8);
}

/* 0x801FCB0C | size: 0x24 */
void fn_801FCB0C(u8* ptr, u8 idx, u32 val) {
    if (ptr == NULL) { return; }
    if (idx >= 4) { return; }
    ptr += idx * 4;
    *(u32*)(ptr + 0x24) = val;
}

/* 0x801FCB40 | size: 0x24 */
void fn_801FCB40(u8* ptr, u8 idx, u16 val) {
    if (ptr == NULL) { return; }
    if (idx >= 8) { return; }
    ptr += idx * 2;
    *(u16*)(ptr + 0x14) = val;
}

/* 0x801FCBBC | size: 0x34 */
u32 fn_801FCBBC(u8* ptr, u8 idx) {
    if (ptr == NULL) { return 0; }
    if (idx >= 4) { return 0; }
    ptr += idx * 4;
    return *(u32*)(ptr + 0x24);
}

/* 0x801FCC08 | size: 0x34 */
u32 fn_801FCC08(u8* ptr, u8 idx) {
    if (ptr == NULL) { return 0; }
    if (idx >= 8) { return 0; }
    ptr += idx * 2;
    return *(u16*)(ptr + 0x14);
}

/* 0x801FCCF0 | size: 0x18 */
u8* fn_801FCCF0(u8* ptr) {
    if (ptr == NULL) { return NULL; }
    return ptr + 0x27C4;
}

/* 0x801FCD58 | size: 0x34 */
u8* fn_801FCD58(u8* ptr, u8 idx) {
    if (ptr == NULL) { return NULL; }
    if (idx >= 0xC) { return NULL; }
    return ptr + idx * 0x14 + 0x27F4;
}

/* 0x801FCE14 | size: 0x18 */
u8* fn_801FCE14(u8* ptr) {
    if (ptr == NULL) { return NULL; }
    return ptr + 0x20D4;
}

/* 0x801FCE2C | size: 0x34 */
u8* fn_801FCE2C(u8* ptr, u16 idx) {
    if (ptr == NULL) { return NULL; }
    if (idx >= 2) { return NULL; }
    return ptr + idx * 0x6E0 + 0x1314;
}

/* 0x801FCE60 | size: 0x34 */
u8* fn_801FCE60(u8* ptr, u16 idx) {
    if (ptr == NULL) { return NULL; }
    if (idx >= 6) { return NULL; }
    return ptr + idx * 0x154 + 0xB1C;
}

/* 0x801FCEAC | size: 0x18 */
u8* fn_801FCEAC(u8* ptr) {
    if (ptr == NULL) { return NULL; }
    return ptr + 0x4;
}

/* 0x801FCEC4 | size: 0x38 */
void fn_801FCEC4(u32* dst, u32* src) {
    s32 i;
    if (dst == 0) { return; }
    if (src == 0) { return; }
    for (i = 0; i < 0xDC; i++) {
        dst[i * 2] = src[i * 2];
        dst[i * 2 + 1] = src[i * 2 + 1];
    }
}

/* 0x801FCF8C | size: 0x18 */
u8* fn_801FCF8C(u8* ptr) {
    if (ptr == NULL) { return NULL; }
    return ptr + 0x548;
}

/* 0x801FCFEC | size: 0x18 */
u32 fn_801FCFEC(u8* ptr) {
    if (ptr == NULL) { return 9; }
    return *(u16*)(ptr + 0x672);
}

/* 0x801FD11C | size: 0x34 */
u8* fn_801FD11C(u8* ptr, u16 idx) {
    if (ptr == NULL) { return NULL; }
    if (idx >= 4) { return NULL; }
    return ptr + idx * 0xC + 0x6B0;
}

/* 0x801FD160 | size: 0x18 */
s32 fn_801FD160(u8* ptr) {
    if (ptr == NULL) { return -1; }
    return *(s16*)(ptr + 0x6AE);
}

/* 0x801FD5B0 | size: 0x18 */
u8* fn_801FD5B0(u8* ptr) {
    if (ptr == NULL) { return NULL; }
    return ptr + 0x664;
}

/* 0x801FD5F0 | size: 0x24 */
void fn_801FD5F0(u8* ptr, u8 idx, u16 val) {
    if (ptr == NULL) { return; }
    if (idx >= 2) { return; }
    ptr += idx * 2;
    *(u16*)(ptr + 0x65C) = val;
}

/* 0x801FD614 | size: 0x34 */
u32 fn_801FD614(u8* ptr, u8 idx) {
    if (ptr == NULL) { return 0; }
    if (idx >= 2) { return 0; }
    ptr += idx * 2;
    return *(u16*)(ptr + 0x65C);
}

/* 0x801FD648 | size: 0x18 */
u8* fn_801FD648(u8* ptr) {
    if (ptr == NULL) { return NULL; }
    return ptr + 0x62C;
}

/* 0x801FD660 | size: 0x24 */
void fn_801FD660(u8* ptr, u8 idx, u16 val) {
    if (ptr == NULL) { return; }
    if (idx >= 0xC) { return; }
    ptr += idx * 2;
    *(u16*)(ptr + 0x612) = val;
}

/* 0x801FD684 | size: 0x34 */
s32 fn_801FD684(u8* ptr, u8 idx) {
    if (ptr == NULL) { return -1; }
    if (idx >= 0xC) { return -1; }
    ptr += idx * 2;
    return *(s16*)(ptr + 0x612);
}

/* 0x801FDA08 | size: 0x18 */
u8* fn_801FDA08(u8* ptr) {
    if (ptr == NULL) { return NULL; }
    return ptr + 0x5F4;
}

/* 0x801FDA20 | size: 0x18 */
u8* fn_801FDA20(u8* ptr) {
    if (ptr == NULL) { return NULL; }
    return ptr + 0x49C;
}

/* 0x801FDA38 | size: 0x34 */
u8* fn_801FDA38(u8* ptr, u16 idx) {
    if (ptr == NULL) { return NULL; }
    if (idx >= 0x34) { return NULL; }
    return ptr + idx * 0x10 + 0x15C;
}

/* 0x801FDA6C | size: 0x18 */
u8* fn_801FDA6C(u8* ptr) {
    if (ptr == NULL) { return NULL; }
    return ptr + 0x8;
}

/* 0x801FDAFC | size: 0x18 */
s32 fn_801FDAFC(u8* ptr) {
    if (ptr == NULL) { return -1; }
    return *(s16*)(ptr + 0x14C);
}

/* 0x801FDB14 | size: 0x34 */
u8* fn_801FDB14(u8* ptr, u16 idx) {
    if (ptr == NULL) { return NULL; }
    if (idx >= 1) { return NULL; }
    return ptr + idx * 0x10 + 0x13C;
}

/* 0x801FDB48 | size: 0x18 */
u8* fn_801FDB48(u8* ptr) {
    if (ptr == NULL) { return NULL; }
    return ptr + 0x4;
}

/* 0x801FDB78 | size: 0x5F0 | large */
void fn_801FDB78(void) {
    extern void fn_80119ED0();
    extern void fn_8011A3E4();
    extern void fn_8011A6D4();
    extern void fn_8011ACB4();
    extern void fn_8011AE40();
    extern void fn_8011B67C();
    extern void fn_80121574();
    extern void fn_8012165C();
    extern void fn_8012182C();
    extern void fn_8012189C();
    extern void fn_80121ADC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r31 = r3;
    r28 = r4;
    r27 = r5;
    r3 = 0x10;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x10;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xc8) {
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xcd) {
        }
        }
        r3 = r31;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r26 = r3;
        r3 = 0x10;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xc8) {
            }
            if (r26 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r3 = r26;
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r4 = 0x10;
            fn_80121ADC();
            goto L_801FDC94;
            }
        r3 = 0x10;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xcd) {
            r3 = 0x0;
            goto L_801FDC94;
        }
        r3 = r26;
        r4 = 0x10;
        fn_8011B67C();

            } else {
        r3 = 0x10;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8) {
            r3 = 0x0;

        } else {
            r3 = r31;
            r4 = 0x10;
            fn_8011B67C();
        }
            }
    L_801FDC94: ;
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x10;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xc8) {
                r3 = 0x10;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xcd) {
            }
            }
            r3 = r31;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r26 = r3;
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x7c) {
                r3 = 0x10;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xc8) {
                }
                if (r26 == (u32)0x0) {
                    r3 = 0x0;
                } else {

                    r3 = r26;
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                }
                r4 = 0x10;
                fn_80121574();
                goto L_801FDDA0;
                }
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) {
                r3 = 0x0;
                goto L_801FDDA0;
            }
            r3 = r26;
            r4 = 0x10;
            fn_8011A3E4();

                } else {
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xd8) {
                r3 = 0x0;

            } else {
                r3 = r31;
                r4 = 0x10;
                fn_8011A3E4();
            }
                }
        L_801FDDA0: ;
        r30 = r3;
        r3 = 0x10;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xc8) {
                r3 = 0x10;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xcd) {
            }
            }
            r3 = r31;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r26 = r3;
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x7c) {
                r3 = 0x10;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xc8) {
                }
                if (r26 == (u32)0x0) {
                    r3 = 0x0;
                } else {

                    r3 = r26;
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                }
                r4 = 0x10;
                fn_8012189C();
                goto L_801FDEA4;
                }
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) {
                r3 = -0x1;
                goto L_801FDEA4;
            }
            r3 = r26;
            r4 = 0x10;
            fn_8011AE40();

                } else {
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xd8) {
                r3 = -0x1;

            } else {
                r3 = r31;
                r4 = 0x10;
                fn_8011AE40();
            }
                }
        L_801FDEA4: ;
        r29 = r3 & 0xFF;
        r3 = 0x10;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xc8) {
                r3 = 0x10;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xcd) {
            }
            }
            r3 = r31;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r26 = r3;
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x7c) {
                r3 = 0x10;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xc8) {
                }
                if (r26 == (u32)0x0) {
                    r3 = 0x0;
                } else {

                    r3 = r26;
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                }
                r4 = 0x10;
                fn_8012182C();
                goto L_801FDFA8;
                }
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) {
                r3 = -0x1;
                goto L_801FDFA8;
            }
            r3 = r26;
            r4 = 0x10;
            fn_8011ACB4();

                } else {
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xd8) {
                r3 = -0x1;

            } else {
                r3 = r31;
                r4 = 0x10;
                fn_8011ACB4();
            }
                }
        L_801FDFA8: ;
        r26 = r3 & 0xFF;
        r3 = 0x10;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xc8) {
                r3 = 0x10;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xcd) {
            }
            }
            r3 = r31;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r31 = r3;
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x7c) {
                r3 = 0x10;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xc8) {
                }
                if (r31 == (u32)0x0) {
                    r3 = 0x0;
                } else {

                    r3 = r31;
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                }
                r4 = 0x10;
                fn_8012165C();
                goto L_801FE0AC;
                }
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) {
                r3 = 0x0;
                goto L_801FE0AC;
            }
            r3 = r31;
            r4 = 0x10;
            fn_8011A6D4();

                } else {
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xd8) {
                r3 = 0x0;

            } else {
                r3 = r31;
                r4 = 0x10;
                fn_8011A6D4();
            }
                }
        L_801FE0AC: ;
        /* clrlslwi r0, r26, 24, 8 */;
        r0 = (r0 & ~0x000000FF) | (((r29 << 0) | (r29 >> 32)) & 0x000000FF);
        r26 = r0;
        r26 = (r26 & ~0xFFFF0000) | (((r3 << 16) | (r3 >> 16)) & 0xFFFF0000);

    } else {
        if (r31 == (u32)0x0) {
            r26 = 0x0;
        } else {

            r3 = r31;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            if (r3 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r26 = r3;
        }
        r3 = r26;
        r4 = 0x0;
        r5 = 0x75;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r0 = r3;
        r3 = r26;
        r26 = r0;
        r4 = 0x0;
        r5 = 0x6f;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r30 = r3;
    }
    if (r28 != (u32)0x0) {
        *(u32*)((u8*)r28 + 0x0) = r30;
    }
    if (r27 != (u32)0x0) {
        *(u32*)((u8*)r27 + 0x0) = r26;
    }
    return;
}

/* 0x801FE168 | size: 0x290 | large */
void fn_801FE168(void) {
    extern void fn_8001D994();
    extern void fn_80119ED0();
    extern void fn_8011B67C();
    extern void fn_8011F5FC();
    extern void fn_8011FDC8();
    extern void fn_801202CC();
    extern void fn_80121ADC();
    extern void fn_801F4354();
    extern void fn_801F54A4();
    extern void fn_801FB8F8();
    extern void fn_801FDB78();
    u8 sp[0x160];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r5 = 0xd6;
    r6 = 0x0;
    r31 = r4;
    r30 = r3;
    r4 = 0x0;
    ((void(*)(void))fn_8012640C)();
    if (r3 == (u32)0x0) {
        r3 = 0x0;
    } else {

        r4 = 0x0;
        r5 = 0xcc;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
    }
    r4 = r3;
    r3 = (u32)sp + 0x10;
    fn_8011F5FC();
    r3 = 0x10;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x10;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xc8) {
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xcd) {
        }
        }
        r3 = r30;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r29 = r3;
        r3 = 0x10;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x10;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xc8) {
            }
            if (r29 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r3 = r29;
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r4 = 0x10;
            fn_80121ADC();
            goto L_801FE2BC;
            }
        r3 = 0x10;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xcd) {
            r3 = 0x0;
            goto L_801FE2BC;
        }
        r3 = r29;
        r4 = 0x10;
        fn_8011B67C();

            } else {
        r3 = 0x10;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8) {
            r3 = 0x0;

        } else {
            r3 = r30;
            r4 = 0x10;
            fn_8011B67C();
        }
            }
    L_801FE2BC: ;
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = (u32)sp + 0xc;
        r5 = (u32)sp + 0x8;
        fn_801FDB78();
        r3 = (u32)sp + 0x10;
        r4 = 0x0;
        r5 = 0x6f;
        r6 = 0x0;
        ((void(*)(void))fn_801254B4)();
        r3 = (u32)sp + 0x10;
        r4 = 0x0;
        r5 = 0x75;
        r6 = 0x0;
        ((void(*)(void))fn_801254B4)();
    }
    r4 = r31;
    r3 = (u32)sp + 0x10;
    fn_8011FDC8();
    r3 = r30;
    r4 = 0x0;
    r5 = 0xd5;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    if (r3 == (u32)0x0) {
        r29 = 0x0;
    } else {

        r4 = 0x0;
        r5 = 0xcc;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r29 = r3;
    }
    if (r29 != (u32)0x0) {
        r3 = r29;
        r4 = r31;
        fn_801202CC();
        r3 = r29;
        fn_8001D994();
        *(u8*)((u8*)r31 + 0x28) = r3;
    }
    r0 = 0x1;
    r4 = r30;
    *(u8*)((u8*)r31 + 0x16) = r0;
    r3 = 0x0;
    fn_801F4354();
    if (r3 == (u32)0x0) {
        r0 = 0x0;

    } else {
        fn_801FB8F8();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r0 = 0x1;

        } else {
            r0 = 0x0;
        }
    }
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;
        *(u8*)((u8*)r31 + 0x16) = r0;
    }
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x32;
    r6 = 0x0;
    fn_801F54A4();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = 0x1;
        *(u8*)((u8*)r31 + 0x16) = r0;
    }
    return;
}

/* 0x801FE3F8 | size: 0x70 */
void fn_801FE3F8(void* context, u8* dest) {
    extern void fn_801204A8(void* pokemon, u8* dest);
    void* partyList;
    void* pokemon;

    partyList = fn_8012640C(context, 0, 0xD6, 0);
    if (partyList == NULL) {
        pokemon = NULL;
    } else {
        pokemon = fn_8012640C(partyList, 0, 0xCC, 0);
    }
    if (pokemon != NULL) {
        fn_801204A8(pokemon, dest);
    }
    *(u32*)(dest + 0x40) = (u32)context;
}

/* 0x801FE468 | size: 0xF4 */
void fn_801FE468(void* context, u8* dest) {
    void* pokemon;

    if (context != NULL && dest != NULL) {
        if (context == NULL) {
            pokemon = NULL;
        } else {
            pokemon = fn_8012640C(context, 0, 0xCC, 0);
        }
        *(u16*)(dest + 0x2) = (u16)(u32)fn_8012640C(pokemon, 0, 0x87, 0);
        *(u16*)(dest + 0x4) = (u16)(u32)fn_8012640C(pokemon, 0, 0x88, 0);
        *(u16*)(dest + 0x6) = (u16)(u32)fn_8012640C(pokemon, 0, 0x89, 0);
        *(u16*)(dest + 0xA) = (u16)(u32)fn_8012640C(pokemon, 0, 0x8A, 0);
        *(u16*)(dest + 0xC) = (u16)(u32)fn_8012640C(pokemon, 0, 0x8B, 0);
        *(u16*)(dest + 0x8) = (u16)(u32)fn_8012640C(pokemon, 0, 0x8C, 0);
        *(u8*)(dest + 0x0) = 0;
    }
}

/* 0x801FE55C | size: 0x78 */
void fn_801FE55C(void* self, void* other, u32 offset) {
    extern void* fn_8020E57C(void* data, u32 mode, void* key);
    extern u32 fn_801FD0BC(void* ptr);
    extern void fn_801FD07C(void* ptr, u32 val);
    void* data;
    void* result;
    u32 val;

    if (self != NULL) {
        if (other != NULL) {
            if (self != other) {
                data = fn_8012640C(other, 0, 0x122, 0);
                if ((result = fn_8020E57C(data, 4, self)) != NULL) {
                    val = fn_801FD0BC(result);
                    val += offset;
                    fn_801FD07C(result, val);
                }
            }
        }
    }
}

/* 0x801FE5D4 | size: 0x13C */
typedef struct { u16 fields[18]; } FieldTable18;
void fn_801FE5D4(void* context) {
    extern FieldTable18 lbl_80279CE4;
    FieldTable18 table;
    void* srcSlot;
    void* destSlot;
    void* srcPokemon;
    void* destPokemon;
    u16 fieldId;
    u8 i;

    table = lbl_80279CE4;
    for (i = 0; i < 18; i++) {
        fieldId = table.fields[i];
        srcSlot = fn_8012640C(context, 0, 0xD5, 0);
        destSlot = fn_8012640C(context, 0, 0xD7, 0);
        if (srcSlot != NULL && destSlot != NULL) {
            if (srcSlot == NULL) {
                srcPokemon = NULL;
            } else {
                srcPokemon = fn_8012640C(srcSlot, 0, 0xCC, 0);
            }
            if (destSlot == NULL) {
                destPokemon = NULL;
            } else {
                destPokemon = fn_8012640C(destSlot, 0, 0xCC, 0);
            }
            fn_801254B4(destPokemon, 0, fieldId, 0, (u32)fn_8012640C(srcPokemon, 0, fieldId, 0));
        }
    }
}

/* 0x801FE91C | size: 0x2F4 | large */
void fn_801FE91C(void) {
    extern u8 lbl_80279CB8[];
    extern u8 lbl_80279CD4[];
    extern void fn_8011BEB4();
    u8 sp[0x60];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r5 = (u32)lbl_80279CB8;
    r0 = 0x3;
    r5 = (u32)lbl_80279CB8;
    r7 = (u32)sp + 0x14;
    r31 = r4;
    r30 = r3;
    /* subi r4, r5, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        r0 = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r7 + 0x4) = r3;
        r7 += 8; *(u32*)r7 = r0;
    } while (--ctr != 0);
    r0 = *(u32*)((u8*)r4 + 0x4);
    r3 = (u32)lbl_80279CD4;
    r6 = (u32)lbl_80279CD4;
    *(u32*)((u8*)r7 + 0x4) = r0;
    r5 = *(u32*)((u8*)r6 + 0x0);
    r4 = *(u32*)((u8*)r6 + 0x4);
    r3 = *(u32*)((u8*)r6 + 0x8);
    r0 = *(u16*)((u8*)r6 + 0xC);
    *(u16*)(sp + 0x14) = r0;
    if (r30 == (u32)0x0) return;
    if (r31 == (u32)0x0) return;
    if (r30 == (u32)0x0) {
        r28 = 0x0;
    } else {

        r3 = r30;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        if (r3 == (u32)0x0) {
            r3 = 0x0;
        } else {

            r4 = 0x0;
            r5 = 0xcc;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
        }
        r28 = r3;
    }
    if (r31 == (u32)0x0) {
        r27 = 0x0;
    } else {

        r3 = r31;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        if (r3 == (u32)0x0) {
            r3 = 0x0;
        } else {

            r4 = 0x0;
            r5 = 0xcc;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
        }
        r27 = r3;
    }
    r29 = (u32)sp + 0x18;
    r25 = 0x0;
    while (1) {
        r0 = r25 & 0xFF;
        if (r0 >= (u32)0xe) break;
        /* clrlslwi r0, r25, 24, 1 */;
        r3 = r28;
        r26 = *(u16*)(r29 + r0);
        r4 = 0x0;
        r6 = 0x0;
        r5 = r26;
        ((void(*)(void))fn_8012640C)();
        r0 = r3;
        r3 = r27;
        r7 = r0;
        r5 = r26;
        r4 = 0x0;
        r6 = 0x0;
        ((void(*)(void))fn_801254B4)();
        r25 = r25 + 0x1;

    }
    r29 = 0x0;
    while (1) {
        r0 = r29 & 0xFF;
        if (r0 >= (u32)0x4) break;
        r26 = r29 & 0xFF;
        r3 = r28;
        r6 = r26;
        r4 = 0x0;
        r5 = 0x7f;
        ((void(*)(void))fn_8012640C)();
        r25 = r3;
        r3 = r27;
        r6 = r26;
        r4 = 0x0;
        r7 = r25;
        r5 = 0x7f;
        ((void(*)(void))fn_801254B4)();
        r4 = r25 & 0xFFFF;
        r3 = 0x0;
        r5 = 0x2;
        r6 = 0x0;
        fn_8011BEB4();
        r0 = r3 & 0xFF;
        if (r0 < (u32)0x5) {
            r3 = r28;
            r6 = r26;
            r4 = 0x0;
            r5 = 0x80;
            ((void(*)(void))fn_8012640C)();
            r7 = r3;
            r3 = r27;
            r6 = r26;
            r4 = 0x0;
            r5 = 0x80;
            ((void(*)(void))fn_801254B4)();
        } else {

            r3 = r27;
            r6 = r26;
            r4 = 0x0;
            r5 = 0x80;
            r7 = 0x5;
            ((void(*)(void))fn_801254B4)();
        }
        r29 = r29 + 0x1;

    }
    r29 = (u32)sp + 0x8;
    r27 = 0x0;
    while (1) {
        r0 = r27 & 0xFF;
        if (r0 >= (u32)0x7) break;
        /* clrlslwi r0, r27, 24, 1 */;
        r3 = r30;
        r26 = *(u16*)(r29 + r0);
        r4 = 0x0;
        r6 = 0x0;
        r5 = r26;
        ((void(*)(void))fn_8012640C)();
        r7 = r3;
        r3 = r31;
        r5 = r26;
        r4 = 0x0;
        r6 = 0x0;
        ((void(*)(void))fn_801254B4)();
        r27 = r27 + 0x1;

    }
    r3 = r30;
    r4 = 0x0;
    r5 = 0x100;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    r7 = r3 & 0xFFFF;
    r3 = r31;
    r4 = 0x0;
    r5 = 0x100;
    r6 = 0x0;
    ((void(*)(void))fn_801254B4)();
    r27 = 0x0;
    while (1) {
        r0 = r27 & 0xFF;
        if (r0 >= (u32)0x2) break;
        r26 = r27 & 0xFF;
        r3 = r30;
        r6 = r26;
        r4 = 0x0;
        r5 = 0xff;
        ((void(*)(void))fn_8012640C)();
        r7 = r3 & 0xFFFF;
        r3 = r31;
        r6 = r26;
        r4 = 0x0;
        r5 = 0xff;
        ((void(*)(void))fn_801254B4)();
        r27 = r27 + 0x1;

    }

    return;
}

/* 0x801FEC10 | size: 0xC4 */
typedef struct { u8 data[0x154]; } CopyBlock_0x154;
u32 fn_801FEC10(void* context) {
    extern u32 fn_801254B4();
    CopyBlock_0x154* src;
    CopyBlock_0x154* dest;
    register CopyBlock_0x154* dest2;

    if (context == NULL) {
        return 0;
    }
    src = (CopyBlock_0x154*)fn_8012640C(context, 0, 0xD5, 0);
    dest = (CopyBlock_0x154*)fn_8012640C(context, 0, 0xD7, 0);
    dest2 = dest;
    if (src == NULL) {
        return 0;
    }
    if (dest2 == NULL) {
        return 0;
    }
    *dest2 = *src;
    fn_801254B4(context, 0, 0xD6, 0, (u32)dest2);
    return 1;
}

/* 0x801FED3C | size: 0x238 | large */
void fn_801FED3C(void) {
    extern void fn_80119ED0();
    extern void fn_8011A280();
    extern void fn_8011B67C();
    extern void fn_801214FC();
    extern void fn_80121ADC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r30, r3 */;
    r31 = r4;
    if ((s32)r0 == (s32)0) return;
    r3 = 0x14;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x14;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xc8) {
            r3 = 0x14;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xcd) {
        }
        }
        r3 = r30;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r29 = r3;
        r3 = 0x14;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x14;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xc8) {
            }
            if (r29 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r3 = r29;
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r4 = 0x14;
            fn_80121ADC();
            goto L_801FEE58;
            }
        r3 = 0x14;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xcd) {
            r3 = 0x0;
            goto L_801FEE58;
        }
        r3 = r29;
        r4 = 0x14;
        fn_8011B67C();

            } else {
        r3 = 0x14;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8) {
            r3 = 0x0;

        } else {
            r3 = r30;
            r4 = 0x14;
            fn_8011B67C();
        }
            }
    L_801FEE58: ;
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) return;
    r3 = 0x14;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x14;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xc8) {
            r3 = 0x14;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xcd) {
        }
        }
        r3 = r30;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r29 = r3;
        r3 = 0x14;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x14;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xc8) {
            }
            if (r29 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r3 = r29;
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r5 = r31;
            r4 = 0x14;
            fn_801214FC();
            return;
            }
        r3 = 0x14;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xcd) return;
        r3 = r29;
        r5 = r31;
        r4 = 0x14;
        fn_8011A280();
        return;
            }
    r3 = 0x14;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0xd8) return;
    r3 = r30;
    r5 = r31;
    r4 = 0x14;
    fn_8011A280();

    return;
}

/* 0x801FEF74 | size: 0x248 | large */
/* 0x801FEF74 | size: 0x248 | large | 99.25%: real correct C, RELOC/scheduling-tie wall (see WALLS/equivalent.txt) */
#pragma push
#pragma scheduling off
u8 fn_801FEF74(void* trainer) {
    extern u16 fn_80119ED0(s32 id);
    extern u8 fn_8011B67C(void* obj, s32 arg);
    extern u8 fn_80121ADC(void* obj, u32 field);
    extern void fn_80121574(void* obj, s32 arg);
    extern void fn_8011A3E4(void* obj, s32 arg);
    void* obj;
    void* target;
    void* obj2;
    void* target2;
    u8 status;

    if (trainer == NULL) {
        return 0;
    }
    if (fn_80119ED0(0x14) == 0x7C || fn_80119ED0(0x14) == 0xC8 || fn_80119ED0(0x14) == 0xCD) {
        obj = fn_8012640C(trainer, 0, 0xD6, 0);
        if (fn_80119ED0(0x14) == 0x7C || fn_80119ED0(0x14) == 0xC8) {
            if (obj == NULL) {
                target = NULL;
            } else {
                target = fn_8012640C(obj, 0, 0xCC, 0);
            }
            status = fn_80121ADC(target, 0x14);
        } else if (fn_80119ED0(0x14) != 0xCD) {
            status = 0;
        } else {
            status = fn_8011B67C(obj, 0x14);
        }
    } else if (fn_80119ED0(0x14) != 0xD8) {
        status = 0;
    } else {
        status = fn_8011B67C(trainer, 0x14);
    }
    if (status == 1) {
        if (fn_80119ED0(0x14) == 0x7C || fn_80119ED0(0x14) == 0xC8 || fn_80119ED0(0x14) == 0xCD) {
            obj2 = fn_8012640C(trainer, 0, 0xD6, 0);
            if (fn_80119ED0(0x14) == 0x7C || fn_80119ED0(0x14) == 0xC8) {
                if (obj2 == NULL) {
                    target2 = NULL;
                } else {
                    target2 = fn_8012640C(obj2, 0, 0xCC, 0);
                }
                fn_80121574(target2, 0x14);
            } else if (fn_80119ED0(0x14) != 0xCD) {
                return 0;
            } else {
                fn_8011A3E4(obj2, 0x14);
            }
        } else if (fn_80119ED0(0x14) != 0xD8) {
            return 0;
        } else {
            fn_8011A3E4(trainer, 0x14);
        }
    }
    return 0;
}
#pragma pop

/* 0x801FF1BC | size: 0x974 | massive */
void fn_801FF1BC(void) {
    extern u8 lbl_80375CA8[];
    extern void fn_80119ED0();
    extern void fn_8011A3E4();
    extern void fn_8011A6D4();
    extern void fn_8011B67C();
    extern void fn_8011B788();
    extern void fn_80121574();
    extern void fn_8012165C();
    extern void fn_80121ADC();
    extern void fn_80121B4C();
    extern void fn_801233F4();
    extern void fn_80123FBC();
    extern void fn_801EF634();
    extern void fn_801F0134();
    extern void fn_801F11CC();
    extern void fn_801F54A4();
    extern void fn_801FFB30();
    extern void fn_802099AC();
    extern void fn_8020D878();
    extern void fn_8022B2CC();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r5 = 0x14;
    r6 = 0x0;
    r31 = r3;
    r28 = r4;
    r3 = 0x0;
    r4 = 0x0;
    fn_801F54A4();
    r30 = r3 & 0xFFFF;
    if (r31 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    if (r31 != (u32)0x0) {

    if (r31 != (u32)0x0) {

    fn_801EF634();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x1) {

    r3 = r31;
    r4 = 0x0;
    r5 = 0xd6;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    /* mr. r26, r3 */;
    if (r0 != (u32)0x1) {

    if (r0 != (u32)0x1) {

    fn_801EF634();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x1) {

    r3 = r26;
    r4 = 0x0;
    r5 = 0xcb;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    if (r3 != (u32)0x0) {

    fn_80123FBC();
    r0 = r3 & 0xFF;
    if (r3 != (u32)0x0) {

    if (r26 == (u32)0x0) {
    r3 = 0x0;
    } else {

    r3 = r26;
    r4 = 0x0;
    r5 = 0xcc;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    }
    if (r3 != (u32)0x0) {

    fn_80123FBC();
    r0 = r3 & 0xFF;
    if (r3 != (u32)0x0) {

    r3 = r26;
    r4 = 0x0;
    r5 = 0xce;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    if ((s32)r3 >= (s32)0x0) {

    r0 = 0x1;
    }
    }
    }
    }
    }
    }
    }
    r0 = r0 & 0xFF;
    if ((s32)r3 != (s32)0x0) {

    r0 = 0x1;
    }
    }
    }
    }
    r0 = r0 & 0xFF;
    if ((s32)r3 != (s32)0x0) {

        r3 = r31;
        r4 = 0x0;
        r5 = 0x120;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
    if ((s32)r3 != (s32)0x1) {

            r3 = r31;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            /* mr. r26, r3 */;
            if ((s32)r3 != (s32)0x1) {

            if ((s32)r3 != (s32)0x1) {

            fn_801EF634();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x1) {

            r3 = r26;
            r4 = 0x0;
            r5 = 0xcb;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            if (r3 != (u32)0x0) {

            fn_80123FBC();
            r0 = r3 & 0xFF;
            if (r3 != (u32)0x0) {

            if (r26 == (u32)0x0) {
            r3 = 0x0;
            } else {

            r3 = r26;
            r4 = 0x0;
            r5 = 0xcc;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            }
            if (r3 != (u32)0x0) {

            fn_80123FBC();
            r0 = r3 & 0xFF;
            if (r3 != (u32)0x0) {

            r3 = r26;
            r4 = 0x0;
            r5 = 0xce;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            if ((s32)r3 >= (s32)0x0) {

            r0 = 0x1;
            }
    }
    }
    }
    }
    }
    }
            r0 = r0 & 0xFF;
            if ((s32)r3 != (s32)0x0) {

            r3 = r26;
            r4 = 0x0;
            r5 = 0xd2;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            if ((s32)r3 != (s32)0x1) {

            if (r26 == (u32)0x0) {
            r3 = 0x0;
            } else {

            r3 = r26;
            r4 = 0x0;
            r5 = 0xcc;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            }
            if (r3 != (u32)0x0) {

            fn_801233F4();
            r0 = r3 & 0xFF;
            if (r3 != (u32)0x0) {

            r0 = 0x1;
            }
    }
    }
    }
    }
            r0 = r0 & 0xFF;
    if (r3 != (u32)0x0) {

                r0 = 0x1;
    }
    }
    }
    }
    r0 = r0 & 0xFF;
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    if (r31 == (u32)0x0) {
        r29 = 0x0;
    } else {

        r3 = r31;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        if (r3 == (u32)0x0) {
            r3 = 0x0;
        } else {

            r4 = 0x0;
            r5 = 0xcc;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
        }
        r29 = r3;
    }
    r3 = r31;
    fn_801FFB30();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r28 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r31;
            r5 = r30;
            r4 = 0xa5;
            r6 = 0x0;
            r7 = 0x1;
            r8 = 0x1;
            r9 = -0x1;
            fn_8022B2CC();
            r4 = r30;
            fn_801F0134();
            r0 = r3;
            r3 = r31;
            r26 = r0;
            r4 = 0x0;
            r5 = 0xd9;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            if (r3 != (u32)0x0) {
                r6 = r26;
                r4 = -0x1;
                r5 = 0xa5;
                r7 = 0x1;
                fn_802099AC();
                r3 = r31;
                r4 = 0x0;
                r5 = 0xfe;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
                /* mr. r26, r3 */;
                if (r3 != (u32)0x0) {
                    r4 = (u32)lbl_80375CA8;
                    r5 = r31;
                    r8 = (u32)lbl_80375CA8;
                    r6 = 0x13;
                    r4 = 0x0;
                    r7 = 0x0;
                    fn_801F11CC();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x1) {
                        r3 = r26;
                        r4 = 0xa5;
                        fn_8020D878();
        }
        }
        }
        }
        r3 = 0x1;
        return;
    }
    r3 = 0x2a;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xc8) {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xcd) {
        }
        }
        r3 = r31;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r26 = r3;
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xc8) {
            }
            if (r26 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r3 = r26;
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r4 = 0x2a;
            fn_80121ADC();
            goto L_801FF714;
            }
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xcd) {
            r3 = 0x0;
            goto L_801FF714;
        }
        r3 = r26;
        r4 = 0x2a;
        fn_8011B67C();

            } else {
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8) {
            r3 = 0x0;

        } else {
            r3 = r31;
            r4 = 0x2a;
            fn_8011B67C();
        }
            }
    L_801FF714: ;
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) { r3 = 0x0; return; }
    r3 = 0x2a;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xc8) {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xcd) {
        }
        }
        r3 = r31;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r26 = r3;
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xc8) {
            }
            if (r26 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r3 = r26;
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r4 = 0x2a;
            fn_8012165C();

            } else {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) {
                r3 = 0x0;

            } else {
                r3 = r26;
                r4 = 0x2a;
                fn_8011A6D4();
            }
            }
        r27 = r3;

            } else {
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8) {
            r27 = 0x0;

        } else {
            r3 = r31;
            r4 = 0x2a;
            fn_8011A6D4();
            r27 = r3;
        }
            }
    r3 = 0x2a;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xc8) {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xcd) {
        }
        }
        r3 = r31;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r26 = r3;
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xc8) {
            }
            if (r26 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r3 = r26;
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r4 = 0x2a;
            fn_80121574();

            } else {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) {
                r3 = 0x0;

            } else {
                r3 = r26;
                r4 = 0x2a;
                fn_8011A3E4();
            }
            }
        r26 = r3;

            } else {
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8) {
            r26 = 0x0;

        } else {
            r3 = r31;
            r4 = 0x2a;
            fn_8011A3E4();
            r26 = r3;
        }
            }
    r3 = r29;
    r6 = r26 & 0xFFFF;
    r4 = 0x0;
    r5 = 0x7f;
    ((void(*)(void))fn_8012640C)();
    r0 = r28 & 0xFF;
    r28 = r3 & 0xFFFF;
    if (r0 == (u32)0xd8) { r3 = 0x2; return; }
    r0 = r27 & 0xFFFF;
    do {
    if (r0 == (u32)r28) break;
    r3 = r31;
    r4 = 0x0;
    r5 = 0xee;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    r3 = 0x2a;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xc8) {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xcd) {
        }
        }
        r3 = r31;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r27 = r3;
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xc8) {
            }
            if (r27 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r3 = r27;
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r4 = 0x2a;
            fn_80121B4C();
            break;
            }
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xcd) break;
        r3 = r27;
        r4 = 0x2a;
        fn_8011B788();
        break;
            }
    r3 = 0x2a;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0xd8) break;
    r3 = r31;
    r4 = 0x2a;
    fn_8011B788();
    } while (0);
    r3 = r31;
    r4 = r28;
    r5 = r30;
    r27 = (s8)r26;
    r6 = 0x0;
    r7 = 0x1;
    r8 = 0x1;
    r9 = -0x1;
    fn_8022B2CC();
    r4 = r30;
    fn_801F0134();
    r26 = r3;
    r3 = r31;
    r4 = 0x0;
    r5 = 0xd9;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    if (r3 == (u32)0x0) { r3 = 0x2; return; }
    r4 = r27;
    r5 = r28;
    r6 = r26;
    r7 = 0x1;
    fn_802099AC();
    r3 = r31;
    r4 = 0x0;
    r5 = 0xfe;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    /* mr. r26, r3 */;
    if (r3 == (u32)0x0) { r3 = 0x2; return; }
    r4 = (u32)lbl_80375CA8;
    r5 = r31;
    r8 = (u32)lbl_80375CA8;
    r6 = 0x13;
    r4 = 0x0;
    r7 = 0x0;
    fn_801F11CC();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) { r3 = 0x2; return; }
    r3 = r26;
    r4 = r28;
    fn_8020D878();

    r3 = 0x2;
    return;

    r3 = 0x0;

    return;
}

/* 0x801FFB30 | size: 0x398 | large */
void fn_801FFB30(void) {
    extern void fn_80119ED0();
    extern void fn_8011A3E4();
    extern void fn_8011B67C();
    extern void fn_80121574();
    extern void fn_80121ADC();
    extern void fn_80123CD4();
    extern void fn_801FFEC8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r31, r3 */;
    if ((s32)r0 == (s32)0) {
        r30 = 0x0;

    } else {
        r3 = 0x0;
        while (1) {
            r0 = r3 & 0xFF;
            if (r0 >= (u32)0x4) break;
            r3 = r3 + 0x1;

        }
        r30 = 0x0;
        r28 = r30;
        while (1) {
            r0 = r28 & 0xFF;
            if (r0 >= (u32)0x4) break;
            if (r31 != (u32)0x0) {

            r3 = r31;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            if (r3 != (u32)0x0) {

                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
        }
            r29 = r28 & 0xFF;
            r4 = r29;
            fn_80123CD4();
            r0 = r3 & 0xFF;
            do {
            if (r3 == (u32)0x0) break;
            if (r31 != (u32)0x0) {

            r3 = r31;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            if (r3 != (u32)0x0) {

                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
        }
            r6 = r29;
            r4 = 0x0;
            r5 = 0x7f;
            ((void(*)(void))fn_8012640C)();
            r0 = r3 & 0xFFFF;
            if (r3 == (u32)0x0 || r0 == (u32)0x165 || r0 == (u32)0x163) break;

            r3 = r31;
            r4 = r29;
            r5 = 0x0;
            r6 = 0x0;
            fn_801FFEC8();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x163) break;
            r30 = r30 + 0x1;
            } while (0);
            r28 = r28 + 0x1;

        }
    }
    r0 = r30 & 0xFF;
    if (r0 == (u32)0x4) {
        r3 = 0x1;
        return;
    }
    r3 = 0x2a;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xc8) {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xcd) {
        }
        }
        r3 = r31;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r30 = r3;
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xc8) {
            }
            if (r30 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r3 = r30;
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r4 = 0x2a;
            fn_80121ADC();
            goto L_801FFD78;
            }
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xcd) {
            r3 = 0x0;
            goto L_801FFD78;
        }
        r3 = r30;
        r4 = 0x2a;
        fn_8011B67C();

            } else {
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8) {
            r3 = 0x0;

        } else {
            r3 = r31;
            r4 = 0x2a;
            fn_8011B67C();
        }
            }
    L_801FFD78: ;
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) { r3 = 0x0; return; }
    r3 = 0x2a;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xc8) {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xcd) {
        }
        }
        r3 = r31;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r30 = r3;
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xc8) {
            }
            if (r30 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r3 = r30;
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r4 = 0x2a;
            fn_80121574();

            } else {
            r3 = 0x2a;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) {
                r3 = 0x0;

            } else {
                r3 = r30;
                r4 = 0x2a;
                fn_8011A3E4();
            }
            }
        r0 = r3;

            } else {
        r3 = 0x2a;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8) {
            r0 = 0x0;

        } else {
            r3 = r31;
            r4 = 0x2a;
            fn_8011A3E4();
            r0 = r3;
        }
            }
    r3 = r31;
    r4 = r0 & 0xFFFF;
    r5 = 0x0;
    r6 = 0x0;
    fn_801FFEC8();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0xd8) { r3 = 0x0; return; }
    r3 = 0x1;
    return;

    r3 = 0x0;

    return;
}

/* 0x801FFEC8 | size: 0xB94 | massive */
void fn_801FFEC8(void) {
    extern u8 lbl_80279C90[];
    extern void fn_80119ED0();
    extern void fn_8011A6D4();
    extern void fn_8011B67C();
    extern void fn_8011B788();
    extern void fn_8011BEB4();
    extern void fn_8012165C();
    extern void fn_80121ADC();
    extern void fn_80121B4C();
    extern void fn_80123090();
    extern void fn_80123CD4();
    extern void fn_801DA36C();
    extern void fn_801F1F30();
    u8 sp[0x50];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    /* mr. r31, r3 */;
    r25 = r4;
    r24 = r5;
    r23 = r6;
    r30 = 0x0;
    if ((s32)r0 == (s32)0) {
        r3 = 0x6;
        return;
    }
    if ((s32)r0 == (s32)0) {
        r26 = 0x0;
    } else {

        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        if (r3 == (u32)0x0) {
            r3 = 0x0;
        } else {

            r4 = 0x0;
            r5 = 0xcc;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
        }
        r26 = r3;
    }
    r3 = r26;
    r6 = r25;
    r4 = 0x0;
    r5 = 0x7f;
    ((void(*)(void))fn_8012640C)();
    r29 = r3 & 0xFFFF;
    r3 = 0x0;
    r4 = r29;
    r5 = 0x7;
    r6 = 0x0;
    fn_8011BEB4();
    r22 = r3 & 0xFFFF;
    r3 = r26;
    r6 = r25;
    r4 = 0x0;
    r5 = 0x80;
    ((void(*)(void))fn_8012640C)();
    r21 = r3 & 0xFF;
    r3 = r31;
    r4 = 0x0;
    r5 = 0xd6;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    /* mr. r28, r3 */;
    if (r3 == (u32)0x0) {
        r27 = 0x0;
    } else {

        r4 = 0x0;
        r5 = 0xcc;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r27 = r3;
    }
    if (r27 != (u32)0x0) {

    r3 = 0x3d;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x3d;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 == (u32)0xc8) {
        }
        if (r28 == (u32)0x0) {
        r3 = 0x0;
        } else {

        r3 = r28;
        r4 = 0x0;
        r5 = 0xcc;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        }
        r4 = 0x3d;
        fn_80121ADC();

        } else {
        r3 = 0x3d;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xcd) {
            r3 = 0x0;

        } else {
            r3 = r28;
            r4 = 0x3d;
            fn_8011B67C();
        }
        }
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {

        r3 = r27;
        fn_80123090();
        r27 = r3;
    }
    }
    r3 = r31;
    r4 = 0x0;
    r5 = 0xf0;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    r28 = r3 & 0xFFFF;
    r3 = 0x29;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x29;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xc8) {
            r3 = 0x29;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xcd) {
        }
        }
        r3 = r31;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r20 = r3;
        r3 = 0x29;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x29;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xc8) {
            }
            if (r20 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r3 = r20;
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r4 = 0x29;
            fn_80121ADC();
            goto L_80200180;
            }
        r3 = 0x29;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xcd) {
            r3 = 0x0;
            goto L_80200180;
        }
        r3 = r20;
        r4 = 0x29;
        fn_8011B67C();

            } else {
        r3 = 0x29;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8) {
            r3 = 0x0;

        } else {
            r3 = r31;
            r4 = 0x29;
            fn_8011B67C();
        }
            }
    L_80200180: ;
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x29;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x29;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xc8) {
                r3 = 0x29;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xcd) {
        }
            }
            r3 = r31;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r20 = r3;
            r3 = 0x29;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x7c) {
                r3 = 0x29;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xc8) {
                }
                if (r20 == (u32)0x0) {
                    r3 = 0x0;
                } else {

                    r3 = r20;
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                }
                r4 = 0x29;
                fn_8012165C();
                goto L_8020028C;
                }
            r3 = 0x29;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) {
                r3 = 0x0;
                goto L_8020028C;
            }
            r3 = r20;
            r4 = 0x29;
            fn_8011A6D4();

                } else {
            r3 = 0x29;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xd8) {
                r3 = 0x0;

            } else {
                r3 = r31;
                r4 = 0x29;
                fn_8011A6D4();
            }
                }
        L_8020028C: ;
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8 && r0 == (u32)r29 && r0 != (u32)0x165) {

            r30 = 0x1;
    }
    }
    r3 = 0x1b;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x1b;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xc8) {
            r3 = 0x1b;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xcd) {
        }
        }
        r3 = r31;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r20 = r3;
        r3 = 0x1b;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x1b;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xc8) {
            }
            if (r20 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r3 = r20;
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r4 = 0x1b;
            fn_80121ADC();
            goto L_802003A8;
            }
        r3 = 0x1b;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xcd) {
            r3 = 0x0;
            goto L_802003A8;
        }
        r3 = r20;
        r4 = 0x1b;
        fn_8011B67C();

            } else {
        r3 = 0x1b;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8) {
            r3 = 0x0;

        } else {
            r3 = r31;
            r4 = 0x1b;
            fn_8011B67C();
        }
            }
    L_802003A8: ;
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1 || r29 != (u32)r28 || r29 != (u32)0xa5) {

        r0 = r24 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = (u32)lbl_80279C90;
            r0 = 0x2;
            r3 = (u32)lbl_80279C90;
            r5 = (u32)sp + 0x4;
            /* subi r4, r3, 0x4 */;
            ctr_fn = (void(*)(void))r0;
            do {
                r3 = *(u32*)((u8*)r4 + 0x4);
                r0 = *(u32*)((u8*)r4 + 0x8);
                *(u32*)((u8*)r5 + 0x4) = r3;
                r5 += 8; *(u32*)r5 = r0;
            } while (--ctr != 0);
            r0 = *(u16*)((u8*)r4 + 0x4);
            *(u16*)((u8*)r5 + 0x4) = r0;
            if (r31 != (u32)0x0) {
                r24 = (u32)sp + 0x8;
                r30 = 0x0;
                while (1) {
                    r0 = r30 & 0xFF;
                    if (r0 >= (u32)0x9) break;
                    /* clrlslwi r0, r30, 24, 1 */;
                    r28 = *(u16*)(r24 + r0);
                    r3 = r28;
                    fn_80119ED0();
                    r0 = r3 & 0xFFFF;
                    if (r0 != (u32)0x7c) {
                        r3 = r28;
                        fn_80119ED0();
                        r0 = r3 & 0xFFFF;
                        if (r0 != (u32)0xc8) {
                            r3 = r28;
                            fn_80119ED0();
                            r0 = r3 & 0xFFFF;
                            if (r0 == (u32)0xcd) {
                }
                        }
                        r3 = r31;
                        r4 = 0x0;
                        r5 = 0xd6;
                        r6 = 0x0;
                        ((void(*)(void))fn_8012640C)();
                        r20 = r3;
                        r3 = r28;
                        fn_80119ED0();
                        r0 = r3 & 0xFFFF;
                        if (r0 != (u32)0x7c) {
                            r3 = r28;
                            fn_80119ED0();
                            r0 = r3 & 0xFFFF;
                            if (r0 == (u32)0xc8) {
                            }
                            if (r20 == (u32)0x0) {
                                r3 = 0x0;
                            } else {

                                r3 = r20;
                                r4 = 0x0;
                                r5 = 0xcc;
                                r6 = 0x0;
                                ((void(*)(void))fn_8012640C)();
                            }
                            r4 = r28;
                            fn_80121ADC();
                            goto L_80200520;
                            }
                        r3 = r28;
                        fn_80119ED0();
                        r0 = r3 & 0xFFFF;
                        if (r0 != (u32)0xcd) {
                            r3 = 0x0;
                            goto L_80200520;
                        }
                        r3 = r20;
                        r4 = r28;
                        fn_8011B67C();

                            } else {
                        r3 = r28;
                        fn_80119ED0();
                        r0 = r3 & 0xFFFF;
                        if (r0 != (u32)0xd8) {
                            r3 = 0x0;

                        } else {
                            r3 = r31;
                            r4 = r28;
                            fn_8011B67C();
                        }
                            }
                    L_80200520: ;
                    r0 = r3 & 0xFF;
                    do {
                    if (r0 != (u32)0x1) break;
                    r3 = r31;
                    r4 = 0x0;
                    r5 = 0xee;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                    r20 = r3;
                    if (r28 == (u32)0x0) {
                        if (r20 != (u32)0x0) {
                            r4 = 0x1;
                            fn_801DA36C();
                            r3 = r20;
                            r4 = 0x2;
                            fn_801DA36C();
                        }

                    } else {
                        if (r20 != (u32)0x0) {
                            if (r28 == (u32)0x8) {
                                r4 = 0x1;
                                fn_801DA36C();
                            }
                            if (r28 == (u32)0x7) {
                                r3 = r20;
                                r4 = 0x2;
                                fn_801DA36C();
                }
                        }
                    }
                    r3 = r28;
                    fn_80119ED0();
                    r0 = r3 & 0xFFFF;
                    if (r0 != (u32)0x7c) {
                        r3 = r28;
                        fn_80119ED0();
                        r0 = r3 & 0xFFFF;
                        if (r0 != (u32)0xc8) {
                            r3 = r28;
                            fn_80119ED0();
                            r0 = r3 & 0xFFFF;
                            if (r0 == (u32)0xcd) {
                }
                        }
                        r3 = r31;
                        r4 = 0x0;
                        r5 = 0xd6;
                        r6 = 0x0;
                        ((void(*)(void))fn_8012640C)();
                        r20 = r3;
                        r3 = r28;
                        fn_80119ED0();
                        r0 = r3 & 0xFFFF;
                        if (r0 != (u32)0x7c) {
                            r3 = r28;
                            fn_80119ED0();
                            r0 = r3 & 0xFFFF;
                            if (r0 == (u32)0xc8) {
                            }
                            if (r20 == (u32)0x0) {
                                r3 = 0x0;
                            } else {

                                r3 = r20;
                                r4 = 0x0;
                                r5 = 0xcc;
                                r6 = 0x0;
                                ((void(*)(void))fn_8012640C)();
                            }
                            r4 = r28;
                            fn_80121B4C();
                            break;
                            }
                        r3 = r28;
                        fn_80119ED0();
                        r0 = r3 & 0xFFFF;
                        if (r0 != (u32)0xcd) break;
                        r3 = r20;
                        r4 = r28;
                        fn_8011B788();
                        break;
                            }
                    r3 = r28;
                    fn_80119ED0();
                    r0 = r3 & 0xFFFF;
                    if (r0 != (u32)0xd8) break;
                    r3 = r31;
                    r4 = r28;
                    fn_8011B788();
                    } while (0);
                    r30 = r30 + 0x1;

                }
        }
        }
        r30 = 0x2;
    }
    r3 = 0x30;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x7c) {
        r3 = 0x30;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xc8) {
            r3 = 0x30;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xcd) {
        }
        }
        r3 = r31;
        r4 = 0x0;
        r5 = 0xd6;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r20 = r3;
        r3 = 0x30;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x30;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0xc8) {
            }
            if (r20 == (u32)0x0) {
                r3 = 0x0;
            } else {

                r3 = r20;
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
            }
            r4 = 0x30;
            fn_80121ADC();
            goto L_8020079C;
            }
        r3 = 0x30;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xcd) {
            r3 = 0x0;
            goto L_8020079C;
        }
        r3 = r20;
        r4 = 0x30;
        fn_8011B67C();

            } else {
        r3 = 0x30;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8) {
            r3 = 0x0;

        } else {
            r3 = r31;
            r4 = 0x30;
            fn_8011B67C();
        }
            }
    L_8020079C: ;
    r0 = r3 & 0xFF;
    if ((r0 == (u32)0x1) && (r22 == (u32)0x0)) {

        r30 = 0x3;
    }
    r4 = r31;
    r5 = r29;
    r3 = 0x0;
    fn_801F1F30();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r30 = 0x4;
    }
    r0 = r27 & 0xFFFF;
    if (r0 == (u32)0x1d) {
        r3 = 0x36;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = 0x36;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xc8) {
                r3 = 0x36;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xcd) {
        }
            }
            r3 = r31;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r20 = r3;
            r3 = 0x36;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x7c) {
                r3 = 0x36;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xc8) {
                }
                if (r20 == (u32)0x0) {
                    r3 = 0x0;
                } else {

                    r3 = r20;
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                }
                r4 = 0x36;
                fn_80121ADC();
                goto L_802008E0;
                }
            r3 = 0x36;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) {
                r3 = 0x0;
                goto L_802008E0;
            }
            r3 = r20;
            r4 = 0x36;
            fn_8011B67C();

                } else {
            r3 = 0x36;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xd8) {
                r3 = 0x0;

            } else {
                r3 = r31;
                r4 = 0x36;
                fn_8011B67C();
            }
                }
        L_802008E0: ;
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x36;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x7c) {
                r3 = 0x36;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 != (u32)0xc8) {
                    r3 = 0x36;
                    fn_80119ED0();
                    r0 = r3 & 0xFFFF;
                    if (r0 == (u32)0xcd) {
            }
                }
                r3 = r31;
                r4 = 0x0;
                r5 = 0xd6;
                r6 = 0x0;
                ((void(*)(void))fn_8012640C)();
                r20 = r3;
                r3 = 0x36;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 != (u32)0x7c) {
                    r3 = 0x36;
                    fn_80119ED0();
                    r0 = r3 & 0xFFFF;
                    if (r0 == (u32)0xc8) {
                    }
                    if (r20 == (u32)0x0) {
                        r3 = 0x0;
                    } else {

                        r3 = r20;
                        r4 = 0x0;
                        r5 = 0xcc;
                        r6 = 0x0;
                        ((void(*)(void))fn_8012640C)();
                    }
                    r4 = 0x36;
                    fn_8012165C();
                    goto L_802009F4;
                    }
                r3 = 0x36;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 != (u32)0xcd) {
                    r3 = 0x0;
                    goto L_802009F4;
                }
                r3 = r20;
                r4 = 0x36;
                fn_8011A6D4();
                goto L_802009F4;
                    }
            r3 = 0x36;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xd8) {
                r3 = 0x0;
                goto L_802009F4;
            }
            r3 = r31;
            r4 = 0x36;
            fn_8011A6D4();

        } else {
            r3 = 0x0;
        }
        L_802009F4: ;
        r0 = r3 & 0xFFFF;
        if ((r0 != (u32)0xd8) && (r0 != (u32)0x165) && (r0 != (u32)0xffff) && (r0 != (u32)r29)) {

            r30 = 0x5;
        }
        if (r23 != (u32)0x0) {
            *(u16*)((u8*)r23 + 0x0) = r3;
    }
    }
    r3 = r26;
    r4 = r25;
    fn_80123CD4();
    r0 = r3 & 0xFF;
    if (r23 != (u32)0x0) {
        if (r21 != (u32)0x0) { r3 = r30; return; }
    }
    r30 = 0x6;

    r3 = r30;

    return;
}

/* 0x80200A5C | size: 0xB4 */
typedef struct { u16 fields[9]; } FieldTable9;
u32 fn_80200A5C(void* context) {
    extern FieldTable9 lbl_80279CA4;
    FieldTable9 table;
    u8 i;

    table = lbl_80279CA4;
    if (context == NULL) {
        return 0;
    }
    for (i = 0; i < 9; i++) {
        if ((s32)fn_8012640C(context, 0, table.fields[i], 0) == 1) {
            return 1;
        }
    }
    return 0;
}

/* 0x80200B10 | size: 0x2F0 | large */
void fn_80200B10(void) {
    extern u8 lbl_80279C90[];
    extern void fn_80119ED0();
    extern void fn_8011B67C();
    extern void fn_8011B788();
    extern void fn_80121ADC();
    extern void fn_80121B4C();
    extern void fn_801DA36C();
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r4 = (u32)lbl_80279C90;
    r4 = (u32)lbl_80279C90;
    r0 = 0x2;
    r5 = (u32)sp + 0x4;
    r30 = r3;
    /* subi r4, r4, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        r0 = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r5 + 0x4) = r3;
        r5 += 8; *(u32*)r5 = r0;
    } while (--ctr != 0);
    r0 = *(u16*)((u8*)r4 + 0x4);
    *(u16*)((u8*)r5 + 0x4) = r0;
    if (r30 == (u32)0x0) return;
    r29 = (u32)sp + 0x8;
    r31 = 0x0;
    while (1) {
        r0 = r31 & 0xFF;
        if (r0 >= (u32)0x9) break;
        /* clrlslwi r0, r31, 24, 1 */;
        r28 = *(u16*)(r29 + r0);
        r3 = r28;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xc8) {
                r3 = r28;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xcd) {
            }
            }
            r3 = r30;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r27 = r3;
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x7c) {
                r3 = r28;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xc8) {
                }
                if (r27 == (u32)0x0) {
                    r3 = 0x0;
                } else {

                    r3 = r27;
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                }
                r4 = r28;
                fn_80121ADC();
                goto L_80200C74;
                }
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) {
                r3 = 0x0;
                goto L_80200C74;
            }
            r3 = r27;
            r4 = r28;
            fn_8011B67C();

                } else {
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xd8) {
                r3 = 0x0;

            } else {
                r3 = r30;
                r4 = r28;
                fn_8011B67C();
            }
                }
        L_80200C74: ;
        r0 = r3 & 0xFF;
        do {
        if (r0 != (u32)0x1) break;
        r3 = r30;
        r4 = 0x0;
        r5 = 0xee;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r27 = r3;
        if (r28 == (u32)0x0) {
            if (r27 != (u32)0x0) {
                r4 = 0x1;
                fn_801DA36C();
                r3 = r27;
                r4 = 0x2;
                fn_801DA36C();
            }

        } else {
            if (r27 != (u32)0x0) {
                if (r28 == (u32)0x8) {
                    r4 = 0x1;
                    fn_801DA36C();
                }
                if (r28 == (u32)0x7) {
                    r3 = r27;
                    r4 = 0x2;
                    fn_801DA36C();
            }
            }
        }
        r3 = r28;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xc8) {
                r3 = r28;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xcd) {
            }
            }
            r3 = r30;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r27 = r3;
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x7c) {
                r3 = r28;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xc8) {
                }
                if (r27 == (u32)0x0) {
                    r3 = 0x0;
                } else {

                    r3 = r27;
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                }
                r4 = r28;
                fn_80121B4C();
                break;
                }
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) break;
            r3 = r27;
            r4 = r28;
            fn_8011B788();
            break;
                }
        r3 = r28;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8) break;
        r3 = r30;
        r4 = r28;
        fn_8011B788();
        } while (0);
        r31 = r31 + 0x1;

    }

    return;
}

/* 0x80200E00 | size: 0x2C8 | large */
void fn_80200E00(void) {
    extern u32 lbl_8047E518;
    extern u16 lbl_8047E51C;
    extern void fn_80119ED0();
    extern void fn_8011B67C();
    extern void fn_8011B788();
    extern void fn_80121ADC();
    extern void fn_80121B4C();
    extern void fn_801DA36C();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r30, r3 */;
    r3 = lbl_8047E518;
    r0 = lbl_8047E51C;
    *(u16*)(sp + 0xC) = r0;
    if ((s32)r0 == (s32)0) return;
    r29 = (u32)sp + 0x8;
    r31 = 0x0;
    while (1) {
        r0 = r31 & 0xFF;
        if (r0 >= (u32)0x3) break;
        /* clrlslwi r0, r31, 24, 1 */;
        r28 = *(u16*)(r29 + r0);
        r3 = r28;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xc8) {
                r3 = r28;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xcd) {
            }
            }
            r3 = r30;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r27 = r3;
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x7c) {
                r3 = r28;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xc8) {
                }
                if (r27 == (u32)0x0) {
                    r3 = 0x0;
                } else {

                    r3 = r27;
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                }
                r4 = r28;
                fn_80121ADC();
                goto L_80200F3C;
                }
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) {
                r3 = 0x0;
                goto L_80200F3C;
            }
            r3 = r27;
            r4 = r28;
            fn_8011B67C();

                } else {
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xd8) {
                r3 = 0x0;

            } else {
                r3 = r30;
                r4 = r28;
                fn_8011B67C();
            }
                }
        L_80200F3C: ;
        r0 = r3 & 0xFF;
        do {
        if (r0 != (u32)0x1) break;
        r3 = r30;
        r4 = 0x0;
        r5 = 0xee;
        r6 = 0x0;
        ((void(*)(void))fn_8012640C)();
        r27 = r3;
        if (r28 == (u32)0x0) {
            if (r27 != (u32)0x0) {
                r4 = 0x1;
                fn_801DA36C();
                r3 = r27;
                r4 = 0x2;
                fn_801DA36C();
            }

        } else {
            if (r27 != (u32)0x0) {
                if (r28 == (u32)0x8) {
                    r4 = 0x1;
                    fn_801DA36C();
                }
                if (r28 == (u32)0x7) {
                    r3 = r27;
                    r4 = 0x2;
                    fn_801DA36C();
            }
            }
        }
        r3 = r28;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xc8) {
                r3 = r28;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xcd) {
            }
            }
            r3 = r30;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r27 = r3;
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x7c) {
                r3 = r28;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xc8) {
                }
                if (r27 == (u32)0x0) {
                    r3 = 0x0;
                } else {

                    r3 = r27;
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                }
                r4 = r28;
                fn_80121B4C();
                break;
                }
            r3 = r28;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) break;
            r3 = r27;
            r4 = r28;
            fn_8011B788();
            break;
                }
        r3 = r28;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0xd8) break;
        r3 = r30;
        r4 = r28;
        fn_8011B788();
        } while (0);
        r31 = r31 + 0x1;

    }

    return;
}

/* 0x802010C8 | size: 0x180 | medium */
void fn_802010C8(void) {
    extern u32 lbl_8047E510;
    extern u16 lbl_8047E514;
    extern void fn_80119ED0();
    extern void fn_8011B67C();
    extern void fn_80121ADC();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r28, r3 */;
    r3 = lbl_8047E510;
    r0 = lbl_8047E514;
    *(u16*)(sp + 0xC) = r0;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        return;
    }
    r31 = (u32)sp + 0x8;
    r29 = 0x0;
    while (1) {
        r0 = r29 & 0xFF;
        if (r0 >= (u32)0x3) break;
        /* clrlslwi r0, r29, 24, 1 */;
        r30 = *(u16*)(r31 + r0);
        r3 = r30;
        fn_80119ED0();
        r0 = r3 & 0xFFFF;
        if (r0 != (u32)0x7c) {
            r3 = r30;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xc8) {
                r3 = r30;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xcd) {
            }
            }
            r3 = r28;
            r4 = 0x0;
            r5 = 0xd6;
            r6 = 0x0;
            ((void(*)(void))fn_8012640C)();
            r27 = r3;
            r3 = r30;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x7c) {
                r3 = r30;
                fn_80119ED0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0xc8) {
                }
                if (r27 == (u32)0x0) {
                    r3 = 0x0;
                } else {

                    r3 = r27;
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    ((void(*)(void))fn_8012640C)();
                }
                r4 = r30;
                fn_80121ADC();
                goto L_8020120C;
                }
            r3 = r30;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xcd) {
                r3 = 0x0;
                goto L_8020120C;
            }
            r3 = r27;
            r4 = r30;
            fn_8011B67C();

                } else {
            r3 = r30;
            fn_80119ED0();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xd8) {
                r3 = 0x0;

            } else {
                r3 = r28;
                r4 = r30;
                fn_8011B67C();
            }
                }
        L_8020120C: ;
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r30;
            return;
        }
        r29 = r29 + 0x1;

    }
    r3 = 0x0;

    return;
}

/* 0x80201248 | size: 0xF8 */
u8 fn_80201248(void* context, u16* output) {
    extern u8 fn_8020E614(void* entry);
    extern u16 fn_801FD0EC(void* entry);
    u8* entryList;
    u8* entry;
    u8 count;
    u8 i;
    u16 moveId;

    if (context == NULL) {
        return 0;
    }
    entryList = (u8*)fn_8012640C(context, 0, 0x122, 0);
    if (output != NULL) {
        for (i = 0; i < 4; i++) {
            output[i] = 0;
        }
    } else {
        for (i = 0; i < 4; i++) {
        }
    }
    count = 0;
    for (i = 0; i < 4; i++) {
        entry = entryList + i * 0xC;
        if ((u8)fn_8020E614((void*)entry) != 0) {
            moveId = fn_801FD0EC((void*)entry);
            if (moveId != 0) {
                if (moveId != 0x165) {
                    if (output != NULL) {
                        output[count] = moveId;
                    }
                    count++;
                }
            }
        }
    }
    return count;
}

/* 0x80201340 | size: 0xC0 */
void fn_80201340(void* context, void* target, u16 value) {
    extern void fn_801FD09C(void* entry, u16 val);
    extern void* fn_8020E57C(void* data, u32 mode, void* key);
    extern u8 fn_8020E614(void* entry);
    u8* data;
    u8* entry;
    u32 i;
    u32 offset;

    if (context == NULL) {
        return;
    }
    fn_801254B4(context, 0, 0xF7, 0, value);
    data = (u8*)fn_8012640C(context, 0, 0x122, 0);
    if (target != NULL) {
        if (fn_8020E57C((void*)data, 4, target) == NULL) {
            return;
        }
        fn_801FD09C(fn_8020E57C((void*)data, 4, target), value);
        return;
    }
    i = 0;
    offset = 0;
    do {
        entry = data + offset;
        if ((u8)fn_8020E614((void*)entry) != 0) {
            fn_801FD09C((void*)entry, value);
        }
        i++;
        offset += 0xC;
    } while (i < 4);
}

/* 0x80201400 | size: 0x7C */
s32 fn_80201400(void* arg0, u32 arg1) {
    extern s32 fn_80120D6C(void*, u32);
    void* result;

    result = fn_8012640C(arg0, 0, 0xD6, 0);
    if (result == NULL) {
        return -1;
    }
    if (result == NULL) {
        result = NULL;
    } else {
        result = fn_8012640C(result, 0, 0xCC, 0);
    }
    if (result == NULL) {
        return -1;
    }
    return fn_80120D6C(result, arg1);
}

/* 0x8020147C | size: 0xF0 */
u16 fn_8020147C(void* context, u16 moveId, u8 slot, u8 updateFlag) {
    extern u16 fn_80123110(void* pokemon, u16 moveId, u8 slot);
    void* partyList;
    void* pokemon;
    u16 result;

    partyList = fn_8012640C(context, 0, 0xD6, 0);
    if (partyList == NULL) {
        return 0;
    }
    if (partyList == NULL) {
        pokemon = NULL;
    } else {
        pokemon = fn_8012640C(partyList, 0, 0xCC, 0);
    }
    if (pokemon == NULL) {
        result = 0;
    } else {
        result = fn_80123110(pokemon, moveId, slot);
    }
    if (updateFlag == 1) {
        if (slot == 0) {
            if (result != 0) {
                fn_801254B4(context, 0, 0xFA, 0, result);
            }
        }
        if (slot == 1) {
            if (moveId != 0) {
                fn_801254B4(context, 0, 0xFA, 0, 0);
            }
        }
    }
    return result;
}

/* 0x8020156C | size: 0x94 */
typedef struct { u16 fields[7]; } FieldTable7;
u32 fn_8020156C(void* context) {
    extern FieldTable7 lbl_80279C80;
    FieldTable7 table;
    u8 i;
    u32 sum;

    table = lbl_80279C80;
    sum = 0;
    for (i = 0; i < 7; i++) {
        sum = (sum + (u32)fn_8012640C(context, 0, table.fields[i], 0)) & 0xFFFF;
    }
    return sum;
}

/* 0x80201600 | size: 0xA4 */
void fn_80201600(void* dest, void* src) {
    extern FieldTable7 lbl_80279C70;
    FieldTable7 table;
    u16 fieldId;
    u8 i;

    table = lbl_80279C70;
    for (i = 0; i < 7; i++) {
        fieldId = table.fields[i];
        fn_801254B4(dest, 0, fieldId, 0, (u32)fn_8012640C(src, 0, fieldId, 0));
    }
}

/* 0x802016A4 | size: 0x60 */
void* fn_802016A4(void* arg0) {
    extern void* fn_801231A4(void*);
    void* result;

    result = fn_8012640C(arg0, 0, 0xD6, 0);
    if (result == NULL) {
        result = NULL;
    } else {
        result = fn_8012640C(result, 0, 0xCC, 0);
    }
    if (result == NULL) {
        result = NULL;
    } else {
        result = fn_801231A4(result);
    }
    return result;
}

/* 0x80201704 | size: 0x60 */
void* fn_80201704(void* arg0) {
    extern void* fn_80121410(void*);
    void* result;

    result = fn_8012640C(arg0, 0, 0xD6, 0);
    if (result == NULL) {
        result = NULL;
    } else {
        result = fn_8012640C(result, 0, 0xCC, 0);
    }
    if (result == NULL) {
        result = NULL;
    } else {
        result = fn_80121410(result);
    }
    return result;
}

/* 0x80201764 | size: 0x12C */
void fn_80201764(void* context, void* item, void* target) {
    extern u16 fn_80119ED0(void* item);
    extern void fn_8011A280(void* partyList, void* item, void* target);
    extern void fn_801214FC(void* pokemon, void* item, void* target);
    void* partyList;
    void* pokemon;
    u16 itemType;

    itemType = (u16)fn_80119ED0(item);
    if (itemType != 0x7C) {
        if ((u16)fn_80119ED0(item) != 0xC8) {
            if ((u16)fn_80119ED0(item) != 0xCD) {
                if ((u16)fn_80119ED0(item) == 0xD8) {
                    fn_8011A280(context, item, target);
                }
                return;
            }
        }
    }
    partyList = fn_8012640C(context, 0, 0xD6, 0);
    if ((u16)fn_80119ED0(item) == 0x7C || (u16)fn_80119ED0(item) == 0xC8) {
        if (partyList == NULL) {
            pokemon = NULL;
        } else {
            pokemon = fn_8012640C(partyList, 0, 0xCC, 0);
        }
        fn_801214FC(pokemon, item, target);
    } else {
        if ((u16)fn_80119ED0(item) == 0xCD) {
            fn_8011A280(partyList, item, target);
        }
    }
}
