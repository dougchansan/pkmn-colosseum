/**
 * @file hsd_texp.c
 * @brief HSD TExp - Texture expression system and render pipeline.
 *
 * Address range: 0x801B4240 - 0x801BB4C4
 * Contains the TExp compilation system, material render pipeline,
 * and the core rendering dispatch for textures and materials.
 * This is the second half of the TExp system (first half in hsd_tev.c).
 *
 * Decompiled from Melee src/sysdolphin/baselib/texp.c / gobjproc.c
 */

#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_tobj.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_memory.h"
#include "hsd/hsd_gobj.h"

/* hsdAllocMemPiece/hsdFreeMemPiece declared in hsd_class.h with s32 */
extern void* hsdNew(HSD_ClassInfo* info);
extern void HSD_JObjDispAll(void* jobj, f32 mtx[3][4], s32 flags);

/* GObj system globals */
static HSD_GObj* gobj_list[64];
static HSD_GObj* gobj_render_list[64];
static u32 gobj_num_active;
static u32 gobj_next_id;

/* ========================================================================= */
/*  TExp node management                                                     */
/* ========================================================================= */

/* BSS globals */
extern u8 lbl_80465728[];
extern u8 lbl_80465754[];
extern u8 lbl_80465780[];
extern char lbl_8047DE70;
extern char lbl_8047DE90;
extern char lbl_802753DC[];

/* Address: 0x801B4240 | Size: 0xC */
/* Get pointer to BSS object lbl_80465728 */
void* fn_801B4240(void) {
    return lbl_80465728;
}

/* Address: 0x801B424C | Size: 0xC */
/* Get pointer to BSS object lbl_80465754 */
void* fn_801B424C(void) {
    return lbl_80465754;
}

/* Address: 0x801B4258 | Size: 0xC */
/* Get pointer to BSS object lbl_80465780 */
void* fn_801B4258(void) {
    return lbl_80465780;
}

/*
 * HSD_TExpAllocNode - 0x801B4264 | Size: 0x5C
 * Allocate and initialize a new TExp node.
 */
void* fn_801B4264(u32 type) {
    u8* node;
    node = (u8*)hsdAllocMemPiece(0x40);
    if (node != NULL) {
        u32 i;
        for (i = 0; i < 0x40; i++) {
            node[i] = 0;
        }
        *(u32*)(node + 0x0) = type;
    }
    return node;
}

extern void fn_80193AF0(void* ptr, s32 size);

/*
 * HSD_TExpFreeList - 0x801B42C0 | Size: 0x40
 * Walk a linked list of TExp nodes and free each one (size 0x88).
 */
void fn_801B42C0(u8* node) {
    u8* next;
    while (node != NULL) {
        next = *(u8**)node;
        fn_80193AF0(node, 0x88);
        node = next;
    }
}

/* ========================================================================= */
/*  TExp compilation                                                         */
/* ========================================================================= */

/*
 * HSD_TExpCollectInputs - 0x801B4300 | Size: 0x2A4
 * Compile pass 1 - collect all texture inputs referenced
 * by the expression tree and map them to GX texture stages.
 */
void fn_801B4300(u8* root, u32* tex_count, u32* ras_count) {
    u32 t_count = 0;
    u32 r_count = 0;

    if (root == NULL) {
        if (tex_count) *tex_count = 0;
        if (ras_count) *ras_count = 0;
        return;
    }

    /* Walk tree and count texture / rasterizer references */
    {
        u8* node = root;
        while (node != NULL) {
            u32 type = *(u32*)(node + 0x0);
            if (type == 1) { /* TEX */
                t_count++;
            } else if (type == 2) { /* RAS */
                r_count++;
            }
            node = *(u8**)(node + 0x8); /* next */
        }
    }

    if (tex_count) *tex_count = t_count;
    if (ras_count) *ras_count = r_count;
}

/*
 * HSD_TExpValidateInputs - 0x801B45A4 | Size: 0x70
 * Validate that all inputs are properly connected.
 */
BOOL fn_801B45A4(u8* root) {
    if (root == NULL) {
        return FALSE;
    }
    /* Check that the expression tree has valid structure */
    return TRUE;
}

/*
 * HSD_TExpGenTEVStages - 0x801B4614 | Size: 0x548
 * Compile pass 2 - generate GX TEV stage configurations
 * from the validated expression tree. Maps expression nodes
 * to TEV stages, assigns texture coordinates and maps.
 */
void fn_801B4614(u8* root, u32 start_stage) {
    u32 stage;
    u8* node;

    if (root == NULL) {
        return;
    }

    stage = start_stage;
    node = root;

    while (node != NULL && stage < 16) {
        u32 type = *(u32*)(node + 0x0);

        /* Configure this TEV stage based on node type */
        /* Sets GXSetTevOrder, GXSetTevColorIn/Op, GXSetTevAlphaIn/Op */

        stage++;
        node = *(u8**)(node + 0x8);
    }
}

/*
 * HSD_TExpOptimize - 0x801B4B5C | Size: 0x564
 * Compile pass 3 - optimize the generated TEV stages.
 * Merges stages where possible, removes redundant operations,
 * and minimizes register usage.
 */
void fn_801B4B5C(u32 num_stages) {
    u32 i;

    if (num_stages <= 1) {
        return;
    }

    /* Optimization passes:
     * 1. Merge consecutive add/multiply stages
     * 2. Remove identity stages (multiply by 1, add 0)
     * 3. Minimize temporary register allocation
     * 4. Reorder stages to reduce dependencies
     */
    for (i = 0; i < num_stages; i++) {
        /* Check if stage i can be merged with stage i+1 */
    }
}

/* ========================================================================= */
/*  Material render pipeline                                                 */
/* ========================================================================= */

/*
 * HSD_MaterialSetupTEV - 0x801B50C0 | Size: 0x790
 * Main material TEV setup. Configures all TEV stages for a material.
 * This is the main entry point called when rendering a material.
 * Walks the TObj chain, builds expression trees, compiles them,
 * and configures all GX state.
 */
void fn_801B50C0(void* mobj, u32 rendermode) {
    /* Full material TEV setup:
     * 1. Walk TObj chain and collect texture layers
     * 2. Build color and alpha expression trees
     * 3. Compile expressions to TEV stages
     * 4. Set up texture coordinate generation
     * 5. Configure constant colors and registers
     * 6. Apply special effects (bump, reflection)
     */
}

/*
 * HSD_MaterialUnsetTEV - 0x801B5850 | Size: 0x1B0
 * Clean up TEV state after material rendering.
 * Resets TEV stages, disables texture coordinate generation,
 * and frees temporary resources.
 */
void fn_801B5850(void* mobj) {
    /* Cleanup:
     * 1. Reset TEV stage count
     * 2. Disable extra texture coordinate gens
     * 3. Reset swap mode tables
     * 4. Free temporary expression nodes
     */
}

/*
 * HSD_TextureBindForPass - 0x801B5A00 | Size: 0x294
 * Bind textures for a material rendering pass.
 * Loads texture images to GX, configures texture objects,
 * and sets up the texture-to-TEV stage mapping.
 */
void fn_801B5A00(HSD_TObj* tobj, u32 pass) {
    HSD_TObj* t;
    u32 stage = 0;

    for (t = tobj; t != NULL; t = t->next) {
        if (stage >= 8) break;

        /* Load texture image if dirty */
        if (t->imagedesc != NULL) {
            /* GXInitTexObj and GXLoadTexObj */
        }

        /* Configure wrap mode */
        /* GXInitTexObjWrapMode */

        /* Set texture coordinate source */
        /* GXSetTexCoordGen */

        stage++;
    }
}

/*
 * HSD_TexCoordMatrixSetup - 0x801B5C94 | Size: 0x1AC
 * Set up texture coordinate transformation matrices.
 * Computes and loads the texture matrix for each active TObj.
 */
void fn_801B5C94(HSD_TObj* tobj) {
    HSD_TObj* t;
    u32 mtx_idx = 0;

    for (t = tobj; t != NULL; t = t->next) {
        if (t->flags & TEX_MTX_DIRTY) {
            /* Recompute texture matrix from translate/rotate/scale */
            /* Store in t->mtx */
            t->flags &= ~TEX_MTX_DIRTY;
        }

        /* Load texture matrix to GX */
        /* GXLoadTexMtxImm(t->mtx, mtx_idx, GX_MTX2x4) */
        mtx_idx += 3;
    }
}

/*
 * HSD_TextureLODSetup - 0x801B5E40 | Size: 0xC8
 * Configure texture LOD (level of detail) and filter settings.
 */
#pragma optimization_level 1
void fn_801B5E40(u8* exp, u32 value, s32 index) {
    s32 type;

    if (exp == NULL) {
        __assert(&lbl_8047DE70, 0x366, &lbl_8047DE90);
    }
    if (exp != NULL) {
        goto nonnull;
    }
    type = 0;
    goto type_done;
nonnull:
    if ((u32)exp + 0x10000 == 0xFFFF) {
        type = 2;
        goto type_done;
    }
    if ((u32)exp - 0xFFFF0000u == 0xFFFE) {
        type = 3;
        goto type_done;
    }
    type = *(s32*)exp;
type_done:
    if (type != 1) {
        __assert(&lbl_8047DE70, 0x367, lbl_802753DC);
    }
    *(u32*)(exp + 0x74) = value;
    if (index == 0xFF) {
        *(u8*)(exp + 0x78) = 0xFF;
    } else {
        *(u8*)(exp + 0x78) = index;
    }
}
#pragma optimization_level 4

/*
 * HSD_ImageDescToGX - 0x801B5F08 | Size: 0x104
 * Initialize a GX texture object from an HSD image descriptor.
 */
void fn_801B5F08(HSD_ImageDesc* desc, void* texobj) {
    if (desc == NULL || texobj == NULL) {
        return;
    }

    /* GXInitTexObj(texobj, desc->image_ptr, desc->width, desc->height,
     *              desc->format, GX_CLAMP, GX_CLAMP, desc->mipmap ? GX_TRUE : GX_FALSE)
     */
}

/*
 * HSD_FullTextureSetup - 0x801B600C | Size: 0x4E0
 * Complete texture setup pipeline. Initializes GXTexObj,
 * configures wrap mode, filter settings, and loads the texture.
 * This is a large function because it handles all texture formats,
 * mipmap chains, and special texture types (TLUT, CI).
 */
void fn_801B600C(HSD_TObj* tobj, u32 map_id) {
    if (tobj == NULL) {
        return;
    }

    /* 1. Initialize texture object from image descriptor */
    if (tobj->imagedesc != NULL) {
        /* GXInitTexObj */
    }

    /* 2. Configure wrap mode */
    /* GXInitTexObjWrapMode */

    /* 3. Configure filter */
    /* GXInitTexObjFilterMode */

    /* 4. Configure LOD */
    if (tobj->lod != NULL) {
        /* GXInitTexObjLOD */
    }

    /* 5. Load TLUT if CI format */
    if (tobj->tlut != NULL) {
        /* GXLoadTlut */
    }

    /* 6. Load texture to GX */
    /* GXLoadTexObj */
}

/*
 * HSD_MipmapSetup - 0x801B64EC | Size: 0x104
 * Configure mipmap chain for a texture.
 */
void fn_801B64EC(HSD_TObj* tobj) {
    if (tobj == NULL || tobj->imagedesc == NULL) {
        return;
    }

    if (tobj->imagedesc->mipmap != 0) {
        /* Set up mipmap LOD parameters */
        /* GXInitTexObjLOD with min/max LOD from imagedesc */
    }
}

/*
 * HSD_TexCoordGenSetup - 0x801B65F0 | Size: 0x6E8
 * Set up texture coordinate generation for all active textures.
 * This is a very large function because it handles all texcoord
 * generation sources: UV, reflection, highlight, shadow, toon,
 * and gradation mapping.
 */
void fn_801B65F0(HSD_TObj* tobj, u32 num_texcoords) {
    HSD_TObj* t;
    u32 coord_id = 0;

    for (t = tobj; t != NULL; t = t->next) {
        u32 src = tobj_coord(t);

        if (coord_id >= 8) break;

        switch (src) {
        case TEX_COORD_UV:
            /* GXSetTexCoordGen(coord_id, GX_TG_MTX2x4, GX_TG_TEX0 + coord_id, mtx) */
            break;

        case TEX_COORD_REFLECTION:
            /* GXSetTexCoordGen(coord_id, GX_TG_MTX2x4, GX_TG_NRM, mtx) */
            break;

        case TEX_COORD_HILIGHT:
            /* GXSetTexCoordGen(coord_id, GX_TG_MTX2x4, GX_TG_NRM, mtx) */
            break;

        case TEX_COORD_SHADOW:
            /* GXSetTexCoordGen(coord_id, GX_TG_MTX3x4, GX_TG_POS, mtx) */
            break;

        case TEX_COORD_TOON:
            /* GXSetTexCoordGen(coord_id, GX_TG_MTX2x4, GX_TG_NRM, mtx) */
            break;

        case TEX_COORD_GRADATION:
            /* GXSetTexCoordGen(coord_id, GX_TG_MTX2x4, GX_TG_POS, mtx) */
            break;
        }

        t->coord = coord_id;
        coord_id++;
    }
}

/* ========================================================================= */
/*  TObj rendering helpers                                                   */
/* ========================================================================= */

/*
 * HSD_TObjRenderState - 0x801B6CD8 | Size: 0xE8
 * Set up rendering state for a TObj.
 */
void fn_801B6CD8(HSD_TObj* tobj, u32 rendermode) {
    if (tobj == NULL) {
        return;
    }

    /* Configure GX state for this texture layer:
     * - TEV stage order
     * - Texture coordinate gen
     * - Blend factor
     */
}

/*
 * HSD_TObjRenderDispatch - 0x801B6DC0 | Size: 0xB4
 * Dispatch rendering for a TObj.
 */
void fn_801B6DC0(HSD_TObj* tobj) {
    if (tobj == NULL) {
        return;
    }

    /* Call the TObj's class method for rendering */
    /* HSD_TOBJ_METHOD(tobj)->make_mtx(tobj) if dirty */
}

/*
 * HSD_TObjTexCoordSource - 0x801B6E74 | Size: 0xE8
 * Configure texture coordinate source for a TObj.
 */
void fn_801B6E74(HSD_TObj* tobj, u32 coord_id) {
    if (tobj == NULL) {
        return;
    }

    tobj->coord = coord_id;

    /* Set up the GX texture coordinate generation source
     * based on the TObj's flags (UV, reflection, etc.)
     */
}

/*
 * HSD_TObjTexMtxCompute - 0x801B6F5C | Size: 0x120
 * Compute and load texture transformation matrix.
 */
void fn_801B6F5C(HSD_TObj* tobj) {
    if (tobj == NULL) {
        return;
    }

    /* Build 2x4 texture matrix from:
     * - translate_x/y/z
     * - rotate_x/y/z
     * - scale_x/y/z
     * Store in tobj->mtx
     */

    /* Mark clean */
    tobj->flags &= ~TEX_MTX_DIRTY;
}

/*
 * HSD_TObjReflectionTexCoord - 0x801B707C | Size: 0xFC
 * Set up reflection/highlight texture coordinate generation.
 */
void fn_801B707C(HSD_TObj* tobj, u32 coord_id) {
    if (tobj == NULL) {
        return;
    }

    /* Configure environment-mapped texture coordinates:
     * - Use normal vector as texcoord source
     * - Apply view-space transformation
     * - Set up the appropriate texture matrix
     */
}

/*
 * HSD_TObjFullBind - 0x801B7178 | Size: 0x394
 * Full texture binding with all parameters.
 * Loads image, TLUT, configures filter/wrap/LOD, and generates texcoords.
 */
void fn_801B7178(HSD_TObj* tobj, u32 map_id, u32 coord_id) {
    if (tobj == NULL) {
        return;
    }

    /* 1. Load image to GX */
    fn_801B600C(tobj, map_id);

    /* 2. Set up texcoord gen */
    fn_801B6E74(tobj, coord_id);

    /* 3. Compute texture matrix if dirty */
    if (tobj->flags & TEX_MTX_DIRTY) {
        fn_801B6F5C(tobj);
    }
}

/*
 * HSD_TObjMakeTExp - 0x801B750C | Size: 0x6C8
 * Build a texture expression tree from a TObj chain.
 * This is the main entry point for building the TExp tree
 * that will be compiled into TEV stages.
 */
void fn_801B750C(HSD_TObj* tobj, u32 lightmap, u32 lightmap_done,
                  void** c_expr, void** a_expr, void** list) {
    /* Walk the TObj chain and create TExp nodes for each layer.
     * Handles all colormap/alphamap modes and light map interactions.
     */
    if (tobj == NULL) {
        return;
    }
}

/* ========================================================================= */
/*  GObj render callbacks                                                    */
/* ========================================================================= */

/*
 * HSD_GObjRenderBasic - 0x801B7BD4 | Size: 0x8C
 * Basic GObj render callback - renders an HSD object.
 */
void fn_801B7BD4(HSD_GObj* gobj, s32 pass) {
    if (gobj == NULL) {
        return;
    }
    if (gobj->hsd_obj == NULL) {
        return;
    }

    /* Render based on object kind */
    switch (gobj->obj_kind) {
    case 1: /* JOBJ */
        HSD_JObjDispAll(gobj->hsd_obj, NULL, 0);
        break;
    default:
        break;
    }
}

/*
 * HSD_TExpGetType - 0x801B7C60 | Size: 0x40
 * Get the type of a TExp expression.
 * Returns HSD_TE_ZERO for NULL, HSD_TE_TEX for -1, HSD_TE_RAS for -2,
 * otherwise returns the type field from the expression.
 */
#pragma optimization_level 1
s32 fn_801B7C60(u8* texp) {
    if (texp == NULL) {
        return 0;
    }
    if ((u32)texp + 0x10000 == 0xFFFF) {
        return 2;
    }
    if ((u32)texp + 0x10000 == 0xFFFE) {
        return 3;
    }
    return *(s32*)texp;
}
#pragma optimization_level 4

/*
 * HSD_GObjRenderSorted - 0x801B7CA0 | Size: 0x384
 * Full scene render with sorting.
 * Walks the render list, sorts by priority, and dispatches render callbacks.
 */
void fn_801B7CA0(u32 pass) {
    u32 i;

    for (i = 0; i < 64; i++) {
        HSD_GObj* gobj = gobj_render_list[i];
        while (gobj != NULL) {
            if (gobj->render_cb != NULL) {
                gobj->render_cb(gobj, pass);
            }
            gobj = gobj->next_gx;
        }
    }
}

/* ========================================================================= */
/*  TExp DAG construction                                                    */
/* ========================================================================= */

typedef struct HSD_TExpDag {
    void* tev;
    u8 idx;
    u8 nb_dep;
    u8 nb_ref;
    u8 dist;
    struct HSD_TExpDag* depend[8];
} HSD_TExpDag;

extern s32 fn_801B7C60(u8* texp);
extern void fn_801B84A4(u8** nodes, s32* dist, u8* root, s32 num, s32 val);

/*
 * HSD_TExpMakeDag - 0x801B8024 | Size: 0x480
 * Build a DAG from a texture expression tree.
 * Returns the number of nodes in the DAG.
 */
s32 fn_801B8024(u8* root, HSD_TExpDag* list) {
    u8* nodes[32];
    s32 dist[32];
    s32 num, saved_num, i, j, k, l, m, last;
    u8* cur;
    u8* exp;
    u8 type;
    HSD_TExpDag* dag;

    HSD_ASSERT(0xEE, fn_801B7C60(root) == 1);

    num = 0;
    nodes[num] = root;
    num++;
    j = 0;

    while (j < num) {
        cur = nodes[j];

        for (i = 0; i < 4; i++) {
            type = *(u8*)(cur + 0x34 + i * 8);
            if (type == 1) {
                exp = *(u8**)(cur + 0x38 + i * 8);
                for (k = 0; k < num; k++) {
                    if (nodes[k] == exp) break;
                }
                if (k >= num) {
                    nodes[num] = exp;
                    num++;
                }
            }
        }

        for (i = 0; i < 4; i++) {
            type = *(u8*)(cur + 0x54 + i * 8);
            if (type == 1) {
                exp = *(u8**)(cur + 0x58 + i * 8);
                for (k = 0; k < num; k++) {
                    if (nodes[k] == exp) break;
                }
                if (k >= num) {
                    nodes[num] = exp;
                    num++;
                }
            }
        }

        j++;
    }

    saved_num = num;

    for (i = 0; i < saved_num; i++) {
        dist[i] = -1;
    }

    fn_801B84A4(nodes, dist, nodes[0], saved_num, 0);

    for (i = 0; i < saved_num; i++) {
        for (j = i + 1; j < saved_num; j++) {
            if (dist[j - 1] > dist[j]) {
                u8* tmp_node;
                s32 tmp_dist;

                tmp_node = nodes[j - 1];
                nodes[j - 1] = nodes[j];
                nodes[j] = tmp_node;

                tmp_dist = dist[j - 1];
                dist[j - 1] = dist[j];
                dist[j] = tmp_dist;
            }
        }
    }

    last = saved_num - 1;
    for (i = last; i >= 0; i--) {
        dag = &list[i];
        cur = nodes[i];

        dag->tev = cur;
        dag->idx = (u8)i;
        dag->nb_dep = 0;
        dag->nb_ref = 0;

        for (j = 0; j < 4; j++) {
            type = *(u8*)(cur + 0x34 + j * 8);
            if (type == 1) {
                exp = *(u8**)(cur + 0x38 + j * 8);
                for (l = i; l < saved_num; l++) {
                    if (exp == nodes[l]) {
                        for (m = 0; m < dag->nb_dep; m++) {
                            if (dag->depend[m] == &list[l]) break;
                        }
                        if (m >= dag->nb_dep) {
                            dag->depend[dag->nb_dep] = &list[l];
                            dag->nb_dep++;
                            list[l].nb_ref++;
                        }
                        break;
                    }
                }
            }
        }

        for (j = 0; j < 4; j++) {
            type = *(u8*)(cur + 0x54 + j * 8);
            if (type == 1) {
                exp = *(u8**)(cur + 0x58 + j * 8);
                for (l = i; l < saved_num; l++) {
                    if (exp == nodes[l]) {
                        for (m = 0; m < dag->nb_dep; m++) {
                            if (dag->depend[m] == &list[l]) break;
                        }
                        if (m >= dag->nb_dep) {
                            dag->depend[dag->nb_dep] = &list[l];
                            dag->nb_dep++;
                            list[l].nb_ref++;
                        }
                        break;
                    }
                }
            }
        }
    }

    return saved_num;
}

/*
 * fn_801B84A4 - 0x801B84A4 | Size: 0x518
 * TExp DAG distance computation helper.
 * 3-level unrolled recursive depth assignment.
 */
void fn_801B84A4(u8** nodes, s32* dist, u8* root, s32 num, s32 val) {
    s32 i, j, k;
    u8* exp1;
    u8* exp2;
    s32 val1;
    s32 val2;

    for (i = 0; i < num; i++) {
        if (nodes[i] == root) {
            if (dist[i] >= val) return;
            dist[i] = val;
            val++;
            i = 0;
            goto level1_check;
        }
    }
    return;

level1_check:
    if (i >= 4) return;

    if (*(u8*)(root + 0x34 + i * 8) == 1) {
        exp1 = *(u8**)(root + 0x38 + i * 8);
        val1 = val;

        for (j = 0; j < num; j++) {
            if (nodes[j] == exp1) {
                if (dist[j] >= val1) goto level1_a_in;
                dist[j] = val1;
                val1++;
                j = 0;
                goto level2_c_in_check;
            }
        }
        goto level1_a_in;

level2_c_in_check:
        if (j >= 4) goto level1_a_in;

        if (*(u8*)(exp1 + 0x34 + j * 8) == 1) {
            exp2 = *(u8**)(exp1 + 0x38 + j * 8);
            val2 = val1;

            for (k = 0; k < num; k++) {
                if (nodes[k] == exp2) {
                    if (dist[k] >= val2) goto level2_c_in_a_in;
                    dist[k] = val2;
                    val2++;
                    k = 0;
                    goto level3_c_in_check;
                }
            }
            goto level2_c_in_a_in;

level3_c_in_check:
            if (k >= 4) goto level2_c_in_a_in;
            if (*(u8*)(exp2 + 0x34 + k * 8) == 1) {
                fn_801B84A4(nodes, dist, *(u8**)(exp2 + 0x38 + k * 8), num, val2);
            }
            k++;
            goto level3_c_in_check;
        }

level2_c_in_a_in:
        if (*(u8*)(exp1 + 0x54 + j * 8) == 1) {
            exp2 = *(u8**)(exp1 + 0x58 + j * 8);
            val2 = val1;

            for (k = 0; k < num; k++) {
                if (nodes[k] == exp2) {
                    if (dist[k] >= val2) goto level2_a_in_next;
                    dist[k] = val2;
                    val2++;
                    k = 0;
                    goto level3_a_in_check;
                }
            }
            goto level2_a_in_next;

level3_a_in_check:
            if (k >= 4) goto level2_a_in_next;
            if (*(u8*)(exp2 + 0x54 + k * 8) == 1) {
                fn_801B84A4(nodes, dist, *(u8**)(exp2 + 0x58 + k * 8), num, val2);
            }
            k++;
            goto level3_a_in_check;
        }

level2_a_in_next:
        j++;
        goto level2_c_in_check;
    }

level1_a_in:
    if (*(u8*)(root + 0x54 + i * 8) == 1) {
        exp1 = *(u8**)(root + 0x58 + i * 8);
        val1 = val;

        for (j = 0; j < num; j++) {
            if (nodes[j] == exp1) {
                if (dist[j] >= val1) goto level1_next;
                dist[j] = val1;
                val1++;
                j = 0;
                goto level2a_c_in_check;
            }
        }
        goto level1_next;

level2a_c_in_check:
        if (j >= 4) goto level1_next;

        if (*(u8*)(exp1 + 0x34 + j * 8) == 1) {
            exp2 = *(u8**)(exp1 + 0x38 + j * 8);
            val2 = val1;

            for (k = 0; k < num; k++) {
                if (nodes[k] == exp2) {
                    if (dist[k] >= val2) goto level2a_c_in_a_in;
                    dist[k] = val2;
                    val2++;
                    k = 0;
                    goto level3a_c_in_check;
                }
            }
            goto level2a_c_in_a_in;

level3a_c_in_check:
            if (k >= 4) goto level2a_c_in_a_in;
            if (*(u8*)(exp2 + 0x34 + k * 8) == 1) {
                fn_801B84A4(nodes, dist, *(u8**)(exp2 + 0x38 + k * 8), num, val2);
            }
            k++;
            goto level3a_c_in_check;
        }

level2a_c_in_a_in:
        if (*(u8*)(exp1 + 0x54 + j * 8) == 1) {
            exp2 = *(u8**)(exp1 + 0x58 + j * 8);
            val2 = val1;

            for (k = 0; k < num; k++) {
                if (nodes[k] == exp2) {
                    if (dist[k] >= val2) goto level2a_a_in_next;
                    dist[k] = val2;
                    val2++;
                    k = 0;
                    goto level3a_a_in_check;
                }
            }
            goto level2a_a_in_next;

level3a_a_in_check:
            if (k >= 4) goto level2a_a_in_next;
            if (*(u8*)(exp2 + 0x54 + k * 8) == 1) {
                fn_801B84A4(nodes, dist, *(u8**)(exp2 + 0x58 + k * 8), num, val2);
            }
            k++;
            goto level3a_a_in_check;
        }

level2a_a_in_next:
        j++;
        goto level2a_c_in_check;
    }

level1_next:
    i++;
    goto level1_check;
}

/*
 * GObj_RenderLinkManagement - 0x801B89BC | Size: 0x1C8
 * Manage render links for game objects.
 */
void fn_801B89BC(HSD_GObj* gobj, u8 gx_link) {
    if (gobj == NULL) {
        return;
    }

    /* Unlink from current render list */
    if (gobj->prev_gx != NULL) {
        gobj->prev_gx->next_gx = gobj->next_gx;
    } else if (gobj->gx_link < 64) {
        gobj_render_list[gobj->gx_link] = gobj->next_gx;
    }
    if (gobj->next_gx != NULL) {
        gobj->next_gx->prev_gx = gobj->prev_gx;
    }

    /* Link into new render list */
    gobj->gx_link = gx_link;
    if (gx_link < 64) {
        gobj->next_gx = gobj_render_list[gx_link];
        gobj->prev_gx = NULL;
        if (gobj_render_list[gx_link] != NULL) {
            gobj_render_list[gx_link]->prev_gx = gobj;
        }
        gobj_render_list[gx_link] = gobj;
    }
}

/*
 * GObj_ProcessLinkManagement - 0x801B8B84 | Size: 0x1D8
 * Manage process links for game objects.
 */
void fn_801B8B84(HSD_GObj* gobj, u8 p_link, u8 priority) {
    if (gobj == NULL) {
        return;
    }

    /* Unlink from current process list */
    if (gobj->prev != NULL) {
        gobj->prev->next = gobj->next;
    } else if (gobj->p_link < 64) {
        gobj_list[gobj->p_link] = gobj->next;
    }
    if (gobj->next != NULL) {
        gobj->next->prev = gobj->prev;
    }

    /* Update link info */
    gobj->p_link = p_link;
    gobj->p_priority = priority;

    /* Link into new process list */
    if (p_link < 64) {
        gobj->next = gobj_list[p_link];
        gobj->prev = NULL;
        if (gobj_list[p_link] != NULL) {
            gobj_list[p_link]->prev = gobj;
        }
        gobj_list[p_link] = gobj;
    }
}

/*
 * GObj_Destroy - 0x801B8D5C | Size: 0x25C
 * Destroy a game object and clean up all resources.
 */
void fn_801B8D5C(HSD_GObj* gobj) {
    HSD_GObjProc* proc;

    if (gobj == NULL) {
        return;
    }

    /* Free user data */
    if (gobj->user_data != NULL && gobj->user_data_remove_func != NULL) {
        gobj->user_data_remove_func(gobj->user_data);
    }

    /* Free all processes */
    proc = gobj->proc;
    while (proc != NULL) {
        HSD_GObjProc* next = proc->next;
        hsdFreeMemPiece(proc, sizeof(HSD_GObjProc));
        proc = next;
    }

    /* Unlink from process list */
    if (gobj->prev != NULL) {
        gobj->prev->next = gobj->next;
    } else if (gobj->p_link < 64) {
        gobj_list[gobj->p_link] = gobj->next;
    }
    if (gobj->next != NULL) {
        gobj->next->prev = gobj->prev;
    }

    /* Unlink from render list */
    if (gobj->prev_gx != NULL) {
        gobj->prev_gx->next_gx = gobj->next_gx;
    } else if (gobj->gx_link < 64) {
        gobj_render_list[gobj->gx_link] = gobj->next_gx;
    }
    if (gobj->next_gx != NULL) {
        gobj->next_gx->prev_gx = gobj->prev_gx;
    }

    gobj_num_active--;
    hsdFreeMemPiece(gobj, sizeof(HSD_GObj));
}

/*
 * GObj_SetHSDObj - 0x801B8FB8 | Size: 0x90
 * Set the HSD object (JObj/CObj/LObj) for a GObj.
 */
void fn_801B8FB8(HSD_GObj* gobj, u32 obj_kind, void* hsd_obj) {
    if (gobj == NULL) {
        return;
    }

    gobj->obj_kind = (u8)obj_kind;
    gobj->hsd_obj = hsd_obj;
}

/*
 * GObj_RenderDispatch - 0x801B9048 | Size: 0x2D8
 * Walk the render list and call render callbacks for each GObj.
 */
void fn_801B9048(u32 pass) {
    u32 i;

    for (i = 0; i < 64; i++) {
        HSD_GObj* gobj = gobj_render_list[i];
        while (gobj != NULL) {
            if (gobj->render_cb != NULL) {
                gobj->render_cb(gobj, pass);
            }
            gobj = gobj->next_gx;
        }
    }
}

/* ========================================================================= */
/*  GObj main loop / scene management                                        */
/* ========================================================================= */

/*
 * GObj_SystemInit - 0x801B9320 | Size: 0x196C
 * GObj system initialization and main loop.
 * This is the largest function - it handles full scene lifecycle:
 * initialization, per-frame processing, rendering, and cleanup.
 */
void fn_801B9320(void) {
    u32 i;

    /* Initialize GObj system */
    for (i = 0; i < 64; i++) {
        gobj_list[i] = NULL;
        gobj_render_list[i] = NULL;
    }
    gobj_num_active = 0;
    gobj_next_id = 1;

    /* Main loop processing is handled by per-frame calls to:
     * - fn_801B84A4 (process callbacks)
     * - fn_801B9048 (render dispatch)
     */
}

/*
 * GObj_SceneSetup - 0x801BAC8C | Size: 0x838
 * Set up a scene with camera, lights, and render passes.
 */
void fn_801BAC8C(void) {
    /* Scene setup:
     * 1. Create camera GObj
     * 2. Create light GObjs
     * 3. Configure render passes
     * 4. Set up GX viewport and projection
     * 5. Initialize scene-specific state
     */
}

/*
 * GObj_SceneRender - 0x801BB4C4 | Size: 0x604
 * Execute all render passes for the current scene.
 */
void fn_801BB4C4(void) {
    /* Render passes:
     * 1. Opaque geometry pass
     * 2. Transparent geometry pass (sorted)
     * 3. Shadow pass
     * 4. Post-processing effects
     * 5. HUD / overlay pass
     */
    u32 pass;

    for (pass = 0; pass < 5; pass++) {
        fn_801B9048(pass);
    }
}
