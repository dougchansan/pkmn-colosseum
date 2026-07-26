/**
 * @file gs_scratch.c
 * @brief GSscratch (scratch/ARAM-backed allocator)
 *
 * Split from gs_range_800E202C.c (0x800EE928-0x800EEF48) — one XD source unit per
 * segment (Fable re-split, 2026-07-07). Functions asm-only until matched.
 */
#include "dolphin/types.h"

typedef struct GSscratchAllocation {
    u8 firstBlock;
    u8 blockCount;
    u8 _pad[2];
    void (*callback)(BOOL valid, void *ptr, u8 blockCount);
} GSscratchAllocation;

extern GSscratchAllocation lbl_804018F0[32];
extern u8 *lbl_8047ABE0;
extern u8 lbl_8047ABE8;
extern u32 lbl_8047ABEC;
extern u32 lbl_8047ABDC;
extern u32 lbl_8047ABD8;

extern void LCEnable(void);

void GSscratchSetValid(void)
{
    s32 count;
    GSscratchAllocation *allocation;

    if (lbl_8047ABE8 != 0) {
        allocation = lbl_804018F0;
        count = 32;
        while (count-- != 0) {
            if (allocation->firstBlock != 0xFF &&
                allocation->callback != NULL) {
                allocation->callback(
                    TRUE,
                    lbl_8047ABE0 + allocation->firstBlock * 0x200,
                    allocation->blockCount);
            }
            allocation++;
        }
        lbl_8047ABE8 = 0;
    }
}

void GSscratchSetInvalid(void)
{
    s32 count;
    GSscratchAllocation *allocation;

    if (lbl_8047ABE8 != 1) {
        allocation = lbl_804018F0;
        count = 32;
        while (count-- != 0) {
            if (allocation->firstBlock != 0xFF &&
                allocation->callback != NULL) {
                allocation->callback(
                    FALSE,
                    lbl_8047ABE0 + allocation->firstBlock * 0x200,
                    allocation->blockCount);
            }
            allocation++;
        }
        lbl_8047ABE8 = 1;
    }
}

static inline GSscratchAllocation *GSscratchFindAllocation(u8 firstBlock)
{
    GSscratchAllocation *allocation;
    u32 i;

    allocation = lbl_804018F0;
    for (i = 0; i < 32; allocation++, i++) {
        if (allocation->firstBlock == firstBlock) {
            return allocation;
        }
    }
    return NULL;
}

static inline GSscratchAllocation *GSscratchFindFreeAllocation(void)
{
    GSscratchAllocation *allocation;
    u32 i;

    allocation = lbl_804018F0;
    for (i = 0; i < 32; allocation++, i++) {
        if (allocation->firstBlock == 0xFF) {
            return allocation;
        }
    }
    return NULL;
}

u8 GSscratchIsPtr(void *ptr)
{
    return ((u32)ptr & 0xF0000000) == ((u32)lbl_8047ABE0 & 0xF0000000);
}

void GSscratchFree(void *ptr)
{
    GSscratchAllocation *allocation;
    u32 blockMask;
    u8 firstBlock;
    u8 blockCount;
    u32 usedBlocks;

    allocation = GSscratchFindAllocation(
        ((u32)ptr - (u32)lbl_8047ABE0) >> 9);
    if (allocation == NULL) {
        return;
    }

    blockCount = allocation->blockCount;
    firstBlock = allocation->firstBlock;
    blockMask = 0x80000000;
    while (firstBlock-- != 0) {
        blockMask >>= 1;
    }

    usedBlocks = lbl_8047ABEC;
    while (blockCount-- != 0) {
        usedBlocks &= ~blockMask;
        blockMask >>= 1;
    }
    lbl_8047ABEC = usedBlocks;
    allocation->firstBlock = 0xFF;
}

void *GSscratchAlloc(u8 blockCount,
                     void (*callback)(BOOL valid, void *ptr, u8 blockCount))
{
    GSscratchAllocation *allocation;
    u32 usedMask;
    u32 scanMask;
    u32 occupied;
    u8 firstBlock;
    u8 remaining;

    if (lbl_8047ABE8 == 1 || blockCount == 0 || blockCount > 32) {
        return NULL;
    }

    for (firstBlock = 0; firstBlock < 32; firstBlock++) {
        scanMask = 0x80000000 >> firstBlock;
        occupied = 0;
        remaining = blockCount;
        while (remaining-- != 0) {
            occupied <<= 1;
            if (scanMask == 0 || (lbl_8047ABEC & scanMask) != 0) {
                occupied |= 1;
            }
            scanMask >>= 1;
        }
        if (occupied != 0) {
            continue;
        }

        allocation = GSscratchFindFreeAllocation();
        if (allocation == NULL) {
            return NULL;
        }

        allocation->firstBlock = firstBlock;
        allocation->blockCount = blockCount;
        allocation->callback = callback;
        usedMask = lbl_8047ABEC;
        scanMask = 0x80000000 >> firstBlock;
        remaining = blockCount;
        while (remaining-- != 0) {
            usedMask |= scanMask;
            scanMask >>= 1;
        }
        lbl_8047ABEC = usedMask;
        return lbl_8047ABE0 + firstBlock * 0x200;
    }
    return NULL;
}

void GSscratchInit(u8 reservedBlocks)
{
    GSscratchAllocation *allocation;
    u32 reservedMask;
    u8 i;

    lbl_8047ABEC = 0;
    allocation = lbl_804018F0;
    for (i = 0; i < 32; allocation++, i++) {
        allocation->firstBlock = 0xFF;
    }

    LCEnable();
    lbl_8047ABD8 = reservedBlocks << 9;
    lbl_8047ABE0 = (u8*)0xE0000000;
    if (reservedBlocks != 0) {
        reservedMask = 0xFFFFFFFF << (32 - reservedBlocks);
        lbl_8047ABEC = reservedMask;
        lbl_804018F0[0].firstBlock = 0;
        lbl_804018F0[0].blockCount = reservedBlocks;
    }
    lbl_8047ABE8 = 0;
}

extern void LCQueueWait(u32 len);
extern u32 LCQueueLength(void);
extern u32 LCStoreData(void *dest, void *src, u32 len);

void GSscratchWaitForCompletion(void)
{
    LCQueueWait(lbl_8047ABDC);
    lbl_8047ABDC = 0;
}

u32 GSscratchStore(void *dest, void *src, u32 len)
{
    u32 queued;

    if (LCQueueLength() >= 15) {
        return 1;
    }
    if (((u32)src & 0x1F) != 0) {
        return 2;
    }
    if (((u32)dest & 0x1F) != 0) {
        return 2;
    }
    if ((len & 0x1F) != 0) {
        return 2;
    }

    queued = LCStoreData(dest, src, len);
    lbl_8047ABDC += queued;
    return 0;
}
