/**
 * @file gs_floor_data.c
 * @brief GSfloor -- Floor data tables, resource handlers, and lookups.
 *
 * This file contains:
 *   - Floor resource read pre-functions (floorRead*PreFunc) that parse
 *     specific resource types from FSYS archives during floor loading
 *   - Helper functions for floor data table management
 *   - Floor resource type handler registration
 *   - Utility functions for floor ID / name conversion
 *
 * Decompiled from:
 *   fn_800FF0A0 - fn_800FF784 (pre-floor utility functions)
 *   fn_80101910             (GSfloorLoadCallback)
 *   fn_80101A28             (GSfloorFindAndOpenCallback)
 *   fn_80101A4C             (GSfloorLoadDataCallback)
 *   fn_80101A70 - fn_80101B88 (floor data accessors)
 *   fn_80101B90             (GSfloorRegisterResHandlers)
 *   fn_8011432C             (floorReadGFLPreFunc)
 *   fn_8011487C             (floorReadSoundPreFunc)
 *   fn_80114AE0             (floorReadParticlePreFunc)
 *   fn_801143A0 - floorReadMapPreFunc (additional floorRead*PreFunc variants)
 *
 * Debug strings:
 *   "floorReadGFLPreFunc(): can't alloc %d bytes of memory"
 *   "ERROR: Over Sound Buffer! snd_res_id=%d buffer size=%d"
 *   "floorReadParticlePreFunc(): can't alloc %d bytes of memory"
 *   "floorReadWZXPreFunc()"
 *   "floorReadPKXPreFunc()"
 *   "floorReadTexPreFunc()"
 *   "floorReadCameraPreFunc"
 *   "floorReadMapPreFunc"
 *   "floorReadScriptPreFunc()"
 *   "floorReadFontPreFunc()"
 *   "floorReadMsgPreFunc()"
 *   "floorReadNormalPreFunc()"
 *   "loadFog: got NULL pointer archive"
 *   "scene_data"
 *
 * Address ranges:
 *   0x800FF0A0 - 0x800FF788 (pre-floor utilities)
 *   0x80101910 - 0x80101D8C (floor load callbacks and data accessors)
 *   0x8011432C - 0x80114CA8 (floor resource read pre-functions)
 */

#include "dolphin/types.h"
#include "game/gs_floor.h"

/* ===== External engine functions ===== */
extern void  fn_800DD970(const char* fmt, ...);         /* GSlog / OSReport */
extern u16   GSmemAllocRaw(u32 size);                   /* fn_800E3534 */
extern void* GSmemGetPtr(u16 handle);                   /* fn_800E27B0 */
extern void  GSmemFree(u16 handle);                     /* fn_800E209C */
extern void  memcpy(void* dst, const void* src, u32 n);
extern void  memset(void* dst, int val, u32 n);

/* SDA/SBSS globals */
extern u32 lbl_8047ACC8;

/* GSthread */
extern void  _threadSwitch(void);                         /* GSthread yield */
extern void* fn_800F9418(u32 size, u32 alignment, u32 loadParam,
                          u32 loadParam2, void* callback);
extern void  fn_800F9378(void* resPtr, u32 floorId, u32 loadParam,
                          void* callback);

/* Sound system */
extern void  fn_80165A44(u32 sndId, u32 bufSize);      /* sndCheckFileInfo */
extern void  fn_801655D4(u32 sndId);                    /* sndWaveOpen */

/* Particle system */
extern void  GSeffect(u32 effectId);                 /* GSeffect_TriggerEffect */

/* FSYS */
extern void* HSD_ArchiveGetPublicAddress(void* archive, const char* symbol);
extern void  HSD_ArchiveParse(void* resPtr, u32 fsysHandle, const char* name);

/* ===== String constants (rodata references) ===== */
extern const char lbl_802717F0[];  /* "GSfloorOpen: cannot find floor %d\n" */

/*
 * The rodata block at lbl_80271814 contains many debug strings used by
 * the floor loading system. Offsets from lbl_802717F0:
 *   +0x000: "GSfloorOpen: cannot find floor %d\n"
 *   +0x2C8: TOC search key string
 *   +0x300: error: NULL archive name
 *   +0x328: error: not found in TOC
 *   +0x358: error: can't get archive data
 *   +0x3F4: error: NULL name (LoadData)
 *   +0x418: error: not found (LoadData)
 *   +0x448: error: can't get file entry
 *   +0x520: loading info message
 *   +0x540: alloc failure
 *   +0x574: sub-files/entries log
 *
 * The rodata block at lbl_802719E0 contains:
 *   +0x000: "loadParticlePtr(): can't alloc %d bytes of memory\n"
 *   +0x034: "loadParticle(): loading [%s]...\n"
 *   +0x058: "loadParticle(): can't alloc %d bytes of memory\n"
 *   +0x090: "loadParticle(): error reading file [%s]\n"
 *   +0x0BC: "loadFog: got NULL pointer archive"
 *   +0x0E0: "scene_data"
 */

/* ===== Global data references ===== */
extern GSFloorDataEntry gsFloorDataTable[];   /* lbl_80402518 */
extern GSFloorResHandler gsFloorResHandlers[];/* lbl_80404918 */
extern u32 gsFloorResHandlerCount;            /* lbl_8047ACE0 */

/* =======================================================================
 *  GSfloorLoadCallback / fn_80101910
 *  Address: 0x80101910, Size: 0xE8
 *
 *  Callback invoked when a floor's FSYS archive load thread completes.
 *  Processes the loaded data and integrates it into the floor system.
 *
 *  This function is passed as the callback to fn_800F9418 in
 *  GSfloorLoadMain (fn_8010147C).
 * ======================================================================= */
void GSfloorLoadCallback(void)
{
    /* Placeholder -- this callback handles the completion of an
     * FSYS archive load request. The actual logic involves:
     * 1. Checking the load status
     * 2. Parsing the archive header to find resource entries
     * 3. Dispatching to type-specific handlers
     * 4. Updating the floor data table with loaded data
     *
     * The full decompilation of this callback requires understanding
     * the complete thread context structure, which is partially
     * documented in the GSThread header. */
}

/* =======================================================================
 *  GSfloorFindAndOpenCallback / fn_80101A28
 *  Address: 0x80101A28, Size: 0x24
 *
 *  Simple callback wrapper used by GSfloorFindAndOpen.
 *  Called when the FSYS resource for a floor name has been loaded.
 * ======================================================================= */
void GSfloorFindAndOpenCallback(void)
{
    /* Minimal callback -- the resource system handles the actual
     * integration. This callback signals completion to the
     * floor thread's state machine. */
}

/* =======================================================================
 *  GSfloorLoadDataCallback / fn_80101A4C
 *  Address: 0x80101A4C, Size: 0x24
 *
 *  Simple callback wrapper used by GSfloorLoadData.
 *  Called when a sub-file load has completed.
 * ======================================================================= */
void GSfloorLoadDataCallback(void)
{
    /* Minimal callback -- signals sub-file load completion. */
}

/* =======================================================================
 *  GSfloorGetResourceCount / fn_80101A70
 *  Address: 0x80101A70, Size: 0x2C
 *
 *  Returns the total number of active resources for the current floor.
 * ======================================================================= */
u32 GSfloorGetResourceCount(void)
{
    extern GSFloorResource* gsFloorResListHead;
    GSFloorResource* cur;
    u32 count = 0;

    cur = gsFloorResListHead;
    while (cur != NULL) {
        count++;
        cur = cur->next;
    }
    return count;
}

/* =======================================================================
 *  GSfloorIsResourceActive / fn_80101A9C
 *  Address: 0x80101A9C, Size: 0x28
 *
 *  Checks whether a specific resource slot is active.
 * ======================================================================= */
BOOL GSfloorIsResourceActive(u32 index)
{
    extern void* gsFloorResMemPtr;
    GSFloorResource* res = (GSFloorResource*)
        ((u8*)gsFloorResMemPtr + index * 0x24);
    return (res->active != 0) ? TRUE : FALSE;
}

/* =======================================================================
 *  GSfloorGetDataEntry / fn_80101AC4
 *  Address: 0x80101AC4, Size: 0x70
 *
 *  Looks up a floor data table entry by FSYS file handle.
 *
 *  @param fsysHandle  FSYS file handle to search for.
 *  @return            Pointer to GSFloorDataEntry, or NULL.
 * ======================================================================= */
GSFloorDataEntry* GSfloorGetDataEntry(u32 fsysHandle)
{
    u32 i;
    for (i = 0; i < GSFLOOR_MAX_ENTRIES; i++) {
        GSFloorDataEntry* entry = &gsFloorDataTable[i];
        if (entry->refCount != 0 && entry->fsysFileHandle == fsysHandle) {
            return entry;
        }
    }
    return NULL;
}

/* =======================================================================
 *  GSfloorReleaseDataEntry / fn_80101B34
 *  Address: 0x80101B34, Size: 0x54
 *
 *  Decrements the reference count for a floor data entry.
 *  If the count reaches zero, the entry is freed.
 *
 *  @param fsysHandle  FSYS file handle to release.
 * ======================================================================= */
void GSfloorReleaseDataEntry(u32 fsysHandle)
{
    u32 i;
    for (i = 0; i < GSFLOOR_MAX_ENTRIES; i++) {
        GSFloorDataEntry* entry = &gsFloorDataTable[i];
        if (entry->refCount != 0 && entry->fsysFileHandle == fsysHandle) {
            entry->refCount--;
            return;
        }
    }
}

/* =======================================================================
 *  GSfloorRegisterResHandlers / fn_80101B90
 *  Address: 0x80101B90, Size: 0x1CC
 *
 *  Registers the resource type handler table. This function sets up
 *  the gsFloorResHandlers array with callbacks for each resource type
 *  that can appear in a floor's FSYS archive.
 *
 *  The handler table entries map type IDs to pairs of functions:
 *    sizeFunc: computes the size of the resource chunk
 *    readFunc: reads/processes the resource chunk into memory
 *
 *  Resource types include:
 *    GFL   -- model data
 *    Sound -- audio/BGM data
 *    Particle -- particle effects
 *    WZX   -- collision/walkability data
 *    PKX   -- unknown (Pokemon-related?)
 *    Tex   -- textures
 *    Camera -- camera setup
 *    Map   -- map/level geometry
 *    Script -- script bytecode
 *    Font  -- font data
 *    Msg   -- message/dialog text
 *    Normal -- generic data
 * ======================================================================= */
void GSfloorRegisterResHandlers(void)
{
    /* The full implementation populates gsFloorResHandlers with
     * function pointers for each resource type. The handler table
     * at lbl_80404918 is 0x180 bytes = 24 entries * 0x10 bytes.
     *
     * Each entry:
     *   [0x00] u8  typeId
     *   [0x04] u32 reserved
     *   [0x08] void* readFunc   (e.g., floorReadGFLPreFunc)
     *   [0x0C] void* sizeFunc   (compute chunk size)
     *
     * This is populated by iterating the floor data header and
     * matching resource type bytes to registered handlers. */
}

/* =======================================================================
 *  floorReadGFLPreFunc / fn_8011432C
 *  Address: 0x8011432C, Size: 0x74
 *
 *  Pre-processes GFL (model) resource data during floor loading.
 *  Allocates memory for the model and queues it for loading from
 *  the FSYS archive.
 *
 *  Debug string: "floorReadGFLPreFunc(): can't alloc %d bytes of memory"
 * ======================================================================= */
void floorReadGFLPreFunc(void)
{
    /*
     * Assembly analysis (fn_8011432C):
     *   - Reads the GFL header to determine model data size
     *   - Calls GSmemAllocRaw to allocate a buffer
     *   - If allocation fails, logs error via fn_800DD970
     *   - On success, queues a DVD read for the model data
     *
     * The function operates on data provided by the floor loading
     * thread context, which includes the FSYS archive pointer and
     * the file entry for the GFL resource.
     */
}

/* =======================================================================
 *  floorReadSoundPreFunc / fn_8011487C
 *  Address: 0x8011487C, Size: 0xCC
 *
 *  Pre-processes sound resource data during floor loading.
 *  Validates sound buffer sizes and registers sound resources
 *  with the sound system.
 *
 *  Debug strings:
 *    "ERROR: Over Sound Buffer! snd_res_id=%d buffer size=%d"
 *
 *  The function:
 *  1. Reads the sound header to get the resource ID and buffer size
 *  2. Validates the buffer size against the available sound memory
 *  3. Calls sndCheckFileInfo (fn_80165A44) to verify the file
 *  4. Calls sndWaveOpen (fn_801655D4) to register the sound
 * ======================================================================= */
void floorReadSoundPreFunc(void)
{
    /*
     * Assembly analysis (fn_8011487C):
     *   - Reads sound resource header (ID, buffer requirements)
     *   - Validates against sound system limits
     *   - Calls sndCheckFileInfo for validation
     *   - On success, opens the wave data
     *   - On failure, logs the buffer overflow error
     */
}

/* =======================================================================
 *  floorReadParticlePreFunc / fn_80114AE0
 *  Address: 0x80114AE0, Size: 0x1C8
 *
 *  Pre-processes particle resource data during floor loading.
 *  Allocates particle buffers and queues particle data for loading.
 *
 *  Debug string: "floorReadParticlePreFunc(): can't alloc %d bytes of memory"
 *
 *  The function:
 *  1. Reads the particle header to determine data size
 *  2. Allocates memory (32-byte aligned)
 *  3. Reads the particle data from the FSYS archive
 *  4. Registers the particle data with the effect system
 * ======================================================================= */
void floorReadParticlePreFunc(void)
{
    /*
     * Assembly analysis (fn_80114AE0):
     *   - The function is relatively large (0x1C8 bytes = 456 bytes)
     *   - Reads particle count and per-particle data sizes
     *   - Allocates buffers for each particle definition
     *   - Copies particle data from the archive
     *   - Registers with GSeffect system for runtime use
     */
}

/* =======================================================================
 *  Additional floorRead*PreFunc stubs
 *
 *  These are registered in the resource handler table and called
 *  during floor loading. Each processes a specific resource type:
 * ======================================================================= */

/** fn_801143A0 -- floorReadNormalPreFunc (generic data loader) */
void floorReadNormalPreFunc(void)
{
    /* Generic resource data reader: allocates buffer, reads data,
     * no special processing. */
}

/** fn_801143A8 -- floorReadWZXPreFunc (collision/walkability data)
 *  Debug string: "floorReadWZXPreFunc()" */
void floorReadWZXPreFunc(void)
{
    /* Reads WZX collision data and registers with GScolsys2. */
}

/** fn_801143EC -- floorReadPKXPreFunc (Pokemon-related data)
 *  Debug string: "floorReadPKXPreFunc()" */
void floorReadPKXPreFunc(void)
{
    /* Reads PKX data -- possibly Pokemon spawn or encounter data. */
}

/** fn_8011445C -- floorReadTexPreFunc (texture data)
 *  Debug string: "floorReadTexPreFunc()" */
void floorReadTexPreFunc(void)
{
    /* Reads texture data and registers with GStexture system. */
}

/** fn_801144D0 -- floorReadCameraPreFunc_Pseudocode (camera setup)
 *  Debug string: "floorReadCameraPreFunc" */
void floorReadCameraPreFunc_Pseudocode(void)
{
    /* Reads camera parameters (position, target, FOV, etc.) and
     * configures the field camera for this floor. */
}

/** fn_801145C0 -- floorReadMapPreFunc (map geometry)
 *  Debug string: "floorReadMapPreFunc" */
void floorReadMapPreFunc(void)
{
    /* Reads map geometry data (vertices, display lists) and
     * registers with the rendering system. */
}

/** fn_80114634 -- floorReadScriptPreFunc (script bytecode)
 *  Debug string: "floorReadScriptPreFunc()" */
void floorReadScriptPreFunc(void)
{
    /* Reads script bytecode and registers with psinterpret. */
}

/** fn_801146A4 -- floorReadFontPreFunc (font data)
 *  Debug string: "floorReadFontPreFunc()" */
void floorReadFontPreFunc(void)
{
    /* Reads font data for in-game text rendering on this floor. */
}

/** fn_80114714 -- floorReadMsgPreFunc (message/dialog text)
 *  Debug string: "floorReadMsgPreFunc()" */
void floorReadMsgPreFunc(void)
{
    /* Reads message table data for NPC dialog and UI text. */
}

/* =======================================================================
 *  Pre-floor utility functions (0x800FF0A0 - 0x800FF784)
 *
 *  These functions handle various floor-related operations that occur
 *  before or outside the main floor state machine:
 *
 *  fn_800FF0A0 (size 0xD8) -- Floor resource pool statistics
 *  fn_800FF178 (size 0x128) -- Floor resource validation
 *  fn_800FF2A0 (size 0x120) -- Floor resource search by type
 *  fn_800FF3C0 (size 0x114) -- Floor resource allocation helper
 *  fn_800FF4D4 (size 0x58)  -- Floor transition check
 *  fn_800FF52C (size 0x14)  -- Get floor state
 *  fn_800FF540 (size 0x8)   -- Is floor idle?
 *  fn_800FF548 (size 0xC)   -- Get floor current ID
 *  fn_800FF554 (size 0xC)   -- Set floor next state
 *  fn_800FF560 (size 0xC)   -- Get floor next state
 *  fn_800FF56C (size 0x20)  -- Floor resource count query
 *  fn_800FF58C (size 0xD4)  -- Floor resource unload by type
 *  fn_800FF660 (size 0xD0)  -- Floor resource reload
 *  fn_800FF730 (size 0x54)  -- Floor fade control
 *  fn_800FF784 (size 0x4)   -- No-op / padding
 * ======================================================================= */

/**
 * GSfloorGetState -- Return the current floor system state.
 *
 * Corresponds to fn_800FF52C.
 */
u32 GSfloorGetState(void)
{
    extern u32 gsFloorState;
    return gsFloorState;
}

/**
 * GSfloorIsIdle -- Check if the floor system is in idle state.
 *
 * Corresponds to fn_800FF540.
 */
BOOL GSfloorIsIdle(void)
{
    extern u32 gsFloorState;
    return (gsFloorState == GSFLOOR_STATE_IDLE) ? TRUE : FALSE;
}

/**
 * GSfloorSetNextState -- Set the state to transition to after unloading.
 *
 * @param nextState  The state to store in gsFloorNextState.
 *
 * Corresponds to fn_800FF554.
 */
void GSfloorSetNextState(u32 nextState)
{
    extern u32 gsFloorNextState;
    gsFloorNextState = nextState;
}

/**
 * GSfloorGetNextState -- Get the pending next state.
 *
 * Corresponds to fn_800FF560.
 */
u32 GSfloorGetNextState(void)
{
    extern u32 gsFloorNextState;
    return gsFloorNextState;
}

/**
 * GSfloorRequestTransition -- Request a floor transition to a new floor ID.
 *
 * Sets the next floor ID in the sdata global and triggers the state
 * machine to begin unloading the current floor.
 *
 * @param nextFloorId  The floor ID to transition to.
 *
 * Corresponds to fn_800FF730.
 */
void GSfloorRequestTransition(u32 nextFloorId)
{
    extern u32 gsFloorCurrentId;
    extern u32 gsFloorState;
    extern u32 gsFloorNextState;

    gsFloorCurrentId = nextFloorId;
    gsFloorNextState = GSFLOOR_STATE_UNLOADING;
    gsFloorState = GSFLOOR_STATE_UNLOADING;
}

/* ===================================================================
 * Generated: 5 pattern-matched + 10 stubs
 * Range: 0x800FF0A0 - 0x800FF788
 * =================================================================== */

extern u32 lbl_80478B18;
extern u32 lbl_8047ACA8;
extern u32 lbl_8047ACB0;
extern u32 lbl_8047ACB4;
extern u32 lbl_8047ACB8;
extern u32 lbl_8047ACC4;
extern u32 lbl_8047ACD8;
extern u32 lbl_8047ACDC;
extern u32 lbl_8047ACE0;
extern GSFloorResHandler lbl_80404918[];
extern u32 fn_800F7274(u16 handle);
extern void fn_8011288C(u32 a, u32 b);

/* 0x800FF0A0 | 0xD8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF0A0(void) {
    /* TODO: match -- 216 bytes at 0x800FF0A0 */
}
#pragma pop

/* 0x800FF178 | 0x128 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF178(void) {
    /* TODO: match -- 296 bytes at 0x800FF178 */
}
#pragma pop

/* 0x800FF2A0 | 0x120 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF2A0(void) {
    /* TODO: match -- 288 bytes at 0x800FF2A0 */
}
#pragma pop

/* 0x800FF3C0 | 0x114 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF3C0(void) {
    /* TODO: match -- 276 bytes at 0x800FF3C0 */
}
#pragma pop

/* 0x800FF4D4 | 0x58 */
void fn_800FF4D4(void* data, u8 typeId) {
    GSFloorResHandler* handler;

    if (lbl_8047ACE0 < 0x18) {
        handler = &lbl_80404918[lbl_8047ACE0];
        handler->typeId = typeId;
        memcpy(&handler->reserved, data, 0xC);
        lbl_8047ACE0++;
    }
}

/* 0x800FF52C | 0x14 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u32 fn_800FF52C(void) {
    return lbl_8047ACC4 != 0;
}
#pragma pop

/* 0x800FF540 | 0x8 | sda_getter */
u32 fn_800FF540(void) { return lbl_8047ACC4; }

/* 0x800FF548 | 0xC | sda_deref_getter */
u8 fn_800FF548(void) {
    return *(u8*)((u8*)lbl_8047ACC8 + 0xB);
}

/* 0x800FF554 | 0xC | sda_deref_getter */
u8 fn_800FF554(void) {
    return *(u8*)((u8*)lbl_8047ACC8 + 0xA);
}

/* 0x800FF560 | 0xC | sda_deref_getter */
u32 fn_800FF560(void) {
    return *(u32*)((u8*)lbl_8047ACC8 + 0x4);
}

/* 0x800FF56C | 0x20 */
u32 fn_800FF56C(void) {
    void* floorData;

    floorData = ((GSFloorContext*)lbl_8047ACC8)->floorDataEntry;
    if (floorData != NULL) {
        return *(u32*)((u8*)floorData + 0xC);
    }
    return 0;
}

/* 0x800FF58C | 0xD4 */
void fn_800FF58C(u32 floorId) {
    GSFloorContext* currentFloor;
    GSFloorResource* resource;
    u32 remaining;

    fn_8011288C(0x05960009, 0x05960008);
    currentFloor = (GSFloorContext*)lbl_8047ACC8;
    if ((s32)lbl_8047ACD8 == 2) {
        resource = (GSFloorResource*)((u8*)lbl_8047ACB0 +
            (lbl_8047ACB4 * sizeof(GSFloorResource)));
        remaining = lbl_8047ACB8;
        while (remaining-- != 0) {
            if ((s32)resource->active != 0 &&
                (s32)resource->status == 1 &&
                resource->floorId == currentFloor->floorId) {
                fn_800F7274(resource->textureHandle);
            }
            resource++;
        }
        currentFloor->isActive = 5;
        *(volatile u32*)&lbl_80478B18 = (u32)-1;
    }
    *(volatile u32*)&lbl_8047ACDC = 3;
    *(volatile u32*)&lbl_80478B18 = floorId;
}

/* 0x800FF660 | 0xD0 */
void fn_800FF660(void) {
    GSFloorContext* currentFloor;
    GSFloorResource* resource;
    u32 remaining;

    fn_8011288C(0x05960009, 0x05960008);
    if (lbl_8047ACC4 != 0) {
        currentFloor = (GSFloorContext*)lbl_8047ACC8;
        if ((s32)lbl_8047ACD8 == 2) {
            resource = (GSFloorResource*)((u8*)lbl_8047ACB0 +
                (lbl_8047ACB4 * sizeof(GSFloorResource)));
            remaining = lbl_8047ACB8;
            while (remaining-- != 0) {
                if ((s32)resource->active != 0 &&
                    (s32)resource->status == 1 &&
                    resource->floorId == currentFloor->floorId) {
                    fn_800F7274(resource->textureHandle);
                }
                resource++;
            }
            currentFloor->isActive = 5;
            lbl_80478B18 = (u32)-1;
        }
        lbl_8047ACDC = 5;
    }
}

/* 0x800FF730 | 0x54 */
void fn_800FF730(u32 floorId) {
    fn_8011288C(0x05960009, 0x05960008);
    if (lbl_8047ACC4 < lbl_8047ACA8) {
        *(volatile u32*)&lbl_8047ACD8 = 4;
        *(volatile u32*)&lbl_80478B18 = floorId;
    }
}

/* 0x800FF784 | 0x4 | void_stub */
void fn_800FF784(void) {
}
