/**
 * @file hsd_mobj_ext.c
 * @brief HSD MObj extended - Material setup, render mode, PE configuration.
 *
 * Address range: 0x801A8354 - 0x801A85A4
 * Contains material setup helpers, pixel engine configuration,
 * and render mode accessor stubs. These are the tail end of the
 * MObj/material system before the matrix utility code begins.
 */

#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_mobj.h"

/* External functions */
/* HSD_AObjReqAnim(frame in f1). The one-arg local decl avoids a duplicate
 * compiler-emitted fmr; the inline fmr below matches the original call setup. */
extern void fn_801C29C4(HSD_AObj* aobj);
extern void fn_801BEE68(HSD_TObj* tobj, u32 flags, f32 frame); /* HSD_TObjReqAnimAll */
extern void fn_801C25E4(HSD_AObj* aobj);                /* HSD_AObjRemove */
extern HSD_AObj* fn_801C2670(void* aobjdesc);           /* HSD_AObjLoadDesc */
extern void fn_801BEEDC(HSD_TObj* tobj, void* texanim); /* HSD_TObjAddAnimAll */
extern void fn_801AA35C(void* list, u32 size, u32 alignment);
extern void HSD_ObjFree(void* list, void* data);
extern void* HSD_ObjAlloc(void* list);
extern void __assert(const char* file, u32 line, const char* msg);

/* BSS vtx desc globals */
extern u8 lbl_80465620[];
extern u8 lbl_8046564C[];

/* SDA2 string constants */
extern const char lbl_8047DC48[6];
extern const char lbl_8047DC50[4];
extern const char lbl_8047DC54[4];

/* SDA variable */
extern HSD_MObj* lbl_8047B2D4;

/* ========================================================================= */
/*  Material PE and texture setup                                            */
/* ========================================================================= */

/* Address: 0x801A8354 | Size: 0x68 */
/* HSD_MObjReqAnim - request material animation at given frame.
 * r3=mobj, r4=flags, f1=frame */
void HSD_MObjReqAnimByFlags(HSD_MObj* mobj, u32 flags, f32 frame) {
    if (mobj == NULL) {
        return;
    }
    if (flags & 4) {
        asm {
            fmr f1, f31
        }
        fn_801C29C4(mobj->aobj);
    }
    fn_801BEE68(mobj->tobj, flags, frame);
}

/* Address: 0x801A83BC | Size: 0x6C */
/* HSD_MObjAddAnim - add material animation to an MObj */
void fn_801A83BC(HSD_MObj* mobj, HSD_MatAnim* matanim) {
    if (mobj == NULL) {
        return;
    }
    if (matanim == NULL) {
        return;
    }
    if (*(HSD_AObj* volatile*)&mobj->aobj != NULL) {
        fn_801C25E4(mobj->aobj);
    }
    mobj->aobj = fn_801C2670(matanim->aobjdesc);
    fn_801BEEDC(mobj->tobj, matanim->texanim);
}

/* ========================================================================= */
/*  Render mode flag operations                                              */
/* ========================================================================= */

/* Address: 0x801A8428 | Size: 0x18 */
/* HSD_MObjClearFlags - clear bits in rendermode */
void HSD_MObjClearFlags(HSD_MObj* mobj, u32 flags) {
    if (mobj == NULL) {
        return;
    }
    mobj->rendermode &= ~flags;
}

/* Address: 0x801A8440 | Size: 0x18 */
/* HSD_MObjSetFlags - set bits in rendermode */
void HSD_MObjSetFlags(HSD_MObj* mobj, u32 flags) {
    if (mobj == NULL) {
        return;
    }
    mobj->rendermode |= flags;
}

/* Address: 0x801A8458 | Size: 0x18 */
/* HSD_MObjGetFlags - return the entire rendermode value */
u32 HSD_MObjGetFlags(HSD_MObj* mobj) {
    if (mobj != NULL) {
        return mobj->rendermode;
    }
    return 0;
}

/* NOTE: HSD_MObjSetCurrent is an SDA setter for lbl_8047B2D4, already 100% matching,
 * but it belongs in this TU so we include it */

/* Address: 0x801A8470 | Size: 0x8 */
void HSD_MObjSetCurrent(HSD_MObj* mobj) {
    lbl_8047B2D4 = mobj;
}

/* ========================================================================= */
/*  Vertex descriptor list management                                        */
/* ========================================================================= */

/* Address: 0x801A8478 | Size: 0x30 */
/* Initialize matrix allocation data. */
void HSD_MtxInitAllocData(void) {
    fn_801AA35C(lbl_80465620, 0x30, 4);
}

/* Address: 0x801A84A8 | Size: 0xC */
/* Get matrix allocation data. */
void* HSD_MtxGetAllocData(void) {
    return lbl_80465620;
}

/* Address: 0x801A84B4 | Size: 0x30 */
/* Initialize alpha vertex descriptor list */
void HSD_VecInitAllocData(void) {
    fn_801AA35C(lbl_8046564C, 0xC, 4);
}

/* Address: 0x801A84E4 | Size: 0xC */
/* Get pointer to alpha vertex descriptor list */
void* HSD_VecGetAllocData(void) {
    return lbl_8046564C;
}

/* Address: 0x801A84F0 | Size: 0x34 */
/* Free matrix allocation data. */
void HSD_MtxFree(void* data) {
    if (data != NULL) {
        HSD_ObjFree(lbl_80465620, data);
    }
}

/* Address: 0x801A8524 | Size: 0x4C */
/* Allocate matrix data. */
void* HSD_MtxAlloc(void) {
    void* result;
    result = HSD_ObjAlloc(lbl_80465620);
    if (result == NULL) {
        __assert(lbl_8047DC48, 0x396, lbl_8047DC50);
    }
    return result;
}

/* Address: 0x801A8570 | Size: 0x34 */
/* Set vertex attribute format for alpha channel */
void HSD_VecFree(void* data) {
    if (data != NULL) {
        HSD_ObjFree(lbl_8046564C, data);
    }
}

/* Address: 0x801A85A4 | Size: 0x4C */
/* Allocate/init alpha channel vtx desc with assert */
void* HSD_VecAlloc(void) {
    void* result;
    result = HSD_ObjAlloc(lbl_8046564C);
    if (result == NULL) {
        __assert(lbl_8047DC48, 0x377, lbl_8047DC54);
    }
    return result;
}
