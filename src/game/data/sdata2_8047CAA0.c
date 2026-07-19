#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed gs_render / gs_material .sdata2 constants. String labels are used by
 * render/material assert paths; numeric labels are referenced from gs_render.c
 * and gs_material.c. This prefix stops before GSmodelSetMaterialAlpha's
 * compiler-owned conversion pool at 0x8047CB88.
 */
SDATA2 const u8 lbl_8047CAA0[8] = "(float)";
SDATA2 const u8 lbl_8047CAA8[7] = "(null)";
SDATA2 const f32 lbl_8047CAB0 = 0.0f;
SDATA2 const f32 lbl_8047CAB4 = 0.1f;
SDATA2 const f32 lbl_8047CAB8 = 10.0f;
SDATA2 const f32 lbl_8047CABC = 1000000000.0f;
SDATA2 const f64 lbl_8047CAC0 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047CAC8 = 0.0f;
SDATA2 const f32 lbl_8047CACC = 255.0f;
SDATA2 const f64 lbl_8047CAD0 = 4.503599627370496e+15;
SDATA2 const f32 lbl_8047CAD8 = 1.0f;
SDATA2 const f32 lbl_8047CADC = 0.0f;
SDATA2 const f32 lbl_8047CAE0 = 1.0f;
SDATA2 const f32 lbl_8047CAE4 = 0.0f;
SDATA2 const f32 lbl_8047CAE8[2] = { 3.0f, 0.0f };
SDATA2 const f32 lbl_8047CAF0 = 0.0f;
SDATA2 const f32 lbl_8047CAF4 = 1.0f;
SDATA2 const f32 lbl_8047CAF8 = 1.0f;
SDATA2 const f32 lbl_8047CAFC = 0.0f;
SDATA2 const f64 lbl_8047CB00 = 4.503599627370496e+15;
SDATA2 const f64 lbl_8047CB08 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047CB10[2] = { 1.0f, 0.0f };
SDATA2 const f32 lbl_8047CB18 = 90.0f;
SDATA2 const f32 lbl_8047CB1C = 1.0f;
SDATA2 const f32 lbl_8047CB20 = 180.0f;
SDATA2 const f32 lbl_8047CB24 = -1.0f;
SDATA2 const f64 lbl_8047CB28 = 180.0;
SDATA2 const f32 lbl_8047CB30 = 0.5f;
SDATA2 const f32 lbl_8047CB34 = 2.0f;
SDATA2 const f32 lbl_8047CB38 = 0.01745329238474369f;
SDATA2 const f64 lbl_8047CB40 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047CB48 = 100.0f;
SDATA2 const f64 lbl_8047CB50 = 4.503599627370496e+15;
SDATA2 const f64 lbl_8047CB58 = 4.503599627370496e+15;
SDATA2 const u8 lbl_8047CB60[7] = "jobj.h";
SDATA2 const u8 lbl_8047CB68[5] = "jobj";
SDATA2 const u8 lbl_8047CB70[6] = "scale";
SDATA2 const u8 lbl_8047CB78[4] = "mtx";
SDATA2 const f32 lbl_8047CB7C = 0.0f;
SDATA2 const f32 lbl_8047CB80 = 1.0f;
SDATA2 const f32 lbl_8047CB84 = 0.5f;
