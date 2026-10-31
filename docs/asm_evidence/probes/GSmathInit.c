/* Closest C candidate for the compiler probe (see docs/asm_evidence/gs_math_init.md).
 * C has no way to write the paired-single quantisation registers GQR2-GQR5
 * (mtspr 914-917); the nearest C keeps the two calls and stores the GQR
 * values through an ordinary variable, which MWCC compiles to plain stores. */
#include "dolphin/types.h"

extern void GSsrandTime(void);
extern void GSmathInitCosTable(void);

volatile u32 GSmathInit_gqr[4];

void GSmathInit(void) {
    GSmathInit_gqr[0] = 0x00040004;
    GSmathInit_gqr[1] = 0x00050005;
    GSmathInit_gqr[2] = 0x00060006;
    GSmathInit_gqr[3] = 0x00070007;
    GSsrandTime();
    GSmathInitCosTable();
}
