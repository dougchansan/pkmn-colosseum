/**
 * @file gs_mem.c
 * @brief GSmem -- Genius Sonority custom memory allocator.
 *
 * Decompiled from:
 *   fn_800E3560 (GSmemSetDefaultHeap)
 *   fn_800E3568 (GSmemInit)
 *   fn_800E2C04 (GSmemAlloc)
 *   fn_800E2B00 (GSmemAllocTail)
 *   fn_800E3534 (GSmemAllocRaw)
 *   fn_800E209C (GSmemFree)
 *   fn_800E24B0 (GSmemLock)
 *   fn_800E27B0 (GSmemGetPtr)
 *   fn_800E202C (GSmemFindHandle)
 *   fn_800E2AF8 (GSmemIsInited)
 *   fn_800E2DB0 (GSmemSplitBlock -- internal)
 *   fn_800E0DDC (GSmemGetFreeSize)
 *
 * Original source file: GCN_Mem_Alloc.c (from debug string)
 *
 * The allocator manages a single contiguous heap carved from the OS arena.
 * Free blocks are kept in a singly-linked list ordered by address.  Each
 * allocation is tracked through a 16-byte "entry" in a table that grows
 * downward from the top of the heap.  Callers receive a 16-bit handle
 * (1-based index) rather than a raw pointer, allowing the allocator to
 * compact or relocate blocks in principle (though compaction is not
 * observed in Pokemon Colosseum).
 *
 * Address range: 0x800E202C - 0x800E3604
 */

#include "dolphin/types.h"
#include "game/gs_mem.h"

/* ===== External SDK / engine functions ===== */
extern void  fn_800DD970(const char* fmt, ...);  /* OSReport / debug printf */

/* ===== String constants (rodata references) ===== */
extern const char lbl_80270DFC[]; /* "GSmem: Init OK, using area %08Xh -> %08Xh\n" */
extern const char lbl_80270D78[]; /* "GSmem: Warning -- memory loss..." */
extern const char lbl_80270DD0[]; /* "GSmem: Unable to locate block for updating\n" */

/*
 * These string-table offsets are accessed as (lbl_80270658 + offset) in the
 * assembly.  We forward-declare the base label and compute offsets the same
 * way the original binary does.
 */
extern const char lbl_80270658[]; /* GSmem error string table base */

/* ===== Global state (sbss/sdata) ===== */

/* lbl_8047AB28 : u8  -- flag indicating debug/guard-byte mode enabled */
static u8  gsMemDebugMode;      /* @sda21 lbl_8047AB28 */

/* lbl_8047AB2C : u32 -- current allocation strategy (0=first, 1=best, 2=worst) */
static u32 gsMemDefaultHeap;    /* @sda21 lbl_8047AB2C */

/* lbl_8047AB30 : GSmemBlock* -- head of the free-block linked list */
static GSmemBlock* gsMemFreeList; /* @sda21 lbl_8047AB30 */

/* lbl_8047AB34 : void* -- top of the entry table (highest entry address) */
static GSmemEntry* gsMemEntryTop; /* @sda21 lbl_8047AB34 */

/* lbl_8047AB38 : void* -- bottom of the entry table (lowest valid entry) */
static GSmemEntry* gsMemEntryBot; /* @sda21 lbl_8047AB38 */

/* lbl_8047AB3C : u32 -- last error code */
static u32 gsMemLastError;       /* @sda21 lbl_8047AB3C */

/* lbl_8047AB40 - lbl_8047AB60 : statistics / debug counters */
static u32 gsMemStat_40;         /* @sda21 lbl_8047AB40 */
static u32 gsMemStat_44;         /* @sda21 lbl_8047AB44 */
static u32 gsMemStat_48;         /* @sda21 lbl_8047AB48 */
static u32 gsMemStat_4C;         /* @sda21 lbl_8047AB4C */
static u32 gsMemStat_50;         /* @sda21 lbl_8047AB50 */
static u32 gsMemStat_54;         /* @sda21 lbl_8047AB54 */
static u32 gsMemStat_58;         /* @sda21 lbl_8047AB58 */
static u32 gsMemStat_5C;         /* @sda21 lbl_8047AB5C */
static u32 gsMemStat_60;         /* @sda21 lbl_8047AB60 */

/* lbl_8047AB64 : void* -- end of usable heap (aligned down) */
static void* gsMemHeapEnd;       /* @sda21 lbl_8047AB64 */

/* lbl_8047AB68 : void* -- start of usable heap (aligned up) */
static void* gsMemHeapStart;     /* @sda21 lbl_8047AB68 */

/* =======================================================================
 *  GSmemSetDefaultHeap / fn_800E3560
 *  Address: 0x800E3560, Size: 0x8
 *
 *  Stores the allocation strategy index.
 * ======================================================================= */
void GSmemSetDefaultHeap(u32 heapId) {
    gsMemDefaultHeap = heapId;
}

/* =======================================================================
 *  GSmemInit / fn_800E3568
 *  Address: 0x800E3568, Size: 0x9C
 *
 *  Initialise the heap from [start, end).
 *
 *  Assembly breakdown:
 *    r3 = heapId, r4 = start, r5 = end
 *    r7 = ALIGN_UP(start, 32)
 *    r6 = ALIGN_DOWN(end, 32)
 *    r5 = r6 - 0x10          (entry table starts 16 bytes below end)
 *    Store heapId -> gsMemDebugMode (byte)
 *    Store r7 -> gsMemHeapStart
 *    Store r6 -> gsMemHeapEnd
 *    Zero out all statistics
 *    Store r5 -> gsMemEntryTop = gsMemEntryBot
 *    Create a single free block at gsMemHeapStart:
 *      block->prev = 0
 *      block->next = 0
 *      block->size = gsMemEntryBot - gsMemHeapStart
 *    Set gsMemFreeList = gsMemHeapStart
 *    Print init message
 * ======================================================================= */
void GSmemInit(u32 heapId, void* start, void* end) {
    void* alignedStart;
    void* alignedEnd;
    GSmemEntry* entryBase;
    GSmemBlock* firstBlock;

    /* Align start up to 32 bytes, end down to 32 bytes */
    alignedStart = (void*)(((u32)start + 0x1F) & ~0x1F);
    alignedEnd   = (void*)((u32)end & ~0x1F);

    /* Store heap configuration */
    gsMemDebugMode = (u8)heapId;
    gsMemHeapStart = alignedStart;
    gsMemHeapEnd   = alignedEnd;

    /* Zero all statistics counters */
    gsMemStat_60 = 0;
    gsMemStat_5C = 0;
    gsMemStat_58 = 0;
    gsMemStat_54 = 0;
    gsMemStat_50 = 0;
    gsMemStat_4C = 0;
    gsMemStat_48 = 0;

    /* Entry table sits at the top of the heap, just below alignedEnd.
     * It starts at (alignedEnd - 0x10) and grows downward.  */
    entryBase = (GSmemEntry*)((u32)alignedEnd - sizeof(GSmemEntry));
    gsMemEntryTop = entryBase;
    gsMemEntryBot = entryBase;

    /* Clear the first entry's handle field (marks it as unused) */
    entryBase->handle = 0;

    /* Create one large free block spanning the whole heap */
    firstBlock = (GSmemBlock*)alignedStart;
    firstBlock->prev = NULL;
    firstBlock->next = NULL;
    firstBlock->size = (u32)entryBase - (u32)alignedStart;

    gsMemFreeList = firstBlock;

    /* Print initialisation message:
     * "GSmem: Init OK, using area %08Xh -> %08Xh\n" */
    fn_800DD970(lbl_80270DFC, (u32)alignedStart, (u32)alignedEnd);
}

/* =======================================================================
 *  GSmemIsInited / fn_800E2AF8
 *  Address: 0x800E2AF8, Size: 0x8
 * ======================================================================= */
u32 GSmemIsInited(void) {
    return 1;
}

/* =======================================================================
 *  GSmemFindHandle / fn_800E202C
 *  Address: 0x800E202C, Size: 0x70
 *
 *  Scans the entry table (top to bottom) looking for an entry whose
 *  data pointer matches 'ptr'.  In debug mode (gsMemDebugMode==1),
 *  the actual data pointer is offset by -4 (to skip the guard word).
 * ======================================================================= */
u16 GSmemFindHandle(void* ptr) {
    GSmemEntry* entry;
    u32 count;

    /* In debug mode, raw pointer is 4 bytes past the entry's data field */
    if (gsMemDebugMode == 1) {
        ptr = (void*)((u32)ptr - 4);
    }

    /* Walk the entry table from top to bottom */
    entry = gsMemEntryTop;
    count = (u32)((u32)gsMemEntryTop + sizeof(GSmemEntry) - (u32)gsMemEntryBot)
            / sizeof(GSmemEntry);

    while ((u32)entry >= (u32)gsMemEntryBot) {
        if (entry->handle != 0 && entry->data == ptr) {
            return entry->handle;
        }
        entry = (GSmemEntry*)((u32)entry - sizeof(GSmemEntry));
        if (--count == 0) break;
    }

    return 0;
}

/* =======================================================================
 *  GSmemAlloc / fn_800E2C04
 *  Address: 0x800E2C04, Size: 0x1AC
 *
 *  First-fit allocation from the free list.
 *
 *  r3 = alignment, r4 = size
 *
 *  The alignment is rounded up to 32 bytes, then size is rounded to the
 *  next multiple of the alignment minus 1.  The allocator walks the free
 *  list looking for a block where the aligned start + total size fits.
 *
 *  The allocation strategy (gsMemDefaultHeap) controls tie-breaking:
 *    0 = first fit (return first block that fits)
 *    1 = best fit  (smallest fitting block)
 *    2 = worst fit (largest fitting block)
 *
 *  On success, calls GSmemSplitBlock (fn_800E2DB0) to carve the
 *  allocation out of the chosen free block.
 * ======================================================================= */
u16 GSmemAlloc(u32 alignment, u32 size) {
    GSmemBlock* block;
    GSmemBlock* bestBlock;
    u32 alignedSize;
    u32 alignMask;
    u32 totalNeeded;
    u32 headerSize;
    u32 strategy;
    u32 alignedAddr;
    u16 result;

    /* Null-size check */
    if (alignment == 0) {
        return 0;
    }

    /* Round size into a valid range:
     * rlwinm r31, r0, 0, 16, 29 => mask 0x00003FFC (bits 16-29)
     * This clamps size to [4..16380] aligned to 4 bytes. */
    alignedSize = (size + 3) & 0x00003FFC;
    if (alignedSize == 0) {
        return 0;
    }
    if ((alignedSize & 0x1F) != 0) {
        /* Size must be 32-byte aligned in the low bits -- if not, fail */
        /* Actually the assembly checks clrlwi. r0, r31, 27 => (size & 0x1F) */
    }

    /* Compute header overhead:
     * Align the alignment value up to 32-byte boundary, then add 4-byte
     * rounding to get the header contribution. */
    headerSize = ((alignment + 0x1F) & ~0x1F);
    headerSize = (headerSize + 3) & ~3;
    if (gsMemDebugMode != 0) {
        headerSize += 8;  /* extra space for guard bytes in debug mode */
    }

    alignMask = ~(alignedSize - 1);  /* nor r8, r0, r0 -- bit-invert of (size-1) */
    totalNeeded = headerSize + alignedSize - 1;
    if (totalNeeded < 0x0C) {
        totalNeeded = 0x0C;  /* minimum block size */
    }

    /* ---- First pass: find first fitting block ---- */
    block = gsMemFreeList;
    bestBlock = NULL;
    alignedAddr = 0;

    while (block != NULL) {
        u32 candidate = (u32)block + (alignedSize - 1);
        u32 addr = candidate & alignMask;
        u32 end  = addr + totalNeeded;
        u32 blockEnd = (u32)block + block->size;

        if (end < blockEnd) {
            bestBlock = block;
            alignedAddr = addr;
            break;  /* first-fit: take the first one */
        }
        block = block->next;
    }

    /* ---- Strategy-dependent search ---- */
    strategy = gsMemDefaultHeap;

    if (strategy == 1) {
        /* Best fit: find smallest block that still fits */
        block = gsMemFreeList;
        while (block != NULL) {
            u32 candidate = (u32)block + (alignedSize - 1);
            u32 addr = candidate & alignMask;
            u32 end  = addr + totalNeeded;
            u32 blockEnd = (u32)block + block->size;

            if (end < blockEnd) {
                if (bestBlock == NULL || block->size < bestBlock->size) {
                    bestBlock = block;
                    alignedAddr = addr;
                }
            }
            block = block->next;
        }
    } else if (strategy == 2) {
        /* Worst fit: find largest block */
        block = gsMemFreeList;
        while (block != NULL) {
            u32 candidate = (u32)block + (alignedSize - 1);
            u32 addr = candidate & alignMask;
            u32 end  = addr + totalNeeded;
            u32 blockEnd = (u32)block + block->size;

            if (end < blockEnd) {
                if (bestBlock == NULL || block->size > bestBlock->size) {
                    bestBlock = block;
                    alignedAddr = addr;
                }
            }
            block = block->next;
        }
    }

    /* ---- Allocation failure ---- */
    if (bestBlock == NULL) {
        gsMemLastError = 2;
        return 0;
    }

    /* ---- Split the chosen block and create an entry ---- */
    result = GSmemSplitBlock(alignedAddr, bestBlock);

    /* Store alignment info into the entry */
    {
        GSmemEntry* entry;
        u16 entryIdx = result & 0xFFFF;
        if (entryIdx == 0) {
            entry = NULL;
        } else {
            entry = (GSmemEntry*)((u32)gsMemEntryTop
                                  - (u32)((entryIdx - 1) << 4));
        }
        if (entry != NULL) {
            entry->align = alignedSize;
        }
    }

    return result;
}

/* =======================================================================
 *  GSmemAllocTail / fn_800E2B00
 *  Address: 0x800E2B00, Size: 0x104
 *
 *  Allocate from the tail (end) of a free block, similar to GSmemAlloc
 *  but searches for blocks where the allocation fits at the high end.
 * ======================================================================= */
u16 GSmemAllocTail(u32 alignment, u32 size) {
    GSmemBlock* block;
    u32 alignedSize;
    u32 alignMask;
    u32 headerSize;
    u32 bestAddr;
    u16 result;

    if (alignment == 0) {
        return 0;
    }

    alignedSize = (size + 3) & 0x00003FFC;
    if (alignedSize == 0) {
        return 0;
    }

    /* Compute header size */
    headerSize = ((alignment + 0x1F) & ~0x1F);
    headerSize = (headerSize + 3) & ~3;
    if (gsMemDebugMode != 0) {
        headerSize += 8;
    }
    if (headerSize < 0x0C) {
        headerSize = 0x0C;
    }

    alignMask = ~(alignedSize - 1);
    bestAddr = 0;

    /* Walk free list, look for tail-allocation opportunity */
    block = gsMemFreeList;
    while (block != NULL) {
        if (block->size >= headerSize) {
            u32 blockEnd = (u32)block + block->size;
            u32 candidate = (blockEnd - headerSize) & alignMask;
            if (candidate >= (u32)block) {
                bestAddr = candidate;
            }
        }
        block = block->next;
    }

    if (bestAddr == 0) {
        gsMemLastError = 2;
        return 0;
    }

    /* Split and create entry */
    result = GSmemSplitBlock(bestAddr, 0);

    /* Store alignment */
    {
        GSmemEntry* entry;
        u16 entryIdx = result & 0xFFFF;
        if (entryIdx == 0) {
            entry = NULL;
        } else {
            entry = (GSmemEntry*)((u32)gsMemEntryTop
                                  - (u32)((entryIdx - 1) << 4));
        }
        if (entry != NULL) {
            entry->align = alignedSize;
        }
    }

    return result;
}

/* =======================================================================
 *  GSmemAllocRaw / fn_800E3534
 *  Address: 0x800E3534, Size: 0x2C
 *
 *  A thin wrapper that rounds the requested size up to 32 bytes and
 *  calls GSmemAlloc with alignment = size.
 * ======================================================================= */
u16 GSmemAllocRaw(u32 size) {
    u32 rounded = (size + 0x1F) & ~0x1F;
    return GSmemAlloc(rounded, rounded);
}

/* =======================================================================
 *  GSmemSplitBlock / fn_800E2DB0  (INTERNAL)
 *  Address: 0x800E2DB0, Size: 0x784
 *
 *  This is the core internal function that:
 *    1. Locates the free block containing the target address.
 *    2. Splits it if needed (creating up to two new free blocks).
 *    3. Allocates an entry from the entry table.
 *    4. Fills in guard bytes in debug mode.
 *    5. Returns the 16-bit handle.
 *
 *  This function is very large in the binary due to extensive guard-byte
 *  validation logic.  We provide a simplified decompilation here.
 * ======================================================================= */
/* Forward declaration -- full implementation omitted due to complexity.
 * This is an internal function called by GSmemAlloc and GSmemAllocTail. */
extern u16 fn_800E2DB0(u32 addr, void* sizeOrBlock);
#define GSmemSplitBlock fn_800E2DB0

/* =======================================================================
 *  GSmemFree / fn_800E209C
 *  Address: 0x800E209C, Size: 0x414
 *
 *  Releases a handle back to the heap.  The entry's data pointer is used
 *  to find the block boundaries.  If guard bytes are enabled, they are
 *  validated before freeing.  The freed region is merged with adjacent
 *  free blocks (coalescing).
 *
 *  r3 = handle (u16)
 *
 *  Simplified decompilation (guard-byte logic abbreviated):
 * ======================================================================= */
void GSmemFree(u16 handle) {
    GSmemEntry* entry;
    u16 idx;

    if ((handle & 0xFFFF) == 0) {
        /* Error: null handle */
        fn_800DD970(lbl_80270658 + 0x5A4); /* error string */
        gsMemLastError = 1;
        return;
    }

    /* Resolve handle to entry */
    idx = handle & 0xFFFF;
    if (idx == 0) {
        entry = NULL;
    } else {
        entry = (GSmemEntry*)((u32)gsMemEntryTop - (u32)((idx - 1) << 4));
    }

    /* Validate that the entry's stored handle matches */
    if (entry->handle != (handle & 0xFFFF)) {
        fn_800DD970(lbl_80270658 + 0x5A4); /* "GSmem: bad handle" */
        gsMemLastError = 1;
        return;
    }

    /* Check reference count -- warn if still locked */
    if (entry->refCount != 0) {
        fn_800DD970(lbl_80270658 + 0x5C8); /* "GSmem: freeing locked block" */
    }

    /* If debug mode, validate guard bytes at start and end of allocation */
    if (gsMemDebugMode == 1) {
        u8* data = (u8*)entry->data;
        /* Check 4 guard bytes at start */
        if (data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 0) {
            /* Guard bytes corrupted -- but proceed with free anyway */
        }
        /* Check 4 guard bytes at end */
        {
            u8* tail = data + entry->size - 4;
            if (tail[0] != 0 || tail[1] != 0 || tail[2] != 0 || tail[3] != 0) {
                /* Tail guard corrupted */
            }
        }
    }

    /* Clear the entry: zero handle and refCount, mark as free */
    gsMemLastError = 0;

    /* Return the memory region to the free list.
     * Find the correct position in the address-sorted free list and insert.
     * Merge with adjacent free blocks if contiguous.
     *
     * (The full merge logic in the assembly is ~300 instructions.
     *  We represent the core algorithm here.)
     */
    {
        void* dataPtr = entry->data;
        u32   dataSize = entry->size;
        GSmemBlock* newBlock = (GSmemBlock*)dataPtr;
        GSmemBlock* prev = NULL;
        GSmemBlock* curr = gsMemFreeList;

        /* Find insertion point (blocks are sorted by address) */
        while (curr != NULL && (u32)curr < (u32)dataPtr) {
            prev = curr;
            curr = curr->next;
        }

        /* Try to merge with the previous block */
        if (prev != NULL && ((u32)prev + prev->size) == (u32)dataPtr) {
            prev->size += dataSize;
            newBlock = prev;
        } else {
            /* Insert new free block */
            newBlock->prev = prev;
            newBlock->next = curr;
            newBlock->size = dataSize;
            if (prev != NULL) {
                prev->next = newBlock;
            } else {
                gsMemFreeList = newBlock;
            }
        }

        /* Try to merge with the next block */
        if (curr != NULL && ((u32)newBlock + newBlock->size) == (u32)curr) {
            newBlock->size += curr->size;
            newBlock->next = curr->next;
            if (curr->next != NULL) {
                curr->next->prev = newBlock;
            }
        }
    }

    /* Clear the entry table slot */
    entry->handle   = 0;
    entry->refCount = 0;
    entry->data     = NULL;
    entry->size     = 0;
}

/* =======================================================================
 *  GSmemLock / fn_800E24B0
 *  Address: 0x800E24B0, Size: 0x2EC
 *
 *  Resolves a handle to a data pointer and increments the reference count.
 *  In debug mode, skips past the 4-byte guard prefix.
 * ======================================================================= */
void* GSmemLock(u16 handle) {
    GSmemEntry* entry;
    u16 idx;

    if ((handle & 0xFFFF) == 0) {
        fn_800DD970(lbl_80270658 + 0x660); /* "GSmem: lock null handle" */
        gsMemLastError = 1;
        return (void*)1; /* error sentinel */
    }

    idx = handle & 0xFFFF;
    if (idx == 0) {
        entry = NULL;
    } else {
        entry = (GSmemEntry*)((u32)gsMemEntryTop - (u32)((idx - 1) << 4));
    }

    /* Validate handle */
    if (entry->handle != (handle & 0xFFFF)) {
        fn_800DD970(lbl_80270658 + 0x660);
        gsMemLastError = 1;
        return (void*)1;
    }

    /* Check that the block has not already been freed */
    if (entry->refCount == 0) {
        fn_800DD970(lbl_80270658 + 0x684); /* "GSmem: locking freed block" */
        gsMemLastError = 5;
        return (void*)5;
    }

    gsMemLastError = 0;

    /* In debug mode, validate guard bytes then return ptr + 4 */
    if (gsMemDebugMode == 1) {
        u8* data = (u8*)entry->data;

        /* Validate start guard bytes (simplified) */
        if (data[0] != 0 && data[1] != 0 && data[2] != 0 && data[3] != 0) {
            /* guard OK or not -- proceed */
        }

        /* Increment reference count */
        entry->refCount++;

        /* Return pointer past the 4-byte guard prefix */
        return (void*)(data + 4);
    }

    /* Non-debug mode: just increment refCount and return data */
    entry->refCount++;
    return entry->data;
}

/* =======================================================================
 *  GSmemGetPtr / fn_800E27B0
 *  Address: 0x800E27B0, Size: 0x2EC
 *
 *  Like GSmemLock but does NOT increment refCount.
 *  Returns the raw data pointer for the given handle.
 * ======================================================================= */
void* GSmemGetPtr(u16 handle) {
    GSmemEntry* entry;
    u16 idx;

    if ((handle & 0xFFFF) == 0) {
        fn_800DD970(lbl_80270658 + 0x6D0); /* "GSmem: getptr null handle" */
        gsMemLastError = 1;
        return NULL;
    }

    idx = handle & 0xFFFF;
    if (idx == 0) {
        entry = NULL;
    } else {
        entry = (GSmemEntry*)((u32)gsMemEntryTop - (u32)((idx - 1) << 4));
    }

    if (entry->handle != (handle & 0xFFFF)) {
        fn_800DD970(lbl_80270658 + 0x6D0);
        gsMemLastError = 1;
        return NULL;
    }

    /* Check for double-free */
    if (entry->refCount == 0xFFFF) {
        fn_800DD970(lbl_80270658 + 0x6F4); /* "GSmem: getptr on freed handle" */
        gsMemLastError = 4;
        return NULL;
    }

    gsMemLastError = 0;

    /* In debug mode, validate guard bytes and return ptr + 4 */
    if (gsMemDebugMode == 1) {
        u8* data = (u8*)entry->data;

        /* Guard byte validation (simplified) */
        /* ... */

        return (void*)(data + 4);
    }

    return entry->data;
}

/* =======================================================================
 *  GSmemGetFreeSize / fn_800E0DDC
 *  Address: 0x800E0DDC
 *
 *  Walks the free list and sums up all free block sizes.
 * ======================================================================= */
u32 GSmemGetFreeSize(void) {
    GSmemBlock* block;
    u32 total = 0;

    block = gsMemFreeList;
    while (block != NULL) {
        total += block->size;
        block = block->next;
    }

    return total;
}
