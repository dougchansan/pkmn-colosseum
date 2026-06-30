/**
 * @file dvd_shim.c
 * @brief DVD filesystem replacement -- reads from ISO file. Stub implementations.
 *
 * References:
 *   - dvd_shim.h for full API documentation
 *   - YAGCD -- GCN disc filesystem documentation
 *
 * Phase 3 PC port scaffolding -- skeleton only.
 */

#ifdef __MWERKS__
/* GCN build: pcport shim not applicable */
#else

#include "dvd_shim.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* =========================================================================
 * Internal state
 * ========================================================================= */

/** Path to the ISO file */
static char g_isoPath[512] = "game.iso";

/** Path to extracted root directory (alternative to ISO) */
static char g_rootPath[512] = "";

/** Whether we are using extracted directory mode vs ISO mode */
static int g_useExtractedDir = 0;

/** ISO file handle */
static FILE* g_isoFile = NULL;

/** Parsed FST entries */
static FSTEntry* g_fstEntries = NULL;
static u32 g_fstNumEntries = 0;

/** FST string table */
static char* g_fstStringTable = NULL;

/** Total bytes read counter */
static u32 g_totalBytesRead = 0;

/** Initialized flag */
static int g_dvdInitialized = 0;

/* =========================================================================
 * Helpers
 * ========================================================================= */

static u32 read_be32(const u8* p) {
    return ((u32)p[0] << 24) | ((u32)p[1] << 16) |
           ((u32)p[2] << 8)  | (u32)p[3];
}

/* =========================================================================
 * Public API implementation
 * ========================================================================= */

void DVDSetISOPath(const char* path) {
    if (path) {
        strncpy(g_isoPath, path, sizeof(g_isoPath) - 1);
        g_isoPath[sizeof(g_isoPath) - 1] = '\0';
    }
    g_useExtractedDir = 0;
}

void DVDSetRootPath(const char* path) {
    if (path) {
        strncpy(g_rootPath, path, sizeof(g_rootPath) - 1);
        g_rootPath[sizeof(g_rootPath) - 1] = '\0';
        g_useExtractedDir = 1;
    }
}

BOOL DVDInit_PC(void) {
    /* TODO: Phase 3 -- Parse the GCN ISO filesystem
     *
     * GCN ISO layout:
     *   0x0000: Boot block (DVDDiskID)
     *   0x0420: Debug/BI2 info
     *   0x2440: Apploader
     *   (FSTOffset from boot block at offset 0x424)
     *   FSTOffset: File String Table
     *
     * FST format:
     *   Entry 0: Root directory (fileLength = total number of entries)
     *   Each entry is 12 bytes:
     *     u8  flags     -- 0=file, 1=directory
     *     u24 nameOff   -- offset into string table
     *     u32 offset    -- file offset in ISO (for files)
     *                      parent entry index (for directories)
     *     u32 length    -- file length (for files)
     *                      next entry index (for directories)
     *
     * Steps:
     * 1. Open the ISO:
     *    g_isoFile = fopen(g_isoPath, "rb");
     *
     * 2. Read the FST offset from the boot block (offset 0x424):
     *    fseek(g_isoFile, 0x424, SEEK_SET);
     *    u8 buf[4]; fread(buf, 4, 1, g_isoFile);
     *    u32 fstOffset = read_be32(buf);
     *
     * 3. Read the root entry to get entry count:
     *    fseek(g_isoFile, fstOffset, SEEK_SET);
     *    fread(rootEntry, 12, 1, g_isoFile);
     *    g_fstNumEntries = read_be32(&rootEntry[8]);
     *
     * 4. Read all FST entries:
     *    g_fstEntries = malloc(g_fstNumEntries * 12);
     *    fseek(g_isoFile, fstOffset, SEEK_SET);
     *    fread(g_fstEntries, 12, g_fstNumEntries, g_isoFile);
     *
     * 5. Read the string table (immediately after entries):
     *    u32 stringTableOffset = fstOffset + g_fstNumEntries * 12;
     *    // Read to end of FST area
     *    g_fstStringTable = malloc(stringTableSize);
     *    fread(g_fstStringTable, 1, stringTableSize, g_isoFile);
     *
     * 6. Build a path -> entry lookup table for fast DVDOpen
     */

    if (g_useExtractedDir) {
        printf("[dvd_shim] Using extracted directory: %s\n", g_rootPath);
        g_dvdInitialized = 1;
        return 1;
    }

    printf("[dvd_shim] DVDInit_PC stub -- ISO path: %s\n", g_isoPath);

    /* Stub: mark as initialized without actually opening the ISO */
    g_dvdInitialized = 1;
    g_totalBytesRead = 0;

    return 1;
}

BOOL DVDOpen(const char* path, DVDFileInfo_PC* fileInfo) {
    if (!path || !fileInfo || !g_dvdInitialized) return 0;

    memset(fileInfo, 0, sizeof(*fileInfo));

    /* TODO: Phase 3 -- Look up file in FST
     *
     * Option A (ISO mode):
     *   s32 entryNum = DVDConvertPathToEntrynum(path);
     *   if (entryNum < 0) return FALSE;
     *
     *   FSTEntry* entry = &g_fstEntries[entryNum];
     *   fileInfo->startAddr = entry->fileOffset;
     *   fileInfo->length = entry->fileLength;
     *   fileInfo->hostFile = g_isoFile; // share the ISO file handle
     *   fileInfo->cb.state = DVD_STATE_END;
     *
     * Option B (extracted directory mode):
     *   char fullPath[1024];
     *   snprintf(fullPath, sizeof(fullPath), "%s/%s", g_rootPath, path);
     *   FILE* f = fopen(fullPath, "rb");
     *   if (!f) return FALSE;
     *   fseek(f, 0, SEEK_END);
     *   fileInfo->length = (u32)ftell(f);
     *   fseek(f, 0, SEEK_SET);
     *   fileInfo->startAddr = 0;
     *   fileInfo->hostFile = f;
     *   fileInfo->cb.state = DVD_STATE_END;
     */

    printf("[dvd_shim] DVDOpen stub: %s\n", path);
    return 0;
}

BOOL DVDClose(DVDFileInfo_PC* fileInfo) {
    if (!fileInfo) return 0;

    /* TODO: Phase 3 -- Close file handle
     *
     * In extracted directory mode:
     *   if (fileInfo->hostFile && fileInfo->hostFile != g_isoFile) {
     *       fclose((FILE*)fileInfo->hostFile);
     *   }
     * In ISO mode: don't close -- g_isoFile is shared.
     *
     * fileInfo->hostFile = NULL;
     */

    memset(fileInfo, 0, sizeof(*fileInfo));
    return 1;
}

s32 DVDReadPrio(DVDFileInfo_PC* fileInfo, void* addr,
                s32 length, s32 offset, s32 priority) {
    (void)priority;

    if (!fileInfo || !addr || length <= 0) return DVD_RESULT_FATAL;

    /* TODO: Phase 3 -- Read from ISO or extracted file
     *
     * FILE* f = (FILE*)fileInfo->hostFile;
     * if (!f) return DVD_RESULT_FATAL;
     *
     * long seekPos = (long)fileInfo->startAddr + offset;
     * if (fseek(f, seekPos, SEEK_SET) != 0)
     *     return DVD_RESULT_FATAL;
     *
     * size_t bytesRead = fread(addr, 1, (size_t)length, f);
     * g_totalBytesRead += (u32)bytesRead;
     *
     * fileInfo->cb.transferredSize = (u32)bytesRead;
     * fileInfo->cb.state = DVD_STATE_END;
     *
     * return (s32)bytesRead;
     */

    (void)offset;
    return DVD_RESULT_FATAL;
}

BOOL DVDReadAsyncPrio(DVDFileInfo_PC* fileInfo, void* addr,
                      s32 length, s32 offset,
                      void (*callback)(s32, DVDFileInfo_PC*),
                      s32 priority) {
    /* TODO: Phase 3 -- Async read (initially synchronous on PC)
     *
     * For the initial implementation, perform the read synchronously
     * and call the callback immediately:
     *
     * s32 result = DVDReadPrio(fileInfo, addr, length, offset, priority);
     * if (callback)
     *     callback(result >= 0 ? DVD_RESULT_GOOD : DVD_RESULT_FATAL, fileInfo);
     * return result >= 0;
     *
     * For a future optimization, queue reads on a background I/O thread.
     */

    (void)fileInfo; (void)addr; (void)length; (void)offset;
    (void)callback; (void)priority;

    return 0;
}

s32 DVDGetFileInfoStatus(DVDFileInfo_PC* fileInfo) {
    if (!fileInfo) return DVD_STATE_END;
    return fileInfo->cb.state;
}

u32 DVDGetLength(DVDFileInfo_PC* fileInfo) {
    if (!fileInfo) return 0;
    return fileInfo->length;
}

BOOL DVDCancel(DVDCommandBlock_PC* block) {
    if (!block) return 0;

    /* TODO: Phase 3 -- Cancel async operation
     *
     * On PC with synchronous reads, this is effectively a no-op.
     * With async I/O, signal the I/O thread to cancel.
     */

    block->state = DVD_STATE_CANCELED;
    return 1;
}

s32 DVDConvertPathToEntrynum(const char* path) {
    (void)path;

    /* TODO: Phase 3 -- Walk the FST to find the entry number
     *
     * Start at entry 0 (root directory).
     * For each path component:
     *   Search child entries of the current directory.
     *   Compare names using the string table.
     *   If found, descend into subdirectory or return file entry number.
     *
     * GCN paths use '/' as separator. The root is '/'.
     *
     * Algorithm:
     *   u32 currentDir = 0;
     *   u32 dirEnd = g_fstEntries[0].fileLength; // next entry after root
     *
     *   for each path component:
     *     for (u32 i = currentDir + 1; i < dirEnd; ) {
     *         const char* entryName = g_fstStringTable + getNameOffset(i);
     *         if (strcmp(entryName, component) == 0) {
     *             if (isLastComponent) return i;
     *             if (g_fstEntries[i].isDir) {
     *                 currentDir = i;
     *                 dirEnd = g_fstEntries[i].fileLength;
     *                 break;
     *             }
     *             return -1; // path component is a file, not dir
     *         }
     *         // Skip past this entry (and all children if dir)
     *         i = g_fstEntries[i].isDir ? g_fstEntries[i].fileLength : i + 1;
     *     }
     */

    return -1;
}

s32 DVDGetDriveStatus_PC(void) {
    /* On PC, the "disc" is always ready */
    return DVD_STATE_END;
}

void DVDShim_Shutdown(void) {
    if (g_isoFile) {
        fclose(g_isoFile);
        g_isoFile = NULL;
    }
    if (g_fstEntries) {
        free(g_fstEntries);
        g_fstEntries = NULL;
    }
    if (g_fstStringTable) {
        free(g_fstStringTable);
        g_fstStringTable = NULL;
    }
    g_fstNumEntries = 0;
    g_dvdInitialized = 0;
}

u32 DVDShim_GetBytesRead(void) {
    return g_totalBytesRead;
}


#endif /* __MWERKS__ */