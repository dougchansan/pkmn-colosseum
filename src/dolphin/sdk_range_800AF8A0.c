/**
 * @file sdk_range_800AF8A0.c
 * @brief CARDBios.c middle, 0x800AF8A0 - 0x800B016C (Retry .. CARDInit).
 *
 * Standalone carve of sdk_range_800AE3F0.c: SDK_EXACT_800AF8A0_800B016C
 * hides the range file's bodies, so only these seven functions are emitted,
 * in address order. CARDBios.c defines the control blocks (__CARDBlock,
 * here lbl_803FC620), so this unit owns that .bss. CARDInit keeps the base
 * address in r30 only when the array is defined in the translation unit.
 */

#define SDK_EXACT_800AF8A0_800B016C
#include "src/dolphin/sdk_range_800AE3F0.c"

/* __CARDBlock */
CARDControl lbl_803FC620[2];

void TimeoutHandler(OSAlarm* alarm);
void __CARDTxHandler(s32 chan);
void UnlockedCallback(s32 chan, s32 result);
BOOL EXIImmEx(s32 chan, void* buf, s32 len, u32 mode);
extern u16 OSGetFontEncode(void);
extern void OSRegisterResetFunction(void* info);
extern void __CARDSetDiskID(void* diskId);

#define OS_BUS_CLOCK (*(u32*) 0x800000F8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)
#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))
#define OSSecondsToTicks(sec) ((OSTime) (sec) * OS_TIMER_CLOCK)

static inline void SetupTimeoutAlarm(CARDControl* card)
{
    OSCancelAlarm(&card->alarm);
    switch (card->cmd[0]) {
    case 0xF2:
        OSSetAlarm(&card->alarm, OSMillisecondsToTicks(100),
                   (OSAlarmHandler) TimeoutHandler);
        break;
    case 0xF3:
        break;
    case 0xF4:
    case 0xF1:
        OSSetAlarm(&card->alarm,
                   OSSecondsToTicks((OSTime) 2) *
                       (card->sectorSize / 0x2000),
                   (OSAlarmHandler) TimeoutHandler);
        break;
    }
}

s32 Retry(s32 chan)
{
    CARDControl* card;
    CARDControl* command;

    card = &lbl_803FC620[chan];
    if (!EXISelect(chan, 0, 4)) {
        EXIUnlock(chan);
        return -3;
    }

    SetupTimeoutAlarm(card);
    command = card;
    if (!EXIImmEx(chan, command->cmd, command->cmdLen, 1)) {
        EXIDeselect(chan);
        EXIUnlock(chan);
        return -3;
    }

    if (card->cmd[0] == 0x52 &&
        !EXIImmEx(chan, (u8*) card->workArea + sizeof(CARDID),
                     *(u32*) ((u8*) command + 0x14), 1))
    {
        EXIDeselect(chan);
        EXIUnlock(chan);
        return -3;
    }

    if ((u32) command->field_A4 == (u32) -1) {
        EXIDeselect(chan);
        EXIUnlock(chan);
        return 0;
    }

    if (!EXIDma(chan, card->buffer,
                command->cmd[0] == 0x52 ? 0x200 : 0x80,
                command->field_A4, (EXICallback) __CARDTxHandler))
    {
        EXIDeselect(chan);
        EXIUnlock(chan);
        return -3;
    }

    return 0;
}

void UnlockedCallback(s32 chan, s32 result) {
    CARDCallback callback;
    CARDControl* card;

    card = &lbl_803FC620[chan];
    if (result >= 0) {
        card->unlockCallback = UnlockedCallback;
        if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
            result = 0;
        } else {
            card->unlockCallback = NULL;
            result = Retry(chan);
        }
    }

    if (result < 0) {
        switch (card->cmd[0]) {
        case 0x52:
            callback = card->txCallback;
            if (callback != NULL) {
                card->txCallback = NULL;
                callback(chan, result);
            }
            break;
        case 0xF2:
        case 0xF4:
        case 0xF1:
            callback = card->callback_CC;
            if (callback != NULL) {
                card->callback_CC = NULL;
                callback(chan, result);
            }
            break;
        }
    }
}

s32 __CARDStart(s32 chan, void* txCallback, void* exiCallback)
{
    BOOL enabled;
    CARDControl* card;
    s32 result;

    enabled = OSDisableInterrupts();
    card = &lbl_803FC620[chan];
    if (!card->attached) {
        result = -3;
    } else {
        if (txCallback != NULL) {
            card->txCallback = (CARDCallback) txCallback;
        }
        if (exiCallback != NULL) {
            card->callback_CC = (CARDCallback) exiCallback;
        }

        card->unlockCallback = UnlockedCallback;
        if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
            result = -1;
        } else {
            card->unlockCallback = NULL;
            if (!EXISelect(chan, 0, 4)) {
                EXIUnlock(chan);
                result = -3;
            } else {
                SetupTimeoutAlarm(card);
                result = 0;
            }
        }
    }

    OSRestoreInterrupts(enabled);
    return result;
}

s32 __CARDReadSegment(s32 chan, CARDCallback callback)
{
    CARDControl* card;
    s32 result;

    card = &lbl_803FC620[chan];
    card->cmd[0] = 0x52;
    card->cmd[1] = (card->addr >> 17) & 0x7F;
    card->cmd[2] = (card->addr >> 9) & 0xFF;
    card->cmd[3] = (card->addr >> 7) & 3;
    card->cmd[4] = card->addr & 0x7F;
    card->cmdLen = 5;
    card->field_A4 = 0;
    card->field_A8 = 0;

    result = __CARDStart(chan, callback, NULL);
    if (result == -1) {
        result = 0;
    } else if (result >= 0) {
        if (!EXIImmEx(chan, card->cmd, card->cmdLen, 1) ||
            !EXIImmEx(chan, (u8*) card->workArea + sizeof(CARDID),
                          *(u32*) ((u8*) card + 0x14), 1) ||
            !EXIDma(chan, card->buffer, 0x200, card->field_A4,
                    (EXICallback)__CARDTxHandler))
        {
            card->txCallback = NULL;
            EXIDeselect(chan);
            EXIUnlock(chan);
            result = -3;
        } else {
            result = 0;
        }
    }
    return result;
}

s32 __CARDWritePage(s32 chan, CARDCallback callback) {
    CARDControl* card;
    s32 result;

    card = &lbl_803FC620[chan];
    card->cmd[0] = 0xF2;
    card->cmd[1] = (card->addr >> 17) & 0x7F;
    card->cmd[2] = (card->addr >> 9) & 0xFF;
    card->cmd[3] = (card->addr >> 7) & 3;
    card->cmd[4] = card->addr & 0x7F;
    card->cmdLen = 5;
    card->field_A4 = 1;
    card->field_A8 = 3;

    result = __CARDStart(chan, NULL, callback);
    if (result == -1) {
        result = 0;
    } else if (result >= 0) {
        if (!EXIImmEx(chan, card->cmd, card->cmdLen, 1) ||
            !EXIDma(chan, card->buffer, 0x80, card->field_A4,
                    (EXICallback)__CARDTxHandler)) {
            card->callback_CC = NULL;
            EXIDeselect(chan);
            EXIUnlock(chan);
            result = -3;
        } else {
            result = 0;
        }
    }
    return result;
}

s32 __CARDEraseSector(s32 chan, u32 addr, void* callback) {
    CARDControl* card = &lbl_803FC620[chan];
    s32 result;

    card->cmd[0] = 0xf1;
    card->cmd[1] = (addr >> 17) & 0x7f;
    card->cmd[2] = (addr >> 9) & 0xff;
    card->cmdLen = 3;
    card->field_A4 = -1;
    card->field_A8 = 3;
    result = __CARDStart(chan, NULL, callback);
    if (result == -1) {
        result = 0;
    } else if (result >= 0) {
        if (EXIImmEx(chan, card->cmd, card->cmdLen, 1) == 0) {
            card->callback_CC = NULL;
            result = -3;
        } else {
            result = 0;
        }
        EXIDeselect(chan);
        EXIUnlock(chan);
    }
    return result;
}

/* XD's CARDInit, src/dolphin/card/CARDBios.c:
 * https://github.com/TeamOrre/xd-decomp@4989794e6c6430684e033bc56f4bb97c9a921e73 */
void CARDInit(void) {
    int chan;

    if (lbl_803FC620[0].diskId && lbl_803FC620[1].diskId) {
        return;
    }

    lbl_8047A970 = OSGetFontEncode();

    OSRegisterVersion(lbl_80478A48);

    DSPInit();
    OSInitAlarm();

    for (chan = 0; chan < 2; ++chan) {
        CARDControl* card = &lbl_803FC620[chan];

        card->result = -3;
        OSInitThreadQueue((OSThreadQueue*) card->_08C);
        OSCreateAlarm(&card->alarm);
    }
    __CARDSetDiskID((void*)0x80000000);

    OSRegisterResetFunction(lbl_803127F0);
}
