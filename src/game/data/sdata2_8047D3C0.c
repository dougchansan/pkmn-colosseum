#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

typedef void (*Sdata2FuncPtr)(void);

#if !defined(SDATA2_8047D3C0_ISOLATED)
#define SDATA2_8047D3C0_PREFIX
#define SDATA2_8047D3C0_SYNTHMACROS_LEGACY
#define SDATA2_8047D3C0_SUFFIX
#endif

extern void fn_80167B70(void);
extern void fn_80167BB0(void);

/*
 * Mixed audio/DVD .sdata2 constants. Target relocations tie the run to JAudio
 * parameter helpers, SAL vector/matrix helpers, people-field volume scaling,
 * reverb/sound listener setup, GSDVD version/assert strings, and the adjacent
 * particle script/display constants.
 */
#if defined(SDATA2_8047D3C0_PREFIX)
SDATA2 const f32 lbl_8047D3C0[2] = { 4096.0f, 0.0f };
#endif

#if defined(SDATA2_8047D3C0_SYNTHMACROS_LEGACY)
SDATA2 const f32 lbl_8047D3C8 = 4096.0f;
SDATA2 const f32 lbl_8047D3CC = 1.1920928955078125e-07f;
SDATA2 const f32 lbl_8047D3D0 = 0.0078125f;
SDATA2 const f64 lbl_8047D3D8 = 4.503599627370496e+15;
SDATA2 const f64 lbl_8047D3E0 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047D3E8 = 1023.0f;
SDATA2 const f32 lbl_8047D3EC = 1.0f;
#endif

#if defined(SDATA2_8047D3C0_SUFFIX)
SDATA2 const f32 lbl_8047D3F0 = 1.0594631433486938f;
SDATA2 const f64 lbl_8047D3F8 = 4.503599627370496e+15;
SDATA2 const f32 lbl_8047D400 = 4096.0f;
SDATA2 const f64 lbl_8047D408 = 2.0;
SDATA2 const f32 lbl_8047D410 = 1.2715657859985185e-08f;
SDATA2 const f32 lbl_8047D414 = 1000.0f;
SDATA2 const f64 lbl_8047D418 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047D420 = 0.00032383418874815106f;
SDATA2 const f64 lbl_8047D428 = 4.503599627370496e+15;
SDATA2 const f32 lbl_8047D430 = 127.0f;
SDATA2 const f32 lbl_8047D434 = 1.0f;
SDATA2 const f32 lbl_8047D438 = 0.7078999876976013f;
SDATA2 const f64 lbl_8047D440 = 4.503599627370496e+15;
SDATA2 const f32 lbl_8047D448 = 2.422030149773491e-07f;
SDATA2 const f64 lbl_8047D450 = 1.0;
SDATA2 const f32 lbl_8047D458 = 2.0f;
SDATA2 const f32 lbl_8047D45C = 0.766040027141571f;
SDATA2 const f32 lbl_8047D460[2] = { 0.0f, 0.0f };
SDATA2 const f32 lbl_8047D468 = 0.0f;
SDATA2 const f64 lbl_8047D470 = 4.503599627370496e+15;
SDATA2 const f32 lbl_8047D478 = -1.0f;
SDATA2 const f32 lbl_8047D47C = 1.2014794492642977e-07f;
SDATA2 const f64 lbl_8047D480 = 0.5;
SDATA2 const f32 lbl_8047D488 = 127.0f;
SDATA2 const f32 lbl_8047D48C = 1.0f;
SDATA2 const f64 lbl_8047D490 = 3.0;
SDATA2 const f32 lbl_8047D498 = 0.016666668f;
SDATA2 const f32 lbl_8047D49C = 64.0f;
SDATA2 const f32 lbl_8047D4A0 = 8192.0f;
SDATA2 const f32 lbl_8047D4A4 = 0.0078740157f;
SDATA2 const f32 lbl_8047D4A8 = 0.08f;
SDATA2 const f32 lbl_8047D4AC = 0.15f;
SDATA2 const f32 lbl_8047D4B0[2] = { 0.3f, 0.0f };
SDATA2 const f32 lbl_8047D4B8 = 0.0f;
SDATA2 const f64 lbl_8047D4C0 = 0.5;
SDATA2 const f64 lbl_8047D4C8 = 3.0;
SDATA2 const f32 lbl_8047D4D0[2] = { 1.0f, 0.0f };
SDATA2 const f32 lbl_8047D4D8 = 1.0f;
SDATA2 const f32 lbl_8047D4DC = 32767.0f;
SDATA2 const f32 lbl_8047D4E0 = 4096.0f;
SDATA2 const f64 lbl_8047D4E8 = 4.503599627370496e+15;
SDATA2 const f32 lbl_8047D4F0 = 0.0f;
SDATA2 const f32 lbl_8047D4F4 = 1.0f;
SDATA2 const f32 lbl_8047D4F8 = 0.01f;
SDATA2 const f32 lbl_8047D4FC = 10.0f;
SDATA2 const f32 lbl_8047D500 = 0.1f;
SDATA2 const f32 lbl_8047D504 = 32000.0f;
SDATA2 const f64 lbl_8047D508 = 10.0;
SDATA2 const f32 lbl_8047D510 = 0.05f;
SDATA2 const f32 lbl_8047D514 = 0.8f;
SDATA2 const f64 lbl_8047D518 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047D520 = 100.0f;
SDATA2 const f64 lbl_8047D528 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047D530 = 0.6f;
SDATA2 const f32 lbl_8047D534 = 0.3f;
SDATA2 const f32 lbl_8047D538[2] = { 0.5f, 0.0f };
SDATA2 const f32 lbl_8047D540 = 0.0f;
SDATA2 const f32 lbl_8047D544 = 1000.0f;
#endif
