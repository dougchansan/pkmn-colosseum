/**
 * @file fsys_file_candidate_8017DEA4.c
 * @brief FSYS per-load-mode request starters (0x8017DEA4 - 0x8017E30C).
 *
 * Each function registers the request with _fsysGetFilename (which fills
 * the slot, builds its filename and, when the drive is free, makes the slot
 * the manager's active slot) and, if this slot became active, opens the
 * archive file and starts the first read:
 *   fsysStartLoadMode1  mode 1: GSdvdSeekAsync on the open file (or fail the slot);
 *   fsysStartLoadMode7  mode 7, fsysStartLoadMode2 mode 2: read the 0x40-byte header
 *                into the slot;
 *   fsysStartLoadMode0  mode 0, fsysStartLoadMode3 mode 3: unless a reload was requested,
 *                copy the header from the ARAM cache (fn_8017F794) or read
 *                it from disc.
 * Every read completes through fsysDVDReadCallback.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): every function in this range
 * is exact with that single unit-wide flag and no local pragmas.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

extern FSYSManager lbl_80453FEC;

extern void DCFlushRange(void* addr, u32 nBytes);
extern u32 GSdvdOpen(const char* path);
extern void GSdvdSeekAsync(u32 fileInfo, u32 offset, void (*callback)(s32 result));
extern void GSdvdReadAsync(u32 fileInfo, void* dst, u32 length, u32 offset,
                        void (*callback)(s32 result));
extern void fsysSlotFinishB(FSYSSlot* slot);
extern void fsysDVDReadCallback(s32 result);
extern u32 fn_8017F794(u32 fileHandle, u32 key1, u32 key2);
extern void fn_80180320(void* main, void* aram, u32 size);

/* Address: 0x8017DEA4 | size: 0xA8 */
void fsysStartLoadMode1(FSYSSlot* slot, u32 fileHandle, u32 callbackA, u32 callbackB,
                 u32 callbackC)
{
    _fsysGetFilename(slot, fileHandle, callbackA, callbackB, callbackC, 1);
    if (lbl_80453FEC.activeSlot == slot) {
        if (slot->fileInfo0 == 0) {
            slot->fileInfo0 = GSdvdOpen(slot->filename);
        }
        if (slot->fileInfo0 != 0) {
            GSdvdSeekAsync(slot->fileInfo0, 0, fsysDVDReadCallback);
        } else {
            fsysSlotFinishB(slot);
        }
    }
}

/* Address: 0x8017DF4C | size: 0xA8 */
void fsysStartLoadMode7(FSYSSlot* slot, u32 fileHandle, u32 callbackA, u32 callbackB,
                 u32 callbackC)
{
    u32 size;

    _fsysGetFilename(slot, fileHandle, callbackA, callbackB, callbackC, 7);
    if (lbl_80453FEC.activeSlot == slot) {
        if (slot->fileInfo0 == 0) {
            slot->fileInfo0 = GSdvdOpen(slot->filename);
        }
        size = 0x40;
        GSdvdReadAsync(slot->fileInfo0, slot, (size + 0x1F) & ~0x1F, 0, fsysDVDReadCallback);
    }
}

/* Address: 0x8017DFF4 | size: 0xA8 */
void fsysStartLoadMode2(FSYSSlot* slot, u32 fileHandle, u32 callbackA, u32 callbackB,
                 u32 callbackC)
{
    u32 size;

    _fsysGetFilename(slot, fileHandle, callbackA, callbackB, callbackC, 2);
    if (lbl_80453FEC.activeSlot == slot) {
        if (slot->fileInfo0 == 0) {
            slot->fileInfo0 = GSdvdOpen(slot->filename);
        }
        size = 0x40;
        GSdvdReadAsync(slot->fileInfo0, slot, (size + 0x1F) & ~0x1F, 0, fsysDVDReadCallback);
    }
}

/* Address: 0x8017E09C | size: 0x13C */
void fsysStartLoadMode0(FSYSSlot* slot, u32 fileHandle, u32 callbackA, u32 callbackB,
                 u32 callbackC)
{
    u32 size;
    u8 readFromCache;
    u32 cached;

    readFromCache = 0;
    _fsysGetFilename(slot, fileHandle, callbackA, callbackB, callbackC, 0);
    if (lbl_80453FEC.activeSlot == slot) {
        if ((s32)slot->reloadFlag == 1) {
            if (slot->fileInfo0 == 0) {
                slot->fileInfo0 = GSdvdOpen(slot->filename);
            }
            return;
        }
        if (slot->fileInfo0 == 0) {
            slot->fileInfo0 = GSdvdOpen(slot->filename);
        }
        size = 0x40;
        cached = fn_8017F794(slot->fileHandle, 0, 0);
        if (cached != 0) {
            fn_80180320(slot, (void*)cached, (size + 0x1F) & ~0x1F);
            DCFlushRange(slot, (size + 0x1F) & ~0x1F);
            slot->status = 2;
            readFromCache = 1;
        }
        /* Retail tests the file handle here and never branches on the
         * result (0x8017E188: lwz r0,0x68(r31); cmplwi r0,0x0, then the
         * clrlwi. of readFromCache overwrites cr0). At optimisation level 0
         * that is what an if whose body compiled to nothing leaves behind,
         * e.g. a debug report stripped from the release build; fsysStartLoadMode3,
         * otherwise identical, has no such test. */
        if (slot->fileInfo0 != 0) {
            (void)0;
        }
        if (!readFromCache) {
            GSdvdReadAsync(slot->fileInfo0, slot, (size + 0x1F) & ~0x1F, 0, fsysDVDReadCallback);
        }
    }
}

/* Address: 0x8017E1D8 | size: 0x134 */
void fsysStartLoadMode3(FSYSSlot* slot, u32 fileHandle, u32 callbackA, u32 callbackB,
                 u32 callbackC)
{
    u32 size;
    u8 readFromCache;
    u32 cached;

    readFromCache = 0;
    _fsysGetFilename(slot, fileHandle, callbackA, callbackB, callbackC, 3);
    if (lbl_80453FEC.activeSlot == slot) {
        if ((s32)slot->reloadFlag == 1) {
            if (slot->fileInfo0 == 0) {
                slot->fileInfo0 = GSdvdOpen(slot->filename);
            }
            return;
        }
        if (slot->fileInfo0 == 0) {
            slot->fileInfo0 = GSdvdOpen(slot->filename);
        }
        size = 0x40;
        cached = fn_8017F794(slot->fileHandle, 0, 0);
        if (cached != 0) {
            fn_80180320(slot, (void*)cached, (size + 0x1F) & ~0x1F);
            DCFlushRange(slot, (size + 0x1F) & ~0x1F);
            slot->status = 2;
            readFromCache = 1;
        }
        if (!readFromCache) {
            GSdvdReadAsync(slot->fileInfo0, slot, (size + 0x1F) & ~0x1F, 0, fsysDVDReadCallback);
        }
    }
}
