/**
 * @file fsys_file_r52_8017D410_prefix.c
 * @brief FSYS slot lookup, handle-cache LRU and pending-load dispatch
 *        (0x8017D410 - 0x8017D960).
 *
 * fsysFindSlotByHandle finds the slot that owns a file handle (taking a reference
 * according to the request mode) or claims the first free slot;
 * fsysPreloadArchive (re)starts a load-mode-7 request for an archive through
 * fsysStartLoadMode7 unless it is already loaded in mode 2 or 7;
 * fsysFlushCachedHandles flushes every cached handle; fsysSlotRefreshHandle moves a slot's
 * archive key to the most-recently-used end of the 100-entry handle table;
 * fsysStartPendingLoad starts the first pending (status 500) slot with the loader
 * for its load mode; fsysSlotFinishA / fsysSlotFinishB finish a slot and free it.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): every function in this range
 * is exact with that single unit-wide flag and no local pragmas.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

extern FSYSManager lbl_80453FEC;
extern FSYSSlot* lbl_8047B1B4;       /* slot array */
extern FSYSFileHandle* lbl_8047B1B8; /* handle table, FSYS_MAX_HANDLES entries */
extern s32 lbl_8047B1BC;             /* handle table fill count */

extern void* memcpy(void* dst, const void* src, u32 size);
extern void fn_8017F800(s32 handleID);
extern void fn_8017D960(FSYSSlot* slot);
extern void fsysStartLoadMode1(FSYSSlot* slot, u32 fileHandle, u32 callbackA,
                        u32 callbackB, u32 callbackC);
extern void fsysStartLoadMode7(FSYSSlot* slot, u32 fileHandle, u32 callbackA,
                        u32 callbackB, u32 callbackC);
extern void fsysStartLoadMode2(FSYSSlot* slot, u32 fileHandle, u32 callbackA,
                        u32 callbackB, u32 callbackC);
extern void fsysStartLoadMode0(FSYSSlot* slot, u32 fileHandle, u32 callbackA,
                        u32 callbackB, u32 callbackC);

/* Address: 0x8017D410 | size: 0x15C */
FSYSSlot* fsysFindSlotByHandle(u32 fileHandle, s32 mode)
{
    u32 i;
    FSYSSlot* slot;

    slot = lbl_8047B1B4;
    for (i = 0; i < lbl_80453FEC.maxSlots; slot++, i++) {
        if (slot->status == 0 || slot->fileHandle != fileHandle) {
            continue;
        }

        if (slot->status != FSYS_STATUS_LOADED) {
            if (mode == 2 || mode == 7) {
                return NULL;
            }
            if ((s32)slot->loadMode == 3) {
                return slot;
            }
            slot->reloadFlag = 1;
            return slot;
        }

        switch (mode) {
        case 0:
            if ((s32)slot->loadMode != 2 && (s32)slot->loadMode != 7) {
                slot->refCount++;
            }
            break;
        case 2:
        case 7:
            if (slot->refCount == 0) {
                slot->refCount++;
            }
            break;
        }
        return slot;
    }

    slot = lbl_8047B1B4;
    for (i = 0; i < lbl_80453FEC.maxSlots; slot++, i++) {
        if (slot->status == 0) {
            slot->refCount = 0;
            slot->refCount++;
            return slot;
        }
    }
    return NULL;
}

/* Address: 0x8017D56C | size: 0xB8 */
s32 fsysPreloadArchive(u32 fileHandle)
{
    FSYSSlot* slot;

    slot = fsysFindSlotByHandle(fileHandle, 7);
    if (slot) {
        if (slot->status == FSYS_STATUS_LOADED) {
            if ((s32)slot->loadMode == 2 || (s32)slot->loadMode == 7) {
                return 0;
            }
            slot->status = FSYS_STATUS_FREE;
            fsysStartLoadMode7(slot, fileHandle, 0, 0, 0);
            return 1;
        }
        fsysStartLoadMode7(slot, fileHandle, 0, 0, 0);
        return 1;
    }
    return 0;
}

/* Address: 0x8017D624 | size: 0x68 */
void fsysFlushCachedHandles(void)
{
    FSYSFileHandle* table;
    s32 i;

    table = lbl_8047B1B8;
    for (i = 0; i < lbl_8047B1BC; i++) {
        fn_8017F800(table[i].handleID);
        table[i].handleID = -1;
    }
    lbl_8047B1BC = 0;
}

/* Address: 0x8017D68C | size: 0x174 */
FSYSFileHandle* fsysSlotRefreshHandle(FSYSSlot* slot)
{
    FSYSFileHandle* table;
    FSYSFileHandle saved;
    s32 foundIndex;
    s32 i;
    s32 handleID;

    foundIndex = -1;
    table = lbl_8047B1B8;
    handleID = slot->field_08;
    if (slot->field_10 & 0x40000000) {
        return NULL;
    }

    for (i = 0; i < FSYS_MAX_HANDLES; table++, i++) {
        if (table->handleID == handleID) {
            foundIndex = i;
            memcpy(&saved, table, sizeof(FSYSFileHandle));
            break;
        }
    }

    if (foundIndex >= 0) {
        table = lbl_8047B1B8;
        for (i = 0; i < lbl_8047B1BC - 1; i++) {
            if (i >= foundIndex) {
                table[i] = table[i + 1];
            }
        }
        table[i].handleID = saved.handleID;
        table[i].userData = saved.userData;
    } else {
        table = lbl_8047B1B8;
        if (lbl_8047B1BC == FSYS_MAX_HANDLES) {
            for (i = 0; i < FSYS_MAX_HANDLES - 1; i++) {
                table[i] = table[i + 1];
            }
            lbl_8047B1BC--;
        }
        table = &lbl_8047B1B8[lbl_8047B1BC++];
        table->handleID = handleID;
        table->userData = 0;
    }
    return table;
}

/* Address: 0x8017D800 | size: 0xF8 */
void fsysStartPendingLoad(void)
{
    u32 i;
    FSYSSlot* slot;

    slot = lbl_8047B1B4;
    for (i = 0; i < lbl_80453FEC.maxSlots; i++) {
        if (slot->status == FSYS_STATUS_PENDING) {
            switch (slot->loadMode) {
            case 0:
                fsysStartLoadMode0(slot, slot->fileHandle, slot->callbackA,
                            slot->callbackB, slot->callbackC);
                return;
            case 1:
                fsysStartLoadMode1(slot, slot->fileHandle, slot->callbackA,
                            slot->callbackB, slot->callbackC);
                return;
            case 2:
                fsysStartLoadMode2(slot, slot->fileHandle, slot->callbackA,
                            slot->callbackB, slot->callbackC);
                break;
            case 7:
                fsysStartLoadMode7(slot, slot->fileHandle, slot->callbackA,
                            slot->callbackB, slot->callbackC);
                break;
            }
        } else {
            slot++;
        }
    }
}

/* Address: 0x8017D8F8 | size: 0x34 */
void fsysSlotFinishA(FSYSSlot* slot)
{
    fn_8017D960(slot);
    slot->status = FSYS_STATUS_FREE;
}

/* Address: 0x8017D92C | size: 0x34 */
void fsysSlotFinishB(FSYSSlot* slot)
{
    fn_8017D960(slot);
    slot->status = FSYS_STATUS_FREE;
}
