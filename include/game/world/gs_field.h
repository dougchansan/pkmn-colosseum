/**
 * @file gs_field.h
 * @brief GSfield -- Field/world management system for Pokemon Colosseum.
 *
 * GSfield is the largest subsystem in the game engine (142KB, 824 functions).
 * It sits between GScolsys (collision) and GSeffect (VFX) in the link order
 * and manages everything related to the overworld/field:
 *
 *   - Collision-to-world integration (ray casts, triangle lookups via GScolsys)
 *   - Floor resource loading (GFL, WZX, PKX, textures, cameras, maps,
 *     scripts, fonts, messages, particles, sound buffers)
 *   - Field camera system (position, target, FOV, interpolation)
 *   - Scene data parsing and relocation
 *   - Field object management (placement, visibility, animation)
 *   - World state updates (per-frame tick for field logic)
 *   - NPC spawn point and region management
 *
 * The system is organized into three logical sub-modules:
 *   1. gs_field_colquery.c  -- collision query wrappers (0x8010E138-0x80114300)
 *   2. gs_field_resource.c  -- floor resource pre-funcs   (0x8011432C-0x80114CA8)
 *   3. gs_field_world.c     -- field camera, world logic  (0x80114CA8-0x80130CE0)
 *
 * Debug strings:
 *   "scene_data"
 *   "floorReadGFLPreFunc(): can't alloc %d bytes of memory"
 *   "ERROR: Over Sound Buffer! snd_res_id=%d buffer size=%d"
 *   "floorReadParticlePreFunc(): can't alloc %d bytes of memory"
 *   "floorReadWZXPreFunc(): can't alloc %d bytes of memory"
 *   "floorReadPKXPreFunc(): can't alloc %d bytes of memory"
 *   "floorReadTexPreFunc(): can't alloc %d bytes of memory"
 *   "floorReadCameraPreFunc: can't alloc %d bytes of memory"
 *   "floorReadMapPreFunc: can't alloc %d bytes of memory"
 *   "floorReadScriptPreFunc(): can't alloc %d bytes of memory"
 *   "floorReadFontPreFunc(): can't alloc %d bytes of memory"
 *   "floorReadMsgPreFunc(): can't alloc %d bytes of memory"
 *   "floorReadNormalPreFunc(): can't alloc %d bytes of memory"
 *   "floorUpdateFieldCamera: error updating field camera - divide by zero!"
 *
 * Address range: 0x8010E138 - 0x80130CE0 (142KB, 824 functions)
 */
#ifndef GS_FIELD_H
#define GS_FIELD_H

#include "dolphin/types.h"

/* ===================================================================
 * Constants
 * =================================================================== */

/** Maximum number of collision query results stored per query */
#define GSFIELD_MAX_QUERY_RESULTS   8

/** Number of floor resource types (GFL, sound, particle, WZX, PKX,
 *  texture, camera, map, script, font, message, normal) */
#define GSFIELD_NUM_RES_TYPES       12

/** Size of the field camera state structure */
#define GSFIELD_CAMERA_STATE_SIZE   0x50

/* ===================================================================
 * Collision query result
 * =================================================================== */

/**
 * Per-hit result from a collision ray/sweep test.
 * Stored in arrays of up to GSFIELD_MAX_QUERY_RESULTS entries.
 * Each entry is 0x0C bytes.
 */
typedef struct GSFieldHitResult {
    /* 0x00 */ f32 distance;    /**< parametric distance along ray */
    /* 0x04 */ u16 triIndex;    /**< triangle index within mesh */
    /* 0x06 */ u16 meshIndex;   /**< collision mesh layer index */
    /* 0x08 */ u8  surfaceType; /**< surface type ID (from WZX) */
    /* 0x09 */ u8  flags;       /**< hit flags (double-sided, etc.) */
    /* 0x0A */ u8  pad[2];
} GSFieldHitResult;

/* ===================================================================
 * Floor resource pre-function callbacks
 * =================================================================== */

/**
 * Callback signature for floor resource pre-load functions.
 * Each resource type (GFL, sound, particle, etc.) has a pre-func
 * that allocates memory and sets up loading state before the
 * actual async load begins.
 *
 * @param resId     Resource slot ID within the floor archive
 * @param loadMode  Loading mode flags
 * @param dataSize  Size of the resource data in bytes
 * @return          Pointer to allocated buffer, or NULL on failure
 */
typedef void* (*GSFieldResPreFunc)(u32 resId, u32 loadMode, u32 dataSize);

void* floorOpenObject(u32 modelIndex);

/* ===================================================================
 * Field camera state
 * =================================================================== */

/**
 * Field camera interpolation and control state.
 * Managed by floorUpdateFieldCamera at 0x80117514.
 */
typedef struct GSFieldCamera {
    /* 0x00 */ f32 posX, posY, posZ;        /**< current camera position */
    /* 0x0C */ f32 targetX, targetY, targetZ; /**< look-at target */
    /* 0x18 */ f32 fov;                      /**< field of view (degrees) */
    /* 0x1C */ f32 nearClip;                 /**< near clip plane */
    /* 0x20 */ f32 farClip;                  /**< far clip plane */
    /* 0x24 */ f32 interpSpeed;              /**< interpolation rate (0-1) */
    /* 0x28 */ u32 flags;                    /**< camera mode flags */
    /* 0x2C */ u32 state;                    /**< camera state machine step */
    /* 0x30 */ f32 destPosX, destPosY, destPosZ;   /**< destination position */
    /* 0x3C */ f32 destTargX, destTargY, destTargZ; /**< destination target */
    /* 0x48 */ f32 destFov;                  /**< destination FOV */
    /* 0x4C */ u32 pad;
} GSFieldCamera;

/* ===================================================================
 * Public API -- Collision Queries (gs_field_colquery.c)
 * =================================================================== */

/**
 * fn_8010E138 -- Cast a ray against the collision mesh and store results.
 *
 * Iterates over collision triangles in the active WZX data, transforms
 * vertices through the model-view matrix, and tests ray intersection.
 * Results are sorted by distance and stored in the output array.
 *
 * @param origin    Ray origin (Vec3f*)
 * @param direction Ray direction/extent (Vec3f*)
 * @return          Number of hits found (0 = no collision)
 */
s32 GSfield_RayCast(void* origin, void* direction);

/**
 * fn_8010E53C -- Perform a swept-sphere collision test.
 */
s32 GSfield_SphereSweep(void* origin, void* direction, f32 radius);

/**
 * fn_8010EFE4 -- Test a line segment against a collision plane.
 */
s32 GSfield_LinePlaneTest(void* segStart, void* segEnd,
                           void* planeNormal, s32 doubleSided);

/* ===================================================================
 * Public API -- Floor Resource Pre-funcs (gs_field_resource.c)
 * =================================================================== */

void* floorReadGFLPreFunc(u32 resId, u32 loadMode, u32 dataSize);
void* floorReadNotLinkedParticlePreFunc(u32 resId, u32 loadMode, u32 dataSize);
void* floorReadParticlePreFunc(u32 resId, u32 loadMode, u32 dataSize);
void* floorReadWZXPreFunc(u32 resId, u32 loadMode, u32 dataSize);
void* floorReadPKXPreFunc(u32 resId, u32 loadMode, u32 dataSize);
void* floorReadCameraPreFunc(void* owner, u32 param, u32 alloc_size);
void* floorReadMapPreFunc(void* owner, u32 param, u32 alloc_size);

/* ===================================================================
 * Public API -- Field Camera (gs_field_world.c)
 * =================================================================== */

u8 fn_80117AE4(u32 mode);

/**
 * floorUpdateFieldCamera -- Update the field camera each frame.
 *
 * Interpolates between current and destination camera parameters.
 * Includes a divide-by-zero safety check (see debug string).
 */
u8 floorUpdateFieldCamera();

#endif /* GS_FIELD_H */
