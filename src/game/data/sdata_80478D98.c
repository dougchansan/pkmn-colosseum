#include "dolphin/types.h"

#define SDATA __declspec(section ".sdata")
#define BYTE_ALIGNED __attribute__((aligned(1)))

#pragma force_active on

/* Independently reconstructed globals, 0x80478D98..0x80478DB4.
 * See docs/recon/sdata_80478D08_reconstruction.md for typed-use evidence. */

/* toolentryDebugPokemonCreate prepares four parties of six Pokemon. */
SDATA s32 lbl_80478D98 = 4 * 6;
SDATA u8 padding_80478D9C[4] BYTE_ALIGNED = {0};

/* effectPlayWaza and loadSequence consume (group, type) u16 pairs.
 * wazaSequenceResolveResource validates type 1..4 and the source-backed group bounds. */
SDATA u16 lbl_80478DA0[2] = {100, 3};
SDATA u8 padding_80478DA4[4] BYTE_ALIGNED = {0};
SDATA u16 lbl_80478DA8[4] = {77, 4, 122, 4};
SDATA u16 lbl_80478DB0[2] = {122, 4};

#pragma force_active reset
