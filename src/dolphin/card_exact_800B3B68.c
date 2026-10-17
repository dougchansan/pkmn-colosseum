/**
 * @file card_exact_800B3B68.c
 * @brief Exact pure-C CARD island, 0x800B3B68 - 0x800B4644: CARDFormat.c's
 *        tail and CARDOpen.c (9 functions).
 */
#define CARD_EXACT_800B3B68_ONLY
#include "src/dolphin/sdk_range_800AE3F0.c"

/* The SDK's CARDIsValidBlockNo (a macro in card.h). */
#define CARDIsValidBlockNoLocal(card, block) (5 <= (block) && (block) < (card)->cBlock)

s32 CARDOpen(s32 chan, s32 fileNo, CARDFileInfo* fileInfo)
{
    CARDControl* card;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    s32 result;

    if (fileNo < 0 || fileNo >= 127) {
        return -128;
    }

    fileInfo->chan = -1;
    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileNo];
    result = __CARDAccess(card, entry);
    if (result == -10) {
        result = __CARDIsWritable(entry);
    }
    if (result >= 0) {
        if (!CARDIsValidBlockNoLocal(card, entry->startBlock)) {
            result = -6;
        } else {
            fileInfo->chan = chan;
            fileInfo->fileNo = fileNo;
            fileInfo->offset = 0;
            fileInfo->startBlock = entry->startBlock;
        }
    }

    return __CARDPutControlBlock(card, result);
}

s32 CARDClose(CARDFileInfo* fileInfo)
{
    CARDControl* card;
    s32 result;

    result = __CARDGetControlBlock(fileInfo->chan, &card);
    if (result < 0) {
        return result;
    }
    fileInfo->chan = -1;
    return __CARDPutControlBlock(card, 0);
}

BOOL __CARDIsOpened(CARDControl* card, s32 fileNo)
{
    return FALSE;
}
