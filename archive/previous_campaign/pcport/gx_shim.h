/**
 * @file gx_shim.h
 * @brief GX API shim -- declares all ~35 GX functions used by Pokemon Colosseum.
 *
 * This header provides the GCN GX SDK function prototypes that the game
 * code (GSgfx, GStexture, HSD, effect system) calls. On the GameCube these
 * are provided by the Dolphin SDK; for the PC port they are implemented in
 * gx_shim.c as an OpenGL 3.3 translation layer.
 *
 * Function signatures match the official GCN SDK (GX library rev 1.3).
 *
 * References:
 *   - docs/pc_port_design.md Section 1 (GX API Surface Analysis)
 *   - docs/pc_port_design.md Section 8 (API Mapping)
 *   - include/game/gs_render.h, include/game/gs_gfx.h
 *   - include/game/gs_texture.h
 *   - include/hsd/hsd_mobj.h, hsd_tobj.h, hsd_pobj.h, hsd_lobj.h, hsd_cobj.h
 *
 * Phase 3 PC port scaffolding -- skeleton only; implementations are stubs.
 */
#ifndef PCPORT_GX_SHIM_H
#define PCPORT_GX_SHIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Basic GX types (matching GCN SDK)
 * ========================================================================= */

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;   /* match include/dolphin/types.h (long, not int) so */
typedef signed char    s8;    /* strict compilers don't see a typedef redefinition */
typedef signed short   s16;
typedef signed long    s32;
typedef float          f32;
typedef int            BOOL;

/* GXColor: 4 bytes packed RGBA */
typedef struct {
    u8 r, g, b, a;
} GXColor;

/* GXTexObj: opaque, 32 bytes on GCN */
typedef struct {
    u8 _pad[0x20];
} GXTexObj;

/* GXTlutObj: opaque, 12 bytes on GCN */
typedef struct {
    u8 _pad[0x0C];
} GXTlutObj;

/* GXLightObj: opaque, 64 bytes on GCN */
typedef struct {
    u8 _pad[0x40];
} GXLightObj;

/* Mtx: 3x4 row-major matrix (standard GCN MTX type) */
typedef f32 Mtx[3][4];

/* Mtx44: 4x4 row-major matrix */
typedef f32 Mtx44[4][4];

/* =========================================================================
 * GX enumerations (only the values actually used by Colosseum)
 * ========================================================================= */

/* GXTexFmt */
typedef enum {
    GX_TF_I4     = 0x0,
    GX_TF_I8     = 0x1,
    GX_TF_IA4    = 0x2,
    GX_TF_IA8    = 0x3,
    GX_TF_RGB565 = 0x4,
    GX_TF_RGB5A3 = 0x5,
    GX_TF_RGBA8  = 0x6,
    GX_TF_C4     = 0x8,
    GX_TF_C8     = 0x9,
    GX_TF_C14X2  = 0xA,
    GX_TF_CMPR   = 0xE
} GXTexFmt;

/* GXTlutFmt */
typedef enum {
    GX_TL_IA8    = 0x0,
    GX_TL_RGB565 = 0x1,
    GX_TL_RGB5A3 = 0x2
} GXTlutFmt;

/* GXTexWrapMode */
typedef enum {
    GX_CLAMP  = 0,
    GX_REPEAT = 1,
    GX_MIRROR = 2
} GXTexWrapMode;

/* GXTexFilter */
typedef enum {
    GX_NEAR          = 0,
    GX_LINEAR        = 1,
    GX_NEAR_MIP_NEAR = 2,
    GX_LIN_MIP_NEAR  = 3,
    GX_NEAR_MIP_LIN  = 4,
    GX_LIN_MIP_LIN   = 5
} GXTexFilter;

/* GXTexMapID */
typedef enum {
    GX_TEXMAP0 = 0,
    GX_TEXMAP1,
    GX_TEXMAP2,
    GX_TEXMAP3,
    GX_TEXMAP4,
    GX_TEXMAP5,
    GX_TEXMAP6,
    GX_TEXMAP7,
    GX_TEXMAP_NULL = 0xFF
} GXTexMapID;

/* GXPrimitive */
typedef enum {
    GX_QUADS         = 0x80,
    GX_TRIANGLES     = 0x90,
    GX_TRIANGLESTRIP = 0x98,
    GX_TRIANGLEFAN   = 0xA0,
    GX_LINES         = 0xA8,
    GX_LINESTRIP     = 0xB0,
    GX_POINTS        = 0xB8
} GXPrimitive;

/* GXVtxFmt */
typedef enum {
    GX_VTXFMT0 = 0,
    GX_VTXFMT1,
    GX_VTXFMT2,
    GX_VTXFMT3,
    GX_VTXFMT4,
    GX_VTXFMT5,
    GX_VTXFMT6,
    GX_VTXFMT7
} GXVtxFmt;

/* GXAttr */
typedef enum {
    GX_VA_PNMTXIDX = 0,
    GX_VA_TEX0MTXIDX = 1,
    GX_VA_TEX1MTXIDX = 2,
    GX_VA_TEX2MTXIDX = 3,
    GX_VA_TEX3MTXIDX = 4,
    GX_VA_TEX4MTXIDX = 5,
    GX_VA_TEX5MTXIDX = 6,
    GX_VA_TEX6MTXIDX = 7,
    GX_VA_TEX7MTXIDX = 8,
    GX_VA_POS = 9,
    GX_VA_NRM = 10,
    GX_VA_CLR0 = 11,
    GX_VA_CLR1 = 12,
    GX_VA_TEX0 = 13,
    GX_VA_TEX1 = 14,
    GX_VA_TEX2 = 15,
    GX_VA_TEX3 = 16,
    GX_VA_TEX4 = 17,
    GX_VA_TEX5 = 18,
    GX_VA_TEX6 = 19,
    GX_VA_TEX7 = 20,
    GX_VA_NULL = 0xFF
} GXAttr;

/* GXAttrType */
typedef enum {
    GX_NONE = 0,
    GX_DIRECT = 1,
    GX_INDEX8 = 2,
    GX_INDEX16 = 3
} GXAttrType;

/* GXCompCnt */
typedef enum {
    GX_POS_XY = 0,
    GX_POS_XYZ = 1,
    GX_NRM_XYZ = 0,
    GX_CLR_RGB = 0,
    GX_CLR_RGBA = 1,
    GX_TEX_S = 0,
    GX_TEX_ST = 1
} GXCompCnt;

/* GXCompType */
typedef enum {
    GX_U8 = 0,
    GX_S8 = 1,
    GX_U16 = 2,
    GX_S16 = 3,
    GX_F32 = 4,
    GX_RGB8 = 1,
    GX_RGBA8 = 5
} GXCompType;

/* GXCullMode */
typedef enum {
    GX_CULL_NONE  = 0,
    GX_CULL_FRONT = 1,
    GX_CULL_BACK  = 2,
    GX_CULL_ALL   = 3
} GXCullMode;

/* GXBlendMode */
typedef enum {
    GX_BM_NONE     = 0,
    GX_BM_BLEND    = 1,
    GX_BM_LOGIC    = 2,
    GX_BM_SUBTRACT = 3,
    GX_MAX_BLENDMODE
} GXBlendMode;

/* GXBlendFactor */
typedef enum {
    GX_BL_ZERO        = 0,
    GX_BL_ONE         = 1,
    GX_BL_SRCCLR      = 2,
    GX_BL_INVSRCCLR   = 3,
    GX_BL_SRCALPHA    = 4,
    GX_BL_INVSRCALPHA = 5,
    GX_BL_DSTALPHA    = 6,
    GX_BL_INVDSTALPHA = 7
} GXBlendFactor;

/* GXLogicOp */
typedef enum {
    GX_LO_CLEAR   = 0,
    GX_LO_AND     = 1,
    GX_LO_REVAND  = 2,
    GX_LO_COPY    = 3,
    GX_LO_INVAND  = 4,
    GX_LO_NOOP    = 5,
    GX_LO_XOR     = 6,
    GX_LO_OR      = 7,
    GX_LO_NOR     = 8,
    GX_LO_EQUIV   = 9,
    GX_LO_INV     = 10,
    GX_LO_REVOR   = 11,
    GX_LO_INVCOPY = 12,
    GX_LO_INVOR   = 13,
    GX_LO_NAND    = 14,
    GX_LO_SET     = 15
} GXLogicOp;

/* GXCompare */
typedef enum {
    GX_NEVER   = 0,
    GX_LESS    = 1,
    GX_EQUAL   = 2,
    GX_LEQUAL  = 3,
    GX_GREATER = 4,
    GX_NEQUAL  = 5,
    GX_GEQUAL  = 6,
    GX_ALWAYS  = 7
} GXCompare;

/* GXAlphaOp */
typedef enum {
    GX_AOP_AND  = 0,
    GX_AOP_OR   = 1,
    GX_AOP_XOR  = 2,
    GX_AOP_XNOR = 3
} GXAlphaOp;

/* GXTevStageID */
typedef enum {
    GX_TEVSTAGE0  = 0,
    GX_TEVSTAGE1,
    GX_TEVSTAGE2,
    GX_TEVSTAGE3,
    GX_TEVSTAGE4,
    GX_TEVSTAGE5,
    GX_TEVSTAGE6,
    GX_TEVSTAGE7,
    GX_TEVSTAGE8,
    GX_TEVSTAGE9,
    GX_TEVSTAGE10,
    GX_TEVSTAGE11,
    GX_TEVSTAGE12,
    GX_TEVSTAGE13,
    GX_TEVSTAGE14,
    GX_TEVSTAGE15,
    GX_MAX_TEVSTAGE
} GXTevStageID;

/* GXTevMode (shorthand presets for GXSetTevOp) */
typedef enum {
    GX_MODULATE  = 0,
    GX_DECAL     = 1,
    GX_BLEND     = 2,
    GX_REPLACE   = 3,
    GX_PASSCLR   = 4
} GXTevMode;

/* GXTevColorArg */
typedef enum {
    GX_CC_CPREV  = 0,
    GX_CC_APREV  = 1,
    GX_CC_C0     = 2,
    GX_CC_A0     = 3,
    GX_CC_C1     = 4,
    GX_CC_A1     = 5,
    GX_CC_C2     = 6,
    GX_CC_A2     = 7,
    GX_CC_TEXC   = 8,
    GX_CC_TEXA   = 9,
    GX_CC_RASC   = 10,
    GX_CC_RASA   = 11,
    GX_CC_ONE    = 12,
    GX_CC_HALF   = 13,
    GX_CC_KONST  = 14,
    GX_CC_ZERO   = 15
} GXTevColorArg;

/* GXTevAlphaArg */
typedef enum {
    GX_CA_APREV  = 0,
    GX_CA_A0     = 1,
    GX_CA_A1     = 2,
    GX_CA_A2     = 3,
    GX_CA_TEXA   = 4,
    GX_CA_RASA   = 5,
    GX_CA_KONST  = 6,
    GX_CA_ZERO   = 7
} GXTevAlphaArg;

/* GXTevOp */
typedef enum {
    GX_TEV_ADD           = 0,
    GX_TEV_SUB           = 1,
    GX_TEV_COMP_R8_GT    = 8,
    GX_TEV_COMP_R8_EQ    = 9,
    GX_TEV_COMP_GR16_GT  = 10,
    GX_TEV_COMP_GR16_EQ  = 11,
    GX_TEV_COMP_BGR24_GT = 12,
    GX_TEV_COMP_BGR24_EQ = 13,
    GX_TEV_COMP_RGB8_GT  = 14,
    GX_TEV_COMP_RGB8_EQ  = 15
} GXTevOp;

/* GXTevScale */
typedef enum {
    GX_CS_SCALE_1  = 0,
    GX_CS_SCALE_2  = 1,
    GX_CS_SCALE_4  = 2,
    GX_CS_DIVIDE_2 = 3
} GXTevScale;

/* GXTevBias */
typedef enum {
    GX_TB_ZERO      = 0,
    GX_TB_ADDHALF   = 1,
    GX_TB_SUBHALF   = 2
} GXTevBias;

/* GXTevRegID */
typedef enum {
    GX_TEVPREV = 0,
    GX_TEVREG0 = 1,
    GX_TEVREG1 = 2,
    GX_TEVREG2 = 3
} GXTevRegID;

/* GXChannelID */
typedef enum {
    GX_COLOR0     = 0,
    GX_COLOR1     = 1,
    GX_ALPHA0     = 2,
    GX_ALPHA1     = 3,
    GX_COLOR0A0   = 4,
    GX_COLOR1A1   = 5,
    GX_COLOR_ZERO = 6,
    GX_ALPHA_BUMP = 7,
    GX_ALPHA_BUMPN = 8,
    GX_COLOR_NULL = 0xFF
} GXChannelID;

/* GXColorSrc */
typedef enum {
    GX_SRC_REG = 0,
    GX_SRC_VTX = 1
} GXColorSrc;

/* GXLightID (bitmask) */
typedef enum {
    GX_LIGHT_NULL = 0,
    GX_LIGHT0     = 0x001,
    GX_LIGHT1     = 0x002,
    GX_LIGHT2     = 0x004,
    GX_LIGHT3     = 0x008,
    GX_LIGHT4     = 0x010,
    GX_LIGHT5     = 0x020,
    GX_LIGHT6     = 0x040,
    GX_LIGHT7     = 0x080
} GXLightID;

/* GXDiffuseFn */
typedef enum {
    GX_DF_NONE  = 0,
    GX_DF_SIGN  = 1,
    GX_DF_CLAMP = 2
} GXDiffuseFn;

/* GXAttnFn */
typedef enum {
    GX_AF_SPEC = 0,
    GX_AF_SPOT = 1,
    GX_AF_NONE = 2
} GXAttnFn;

/* GXFogType */
typedef enum {
    GX_FOG_NONE      = 0x00,
    GX_FOG_PERSP_LIN = 0x02,
    GX_FOG_PERSP_EXP = 0x04,
    GX_FOG_PERSP_EXP2 = 0x05,
    GX_FOG_PERSP_REVEXP = 0x06,
    GX_FOG_PERSP_REVEXP2 = 0x07,
    GX_FOG_ORTHO_LIN = 0x0A,
    GX_FOG_ORTHO_EXP = 0x0C,
    GX_FOG_ORTHO_EXP2 = 0x0D,
    GX_FOG_ORTHO_REVEXP = 0x0E,
    GX_FOG_ORTHO_REVEXP2 = 0x0F,
    GX_FOG_LIN = GX_FOG_PERSP_LIN,
    GX_FOG_EXP = GX_FOG_PERSP_EXP,
    GX_FOG_EXP2 = GX_FOG_PERSP_EXP2
} GXFogType;

/* GXProjectionType */
typedef enum {
    GX_PERSPECTIVE  = 0,
    GX_ORTHOGRAPHIC = 1
} GXProjectionType;

/* GXTexCoordID */
typedef enum {
    GX_TEXCOORD0 = 0,
    GX_TEXCOORD1,
    GX_TEXCOORD2,
    GX_TEXCOORD3,
    GX_TEXCOORD4,
    GX_TEXCOORD5,
    GX_TEXCOORD6,
    GX_TEXCOORD7,
    GX_TEXCOORD_NULL = 0xFF
} GXTexCoordID;

/* GXTexGenType -- source for texcoord generation */
typedef enum {
    GX_TG_MTX3x4 = 0,
    GX_TG_MTX2x4 = 1,
    GX_TG_BUMP0  = 2,
    GX_TG_SRTG   = 10
} GXTexGenType;

/* GXTexGenSrc -- input to texcoord generator */
typedef enum {
    GX_TG_POS   = 0,
    GX_TG_NRM   = 1,
    GX_TG_BINRM = 2,
    GX_TG_TANGENT = 3,
    GX_TG_TEX0  = 4,
    GX_TG_TEX1  = 5,
    GX_TG_TEX2  = 6,
    GX_TG_TEX3  = 7,
    GX_TG_TEX4  = 8,
    GX_TG_TEX5  = 9,
    GX_TG_TEX6  = 10,
    GX_TG_TEX7  = 11,
    GX_TG_TEXCOORD0 = 12,
    GX_TG_TEXCOORD1 = 13,
    GX_TG_TEXCOORD2 = 14,
    GX_TG_TEXCOORD3 = 15,
    GX_TG_TEXCOORD4 = 16,
    GX_TG_TEXCOORD5 = 17,
    GX_TG_TEXCOORD6 = 18,
    GX_TG_COLOR0 = 19,
    GX_TG_COLOR1 = 20
} GXTexGenSrc;

/* GXTexMtx -- texture matrix slot */
typedef enum {
    GX_TEXMTX0  = 30,
    GX_TEXMTX1  = 33,
    GX_TEXMTX2  = 36,
    GX_TEXMTX3  = 39,
    GX_TEXMTX4  = 42,
    GX_TEXMTX5  = 45,
    GX_TEXMTX6  = 48,
    GX_TEXMTX7  = 51,
    GX_IDENTITY = 60
} GXTexMtx;

/* GXGamma */
typedef enum {
    GX_GM_1_0 = 0,
    GX_GM_1_7 = 1,
    GX_GM_2_2 = 2
} GXGamma;

/* GXPixelFmt */
typedef enum {
    GX_PF_RGB8_Z24   = 0,
    GX_PF_RGBA6_Z24  = 1,
    GX_PF_RGB565_Z16 = 2,
    GX_PF_Z24        = 3,
    GX_PF_Y8         = 4,
    GX_PF_U8         = 5,
    GX_PF_V8         = 6,
    GX_PF_YUV420     = 7
} GXPixelFmt;

/* GXBool */
#define GX_TRUE  1
#define GX_FALSE 0
typedef u8 GXBool;

/* Callback type for GXSetDrawDoneCallback */
typedef void (*GXDrawDoneCallback)(void);

/* =========================================================================
 * GX Function Declarations -- ~35 functions used by Colosseum
 *
 * Organized by subsystem, matching the analysis in pc_port_design.md S1.
 * ========================================================================= */

/* --- 1. Initialization and FIFO (Section 1.1) --- */

/**
 * GXInit -- Initialize the GX graphics system.
 * Called from GSgfxInit (gs_gfx.c, fn_801C021C).
 * PC port: Initialize OpenGL 3.3 context via GLFW + GLAD.
 */
void GXInit(void* base, u32 size);

/**
 * GXSetDrawDoneCallback -- Register a callback for draw completion.
 * Called from GSgfxInit (fn_801C01C8).
 * PC port: No-op (OpenGL is synchronous).
 */
GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback cb);

/**
 * GXSetDispCopyGamma -- Set gamma correction for display copy.
 * Called from GSgfxInit (fn_801BF4C4) with gamma=GX_GM_1_0.
 * PC port: Handled by sRGB framebuffer or window system gamma.
 */
void GXSetDispCopyGamma(GXGamma gamma);

/* --- 2. Viewport, Scissor, Projection (Section 1.2) --- */

/**
 * GXSetViewport -- Set the viewport rectangle and depth range.
 * Called from GSgfx_PreRetraceCallback, GSgfx_BeginFrame (fn_800AA2F0).
 * PC port: glViewport + glDepthRange.
 */
void GXSetViewport(f32 xOrig, f32 yOrig, f32 wd, f32 ht,
                   f32 nearZ, f32 farZ);

/**
 * GXSetProjection -- Load a projection matrix.
 * Called from GSgfx_PreRetraceCallback, GSgfx_BeginFrame (fn_800BD640).
 * PC port: Upload to u_projMatrix uniform.
 */
void GXSetProjection(Mtx44 mtx, GXProjectionType type);

/**
 * GXSetScissor -- Set the scissor rectangle.
 * Called from GStextureUploadFromBuffer, effect rendering (fn_800B962C).
 * PC port: glScissor + glEnable(GL_SCISSOR_TEST).
 */
void GXSetScissor(u32 xOrig, u32 yOrig, u32 wd, u32 ht);

/* --- 3. Matrix Operations (Section 1.3) --- */

/**
 * GXLoadPosMtxImm -- Load a 3x4 position/modelview matrix.
 * Called from GSgfx_ConfigurePipeline, matrix stack push/pop (fn_800BD744).
 * PC port: Upload to u_modelViewMatrix uniform.
 */
void GXLoadPosMtxImm(Mtx mtx, u32 id);

/**
 * GXSetCurrentMtx -- Select the current position matrix slot.
 * Called from HSD PObj rendering.
 * PC port: Tracks the active matrix slot.
 */
void GXSetCurrentMtx(u32 id);

/**
 * GXSetVtxDesc -- Set a vertex attribute descriptor type.
 * Called from HSD PObj descriptor setup.
 */
void GXSetVtxDesc(GXAttr attr, GXAttrType type);

/**
 * GXClearVtxDesc -- Clear all vertex attribute descriptors.
 * Called from HSD PObj descriptor setup.
 */
void GXClearVtxDesc(void);

/**
 * GXSetVtxAttrFmt -- Set a vertex attribute format entry.
 * Called from HSD PObj descriptor setup.
 */
void GXSetVtxAttrFmt(GXVtxFmt vtxfmt, GXAttr attr,
                     GXCompCnt cnt, GXCompType type, u8 frac);

/**
 * GXSetArray -- Set the base pointer and stride for an indexed vertex array.
 * Called from HSD PObj descriptor setup.
 */
void GXSetArray(GXAttr attr, void* base, u8 stride);

/**
 * GXLoadNrmMtxImm -- Load a 3x3 normal matrix (from a 3x4 Mtx).
 * Called from the pipeline for lighting normal transforms.
 * PC port: Upload to u_normalMatrix uniform (inverse transpose of MV).
 */
void GXLoadNrmMtxImm(Mtx mtx, u32 id);

/* --- 4. TEV / Blend / Alpha / Z / Fog State (Section 1.4) --- */

/**
 * GXSetTevOp -- Set TEV operation using a preset mode.
 * Called from GSgfx_ConfigureTEV (fn_800DA028).
 * PC port: Select pre-compiled GLSL shader variant.
 */
void GXSetTevOp(GXTevStageID stage, GXTevMode mode);

/**
 * GXSetTevColorIn -- Set TEV color input sources for a stage.
 * Called from HSD MObj TEV setup, GSmaterial_Create.
 * PC port: Input to the GLSL shader generator.
 */
void GXSetTevColorIn(GXTevStageID stage,
                     GXTevColorArg a, GXTevColorArg b,
                     GXTevColorArg c, GXTevColorArg d);

/**
 * GXSetTevAlphaIn -- Set TEV alpha input sources for a stage.
 * Called from HSD MObj TEV setup, GSmaterial_Create.
 * PC port: Input to the GLSL shader generator.
 */
void GXSetTevAlphaIn(GXTevStageID stage,
                     GXTevAlphaArg a, GXTevAlphaArg b,
                     GXTevAlphaArg c, GXTevAlphaArg d);

/**
 * GXSetTevColorOp -- Set TEV color combiner operation for a stage.
 * PC port: Input to the GLSL shader generator.
 */
void GXSetTevColorOp(GXTevStageID stage, GXTevOp op,
                     GXTevBias bias, GXTevScale scale,
                     GXBool clamp, GXTevRegID out_reg);

/**
 * GXSetTevAlphaOp -- Set TEV alpha combiner operation for a stage.
 * PC port: Input to the GLSL shader generator.
 */
void GXSetTevAlphaOp(GXTevStageID stage, GXTevOp op,
                     GXTevBias bias, GXTevScale scale,
                     GXBool clamp, GXTevRegID out_reg);

/**
 * GXSetTevColor -- Set a TEV color register value.
 * PC port: glUniform4f(u_tevColor[n], ...).
 */
void GXSetTevColor(GXTevRegID id, GXColor color);

/**
 * GXSetTevKColor -- Set a TEV constant (Konst) color register.
 * PC port: glUniform4f(u_tevKonst[n], ...).
 */
void GXSetTevKColor(GXTevRegID id, GXColor color);

/**
 * GXSetNumTevStages -- Set the number of active TEV stages.
 * PC port: Shader variant selection key.
 */
void GXSetNumTevStages(u8 nStages);

/**
 * GXSetTevOrder -- Set the texture and rasterized color for a TEV stage.
 * PC port: Input to the GLSL shader generator.
 */
void GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord,
                   GXTexMapID map, GXChannelID color);

/**
 * GXSetTevKColorSel -- Select the konst color source for a TEV stage.
 * Called from HSD TEV setup (hsd_tev.c).
 * PC port: store the K-color selection per stage (consumed by the TEV path).
 */
void GXSetTevKColorSel(GXTevStageID stage, u32 sel);

/**
 * GXSetTevKAlphaSel -- Select the konst alpha source for a TEV stage.
 * Called from HSD TEV setup (hsd_tev.c).
 * PC port: store the K-alpha selection per stage (consumed by the TEV path).
 */
void GXSetTevKAlphaSel(GXTevStageID stage, u32 sel);

/**
 * GXSetTevSwapModeTable -- Define a color-channel swap table entry.
 * Called from HSD TEV setup (hsd_tev.c).
 * PC port: store the per-table RGBA channel selectors (functional no-op store).
 */
void GXSetTevSwapModeTable(u32 id, u32 r, u32 g, u32 b, u32 a);

/**
 * GXSetTevIndirect -- Configure indirect texturing for a TEV stage.
 * Called from HSD TEV setup (hsd_tev.c).
 * PC port: store the indirect parameters per stage (functional no-op store).
 */
void GXSetTevIndirect(GXTevStageID stage, u32 ind_stage, u32 format,
                      u32 bias_sel, u32 mtx_sel, u32 wrap_s, u32 wrap_t,
                      u32 add_prev, u32 utc_lod, u32 alpha_sel);

/**
 * GXSetBlendMode -- Set the blending mode.
 * Called from GSgfx_ConfigureBlend (fn_800DA2BC).
 * PC port: glBlendFunc + glBlendEquation.
 */
void GXSetBlendMode(GXBlendMode type, GXBlendFactor src_factor,
                    GXBlendFactor dst_factor, GXLogicOp op);

/**
 * GXSetAlphaCompare -- Set alpha test comparison.
 * Called from GSgfx_ConfigureAlpha (fn_800DA100).
 * PC port: Fragment shader discard logic.
 */
void GXSetAlphaCompare(GXCompare comp0, u8 ref0,
                       GXAlphaOp op,
                       GXCompare comp1, u8 ref1);

/**
 * GXSetZMode -- Set depth test mode.
 * Called from GSgfx_ConfigureZ (fn_800DA1E8).
 * PC port: glDepthFunc + glDepthMask + glEnable(GL_DEPTH_TEST).
 */
void GXSetZMode(GXBool compare_enable, GXCompare func,
                GXBool update_enable);

/**
 * GXSetZCompLoc -- Set Z comparison location (before or after texture).
 * PC port: Affects discard placement in fragment shader.
 */
void GXSetZCompLoc(GXBool before_tex);

/**
 * GXSetFog -- Set distance fog parameters.
 * Called from GSgfx_ConfigureFog (fn_800D9F40).
 * PC port: Fog uniforms in fragment shader.
 */
void GXSetFog(GXFogType type, f32 startz, f32 endz,
              f32 nearz, f32 farz, GXColor color);

/**
 * GXSetCullMode -- Set polygon culling mode.
 * Called from effect rendering, draw dispatch (fn_800D6A00).
 * PC port: glEnable(GL_CULL_FACE) + glCullFace.
 */
void GXSetCullMode(GXCullMode mode);

/**
 * GXSetChanCtrl -- Set color channel control (lighting enable).
 * Called from main render loop, lighting pipeline (fn_80101B90, fn_800DC0D4).
 * PC port: Enable/disable per-channel lighting in shader.
 */
void GXSetChanCtrl(GXChannelID chan, GXBool enable,
                   GXColorSrc amb_src, GXColorSrc mat_src,
                   u32 light_mask, GXDiffuseFn diff_fn,
                   GXAttnFn attn_fn);

/**
 * GXSetChanAmbColor -- Set ambient color for a color channel.
 * PC port: glUniform4f(u_ambientColor, ...).
 */
void GXSetChanAmbColor(GXChannelID chan, GXColor color);

/**
 * GXSetChanMatColor -- Set material color for a color channel.
 * PC port: glUniform4f(u_matColor, ...).
 */
void GXSetChanMatColor(GXChannelID chan, GXColor color);

/* --- 5. Texture State (Section 1.5) --- */

/**
 * GXInitTexObj -- Initialize a texture object.
 * Called from GStextureCreate, GStextureSetupFromTPL (fn_800BA9E4).
 * PC port: glGenTextures + glTexParameteri (wrap, filter).
 */
void GXInitTexObj(GXTexObj* obj, void* image,
                  u16 width, u16 height, GXTexFmt format,
                  GXTexWrapMode wrap_s, GXTexWrapMode wrap_t,
                  GXBool mipmap);

/**
 * GXHostInitTexObjRGBA8 -- Host-only texture upload from linear RGBA8 pixels.
 * Used by narrow archive/TEV smoke paths after CPU-side interpretation.
 */
void GXHostInitTexObjRGBA8(GXTexObj* obj, const void* rgba,
                           u16 width, u16 height,
                           GXTexWrapMode wrap_s, GXTexWrapMode wrap_t);

/**
 * GXHostUpdateTexObjRGBA8 -- replace an existing host texture's pixels in place
 * (glTexSubImage2D) when the dimensions are unchanged, else (re)create it. Lets a
 * video frame stream reuse one GL texture instead of leaking one per frame.
 */
void GXHostUpdateTexObjRGBA8(GXTexObj* obj, const void* rgba,
                             u16 width, u16 height);

/**
 * GXInitTexObjFilterMode -- Override the min/mag filter on a texture object.
 * Called from HSD TObj setup (hsd_tobj_ext.c, hsd_texp.c).
 * PC port: re-apply GL_TEXTURE_MIN/MAG_FILTER to the object's GL texture.
 */
void GXInitTexObjFilterMode(GXTexObj* obj, GXTexFilter min_filt,
                            GXTexFilter mag_filt);

/**
 * GXInitTexObjLOD -- Set LOD / mipmap filtering parameters on a texture object.
 * Called from HSD TObj mipmap setup (hsd_tobj_ext.c, hsd_texp.c).
 * PC port: apply GL_TEXTURE_MIN/MAG_FILTER + MIN/MAX_LOD + LOD_BIAS.
 */
void GXInitTexObjLOD(GXTexObj* obj, GXTexFilter min_filt, GXTexFilter mag_filt,
                     f32 min_lod, f32 max_lod, f32 lod_bias,
                     GXBool bias_clamp, GXBool do_edge_lod, u8 max_aniso);

/**
 * GXInitTexObjWrapMode -- Override the S/T wrap mode on a texture object.
 * Called from HSD TObj setup (hsd_tobj_ext.c, hsd_texp.c).
 * PC port: re-apply GL_TEXTURE_WRAP_S/T to the object's GL texture.
 */
void GXInitTexObjWrapMode(GXTexObj* obj, GXTexWrapMode wrap_s,
                          GXTexWrapMode wrap_t);

/**
 * GXInitTlutObj -- Initialize a TLUT (palette) object.
 * Called from GStextureCreate, GStextureSetupFromTPL (fn_800BB050).
 * PC port: Decode TLUT on CPU (no GL equivalent needed).
 */
void GXInitTlutObj(GXTlutObj* obj, void* lut,
                   GXTlutFmt fmt, u16 n_entries);

/**
 * GXLoadTexObj -- Bind a texture object to a texture map slot.
 * PC port: glActiveTexture + glBindTexture.
 */
void GXLoadTexObj(GXTexObj* obj, GXTexMapID id);

/**
 * GXInvalidateTexAll -- Invalidate the entire GX texture cache.
 * Called from GStextureFlush, GStextureUploadFromBuffer (fn_800BB29C).
 * PC port: No-op (PC texture cache is coherent).
 */
void GXInvalidateTexAll(void);

/**
 * GXCopyTex -- Copy the EFB to a texture.
 * Called from GStextureUploadFromBuffer (fn_800B9FE4).
 * PC port: Render to FBO, use FBO color attachment as texture.
 */
void GXCopyTex(void* dest, GXBool clear);

/**
 * GXSetCopyTexSrc -- Set the source rectangle for texture copy.
 * Called from GStextureUploadFromBuffer (fn_800B96F8).
 * PC port: Configure FBO blit source region.
 */
void GXSetTexCopySrc(u16 left, u16 top, u16 wd, u16 ht);

/**
 * GXPixModeSync -- Wait for pixel pipeline to finish.
 * Called from GStextureUploadFromBuffer (fn_800B8E74).
 * PC port: glFinish (rarely needed).
 */
void GXPixModeSync(void);

/* --- 6. Lighting (Section 1.6) --- */

/**
 * GXInitLightObj -- Initialize a light object with position, direction, color.
 * Called from GSgfx_InitLighting (fn_800DB890).
 * PC port: Set per-light uniform struct.
 */
void GXInitLightObj(GXLightObj* obj, GXColor color,
                    f32 px, f32 py, f32 pz);

/**
 * GXInitLightDir -- Set the direction of a light.
 */
void GXInitLightDir(GXLightObj* obj, f32 nx, f32 ny, f32 nz);

/**
 * GXInitLightDistAttn -- Set distance attenuation for a light.
 */
void GXInitLightDistAttn(GXLightObj* obj, f32 ref_dist,
                         f32 ref_brightness, GXDiffuseFn fn);

/**
 * GXInitLightSpot -- Set spotlight parameters.
 */
void GXInitLightSpot(GXLightObj* obj, f32 cutoff, u32 spot_fn);

/**
 * GXInitSpecularDir -- Set specular highlight direction.
 */
void GXInitSpecularDir(GXLightObj* obj, f32 nx, f32 ny, f32 nz);

/**
 * GXInitLightShininess -- Set specular shininess.
 */
void GXInitLightShininess(GXLightObj* obj, f32 shininess);

/**
 * GXLoadLightObj -- Upload a light object to a hardware light slot.
 * Called from lighting pipeline (fn_800DB890-fn_800DC224).
 * PC port: Upload light data to uniform buffer.
 */
void GXLoadLightObj(GXLightObj* obj, GXLightID id);

/* --- 7. Draw Commands (Section 1.7) --- */

/**
 * GXBegin -- Begin submitting vertices for a primitive.
 * Called from effect rendering, draw dispatch (fn_800B8DF4).
 * PC port: Begin accumulating into dynamic VBO.
 */
void GXBegin(GXPrimitive type, GXVtxFmt vtxfmt, u16 nverts);

/**
 * GXEnd -- End vertex submission.
 * Called from effect rendering (fn_800B856C).
 * PC port: glDrawArrays from dynamic VBO.
 */
void GXEnd(void);

/**
 * GXPosition3f32 -- Submit a vertex position (immediate mode).
 * Called from effect rendering (fn_800D67BC).
 * PC port: Write to dynamic VBO.
 */
void GXPosition3f32(f32 x, f32 y, f32 z);

/**
 * GXColor4u8 -- Submit a vertex color (immediate mode).
 * Called from effect rendering (fn_800D6680).
 * PC port: Write to dynamic VBO.
 */
void GXColor4u8(u8 r, u8 g, u8 b, u8 a);

/**
 * GXTexCoord2f32 -- Submit a texture coordinate (immediate mode).
 * Called from effect rendering (fn_800D5CB8).
 * PC port: Write to dynamic VBO.
 */
void GXTexCoord2f32(f32 s, f32 t);

/**
 * GXNormal3f32 -- Submit a normal (immediate mode).
 * PC port: stores the normal on the current immediate vertex for smooth
 * shader lighting. Vertices without normals leave this at zero and use the
 * shader's derivative fallback.
 */
void GXNormal3f32(f32 x, f32 y, f32 z);

/**
 * GXCallDisplayList -- Execute a pre-compiled display list.
 * Called from GSgfx_DrawDispatch (fn_800E1544).
 * PC port: glDrawArrays from pre-built static VBO.
 */
typedef struct {
    f32 pos[3];
    u8 color[4];
    f32 texcoord[2];
} GXHostDisplayListVertex;

typedef struct {
    u32 magic;
    u32 primitive;
    u32 vertexCount;
    const GXHostDisplayListVertex* vertices;
} GXHostDisplayList;

enum {
    GX_HOST_DISPLAY_LIST_MAGIC = 0x5043444Cu
};

void GXCallDisplayList(void* list, u32 nbytes);

/* --- 8. Framebuffer / Copy (Section 1.8) --- */

/**
 * GXCopyDisp -- Copy the EFB to the XFB (display).
 * Called from GSgfxSwapBuffers.
 * PC port: glfwSwapBuffers.
 */
void GXCopyDisp(void* dest, GXBool clear);

/**
 * GXSetCopyFilter -- Set the anti-aliasing copy filter.
 * Called from GStextureUploadFromBuffer (fn_800BCE88).
 * PC port: Not needed (multisampling handled differently on PC).
 */
void GXSetCopyFilter(GXBool aa, u8 sample_pattern[12][2],
                     GXBool vf, u8 vfilter[7]);

/**
 * GXSetDispCopyDst -- Set the destination stride for display copy.
 * Called from GStextureUploadFromBuffer (fn_800BCE88).
 * PC port: Not needed.
 */
void GXSetDispCopyDst(u16 wd);

/**
 * GXSetNumChans -- Set the number of active color channels.
 * PC port: Configure per-channel lighting in vertex shader.
 */
void GXSetNumChans(u8 nChans);

/**
 * GXSetNumTexGens -- Set the number of active texture coordinate generators.
 * PC port: Configure texcoord generation in vertex shader.
 */
void GXSetNumTexGens(u8 nTexGens);

/**
 * GXHostSetVertexAlphaScale -- Host-only color alpha modulation for narrow
 * material-backed smoke paths.
 */
void GXHostSetVertexAlphaScale(f32 alphaScale);

/**
 * GXHostClearTextureBinding -- Host-only reset for the narrow texture bridge.
 * Clears the currently bound texture and disables texcoord generation state.
 */
void GXHostClearTextureBinding(void);

/**
 * GXHostSetLightingEnabled -- Host-only toggle for directional vertex lighting.
 * When enabled (non-zero), the modern TEV->GLSL path shades each fragment by a
 * hardcoded directional lambert (reconstructed from view-space derivatives) so
 * 3D scene geometry gets visible face shading. 2D overlays leave it disabled to
 * stay full-bright. Affects only the modern shader draw path.
 */
void GXHostSetLightingEnabled(GXBool enabled);

/**
 * GXHostSetLightParams -- Host-only override of the directional sun used by the
 * lambert pass. dx/dy/dz is the view-space light direction (normalized in the
 * shader); ambient [0..1] is the floor brightness for unlit faces (lower = more
 * 3D contrast). Lets the scene tune ruin-column shading; see PCPORT_LIGHT_*.
 */
void GXHostSetLightParams(f32 dx, f32 dy, f32 dz, f32 ambient);

/**
 * GXHostSetExposure -- Host-only final RGB gain (1.0 = neutral). >1 brightens
 * the fragment colour, used to lift the unlit dark-albedo character meshes
 * toward the brightness of the lit reference art. Output is clamped to [0,1].
 */
void GXHostSetExposure(f32 gain);

/**
 * GXSetTexCoordGen2 -- Configure a texture coordinate generator.
 * Called from HSD TObj setup (hsd_tobj_ext.c).
 * PC port: records the texgen configuration and marks the tex matrix slot
 * as active for the named texcoord so the vertex shader applies the SRT.
 *
 * @param dst_coord   Output texcoord ID (GX_TEXCOORD0..7).
 * @param func        Texcoord generator type (GXTexGenType).
 * @param src_param   Texcoord generator input (GXTexGenSrc).
 * @param mtx         Texture matrix slot (GXTexMtx or GX_IDENTITY).
 * @param normalize   If GX_TRUE, normalize the generated texcoord.
 * @param pt_mtx      Post-transform matrix slot (pass GX_IDENTITY normally).
 */
void GXSetTexCoordGen2(GXTexCoordID dst_coord, GXTexGenType func,
                       GXTexGenSrc src_param, GXTexMtx mtx,
                       GXBool normalize, u32 pt_mtx);

/**
 * GXHostSetTexMatrix -- Host-only: load a 3x4 SRT texture matrix into a slot.
 * @param slot  Texcoord slot index (0..7), matching GX_TEXCOORD0..7.
 * @param m     Row-major 3x4 matrix (GCN Mtx convention). The upper-left 3x3
 *              is used to transform (s, t, 1) in the vertex shader.
 *              Pass NULL to restore the identity matrix for that slot.
 */
void GXHostSetTexMatrix(u32 slot, const f32 m[3][4]);

/**
 * GXHostEnableOffscreenRender -- Host-only offscreen framebuffer path.
 * When enabled, GX draws into a texture-backed FBO instead of the default
 * window framebuffer. This is used for headless / PCPORT_DUMP capture.
 */
void GXHostEnableOffscreenRender(u32 width, u32 height);

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_GX_SHIM_H */
