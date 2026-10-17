/**
 * @file fsys_system_8017AAA4.c
 * @brief FSYS per-frame task, state query and subsystem init
 *        (0x8017AAA4 - 0x8017AF6C).
 *
 * fsysUpdateTask is the GSgapp task fn_8003708C registers right after
 * _fsysInitTOC: it polls the drive, reports fatal / no-disk / cover-open /
 * wrong-disk / retry states through the registered error callback, and
 * otherwise steps every load slot through the status dispatch table
 * lbl_8036C3E0 until each handler reports it is done for this frame.
 *
 * _fsysInitTOC allocates the slot array, the 100-entry file handle table
 * and the two 128 KiB DVD staging buffers, reads gsfsys.toc
 * (lbl_80273F70) whole into memory, then starts the load-request manager.
 *
 * The fsys code is built at optimisation level 0 with the peephole and
 * scheduling passes still on (see configure.py); the static inline helpers
 * below are the ones the target expands: fsysAlloc four times in
 * _fsysInitTOC, and the others leave their return values routed through
 * an extra register or stack temporary exactly as inlining does.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

typedef struct FSYSDispatchEntry {
    s32 status;
    s32 (*handler)(FSYSSlot* slot);
} FSYSDispatchEntry;

extern FSYSManager lbl_80453FEC;
extern void* lbl_8047B1B0;           /* gsfsys.toc image */
extern FSYSSlot* lbl_8047B1B4;       /* slot array */
extern FSYSFileHandle* lbl_8047B1B8; /* file handle table */
extern u32 lbl_8047B1BC;             /* file handle count */
extern void* lbl_8047B1C0[2];        /* DVD staging buffers */
extern s32 lbl_8047B1C8;             /* previous drive status */
extern FSYSDispatchEntry lbl_8036C3E0[];
extern const char lbl_80273F70[];    /* "gsfsys.toc" */

extern void* memset(void* dst, int value, u32 size);
extern char* strcpy(char* dst, const char* src);
extern void* DVDGetCurrentDiskID(void); /* DVDGetCurrentDiskID */
extern void* GSmemLock(u16 handle);
extern u16 GSmemAlloc(u32 size, u32 alignment);
extern s32 fn_80167E10(u32 fileInfo); /* DVDGetCommandBlockStatus wrapper */
extern s32 GSdvdGetDriveStatus(void);         /* DVDGetDriveStatus wrapper */
extern u32 GSdvdGetLength(u32 file);
extern void GSdvdClose(u32 file);
extern void GSdvdRead(u32 file, void* dst, u32 size, u32 offset);
extern u32 GSdvdOpen(const char* path);
extern void GSdvdInit(u32 count);
extern void fsysCacheInit(u32 queueCount, u32 arenaStart, u32 arenaSize);
extern void fn_8018094C(void);
extern void fn_80180B94(u32 count);

static inline s32 fsysGetDriveStatus(void)
{
    return GSdvdGetDriveStatus();
}

static inline s32 fsysUpdateSlot(FSYSSlot* slot)
{
    FSYSDispatchEntry* entry;
    s32 index;
    s32 result;

    result = 1;
    if (slot->archiveData && slot->fileInfo0) {
        slot->padding054 = fn_80167E10(slot->fileInfo0);
    }
    for (entry = lbl_8036C3E0, index = 0;; entry++, index++) {
        if (entry->status == slot->status) {
            result = entry->handler(slot);
            break;
        }
        if (entry->status < 0) {
            break;
        }
    }
    return result;
}

/* Address: 0x8017AAA4 | size: 0x18C */
void fsysUpdateTask(void)
{
    FSYSSlot* slot;
    u32 i;
    u8 stopped;
    s32 result;

    fn_8018094C();
    stopped = 0;
    lbl_8047B1C8 = lbl_80453FEC.driveStatus;
    lbl_80453FEC.driveStatus = fsysGetDriveStatus();
    switch (lbl_80453FEC.driveStatus) {
    case -1:
    case 4:
    case 5:
    case 6:
    case 9:
    case 10:
    case 11:
        if (lbl_80453FEC.errorCallback) {
            lbl_80453FEC.errorCallback(lbl_80453FEC.driveStatus,
                                       lbl_80453FEC.errorArg0,
                                       lbl_80453FEC.errorArg1);
        }
        stopped = 1;
        break;
    }

    if (!stopped) {
        slot = lbl_8047B1B4;
        for (i = 0; i < lbl_80453FEC.maxSlots; slot++, i++) {
            do {
                result = fsysUpdateSlot(slot);
            } while (result == 0);
        }
    }
}

/* Address: 0x8017AC30 | size: 0x10 */
u32 fn_8017AC30(void)
{
    return lbl_80453FEC.field_28;
}

static inline void* fsysAlloc(u32 size)
{
    u32 alignedSize;
    u16 handle;

    alignedSize = (size + 0x1F) & ~0x1F;
    handle = GSmemAlloc(alignedSize, 0x20);
    if (handle != 0) {
        return GSmemLock(handle);
    }
    return NULL;
}

static inline void* fsysLoadFile(const char* name)
{
    char path[0x80];
    u32 size;
    u32 file;
    void* buffer;

    strcpy(path, name);
    file = GSdvdOpen(path);
    size = GSdvdGetLength(file);
    buffer = fsysAlloc((size + 0x1F) & ~0x1F);
    GSdvdRead(file, buffer, (size + 0x1F) & ~0x1F, 0);
    GSdvdClose(file);
    return buffer;
}

/* Address: 0x8017AC40 | size: 0x32C */
s32 _fsysInitTOC(u32 numSlots, FSYSErrorCallback errorCallback, u32 errorArg0,
                 u32 errorArg1)
{
    s32 i;
    FSYSSlot* slot;
    /* Retail stores the disk ID pointer to the frame and never reads it. */
    void* diskID;

    lbl_80453FEC.driveStatus = 0;
    lbl_8047B1C8 = 0;
    lbl_80453FEC.field_08 = 0;
    lbl_80453FEC.maxSlots = numSlots;
    lbl_80453FEC.errorCallback = errorCallback;
    lbl_80453FEC.errorArg0 = errorArg0;
    lbl_80453FEC.errorArg1 = errorArg1;
    lbl_80453FEC.field_24 = 0;
    lbl_80453FEC.field_28 = 1;
    lbl_80453FEC.activeSlot = NULL;
    lbl_80453FEC.currentSlot = NULL;

    lbl_8047B1B4 = fsysAlloc(numSlots * sizeof(FSYSSlot));
    lbl_8047B1B8 = fsysAlloc(FSYS_MAX_HANDLES * sizeof(FSYSFileHandle));
    lbl_8047B1BC = 0;
    for (i = 0; i < FSYS_MAX_HANDLES; i++) {
        lbl_8047B1B8[i].handleID = -1;
        lbl_8047B1B8[i].userData = 0;
    }

    GSdvdInit(numSlots);
    diskID = DVDGetCurrentDiskID();
    slot = lbl_8047B1B4;
    for (i = 0; i < lbl_80453FEC.maxSlots; i++, slot++) {
        memset(slot, 0, sizeof(FSYSSlot));
        slot->archiveHandle = 0;
        slot->archiveData = NULL;
        slot->padding054 = 0;
        slot->status = 0;
        slot->reloadFlag = 0;
        slot->padding05C = 0;
        slot->refCount = 0;
        slot->fileInfo0 = 0;
        slot->tocBuffer = NULL;
    }

    for (i = 0; i < 2; i++) {
        lbl_8047B1C0[i] = fsysAlloc(0x20000);
    }

    lbl_8047B1B0 = fsysLoadFile(lbl_80273F70);

    fsysCacheInit(8, 0xA00000, 0x600000);
    fn_80180B94(100);
    return 1;
}
