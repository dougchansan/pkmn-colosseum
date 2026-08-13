/**
 * @file fsys_file.c
 * @brief FSYS file lookup, entry processing, and archive load state machine.
 *
 * Contains the core file lookup/check functions (fn_8017B07C,
 * fn_8017B13C), the per-entry processor (fn_8017E30C), and the
 * multi-stage load state machine (_fsysGetFilename).
 *
 * Address range: 0x8017B07C - 0x8017F2C4 (per splits.txt; confirmed
 * correct for this unit).
 *
 * 2026-07-02 reconciliation: removed 2 orphan definitions whose names
 * are not present in symbols.txt and never paired in objdiff:
 *   - FSYSCacheLookup: claimed address 0x8017F794 is actually outside
 *     this unit entirely (belongs to game/gs_range_8017F2C4.c per
 *     splits.txt); its one caller in fn_8017E30C now calls the real,
 *     not-yet-decompiled fn_8017F794 directly instead, matching how
 *     the rest of this file already calls it.
 *   - FSYSBeginLoad: renamed (not deleted) to its real name,
 *     _fsysGetFilename - strong evidence from matching address/size
 *     against this unit's own objdiff report AND ground-truth
 *     disassembly of this unit's real matched wrapper functions, which
 *     literally contain "bl _fsysGetFilename".
 * The stale FSYSCheckFileLoaded/FSYSRequestFile/FSYSProcessEntry
 * prototypes in include/game/fsys/fsys.h (for names a prior pass had
 * already renamed to fn_8017B07C/fn_8017B13C/fn_8017E30C in this file)
 * were also removed there.
 */

#include "game/fsys/fsys.h"

/* ===================================================================
 * External SDK / engine functions
 * =================================================================== */

extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* Memory allocation */
extern u16  fn_800E2C04(u32 size, u32 alignment);
extern u16  fn_800E2B00(u32 size, u32 alignment);
extern void* fn_800E27B0(u16 handle);
extern void  fn_800E24B0(u16 handle);
extern void  fn_800E209C(u16 handle);
extern u16   fn_800E202C(void* ptr);

/* DVD / file operations */
extern void  strcpy(void* dst, const void* src);
extern u32   fn_80167F28(const char* path);
extern u32   fn_80167E5C(u32 fileInfo);
extern void  fn_80167E64(u32 fileInfo);
extern u8    fn_80167EF8(void* path);
extern void  fn_80167ED0(u32 fileInfo, void* buf, u32 len, u32 offset);

/* DMA / cache */
extern void  DCFlushRange(void* addr, u32 len);

/* sprintf-like */
extern void  sprintf(char* dst, const char* fmt, ...);

/* Memory read (heap-to-ptr with DMA) */
extern void* GSresAllocResourceAlign(u32 size, u32 priority, u32 alignment, u32 fileID, u32 param);
extern void* GSresGetResource(u32 fileHandle, u32 fileID);

/* Slot search */
extern FSYSSlot* FSYSFindSlot(u32 fileHandle, u32 mode);

/* Decompression */
extern void FSYSDecompressLZSS(void* dst, const void* src, u32 size);

/* Extended load function (defined in fsys_load.c) */
extern void fn_8017E1D8(FSYSSlot* slot, u32 fileHandle,
                         u32 callbackA, u32 callbackB, u32 callbackC);

/* DMA copy (defined in DVD layer) */
extern void fn_80180320(void* dst, void* src, u32 size);

/* ===================================================================
 * External globals
 * =================================================================== */

extern FSYSManager gFSYSManager;
extern FSYSSlot*   gFSYSSlots;
extern void*       gFSYSTocData;

/* lbl_80478C48 -- DVD info count / pool state */
extern u32 gDVDPoolState;

/* lbl_8036C2A0 -- DecompPoolEntry base (used for cache searches) */
extern DecompPoolEntry gDecompPoolBase[];

/* lbl_80453FDC -- LZSS decompression context */
extern FSYSDecompContext gLZSSContext;

/* lbl_80452FC8 -- LZSS sliding window */
extern u8 gLZSSWindow[];

/* FSYS stores a fixed 0x28-byte entry header followed by a runtime sub-entry trailer. */
static FSYSSubEntry* FSYSFileEntry_GetSubEntry(FSYSFileEntry* entry) {
    return (FSYSSubEntry*)((u8*)entry + 0x28);
}

/* ===================================================================
 * fn_8017B07C: fn_8017B07C
 *
 * Checks whether a specific file (identified by nameHash) within the
 * archive loaded at fileHandle is fully decompressed and available.
 *
 * Iterates through all entries in the archive's TOC. For each entry,
 * resolves the file entry through two levels of indirection:
 *   archive_base + offset_table_offset -> string_table_offset -> entry
 *
 * If the entry's nameHash matches and its sub-entry state == 6 (complete),
 * returns 1.
 *
 * @param fileHandle  The DVD file handle identifying the archive
 * @param nameHash    The name hash / resource ID of the file to check
 * @return            1 if the file is fully loaded, 0 otherwise
 * =================================================================== */
extern FSYSSlot* fn_8017D410(u32 fileHandle, s32 mode);

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_8017B07C(u32 fileHandle, u32 nameHash) {
    FSYSSlot* slot;
    u8* archive;
    s32 found;
    u32 i;
    FSYSFileEntry* entry;
    FSYSFileEntry* e2;
    FSYSFileEntry* spareA;
    u32* firstTable;
    u32* entryTable;
    FSYSFileEntry* spareB;
    slot = fn_8017D410(fileHandle, 3);
    found = 0;
    if (slot) {
        e2 = e2;
        for (i = 0; i < slot->numEntries; i++) {
            archive = (u8*)slot->archiveData;
            if (archive) {
                firstTable = (u32*)(archive + *(u32*)(archive + 0x18));
                entryTable = (u32*)(archive + firstTable[0]);
                spareA = (FSYSFileEntry*)(archive + entryTable[i]);
            } else {
                spareA = NULL;
            }
            entry = spareA;
            e2 = entry;
            if (e2->nameHash == nameHash) {
                if (*(s32*)((u8*)entry + 0x28) != 6) {
                    spareB = spareB;
                } else {
                    found = 1;
                }
            }
            spareB = spareB;
        }
        return found;
    }
    return found;
}
#pragma pop

/* ===================================================================
 * fn_8017B13C: fn_8017B13C
 *
 * Requests loading of a file from an already-loaded archive.
 * Finds the slot, stores the request ID, then calls the extended
 * load function (fn_8017E1D8) with NULL callbacks.
 *
 * @param fileHandle  DVD file handle
 * @param requestID   Resource name hash to load
 * @return            1 on success, 0 if no slot available
 * =================================================================== */

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_8017B13C(u32 fileHandle, u32 requestID) {
    FSYSSlot* slot;

    slot = fn_8017D410(fileHandle, 3);
    if (slot) {
        slot->requestID = requestID;
        fn_8017E1D8(slot, fileHandle, 0, 0, 0);
        return 1;
    }
    return 0;
}
#pragma pop

/* Note: numEntries is at offset 0x0C in the FSYSSlot struct, now a named field. */

/* ===================================================================
 * fn_8017E30C: fn_8017E30C
 *
 * Core archive entry processor. After an archive is loaded into memory,
 * this function processes each file entry within it:
 *
 * 1. Searches the archive's TOC for an entry matching slot->requestID.
 * 2. If found, checks the compression flag (entry->flags bit 0).
 * 3. For compressed entries:
 *    a. Allocates a temporary buffer for the compressed data.
 *    b. Reads compressed data via DMA (fn_80180320).
 *    c. Copies the 16-byte LZSS header to gLZSSContext.
 *    d. Clears the LZSS sliding window (4078 bytes).
 *    e. Calls FSYSDecompressLZSS to decompress in-place.
 *    f. Flushes the D-cache.
 *    g. Frees the temporary compressed buffer.
 * 4. For uncompressed entries:
 *    a. Allocates a buffer for the raw data.
 *    b. Reads data directly via DMA.
 *    c. Flushes the D-cache.
 * 5. Looks up the decompression pool (lbl_8036C2A0) for a callback
 *    and calls it if present.
 *
 * @param slot  The FSYSSlot containing the archive and request info
 * @return      1 on success, 0 if entry not found or allocation failed
 * =================================================================== */
s32 fn_8017E30C(FSYSSlot* slot) {
    u32 i;
    s32 found = 0;
    FSYSFileEntry* fileEntry = NULL;
    FSYSSubEntry* subEntry = NULL;
    void* allocatedBuf;
    u32 isCompressed;

    /* Search the archive TOC for the requested entry */
    if (slot->archiveData != NULL) {
        u32 numEntries = slot->numEntries;
        for (i = 0; i < numEntries; i++) {
            void* archBase = slot->archiveData;

            if (archBase != NULL) {
                u32 stringTableOff = *(u32*)((u8*)archBase + 0x18);
                u32* offTable = (u32*)((u8*)archBase + stringTableOff);
                u32  strOff   = offTable[0];
                u32* entryTable = (u32*)((u8*)archBase + strOff);
                u32  entryOff  = entryTable[i];
                fileEntry = (FSYSFileEntry*)((u8*)archBase + entryOff);
            } else {
                fileEntry = NULL;
            }

            if (fileEntry != NULL) {
                /* Compare entry nameHash against slot requestID */
                if (fileEntry->nameHash == slot->requestID) {
                    subEntry = FSYSFileEntry_GetSubEntry(fileEntry);
                    found = 1;
                    break;
                }
            }
        }
    }

    if (found == 0) {
        return 0;
    }

    /* Try to find the data in the cache first. fn_8017F794 (not yet
     * decompiled; belongs to game/gs_range_8017F2C4.c per splits.txt)
     * is called the same way throughout the rest of this file - see
     * the 2026-07-02 note near FSYSCacheLookup's old location below. */
    {
        extern u32 fn_8017F794();
        allocatedBuf = (void*)fn_8017F794(slot->fileHandle,
                                           fileEntry->groupID,
                                           fileEntry->nameHash);
    }
    if (allocatedBuf == NULL) {
        return 0;
    }

    /* Check compression flag (bit 0 of entry->flags) */
    isCompressed = fileEntry->flags & 1;

    if (isCompressed) {
        /* ===== Compressed entry path ===== */
        u32 decompSize = fileEntry->decompressedSize;
        u32 allocSize;
        u16 handle;
        DecompPoolEntry* poolEntry;
        void* compBuf;
        void* compData;

        /* Allocate temporary buffer for compressed data */
        allocSize = (decompSize + 0x1F) & ~0x1F;
        handle = fn_800E2B00(allocSize, 0x20);
        if (handle != 0) {
            compBuf = fn_800E27B0(handle);
        } else {
            compBuf = NULL;
        }
        subEntry->buffer = compBuf;

        /* Read compressed data from DVD into temporary buffer */
        fn_80180320(subEntry->buffer, allocatedBuf, decompSize);

        /* Flush the D-cache for the compressed data */
        DCFlushRange(subEntry->buffer, decompSize);

        compData = subEntry->buffer;
        subEntry->buffer = NULL;

        /* Find the output allocator before decoding the temporary input. */
        {
            u32 j;
            DecompPoolEntry* dp = gDecompPoolBase;
            poolEntry = NULL;

            for (j = 0; j < gDVDPoolState; j++) {
                if (dp->fileID == fileEntry->groupID) {
                    poolEntry = dp;
                    break;
                }
                dp = (DecompPoolEntry*)((u8*)dp + FSYS_DECOMP_ENTRY_SIZE);
            }

        }

        if (poolEntry != NULL && poolEntry->callback != NULL) {
            typedef void* (*DecompCallback)(u32 fileHandle, u32 fileID, u32 compSize);
            DecompCallback cb = (DecompCallback)poolEntry->callback;
            allocatedBuf = cb(slot->fileHandle, fileEntry->nameHash,
                             fileEntry->compressedSize);
        } else {
            allocSize = (fileEntry->compressedSize + 0x1F) & ~0x1F;
            allocatedBuf = GSresAllocResourceAlign(allocSize,
                                                    slot->archiveHandle,
                                                    fileEntry->nameHash, 0, 0);
        }

        subEntry->buffer = allocatedBuf;
        if (subEntry->buffer == NULL) {
            slot->status = FSYS_STATUS_ERROR;
            subEntry->state = 7;
            return 1;
        }

        /* Copy the LZSS header and decode from the temporary input. */
        memcpy(&gLZSSContext, compData, LZSS_HEADER_SKIP);
        {
            u32 j;
            for (j = 0; j < LZSS_WINDOW_SIZE; j++) {
                gLZSSWindow[j] = 0;
            }
        }
        FSYSDecompressLZSS(subEntry->buffer, compData, gLZSSContext.decompSize);
        DCFlushRange(subEntry->buffer, gLZSSContext.decompSize);

        {
            u16 tmpHandle = fn_800E202C(compBuf);
            if (tmpHandle != 0) {
                fn_800E24B0(tmpHandle);
                fn_800E209C(tmpHandle);
            }
        }
    } else {
        /* ===== Uncompressed entry path ===== */
        DecompPoolEntry* poolEntry;
        u32 uncompSize = fileEntry->decompressedSize;

        /* Search the decomp pool for a matching entry with callback */
        {
            u32 j;
            DecompPoolEntry* dp = gDecompPoolBase;
            poolEntry = NULL;

            for (j = 0; j < gDVDPoolState; j++) {
                if (dp->fileID == fileEntry->groupID) {
                    poolEntry = dp;
                    break;
                }
                dp = (DecompPoolEntry*)((u8*)dp + FSYS_DECOMP_ENTRY_SIZE);
            }
        }

        /* If a callback exists, use it to allocate/process the buffer */
        if (poolEntry != NULL && poolEntry->callback != NULL) {
            typedef void* (*AllocCallback)(u32 fileHandle, u32 fileID, u32 size);
            AllocCallback cb = (AllocCallback)poolEntry->callback;
            allocatedBuf = cb(slot->fileHandle, fileEntry->nameHash,
                             fileEntry->compressedSize);
        } else {
            /* No callback; allocate buffer from heap */
            u32 allocSize = (uncompSize + 0x1F) & ~0x1F;
            allocatedBuf = (void*)GSresAllocResourceAlign(allocSize,
                                               slot->fileHandle,
                                               fileEntry->nameHash,
                                               0, 0);
        }
    }

    /* Store the final buffer pointer */
    subEntry->buffer = allocatedBuf;
    subEntry->buffer = allocatedBuf;  /* double-store in original */

    if (subEntry->buffer == NULL) {
        /* Allocation failed */
        slot->status = FSYS_STATUS_ERROR;
        subEntry->state = 7;
        return 1;
    }

    /* Read uncompressed data (or already have decompressed data) */
    if (isCompressed == 0) {
        fn_80180320(subEntry->buffer, allocatedBuf, fileEntry->decompressedSize);
        DCFlushRange(subEntry->buffer, fileEntry->decompressedSize);
    }

    /* Look up decomp pool again for a post-read callback */
    {
        u32 j;
        DecompPoolEntry* dp = gDecompPoolBase;
        DecompPoolEntry* poolEntry2 = NULL;

        for (j = 0; j < gDVDPoolState; j++) {
            if (dp->fileID == fileEntry->groupID) {
                poolEntry2 = dp;
                break;
            }
            dp = (DecompPoolEntry*)((u8*)dp + FSYS_DECOMP_ENTRY_SIZE);
        }

        if (poolEntry2 != NULL) {
            /* Flush D-cache and invoke the completion callback */
            void* readResult = GSresGetResource(slot->fileHandle, fileEntry->nameHash);
            if (readResult != NULL) {
                if (isCompressed) {
                    DCFlushRange(readResult, fileEntry->compressedSize);
                } else {
                    DCFlushRange(readResult, fileEntry->decompressedSize);
                }
            }

            if (*(void**)((u8*)poolEntry2 + 0xC) != NULL) {
                typedef void* (*PostCallback)(u32 fh, u32 id, u32 sz);
                PostCallback cb =
                    (PostCallback)*(void**)((u8*)poolEntry2 + 0xC);
                if (isCompressed) {
                    cb(slot->fileHandle, fileEntry->nameHash,
                       fileEntry->compressedSize);
                } else {
                    cb(slot->fileHandle, fileEntry->nameHash,
                       fileEntry->decompressedSize);
                }
            }
        }
    }

    return 1;
}

/* ===================================================================
 * _fsysGetFilename (0x8017EB6C, size 0x59C)
 *
 * 2026-07-02 reconciliation: this function was previously named/
 * declared as the invented "FSYSBeginLoad" (an orphan - not present in
 * symbols.txt, never paired in objdiff). Renamed to its real name,
 * _fsysGetFilename, on strong evidence: the real target address
 * (0x8017EB6C) and size (0x59C = 1436 bytes, matching the unmatched
 * "_fsysGetFilename" slot in this unit's objdiff report) both line up,
 * AND the ground-truth disassembly for the four real, matched wrapper
 * functions below (fn_8017DEA4.inc, fn_8017DF4C.inc, fn_8017DFF4.inc,
 * fn_8017E09C.inc) each literally contain "bl _fsysGetFilename" with
 * the exact same r3-r8 argument setup this function's callers use.
 *
 * Implements the FSYS archive load state machine. This function is
 * called to initiate or continue loading an archive, and handles
 * state transitions:
 *
 *   PENDING (0x1F4) or FREE (0) -> Search TOC, format filename,
 *     check if another load is active, set status to LOADING/READING.
 *
 *   LOADED (0x3E8) -> re-entry with mode 3: resolve file within
 *     the already-loaded archive using the TOC hash table.
 *
 * The filename is constructed using sprintf("%s.fsys", name) where
 * name is resolved from the TOC entry table based on the fileHandle.
 *
 * @param slot       Target FSYSSlot
 * @param fileHandle DVD file handle / resource ID
 * @param callbackA  Completion callback A (stored at slot+0x134)
 * @param callbackB  Completion callback B (stored at slot+0x138)
 * @param callbackC  Completion callback C (stored at slot+0x13C)
 * @param loadMode   Load mode (stored at slot+0x4C)
 * =================================================================== */
void _fsysGetFilename(FSYSSlot* slot, u32 fileHandle,
                   u32 callbackA, u32 callbackB, u32 callbackC,
                   u32 loadMode) {
    s32 status = slot->status;
#define FSYS_LOOKUP(buffer)                                                   \
    do {                                                                      \
        volatile u32 count = *(u32*)((u8*)gFSYSTocData + 8);                  \
        u32* entry = (u32*)((u8*)gFSYSTocData +                              \
                            *(u32*)((u8*)gFSYSTocData + 0x10));              \
        u32 index = 0;                                                        \
        char* name = NULL;                                                    \
        while (index < count) {                                               \
            if (entry[0] == fileHandle) {                                    \
                name = (char*)((u8*)gFSYSTocData + entry[1]);                \
                break;                                                        \
            }                                                                 \
            entry = (u32*)((u8*)entry + 8);                                  \
            index++;                                                          \
        }                                                                     \
        sprintf(buffer, "%s.fsys", name);                                   \
        strcpy(slot->filename, buffer);                                       \
    } while (0)
#define FSYS_INIT(reload, clear_data)                                         \
    do {                                                                      \
        slot->archiveHandle = 0;                                              \
        if (clear_data) slot->archiveData = NULL;                             \
        slot->archiveSize = 0;                                                \
        slot->callbackA = callbackA;                                          \
        slot->callbackB = callbackB;                                          \
        slot->callbackC = callbackC;                                          \
        slot->fileHandle = fileHandle;                                        \
        slot->fileIndex = 0;                                                  \
        slot->loadMode = loadMode;                                            \
        slot->reloadFlag = reload;                                            \
        if (clear_data) slot->padding100 = 0;                                 \
    } while (0)
#define FSYS_SET_STATUS(error_state)                                          \
    do {                                                                      \
        if (loadMode >= 4) {                                                  \
            if (loadMode != 7) return;                                       \
            slot->status = error_state;                                      \
        } else if (loadMode == 1) {                                          \
            slot->status = FSYS_STATUS_READING;                              \
        } else if (loadMode >= 0) {                                          \
            slot->status = error_state;                                      \
        }                                                                     \
        return;                                                               \
    } while (0)

    if (status == FSYS_STATUS_PENDING) {
        goto initial_load;
    } else if (status >= FSYS_STATUS_PENDING) {
        if (status == FSYS_STATUS_LOADED) {
            goto loaded_archive;
        }
        goto reload_load;
    } else if (status != FSYS_STATUS_FREE) {
        goto reload_load;
    }

initial_load:
    {
        char nameBuf[0x80];
        FSYS_INIT(0, 1);
        FSYS_LOOKUP(nameBuf);
        if (gFSYSManager.activeSlot != NULL && slot != gFSYSManager.activeSlot) {
            slot->status = FSYS_STATUS_PENDING;
            return;
        }
        gFSYSManager.activeSlot = slot;
        gFSYSManager.currentSlot = slot;
        FSYS_SET_STATUS(FSYS_STATUS_LOADING);
    }

loaded_archive:
    {
        if (loadMode == 3) {
            char nameBuf[0x80];
            FSYS_INIT(1, 0);
            FSYS_LOOKUP(nameBuf);
            if (gFSYSManager.activeSlot != NULL) {
                slot->status = FSYS_STATUS_PENDING;
                return;
            }
            gFSYSManager.activeSlot = slot;
            gFSYSManager.currentSlot = slot;
            FSYS_SET_STATUS(FSYS_STATUS_ERROR);
        } else {
            char nameBuf[0x80];
            FSYS_INIT(1, 0);
            FSYS_LOOKUP(nameBuf);
            if (gFSYSManager.activeSlot != NULL) {
                slot->status = FSYS_STATUS_PENDING;
                return;
            }
            gFSYSManager.activeSlot = slot;
            gFSYSManager.currentSlot = slot;
            FSYS_SET_STATUS(FSYS_STATUS_ERROR);
        }
    }

reload_load:
    if (slot->reloadFlag == 1) {
        char nameBuf[0x80];
        slot->status = FSYS_STATUS_FREE;
        FSYS_INIT(0, 1);
        FSYS_LOOKUP(nameBuf);
        if (gFSYSManager.activeSlot != NULL && slot != gFSYSManager.activeSlot) {
            slot->status = FSYS_STATUS_PENDING;
            return;
        }
        gFSYSManager.activeSlot = slot;
        gFSYSManager.currentSlot = slot;
        FSYS_SET_STATUS(FSYS_STATUS_LOADING);
    }

#undef FSYS_SET_STATUS
#undef FSYS_INIT
#undef FSYS_LOOKUP
}

/*
 * FSYSCacheLookup - orphan removed (see file header). Not present in
 * symbols.txt; its claimed address (0x8017F794) belongs to
 * game/gs_range_8017F2C4.c per splits.txt, not this unit at all. The
 * rest of this file already calls the real (not-yet-decompiled)
 * function at that address correctly as `extern u32 fn_8017F794()`
 * (see e.g. lines below and fn_8017E30C above, which was updated to do
 * the same instead of calling this fictional wrapper).
 */

/* fn_80180320 is declared at the top of this file */


/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 18 functions matched
 * =================================================================== */

/* Address: 0x8017BFE8 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017BFE8(void) { return 0; }

/* Address: 0x8017BFF0 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017BFF0(void) { return 0; }

/* Address: 0x8017BFF8 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017BFF8(void) { return 0; }

/* Address: 0x8017C000 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017C000(void) { return 1; }

/* Address: 0x8017C394 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017C394(void) { return 1; }

/* Address: 0x8017C568 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017C568(void) { return 1; }

/* Address: 0x8017C570 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017C570(void) { return 1; }

/* Address: 0x8017C578 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017C578(void) { return 0; }

/* Address: 0x8017C590 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017C590(void) { return 1; }

/* Address: 0x8017C598 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017C598(void) { return 1; }

/* Address: 0x8017C5B0 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017C5B0(void) { return 1; }

/* Address: 0x8017C88C | Size: 0x8 | Pattern: return_constant */
u32 fn_8017C88C(void) { return 1; }

/* Address: 0x8017C8C0 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017C8C0(void) { return 1; }

/* Address: 0x8017C8F4 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017C8F4(void) { return 1; }

/* Address: 0x8017CEC8 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017CEC8(void) { return 1; }

/* Address: 0x8017CED0 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017CED0(void) { return 1; }

/* Address: 0x8017D400 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017D400(void) { return 1; }

/* Address: 0x8017D408 | Size: 0x8 | Pattern: return_constant */
u32 fn_8017D408(void) { return 1; }

/* WP-0054: restored asm wrappers */
extern void fn_80167E34(void);
extern u32 lbl_8047B1B4;
extern FSYSManager lbl_80453FEC;
void fn_8017B1AC(void) {
    fn_80167E34();
}

/* 0x8017B1CC | 0x100
 * Release a reference on the loaded archive for `fileHandle`. When the
 * refcount drops to zero, free the backing allocation via the handle-pool
 * helpers and clear the slot's identifying fields.
 */
#pragma push
#pragma optimization_level 0
void fn_8017B1CC(u32 fileHandle) {
    FSYSSlot* spare;
    FSYSSlot* slot;
    FSYSSlot* hit;
    FSYSSlot* found;
    u32 i;
    u16 handle;

    slot = (FSYSSlot*)lbl_8047B1B4;
    for (i = 0; i < lbl_80453FEC.maxSlots; i++) {
        if ((s32)slot->status != FSYS_STATUS_FREE) {
            if (slot->fileHandle == fileHandle) {
                hit = slot;
                goto done;
            } else {
                slot++;
            }
        } else {
            slot++;
        }
    }
    hit = NULL;
done:
    found = hit;
    spare = spare;
    if (!found) {
        return;
    }
    if ((s32)found->status != FSYS_STATUS_LOADED) {
        return;
    }
    if (!found->archiveData) {
        return;
    }
    if ((s32)--found->refCount > 0) {
        return;
    }
    handle = fn_800E202C(found->archiveData);
    if (handle != 0) {
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }
    found->archiveData = NULL;
    found->status     = 0;
    found->padding05C = 0;
    found->fileHandle = 0;
    found->reloadFlag = 0;
    found->loadMode   = 0;
}
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8017B2CC(void) {
#include "src/game/gs_scene_fn_8017B2CC.inc"
}
#else
s32 fn_8017B2CC(u32 fileHandle) {
    FSYSSlot* slot;
    FSYSSlot* hit;
    FSYSSlot* found;
    u32 i;

    slot = (FSYSSlot*)lbl_8047B1B4;
    for (i = 0; i < lbl_80453FEC.maxSlots; i++) {
        if ((s32)slot->status != FSYS_STATUS_FREE) {
            if (slot->fileHandle == fileHandle) {
                hit = slot;
                goto done;
            } else {
                slot++;
            }
        } else {
            slot++;
        }
    }
    hit = NULL;
done:
    found = hit;
    if (found) {
        if ((s32)found->status == FSYS_STATUS_LOADED) {
            return 0;
        }
        return 1;
    }
    return -1;
}
#endif
#pragma pop

/* 0x8017B370 | 0x74 */
#pragma push
#pragma optimization_level 0
s32 fn_8017B370(u32 fileHandle) {
    FSYSSlot* slot;
    extern void fn_8017E09C(FSYSSlot*, u32, u32, u32, u32);

    slot = fn_8017D410(fileHandle, 0);
    if (slot) {
        lbl_80453FEC.field_28 = 1;
        fn_8017E09C(slot, fileHandle, 0, 0, 0);
        return 1;
    }
    return 0;
}
#pragma pop

/* 0x8017B3E4 | 0x64 */
#pragma push
#pragma optimization_level 0
s32 fn_8017B3E4(u32 fileHandle) {
    FSYSSlot* slot;
    extern void fn_8017E09C(FSYSSlot*, u32, u32, u32, u32);

    slot = fn_8017D410(fileHandle, 0);
    if (slot) {
        fn_8017E09C(slot, fileHandle, 0, 0, 0);
        return 1;
    }
    return 0;
}
#pragma pop

/* ===================================================================
 * WP-0009 stubs (address range 0x8017B448 - 0x8017E1D8)
 * =================================================================== */

/* 0x8017B448 | 0x74 */
extern u32 lbl_8047B1B4;
#if 0
asm void fn_8017B448(void) {
#include "src/game/fsys/fsys_file_fn_8017B448.inc"
}
#else
typedef struct fn8017B448_Node {
    u8 _pad00[0x40];
    u32* field40;   /* 0x40 -> ptr, deref +0xc */
    u8 _pad44[0x48 - 0x44];
    s32 field48;    /* 0x48 */
    u8 _pad4c[0x5c - 0x4c];
    s32 field5c;    /* 0x5c */
    u8 _pad60[0xf8 - 0x60];
    u32 fieldf8;    /* 0xf8 */
} fn8017B448_Node;
#define COUNT_8017B448 lbl_80453FEC.maxSlots
#pragma push
#pragma optimization_level 0
s32 fn_8017B448(u32 handle) {
    u32 i;
    fn8017B448_Node* node;
    s32 value;
    u32* ptr;
    s32 result;

    node = (fn8017B448_Node*)lbl_8047B1B4;
    for (i = 0; i < COUNT_8017B448; node = (fn8017B448_Node*)((u8*)node + 0x140), i++) {
        if (node->field48 != 0 && node->fieldf8 == handle && node->field5c != 0) {
            ptr = node->field40;
            value = ptr[3];
            goto done;
        }
    }
    value = -1;
done:
    result = value;
    return result;
}
#pragma pop
#endif

extern void fn_8017F800(u32 fileHandle);
extern u32 fn_8017FA5C(void);
extern u32 fn_8017F794();
extern void fn_80167E98();
extern void fn_8017F108(s32 result);
extern void fn_80179FA4();
extern u32 lbl_8047B1B8;
extern u32 lbl_8047B1BC;

#define FSYS_COMPRESSED_FLAG 0x80000000u
#define FSYS_LZSS_MAGIC 0x4C5A5353u
#define FSYS_SLOT_FILE0(slot) ((slot)->fileInfo0)
#define FSYS_SLOT_FILE1(slot) (*(u32*)((u8*)(slot) + 0x6C))
#define FSYS_SLOT_CURRENT_SUB(slot) (*(FSYSSubEntry**)((u8*)(slot) + 0xFC))
#define FSYS_POOL_ALLOC_CB(pool) (*(void**)((u8*)(pool) + 0x08))
#define FSYS_POOL_DONE_CB(pool) (*(void**)((u8*)(pool) + 0x0C))

typedef void* (*FSYSAllocCallback)(u32 fileHandle, u32 nameHash, u32 size);
typedef void (*FSYSDoneCallback)(u32 fileHandle, u32 nameHash, u32 size);
typedef void (*FSYSUserCallback)(u32 loadMode, u32 callbackB, u32 callbackC);

static u32 FSYSAlign32(u32 size) {
    return (size + 0x1F) & ~0x1Fu;
}

static FSYSFileEntry* FSYSGetEntryByIndex(FSYSSlot* slot, u32 index) {
    u8* archive;
    u32* firstTable;
    u32* entryTable;

    archive = (u8*)slot->archiveData;
    if (archive == NULL) {
        return NULL;
    }

    firstTable = (u32*)(archive + *(u32*)(archive + 0x18));
    entryTable = (u32*)(archive + firstTable[0]);
    return (FSYSFileEntry*)(archive + entryTable[index]);
}

static DecompPoolEntry* FSYSFindDecompPool(u32 groupID) {
    DecompPoolEntry* pool;
    u32 i;

    pool = gDecompPoolBase;
    for (i = 0; i < gDVDPoolState; i++) {
        if (pool->fileID == groupID) {
            return pool;
        }
        pool = (DecompPoolEntry*)((u8*)pool + FSYS_DECOMP_ENTRY_SIZE);
    }

    return NULL;
}

static void* FSYSAllocHandle2B00(u32 size) {
    u16 handle;

    handle = fn_800E2B00(FSYSAlign32(size), 0x20);
    if (handle != 0) {
        return fn_800E27B0(handle);
    }
    return NULL;
}

static void* FSYSAllocHandle2C04(u32 size) {
    u16 handle;

    handle = fn_800E2C04(FSYSAlign32(size), 0x20);
    if (handle != 0) {
        return fn_800E27B0(handle);
    }
    return NULL;
}

static void FSYSFreeByPointer(void* ptr) {
    u16 handle;

    handle = fn_800E202C(ptr);
    if (handle != 0) {
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }
}

static s32 FSYSFindHandleID(s32 handleID) {
    FSYSFileHandle* table;
    s32 i;

    table = (FSYSFileHandle*)lbl_8047B1B8;
    for (i = 0; i < (s32)lbl_8047B1BC; i++, table++) {
        if (table->handleID == handleID) {
            return table->handleID;
        }
    }

    return -1;
}

static s32 FSYSRemoveHandleID(s32 handleID) {
    FSYSFileHandle* table;
    s32 found;
    s32 i;

    table = (FSYSFileHandle*)lbl_8047B1B8;
    found = -1;
    for (i = 0; i < (s32)lbl_8047B1BC; i++) {
        if (table[i].handleID == handleID) {
            table[i].handleID = -1;
            found = i;
            break;
        }
    }

    if (found < 0) {
        return -1;
    }

    for (i = found; i < (s32)lbl_8047B1BC - 1; i++) {
        table[i] = table[i + 1];
    }

    lbl_8047B1BC--;
    table[lbl_8047B1BC].handleID = -1;
    return 0;
}

static void FSYSEvictUntilSpace(FSYSSlot* slot, u32 neededSize) {
    s32 handleID;

    if (FSYSFindHandleID((s32)slot->field_08) >= 0) {
        return;
    }

    while (neededSize > fn_8017FA5C()) {
        handleID = ((FSYSFileHandle*)lbl_8047B1B8)->handleID;
        if (handleID < 0) {
            break;
        }
        fn_8017F800((u32)handleID);
        if (FSYSRemoveHandleID(handleID) < 0) {
            break;
        }
    }
}

static void FSYSRecomputeArchiveSize(FSYSSlot* slot) {
    u32 i;
    u32 total;
    FSYSFileEntry* entry;
    u8* archive;

    total = 0;
    for (i = 0; i < slot->numEntries; i++) {
        entry = FSYSGetEntryByIndex(slot, i);
        if (entry != NULL) {
            total += entry->decompressedSize;
        }
    }

    archive = (u8*)slot->archiveData;
    if (archive != NULL) {
        total += *(u32*)(archive + slot->field_18 + 8);
    }
    slot->totalDecompSize = total;
}

static u32 FSYSRefreshExternalEntrySize(FSYSSlot* slot, FSYSFileEntry* entry) {
    u8* archive;
    u32 fileInfo;

    archive = (u8*)slot->archiveData;
    if (archive == NULL || (*(u32*)(archive + 0x10) & 1) == 0) {
        return entry->decompressedSize;
    }

    FSYS_SLOT_FILE1(slot) = 0;
    if (fn_80167EF8(archive + entry->dataOffset) != 0) {
        FSYS_SLOT_FILE1(slot) = fn_80167F28((char*)archive + entry->dataOffset);
    }

    fileInfo = FSYS_SLOT_FILE1(slot);
    if (fileInfo != 0) {
        entry->decompressedSize = fn_80167E5C(fileInfo);
        FSYSRecomputeArchiveSize(slot);
        fn_80167E64(fileInfo);
        FSYS_SLOT_FILE1(slot) = 0;
        FSYSFileEntry_GetSubEntry(entry)->ready = 1;
    }

    return entry->decompressedSize;
}

static void* FSYSAllocEntryBuffer(FSYSSlot* slot, FSYSFileEntry* entry, u32 size) {
    DecompPoolEntry* pool;
    FSYSAllocCallback callback;

    pool = FSYSFindDecompPool(entry->groupID);
    if (pool != NULL && FSYS_POOL_ALLOC_CB(pool) != NULL) {
        callback = (FSYSAllocCallback)FSYS_POOL_ALLOC_CB(pool);
        return callback(slot->fileHandle, entry->nameHash, size);
    }

    return (void*)GSresAllocResourceAlign(FSYSAlign32(size), 0x20, slot->fileHandle, entry->nameHash, 0);
}

static void FSYSRunDoneCallback(FSYSSlot* slot, FSYSFileEntry* entry) {
    DecompPoolEntry* pool;
    FSYSDoneCallback callback;
    void* cached;
    u32 size;

    pool = FSYSFindDecompPool(entry->groupID);
    if (pool == NULL || FSYS_POOL_DONE_CB(pool) == NULL) {
        return;
    }

    cached = GSresGetResource(slot->fileHandle, entry->nameHash);
    if (entry->flags & FSYS_COMPRESSED_FLAG) {
        size = entry->compressedSize;
    } else {
        size = entry->decompressedSize;
    }

    if (cached != NULL) {
        DCFlushRange(cached, size);
    }

    callback = (FSYSDoneCallback)FSYS_POOL_DONE_CB(pool);
    callback(slot->fileHandle, entry->nameHash, size);
}

static void FSYSReadEntryAsync(FSYSSlot* slot, FSYSSubEntry* sub, FSYSFileEntry* entry) {
    u8* archive;
    u32 size;

    archive = (u8*)slot->archiveData;
    size = FSYSAlign32(entry->decompressedSize);

    if (archive != NULL && (*(u32*)(archive + 0x10) & 1) != 0) {
        FSYS_SLOT_FILE1(slot) = 0;
        if (fn_80167EF8(archive + entry->dataOffset) != 0) {
            FSYS_SLOT_FILE1(slot) = fn_80167F28((char*)archive + entry->dataOffset);
        }

        if (FSYS_SLOT_FILE1(slot) != 0) {
            fn_80167E98(FSYS_SLOT_FILE1(slot), sub->buffer, size, 0, fn_8017F108);
            return;
        }
    }

    fn_80167E98(FSYS_SLOT_FILE0(slot), sub->buffer, size, entry->padding04, fn_8017F108);
}

static void FSYSScheduleSceneRead(FSYSSlot* slot, FSYSFileEntry* entry, FSYSSubEntry* sub) {
    u8* archive;
    u32 cached;

    archive = (u8*)slot->archiveData;
    cached = 0;
    if (archive != NULL) {
        cached = fn_8017F794(*(u32*)(archive + 8), entry->groupID, entry->nameHash);
    }

    if (cached != 0) {
        sub->state = 4;
        slot->status = FSYS_STATUS_ERROR;
        return;
    }

    sub->state = 3;
    slot->status = 0x12D;
    fn_80179FA4(slot, entry->padding04, entry->decompressedSize, 0, slot, 0,
                archive != NULL ? archive + entry->dataOffset : NULL, entry);
}

/* 0x8017B4BC | 0xE8 */
extern u32 lbl_8047B1B4;
#if 0
asm void fn_8017B4BC(void) {
#include "src/game/fsys/fsys_file_fn_8017B4BC.inc"
}
#else
#pragma push
#pragma optimization_level 0
u32 fn_8017B4BC(u32 fileHandle, u32 index) {
    u8* archiveA;
    u8* secondaryTable;
    u32 volatile result;
    u8* volatile archiveCopy;
    u8* archiveB;
    u8* volatile secondaryStore;
    u32* firstTableA;
    u32* firstTableB;
    u32* entryTable;
    u32 i;
    u8 found;
    FSYSFileEntry* entry;
    FSYSFileEntry* spareA;
    FSYSFileEntry* spareB;
    FSYSFileEntry* spareC;
    FSYSSlot* slot;

    slot = (FSYSSlot*)lbl_8047B1B4;
    slot = (FSYSSlot*)lbl_8047B1B4;
    spareA = spareA;
    spareB = spareB;
    spareC = spareC;
    for (i = 0; i < lbl_80453FEC.maxSlots; i++) {
        found = found;
        archiveA = (u8*)slot->archiveData;
        if (archiveA) {
            firstTableA = (u32*)(archiveA + *(u32*)(archiveA + 0x18));
            secondaryTable = archiveA + firstTableA[1];
        } else {
            secondaryTable = NULL;
        }
        secondaryStore = secondaryTable;
        if ((s32)slot->status != FSYS_STATUS_FREE && slot->fileHandle == fileHandle &&
            (s32)slot->padding05C != 0) {
            archiveB = (u8*)slot->archiveData;
            archiveCopy = (u8*)slot->archiveData;
            if (archiveB) {
                firstTableB = (u32*)(archiveB + *(u32*)(archiveB + 0x18));
                entryTable = (u32*)(archiveB + firstTableB[0]);
                entry = (FSYSFileEntry*)(archiveB + entryTable[index]);
            } else {
                entry = NULL;
            }
            result = entry->nameHash;
            found = 1;
            goto done;
        }
        slot++;
    }
    found = 0;

done:
    if (found) {
        return result;
    }
    return 0;
}
#pragma pop
#endif

/* 0x8017B5A4 | 0x1C */
#pragma push
#pragma optimization_level 0
u32 fn_8017B5A4(u32 val) {
    u32 result;

    result = (val >> 9) & 0x3F;
    return result;
}
#pragma pop

/* 0x8017B5C0 | 0xF8 */
extern void fn_8017BD34();
extern u32 fn_8017F728();
extern void fn_8017A814(void);
extern void* fn_80180584();
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8017B5C0(void) {
#include "src/game/fsys/fsys_file_fn_8017B5C0.inc"
}
#else
void fn_8017B5C0(FSYSSlot* slot, FSYSFileEntry* entry, u32 index) {
    FSYSSubEntry* sub;
    u32 cached;
    void* ptr;
    u16 handle;
    u32 offset;
    u32 sizeA;
    u32 sizeC;
    u32 sizeB;
    void* zero;

    if (entry->flags & FSYS_COMPRESSED_FLAG) {
        sub = FSYS_SLOT_CURRENT_SUB(slot);
        sub->state = 5;
        zero = NULL;
        slot->status = 0x65;
        cached = cached;
        sizeA = (entry->decompressedSize + 0x1F) & ~0x1Fu;
        sizeB = sizeA;
        sizeC = (sizeB + 0x1F) & ~0x1Fu;
        handle = fn_800E2B00(sizeC, 0x20);
        if (handle != 0) {
            ptr = fn_800E27B0(handle);
        } else {
            ptr = NULL;
        }
        sub->buffer = ptr;

        cached = fn_8017F794(slot->fileHandle, entry->groupID, entry->nameHash);
        offset = fn_8017F728(slot->fileHandle, entry->groupID, entry->nameHash);
        fn_80180584(sub->buffer, (void*)cached, offset, fn_8017A814, slot);
    } else {
        fn_8017BD34(slot, entry, index);
    }
}
#endif
#pragma pop

/* 0x8017B6B8 | 0x4C8 */
extern void fn_8017F25C();
extern u32 lbl_80478C48;
extern u8 lbl_8036C2A0[];
#if 0
asm void fn_8017B6B8(void) {
#include "src/game/fsys/fsys_file_fn_8017B6B8.inc"
}
#else
void fn_8017B6B8(FSYSSlot* slot, FSYSFileEntry* entry, u32 index) {
    FSYSSubEntry* sub;
    FSYSFileEntry* currentEntry;
    void* cached;
    u32 size;
    u32 archiveKey;

    sub = FSYS_SLOT_CURRENT_SUB(slot);
    sub->state = 1;
    slot->status = 0x65;

    if (entry->flags & FSYS_COMPRESSED_FLAG) {
        sub->buffer = FSYSAllocHandle2B00(entry->decompressedSize);
        slot->padding100 = (u32)sub->buffer;
    } else {
        DecompPoolEntry* pool;
        u8* archive;
        u32 fileInfo;
        u32 i;
        u32 total;
        FSYSFileEntry* totalEntry;

        size = entry->decompressedSize;
        archive = (u8*)slot->archiveData;
        if ((*(u32*)(archive + 0x10) & 1) != 0) {
            FSYS_SLOT_FILE1(slot) = 0;
            if (fn_80167EF8(archive + entry->dataOffset) != 0) {
                FSYS_SLOT_FILE1(slot) = fn_80167F28((char*)archive + entry->dataOffset);
            }

            fileInfo = FSYS_SLOT_FILE1(slot);
            if (fileInfo != 0) {
                size = fn_80167E5C(fileInfo);
                entry->decompressedSize = size;
                total = 0;
                for (i = 0; i < slot->numEntries; i++) {
                    totalEntry = FSYSGetEntryByIndex(slot, i);
                    total += totalEntry->decompressedSize;
                }
                total += *(u32*)(archive + slot->field_18 + 8);
                slot->totalDecompSize = total;
                fn_80167E64(fileInfo);
                FSYS_SLOT_FILE1(slot) = 0;
                FSYSFileEntry_GetSubEntry(entry)->ready = 1;
            }
        }
        pool = gDecompPoolBase;
        for (i = 0; i < gDVDPoolState; i++) {
            if (pool->fileID == entry->groupID) {
                break;
            }
            pool = (DecompPoolEntry*)((u8*)pool + FSYS_DECOMP_ENTRY_SIZE);
        }
        if (i == gDVDPoolState) {
            pool = NULL;
        }

        if (FSYS_POOL_ALLOC_CB(pool) != NULL) {
            sub->buffer = ((FSYSAllocCallback)FSYS_POOL_ALLOC_CB(pool))(
                slot->fileHandle, entry->nameHash, size);
        } else {
            sub->buffer = (void*)GSresAllocResourceAlign(
                FSYSAlign32(size), 0x20, slot->fileHandle, entry->nameHash, 0);
        }
    }

    if (sub->buffer == NULL) {
        slot->status = FSYS_STATUS_ERROR;
        sub->state = 7;
        return;
    }

    currentEntry = FSYSGetEntryByIndex(slot, index);
    archiveKey = 0;
    if (slot->archiveData != NULL) {
        archiveKey = *(u32*)((u8*)slot->archiveData + 8);
    }

    if (currentEntry != NULL &&
        fn_8017F794(archiveKey, currentEntry->groupID, currentEntry->nameHash) != 0) {
        currentEntry = FSYSGetEntryByIndex(slot, slot->archiveSize);
        if (currentEntry != NULL) {
            cached = (void*)fn_8017F794(slot->fileHandle, currentEntry->groupID,
                                        currentEntry->nameHash);
            slot->status = 0xA1;
            fn_80180584(sub->buffer, cached, currentEntry->decompressedSize,
                        fn_8017F25C, slot);
        }
        return;
    }

    if (currentEntry != NULL) {
        FSYSReadEntryAsync(slot, sub, currentEntry);
    }
}
#endif

typedef struct FSYSCallbackEntry8017BB80 {
    u32 field_00;
    u32 groupID;
    u32 field_08;
    void* (*callback)(u32 fileHandle, u32 nameHash, u32 size);
} FSYSCallbackEntry8017BB80;

s32 fn_8017BB80(FSYSSlot* slot, FSYSFileEntry* entry)
{
    FSYSCallbackEntry8017BB80* callbackEntry;
    void* resource;
    u32 i;
    u32 size;

    callbackEntry = (FSYSCallbackEntry8017BB80*)gDecompPoolBase;
    for (i = 0; i < gDVDPoolState; i++, callbackEntry++) {
        if (callbackEntry->groupID == entry->groupID) {
            break;
        }
    }
    if (i == gDVDPoolState) {
        callbackEntry = NULL;
    }

    if (callbackEntry->callback != NULL) {
        resource = GSresGetResource(slot->fileHandle, entry->nameHash);
        size = (entry->flags & 0x80000000) ? entry->compressedSize
                                           : entry->decompressedSize;
        if (resource != NULL) {
            DCFlushRange(resource, size);
        }
        callbackEntry->callback(slot->fileHandle, entry->nameHash, size);
    }
    return 0;
}

/* 0x8017BC90 | 0xA4 */
#pragma push
#pragma optimization_level 0
void* fn_8017BC90(void* unused0, u32 unused1, const void* compressed,
                  void* work, u32 unused4) {
    void* output;
    void* workCopy;
    s32 i;
    extern void fn_8017F2C4(void*, const void*, u32);

    workCopy = work;
    workCopy = workCopy;
    output = *(void**)((u8*)work + 4);
    if (output) {
        memcpy(&gLZSSContext, compressed, 0x10);
        for (i = 0; i < 0xFEE; i++) {
            gLZSSWindow[i] = 0;
        }
        fn_8017F2C4(output, compressed, gLZSSContext.decompSize);
        DCFlushRange(output, gLZSSContext.decompSize);
    }
    return output;
}
#pragma pop

/* 0x8017BD34 | 0x2B4 */
extern void fn_8017A95C(void);
extern u32 lbl_80478C48;
#if 0
asm void fn_8017BD34(void) {
#include "src/game/fsys/fsys_file_fn_8017BD34.inc"
}
#else
void fn_8017BD34(FSYSSlot* slot, FSYSFileEntry* entry) {
    FSYSSubEntry* sub;
    void* cached;
    u32 offset;
    u32 size;

    sub = FSYS_SLOT_CURRENT_SUB(slot);
    sub->state = 5;
    slot->status = 0x65;

    size = FSYSRefreshExternalEntrySize(slot, entry);
    sub->buffer = FSYSAllocEntryBuffer(slot, entry, size);

    cached = (void*)fn_8017F794(slot->fileHandle, entry->groupID, entry->nameHash);
    offset = fn_8017F728(slot->fileHandle, entry->groupID, entry->nameHash);
    fn_80180584(sub->buffer, cached, offset, fn_8017A95C, slot);
}
#endif

/* 0x8017C008 | 0x6C */
extern void fn_80180C78();
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8017C008(void) {
#include "src/game/fsys/fsys_file_fn_8017C008.inc"
}
#else
u32 fn_8017C008(FSYSSlot* slot) {
    FSYSSubEntry* sub;
    u32* buf;

    slot = slot;
    sub = FSYS_SLOT_CURRENT_SUB(slot);
    buf = (u32*)sub->buffer;
    if (*buf == FSYS_LZSS_MAGIC) {
        fn_80180C78(slot, sub, 0);
    }

    slot->status = FSYS_STATUS_ERROR;
    return 0;
}
#endif
#pragma pop

/* 0x8017C074 | 0x164 */
extern u32 lbl_80478C48;
#if 0
asm void fn_8017C074(void) {
#include "src/game/fsys/fsys_file_fn_8017C074.inc"
}
#else
void fn_8017C074(FSYSSlot* slot, FSYSSubEntry* sub, u32 index, void* work) {
    FSYSFileEntry* entry;
    DecompPoolEntry* pool;
    FSYSAllocCallback callback;
    void* buffer;

    entry = FSYSGetEntryByIndex(slot, index);
    if (entry == NULL) {
        return;
    }

    *(void**)((u8*)work + 0x38) = sub->buffer;
    sub->buffer = NULL;

    pool = FSYSFindDecompPool(entry->groupID);
    if (pool != NULL && FSYS_POOL_ALLOC_CB(pool) != NULL) {
        callback = (FSYSAllocCallback)FSYS_POOL_ALLOC_CB(pool);
        buffer = callback(slot->fileHandle, entry->nameHash, entry->compressedSize);
    } else {
        buffer = (void*)GSresAllocResourceAlign(FSYSAlign32(entry->compressedSize), 0x20,
                                    slot->field_08, entry->nameHash, 0);
    }

    sub->buffer = buffer;
    sub->buffer = buffer;
}
#endif

/* 0x8017C1D8 | 0x1BC */
extern u32 lbl_80478C48;
#if 0
asm void fn_8017C1D8(void) {
#include "src/game/fsys/fsys_file_fn_8017C1D8.inc"
}
#else
void fn_8017C1D8(FSYSSlot* slot, FSYSSubEntry* sub, u32 index, void* work) {
    FSYSFileEntry* entry;
    void* temp;

    entry = FSYSGetEntryByIndex(slot, index);
    temp = *(void**)((u8*)work + 0x38);
    FSYSFreeByPointer(temp);
    *(void**)((u8*)work + 0x38) = NULL;
    slot->padding100 = 0;

    if (sub->buffer == NULL) {
        sub->state = 7;
        return;
    }

    sub->state = 6;
    if (entry != NULL) {
        FSYSRunDoneCallback(slot, entry);
    }
}
#endif

/* 0x8017C39C | 0x78 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8017C39C(void) {
#include "src/game/fsys/fsys_file_fn_8017C39C.inc"
}
#else
u32 fn_8017C39C(FSYSSlot *slot)
{
    u8 *archive;
    FSYSSubEntry *sub;
    FSYSFileEntry *spare1;
    FSYSFileEntry *entry;
    u32 *entryTable;
    u32 *firstTable;
    FSYSFileEntry *spare2;
    u32 index;

    archive = (u8 *) slot->archiveData;
    index = slot->archiveSize;
    spare1 = spare1;
    if (archive) {
        firstTable = (u32 *) (archive + *((u32 *) (archive + 0x18)));
        entryTable = (u32 *) (archive + firstTable[0]);
        entry = (FSYSFileEntry *) (archive + entryTable[index]);
    } else {
        spare1 = spare1;
        entry = NULL;
    }
    sub = *((FSYSSubEntry **) ((u8 *) slot + 0xFC));
    spare2 = spare2;
    if (entry->flags & 0x80000000u) {
        sub->state = 4;
    } else {
        sub->state = 4;
    }
    slot->status = 0x064;
    return 0;
}
#endif
#pragma pop

/* 0x8017C414 | 0x154 */
extern s32 fn_8017A624(void*);
extern u32 OSDisableInterrupts(void);
extern void OSRestoreInterrupts(u32);
#if 0
asm void fn_8017C414(void) {
#include "src/game/fsys/fsys_file_fn_8017C414.inc"
}
#else
#pragma optimization_level 4
u32 fn_8017C414(void* arg) {
    u32 enabled;
    FSYSManager* mgr;
    FSYSSlot* slot;

    if (fn_8017A624(arg) != 0) {
        enabled = OSDisableInterrupts();
        mgr = &lbl_80453FEC;
        slot = mgr->activeSlot;

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
    return 1;
}
#endif

/* 0x8017C580 | 0x10 */
#if 0
asm void fn_8017C580(void) {
#include "src/game/fsys/fsys_file_fn_8017C580.inc"
}
#else
#pragma optimization_level 4
u32 fn_8017C580(FSYSSlot* slot) {
    slot->status = 0x66;
    return 0;
}
#endif

/* 0x8017C5A0 | 0x10 */
#if 0
asm void fn_8017C5A0(void) {
#include "src/game/fsys/fsys_file_fn_8017C5A0.inc"
}
#else
#pragma optimization_level 4
u32 fn_8017C5A0(FSYSSlot* slot) {
    slot->status = 0x66;
    return 0;
}
#endif

/* 0x8017C5B8 | 0x128 */
extern FSYSFileHandle* fn_8017D68C(FSYSSlot*);
extern u32 fn_8017F928();
extern void fn_80180694();
extern u32 lbl_8047B1B8;
extern u32 lbl_8047B1BC;
#if 0
asm void fn_8017C5B8(void) {
#include "src/game/fsys/fsys_file_fn_8017C5B8.inc"
}
#else
#pragma optimization_level 4
u32 fn_8017C5B8(FSYSSlot* slot) {
    u8* archive;
    u32 fileIndex;
    u8* entry;
    u32* firstTable;
    u32* entryTable;
    u32 result;
    u32 size;
    u8* subEntry;
    FSYSFileHandle* handleEntry;
    s32 i;
    volatile u32 saveResult;

    archive = slot->archiveData;
    fileIndex = slot->archiveSize;
    if (archive != NULL) {
        firstTable = (u32*)(archive + *(u32*)(archive + 0x18));
        entryTable = (u32*)(archive + *firstTable);
        entry = archive + entryTable[fileIndex];
    } else {
        entry = NULL;
    }

    subEntry = (u8*)slot->fileIndex;
    result = (u32)fn_8017F794(slot->fileHandle, *(u32*)(entry + 0x20), *(u32*)entry);
    if (result == 0) {
        size = *(u32*)(entry + 0x14);
        result = (u32)fn_8017F928(size, slot->fileHandle, *(u32*)(entry + 0x20), *(u32*)entry);
        if (result == 0) {
            slot->status = 0x98;
        } else {
            if ((s32)slot->loadMode == 3) {
                fn_8017D68C(slot);
                handleEntry = (FSYSFileHandle*)lbl_8047B1B8;
                i = 0;
                while (i < (s32)*(volatile u32*)&lbl_8047B1BC) {
                    if (handleEntry->handleID == (s32)slot->field_08) {
                        break;
                    }
                    handleEntry++;
                    i++;
                }
            }
            slot->status = 0x97;
            fn_80180694(*(void**)(subEntry + 4), result, size, fn_8017F25C, slot);
        }
    } else {
        slot->status = 0x98;
    }

    saveResult = result;
    return 1;
}
#endif

/* 0x8017C6E0 | 0x1AC */
extern u32 lbl_80478C48;
#if 0
asm void fn_8017C6E0(void) {
#include "src/game/fsys/fsys_file_fn_8017C6E0.inc"
}
#else
u32 fn_8017C6E0(FSYSSlot* slot) {
    FSYSFileEntry* entry;
    FSYSSubEntry* sub;

    entry = FSYSGetEntryByIndex(slot, slot->archiveSize);
    sub = FSYS_SLOT_CURRENT_SUB(slot);
    if (entry == NULL || sub == NULL) {
        slot->status = FSYS_STATUS_ERROR;
        return 0;
    }

    DCFlushRange(sub->buffer, entry->decompressedSize);
    if (entry->flags & FSYS_COMPRESSED_FLAG) {
        slot->status = 0x67;
        sub->state = 2;
        return 0;
    }

    sub->state = 6;
    FSYSRunDoneCallback(slot, entry);
    slot->status = FSYS_STATUS_ERROR;
    return 0;
}
#endif

/* 0x8017C894 | 0x2C */
extern void fn_8017D8F8();
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_8017C894(u32 arg) {
    fn_8017D8F8(arg);
    return 0;
}
#pragma pop

/* 0x8017C8C8 | 0x2C */
extern void fn_8017D92C();
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_8017C8C8(u32 arg) {
    fn_8017D92C(arg);
    return 0;
}
#pragma pop

/* 0x8017C8FC | 0x580 */
#if 0
asm void fn_8017C8FC(void) {
#include "src/game/fsys/fsys_file_fn_8017C8FC.inc"
}
#else
u32 fn_8017C8FC(FSYSSlot* slot) {
    u32 i;
    FSYSFileEntry* entry;
    FSYSSubEntry* sub;
    u32 cached;

    FSYS_SLOT_CURRENT_SUB(slot) = NULL;

    if ((s32)slot->loadMode == 3) {
        for (i = 0; i < slot->numEntries; i++) {
            entry = FSYSGetEntryByIndex(slot, i);
            if (entry == NULL || entry->nameHash != slot->requestID) {
                continue;
            }

            slot->archiveSize = i;
            sub = FSYSFileEntry_GetSubEntry(entry);
            cached = fn_8017F794(slot->fileHandle, entry->groupID, entry->nameHash);

            if (sub->state == 0) {
                FSYS_SLOT_CURRENT_SUB(slot) = sub;
                fn_8017B6B8(slot, entry, i);
                break;
            }

            if (sub->state == 4 && (s32)slot->reloadFlag == 1) {
                FSYS_SLOT_CURRENT_SUB(slot) = sub;
                if (cached == 0) {
                    fn_8017B6B8(slot, entry, i);
                } else {
                    fn_8017B5C0(slot, entry, i);
                }
                break;
            }
            if (sub->state == 6 && (s32)slot->reloadFlag == 1) {
                FSYS_SLOT_CURRENT_SUB(slot) = sub;
                if (cached == 0) {
                    fn_8017B6B8(slot, entry, i);
                } else {
                    fn_8017B5C0(slot, entry, i);
                }
                break;
            }
            break;
        }
    } else {
        for (i = 0; i < slot->numEntries; i++) {
            slot->archiveSize = i;
            entry = FSYSGetEntryByIndex(slot, i);
            if (entry == NULL) {
                continue;
            }

            sub = FSYSFileEntry_GetSubEntry(entry);
            cached = fn_8017F794(slot->fileHandle, entry->groupID, entry->nameHash);

            if (sub->state == 0) {
                if (entry->decompressedSize == 0) {
                    switch (slot->loadMode) {
                        case 0:
                        case 2:
                        case 7:
                            sub->state = 6;
                            break;
                        default:
                            break;
                    }
                    continue;
                }

                switch (slot->loadMode) {
                    case 0:
                        FSYS_SLOT_CURRENT_SUB(slot) = sub;
                        fn_8017B6B8(slot, entry, i);
                        break;
                    case 2:
                    case 7:
                        FSYS_SLOT_CURRENT_SUB(slot) = sub;
                        FSYSScheduleSceneRead(slot, entry, sub);
                        break;
                    default:
                        break;
                }
                if (FSYS_SLOT_CURRENT_SUB(slot) != NULL) {
                    break;
                }
            } else if (sub->state == 4 && (s32)slot->reloadFlag == 1) {
                if (slot->loadMode == 0) {
                    FSYS_SLOT_CURRENT_SUB(slot) = sub;
                    if (cached == 0) {
                        fn_8017B6B8(slot, entry, i);
                    } else {
                        fn_8017B5C0(slot, entry, i);
                    }
                    break;
                }
            }
        }
    }

    if (FSYS_SLOT_CURRENT_SUB(slot) == NULL) {
        slot->status = 0x7D0;
        return 1;
    }

    return 0;
}
#endif

/* 0x8017CE7C | 0x4C */
extern void fn_8017D960();
extern void fn_8017DAB8();
#if 0
asm void fn_8017CE7C(void) {
#include "src/game/fsys/fsys_file_fn_8017CE7C.inc"
}
#else
#pragma optimization_level 0
u32 fn_8017CE7C(FSYSSlot* slot) {
    if ((s32)slot->loadMode == 3) {
        fn_8017D960(slot);
    } else {
        fn_8017DAB8(slot);
    }
    return 1;
}
#endif

/* 0x8017CED8 | 0x4C8 */
extern void fn_8017F800(u32);
extern void* fn_80180450();
extern u32 lbl_8047B1B8;
extern u32 lbl_8047B1BC;
#if 0
asm void fn_8017CED8(void) {
#include "src/game/fsys/fsys_file_fn_8017CED8.inc"
}
#else
u32 fn_8017CED8(FSYSSlot* slot) {
    u32 archiveSize;
    u32 alignedArchiveSize;
    u32 cached;
    u32 i;
    FSYSFileEntry* entry;
    FSYSSubEntry* sub;

    archiveSize = slot->field_1C;
    cached = fn_8017F794(slot->fileHandle, 0, 1);
    if (cached == 0) {
        FSYSFileHandle* table;
        s32 handleID;
        s32 found;
        s32 j;

        alignedArchiveSize = FSYSAlign32(archiveSize);
        DCFlushRange(slot->archiveData, alignedArchiveSize);
        table = (FSYSFileHandle*)lbl_8047B1B8;
        found = -1;
        for (j = 0; j < (s32)lbl_8047B1BC; j++) {
            if (table[j].handleID == (s32)slot->field_08) {
                found = table[j].handleID;
                break;
            }
        }
        if (found < 0) {
            while (alignedArchiveSize > fn_8017FA5C()) {
                table = (FSYSFileHandle*)lbl_8047B1B8;
                handleID = table[0].handleID;
                if (handleID < 0) {
                    break;
                }
                fn_8017F800((u32)handleID);

                found = -1;
                table = (FSYSFileHandle*)lbl_8047B1B8;
                for (j = 0; j < (s32)lbl_8047B1BC; j++) {
                    if (table[j].handleID == handleID) {
                        table[j].handleID = -1;
                        found = j;
                        break;
                    }
                }
                if (found < 0) {
                    break;
                }

                for (j = 0; j < (s32)lbl_8047B1BC - 1; j++) {
                    if (j >= found) {
                        table[j] = table[j + 1];
                    }
                }
                lbl_8047B1BC--;
                table[lbl_8047B1BC].handleID = -1;
            }
        }
        cached = fn_8017F928(alignedArchiveSize, slot->fileHandle, 0, 1);
        if (cached != 0) {
            fn_80180450(slot->archiveData, (void*)cached, alignedArchiveSize);
        }
    }

    FSYSEvictUntilSpace(slot, slot->totalDecompSize);
    fn_8017D68C(slot);

    for (i = 0; i < slot->numEntries; i++) {
        entry = FSYSGetEntryByIndex(slot, i);
        if (entry != NULL) {
            sub = FSYSFileEntry_GetSubEntry(entry);
            sub->state = 0;
            sub->buffer = NULL;
            sub->ready = 0;
        }
    }

    slot->padding05C = 1;
    slot->status = FSYS_STATUS_ERROR;
    return 0;
}
#endif

/* 0x8017D3A0 | 0x34 */
#if 0
asm void fn_8017D3A0(void) {
#include "src/game/fsys/fsys_file_fn_8017D3A0.inc"
}
#else
#pragma optimization_level 4
u32 fn_8017D3A0(FSYSSlot* slot) {
    switch (slot->padding054) {
        case 4:
        case 5:
        case 11:
            return 1;
        default:
            return 0;
    }
}
#endif

/* 0x8017D3D4 | 0x2C */
extern void fn_8017DB74();
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_8017D3D4(u32 arg) {
    fn_8017DB74(arg);
    return 0;
}
#pragma pop

/* 0x8017D56C | 0xB8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_8017D56C(u32 fileHandle) {
    FSYSSlot* slot;
    extern void fn_8017DF4C(FSYSSlot*, u32, u32, u32, u32);

    if (slot = fn_8017D410(fileHandle, 7)) {
        if ((s32)slot->status == 0x3E8) {
            if ((s32)slot->loadMode == 2 || (s32)slot->loadMode == 7) {
                return 0;
            }
            slot->status = 0;
            fn_8017DF4C(slot, fileHandle, 0, 0, 0);
            return 1;
        }
        fn_8017DF4C(slot, fileHandle, 0, 0, 0);
        return 1;
    }
    return 0;
}
#pragma pop

/* 0x8017D624 | 0x68 */
extern u32 lbl_8047B1B8;
extern u32 lbl_8047B1BC;
extern void fn_8017F800();
#if 0
asm void fn_8017D624(void) {
#include "src/game/fsys/fsys_file_fn_8017D624.inc"
}
#else
#pragma optimization_level 0
void fn_8017D624(void) {
    FSYSFileHandle* table = (FSYSFileHandle*)lbl_8047B1B8;
    s32 i;

    for (i = 0; i < (s32)lbl_8047B1BC; i++) {
        fn_8017F800(table[i].handleID);
        table[i].handleID = -1;
    }
    lbl_8047B1BC = 0;
}
#endif

/* 0x8017D68C | 0x174 */
extern u32 lbl_8047B1B8;
extern u32 lbl_8047B1BC;
#pragma optimization_level 0
FSYSFileHandle* fn_8017D68C(FSYSSlot* slot) {
    FSYSFileHandle* table;
    FSYSFileHandle saved;
    s32 foundIndex;
    s32 i;
    s32 handleID;

    foundIndex = -1;
    table = (FSYSFileHandle*)lbl_8047B1B8;
    handleID = slot->field_08;
    if (slot->field_10 & 0x40000000) {
        return NULL;
    }

    i = 0;
    while (i < 0x64) {
        if (table->handleID == handleID) {
            foundIndex = i;
            memcpy(&saved, (void*)table, 8);
            break;
        }
        table++;
        i++;
    }

    if (foundIndex >= 0) {
        table = (FSYSFileHandle*)lbl_8047B1B8;
        i = 0;
        while (i < (s32)lbl_8047B1BC - 1) {
            if (i >= foundIndex) {
                table[i] = table[i + 1];
            }
            i++;
        }
        table[i].handleID = saved.handleID;
        table[i].userData = saved.userData;
    } else {
        table = (FSYSFileHandle*)lbl_8047B1B8;
        if ((s32)lbl_8047B1BC == 0x64) {
            i = 0;
            while (i < 0x63) {
                table[i] = table[i + 1];
                i++;
            }
            lbl_8047B1BC = lbl_8047B1BC - 1;
        }
        table = &((FSYSFileHandle*)lbl_8047B1B8)[lbl_8047B1BC++];
        table->handleID = handleID;
        table->userData = 0;
    }

    return table;
}

/* 0x8017D800 | 0xF8 */
extern void fn_8017DEA4();
extern void fn_8017DF4C();
extern void fn_8017DFF4();
extern void fn_8017E09C();
extern u32 lbl_8047B1B4;
#if 0
asm void fn_8017D800(void) {
#include "src/game/fsys/fsys_file_fn_8017D800.inc"
}
#else
void fn_8017D800(void) {
    u32 i;
    FSYSSlot* slot;

    slot = (FSYSSlot*)lbl_8047B1B4;
    for (i = 0; i < lbl_80453FEC.maxSlots; i++) {
        if ((s32)slot->status == FSYS_STATUS_PENDING) {
            switch (slot->loadMode) {
                case 0:
                    fn_8017E09C(slot, slot->fileHandle, slot->callbackA,
                                slot->callbackB, slot->callbackC);
                    return;
                case 1:
                    fn_8017DEA4(slot, slot->fileHandle, slot->callbackA,
                                slot->callbackB, slot->callbackC);
                    return;
                case 2:
                    fn_8017DFF4(slot, slot->fileHandle, slot->callbackA,
                                slot->callbackB, slot->callbackC);
                    break;
                case 7:
                    fn_8017DF4C(slot, slot->fileHandle, slot->callbackA,
                                slot->callbackB, slot->callbackC);
                    break;
            }
        } else {
            slot++;
        }
    }
}
#endif

/* 0x8017D8F8 | 0x34 */
#if 0
asm void fn_8017D8F8(void) {
#include "src/game/fsys/fsys_file_fn_8017D8F8.inc"
}
#else
#pragma optimization_level 4
#pragma dont_inline on
void fn_8017D8F8(FSYSSlot* slot) {
    fn_8017D960(slot);
    slot->status = FSYS_STATUS_FREE;
}
#endif

/* 0x8017D92C | 0x34 */
#if 0
asm void fn_8017D92C(void) {
#include "src/game/fsys/fsys_file_fn_8017D92C.inc"
}
#else
#pragma optimization_level 4
void fn_8017D92C(FSYSSlot* slot) {
    fn_8017D960(slot);
    slot->status = FSYS_STATUS_FREE;
}
#pragma dont_inline reset
#endif

/* 0x8017D960 | 0x158 */
#if 0
asm void fn_8017D960(void) {
#include "src/game/fsys/fsys_file_fn_8017D960.inc"
}
#else
void fn_8017D960(FSYSSlot* slot) {
    FSYSFileEntry* entry;
    u32 i;
    FSYSUserCallback callback;

    slot->status = FSYS_STATUS_LOADED;

    if ((s32)slot->reloadFlag == 1) {
        if ((s32)slot->loadMode == 3) {
            for (i = 0; i < slot->numEntries; i++) {
                entry = FSYSGetEntryByIndex(slot, i);
                if (entry != NULL && entry->nameHash == slot->requestID) {
                    slot->archiveSize = i;
                    break;
                }
            }
        } else {
            for (i = 0; i < slot->numEntries; i++) {
                entry = FSYSGetEntryByIndex(slot, i);
                (void)entry;
                slot->archiveSize = i;
            }
        }
        slot->loadMode = 2;
    }

    if (FSYS_SLOT_FILE0(slot) != 0) {
        fn_80167E64(FSYS_SLOT_FILE0(slot));
        FSYS_SLOT_FILE0(slot) = 0;
        gFSYSManager.activeSlot = NULL;
        fn_8017D800();
    }

    if (slot->callbackA != 0) {
        callback = (FSYSUserCallback)slot->callbackA;
        callback(slot->loadMode, slot->callbackB, slot->callbackC);
    }
}
#endif

/* 0x8017DAB8 | 0xBC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8017DAB8(void) {
#include "src/game/fsys/fsys_file_fn_8017DAB8.inc"
}
#else
void fn_8017DAB8(FSYSSlot* slot) {
    FSYSSubEntry* sub;
    u32 i;
    u8* archive;
    s32 flag;
    FSYSFileEntry* entry;
    u32* entryTable;
    u32* firstTable;
    FSYSFileEntry* spare;
    FSYSFileEntry* e2;

    sub = sub;
    flag = 0;
    for (i = 0; i < slot->numEntries; i++) {
        archive = (u8*)slot->archiveData;
        flag = flag;
        if (archive) {
            firstTable = (u32*)(archive + *(u32*)(archive + 0x18));
            entryTable = (u32*)(archive + firstTable[0]);
            entry = (FSYSFileEntry*)(archive + entryTable[i]);
        } else {
            entry = NULL;
            e2 = entry;
        }
        slot->archiveSize = i;
        sub = (FSYSSubEntry*)((u8*)e2 + 0x28);
        spare = spare;
        if (((*(s32*)((u8*)entry + 0x28) != 4) &&
             ((s32)sub->state != 6)) &&
            ((s32)sub->state != 7)) {
            flag = 1;
            break;
        }
    }
    if (flag == 0) {
        fn_8017D960(slot);
    }
}
#endif
#pragma pop

/* 0x8017DB74 | 0x330 */
extern u32 lbl_8047B1B8;
extern u32 lbl_8047B1BC;
#if 0
asm void fn_8017DB74(void) {
#include "src/game/fsys/fsys_file_fn_8017DB74.inc"
}
#else
void fn_8017DB74(FSYSSlot* slot) {
    u32 headerSize;
    u32 archiveSize;
    u32 cached;
    void* archive;

    headerSize = 0x40;
    DCFlushRange(slot, headerSize);

    if ((s32)slot->loadMode == 7) {
        slot->field_10 |= 0x40000000u;
    }

    cached = fn_8017F794(slot->fileHandle, 0, 0);
    if (cached == 0) {
        FSYSFileHandle* table;
        s32 handleID;
        s32 found;
        s32 j;

        table = (FSYSFileHandle*)lbl_8047B1B8;
        found = -1;
        for (j = 0; j < (s32)lbl_8047B1BC; j++) {
            if (table[j].handleID == (s32)slot->field_08) {
                found = table[j].handleID;
                break;
            }
        }
        if (found < 0) {
            while (FSYSAlign32(headerSize) > fn_8017FA5C()) {
                table = (FSYSFileHandle*)lbl_8047B1B8;
                handleID = table[0].handleID;
                if (handleID < 0) {
                    break;
                }
                fn_8017F800((u32)handleID);
                found = -1;
                table = (FSYSFileHandle*)lbl_8047B1B8;
                for (j = 0; j < (s32)lbl_8047B1BC; j++) {
                    if (table[j].handleID == handleID) {
                        table[j].handleID = -1;
                        found = j;
                        break;
                    }
                }
                if (found < 0) {
                    break;
                }
                for (j = 0; j < (s32)lbl_8047B1BC - 1; j++) {
                    if (j >= found) {
                        table[j] = table[j + 1];
                    }
                }
                lbl_8047B1BC--;
                table[lbl_8047B1BC].handleID = -1;
            }
        }
        cached = fn_8017F928(FSYSAlign32(headerSize), slot->fileHandle, 0, 0);
        if (cached != 0) {
            fn_80180450(slot, (void*)cached, FSYSAlign32(headerSize));
        }
    }

    archiveSize = slot->field_1C;
    archive = FSYSAllocHandle2C04(archiveSize);
    slot->archiveData = archive;

    cached = fn_8017F794(slot->fileHandle, 0, 1);
    if (cached != 0 && archive != NULL) {
        fn_80180320(archive, (void*)cached, FSYSAlign32(archiveSize));
        DCFlushRange(archive, FSYSAlign32(archiveSize));
        slot->status = 4;
        return;
    }

    slot->status = 3;
    if (archive != NULL) {
        memcpy(archive, slot, headerSize);
        fn_80167E98(FSYS_SLOT_FILE0(slot), (u8*)archive + headerSize,
                    (archiveSize - 0x21) & ~0x1Fu, headerSize, fn_8017F108);
    }
}
#endif

/* 0x8017DEA4 | 0xA8 */
extern void fn_80167DD8();
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8017DEA4(FSYSSlot* slot, u32 fileHandle, u32 callbackA,
                 u32 callbackB, u32 callbackC) {
    _fsysGetFilename(slot, fileHandle, callbackA, callbackB, callbackC, 1);

    if (lbl_80453FEC.activeSlot == slot) {
        if (FSYS_SLOT_FILE0(slot) == 0) {
            FSYS_SLOT_FILE0(slot) = fn_80167F28(slot->filename);
        }

        if (FSYS_SLOT_FILE0(slot) != 0) {
            fn_80167DD8(FSYS_SLOT_FILE0(slot), 0, fn_8017F108);
        } else {
            fn_8017D92C(slot);
        }
    }
}
#pragma pop

/* 0x8017DF4C | 0xA8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8017DF4C(void) {
#include "src/game/fsys/fsys_file_fn_8017DF4C.inc"
}
#else
void fn_8017DF4C(FSYSSlot* slot, u32 fileHandle, u32 callbackA,
                 u32 callbackB, u32 callbackC) {
    u32 len;

    _fsysGetFilename(slot, fileHandle, callbackA, callbackB, callbackC, 7);

    if (lbl_80453FEC.activeSlot == slot) {
        if (FSYS_SLOT_FILE0(slot) == 0) {
            FSYS_SLOT_FILE0(slot) = fn_80167F28(slot->filename);
        }
        len = 0x40;
        fn_80167E98(FSYS_SLOT_FILE0(slot), slot, (len + 0x1F) & ~0x1Fu, 0, fn_8017F108);
    }
}
#endif
#pragma pop

/* 0x8017DFF4 | 0xA8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8017DFF4(void) {
#include "src/game/fsys/fsys_file_fn_8017DFF4.inc"
}
#else
void fn_8017DFF4(FSYSSlot* slot, u32 fileHandle, u32 callbackA,
                 u32 callbackB, u32 callbackC) {
    u32 len;

    _fsysGetFilename(slot, fileHandle, callbackA, callbackB, callbackC, 2);

    if (lbl_80453FEC.activeSlot == slot) {
        if (FSYS_SLOT_FILE0(slot) == 0) {
            FSYS_SLOT_FILE0(slot) = fn_80167F28(slot->filename);
        }
        len = 0x40;
        fn_80167E98(FSYS_SLOT_FILE0(slot), slot, (len + 0x1F) & ~0x1Fu, 0, fn_8017F108);
    }
}
#endif
#pragma pop

/* 0x8017E09C | 0x13C */
#if 0
asm void fn_8017E09C(void) {
#include "src/game/fsys/fsys_file_fn_8017E09C.inc"
}
#else
void fn_8017E09C(FSYSSlot* slot, u32 fileHandle, u32 callbackA,
                 u32 callbackB, u32 callbackC) {
    u32 readFromCache;
    u32 cached;

    readFromCache = 0;
    _fsysGetFilename(slot, fileHandle, callbackA, callbackB, callbackC, 0);

    if (gFSYSManager.activeSlot == slot) {
        if (slot->reloadFlag == 1) {
            if (FSYS_SLOT_FILE0(slot) == 0) {
                FSYS_SLOT_FILE0(slot) = fn_80167F28(slot->filename);
            }
            return;
        }

        if (FSYS_SLOT_FILE0(slot) == 0) {
            FSYS_SLOT_FILE0(slot) = fn_80167F28(slot->filename);
        }

        cached = fn_8017F794(slot->fileHandle, 0, 0);
        if (cached != 0) {
            fn_80180320(slot, (void*)cached, FSYSAlign32(0x40));
            DCFlushRange(slot, FSYSAlign32(0x40));
            slot->status = 2;
            readFromCache = 1;
        }

        if (readFromCache == 0) {
            fn_80167E98(FSYS_SLOT_FILE0(slot), slot, FSYSAlign32(0x40), 0, fn_8017F108);
        }
    }
}
#endif

void fn_8017E1D8(FSYSSlot* slot, u32 fileHandle, u32 callbackA,
                 u32 callbackB, u32 callbackC)
{
    u32 cached;
    u32 size;
    u8 readFromCache;

    readFromCache = 0;
    _fsysGetFilename(slot, fileHandle, callbackA, callbackB, callbackC, 3);

    if (gFSYSManager.activeSlot != slot) {
        return;
    }
    if ((s32)slot->reloadFlag == 1) {
        if (FSYS_SLOT_FILE0(slot) == 0) {
            FSYS_SLOT_FILE0(slot) = fn_80167F28(slot->filename);
        }
        return;
    }
    if (FSYS_SLOT_FILE0(slot) == 0) {
        FSYS_SLOT_FILE0(slot) = fn_80167F28(slot->filename);
    }

    size = 0x40;
    cached = fn_8017F794(slot->fileHandle, 0, 0);
    if (cached != 0) {
        fn_80180320(slot, (void*)cached, FSYSAlign32(size));
        DCFlushRange(slot, FSYSAlign32(size));
        slot->status = 2;
        readFromCache = 1;
    }
    if (!readFromCache) {
        fn_80167E98(FSYS_SLOT_FILE0(slot), slot, FSYSAlign32(size), 0,
                    fn_8017F108);
    }
}

/* WP-0010 stubs */

/* 0x8017F25C | 0x68 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8017F25C(s32 result, FSYSSlot* slot) {
    FSYSSlot* savedSlot;
    u32 enabled;

    savedSlot = slot;
    enabled = OSDisableInterrupts();
    if (result == 1) {
        savedSlot->status = 0xA2;
    } else {
        savedSlot->status = 0x98;
    }
    OSRestoreInterrupts(enabled);
}
#pragma pop
