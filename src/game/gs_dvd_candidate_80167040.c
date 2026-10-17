/**
 * @file gs_dvd_candidate_80167040.c
 * @brief MusyX sound-runtime work pools and DVD error task, 0x80167040 - 0x80167E54.
 *
 * Standalone owner for the linked .text range, with its functions in retail
 * address order. The same bodies remain in gs_dvd.c for the neighbouring
 * candidate score units that still include it. The two switch jump tables
 * (_gsdvdErrorTask, _errorTask_State_None) are this
 * unit's .data at 0x8036BF20 - 0x8036BF80.
 */
#include "dolphin/types.h"

typedef struct GSsndWork {
    u8 flags;
    u8 priority;
    u8 unk2;
    u8 unk3;
    u8 stackDepth;
    u8 volumeStack[3];
    u32 handle;
    u32 unkC;
    u32 unk10;
} GSsndWork;

typedef struct GSsndEntry {
    u8 flags;
    u8 unk1;
    u8 reverb;
    u8 waveIndex;
    u16 waveId;
    u16 unk6;
    GSsndWork* work;
} GSsndEntry;

typedef struct GSsndFlagBits {
    u8 isSe : 1;
    u8 unk6 : 1;
    u8 active : 1;
    u8 paused : 1;
    u8 unk0_3 : 4;
} GSsndFlagBits;

typedef struct GSsndReverbState {
    u8 pad0[0x1C4];
    u8 enabled;
    u8 pad1[3];
    f32 coloration;
    f32 mix;
    f32 time;
    f32 damping;
    f32 preDelay;
    f32 crosstalk;
} GSsndReverbState;

typedef struct GSDVDWork {
    u8 active;
    u8 started;
    u8 drawing;
    u8 _pad03;
    u8 fileInfo[0x3C];
    void (*callback)(s32 result, struct GSDVDWork* work);
} GSDVDWork;

typedef struct GSsndOpenState {
    u8 _pad00;
    u8 active;
    u16 soundId;
    u8 _pad04[0x14];
} GSsndOpenState;

typedef struct GSsndStartParams {
    u32 unk00;
    u32 unk04;
    u32 unk08;
    u16 unk0C;
    u16 value;
    u8 volume;
    u8 pad11;
    u8 unk12;
    u8 pad13;
    u32 unk14;
    u8 unk18;
    u8 pad19[3];
    u32 unk1C;
} GSsndStartParams;

extern void GSlogWrite(const char* fmt, ...);
extern void* GSresGetResource(void* ptr, u32 param);
extern void soundStopRaw(u32 a, u32 b, u32 c);
extern void GSsndGetStatus(u32 index);
extern u32 soundAttachWork(u32 id, u32 arg1, u32 arg2);
extern void soundApplyReverb(u32 reverb);
extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL level);
extern u32 fn_800E2C04(u32 size, u32 align);
extern void* fn_800E27B0(u32 handle);
extern u32 fn_800E202C(void);
extern void fn_800E24B0(u32 handle);
extern void fn_800E209C(u32 handle);
extern s32 DVDGetCommandBlockStatus(const void* block);
extern s32 DVDGetDriveStatus(void);
extern BOOL DVDSeekAsyncPrio(void* fileInfo, s32 offset,
                             void (*callback)(s32 result, void* fileInfo),
                             s32 priority);
extern u32 ARGetDMAStatus(void);
extern u32 ARQGetChunkSize(void);
extern u32 sndFXCtrl(u32 voice, u8 control, u8 value);
extern u32 sndFXKeyOff(u32 voice);
extern u32 sndFXCheck(u32 voice);
extern u32 sndFXStartEx(u16 id, u8 volume, u8 pan, u8 studio);
extern void sndSeqVolume(u8 group, u16 volume, u32 sequence, u8 fade);
extern u32 fn_8015A368(u8 group, u16 id, void* resource, void* params,
                       u32 flags);
extern void fn_80159ED0(u8* callback, u32 chunkSize);
extern u8 fn_80159EF0(void* resource, u16 soundId, void* sampleData,
                      void* resource2, void* resource3);
extern void winMsgOpenError(u32 message, u32 mode, u32 wait);
extern void winMsgCloseError(u32 wait);
extern void fn_800056D4(void);

extern void _AsyncCallback(s32 result, void* fileInfo);
extern void fn_80167E64(u8* file);
extern GSDVDWork* fn_80167F28(const char* path);
extern s32 fn_80167ED0(GSDVDWork* work, void* addr, s32 length, s32 offset);

extern const char lbl_80273748[]; /* "_sndCheckSndWorkALL:Start" */
extern const char lbl_80273764[]; /* "_sndCheckSndWorkALL:End" */
extern const char lbl_80273780[]; /* "[GSDVD_ERROR_STATE_COVEROPEN_WAIT]..." */

extern GSsndEntry* lbl_80478FAC;
extern GSsndOpenState* lbl_80478FB4;
extern f32* lbl_80478FA4;
extern GSsndReverbState lbl_80452500;
extern u32 lbl_80478C10;
extern u32 lbl_80478C14;
extern u32 lbl_80478C18;
extern u32 lbl_80478C1C;
extern u32 lbl_80478C20;
extern u32 lbl_8047B0B8;
extern GSDVDWork* lbl_8047B0BC;
extern void* lbl_8047B0C0;
extern u8* lbl_8047B0C4;
extern u32 lbl_8047B0C8;
extern u8* lbl_8047B0CC;
extern u32 lbl_8047B0D0;
extern u8* lbl_8047B0D4;
extern u32 lbl_8047B0D8;
extern u8* lbl_8047B0DC;
extern u32 lbl_8047B0E0;
extern u32 lbl_8047B0E4;
extern u32 lbl_8047B0E8;
extern s32 lbl_8047B0F0;

u32 fn_80167720(u32 handle);
u8 fn_80167070(u32 entry, u32 flag);
void _sndInitParms(GSsndEntry* entry, GSsndWork* work);
void fn_80167B70(void);
void* fn_80167BB0(u32 size);
void* _sndSetSampleDataUploadCallbackFunction(u32 offset, u32 length);
s32 fn_80167E34(void);

void fn_80167040(u32 handle)
{
    u32 entry;

    entry = fn_80167720(handle);
    if (entry != 0) {
        fn_80167070(entry, 0);
    }
}

u8 fn_80167070(u32 index, u32 stop)
{
    extern void fn_8016782C(u8* flag);
    GSsndEntry* entry = &lbl_80478FAC[index];
    GSsndWork* work;

    if (((GSsndFlagBits*)&entry->flags)->active != 1) {
        return 0;
    }
    work = entry->work;
    if (work == NULL) {
        return 0;
    }
    if ((u8)stop == 1) {
        soundStopRaw(index, 0, 0);
    }
    _sndInitParms(entry, work);
    fn_8016782C((u8*)work);
    entry->work = NULL;
    ((GSsndFlagBits*)&entry->flags)->active = 0;
    return 1;
}

u8 soundPushWave(u32 slot, u32 stream, const char* path, u32 offset,
               void* resourceOwner, u32 resourceId3, u32 resourceId1,
               u32 resourceId2)
{
    extern void fn_80167B70();
    GSsndOpenState* state = &lbl_80478FB4[slot];
    void* resource1;
    void* sampleData;
    void* resource2;
    void* resource3;

    if (state->active == 1) {
        return 0;
    }
    resource1 = GSresGetResource(resourceOwner, resourceId1);
    if (resource1 == NULL) {
        return 0;
    }
    resource2 = GSresGetResource(resourceOwner, resourceId2);
    if (resource2 == NULL) {
        return 0;
    }
    resource3 = GSresGetResource(resourceOwner, resourceId3);
    if (resource3 == NULL) {
        return 0;
    }
    if (stream == 1) {
        sampleData = NULL;
        fn_80159ED0((u8*)_sndSetSampleDataUploadCallbackFunction,
                    ARQGetChunkSize());
        lbl_8047B0BC = fn_80167F28(path);
        if (lbl_8047B0BC == NULL) {
            return 0;
        }
        lbl_8047B0B8 = offset;
        lbl_8047B0C0 = fn_80167BB0(ARQGetChunkSize());
        if (lbl_8047B0C0 == NULL) {
            fn_80167E64((u8*)lbl_8047B0BC);
            return 0;
        }
    } else {
        sampleData = (void*)path;
        fn_80159ED0(NULL, 0);
    }
    if (!fn_80159EF0(resource1, state->soundId, sampleData, resource2,
                     resource3)) {
        if (stream == 1) {
            fn_80167E64((u8*)lbl_8047B0BC);
        }
        return 0;
    }
    if (stream == 1) {
        fn_80167B70(lbl_8047B0C0);
        fn_80167E64((u8*)lbl_8047B0BC);
    }
    state->active = 1;
    return 1;
}

void* _sndSetSampleDataUploadCallbackFunction(u32 offset, u32 length)
{
    s32 result;

    while (ARGetDMAStatus() != 0) {
    }
    for (;;) {
        result = fn_80167ED0(lbl_8047B0BC, lbl_8047B0C0, length,
                             offset + lbl_8047B0B8);
        switch (result) {
        case -3:
        case -1:
            continue;
        }
        break;
    }
    return lbl_8047B0C0;
}

void _sndCheckSndWorkALL(void)
{
    int i;

    GSlogWrite(lbl_80273748);
    for (i = 0; i < lbl_8047B0E8; i++) {
        GSsndGetStatus(i);
    }
    GSlogWrite(lbl_80273764);
}

u32 fn_8016737C(GSsndEntry* entry, u32 volume, u32 limit)
{
    GSsndWork* work = entry->work;

    if (work == NULL) {
        return 0;
    }

    work->priority = limit & 0x7F;
    if (work->handle != -1) {
        if (((GSsndFlagBits*)&entry->flags)->isSe == 1) {
            sndFXCtrl(work->handle, 7, work->priority);
        } else {
            sndSeqVolume(work->priority, (u16)volume, work->handle, 0);
        }
        return 1;
    }
    return 0;
}

u8 _sndSetVolumeWork(u32 id, u32 volume)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    GSsndWork* work;

    if (((GSsndFlagBits*)&entry->flags)->active != 1) {
        if (((GSsndFlagBits*)&entry->flags)->isSe == 1) {
            soundAttachWork(id, 0, 0);
        } else {
            return 0;
        }
    }

    work = entry->work;
    if (work != NULL) {
        work->priority = volume & 0x7F;
    }
    return 1;
}

u32 _sndStopSE(GSsndEntry* entry, u32 arg1, u32 arg2)
{
    GSsndWork* work = entry->work;
    u32 result;

    if (work == NULL) {
        return 0;
    }
    if (work->handle == -1U) {
        return 0;
    }
    if (sndFXCheck(work->handle) != -1) {
        result = sndFXKeyOff(work->handle);
    } else {
        result = 0;
    }
    work->handle = -1;
    return result;
}

u32 _sndStopBGM(GSsndEntry* entry, u32 fade, u32 arg2)
{
    GSsndWork* work = entry->work;

    if (work == NULL) {
        return 0;
    }
    if (((GSsndFlagBits*)&entry->flags)->active != 1) {
        return 0;
    }
    if (work->handle == -1U) {
        return 0;
    }
    sndSeqVolume(0, (u16)fade, work->handle, 1);
    work->handle = -1;
    return 1;
}

u32 soundStartFX(GSsndEntry* entry, u32 id)
{
    GSsndWork* work;

    if (((GSsndFlagBits*)&entry->flags)->active != 1) {
        soundAttachWork(id, 0, 0);
    }
    work = entry->work;
    if (work != NULL) {
        work->handle =
            sndFXStartEx(entry->waveId, work->priority, work->unk2, 0);
        sndFXCtrl(work->handle, 7, work->priority);
        return 1;
    }
    return 0;
}

u32 fn_8016761C(GSsndEntry* entry, u16 value)
{
    GSsndStartParams params;
    void* resource;
    GSsndWork* work = entry->work;

    if (work == NULL) {
        return 0;
    }
    if (((GSsndFlagBits*)&entry->flags)->active != 1) {
        return 0;
    }
    if (work->handle != -1U) {
        return 0;
    }
    resource = (void*)GSresGetResource((void*)work->unkC, work->unk10);
    if (resource == NULL) {
        return 0;
    }
    soundApplyReverb(entry->reverb);
    params.unk00 = 4;
    params.unk04 = 0;
    params.unk08 = 0;
    params.unk0C = 0x100;
    params.value = value;
    params.volume = work->priority;
    params.unk12 = 0;
    params.unk14 = 0;
    params.unk18 = 0;
    params.unk1C = 0;
    work->handle =
        fn_8015A368(entry->unk1, entry->waveId, resource, &params, 0);
    return 1;
}

u32 fn_80167720(u32 handle)
{
    int i;

    for (i = 0; i < lbl_8047B0E8; i++) {
        GSsndWork* work = lbl_80478FAC[i].work;

        if (work != NULL && work->handle == handle) {
            return i;
        }
    }
    return 0;
}

u32 soundFindOwner(u32 unkC, u32 unk10)
{
    int i;

    for (i = 0; i < lbl_8047B0E8; i++) {
        GSsndWork* work = lbl_80478FAC[i].work;

        if (work != NULL && work->unkC == unkC && work->unk10 == unk10) {
            return i;
        }
    }
    return -1;
}

void fn_801677BC(u8* flag)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    *flag = 0;
    OSRestoreInterrupts(enabled);
}

void fn_801677F4(u8* flag)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    *flag = 0;
    OSRestoreInterrupts(enabled);
}

void fn_8016782C(u8* flag)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    *flag = 0;
    OSRestoreInterrupts(enabled);
}

u8* fn_80167864(void)
{
    BOOL enabled;
    u8* entry;
    u32 i;
    u32 offset;
    u8* result;

    enabled = OSDisableInterrupts();
    result = 0;
    for (i = 0; i < lbl_8047B0C8; i++) {
        entry = &lbl_8047B0C4[i * 0x78];
        if (*entry != 1) {
            offset = i * 0x78;
            lbl_8047B0C4[offset] = 1;
            result = &lbl_8047B0C4[offset];
            break;
        }
    }
    OSRestoreInterrupts(enabled);
    return result;
}

u8* fn_801678E4(void)
{
    BOOL enabled;
    u8* entry;
    u8* base;
    u32 i;
    u32 offset;
    u8* result;

    enabled = OSDisableInterrupts();
    entry = base = lbl_8047B0CC;
    result = 0;
    i = 0;
    while (i < lbl_8047B0D0) {
        if (*entry != 1) {
            offset = i * 0xD0;
            base[offset] = 1;
            result = &lbl_8047B0CC[offset];
            break;
        }
        entry += 0xD0;
        i++;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

u8* fn_80167964(void)
{
    BOOL enabled;
    u8* entry;
    u8* base;
    u32 i;
    u32 offset;
    u8* result;

    enabled = OSDisableInterrupts();
    entry = base = lbl_8047B0DC;
    result = 0;
    i = 0;
    while (i < lbl_8047B0E0) {
        if (*entry != 1) {
            offset = i * 0x14;
            base[offset] = 1;
            result = &lbl_8047B0DC[offset];
            break;
        }
        entry += 0x14;
        i++;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

void fn_801679E4(void)
{
    u32 i;

    for (i = 0; i < lbl_8047B0C8; i++) {
        lbl_8047B0C4[i * 0x78] = 0;
    }
}

void fn_80167A14(void)
{
    u32 i;

    for (i = 0; i < lbl_8047B0D0; i++) {
        lbl_8047B0CC[i * 0xD0] = 0;
    }
}

void _sndInitStack(void)
{
    u32 i;

    for (i = 0; i < lbl_8047B0D8; i++) {
        lbl_8047B0D4[i] = 0;
    }
}

void fn_80167A6C(void)
{
    u32 i;

    for (i = 0; i < lbl_8047B0E0; i++) {
        lbl_8047B0DC[i * 0x14] = 0;
    }
}

void _sndSetReverbParm(u32 index)
{
    f32* source = &lbl_80478FA4[index * 6];

    lbl_80452500.enabled = 0;
    lbl_80452500.coloration = source[0];
    lbl_80452500.mix = source[1];
    lbl_80452500.time = source[2];
    lbl_80452500.damping = source[3];
    lbl_80452500.preDelay = source[4];
    lbl_80452500.crosstalk = source[5];
    lbl_8047B0E4 = index;
}

void _sndInitParms(GSsndEntry* entry, GSsndWork* work)
{
    BOOL enabled = OSDisableInterrupts();
    GSsndFlagBits* flags = (GSsndFlagBits*)&entry->flags;

    flags->active = 1;
    flags->paused = 0;
    entry->work = work;
    work->handle = -1;
    work->stackDepth = 0;
    work->priority = 0x7F;
    work->unk2 = 0x40;
    work->unk3 = 0x40;
    OSRestoreInterrupts(enabled);
}

void fn_80167B70(void)
{
    u32 handle = fn_800E202C();

    if ((u16)handle != 0) {
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }
}

void* fn_80167BB0(u32 size)
{
    u32 handle = fn_800E2C04(size, 0x20);

    if ((u16)handle != 0) {
        return fn_800E27B0(handle);
    }
    return 0;
}

void _gsdvdErrorTask(void)
{
    extern void _errorTask_State_None(s32 state);
    extern void _gsdvdError_MsgOpen(u32 message);
    s32 driveState = fn_80167E34();

    switch (lbl_8047B0F0) {
    case 0:
        _errorTask_State_None(driveState);
        break;
    case 1:
        _gsdvdError_MsgOpen(lbl_80478C10);
        lbl_8047B0F0 = 2;
        break;
    case 2:
        GSlogWrite(lbl_80273780, driveState);
        if (driveState != 5 && driveState != 2) {
            winMsgCloseError(1);
            lbl_8047B0F0 = 0;
        }
        break;
    case 3:
        _gsdvdError_MsgOpen(lbl_80478C14);
        lbl_8047B0F0 = 4;
        break;
    case 4:
        if (driveState != 4) {
            winMsgCloseError(1);
            lbl_8047B0F0 = 0;
        }
        break;
    case 5:
        _gsdvdError_MsgOpen(lbl_80478C18);
        lbl_8047B0F0 = 6;
        break;
    case 6:
        if (driveState != 6) {
            winMsgCloseError(1);
            lbl_8047B0F0 = 0;
        }
        break;
    case 7:
        _gsdvdError_MsgOpen(lbl_80478C1C);
        lbl_8047B0F0 = 8;
        break;
    case 8:
        if (driveState != 11) {
            winMsgCloseError(1);
            lbl_8047B0F0 = 0;
        }
        break;
    case 9:
        fn_800056D4();
        _gsdvdError_MsgOpen(lbl_80478C20);
        lbl_8047B0F0 = 10;
        break;
    case 10:
        /* Fatal-error message is up; nothing further to do. */
        break;
    }
}

void _gsdvdError_MsgOpen(u32 message)
{
    if (message != 1) {
        winMsgOpenError(message, 1, 1);
    }
}

void _errorTask_State_None(s32 state)
{
    switch (state) {
    case -1:
        lbl_8047B0F0 = 9;
        break;
    case 5:
        lbl_8047B0F0 = 1;
        break;
    case 4:
        lbl_8047B0F0 = 3;
        break;
    case 6:
        lbl_8047B0F0 = 5;
        break;
    case 11:
        lbl_8047B0F0 = 7;
        break;
    }
}

void fn_80167DC0(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4)
{
    lbl_80478C10 = arg0;
    lbl_80478C14 = arg1;
    lbl_80478C18 = arg2;
    lbl_80478C1C = arg3;
    lbl_80478C20 = arg4;
}

u8 fn_80167DD8(GSDVDWork* work, s32 offset,
               void (*callback)(s32 result, GSDVDWork* work))
{
    work->callback = callback;
    return DVDSeekAsyncPrio(work->fileInfo, offset, _AsyncCallback, 2);
}

s32 fn_80167E10(u8* handle)
{
    return DVDGetCommandBlockStatus(handle + 4);
}

s32 fn_80167E34(void)
{
    return DVDGetDriveStatus();
}
