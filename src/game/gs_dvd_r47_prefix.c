/**
 * @file gs_dvd_r47_prefix.c
 * @brief Exact DVD work-slot and screen-filter helpers, 0x80167FA8 - 0x80168638.
 *
 * Standalone owner for the linked .text range; the same bodies remain in
 * gs_dvd.c for the neighbouring candidate score units.
 */
#include "dolphin/types.h"

#define GS_SDATA2 __declspec(section ".sdata2")

typedef struct GSDVDWork {
    u8 active;
    u8 started;
    u8 drawing;
    u8 _pad03;
    u8 fileInfo[0x3C];
    void (*callback)(s32 result, struct GSDVDWork* work);
} GSDVDWork;

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

extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL level);
extern u32 fn_800E2C04(u32 size, u32 align);
extern void* fn_800E27B0(u32 handle);
extern void GXDrawDone(void);
extern void fn_800B856C(void);
extern void DCFlushRange(void* address, u32 size);
extern void DVDInit(void);
extern BOOL DVDSetAutoFatalMessaging(BOOL enable);
extern s32 fn_800057A0(void);
extern s32 fn_800057A8(void);
extern u8* fn_800A7BCC(void);
extern u32 GSgappCreate(s32 state, u8 priority, u32 parameter,
                       void (*callback)(void));
extern void _gsdvdErrorTask(void);
extern void gfxSetFlags(u32 enable);
extern void gfxClearFlags(u32 value);
extern void gfxSetOrtho(f32 red, f32 green, f32 blue, f32 alpha);
extern void gfxSetBlendMode(u32, u32, u32);
extern void gfxSetUpdates(u32, u32, u32);
extern void fn_800DA100(u32, u32, u32, u32, u32, u32);
extern void gfxSetZMode(u32, u32, u32);
extern void fn_800D9ED8(u32);
extern void gfxSetCullMode(u32);
extern void gfxSetVtxDesc(u32);
extern void gfxSetPrimitive(u32);
extern void gfxBegin(u32);
extern void fn_800D5FA4(u32);
extern void fn_800D5A38(u32, u32);
extern void gfxEnd(void);

extern GSDVDWork* lbl_8047B0F4;
extern u32 lbl_8047B0F8;
extern GSFilterState lbl_804526E0;
extern const u8* lbl_80478C24;
extern const u8* lbl_80478C28;
extern GS_SDATA2 const u8 lbl_8047D578[]; /* "GC6J" */
extern GS_SDATA2 const u8 lbl_8047D594[]; /* "GC6E" */
extern const f32 lbl_8047D5A0;
extern const f32 lbl_8047D5A4;
extern const f32 lbl_8047D5A8;

GSDVDWork* _info2work(void* fileInfo);
void fn_80168164(u8* flag);
GSDVDWork* fn_8016819C(void);
void fn_8016821C(void);
void* fn_8016824C(u32 size);

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
            lbl_80478C24 = lbl_8047D578;
            break;
        case 1:
            lbl_80478C24 = lbl_8047D594;
            break;
        case 2:
            lbl_80478C24 = lbl_8047D594;
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
    GSgappCreate(1, 0x13, 0, _gsdvdErrorTask);
    return 1;
}

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
    GSDVDWork* cursor;
    GSDVDWork* base;
    u32 i;

    cursor = base = lbl_8047B0F4;
    for (i = 0; i < lbl_8047B0F8; cursor++, i++) {
        if (cursor->active != 0 && cursor->fileInfo == fileInfo) {
            return &base[i];
        }
    }
    return NULL;
}

void fn_80168164(u8* flag)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    *flag = 0;
    OSRestoreInterrupts(enabled);
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
    work = base = lbl_8047B0F4;
    for (i = 0; i < lbl_8047B0F8; work++, i++) {
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

void* fn_8016824C(u32 size)
{
    u32 handle = fn_800E2C04(size, 0x20);

    if ((u16)handle != 0) {
        return fn_800E27B0(handle);
    }
    return 0;
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
            gfxSetFlags(1);
            gfxClearFlags(6);
            gfxSetOrtho(lbl_8047D5A0, lbl_8047D5A0, lbl_8047D5A4,
                        lbl_8047D5A8);
            gfxSetBlendMode(1, 6, 7);
            gfxSetUpdates(1, 1, 0);
            fn_800DA100(0, 7, 0, 0, 7, 0);
            gfxSetZMode(1, 1, 1);
            fn_800D9ED8(1);
            gfxSetCullMode(0);
            gfxSetVtxDesc(lbl_804526E0.renderState);
            gfxSetPrimitive(6);
            gfxBegin((u16)(lbl_804526E0.drawingCount * 4));
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
            gfxEnd();
            fn_800D9ED8(0);
        }
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
