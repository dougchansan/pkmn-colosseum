/* oracle_runtime.h - shared full-program A-side runtime for the oracle.
 *
 * Provides:
 *   - prototypes for all 154 DolRecomp chunks (oracle_chunk_protos.inc)
 *   - a 154-entry chunk dispatch table          (oracle_chunk_table.inc)
 *   - DOL section preload into guest RAM
 *   - SDA bases (r2/r13) recovered from __init_registers
 *   - a sentinel-LR driver that runs across ALL chunks (Tier-2 reach: deep
 *     call trees execute their REAL callees, not just leaves)
 *
 * The two .inc files are generated on the box from generated.h (see
 * build_full.sh / oracle_auto.sh) and live next to libcolo.a; they are NOT
 * checked in (machine-generated, large-ish). Link against libcolo.a.
 */
#ifndef ORACLE_RUNTIME_H
#define ORACLE_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "core/cpu.h"
#include "frontend/dol.h"

/* all 154 chunk entry prototypes */
#include "oracle_chunk_protos.inc"

#define ORC_SENTINEL  0x0BADF00Du
#define ORC_GUEST_BASE 0x80000000u
#define ORC_SDA_R13   0x80480820u   /* _SDA_BASE_  (from __init_registers) */
#define ORC_SDA_R2    0x804836A0u   /* _SDA2_BASE_ */

typedef struct { u32 lo, hi; void (*fn)(CPUState*); } OrcChunk;
static const OrcChunk ORC_CHUNKS[] = {
#include "oracle_chunk_table.inc"
};
#define ORC_NCHUNKS (sizeof(ORC_CHUNKS)/sizeof(ORC_CHUNKS[0]))

/* Dispatch one block at ctx->pc; returns 1 if a chunk owned it, else 0. */
static int orc_dispatch(CPUState* ctx) {
    u32 pc = ctx->pc;
    for (unsigned i = 0; i < ORC_NCHUNKS; i++) {
        if (pc >= ORC_CHUNKS[i].lo && pc < ORC_CHUNKS[i].hi &&
            ((pc - ORC_CHUNKS[i].lo) & 3u) == 0u) {
            ORC_CHUNKS[i].fn(ctx);
            return 1;
        }
    }
    return 0;
}

/* Run from ctx->pc until it returns to a PC owned by no chunk (the sentinel),
 * an exception, or the block budget is exhausted (runaway / Tier-2 too deep).
 * Returns: 0 = returned cleanly, -1 = exception, -2 = runaway/unreachable. */
static int orc_run(CPUState* ctx, unsigned max_blocks) {
    for (unsigned i = 0; i < max_blocks; i++) {
        if (!orc_dispatch(ctx)) return 0;            /* hit sentinel */
        if (ctx->exception) return -1;
    }
    return -2;                                       /* budget exhausted */
}

/* Preload all DOL code+data sections into guest RAM (jumptables/rodata/sdata2). */
static void orc_preload(CPUState* cpu, const DOLFile* dol) {
    for (int i = 0; i < DOL_NUM_TEXT; i++) {
        u32 a = dol->header.text_addresses[i], s = dol->header.text_sizes[i];
        const u8* d = dol_get_text_section(dol, i);
        if (d && s) for (u32 o = 0; o < s; o++) mem_write8(cpu, a + o, d[o]);
    }
    for (int i = 0; i < DOL_NUM_DATA; i++) {
        u32 a = dol->header.data_addresses[i], s = dol->header.data_sizes[i];
        const u8* d = dol_get_data_section(dol, i);
        if (d && s) for (u32 o = 0; o < s; o++) mem_write8(cpu, a + o, d[o]);
    }
}

/* Reset CPU, preload DOL, set SDA bases + stack. Call before each invocation. */
static void orc_prep(CPUState* cpu, const DOLFile* dol) {
    cpu_reset(cpu);
    orc_preload(cpu, dol);
    cpu->gpr[1]  = 0x80300000u;     /* stack pointer, mid-RAM */
    cpu->gpr[2]  = ORC_SDA_R2;
    cpu->gpr[13] = ORC_SDA_R13;
    /* Enable Gekko paired-single mode (HID2.PSE|LSQE), as the GC boot ROM does.
     * Without this, psq_l/psq_st raise an illegal-instruction (0x700) exception
     * in float-heavy library code (e.g. the tan helper fn_800CD648). */
    cpu->hid2 |= PPC_HID2_PSE | PPC_HID2_LSQE;
}

#endif /* ORACLE_RUNTIME_H */
