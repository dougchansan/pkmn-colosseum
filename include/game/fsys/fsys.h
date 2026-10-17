#ifndef GAME_FSYS_H
#define GAME_FSYS_H

#include "dolphin/types.h"

/*
 * FSYS Archive Format -- Pokemon Colosseum's custom archive system.
 *
 * FSYS archives contain game assets (models, textures, scripts, etc.)
 * packed with optional LZSS compression. The format has been partially
 * documented by StarsMmd and the Colosseum hacking community.
 *
 * On-disc archive header: see FSYSArchiveHeader below (entry count at
 * 0x0C, flags at 0x10, table-offset pair at 0x18).
 *
 * File entry (0x28 bytes each, accessed via TOC indirection):
 *   0x00: u32 nameHash        -- filename hash or resource ID
 *   0x04: (padding)
 *   0x08: u32 compressedSize  -- compressed data size
 *   0x0C: u32 flags           -- bit 0 = LZSS compressed
 *   0x10: (reserved)
 *   0x14: u32 decompressedSize
 *   0x18: (reserved)
 *   0x1C: u32 dataOffset      -- offset of this file's data
 *   0x20: u32 groupID         -- owner archive / resource group
 *   0x24-0x27: (reserved)
 *   0x28: subEntry[...]       -- begins here; loaded into FSYSFileEntry
 *
 * Decompression uses a standard LZSS variant with a 4078-byte (0xFEE)
 * sliding window. Flag bytes control 8 chunks: bit=1 means literal,
 * bit=0 means (offset, length) back-reference.
 *
 * Address range of FSYS code in DOL: 0x8017A5FC - 0x8018FE30+
 *
 * Key global data:
 *   lbl_80453FEC (0x2C bytes) -- FSYSManager: slot count, active load, etc.
 *   lbl_8047B1B4             -- pointer to FSYSSlot array
 *   lbl_8047B1B8             -- pointer to file handle table (100 entries)
 *   lbl_8047B1B0             -- pointer to loaded TOC data
 *   lbl_80452FC8 (0x1014)   -- LZSS sliding window buffer
 *   lbl_80453FDC (0x10)     -- LZSS decompression context
 */

/* ===================================================================
 * Constants
 * =================================================================== */

/* FSYS magic number: 'F','S','Y','S' */
#define FSYS_MAGIC 0x46535953u

/* LZSS sliding window size (standard 4096 minus 18) */
#define LZSS_WINDOW_SIZE   0xFEE
#define LZSS_BUFFER_SIZE   0x1000
#define LZSS_HEADER_SKIP   0x10

/* Maximum number of concurrent file handles */
#define FSYS_MAX_HANDLES    100

/* FSYSSlot size in bytes */
#define FSYS_SLOT_SIZE      0x140

/* Archive load request queue entry size */
#define FSYS_QUEUE_ENTRY_SIZE 0x44

/* DVDFileInfo pool entry size */
#define FSYS_DVDINFO_SIZE   0x20

/* DVDFileInfo pool count */
#define FSYS_DVDINFO_COUNT  0x400

/* Decomp context pool entry size */
#define FSYS_DECOMP_ENTRY_SIZE 0x10

/* Decomp context pool count */
#define FSYS_DECOMP_POOL_COUNT 0x1000

/* Load status values stored in FSYSSlot.status (offset 0x48) */
#define FSYS_STATUS_FREE       0x000  /* slot unused */
#define FSYS_STATUS_LOADING    0x001  /* load in progress (initial) */
#define FSYS_STATUS_READING    0x0C8  /* DVD read in progress (200) */
#define FSYS_STATUS_PENDING    0x1F4  /* queued / waiting (500) */
#define FSYS_STATUS_LOADED     0x3E8  /* load complete (1000) */

/* Error code used when allocation fails */
#define FSYS_STATUS_ERROR      0x064  /* 100 */

/* ===================================================================
 * Structures
 * =================================================================== */

/*
 * FSYSArchiveHeader -- start of a loaded FSYS archive (slot->archiveData).
 * tableOffset points at a pair of archive-relative offsets: [0] is the
 * per-entry offset list, [1] the name table.
 */
typedef struct FSYSArchiveHeader {
    /* 0x00 */ u32 magic;       /* 'FSYS' */
    /* 0x04 */ u32 version;
    /* 0x08 */ u32 groupID;     /* archive key passed to fn_8017F794 */
    /* 0x0C */ u32 numEntries;
    /* 0x10 */ u32 flags;       /* bit 0: entries are external files */
    /* 0x14 */ u32 unk14;
    /* 0x18 */ u32 tableOffset;
    /* 0x1C */ u32 dataOffset;
    /* 0x20 */ u32 fileSize;
} FSYSArchiveHeader;

/*
 * FSYSFileEntry -- describes one file inside an FSYS archive.
 * Derived from the on-disc entry format; entries are accessed via
 * two levels of indirection through the archive's TOC.
 *
 * Located at archive_base + stringTableOff -> indirection -> entry
 */
typedef struct FSYSFileEntry {
    /* 0x00 */ u32 nameHash;         /* filename hash / resource identifier */
    /* 0x04 */ u32 padding04;
    /* 0x08 */ u32 compressedSize;   /* compressed data size in bytes */
    /* 0x0C */ u32 flags;            /* bit 0: LZSS compressed */
    /* 0x10 */ u32 reserved10;
    /* 0x14 */ u32 decompressedSize; /* decompressed data size in bytes */
    /* 0x18 */ u32 reserved18;
    /* 0x1C */ u32 dataOffset;       /* offset within archive to file data */
    /* 0x20 */ u32 groupID;          /* resource group / archive file ID */
    /* 0x24 */ u32 reserved24;
    /* 0x28 */ u8  subEntry[0x00];   /* variable-size sub-entry data follows */
} FSYSFileEntry;

/*
 * FSYSSubEntry -- sub-entry within a file entry (offset 0x28 from entry start).
 * Contains the resolved buffer pointer and state.
 */
typedef struct FSYSSubEntry {
    /* 0x00 */ u32  state;           /* sub-entry state / completion flag */
    /* 0x04 */ void* buffer;         /* pointer to decompressed data buffer */
    /* 0x08 */ u32  ready;           /* nonzero when data is ready */
} FSYSSubEntry;

/*
 * FSYSSlot -- per-archive load slot.
 * The game pre-allocates an array of these (count from FSYSManager.maxSlots).
 * Each slot is 0x140 bytes.  Only the fields we have identified so far are
 * listed; the rest are reserved / padding.
 */
typedef struct FSYSSlot {
    /* 0x000 */ u32  field_00;          /* 0x00 */
    /* 0x004 */ u32  field_04;          /* 0x04 */
    /* 0x008 */ u32  field_08;          /* 0x08 */
    /* 0x00C */ u32  numEntries;        /* 0x0C: file entry count for this archive */
    /* 0x010 */ u32  field_10;          /* 0x10 */
    /* 0x014 */ u32  field_14;          /* 0x14 */
    /* 0x018 */ u32  field_18;          /* 0x18: string table related offset */
    /* 0x01C */ u32  field_1C;          /* 0x1C */
    /* 0x020 */ u32  totalDecompSize;   /* sum of all decompressed file sizes */
    /* 0x024 */ u8   padding024[0x1C];
    /* 0x040 */ FSYSArchiveHeader* archiveData; /* loaded archive raw data */
    /* 0x044 */ u32  archiveHandle;     /* DVD / stream handle */
    /* 0x048 */ s32  status;            /* one of FSYS_STATUS_* values */
    /* 0x04C */ s32  loadMode;          /* load mode / priority class */
    /* 0x050 */ s32  reloadFlag;        /* if nonzero, reload requested */
    /* 0x054 */ u32  padding054;
    /* 0x058 */ u32  padding058;
    /* 0x05C */ s32  padding05C;
    /* 0x060 */ s32  refCount;          /* reference count */
    /* 0x064 */ u32  padding064;
    /* 0x068 */ u32  fileInfo0;
    /* 0x06C */ void* tocBuffer;        /* internal TOC working buffer */
    /* 0x070 */ char filename[0x80];    /* archive filename (filled by sprintf) */
    /* 0x0F0 */ u8   padding0F0[0x04];
    /* 0x0F4 */ u32  entryIndex;        /* archive entry being loaded */
    /* 0x0F8 */ u32  fileHandle;        /* DVD file handle / resource ID */
    /* 0x0FC */ FSYSSubEntry* currentSub; /* runtime trailer of that entry (entry + 0x28) */
    /* 0x100 */ u32  padding100;
    /* 0x104 */ u32  requestID;         /* requested resource name hash */
    /* 0x108 */ u8   padding108[0x08];
    /* 0x110 */ u32  dmaChunkSize;      /* size of current chunked DVD transfer */
    /* 0x114 */ s32  dmaBytesRemaining; /* bytes left in the chunked transfer */
    /* 0x118 */ u32  dmaSrcOffset;      /* accumulating DVD source offset */
    /* 0x11C */ u32  dmaDstOffset;      /* accumulating destination offset */
    /* 0x120 */ u8   padding120[0x04];
    /* 0x124 */ void* dmaAsyncRequest;  /* handle polled via fn_801808B4 */
    /* 0x128 */ void* dmaCopyDst;       /* destination pointer for fn_801807A8 */
    /* 0x12C */ u8   padding12C[0x08];
    /* 0x134 */ u32  callbackA;         /* completion callback A */
    /* 0x138 */ u32  callbackB;         /* completion callback B */
    /* 0x13C */ u32  callbackC;         /* completion callback C */
} FSYSSlot; /* size: 0x140 */

/*
 * FSYSManager -- singleton that manages the FSYS subsystem.
 * Stored at lbl_80453FEC, size 0x2C.
 */
typedef void (*FSYSErrorCallback)(s32 driveStatus, u32 arg0, u32 arg1);

typedef struct FSYSManager {
    /* 0x00 */ u32  maxSlots;          /* number of pre-allocated FSYSSlots */
    /* 0x04 */ u32  field_04;
    /* 0x08 */ u32  field_08;
    /* 0x0C */ s32  driveStatus;       /* last GSdvdGetDriveStatus (DVDGetDriveStatus) result */
    /* 0x10 */ FSYSErrorCallback errorCallback; /* called on fatal/cover/retry drive states */
    /* 0x14 */ u32  errorArg0;
    /* 0x18 */ u32  errorArg1;
    /* 0x1C */ FSYSSlot* activeSlot;   /* currently loading slot */
    /* 0x20 */ FSYSSlot* currentSlot;  /* current slot being processed */
    /* 0x24 */ u32  field_24;
    /* 0x28 */ u32  field_28;
} FSYSManager; /* size: 0x2C */

/*
 * FSYSFileHandle -- entry in the file handle lookup table.
 * Array of 100 entries at lbl_8047B1B8, 8 bytes each.
 */
typedef struct FSYSFileHandle {
    /* 0x00 */ s32 handleID;   /* -1 if unused */
    /* 0x04 */ u32 userData;   /* associated data pointer or zero */
} FSYSFileHandle; /* size: 0x08 */

/*
 * FSYSDecompContext -- LZSS decompression header context.
 * Stored at lbl_80453FDC, size 0x10.
 */
typedef struct FSYSDecompContext {
    /* 0x00 */ u32  field_00;
    /* 0x04 */ u32  decompSize;     /* total decompressed size */
    /* 0x08 */ u32  compSize;       /* total compressed size */
    /* 0x0C */ u32  field_0C;
} FSYSDecompContext; /* size: 0x10 */

/*
 * DVDPoolEntry -- pre-allocated DVD info structure.
 * Array of 0x400 entries, each 0x20 bytes, at lbl_80454018+0x20.
 */
typedef struct DVDPoolEntry {
    /* 0x00 */ u32  field_00;
    /* 0x04 */ void* buffer;    /* data pointer */
    /* 0x08 */ void* next;      /* next in free/used list */
    /* 0x0C */ void* callback;  /* read completion callback */
    /* 0x10 */ u32  field_10;
    /* 0x14 */ u32  field_14;
    /* 0x18 */ u32  field_18;
    /* 0x1C */ u32  field_1C;
} DVDPoolEntry; /* size: 0x20 */

/*
 * DecompPoolEntry -- decompression pool entry.
 * Array of 0x1000 entries at lbl_80455070, each 0x10 bytes.
 */
typedef struct DecompPoolEntry {
    /* 0x00 */ u32  field_00;
    /* 0x04 */ u32  fileID;     /* matches archive groupID */
    /* 0x08 */ void* callback;  /* decompression complete callback */
    /* 0x0C */ void* data;      /* decompressed data pointer */
} DecompPoolEntry; /* size: 0x10 */

/* ===================================================================
 * Global data (defined in .bss / .sbss, declared extern here)
 * =================================================================== */

/* FSYSManager singleton (lbl_80453FEC) */
extern FSYSManager gFSYSManager;

/* Pointer to FSYSSlot array (lbl_8047B1B4) */
extern FSYSSlot* gFSYSSlots;

/* Pointer to file handle table (lbl_8047B1B8) */
extern FSYSFileHandle* gFSYSHandleTable;

/* Number of active file handles (lbl_8047B1BC) */
extern u32 gFSYSHandleCount;

/* Pointer to loaded TOC data (lbl_8047B1B0) */
extern void* gFSYSTocData;

/* DVD buffer pool pointers (lbl_8047B1C0, 2 entries) */
extern void* gFSYSDVDBuffers[2];

/* LZSS sliding window buffer (lbl_80452FC8, 0x1014 bytes) */
extern u8 gLZSSWindow[LZSS_BUFFER_SIZE + 0x14];

/* LZSS decompression context (lbl_80453FDC) */
extern FSYSDecompContext gLZSSContext;

/* ===================================================================
 * Function declarations
 * =================================================================== */

/* --- fsys_load.c --- */

/*
 * _fsysInitTOC: FSYSInit
 * Master initialization of the FSYS subsystem.
 * Allocates slot array, file handle table, DVD buffer pools, loads the
 * "gsfsys.toc" table-of-contents, and starts the load manager thread.
 *
 * @param numSlots  Number of concurrent load slots to allocate
 * @param param2    Unknown parameter (stored at manager+0x10)
 * @param param3    Unknown parameter (stored at manager+0x14)
 * @param param4    Unknown parameter (stored at manager+0x18)
 * @return          Always 1 on success
 */
s32 FSYSInit(u32 numSlots, u32 param2, u32 param3, u32 param4);

/*
 * fsysFindSlotByHandle: FSYSFindSlot
 * Searches the slot array for a slot matching the given file handle.
 * If no matching active slot is found, returns the first free slot.
 *
 * @param fileHandle  The file handle / resource ID to search for
 * @param mode        Search mode (affects reference counting behavior)
 * @return            Pointer to matching FSYSSlot, or NULL
 */
FSYSSlot* FSYSFindSlot(u32 fileHandle, u32 mode);

/*
 * fn_8017AF6C: FSYSLoadArchive
 * Begins loading an FSYS archive by file handle and resource ID.
 *
 * @param fileHandle  DVD file handle
 * @param requestID   Resource ID / name hash to load
 * @return            1 on success, 0 on failure
 */
s32 FSYSLoadArchive(u32 fileHandle, u32 requestID);

/*
 * fn_8017B000: FSYSLoadArchiveEx
 * Extended archive load with callbacks.
 *
 * @param fileHandle  DVD file handle
 * @param requestID   Resource ID
 * @param callbackA   Completion callback A
 * @param callbackB   Completion callback B
 * @param callbackC   Completion callback C
 * @return            1 on success, 0 on failure
 */
s32 FSYSLoadArchiveEx(u32 fileHandle, u32 requestID,
                      u32 callbackA, u32 callbackB, u32 callbackC);

/*
 * fsysCacheInit: FSYSInitLoadManager
 * Initializes the load manager: allocates DVD pool, decompression pool,
 * and request queue structures.
 *
 * @param maxRequests  Maximum concurrent DVD read requests
 * @param alignment    Memory alignment for buffers
 * @param poolSize     Decompression pool size
 */
void FSYSInitLoadManager(u32 maxRequests, u32 alignment, u32 poolSize);

/* --- fsys_file.c ---
 *
 * 2026-07-02 reconciliation: removed orphan prototypes (none of these
 * names are present in symbols.txt, none paired in objdiff):
 *   - FSYSCheckFileLoaded, FSYSRequestFile, FSYSProcessEntry: already
 *     dead - src/game/fsys/fsys_file.c no longer defines anything under
 *     these names (a prior pass already renamed the real bodies to
 *     fsysIsEntryLoaded, fsysRequestEntry, fsysLoadEntrySync respectively; these
 *     prototypes were stale leftovers with no matching definition).
 *   - FSYSBeginLoad: its definition in fsys_file.c was renamed to
 *     _fsysGetFilename (real name, confirmed by matching address/size
 *     against the unit's objdiff report AND ground-truth disassembly
 *     of that unit's real wrapper functions, which literally call
 *     "bl _fsysGetFilename"). See fsys_file.c for the full evidence.
 *   - FSYSCacheLookup: its definition in fsys_file.c was deleted
 *     outright - its claimed address belonged to a different unit
 *     entirely (game/gs_range_8017F2C4.c per splits.txt), and the rest
 *     of fsys_file.c already calls that real, not-yet-decompiled
 *     function correctly as `extern u32 fn_8017F794()`.
 */

/*
 * _fsysGetFilename (0x8017EB6C, size 0x59C)
 * Initiates or continues loading an FSYS archive. Implements a state
 * machine that transitions through PENDING -> LOADING -> LOADED.
 *
 * @param slot       Pointer to the FSYSSlot
 * @param fileHandle DVD file handle
 * @param callbackA  Callback A
 * @param callbackB  Callback B
 * @param callbackC  Callback C
 * @param loadMode   Load mode / priority
 * @return           (void)
 */
void _fsysGetFilename(FSYSSlot* slot, u32 fileHandle,
                   u32 callbackA, u32 callbackB, u32 callbackC, s32 loadMode);

/* --- fsys_decomp.c --- */

/*
 * fn_8017F2C4: FSYSDecompressLZSS
 * Decompresses LZSS-compressed data from an FSYS archive.
 *
 * Uses a 4078-byte sliding window (gLZSSWindow). The compressed
 * stream starts at offset 0x10 (skipping a 16-byte header). Each
 * flag byte controls 8 chunks via its low bits: bit=1 means read a
 * literal byte, bit=0 means read a 2-byte (offset, length) pair
 * for a back-reference copy from the sliding window.
 *
 * @param dst   Destination buffer for decompressed data
 * @param src   Source compressed data (raw archive data)
 * @param size  Compressed data size (from decompression context)
 */
void FSYSDecompressLZSS(void* dst, const void* src, u32 size);

#endif /* GAME_FSYS_H */
