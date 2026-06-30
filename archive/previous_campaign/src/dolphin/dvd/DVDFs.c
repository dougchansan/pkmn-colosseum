#include "dolphin/dvd/dvd.h"

/*
 * DVDFs.c - DVD filesystem initialization.
 *
 * Reads the FST (File System Table) pointer from the disc header
 * and sets up the string table pointer and entry count.
 *
 * Matches: 0x800A4CF0 | size: 0x38
 */

/* Boot info / disc header at 0x80000000 */
#define BOOT_INFO       ((u32*)0x80000000)

/*
 * SDA-relative globals. The target's data symbols carry an address suffix
 * (e.g. FstStart_8047A7CC); the stub functions below already reference them
 * by that name, so the definitions use the suffixed names to match.
 */
static u32* BootInfo_8047A7C8;       /* 0x8047A7C8 */
static u32* FstStart_8047A7CC;       /* 0x8047A7CC */
static u32* FstStringStart_8047A7D0; /* 0x8047A7D0 */
static u32  MaxEntryNum_8047A7D4;    /* 0x8047A7D4 */

extern u32 __DVDLongFileNameFlag;

typedef struct DVDFstEntry {
    u32 typeAndNameOffset;
    u32 parentOrStart;
    u32 nextOrLength;
} DVDFstEntry;

#define DVD_FST_TYPE_MASK 0xFF000000u
#define DVD_FST_ENTRY(entrynum) (((DVDFstEntry*)FstStart_8047A7CC)[(entrynum)])
#define DVD_FST_WORD(entrynum, word) (FstStart_8047A7CC[(entrynum) * 3 + (word)])
#define DVD_FST_TYPE_NAME(entrynum) DVD_FST_WORD((entrynum), 0)
#define DVD_FST_PARENT_OR_START(entrynum) DVD_FST_WORD((entrynum), 1)
#define DVD_FST_NEXT_OR_LENGTH(entrynum) DVD_FST_WORD((entrynum), 2)
#define DVD_FST_NAME(entrynum) \
    ((char*)FstStringStart_8047A7D0 + (DVD_FST_TYPE_NAME(entrynum) & 0xFFFFFF))

/*
 * __DVDFSInit - Initialize the DVD filesystem
 * 0x800A4CF0 | size: 0x38
 *
 * Reads the FST location from offset 0x38 in the boot info,
 * then extracts the root entry count and string table offset.
 *
 * FST entry format (12 bytes each):
 *   word 0: flags/name_offset
 *   word 1: file_offset or parent_dir
 *   word 2: file_length or num_entries (for root: total entries)
 */
void __DVDFSInit(void) {
    BootInfo_8047A7C8 = BOOT_INFO;

    /* FST start address is at offset 0x38 in boot info */
    FstStart_8047A7CC = (u32*)BootInfo_8047A7C8[0x38 / 4];

    if (FstStart_8047A7CC) {
        /* Root entry's third word contains the total number of entries */
        MaxEntryNum_8047A7D4 = DVD_FST_ENTRY(0).nextOrLength;

        /* String table immediately follows the FST entries */
        FstStringStart_8047A7D0 = (u32*)&DVD_FST_ENTRY(MaxEntryNum_8047A7D4);
    }
}

/* ========================================================== */
/* Stub functions for coverage - TODO: decompile              */
/* ========================================================== */

/* fn_800A4D28 - 0x800A4D28 | size: 0x2F4 */
s32 fn_800A4D28() {
    extern u8 lbl_803118F0[];
    extern u8 lbl_804789C0[];
    extern u32 lbl_8047A7D8;
    extern void fn_800060F0();
    extern void fn_800C7558();
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r23 = r3;
    r3 = (u32)lbl_803118F0;
    r25 = r23 + 0x0;
    r31 = (u32)lbl_803118F0;
    r26 = lbl_8047A7D8;
while (1) {
        r3 = *(u8*)((u8*)r23 + 0x0);
        tmp = (s8)r3;
        if ((s32)tmp == 0) {
            r3 = r26;
            return;
        }
        tmp = (s8)r3;
        if ((s32)tmp == 0x2f) {
            r26 = 0x0;
            r23 = r23 + 0x1;
            continue;
        }
        do {
            if ((s32)tmp != 0x2e) break;
            r3 = *(u8*)((u8*)r23 + 0x1);
            tmp = (s8)r3;
            if ((s32)tmp == 0x2e) {
                r3 = *(u8*)((u8*)r23 + 0x2);
                if ((s32)r3 == 0x2f) {
                    r3 = r26 * 0xc;
                    r4 = *(u32*)FstStart_8047A7CC;
                    tmp = r3 + 0x4;
                    r26 = *(u32*)(r4 + tmp);
                    r23 = r23 + 0x3;
                    continue;
                }
                tmp = (s8)r3;
                if ((s32)r3 != 0x2f) break;
                tmp = r26 * 0xc;
                r3 = *(u32*)FstStart_8047A7CC;
                r3 = r3 + tmp;
                r3 = *(u32*)((u8*)r3 + 0x4);
                return;
            }
            if ((s32)tmp == 0x2f) {
                r23 = r23 + 0x2;
                continue;
            }
            tmp = (s8)r3;
            if ((s32)tmp != 0x2f) break;
            r3 = r26;
            return;
        } while (0);

        tmp = *(u32*)__DVDLongFileNameFlag;
        if (tmp == 0) {
            r28 = r23 + 0x0;
            r5 = 0x0;
            r4 = 0x0;
            while ((s32)tmp != 0x2f) {
                    r3 = *(u8*)((u8*)r28 + 0x0);
                    tmp = (s8)r3;
                    if ((s32)tmp == 0x20) break;
                    tmp = (s8)r3;
                    tmp = (s8)r3;
                    if ((s32)tmp == 0x2e) {
                        tmp = r28 - r23;
                        if ((s32)tmp <= 8) {
                            if ((s32)r5 == 1) {
                            }
                            r4 = 0x1;
                            break;
                            }
                        r24 = r28 + 0x1;
                        r5 = 0x1;

                    } else {
                        if ((s32)tmp == 0x20) {
                            r4 = 0x1;
                        }
                        }
                    r28 = r28 + 0x1;
            }

            if ((s32)r5 == 1) {
                tmp = r28 - r24;
                if ((s32)tmp > 3) {
                    r4 = 0x1;
            }
            }
            if ((s32)r4 != 0) {
                r5 = r31 + 0x0;
                r6 = r25 + 0x0;
                r3 = (u32)lbl_804789C0;
                r4 = 0x17b;
                fn_800060F0();

            } else {
                r28 = r23;
                while ((s32)tmp != 0x2f) {
                        r3 = *(u8*)((u8*)r28 + 0x0);
                        tmp = (s8)r3;
                        if ((s32)r4 == 0) break;
                        tmp = (s8)r3;
                        r28 = r28 + 0x1;
                }
            }
            }
        tmp = *(u8*)((u8*)r28 + 0x0);
        tmp = (s8)tmp;
        if ((s32)tmp == 0x2f) {
            r30 = 0x0;
        } else {

            r30 = 0x1;
        }
        r29 = r26 * 0xc;
        r27 = r28 - r23;
        r26 = r26 + 0x1;
        while (1) {
            r3 = *(u32*)FstStart_8047A7CC;
            tmp = r3 + 0x8;
            tmp = *(u32*)(r29 + tmp);
            if (r26 >= tmp) break;
            r28 = r26 * 0xc;
            r4 = *(u32*)(r3 + r28);
            /* clrrwi. tmp, r4, 24 */;
            if ((s32)tmp == 0x2f) {
                tmp = 0x0;
            } else {

                tmp = 0x1;
            }
            if ((s32)tmp == 0) {
                if ((s32)r30 != 1) {
                }
                r3 = *(u32*)FstStringStart_8047A7D0;
                tmp = r4 & 0xFFFFFF;
                r21 = r23 + 0x0;
                r20 = r3 + tmp;
                while ((s32)r3 != (s32)r22) {
                        tmp = *(u8*)((u8*)r20 + 0x0);
                        tmp = (s8)tmp;
                        tmp = *(u8*)((u8*)r20 + 0x0);
                        r20 = r20 + 0x1;
                        r3 = (s8)tmp;
                        fn_800C7558();
                        tmp = *(u8*)((u8*)r21 + 0x0);
                        r22 = r3 + 0x0;
                        r21 = r21 + 0x1;
                        r3 = (s8)tmp;
                        fn_800C7558();
                        if ((s32)r3 != (s32)r22) {
                            tmp = 0x0;

                        } else {
                    }
                    r3 = *(u8*)((u8*)r21 + 0x0);
                    if ((s32)r3 != 0x2f) {
                        tmp = (s8)r3;
                        if ((s32)r3 == 0x2f) {
                        }
                        tmp = 0x1;

                        } else {
                        tmp = 0x0;
                            }
                        }

                if ((s32)tmp != 1) {
                    }
                tmp = *(u32*)FstStart_8047A7CC;
                r3 = tmp + r28;
                tmp = *(u32*)((u8*)r3 + 0x0);
                /* clrrwi. tmp, tmp, 24 */;
                if ((s32)tmp == 1) {
                    tmp = 0x0;
                } else {

                    tmp = 0x1;
                }
                if ((s32)tmp != 0) {
                    tmp = *(u32*)((u8*)r3 + 0x8);
                } else {

                    tmp = r26 + 0x1;
                }
                r26 = tmp;

            }
            r3 = -0x1;
            return;
                }
        if ((s32)r30 == 0) {
            r3 = r26;
            return;
        }
        r23 = r27 + r23;
        r23 = r23 + 0x1;
        continue;
}

    return;
}

/* fn_800A501C - 0x800A501C | size: 0xC8 */
BOOL fn_800A501C(const char* path, DVDFileInfo* fileInfo) {
    extern char lbl_803119B8[];
    extern void OSReport(const char* fmt, ...);
    char cwd[0x80];
    s32 entrynum = fn_800A4D28(path);

    if (entrynum < 0) {
        fn_800A5268(cwd, sizeof(cwd));
        OSReport(lbl_803119B8, path, cwd);
        return FALSE;
    }

    {
        s32 isDir;

        if ((DVD_FST_TYPE_NAME(entrynum) & DVD_FST_TYPE_MASK) == 0) {
            isDir = FALSE;
        } else {
            isDir = TRUE;
        }

        if (isDir != FALSE) {
            return FALSE;
        }
    }

    fileInfo->startAddr = DVD_FST_PARENT_OR_START(entrynum);
    fileInfo->length = DVD_FST_NEXT_OR_LENGTH(entrynum);
    fileInfo->callback = NULL;
    fileInfo->cb.state = 0;

    return TRUE;
}

/* fn_800A50E4 - 0x800A50E4 | size: 0x24
 * DVDClose - cancel any pending command on the file, then report success.
 */
int fn_800A50E4(DVDFileInfo* fileInfo) {
    DVDCancel(&fileInfo->cb);
    return 1;
}

/* fn_800A5108 - 0x800A5108 | size: 0x160 */
u32 fn_800A5108(u32 entrynum, char* path, u32 maxlen) {
    char* name;
    char* parentName;
    u32 parent;
    u32 len;
    u32 remaining;
    u32 limit;
    u32 slash;
    char* dst;
    char* src;
    /* Preserves the original 0x30-byte frame without emitted accesses. */
    volatile u32 framePad[2];

    if (entrynum == 0) {
        return 0;
    }

    name = DVD_FST_NAME(entrynum);
    parent = DVD_FST_PARENT_OR_START(entrynum);

    if (parent != 0) {
        goto hasParent;
    }

    len = 0;
    goto afterParent;

hasParent:
    parentName = DVD_FST_NAME(parent);
    len = fn_800A5108(DVD_FST_PARENT_OR_START(parent), path, maxlen);

    if (len != maxlen) {
    } else {
        goto afterParent;
    }

    slash = '/';
    path[len++] = slash;
    limit = maxlen - len;
    remaining = limit;
    dst = path + len;
    goto checkParentName;

parentNameLoop:
    *dst = *(volatile u8*)parentName;
    parentName++;
    remaining--;
    dst++;

checkParentName:
    if (remaining == 0) {
        goto doneParentName;
    }
    if ((s8)*parentName != 0) {
        goto parentNameLoop;
    }

doneParentName:
    len += limit - remaining;

afterParent:
    if (len != maxlen) {
    } else {
        goto done;
    }

    slash = '/';
    path[len++] = slash;
    limit = maxlen - len;
    src = name;
    remaining = limit;
    dst = path + len;
    goto checkCurrentName;

currentNameLoop:
    *dst = *(volatile u8*)src;
    src++;
    remaining--;
    dst++;

checkCurrentName:
    if (remaining == 0) {
        goto doneCurrentName;
    }
    if ((s8)*src != 0) {
        goto currentNameLoop;
    }

doneCurrentName:
    len += limit - remaining;

done:
    return len;
}

/* fn_800A5268 - 0x800A5268 | size: 0xC4 */
BOOL fn_800A5268(char* path, u32 maxlen) {
    extern u32 lbl_8047A7D8;
    u32 currentDir = lbl_8047A7D8;
    u32 len = fn_800A5108(currentDir, path, maxlen);
    s32 result;

    if (len == maxlen) {
        result = FALSE;
        path[maxlen - 1] = '\0';
        goto done;
    }

    {
        s32 isDir;

        if ((DVD_FST_ENTRY(currentDir).typeAndNameOffset & DVD_FST_TYPE_MASK) == 0) {
            isDir = FALSE;
        } else {
            isDir = TRUE;
        }

        if (isDir != FALSE) {
            if (len == maxlen - 1) {
                result = FALSE;
                path[len] = '\0';
                goto done;
            }
            path[len] = '/';
            len++;
        }
    }

    path[len] = '\0';
    result = TRUE;

done:
    return result;
}

/* fn_800A532C - 0x800A532C | size: 0xC0 */
BOOL fn_800A532C(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset,
                 DVDCBCallback callback, s32 prio) {
    extern char lbl_803119F0[];
    extern char lbl_804789C0;
    extern void fn_800060F0(const char* file, u32 line, const char* expr, ...);
    extern void fn_800A53EC(s32 result, DVDFileInfo* fileInfo);
    s32 end;

    if (offset < 0 || (u32)offset >= fileInfo->length) {
        fn_800060F0(&lbl_804789C0, 0x2E6, lbl_803119F0);
    }

    end = offset + length;
    if (end < 0 || (u32)end >= fileInfo->length + 0x20) {
        fn_800060F0(&lbl_804789C0, 0x2EC, lbl_803119F0);
    }

    fileInfo->callback = callback;
    DVDReadAbsAsyncPrio(&fileInfo->cb, addr, length, fileInfo->startAddr + offset,
                        (DVDCBCallback)fn_800A53EC, prio);
    return TRUE;
}

/* fn_800A53EC - 0x800A53EC | size: 0x30
 * cbForSeekAsync - forward the result to the file's user callback.
 */
void fn_800A53EC(s32 result, DVDFileInfo* fileInfo) {
    if (fileInfo->callback) {
        (fileInfo->callback)(result, &fileInfo->cb);
    }
}

/* fn_800A541C - 0x800A541C | size: 0x118 */
s32 fn_800A541C(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset,
                s32 prio) {
    extern char lbl_80311A24[];
    extern char lbl_804789C0;
    extern u8 __DVDThreadQueue;
    extern void fn_800060F0(const char* file, u32 line, const char* expr, ...);
    extern void fn_800A238C(void* queue);
    extern void fn_800A5534(void);
    extern BOOL OSDisableInterrupts(void);
    extern void OSRestoreInterrupts(BOOL enabled);
    s32 end;
    BOOL enabled;

    if (offset < 0 || (u32)offset >= fileInfo->length) {
        fn_800060F0(&lbl_804789C0, 0x32C, lbl_80311A24);
    }

    end = offset + length;
    if (end < 0 || (u32)end >= fileInfo->length + 0x20) {
        fn_800060F0(&lbl_804789C0, 0x332, lbl_80311A24);
    }

    if (!DVDReadAbsAsyncPrio(&fileInfo->cb, addr, length, fileInfo->startAddr + offset,
                             (DVDCBCallback)fn_800A5534, prio)) {
        return -1;
    }

    enabled = OSDisableInterrupts();
    while (TRUE) {
        if (fileInfo->cb.state == 0) {
            fileInfo = (DVDFileInfo*)fileInfo->cb.transferredSize;
            goto done;
        }
        if (fileInfo->cb.state == -1) {
            fileInfo = (DVDFileInfo*)-1;
            goto done;
        }
        if (fileInfo->cb.state == 10) {
            fileInfo = (DVDFileInfo*)-3;
            goto done;
        }
        fn_800A238C(&__DVDThreadQueue);
    }

done:
    OSRestoreInterrupts(enabled);
    return (s32)fileInfo;
}

/* fn_800A5534 - 0x800A5534 | size: 0x24
 * cbForReadSync - wake the DVD-FS wait thread once the read completes.
 */
void fn_800A5534(void) {
    extern void fn_800A2478(void* queue);
    extern u8 __DVDThreadQueue[8];
    fn_800A2478(&__DVDThreadQueue);
}

/* fn_800A5558 - 0x800A5558 | size: 0x98 */
BOOL fn_800A5558(DVDFileInfo* fileInfo, s32 offset, DVDCBCallback callback,
                 s32 prio) {
    extern char lbl_80311A54[];
    extern char lbl_804789C0;
    extern void fn_800060F0(const char* file, u32 line, const char* expr, ...);
    extern void fn_800A55F0(s32 result, DVDFileInfo* fileInfo);
    extern BOOL DVDSeekAbsAsyncPrio(DVDCommandBlock* block, s32 offset,
                                    DVDCBCallback callback, s32 prio);

    if (offset < 0 || (u32)offset >= fileInfo->length) {
        fn_800060F0(&lbl_804789C0, 0x383, lbl_80311A54);
    }

    fileInfo->callback = callback;
    DVDSeekAbsAsyncPrio(&fileInfo->cb, fileInfo->startAddr + offset,
                        (DVDCBCallback)fn_800A55F0, prio);
    return TRUE;
}

/* fn_800A55F0 - 0x800A55F0 | size: 0x30
 * cbForPrepareStreamAsync - forward the result to the file's user callback.
 */
void fn_800A55F0(s32 result, DVDFileInfo* fileInfo) {
    if (fileInfo->callback) {
        (fileInfo->callback)(result, &fileInfo->cb);
    }
}

/* fn_800A5620 - 0x800A5620 | size: 0x4 */
/* Empty function (blr) - no-op placeholder */
void fn_800A5620(void) {
}
