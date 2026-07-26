/**
 * @file gs_dvd.c
 * @brief DVD-adjacent MusyX sound-runtime unit (per symbols.txt this is
 * mostly _snd-prefixed MusyX audio code, NOT a "GSDVD" disc-I/O API -
 * see 2026-07-02 note below).
 *
 * Per config/GC6E01/splits.txt this unit's real range is
 * 0x80167040 - 0x80168C64. Functions with real (symbols.txt) names -
 * e.g. _sndSetSampleDataUploadCallbackFunction, _sndCheckSndWorkALL,
 * _sndSetVolumeWork, _sndStopSE, _sndStopBGM, _sndInitStack,
 * _sndSetReverbParm, _sndInitParms, _gsdvdErrorTask_801879AC,
 * _gsdvdError_MsgOpen, GSfilterCreate - confirm this is MusyX audio
 * runtime, matching the "people_field" precedent from a prior
 * campaign note (port from reference, don't brute-force). The many
 * remaining fn_XXXXXXXX gaps are genuinely un-decompiled and still
 * need real work; treat any GSDVD_* naming still present below with
 * suspicion, it has not been verified this pass.
 *
 * 2026-07-02 reconciliation: removed 7 orphan definitions whose names
 * are not present in symbols.txt and never paired in objdiff -
 * GSDVD_CheckAndClose, GSDVD_CloseHandle, GSDVD_Open, GSDVD_EmptyFunc,
 * GSDVD_ErrorStateMachine, GSDVD_ErrorCoverOpenMain, GSDVD_Init - along
 * with their unused GSDVDHandle/GSDVDEntry typedefs. Their bodies were
 * invented fiction (fake register-variable pseudo-assembly, e.g.
 * GSDVD_Init's body was actually a free-list/allocator coalescing
 * routine, unrelated to "DVD init"). This pass did not otherwise
 * change any other function or the rest of this file's speculative
 * GSDVD_* naming table below, which remains unverified.
 *
 * 2026-07-03: re-tried porting the archive bodies for fn_80167040
 * ("GSDVD_CheckAndClose", 48 bytes) and fn_80167070
 * ("GSDVD_CloseHandle", 168 bytes) under their correct current
 * fn_ names -- both compile but land at 66.7% / 76.2% fuzzy match
 * (not 100%), confirming the archive content for this file is
 * unreliable register-level fiction, not real disassembly. Reverted
 * both, no net change. The only new function landed this pass is
 * fn_80167FA4 (trivial 4-byte empty function / blr-only stub),
 * confirmed 100% via objdiff.
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

#define SND_FLAG_SE 0x80
#define SND_FLAG_ACTIVE 0x20
#define SND_FLAG_PAUSED 0x10

/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const char* fmt, ...);         /* OSReport / GSlog */
extern void* GSresGetResource(void* ptr, u32 param);        /* resource resolution */
extern void  fn_801669E4(u32 a, u32 b, u32 c);         /* sound subsystem */
extern void  fn_8016A644(void* ptr);                    /* resource cleanup */
extern void  fn_80169520(void* ptr);                    /* status flag update */

/* ===== String constants (rodata) ===== */
extern const char lbl_80273748[]; /* "_sndCheckSndWorkALL:Start" */
extern const char lbl_80273764[]; /* "_sndCheckSndWorkALL:End" */
extern const char lbl_80273780[]; /* "[GSDVD_ERROR_STATE_COVEROPEN_WAIT]..." */

/* ===== BSS / global state ===== */
extern GSsndEntry* lbl_80478FAC;

/* Forward declarations for converted functions */
s32 fn_80167E54(void);
u32 fn_80167E5C(u8* obj);
void fn_80167FA4(void);


/*
 * 2026-07-02 reconciliation: removed the fictional GSDVDHandle /
 * GSDVDEntry typedefs and the fictional bodies of GSDVD_CheckAndClose,
 * GSDVD_CloseHandle, GSDVD_Open, GSDVD_EmptyFunc and
 * GSDVD_ErrorStateMachine (orphans - none of these names are present in
 * symbols.txt, none paired in objdiff, and neither typedef was even
 * referenced by anything else in this file). See the file header for
 * the honest picture of what this unit actually is.
 */

/* ==================================================================
 * fn_80168284 -- _sndCheckSndWorkALL
 *
 * Validate all sound work buffers. Logs start/end markers.
 * 388 bytes.
 *
 * From disassembly references:
 *   lbl_80273748: "_sndCheckSndWorkALL:Start"
 *   lbl_80273764: "_sndCheckSndWorkALL:End"
 * ================================================================== */
extern u32 lbl_8047B0E8;
extern void fn_801666BC(u32 index);

void _sndCheckSndWorkALL(void)
{
    u32 i;

    GSlogWrite(lbl_80273748);
    for (i = 0; i < lbl_8047B0E8; i++) {
        fn_801666BC(i);
    }
    GSlogWrite(lbl_80273764);
}

/*
 * 2026-07-02 reconciliation: removed the fictional bodies of
 * GSDVD_ErrorCoverOpenMain and GSDVD_Init (orphans - neither name is
 * present in symbols.txt, neither paired in objdiff). Both were
 * unreferenced anywhere else in-tree. GSDVD_Init's real target address
 * (0x80168934, per its own header comment) is symbols.txt's
 * "particleSort" (size matches exactly, 0x330 bytes) - an unrelated
 * name that doesn't belong to a DVD/sound module; renaming to it was
 * not attempted since the removed body was generic linked-list/
 * allocator code with no real connection to that function's semantics.
 */

/* ==================================================================
 * fn_80167FA4 -- ported from archive (verified 100% via objdiff).
 * Archive labeled this "GSDVD_EmptyFunc" but that name is not in
 * symbols.txt (this unit is actually MusyX audio, see file header);
 * kept the fn_ name. 4-byte empty function (just blr).
 * ================================================================== */
void fn_80167FA4(void) {
    /* intentionally empty */
}

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 2 functions matched
 * =================================================================== */

/* Address: 0x80167E54 | Size: 0x8 | Pattern: return_constant */
s32 fn_80167E54(void) { return -1; }

/* Address: 0x80167E5C | Size: 0x8 | Pattern: simple_getter */
u32 fn_80167E5C(u8* obj) {
    return *(u32*)((u8*)obj + 0x38);
}

/* 2026-07-24: decompiled directly from the current disassembly (not the
 * unreliable archive bodies noted above). */
extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL level);
extern u32 fn_80167720(u32 handle);
extern u8 fn_80167070(u32 entry, u32 flag);
extern u8 *lbl_8047B0C4;
extern u32 lbl_8047B0C8;
extern u8 *lbl_8047B0CC;
extern u32 lbl_8047B0D0;
extern u8 *lbl_8047B0DC;
extern u32 lbl_8047B0E0;
extern void winMsgOpenError(u32 message, u32 mode, u32 wait);

void fn_80167040(u32 handle)
{
    u32 entry;

    entry = fn_80167720(handle);
    if (entry != 0) {
        fn_80167070(entry, 0);
    }
}

void fn_801677BC(u8 *flag)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    *flag = 0;
    OSRestoreInterrupts(enabled);
}

void fn_801677F4(u8 *flag)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    *flag = 0;
    OSRestoreInterrupts(enabled);
}

void fn_8016782C(u8 *flag)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    *flag = 0;
    OSRestoreInterrupts(enabled);
}

u8 *fn_80167864(void)
{
    BOOL enabled;
    u32 i;
    u32 offset;
    u8 *entry;
    u8 *result;

    enabled = OSDisableInterrupts();
    entry = lbl_8047B0C4;
    result = 0;
    for (i = 0; i < lbl_8047B0C8; i++, entry += 0x78) {
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

u8 *fn_801678E4(void)
{
    BOOL enabled;
    u32 i;
    u32 offset;
    u8 *entry;
    u8 *result;

    enabled = OSDisableInterrupts();
    entry = lbl_8047B0CC;
    result = 0;
    for (i = 0; i < lbl_8047B0D0; i++, entry += 0xD0) {
        if (*entry != 1) {
            offset = i * 0xD0;
            lbl_8047B0CC[offset] = 1;
            result = &lbl_8047B0CC[offset];
            break;
        }
    }
    OSRestoreInterrupts(enabled);
    return result;
}

u8 *fn_80167964(void)
{
    BOOL enabled;
    u32 i;
    u32 offset;
    u8 *entry;
    u8 *result;

    enabled = OSDisableInterrupts();
    entry = lbl_8047B0DC;
    result = 0;
    for (i = 0; i < lbl_8047B0E0; i++, entry += 0x14) {
        if (*entry != 1) {
            offset = i * 0x14;
            lbl_8047B0DC[offset] = 1;
            result = &lbl_8047B0DC[offset];
            break;
        }
    }
    OSRestoreInterrupts(enabled);
    return result;
}

void _gsdvdError_MsgOpen(u32 message)
{
    if (message != 1) {
        winMsgOpenError(message, 1, 1);
    }
}

void fn_801679E4(void)
{
    u32 offset;
    u32 i;
    u8 value;

    offset = 0;
    i = 0;
    value = 0;
    while (i < lbl_8047B0C8) {
        lbl_8047B0C4[offset] = value;
        i++;
        offset += 0x78;
    }
}

void fn_80167A14(void)
{
    u32 offset;
    u32 i;
    u8 value;

    offset = 0;
    i = 0;
    value = 0;
    while (i < lbl_8047B0D0) {
        lbl_8047B0CC[offset] = value;
        i++;
        offset += 0xD0;
    }
}


extern u32 fn_800E2C04(u32 size, u32 align);
extern void* fn_800E27B0(u32 handle);
extern u32 fn_800E202C(void);
extern void fn_800E24B0(u32 handle);
extern void fn_800E209C(u32 handle);
extern s32 DVDConvertPathToEntrynum(const char* path);
extern s32 DVDGetCommandBlockStatus(const void* block);
extern s32 DVDGetDriveStatus(void);
extern void DVDClose(void* fileInfo);
extern s32 DVDRead(void* fileInfo, void* addr, s32 length, s32 offset,
                   s32 priority);
extern BOOL DVDReadAsync(void* fileInfo, void* addr, s32 length, s32 offset,
                         void (*callback)(s32 result, void* fileInfo),
                         s32 priority);
void fn_80168164(u8* flag);

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

typedef struct GSFilter {
    u8 index;
    u8 active;
    u8 drawing;
    u8 _pad03;
} GSFilter;

typedef struct GSFilterState {
    GSFilter* filters;
    u8* colors;
    u16 viewport[8];
    u8 capacity;
    u8 count;
    u8 drawingCount;
    u8 _pad1B;
    u16 filterHandle;
    u16 colorHandle;
    u32 renderState;
} GSFilterState;

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

extern GSDVDWork* lbl_8047B0F4;
extern u32 lbl_8047B0F8;
extern GSFilterState lbl_804526E0;
extern void GXDrawDone(void);
extern void fn_800B856C(void);
extern u32 sndFXCtrl(u32 voice, u8 control, u8 value);
extern u32 sndFXKeyOff(u32 voice);
extern void sndSeqVolume(u8 group, u16 volume, u32 sequence, u8 fade);
extern u32 sndFXCheck(u32 voice);
extern u32 fn_80166B3C(u32 id, u32 arg1, u32 arg2);
extern BOOL DVDSeekAsyncPrio(void* fileInfo, s32 offset,
                            void (*callback)(s32 result, void* fileInfo),
                            s32 priority);

extern u8* lbl_8047B0D4;
extern u32 lbl_8047B0D8;
extern u8* lbl_8047B0DC;
extern u32 lbl_8047B0E0;
extern u32 lbl_8047B0E4;
extern u32 lbl_8047B0E8;
extern f32* lbl_80478FA4;
extern GSsndReverbState lbl_80452500;
extern u32 lbl_80478C10;
extern u32 lbl_80478C14;
extern u32 lbl_80478C18;
extern u32 lbl_80478C1C;
extern u32 lbl_80478C20;
extern const u8 lbl_8047D578[];
extern const u8 lbl_8047D594[];
extern const u8* lbl_80478C24;
extern const u8* lbl_80478C28;
extern u32 lbl_8047B0B8;
extern GSDVDWork* lbl_8047B0BC;
extern void* lbl_8047B0C0;
extern GSsndOpenState* lbl_80478FB4;
extern const char lbl_8047D584[];
extern const char lbl_8047D58C[];
extern u32 ARGetDMAStatus(void);
extern void __assert(const char* file, u32 line, const char* condition);
extern BOOL DVDOpen(const char* path, void* fileInfo);
extern u32 sndFXStartEx(u16 id, u8 volume, u8 pan, u8 studio);
extern void fn_80166C34(u32 reverb);
extern u32 fn_8015A368(u8 group, u16 id, void* resource, void* params,
                      u32 flags);
extern void DCFlushRange(void* address, u32 size);
extern void DVDInit(void);
extern BOOL DVDSetAutoFatalMessaging(BOOL enable);
extern s32 fn_800057A0(void);
extern s32 fn_800057A8(void);
extern u8* fn_800A7BCC(void);
extern u32 GSgappCreate(s32 state, u8 priority, u32 parameter,
                       void (*callback)(void));
extern void winMsgCloseError(u32 wait);
extern void fn_800056D4(void);
extern u32 ARQGetChunkSize(void);
extern void fn_80159ED0(u8* callback, u32 chunkSize);
extern u8 fn_80159EF0(void* resource, u16 soundId, void* sampleData,
                     void* resource2, void* resource3);
extern const f32 lbl_8047D5A0;
extern const f32 lbl_8047D5A4;
extern const f32 lbl_8047D5A8;
extern void fn_800D88DC(u32 enable);
extern void fn_800D888C(u32 value);
extern void fn_800D9B58(f32 red, f32 green, f32 blue, f32 alpha);
extern void fn_800DA4C4(u32, u32, u32);
extern void fn_800DA2BC(u32, u32, u32);
extern void fn_800DA100(u32, u32, u32, u32, u32, u32);
extern void fn_800DA1E8(u32, u32, u32);
extern void fn_800D9ED8(u32);
extern void fn_800DA028(u32);
extern void fn_800D7820(u32);
extern void fn_800D6A00(u32);
extern void fn_800D67BC(u32);
extern void fn_800D5FA4(u32);
extern void fn_800D5A38(u32, u32);
extern void fn_800D6728(void);
extern u16 _toolentryAlloc__FUl(u32 size);
extern void* fn_800D7894(void);
extern void fn_800D7868(void*, u32, u32, u32, u32, u8, void*, u8);
extern void fn_800D75F4(void*);
extern void* memset(void*, int, u32);
extern GSFilter* lbl_8047B100;

void _sndInitParms(GSsndEntry* entry, GSsndWork* work);
s32 fn_80167ED0(GSDVDWork* work, void* addr, s32 length, s32 offset);
GSDVDWork* fn_8016819C(void);
void fn_8016821C(void);
void* fn_8016824C(u32 size);
void _gsdvdErrorTask_801879AC(void);
void fn_80167B70(void);
void* fn_80167BB0(u32 size);
void fn_80167E64(u8* file);
GSDVDWork* fn_80167F28(const char* path);
void* _sndSetSampleDataUploadCallbackFunction(u32 offset, u32 length);

GSDVDWork* _info2work(void* fileInfo);

void _AsyncCallback(s32 result, void* fileInfo)
{
    GSDVDWork* work;

    work = _info2work(fileInfo);
    if (work != NULL && work->callback != NULL) {
        work->callback(result, work);
    }
}

GSDVDWork* _info2work(void* fileInfo)
{
    GSDVDWork* base;
    GSDVDWork* work;
    u32 i;

    base = lbl_8047B0F4;
    work = base;
    for (i = 0; i < lbl_8047B0F8; i++, work++) {
        if (work->active != 0 && work->fileInfo == fileInfo) {
            return &base[i];
        }
    }
    return NULL;
}

void fn_80167B70(void)
{
    u32 handle = fn_800E202C();

    if ((u16)handle != 0) {
        fn_800E24B0(handle);
        fn_800E209C(handle);
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
        fn_801669E4(index, 0, 0);
    }
    _sndInitParms(entry, work);
    fn_8016782C((u8*)work);
    entry->work = NULL;
    ((GSsndFlagBits*)&entry->flags)->active = 0;
    return 1;
}

u8 fn_80167118(u32 slot, u32 stream, const char* path, u32 offset,
               void* resourceOwner, u32 resourceId3, u32 resourceId1,
               u32 resourceId2)
{
    extern void fn_80167B70();
    GSsndOpenState* state = &lbl_80478FB4[slot];
    void* resource1;
    void* resource2;
    void* resource3;
    void* sampleData;

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

u32 fn_8016758C(GSsndEntry* entry, u32 id)
{
    GSsndWork* work;

    if (((GSsndFlagBits*)&entry->flags)->active != 1) {
        fn_80166B3C(id, 0, 0);
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
    fn_80166C34(entry->reverb);
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

void* fn_80167BB0(u32 size)
{
    u32 handle = fn_800E2C04(size, 0x20);

    if ((u16)handle != 0) {
        return fn_800E27B0(handle);
    }
    return 0;
}

s32 fn_80167E10(u8* handle)
{
    return DVDGetCommandBlockStatus(handle + 4);
}

s32 fn_80167E34(void)
{
    return DVDGetDriveStatus();
}

void fn_80167E64(u8* file)
{
    fn_80168164(file);
    DVDClose(file + 4);
}

u32 fn_80167EF8(const char* path)
{
    return DVDConvertPathToEntrynum(path) != -1;
}

GSDVDWork* fn_80167F28(const char* path)
{
    GSDVDWork* work = fn_8016819C();

    if (work == NULL) {
        return NULL;
    }
    if (!DVDOpen(path, work->fileInfo)) {
        __assert("GSdvd.c", 0x26A, "FALSE");
        fn_80168164((u8*)work);
        return NULL;
    }
    return work;
}

u8 fn_80167E98(GSDVDWork* work, void* addr, s32 length, s32 offset,
                void (*callback)(s32 result, GSDVDWork* work))
{
    work->callback = callback;
    return DVDReadAsync(work->fileInfo, addr, length, offset, _AsyncCallback,
                        2);
}

s32 fn_80167ED0(GSDVDWork* work, void* addr, s32 length, s32 offset)
{
    return DVDRead(work->fileInfo, addr, length, offset, 2);
}

void fn_80168164(u8* flag)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    *flag = 0;
    OSRestoreInterrupts(enabled);
}

u32 fn_80167FA8(u32 workCount)
{
    u8* destination;

    lbl_8047B0F8 = workCount;
    lbl_8047B0F4 = fn_8016824C(workCount * sizeof(GSDVDWork));
    if (lbl_8047B0F4 == NULL) {
        return 0;
    }
    fn_8016821C();
    DVDInit();
    if (fn_800057A8() != 4) {
        switch (fn_800057A0()) {
        case 0:
            lbl_80478C24 = (const u8*)"GC6J";
            break;
        case 1:
            lbl_80478C24 = (const u8*)"GC6E";
            break;
        case 2:
            lbl_80478C24 = (const u8*)"GC6E";
            break;
        }
        destination = fn_800A7BCC();
        destination[0] = lbl_80478C24[0];
        destination[1] = lbl_80478C24[1];
        destination[2] = lbl_80478C24[2];
        destination[3] = lbl_80478C24[3];
        destination[4] = lbl_80478C28[0];
        destination[5] = lbl_80478C28[1];
        destination[6] = 0;
        destination[7] = 0;
    }
    DVDSetAutoFatalMessaging(TRUE);
    GSgappCreate(1, 0x13, 0, _gsdvdErrorTask_801879AC);
    return 1;
}

GSDVDWork* fn_8016819C(void)
{
    GSDVDWork* base;
    GSDVDWork* work;
    GSDVDWork* result;
    BOOL enabled;
    u32 i;

    enabled = OSDisableInterrupts();
    result = NULL;
    base = lbl_8047B0F4;
    work = base;
    for (i = 0; i < lbl_8047B0F8; i++, work++) {
        if (work->active != 1) {
            base[i].active = 1;
            result = &lbl_8047B0F4[i];
            break;
        }
    }
    OSRestoreInterrupts(enabled);
    return result;
}

void fn_8016821C(void)
{
    u32 i;

    i = 0;
    while (i < lbl_8047B0F8) {
        lbl_8047B0F4[i].active = 0;
        i++;
    }
}

void fn_80168284(void)
{
    u32 index;
    u8 capacity;
    GSFilter* filter;

    if (lbl_804526E0.count != 0) {
        if (lbl_804526E0.drawingCount != 0) {
            capacity = lbl_804526E0.capacity;
            filter = lbl_804526E0.filters;
            fn_800D88DC(1);
            fn_800D888C(6);
            fn_800D9B58(lbl_8047D5A0, lbl_8047D5A0, lbl_8047D5A4,
                        lbl_8047D5A8);
            fn_800DA4C4(1, 6, 7);
            fn_800DA2BC(1, 1, 0);
            fn_800DA100(0, 7, 0, 0, 7, 0);
            fn_800DA1E8(1, 1, 1);
            fn_800D9ED8(1);
            fn_800DA028(0);
            fn_800D7820(lbl_804526E0.renderState);
            fn_800D6A00(6);
            fn_800D67BC((u16)(lbl_804526E0.drawingCount * 4));
            for (index = 0; (u8)index < capacity; index++, filter++) {
                if (filter->drawing != 0) {
                    fn_800D5FA4(0);
                    fn_800D5A38(0, index);
                    fn_800D5FA4(1);
                    fn_800D5A38(0, index);
                    fn_800D5FA4(2);
                    fn_800D5A38(0, index);
                    fn_800D5FA4(3);
                    fn_800D5A38(0, index);
                }
            }
            fn_800D6728();
            fn_800D9ED8(0);
        }
    }
}

void* fn_8016824C(u32 size)
{
    u32 handle = fn_800E2C04(size, 0x20);

    if ((u16)handle != 0) {
        return fn_800E27B0(handle);
    }
    return 0;
}

void fn_801684F0(GSDVDWork* work)
{
    if (work->started != 0) {
        work->started = 0;
        if (work->drawing != 0) {
            GXDrawDone();
            fn_800B856C();
            work->drawing = 0;
            lbl_804526E0.drawingCount--;
        }
        lbl_804526E0.count--;
    }
}

void fn_80168408(GSFilter* filter, const u8* color)
{
    u8* destination;

    if (filter->active == 0) {
        return;
    }
    destination = &lbl_804526E0.colors[filter->index * 4];
    destination[0] = color[0];
    destination[1] = color[1];
    destination[2] = color[2];
    destination[3] = color[3];
    DCFlushRange(destination, 4);
    if (color[3] != 0) {
        if (filter->drawing == 0) {
            filter->drawing = 1;
            lbl_804526E0.drawingCount++;
        }
    } else if (filter->drawing != 0) {
        GXDrawDone();
        fn_800B856C();
        filter->drawing = 0;
        lbl_804526E0.drawingCount--;
    }
}

GSFilter* GSfilterCreate(const u8* color)
{
    GSFilter* filter;
    u8 capacity = lbl_804526E0.capacity;
    u8 index;

    if (lbl_804526E0.count < capacity) {
        filter = lbl_804526E0.filters;
        for (index = 0; index < capacity; index++, filter++) {
            if (filter->active == 0) {
                u8* destination = &lbl_804526E0.colors[index * 4];
                destination[0] = color[0];
                destination[1] = color[1];
                destination[2] = color[2];
                destination[3] = color[3];
                if (color[3] != 0 && filter->drawing == 0) {
                    filter->drawing = 1;
                    lbl_804526E0.drawingCount++;
                }
                filter->active = 1;
                lbl_804526E0.count++;
                return filter;
            }
        }
    }
    return NULL;
}

void fn_80168638(u32 capacity)
{
    u32 size;
    u16 handle;
    u32 index;
    void* renderState;
    u32 clearColor;

    memset(&lbl_804526E0, 0, sizeof(GSFilterState));
    size = (capacity & 0xFF) * sizeof(GSFilter);

    handle = _toolentryAlloc__FUl(size);
    if (handle == 0) {
        return;
    }
    lbl_804526E0.filterHandle = handle;
    lbl_804526E0.filters = fn_800E27B0(handle);
    memset(lbl_804526E0.filters, 0, size);

    handle = _toolentryAlloc__FUl(size);
    if (handle == 0) {
        fn_800E24B0(lbl_804526E0.filterHandle);
        fn_800E209C(lbl_804526E0.filterHandle);
        memset(&lbl_804526E0, 0, sizeof(GSFilterState));
        return;
    }
    lbl_804526E0.colorHandle = handle;
    lbl_804526E0.colors = fn_800E27B0(handle);
    memset(lbl_804526E0.colors, 0, size);

    handle = _toolentryAlloc__FUl(size);
    if (handle == 0) {
        fn_800E24B0(lbl_804526E0.filterHandle);
        fn_800E209C(lbl_804526E0.filterHandle);
        fn_800E24B0(lbl_804526E0.colorHandle);
        fn_800E209C(lbl_804526E0.colorHandle);
        if (lbl_804526E0.renderState != 0) {
            fn_800D75F4((void*)lbl_804526E0.renderState);
        }
        memset(&lbl_804526E0, 0, sizeof(GSFilterState));
        return;
    }

    lbl_804526E0.viewport[0] = 0;
    lbl_804526E0.viewport[1] = 0;
    lbl_804526E0.viewport[2] = 0;
    lbl_804526E0.viewport[3] = 480;
    lbl_804526E0.viewport[4] = 640;
    lbl_804526E0.viewport[5] = 480;
    lbl_804526E0.viewport[6] = 640;
    lbl_804526E0.viewport[7] = 0;
    lbl_804526E0.capacity = capacity;

    renderState = fn_800D7894();
    if (renderState != NULL) {
        fn_800D7868(renderState, 1, 1, 0, 2, 0, lbl_804526E0.viewport, 4);
        fn_800D7868(renderState, 4, 1, 6, 10, 0, lbl_804526E0.colors, 4);
    }
    lbl_804526E0.renderState = (u32)renderState;

    for (index = 0; index < (capacity & 0xFF); index++) {
        lbl_804526E0.filters[index].index = index;
    }

    clearColor = 0;
    lbl_8047B100 = GSfilterCreate((const u8*)&clearColor);
    GSgappCreate(1, 0xFC, 0, fn_80168284);
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
            fn_80166B3C(id, 0, 0);
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

u32 fn_80167720(u32 handle)
{
    GSsndEntry* entry;
    u32 count;
    u32 i;

    entry = lbl_80478FAC;
    i = 0;
    count = lbl_8047B0E8;
    while (i < count) {
        if (entry->work != NULL && entry->work->handle == handle) {
            return i;
        }
        entry++;
        i++;
    }
    return 0;
}

u32 fn_80167768(u32 unkC, u32 unk10)
{
    GSsndEntry* entry;
    u32 count;
    u32 i;

    entry = lbl_80478FAC;
    i = 0;
    count = lbl_8047B0E8;
    while (i < count) {
        if (entry->work != NULL && entry->work->unkC == unkC &&
            entry->work->unk10 == unk10) {
            return i;
        }
        entry++;
        i++;
    }
    return -1;
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
    u32 offset;
    u32 i;
    u8 value;

    offset = 0;
    i = 0;
    value = 0;
    while (i < lbl_8047B0E0) {
        lbl_8047B0DC[offset] = value;
        i++;
        offset += 0x14;
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

extern s32 lbl_8047B0F0;

void _errorTask_State_None_80187B24(s32 state)
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

void _gsdvdErrorTask_801879AC(void)
{
    extern void _errorTask_State_None_80187B24(s32 state);
    extern void _gsdvdError_MsgOpen(u32 message);
    s32 driveState = fn_80167E34();

    switch (lbl_8047B0F0) {
    case 0:
        _errorTask_State_None_80187B24(driveState);
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
    case 10:
        fn_800056D4();
        _gsdvdError_MsgOpen(lbl_80478C20);
        lbl_8047B0F0 = 10;
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
