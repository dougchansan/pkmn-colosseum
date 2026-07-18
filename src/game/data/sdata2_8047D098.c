#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed game/effect .sdata2 constants. Text relocations reference the first
 * run from gs_field_world.o, the movie string/effect setup constants from
 * effect_util.o. The compiler-owned integer-conversion literal and following
 * constants are split with their owning source and the trailing data unit.
 */
SDATA2 const f32 lbl_8047D098 = -6.283184051513672f;
SDATA2 const f32 lbl_8047D09C = -3.141592025756836f;
SDATA2 const f32 lbl_8047D0A0 = 3.141592025756836f;
SDATA2 const f32 lbl_8047D0A4 = 0.0017453288892284036f;
SDATA2 const f32 lbl_8047D0A8 = 1.0000000116860974e-07f;
SDATA2 const f32 lbl_8047D0AC = 9.0f;
SDATA2 const f32 lbl_8047D0B0 = 100.0f;
SDATA2 const f32 lbl_8047D0B4 = 28.0f;
SDATA2 const f32 lbl_8047D0B8 = 0.0010000000474974513f;
SDATA2 const f32 lbl_8047D0BC = 1.5707963705062866f;
SDATA2 const f32 lbl_8047D0C0 = 3.183098793029785f;
SDATA2 const f32 lbl_8047D0C4 = 3.1415927410125732f;
SDATA2 const f64 lbl_8047D0C8 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047D0D0 = 0.20000000298023224f;
SDATA2 const f32 lbl_8047D0D4 = 12.0f;
SDATA2 const f32 lbl_8047D0D8 = -1000000.0f;
SDATA2 const f64 lbl_8047D0E0 = 4.503601774854144e+15;
SDATA2 const u8 lbl_8047D0E8[6] = "movie";
SDATA2 const f32 lbl_8047D0F0 = 0.0f;
SDATA2 const f32 lbl_8047D0F4 = 640.0f;
SDATA2 const f32 lbl_8047D0F8 = 480.0f;
SDATA2 const f32 lbl_8047D0FC = 100.0f;
SDATA2 const f32 lbl_8047D100 = 380.0f;
SDATA2 const f32 lbl_8047D104 = 200.0f;
