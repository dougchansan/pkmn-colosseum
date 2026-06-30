/**
 * @file hsd_host.c
 * @brief PC-port host implementations for HSD memory + display entry points.
 *
 * These four symbols have NO real decompiled C in src/hsd and would otherwise
 * be filled in by pcport_link.py's auto-stub generator (which emits a wrong-
 * signature `int sym(){return 0;}` no-op). We provide correctly-typed host
 * implementations here so the link resolves them functionally:
 *
 *   - HSD_MemAlloc / HSD_Free
 *       The HSD allocator. No decompiled C exists for these (asm-only in the
 *       original); HSD_JObjAlloc / HSD_FObjAlloc / HSD_*LoadDesc etc. all route
 *       through them. A malloc/free wrapper with the exact HSD signature is the
 *       correct host behaviour and resolves the entire alloc/free-list chain
 *       (no separate HSD_ObjAlloc/HSD_ObjFree symbols are referenced from C —
 *       they appear only in config comments, so nothing to stub for them).
 *
 *   - HSD_JObjDispAll
 *       The real implementation lives in src/hsd/hsd_jobj_display.c but ONLY as
 *       register-style asm pseudo-C (fn_801A1988 ... fn_801A3FBC); the named
 *       symbol `HSD_JObjDispAll` is never defined there, so it links unresolved.
 *       That TU is not host-portable as-is, so we provide a minimal functional
 *       host impl (scene-graph walk; actual GX emission is handled by the
 *       pcport GX shim elsewhere — display here is a safe no-op-per-node).
 *
 *   - HSD_FObjInterpretAnimAll
 *       Declared in include/hsd/hsd_fobj.h and called from src/hsd/hsd_aobj.c,
 *       but never defined in C (src/hsd/hsd_fobj.c has only load/alloc/remove).
 *       We provide a minimal, crash-safe host impl that walks the FObj list.
 *
 * NOTE on HSD_JObjDispAll signature: include/hsd/hsd_jobj.h declares the
 * canonical 4-arg form; src/hsd/hsd_texp.c re-declares a 3-arg form locally and
 * calls it with 3 args. In C there is no name mangling, so a single linker
 * symbol satisfies both. We define the canonical 4-arg header signature; the
 * 3-arg call site simply leaves `rendermode` indeterminate, which is harmless
 * for the host display path.
 *
 * PCPORT-only. Never compiled into the CodeWarrior byte-match build.
 */

#ifdef PCPORT

#include "dolphin/types.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_jobj.h"
#include "hsd/hsd_dobj.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_tobj.h"
#include "hsd/hsd_pobj.h"
#include "hsd/hsd_robj.h"
#include "hsd/hsd_forward.h"
#include "real_content_host.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* ========================================================================= */
/*  HSD allocator (host wrapper over malloc/free)                            */
/* ========================================================================= */

/* Signatures must match include/hsd/hsd_memory.h exactly:
 *   void* HSD_MemAlloc(s32 size);
 *   void  HSD_Free(void* ptr);
 */
void* HSD_MemAlloc(s32 size)
{
    void* ptr;

    if (size <= 0) {
        return NULL;
    }
    ptr = malloc((size_t) size);
    /* HSD callers (HSD_FObjAlloc, HSD_JObjAlloc, ...) memset after alloc, but
     * zero here too so any caller that forgets gets clean memory. */
    if (ptr != NULL) {
        memset(ptr, 0, (size_t) size);
    }
    return ptr;
}

void HSD_Free(void* ptr)
{
    if (ptr != NULL) {
        free(ptr);
    }
}

/* Host-only class fallback. The bootstrap link intentionally avoids the full
 * asm-heavy HSD class TU, but several host HSD loaders only need basic
 * zeroed allocation and "unknown class" lookup behavior. */
void* hsdNew(HSD_ClassInfo* info)
{
    HSD_Class* object;
    s32 size;

    size = info != NULL && info->head.obj_size > 0 ?
        info->head.obj_size : (s32) sizeof(HSD_Class);
    object = (HSD_Class*) HSD_MemAlloc(size);
    if (object != NULL) {
        object->class_info = info;
        if (info != NULL && info->init != NULL) {
            info->init(object);
        }
    }
    return object;
}

HSD_ClassInfo* hsdSearchClassInfo(const char* class_name)
{
    (void) class_name;
    return NULL;
}

HSD_JObj* HSD_JObjAlloc(void)
{
    HSD_JObj* jobj = (HSD_JObj*) HSD_MemAlloc((s32) sizeof(HSD_JObj));

    if (jobj != NULL) {
        memset(jobj, 0, sizeof(HSD_JObj));
    }
    return jobj;
}

void HSD_JObjUnref(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        HSD_Free(jobj);
    }
}

/* ========================================================================= */
/*  TExp construction bridge (host override for placeholder labels)           */
/* ========================================================================= */

#define PCPORT_HSD_TE_ZERO 0U
#define PCPORT_HSD_TE_TEV  1U
#define PCPORT_HSD_TE_TEX  2U
#define PCPORT_HSD_TE_RAS  3U
#define PCPORT_HSD_TE_CNST 4U

#define PCPORT_HSD_TE_RGB 1U
#define PCPORT_HSD_TE_A   5U
#define PCPORT_HSD_TE_X   6U
#define PCPORT_HSD_TE_0   7U
#define PCPORT_HSD_TE_1   8U

typedef struct PCPort_TEArg {
    u8 type;
    u8 sel;
    u8 arg;
    u8 pad;
    HSD_TExp* exp;
} PCPort_TEArg;

typedef struct PCPort_TETev {
    u32 type;
    HSD_TExp* next;
    s32 c_ref;
    u8 c_dst;
    u8 c_op;
    u8 c_bias;
    u8 c_scale;
    u8 c_clamp;
    u8 c_pad[3];
    s32 a_ref;
    u8 a_dst;
    u8 a_op;
    u8 a_bias;
    u8 a_scale;
    u8 a_clamp;
    u8 a_pad[3];
    PCPort_TEArg c_in[4];
    PCPort_TEArg a_in[4];
    HSD_TObj* tex;
    u8 chan;
    u8 pad[3];
} PCPort_TETev;

typedef struct PCPort_TECnst {
    u32 type;
    HSD_TExp* next;
    void* val;
    u32 comp;
    u32 ctype;
    u8 reg;
    u8 idx;
    u8 ref;
    u8 pad;
} PCPort_TECnst;

union HSD_TExp {
    u32 type;
    struct {
        u32 type;
        HSD_TExp* next;
    } comm;
    PCPort_TETev tev;
    PCPort_TECnst cnst;
};

static u32 PCPort_TExpGetType(HSD_TExp* texp)
{
    uintptr_t raw = (uintptr_t) texp;

    if (raw == 0U) {
        return PCPORT_HSD_TE_ZERO;
    }
    if (raw == (uintptr_t) -1) {
        return PCPORT_HSD_TE_TEX;
    }
    if (raw == (uintptr_t) -2) {
        return PCPORT_HSD_TE_RAS;
    }
    return texp->type;
}

static HSD_TExp* PCPort_TExpAlloc(u32 type)
{
    HSD_TExp* texp = (HSD_TExp*) HSD_MemAlloc((s32) sizeof(HSD_TExp));

    if (texp == NULL) {
        return NULL;
    }
    memset(texp, 0, sizeof(HSD_TExp));
    texp->type = type;
    return texp;
}

static void PCPort_TExpLink(HSD_TExp** list, HSD_TExp* texp)
{
    if (list == NULL || texp == NULL) {
        return;
    }
    texp->comm.next = *list;
    *list = texp;
}

static void PCPort_TExpInitArg(PCPort_TEArg* arg)
{
    arg->type = (u8) PCPORT_HSD_TE_ZERO;
    arg->sel = (u8) PCPORT_HSD_TE_0;
    arg->arg = 0xFF;
    arg->pad = 0;
    arg->exp = NULL;
}

static void PCPort_TExpSetArg(PCPort_TEArg* arg, u32 sel, HSD_TExp* exp)
{
    arg->type = (u8) PCPort_TExpGetType(exp);
    arg->sel = (u8) sel;
    arg->arg = 0xFF;
    arg->pad = 0;
    arg->exp = exp;
}

static BOOL PCPort_TExpIsTev(HSD_TExp* texp)
{
    return PCPort_TExpGetType(texp) == PCPORT_HSD_TE_TEV;
}

static HSD_TExp* PCPort_TExpFindCnst(HSD_TExp** list, void* val, u32 comp,
                                     u32 ctype)
{
    HSD_TExp* texp;

    if (list == NULL) {
        return NULL;
    }
    for (texp = *list; texp != NULL; texp = texp->comm.next) {
        if (PCPort_TExpGetType(texp) == PCPORT_HSD_TE_CNST &&
            texp->cnst.val == val && texp->cnst.comp == comp &&
            texp->cnst.ctype == ctype) {
            return texp;
        }
    }
    return NULL;
}

HSD_TExp* fn_801B707C(HSD_TExp** list)
{
    HSD_TExp* texp = PCPort_TExpAlloc(PCPORT_HSD_TE_TEV);
    s32 i;

    if (texp == NULL) {
        return NULL;
    }
    texp->tev.c_ref = 0;
    texp->tev.c_dst = 0;
    texp->tev.c_op = 0;
    texp->tev.c_bias = 0;
    texp->tev.c_scale = 0;
    texp->tev.c_clamp = 1;
    texp->tev.a_ref = 0;
    texp->tev.a_dst = 0;
    texp->tev.a_op = 0;
    texp->tev.a_bias = 0;
    texp->tev.a_scale = 0;
    texp->tev.a_clamp = 1;
    texp->tev.tex = NULL;
    texp->tev.chan = 0xFF;
    for (i = 0; i < 4; i++) {
        PCPort_TExpInitArg(&texp->tev.c_in[i]);
        PCPort_TExpInitArg(&texp->tev.a_in[i]);
    }
    PCPort_TExpLink(list, texp);
    return texp;
}

HSD_TExp* fn_801B6F5C(void* val, u32 comp, u32 ctype, HSD_TExp** list)
{
    HSD_TExp* texp = PCPort_TExpFindCnst(list, val, comp, ctype);

    if (texp != NULL) {
        return texp;
    }
    texp = PCPort_TExpAlloc(PCPORT_HSD_TE_CNST);
    if (texp == NULL) {
        return NULL;
    }
    texp->cnst.val = val;
    texp->cnst.comp = comp;
    texp->cnst.ctype = ctype;
    texp->cnst.reg = 0xFF;
    texp->cnst.idx = 0xFF;
    texp->cnst.ref = 0;
    PCPort_TExpLink(list, texp);
    return texp;
}

void fn_801B5E40(HSD_TExp* texp, HSD_TObj* tobj, u32 chan)
{
    if (!PCPort_TExpIsTev(texp)) {
        return;
    }
    texp->tev.tex = tobj;
    texp->tev.chan = (u8) chan;
}

void fn_801B6E74(HSD_TExp* texp, u32 op, u32 bias, u32 scale, u32 clamp)
{
    if (!PCPort_TExpIsTev(texp)) {
        return;
    }
    texp->tev.c_op = (u8) op;
    texp->tev.c_bias = (u8) bias;
    texp->tev.c_scale = (u8) scale;
    texp->tev.c_clamp = (u8) clamp;
}

void fn_801B6CD8(HSD_TExp* texp, u32 op, u32 bias, u32 scale, u32 clamp)
{
    if (!PCPort_TExpIsTev(texp)) {
        return;
    }
    texp->tev.a_op = (u8) op;
    texp->tev.a_bias = (u8) bias;
    texp->tev.a_scale = (u8) scale;
    texp->tev.a_clamp = (u8) clamp;
}

void fn_801B64EC(HSD_TExp* texp, u32 sel0, HSD_TExp* exp0, u32 sel1,
                 HSD_TExp* exp1, u32 sel2, HSD_TExp* exp2, u32 sel3,
                 HSD_TExp* exp3)
{
    if (!PCPort_TExpIsTev(texp)) {
        return;
    }
    PCPort_TExpSetArg(&texp->tev.c_in[0], sel0, exp0);
    PCPort_TExpSetArg(&texp->tev.c_in[1], sel1, exp1);
    PCPort_TExpSetArg(&texp->tev.c_in[2], sel2, exp2);
    PCPort_TExpSetArg(&texp->tev.c_in[3], sel3, exp3);
}

void fn_801B5F08(HSD_TExp* texp, u32 sel0, HSD_TExp* exp0, u32 sel1,
                 HSD_TExp* exp1, u32 sel2, HSD_TExp* exp2, u32 sel3,
                 HSD_TExp* exp3)
{
    if (!PCPort_TExpIsTev(texp)) {
        return;
    }
    PCPort_TExpSetArg(&texp->tev.a_in[0], sel0, exp0);
    PCPort_TExpSetArg(&texp->tev.a_in[1], sel1, exp1);
    PCPort_TExpSetArg(&texp->tev.a_in[2], sel2, exp2);
    PCPort_TExpSetArg(&texp->tev.a_in[3], sel3, exp3);
}

s32 fn_801B7C60(HSD_TExp* texp)
{
    return PCPort_TExpIsTev(texp) ? 1 : 0;
}

s32 fn_801B4300(void* tevdesc_out, HSD_TExp** texp_list)
{
    (void) texp_list;
    if (tevdesc_out != NULL) {
        *(void**) tevdesc_out = NULL;
    }
    return 0;
}

static HSD_TExp* PCPort_TExpTexSource(void)
{
    return (HSD_TExp*) (uintptr_t) -1;
}

static HSD_TExp* PCPort_TExpPrevOrTex(HSD_TExp* previous)
{
    return previous != NULL ? previous : PCPort_TExpTexSource();
}

void PCPort_TObjMakeTExp(HSD_TObj* tobj, u32 lightmap, u32 lightmap_done,
                         HSD_TExp** c, HSD_TExp** a, HSD_TExp** list)
{
    HSD_TExp* texp;
    HSD_TExp* prev_c;
    HSD_TExp* prev_a;

    (void) lightmap;
    (void) lightmap_done;
    if (tobj == NULL || c == NULL || a == NULL || list == NULL) {
        return;
    }

    prev_c = *c;
    prev_a = *a;
    texp = fn_801B707C(list);
    if (texp == NULL) {
        return;
    }
    fn_801B5E40(texp, tobj, 0xFF);

    fn_801B6E74(texp, 0, 0, 0, 1);
    switch (tobj_colormap(tobj)) {
    case TEX_COLORMAP_REPLACE:
        fn_801B64EC(texp, PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_RGB, PCPort_TExpTexSource());
        *c = texp;
        break;
    case TEX_COLORMAP_NONE:
    case TEX_COLORMAP_PASS:
        fn_801B64EC(texp, PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_RGB, PCPort_TExpPrevOrTex(prev_c));
        *c = texp;
        break;
    case TEX_COLORMAP_ADD:
    case TEX_COLORMAP_SUB:
    case TEX_COLORMAP_BLEND:
    case TEX_COLORMAP_ALPHA_MASK:
    case TEX_COLORMAP_RGB_MASK:
    case TEX_COLORMAP_MODULATE:
    default:
        fn_801B64EC(texp, PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_RGB, PCPort_TExpTexSource(),
                    PCPORT_HSD_TE_RGB, prev_c,
                    PCPORT_HSD_TE_0, NULL);
        *c = texp;
        break;
    }

    fn_801B6CD8(texp, 0, 0, 0, 1);
    switch (tobj_alphamap(tobj)) {
    case TEX_ALPHAMAP_REPLACE:
        fn_801B5F08(texp, PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_A, PCPort_TExpTexSource());
        *a = texp;
        break;
    case TEX_ALPHAMAP_NONE:
    case TEX_ALPHAMAP_PASS:
        fn_801B5F08(texp, PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_A, PCPort_TExpPrevOrTex(prev_a));
        *a = texp;
        break;
    case TEX_ALPHAMAP_ADD:
    case TEX_ALPHAMAP_SUB:
    case TEX_ALPHAMAP_BLEND:
    case TEX_ALPHAMAP_ALPHA_MASK:
    case TEX_ALPHAMAP_MODULATE:
    default:
        fn_801B5F08(texp, PCPORT_HSD_TE_0, NULL,
                    PCPORT_HSD_TE_A, PCPort_TExpTexSource(),
                    PCPORT_HSD_TE_A, prev_a,
                    PCPORT_HSD_TE_0, NULL);
        *a = texp;
        break;
    }
}

static void PCPort_MObjSeedMaterial(HSD_MObj* mobj, HSD_TExp** c,
                                    HSD_TExp** a, HSD_TExp** list)
{
    HSD_TExp* texp;
    HSD_TExp* diffuse;
    HSD_TExp* alpha;

    if (c == NULL || a == NULL || list == NULL) {
        return;
    }
    if (mobj == NULL || mobj->mat == NULL) {
        *c = NULL;
        *a = NULL;
        return;
    }

    texp = fn_801B707C(list);
    if (texp == NULL) {
        return;
    }
    diffuse = fn_801B6F5C(&mobj->mat->diffuse, PCPORT_HSD_TE_RGB, 0, list);
    alpha = fn_801B6F5C(&mobj->mat->alpha, PCPORT_HSD_TE_X, 3, list);

    fn_801B6E74(texp, 0, 0, 0, 1);
    fn_801B64EC(texp, PCPORT_HSD_TE_0, NULL,
                PCPORT_HSD_TE_0, NULL,
                PCPORT_HSD_TE_0, NULL,
                diffuse != NULL ? PCPORT_HSD_TE_RGB : PCPORT_HSD_TE_1,
                diffuse);
    fn_801B6CD8(texp, 0, 0, 0, 1);
    fn_801B5F08(texp, PCPORT_HSD_TE_0, NULL,
                PCPORT_HSD_TE_0, NULL,
                PCPORT_HSD_TE_0, NULL,
                alpha != NULL ? PCPORT_HSD_TE_X : PCPORT_HSD_TE_1,
                alpha);
    *c = texp;
    *a = texp;
}

HSD_TExp* PCPort_MObjMakeTExp(HSD_MObj* mobj, HSD_TObj* tobj_top,
                              HSD_TExp** list)
{
    HSD_TExp* color;
    HSD_TExp* alpha;
    HSD_TExp* root;
    HSD_TObj* tobj;
    u32 lightmap_done;

    if (list == NULL) {
        return NULL;
    }
    *list = NULL;
    color = NULL;
    alpha = NULL;
    lightmap_done = 0;

    PCPort_MObjSeedMaterial(mobj, &color, &alpha, list);
    for (tobj = tobj_top; tobj != NULL; tobj = tobj->next) {
        HSD_ClassInfo* info;
        HSD_TObjInfo* method;
        u32 lightmap;

        if ((tobj->id & 0xFF) == 0xFF) {
            continue;
        }
        info = tobj->parent.parent.class_info;
        method = info != NULL ? HSD_TOBJ_INFO(info) : NULL;
        lightmap = tobj_lightmap(tobj);
        if (method != NULL && method->make_texp != NULL) {
            method->make_texp(tobj, lightmap, lightmap_done, &color, &alpha,
                              list);
        } else {
            PCPort_TObjMakeTExp(tobj, lightmap, lightmap_done, &color, &alpha,
                                list);
        }
        lightmap_done |= lightmap;
    }

    if (color == NULL && alpha == NULL) {
        return *list;
    }
    if (color == alpha && fn_801B7C60(color)) {
        return color;
    }

    root = fn_801B707C(list);
    if (root == NULL) {
        return color != NULL ? color : alpha;
    }
    fn_801B6E74(root, 0, 0, 0, 1);
    fn_801B64EC(root, PCPORT_HSD_TE_0, NULL,
                PCPORT_HSD_TE_0, NULL,
                PCPORT_HSD_TE_0, NULL,
                PCPORT_HSD_TE_RGB, color);
    fn_801B6CD8(root, 0, 0, 0, 1);
    fn_801B5F08(root, PCPORT_HSD_TE_0, NULL,
                PCPORT_HSD_TE_0, NULL,
                PCPORT_HSD_TE_0, NULL,
                PCPORT_HSD_TE_A, alpha != NULL ? alpha : color);
    return root;
}

/* ========================================================================= */
/*  HSD_JObjLoadJoint — build a live HSD_JObj tree from a HSD_Joint desc      */
/* ========================================================================= */

/* The real HSD_JObjLoadJoint is asm-only in the original (no decompiled C),
 * so it would auto-stub to a NULL-returning no-op. This is a faithful host
 * functional implementation of the standard HSD joint-tree load: it allocates
 * each JObj via the real HSD_JObjAlloc, copies the joint's flags + S/R/T, loads
 * the attached display object via the GAME'S real HSD_DObjLoadDesc (which loads
 * the real MObj/PObj/TObj incl. their animation), recurses children + siblings,
 * and marks the matrix dirty. All leaf work is the game's own code; this is just
 * the recursive glue the asm function would otherwise perform. Building the real
 * HSD_JObj tree is the prerequisite for running the game's HSD_JObjAnimAll
 * (texture-matrix UV scroll = the title "sand") + its real render. */
/* PC host detail: the helpers below load all canonical JObjs first, then
 * resolve post-load refs. Batch 5A made the host JObj walkers graph-aware, so
 * JOBJ_INSTANCE children can now be wired to the canonical live child instead
 * of being loaded as private duplicate subtrees. */

typedef struct PCPort_JObjLoadEntry {
    HSD_Joint* joint;
    HSD_JObj* jobj;
} PCPort_JObjLoadEntry;

typedef struct PCPort_JObjLoadContext {
    BOOL failed;
    PCPort_JObjLoadEntry* entries;
    size_t entryCount;
    size_t entryCapacity;
} PCPort_JObjLoadContext;

static void PCPort_JObjLoadContextDestroy(PCPort_JObjLoadContext* ctx)
{
    free(ctx->entries);
    ctx->entries = NULL;
    ctx->entryCount = 0;
    ctx->entryCapacity = 0;
}

static BOOL PCPort_JObjLoadContextGrow(PCPort_JObjLoadContext* ctx)
{
    PCPort_JObjLoadEntry* entries;
    size_t newCapacity;

    newCapacity = ctx->entryCapacity == 0 ? 32 : ctx->entryCapacity * 2;
    if (newCapacity <= ctx->entryCapacity) {
        ctx->failed = TRUE;
        return FALSE;
    }

    entries = (PCPort_JObjLoadEntry*) realloc(
        ctx->entries, newCapacity * sizeof(ctx->entries[0]));
    if (entries == NULL) {
        ctx->failed = TRUE;
        return FALSE;
    }

    ctx->entries = entries;
    ctx->entryCapacity = newCapacity;
    return TRUE;
}

static BOOL PCPort_JObjLoadContextRegister(PCPort_JObjLoadContext* ctx,
                                           HSD_Joint* joint,
                                           HSD_JObj* jobj)
{
    size_t i;

    if (joint == NULL || jobj == NULL) {
        return TRUE;
    }

    for (i = 0; i < ctx->entryCount; i++) {
        if (ctx->entries[i].joint == joint) {
            ctx->entries[i].jobj = jobj;
            return TRUE;
        }
    }

    if (ctx->entryCount >= ctx->entryCapacity &&
        !PCPort_JObjLoadContextGrow(ctx))
    {
        return FALSE;
    }

    ctx->entries[ctx->entryCount].joint = joint;
    ctx->entries[ctx->entryCount].jobj = jobj;
    ctx->entryCount++;
    return TRUE;
}

static HSD_JObj* PCPort_JObjLoadContextFind(PCPort_JObjLoadContext* ctx,
                                            HSD_Joint* joint)
{
    size_t i;

    if (joint == NULL) {
        return NULL;
    }

    for (i = 0; i < ctx->entryCount; i++) {
        if (ctx->entries[i].joint == joint) {
            return ctx->entries[i].jobj;
        }
    }
    return NULL;
}

static HSD_JObj* PCPort_JObjAllocForJoint(HSD_Joint* joint)
{
    HSD_ClassInfo* info;

    if (joint != NULL && joint->class_name != NULL) {
        info = hsdSearchClassInfo(joint->class_name);
        if (info != NULL) {
            return (HSD_JObj*) hsdNew(info);
        }
    }
    return HSD_JObjAlloc();
}

static void PCPort_JObjLoadPayload(HSD_JObj* jobj, HSD_Joint* joint)
{
    jobj->flags |= joint->flags;
    jobj->rotate_x = joint->rotation_x;
    jobj->rotate_y = joint->rotation_y;
    jobj->rotate_z = joint->rotation_z;
    jobj->scale_x = joint->scale_x;
    jobj->scale_y = joint->scale_y;
    jobj->scale_z = joint->scale_z;
    jobj->translate_x = joint->position_x;
    jobj->translate_y = joint->position_y;
    jobj->translate_z = joint->position_z;
    jobj->scl = NULL;
    jobj->envelopemtx = joint->mtx;

    if (jobj->flags & JOBJ_SPLINE) {
        jobj->u.spline = joint->u.spline;
    } else if (jobj->flags & JOBJ_PTCL) {
        jobj->u.ptcl = joint->u.ptcl;
    } else if (joint->u.dobjdesc != NULL) {
        jobj->u.dobj = HSD_DObjLoadDesc(joint->u.dobjdesc);
    }
    if (joint->robjdesc != NULL) {
        jobj->robj = HSD_RObjLoadDesc(joint->robjdesc);
    }
}

static HSD_JObj* PCPort_JObjLoadTree(HSD_Joint* joint,
                                     HSD_JObj* parent,
                                     PCPort_JObjLoadContext* ctx)
{
    HSD_JObj* first;
    HSD_JObj* prev;
    HSD_JObj* jobj;

    first = NULL;
    prev = NULL;
    while (joint != NULL && !ctx->failed) {
        jobj = PCPort_JObjAllocForJoint(joint);
        if (jobj == NULL) {
            ctx->failed = TRUE;
            break;
        }

        jobj->next = NULL;
        jobj->parent = parent;
        jobj->child = NULL;
        PCPort_JObjLoadPayload(jobj, joint);

        if (prev != NULL) {
            prev->next = jobj;
        } else {
            first = jobj;
        }
        prev = jobj;
        if (!PCPort_JObjLoadContextRegister(ctx, joint, jobj)) {
            break;
        }

        if (!(jobj->flags & JOBJ_INSTANCE)) {
            jobj->child = PCPort_JObjLoadTree(joint->child, jobj, ctx);
        }
        HSD_JObjSetMtxDirty(jobj);
        joint = joint->next;
    }
    return first;
}

static void PCPort_JObjResolveTree(HSD_JObj* jobj,
                                   HSD_Joint* joint,
                                   PCPort_JObjLoadContext* ctx)
{
    while (jobj != NULL && joint != NULL) {
        if (jobj->robj != NULL && joint->robjdesc != NULL) {
            HSD_RObjResolveRefsAll(jobj->robj, joint->robjdesc);
        }
        if (union_type_dobj(jobj) && jobj->u.dobj != NULL) {
            HSD_DObjResolveRefsAll(jobj->u.dobj, joint->u.dobjdesc);
        }

        if (jobj->flags & JOBJ_INSTANCE) {
            if (joint->child != NULL) {
                jobj->child = PCPort_JObjLoadContextFind(ctx, joint->child);
                if (jobj->child == NULL) {
                    ctx->failed = TRUE;
                    return;
                }
            }
        } else {
            PCPort_JObjResolveTree(jobj->child, joint->child, ctx);
            if (ctx->failed) {
                return;
            }
        }

        jobj = jobj->next;
        joint = joint->next;
    }
}

HSD_JObj* HSD_JObjLoadJoint(HSD_Joint* joint)
{
    PCPort_JObjLoadContext ctx;
    HSD_JObj* root;

    if (joint == NULL) {
        return NULL;
    }

    memset(&ctx, 0, sizeof(ctx));
    root = PCPort_JObjLoadTree(joint, NULL, &ctx);
    if (!ctx.failed) {
        PCPort_JObjResolveTree(root, joint, &ctx);
    }
    if (ctx.failed && root != NULL) {
        HSD_JObjRemoveAll(root);
        root = NULL;
    }
    PCPort_JObjLoadContextDestroy(&ctx);
    return root;
}

HSD_JObj* fn_801A0FBC(HSD_Joint* joint)
{
    return HSD_JObjLoadJoint(joint);
}

static void PCPort_JObjRegisterTree(HSD_JObj* jobj,
                                    HSD_Joint* joint,
                                    PCPort_JObjLoadContext* ctx)
{
    while (jobj != NULL && joint != NULL && !ctx->failed) {
        PCPort_JObjLoadContextRegister(ctx, joint, jobj);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            PCPort_JObjRegisterTree(jobj->child, joint->child, ctx);
        }
        jobj = jobj->next;
        joint = joint->next;
    }
}

void HSD_JObjResolveRefsAll(HSD_JObj* jobj, HSD_Joint* joint)
{
    PCPort_JObjLoadContext ctx;

    if (jobj == NULL || joint == NULL) {
        return;
    }
    memset(&ctx, 0, sizeof(ctx));
    PCPort_JObjRegisterTree(jobj, joint, &ctx);
    PCPort_JObjResolveTree(jobj, joint, &ctx);
    PCPort_JObjLoadContextDestroy(&ctx);
}

/* ========================================================================= */
/*  HSD_JObjDispAll — scene-graph display entry (host)                       */
/* ========================================================================= */

/* Canonical signature from include/hsd/hsd_jobj.h:
 *   void HSD_JObjDispAll(HSD_JObj* jobj, f32 vmtx[3][4], u32 flags,
 *                        u32 rendermode);
 *
 * Walks the joint hierarchy (child + sibling chain) so reference/visibility
 * traversal behaves; actual primitive emission is performed by the pcport GX
 * shim path, so per-node display is a safe no-op here. Hidden joints and their
 * subtrees are skipped, matching JOBJ_HIDDEN semantics. */
void HSD_JObjDispAll(HSD_JObj* jobj, f32 vmtx[3][4], u32 flags, u32 rendermode)
{
    HSD_JObj* j;

    (void) vmtx;
    (void) flags;
    (void) rendermode;

    for (j = jobj; j != NULL; j = j->next) {
        if (j->flags & JOBJ_HIDDEN) {
            continue;
        }
        /* Per-node display (GX emission) is handled by the pcport GX shim.
         * Recurse into children to keep the full hierarchy traversed. */
        if (j->child != NULL) {
            HSD_JObjDispAll(j->child, vmtx, flags, rendermode);
        }
    }
}

/* ========================================================================= */
/*  HSD_FObjInterpretAnimAll — animation interpretation (host)              */
/* ========================================================================= */

/* Canonical signature from include/hsd/hsd_fobj.h:
 *   void HSD_FObjInterpretAnimAll(void* fobj, void* obj,
 *                                 HSD_ObjUpdateFunc obj_update, f32 rate);
 *
 * Faithful host decompilation of fn_80199A88: walk the FObj list and run the
 * per-FObj keyframe interpreter (HSD_FObjInterpretAnim, fn_80199AF8) on each.
 * The interpreter + FObjLoadData + cubic-Hermite eval live in hsd_fobj_host.c
 * (decompiled from the original GC asm; see that file). */
void HSD_FObjInterpretAnimAll(void* fobj, void* obj,
                              HSD_ObjUpdateFunc obj_update, f32 rate)
{
    HSD_FObj* f;

    for (f = (HSD_FObj*) fobj; f != NULL; f = f->next) {
        HSD_FObjInterpretAnim(f, obj, obj_update, rate);
    }
}

/* ========================================================================= */
/*  Animation dispatch (host) — FObj value -> JObj/TObj field                 */
/* ========================================================================= */

/*
 * VERIFIED DISPATCH MODEL (see lane verification + project memory):
 *   HSD_AObjInterpretAnim(aobj, obj, update_func) calls HSD_FObjInterpretAnimAll,
 *   which runs the per-FObj interpreter (hsd_fobj_host.c). That interpreter calls
 *   update_func(obj, fobj->obj_type, &value) for each produced value -- BUT ONLY
 *   when update_func != NULL. If NULL is passed (as the adapted src TObj/JObj C
 *   does), NO value is dispatched to any field and the animation has no effect.
 *
 *   On GC the real fn_801BBFE4/fn_801BC33C do the TObj texture-matrix build from
 *   already-updated TObj scalar fields (a separate pass over the bound TObj list);
 *   the actual FObj->field assignment is what the update_func is for. For the host
 *   we therefore PASS A REAL update_func so the FObj keyframe values flow into the
 *   live HSD_JObj / HSD_TObj SRT fields. These two dispatchers map the HSD_A_J_* /
 *   HSD_A_T_* obj_type codes (include/hsd/hsd_jobj.h, hsd_tobj.h) to the matching
 *   struct field.
 */

static void PCPort_JObjUpdateFunc(void* obj, u32 type, HSD_ObjData* val)
{
    HSD_JObj* jobj = (HSD_JObj*) obj;
    if (jobj == NULL || val == NULL) {
        return;
    }
    switch (type) {
    case HSD_A_J_ROTX: jobj->rotate_x    = val->fv; break;
    case HSD_A_J_ROTY: jobj->rotate_y    = val->fv; break;
    case HSD_A_J_ROTZ: jobj->rotate_z    = val->fv; break;
    case HSD_A_J_TRAX: jobj->translate_x = val->fv; break;
    case HSD_A_J_TRAY: jobj->translate_y = val->fv; break;
    case HSD_A_J_TRAZ: jobj->translate_z = val->fv; break;
    case HSD_A_J_SCAX: jobj->scale_x     = val->fv; break;
    case HSD_A_J_SCAY: jobj->scale_y     = val->fv; break;
    case HSD_A_J_SCAZ: jobj->scale_z     = val->fv; break;
    default:
        /* HSD_A_J_PATH / NODE / BRANCH / SETBYTE / SETFLOAT are not driven on the
         * host SRT path; ignore so an unexpected code never writes a stray field. */
        return;
    }
    HSD_JObjSetMtxDirty(jobj);
}

static void PCPort_TObjUpdateFunc(void* obj, u32 type, HSD_ObjData* val)
{
    HSD_TObj* tobj = (HSD_TObj*) obj;
    if (tobj == NULL || val == NULL) {
        return;
    }
    switch (type) {
    case HSD_A_T_TRAU: tobj->translate_x = val->fv; break;
    case HSD_A_T_TRAV: tobj->translate_y = val->fv; break;
    case HSD_A_T_SCAU: tobj->scale_x     = val->fv; break;
    case HSD_A_T_SCAV: tobj->scale_y     = val->fv; break;
    case HSD_A_T_ROTX: tobj->rotate_x    = val->fv; break;
    case HSD_A_T_ROTY: tobj->rotate_y    = val->fv; break;
    case HSD_A_T_ROTZ: tobj->rotate_z    = val->fv; break;
    case HSD_A_T_BLEND: tobj->blending   = val->fv; break;
    case HSD_A_T_TIMG: /* texture image swap from imagetbl[iv] */
        if (tobj->imagetbl != NULL && val->iv >= 0) {
            tobj->imagedesc = tobj->imagetbl[val->iv];
        }
        return; /* image swap does not dirty the tex matrix */
    case HSD_A_T_TCLT: /* TLUT swap from tluttbl[iv] */
        if (tobj->tluttbl != NULL && val->iv >= 0) {
            tobj->tlut = tobj->tluttbl[val->iv];
        }
        return;
    default:
        /* HSD_A_T_LOD_BIAS and any unknown code: no SRT effect. */
        return;
    }
    /* Any translate/rotate/scale/blend change invalidates the texture matrix. */
    tobj->flags |= TEX_MTX_DIRTY;
}

extern void MObjUpdateFunc(HSD_MObj* mobj, u32 type, HSD_ObjData* val);

/* ------------------------------------------------------------------------- */
/*  HSD_MObjAnim (host override)                                              */
/*                                                                           */
/*  The adapted src body only advances TObj animation, so material AObj keys  */
/*  never reach the MObj updater. Route MObj keys through the Colosseum        */
/*  material dispatcher, then preserve the existing TObj animation chain.      */
/* ------------------------------------------------------------------------- */
void HSD_MObjAnim(HSD_MObj* mobj)
{
    if (mobj == NULL) {
        return;
    }
    if (mobj->aobj != NULL) {
        HSD_AObjInterpretAnim(mobj->aobj, mobj, MObjUpdateFunc);
    }
    HSD_TObjAnimAll(mobj->tobj);
}

/* ------------------------------------------------------------------------- */
/*  HSD_TObjAnim (host override)                                              */
/*                                                                           */
/*  src/hsd/hsd_tobj.c defines HSD_TObjAnim with an EMPTY body (the adapted   */
/*  src never wired the dispatch). pcport/hsd_host.c is in the BOOT link set, */
/*  compiled+linked BEFORE the generated src/hsd TUs, so /FORCE:MULTIPLE      */
/*  takes THIS definition first -- a functional host override of the empty    */
/*  game stub. It interprets the TObj's AObj keys into the TObj SRT fields    */
/*  via PCPort_TObjUpdateFunc. (Reached through HSD_MObjAnim ->               */
/*  HSD_TObjAnimAll -> HSD_TObjAnim, which the src TUs still provide.)        */
/* ------------------------------------------------------------------------- */
void HSD_TObjAnim(HSD_TObj* tobj)
{
    if (tobj == NULL || tobj->aobj == NULL) {
        return;
    }
    HSD_AObjInterpretAnim(tobj->aobj, tobj, PCPort_TObjUpdateFunc);
}

void HSD_TObjAnimAll(HSD_TObj* tobj)
{
    while (tobj != NULL) {
        HSD_TObjAnim(tobj);
        tobj = tobj->next;
    }
}

static HSD_TObj* PCPort_TObjAlloc(void)
{
    HSD_TObj* tobj = (HSD_TObj*) HSD_MemAlloc((s32) sizeof(HSD_TObj));

    if (tobj != NULL) {
        memset(tobj, 0, sizeof(HSD_TObj));
    }
    return tobj;
}

static void PCPort_TObjLoad(HSD_TObj* tobj, HSD_TObjDesc* desc)
{
    if (tobj == NULL || desc == NULL) {
        return;
    }
    tobj->id = desc->id;
    tobj->src = desc->src;
    tobj->rotate_x = desc->rotate_x;
    tobj->rotate_y = desc->rotate_y;
    tobj->rotate_z = desc->rotate_z;
    tobj->scale_x = desc->scale_x;
    tobj->scale_y = desc->scale_y;
    tobj->scale_z = desc->scale_z;
    tobj->translate_x = desc->translate_x;
    tobj->translate_y = desc->translate_y;
    tobj->translate_z = desc->translate_z;
    tobj->wrap_s = desc->wrap_s;
    tobj->wrap_t = desc->wrap_t;
    tobj->repeat_s = desc->repeat_s;
    tobj->repeat_t = desc->repeat_t;
    tobj->flags = desc->blend_flags;
    tobj->blending = desc->blending;
    tobj->magFilt = desc->magFilt;
    tobj->imagedesc = desc->imagedesc;
    tobj->tlut = desc->tlutdesc;
    tobj->lod = desc->lod;
    tobj->tev = desc->tev;
}

HSD_TObj* HSD_TObjLoadDesc(HSD_TObjDesc* desc)
{
    HSD_TObj* first = NULL;
    HSD_TObj* prev = NULL;

    while (desc != NULL) {
        HSD_TObj* tobj = PCPort_TObjAlloc();
        if (tobj == NULL) {
            HSD_TObjRemoveAll(first);
            return NULL;
        }
        PCPort_TObjLoad(tobj, desc);
        if (prev != NULL) {
            prev->next = tobj;
        } else {
            first = tobj;
        }
        prev = tobj;
        desc = desc->next;
    }
    return first;
}

HSD_PObj* HSD_PObjLoadDesc(HSD_PObjDesc* desc)
{
    HSD_PObj* first = NULL;
    HSD_PObj* prev = NULL;

    while (desc != NULL) {
        HSD_PObj* pobj = (HSD_PObj*) HSD_MemAlloc((s32) sizeof(HSD_PObj));
        if (pobj == NULL) {
            HSD_PObjRemoveAll(first);
            return NULL;
        }
        memset(pobj, 0, sizeof(HSD_PObj));
        pobj->verts = desc->verts;
        pobj->flags = desc->flags;
        pobj->n_display = desc->n_display;
        pobj->display = desc->display;
        pobj->u.jobj = NULL;
        if (prev != NULL) {
            prev->next = pobj;
        } else {
            first = pobj;
        }
        prev = pobj;
        desc = desc->next;
    }
    return first;
}

HSD_DObj* HSD_DObjLoadDesc(HSD_DObjDesc* desc)
{
    HSD_DObj* first = NULL;
    HSD_DObj* prev = NULL;

    while (desc != NULL) {
        HSD_DObj* dobj = (HSD_DObj*) HSD_MemAlloc((s32) sizeof(HSD_DObj));
        if (dobj == NULL) {
            HSD_DObjRemoveAll(first);
            return NULL;
        }
        memset(dobj, 0, sizeof(HSD_DObj));
        dobj->mobj = HSD_MObjLoadDesc(desc->mobjdesc);
        dobj->pobj = HSD_PObjLoadDesc(desc->pobjdesc);
        if (prev != NULL) {
            prev->next = dobj;
        } else {
            first = dobj;
        }
        prev = dobj;
        desc = desc->next;
    }
    return first;
}

void HSD_TObjRemoveAll(HSD_TObj* tobj)
{
    while (tobj != NULL) {
        HSD_TObj* next = tobj->next;
        HSD_AObjRemove(tobj->aobj);
        HSD_Free(tobj);
        tobj = next;
    }
}

void HSD_FObjRemoveAll(HSD_FObj* fobj)
{
    while (fobj != NULL) {
        HSD_FObj* next = fobj->next;
        HSD_Free(fobj);
        fobj = next;
    }
}

HSD_MObj* HSD_MObjLoadDesc(HSD_MObjDesc* desc)
{
    HSD_MObj* mobj;

    if (desc == NULL) {
        return NULL;
    }
    mobj = (HSD_MObj*) HSD_MemAlloc((s32) sizeof(HSD_MObj));
    if (mobj == NULL) {
        return NULL;
    }
    memset(mobj, 0, sizeof(HSD_MObj));
    mobj->rendermode = desc->rendermode;
    mobj->tobj = HSD_TObjLoadDesc(desc->texdesc);
    mobj->mat = desc->mat;
    mobj->pe = desc->pedesc;
    return mobj;
}

void HSD_MObjSetAlpha(HSD_MObj* mobj, f32 alpha)
{
    if (mobj != NULL && mobj->mat != NULL) {
        mobj->mat->alpha = alpha;
    }
}

void HSD_MObjSetup(HSD_MObj* mobj, u32 rendermode)
{
    (void) rendermode;
    if (mobj != NULL) {
        PCPort_MObjMakeTExp(mobj, mobj->tobj, &mobj->texp);
    }
}

s32 fn_801A7D58(HSD_MObj* dst, HSD_MObj* src)
{
    if (dst == NULL || src == NULL) {
        return -1;
    }
    *dst = *src;
    dst->tobj = NULL;
    dst->aobj = NULL;
    dst->texp = NULL;
    return 0;
}

void MObjUpdateFunc(HSD_MObj* mobj, u32 type, HSD_ObjData* val)
{
    if (mobj == NULL || mobj->mat == NULL || val == NULL) {
        return;
    }
    switch (type) {
    case HSD_A_M_AMBIENT_R:
        mobj->mat->ambient = (mobj->mat->ambient & 0x00FFFFFFu) |
            (((u32) val->fv & 0xFFu) << 24);
        break;
    case HSD_A_M_AMBIENT_G:
        mobj->mat->ambient = (mobj->mat->ambient & 0xFF00FFFFu) |
            (((u32) val->fv & 0xFFu) << 16);
        break;
    case HSD_A_M_AMBIENT_B:
        mobj->mat->ambient = (mobj->mat->ambient & 0xFFFF00FFu) |
            (((u32) val->fv & 0xFFu) << 8);
        break;
    case HSD_A_M_DIFFUSE_R:
        mobj->mat->diffuse = (mobj->mat->diffuse & 0x00FFFFFFu) |
            (((u32) val->fv & 0xFFu) << 24);
        break;
    case HSD_A_M_DIFFUSE_G:
        mobj->mat->diffuse = (mobj->mat->diffuse & 0xFF00FFFFu) |
            (((u32) val->fv & 0xFFu) << 16);
        break;
    case HSD_A_M_DIFFUSE_B:
        mobj->mat->diffuse = (mobj->mat->diffuse & 0xFFFF00FFu) |
            (((u32) val->fv & 0xFFu) << 8);
        break;
    case HSD_A_M_ALPHA:
        mobj->mat->alpha = val->fv;
        break;
    default:
        break;
    }
}

/* ------------------------------------------------------------------------- */
/*  HSD_JObjAnim (host override)                                              */
/*                                                                           */
/*  src/hsd/hsd_jobj.c HSD_JObjAnim only runs HSD_RObjAnimAll + HSD_DObjAnim  */
/*  and never interprets the joint's OWN aobj -> joint SRT animation does     */
/*  nothing. This BOOT-set override adds the missing                          */
/*  HSD_AObjInterpretAnim(jobj->aobj, jobj, PCPort_JObjUpdateFunc) before the */
/*  RObj/DObj passes. HSD_JObjAnimAll (src) recurses calling HSD_JObjAnim, so */
/*  overriding just HSD_JObjAnim covers the whole tree.                       */
/* ------------------------------------------------------------------------- */
void HSD_JObjAnim(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return;
    }
    if (jobj->aobj != NULL) {
        HSD_AObjInterpretAnim(jobj->aobj, jobj, PCPort_JObjUpdateFunc);
    }
    HSD_RObjAnimAll(jobj->robj);
    if (union_type_dobj(jobj)) {
        /* DObjAnimAll walks the whole DObj chain (the src HSD_JObjAnim used the
         * single-DObj HSD_DObjAnim, which would skip chained material sets). */
        HSD_DObjAnimAll(jobj->u.dobj);
    }
}

void PCPort_HSDJObjAnimJointOnlyAll(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return;
    }
    if (jobj->aobj != NULL) {
        HSD_AObjInterpretAnim(jobj->aobj, jobj, PCPort_JObjUpdateFunc);
    }
    HSD_RObjAnimAll(jobj->robj);
    PCPort_HSDJObjAnimJointOnlyAll(jobj->child);
    PCPort_HSDJObjAnimJointOnlyAll(jobj->next);
}

void HSD_JObjSetFlags(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        jobj->flags |= flags;
    }
}

void HSD_JObjClearFlags(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        jobj->flags &= ~flags;
    }
}

void HSD_JObjRemoveAnim(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return;
    }
    HSD_AObjRemove(jobj->aobj);
    jobj->aobj = NULL;
    HSD_RObjRemoveAnimAll(jobj->robj);
    if (union_type_dobj(jobj)) {
        HSD_DObjRemoveAll(jobj->u.dobj);
        jobj->u.dobj = NULL;
    }
}

/* ------------------------------------------------------------------------- */
/*  Graph-safe JObj walkers (host overrides)                                  */
/*                                                                           */
/*  Batch 5A guardrail: Colosseum's JOBJ_INSTANCE/reference path can make the  */
/*  JObj relation graph share child chains instead of being a strict tree.     */
/*  The adapted src walkers recurse through child+next with no visited guard,  */
/*  which is unsafe once the loader starts preserving shared instance edges.   */
/*  These BOOT-set overrides keep the original per-node work but make every    */
/*  public all-walker process each JObj pointer at most once per call.         */
/* ------------------------------------------------------------------------- */

#define PCPORT_JOBJ_VISIT_INLINE_CAPACITY 1024u

typedef struct PCPort_JObjVisit {
    HSD_JObj** items;
    size_t count;
    size_t capacity;
    BOOL overflowed;
    HSD_JObj* inline_items[PCPORT_JOBJ_VISIT_INLINE_CAPACITY];
} PCPort_JObjVisit;

static void PCPort_JObjVisitInit(PCPort_JObjVisit* visit)
{
    visit->items = visit->inline_items;
    visit->count = 0;
    visit->capacity = PCPORT_JOBJ_VISIT_INLINE_CAPACITY;
    visit->overflowed = FALSE;
}

static void PCPort_JObjVisitDestroy(PCPort_JObjVisit* visit)
{
    if (visit->items != visit->inline_items) {
        free(visit->items);
    }
    visit->items = NULL;
    visit->count = 0;
    visit->capacity = 0;
}

static BOOL PCPort_JObjVisitGrow(PCPort_JObjVisit* visit)
{
    size_t new_capacity;
    HSD_JObj** new_items;

    new_capacity = visit->capacity * 2u;
    if (visit->items == visit->inline_items) {
        new_items = (HSD_JObj**) malloc(new_capacity * sizeof(*new_items));
        if (new_items != NULL) {
            memcpy(new_items, visit->inline_items,
                   visit->count * sizeof(*new_items));
        }
    } else {
        new_items = (HSD_JObj**) realloc(visit->items,
                                         new_capacity * sizeof(*new_items));
    }
    if (new_items == NULL) {
        visit->overflowed = TRUE;
        return FALSE;
    }
    visit->items = new_items;
    visit->capacity = new_capacity;
    return TRUE;
}

static BOOL PCPort_JObjVisitMark(PCPort_JObjVisit* visit, HSD_JObj* jobj)
{
    size_t i;

    if (jobj == NULL) {
        return FALSE;
    }
    for (i = 0; i < visit->count; i++) {
        if (visit->items[i] == jobj) {
            return FALSE;
        }
    }
    if (visit->count >= visit->capacity && !PCPort_JObjVisitGrow(visit)) {
        return FALSE;
    }
    visit->items[visit->count++] = jobj;
    return TRUE;
}

static void PCPort_JObjSetFlagsAllRec(HSD_JObj* jobj, u32 flags,
                                      PCPort_JObjVisit* visit)
{
    while (jobj != NULL) {
        HSD_JObj* next;

        if (!PCPort_JObjVisitMark(visit, jobj)) {
            break;
        }
        next = jobj->next;
        HSD_JObjSetFlags(jobj, flags);
        PCPort_JObjSetFlagsAllRec(jobj->child, flags, visit);
        jobj = next;
    }
}

void HSD_JObjSetFlagsAll(HSD_JObj* jobj, u32 flags)
{
    PCPort_JObjVisit visit;

    PCPort_JObjVisitInit(&visit);
    PCPort_JObjSetFlagsAllRec(jobj, flags, &visit);
    PCPort_JObjVisitDestroy(&visit);
}

static void PCPort_JObjClearFlagsAllRec(HSD_JObj* jobj, u32 flags,
                                        PCPort_JObjVisit* visit)
{
    while (jobj != NULL) {
        HSD_JObj* next;

        if (!PCPort_JObjVisitMark(visit, jobj)) {
            break;
        }
        next = jobj->next;
        HSD_JObjClearFlags(jobj, flags);
        PCPort_JObjClearFlagsAllRec(jobj->child, flags, visit);
        jobj = next;
    }
}

void HSD_JObjClearFlagsAll(HSD_JObj* jobj, u32 flags)
{
    PCPort_JObjVisit visit;

    PCPort_JObjVisitInit(&visit);
    PCPort_JObjClearFlagsAllRec(jobj, flags, &visit);
    PCPort_JObjVisitDestroy(&visit);
}

static BOOL PCPort_JObjSetMtxDirtySubRec(HSD_JObj* jobj,
                                         PCPort_JObjVisit* visit)
{
    HSD_JObj* child;

    if (!PCPort_JObjVisitMark(visit, jobj)) {
        return FALSE;
    }

    jobj->flags |= JOBJ_MTX_DIRTY;
    child = jobj->child;
    while (child != NULL) {
        HSD_JObj* next;

        next = child->next;
        if (!(child->flags & JOBJ_MTX_INDEP_PARENT) &&
            !PCPort_JObjSetMtxDirtySubRec(child, visit))
        {
            break;
        }
        if (next == child) {
            break;
        }
        child = next;
    }
    return TRUE;
}

void HSD_JObjSetMtxDirtySub(HSD_JObj* jobj)
{
    PCPort_JObjVisit visit;

    if (jobj == NULL) {
        return;
    }
    PCPort_JObjVisitInit(&visit);
    PCPort_JObjSetMtxDirtySubRec(jobj, &visit);
    PCPort_JObjVisitDestroy(&visit);
}

static void PCPort_JObjRemoveAllRec(HSD_JObj* jobj,
                                    PCPort_JObjVisit* visit)
{
    while (jobj != NULL) {
        HSD_JObj* next;
        HSD_JObj* child;

        if (!PCPort_JObjVisitMark(visit, jobj)) {
            break;
        }
        next = jobj->next;
        child = jobj->child;
        PCPort_JObjRemoveAllRec(child, visit);
        jobj->child = NULL;
        jobj->next = NULL;
        jobj->parent = NULL;
        HSD_JObjUnref(jobj);
        jobj = next;
    }
}

void HSD_JObjRemoveAll(HSD_JObj* jobj)
{
    PCPort_JObjVisit visit;

    PCPort_JObjVisitInit(&visit);
    PCPort_JObjRemoveAllRec(jobj, &visit);
    PCPort_JObjVisitDestroy(&visit);
}

static void PCPort_JObjRemoveAnimAllRec(HSD_JObj* jobj,
                                        PCPort_JObjVisit* visit)
{
    while (jobj != NULL) {
        HSD_JObj* next;

        if (!PCPort_JObjVisitMark(visit, jobj)) {
            break;
        }
        next = jobj->next;
        HSD_JObjRemoveAnim(jobj);
        PCPort_JObjRemoveAnimAllRec(jobj->child, visit);
        jobj = next;
    }
}

void HSD_JObjRemoveAnimAll(HSD_JObj* jobj)
{
    PCPort_JObjVisit visit;

    PCPort_JObjVisitInit(&visit);
    PCPort_JObjRemoveAnimAllRec(jobj, &visit);
    PCPort_JObjVisitDestroy(&visit);
}

static void PCPort_JObjReqAnimAllRec(HSD_JObj* jobj, f32 frame,
                                     PCPort_JObjVisit* visit)
{
    while (jobj != NULL) {
        HSD_JObj* next;

        if (!PCPort_JObjVisitMark(visit, jobj)) {
            break;
        }
        next = jobj->next;
        HSD_AObjReqAnim(jobj->aobj, frame);
        HSD_RObjReqAnimAll(jobj->robj, frame);
        if (union_type_dobj(jobj)) {
            HSD_DObjReqAnimAll(jobj->u.dobj, frame);
        }
        PCPort_JObjReqAnimAllRec(jobj->child, frame, visit);
        jobj = next;
    }
}

void HSD_JObjReqAnimAll(HSD_JObj* jobj, f32 frame)
{
    PCPort_JObjVisit visit;

    PCPort_JObjVisitInit(&visit);
    PCPort_JObjReqAnimAllRec(jobj, frame, &visit);
    PCPort_JObjVisitDestroy(&visit);
}

static void PCPort_JObjAddAnimAllRec(HSD_JObj* jobj,
                                     HSD_AnimJoint* animjoint,
                                     HSD_MatAnimJoint* matanimjoint,
                                     HSD_ShapeAnimJoint* shapeanimjoint,
                                     PCPort_JObjVisit* visit)
{
    while (jobj != NULL) {
        HSD_JObj* next;
        HSD_AnimJoint* next_animjoint;
        HSD_MatAnimJoint* next_matanimjoint;
        HSD_ShapeAnimJoint* next_shapeanimjoint;

        if (!PCPort_JObjVisitMark(visit, jobj)) {
            break;
        }

        next = jobj->next;
        next_animjoint = animjoint != NULL ? animjoint->next : NULL;
        next_matanimjoint = matanimjoint != NULL ? matanimjoint->next : NULL;
        next_shapeanimjoint = shapeanimjoint != NULL ? shapeanimjoint->next : NULL;

        if (animjoint != NULL) {
            if (jobj->aobj != NULL) {
                HSD_AObjRemove(jobj->aobj);
            }
            jobj->aobj = HSD_AObjLoadDesc(animjoint->aobjdesc);
            HSD_RObjAddAnimAll(jobj->robj, animjoint->robj_anim);
        }

        if (union_type_dobj(jobj)) {
            HSD_DObjAddAnimAll(
                jobj->u.dobj,
                matanimjoint != NULL ? matanimjoint->matanim : NULL,
                shapeanimjoint != NULL ? shapeanimjoint->shapeanimdobj : NULL);
        }

        PCPort_JObjAddAnimAllRec(
            jobj->child,
            animjoint != NULL ? animjoint->child : NULL,
            matanimjoint != NULL ? matanimjoint->child : NULL,
            shapeanimjoint != NULL ? shapeanimjoint->child : NULL,
            visit);

        jobj = next;
        animjoint = next_animjoint;
        matanimjoint = next_matanimjoint;
        shapeanimjoint = next_shapeanimjoint;
    }
}

void HSD_JObjAddAnimAll(HSD_JObj* jobj, HSD_AnimJoint* animjoint,
                        HSD_MatAnimJoint* matanimjoint,
                        HSD_ShapeAnimJoint* shapeanimjoint)
{
    PCPort_JObjVisit visit;

    PCPort_JObjVisitInit(&visit);
    PCPort_JObjAddAnimAllRec(jobj, animjoint, matanimjoint, shapeanimjoint,
                             &visit);
    PCPort_JObjVisitDestroy(&visit);
}

static void PCPort_JObjAnimAllRec(HSD_JObj* jobj, PCPort_JObjVisit* visit)
{
    while (jobj != NULL) {
        HSD_JObj* next;

        if (!PCPort_JObjVisitMark(visit, jobj)) {
            break;
        }
        next = jobj->next;
        HSD_JObjAnim(jobj);
        PCPort_JObjAnimAllRec(jobj->child, visit);
        jobj = next;
    }
}

void HSD_JObjAnimAll(HSD_JObj* jobj)
{
    PCPort_JObjVisit visit;

    PCPort_JObjVisitInit(&visit);
    PCPort_JObjAnimAllRec(jobj, &visit);
    PCPort_JObjVisitDestroy(&visit);
}

/* ------------------------------------------------------------------------- */
/*  HSD_FObjReqAnimAll (host override)                                        */
/*                                                                           */
/*  The adapted src/hsd/hsd_fobj.c HSD_FObjReqAnimAll sets f->flags = 0,      */
/*  which is the interpreter's "idle" state -> the animation never starts.    */
/*  The real GC fn sets the low nibble to 2 (load-next-packet). This BOOT-set */
/*  override does the correct thing so HSD_JObjReqAnimAll alone is enough to  */
/*  arm playback (it routes through HSD_AObjReqAnim -> HSD_FObjReqAnimAll).    */
/* ------------------------------------------------------------------------- */
#ifndef PCPORT_USE_SRC_FOBJ_REQ_ANIM_ALL
void HSD_FObjReqAnimAll(HSD_FObj* fobj, f32 startframe)
{
    HSD_FObj* f;
    for (f = fobj; f != NULL; f = f->next) {
        f->ad = f->ad_head;
        f->time = startframe;
        f->flags = (u8) ((f->flags & 0xF0) | 2); /* load-next-packet start state */
        f->nb_pack = 0;
        /* Reset segment state so the interpreter re-reads fterm from the stream
         * on the next call.  Without this, re-arm after loop-rewind leaves fterm
         * at the last-cycle value, causing state-4 to sample the terminal segment
         * control points for the entire next cycle instead of starting from
         * segment 0.  The initial load has fterm=0 (from HSD_FObjAlloc memset),
         * which produces the correct "fall through to state-3 immediately" path;
         * re-arm must replicate that condition. */
        f->fterm = 0;
        f->p0    = 0.0f;
        f->p1    = 0.0f;
        f->d0    = 0.0f;
        f->d1    = 0.0f;
    }
}
#endif

/* ------------------------------------------------------------------------- */
/*  PCPort_HSDStartAnimAll — belt-and-suspenders FObj kickoff                 */
/*                                                                           */
/*  After HSD_JObjAddAnimAll builds the live AObj/FObj tree, walk it and put  */
/*  every FObj chain into the start state. HSD_JObjReqAnimAll already routes  */
/*  through the overridden HSD_FObjReqAnimAll above, but this also covers any */
/*  AObj reached only via the DObj/MObj material path and is safe to call     */
/*  redundantly (idempotent: it just (re)arms the state machine at frame 0).  */
/* ------------------------------------------------------------------------- */
static void PCPort_StartAObj(HSD_AObj* aobj)
{
    if (aobj != NULL && aobj->fobj != NULL) {
        PCPort_FObjStartAnim(aobj->fobj, 0.0f);
    }
}

static void PCPort_HSDStartAnimAllRec(HSD_JObj* root, PCPort_JObjVisit* visit)
{
    HSD_JObj* j;
    for (j = root; j != NULL; j = j->next) {
        if (!PCPort_JObjVisitMark(visit, j)) {
            break;
        }
        PCPort_StartAObj(j->aobj);
        if (union_type_dobj(j)) {
            HSD_DObj* d;
            for (d = j->u.dobj; d != NULL; d = d->next) {
                HSD_MObj* m = d->mobj;
                if (m != NULL) {
                    HSD_TObj* t;
                    PCPort_StartAObj(m->aobj);
                    for (t = m->tobj; t != NULL; t = t->next) {
                        PCPort_StartAObj(t->aobj);
                    }
                }
            }
        }
        if (j->child != NULL) {
            PCPort_HSDStartAnimAllRec(j->child, visit);
        }
    }
}

void PCPort_HSDStartAnimAll(HSD_JObj* root)
{
    PCPort_JObjVisit visit;

    PCPort_JObjVisitInit(&visit);
    PCPort_HSDStartAnimAllRec(root, &visit);
    PCPort_JObjVisitDestroy(&visit);
}

#endif /* PCPORT */
