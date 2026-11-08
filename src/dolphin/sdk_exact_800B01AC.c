#include "dolphin/os/OSInterrupt.h"
#include "src/dolphin/card_dsp_private.h"

extern s32 CARDUnmount(s32 chan);

DVDDiskID* CARDGetChanWord108(s32 chan)
{
    return lbl_803FC620[chan].diskId;
}

s32 CARDSetChanWord108(s32 chan, DVDDiskID* diskId)
{
    CARDControl* card = &lbl_803FC620[chan];
    DVDDiskID* id;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (card->result == -1) {
        return -1;
    }
    if (diskId != NULL) {
        id = diskId;
    } else {
        id = (DVDDiskID*)0x80000000;
    }
    card->diskId = id;
    OSRestoreInterrupts(enabled);
    return 0;
}

s32 __CARDGetControlBlock(s32 chan, CARDControl** pcard)
{
    s32 result;
    CARDControl* card = &lbl_803FC620[chan];
    BOOL enabled;

    if (chan < 0 || chan >= 2 || card->diskId == NULL) {
        return -128;
    }
    enabled = OSDisableInterrupts();
    if (card->attached == 0) {
        result = -3;
    } else if (card->result == -1) {
        result = -1;
    } else {
        card->result = -1;
        card->apiCallback = NULL;
        *pcard = card;
        result = 0;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

s32 __CARDPutControlBlock(CARDControl* card, s32 result)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (card->attached) {
        card->result = result;
    } else if (card->result == -1) {
        card->result = result;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

s32 CARDGetResultCode(s32 chan)
{
    CARDControl* card;

    if (chan < 0 || chan >= 2) {
        return -128;
    }
    card = &lbl_803FC620[chan];
    return card->result;
}

s32 CARDFreeBlocks(s32 chan, s32* byteNotUsed, s32* filesNotUsed)
{
    CARDControl* card;
    s32 result;
    u16* fat;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    u16 fileNo;
    u16* __CARDGetFatBlock(CARDControl* card);
    CARDDirEntry* __CARDGetDirBlock(CARDControl* card);

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    fat = __CARDGetFatBlock(card);
    dir = __CARDGetDirBlock(card);
    if (fat == NULL || dir == NULL) {
        return __CARDPutControlBlock(card, -6);
    }

    if (byteNotUsed != NULL) {
        *byteNotUsed = (s32)(card->sectorSize * fat[3]);
    }

    if (filesNotUsed != NULL) {
        *filesNotUsed = 0;
        for (fileNo = 0; fileNo < 127; fileNo++) {
            entry = &dir[fileNo];
            if ((u8)entry->fileName[0] == 0xFF) {
                ++*filesNotUsed;
            }
        }
    }

    return __CARDPutControlBlock(card, 0);
}

BOOL OnReset_800C0734(BOOL final)
{
    if (!final) {
        if (CARDUnmount(0) == -1 || CARDUnmount(1) == -1) {
            return FALSE;
        }
    }
    return TRUE;
}

u32 bitrev(u32 data)
{
    u32 work;
    u32 i;
    u32 k = 0;
    u32 j = 1;

    work = 0;
    for (i = 0; i < 32; i++) {
        if (i > 15) {
            if (i == 31) {
                work |= (((data & (1u << 31)) >> 31) & 1);
            } else {
                work |= ((data & (1u << i)) >> j);
                j += 2;
            }
        } else {
            work |= ((data & (1u << i)) << (31 - i - k));
            k++;
        }
    }

    return work;
}
