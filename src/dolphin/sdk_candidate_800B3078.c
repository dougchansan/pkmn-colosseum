/**
 * CARDMount.c: CARDProbeEx, DoMount, __CARDMountCallback and
 * CARDMountAsync, 0x800B3078 - 0x800B38DC. Types and shared declarations
 * come from sdk_range_800AE3F0.c with its function bodies switched off.
 */
#define SDK_RANGE_EXACT_ACTIVE
#include "src/dolphin/sdk_range_800AE3F0.c"
extern void __CARDExtHandler(s32 chan);

s32 CARDProbeEx(s32 chan, s32* memSize, s32* sectorSize)
{
    extern u32 EXIGetState(s32 chan);
    CARDControl* card;
    BOOL enabled;
    s32 result;
    s32 probe;
    u32 id;

    if (chan < 0 || chan >= 2) {
        return -128;
    }
    if (*(volatile u8*) 0x800030E3 & 0x80) {
        return -3;
    }

    card = &lbl_803FC620[chan];
    enabled = OSDisableInterrupts();
    probe = EXIProbeEx(chan);
    if (probe == -1) {
        result = -3;
    } else if (probe == 0) {
        result = -1;
    } else if (card->attached) {
        if (card->field_24 < 1) {
            result = -1;
        } else {
            if (memSize != NULL) {
                *memSize = card->size;
            }
            if (sectorSize != NULL) {
                *sectorSize = card->sectorSize;
            }
            result = 0;
        }
    } else if (EXIGetState(chan) & 8) {
        result = -2;
    } else if (!EXIGetID(chan, 0, &id)) {
        result = -1;
    } else if (IsCard(id)) {
        if (memSize != NULL) {
            *memSize = id & 0xFC;
        }
        if (sectorSize != NULL) {
            *sectorSize = lbl_80312960[(id & 0x3800) >> 11];
        }
        result = 0;
    } else {
        result = -2;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

/* DoMount */
s32 DoMount(s32 chan)
{
    extern s32 lbl_80312960[8];
    extern u32 lbl_80312980[8];
    extern u16 lbl_80478A58;
    extern s32 EXIGetID(s32, s32, u32*);
    extern s32 IsCard(u32);
    extern s32 __CARDClearStatus(s32);
    extern s32 __CARDReadStatus(s32, u8*);
    extern s32 EXIProbe(s32);
    extern s32 __CARDUnlock(s32, u8*);
    extern s32 __CARDEnableInterrupt(s32, s32);
    extern void __CARDExiHandler(void);
    extern void EXISetExiCallback(s32, void*);
    extern void EXIUnlock(s32);
    extern s32 __CARDRead(s32, u32, u32, void*, CARDCallback);
    extern void __CARDMountCallback(s32, s32);
    extern void DoUnmount(s32, s32);
    CARDMountControl* card =
        (CARDMountControl*)&lbl_803FC620[chan];
    u32 id;
    u8 status;
    s32 result;
    OSSramEx* sram;
    int i;
    u8 checksum;
    int step;

    if (card->mountStep == 0) {
        if (EXIGetID(chan, 0, &id) == 0) {
            result = -3;
        } else if (IsCard(id)) {
            result = 0;
        } else {
            result = -2;
        }
        if (result < 0) {
            goto error;
        }

        card->cid = id;
        card->size = (u16)(id & 0xFC);
        card->sectorSize = lbl_80312960[(id & 0x3800) >> 11];
        card->cBlock = (u16)((card->size * 1024 * 1024 / 8) / card->sectorSize);
        card->latency = lbl_80312980[(id & 0x700) >> 8];

        result = __CARDClearStatus(chan);
        if (result < 0) {
            goto error;
        }
        result = __CARDReadStatus(chan, &status);
        if (result < 0) {
            goto error;
        }
        if (!EXIProbe(chan)) {
            result = -3;
            goto error;
        }

        if (!(status & 0x40)) {
            result = __CARDUnlock(chan, card->id);
            if (result < 0) {
                goto error;
            }
            checksum = 0;
            sram = __OSLockSramEx();
            for (i = 0; i < 12; i++) {
                sram->flashID[chan][i] = card->id[i];
                checksum += card->id[i];
            }
            sram->flashIDCheckSum[chan] = (u8)~checksum;
            __OSUnlockSramEx(TRUE);
            return result;
        } else {
            card->mountStep = 1;
            checksum = 0;
            sram = __OSLockSramEx();
            for (i = 0; i < 12; i++) {
                checksum += sram->flashID[chan][i];
            }
            __OSUnlockSramEx(FALSE);
            if (sram->flashIDCheckSum[chan] != (u8)~checksum) {
                result = -5;
                goto error;
            }
        }
    }

    if (card->mountStep == 1) {
        if (card->cid == 0x80000004) {
            u16 vendor;

            sram = __OSLockSramEx();
            vendor = *(u16*)sram->flashID[chan];
            __OSUnlockSramEx(FALSE);
            if (lbl_80478A58 == 0xFFFF || vendor != lbl_80478A58) {
                result = -2;
                goto error;
            }
        }
        card->mountStep = 2;
        result = __CARDEnableInterrupt(chan, TRUE);
        if (result < 0) {
            goto error;
        }
        EXISetExiCallback(chan, __CARDExiHandler);
        EXIUnlock(chan);
        DCInvalidateRange(card->workArea, 0xA000);
    }

    step = card->mountStep - 2;
    result = __CARDRead(chan, (u32)card->sectorSize * step, 0x2000,
                        (u8*)card->workArea + step * 0x2000,
                        __CARDMountCallback);
    if (result < 0) {
        __CARDPutControlBlock((CARDControl*)card, result);
    }
    return result;

error:
    EXIUnlock(chan);
    DoUnmount(chan, result);
    return result;
}

void __CARDMountCallback(s32 chan, s32 result)
{
    CARDControl* card;
    CARDCallback callback;

    card = &lbl_803FC620[chan];
    switch (result) {
    case 0:
        if (++card->field_24 < 7) {
            result = DoMount(chan);
            if (result >= 0) {
                return;
            }
        } else {
            result = __CARDVerify(card);
        }
        break;
    case 1:
        card->unlockCallback = __CARDMountCallback;
        if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
            return;
        }
        card->unlockCallback = NULL;
        result = DoMount(chan);
        if (result >= 0) {
            return;
        }
        break;
    case -5:
    case -3:
        DoUnmount(chan, result);
        break;
    }

    callback = card->apiCallback;
    card->apiCallback = NULL;
    __CARDPutControlBlock(card, result);
    callback(chan, result);
}

s32 CARDMountAsync(s32 chan, void* workArea, CARDCallback detachCallback,
                   CARDCallback attachCallback)
{
    extern u32 EXIGetState(s32 chan);
    extern BOOL EXIAttach(s32 chan, EXICallback callback); /* EXIAttach */
    CARDControl* card;
    BOOL enabled;

    if (chan < 0 || chan >= 2) {
        return -128;
    }
    if (*(volatile u8*) 0x800030E3 & 0x80) {
        return -3;
    }

    card = &lbl_803FC620[chan];
    enabled = OSDisableInterrupts();
    if (card->result == -1) {
        OSRestoreInterrupts(enabled);
        return -1;
    }

    if (!card->attached && (EXIGetState(chan) & 8)) {
        OSRestoreInterrupts(enabled);
        return -2;
    }

    card->result = -1;
    card->workArea = workArea;
    card->extCallback = detachCallback;
    card->apiCallback =
        attachCallback != NULL ? attachCallback : __CARDDefaultApiCallback;
    card->callback_CC = NULL;

    if (!card->attached &&
        !EXIAttach(chan, (EXICallback) __CARDExtHandler))
    {
        card->result = -3;
        OSRestoreInterrupts(enabled);
        return -3;
    }

    card->field_24 = 0;
    card->attached = TRUE;
    EXISetExiCallback(chan, NULL);
    OSCancelAlarm(&card->alarm);
    card->dirBlock = NULL;
    card->fatBlock = NULL;
    OSRestoreInterrupts(enabled);

    card->unlockCallback = __CARDMountCallback;
    if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
        return 0;
    }

    card->unlockCallback = NULL;
    return DoMount(chan);
}
