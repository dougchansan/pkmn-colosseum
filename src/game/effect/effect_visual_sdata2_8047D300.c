#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * effect_visual .sdata2 constants shared by distortionEffectStart,
 * _distortionRender and the _distortionUpdate unit. Source references are in
 * src/game/effect/effect_visual.c.
 */
SDATA2 const f32 lbl_8047D300 = 0.0f;
SDATA2 const f32 lbl_8047D304 = -1.0f;
SDATA2 const f32 lbl_8047D308 = 1.0f;
