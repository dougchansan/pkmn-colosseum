/**
 * @file gs_colsys.h
 * @brief GScolsys2 -- surface-type interaction table + matched leaf helpers.
 *
 * Backs src/game/gs_colsys.c, unit 0x8010C364-0x8010CBD0 (16 fns, 9
 * matched). Declares only what that unit's real, compiled code actually
 * needs: the collision-state struct (lbl_80404C68), the surface-type
 * table entry layout (lbl_8035B500), and the handful of size/count
 * constants those two rely on.
 *
 * A prior campaign-generation transplant had attached a full invented
 * "GScolsys2_*" public API to this header (WZX mesh loading/relocation,
 * transform building, debug-draw display lists, ground raycasting --
 * ~30 prototypes plus supporting mesh/triangle structs), matching a
 * fiction block that was removed from gs_colsys.c as dead, unreferenced
 * code (see that file's header comment). None of those names appear in
 * symbols.txt and nothing outside gs_colsys.c ever included this header,
 * so the fictional declarations have been removed along with it.
 *
 * Address range: 0x8010C364 - 0x8010CBD0
 */
#ifndef GS_COLSYS_H
#define GS_COLSYS_H

#include "dolphin/types.h"

/* ===================================================================
 * Constants
 * =================================================================== */

/** Maximum number of collision layers (double-buffered) */
#define GSCOLSYS_MAX_LAYERS         4

/** Size of one collision layer in bytes */
#define GSCOLSYS_LAYER_SIZE         0xDC0

/** Size of one triangle collision entry */
#define GSCOLSYS_TRI_ENTRY_SIZE     0x28

/** Number of surface types in the type table */
#define GSCOLSYS_NUM_SURFACE_TYPES  0x12

/* ===================================================================
 * Structures
 * =================================================================== */

/** 3D vector (12 bytes) -- parameter type of the matched
 *  GScolsy2UtilGetSidePlanePoint. */
typedef struct Vec3f {
    f32 x, y, z;
} Vec3f;

/**
 * Surface type table entry -- 0x2C bytes.
 *
 * Stored in the static table at lbl_8035B500. Contains surface
 * type attributes and a type-vs-type interaction matrix.
 *
 * Fields recovered from fn_8010C46C..zokuseiDataBiosGetAisyou:
 *   0x00: u8   surfaceFlags
 *   0x02: u16  surfaceId
 *   0x04: u32  surfaceParam
 *   0x08: u16[18] typeInteraction  -- interaction codes vs other types
 */
typedef struct GSColSurfaceType {
    /* 0x00 */ u8   surfaceFlags;
    /* 0x01 */ u8   pad01;
    /* 0x02 */ u16  surfaceId;
    /* 0x04 */ u32  surfaceParam;
    /* 0x08 */ u16  typeInteraction[GSCOLSYS_NUM_SURFACE_TYPES];
} GSColSurfaceType;

/**
 * Per-floor copy of one CCD object's placement -- 0x28 bytes.
 * fn_8010CD6C fills trans/rot/scale from the loaded CCD records and
 * clears the flags; GScolsys2Get/SetObjEnable test and toggle bit 0.
 */
typedef struct GSColFloorObj {
    /* 0x00 */ Vec3f trans;
    /* 0x0C */ Vec3f rot;
    /* 0x18 */ Vec3f scale;
    /* 0x24 */ u16   flags;
    /* 0x26 */ u8    pad26[2];
} GSColFloorObj;

/**
 * Floor human-collision slot -- 0x14 bytes. fn_80110084 registers one,
 * GScolsys2HumanEnable toggles it and GScolsys2CheckCollision resolves the person
 * from keys 0/1 (fn_8018D998 -> peopleSearchID) and tests a cylinder of
 * the given radius and height. Flags: bit 0 in use, bit 1 disabled.
 */
typedef struct GSColFloorEvent {
    /* 0x00 */ s32 key0;
    /* 0x04 */ s32 key1;
    /* 0x08 */ f32 radius;
    /* 0x0C */ f32 height;
    /* 0x10 */ u16 flags;
    /* 0x12 */ u8  pad12[2];
} GSColFloorEvent;

/** One collision floor (layer) -- GSCOLSYS_LAYER_SIZE (0xDC0) bytes. */
typedef struct GSColFloor {
    /* 0x000 */ GSColFloorObj   objs[64];
    /* 0xA00 */ GSColFloorEvent events[48];
} GSColFloor;

/**
 * Main collision system state -- 0x3710 bytes (BSS at lbl_80404C68).
 *
 * Layout:
 *   0x0000: void*  wzxDataPtr      -- pointer to active WZX collision data
 *   0x0004: GSColFloor floors[4], 0xDC0 bytes each
 *   0x3704: s32    activeLayer     -- current active layer index (-1 = none)
 *   0x3708: u32    gfxRenderHandle -- GSgfx render object handle for debug draw
 *   0x370C: void*  displayList     -- cached display list for debug collision vis
 */
typedef struct GSColSysState {
    /* 0x0000 */ void*  wzxDataPtr;
    /* 0x0004 */ GSColFloor floors[GSCOLSYS_MAX_LAYERS];
    /* 0x3704 */ s32    activeLayer;
    /* 0x3708 */ u32    gfxRenderHandle;
    /* 0x370C */ void*  displayList;
} GSColSysState;

s32 GScolsys2UtilGetCpPlaneLine(Vec3f* intersection, f32* lineParameter,
                                const Vec3f* normal,
                                const Vec3f* planePoint,
                                const Vec3f* lineStart,
                                const Vec3f* lineEnd);

#endif /* GS_COLSYS_H */
