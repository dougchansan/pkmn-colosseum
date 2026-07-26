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
 * Note: itemDataBiosGetPtr was previously defined here under the invented name
 * "peopleFieldGetByIndex"; it has been renamed to match the unmatched
 * fn_ slot it actually fills (address + exact size match, and the
 * "peopleFieldGetByIndex" label is independently corroborated by a
 * comment in people_field.c).
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
 * Getter/setter cluster (itemParamGetFriend3Up - itemDataBiosCheckImportable):
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
 *     0x00: f32   field_00       (itemDataBiosSetUseful get, itemParamGetPPMaxUpFlag set)
 *     0x04: u8    flags_04       (itemParamGetPPMaxFullFlag bit test)
 *     0x05: s8    field_05       (itemParamGetFriend1Up get)
 *     0x06: s8    field_06       (itemParamGetFriend2Up get)
 *     0x07: s8    field_07       (itemParamGetFriend3Up get)
 *     0x08: (unknown)
 *     0x0C: u8    field_0C       (itemParamGetDefenceEffortUp get)
 *     0x0D: u8    field_0D       (itemParamGetQuickEffortUp get)
 *     0x0E: u8    field_0E       (itemParamGetSpDefenceEffortUp get)
 *     0x0F: u8    field_0F       (itemParamGetSpAttackEffortUp get)
 *     0x10: f32   posX           (itemDataBiosSetFightUseKoukaDataId get, itemBiosGetNum set)
 *     0x14: f32   posY           (itemDataBiosSetItemSoubiDataId get, itemBiosGetItemDataId set)
 *     0x18: f32   posZ           (itemDataBiosSetDoc get, itemBallDataBiosGetSnatchSnatchWzxDataId set)
 *     0x1C: f32   rotAngle       (itemDataBiosSetImportant get, itemBallDataBiosGetSnatchShakeWzxDataId set)
 *     0x20: f32   scale          (itemDataBiosSetPrice get, itemBallDataBiosGetSnatchPokeoutWzxDataId set)
 *     0x24: u32   modelRef       (itemDataBiosSetKind get, itemBallDataBiosGetSnatchBalllandWzxDataId set)
 *
 *   itemDataBiosGetPtr (peopleFieldGetByIndex) -- 0x50 bytes
 *     The single most-called function in the entire range (48 external
 *     callers). Takes a u16 index, validates against gPeopleFieldCount,
 *     looks up the slot via gPeopleFieldLookup, multiplies by 0x28 to
 *     get the offset into gPeopleFieldArray, and returns the pointer.
 *     Pattern:
 *       if (index >= gPeopleFieldCount) return NULL;
 *       slot = gPeopleFieldLookup[index];
 *       if (slot >= gMaxSlotCount) return NULL;
 *       return &gPeopleFieldArray[slot];
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
extern void  GSlogWrite(const char* fmt, ...);     /* OSReport */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* GSmem allocator */
extern u16   _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u16 handle);

/* Model system */
extern void  GSmodelGetPart(void* model, u32 param);
extern void  GSpartFree(void* model, u32 param);
extern void  fn_800E24B0(u16 handle);
extern void  fn_800E209C(u16 handle);
extern void  set__5GSvecFfff(void* dst, void* src);
extern void  GSvecCopy(void* dst, void* src);
extern void  GSvecAdd(void* model, void* param);
extern void  __cvt_fp2unsigned(void* param1, void* param2);

/* Floor resource system */
extern void* GSresGetResource(u16 group, u16 model, u16 param);
extern void* fightFloorGetNowPtr(void);
extern s32   fn_80144574(void* arg0, void* arg1, void* arg2, void* arg3, void* arg4);

/* Thread/task system */
extern void* GSgappCreate(u32 pri, u32 type, void* buf, void* callback);

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

typedef struct ItemParamData {
    /* 0x00 */ u8  recoveryAndAttackFlags;
    /* 0x01 */ u8  defenceQuickUp;
    /* 0x02 */ u8  hitSpAttackUp;
    /* 0x03 */ u8  statusFlags;
    /* 0x04 */ u8  miscFlags;
    /* 0x05 */ s8  friend1Up;
    /* 0x06 */ s8  friend2Up;
    /* 0x07 */ s8  friend3Up;
    /* 0x08 */ u8  hpEffortUp;
    /* 0x09 */ u8  attackEffortUp;
    /* 0x0A */ u8  hpUp;
    /* 0x0B */ u8  ppUp;
    /* 0x0C */ u8  defenceEffortUp;
    /* 0x0D */ u8  quickEffortUp;
    /* 0x0E */ u8  spDefenceEffortUp;
    /* 0x0F */ u8  spAttackEffortUp;
} ItemParamData;

typedef struct ItemBiosData {
    /* 0x00 */ u16 itemDataId;
    /* 0x02 */ u16 num;
} ItemBiosData;

typedef struct ItemBallDataBios {
    /* 0x00 */ u16 fightKoukaDataId;
    /* 0x02 */ u16 padding_02;
    /* 0x04 */ u32 inWzxDataId;
    /* 0x08 */ u32 openWzxDataId;
    /* 0x0C */ u32 outWzxDataId;
    /* 0x10 */ u32 downinWzxDataId;
    /* 0x14 */ u32 throwWzxDataId;
    /* 0x18 */ u32 snatchAttackWzxDataId;
    /* 0x1C */ u32 snatchBalllandWzxDataId;
    /* 0x20 */ u32 snatchMissWzxDataId;
    /* 0x24 */ u32 snatchPokeoutWzxDataId;
    /* 0x28 */ u32 snatchShakeWzxDataId;
    /* 0x2C */ u32 snatchSnatchWzxDataId;
} ItemBallDataBios;

typedef struct ItemDataBios {
    /* 0x00 */ u8  kind;
    /* 0x01 */ u8  important;
    /* 0x02 */ u8  useful;
    /* 0x03 */ u8  field_03;
    /* 0x04 */ u8  itemEffectParam;
    /* 0x05 */ u8  padding_05;
    /* 0x06 */ u16 price;
    /* 0x08 */ u16 coupon;
    /* 0x0A */ u16 itemSoubiDataId;
    /* 0x0C */ u16 fightUseKoukaDataId;
    /* 0x0E */ u16 padding_0E;
    /* 0x10 */ u32 name;
    /* 0x14 */ u32 doc;
    /* 0x18 */ u32 buff;
    /* 0x1C */ u32 fieldUseFunc;
    /* 0x20 */ u32 battleUseFunc;
    /* 0x24 */ s8  useFriend[3];
    /* 0x27 */ u8  padding_27;
} ItemDataBios;

typedef struct WazaMachineData {
    /* 0x00 */ u32 field_00;
    /* 0x04 */ u32 wazaId;
} WazaMachineData;

/* ===================================================================
 * DECOMPILED: itemDataBiosGetPtr (aka "peopleFieldGetByIndex" in cross-file
 * comments, e.g. people_field.c)
 *
 * The most-called function in the entire range (48 external callers).
 * Looks up a PeopleFieldEntry by its u16 index.
 * =================================================================== */

/* Global state (sdata/sbss) */
extern u32 lbl_80478BD8;   /* gPeopleFieldCount */
extern u32 lbl_80478BB0;   /* gPeopleFieldMaxSlots */
extern u16 lbl_803681E8[]; /* gPeopleFieldLookup */
extern PeopleFieldEntry lbl_80363CE8[]; /* gPeopleFieldArray */

/* Returns u8* (not PeopleFieldEntry*) to match the extern declaration
 * used at this function's call sites elsewhere in this file (e.g. the
 * fn_80142B24 dispatcher below). */
u8* itemDataBiosGetPtr(u16 index) {
    u16 slot;

    if (index >= lbl_80478BD8) {
        return NULL;
    }

    slot = lbl_803681E8[index];
    if (slot >= lbl_80478BB0) {
        return NULL;
    }

    return (u8*)&lbl_80363CE8[slot];
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
/* itemParamGetFriend3Up: getField07 */
/* itemParamGetFriend2Up: getField06 */
/* itemParamGetFriend1Up: getField05 */
/* itemParamGetPPMaxFullFlag: testFlags04 */
/* itemParamGetSpAttackEffortUp: getField0F */
/* itemParamGetSpDefenceEffortUp: getField0E */
/* itemParamGetQuickEffortUp: getField0D */
/* itemParamGetDefenceEffortUp: getField0C */
/* itemParamGetEvolutionFlag: getField08_lo (bit extract) */
/* itemParamGetPPUp: getField08_hi */
/* itemParamGetPPSelectFlag: setField08_lo */
/* itemParamGetHPUp: getField09 */
/* itemParamGetReviveFlag: setField09 */
/* itemParamGetAttackEffortUp: getField0A */
/* itemParamGetHPEffortUp: getField0B */
/* itemParamGetPPMaxUpFlag: setField00 (f32) */
/* itemParamGetConfuseFlag: setField04_bit0 */
/* itemParamGetParalyzeFlag: setField04_bit1 */
/* itemParamGetFreezeFlag: setField04_bit2 */
/* itemParamGetBurnFlag: setField04_bit3 */
/* itemParamGetPoisonFlag: setField05 (s8) */
/* itemParamGetSleepFlag: setField06 (s8) */
/* itemParamGetLevelUpFlag: setField07 (s8) */
/* itemParamGetGuardFlag: setField08 (u32) */
/* itemParamGetSpAttackUp: setField0C (u8) */
/* itemParamGetHitUp: setField0D (u8) */
/* itemParamGetQuickUp: setField0E (u8) */
/* itemParamGetDefenceUp: setField0F (u8) */
/* itemParamGetAttackUp: clearFlags04 */
/* itemParamGetCriticalFlag: setFlags04 */
/* itemParamGetMeromeroFlag: setField_special1 */
/* itemParamGetPtr: setField_special2 */
/* tasteDataGetAisyou: setField_special3 */
/* tasteDataGetNigateMsgDataId: getField_special1 */
/* tasteDataGetPtr: getField_special2 */
/* itemSoubiDataBiosGetFightKoukaDataId: getField_special3 */
/* itemSoubiDataBiosGetPtr: setField_special4 */
/* itemBiosSetNum: getField10_int (posX as int) */
/* itemBiosSetItemDataId: getField14_int (posY as int) */
/* itemBallDataBiosSetFightKoukaDataId: getField18_int (posZ as int) */
/* itemDataBiosSetBuff: getField1C_int (rotAngle as int) */
/* itemDataBiosSetUseFriend: setField10_int */
/* itemDataBiosSetFightUseKoukaDataId: getPosX (f32) */
/* itemDataBiosSetItemSoubiDataId: getPosY (f32) */
/* itemDataBiosSetDoc: getPosZ (f32) */
/* itemDataBiosSetUseful: getField00 (f32) */
/* itemDataBiosSetImportant: getRotAngle (f32) */
/* itemDataBiosSetPrice: getScale (f32) */
/* itemDataBiosSetKind: getModelRef (u32) */
/* itemDataBiosSetName: getField24_byte */
/* itemBiosGetNum: setPosX (f32) */
/* itemBiosGetItemDataId: setPosY (f32) */
/* itemBallDataBiosGetSnatchSnatchWzxDataId: setPosZ (f32) */
/* itemBallDataBiosGetSnatchShakeWzxDataId: setRotAngle (f32) */
/* itemBallDataBiosGetSnatchPokeoutWzxDataId: setScale (f32) */
/* itemBallDataBiosGetSnatchMissWzxDataId: setModelRef */
/* itemBallDataBiosGetSnatchBalllandWzxDataId: setField24 */
/* itemBallDataBiosGetSnatchAttackWzxDataId: setField24_byte */
/* itemBallDataBiosGetThrowWzxDataId: getField_ext1 */
/* itemBallDataBiosGetDowninWzxDataId: getField_ext2 */
/* itemBallDataBiosGetOutWzxDataId: getField_ext3 */
/* itemBallDataBiosGetOpenWzxDataId: getField_ext4 */
/* itemBallDataBiosGetInWzxDataId: getField_ext5 */
/* itemBallDataBiosGetFightKoukaDataId: getField_ext6 */
/* itemBallDataBiosGetPtr: getField_ext7 (with extra logic) */
/* itemDataBiosGetBattleUseFunc: getField_ext8 */
/* itemDataBiosGetFieldUseFunc: getField_ext9 */
/* itemDataBiosGetItemEffectParam: getField_ext10 */
/* itemDataBiosGetBuff: getField_ext11 */
/* itemDataBiosGetUseFriend: setField_ext1 */
/* itemDataBiosGetKinomiNo: setField_ext2 */
/* itemDataBiosGetHidenMachineNo: setField_ext3 (0x68 bytes) */
/* itemDataBiosGetWazaIDByWazaMachineNo: setField_ext4 */
/* itemDataBiosGetWazaMachineNo: setField_ext5 */
/* itemDataBiosGetFightUseKoukaDataId: getField_ext12 */
/* itemDataBiosGetItemSoubiDataId: getField_ext13 */
/* itemDataBiosGetDoc: getField_ext14 */
/* fn_80143F9C: getField_ext15 */
/* fn_80143FB4: getField_ext16 */
/* fn_80143FCC: getField_ext17 */
/* itemDataBiosGetCoupon: getField_ext18 */
/* itemDataBiosGetPrice: getField_ext19 */
/* itemDataBiosGetKind: getField_ext20 */
/* itemDataBiosCheckExportable: setField_ext6 */
/* itemDataBiosCheckImportable: setField_ext7 */
/* itemDataBiosGetName: peopleFieldQuery (0x18 bytes) -- quick status check */

/* itemUse2PokemonSimulation: peopleFieldOpenModel (0xB8 bytes) */
/* friendXUp__FP7PokemonP12FightPokemonScUsUs: peopleFieldConfigModel (0x224 bytes) */
/* fn_801443CC: peopleFieldFinalizeModel (0x1A8 bytes) */
/* renamed symbols referenced by asm incs (symbolmap port) */
/* Forward declarations for self-referencing asm blocks */
extern void fn_801425E8(u32* base, u16 count, u8 mode);
extern void fn_80142B24(void*, u32, u16, u32, u32);
extern s32 itemGetStatus(u32, u16, u16, u32);
extern u16  itemSoubiDataBiosGetFightKoukaDataId(u8* p);
extern u8*  itemSoubiDataBiosGetPtr(u16 idx);
extern void itemBiosSetNum(u8* p, u16 val);
extern void itemBiosSetItemDataId(u8* p, u16 val);
extern void itemBallDataBiosSetFightKoukaDataId(u8* p, u16 val);
extern void itemDataBiosSetBuff(u8* p, u32 val);
extern void itemDataBiosSetUseFriend(u8* p, u16 idx, u8 val);
extern void itemDataBiosSetFightUseKoukaDataId(u8* p, u16 val);
extern void itemDataBiosSetItemSoubiDataId(u8* p, u16 val);
extern void itemDataBiosSetDoc(u8* p, u32 val);
extern void itemDataBiosSetUseful(u8* p, u8 val);
extern void itemDataBiosSetImportant(u8* p, u8 val);
extern void itemDataBiosSetPrice(u8* p, u16 val);
extern void itemDataBiosSetKind(u8* p, u8 val);
extern void itemDataBiosSetName(u8* p, u32 val);

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

#define PEOPLE_ENTRY_ID(entry) ((u16)itemGetStatus((u32)(entry), 0, 0x1b, 0))
#define PEOPLE_ENTRY_COUNT(entry) ((u16)itemGetStatus((u32)(entry), 0, 0x1c, 0))
#define PEOPLE_ITEM_ID_VALID(id) \
    (itemGetStatus(0, (u16)(id), 1, 0) != 0 && (u16)(id) < lbl_80478BD8)
#define PEOPLE_ENTRY_VALID(entry) \
    ((entry) != NULL && PEOPLE_ENTRY_ID(entry) != 0 && PEOPLE_ITEM_ID_VALID(PEOPLE_ENTRY_ID(entry)))

#if 0
asm void fn_80140ACC(void) {
#include "src/game/people/people_data_fn_80140ACC.inc"
}
#else

/* Resolve the placement offset for a field-person record group. */
s32 fn_80140588(u32* entries, u16 count, u16 dataIndex, u16 extent,
                u8 accumulate)
{
    u32* entry;
    u16 i;
    s32 result;

    if (entries == NULL) {
        return -1;
    }
    if (!PEOPLE_ITEM_ID_VALID(dataIndex)) {
        return -1;
    }

    if (accumulate == 0) {
        if (entries == NULL) {
            entry = NULL;
        } else if (!PEOPLE_ITEM_ID_VALID(dataIndex)) {
            entry = NULL;
        } else {
            entry = NULL;
            for (i = 0; i < count; i++) {
                u32* current = entries + i;
                if (PEOPLE_ENTRY_VALID(current) &&
                    dataIndex == PEOPLE_ENTRY_ID(current)) {
                    entry = current;
                    break;
                }
            }
        }

        if (entry != NULL) {
            result = (u16)extent - PEOPLE_ENTRY_COUNT(entry);
        } else {
            u32* freeEntry;

            if (entries == NULL || !PEOPLE_ITEM_ID_VALID(dataIndex)) {
                freeEntry = NULL;
            } else {
                freeEntry = NULL;
                for (i = 0; i < count; i++) {
                    u32* current = entries + i;
                    if (!PEOPLE_ENTRY_VALID(current)) {
                        freeEntry = current;
                        break;
                    }
                }
            }
            result = freeEntry != NULL ? (u16)extent : 0;
        }
    } else {
        result = 0;
        for (i = 0; i < count; i++) {
            entry = entries + i;
            if (!PEOPLE_ENTRY_VALID(entry)) {
                result += (u16)extent;
            } else if (PEOPLE_ENTRY_VALID(entry) &&
                       dataIndex == PEOPLE_ENTRY_ID(entry)) {
                result += (u16)extent - PEOPLE_ENTRY_COUNT(entry);
            }
        }
    }

    return result;
}

#pragma optimization_level 4
s32 fn_80140ACC(u32* base, u16 count, u16 id, u16 amount, s16 index, u16 unusedLimit, u8 sortMode) {
    u32* entry;
    u16 remaining;
    u16 current;
    u16 next;
    u16 i;
    u8 needsSort;

    if (base == NULL) {
        return -1;
    }
    if (!PEOPLE_ITEM_ID_VALID(id)) {
        return -1;
    }

    remaining = amount;
    needsSort = 0;
    unusedLimit = unusedLimit;

    if (index < 0) {
        while (remaining != 0) {
            entry = NULL;
            for (i = 0; i < count; i++) {
                if (PEOPLE_ENTRY_VALID(base + i) && PEOPLE_ENTRY_ID(base + i) == id) {
                    entry = base + i;
                    break;
                }
            }

            if (entry == NULL) {
                break;
            }

            current = PEOPLE_ENTRY_COUNT(entry);
            if (current > remaining) {
                next = (u16)(current - remaining);
                remaining = 0;
            } else {
                next = 0;
                remaining = (u16)(remaining - current);
            }

            fn_80142B24(entry, 0, 0x1c, 0, next);
            if (next == 0) {
                fn_80142B24(entry, 0, 0x1b, 0, 0);
                fn_80142B24(entry, 0, 0x1c, 0, 0);
                needsSort = 1;
            }
        }
    } else {
        if ((u16)index >= count) {
            return -1;
        }

        entry = base + (u16)index;
        if (!PEOPLE_ENTRY_VALID(entry) || PEOPLE_ENTRY_ID(entry) != id) {
            return -1;
        }

        current = PEOPLE_ENTRY_COUNT(entry);
        if (current > remaining) {
            next = (u16)(current - remaining);
            remaining = 0;
        } else {
            next = 0;
            remaining = (u16)(remaining - current);
        }

        fn_80142B24(entry, 0, 0x1c, 0, next);
        if (next == 0) {
            fn_80142B24(entry, 0, 0x1b, 0, 0);
            fn_80142B24(entry, 0, 0x1c, 0, 0);
            needsSort = 1;
        }
    }

    if (needsSort) {
        fn_801425E8(base, count, sortMode);
    }
    return remaining;
}
#endif
extern u32 lbl_80478BD8;   /* gPeopleFieldCount */
#if 0
asm void fn_80141308(void) {
#include "src/game/people/people_data_fn_80141308.inc"
}
#else
#pragma optimization_level 4
s32 fn_80141308(u32* base, u16 count, u16 id, u16 amount, s16 index, u16 maxCount, u8 sortMode, u8 forceIndex) {
    u32* entry;
    u16 current;
    u16 next;
    u16 overflow;
    u16 i;
    u8 needsSort;

    if (base == NULL) {
        return -1;
    }
    if (!PEOPLE_ITEM_ID_VALID(id)) {
        return -1;
    }

    entry = NULL;
    overflow = amount;
    needsSort = 0;

    if (forceIndex == 0) {
        for (i = 0; i < count; i++) {
            if (PEOPLE_ENTRY_VALID(base + i) && PEOPLE_ENTRY_ID(base + i) == id) {
                entry = base + i;
                break;
            }
        }

        if (entry == NULL) {
            for (i = 0; i < count; i++) {
                if (!PEOPLE_ENTRY_VALID(base + i)) {
                    entry = base + i;
                    needsSort = 1;
                    break;
                }
            }
        }
    } else {
        if (index < 0 || (u16)index >= count) {
            return -1;
        }
        entry = base + (u16)index;
        needsSort = !PEOPLE_ENTRY_VALID(entry) || PEOPLE_ENTRY_ID(entry) != id;
    }

    if (entry == NULL) {
        return overflow;
    }

    if (!PEOPLE_ENTRY_VALID(entry) || PEOPLE_ENTRY_ID(entry) != id) {
        fn_80142B24(entry, 0, 0x1b, 0, 0);
        fn_80142B24(entry, 0, 0x1c, 0, 0);
        fn_80142B24(entry, 0, 0x1b, 0, id);
        current = 0;
        needsSort = 1;
    } else {
        current = PEOPLE_ENTRY_COUNT(entry);
    }

    if (current >= maxCount) {
        overflow = amount;
    } else {
        next = (u16)(current + amount);
        if (next > maxCount || next < current) {
            overflow = (u16)(next - maxCount);
            next = maxCount;
        } else {
            overflow = 0;
        }
        fn_80142B24(entry, 0, 0x1c, 0, next);
    }

    if (needsSort) {
        fn_801425E8(base, count, sortMode);
    }
    return overflow;
}
#endif
extern u32 lbl_80478BD8;   /* gPeopleFieldCount */
#if 0
asm void fn_80142368(void) {
#include "src/game/people/people_data_fn_80142368.inc"
}
#else
#pragma optimization_level 4
u32* fn_80142368(u32* base, u16 count, u16 id, u8 mode, u16 limit) {
    u32* entry;
    u16 i;

    if (base == NULL) {
        return NULL;
    }
    if (!PEOPLE_ITEM_ID_VALID(id)) {
        return NULL;
    }

    for (i = 0; i < count; i++) {
        entry = base + i;
        if (mode == 2) {
            if (!PEOPLE_ENTRY_VALID(entry)) {
                return entry;
            }
        } else if (PEOPLE_ENTRY_VALID(entry) && PEOPLE_ENTRY_ID(entry) == id) {
            if (mode == 1) {
                if (PEOPLE_ENTRY_COUNT(entry) < limit) {
                    return entry;
                }
            } else {
                return entry;
            }
        }
    }

    return NULL;
}
#endif
extern u32 lbl_80478BD8;   /* gPeopleFieldCount */
#if 0
asm void fn_801425E8(void) {
#include "src/game/people/people_data_fn_801425E8.inc"
}
#else
#pragma optimization_level 4
void fn_801425E8(u32* base, u16 count, u8 mode) {
    u32* entry;
    u32* other;
    u32 temp;
    u16 i;
    u16 j;

    if (base == NULL || count == 0) {
        return;
    }

    if (mode == 0 || mode == 1) {
        for (i = 0; i + 1 < count; i++) {
            entry = base + i;
            if (!PEOPLE_ENTRY_VALID(entry)) {
                for (j = (u16)(i + 1); j < count; j++) {
                    other = base + j;
                    if (PEOPLE_ENTRY_VALID(other)) {
                        temp = *entry;
                        *entry = *other;
                        *other = temp;
                        break;
                    }
                }
            }
        }
    }

    if (mode == 1) {
        for (i = 0; i + 1 < count; i++) {
            entry = base + i;
            if (PEOPLE_ENTRY_VALID(entry)) {
                for (j = (u16)(i + 1); j < count; j++) {
                    other = base + j;
                    if (PEOPLE_ENTRY_VALID(other) &&
                        PEOPLE_ENTRY_ID(entry) > PEOPLE_ENTRY_ID(other)) {
                        temp = *entry;
                        *entry = *other;
                        *other = temp;
                    }
                }
            }
        }
    }
}
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
extern u8* itemDataBiosGetPtr(u16 idx);
extern u8* itemBallDataBiosGetPtr(u16 idx);
extern void fightItemBiosSetItemDataId(u8* p, u16 val);
extern void fightItemBiosSetTargetDataId(u8* p, u16 val);
extern void fightItemBiosSetCount(u8* p, u32 val);
extern void fightItemBiosSetBuff(u8* p, u32 val);
extern void jumptable_80367D60();
#if 0
asm void fn_80142B24(void* p, u32 a, u16 b, u32 c, u32 d) {
#include "src/game/people/people_data_fn_80142B24.inc"
}
#else
#pragma optimization_level 4
void fn_80142B24(void* p, u32 a, u16 b, u32 c, u32 d) {
    u8* target;

    if (b == 0 || b >= 0x23) {
        return;
    }

    if (b < 0xb) {
        target = itemDataBiosGetPtr((u16)a);
        if (target == NULL) {
            return;
        }
    } else if (b < 0x18) {
        target = itemBallDataBiosGetPtr((u16)a);
        if (target == NULL) {
            return;
        }
    } else {
        target = p;
        if (target == NULL) {
            return;
        }
    }

    switch (b) {
    case 1:
        itemDataBiosSetName(target, d);
        break;
    case 2:
        itemDataBiosSetKind(target, (u8)d);
        break;
    case 3:
        itemDataBiosSetPrice(target, (u16)d);
        break;
    case 4:
        itemDataBiosSetImportant(target, (u8)d);
        break;
    case 5:
        itemDataBiosSetUseful(target, (u8)d);
        break;
    case 6:
        itemDataBiosSetDoc(target, d);
        break;
    case 7:
        itemDataBiosSetItemSoubiDataId(target, (u16)d);
        break;
    case 8:
        itemDataBiosSetFightUseKoukaDataId(target, (u16)d);
        break;
    case 9:
        itemDataBiosSetUseFriend(target, (u16)c, (u8)d);
        break;
    case 10:
        itemDataBiosSetBuff(target, d);
        /* fallthrough */
    case 12:
        itemBallDataBiosSetFightKoukaDataId(target, (u16)d);
        break;
    case 27:
        itemBiosSetItemDataId(target, (u16)d);
        break;
    case 28:
        itemBiosSetNum(target, (u16)d);
        break;
    case 30:
        fightItemBiosSetItemDataId(target, (u16)d);
        break;
    case 31:
        fightItemBiosSetTargetDataId(target, (u16)d);
        break;
    case 32:
        fightItemBiosSetCount(target, d);
        break;
    case 33:
        fightItemBiosSetBuff(target, d);
        break;
    default:
        break;
    }
}
#endif
extern u32 itemDataBiosGetName(u8* p);
extern u8 itemDataBiosGetKind(u8* p);
extern u32 itemDataBiosGetPrice(u8* p);
extern u8 fn_80143FCC(u8* p);
extern u8 fn_80143FB4(u8* p);
extern u32 itemDataBiosGetDoc(u8* p);
extern u32 itemDataBiosGetItemSoubiDataId(u8* p);
extern u32 itemDataBiosGetFightUseKoukaDataId(u8* p);
extern s32 itemDataBiosGetUseFriend(u8* p, u16 idx);
extern u32 itemDataBiosGetBuff(u8* p);
extern u32 itemBallDataBiosGetFightKoukaDataId(u8* p);
extern u32 itemBallDataBiosGetInWzxDataId(u8* p);
extern u32 itemBallDataBiosGetOpenWzxDataId(u8* p);
extern u32 itemBallDataBiosGetOutWzxDataId(u8* p);
extern u32 itemBallDataBiosGetDowninWzxDataId(u8* p);
extern u32 itemBallDataBiosGetThrowWzxDataId(u8* p);
extern u32 itemBallDataBiosGetSnatchAttackWzxDataId(u8* p);
extern u32 itemBallDataBiosGetSnatchBalllandWzxDataId(u8* p);
extern u32 itemBallDataBiosGetSnatchMissWzxDataId(u8* p);
extern u32 itemBallDataBiosGetSnatchPokeoutWzxDataId(u8* p);
extern u32 itemBallDataBiosGetSnatchShakeWzxDataId(u8* p);
extern u32 itemBallDataBiosGetSnatchSnatchWzxDataId(u8* p);
extern u32 itemBiosGetItemDataId(u8* p);
extern u32 itemBiosGetNum(u8* p);
extern u32 fightItemBiosGetItemDataId(u8* p);
extern u32 fightItemBiosGetTargetDataId(u8* p);
extern u32 fightItemBiosGetCount(u8* p);
extern u32 fightItemBiosGetBuff(u8* p);
extern void jumptable_80367DE8();
#if 0
asm s32 itemGetStatus(u32 a, u16 b, u16 c, u32 d) {
#include "src/game/people/people_data_fn_80142CF4.inc"
}
#else
/* Keep the standalone farm shape: later definitions must not auto-inline here. */
#pragma push
#pragma inline_depth(0)
s32 itemGetStatus(u32 a, u16 b, u16 c, u32 d) {
    u8* target;

    if (c == 0 || c >= 0x23) {
        return 0;
    }

    if (c < 0xb) {
        target = itemDataBiosGetPtr(b);
        if (target == NULL) {
            return 0;
        }
    } else if (c < 0x18) {
        target = itemBallDataBiosGetPtr(b);
        if (target == NULL) {
            return 0;
        }
    } else if (c < 0x1a) {
        target = itemSoubiDataBiosGetPtr(b);
        if (target == NULL) {
            return 0;
        }
    } else {
        if (a == 0) {
            return 0;
        }
        target = (u8*)a;
    }

    switch (c) {
    case 1:
        return itemDataBiosGetName(target);
    case 2:
        return itemDataBiosGetKind(target);
    case 3:
        return (u16)itemDataBiosGetPrice(target);
    case 4:
        return fn_80143FCC(target);
    case 5:
        return fn_80143FB4(target);
    case 6:
        return itemDataBiosGetDoc(target);
    case 7:
        return (u16)itemDataBiosGetItemSoubiDataId(target);
    case 8:
        return (u16)itemDataBiosGetFightUseKoukaDataId(target);
    case 9:
        return (s8)itemDataBiosGetUseFriend(target, (u16) ((((((((((d & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu));
    case 10:
        return itemDataBiosGetBuff(target);
    case 12:
        return (u16)itemBallDataBiosGetFightKoukaDataId(target);
    case 13:
        return itemBallDataBiosGetInWzxDataId(target);
    case 14:
        return itemBallDataBiosGetOpenWzxDataId(target);
    case 15:
        return itemBallDataBiosGetOutWzxDataId(target);
    case 16:
        return itemBallDataBiosGetDowninWzxDataId(target);
    case 17:
        return itemBallDataBiosGetThrowWzxDataId(target);
    case 18:
        return itemBallDataBiosGetSnatchAttackWzxDataId(target);
    case 19:
        return itemBallDataBiosGetSnatchBalllandWzxDataId(target);
    case 20:
        return itemBallDataBiosGetSnatchMissWzxDataId(target);
    case 21:
        return itemBallDataBiosGetSnatchPokeoutWzxDataId(target);
    case 22:
        return itemBallDataBiosGetSnatchShakeWzxDataId(target);
    case 23:
        return itemBallDataBiosGetSnatchSnatchWzxDataId(target);
    case 25:
        return (u16)itemSoubiDataBiosGetFightKoukaDataId(target);
    case 27:
        return (u16)itemBiosGetItemDataId(target);
    case 28:
        return (u16)itemBiosGetNum(target);
    case 30:
        return (u16)fightItemBiosGetItemDataId(target);
    case 31:
        return (u16)fightItemBiosGetTargetDataId(target);
    case 32:
        return fightItemBiosGetCount(target);
    case 33:
        return fightItemBiosGetBuff(target);
    default:
        return 0;
    }
}
#pragma pop
#endif
extern void jumptable_80367E70();
extern u8 lbl_802730E0[];

typedef struct ItemParamConvertEntry {
    u32 field;
    u32 srcOffset;
    u32 mask;
    u32 type;
} ItemParamConvertEntry;

#define PEOPLE_SET_FLAG_BYTE(byte, mask, flag) \
    ((byte) = (u8)(((byte) & (u8)~(mask)) | ((flag) ? (mask) : 0)))

#if 0
asm void fn_80142EF8(void) {
#include "src/game/people/people_data_fn_80142EF8.inc"
}
#else
#pragma optimization_level 4
void itemParamConvertOrigFormat(u8* dst, u8* src) {
    ItemParamConvertEntry* table;
    ItemParamConvertEntry* row;
    u8* stream;
    u32 i;
    u32 bit;
    u32 decoded;
    u32 flag;
    u32 type;
    u8 srcByte;
    u8 maskByte;
    s8 signedValue;

    table = (ItemParamConvertEntry*)lbl_802730E0;
    stream = src + 6;

    for (i = 0; i < 0x20; i++) {
        row = table + i;
        type = row->type;
        srcByte = src[row->srcOffset];
        maskByte = (u8)(row->mask >> 24);
        decoded = 0;
        flag = 0;
        signedValue = 0;

        if (type == 2 || type == 3) {
            if ((srcByte & maskByte) != 0) {
                decoded = *stream++;
                if (type == 3) {
                    signedValue = (s8)decoded;
                }
            }
        } else {
            bit = 0;
            while (bit < 8 && ((maskByte >> bit) & 1) == 0) {
                bit++;
            }
            decoded = (u8)((srcByte & maskByte) >> bit);
            if (type == 0) {
                flag = decoded != 0;
            }
        }

        switch (row->field) {
        case 0:
            PEOPLE_SET_FLAG_BYTE(dst[0], 0x80, flag);
            break;
        case 2:
            PEOPLE_SET_FLAG_BYTE(dst[0], 0x20, flag);
            break;
        case 3:
            dst[0] = (u8)((dst[0] & (u8)~0x1e) | ((decoded & 0xf) << 1));
            break;
        case 4:
            dst[1] = (u8)((dst[1] & 0x0f) | ((decoded & 0xf) << 4));
            break;
        case 5:
            dst[1] = (u8)((dst[1] & 0xf0) | (decoded & 0xf));
            break;
        case 6:
            dst[2] = (u8)((dst[2] & 0x0f) | ((decoded & 0xf) << 4));
            break;
        case 7:
            dst[2] = (u8)((dst[2] & 0xf0) | (decoded & 0xf));
            break;
        case 8:
            PEOPLE_SET_FLAG_BYTE(dst[3], 0x80, flag);
            break;
        case 9:
            PEOPLE_SET_FLAG_BYTE(dst[3], 0x40, flag);
            break;
        case 10:
            PEOPLE_SET_FLAG_BYTE(dst[3], 0x20, flag);
            break;
        case 11:
            PEOPLE_SET_FLAG_BYTE(dst[3], 0x10, flag);
            break;
        case 12:
            PEOPLE_SET_FLAG_BYTE(dst[3], 0x08, flag);
            break;
        case 13:
            PEOPLE_SET_FLAG_BYTE(dst[3], 0x04, flag);
            break;
        case 14:
            PEOPLE_SET_FLAG_BYTE(dst[3], 0x02, flag);
            break;
        case 15:
            PEOPLE_SET_FLAG_BYTE(dst[3], 0x01, flag);
            break;
        case 16:
            PEOPLE_SET_FLAG_BYTE(dst[4], 0x80, flag);
            break;
        case 17:
            dst[8] = (u8)decoded;
            break;
        case 18:
            dst[9] = (u8)decoded;
            break;
        case 19:
            PEOPLE_SET_FLAG_BYTE(dst[4], 0x40, flag);
            break;
        case 20:
            dst[0xa] = (u8)decoded;
            break;
        case 21:
            PEOPLE_SET_FLAG_BYTE(dst[4], 0x20, flag);
            break;
        case 22:
            dst[0xb] = (u8)decoded;
            break;
        case 23:
            PEOPLE_SET_FLAG_BYTE(dst[4], 0x10, flag);
            break;
        case 24:
            dst[0xc] = (u8)decoded;
            break;
        case 25:
            dst[0xd] = (u8)decoded;
            break;
        case 26:
            dst[0xe] = (u8)decoded;
            break;
        case 27:
            dst[0xf] = (u8)decoded;
            break;
        case 28:
            PEOPLE_SET_FLAG_BYTE(dst[4], 0x08, flag);
            break;
        case 29:
            dst[5] = (u8)signedValue;
            break;
        case 30:
            dst[6] = (u8)signedValue;
            break;
        case 31:
            dst[7] = (u8)signedValue;
            break;
        default:
            break;
        }
    }
}
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
#pragma optimization_level 4
s8 itemParamGetFriend3Up(u8* p) {
    ItemParamData* itemParam;

    itemParam = (ItemParamData*)p;
    if (itemParam == NULL) return 0;
    return (s8)((u8)itemParam->friend3Up);
}

#if 0
asm void itemParamGetFriend2Up(void) {
#include "src/game/people/people_data_fn_801436B8.inc"
}
#else
#pragma optimization_level 4
s8 itemParamGetFriend2Up(u8* p) {
    if (p == NULL) return 0;
    return (s8)p[0x6];
}
#endif
#if 0
asm void itemParamGetFriend1Up(void) {
#include "src/game/people/people_data_fn_801436D4.inc"
}
#else
#pragma optimization_level 4
s8 itemParamGetFriend1Up(u8* p) {
    if (p == NULL) return 0;
    return (s8)p[0x5];
}
#endif
#if 0
asm void itemParamGetPPMaxFullFlag(void) {
#include "src/game/people/people_data_fn_801436F0.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetPPMaxFullFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x4] >> 3) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetSpAttackEffortUp(void) {
#include "src/game/people/people_data_fn_80143718.inc"
}
#else
#pragma optimization_level 4
u8 itemParamGetSpAttackEffortUp(u8* p) {
    if (p == NULL) return 0;
    return p[0xf];
}
#endif
#if 0
asm void itemParamGetSpDefenceEffortUp(void) {
#include "src/game/people/people_data_fn_80143730.inc"
}
#else
#pragma optimization_level 4
u8 itemParamGetSpDefenceEffortUp(u8* p) {
    if (p == NULL) return 0;
    return p[0xe];
}
#endif
#if 0
asm void itemParamGetQuickEffortUp(void) {
#include "src/game/people/people_data_fn_80143748.inc"
}
#else
#pragma optimization_level 4
u8 itemParamGetQuickEffortUp(u8* p) {
    if (p == NULL) return 0;
    return p[0xd];
}
#endif
#if 0
asm void itemParamGetDefenceEffortUp(void) {
#include "src/game/people/people_data_fn_80143760.inc"
}
#else
#pragma optimization_level 4
u8 itemParamGetDefenceEffortUp(u8* p) {
    if (p == NULL) return 0;
    return p[0xc];
}
#endif
#if 0
asm void itemParamGetEvolutionFlag(void) {
#include "src/game/people/people_data_fn_80143778.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetEvolutionFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x4] >> 4) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetPPUp(void) {
#include "src/game/people/people_data_fn_801437A0.inc"
}
#else
#pragma optimization_level 4
u8 itemParamGetPPUp(u8* p) {
    if (p == NULL) return 0;
    return p[0xb];
}
#endif
#if 0
asm void itemParamGetPPSelectFlag(void) {
#include "src/game/people/people_data_fn_801437B8.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetPPSelectFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x4] >> 5) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetHPUp(void) {
#include "src/game/people/people_data_fn_801437E0.inc"
}
#else
#pragma optimization_level 4
u8 itemParamGetHPUp(u8* p) {
    if (p == NULL) return 0;
    return p[0xa];
}
#endif
#if 0
asm void itemParamGetReviveFlag(void) {
#include "src/game/people/people_data_fn_801437F8.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetReviveFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x4] >> 6) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetAttackEffortUp(void) {
#include "src/game/people/people_data_fn_80143820.inc"
}
#else
#pragma optimization_level 4
u8 itemParamGetAttackEffortUp(u8* p) {
    if (p == NULL) return 0;
    return p[0x9];
}
#endif
#if 0
asm void itemParamGetHPEffortUp(void) {
#include "src/game/people/people_data_fn_80143838.inc"
}
#else
#pragma optimization_level 4
u8 itemParamGetHPEffortUp(u8* p) {
    if (p == NULL) return 0;
    return p[0x8];
}
#endif
#if 0
asm void itemParamGetPPMaxUpFlag(void) {
#include "src/game/people/people_data_fn_80143850.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetPPMaxUpFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x4] >> 7) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetConfuseFlag(void) {
#include "src/game/people/people_data_fn_80143878.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetConfuseFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!(p[0x3] & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetParalyzeFlag(void) {
#include "src/game/people/people_data_fn_801438A0.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetParalyzeFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 1) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetFreezeFlag(void) {
#include "src/game/people/people_data_fn_801438C8.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetFreezeFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 2) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetBurnFlag(void) {
#include "src/game/people/people_data_fn_801438F0.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetBurnFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 3) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetPoisonFlag(void) {
#include "src/game/people/people_data_fn_80143918.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetPoisonFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 4) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetSleepFlag(void) {
#include "src/game/people/people_data_fn_80143940.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetSleepFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 5) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetLevelUpFlag(void) {
#include "src/game/people/people_data_fn_80143968.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetLevelUpFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 6) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetGuardFlag(void) {
#include "src/game/people/people_data_fn_80143990.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetGuardFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x3] >> 7) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetSpAttackUp(void) {
#include "src/game/people/people_data_fn_801439B8.inc"
}
#else
#pragma optimization_level 4
u32 itemParamGetSpAttackUp(u8* p) {
    if (p == NULL) return 0;
    return (u32)(p[0x2] & 0xF);
}
#endif
#if 0
asm void itemParamGetHitUp(void) {
#include "src/game/people/people_data_fn_801439D4.inc"
}
#else
#pragma optimization_level 4
u32 itemParamGetHitUp(u8* p) {
    if (p == NULL) return 0;
    return (u32)((p[0x2] >> 4) & 0xF);
}
#endif
#if 0
asm void itemParamGetQuickUp(void) {
#include "src/game/people/people_data_fn_801439F0.inc"
}
#else
#pragma optimization_level 4
u32 itemParamGetQuickUp(u8* p) {
    if (p == NULL) return 0;
    return (u32)(p[0x1] & 0xF);
}
#endif
#if 0
asm void itemParamGetDefenceUp(void) {
#include "src/game/people/people_data_fn_80143A0C.inc"
}
#else
#pragma optimization_level 4
u32 itemParamGetDefenceUp(u8* p) {
    if (p == NULL) return 0;
    return (u32)((p[0x1] >> 4) & 0xF);
}
#endif
#if 0
asm void itemParamGetAttackUp(void) {
#include "src/game/people/people_data_fn_80143A28.inc"
}
#else
#pragma optimization_level 4
u32 itemParamGetAttackUp(u8* p) {
    if (p == NULL) return 0;
    return (u32)((p[0x0] >> 1) & 0xF);
}
#endif
#if 0
asm void itemParamGetCriticalFlag(void) {
#include "src/game/people/people_data_fn_80143A44.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetCriticalFlag(u8* p) {
    s32 v;
    if (p == NULL) return 0;
    v = !!((p[0x0] >> 5) & 1);
    return v;
}
#pragma pop
#endif
#if 0
asm void itemParamGetMeromeroFlag(void) {
#include "src/game/people/people_data_fn_80143A6C.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 itemParamGetMeromeroFlag(u8* p) {
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
asm void itemParamGetPtr(void) {
#include "src/game/people/people_data_fn_80143A94.inc"
}
#else
#pragma optimization_level 4
u8* itemParamGetPtr(u8 idx) {
    u8* result = &lbl_80368630[(u8)idx * 16];
    if (idx < lbl_80478BE0) return result;
    return NULL;
}
#endif
#if 0
asm void tasteDataGetAisyou(void) {
#include "src/game/people/people_data_fn_80143ABC.inc"
}
#else
#pragma optimization_level 4
s8 tasteDataGetAisyou(u8* p, u16 idx) {
    if (p == NULL) return 0;
    if ((u32)(idx & 0xFFFF) >= 0x19) return 0;
    return (s8)(p[idx + 4]);
}
#endif
#if 0
asm void tasteDataGetNigateMsgDataId(void) {
#include "src/game/people/people_data_fn_80143AF0.inc"
}
#else
#pragma optimization_level 4
u32 tasteDataGetNigateMsgDataId(u8* p) {
    if (p == NULL) return 0;
    return *(u32*)p;
}
#endif
extern u32 lbl_80478BC8;
extern u8 lbl_80367F78[];
#if 0
asm void tasteDataGetPtr(void) {
#include "src/game/people/people_data_fn_80143B08.inc"
}
#else
#pragma optimization_level 4
u8* tasteDataGetPtr(u16 idx) {
    u8* result = &lbl_80367F78[(u16)idx * 32];
    if ((u16)idx < lbl_80478BC8) return result;
    return NULL;
}
#endif
#if 0
asm void itemSoubiDataBiosGetFightKoukaDataId(void) {
#include "src/game/people/people_data_fn_80143B30.inc"
}
#else
#pragma optimization_level 4
u16 itemSoubiDataBiosGetFightKoukaDataId(u8* p) {
    if (p == NULL) return 0;
    return *(u16*)p;
}
#endif
extern u32 lbl_80478BC0;
extern u8 lbl_80367EF0[];
#if 0
asm void itemSoubiDataBiosGetPtr(void) {
#include "src/game/people/people_data_fn_80143B48.inc"
}
#else
#pragma optimization_level 4
u8* itemSoubiDataBiosGetPtr(u16 idx) {
    u8* result = &lbl_80367EF0[(u16)idx * 2];
    if ((u16)idx < lbl_80478BC0) return result;
    return NULL;
}
#endif
#if 0
asm void itemBiosSetNum(void) {
#include "src/game/people/people_data_fn_80143B70.inc"
}
#else
#pragma optimization_level 4
void itemBiosSetNum(u8* p, u16 val) {
    if (p == NULL) return;
    *(u16*)(p + 0x2) = val;
}
#endif
#if 0
asm void itemBiosSetItemDataId(void) {
#include "src/game/people/people_data_fn_80143B80.inc"
}
#else
#pragma optimization_level 4
void itemBiosSetItemDataId(u8* p, u16 val) {
    if (p == NULL) return;
    *(u16*)(p + 0x0) = val;
}
#endif
#if 0
asm void itemBallDataBiosSetFightKoukaDataId(void) {
#include "src/game/people/people_data_fn_80143B90.inc"
}
#else
#pragma optimization_level 4
void itemBallDataBiosSetFightKoukaDataId(u8* p, u16 val) {
    if (p == NULL) return;
    *(u16*)(p + 0x0) = val;
}
#endif
#if 0
asm void itemDataBiosSetBuff(void) {
#include "src/game/people/people_data_fn_80143BA0.inc"
}
#else
#pragma optimization_level 4
void itemDataBiosSetBuff(u8* p, u32 val) {
    if (p == NULL) return;
    *(u32*)(p + 0x18) = val;
}
#endif
#if 0
asm void itemDataBiosSetUseFriend(void) {
#include "src/game/people/people_data_fn_80143BB0.inc"
}
#else
#pragma optimization_level 4
void itemDataBiosSetUseFriend(u8* p, u16 idx, u8 val) {
    u32 i;
    if (p == NULL) return;
    i = (u32)(idx & 0xFFFF);
    if (i >= 3) return;
    p[i + 0x24] = val;
}
#endif
#if 0
asm void itemDataBiosSetFightUseKoukaDataId(void) {
#include "src/game/people/people_data_fn_80143BD0.inc"
}
#else
#pragma optimization_level 4
void itemDataBiosSetFightUseKoukaDataId(u8* p, u16 val) {
    if (p == NULL) return;
    *(u16*)(p + 0xc) = val;
}
#endif
#if 0
asm void itemDataBiosSetItemSoubiDataId(void) {
#include "src/game/people/people_data_fn_80143BE0.inc"
}
#else
#pragma optimization_level 4
void itemDataBiosSetItemSoubiDataId(u8* p, u16 val) {
    if (p == NULL) return;
    *(u16*)(p + 0xa) = val;
}
#endif
#if 0
asm void itemDataBiosSetDoc(void) {
#include "src/game/people/people_data_fn_80143BF0.inc"
}
#else
#pragma optimization_level 4
void itemDataBiosSetDoc(u8* p, u32 val) {
    if (p == NULL) return;
    *(u32*)(p + 0x14) = val;
}
#endif
#if 0
asm void itemDataBiosSetUseful(void) {
#include "src/game/people/people_data_fn_80143C00.inc"
}
#else
#pragma optimization_level 4
void itemDataBiosSetUseful(u8* p, u8 val) {
    if (p == NULL) return;
    p[0x2] = val;
}
#endif
#if 0
asm void itemDataBiosSetImportant(void) {
#include "src/game/people/people_data_fn_80143C10.inc"
}
#else
#pragma optimization_level 4
void itemDataBiosSetImportant(u8* p, u8 val) {
    if (p == NULL) return;
    p[0x1] = val;
}
#endif
#if 0
asm void itemDataBiosSetPrice(void) {
#include "src/game/people/people_data_fn_80143C20.inc"
}
#else
#pragma optimization_level 4
void itemDataBiosSetPrice(u8* p, u16 val) {
    if (p == NULL) return;
    *(u16*)(p + 0x6) = val;
}
#endif
#if 0
asm void itemDataBiosSetKind(void) {
#include "src/game/people/people_data_fn_80143C30.inc"
}
#else
#pragma optimization_level 4
void itemDataBiosSetKind(u8* p, u8 val) {
    if (p == NULL) return;
    p[0x0] = val;
}
#endif
#if 0
asm void itemDataBiosSetName(void) {
#include "src/game/people/people_data_fn_80143C40.inc"
}
#else
#pragma optimization_level 4
void itemDataBiosSetName(u8* p, u32 val) {
    if (p == NULL) return;
    *(u32*)(p + 0x10) = val;
}
#endif

extern u32 lbl_80478BB8;
extern u32 lbl_80478BD0;
extern ItemBallDataBios lbl_80367AF0[];
extern WazaMachineData lbl_80368018[];

#pragma optimization_level 4
u32 itemBiosGetNum(u8* p) {
    ItemBiosData* item;

    item = (ItemBiosData*)p;
    if (item == NULL) return 0;
    return item->num;
}

u32 itemBiosGetItemDataId(u8* p) {
    ItemBiosData* item;

    item = (ItemBiosData*)p;
    if (item == NULL) return 0;
    return item->itemDataId;
}

u32 itemBallDataBiosGetSnatchSnatchWzxDataId(u8* p) {
    ItemBallDataBios* ballData;

    ballData = (ItemBallDataBios*)p;
    if (ballData == NULL) return 0;
    return ballData->snatchSnatchWzxDataId;
}

u32 itemBallDataBiosGetSnatchShakeWzxDataId(u8* p) {
    ItemBallDataBios* ballData;

    ballData = (ItemBallDataBios*)p;
    if (ballData == NULL) return 0;
    return ballData->snatchShakeWzxDataId;
}

u32 itemBallDataBiosGetSnatchPokeoutWzxDataId(u8* p) {
    ItemBallDataBios* ballData;

    ballData = (ItemBallDataBios*)p;
    if (ballData == NULL) return 0;
    return ballData->snatchPokeoutWzxDataId;
}

u32 itemBallDataBiosGetSnatchMissWzxDataId(u8* p) {
    ItemBallDataBios* ballData;

    ballData = (ItemBallDataBios*)p;
    if (ballData == NULL) return 0;
    return ballData->snatchMissWzxDataId;
}

u32 itemBallDataBiosGetSnatchBalllandWzxDataId(u8* p) {
    ItemBallDataBios* ballData;

    ballData = (ItemBallDataBios*)p;
    if (ballData == NULL) return 0;
    return ballData->snatchBalllandWzxDataId;
}

u32 itemBallDataBiosGetSnatchAttackWzxDataId(u8* p) {
    ItemBallDataBios* ballData;

    ballData = (ItemBallDataBios*)p;
    if (ballData == NULL) return 0;
    return ballData->snatchAttackWzxDataId;
}

u32 itemBallDataBiosGetThrowWzxDataId(u8* p) {
    ItemBallDataBios* ballData;

    ballData = (ItemBallDataBios*)p;
    if (ballData == NULL) return 0;
    return ballData->throwWzxDataId;
}

u32 itemBallDataBiosGetDowninWzxDataId(u8* p) {
    ItemBallDataBios* ballData;

    ballData = (ItemBallDataBios*)p;
    if (ballData == NULL) return 0;
    return ballData->downinWzxDataId;
}

u32 itemBallDataBiosGetOutWzxDataId(u8* p) {
    ItemBallDataBios* ballData;

    ballData = (ItemBallDataBios*)p;
    if (ballData == NULL) return 0;
    return ballData->outWzxDataId;
}

u32 itemBallDataBiosGetOpenWzxDataId(u8* p) {
    ItemBallDataBios* ballData;

    ballData = (ItemBallDataBios*)p;
    if (ballData == NULL) return 0;
    return ballData->openWzxDataId;
}

u32 itemBallDataBiosGetInWzxDataId(u8* p) {
    ItemBallDataBios* ballData;

    ballData = (ItemBallDataBios*)p;
    if (ballData == NULL) return 0;
    return ballData->inWzxDataId;
}

u32 itemBallDataBiosGetFightKoukaDataId(u8* p) {
    ItemBallDataBios* ballData;

    ballData = (ItemBallDataBios*)p;
    if (ballData == NULL) return 0;
    return ballData->fightKoukaDataId;
}

u8* itemBallDataBiosGetPtr(u16 idx) {
    u16 ballId;

    ballId = idx;
    if (ballId >= lbl_80478BB8) return NULL;
    return (u8*)&lbl_80367AF0[ballId];
}

u32 itemDataBiosGetBattleUseFunc(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->battleUseFunc;
}

u32 itemDataBiosGetFieldUseFunc(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->fieldUseFunc;
}

u32 itemDataBiosGetItemEffectParam(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->itemEffectParam;
}

u32 itemDataBiosGetBuff(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->buff;
}

s32 itemDataBiosGetUseFriend(u8* p, u16 idx) {
    ItemDataBios* itemData;
    u16 friendIdx;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;

    friendIdx = idx;
    if (friendIdx >= 3) return 0;
    return (s8)((u8)itemData->useFriend[friendIdx]);
}

u32 itemDataBiosGetKinomiNo(u16 itemId) {
    u16 itemNo;

    itemNo = itemId;
    if (itemNo < 0x85 || itemNo > 0xAF) return 0xFF;
    return (u8)(itemId - 0x85);
}

u32 itemDataBiosGetHidenMachineNo(u8* p) {
    ItemDataBios* itemData;
    u8 machineNo;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) {
        machineNo = 0;
    } else if (itemData->kind != 4) {
        machineNo = 0xFF;
    } else {
        machineNo = (u8)itemData->buff;
    }

    if (machineNo == 0xFF) return 0xFF;
    if (machineNo < 0x32 || machineNo >= lbl_80478BD0) return 0xFF;
    return (u8)(machineNo - 0x32);
}

u32 itemDataBiosGetWazaIDByWazaMachineNo(u32 machineNo) {
    if ((u8)machineNo >= lbl_80478BD0) return 0;
    return (u16)lbl_80368018[(u8)machineNo].wazaId;
}

u32 itemDataBiosGetWazaMachineNo(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    if (itemData->kind != 4) return 0xFF;
    return (u8)itemData->buff;
}

u32 itemDataBiosGetFightUseKoukaDataId(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->fightUseKoukaDataId;
}

u32 itemDataBiosGetItemSoubiDataId(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->itemSoubiDataId;
}

u32 itemDataBiosGetDoc(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->doc;
}

u8 fn_80143F9C(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->field_03;
}

u8 fn_80143FB4(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->useful;
}

u8 fn_80143FCC(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->important;
}

u32 itemDataBiosGetCoupon(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->coupon;
}

u32 itemDataBiosGetPrice(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->price;
}

u8 itemDataBiosGetKind(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->kind;
}

s32 itemDataBiosCheckExportable(u8* p) {
    ItemDataBios* itemData;
    u8 important;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    if (itemData == NULL) {
        important = 0;
    } else {
        important = itemData->important;
    }
    if (important != 0) return 0;
    return 1;
}

s32 itemDataBiosCheckImportable(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->name != 0;
}

u32 itemDataBiosGetName(u8* p) {
    ItemDataBios* itemData;

    itemData = (ItemDataBios*)p;
    if (itemData == NULL) return 0;
    return itemData->name;
}

s32 itemUse2PokemonSimulation(void* arg0, void* arg1, void* arg2, void* arg3, void* arg4) {
    void* savedFightFloor;
    void* fightFloor;
    u16 memHandle;
    s32 result;

    fightFloor = fightFloorGetNowPtr();
    if (fightFloor == NULL) {
        return 0;
    }

    memHandle = _toolentryAlloc__FUl(0xA4E8);
    savedFightFloor = fn_800E27B0(memHandle);
    memcpy(savedFightFloor, fightFloor, 0xA4E8);
    result = fn_80144574(arg0, arg1, arg2, arg3, arg4);
    memcpy(fightFloor, savedFightFloor, 0xA4E8);
    fn_800E24B0(memHandle);
    fn_800E209C(memHandle);

    return result;
}
