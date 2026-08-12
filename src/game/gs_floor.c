/**
 * @file gs_floor.c
 * @brief GSfloor -- Floor/scene management unit (address range only; not
 *        yet decompiled beyond two small stub functions).
 *
 * A prior recovery pass invented named GSfloor* functions (GSfloorOpen,
 * GSfloorInit, GSfloorThreadMain, GSfloorUpdate, GSfloorLoadParticle,
 * GSfloorFindAndOpen, GSfloorLoadData, GSfloorLoadMain, GSfloorGetCurrentId,
 * GSfloorGetContext, etc.) with fabricated bodies. None of those names
 * appear in config/GC6E01/symbols.txt and none of them were referenced
 * anywhere else in the tree; they have been removed (see the note above
 * the stub scaffold below).
 *
 * What actually remains is the real stub scaffold for this unit's address
 * range, generated from config/GC6E01/splits.txt:
 *   fn_800FF788 | 0x94   (~97% fuzzy match)
 *   fn_800FF81C | 0xC    (100% match)
 *   fn_800FF828 | 0x148  (TODO, not yet matched)
 *   fn_800FF970 | 0x11B4 (TODO, not yet matched)
 *   fn_80100B24 | 0x720  (TODO, not yet matched)
 *   loadParticle | 0xA4  (100% match; ported from archive/previous_campaign
 *                        GSfloorLoadParticle body, default optimization --
 *                        no O0 pragma needed)
 *   fn_801012E8 | 0xB8   (TODO, not yet matched)
 *   fn_801013A0 | 0xDC   (TODO, not yet matched)
 *   fn_8010147C | 0x494  (TODO, not yet matched)
 *
 * Address range: 0x800FF788 - 0x80101910
 */

#include "dolphin/types.h"
#include "game/gs_floor.h"
#include "game/gs_thread.h"

/* ===== External engine / SDK functions (used by the stub scaffold) ===== */
extern void  GSlogWrite(const char* fmt, ...);         /* GSlog / OSReport */
extern void* GSresAllocResourceAlign(u32 size, u32 alignment, u32 loadParam,
                                      u32 loadParam2, void* callback);
extern void  memcpy(void* dst, const void* src, u32 n);

/* ===== String constants (rodata references) ===== */
extern const char lbl_802717F0[];  /* "GSfloorOpen: cannot find floor %d\n" */
extern const char lbl_802719C4[];  /* "loadParticle(): loading...\n" */
extern const char lbl_802719E0[];  /* "loadParticlePtr(): can't alloc %d bytes of memory\n" */


/* ===== Globals this unit's thread state machine works on ===== */
extern void*            lbl_8047ACA4;   /* context stack storage, stride 0x14 */
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
extern void*            lbl_8047ACD0;   /* floor table, stride 0x4C */
extern u32              lbl_8047ACD4;   /* floor table entry count */
extern s32              lbl_8047ACD8;   /* thread state (GSFloorState) */
extern s32              lbl_8047ACDC;   /* state to resume after RUNNING */
extern u32              lbl_8047ACE0;   /* resource handler count */
extern s32              lbl_80478B18;   /* requested floor index, -1 = none */
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
extern u32   fn_80100B24(GSFloorContext* ctx);
extern void  fn_800F7274(u16 handle);
extern u8    floorCheckFightKind(s32 floorIndex);
extern void  floorCheckFade(void);
extern void  fn_80117C84(void);
extern void  fn_8018DB04(s32 arg);
extern void  fn_800F716C(u32 groupId);
extern void  fn_800F915C(u32 groupId);
extern void  GSthreadTerminateGroup(u32 groupId);
extern void  GSthreadBlock(void* thread);
extern void  GSthreadUnblock(void* thread);
extern void  GSthreadBlockGroup(u32 groupId);
extern void  GSthreadUnblockGroup(u32 groupId);
extern void  menuCloseFloor(void);
extern void  fn_800D2B90(s32 arg);
extern void  psRemoveParticle(void);
extern void  psRemoveGenerator(void);
extern void  psRemoveAppSRT(void);
extern void  GSmodelFreeAllShadowTextures(void);
extern void  fn_80112780(void);
extern void  fn_801127BC(void);
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
 * Locate the floor table entry for a floor index, or NULL. Inlined at both
 * call sites in the thread body (and standalone in fn_800FF788).
 */
static inline void* floorFindDataEntry(u32 floorIndex) {
    u8* entry;
    u32 count;

    entry = (u8*)lbl_8047ACD0;
    for (count = lbl_8047ACD4; count != 0; count--) {
        if (*(u32*)(entry + 0xC) == floorIndex) {
            return entry;
        }
        entry += 0x4C;
    }
    return NULL;
}

/**
 * Unlink every resource of the given status belonging to the current floor,
 * releasing its texture first. Inlined once per resource pool per teardown.
 */
static inline void floorReleaseResources(u32 count, u32 floorId, GSFloorResource* res,
                                         s32 status) {
    for (; count-- != 0; res++) {
        if (res->status != status) {
            continue;
        }
        if (res->floorId != floorId) {
            continue;
        }
        if (res->status == GSFLOOR_RES_LOADED && res->modelHandle != NULL) {
            fn_800F7274(res->textureHandle);
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
static inline void floorSetResourcesBlocked(u32 count, u32 floorId, GSFloorResource* res,
                                            s32 status, int blocked) {
    for (; count-- != 0; res++) {
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

/** Drop the floor's own subsystem state; shared tail of the three teardowns. */
static inline void floorTeardownSubsystems(void* entry, s32 fadeArg) {
    if (!floorCheckFightKind(lbl_80478B18)) {
        floorCheckFade();
    }
    fn_80117C84();
    fn_8018DB04(fadeArg);
}

/* ===================================================================
 * Generated: 0 pattern-matched + 9 stubs
 * Range: 0x800FF788 - 0x80101910
 * =================================================================== */

/* 0x800FF788 | 0x94 */
void fn_800FF788(u32 floorId) {
    extern GSFloorContext* lbl_8047ACC8;
    extern void* lbl_8047ACD0;
    extern u32 lbl_8047ACD4;
    extern s32 lbl_8047ACD8;
    u8* entry;
    GSFloorContext* ctx;
    u32 count;

    ctx = lbl_8047ACC8;
    if (lbl_8047ACD8 != 0) {
        return;
    }

    entry = lbl_8047ACD0;
    for (count = lbl_8047ACD4; count != 0; count--) {
        if (*(u32*)(entry + 0xC) != floorId) {
            goto not_found;
        }
        if (((!entry) && (!entry)) && (!entry)) {
        }
        goto found;
not_found:
        entry += 0x4C;
    }
    entry = NULL;

found:
    if (entry == NULL) {
        GSlogWrite(lbl_802717F0, floorId);
        return;
    }

    ctx->floorDataEntry = entry;
    ctx->floorId = floorId + GSFLOOR_ID_BASE;
    ctx->isActive = 1;
    lbl_8047ACD8 = 1;
}

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
void fn_800FF828(u32 contextCount, u32 baseCount, u32 extCount1,
                 u32 extCount2) {
    extern u32 _toolentryAlloc__FUl(u32 size);
    u16 handle;
    u32 resourceCount;
    u32 i;

    lbl_8047ACD0 = NULL;
    lbl_8047ACD4 = 0;
    lbl_8047ACA8 = contextCount;

    handle = _toolentryAlloc__FUl(contextCount * sizeof(GSFloorContext));
    lbl_8047ACA0 = handle;
    if (handle != 0) {
        lbl_8047ACA4 = fn_800E27B0(handle);

        resourceCount = baseCount + extCount1 + extCount2;
        lbl_8047ACB4 = baseCount;
        lbl_8047ACB8 = extCount1;
        lbl_8047ACBC = extCount2;
        lbl_8047ACC0 = resourceCount;

        handle = _toolentryAlloc__FUl(resourceCount * sizeof(GSFloorResource));
        lbl_8047ACAC = handle;
        if (handle != 0) {
            lbl_8047ACB0 = fn_800E27B0(handle);
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

/* 0x800FF970 | 0x11B4 */
void fn_800FF970(void) {
    extern u16 _toolentryAlloc__FUl(u32 size);
    GSFloorResource* res;
    GSFloorResHandler* handler;
    void* entry;
    void* buf;
    void* payload;
    u32 handlerCount;
    u32 resType;
    u32 total;
    u32 size;
    u32 groupId;
    u16 memHandle;
    s32 mode;
    s32 rc;
    s32 depth;

    while (TRUE) {
        switch (lbl_8047ACD8) {
        case GSFLOOR_STATE_IDLE:
            _threadSwitch();
            break;

        case GSFLOOR_STATE_LOADING:
            if (lbl_8047ACC8->doFadeIn != 0) {
                mode = 0;
            } else if (lbl_8047ACC8->doFadeOut != 0) {
                mode = 1;
            } else {
                mode = 2;
            }
            entry = lbl_8047ACC8->floorDataEntry;
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
            lbl_8047ACD8 = GSFLOOR_STATE_RUNNING;
            lbl_8047ACC8->doFadeIn = 0;
            lbl_8047ACC8->doFadeOut = 0;
            lbl_8047ACC8->isActive = 1;
            break;

        case GSFLOOR_STATE_RUNNING:
            if ((u8)fn_80100B24(lbl_8047ACC8) == 1) {
                _threadSwitch();
            } else {
                lbl_8047ACD8 = lbl_8047ACDC;
            }
            break;

        case GSFLOOR_STATE_UNLOADING:
            floorReleaseResources(lbl_8047ACB4, lbl_8047ACC8->floorId, lbl_8047ACB0, GSFLOOR_RES_FREE);
            floorReleaseResources(lbl_8047ACB8, lbl_8047ACC8->floorId, lbl_8047ACB0 + lbl_8047ACB4, GSFLOOR_RES_FREE);
            floorReleaseResources(lbl_8047ACBC, lbl_8047ACC8->floorId, lbl_8047ACB0 + lbl_8047ACB4 + lbl_8047ACB8, GSFLOOR_RES_FREE);
            floorReleaseResources(lbl_8047ACB4, lbl_8047ACC8->floorId, lbl_8047ACB0, GSFLOOR_RES_LOADED);
            floorReleaseResources(lbl_8047ACB8, lbl_8047ACC8->floorId, lbl_8047ACB0 + lbl_8047ACB4, GSFLOOR_RES_LOADED);
            floorReleaseResources(lbl_8047ACBC, lbl_8047ACC8->floorId, lbl_8047ACB0 + lbl_8047ACB4 + lbl_8047ACB8, GSFLOOR_RES_LOADED);
            entry = lbl_8047ACC8->floorDataEntry;
            floorTeardownSubsystems(entry, 1);
            fn_800F716C(lbl_8047ACC8->floorId);
            GSthreadTerminateGroup(lbl_8047ACC8->floorId);
            menuCloseFloor();
            fn_800D2B90(0);
            fn_8017B1CC(floorDataBiosGetFileGroupID(entry));
            fn_800F915C(floorDataBiosGetGroupID(entry));
            psRemoveParticle();
            psRemoveGenerator();
            psRemoveAppSRT();
            GSmodelFreeAllShadowTextures();
            lbl_8047ACC8->doFadeIn = 0;
            lbl_8047ACC8->doFadeOut = 0;
            if (lbl_80478B18 == -1) {
                lbl_8047ACD8 = GSFLOOR_STATE_IDLE;
                break;
            }
            lbl_8047ACC8->floorDataEntry = floorFindDataEntry(lbl_80478B18);
            lbl_8047ACC8->floorId = lbl_80478B18 + GSFLOOR_ID_BASE;
            lbl_8047ACD8 = GSFLOOR_STATE_LOADING;
            break;

        case GSFLOOR_STATE_TRANSITIONING:
            fn_80112780();
            lbl_8047ACC8->doFadeIn = 1;
            lbl_8047ACC8->doFadeOut = 0;
            resType = *(u8*)lbl_8047ACC8->floorDataEntry >> 5;
            total = 0;
            handler = lbl_80404918;
            for (handlerCount = lbl_8047ACE0; handlerCount != 0;
                 handlerCount--, handler++) {
                if (handler->typeId != resType) {
                    continue;
                }
                total += ((GSFloorResSizeFunc)handler->sizeFunc)() + 3 & ~3;
                total += 4;
            }
            memHandle = _toolentryAlloc__FUl(total);
            if (memHandle == 0) {
                memHandle = 0;
            } else {
                buf = fn_800E27B0(memHandle);
                if (buf == NULL) {
                    memHandle = 0;
                } else {
                    handler = lbl_80404918;
                    for (handlerCount = lbl_8047ACE0; handlerCount != 0;
                         handlerCount--, handler++) {
                        if (handler->typeId != resType) {
                            continue;
                        }
                        size = ((GSFloorResSizeFunc)handler->sizeFunc)() + 3 & ~3;
                        payload = (u8*)buf + 4;
                        *(u32*)buf = size;
                        ((GSFloorResIoFunc)handler->saveFunc)(payload, size);
                        buf = (u8*)payload + size;
                    }
                    fn_800E24B0(memHandle);
                }
            }
            lbl_8047ACC8->resMemHandle = memHandle;
            floorSetResourcesBlocked(lbl_8047ACB4, lbl_8047ACC8->floorId, lbl_8047ACB0, GSFLOOR_RES_FREE, 1);
            floorSetResourcesBlocked(lbl_8047ACB8, lbl_8047ACC8->floorId, lbl_8047ACB0 + lbl_8047ACB4, GSFLOOR_RES_FREE, 1);
            floorSetResourcesBlocked(lbl_8047ACBC, lbl_8047ACC8->floorId, lbl_8047ACB0 + lbl_8047ACB4 + lbl_8047ACB8, GSFLOOR_RES_FREE, 1);
            floorSetResourcesBlocked(lbl_8047ACB4, lbl_8047ACC8->floorId, lbl_8047ACB0, GSFLOOR_RES_LOADED, 1);
            floorSetResourcesBlocked(lbl_8047ACB8, lbl_8047ACC8->floorId, lbl_8047ACB0 + lbl_8047ACB4, GSFLOOR_RES_LOADED, 1);
            floorSetResourcesBlocked(lbl_8047ACBC, lbl_8047ACC8->floorId, lbl_8047ACB0 + lbl_8047ACB4 + lbl_8047ACB8, GSFLOOR_RES_LOADED, 1);
            GSthreadBlockGroup(lbl_8047ACC8->floorId);
            entry = lbl_8047ACC8->floorDataEntry;
            floorTeardownSubsystems(entry, 0);
            menuCloseFloor();
            fn_800D2B90(0);
            fn_8017B1CC(floorDataBiosGetFileGroupID(entry));
            fn_800F915C(floorDataBiosGetGroupID(entry));
            psRemoveParticle();
            psRemoveGenerator();
            psRemoveAppSRT();
            GSmodelFreeAllShadowTextures();
            depth = lbl_8047ACC4 + 1;
            lbl_8047ACC4 = depth;
            lbl_8047ACC8 = (GSFloorContext*)((u8*)lbl_8047ACA4 + depth * 0x14);
            lbl_8047ACC8->doFadeIn = 1;
            lbl_8047ACC8->doFadeOut = 0;
            lbl_8047ACC8->floorDataEntry = floorFindDataEntry(lbl_80478B18);
            lbl_8047ACC8->floorId = lbl_80478B18 + GSFLOOR_ID_BASE;
            lbl_8047ACD8 = GSFLOOR_STATE_LOADING;
            break;

        case GSFLOOR_STATE_FINALIZING:
            floorReleaseResources(lbl_8047ACB4, lbl_8047ACC8->floorId, lbl_8047ACB0, GSFLOOR_RES_FREE);
            floorReleaseResources(lbl_8047ACB8, lbl_8047ACC8->floorId, lbl_8047ACB0 + lbl_8047ACB4, GSFLOOR_RES_FREE);
            floorReleaseResources(lbl_8047ACBC, lbl_8047ACC8->floorId, lbl_8047ACB0 + lbl_8047ACB4 + lbl_8047ACB8, GSFLOOR_RES_FREE);
            floorReleaseResources(lbl_8047ACB4, lbl_8047ACC8->floorId, lbl_8047ACB0, GSFLOOR_RES_LOADED);
            floorReleaseResources(lbl_8047ACB8, lbl_8047ACC8->floorId, lbl_8047ACB0 + lbl_8047ACB4, GSFLOOR_RES_LOADED);
            floorReleaseResources(lbl_8047ACBC, lbl_8047ACC8->floorId, lbl_8047ACB0 + lbl_8047ACB4 + lbl_8047ACB8, GSFLOOR_RES_LOADED);
            entry = lbl_8047ACC8->floorDataEntry;
            floorTeardownSubsystems(entry, 1);
            fn_800F716C(lbl_8047ACC8->floorId);
            GSthreadTerminateGroup(lbl_8047ACC8->floorId);
            menuCloseFloor();
            fn_800D2B90(0);
            fn_8017B1CC(floorDataBiosGetFileGroupID(entry));
            fn_800F915C(floorDataBiosGetGroupID(entry));
            fn_8010D038();
            psRemoveParticle();
            psRemoveGenerator();
            psRemoveAppSRT();
            GSmodelFreeAllShadowTextures();
            depth = lbl_8047ACC4 - 1;
            lbl_8047ACC4 = depth;
            lbl_8047ACC8 = (GSFloorContext*)((u8*)lbl_8047ACA4 + depth * 0x14);
            lbl_8047ACC8->doFadeIn = 0;
            lbl_8047ACC8->doFadeOut = 1;
            entry = lbl_8047ACC8->floorDataEntry;
            fn_801123D4(entry, 0);
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
            _threadSwitch();
            _threadSwitch();
            memHandle = lbl_8047ACC8->resMemHandle;
            resType = *(u8*)lbl_8047ACC8->floorDataEntry >> 5;
            buf = fn_800E27B0(memHandle);
            if (buf != NULL) {
                handler = lbl_80404918;
                for (handlerCount = lbl_8047ACE0; handlerCount != 0;
                     handlerCount--, handler++) {
                    if (handler->typeId != resType) {
                        continue;
                    }
                    size = *(u32*)buf;
                    payload = (u8*)buf + 4;
                    ((GSFloorResIoFunc)handler->loadFunc)(payload, size);
                    buf = (u8*)payload + size;
                }
                fn_800E24B0(memHandle);
                fn_800E209C(memHandle);
            }
            floorSetResourcesBlocked(lbl_8047ACB4, lbl_8047ACC8->floorId, lbl_8047ACB0, GSFLOOR_RES_FREE, 0);
            floorSetResourcesBlocked(lbl_8047ACB4, lbl_8047ACC8->floorId, lbl_8047ACB0, GSFLOOR_RES_LOADED, 0);
            GSthreadUnblockGroup(lbl_8047ACC8->floorId);
            lbl_8047ACD8 = GSFLOOR_STATE_RUNNING;
            lbl_8047ACC8->isActive = 1;
            break;
        }
    }
}

/* 0x80100B24 | 0x720 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u32 fn_80100B24(GSFloorContext* ctx) {
    GSFloorResource* res;
    GSFloorResource* next;
    void* entry;

    switch (ctx->isActive) {
    case 1:
        entry = ctx->floorDataEntry;
        for (res = lbl_8047ACCC; res != NULL; res = next) {
            next = res->next;
            if (res->active != 1 || res->pending != 0) {
                continue;
            }
            if (res->status == 0) {
                ((GSFloorResInitFunc)res->callback)(entry, res->floorId);
            }
            if (res->status == 1) {
                u32 groupId = floorDataBiosGetGroupID(entry);

                res->textureHandle =
                    fn_800F7318(res->priority, res->callback, 0x4000, 0, 0, 4,
                                groupId, 0, 0, 0);
                res->modelHandle = fn_800F7108(res->textureHandle);
            }
        }
        ctx->isActive = 2;
        break;

    case 2:
        for (res = lbl_8047ACB0; res < lbl_8047ACB0 + lbl_8047ACB4; res++) {
            if (res->active == 0 || res->status != GSFLOOR_RES_LOADED ||
                res->pending != 0 || res->modelHandle == NULL) {
                continue;
            }
            if (GSthreadIsRunning((u32)res->modelHandle) != 0) {
                return 1;
            }
        }
        for (res = lbl_8047ACB0 + lbl_8047ACB4;
             res < lbl_8047ACB0 + lbl_8047ACB4 + lbl_8047ACB8; res++) {
            if (res->active == 0 || res->status != GSFLOOR_RES_LOADED ||
                res->pending != 0 || res->modelHandle == NULL) {
                continue;
            }
            GSthreadClose((u32)res->modelHandle);
            res->modelHandle = NULL;
        }
        ctx->isActive = 3;
        break;

    case 3:
        if (ctx->doFadeOut != 0) {
            floorSetResourcesBlocked(lbl_8047ACB4, ctx->floorId, lbl_8047ACB0,
                                     GSFLOOR_RES_FREE, 0);
            floorSetResourcesBlocked(lbl_8047ACB8, ctx->floorId,
                                     lbl_8047ACB0 + lbl_8047ACB4,
                                     GSFLOOR_RES_FREE, 0);
            floorSetResourcesBlocked(lbl_8047ACB4, ctx->floorId, lbl_8047ACB0,
                                     GSFLOOR_RES_LOADED, 0);
            floorSetResourcesBlocked(lbl_8047ACB8, ctx->floorId,
                                     lbl_8047ACB0 + lbl_8047ACB4,
                                     GSFLOOR_RES_LOADED, 0);
        }
        ctx->isActive = 4;
        fn_801127BC();
        break;

    case 4:
        entry = ctx->floorDataEntry;
        for (res = lbl_8047ACCC; res != NULL; res = next) {
            next = res->next;
            if (res->active != 3 || res->pending != 0) {
                continue;
            }
            if (res->status == 0) {
                ((GSFloorResInitFunc)res->callback)(entry, res->floorId);
            }
            if (res->status == 1) {
                u32 groupId = floorDataBiosGetGroupID(entry);

                res->textureHandle =
                    fn_800F7318(res->priority, res->callback, 0x4000, 0, 0, 4,
                                groupId, 0, 0, 0);
                res->modelHandle = fn_800F7108(res->textureHandle);
            }
        }
        if (ctx->isActive == 3) {
            ctx->isActive = 4;
        }
        break;

    case 5:
        entry = ctx->floorDataEntry;
        for (res = lbl_8047ACCC; res != NULL; res = next) {
            next = res->next;
            if (res->active != 3 || res->pending != 0 || res->status != 0) {
                continue;
            }
            ((GSFloorResInitFunc)res->callback)(entry, res->floorId);
        }
        break;

    case 6:
        fn_80112780();
        entry = ctx->floorDataEntry;
        for (res = lbl_8047ACCC; res != NULL; res = next) {
            next = res->next;
            if (res->active != 5 || res->pending != 0) {
                continue;
            }
            if (res->status == 0) {
                ((GSFloorResInitFunc)res->callback)(entry, res->floorId);
            }
            if (res->status == 1) {
                u32 groupId = floorDataBiosGetGroupID(entry);

                res->textureHandle =
                    fn_800F7318(res->priority, res->callback, 0x4000, 0, 0, 4,
                                groupId, 0, 0, 0);
                res->modelHandle = fn_800F7108(res->textureHandle);
            }
        }
        ctx->isActive = 6;
        break;

    default:
        for (res = lbl_8047ACB0 + lbl_8047ACB4 + lbl_8047ACB8;
             res < lbl_8047ACB0 + lbl_8047ACB4 + lbl_8047ACB8 + lbl_8047ACBC;
             res++) {
            if (res->active == 0 || res->status != GSFLOOR_RES_LOADED ||
                res->pending != 0 || res->modelHandle == NULL) {
                continue;
            }
            if (GSthreadIsRunning((u32)res->modelHandle) != 0) {
                return 1;
            }
        }
        for (res = lbl_8047ACB0 + lbl_8047ACB4 + lbl_8047ACB8;
             res < lbl_8047ACB0 + lbl_8047ACB4 + lbl_8047ACB8 + lbl_8047ACBC;
             res++) {
            if (res->active == 0 || res->status != GSFLOOR_RES_LOADED ||
                res->pending != 0 || res->modelHandle == NULL) {
                continue;
            }
            GSthreadClose((u32)res->modelHandle);
            res->modelHandle = NULL;
        }
        return 0;
    }

    return 1;
}
#pragma pop

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
void fn_801012E8(void* archive, u32 resourceArg, u32 callbackArg) {
    extern void* HSD_ArchiveGetPublicAddress(void*, const char*);
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
void fn_801013A0(void* archive, u32 resourceArg, u32 modelIndex,
                 u32 callbackArg) {
    extern void* HSD_ArchiveGetPublicAddress(void*, const char*);
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
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010147C(void) {
    /* TODO: match -- 1172 bytes at 0x8010147C */
}
#pragma pop
