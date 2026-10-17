/**
 * @file fobj.c
 * @brief sysdolphin fobj.c - keyframe animation curves (FObj).
 *
 * Retail extent: the whole TU
 *   .text   0x8019970C-0x8019B7C0  HSD_FObjFree .. HSD_FObjGetAllocData
 *   .rodata 0x80274758-0x802747B8  "fobj: alloc", "st == FOBJ_LOAD_WAIT",
 *                                  "st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA",
 *                                  "fobj: free"
 *   .sdata2 0x8047DA30-0x8047DA60  "fobj.c", "new", 0.0f, 0.0, 1.0 and the
 *                                  u32/s32 -> double conversion biases
 * fobj_alloc_data (.bss 0x80465378) is still owned by the carved .bss unit
 * and stays extern.
 *
 * Built with the sysdolphin library flags (GC/1.3.2 -O4,p -O1
 * -inline auto,deferred -use_lmw_stmw on -str reuse,readonly). Deferred
 * inlining emits functions in reverse definition order, so they are listed
 * in HAL/Melee source order (HSD_FObjGetAllocData first). Adapted from
 * doldecomp/melee src/sysdolphin/baselib/fobj.c; Colosseum's newer
 * sysdolphin differs in a few places:
 * - HSD_FObjAlloc/HSD_FObjFree report to an allocation hook
 *   (fn_80199A84, an empty function with the pool and "fobj: alloc" /
 *   "fobj: free"); it is inlined away except where the inline depth runs
 *   out, which is why a copy is emitted after HSD_FObjLoadDesc.
 * - FObjLoadData is an ordinary function (4.6 KB with its parse helpers
 *   inlined), called from HSD_FObjInterpretAnim.
 * - HSD_FObjGetState is written with an if/else (retail tests fobj != NULL
 *   and falls through to the zero result).
 *
 * The recursive HSD_FObjLoadDesc and HSD_FObjRemoveAll are expanded into
 * themselves by the inliner (4 and 8 levels); the depth MWCC picks for
 * HSD_FObjLoadDesc depends on the size of its body, and the plain
 * `if (desc)` test is what reproduces retail's four levels.
 */

#include "hsd/hsd_fobj.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_objalloc.h"

extern void* memset(void* dst, int c, u32 n);
extern void HSD_ObjFree(HSD_ObjAllocData* data, void* obj);
extern void HSD_ObjAllocInit(HSD_ObjAllocData* data, u32 size, u32 align);
/* splGetHelmite */
extern f32 splGetHelmite(f32 fterm_inv, f32 time, f32 p0, f32 p1, f32 d0,
                       f32 d1);

extern HSD_ObjAllocData lbl_80465378; /* fobj_alloc_data */
#define fobj_alloc_data lbl_80465378

HSD_ObjAllocData* HSD_FObjGetAllocData(void)
{
    return &fobj_alloc_data;
}

void HSD_FObjInitAllocData(void)
{
    HSD_ObjAllocInit(HSD_FObjGetAllocData(), sizeof(HSD_FObj), 4);
}

void HSD_FObjRemove(HSD_FObj* fobj)
{
    if (!fobj) {
        return;
    }

    HSD_FObjFree(fobj);
}

void HSD_FObjRemoveAll(HSD_FObj* fobj)
{
    if (fobj == NULL) {
        return;
    }
    HSD_FObjRemoveAll(fobj->next);
    HSD_FObjRemove(fobj);
}

static inline u32 HSD_FObjSetState(HSD_FObj* fobj, u32 state)
{
    if (fobj) {
        fobj->flags = (state & 0xF) | (fobj->flags & 0xF0);
    }
    return state;
}

static inline u32 HSD_FObjGetState(HSD_FObj* fobj)
{
    if (fobj == NULL) {
        return 0;
    } else {
        return fobj->flags & 0xF;
    }
}

static inline void HSD_FObjReqAnim(HSD_FObj* fobj, f32 startframe)
{
    if (fobj == NULL) {
        return;
    }

    fobj->ad = fobj->ad_head;
    fobj->time = (f32) fobj->startframe + startframe;
    fobj->op = 0;
    fobj->op_intrp = 0;
    fobj->flags &= ~0x40;
    fobj->nb_pack = 0;
    fobj->fterm = 0;
    fobj->p0 = 0.f;
    fobj->p1 = 0.f;
    fobj->d0 = 0.f;
    fobj->d1 = 0.f;
    HSD_FObjSetState(fobj, 1);
}

void HSD_FObjReqAnimAll(HSD_FObj* fobj, f32 startframe)
{
    HSD_FObj* fp;

    if (fobj == NULL) {
        return;
    }

    for (fp = fobj; fp != NULL; fp = fp->next) {
        HSD_FObjReqAnim(fp, startframe);
    }
}

static inline void FObj_FlushKeyData(HSD_FObj* fobj, void* obj,
                                     HSD_ObjUpdateFunc obj_update, f32 rate)
{
    if (fobj->op_intrp == HSD_A_OP_KEY) {
        HSD_FObjInterpretAnim(fobj, obj, obj_update, rate);
    }
}

static inline void HSD_FObjStopAnim(HSD_FObj* fobj, void* obj,
                                    HSD_ObjUpdateFunc obj_update, f32 rate)
{
    if (fobj == NULL) {
        return;
    }

    FObj_FlushKeyData(fobj, obj, obj_update, rate);
    HSD_FObjSetState(fobj, 0);
}

void HSD_FObjStopAnimAll(HSD_FObj* fobj, void* obj,
                         HSD_ObjUpdateFunc obj_update, f32 rate)
{
    for (; fobj != NULL; fobj = fobj->next) {
        HSD_FObjStopAnim(fobj, obj, obj_update, rate);
    }
}

static inline f32 parseFloat(u8** pos, u8 frac)
{
    union {
        f32 f;
        u32 d;
    } u;
    f32 numer;
    s32 denom;

    if (frac == HSD_A_FRAC_FLOAT) {
        u.d = (s32) ((*pos)++)[0];
        u.d |= ((*pos)++)[0] << 8;
        u.d |= ((*pos)++)[0] << 16;
        u.d |= ((*pos)++)[0] << 24;
        return u.f;
    }

    denom = (1 << (frac & 0x1F));
    switch (frac & 0xE0) {
    case HSD_A_FRAC_S8:
        numer = (s8) (*pos)[0];
        *pos += 1;
        break;
    case HSD_A_FRAC_U8:
        numer = (*pos)[0];
        *pos += 1;
        break;
    case HSD_A_FRAC_S16:
        numer = ((s8) (*pos)[1] << 8) | (*pos)[0];
        *pos += 2;
        break;
    case HSD_A_FRAC_U16:
        numer = ((*pos)[1] << 8) | (*pos)[0];
        *pos += 2;
        break;
    default:
        return 0.0f;
    }
    return numer / denom;
}

static inline u8 parseOpCode(u8** curr_parse)
{
    return (**curr_parse) & 0xF;
}

static inline u32 parsePackInfo(u8** adp)
{
    u8 d;
    u32 nb_pack;
    s32 shift;

    d = *(*adp)++;
    nb_pack = ((d >> 4) & 7) + 1;
    shift = 3;
    if (!(d & 0x80)) {
        return nb_pack;
    }
    do {
        d = *(*adp)++;
        nb_pack += (d & 0x7F) << shift;
        shift += 7;
    } while (d & 0x80);
    return nb_pack;
}

static inline void FObjLaunchKeyData(HSD_FObj* fobj)
{
    if ((fobj->flags & 0x40) != 0) {
        fobj->op_intrp = fobj->op;
        fobj->flags &= ~0x40;
        fobj->flags |= 0x80;
        fobj->p0 = fobj->p1;
    }
}

static inline s32 parseWait(u8** adp)
{
    u8 d;
    s32 wait = 0;
    s32 shift = 0;

    do {
        d = *(*adp)++;
        wait |= (d & 0x7f) << shift;
        shift += 7;
    } while (d & 0x80);

    return wait;
}

static inline u32 FObjLoadWait(HSD_FObj* fobj)
{
    u32 st = HSD_FObjGetState(fobj);
    HSD_ASSERT(0x16C, st == FOBJ_LOAD_WAIT);

    if ((unsigned) (fobj->ad - fobj->ad_head) >= fobj->length) {
        return 6;
    } else {
        fobj->fterm = parseWait(&fobj->ad);
        fobj->flags |= 0x20;
        return HSD_FObjSetState(fobj, 2);
    }
}

static inline u32 FObjAnimCON(HSD_FObj* fobj)
{
    u32 st = HSD_FObjGetState(fobj);
    HSD_ASSERT(0x17F, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);

    fobj->p0 = fobj->p1;
    fobj->p1 = parseFloat(&fobj->ad, fobj->frac_value);
    if (fobj->op_intrp != 5) {
        fobj->d0 = fobj->d1;
        fobj->d1 = 0.0F;
    }

    return HSD_FObjSetState(fobj, st == FOBJ_LOAD_DATA0 ? 3 : 4);
}

static inline u32 FObjAnimLinear(HSD_FObj* fobj)
{
    u32 st = HSD_FObjGetState(fobj);
    HSD_ASSERT(0x193, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);

    fobj->p0 = fobj->p1;
    fobj->p1 = parseFloat(&fobj->ad, fobj->frac_value);
    if (fobj->op_intrp != 5) {
        fobj->d0 = fobj->d1;
        fobj->d1 = 0.0F;
    }

    return HSD_FObjSetState(fobj, st == FOBJ_LOAD_DATA0 ? 3 : 4);
}

static inline u32 FObjAnimSPL0(HSD_FObj* fobj)
{
    u32 st = HSD_FObjGetState(fobj);
    HSD_ASSERT(0x1A7, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);

    fobj->p0 = fobj->p1;
    fobj->d0 = fobj->d1;
    fobj->p1 = parseFloat(&fobj->ad, fobj->frac_value);
    fobj->d1 = 0.0F;

    return HSD_FObjSetState(fobj, st == FOBJ_LOAD_DATA0 ? 3 : 4);
}

static inline u32 FObjAnimSPL(HSD_FObj* fobj)
{
    u32 st = HSD_FObjGetState(fobj);
    HSD_ASSERT(0x1B9, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);

    fobj->p0 = fobj->p1;
    fobj->p1 = parseFloat(&fobj->ad, fobj->frac_value);
    fobj->d0 = fobj->d1;
    fobj->d1 = parseFloat(&fobj->ad, fobj->frac_slope);

    return HSD_FObjSetState(fobj, st == FOBJ_LOAD_DATA0 ? 3 : 4);
}

static inline u32 FObjAnimSLP(HSD_FObj* fobj)
{
    u32 st = HSD_FObjGetState(fobj);
    HSD_ASSERT(0x1CC, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);

    fobj->d0 = fobj->d1;
    fobj->d1 = parseFloat(&fobj->ad, fobj->frac_slope);

    return HSD_FObjGetState(fobj);
}

static inline u32 FObjAnimKey(HSD_FObj* fobj)
{
    u32 st = HSD_FObjGetState(fobj);
    HSD_ASSERT(0x1E9, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);

    FObjLaunchKeyData(fobj);
    fobj->p1 = parseFloat(&fobj->ad, fobj->frac_value);
    fobj->flags |= 0x40;

    return HSD_FObjSetState(fobj, st == FOBJ_LOAD_DATA0 ? 3 : 4);
}

u32 FObjLoadData(HSD_FObj* fobj)
{
    if ((unsigned) (fobj->ad - fobj->ad_head) >= fobj->length) {
        return 6;
    } else {
        fobj->op_intrp = fobj->op;
        if (fobj->nb_pack == 0) {
            fobj->op = parseOpCode(&fobj->ad);
            fobj->nb_pack = parsePackInfo(&fobj->ad);
        }

        fobj->nb_pack -= 1;

        switch (fobj->op) {
        case HSD_A_OP_CON:
            return FObjAnimCON(fobj);

        case HSD_A_OP_LIN:
            return FObjAnimLinear(fobj);

        case HSD_A_OP_SPL0:
            return FObjAnimSPL0(fobj);

        case HSD_A_OP_SPL:
            return FObjAnimSPL(fobj);

        case HSD_A_OP_SLP:
            return FObjAnimSLP(fobj);

        case HSD_A_OP_KEY:
            return FObjAnimKey(fobj);

        default:
            return 0;
        }
    }
}

static inline void FObjUpdateAnim(HSD_FObj* fobj, void* obj,
                                  HSD_ObjUpdateFunc obj_update)
{
    f32 value;
    HSD_ObjData fobjdata;

    if (obj_update == NULL) {
        return;
    }
    switch (fobj->op_intrp) {
    case HSD_A_OP_KEY:
        if (fobj->flags & 0x80) {
            fobjdata.fv = fobj->p0;
            fobj->flags &= 0xFFFFFF7F;
        } else {
            return;
        }
        break;
    case HSD_A_OP_CON:
        if (fobj->time >= fobj->fterm) {
            value = fobj->p1;
        } else {
            value = fobj->p0;
        }
        fobjdata.fv = value;
        break;
    case HSD_A_OP_LIN:
        if (fobj->flags & 0x20) {
            fobj->flags = fobj->flags & 0xFFFFFFDF;
            if (fobj->fterm != 0) {
                fobj->d0 = (fobj->p1 - fobj->p0) / fobj->fterm;
            } else {
                fobj->d0 = 0;
                fobj->p0 = fobj->p1;
            }
        }
        fobjdata.fv = fobj->d0 * fobj->time + fobj->p0;
        break;
    case HSD_A_OP_SPL0:
    case HSD_A_OP_SPL:
    case HSD_A_OP_SLP:
        if (fobj->fterm != 0) {
            fobjdata.fv = splGetHelmite(1.0 / fobj->fterm, fobj->time, fobj->p0,
                                      fobj->p1, fobj->d0, fobj->d1);
        } else {
            fobjdata.fv = fobj->p1;
        }
        break;
    default:
        break;
    }
    obj_update(obj, fobj->obj_type, &fobjdata);
}

void HSD_FObjInterpretAnim(HSD_FObj* fobj, void* obj,
                           HSD_ObjUpdateFunc obj_update, f32 rate)
{
    f32 fterm;
    u32 state;

    fterm = 0.0F;
    state = fobj != NULL ? HSD_FObjGetState(fobj) : 0;
    if (state != 0 && !(fobj->time += rate, (fobj->time < 0.0))) {
        for (;;) {
            switch (state) {
            case 6: {
                fobj->time += fterm;
                FObjLaunchKeyData(fobj);
                FObjUpdateAnim(fobj, obj, obj_update);
                return;
            }
            case 1:
            case 2: {
                state = FObjLoadData(fobj);
                break;
            }
            case 3: {
                if ((fobj->flags & 0x80) != 0) {
                    FObjUpdateAnim(fobj, obj, obj_update);
                }
                state = FObjLoadWait(fobj);
                break;
            }
            case 4: {
                if (fobj->fterm <= fobj->time) {
                    state = 3;
                    fterm = fobj->fterm;
                    fobj->time -= fobj->fterm;
                    HSD_FObjSetState(fobj, state);
                    break;
                }
                FObjUpdateAnim(fobj, obj, obj_update);
                state = 5;
                HSD_FObjSetState(fobj, state);
                return;
            }
            case 5: {
                state = 4;
                HSD_FObjSetState(fobj, state);
                break;
            }
            case 0:
                return;
            }
        }
    }
}

void HSD_FObjInterpretAnimAll(void* fobj, void* obj,
                              HSD_ObjUpdateFunc obj_update, f32 rate)
{
    HSD_FObj* fp = (HSD_FObj*) fobj;
    while (fp != NULL) {
        HSD_FObjInterpretAnim(fp, obj, obj_update, rate);
        fp = fp->next;
    }
}

/* Allocation hook: empty in the retail build. */
void fn_80199A84(HSD_ObjAllocData* data, const char* msg)
{
}

HSD_FObj* HSD_FObjLoadDesc(HSD_FObjDesc* desc)
{
    if (desc) {
        HSD_FObj* fobj = HSD_FObjAlloc();
        fobj->next = HSD_FObjLoadDesc(desc->next);
        fobj->startframe = desc->startframe;
        fobj->obj_type = desc->type;
        fobj->frac_value = desc->frac_value;
        fobj->frac_slope = desc->frac_slope;
        fobj->ad_head = desc->ad;
        fobj->length = desc->length;
        fobj->flags = 0;
        return fobj;
    }
    return NULL;
}

HSD_FObj* HSD_FObjAlloc(void)
{
    HSD_FObj* new = HSD_ObjAlloc(&fobj_alloc_data);
    fn_80199A84(&fobj_alloc_data, "fobj: alloc");
    HSD_ASSERT(0x2F3, new);
    memset(new, 0, sizeof(HSD_FObj));
    return new;
}

void HSD_FObjFree(HSD_FObj* fobj)
{
    HSD_ObjFree(&fobj_alloc_data, fobj);
    fn_80199A84(&fobj_alloc_data, "fobj: free");
}
