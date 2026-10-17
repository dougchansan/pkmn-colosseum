/**
 * @file fsys_slot_8017B1CC.c
 * @brief FSYS archive slot queries and release (0x8017B1CC - 0x8017B4BC).
 *
 * Every function here resolves an archive by its file handle, either by
 * scanning the slot array itself (fsysFindSlot, expanded in both
 * GSresReleaseById and fn_8017B2CC) or through fsysFindSlotByHandle.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py).
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

extern FSYSManager lbl_80453FEC;
extern FSYSSlot* lbl_8047B1B4; /* slot array */

extern u16 GSmemGetHandleByPtr(void* ptr);      /* handle owning a heap pointer */
extern void fn_800E24B0(u16 handle);
extern void GSmemFree(u16 handle);
extern FSYSSlot* fsysFindSlotByHandle(u32 fileHandle, s32 mode);
extern void fsysStartLoadMode0(FSYSSlot* slot, u32 fileHandle, u32 callbackA,
                        u32 callbackB, u32 callbackC);

static inline FSYSSlot* fsysFindSlot(u32 fileHandle)
{
    FSYSSlot* slot;
    u32 i;

    slot = lbl_8047B1B4;
    for (i = 0; i < lbl_80453FEC.maxSlots; i++) {
        if (slot->status != 0) {
            if (slot->fileHandle == fileHandle) {
                return slot;
            } else {
                slot++;
            }
        } else {
            slot++;
        }
    }
    return NULL;
}

static inline void fsysFree(void* ptr)
{
    u16 handle;

    handle = GSmemGetHandleByPtr(ptr);
    if (handle != 0) {
        fn_800E24B0(handle);
        GSmemFree(handle);
    }
}

/* Address: 0x8017B1CC | size: 0x100
 * Drop one reference to a loaded archive; the last release frees the
 * archive image and returns the slot to the free state. */
void GSresReleaseById(u32 fileHandle)
{
    FSYSSlot* slot;

    slot = fsysFindSlot(fileHandle);
    if (!slot) {
        return;
    }
    if (slot->status != FSYS_STATUS_LOADED) {
        return;
    }
    if (!slot->archiveData) {
        return;
    }
    if (--slot->refCount > 0) {
        return;
    }
    fsysFree(slot->archiveData);
    slot->archiveData = NULL;
    slot->status = FSYS_STATUS_FREE;
    slot->padding05C = 0;
    slot->fileHandle = 0;
    slot->reloadFlag = 0;
    slot->loadMode = 0;
}

/* Address: 0x8017B2CC | size: 0xA4
 * 0 when the archive is loaded, 1 while it is still in flight, -1 when no
 * slot holds it. */
s32 fn_8017B2CC(u32 fileHandle)
{
    FSYSSlot* slot;

    slot = fsysFindSlot(fileHandle);
    if (slot) {
        if (slot->status == FSYS_STATUS_LOADED) {
            return 0;
        }
        return 1;
    }
    return -1;
}

/* Address: 0x8017B370 | size: 0x74 */
s32 fn_8017B370(u32 fileHandle)
{
    FSYSSlot* slot;

    slot = fsysFindSlotByHandle(fileHandle, 0);
    if (slot) {
        lbl_80453FEC.field_28 = 1;
        fsysStartLoadMode0(slot, fileHandle, 0, 0, 0);
        return 1;
    }
    return 0;
}

/* Address: 0x8017B3E4 | size: 0x64 */
s32 fn_8017B3E4(u32 fileHandle)
{
    FSYSSlot* slot;

    slot = fsysFindSlotByHandle(fileHandle, 0);
    if (slot) {
        fsysStartLoadMode0(slot, fileHandle, 0, 0, 0);
        return 1;
    }
    return 0;
}

static inline s32 fsysGetEntryCount(u32 fileHandle)
{
    FSYSSlot* slot;
    u32 i;
    FSYSArchiveHeader* archive;

    slot = lbl_8047B1B4;
    for (i = 0; i < lbl_80453FEC.maxSlots; slot++, i++) {
        if (slot->status != 0 && slot->fileHandle == fileHandle &&
            slot->padding05C != 0) {
            archive = slot->archiveData;
            return archive->numEntries;
        }
    }
    return -1;
}

/* Address: 0x8017B448 | size: 0x74
 * Entry count of the archive loaded for fileHandle, or -1. */
s32 fn_8017B448(u32 fileHandle)
{
    s32 count;

    count = fsysGetEntryCount(fileHandle);
    return count;
}
