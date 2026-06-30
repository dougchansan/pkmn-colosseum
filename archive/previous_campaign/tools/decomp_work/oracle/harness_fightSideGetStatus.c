/* ============================================================================
 * Behavioral-equivalence ORACLE - fightSideGetStatus (0x801F76B8)
 *
 * A-side: ORIGINAL machine code, statically recompiled by DolRecomp
 *         (generated func_801F55E0, flat big-endian guest RAM).
 * B-side: OUR decompiled C, taken VERBATIM from src/game/pokemon.c
 *         (see bside_real_src.inc - extracted by line-range, never retyped).
 *
 * Both sides run on the SAME canonical memory model. A-side RAM is big-endian
 * (guest native, via DolRecomp's mem_*); B-side gets a host pointer into a
 * little-endian mirror holding identical logical values. Pointer-valued
 * results are normalised back to guest addresses before comparison, so the
 * two return values are directly comparable with zero endian artifacts.
 *
 * NO src/ files are modified. The src text is #included read-only.
 * ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "core/cpu.h"          /* DolRecomp A-side runtime */
#include "frontend/dol.h"      /* DOL loader: preload code+data into guest RAM */

/* ---- A-side generated entry (chunk owning 0x801F55E0..0x801F95E0) ---- */
void func_801F55E0(CPUState* ctx);
#define CHUNK_LO 0x801F55E0u
#define CHUNK_HI 0x801F95E0u
#define SENTINEL 0x0BADF00Du
#define GUEST_BASE 0x80000000u

/* ============================================================================
 * B-side: REAL src/ code, compiled in its own namespace.
 * We #include the verbatim source text inside a fresh translation context.
 * The src bodies reference u8/u16/u32/NULL (from bside_shim.h) and two BSS
 * globals lbl_80478F38 / lbl_80478F3C (the runtime status table).
 * ========================================================================== */
#include "bside_shim.h"

/* BSS globals the real fn_801F7870 reads. In the game these live at guest
 * 0x80478F38/0x80478F3C (BSS, zero at load). We expose them as real storage
 * and keep the A-side RAM consistent with whatever we set here. */
u32 lbl_80478F38;   /* count */
u32 lbl_80478F3C;   /* table base (a guest address) */

/* B-side RAM mirror: little-endian host copy of the same logical bytes the
 * A-side holds big-endian. Sized to cover the test object region. */
static u8* g_bram;        /* host buffer, index 0 == guest 0x80000000 */
static u32 g_bram_size;

/* Convert a B-side host pointer (into g_bram) back to a guest address so it
 * can be compared with the A-side's guest-address return. */
static u32 host_to_guest(void* p) {
    if (p == NULL) return 0;
    return GUEST_BASE + (u32)((u8*)p - g_bram);
}

/* Host-ABI version of fn_801F7870 (table remap for slotType 1/2/3).
 *
 * The REAL src body is:
 *     u32 count = *(u32*)lbl_80478F38;
 *     if ((u16)idx >= count) return NULL;
 *     return (u8*)(lbl_80478F3C + (u16)idx * 0x14);
 * On the 32-bit guest, lbl_80478F3C is a guest address and (u8*)(u32) is a
 * valid pointer. On an LP64 host, casting a 32-bit value to u8* truncates and
 * is meaningless. We therefore reproduce the EXACT logic (same count gate,
 * same 0x14 stride) but materialise a real host pointer into g_bram. The
 * control flow and arithmetic are identical to the src; only pointer
 * materialisation is host-corrected. lbl_80478F3C still holds the guest base.
 *
 * This is the one place the verbatim src cannot run unmodified on a 64-bit
 * host; it is isolated, documented, and the logic is preserved 1:1. */
static u8* bside_fn_801F7870_host(u32 idx) {  /* extern in src decls (u32) */
    u32 count = lbl_80478F38;                 /* count value (we store it directly) */
    if ((u16)idx >= count) return NULL;
    u32 guest = lbl_80478F3C + (u16)idx * 0x14u;   /* same arithmetic as src */
    return g_bram + (guest - GUEST_BASE);     /* host pointer into shared mirror */
}
#define fn_801F7870 bside_fn_801F7870_host

/* The real src bodies, verbatim (fn_801F7870 excluded; see host version above).
 * They are plain C functions operating on u8* host pointers - exactly as the
 * shipped game C does. */
#include "bside_real_src.inc"

#undef fn_801F7870

/* ============================================================================
 * Canonical memory model: one logical layout written into BOTH RAMs.
 * ========================================================================== */
static CPUState g_cpu;     /* A-side */
static DOLFile  g_dol;     /* original start.dol, loaded once */
static int      g_dol_ok;

/* Preload all DOL code+data sections into A-side guest RAM. Needed so the
 * original's jumptable / rodata reads (e.g. jumptable_8037564C in .data)
 * resolve. Must run after every cpu_reset (which zeroes RAM). */
static void preload_dol_into_ram(void) {
    if (!g_dol_ok) return;
    for (int i = 0; i < DOL_NUM_TEXT; i++) {
        u32 addr = g_dol.header.text_addresses[i];
        u32 size = g_dol.header.text_sizes[i];
        const u8* data = dol_get_text_section(&g_dol, i);
        if (!data || !size) continue;
        for (u32 o = 0; o < size; o++) mem_write8(&g_cpu, addr + o, data[o]);
    }
    for (int i = 0; i < DOL_NUM_DATA; i++) {
        u32 addr = g_dol.header.data_addresses[i];
        u32 size = g_dol.header.data_sizes[i];
        const u8* data = dol_get_data_section(&g_dol, i);
        if (!data || !size) continue;
        for (u32 o = 0; o < size; o++) mem_write8(&g_cpu, addr + o, data[o]);
    }
}

/* Write a u32 big-endian into A-side guest RAM. */
static void a_write32(u32 guest_addr, u32 v) { mem_write32(&g_cpu, guest_addr, v); }
/* Write a u16 big-endian into A-side guest RAM. */
static void a_write16(u32 guest_addr, u16 v) { mem_write16(&g_cpu, guest_addr, v); }
static void a_write8 (u32 guest_addr, u8  v) { mem_write8 (&g_cpu, guest_addr, v); }

/* Write the SAME value little-endian (host native) into the B-side mirror. */
static void b_write32(u32 guest_addr, u32 v) { memcpy(g_bram + (guest_addr-GUEST_BASE), &v, 4); }
static void b_write16(u32 guest_addr, u16 v) { memcpy(g_bram + (guest_addr-GUEST_BASE), &v, 2); }
static void b_write8 (u32 guest_addr, u8  v) { g_bram[guest_addr-GUEST_BASE] = v; }

/* Set a logical u32 field into both RAMs at the same guest address. */
static void poke32(u32 ga, u32 v) { a_write32(ga, v); b_write32(ga, v); }
static void poke16(u32 ga, u16 v) { a_write16(ga, v); b_write16(ga, v); }
static void poke8 (u32 ga, u8  v) { a_write8 (ga, v); b_write8 (ga, v); }

/* forward decl: A-side runner (defined below) */
static u32 A_fight_noreset(u32 r3, u32 r4, u32 r5, u32 r6);

/* Which slotTypes return a POINTER (vs a value)? In fightSideGetStatus:
 *   slotType 6 -> fn_801F7908 (ptr), 7 -> fn_801F78D4 (ptr): pointer results.
 *   slotType 1,2,3,5,8: value getters. slotType<4 also REMAPS ptr1 via
 *   fn_801F7870, but the RESULT of the dispatch is still a value (1,2,3) or 0. */
static int slottype_returns_pointer(u32 st) {
    u32 s = (u16)st;
    return (s == 6 || s == 7);
}

/* ---- B-side call: run REAL src fightSideGetStatus ---- */
/* fightSideGetStatus(u8* ptr1, u32 param2, u32 slotType, u32 param5).
 * For pointer-target inputs we pass a host pointer into g_bram. */
static u32 B_fight(u32 r3_guest, u32 r4, u32 r5, u32 r6) {
    u8* ptr1 = (r3_guest == 0) ? NULL : (g_bram + (r3_guest - GUEST_BASE));
    u32 ret = fightSideGetStatus(ptr1, r4, r5, r6);
    /* fightSideGetStatus's declared return type is u32. For the pointer-valued
     * slotTypes (6,7) the real src returns a u8* TRUNCATED to 32 bits at the
     * return; on LP64 that loses g_bram's high 32 bits. We reconstruct the full
     * host pointer (re-attach g_bram's high half) and normalise to a guest
     * address. We key strictly on slotType (not a value-range heuristic) so a
     * value getter that happens to return a pointer-shaped integer is NEVER
     * misinterpreted. */
    if (slottype_returns_pointer(r5) && ret != 0) {
        uintptr_t hi   = (uintptr_t)g_bram & ~(uintptr_t)0xFFFFFFFFu;
        u8* asp = (u8*)(hi | (uintptr_t)ret);
        if (asp >= g_bram && asp < g_bram + g_bram_size) return host_to_guest(asp);
        /* pointer outside our mirror: report raw (will surface as mismatch) */
    }
    return ret;
}

/* Re-seed both RAMs with the canonical object after an A-side cpu_reset. */
typedef void (*SeedFn)(void);
static SeedFn g_seed;

static u32 RUN(SeedFn seed, u32 r3, u32 r4, u32 r5, u32 r6, u32* out_a, u32* out_b) {
    /* A-side: reset (clears RAM), preload original code+data, then seed, call. */
    cpu_reset(&g_cpu);
    preload_dol_into_ram();
    memset(g_bram, 0, g_bram_size);
    lbl_80478F38 = 0; lbl_80478F3C = 0;
    if (seed) seed();
    u32 a = A_fight_noreset(r3, r4, r5, r6);
    u32 b = B_fight(r3, r4, r5, r6);
    if (out_a) *out_a = a; if (out_b) *out_b = b;
    return (a == b);
}

/* SDA bases, recovered from __init_registers (0x800032B0) in start.dol:
 *   r13 (_SDA_BASE_)  = 0x80480820
 *   r2  (_SDA2_BASE_) = 0x804836A0
 * fn_801F7870 reads pointers at r13-30952 (0x80478F38) and r13-30948
 * (0x80478F3C); those guest words hold POINTERS to the count and table. */
#define SDA_R13 0x80480820u
#define SDA_R2  0x804836A0u

/* A-side without the internal reset (RUN already reset+seeded). */
static u32 A_fight_noreset(u32 r3, u32 r4, u32 r5, u32 r6) {
    g_cpu.gpr[1] = 0x80300000u;
    g_cpu.gpr[2] = SDA_R2;
    g_cpu.gpr[13]= SDA_R13;
    g_cpu.gpr[3] = r3; g_cpu.gpr[4] = r4; g_cpu.gpr[5] = r5; g_cpu.gpr[6] = r6;
    g_cpu.lr = SENTINEL;
    g_cpu.pc = 0x801F76B8u;
    for (unsigned i = 0; i < 200000u; i++) {
        u32 pc = g_cpu.pc;
        if (pc >= CHUNK_LO && pc < CHUNK_HI && ((pc - CHUNK_LO) & 3u) == 0u) {
            func_801F55E0(&g_cpu);
            if (g_cpu.exception) { fprintf(stderr,"A exc pc=%08X\n", g_cpu.pc); return 0xEEEE0001u; }
        } else {
            return g_cpu.gpr[3];
        }
    }
    return 0xEEEE0002u;
}

/* ============================================================================
 * Seeds: populate the canonical object(s) the getters read.
 * Object at OBJ (guest 0x80400000):
 *   +0x00  : u16 species/value  (read by fn_801F7858, fn_801F793C, fn_801F78AC)
 *   +0x4,+0xC over idx*4 : u32  (read by fn_801F7824, fn_801F77F0)
 *   +0x522C: u8                 (read by fn_801F78BC)
 * ========================================================================== */
#define OBJ 0x80400000u
static void seed_object(void) {
    poke16(OBJ + 0x0,   0xABCD);          /* [0] u16 */
    poke32(OBJ + 0x4,   0x11111111);      /* idx0 +0x4 */
    poke32(OBJ + 0x8,   0x22222222);      /* idx1 +0x4 */
    poke32(OBJ + 0xC,   0x33333333);      /* idx0 +0xC */
    poke32(OBJ + 0x10,  0x44444444);      /* idx1 +0xC */
    poke8 (OBJ + 0x522C,0x5A);            /* u8 status flag */
}
/* Seed for slotType 1/2/3: populate the SDA-relative status table.
 *
 * Per the ORIGINAL asm (fn_801F7870 @ 0x801F7870):
 *   r4 = *(u32*)(r13-30952) ; i.e. *(u32*)0x80478F38   -> pointer to count word
 *   count = *(u32*)r4
 *   base  = *(u32*)(r13-30948) ; i.e. *(u32*)0x80478F3C -> table base address
 *   if (idx >= count) return NULL; else return base + idx*20
 *
 * We model the table base = OBJ, stride 20 (0x14), count = CNT. The dispatched
 * getter then reads off (OBJ + idx*0x14). We point the count word at a scratch
 * cell COUNTW. Both A and B sides must agree on the returned pointer and the
 * value the getter reads from it. */
#define COUNTW 0x80401000u   /* scratch cell holding the count */
#define TBL_CNT 4u
static void seed_table(void) {
    seed_object();
    /* Lay out a few table rows at OBJ so getters off (OBJ+idx*0x14) read data. */
    /* Row 0 (idx0) sits at OBJ; the getters for case 2/3 read +0x4/+0xC. */
    /* A-side SDA wiring: */
    a_write32(0x80478F38, COUNTW);   /* ptr-to-count */
    a_write32(COUNTW,     TBL_CNT);  /* count value */
    a_write32(0x80478F3C, OBJ);      /* table base */
    /* B-side: src does count=*(u32*)lbl_80478F38 and base=lbl_80478F3C.
     * We make the host fn read the SAME logical count/base: count = TBL_CNT,
     * base = OBJ (guest). The host fn_801F7870 we provide uses these directly. */
    lbl_80478F38 = TBL_CNT;          /* B-side count (host fn reads value) */
    lbl_80478F3C = OBJ;              /* B-side base  (guest addr) */
}

int main(int argc, char** argv) {
    if (!cpu_init(&g_cpu)) { fprintf(stderr,"cpu_init failed\n"); return 2; }
    g_bram_size = g_cpu.ram_size;          /* same size as guest RAM */
    g_bram = (u8*)calloc(1, g_bram_size);
    if (!g_bram) { fprintf(stderr,"bram alloc failed\n"); return 2; }

    /* Load the ORIGINAL start.dol so jumptables/rodata resolve in A-side RAM.
     * Path from argv[1] or $ORACLE_DOL (default /tmp/start.dol). */
    const char* dol_path = (argc > 1) ? argv[1]
                         : (getenv("ORACLE_DOL") ? getenv("ORACLE_DOL")
                                                 : "/tmp/start.dol");
    g_dol_ok = dol_load(&g_dol, dol_path);
    if (!g_dol_ok) {
        fprintf(stderr, "FATAL: could not load DOL '%s' - A-side jumptables "
                        "will not resolve. Pass path as argv[1] or $ORACLE_DOL.\n", dol_path);
        return 2;
    }
    fprintf(stderr, "loaded DOL %s (entry %08X)\n", dol_path, g_dol.header.entry_point);

    int total=0, mism=0;
    printf("# fightSideGetStatus oracle: A=DolRecomp original | B=src/game/pokemon.c\n");
    printf("# slotType param5  r3      ->  A(orig)    B(ours)   verdict\n");

    /* ---- Structured sweep: every slotType, several param5, ptr=OBJ ---- */
    for (u32 st = 0; st <= 11; st++) {
        for (u32 p5 = 0; p5 < 4; p5++) {
            /* slotType<4 uses the BSS table seed; else direct object. */
            SeedFn seed = (st >= 1 && st < 4) ? seed_table : seed_object;
            u32 a=0,b=0;
            u32 ok = RUN(seed, OBJ, /*param2=*/p5, st, p5, &a, &b);
            total++; if(!ok) mism++;
            printf("  st=%2u p5=%u  %08X  ->  %08X  %08X  %s\n",
                   st, p5, OBJ, a, b, ok?"ok":"**MISMATCH**");
        }
    }
    /* ---- NULL pointer + table-empty paths ---- */
    { u32 a=0,b=0; u32 ok=RUN(seed_object,0,0,7,0,&a,&b); total++; if(!ok)mism++;
      printf("  null ptr st=7      ->  %08X  %08X  %s\n", a,b, ok?"ok":"**MISMATCH**"); }
    { u32 a=0,b=0; u32 ok=RUN(seed_object,0,0,2,0,&a,&b); total++; if(!ok)mism++;  /* st=2 needs table, but seeded object only -> table empty -> NULL */
      printf("  st=2 empty-table   ->  %08X  %08X  %s\n", a,b, ok?"ok":"**MISMATCH**"); }

    /* ---- Light fuzz: random object field values + random st/p5 ---- */
    srand(12345);
    for (int it=0; it<2000; it++) {
        u32 st = rand()%12;
        u32 p5 = rand()%8;
        SeedFn base_seed = (st>=1 && st<4) ? seed_table : seed_object;
        /* wrap base seed with randomized field contents */
        u32 a=0,b=0;
        /* custom seed via closure-ish: set globals then call */
        cpu_reset(&g_cpu); preload_dol_into_ram(); memset(g_bram,0,g_bram_size);
        lbl_80478F38=0; lbl_80478F3C=0;
        /* random object */
        u16 f0 = (u16)rand(); u32 f4=((u32)rand()<<16)^rand(); u32 fc=((u32)rand()<<16)^rand();
        u8  fb = (u8)rand();
        poke16(OBJ+0x0,f0);
        poke32(OBJ+0x4,f4); poke32(OBJ+0x8,(u32)rand());
        poke32(OBJ+0xC,fc); poke32(OBJ+0x10,(u32)rand());
        poke8(OBJ+0x522C,fb);
        if (st>=1 && st<4){ u32 cnt=rand()%6;
                            /* A-side: SDA pointer-indirect (same as seed_table) */
                            a_write32(0x80478F38, COUNTW); a_write32(COUNTW, cnt);
                            a_write32(0x80478F3C, OBJ);
                            /* B-side: host fn reads value directly */
                            lbl_80478F38=cnt; lbl_80478F3C=OBJ; }
        (void)base_seed;
        a = A_fight_noreset(OBJ, p5, st, p5);
        b = B_fight(OBJ, p5, st, p5);
        total++; if (a!=b){ mism++;
            printf("  FUZZ MISMATCH st=%u p5=%u f0=%04X f4=%08X fc=%08X fb=%02X -> A=%08X B=%08X\n",
                   st,p5,f0,f4,fc,fb,a,b); }
    }

    printf("\nRESULT: %d cases, %d mismatches -> %s\n",
           total, mism, mism==0 ? "ORACLE CONFIRMS EQUIVALENCE (machine-verified)"
                                : "DIVERGENCE FOUND");
    free(g_bram);
    cpu_free(&g_cpu);
    return mism==0 ? 0 : 1;
}
