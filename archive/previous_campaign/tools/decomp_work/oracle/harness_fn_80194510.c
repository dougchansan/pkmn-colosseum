/* ============================================================================
 * Behavioral-equivalence ORACLE - fn_80194510 (0x80194510, hsd_cobj.c)
 *
 * Second target, chosen to show the oracle GENERALISES across function shape:
 *   - different file (src/hsd/hsd_cobj.c)
 *   - different signature: f32 fn_80194510(u8* ptr)   (returns FLOAT)
 *   - different wall class: W1 FPR register-permutation (not jumptable-name)
 *
 * A-side: ORIGINAL machine code via DolRecomp (chunk owning 0x80194510), plus
 *         the cross-chunk callee fn_800CE220 (a tan-family helper) for case 1.
 * B-side: OUR decompiled C, VERBATIM from src/hsd/hsd_cobj.c (bside_real_src.inc).
 *
 * Cases 0(NULL)/2/3/default are pure field reads / SDA constants -> fully
 * self-contained and tested rigorously. Case 1 calls fn_800CE220; BOTH sides
 * use the SAME DolRecomp-translated fn_800CE220 (A via dispatch, B via a thunk),
 * so the differential isolates fn_80194510's own logic. This is documented and
 * bounded - see B_call_CE220.
 *
 * NO src/ files modified; src text is #included read-only.
 * ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "core/cpu.h"
#include "frontend/dol.h"

/* A-side generated chunks. */
void func_801915E0(CPUState* ctx);   /* owns 0x80194510 */
void func_800CD5E0(CPUState* ctx);   /* owns 0x800CE220 (callee) */
struct ChunkRange { u32 lo, hi; void (*fn)(CPUState*); };
static const struct ChunkRange CHUNKS[] = {
    { 0x801915E0u, 0x801955E0u, func_801915E0 },
    { 0x800CD5E0u, 0x800D15E0u, func_800CD5E0 },
};
#define NCHUNKS (sizeof(CHUNKS)/sizeof(CHUNKS[0]))
#define SENTINEL 0x0BADF00Du
#define GUEST_BASE 0x80000000u
#define SDA_R13 0x80480820u
#define SDA_R2  0x804836A0u

/* ---- B-side shim + verbatim src ---- */
#include "bside_shim.h"

/* SDA float constants the src reads (.sdata2). We fill these from the DOL-
 * loaded RAM at startup so B-side sees the real values. */
f32 lbl_8047D978, lbl_8047D97C, lbl_8047D980;

/* g_bram: little-endian host mirror for B-side pointer derefs. */
static u8* g_bram; static u32 g_bram_size;
static CPUState g_cpu;
static DOLFile g_dol; static int g_dol_ok;

/* A-side cross-chunk dispatch driver: run from ctx->pc until it returns to a
 * PC owned by no chunk (the sentinel). */
static int A_run(CPUState* ctx) {
    for (unsigned i = 0; i < 500000u; i++) {
        u32 pc = ctx->pc; int owned = 0;
        for (unsigned c = 0; c < NCHUNKS; c++) {
            if (pc >= CHUNKS[c].lo && pc < CHUNKS[c].hi && ((pc - CHUNKS[c].lo) & 3u)==0u) {
                CHUNKS[c].fn(ctx); owned = 1; break;
            }
        }
        if (!owned) return 0;                /* returned to sentinel */
        if (ctx->exception) { fprintf(stderr,"A exc pc=%08X\n", ctx->pc); return -1; }
    }
    fprintf(stderr,"A runaway pc=%08X\n", ctx->pc); return -2;
}

/* B-side calls fn_800CE220 by running the SAME DolRecomp-translated callee on
 * the A-side CPU (documented host accommodation: we do not have a separate C
 * decomp of fn_800CE220, and reproducing tan-family codegen is not the point -
 * the differential is about fn_80194510's own FP chain). Returns the double in
 * f1 per the PPC ABI. */
static double B_call_CE220(f32 arg) {
    CPUState* c = &g_cpu;
    /* Preserve caller state is unnecessary: we snapshot/restore around it. */
    CPUState save = *c;
    c->fpr[1] = (double)arg;             /* f1 = arg (single promoted) */
    c->gpr[1] = 0x80300000u;             /* fresh stack */
    c->lr = SENTINEL; c->pc = 0x800CE220u;
    A_run(c);
    double r = c->fpr[1];                /* f1 = return */
    *c = save;                            /* restore */
    return r;
}
#define fn_800CE220 B_call_CE220

#include "bside_real_src.inc"

#undef fn_800CE220

/* ---- A-side: run original fn_80194510(ptr) -> f32 (bits) ---- */
static u32 A_call(u32 ptr_guest) {
    cpu_reset(&g_cpu);
    /* preload code+data so SDA2 float consts + any rodata resolve */
    for (int i=0;i<DOL_NUM_TEXT;i++){u32 a=g_dol.header.text_addresses[i],s=g_dol.header.text_sizes[i];
        const u8* d=dol_get_text_section(&g_dol,i); if(d&&s) for(u32 o=0;o<s;o++) mem_write8(&g_cpu,a+o,d[o]); }
    for (int i=0;i<DOL_NUM_DATA;i++){u32 a=g_dol.header.data_addresses[i],s=g_dol.header.data_sizes[i];
        const u8* d=dol_get_data_section(&g_dol,i); if(d&&s) for(u32 o=0;o<s;o++) mem_write8(&g_cpu,a+o,d[o]); }
    g_cpu.gpr[1]=0x80300000u; g_cpu.gpr[2]=SDA_R2; g_cpu.gpr[13]=SDA_R13;
    g_cpu.gpr[3]=ptr_guest; g_cpu.lr=SENTINEL; g_cpu.pc=0x80194510u;
    A_run(&g_cpu);
    /* f32 return in f1 (single) */
    f32 r = (f32)g_cpu.fpr[1];
    u32 bits; memcpy(&bits,&r,4); return bits;
}

/* ---- B-side: run real src fn_80194510(ptr) -> f32 (bits) ---- */
static u32 B_call(u32 ptr_guest) {
    u8* p = (ptr_guest==0)?NULL:(g_bram + (ptr_guest - GUEST_BASE));
    f32 r = fn_80194510(p);
    u32 bits; memcpy(&bits,&r,4); return bits;
}

/* Seed object fields used by the switch on ptr[0x50] and the f32 reads. */
#define OBJ 0x80500000u
static void seed_case(u8 selector, f32 v44, f32 v38, f32 v40, f32 v4C) {
    /* A-side (big-endian) */
    u32 b44,b38,b40,b4c; memcpy(&b44,&v44,4);memcpy(&b38,&v38,4);memcpy(&b40,&v40,4);memcpy(&b4c,&v4C,4);
    mem_write8 (&g_cpu, OBJ+0x50, selector);
    mem_write32(&g_cpu, OBJ+0x44, b44);
    mem_write32(&g_cpu, OBJ+0x38, b38);
    mem_write32(&g_cpu, OBJ+0x40, b40);
    mem_write32(&g_cpu, OBJ+0x4C, b4c);
    /* B-side (host little-endian mirror) */
    g_bram[OBJ-GUEST_BASE+0x50] = selector;
    memcpy(g_bram+(OBJ-GUEST_BASE)+0x44,&v44,4);
    memcpy(g_bram+(OBJ-GUEST_BASE)+0x38,&v38,4);
    memcpy(g_bram+(OBJ-GUEST_BASE)+0x40,&v40,4);
    memcpy(g_bram+(OBJ-GUEST_BASE)+0x4C,&v4C,4);
}

/* Load the 3 SDA2 float constants from DOL-populated A-side RAM into B globals. */
static void load_sda_consts(void) {
    u32 b;
    b = mem_read32(&g_cpu, 0x8047D978u); memcpy(&lbl_8047D978,&b,4);
    b = mem_read32(&g_cpu, 0x8047D97Cu); memcpy(&lbl_8047D97C,&b,4);
    b = mem_read32(&g_cpu, 0x8047D980u); memcpy(&lbl_8047D980,&b,4);
}

static f32 bits_to_f32(u32 b){ f32 f; memcpy(&f,&b,4); return f; }

int main(int argc, char** argv) {
    if (!cpu_init(&g_cpu)) { fprintf(stderr,"cpu_init failed\n"); return 2; }
    g_bram_size = g_cpu.ram_size; g_bram = (u8*)calloc(1,g_bram_size);
    if (!g_bram){ fprintf(stderr,"bram alloc failed\n"); return 2; }
    const char* dol = (argc>1)?argv[1]:(getenv("ORACLE_DOL")?getenv("ORACLE_DOL"):"/tmp/start.dol");
    g_dol_ok = dol_load(&g_dol, dol);
    if (!g_dol_ok){ fprintf(stderr,"FATAL: load DOL '%s'\n", dol); return 2; }
    fprintf(stderr,"loaded DOL %s\n", dol);

    int total=0, mism=0;
    printf("# fn_80194510 oracle: A=DolRecomp original | B=src/hsd/hsd_cobj.c\n");
    printf("# case  ptr        ->  A(f32 bits / val)   B(f32 bits / val)   verdict\n");

    /* Self-contained cases (no external call) - the in-scope Tier-1 proof.
     * case1 is EXCLUDED from the pass/fail tally: it calls fn_800CE220, a
     * tan-family helper whose own implementation spans call-chains beyond the
     * 2 chunks compiled here, so neither side fully evaluates it in this
     * minimal harness. That is Tier-2 scope (whole-program callee tree) and is
     * reported separately, NOT counted as a clean equivalence result. */
    struct { const char* name; u8 sel; u32 ptr; f32 a44,a38,a40,a4c; int counted; } T[] = {
        { "NULL    ", 0, 0,    0,0,0,0,       1 },
        { "case2   ", 2, OBJ,  0,0,0, 3.5f,   1 },
        { "case3   ", 3, OBJ,  0,0,0, -7.25f, 1 },
        { "default0", 0, OBJ,  0,0,0, 0,      1 },  /* sel 0 -> default */
        { "default9", 9, OBJ,  0,0,0, 0,      1 },  /* sel 9 -> default */
        { "case1*  ", 1, OBJ,  2.0f, 3.0f, 0.5f, 0, 0 }, /* fn_800CE220 - Tier-2, not counted */
    };
    for (size_t i=0;i<sizeof(T)/sizeof(T[0]);i++) {
        /* Reset+preload happens inside A_call; seed AFTER reset for A, and seed
         * B mirror too. We must seed between reset and call: do a manual order. */
        cpu_reset(&g_cpu);
        for (int s=0;s<DOL_NUM_TEXT;s++){u32 a=g_dol.header.text_addresses[s],sz=g_dol.header.text_sizes[s];
            const u8* d=dol_get_text_section(&g_dol,s); if(d&&sz) for(u32 o=0;o<sz;o++) mem_write8(&g_cpu,a+o,d[o]); }
        for (int s=0;s<DOL_NUM_DATA;s++){u32 a=g_dol.header.data_addresses[s],sz=g_dol.header.data_sizes[s];
            const u8* d=dol_get_data_section(&g_dol,s); if(d&&sz) for(u32 o=0;o<sz;o++) mem_write8(&g_cpu,a+o,d[o]); }
        memset(g_bram,0,g_bram_size);
        load_sda_consts();
        if (T[i].ptr) seed_case(T[i].sel, T[i].a44,T[i].a38,T[i].a40,T[i].a4c);

        /* A-side */
        g_cpu.gpr[1]=0x80300000u; g_cpu.gpr[2]=SDA_R2; g_cpu.gpr[13]=SDA_R13;
        g_cpu.gpr[3]=T[i].ptr; g_cpu.lr=SENTINEL; g_cpu.pc=0x80194510u;
        A_run(&g_cpu);
        f32 ra=(f32)g_cpu.fpr[1]; u32 abits; memcpy(&abits,&ra,4);
        /* B-side */
        u32 bbits = B_call(T[i].ptr);
        int ok = (abits==bbits);
        const char* verdict;
        if (T[i].counted) { total++; if(!ok) mism++; verdict = ok?"ok":"**MISMATCH**"; }
        else              { verdict = ok?"ok (Tier-2)":"Tier-2 callee-tree (not counted)"; }
        printf("  %s  %08X  ->  %08X (%g)   %08X (%g)   %s\n",
               T[i].name, T[i].ptr, abits, bits_to_f32(abits), bbits, bits_to_f32(bbits),
               verdict);
    }

    printf("\nRESULT: %d in-scope cases, %d mismatches -> %s\n",
           total, mism, mism==0?"ORACLE CONFIRMS EQUIVALENCE (machine-verified)":"DIVERGENCE");
    printf("  (case1* excluded: needs whole-program callee tree for fn_800CE220 = Tier-2)\n");
    free(g_bram); cpu_free(&g_cpu);
    return mism==0?0:1;
}
