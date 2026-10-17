#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Numeric .sdata2 run after menuOffScreenDraw's pool (0x8047CE48-0x8047CE70, owned
 * by menu_offscreen.c). Text relocations reference it from
 * gs_range_80109C88.c.
 */
SDATA2 const f32 lbl_8047CE70 = 0.5f;
SDATA2 const f32 lbl_8047CE74 = 10.0f;
SDATA2 const f32 lbl_8047CE78 = -0.08726646f;
SDATA2 const f32 lbl_8047CE7C = 0.2617994f;
SDATA2 const f32 lbl_8047CE80 = 0.0f;
SDATA2 const f32 lbl_8047CE84 = 0.8f;
SDATA2 const f32 lbl_8047CE88 = 0.9f;
SDATA2 const f32 lbl_8047CE8C = 1.0f;
SDATA2 const f32 lbl_8047CE90 = 1.1f;
SDATA2 const f32 lbl_8047CE94 = 1.15f;
