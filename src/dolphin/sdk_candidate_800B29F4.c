#include "src/dolphin/card_dsp_private.h"

extern s32 VerifyID(CARDControl* card);
extern s32 VerifyDir(CARDControl* card, s32* checkCode);
extern s32 VerifyFAT(CARDControl* card, s32* checkCode);
extern s32 __CARDUpdateDir(s32 chan, CARDCallback callback);
extern s32 __CARDUpdateFatBlock(s32 chan, u16* fat, CARDCallback callback);
extern void* memcpy(void* dst, const void* src, u32 length);
extern void* memset(void* dst, s32 value, u32 size);
extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL level);

static inline void cardCheckSumInline(void* ptr, s32 length, u16* checksum,
                                      u16* checksumInv)
{
    u16* p;
    s32 i;

    length /= sizeof(u16);
    *checksum = *checksumInv = 0;
    for (i = 0, p = ptr; i < length; i++, p++) {
        *checksum += *p;
        *checksumInv += ~*p;
    }
    if (*checksum == 0xFFFF) {
        *checksum = 0;
    }
    if (*checksumInv == 0xFFFF) {
        *checksumInv = 0;
    }
}

s32 CARDCheckExAsync(s32 chan, s32* xferBytes, CARDCallback callback)
{
    CARDControl* card;
    CARDDirEntry* dir[2];
    u16* fat[2];
    u16* map;
    s32 result;
    s32 errors;
    s32 currentFat;
    s32 currentDir;
    s32 fileNo;
    u16 iBlock;
    u16 cBlock;
    u16 cFree;
    BOOL updateFat = FALSE;
    BOOL updateDir = FALSE;
    BOOL updateOrphan = FALSE;

    if (xferBytes != NULL) {
        *xferBytes = 0;
    }

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    result = VerifyID(card);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }

    errors = VerifyDir(card, &currentDir);
    errors += VerifyFAT(card, &currentFat);
    if (errors > 1) {
        return __CARDPutControlBlock(card, -6);
    }

    dir[0] = (CARDDirEntry*)((u8*)card->workArea + 0x2000);
    dir[1] = (CARDDirEntry*)((u8*)card->workArea + 0x4000);
    fat[0] = (u16*)((u8*)card->workArea + 0x6000);
    fat[1] = (u16*)((u8*)card->workArea + 0x8000);

    switch (errors) {
    case 0:
        break;
    case 1:
        if (card->dirBlock == NULL) {
            card->dirBlock = dir[currentDir];
            memcpy(dir[currentDir], dir[currentDir ^ 1], 0x2000);
            updateDir = TRUE;
        } else {
            card->fatBlock = fat[currentFat];
            memcpy(fat[currentFat], fat[currentFat ^ 1], 0x2000);
            updateFat = TRUE;
        }
        break;
    }

    map = fat[currentFat ^ 1];
    memset(map, 0, 0x2000);

    for (fileNo = 0; fileNo < 127; fileNo++) {
        CARDDirEntry* entry;

        entry = &((CARDDirEntry*)card->dirBlock)[fileNo];
        if (entry->gameName[0] == 0xFF) {
            continue;
        }

        for (iBlock = entry->startBlock, cBlock = 0;
             iBlock != 0xFFFF && cBlock < entry->length;
             iBlock = ((u16*)card->fatBlock)[iBlock], cBlock++) {
            if (iBlock < 5 || iBlock >= card->cBlock ||
                ++map[iBlock] > 1) {
                return __CARDPutControlBlock(card, -6);
            }
        }
        if (cBlock != entry->length || iBlock != 0xFFFF) {
            return __CARDPutControlBlock(card, -6);
        }
    }

    cFree = 0;
    for (iBlock = 5; iBlock < card->cBlock; iBlock++) {
        u16 nextBlock;

        nextBlock = ((u16*)card->fatBlock)[iBlock];
        if (map[iBlock] == 0) {
            if (nextBlock != 0) {
                ((u16*)card->fatBlock)[iBlock] = 0;
                updateOrphan = TRUE;
            }
            cFree++;
        } else if ((nextBlock < 5 || nextBlock >= card->cBlock) &&
                   nextBlock != 0xFFFF) {
            return __CARDPutControlBlock(card, -6);
        }
    }

    if (cFree != ((u16*)card->fatBlock)[3]) {
        ((u16*)card->fatBlock)[3] = cFree;
        updateOrphan = TRUE;
    }
    if (updateOrphan) {
        cardCheckSumInline(&((u16*)card->fatBlock)[2], 0x1FFC,
                           &((u16*)card->fatBlock)[0],
                           &((u16*)card->fatBlock)[1]);
    }

    memcpy(fat[currentFat ^ 1], fat[currentFat], 0x2000);

    if (updateDir) {
        if (xferBytes != NULL) {
            *xferBytes = 0x2000;
        }
        return __CARDUpdateDir(chan, callback);
    }

    if (updateFat | updateOrphan) {
        if (xferBytes != NULL) {
            *xferBytes = 0x2000;
        }
        return __CARDUpdateFatBlock(chan, card->fatBlock, callback);
    }

    __CARDPutControlBlock(card, 0);
    if (callback != NULL) {
        BOOL enabled;

        enabled = OSDisableInterrupts();
        callback(chan, 0);
        OSRestoreInterrupts(enabled);
    }
    return 0;
}
