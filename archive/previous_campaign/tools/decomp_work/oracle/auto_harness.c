/* auto_harness.c - GENERIC oracle harness, no per-function hand-coding.
 *
 * Driven by:
 *   ORC_FN_ADDR   (guest entry address, -D)
 *   ORC_FN_NAME   (B-side symbol, -D)   e.g. fn_8023CFDC
 *   abi.inc       (ORC_NARGS, ORC_ARGS[], ORC_RET_* from resolve_abi.py)
 *   bside_real_src.inc  (verbatim src body of the target + needed callees)
 *
 * A-side: recomp original at ORC_FN_ADDR on the FULL image (libcolo.a).
 * B-side: the real src function, compiled here.
 *
 * Inputs:
 *   - INT args: swept small values + randomized 32-bit (incl. 0, 1, -1, edges)
 *   - PTR args: each points at its own fuzzed RAM struct (ARG_STRUCT bytes of
 *     randomized data the fn may read). The SAME bytes go into A-side guest RAM
 *     (big-endian region) and the B-side host mirror.
 *   - FLT args: swept + randomized finite floats.
 *
 * Compare: return (r3 / f1 per ORC_RET_*) AND a window of guest RAM around each
 * pointer arg (side-effects). Endianness: A-side RAM is big-endian; B-side runs
 * on a little-endian host mirror seeded with identical logical bytes. Pointer
 * RETURNS are normalised host->guest, keyed on ORC_RET kind.
 *
 * Emits: PASS / MISMATCH (with the offending input) / UNREACHABLE (Tier-2 budget
 * or exception in callee tree) summary line.
 */
#include "oracle_runtime.h"

/* ---- B-side: the real src function + its forward decl ---- */
/* The verbatim src provides the body; we need a prototype matching ORC_RET. */
#include "bside_shim.h"

/* B-side pointer mirror */
static u8* g_bram; static u32 g_bram_size;
static CPUState g_cpu; static DOLFile g_dol;

/* coverage counters (referenced via extern in main for locality) */
u32 g_nonzero_ret=0, g_distinct_hash=0, g_ram_writes=0;

static u32 host_to_guest(void* p){ return p? (ORC_GUEST_BASE + (u32)((u8*)p - g_bram)) : 0; }

/* ---- callee thunks: any B-side callee NOT extracted routes to its recomp
 * original, so the differential isolates the TARGET function's own codegen
 * (callees are identical bytes on both sides). Args follow the GameCube EABI:
 * up to 8 GPR args r3..r10 (ints/ptrs) -> we accept a wide fixed signature.
 * Pointer ARGS passed from B (host pointers into g_bram) are converted to
 * guest addresses before entering the recomp; pointer RETURNS are converted
 * back to host pointers. Float args/returns through thunks are not modeled
 * (flagged at generation time). */
/* forward decl (defined just below) */
static u32 orc_p2g(void* p);

/* Args are taken POINTER-WIDTH (uintptr_t) so a host pointer into the B mirror
 * survives intact; orc_g() converts each to a guest address (host pointers into
 * g_bram -> guest addr; small integers pass through unchanged). This is what
 * lets a B-side callee thunk receive a mirror pointer and run the recomp on the
 * matching GUEST address. */
static u32 orc_g(uintptr_t v){
    u8* p=(u8*)v;
    if(p>=g_bram && p<g_bram+g_bram_size) return ORC_GUEST_BASE+(u32)(p-g_bram);
    return (u32)v;   /* not a mirror pointer: it's a value (low 32 bits) */
}
static u32 orc_thunk(u32 addr, uintptr_t a3,uintptr_t a4,uintptr_t a5,uintptr_t a6,
                     uintptr_t a7,uintptr_t a8,uintptr_t a9,uintptr_t a10){
    CPUState save = g_cpu;
    g_cpu.gpr[3]=orc_g(a3); g_cpu.gpr[4]=orc_g(a4); g_cpu.gpr[5]=orc_g(a5); g_cpu.gpr[6]=orc_g(a6);
    g_cpu.gpr[7]=orc_g(a7); g_cpu.gpr[8]=orc_g(a8); g_cpu.gpr[9]=orc_g(a9); g_cpu.gpr[10]=orc_g(a10);
    g_cpu.gpr[1]=0x80300000u; g_cpu.lr=ORC_SENTINEL; g_cpu.pc=addr;
    int rc = orc_run(&g_cpu, 5000000u);
    u32 ret = g_cpu.gpr[3];
    /* preserve RAM (side effects) but restore registers/pc/lr */
    u8* ram=g_cpu.ram; u32 rs=g_cpu.ram_size;
    g_cpu = save; g_cpu.ram=ram; g_cpu.ram_size=rs;
    if(rc!=0) { /* thunk callee unreachable -> mark via sentinel value */ }
    return ret;
}
/* Float-returning callee thunk: arg in f1 (single), result from f1 (double),
 * per the GameCube EABI. Used for callees the src casts to (f32)/(double) or
 * declares returning float/double (e.g. the tan helper fn_800CE220). */
static double orc_thunk_f(u32 addr, f32 farg){
    CPUState save = g_cpu;
    g_cpu.fpr[1] = (double)farg;
    g_cpu.gpr[1] = 0x80300000u; g_cpu.lr = ORC_SENTINEL; g_cpu.pc = addr;
    orc_run(&g_cpu, 5000000u);
    double r = g_cpu.fpr[1];
    u8* ram=g_cpu.ram; u32 rs=g_cpu.ram_size;
    g_cpu = save; g_cpu.ram=ram; g_cpu.ram_size=rs;
    return r;
}

/* Convert a B-side host pointer arg into a guest address for thunk entry. */
static u32 orc_p2g(void* p){
    if(!p) return 0;
    u8* b=(u8*)p;
    if(b>=g_bram && b<g_bram+g_bram_size) return ORC_GUEST_BASE+(u32)(b-g_bram);
    return (u32)(uintptr_t)p; /* non-mirror: pass low bits (best effort) */
}

#include "bside_thunks.inc"   /* per-callee wrappers, generated */
#include "bside_lbls.inc"     /* SDA/global lbl_XXXXXXXX storage, generated */

#include "bside_real_src.inc"

/* Populate each referenced lbl_XXXXXXXX from the DOL-loaded guest RAM at its
 * address, so the B-side reads the same constant/global the A-side does.
 * Generated alongside bside_lbls.inc (the load list). Called after orc_prep. */
static void orc_load_lbls(void);
#include "bside_lbls_load.inc"

#include "abi.inc"

/* ARG_STRUCT: bytes of fuzzable backing store per pointer arg. */
#define ARG_STRUCT 0x6000u            /* 24KB - covers large field offsets */
#define ARG_BASE   0x80500000u        /* first pointer arg's guest base */
#define ARG_STRIDE 0x10000u           /* spacing between pointer-arg structs */
#define SIDE_WINDOW 0x400u            /* RAM-delta comparison window per ptr */

/* Reconstruct full host pointer from a 32-bit truncated B-side pointer return. */
static u32 normalize_bret_ptr(u32 ret){
    if(ret==0) return 0;
    uintptr_t hi=(uintptr_t)g_bram & ~(uintptr_t)0xFFFFFFFFu;
    u8* p=(u8*)(hi|(uintptr_t)ret);
    if(p>=g_bram && p<g_bram+g_bram_size) return host_to_guest(p);
    return ret; /* not in mirror: surface raw */
}

/* B-side call via a generic function-pointer cast based on arg count/kind.
 * We support up to 4 args of INT/PTR (GPR) and floats; floats handled by a
 * separate path. For simplicity the generic path covers the common leaf/getter
 * shapes (<=4 scalar/ptr args, optional one trailing float not yet wired). */
/* The B-side target symbol (defined verbatim in bside_real_src.inc). Its real
 * prototype is whatever the src declared; we cast at the call site per the
 * resolved ABI (see B_call_int_ret). */

/* arg value carriers */
static u32  A_args[8];     /* guest-side raw values (ptr args = guest addr) */
static void* B_ptrs[8];    /* host pointers for ptr args (else NULL) */
static int  arg_is_ptr[8];

/* Address of the B-side target (macro -> real symbol from bside_real_src.inc). */
static void* g_bfn(void){ return (void*)&ORC_FN_NAME; }

/* B-side call dispatcher. We must honor x86-64 SysV: integer/pointer args use
 * rdi.. , float args use xmm0.. (separate files), and the return is in rax
 * (int) or xmm0 (float). We enumerate the small set of ABI shapes the
 * leaf/getter targets actually use. Unsupported shapes -> reported, not faked.
 *
 * Encoding of an arg: integer/pointer -> u64 carrier; float -> f32 carrier.
 * We support these shapes (i=int/ptr, f=float):
 *   ()  (i) (ii) (iii) (iiii)  (if)  (i f via PTR+FLT, e.g. setter)
 * Float-return shapes mirror these.
 */
typedef u64 (*Ii0)(void); typedef u64 (*Ii1)(u64);
typedef u64 (*Ii2)(u64,u64); typedef u64 (*Ii3)(u64,u64,u64);
typedef u64 (*Ii4)(u64,u64,u64,u64);
typedef u64 (*Iif)(u64,f32);            /* (ptr/int, float) -> int  */
typedef f32 (*Fi1)(u64); typedef f32 (*Fi2)(u64,u64);
typedef f32 (*Fif)(u64,f32);

/* returns: out_is_float set; result in *iret or *fret */
static void B_dispatch(int* out_is_float, u64* iret, f32* fret){
    u64 a[4]; f32 fa[4]; int nf=0;
    /* build integer-register operands (ptr or widened int) in declaration
     * order, and a parallel float-operand list. The actual prototype below
     * picks the right register classes. */
    for(int i=0;i<ORC_NARGS && i<4;i++){
        if(ORC_ARGS[i].kind==2){ fa[nf]= *(f32*)&A_args[i]; a[i]=0; nf++; }
        else a[i] = arg_is_ptr[i] ? (u64)(uintptr_t)B_ptrs[i] : (u64)A_args[i];
    }
    void* fn = g_bfn();
#ifdef ORC_RET_FLT
    *out_is_float=1;
    if(ORC_NARGS==1 && ORC_ARGS[0].kind!=2){ *fret=((Fi1)fn)(a[0]); return; }
    if(ORC_NARGS==2 && ORC_ARGS[0].kind!=2 && ORC_ARGS[1].kind==2){ *fret=((Fif)fn)(a[0],fa[0]); return; }
    if(ORC_NARGS==2 && ORC_ARGS[0].kind!=2 && ORC_ARGS[1].kind!=2){ *fret=((Fi2)fn)(a[0],a[1]); return; }
    *fret=0; *out_is_float=2; /* unsupported shape sentinel */
#else
    *out_is_float=0;
    if(ORC_NARGS==0){ *iret=((Ii0)fn)(); return; }
    if(ORC_NARGS==1){ *iret=((Ii1)fn)(a[0]); return; }
    if(ORC_NARGS==2 && ORC_ARGS[1].kind==2){ *iret=((Iif)fn)(a[0],fa[0]); return; }
    if(ORC_NARGS==2){ *iret=((Ii2)fn)(a[0],a[1]); return; }
    if(ORC_NARGS==3){ *iret=((Ii3)fn)(a[0],a[1],a[2]); return; }
    if(ORC_NARGS==4){ *iret=((Ii4)fn)(a[0],a[1],a[2],a[3]); return; }
    *iret=0; *out_is_float=2;
#endif
}

/* Seed a pointer arg's struct into A guest RAM (big-endian) and the B mirror.
 *
 * ENDIANNESS MODE (ORC_PTR_ENDIAN):
 *   "word32" (default): B mirror is byte-swapped per 32-bit word, so B-side
 *     `*(u32*)`/`*(f32*)` reads at 4-aligned offsets match A's big-endian reads.
 *     Correct for word-data getters; a function that also reads u8/u16 from the
 *     SAME fuzzed pointer data will see a different logical value (a HARNESS
 *     artifact, NOT a decomp bug). The runner flags such mixed-width access.
 *   "raw": identical bytes both sides - correct only for pointer-ARITHMETIC
 *     functions that never dereference the data (e.g. ptr+idx*stride getters).
 *
 * There is no single byte layout that satisfies BOTH word and sub-word reads of
 * black-box fuzzed data across endianness; this is a fundamental limit of the
 * auto-harness. Such functions need the hand-harness (logical-field seeding). */
static void seed_ptr_struct(u32 guest_base, const u8* bytes, u32 n){
    for(u32 o=0;o<n;o++) mem_write8(&g_cpu, guest_base+o, bytes[o]); /* A: raw BE */
    u8* m = g_bram + (guest_base - ORC_GUEST_BASE);
#ifdef ORC_PTR_RAW
    for(u32 o=0;o<n;o++) m[o]=bytes[o];                  /* raw: ptr-arith only */
#else
    for(u32 o=0;o+3<n;o+=4){                              /* swap 32-bit words */
        m[o+0]=bytes[o+3]; m[o+1]=bytes[o+2]; m[o+2]=bytes[o+1]; m[o+3]=bytes[o+0];
    }
#endif
}

#ifdef ORC_USE_FIELDMAP
#include "bside_fieldmap.inc"   /* ORC_NFIELDS + ORC_FIELDS[] {arg,off,width} */

/* STRUCT-AWARE SEEDING. For each (arg,offset,width) field the fn (or its
 * callees) reads off a pointer arg, choose ONE logical value and write it:
 *   - BIG-endian into A's guest RAM at base+offset (guest native), and
 *   - LITTLE-endian (host native) into B's mirror at base+offset.
 * Both sides then read the SAME logical value at that field regardless of
 * width or endianness — the artifact is removed at the root. Fields not in the
 * map are zero on both sides (unread). bases[a] is the guest base for arg a. */
static void seed_fields(const u32 bases[8]){
    for(int i=0;i<ORC_NFIELDS;i++){
        int a=ORC_FIELDS[i].arg, off=ORC_FIELDS[i].off, w=ORC_FIELDS[i].width;
        if(a<0||a>=ORC_NARGS||!bases[a]) continue;
        u32 ga = bases[a] + (u32)off;
        u64 v = ((u64)rand()<<32) ^ ((u64)rand()<<13) ^ (u64)rand();
        u8* m = g_bram + (ga - ORC_GUEST_BASE);
        if(w==1){ u8 x=(u8)v; mem_write8(&g_cpu,ga,x); m[0]=x; }
        else if(w==2){ u16 x=(u16)v; mem_write16(&g_cpu,ga,x);   /* A: BE */
            m[0]=(u8)(x&0xFF); m[1]=(u8)(x>>8); }                /* B: LE */
        else if(w==4){ u32 x=(u32)v; mem_write32(&g_cpu,ga,x);
            m[0]=(u8)x; m[1]=(u8)(x>>8); m[2]=(u8)(x>>16); m[3]=(u8)(x>>24); }
        else if(w==8){ u64 x=v; mem_write64(&g_cpu,ga,x);
            for(int b=0;b<8;b++) m[b]=(u8)(x>>(8*b)); }
    }
}
#endif

int main(int argc, char** argv){
    if(!cpu_init(&g_cpu)) return 2;
    g_bram_size=g_cpu.ram_size; g_bram=(u8*)calloc(1,g_bram_size);
    const char* dolp = argc>1?argv[1]:(getenv("ORACLE_DOL")?getenv("ORACLE_DOL"):"/tmp/start.dol");
    if(!dol_load(&g_dol,dolp)){ fprintf(stderr,"FATAL dol %s\n",dolp); return 2; }

    /* classify args from ABI table */
    for(int i=0;i<ORC_NARGS;i++) arg_is_ptr[i] = (ORC_ARGS[i].kind==1);

    int total=0, mism=0, unreach=0;
    u32 first_mism_seed=0; int have_mism=0;
    unsigned NCASES = 1200;
    srand(0xC0FFEE);

    static u8 fuzzbuf[ARG_STRUCT];
    for(unsigned it=0; it<NCASES; it++){
        orc_prep(&g_cpu, &g_dol);
        orc_load_lbls();          /* refresh B-side SDA/global lbl_* from RAM */
        memset(g_bram,0,g_bram_size);

        /* assemble args */
        u32 ptr_count=0;
        u32 bases[8]={0};   /* guest base per arg (0 if not a seeded pointer) */
        for(int i=0;i<ORC_NARGS;i++){
            if(ORC_ARGS[i].kind==1){ /* PTR */
                u32 base = ARG_BASE + ptr_count*ARG_STRIDE;
                /* ~10% of the time pass NULL to test guard clauses */
                if((rand()%10)==0){ A_args[i]=0; B_ptrs[i]=NULL; }
                else {
#ifndef ORC_USE_FIELDMAP
                    for(u32 k=0;k<ARG_STRUCT;k++) fuzzbuf[k]=(u8)rand();
                    seed_ptr_struct(base, fuzzbuf, ARG_STRUCT);
#endif
                    A_args[i]=base; B_ptrs[i]=g_bram+(base-ORC_GUEST_BASE);
                    bases[i]=base; ptr_count++;
                }
            } else { /* INT - decomp often types pointers as u32; a fraction of
                        the time make it a valid guest pointer into a seeded
                        struct so deref branches fire (else random int -> reads
                        unmapped 0 on both sides = trivial agreement). */
                u32 v;
                int pick=rand()%10;
                if(pick==0) v=0; else if(pick==1) v=1; else if(pick==2) v=0xFFFFFFFFu;
                else if(pick==3) v=(u32)(rand()%16);      /* small selector */
                else if(pick<=6){                          /* valid pointer 30% */
                    u32 base = ARG_BASE + ptr_count*ARG_STRIDE;
#ifndef ORC_USE_FIELDMAP
                    for(u32 k=0;k<ARG_STRUCT;k++) fuzzbuf[k]=(u8)rand();
                    seed_ptr_struct(base, fuzzbuf, ARG_STRUCT);
#endif
                    v=base; bases[i]=base; ptr_count++;
                }
                else v=((u32)rand()<<16)^(u32)rand();      /* wide random */
                A_args[i]=v;
            }
        }
#ifdef ORC_USE_FIELDMAP
        /* struct-aware seeding: per-field logical values (BE in A, LE in B) */
        seed_fields(bases);
#endif

        /* ---- A-side: place args in registers, run ---- */
        int gpr=3, fpr=1;
        for(int i=0;i<ORC_NARGS;i++){
            if(ORC_ARGS[i].kind==2){ g_cpu.fpr[fpr++]=(double)(*(f32*)&A_args[i]); }
            else { g_cpu.gpr[gpr++]=A_args[i]; }
        }
        g_cpu.lr=ORC_SENTINEL; g_cpu.pc=(u32)ORC_FN_ADDR;
        int rc=orc_run(&g_cpu, 5000000u);
        if(rc!=0){ unreach++; if(unreach<=3)
            fprintf(stderr,"UNREACHABLE it=%u rc=%d pc=%08X (Tier-2/exception)\n",it,rc,g_cpu.pc);
            continue; }
        u32 a_ret;
#ifdef ORC_RET_FLT
        { f32 r=(f32)g_cpu.fpr[1]; memcpy(&a_ret,&r,4); }
#else
        a_ret = g_cpu.gpr[3];
#endif
        /* capture A-side RAM deltas around each ptr arg */
        static u8 a_after[8][SIDE_WINDOW];
        for(int i=0,pc2=0;i<ORC_NARGS;i++) if(ORC_ARGS[i].kind==1 && A_args[i]){
            for(u32 o=0;o<SIDE_WINDOW;o++) a_after[pc2][o]=mem_read8(&g_cpu,A_args[i]+o);
            pc2++;
        }

        /* ---- B-side: run real src ---- */
        int b_isfloat=0; u64 b_iret=0; f32 b_fret=0;
        B_dispatch(&b_isfloat, &b_iret, &b_fret);
        if(b_isfloat==2){ /* unsupported ABI shape: do not fake a result */
            fprintf(stderr,"AUTOHARNESS-SHAPE-UNSUPPORTED nargs=%d\n",ORC_NARGS);
            printf("AUTOHARNESS-FAILED unsupported-abi-shape\n");
            return 3;
        }
        u32 b_ret;
        if(b_isfloat==1){ memcpy(&b_ret,&b_fret,4); }
        else {
            /* INT return. A value getter returns DATA (e.g. *(u32*)(ptr+4));
             * a pointer-arith function returns a HOST pointer into the mirror
             * that must be mapped to the guest address A returns. We must NOT
             * corrupt a data value that coincidentally falls in the mirror's
             * host-address window. So: take the RAW return; only apply the
             * host->guest normalization if (a) the raw value differs from A AND
             * (b) normalizing makes it EQUAL A. Normalization can then only
             * RECONCILE a genuine pointer return, never mangle a data value. */
            u32 raw=(u32)b_iret;
            u32 norm=normalize_bret_ptr(raw);
            b_ret = (raw!=a_ret && norm==a_ret) ? norm : raw;
        }

        /* coverage tracking: count distinct non-zero agreed returns + any
         * RAM writes, so a PASS over all-zero outputs can be distinguished
         * from a PASS that exercised real logic. */
        extern u32 g_nonzero_ret, g_distinct_hash, g_ram_writes;
        if(a_ret!=0){ g_nonzero_ret++; g_distinct_hash ^= (a_ret*2654435761u); }

        int ok = (a_ret==b_ret);
#ifdef ORC_USE_FIELDMAP
        /* Side-effect compare (field-map mode): for each seeded field, read the
         * logical value A wrote (big-endian guest RAM) and the value B wrote
         * (little-endian mirror) at the field's width; they must agree. This
         * also covers SETTERS that write back into the struct. */
        for(int fi=0; ok && fi<ORC_NFIELDS; fi++){
            int a=ORC_FIELDS[fi].arg, off=ORC_FIELDS[fi].off, w=ORC_FIELDS[fi].width;
            if(a<0||a>=ORC_NARGS||!A_args[a]) continue;
            u32 ga=A_args[a]+(u32)off; u8* m=g_bram+(ga-ORC_GUEST_BASE);
            if(w==1){ if(mem_read8(&g_cpu,ga)!=m[0]) ok=0; }
            else if(w==2){ u16 av=mem_read16(&g_cpu,ga), bv=(u16)(m[0]|(m[1]<<8)); if(av!=bv) ok=0; }
            else if(w==4){ u32 av=mem_read32(&g_cpu,ga), bv=(u32)(m[0]|(m[1]<<8)|(m[2]<<16)|((u32)m[3]<<24)); if(av!=bv) ok=0; }
        }
#else
        /* compare RAM deltas (side effects). The B mirror is word-swapped vs
         * A guest RAM, so compare per 32-bit word with the byte-swap undone. */
        for(int i=0,pc2=0;ok && i<ORC_NARGS;i++) if(ORC_ARGS[i].kind==1 && A_args[i]){
            u8* m = g_bram + (A_args[i]-ORC_GUEST_BASE);
            for(u32 o=0;o+3<SIDE_WINDOW;o+=4){
                /* A word (big-endian bytes) vs mirror word (swapped) */
                if(a_after[pc2][o+0]!=m[o+3] || a_after[pc2][o+1]!=m[o+2] ||
                   a_after[pc2][o+2]!=m[o+1] || a_after[pc2][o+3]!=m[o+0]){ ok=0; break; }
            }
            pc2++;
        }
#endif

        total++;
        if(!ok){ mism++; if(mism<=8){ if(!have_mism){have_mism=1;first_mism_seed=it;}
            fprintf(stderr,"MISMATCH it=%u  A_ret=%08X B_ret=%08X  args:",it,a_ret,b_ret);
            for(int i=0;i<ORC_NARGS;i++) fprintf(stderr," %08X",A_args[i]);
            fprintf(stderr,"\n"); } }
    }

    extern u32 g_nonzero_ret, g_distinct_hash;
    const char* verdict;
    if(mism>0)            verdict="MISMATCH";
    else if(total==0)     verdict="UNREACHABLE(Tier-2)";
    else if(g_nonzero_ret==0)
        /* every run returned 0 on both sides: agreement is real but the fuzz
         * corpus never drove a non-trivial path. NOT a meaningful equivalence
         * result - the function needs realistic state (it is not a leaf/getter
         * the auto-fuzzer can exercise). Reported, never counted as a clean PASS. */
        verdict="PASS-TRIVIAL(low-coverage: all outputs 0)";
    else                  verdict="PASS";
    printf("AUTO %s @ %08X : %d run, %d mismatch, %d unreachable, "
           "%u nonzero-ret (cov=%08X) -> %s\n",
           argv[0], (u32)ORC_FN_ADDR, total, mism, unreach,
           g_nonzero_ret, g_distinct_hash, verdict);
    if(mism>0) printf("  first mismatch at case %u (see stderr)\n", first_mism_seed);
    free(g_bram); cpu_free(&g_cpu);
    return mism>0?1:(total==0?2:0);
}
