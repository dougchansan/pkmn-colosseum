/** Byte-exact PDA angle reset, 0x80038A00 - 0x80038A0C. */
#include "dolphin/types.h"

extern f32 lbl_8047A484;
extern f32 lbl_8047BA58;

void pcResetTimer8047A484(void)
{
    lbl_8047A484 = lbl_8047BA58;
}
