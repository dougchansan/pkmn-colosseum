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
#include "game/people/people_data.h"

/* Forward declarations for functions defined later in this TU but used
 * earlier (order kept close to the archive's original topical grouping). */
struct PeopleEntry;
PeopleEntry* peopleInit(u32 maxPeople);
s32 peopleGetMaxCount(void);
PeopleEntry* peopleGetEntry(s32 index);
PeopleEntry* fn_8018FCE0(void);
s32 peopleFree(PeopleEntry* entry);
void* peopleGetModel(PeopleEntry* entry);
BOOL peopleTestFlags(PeopleEntry* entry, u32 mask);
void peopleSetFlags(PeopleEntry* entry, u32 mask);
void peopleClearFlags(PeopleEntry* entry, u32 mask);
void peopleWriteFlags(PeopleEntry* entry, u32 flags);
void peopleSetTransform(PeopleEntry* entry, void* mtx);
void* peopleGetTransform(PeopleEntry* entry);
void fn_80181224(void);
void fn_80181850(void);
s32 fn_80180C78(PeopleOpenWork* work);
void fn_8018114C(void);
void fn_801812C4(PeopleEntry* entry);
s32 fn_80183688(void* self);
s32 fn_80183730(void* self);
void fn_80188FA0(u32 groupId, u32 index, u32 pathId, u32 pathParam);
void fn_80188CA0(u32 groupId, u32 index, u32 targetX, u32 targetY, u32 targetZ);
void fn_80188AF4(u32 groupId, u32 index);
void fn_8018F4C8(void* entry, u8 param, s32* outNode, u8* outResult);
void* fn_8018D998(u32 groupId, u32 index);
s32 fn_801812E8(u32 groupId, u32 index, u8 doInteract);
s32 fn_80181478(u32 groupId, u32 index, u8 doSetup);
void fn_8018E1C4(PeopleEntry* entry, void* spawnData, u32 motionId, u32 param);
void fn_8018FB60(PeopleEntry* entry, u8 animId);
void fn_8018FB2C(PeopleEntry* entry, u8 animId);
void fn_8018FC2C(PeopleEntry* entry);
void fn_8018FC74(PeopleEntry* entry, void* vec);
void fn_8018FC98(PeopleEntry* entry, void* pos);
void* fn_8018FCBC(PeopleEntry* entry);
void fn_8018FC08(PeopleEntry* entry, void* vec);
void* peopleInfoBiosGetPtr(void* scriptObj);
u8 fn_80188214();
void fn_8018E9B4(PeopleEntry* entry, void* position, void* transform);

/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const char* fmt, ...);     /* OSReport / debug printf */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* External functions referenced from asm wrappers */
extern void GSlightSetTarget(void);
extern void GSlightSetPosition(void);
extern void set__5GSvecFfff();
extern void* GSmodelGetRotationPtr(void*);
extern void* GSmodelGetPositionPtr(void*);
extern void GSmodelGetVisibility(void*);
extern void GSmodelGetRotation(void*);
extern void GSmodelGetPosition(void*, void*);
extern void GSmodelSetVisibility(void*, u8);
extern void GSmodelSetRotation(void*);
extern void GSmodelSetPosition(void*);
extern void fn_800E9B2C(void);
extern void GSmodelPushState(void* model, void* state);
extern void GSmodelSetTexAnimFrame(void);
extern void GSmodelSetTexAnimRate(void);
extern void GSmodelSetTexAnimIndex(void);
extern void GSmodelGetAnimIndex(void);
extern void GSmodelHasAnimationEnded(void);
extern void GSmodelIsAnimating(void);
extern void GSmodelSetAnimRate(void);
extern void GSmodelSetAnimFrame(void);
extern void GSmodelSetAnimType(void);
extern void GSmodelSetAnimIndex(void);
extern void GScolsys2HumanEnable(void);

/* GSmem allocator */
extern u16   _toolentryAlloc__FUl(u32 size);                 /* GSmemAllocRaw */
extern void* fn_800E27B0(u32 handle);               /* GSmemGetPtr */

/* Floor/field system */
extern void* fn_80167F28(const char* name);         /* field lookup by name */
extern void* fn_80167E5C(void);                     /* get current field */
extern void  fn_80167ED0(void* field, void* obj, void* data, u32 param);
extern void  fn_80167E64(void* field);              /* field finalize */

/* Thread/task system */
extern void* GSgappCreate(u32 pri, u32 type, void* taskBuf, void* callback);
extern void  GSgappTerminate(void* task);               /* task cleanup */
extern void  GSthreadUnblock(void* thread);
extern void  GSthreadBlock(void* thread);

/* Model system */
extern s32   fn_8017BB80(void* floorObj, void* modelData); /* model open */
extern void* fn_8017BC90(void* floorObj, u32 modelId, u32 param, void* extraData);

/* Flag system */
extern BOOL  fn_800F7108(u16 flagId);               /* GSflagGet (bit check) */
extern void* GSlightCreate(void);
extern void  GSlightSetType(void* light, s32 type);
extern void  GSlightSetActive(void* light, u8 active);
extern void  fn_800FF4D4(void* data, u8 type);

/* Collision/model helpers */
extern void  GSvecCopy(void* dst, void* src);     /* matrix/vector copy */
extern void  fn_800E0168(void* dst, void* srcA, void* srcB);  /* cross/setup */

/* ===== Rodata string references ===== */
extern const char lbl_80273F80[];  /* floor name for blank-frame init */
extern const char lbl_80273FD8[];  /* "Warining: people[%d,%d] group is different!!\n" */
extern f32 lbl_8047D8B0;           /* default moveSpeed constant */
extern f32 lbl_8047D798;
extern const void* lbl_80273F90[];

/* ===== Global state (sbss, owned by the data split — extern here) ===== */

extern void* lbl_8047B1E0;           /* current floor object link */
extern PeopleOpenWork* lbl_8047B1E4; /* active PeopleOpenWork pointer */
extern s32 lbl_8047B1E8;             /* people open count/max for linked list */
extern u32 lbl_8047B1F0[2];          /* ambient light handles (2) */
extern s32 lbl_8047B1F8;             /* maximum people count */
extern u16 lbl_8047B1FC;             /* GSmem handle for the people array */
extern PeopleEntry* lbl_8047B200;    /* pointer to people array (heap-allocated) */

/* Readable aliases */
#define gPeopleFloorObj  lbl_8047B1E0
#define gPeopleOpenWork  lbl_8047B1E4
#define gPeopleOpenCount lbl_8047B1E8
#define gPeopleLights    lbl_8047B1F0
#define gPeopleMaxCount  lbl_8047B1F8
#define gPeopleMemHandle lbl_8047B1FC
#define gPeopleArray     lbl_8047B200

/* ===== Sdata2 float constants ===== */
/* lbl_8047D798 @sda21 : constant used in fn_801812C4 */
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
#pragma dont_inline on
PeopleEntry* peopleInit(u32 maxPeople)
{
    u32 totalSize;

    totalSize = maxPeople * PEOPLE_ENTRY_SIZE;

    /* Allocate from GSmem */
    gPeopleArray = (PeopleEntry*)fn_800E27B0(gPeopleMemHandle = _toolentryAlloc__FUl(totalSize));

    /* Zero-fill entire array */
    memset(gPeopleArray, 0, totalSize);

    /* Store max count */
    gPeopleMaxCount = (s32)maxPeople;

    return gPeopleArray;
}
#pragma dont_inline reset

/* =======================================================================
 * fn_8018FDB4 -- peopleGetMaxCount
 *
 * Return the maximum number of people slots.
 * Frequently called in loops as the upper bound.
 *
 * Called via `bl` at every call site in the target binary (never inlined,
 * despite being trivially small) -- pragma'd not-inline to match. */
#pragma dont_inline on
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
 *
 * Called via `bl` at every call site in the target binary (never inlined) --
 * pragma'd not-inline to match. */
PeopleEntry* peopleGetEntry(s32 index)
{
    if (index < 0 || gPeopleMaxCount <= index) {
        return NULL;
    }
    return (PeopleEntry*)((u8*)gPeopleArray + index * PEOPLE_ENTRY_SIZE);
}
#pragma dont_inline reset

/* =======================================================================
 * fn_8018FCE0 -- peopleAlloc
 *
 * Find the first free (inactive) slot in the people array, zero it,
 * and mark it as active. Sets up self-pointer, shadow ID, and move speed.
 *
 * Returns: PeopleEntry* to the newly allocated slot, or NULL if full.
 * ======================================================================= */
PeopleEntry* fn_8018FCE0(void)
{
    PeopleEntry* entry;
    s32 maxCount;
    int i;
    PeopleEntry* found;
    f32 moveSpeed;

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
            moveSpeed = lbl_8047D8B0;
            found->selfPtr = found;          /* self-pointer for script lookup */
            found->shadowId = -1;            /* no shadow by default */
            found->moveSpeed = moveSpeed; /* default moveSpeed from sdata2 */

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
/* These small accessors are called via `bl` at every observed call site in
 * the target binary (loop bodies throughout the people/*.c family never get
 * them inlined) -- pragma'd not-inline site-wide to match. */
#pragma dont_inline on
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
    return (((s32)(-result) | (s32)result) >> 31) & 1;
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
 * Delegates to GSvecCopy (matrix/vector copy).
 *
 * r3 = PeopleEntry*
 * r4 = source matrix pointer
 * ======================================================================= */
void peopleSetTransform(PeopleEntry* entry, void* mtx)
{
    GSvecCopy((u8*)entry + 0x9C, mtx);
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
#pragma dont_inline reset

/* =======================================================================
 * fn_80181224 -- peopleFloorInit
 *
 * Called when a floor is being set up. Draws 30 blank frames via the
 * field system, then resets the open work state.
 *
 * This ensures all pending model loads and animation setups complete
 * before the floor becomes visible.
 * ======================================================================= */
#pragma optimization_level 2
void fn_80181224(void)
{
    void* field;
    PeopleOpenWork** openWorkSlot;
    s32 i;
    void* fieldData;
    void* floorObj;
    PeopleOpenWork* work;

    floorObj = gPeopleFloorObj;
    field = fn_80167F28(lbl_80273F80);
    fieldData = fn_80167E5C();

    for (i = 0; i < 30; i++) {
        fn_80167ED0(field, floorObj, fieldData, 0);
    }

    fn_80167E64(field);

    /* Reset open work state */
    work = *(openWorkSlot = &gPeopleOpenWork);
    work->subState = 0;

    /* Clean up the thread/task */
    GSgappTerminate((*openWorkSlot)->threadObj);
}
#pragma optimization_level reset

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
void fn_80181850(void)
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
            modelPos = fn_8018FCBC(entry);
            GSvecCopy((u8*)entry + 0x2C, modelPos);  /* simplified */

            modelTrans = peopleGetTransform(entry);
            GSvecCopy((u8*)entry + 0x38, modelTrans); /* simplified */
        }

        /* Update entry position tracking */
        fn_8018FC98(entry, (void*)((u8*)entry + 0x44));
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
                fn_801812C4(entry);
            }
        }

    next_entry:
        i--;
    }
}

/* 0x8018A44C | 0x2B4 */
extern u32 lbl_8047D814;
extern u32 lbl_8047D7C0;
extern u32 lbl_8047D7C8;
#if 0
asm void fn_8018A44C(void) {
#include "src/game/people/people_fn_8018A44C.inc"
}
#else
void fn_8018A44C(void) {
    /* TODO: match -- 692 bytes at 0x8018A44C */
}
#endif

/* 0x8018A700 | 0x3CC */
extern f32 fn_800E008C(void* param);
extern void fn_800E013C(void);
extern void GSvecAdd(void);
extern u32 lbl_8047D800;
extern f32 lbl_8047D7A0;
extern f32 lbl_8047D79C;
#if 0
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
extern f32 lbl_8047D79C;
extern u8 lbl_80274008[];
extern u8 lbl_8036C510[];
#if 0
asm void peopleWaitSyncMotionBlend(void) {
#include "src/game/people/people_fn_8018AEC0.inc"
}
#else
void peopleWaitSyncMotionBlend(void) {
    /* TODO: match -- 444 bytes at 0x8018AEC0 */
}
#endif

/* 0x8018B220 | 0x148 */
extern void GSmodelStopAnimation(void*);
#if 0
asm void fn_8018B220(void) {
#include "src/game/people/people_fn_8018B220.inc"
}
#else
void fn_8018B220(u32 groupId, u32 index) {
    PeopleEntry* found;
    PeopleEntry* entry;
    s32 i;
    s32 j;

    /* Loop 1: search by groupId + index */
    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    /* Loop 2: fallback search by index only */
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    /* Loop 3: search by selfPtr */
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry != NULL) {
        void* model = peopleGetModel(entry);
        if (model != NULL) {
            GSmodelStopAnimation(model);
        }
    }
}
#endif

/* 0x8018B368 | 0x1F0 */
extern void GSmodelStartAnimation(void);
extern f32 lbl_8047D7A0;
extern u32 lbl_8047D7C8;
extern u32 lbl_8047D7A4;
#if 0
asm void fn_8018B368(void) {
#include "src/game/people/people_fn_8018B368.inc"
}
#else
void fn_8018B368(void) {
    /* TODO: match -- 496 bytes at 0x8018B368 */
}
#endif

/* 0x8018B558 | 0x214 */
extern void GSmodelSetAnimBlend(void);
extern void GSmodelSetBlendFactor(void);
extern void GSmodelGetFrameCount(void);
extern void GSmodelSetBlendAnimFrameForce(void);
extern u32 lbl_8047D7D0;
extern f32 lbl_8047D79C;
extern f32 lbl_8047D7A0;
extern u32 lbl_8047D834;
#if 0
asm void fn_8018B558(void) {
#include "src/game/people/people_fn_8018B558.inc"
}
#else
void fn_8018B558(void) {
    /* TODO: match -- 532 bytes at 0x8018B558 */
}
#endif

/* 0x8018BC88 | 0x16C */
extern void* GSmodelGetPart(void* modelHandle, s32 motionId);
extern void GSpartGetTransform(void* part, void* mtxOut, u32 param3, u32 param4);
extern void GSpartFree(void* part);

/* Find a people entry by (groupId, index) and either copy its world
 * transform into *target (motionId < 0), or fetch the transform of a
 * specific model part (motionId >= 0) via the GS "part" API. */
void fn_8018BC88(u32 groupId, u32 index, s32 motionId, void* target) {
    s32 i;
    s32 j;
    PeopleEntry* found;
    PeopleEntry* entry;
    void* part;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry == NULL) {
        return;
    }
    if (motionId >= 0) {
        part = GSmodelGetPart(entry->modelHandle, motionId);
        GSpartGetTransform(part, target, 0, 0);
        GSpartFree(part);
    } else {
        GSvecCopy(target, fn_8018FCBC(entry));
    }
}

/* 0x8018D680 | 0x150 */
extern void atan2(void);
extern f64 fmod(f64 angle);
extern void fn_800E0718(void);
extern void GSvecTransformQuat(void);
extern void GSvecDistance(void);
extern u32 lbl_8047D7A4;
extern f32 lbl_8047D7A0;
extern f64 lbl_8047D7F0;
extern u32 lbl_8047D7A8;
extern u32 lbl_8047D820;
extern u8 lbl_8031554C[];
#if 0
asm void fn_8018D680(void) {
#include "src/game/people/people_fn_8018D680.inc"
}
#else
void fn_8018D680(void) {
    /* TODO: match -- 336 bytes at 0x8018D680 */
}
#endif

/* 0x8018DB68 | 0x140 */
#if 0
asm void fn_8018DB68(void) {
#include "src/game/people/people_fn_8018DB68.inc"
}
#else
void fn_8018DB68(void) {
    /* TODO: match -- 320 bytes at 0x8018DB68 */
}
#endif

/* 0x8018DCA8 | 0x3A8 */
extern void fn_800E24B0(void);
extern void fn_800E209C(void);
extern void fn_800F9210(void);
extern void GSmodelFree(void);
#if 0
asm void fn_8018DCA8(void) {
#include "src/game/people/people_fn_8018DCA8.inc"
}
#else
void fn_8018DCA8(void) {
    /* TODO: match -- 936 bytes at 0x8018DCA8 */
}
#endif

/* 0x8018E9B4 | 0x338 */
extern void* GSresGetResource();
extern void fn_800F7BC4(void);
extern void fn_80101B90(void);
extern void GScolsys2ThruGetEventID(void);
extern void fn_8012BAF0(void);
extern void GScolsys2HumanCollision(void);
extern void fn_8010F320(void);
extern void PSVECSubtract(void);
extern void PSVECAdd(void);
extern void fn_801101B4(void);
extern void fn_8010E138(void);
extern u32 lbl_8047D890;
extern const f32 lbl_8047D7EC;
extern u32 lbl_8047D894;
extern u32 lbl_8047D800;
extern f32 lbl_8047D7A0;
/* renamed symbols referenced by asm incs (symbolmap port) */
extern void heroMoveSetEventList();
extern void sin();   /* MSL trig (renamed) — referenced by asm incs */
extern void cos();   /* MSL trig (renamed) — referenced by asm incs */
extern void GScolsy2UtilGetCpPlanePoint();
extern void GScolsy2UtilChkInTri();
extern void GSmodelPopState();
#if 0
asm void fn_8018E9B4(void) {
#include "src/game/people/people_fn_8018E9B4.inc"
}
#endif

/* 0x8018ECEC | 0x3A0 */
extern void fn_800D3088(void);
extern f32 lbl_8047D7A0;
extern f64 lbl_8047D7F0;
extern u32 lbl_8047D7A8;
extern u32 lbl_8047D820;
extern u32 lbl_8047D898;
extern u32 lbl_8047D7B0;
extern u32 lbl_8047D7D0;
extern u32 lbl_8047D89C;
#if 0
asm void fn_8018ECEC(void) {
#include "src/game/people/people_fn_8018ECEC.inc"
}
#else
#pragma dont_inline on
void fn_8018ECEC(PeopleEntry* entry, f32 step) {
    /* TODO: match -- 928 bytes at 0x8018ECEC */
}
#pragma dont_inline reset
#endif

/* WP-0010 stubs */

/* 0x80181478 | 0x3D8 */
extern f32 lbl_8047D7A0;
extern u32 lbl_8047D7A4;
#if 0
asm void fn_80181478(void) {
#include "src/game/people/people_fn_80181478.inc"
}
#else
s32 fn_80181478(u32 groupId, u32 index, u8 doSetup) { /* TODO: match -- 984 bytes at 0x80181478 */ return 0; }
#endif

/* 0x80181EB0 | 0x308 */
extern void fn_80113F48(void);
extern void fn_801CBA0C(void);
extern void fn_800E3CC8(void);
extern void GSmodelClearShadowFlags(void);
extern void fn_801CB834(void);
extern void fn_80166A28(void);
extern void fn_800F7318(void);
#if 0
asm void fn_80181EB0(void) {
#include "src/game/people/people_fn_80181EB0.inc"
}
#else
void fn_80181EB0(void) { /* TODO: match -- 776 bytes at 0x80181EB0 */ }
#endif

/* 0x801821B8 | 0xE60 */
extern u32 lbl_8047D7D8;
extern f32 lbl_8047D7A0;
extern u32 lbl_8047D7A4;
extern u32 lbl_8047D7D0;
extern u32 lbl_8047D7E0;
extern u8 lbl_8036C4F8[];
#if 0
asm void fn_801821B8(void) {
#include "src/game/people/people_fn_801821B8.inc"
}
#else
void fn_801821B8(void) { /* TODO: match -- 3680 bytes at 0x801821B8 */ }
#endif

/* 0x801837D8 | 0x180 */
#if 0
asm void fn_801837D8(void) {
#include "src/game/people/people_fn_801837D8.inc"
}
#else
void fn_801837D8(void) { /* TODO: match -- 384 bytes at 0x801837D8 */ }
#endif

/* 0x80183958 | 0x24 */
extern void fn_801170A4(void);
extern void floorCharacterBiosGetMoveSctID(void);
#if 0
asm void fn_80183958(void) {
#include "src/game/people/people_fn_80183958.inc"
}
#else
#pragma optimization_level 0
void fn_80183958(void) {
    fn_801170A4();
    floorCharacterBiosGetMoveSctID();
}
#endif

/* 0x801839A0 | 0x1A4 */
extern f32 lbl_8047D7A0;
extern f32 lbl_8047D79C;
#if 0
asm void fn_801839A0(void) {
#include "src/game/people/people_fn_801839A0.inc"
}
#else
void fn_801839A0(void) { /* TODO: match -- 420 bytes at 0x801839A0 */ }
#endif

/* 0x80183B44 | 0x19C */
extern f32 lbl_8047D7A0;
extern u32 lbl_8047D7E8;
extern const f32 lbl_8047D7EC;
extern f32 lbl_8047D79C;
#if 0
asm void fn_80183B44(void) {
#include "src/game/people/people_fn_80183B44.inc"
}
#else
void fn_80183B44(void) { /* TODO: match -- 412 bytes at 0x80183B44 */ }
#endif

/* 0x80183CE0 | 0x17C */
#if 0
asm void fn_80183CE0(void) {
#include "src/game/people/people_fn_80183CE0.inc"
}
#else
void fn_80183CE0(void) { /* TODO: match -- 380 bytes at 0x80183CE0 */ }
#endif

/* 0x80183E5C | 0x168 -- find a people entry by (groupId, index) and kick off
 * a special motion/interact state: resets a u16 field at +0x6A, sets
 * moveSpeed to 1.0, and enters state 3 (flag set) or state 2 (flag clear). */
#pragma optimization_level 4
BOOL fn_80183E5C(u32 groupId, u32 index, u8 flag) {
    s32 i;
    s32 j;
    void* found;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry == NULL) {
        return 0;
    }
    entry->subState = 0;
    *(u16*)((u8*)entry + 0x6A) = 0;
    entry->moveSpeed = lbl_8047D79C;
    entry->subState = 1;
    if (flag) {
        entry->state = 3;
    } else {
        entry->state = 2;
    }
    return 1;
}
#pragma optimization_level reset

/* 0x80183FC4 | 0x1CC */
extern u8 lbl_8027404C[];
#if 0
asm void peopleAddWalkList(void) {
#include "src/game/people/people_fn_80183FC4.inc"
}
#else
void peopleAddWalkList(void) { /* TODO: match -- 460 bytes at 0x80183FC4 */ }
#endif

/* 0x80184190 | 0x2C0 */
#if 0
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
#if 0
asm void fn_80184948(void) {
#include "src/game/people/people_fn_80184948.inc"
}
#else
void fn_80184948(void) { /* TODO: match -- 328 bytes at 0x80184948 */ }
#endif

/* 0x80184A90 | 0x2F0 */
extern f32 lbl_8047D7A0;
extern f64 lbl_8047D7F0;
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
#if 0
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
extern f32 lbl_8047D7A0;
extern u32 lbl_8047D7C8;
extern u32 lbl_8047D7D0;
extern f32 lbl_8047D7C4;
extern u32 lbl_8047D818;
extern f64 lbl_8047D7F0;
extern u32 lbl_8047D7A8;
extern u32 lbl_8047D820;
extern u32 lbl_8047D7C0;
extern f32 lbl_8047D79C;
#if 0
asm void fn_80184D80(void) {
#include "src/game/people/people_fn_80184D80.inc"
}
#else
void fn_80184D80(void) { /* TODO: match -- 1228 bytes at 0x80184D80 */ }
#endif

/* 0x8018524C | 0x678 */
extern f32 lbl_8047D7A0;
extern f64 lbl_8047D7F0;
extern u32 lbl_8047D7A8;
extern u32 lbl_8047D820;
extern u32 lbl_8047D7C0;
extern u32 lbl_8047D7C8;
extern f32 lbl_8047D79C;
extern f32 lbl_8047D7C4;
#if 0
asm void fn_8018524C(void) {
#include "src/game/people/people_fn_8018524C.inc"
}
#else
void fn_8018524C(void) { /* TODO: match -- 1656 bytes at 0x8018524C */ }
#endif

/* 0x801858C4 | 0x1E8 */
extern u32 lbl_8047D828;
#if 0
asm void fn_801858C4(void) {
#include "src/game/people/people_fn_801858C4.inc"
}
#else
void fn_801858C4(void) { /* TODO: match -- 488 bytes at 0x801858C4 */ }
#endif

/* 0x80185AAC | 0xE4 */
#if 0
asm void fn_80185AAC(void) {
#include "src/game/people/people_fn_80185AAC.inc"
}
#else
s32 fn_80185AAC(PeopleEntry* entry) {
    u8 vec[12];
    f32 oldLength;

    fn_800E0168(vec, entry->field_5C, fn_8018FCBC(entry));
    oldLength = fn_800E008C(vec);
    if (!fn_80188214(entry->groupId, entry->index, entry->moveSpeed)) {
        return 2;
    }
    fn_800E0168(vec, entry->field_5C, fn_8018FCBC(entry));
    if (fn_800E008C(vec) > oldLength) {
        fn_8018FC74(entry, entry->field_5C);
        fn_8018E9B4(entry, fn_8018FCBC(entry), peopleGetTransform(entry));
        return 1;
    }
    return 0;
}

void fn_8018E9B4(PeopleEntry* entry, void* position, void* transform) {
    /* TODO: match -- 824 bytes at 0x8018E9B4 */
}
#endif

/* 0x80185B90 | 0x358 */
extern f32 lbl_8047D7A0;
extern u32 lbl_8047D7D0;
extern f32 lbl_8047D79C;
extern u32 lbl_8047D830;
extern u32 lbl_8047D834;
extern u32 lbl_8047D7A4;
#if 0
asm void fn_80185B90(void) {
#include "src/game/people/people_fn_80185B90.inc"
}
#else
void fn_80185B90(void) { /* TODO: match -- 856 bytes at 0x80185B90 */ }
#endif

/* 0x80185F44 | 0x1B4 */
extern u32 lbl_8047D814;
extern f64 lbl_8047D7F0;
#if 0
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
asm void peopleGazeHeroCheck(void) {
#include "src/game/people/people_fn_80186254.inc"
}
#else
#pragma push
#pragma scheduling on
void peopleGazeHeroCheck(u32 a, u32 b) {
    fn_80186284(a, b, lbl_8047D7EC, 0, 100, lbl_8047D838);
}
#pragma pop
#endif

/* 0x80186284 | 0x39C */
extern void fn_8010F188(void);
extern u32 lbl_8047D83C;
extern u32 lbl_8047D800;
extern f64 lbl_8047D7F0;
extern u32 lbl_8047D7A8;
extern u32 lbl_8047D820;
extern u32 lbl_8047D814;
extern u32 lbl_8047D7A4;
#if 0
asm void fn_80186284(u32 a, u32 b, f32 x, s32 c, s32 d, f32 y) {
#include "src/game/people/people_fn_80186284.inc"
}
#else
void fn_80186284(u32 a, u32 b, f32 x, s32 c, s32 d, f32 y) { /* TODO: match -- 924 bytes at 0x80186284 */ }
#endif

/* 0x80186620 | 0x53C */
extern void cameraGetActive(void);
extern void fn_800D258C(void);
extern void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void);
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
extern void GScolsy2UtilGetPointExtentionLine(void);
extern void heroMoveSetLockFrame(void);
extern u32 lbl_8047D840;
extern u8 lbl_80314638[];
extern u32 lbl_8047D844;
extern u32 lbl_8047D83C;
extern f32 lbl_8047D7A0;
extern f32 lbl_8047D79C;
extern u32 lbl_8047D848;
#if 0
asm void fn_80186620(void) {
#include "src/game/people/people_fn_80186620.inc"
}
#else
void fn_80186620(void) { /* TODO: match -- 1340 bytes at 0x80186620 */ }
#endif

/* 0x80186B5C | 0x58C */
extern void fn_800F7A7C(void);
extern void fn_800F7A08(void);
extern void cameraGetRotY(void);
extern u8 lbl_80273FB4[];
extern u32 lbl_8047D7C8;
extern u32 lbl_8047D84C;
extern f32 lbl_8047D7A0;
extern u32 lbl_8047D850;
extern u32 lbl_8047D858;
extern u32 lbl_8047D860;
extern u8 lbl_80478AC0[];
extern f32 lbl_8047D7C4;
extern u32 lbl_8047D868;
extern u32 lbl_8047D86C;
extern u32 lbl_8047D7E8;
extern u32 lbl_8047D870;
extern u32 lbl_8047D818;
extern u32 lbl_8047D830;
extern u32 lbl_8047D7A4;
extern f32 lbl_8047D79C;
extern u32 lbl_8047D874;
extern u32 lbl_8047D7D0;
#if 0
asm void fn_80186B5C(void) {
#include "src/game/people/people_fn_80186B5C.inc"
}
#else
void fn_80186B5C(void) { /* TODO: match -- 1420 bytes at 0x80186B5C */ }
#endif

/* 0x801870E8 | 0x3D4 */
extern void GScolsy2UtilGetSidePlanePoint(void);
extern void fn_8010FA54(void);
extern void PSVECSquareDistance(void);
extern void fn_8010F71C(void);
extern u32 lbl_8047D844;
extern f32 lbl_8047D7A0;
#if 0
asm void fn_801870E8(void) {
#include "src/game/people/people_fn_801870E8.inc"
}
#else
void fn_801870E8(void) { /* TODO: match -- 980 bytes at 0x801870E8 */ }
#endif

/* 0x801874BC | 0x450 */
extern u32 lbl_8047D844;
#if 0
asm void fn_801874BC(void) {
#include "src/game/people/people_fn_801874BC.inc"
}
#else
void fn_801874BC(void) { /* TODO: match -- 1104 bytes at 0x801874BC */ }
#endif

/* 0x80187A60 | 0x2E8 */
extern u32 lbl_8047D7C0;
extern u32 lbl_8047D7C8;
#if 0
asm void fn_80187A60(void) {
#include "src/game/people/people_fn_80187A60.inc"
}
#else
void fn_80187A60(void) { /* TODO: match -- 744 bytes at 0x80187A60 */ }
#endif

/* 0x80188984 | 0x170 -- find a people entry by (groupId, index) and busy-wait
 * (optionally, if flag != 0) until its collision position converges onto its
 * target position (field_0C/field_0x10 vs field_B4/field_B8, all really f32
 * despite the placeholder u32 typing in the header). Returns 0 once
 * converged or if the entry isn't found; returns 1 immediately if not
 * converged and flag == 0 (caller opted out of waiting). */
BOOL fn_80188984(u32 groupId, u32 index, u8 flag) {
    s32 i;
    s32 j;
    void* found;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry == NULL) {
        return 0;
    }
    for (;;) {
        if (*(f32*)((u8*)entry + 0xC) == *(f32*)((u8*)entry + 0xB4) &&
            *(f32*)((u8*)entry + 0x10) == *(f32*)((u8*)entry + 0xB8)) {
            return 0;
        }
        if (flag) {
            _threadSwitch();
            continue;
        }
        return 1;
    }
}

/* SDA data labels referenced by asm incs (symbolmap port), typed by load width */
extern f32 lbl_8047D8A0;
extern f32 lbl_8047D8B0;

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
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
    return gPeopleLights[r3];
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
u8* peopleInfoBiosGetPtrFromIndex(u32 index) {
    if (index >= *(u32*)lbl_80478E78) {
        return (u8*)0;
    }
    return (u8*)lbl_80478E7C + index * 0x2C;
}
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void peopleBiosGetPushDataSize(void) {
#include "src/game/people/people_fn_8018F730.inc"
}
#else
#pragma optimization_level 4
u32 peopleBiosGetPushDataSize(void) {
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
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void peopleBiosPushData(void) {
#include "src/game/people/people_fn_8018F788.inc"
}
#else
void peopleBiosPushData(u8* dst, u32 size) {
    u32 offset;
    u8* end;
    u8* current;
    void* model;
    s32 i;

    i = 0;
    current = dst;
    offset = 0;
    end = dst + size;
    while (i < gPeopleMaxCount) {
        PeopleEntry* entry;

        if (i < 0 || gPeopleMaxCount <= i) {
            entry = NULL;
        } else {
            entry = (PeopleEntry*)((u8*)gPeopleArray + offset);
        }
        if (entry->active) {
            model = entry->modelHandle;
            memcpy(current, (u8*)entry + 0x20, 0xBC);
            memcpy(current + 0xBC, GSmodelGetPositionPtr(model), 0xC);
            memcpy(current + 0xC8, GSmodelGetRotationPtr(model), 0xC);
            memcpy(current + 0xD4, (u8*)model + 0x120, 0xC);
            memcpy(current + 0xE0, (u8*)model + 0x12C, 0xC);
            GSmodelPushState(model, current + 0xEC);
            current += PEOPLE_SPAWN_DATA_SIZE;
            if (current > end) {
                break;
            }
        }
        offset += PEOPLE_ENTRY_SIZE;
        i++;
    }
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void peopleBiosPopData(void) {
#include "src/game/people/people_fn_8018F87C.inc"
}
#else
void peopleBiosPopData(void) { /* TODO */ }
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void peopleGetPosition(void) {
#include "src/game/people/people_fn_8018FC50.inc"
}
#else
void peopleGetPosition(PeopleEntry* entry) {
    GSmodelGetRotationPtr(entry->modelHandle);
}
#endif
#pragma pop

/* =========================================================================
 * The following addresses fall inside the verified people.c range
 * (0x80180C78 - 0x8018FE30) but were never recovered by the archived
 * campaign (no .inc dump, no C attempt). They are parked here as honest
 * unmatched stubs -- real C (not asm), so they compile, but they do not
 * byte-match. Signatures are taken from people.h where the function was
 * already named/documented there; otherwise a safe void(void) stub is used.
 * ========================================================================= */
/* fn_80180C78 = peopleOpenCallback (see people.h) -- not recovered, gap in archive campaign */
s32 fn_80180C78(PeopleOpenWork* work) {
    return 0;
}

/* fn_80181094 = peopleOpenThread (size 0xB8) */
#pragma optimization_level 0
void fn_80181094(void) {
    u8* offsets;
    u8* table;
    u8* base;
    u8* modelData;
    base = *(u8**)((u8*)gPeopleOpenWork->parentObj + 0x40);
    table = *(u8**)((u8*)gPeopleOpenWork->parentObj + 0x40) + *(u32*)(base + 0x18);
    offsets = *(u8**)((u8*)gPeopleOpenWork->parentObj + 0x40) + *(u32*)table;
    modelData = *(u8**)((u8*)gPeopleOpenWork->parentObj + 0x40)
              + *(u32*)(offsets + gPeopleOpenWork->floorParam * 4);
    fn_8017BB80(gPeopleOpenWork->parentObj, modelData);
    gPeopleOpenWork->subState = 2;
    GSgappTerminate(gPeopleOpenWork->threadObj);
}
#pragma optimization_level reset

/* fn_8018114C = peopleCloseCallback (see people.h) -- not recovered, gap in archive campaign */
#pragma optimization_level 0
void fn_8018114C(void) {
    u8* modelData;
    u8* offsets;
    u8* table;
    u8* base;
    base = *(u8**)((u8*)gPeopleOpenWork->parentObj + 0x40);
    table = *(u8**)((u8*)gPeopleOpenWork->parentObj + 0x40) + *(u32*)(base + 0x18);
    offsets = *(u8**)((u8*)gPeopleOpenWork->parentObj + 0x40) + *(u32*)table;
    modelData = *(u8**)((u8*)gPeopleOpenWork->parentObj + 0x40)
              + *(u32*)(offsets + gPeopleOpenWork->floorParam * 4);
    fn_8017BC90(gPeopleOpenWork->parentObj, *(u32*)modelData,
                *(u32*)((u8*)gPeopleOpenWork + 0x38), gPeopleOpenWork->extraData);
    *(u32*)gPeopleOpenWork->extraData = 0;
    gPeopleOpenWork->subState = 2;
    GSgappTerminate(gPeopleOpenWork->threadObj);
}
#pragma optimization_level reset

/* fn_801812C4 = peopleMoveUpdate (see people.h) -- not recovered, gap in archive campaign */
#pragma optimization_level 2
void fn_801812C4(PeopleEntry* entry) {
    f32 step;

    step = lbl_8047D798;
    fn_8018ECEC(entry, step);
}
#pragma optimization_level reset

/* fn_801812E8 = peopleFindAndInteract (see people.h) -- find a people entry
 * by (groupId, index) and toggle it in/out of the "interacting" state
 * (saving/restoring the previous state around states 4-5). */
s32 fn_801812E8(u32 groupId, u32 index, u8 doInteract) {
    s32 i;
    s32 j;
    void* found;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry == NULL) {
        return 0;
    }
    if (doInteract) {
        entry->prevState = entry->state;
        if ((s32)entry->state < 6 && (s32)entry->state >= 4) {
            entry->state = 0;
        }
    } else {
        if ((s32)entry->prevState < 6 && (s32)entry->prevState >= 4) {
            entry->state = entry->prevState;
            entry->animBlendFactor = lbl_8047D79C;
            entry->subState = 0;
        }
    }
    return 1;
}

/* fn_80183018 -- not recovered, gap in archive campaign (size 0x338) */
void fn_80183018(void) {
}

/* fn_80183350 -- not recovered, gap in archive campaign (size 0x338) */
void fn_80183350(void) {
}

/* 0x80183688 | size: 0xA8 */
s32 fn_80183688(void* self) {
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (entry->active == 0) continue;
        if (entry->selfPtr != self) continue;
        if ((entry->flagId && entry->flagId) && entry->flagId) {
            /* Preserve MWCC register allocation. */
        }
        goto found;
    }
    entry = NULL;
found:
    if (entry == NULL) {
        return 0;
    }
    entry->visible = 1;
    if ((entry = (PeopleEntry*)fn_800F7108(entry->flagId)) == NULL) {
        return 0;
    }
    GSthreadUnblock(entry);
    return 1;
}

/* 0x80183730 | size: 0xA8 */
s32 fn_80183730(void* self) {
    s32 i;
    int new_var;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        new_var = entry->active == 0;
        if (new_var) continue;
        if (entry->selfPtr != self) continue;
        if ((entry->flagId && entry->flagId) && entry->flagId) {
            /* Preserve MWCC register allocation. */
        }
        goto found;
    }
    entry = NULL;
found:
    if (entry == NULL) {
        return 0;
    }
    entry->visible = 0;
    if ((entry = (PeopleEntry*)fn_800F7108(entry->flagId)) == NULL) {
        return 0;
    }
    GSthreadBlock(entry);
    return 1;
}

/* fn_8018397C -- not recovered, gap in archive campaign (size 0x24) */
u32 fn_8018397C(u8* arg1, u32 arg2) {
    extern u32 fn_801170A4(u8*, u32);
    extern u32 floorCharacterBiosGetTalkSctID(void*);
    return floorCharacterBiosGetTalkSctID((void*)fn_801170A4(arg1, arg2));
}

/* fn_80184470 -- not recovered, gap in archive campaign (size 0x174) */
void fn_80184470(void) {
}

/* fn_801845E4 -- not recovered, gap in archive campaign (size 0x2EC) */
void fn_801845E4(void) {
}

/* fn_801848D0 -- not recovered, gap in archive campaign (size 0x78) */
void fn_801848D0(void* a, s32 b, s32 c, s32 d) {
    void GSmodelAttachToGSpart();
    void* res = GSresGetResource(b, c);
    if (res != 0) {
        void* part = GSmodelGetPart(res, d);
        GSmodelAttachToGSpart(a, part, 7, 0, 1);
        GSpartFree(part);
    }
}

/* fn_80185EE8 -- not recovered, gap in archive campaign (size 0x5C) */
void fn_80185EE8(u32 a, u32 b, u32 c) {
    void fn_8018AACC();
    u8 local[24];
    set__5GSvecFfff(local);
    fn_8018AACC(a, b, c, local);
}

/* fn_801860F8 -- not recovered, gap in archive campaign (size 0x15C) */
void fn_801860F8(void) {
}

/* fn_8018790C -- not recovered, gap in archive campaign (size 0x154) */
void fn_8018790C(void) {
}

/* fn_80187D48 -- not recovered, gap in archive campaign (size 0x314) */
void fn_80187D48(void) {
}

/* fn_8018805C -- not recovered, gap in archive campaign (size 0x1B8) */
void fn_8018805C(void) {
}

/* fn_80188214 -- not recovered, gap in archive campaign (size 0x3B0) */
u8 fn_80188214() {
}

/* fn_801885C4 -- not recovered, gap in archive campaign (size 0x214) */
void fn_801885C4(void) {
}

/* fn_801887D8 -- find a people entry by (groupId, index) and compute an
 * animation blend ratio against entry->field_34/field_38 (float time range)
 * for a caller-supplied time-source object (param3, fed to fn_800E008C). */
f32 fn_801887D8(u32 groupId, u32 index, void* param3) {
    s32 i;
    s32 j;
    void* found;
    PeopleEntry* entry;
    f32 result;
    f32 t;
    f32 startTime;
    f32 endTime;

    result = lbl_8047D7A0;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry == NULL) {
        goto end;
    }
    t = fn_800E008C(param3);
    endTime = *(f32*)&entry->field_38;
    if (endTime <= t) {
        result = lbl_8047D7C4;
    } else {
        startTime = *(f32*)&entry->field_34;
        if (startTime <= t) {
            if (endTime != lbl_8047D7A0) {
                result = lbl_8047D79C + (t - startTime) / (endTime - startTime);
            }
        } else if (startTime != lbl_8047D7A0) {
            result = t / startTime;
        }
    }
end:
    return result;
}

/* fn_80188AF4 = fn_80188AF4 (see people.h) -- not recovered, gap in archive campaign */
void fn_80188AF4(u32 groupId, u32 index) {

}

/* fn_80188CA0 = fn_80188CA0 (see people.h) -- not recovered, gap in archive campaign */
void fn_80188CA0(u32 groupId, u32 index, u32 targetX, u32 targetY, u32 targetZ) {

}

/* fn_80188F78 -- not recovered, gap in archive campaign (size 0x28) */
void fn_80188F78(u32 groupId, u32 index) {
    fn_80188FA0(groupId, index, 0, 100);
}

/* fn_80188FA0 = fn_80188FA0 (see people.h) -- not recovered, gap in archive campaign */
void fn_80188FA0(u32 groupId, u32 index, u32 pathId, u32 pathParam) {

}

/* fn_80189328 -- find a people entry by (groupId, index); read its current
 * PEOPLE_FLAG_TALKABLE state, set or clear that flag per 'enable', and
 * return the *previous* state. Returns 0 if the entry isn't found. */
BOOL fn_80189328(u32 groupId, u32 index, u8 enable) {
    s32 i;
    s32 j;
    void* found;
    PeopleEntry* entry;
    BOOL wasTalkable;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry == NULL) {
        return 0;
    }
    wasTalkable = peopleTestFlags(entry, PEOPLE_FLAG_TALKABLE);
    if (enable) {
        peopleSetFlags(entry, PEOPLE_FLAG_TALKABLE);
    } else {
        peopleClearFlags(entry, PEOPLE_FLAG_TALKABLE);
    }
    return wasTalkable;
}

/* fn_80189490 -- not recovered, gap in archive campaign (size 0x500) */
void fn_80189490(void) {
}

/* fn_80189990 -- not recovered, gap in archive campaign (size 0x8F0) */
void fn_80189990(void) {
}

/* Find an entry by (groupId, index), then report whether its current movement
 * has completed. If waitFlag is set, yield until it reaches a terminal state. */
static inline PeopleEntry* peopleFindSelf(u32 groupId, u32 index)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        return entry->selfPtr;
    }

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite(lbl_80273FD8, groupId, index);
        return entry->selfPtr;
    }
    return NULL;
}

static inline PeopleEntry* peopleFindBySelf(PeopleEntry* found)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        return entry;
    }
    return NULL;
}

BOOL peopleMoveCheck(u32 groupId, u32 index, u8 waitFlag)
{
    PeopleEntry* entry;
    PeopleEntry* found;
    u8 isVisible;

    found = peopleFindSelf(groupId, index);
    entry = peopleFindBySelf(found);

    if (entry == NULL) {
        return FALSE;
    }
    for (;;) {
        if (entry->state == 0 && entry->pad22 == 0) {
            return FALSE;
        }
        if (entry->visible != 0) {
            isVisible = TRUE;
        } else if ((u32)fn_800F7108(entry->flagId) == 0) {
            isVisible = TRUE;
        } else {
            isVisible = FALSE;
        }
        if (!isVisible && entry->pad22 == 0) {
            GSlogWrite(lbl_80274078, lbl_8036C52C);
            return FALSE;
        }
        if (waitFlag) {
            _threadSwitch();
            continue;
        }
        return TRUE;
    }
}

/* fn_8018AACC -- not recovered, gap in archive campaign (size 0x3F4) */
void fn_8018AACC(void) {
}

/* peopleWaitSyncMotion -- not recovered, gap in archive campaign (size 0x1A4) */
void peopleWaitSyncMotion(void) {
}

/* fn_8018B76C -- not recovered, gap in archive campaign (size 0x298) */
void fn_8018B76C(void) {
}

/* fn_8018BA04 -- not recovered, gap in archive campaign (size 0x284) */
void fn_8018BA04(void) {
}

/* fn_8018BDF4 -- find a people entry by (groupId, index) and set its
 * position. No-op if pos is NULL (search is skipped entirely). */
void fn_8018BDF4(u32 groupId, u32 index, void* pos) {
    s32 i;
    s32 j;
    PeopleEntry* found;
    PeopleEntry* entry;

    if (pos == NULL) {
        return;
    }

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry != NULL) {
        fn_8018FC98(entry, pos);
    }
}

/* fn_8018BF24 -- find a people entry by (groupId, index), angle-wrap a
 * caller-supplied Vec3 of angles in place, then feed it to fn_8018FC08 and
 * stash the Y component into entry->field_40. */
void fn_8018BF24(u32 groupId, u32 index, f32* vec) {
    s32 i;
    s32 j;
    PeopleEntry* found;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry != NULL) {
        vec[0] = (f32)fmod(lbl_8047D7F0 + vec[0]);
        vec[1] = (f32)fmod(lbl_8047D7F0 + vec[1]);
        vec[2] = (f32)fmod(lbl_8047D7F0 + vec[2]);
        fn_8018FC08(entry, vec);
        *(f32*)&entry->field_40 = vec[1];
    }
}

/* fn_8018C0A8 -- find a people entry by (groupId, index), fetch its scale
 * into *vec, then copy vec into the entry's transform. */
void fn_8018C0A8(u32 groupId, u32 index, void* vec) {
    s32 i;
    s32 j;
    PeopleEntry* found;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry != NULL) {
        fn_8018FC74(entry, vec);
        peopleSetTransform(entry, vec);
    }
}

/* fn_8018C1E8 -- not recovered, gap in archive campaign (size 0x23C) */
void fn_8018C1E8(void) {
}

/* fn_8018C424 -- find a people entry by (groupId, index) and test flags.
 * Returns 0 if no matching entry is found. */
BOOL fn_8018C424(u32 groupId, u32 index, u32 mask) {
    s32 i;
    s32 j;
    PeopleEntry* found;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry == NULL) {
        return 0;
    }
    return peopleTestFlags(entry, mask);
}

/* fn_8018C558 -- not recovered, gap in archive campaign (size 0x144) */
void fn_8018C558(void) {
}

/* fn_8018C69C -- find a people entry by (groupId, index) and clear flags. */
void fn_8018C69C(u32 groupId, u32 index, u32 mask) {
    s32 i;
    s32 j;
    PeopleEntry* found;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry != NULL) {
        peopleClearFlags(entry, mask);
    }
}

/* fn_8018C7C8 -- find a people entry by (groupId, index) and set flags. */
void fn_8018C7C8(u32 groupId, u32 index, u32 mask) {
    s32 i;
    s32 j;
    PeopleEntry* found;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry != NULL) {
        peopleSetFlags(entry, mask);
    }
}

/* fn_8018C8F4 -- find a people entry by (groupId, index) and overwrite flags. */
void fn_8018C8F4(u32 groupId, u32 index, u32 flags) {
    s32 i;
    s32 j;
    PeopleEntry* found;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry != NULL) {
        peopleWriteFlags(entry, flags);
    }
}

/* fn_8018CA20 -- find a people entry by (groupId, index) and set its shadow
 * animation, forcing 0 unless the entry already has an animId set. */
void fn_8018CA20(u32 groupId, u32 index, u8 animId) {
    s32 i;
    s32 j;
    PeopleEntry* found;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        found = entry->selfPtr;
        goto loop3;
    }

    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite((const char*)lbl_80273FD8, groupId, index);
        found = entry->selfPtr;
        goto loop3;
    }
    found = NULL;

loop3:
    for (j = 0; j < peopleGetMaxCount(); j++) {
        entry = peopleGetEntry(j);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        goto found_entry;
    }
    entry = NULL;

found_entry:
    if (entry != NULL) {
        if (entry->animId == 0) {
            animId = 0;
        }
        fn_8018FB2C(entry, animId);
    }
}

/* fn_8018CB5C -- not recovered, gap in archive campaign (size 0x1AC) */
void fn_8018CB5C(void) {
}

/* fn_8018CD08 -- not recovered, gap in archive campaign (size 0x978) */
void fn_8018CD08(void) {
}

/* fn_8018D7D0 -- not recovered, gap in archive campaign (size 0x158) */
void fn_8018D7D0(void) {
}

/* peopleSearchID -- not recovered, gap in archive campaign (size 0x70) */
void* peopleSearchID(u32 id) {
    s32 i;
    for (i = 0; i < peopleGetMaxCount(); i++) {
        PeopleEntry* entry = peopleGetEntry(i);
        if (entry->active != 0 && (u32)entry->selfPtr == id) {
            return entry;
        }
    }
    return NULL;
}

/* fn_8018D998 -- not recovered, gap in archive campaign (size 0xF0) */
void* fn_8018D998(u32 groupId, u32 index) {
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (entry->active != 0 && entry->groupId == groupId && entry->index == index) {
            return entry->selfPtr;
        }
    }
    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (entry->active != 0 && entry->index == index) {
            GSlogWrite((const char*)lbl_80273FD8, groupId, index);
            return entry->selfPtr;
        }
    }
    return NULL;
}

/* fn_8018DA88 -- not recovered, gap in archive campaign (size 0x7C) */
void fn_8018DA88(void) {
    void GSthreadBlock();
    s32 i;
    for (i = 0; i < peopleGetMaxCount(); i++) {
        PeopleEntry* entry = peopleGetEntry(i);
        if (entry->active != 0 && entry != NULL) {
            void* r;
            entry->visible = 0;
            r = (void*)fn_800F7108(entry->flagId);
            if (r != NULL) {
                GSthreadBlock(r);
            }
        }
    }
}

/* fn_8018DB04 -- not recovered, gap in archive campaign (size 0x64) */
void fn_8018DB04(void* param) {
    void fn_8018DCA8();
    s32 i;
    for (i = 0; i < peopleGetMaxCount(); i++) {
        PeopleEntry* entry = peopleGetEntry(i);
        if (entry->active != 0) {
            fn_8018DCA8(entry, param);
        }
    }
}

/* fn_8018E050 -- not recovered, gap in archive campaign (size 0x174) */
void fn_8018E050(void) {
}

/* fn_8018E1C4 = fn_8018E1C4 (see people.h) -- not recovered, gap in archive campaign */
void fn_8018E1C4(PeopleEntry* entry, void* spawnData, u32 motionId, u32 param) {

}

/* fn_8018E920 -- not recovered, gap in archive campaign (size 0x94) */
void fn_8018E920(u32 maxPeople) {
    void* data[3];
    s32 i;
    void** light;

    data[0] = (void*)lbl_80273F90[0];
    data[1] = (void*)lbl_80273F90[1];
    data[2] = (void*)lbl_80273F90[2];
    peopleInit(maxPeople);
    i = 0;
    light = (void**)gPeopleLights;
    while (i < 2) {
        *light = GSlightCreate();
        GSlightSetType(*light, 2);
        GSlightSetActive(*light, 0);
        i++;
        light++;
    }
    fn_800FF4D4(data, 1);
}

/* fn_8018F08C -- not recovered, gap in archive campaign (size 0x280) */
void fn_8018F08C(void) {
}

/* fn_8018F4C8 = fn_8018F4C8 (see people.h) -- not recovered, gap in archive campaign */
void fn_8018F4C8(void* entry, u8 param, s32* outNode, u8* outResult) {
    s8* data = entry;

    if (data == NULL) {
        return;
    }
    switch (param) {
    case 1:
        *outNode = data[1];
        *outResult = 1;
        break;
    case 2:
        *outNode = data[2];
        *outResult = 1;
        break;
    case 3:
        *outNode = data[3];
        *outResult = 1;
        break;
    case 4:
        *outNode = data[4];
        *outResult = 0;
        break;
    case 5:
        *outNode = data[1];
        *outResult = 1;
        break;
    case 6:
        *outNode = data[6];
        *outResult = 0;
        break;
    case 7:
        *outNode = data[7];
        *outResult = 1;
        break;
    case 8:
        *outNode = data[8];
        *outResult = 0;
        break;
    }
}

/* fn_8018F5E4 -- not recovered, gap in archive campaign (size 0x18) */
f32 fn_8018F5E4(void* ptr) {
    extern f32 lbl_8047D8A8;
    return (ptr != NULL) ? *(f32*)((u8*)ptr + 0x18) : lbl_8047D8A8;
}

/* fn_8018F618 -- not recovered, gap in archive campaign (size 0x20) */
f32 fn_8018F618(void* ptr) {
    extern f32 lbl_8047D8A8;
    extern f32 lbl_8047D8AC;
    return (ptr != NULL) ? lbl_8047D8AC * *(f32*)((u8*)ptr + 0x20) : lbl_8047D8A8;
}

/* fn_8018F638 -- not recovered, gap in archive campaign (size 0x20) */
f32 fn_8018F638(void* ptr) {
    extern f32 lbl_8047D8A8;
    extern f32 lbl_8047D8AC;
    return (ptr != NULL) ? lbl_8047D8AC * *(f32*)((u8*)ptr + 0x1c) : lbl_8047D8A8;
}

/* fn_8018F658 -- not recovered, gap in archive campaign (size 0x20) */
f32 fn_8018F658(void* ptr) {
    extern f32 lbl_8047D8A8;
    extern f32 lbl_8047D8AC;
    return (ptr != NULL) ? lbl_8047D8AC * *(f32*)((u8*)ptr + 0x20) : lbl_8047D8A8;
}

/* fn_8018F678 -- not recovered, gap in archive campaign (size 0x20) */
f32 fn_8018F678(void* ptr) {
    extern f32 lbl_8047D8A8;
    extern f32 lbl_8047D8AC;
    return (ptr != NULL) ? lbl_8047D8AC * *(f32*)((u8*)ptr + 0x1c) : lbl_8047D8A8;
}

/* fn_8018F698 -- not recovered, gap in archive campaign (size 0x1C) */
s32 fn_8018F698(void* ptr) {
    return (ptr != NULL) ? (s32)(s8)((u8*)ptr)[9] : -1;
}

/* peopleInfoBiosGetPtr = peopleInfoBiosGetPtr (see people.h) -- not recovered, gap in archive campaign */
void* peopleInfoBiosGetPtr(void* scriptObj) {
    u32 count = *(u32*)lbl_80478E78;
    PeopleInfoBiosEntry* entry = (PeopleInfoBiosEntry*)lbl_80478E7C;
    while (count != 0) {
        if (entry->scriptRef == scriptObj) {
            return entry;
        }
        entry++;
        count--;
    }
    return NULL;
}

/* fn_8018FB2C = fn_8018FB2C (see people.h) -- not recovered, gap in archive campaign */
void fn_8018FB2C(PeopleEntry* entry, u8 animId) {
    void GScolsys2HumanEnable();
    s32 f80;
    entry->shadowAnimId = animId;
    f80 = entry->shadowId;
    if (f80 >= 0) {
        GScolsys2HumanEnable(f80, animId);
    }
}

/* fn_8018FB60 = fn_8018FB60 (see people.h) -- not recovered, gap in archive campaign */
void fn_8018FB60(PeopleEntry* entry, u8 animId) {
    void* f8 = entry->modelHandle;
    if (f8 != NULL) {
        entry->animId = animId;
        GSmodelSetVisibility(f8, animId);
    }
}

/* fn_8018FC08 = fn_8018FC08 (see people.h) -- not recovered, gap in archive campaign
 * (2-arg signature corrected from caller fn_8018BF24's disassembly: r3=entry,
 * r4=vec -- the 1-arg forward decl was a placeholder). */
void fn_8018FC08(PeopleEntry* entry, void* vec) {
    extern void GSmodelSetRotation(void*, void*);
    GSmodelSetRotation(entry->modelHandle, vec);
}

/* fn_8018FC2C = fn_8018FC2C (see people.h) -- not recovered, gap in archive campaign */
void fn_8018FC2C(PeopleEntry* entry) {
    extern void GSmodelGetRotation(void*);
    GSmodelGetRotation(entry->modelHandle);
}

/* fn_8018FC74 = fn_8018FC74 (see people.h) -- not recovered, gap in archive campaign
 * (2-arg signature corrected from caller fn_8018C0A8's disassembly). */
void fn_8018FC74(PeopleEntry* entry, void* vec) {
    extern void GSmodelSetPosition(void*, void*);
    GSmodelSetPosition(entry->modelHandle, vec);
}

/* fn_8018FC98 = fn_8018FC98 (see people.h) -- not recovered, gap in archive campaign */
void fn_8018FC98(PeopleEntry* entry, void* pos) {
    extern void GSmodelGetPosition(void*, void*);
    GSmodelGetPosition(entry->modelHandle, pos);
}

/* fn_8018FCBC = fn_8018FCBC (see people.h) -- not recovered, gap in archive campaign */
void* fn_8018FCBC(PeopleEntry* entry) {
    extern void* GSmodelGetPositionPtr(void*);
    return GSmodelGetPositionPtr(entry->modelHandle);
}
