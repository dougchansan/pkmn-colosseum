#ifndef HSD_SPLINE_H
#define HSD_SPLINE_H

#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "hsd/hsd_forward.h"

/* HAL sysdolphin spline.h */
struct HSD_Spline {
    /* 0x00 */ u8 type;
    /* 0x02 */ s16 numcv;
    /* 0x04 */ f32 tension;
    /* 0x08 */ Vec3* cv;
    /* 0x0C */ f32 totalLength;
    /* 0x10 */ f32* segLength;
    /* 0x14 */ f32 (*segPoly)[5];
};

f32 splGetHelmite(f32 fterm, f32 time, f32 p0, f32 p1, f32 d0,
                f32 d1);                                   /* splGetHelmite */
void splGetSplinePoint(Vec3* p, HSD_Spline* spline, f32 u);    /* splGetSplinePoint */
f32 splArcLengthGetParameter(HSD_Spline* spline, f32 u); /* splArcLengthGetParameter */
void splArcLengthPoint(Vec3* p, HSD_Spline* spline, f32 u);

#endif /* HSD_SPLINE_H */
