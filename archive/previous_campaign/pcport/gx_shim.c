/**
 * @file gx_shim.c
 * @brief GX-to-OpenGL 3.3 translation layer -- stub implementations.
 *
 * Each function is a stub that will be filled in during Phase 3 to translate
 * the GCN GX call into the equivalent OpenGL 3.3 operation.
 *
 * References:
 *   - docs/pc_port_design.md Section 8 (Core API Mapping Table)
 *   - gx_shim.h for full function documentation
 *   - gx_tev.h for TEV-to-GLSL shader generation
 *   - gx_texture.h for GCN texture format decoding
 *
 * Phase 3 PC port scaffolding -- skeleton only.
 */

#ifdef __MWERKS__
/* GCN build: pcport shim not applicable */
#else

#include "gx_shim.h"
#include "pcport_window.h"
#include "gx_tev.h"
#include "gx_texture.h"

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#ifndef GL_MIRRORED_REPEAT
#define GL_MIRRORED_REPEAT 0x8370
#endif

#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER 0x8D40
#endif
#ifndef GL_RENDERBUFFER
#define GL_RENDERBUFFER 0x8D41
#endif
#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0 0x8CE0
#endif
#ifndef GL_DEPTH_STENCIL_ATTACHMENT
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#endif
#ifndef GL_DEPTH24_STENCIL8
#define GL_DEPTH24_STENCIL8 0x88F0
#endif
#ifndef GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#endif

/* =========================================================================
 * Internal state tracking
 *
 * The GX shim maintains a shadow of the GX state machine so that
 * redundant GL state changes can be avoided.
 * ========================================================================= */

/** Current TEV stage configuration (fed to shader generator) */
static GXTevState g_tevState;

/** Number of active TEV stages */
static u8 g_numTevStages = 1;

/* -------------------------------------------------------------------------
 * Extended per-stage TEV state.
 *
 * The shared GXTevStageState (gx_tev.h, owned elsewhere) has no fields for
 * konst color/alpha selection, swap tables, or indirect texturing. The PC
 * port keeps these here so the gx_tev/draw path can read them via the
 * GXHost* accessors below without touching the shared struct.
 * ------------------------------------------------------------------------- */

/** Per-stage konst color selector (GXTevKColorSel) */
static u8 g_tevKColorSel[GX_MAX_TEVSTAGE];

/** Per-stage konst alpha selector (GXTevKAlphaSel) */
static u8 g_tevKAlphaSel[GX_MAX_TEVSTAGE];

/** Swap-mode tables: [table id][R,G,B,A] channel selectors */
#define GX_TEV_SWAP_TABLE_COUNT 4
static u8 g_tevSwapTable[GX_TEV_SWAP_TABLE_COUNT][4];

/** Per-stage indirect texturing parameters (stored, applied as no-op) */
typedef struct {
    u8 active;
    u8 indStage;
    u8 format;
    u8 biasSel;
    u8 mtxSel;
    u8 wrapS;
    u8 wrapT;
    u8 addPrev;
    u8 utcLod;
    u8 alphaSel;
} GXTevIndirectState;

static GXTevIndirectState g_tevIndirect[GX_MAX_TEVSTAGE];

/** Current projection matrix */
static f32 g_projMatrix[4][4];
static GXProjectionType g_projType;

/** Current modelview matrix slots (GX supports 10 position matrix slots) */
static f32 g_posMtx[10][3][4];
static f32 g_nrmMtx[10][3][4];

/** Current viewport parameters */
static f32 g_viewportX, g_viewportY, g_viewportW, g_viewportH;
static f32 g_viewportNear, g_viewportFar;

static GLuint g_offscreenFbo = 0;
static GLuint g_offscreenColorTex = 0;
static GLuint g_offscreenDepthRbo = 0;
static u32 g_offscreenWidth = 0;
static u32 g_offscreenHeight = 0;
static int g_offscreenEnabled = 0;

typedef void   (APIENTRY *PFNGLGENFRAMEBUFFERSPROC)(GLsizei, GLuint*);
typedef void   (APIENTRY *PFNGLBINDFRAMEBUFFERPROC)(GLenum, GLuint);
typedef void   (APIENTRY *PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei, const GLuint*);
typedef void   (APIENTRY *PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum, GLenum, GLenum, GLuint, GLint);
typedef void   (APIENTRY *PFNGLGENRENDERBUFFERSPROC)(GLsizei, GLuint*);
typedef void   (APIENTRY *PFNGLBINDRENDERBUFFERPROC)(GLenum, GLuint);
typedef void   (APIENTRY *PFNGLDELETERENDERBUFFERSPROC)(GLsizei, const GLuint*);
typedef void   (APIENTRY *PFNGLRENDERBUFFERSTORAGEPROC)(GLenum, GLenum, GLsizei, GLsizei);
typedef void   (APIENTRY *PFNGLFRAMEBUFFERRENDERBUFFERPROC)(GLenum, GLenum, GLenum, GLuint);
typedef GLenum (APIENTRY *PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum);

static PFNGLGENFRAMEBUFFERSPROC g_glGenFramebuffers = NULL;
static PFNGLBINDFRAMEBUFFERPROC g_glBindFramebuffer = NULL;
static PFNGLDELETEFRAMEBUFFERSPROC g_glDeleteFramebuffers = NULL;
static PFNGLFRAMEBUFFERTEXTURE2DPROC g_glFramebufferTexture2D = NULL;
static PFNGLGENRENDERBUFFERSPROC g_glGenRenderbuffers = NULL;
static PFNGLBINDRENDERBUFFERPROC g_glBindRenderbuffer = NULL;
static PFNGLDELETERENDERBUFFERSPROC g_glDeleteRenderbuffers = NULL;
static PFNGLRENDERBUFFERSTORAGEPROC g_glRenderbufferStorage = NULL;
static PFNGLFRAMEBUFFERRENDERBUFFERPROC g_glFramebufferRenderbuffer = NULL;
static PFNGLCHECKFRAMEBUFFERSTATUSPROC g_glCheckFramebufferStatus = NULL;

/** Scissor state */
static u32 g_scissorX, g_scissorY, g_scissorW, g_scissorH;

/** Blend state */
static GXBlendMode g_blendType;
static GXBlendFactor g_blendSrc, g_blendDst;
static GXLogicOp g_blendLogicOp;

/** Alpha compare state */
static GXCompare g_alphaComp0, g_alphaComp1;
static u8 g_alphaRef0, g_alphaRef1;
static GXAlphaOp g_alphaOp;

/** Depth state */
static GXBool g_zEnable, g_zUpdate;
static GXCompare g_zFunc;

/** Fog state */
static GXFogType g_fogType;
static f32 g_fogStart, g_fogEnd, g_fogNear, g_fogFar;
static GXColor g_fogColor;

/** Cull mode */
static GXCullMode g_cullMode;

/** Channel state (lighting) */
static GXColor g_chanAmbColor[2];
static GXColor g_chanMatColor[2];
static u8 g_chanCtrlEnable[2];   /* per-channel GXSetChanCtrl enable flag */
static u8 g_chanMatSrc[2];       /* per-channel material color source (GXColorSrc) */

/** TEV color/konst registers */
static GXColor g_tevColorRegs[4];
static GXColor g_tevKonstRegs[4];

/** Light objects */
static GXLightObj g_lightObjs[8];

/** Immediate-mode vertex accumulation buffer */
#define GX_IMM_VTX_MAX 65536

typedef struct {
    f32 pos[3];
    u8  color[4];
    f32 texcoord[2];
    f32 normal[3];   /* per-vertex normal for smooth (Gouraud) shading; (0,0,0) = unset */
} GXImmVertex;

static GXImmVertex g_immVertices[GX_IMM_VTX_MAX];
static GXImmVertex g_immExpandedVertices[(GX_IMM_VTX_MAX * 3) / 2];
static GXImmVertex g_modulatedVertices[(GX_IMM_VTX_MAX * 3) / 2];
static u32 g_immVertexCount = 0;
static GXPrimitive g_immPrimType;
static u16 g_immExpectedVerts = 0;
static u32 g_currentMtxId = 0;
static GXDrawDoneCallback g_drawDoneCallback = (GXDrawDoneCallback)0;
static GXGamma g_dispCopyGamma = GX_GM_1_0;
static int g_gxInitialized = 0;
static u32 g_lastSubmittedVertexCount = 0;
static u32 g_lastExpandedVertexCount = 0;
static GXPrimitive g_lastSubmittedPrimitive = GX_POINTS;
static f32 g_vertexAlphaScale = 1.0f;
static int g_lightingEnabled = 0;
static GLuint g_boundTextureId = 0;
static GXTexMapID g_boundTextureMap = GX_TEXMAP_NULL;
static u8 g_numTexGens = 0;

/** Set when GLAD loaded and the modern TEV->GLSL draw path is usable. */
static int g_tevPathReady = 0;

/** Per-slot texture SRT matrices (8 slots, row-major 3x4). Loaded by
 *  GXHostSetTexMatrix / GXSetTexCoordGen2 and forwarded to gx_tev. */
static f32 g_texMatrixStore[8][3][4];

typedef struct {
    u32 magic;
    u32 glTexId;
    u16 width;
    u16 height;
    u32 format;
    u8 wrapS;
    u8 wrapT;
    u8 mipmap;
    u8 minFilt;   /* GXTexFilter for GL_TEXTURE_MIN_FILTER */
    u8 magFilt;   /* GXTexFilter for GL_TEXTURE_MAG_FILTER */
    u8 reserved[3];
} GXHostTexObj;

enum {
    GX_HOST_TEXOBJ_MAGIC = 0x50435458u
};

typedef struct {
    GXAttrType type;
    void* base;
    u8 stride;
} GXVtxDescState;

typedef struct {
    GXCompCnt compCnt;
    GXCompType compType;
    u8 frac;
} GXVtxAttrFmtState;

#define GX_ATTR_STATE_MAX (GX_VA_TEX7 + 1)
#define GX_VTXFMT_STATE_MAX (GX_VTXFMT7 + 1)

static GXVtxDescState g_vtxDescState[GX_ATTR_STATE_MAX];
static GXVtxAttrFmtState g_vtxFmtState[GX_VTXFMT_STATE_MAX][GX_ATTR_STATE_MAX];

static u16 GXReadBE16(const u8* src) {
    return (u16)(((u16)src[0] << 8) | src[1]);
}

static int GXGetIndexByteCount(GXAttrType type) {
    switch (type) {
        case GX_INDEX8:
            return 1;
        case GX_INDEX16:
            return 2;
        default:
            return 0;
    }
}

static int GXIsSupportedDisplayListAttr(GXAttr attr) {
    return attr == GX_VA_POS || attr == GX_VA_NRM || attr == GX_VA_CLR0 ||
           attr == GX_VA_TEX0 || attr == GX_VA_TEX1;
}

static int GXAttrMatchesActiveTexCoord(GXAttr attr) {
    GXTexCoordID coord = g_tevState.stages[0].texCoordId;

    if (coord == GX_TEXCOORD1) {
        return attr == GX_VA_TEX1;
    }

    return attr == GX_VA_TEX0;
}

static f32 GXReadNumericComponent(const void* src, GXCompType type, u8 frac) {
    f32 scale = 1.0f;

    if (frac != 0) {
        scale = 1.0f / (f32)(1u << frac);
    }

    switch (type) {
        case GX_U8:
            return (f32)(*(const u8*)src) * scale;
        case GX_S8:
            return (f32)(*(const s8*)src) * scale;
        case GX_U16:
            return (f32)(*(const u16*)src) * scale;
        case GX_S16:
            return (f32)(*(const s16*)src) * scale;
        case GX_F32:
        default: {
            f32 value;
            memcpy(&value, src, sizeof(value));
            return value;
        }
    }
}

static u32 GXGetCompTypeSize(GXCompType type) {
    if (type == GX_RGB8) {
        return 1;
    }

    switch (type) {
        case GX_U8:
        case GX_S8:
            return 1;
        case GX_U16:
        case GX_S16:
            return 2;
        case GX_F32:
        case GX_RGBA8:
        default:
            return 4;
    }
}

static int GXDecodeIndexedAttr(const u8** cursor, const u8* end,
                               GXVtxFmt vtxfmt, GXAttr attr,
                               GXImmVertex* out) {
    const GXVtxDescState* desc = &g_vtxDescState[attr];
    const GXVtxAttrFmtState* fmt = &g_vtxFmtState[vtxfmt][attr];
    u32 index = 0;
    const u8* base;

    if (desc->type == GX_NONE) {
        return 1;
    }

    if (!GXIsSupportedDisplayListAttr(attr)) {
        return 0;
    }

    if (desc->base == NULL || desc->stride == 0) {
        return 0;
    }

    if (desc->type == GX_INDEX8) {
        if ((*cursor + 1) > end) {
            return 0;
        }
        index = (*cursor)[0];
        *cursor += 1;
    } else if (desc->type == GX_INDEX16) {
        if ((*cursor + 2) > end) {
            return 0;
        }
        index = GXReadBE16(*cursor);
        *cursor += 2;
    } else {
        return 0;
    }

    base = (const u8*)desc->base + (index * desc->stride);

    switch (attr) {
        case GX_VA_POS:
            out->pos[0] = GXReadNumericComponent(base + 0, fmt->compType, fmt->frac);
            out->pos[1] = GXReadNumericComponent(base + GXGetCompTypeSize(fmt->compType), fmt->compType, fmt->frac);
            if (fmt->compCnt == GX_POS_XYZ) {
                out->pos[2] = GXReadNumericComponent(base + (GXGetCompTypeSize(fmt->compType) * 2), fmt->compType, fmt->frac);
            } else {
                out->pos[2] = 0.0f;
            }
            break;
        case GX_VA_CLR0:
            if (fmt->compType == GX_RGBA8) {
                out->color[0] = base[0];
                out->color[1] = base[1];
                out->color[2] = base[2];
                out->color[3] = base[3];
            } else if (fmt->compType == GX_RGB8) {
                out->color[0] = base[0];
                out->color[1] = base[1];
                out->color[2] = base[2];
                out->color[3] = 0xFF;
            } else {
                return 0;
            }
            break;
        case GX_VA_TEX0:
        case GX_VA_TEX1:
            if (GXAttrMatchesActiveTexCoord(attr)) {
                out->texcoord[0] = GXReadNumericComponent(base + 0, fmt->compType, fmt->frac);
                if (fmt->compCnt == GX_TEX_ST) {
                    out->texcoord[1] = GXReadNumericComponent(base + GXGetCompTypeSize(fmt->compType), fmt->compType, fmt->frac);
                } else {
                    out->texcoord[1] = 0.0f;
                }
            }
            break;
        case GX_VA_NRM:
            out->normal[0] = GXReadNumericComponent(base + 0, fmt->compType, fmt->frac);
            out->normal[1] = GXReadNumericComponent(base + GXGetCompTypeSize(fmt->compType), fmt->compType, fmt->frac);
            out->normal[2] = GXReadNumericComponent(base + (GXGetCompTypeSize(fmt->compType) * 2), fmt->compType, fmt->frac);
            break;
        default:
            return 0;
    }

    return 1;
}

static void GXEnsureCurrentContext(void) {
    GLFWwindow* window = PCPort_GetHostWindow();

    if (window == NULL) {
        return;
    }

    if (glfwGetCurrentContext() != window) {
        glfwMakeContextCurrent(window);
    }
}

static void GXLoadMatrixMode(GLenum mode, const f32 matrix[4][4]) {
    GLfloat glMatrix[16];
    int row;
    int col;

    GXEnsureCurrentContext();

    for (col = 0; col < 4; ++col) {
        for (row = 0; row < 4; ++row) {
            glMatrix[(col * 4) + row] = matrix[row][col];
        }
    }

    glMatrixMode(mode);
    glLoadMatrixf(glMatrix);
}

static void GXApplyProjectionMatrix(void) {
    GXLoadMatrixMode(GL_PROJECTION, g_projMatrix);
}

static void GXApplyModelViewMatrix(const Mtx mtx) {
    f32 expanded[4][4] = {
        { mtx[0][0], mtx[0][1], mtx[0][2], mtx[0][3] },
        { mtx[1][0], mtx[1][1], mtx[1][2], mtx[1][3] },
        { mtx[2][0], mtx[2][1], mtx[2][2], mtx[2][3] },
        { 0.0f,      0.0f,      0.0f,      1.0f }
    };

    GXLoadMatrixMode(GL_MODELVIEW, expanded);
}

static void GXDestroyOffscreenRenderTarget(void) {
    if (g_glDeleteRenderbuffers == NULL ||
        g_glDeleteFramebuffers == NULL) {
        g_offscreenEnabled = 0;
        g_offscreenWidth = 0u;
        g_offscreenHeight = 0u;
        return;
    }

    if (g_offscreenDepthRbo != 0u) {
        g_glDeleteRenderbuffers(1, &g_offscreenDepthRbo);
        g_offscreenDepthRbo = 0u;
    }
    if (g_offscreenColorTex != 0u) {
        glDeleteTextures(1, &g_offscreenColorTex);
        g_offscreenColorTex = 0u;
    }
    if (g_offscreenFbo != 0u) {
        g_glDeleteFramebuffers(1, &g_offscreenFbo);
        g_offscreenFbo = 0u;
    }
    g_offscreenEnabled = 0;
    g_offscreenWidth = 0u;
    g_offscreenHeight = 0u;
}

static void GXBindOffscreenRenderTarget(void) {
    if (!g_offscreenEnabled || g_offscreenFbo == 0u) {
        return;
    }

    if (g_glBindFramebuffer == NULL) {
        return;
    }

    g_glBindFramebuffer(GL_FRAMEBUFFER, g_offscreenFbo);
    glViewport(0, 0, (GLsizei)g_offscreenWidth, (GLsizei)g_offscreenHeight);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
}

static int GXLoadOffscreenGLFunctions(void) {
    if (g_glGenFramebuffers != NULL) {
        return 1;
    }

#define GX_LOAD_GL_PROC(name) \
    do { \
        g_##name = (void*)glfwGetProcAddress(#name); \
        if (g_##name == NULL) { \
            fprintf(stderr, "[gx_shim] missing GL proc %s\n", #name); \
            return 0; \
        } \
    } while (0)

    GX_LOAD_GL_PROC(glGenFramebuffers);
    GX_LOAD_GL_PROC(glBindFramebuffer);
    GX_LOAD_GL_PROC(glDeleteFramebuffers);
    GX_LOAD_GL_PROC(glFramebufferTexture2D);
    GX_LOAD_GL_PROC(glGenRenderbuffers);
    GX_LOAD_GL_PROC(glBindRenderbuffer);
    GX_LOAD_GL_PROC(glDeleteRenderbuffers);
    GX_LOAD_GL_PROC(glRenderbufferStorage);
    GX_LOAD_GL_PROC(glFramebufferRenderbuffer);
    GX_LOAD_GL_PROC(glCheckFramebufferStatus);

#undef GX_LOAD_GL_PROC
    return 1;
}

static GXHostTexObj* GXGetHostTexObj(GXTexObj* obj) {
    return (GXHostTexObj*)(void*)obj;
}

static const GXHostTexObj* GXGetConstHostTexObj(const GXTexObj* obj) {
    return (const GXHostTexObj*)(const void*)obj;
}

static GLenum GXTranslateWrapMode(GXTexWrapMode mode) {
    switch (mode) {
        case GX_REPEAT:
            return GL_REPEAT;
        case GX_MIRROR:
            return GL_MIRRORED_REPEAT;
        case GX_CLAMP:
        default:
            return GL_CLAMP_TO_EDGE;
    }
}

static GLenum GXTranslateMagFilter(GXTexFilter filter) {
    return filter == GX_NEAR ? GL_NEAREST : GL_LINEAR;
}

static GLenum GXTranslateMinFilter(GXTexFilter filter, GXBool mipmap) {
    if (!mipmap) {
        return GXTranslateMagFilter(filter);
    }

    switch (filter) {
        case GX_NEAR:
        case GX_NEAR_MIP_NEAR:
        case GX_NEAR_MIP_LIN:
            return GL_NEAREST_MIPMAP_NEAREST;
        case GX_LINEAR:
        case GX_LIN_MIP_NEAR:
        case GX_LIN_MIP_LIN:
        default:
            return GL_LINEAR_MIPMAP_LINEAR;
    }
}

static GLenum GXTranslateTevModeToEnvMode(GXTevMode mode) {
    switch (mode) {
        case GX_REPLACE:
            return GL_REPLACE;
        case GX_DECAL:
            return GL_DECAL;
        case GX_BLEND:
            return GL_BLEND;
        case GX_MODULATE:
        default:
            return GL_MODULATE;
    }
}

/* =========================================================================
 * 1. Initialization and FIFO
 * ========================================================================= */

void GXInit(void* base, u32 size) {
    GLFWwindow* window;
    const char* headlessEnv;
    int glTrace;

    (void)base; (void)size;
    glTrace = (getenv("PCPORT_GL_TRACE") != NULL);
    if (glTrace) {
        fprintf(stderr, "[gx_shim] GXInit:start\n");
    }

    memset(&g_tevState, 0, sizeof(g_tevState));
    memset(g_vtxDescState, 0, sizeof(g_vtxDescState));
    memset(g_vtxFmtState, 0, sizeof(g_vtxFmtState));
    memset(g_tevKColorSel, 0, sizeof(g_tevKColorSel));
    memset(g_tevKAlphaSel, 0, sizeof(g_tevKAlphaSel));
    memset(g_tevIndirect, 0, sizeof(g_tevIndirect));
    memset(g_chanCtrlEnable, 0, sizeof(g_chanCtrlEnable));
    memset(g_chanMatSrc, 0, sizeof(g_chanMatSrc));
    {
        u32 swapTableIndex;
        /* Default identity swap tables: R->R, G->G, B->B, A->A. */
        for (swapTableIndex = 0; swapTableIndex < GX_TEV_SWAP_TABLE_COUNT;
             ++swapTableIndex) {
            g_tevSwapTable[swapTableIndex][0] = 0;
            g_tevSwapTable[swapTableIndex][1] = 1;
            g_tevSwapTable[swapTableIndex][2] = 2;
            g_tevSwapTable[swapTableIndex][3] = 3;
        }
    }
    g_numTevStages = 1;
    g_immVertexCount = 0;
    g_currentMtxId = 0;
    g_drawDoneCallback = (GXDrawDoneCallback)0;
    g_dispCopyGamma = GX_GM_1_0;
    g_gxInitialized = 1;
    g_vertexAlphaScale = 1.0f;
    g_lightingEnabled = 0;
    g_boundTextureId = 0;
    g_boundTextureMap = GX_TEXMAP_NULL;
    g_numTexGens = 0;
    g_tevState.numTexGens = 0;
    g_tevState.stages[0].tevMode = GX_PASSCLR;
    headlessEnv = getenv("PCPORT_OFFSCREEN");
    if (headlessEnv == NULL || headlessEnv[0] == '\0' || headlessEnv[0] == '0') {
        headlessEnv = getenv("PCPORT_HEADLESS_GL");
    }
    if (headlessEnv == NULL || headlessEnv[0] == '\0' || headlessEnv[0] == '0') {
        headlessEnv = getenv("PCPORT_DUMP");
    }

    /* Texture SRT matrix store: default to identity for all 8 slots. */
    {
        u32 s;
        for (s = 0; s < 8; ++s) {
            memset(g_texMatrixStore[s], 0, sizeof(g_texMatrixStore[s]));
            g_texMatrixStore[s][0][0] = 1.0f;
            g_texMatrixStore[s][1][1] = 1.0f;
            g_texMatrixStore[s][2][2] = 1.0f;
        }
    }

    /* Default alpha compare to "always pass". GX hardware powers up with the
     * alpha test effectively disabled, and the legacy fixed-function draw did
     * no alpha testing. The modern shader path honors alpha compare, so the
     * zero-initialized default (GX_NEVER/GX_NEVER/AND) would discard every
     * fragment. Match the hardware/legacy default instead. */
    g_alphaComp0 = GX_ALWAYS;
    g_alphaComp1 = GX_ALWAYS;
    g_alphaOp = GX_AOP_AND;
    g_alphaRef0 = 0;
    g_alphaRef1 = 0;

    /* Default matrices to identity. The legacy fixed-function path relied on
     * the GL matrix stack being identity until GXSetProjection/GXLoadPosMtxImm
     * were called; the modern shader path reads g_projMatrix/g_posMtx directly,
     * so they must start as identity (not zero) or pre-matrix draws (e.g. the
     * full-screen background quad) collapse to a point and render nothing. */
    memset(g_projMatrix, 0, sizeof(g_projMatrix));
    g_projMatrix[0][0] = g_projMatrix[1][1] =
        g_projMatrix[2][2] = g_projMatrix[3][3] = 1.0f;
    {
        u32 m;
        for (m = 0; m < 10; ++m) {
            memset(g_posMtx[m], 0, sizeof(g_posMtx[m]));
            memset(g_nrmMtx[m], 0, sizeof(g_nrmMtx[m]));
            g_posMtx[m][0][0] = g_posMtx[m][1][1] = g_posMtx[m][2][2] = 1.0f;
            g_nrmMtx[m][0][0] = g_nrmMtx[m][1][1] = g_nrmMtx[m][2][2] = 1.0f;
        }
    }

    window = PCPort_GetHostWindow();
    if (window == NULL) {
        printf("[gx_shim] GXInit host state initialized without a window\n");
        return;
    }

    if (glfwGetCurrentContext() != window) {
        glfwMakeContextCurrent(window);
    }

    /* Load the offscreen framebuffer entry points if the headless path is
     * active. The legacy fixed-function path remains available for the default
     * windowed case. */
    if (headlessEnv != NULL && headlessEnv[0] != '\0' && headlessEnv[0] != '0') {
        GXLoadOffscreenGLFunctions();
    }

    /* Initialize the TEV->GLSL shader path now that the GL context is current.
     * All modern-GL usage stays inside gx_tev.c; this is a no-op on subsequent
     * GXInit calls. If it fails, GXSubmitVertices falls back to the legacy
     * fixed-function draw below. */
    g_tevPathReady = gx_tev_ensure_loaded();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (headlessEnv != NULL && headlessEnv[0] != '\0' && headlessEnv[0] != '0') {
        if (glTrace) {
            fprintf(stderr, "[gx_shim] GXInit:enable-offscreen\n");
        }
        GXHostEnableOffscreenRender((u32)PCPort_GetVideoWidth(),
                                    (u32)PCPort_GetVideoHeight());
    }

    printf("[gx_shim] GXInit configured host GL defaults\n");
    if (glTrace) {
        fprintf(stderr, "[gx_shim] GXInit:done\n");
    }
}

GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback cb) {
    GXDrawDoneCallback prev = g_drawDoneCallback;
    g_drawDoneCallback = cb;
    return prev;
}

void GXSetDispCopyGamma(GXGamma gamma) {
    g_dispCopyGamma = gamma;
}

/* =========================================================================
 * 2. Viewport, Scissor, Projection
 * ========================================================================= */

void GXSetViewport(f32 xOrig, f32 yOrig, f32 wd, f32 ht,
                   f32 nearZ, f32 farZ) {
    GLFWwindow* window;
    int fbWidth;
    int fbHeight;
    int x;
    int y;

    g_viewportX = xOrig;
    g_viewportY = yOrig;
    g_viewportW = wd;
    g_viewportH = ht;
    g_viewportNear = nearZ;
    g_viewportFar = farZ;

    window = PCPort_GetHostWindow();
    if (window == NULL) {
        return;
    }

    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    x = (int)xOrig;
    y = fbHeight - (int)(yOrig + ht);
    if (y < 0) {
        y = 0;
    }

    glViewport(x, y, (int)wd, (int)ht);
    glDepthRange((GLdouble)nearZ, (GLdouble)farZ);
}

void GXSetProjection(Mtx44 mtx, GXProjectionType type) {
    g_projType = type;
    memcpy(g_projMatrix, mtx, sizeof(g_projMatrix));
    if (getenv("PCPORT_PRIM_DEBUG") != NULL) {
        fprintf(stderr, "[proj] type=%d row2=[%.5f %.5f %.5f %.5f] row3=[%.5f %.5f %.5f %.5f]\n",
                (int)type, g_projMatrix[2][0], g_projMatrix[2][1], g_projMatrix[2][2],
                g_projMatrix[2][3], g_projMatrix[3][0], g_projMatrix[3][1],
                g_projMatrix[3][2], g_projMatrix[3][3]);
    }
    GXApplyProjectionMatrix();

    /* TODO: Phase 3b -- Upload projection matrix
     *
     * GCN uses a different projection matrix convention than OpenGL:
     * - GCN clip space Z: [-1, 0] (near = -1, far = 0)
     * - OpenGL clip space Z: [-1, 1] (near = -1, far = 1)
     *
     * Apply the correction: projMatrix[2][2] and projMatrix[2][3]
     * need to be adjusted to map from GCN Z range to GL Z range.
     *
     * Upload via: glUniformMatrix4fv(u_projMatrix_loc, 1, GL_TRUE,
     *                                 (GLfloat*)correctedMtx);
     * (GL_TRUE for row-major -> GL column-major transpose)
     */
}

void GXSetScissor(u32 xOrig, u32 yOrig, u32 wd, u32 ht) {
    GLFWwindow* window;
    int fbWidth;
    int fbHeight;
    int y;

    g_scissorX = xOrig;
    g_scissorY = yOrig;
    g_scissorW = wd;
    g_scissorH = ht;

    window = PCPort_GetHostWindow();
    if (window == NULL) {
        return;
    }

    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    y = fbHeight - (int)(yOrig + ht);
    if (y < 0) {
        y = 0;
    }

    glEnable(GL_SCISSOR_TEST);
    glScissor((int)xOrig, y, (int)wd, (int)ht);
}

/* =========================================================================
 * 3. Matrix Operations
 * ========================================================================= */

void GXLoadPosMtxImm(Mtx mtx, u32 id) {
    if (id >= 10) return;
    memcpy(g_posMtx[id], mtx, sizeof(Mtx));
    GXApplyModelViewMatrix(mtx);

    /* TODO: Phase 3b -- Upload modelview matrix
     *
     * If id == 0 (the default matrix slot used for most rendering):
     *   glUniformMatrix4x3fv(u_modelViewMatrix_loc, 1, GL_TRUE,
     *                         (GLfloat*)mtx);
     * Or expand to 4x4 and use glUniformMatrix4fv.
     *
     * For skinning (envelope mode), matrix slots 1-9 hold bone matrices:
     *   glUniformMatrix4fv(u_boneMatrix_loc + id, 1, GL_TRUE, ...)
     */
}

void GXLoadNrmMtxImm(Mtx mtx, u32 id) {
    if (id >= 10) return;
    memcpy(g_nrmMtx[id], mtx, sizeof(Mtx));

    /* Phase 3b -- Upload normal matrix.
     *
     * GX supplies the normal matrix pre-computed (inverse transpose of the
     * modelview upper 3x3). Record it for the modern TEV->GLSL path; only the
     * upper-left 3x3 is consumed as the u_normalMatrix mat3 in the vertex
     * shader. The actual per-face lambert is reconstructed from view-space
     * derivatives in the fragment shader (the host geometry path carries no
     * per-vertex normal), so this keeps the u_normalMatrix plumbing correct
     * for any future normal-bearing geometry. */
    if (g_tevPathReady && id == g_currentMtxId) {
        gx_tev_set_normal_matrix(g_nrmMtx[id]);
    }
}

void GXSetCurrentMtx(u32 id) {
    g_currentMtxId = id;
}

void GXSetVtxDesc(GXAttr attr, GXAttrType type) {
    if (attr >= GX_ATTR_STATE_MAX) {
        return;
    }

    g_vtxDescState[attr].type = type;
}

void GXClearVtxDesc(void) {
    memset(g_vtxDescState, 0, sizeof(g_vtxDescState));
}

void GXSetVtxAttrFmt(GXVtxFmt vtxfmt, GXAttr attr,
                     GXCompCnt cnt, GXCompType type, u8 frac) {
    if (vtxfmt >= GX_VTXFMT_STATE_MAX || attr >= GX_ATTR_STATE_MAX) {
        return;
    }

    g_vtxFmtState[vtxfmt][attr].compCnt = cnt;
    g_vtxFmtState[vtxfmt][attr].compType = type;
    g_vtxFmtState[vtxfmt][attr].frac = frac;
}

void GXSetArray(GXAttr attr, void* base, u8 stride) {
    if (attr >= GX_ATTR_STATE_MAX) {
        return;
    }

    g_vtxDescState[attr].base = base;
    g_vtxDescState[attr].stride = stride;
}

void GXHostSetVertexAlphaScale(f32 alphaScale) {
    if (alphaScale < 0.0f) {
        alphaScale = 0.0f;
    } else if (alphaScale > 1.0f) {
        alphaScale = 1.0f;
    }

    g_vertexAlphaScale = alphaScale;
}

void GXHostSetLightingEnabled(GXBool enabled) {
    g_lightingEnabled = enabled ? 1 : 0;
}

void GXHostSetLightParams(f32 dx, f32 dy, f32 dz, f32 ambient) {
    gx_tev_set_light_params(dx, dy, dz, ambient);
}

void GXHostSetExposure(f32 gain) {
    gx_tev_set_exposure(gain);
}

/* Map a GXTexMtx slot enum to a 0-based slot index (GX_TEXMTX0 = slot 0,
 * GX_TEXMTX1 = slot 1, ..., GX_IDENTITY = -1 = no matrix). */
static int GXTexMtxSlot(GXTexMtx mtx) {
    if (mtx == GX_IDENTITY) return -1;
    if (mtx < GX_TEXMTX0 || mtx > GX_TEXMTX7) return -1;
    return (int)((mtx - GX_TEXMTX0) / 3);
}

void GXSetTexCoordGen2(GXTexCoordID dst_coord, GXTexGenType func,
                       GXTexGenSrc src_param, GXTexMtx mtx,
                       GXBool normalize, u32 pt_mtx) {
    int mtxSlot;
    (void)func; (void)src_param; (void)normalize; (void)pt_mtx;

    /* Map the named texture matrix to a 0-based texcoord slot so that the
     * vertex shader applies the correct SRT to each generated texcoord.
     * When mtx == GX_IDENTITY, the corresponding slot stays identity. */
    if ((u32)dst_coord >= 8) return;
    mtxSlot = GXTexMtxSlot(mtx);
    if (mtxSlot >= 0) {
        /* Associate this texcoord slot with the matrix slot index.  If the
         * slot differs from dst_coord (GCN allows remapping), copy the matrix
         * from the source slot so the dst_coord slot is always authoritative
         * in the shader. */
        if (mtxSlot != (int)(u32)dst_coord) {
            /* Copy current matrix from mtxSlot to dst_coord. */
            memcpy(g_texMatrixStore[dst_coord], g_texMatrixStore[mtxSlot],
                   sizeof(g_texMatrixStore[0]));
        }
        gx_tev_set_tex_matrix((u32)dst_coord, g_texMatrixStore[dst_coord]);
    } else {
        /* GX_IDENTITY: reset this texcoord slot to identity. */
        gx_tev_set_tex_matrix((u32)dst_coord, (const f32(*)[4])0);
    }
}

void GXHostSetTexMatrix(u32 slot, const f32 m[3][4]) {
    if (slot >= 8) return;
    if (m != (const f32(*)[4])0) {
        memcpy(g_texMatrixStore[slot], m, sizeof(g_texMatrixStore[slot]));
    } else {
        /* NULL -> reset to identity. */
        memset(g_texMatrixStore[slot], 0, sizeof(g_texMatrixStore[slot]));
        g_texMatrixStore[slot][0][0] = 1.0f;
        g_texMatrixStore[slot][1][1] = 1.0f;
        g_texMatrixStore[slot][2][2] = 1.0f;
    }
    gx_tev_set_tex_matrix(slot, g_texMatrixStore[slot]);
}

void GXHostEnableOffscreenRender(u32 width, u32 height) {
    GLuint colorTex;
    GLuint depthRbo;
    GLuint fbo;
    GLenum status;

    if (width == 0u || height == 0u) {
        return;
    }

    GXEnsureCurrentContext();

    if (g_offscreenEnabled &&
        g_offscreenWidth == width &&
        g_offscreenHeight == height) {
        GXBindOffscreenRenderTarget();
        return;
    }

    GXDestroyOffscreenRenderTarget();

    if (g_glGenFramebuffers == NULL || g_glBindFramebuffer == NULL ||
        g_glDeleteFramebuffers == NULL || g_glFramebufferTexture2D == NULL ||
        g_glGenRenderbuffers == NULL || g_glBindRenderbuffer == NULL ||
        g_glDeleteRenderbuffers == NULL || g_glRenderbufferStorage == NULL ||
        g_glFramebufferRenderbuffer == NULL || g_glCheckFramebufferStatus == NULL) {
        fprintf(stderr, "[gx_shim] offscreen GL functions unavailable\n");
        return;
    }

    g_glGenFramebuffers(1, &fbo);
    g_glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)width, (GLsizei)height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    g_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_2D, colorTex, 0);

    g_glGenRenderbuffers(1, &depthRbo);
    g_glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
    g_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                            (GLsizei)width, (GLsizei)height);
    g_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                GL_RENDERBUFFER, depthRbo);

    status = g_glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "[gx_shim] offscreen framebuffer incomplete: 0x%X\n",
                (unsigned)status);
        g_glBindFramebuffer(GL_FRAMEBUFFER, 0);
        g_glDeleteRenderbuffers(1, &depthRbo);
        glDeleteTextures(1, &colorTex);
        return;
    }

    g_offscreenFbo = fbo;
    g_offscreenColorTex = colorTex;
    g_offscreenDepthRbo = depthRbo;
    g_offscreenWidth = width;
    g_offscreenHeight = height;
    g_offscreenEnabled = 1;

    GXBindOffscreenRenderTarget();
    if (getenv("PCPORT_GL_TRACE") != NULL) {
        fprintf(stderr, "[gx_shim] offscreen render target enabled %ux%u\n",
                (unsigned)width, (unsigned)height);
    }
}

/* =========================================================================
 * 4. TEV / Blend / Alpha / Z / Fog State
 * ========================================================================= */

void GXSetTevOp(GXTevStageID stage, GXTevMode mode) {
    if ((u32)stage >= GX_MAX_TEVSTAGE) return;

    /* TODO: Phase 3c -- TEV preset to shader variant
     *
     * GXSetTevOp is a convenience function that sets both colorIn and
     * alphaIn for a TEV stage based on the preset mode:
     *
     * GX_MODULATE:
     *   color = tex * ras
     *   alpha = tex_a * ras_a
     *
     * GX_DECAL:
     *   color = lerp(ras, tex, tex_a)
     *   alpha = ras_a
     *
     * GX_BLEND:
     *   color = lerp(ras, tex, ras_a)  [approximation]
     *   alpha = tex_a * ras_a
     *
     * GX_REPLACE:
     *   color = tex
     *   alpha = tex_a
     *
     * GX_PASSCLR:
     *   color = ras
     *   alpha = ras_a
     *
     * Store the mode in g_tevState for the shader generator,
     * then mark the shader as dirty to trigger recompilation.
     */

    g_tevState.stages[stage].tevMode = mode;
    g_tevState.dirty = 1;
}

void GXSetTevColorIn(GXTevStageID stage,
                     GXTevColorArg a, GXTevColorArg b,
                     GXTevColorArg c, GXTevColorArg d) {
    if ((u32)stage >= GX_MAX_TEVSTAGE) return;

    /* TODO: Phase 3c -- Store TEV color input configuration
     *
     * Record the four color input sources for this TEV stage.
     * These will be read by the shader generator to emit GLSL code.
     */

    g_tevState.stages[stage].colorIn[0] = a;
    g_tevState.stages[stage].colorIn[1] = b;
    g_tevState.stages[stage].colorIn[2] = c;
    g_tevState.stages[stage].colorIn[3] = d;
    g_tevState.dirty = 1;
}

void GXSetTevAlphaIn(GXTevStageID stage,
                     GXTevAlphaArg a, GXTevAlphaArg b,
                     GXTevAlphaArg c, GXTevAlphaArg d) {
    if ((u32)stage >= GX_MAX_TEVSTAGE) return;

    /* TODO: Phase 3c -- Store TEV alpha input configuration */

    g_tevState.stages[stage].alphaIn[0] = a;
    g_tevState.stages[stage].alphaIn[1] = b;
    g_tevState.stages[stage].alphaIn[2] = c;
    g_tevState.stages[stage].alphaIn[3] = d;
    g_tevState.dirty = 1;
}

void GXSetTevColorOp(GXTevStageID stage, GXTevOp op,
                     GXTevBias bias, GXTevScale scale,
                     GXBool clamp, GXTevRegID out_reg) {
    if ((u32)stage >= GX_MAX_TEVSTAGE) return;

    /* TODO: Phase 3c -- Store TEV color operation
     *
     * The TEV color combiner computes:
     *   result = (d + mix(a, b, c)) * scale + bias
     * Where op selects ADD or SUB, and scale/bias are modifiers.
     *
     * clamp: if true, clamp result to [0,1]
     * out_reg: which register receives the result (PREV, REG0-2)
     */

    g_tevState.stages[stage].colorOp = op;
    g_tevState.stages[stage].colorBias = bias;
    g_tevState.stages[stage].colorScale = scale;
    g_tevState.stages[stage].colorClamp = clamp;
    g_tevState.stages[stage].colorOutReg = out_reg;
    g_tevState.dirty = 1;
}

void GXSetTevAlphaOp(GXTevStageID stage, GXTevOp op,
                     GXTevBias bias, GXTevScale scale,
                     GXBool clamp, GXTevRegID out_reg) {
    if ((u32)stage >= GX_MAX_TEVSTAGE) return;

    /* TODO: Phase 3c -- Store TEV alpha operation (same as colorOp) */

    g_tevState.stages[stage].alphaOp = op;
    g_tevState.stages[stage].alphaBias = bias;
    g_tevState.stages[stage].alphaScale = scale;
    g_tevState.stages[stage].alphaClamp = clamp;
    g_tevState.stages[stage].alphaOutReg = out_reg;
    g_tevState.dirty = 1;
}

void GXSetTevColor(GXTevRegID id, GXColor color) {
    if ((u32)id > 3) return;
    g_tevColorRegs[id] = color;

    /* TODO: Phase 3c -- Upload TEV color register to shader
     *
     * glUniform4f(u_tevColor_loc[id],
     *             color.r / 255.0f, color.g / 255.0f,
     *             color.b / 255.0f, color.a / 255.0f);
     */
}

void GXSetTevKColor(GXTevRegID id, GXColor color) {
    if ((u32)id > 3) return;
    g_tevKonstRegs[id] = color;

    /* TODO: Phase 3c -- Upload TEV konst color register to shader
     *
     * glUniform4f(u_tevKonst_loc[id],
     *             color.r / 255.0f, color.g / 255.0f,
     *             color.b / 255.0f, color.a / 255.0f);
     */
}

void GXSetNumTevStages(u8 nStages) {
    g_numTevStages = nStages;
    g_tevState.numStages = nStages;
    g_tevState.dirty = 1;

    /* TODO: Phase 3c -- This value is part of the shader cache key.
     * When the TEV state is flushed before a draw call, the number
     * of stages determines how many TEV stage loops to emit in GLSL.
     */
}

void GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord,
                   GXTexMapID map, GXChannelID color) {
    if ((u32)stage >= GX_MAX_TEVSTAGE) return;

    /* TODO: Phase 3c -- Store TEV order for shader generation
     *
     * This tells each TEV stage:
     *   - Which texcoord to use for sampling (coord)
     *   - Which texture map slot to sample (map)
     *   - Which rasterized color channel to use (color)
     */

    g_tevState.stages[stage].texCoordId = coord;
    g_tevState.stages[stage].texMapId = map;
    g_tevState.stages[stage].channelId = color;
    g_tevState.dirty = 1;
}

void GXSetTevKColorSel(GXTevStageID stage, u32 sel) {
    if ((u32)stage >= GX_MAX_TEVSTAGE) return;

    /* Records which konst color (or konst-color component) this stage reads
     * when a colorIn slot is GX_CC_KONST. Consumed by the TEV shader path
     * via GXHostGetTevKColorSel. */
    g_tevKColorSel[stage] = (u8)sel;
    g_tevState.dirty = 1;
}

void GXSetTevKAlphaSel(GXTevStageID stage, u32 sel) {
    if ((u32)stage >= GX_MAX_TEVSTAGE) return;

    /* Records which konst alpha (or konst-alpha component) this stage reads
     * when an alphaIn slot is GX_CA_KONST. Consumed by the TEV shader path
     * via GXHostGetTevKAlphaSel. */
    g_tevKAlphaSel[stage] = (u8)sel;
    g_tevState.dirty = 1;
}

void GXSetTevSwapModeTable(u32 id, u32 r, u32 g, u32 b, u32 a) {
    if (id >= GX_TEV_SWAP_TABLE_COUNT) return;

    /* Stores the RGBA channel-swap selectors for swap table 'id'. The TEV
     * path reads these to remap ras/tex channels before combining. Stored
     * functionally; the default identity table (R,G,B,A) is a pass-through. */
    g_tevSwapTable[id][0] = (u8)r;
    g_tevSwapTable[id][1] = (u8)g;
    g_tevSwapTable[id][2] = (u8)b;
    g_tevSwapTable[id][3] = (u8)a;
    g_tevState.dirty = 1;
}

void GXSetTevIndirect(GXTevStageID stage, u32 ind_stage, u32 format,
                      u32 bias_sel, u32 mtx_sel, u32 wrap_s, u32 wrap_t,
                      u32 add_prev, u32 utc_lod, u32 alpha_sel) {
    GXTevIndirectState* ind;

    if ((u32)stage >= GX_MAX_TEVSTAGE) return;

    /* Functional no-op: indirect texturing (bump/distortion) is not yet
     * emulated on the GL path. The parameters are recorded so the draw path
     * can later detect that a stage requested indirect lookups. */
    ind = &g_tevIndirect[stage];
    ind->active = 1;
    ind->indStage = (u8)ind_stage;
    ind->format = (u8)format;
    ind->biasSel = (u8)bias_sel;
    ind->mtxSel = (u8)mtx_sel;
    ind->wrapS = (u8)wrap_s;
    ind->wrapT = (u8)wrap_t;
    ind->addPrev = (u8)add_prev;
    ind->utcLod = (u8)utc_lod;
    ind->alphaSel = (u8)alpha_sel;
    g_tevState.dirty = 1;
}

void GXSetBlendMode(GXBlendMode type, GXBlendFactor src_factor,
                    GXBlendFactor dst_factor, GXLogicOp op) {
    GLenum src = GL_ONE;
    GLenum dst = GL_ONE;

    g_blendType = type;
    g_blendSrc = src_factor;
    g_blendDst = dst_factor;
    g_blendLogicOp = op;

    switch (src_factor) {
        case GX_BL_ZERO: src = GL_ZERO; break;
        case GX_BL_ONE: src = GL_ONE; break;
        case GX_BL_SRCCLR: src = GL_SRC_COLOR; break;
        case GX_BL_INVSRCCLR: src = GL_ONE_MINUS_SRC_COLOR; break;
        case GX_BL_SRCALPHA: src = GL_SRC_ALPHA; break;
        case GX_BL_INVSRCALPHA: src = GL_ONE_MINUS_SRC_ALPHA; break;
        case GX_BL_DSTALPHA: src = GL_DST_ALPHA; break;
        case GX_BL_INVDSTALPHA: src = GL_ONE_MINUS_DST_ALPHA; break;
    }

    switch (dst_factor) {
        case GX_BL_ZERO: dst = GL_ZERO; break;
        case GX_BL_ONE: dst = GL_ONE; break;
        case GX_BL_SRCCLR: dst = GL_SRC_COLOR; break;
        case GX_BL_INVSRCCLR: dst = GL_ONE_MINUS_SRC_COLOR; break;
        case GX_BL_SRCALPHA: dst = GL_SRC_ALPHA; break;
        case GX_BL_INVSRCALPHA: dst = GL_ONE_MINUS_SRC_ALPHA; break;
        case GX_BL_DSTALPHA: dst = GL_DST_ALPHA; break;
        case GX_BL_INVDSTALPHA: dst = GL_ONE_MINUS_DST_ALPHA; break;
    }

    if (type == GX_BM_NONE) {
        glDisable(GL_BLEND);
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(src, dst);
}

void GXSetAlphaCompare(GXCompare comp0, u8 ref0,
                       GXAlphaOp op,
                       GXCompare comp1, u8 ref1) {
    g_alphaComp0 = comp0;
    g_alphaRef0 = ref0;
    g_alphaOp = op;
    g_alphaComp1 = comp1;
    g_alphaRef1 = ref1;

    /* TODO: Phase 3e -- Alpha test in fragment shader
     *
     * OpenGL 3.3 core has no fixed-function alpha test.
     * Set uniforms for the fragment shader's alpha test logic:
     *
     * glUniform1i(u_alphaComp0_loc, comp0);
     * glUniform1f(u_alphaRef0_loc, ref0 / 255.0f);
     * glUniform1i(u_alphaOp_loc, op);
     * glUniform1i(u_alphaComp1_loc, comp1);
     * glUniform1f(u_alphaRef1_loc, ref1 / 255.0f);
     *
     * The fragment shader uses:
     *   if (!alphaTest(fragColor.a)) discard;
     */
}

void GXSetZMode(GXBool compare_enable, GXCompare func,
                GXBool update_enable) {
    g_zEnable = compare_enable;
    g_zFunc = func;
    g_zUpdate = update_enable;

    if (compare_enable) {
        GLenum depthFunc = GL_LEQUAL;

        switch (func) {
            case GX_NEVER: depthFunc = GL_NEVER; break;
            case GX_LESS: depthFunc = GL_LESS; break;
            case GX_EQUAL: depthFunc = GL_EQUAL; break;
            case GX_LEQUAL: depthFunc = GL_LEQUAL; break;
            case GX_GREATER: depthFunc = GL_GREATER; break;
            case GX_NEQUAL: depthFunc = GL_NOTEQUAL; break;
            case GX_GEQUAL: depthFunc = GL_GEQUAL; break;
            case GX_ALWAYS: depthFunc = GL_ALWAYS; break;
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(depthFunc);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    glDepthMask(update_enable ? GL_TRUE : GL_FALSE);
}

void GXSetZCompLoc(GXBool before_tex) {
    (void)before_tex;

    /* TODO: Phase 3e -- Z comparison location
     *
     * before_tex=1: Depth test happens before texture lookup (early Z).
     *   In the fragment shader, discard based on alpha AFTER depth write.
     * before_tex=0: Depth test happens after texture lookup.
     *   In the fragment shader, discard based on alpha BEFORE depth write.
     *
     * This affects the ordering of the discard statement relative to
     * the depth write in the generated fragment shader.
     */
}

void GXSetFog(GXFogType type, f32 startz, f32 endz,
              f32 nearz, f32 farz, GXColor color) {
    g_fogType = type;
    g_fogStart = startz;
    g_fogEnd = endz;
    g_fogNear = nearz;
    g_fogFar = farz;
    g_fogColor = color;

    /* Push the fog state into the TEV shader render state. The shader computes
     * linear fog (GX_FOG_PERSP_LIN) from the eye-space distance; GX_FOG_NONE
     * disables it. Uploaded per-draw in gx_tev_bind. */
    gx_tev_set_fog((u8)type, startz, endz,
                   color.r, color.g, color.b, color.a);
}

void GXSetCullMode(GXCullMode mode) {
    g_cullMode = mode;

    if (mode == GX_CULL_NONE) {
        glDisable(GL_CULL_FACE);
        return;
    }

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    if (mode == GX_CULL_FRONT) {
        glCullFace(GL_FRONT);
    } else if (mode == GX_CULL_BACK) {
        glCullFace(GL_BACK);
    } else {
        glCullFace(GL_FRONT_AND_BACK);
    }
}

/* Map a GXChannelID to a 0/1 color-channel index, or -1 if not a color
 * channel this basic path handles. COLOR0/COLOR0A0 -> 0, COLOR1/COLOR1A1 -> 1. */
static int GXChanColorIndex(GXChannelID chan) {
    switch (chan) {
        case GX_COLOR0:
        case GX_COLOR0A0:
            return 0;
        case GX_COLOR1:
        case GX_COLOR1A1:
            return 1;
        default:
            return -1;
    }
}

void GXSetChanCtrl(GXChannelID chan, GXBool enable,
                   GXColorSrc amb_src, GXColorSrc mat_src,
                   u32 light_mask, GXDiffuseFn diff_fn,
                   GXAttnFn attn_fn) {
    int idx;
    (void)amb_src; (void)light_mask; (void)diff_fn; (void)attn_fn;

    /* Basic GX lighting channel: track per-channel enable + material source.
     * The host shader path replaces the rasterized color with the register
     * material color modulated by the register ambient color when the channel
     * is enabled with mat_src == GX_SRC_REG (the common single-channel case).
     * Per-light dynamic lighting (light objects, attenuation, diffuse fn) is
     * not yet implemented -- this covers the ambient+material flat-shaded case. */
    idx = GXChanColorIndex(chan);
    if (idx < 0) return;

    g_chanCtrlEnable[idx] = enable ? 1 : 0;
    g_chanMatSrc[idx] = (u8)mat_src;

    /* Drive the host channel lighting off color channel 0 (the channel the
     * field/title geometry rasterizes through). Active only when enabled and
     * the material color comes from the register. */
    gx_tev_set_chan_lighting(g_chanCtrlEnable[0] &&
                             g_chanMatSrc[0] == GX_SRC_REG);
}

void GXSetChanAmbColor(GXChannelID chan, GXColor color) {
    int idx = GXChanColorIndex(chan);
    if (idx < 0) return;

    g_chanAmbColor[idx] = color;
    if (idx == 0) {
        gx_tev_set_chan_amb_color(color.r, color.g, color.b, color.a);
    }
}

void GXSetChanMatColor(GXChannelID chan, GXColor color) {
    int idx = GXChanColorIndex(chan);
    if (idx < 0) return;

    g_chanMatColor[idx] = color;
    if (idx == 0) {
        gx_tev_set_chan_mat_color(color.r, color.g, color.b, color.a);
    }
}

/* =========================================================================
 * 5. Texture State
 * ========================================================================= */

void GXHostClearTextureBinding(void) {
    g_boundTextureId = 0;
    g_boundTextureMap = GX_TEXMAP_NULL;

    GXEnsureCurrentContext();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

static int GXUploadHostTexture(GXTexObj* obj,
                               u16 width,
                               u16 height,
                               GXTexFmt format,
                               GXTexWrapMode wrap_s,
                               GXTexWrapMode wrap_t,
                               GXBool mipmap,
                               GLenum glInternalFormat,
                               GLenum glFormat,
                               GLenum glType,
                               const void* pixels) {
    GXHostTexObj* hostObj;

    if (obj == NULL || pixels == NULL || width == 0 || height == 0) {
        return 0;
    }

    GXEnsureCurrentContext();
    hostObj = GXGetHostTexObj(obj);

    glGenTextures(1, (GLuint*)&hostObj->glTexId);
    if (hostObj->glTexId == 0) {
        memset(obj, 0, sizeof(*obj));
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, hostObj->glTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    (GLint)GXTranslateWrapMode(wrap_s));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    (GLint)GXTranslateWrapMode(wrap_t));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    (GLint)GXTranslateMinFilter(GX_LINEAR, mipmap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    (GLint)GXTranslateMagFilter(GX_LINEAR));
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 (GLint)glInternalFormat,
                 width,
                 height,
                 0,
                 glFormat,
                 glType,
                 pixels);
    hostObj->magic = GX_HOST_TEXOBJ_MAGIC;
    hostObj->width = width;
    hostObj->height = height;
    hostObj->format = format;
    hostObj->wrapS = (u8)wrap_s;
    hostObj->wrapT = (u8)wrap_t;
    hostObj->mipmap = (u8)mipmap;
    hostObj->minFilt = (u8)GX_LINEAR;
    hostObj->magFilt = (u8)GX_LINEAR;
    return 1;
}

void GXInitTexObj(GXTexObj* obj, void* image,
                  u16 width, u16 height, GXTexFmt format,
                  GXTexWrapMode wrap_s, GXTexWrapMode wrap_t,
                  GXBool mipmap) {
    GXDecodedTexture decoded;

    if (obj == NULL || image == NULL || width == 0 || height == 0) {
        return;
    }

    memset(&decoded, 0, sizeof(decoded));
    memset(obj, 0, sizeof(*obj));

    if (gx_texture_decode(image,
                          width,
                          height,
                          format,
                          NULL,
                          (GXTlutFmt)0,
                          0,
                          &decoded) != 0) {
        return;
    }
    GXUploadHostTexture(obj,
                        width,
                        height,
                        format,
                        wrap_s,
                        wrap_t,
                        mipmap,
                        decoded.glInternalFormat,
                        decoded.glFormat,
                        decoded.glType,
                        decoded.data);
    gx_texture_free(&decoded);
}

void GXHostInitTexObjRGBA8(GXTexObj* obj, const void* rgba,
                           u16 width, u16 height,
                           GXTexWrapMode wrap_s, GXTexWrapMode wrap_t) {
    if (obj == NULL || rgba == NULL) {
        return;
    }

    memset(obj, 0, sizeof(*obj));
    GXUploadHostTexture(obj,
                        width,
                        height,
                        GX_TF_RGBA8,
                        wrap_s,
                        wrap_t,
                        GX_FALSE,
                        GL_RGBA8,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        rgba);
}

void GXHostUpdateTexObjRGBA8(GXTexObj* obj, const void* rgba,
                             u16 width, u16 height) {
    GXHostTexObj* hostObj;

    if (obj == NULL || rgba == NULL || width == 0 || height == 0) {
        return;
    }
    hostObj = GXGetHostTexObj(obj);

    /* First use, dimension change, or a stale object -> (re)create the texture.
     * Otherwise reuse the existing GL texture and just replace its pixels, so a
     * per-frame video update does not leak one GL texture per frame. */
    if (hostObj->magic != GX_HOST_TEXOBJ_MAGIC || hostObj->glTexId == 0 ||
        hostObj->width != width || hostObj->height != height) {
        GXHostInitTexObjRGBA8(obj, rgba, width, height, GX_CLAMP, GX_CLAMP);
        return;
    }

    GXEnsureCurrentContext();
    glBindTexture(GL_TEXTURE_2D, hostObj->glTexId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                    GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glBindTexture(GL_TEXTURE_2D, g_boundTextureId);
}

void GXInitTexObjFilterMode(GXTexObj* obj, GXTexFilter min_filt,
                            GXTexFilter mag_filt) {
    GXHostTexObj* hostObj;

    if (obj == NULL) {
        return;
    }

    hostObj = GXGetHostTexObj(obj);
    if (hostObj->magic != GX_HOST_TEXOBJ_MAGIC || hostObj->glTexId == 0) {
        return;
    }

    hostObj->minFilt = (u8)min_filt;
    hostObj->magFilt = (u8)mag_filt;

    GXEnsureCurrentContext();
    glBindTexture(GL_TEXTURE_2D, hostObj->glTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    (GLint)GXTranslateMinFilter(min_filt,
                                                (GXBool)hostObj->mipmap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    (GLint)GXTranslateMagFilter(mag_filt));
    glBindTexture(GL_TEXTURE_2D, g_boundTextureId);
}

void GXInitTexObjLOD(GXTexObj* obj, GXTexFilter min_filt, GXTexFilter mag_filt,
                     f32 min_lod, f32 max_lod, f32 lod_bias,
                     GXBool bias_clamp, GXBool do_edge_lod, u8 max_aniso) {
    GXHostTexObj* hostObj;

    (void)bias_clamp;
    (void)do_edge_lod;
    (void)max_aniso;

    if (obj == NULL) {
        return;
    }

    hostObj = GXGetHostTexObj(obj);
    if (hostObj->magic != GX_HOST_TEXOBJ_MAGIC || hostObj->glTexId == 0) {
        return;
    }

    hostObj->minFilt = (u8)min_filt;
    hostObj->magFilt = (u8)mag_filt;

    GXEnsureCurrentContext();
    glBindTexture(GL_TEXTURE_2D, hostObj->glTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    (GLint)GXTranslateMinFilter(min_filt,
                                                (GXBool)hostObj->mipmap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    (GLint)GXTranslateMagFilter(mag_filt));
#ifdef GL_TEXTURE_MIN_LOD
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, (GLfloat)min_lod);
#endif
#ifdef GL_TEXTURE_MAX_LOD
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, (GLfloat)max_lod);
#endif
#ifdef GL_TEXTURE_LOD_BIAS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, (GLfloat)lod_bias);
#else
    (void)lod_bias;
#endif
    glBindTexture(GL_TEXTURE_2D, g_boundTextureId);
}

void GXInitTexObjWrapMode(GXTexObj* obj, GXTexWrapMode wrap_s,
                          GXTexWrapMode wrap_t) {
    GXHostTexObj* hostObj;

    if (obj == NULL) {
        return;
    }

    hostObj = GXGetHostTexObj(obj);
    if (hostObj->magic != GX_HOST_TEXOBJ_MAGIC || hostObj->glTexId == 0) {
        return;
    }

    hostObj->wrapS = (u8)wrap_s;
    hostObj->wrapT = (u8)wrap_t;

    GXEnsureCurrentContext();
    glBindTexture(GL_TEXTURE_2D, hostObj->glTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    (GLint)GXTranslateWrapMode(wrap_s));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    (GLint)GXTranslateWrapMode(wrap_t));
    glBindTexture(GL_TEXTURE_2D, g_boundTextureId);
}

void GXInitTlutObj(GXTlutObj* obj, void* lut,
                   GXTlutFmt fmt, u16 n_entries) {
    (void)obj; (void)lut; (void)fmt; (void)n_entries;

    /* TODO: Phase 3d -- Decode TLUT on CPU
     *
     * There is no GL equivalent for hardware palette lookups.
     * Store the palette data pointer inside the GXTlutObj so that
     * GXInitTexObj can use it for CI4/CI8/CI14x2 texture decoding.
     *
     * The palette entries are in GXTlutFmt format:
     *   GX_TL_IA8    -> 16-bit intensity+alpha
     *   GX_TL_RGB565 -> 16-bit RGB
     *   GX_TL_RGB5A3 -> 16-bit RGB with 1-bit alpha mode flag
     */
}

void GXLoadTexObj(GXTexObj* obj, GXTexMapID id) {
    const GXHostTexObj* hostObj;

    if (obj == NULL || id == GX_TEXMAP_NULL) {
        GXHostClearTextureBinding();
        return;
    }

    hostObj = GXGetConstHostTexObj(obj);
    if (hostObj->magic != GX_HOST_TEXOBJ_MAGIC || hostObj->glTexId == 0) {
        GXHostClearTextureBinding();
        return;
    }

    GXEnsureCurrentContext();
    glBindTexture(GL_TEXTURE_2D, hostObj->glTexId);
    g_boundTextureId = hostObj->glTexId;
    g_boundTextureMap = id;
}

void GXInvalidateTexAll(void) {
    /* No-op on PC -- texture cache is always coherent. */
}

void GXCopyTex(void* dest, GXBool clear) {
    (void)dest; (void)clear;

    /* TODO: Phase 3f -- EFB copy to texture (render-to-texture)
     *
     * This is used by GStextureUploadFromBuffer for render-to-texture
     * effects (blur, distortion, aura).
     *
     * Implementation:
     * 1. Bind the FBO that was set up as the render target
     * 2. Use glCopyTexSubImage2D or glBlitFramebuffer to copy
     *    the FBO color attachment to the destination texture
     * 3. If clear==GX_TRUE, clear the FBO after copy
     *
     * The 'dest' pointer on GCN points to main RAM; on PC, map it
     * to the corresponding GL texture ID.
     */
}

void GXSetTexCopySrc(u16 left, u16 top, u16 wd, u16 ht) {
    (void)left; (void)top; (void)wd; (void)ht;

    /* TODO: Phase 3f -- Configure FBO blit source region
     *
     * Store the source rectangle for the next GXCopyTex call.
     * Used by GStextureUploadFromBuffer to specify which part
     * of the EFB to copy.
     */
}

void GXPixModeSync(void) {
    /* TODO: Phase 3f -- Pipeline synchronization
     *
     * glFinish(); // or glMemoryBarrier if using compute shaders
     *
     * This is rarely needed on PC since GL driver handles sync.
     * Only call glFinish() if we see rendering artifacts from
     * missing synchronization.
     */
}

/* =========================================================================
 * 6. Lighting
 * ========================================================================= */

void GXInitLightObj(GXLightObj* obj, GXColor color,
                    f32 px, f32 py, f32 pz) {
    (void)obj; (void)color; (void)px; (void)py; (void)pz;

    /* TODO: Phase 3e -- Initialize light parameters
     *
     * Store position and color in the GXLightObj structure.
     * The 64-byte opaque struct can be repurposed to hold:
     *   - vec3 position (12 bytes)
     *   - vec3 direction (12 bytes)
     *   - vec4 color (16 bytes)
     *   - attenuation params (12 bytes)
     *   - spot params (12 bytes)
     */
}

void GXInitLightDir(GXLightObj* obj, f32 nx, f32 ny, f32 nz) {
    (void)obj; (void)nx; (void)ny; (void)nz;
    /* TODO: Phase 3e -- Store light direction in GXLightObj */
}

void GXInitLightDistAttn(GXLightObj* obj, f32 ref_dist,
                         f32 ref_brightness, GXDiffuseFn fn) {
    (void)obj; (void)ref_dist; (void)ref_brightness; (void)fn;
    /* TODO: Phase 3e -- Store distance attenuation parameters */
}

void GXInitLightSpot(GXLightObj* obj, f32 cutoff, u32 spot_fn) {
    (void)obj; (void)cutoff; (void)spot_fn;
    /* TODO: Phase 3e -- Store spotlight parameters */
}

void GXInitSpecularDir(GXLightObj* obj, f32 nx, f32 ny, f32 nz) {
    (void)obj; (void)nx; (void)ny; (void)nz;
    /* TODO: Phase 3e -- Store specular direction */
}

void GXInitLightShininess(GXLightObj* obj, f32 shininess) {
    (void)obj; (void)shininess;
    /* TODO: Phase 3e -- Store shininess exponent */
}

void GXLoadLightObj(GXLightObj* obj, GXLightID id) {
    (void)obj; (void)id;

    /* TODO: Phase 3e -- Upload light to shader
     *
     * Determine the light index from id (log2 of the bitmask):
     *   int idx = __builtin_ctz(id);
     *
     * Upload the light struct from GXLightObj to the uniform:
     *   glUniform3f(u_lights_pos_loc[idx], pos.x, pos.y, pos.z);
     *   glUniform3f(u_lights_dir_loc[idx], dir.x, dir.y, dir.z);
     *   glUniform4f(u_lights_color_loc[idx], r, g, b, a);
     *   glUniform1f(u_lights_cosAtten_loc[idx], cosAtten);
     *   glUniform1f(u_lights_distAtten_loc[idx], distAtten);
     */
}

/* =========================================================================
 * 7. Draw Commands
 * ========================================================================= */

/*
 * Push the tracked GX state into the gx_tev shader backend and draw via the
 * modern TEV->GLSL path. Returns 1 on success, 0 if the shader path is not
 * ready or gx_tev_submit failed (caller then falls back to fixed-function).
 *
 * 'drawVertices'/'drawCount' are the already-expanded (quad->tri) immediate
 * vertices; the GXImmVertex layout is binary-compatible with GXTevVertex.
 */
static int GXSubmitViaShader(GLenum glPrim,
                             const GXImmVertex* drawVertices,
                             u32 drawCount,
                             GXTevMode tevMode) {
    u32 currentMtx;
    int useTexture;
    int submitted;

    if (!g_tevPathReady) {
        return 0;
    }

    /* Matrices: projection + the active modelview slot (g_currentMtxId). */
    gx_tev_set_proj_matrix(g_projMatrix);
    currentMtx = (g_currentMtxId < 10) ? g_currentMtxId : 0;
    gx_tev_set_modelview_matrix(g_posMtx[currentMtx]);
    gx_tev_set_normal_matrix(g_nrmMtx[currentMtx]);

    /* Directional lighting gate: only geometry that opted in (3D scene draws)
     * is shaded; 2D overlays leave it off so they stay full-bright. */
    gx_tev_set_lighting_enabled(g_lightingEnabled);

    /* TEV color + konst registers (PREV/REG0..2, K0..K3). */
    {
        u32 i;
        for (i = 0; i < 4; ++i) {
            gx_tev_set_tev_color(i, g_tevColorRegs[i].r, g_tevColorRegs[i].g,
                                 g_tevColorRegs[i].b, g_tevColorRegs[i].a);
            gx_tev_set_konst_color(i, g_tevKonstRegs[i].r, g_tevKonstRegs[i].g,
                                   g_tevKonstRegs[i].b, g_tevKonstRegs[i].a);
        }
    }

    /* Per-stage konst color/alpha selectors (GXSetTevKColorSel/KAlphaSel).
     * Passed to the shader as a per-draw uniform array so the shader cache is
     * keyed only on the combiner structure, not the konst selection. */
    {
        u32 i;
        for (i = 0; i < GX_MAX_TEVSTAGE && i < (u32)GX_TEV_MAX_STAGES; ++i) {
            gx_tev_set_konst_sel(i, g_tevKColorSel[i], g_tevKAlphaSel[i]);
        }
    }

    /* Alpha compare (shader discard) + host vertex alpha scale.
     *
     * The legacy fixed-function draw path never implemented alpha testing
     * (Phase-3e TODO), so it always rendered. The GSgfx host render state for
     * these draws carries the uninitialized default comp0=GX_NEVER (paired with
     * op=AND, comp1=GX_LESS), which is a statically "never pass" test. Honoring
     * a literal GX_NEVER would discard every fragment and render a black frame,
     * regressing the working render. A real GX_NEVER is meaningless on console
     * too, so its only source here is the uninitialized state; substitute
     * GX_ALWAYS so the whole test becomes a no-op. The uninitialized GSgfx
     * default is the pair (comp0=GX_NEVER, comp1=GX_LESS), which always
     * discards; keying off comp0==GX_NEVER disables both comparators. Genuine
     * alpha tests (GX_GREATER, GX_GEQUAL, GX_LESS, etc. used for cutout
     * textures) leave comp0 as a real comparator and are fully honored. */
    if (g_alphaComp0 == GX_NEVER) {
        gx_tev_set_alpha_compare((u8)GX_ALWAYS, 0, (u8)GX_AOP_AND,
                                 (u8)GX_ALWAYS, 0);
    } else {
        gx_tev_set_alpha_compare((u8)g_alphaComp0, g_alphaRef0, (u8)g_alphaOp,
                                 (u8)g_alphaComp1, g_alphaRef1);
    }
    gx_tev_set_vertex_alpha_scale(g_vertexAlphaScale);

    useTexture = g_boundTextureId != 0 &&
                 g_numTexGens != 0 &&
                 tevMode != GX_PASSCLR;

    submitted = gx_tev_submit(&g_tevState, (u32)glPrim,
                              (const GXTevVertex*)drawVertices, drawCount,
                              (u32)g_boundTextureId, useTexture);
    return submitted;
}

static int GXSubmitVertices(GXPrimitive primType,
                            const GXImmVertex* sourceVertices,
                            u32 sourceCount,
                            u32* outExpandedCount) {
    GLenum glPrim;
    GXTevMode tevMode = (GXTevMode)g_tevState.stages[0].tevMode;
    const GXImmVertex* drawVertices = sourceVertices;
    u32 drawCount = sourceCount;
    u32 i;
    int useTexture;

    if (outExpandedCount != NULL) {
        *outExpandedCount = 0;
    }

    GXEnsureCurrentContext();
    glReadBuffer(GL_BACK);

    switch (primType) {
        case GX_QUADS:
            glPrim = GL_TRIANGLES;
            if ((drawCount % 4) != 0 || drawCount == 0) {
                return 0;
            }
            if (((drawCount / 4) * 6) >
                (sizeof(g_immExpandedVertices) / sizeof(g_immExpandedVertices[0]))) {
                return 0;
            }
            for (i = 0; i < drawCount; i += 4) {
                u32 out = (i / 4) * 6;

                g_immExpandedVertices[out + 0] = sourceVertices[i + 0];
                g_immExpandedVertices[out + 1] = sourceVertices[i + 1];
                g_immExpandedVertices[out + 2] = sourceVertices[i + 2];
                g_immExpandedVertices[out + 3] = sourceVertices[i + 0];
                g_immExpandedVertices[out + 4] = sourceVertices[i + 2];
                g_immExpandedVertices[out + 5] = sourceVertices[i + 3];
            }
            drawVertices = g_immExpandedVertices;
            drawCount = (drawCount / 4) * 6;
            break;
        case GX_TRIANGLES:
            glPrim = GL_TRIANGLES;
            break;
        case GX_TRIANGLESTRIP:
            glPrim = GL_TRIANGLE_STRIP;
            break;
        case GX_TRIANGLEFAN:
            glPrim = GL_TRIANGLE_FAN;
            break;
        case GX_LINES:
            glPrim = GL_LINES;
            break;
        case GX_LINESTRIP:
            glPrim = GL_LINE_STRIP;
            break;
        case GX_POINTS:
            glPrim = GL_POINTS;
            break;
        default:
            return 0;
    }

    if (outExpandedCount != NULL) {
        *outExpandedCount = drawCount;
    }

    /* Modern TEV->GLSL path. The shader applies the vertex alpha scale itself
     * (u_vertexAlphaScale), so feed it the expanded-but-unmodulated vertices.
     * On success we are done; otherwise fall through to the legacy
     * fixed-function draw below (which pre-modulates alpha on the CPU). */
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
    if (GXSubmitViaShader(glPrim, drawVertices, drawCount, tevMode)) {
        glFlush();
        return 1;
    }

    /* Legacy fixed-function fallback. Drop any modern-GL program/VAO binding
     * left by a prior shader draw so fixed-function rendering is unaffected. */
    if (g_tevPathReady) {
        gx_tev_unbind();
    }

    if (g_vertexAlphaScale < 0.999f || g_vertexAlphaScale > 1.001f) {
        if (drawCount > (sizeof(g_modulatedVertices) / sizeof(g_modulatedVertices[0]))) {
            return 0;
        }

        memcpy(g_modulatedVertices,
               drawVertices,
               (size_t)drawCount * sizeof(g_modulatedVertices[0]));
        for (i = 0; i < drawCount; ++i) {
            u32 scaledAlpha =
                (u32)(((f32)g_modulatedVertices[i].color[3] * g_vertexAlphaScale) + 0.5f);

            if (scaledAlpha > 255u) {
                scaledAlpha = 255u;
            }
            g_modulatedVertices[i].color[3] = (u8)scaledAlpha;
        }

        drawVertices = g_modulatedVertices;
    }

    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
    useTexture = g_boundTextureId != 0 &&
                 g_numTexGens != 0 &&
                 tevMode != GX_PASSCLR;
    if (useTexture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_boundTextureId);
        glTexEnvi(GL_TEXTURE_ENV,
                  GL_TEXTURE_ENV_MODE,
                  (GLint)GXTranslateTevModeToEnvMode(tevMode));
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(GXImmVertex), &drawVertices[0].pos[0]);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(GXImmVertex), &drawVertices[0].color[0]);
    glTexCoordPointer(2, GL_FLOAT, sizeof(GXImmVertex), &drawVertices[0].texcoord[0]);
    glDrawArrays(glPrim, 0, (GLsizei)drawCount);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glFlush();
    return 1;
}

static int GXHasUnsupportedDisplayListAttr(void) {
    u32 attr;

    for (attr = 0; attr < GX_ATTR_STATE_MAX; ++attr) {
        if (g_vtxDescState[attr].type != GX_NONE &&
            !GXIsSupportedDisplayListAttr((GXAttr)attr)) {
            return 1;
        }
    }

    return 0;
}

void GXBegin(GXPrimitive type, GXVtxFmt vtxfmt, u16 nverts) {
    (void)vtxfmt;
    GXEnsureCurrentContext();
    glDrawBuffer(GL_BACK);
    g_immPrimType = type;
    g_immExpectedVerts = nverts;
    g_immVertexCount = 0;

    /* TODO: Phase 3b -- Begin immediate-mode vertex accumulation
     *
     * Before drawing, flush the current TEV/blend/Z state:
     * 1. If g_tevState.dirty, compile or look up the GLSL shader
     *    from the TEV state cache (gx_tev_compile)
     * 2. Bind the shader program
     * 3. Set all uniforms (matrices, TEV regs, blend/alpha/Z/fog)
     * 4. Begin writing vertices into the dynamic VBO
     *
     * Reset the vertex write pointer to the start of the buffer.
     */
}

void GXEnd(void) {
    u32 expandedCount = 0;

    GXEnsureCurrentContext();
    g_lastSubmittedVertexCount = g_immVertexCount;
    g_lastSubmittedPrimitive = g_immPrimType;
    if (!GXSubmitVertices(g_immPrimType, g_immVertices, g_immVertexCount,
                          &expandedCount)) {
        g_immVertexCount = 0;
        g_immExpectedVerts = 0;
        return;
    }
    g_lastExpandedVertexCount = expandedCount;

    /* TODO: Phase 3b -- Flush immediate-mode vertices to OpenGL
     *
     * 1. Upload g_immVertices[0..g_immVertexCount-1] to the dynamic VBO:
     *    glBindBuffer(GL_ARRAY_BUFFER, g_immVBO);
     *    glBufferSubData(GL_ARRAY_BUFFER, 0,
     *                    g_immVertexCount * sizeof(GXImmVertex),
     *                    g_immVertices);
     *
     * 2. Translate the GX primitive type to GL:
     *    GLenum glPrim;
     *    switch (g_immPrimType) {
     *        case GX_TRIANGLES:     glPrim = GL_TRIANGLES; break;
     *        case GX_TRIANGLESTRIP: glPrim = GL_TRIANGLE_STRIP; break;
     *        case GX_TRIANGLEFAN:   glPrim = GL_TRIANGLE_FAN; break;
     *        case GX_LINES:         glPrim = GL_LINES; break;
     *        case GX_LINESTRIP:     glPrim = GL_LINE_STRIP; break;
     *        case GX_POINTS:        glPrim = GL_POINTS; break;
     *        case GX_QUADS:
     *            // Split each quad into 2 triangles
     *            convertQuadsToTriangles();
     *            glPrim = GL_TRIANGLES;
     *            break;
     *    }
     *
     * 3. Draw:
     *    glDrawArrays(glPrim, 0, g_immVertexCount);
     */

    g_immVertexCount = 0;
    g_immExpectedVerts = 0;
}

void GXPosition3f32(f32 x, f32 y, f32 z) {
    if (g_immVertexCount >= GX_IMM_VTX_MAX) return;

    /* TODO: Phase 3b -- Write position to immediate-mode buffer */
    g_immVertices[g_immVertexCount].pos[0] = x;
    g_immVertices[g_immVertexCount].pos[1] = y;
    g_immVertices[g_immVertexCount].pos[2] = z;
    g_immVertices[g_immVertexCount].normal[0] = 0.0f;
    g_immVertices[g_immVertexCount].normal[1] = 0.0f;
    g_immVertices[g_immVertexCount].normal[2] = 0.0f;
}

void GXColor4u8(u8 r, u8 g, u8 b, u8 a) {
    if (g_immVertexCount >= GX_IMM_VTX_MAX) return;

    /* TODO: Phase 3b -- Write color to immediate-mode buffer */
    g_immVertices[g_immVertexCount].color[0] = r;
    g_immVertices[g_immVertexCount].color[1] = g;
    g_immVertices[g_immVertexCount].color[2] = b;
    g_immVertices[g_immVertexCount].color[3] = a;
}

void GXTexCoord2f32(f32 s, f32 t) {
    if (g_immVertexCount >= GX_IMM_VTX_MAX) return;

    /* TODO: Phase 3b -- Write texcoord to immediate-mode buffer
     * After writing the texcoord, advance the vertex counter since
     * texcoord is typically the last attribute submitted per vertex.
     */
    g_immVertices[g_immVertexCount].texcoord[0] = s;
    g_immVertices[g_immVertexCount].texcoord[1] = t;
    g_immVertexCount++;
}

void GXNormal3f32(f32 x, f32 y, f32 z) {
    if (g_immVertexCount >= GX_IMM_VTX_MAX) return;

    g_immVertices[g_immVertexCount].normal[0] = x;
    g_immVertices[g_immVertexCount].normal[1] = y;
    g_immVertices[g_immVertexCount].normal[2] = z;
}

void GXCallDisplayList(void* list, u32 nbytes) {
    const GXHostDisplayList* hostList;
    const u8* cursor;
    const u8* end;
    u32 totalSubmitted = 0;
    u32 totalExpanded = 0;
    GXPrimitive lastPrimitive = GX_POINTS;
    u32 i;
    int dbgBounds = (getenv("PCPORT_PRIM_DEBUG") != NULL);
    float mnx = 1e30f, mny = 1e30f, mnz = 1e30f;
    float mxx = -1e30f, mxy = -1e30f, mxz = -1e30f;
    float ndcMnX = 1e30f, ndcMnY = 1e30f, ndcMxX = -1e30f, ndcMxY = -1e30f;
    float ndcMnZ = 1e30f, ndcMxZ = -1e30f;
    int nBehind = 0, nOnScreen = 0, nZok = 0;

    GXEnsureCurrentContext();

    if (list == NULL || nbytes == 0) {
        return;
    }

    hostList = (const GXHostDisplayList*)list;
    if (nbytes >= sizeof(GXHostDisplayList) &&
        hostList->magic == GX_HOST_DISPLAY_LIST_MAGIC &&
        hostList->vertices != NULL && hostList->vertexCount != 0) {
        u32 expandedCount = 0;

        g_lastSubmittedVertexCount = hostList->vertexCount;
        g_lastSubmittedPrimitive = (GXPrimitive)hostList->primitive;
        if (!GXSubmitVertices((GXPrimitive)hostList->primitive,
                              (const GXImmVertex*)hostList->vertices,
                              hostList->vertexCount,
                              &expandedCount)) {
            return;
        }
        g_lastExpandedVertexCount = expandedCount;
        return;
    }

    if (GXHasUnsupportedDisplayListAttr()) {
        return;
    }

    cursor = (const u8*)list;
    end = cursor + nbytes;

    while (cursor < end) {
        u8 cmd;
        GXPrimitive primType;
        GXVtxFmt vtxfmt;
        u16 vertexCount;
        u32 expandedCount = 0;

        if ((end - cursor) < 3) {
            return;
        }

        cmd = *cursor++;
        primType = (GXPrimitive)(cmd & 0xF8);
        vtxfmt = (GXVtxFmt)(cmd & 0x07);
        vertexCount = GXReadBE16(cursor);
        cursor += 2;

        if (getenv("PCPORT_PRIM_DEBUG") != NULL) {
            static int n = 0;
            if (n++ < 60) {
                fprintf(stderr, "[prim] type=0x%02X count=%u\n",
                        (unsigned)primType, (unsigned)vertexCount);
            }
        }

        if (vertexCount == 0 || vertexCount > GX_IMM_VTX_MAX ||
            vtxfmt >= GX_VTXFMT_STATE_MAX) {
            return;
        }

        for (i = 0; i < vertexCount; ++i) {
            GXImmVertex vertex;

            memset(&vertex, 0, sizeof(vertex));
            vertex.color[0] = 0xFF;
            vertex.color[1] = 0xFF;
            vertex.color[2] = 0xFF;
            vertex.color[3] = 0xFF;

            /* GC vertex-stream order: POS, NRM, CLR0, TEX0, TEX1. NRM must be
             * decoded between POS and CLR0 or skinned meshes (which carry a
             * normal index) desync the stream. */
            if (!GXDecodeIndexedAttr(&cursor, end, vtxfmt, GX_VA_POS, &vertex) ||
                !GXDecodeIndexedAttr(&cursor, end, vtxfmt, GX_VA_NRM, &vertex) ||
                !GXDecodeIndexedAttr(&cursor, end, vtxfmt, GX_VA_CLR0, &vertex) ||
                !GXDecodeIndexedAttr(&cursor, end, vtxfmt, GX_VA_TEX0, &vertex) ||
                !GXDecodeIndexedAttr(&cursor, end, vtxfmt, GX_VA_TEX1, &vertex)) {
                if (getenv("PCPORT_PRIM_DEBUG") != NULL) {
                    fprintf(stderr, "[prim] ABORT decode at vert %u/%u (cmd bytesLeft=%ld)\n",
                            (unsigned)i, (unsigned)vertexCount, (long)(end - cursor));
                }
                return;
            }

            if (dbgBounds) {
                const f32 (*M)[4] = g_posMtx[g_currentMtxId];
                const f32 (*P)[4] = g_projMatrix;
                f32 cx, cy, cz, clx, cly, clw;
                if (vertex.pos[0] < mnx) mnx = vertex.pos[0];
                if (vertex.pos[1] < mny) mny = vertex.pos[1];
                if (vertex.pos[2] < mnz) mnz = vertex.pos[2];
                if (vertex.pos[0] > mxx) mxx = vertex.pos[0];
                if (vertex.pos[1] > mxy) mxy = vertex.pos[1];
                if (vertex.pos[2] > mxz) mxz = vertex.pos[2];
                /* modelview (3x4) then projection (4x4) -> clip -> NDC */
                cx = M[0][0]*vertex.pos[0]+M[0][1]*vertex.pos[1]+M[0][2]*vertex.pos[2]+M[0][3];
                cy = M[1][0]*vertex.pos[0]+M[1][1]*vertex.pos[1]+M[1][2]*vertex.pos[2]+M[1][3];
                cz = M[2][0]*vertex.pos[0]+M[2][1]*vertex.pos[1]+M[2][2]*vertex.pos[2]+M[2][3];
                clx = P[0][0]*cx+P[0][1]*cy+P[0][2]*cz+P[0][3];
                cly = P[1][0]*cx+P[1][1]*cy+P[1][2]*cz+P[1][3];
                clw = P[3][0]*cx+P[3][1]*cy+P[3][2]*cz+P[3][3];
                if (clw <= 0.0001f) {
                    nBehind++;
                } else {
                    f32 clz = P[2][0]*cx+P[2][1]*cy+P[2][2]*cz+P[2][3];
                    f32 nx = clx/clw, ny = cly/clw, nz = clz/clw;
                    if (nx < ndcMnX) ndcMnX = nx;
                    if (ny < ndcMnY) ndcMnY = ny;
                    if (nx > ndcMxX) ndcMxX = nx;
                    if (ny > ndcMxY) ndcMxY = ny;
                    if (nz < ndcMnZ) ndcMnZ = nz;
                    if (nz > ndcMxZ) ndcMxZ = nz;
                    if (nx >= -1.0f && nx <= 1.0f && ny >= -1.0f && ny <= 1.0f) nOnScreen++;
                    if (nz >= -1.0f && nz <= 1.0f) nZok++;
                }
            }
            g_immVertices[i] = vertex;
        }

        if (dbgBounds && totalSubmitted == 0 && vertexCount >= 4) {
            fprintf(stderr, "[strip] cmd v0=(%.1f,%.1f,%.1f) v1=(%.1f,%.1f,%.1f) v2=(%.1f,%.1f,%.1f) v3=(%.1f,%.1f,%.1f)\n",
                    g_immVertices[0].pos[0], g_immVertices[0].pos[1], g_immVertices[0].pos[2],
                    g_immVertices[1].pos[0], g_immVertices[1].pos[1], g_immVertices[1].pos[2],
                    g_immVertices[2].pos[0], g_immVertices[2].pos[1], g_immVertices[2].pos[2],
                    g_immVertices[3].pos[0], g_immVertices[3].pos[1], g_immVertices[3].pos[2]);
        }

        if (!GXSubmitVertices(primType, g_immVertices, vertexCount,
                              &expandedCount)) {
            return;
        }

        totalSubmitted += vertexCount;
        totalExpanded += expandedCount;
        lastPrimitive = primType;
    }

    if (dbgBounds && totalSubmitted > 0) {
        fprintf(stderr, "[dl] verts=%u onScreen=%d zOK=%d behind=%d ndcZ=[%.5f..%.5f] ndcXY=[%.2f,%.2f..%.2f,%.2f]\n",
                (unsigned)totalSubmitted, nOnScreen, nZok, nBehind,
                ndcMnZ, ndcMxZ, ndcMnX, ndcMnY, ndcMxX, ndcMxY);
    }

    g_lastSubmittedVertexCount = totalSubmitted;
    g_lastExpandedVertexCount = totalExpanded;
    g_lastSubmittedPrimitive = lastPrimitive;
}

/* =========================================================================
 * 8. Framebuffer / Copy
 * ========================================================================= */

void GXCopyDisp(void* dest, GXBool clear) {
    GLFWwindow* window;

    (void)dest;

    window = PCPort_GetHostWindow();
    if (window == NULL || !g_gxInitialized) {
        return;
    }

    GXEnsureCurrentContext();

    if (PCPort_IsVideoBlack()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    if (!g_offscreenEnabled) {
        glfwSwapBuffers(window);
    }

    if (g_drawDoneCallback != (GXDrawDoneCallback)0) {
        g_drawDoneCallback();
    }

    if (clear) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    if (!g_offscreenEnabled) {
        glfwPollEvents();
    }
}

void GXSetCopyFilter(GXBool aa, u8 sample_pattern[12][2],
                     GXBool vf, u8 vfilter[7]) {
    (void)aa; (void)sample_pattern; (void)vf; (void)vfilter;

    /* TODO: Phase 3h -- Multisampling is handled differently on PC.
     *
     * If MSAA is desired, request it in the GLFW window hints:
     *   glfwWindowHint(GLFW_SAMPLES, 4);
     * And enable:
     *   glEnable(GL_MULTISAMPLE);
     *
     * The GCN copy filter parameters are ignored.
     */
}

void GXSetDispCopyDst(u16 wd) {
    (void)wd;
    /* No-op on PC -- display copy stride is handled by the window system. */
}

void GXSetNumChans(u8 nChans) {
    (void)nChans;

    /* TODO: Phase 3e -- Configure number of active color channels
     *
     * Set a uniform or shader variant key to control how many
     * color channels are computed in the vertex shader.
     * Colosseum typically uses 1 channel (COLOR0A0).
     */
}

void GXSetNumTexGens(u8 nTexGens) {
    g_numTexGens = nTexGens;
    g_tevState.numTexGens = nTexGens;
    g_tevState.dirty = 1;
}

u32 GXHostGetLastSubmittedVertexCount(void) {
    return g_lastSubmittedVertexCount;
}

u32 GXHostGetLastExpandedVertexCount(void) {
    return g_lastExpandedVertexCount;
}

u32 GXHostGetLastSubmittedPrimitive(void) {
    return (u32)g_lastSubmittedPrimitive;
}

u32 GXHostGetTevKColorSel(u32 stage) {
    if (stage >= GX_MAX_TEVSTAGE) {
        return 0;
    }
    return g_tevKColorSel[stage];
}

u32 GXHostGetTevKAlphaSel(u32 stage) {
    if (stage >= GX_MAX_TEVSTAGE) {
        return 0;
    }
    return g_tevKAlphaSel[stage];
}


#endif /* __MWERKS__ */
