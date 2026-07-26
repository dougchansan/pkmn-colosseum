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
    u32 pad10;
    f32* values;
    f32 firstValue;
    f32 lastValue;
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
    if (spline->state != 1) {
        GSlogWrite(lbl_80273B5C);
        return;
    }

    index = spline->valueCount;
    if (index >= spline->capacity) {
        GSlogWrite(lbl_80273B20);
        return;
    }

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
}
