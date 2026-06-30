/**
 * @file dvd_shim.h
 * @brief DVD filesystem replacement -- reads from ISO file on disk.
 *
 * Replaces the Dolphin SDK DVD functions with standard C file I/O that
 * reads from a GCN ISO image (or extracted filesystem directory).
 *
 * The game accesses disc data through:
 *   1. DVDOpen/DVDReadPrio/DVDClose -- standard file access
 *   2. GSDVD_Open -- game-level async disc access wrapper
 *   3. FSYS archive loading (fsys_load.c, fsys_file.c)
 *
 * This shim intercepts at the DVD SDK level so that the game's higher-level
 * GSDVD and FSYS code works without modification.
 *
 * References:
 *   - include/dolphin/dvd/dvd.h (DVDFileInfo, DVDCommandBlock)
 *   - include/game/gs_dvd.h (GSDVD wrapper)
 *   - src/dolphin/dvd/DVD.c, DVDFs.c
 *
 * Phase 3 PC port scaffolding -- skeleton only.
 */
#ifndef PCPORT_DVD_SHIM_H
#define PCPORT_DVD_SHIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard types */
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long    u32;
typedef signed long      s32;
typedef signed long long s64;
typedef int            BOOL;

/* =========================================================================
 * DVD file structures (matching Dolphin SDK)
 * ========================================================================= */

/** DVD command block (simplified for PC port) */
typedef struct DVDCommandBlock_PC {
    u32 command;
    s32 state;
    u32 offset;
    u32 length;
    void* addr;
    u32 transferredSize;
    void (*callback)(s32 result, struct DVDCommandBlock_PC* block);
} DVDCommandBlock_PC;

/** DVD file info (matching SDK DVDFileInfo layout for compatibility) */
typedef struct DVDFileInfo_PC {
    DVDCommandBlock_PC cb;
    u32 startAddr;      /* Offset in the ISO where the file starts */
    u32 length;         /* File length in bytes */
    void (*callback)(s32 result, struct DVDFileInfo_PC* fileInfo);

    /* PC-specific extension: host file handle */
    void* hostFile;     /* FILE* or HANDLE for the ISO/directory file */
} DVDFileInfo_PC;

/** FST entry for the GCN filesystem table */
typedef struct {
    u8  isDir;          /* 0=file, 1=directory */
    u8  nameOffset[3];  /* 24-bit offset into string table (big-endian) */
    u32 fileOffset;     /* File offset in ISO, or parent index for dirs */
    u32 fileLength;     /* File length, or next entry index for dirs */
} FSTEntry;

/* =========================================================================
 * DVD result codes (matching SDK)
 * ========================================================================= */

#define DVD_RESULT_GOOD      0
#define DVD_RESULT_FATAL    -1
#define DVD_RESULT_IGNORED  -2
#define DVD_RESULT_CANCELED -3

/* DVD states */
#define DVD_STATE_END        0
#define DVD_STATE_BUSY       1
#define DVD_STATE_WAITING    2
#define DVD_STATE_COVER_OPEN 5
#define DVD_STATE_CANCELED  10

/* =========================================================================
 * Public API -- DVD SDK function replacements
 * ========================================================================= */

/**
 * DVDInit_PC -- Initialize the DVD shim.
 *
 * Parses the FST (File String Table) from the ISO image to build
 * an in-memory file table for DVDOpen lookups.
 *
 * Must be called before any other DVD functions.
 *
 * @return  BOOL: TRUE on success, FALSE on failure.
 */
BOOL DVDInit_PC(void);

/**
 * DVDSetISOPath -- Set the path to the GCN ISO file.
 *
 * @param path  Path to the .iso file on the host filesystem.
 *
 * Must be called before DVDInit_PC. If not called, the shim will
 * look for "game.iso" in the current directory.
 */
void DVDSetISOPath(const char* path);

/**
 * DVDSetRootPath -- Set the path to an extracted filesystem directory.
 *
 * @param path  Path to the extracted root directory.
 *
 * Alternative to ISO mode: if set, DVDOpen will look for files
 * directly on the host filesystem instead of reading from an ISO.
 */
void DVDSetRootPath(const char* path);

/**
 * DVDOpen -- Open a file on the virtual disc.
 *
 * @param path      File path within the GCN filesystem (e.g., "/common_rel.fsys").
 * @param fileInfo  DVDFileInfo structure to fill with file location data.
 * @return          TRUE if the file was found, FALSE otherwise.
 *
 * Looks up the file in the FST parsed from the ISO, stores the
 * offset and length in fileInfo. Does NOT read the file data.
 */
BOOL DVDOpen(const char* path, DVDFileInfo_PC* fileInfo);

/**
 * DVDClose -- Close a previously opened file.
 *
 * @param fileInfo  File info from DVDOpen.
 * @return          TRUE on success.
 *
 * Releases any resources associated with the file handle.
 */
BOOL DVDClose(DVDFileInfo_PC* fileInfo);

/**
 * DVDReadPrio -- Read data from an open file (synchronous).
 *
 * @param fileInfo  File info from DVDOpen.
 * @param addr      Destination buffer (must be 32-byte aligned on GCN;
 *                  no alignment requirement on PC).
 * @param length    Number of bytes to read.
 * @param offset    Byte offset within the file.
 * @param priority  Read priority (ignored on PC).
 * @return          Number of bytes read, or DVD_RESULT_FATAL on error.
 *
 * Seeks to (fileInfo->startAddr + offset) in the ISO and reads
 * 'length' bytes into the destination buffer.
 */
s32 DVDReadPrio(DVDFileInfo_PC* fileInfo, void* addr,
                s32 length, s32 offset, s32 priority);

/**
 * DVDReadAsyncPrio -- Read data from an open file (asynchronous).
 *
 * @param fileInfo  File info from DVDOpen.
 * @param addr      Destination buffer.
 * @param length    Number of bytes to read.
 * @param offset    Byte offset within the file.
 * @param callback  Completion callback.
 * @param priority  Read priority (ignored on PC).
 * @return          TRUE if the read was queued.
 *
 * On GCN, this queues an async DMA. On PC, we either:
 * a) Perform the read synchronously and call the callback immediately
 * b) Queue it on a background thread for true async I/O
 *
 * For initial implementation, synchronous is fine.
 */
BOOL DVDReadAsyncPrio(DVDFileInfo_PC* fileInfo, void* addr,
                      s32 length, s32 offset,
                      void (*callback)(s32, DVDFileInfo_PC*),
                      s32 priority);

/**
 * DVDGetFileInfoStatus -- Get the status of an async operation.
 *
 * @param fileInfo  File info to query.
 * @return          DVD state (DVD_STATE_END if complete).
 */
s32 DVDGetFileInfoStatus(DVDFileInfo_PC* fileInfo);

/**
 * DVDGetLength -- Get the length of an open file.
 *
 * @param fileInfo  File info from DVDOpen.
 * @return          File length in bytes.
 */
u32 DVDGetLength(DVDFileInfo_PC* fileInfo);

/**
 * DVDCancel -- Cancel a pending async read.
 *
 * @param block  Command block to cancel.
 * @return       TRUE if cancelled.
 */
BOOL DVDCancel(DVDCommandBlock_PC* block);

/**
 * DVDConvertPathToEntrynum -- Look up the FST entry number for a path.
 *
 * @param path  File path within the GCN filesystem.
 * @return      Entry number (>=0), or -1 if not found.
 *
 * Used internally by DVDOpen and can also be called directly by
 * game code that needs to check file existence.
 */
s32 DVDConvertPathToEntrynum(const char* path);

/**
 * DVDGetDriveStatus_PC -- Get the DVD drive status.
 *
 * @return  Always returns 0 (DVD_STATE_END) on PC -- disc is always ready.
 */
s32 DVDGetDriveStatus_PC(void);

/* =========================================================================
 * PC-specific extensions
 * ========================================================================= */

/**
 * DVDShim_Shutdown -- Clean up the DVD shim.
 *
 * Close the ISO file handle and free the FST.
 */
void DVDShim_Shutdown(void);

/**
 * DVDShim_GetBytesRead -- Get total bytes read from disc (for stats).
 *
 * @return  Total bytes read since initialization.
 */
u32 DVDShim_GetBytesRead(void);

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_DVD_SHIM_H */
