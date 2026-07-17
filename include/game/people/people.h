/**
 * @file people.h
 * @brief People/NPC system -- structures, enums, and function declarations.
 *
 * The "People" system manages all NPCs in the overworld. Each NPC is tracked
 * as a PeopleEntry in a flat array (max count set at init time). NPCs are
 * identified by a pair of IDs: a "group" ID (floor/area) and an "index"
 * within that group. A pointer indirection (self-pointer at +0x04) is used
 * to resolve script-side object references.
 *
 * All people functions reside in a single translation unit (people.c),
 * covering core management, movement, and interaction subsystems.
 *
 * Data was recovered from disassembly of address range:
 *   0x80180C78 - 0x8018FE30  (People/NPC system, ~227 functions)
 *
 * Debug string: "Warining: people[%d,%d] group is different!!"
 *               "ERROR! [%s]: People[%d,%d] WalkMotion[%d] is frame zero."
 *               "ERROR! [%s]: People[%d,%d] RunMotion[%d] is frame zero."
 *               "talk -> people(%d,%d)  len =%.2f  ang =%.2f  area =%.2f"
 *               "peopleOpenSub", "peopleWaitSyncMotion",
 *               "peopleWaitSyncMotionBlend", "peopleMoveCheck"
 */

#ifndef GAME_PEOPLE_PEOPLE_H
#define GAME_PEOPLE_PEOPLE_H

#include "dolphin/types.h"

/* =========================================================================
 * Constants
 * ========================================================================= */

/** Maximum number of people that can be alive per iteration in peopleUpdate */
#define PEOPLE_UPDATE_TICK_COUNT   0x3C  /* 60 */

/** Default people entry stride in the flat array */
#define PEOPLE_ENTRY_SIZE          0xDC  /* 220 bytes */

/** Size of NPC spawn data record from floor data */
#define PEOPLE_SPAWN_DATA_SIZE     0x158

/** Initial flags value to mark an NPC as active */
#define PEOPLE_FLAG_ACTIVE         0x0001

/** Flag: NPC is interactable (talkable) */
#define PEOPLE_FLAG_TALKABLE       0x0002

/** Flag: NPC has 3D model loaded */
#define PEOPLE_FLAG_HAS_MODEL      0x4000

/** Walk-list behavior flags */
#define PEOPLE_WALK_LIST_ACTIVE    0x0700

/* =========================================================================
 * Enumerations
 * ========================================================================= */

/**
 * People state -- stored at offset 0x54 of PeopleEntry.
 * Controls the NPC's behavior state machine.
 */
typedef enum PeopleState {
    PEOPLE_STATE_IDLE         = 0,
    PEOPLE_STATE_RESERVED1    = 1,
    PEOPLE_STATE_RESERVED2    = 2,
    PEOPLE_STATE_RESERVED3    = 3,
    PEOPLE_STATE_INTERACTING  = 4,  /* NPC is being talked to / interacted with */
    PEOPLE_STATE_CUTSCENE     = 5,  /* NPC is controlled by a cutscene script */
    PEOPLE_STATE_INACTIVE     = 6,  /* NPC is deactivated (hidden) */
} PeopleState;

/**
 * People movement type -- stored at offset 0x96 of PeopleEntry.
 * Determines how the NPC moves on each frame.
 */
typedef enum PeopleMoveType {
    PEOPLE_MOVE_NONE           = 0,  /* Standing still, no auto-movement */
    PEOPLE_MOVE_WALK_PATH      = 1,  /* Walking along a predefined path */
    PEOPLE_MOVE_WALK_POSITION  = 2,  /* Walking toward a target XYZ position */
    PEOPLE_MOVE_RUN_POSITION   = 3,  /* Running toward a target XYZ position */
} PeopleMoveType;

/* =========================================================================
 * Structures
 * ========================================================================= */

/**
 * PeopleEntry -- runtime data for one NPC.
 *
 * Total size: 0xDC bytes (220 bytes).
 *
 * Field offsets recovered from disassembly:
 *   0x00: u8   active          -- nonzero if this slot is in use
 *   0x04: void* selfPtr        -- points to this entry (for script lookup)
 *   0x08: void* modelHandle    -- GS model system handle (NULL if no model)
 *   0x0C: (unknown)
 *   0x10: (unknown)
 *   0x14: (unknown)
 *   0x18: (unknown)
 *   0x1C: void* nextLink       -- linked list next pointer (for chaining)
 *   0x20: u8   visible         -- whether the NPC should be drawn
 *   0x21: u8   animId          -- current walk/idle animation bank index
 *   0x23: u8   shadowAnimId    -- shadow/alt animation index
 *   0x24: u32  flags           -- bitfield of PEOPLE_FLAG_* values
 *   0x28: u32  groupId         -- floor/area group this NPC belongs to
 *   0x2C: u32  index           -- index within the group
 *   0x30: void* scriptRef      -- reference to associated script object
 *   0x34: (unknown)
 *   0x38: (unknown)
 *   0x3C: f32  talkRange       -- interaction distance (for talk checks)
 *   0x40-0x47: (unknown)
 *   0x48: s32  walkTargetNode  -- target node for walk behavior
 *   0x4C: u16  flagId          -- game flag ID that controls visibility
 *   0x50: s32  shadowId        -- shadow index (-1 if none)
 *   0x54: u8   state           -- PeopleState enum
 *   0x55: u8   subState        -- sub-state for interaction/cutscene
 *   0x56: u8   talkLock        -- lock flag during talk interaction
 *   0x57: u8   prevState       -- saved previous state
 *   0x58: f32  moveSpeed       -- current movement speed
 *   0x5C-0x73: (unknown -- likely position/rotation data)
 *   0x74: Vec3 collisionPos    -- collision/interaction position vector
 *   0x80-0x83: (unknown)
 *   0x84: f32  animBlendFactor -- animation blending weight
 *   0x88-0x8F: (unknown)
 *   0x90: u32  motionIndex     -- current motion/animation sequence index
 *   0x94: (unknown)
 *   0x95: u8   isTalkable      -- cached talkability check result
 *   0x96: u8   moveType        -- PeopleMoveType enum
 *   0x9C: Vec3 transform       -- position/transform (set via GSvecCopy)
 *   0xA8: f32  targetX         -- walk target X position
 *   0xAC: f32  targetY         -- walk target Y position
 *   0xB0: f32  targetZ         -- walk target Z position
 *   0xB4-0xB7: (unknown)
 *   0xB8-0xBB: (unknown)
 *   0xBC-0xDB: (extended spawn data, copied from floor resource)
 *     0xC0: u32 walkPathId     -- walk path list data 1
 *     0xC4: u32 walkPathParam  -- walk path list data 2
 *     0xC8: s32 walkNodeA      -- walk motion node A (-1 = unset)
 *     0xCC: s32 walkNodeB      -- walk motion node B (-1 = unset)
 *     0xD0: s32 walkNodeC      -- walk motion node C (-1 = unset)
 *     0xD4-0xD7: (unknown)
 *     0xD8: f32 walkAnimRate   -- walk animation playback rate
 */
typedef struct PeopleEntry {
    u8   active;            /* 0x00 */
    u8   pad01[3];          /* 0x01 */
    void* selfPtr;          /* 0x04 */
    void* modelHandle;      /* 0x08 */
    u32  field_0C;          /* 0x0C */
    void* updateCallback;   /* 0x10 */
    u32  field_14;          /* 0x14 */
    void* threadHandle;     /* 0x18 */
    void* nextLink;         /* 0x1C */
    u8   visible;           /* 0x20 */
    u8   animId;            /* 0x21 */
    u8   pad22;             /* 0x22 */
    u8   shadowAnimId;      /* 0x23 */
    u32  flags;             /* 0x24 */
    u32  groupId;           /* 0x28 */
    u32  index;             /* 0x2C */
    void* scriptRef;        /* 0x30 */
    u32  field_34;          /* 0x34 */
    u32  field_38;          /* 0x38 */
    f32  talkRange;         /* 0x3C */
    u32  field_40;          /* 0x40 */
    u32  field_44;          /* 0x44 */
    s32  walkTargetNode;    /* 0x48 */
    u16  flagId;            /* 0x4C */
    u16  pad4E;             /* 0x4E */
    s32  shadowId;          /* 0x50 */
    u8   state;             /* 0x54 */
    u8   subState;          /* 0x55 */
    u8   talkLock;          /* 0x56 */
    u8   prevState;         /* 0x57 */
    f32  moveSpeed;         /* 0x58 */
    u8   field_5C[0x18];    /* 0x5C - 0x73: position/rotation */
    u8   collisionData[0x10]; /* 0x74 - 0x83: collision position */
    f32  animBlendFactor;   /* 0x84 */
    u8   field_88[0x08];    /* 0x88 - 0x8F */
    u32  motionIndex;       /* 0x90 */
    u32  field_94;          /* 0x94 */
    u8   isTalkable;        /* 0x95 (stored from talk check) */
    u8   moveType;          /* 0x96 */
    u8   pad97;             /* 0x97 */
    u32  field_98;          /* 0x98 */
    u8   transform[0x0C];   /* 0x9C - 0xA7: position transform */
    f32  targetX;           /* 0xA8 */
    f32  targetY;           /* 0xAC */
    f32  targetZ;           /* 0xB0 */
    u32  field_B4;          /* 0xB4 */
    u32  field_B8;          /* 0xB8 */
    u8   spawnData[0x20];   /* 0xBC - 0xDB: extended data from floor */
} PeopleEntry;

/**
 * PeopleInfoBiosEntry -- entry in the separate "info bios" table referenced
 * by lbl_80478E78 (count, indirected through a stored pointer value) /
 * lbl_80478E7C (array base). This is NOT a PeopleEntry: distinct base
 * pointer, distinct stride (0x2C bytes vs PeopleEntry's 0xDC).
 *
 * Field offsets recovered from disassembly:
 *   0x00-0x0B: (unknown)
 *   0x0C: void* scriptRef -- script object pointer, compared for identity
 *                            in peopleInfoBiosGetPtr
 *   0x10-0x2B: (unknown)
 */
typedef struct PeopleInfoBiosEntry {
    u8    field_00[0x0C];   /* 0x00 - 0x0B */
    void* scriptRef;        /* 0x0C */
    u8    field_10[0x1C];   /* 0x10 - 0x2B */
} PeopleInfoBiosEntry;

/**
 * PeopleOpenWork -- transient work structure used during peopleOpen.
 * Passed in the "generator" thread context.
 *
 * Offsets referenced from the open callback:
 *   0x00: u32 state         -- open state machine phase
 *   0x0C: u32 subState      -- sub-state
 *   0x10: void* callback    -- function pointer (set to fn_80181094)
 *   0x14: u32 param1
 *   0x18: void* threadObj   -- created thread/task handle
 *   0x1C: void* nextLink
 *   0x20: void* parentObj   -- parent (floor) object
 *   0x24: u32 floorParam    -- floor parameter (+0xF4 of parent)
 *   0x28: void* extraData
 *   0x34: u32 field_34
 *   0x38: (unknown)
 */
typedef struct PeopleOpenWork {
    u32   state;            /* 0x00 */
    u32   pad04;            /* 0x04 */
    u32   pad08;            /* 0x08 */
    u32   subState;         /* 0x0C */
    void* callback;         /* 0x10 */
    u32   param1;           /* 0x14 */
    void* threadObj;        /* 0x18 */
    void* nextLink;         /* 0x1C */
    void* parentObj;        /* 0x20 */
    u32   floorParam;       /* 0x24 */
    void* extraData;        /* 0x28 */
    u8    padRest[0x78];    /* 0x2C+ */
} PeopleOpenWork;

/* =========================================================================
 * Function declarations -- people.c (Core)
 * ========================================================================= */

/** Initialize the people system: allocate array for 'maxPeople' slots.
 *  fn_8018FDD0 -- called early during floor/world init. */
PeopleEntry* peopleInit(u32 maxPeople);

/** Get the maximum number of people slots. fn_8018FDB4 */
s32 peopleGetMaxCount(void);

/** Get a PeopleEntry by slot index. fn_8018FD88 */
PeopleEntry* peopleGetEntry(s32 index);

/** Allocate a free people slot and initialize it. fn_8018FCE0 */
PeopleEntry* peopleAlloc(void);

/** Free/despawn a people entry. fn_8018FDBC */
s32 peopleFree(PeopleEntry* entry);

/** Main per-frame update for all active people. fn_80181850 */
void peopleUpdate(void);

/** Initialize people for a floor (draw 30 blank frames). fn_80181224 */
void peopleFloorInit(void);

/** Open/spawn callback -- called from the floor loader. fn_80180C78 (includes fn_80181094) */
s32 peopleOpenCallback(PeopleOpenWork* work);

/** Close/cleanup callback for all people in a group. fn_8018114C */
void peopleCloseCallback(void);

/** Get the model handle from a PeopleEntry. fn_8018FBD4 */
void* peopleGetModel(PeopleEntry* entry);

/** Test whether flags are set on a people entry. fn_8018FB94 */
BOOL peopleTestFlags(PeopleEntry* entry, u32 mask);

/** Set flag bits on a people entry. fn_8018FBBC */
void peopleSetFlags(PeopleEntry* entry, u32 mask);

/** Clear flag bits on a people entry. fn_8018FBAC */
void peopleClearFlags(PeopleEntry* entry, u32 mask);

/** Overwrite all flags on a people entry. fn_8018FBCC */
void peopleWriteFlags(PeopleEntry* entry, u32 flags);

/** Set the world transform for a people entry. fn_8018FBDC */
void peopleSetTransform(PeopleEntry* entry, void* mtx);

/** Get the world transform pointer for a people entry. fn_8018FC00 */
void* peopleGetTransform(PeopleEntry* entry);

/* =========================================================================
 * Function declarations -- Movement subsystem
 * ========================================================================= */

/** Tick-update one NPC's movement (called 60 times per update cycle).
 *  fn_801812C4 -> fn_8018ECEC */
void peopleMoveUpdate(PeopleEntry* entry);

/** Main movement dispatch by moveType. fn_8018ECEC */
void peopleMoveDispatch(f32 dt);

/** Walk-path movement handler. fn_80188FA0 */
void peopleMoveWalkPath(u32 groupId, u32 index, u32 pathId, u32 pathParam);

/** Walk-to-position movement handler. fn_80188CA0 */
void peopleMoveWalkPosition(u32 groupId, u32 index, u32 targetX, u32 targetY, u32 targetZ);

/** Walk-list "add" handler -- self-tag string proven at 0x80183FC4
 * (named peopleAddWalkList; the old fn_80188AF4 attribution here was
 * wrong, same class as the peopleMoveCheck fix). */

/** Check movement completion (self-tag string lbl_8036C52C; scrCmd table
 * index 62). 0x8018A280 -- the old fn_8018F4C8 attribution here was wrong.
 * Real ABI per disasm: r3-r5 = groupId, index, waitFlag (was void(void)). */
BOOL peopleMoveCheck(u32 groupId, u32 index, u8 waitFlag);

/** Proximity check -- is the player within talk range? fn_8018D680 */
BOOL peopleIsWithinRange(u32 posA, u32 posB, u32 posC, f32 range);

/* =========================================================================
 * Function declarations -- Interaction subsystem
 * ========================================================================= */

/** Look up a people entry by (groupId, index) and handle talk state.
 *  fn_801812E8 -- called from psinterpret (script system). */
s32 peopleFindAndInteract(u32 groupId, u32 index, u8 doInteract);

/** Extended find-and-interact with motion/animation setup.
 *  fn_80181478 -- sets walk target, animation, and motion data. */
s32 peopleFindAndSetupMotion(u32 groupId, u32 index, u8 doSetup);

/** Open an NPC and configure its model, motion, and initial state.
 *  fn_8018E1C4 -- called during peopleOpen to set up a spawned NPC.
 *  Parameters come from the floor spawn data. */
void peopleOpenSetup(PeopleEntry* entry, void* spawnData, u32 motionId, u32 param);

/** Full open procedure with spawn data processing.
 *  peopleBiosPopData -- iterates spawn data records and calls peopleOpenSetup. */
void peopleOpenFromSpawnData(void* spawnDataArray, u32 count, u32 groupId);

/** Set the NPC's animation bank. fn_8018FB60 */
void peopleSetAnim(PeopleEntry* entry, u8 animId);

/** Set the NPC's shadow animation. fn_8018FB2C */
void peopleSetShadowAnim(PeopleEntry* entry, u8 animId);

/** Get rotation data from model. fn_8018FC2C */
void peopleGetRotation(PeopleEntry* entry);

/** Get position data from model. fn_8018FC50 */
void peopleGetPosition(PeopleEntry* entry);

/** Get scale data from model. fn_8018FC74 */
void peopleGetScale(PeopleEntry* entry);

/** Set position on model. fn_8018FC98 */
void peopleSetPosition(PeopleEntry* entry, void* pos);

/** Get model position (world coords). fn_8018FCBC */
void* peopleGetModelPosition(PeopleEntry* entry);

/** Get model rotation. fn_8018FC08 */
void peopleGetModelRotation(PeopleEntry* entry);

/** Query script ref from a people entry's model. peopleInfoBiosGetPtr */
void* peopleGetScriptRef(void* scriptObj);

#endif /* GAME_PEOPLE_PEOPLE_H */
