/**
 * @file gs_material_800DF498.c
 * @brief GSmaterial: PE descriptor/flag setters, pool create/free/init and
 *        the GS material MObj subclass (TEV expression and env-map setup).
 *
 * Address range: 0x800DF498 - 0x800DFEEC (retail GSmaterial.cpp tail).
 *
 * Shared body of two units:
 *   gs_material_exact_800DF498.c      0x800DF498 - 0x800DFABC (Matching;
 *                                      defines GS_MATERIAL_EXACT_HEAD_ONLY)
 *   gs_material_candidate_800DFABC.c  0x800DFABC - 0x800DFE98 (Matching;
 *                                      defines GS_MATERIAL_ENVMAP_TAIL_ONLY)
 * _matGSmatObjLoad (0x800DFE98 - 0x800DFEEC) is gs_material_exact_800DFE98.c.
 * _matGSmatEnableEnvMapExt is exact (lane D18): the extension-disable
 * inline's bit mask is a u16 like GSmaterial.extensions (a u32 mask gives
 * it two more interference neighbours, so it is coloured before the TObj
 * and takes r31 instead of r29), and the texture local is declared before
 * the MObj local.
 */

#include "dolphin/types.h"
#include "game/gs_texture.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_tobj.h"

/* Common head shared by every HSD TEV expression node (HAL texp.h). */
union HSD_TExp {
    u32 type;
    struct {
        u32 type;
        HSD_TExp* next;
    } comm;
};

/* Marks the saved PE descriptor / image descriptor slots as unused. */
#define GSMATERIAL_UNSAVED ((void*)0xFEFEFEFE)

typedef struct GSmaterial {
    /* 0x00 */ u8 inUse;
    /* 0x01 */ u8 alpha;
    /* 0x02 */ u16 extensions;
    /* 0x04 */ u32 savedFlags;
    /* 0x08 */ HSD_MObj* mobj;
    /* 0x0C */ u8 modulate[4];
    /* 0x10 */ u32 channels[4];
    /* 0x20 */ HSD_TObj* envTObj;
    /* 0x24 */ HSD_ImageDesc* envImageDesc;
    /* 0x28 */ GStextureHandle* envTexture;
    /* 0x2C */ s32 envMode;
    /* 0x30 */ s32 envType;
    /* 0x34 */ f32 envBlend;
    /* 0x38 */ HSD_ImageDesc* savedImageDesc;
    /* 0x3C */ HSD_PEDesc* savedPEDesc;
} GSmaterial; /* size 0x40 */

/* GS material MObj: an HSD_MObj with a back pointer to its GSmaterial. */
typedef struct GSmatObj {
    /* 0x00 */ HSD_MObj mobj;
    /* 0x20 */ GSmaterial* material;
} GSmatObj; /* size 0x24 */

extern void GSlogWrite(const char* format, ...);
extern u16 _toolentryAlloc__FUl(u32 size);
extern void* GSmemLock(u16 handle);
extern void GXDrawDone(void);
extern void fn_801A6DA0(HSD_MObj* mobj, HSD_TObj* tobj); /* HSD_MObjAddTObj */
extern s32 HSD_TExpGetType(HSD_TExp* texp);
extern void fn_801B6DC0(HSD_TExp* texp, u32 r, u32 g, u32 b, u32 a); /* TEV channel swap */
extern HSD_TExp* HSD_TExpCnst(void* value, u32 comp, u32 type, HSD_TExp** list);
extern HSD_TExp* HSD_TExpTev(HSD_TExp** list); /* HSD_TExpTev */
extern void HSD_TExpColorOp(HSD_TExp* texp, u32 op, u32 bias, u32 scale, u32 clamp);
extern void HSD_TExpColorIn(HSD_TExp* texp, u32 selA, HSD_TExp* expA, u32 selB, HSD_TExp* expB,
                        u32 selC, HSD_TExp* expC, u32 selD, HSD_TExp* expD);
extern void HSD_TExpAlphaOp(HSD_TExp* texp, u32 op, u32 bias, u32 scale, u32 clamp);
extern void HSD_TExpAlphaIn(HSD_TExp* texp, u32 selA, HSD_TExp* expA, u32 selB, HSD_TExp* expB,
                        u32 selC, HSD_TExp* expC, u32 selD, HSD_TExp* expD);
extern u8 GStextureGetMiplevels(GStextureHandle* tex);
extern u16 GStextureGetXsize(GStextureHandle* tex);
extern u16 GStextureGetYsize(GStextureHandle* tex);
extern s32 GStextureGetGXformat(GStextureHandle* tex, u8 alpha);

extern const char lbl_80270528[]; /* SetPEdescr: descriptor already replaced */
extern const char lbl_8027056C[]; /* "GSmaterialCreate: Run out of materials..." */
extern char lbl_802705C0[];       /* "GSmaterial MObj" */
extern const char lbl_802705D0[]; /* "GSmaterial: Unsupported texture format..." */
extern const char lbl_80270610[]; /* "GSmaterial: Error creating environment map..." */
extern HSD_MObjInfo lbl_80315490; /* GS material MObj class */
extern HSD_TObjDesc lbl_803154E4; /* environment-map TObj descriptor */
extern HSD_MObjInfo lbl_8036CB30; /* hsdMObj */
extern const f32 lbl_8047CAC8;

extern u16 lbl_8047AB18;        /* material pool GSmem handle */
extern GSmaterial* lbl_8047AB1C; /* material pool */
extern u32 lbl_8047AB20;        /* material pool size */

HSD_TExp* _matGSmatObjMakeTExp(HSD_MObj* mobj, HSD_TObj* tobj, HSD_TExp** list);
void _matGSmatEnableEnvMapExt(GSmaterial* material);
int _matGSmatObjLoad(HSD_MObj* mobj, HSD_MObjDesc* desc);

/*
 * The extension-disable body GSmaterialDisableExtension (0x800DF248) wraps.
 * The target expands it three times in this range: in GSmaterialRemoveExtraTObj with every
 * extension bit and twice in _matGSmatEnableEnvMapExt with the env-map bit.
 * The TObj unlink re-reads material->mobj rather than reusing the outer
 * local; the target reloads it there too.
 */
static inline void materialDisableExtension(GSmaterial* material, u32 flags)
{
    HSD_MObj* mobj = material->mobj;
    u16 bits = flags & material->extensions;

    if (bits & 1) {
        material->modulate[3] = 0x7F;
        material->modulate[2] = 0x7F;
        material->modulate[1] = 0x7F;
        material->modulate[0] = 0x7F;
    }
    if (bits & 2) {
        material->channels[0] = 0;
        material->channels[1] = 1;
        material->channels[2] = 2;
        material->channels[3] = 3;
    }
    if (bits & 4) {
        HSD_MObj* owner = material->mobj;
        HSD_TObj* envTObj = material->envTObj;
        HSD_ImageDesc* imageDesc = material->envImageDesc;
        HSD_TObj* tobj = owner->tobj;

        if (tobj != NULL && envTObj != NULL) {
            if (tobj == envTObj) {
                owner->tobj = envTObj->next;
            } else {
                while (tobj != NULL) {
                    if (tobj->next == envTObj) {
                        tobj->next = envTObj->next;
                    }
                    tobj = tobj->next;
                }
            }
            if (imageDesc != NULL) {
                HSD_ImageDescFree(imageDesc);
            }
            if (envTObj != NULL) {
                HSD_TObjRemove(envTObj);
            }
        }
        material->envTexture = NULL;
    }

    material->extensions &= ~bits;
    if (material->extensions == 0) {
        ((GSmatObj*)mobj)->material = NULL;
    }
    HSD_MObjCompileTev(mobj);
}

#if !defined(GS_MATERIAL_ENVMAP_TAIL_ONLY)
void GSmaterialSetPEdescr(GSmaterial* material, HSD_PEDesc* pe)
{
    if (material->savedPEDesc == GSMATERIAL_UNSAVED) {
        material->savedPEDesc = material->mobj->pe;
    } else {
        GSlogWrite(lbl_80270528);
    }
    material->mobj->pe = pe;
}

void GSmaterialResetFlags(GSmaterial* material)
{
    HSD_MObjClearFlags(material->mobj, 0x4000600F);
    HSD_MObjSetFlags(material->mobj, material->savedFlags);
    HSD_MObjCompileTev(material->mobj);
}

void GSmaterialSetFlags(GSmaterial* material, u32 flags)
{
    u32 renderMode;

    material->savedFlags = HSD_MObjGetFlags(material->mobj);
    HSD_MObjClearFlags(material->mobj, 0x4000600F);
    renderMode = 0;
    if (flags & 0x01) {
        renderMode |= 0x1;
    }
    if (flags & 0x02) {
        renderMode |= 0x2;
    }
    if (flags & 0x04) {
        renderMode |= 0x4;
    }
    if (flags & 0x08) {
        renderMode |= 0x8;
    }
    if (flags & 0x10) {
        renderMode |= 0x40000000;
    }
    if (flags & 0x20) {
        renderMode |= 0x2000;
    }
    if (flags & 0x40) {
        renderMode |= 0x4000;
    }
    HSD_MObjSetFlags(material->mobj, renderMode);
    HSD_MObjCompileTev(material->mobj);
}

void GSmaterialRemoveExtraTObj(GSmaterial* material)
{
    HSD_PEDesc* pe = material->savedPEDesc;

    if (pe != GSMATERIAL_UNSAVED) {
        material->mobj->pe = pe;
        material->savedPEDesc = GSMATERIAL_UNSAVED;
    }

    if (material->savedImageDesc != GSMATERIAL_UNSAVED) {
        HSD_TObj* tobj;
        HSD_ImageDesc* imageDesc;

        GXDrawDone();
        tobj = material->mobj->tobj;
        imageDesc = tobj->imagedesc;
        tobj->imagedesc = material->savedImageDesc;
        HSD_ImageDescFree(imageDesc);
        material->savedImageDesc = GSMATERIAL_UNSAVED;
    }

    materialDisableExtension(material, 0xFFFF);
    material->inUse = 0;
}

static inline GSmaterial* materialFindFree(void)
{
    GSmaterial* material = lbl_8047AB1C;
    u32 i;

    for (i = 0; i < lbl_8047AB20; i++, material++) {
        if (material->inUse == 0) {
            return material;
        }
    }
    return NULL;
}

GSmaterial* GSmaterialCreate(void)
{
    GSmaterial* material = materialFindFree();

    if (material == NULL) {
        GSlogWrite(lbl_8027056C);
        return NULL;
    }
    material->inUse = 1;
    material->savedPEDesc = GSMATERIAL_UNSAVED;
    material->extensions = 0;
    material->channels[0] = 0;
    material->channels[1] = 1;
    material->channels[2] = 2;
    material->channels[3] = 3;
    material->envTexture = NULL;
    material->envTObj = NULL;
    material->envImageDesc = NULL;
    material->savedImageDesc = GSMATERIAL_UNSAVED;
    return material;
}

void GSmaterialInit(u32 count)
{
    u32 i;

    lbl_8047AB20 = count;
    lbl_8047AB18 = _toolentryAlloc__FUl(count * sizeof(GSmaterial));
    if (lbl_8047AB18 != 0) {
        lbl_8047AB1C = GSmemLock(lbl_8047AB18);
        for (i = 0; i < lbl_8047AB20; i++) {
            lbl_8047AB1C[i].inUse = 0;
        }
        HSD_MObjSetDefaultClass((HSD_ClassInfo*)&lbl_80315490);
    }
}

void _GSmaterialObjInit(void)
{
    hsdInitClassInfo((HSD_ClassInfo*)&lbl_80315490, (HSD_ClassInfo*)&lbl_8036CB30,
                     lbl_802705C0, lbl_802705C0, sizeof(HSD_MObjInfo), sizeof(GSmatObj));
    lbl_80315490.load = _matGSmatObjLoad;
    lbl_80315490.make_texp = _matGSmatObjMakeTExp;
}

HSD_TExp* _matGSmatObjMakeTExp(HSD_MObj* mobj, HSD_TObj* tobj, HSD_TExp** list)
{
    HSD_TExp* t;
    HSD_TExp* texp;
    GSmaterial* material;
    HSD_TExp* color;
    HSD_TExp* tev;
    HSD_TExp* alpha;

    texp = lbl_8036CB30.make_texp(mobj, tobj, list);
    material = ((GSmatObj*)mobj)->material;

    if (material == NULL) {
        return texp;
    }

    if (material->extensions & 2) {
        for (t = texp; t != NULL; t = t->comm.next) {
            if (HSD_TExpGetType(t) == 1) {
                fn_801B6DC0(t, material->channels[0], material->channels[1],
                            material->channels[2], material->channels[3]);
            }
        }
    }

    if (material->extensions & 1) {
        color = HSD_TExpCnst(&material->modulate[0], 1, 0, list);
        alpha = HSD_TExpCnst(&material->modulate[3], 6, 0, list);
        tev = HSD_TExpTev(list);

        HSD_TExpColorOp(tev, 0, 0, 1, 1);
        HSD_TExpColorIn(tev, 0, NULL, 1, color, 1, texp, 0, NULL);
        HSD_TExpAlphaOp(tev, 0, 0, 0, 1);
        HSD_TExpAlphaIn(tev, 0, NULL, 5, alpha, 5, texp, 0, NULL);
        texp = tev;
    }
    return texp;
}

#endif /* !GS_MATERIAL_ENVMAP_TAIL_ONLY */

#if !defined(GS_MATERIAL_EXACT_HEAD_ONLY)
void _matGSmatEnableEnvMapExt(GSmaterial* material)
{
    GStextureHandle* tex = material->envTexture;
    HSD_MObj* mobj;
    HSD_ImageDesc* imageDesc;
    HSD_TObj* tobj;
    HSD_TObj* last;

    if (tex != NULL) {
        mobj = material->mobj;
        imageDesc = HSD_ImageDescAlloc();
        lbl_803154E4.blend_flags = 0x80;
        lbl_803154E4.blending = material->envBlend;
        switch (material->envMode) {
        case 1:
            lbl_803154E4.blend_flags = lbl_803154E4.blend_flags | 5;
            break;
        case 0:
            lbl_803154E4.blend_flags = lbl_803154E4.blend_flags | 1;
            break;
        case 2:
        default:
            lbl_803154E4.blend_flags = lbl_803154E4.blend_flags | 6;
            break;
        }
        if (material->envType == 0) {
            lbl_803154E4.blend_flags = lbl_803154E4.blend_flags | 0x40000;
        } else {
            lbl_803154E4.blend_flags = lbl_803154E4.blend_flags | 0x30000;
        }

        imageDesc->image_ptr = GStextureLockImage(tex, 0);
        imageDesc->width = GStextureGetXsize(tex);
        imageDesc->height = GStextureGetYsize(tex);
        imageDesc->format = GStextureGetGXformat(tex, 1);
        switch (imageDesc->format) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 14:
            break;
        default:
            GSlogWrite(lbl_802705D0);
            HSD_ImageDescFree(imageDesc);
            HSD_ImageDescRemove(imageDesc);
            materialDisableExtension(material, 4);
            return;
        }

        imageDesc->mipmap = GStextureGetMiplevels(tex) != 0;
        /* RULE-EXCEPTION(user-approved): extern named stand-in for the TU's pool literal - see docs/RULE_EXCEPTIONS.md */
        imageDesc->minLOD = lbl_8047CAC8;
        imageDesc->maxLOD = lbl_8047CAC8;
        lbl_803154E4.imagedesc = imageDesc;
        tobj = HSD_TObjLoadDesc(&lbl_803154E4);
        last = HSD_MObjGetTObj(mobj);
        if (last != NULL) {
            while (last->next != NULL) {
                last = last->next;
            }
            HSD_MObjAddTObjNext(mobj, last, tobj);
        } else {
            fn_801A6DA0(mobj, tobj);
        }
        material->envTObj = tobj;
        material->envImageDesc = imageDesc;
        GStextureUnlockImage(tex);
    } else {
        GSlogWrite(lbl_80270610);
        materialDisableExtension(material, 4);
    }
}

/* _matGSmatObjLoad (0x800DFE98) is in gs_material_exact_800DFE98.c. */
#endif /* !GS_MATERIAL_EXACT_HEAD_ONLY */
