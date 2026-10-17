/**
 * @file hsd_wobj.c
 * @brief HSD WObj - World object implementation.
 *
 * Colosseum address: 0x801914F4 (HSD_WObjInit)
 * Adapted from doldecomp/melee src/sysdolphin/baselib/wobj.c
 *
 * The WObj class name in Colosseum is "had_wobj" (matching Melee),
 * visible in the binary via hsdInitClassInfo.
 */

#include "hsd/hsd_wobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_archive.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_jobj.h"
#include "hsd/hsd_object.h"
#include "hsd/hsd_robj.h"

void WObjInfoInit(void);
static int WObjLoad_Early(HSD_WObj* wobj, HSD_WObjDesc* desc);
int WObjLoad(HSD_WObj* wobj, HSD_WObjDesc* desc);
void WObjAmnesia(HSD_ClassInfo* info);
void WObjRelease(HSD_WObj* wobj);
void WObjUpdateFunc(void* obj, u32 type, void* value);

extern HSD_ClassInfo* lbl_8047B218;

/* 0x801914F4 | 0x98 */
extern char lbl_80274468[]; /* "sysdolphin_base_library" */
extern char lbl_80274480[]; /* "had_wobj" */
void WObjInfoInit(void)
{
    extern u8 lbl_8036C5F0[]; /* hsdWObj class info */

    hsdInitClassInfo(HSD_CLASS_INFO(lbl_8036C5F0),
                     HSD_CLASS_INFO(&hsdObj), lbl_80274468, lbl_80274480,
                     sizeof(HSD_WObjInfo), sizeof(HSD_WObj));
    HSD_CLASS_INFO(lbl_8036C5F0)->release = (void (*)(HSD_Class*)) WObjRelease;
    HSD_CLASS_INFO(lbl_8036C5F0)->amnesia = WObjAmnesia;
    HSD_WOBJ_INFO(lbl_8036C5F0)->load = WObjLoad;
    HSD_WOBJ_INFO(lbl_8036C5F0)->update =
        (void (*)(HSD_WObj*, u32, void*)) WObjUpdateFunc;
}

/* 0x8019158C | 0x48 */
void WObjAmnesia(HSD_ClassInfo* info)
{
    extern u8 lbl_8036C5F0[];

    if (info == lbl_8047B218) {
        lbl_8047B218 = NULL;
    }
    ((HSD_ClassInfo*) *(u32*) (lbl_8036C5F0 + 0x14))->amnesia(info);
}

/* 0x801915D4 | 0x54 */
extern void fn_801AE50C(void* aobj);
extern u8 lbl_8036C5F0[];
void WObjRelease(HSD_WObj* wobj) {
    fn_801AE50C(wobj->robj);
    HSD_AObjRemove(wobj->aobj);
    {
        u32* parent_info = *(u32**)(lbl_8036C5F0 + 0x14);
        ((void (*)(HSD_WObj*))parent_info[0x30 / 4])(wobj);
    }
}

/* 0x80191628 | 0x60 */
extern void* fn_80193828(void*);
extern void __assert(const char*, u32, const char*);
extern const char lbl_8047D8C8[7];
extern const char lbl_8047D8D0[5];
HSD_WObj* HSD_WObjAlloc(void)
{
    extern u8 lbl_8036C5F0[];
    HSD_WObj* wobj;

    if ((wobj = (HSD_WObj*) fn_80193828(
        lbl_8047B218 != NULL
            ? lbl_8047B218
            : (HSD_ClassInfo*) lbl_8036C5F0)) == NULL)
    {
        __assert(lbl_8047D8C8, 0x257, lbl_8047D8D0);
    }
    return wobj;
}

/* 0x80191688 | 0x100 */
extern void HSD_JObjSetupMatrixSub(HSD_JObj* jobj);
/* sdata2/rodata assert strings and float constants owned by other objects */
extern u8 lbl_8027448C[];
extern u8 lbl_80274498[];
extern char lbl_8047D8D8;
extern char lbl_8047D8E0;
extern f64 lbl_8047D8E8;
extern f32 lbl_8047D8F0;
extern f64 lbl_8047D8F8;
extern f32 lbl_8047D900;
extern char lbl_8047D904;

#undef WOBJ_USE_ANIM_POS
#undef WOBJ_POS_DIRTY
#define WOBJ_USE_ANIM_POS 0x1u
#define WOBJ_POS_DIRTY 0x2u

/* Local mirrors of HSD_JObjSetupMatrix's dirty test. These cannot come from
   hsd_jobj.h: the assert baked into the target carries that header's own
   __FILE__ / line, which the externs above name directly. */
static inline BOOL JObjMtxIsDirty(HSD_JObj* jobj)
{
    BOOL result;

    if (jobj == NULL) {
        __assert(&lbl_8047D8D8, 0x25D, &lbl_8047D8E0);
    }
    result = FALSE;
    if (!(jobj->flags & JOBJ_USER_DEF_MTX) && (jobj->flags & JOBJ_MTX_DIRTY)) {
        result = TRUE;
    }
    return result;
}

static inline void WObjJObjSetupMatrix(HSD_JObj* jobj)
{
    if (jobj == NULL || !JObjMtxIsDirty(jobj)) {
        return;
    }
    HSD_JObjSetupMatrixSub(jobj);
}

/* Bake the animated position into pos and drop the "animated" flag, so that
   callers always read a plain world-space vector. jobj_ is the caller's
   scratch HSD_JObj*, shared across every use in a function. */
#define WOBJ_RESOLVE_ANIM_POSITION(wobj_, jobj_)                               \
    do {                                                                       \
        if (((wobj_)->flags & WOBJ_USE_ANIM_POS) != 0) {                       \
            if ((wobj_)->aobj != NULL) {                                       \
                if ((wobj_)->aobj->hsd_obj != NULL) {                          \
                    (jobj_) = (HSD_JObj*) (wobj_)->aobj->hsd_obj;              \
                    if ((jobj_) != NULL) {                                     \
                        if (JObjMtxIsDirty(jobj_)) {                           \
                            HSD_JObjSetupMatrixSub(jobj_);                                \
                        }                                                      \
                    }                                                          \
                    PSMTXMultVec((jobj_)->mtx, &(wobj_)->pos, &(wobj_)->pos);  \
                }                                                              \
            }                                                                  \
            (wobj_)->flags &= 0xFFFFFFFE;                                      \
        }                                                                      \
    } while (0)

/* Mirrors HSD_WObjSetPositionX/Y/Z: evaluate the new component, then (for a
   non-NULL wobj) bake any animated position and store the component. */
#define WOBJ_SET_POSITION_COMPONENT(wobj_, jobj_, field_, val_)                \
    do {                                                                       \
        f32 component_ = (val_);                                               \
        if ((wobj_) != NULL) {                                                 \
            WOBJ_RESOLVE_ANIM_POSITION(wobj_, jobj_);                          \
            (wobj_)->pos.field_ = component_;                                  \
            (wobj_)->flags |= WOBJ_POS_DIRTY;                                  \
        }                                                                      \
    } while (0)

void HSD_WObjGetPosition(HSD_WObj* wobj, Vec* position)
{
    HSD_JObj* jobj;

    if (wobj == NULL || position == NULL) {
        return;
    }
    if ((wobj->flags & WOBJ_USE_ANIM_POS) != 0) {
        if (wobj->aobj != NULL && wobj->aobj->hsd_obj != NULL) {
            WObjJObjSetupMatrix(jobj = (HSD_JObj*) wobj->aobj->hsd_obj);
            PSMTXMultVec(jobj->mtx, &wobj->pos, &wobj->pos);
        }
        wobj->flags &= 0xFFFFFFFE;
    }
    *position = wobj->pos;
}

/* 0x80191788 | 0x48 */
void HSD_WObjSetPosition(HSD_WObj* wobj, Vec* position)
{
    if (wobj == NULL || position == NULL) {
        return;
    }
    wobj->pos = *position;
    wobj->flags = wobj->flags | WOBJ_POS_DIRTY;
    wobj->flags = wobj->flags & 0xFFFFFFFE;
}

/* 0x801917D0 | 0xCC */
extern HSD_ClassInfo* fn_80193748(char* class_name);
HSD_WObj* HSD_WObjLoadDesc(HSD_WObjDesc* desc)
{
    extern u8 lbl_8036C5F0[];

    if (desc != NULL) {
        HSD_WObj* wobj;
        HSD_ClassInfo* info;

        if (desc->class_name == NULL ||
            !(info = fn_80193748(desc->class_name)))
        {
            /* HSD_WObjAlloc() inlined */
            wobj = (HSD_WObj*) fn_80193828(
                lbl_8047B218 ? lbl_8047B218 : (HSD_ClassInfo*) lbl_8036C5F0);
            if (wobj == NULL) {
                __assert(lbl_8047D8C8, 0x257, lbl_8047D8D0);
            }
        } else {
            wobj = (HSD_WObj*) fn_80193828(info);
            if (wobj == NULL) {
                __assert(lbl_8047D8C8, 0x104, lbl_8047D8D0);
            }
        }
        HSD_WOBJ_METHOD(wobj)->load(wobj, desc);
        return wobj;
    }
    return NULL;
}

/* 0x8019189C | 0xB0 */
extern void fn_801AEBE4(void* robj, void* desc);
void HSD_WObjInit(HSD_WObj* wobj, HSD_WObjDesc* desc)
{
    if (wobj == NULL || desc == NULL) {
        return;
    }

    HSD_WObjSetPosition(wobj, &desc->pos);
    if (wobj->robj != NULL) {
        fn_801AE50C(wobj->robj);
    }
    wobj->robj = (HSD_RObj*) HSD_RObjLoadDesc(desc->robjdesc);
    fn_801AEBE4(wobj->robj, desc->robjdesc);
}

/* 0x8019194C | 0xA0 */
int WObjLoad(HSD_WObj* wobj, HSD_WObjDesc* desc)
{
    HSD_WObjSetPosition(wobj, &desc->pos);
    if (wobj->robj != NULL) {
        fn_801AE50C(wobj->robj);
    }
    wobj->robj = (HSD_RObj*) HSD_RObjLoadDesc(desc->robjdesc);
    fn_801AEBE4(wobj->robj, desc->robjdesc);
    return 0;
}

/* 0x801919EC | 0x48 */
extern void fn_801B0040(void* robj);
void HSD_WObjInterpretAnim(HSD_WObj* wobj) {
    if (wobj != NULL) {
        HSD_AObjInterpretAnim(wobj->aobj, wobj,
                              (HSD_ObjUpdateFunc) HSD_WOBJ_METHOD(wobj)->update);
        fn_801B0040(wobj->robj);
    }
}

/* 0x80191A34 | 0x398 */
extern void splArcLengthPoint(Vec* out, HSD_Spline* spline, f32 frame);
void WObjUpdateFunc(void* obj, u32 type, void* value)
{
    HSD_WObj* wobj;
    Vec position;
    HSD_JObj* jobj;
    f32* fval;

    wobj = (HSD_WObj*) obj;
    fval = (f32*) value;

    if (wobj == NULL) {
        return;
    }

    switch (type) {
    case HSD_A_J_PATH:
        if (*fval < lbl_8047D8E8) {
            *fval = lbl_8047D8F0;
        }
        if (lbl_8047D8F8 < *fval) {
            *fval = lbl_8047D900;
        }
        if (wobj->aobj == NULL) {
            __assert(lbl_8047D8C8, 0x98, (const char*) lbl_8027448C);
        }
        jobj = (HSD_JObj*) wobj->aobj->hsd_obj;
        if (jobj == NULL) {
            __assert(lbl_8047D8C8, 0x9A, &lbl_8047D904);
        }
        if (jobj->u.spline == NULL) {
            __assert(lbl_8047D8C8, 0x9B, (const char*) lbl_80274498);
        }
        splArcLengthPoint(&position, jobj->u.spline, *fval);
        HSD_WObjSetPosition(wobj, &position);
        wobj->flags |= WOBJ_USE_ANIM_POS;
        break;
    case HSD_A_J_TRAX:
        WOBJ_SET_POSITION_COMPONENT(wobj, jobj, x, *fval);
        break;
    case HSD_A_J_TRAY:
        WOBJ_SET_POSITION_COMPONENT(wobj, jobj, y, *fval);
        break;
    case HSD_A_J_TRAZ:
        WOBJ_SET_POSITION_COMPONENT(wobj, jobj, z, *fval);
        break;
    }
}

/* 0x80191DCC | 0x6C */
typedef struct { void* aobj_desc; void* robj_desc; } WObjADesc;
void HSD_WObjAddAnim(HSD_WObj* wobj, HSD_WObjAnim* desc) {
    if (wobj == NULL) {
        return;
    }
    if (desc == NULL) {
        return;
    }
    if (wobj->aobj != NULL) {
        HSD_AObjRemove(wobj->aobj);
    }
    wobj->aobj = HSD_AObjLoadDesc(desc->aobjdesc);
    HSD_RObjAddAnimAll(wobj->robj, desc->robjanim);
}

/* 0x80191E38 | 0x50 */
extern void fn_801AFEFC(void* robj, f32 frame);
void HSD_WObjReqAnim(HSD_WObj* wobj, f32 frame) {
    if (wobj != NULL) {
        HSD_AObjReqAnim(wobj->aobj, frame);
        fn_801AFEFC(wobj->robj, frame);
    }
}

/* 0x80191E88 | 0x44 */
extern void fn_801AFFE0(void* robj);
void HSD_WObjRemoveAnim(HSD_WObj* wobj) {
    if (wobj != NULL) {
        HSD_AObjRemove(wobj->aobj);
        wobj->aobj = NULL;
        fn_801AFFE0(wobj->robj);
    }
}

/* HSD_ArchiveGetPublicAddress (0x80191ECC) | 0x98 */
extern int strcmp(const char* s1, const char* s2);
void* HSD_ArchiveGetPublicAddress(HSD_Archive* archive, const char* symbols)
{
    u32 i;

    for (i = 0; i < archive->header.nb_public; i++) {
        int comparison =
            strcmp(archive->symbols + archive->public_info[i].symbol, symbols);

        if (comparison == 0) {
            return archive->data + archive->public_info[i].offset;
        }
    }

    return NULL;
}

/* 0x80191F64 | 0x180 */
extern void OSReport(const char* fmt, ...);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void* memset(void* dst, int val, u32 size);
extern const char lbl_802744A8[];

static inline void Locate(HSD_Archive* archive)
{
    u32 i;
    u32* ptr;

    for (i = 0; i < archive->header.nb_reloc; i++) {
        ptr = (u32*) (archive->data + archive->reloc_info[i].offset);
        *ptr += (u32) archive->data;
    }
}

s32 HSD_ArchiveParse(HSD_Archive* archive, u8* src, u32 file_size)
{
    u32 offset = 0;

    if (archive == NULL) {
        return -1;
    }

    memset(archive, 0, sizeof(HSD_Archive));
    archive->flags = archive->flags | HSD_ARCHIVE_DONT_FREE;
    memcpy(archive, src, sizeof(HSD_ArchiveHeader));

    if (archive->header.file_size != file_size) {
        OSReport(lbl_802744A8);
        return -1;
    }

    offset += sizeof(HSD_ArchiveHeader);
    if (archive->header.data_size != 0) {
        archive->data = src + offset;
        offset += archive->header.data_size;
    }
    if (archive->header.nb_reloc != 0) {
        archive->reloc_info = (HSD_ArchiveRelocationInfo*) (src + offset);
        offset += archive->header.nb_reloc * sizeof(HSD_ArchiveRelocationInfo);
    }
    if (archive->header.nb_public != 0) {
        archive->public_info = (HSD_ArchivePublicInfo*) (src + offset);
        offset += archive->header.nb_public * sizeof(HSD_ArchivePublicInfo);
    }
    if (archive->header.nb_extern != 0) {
        archive->extern_info = (HSD_ArchiveExternInfo*) (src + offset);
        offset += archive->header.nb_extern * sizeof(HSD_ArchiveExternInfo);
    }
    if (offset < archive->header.file_size) {
        archive->symbols = (char*) (src + offset);
    }

    archive->top_ptr = src;
    Locate(archive);

    return 0;
}
