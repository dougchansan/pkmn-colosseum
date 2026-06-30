/**
 * @file people.c
 * @brief Core People/NPC system -- init, alloc, update, spawn, despawn.
 *
 * Decompiled from:
 *   fn_8018FDD0 (peopleInit)         -- allocate array for N people slots
 *   fn_8018FDB4 (peopleGetMaxCount)  -- return max slot count
 *   fn_8018FD88 (peopleGetEntry)     -- index -> PeopleEntry*
 *   fn_8018FCE0 (peopleAlloc)        -- find free slot, memset, mark active
 *   fn_8018FDBC (peopleFree)         -- mark slot inactive
 *   fn_80181850 (peopleUpdate)       -- per-frame update loop for all NPCs
 *   fn_80181224 (peopleFloorInit)    -- draw 30 blank frames at floor start
 *   fn_80180C78 (peopleOpenCallback) -- floor loader callback (opens people)
 *   fn_80181094 (peopleOpenThread)   -- continuation callback after open
 *   fn_8018114C (peopleCloseCallback)-- cleanup callback
 *   fn_8018FBD4 (peopleGetModel)     -- return modelHandle from entry
 *   fn_8018FB94 (peopleTestFlags)    -- test flag bits
 *   fn_8018FBBC (peopleSetFlags)     -- set flag bits
 *   fn_8018FBAC (peopleClearFlags)   -- clear flag bits
 *   fn_8018FBCC (peopleWriteFlags)   -- write all flags
 *   fn_8018FBDC (peopleSetTransform) -- copy 3x3 matrix into entry
 *   fn_8018FC00 (peopleGetTransform) -- return pointer to entry transform
 *
 * Address range: 0x80180C78 - 0x80181850, 0x8018FB94 - 0x8018FE30
 *
 * Global state:
 *   lbl_8047B1F8 (sbss) -- s32  gPeopleMaxCount
 *   lbl_8047B200 (sbss) -- PeopleEntry* gPeopleArray  (heap-allocated)
 *   lbl_8047B1FC (sbss) -- u16  gPeopleMemHandle (GSmem handle)
 *   lbl_8047B1E0 (sbss) -- void* gPeopleFloorObj (current floor link)
 *   lbl_8047B1E4 (sbss) -- PeopleOpenWork* gPeopleOpenWork
 *   lbl_8047B1E8 (sbss) -- s32  gPeopleOpenCount
 */

#include "dolphin/types.h"
#include "game/people/people.h"

/* ===== External SDK / engine functions ===== */
extern void  fn_800DD970(const char* fmt, ...);     /* OSReport / debug printf */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* External functions referenced from asm wrappers */
extern void fn_800DCC3C(void);
extern void fn_800DCC60(void);
extern void fn_800E01F4(void);
extern void fn_800E3CF8(void*);
extern void fn_800E3D00(void*);
extern void fn_800E3D08(void*);
extern void fn_800E3D6C(void*);
extern void fn_800E3D98(void*, void*);
extern void fn_800E4014(void*, u8);
extern void fn_800E4170(void*);
extern void fn_800E43A4(void*);
extern void fn_800E9B2C(void);
extern void fn_800E9C6C(void);
extern void fn_800EC2A4(void);
extern void fn_800EC308(void);
extern void fn_800EC35C(void);
extern void fn_800EC578(void);
extern void fn_800EC954(void);
extern void fn_800EC960(void);
extern void fn_800EC9DC(void);
extern void fn_800ECA78(void);
extern void fn_800ECB74(void);
extern void fn_800ECCA8(void);
extern void fn_8010FFC4(void);
extern void fn_801812C4(void);
extern void fn_801848D0(void);
extern void fn_80188AF4(void);
extern void fn_80188CA0(void);
extern void fn_80188FA0(void);
extern void fn_8018E1C4(void);
extern void fn_8018F08C(void);
extern void fn_8018F4C8(void);
extern void fn_8018F6F4(void);
extern void fn_8018FBD4(void);
extern void fn_8018FCBC(void);
extern void fn_8018FD88(void);
extern void fn_8018FDB4(void);

/* GSmem allocator */
extern u16   fn_800E3534(u32 size);                 /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);               /* GSmemGetPtr */

/* Floor/field system */
extern void* fn_80167F28(const char* name);         /* field lookup by name */
extern void* fn_80167E5C(void);                     /* get current field */
extern void  fn_80167ED0(void* field, void* obj, void* data, u32 param);
extern void  fn_80167E64(void* field);              /* field finalize */

/* Thread/task system */
extern void* fn_800FE834(u32 pri, u32 type, void* taskBuf, void* callback);
extern void  fn_800FE714(void* task);               /* task cleanup */

/* Model system */
extern void  fn_8017BB80(void* floorObj, void* modelData); /* model open */
extern void  fn_8017BC90(void* floorObj, u32 modelId, u32 param, void* extraData, u32 extra2);

/* Flag system */
extern BOOL  fn_800F7108(u16 flagId);               /* GSflagGet (bit check) */

/* Collision/model helpers */
extern void  fn_800E01D0(void* dst, void* src);     /* matrix/vector copy */
extern void  fn_800E0168(void* dst, void* srcA, void* srcB);  /* cross/setup */

/* ===== Rodata string references ===== */
extern const char lbl_80273F80[];  /* floor name for blank-frame init */
extern const char lbl_80273FD8[];  /* "Warining: people[%d,%d] group is different!!\n" */

/* ===== Global state (sbss / sdata) ===== */

/* lbl_8047B1F8 @sda21 : maximum people count */
static s32 gPeopleMaxCount;

/* lbl_8047B200 @sda21 : pointer to people array (heap-allocated) */
static PeopleEntry* gPeopleArray;

/* lbl_8047B1FC @sda21 : GSmem handle for the people array */
static u16 gPeopleMemHandle;

/* lbl_8047B1E0 @sda21 : current floor object link */
static void* gPeopleFloorObj;

/* lbl_8047B1E4 @sda21 : active PeopleOpenWork pointer */
static PeopleOpenWork* gPeopleOpenWork;

/* lbl_8047B1E8 @sda21 : people open count/max for linked list */
static s32 gPeopleOpenCount;

/* ===== Sdata2 float constants ===== */
/* lbl_8047D798 @sda21 : constant used in peopleMoveUpdate */
/* lbl_8047D79C @sda21 : float 0.0 */
/* lbl_8047D7A0 @sda21 : float 0.0 (zero) */
/* lbl_8047D7A4 @sda21 : float 1.0 */
/* lbl_8047D8B0 @sda21 : float default moveSpeed */

/* =======================================================================
 * fn_8018FDD0 -- peopleInit
 *
 * Allocate and zero-initialize a flat array of PeopleEntry slots.
 * Called during floor loading to prepare the NPC pool.
 *
 * r3 = maxPeople (number of slots)
 * Returns: pointer to the people array (gPeopleArray)
 * ======================================================================= */
PeopleEntry* peopleInit(u32 maxPeople)
{
    u32 totalSize;

    totalSize = maxPeople * PEOPLE_ENTRY_SIZE;

    /* Allocate from GSmem */
    gPeopleMemHandle = fn_800E3534(totalSize);
    gPeopleArray = (PeopleEntry*)fn_800E27B0(gPeopleMemHandle);

    /* Zero-fill entire array */
    memset(gPeopleArray, 0, totalSize);

    /* Store max count */
    gPeopleMaxCount = (s32)maxPeople;

    return gPeopleArray;
}

/* =======================================================================
 * fn_8018FDB4 -- peopleGetMaxCount
 *
 * Return the maximum number of people slots.
 * Frequently called in loops as the upper bound.
 * ======================================================================= */
s32 peopleGetMaxCount(void)
{
    return gPeopleMaxCount;
}

/* =======================================================================
 * fn_8018FD88 -- peopleGetEntry
 *
 * Return a pointer to the PeopleEntry at the given index.
 * Bounds-checked: returns NULL if index < 0 or >= maxCount.
 *
 * r3 = index
 * Returns: PeopleEntry* or NULL
 * ======================================================================= */
PeopleEntry* peopleGetEntry(s32 index)
{
    if (index < 0 || gPeopleMaxCount <= index) {
        return NULL;
    }
    return (PeopleEntry*)((u8*)gPeopleArray + index * PEOPLE_ENTRY_SIZE);
}

/* =======================================================================
 * fn_8018FCE0 -- peopleAlloc
 *
 * Find the first free (inactive) slot in the people array, zero it,
 * and mark it as active. Sets up self-pointer, shadow ID, and move speed.
 *
 * Returns: PeopleEntry* to the newly allocated slot, or NULL if full.
 * ======================================================================= */
PeopleEntry* peopleAlloc(void)
{
    s32 i;
    s32 maxCount;
    PeopleEntry* entry;
    PeopleEntry* found;

    maxCount = gPeopleMaxCount;
    entry = gPeopleArray;

    /* Use CTR-based countdown loop (matches bdnz in asm) */
    for (i = 0; maxCount > 0; maxCount--) {
        if (i < 0 || gPeopleMaxCount <= i) {
            found = NULL;
        } else {
            found = entry;
        }

        if (found->active == 0) {
            /* Found a free slot */
            memset(found, 0, PEOPLE_ENTRY_SIZE);

            found->active = 1;
            found->selfPtr = found;          /* self-pointer for script lookup */
            found->shadowId = -1;            /* no shadow by default */
            /* found->moveSpeed = default float from sdata2 */

            return found;
        }

        entry = (PeopleEntry*)((u8*)entry + PEOPLE_ENTRY_SIZE);
        i++;
    }

    return NULL;
}

/* =======================================================================
 * fn_8018FDBC -- peopleFree
 *
 * Mark a people entry as inactive. Clears both the active flag and the
 * visible flag.
 *
 * r3 = PeopleEntry*
 * Returns: 1
 * ======================================================================= */
s32 peopleFree(PeopleEntry* entry)
{
    entry->active = 0;
    entry->visible = 0;
    return 1;
}

/* =======================================================================
 * fn_8018FBD4 -- peopleGetModel
 *
 * Return the model handle from a people entry.
 *
 * r3 = PeopleEntry*
 * Returns: modelHandle (offset 0x08)
 * ======================================================================= */
void* peopleGetModel(PeopleEntry* entry)
{
    return entry->modelHandle;
}

/* =======================================================================
 * fn_8018FB94 -- peopleTestFlags
 *
 * Test whether any of the bits in 'mask' are set in the entry's flags.
 *
 * r3 = PeopleEntry*
 * r4 = mask
 * Returns: 1 if any bits match, 0 otherwise
 * ======================================================================= */
BOOL peopleTestFlags(PeopleEntry* entry, u32 mask)
{
    u32 result;

    result = entry->flags & mask;
    /* Convert nonzero to 1: ((-x) | x) >> 31 */
    return (u32)(((s32)(-result) | (s32)result) >> 31) & 1;
}

/* =======================================================================
 * fn_8018FBBC -- peopleSetFlags
 *
 * Set (OR) flag bits on a people entry.
 * ======================================================================= */
void peopleSetFlags(PeopleEntry* entry, u32 mask)
{
    entry->flags |= mask;
}

/* =======================================================================
 * fn_8018FBAC -- peopleClearFlags
 *
 * Clear (AND-NOT) flag bits on a people entry.
 * ======================================================================= */
void peopleClearFlags(PeopleEntry* entry, u32 mask)
{
    entry->flags &= ~mask;
}

/* =======================================================================
 * fn_8018FBCC -- peopleWriteFlags
 *
 * Overwrite all flags on a people entry.
 * ======================================================================= */
void peopleWriteFlags(PeopleEntry* entry, u32 flags)
{
    entry->flags = flags;
}

/* =======================================================================
 * fn_8018FBDC -- peopleSetTransform
 *
 * Copy a 3x3 matrix (or vector) into the entry's transform at +0x9C.
 * Delegates to fn_800E01D0 (matrix/vector copy).
 *
 * r3 = PeopleEntry*
 * r4 = source matrix pointer
 * ======================================================================= */
void peopleSetTransform(PeopleEntry* entry, void* mtx)
{
    fn_800E01D0((u8*)entry + 0x9C, mtx);
}

/* =======================================================================
 * fn_8018FC00 -- peopleGetTransform
 *
 * Return a pointer to the entry's transform data at +0x9C.
 * ======================================================================= */
void* peopleGetTransform(PeopleEntry* entry)
{
    return (u8*)entry + 0x9C;
}

/* =======================================================================
 * fn_80181224 -- peopleFloorInit
 *
 * Called when a floor is being set up. Draws 30 blank frames via the
 * field system, then resets the open work state.
 *
 * This ensures all pending model loads and animation setups complete
 * before the floor becomes visible.
 * ======================================================================= */
void peopleFloorInit(void)
{
    void* field;
    void* fieldData;
    s32 i;

    field = fn_80167F28(lbl_80273F80);
    fieldData = fn_80167E5C();

    for (i = 0; i < 30; i++) {
        fn_80167ED0(field, gPeopleFloorObj, fieldData, 0);
    }

    fn_80167E64(field);

    /* Reset open work state */
    gPeopleOpenWork->subState = 0;

    /* Clean up the thread/task */
    fn_800FE714(gPeopleOpenWork->threadObj);
}

/* =======================================================================
 * fn_80181850 -- peopleUpdate
 *
 * Main per-frame update loop for all active people entries.
 * Called from the main game loop (via fn_80005FE0 -> bl fn_80181850).
 *
 * For each active entry:
 *   1. Check HAS_MODEL flag -> update collision position if set
 *   2. Check visibility (visible flag or game flag check)
 *   3. If visible and not in talk-lock: run movement update
 *   4. If active: run 60 ticks of movement simulation
 *
 * This is one of the larger functions (0x660 bytes = 1632 bytes).
 * ======================================================================= */
void peopleUpdate(void)
{
    s32 maxCount;
    s32 i;
    PeopleEntry* entry;
    s32 shouldUpdate;
    u32 j;

    maxCount = peopleGetMaxCount();

    for (i = maxCount - 1; i >= 0; /* decremented at bottom */) {
        entry = peopleGetEntry(i);
        if (entry->active == 0) {
            goto next_entry;
        }

        /* Check if model has PEOPLE_FLAG_HAS_MODEL flag set */
        if (peopleTestFlags(entry, PEOPLE_FLAG_HAS_MODEL)) {
            /* Update collision/interaction position from model world coords */
            void* modelPos;
            void* modelTrans;
            modelPos = peopleGetModelPosition(entry);
            fn_800E01D0((u8*)entry + 0x2C, modelPos);  /* simplified */

            modelTrans = peopleGetTransform(entry);
            fn_800E01D0((u8*)entry + 0x38, modelTrans); /* simplified */
        }

        /* Update entry position tracking */
        peopleSetPosition(entry, (void*)((u8*)entry + 0x44));
        peopleSetTransform(entry, (void*)((u8*)entry + 0x44));

        /* Reset talk range to 0 */
        entry->talkRange = 0.0f;

        /* Determine visibility */
        if (entry->visible != 0) {
            shouldUpdate = 1;
        } else {
            /* Check game flag for dynamic visibility */
            if (fn_800F7108(entry->flagId) == 0) {
                shouldUpdate = 1;
            } else {
                shouldUpdate = 0;
            }
        }

        if (shouldUpdate) {
            /* Skip if NPC is locked in talk interaction */
            if (entry->talkLock != 0) {
                goto check_movement;
            }
            /* ... (extensive state machine for walk, idle, talk transitions) ... */
        }

    check_movement:
        /* Run movement dispatch based on moveType */
        if (entry->moveType != PEOPLE_MOVE_NONE) {
            for (j = 0; j < PEOPLE_UPDATE_TICK_COUNT; j++) {
                peopleMoveUpdate(entry);
            }
        }

    next_entry:
        i--;
    }
}

/* 0x8018A44C | 0x2B4 */
extern void fn_8018FC2C(void);
extern u32 lbl_8047D814;
extern u32 lbl_8047D7C0;
extern u32 lbl_8047D7C8;
#if 1
asm void fn_8018A44C(void) {
#include "src/game/people/people_fn_8018A44C.inc"
}
#else
void fn_8018A44C(void) {
    /* TODO: match -- 692 bytes at 0x8018A44C */
}
#endif

/* 0x8018A700 | 0x3CC */
extern void fn_8018FC98(void);
extern void fn_800E008C(void);
extern void fn_800E013C(void);
extern void fn_800E019C(void);
extern void fn_8018AACC(void);
extern u32 lbl_8047D800;
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D79C;
#if 1
asm void fn_8018A700(void) {
#include "src/game/people/people_fn_8018A700.inc"
}
#else
void fn_8018A700(void) {
    /* TODO: match -- 972 bytes at 0x8018A700 */
}
#endif

/* 0x8018AEC0 | 0x1BC */
extern void _threadSwitch(void);
extern u32 lbl_8047D79C;
extern u8 lbl_80274008[];
extern u8 lbl_8036C510[];
#if 1
asm void fn_8018AEC0(void) {
#include "src/game/people/people_fn_8018AEC0.inc"
}
#else
void fn_8018AEC0(void) {
    /* TODO: match -- 444 bytes at 0x8018AEC0 */
}
#endif

/* 0x8018B220 | 0x148 */
extern void fn_800EC96C(void);
#if 1
asm void fn_8018B220(void) {
#include "src/game/people/people_fn_8018B220.inc"
}
#else
void fn_8018B220(u32 groupId, u32 index) {
    PeopleEntry* found;
    s32 i;
    s32 j;
    PeopleEntry* entry;

    found = NULL;

    /* Loop 1: search by groupId + index */
    for (i = 0; i < fn_8018FDB4(); i++) {
        entry = (PeopleEntry*)fn_8018FD88(i);
        if (*(u8*)entry == 0) continue;
        if (*(u32*)((u8*)entry + 0x28) != groupId) continue;
        if (*(u32*)((u8*)entry + 0x2C) != index) continue;
        found = *(PeopleEntry**)((u8*)entry + 0x04);
        goto loop3;
    }

    /* Loop 2: fallback search by index only */
    for (j = 0; j < fn_8018FDB4(); j++) {
        entry = (PeopleEntry*)fn_8018FD88(j);
        found = entry;
        if (*(u8*)entry == 0) continue;
        if (*(u32*)((u8*)found + 0x2C) != index) continue;
        fn_800DD970((u8*)lbl_80273FD8, groupId, index);
        found = *(PeopleEntry**)((u8*)found + 0x04);
        goto loop3;
    }
    found = NULL;

loop3:
    /* Loop 3: search by selfPtr */
    for (j = 0; j < fn_8018FDB4(); j++) {
        entry = (PeopleEntry*)fn_8018FD88(j);
        if (*(u8*)entry == 0) continue;
        if (*(u32*)((u8*)entry + 0x04) != (u32)found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry != NULL) {
        void* model = fn_8018FBD4(entry);
        if (model != NULL) {
            fn_800EC96C(model);
        }
    }
}
#endif

/* 0x8018B368 | 0x1F0 */
extern void fn_800EC990(void);
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D7C8;
extern u32 lbl_8047D7A4;
#if 1
asm void fn_8018B368(void) {
#include "src/game/people/people_fn_8018B368.inc"
}
#else
void fn_8018B368(void) {
    /* TODO: match -- 496 bytes at 0x8018B368 */
}
#endif

/* 0x8018B558 | 0x214 */
extern void fn_800EC5FC(void);
extern void fn_800EC5B8(void);
extern void fn_800EC4D0(void);
extern void fn_800EC8C8(void);
extern u32 lbl_8047D7D0;
extern u32 lbl_8047D79C;
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D834;
#if 1
asm void fn_8018B558(void) {
#include "src/game/people/people_fn_8018B558.inc"
}
#else
void fn_8018B558(void) {
    /* TODO: match -- 532 bytes at 0x8018B558 */
}
#endif

/* 0x8018BC88 | 0x16C */
extern void fn_800EE150(void);
extern void fn_800EE3BC(void);
extern void fn_800EE828(void);
#if 1
asm void fn_8018BC88(void) {
#include "src/game/people/people_fn_8018BC88.inc"
}
#else
void fn_8018BC88(void) {
    /* TODO: match -- 364 bytes at 0x8018BC88 */
}
#endif

/* 0x8018D680 | 0x150 */
extern void fn_800CE2D8(void);
extern void fn_800CE318(void);
extern void fn_800E0718(void);
extern void fn_800DFEEC(void);
extern void fn_800E0040(void);
extern u32 lbl_8047D7A4;
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D7F0;
extern u32 lbl_8047D7A8;
extern u32 lbl_8047D820;
extern u8 lbl_8031554C[];
#if 1
asm void fn_8018D680(void) {
#include "src/game/people/people_fn_8018D680.inc"
}
#else
void fn_8018D680(void) {
    /* TODO: match -- 336 bytes at 0x8018D680 */
}
#endif

/* 0x8018DB68 | 0x140 */
extern void fn_8018DCA8(void);
#if 1
asm void fn_8018DB68(void) {
#include "src/game/people/people_fn_8018DB68.inc"
}
#else
void fn_8018DB68(void) {
    /* TODO: match -- 320 bytes at 0x8018DB68 */
}
#endif

/* 0x8018DCA8 | 0x3A8 */
extern void fn_8018FB60(void);
extern void fn_8018FB2C(void);
extern void fn_800E24B0(void);
extern void fn_800E209C(void);
extern void fn_800F9210(void);
extern void fn_800E4BF4(void);
extern void fn_8018FDBC(void);
#if 1
asm void fn_8018DCA8(void) {
#include "src/game/people/people_fn_8018DCA8.inc"
}
#else
void fn_8018DCA8(void) {
    /* TODO: match -- 936 bytes at 0x8018DCA8 */
}
#endif

/* 0x8018E9B4 | 0x338 */
extern void fn_800F9318(void);
extern void fn_800F7BC4(void);
extern void fn_8018FC74(void);
extern void fn_80101B90(void);
extern void fn_8018FB94(void);
extern void fn_8018F5E4(void);
extern void fn_8011163C(void);
extern void fn_8012BAF0(void);
extern void fn_8010FDF8(void);
extern void fn_8010F320(void);
extern void fn_800A3A9C(void);
extern void fn_800A3A78(void);
extern void fn_801101B4(void);
extern void fn_8010E138(void);
extern u32 lbl_8047D890;
extern const f32 lbl_8047D7EC;
extern u32 lbl_8047D894;
extern u32 lbl_8047D800;
extern u32 lbl_8047D7A0;
/* renamed symbols referenced by asm incs (symbolmap port) */
extern void heroMoveSetEventList();
extern void sin();   /* MSL trig (renamed) — referenced by asm incs */
extern void cos();   /* MSL trig (renamed) — referenced by asm incs */
extern void GScolsy2UtilGetCpPlanePoint();
extern void GScolsy2UtilChkInTri();
extern void GSmodelPopState();
#if 1
asm void fn_8018E9B4(void) {
#include "src/game/people/people_fn_8018E9B4.inc"
}
#else
void fn_8018E9B4(void) {
    /* TODO: match -- 824 bytes at 0x8018E9B4 */
}
#endif

/* 0x8018ECEC | 0x3A0 */
extern void fn_8018F698(void);
extern void fn_8018F678(void);
extern void fn_8018F658(void);
extern void fn_8018F638(void);
extern void fn_8018F618(void);
extern void fn_800D3088(void);
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D7F0;
extern u32 lbl_8047D7A8;
extern u32 lbl_8047D820;
extern u32 lbl_8047D898;
extern u32 lbl_8047D7B0;
extern u32 lbl_8047D7D0;
extern u32 lbl_8047D89C;
#if 1
asm void fn_8018ECEC(void) {
#include "src/game/people/people_fn_8018ECEC.inc"
}
#else
void fn_8018ECEC(void) {
    /* TODO: match -- 928 bytes at 0x8018ECEC */
}
#endif

/* WP-0010 stubs */

/* 0x80181478 | 0x3D8 */
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D7A4;
#if 1
asm void fn_80181478(void) {
#include "src/game/people/people_fn_80181478.inc"
}
#else
void fn_80181478(void) { /* TODO: match -- 984 bytes at 0x80181478 */ }
#endif

/* 0x80181EB0 | 0x308 */
extern void fn_80113F48(void);
extern void fn_801CBA0C(void);
extern void fn_801845E4(void);
extern void fn_800E3CC8(void);
extern void fn_800E90C8(void);
extern void fn_801CB834(void);
extern void fn_80166A28(void);
extern void fn_800F7318(void);
#if 1
asm void fn_80181EB0(void) {
#include "src/game/people/people_fn_80181EB0.inc"
}
#else
void fn_80181EB0(void) { /* TODO: match -- 776 bytes at 0x80181EB0 */ }
#endif

/* 0x801821B8 | 0xE60 */
extern void fn_8018FBAC(void);
extern void fn_8018FBDC(void);
extern u32 lbl_8047D7D8;
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D7A4;
extern u32 lbl_8047D7D0;
extern u32 lbl_8047D7E0;
extern u8 lbl_8036C4F8[];
#if 1
asm void fn_801821B8(void) {
#include "src/game/people/people_fn_801821B8.inc"
}
#else
void fn_801821B8(void) { /* TODO: match -- 3680 bytes at 0x801821B8 */ }
#endif

/* 0x801837D8 | 0x180 */
#if 1
asm void fn_801837D8(void) {
#include "src/game/people/people_fn_801837D8.inc"
}
#else
void fn_801837D8(void) { /* TODO: match -- 384 bytes at 0x801837D8 */ }
#endif

/* 0x80183958 | 0x24 */
extern void fn_801170A4(void);
extern void fn_80116EC8(void);
#if 0
asm void fn_80183958(void) {
#include "src/game/people/people_fn_80183958.inc"
}
#else
#pragma optimization_level 4
void fn_80183958(void) {
    fn_801170A4();
    fn_80116EC8();
}
#endif

/* 0x801839A0 | 0x1A4 */
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D79C;
#if 1
asm void fn_801839A0(void) {
#include "src/game/people/people_fn_801839A0.inc"
}
#else
void fn_801839A0(void) { /* TODO: match -- 420 bytes at 0x801839A0 */ }
#endif

/* 0x80183B44 | 0x19C */
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D7E8;
extern const f32 lbl_8047D7EC;
extern u32 lbl_8047D79C;
#if 1
asm void fn_80183B44(void) {
#include "src/game/people/people_fn_80183B44.inc"
}
#else
void fn_80183B44(void) { /* TODO: match -- 412 bytes at 0x80183B44 */ }
#endif

/* 0x80183CE0 | 0x17C */
#if 1
asm void fn_80183CE0(void) {
#include "src/game/people/people_fn_80183CE0.inc"
}
#else
void fn_80183CE0(void) { /* TODO: match -- 380 bytes at 0x80183CE0 */ }
#endif

/* 0x80183E5C | 0x168 */
extern u32 lbl_8047D79C;
#if 1
asm void fn_80183E5C(void) {
#include "src/game/people/people_fn_80183E5C.inc"
}
#else
void fn_80183E5C(void) { /* TODO: match -- 360 bytes at 0x80183E5C */ }
#endif

/* 0x80183FC4 | 0x1CC */
extern u8 lbl_8027404C[];
#if 1
asm void fn_80183FC4(void) {
#include "src/game/people/people_fn_80183FC4.inc"
}
#else
void fn_80183FC4(void) { /* TODO: match -- 460 bytes at 0x80183FC4 */ }
#endif

/* 0x80184190 | 0x2C0 */
#if 1
asm void fn_80184190(void) {
#include "src/game/people/people_fn_80184190.inc"
}
#else
void fn_80184190(void) { /* TODO: match -- 704 bytes at 0x80184190 */ }
#endif

/* 0x80184450 | 0x20 */
#if 0
asm void fn_80184450(void) {
#include "src/game/people/people_fn_80184450.inc"
}
#else
#pragma optimization_level 4
void fn_80184450(void) {
    _threadSwitch();
}
#endif

/* 0x80184948 | 0x148 */
#if 1
asm void fn_80184948(void) {
#include "src/game/people/people_fn_80184948.inc"
}
#else
void fn_80184948(void) { /* TODO: match -- 328 bytes at 0x80184948 */ }
#endif

/* 0x80184A90 | 0x2F0 */
extern void fn_8018FC08(void);
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D7F0;
extern u32 lbl_8047D7F8;
extern u32 lbl_8047D7A8;
extern u32 lbl_8047D7FC;
extern u32 lbl_8047D7C0;
extern u32 lbl_8047D804;
extern u32 lbl_8047D800;
extern u32 lbl_8047D808;
extern u32 lbl_8047D810;
extern u32 lbl_8047D814;
extern u32 lbl_8047D7D0;
#if 1
asm void fn_80184A90(void) {
#include "src/game/people/people_fn_80184A90.inc"
}
#else
void fn_80184A90(void) { /* TODO: match -- 752 bytes at 0x80184A90 */ }
#endif

/* 0x80184D80 | 0x4CC */
extern void fn_800D37CC(void);
extern void fn_800E0BE4(void);
extern void fn_800CE148(void);
extern void fn_800CDBE0(void);
extern void fn_800E0BA0(void);
extern u8 lbl_80273FC0[];
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D7C8;
extern u32 lbl_8047D7D0;
extern u32 lbl_8047D7C4;
extern u32 lbl_8047D818;
extern u32 lbl_8047D7F0;
extern u32 lbl_8047D7A8;
extern u32 lbl_8047D820;
extern u32 lbl_8047D7C0;
extern u32 lbl_8047D79C;
extern void fn_80185AAC(void);
#if 1
asm void fn_80184D80(void) {
#include "src/game/people/people_fn_80184D80.inc"
}
#else
void fn_80184D80(void) { /* TODO: match -- 1228 bytes at 0x80184D80 */ }
#endif

/* 0x8018524C | 0x678 */
extern void fn_80188214(void);
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D7F0;
extern u32 lbl_8047D7A8;
extern u32 lbl_8047D820;
extern u32 lbl_8047D7C0;
extern u32 lbl_8047D7C8;
extern u32 lbl_8047D79C;
extern u32 lbl_8047D7C4;
#if 1
asm void fn_8018524C(void) {
#include "src/game/people/people_fn_8018524C.inc"
}
#else
void fn_8018524C(void) { /* TODO: match -- 1656 bytes at 0x8018524C */ }
#endif

/* 0x801858C4 | 0x1E8 */
extern void fn_8018FC00(void);
extern u32 lbl_8047D828;
#if 1
asm void fn_801858C4(void) {
#include "src/game/people/people_fn_801858C4.inc"
}
#else
void fn_801858C4(void) { /* TODO: match -- 488 bytes at 0x801858C4 */ }
#endif

/* 0x80185AAC | 0xE4 */
#if 1
asm void fn_80185AAC(void) {
#include "src/game/people/people_fn_80185AAC.inc"
}
#else
void fn_80185AAC(void) { /* TODO: match -- 228 bytes at 0x80185AAC */ }
#endif

/* 0x80185B90 | 0x358 */
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D7D0;
extern u32 lbl_8047D79C;
extern u32 lbl_8047D830;
extern u32 lbl_8047D834;
extern u32 lbl_8047D7A4;
#if 1
asm void fn_80185B90(void) {
#include "src/game/people/people_fn_80185B90.inc"
}
#else
void fn_80185B90(void) { /* TODO: match -- 856 bytes at 0x80185B90 */ }
#endif

/* 0x80185F44 | 0x1B4 */
extern u32 lbl_8047D814;
extern u32 lbl_8047D7F0;
#if 1
asm void fn_80185F44(void) {
#include "src/game/people/people_fn_80185F44.inc"
}
#else
void fn_80185F44(void) { /* TODO: match -- 436 bytes at 0x80185F44 */ }
#endif

/* 0x80186254 | 0x30 */
extern const f32 lbl_8047D7EC;
extern const f32 lbl_8047D838;
extern void fn_80186284(u32 a, u32 b, f32 x, s32 c, s32 d, f32 y);
#if 0
asm void fn_80186254(void) {
#include "src/game/people/people_fn_80186254.inc"
}
#else
#pragma push
#pragma scheduling on
void fn_80186254(u32 a, u32 b) {
    fn_80186284(a, b, lbl_8047D7EC, 0, 100, lbl_8047D838);
}
#pragma pop
#endif

/* 0x80186284 | 0x39C */
extern void fn_8010F188(void);
extern u32 lbl_8047D83C;
extern u32 lbl_8047D800;
extern u32 lbl_8047D7F0;
extern u32 lbl_8047D7A8;
extern u32 lbl_8047D820;
extern u32 lbl_8047D814;
extern u32 lbl_8047D7A4;
#if 1
asm void fn_80186284(u32 a, u32 b, f32 x, s32 c, s32 d, f32 y) {
#include "src/game/people/people_fn_80186284.inc"
}
#else
void fn_80186284(void) { /* TODO: match -- 924 bytes at 0x80186284 */ }
#endif

/* 0x80186620 | 0x53C */
extern void fn_801779EC(void);
extern void fn_800D258C(void);
extern void fn_800D2248(void);
extern void fn_800DA028(void);
extern void fn_800D7820(void);
extern void fn_800D88DC(void);
extern void fn_800D888C(void);
extern void fn_800DA4C4(void);
extern void fn_800D9ED8(void);
extern void fn_800D6A00(void);
extern void fn_800D67BC(void);
extern void fn_800D6680(void);
extern void fn_800D5CB8(void);
extern void fn_800D6728(void);
extern void fn_800E0060(void);
extern void fn_800E0000(void);
extern void fn_8010F6A0(void);
extern void fn_8012B184(void);
extern u32 lbl_8047D840;
extern u8 lbl_80314638[];
extern u32 lbl_8047D844;
extern u32 lbl_8047D83C;
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D79C;
extern u32 lbl_8047D848;
extern void fn_80186B5C(void);
extern void fn_801870E8(void);
#if 1
asm void fn_80186620(void) {
#include "src/game/people/people_fn_80186620.inc"
}
#else
void fn_80186620(void) { /* TODO: match -- 1340 bytes at 0x80186620 */ }
#endif

/* 0x80186B5C | 0x58C */
extern void fn_800F7A7C(void);
extern void fn_800F7A08(void);
extern void fn_80176684(void);
extern u8 lbl_80273FB4[];
extern u32 lbl_8047D7C8;
extern u32 lbl_8047D84C;
extern u32 lbl_8047D7A0;
extern u32 lbl_8047D850;
extern u32 lbl_8047D858;
extern u32 lbl_8047D860;
extern u8 lbl_80478AC0[];
extern u32 lbl_8047D7C4;
extern u32 lbl_8047D868;
extern u32 lbl_8047D86C;
extern u32 lbl_8047D7E8;
extern u32 lbl_8047D870;
extern u32 lbl_8047D818;
extern u32 lbl_8047D830;
extern u32 lbl_8047D7A4;
extern u32 lbl_8047D79C;
extern u32 lbl_8047D874;
extern u32 lbl_8047D7D0;
#if 1
asm void fn_80186B5C(void) {
#include "src/game/people/people_fn_80186B5C.inc"
}
#else
void fn_80186B5C(void) { /* TODO: match -- 1420 bytes at 0x80186B5C */ }
#endif

/* 0x801870E8 | 0x3D4 */
extern void fn_8010C77C(void);
extern void fn_8010FA54(void);
extern void fn_800A3BD8(void);
extern void fn_8010F71C(void);
extern u32 lbl_8047D844;
extern u32 lbl_8047D7A0;
#if 1
asm void fn_801870E8(void) {
#include "src/game/people/people_fn_801870E8.inc"
}
#else
void fn_801870E8(void) { /* TODO: match -- 980 bytes at 0x801870E8 */ }
#endif

/* 0x801874BC | 0x450 */
extern u32 lbl_8047D844;
#if 1
asm void fn_801874BC(void) {
#include "src/game/people/people_fn_801874BC.inc"
}
#else
void fn_801874BC(void) { /* TODO: match -- 1104 bytes at 0x801874BC */ }
#endif

/* 0x80187A60 | 0x2E8 */
extern u32 lbl_8047D7C0;
extern u32 lbl_8047D7C8;
#if 1
asm void fn_80187A60(void) {
#include "src/game/people/people_fn_80187A60.inc"
}
#else
void fn_80187A60(void) { /* TODO: match -- 744 bytes at 0x80187A60 */ }
#endif

/* 0x80188984 | 0x170 */
#if 1
asm void fn_80188984(void) {
#include "src/game/people/people_fn_80188984.inc"
}
#else
void fn_80188984(void) { /* TODO: match -- 368 bytes at 0x80188984 */ }
#endif

/* SDA data labels referenced by asm incs (symbolmap port), typed by load width */
extern f32 lbl_8047D8A0;
extern u32 lbl_8047B1F0[2];
extern f32 lbl_8047D8B0;

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 1
asm void fn_8018F30C(void) {
#include "src/game/people/people_fn_8018F30C.inc"
}
#else
void fn_8018F30C(void) { /* TODO */ }
#endif
#pragma pop
u32 fn_8018F470(u32 r3) {
    if (r3 >= 2) {
        return 0;
    }
    return lbl_8047B1F0[r3];
}
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8018F490(void) {
#include "src/game/people/people_fn_8018F490.inc"
}
#else
#pragma optimization_level 4
u32 fn_8018F490(void* p) {
    if (p != NULL) {
        return (*(u8*)p >> 4) & 1;
    }
    return 0;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8018F4AC(void) {
#include "src/game/people/people_fn_8018F4AC.inc"
}
#else
#pragma optimization_level 4
u32 fn_8018F4AC(void* p) {
    if (p != NULL) {
        return (*(u8*)p >> 5) & 7;
    }
    return 0;
}
#endif
#pragma pop
extern f32 lbl_8047D8A8;
f32 fn_8018F5B4(u8* ptr) {
    if (ptr != NULL) {
        return *(f32*)(ptr + 0x14);
    }
    return lbl_8047D8A8;
}
f32 fn_8018F5CC(u8* ptr) {
    if (ptr != NULL) {
        return *(f32*)(ptr + 0x10);
    }
    return lbl_8047D8A8;
}
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8018F5FC(void) {
#include "src/game/people/people_fn_8018F5FC.inc"
}
#else
#pragma optimization_level 4
u32 fn_8018F5FC(void* p) {
    if (p != NULL) {
        return (*(u8*)p >> 2) & 3;
    }
    return 0;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8018F6B4(void) {
#include "src/game/people/people_fn_8018F6B4.inc"
}
#else
#pragma optimization_level 4
void* fn_8018F6B4(void* p) {
    if (p != NULL) {
        return *(void**)((u8*)p + 0x0C);
    }
    return NULL;
}
#endif
#pragma pop
extern u32 lbl_80478E78;
extern u32 lbl_80478E7C;
extern s32 lbl_8047B1F8;
extern PeopleEntry* lbl_8047B200;
u8* fn_8018F6CC(u32 index) {
    if (index >= *(u32*)lbl_80478E78) {
        return (u8*)0;
    }
    return (u8*)lbl_80478E7C + index * 0x2C;
}
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8018F730(void) {
#include "src/game/people/people_fn_8018F730.inc"
}
#else
#pragma optimization_level 4
u32 fn_8018F730(void) {
    PeopleEntry* current;
    PeopleEntry* entry = lbl_8047B200;
    s32 count = lbl_8047B1F8;
    u32 total;
    s32 i;

    total = 0;
    for (i = 0; i < count; i++) {
        if (i < 0 || count <= i) {
            current = NULL;
        } else {
            current = entry;
        }
        if (current->active != 0) {
            total += PEOPLE_SPAWN_DATA_SIZE;
        }
        entry = (PeopleEntry*)((u8*)entry + PEOPLE_ENTRY_SIZE);
    }

    return total;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 1
asm void fn_8018F788(void) {
#include "src/game/people/people_fn_8018F788.inc"
}
#else
void fn_8018F788(void) { /* TODO */ }
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 1
asm void fn_8018F87C(void) {
#include "src/game/people/people_fn_8018F87C.inc"
}
#else
void fn_8018F87C(void) { /* TODO */ }
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_8018FC50(void) {
#include "src/game/people/people_fn_8018FC50.inc"
}
#else
void fn_8018FC50(PeopleEntry* entry) {
    fn_800E3CF8(entry->modelHandle);
}
#endif
#pragma pop
