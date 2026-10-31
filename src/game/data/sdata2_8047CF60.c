#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * The 0.0f / 1.0f literal pool of the TU after GScolsys2Thru
 * (GScolsys2ThruTest, GScolsys2Thru_r56_80111864_o4s.c), which reads it by name
 * while that unit is unlinked. 0x8047CF48-0x8047CF60 is owned by
 * GScolsys2Thru.c; 0x8047CF68-0x8047CF70 is GScolsys2Sun.c's pool and
 * 0x8047CF70-0x8047CFA0 is floor.c's, each owned by its unit.
 */
SDATA2 const f32 lbl_8047CF60 = 0.0f;
SDATA2 const f32 lbl_8047CF64 = 1.0f;
