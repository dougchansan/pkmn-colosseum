/**
 * @file hsd_pobj.h
 * @brief HSD PObj - Primitive/polygon objects.
 *
 * PObj manages mesh/polygon data: vertex descriptor lists, display
 * list data, skinning (rigid, envelope, shape animation), and
 * rendering dispatch.
 *
 * Colosseum address range: 0x801AA608 (HSD_PObjInit)
 * Adapted from the Melee decompilation (doldecomp/melee).
 */
#ifndef HSD_POBJ_H
#define HSD_POBJ_H

#include "dolphin/types.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_forward.h"

/* ========================================================================= */
/*  Matrix type constants                                                    */
/* ========================================================================= */

#define HSD_MTX_RIGID    1
#define HSD_MTX_ENVELOPE 2

/* ========================================================================= */
/*  PObj structure                                                           */
/* ========================================================================= */

struct HSD_PObj {
    HSD_Class parent;
    HSD_PObj* next;
    HSD_VtxDescList* verts;
    u16 flags;
    u16 n_display;
    u8* display;               /* display list: primitive, vtxcnt, indices */
    union {
        HSD_JObj* jobj;
        HSD_ShapeSet* shape_set;
        HSD_SList* envelope_list;
    } u;
    HSD_AObj* aobj;
};

/* ========================================================================= */
/*  PObj descriptor (data format)                                            */
/* ========================================================================= */

struct HSD_PObjDesc {
    char* class_name;
    HSD_PObjDesc* next;
    HSD_VtxDescList* verts;
    u16 flags;
    u16 n_display;
    u8* display;
    union {
        HSD_Joint* joint;
        HSD_ShapeSetDesc* shape_set;
        HSD_EnvelopeDesc** envelope_p;
    } u;
};

/* ========================================================================= */
/*  Vertex descriptor list                                                   */
/* ========================================================================= */

struct HSD_VtxDescList {
    s32 attr;          /* GXAttr */
    s32 attr_type;     /* GXAttrType */
    s32 comp_cnt;      /* GXCompCnt */
    s32 comp_type;     /* GXCompType */
    u8 frac;
    u16 stride;
    void* vertex;
};

/* ========================================================================= */
/*  Envelope (skeletal skinning weights)                                     */
/* ========================================================================= */

struct HSD_Envelope {
    HSD_Envelope* next;
    HSD_JObj* jobj;
    f32 weight;
};

struct HSD_EnvelopeDesc {
    HSD_Joint* joint;
    f32 weight;
};

/* ========================================================================= */
/*  Shape set (morph target / shape animation)                               */
/* ========================================================================= */

struct HSD_ShapeSet {
    u16 flags;
    u16 nb_shape;
    s32 nb_vertex_index;
    HSD_VtxDescList* vertex_desc;
    u8** vertex_idx_list;
    s32 nb_normal_index;
    HSD_VtxDescList* normal_desc;
    u8** normal_idx_list;
    union {
        f32* bp;
        f32 bl;
    } blend;
};

struct HSD_ShapeSetDesc {
    u16 flags;
    u16 nb_shape;
    s32 nb_vertex_index;
    HSD_VtxDescList* vertex_desc;
    u8** vertex_idx_list;
    s32 nb_normal_index;
    HSD_VtxDescList* normal_desc;
    u8** normal_idx_list;
};

/* ========================================================================= */
/*  Shape animation types                                                    */
/* ========================================================================= */

struct HSD_ShapeAnim {
    HSD_ShapeAnim* next;
    HSD_AObjDesc* aobjdesc;
};

struct HSD_ShapeAnimJoint {
    HSD_ShapeAnimJoint* child;
    HSD_ShapeAnimJoint* next;
    HSD_ShapeAnimDObj* shapeanimdobj;
};

/* ========================================================================= */
/*  PObj class info                                                          */
/* ========================================================================= */

struct HSD_PObjInfo {
    HSD_ClassInfo parent;
    void (*disp)(HSD_PObj* pobj, f32 vmtx[3][4], f32 pmtx[3][4],
                 u32 rendermode);
    void (*setup_mtx)(HSD_PObj* pobj, f32 vmtx[3][4], f32 pmtx[3][4],
                      u32 rendermode);
    s32 (*load)(HSD_PObj* pobj, HSD_PObjDesc* desc);
};

/* ========================================================================= */
/*  Globals and macros                                                       */
/* ========================================================================= */

extern HSD_PObjInfo hsdPObj;

#define HSD_POBJ(o) ((HSD_PObj*) (o))
#define HSD_POBJ_INFO(i) ((HSD_PObjInfo*) (i))
#define HSD_POBJ_METHOD(o) HSD_POBJ_INFO(HSD_CLASS_METHOD(o))

/* ========================================================================= */
/*  Function declarations                                                    */
/* ========================================================================= */

HSD_PObj* HSD_PObjAlloc(void);
u32 HSD_PObjGetFlags(HSD_PObj* pobj);
HSD_PObj* HSD_PObjLoadDesc(HSD_PObjDesc* desc);
void HSD_PObjAddAnim(HSD_PObj* pobj, HSD_ShapeAnim* anim);
void HSD_PObjAddAnimAll(HSD_PObj* pobj, HSD_ShapeAnim* anim);
void HSD_PObjAnim(HSD_PObj* pobj);
void HSD_PObjAnimAll(HSD_PObj* pobj);
void HSD_PObjResolveRefs(HSD_PObj* pobj, HSD_PObjDesc* desc);
void HSD_PObjResolveRefsAll(HSD_PObj* pobj, HSD_PObjDesc* desc);
void HSD_PObjRemove(HSD_PObj* pobj);
void HSD_PObjRemoveAll(HSD_PObj* pobj);
void HSD_PObjDisp(HSD_PObj* pobj, f32 vmtx[3][4], f32 pmtx[3][4],
                  u32 rendermode);

#endif /* HSD_POBJ_H */
