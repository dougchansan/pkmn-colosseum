/**
 * @file gs_floor.h
 * @brief GSfloor -- Genius Sonority floor/scene management system.
 *
 * GSfloor manages the loading and unloading of world areas ("floors") in
 * Pokemon Colosseum. Each floor corresponds to a named area (e.g. D1_garage_1F,
 * M1_out, S1_out) and has associated FSYS archives containing models, scripts,
 * textures, particles, sound, collision data, and a REL module for area code.
 *
 * The floor system uses a cooperative thread (GSthread) that runs as a state
 * machine. Floor transitions follow this sequence:
 *   1. Unload old floor resources (textures, particles, collision, REL)
 *   2. Load new floor FSYS archive
 *   3. Parse and allocate floor resource chunks (map, textures, particles, etc.)
 *   4. Register loaded resources with the engine subsystems
 *   5. Resume gameplay on the new floor
 *
 * The floor resource handler table at lbl_80404918 stores 0x18 entries of 0x10
 * bytes each, mapping resource type IDs to load/size callbacks; it is used
 * directly by gs_floor_data.c via GSFloorResHandler.
 *
 * Address range: 0x800FF788 - 0x80110000 (GSfloor core, not yet decompiled
 * beyond the stub scaffold in gs_floor.c)
 *
 * Note: the floorRead*PreFunc resource handler family (0x8011432C+) is a
 * separate unit declared in game/world/gs_field.h and defined across
 * gs_field_resource.c / gs_field_world.c; it is not part of this header.
 *
 * Note: lbl_80402518 is NOT this unit's floor data table -- it is
 * gs_model.c's model resource table (see gs_model.c). An earlier pass
 * mislabeled it; the struct/const comments below have been corrected.
 */
#ifndef GS_FLOOR_H
#define GS_FLOOR_H

#include "dolphin/types.h"

/* ===================================================================
 * Constants
 * =================================================================== */

/** Maximum number of floor data entries in the static table */
#define GSFLOOR_MAX_ENTRIES     0x80

/** Size of one floor data entry in bytes */
#define GSFLOOR_ENTRY_SIZE      0x48

/** Maximum number of resource type handlers */
#define GSFLOOR_MAX_RES_TYPES   0x18

/** Size of one resource handler entry in bytes */
#define GSFLOOR_RES_HANDLER_SIZE 0x10

/** Floor ID encoding: upper 16 bits are floor index, lower 16 bits are 0x5001 */
#define GSFLOOR_ID_BASE         0x5001

/** Invalid floor ID sentinel */
#define GSFLOOR_ID_INVALID      ((u32)-1)

/* ===================================================================
 * Floor state machine phases
 * =================================================================== */
typedef enum GSFloorState {
    GSFLOOR_STATE_IDLE          = 0,  /**< No floor active, waiting for open */
    GSFLOOR_STATE_LOADING       = 1,  /**< Loading new floor resources */
    GSFLOOR_STATE_RUNNING       = 2,  /**< Floor loaded, running resource updates */
    GSFLOOR_STATE_UNLOADING     = 3,  /**< Unloading current floor, preparing transition */
    GSFLOOR_STATE_TRANSITIONING = 4,  /**< Mid-transition, switching floor data */
    GSFLOOR_STATE_FINALIZING    = 5,  /**< Final phase, completing transition */
} GSFloorState;

/* ===================================================================
 * Floor resource chunk states (used by GSFloorResource.status)
 * =================================================================== */
typedef enum GSFloorResStatus {
    GSFLOOR_RES_FREE       = 0,  /**< Resource slot unused */
    GSFLOOR_RES_LOADED     = 1,  /**< Resource loaded into memory */
} GSFloorResStatus;

/* ===================================================================
 * Structures
 * =================================================================== */

/**
 * GSFloorContext -- runtime floor state, accessed via gsFloorCurrent.
 *
 * Size: at least 0x14 bytes. Stored in the GSmem-allocated pool
 * (handle gsFloorPoolHandle, pointer gsFloorPoolPtr).
 *
 * Fields recovered from disassembly of GSfloorOpen and _floorManagerThread:
 *   0x00: void*  floorDataEntry  -- pointer to GSFloorDataEntry for this floor
 *   0x04: u32    floorId         -- composite floor ID (floor_index * 0x5001 + base)
 *   0x08: u16    resMemHandle    -- GSmem handle for resource memory block
 *   0x0A: u8     doFadeIn        -- if nonzero, fade in on entry
 *   0x0B: u8     doFadeOut       -- if nonzero, fade out on exit
 *   0x10: s32    isActive        -- resource phase run by fn_80100B24 (1..6)
 */
typedef struct GSFloorContext {
    /* 0x00 */ void*   floorDataEntry;
    /* 0x04 */ u32     floorId;
    /* 0x08 */ u16     resMemHandle;
    /* 0x0A */ u8      doFadeIn;
    /* 0x0B */ u8      doFadeOut;
    /* 0x0C */ u32     pad0C;
    /* 0x10 */ s32     isActive;
} GSFloorContext;

/**
 * GSFloorResource -- describes one loaded resource chunk.
 *
 * Size: 0x24 bytes (36 bytes).
 * The floor system allocates an array of these: total = maxBase + maxExt1 + maxExt2.
 * Resources are organized into three pools:
 *   Pool 0: base resources (count = maxBase)
 *   Pool 1: extended pool 1 (count = maxExt1, starts at base[maxBase])
 *   Pool 2: extended pool 2 (count = maxExt2, starts at pool1[maxExt1])
 *
 * Fields recovered from iteration patterns in _floorManagerThread:
 *   0x00: GSFloorResource* prev  -- linked list: previous resource
 *   0x04: GSFloorResource* next  -- linked list: next resource
 *   0x08: u32  active            -- nonzero if resource is in use
 *   0x0C: u32  status            -- 0 = base, 1 = loaded (GSFloorResStatus)
 *   0x10: u32  floorId           -- owning floor ID
 *   0x14: u8   priority          -- scheduling priority
 *   0x15: u8   pending           -- resource pending flag (cleared on load)
 *   0x16: u8   pad16
 *   0x17: u8   pad17
 *   0x18: void* callback         -- resource load callback function pointer
 *   0x1C: void* modelHandle      -- loaded model/data handle
 *   0x20: u16  textureHandle     -- GSmem handle for associated texture
 *   0x22: u16  pad22
 */
typedef struct GSFloorResource {
    /* 0x00 */ struct GSFloorResource* prev;
    /* 0x04 */ struct GSFloorResource* next;
    /* 0x08 */ s32     active;
    /* 0x0C */ s32     status;
    /* 0x10 */ u32     floorId;
    /* 0x14 */ u8      priority;
    /* 0x15 */ u8      pending;
    /* 0x16 */ u8      pad16;
    /* 0x17 */ u8      pad17;
    /* 0x18 */ void*   callback;
    /* 0x1C */ void*   modelHandle;
    /* 0x20 */ u16     textureHandle;
    /* 0x22 */ u16     pad22;
} GSFloorResource;

/**
 * GSFloorDataEntry -- speculative floor table entry layout (unverified).
 *
 * Size: 0x48 bytes (72 bytes), 0x80 entries total.
 * NOTE: lbl_80402518 (previously claimed as the backing storage for this
 * table) is actually gs_model.c's model resource table, not a floor data
 * table. No confirmed backing storage for this struct is currently known.
 * This type is currently unused; kept only as a size/layout note pending
 * real decompilation of loadArchivePtr.
 */
typedef struct GSFloorDataEntry {
    /* 0x00 */ u8   data[0x40];      /**< Floor-specific data (area config, etc.) */
    /* 0x40 */ u32  fsysFileHandle;  /**< FSYS archive file handle */
    /* 0x44 */ s32  refCount;        /**< Reference count: 0 = free, >0 = in use */
} GSFloorDataEntry;

/**
 * GSFloorResHandler -- resource type handler table entry.
 *
 * Size: 0x10 bytes (16 bytes).
 * Stored at lbl_80404918. Each entry maps a resource type byte to
 * a pair of callbacks: one to compute the size of the resource,
 * and one to read/load it into memory.
 *
 * Fields recovered from _floorManagerThread (state 4):
 *   0x00: u8   typeId        -- resource type identifier (matched against header byte)
 *   0x01-0x03: padding
 *   0x04: void* loadFunc     -- function to restore the resource
 *   0x08: void* saveFunc     -- function to snapshot the resource
 *   0x0C: void* sizeFunc     -- function to compute the resource size
 */
typedef struct GSFloorResHandler {
    /* 0x00 */ u8    typeId;
    /* 0x01 */ u8    pad01[3];
    /* 0x04 */ void* loadFunc;
    /* 0x08 */ void* saveFunc;
    /* 0x0C */ void* sizeFunc;
} GSFloorResHandler;

/* ===================================================================
 * Public API
 *
 * NOTE: a prior recovery pass invented a full named API here
 * (GSfloorInit, GSfloorOpen, GSfloorSetFloorTable, GSfloorThreadMain,
 * GSfloorUpdate, GSfloorLoadParticle, GSfloorFindAndOpen, GSfloorLoadData,
 * GSfloorLoadMain, GSfloorGetCurrentId, GSfloorGetContext) with fabricated
 * bodies in gs_floor.c. None of those names appear in
 * config/GC6E01/symbols.txt and nothing else in the tree referenced them;
 * the prototypes and their definitions have been removed. This unit is not
 * yet meaningfully decompiled -- see gs_floor.c for the real stub scaffold.
 * The structs above remain because gs_floor_data.c genuinely uses
 * GSFloorResource / GSFloorResHandler.
 * =================================================================== */

u32 floorGetCurrentContext(void);

#endif /* GS_FLOOR_H */
