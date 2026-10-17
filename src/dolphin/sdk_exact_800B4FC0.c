#include "src/dolphin/card_dsp_private.h"

extern s32 __CARDPutControlBlock(CARDControl* card, s32 result);
extern s32 __CARDWrite(s32 chan, u32 addr, u32 length, void* buffer,
                       CARDCallback callback);
extern void WriteCallback(s32 chan, s32 result);

void EraseCallback(s32 chan, s32 result)
{
    CARDControl* card = &lbl_803FC620[chan];
    CARDCallback callback;
    CARDFileInfo* fileInfo;

    if (result >= 0) {
        fileInfo = card->fileInfo;
        result = __CARDWrite(chan, card->sectorSize * fileInfo->startBlock,
                            card->sectorSize, card->buffer, WriteCallback);
        if (result >= 0) {
            return;
        }
    }
    callback = card->apiCallback;
    card->apiCallback = NULL;
    __CARDPutControlBlock(card, result);
    callback(chan, result);
}
