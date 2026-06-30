/**
 * @file gx_tev.c
 * @brief TEV combiner to GLSL shader translation -- functional first pass.
 *
 * Translates GCN TEV combiner configurations into GLSL 330 fragment shaders.
 * Shaders are generated at runtime and cached by a hash of the TEV state.
 *
 * The approach mirrors Dolphin Emulator's shader generation and the
 * encounter/aurora library from the Metroid Prime decomp. Since Colosseum
 * uses a limited set of TEV configurations (estimated 20-40 unique
 * combinations), most shaders will be compiled on first encounter and
 * then served from cache for the rest of the session.
 *
 * This implementation replaces the legacy fixed-function GL_MODULATE draw
 * path with a real programmable pipeline:
 *   - GLSL 3.30 vertex + fragment shaders generated from GX TEV state.
 *   - A hash-cached set of compiled/linked programs.
 *   - A shared VAO/VBO modern submit path (gx_tev_submit).
 *
 * References:
 *   - docs/pc_port_design.md Section 2 (TEV Combiner Translation)
 *   - docs/pc_port_design.md Section 2.4 (Preset Optimization)
 *   - gx_tev.h for data structures and API
 *
 * Phase 3 PC port -- functional first pass.
 */

#ifdef __MWERKS__
/* GCN build: pcport shim not applicable */
#else

#include "gx_tev.h"

#include <glad/glad.h>

#include <stdio.h>
#include <string.h>

#ifndef GL_TRUE
#define GL_TRUE 1
#endif
#ifndef GL_FALSE
#define GL_FALSE 0
#endif

/* =========================================================================
 * Shader cache
 * ========================================================================= */

static GXTevShaderEntry g_shaderCache[TEV_SHADER_CACHE_MAX];
static u32 g_shaderCacheCount = 0;
static u32 g_cacheHits = 0;
static u32 g_cacheMisses = 0;

/* =========================================================================
 * Modern render state (mirror of the relevant GX state for uniform upload)
 * ========================================================================= */

typedef struct {
    f32 proj[4][4];        /* row-major 4x4 projection */
    f32 modelView[3][4];   /* row-major 3x4 modelview */
    f32 normalMatrix[3][4];/* row-major 3x4 normal matrix (upper 3x3 used) */

    f32 tevColor[4][4];    /* PREV/REG0..2, normalized RGBA */
    f32 konstColor[4][4];  /* K0..K3, normalized RGBA */

    s32 alphaComp0;
    s32 alphaComp1;
    f32 alphaRef0;
    f32 alphaRef1;
    s32 alphaOp;

    f32 vertexAlphaScale;

    s32 lightingEnabled;   /* 1 = apply directional lambert; 0 = full bright */
    f32 lightDir[3];       /* view-space sun direction (normalized in shader) */
    f32 lightAmbient;      /* floor brightness for unlit faces [0..1] */
    f32 exposure;          /* final RGB gain (1.0 = neutral); lifts unlit dark
                            * albedo to read like the lit reference */

    /* Per-stage konst color/alpha selectors (GXTevKColorSel/KAlphaSel).
     * Uploaded as int-array uniforms so the shader does selection at draw
     * time without a recompile. */
    s32 konstColorSel[GX_TEV_MAX_STAGES];
    s32 konstAlphaSel[GX_TEV_MAX_STAGES];

    /* Fog state (GXSetFog). */
    s32 fogEnable;         /* 1 = fog active (GX_FOG_PERSP_LIN) */
    s32 fogType;           /* GXFogType */
    f32 fogStart;          /* eye-space near distance */
    f32 fogEnd;            /* eye-space far distance */
    f32 fogColor[4];       /* normalized RGBA */

    /* GX lighting channel (GXSetChanCtrl + ambient/material color). */
    s32 chanLightEnabled;  /* 1 = use mat color * ambient instead of v_color0 */
    f32 chanMatColor[4];   /* normalized RGBA material color */
    f32 chanAmbColor[4];   /* normalized RGBA ambient color */

    /* Texture SRT matrices (one mat3 per texcoord slot, 8 slots).
     * Each matrix is stored as a row-major 3x3 (9 floats) and applied to
     * (s, t, 1) in the vertex shader to implement scroll/rotate/scale.
     * Defaults to identity so untouched slots pass UVs unchanged. */
    f32 texMatrix[8][3][3];
} GXTevRenderState;

static GXTevRenderState g_rs;

/* =========================================================================
 * GL resources for the modern submit path
 * ========================================================================= */

static GLuint g_vao = 0;
static GLuint g_vbo = 0;
static GLuint g_boundProgram = 0;
static int    g_glReady = 0;

/* =========================================================================
 * Standard vertex shader source (shared across all TEV configurations)
 * ========================================================================= */

static const char* VERTEX_SHADER_SOURCE =
    "#version 330 core\n"
    "\n"
    "layout(location = 0) in vec3 a_position;\n"
    "layout(location = 1) in vec4 a_color0;\n"
    "layout(location = 2) in vec2 a_texcoord0;\n"
    /* a_normal (location 3) is optional per draw. Vertices without normals leave
     * it zero, and the fragment shader falls back to derivative face normals. */
    "layout(location = 3) in vec3 a_normal;\n"
    "\n"
    "uniform mat4 u_projMatrix;\n"
    "uniform mat4 u_modelViewMatrix;\n"
    "uniform mat3 u_normalMatrix;\n"
    /* Texture SRT matrices: one mat3 per texcoord slot (8 slots).
     * Applied to (s, t, 1) to implement scroll, rotate, scale.
     * Defaults to identity so untouched slots pass UVs unchanged. */
    "uniform mat3 u_texMatrix[8];\n"
    "\n"
    "out vec4 v_color0;\n"
    "out vec2 v_texcoord0;\n"
    "out vec2 v_texcoord1;\n"
    "out vec3 v_viewPos;\n"
    "out vec3 v_normal;\n"
    "\n"
    "void main() {\n"
    "    vec4 viewPos = u_modelViewMatrix * vec4(a_position, 1.0);\n"
    "    gl_Position = u_projMatrix * viewPos;\n"
    "    v_viewPos = viewPos.xyz;\n"
    "    v_normal = u_normalMatrix * a_normal;\n"
    "    v_color0 = a_color0;\n"
    /* Apply the per-slot SRT matrix: multiply (s,t,1) by the mat3, then
     * take the first two components as the transformed UV.  When u_texMatrix[n]
     * is identity (the default) this is a no-op: (s,t,1)*I = (s,t,1). */
    "    v_texcoord0 = (u_texMatrix[0] * vec3(a_texcoord0, 1.0)).xy;\n"
    "    v_texcoord1 = (u_texMatrix[1] * vec3(a_texcoord0, 1.0)).xy;\n"
    "}\n";

/* =========================================================================
 * Internal helpers: GLSL expression generation
 * ========================================================================= */

/**
 * Map a GXTevColorArg to a GLSL vec3 expression for the active stage.
 * 'texName' is the GLSL variable holding the sampled texel (vec4).
 */
static const char* tev_color_input_expr(u8 arg) {
    switch (arg) {
        case GX_CC_CPREV: return "prev.rgb";
        case GX_CC_APREV: return "vec3(prev.a)";
        case GX_CC_C0:    return "creg0.rgb";
        case GX_CC_A0:    return "vec3(creg0.a)";
        case GX_CC_C1:    return "creg1.rgb";
        case GX_CC_A1:    return "vec3(creg1.a)";
        case GX_CC_C2:    return "creg2.rgb";
        case GX_CC_A2:    return "vec3(creg2.a)";
        case GX_CC_TEXC:  return "texc.rgb";
        case GX_CC_TEXA:  return "vec3(texc.a)";
        case GX_CC_RASC:  return "rasc.rgb";
        case GX_CC_RASA:  return "vec3(rasc.a)";
        case GX_CC_ONE:   return "vec3(1.0)";
        case GX_CC_HALF:  return "vec3(0.5)";
        case GX_CC_KONST: return "konst.rgb";
        case GX_CC_ZERO:
        default:          return "vec3(0.0)";
    }
}

/**
 * Map a GXTevAlphaArg to a GLSL float expression for the active stage.
 */
static const char* tev_alpha_input_expr(u8 arg) {
    switch (arg) {
        case GX_CA_APREV: return "prev.a";
        case GX_CA_A0:    return "creg0.a";
        case GX_CA_A1:    return "creg1.a";
        case GX_CA_A2:    return "creg2.a";
        case GX_CA_TEXA:  return "texc.a";
        case GX_CA_RASA:  return "rasc.a";
        case GX_CA_KONST: return "konst.a";
        case GX_CA_ZERO:
        default:          return "0.0";
    }
}

static const char* tev_scale_factor(u8 scale) {
    switch (scale) {
        case GX_CS_SCALE_2:  return "2.0";
        case GX_CS_SCALE_4:  return "4.0";
        case GX_CS_DIVIDE_2: return "0.5";
        case GX_CS_SCALE_1:
        default:             return "1.0";
    }
}

static const char* tev_bias_value(u8 bias) {
    switch (bias) {
        case GX_TB_ADDHALF: return "0.5";
        case GX_TB_SUBHALF: return "-0.5";
        case GX_TB_ZERO:
        default:            return "0.0";
    }
}

/* Which GXTevReg this stage's output register name maps to. */
static const char* tev_out_reg_name(u8 outReg) {
    switch (outReg) {
        case GX_TEVREG0: return "creg0";
        case GX_TEVREG1: return "creg1";
        case GX_TEVREG2: return "creg2";
        case GX_TEVPREV:
        default:         return "prev";
    }
}

/**
 * Resolve a stage to effective color/alpha inputs. For preset modes
 * (GXSetTevOp) the inputs are synthesized; otherwise the explicit
 * GXSetTevColorIn/AlphaIn arrays are used.
 *
 * On return the cIn[4]/aIn[4]/ops describe a normal (d + lerp(a,b,c)) stage.
 */
typedef struct {
    u8 cIn[4];      /* a,b,c,d color args */
    u8 aIn[4];      /* a,b,c,d alpha args */
    u8 colorOp, colorBias, colorScale, colorClamp, colorOutReg;
    u8 alphaOp, alphaBias, alphaScale, alphaClamp, alphaOutReg;
} GXTevResolvedStage;

static int tev_mode_is_preset(u8 mode) {
    return mode == GX_MODULATE || mode == GX_DECAL || mode == GX_BLEND ||
           mode == GX_REPLACE  || mode == GX_PASSCLR;
}

static void tev_resolve_stage(const GXTevStageState* s, GXTevResolvedStage* out) {
    memset(out, 0, sizeof(*out));

    /* Defaults for a passthrough ADD stage with no scale/bias and clamp on. */
    out->colorOp = GX_TEV_ADD;
    out->alphaOp = GX_TEV_ADD;
    out->colorScale = GX_CS_SCALE_1;
    out->alphaScale = GX_CS_SCALE_1;
    out->colorBias = GX_TB_ZERO;
    out->alphaBias = GX_TB_ZERO;
    out->colorClamp = GX_TRUE;
    out->alphaClamp = GX_TRUE;
    out->colorOutReg = GX_TEVPREV;
    out->alphaOutReg = GX_TEVPREV;

    if (tev_mode_is_preset(s->tevMode)) {
        /* Synthesize inputs from the GXSetTevOp preset. The TEV equation is
         * out = d + lerp(a,b,c); presets pick a/b/c/d so that: */
        switch (s->tevMode) {
            case GX_MODULATE: /* color = ras*tex ; alpha = ras_a*tex_a */
                out->cIn[0] = GX_CC_ZERO; out->cIn[1] = GX_CC_TEXC;
                out->cIn[2] = GX_CC_RASC; out->cIn[3] = GX_CC_ZERO;
                out->aIn[0] = GX_CA_ZERO; out->aIn[1] = GX_CA_TEXA;
                out->aIn[2] = GX_CA_RASA; out->aIn[3] = GX_CA_ZERO;
                break;
            case GX_DECAL:    /* color = lerp(ras, tex, tex_a) ; alpha = ras_a */
                out->cIn[0] = GX_CC_RASC; out->cIn[1] = GX_CC_TEXC;
                out->cIn[2] = GX_CC_TEXA; out->cIn[3] = GX_CC_ZERO;
                out->aIn[0] = GX_CA_ZERO; out->aIn[1] = GX_CA_ZERO;
                out->aIn[2] = GX_CA_ZERO; out->aIn[3] = GX_CA_RASA;
                break;
            case GX_BLEND:    /* color = lerp(ras, tex, ras_a) ; alpha = ras_a*tex_a */
                out->cIn[0] = GX_CC_RASC; out->cIn[1] = GX_CC_TEXC;
                out->cIn[2] = GX_CC_RASA; out->cIn[3] = GX_CC_ZERO;
                out->aIn[0] = GX_CA_ZERO; out->aIn[1] = GX_CA_TEXA;
                out->aIn[2] = GX_CA_RASA; out->aIn[3] = GX_CA_ZERO;
                break;
            case GX_REPLACE:  /* color = tex ; alpha = tex_a */
                out->cIn[0] = GX_CC_ZERO; out->cIn[1] = GX_CC_ZERO;
                out->cIn[2] = GX_CC_ZERO; out->cIn[3] = GX_CC_TEXC;
                out->aIn[0] = GX_CA_ZERO; out->aIn[1] = GX_CA_ZERO;
                out->aIn[2] = GX_CA_ZERO; out->aIn[3] = GX_CA_TEXA;
                break;
            case GX_PASSCLR:  /* color = ras ; alpha = ras_a */
            default:
                out->cIn[0] = GX_CC_ZERO; out->cIn[1] = GX_CC_ZERO;
                out->cIn[2] = GX_CC_ZERO; out->cIn[3] = GX_CC_RASC;
                out->aIn[0] = GX_CA_ZERO; out->aIn[1] = GX_CA_ZERO;
                out->aIn[2] = GX_CA_ZERO; out->aIn[3] = GX_CA_RASA;
                break;
        }
        return;
    }

    /* Custom stage: use the explicit inputs and ops recorded from
     * GXSetTevColorIn / GXSetTevAlphaIn / GXSetTevColorOp / GXSetTevAlphaOp. */
    out->cIn[0] = s->colorIn[0]; out->cIn[1] = s->colorIn[1];
    out->cIn[2] = s->colorIn[2]; out->cIn[3] = s->colorIn[3];
    out->aIn[0] = s->alphaIn[0]; out->aIn[1] = s->alphaIn[1];
    out->aIn[2] = s->alphaIn[2]; out->aIn[3] = s->alphaIn[3];

    out->colorOp = s->colorOp;
    out->colorBias = s->colorBias;
    out->colorScale = s->colorScale;
    out->colorClamp = s->colorClamp;
    out->colorOutReg = s->colorOutReg;

    out->alphaOp = s->alphaOp;
    out->alphaBias = s->alphaBias;
    out->alphaScale = s->alphaScale;
    out->alphaClamp = s->alphaClamp;
    out->alphaOutReg = s->alphaOutReg;
}

/* Bounded append helper: append 'src' to 'buf' (size 'cap'), tracking '*pos'. */
static void tev_append(char* buf, u32 cap, u32* pos, const char* src) {
    u32 i = 0;
    while (src[i] != '\0' && (*pos + 1) < cap) {
        buf[*pos] = src[i];
        (*pos)++;
        i++;
    }
    buf[*pos] = '\0';
}

/* =========================================================================
 * Fragment shader generation
 * ========================================================================= */

s32 gx_tev_generate_fragment_shader(const GXTevState* state,
                                    char* outBuf, u32 bufSize) {
    u32 pos = 0;
    u32 stageCount;
    u32 i;
    char line[512];

    if (!state || !outBuf || bufSize == 0) return -1;

    stageCount = state->numStages;
    if (stageCount == 0) stageCount = 1;
    if (stageCount > GX_TEV_MAX_STAGES) stageCount = GX_TEV_MAX_STAGES;

    tev_append(outBuf, bufSize, &pos,
        "#version 330 core\n"
        "\n"
        "uniform sampler2D u_tex0;\n"
        "uniform sampler2D u_tex1;\n"
        "uniform vec4 u_tevColor[4];\n"
        "uniform vec4 u_tevKonst[4];\n"
        "uniform int  u_hasTexture;\n"
        "uniform float u_vertexAlphaScale;\n"
        "uniform int  u_alphaComp0;\n"
        "uniform int  u_alphaComp1;\n"
        "uniform float u_alphaRef0;\n"
        "uniform float u_alphaRef1;\n"
        "uniform int  u_alphaOp;\n"
        "uniform int  u_lightingEnabled;\n"
        "uniform vec3 u_lightDir;\n"
        "uniform float u_lightAmbient;\n"
        "uniform float u_exposure;\n"
        /* Per-stage konst selectors (GXTevKColorSel/KAlphaSel raw values). */
        "uniform int  u_tevKonstColorSel[16];\n"
        "uniform int  u_tevKonstAlphaSel[16];\n"
        /* Fog (GXSetFog). */
        "uniform int  u_fogEnable;\n"
        "uniform int  u_fogType;\n"
        "uniform float u_fogStart;\n"
        "uniform float u_fogEnd;\n"
        "uniform vec4 u_fogColor;\n"
        /* GX lighting channel (ambient + material color). */
        "uniform int  u_chanLightEnabled;\n"
        "uniform vec4 u_chanMatColor;\n"
        "uniform vec4 u_chanAmbColor;\n"
        "\n"
        "in vec4 v_color0;\n"
        "in vec2 v_texcoord0;\n"
        "in vec2 v_texcoord1;\n"
        "in vec3 v_viewPos;\n"
        "in vec3 v_normal;\n"
        "\n"
        "out vec4 fragColor;\n"
        "\n"
        "bool tevCompare(int comp, float val, float ref) {\n"
        "    if (comp == 0) return false;\n"        /* NEVER */
        "    if (comp == 1) return val <  ref;\n"   /* LESS */
        "    if (comp == 2) return val == ref;\n"   /* EQUAL */
        "    if (comp == 3) return val <= ref;\n"   /* LEQUAL */
        "    if (comp == 4) return val >  ref;\n"   /* GREATER */
        "    if (comp == 5) return val != ref;\n"   /* NEQUAL */
        "    if (comp == 6) return val >= ref;\n"   /* GEQUAL */
        "    return true;\n"                        /* ALWAYS */
        "}\n"
        "\n"
        /* GXTevKColorSel -> vec4 konst. Whole-color sels 0x0C..0x0F pick
         * K0..K3.rgb; channel sels 0x10..0x1F pick a single K register
         * channel broadcast to rgb; constant sels 0x00..0x07 are 8/8..1/8. */
        "vec3 tevKonstColor(int sel) {\n"
        "    if (sel >= 16) {\n"
        "        int k = (sel - 16) / 4;\n"
        "        int c = (sel - 16) - k * 4;\n"
        "        return vec3(u_tevKonst[k][c]);\n"
        "    }\n"
        "    if (sel >= 12) return u_tevKonst[sel - 12].rgb;\n"
        "    if (sel <= 7) return vec3(float(8 - sel) / 8.0);\n"
        "    return vec3(1.0);\n"
        "}\n"
        /* GXTevKAlphaSel -> float konst alpha. Channel sels 0x10..0x1F pick a
         * single K register channel; constant sels 0x00..0x07 are 8/8..1/8. */
        "float tevKonstAlpha(int sel) {\n"
        "    if (sel >= 16) {\n"
        "        int k = (sel - 16) / 4;\n"
        "        int c = (sel - 16) - k * 4;\n"
        "        return u_tevKonst[k][c];\n"
        "    }\n"
        "    if (sel <= 7) return float(8 - sel) / 8.0;\n"
        "    return 1.0;\n"
        "}\n"
        "\n"
        "void main() {\n"
        /* Rasterized color: when GX channel lighting is enabled with a register
         * material source, the channel color is matColor * ambColor (the common
         * single-channel ambient/material case). Otherwise use vertex color. */
        "    vec3 rasrgb = (u_chanLightEnabled != 0)\n"
        "        ? (u_chanMatColor.rgb * u_chanAmbColor.rgb)\n"
        "        : v_color0.rgb;\n"
        "    float rasa = (u_chanLightEnabled != 0)\n"
        "        ? (u_chanMatColor.a * u_chanAmbColor.a)\n"
        "        : v_color0.a;\n"
        "    vec4 rasc  = vec4(rasrgb, rasa * u_vertexAlphaScale);\n"
        "    vec4 prev  = rasc;\n"
        "    vec4 creg0 = u_tevColor[1];\n"
        "    vec4 creg1 = u_tevColor[2];\n"
        "    vec4 creg2 = u_tevColor[3];\n"
        "    vec4 texc  = vec4(1.0);\n"
        "    vec4 konst = vec4(1.0);\n");

    /* Emit each TEV stage. */
    for (i = 0; i < stageCount; ++i) {
        const GXTevStageState* s = &state->stages[i];
        GXTevResolvedStage rs;
        const char* outName;
        const char* texCoordVar;
        const char* texSampler;
        int usesTexture;

        tev_resolve_stage(s, &rs);

        /* Determine whether this stage samples a texture. Presets DECAL/BLEND/
         * REPLACE/MODULATE all sample; PASSCLR does not. Custom stages sample
         * if any input references TEXC/TEXA. */
        usesTexture = 0;
        if (tev_mode_is_preset(s->tevMode)) {
            usesTexture = (s->tevMode != GX_PASSCLR);
        } else {
            int k;
            for (k = 0; k < 4; ++k) {
                if (rs.cIn[k] == GX_CC_TEXC || rs.cIn[k] == GX_CC_TEXA) usesTexture = 1;
                if (rs.aIn[k] == GX_CA_TEXA) usesTexture = 1;
            }
        }

        /* Texcoord/sampler selection (texcoord1 maps to GX_TEXCOORD1). */
        texCoordVar = (s->texCoordId == GX_TEXCOORD1) ? "v_texcoord1" : "v_texcoord0";
        texSampler  = (s->texMapId == GX_TEXMAP1) ? "u_tex1" : "u_tex0";

        snprintf(line, sizeof(line), "\n    // --- TEV stage %u ---\n", i);
        tev_append(outBuf, bufSize, &pos, line);

        if (usesTexture) {
            /* When the bound texture is unavailable, fall back to white so the
             * stage degrades to its rasterized contribution instead of black. */
            snprintf(line, sizeof(line),
                "    texc = (u_hasTexture != 0) ? texture(%s, %s) : vec4(1.0);\n",
                texSampler, texCoordVar);
            tev_append(outBuf, bufSize, &pos, line);
        }

        /* konst: select per-stage from the GXTevKColorSel/KAlphaSel selectors
         * (uploaded as the u_tevKonst*Sel uniform arrays). The selection runs
         * at draw time so the shader cache is not keyed on the konst sels. */
        snprintf(line, sizeof(line),
            "    konst.rgb = tevKonstColor(u_tevKonstColorSel[%u]);\n"
            "    konst.a   = tevKonstAlpha(u_tevKonstAlphaSel[%u]);\n",
            i, i);
        tev_append(outBuf, bufSize, &pos, line);

        outName = tev_out_reg_name(rs.colorOutReg);

        /* --- Color combiner: out.rgb = (d + lerp(a,b,c)) * scale + bias --- */
        if (rs.colorOp == GX_TEV_ADD || rs.colorOp == GX_TEV_SUB) {
            const char* sign = (rs.colorOp == GX_TEV_SUB) ? "-" : "+";
            snprintf(line, sizeof(line),
                "    %s.rgb = ((%s) %s mix(%s, %s, %s)) * %s + vec3(%s);\n",
                outName,
                tev_color_input_expr(rs.cIn[3]), sign,
                tev_color_input_expr(rs.cIn[0]),
                tev_color_input_expr(rs.cIn[1]),
                tev_color_input_expr(rs.cIn[2]),
                tev_scale_factor(rs.colorScale),
                tev_bias_value(rs.colorBias));
            tev_append(outBuf, bufSize, &pos, line);
        } else {
            /* Comparison ops: out.rgb = d + ((a OP b) ? c : 0). Use the R
             * channel as a representative comparison for the first pass. */
            const char* cmp = (rs.colorOp & 1) ? "==" : ">";
            snprintf(line, sizeof(line),
                "    %s.rgb = (%s) + ((%s.r %s %s.r) ? (%s) : vec3(0.0));\n",
                outName,
                tev_color_input_expr(rs.cIn[3]),
                tev_color_input_expr(rs.cIn[0]), cmp,
                tev_color_input_expr(rs.cIn[1]),
                tev_color_input_expr(rs.cIn[2]));
            tev_append(outBuf, bufSize, &pos, line);
        }
        if (rs.colorClamp) {
            snprintf(line, sizeof(line),
                "    %s.rgb = clamp(%s.rgb, 0.0, 1.0);\n", outName, outName);
            tev_append(outBuf, bufSize, &pos, line);
        }

        /* --- Alpha combiner --- */
        outName = tev_out_reg_name(rs.alphaOutReg);
        if (rs.alphaOp == GX_TEV_ADD || rs.alphaOp == GX_TEV_SUB) {
            const char* sign = (rs.alphaOp == GX_TEV_SUB) ? "-" : "+";
            snprintf(line, sizeof(line),
                "    %s.a = ((%s) %s mix(%s, %s, %s)) * %s + (%s);\n",
                outName,
                tev_alpha_input_expr(rs.aIn[3]), sign,
                tev_alpha_input_expr(rs.aIn[0]),
                tev_alpha_input_expr(rs.aIn[1]),
                tev_alpha_input_expr(rs.aIn[2]),
                tev_scale_factor(rs.alphaScale),
                tev_bias_value(rs.alphaBias));
            tev_append(outBuf, bufSize, &pos, line);
        } else {
            const char* cmp = (rs.alphaOp & 1) ? "==" : ">";
            snprintf(line, sizeof(line),
                "    %s.a = (%s) + ((%s %s %s) ? (%s) : 0.0);\n",
                outName,
                tev_alpha_input_expr(rs.aIn[3]),
                tev_alpha_input_expr(rs.aIn[0]), cmp,
                tev_alpha_input_expr(rs.aIn[1]),
                tev_alpha_input_expr(rs.aIn[2]));
            tev_append(outBuf, bufSize, &pos, line);
        }
        if (rs.alphaClamp) {
            snprintf(line, sizeof(line),
                "    %s.a = clamp(%s.a, 0.0, 1.0);\n", outName, outName);
            tev_append(outBuf, bufSize, &pos, line);
        }
    }

    /* --- Alpha test (discard) --- */
    tev_append(outBuf, bufSize, &pos,
        "\n"
        "    bool ap0 = tevCompare(u_alphaComp0, prev.a, u_alphaRef0);\n"
        "    bool ap1 = tevCompare(u_alphaComp1, prev.a, u_alphaRef1);\n"
        "    bool alphaPass;\n"
        "    if (u_alphaOp == 0)      alphaPass = ap0 && ap1;\n"  /* AND */
        "    else if (u_alphaOp == 1) alphaPass = ap0 || ap1;\n"  /* OR */
        "    else if (u_alphaOp == 2) alphaPass = ap0 != ap1;\n"  /* XOR */
        "    else                     alphaPass = ap0 == ap1;\n"  /* XNOR */
        "    if (!alphaPass) discard;\n"
        "\n"
        "    fragColor = prev;\n"
        "\n"
        /* Directional lighting. Prefer real per-vertex normals when the draw
         * supplies them; fall back to derivative face normals for older host
         * paths that still submit only POS+CLR+TEX. */
        "    if (u_lightingEnabled != 0) {\n"
        "        vec3 faceN = v_normal;\n"
        "        float fl = length(faceN);\n"
        "        if (fl <= 1e-6) {\n"
        "            vec3 dpdx = dFdx(v_viewPos);\n"
        "            vec3 dpdy = dFdy(v_viewPos);\n"
        "            faceN = cross(dpdx, dpdy);\n"
        "            fl = length(faceN);\n"
        "        }\n"
        "        if (fl > 1e-6) {\n"
        "            faceN /= fl;\n"
        "            if (faceN.z < 0.0) faceN = -faceN;\n"
        "            vec3 L = normalize(u_lightDir);\n"
        "            float ndl = max(0.0, dot(faceN, L));\n"
        "            float lambert = u_lightAmbient + (1.0 - u_lightAmbient) * ndl;\n"
        "            fragColor.rgb *= lambert;\n"
        "        }\n"
        "    }\n"
        /* Linear fog (GX_FOG_PERSP_LIN). The fog factor is 1 at fogStart and 0
         * at fogEnd; fragColor blends toward the fog color as it drops. Eye-space
         * distance uses the view-space position magnitude (perspective). Only the
         * linear type (u_fogType == 2) is computed; other types are treated as no
         * fog until needed. */
        "    if (u_fogEnable != 0 && u_fogType == 2) {\n"
        "        float fogZ = length(v_viewPos);\n"
        "        float denom = u_fogEnd - u_fogStart;\n"
        "        float fogFactor = (abs(denom) > 1e-6)\n"
        "            ? clamp((u_fogEnd - fogZ) / denom, 0.0, 1.0)\n"
        "            : 1.0;\n"
        "        fragColor.rgb = mix(u_fogColor.rgb, fragColor.rgb, fogFactor);\n"
        "    }\n"
        /* Final exposure gain. 1.0 = neutral (no effect on existing scenes). A
         * value >1 lifts an unlit dark-albedo mesh (e.g. the character models,
         * which the game would otherwise light at runtime) toward the brightness
         * of the lit reference art. Clamp so colours saturate to white rather
         * than wrapping. */
        "    fragColor.rgb = clamp(fragColor.rgb * u_exposure, 0.0, 1.0);\n"
        "}\n");

    return (s32)pos;
}

s32 gx_tev_generate_vertex_shader(u8 numTexGens, u8 numChans,
                                  char* outBuf, u32 bufSize) {
    s32 len;
    (void)numTexGens; (void)numChans;

    if (!outBuf || bufSize == 0) return -1;
    len = snprintf(outBuf, bufSize, "%s", VERTEX_SHADER_SOURCE);
    return len;
}

/* =========================================================================
 * GL compile/link helpers
 * ========================================================================= */

static GLuint tev_compile_shader(GLenum type, const char* src) {
    GLuint sh;
    GLint status = GL_FALSE;

    sh = glCreateShader(type);
    if (sh == 0) return 0;

    glShaderSource(sh, 1, &src, NULL);
    glCompileShader(sh);
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        char log[1024];
        GLsizei n = 0;
        glGetShaderInfoLog(sh, (GLsizei)sizeof(log), &n, log);
        log[(n < (GLsizei)sizeof(log)) ? n : (GLsizei)sizeof(log) - 1] = '\0';
        printf("[gx_tev] %s shader compile failed:\n%s\n",
               (type == GL_VERTEX_SHADER) ? "vertex" : "fragment", log);
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

static GLuint tev_link_program(GLuint vs, GLuint fs) {
    GLuint prog;
    GLint status = GL_FALSE;

    prog = glCreateProgram();
    if (prog == 0) return 0;

    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        char log[1024];
        GLsizei n = 0;
        glGetProgramInfoLog(prog, (GLsizei)sizeof(log), &n, log);
        log[(n < (GLsizei)sizeof(log)) ? n : (GLsizei)sizeof(log) - 1] = '\0';
        printf("[gx_tev] program link failed:\n%s\n", log);
        glDeleteProgram(prog);
        return 0;
    }
    return prog;
}

static void tev_cache_uniform_locations(GXTevShaderEntry* entry) {
    GLuint p = entry->glProgram;

    entry->loc_projMatrix      = glGetUniformLocation(p, "u_projMatrix");
    entry->loc_modelViewMatrix = glGetUniformLocation(p, "u_modelViewMatrix");
    entry->loc_normalMatrix    = glGetUniformLocation(p, "u_normalMatrix");
    entry->loc_lightDir        = glGetUniformLocation(p, "u_lightDir");
    entry->loc_lightAmbient    = glGetUniformLocation(p, "u_lightAmbient");
    entry->loc_exposure        = glGetUniformLocation(p, "u_exposure");

    entry->loc_tex[0] = glGetUniformLocation(p, "u_tex0");
    entry->loc_tex[1] = glGetUniformLocation(p, "u_tex1");

    entry->loc_tevColor[0] = glGetUniformLocation(p, "u_tevColor");
    entry->loc_tevKonst[0] = glGetUniformLocation(p, "u_tevKonst");

    entry->loc_alphaComp0 = glGetUniformLocation(p, "u_alphaComp0");
    entry->loc_alphaComp1 = glGetUniformLocation(p, "u_alphaComp1");
    entry->loc_alphaRef0  = glGetUniformLocation(p, "u_alphaRef0");
    entry->loc_alphaRef1  = glGetUniformLocation(p, "u_alphaRef1");
    entry->loc_alphaOp    = glGetUniformLocation(p, "u_alphaOp");

    /* Host-only uniforms now have their own dedicated locations (they no longer
     * piggyback on the fog uniform slots). */
    entry->loc_hasTexture       = glGetUniformLocation(p, "u_hasTexture");
    entry->loc_vertexAlphaScale = glGetUniformLocation(p, "u_vertexAlphaScale");
    entry->loc_lightingEnabled  = glGetUniformLocation(p, "u_lightingEnabled");

    /* Per-stage konst selectors (uniform arrays). */
    entry->loc_tevKonstColorSel = glGetUniformLocation(p, "u_tevKonstColorSel");
    entry->loc_tevKonstAlphaSel = glGetUniformLocation(p, "u_tevKonstAlphaSel");

    /* Real fog uniforms. */
    entry->loc_fogEnable = glGetUniformLocation(p, "u_fogEnable");
    entry->loc_fogType   = glGetUniformLocation(p, "u_fogType");
    entry->loc_fogStart  = glGetUniformLocation(p, "u_fogStart");
    entry->loc_fogEnd    = glGetUniformLocation(p, "u_fogEnd");
    entry->loc_fogColor  = glGetUniformLocation(p, "u_fogColor");

    /* GX lighting channel (ambient + material color). */
    entry->loc_chanLightEnabled = glGetUniformLocation(p, "u_chanLightEnabled");
    entry->loc_chanMatColor     = glGetUniformLocation(p, "u_chanMatColor");
    entry->loc_chanAmbColor     = glGetUniformLocation(p, "u_chanAmbColor");

    /* Texture SRT matrix array (u_texMatrix[0] is the base of the array). */
    entry->loc_texMatrix = glGetUniformLocation(p, "u_texMatrix");
}

/* =========================================================================
 * Public API: init / shutdown / hash
 * ========================================================================= */

int gx_tev_ensure_loaded(void) {
    static int s_loaded = 0;
    static int s_failed = 0;

    if (s_loaded) return 1;
    if (s_failed) return 0;

    /* GLAD must be loaded once, after a GL context is current. gx_shim.c uses
     * the legacy system GL pulled in by GLFW; the modern (core 3.3) entry
     * points used by this file are NULL until GLAD resolves them. */
    if (!gladLoadGL()) {
        printf("[gx_tev] ERROR: gladLoadGL() failed -- modern GL path disabled\n");
        s_failed = 1;
        return 0;
    }

    gx_tev_init();
    s_loaded = 1;
    printf("[gx_tev] GLAD loaded; modern TEV shader path active\n");
    return 1;
}

void gx_tev_init(void) {
    memset(g_shaderCache, 0, sizeof(g_shaderCache));
    g_shaderCacheCount = 0;
    g_cacheHits = 0;
    g_cacheMisses = 0;

    memset(&g_rs, 0, sizeof(g_rs));
    /* Identity matrices so a draw before any matrix upload is still visible. */
    g_rs.proj[0][0] = g_rs.proj[1][1] = g_rs.proj[2][2] = g_rs.proj[3][3] = 1.0f;
    g_rs.modelView[0][0] = g_rs.modelView[1][1] = g_rs.modelView[2][2] = 1.0f;
    g_rs.normalMatrix[0][0] = g_rs.normalMatrix[1][1] = g_rs.normalMatrix[2][2] = 1.0f;
    g_rs.vertexAlphaScale = 1.0f;
    g_rs.lightingEnabled = 0;
    /* Default sun: from the upper-left-front, with strong horizontal (x/z) bite
     * so vertical ruin faces split into lit/shadowed sides. Lower ambient than
     * the old 0.30 flat fill so the 3D form reads. Tunable via PCPORT_LIGHT_*. */
    g_rs.lightDir[0] = 0.55f;
    g_rs.lightDir[1] = 0.50f;
    g_rs.lightDir[2] = 0.55f;
    g_rs.lightAmbient = 0.18f;
    g_rs.exposure = 1.0f;
    g_rs.alphaComp0 = GX_ALWAYS;
    g_rs.alphaComp1 = GX_ALWAYS;
    g_rs.alphaOp = GX_AOP_AND;
    {
        u32 i;
        for (i = 0; i < 4; ++i) {
            g_rs.tevColor[i][0] = g_rs.tevColor[i][1] =
                g_rs.tevColor[i][2] = g_rs.tevColor[i][3] = 0.0f;
            g_rs.konstColor[i][0] = g_rs.konstColor[i][1] =
                g_rs.konstColor[i][2] = g_rs.konstColor[i][3] = 1.0f;
        }
        /* Konst selectors default to 0 (constant 8/8 = 1.0), a no-op for the
         * common case where stages do not read konst. */
        for (i = 0; i < GX_TEV_MAX_STAGES; ++i) {
            g_rs.konstColorSel[i] = 0;
            g_rs.konstAlphaSel[i] = 0;
        }
    }

    /* Texture SRT matrices: default to identity for all 8 slots so UVs pass
     * through unchanged until a scene explicitly sets a non-identity matrix. */
    {
        u32 s;
        for (s = 0; s < 8; ++s) {
            memset(g_rs.texMatrix[s], 0, sizeof(g_rs.texMatrix[s]));
            g_rs.texMatrix[s][0][0] = 1.0f;
            g_rs.texMatrix[s][1][1] = 1.0f;
            g_rs.texMatrix[s][2][2] = 1.0f;
        }
    }

    /* Fog: disabled by default; default color black, sane near/far. */
    g_rs.fogEnable = 0;
    g_rs.fogType = 0;
    g_rs.fogStart = 0.0f;
    g_rs.fogEnd = 1.0f;
    g_rs.fogColor[0] = g_rs.fogColor[1] = g_rs.fogColor[2] = 0.0f;
    g_rs.fogColor[3] = 1.0f;

    /* GX lighting channel: disabled by default; white material, white ambient
     * (so if enabled before a color is set, the channel color is full bright). */
    g_rs.chanLightEnabled = 0;
    g_rs.chanMatColor[0] = g_rs.chanMatColor[1] =
        g_rs.chanMatColor[2] = g_rs.chanMatColor[3] = 1.0f;
    g_rs.chanAmbColor[0] = g_rs.chanAmbColor[1] =
        g_rs.chanAmbColor[2] = g_rs.chanAmbColor[3] = 1.0f;

    printf("[gx_tev] TEV shader cache initialized (max %d entries)\n",
           TEV_SHADER_CACHE_MAX);
}

void gx_tev_unbind(void) {
    if (!g_glReady && g_boundProgram == 0) return;
    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    g_boundProgram = 0;
}

void gx_tev_shutdown(void) {
    u32 i;
    for (i = 0; i < g_shaderCacheCount; ++i) {
        if (g_shaderCache[i].valid && g_shaderCache[i].glProgram != 0) {
            glDeleteProgram((GLuint)g_shaderCache[i].glProgram);
        }
    }
    memset(g_shaderCache, 0, sizeof(g_shaderCache));
    g_shaderCacheCount = 0;

    if (g_vbo != 0) { glDeleteBuffers(1, &g_vbo); g_vbo = 0; }
    if (g_vao != 0) { glDeleteVertexArrays(1, &g_vao); g_vao = 0; }
    g_glReady = 0;
    g_boundProgram = 0;
}

u32 gx_tev_hash(const GXTevState* state) {
    const u8* data = (const u8*)state;
    u32 hash = 0x811c9dc5u;
    u32 i;
    u32 len = sizeof(GXTevState);
    for (i = 0; i < len; ++i) {
        hash ^= data[i];
        hash *= 0x01000193u;
    }
    return hash;
}

/* =========================================================================
 * Public API: compile (real GL) + bind
 * ========================================================================= */

GXTevShaderEntry* gx_tev_compile(const GXTevState* state) {
    u32 hash = gx_tev_hash(state);
    u32 i;
    char fragSrc[TEV_SHADER_SRC_MAX];
    GLuint vs, fs, prog;
    GXTevShaderEntry* entry;

    /* Cache lookup. */
    for (i = 0; i < g_shaderCacheCount; ++i) {
        if (g_shaderCache[i].valid && g_shaderCache[i].stateHash == hash) {
            g_cacheHits++;
            return &g_shaderCache[i];
        }
    }

    g_cacheMisses++;

    if (g_shaderCacheCount >= TEV_SHADER_CACHE_MAX) {
        printf("[gx_tev] ERROR: shader cache full (%d) -- cannot compile\n",
               TEV_SHADER_CACHE_MAX);
        return (GXTevShaderEntry*)0;
    }

    if (gx_tev_generate_fragment_shader(state, fragSrc, sizeof(fragSrc)) < 0) {
        return (GXTevShaderEntry*)0;
    }

    vs = tev_compile_shader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
    if (vs == 0) return (GXTevShaderEntry*)0;

    fs = tev_compile_shader(GL_FRAGMENT_SHADER, fragSrc);
    if (fs == 0) { glDeleteShader(vs); return (GXTevShaderEntry*)0; }

    prog = tev_link_program(vs, fs);
    /* Shaders can be detached/deleted once linked. */
    glDeleteShader(vs);
    glDeleteShader(fs);
    if (prog == 0) return (GXTevShaderEntry*)0;

    entry = &g_shaderCache[g_shaderCacheCount];
    memset(entry, 0, sizeof(*entry));
    entry->stateHash = hash;
    entry->glProgram = (u32)prog;
    entry->valid = 1;
    tev_cache_uniform_locations(entry);

    g_shaderCacheCount++;
    return entry;
}

void gx_tev_bind(const GXTevShaderEntry* entry) {
    GLuint prog;
    if (!entry || !entry->valid) return;

    prog = (GLuint)entry->glProgram;
    if (prog != g_boundProgram) {
        glUseProgram(prog);
        g_boundProgram = prog;
    }

    /* --- Matrices (transpose row-major GCN -> column-major GL) --- */
    if (entry->loc_projMatrix >= 0) {
        glUniformMatrix4fv(entry->loc_projMatrix, 1, GL_TRUE,
                           (const GLfloat*)&g_rs.proj[0][0]);
    }
    if (entry->loc_modelViewMatrix >= 0) {
        /* Expand the 3x4 modelview to 4x4 (last row 0,0,0,1). */
        GLfloat mv[16];
        int r, c;
        for (r = 0; r < 3; ++r) {
            for (c = 0; c < 4; ++c) {
                mv[r * 4 + c] = g_rs.modelView[r][c];
            }
        }
        mv[12] = 0.0f; mv[13] = 0.0f; mv[14] = 0.0f; mv[15] = 1.0f;
        glUniformMatrix4fv(entry->loc_modelViewMatrix, 1, GL_TRUE, mv);
    }

    /* --- Normal matrix (upper-left 3x3 of the normal/modelview matrix) --- */
    if (entry->loc_normalMatrix >= 0) {
        GLfloat nm[9];
        int r, c;
        for (r = 0; r < 3; ++r) {
            for (c = 0; c < 3; ++c) {
                nm[r * 3 + c] = g_rs.normalMatrix[r][c];
            }
        }
        glUniformMatrix3fv(entry->loc_normalMatrix, 1, GL_TRUE, nm);
    }

    /* --- TEV color + konst register arrays --- */
    if (entry->loc_tevColor[0] >= 0) {
        glUniform4fv(entry->loc_tevColor[0], 4, (const GLfloat*)&g_rs.tevColor[0][0]);
    }
    if (entry->loc_tevKonst[0] >= 0) {
        glUniform4fv(entry->loc_tevKonst[0], 4, (const GLfloat*)&g_rs.konstColor[0][0]);
    }

    /* --- Texture samplers (units 0 and 1) --- */
    if (entry->loc_tex[0] >= 0) glUniform1i(entry->loc_tex[0], 0);
    if (entry->loc_tex[1] >= 0) glUniform1i(entry->loc_tex[1], 1);

    /* --- Alpha test --- */
    if (entry->loc_alphaComp0 >= 0) glUniform1i(entry->loc_alphaComp0, g_rs.alphaComp0);
    if (entry->loc_alphaComp1 >= 0) glUniform1i(entry->loc_alphaComp1, g_rs.alphaComp1);
    if (entry->loc_alphaRef0  >= 0) glUniform1f(entry->loc_alphaRef0, g_rs.alphaRef0);
    if (entry->loc_alphaRef1  >= 0) glUniform1f(entry->loc_alphaRef1, g_rs.alphaRef1);
    if (entry->loc_alphaOp    >= 0) glUniform1i(entry->loc_alphaOp, g_rs.alphaOp);

    /* --- Host-only uniforms (now with dedicated locations) --- */
    if (entry->loc_vertexAlphaScale >= 0)
        glUniform1f(entry->loc_vertexAlphaScale, g_rs.vertexAlphaScale);
    if (entry->loc_lightingEnabled >= 0)
        glUniform1i(entry->loc_lightingEnabled, g_rs.lightingEnabled);
    if (entry->loc_lightDir >= 0)
        glUniform3f(entry->loc_lightDir, g_rs.lightDir[0], g_rs.lightDir[1], g_rs.lightDir[2]);
    if (entry->loc_lightAmbient >= 0)
        glUniform1f(entry->loc_lightAmbient, g_rs.lightAmbient);
    if (entry->loc_exposure >= 0)
        glUniform1f(entry->loc_exposure, g_rs.exposure > 0.0f ? g_rs.exposure : 1.0f);

    /* --- Per-stage konst selectors (int arrays) --- */
    if (entry->loc_tevKonstColorSel >= 0)
        glUniform1iv(entry->loc_tevKonstColorSel, GX_TEV_MAX_STAGES,
                     (const GLint*)g_rs.konstColorSel);
    if (entry->loc_tevKonstAlphaSel >= 0)
        glUniform1iv(entry->loc_tevKonstAlphaSel, GX_TEV_MAX_STAGES,
                     (const GLint*)g_rs.konstAlphaSel);

    /* --- Fog --- */
    if (entry->loc_fogEnable >= 0) glUniform1i(entry->loc_fogEnable, g_rs.fogEnable);
    if (entry->loc_fogType   >= 0) glUniform1i(entry->loc_fogType, g_rs.fogType);
    if (entry->loc_fogStart  >= 0) glUniform1f(entry->loc_fogStart, g_rs.fogStart);
    if (entry->loc_fogEnd    >= 0) glUniform1f(entry->loc_fogEnd, g_rs.fogEnd);
    if (entry->loc_fogColor  >= 0)
        glUniform4f(entry->loc_fogColor, g_rs.fogColor[0], g_rs.fogColor[1],
                    g_rs.fogColor[2], g_rs.fogColor[3]);

    /* --- GX lighting channel (ambient + material color) --- */
    if (entry->loc_chanLightEnabled >= 0)
        glUniform1i(entry->loc_chanLightEnabled, g_rs.chanLightEnabled);
    if (entry->loc_chanMatColor >= 0)
        glUniform4f(entry->loc_chanMatColor, g_rs.chanMatColor[0], g_rs.chanMatColor[1],
                    g_rs.chanMatColor[2], g_rs.chanMatColor[3]);
    if (entry->loc_chanAmbColor >= 0)
        glUniform4f(entry->loc_chanAmbColor, g_rs.chanAmbColor[0], g_rs.chanAmbColor[1],
                    g_rs.chanAmbColor[2], g_rs.chanAmbColor[3]);

    /* --- Texture SRT matrices (8 mat3 uniforms, row-major) ---
     * Upload all 8 slots as a single glUniformMatrix3fv array call.
     * GL_TRUE = transpose so our row-major storage matches GL column-major. */
    if (entry->loc_texMatrix >= 0) {
        glUniformMatrix3fv(entry->loc_texMatrix, 8, GL_TRUE,
                           &g_rs.texMatrix[0][0][0]);
    }
}

/* =========================================================================
 * Public API: render state setters
 * ========================================================================= */

void gx_tev_set_proj_matrix(const f32 m[4][4]) {
    memcpy(g_rs.proj, m, sizeof(g_rs.proj));
}

void gx_tev_set_modelview_matrix(const f32 m[3][4]) {
    memcpy(g_rs.modelView, m, sizeof(g_rs.modelView));
}

void gx_tev_set_normal_matrix(const f32 m[3][4]) {
    memcpy(g_rs.normalMatrix, m, sizeof(g_rs.normalMatrix));
}

void gx_tev_set_lighting_enabled(int enabled) {
    g_rs.lightingEnabled = enabled ? 1 : 0;
}

void gx_tev_set_light_params(f32 dx, f32 dy, f32 dz, f32 ambient) {
    g_rs.lightDir[0] = dx;
    g_rs.lightDir[1] = dy;
    g_rs.lightDir[2] = dz;
    if (ambient < 0.0f) ambient = 0.0f;
    if (ambient > 1.0f) ambient = 1.0f;
    g_rs.lightAmbient = ambient;
}

void gx_tev_set_exposure(f32 gain) {
    if (gain <= 0.0f) gain = 1.0f;
    g_rs.exposure = gain;
}

void gx_tev_set_tev_color(u32 id, u8 r, u8 g, u8 b, u8 a) {
    if (id > 3) return;
    g_rs.tevColor[id][0] = (f32)r / 255.0f;
    g_rs.tevColor[id][1] = (f32)g / 255.0f;
    g_rs.tevColor[id][2] = (f32)b / 255.0f;
    g_rs.tevColor[id][3] = (f32)a / 255.0f;
}

void gx_tev_set_konst_color(u32 id, u8 r, u8 g, u8 b, u8 a) {
    if (id > 3) return;
    g_rs.konstColor[id][0] = (f32)r / 255.0f;
    g_rs.konstColor[id][1] = (f32)g / 255.0f;
    g_rs.konstColor[id][2] = (f32)b / 255.0f;
    g_rs.konstColor[id][3] = (f32)a / 255.0f;
}

void gx_tev_set_alpha_compare(u8 comp0, u8 ref0, u8 op, u8 comp1, u8 ref1) {
    g_rs.alphaComp0 = (s32)comp0;
    g_rs.alphaComp1 = (s32)comp1;
    g_rs.alphaRef0 = (f32)ref0 / 255.0f;
    g_rs.alphaRef1 = (f32)ref1 / 255.0f;
    g_rs.alphaOp = (s32)op;
}

void gx_tev_set_vertex_alpha_scale(f32 scale) {
    if (scale < 0.0f) scale = 0.0f;
    if (scale > 1.0f) scale = 1.0f;
    g_rs.vertexAlphaScale = scale;
}

void gx_tev_set_konst_sel(u32 stage, u8 colorSel, u8 alphaSel) {
    if (stage >= (u32)GX_TEV_MAX_STAGES) return;
    g_rs.konstColorSel[stage] = (s32)colorSel;
    g_rs.konstAlphaSel[stage] = (s32)alphaSel;
}

void gx_tev_set_fog(u8 type, f32 startz, f32 endz, u8 r, u8 g, u8 b, u8 a) {
    /* GX_FOG_NONE (0) disables fog; any other type stores its value but only
     * the linear type (2) is computed in the shader. */
    g_rs.fogType = (s32)type;
    g_rs.fogEnable = (type != 0) ? 1 : 0;
    g_rs.fogStart = startz;
    g_rs.fogEnd = endz;
    g_rs.fogColor[0] = (f32)r / 255.0f;
    g_rs.fogColor[1] = (f32)g / 255.0f;
    g_rs.fogColor[2] = (f32)b / 255.0f;
    g_rs.fogColor[3] = (f32)a / 255.0f;
}

void gx_tev_set_chan_lighting(int enabled) {
    g_rs.chanLightEnabled = enabled ? 1 : 0;
}

void gx_tev_set_chan_mat_color(u8 r, u8 g, u8 b, u8 a) {
    g_rs.chanMatColor[0] = (f32)r / 255.0f;
    g_rs.chanMatColor[1] = (f32)g / 255.0f;
    g_rs.chanMatColor[2] = (f32)b / 255.0f;
    g_rs.chanMatColor[3] = (f32)a / 255.0f;
}

void gx_tev_set_chan_amb_color(u8 r, u8 g, u8 b, u8 a) {
    g_rs.chanAmbColor[0] = (f32)r / 255.0f;
    g_rs.chanAmbColor[1] = (f32)g / 255.0f;
    g_rs.chanAmbColor[2] = (f32)b / 255.0f;
    g_rs.chanAmbColor[3] = (f32)a / 255.0f;
}

void gx_tev_set_tex_matrix(u32 slot, const f32 m[3][4]) {
    u32 r, c;
    if (slot >= 8) return;
    if (m == (const f32(*)[4])0) {
        /* NULL -> restore identity. */
        memset(g_rs.texMatrix[slot], 0, sizeof(g_rs.texMatrix[slot]));
        g_rs.texMatrix[slot][0][0] = 1.0f;
        g_rs.texMatrix[slot][1][1] = 1.0f;
        g_rs.texMatrix[slot][2][2] = 1.0f;
        return;
    }
    /* Copy the upper-left 3x3 of the 3x4 input matrix.
     * The GCN TObj SRT matrix is 3x4; column 3 (translation) is irrelevant for
     * UV scrolling encoded as a 2D affine (s,t,1) multiply -- the translation
     * is in columns 0-2 row 2, so the 3x3 already captures it correctly. */
    for (r = 0; r < 3; ++r) {
        for (c = 0; c < 3; ++c) {
            g_rs.texMatrix[slot][r][c] = m[r][c];
        }
    }
}

/* =========================================================================
 * Public API: modern submit path (VAO/VBO)
 * ========================================================================= */

static void tev_ensure_gl_objects(void) {
    if (g_glReady) return;

    glGenVertexArrays(1, &g_vao);
    glGenBuffers(1, &g_vbo);

    glBindVertexArray(g_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);

    /* Attribute layout mirrors GXTevVertex / GXImmVertex:
     *   loc 0: position  (3 x f32)  offset 0
     *   loc 1: color0    (4 x u8 normalized) offset 12
     *   loc 2: texcoord0 (2 x f32)  offset 16
     *   loc 3: normal    (3 x f32)  offset 24
     */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          (GLsizei)sizeof(GXTevVertex),
                          (const void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                          (GLsizei)sizeof(GXTevVertex),
                          (const void*)(3 * sizeof(f32)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          (GLsizei)sizeof(GXTevVertex),
                          (const void*)(3 * sizeof(f32) + 4 * sizeof(u8)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                          (GLsizei)sizeof(GXTevVertex),
                          (const void*)(3 * sizeof(f32) + 4 * sizeof(u8) +
                                        2 * sizeof(f32)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    g_glReady = 1;
}

int gx_tev_submit(const GXTevState* state, u32 glPrim,
                  const GXTevVertex* verts, u32 count,
                  u32 glTexId, int hasTexture) {
    GXTevShaderEntry* entry;
    GLint hasTexLoc;

    if (!state || !verts || count == 0) return 0;

    entry = gx_tev_compile(state);
    if (!entry || !entry->valid || entry->glProgram == 0) return 0;

    tev_ensure_gl_objects();

    /* Bind program + upload uniforms from the current render state. */
    gx_tev_bind(entry);

    /* Per-draw texture availability flag (dedicated u_hasTexture uniform). */
    hasTexLoc = entry->loc_hasTexture;
    if (hasTexLoc >= 0) {
        glUniform1i(hasTexLoc, (hasTexture && glTexId != 0) ? 1 : 0);
    }

    /* Bind the texture on unit 0 (and unit 1 mirrors it for single-tex draws). */
    if (hasTexture && glTexId != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, (GLuint)glTexId);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, (GLuint)glTexId);
        glActiveTexture(GL_TEXTURE0);
    }

    /* Upload vertices to the shared VBO and draw. */
    glBindVertexArray(g_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)((size_t)count * sizeof(GXTevVertex)),
                 verts, GL_STREAM_DRAW);

    glDrawArrays((GLenum)glPrim, 0, (GLsizei)count);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return 1;
}

void gx_tev_get_cache_stats(u32* outHits, u32* outMisses, u32* outTotal) {
    if (outHits) *outHits = g_cacheHits;
    if (outMisses) *outMisses = g_cacheMisses;
    if (outTotal) *outTotal = g_shaderCacheCount;
}


#endif /* __MWERKS__ */
