#include "dolphin/exi/EXI.h"
#include "dolphin/os/OSContext.h"
#include "src/dolphin/card_dsp_private.h"

extern EXICallback EXISetExiCallback(s32 chan, EXICallback callback);

void __CARDDefaultApiCallback(s32 chan, s32 result)
{
}

void __CARDExtHandler(s32 chan, OSContext* context)
{
    CARDControl* card = &lbl_803FC620[chan];
    CARDCallback callback;

    if (card->attached != 0) {
        card->attached = 0;
        EXISetExiCallback(chan, NULL);
        OSCancelAlarm(&card->alarm);
        callback = card->callback_CC;
        if (callback != NULL) {
            card->callback_CC = NULL;
            callback(chan, -3);
        }
        if (card->result != -1) {
            card->result = -3;
        }
        callback = card->extCallback;
        if (callback != NULL && card->field_24 >= 7) {
            card->extCallback = NULL;
            callback(chan, -3);
        }
    }
}
