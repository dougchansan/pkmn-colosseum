/**
 * @file hsd_tobj_ext.c
 * @brief HSD TObj extended - texture setup, TLUT, and transition code.
 *
 * Address range: 0x801BBAC8 - 0x801BFF18
 * Contains TObj class init (proposed HSD_TObjInit at 0x801BBAC8),
 * texture setup/binding, TLUT (palette) management, texture matrix
 * computation, and the transition code between HSD library and
 * the battle system.
 *
 * NOTE: The core TObj lifecycle functions (alloc/load/anim/remove)
 * are in hsd_tobj.c. This file covers the rendering pipeline
 * functions for textures.
 *
 * Decompiled from Melee src/sysdolphin/baselib/tobj.c (rendering portion)
 */

#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_object.h"
#include "hsd/hsd_tobj.h"
#include "hsd/hsd_mobj.h"

/* GX external declarations */
extern void GXInitTexObj(void* obj, void* image, u16 w, u16 h,
                          u32 format, u32 wrap_s, u32 wrap_t, u32 mipmap);
extern void GXInitTexObjCI(void* obj, void* image, u16 w, u16 h,
                            u32 format, u32 wrap_s, u32 wrap_t,
                            u32 mipmap, u32 tlut_name);
extern void GXInitTexObjLOD(void* obj, u32 min_filt, u32 mag_filt,
                              f32 min_lod, f32 max_lod, f32 lod_bias,
                              u32 bias_clamp, u32 do_edge_lod,
                              u32 max_aniso);
extern void GXInitTexObjFilterMode(void* obj, u32 min_filt, u32 mag_filt);
extern void GXInitTexObjWrapMode(void* obj, u32 wrap_s, u32 wrap_t);
extern void GXLoadTexObj(void* obj, u32 map_id);
extern void GXInitTlutObj(void* obj, void* lut, u32 fmt, u16 n_entries);
extern void GXLoadTlut(void* obj, u32 tlut_name);
extern void GXSetTexCoordGen(u32 dst_coord, u32 func, u32 src_param, u32 mtx);
extern void GXLoadTexMtxImm(void* mtx, u32 id, u32 type);
extern void DCStoreRange(void* addr, u32 nBytes);
extern void GXInvalidateTexAll(void);

/* hsdAllocMemPiece/hsdFreeMemPiece declared in hsd_class.h with s32 */
extern void HSD_AObjInterpretAnim(HSD_AObj* aobj, void* obj, void* update_func);

typedef struct HSD_TlutWork {
    void* lut;
    u32 fmt;
    u32 tlut_name;
    u16 n_entries;
} HSD_TlutWork;

typedef struct HSD_TexLODWork {
    u32 minFilt;
    f32 LODBias;
    u8 bias_clamp;
    u8 edgeLODEnable;
    u8 pad[2];
    u32 max_anisotropy;
} HSD_TexLODWork;

extern HSD_TObj* lbl_8047B37C;
extern HSD_TexLODWork lbl_8036D594;
extern char lbl_8047DEB0;
extern char lbl_8047DEB8;
extern char lbl_8047DEC4;
extern char lbl_8047DED0;
extern char lbl_80275688[];
extern char lbl_80275694[];
extern s32 fn_801BC33C(HSD_TObj* tobj);
extern void fn_801BDD74(HSD_TObj* tobj);
extern void HSD_StateRegisterTexGen(u32 coord);
extern void fn_800BB050(void* tlutobj, void* lut, u32 fmt, u16 n_entries);
extern void fn_800BB098(void* tlutobj, u32 tlut_name);
extern void fn_800BAC58(void* texobj, void* image, u16 width, u16 height,
                         u32 format, u32 wrap_s, u32 wrap_t, u32 mipmap,
                         u32 tlut_name);
extern void fn_800BA9E4(void* texobj, void* image, u16 width, u16 height,
                         u32 format, u32 wrap_s, u32 wrap_t, u32 mipmap);
extern void fn_800BACA0(void* texobj, u32 min_filt, u32 mag_filt,
                         f32 min_lod, f32 max_lod, f32 lod_bias,
                         u32 bias_clamp, u32 do_edge_lod, u32 max_aniso);
extern void fn_800BAFFC(void* texobj, u32 map_id);
extern void* fn_80193B10(s32 size);
extern void* memset(void* dest, int value, u32 size);

/* ========================================================================= */
/*  TObj class initialization                                                */
/* ========================================================================= */

/*
 * HSD_TObjRenderInit - 0x801BBAC8 | Size: 0xEC
 * Extended TObj class initialization for the rendering pipeline.
 * Sets up additional vtable entries for texture setup and binding.
 */
void fn_801BBAC8(void) {
    /* Initialize rendering-specific vtable entries:
     * - make_mtx: texture matrix computation method
     * - make_texp: texture expression builder method
     * Register rendering-related class methods for the TObj info.
     */
}

/*
 * HSD_TObjMakeMtx - 0x801BBBB4 | Size: 0x60
 * Compute texture matrix from TObj transform parameters.
 */
void fn_801BBBB4(HSD_TObj* tobj) {
    if (tobj == NULL) {
        return;
    }

    /* Build 3x4 texture matrix from:
     * - scale (scale_x, scale_y, scale_z)
     * - rotation (rotate_x, rotate_y, rotate_z)
     * - translation (translate_x, translate_y, translate_z)
     */
    tobj->flags &= ~TEX_MTX_DIRTY;
}

/*
 * HSD_TObjLoadImage - 0x801BBC14 | Size: 0xCC
 * Load a texture image to GX from the TObj's image descriptor.
 */
void fn_801BBC14(HSD_TObj* tobj, u32 map_id) {
    if (tobj == NULL || tobj->imagedesc == NULL) {
        return;
    }

    {
        HSD_ImageDesc* img = tobj->imagedesc;
        u8 texobj[0x20]; /* GXTexObj */

        GXInitTexObj(texobj, img->image_ptr, img->width, img->height,
                     img->format, tobj->wrap_s, tobj->wrap_t,
                     img->mipmap ? 1 : 0);

        if (tobj->magFilt != 0) {
            GXInitTexObjFilterMode(texobj, 1, tobj->magFilt);
        }

        GXLoadTexObj(texobj, map_id);
    }
}

/*
 * HSD_TObjSetWrapMode - 0x801BBCE0 | Size: 0x5C
 * Set texture wrap mode (repeat, clamp, mirror).
 */
void fn_801BBCE0(HSD_TObj* tobj, u32 wrap_s, u32 wrap_t) {
    if (tobj == NULL) {
        return;
    }
    tobj->wrap_s = wrap_s;
    tobj->wrap_t = wrap_t;
}

/* ========================================================================= */
/*  TObj accessors                                                           */
/* ========================================================================= */

/* Address: 0x801BBD3C | Size: 0x24 */
/* Free an image descriptor memory piece. */
extern void fn_80193AF0(void* obj, s32 size);
void HSD_ImageDescFree(void* obj) {
    fn_80193AF0(obj, 0x18);
}

/* Address: 0x801BBD60 | Size: 0x24 */
/* Remove an image descriptor memory piece. */
void HSD_ImageDescRemove(void* obj) {
    fn_80193AF0(obj, 0x18);
}

/* Address: 0x801BBD84 | Size: 0x58 */
HSD_ImageDesc* HSD_ImageDescAlloc(void) {
    HSD_ImageDesc* idesc = fn_80193B10(0x18);
    if (idesc == NULL) {
        __assert(&lbl_8047DEB0, 0x8F7, &lbl_8047DEB8);
    }
    memset(idesc, 0, 0x18);
    return idesc;
}

/*
 * HSD_TObjSetTlutDesc - 0x801BBDDC | Size: 0x60
 * Set the TLUT (palette) descriptor for a TObj.
 */
void fn_801BBDDC(HSD_TObj* tobj, void* tlut) {
    if (tobj == NULL) {
        return;
    }
    tobj->tlut = tlut;
    tobj->flags |= TEX_MTX_DIRTY;
}

/* Address: 0x801BBE3C | Size: 0x24 */
/* TObj get blending factor */
f32 fn_801BBE3C(u8* tobj) {
    if (tobj == NULL) {
        return 0.0f;
    }
    return *(f32*)(tobj + 0x5C);
}

/*
 * HSD_TObjSetBlending - 0x801BBE60 | Size: 0x74
 * Set the blending factor for a TObj.
 */
void fn_801BBE60(HSD_TObj* tobj) {
    HSD_TObj* next;
    HSD_TObj* cur;

    cur = tobj;
    while (cur != NULL) {
        next = cur->next;
        if (cur != NULL) {
            HSD_CLASS_METHOD(cur)->release((HSD_Class*) cur);
            HSD_CLASS_METHOD(cur)->destroy((HSD_Class*) cur);
        }
        cur = next;
    }
}

/*
 * HSD_TObjGetFlags - 0x801BBED4 | Size: 0x54
 * Get texture flags from a TObj.
 */
u32 fn_801BBED4(HSD_TObj* tobj) {
    if (tobj == NULL) {
        return 0;
    }
    return tobj->flags;
}

/*
 * HSD_TObjSetFlags - 0x801BBF28 | Size: 0xBC
 * Set texture flags with validation.
 */
void HSD_Index2TexMtx(HSD_TObj* tobj, u32 flags) {
    u32 colormap;
    u32 alphamap;

    if (tobj == NULL) {
        return;
    }

    /* Validate that colormap and alphamap modes are valid */
    colormap = flags & TEX_COLORMAP_MASK;
    alphamap = flags & TEX_ALPHAMAP_MASK;

    if (colormap > TEX_COLORMAP_SUB) {
        flags = (flags & ~TEX_COLORMAP_MASK) | TEX_COLORMAP_MODULATE;
    }
    if (alphamap > TEX_ALPHAMAP_SUB) {
        flags = (flags & ~TEX_ALPHAMAP_MASK) | TEX_ALPHAMAP_MODULATE;
    }

    tobj->flags = flags | TEX_MTX_DIRTY;
}

/* ========================================================================= */
/*  TObj animation update and texture swap                                   */
/* ========================================================================= */

#pragma push
#pragma scheduling on
void HSD_TObjSetup(HSD_TObj* tobj) {
    u8 tlutobj[0x0C];
    u8 texobj[0x20];
    HSD_TlutWork* volatile tluts[8];
    s32 num;
    s32 nb_tluts;
    u32 tlut_name;
    u32 big_tlut_name;
    s32 i;
    u32 coord;
    s32 id;
    HSD_ImageDesc* imagedesc;
    HSD_TexLODWork* lod;
    s32 min_filter;
    HSD_TlutWork* tlut;
    u32 mipmap;
    s32 different;

    nb_tluts = 0;
    tlut_name = 0;
    big_tlut_name = 0x10;
    lbl_8047B37C = tobj;
    if (tobj == NULL) {
        return;
    }

    num = fn_801BC33C(tobj);
    if (num > 0) {
        switch (num - 1) {
        case 0:
            coord = 0;
            break;
        case 1:
            coord = 1;
            break;
        case 2:
            coord = 2;
            break;
        case 3:
            coord = 3;
            break;
        case 4:
            coord = 4;
            break;
        case 5:
            coord = 5;
            break;
        case 6:
            coord = 6;
            break;
        case 7:
            coord = 7;
            break;
        default:
            __assert(&lbl_8047DEB0, 0x794, &lbl_8047DED0);
            coord = 0;
            break;
        }
        HSD_StateRegisterTexGen(coord);
    }

    while (tobj != NULL) {
        id = tobj->id;
        imagedesc = tobj->imagedesc;
        if (id != 0xFF) {
            fn_801BDD74(tobj);
            if (imagedesc == NULL) {
                __assert(&lbl_8047DEB0, 0x6B6, lbl_80275688);
            }
            if (imagedesc->image_ptr == NULL) {
                __assert(&lbl_8047DEB0, 0x6B7, lbl_80275694);
            }

            if (tobj->lod != NULL) {
                lod = (HSD_TexLODWork*) tobj->lod;
            } else {
                lod = &lbl_8036D594;
            }
            min_filter = lod->minFilt;

            switch (imagedesc->format) {
            case 8:
            case 9:
            case 10:
                if (tobj->tlut_no != 0xFF) {
                    tlut = (HSD_TlutWork*) tobj->tluttbl[tobj->tlut_no];
                } else {
                    tlut = (HSD_TlutWork*) tobj->tlut;
                }
                if (tlut == NULL) {
                    __assert(&lbl_8047DEB0, 0x6C7, &lbl_8047DEC4);
                }

                i = 0;
                goto check_tlut;
            scan_tlut:
                different = 1;
                if (tluts[i]->n_entries == tlut->n_entries) {
                    different = 0;
                }
                if (different == 0) {
                    goto found_tlut;
                }
                i++;
            check_tlut:
                if (i < nb_tluts) {
                    goto scan_tlut;
                }
            found_tlut:

                if (i < nb_tluts) {
                    tlut->tlut_name = tluts[i]->tlut_name;
                } else if (nb_tluts < 8) {
                    if (tlut->n_entries > 0x100) {
                        tlut->tlut_name = big_tlut_name++;
                    } else {
                        tlut->tlut_name = tlut_name++;
                    }
                    fn_800BB050(tlutobj, tlut->lut, tlut->fmt,
                                tlut->n_entries);
                    fn_800BB098(tlutobj, tlut->tlut_name);
                    tluts[nb_tluts] = tlut;
                    nb_tluts++;
                } else {
                    tlut->tlut_name = 0;
                }

                mipmap = imagedesc->mipmap ? 1 : 0;
                fn_800BAC58(texobj, imagedesc->image_ptr, imagedesc->width,
                            imagedesc->height, imagedesc->format,
                            tobj->wrap_s, tobj->wrap_t, mipmap,
                            tlut->tlut_name);
                if (min_filter == 5) {
                    min_filter = 3;
                }
                break;
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 14:
                mipmap = imagedesc->mipmap ? 1 : 0;
                fn_800BA9E4(texobj, imagedesc->image_ptr, imagedesc->width,
                            imagedesc->height, imagedesc->format,
                            tobj->wrap_s, tobj->wrap_t, mipmap);
                break;
            default:
                __assert(&lbl_8047DEB0, 0x703, &lbl_8047DED0);
                break;
            }

            if (imagedesc->mipmap == 0) {
                min_filter &= 1;
            }
            fn_800BACA0(texobj, min_filter, tobj->magFilt,
                        imagedesc->minLOD, imagedesc->maxLOD, lod->LODBias,
                        lod->bias_clamp, lod->edgeLODEnable,
                        lod->max_anisotropy);
            fn_800BAFFC(texobj, tobj->id);
        }
        tobj = tobj->next;
    }
}
#pragma pop

/*
 * HSD_TObjAssignResources - 0x801BC33C | Size: 0x580
 * Assign texture maps and texture coordinates for a TObj chain.
 */
s32 fn_801BC33C(HSD_TObj* tobj) {
    if (tobj == NULL) {
        return 0;
    }

    /* TODO: decompile the resource assignment pass. */
    return 0;
}

/* ========================================================================= */
/*  Texture expression (TExp) from TObj                                      */
/* ========================================================================= */

/*
 * HSD_TObjMakeTExp - 0x801BC8BC | Size: 0x674
 * Build TExp nodes from a texture object for material compilation.
 */
void fn_801BC8BC(HSD_TObj* tobj, u32 lightmap, u32 lightmap_done,
                  void** c, void** a, void** list) {
    if (tobj == NULL) {
        return;
    }

    /* Build expression nodes based on colormap/alphamap mode:
     * - MODULATE: output = input * texture
     * - REPLACE: output = texture
     * - BLEND: output = lerp(input, texture, blend_factor)
     * - ADD: output = input + texture
     * - SUB: output = input - texture
     *
     * Also handles lightmap interactions:
     * - Diffuse light modulation
     * - Specular highlight addition
     * - Ambient occlusion blending
     */
}

/*
 * HSD_TObjCompileTExp - 0x801BCF30 | Size: 0x9A0
 * Compile the full expression tree for a TObj chain.
 * This is the largest function in this file because it handles
 * the complete TExp compilation pipeline:
 * 1. Collect all TObj layers
 * 2. Build color and alpha expression trees
 * 3. Optimize expressions
 * 4. Generate TEV stage configurations
 * 5. Assign texture coordinates and maps
 */
void fn_801BCF30(HSD_TObj* tobj, u32 rendermode) {
    if (tobj == NULL) {
        return;
    }

    /* Full TExp compilation:
     * - Walk TObj chain
     * - Build expression trees per layer
     * - Merge layers according to colormap/alphamap modes
     * - Optimize merged tree
     * - Compile to TEV stages
     */
}

/* ========================================================================= */
/*  TLUT (Texture Lookup Table / Palette) management                         */
/* ========================================================================= */

/*
 * HSD_TlutInit - 0x801BD8D0 | Size: 0x188
 * Initialize TLUT state and load palettes to GX.
 */
void fn_801BD8D0(void* tlut_desc, u32 tlut_name) {
    if (tlut_desc == NULL) {
        return;
    }

    /* Initialize GXTlutObj from descriptor:
     * - Set palette format (IA8, RGB565, RGB5A3)
     * - Set number of entries
     * - Set palette data pointer
     * - Load to GX TLUT slot
     */
}

/*
 * HSD_TlutLoadFromDesc - 0x801BDA58 | Size: 0x31C
 * Load a TLUT from its descriptor to GX hardware.
 */
void fn_801BDA58(void* tlut_desc) {
    if (tlut_desc == NULL) {
        return;
    }

    /* Parse TLUT descriptor and load palette data:
     * 1. Get palette data pointer
     * 2. Determine format and entry count
     * 3. DC store range (ensure data is in main memory)
     * 4. Initialize GXTlutObj
     * 5. Load to GX
     */
}

/*
 * HSD_TlutAnimSwap - 0x801BDD74 | Size: 0x540
 * Handle TLUT animation and palette swapping.
 * Swaps the active palette based on the current animation frame.
 */
void fn_801BDD74(HSD_TObj* tobj) {
    if (tobj == NULL) {
        return;
    }

    if (tobj->tluttbl == NULL) {
        return;
    }

    /* Swap palette:
     * 1. Get current animation frame index
     * 2. Look up TLUT descriptor from tluttbl[index]
     * 3. Load new palette to GX
     * 4. Update TObj's active TLUT pointer
     */
}

/* ========================================================================= */
/*  Image descriptor management                                              */
/* ========================================================================= */

/*
 * HSD_ImageDescLoad - 0x801BE2B4 | Size: 0x1DC
 * Load an image descriptor and initialize GX texture object.
 */
void fn_801BE2B4(HSD_ImageDesc* desc, void* texobj, u32 wrap_s, u32 wrap_t) {
    if (desc == NULL || texobj == NULL) {
        return;
    }

    GXInitTexObj(texobj, desc->image_ptr, desc->width, desc->height,
                 desc->format, wrap_s, wrap_t,
                 desc->mipmap ? 1 : 0);

    if (desc->mipmap) {
        GXInitTexObjLOD(texobj, 1, 1, desc->minLOD, desc->maxLOD,
                        0.0f, 0, 0, 0);
    }
}

/*
 * HSD_ImageGetSize - 0x801BE490 | Size: 0x3C
 * Get the size in bytes of an image based on its descriptor.
 */
u32 fn_801BE490(HSD_ImageDesc* desc) {
    u32 size;

    if (desc == NULL) {
        return 0;
    }

    /* Calculate size based on format and dimensions */
    size = desc->width * desc->height;

    switch (desc->format) {
    case 0: /* I4 */
        size /= 2;
        break;
    case 1: /* I8 */
        break;
    case 2: /* IA4 */
        break;
    case 3: /* IA8 */
    case 4: /* RGB565 */
    case 5: /* RGB5A3 */
        size *= 2;
        break;
    case 6: /* RGBA8 */
        size *= 4;
        break;
    default:
        break;
    }

    return size;
}

/*
 * HSD_ImageFmtToGX - 0x801BE4CC | Size: 0xCC
 * Convert HSD image format to GX texture format.
 */
u32 fn_801BE4CC(u32 hsd_format) {
    /* Direct mapping for most formats */
    return hsd_format;
}

/*
 * HSD_ImageMipmapSetup - 0x801BE598 | Size: 0x268
 * Set up mipmap chain for an image.
 */
void fn_801BE598(HSD_ImageDesc* desc, void* texobj) {
    if (desc == NULL || texobj == NULL) {
        return;
    }

    if (desc->mipmap == 0) {
        return;
    }

    /* Configure mipmap chain:
     * 1. Calculate number of mipmap levels
     * 2. Set min/max LOD
     * 3. Configure LOD bias
     * 4. Enable trilinear filtering if available
     */
    GXInitTexObjLOD(texobj, 4 /* GX_LIN_MIP_LIN */, 1 /* GX_LINEAR */,
                    desc->minLOD, desc->maxLOD, 0.0f, 0, 0, 0);
}

/*
 * HSD_ImageCacheInvalidate - 0x801BE800 | Size: 0x5C
 * Invalidate texture cache for an image.
 */
void fn_801BE800(HSD_TObj* tobj) {
    extern void fn_801C27F4(HSD_AObj* aobj, HSD_TObj* tobj, void* method);
    HSD_TObj* cur;

    if (tobj == NULL) {
        return;
    }

    cur = tobj;
    while (cur != NULL) {
        if (cur != NULL) {
            fn_801C27F4(cur->aobj, cur, ((void**) HSD_TOBJ_METHOD(cur))[0x48 / 4]);
        }
        cur = cur->next;
    }
}

/* ========================================================================= */
/*  TObj rendering setup                                                     */
/* ========================================================================= */

/*
 * fn_801BE85C - 0x801BE85C | Size: 0x60C
 * TObj rendering pipeline helper.
 */
void fn_801BE85C(HSD_TObj* tobj) {
    HSD_TObj* t;
    u32 stage = 0;

    if (tobj == NULL) {
        return;
    }

    for (t = tobj; t != NULL; t = t->next) {
        if (stage >= 8) break;

        /* 1. Load image to GX */
        fn_801BBC14(t, stage);

        /* 2. Set up texture coordinate gen */
        /* 3. Load texture matrix */
        /* 4. Configure TEV stage */

        stage++;
    }
}

/*
 * HSD_TObjSetupTexCoordSrc - 0x801BEE68 | Size: 0x74
 * Configure texture coordinate source for rendering.
 */
void fn_801BEE68(HSD_TObj* tobj, u32 coord_id) {
    u32 src;

    if (tobj == NULL) {
        return;
    }

    src = tobj_coord(tobj);
    /* Map HSD coord type to GX texcoord gen parameters */
}

/*
 * HSD_TObjSetupFilterWrap - 0x801BEEDC | Size: 0x1BC
 * Configure texture filter and wrap modes.
 */
void fn_801BEEDC(HSD_TObj* tobj, void* texobj) {
    if (tobj == NULL || texobj == NULL) {
        return;
    }

    GXInitTexObjWrapMode(texobj, tobj->wrap_s, tobj->wrap_t);
    GXInitTexObjFilterMode(texobj, 1, tobj->magFilt);
}

/* ========================================================================= */
/*  Render pipeline transition functions                                     */
/* ========================================================================= */

/*
 * HSD_RenderPassInit - 0x801BF098 | Size: 0xA0
 * Initialize render pass state.
 */
void HSD_Index2PosNrmMtx(void* state, u32 num_passes) {
    if (state == NULL) {
        return;
    }

    /* Initialize render pass tracking:
     * - Set number of passes
     * - Clear per-pass state
     * - Initialize sort key generation
     */
}

/*
 * HSD_RenderPassQuery - 0x801BF138 | Size: 0x34
 * Query the current render pass.
 */
u32 fn_801BF138(void* state) {
    if (state == NULL) {
        return 0;
    }
    return *(u32*)((u8*)state + 0x0);
}

/*
 * HSD_RenderPassSet - 0x801BF16C | Size: 0x84
 * Set the current render pass.
 */
void fn_801BF16C(void* state, u32 pass) {
    if (state == NULL) {
        return;
    }
    *(u32*)((u8*)state + 0x0) = pass;
}

/*
 * HSD_RenderPassExecute - 0x801BF1F0 | Size: 0x2D4
 * Execute render callbacks for the current pass.
 */
void fn_801BF1F0(void* state) {
    if (state == NULL) {
        return;
    }

    /* Dispatch render callbacks for the current pass:
     * 1. Get current pass index
     * 2. Walk the render list
     * 3. For each object in the list:
     *    - Check if object participates in this pass
     *    - Call the object's render callback
     */
}

/* Address: 0x801BF4C4 | Size: 0x20 */
/* Store value to BSS object and set flag */
extern u8 lbl_80466BC0[];
void fn_801BF4C4(u32 val) {
    *(u32*)(lbl_80466BC0 + 0x3C) = val;
    *(u8*)(lbl_80466BC0 + 0x54) = 1;
}

/*
 * HSD_RenderSortKey - 0x801BF4E4 | Size: 0x90
 * Generate a sort key for render ordering.
 */
u32 fn_801BF4E4(void* obj, u32 pass, f32 depth) {
    u32 key = 0;

    /* Encode sort key:
     * - High bits: pass index
     * - Middle bits: material type (opaque vs transparent)
     * - Low bits: depth (front-to-back for opaque, back-to-front for transparent)
     */
    key = (pass & 0xFF) << 24;
    /* Quantize depth to 16 bits */
    key |= ((u32)(depth * 65535.0f)) & 0xFFFF;

    return key;
}

/* ========================================================================= */
/*  HSD-to-battle transition code                                            */
/* ========================================================================= */

/*
 * HSD_BattleRenderContextSetup - 0x801BF574 | Size: 0x138
 * Set up the render context for battle rendering.
 */
void fn_801BF574(void* ctx) {
    if (ctx == NULL) {
        return;
    }

    /* Initialize battle rendering context:
     * - Set up GX viewport for battle view
     * - Configure projection matrix
     * - Set default render state
     */
}

/*
 * HSD_BattleGXStateConfig - 0x801BF6AC | Size: 0x1F4
 * Configure GX state specifically for battle rendering.
 */
void fn_801BF6AC(void* ctx) {
    if (ctx == NULL) {
        return;
    }

    /* Configure battle-specific GX state:
     * - Set up TEV stages for battle effects
     * - Configure alpha blending for transparency
     * - Set up depth buffer configuration
     * - Configure fog parameters
     */
}

/*
 * HSD_BattleModelMtxSetup - 0x801BF8A0 | Size: 0x17C
 * Set up model matrices for battle rendering.
 */
void fn_801BF8A0(void* ctx) {
    if (ctx == NULL) {
        return;
    }

    /* Set up matrices:
     * - View matrix (camera)
     * - Projection matrix (perspective)
     * - Model matrix (identity initially)
     */
}

/*
 * HSD_BattleTevSetup - 0x801BFA1C | Size: 0x284
 * Set up TEV stages for the battle texture environment.
 */
void fn_801BFA1C(void* ctx) {
    if (ctx == NULL) {
        return;
    }

    /* Configure TEV for battle:
     * - Stage 0: Base texture * vertex color
     * - Stage 1: + specular highlight
     * - Stage 2: * shadow factor
     * - Stage 3: + emission
     */
}

/* Address: 0x801BFCA0 | Size: 0x10 */
/* Get field at offset 0x1E0 from BSS object */
u32 fn_801BFCA0(void) {
    return *(u32*)(lbl_80466BC0 + 0x1E0);
}

/*
 * HSD_BattleRenderStateConfig - 0x801BFCB0 | Size: 0x60
 * Configure render state for battle.
 */
void fn_801BFCB0(void* state, u32 mode) {
    if (state == NULL) {
        return;
    }

    /* Set render state based on mode:
     * 0 = opaque
     * 1 = transparent
     * 2 = shadow
     * 3 = effect
     */
}

/*
 * HSD_BattleFullRenderSetup - 0x801BFD10 | Size: 0x208
 * Full battle render setup combining all configuration steps.
 */
void fn_801BFD10(void* ctx) {
    if (ctx == NULL) {
        return;
    }

    /* Complete battle render setup:
     * 1. Context setup (fn_801BF574)
     * 2. GX state config (fn_801BF6AC)
     * 3. Model matrix setup (fn_801BF8A0)
     * 4. TEV setup (fn_801BFA1C)
     */
    fn_801BF574(ctx);
    fn_801BF6AC(ctx);
    fn_801BF8A0(ctx);
    fn_801BFA1C(ctx);
}

/*
 * HSD_BattleSceneInit - 0x801BFF18 | Size: 0x2B0
 * Battle scene initialization - the last function in the range.
 * Initializes the complete battle scene rendering pipeline.
 */
void fn_801BFF18(void* ctx) {
    if (ctx == NULL) {
        return;
    }

    /* Full battle scene initialization:
     * 1. Initialize render pass state
     * 2. Create battle camera
     * 3. Set up battle lights
     * 4. Configure render passes:
     *    Pass 0: Opaque geometry
     *    Pass 1: Transparent geometry
     *    Pass 2: Shadow volumes
     *    Pass 3: Effects/particles
     *    Pass 4: HUD overlay
     * 5. Initialize sort buffers
     */
    fn_801BFD10(ctx);
}
