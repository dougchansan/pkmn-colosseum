/**
 * @file DVD_exact_800A76E4.c
 * @brief Exact DVD reset routine, 0x800A76E4 - 0x800A7728.
 */

#include "dolphin/types.h"
#include "dolphin/dvd/dvd.h"

volatile u32 __DIRegs[16] : 0xCC006000;
extern u32 ResetRequired_8047A820;
extern u32 ResumeFromHere_8047A810;

void DVDReset(void)
{
    DVDLowReset();
    __DIRegs[0] = 0x2A;
    __DIRegs[1] = __DIRegs[1];
    ResetRequired_8047A820 = 0;
    ResumeFromHere_8047A810 = 0;
}
