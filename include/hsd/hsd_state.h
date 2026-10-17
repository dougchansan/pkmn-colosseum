/**
 * @file hsd_state.h
 * @brief HSD render state cache (state.c).
 *
 * Colosseum address range: 0x801B25C4 - 0x801B3168.
 * Adapted from the Melee decompilation (doldecomp/melee, sysdolphin/baselib/
 * state.h).
 *
 * state.c is not linked as one unit yet (HSD_StateSetZMode, HSD_StateSetZMode, is
 * not exact), so its exact ranges are built as separate units and the state
 * words, material state and invalidate table stay in their data units under
 * their address names.
 */
#ifndef HSD_STATE_H
#define HSD_STATE_H

#include "dolphin/types.h"
#include "dolphin/gx/GX.h"

#define HSD_STATE_NONE 0
#define HSD_STATE_PRIMITIVE 0x1
#define HSD_STATE_VTX_ATTR 0x2
#define HSD_STATE_COLOR_CHANNEL 0x4
#define HSD_STATE_TEV_STAGE 0x8
#define HSD_STATE_TEV_REGISTER 0x10
#define HSD_STATE_TEX_COORD_GEN 0x20
#define HSD_STATE_RENDER_MODE 0x40

typedef struct HSD_StateInvalidateEntry {
    int mask;
    void (*func)(void);
} HSD_StateInvalidateEntry;

typedef struct HSD_MaterialState {
    GXColor ambient;
    GXColor diffuse;
    GXColor specular;
    u8 alpha;
    f32 shininess;
} HSD_MaterialState;

/* state.c data, by address */
extern u8 lbl_8047B318;  /* state_dither */
extern u8 lbl_8047B319;  /* state_before_tex */
extern u8 lbl_8047B31A;  /* state_dst_alpha */
extern u8 lbl_8047B31B;  /* state_enable_dst_alpha */
extern u8 lbl_8047B31C;  /* state_alpha_update */
extern u8 lbl_8047B31D;  /* state_color_update */
extern u8 lbl_8047B31E;  /* state_alpha_ref1 */
extern int lbl_8047B320; /* state_alpha_comp1 */
extern int lbl_8047B324; /* state_alpha_op */
extern u8 lbl_8047B328;  /* state_alpha_ref0 */
extern int lbl_8047B32C; /* state_alpha_comp0 */
extern u8 lbl_8047B330;  /* state_z_update */
extern int lbl_8047B334; /* state_z_func */
extern u8 lbl_8047B338;  /* state_z_enable */
extern int lbl_8047B33C; /* state_logic_op */
extern int lbl_8047B340; /* state_dst_factor */
extern int lbl_8047B344; /* state_src_factor */
extern int lbl_8047B348; /* state_blend_type */
extern int lbl_8047B34C; /* state_cull_mode */
extern u8 lbl_8047B350;  /* state_point_size */
extern u8 lbl_8047B351;  /* state_line_width */
extern HSD_MaterialState lbl_80465710;           /* matstate */
extern HSD_StateInvalidateEntry lbl_8036CFA8[];  /* invalidate_funcs */

/* Functions still carrying address names in symbols.txt:
 *   HSD_StateInvalidate HSD_StateInvalidate
 *   _HSD_StateInvalidateRenderMode _HSD_StateInvalidateRenderMode
 *   _HSD_StateInvalidateVtxAttr _HSD_StateInvalidateVtxAttr
 *   _HSD_StateInvalidatePrimitive _HSD_StateInvalidatePrimitive
 *   HSD_StateSetAlphaUpdate HSD_StateSetAlphaUpdate  HSD_StateSetColorUpdate HSD_StateSetColorUpdate
 *   HSD_StateSetZMode HSD_StateSetZMode        HSD_StateSetCullMode HSD_StateSetCullMode
 *   HSD_SetMaterialShininess HSD_SetMaterialShininess fn_801B28C8 HSD_SetMaterialColor
 *   fn_801B294C HSD_SetupRenderModeWithCustomPE
 *   fn_801B29E4 HSD_SetupPEMode          fn_801B2F1C HSD_SetupChannelMode
 */
void HSD_StateInvalidate(int mask);
void _HSD_StateInvalidateRenderMode(void);
void _HSD_StateInvalidateVtxAttr(void);
void _HSD_StateInvalidatePrimitive(void);
void HSD_StateSetAlphaUpdate(int enable);
void HSD_StateSetColorUpdate(int enable);
void HSD_StateSetZMode(int enable, int func, int update);
void HSD_StateSetCullMode(int mode);
void HSD_SetMaterialShininess(f32 shininess);
void HSD_SetMaterialColor(GXColor ambient, GXColor diffuse, GXColor specular,
                 f32 alpha);

#endif /* HSD_STATE_H */
