/**
 * @file gs_dvd_r47_80168638_o4s.c
 * @brief Screen-filter manager initialisation, 0x80168638 - 0x80168934.
 *
 * Function-boundary carve of fn_80168638, the last function of the
 * screen-filter block that gs_dvd_r47_prefix.c owns up to 0x80168638.
 * The range has no jump tables or pooled constants: every immediate is
 * an `li`, and all data (lbl_804526E0, lbl_8047B100) stays extern.
 *
 * Retail expands GSfilterCreate (out of line at 0x80168570, owned by
 * gs_dvd_r47_prefix.c) in place here, with the clear colour's bytes
 * constant-folded and its colour[3] test removed. The body below is the
 * same GSfilterCreate source as the prefix's out-of-line copy, declared
 * `inline` so this carve reproduces the expansion without emitting a
 * second GSfilterCreate symbol.
 */
#include "dolphin/types.h"

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

extern void* memset(void* dst, int value, u32 size);
extern u16 _toolentryAlloc__FUl(u32 size);
extern void* GSmemLock(u32 handle);
extern void fn_800E24B0(u32 handle);
extern void GSmemFree(u32 handle);
extern void* gfxVtxDescAlloc(void);
extern void gfxVtxDescSetAttr(void*, u32, u32, u32, u32, u8, void*, u8);
extern void gfxVtxDescFree(void*);
extern u32 GSgappCreate(s32 state, u8 priority, u32 parameter,
                       void (*callback)(void));
extern void fn_80168284(void);

extern GSFilterState lbl_804526E0;
extern GSFilter* lbl_8047B100;

inline GSFilter* GSfilterCreate(const u8* color)
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

/*
 * Tears the filter manager back down. Repeated expansion: retail carries
 * this exact sequence on both allocation-failure paths.
 */
static inline void GSfilterRelease(void)
{
    u16 handle;

    handle = lbl_804526E0.filterHandle;
    if (handle != 0) {
        fn_800E24B0(handle);
        GSmemFree(handle);
    }
    handle = lbl_804526E0.colorHandle;
    if (handle != 0) {
        fn_800E24B0(handle);
        GSmemFree(handle);
    }
    if (lbl_804526E0.renderState != 0) {
        gfxVtxDescFree((void*)lbl_804526E0.renderState);
    }
    memset(&lbl_804526E0, 0, sizeof(GSFilterState));
}

/*
 * Creates the filter render state and binds the viewport and colour
 * arrays to it. Inline fingerprint: on the NULL path retail loads the
 * result register with a fresh `li r28, 0` even though the preceding
 * `mr. r28, r3` has just proven it zero; that is this helper's own
 * `return NULL` materialised into the inlined return temp. Written
 * inline in the caller, the store takes the tested value directly and
 * no such load exists.
 */
static inline void* GSfilterRenderStateCreate(void)
{
    void* renderState = gfxVtxDescAlloc();

    if (renderState != NULL) {
        gfxVtxDescSetAttr(renderState, 1, 1, 0, 2, 0, lbl_804526E0.viewport, 4);
        gfxVtxDescSetAttr(renderState, 4, 1, 6, 10, 0, lbl_804526E0.colors, 4);
        return renderState;
    }
    return NULL;
}

void fn_80168638(u8 capacity)
{
    u32 size;
    u16 handle;
    GSFilter* filter;
    u32 index;
    u8 clearColor[4];

    memset(&lbl_804526E0, 0, sizeof(GSFilterState));
    size = capacity * sizeof(GSFilter);

    handle = _toolentryAlloc__FUl(size);
    if (handle == 0) {
        return;
    }
    lbl_804526E0.filterHandle = handle;
    lbl_804526E0.filters = GSmemLock(lbl_804526E0.filterHandle);
    memset(lbl_804526E0.filters, 0, size);

    handle = _toolentryAlloc__FUl(size);
    if (handle == 0) {
        GSfilterRelease();
        return;
    }
    lbl_804526E0.colorHandle = handle;
    lbl_804526E0.colors = GSmemLock(lbl_804526E0.colorHandle);
    memset(lbl_804526E0.colors, 0, size);

    /* Retail allocates a third block and only checks it for failure. */
    handle = _toolentryAlloc__FUl(size);
    if (handle == 0) {
        GSfilterRelease();
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

    lbl_804526E0.renderState = (u32)GSfilterRenderStateCreate();

    filter = lbl_804526E0.filters;
    for (index = 0; (u8)index < capacity; index++, filter++) {
        filter->index = index;
    }

    clearColor[0] = 0;
    clearColor[1] = 0;
    clearColor[2] = 0;
    clearColor[3] = 0;
    lbl_8047B100 = GSfilterCreate(clearColor);
    GSgappCreate(1, 0xFC, 0, fn_80168284);
}
