/*
 * dolphin/dvd/dvd.h declares "u32 DVDGetCurrentDir(void);" which does not
 * match this unit's real two-argument DVDGetCurrentDir (buf, maxlen). That
 * header is shared and out of scope for this file, so shadow the name only
 * for the duration of the include and restore it before defining the real
 * function below.
 */
#define DVDGetCurrentDir DVDGetCurrentDir_stale_header_prototype
#include "dolphin/dvd/dvd.h"
#undef DVDGetCurrentDir
#include "dolphin/dvd/dvdfs_internal.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"

/* SDA-relative global used by the callback wait path in this unit */
extern OSThreadQueue __DVDThreadQueue;

/* Forward declarations for callbacks defined later in this file. */
void cbForReadAsync(s32 result, DVDFileInfo* fileInfo);
void cbForReadSync(s32 result, DVDCommandBlock* block);
void cbForSeekAsync(s32 result, DVDFileInfo* fileInfo);
BOOL DVDGetCurrentDir(char* path, u32 maxlen);

/* FST entry layout - mirrors DVDFs.c's DVDFstEntry. Duplicated locally
 * since the type isn't shared through a header; FstStart_8047A7CC is the
 * same global object defined (non-static) in DVDFs.c. */
typedef struct DVDFstEntry {
    u32 typeAndNameOffset;
    u32 parentOrStart;
    u32 nextOrLength;
} DVDFstEntry;

#define DVD_FST_TYPE_MASK 0xFF000000u

extern DVDFstEntry* FstStart_8047A7CC;
extern char* FstStringStart_8047A7D0;
extern char lbl_803118F0[];

/* Current-directory entry number (sda-relative global at 0x8047A7D8). */
extern u32 lbl_8047A7D8;
extern u32 __DVDLongFileNameFlag;

static u32 entryToPath(u32 entrynum, char* path, u32 maxlen);
extern s32 tolower(s32 ch);

#define entryIsDir(i)                                                         \
    (((FstStart_8047A7CC[i].typeAndNameOffset & DVD_FST_TYPE_MASK) == 0)      \
         ? FALSE                                                              \
         : TRUE)
#define stringOff(i)                                                          \
    (FstStart_8047A7CC[i].typeAndNameOffset & ~DVD_FST_TYPE_MASK)
#define parentDir(i) (FstStart_8047A7CC[i].parentOrStart)
#define nextDir(i) (FstStart_8047A7CC[i].nextOrLength)

static BOOL isSame(const char* path, const char* string) {
    while (*string != '\0') {
        if (tolower(*path++) != tolower(*string++)) {
            return FALSE;
        }
    }

    if ((*path == '/') || (*path == '\0')) {
        return TRUE;
    }

    return FALSE;
}

static u32 myStrncpy(char* dest, char* src, u32 maxlen) {
    u32 i = maxlen;

    while ((i > 0) && (*src != '\0')) {
        *dest++ = *src++;
        i--;
    }

    return maxlen - i;
}

s32 DVDConvertPathToEntrynum(const char* pathPtr) {
    const char* ptr;
    char* stringPtr;
    BOOL isDir;
    u32 length;
    u32 dirLookAt;
    u32 i;
    const char* origPathPtr = pathPtr;
    const char* extentionStart;
    BOOL illegal;
    BOOL extention;

    dirLookAt = lbl_8047A7D8;

    while (1) {
        if (*pathPtr == '\0') {
            return (s32)dirLookAt;
        } else if (*pathPtr == '/') {
            dirLookAt = 0;
            pathPtr++;
            continue;
        } else if (*pathPtr == '.') {
            if (*(pathPtr + 1) == '.') {
                if (*(pathPtr + 2) == '/') {
                    dirLookAt = parentDir(dirLookAt);
                    pathPtr += 3;
                    continue;
                } else if (*(pathPtr + 2) == '\0') {
                    return (s32)parentDir(dirLookAt);
                }
            } else if (*(pathPtr + 1) == '/') {
                pathPtr += 2;
                continue;
            } else if (*(pathPtr + 1) == '\0') {
                return (s32)dirLookAt;
            }
        }

        if (__DVDLongFileNameFlag == 0) {
            extention = FALSE;
            illegal = FALSE;

            for (ptr = pathPtr; (*ptr != '\0') && (*ptr != '/'); ptr++) {
                if (*ptr == '.') {
                    if ((ptr - pathPtr > 8) || (extention == TRUE)) {
                        illegal = TRUE;
                        break;
                    }
                    extention = TRUE;
                    extentionStart = ptr + 1;
                } else if (*ptr == ' ') {
                    illegal = TRUE;
                }
            }

            if ((extention == TRUE) && (ptr - extentionStart > 3)) {
                illegal = TRUE;
            }

            if (illegal) {
                fn_800060F0(lbl_804789C0, 0x17B, lbl_803118F0, origPathPtr);
            }
        } else {
            for (ptr = pathPtr; (*ptr != '\0') && (*ptr != '/'); ptr++) {
            }
        }

        isDir = (*ptr == '\0') ? FALSE : TRUE;
        length = (u32)(ptr - pathPtr);
        ptr = pathPtr;

        for (i = dirLookAt + 1; i < nextDir(dirLookAt);
             i = entryIsDir(i) ? nextDir(i) : (i + 1)) {
            if ((entryIsDir(i) == FALSE) && (isDir == TRUE)) {
                continue;
            }

            stringPtr = FstStringStart_8047A7D0 + stringOff(i);

            if (isSame(ptr, stringPtr) == TRUE) {
                goto next_hier;
            }
        }

        return -1;

    next_hier:
        if (!isDir) {
            return (s32)i;
        }

        dirLookAt = i;
        pathPtr += length + 1;
    }
}

BOOL DVDOpen(const char* path, DVDFileInfo* fileInfo) {
    extern char lbl_803119B8[];
    extern void OSReport(const char* fmt, ...);
    char cwd[0x80];
    s32 entrynum = DVDConvertPathToEntrynum(path);

    if (entrynum < 0) {
        DVDGetCurrentDir(cwd, sizeof(cwd));
        OSReport(lbl_803119B8, path, cwd);
        return FALSE;
    }

    {
        s32 isDir;

        if ((FstStart_8047A7CC[entrynum].typeAndNameOffset & DVD_FST_TYPE_MASK) == 0) {
            isDir = FALSE;
        } else {
            isDir = TRUE;
        }

        if (isDir != FALSE) {
            return FALSE;
        }
    }

    fileInfo->startAddr = FstStart_8047A7CC[entrynum].parentOrStart;
    fileInfo->length = FstStart_8047A7CC[entrynum].nextOrLength;
    fileInfo->callback = NULL;
    fileInfo->cb.state = 0;

    return TRUE;
}

BOOL DVDClose(DVDFileInfo* fileInfo) {
    DVDCancel(&fileInfo->cb);
    return TRUE;
}

static u32 entryToPath(u32 entry, char* path, u32 maxlen) {
    char* name;
    u32 loc;

    if (entry == 0) {
        return 0;
    }

    name = FstStringStart_8047A7D0 + stringOff(entry);
    loc = entryToPath(parentDir(entry), path, maxlen);

    if (loc == maxlen) {
        return loc;
    }

    *(path + loc++) = '/';
    loc += myStrncpy(path + loc, name, maxlen - loc);

    return loc;
}

static BOOL DVDConvertEntrynumToPath(s32 entrynum, char* path, u32 maxlen) {
    u32 loc;

    loc = entryToPath((u32)entrynum, path, maxlen);

    if (loc == maxlen) {
        path[maxlen - 1] = '\0';
        return FALSE;
    }

    if (entryIsDir(entrynum)) {
        if (loc == maxlen - 1) {
            path[loc] = '\0';
            return FALSE;
        }

        path[loc++] = '/';
    }

    path[loc] = '\0';
    return TRUE;
}

BOOL DVDGetCurrentDir(char* path, u32 maxlen) {
    return DVDConvertEntrynumToPath((s32)lbl_8047A7D8, path, maxlen);
}

BOOL DVDReadAsync(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset,
                  DVDCBCallback callback, s32 prio) {
    s32 end;

    if (offset < 0 || (u32)offset >= fileInfo->length) {
        fn_800060F0(lbl_804789C0, 0x2E6, lbl_803119F0);
    }

    end = offset + length;
    if (end < 0 || (u32)end >= fileInfo->length + 0x20) {
        fn_800060F0(lbl_804789C0, 0x2EC, lbl_803119F0);
    }

    fileInfo->callback = callback;
    DVDReadAbsAsyncPrio(&fileInfo->cb, addr, length, fileInfo->startAddr + offset,
                        (DVDCBCallback)cbForReadAsync, prio);
    return TRUE;
}

void cbForReadAsync(s32 result, DVDFileInfo* fileInfo) {
    if (fileInfo->callback != NULL) {
        fileInfo->callback(result, &fileInfo->cb);
    }
}

s32 DVDRead(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset, s32 prio) {
    DVDCommandBlock* block;
    BOOL enabled;
    s32 state;
    s32 result;
    s32 end;

    if (offset < 0 || (u32)offset >= fileInfo->length) {
        fn_800060F0(lbl_804789C0, 0x32C, lbl_80311A24);
    }

    end = offset + length;
    if (end < 0 || (u32)end >= fileInfo->length + 0x20) {
        fn_800060F0(lbl_804789C0, 0x332, lbl_80311A24);
    }

    block = &fileInfo->cb;
    if (!DVDReadAbsAsyncPrio(block, addr, length, fileInfo->startAddr + offset,
                             cbForReadSync, prio)) {
        return -1;
    }

    enabled = OSDisableInterrupts();
    for (;;) {
        state = ((volatile DVDCommandBlock*)block)->state;
        if (state == 0) {
            result = block->transferredSize;
            break;
        }
        if (state == -1) {
            result = -1;
            break;
        }
        if (state == 10) {
            result = -3;
            break;
        }
        OSSleepThread(&__DVDThreadQueue);
    }

    OSRestoreInterrupts(enabled);
    return result;
}

void cbForReadSync(s32 result, DVDCommandBlock* block) {
    OSWakeupThread(&__DVDThreadQueue);
}

BOOL DVDSeekAsyncPrio(DVDFileInfo* fileInfo, s32 offset, DVDCBCallback callback,
                      s32 prio) {
    extern BOOL DVDSeekAbsAsyncPrio(DVDCommandBlock* block, s32 offset,
                                    DVDCBCallback callback, s32 prio);

    if (offset < 0 || (u32)offset >= fileInfo->length) {
        fn_800060F0(lbl_804789C0, 0x383, lbl_80311A54);
    }

    fileInfo->callback = callback;
    DVDSeekAbsAsyncPrio(&fileInfo->cb, fileInfo->startAddr + offset,
                        (DVDCBCallback)cbForSeekAsync, prio);
    return TRUE;
}

void cbForSeekAsync(s32 result, DVDFileInfo* fileInfo) {
    if (fileInfo->callback != NULL) {
        fileInfo->callback(result, &fileInfo->cb);
    }
}

void defaultOptionalCommandChecker(DVDCommandBlock* command) {
    (void)command;
}
