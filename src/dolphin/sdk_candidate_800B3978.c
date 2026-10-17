/**
 * @file sdk_candidate_800B3978.c
 * @brief CARDUnmount (CARDMount.c tail) and FormatCallback (CARDFormat.c
 * head), 0x800B3978 - 0x800B3B68.
 *
 * CARDUnmount inlines DoUnmount (linked in sdk_exact_800B38DC.c), so this
 * carve keeps a static inline copy. Forms follow the Dolphin SDK as
 * decompiled in XD:
 * https://github.com/TeamOrre/xd-decomp/blob/4989794e6c6430684e033bc56f4bb97c9a921e73/src/dolphin/card/CARDMount.c
 * https://github.com/TeamOrre/xd-decomp/blob/4989794e6c6430684e033bc56f4bb97c9a921e73/src/dolphin/card/CARDFormat.c
 */

#include "dolphin/exi/EXI.h"
#include "dolphin/os/OSInterrupt.h"
#include "src/dolphin/card_dsp_private.h"

extern EXICallback EXISetExiCallback(s32 chan, EXICallback callback);
extern void EXIDetach(s32 chan);
extern void* memcpy(void* dst, const void* src, u32 n);
extern s32 __CARDEraseSector(s32 chan, u32 addr, CARDCallback callback); /* __CARDEraseSector */
extern s32 __CARDWrite(s32 chan, u32 addr, s32 length, void* buffer,
                       CARDCallback callback); /* __CARDWrite */

static inline void DoUnmount(s32 chan, s32 result)
{
    CARDControl* card;
    BOOL enabled;

    card = &lbl_803FC620[chan];
    enabled = OSDisableInterrupts();
    if (card->attached) {
        EXISetExiCallback(chan, NULL);
        EXIDetach(chan);
        OSCancelAlarm(&card->alarm);
        card->attached = FALSE;
        card->result = result;
        card->field_24 = 0;
    }
    OSRestoreInterrupts(enabled);
}

s32 CARDUnmount(s32 chan)
{
    CARDControl* card;
    s32 result;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    DoUnmount(chan, -3);
    return 0;
}

void FormatCallback(s32 chan, s32 result)
{
    CARDControl* card;
    CARDCallback callback;

    card = &lbl_803FC620[chan];
    if (result < 0) {
        goto error;
    }

    ++card->formatStep;
    if (card->formatStep < 5) {
        result = __CARDEraseSector(chan, card->sectorSize * card->formatStep,
                             FormatCallback);
        if (result >= 0) {
            return;
        }
    } else if (card->formatStep < 10) {
        int step = card->formatStep - 5;
        result = __CARDWrite(chan, card->sectorSize * step, 0x2000,
                             (u8*)card->workArea + 0x2000 * step,
                             FormatCallback);
        if (result >= 0) {
            return;
        }
    } else {
        card->dirBlock = (u8*)card->workArea + 0x2000;
        memcpy(card->dirBlock, (u8*)card->workArea + 0x4000, 0x2000);
        card->fatBlock = (u8*)card->workArea + 0x6000;
        memcpy(card->fatBlock, (u8*)card->workArea + 0x8000, 0x2000);
    }

error:
    callback = card->apiCallback;
    card->apiCallback = NULL;
    __CARDPutControlBlock(card, result);
    callback(chan, result);
}
