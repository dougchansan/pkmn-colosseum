#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * effect_visual .sdata2 constants after _distortionUpdate's pool (0x8047D310 -
 * 0x8047D328), which links with that function. Source references for this
 * range are in src/game/effect/effect_visual.c; the target slice has no
 * relocations.
 */
SDATA2 const f32 lbl_8047D328 = 0.5f;
SDATA2 const f32 lbl_8047D32C = 0.0f;
SDATA2 const f32 lbl_8047D330 = 1.100000023841858f;
SDATA2 const f32 lbl_8047D334 = 0.01745329238474369f;
SDATA2 const f32 lbl_8047D338[2] = { 2.0f, 0.0f };
SDATA2 const f32 lbl_8047D340 = 3.4000000953674316f;
SDATA2 const f64 lbl_8047D348 = 4.503601774854144e+15;
