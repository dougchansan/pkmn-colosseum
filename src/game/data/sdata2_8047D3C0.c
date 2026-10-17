#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

typedef void (*Sdata2FuncPtr)(void);

#if !defined(SDATA2_8047D3C0_ISOLATED)
#define SDATA2_8047D3C0_PREFIX
#define SDATA2_8047D3C0_SYNTHMACROS_LEGACY
#define SDATA2_8047D3C0_AFTER_SNDMATH
#endif

extern void _soundMemFree(void);
extern void _soundMemAlloc(void);

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

/* musyx/runtime/synth_ac.c owns 0x8047D3F0 - 0x8047D408 and
 * musyx/runtime/synth_adsr.c owns 0x8047D408 - 0x8047D430. */
/* musyx/runtime/hw_volconv.c owns 0x8047D430 - 0x8047D468. */

/* musyx/runtime/snd3d.c owns 0x8047D468 - 0x8047D4B8. */

/* musyx/runtime/snd_math.c owns 0x8047D4B8 - 0x8047D4D8. */

/* musyx/runtime/hardware.c owns 0x8047D4D8 - 0x8047D4F0. */

/* reverb_candidate_80164520.c owns 0x8047D4F0 - 0x8047D540 (complete reverb.c pool). */

#if defined(SDATA2_8047D3C0_AFTER_SNDMATH)
SDATA2 const f64 lbl_8047D528 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047D530 = 0.6f;
SDATA2 const f32 lbl_8047D534 = 0.3f;
SDATA2 const f32 lbl_8047D538[2] = { 0.5f, 0.0f };
SDATA2 const f32 lbl_8047D540 = 0.0f;
SDATA2 const f32 lbl_8047D544 = 1000.0f;
#endif
