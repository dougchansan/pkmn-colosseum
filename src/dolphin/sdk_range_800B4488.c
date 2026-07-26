/**
 * @file sdk_range_800B4488.c
 * @brief Shared pure-C candidate source for 0x800B4488 - 0x800B71F0.
 */
#define CARD_EXACT_800B3B68_ONLY
#include "src/dolphin/sdk_range_800AE3F0.c"

static inline BOOL CARDIsValidBlockNoLocal(CARDControl* card, u16 block)
{
    return 5 <= block && block < card->cBlock;
}

s32 fn_800B4488(s32 chan, s32 fileNo, CARDFileInfo* fileInfo)
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
    result = fn_800B4270(card, entry);
    if (result == -10) {
        result = fn_800B4308(entry);
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
