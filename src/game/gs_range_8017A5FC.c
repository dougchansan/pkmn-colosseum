/**
 * @file gs_range_8017A5FC.c
 * @brief gs-engine code, 0x8017A5FC - 0x8017B07C (9 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

#include "game/fsys/fsys.h"

/* Address: 0x8017A5FC | size: 0x28 */
extern FSYSManager lbl_80453FEC;
void fn_8017A5FC(void);
#if !defined(GS_RANGE_8017A5FC_SPLIT) || defined(GS_RANGE_8017A5FC_PREFIX)
#pragma optimization_level 0
void fn_8017A5FC(void)
{
    FSYSSlot* slot;

    slot = lbl_80453FEC.activeSlot;
    slot->status = 0x12f;
}
#pragma optimization_level reset
#endif

/* Address: 0x8017A624 | size: 0x1F0 */
#if !defined(GS_RANGE_8017A5FC_SPLIT) || defined(GS_RANGE_8017A624_MIDDLE)
extern s32 fn_801808B4(void* req);
extern void* fn_801807A8(void* src, void* dst, u32 size);
extern u8 fn_80167E98(void* work, void* addr, s32 length, s32 offset,
                       void* callback);

s32 fn_8017A624(void* arg)
{
    FSYSSlot* slot;
    void* archiveData;
    void* dvdBuf;
    void* bufferPtr;
    u32 bit;

    slot = (FSYSSlot*)arg;
    archiveData = slot->archiveData;

    if (slot->dmaAsyncRequest != 0) {
        do {
        } while (fn_801808B4(slot->dmaAsyncRequest));
    }

    if (slot->dmaCopyDst != 0) {
        slot->dmaAsyncRequest = fn_801807A8(
            gFSYSDVDBuffers[lbl_80453FEC.field_24], slot->dmaCopyDst,
            slot->dmaChunkSize);
    } else {
        slot->dmaAsyncRequest = 0;
    }

    slot->dmaBytesRemaining -= 0x20000;
    if (slot->dmaBytesRemaining > 0) {
        lbl_80453FEC.field_24 ^= 1;
        dvdBuf = gFSYSDVDBuffers[lbl_80453FEC.field_24];

        if (slot->dmaBytesRemaining < 0x20000) {
            slot->dmaChunkSize = (slot->dmaBytesRemaining + 0x1F) & ~0x1F;
        } else {
            slot->dmaChunkSize = 0x20000;
        }

        slot->dmaSrcOffset += 0x20000;
        slot->dmaDstOffset += 0x20000;
        if (slot->dmaCopyDst != 0) {
            slot->dmaCopyDst = (void*)((u8*)slot->dmaCopyDst + 0x20000);
        }
        slot->status = 0x12e;

        bit = *(u32*)((u8*)archiveData + 0x10) & 1;
        if (bit && slot->tocBuffer != 0) {
            bufferPtr = slot->tocBuffer;
        } else {
            bufferPtr = (void*)slot->fileInfo0;
        }
        fn_80167E98(bufferPtr, dvdBuf, slot->dmaChunkSize, slot->dmaDstOffset,
                    fn_8017A5FC);
        return 0;
    }

    if (slot->fileInfo0 != 0 && slot->callbackA != 0) {
        ((void (*)(u32, u32, u32))slot->callbackA)(
            slot->loadMode, slot->callbackB, slot->callbackC);
    }
    return 1;
}
#endif

/* Address: 0x8017A814 | size: 0x148 */
#if !defined(GS_RANGE_8017A5FC_SPLIT) || defined(GS_RANGE_8017A814_SUFFIX)
extern u32 OSDisableInterrupts(void);
extern void OSRestoreInterrupts(u32 level);
extern void fn_80167E64(u32 fileInfo);

void fn_8017A814(void* unused0, void* unused1)
{
    u32 enabled;
    FSYSSlot* slot;

    enabled = OSDisableInterrupts();
    slot = lbl_80453FEC.activeSlot;

    switch (slot->status) {
    case 1:
        slot->status = 2;
        break;
    case 3:
        slot->status = 4;
        break;
    case 0x65:
        slot->status = 0x96;
        break;
    case 0xC8:
        slot->status = 0xC9;
        break;
    case 0x12F:
        slot->status = 0x130;
        break;
    case 0x190:
        slot->status = 0x191;
        break;
    case 2:
    case 4:
    case 0x64:
    case 0xC9:
    case 0x12D:
        break;
    default:
        slot->status = 0x3E8;
        slot->archiveHandle = 1;
        break;
    }

    if (slot->tocBuffer != NULL) {
        fn_80167E64((u32)slot->tocBuffer);
        slot->tocBuffer = NULL;
    }
    OSRestoreInterrupts(enabled);
}

/* Address: 0x8017A95C | size: 0x148 */
void fn_8017A95C(void* unused0, void* unused1)
{
    u32 enabled;
    FSYSSlot* slot;

    enabled = OSDisableInterrupts();
    slot = lbl_80453FEC.activeSlot;

    switch (slot->status) {
    case 1:
        slot->status = 2;
        break;
    case 3:
        slot->status = 4;
        break;
    case 0x65:
        slot->status = 0x96;
        break;
    case 0xC8:
        slot->status = 0xC9;
        break;
    case 0x12F:
        slot->status = 0x130;
        break;
    case 0x190:
        slot->status = 0x191;
        break;
    case 2:
    case 4:
    case 0x64:
    case 0xC9:
    case 0x12D:
        break;
    default:
        slot->status = 0x3E8;
        slot->archiveHandle = 1;
        break;
    }

    if (slot->tocBuffer != NULL) {
        fn_80167E64((u32)slot->tocBuffer);
        slot->tocBuffer = NULL;
    }
    OSRestoreInterrupts(enabled);
}

/* Address: 0x8017AAA4 | size: 0x18C */
typedef struct FSYSDispatchEntry {
    s32 status;
    s32 (*handler)(FSYSSlot* slot);
} FSYSDispatchEntry;

extern void fn_8018094C(void);
extern s32 fn_80167E34(void);
extern s32 fn_80167E10(void* handle);
extern u32 lbl_8047B1C8;
extern FSYSDispatchEntry lbl_8036C3E0[];

void fn_8017AAA4(void)
{
    FSYSSlot* slot;
    FSYSDispatchEntry* entry;
    s32 result;
    u32 i;
    u32 done;
    u32 doDispatch;
    s32 v;

    fn_8018094C();
    done = 0;
    lbl_8047B1C8 = lbl_80453FEC.numEntries;

    lbl_80453FEC.numEntries = fn_80167E34();
    v = (s32)lbl_80453FEC.numEntries;
    doDispatch = (v == -1) || (v >= 4 && v <= 6) || (v >= 9 && v <= 11);

    if (doDispatch) {
        if (lbl_80453FEC.tocDataPtr != 0) {
            ((void (*)(u32, u32, u32))lbl_80453FEC.tocDataPtr)(
                lbl_80453FEC.numEntries, lbl_80453FEC.field_14,
                lbl_80453FEC.field_18);
        }
        done = 1;
    }

    if (!done) {
        slot = gFSYSSlots;
        for (i = 0; i < lbl_80453FEC.maxSlots; i++, slot++) {
        retry:
            result = 1;
            if (slot->archiveData != 0 && slot->fileInfo0 != 0) {
                slot->padding054 = fn_80167E10((void*)slot->fileInfo0);
            }

            entry = lbl_8036C3E0;
            for (;;) {
                if (entry->status == (s32)slot->status) {
                    result = entry->handler(slot);
                    break;
                }
                if (entry->status < 0) {
                    break;
                }
                entry++;
            }

            if (result == 0) {
                goto retry;
            }
        }
    }
}

u32 fn_8017AC30(void)
{
    return lbl_80453FEC.field_28;
}

s32 _fsysInitTOC(u32 numSlots, u32 tocDataPtr, u32 callbackArg0,
                 u32 callbackArg1)
{
    extern u16 fn_800E2C04(u32 size, u32 alignment);
    extern void* fn_800E27B0(u16 handle);
    extern void fn_80167FA8(u32 count);
    extern void fn_800A7BCC(void);
    extern u32 fn_80167F28(const char* path);
    extern u32 fn_80167E5C(u32 file);
    extern void fn_80167ED0(u32 file, void* destination, u32 size,
                            u32 offset);
    extern void fn_80167E64(u32 file);
    extern void fn_801800F8(u32 queueCount, u32 arenaStart,
                            u32 arenaSize);
    extern void fn_80180B94(u32 count);
    extern char* strcpy(char* destination, const char* source);
    extern const char lbl_80273F70[];
    FSYSSlot* slot;
    u16 handle;
    u32 allocationSize;
    u32 tocSize;
    u32 file;
    s32 i;
    char path[0x80];

    lbl_80453FEC.maxSlots = numSlots;
    lbl_80453FEC.field_08 = 0;
    lbl_80453FEC.numEntries = 0;
    lbl_80453FEC.tocDataPtr = tocDataPtr;
    lbl_80453FEC.field_14 = callbackArg0;
    lbl_80453FEC.field_18 = callbackArg1;
    lbl_80453FEC.activeSlot = NULL;
    lbl_80453FEC.currentSlot = NULL;
    lbl_80453FEC.field_24 = 0;
    lbl_80453FEC.field_28 = 1;
    lbl_8047B1C8 = 0;

    allocationSize = (numSlots * sizeof(FSYSSlot) + 0x1F) & ~0x1F;
    handle = fn_800E2C04(allocationSize, 0x20);
    gFSYSSlots = handle != 0 ? fn_800E27B0(handle) : NULL;

    handle = fn_800E2C04(FSYS_MAX_HANDLES * sizeof(FSYSFileHandle), 0x20);
    gFSYSHandleTable = handle != 0 ? fn_800E27B0(handle) : NULL;
    gFSYSHandleCount = 0;
    for (i = 0; i < FSYS_MAX_HANDLES; i++) {
        gFSYSHandleTable[i].handleID = -1;
        gFSYSHandleTable[i].userData = 0;
    }

    fn_80167FA8(numSlots);
    fn_800A7BCC();
    slot = gFSYSSlots;
    for (i = 0; (u32)i < numSlots; i++, slot++) {
        memset(slot, 0, sizeof(FSYSSlot));
        slot->archiveData = NULL;
        slot->archiveHandle = 0;
        slot->reloadFlag = 0;
        slot->padding054 = 0;
        slot->padding05C = 0;
        slot->refCount = 0;
        slot->fileInfo0 = 0;
        slot->tocBuffer = NULL;
    }

    for (i = 0; i < 2; i++) {
        handle = fn_800E2C04(0x20000, 0x20);
        gFSYSDVDBuffers[i] = handle != 0 ? fn_800E27B0(handle) : NULL;
    }

    strcpy(path, lbl_80273F70);
    file = fn_80167F28(path);
    tocSize = fn_80167E5C(file);
    allocationSize = (tocSize + 0x1F) & ~0x1F;
    handle = fn_800E2C04(allocationSize, 0x20);
    gFSYSTocData = handle != 0 ? fn_800E27B0(handle) : NULL;
    fn_80167ED0(file, gFSYSTocData, allocationSize, 0);
    fn_80167E64(file);

    fn_801800F8(8, 0xA00000, 0x600000);
    fn_80180B94(100);
    return 1;
}

extern FSYSSlot* fn_8017D410(u32 fileHandle, u32 mode);
extern u8 fn_8017E30C(FSYSSlot* slot);

#pragma optimization_level 0
s32 fn_8017AF6C(u32 fileHandle, u32 requestID)
{
    FSYSSlot* slot;

    slot = fn_8017D410(fileHandle, 3);
    if (slot->fileHandle == fileHandle) {
        slot->requestID = requestID;
        slot->callbackA = 0;
        slot->callbackB = 0;
        slot->callbackC = 0;
        if (fn_8017E30C(slot)) {
            return 1;
        }
    } else {
        return 0;
    }
    return 0;
}
#pragma optimization_level reset

extern void fn_8017E1D8(FSYSSlot* slot, u32 fileHandle, u32 callbackA,
                         u32 callbackB, u32 callbackC);

#pragma optimization_level 0
s32 fn_8017B000(u32 fileHandle, u32 requestID, u32 callbackA, u32 callbackB,
                u32 callbackC)
{
    FSYSSlot* slot;

    slot = fn_8017D410(fileHandle, 3);
    if (slot != 0) {
        slot->requestID = requestID;
        fn_8017E1D8(slot, fileHandle, callbackA, callbackB, callbackC);
        return 1;
    }
    return 0;
}
#pragma optimization_level reset
#endif
