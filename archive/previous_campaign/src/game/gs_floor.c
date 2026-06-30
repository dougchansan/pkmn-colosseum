/**
 * @file gs_floor.c
 * @brief GSfloor -- Floor/scene management (init, load, unload, transition).
 *
 * Decompiled from:
 *   fn_800FF788 (GSfloorOpen)
 *   fn_800FF81C (GSfloorSetFloorTable)
 *   fn_800FF828 (GSfloorInit)
 *   fn_800FF970 (GSfloorThreadMain)
 *   fn_80100B24 (GSfloorUpdate)
 *   fn_80101244 (GSfloorLoadParticle)
 *   fn_801012E8 (GSfloorFindAndOpen)
 *   fn_801013A0 (GSfloorLoadData)
 *   fn_8010147C (GSfloorLoadMain)
 *   fn_80101910 (GSfloorLoadCallback -- internal)
 *
 * Debug strings:
 *   "GSfloorOpen: cannot find floor %d"
 *   "loadParticle(): loading..."
 *   "loadBGM(): loading [%s]..."
 *   "_floorGetFilesize(): can't open file [%s]"
 *   "_floorLoadFile(): can't open file [%s]"
 *   "_floorLoadFile(): error reading file [%s]"
 *
 * The floor system creates a cooperative GSthread that runs a state machine.
 * State transitions:
 *   STATE_IDLE (0)          -- yield until GSfloorOpen sets state to 1
 *   STATE_LOADING (1)       -- unload old resources, load new floor data,
 *                              allocate resource chunks, yield
 *   STATE_RUNNING (2)       -- per-frame update, dispatch resource callbacks,
 *                              check model/texture load completion
 *   STATE_UNLOADING (3)     -- clean up current floor, free resources
 *   STATE_TRANSITIONING (4) -- re-parse floor data for next floor, set up
 *                              the resource handlers, call subsystem inits
 *   STATE_FINALIZING (5)    -- wait for all pending resources, finalize
 *
 * Address range: 0x800FF788 - 0x80101910
 */

#include "dolphin/types.h"
#include "game/gs_floor.h"

/* ===== External engine / SDK functions ===== */
extern void  fn_800DD970(const char* fmt, ...);         /* GSlog / OSReport */
extern u16   GSmemAllocRaw(u32 size);                   /* fn_800E3534 */
extern void* GSmemGetPtr(u16 handle);                   /* fn_800E27B0 */
extern void* GSmemLock(u16 handle);                     /* fn_800E24B0 */
extern void  GSmemFree(u16 handle);                     /* fn_800E209C */
extern void  memcpy(void* dst, const void* src, u32 n);

/* GSthread */
extern void* GSthreadCreate(u32 affinity, u32 priority, u32 stackSize,
                             u32 usesFPU, u32 autoStart, void* entryFunc);
extern void  _threadSwitch(void);                         /* GSthread yield */
extern void  fn_800F0438(void* modelHandle);            /* GSthread resume model */
extern void  fn_800F0424(void* modelHandle);            /* GSthread pause model */
extern void  fn_800F04C4(u32 floorId);                  /* GSthread flush floor */
extern void* fn_800F716C(u32 floorId);                  /* GStexture load for floor */
extern u16   fn_800F7274(u16 handle);                   /* GStexture free */
extern void* fn_800F7108(u16 handle);                   /* GStexture get pointer */
extern u32   fn_800F7318(u8 priority, void* callback,
                          u32 stackSize, u32 p3, u32 p4,
                          u32 p5, u32 p6, u32 p7, u32 p8);
                                                         /* GStexture create resource */

/* Resource loading */
extern void  fn_800F9378(void* resPtr, u32 floorId, u32 loadParam,
                          void* callback);               /* GSres load file */
extern void* fn_800F9418(u32 size, u32 alignment, u32 loadParam,
                          u32 loadParam2, void* callback); /* GSres alloc+load */
extern void* fn_800F04BC(void* handle);                  /* Model check loaded */
extern void  fn_800F0494(void* handle);                  /* Model free */

/* Floor subsystem inits */
extern void  fn_801123D4(u32 floorDataEntry, u32 loadMode); /* Floor resource init */
extern s32   fn_80112380(u32 floorId);                   /* Floor check ready */
extern void  fn_8011274C(void);                          /* Floor finalize load */
extern void  fn_80112780(void);                          /* Floor begin transition */
extern void  fn_80117C84(void);                          /* Floor camera update */
extern void  fn_80115A38(u32 floorDataEntry);            /* Floor res set active */
extern u32   fn_80115A80(u32 floorDataEntry);            /* Floor res set inactive */
extern void  fn_8010D064(void);                          /* Collision init */
extern void  fn_8010CC54(void);                          /* Collision finalize */
extern void  fn_8010CD6C(void);                          /* Collision cleanup */
extern void  fn_801024C0(void);                          /* Floor post-load hook */
extern void  fn_800D2B90(u32 param);                     /* GS renderer set floor */

/* Script / NPC / Sound / VFX */
extern void  fn_800F915C(void);                          /* Particle cleanup */
extern void  fn_80169DF8(void);                          /* Script cleanup */
extern void  fn_80175B94(void);                          /* Generator cleanup */
extern void  fn_8016AAAC(void);                          /* Script system reset */
extern void  fn_800E8EFC(void);                          /* Material cleanup */
extern void  fn_8018DB04(u32 param);                     /* People system notify */

/* Floor loading callbacks (forward declarations) */
extern void  fn_80101910(void);                          /* GSfloorLoadCallback */
extern void  fn_80101A28(void);                          /* GSfloorFindAndOpenCallback */
extern void  fn_80101A4C(void);                          /* GSfloorLoadDataCallback */

/* GFL model processing */
extern void* fn_800E4D18(u32 handle);

/* FSYS / Archive */
extern void* HSD_ArchiveGetPublicAddress(void* archive, const char* symbol);
extern void  HSD_ArchiveParse(void* resPtr, u32 fsysHandle, const char* name); /* FSYS bind */
extern void* fn_800D27FC(u32 handle);                    /* FSYS get archive ptr */

/* Floor resource loading callbacks */
extern void  fn_8017B3E4(void);                          /* FSYS load wait begin */
extern s32   fn_8017B2CC(void);                          /* FSYS load wait check */
extern void  fn_8017B1CC(void);                          /* FSYS unload archive */

/* ===== String constants (rodata references) ===== */
extern const char lbl_802717F0[];  /* "GSfloorOpen: cannot find floor %d\n" */
extern const char lbl_80271814[];  /* Multi-line error/info block (Japanese + English) */
extern const char lbl_802719C4[];  /* "loadParticle(): loading...\n" */
extern const char lbl_802719E0[];  /* "loadParticlePtr(): can't alloc %d bytes of memory\n" */

/* ===== Global state (sbss / sdata) ===== */

/* --- Floor system configuration --- */
static u16            gsFloorMemHandle;      /* lbl_8047ACA0 : GSmem handle for alloc table */
static void*          gsFloorMemPtr;         /* lbl_8047ACA4 : resolved pointer */
static u32            gsFloorMaxFloors;      /* lbl_8047ACA8 : max floor index count */
static u16            gsFloorResMemHandle;   /* lbl_8047ACAC : GSmem handle for resource array */
static void*          gsFloorResMemPtr;      /* lbl_8047ACB0 : resolved resource array pointer */

/* --- Resource pool configuration --- */
static u32            gsFloorMaxBase;        /* lbl_8047ACB4 : number of base resource slots */
static u32            gsFloorMaxExt1;        /* lbl_8047ACB8 : number of ext pool 1 slots */
static u32            gsFloorMaxExt2;        /* lbl_8047ACBC : number of ext pool 2 slots */
static u32            gsFloorTotalRes;       /* lbl_8047ACC0 : total = base + ext1 + ext2 */

/* --- Runtime state --- */
static u32            gsFloorUnused;         /* lbl_8047ACC4 : unused/padding */
static GSFloorContext* gsFloorCurrent;       /* lbl_8047ACC8 : pointer to current floor context */
static GSFloorResource* gsFloorResListHead;  /* lbl_8047ACCC : head of active resource list */
static void*          gsFloorTablePtr;       /* lbl_8047ACD0 : pointer to floor data table */
static u32            gsFloorTableCount;     /* lbl_8047ACD4 : number of entries in table */
static u32            gsFloorState;          /* lbl_8047ACD8 : current state machine phase */
static u32            gsFloorNextState;      /* lbl_8047ACDC : next state to transition to */
static u32            gsFloorResHandlerCount;/* lbl_8047ACE0 : number of resource type handlers */

/* --- Current floor ID (sdata, initialized to -1) --- */
static u32            gsFloorCurrentId;      /* lbl_80478B18 : current floor identifier */

/* --- Static tables (bss) --- */
static GSFloorDataEntry gsFloorDataTable[GSFLOOR_MAX_ENTRIES]; /* lbl_80402518 */
static GSFloorResHandler gsFloorResHandlers[GSFLOOR_MAX_RES_TYPES]; /* lbl_80404918 */

/* =======================================================================
 *  GSfloorOpen / fn_800FF788
 *  Address: 0x800FF788, Size: 0x94
 *
 *  Opens a floor by ID. Searches the floor data table for a matching
 *  entry. If found, stores it as the pending floor in the current
 *  context and sets the state to LOADING.
 *
 *  r3 = floorId
 *
 *  The floor data table (gsFloorTablePtr) is an array of 0x4C-byte
 *  entries. Field at offset 0x0C stores the floor identifier. The
 *  search iterates gsFloorTableCount entries.
 * ======================================================================= */
void GSfloorOpen(u32 floorId)
{
    GSFloorContext* ctx;
    u32 i;
    void* entry;

    /* Already loading? Bail out */
    if (gsFloorState != GSFLOOR_STATE_IDLE)
        return;

    /* Search the floor data table for this floor ID */
    entry = gsFloorTablePtr;
    if (gsFloorTableCount != 0) {
        for (i = 0; i < gsFloorTableCount; i++) {
            u32* entryBase = (u32*)((u8*)entry + i * 0x4C);
            if (entryBase[3] == floorId) { /* offset 0x0C = floor ID */
                goto found;
            }
        }
    }
    entry = NULL;

found:
    if (entry == NULL) {
        fn_800DD970(lbl_802717F0, floorId);
        return;
    }

    ctx = gsFloorCurrent;
    ctx->floorDataEntry = entry;
    ctx->floorId = floorId + GSFLOOR_ID_BASE;
    ctx->isActive = 1;
    gsFloorState = GSFLOOR_STATE_LOADING;
}

/* =======================================================================
 *  GSfloorSetFloorTable / fn_800FF81C
 *  Address: 0x800FF81C, Size: 0xC
 *
 *  Sets the floor data table pointer and entry count.
 * ======================================================================= */
void GSfloorSetFloorTable(void* tablePtr, u32 count)
{
    gsFloorTablePtr = tablePtr;
    gsFloorTableCount = count;
}

/* =======================================================================
 *  GSfloorInit / fn_800FF828
 *  Address: 0x800FF828, Size: 0x148
 *
 *  Initializes the floor system:
 *   1. Clears the floor data table pointer
 *   2. Allocates the floor context pool from GSmem (maxFloors * 0x14 bytes)
 *   3. Allocates the resource array from GSmem (totalRes * 0x24 bytes)
 *   4. Initializes all resource slots to empty
 *   5. Creates the floor management thread (GSthreadCreate)
 *
 *  r3 = maxFloors, r4 = maxBase, r5 = maxExt1, r6 = maxExt2
 * ======================================================================= */
void GSfloorInit(u32 maxFloors, u32 maxBase, u32 maxExt1, u32 maxExt2)
{
    u32 totalRes;
    u32 allocSize;
    u32 i;

    /* Reset floor data table */
    gsFloorTablePtr = NULL;
    gsFloorTableCount = 0;

    /* Save configuration */
    gsFloorMaxFloors = maxFloors;

    /* Allocate floor context pool: maxFloors * sizeof(GSFloorContext) = maxFloors * 0x14 */
    allocSize = maxFloors * 0x14;
    gsFloorMemHandle = GSmemAllocRaw(allocSize);
    if ((gsFloorMemHandle & 0xFFFF) == 0)
        return;

    gsFloorMemPtr = GSmemGetPtr(gsFloorMemHandle);

    /* Store pool configuration */
    gsFloorMaxBase = maxBase;
    gsFloorMaxExt1 = maxExt1;
    gsFloorMaxExt2 = maxExt2;
    totalRes = maxBase + maxExt1 + maxExt2;
    gsFloorTotalRes = totalRes;

    /* Allocate resource array: totalRes * sizeof(GSFloorResource) = totalRes * 0x24 */
    allocSize = totalRes * 0x24;
    gsFloorResMemHandle = GSmemAllocRaw(allocSize);
    if ((gsFloorResMemHandle & 0xFFFF) == 0)
        return;

    gsFloorResMemPtr = GSmemGetPtr(gsFloorResMemHandle);

    /* Initialize all resource slots */
    for (i = 0; i < totalRes; i++) {
        GSFloorResource* res = (GSFloorResource*)((u8*)gsFloorResMemPtr + i * 0x24);
        res->active = 0;  /* offset 0x08 */
    }

    /* Initialize the floor context */
    gsFloorResListHead = NULL;
    gsFloorUnused = 0;

    gsFloorCurrent = (GSFloorContext*)gsFloorMemPtr;
    gsFloorCurrent->floorDataEntry = NULL;
    gsFloorCurrent->floorId = 0;
    gsFloorCurrent->resMemHandle = 0;
    gsFloorCurrent->doFadeIn = 0;
    gsFloorCurrent->doFadeOut = 0;
    gsFloorCurrent->isActive = 0;

    gsFloorState = GSFLOOR_STATE_IDLE;
    gsFloorCurrentId = GSFLOOR_ID_INVALID;

    /* Create the floor management thread:
     * affinity=0, priority=0x7D0, stackSize=0x4000,
     * usesFPU=1, autoStart=1, entryFunc=GSfloorThreadMain */
    GSthreadCreate(0, 0x7D0, 0x4000, 1, 1, (void*)GSfloorThreadMain);
}

/* =======================================================================
 *  unlinkResource -- internal helper
 *
 *  Removes a GSFloorResource from the doubly-linked active list.
 *  If the resource has a loaded texture (status==1), frees it via
 *  fn_800F7274.
 * ======================================================================= */
static void unlinkResource(GSFloorResource* res)
{
    /* If resource has a loaded texture, free it */
    if (res->status == GSFLOOR_RES_LOADED) {
        if (res->modelHandle != NULL) {
            fn_800F7274(res->textureHandle);
        }
    }

    /* Clear active flag */
    res->active = 0;

    /* Unlink from doubly-linked list */
    if (res->prev != NULL) {
        res->prev->next = res->next;
    }
    if (res->next != NULL) {
        res->next->prev = res->prev;
    }

    /* If this was the list head, advance head */
    if (gsFloorResListHead == res) {
        gsFloorResListHead = res->next;
    }

    /* Clear link pointers */
    res->prev = NULL;
    res->next = NULL;
}

/* =======================================================================
 *  unloadResourcePool -- internal helper
 *
 *  Iterates a pool of GSFloorResource entries and unlinks all matching
 *  the given floorId with the specified status filter.
 * ======================================================================= */
static void unloadResourcePool(GSFloorResource* pool, u32 count,
                                u32 floorId, u32 statusFilter)
{
    u32 i;
    for (i = 0; i < count; i++) {
        GSFloorResource* res = &pool[i];
        if (res->status == statusFilter && res->floorId == floorId) {
            unlinkResource(res);
        }
    }
}

/* =======================================================================
 *  markResourcePending -- internal helper
 *
 *  Iterates a pool of GSFloorResource entries and marks matching
 *  resources as pending (setting the pending byte at offset 0x15).
 *  Also resumes model handles if status == LOADED.
 * ======================================================================= */
static void markResourcePending(GSFloorResource* pool, u32 count,
                                 u32 floorId, u32 statusFilter)
{
    u32 i;
    for (i = 0; i < count; i++) {
        GSFloorResource* res = &pool[i];
        if (res->status == statusFilter && res->floorId == floorId) {
            res->pending = 1;
            if (res->status == GSFLOOR_RES_LOADED) {
                if (res->modelHandle != NULL) {
                    fn_800F0438(res->modelHandle);
                }
            }
        }
    }
}

/* =======================================================================
 *  GSfloorThreadMain / fn_800FF970
 *  Address: 0x800FF970, Size: 0x11B4
 *
 *  Main thread entry for the floor state machine. This is a cooperative
 *  thread that never returns -- it loops through states and yields
 *  between frames via _threadSwitch.
 *
 *  State machine:
 *    0 (IDLE):          Yield. No floor active.
 *    1 (LOADING):       Check doFadeIn / doFadeOut flags to determine
 *                       load mode (0=fresh, 1=fade-in only, 2=full).
 *                       Call floor resource init, collision init,
 *                       FSYS load wait, then transition to RUNNING.
 *    2 (RUNNING):       Call GSfloorUpdate. If still loading, yield.
 *                       When done, read nextState and transition.
 *    3 (UNLOADING):     Free all resources for the departing floor
 *                       across all three resource pools (base, ext1, ext2)
 *                       and for both status types (FREE and LOADED).
 *                       Then load next floor and transition to state 4.
 *    4 (TRANSITIONING): Begin the transition. Parse the new floor's
 *                       resource chunks, allocate memory, register
 *                       resource handlers, mark resources as pending.
 *    5 (FINALIZING):    Final wait state (unused in normal flow).
 * ======================================================================= */
void GSfloorThreadMain(void)
{
    u32 loadMode;
    u32 floorDataEntry;
    s32 loadResult;
    u32 i;

    for (;;) {
        switch (gsFloorState) {

        case GSFLOOR_STATE_IDLE:
            /* No floor active -- yield and poll again */
            _threadSwitch();
            break;

        case GSFLOOR_STATE_LOADING: {
            GSFloorContext* ctx = gsFloorCurrent;

            /* Determine load mode from fade flags */
            if (ctx->doFadeIn != 0) {
                loadMode = 0;   /* Fresh load (fade in) */
            } else if (ctx->doFadeOut != 0) {
                loadMode = 1;   /* Fade out only (continuing) */
            } else {
                loadMode = 2;   /* Full load (no fade) */
            }

            floorDataEntry = (u32)ctx->floorDataEntry;

            /* Initialize floor resources for this load mode */
            fn_801123D4(floorDataEntry, loadMode);

            /* If fresh load, initialize collision */
            if (loadMode == 0) {
                fn_8010D064();
            }

            /* If full load, finalize collision */
            if (loadMode == 2) {
                fn_8010CC54();
            }

            /* Set floor resource as active */
            fn_80115A38(floorDataEntry);

            /* Begin FSYS archive load wait */
            fn_8017B3E4();

            /* Wait for FSYS load to complete */
            for (;;) {
                fn_80115A38(floorDataEntry);
                loadResult = fn_8017B2CC();
                if (loadResult < 0) {
                    fn_800DD970(lbl_80271814);
                }
                if (loadResult == 0) {
                    break;
                }
                _threadSwitch(); /* yield */
            }

            /* Deactivate floor resource */
            fn_80115A80(floorDataEntry);

            /* Clean up collision if not fade-out mode */
            if (loadMode != 1) {
                fn_8010CD6C();
            }

            /* Yield twice (frame sync) */
            _threadSwitch();
            _threadSwitch();

            /* Transition to RUNNING state */
            gsFloorState = GSFLOOR_STATE_RUNNING;
            ctx->doFadeIn = 0;
            ctx->doFadeOut = 0;
            ctx->isActive = 1;
            break;
        }

        case GSFLOOR_STATE_RUNNING: {
            GSFloorContext* ctx = gsFloorCurrent;
            u8 result = GSfloorUpdate(ctx);
            if (result == 1) {
                _threadSwitch(); /* yield, still loading */
                break;
            }
            /* Update complete -- read next state from pending */
            gsFloorState = gsFloorNextState;
            break;
        }

        case GSFLOOR_STATE_UNLOADING: {
            GSFloorContext* ctx = gsFloorCurrent;
            GSFloorResource* resBase = (GSFloorResource*)gsFloorResMemPtr;
            u32 floorId = ctx->floorId;

            /*
             * Free all resources belonging to the departing floor.
             * Walk three pools (base, ext1, ext2) and free
             * resources with status == FREE first, then status == LOADED.
             */

            /* Pool 0 (base): free status==FREE entries */
            unloadResourcePool(resBase, gsFloorMaxBase,
                               floorId, GSFLOOR_RES_FREE);

            /* Pool 1 (ext1): free status==FREE entries */
            {
                GSFloorResource* pool1 = (GSFloorResource*)
                    ((u8*)resBase + gsFloorMaxBase * 0x24);
                unloadResourcePool(pool1, gsFloorMaxExt1,
                                   floorId, GSFLOOR_RES_FREE);
            }

            /* Pool 2 (ext2): free status==FREE entries */
            {
                u32 offset = (gsFloorMaxBase + gsFloorMaxExt1) * 0x24;
                GSFloorResource* pool2 = (GSFloorResource*)
                    ((u8*)resBase + offset);
                unloadResourcePool(pool2, gsFloorMaxExt2,
                                   floorId, GSFLOOR_RES_FREE);
            }

            /* Pool 0 (base): free status==LOADED entries */
            unloadResourcePool(resBase, gsFloorMaxBase,
                               floorId, GSFLOOR_RES_LOADED);

            /* Pool 1 (ext1): free status==LOADED entries */
            {
                GSFloorResource* pool1 = (GSFloorResource*)
                    ((u8*)resBase + gsFloorMaxBase * 0x24);
                unloadResourcePool(pool1, gsFloorMaxExt1,
                                   floorId, GSFLOOR_RES_LOADED);
            }

            /* Pool 2 (ext2): free status==LOADED entries */
            {
                u32 offset = (gsFloorMaxBase + gsFloorMaxExt1) * 0x24;
                GSFloorResource* pool2 = (GSFloorResource*)
                    ((u8*)resBase + offset);
                unloadResourcePool(pool2, gsFloorMaxExt2,
                                   floorId, GSFLOOR_RES_LOADED);
            }

            /* Check floor readiness and finalize */
            floorDataEntry = (u32)ctx->floorDataEntry;
            if (fn_80112380(gsFloorCurrentId) == 0) {
                fn_8011274C();
            }

            /* Update field camera */
            fn_80117C84();

            /* Notify people system */
            fn_8018DB04(1);

            /* Flush textures for this floor */
            {
                u32 fId = ctx->floorId;
                fn_800F716C(fId);
                fn_800F04C4(fId);
            }

            /* Post-load hook */
            fn_801024C0();

            /* Notify renderer */
            fn_800D2B90(0);

            /* Set floor res active/inactive */
            fn_80115A38(floorDataEntry);
            fn_8017B1CC();  /* Unload FSYS archive */
            fn_80115A80(floorDataEntry);

            /* Clean up subsystems */
            fn_800F915C();   /* Particle cleanup */
            fn_80169DF8();   /* Script cleanup */
            fn_80175B94();   /* Generator cleanup */
            fn_8016AAAC();   /* Script system reset */
            fn_800E8EFC();   /* Material cleanup */

            /* Clear fade flags */
            ctx->doFadeIn = 0;
            ctx->doFadeOut = 0;

            /* Look up next floor in the floor data table */
            if ((gsFloorCurrentId + 0x10000) == 0xFFFF) {
                /* Invalid / no next floor */
                gsFloorState = GSFLOOR_STATE_IDLE;
            } else {
                /* Search floor table for the next floor ID */
                u32* tblPtr = (u32*)gsFloorTablePtr;
                u32 tblCount = gsFloorTableCount;
                void* found = NULL;
                u32 j;

                for (j = 0; j < tblCount; j++) {
                    u32* entry = (u32*)((u8*)tblPtr + j * 0x4C);
                    if (entry[3] == gsFloorCurrentId) {
                        found = entry;
                        break;
                    }
                }

                ctx->floorDataEntry = found;
                ctx->floorId = gsFloorCurrentId + GSFLOOR_ID_BASE;
                gsFloorState = GSFLOOR_STATE_LOADING;
            }
            break;
        }

        case GSFLOOR_STATE_TRANSITIONING: {
            GSFloorContext* ctx = gsFloorCurrent;
            GSFloorResource* resBase;
            u32 floorId;
            u32 resTypeId;
            u32 totalSize;
            u16 memHandle;

            /* Begin transition */
            fn_80112780();

            /* Set fade-in flag */
            ctx->doFadeIn = 1;
            ctx->doFadeOut = 0;

            /* Get the floor data header to determine resource type */
            {
                u8* floorData = (u8*)ctx->floorDataEntry;
                u8 headerByte = floorData[0];
                resTypeId = (headerByte >> 1) & 0x7;  /* extract 3-bit type ID */
            }

            /* Pass 1: compute total size of all resource chunks */
            totalSize = 0;
            {
                GSFloorResHandler* handler = gsFloorResHandlers;
                u32 handlerCount = gsFloorResHandlerCount;
                u32 h;
                for (h = 0; h < handlerCount; h++) {
                    if (handler->typeId == resTypeId) {
                        /* Call size function to get chunk size */
                        u32 (*sizeFunc)(void) = (u32 (*)(void))handler->sizeFunc;
                        u32 chunkSize = sizeFunc();
                        /* Align to 4 bytes and add header */
                        totalSize += (chunkSize + 3) & ~3;
                        totalSize += 4;
                    }
                    handler++;
                }
            }

            /* Allocate memory for all resource chunks */
            memHandle = GSmemAllocRaw(totalSize);
            if ((memHandle & 0xFFFF) == 0) {
                memHandle = 0;
                goto transitionEnd;
            }

            {
                u8* memPtr = (u8*)GSmemGetPtr(memHandle);
                if (memPtr == NULL) {
                    memHandle = 0;
                    goto transitionEnd;
                }

                /* Pass 2: read each resource chunk into the allocated block */
                {
                    GSFloorResHandler* handler = gsFloorResHandlers;
                    u32 handlerCount = gsFloorResHandlerCount;
                    u32 h;
                    u8* writePtr = memPtr;
                    for (h = 0; h < handlerCount; h++) {
                        if (handler->typeId == resTypeId) {
                            u32 (*sizeFunc)(void) = (u32 (*)(void))handler->sizeFunc;
                            u32 chunkSize = sizeFunc();
                            u32 alignedSize = (chunkSize + 3) & ~3;

                            /* Store size as header */
                            *(u32*)writePtr = alignedSize;

                            /* Call read function to fill the data */
                            {
                                void (*readFunc)(void*, u32) =
                                    (void (*)(void*, u32))handler->readFunc;
                                readFunc(writePtr + 4, alignedSize);
                            }

                            writePtr += 4 + alignedSize;
                        }
                        handler++;
                    }
                }

                /* Free the allocation (data has been processed) */
                GSmemFree(memHandle);
            }

        transitionEnd:
            /* Store resource memory handle */
            ctx->resMemHandle = memHandle;

            /* Mark all base-pool resources as pending for the new floor */
            resBase = (GSFloorResource*)gsFloorResMemPtr;
            floorId = ctx->floorId;

            markResourcePending(resBase, gsFloorMaxBase,
                                floorId, GSFLOOR_RES_FREE);

            {
                GSFloorResource* pool1 = (GSFloorResource*)
                    ((u8*)resBase + gsFloorMaxBase * 0x24);
                markResourcePending(pool1, gsFloorMaxExt1,
                                    floorId, GSFLOOR_RES_FREE);
            }

            {
                u32 offset = (gsFloorMaxBase + gsFloorMaxExt1) * 0x24;
                GSFloorResource* pool2 = (GSFloorResource*)
                    ((u8*)resBase + offset);
                markResourcePending(pool2, gsFloorMaxExt2,
                                    floorId, GSFLOOR_RES_FREE);
            }

            /* Also mark LOADED resources as pending */
            markResourcePending(resBase, gsFloorMaxBase,
                                floorId, GSFLOOR_RES_LOADED);

            {
                GSFloorResource* pool1 = (GSFloorResource*)
                    ((u8*)resBase + gsFloorMaxBase * 0x24);
                markResourcePending(pool1, gsFloorMaxExt1,
                                    floorId, GSFLOOR_RES_LOADED);
            }

            {
                u32 offset = (gsFloorMaxBase + gsFloorMaxExt1) * 0x24;
                GSFloorResource* pool2 = (GSFloorResource*)
                    ((u8*)resBase + offset);
                markResourcePending(pool2, gsFloorMaxExt2,
                                    floorId, GSFLOOR_RES_LOADED);
            }

            /* Transition to LOADING state */
            gsFloorState = GSFLOOR_STATE_LOADING;
            break;
        }

        case GSFLOOR_STATE_FINALIZING:
            /* Wait for all pending operations, then return to idle */
            gsFloorState = GSFLOOR_STATE_IDLE;
            break;

        default:
            break;
        }
    }
}

/* =======================================================================
 *  GSfloorUpdate / fn_80100B24
 *  Address: 0x80100B24, Size: 0x720
 *
 *  Per-frame update for the running floor. Processes active resources
 *  in the linked list, dispatching callbacks and checking for load
 *  completion.
 *
 *  r3 = GSFloorContext* ctx
 *  Returns: 1 if still loading (caller should yield), 0 if done
 *
 *  The function operates as a sub-state machine within state 2:
 *    sub-state 1: Dispatch resource callbacks for unloaded resources.
 *                 For loaded resources, create texture handles.
 *    sub-state 2: Check all resources for load completion. If any
 *                 are still pending, return 0 (not done). When all
 *                 are complete, advance to sub-state 3.
 *    sub-state 3: Pause/unpause resources, finalize loading.
 *    sub-state 4: Wait for model load confirmation, transition.
 *    sub-state 5: Final wait, return 0 to caller.
 *    sub-state 6: Cleanup and return 1 (done).
 * ======================================================================= */
u8 GSfloorUpdate(GSFloorContext* ctx)
{
    u32 subState;
    GSFloorResource* cur;
    GSFloorResource* savedNext;
    u32 floorDataEntry;

    subState = ctx->isActive;

    switch (subState) {
    case 1: {
        /*
         * Walk the active resource list and dispatch callbacks.
         * For unloaded resources (status == FREE), call the callback.
         * For loaded resources (status == LOADED), create texture handles.
         */
        if (ctx->doFadeOut == 0) {
            floorDataEntry = (u32)ctx->floorDataEntry;
            cur = gsFloorResListHead;
            while (cur != NULL) {
                savedNext = cur->next;
                if (cur->active == 1 && cur->pending == 0) {
                    if (cur->status == GSFLOOR_RES_FREE) {
                        /* Call the resource load callback */
                        void (*cb)(u32, u32) = (void (*)(u32, u32))cur->callback;
                        cb(floorDataEntry, cur->floorId);
                    }
                    if (cur->status == GSFLOOR_RES_LOADED) {
                        /* Resource loaded: set floor res active, create texture */
                        u32 floorId2 = (u32)fn_80115A80(floorDataEntry);
                        u16 texHandle = fn_800F7318(
                            cur->priority, cur->callback,
                            0x4000, 0, 0, 4, 0, 0, 0);
                        cur->textureHandle = texHandle;
                        cur->modelHandle = fn_800F7108(texHandle);
                    }
                }
                cur = cur->next;
                if (cur == NULL) {
                    cur = savedNext;
                }
            }
        } else {
            /* Fade-out mode: simpler dispatch */
            u32 floorData2 = (u32)ctx->floorDataEntry;
            cur = gsFloorResListHead;
            while (cur != NULL) {
                savedNext = cur->next;
                if (cur->active == 1 && cur->pending == 0) {
                    if (cur->status == GSFLOOR_RES_FREE) {
                        void (*cb)(u32, u32) = (void (*)(u32, u32))cur->callback;
                        cb(floorData2, cur->floorId);
                    }
                }
                cur = cur->next;
                if (cur == NULL) {
                    cur = savedNext;
                }
            }
        }

        /* Advance sub-state to 2 */
        ctx->isActive = 2;
        break;
    }

    case 2: {
        /* Check all base-pool resources for load completion */
        GSFloorResource* resBase = (GSFloorResource*)gsFloorResMemPtr;
        u32 count = gsFloorMaxBase;
        u32 i;
        u8 allDone = 1;

        for (i = 0; i < count; i++) {
            GSFloorResource* res = &resBase[i];
            if (res->active != 0 && res->status == GSFLOOR_RES_LOADED
                && res->pending == 0) {
                if (res->modelHandle != NULL) {
                    u8 loaded = (u8)(u32)fn_800F04BC(res->modelHandle);
                    if (loaded == 0) {
                        allDone = 0;
                    }
                }
            }
        }

        if (allDone) {
            /* All resources loaded: free model handles for base pool */
            GSFloorResource* resBase2 = (GSFloorResource*)gsFloorResMemPtr;
            u32 count2 = gsFloorMaxBase;
            u32 j;
            for (j = 0; j < count2; j++) {
                GSFloorResource* res = &resBase2[j];
                if (res->active != 0 && res->status == GSFLOOR_RES_LOADED
                    && res->pending == 0) {
                    if (res->modelHandle != NULL) {
                        fn_800F0494(res->modelHandle);
                        res->modelHandle = NULL;
                    }
                }
            }

            /* Transition to sub-state 3 */
            ctx->isActive = 3;

            /* Pause/unpause resources across ext pools */
            {
                GSFloorResource* pool1 = (GSFloorResource*)
                    ((u8*)resBase + gsFloorMaxBase * 0x24);
                u32 count1 = gsFloorMaxExt1;
                u32 floorId = ctx->floorId;

                for (i = 0; i < count1; i++) {
                    GSFloorResource* res = &pool1[i];
                    if (res->status == GSFLOOR_RES_FREE
                        && res->floorId == floorId) {
                        res->pending = 0;
                        if (res->status == GSFLOOR_RES_LOADED
                            && res->modelHandle != NULL) {
                            fn_800F0424(res->modelHandle);
                        }
                    }
                }
            }
        }
        break;
    }

    case 3:
        /* Ongoing checks -- return 0 to allow next sub-state */
        ctx->isActive = 4;
        break;

    case 4:
        /* Wait for model confirmation */
        ctx->isActive = 5;
        break;

    case 5:
        /* Floor fully loaded, signal completion */
        ctx->isActive = 6;
        break;

    case 6:
        /* Cleanup complete, return to update loop */
        return 1;

    default:
        break;
    }

    return 0;
}

/* =======================================================================
 *  GSfloorLoadParticle / fn_80101244
 *  Address: 0x80101244, Size: 0xA4
 *
 *  Loads particle data for the current floor. Allocates a 32-byte-aligned
 *  buffer, then memcpy's the particle data.
 *
 *  r3 = dst, r4 = size, r5 = callback, r6 = callbackArg
 * ======================================================================= */
void GSfloorLoadParticle(void* dst, u32 size, void* callback, void* callbackArg)
{
    void* buf;

    fn_800DD970(lbl_802719C4);  /* "loadParticle(): loading..." */

    /* Allocate 32-byte aligned buffer for particle data */
    buf = fn_800F9418((size + 0x1F) & ~0x1F, 0x20,
                       (u32)callback, (u32)callbackArg, NULL);
    if (buf == NULL) {
        fn_800DD970(lbl_802719E0, size);
        return;
    }

    memcpy(buf, dst, size);
}

/* =======================================================================
 *  GSfloorFindAndOpen / fn_801012E8
 *  Address: 0x801012E8, Size: 0xB8
 *
 *  Finds a floor's FSYS data by name and requests loading.
 *
 *  r3 = archiveName, r4 = floorId, r5 = loadParam
 * ======================================================================= */
void GSfloorFindAndOpen(const char* archiveName, u32 floorId, u32 loadParam)
{
    void* tocEntry;
    void* archivePtr;

    if (archiveName == NULL) {
        fn_800DD970(lbl_802717F0 + 0x300);  /* error: NULL archive name */
        return;
    }

    /* Look up archive in FSYS TOC */
    tocEntry = HSD_ArchiveGetPublicAddress((void*)archiveName,
                                           lbl_802717F0 + 0x2C8);  /* TOC search key */
    if (tocEntry == NULL) {
        fn_800DD970(lbl_802717F0 + 0x328);  /* error: not found in TOC */
        return;
    }

    /* Get the archive data pointer */
    archivePtr = fn_800D27FC(*(u32*)((u8*)tocEntry + 4));
    if (archivePtr == NULL) {
        fn_800DD970(lbl_802717F0 + 0x358);  /* error: can't get archive data */
    }

    /* Begin resource loading */
    fn_800F9378(archivePtr, floorId, loadParam, (void*)fn_80101A28);
}

/* Internal callback for GSfloorFindAndOpen (fn_80101A28) -- declared above */

/* =======================================================================
 *  GSfloorLoadData / fn_801013A0
 *  Address: 0x801013A0, Size: 0xDC
 *
 *  Loads a sub-file from a named FSYS archive.
 *
 *  r3 = archiveName, r4 = subFileIndex (mapped to floorId), r5 = fileIndex,
 *  r6 = loadParam
 * ======================================================================= */
void GSfloorLoadData(const char* archiveName, u32 subFileIndex,
                     u32 fileIndex, u32 loadParam)
{
    void* tocEntry;
    u32* fileList;
    void* archivePtr;
    u32 idx;

    if (archiveName == NULL) {
        fn_800DD970(lbl_802717F0 + 0x3F4);  /* error: NULL name */
        return;
    }

    /* Look up archive in FSYS TOC */
    tocEntry = HSD_ArchiveGetPublicAddress((void*)archiveName,
                                           lbl_802717F0 + 0x2C8);
    if (tocEntry == NULL) {
        fn_800DD970(lbl_802717F0 + 0x418);  /* error: not found */
        return;
    }

    /* Walk the file list to find the target sub-file */
    fileList = (u32*)*(u32*)tocEntry;
    idx = 0;
    while (*fileList != 0) {
        if (idx == fileIndex) {
            /* Found the target file entry */
            archivePtr = (void*)fn_800E4D18(fileList[idx]);
            if (archivePtr == NULL) {
                fn_800DD970(lbl_802717F0 + 0x448, idx);
            }
            fn_800F9378(archivePtr, subFileIndex, loadParam,
                         (void*)fn_80101A4C);
            return;
        }
        fileList++;
        idx++;
    }
}

/* Internal callback for GSfloorLoadData (fn_80101A4C) -- declared above */

/* =======================================================================
 *  GSfloorLoadMain / fn_8010147C
 *  Address: 0x8010147C, Size: 0x494
 *
 *  Main floor loading procedure. Manages the floor data entry table,
 *  allocates a resource load thread, and registers the floor in the
 *  static floor data table.
 *
 *  r3 = fsysHandle, r4 = archiveName, r5 = loadParam1, r6 = loadParam2
 *
 *  The function:
 *  1. Searches the floor data table (gsFloorDataTable, 0x80 entries)
 *     for an existing entry with matching fsysHandle. If found,
 *     increments its reference count.
 *  2. If not found, allocates a new resource thread (fn_800F9418),
 *     binds it to the FSYS archive (HSD_ArchiveParse), finds a free slot
 *     in the table, and copies the entry data.
 *  3. Counts the total number of sub-files and FSYS entries, and
 *     logs the result.
 * ======================================================================= */
void GSfloorLoadMain(u32 fsysHandle, const char* archiveName,
                     u32 loadParam1, u32 loadParam2)
{
    GSFloorDataEntry* table;
    GSFloorDataEntry* found;
    void* resThread;
    void* tocEntry;
    u32 numSubFiles;
    u32 numEntries;
    u32 i;

    if (fsysHandle == 0 || archiveName == NULL)
        return;

    fn_800DD970(lbl_802717F0 + 0x520);  /* loading info message */

    /* Search the unrolled loop: 8 entries per iteration, 0x10 iters = 0x80 entries */
    table = gsFloorDataTable;
    found = NULL;

    for (i = 0; i < GSFLOOR_MAX_ENTRIES; i++) {
        GSFloorDataEntry* entry = &table[i];
        if (entry->refCount != 0 && entry->fsysFileHandle == fsysHandle) {
            found = entry;
            break;
        }
    }

    if (found != NULL) {
        /* Entry already exists: increment reference count */
        found->refCount++;
        goto postRegister;
    }

    /* No existing entry: create a new resource load thread */
    resThread = fn_800F9418(0x60, 0x20, loadParam1, loadParam2,
                             (void*)fn_80101910);
    if (resThread == NULL) {
        fn_800DD970(lbl_802717F0 + 0x540, 0x44);
        return;
    }

    /* Bind to the FSYS archive */
    HSD_ArchiveParse(resThread, fsysHandle, archiveName);

    /* Find a free slot and copy the entry data */
    /* First pass: search in blocks of 4 (unrolled) for matching handles */
    for (i = 0; i < GSFLOOR_MAX_ENTRIES; i++) {
        GSFloorDataEntry* entry = &table[i];
        if (entry->refCount != 0) {
            u32 newHandle = ((GSFloorDataEntry*)resThread)->fsysFileHandle;
            if (entry->fsysFileHandle == newHandle) {
                entry->refCount++;
                goto postRegister;
            }
        }
    }

    /* No matching slot: find any free slot */
    for (i = 0; i < GSFLOOR_MAX_ENTRIES; i++) {
        GSFloorDataEntry* entry = &table[i];
        if (entry->refCount == 0) {
            memcpy(entry, resThread, 0x44);
            entry->refCount = 1;
            break;
        }
    }

postRegister:
    /* Look up TOC for statistics logging */
    tocEntry = HSD_ArchiveGetPublicAddress(found ? found : &table[0],
                                           lbl_802717F0 + 0x2C8);

    /* Count sub-files */
    numSubFiles = 0;
    {
        u32* fileList = (u32*)*(u32*)tocEntry;
        if (fileList != NULL) {
            while (*fileList != 0) {
                fileList++;
                numSubFiles++;
            }
        }
    }

    /* Count entries */
    numEntries = 0;
    {
        u32* entryList = (u32*)((u8*)tocEntry + 8);
        if (entryList != NULL) {
            while (*entryList != 0) {
                entryList++;
                numEntries++;
            }
        }
    }

    fn_800DD970(lbl_802717F0 + 0x574, numSubFiles, numEntries);
}

/* Internal load callback (fn_80101910) -- declared above */

/* =======================================================================
 *  GSfloorGetCurrentId
 *  Returns the current floor ID from the sdata global.
 * ======================================================================= */
u32 GSfloorGetCurrentId(void)
{
    return gsFloorCurrentId;
}

/* =======================================================================
 *  GSfloorGetContext
 *  Returns the current floor context pointer.
 * ======================================================================= */
GSFloorContext* GSfloorGetContext(void)
{
    return gsFloorCurrent;
}

/* ===================================================================
 * Generated: 0 pattern-matched + 9 stubs
 * Range: 0x800FF788 - 0x80101910
 * =================================================================== */

/* 0x800FF788 | 0x94 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF788(void) {
    /* TODO: match -- 148 bytes at 0x800FF788 */
}
#pragma pop

/* 0x800FF81C | 0xC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF81C(void* tablePtr, u32 tableCount) {
    extern void* lbl_8047ACD0;
    extern u32 lbl_8047ACD4;
    lbl_8047ACD0 = tablePtr;
    lbl_8047ACD4 = tableCount;
}
#pragma pop

/* 0x800FF828 | 0x148 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF828(void) {
    /* TODO: match -- 328 bytes at 0x800FF828 */
}
#pragma pop

/* 0x800FF970 | 0x11B4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF970(void) {
    /* TODO: match -- 4532 bytes at 0x800FF970 */
}
#pragma pop

/* 0x80100B24 | 0x720 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80100B24(void) {
    /* TODO: match -- 1824 bytes at 0x80100B24 */
}
#pragma pop

/* 0x80101244 | 0xA4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80101244(void) {
    /* TODO: match -- 164 bytes at 0x80101244 */
}
#pragma pop

/* 0x801012E8 | 0xB8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801012E8(void) {
    /* TODO: match -- 184 bytes at 0x801012E8 */
}
#pragma pop

/* 0x801013A0 | 0xDC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801013A0(void) {
    /* TODO: match -- 220 bytes at 0x801013A0 */
}
#pragma pop

/* 0x8010147C | 0x494 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010147C(void) {
    /* TODO: match -- 1172 bytes at 0x8010147C */
}
#pragma pop
