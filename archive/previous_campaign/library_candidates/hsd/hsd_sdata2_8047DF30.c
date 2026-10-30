#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * HAL video.c's .sdata2: its __FILE__ string and the 1.0F copy scale
 * (0x8047DF30-0x8047DF40). Kept as a data unit while video.c's text is
 * carved around fn_801BF8A0 (see src/hsd/video.c).
 */
SDATA2 const u8 lbl_8047DF30[8] = "video.c";
SDATA2 const f32 lbl_8047DF38[2] = { 1.0f, 0.0f };
