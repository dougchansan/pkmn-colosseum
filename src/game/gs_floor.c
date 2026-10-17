/**
 * @file gs_floor.c
 * @brief GSfloor -- floor (scene) loading, the floor worker thread and the
 *        per-floor resource pools.
 *
 * Address range: 0x800FF788 - 0x80101910
 *   fn_800FF788   request a floor (idle worker only)
 *   fn_800FF81C   install the floor table
 *   fn_800FF828   allocate the floor-context stack and the three resource
 *                 pools, then start fn_800FF970 as a GS thread
 *   fn_800FF970   the cooperative floor worker: a state machine over
 *                 lbl_8047ACD8 (idle / loading / running / unloading /
 *                 entering a nested floor / returning from one) that yields
 *                 through _threadSwitch
 *   fn_80100B24   the per-frame resource phase machine the worker runs while
 *                 a floor is up (GSFloorContext.isActive)
 *   loadParticle, loadCamera, loadModel, loadArchivePtr
 *                 resource registration helpers (particles, models, archives)
 *
 * The static inline helpers below are the ones the object proves: each is
 * expanded at two or more sites with the same instruction sequence, or (the
 * resource snapshot and the archive-cache lookup) carries an inline return
 * artifact -- a known-zero/known-non-NULL result rematerialised and re-tested
 * on the path that produced it.
 *
 * Status: every function matches retail with no compiler-control pragmas;
 * the unit is linked.
 */

#include "dolphin/types.h"
#include "game/gs_floor.h"
#include "game/gs_thread.h"
#include "hsd/hsd_archive.h"

/* ===== External engine / SDK functions ===== */
extern void  GSlogWrite(const char* fmt, ...);         /* GSlog / OSReport */
extern void* GSresAllocResourceAlign(u32 size, u32 alignment, u32 loadParam,
                                      u32 loadParam2, void* callback);
extern void  fn_80101910(void* resource);
extern void  memcpy(void* dst, const void* src, u32 n);

/* ===== String constants (rodata references) ===== */
extern const char lbl_802717F0[];  /* "GSfloorOpen: cannot find floor %d\n" */
extern const char lbl_802719C4[];  /* "loadParticle(): loading...\n" */
extern const char lbl_802719E0[];  /* "loadParticlePtr(): can't alloc %d bytes of memory\n" */


/**
 * One entry of the floor table installed by fn_800FF81C (stride 0x4C). Only
 * the fields this unit reads are named: the top three bits of the first byte
 * select which resource handlers snapshot state across a nested floor, and
 * the word at 0x0C is the floor index the table is searched by.
 */
typedef struct GSFloorTableEntry {
    /* 0x00 */ u8  resType : 3;
    /* 0x00 */ u8  flags00 : 5;
    /* 0x01 */ u8  pad01[0x0B];
    /* 0x0C */ u32 floorIndex;
    /* 0x10 */ u8  pad10[0x3C];
} GSFloorTableEntry;

/* ===== Globals this unit's thread state machine works on ===== */
extern GSFloorContext*  lbl_8047ACA4;   /* context stack storage */
extern u16              lbl_8047ACA0;
extern u32              lbl_8047ACA8;
extern u16              lbl_8047ACAC;
extern GSFloorResource* lbl_8047ACB0;   /* resource pool base, stride 0x24 */
extern u32              lbl_8047ACB4;   /* pool 0 entry count */
extern u32              lbl_8047ACB8;   /* pool 1 entry count */
extern u32              lbl_8047ACBC;   /* pool 2 entry count */
extern u32              lbl_8047ACC0;
extern s32              lbl_8047ACC4;   /* context stack depth */
extern GSFloorContext*  lbl_8047ACC8;   /* current context */
extern GSFloorResource* lbl_8047ACCC;   /* active-list cursor */
extern GSFloorTableEntry* lbl_8047ACD0;   /* floor table, stride 0x4C */
extern u32              lbl_8047ACD4;   /* floor table entry count */
extern s32              lbl_8047ACD8;   /* thread state (GSFloorState) */
extern s32              lbl_8047ACDC;   /* state to resume after RUNNING */
extern u32              lbl_8047ACE0;   /* resource handler count */
/*
 * Requested floor index, -1 = none. Written by game code running on other GS
 * threads and polled by the worker; retail reloads it at every use (the -1
 * test and the table search that follows it, with no store in between), as
 * the volatile declaration in gs_floor_data.c already records.
 */
extern volatile u32     lbl_80478B18;
extern GSFloorResHandler lbl_80404918[];
extern const char       lbl_80271814[]; /* "not find floor %d\n" */

extern void  _threadSwitch(void);
extern void  fn_801123D4(void* entry, s32 mode);
extern void  fn_8010D064(void);
extern void  fn_8010CC54(void);
extern void  fn_8010CD6C(u32 groupId);
extern void  fn_8010D038(void);
extern u32   floorDataBiosGetFileGroupID(void* entry);
extern u32   floorDataBiosGetGroupID(void* entry);
extern void  fn_8017B3E4(u32 fileGroupId);
extern s32   fn_8017B2CC(u32 fileGroupId);
extern void  fn_8017B1CC(u32 fileGroupId);
extern u8    fn_80100B24(GSFloorContext* ctx);
extern void  floorTerminateScriptThreads(u16 handle);
extern u8    floorCheckFightKind(s32 floorIndex);
extern void  floorCheckFade(void);
extern void  tvRelease(void);
extern void  fn_8018DB04(s32 arg);
extern void  fn_800F716C(u32 groupId);
extern void  fn_800F915C(u32 groupId);
extern void  GSthreadTerminateGroup(u32 groupId);
extern void  GSthreadBlock(void* thread);
extern void  GSthreadUnblock(void* thread);
extern void  GSthreadBlockGroup(u32 groupId);
extern void  GSthreadUnblockGroup(u32 groupId);
extern void  menuCloseFloor(void);
extern void  gfxSetFog(s32 arg);
extern void  psRemoveParticle(void);
extern void  psRemoveGenerator(void);
extern void  psRemoveAppSRT(void);
extern void  GSmodelFreeAllShadowTextures(void);
extern void  fn_80112780(void);
extern void  fn_801127BC(void);
extern u16   _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u16 handle);
extern void  fn_800E24B0(u16 handle);
extern void  fn_800E209C(u16 handle);
extern u32   fn_800F7318(u32 task, void* callback, u32 stackSize, u32 arg3,
                         u32 arg4, u32 arg5, ...);
extern void* fn_800F7108(u16 handle);
extern u8    GSthreadIsRunning(u32 task);
extern void  GSthreadClose(u32 task);
void fn_800FF970(void);

/** Resource-handler callback shapes, reached through the table at lbl_80404918. */
typedef u32  (*GSFloorResSizeFunc)(void);
typedef void (*GSFloorResIoFunc)(void* buf, u32 size);
typedef void (*GSFloorResInitFunc)(void* entry, u32 floorId);

/**
 * Locate the floor table entry for a floor index, or NULL. Expanded in
 * fn_800FF788 and twice in the worker.
 */
static inline void* floorFindDataEntry(u32 floorIndex) {
    GSFloorTableEntry* entry;
    u32 count;

    entry = lbl_8047ACD0;
    for (count = lbl_8047ACD4; count-- != 0; entry++) {
        if (entry->floorIndex == floorIndex) {
            return entry;
        }
    }
    return NULL;
}

/*
 * The three resource pools sit back to back in lbl_8047ACB0 with their sizes
 * in lbl_8047ACB4/B8/BC. Every pool walk rereads these globals at the start
 * of each loop (fn_80100B24's thread-stop walk reloads them between its two
 * passes over the same pool), so the walks take a pool number rather than a
 * precomputed range.
 */

/** First resource of pool 0, 1 or 2. */
static inline GSFloorResource* floorPoolTop(s32 pool) {
    if (pool == 0) {
        return lbl_8047ACB0;
    }
    if (pool == 1) {
        return &lbl_8047ACB0[lbl_8047ACB4];
    }
    return &lbl_8047ACB0[lbl_8047ACB4 + lbl_8047ACB8];
}

/** Number of resource slots in pool 0, 1 or 2. */
static inline u32 floorPoolNum(s32 pool) {
    if (pool == 0) {
        return lbl_8047ACB4;
    }
    if (pool == 1) {
        return lbl_8047ACB8;
    }
    return lbl_8047ACBC;
}

/**
 * Unlink every resource of the given status belonging to the current floor,
 * releasing its texture first. Expanded once per resource pool and status in
 * each of the two floor teardowns (six identical copies apiece).
 */
static inline void floorReleaseResources(s32 pool, u32 floorId, s32 status) {
    u32 n;
    GSFloorResource* res;

    for (res = floorPoolTop(pool), n = floorPoolNum(pool); n-- != 0; res++) {
        if (res->status != status) {
            continue;
        }
        if (res->floorId != floorId) {
            continue;
        }
        if (res->status == GSFLOOR_RES_LOADED && res->modelHandle != NULL) {
            floorTerminateScriptThreads(res->textureHandle);
        }
        res->active = 0;
        if (res->prev != NULL) {
            res->prev->next = res->next;
        }
        if (res->next != NULL) {
            res->next->prev = res->prev;
        }
        if (lbl_8047ACCC == res) {
            lbl_8047ACCC = res->next;
        }
        res->prev = NULL;
        res->next = NULL;
    }
}

/**
 * Park or resume every resource of the given status belonging to the current
 * floor, so a transition can run without its threads touching the pool.
 */
static inline void floorSetResourcesBlocked(s32 pool, u32 floorId, s32 status, u8 blocked) {
    u32 n;
    GSFloorResource* res;

    for (res = floorPoolTop(pool), n = floorPoolNum(pool); n-- != 0; res++) {
        if (res->status != status) {
            continue;
        }
        if (res->floorId != floorId) {
            continue;
        }
        res->pending = blocked;
        if (res->status != GSFLOOR_RES_LOADED) {
            continue;
        }
        if (res->modelHandle == NULL) {
            continue;
        }
        if (blocked) {
            GSthreadBlock(res->modelHandle);
        } else {
            GSthreadUnblock(res->modelHandle);
        }
    }
}

/**
 * Take a floor's archive and subsystem state down again; the counterpart of
 * floorLoadData, expanded in all three teardown states. A full unload also
 * drops the floor's thread group; a suspend (entering a nested floor) keeps
 * it so the parked threads can resume later.
 */
static inline void floorUnloadData(void* entry, s32 release) {
    if (!floorCheckFightKind(lbl_80478B18)) {
        floorCheckFade();
    }
    tvRelease();
    fn_8018DB04(release);
    if (release) {
        fn_800F716C(lbl_8047ACC8->floorId);
        GSthreadTerminateGroup(lbl_8047ACC8->floorId);
    }
    menuCloseFloor();
    gfxSetFog(0);
    fn_8017B1CC(floorDataBiosGetFileGroupID(entry));
    fn_800F915C(floorDataBiosGetGroupID(entry));
}

/**
 * Bring a floor's archive in and hand it to the subsystems. Expanded in the
 * loading state with a runtime mode and again when a nested floor returns
 * (mode 1, the fade-out re-entry: no fresh setup and no group registration).
 * Yields until the file group reports ready, then twice more.
 */
static inline void floorLoadData(void* entry, s32 mode) {
    u32 groupId;
    s32 rc;

    fn_801123D4(entry, mode);
    if (mode == 0) {
        fn_8010D064();
    }
    if (mode == 2) {
        fn_8010CC54();
    }
    fn_8017B3E4(floorDataBiosGetFileGroupID(entry));
    while (TRUE) {
        rc = fn_8017B2CC(floorDataBiosGetFileGroupID(entry));
        if (rc < 0) {
            GSlogWrite(lbl_80271814);
        }
        if (rc == 0) {
            break;
        }
        _threadSwitch();
    }
    groupId = floorDataBiosGetGroupID(entry);
    if (mode != 1) {
        fn_8010CD6C(groupId);
    }
    _threadSwitch();
    _threadSwitch();
}

/**
 * Word-aligned size of one handler's state block. Expanded in both passes of
 * floorSaveResourceState (the same bctrl / addi 3 / clrrwi 2 sequence).
 */
static inline u32 floorResStateSize(GSFloorResHandler* handler) {
    return (((GSFloorResSizeFunc)handler->sizeFunc)() + 3) & ~3;
}

/**
 * Snapshot every registered state block of the current floor's resource type
 * into one GSmem allocation (each block prefixed by its word-aligned size) and
 * return its handle, or 0 when nothing could be allocated. Single use; the
 * retail code rematerialises the `return 0` into the handle register on the
 * allocation-failed path, where the handle is already zero.
 *
 * The second pass's count, size and handler sit in the registers MWCC hands
 * out last (r27/r26/r25). The first pass's loop header initialises count
 * before handler; that is what orders the later live ranges of the two
 * reused loop variables (count, then handler) -- the same header with the
 * handler assigned first gives them the other way round.
 */
static inline u16 floorSaveResourceState(void) {
    u32 resType;
    u16 handle;
    u32 total;
    u32 count;
    GSFloorResHandler* handler;
    u32 size;
    u8* data;
    u8* buf;

    resType = ((GSFloorTableEntry*)lbl_8047ACC8->floorDataEntry)->resType;
    total = 0;
    for (count = lbl_8047ACE0, handler = lbl_80404918; count-- != 0; handler++) {
        if (handler->typeId == resType) {
            total += floorResStateSize(handler) + 4;
        }
    }
    handle = _toolentryAlloc__FUl(total);
    if (handle == 0) {
        return 0;
    }
    buf = fn_800E27B0(handle);
    if (buf == NULL) {
        return 0;
    }
    handler = lbl_80404918;
    for (count = lbl_8047ACE0; count-- != 0; handler++) {
        if (handler->typeId == resType) {
            size = floorResStateSize(handler);
            data = buf + 4;
            *(u32*)buf = size;
            ((GSFloorResIoFunc)handler->saveFunc)(data, size);
            buf = data + size;
        }
    }
    fn_800E24B0(handle);
    return handle;
}

/**
 * Counterpart of floorSaveResourceState: hand every saved block back to its
 * handler's load callback and free the snapshot. Single use (the return from
 * a nested floor); the retail loop materialises the handler table address
 * straight into the loop register, whereas the same code written in the
 * worker's case body goes through r0 and an extra `mr` (1134 instructions
 * against retail's 1133) -- only the inline's early return reproduces it.
 */
static inline void floorLoadResourceState(void) {
    u32 resType;
    u16 handle;
    GSFloorResHandler* handler;
    u8* data;
    u32 size;
    u32 count;
    u8* buf;

    handle = lbl_8047ACC8->resMemHandle;
    resType = ((GSFloorTableEntry*)lbl_8047ACC8->floorDataEntry)->resType;
    buf = fn_800E27B0(handle);
    if (buf == NULL) {
        return;
    }
    handler = lbl_80404918;
    for (count = lbl_8047ACE0; count-- != 0; handler++) {
        if (handler->typeId == resType) {
            size = *(u32*)buf;
            data = buf + 4;
            ((GSFloorResIoFunc)handler->loadFunc)(data, size);
            buf = data + size;
        }
    }
    fn_800E24B0(handle);
    fn_800E209C(handle);
}

/**
 * Run the init callback of every idle resource in the given state, and start
 * a GS thread for every threaded one. The callback may unlink the resource,
 * so when the resource has no successor any more the walk falls back on the
 * one it saved beforehand. Expanded in phases 1, 3 and 5 of fn_80100B24 (the
 * same instruction sequence with the state constant 1, 3 or 5).
 *
 * The successor step is written `if (res->next == NULL) res = next; else
 * res = res->next;`. The equivalent `res = res->next; if (res == NULL)
 * res = next;` emits the same instructions but keeps `res` among the
 * helper's own locals for colouring, so phase 1's `res` lands on r28
 * instead of retail's r27; a conditional expression gives retail's colours
 * but a `beq; b` pair where retail has one `bne`.
 */
static inline void floorStartResources(GSFloorContext* ctx, s32 state) {
    GSFloorResource* res;
    void* entry = ctx->floorDataEntry;
    GSFloorResource* next;

    res = lbl_8047ACCC;
    while (res != NULL) {
        next = res->next;
        if (res->active == state && res->pending == 0) {
            if (res->status == GSFLOOR_RES_FREE) {
                ((GSFloorResInitFunc)res->callback)(entry, res->floorId);
            }
            if (res->status == GSFLOOR_RES_LOADED) {
                res->textureHandle = fn_800F7318(res->priority, res->callback, 0x4000, 0, 0, 4,
                                                 floorDataBiosGetGroupID(entry), 0, 0, 0);
                res->modelHandle = fn_800F7108(res->textureHandle);
            }
        }
        if (res->next == NULL) {
            res = next;
        } else {
            res = res->next;
        }
    }
}

/**
 * The init-only walk: run the init callback of every idle, unthreaded
 * resource in the given state, with the same successor step as
 * floorStartResources. Expanded in phases 1 (fading back into a floor) and
 * 4 of fn_80100B24 (the same instruction sequence with the state constant 1
 * or 3).
 */
static inline void floorInitResources(GSFloorContext* ctx, s32 state) {
    void* entry = ctx->floorDataEntry;
    GSFloorResource* next;
    GSFloorResource* res;

    res = lbl_8047ACCC;
    while (res != NULL) {
        next = res->next;
        if (res->active == state && res->pending == 0 && res->status == GSFLOOR_RES_FREE) {
            ((GSFloorResInitFunc)res->callback)(entry, res->floorId);
        }
        if (res->next == NULL) {
            res = next;
        } else {
            res = res->next;
        }
    }
}


/**
 * Once no threaded resource of a pool is still running, close all of their
 * threads and report TRUE; report FALSE while any is still running.
 * Expanded in phases 2 (pool 0) and 6 (pool 2) of fn_80100B24.
 */
static inline u8 floorStopPoolThreads(s32 pool) {
    u32 n;
    GSFloorResource* res;

    for (res = floorPoolTop(pool), n = floorPoolNum(pool); n-- != 0; res++) {
        if (res->active != 0 && res->status == GSFLOOR_RES_LOADED && res->pending == 0 &&
            res->modelHandle != NULL && GSthreadIsRunning((u32)res->modelHandle)) {
            return FALSE;
        }
    }
    for (res = floorPoolTop(pool), n = floorPoolNum(pool); n-- != 0; res++) {
        if (res->active != 0 && res->status == GSFLOOR_RES_LOADED && res->pending == 0 &&
            res->modelHandle != NULL) {
            GSthreadClose((u32)res->modelHandle);
            res->modelHandle = NULL;
        }
    }
    return TRUE;
}

/**
 * One slot of the parsed-archive cache at lbl_80402518 (0x80 slots): a copy
 * of the HSD_Archive header block plus a reference count; 0 marks a free slot.
 */
typedef struct GSArchiveCacheEntry {
    /* 0x00 */ HSD_Archive archive;
    /* 0x44 */ s32 refs;
} GSArchiveCacheEntry;

#define GS_ARCHIVE_CACHE_NUM 0x80

extern GSArchiveCacheEntry lbl_80402518[GS_ARCHIVE_CACHE_NUM];

/**
 * Cached archive already parsed from this file image, or NULL. Single use;
 * the caller re-tests the result for NULL on the found path too, where it is
 * known non-NULL (the helper's return routed through the result register).
 */
static inline HSD_Archive* floorFindCachedArchive(void* data) {
    GSArchiveCacheEntry* entry;
    s32 i;

    entry = lbl_80402518;
    for (i = 0; i < GS_ARCHIVE_CACHE_NUM; i++, entry++) {
        if (entry->refs != 0 && entry->archive.top_ptr == data) {
            return &entry->archive;
        }
    }
    return NULL;
}

/**
 * Count one more user of an archive's file image: bump the matching cache
 * slot, or copy the archive into the first free slot.
 */
static inline void floorCacheArchive(HSD_Archive* archive) {
    GSArchiveCacheEntry* entry;
    s32 i;

    entry = lbl_80402518;
    for (i = 0; i < GS_ARCHIVE_CACHE_NUM; i++, entry++) {
        if (entry->refs != 0 && entry->archive.top_ptr == archive->top_ptr) {
            entry->refs++;
            return;
        }
    }
    entry = lbl_80402518;
    for (i = 0; i < GS_ARCHIVE_CACHE_NUM; i++, entry++) {
        if (entry->refs == 0) {
            memcpy(entry, archive, sizeof(HSD_Archive));
            entry->refs = 1;
            return;
        }
    }
}

/* 0x800FF788 | 0x94 */
void fn_800FF788(u32 floorId) {
    GSFloorContext* ctx;
    void* entry;

    ctx = lbl_8047ACC8;
    if (lbl_8047ACD8 != GSFLOOR_STATE_IDLE) {
        return;
    }
    entry = floorFindDataEntry(floorId);
    if (entry == NULL) {
        GSlogWrite(lbl_802717F0, floorId);
        return;
    }
    ctx->floorDataEntry = entry;
    ctx->floorId = floorId + GSFLOOR_ID_BASE;
    ctx->isActive = 1;
    lbl_8047ACD8 = GSFLOOR_STATE_LOADING;
}

/* 0x800FF81C | 0xC */
void fn_800FF81C(GSFloorTableEntry* table, u32 count) {
    lbl_8047ACD0 = table;
    lbl_8047ACD4 = count;
}

/* 0x800FF828 | 0x148 */
void fn_800FF828(u32 contextCount, u32 baseCount, u32 extCount1,
                 u32 extCount2) {
    u32 resourceCount;
    u32 i;

    lbl_8047ACD0 = NULL;
    lbl_8047ACD4 = 0;
    lbl_8047ACA8 = contextCount;

    lbl_8047ACA0 = _toolentryAlloc__FUl(contextCount * sizeof(GSFloorContext));
    if (lbl_8047ACA0 != 0) {
        lbl_8047ACA4 = fn_800E27B0(lbl_8047ACA0);

        resourceCount = baseCount + extCount1 + extCount2;
        lbl_8047ACB4 = baseCount;
        lbl_8047ACB8 = extCount1;
        lbl_8047ACBC = extCount2;
        lbl_8047ACC0 = resourceCount;

        lbl_8047ACAC = _toolentryAlloc__FUl(resourceCount * sizeof(GSFloorResource));
        if (lbl_8047ACAC != 0) {
            lbl_8047ACB0 = fn_800E27B0(lbl_8047ACAC);
            for (i = 0; i < lbl_8047ACC0; i++) {
                lbl_8047ACB0[i].active = 0;
            }

            lbl_8047ACCC = NULL;
            lbl_8047ACC4 = 0;
            lbl_8047ACC8 = lbl_8047ACA4;
            lbl_8047ACC8->floorDataEntry = NULL;
            lbl_8047ACC8->floorId = 0;
            lbl_8047ACC8->resMemHandle = 0;
            lbl_8047ACC8->doFadeIn = 0;
            lbl_8047ACC8->doFadeOut = 0;
            lbl_8047ACC8->isActive = 0;
            lbl_8047ACD8 = 0;
            lbl_80478B18 = -1;
            GSthreadCreate(0, 0x7D0, 0x4000, 1, 1, fn_800FF970);
        }
    }
}

/*
 * 0x800FF970 | 0x11B4 -- floor worker thread.
 *
 * Matches retail. The register colouring of the "enter nested floor" and
 * "return from nested floor" states is fixed by the helper structure:
 * floorLoadResourceState (retail has no r0 copy of the handler table
 * address), floorResStateSize expanded in both snapshot passes, and the
 * snapshot's first loop header initialising count before handler.
 *
 * How MWCC (GC/1.3, -O4,p) colours callee-saved registers here, measured on
 * this object and on small test units: inline-expansion locals are coloured
 * first, expansion by expansion in source order -- the first expansion of a
 * helper in a function takes its locals in reverse declaration order and
 * then its parameters, later expansions take parameters first and locals in
 * declaration order; then the function's own locals in declaration order
 * (top level, then blocks); the later live ranges of a variable reused
 * across loops come last, in the order its first loop header assigned them.
 * A change in one state therefore shifts every later state.
 */
void fn_800FF970(void) {

    while (TRUE) {
        switch (lbl_8047ACD8) {
        case GSFLOOR_STATE_IDLE:
            _threadSwitch();
            break;

        case GSFLOOR_STATE_LOADING: {
            s32 mode;

            if (lbl_8047ACC8->doFadeIn != 0) {
                mode = 0;
            } else if (lbl_8047ACC8->doFadeOut != 0) {
                mode = 1;
            } else {
                mode = 2;
            }
            floorLoadData(lbl_8047ACC8->floorDataEntry, mode);
            lbl_8047ACD8 = GSFLOOR_STATE_RUNNING;
            lbl_8047ACC8->doFadeIn = 0;
            lbl_8047ACC8->doFadeOut = 0;
            lbl_8047ACC8->isActive = 1;
            break;
        }

        case GSFLOOR_STATE_RUNNING:
            if (fn_80100B24(lbl_8047ACC8) == 1) {
                _threadSwitch();
            } else {
                lbl_8047ACD8 = lbl_8047ACDC;
            }
            break;

        case GSFLOOR_STATE_UNLOADING: {
            floorReleaseResources(0, lbl_8047ACC8->floorId, GSFLOOR_RES_FREE);
            floorReleaseResources(1, lbl_8047ACC8->floorId, GSFLOOR_RES_FREE);
            floorReleaseResources(2, lbl_8047ACC8->floorId, GSFLOOR_RES_FREE);
            floorReleaseResources(0, lbl_8047ACC8->floorId, GSFLOOR_RES_LOADED);
            floorReleaseResources(1, lbl_8047ACC8->floorId, GSFLOOR_RES_LOADED);
            floorReleaseResources(2, lbl_8047ACC8->floorId, GSFLOOR_RES_LOADED);
            floorUnloadData(lbl_8047ACC8->floorDataEntry, 1);
            psRemoveParticle();
            psRemoveGenerator();
            psRemoveAppSRT();
            GSmodelFreeAllShadowTextures();
            lbl_8047ACC8->doFadeIn = 0;
            lbl_8047ACC8->doFadeOut = 0;
            if (lbl_80478B18 != (u32)-1) {
                lbl_8047ACC8->floorDataEntry = floorFindDataEntry(lbl_80478B18);
                lbl_8047ACC8->floorId = lbl_80478B18 + GSFLOOR_ID_BASE;
                lbl_8047ACD8 = GSFLOOR_STATE_LOADING;
            } else {
                lbl_8047ACD8 = GSFLOOR_STATE_IDLE;
            }
            break;
        }

        case GSFLOOR_STATE_TRANSITIONING: {
            fn_80112780();
            lbl_8047ACC8->doFadeIn = 1;
            lbl_8047ACC8->doFadeOut = 0;
            lbl_8047ACC8->resMemHandle = floorSaveResourceState();
            floorSetResourcesBlocked(0, lbl_8047ACC8->floorId, GSFLOOR_RES_FREE, 1);
            floorSetResourcesBlocked(1, lbl_8047ACC8->floorId, GSFLOOR_RES_FREE, 1);
            floorSetResourcesBlocked(2, lbl_8047ACC8->floorId, GSFLOOR_RES_FREE, 1);
            floorSetResourcesBlocked(0, lbl_8047ACC8->floorId, GSFLOOR_RES_LOADED, 1);
            floorSetResourcesBlocked(1, lbl_8047ACC8->floorId, GSFLOOR_RES_LOADED, 1);
            floorSetResourcesBlocked(2, lbl_8047ACC8->floorId, GSFLOOR_RES_LOADED, 1);
            GSthreadBlockGroup(lbl_8047ACC8->floorId);
            floorUnloadData(lbl_8047ACC8->floorDataEntry, 0);
            psRemoveParticle();
            psRemoveGenerator();
            psRemoveAppSRT();
            GSmodelFreeAllShadowTextures();
            lbl_8047ACC4++;
            lbl_8047ACC8 = &lbl_8047ACA4[lbl_8047ACC4];
            lbl_8047ACC8->doFadeIn = 1;
            lbl_8047ACC8->doFadeOut = 0;
            lbl_8047ACC8->floorDataEntry = floorFindDataEntry(lbl_80478B18);
            lbl_8047ACC8->floorId = lbl_80478B18 + GSFLOOR_ID_BASE;
            lbl_8047ACD8 = GSFLOOR_STATE_LOADING;
            break;
        }

        case GSFLOOR_STATE_FINALIZING: {
            floorReleaseResources(0, lbl_8047ACC8->floorId, GSFLOOR_RES_FREE);
            floorReleaseResources(1, lbl_8047ACC8->floorId, GSFLOOR_RES_FREE);
            floorReleaseResources(2, lbl_8047ACC8->floorId, GSFLOOR_RES_FREE);
            floorReleaseResources(0, lbl_8047ACC8->floorId, GSFLOOR_RES_LOADED);
            floorReleaseResources(1, lbl_8047ACC8->floorId, GSFLOOR_RES_LOADED);
            floorReleaseResources(2, lbl_8047ACC8->floorId, GSFLOOR_RES_LOADED);
            floorUnloadData(lbl_8047ACC8->floorDataEntry, 1);
            fn_8010D038();
            psRemoveParticle();
            psRemoveGenerator();
            psRemoveAppSRT();
            GSmodelFreeAllShadowTextures();
            lbl_8047ACC4--;
            lbl_8047ACC8 = &lbl_8047ACA4[lbl_8047ACC4];
            lbl_8047ACC8->doFadeIn = 0;
            lbl_8047ACC8->doFadeOut = 1;
            floorLoadData(lbl_8047ACC8->floorDataEntry, 1);
            floorLoadResourceState();
            floorSetResourcesBlocked(0, lbl_8047ACC8->floorId, GSFLOOR_RES_FREE, 0);
            floorSetResourcesBlocked(0, lbl_8047ACC8->floorId, GSFLOOR_RES_LOADED, 0);
            GSthreadUnblockGroup(lbl_8047ACC8->floorId);
            lbl_8047ACD8 = GSFLOOR_STATE_RUNNING;
            lbl_8047ACC8->isActive = 1;
            break;
        }
        }
    }
}

/*
 * 0x80100B24 | 0x720 -- per-frame resource phases of the current floor.
 *
 * Built at the unit's own flags (the old optimization_level 0 pragma never
 * matched: retail is scheduled -O4 code with stmw). Every phase walks the
 * resources through a helper expanded more than once in this function:
 * floorStartResources (phases 1, 3 and 5), floorInitResources (phases 1 and
 * 4), floorStopPoolThreads (phases 2 and 6) and floorSetResourcesBlocked
 * (four times in phase 2, twice more in the worker).
 *
 * The register colouring pins that structure down. Phase 1's two walks
 * (retail: next/entry in r30/r29 and entry/next in r30/r29, res in r27) are
 * the first expansions in the function, so their locals are coloured before
 * phase 2's; written as case-body blocks they coloured after everything
 * else (r27..r29). With the walks expanded, phase 2's thread-stop pass is
 * a later expansion, which is what lets floorStopPoolThreads declare `n`
 * before `res` (phase 6's r28/r27) without disturbing phase 2.
 *
 * See the note on fn_800FF970 for the colouring order.
 */
u8 fn_80100B24(GSFloorContext* ctx) {
    switch (ctx->isActive) {
    case 1:
        if (ctx->doFadeOut == 0) {
            floorStartResources(ctx, 1);
        } else {
            floorInitResources(ctx, 1);
        }
        ctx->isActive = 2;
        break;

    case 2:
        if (floorStopPoolThreads(0)) {
            ctx->isActive = 3;
            if (ctx->doFadeOut != 0) {
                floorSetResourcesBlocked(1, ctx->floorId, GSFLOOR_RES_FREE, 0);
                floorSetResourcesBlocked(2, ctx->floorId, GSFLOOR_RES_FREE, 0);
                floorSetResourcesBlocked(1, ctx->floorId, GSFLOOR_RES_LOADED, 0);
                floorSetResourcesBlocked(2, ctx->floorId, GSFLOOR_RES_LOADED, 0);
                ctx->isActive = 4;
            }
            fn_801127BC();
        }
        break;

    case 3:
        floorStartResources(ctx, 3);
        if (ctx->isActive == 3) {
            ctx->isActive = 4;
        }
        break;

    case 4:
        floorInitResources(ctx, 3);
        break;

    case 5:
        fn_80112780();
        floorStartResources(ctx, 5);
        ctx->isActive = 6;
        break;

    case 6:
        if (floorStopPoolThreads(2)) {
            return 0;
        }
        break;
    }
    return 1;
}

/* 0x80101244 | 0xA4 */
void loadParticle(void* dst, u32 size, void* callback, void* callbackArg) {
    void* buf;

    GSlogWrite(lbl_802719C4);

    buf = GSresAllocResourceAlign((size + 0x1F) & ~0x1F, 0x20,
                                   (u32)callback, (u32)callbackArg, NULL);
    if (buf == NULL) {
        GSlogWrite(lbl_802719E0, size);
        return;
    }

    memcpy(buf, dst, size);
}

/* 0x801012E8 | 0xB8 */
void loadCamera(void* archive, u32 resourceArg, u32 callbackArg) {
    extern void* fn_800D27FC(void*);
    extern void GSresRegisterResource(void*, u32, u32, void*);
    extern void fn_80101A28(void);
    const char* messages = lbl_802717F0;
    void** publicData;
    void* resource;

    if (archive == NULL) {
        GSlogWrite(messages + 0x300);
        return;
    }
    publicData = HSD_ArchiveGetPublicAddress(archive, messages + 0x2C8);
    if (publicData == NULL) {
        GSlogWrite(messages + 0x328);
        return;
    }
    resource = fn_800D27FC(publicData[1]);
    if (resource == NULL) {
        GSlogWrite(messages + 0x358);
    }
    GSresRegisterResource(resource, resourceArg, callbackArg,
                          (void*)fn_80101A28);
}

/* 0x801013A0 | 0xDC */
void loadModel(void* archive, u32 resourceArg, u32 modelIndex,
                 u32 callbackArg) {
    extern void* GSmodelLoad(void*);
    extern void GSresRegisterResource(void*, u32, u32, void*);
    extern void fn_80101A4C(void);
    const char* messages = lbl_802717F0;
    void*** publicData;
    void** models;
    void* model;
    u32 index;

    if (archive == NULL) {
        GSlogWrite(messages + 0x3F4);
        return;
    }
    publicData = HSD_ArchiveGetPublicAddress(archive, messages + 0x2C8);
    if (publicData == NULL) {
        GSlogWrite(messages + 0x418);
        return;
    }
    models = *publicData;
    for (index = 0; models[index] != NULL; index++) {
        if (index == modelIndex) {
            model = GSmodelLoad(models[index]);
            if (model == NULL) {
                GSlogWrite(messages + 0x448, index);
            }
            GSresRegisterResource(model, resourceArg, callbackArg,
                                  (void*)fn_80101A4C);
            return;
        }
    }
}

/* 0x8010147C | 0x494 */
void loadArchivePtr(u8* data, u32 size, u32 loadParam, u32 callbackArg) {
    const char* messages = lbl_802717F0;
    HSD_Archive* archive;
    HSD_Archive* copy;
    u32** publicData;
    u32* list;
    s32 modelNum;
    s32 animNum;

    if (data == NULL || size == 0) {
        return;
    }

    GSlogWrite(messages + 0x520);
    archive = floorFindCachedArchive(data);
    if (archive == NULL) {
        archive = GSresAllocResourceAlign(0x60, 0x20, loadParam,
                                          callbackArg, fn_80101910);
        if (archive == NULL) {
            GSlogWrite(messages + 0x540, sizeof(HSD_Archive));
            return;
        }
        HSD_ArchiveParse(archive, data, size);
        floorCacheArchive(archive);
    } else {
        copy = GSresAllocResourceAlign(0x60, 0x20, loadParam,
                                       callbackArg, fn_80101910);
        if (copy == NULL) {
            GSlogWrite(messages + 0x540, sizeof(HSD_Archive));
            return;
        }
        memcpy(copy, archive, sizeof(HSD_Archive));
        floorCacheArchive(copy);
    }

    modelNum = 0;
    animNum = 0;
    publicData = HSD_ArchiveGetPublicAddress(archive, messages + 0x2C8);
    if ((list = publicData[0]) != NULL) {
        while (*list != 0) {
            list++;
            modelNum++;
        }
    }
    if ((list = publicData[2]) != NULL) {
        while (*list != 0) {
            list++;
            animNum++;
        }
    }
    GSlogWrite(messages + 0x574, modelNum, animNum);
}
