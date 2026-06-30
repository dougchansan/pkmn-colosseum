/**
 * @file people_data.c
 * @brief People/NPC field-level data management -- allocation, lookup,
 *        model loading, accessor functions.
 *
 * This file implements the lower-level People/NPC data layer that manages
 * NPC struct allocation, slot lookup, field getters/setters, and model
 * loading. It sits below the high-level people.c (0x80180C78+) which
 * handles floor-level init/update/spawn, and is called extensively by
 * the script system and battle system.
 *
 * Address range: 0x80140588 - 0x80144574 (approximately 0x3FEC bytes)
 * Function count: ~100 functions (83 of which are tiny getters/setters)
 *
 * Key functions:
 *
 *   fn_80140588 (peopleFieldOpen)      -- 0x514 bytes
 *     Open/spawn an NPC from field data. Takes a PeopleEntry pointer,
 *     group/index pair, spawn data, and a "force" flag. Calls into
 *     itemGetStatus (peopleFieldAlloc) to find or create a slot, loads
 *     the model, and configures the NPC's initial state.
 *     References: lbl_80478BD8 (gPeopleFieldCount),
 *                 lbl_803681E8 (gPeopleFieldLookup),
 *                 lbl_80363CE8 (gPeopleFieldArray)
 *
 *   fn_80140A9C (peopleFieldGetSlot)   -- 0x30 bytes
 *     Simple slot lookup by index. Returns pointer into gPeopleFieldArray.
 *     Called from 3 external sites (effect/shadow modules).
 *
 *   fn_80140ACC (peopleFieldLoadModel) -- 0x83C bytes
 *     Load NPC model and configure animation data. This is a large
 *     function that processes the model resource, sets up joint matrices,
 *     configures walk/run motion data, and initializes the animation
 *     state machine. References gPeopleFieldArray globals.
 *
 *   fn_80141308 (peopleFieldUpdate)    -- 0x1060 bytes (largest in range!)
 *     Per-frame update for a single NPC in the field. Handles state
 *     transitions, animation blending, position interpolation, collision
 *     checks, and rendering submission. This is the core NPC tick function.
 *     References gPeopleFieldArray and lbl_80434E64 (gPeopleFieldWork).
 *
 *   fn_80142368 (peopleFieldCleanup)   -- 0x280 bytes
 *     Cleanup/release an NPC slot. Frees model resources and resets state.
 *
 *   fn_801425E8 (peopleFieldSetup)     -- 0x39C bytes
 *     Configure an NPC's properties after model loading. Sets position,
 *     rotation, scale, motion parameters from spawn data.
 *
 *   fn_80142984 (peopleFieldGetByID)   -- 0x64 bytes
 *     Look up an NPC by its group+index ID pair. Called by 18 external
 *     functions (heavily used by the script system).
 *
 *   fn_801429E8 (peopleFieldGetEntry)  -- 0xA0 bytes
 *     Extended NPC lookup that validates the entry and returns a
 *     PeopleFieldEntry pointer. Called by 28 external functions.
 *
 *   fn_80142A88 (peopleFieldSetState)  -- 0x9C bytes
 *     Set the state of an NPC. Used by battle and cutscene systems.
 *
 *   fn_80142B24 (peopleFieldApplyMotion) -- 0x1D0 bytes
 *     Apply a motion/animation to an NPC. Blends between current and
 *     target animation states.
 *
 *   itemGetStatus (peopleFieldAlloc)     -- 0x204 bytes
 *     Allocate or find an NPC slot. If a slot with the same group+index
 *     already exists, returns it. Otherwise allocates a new slot from
 *     the free pool. Called by 38 external functions.
 *
 *   fn_80142EF8 (peopleFieldRelease)   -- 0x2B4 bytes
 *     Release an NPC slot back to the free pool.
 *
 *   fn_801431AC (peopleFieldInit)      -- 0x4F0 bytes
 *     Initialize the field people system. Allocates the NPC array,
 *     lookup table, and work buffer. Sets up global state.
 *
 * Getter/setter cluster (fn_8014369C - fn_80144064):
 *   83 tiny functions (most 0x10-0x28 bytes each) that provide safe
 *   access to PeopleFieldEntry struct fields. Each follows the pattern:
 *     if (ptr == NULL) return 0;
 *     return ptr->fieldAtOffset;
 *
 *   The struct accessed is PeopleFieldEntry (0x28 bytes per slot),
 *   stored in gPeopleFieldArray (lbl_80363CE8), indexed via
 *   gPeopleFieldLookup (lbl_803681E8).
 *
 *   PeopleFieldEntry layout (from getter offsets):
 *     0x00: f32   field_00       (fn_80143C00 get, fn_80143850 set)
 *     0x04: u8    flags_04       (fn_801436F0 bit test)
 *     0x05: s8    field_05       (fn_801436D4 get)
 *     0x06: s8    field_06       (fn_801436B8 get)
 *     0x07: s8    field_07       (fn_8014369C get)
 *     0x08: (unknown)
 *     0x0C: u8    field_0C       (fn_80143760 get)
 *     0x0D: u8    field_0D       (fn_80143748 get)
 *     0x0E: u8    field_0E       (fn_80143730 get)
 *     0x0F: u8    field_0F       (fn_80143718 get)
 *     0x10: f32   posX           (fn_80143BD0 get, fn_80143C50 set)
 *     0x14: f32   posY           (fn_80143BE0 get, fn_80143C68 set)
 *     0x18: f32   posZ           (fn_80143BF0 get, fn_80143C80 set)
 *     0x1C: f32   rotAngle       (fn_80143C10 get, fn_80143C98 set)
 *     0x20: f32   scale          (fn_80143C20 get, fn_80143CB0 set)
 *     0x24: u32   modelRef       (fn_80143C30 get, fn_80143CE0 set)
 *
 *   fn_801440A0 (peopleFieldGetByIndex) -- 0x50 bytes
 *     The single most-called function in the entire range (48 external
 *     callers). Takes a u16 index, validates against gPeopleFieldCount,
 *     looks up the slot via gPeopleFieldLookup, multiplies by 0x28 to
 *     get the offset into gPeopleFieldArray, and returns the pointer.
 *     Pattern:
 *       if (index >= gPeopleFieldCount) return NULL;
 *       slot = gPeopleFieldLookup[index];
 *       if (slot >= gMaxSlotCount) return NULL;
 *       return &gPeopleFieldArray[slot * 0x28];
 *
 * Global data:
 *   lbl_80478BD8 (sbss) -- u32 gPeopleFieldCount (max index count)
 *   lbl_80478BB0 (sbss) -- u32 gPeopleFieldMaxSlots
 *   lbl_803681E8 (bss)  -- u16[] gPeopleFieldLookup (index -> slot mapping)
 *   lbl_80363CE8 (bss)  -- PeopleFieldEntry[] gPeopleFieldArray (0x28 bytes each)
 *   lbl_80434E64 (bss)  -- PeopleFieldWork (extended work buffer)
 */

#include "dolphin/types.h"
#include "game/people/people.h"

/* ===== External SDK / engine functions ===== */
extern void  fn_800DD970(const char* fmt, ...);     /* OSReport */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* GSmem allocator */
extern u16   fn_800E3534(u32 size);
extern void* fn_800E27B0(u16 handle);

/* Model system */
extern void  fn_800EE150(void* model, u32 param);
extern void  fn_800EE828(void* model, u32 param);
extern void  fn_800E24B0(void* model, u32 param);
extern void  fn_800E209C(void* model, u32 param);
extern void  fn_800E01F4(void* dst, void* src);
extern void  fn_800E01D0(void* dst, void* src);
extern void  fn_800E019C(void* model, void* param);
extern void  fn_800C46B0(void* param1, void* param2);

/* Floor resource system */
extern void* fn_800F9318(u16 group, u16 model, u16 param);

/* Thread/task system */
extern void* fn_800FE834(u32 pri, u32 type, void* buf, void* callback);

/* Interrupt control */
extern u32  OSDisableInterrupts(void);
extern void OSRestoreInterrupts(u32 level);

/* ===================================================================
 * PeopleFieldEntry -- compact NPC data for field rendering.
 *
 * 0x28 bytes per entry. Stored in a flat array at lbl_80363CE8.
 * Accessed via the 83 getter/setter functions at 0x8014369C-0x80144064.
 * =================================================================== */

typedef struct PeopleFieldEntry {
    /* 0x00 */ f32    field_00;
    /* 0x04 */ u8     flags_04;
    /* 0x05 */ s8     field_05;
    /* 0x06 */ s8     field_06;
    /* 0x07 */ s8     field_07;
    /* 0x08 */ u32    field_08;
    /* 0x0C */ u8     field_0C;
    /* 0x0D */ u8     field_0D;
    /* 0x0E */ u8     field_0E;
    /* 0x0F */ u8     field_0F;
    /* 0x10 */ f32    posX;
    /* 0x14 */ f32    posY;
    /* 0x18 */ f32    posZ;
    /* 0x1C */ f32    rotAngle;
    /* 0x20 */ f32    scale;
    /* 0x24 */ u32    modelRef;
} PeopleFieldEntry;

/* ===================================================================
 * DECOMPILED: fn_801440A0 -- peopleFieldGetByIndex
 *
 * The most-called function in the entire range (48 external callers).
 * Looks up a PeopleFieldEntry by its u16 index.
 * =================================================================== */

/* Global state (sdata/sbss) */
extern u32 lbl_80478BD8;   /* gPeopleFieldCount */
extern u32 lbl_80478BB0;   /* gPeopleFieldMaxSlots */
extern u16 lbl_803681E8[]; /* gPeopleFieldLookup */
extern u8  lbl_80363CE8[]; /* gPeopleFieldArray base */

PeopleFieldEntry* peopleFieldGetByIndex(u16 index) {
    u16 slot;

    if (index >= lbl_80478BD8) {
        return NULL;
    }

    slot = lbl_803681E8[index];
    if (slot >= lbl_80478BB0) {
        return NULL;
    }

    return (PeopleFieldEntry*)(&lbl_80363CE8[slot * 0x28]);
}

/* ===================================================================
 * STUB DECLARATIONS -- remaining functions
 *
 * Full decompilation deferred; the asm files remain authoritative.
 * Function addresses and proposed names documented for cross-reference.
 * =================================================================== */

/* fn_80140588: peopleFieldOpen (0x514 bytes) */
/* fn_80140A9C: peopleFieldGetSlot (0x30 bytes) */
/* fn_80140ACC: peopleFieldLoadModel (0x83C bytes) */
/* fn_80141308: peopleFieldUpdate (0x1060 bytes -- largest single function) */
/* fn_80142368: peopleFieldCleanup (0x280 bytes) */
/* fn_801425E8: peopleFieldSetup (0x39C bytes) */
/* fn_80142984: peopleFieldGetByID (0x64 bytes) */
/* fn_801429E8: peopleFieldGetEntry (0xA0 bytes) */
/* fn_80142A88: peopleFieldSetState (0x9C bytes) */
/* fn_80142B24: peopleFieldApplyMotion (0x1D0 bytes) */
/* itemGetStatus: peopleFieldAlloc (0x204 bytes) */
/* fn_80142EF8: peopleFieldRelease (0x2B4 bytes) */
/* fn_801431AC: peopleFieldInit (0x4F0 bytes) */

/* --- Getter/Setter cluster (83 functions) --- */
/* fn_8014369C: getField07 */
/* fn_801436B8: getField06 */
/* fn_801436D4: getField05 */
/* fn_801436F0: testFlags04 */
/* fn_80143718: getField0F */
/* fn_80143730: getField0E */
/* fn_80143748: getField0D */
/* fn_80143760: getField0C */
/* fn_80143778: getField08_lo (bit extract) */
/* fn_801437A0: getField08_hi */
/* fn_801437B8: setField08_lo */
/* fn_801437E0: getField09 */
/* fn_801437F8: setField09 */
/* fn_80143820: getField0A */
/* fn_80143838: getField0B */
/* fn_80143850: setField00 (f32) */
/* fn_80143878: setField04_bit0 */
/* fn_801438A0: setField04_bit1 */
/* fn_801438C8: setField04_bit2 */
/* fn_801438F0: setField04_bit3 */
/* fn_80143918: setField05 (s8) */
/* fn_80143940: setField06 (s8) */
/* fn_80143968: setField07 (s8) */
/* fn_80143990: setField08 (u32) */
/* fn_801439B8: setField0C (u8) */
/* fn_801439D4: setField0D (u8) */
/* fn_801439F0: setField0E (u8) */
/* fn_80143A0C: setField0F (u8) */
/* fn_80143A28: clearFlags04 */
/* fn_80143A44: setFlags04 */
/* fn_80143A6C: setField_special1 */
/* fn_80143A94: setField_special2 */
/* fn_80143ABC: setField_special3 */
/* fn_80143AF0: getField_special1 */
/* fn_80143B08: getField_special2 */
/* fn_80143B30: getField_special3 */
/* fn_80143B48: setField_special4 */
/* fn_80143B70: getField10_int (posX as int) */
/* fn_80143B80: getField14_int (posY as int) */
/* fn_80143B90: getField18_int (posZ as int) */
/* fn_80143BA0: getField1C_int (rotAngle as int) */
/* fn_80143BB0: setField10_int */
/* fn_80143BD0: getPosX (f32) */
/* fn_80143BE0: getPosY (f32) */
/* fn_80143BF0: getPosZ (f32) */
/* fn_80143C00: getField00 (f32) */
/* fn_80143C10: getRotAngle (f32) */
/* fn_80143C20: getScale (f32) */
/* fn_80143C30: getModelRef (u32) */
/* fn_80143C40: getField24_byte */
/* fn_80143C50: setPosX (f32) */
/* fn_80143C68: setPosY (f32) */
/* fn_80143C80: setPosZ (f32) */
/* fn_80143C98: setRotAngle (f32) */
/* fn_80143CB0: setScale (f32) */
/* fn_80143CC8: setModelRef */
/* fn_80143CE0: setField24 */
/* fn_80143CF8: setField24_byte */
/* fn_80143D10: getField_ext1 */
/* fn_80143D28: getField_ext2 */
/* fn_80143D40: getField_ext3 */
/* fn_80143D58: getField_ext4 */
/* fn_80143D70: getField_ext5 */
/* fn_80143D88: getField_ext6 */
/* fn_80143DA0: getField_ext7 (with extra logic) */
/* fn_80143DCC: getField_ext8 */
/* fn_80143DE4: getField_ext9 */
/* fn_80143DFC: getField_ext10 */
/* fn_80143E14: getField_ext11 */
/* fn_80143E2C: setField_ext1 */
/* fn_80143E60: setField_ext2 */
/* fn_80143E88: setField_ext3 (0x68 bytes) */
/* fn_80143EF0: setField_ext4 */
/* fn_80143F24: setField_ext5 */
/* fn_80143F54: getField_ext12 */
/* fn_80143F6C: getField_ext13 */
/* fn_80143F84: getField_ext14 */
/* fn_80143F9C: getField_ext15 */
/* fn_80143FB4: getField_ext16 */
/* fn_80143FCC: getField_ext17 */
/* fn_80143FE4: getField_ext18 */
/* fn_80143FFC: getField_ext19 */
/* fn_80144014: getField_ext20 */
/* fn_8014402C: setField_ext6 */
/* fn_80144064: setField_ext7 */
/* fn_80144088: peopleFieldQuery (0x18 bytes) -- quick status check */

/* fn_801440F0: peopleFieldOpenModel (0xB8 bytes) */
/* fn_801441A8: peopleFieldConfigModel (0x224 bytes) */
/* fn_801443CC: peopleFieldFinalizeModel (0x1A8 bytes) */
/* renamed symbols referenced by asm incs (symbolmap port) */
/* Forward declarations for self-referencing asm blocks */
extern void fn_801425E8(void);
extern void fn_80142B24(void*, u32, u16, u32, u32);
extern s32 itemGetStatus(u32, u16, u16, u32);
extern u16  fn_80143B30(u8* p);
extern u8*  fn_80143B48(u16 idx);
extern void fn_80143B70(u8* p, u16 val);
extern void fn_80143B80(u8* p, u16 val);
extern void fn_80143B90(u8* p, u16 val);
extern void fn_80143BA0(u8* p, u32 val);
extern void fn_80143BB0(u8* p, u16 idx, u8 val);
extern void fn_80143BD0(u8* p, u16 val);
extern void fn_80143BE0(u8* p, u16 val);
extern void fn_80143BF0(u8* p, u32 val);
extern void fn_80143C00(u8* p, u8 val);
extern void fn_80143C10(u8* p, u8 val);
extern void fn_80143C20(u8* p, u16 val);
extern void fn_80143C30(u8* p, u8 val);
extern void fn_80143C40(u8* p, u32 val);

#if 0
asm void fn_80140A9C(void) {
#include "src/game/people/people_data_fn_80140A9C.inc"
}
#else
#pragma optimization_level 4
void fn_80140A9C(u32* a, u32* b) {
    volatile u32 new_var;
    u32 tmp;
    if (a == NULL) return;
    if (b == NULL) return;
    tmp = (new_var = *a);
    *a = *b;
    *b = tmp;
}
#endif
extern u32 lbl_80478BD8;   /* gPeopleFieldCount */
#if 1
asm void fn_80140ACC(void) {
#include "src/game/people/people_data_fn_80140ACC.inc"
}
#else
void fn_80140ACC(void) { /* TODO */ }
#endif
extern u32 lbl_80478BD8;   /* gPeopleFieldCount */
#if 1
asm void fn_80141308(void) {
#include "src/game/people/people_data_fn_80141308.inc"
}
#else
void fn_80141308(void) { /* TODO */ }
#endif
extern u32 lbl_80478BD8;   /* gPeopleFieldCount */
#if 1
asm void fn_80142368(void) {
#include "src/game/people/people_data_fn_80142368.inc"
}
#else
void fn_80142368(void) { /* TODO */ }
#endif
extern u32 lbl_80478BD8;   /* gPeopleFieldCount */
#if 1
asm void fn_801425E8(void) {
#include "src/game/people/people_data_fn_801425E8.inc"
}
#else
void fn_801425E8(void) { /* TODO */ }
#endif
extern u32 lbl_80478BD8;   /* gPeopleFieldCount */
#if 0
asm void fn_80142984(void) {
#include "src/game/people/people_data_fn_80142984.inc"
}
#else
#pragma optimization_level 4
s32 fn_80142984(u16 id) {
    s32 r;

    r = itemGetStatus(0, id, 1, 0);
    if (r == 0) return 0;
    return lbl_80478BD8 > (u16)id;
}
#endif
extern u32 lbl_80478BD8;   /* gPeopleFieldCount */
#if 0
asm void fn_801429E8(void) {
#include "src/game/people/people_data_fn_801429E8.inc"
}
#else
#pragma optimization_level 4
s32 fn_801429E8(u32 arg) {
    u16 r31;
    u8 valid;

    if (arg == 0) return 0;

    r31 = (u16)itemGetStatus(arg, 0, 0x1b, 0);
    if (r31 == 0) return 0;

    if (itemGetStatus(0, r31, 1, 0) == 0) {
        valid = 0;
    } else if (r31 >= lbl_80478BD8) {
        valid = 0;
    } else {
        valid = 1;
    }

    return valid != 0;
}
#endif
#if 0
asm void fn_80142A88(void) {
#include "src/game/people/people_data_fn_80142A88.inc"
}
#else
#pragma optimization_level 4
void fn_80142A88(u32* base, u16 count) {
    u32* ptr;
    u16 i;

    if (base == NULL) return;

    for (i = 0; (u16)i < (u16)count; i++) {
        if ((ptr = base + i) != NULL) {
            fn_80142B24((void*)ptr, 0, 0x1b, 0, 0);
            fn_80142B24((void*)ptr, 0, 0x1c, 0, 0);
        }
    }
}
#endif
extern void fn_801440A0(void);
extern void fn_80143DA0(void);
extern void fn_8020A328(void);
extern void fn_8020A318(void);
extern void fn_8020A308(void);
extern void fn_8020A2F8(void);
extern void jumptable_80367D60();
#if 1
asm void fn_80142B24(void* p, u32 a, u16 b, u32 c, u32 d) {
#include "src/game/people/people_data_fn_80142B24.inc"
}
#else
void fn_80142B24(void* p, u32 a, u16 b, u32 c, u32 d) { /* TODO */ }
#endif
extern void fn_80144088(void);
extern void fn_80144014(void);
extern void fn_80143FFC(void);
extern void fn_80143FCC(void);
extern void fn_80143FB4(void);
extern void fn_80143F84(void);
extern void fn_80143F6C(void);
extern void fn_80143F54(void);
extern void fn_80143E2C(void);
extern void fn_80143E14(void);
extern void fn_80143D88(void);
extern void fn_80143D70(void);
extern void fn_80143D58(void);
extern void fn_80143D40(void);
extern void fn_80143D28(void);
extern void fn_80143D10(void);
extern void fn_80143CF8(void);
extern void fn_80143CE0(void);
extern void fn_80143CC8(void);
extern void fn_80143CB0(void);
extern void fn_80143C98(void);
extern void fn_80143C80(void);
extern void fn_80143C68(void);
extern void fn_80143C50(void);
extern void fn_8020A380(void);
extern void fn_8020A368(void);
extern void fn_8020A350(void);
extern void fn_8020A338(void);
extern void jumptable_80367DE8();
#if 1
asm s32 itemGetStatus(u32 a, u16 b, u16 c, u32 d) {
#include "src/game/people/people_data_fn_80142CF4.inc"
}
#else
s32 itemGetStatus(u32 a, u16 b, u16 c, u32 d) { /* TODO */ return 0; }
#endif
extern void jumptable_80367E70();
extern u8 lbl_802730E0[];
#if 1
asm void fn_80142EF8(void) {
#include "src/game/people/people_data_fn_80142EF8.inc"
}
#else
void fn_80142EF8(void) { /* TODO */ }
#endif
#if 0
asm void fn_801431AC(void) {
#include "src/game/people/people_data_fn_801431AC.inc"
}
#else
s32 itemParamGetRecoverType(u8* itemParam) {
    s32 selectedType;
    s32 selectedCount;
    s32 returnType;
    u32 bitValue;
    u8 firstGate;
    u8 statusNibble;
    u8 byteFlag;

    if (itemParam == NULL) {
        return 0x16;
    }

    if (itemParam == NULL) {
        firstGate = 0;
    } else {
        bitValue = (itemParam[0] >> 5) & 1;
        firstGate = ((-bitValue | bitValue) >> 31);
    }
    if (firstGate) {
        goto returnZero;
    }

    if (itemParam == NULL) {
        statusNibble = 0;
    } else {
        statusNibble = (itemParam[0] >> 1) & 0xF;
    }
    if (statusNibble) {
        goto returnZero;
    }

    if (itemParam == NULL) {
        statusNibble = 0;
    } else {
        statusNibble = (itemParam[1] >> 4) & 0xF;
    }
    if (statusNibble) {
        goto returnZero;
    }

    if (itemParam == NULL) {
        statusNibble = 0;
    } else {
        statusNibble = itemParam[1] & 0xF;
    }
    if (statusNibble) {
        goto returnZero;
    }

    if (itemParam == NULL) {
        statusNibble = 0;
    } else {
        statusNibble = (itemParam[2] >> 4) & 0xF;
    }
    if (statusNibble) {
        goto returnZero;
    }

    if (itemParam == NULL) {
        statusNibble = 0;
    } else {
        statusNibble = itemParam[2] & 0xF;
    }
    if (statusNibble) {
        goto returnZero;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 7) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (!byteFlag) {
        goto afterReturnZero;
    }
returnZero:
    return 0;
afterReturnZero:

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[0] >> 6) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        return 1;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 6) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        return 2;
    }

    selectedCount = 0;

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 5) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 3;
        selectedCount = 1;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 4) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 4;
        selectedCount++;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 3) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 5;
        selectedCount++;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 2) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 6;
        selectedCount++;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 1) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 7;
        selectedCount++;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = itemParam[3] & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 8;
        selectedCount++;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[0] >> 7) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 9;
        selectedCount++;
    }

    if (selectedCount > 0) {
        returnType = selectedType;
        if (selectedCount > 1) {
            returnType = 0xA;
        }
        return returnType;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[0xA];
    }
    if (byteFlag) {
        return 0xB;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[9];
    }
    if (byteFlag) {
        return 0xC;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[8];
    }
    if (byteFlag) {
        return 0xD;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[0xF];
    }
    if (byteFlag) {
        return 0xE;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[0xE];
    }
    if (byteFlag) {
        return 0xF;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[0xD];
    }
    if (byteFlag) {
        return 0x10;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[0xC];
    }
    if (byteFlag) {
        return 0x11;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[4] >> 4) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        return 0x12;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[4] >> 7) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        return 0x13;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[4] >> 3) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        return 0x14;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[4] >> 5) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        goto returnFifteen;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[0xB];
    }
    if (!byteFlag) {
        goto returnSixteen;
    }

returnFifteen:
    return 0x15;
returnSixteen:
    return 0x16;
}
#endif
#if 0
asm void fn_801436B8(void) {
#include "src/game/people/people_data_fn_801436B8.inc"
}
#else
#pragma optimization_level 4
s8 fn_801436B8(u8* p) {
    if (p == NULL) return 0;
    return (s8)p[0x6];
}
#endif
#if 0
asm void fn_801436D4(void) {
#include "src/game/people/people_data_fn_801436D4.inc"
}
#else
#pragma optimization_level 4
s8 fn_801436D4(u8* p) {
    if (p == NULL) return 0;
    return (s8)p[0x5];
}
#endif
#if 0
asm void fn_801436F0(void) {
#include "src/game/people/people_data_fn_801436F0.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_801436F0(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x4] >> 3) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_80143718(void) {
#include "src/game/people/people_data_fn_80143718.inc"
}
#else
#pragma optimization_level 4
u8 fn_80143718(u8* p) {
    if (p == NULL) return 0;
    return p[0xf];
}
#endif
#if 0
asm void fn_80143730(void) {
#include "src/game/people/people_data_fn_80143730.inc"
}
#else
#pragma optimization_level 4
u8 fn_80143730(u8* p) {
    if (p == NULL) return 0;
    return p[0xe];
}
#endif
#if 0
asm void fn_80143748(void) {
#include "src/game/people/people_data_fn_80143748.inc"
}
#else
#pragma optimization_level 4
u8 fn_80143748(u8* p) {
    if (p == NULL) return 0;
    return p[0xd];
}
#endif
#if 0
asm void fn_80143760(void) {
#include "src/game/people/people_data_fn_80143760.inc"
}
#else
#pragma optimization_level 4
u8 fn_80143760(u8* p) {
    if (p == NULL) return 0;
    return p[0xc];
}
#endif
#if 0
asm void fn_80143778(void) {
#include "src/game/people/people_data_fn_80143778.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_80143778(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x4] >> 4) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_801437A0(void) {
#include "src/game/people/people_data_fn_801437A0.inc"
}
#else
#pragma optimization_level 4
u8 fn_801437A0(u8* p) {
    if (p == NULL) return 0;
    return p[0xb];
}
#endif
#if 0
asm void fn_801437B8(void) {
#include "src/game/people/people_data_fn_801437B8.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_801437B8(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x4] >> 5) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_801437E0(void) {
#include "src/game/people/people_data_fn_801437E0.inc"
}
#else
#pragma optimization_level 4
u8 fn_801437E0(u8* p) {
    if (p == NULL) return 0;
    return p[0xa];
}
#endif
#if 0
asm void fn_801437F8(void) {
#include "src/game/people/people_data_fn_801437F8.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_801437F8(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x4] >> 6) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_80143820(void) {
#include "src/game/people/people_data_fn_80143820.inc"
}
#else
#pragma optimization_level 4
u8 fn_80143820(u8* p) {
    if (p == NULL) return 0;
    return p[0x9];
}
#endif
#if 0
asm void fn_80143838(void) {
#include "src/game/people/people_data_fn_80143838.inc"
}
#else
#pragma optimization_level 4
u8 fn_80143838(u8* p) {
    if (p == NULL) return 0;
    return p[0x8];
}
#endif
#if 0
asm void fn_80143850(void) {
#include "src/game/people/people_data_fn_80143850.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_80143850(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x4] >> 7) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_80143878(void) {
#include "src/game/people/people_data_fn_80143878.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_80143878(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!(p[0x3] & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_801438A0(void) {
#include "src/game/people/people_data_fn_801438A0.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_801438A0(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 1) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_801438C8(void) {
#include "src/game/people/people_data_fn_801438C8.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_801438C8(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 2) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_801438F0(void) {
#include "src/game/people/people_data_fn_801438F0.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_801438F0(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 3) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_80143918(void) {
#include "src/game/people/people_data_fn_80143918.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_80143918(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 4) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_80143940(void) {
#include "src/game/people/people_data_fn_80143940.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_80143940(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 5) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_80143968(void) {
#include "src/game/people/people_data_fn_80143968.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_80143968(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 6) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_80143990(void) {
#include "src/game/people/people_data_fn_80143990.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_80143990(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 7) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_801439B8(void) {
#include "src/game/people/people_data_fn_801439B8.inc"
}
#else
#pragma optimization_level 4
u32 fn_801439B8(u8* p) {
    if (p == NULL) return 0;
    return (u32)(p[0x2] & 0xF);
}
#endif
#if 0
asm void fn_801439D4(void) {
#include "src/game/people/people_data_fn_801439D4.inc"
}
#else
#pragma optimization_level 4
u32 fn_801439D4(u8* p) {
    if (p == NULL) return 0;
    return (u32)((p[0x2] >> 4) & 0xF);
}
#endif
#if 0
asm void fn_801439F0(void) {
#include "src/game/people/people_data_fn_801439F0.inc"
}
#else
#pragma optimization_level 4
u32 fn_801439F0(u8* p) {
    if (p == NULL) return 0;
    return (u32)(p[0x1] & 0xF);
}
#endif
#if 0
asm void fn_80143A0C(void) {
#include "src/game/people/people_data_fn_80143A0C.inc"
}
#else
#pragma optimization_level 4
u32 fn_80143A0C(u8* p) {
    if (p == NULL) return 0;
    return (u32)((p[0x1] >> 4) & 0xF);
}
#endif
#if 0
asm void fn_80143A28(void) {
#include "src/game/people/people_data_fn_80143A28.inc"
}
#else
#pragma optimization_level 4
u32 fn_80143A28(u8* p) {
    if (p == NULL) return 0;
    return (u32)((p[0x0] >> 1) & 0xF);
}
#endif
#if 0
asm void fn_80143A44(void) {
#include "src/game/people/people_data_fn_80143A44.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_80143A44(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x0] >> 5) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void fn_80143A6C(void) {
#include "src/game/people/people_data_fn_80143A6C.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_80143A6C(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x0] >> 7) & 1);
    return v;
}
#pragma pop
#endif
extern u32 lbl_80478BE0;
extern u8 lbl_80368630[];
#if 0
asm void fn_80143A94(void) {
#include "src/game/people/people_data_fn_80143A94.inc"
}
#else
#pragma optimization_level 4
u8* fn_80143A94(u8 idx) {
    u8* result = &lbl_80368630[(u8)idx * 16];
    if (idx < lbl_80478BE0) return result;
    return NULL;
}
#endif
#if 0
asm void fn_80143ABC(void) {
#include "src/game/people/people_data_fn_80143ABC.inc"
}
#else
#pragma optimization_level 4
s8 fn_80143ABC(u8* p, u16 idx) {
    if (p == NULL) return 0;
    if ((u32)(idx & 0xFFFF) >= 0x19) return 0;
    return (s8)(p[idx + 4]);
}
#endif
#if 0
asm void fn_80143AF0(void) {
#include "src/game/people/people_data_fn_80143AF0.inc"
}
#else
#pragma optimization_level 4
u32 fn_80143AF0(u8* p) {
    if (p == NULL) return 0;
    return *(u32*)p;
}
#endif
extern u32 lbl_80478BC8;
extern u8 lbl_80367F78[];
#if 0
asm void fn_80143B08(void) {
#include "src/game/people/people_data_fn_80143B08.inc"
}
#else
#pragma optimization_level 4
u8* fn_80143B08(u16 idx) {
    u8* result = &lbl_80367F78[(u16)idx * 32];
    if ((u16)idx < lbl_80478BC8) return result;
    return NULL;
}
#endif
#if 0
asm void fn_80143B30(void) {
#include "src/game/people/people_data_fn_80143B30.inc"
}
#else
#pragma optimization_level 4
u16 fn_80143B30(u8* p) {
    if (p == NULL) return 0;
    return *(u16*)p;
}
#endif
extern u32 lbl_80478BC0;
extern u8 lbl_80367EF0[];
#if 0
asm void fn_80143B48(void) {
#include "src/game/people/people_data_fn_80143B48.inc"
}
#else
#pragma optimization_level 4
u8* fn_80143B48(u16 idx) {
    u8* result = &lbl_80367EF0[(u16)idx * 2];
    if ((u16)idx < lbl_80478BC0) return result;
    return NULL;
}
#endif
#if 0
asm void fn_80143B70(void) {
#include "src/game/people/people_data_fn_80143B70.inc"
}
#else
#pragma optimization_level 4
void fn_80143B70(u8* p, u16 val) {
    if (p == NULL) return;
    *(u16*)(p + 0x2) = val;
}
#endif
#if 0
asm void fn_80143B80(void) {
#include "src/game/people/people_data_fn_80143B80.inc"
}
#else
#pragma optimization_level 4
void fn_80143B80(u8* p, u16 val) {
    if (p == NULL) return;
    *(u16*)(p + 0x0) = val;
}
#endif
#if 0
asm void fn_80143B90(void) {
#include "src/game/people/people_data_fn_80143B90.inc"
}
#else
#pragma optimization_level 4
void fn_80143B90(u8* p, u16 val) {
    if (p == NULL) return;
    *(u16*)(p + 0x0) = val;
}
#endif
#if 0
asm void fn_80143BA0(void) {
#include "src/game/people/people_data_fn_80143BA0.inc"
}
#else
#pragma optimization_level 4
void fn_80143BA0(u8* p, u32 val) {
    if (p == NULL) return;
    *(u32*)(p + 0x18) = val;
}
#endif
#if 0
asm void fn_80143BB0(void) {
#include "src/game/people/people_data_fn_80143BB0.inc"
}
#else
#pragma optimization_level 4
void fn_80143BB0(u8* p, u16 idx, u8 val) {
    u32 i;
    if (p == NULL) return;
    i = (u32)(idx & 0xFFFF);
    if (i >= 3) return;
    p[i + 0x24] = val;
}
#endif
#if 0
asm void fn_80143BD0(void) {
#include "src/game/people/people_data_fn_80143BD0.inc"
}
#else
#pragma optimization_level 4
void fn_80143BD0(u8* p, u16 val) {
    if (p == NULL) return;
    *(u16*)(p + 0xc) = val;
}
#endif
#if 0
asm void fn_80143BE0(void) {
#include "src/game/people/people_data_fn_80143BE0.inc"
}
#else
#pragma optimization_level 4
void fn_80143BE0(u8* p, u16 val) {
    if (p == NULL) return;
    *(u16*)(p + 0xa) = val;
}
#endif
#if 0
asm void fn_80143BF0(void) {
#include "src/game/people/people_data_fn_80143BF0.inc"
}
#else
#pragma optimization_level 4
void fn_80143BF0(u8* p, u32 val) {
    if (p == NULL) return;
    *(u32*)(p + 0x14) = val;
}
#endif
#if 0
asm void fn_80143C00(void) {
#include "src/game/people/people_data_fn_80143C00.inc"
}
#else
#pragma optimization_level 4
void fn_80143C00(u8* p, u8 val) {
    if (p == NULL) return;
    p[0x2] = val;
}
#endif
#if 0
asm void fn_80143C10(void) {
#include "src/game/people/people_data_fn_80143C10.inc"
}
#else
#pragma optimization_level 4
void fn_80143C10(u8* p, u8 val) {
    if (p == NULL) return;
    p[0x1] = val;
}
#endif
#if 0
asm void fn_80143C20(void) {
#include "src/game/people/people_data_fn_80143C20.inc"
}
#else
#pragma optimization_level 4
void fn_80143C20(u8* p, u16 val) {
    if (p == NULL) return;
    *(u16*)(p + 0x6) = val;
}
#endif
#if 0
asm void fn_80143C30(void) {
#include "src/game/people/people_data_fn_80143C30.inc"
}
#else
#pragma optimization_level 4
void fn_80143C30(u8* p, u8 val) {
    if (p == NULL) return;
    p[0x0] = val;
}
#endif
#if 0
asm void fn_80143C40(void) {
#include "src/game/people/people_data_fn_80143C40.inc"
}
#else
#pragma optimization_level 4
void fn_80143C40(u8* p, u32 val) {
    if (p == NULL) return;
    *(u32*)(p + 0x10) = val;
}
#endif
