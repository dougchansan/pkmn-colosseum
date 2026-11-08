#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * gs_material .sdata2 constants from 0x8047CBE8 through 0.0001f (the bias
 * double at 0x8047CBE0 is modelShadowRender's literal pool). The int-to-float biases at 0x8047CC80/0x8047CC88 are the
 * literal pool of game/gs_model_anim_suffix_candidate_800ED8C4.c
 * (modelCalcAnimFrame's conversions); the aobj.h string follows in sdata2_8047CC90.c.
 */
SDATA2 const u8 lbl_8047CBE8[7] = "jobj.h";
SDATA2 const u8 lbl_8047CBF0[5] = "jobj";
SDATA2 const u8 lbl_8047CBF8[6] = "scale";
SDATA2 const u8 lbl_8047CC00[7] = "jobj.h";
SDATA2 const u8 lbl_8047CC08[5] = "jobj";
SDATA2 const u8 lbl_8047CC10[8] = "parse.c";
SDATA2 const f32 lbl_8047CC18 = 1.0f;
SDATA2 const f32 lbl_8047CC1C = 0.0f;
SDATA2 const u8 lbl_8047CC20[8] = "jp->mtx";
SDATA2 const f32 lbl_8047CC28 = 0.0f;
SDATA2 const u8 lbl_8047CC2C[7] = "jobj.h";
SDATA2 const u8 lbl_8047CC34[12] = "jobj";
SDATA2 const u8 lbl_8047CC40[7] = "jobj.h";
SDATA2 const u8 lbl_8047CC48[5] = "jobj";
SDATA2 const f32 lbl_8047CC50[2] = { 1.0f, 0.0f };
SDATA2 const f32 lbl_8047CC58 = 1.2f;
SDATA2 const f32 lbl_8047CC5C = 0.0f;
SDATA2 const f32 lbl_8047CC60 = 1.0f;
SDATA2 const f32 lbl_8047CC64 = -1.0f;
SDATA2 const u8 lbl_8047CC68[7] = "jobj.h";
SDATA2 const u8 lbl_8047CC70[5] = "jobj";
SDATA2 const f32 lbl_8047CC78 = 0.0001f;
