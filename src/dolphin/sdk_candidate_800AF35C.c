/** Standalone owner for __CARDExiHandler. */
#include "dolphin/exi/EXI.h"
#include "dolphin/os/OSContext.h"
#include "src/dolphin/card_dsp_private.h"

extern s32 fn_800AF660(s32 chan, u8* status);
extern s32 __CARDClearStatus(s32 chan);
extern s32 fn_800AF8A0(s32 chan);

void __CARDExiHandler(s32 chan, OSContext* context)
{
    CARDControl* card;
    CARDCallback callback;
    u8 status;
    s32 result;

    (void)context;

    card = &lbl_803FC620[chan];
    OSCancelAlarm(&card->alarm);
    if (!card->attached) {
        return;
    }
    if (!EXILock(chan, 0, NULL)) {
        result = -128;
        goto fatal;
    }
    result = fn_800AF660(chan, &status);
    if (result < 0 || (result = __CARDClearStatus(chan)) < 0) {
        goto error;
    }
    result = (status & 0x18) ? -5 : 0;
    if (result == -5 && --card->field_A8 > 0) {
        result = fn_800AF8A0(chan);
        if (result >= 0) {
            return;
        }
        goto fatal;
    }

error:
    EXIUnlock(chan);

fatal:
    callback = card->callback_CC;
    if (callback != NULL) {
        card->callback_CC = NULL;
        callback(chan, result);
    }
}
