/**
 * @file sdk_range_800AF8A0.c
 * @brief Candidate Dolphin SDK range, 0x800AF8A0 - 0x800B1788.
 */

#include "src/dolphin/sdk_range_800AE3F0.c"

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

s32 fn_800AF8A0(s32 chan)
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
    if (!fn_80098368(chan, command->cmd, command->cmdLen, 1)) {
        EXIDeselect(chan);
        EXIUnlock(chan);
        return -3;
    }

    if (card->cmd[0] == 0x52 &&
        !fn_80098368(chan, (u8*) card->workArea + sizeof(CARDID),
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

s32 fn_800AFBDC(s32 chan, void* txCallback, void* exiCallback)
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

    result = fn_800AFBDC(chan, callback, NULL);
    if (result == -1) {
        result = 0;
    } else if (result >= 0) {
        if (!fn_80098368(chan, card->cmd, card->cmdLen, 1) ||
            !fn_80098368(chan, (u8*) card->workArea + sizeof(CARDID),
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
