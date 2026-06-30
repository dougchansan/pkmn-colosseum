/**
 * @file gx_tev.h
 * @brief TEV combiner to GLSL shader translation.
 *
 * The GCN TEV (Texture Environment) is a fixed-function, multi-stage texture
 * combiner with up to 16 stages. Each stage has separate color and alpha
 * equations of the form:
 *
 *   output = (d + lerp(a, b, c)) * scale + bias
 *
 * This module takes a snapshot of the current TEV state and compiles it into
 * a GLSL 330 fragment shader string. Compiled shaders are cached by a hash
 * of the TEV state so that identical configurations reuse the same program.
 *
 * References:
 *   - docs/pc_port_design.md Section 2 (TEV Combiner Translation)
 *   - docs/pc_port_design.md Section 2.3 (GLSL Translation Strategy)
 *   - include/hsd/hsd_tobj.h (TEX_COLORMAP_*, TEX_ALPHAMAP_* modes)
 *   - include/hsd/hsd_mobj.h (HSD_MObj TEV setup)
 *
 * Phase 3 PC port scaffolding -- skeleton only.
 */
#ifndef PCPORT_GX_TEV_H
#define PCPORT_GX_TEV_H

#include "gx_shim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Constants
 * ========================================================================= */

/** Maximum number of TEV stages (GCN hardware limit) */
#define GX_TEV_MAX_STAGES   16

/** Maximum number of cached shader programs */
#define TEV_SHADER_CACHE_MAX 256

/** Maximum length of a generated GLSL fragment shader source string */
#define TEV_SHADER_SRC_MAX  8192

/* =========================================================================
 * TEV stage state -- per-stage configuration
 * ========================================================================= */

typedef struct GXTevStageState {
    /* Color combiner inputs (a, b, c, d) */
    u8 colorIn[4];      /* GXTevColorArg values */
    /* Alpha combiner inputs (a, b, c, d) */
    u8 alphaIn[4];      /* GXTevAlphaArg values */

    /* Color combiner operation */
    u8 colorOp;          /* GXTevOp */
    u8 colorBias;        /* GXTevBias */
    u8 colorScale;       /* GXTevScale */
    u8 colorClamp;       /* GXBool */
    u8 colorOutReg;      /* GXTevRegID */

    /* Alpha combiner operation */
    u8 alphaOp;          /* GXTevOp */
    u8 alphaBias;        /* GXTevBias */
    u8 alphaScale;       /* GXTevScale */
    u8 alphaClamp;       /* GXBool */
    u8 alphaOutReg;      /* GXTevRegID */

    /* TEV stage binding */
    u8 texCoordId;       /* GXTexCoordID */
    u8 texMapId;         /* GXTexMapID */
    u8 channelId;        /* GXChannelID */

    /* Convenience: GXSetTevOp preset mode (if set) */
    u8 tevMode;          /* GXTevMode (0xFF = custom, not preset) */
} GXTevStageState;

/* =========================================================================
 * Full TEV state snapshot -- used as shader cache key
 * ========================================================================= */

typedef struct GXTevState {
    /** Per-stage configuration */
    GXTevStageState stages[GX_TEV_MAX_STAGES];

    /** Number of active TEV stages */
    u8 numStages;

    /** Dirty flag: set when any TEV state changes, cleared after compile */
    u8 dirty;

    /** Number of active texture generators (for texcoord varying count) */
    u8 numTexGens;

    /** Number of active color channels */
    u8 numChans;

    /** Alpha compare state (for discard in fragment shader) */
    u8 alphaComp0;       /* GXCompare */
    u8 alphaComp1;       /* GXCompare */
    u8 alphaRef0;
    u8 alphaRef1;
    u8 alphaOp;          /* GXAlphaOp */

    /** Fog enabled flag */
    u8 fogEnable;

    /** Z compare location (before/after texture) */
    u8 zCompLocBeforeTex;
} GXTevState;

/* =========================================================================
 * Shader cache entry
 * ========================================================================= */

typedef struct GXTevShaderEntry {
    /** Hash of the GXTevState that produced this shader */
    u32 stateHash;

    /** OpenGL shader program ID */
    u32 glProgram;

    /** Uniform locations (cached after linking) */
    s32 loc_projMatrix;
    s32 loc_modelViewMatrix;
    s32 loc_normalMatrix;
    s32 loc_lightDir;       /* u_lightDir: view-space directional sun */
    s32 loc_lightAmbient;   /* u_lightAmbient: floor brightness for unlit faces */
    s32 loc_exposure;       /* u_exposure: final RGB gain (1.0 = neutral) */
    s32 loc_tex[8];
    s32 loc_tevColor[4];
    s32 loc_tevKonst[4];
    s32 loc_matAmbient;
    s32 loc_matDiffuse;
    s32 loc_alphaComp0;
    s32 loc_alphaRef0;
    s32 loc_alphaComp1;
    s32 loc_alphaRef1;
    s32 loc_alphaOp;

    /** Real fog uniforms (no longer repurposed for host-only state). */
    s32 loc_fogEnable;
    s32 loc_fogType;
    s32 loc_fogStart;
    s32 loc_fogEnd;
    s32 loc_fogColor;

    /** Host-only uniforms (dedicated names, no longer piggybacking on fog). */
    s32 loc_hasTexture;
    s32 loc_vertexAlphaScale;
    s32 loc_lightingEnabled;

    /** Per-stage konst color/alpha selectors (GXTevKColorSel/KAlphaSel). */
    s32 loc_tevKonstColorSel;
    s32 loc_tevKonstAlphaSel;

    /** GX lighting channel (ambient + material color) uniforms. */
    s32 loc_chanLightEnabled;
    s32 loc_chanMatColor;
    s32 loc_chanAmbColor;

    s32 loc_texSwizzle[8];

    /** Texture SRT matrix array (one mat3 per texcoord gen, 8 max). */
    s32 loc_texMatrix;

    /** Validity flag */
    u8 valid;
} GXTevShaderEntry;

/* =========================================================================
 * Modern (GL 3.3 core) render path
 *
 * The functions below replace the legacy fixed-function GL_MODULATE draw
 * path with a real TEV->GLSL pipeline: a hash-cached generated program, a
 * shared VAO/VBO, and uniform uploads driven from the current GX state.
 *
 * gx_shim.c drives this by:
 *   1. Pushing the current GX state into a GXTevRenderState (the setters
 *      below), and
 *   2. Calling gx_tev_submit() with the accumulated immediate-mode vertices.
 *
 * The vertex layout (GXTevVertex) is binary-compatible with the shim's
 * immediate-mode GXImmVertex (pos[3] f32, color[4] u8, texcoord[2] f32) so
 * the existing vertex buffer can be passed straight through.
 * ========================================================================= */

/** Vertex layout for the modern submit path. Matches gx_shim.c GXImmVertex. */
typedef struct GXTevVertex {
    f32 pos[3];
    u8  color[4];
    f32 texcoord[2];
    f32 normal[3];
} GXTevVertex;

/**
 * gx_tev_set_proj_matrix -- Record the current 4x4 projection matrix.
 * @param m  Row-major 4x4 matrix (GCN Mtx44 convention).
 *
 * The matrix is stored row-major; the modern path transposes it for GL and
 * applies the GCN->GL clip-space Z remap when uploading.
 */
void gx_tev_set_proj_matrix(const f32 m[4][4]);

/**
 * gx_tev_set_modelview_matrix -- Record the current 3x4 modelview matrix.
 * @param m  Row-major 3x4 matrix (GCN Mtx convention).
 */
void gx_tev_set_modelview_matrix(const f32 m[3][4]);

/**
 * gx_tev_set_normal_matrix -- Record the current 3x4 normal matrix.
 * @param m  Row-major 3x4 matrix; the upper-left 3x3 is uploaded as the
 *           u_normalMatrix mat3 used to transform vertex normals.
 */
void gx_tev_set_normal_matrix(const f32 m[3][4]);

/**
 * gx_tev_set_lighting_enabled -- Toggle directional vertex lighting.
 * @param enabled  Non-zero applies the hardcoded directional lambert to the
 *                 final fragment RGB (alpha unchanged); zero leaves fragments
 *                 full-bright (used for 2D overlays).
 */
void gx_tev_set_lighting_enabled(int enabled);

/**
 * gx_tev_set_light_params -- Set the directional sun used by the lambert pass.
 * @param dx,dy,dz  View-space light direction (need not be normalized; the
 *                  shader normalizes). Larger horizontal (x/z) components carve
 *                  more side-shadow into vertical faces (e.g. ruin columns).
 * @param ambient   Floor brightness [0..1] applied to fully-unlit faces; lower
 *                  = more contrast. Tunable live via PCPORT_LIGHT_DIR/_AMB.
 */
void gx_tev_set_light_params(f32 dx, f32 dy, f32 dz, f32 ambient);

/**
 * gx_tev_set_exposure -- Final RGB gain applied to the fragment colour.
 * @param gain  1.0 = neutral (no change). >1 brightens; used to lift unlit
 *              dark-albedo character meshes toward the lit reference. Values
 *              <=0 are treated as 1.0. Output is clamped to [0,1].
 */
void gx_tev_set_exposure(f32 gain);

/**
 * gx_tev_set_tev_color -- Record a TEV color register (GX_TEVREG0..2 / PREV).
 * @param id     0..3 (GXTevRegID).
 * @param r,g,b,a  8-bit channel values.
 */
void gx_tev_set_tev_color(u32 id, u8 r, u8 g, u8 b, u8 a);

/**
 * gx_tev_set_konst_color -- Record a TEV konstant color register.
 * @param id     0..3 (GXTevRegID).
 * @param r,g,b,a  8-bit channel values.
 */
void gx_tev_set_konst_color(u32 id, u8 r, u8 g, u8 b, u8 a);

/**
 * gx_tev_set_alpha_compare -- Record the alpha-test state for shader discard.
 */
void gx_tev_set_alpha_compare(u8 comp0, u8 ref0, u8 op, u8 comp1, u8 ref1);

/**
 * gx_tev_set_vertex_alpha_scale -- Host-only per-draw vertex alpha modulation.
 * @param scale  [0,1] multiplier applied to vertex color alpha in the shader.
 */
void gx_tev_set_vertex_alpha_scale(f32 scale);

/**
 * gx_tev_set_konst_sel -- Record a TEV stage's konst color/alpha selector.
 * @param stage      TEV stage index (0..GX_TEV_MAX_STAGES-1).
 * @param colorSel   GXTevKColorSel value (selects K0..K3 whole color,
 *                   an R/G/B/A channel, or a fixed constant 1/8..8/8).
 * @param alphaSel   GXTevKAlphaSel value (selects a K register R/G/B/A
 *                   channel or a fixed constant).
 *
 * Selection is applied in the fragment shader via a per-draw uniform array
 * (no shader recompile / cache-key change), so the shader cache stays keyed
 * on the combiner structure only.
 */
void gx_tev_set_konst_sel(u32 stage, u8 colorSel, u8 alphaSel);

/**
 * gx_tev_set_fog -- Record the GX fog state (from GXSetFog).
 * @param type     GXFogType (only GX_FOG_PERSP_LIN is computed; others fall
 *                 back to no fog).
 * @param startz   Fog start Z (eye-space distance where fog begins).
 * @param endz     Fog end Z (eye-space distance where fog is fully applied).
 * @param r,g,b,a  Fog color, 8-bit channels.
 */
void gx_tev_set_fog(u8 type, f32 startz, f32 endz, u8 r, u8 g, u8 b, u8 a);

/**
 * gx_tev_set_chan_lighting -- Toggle the GX color-channel lighting path.
 * @param enabled  Non-zero replaces the rasterized vertex color with the
 *                 GX material color modulated by the ambient color
 *                 (GXSetChanCtrl with mat_src == GX_SRC_REG). Zero leaves
 *                 the rasterized vertex color untouched.
 */
void gx_tev_set_chan_lighting(int enabled);

/**
 * gx_tev_set_chan_mat_color -- Set the GX material color (GXSetChanMatColor).
 * @param r,g,b,a  8-bit channels.
 */
void gx_tev_set_chan_mat_color(u8 r, u8 g, u8 b, u8 a);

/**
 * gx_tev_set_chan_amb_color -- Set the GX ambient color (GXSetChanAmbColor).
 * @param r,g,b,a  8-bit channels.
 */
void gx_tev_set_chan_amb_color(u8 r, u8 g, u8 b, u8 a);

/**
 * gx_tev_set_tex_matrix -- Set a texture SRT matrix for a texcoord slot.
 * @param slot   Texcoord slot index (0..7).
 * @param m      Row-major 3x4 GCN TObj SRT matrix (only the upper-left 2x3
 *               ST-scale+rotate portion is used; the shader applies it as a
 *               mat3 to (s, t, 1) so scroll and rotation work correctly).
 *               Pass NULL to restore the slot to the identity matrix.
 *
 * Defaults to identity for every slot, so existing scenes are unaffected.
 * Call after GXSetTexCoordGen2 (or directly via GXHostSetTexMatrix) to
 * animate UVs (scrolling clouds, water ripple, projected decals).
 */
void gx_tev_set_tex_matrix(u32 slot, const f32 m[3][4]);

/**
 * gx_tev_submit -- Compile/look up the program for the given TEV state, bind
 * it, upload vertices to the shared VBO, and draw.
 *
 * @param state     Current TEV state snapshot (cache key + shader source).
 * @param glPrim    GL primitive enum (GL_TRIANGLES, GL_TRIANGLE_STRIP, ...).
 * @param verts     Pointer to vertex array (GXTevVertex layout).
 * @param count     Number of vertices.
 * @param glTexId   Bound GL texture name (0 = none).
 * @param hasTexture Non-zero if the active stage samples a texture.
 * @return 1 on success, 0 on failure (cache full, no program, bad args).
 */
int gx_tev_submit(const GXTevState* state, u32 glPrim,
                  const GXTevVertex* verts, u32 count,
                  u32 glTexId, int hasTexture);

/* =========================================================================
 * Public API
 * ========================================================================= */

/**
 * gx_tev_ensure_loaded -- Load GLAD (once) and initialize the TEV shader path.
 *
 * gx_shim.c uses the legacy system GL pulled in by GLFW and must NOT include
 * <glad/glad.h> (it conflicts there). This entry point keeps all GLAD usage
 * inside gx_tev.c: on first call it runs gladLoadGL() to resolve the modern
 * (core 3.3) entry points, then calls gx_tev_init(). Subsequent calls are
 * cheap no-ops.
 *
 * Must be called after a GL context is current (e.g. from GXInit).
 *
 * @return 1 if the modern GL path is available, 0 if GLAD failed to load.
 */
int gx_tev_ensure_loaded(void);

/**
 * gx_tev_unbind -- Unbind the shader program / VAO so a subsequent legacy
 * fixed-function draw in gx_shim.c is not affected by leftover modern-GL
 * state. Safe to call even if the modern path was never used.
 */
void gx_tev_unbind(void);

/**
 * gx_tev_init -- Initialize the TEV shader cache.
 *
 * Allocates the cache array and compiles the default "passthrough" shader
 * (single TEV stage, PASSCLR mode).
 *
 * Call once during GXInit, before any rendering.
 */
void gx_tev_init(void);

/**
 * gx_tev_shutdown -- Free all cached shaders and release resources.
 *
 * Call during shutdown to clean up GL shader programs.
 */
void gx_tev_shutdown(void);

/**
 * gx_tev_compile -- Compile or look up a GLSL shader for the current TEV state.
 *
 * @param state  Pointer to the current TEV state snapshot.
 * @return       Pointer to the cached shader entry, or NULL on compile failure.
 *
 * If a shader for this state hash already exists in the cache, return it.
 * Otherwise, generate a new GLSL fragment shader from the TEV stage
 * configuration, compile and link it with the standard vertex shader,
 * and store it in the cache.
 *
 * The generated fragment shader follows the pattern in pc_port_design.md S2.3:
 *
 *   #version 330 core
 *   uniform sampler2D u_tex[8];
 *   uniform vec4 u_tevColor[4];
 *   uniform vec4 u_tevKonst[4];
 *   in vec4 v_color0;
 *   in vec2 v_texcoord[8];
 *   out vec4 fragColor;
 *
 *   void main() {
 *       vec4 prev = v_color0;
 *       // --- TEV Stage 0 ---
 *       // ... generated from stage[0] colorIn/alphaIn/colorOp/alphaOp
 *       // --- TEV Stage 1..N ---
 *       // ... generated similarly
 *       // --- Alpha test ---
 *       // if (!alphaTest(prev.a)) discard;
 *       fragColor = prev;
 *   }
 */
GXTevShaderEntry* gx_tev_compile(const GXTevState* state);

/**
 * gx_tev_bind -- Bind the shader for the current TEV state and set uniforms.
 *
 * @param entry  Shader cache entry from gx_tev_compile.
 *
 * Calls glUseProgram and sets all uniform values from the current GX state.
 */
void gx_tev_bind(const GXTevShaderEntry* entry);

/**
 * gx_tev_hash -- Compute a hash of the TEV state for cache lookup.
 *
 * @param state  Pointer to the TEV state to hash.
 * @return       32-bit hash value.
 *
 * Uses FNV-1a or similar fast hash over the state bytes.
 */
u32 gx_tev_hash(const GXTevState* state);

/**
 * gx_tev_generate_fragment_shader -- Generate GLSL source from TEV state.
 *
 * @param state    Pointer to the TEV state.
 * @param outBuf   Output buffer for the GLSL source string.
 * @param bufSize  Size of the output buffer.
 * @return         Length of the generated source, or -1 on error.
 *
 * Emits a complete GLSL 330 fragment shader that implements the TEV
 * combiner logic described by the state.
 */
s32 gx_tev_generate_fragment_shader(const GXTevState* state,
                                    char* outBuf, u32 bufSize);

/**
 * gx_tev_generate_vertex_shader -- Generate the standard vertex shader.
 *
 * @param numTexGens  Number of active texture coordinate generators.
 * @param numChans    Number of active color channels.
 * @param outBuf      Output buffer for the GLSL source string.
 * @param bufSize     Size of the output buffer.
 * @return            Length of the generated source, or -1 on error.
 *
 * The vertex shader is mostly static (same for all TEV configurations),
 * but the number of texcoord varyings and color channel computations
 * varies.
 */
s32 gx_tev_generate_vertex_shader(u8 numTexGens, u8 numChans,
                                  char* outBuf, u32 bufSize);

/**
 * gx_tev_get_cache_stats -- Get shader cache hit/miss statistics.
 *
 * @param outHits    Pointer to receive hit count.
 * @param outMisses  Pointer to receive miss count.
 * @param outTotal   Pointer to receive total cached shaders.
 */
void gx_tev_get_cache_stats(u32* outHits, u32* outMisses, u32* outTotal);

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_GX_TEV_H */
