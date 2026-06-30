/**
 * @file pokemon.c
 * @brief Pokemon data structure accessors and party management.
 *
 * =========================================================================
 * SUBSYSTEM ANALYSIS
 * =========================================================================
 *
 * Address range: 0x801F000C - 0x801F7F80
 * Total functions: ~170
 * Total code size: ~32KB
 *
 * This file implements the runtime Pokemon data access layer. It is the
 * single most critical data interface in Colosseum -- virtually every
 * game subsystem calls through these functions to read/write Pokemon data.
 *
 * KEY FUNCTIONS (by call frequency):
 *
 *   fn_801F54A4 (PokemonGet)              - 510 calls, 0xD18 bytes
 *     Reads any field from a Pokemon structure. Dispatches through
 *     jumptable_803754AC (93 entries, one per field type). The field ID
 *     is passed as r5 and validated against 0x60 (max). If r5 == 0, it
 *     calls GetCurrentPokemon (fn_801F6B48). For r5 in 1-7, it resolves
 *     the slot via fn_801F6738. For r5 in 8-0x5E, it uses the jumptable.
 *
 *   fn_801F025C (PokemonSlotLookupDefault) - 438 calls, 0x50 bytes
 *     Convenience wrapper: calls PokemonGet(0,0,0x14,0) to get party
 *     count, then calls PokemonSlotLookup(slotType, index, count).
 *
 *   fn_801F4C14 (PokemonSet)              - 223 calls, 0x890 bytes
 *     Writes any field. Similar dispatch to PokemonGet but uses
 *     jumptable_80375330 (94 entries). Takes an extra r7 parameter
 *     as the value to write.
 *
 *   fn_801F02AC (PokemonSlotLookup)       - 89 calls, 0x46C bytes
 *     Resolves a "slot type" (party, battle, PC, etc.) to a concrete
 *     Pokemon pointer. Uses jumptable_803752F8 (13 entries) for slot
 *     types 0x11-0x1D. Each case calls PokemonGet with different field
 *     IDs (0x36, 0x42, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4B, 0x4C).
 *
 * GETTER/SETTER PAIRS (0x801F640C - 0x801F6B48):
 *   These are ~88 tiny (0x08-0x38 byte) functions that directly read/write
 *   fields in the Pokemon structure. They follow two patterns:
 *
 *   Pattern A (global state, 0x08 bytes):
 *     stw/lwz r3, lbl_XXXX@sda21(r0)
 *     blr
 *
 *   Pattern B (struct field, 0x14-0x1C bytes):
 *     cmplwi r3, 0x0
 *     beqlr / bne .next
 *     addis r3, r3, 0x1       ; High 16-bit offset
 *     stw/lwz r4, -0x5bXX(r3) ; Low 16-bit offset -> actual = 0xA4XX
 *     blr
 *
 *   Pattern C (array field, 0x28-0x38 bytes):
 *     cmplwi r3, 0x0
 *     beqlr
 *     cmplwi r4, 0x8           ; Array bounds check
 *     bgelr
 *     addis r3, r3, 0x1
 *     clrlslwi r0, r4, 16, 2  ; index * 4
 *     add r3, r3, r0
 *     lwz r3, -0x5b3c(r3)     ; Array base at 0xA4C4
 *     blr
 *
 * STRUCT FIELD MAP (offsets from base pointer):
 *   The addis+negative-offset pattern encodes fields at offsets 0xA480-0xA4F0:
 *
 *   0xA490 : u32  (fn_801F641C set, fn_801F6430 get) - possibly personality value
 *   0xA48C : u32  (fn_801F644C set, fn_801F6460 get) - possibly OT ID
 *   0xA488 : u32  (fn_801F647C set, fn_801F6490 get) - possibly experience
 *   0xA484 : u32  (fn_801F64AC set, fn_801F64C0 get) - possibly encryption key
 *   0xA4E4 : u16  (fn_801F64DC set, fn_801F64F0 get) - possibly species
 *   0xA4C4 : u32[8] (fn_801F650C set, fn_801F6544 get) - possibly move/PP data
 *   0xA4C0 : u16  (fn_801F65C0 set, fn_801F65D4 get) - possibly held item
 *
 * STAT CALCULATION HELPERS (0x801F6B54 - 0x801F7F80):
 *   Functions that compute derived stats (effective HP, stat stages,
 *   nature modifiers, level-up calculations). These call into the
 *   pokemon_data.c base stat tables and apply formulas.
 *
 * PARTY MANAGEMENT (0x801F0718 - 0x801F4C14):
 *   fn_801F0B00 (0x404 bytes): Large party initialization function
 *   fn_801F3CE8 (0x538 bytes): Party comparison/sorting (calls fn_802050F4)
 *   fn_801F3BB4 (0x134 bytes): Party slot swap
 *   fn_801F2B5C (0x3E0 bytes): Party data copy/clone
 *
 * BSS STATE (SDA21-relative globals):
 *   lbl_8047B5F0 : void*, Pokemon system state pointer (fn_801F640C/14)
 *
 * =========================================================================
 */

#include "game/pokemon.h"

/* =========================================================================
 * External function declarations
 * ========================================================================= */

/* fn_80008184: GSthread_GetCurrentContext or similar - called from fn_801F000C */
extern u32 fn_80008184(void);

/* _threadSwitch: VSync/frame wait - called in frame loop */
extern void _threadSwitch(void);

/* fn_800D3088: Returns frame delta or similar timing value */
extern u32 fn_800D3088(void);

/* fn_801F6738: Resolve party slot to Pokemon pointer by index */
extern struct Pokemon* fn_801F6738(u32 index);

/* fn_801F6B48: Get currently selected/active Pokemon */
extern struct Pokemon* fn_801F6B48(void);

/* fn_801F61BC: Get Pokemon system context pointer */
extern struct Pokemon* fn_801F61BC(void);

/* fn_802050F4: Pokemon nature/friendship comparison helper */
extern s8 fn_802050F4(struct Pokemon* pokemon);

/* Forward declarations for fn_801F54A4 asm wrapper */
extern u32 jumptable_803754AC[];
extern void fn_80119ED0(void);
extern void fn_8011B444(void);
extern void fn_8011B67C(void);
extern void fn_8012640C(void);
extern void fn_801EF634(void);
extern void fn_801F981C(void);
extern void fn_8020DD44(void);
extern void fn_8020DD80(void);
extern void fn_8020E0B0(void);
extern void fn_8020E0C8(void);
extern void fn_8020E0E0(void);
extern void fn_8020E0F8(void);
extern void fn_8020E124(void);
extern void fn_8020E1A4(void);
extern void fn_8020E1BC(void);
extern void fn_8020E1D4(void);
extern void fn_8020E1EC(void);
extern void fn_8020E204(void);

#pragma pack(2)
typedef struct CopyBuf {
    u32 a;
    u32 b;
    u32 c;
    u32 d;
    u32 e;
    u32 f;
    u16 g;
} CopyBuf;
#pragma pack()

extern const CopyBuf lbl_80279C28;
extern void fn_8020E230(void);
extern void fn_8020E248(void);
extern void fn_8020E260(void);
extern void fn_8020E278(void);
extern void fn_8020E290(void);
extern void fn_8020E2A8(void);
extern void fn_8020E2C0(void);
extern void fn_8020E2D8(void);
extern void fn_8020E2F0(void);
extern void fn_8020E308(void);
extern void fn_8020E320(void);
extern void fn_8020E338(void);
extern void fn_8020E350(void);
extern void fn_8020E368(void);
extern void fn_8020E380(void);
extern void fn_8020E398(void);
extern void fn_8020E3B0(void);
extern void fn_8020E3C8(void);
extern void fn_8020E3E0(void);
extern void fn_8020E3F8(void);
extern void fn_8020E410(void);
extern void fn_8020E428(void);
extern void fn_8020E440(void);
extern void fn_8020E458(void);
extern void fn_8020E470(void);
extern void fn_8020E488(void);
extern void fn_8020E4B4(void);

/* =========================================================================
 * fn_801F000C - FrameWaitForDuration
 *
 * Waits for a specified number of frames by calling the frame-wait
 * and frame-delta functions in a loop.
 *
 * @param duration  Number of frame units to wait
 * ========================================================================= */
void FrameWaitForDuration(u32 duration) {
    /* fn_80008184 returns a context/flag; if non-zero, enter wait loop */
    u32 target = fn_80008184();
    if (target != 0) {
        u32 elapsed = 0;
        while (elapsed < target) {
            _threadSwitch();
            elapsed += fn_800D3088();
        }
    }
}

/* =========================================================================
 * fn_801F025C - PokemonSlotLookupDefault
 *
 * Gets the total party count via PokemonGet, then calls PokemonSlotLookup
 * to resolve the given slot type and index.
 *
 * r3 = slot type (POKE_SLOT_PARTY, etc.)
 * r4 = slot index
 * ========================================================================= */
u32 PokemonSlotLookupDefault(u16 slotType, u32 index) {
    u16 count = (u16)PokemonGet(NULL, 0, POKE_FIELD_IV_SPATK, 0); /* field 0x14 = party count */
    return PokemonSlotLookup(slotType, index, count);
}

/* =========================================================================
 * fn_801F02AC - PokemonSlotLookup
 *
 * Large dispatch function (0x46C bytes) that resolves slot types to data.
 *
 * Phase 1: Validates slot type. If type == 1, returns immediately.
 * Phase 2: For types 0x11-0x1D, uses jumptable_803752F8 to dispatch.
 *          Each case calls PokemonGet with the appropriate field ID:
 *            0x11: field 0x36 (party member)
 *            0x12: field 0x42 (active battle Pokemon)
 *            0x13: field 0x44
 *            0x14: field 0x45
 *            ...through 0x1D: field 0x4C
 * Phase 3: For types > 0x1D, performs additional lookups via
 *          fn_8012640C and fn_80125424.
 *
 * [Assembly stub - full decompilation requires jump table analysis]
 * ========================================================================= */
/* TODO: Decompile fn_801F02AC (0x46C bytes, jumptable_803752F8) */

/* =========================================================================
 * fn_801F4C14 - PokemonSet
 *
 * Massive setter dispatch (0x890 bytes). Validates the field ID against
 * 0x60, then determines the target Pokemon:
 *   - If field < 8: resolve via fn_801F6738 (party slot by index)
 *   - If field < 0x5F and pokemon is NULL: use fn_801F6B48 (current)
 * Then dispatches through jumptable_80375330 (94 entries) to call
 * the appropriate setter function.
 *
 * First few jumptable entries call:
 *   Case 0x00: fn_801F65F0 (set species via u16)
 *   Case 0x01: fn_801F667C (set held item via u16)
 *   Case 0x02: fn_801F66EC (set move by slot)
 *   ...
 *
 * [Assembly stub - full decompilation requires jump table analysis]
 * ========================================================================= */
/* TODO: Decompile fn_801F4C14 (0x890 bytes, jumptable_80375330) */

/* =========================================================================
 * fn_801F54A4 - PokemonGet
 *
 * Massive getter dispatch (0xD18 bytes). Same structure as PokemonSet:
 *   - Validates field ID against 0x60
 *   - Resolves Pokemon pointer
 *   - Dispatches through jumptable_803754AC (93 entries)
 *
 * First few jumptable entries call:
 *   Case 0x00: fn_801F6600 (get species, u16)
 *   Case 0x01: fn_801F6720 (get held item, u16, clrlwi r3, r3, 16)
 *   Case 0x02: fn_801F66EC (get move by slot index)
 *   ...
 *
 * [Assembly stub - full decompilation requires jump table analysis]
 * ========================================================================= */
/* TODO: Decompile fn_801F54A4 (0xD18 bytes, jumptable_803754AC) */

/* =========================================================================
 * Getter/Setter pairs for Pokemon structure fields
 *
 * These are the lowest-level accessors. Each pair accesses a specific
 * offset in the Pokemon structure. The "addis r3, r3, 1" plus negative
 * offset pattern means the actual struct offset = 0x10000 + (-0x5bXX) = 0xA4XX.
 * ========================================================================= */

/* --- fn_801F640C / fn_801F6414: Global state pointer (lbl_8047B5F0) --- */
void SetPokemonStatePtr(void* ptr) {
    /* stw r3, lbl_8047B5F0@sda21(r0) */
    /* Direct SDA write - handled by linker */
}

void* GetPokemonStatePtr(void) {
    /* lwz r3, lbl_8047B5F0@sda21(r0) */
    return NULL; /* Placeholder */
}

/* --- fn_801F641C / fn_801F6430: Struct field at offset 0xA490 --- */
/* TODO: Identify this field - possibly personality value or PID */

/* --- fn_801F644C / fn_801F6460: Struct field at offset 0xA48C --- */
/* TODO: Identify this field - possibly original trainer ID */

/* --- fn_801F647C / fn_801F6490: Struct field at offset 0xA488 --- */
/* TODO: Identify this field - possibly experience points */

/* --- fn_801F64AC / fn_801F64C0: Struct field at offset 0xA484 --- */
/* TODO: Identify this field - possibly encryption key */

/* --- fn_801F64DC / fn_801F64F0: Struct field at offset 0xA4E4 (u16) --- */
/* TODO: Identify this field - possibly species ID */

/* --- fn_801F650C / fn_801F6544: Struct array at offset 0xA4C4 (u32[8]) --- */
/* This is likely the move/PP data array. 8 entries x 4 bytes = 32 bytes.
 * With 4 moves, each entry could hold move ID + PP packed as u16+u16. */

/* --- fn_801F65C0 / fn_801F65D4: Struct field at offset 0xA4C0 (u16) --- */
/* TODO: Identify this field - possibly held item */

/* =========================================================================
 * fn_801F3CE8 - PartyCompare
 *
 * Large function (0x538 bytes) that compares two Pokemon for sorting.
 * Calls fn_802050F4 on each to get a comparison value (likely nature
 * or friendship-derived), then swaps entries if needed. Used by the
 * party sorting algorithm in fn_801F3BB4.
 *
 * Parameters: r3=context, r4=pokemonA, r5=pokemonB, r6=sortFlags
 * ========================================================================= */
/* TODO: Decompile fn_801F3CE8 (0x538 bytes) */

/* =========================================================================
 * fn_801F0B00 - PartyInit
 *
 * Large initialization function (0x404 bytes) that sets up the party
 * system. Called from battle_main.c during battle scene setup.
 * Initializes multiple rendering passes via scene callbacks.
 * ========================================================================= */
/* TODO: Decompile fn_801F0B00 (0x404 bytes) */

/* =========================================================================
 * fn_801F2B5C - PartyCopy
 *
 * Deep-copies Pokemon data between party slots (0x3E0 bytes).
 * Iterates through all fields and copies them individually via
 * the PokemonGet/PokemonSet interface.
 * ========================================================================= */
/* TODO: Decompile fn_801F2B5C (0x3E0 bytes) */

/* =========================================================================
 * Stat calculation helpers (0x801F6B54 - 0x801F7F80)
 * ========================================================================= */

/* fn_801F6B54 (0xF8 bytes): Calculate effective stat with nature modifier */
/* fn_801F7090 (0xE4 bytes): Calculate HP stat */
/* fn_801F7174 (0xE4 bytes): Calculate non-HP stat */
/* fn_801F7530 (0xC8 bytes): Apply stat stage modifiers */
/* fn_801F7954 (0x21C bytes): Level-up stat recalculation */
/* fn_801F7C54 (0x20C bytes): Full stat recalculation from base+IV+EV+nature */

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 45 functions matched
 * =================================================================== */

extern u32 lbl_8047B5F0;

/* Forward declarations for converted functions */
u16 fn_801F6720(u8* ptr);
u32 fn_801F6600(u8* ptr);
u32 fn_801F66EC(u8* ptr, u8 idx);

/* Forward declarations for functions referenced by fn_801F54A4 asm wrapper */
u8* fn_801F6544(u8* ptr);
u32 fn_801F6588(u8* ptr, u16 idx);
u8* fn_801F6AB4(u8* ptr, u16 idx);
u8* fn_801F6B18(u8* ptr, u16 idx);
u32 fn_801F7258(u32 param_1);
u32 fn_801F7404(u32 param_1);
void fn_801F025C(u32 slotType, u32 idx);
u32 fn_801F02AC(u32 slotType, u32 ptr, u32 count);
void fn_801F2B5C(u32, void (*)(u32, u32, u32), u32, u8);
void fn_801F3BB4(void*, u32*, u16, u32);
s32 fn_801F3CE8(void*, void*, void*, u8);
u8 fn_801F4C14(u32, u16, u32, u16, u32);
void fn_801F65F0(u8* ptr, u32 val);
void fn_801F667C(u8* ptr, u16 val);
void fn_801F6B54(u32, u32, u32, u32, u32);
s32 fn_801F7090(u32, u32, u32);
s32 fn_801F7174(u32, u32, u32);
void fn_801F7530(u32, u16);
void fn_801F7954(u8* ptr, u8* arr);
void fn_801F7C54(u8* ptr, u16* out, u16 count, u8 mode);

/* Address: 0x801F0204 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801F0204(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x801F021C | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801F021C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* Address: 0x801F640C | Size: 0x8 | Pattern: sda_setter */
void fn_801F640C(u32 val) {
    lbl_8047B5F0 = val;
}

/* Address: 0x801F6414 | Size: 0x8 | Pattern: sda_getter */
u32 fn_801F6414(void) {
    return lbl_8047B5F0;
}

/* Address: 0x801F641C | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F641C(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA490) = val;
}

/* Address: 0x801F644C | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F644C(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA48C) = val;
}

/* Address: 0x801F647C | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F647C(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA488) = val;
}

/* Address: 0x801F64AC | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F64AC(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA484) = val;
}

/* Address: 0x801F64DC | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F64DC(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)((u8*)ptr + 0xA4E4) = val;
}

/* Address: 0x801F65C0 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F65C0(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)((u8*)ptr + 0xA4C0) = val;
}

/* Address: 0x801F6618 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801F6618(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x10]) = val;
}

/* Address: 0x801F6628 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801F6628(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0xC]) = val;
}

/* Address: 0x801F6638 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801F6638(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x8]) = val;
}

/* Address: 0x801F6648 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801F6648(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x0]) = val;
}

/* Address: 0x801F668C | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801F668C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x10]);
}

/* Address: 0x801F66A4 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801F66A4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0xC]);
}

/* Address: 0x801F66BC | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801F66BC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x8]);
}

/* Address: 0x801F66D4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801F66D4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* Address: 0x801F6764 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F6764(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)((u8*)ptr + 0xA4BE) = val;
}

/* Address: 0x801F6778 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F6778(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)((u8*)ptr + 0xA4BC) = val;
}

/* Address: 0x801F678C | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F678C(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)((u8*)ptr + 0xA4BA) = val;
}

/* Address: 0x801F67A0 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F67A0(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)((u8*)ptr + 0xA4B8) = val;
}

/* Address: 0x801F67B4 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F67B4(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA4B0) = val;
}

/* Address: 0x801F67C8 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F67C8(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA4AC) = val;
}

/* Address: 0x801F67DC | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F67DC(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA4A8) = val;
}

/* Address: 0x801F67F0 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F67F0(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA4B4) = val;
}

/* Address: 0x801F6804 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F6804(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA4A4) = val;
}

/* Address: 0x801F6818 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F6818(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA4A0) = val;
}

/* Address: 0x801F682C | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F682C(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA49C) = val;
}

/* Address: 0x801F6840 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F6840(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA498) = val;
}

/* Address: 0x801F6854 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F6854(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA494) = val;
}

/* Address: 0x801F6868 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F6868(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA480) = val;
}

/* Address: 0x801F687C | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F687C(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA47C) = val;
}

/* Address: 0x801F6890 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F6890(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA478) = val;
}

/* Address: 0x801F68A4 | Size: 0x14 | Pattern: nullcheck_addis_setter */
void fn_801F68A4(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)((u8*)ptr + 0xA474) = val;
}

/* Address: 0x801F68B8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801F68B8(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x12]) = val;
}

/* Address: 0x801F68C8 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801F68C8(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x10]) = val;
}

/* Address: 0x801F6AE8 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801F6AE8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x12]);
}

/* Address: 0x801F6B00 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801F6B00(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x10]);
}

/* Address: 0x801F77E0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801F77E0(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x0]) = val;
}

/* Address: 0x801F7858 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801F7858(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x0]);
}

/* Address: 0x801F789C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801F789C(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x522C]) = val;
}

/* Address: 0x801F78AC | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801F78AC(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x0]) = val;
}

/* Address: 0x801F78BC | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801F78BC(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x522C]);
}

/* Address: 0x801F793C | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801F793C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x0]);
}

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 27 functions matched
 * =================================================================== */

/* Address: 0x801F6430 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F6430(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA490);
}

/* Address: 0x801F6460 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F6460(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA48C);
}

/* Address: 0x801F6490 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F6490(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA488);
}

/* Address: 0x801F64C0 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F64C0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA484);
}

/* Address: 0x801F64F0 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F64F0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)((u8*)ptr + 0xA4E4);
}

/* Address: 0x801F65D4 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F65D4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(s16*)((u8*)ptr + 0xA4C0);
}

/* Address: 0x801F65F0 | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801F65F0(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x4]) = val;
}

/* Address: 0x801F6600 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801F6600(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x801F667C | Size: 0x10 | Pattern: nullcheck_setter */
void fn_801F667C(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x2]) = val;
}

/* Address: 0x801F6720 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fn_801F6720(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x2]);
}

/* Address: 0x801F68D8 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F68D8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)((u8*)ptr + 0xA4BE);
}

/* Address: 0x801F68F4 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F68F4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)((u8*)ptr + 0xA4BC);
}

/* Address: 0x801F6910 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F6910(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)((u8*)ptr + 0xA4BA);
}

/* Address: 0x801F692C | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F692C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)((u8*)ptr + 0xA4B8);
}

/* Address: 0x801F6948 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F6948(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA4B0);
}

/* Address: 0x801F6964 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F6964(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA4AC);
}

/* Address: 0x801F6980 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F6980(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA4A8);
}

/* Address: 0x801F699C | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F699C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA4B4);
}

/* Address: 0x801F69B8 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F69B8(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA4A4);
}

/* Address: 0x801F69D4 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F69D4(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA4A0);
}

/* Address: 0x801F69F0 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F69F0(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA49C);
}

/* Address: 0x801F6A0C | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F6A0C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA498);
}

/* Address: 0x801F6A28 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F6A28(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA494);
}

/* Address: 0x801F6A44 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F6A44(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA480);
}

/* Address: 0x801F6A60 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F6A60(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA47C);
}

/* Address: 0x801F6A7C | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F6A7C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA478);
}

/* Address: 0x801F6A98 | Size: 0x1C | Pattern: nullcheck_addis_getter_bne */
u32 fn_801F6A98(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)((u8*)ptr + 0xA474);
}

/* #######################################################################
 * COVERAGE STUBS: Pokemon data management (0x801F000C - 0x801F7F80)
 * 119 functions remaining for full coverage of pokemon.c TU.
 *
 * Key functions in this range:
 *   fn_801F54A4 (PokemonGet)  - 510 calls, 0xD18 bytes, field dispatch
 *   fn_801F4C14 (PokemonSet)  - 223 calls, 0x890 bytes, field write
 *   fn_801F02AC (PokemonSlotLookup) - 89 calls, 0x46C bytes
 *   fn_801F025C (PokemonSlotLookupDefault) - 438 calls, 0x50 bytes
 *   fn_801F0B00 (PartyInit)   - 0x404 bytes
 *   fn_801F3CE8 (PartyCompare) - 0x538 bytes
 *   fn_801F2B5C (PartyDataCopy) - 0x3E0 bytes
 * ####################################################################### */

/* 0x801F000C | size: 0x4C | small */
void fn_801F000C(void) {
    u32 total;
    u32 elapsed;
    total = fn_80008184();
    if (total != 0) {
        elapsed = 0;
        while (elapsed < total) {
            _threadSwitch();
            elapsed += fn_800D3088();
        }
    }
}

/* 0x801F0058 | size: 0x78 | small */
u32 fn_801F0058(u32 param1, u32 param2) {
    extern u32 fn_801F02AC();
    u32 slotVal;
    u32 cmpVal;
    slotVal = fn_801F02AC(4, 0, param2);
    if (slotVal == 0)
        return 0;
    cmpVal = fn_801F02AC(2, param1, param2);
    if (cmpVal == 0)
        return 0;
    return (u32)__cntlzw(slotVal - cmpVal) >> 5;
}

/* 0x801F00D0 | size: 0x64 | small */
u32 fn_801F00D0(u32 param1, u32 param2) {
    extern u32 fn_801F02AC();
    u32 slot;
    u32 result;
    slot = fn_801F02AC(4, 0, param2);
    if (slot == 0)
        return 0;
    result = fn_801F02AC(param1, slot, param2);
    if (result == 0)
        result = 0;
    return result;
}

/* 0x801F0134 | size: 0xD0 | medium */
u32 fn_801F0134(u32 param1, u32 param2) {
    extern u8 lbl_80375AC8[];
    extern u32 lbl_80478D40;
    extern u32 fn_801F02AC();
    u32 i;
    u32 slotBase;
    u8* entry;
    u32 byte;
    slotBase = fn_801F02AC(4, 0, param2);
    if (slotBase == 0)
        return 0;
    i = 0;
    while ((u16)i < lbl_80478D40) {
        if ((u16)i != 0) {
            entry = &lbl_80375AC8[((u32)(u16)i) * 8];
            if ((u16)i >= lbl_80478D40)
                entry = NULL;
            if (entry != NULL) {
                if (entry == NULL)
                    byte = 0;
                else
                    byte = entry[1];
                if ((u8)byte != 0) {
                    if (fn_801F02AC(i, slotBase, param2) == param1) {
                        return i;
                    }
                }
            }
        }
        i++;
    }
    i = 0;
    return i;
}

/* 0x801F0234 | size: 0x28 | small */
u8* fn_801F0234(u32 index) {
    extern u8 lbl_80375AC8[];
    extern u32 lbl_80478D40;
    u8* entry = &lbl_80375AC8[((u32)(u16)index) * 8];
    if ((u16)index < lbl_80478D40)
        return entry;
    return NULL;
}

/* 0x801F025C | size: 0x50 | small */
void fn_801F025C(u32 slotType, u32 idx) {
    extern u32 fn_801F54A4();
    extern void fn_801F02AC();
    u32 count;
    count = (u16)fn_801F54A4(0, 0, 0x14, 0);
    fn_801F02AC(slotType, idx, count);
}

/* 0x801F02AC | size: 0x46C | large */
u32 fn_801F02AC(u32 slotType, u32 ptr, u32 count) {
    extern u32 fn_801F54A4(u32, u32, u32, u32);
    extern u32 fn_801FB1C0(u32, u32, u32, u32);
    extern u32 fn_8020E1A4(u32);
    extern u32 fn_8020E1D4(u32);
    extern u32 fn_8020E204(u32);
    extern u32 fightSideGetStatus(u32, u32, u32, u32);
    extern u32 fn_801F0718(u32, u32);
    u32 slots[2];
    u32 sidePkm[4];
    u32 pkmAbil[8];
    u32 ctx;
    u32 ctxObj;
    u32 savedPtr;
    u32 numSides;
    u32 m;
    u32 n;
    u32 k;
    u32 i;
    u32 j;
    u32 flag;

    if ((u16)slotType == 0) {
        return 0;
    }
    ctx = (u32)fn_801F61BC();
    if ((u16)slotType == 1) {
        return ctx;
    }

    switch ((u16)slotType) {
    case 0x11: return fn_801F54A4(ctx, 0, 0x36, 0);
    case 0x12: return fn_801F54A4(ctx, 0, 0x42, 0);
    case 0x13: return fn_801F54A4(ctx, 0, 0x44, 0);
    case 0x14: return fn_801F54A4(ctx, 0, 0x45, 0);
    case 0x15: return fn_801F54A4(ctx, 0, 0x46, 0);
    case 0x16: return fn_801F54A4(ctx, 0, 0x47, 0);
    case 0x17: return fn_801F54A4(ctx, 0, 0x48, 0);
    case 0x18: return fn_801F54A4(ctx, 0, 0x49, 0);
    case 0x19: return fn_801F54A4(ctx, 0, 0x4b, 0);
    case 0x1a: return fn_801F54A4(ctx, 0, 0x4c, 0);
    case 0x1b: return fn_801F54A4(ctx, 0, 0x4d, 0);
    case 0x1c: return fn_801F54A4(ctx, 0, 0x4e, 0);
    case 0x1d: return fn_801F54A4(ctx, 0, 0x4f, 0);
    }

    ctxObj = fn_8020E204(count);
    if (ctxObj == 0) {
        return 0;
    }
    savedPtr = ptr;
    if (ptr != 0) {
        ptr = fn_801F0718(ptr, count);
        if (ptr == 0) {
            return 0;
        }
    }
    m = 0;
    n = 0;
    count = fn_8020E1D4(ctxObj);
    ctxObj = (u8)fn_8020E1A4(ctxObj);
    numSides = (u8)count;
    i = 0;
    while ((u16)i < 2) {
        slots[(u16)i] = fn_801F54A4(ctx, 0, 0x35, i);
        if ((u16)slotType == 4 && (u16)i == 0) {
            return slots[(u16)i];
        }
        if ((u16)slotType == 5 && (u16)i == 1) {
            return slots[(u16)i];
        }
        if (slots[(u16)i] == ptr) {
            flag = 1;
            if ((u16)slotType == 2) return slots[(u16)i];
        } else {
            flag = 0;
            if ((u16)slotType == 3) return slots[(u16)i];
        }
        count = 0;
        j = 0;
        while ((u16)j < (u16)numSides) {
            sidePkm[(u16)n] = fightSideGetStatus(slots[(u16)i], 0, 7, j);
            if ((u16)slotType == 0xb && (u16)i == 0 && (u16)j == 0) {
                return sidePkm[(u16)n];
            }
            if (flag == 1) {
                if ((u16)slotType == 6 && (u16)j == 0) return sidePkm[(u16)n];
                if ((u16)slotType == 7 && (u16)j == 1) return sidePkm[(u16)n];
                if ((u16)slotType == 8) {
                    if (savedPtr != sidePkm[(u16)n]) return sidePkm[(u16)n];
                }
                if ((u16)slotType == 9 && (u16)j == 0) return sidePkm[(u16)n];
                if ((u16)slotType == 0xa && (u16)j == 1) return sidePkm[(u16)n];
            }
            k = 0;
            while ((u16)k < (u16)ctxObj) {
                pkmAbil[(u16)m] = fn_801FB1C0(sidePkm[(u16)n], 0, 0x46, k);
                if (flag == 1) {
                    if ((u16)slotType == 0xc && (u16)count == 0) return pkmAbil[(u16)m];
                    if ((u16)slotType == 0xd && (u16)count == 1) return pkmAbil[(u16)m];
                    if ((u16)slotType == 0xe) {
                        if (savedPtr != pkmAbil[(u16)m]) return pkmAbil[(u16)m];
                    }
                    if ((u16)slotType == 0xf && (u16)count == 0) return pkmAbil[(u16)m];
                    if ((u16)slotType == 0x10 && (u16)count == 1) return pkmAbil[(u16)m];
                }
                m++;
                count++;
                k++;
            }
            n++;
            j++;
        }
        i++;
    }
    return 0;
}

/* 0x801F0718 | size: 0x180 | medium */
u32 fn_801F0718(u32 target, u32 param2) {
    extern u32 fn_801F54A4();
    extern u32 fightSideGetStatus();
    extern u32 fn_801FB1C0();
    extern u32 fn_8020E1A4();
    extern u32 fn_8020E1BC();
    extern u32 fn_8020E1D4();
    extern u32 fn_8020E204();
    u32 base;
    u32 group;
    u32 numSub1r;
    u32 numSlotsr;
    u32 numSub2r;
    u32 k;
    u32 sub;
    u32 slot;
    u32 side;
    u32 j;
    u32 tmp;
    base = fn_801F54A4(0, 0, 0, 0);
    group = fn_8020E204(param2);
    if (group == 0)
        return 0;
    tmp = fn_8020E1D4();
    numSub1r = fn_8020E1A4(group);
    numSub2r = fn_8020E1BC(group);
    numSlotsr = tmp;
    numSub2r = (u8)numSub2r;
    numSub1r = (u8)numSub1r;
    numSlotsr = (u8)numSlotsr;
    k = 0;
    while ((u16)k < 2) {
        slot = fn_801F54A4(base, 0, 0x35, k);
        if (slot == target)
            return slot;
        sub = 0;
        while ((s16)sub < (s16)numSlotsr) {
            side = fightSideGetStatus(slot, 0, 7, sub);
            if (side == target)
                return slot;
            j = 0;
            while ((u16)j < (u16)numSub2r) {
                if (fn_801FB1C0(side, 0, 0x45, j) == target)
                    return slot;
                j++;
            }
            j = 0;
            while ((u16)j < (u16)numSub1r) {
                if (fn_801FB1C0(side, 0, 0x46, j) == target)
                    return slot;
                j++;
            }
            sub++;
        }
        k++;
    }
    return 0;
}

/* 0x801F0898 | size: 0x90 | medium */
u32 fn_801F0898(u32 param) {
    extern u32 fn_8020D82C();
    extern u32 fn_8020D920();
    extern u32 fn_8020D950();
    u32 valid;
    if (param != 0) goto chk1;
    valid = 0;
    goto join;
    chk1:
    if ((u16)fn_8020D950() != 0) goto chk2;
    valid = 0;
    goto join;
    chk2:
    if (fn_8020D920(param) != 0) goto set1;
    valid = 0;
    goto join;
    set1:
    valid = 1;
    join:
    if ((u8)valid == 0)
        return 0;
    if (fn_8020D920(param) == 0)
        return 0;
    return fn_8020D82C();
}

/* 0x801F0928 | size: 0xA8 | medium */
s32 fn_801F0928(u32 param) {
    extern u32 fn_8020D7CC();
    extern u32 fn_8020D7E8();
    extern u32 fn_8020D82C();
    extern u32 fn_8020D920();
    extern u32 fn_8020D950();
    u32 valid;
    if (param != 0) goto chk1;
    valid = 0;
    goto join;
    chk1:
    if ((u16)fn_8020D950() != 0) goto chk2;
    valid = 0;
    goto join;
    chk2:
    if (fn_8020D920(param) != 0) goto set1;
    valid = 0;
    goto join;
    set1:
    valid = 1;
    join:
    if ((u8)valid == 0)
        return -0x80;
    if (fn_8020D920(param) == 0)
        return -0x80;
    fn_8020D82C();
    if (fn_8020D7E8() == 0)
        return -0x80;
    return fn_8020D7CC();
}

/* 0x801F0F04 | size: 0x188 | medium */
u32 fn_801F0F04(void* param) {
    extern u8 lbl_8046D790[];
    extern u32 lbl_8047B5E8;
    extern u32 lbl_8047B5EC;
    extern void fn_8020D78C(void*, u32);
    extern u16 fn_8020D950(void*);
    extern void* fn_8020D920(void*);
    extern void fn_8020D82C(void*);
    extern void fn_8020D7E8(void*);
    extern void fn_8020D7B4(void*);
    extern void fn_8020D968(void*, void*);
    struct Entry { u32 data[12]; };
    void* src;
    u32 idx;
    struct Entry* slot;
    u32 result;
    void* fn;

    src = param;
    idx = lbl_8047B5E8;
    if (((idx + 1) & 0x1F) != lbl_8047B5EC) {
        slot = (struct Entry*)((u8*)lbl_8046D790 + idx * 0x30);
        *(struct Entry*)slot = *(struct Entry*)src;
        lbl_8047B5E8 = (idx + 1) & 0x1F;
        fn_8020D78C(src, idx);
        fn_8020D78C(slot, idx);
    } else {
        slot = 0;
    }
    if (slot == 0) {
        return 0x2;
    }
    {
        u32 active = 0;
        if (src != 0) {
            if ((fn_8020D950(src) & 0xFFFF) != 0) {
                if (fn_8020D920(src) != 0) {
                    active = 1;
                }
            }
        }
        if ((active & 0xFF) == 0) {
            result = 0;
        } else {
            fn = fn_8020D920(src);
            fn_8020D82C(src);
            fn_8020D7E8(src);
            fn_8020D7B4(src);
            if (fn != 0) {
                result = ((u32(*)(void*))fn)(src);
            } else {
                result = 1;
            }
        }
    }
    if ((result & 0xFF) != 1) {
        if (lbl_8047B5E8 != lbl_8047B5EC) {
            lbl_8047B5EC = (lbl_8047B5EC + 1) & 0x1F;
        }
        return result;
    }
    fn_8020D968(slot, src);
    return result;
}

/* 0x801F1170 | size: 0x5C | small */
u32 fn_801F1170(void* param) {
    extern u16 fn_8020D950(void*);
    extern void* fn_8020D920(void*);
    void* obj;

    obj = param;
    if (obj == 0) {
        return 0;
    }
    if ((fn_8020D950(obj) & 0xFFFF) == 0) {
        return 0;
    }
    return -(s32)fn_8020D920(obj) != 0;
}

/* 0x801F11CC | size: 0x294 | large */
s32 fn_801F11CC(void* p1, void* p2, void* p3, void* p4, void* p5, void* p6) {
    extern void fn_8020D78C(void*, s32);
    extern void* fn_8020D814(void*);
    extern u32 fn_8020D82C(void*);
    extern void fn_8020D844(void*, u32, u32);
    extern void fn_8020D868(void*, void*);
    extern void fn_8020D878(void*, void*);
    extern void fn_8020D888(void*, u32);
    extern void fn_8020D898(void*, void*);
    extern void fn_8020D8A8(void*, void*);
    extern void fn_8020D8B8(void*, void*);
    extern void fn_8020D8C8(void*, void*);
    extern void* fn_8020D920(void*);
    extern void* fn_8020D938(void*);
    extern u32 fn_8020D950(void*);
    void* param2;
    void* obj;
    void* param3;
    void* param4;
    void* param5;
    void* param6;
    u32 r25;
    u32 r27;
    u32 r28;
    void* r26;
    void* r24;

    param2 = p2;
    obj = p1;
    param3 = p3;
    param4 = p4;
    param5 = p5;
    param6 = p6;

    fn_8020D8C8(obj, 0);
    fn_8020D8B8(obj, 0);
    fn_8020D8A8(obj, 0);
    r25 = 0;
    while ((r25 & 0xFFFF) < 4) {
        fn_8020D844(obj, r25, 0);
        r25++;
    }
    fn_8020D888(obj, 0);
    fn_8020D878(obj, 0);
    fn_8020D898(obj, 0);
    fn_8020D868(obj, 0);
    fn_8020D78C(obj, -1);
    fn_8020D8C8(obj, param4);
    fn_8020D8B8(obj, param5);
    fn_8020D8A8(obj, param6);
    fn_8020D898(obj, param3);

    {
        u32 active = 0;
        if (obj != 0) {
            if ((fn_8020D950(obj) & 0xFFFF) != 0) {
                if (fn_8020D920(obj) != 0) {
                    active = 1;
                }
            }
        }
        if ((active & 0xFF) == 0) {
            r24 = 0;
            goto end;
        }
    }

    r27 = fn_8020D950(obj);
    r25 = (u32)fn_8020D938(obj);
    r26 = fn_8020D920(obj);
    if (r26 != 0) {
        param6 = 0;
        r28 = r27 & 0xFFFF;
        while (1) {
            r24 = (void*)((u8*)r26 + ((u32)param6 & 0xFFFF) * 8);
            r27 = fn_8020D82C(r24);
            if ((r27 & 0xFFFF) != 0) {
                if (r28 == (fn_8020D82C(r24) & 0xFFFF) && (void*)r25 == fn_8020D814(r24)) {
                    goto found;
                }
                param6 = (void*)((u32)param6 + 1);
                continue;
            }
            r24 = 0;
            break;
        }
    } else {
        r24 = 0;
    }
found:
    if (r24 != 0) {
        fn_8020D8A8(obj, r24);
    }
end:
    if (r24 == 0) {
        fn_8020D8C8(obj, 0);
        fn_8020D8B8(obj, 0);
        fn_8020D8A8(obj, 0);
        r25 = 0;
        while ((r25 & 0xFFFF) < 4) {
            fn_8020D844(obj, r25, 0);
            r25++;
        }
        fn_8020D888(obj, 0);
        fn_8020D878(obj, 0);
        fn_8020D898(obj, 0);
        fn_8020D868(obj, 0);
        fn_8020D78C(obj, -1);
        return 4;
    }
    fn_8020D878(obj, param5);
    fn_8020D868(obj, param2);
    return 1;
}

/* 0x801F1460 | size: 0xB4 | fightActionInit */
void fightActionInit(u8* ptr) {
    extern void fn_8020D8C8(u8*, u32);
    extern void fn_8020D8B8(u8*, u32);
    extern void fn_8020D8A8(u8*, u32);
    extern void fn_8020D844(u8*, u32, u32);
    extern void fn_8020D888(u8*, u32);
    extern void fn_8020D878(u8*, u32);
    extern void fn_8020D898(u8*, u32);
    extern void fn_8020D868(u8*, u32);
    extern void fn_8020D78C(u8*, s32);
    u32 i;

    fn_8020D8C8(ptr, 0);
    fn_8020D8B8(ptr, 0);
    fn_8020D8A8(ptr, 0);
    for (i = 0; (u16)i < 4; i++) {
        fn_8020D844(ptr, i, 0);
    }
    fn_8020D888(ptr, 0);
    fn_8020D878(ptr, 0);
    fn_8020D898(ptr, 0);
    fn_8020D868(ptr, 0);
    fn_8020D78C(ptr, -1);
}

/* 0x801F150C | size: 0x48 | small */
void fn_801F150C(void* param) {
    extern void fn_801F37B0(void*, void*, u32, u32);
    extern void fn_801F3984(void*, u32);
    extern s32 fn_801F1554(void*);
    void* obj;

    obj = param;
    fn_801F37B0(obj, fn_801F1554, 0, 0);
    fn_801F3984(obj, 1);
}

/* 0x801F1554 | size: 0x34 */
extern u32 fn_801254B4(void* context, u32 slot, u16 tableId, u32 flags, u32 value);
s32 fn_801F1554(void* context) {
    fn_801254B4(context, 0, 0x112, 0, 1);
    return 1;
}

/* 0x801F1588 | size: 0x178 | medium */
void fn_801F1588(void* param) {
    extern u16 fn_801EF634(void*);
    extern u32 fn_801F025C(u32, u32);
    extern void fn_801F3984(void*, u32);
    extern u32 fn_801F54A4(void*, u32, u32, u32);
    extern u32 fn_801F6FD4(u32, u16, u16);
    extern u32 fn_801F7090(u32, u16, u16);
    extern u32 fn_801F7174(u32, u16, u16);
    void* obj;
    u16 slot1;
    u16 slot2;
    u32 op;
    u32 base1;
    u32 base2;
    u32 pct1;
    u32 pct2;

    obj = param;
    if ((u16)fn_801EF634(obj) == 1) {
        return;
    }
    slot1 = fn_801F54A4(obj, 0, 0x16, 0) & 0xFFFF;
    slot2 = fn_801F54A4(obj, 0, 0x17, 0) & 0xFFFF;

    op = fn_801F025C(4, 0);
    base1 = fn_801F6FD4(op, slot1, slot2);
    base2 = fn_801F7174(op, slot1, slot2);
    pct1 = base2 * 0x64 / fn_801F7090(op, slot1, slot2);

    op = fn_801F025C(5, 0);
    base2 = fn_801F6FD4(op, slot1, slot2);
    pct2 = fn_801F7174(op, slot1, slot2);
    pct2 = pct2 * 0x64 / fn_801F7090(op, slot1, slot2);

    fn_801F3984(obj, 0);
    if ((base1 & 0xFFFF) > (base2 & 0xFFFF)) {
        fn_801F3984(obj, 2);
    }
    if ((base1 & 0xFFFF) < (base2 & 0xFFFF)) {
        fn_801F3984(obj, 3);
    }
    if ((u16)fn_801EF634(obj) == 0) {
        if (pct1 >= pct2) {
            fn_801F3984(obj, 2);
        }
        if (pct1 <= pct2) {
            fn_801F3984(obj, 3);
        }
    }
}

/* 0x801F1700 | size: 0x58 | small */
u32 fn_801F1700(void* param) {
    extern u32 fn_801F54A4(void*, u32, u32, u32);
    extern s32 fn_80077B84(u32);
    u32 val;
    u8 flag;
    s32 r;

    val = fn_801F54A4(param, 0, 0x34, 0);
    flag = val & 0xFF;
    r = fn_80077B84(val);
    if (flag == 1 && r > 0) {
        return 1;
    }
    return 0;
}

/* 0x801F17B0 | size: 0xD8 | medium */
void fn_801F17B0(void* obj) {
    extern void* fn_801F54A4(void*, u32, u32, u32);
    extern void* fightSideGetStatus(void*, u32, u32, u32);
    extern void* fn_801FB1C0(void*, u32, u32, u32);
    extern void fn_801254B4(void*, u32, u32, u32, u32);
    void* a;
    void* b;
    void* c;
    u32 j;
    u32 k;
    u32 i;

    i = 0;
    while ((i & 0xFFFF) < 2) {
        a = fn_801F54A4(obj, 0, 0x35, i);
        if (a != 0) {
            j = 0;
            while ((j & 0xFFFF) < 2) {
                b = fightSideGetStatus(a, 0, 0x7, j);
                if (b != 0) {
                    k = 0;
                    while ((k & 0xFFFF) < 2) {
                        c = fn_801FB1C0(b, 0, 0x46, k);
                        if (c != 0) {
                            fn_801254B4(c, 0, 0xfa, 0, 0);
                        }
                        k++;
                    }
                }
                j++;
            }
        }
        i++;
    }
}

/* 0x801F1888 | size: 0x54 | small */
u32 fn_801F1888(u32 param_1) {
    extern u32 fn_801F54A4(u32, u32, u32, u32);
    u16 v;

    v = fn_801F54A4(param_1, 0, 0x1A, 0);
    if (v == 0xA || v == 0x11 || v == 0xB) {
        return 1;
    }
    return 0;
}

/* 0x801F18DC | size: 0x3C | small */
u8 fn_801F18DC(u32 param_1) {
    extern u32 fn_801F54A4(u32, u32, u32, u32);

    return (0x10 - (fn_801F54A4(param_1, 0, 0x1a, 0) & 0xFFFF)) == 0;
}

/* 0x801F1918 | size: 0x74 | small */
void fn_801F1918(void* obj, u32 arg2, void* dst) {
    extern u32 fn_802037DC(u32);
    extern void fn_800F9E70(void*, u32);
    extern u32 fn_801F54A4(void*, u32, u32, u32);
    void* d;
    void* o;

    o = obj;
    d = dst;
    fn_800F9E70(d, fn_802037DC(arg2));
    if ((fn_801F54A4(o, 0, 0x22, 0) & 0xFF) == 1) {
        *(u8*)((u8*)d + 0x16) = 0;
    } else {
        *(u8*)((u8*)d + 0x16) = 1;
    }
}

/* 0x801F198C | size: 0x4 | trivial */
void fn_801F198C(void) {}

/* 0x801F1990 | size: 0xDC | medium */
u32 fn_801F1990(void* arg1, void* arg2, u8 arg3, u8 arg4, void* arg5, void* arg6) {
    extern void* _fightFloorGetFightTrainerFightOutPokemonPtrArySub__FPvUsPv(void*, u16, void*);
    extern void fn_801F37B0(void*, void*, void*, u32);
    extern u32 fn_80204DE4(void*, void*, void*);
    void* ary[8];
    struct { void* r4; void* arr; u32 cnt; u32 a; u32 b; } sdata;
    void* ctx1;
    void* ctx2;
    u32 count;
    u32 i;

    ctx1 = arg5;
    ctx2 = arg6;
    {
        u32 j = 0;
        while ((j & 0xFFFF) < 8) {
            ary[(j & 0xFFFF)] = 0;
            j++;
        }
    }
    sdata.r4 = arg2;
    sdata.arr = ary;
    sdata.cnt = 0;
    sdata.a = arg3;
    sdata.b = arg4;
    fn_801F37B0(arg1, _fightFloorGetFightTrainerFightOutPokemonPtrArySub__FPvUsPv, &sdata, 0);
    count = sdata.cnt & 0xFFFF;
    i = 0;
    while ((i & 0xFFFF) < count) {
        if ((fn_80204DE4(ary[(i & 0xFFFF)], ctx1, ctx2) & 0xFF) == 1) {
            return 1;
        }
        i++;
    }
    return 0;
}

/* 0x801F1A6C | size: 0xA8 | medium */
u16 fn_801F1A6C(u32 obj, u32 slot, u32* arr, u8 r6, u8 r7_orig) {
    extern u32 fn_801F54A4(u32, u32, u32, u32);
    extern void fn_801F2B5C(u32, void*, void*, u32);
    extern void _fightFloorGetFightTrainerFightPokemonPtrArySub__FPvUsPv(void);
    u32 buf[6];
    u16 i;
    u32 val;
    u32 res;
    i = 0;
    val = i;
    while ((u16)i < 0x18) {
        arr[i] = val;
        i++;
    }
    buf[0] = slot;
    buf[1] = r6;
    buf[2] = 0;
    buf[3] = (u32)arr;
    res = fn_801F54A4(0, 0, 0x17, 0);
    buf[4] = res;
    buf[5] = r7_orig;
    fn_801F2B5C(obj, (void*)_fightFloorGetFightTrainerFightPokemonPtrArySub__FPvUsPv, buf, 0);
    return (u16)buf[2];
}

/* 0x801F1B14 | size: 0x184 | medium */
u32 _fightFloorGetFightTrainerFightPokemonPtrArySub__FPvUsPv(void* obj, u32 slot, void* data) {
    extern u32 fn_801FA634();
    extern u32 fn_801F8424(void*, u32, u32);
    extern void* fn_801F986C(void*, u32);
    extern u32 fn_80206608();
    struct { u32 slotVal; s32 flag1; u32 count; void** arr; u32 maxcnt; s32 flag2; }* s = data;
    u32 slotVal;
    void** arr;
    void* ptr;
    u32 maxcnt;
    u32 i;

    slotVal = s->slotVal;
    arr = s->arr;
    if ((u8)fn_801FA634() == 0)
        return 1;
    if (s->flag1 == 0) {
        if ((u8)fn_801F8424(obj, slotVal, slot) == 1)
            return 1;
    } else {
        if ((u8)fn_801F8424(obj, slotVal, slot) == 0)
            return 1;
    }
    i = 0;
    maxcnt = (u16)s->maxcnt;
    while ((i & 0xFFFF) < (maxcnt & 0xFFFF)) {
        ptr = fn_801F986C(obj, i);
        if (ptr != 0) {
            if (s->flag2 != 1 || (u8)fn_80206608() != 0) {
                arr[s->count] = ptr;
                s->count++;
            }
        }
        i++;
    }
    return 1;
}

/* 0x801F1C18 | size: 0x80 | small */
u16 fn_801F1C18(u32 obj, u32 slot, u32* arr, u8 r6, u8 r7) {
    extern void fn_801F37B0(u32, void*, void*, u32);
    extern void _fightFloorGetFightTrainerFightOutPokemonPtrArySub__FPvUsPv(void);
    u32 buf[5];
    u16 i;
    u32 val;
    i = 0;
    val = i;
    while ((u16)i < 8) {
        arr[i] = val;
        i++;
    }
    buf[0] = slot;
    buf[1] = (u32)arr;
    buf[2] = 0;
    buf[3] = r6;
    buf[4] = r7;
    fn_801F37B0(obj, (void*)_fightFloorGetFightTrainerFightOutPokemonPtrArySub__FPvUsPv, buf, 0);
    return (u16)buf[2];
}

/* 0x801F1C98 | size: 0xC0 | medium */
u32 _fightFloorGetFightTrainerFightOutPokemonPtrArySub__FPvUsPv(void* obj, u32 slot, void* data) {
    extern u32 fn_802062FC();
    extern u32 fn_801F8424(void*, void*, u32);
    struct { u32 slotVal; void** arr; u32 count; s32 flag; s32 mode; }* s = data;
    u32 slotVal;
    void** arr;

    slotVal = s->slotVal;
    arr = s->arr;
    if (s->mode == 1) {
        if ((u8)fn_802062FC() == 0)
            return 1;
    }
    if (s->flag == 0) {
        if ((u8)fn_801F8424((void*)slotVal, obj, slot) == 1)
            return 1;
    } else {
        if ((u8)fn_801F8424((void*)slotVal, obj, slot) == 0)
            return 1;
    }
    arr[s->count] = obj;
    s->count = s->count + 1;
    return 1;
}

/* 0x801F1D5C | size: 0x60 | small */
void fn_801F1D5C(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32* buf) {
    extern void fn_801F61EC(u32, u32*, u32, u32, u32);
    u16 i = 0;
    while ((u16)i < 8) {
        buf[i] = 0;
        i++;
    }
    fn_801F61EC(arg0, buf, arg1, arg2, arg3);
}

/* 0x801F1DBC | size: 0x174 | medium */
u32 fn_801F1DBC(u32 obj, u32 side) {
    extern u32 fn_801F54A4(u32, u32, u32, u32);
    extern u32 fn_801F7404(void);
    extern u32 fn_801F7258(u32, u32);
    extern u32 fn_801FB8F8(void);
    extern u32 fn_801F0058(u32, u32);
    u32 r31;
    u32 r24;
    u32 r30;
    u32 r28;
    u32 r27;
    u32 r25;
    u32 r26;
    u32 r29;
    u32 r0;
    r31 = side;
    r24 = obj;
    if ((u16)side != 2) {
        if ((u16)side != 3) return 0;
    }
    r30 = (u16)fn_801F54A4(r24, 0, 0x14, 0);
    fn_801F54A4(r24, 0, 0x14, 0);
    r28 = (u16)fn_801F54A4(r24, 0, 0x16, 0);
    r27 = 0;
    while ((u16)r27 < 2) {
        r25 = fn_801F54A4(r24, 0, 0x35, r27);
        if ((u8)fn_801F7404() == 0)
            r25 = 0;
        if (r25 != 0) {
            r26 = 0;
            while ((u16)r26 < r28) {
                r29 = fn_801F7258(r25, r26);
                if (r29 != 0) {
                    if ((u8)fn_801FB8F8() == 1)
                        goto _found;
                }
                r26++;
            }
        }
        r27++;
    }
    r29 = 0;
_found:
    if (r29 != 0) {
        if ((u8)fn_801F0058(r29, r30) == 1)
            r0 = 1;
        else
            r0 = 0;
    } else {
        r0 = 0;
    }
    if ((u8)r0 == 1) {
        if ((u16)r31 == 2) return 1;
        return 0;
    }
    if ((u16)r31 == 3) return 1;
    return 0;
}

/* 0x801F1F30 | size: 0x4C | small */
u8 fn_801F1F30(u32 param_1, u32 param_2, u16 param_3) {
    extern void fn_801F37B0(u32, void*, void*, u32);
    extern void _fightFloorCheckHuuinWazaFightOutPokemonSub__FPvUsPv(void);
    u32 buf[3];

    buf[0] = param_2;
    buf[1] = param_3;
    buf[2] = 0;
    fn_801F37B0(param_1, (void*)_fightFloorCheckHuuinWazaFightOutPokemonSub__FPvUsPv, buf, 0);
    return (u8)buf[2];
}

/* 0x801F1F7C | size: 0x90 | medium */
u32 _fightFloorCheckHuuinWazaFightOutPokemonSub__FPvUsPv(void* obj, u32 slot, void* data) {
    extern u32 fn_802062FC();
    extern u32 fn_80202B88(void*, void*);
    extern u32 fn_802026E4(void*, u32);
    extern void* fn_80205B8C(void*);
    extern s32 fn_80123B5C(void*, u32);
    struct { void* mon; u32 waza; u32 result; }* s = data;
    void* mon;
    u32 waza;

    mon = s->mon;
    waza = (u16)s->waza;
    if ((u8)fn_802062FC() == 0)
        return 1;
    if ((u8)fn_80202B88(obj, mon) != 0) goto _ret1;
    if ((u8)fn_802026E4(obj, 0x27) != 1) goto _ret1;
    if ((s8)fn_80123B5C(fn_80205B8C(obj), waza) < 0) goto _ret1;
    s->result = 1;
    return 0;
_ret1:
    return 1;
}

/* 0x801F2020 | size: 0x1FC | medium */
u32 fn_801F2020(u32 obj, u32 r4arg, u32* out) {
    extern u32 fn_80206780(u32);
    extern void fn_801F37B0(u32, void*, void*, u32);
    extern u32 fn_80207AE0(u32, u32);
    extern u32 fn_80207BF4(u32);
    extern u32 fn_802026E4(u32, u32);
    extern void _fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv(void);
    u32 buf1[4];
    u32 buf2[4];
    u32 buf3[4];
    u32 r27;
    u32 r26;
    u32 r28;
    u32 r31;
    u32 r30;
    u32 r29;
    u32 r25;
    r27 = r4arg;
    r26 = obj;
    r28 = (u32)out;
    r30 = 0;
    r29 = 0;
    if ((u8)fn_80206780(r27) == 0)
        return 0;
    buf1[0] = 0x17;
    buf1[1] = 0;
    buf1[2] = 2;
    buf1[3] = r27;
    fn_801F37B0(r26, (void*)_fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv, buf1, 0);
    buf2[0] = 0x47;
    buf2[1] = 0;
    buf2[2] = 2;
    buf2[3] = r27;
    r31 = buf1[1];
    fn_801F37B0(r26, (void*)_fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv, buf2, 0);
    r25 = buf2[1];
    buf3[0] = 0x2a;
    buf3[1] = 0;
    buf3[2] = 0;
    buf3[3] = r27;
    fn_801F37B0(r26, (void*)_fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv, buf3, 0);
    r26 = buf3[1];
    if ((u8)fn_80207AE0(r27, 2) == 1) goto _setR30;
    if ((u16)fn_80207BF4(r27) != 0x1a) goto _afterR30;
_setR30:
    r30 = 1;
_afterR30:
    if ((u8)fn_80207AE0(r27, 8) == 1) r29 = 1;
    if ((u8)fn_802026E4(r27, 0x16) == 1) goto _ret1;
    if ((u8)fn_802026E4(r27, 0xe) == 1) goto _ret1;
    if ((u8)fn_802026E4(r27, 0x25) == 1) { _ret1: return 1; }
    if (r31 != 0) {
        if (r28 != 0) *(u32*)r28 = r31;
        return 2;
    }
    if (r25 != 0) {
        if ((u8)r30 == 0) {
            if (r28 != 0) *(u32*)r28 = r25;
            return 2;
        }
    }
    if (r26 != 0) {
        if ((u8)r29 == 1) {
            if (r28 != 0) *(u32*)r28 = r26;
            return 2;
        }
    }
    return 0;
}

/* 0x801F221C | size: 0xBC | medium */
u32 fn_801F221C(u32 obj) {
    extern u32 fn_801F54A4(u32, u32, u32, u32);
    extern u32 fn_802062FC(void);
    extern u32 fn_801F1170(void);
    extern u32 fn_8012640C(u32, u32, u32, u32);
    u32 a;
    u32 pkmn;
    u32 i;
    u32 result;
    a = obj;
    result = 1;
    i = 0;
    while ((u16)i < 8) {
        pkmn = fn_801F54A4(a, 0, 0x59, i);
        if (pkmn == 0)
            goto _next;
        if ((u8)fn_802062FC() == 0)
            goto _next;
        if (fn_8012640C(pkmn, 0, 0xfe, 0) == 0)
            goto _next;
        if ((u8)fn_801F1170() == 0)
            goto _next;
        if ((s32)fn_8012640C(pkmn, 0, 0x112, 0) != 0)
            goto _next;
        result = 0;
        break;
    _next:
        i++;
    }
    return result;
}

/* 0x801F22D8 | size: 0x78 | small */
void fn_801F22D8(u32 obj) {
    extern u32 fn_801F025C(u32, u32);
    extern void fn_801F4C14(u32, u32, u32, u32, u32);
    u32 r30;
    u32 r31;
    u32 tmp;
    r30 = obj;
    r31 = fn_801F025C(0x11, r30);
    tmp = fn_801F025C(0x12, r30);
    fn_801F4C14(r30, 0, 0x36, 0, tmp);
    fn_801F4C14(r30, 0, 0x43, 0, r31);
}

/* 0x801F2350 | size: 0xE4 | medium */
s32 fn_801F2350(u32 unused, u32 r4arg) {
    extern u32 fn_8012640C(u32, u32, u32, u32);
    extern u8 fn_8020E614(u32);
    extern u32 fn_801FD104(u32);
    extern u8 fn_80206780(u32);
    extern u32 fn_801FD0D4(u32);
    extern u32 fn_801FD0BC(u32);
    u32 r31;
    s32 r30;
    u32 r29;
    u16 r28;
    u32 acc;
    s32 score;
    u32 poke;
    if (r4arg == 0) return -1;
    r31 = fn_8012640C(r4arg, 0, 0x122, 0);
    r30 = -1;
    r28 = 0;
    while ((u16)r28 < 4) {
        r29 = r31 + (u16)r28 * 12;
        if ((u8)fn_8020E614(r29) != 0) {
            poke = fn_801FD104(r29);
            if (poke != 0) {
                if ((u8)fn_80206780(poke) != 0) {
                    acc = fn_801FD0BC(r29);
                    r29 = fn_801FD0D4(r29);
                    if ((u16)r29 == 0) r29 = 1;
                    score = ((u16)acc * 0x64) / (u16)r29;
                    if (score > r30) r30 = score;
                }
            }
        }
        r28++;
    }
    return r30;
}

/* 0x801F2434 | size: 0x164 | medium */
s32 fn_801F2434(u32 r3_arg, u32 r4_arg) {
    extern u32 fn_801F61EC(u32, u32*, u32, u32, u32);
    extern u32 fn_80203ADC(u32, u32);
    extern u32 fn_80203B5C(u32, u32);
    u32 buf[8];
    s32 r31;
    u16 r30;
    u32 r29;
    u32 r28;
    u32 r27;
    u32 r26;
    u32 div_a;
    u32 div_b;
    if (r4_arg == 0) return -1;
    {
        u16 i = 0;
        while ((u16)i < 8) { buf[i] = 0; i++; }
    }
    r29 = fn_801F61EC(r3_arg, buf, 1, 2, r4_arg);
    if ((u16)r29 == 0) return -1;
    {
        u16 j = 0;
        while ((u16)j < (u16)r29) {
            r26 = buf[(u16)j];
            if (r26 != 0) {
                r28 = fn_80203ADC(r26, 1);
                div_a = fn_80203B5C(r26, 1);
                if ((u16)div_a == 0) div_a = 1;
                r31 = (s32)((u16)r28 * 0x64) / (u16)div_a;
                break;
            }
            j++;
        }
    }
    r28 = (u32)buf;
    r30 = (u16)r29;
    r27 = 0;
    while ((u16)r27 < r30) {
        r26 = buf[(u16)r27];
        if (r26 != 0) {
            r29 = fn_80203ADC(r26, 1);
            div_b = fn_80203B5C(r26, 1);
            if ((u16)div_b == 0) div_b = 1;
            {
                s32 score = (s32)((u16)r29 * 0x64) / (u16)div_b;
                if (score < r31) r31 = score;
            }
        }
        r27++;
    }
    return r31;
}

/* 0x801F2598 | size: 0xBC | medium */
/* 0x801F2598 | size: 0xBC | medium -- best: 76.17%, arr-init/prologue/peephole walls */
u32 fn_801F2598(u32 param_1, u32 param_2, u32 param_3, u32 param_4) {
    extern u32 fn_801F61EC(u32 a, u32* arr, u32 b, u32 c);
    extern u32 fn_800E0C54(void);
    u32 arr[8];
    u32 r31;
    u32 i = 0;
    while ((u16)i < 8) {
        arr[i] = 0;
        i++;
    }
    r31 = fn_801F61EC(param_1, arr, param_2, param_3);
    if ((u16)r31 == 0) return 0;
    {
        u32 r4 = (u16)fn_800E0C54();
        r4 = (u16)((u16)r4 % (u16)r31);
        if ((u16)r4 >= 8) return 0;
        r4 = arr[(u16)r4];
        if (r4 != 0) return r4;
    }
    return 0;
}

/* 0x801F2654 | size: 0x54 | small -- 100% MATCH */
u16 fn_801F2654(u32 param_1, u8 param_2, u32 param_3, u8 param_4) {
    extern u32 fn_801F26A8(u32, u32, u32*);
    extern u16 fn_801F37B0(u32 ctx, void* fn, u32* data, u32 d);
    u32 data[4];
    data[0] = param_3;
    data[1] = 0;
    data[2] = param_2;
    data[3] = param_4;
    fn_801F37B0(param_1, (void*)fn_801F26A8, data, 0);
    return (u16)data[1];
}

/* 0x801F26A8 | size: 0x12C | medium */
/* 0x801F26A8 | size: 0x12C | medium -- best: 91.60%, reg-alloc wall */
u32 fn_801F26A8(u32 ctx, u32 param_2, u32* data) {
    extern u32 fn_802062FC(void);
    extern u32 fn_801F025C(u32, u32);
    u32 r31 = ctx;
    u32 r29;
    u32 r30 = data[0];
    u32* r28 = data;
    u32 r3;
    u32 r0;
    if ((u8)r28[3] == 1) {
        if ((u8)fn_802062FC() == 1) {
            return 1;
        }
    }
    if (r30 == 0) {
        r29 = 0;
    } else {
        r0 = r28[2];
        if ((s32)r0 == 1) {
            r29 = fn_801F025C(2, r30);
        } else if ((s32)r0 == 2) {
            r29 = fn_801F025C(3, r30);
        } else {
            r29 = 0;
        }
    }
    r3 = fn_801F025C(2, r31);
    r0 = r28[2];
    if ((s32)r0 == 1 || (s32)r0 == 2) {
        if (r29 == 0) {
            return 1;
        }
    }
    if ((s32)r0 == 0) {
        if (r30 == 0 || r30 != r31) {
            goto _incr;
        }
        return 1;
    }
    if ((s32)r0 == 1) {
        if (r29 == r3) goto _incr;
        return 1;
    }
    if ((s32)r0 == 2) {
        if (r29 == r3) goto _incr;
    }
    return 1;
    _incr:
    r28[1] = r28[1] + 1;
    return 1;
}

/* 0x801F27D4 | size: 0x30 | small */
void fn_801F27D4(u32 param_1) {
    extern void fn_801F37B0(u32, void*, u32, u32);
    extern u32 fn_801F2804(u32);

    fn_801F37B0(param_1, (void*)fn_801F2804, 0, 0);
}

/* 0x801F2804 | size: 0x34 | small */
u32 fn_801F2804(u32 param_1) {
    extern void fn_8012640C(u32, u32, u32, u32);
    extern void fn_80209FAC(void);

    fn_8012640C(param_1, 0, 0xd9, 0);
    fn_80209FAC();
    return 1;
}

/* 0x801F2838 | size: 0x54 | small */
void fn_801F2838(u32 param_1, u32 param_2, u32 param_3) {
    extern u16 fn_80119ED0(u32 battle_mon);
    extern void fn_8011AB50(u32 a, u32 b, u32 c);
    if ((u16)fn_80119ED0(param_2) == 0x9) {
        fn_8011AB50(param_1, param_2, param_3);
    }
}

/* 0x801F288C | size: 0x54 | small */
u32 fn_801F288C(u32 param_1, u32 param_2) {
    extern u16 fn_80119ED0(u32 battle_mon);
    extern u32 fn_8011ACB4(u32 a, u32 b);
    if ((u16)fn_80119ED0(param_2) != 0x9) {
        return (u32)-1;
    }
    return fn_8011ACB4(param_1, param_2);
}

/* 0x801F28E0 | size: 0x54 | small */
u32 fn_801F28E0(u32 param_1, u32 param_2) {
    extern u16 fn_80119ED0(u32 battle_mon);
    extern u32 fn_8011AE40(u32 a, u32 b);
    if ((u16)fn_80119ED0(param_2) != 0x9) {
        return (u32)-1;
    }
    return fn_8011AE40(param_1, param_2);
}

/* 0x801F2934 | size: 0x54 | small */
void fn_801F2934(u32 param_1, u32 param_2, u32 param_3) {
    extern u16 fn_80119ED0(u32 battle_mon);
    extern void fn_8011B2C0(u32 a, u32 b, u32 c);
    if ((u16)fn_80119ED0(param_2) == 0x9) {
        fn_8011B2C0(param_1, param_2, param_3);
    }
}

/* 0x801F2988 | size: 0x54 | small */
u32 fn_801F2988(u32 param_1, u32 param_2) {
    extern u16 fn_80119ED0(u32 battle_mon);
    extern u32 fn_8011B444(u32 a, u32 b);
    if ((u16)fn_80119ED0(param_2) != 0x9) {
        return 0;
    }
    return fn_8011B444(param_1, param_2);
}

/* 0x801F29DC | size: 0x54 | small */
u32 fn_801F29DC(u32 param_1, u32 param_2) {
    extern u16 fn_80119ED0(u32 battle_mon);
    extern u32 fn_8011B67C(u32 a, u32 b);
    if ((u16)fn_80119ED0(param_2) != 0x9) {
        return 0;
    }
    return fn_8011B67C(param_1, param_2);
}

/* 0x801F2A30 | size: 0x4C | small */
void fn_801F2A30(u32 param_1, u32 param_2) {
    extern u16 fn_80119ED0(u32 battle_mon);
    extern void fn_8011B788(u32 a, u32 b);
    if ((u16)fn_80119ED0(param_2) == 0x9) {
        fn_8011B788(param_1, param_2);
    }
}

/* 0x801F2A7C | size: 0xE0 | medium */
/* 0x801F2A7C | size: 0xE0 | medium -- best: 98.84%, reg r28/r31 swap wall */
u32 fn_801F2A7C(u32 ctx) {
    extern u32 fn_801F54A4(u32 poke, u32 b, u32 field, u32 d);
    extern u32 fn_801FB8F8(u32 mon);
    extern u32 fn_801F7258(u32, u32);
    u32 pokemon = ctx;
    u32 partyMon;
    u32 moveIndex;
    u32 partyIndex;
    u16 moveCount;
    u32 moveMon;

    fn_801F54A4(pokemon, 0, 0x14, 0);
    moveCount = (u16)fn_801F54A4(pokemon, 0, 0x16, 0);
    partyIndex = 0;
    while ((u16)partyIndex < 2) {
        partyMon = fn_801F54A4(pokemon, 0, 0x35, partyIndex);
        if ((u8)fn_801F7404(partyMon) == 0) {
            partyMon = 0;
        }
        if (partyMon != 0) {
            moveIndex = 0;
            while ((u16)moveIndex < moveCount) {
                moveMon = fn_801F7258(partyMon, moveIndex);
                if (moveMon != 0) {
                    if ((u8)fn_801FB8F8(moveMon) == 1) {
                        return moveMon;
                    }
                }
                moveIndex++;
            }
        }
        partyIndex++;
    }
    return 0;
}

/* 0x801F2B5C | size: 0x3E0 | large -- variant 3 */
void fn_801F2B5C(u32 param_1, void (*param_2)(u32, u32, u32), u32 param_3, u8 param_4) {
    extern void _threadSwitch(void);
    extern u32 fn_800F04BC(u32 task);
    extern void fn_800F0494(u32 task);
    extern u32 fn_800FF560(void);
    extern u32 GSthreadCreate(u32 type, u32 data, u32 flags, u32 unk1, u32 unk2, u32 unk3, u32 mon_out);
    extern void fn_800F0654(u32 task, u32 b, u32 mon, u32 count, u32 ctx);
    extern u32 fn_801F54A4(u32 poke, u32 b, u32 field, u32 d);
    extern u32 fn_801FB1C0(u32 mon, u32 b, u32 field, u32 d);
    extern u32 fn_801F7258(u32, u32);
    u32 arr[4];
    u32 r31 = param_1;
    void (*r30)(u32, u32, u32) = param_2;
    u32 r29 = param_3;
    u32 r28;
    u32 r27;
    u32 r26;
    u32 r25;
    u32 r24;
    u32 r23;
    u32 r22;
    u32 r21;
    u32 r20;
    r28 = (u16)fn_801F54A4(0, 0, 0x14, 0);
    r27 = (u16)fn_801F54A4(0, 0, 0x16, 0);
    { u32 i = 0; while ((u16)i < 4) { arr[i] = 0; i++; } }
    r20 = (u8)param_4;
    if (r20 == 1) {
        r25 = r28;
        r22 = 0;
        r24 = 0;
        while ((u16)r24 < 2) {
            r26 = fn_801F54A4(r31, 0, 0x35, r24);
            if ((u8)fn_801F7404(r26) != 0) {
                r26 = 0;
            }
            if (r26 != 0) {
                r23 = 0;
                while ((u16)r23 < r27) {
                    r21 = fn_801F7258(r26, r23);
                    if (r21 != 0) {
                        r20 = (u16)fn_801FB1C0(r21, 0, 0x43, 0);
                        r20 = (u16)fn_801FB1C0(0, r20, 0x2, 0);
                        fn_801FB1C0(r21, 0, 0x4b, 0);
                        if (r20 == 0) {
                            if ((u16)r22 < 4) {
                                { u32 task = fn_800FF560(); arr[r22] = GSthreadCreate(0x12, task, 0x2000, 1, 0, (u32)param_2, r21); }
                                if (arr[r22] != 0) {
                                    fn_800F0654(arr[r22], 3, r21, r25, r29);
                                    r22++;
                                }
                            }
                        }
                    }
                    r23++;
                }
            }
            r24++;
        }
        do {
            _threadSwitch();
            r22 = 0;
            while ((u16)r22 < 4) {
                if (arr[r22] != 0) {
                    if ((u8)fn_800F04BC(arr[r22]) == 1) break;
                }
                r22++;
            }
        } while ((u16)r22 < 4);
        r24 = 0;
        r21 = 0;
        while ((u16)r24 < 4) {
            r22 = (u16)r24 * 4;
            if (arr[r24] != 0) {
                fn_800F0494(arr[r24]);
                arr[r24] = r21;
            }
            r24++;
        }
    } else {
        r22 = 0;
        while ((u16)r22 < 2) {
            r21 = fn_801F54A4(r31, 0, 0x35, r22);
            if ((u8)fn_801F7404(r21) == 4) {
                r21 = 0;
            }
            if (r21 != 0) {
                r23 = 0;
                while ((u16)r23 < r27) {
                    r24 = fn_801F7258(r21, r23);
                    if (r24 != 0) {
                        r25 = (u16)fn_801FB1C0(r24, 0, 0x43, 0);
                        r25 = (u16)fn_801FB1C0(0, r25, 0x2, 0);
                        fn_801FB1C0(r24, 0, 0x4b, 0);
                        if (r25 == 0) {
                            ((void(*)(u32, u32, u32))r30)(r24, r28, r29);
                        }
                    }
                    r23++;
                }
            }
            r22++;
        }
    }
    r22 = 0;
    while ((u16)r22 < 2) {
        r21 = fn_801F54A4(r31, 0, 0x35, r22);
        if ((u8)fn_801F7404(r21) == 2) {
            r21 = 0;
        }
        if (r21 != 0) {
            r23 = 0;
            while ((u16)r23 < r27) {
                r24 = fn_801F7258(r21, r23);
                if (r24 != 0) {
                    r25 = (u16)fn_801FB1C0(r24, 0, 0x43, 0);
                    r25 = (u16)fn_801FB1C0(0, r25, 0x2, 0);
                    fn_801FB1C0(r24, 0, 0x4b, 0);
                    if (r25 != 0) {
                        ((void(*)(u32, u32, u32))r30)(r24, r28, r29);
                    }
                }
                r23++;
            }
        }
        r22++;
    }
}

/* 0x801F2F3C | size: 0x138 | medium */
void fn_801F2F3C(void *param_1) {
    extern u32 fn_801F54A4(void*, u32, u32, u32);
    extern void *fn_801F7258(void*, u32);
    extern u32 fn_801F7404(void*);
    extern void *fn_801F981C(void*, u32);
    extern void *fn_801F02AC(u32, void*, u32);
    extern void fn_80202C1C(void*, void*);
    extern u32 fn_802062FC(void*);
    void *pkmn;
    u32 count, cols, rows, i;
    void *team;
    u32 j;
    void *row_obj;
    u32 k;
    void *cell;

    pkmn = param_1;
    count = fn_801F54A4(pkmn, 0, 0x14, 0) & 0xFFFF;
    cols  = fn_801F54A4(pkmn, 0, 0x16, 0) & 0xFFFF;
    rows  = fn_801F54A4(pkmn, 0, 0x18, 0) & 0xFFFF;
    i = 0;
    while ((i & 0xFFFF) < 2u) {
        team = (void*)fn_801F54A4(pkmn, 0, 0x35, i);
        if (!(fn_801F7404(team) & 0xFF))
            team = NULL;
        if (team != NULL) {
            j = 0;
            while ((j & 0xFFFF) < cols) {
                row_obj = fn_801F7258(team, j);
                if (row_obj != NULL) {
                    k = 0;
                    while ((k & 0xFFFF) < rows) {
                        cell = fn_801F981C(row_obj, k);
                        if (cell != NULL) {
                            if (fn_802062FC(cell) & 0xFF) {
                                void *slot = fn_801F02AC(3, cell, count);
                                fn_80202C1C(cell, slot);
                            }
                        }
                        k++;
                    }
                }
                j++;
            }
        }
        i++;
    }
}

/* 0x801F3074 | size: 0x104 | medium */
void fn_801F3074(void *param_1) {
    extern u32 fn_801F54A4(void*, u32, u32, u32);
    extern void *fn_801F7258(void*, u32);
    extern u32 fn_801F7404(void*);
    extern void *fn_801F02AC(u32, void*, u32);
    extern void fn_801F6B54(void*, u32, u32, u32);
    void *pkmn;
    u32 count, cols, rows, i;
    void *team;
    u32 j;
    void *row_obj;

    pkmn = param_1;
    count = fn_801F54A4(pkmn, 0, 0x14, 0) & 0xFFFF;
    cols  = fn_801F54A4(pkmn, 0, 0x16, 0) & 0xFFFF;
    rows  = fn_801F54A4(pkmn, 0, 0x18, 0) & 0xFFFF;
    i = 0;
    while ((i & 0xFFFF) < 2u) {
        team = (void*)fn_801F54A4(pkmn, 0, 0x35, i);
        if (!(fn_801F7404(team) & 0xFF))
            team = NULL;
        if (team != NULL) {
            j = 0;
            while ((j & 0xFFFF) < cols) {
                row_obj = fn_801F7258(team, j);
                if (row_obj != NULL) {
                    fn_801F02AC(3, row_obj, count);
                    fn_801F6B54(row_obj, count, cols, rows);
                }
                j++;
            }
        }
        i++;
    }
}

/* 0x801F3178 | size: 0x138 | medium */
void fn_801F3178(void *param_1) {
    extern u32 fn_801F54A4(void*, u32, u32, u32);
    extern void *fn_801F7258(void*, u32);
    extern u32 fn_801F7404(void*);
    extern void *fn_801F981C(void*, u32);
    extern void *fn_801F02AC(u32, void*, u32);
    extern void fn_80205274(void*, void*);
    extern u32 fn_802062FC(void*);
    void *pkmn;
    u32 count, cols, rows, i;
    void *team;
    u32 j;
    void *row_obj;
    u32 k;
    void *cell;

    pkmn = param_1;
    count = fn_801F54A4(pkmn, 0, 0x14, 0) & 0xFFFF;
    cols  = fn_801F54A4(pkmn, 0, 0x16, 0) & 0xFFFF;
    rows  = fn_801F54A4(pkmn, 0, 0x18, 0) & 0xFFFF;
    i = 0;
    while ((i & 0xFFFF) < 2u) {
        team = (void*)fn_801F54A4(pkmn, 0, 0x35, i);
        if (!(fn_801F7404(team) & 0xFF))
            team = NULL;
        if (team != NULL) {
            j = 0;
            while ((j & 0xFFFF) < cols) {
                row_obj = fn_801F7258(team, j);
                if (row_obj != NULL) {
                    k = 0;
                    while ((k & 0xFFFF) < rows) {
                        cell = fn_801F981C(row_obj, k);
                        if (cell != NULL) {
                            if (fn_802062FC(cell) & 0xFF) {
                                void *slot = fn_801F02AC(3, cell, count);
                                fn_80205274(cell, slot);
                            }
                        }
                        k++;
                    }
                }
                j++;
            }
        }
        i++;
    }
}

/* 0x801F32B0 | size: 0x3C | small */
u8 fn_801F32B0(u32 param_1) {
    extern void fn_801F2B5C(u32, void*, void*, u32);
    extern void fn_801F32EC(void);
    u8 local;

    local = 0;
    fn_801F2B5C(param_1, (void*)fn_801F32EC, &local, 0);
    return local;
}

/* 0x801F32EC | size: 0xFC | medium */
u32 fn_801F32EC(void *param_1, u32 r4_unused, u8 *out_buf) {
    extern u32 fn_801FA634(void*);
    extern void *fn_801F8A18(void*, u16*);
    extern u32 fn_801F54A4(void*, u32, u32, u32);
    extern void *fn_801FB1C0(void*, u32, u32, u32);
    extern u32 fn_802062FC(void*);
    extern u32 fn_801FB8F8(void*);
    void *pkmn;
    u32 count, i;
    u8 *outptr;
    u16 tmp;

    pkmn = param_1;
    if (!(fn_801FA634(pkmn) & 0xFF))
        return 1;
    tmp = 0;
    if (!fn_801F8A18(pkmn, &tmp))
        return 1;
    count = fn_801F54A4(0, 0, 0x18, 0) & 0xFFFF;
    outptr = out_buf;
    i = 0;
    while ((i & 0xFFFF) < count) {
        void *entry = fn_801FB1C0(pkmn, 0, 0x46, i);
        if (entry != NULL) {
            if (fn_802062FC(entry) & 0xFF) {
                /* nonzero: skip */
            } else if (outptr != NULL) {
                if (*outptr != 2) {
                    if (fn_801FB8F8(pkmn) & 0xFF) {
                        *outptr = 2;
                    } else {
                        *outptr = 1;
                    }
                }
            }
        }
        i++;
    }
    return 1;
}

/* 0x801F33E8 | size: 0x48 | small */
u16 fn_801F33E8(void *param_1, u16 param_2) {
    extern u32 fn_801F37B0(void*, void*, void*, u8);
    extern u32 fn_801F3430(void*, u32, void*);
    u32 buf[2];

    buf[0] = param_2;
    buf[1] = 0;
    fn_801F37B0(param_1, (void*)fn_801F3430, buf, 0);
    return (u16)buf[1];
}

/* 0x801F3430 | size: 0x6C | small */
u32 fn_801F3430(void *param_1, u32 r4_unused, u32 *buf) {
    extern u32 fn_802062FC(void*);
    extern u32 fn_802026E4(void*, u16);

    if (!(fn_802062FC(param_1) & 0xFF))
        return 1;
    if ((fn_802026E4(param_1, (u16)buf[0]) & 0xFF) == 1)
        buf[1]++;
    return 1;
}

/* 0x801F349C | size: 0x50 | small */
void *fn_801F349C(void *param_1, u16 param_2, void *param_3, u8 param_4, u32 param_5) {
    extern u32 fn_801F37B0(void*, void*, void*, void*);
    extern void _fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv(void*, u32, void*);
    u32 buf[4];

    buf[0] = param_2;
    buf[1] = 0;
    buf[2] = param_4;
    buf[3] = param_5;
    fn_801F37B0(param_1, (void*)_fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv, buf, param_3);
    return (void*)buf[1];
}

/* 0x801F34EC | size: 0x138 | medium */
u32 _fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv(void* obj, u32 slot, void* data) {
    extern u32 fn_802062FC();
    extern void* fn_801F025C(u32, void*);
    extern u16 fn_80207BF4(void*);
    struct { u32 tokuseiId; void* result; s32 side; void* pokemon; }* s = data;
    void* pokemon;
    void* sideRef;
    void* objRef;

    pokemon = s->pokemon;
    if ((u8)fn_802062FC() == 0)
        return 1;
    if (pokemon == 0) {
        sideRef = 0;
    } else if (s->side == 1) {
        sideRef = fn_801F025C(2, pokemon);
    } else if (s->side == 2) {
        sideRef = fn_801F025C(3, pokemon);
    } else {
        sideRef = 0;
    }
    objRef = fn_801F025C(2, obj);
    if (s->side == 1 || s->side == 2) {
        if (sideRef == 0)
            return 1;
    }
    if (s->side == 0) {
        if (pokemon == 0) goto _checkId;
        if (pokemon != obj) goto _checkId;
        return 1;
    } else if (s->side == 1 || s->side == 2) {
        if (sideRef == objRef) goto _checkId;
        return 1;
    } else {
        return 1;
    }
_checkId:
    if ((u16)s->tokuseiId == (u16)fn_80207BF4(obj)) {
        s->result = obj;
        return 0;
    }
    return 1;
}

/* 0x801F3624 | size: 0x54 | small */
u16 fn_801F3624(void *param_1, u16 param_2, u8 param_3, u32 param_4) {
    extern u32 fn_801F37B0(void*, void*, void*, u32);
    extern void _fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv(void*, u32, void*);
    u32 buf[4];

    buf[0] = param_2;
    buf[1] = 0;
    buf[2] = param_3;
    buf[3] = param_4;
    fn_801F37B0(param_1, (void*)_fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv, buf, 0);
    return (u16)buf[1];
}

/* 0x801F3678 | size: 0x138 | medium */
u32 _fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv(void* obj, u32 slot, void* data) {
    extern u32 fn_802062FC();
    extern void* fn_801F025C(u32, void*);
    extern u16 fn_80207BF4(void*);
    struct { u32 tokuseiId; u32 count; s32 side; void* pokemon; }* s = data;
    void* pokemon;
    void* sideRef;
    void* objRef;

    pokemon = s->pokemon;
    if ((u8)fn_802062FC() == 0)
        return 1;
    if (pokemon == 0) {
        sideRef = 0;
    } else if (s->side == 1) {
        sideRef = fn_801F025C(2, pokemon);
    } else if (s->side == 2) {
        sideRef = fn_801F025C(3, pokemon);
    } else {
        sideRef = 0;
    }
    objRef = fn_801F025C(2, obj);
    if (s->side == 1 || s->side == 2) {
        if (sideRef == 0)
            return 1;
    }
    if (s->side == 0) {
        if (pokemon == 0) goto _checkId;
        if (pokemon != obj) goto _checkId;
        return 1;
    } else if (s->side == 1 || s->side == 2) {
        if (sideRef == objRef) goto _checkId;
        return 1;
    } else {
        return 1;
    }
_checkId:
    if ((u16)s->tokuseiId == (u16)fn_80207BF4(obj)) {
        s->count = s->count + 1;
    }
    return 1;
}

/* 0x801F37B0 | size: 0x1D4 | medium */
u8 fn_801F37B0(void *param_1, void *callback, void *buf, u8 flag) {
    extern u32 fn_801F54A4(void*, u32, u32, u32);
    extern void *fn_801F7258(void*, u32);
    extern u32 fn_801F7404(void*);
    extern void *fn_801F981C(void*, u32);
    extern u32 fn_80206780(void*);
    typedef u8 (*cbk_t)(void*, u32, void*);
    void *cbk;
    void *cbuf;
    u8 fl;
    void *pkmn;
    u32 ret;
    u32 count, cols, rows;
    u32 i, j, k;
    void *team, *row_obj;

    cbk = callback;
    cbuf = buf;
    fl = flag;
    pkmn = param_1;
    ret = 1;
    count = fn_801F54A4(pkmn, 0, 0x14, 0) & 0xFFFF;
    cols  = fn_801F54A4(pkmn, 0, 0x16, 0) & 0xFFFF;
    rows  = fn_801F54A4(pkmn, 0, 0x18, 0) & 0xFFFF;
    if ((fl & 0xFF) == 1u) {
        i = 0;
        while ((i & 0xFFFF) < 8u) {
            team = (void*)fn_801F54A4(pkmn, 0, 0x59, i);
            if (team != NULL) {
                if (fn_80206780(team) & 0xFF) {
                    if (!(((cbk_t)cbk)(team, count, cbuf) & 0xFF)) {
                        ret = 0;
                        return ret;
                    }
                }
            }
            i++;
        }
        return ret;
    }
    k = 0;
    while ((k & 0xFFFF) < rows) {
        j = 0;
        while ((j & 0xFFFF) < cols) {
            i = 0;
            while ((i & 0xFFFF) < 2u) {
                team = (void*)fn_801F54A4(pkmn, 0, 0x35, i);
                if (!(fn_801F7404(team) & 0xFF))
                    team = NULL;
                row_obj = NULL;
                if (team != NULL) {
                    row_obj = fn_801F7258(team, j);
                    if (row_obj == NULL)
                        row_obj = NULL;
                    else {
                        row_obj = fn_801F981C(row_obj, k);
                        if (row_obj == NULL)
                            row_obj = NULL;
                    }
                }
                if (row_obj != NULL) {
                    if (!(((cbk_t)cbk)(row_obj, count, cbuf) & 0xFF)) {
                        ret = 0;
                        return ret;
                    }
                }
                i++;
            }
            j++;
        }
        k++;
    }
    return ret;
}

/* 0x801F3984 | size: 0x1A0 | medium */
u8 fn_801F3984(void *param_1, u32 param_2) {
    extern u32 fn_801EF634(void*);
    extern void fn_801EF62C(u32);
    extern u32 fn_801F54A4(void*, u32, u32, u32);
    void *pkmn;
    u32 curval;
    u32 newval;
    u32 battletype;

    pkmn = param_1;
    curval = param_2;
    newval = fn_801EF634(pkmn);
    battletype = fn_801F54A4(pkmn, 0, 0x23, 0) & 0xFF;
    if ((newval & 0xFFFF) == (curval & 0xFFFF))
        return 0;
    if ((newval & 0xFFFF) == 1u)
        return 1;
    if ((curval & 0xFFFF) == 0u || (curval & 0xFFFF) == 1u) {
        fn_801EF62C(curval);
        return 1;
    }
    if (battletype == 0u) {
        if ((newval & 0xFFFF) == 3u || (newval & 0xFFFF) == 5u)
            return 0;
        if ((curval & 0xFFFF) == 7u) {
            fn_801EF62C(3);
            return 1;
        }
        if ((curval & 0xFFFF) == 6u) {
            fn_801EF62C(5);
            return 1;
        }
    }
    if ((newval & 0xFFFF) == 7u || (newval & 0xFFFF) == 6u)
        return 0;
    if ((newval & 0xFFFF) == 3u && (curval & 0xFFFF) == 2u) {
        fn_801EF62C(7);
        return 1;
    }
    if ((newval & 0xFFFF) == 2u && (curval & 0xFFFF) == 3u) {
        fn_801EF62C(7);
        return 1;
    }
    if ((newval & 0xFFFF) == 4u && (curval & 0xFFFF) == 5u) {
        fn_801EF62C(6);
        return 1;
    }
    if ((newval & 0xFFFF) == 5u && (curval & 0xFFFF) == 4u) {
        fn_801EF62C(6);
        return 1;
    }
    fn_801EF62C(curval);
    return 1;
}

/* 0x801F3B24 | size: 0x90 | medium */
void fn_801F3B24(void* obj, u32 param2) {
    extern u32 fn_801F54A4(void*, int, int, int);
    extern void fn_801F3BB4(void*, u32*, u16, u32);
    extern void fn_801F4C14(void*, int, int, int, u32*);
    u32 buf[8];
    u32 i;
    i = 0;
    while ((i & 0xFFFF) < 8) {
        *(u32*)((u8*)buf + ((u16)i << 2)) = fn_801F54A4(obj, 0, 0x59, (int)i);
        i++;
    }
    fn_801F3BB4(obj, buf, 8, param2);
    fn_801F4C14(obj, 0, 0x5a, 0, buf);
}

/* 0x801F3BB4 | size: 0x134 | medium */
void fn_801F3BB4(void* obj, u32* arr, u16 count, u32 flag) {
    extern s32 fn_801F3CE8();
    u16 n = count;
    u32 bound = (u32)n - 1;
    u32 i = 0;
    while ((s32)(u16)i < (s32)bound) {
        u32 oi = ((u16)i << 2);
        u32 j = (u16)(i + 1);
        while ((u16)j < n) {
            u32 ai = *(u32*)((u8*)arr + oi);
            if (ai == 0) {
                u32 aj = *(u32*)((u8*)arr + ((u16)j << 2));
                if (aj == 0) { j++; continue; }
            }
            if (ai == 0) {
                u32 oj = ((u16)j << 2);
                u32 av = *(u32*)((u8*)arr + oj);
                *(u32*)((u8*)arr + oi) = av;
                *(u32*)((u8*)arr + oj) = ai;
            } else {
                u32 oj = ((u16)j << 2);
                u32 bj = *(u32*)((u8*)arr + oj);
                if (bj != 0) {
                    s32 ka, kb;
                    if (!(flag & 0xFF)) {
                        ka = 0;
                        kb = 0;
                    } else {
                        ka = fn_802050F4((void*)ai);
                        kb = fn_802050F4((void*)*(u32*)((u8*)arr + oj));
                    }
                    if ((s8)ka < (s8)kb) { j++; continue; }
                    if ((s8)ka > (s8)kb) {
                        u32 tmp = *(u32*)((u8*)arr + oi);
                        u32 tmp2 = *(u32*)((u8*)arr + oj);
                        *(u32*)((u8*)arr + oi) = tmp2;
                        *(u32*)((u8*)arr + oj) = tmp;
                    } else {
                        if (!fn_801F3CE8(obj, *(u32*)((u8*)arr + oi), *(u32*)((u8*)arr + oj), flag)) {
                            u32 tmp = *(u32*)((u8*)arr + oi);
                            u32 tmp2 = *(u32*)((u8*)arr + oj);
                            *(u32*)((u8*)arr + oi) = tmp2;
                            *(u32*)((u8*)arr + oj) = tmp;
                        }
                    }
                }
            }
            j++;
        }
        i++;
    }
}

/* 0x801F3CE8 | size: 0x538 | large */
s32 fn_801F3CE8(void *p1, void *p2, void *p3, u8 p4) {
    extern u32 fn_801F54A4(void*, u32, u32, u32);
    extern u8 fn_801F37B0(void*, void*, void*, u8);
    extern u32 fn_8012640C(void*, u32, u32, u32);
    extern u32 fightSideGetStatus(void*, u32, u32, u32);
    extern u32 fn_801FB1C0(void*, u32, u32, u32);
    extern u32 fn_802043D4(void*, u32, u32, u32, void*);
    extern u32 fn_802051D4(void*);
    extern s32 fn_8011BEB4(u32, u32, u32, u32);
    extern s32 fn_800E0C54(void);
    u32 walkCtx1[4];
    u32 walkCtx2[4];
    u32 abilCat;
    u16 slotCount;
    u8 numBattle;
    void *matchSide;
    void *fightRes;
    u32 i1, j1, k1;
    void *teamObj1;
    u32 cmp1;
    void *matchSide2;
    s32 stat1, stat2;

    /* p2 tested first -> r31; p3 second -> r30 */
    if (p2 == 0 || p3 == 0) {
        return 1;
    }

    /* First walk: callback 0xd on p2 */
    walkCtx1[0] = 0xd;
    walkCtx1[1] = 0;
    walkCtx1[2] = 0;
    walkCtx1[3] = 0;
    fn_801F37B0(p2, (void*)_fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv, walkCtx1, 0);
    if ((u16)walkCtx1[1] != 0) {
        abilCat = 0;
        goto _abilDone;
    }

    /* Second walk: callback 0x4d on p1 */
    walkCtx2[0] = 0x4d;
    walkCtx2[1] = 0;
    walkCtx2[2] = 0;
    walkCtx2[3] = 0;
    fn_801F37B0(p1, (void*)_fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv, walkCtx2, 0);
    if ((u16)walkCtx2[1] != 0) {
        abilCat = 0;
        goto _abilDone;
    }

    /* Determine ability category from p1 fields */
    if ((s32)fn_801F54A4(p1, 0, 0xa, 0x4e) == 1) { abilCat = 0; goto _abilDone; }
    if ((s32)fn_801F54A4(p1, 0, 0xa, 0x4f) == 1) { abilCat = 1; goto _abilDone; }
    if ((s32)fn_801F54A4(p1, 0, 0xa, 0x50) == 1) { abilCat = 2; goto _abilDone; }
    if ((s32)fn_801F54A4(p1, 0, 0xa, 0x51) == 1) { abilCat = 3; goto _abilDone; }
    if ((s32)fn_801F54A4(p1, 0, 0xa, 0x52) == 1) { abilCat = 4; goto _abilDone; }
    if ((s32)fn_801F54A4(p1, 0, 0xa, 0x53) == 1) { abilCat = 1; goto _abilDone; }
    if ((s32)fn_801F54A4(p1, 0, 0xa, 0x54) == 1) { abilCat = 2; goto _abilDone; }
    if ((s32)fn_801F54A4(p1, 0, 0xa, 0x55) == 1) { abilCat = 3; goto _abilDone; }
    abilCat = 0;
    _abilDone:

    numBattle = (u8)fn_801F54A4(p1, 0, 0x1d, 0);
    slotCount = (u16)fn_801F54A4(p1, 0, 0x5b, 0);

    /* First triple loop: find fight side containing pkm2's matching pokemon */
    fightRes = (void*)fn_8012640C(p2, 0, 0xd5, 0);
    if (fightRes == 0) {
        matchSide = 0;
        goto _cmp1;
    }
    i1 = 0;
    while ((u16)i1 < 3) {
        teamObj1 = (void*)fn_801F54A4(p1, 0, 0x35, i1);
        if (teamObj1 != 0) {
            j1 = 0;
            while ((u16)j1 < 3) {
                matchSide = (void*)fightSideGetStatus(teamObj1, 0, 7, j1);
                if (matchSide != 0) {
                    k1 = 0;
                    while ((u16)k1 < 7) {
                        void *pkm;
                        pkm = (void*)fn_801FB1C0(matchSide, 0, 0x45, k1);
                        if (pkm != 0 && pkm == fightRes) {
                            goto _foundSide1;
                        }
                        k1++;
                    }
                }
                j1++;
            }
        }
        i1++;
    }
    matchSide = 0;
    _foundSide1:
    if (matchSide != 0) goto _cmp1;
    matchSide = 0;
    _cmp1:
    {
        void *abilPkm;
        if (matchSide != 0) {
            abilPkm = (void*)fn_801FB1C0(matchSide, 0, 0x44, 0);
        } else {
            abilPkm = 0;
        }
        cmp1 = fn_802043D4(p2, numBattle, abilCat, slotCount, abilPkm);
    }

    /* Second triple loop: find fight side containing pkm3's matching pokemon */
    {
        u32 i2, j2, k2;
        void *teamObj2;
        fightRes = (void*)fn_8012640C(p3, 0, 0xd5, 0);
        if (fightRes == 0) {
            matchSide2 = 0;
            goto _cmp2;
        }
        i2 = 0;
        while ((u16)i2 < 3) {
            teamObj2 = (void*)fn_801F54A4(p1, 0, 0x35, i2);
            if (teamObj2 != 0) {
                j2 = 0;
                while ((u16)j2 < 3) {
                    matchSide2 = (void*)fightSideGetStatus(teamObj2, 0, 7, j2);
                    if (matchSide2 != 0) {
                        k2 = 0;
                        while ((u16)k2 < 7) {
                            void *pkm;
                            pkm = (void*)fn_801FB1C0(matchSide2, 0, 0x45, k2);
                            if (pkm != 0 && pkm == fightRes) {
                                goto _foundSide2;
                            }
                            k2++;
                        }
                    }
                    j2++;
                }
            }
            i2++;
        }
        matchSide2 = 0;
        _foundSide2:
        if (matchSide2 != 0) goto _cmp2;
        matchSide2 = 0;
        _cmp2:
        {
            u32 cmp2;
            void *abilPkm2;
            if (matchSide2 != 0) {
                abilPkm2 = (void*)fn_801FB1C0(matchSide2, 0, 0x44, 0);
            } else {
                abilPkm2 = 0;
            }
            cmp2 = fn_802043D4(p3, numBattle, abilCat, slotCount, abilPkm2);

            if ((u8)p4 != 0) {
                stat1 = (s32)fn_802051D4(p2);
                stat2 = (s32)fn_802051D4(p3);
            } else {
                stat1 = 0;
                stat2 = 0;
            }

            stat1 = fn_8011BEB4(0, stat1, 4, 0);
            stat2 = fn_8011BEB4(0, stat2, 4, 0);
            if ((s8)stat1 != 0 || (s8)stat2 != 0) {
                if (stat1 > stat2) return 1;
                if (stat1 < stat2) return 0;
            }
            if (cmp1 > cmp2) return 1;
            if (cmp1 < cmp2) return 0;
            if (fn_800E0C54() & 1) return 1;
            return 0;
        }
    }
}

/* 0x801F4220 | size: 0x134 | medium */
void* fn_801F4220(void* obj, void* search_val) {
    extern u32 fn_8012640C(void*, int, int, int);
    extern u32 fn_801F54A4(void*, int, int, int);
    extern u32 fightSideGetStatus(void*, int, int, int);
    extern u32 fn_801FB1C0(void*, int, int, int);
    u32 target, side, i, k, j, team, val;
    target = fn_8012640C(search_val, 0, 0xd5, 0);
    if (!target) {
        side = 0;
        goto check;
    }
    side = 0; i = 0; k = 0; j = 0; team = 0;
    while ((i & 0xFFFF) < 2) {
        team = fn_801F54A4(obj, 0, 0x35, (int)i);
        if (team) {
            j = 0;
            while ((j & 0xFFFF) < 2) {
                side = fightSideGetStatus((void*)team, 0, 7, (int)j);
                if (side) {
                    k = 0;
                    while ((k & 0xFFFF) < 6) {
                        val = fn_801FB1C0((void*)side, 0, 0x45, (int)k);
                        if (val != 0 && target == val)
                            goto found;
                        k++;
                    }
                }
                j++;
            }
        }
        i++;
    }
    side = 0;
found:
    if (!side)
        side = 0;
check:
    if (!side)
        return 0;
    {
        u32 r = fn_801FB1C0((void*)side, 0, 0x44, 0);
        if (r == 0) r = 0;
        return (void*)r;
    }
}

/* 0x801F4354 | size: 0x10C | medium */
void* fn_801F4354(void* obj, void* search_val) {
    extern u32 fn_8012640C(void*, int, int, int);
    extern u32 fn_801F54A4(void*, int, int, int);
    extern u32 fightSideGetStatus(void*, int, int, int);
    extern u32 fn_801FB1C0(void*, int, int, int);
    u32 target, side, i, k, j, team, val;
    target = fn_8012640C(search_val, 0, 0xd5, 0);
    if (!target)
        return 0;
    side = 0; i = 0; k = 0; j = 0; team = 0;
    while ((i & 0xFFFF) < 2) {
        team = fn_801F54A4(obj, 0, 0x35, (int)i);
        if (team) {
            j = 0;
            while ((j & 0xFFFF) < 2) {
                side = fightSideGetStatus((void*)team, 0, 7, (int)j);
                if (side) {
                    k = 0;
                    while ((k & 0xFFFF) < 6) {
                        val = fn_801FB1C0((void*)side, 0, 0x45, (int)k);
                        if (val != 0 && target == val)
                            goto done;
                        k++;
                    }
                }
                j++;
            }
        }
        i++;
    }
    side = 0;
done:
    if (!side)
        return 0;
    return (void*)side;
}

/* 0x801F4460 | size: 0xDC | medium */
void* fn_801F4460(void* obj, void* search_val) {
    extern u32 fn_801F54A4(void*, int, int, int);
    extern u32 fightSideGetStatus(void*, int, int, int);
    extern u32 fn_801FB1C0(void*, int, int, int);
    u32 team, j, k, i, side, val;
    team = 0; j = 0; k = 0; i = 0;
    while ((i & 0xFFFF) < 2) {
        team = fn_801F54A4(obj, 0, 0x35, (int)i);
        if (team) {
            j = 0;
            while ((j & 0xFFFF) < 2) {
                side = fightSideGetStatus((void*)team, 0, 7, (int)j);
                if (side) {
                    k = 0;
                    while ((k & 0xFFFF) < 6) {
                        val = fn_801FB1C0((void*)side, 0, 0x45, (int)k);
                        if (val != 0 && search_val == (void*)val)
                            return (void*)side;
                        k++;
                    }
                }
                j++;
            }
        }
        i++;
    }
    return 0;
}

/* 0x801F4718 | size: 0x9C | medium */
void* fn_801F4718(void* obj) {
    extern u32 fn_801F54A4(void*, int, int, int);
    extern void fn_801F61EC(void*, void*, int, int, int);
    u32* tmp;
    u32 subfield;
    u32 i, fill;
    tmp = (u32*)fn_801F54A4(obj, 0, 0x5a, 0);
    if (tmp) {
        fill = 0; i = fill;
        while ((i & 0xFFFF) < 8) {
            *(u32*)((u8*)tmp + ((u16)i << 2)) = fill;
            i++;
        }
    }
    subfield = fn_801F54A4(obj, 0, 0x5a, 0);
    if (subfield == 0)
        return 0;
    fn_801F61EC(obj, (void*)subfield, 0, 0, 0);
}

/* 0x801F47B4 | size: 0x50 | small */
void* fn_801F47B4(void* a, u32 b) {
    extern u32 fn_801F54A4(void*, int, int, u32);
    void *ret;
    u32 result = fn_801F54A4(a, 0, 0x35, b);
    if ((u8)fn_801F7404(result) != 0)
        goto ret_result;
    ret = 0;
    goto end;
ret_result:
    ret = (void*)result;
end:
    return ret;
}

/* 0x801F4804 | size: 0x5C | small */
s16 fn_801F4804(void* obj) {
    extern u32 fn_801F54A4(void*, int, int, int);
    extern void fn_801F4C14(void*, int, int, int, s16);
    s16 val = (s16)fn_801F54A4(obj, 0, 0x58, 0);
    fn_801F4C14(obj, 0, 0x58, 0, (s16)(val + 1));
    return val;
}

/* 0x801F4860 | size: 0x260 | large */
void fn_801F4860(void* obj, u32 param2) {
    extern u32 fn_801F54A4(void*, int, int, int);
    extern void fn_8011B950(u32, u32);
    extern void fn_801F4C14(void*, int, int, int, u16);
    extern void fn_801F7530(u32, u32);
    u32 tmp;
    u32 fill;
    u32* zarr;
    CopyBuf buf;
    u16* tbl;
    u32 i;
    if (!obj) return;
    tmp = fn_801F54A4(obj, 0, 9, 0);
    fn_8011B950(tmp, 1);
    fn_801F4C14(obj, 0, 0xc, 0, 0);
    fn_801F4C14(obj, 0, 0xd, 0, 0);
    tmp = fn_801F54A4(obj, 0, 0x35, 0);
    fn_801F7530(tmp, 2);
    buf = lbl_80279C28;
    tbl = (u16*)&buf;
    i = 0;
    while ((i & 0xFFFF) < 0xd) {
        fn_801F4C14(obj, 0, tbl[(u16)i], 0, 0);
        i++;
    }
    fn_801F4C14(obj, 0, 0x50, 0, 0);
    fn_801F4C14(obj, 0, 0x51, 0, 0);
    fn_801F4C14(obj, 0, 0x52, 0, 0);
    fn_801F4C14(obj, 0, 0x53, 0, 0);
    fn_801F4C14(obj, 0, 0x54, 0, 0);
    fn_801F4C14(obj, 0, 0x55, 0, 0);
    fn_801F4C14(obj, 0, 0x56, 0, 0);
    fn_801F4C14(obj, 0, 0x57, 0, 0);
    fn_801F4C14(obj, 0, 0x58, 0, 0);
    zarr = (u32*)fn_801F54A4(obj, 0, 0x5a, 0);
    if (zarr) {
        fill = 0; i = fill;
        while ((i & 0xFFFF) < 8) {
            *(u32*)((u8*)zarr + ((u16)i << 2)) = fill;
            i++;
        }
    }
    fn_801F4C14(obj, 0, 0x5b, 0, 0);
    fn_801F4C14(obj, 0, 0xd, 0, param2);
    fn_801F4C14(obj, 0, 0xc, 0, 1);
}

/* 0x801F4AC0 | size: 0x154 | medium */
void fn_801F4AC0(void* obj) {
    extern void fn_801F4C14(void*, int, int, int, int);
    CopyBuf buf;
    u16* tbl;
    u32 i;
    buf = lbl_80279C28;
    tbl = (u16*)&buf;
    i = 0;
    while ((i & 0xFFFF) < 0xd) {
        fn_801F4C14(obj, 0, tbl[(u16)i], 0, 0);
        i++;
    }
    fn_801F4C14(obj, 0, 0x50, 0, 0);
    fn_801F4C14(obj, 0, 0x51, 0, 0);
    fn_801F4C14(obj, 0, 0x52, 0, 0);
    fn_801F4C14(obj, 0, 0x53, 0, 0);
    fn_801F4C14(obj, 0, 0x54, 0, 0);
    fn_801F4C14(obj, 0, 0x55, 0, 0);
    fn_801F4C14(obj, 0, 0x56, 0, 0);
    fn_801F4C14(obj, 0, 0x57, 0, 0);
}

/* 0x801F4C14 | size: 0x890 | massive */
u8 fn_801F4C14(u32 p1, u16 p2, u32 p3, u16 p4, u32 p5) {
    extern u32 fn_801F54A4(u32, u32, u32, u32);
    extern u32 fn_801F6738(u16);
    extern u32 fn_801F6B48(void);
    extern void fn_801F65F0(u32, u32);
    extern void fn_801F667C(u32, u16);
    extern void fn_801F6658(u32, u8, u16);
    extern void fn_801F6648(u32, u8);
    extern void fn_801F6638(u32, u32);
    extern void fn_801F6628(u32, u32);
    extern void fn_801F6618(u32, u32);
    extern u32 fn_80119ED0(u16);
    extern u32 fn_8011B444(u32, u16);
    extern void fn_8011B2C0(u32, u16, u16);
    extern void fn_801F68C8(u32, u16);
    extern void fn_801F68B8(u32, u16);
    extern u32 fn_80206780(u32);
    extern void fn_80132A38(u32, u32);
    extern u32 fn_80207BF4(u32);
    extern u32 fn_8011CB6C(u16);
    extern void fn_8011CB54(void);
    extern u32 fn_800FA280(void);
    extern void fn_801F68A4(u32, u32);
    extern u32 fn_801F6A98(u32);
    extern u32 fn_801254B4(u32, u32, u32, u32, u32);
    extern void fn_801F6890(u32, u32);
    extern void fn_801F687C(u32, u32);
    extern void fn_801F6868(u32, u32);
    extern void fn_801F64AC(u32, u32);
    extern void fn_801F647C(u32, u32);
    extern void fn_801F644C(u32, u32);
    extern u32 fn_802040E8(u32);
    extern void fn_801F641C(u32, u32);
    extern void fn_801F640C(u32);
    extern void fn_801F6854(u32, u32);
    extern void fn_801F6840(u32, u32);
    extern void fn_801F682C(u32, u32);
    extern void fn_801F6818(u32, u32);
    extern void fn_801F6804(u32, u32);
    extern void fn_801F67F0(u32, u32);
    extern void fn_801F67DC(u32, u32);
    extern void fn_801F67C8(u32, u32);
    extern void fn_801F67B4(u32, u32);
    extern void fn_801F67A0(u32, u16);
    extern void fn_801F678C(u32, u16);
    extern u32 itemGetStatus(u32, u32, u32, u32);
    extern void fn_801F6778(u32, u16);
    extern void fn_801F6764(u32, u16);
    extern void fn_801F65C0(u32, s16);
    extern void fn_801F6560(u32, u16, u32);
    extern void fn_801F650C(u32, u32);
    extern void fn_801F64DC(u32, u16);
    extern void fn_801F3984(u32, u16);
    extern u32 fn_801F0134(u32, u16);
    u32 ret;
    u32 tmp;

    ret = 0;
    tmp = (u16)fn_801F54A4(p1, 0, 0x14, 0);
    if ((u16)p3 == 0) goto fn_801F4C14_invalid;
    if ((u16)p3 < 0x60) goto fn_801F4C14_range_ok;
    fn_801F4C14_invalid: return 0;
    fn_801F4C14_range_ok:
    if ((u16)p3 < 8) {
        p1 = fn_801F6738(p2);
        if (p1 == 0) { return 0; }
    } else if ((u16)p3 < 0x5f && p1 == 0) {
        p1 = fn_801F6B48();
        if (p1 == 0) { return 0; }
    }
    switch ((u16)p3) {
    case 0x1:
        fn_801F65F0(p1, p5);
        break;
    case 0x2:
        fn_801F667C(p1, (u16)p5);
        break;
    case 0x3:
        fn_801F6658(p1, (u8)p4, (u16)p5);
        break;
    case 0x4:
        fn_801F6648(p1, (u8)p5);
        break;
    case 0x5:
        fn_801F6638(p1, p5);
        break;
    case 0x6:
        fn_801F6628(p1, p5);
        break;
    case 0x7:
        fn_801F6618(p1, p5);
        break;
    case 0xa: {
        u32 _r3;
        _r3 = fn_80119ED0((u16)p5);
        if ((u16)_r3 != 0x9) {
            _r3 = 0;
        } else {
            _r3 = fn_8011B444(p1, (u16)p5);
        }
        ret = _r3;
        if ((u8)ret != 0x2) break;
        _r3 = fn_80119ED0((u16)p5);
        if ((u16)_r3 != 0x9) break;
        fn_8011B2C0(p1, (u16)p5, (u16)p4);
        break;
    }
    case 0xc:
        fn_801F68C8(p1, (u16)p5);
        break;
    case 0xd:
        fn_801F68B8(p1, (u16)p5);
        break;
    case 0x36:
        if (p5 != 0) {
            if ((u8)fn_80206780(p5) == 0x1) {
                fn_80132A38(0xf, p5);
                {
                    u32 _v;
                    _v = fn_80207BF4(p5);
                    _v = fn_8011CB6C((u16)_v);
                    fn_8011CB54();
                    _v = fn_800FA280();
                    fn_80132A38(0x1a, _v);
                }
                fn_80132A38(0x1f, p5);
                fn_80132A38(0x21, p5);
                fn_80132A38(0x20, p5);
            } else {
                fn_80132A38(0xf, 0);
                fn_80132A38(0x1a, 0);
                fn_80132A38(0x1f, 0);
                fn_80132A38(0x21, 0);
                fn_80132A38(0x20, 0);
            }
        } else {
            fn_80132A38(0xf, 0);
            fn_80132A38(0x1a, 0);
            fn_80132A38(0x1f, 0);
            fn_80132A38(0x21, 0);
            fn_80132A38(0x20, 0);
        }
        fn_801F68A4(p1, p5);
        break;
    case 0x37:
        fn_801254B4(fn_801F6A98(p1), 0, 0xda, 0, p5);
        break;
    case 0x38:
        fn_801254B4(fn_801F6A98(p1), 0, 0xdb, 0, p5);
        break;
    case 0x39:
        fn_801254B4(fn_801F6A98(p1), 0, 0xdc, 0, p5);
        break;
    case 0x3a:
        fn_801254B4(fn_801F6A98(p1), 0, 0xdd, 0, p5);
        break;
    case 0x3b:
        fn_801254B4(fn_801F6A98(p1), 0, 0xde, 0, p5);
        break;
    case 0x3c:
        fn_801254B4(fn_801F6A98(p1), 0, 0xdf, 0, p5);
        break;
    case 0x3d:
        fn_801254B4(fn_801F6A98(p1), 0, 0xe0, 0, p5);
        break;
    case 0x3e:
        fn_801254B4(fn_801F6A98(p1), 0, 0xe1, 0, p5);
        break;
    case 0x3f:
        fn_801254B4(fn_801F6A98(p1), 0, 0xe2, 0, p5);
        break;
    case 0x40:
        fn_801254B4(fn_801F6A98(p1), 0, 0xe3, 0, p5);
        break;
    case 0x41:
        fn_801254B4(fn_801F6A98(p1), 0, 0xe4, 0, p5);
        break;
    case 0x42:
        if (p5 != 0) {
            if ((u8)fn_80206780(p5) == 0x1) {
                fn_80132A38(0x10, p5);
                {
                    u32 _v;
                    _v = fn_80207BF4(p5);
                    _v = fn_8011CB6C((u16)_v);
                    fn_8011CB54();
                    _v = fn_800FA280();
                    fn_80132A38(0x1b, _v);
                }
                fn_80132A38(0x42, p5);
                fn_80132A38(0x44, p5);
                fn_80132A38(0x43, p5);
            } else {
                fn_80132A38(0x10, 0);
                fn_80132A38(0x1b, 0);
                fn_80132A38(0x42, 0);
                fn_80132A38(0x44, 0);
                fn_80132A38(0x43, 0);
            }
        } else {
            fn_80132A38(0x10, 0);
            fn_80132A38(0x1b, 0);
            fn_80132A38(0x42, 0);
            fn_80132A38(0x44, 0);
            fn_80132A38(0x43, 0);
        }
        fn_801F6890(p1, p5);
        break;
    case 0x43:
        fn_801F4C14(p1, 0, 0x42, 0, p5);
        {
            u32 species;
            species = fn_801F0134(p5, (u16)tmp);
            fn_801F4C14(p1, 0, 0x40, 0, (u32)(u16)species);
        }
        break;
    case 0x44:
        fn_801F687C(p1, p5);
        break;
    case 0x45:
        fn_801F6868(p1, p5);
        break;
    case 0x46:
        fn_801F64AC(p1, p5);
        break;
    case 0x47:
        if (p5 != 0) {
            if ((u8)fn_80206780(p5) == 0x1) {
                fn_80132A38(0x12, p5);
                {
                    u32 _v;
                    _v = fn_80207BF4(p5);
                    _v = fn_8011CB6C((u16)_v);
                    fn_8011CB54();
                    _v = fn_800FA280();
                    fn_80132A38(0x1d, _v);
                }
            } else {
                fn_80132A38(0x12, 0);
                fn_80132A38(0x1d, 0);
            }
        } else {
            fn_80132A38(0x12, 0);
            fn_80132A38(0x1d, 0);
        }
        fn_801F647C(p1, p5);
        break;
    case 0x48:
        fn_801F644C(p1, p5);
        break;
    case 0x49:
        if (p5 != 0 && (u8)fn_80206780(p5) == 0x1) {
            u32 species;
            species = fn_802040E8(p5);
            fn_801F4C14(p1, 0, 0x56, 0, (u32)(u16)species);
        }
        fn_801F641C(p1, p5);
        break;
    case 0x4a:
        fn_801F640C(p5);
        break;
    case 0x4b:
        if (p5 != 0) {
            if ((u8)fn_80206780(p5) == 0x1) {
                fn_80132A38(0x1e, p5);
                {
                    u32 _v;
                    _v = fn_80207BF4(p5);
                    _v = fn_8011CB6C((u16)_v);
                    fn_8011CB54();
                    _v = fn_800FA280();
                    fn_80132A38(0x1c, _v);
                }
            } else {
                fn_80132A38(0x1e, 0);
                fn_80132A38(0x1c, 0);
            }
        } else {
            fn_80132A38(0x1e, 0);
            fn_80132A38(0x1c, 0);
        }
        fn_801F6854(p1, p5);
        break;
    case 0x4c:
        fn_801F6840(p1, p5);
        break;
    case 0x4d:
        fn_801F682C(p1, p5);
        break;
    case 0x4e:
        fn_801F6818(p1, p5);
        break;
    case 0x4f:
        fn_801F6804(p1, p5);
        break;
    case 0x50:
        fn_801F67F0(p1, p5);
        break;
    case 0x51:
        fn_801F67DC(p1, p5);
        break;
    case 0x52:
        fn_801F67C8(p1, p5);
        break;
    case 0x53:
        fn_801F67B4(p1, p5);
        break;
    case 0x54:
        fn_801F67A0(p1, (u16)p5);
        break;
    case 0x55:
        fn_801F678C(p1, (u16)p5);
        break;
    case 0x56:
        itemGetStatus(0, (u16)p5, 1, 0);
        fn_80132A38(0x29, fn_800FA280());
        fn_801F6778(p1, (u16)p5);
        break;
    case 0x57:
        fn_8011CB6C((u16)p5);
        fn_8011CB54();
        fn_80132A38(0x1c, fn_800FA280());
        fn_801F6764(p1, (u16)p5);
        break;
    case 0x58:
        fn_801F65C0(p1, (s16)p5);
        break;
    case 0x59:
        fn_801F6560(p1, (u16)p4, p5);
        break;
    case 0x5a:
        fn_801F650C(p1, p5);
        break;
    case 0x5b:
        fn_801F64DC(p1, (u16)p5);
        break;
    case 0x5c:
        fn_801F3984(p1, (u16)p5);
        break;
    case 0x5e:
    default:
        fn_80132A38(0x2f, p5);
        break;
    }
    return (u8)ret;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 1
asm void fn_801F54A4(void) {
#include "src/game/pokemon_fn_801F54A4.inc"
}
#else
/* Real C at 98.8% (instruction-equivalent): residuals are a pkm/a16
 * r30/r31 coloring swap, the case-0xA/0xB in-place arg mask shape, and
 * the dtk-named jumptable symbol. See tools/decomp_work/scratch/pokemon_boss.c
 * history; flip #if only at verified 100%. */
/* 0x801F54A4 | size: 0xD18 | PokemonGet: field dispatcher */
s32 fn_801F54A4(u8* pkm, u32 slot, u32 field, u32 arg) {
    extern struct Pokemon* fn_801F6738(u32);
    extern struct Pokemon* fn_801F6B48(void);
    extern u32 fn_80119ED0(u32);
    extern u32 fn_8011B67C(u8*, u32);
    extern u32 fn_8011B444(u8*, u32);
    extern void fn_801F37B0(u8*, u32 (*)(u8*, u32*), u32*, u32);
    extern u32 _fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv(u8*, u32*);
    extern u32 fn_8012640C(u32, u32, u32, u32);
    extern u32 fn_801EF634(void);
    extern u8 fn_801F66D4(u8*);
    extern u32 fn_801F66BC(u8*);
    extern u32 fn_801F66A4(u8*);
    extern u32 fn_801F668C(u8*);
    extern u16 fn_801F6B00(u8*);
    extern u16 fn_801F6AE8(u8*);
    extern u32 fn_801F6A98(u8*);
    extern u32 fn_801F6A7C(u8*);
    extern u32 fn_801F6A60(u8*);
    extern u32 fn_801F6A44(u8*);
    extern u32 fn_801F6A28(u8*);
    extern u32 fn_801F6A0C(u8*);
    extern u32 fn_801F69F0(u8*);
    extern u32 fn_801F69D4(u8*);
    extern u32 fn_801F69B8(u8*);
    extern u32 fn_801F699C(u8*);
    extern u32 fn_801F6980(u8*);
    extern u32 fn_801F6964(u8*);
    extern u32 fn_801F6948(u8*);
    extern u32 fn_801F692C(u8*);
    extern u32 fn_801F6910(u8*);
    extern u32 fn_801F68F4(u8*);
    extern u32 fn_801F68D8(u8*);
    extern u32 fn_801F65D4(u8*);
    extern u32 fn_801F64F0(u8*);
    extern u32 fn_801F64C0(u8*);
    extern u32 fn_801F6490(u8*);
    extern u32 fn_801F6460(u8*);
    extern u32 fn_801F6430(u8*);
    extern u32 fn_801F6414(void);
    extern u32 fn_801F7404(u32);
    extern u32 fn_801F7258(u32, u32);
    extern u32 fn_801F981C(u32, u32);
    extern u32 fn_8020E0F8(u32);
    extern u32 fn_8020E0B0(u32);
    extern u32 fn_8020E0C8(u32);
    extern u32 fn_8020E0E0(u32);
    extern u32 fn_8020DD80(u32);
    extern u32 fn_8020DD44(u32);
    extern u32 fn_8020E204(u32);
    extern u32 fn_8020E1EC(u32);
    extern u32 fn_8020E1D4(u32);
    extern u32 fn_8020E1BC(u32);
    extern u32 fn_8020E1A4(u32);
    extern u32 fn_8020E124(u32);
    extern u32 fn_8020E488(u32);
    extern u32 fn_8020E248(u32);
    extern u32 fn_8020E4B4(u32);
    extern u32 fn_8020E470(u32);
    extern u32 fn_8020E458(u32);
    extern u32 fn_8020E440(u32);
    extern u32 fn_8020E428(u32);
    extern u32 fn_8020E410(u32);
    extern u32 fn_8020E3F8(u32);
    extern u32 fn_8020E3E0(u32);
    extern u32 fn_8020E3C8(u32);
    extern u32 fn_8020E3B0(u32);
    extern u32 fn_8020E398(u32);
    extern u32 fn_8020E380(u32);
    extern u32 fn_8020E368(u32);
    extern u32 fn_8020E350(u32);
    extern u32 fn_8020E338(u32);
    extern u32 fn_8020E320(u32);
    extern u32 fn_8020E308(u32);
    extern u32 fn_8020E2F0(u32);
    extern u32 fn_8020E2D8(u32);
    extern u32 fn_8020E2C0(u32);
    extern u32 fn_8020E2A8(u32);
    extern u32 fn_8020E290(u32);
    extern u32 fn_8020E278(u32);
    extern u32 fn_8020E260(u32);
    extern u32 fn_8020E230(u32);
    u16 f;

    f = (u16)field;
    if (f >= 0x60) {
        return 0;
    }
    if (f == 0) {
        return (s32)fn_801F6B48();
    }
    if (f < 8) {
        if ((pkm = (u8*)fn_801F6738(slot)) == NULL) {
            return 0;
        }
    } else if (f < 0x5F) {
        if (pkm == NULL) {
            if ((pkm = (u8*)fn_801F6B48()) == NULL) {
                return 0;
            }
        }
    }

    switch ((u16)field) {
    case 0x1:
        return fn_801F6600(pkm);
    case 0x2:
        return fn_801F6720(pkm);
    case 0x3:
        return (u16)fn_801F66EC(pkm, (u8)arg);
    case 0x4:
        return fn_801F66D4(pkm);
    case 0x5:
        return fn_801F66BC(pkm);
    case 0x6:
        return fn_801F66A4(pkm);
    case 0x7:
        return fn_801F668C(pkm);
    case 0x9: {
        extern u8* fn_801F6B18();
        return (s32)fn_801F6B18(pkm, arg);
    }
    case 0xA: {
        u32 v;
        arg &= 0xFFFF;
        if ((u16)fn_80119ED0(arg) != 9) {
            v = 0;
        } else {
            v = fn_8011B67C(pkm, arg);
        }
        return (u8)v;
    }
    case 0xB: {
        u32 v;
        arg &= 0xFFFF;
        if ((u16)fn_80119ED0(arg) != 9) {
            v = 0;
        } else {
            v = fn_8011B444(pkm, arg);
        }
        return (u8)v;
    }
    case 0xC:
        return fn_801F6B00(pkm);
    case 0xD:
        return fn_801F6AE8(pkm);
    case 0xE:
        return (u16)fn_8020E0B0(fn_8020E0F8((u16)fn_801F54A4(pkm, 0, 0xD, 0)));
    case 0xF:
        return fn_801F54A4(NULL, (u16)fn_801F54A4(pkm, 0, 0xE, 0), 4, 0);
    case 0x10:
        return fn_801F54A4(NULL, (u16)fn_801F54A4(pkm, 0, 0xE, 0), 5, 0);
    case 0x11:
        return fn_8020DD80((u16)fn_801F54A4(pkm, 0, 0xD, 0));
    case 0x12:
        return fn_8020DD44((u16)fn_801F54A4(pkm, 0, 0xD, 0));
    case 0x13: {
        u32 v;
        if ((u8)arg == 1) {
            u32 a[4];
            u32 b[4];
            a[0] = 0xD;
            a[1] = 0;
            a[2] = 0;
            a[3] = 0;
            fn_801F37B0(pkm, _fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv, a, 0);
            if ((u16)a[1] != 0) {
                v = 0;
                goto done13;
            }
            b[0] = 0x4D;
            b[1] = 0;
            b[2] = 0;
            b[3] = 0;
            fn_801F37B0(pkm, _fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv, b, 0);
            if ((u16)b[1] != 0) {
                v = 0;
                goto done13;
            }
        }
        if (fn_801F54A4(pkm, 0, 0xA, 0x4E) == 1) {
            v = 0;
        } else if (fn_801F54A4(pkm, 0, 0xA, 0x4F) == 1) {
            v = 1;
        } else if (fn_801F54A4(pkm, 0, 0xA, 0x50) == 1) {
            v = 2;
        } else if (fn_801F54A4(pkm, 0, 0xA, 0x51) == 1) {
            v = 3;
        } else if (fn_801F54A4(pkm, 0, 0xA, 0x52) == 1) {
            v = 4;
        } else if (fn_801F54A4(pkm, 0, 0xA, 0x53) == 1) {
            v = 1;
        } else if (fn_801F54A4(pkm, 0, 0xA, 0x54) == 1) {
            v = 2;
        } else if (fn_801F54A4(pkm, 0, 0xA, 0x55) == 1) {
            v = 3;
        } else {
            v = 0;
        }
    done13:
        return (u8)v;
    }
    case 0x14:
        return (u8)fn_8020E0C8(fn_8020E0F8((u16)fn_801F54A4(pkm, 0, 0xD, 0)));
    case 0x15:
        return fn_8020E1EC(fn_8020E204((u16)fn_801F54A4(pkm, 0, 0x14, 0)));
    case 0x16:
        return (u8)fn_8020E1D4(fn_8020E204((u16)fn_801F54A4(pkm, 0, 0x14, 0)));
    case 0x17:
        return (u8)fn_8020E1BC(fn_8020E204((u16)fn_801F54A4(pkm, 0, 0x14, 0)));
    case 0x18:
        return (u8)fn_8020E1A4(fn_8020E204((u16)fn_801F54A4(pkm, 0, 0x14, 0)));
    case 0x19:
        return (u16)fn_8020E124((u16)fn_801F54A4(pkm, 0, 0x14, 0));
    case 0x1A:
        return (u8)fn_8020E0E0(fn_8020E0F8((u16)fn_801F54A4(pkm, 0, 0xD, 0)));
    case 0x1B:
        return fn_8020E248(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x1C:
        return (u8)fn_8020E4B4(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x1D:
        return (u8)fn_8020E470(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x1E:
        return (u8)fn_8020E458(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x1F:
        return (u8)fn_8020E440(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x20:
        return (u8)fn_8020E428(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x21:
        return (u8)fn_8020E410(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x22:
        return (u8)fn_8020E3F8(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x23:
        return (u8)fn_8020E3E0(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x24:
        return (u8)fn_8020E3C8(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x26:
        return (u8)fn_8020E3B0(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x25:
        return (u8)fn_8020E398(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x27:
        return (u8)fn_8020E380(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x28:
        return (u8)fn_8020E368(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x29:
        return (u8)fn_8020E350(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x2A:
        return (u8)fn_8020E338(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x2B:
        return (u8)fn_8020E320(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x2C:
        return (u8)fn_8020E308(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x2D:
        return (u8)fn_8020E2F0(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x2E:
        return (u8)fn_8020E2D8(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x2F:
        return (u8)fn_8020E2C0(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x30:
        return (u8)fn_8020E2A8(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x31:
        return (u8)fn_8020E290(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x32:
        return (u8)fn_8020E278(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x33:
        return (u8)fn_8020E260(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x34:
        return (u8)fn_8020E230(fn_8020E488((u16)fn_801F54A4(pkm, 0, 0x1A, 0)));
    case 0x35: {
        extern u8* fn_801F6AB4();
        return (s32)fn_801F6AB4(pkm, arg);
    }
    case 0x36:
        return fn_801F6A98(pkm);
    case 0x37:
        return fn_8012640C(fn_801F6A98(pkm), 0, 0xDA, 0);
    case 0x38:
        return fn_8012640C(fn_801F6A98(pkm), 0, 0xDB, 0);
    case 0x39:
        return fn_8012640C(fn_801F6A98(pkm), 0, 0xDC, 0);
    case 0x3A:
        return fn_8012640C(fn_801F6A98(pkm), 0, 0xDD, 0);
    case 0x3B:
        return fn_8012640C(fn_801F6A98(pkm), 0, 0xDE, arg);
    case 0x3C:
        return fn_8012640C(fn_801F6A98(pkm), 0, 0xDF, arg);
    case 0x3D:
        return fn_8012640C(fn_801F6A98(pkm), 0, 0xE0, arg);
    case 0x3E:
        return fn_8012640C(fn_801F6A98(pkm), 0, 0xE1, 0);
    case 0x3F:
        return fn_8012640C(fn_801F6A98(pkm), 0, 0xE2, 0);
    case 0x40:
        return fn_8012640C(fn_801F6A98(pkm), 0, 0xE3, 0);
    case 0x41:
        return fn_8012640C(fn_801F6A98(pkm), 0, 0xE4, 0);
    case 0x42:
        return fn_801F6A7C(pkm);
    case 0x44:
        return fn_801F6A60(pkm);
    case 0x45:
        return fn_801F6A44(pkm);
    case 0x46:
        return fn_801F64C0(pkm);
    case 0x47:
        return fn_801F6490(pkm);
    case 0x48:
        return fn_801F6460(pkm);
    case 0x49:
        return fn_801F6430(pkm);
    case 0x4A:
        return fn_801F6414();
    case 0x4B:
        return fn_801F6A28(pkm);
    case 0x4C:
        return fn_801F6A0C(pkm);
    case 0x4D:
        return fn_801F69F0(pkm);
    case 0x4E:
        return fn_801F69D4(pkm);
    case 0x4F:
        return fn_801F69B8(pkm);
    case 0x50:
        return fn_801F699C(pkm);
    case 0x51:
        return fn_801F6980(pkm);
    case 0x52:
        return fn_801F6964(pkm);
    case 0x53:
        return fn_801F6948(pkm);
    case 0x54:
        return (u16)fn_801F692C(pkm);
    case 0x55:
        return (u16)fn_801F6910(pkm);
    case 0x56:
        return (u16)fn_801F68F4(pkm);
    case 0x57:
        return (u16)fn_801F68D8(pkm);
    case 0x58:
        return (s16)fn_801F65D4(pkm);
    case 0x59:
        return fn_801F6588(pkm, (u16)arg);
    case 0x5A:
        return (s32)fn_801F6544(pkm);
    case 0x5B:
        return (u16)fn_801F64F0(pkm);
    case 0x5C:
        return (u16)fn_801EF634();
    case 0x5D: {
        u32 a16, n, c18, c16, k, i, j, m;
        fn_801F54A4(pkm, 0, 0x14, 0);
        c16 = (u16)fn_801F54A4(pkm, 0, 0x16, 0);
        c18 = (u16)fn_801F54A4(pkm, 0, 0x18, 0);
        n = 0;
        a16 = (u16)arg;
        for (i = 0; (u16)i < c18; i++) {
            for (j = 0; (u16)j < c16; j++) {
                for (k = 0; (u16)k < 2; k++) {
                    u32 v;
                    m = fn_801F54A4(pkm, 0, 0x35, k);
                    if ((u8)fn_801F7404(m) == 0) {
                        m = 0;
                    }
                    if (m == 0) {
                        v = 0;
                    } else {
                        v = fn_801F7258(m, j);
                        if (v == 0) {
                            v = 0;
                        } else {
                            v = fn_801F981C(v, i);
                            if (v == 0) {
                                v = 0;
                            }
                        }
                    }
                    if (v != 0 && (u16)n == a16) {
                        return v;
                    }
                    n++;
                }
            }
        }
        return 0;
    }
    default:
        return 0;
    }
}
#endif
#pragma pop

/* 0x801F61EC | size: 0x220 | large */
u32 fn_801F61EC(u32 param_1, u32 *param_2, u32 param_3, u32 param_4, u32 param_5) {
    extern u32 fn_801F02AC(u32, u32, u32);
    extern u32 fn_801F54A4(u32, u32, u16, u32);
    extern u32 fn_801F7258(u32, u32);
    extern u32 fn_801F7404(u32);
    extern u32 fn_801F981C(u32, u32);
    extern u8 fn_802062FC(u32);
    extern u8 fn_80206780(u32);
    u16 uVar3;
    u16 uVar6;
    u16 uVar5;
    u32 uVar7;
    u32 uCount;
    u32 uSlot;
    u32 uVar8;
    u32 uMove;
    u32 uOuter;
    u32 uMid;
    u32 uInner;
    u32 uIdx;

    {
        u32 i = 0;
        u32 val = i;
        while ((i & 0xFFFF) < 8) {
            *(u32*)((u8*)param_2 + ((i & 0xFFFF) << 2)) = val;
            i = i + 1;
        }
    }
    uCount = 0;
    uVar3 = fn_801F54A4(param_1, 0, 0x14, 0);
    uVar6 = fn_801F54A4(param_1, 0, 0x16, 0);
    uVar5 = fn_801F54A4(param_1, 0, 0x18, 0);
    if ((u8)param_4 != 0) {
        if ((u8)fn_80206780(param_5) == 0) {
            return 0;
        }
        uVar7 = fn_801F02AC(2, param_5, uVar3);
    } else {
        uVar7 = 0;
    }
    uOuter = 0;
    while ((uOuter & 0xFFFF) < 2) {
        uSlot = fn_801F54A4(param_1, 0, 0x35, uOuter);
        if ((u8)fn_801F7404(uSlot) == 0) {
            uSlot = 0;
        }
        if (uSlot != 0) {
            if ((u8)param_4 != 1 || uVar7 != uSlot) {
                if ((u8)param_4 != 2 || uVar7 == uSlot) {
                    uMid = 0;
                    while ((uMid & 0xFFFF) < (uVar6 & 0xFFFF)) {
                        uVar8 = fn_801F7258(uSlot, uMid);
                        if (uVar8 != 0) {
                            uInner = 0;
                            while ((uInner & 0xFFFF) < (uVar5 & 0xFFFF)) {
                                uMove = fn_801F981C(uVar8, uInner);
                                if (uMove != 0) {
                                    if (uVar7 != uSlot) {
                                        if ((u8)param_3 == 1) {
                                            if ((u8)fn_802062FC(uMove) != 0) {
                                                goto skip_move;
                                            }
                                        }
                                        if ((u8)param_4 == 3 && uMove == param_5) {
                                            goto skip_move;
                                        }
                                        uIdx = 0;
                                        while ((uIdx & 0xFFFF) < 8) {
                                            if (*(u32*)((u8*)param_2 + ((uIdx & 0xFFFF) << 2)) == 0) {
                                                *(u32*)((u8*)param_2 + ((uIdx & 0xFFFF) << 2)) = uMove;
                                                break;
                                            }
                                            uIdx = uIdx + 1;
                                        }
                                        if ((s16)uIdx < 8) {
                                            uCount = uCount + 1;
                                        }
                                    }
                                }
                                skip_move:
                                uInner = uInner + 1;
                            }
                        }
                        uMid = uMid + 1;
                    }
                }
            }
        }
        uOuter = uOuter + 1;
    }
    return uCount;
}

/* 0x801F650C | size: 0x38 | small */
void fn_801F650C(u32 *param_1, u32 *param_2) {
    u32 i;

    if (param_1 == NULL) return;
    for (i = 0; (i & 0xFFFF) < 8; i = i + 1) {
        *(u32*)((u8*)param_1 + ((u16)i << 2) + 0xA4C4) = *(u32*)((u8*)param_2 + ((u16)i << 2));
    }
}

/* 0x801F6544 | size: 0x1C */
u8* fn_801F6544(u8* ptr) {
    if (ptr == NULL) { return NULL; }
    return ptr + 0xA4C4;
}

/* 0x801F6560 | size: 0x28 */
void fn_801F6560(u8* ptr, u16 idx, u32 val) {
    u32* base;
    if (ptr == NULL) { return; }
    if (idx >= 8) { return; }
    base = (u32*)(ptr + 0xA4C4);
    base[idx] = val;
}

/* 0x801F6588 | size: 0x38 */
u32 fn_801F6588(u8* ptr, u16 idx) {
    u32* base;
    if (ptr == NULL) { return 0; }
    if (idx >= 8) { return 0; }
    base = (u32*)(ptr + 0xA4C4);
    return base[idx];
}

/* 0x801F6658 | size: 0x24 */
void fn_801F6658(u8* ptr, u8 idx, u16 val) {
    if (ptr == NULL) { return; }
    if (idx >= 2) { return; }
    ptr += idx * 2;
    *(u16*)(ptr + 0x14) = val;
}

/* 0x801F66EC | size: 0x34 */
u32 fn_801F66EC(u8* ptr, u8 idx) {
    if (ptr == NULL) { return 0; }
    if (idx >= 2) { return 0; }
    ptr += idx * 2;
    return *(u16*)(ptr + 0x14);
}

/* 0x801F6AB4 | size: 0x34 */
u8* fn_801F6AB4(u8* ptr, u16 idx) {
    if (ptr == NULL) { return NULL; }
    if (idx >= 2) { return NULL; }
    return ptr + idx * 0x5230 + 0x14;
}

/* 0x801F6B18 | size: 0x30 */
u8* fn_801F6B18(u8* ptr, u16 idx) {
    if (ptr == NULL) { return NULL; }
    if (idx >= 1) { return NULL; }
    return ptr + idx * 0x10;
}

/* 0x801F6B54 | size: 0xF8 | medium */
void fn_801F6B54(u32 param_1, u32 param_2, u32 param_3, u32 param_4, u32 param_5) {
    extern u32 fn_801F78D4(u32, u32);
    extern u8 fn_801FA634(u32);
    extern u32 fn_801FB1C0(u32, u32, u16, u32);
    extern u8 fn_802062FC(u32);
    extern u32 fn_8012640C(u32, u32, u16, u32);
    extern void fn_801FAA58(u32, u32, u16, u32);
    u32 outerObj;
    u32 innerObj;
    u32 outerIndex;
    s32 status;
    u16 outerCount;

    outerCount = (u16)param_4;
    outerIndex = 0;
    for (; (outerIndex & 0xFFFF) < (outerCount & 0xFFFF); outerIndex = outerIndex + 1) {
        if (param_1 == 0) {
            outerObj = 0;
        } else {
            outerObj = fn_801F78D4(param_1, outerIndex);
        }
        status = fn_801FA634(outerObj);
        if (status != 0) {
            u16 innerCount;
            u32 innerIndex;

            innerCount = param_5;
            innerIndex = 0;
            for (; (innerIndex & 0xFFFF) < (innerCount & 0xFFFF); innerIndex = innerIndex + 1) {
                innerObj = fn_801FB1C0(outerObj, 0, 0x46, innerIndex);
                status = fn_802062FC(innerObj);
                if (status != 0) {
                    innerObj = fn_8012640C(innerObj, 0, 0xD5, 0);
                    status = fn_8012640C(innerObj, 0, 0xCE, 0);
                    if ((s16)status >= 0) {
                        fn_801FAA58(param_2, 0, 0x57, 0);
                    }
                }
            }
        }
    }
}

/* 0x801F6C4C | size: 0x54 | small */
u32 fn_801F6C4C(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern u32 fn_8011A860(u32, u32);

    if ((u16)fn_80119ED0(param_2) != 6) {
        return 0;
    }
    return fn_8011A860(param_1, param_2);
}

/* 0x801F6CA0 | size: 0x54 | small */
void fn_801F6CA0(u32 param_1, u32 param_2, u32 param_3) {
    extern u32 fn_80119ED0(u32);
    extern void fn_8011AB50(u32, u32, u32);

    if ((u16)fn_80119ED0(param_2) == 6) {
        fn_8011AB50(param_1, param_2, param_3);
    }
}

/* 0x801F6CF4 | size: 0x54 | small */
u32 fn_801F6CF4(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern u32 fn_8011ACB4(u32, u32);

    if ((u16)fn_80119ED0(param_2) != 6) {
        return 0xFFFFFFFF;
    }
    return fn_8011ACB4(param_1, param_2);
}

/* 0x801F6D48 | size: 0x54 | small */
u32 fn_801F6D48(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern u32 fn_8011AE40(u32, u32);

    if ((u16)fn_80119ED0(param_2) != 6) {
        return 0xFFFFFFFF;
    }
    return fn_8011AE40(param_1, param_2);
}

/* 0x801F6D9C | size: 0x54 | small */
u32 fn_801F6D9C(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern u32 fn_8011B130(u32, u32);

    if ((u16)fn_80119ED0(param_2) != 6) {
        return 0xFFFFFFFF;
    }
    return fn_8011B130(param_1, param_2);
}

/* 0x801F6DF0 | size: 0x54 | small */
void fn_801F6DF0(u32 param_1, u32 param_2, u32 param_3) {
    extern u32 fn_80119ED0(u32);
    extern void fn_8011B2C0(u32, u32, u32);

    if ((u16)fn_80119ED0(param_2) == 6) {
        fn_8011B2C0(param_1, param_2, param_3);
    }
}

/* 0x801F6E44 | size: 0x54 | small */
u32 fn_801F6E44(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern u32 fn_8011B444(u32, u32);

    if ((u16)fn_80119ED0(param_2) != 6) {
        return 0;
    }
    return fn_8011B444(param_1, param_2);
}

/* 0x801F6E98 | size: 0x54 | small */
u32 fn_801F6E98(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern u32 fn_8011B67C(u32, u32);

    if ((u16)fn_80119ED0(param_2) != 6) {
        return 0;
    }
    return fn_8011B67C(param_1, param_2);
}

/* 0x801F6EEC | size: 0x4C | small */
void fn_801F6EEC(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern void fn_8011B788(u32, u32);

    if ((u16)fn_80119ED0(param_2) == 6) {
        fn_8011B788(param_1, param_2);
    }
}

/* 0x801F6F38 | size: 0x9C | medium */
s16 fn_801F6F38(u32 param_1, u32 param_2, u32 param_3, u32 param_4) {
    extern u32 fn_801F78D4(u32, u32);
    extern s16 fn_801F81F8(u32, u32, u32);
    extern u8 fn_801FA634(u32);
    u16 uVar4;
    u32 uVar2;
    u16 uVar1;
    u32 uVar5;
    u32 uVar3;

    uVar4 = param_2;
    uVar2 = 0;
    uVar1 = 0;
    for (; (uVar2 & 0xFFFF) < (uVar4 & 0xFFFF); uVar2 = uVar2 + 1) {
        if (param_1 == 0) {
            uVar5 = 0;
        } else {
            uVar5 = fn_801F78D4(param_1, uVar2);
        }
        uVar3 = fn_801FA634(uVar5);
        if (uVar3 != 0) {
            uVar1 = uVar1 + fn_801F81F8(uVar5, param_3, param_4);
        }
    }
    return uVar1;
}

/* 0x801F6FD4 | size: 0xBC | medium */
u16 fn_801F6FD4(u32 param_1, u32 param_2, u32 param_3) {
    extern u32 fn_801F78D4(u32, u32);
    extern u32 fn_801F986C(u32, u32);
    extern u8 fn_801FA634(u32);
    extern u8 fn_80206608(u32);
    u32 uCount;
    u32 uVar4;
    u16 uBound2;
    u32 uVar1;
    u32 iVar2;
    u32 uVar3;
    u16 uBound3;
    u32 uVar5;

    uCount = 0;
    uVar4 = uCount;
    uBound2 = param_2;
    for (; (uVar4 & 0xFFFF) < (uBound2 & 0xFFFF); uVar4 = uVar4 + 1) {
        if (param_1 == 0) {
            uVar1 = 0;
        } else {
            uVar1 = fn_801F78D4(param_1, uVar4);
        }
        uVar3 = fn_801FA634(uVar1);
        if (uVar3 != 0) {
            uBound3 = param_3;
            uVar5 = 0;
            for (; (uVar5 & 0xFFFF) < (uBound3 & 0xFFFF); uVar5 = uVar5 + 1) {
                iVar2 = fn_801F986C(uVar1, uVar5);
                if ((iVar2 != 0) && (uVar3 = fn_80206608(iVar2), uVar3 != 0)) {
                    uCount = uCount + 1;
                }
            }
        }
    }
    return uCount;
}

/* 0x801F7090 | size: 0xE4 | medium */
s32 fn_801F7090(u32 param_1, u32 param_2, u32 param_3) {
    extern u32 fn_801F78D4(u32, u32);
    extern u32 fn_801F986C(u32, u32);
    extern u8 fn_801FA634(u32);
    extern u32 fn_80205BE8(u32);
    extern u8 fn_80206608(u32);
    extern u32 fn_8012640C(u32, u32, u16, u32);
    u16 uBound2;
    s32 iVar8;
    u32 uVar7;
    u32 uVar1;
    u32 iVar2;
    u32 iVar3;
    u32 uVar4;
    u16 uBound3;
    u32 uVar6;

    uBound2 = param_2;
    iVar8 = 0;
    uVar7 = 0;
    for (; (uVar7 & 0xFFFF) < (uBound2 & 0xFFFF); uVar7 = uVar7 + 1) {
        if (param_1 == 0) {
            uVar1 = 0;
        } else {
            uVar1 = fn_801F78D4(param_1, uVar7);
        }
        uVar4 = fn_801FA634(uVar1);
        if (uVar4 != 0) {
            uBound3 = param_3;
            uVar6 = 0;
            for (; (uVar6 & 0xFFFF) < (uBound3 & 0xFFFF); uVar6 = uVar6 + 1) {
                iVar2 = fn_801F986C(uVar1, uVar6);
                if (iVar2 != 0) {
                    iVar3 = fn_80205BE8(iVar2);
                    if ((iVar3 != 0) && (uVar4 = fn_80206608(iVar2), uVar4 != 0)) {
                        uVar4 = fn_8012640C(iVar3, 0, 0x87, 0);
                        iVar8 = iVar8 + (uVar4 & 0xFFFF);
                    }
                }
            }
        }
    }
    return iVar8;
}

/* 0x801F7174 | size: 0xE4 | medium */
s32 fn_801F7174(u32 param_1, u32 param_2, u32 param_3) {
    extern u32 fn_801F78D4(u32, u32);
    extern u32 fn_801F986C(u32, u32);
    extern u8 fn_801FA634(u32);
    extern u32 fn_80205BE8(u32);
    extern u8 fn_80206608(u32);
    extern u32 fn_8012640C(u32, u32, u16, u32);
    int total;
    u32 innerLimit;
    u32 baseObj;
    u32 outerObj;
    u32 innerObj;
    u32 statObj;
    u32 status;
    u16 outerCount;
    u32 outerIndex;

    total = 0;
    baseObj = param_1;
    innerLimit = param_3;
    outerCount = param_2;
    outerIndex = 0;
    for (; (outerIndex & 0xFFFF) < (outerCount & 0xFFFF); outerIndex = outerIndex + 1) {
        if (baseObj == 0) {
            outerObj = 0;
        } else {
            outerObj = fn_801F78D4(baseObj, outerIndex);
        }
        status = fn_801FA634(outerObj);
        if (status != 0) {
            u16 innerCount;
            u32 innerIndex;

            innerCount = innerLimit;
            innerIndex = 0;
            for (; (innerIndex & 0xFFFF) < (innerCount & 0xFFFF); innerIndex = innerIndex + 1) {
                innerObj = fn_801F986C(outerObj, innerIndex);
                if (innerObj != 0) {
                    statObj = fn_80205BE8(innerObj);
                    if ((statObj != 0) && (status = fn_80206608(innerObj), status != 0)) {
                        total = total + (fn_8012640C(statObj, 0, 0x83, 0) & 0xFFFF);
                    }
                }
            }
        }
    }
    return total;
}

/* 0x801F7258 | size: 0x58 | small */
u32 fn_801F7258(u32 param_1) {
    extern u32 fn_801F78D4(u32);
    extern u8 fn_801FA634(u32);
    u32 uVar1;
    u8 cVar2;

    if (param_1 == 0) {
        uVar1 = 0;
    } else {
        uVar1 = fn_801F78D4(param_1);
    }
    cVar2 = fn_801FA634(uVar1);
    if (cVar2 == 0) {
        return 0;
    }
    return uVar1;
}

/* 0x801F72B0 | size: 0xD8 | medium */
void fn_801F72B0(u32 param_1, u16 param_2, u16 param_3, s8* param_4, u8* param_5) {
    extern u32 fn_801F7870(u32);
    extern u16 fn_801F7858(u32);
    u32 uVar1;
    u16 uVar2;

    if (param_4 == NULL) return;
    if (param_5 == NULL) return;
    uVar1 = fn_801F7870(param_1);
    if (uVar1 == 0) {
        uVar2 = 0;
    } else {
        uVar2 = fn_801F7858(uVar1);
    }
    *param_4 = (s8)uVar2;
    *param_5 = 0;
    if ((u16)param_2 == 1) return;
    if ((u16)param_2 != 2) return;
    if (*param_4 == 1) {
        if ((u16)param_3 == 0) {
            *param_5 = 1;
        } else if ((u16)param_3 == 1) {
            *param_5 = -1;
        }
    } else {
        if ((u16)param_3 == 0) {
            *param_5 = -1;
        } else if ((u16)param_3 == 1) {
            *param_5 = 1;
        }
    }
}

/* 0x801F7388 | size: 0x7C | small */
#pragma scheduling off
u8 fn_801F7388(u32 param_1) {
    extern u32 fn_801F78D4(u32, u32);
    extern u8 fn_801FA524(u32);
    u32 uVar2;
    u8 cVar3;
    u32 bVar4;
    u8 cVar1;

    cVar1 = 0;
    bVar4 = 0;
    while ((u8)bVar4 < 2) {
        if (param_1 == 0) {
            uVar2 = 0;
        } else {
            uVar2 = fn_801F78D4(param_1, (u8)bVar4);
        }
        cVar3 = fn_801FA524(uVar2);
        if ((u8)cVar3 != 0) {
            cVar1 = (u8)((u8)cVar1 + 1);
        }
        bVar4 = bVar4 + 1;
    }
    return cVar1;
}
#pragma scheduling on

/* 0x801F7404 | size: 0x7C | small */
u32 fn_801F7404(u32 param_1) {
    extern u16 fn_801EF634(void);
    extern u16 fn_801F793C(u32);
    u16 sVar2;
    u32 uVar1;

    if (param_1 == 0) {
        return 0;
    }
    if ((u16)fn_801EF634() == 1) {
        return 0;
    }
    if (param_1 == 0) {
        uVar1 = 0;
    } else {
        sVar2 = fn_801F793C(param_1);
        uVar1 = (u16)sVar2;
    }
    if ((s32)uVar1 == 0) {
        return 0;
    }
    return 1;
}

/* 0x801F7480 | size: 0xB0 | medium */
void fn_801F7480(u32 param_1, u16 param_2) {
    extern void fn_8011B950(u32, u32);
    extern void fn_801F789C(u32, u32);
    extern void fn_801F78AC(u32, u16);
    extern u32 fn_801F78D4(u32, u32);
    extern u32 fn_801F7908(u32, u32);
    extern void fn_801FA8CC(u32, u32);
    u32 uVar1;

    if (param_1 != 0) {
        if (param_1 != 0) {
            if (param_1 != 0) {
                fn_801F78AC(param_1, 0);
            }
            if (param_1 == 0) {
                uVar1 = 0;
            } else {
                uVar1 = fn_801F7908(param_1, 0);
            }
            fn_8011B950(uVar1, 6);
            if (param_1 == 0) {
                uVar1 = 0;
            } else {
                uVar1 = fn_801F78D4(param_1, 0);
            }
            fn_801FA8CC(uVar1, 2);
            if (param_1 != 0) {
                fn_801F789C(param_1, 0);
            }
        }
        if (param_1 != 0) {
            fn_801F78AC(param_1, (u16)param_2);
        }
    }
}

/* 0x801F7530 | size: 0xC8 | medium */
void fn_801F7530(u32 param_1, u16 param_2) {
    extern void fn_8011B950(u32, u32);
    extern void fn_801F789C(u32, u32);
    extern void fn_801F78AC(u32, u16);
    extern u32 fn_801F78D4(u32, u32);
    extern u32 fn_801F7908(u32, u32);
    extern void fn_801FA8CC(u32, u32);
    u32 uVar1;
    u16 uVar2;
    u32 uVar3;

    if (param_1 != 0) {
        uVar2 = 0;
        while ((u16)uVar2 < (u16)param_2) {
            uVar3 = param_1 + (u32)(uVar2 & 0xFFFF) * 0x5230;
            if (uVar3 != 0) {
                if (uVar3 != 0) {
                    fn_801F78AC(uVar3, 0);
                }
                if (uVar3 == 0) {
                    uVar1 = 0;
                } else {
                    uVar1 = fn_801F7908(uVar3, 0);
                }
                fn_8011B950(uVar1, 6);
                if (uVar3 == 0) {
                    uVar1 = 0;
                } else {
                    uVar1 = fn_801F78D4(uVar3, 0);
                }
                fn_801FA8CC(uVar1, 2);
                if (uVar3 != 0) {
                    fn_801F789C(uVar3, 0);
                }
            }
            uVar2 = uVar2 + 1;
        }
    }
}

/* 0x801F75F8 | size: 0xC4 | medium */
void fn_801F75F8(u8* ptr1, u32 param2, u32 slotType, u32 param4, u32 param5) {
    extern u8* fn_801F7870(u32);
    extern void fn_801F77E0(u8*, u32);
    extern void fn_801F77BC(u8*, u32, u32);
    extern void fn_801F7798(u8*, u32, u32);
    extern void fn_801F78AC(u8*, u32);
    extern void fn_801F789C(u8*, u32);

    if ((u16)slotType == 0) {
        return;
    }
    if ((u16)slotType < 0xA) {
        ;
    } else {
        return;
    }
    if ((u16)slotType < 4) {
        ptr1 = fn_801F7870(param2);
    }
    if (ptr1 == NULL) {
        return;
    }
    switch ((u16)slotType) {
    case 1:
        fn_801F77E0(ptr1, (u16)param5);
        break;
    case 2:
        fn_801F77BC(ptr1, (0, (u8)param4), param5);
        break;
    case 3:
        fn_801F7798(ptr1, (0, (u8)param4), param5);
        break;
    case 5:
        fn_801F78AC(ptr1, (u16)param5);
        break;
    case 8:
        fn_801F789C(ptr1, (u8)param5);
        break;
    }
}

/* 0x801F76B8 | size: 0xE0 | fightSideGetStatus: per-side status dispatcher */
u32 fightSideGetStatus(u8* ptr1, u32 param2, u32 slotType, u32 param5) {
    extern u32 fn_801F77F0(u8*, u32);
    extern u32 fn_801F7824(u8*, u32);
    extern u32 fn_801F7858(u8*);
    extern u8* fn_801F7870(u32);
    extern u32 fn_801F78BC(u8*);
    extern u32 fn_801F78D4(u8*, u32);
    extern u32 fn_801F7908(u8*, u32);
    extern u32 fn_801F793C(u8*);

    if ((u16)slotType == 0 || (u16)slotType >= 0xA) {
        return 0;
    }
    if ((u16)slotType < 4) {
        ptr1 = fn_801F7870(param2);
    }
    if (ptr1 == NULL) {
        return 0;
    }
    switch ((u16)slotType) {
    case 1:
        return (u16)fn_801F7858(ptr1);
    case 2:
        return fn_801F7824(ptr1, (u8)param5);
    case 3:
        return fn_801F77F0(ptr1, (u8)param5);
    case 5:
        return (u16)fn_801F793C(ptr1);
    case 6:
        return fn_801F7908(ptr1, param5);
    case 7:
        return fn_801F78D4(ptr1, param5);
    case 8:
        return (u8)fn_801F78BC(ptr1);
    default:
        return 0;
    }
}

/* 0x801F7798 | size: 0x24 | small */
void fn_801F7798(u8* ptr, u8 idx, u32 val) {
    if (ptr == NULL) return;
    if (idx >= 2) return;
    ptr += (u32)idx * 4;
    *(u32*)(ptr + 0xC) = val;
}

/* 0x801F77BC | size: 0x24 | small */
void fn_801F77BC(u8* ptr, u8 idx, u32 val) {
    if (ptr == NULL) return;
    if (idx >= 2) return;
    ptr += (u32)idx * 4;
    *(u32*)(ptr + 0x4) = val;
}

/* 0x801F77F0 | size: 0x34 | small */
u32 fn_801F77F0(u8* ptr, u8 idx) {
    if (ptr == NULL) return 0;
    if (idx >= 2) return 0;
    ptr += (u32)idx * 4;
    return *(u32*)(ptr + 0xC);
}

/* 0x801F7824 | size: 0x34 | small */
u32 fn_801F7824(u8* ptr, u8 idx) {
    if (ptr == NULL) return 0;
    if (idx >= 2) return 0;
    ptr += (u32)idx * 4;
    return *(u32*)(ptr + 0x4);
}

/* 0x801F7870 | size: 0x2C | small */
u8* fn_801F7870(u16 idx) {
    extern u32 lbl_80478F38;
    extern u32 lbl_80478F3C;
    u32 count = *(u32*)lbl_80478F38;
    if ((u16)idx >= count) return NULL;
    return (u8*)(lbl_80478F3C + (u16)idx * 0x14);
}

/* 0x801F78D4 | size: 0x34 | small */
u8* fn_801F78D4(u8* base, u16 idx) {
    if (base == NULL) return NULL;
    if ((u16)idx >= 2) return NULL;
    return base + (u16)idx * 0x28e4 + 0x64;
}

/* 0x801F7908 | size: 0x34 | small */
u8* fn_801F7908(u8* base, u16 idx) {
    u8* r5;
    r5 = base;
    if (r5 == NULL) return NULL;
    if (idx >= 6) return NULL;
    return r5 + (u32)idx * 16 + 4;
}

/* 0x801F7954 | size: 0x21C | large */
void fn_801F7954(u8* ptr, u8* arr) {
    extern u16 fn_801EF634(void);
    extern u8* fn_801FB1C0(u8*, u32, u16, u32);
    extern u8 fn_8012A130(void);
    extern u8* fn_8012A5B0(u8*, u32, u16);
    extern u8 fn_80123FBC(void);
    extern u32 fn_8012640C(u8*, u32, u16, u32);
    extern u8 fn_80206A04(u8*);
    extern u8 fn_80206608(u8*);
    extern u8* fn_80205BE8(u8*);
    extern u8 fn_80122DDC(u8*);
    u8 r0;
    u8* r30;
    u8* r27;
    u8* r26;
    u16 r31;
    u16 r25;

    if (ptr == NULL) return;
    if ((u16)fn_801EF634() == 1) { r0 = 0; goto L_check; }
    if ((s32)fn_801FB1C0(ptr, 0, 0x43, 0) == 0) { r0 = 0; goto L_check; }
    if (fn_801FB1C0(ptr, 0, 0x44, 0) == NULL) { r0 = 0; goto L_check; }
    if (fn_8012A130() == 0) { r0 = 0; goto L_check; }
    r0 = 1;
L_check:
    if ((u8)r0 == 0) return;
    r30 = fn_801FB1C0(ptr, 0, 0x44, 0);
    {
        u16 j;
        j = 0;
        while ((u16)j < 6) {
            arr[(u16)j] = 0;
            j = j + 1;
        }
    }
    r31 = 0;
    while ((u16)r31 < 6) {
        r27 = fn_8012A5B0(r30, 3, (u16)r31);
        r0 = fn_80123FBC();
        if ((u8)r0 != 0) {
            r25 = 0;
            while ((u16)r25 < 6) {
                r26 = fn_801FB1C0(ptr, 0, 0x45, (u32)(u16)r25);
                r0 = fn_80206A04(r26);
                if ((u8)r0 != 0) {
                    u32 cb = fn_8012640C(r26, 0, 0xcb, 0);
                    if (cb != 0 && r27 == (u8*)cb) {
                        break;
                    }
                }
                r25 = r25 + 1;
            }
            if (r26 == NULL) goto L_next;
            if (fn_80206A04(r26) == 0) goto L_next;
            if ((s32)fn_8012640C(r26, 0, 0xd2, 0) == 1) goto L_next;
            if (fn_80206608(r26) != 0) {
                arr[(u16)r31] = 3;
                goto L_next;
            }
            fn_80205BE8(r26);
            if (fn_80122DDC(r26) != 0) {
                arr[(u16)r31] = 2;
                goto L_next;
            }
            arr[(u16)r31] = 1;
        }
L_next:
        r31 = r31 + 1;
    }
}

/* 0x801F7B70 | size: 0xE4 | medium */
s32 fn_801F7B70(u8* ptr) {
    extern u8* fn_801FB1C0(u8*, u32, u16, u32);
    extern u8* fn_80129BC8(u8*, u32, u16*, u32, u32, u32);
    extern u8 fn_801429E8(u8*);
    extern u16 itemGetStatus(u8*, u32, u16, u32);
    u16 count;
    u8* r31;
    u8* r30;
    u32 r29;
    u8* p44;
    p44 = fn_801FB1C0(ptr, 0, 0x44, 0);
    if (p44 == NULL) return 0;
    r31 = fn_80129BC8(p44, 1, &count, 0, 0, 0);
    if (r31 == NULL) return 0;
    r29 = 0;
    while ((u16)r29 < count) {
        r30 = r31 + (u16)r29 * 4;
        if (fn_801429E8(r30) != 0) {
            if (itemGetStatus(r30, 0, 0x1b, 0) != 0) {
                if (itemGetStatus(r30, 0, 0x1c, 0) != 0) {
                    return 1;
                }
            }
        }
        r29++;
    }
    return 0;
}

/* 0x801F7C54 | size: 0x20C | large */
void fn_801F7C54(u8* ptr, u16* out, u16 count, u8 mode) {
    extern u8* fn_801FB1C0(u8*, u32, u16, u32);
    extern u8* fn_80129BC8(u8*, u32, u16*, u32, u32, u32);
    extern u8 fn_801429E8(u8*);
    extern u16 itemGetStatus(u8*, u32, u16, u32);
    extern u8 fn_802062FC(u8*);
    extern u16 fn_80204C08(u8*);
    u16 sp8;
    u16 spc[2];
    u8* r31;
    u8* r29;
    u8* r23;
    u16 r27;
    u16 r5v;
    u16 r30;
    u16 r28;
    u16 r23i;
    u16 r29i;
    u16 r6i;

    {
        u8* p44 = fn_801FB1C0(ptr, 0, 0x44, 0);
        if (p44 == NULL) { return; }
        r31 = fn_80129BC8(p44, 2, &sp8, 0, 0, 0);
    }
    if (r31 == NULL) { return; }
    for (r23i = 0; (u16)r23i < 2; r23i = r23i + 1) {
        spc[r23i] = 0;
    }
    r30 = 0;
    for (r23i = 0; (u16)r23i < 2; r23i = r23i + 1) {
        r29 = fn_801FB1C0(ptr, 0, 0x46, (u32)(u16)r23i);
        if (fn_802062FC(r29) != 0) {
            r5v = fn_80204C08(r29);
            if ((u16)r5v != 0) {
                spc[r30] = r5v;
                r30 = r30 + 1;
            }
        }
    }
    for (r28 = 0; (u16)r28 < (u16)count; r28 = r28 + 1) {
        out[r28] = 0;
    }
    r28 = 0;
    for (r29i = 0; (u16)r29i < sp8; r29i = r29i + 1) {
        r23 = r31 + (u32)(u16)r29i * 4;
        if (fn_801429E8(r23) != 0) {
            r27 = itemGetStatus(r23, 0, 0x1b, 0);
            if ((u16)r27 != 0) {
                r5v = itemGetStatus(r23, 0, 0x1c, 0);
                if ((u16)r5v != 0) {
                    if ((u8)mode == 1) {
                        for (r6i = 0; (u16)r6i < (u16)r30; r6i = r6i + 1) {
                            if (r27 == spc[r6i]) {
                                if (r27 != (u16)r5v) {
                                    r5v = r5v - 1;
                                }
                            }
                        }
                        if ((u16)r5v == 0) goto next_item;
                    }
                    if ((u16)r28 < (u16)count) {
                        out[r28] = r27;
                        r28 = r28 + 1;
                    }
                }
            }
        }
next_item:;
    }
    (void)r28;
}

/* 0x801F7E60 | size: 0x90 | medium */
s32 fn_801F7E60(u8* ptr) {
    extern u8* fn_801FB1C0(u8*, u32, u16, u32);
    extern u8 fn_802062FC(u8*);
    extern u8 fn_80204A5C(u8*, u32, u32, u32);
    u8* r29;
    u8* r31;
    u32 r30;
    r29 = ptr;
    for (r30 = 0; (u16)r30 < 2; r30++) {
        r31 = fn_801FB1C0(r29, 0, 0x46, r30);
        if (fn_802062FC(r31) != 0) {
            if (fn_80204A5C(r31, 1, 0, 0) == 1) {
                return 1;
            }
        }
    }
    return 0;
}

/* 0x801F7EF0 | size: 0x90 | medium */
s32 fn_801F7EF0(u8* ptr) {
    extern u8* fn_801FB1C0(u8*, u32, u16, u32);
    extern u8 fn_802062FC(u8*);
    extern u8 fn_80204A5C(u8*, u32, u32, u32);
    u8* r29;
    u8* r31;
    u32 r30;
    r29 = ptr;
    for (r30 = 0; (u16)r30 < 2; r30++) {
        r31 = fn_801FB1C0(r29, 0, 0x46, r30);
        if (fn_802062FC(r31) != 0) {
            if (fn_80204A5C(r31, 1, 1, 0) == 1) {
                return 1;
            }
        }
    }
    return 0;
}
