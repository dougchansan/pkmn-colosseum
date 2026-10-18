/**
 * @file gs_spline.c
 * @brief GSspline -- spline control-vector free/create management
 *        (0x80176068 - 0x801765F4).
 *
 * Split from the former game/gs_scene.c CodeCandidate bucket
 * (0x8017572C - 0x8017A5FC); see config/GC6E01/splits.txt for the exact
 * address ranges of the four resulting translation units:
 *   game/generator.c                    0x80173624 - 0x80175F6C
 *   game/gs_xfb_capture.c               0x80175F6C - 0x80176068
 *   game/gs_spline.c                    0x80176068 - 0x801765F4 (this file)
 *   game/camera.c                       0x801765F4 - 0x80179F4C
 *
 * Corresponds to game/pxdvs/GSAPI/GSspline/GSspline.cpp in the XD-era
 * source tree. Shared externs/typedefs for the whole former gs_scene.c
 * range live in include/game/gs_scene_types.h.
 *
 * Functions (3, per config/GC6E01/symbols.txt):
 *   GSsplineFree                  (0x80176068, was fn_80176068)
 *   GSsplineAddControlVectorValue (0x801760C4, not yet decompiled)
 *   GSsplineCreate                (0x80176228, not yet decompiled)
 */

#include "game/gs_scene_types.h"

typedef struct GSspline {
    s32 kind;
    s32 state;
    u8 capacity;
    u8 valueCount;
    u8 pad0A;
    u8 keyCount;
    void* vectors;
    void* secondaryVectors;
    f32* values;
    f32 firstValue;
    f32 lastValue;
    u16 selfHandle;
    u16 dataHandle;
} GSspline;

extern const char lbl_80273A10[];

#ifdef GS_SPLINE_EXACT_PREFIX
#if 0
asm void GSsplineFree(void) {
#include "src/game/gs_scene_fn_80176068.inc"
}
#else
void GSsplineFree(u8* ptr) {
    u16 handle;
    if (ptr != NULL) {
        handle = *(u16*)(ptr + 0x22);
        fn_800E24B0(handle);
        GSmemFree(handle);
        handle = *(u16*)(ptr + 0x20);
        fn_800E24B0(handle);
        GSmemFree(handle);
    }
}
#endif

void GSsplineAddControlVectorValue(GSspline* spline, void* vector, f32 value)
{
    const char* messages = lbl_80273A10;
    u8 index;
    u8 storesKey;
    u8 key;

    if (spline == NULL) {
        return;
    }
    if (spline->state == 1) {
        index = spline->valueCount;
        if (index < spline->capacity) {
            storesKey = TRUE;
            if ((spline->kind == 1 || spline->kind == 2) && index % 3 != 0) {
                storesKey = FALSE;
            }

            GSvecCopy((u8*)spline->vectors + index * 12, vector);
            spline->values[spline->valueCount] = value;
            spline->valueCount++;

            if (storesKey) {
                key = spline->keyCount;
                spline->values[key] = value;
                if (key == 0) {
                    spline->firstValue = value;
                } else if (value < spline->values[key - 1]) {
                    GSlogWrite(messages + 0xCC);
                }
                if (spline->valueCount == spline->capacity) {
                    spline->lastValue = value;
                }
                spline->keyCount++;
            }
        } else {
            GSlogWrite(messages + 0x110);
        }
    } else {
        GSlogWrite(messages + 0x14C);
    }
}

#else
GSspline* GSsplineCreate(s32 kind, s32 state, u8 capacity)
{
    extern u16 _toolentryAlloc__FUl(u32 size);
    extern void* GSmemLock(u16 handle);
    extern void* fn_800E24B0(u16 handle);
    extern void GSmemFree(u16 handle);
    const char* messages = lbl_80273A10;
    GSspline* spline = NULL;
    u16 handle;
    u8* data;
    u8* secondary;

    if (capacity < 2) {
        GSlogWrite(messages + 0x260);
        return NULL;
    }

    if ((kind == 1 || kind == 2) && ((capacity - 1) % 3) != 0) {
        if (capacity == 2) {
            GSlogWrite(messages + 0x298);
            kind = 0;
        } else {
            GSlogWrite(messages + 0x2E8);
            kind = 3;
        }
    } else if (capacity == 2 && kind != 0) {
        kind = 0;
        GSlogWrite(messages + 0x33C);
    }

    handle = _toolentryAlloc__FUl(sizeof(GSspline));
    if (handle != 0) {
        spline = GSmemLock(handle);
        spline->selfHandle = handle;
        spline->kind = kind;
        spline->state = state;
        spline->valueCount = 0;
        spline->keyCount = 0;

        switch (kind) {
        case 0:
            if (state == 1) {
                handle = _toolentryAlloc__FUl(capacity * (12 + sizeof(f32)));
                if (handle != 0) {
                    spline->dataHandle = handle;
                    data = GSmemLock(handle);
                    spline->vectors = data;
                    spline->secondaryVectors = NULL;
                    spline->values = (f32*)(data + capacity * 12);
                    spline->capacity = capacity;
                    spline->state = 1;
                } else {
                    handle = spline->selfHandle;
                    fn_800E24B0(handle);
                    GSmemFree(handle);
                    spline = NULL;
                }
            } else {
                handle = _toolentryAlloc__FUl(capacity * (sizeof(f32) * 2));
                if (handle != 0) {
                    spline->dataHandle = handle;
                    data = GSmemLock(handle);
                    spline->vectors = data;
                    spline->secondaryVectors = NULL;
                    spline->values = (f32*)(data + capacity * 4);
                    spline->capacity = capacity;
                    spline->state = 0;
                } else {
                    handle = spline->selfHandle;
                    fn_800E24B0(handle);
                    GSmemFree(handle);
                    spline = NULL;
                }
            }
            break;
        case 1:
        case 2:
            if (state == 1) {
                handle = _toolentryAlloc__FUl(capacity * 12 + ((capacity + 2) / 3) * sizeof(f32));
                if (handle != 0) {
                    spline->dataHandle = handle;
                    data = GSmemLock(handle);
                    spline->vectors = data;
                    spline->secondaryVectors = NULL;
                    spline->values = (f32*)(data + capacity * 12);
                    spline->capacity = capacity;
                    spline->state = 1;
                } else {
                    handle = spline->selfHandle;
                    fn_800E24B0(handle);
                    GSmemFree(handle);
                    spline = NULL;
                }
            } else {
                handle = _toolentryAlloc__FUl((capacity + (capacity + 2) / 3) * sizeof(f32));
                if (handle != 0) {
                    spline->dataHandle = handle;
                    data = GSmemLock(handle);
                    spline->vectors = data;
                    spline->secondaryVectors = NULL;
                    spline->values = (f32*)(data + capacity * 4);
                    spline->capacity = capacity;
                    spline->state = 0;
                } else {
                    handle = spline->selfHandle;
                    fn_800E24B0(handle);
                    GSmemFree(handle);
                    spline = NULL;
                }
            }
            break;
        default:
            if (state == 1) {
                handle = _toolentryAlloc__FUl(capacity * (12 + sizeof(f32) * 2));
                if (handle != 0) {
                    spline->dataHandle = handle;
                    data = GSmemLock(handle);
                    spline->vectors = data;
                    secondary = data + capacity * 12;
                    spline->secondaryVectors = secondary;
                    spline->values = (f32*)(secondary + capacity * 4);
                    spline->capacity = capacity;
                    spline->state = 1;
                } else {
                    handle = spline->selfHandle;
                    fn_800E24B0(handle);
                    GSmemFree(handle);
                    spline = NULL;
                }
            } else {
                handle = _toolentryAlloc__FUl(capacity * 3 * sizeof(f32));
                if (handle != 0) {
                    spline->dataHandle = handle;
                    data = GSmemLock(handle);
                    spline->vectors = data;
                    secondary = data + capacity * 4;
                    spline->secondaryVectors = secondary;
                    spline->values = (f32*)(secondary + capacity * 4);
                    spline->capacity = capacity;
                    spline->state = 0;
                } else {
                    handle = spline->selfHandle;
                    fn_800E24B0(handle);
                    GSmemFree(handle);
                    spline = NULL;
                }
            }
            break;
        }
    }

    return spline;
}
#endif
