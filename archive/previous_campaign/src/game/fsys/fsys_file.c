/**
 * @file fsys_file.c
 * @brief FSYS file lookup, entry processing, and archive load state machine.
 *
 * Contains the core file lookup/check functions (FSYSCheckFileLoaded,
 * FSYSRequestFile), the per-entry processor (FSYSProcessEntry), the
 * multi-stage load state machine (FSYSBeginLoad), and the cache lookup
 * (FSYSCacheLookup).
 *
 * Address range: 0x8017B07C - 0x8017B1CC, 0x8017E30C - 0x8017F108,
 *                0x8017F794 - 0x8017F928
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
extern void  fn_800C8520(char* dst, const char* fmt, ...);

/* Memory read (heap-to-ptr with DMA) */
extern void* fn_800F9418(u32 size, u32 priority, u32 alignment, u32 fileID, u32 param);
extern void* fn_800F9318(u32 fileHandle, u32 fileID);

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

/* lbl_80454038 -- head of the DVD cache linked list */
extern void* gDVDCacheHead;

/* lbl_80453FDC -- LZSS decompression context */
extern FSYSDecompContext gLZSSContext;

/* lbl_80452FC8 -- LZSS sliding window */
extern u8 gLZSSWindow[];

/* FSYS stores a fixed 0x28-byte entry header followed by a runtime sub-entry trailer. */
static FSYSSubEntry* FSYSFileEntry_GetSubEntry(FSYSFileEntry* entry) {
    return (FSYSSubEntry*)((u8*)entry + 0x28);
}

/* ===================================================================
 * fn_8017B07C: FSYSCheckFileLoaded
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
s32 FSYSCheckFileLoaded(u32 fileHandle, u32 nameHash) {
    FSYSSlot* slot;
    s32 found = 0;
    u32 i;

    slot = FSYSFindSlot(fileHandle, 3);
    if (slot == NULL) {
        return found;
    }

    for (i = 0; i < slot->numEntries; i++) {
        void* archBase = slot->archiveData;
        FSYSFileEntry* entry;

        if (archBase != NULL) {
            /* Two-level indirection to find the file entry:
             * 1. stringTableOff points to an offset table
             * 2. That table points to per-entry offset arrays
             * 3. Index into the array gives the entry offset */
            u32 stringTableOff = *(u32*)((u8*)archBase + 0x18);
            u32* offTable = (u32*)((u8*)archBase + stringTableOff);
            u32  strOff   = offTable[0];
            u32* entryTable = (u32*)((u8*)archBase + strOff);
            u32  entryOff  = entryTable[i];
            entry = (FSYSFileEntry*)((u8*)archBase + entryOff);
        } else {
            entry = NULL;
        }

        if (entry != NULL) {
            FSYSSubEntry* sub = FSYSFileEntry_GetSubEntry(entry);

            if (entry->nameHash == nameHash) {
                if (sub->state == 6) {
                    found = 1;
                }
            }
        }
    }

    return found;
}

/* ===================================================================
 * fn_8017B13C: FSYSRequestFile
 *
 * Requests loading of a file from an already-loaded archive.
 * Finds the slot, stores the request ID, then calls the extended
 * load function (fn_8017E1D8) with NULL callbacks.
 *
 * @param fileHandle  DVD file handle
 * @param requestID   Resource name hash to load
 * @return            1 on success, 0 if no slot available
 * =================================================================== */
s32 FSYSRequestFile(u32 fileHandle, u32 requestID) {
    FSYSSlot* slot;

    slot = FSYSFindSlot(fileHandle, 3);
    if (slot == NULL) {
        return 0;
    }

    slot->requestID = requestID;
    fn_8017E1D8(slot, fileHandle, 0, 0, 0);

    return 1;
}

/* Note: numEntries is at offset 0x0C in the FSYSSlot struct, now a named field. */

/* ===================================================================
 * fn_8017E30C: FSYSProcessEntry
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
s32 FSYSProcessEntry(FSYSSlot* slot) {
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

    /* Try to find the data in the cache first */
    allocatedBuf = FSYSCacheLookup(slot->fileHandle,
                                    fileEntry->groupID,
                                    fileEntry->nameHash);
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

        /* Save compressed data pointer and clear sub-entry buffer */
        {
            void* compData = subEntry->buffer;
            subEntry->buffer = NULL;

            /* Copy the 16-byte LZSS header from the compressed data */
            memcpy(&gLZSSContext, compData, LZSS_HEADER_SKIP);

            /* Clear the LZSS sliding window */
            {
                u32 j;
                for (j = 0; j < LZSS_WINDOW_SIZE; j++) {
                    gLZSSWindow[j] = 0;
                }
            }

            /* Decompress: output goes to allocatedBuf location */
            FSYSDecompressLZSS(allocatedBuf, compData, gLZSSContext.decompSize);

            /* Flush D-cache for decompressed data */
            DCFlushRange(allocatedBuf, gLZSSContext.decompSize);
        }

        /* Free the temporary compressed data buffer handle */
        {
            u16 tmpHandle = fn_800E202C(compBuf);
            if (tmpHandle != 0) {
                fn_800E24B0(tmpHandle);
                fn_800E209C(tmpHandle);
            }
        }

        /* Look up decompression pool for a completion callback */
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

            if (poolEntry == NULL) {
                poolEntry = NULL; /* redundant, matches original */
            }
        }

        /* If a decompression callback exists, invoke it */
        if (poolEntry != NULL && poolEntry->callback != NULL) {
            typedef void* (*DecompCallback)(u32 fileHandle, u32 fileID, u32 compSize);
            DecompCallback cb = (DecompCallback)poolEntry->callback;
            allocatedBuf = cb(slot->fileHandle, fileEntry->nameHash,
                             fileEntry->compressedSize);
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
            allocatedBuf = (void*)fn_800F9418(allocSize,
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
            void* readResult = fn_800F9318(slot->fileHandle, fileEntry->nameHash);
            if (readResult != NULL) {
                if (isCompressed) {
                    DCFlushRange(readResult, fileEntry->compressedSize);
                } else {
                    DCFlushRange(readResult, fileEntry->decompressedSize);
                }
            }

            if (poolEntry2->callback != NULL) {
                typedef void* (*PostCallback)(u32 fh, u32 id, u32 sz);
                PostCallback cb = (PostCallback)poolEntry2->callback;
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
 * fn_8017EB6C: FSYSBeginLoad
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
void FSYSBeginLoad(FSYSSlot* slot, u32 fileHandle,
                   u32 callbackA, u32 callbackB, u32 callbackC,
                   u32 loadMode) {
    FSYSManager* mgr = &gFSYSManager;
    u32 status = slot->status;
    void* tocData;
    u32 numTocEntries;
    u32 i;
    u32* tocEntryPtr;
    char* tocName;
    char nameBuf[0x80];

    /* Handle current status */
    if (status == FSYS_STATUS_PENDING) {
        goto state_loaded;
    } else if (status >= FSYS_STATUS_PENDING) {
        if (status == FSYS_STATUS_LOADED) {
            goto state_loaded;
        }
        return; /* unknown state */
    } else if (status == FSYS_STATUS_FREE) {
        /* Fall through to initial load */
    } else {
        return; /* busy */
    }

    /* Initialize the slot for a new load */
    slot->archiveHandle = 0;
    slot->archiveData   = NULL;
    slot->archiveSize   = 0;
    slot->callbackA     = callbackA;
    slot->callbackB     = callbackB;
    slot->callbackC     = callbackC;
    slot->fileHandle    = fileHandle;
    slot->fileIndex     = 0;
    slot->loadMode      = loadMode;
    slot->reloadFlag    = 0;
    slot->padding100    = 0;

    /* Search the TOC for the matching entry */
    tocData = gFSYSTocData;
    numTocEntries = *(u32*)((u8*)tocData + 0x08);
    tocEntryPtr = (u32*)((u8*)tocData + *(u32*)((u8*)tocData + 0x10));
    tocName = NULL;

    for (i = 0; i < numTocEntries; i++) {
        if (tocEntryPtr[0] == fileHandle) {
            /* Found: resolve the name string */
            u32 nameOff = tocEntryPtr[1];
            tocName = (char*)((u8*)tocData + nameOff);
            break;
        }
        tocEntryPtr = (u32*)((u8*)tocEntryPtr + 8);
    }

    /* Format the filename as "%s.fsys" */
    fn_800C8520(nameBuf, "%s.fsys", tocName);
    strcpy(slot->filename, nameBuf);

    /* Check if another slot is already actively loading */
    if (mgr->activeSlot != NULL) {
        if (slot != mgr->activeSlot) {
            /* Queue this load -- set status to PENDING */
            slot->status = FSYS_STATUS_PENDING;
            return;
        }
    }

    /* Mark this slot as the active loader */
    mgr->activeSlot  = slot;
    mgr->currentSlot = slot;

    /* Apply status based on load mode */
    if (loadMode >= 4) {
        if (loadMode == 7) {
            slot->status = FSYS_STATUS_LOADING;
        } else {
            return;
        }
    } else if (loadMode == 1) {
        slot->status = FSYS_STATUS_READING;
    } else if (loadMode == 0) {
        slot->status = FSYS_STATUS_LOADING;
    } else {
        return;
    }
    return;

state_loaded:
    /* Re-entry: archive is loaded, process a specific file */
    if (loadMode != 3) {
        goto state_check_mode;
    }

    /* Mode 3: resolve file within loaded archive */
    slot->archiveHandle = 0;
    slot->archiveSize   = 0;
    slot->callbackA     = callbackA;
    slot->callbackB     = callbackB;
    slot->callbackC     = callbackC;
    slot->fileHandle    = fileHandle;
    slot->fileIndex     = 0;
    slot->loadMode      = loadMode;
    slot->reloadFlag    = 1;

    /* Search TOC again for the entry */
    tocData = gFSYSTocData;
    numTocEntries = *(u32*)((u8*)tocData + 0x08);
    tocEntryPtr = (u32*)((u8*)tocData + *(u32*)((u8*)tocData + 0x10));
    tocName = NULL;

    for (i = 0; i < numTocEntries; i++) {
        if (tocEntryPtr[0] == fileHandle) {
            u32 nameOff = tocEntryPtr[1];
            tocName = (char*)((u8*)tocData + nameOff);
            break;
        }
        tocEntryPtr = (u32*)((u8*)tocEntryPtr + 8);
    }

    /* Format filename and set up for re-load */
    fn_800C8520(nameBuf, "%s.fsys", tocName);
    strcpy(slot->filename, nameBuf);

    /* Check if loading is blocked */
    if (mgr->activeSlot != NULL) {
        slot->status = FSYS_STATUS_PENDING;
        return;
    }

    mgr->activeSlot  = slot;
    mgr->currentSlot = slot;

state_check_mode:
    if (loadMode >= 4) {
        if (loadMode == 7) {
            slot->status = FSYS_STATUS_ERROR;
        } else {
            return;
        }
    } else if (loadMode == 1) {
        slot->status = FSYS_STATUS_ERROR;
    } else {
        slot->status = FSYS_STATUS_ERROR;
    }
    return;
}

/* ===================================================================
 * fn_8017F794: FSYSCacheLookup
 *
 * Searches the DVD cache linked list (rooted at lbl_80454038) for a
 * previously loaded file matching the given (fileHandle, groupID,
 * nameHash) triple.
 *
 * The cache is a singly-linked list of nodes, each containing:
 *   +0x00: data pointer (returned on match)
 *   +0x04: (padding)
 *   +0x08: next pointer
 *   +0x10: fileHandle
 *   +0x14: groupID
 *   +0x18: nameHash
 *
 * @param fileHandle  DVD file handle
 * @param groupID     Archive group ID
 * @param nameHash    File resource ID
 * @return            Cached data pointer, or NULL if not in cache
 * =================================================================== */
void* FSYSCacheLookup(u32 fileHandle, u32 groupID, u32 nameHash) {
    typedef struct CacheNode {
        void*  data;         /* 0x00 */
        u32    pad04;        /* 0x04 */
        struct CacheNode* next; /* 0x08 */
        u32    pad0C;        /* 0x0C */
        u32    fileHandle;   /* 0x10 */
        u32    groupID;      /* 0x14 */
        u32    nameHash;     /* 0x18 */
        u32    refCount;     /* 0x1C */
    } CacheNode;

    CacheNode* node = (CacheNode*)gDVDCacheHead;

    while (node != NULL) {
        if (node->fileHandle == fileHandle &&
            node->groupID == groupID &&
            node->nameHash == nameHash) {
            return node->data;
        }
        node = node->next;
    }

    return NULL;
}

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
extern void fn_80167E34(void);
void fn_8017B1AC(void) {
    fn_80167E34();
}
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 1
asm void fn_8017B2CC(void) {
#include "src/game/gs_scene_fn_8017B2CC.inc"
}
#else
void fn_8017B2CC(void) { /* TODO */ }
#endif
#pragma pop

/* ===================================================================
 * WP-0009 stubs (address range 0x8017B448 - 0x8017E1D8)
 * =================================================================== */

/* 0x8017B448 | 0x74 */
extern u32 lbl_8047B1B4;
extern u8 lbl_80453FEC[];
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
#define COUNT_8017B448 (*(volatile u32*)lbl_80453FEC)
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

/* 0x8017B4BC | 0xE8 */
extern u32 lbl_8047B1B4;
#if 1
asm void fn_8017B4BC(void) {
#include "src/game/fsys/fsys_file_fn_8017B4BC.inc"
}
#else
void fn_8017B4BC(void) { /* TODO: match -- 232 bytes at 0x8017B4BC */ }
#endif

/* 0x8017B5A4 | 0x1C */
u32 fn_8017B5A4(u32 val) {
    return (val >> 9) & 0x3F;
}

/* 0x8017B5C0 | 0xF8 */
extern void fn_8017BD34(void);
extern u32 fn_8017F794();
extern void fn_8017F728(void);
extern void fn_8017A814(void);
extern void fn_80180584(void);
#if 1
asm void fn_8017B5C0(void) {
#include "src/game/fsys/fsys_file_fn_8017B5C0.inc"
}
#else
void fn_8017B5C0(void) { /* TODO: match -- 248 bytes at 0x8017B5C0 */ }
#endif

/* 0x8017B6B8 | 0x4C8 */
extern void fn_8017F25C();
extern void fn_8017F108(void);
extern void fn_80167E98(void);
extern u32 lbl_80478C48;
extern u8 lbl_8036C2A0[];
#if 1
asm void fn_8017B6B8(void) {
#include "src/game/fsys/fsys_file_fn_8017B6B8.inc"
}
#else
void fn_8017B6B8(void) { /* TODO: match -- 1224 bytes at 0x8017B6B8 */ }
#endif

/* 0x8017BD34 | 0x2B4 */
extern void fn_8017A95C(void);
extern u32 lbl_80478C48;
#if 1
asm void fn_8017BD34(void) {
#include "src/game/fsys/fsys_file_fn_8017BD34.inc"
}
#else
void fn_8017BD34(void) { /* TODO: match -- 692 bytes at 0x8017BD34 */ }
#endif

/* 0x8017C008 | 0x6C */
extern void fn_80180C78();
#if 1
asm void fn_8017C008(void) {
#include "src/game/fsys/fsys_file_fn_8017C008.inc"
}
#else
void fn_8017C008(void) { /* TODO: match -- 108 bytes at 0x8017C008 */ }
#endif

/* 0x8017C074 | 0x164 */
extern u32 lbl_80478C48;
#if 1
asm void fn_8017C074(void) {
#include "src/game/fsys/fsys_file_fn_8017C074.inc"
}
#else
void fn_8017C074(void) { /* TODO: match -- 356 bytes at 0x8017C074 */ }
#endif

/* 0x8017C1D8 | 0x1BC */
extern u32 lbl_80478C48;
#if 1
asm void fn_8017C1D8(void) {
#include "src/game/fsys/fsys_file_fn_8017C1D8.inc"
}
#else
void fn_8017C1D8(void) { /* TODO: match -- 444 bytes at 0x8017C1D8 */ }
#endif

/* 0x8017C39C | 0x78 */
#if 1
asm void fn_8017C39C(void) {
#include "src/game/fsys/fsys_file_fn_8017C39C.inc"
}
#else
void fn_8017C39C(void) { /* TODO: match -- 120 bytes at 0x8017C39C */ }
#endif

/* 0x8017C414 | 0x154 */
extern u32 fn_8017A624(void*);
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
        mgr = (FSYSManager*)lbl_80453FEC;
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
#if 1
asm void fn_8017C6E0(void) {
#include "src/game/fsys/fsys_file_fn_8017C6E0.inc"
}
#else
void fn_8017C6E0(void) { /* TODO: match -- 428 bytes at 0x8017C6E0 */ }
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
extern void fn_80179FA4(void);
#if 1
asm void fn_8017C8FC(void) {
#include "src/game/fsys/fsys_file_fn_8017C8FC.inc"
}
#else
void fn_8017C8FC(void) { /* TODO: match -- 1408 bytes at 0x8017C8FC */ }
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
extern FSYSFileHandle* fn_8017D68C(FSYSSlot*);
extern void fn_8017FA5C(void);
extern void fn_8017F800(void);
extern void fn_80180450(void);
extern u32 lbl_8047B1B8;
extern u32 lbl_8047B1BC;
#if 1
asm void fn_8017CED8(void) {
#include "src/game/fsys/fsys_file_fn_8017CED8.inc"
}
#else
void fn_8017CED8(void) { /* TODO: match -- 1224 bytes at 0x8017CED8 */ }
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
#if 0
asm void fn_8017D68C(void) {
#include "src/game/fsys/fsys_file_fn_8017D68C.inc"
}
#else
#pragma optimization_level 0
FSYSFileHandle* fn_8017D68C(FSYSSlot* slot) {
    FSYSFileHandle* table;
    FSYSFileHandle* src;
    FSYSFileHandle* dst;
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
                src = &table[i + 1];
                dst = &table[i];
                dst->handleID = src->handleID;
                dst->userData = src->userData;
            }
            i++;
        }
        table[i].handleID = saved.handleID;
        table[i].userData = saved.userData;
        table = &table[i];
    } else {
        table = (FSYSFileHandle*)lbl_8047B1B8;
        if ((s32)lbl_8047B1BC == 0x64) {
            i = 0;
            while (i < 0x63) {
                src = &table[i + 1];
                dst = &table[i];
                dst->handleID = src->handleID;
                dst->userData = src->userData;
                i++;
            }
            lbl_8047B1BC = lbl_8047B1BC - 1;
        }
        i = lbl_8047B1BC;
        lbl_8047B1BC = i + 1;
        table = &((FSYSFileHandle*)lbl_8047B1B8)[i];
        table->handleID = handleID;
        table->userData = 0;
    }

    return table;
}
#endif

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
    for (i = 0; i < *(u32*)lbl_80453FEC; i++) {
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
extern void fn_8017D960();
#if 0
asm void fn_8017D8F8(void) {
#include "src/game/fsys/fsys_file_fn_8017D8F8.inc"
}
#else
#pragma optimization_level 4
void fn_8017D8F8(FSYSSlot* slot) {
    fn_8017D960();
    slot->status = FSYS_STATUS_FREE;
}
#endif

/* 0x8017D92C | 0x34 */
extern void fn_8017D960();
#if 0
asm void fn_8017D92C(void) {
#include "src/game/fsys/fsys_file_fn_8017D92C.inc"
}
#else
#pragma optimization_level 4
void fn_8017D92C(FSYSSlot* slot) {
    fn_8017D960();
    slot->status = FSYS_STATUS_FREE;
}
#endif

/* 0x8017D960 | 0x158 */
#if 1
asm void fn_8017D960(void) {
#include "src/game/fsys/fsys_file_fn_8017D960.inc"
}
#else
void fn_8017D960(void) { /* TODO: match -- 344 bytes at 0x8017D960 */ }
#endif

/* 0x8017DAB8 | 0xBC */
#if 1
asm void fn_8017DAB8(void) {
#include "src/game/fsys/fsys_file_fn_8017DAB8.inc"
}
#else
void fn_8017DAB8(void) { /* TODO: match -- 188 bytes at 0x8017DAB8 */ }
#endif

/* 0x8017DB74 | 0x330 */
extern u32 lbl_8047B1B8;
extern u32 lbl_8047B1BC;
#if 1
asm void fn_8017DB74(void) {
#include "src/game/fsys/fsys_file_fn_8017DB74.inc"
}
#else
void fn_8017DB74(void) { /* TODO: match -- 816 bytes at 0x8017DB74 */ }
#endif

/* 0x8017DEA4 | 0xA8 */
extern void fn_8017EB6C(void);
extern void fn_80167DD8(void);
#if 1
asm void fn_8017DEA4(void) {
#include "src/game/fsys/fsys_file_fn_8017DEA4.inc"
}
#else
void fn_8017DEA4(void) { /* TODO: match -- 168 bytes at 0x8017DEA4 */ }
#endif

/* 0x8017DF4C | 0xA8 */
#if 1
asm void fn_8017DF4C(void) {
#include "src/game/fsys/fsys_file_fn_8017DF4C.inc"
}
#else
void fn_8017DF4C(void) { /* TODO: match -- 168 bytes at 0x8017DF4C */ }
#endif

/* 0x8017DFF4 | 0xA8 */
#if 1
asm void fn_8017DFF4(void) {
#include "src/game/fsys/fsys_file_fn_8017DFF4.inc"
}
#else
void fn_8017DFF4(void) { /* TODO: match -- 168 bytes at 0x8017DFF4 */ }
#endif

/* 0x8017E09C | 0x13C */
#if 1
asm void fn_8017E09C(void) {
#include "src/game/fsys/fsys_file_fn_8017E09C.inc"
}
#else
void fn_8017E09C(void) { /* TODO: match -- 316 bytes at 0x8017E09C */ }
#endif

/* WP-0010 stubs */

/* 0x8017F25C | 0x68 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8017F25C(s32 param1, u8* param2) {
    u32 enabled = OSDisableInterrupts();
    if (param1 == 1) {
        *(u32*)(param2 + 0x48) = 0xA2;
    } else {
        *(u32*)(param2 + 0x48) = 0x98;
    }
    OSRestoreInterrupts(enabled);
}
#pragma pop
