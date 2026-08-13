/**
 * @file gs_spline.c
 * @brief GSspline -- spline control-vector free/create management
 *        (0x80176068 - 0x801765F4).
 *
 * Split from the former game/gs_scene.c CodeCandidate bucket
 * (0x8017572C - 0x8017A5FC); see config/GC6E01/splits.txt for the exact
 * address ranges of the four resulting translation units:
 *   game/ps_generator_range_8017572C.c  0x8017572C - 0x80175F6C
 *   game/gs_xfb_capture.c               0x80175F6C - 0x80176068
 *   game/gs_spline.c                    0x80176068 - 0x801765F4 (this file)
 *   game/camera.c                       0x801765F4 - 0x8017A5FC
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

extern const char lbl_80273ADC[];
extern const char lbl_80273B20[];
extern const char lbl_80273B5C[];

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void GSsplineFree(void) {
#include "src/game/gs_scene_fn_80176068.inc"
}
#else
#pragma optimization_level 4
void GSsplineFree(u8* ptr) {
    u16 handle;
    if (ptr != NULL) {
        handle = *(u16*)(ptr + 0x22);
        fn_800E24B0(handle);
        fn_800E209C(handle);
        handle = *(u16*)(ptr + 0x20);
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }
}
#endif
#pragma pop

void GSsplineAddControlVectorValue(GSspline* spline, void* vector, f32 value)
{
    u8 index;
    u8 storesKey;

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
            spline->values[index] = value;
            spline->valueCount++;

            if (storesKey) {
                spline->values[spline->keyCount] = value;
                if (spline->keyCount == 0) {
                    spline->firstValue = value;
                } else if (value < spline->values[spline->keyCount - 1]) {
                    GSlogWrite(lbl_80273ADC);
                }
                if (spline->valueCount == spline->capacity) {
                    spline->lastValue = value;
                }
                spline->keyCount++;
            }
        } else {
            GSlogWrite(lbl_80273B20);
        }
    } else {
        GSlogWrite(lbl_80273B5C);
    }
}

GSspline* GSsplineCreate(s32 kind, s32 state, u8 capacity)
{
    extern u16 _toolentryAlloc__FUl(u32 size);
    extern void* fn_800E27B0(u16 handle);
    extern void* fn_800E24B0(u16 handle);
    extern void fn_800E209C(u16 handle);
    extern char lbl_80273A10[];
    GSspline* spline;
    u16 selfHandle;
    u16 dataHandle;
    u8* data;
    u8 count;

    if (capacity < 2) {
        GSlogWrite(lbl_80273A10 + 0x260);
        return NULL;
    }

    count = capacity;
    if (kind == 1 || kind == 2) {
        if (((count - 1) % 3) != 0) {
            if (count == 2) {
                GSlogWrite(lbl_80273A10 + 0x298);
                kind = 0;
            } else {
                GSlogWrite(lbl_80273A10 + 0x2E8);
                kind = 3;
            }
        }
    } else if (count == 2 && kind != 0) {
        GSlogWrite(lbl_80273A10 + 0x33C);
        kind = 0;
    }

    selfHandle = _toolentryAlloc__FUl(sizeof(GSspline));
    if (selfHandle == 0) {
        return NULL;
    }
    spline = fn_800E27B0(selfHandle);
    spline->selfHandle = selfHandle;
    spline->kind = kind;
    spline->state = state;
    spline->valueCount = 0;
    spline->keyCount = 0;
    spline->secondaryVectors = NULL;
    spline->firstValue = 0.0f;
    spline->lastValue = 0.0f;

    if (kind == 0) {
        if (state == 1) {
            dataHandle = _toolentryAlloc__FUl((u32)count * 16);
            if (dataHandle == 0) {
                fn_800E24B0(selfHandle);
                fn_800E209C(selfHandle);
                return NULL;
            }
            spline->dataHandle = dataHandle;
            data = fn_800E27B0(dataHandle);
            spline->vectors = data;
            spline->secondaryVectors = NULL;
            spline->values = (f32*)(data + count * 12);
            spline->capacity = count;
            spline->state = 1;
        } else {
            dataHandle = _toolentryAlloc__FUl((u32)count * 8);
            if (dataHandle == 0) {
                fn_800E24B0(selfHandle);
                fn_800E209C(selfHandle);
                return NULL;
            }
            spline->dataHandle = dataHandle;
            data = fn_800E27B0(dataHandle);
            spline->vectors = data;
            spline->secondaryVectors = NULL;
            spline->values = (f32*)(data + count * 4);
            spline->capacity = count;
            spline->state = 0;
        }
    } else if (kind >= 0 && kind < 3) {
        if (state == 1) {
            dataHandle = _toolentryAlloc__FUl((u32)count * 12 + ((count + 2) / 3) * 4);
            if (dataHandle == 0) {
                fn_800E24B0(selfHandle);
                fn_800E209C(selfHandle);
                return NULL;
            }
            spline->dataHandle = dataHandle;
            data = fn_800E27B0(dataHandle);
            spline->vectors = data;
            spline->secondaryVectors = NULL;
            spline->values = (f32*)(data + count * 12);
            spline->capacity = count;
            spline->state = 1;
        } else {
            dataHandle = _toolentryAlloc__FUl((count + ((count + 2) / 3)) * 4);
            if (dataHandle == 0) {
                fn_800E24B0(selfHandle);
                fn_800E209C(selfHandle);
                return NULL;
            }
            spline->dataHandle = dataHandle;
            data = fn_800E27B0(dataHandle);
            spline->vectors = data;
            spline->secondaryVectors = NULL;
            spline->values = (f32*)(data + count * 4);
            spline->capacity = count;
            spline->state = 0;
        }
    } else if (state == 1) {
        dataHandle = _toolentryAlloc__FUl((u32)count * 20);
        if (dataHandle == 0) {
            fn_800E24B0(selfHandle);
            fn_800E209C(selfHandle);
            return NULL;
        }
        spline->dataHandle = dataHandle;
        data = fn_800E27B0(dataHandle);
        spline->vectors = data;
        spline->secondaryVectors = data + count * 12;
        spline->values = (f32*)((u8*)spline->secondaryVectors + count * 4);
        spline->capacity = count;
        spline->state = 1;
    } else {
        dataHandle = _toolentryAlloc__FUl((u32)count * 12);
        if (dataHandle == 0) {
            fn_800E24B0(selfHandle);
            fn_800E209C(selfHandle);
            return NULL;
        }
        spline->dataHandle = dataHandle;
        data = fn_800E27B0(dataHandle);
        spline->vectors = data;
        spline->secondaryVectors = data + count * 4;
        spline->values = (f32*)((u8*)spline->secondaryVectors + count * 4);
        spline->capacity = count;
        spline->state = 0;
    }

    return spline;
}
