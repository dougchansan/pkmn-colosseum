/**
 * @file psdisptev.c
 * @brief HAL's particle TEV set-up (sysdolphin psdisptev.c) in the Genius
 *        Sonority fork, 0x8016EC1C - 0x8016F430.
 *
 * The whole translation unit:
 *   .text  0x8016EC1C - 0x8016F430  psSetupTev, psSetupTevInvalidState,
 *          psSetupTevCommon (reverse definition order: the library is built
 *          with -inline deferred)
 *   .sbss  0x8047B170 - 0x8047B178  prevTev
 * psdisp.c's .text ends at 0x8016EC1C (psSetFog) and psinterpret.c's starts
 * at 0x8016F430 (psInterpretParticles); psdisp.c's .sbss ends at 0x8047B170
 * and psinterpret.c's float scratch is at 0x8047B178. The unit has no
 * other data.
 *
 * Reference: Melee's sysdolphin/baselib/psdisptev.c (doldecomp/melee), which
 * the fork keeps unchanged except for prevTev: Colosseum's is a single u32.
 * With Melee's `static u32 prevTev[2]` every MWCC from 1.3 to 2.7 reuses the
 * particle's kind word loaded for the early-out test inside the four
 * "clear PrimEnv" cases (the array store does not kill it); retail reloads
 * pp->kind there, which is what the scalar gives (psSetupTev 98.7 -> 100%).
 * Its .sbss slot is 4 bytes; the next 4 are section padding.
 *
 * Compiler: the particle library flags (pslist.c, particle.c,
 * generator.c): GC/1.3.2 -O4,p -inline auto,deferred -use_lmw_stmw on
 * -sdata 8 -sdata2 8 -str reuse,readonly, unit-wide, no local pragmas.
 *
 * The GX entry points are declared with their address names (symbols.txt)
 * and mapped to the SDK names used by the code.
 */
#include "dolphin/types.h"
#include "sysdolphin/baselib/psstructs.h"

/* GX (dolphin/gx) enum values used here. */
enum {
    GX_TEVSTAGE0 = 0,
    GX_TEVSTAGE1 = 1,
    GX_TEVSTAGE2 = 2,
    GX_TEXCOORD0 = 0,
    GX_TEXCOORD_NULL = 0xFF,
    GX_TEXMAP0 = 0,
    GX_TEXMAP_NULL = 0xFF,
    GX_COLOR0A0 = 4,
    GX_MODULATE = 0,
    GX_TEV_ADD = 0,
    GX_TB_ZERO = 0,
    GX_CS_SCALE_1 = 0,
    GX_TEVPREV = 0,
    GX_TEV_SWAP0 = 0
};
enum {
    GX_CC_CPREV = 0,
    GX_CC_C0 = 2,
    GX_CC_C1 = 4,
    GX_CC_C2 = 6,
    GX_CC_TEXC = 8,
    GX_CC_RASC = 10,
    GX_CC_ZERO = 15
};
enum {
    GX_CA_APREV = 0,
    GX_CA_A0 = 1,
    GX_CA_A1 = 2,
    GX_CA_A2 = 3,
    GX_CA_TEXA = 4,
    GX_CA_RASA = 5,
    GX_CA_ZERO = 7
};

extern void GXSetNumTexGens(u8 num);                                    /* GXSetNumTexGens */
extern void GXSetNumTevStages(u8 num);                                    /* GXSetNumTevStages */
extern void GXSetTevOrder(u32 stage, u32 coord, u32 map, u32 color);  /* GXSetTevOrder */
extern void GXSetTevOp(u32 stage, u32 mode);
extern void GXSetTevSwapMode(u32 stage, u32 ras_sel, u32 tex_sel);       /* GXSetTevSwapMode */
extern void GXSetTevColorOp(u32 stage, u32 op, u32 bias, u32 scale, u8 clamp,
                        u32 out_reg);                               /* GXSetTevColorOp */
extern void GXSetTevAlphaOp(u32 stage, u32 op, u32 bias, u32 scale, u8 clamp,
                        u32 out_reg);                               /* GXSetTevAlphaOp */
extern void GXSetTevColorIn(u32 stage, u32 a, u32 b, u32 c, u32 d);     /* GXSetTevColorIn */
extern void GXSetTevAlphaIn(u32 stage, u32 a, u32 b, u32 c, u32 d);     /* GXSetTevAlphaIn */

#define GXSetNumTexGens GXSetNumTexGens
#define GXSetNumTevStages GXSetNumTevStages

u32 lbl_8047B170; /* prevTev: the last TEV configuration set up */

#define prevTev lbl_8047B170

void psSetupTevCommon(void)
{
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1,
                    GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1,
                    GX_TEVPREV);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1,
                    GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1,
                    GX_TEVPREV);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1,
                    GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1,
                    GX_TEVPREV);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
}

void psSetupTevInvalidState(void)
{
    prevTev = -1;
}

void psSetupTev(HSD_Particle* pp)
{
    u32 kind = pp->kind & 0x80100480;

    if (kind == prevTev) {
        return;
    }

    prevTev = kind;
    switch (prevTev) {
    case 0x80000080:
        pp->kind &= ~0x80;
        prevTev &= ~0x80;
    case 0x80000000:
        GXSetNumTevStages(2);
        GXSetNumTexGens(0);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL,
                      GX_COLOR0A0);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C2, GX_CC_RASC,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A2, GX_CA_ZERO, GX_CA_ZERO,
                        GX_CA_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV,
                        GX_CA_ZERO);
        break;

    case 0x80:
        pp->kind &= ~0x80;
        prevTev &= ~0x80;
    case 0x0:
        GXSetNumTevStages(1);
        GXSetNumTexGens(0);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL,
                      GX_COLOR0A0);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_ZERO, GX_CC_ZERO,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A0, GX_CA_ZERO, GX_CA_ZERO,
                        GX_CA_ZERO);
        break;

    case 0x80000480:
        GXSetNumTevStages(3);
        GXSetNumTexGens(1);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A1, GX_CA_A0, GX_CA_TEXA,
                        GX_CA_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C2, GX_CC_CPREV,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A2, GX_CA_APREV,
                        GX_CA_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_RASC, GX_CC_CPREV,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_APREV, GX_CA_ZERO, GX_CA_ZERO,
                        GX_CA_ZERO);
        break;

    case 0x80000400:
        GXSetNumTevStages(2);
        GXSetNumTexGens(1);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C2, GX_CC_RASC,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A2, GX_CA_ZERO, GX_CA_ZERO,
                        GX_CA_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_CPREV,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_APREV,
                        GX_CA_ZERO);
        break;

    case 0x400:
    case 0x480:
        GXSetNumTevStages(1);
        GXSetNumTexGens(1);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A1, GX_CA_A0, GX_CA_TEXA,
                        GX_CA_ZERO);
        break;

    case 0x100400:
        GXSetNumTevStages(1);
        GXSetNumTexGens(1);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
        break;

    case 0x100480:
        GXSetNumTevStages(2);
        GXSetNumTexGens(1);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A1, GX_CA_A0, GX_CA_TEXA,
                        GX_CA_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_RASA, GX_CA_APREV,
                        GX_CA_ZERO);
        break;

    case 0x80100400:
        GXSetNumTevStages(2);
        GXSetNumTexGens(1);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C2, GX_CC_RASC,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A2, GX_CA_RASA,
                        GX_CA_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_CPREV,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_APREV,
                        GX_CA_ZERO);
        break;

    case 0x80100480:
        GXSetNumTevStages(3);
        GXSetNumTexGens(1);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A1, GX_CA_A0, GX_CA_TEXA,
                        GX_CA_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C2, GX_CC_CPREV,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A2, GX_CA_APREV,
                        GX_CA_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_RASC, GX_CC_CPREV,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_RASA, GX_CA_APREV,
                        GX_CA_ZERO);
        break;

    case 0x100080:
        pp->kind &= ~0x80;
        prevTev &= ~0x80;
    case 0x100000:
        GXSetNumTevStages(1);
        GXSetNumTexGens(0);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL,
                      GX_COLOR0A0);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_RASC, GX_CC_ZERO, GX_CC_ZERO,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_RASA, GX_CA_ZERO, GX_CA_ZERO,
                        GX_CA_ZERO);
        break;

    case 0x80100080:
        pp->kind &= ~0x80;
        prevTev &= ~0x80;
    case 0x80100000:
        GXSetNumTevStages(2);
        GXSetNumTexGens(0);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL,
                      GX_COLOR0A0);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C2, GX_CC_RASC,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A2, GX_CA_RASA,
                        GX_CA_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV,
                        GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV,
                        GX_CA_ZERO);
        break;
    }
}
