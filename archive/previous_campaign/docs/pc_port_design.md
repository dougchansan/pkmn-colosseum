# Pokemon Colosseum PC Port -- GX-to-OpenGL Translation Layer Design

**Status:** Phase 3 design + scaffolding (pcport sources and CMake scaffold
exist, but the shim/native build remain stub-heavy)
**Target:** OpenGL 3.3 Core Profile, GLSL 330
**Date:** 2026-03-19

> Audit note (2026-04-02): `src/pcport/` and the pcport `CMakeLists.txt`
> scaffold now exist, but `GAME_SOURCES` and `HSD_SOURCES` are still empty and
> the shim files are still TODO-heavy stubs. See
> [status_audit.md](status_audit.md) for the current repo-backed status summary.

---

## Table of Contents

1. [GX API Surface Analysis](#1-gx-api-surface-analysis)
2. [TEV Combiner Translation](#2-tev-combiner-translation)
3. [Vertex Format Translation](#3-vertex-format-translation)
4. [Display List Translation](#4-display-list-translation)
5. [Texture Format Translation](#5-texture-format-translation)
6. [Framebuffer Operations](#6-framebuffer-operations)
7. [HSD Integration Points](#7-hsd-integration-points)
8. [Recommended Approach and API Mapping](#8-recommended-approach-and-api-mapping)
9. [Audio Replacement (JAudio2 to SDL2)](#9-audio-replacement-jaudio2-to-sdl2)
10. [Input Replacement (PAD to SDL2)](#10-input-replacement-pad-to-sdl2)

---

## 1. GX API Surface Analysis

The following GX functions are referenced across the decompiled codebase. They were
identified by searching all source files in `src/game/`, `src/hsd/`, and `include/`
for GX function names annotated in comments and extern declarations.

### 1.1 Initialization and FIFO

| GX Function | Address | Called From |
|---|---|---|
| `GXInit` | fn_801C021C | `GSgfxInit` (gs_gfx.c) |
| `GXSetDrawDoneCallback` | fn_801C01C8 | `GSgfxInit` -- registers `GSgfx_DrawDoneCallback` |
| `GXSetDispCopyGamma` | fn_801BF4C4 | `GSgfxInit` -- sets gamma=1 |

### 1.2 Viewport, Scissor, Projection

| GX Function | Address | Called From |
|---|---|---|
| `GXSetViewport` | fn_800AA2F0 | `GSgfx_PreRetraceCallback`, `GSgfx_BeginFrame` |
| `GXSetProjection` | fn_800BD640 | `GSgfx_PreRetraceCallback`, `GSgfx_BeginFrame` |
| `GXSetScissor` | fn_800B962C | `GStextureUploadFromBuffer`, effect rendering |

### 1.3 Matrix Operations

| GX Function | Address | Called From |
|---|---|---|
| `GXLoadPosMtxImm` | fn_800BD744 | `GSgfx_ConfigurePipeline`, matrix stack push/pop |
| `GXLoadNrmMtxImm` | (in pipeline) | Normal matrix loading for lighting |

These are wrapped by ~80 small GSgfx matrix accessor functions in the
0x800D5504--0x800D7900 range. The game maintains its own model-view matrix stack
(`GSGFX_MAX_MTX_DEPTH = 32`) and only flushes to GX when dirty flags are set.

### 1.4 TEV / Blend / Alpha / Z / Fog State

| GX Function (Wrapped) | GS Wrapper | Called From |
|---|---|---|
| `GXSetTevOp` / `GXSetTevColorIn` / `GXSetTevAlphaIn` | `GSgfx_ConfigureTEV` (fn_800DA028) | `GSgfxInit`, material setup, effect render |
| `GXSetBlendMode` | `GSgfx_ConfigureBlend` (fn_800DA2BC) | `GSgfxInit`, material setup |
| `GXSetAlphaCompare` | `GSgfx_ConfigureAlpha` (fn_800DA100) | `GSgfxInit`, material setup |
| `GXSetZMode` | `GSgfx_ConfigureZ` (fn_800DA1E8) | `GSgfxInit`, material setup |
| `GXSetFog` | `GSgfx_ConfigureFog` (fn_800D9F40) | `GSgfxInit`, per-scene fog |
| `GXSetCullMode` | fn_800D6A00 | Effect rendering, draw dispatch |
| `GXSetChanCtrl` | fn_80101B90 | Main render loop |

The default render state set by `GSgfxInit` is:
- Blend: src=1, dst=1, op=1 (additive)
- Z: enable, func=LEQUAL, update=1
- Alpha compare: ref0=0, comp0=ALWAYS, ref1=0, comp1=ALWAYS, op=AND
- TEV: mode 2
- Fog: disabled

### 1.5 Texture State

| GX Function | Address | Called From |
|---|---|---|
| `GXInitTexObj` | fn_800BA9E4 | `GStextureCreate`, `GStextureSetupFromTPL` |
| `GXInitTlutObj` | fn_800BB050 | `GStextureCreate`, `GStextureSetupFromTPL` |
| `GXInvalidateTexAll` | fn_800BB29C | `GStextureFlush`, `GStextureUploadFromBuffer`, gs_render.c |
| `GXCopyTex` | fn_800B9FE4 | `GStextureUploadFromBuffer` (EFB copy to texture) |
| `GXSetCopyTexSrc` | fn_800B96F8 | `GStextureUploadFromBuffer` |
| `GXPixModeSync` | fn_800B8E74 | `GStextureUploadFromBuffer` |

### 1.6 Lighting

| GX Function (Wrapped) | GS Wrapper | Called From |
|---|---|---|
| `GXInitLightObj` / `GXLoadLightObj` | `GSgfx_InitLighting` (fn_800DB890) | `GSgfxInit` |
| `GXSetChanCtrl` | light command dispatch (fn_800DC0D4) | Lighting pipeline |
| Light position/color/attenuation | fn_800DB890--fn_800DC224 range | Per-scene light setup |

The game supports up to 8 hardware lights (`GSGFX_MAX_LIGHTS = 8`). HSD LObj
wraps a 64-byte `GXLightObj` and a specular `GXLightObj` per light.

### 1.7 Draw Commands

| GX Function | Address | Called From |
|---|---|---|
| `GXBegin` / `GXEnd` | fn_800B8DF4 / fn_800B856C | Effect rendering, draw dispatch |
| `GXPosition3f32` | fn_800D67BC | Effect rendering vertex submission |
| `GXColor4u8` | fn_800D6680 | Effect rendering color submission |
| `GXTexCoord2f32` | fn_800D5CB8 | Effect rendering texcoord submission |
| `GXCallDisplayList` | (in draw dispatch) | `GSgfx_DrawDispatch` (fn_800E1544, 2792 bytes) |

### 1.8 Framebuffer / Copy

| GX Function | Address | Called From |
|---|---|---|
| `GXCopyDisp` | (in swap buffers) | `GSgfxSwapBuffers` |
| `GXSetCopyFilter` / `GXSetDispCopyDst` | fn_800BCE88 | `GStextureUploadFromBuffer` |

### 1.9 Summary: Total GX Surface

Approximately **35 distinct GX functions** are used, but many are accessed only
through the GSgfx wrapper layer (gs_render.c, gs_gfx.c). The game does NOT call
GX directly from gameplay code -- all GX access is routed through:

1. **GSgfx** (gs_gfx.c, gs_render.c) -- 278 functions wrapping viewport, matrix,
   TEV, blend, Z, fog, lighting, and draw dispatch
2. **GStexture** (gs_texture.c) -- texture creation, binding, upload
3. **HSD** (hsd_*.c) -- material setup, display object rendering, camera
4. **Effect system** (effect_visual.c) -- 12 visual effect sub-modules with
   direct GX pipeline setup

---

## 2. TEV Combiner Translation

The GCN TEV (Texture Environment) is a fixed-function, multi-stage texture
combiner with up to 16 stages. Each stage has separate color and alpha equations.

### 2.1 TEV Stage Structure

Each TEV stage computes:

```
color_out = (d + lerp(a, b, c)) * scale + bias
alpha_out = (d + lerp(a, b, c)) * scale + bias
```

Where a, b, c, d are selected from: previous stage output, texture sample,
rasterized color, constant color register, zero, or one.

### 2.2 Colosseum TEV Usage

From the decompiled code, Colosseum uses TEV through three paths:

1. **GSgfx_ConfigureTEV** (fn_800DA028): Sets a "TEV mode" integer (seen with
   mode=2 in `GSgfxInit`). This is a pre-configured preset system -- not
   arbitrary TEV programming.

2. **GSmaterial_Create** (fn_800DE680, 2376 bytes): The material system configures
   TEV stages based on material properties and texture count. References
   environment mapping and PE descriptor configuration.

3. **HSD MObj TEV**: The HSD material object has `setup_tev` and `make_texp`
   virtual methods. The TObj flags define colormap and alphamap modes:
   - `TEX_COLORMAP_MODULATE` (4<<16): `color = tex * prev`
   - `TEX_COLORMAP_REPLACE` (5<<16): `color = tex`
   - `TEX_COLORMAP_ADD` (7<<16): `color = tex + prev`
   - `TEX_COLORMAP_BLEND` (3<<16): `color = lerp(prev, tex, blend)`
   - `TEX_ALPHAMAP_MODULATE` (3<<20): `alpha = tex_a * prev_a`
   - `TEX_ALPHAMAP_REPLACE` (4<<20): `alpha = tex_a`

### 2.3 GLSL Translation Strategy

Generate fragment shaders at runtime based on the TEV configuration. Each unique
TEV state produces a shader key; shaders are cached in a hash map.

```glsl
#version 330 core

// Per-stage uniforms (populated from TEV state)
uniform sampler2D u_tex[8];
uniform vec4 u_tevColor[4];   // GX TEV color registers (C0-C3)
uniform vec4 u_tevKonst[4];   // GX constant color registers (K0-K3)
uniform vec4 u_matAmbient;
uniform vec4 u_matDiffuse;

in vec4 v_color0;             // rasterized vertex color (channel 0)
in vec4 v_color1;             // rasterized vertex color (channel 1)
in vec2 v_texcoord[8];

out vec4 fragColor;

void main() {
    vec4 prev = v_color0;     // CPREV starts as rasterized color

    // --- TEV Stage 0 (example: MODULATE) ---
    vec4 texSample0 = texture(u_tex[0], v_texcoord[0]);
    prev.rgb = texSample0.rgb * prev.rgb;
    prev.a   = texSample0.a * prev.a;

    // --- TEV Stage 1..N generated similarly ---

    fragColor = prev;
}
```

The shader generator maps each TEV stage's `(colorIn_a, colorIn_b, colorIn_c,
colorIn_d, colorOp, colorScale, colorBias)` and alpha equivalents to GLSL
arithmetic. This is the same approach used by Dolphin Emulator's shader
generation and the encounter/aurora library from the Metroid Prime decomp.

### 2.4 Preset Optimization

Since Colosseum uses a limited set of TEV configurations (estimated 20-40 unique
combinations from the preset system + HSD material flags), we can:

1. Run the game through all scenes, log every TEV state hash
2. Pre-compile the shader variants as static GLSL source
3. Fall back to runtime generation only for unexpected combinations

---

## 3. Vertex Format Translation

### 3.1 GX Vertex Descriptors

The HSD PObj stores vertex data via `HSD_VtxDescList`:

```c
struct HSD_VtxDescList {
    u32 attr;       // GXAttr (GX_VA_POS, GX_VA_NRM, GX_VA_CLR0, GX_VA_TEX0, etc.)
    u32 attr_type;  // GXAttrType (NONE, DIRECT, INDEX8, INDEX16)
    u32 comp_cnt;   // GXCompCnt (POS_XY/XYZ, NRM_XYZ, CLR_RGB/RGBA, TEX_S/ST)
    u32 comp_type;  // GXCompType (U8, S8, U16, S16, F32)
    u8  frac;       // fractional bits for fixed-point
    u16 stride;     // stride in bytes
    void* vertex;   // pointer to vertex data array
};
```

### 3.2 OpenGL VAO/VBO Translation

For each unique `HSD_VtxDescList` configuration, create a corresponding OpenGL
Vertex Array Object:

| GXAttr | GL Attribute | Location | Components | Notes |
|---|---|---|---|---|
| `GX_VA_POS` | `a_position` | 0 | 2 or 3 (XY/XYZ) | Always present |
| `GX_VA_NRM` | `a_normal` | 1 | 3 (XYZ) | For lighting |
| `GX_VA_CLR0` | `a_color0` | 2 | 3 or 4 (RGB/RGBA) | Vertex color channel 0 |
| `GX_VA_CLR1` | `a_color1` | 3 | 3 or 4 | Vertex color channel 1 |
| `GX_VA_TEX0`..`TEX7` | `a_texcoord0`..`7` | 4..11 | 1 or 2 (S/ST) | Texture coordinates |

**Index handling:** GX supports INDEX8 and INDEX16 attribute types, where the
vertex data is an array of indices into a separate data array. For OpenGL:

- **DIRECT mode:** Copy vertex data directly into a VBO. Straightforward.
- **INDEX8/INDEX16 mode:** De-index the vertex data on the CPU at load time,
  expanding indexed attributes into a flat interleaved VBO. This trades memory
  for simplicity and avoids the need for GX-style indexed attribute lookups.

**Fixed-point conversion:** GX `comp_type` S8/U8/S16/U16 with a `frac` value
means the hardware divides by `2^frac`. We apply this scale factor when
uploading to the VBO (converting to float).

### 3.3 Skinning

The PObj supports three skinning modes:

- **RIGID (1):** Single joint reference. Apply the joint's world matrix as a
  uniform. Simple MVP multiplication in the vertex shader.
- **ENVELOPE (2):** Multi-joint weighted skinning. Upload bone matrices as a
  uniform array. Perform weighted blending in the vertex shader:
  ```glsl
  vec4 skinnedPos = vec4(0.0);
  for (int i = 0; i < numWeights; i++) {
      skinnedPos += u_boneMatrix[boneIndex[i]] * a_position * weight[i];
  }
  ```
- **SHAPE_ANIM:** Morph targets. Interpolate between shape keys on the CPU,
  upload the blended result as a regular VBO.

---

## 4. Display List Translation

### 4.1 GX Display Lists

GX display lists are pre-compiled GPU command buffers stored in the PObj's
`display` field. The `n_display` field gives the byte count. These contain:

- Primitive type byte (triangles=0x90, tristrips=0x98, trifans=0xA0, quads=0x80)
- Vertex count (u16)
- Vertex index data (per the vertex descriptor format)

`GSgfx_DrawDispatch` (fn_800E1544, 2792 bytes) is the main interpreter.

### 4.2 Translation Strategy

At load time (when `HSD_PObjLoadDesc` is called), parse each PObj's display list:

1. Read the primitive type byte
2. Read the vertex count
3. For each vertex, read attribute indices/values according to the VtxDescList
4. De-index all attributes, building a flat vertex buffer
5. Convert GX primitives to GL primitives:

| GX Primitive | GL Primitive | Notes |
|---|---|---|
| `GX_TRIANGLES` (0x90) | `GL_TRIANGLES` | Direct 1:1 |
| `GX_TRIANGLESTRIP` (0x98) | `GL_TRIANGLE_STRIP` | Direct 1:1 |
| `GX_TRIANGLEFAN` (0xA0) | `GL_TRIANGLE_FAN` | Direct 1:1 |
| `GX_QUADS` (0x80) | `GL_TRIANGLES` | Split each quad into 2 triangles |
| `GX_LINES` (0xA8) | `GL_LINES` | Direct 1:1 |
| `GX_LINESTRIP` (0xB0) | `GL_LINE_STRIP` | Direct 1:1 |
| `GX_POINTS` (0xB8) | `GL_POINTS` | Direct 1:1 |

6. Upload the vertex data into a VBO, create the VAO, store the draw parameters
   (primitive type, vertex count, base vertex offset)
7. At render time, `glDrawArrays` or `glDrawElements` replaces `GXCallDisplayList`

### 4.3 Immediate Mode Vertices

The effect system (effect_visual.c) submits vertices via immediate-mode GX calls:
`GXPosition3f32`, `GXColor4u8`, `GXTexCoord2f32`. These are wrapped by GSgfx
functions at fn_800D67BC, fn_800D6680, fn_800D5CB8.

For these, maintain a dynamic VBO that accumulates vertices between `GXBegin` and
`GXEnd` calls, then issue a single `glDrawArrays` at `GXEnd`.

---

## 5. Texture Format Translation

### 5.1 Format Mapping

Colosseum uses the following GCN texture formats, confirmed by the
`GStextureGetGXFormat` switch statement and `GStextureCreate`:

| GS Format | GXTexFmt | BPP | GL Internal Format | GL Upload Format | Notes |
|---|---|---|---|---|---|
| 0x00 (I4) | `GX_TF_I4` | 4 | `GL_R8` | `GL_RED` | Decode from 4-bit, swizzle R->RGBA in shader |
| 0x01 (I8) | `GX_TF_I8` | 8 | `GL_R8` | `GL_RED` | Swizzle R->RGBA in shader |
| 0x30 (IA4) | `GX_TF_IA4` | 8 | `GL_RG8` | `GL_RG` | R=intensity, G=alpha |
| 0x40 (IA8) | `GX_TF_I4` variant | 8 | `GL_RG8` | `GL_RG` | Same decode |
| 0x41 (RGB565) | `GX_TF_RGB565` | 16 | `GL_RGB565` | `GL_RGB` / `GL_UNSIGNED_SHORT_5_6_5` | Direct |
| 0x42 (RGB5A3) | `GX_TF_RGB5A3` | 16 | `GL_RGBA8` | `GL_RGBA` | Decode: if MSB=1, RGB555+opaque; else RGBA4443 |
| 0x43 (RGBA8) | `GX_TF_RGBA8` | 32 | `GL_RGBA8` | `GL_RGBA` | De-tile from GCN layout |
| 0x44 (CI4) | `GX_TF_C4` | 4 | `GL_RGBA8` | `GL_RGBA` | Decode via TLUT on CPU |
| 0x45 (CI14x2) | `GX_TF_C14X2` | 16 | `GL_RGBA8` | `GL_RGBA` | Decode via TLUT on CPU |
| 0x90 (CI8) | `GX_TF_C8` | 8 | `GL_RGBA8` | `GL_RGBA` | Decode via TLUT on CPU |
| 0xA0 (A8) | special (0x27) | 8 | `GL_R8` | `GL_RED` | Alpha-only, shader swizzle |
| 0xB0 (CMPR) | `GX_TF_CMPR` | 4 | `GL_COMPRESSED_RGBA_S3TC_DXT1_EXT` | N/A | GCN CMPR = DXT1 with swapped sub-blocks |

### 5.2 Decoding Pipeline

All GCN textures are stored in a tiled/swizzled layout. The decoding pipeline:

1. **De-tile:** GCN textures are stored in 4x4 or 8x4 tiles (format-dependent).
   Rearrange to linear row-major order.
2. **Format convert:**
   - CI4/CI8/CI14x2: Look up the TLUT palette (already decompiled in
     `GStextureConvertCI`) and produce RGBA8.
   - RGB5A3: Decode the two-mode format (1-bit flag + 15-bit color).
   - CMPR: Swap the two 4x4 sub-blocks within each 8x8 DXT1 block to match
     the S3TC/DXT1 layout, then upload as `GL_COMPRESSED_RGBA_S3TC_DXT1_EXT`.
   - I4/I8/IA4/A8: Upload as GL_R8 or GL_RG8 and handle expansion in the
     fragment shader via a texture swizzle uniform.
3. **Upload:** `glTexImage2D` or `glCompressedTexImage2D`.
4. **Mipmaps:** The GStexture system stores per-mip offsets (`mipOffsets[8]`).
   Upload each level with `glTexImage2D` level parameter.

### 5.3 Texture Swizzle Shader Support

For intensity and alpha-only formats, the fragment shader needs a per-texture
swizzle mode:

```glsl
uniform int u_texSwizzle[8]; // 0=RGBA, 1=RRRR (I), 2=RRRA (IA), 3=111R (A)

vec4 applySwizzle(vec4 raw, int mode) {
    if (mode == 1) return vec4(raw.r, raw.r, raw.r, raw.r);
    if (mode == 2) return vec4(raw.r, raw.r, raw.r, raw.g);
    if (mode == 3) return vec4(1.0, 1.0, 1.0, raw.r);
    return raw;
}
```

---

## 6. Framebuffer Operations

### 6.1 Embedded Framebuffer (EFB) Copy

GCN's EFB is a fixed-size on-chip framebuffer. Colosseum uses EFB copy for:

- **Render-to-texture:** `GStextureUploadFromBuffer` copies the EFB to a
  texture via `GXCopyTex`. Translate to: render to an FBO, then use the FBO's
  color attachment as a texture.
- **XFB swap:** `GSgfxSwapBuffers` copies the EFB to the external framebuffer.
  On PC: `glfwSwapBuffers()`.
- **Blur/distortion effects:** The blur and distortion effects in
  effect_visual.c capture the screen into a texture for post-processing.
  Implement as: render scene to FBO, bind FBO texture, render fullscreen quad
  with post-process shader.

### 6.2 Z-Buffer

GX Z-buffering is configured by `GSgfx_ConfigureZ` with parameters:
- enable (bool), compare function (LEQUAL default), z-update (write enable)

Map directly to:

```c
glEnable(GL_DEPTH_TEST);           // enable
glDepthFunc(GL_LEQUAL);            // func
glDepthMask(GL_TRUE);              // update
```

HSD render mode flags that affect Z:
- `RENDER_ZMODE_ALWAYS` (1<<27): `glDepthFunc(GL_ALWAYS)`
- `RENDER_NO_ZUPDATE` (1<<29): `glDepthMask(GL_FALSE)`

### 6.3 Alpha Test

GX alpha test configured by `GSgfx_ConfigureAlpha` with 6 parameters mapping to
`GXSetAlphaCompare(comp0, ref0, op, comp1, ref1)`.

OpenGL 3.3 core profile does NOT have fixed-function alpha test. Implement in
the fragment shader:

```glsl
uniform int u_alphaComp0;   // GX_ALWAYS=7, GX_LESS=1, GX_GREATER=4, etc.
uniform int u_alphaComp1;
uniform float u_alphaRef0;
uniform float u_alphaRef1;
uniform int u_alphaOp;      // GX_AOP_AND=0, GX_AOP_OR=1, GX_AOP_XOR=2

bool alphaTest(float a) {
    bool pass0 = compareFunc(u_alphaComp0, a, u_alphaRef0);
    bool pass1 = compareFunc(u_alphaComp1, a, u_alphaRef1);
    if (u_alphaOp == 0) return pass0 && pass1;
    if (u_alphaOp == 1) return pass0 || pass1;
    return pass0 ^^ pass1;
}
// In main(): if (!alphaTest(fragColor.a)) discard;
```

### 6.4 Blend Mode

`GSgfx_ConfigureBlend` wraps `GXSetBlendMode(type, src_factor, dst_factor, logic_op)`.
Map to:

```c
glEnable(GL_BLEND);
glBlendFunc(translateSrcFactor(src), translateDstFactor(dst));
glBlendEquation(translateBlendOp(type));
```

The HSD PEDesc provides per-material blend overrides (`src_factor`, `dst_factor`,
`logic_op`). The `RENDER_XLU` flag (1<<30) enables translucent blending.

---

## 7. HSD Integration Points

The HSD (HAL SysDolphin) library is still mixed C + asm-backed wrappers in the
current tree. It is still the primary rendering abstraction and the best place
to insert the GL shim.

### 7.1 Rendering Call Chain

```
Game code
  -> HSD_GObj render callback (hsd_gobj.c)
    -> HSD_JObj display (hsd_jobj.c) -- joint hierarchy traversal
      -> HSD_DObjDisplayFunc2 (hsd_displayfunc.c) -- render pass dispatch
        -> Render pass (XLU/OPA/EFB/Billboard)
          -> HSD_MObjSetup (hsd_mobj.c) -- material + TEV setup
            -> HSD_TObjSetup (hsd_tobj.c) -- texture binding
          -> HSD_PObjDisp (hsd_pobj.c) -- vertex/display list submission
            -> GXCallDisplayList / GX immediate mode
          -> HSD_MObjUnset -- restore material state
```

### 7.2 Shim Insertion Points

The GX shim should intercept at these specific HSD function boundaries:

| HSD Function | Role | Shim Action |
|---|---|---|
| `HSD_CObjSetCurrent` | Begin rendering with camera | Set GL viewport, scissor, projection matrix uniform |
| `HSD_CObjEndCurrent` | End camera rendering | (Restore defaults if needed) |
| `HSD_MObjSetup` | Bind material state | Compile/bind TEV shader, set blend/alpha/Z state, bind textures |
| `HSD_TObjSetup` | Bind a texture to a TEV stage | `glBindTexture` + set sampler uniforms |
| `HSD_PObjDisp` | Submit geometry | `glDrawArrays` from pre-built VBO/VAO |
| `HSD_LObjSetup` | Configure a hardware light | Set light uniform (position, color, attenuation) |
| `HSD_FogSet` | Configure distance fog | Set fog uniforms in shader |

### 7.3 GSmaterial Integration

The Genius Sonority material system (`GSmaterial_Create`, 2376 bytes at
fn_800DE680) sits on top of HSD MObj. It handles:

- PE descriptor configuration (blend, alpha compare, Z mode)
- Texture format validation (environment map format check)
- TEV mode selection

The GSmaterial system produces HSD MObj instances. The shim intercepts at the
HSD MObj level, so GSmaterial works unchanged.

### 7.4 Effect System Integration

The 12 visual effects in effect_visual.c bypass HSD and call GSgfx wrappers
directly. Each effect's render callback does:

1. `GSgfx_ConfigureTEV` / `GSgfx_ConfigureBlend` / `GSgfx_ConfigureZ`
2. Matrix operations (fn_800D7F14 load matrix, fn_800D7820 draw begin)
3. Immediate mode vertex submission (fn_800D67BC, fn_800D6680, fn_800D5CB8)
4. `GXBegin` / `GXEnd`

The shim must also intercept the GSgfx wrapper layer for these cases.

---

## 8. Recommended Approach and API Mapping

### 8.1 Architecture

Use the **gx2gl shim approach** (as proven by the ACGC-PC-Port), enhanced with
ideas from the encounter/aurora library:

```
+-------------------+
|   Game Code       |  Unchanged decompiled C
|   (GS engine)     |
+-------------------+
        |
+-------------------+
|   HSD Library     |  Mixed C + asm-backed wrappers
|   (hsd_*.c)       |
+-------------------+
        |
+-------------------+
|   gx2gl Shim      |  NEW: translates GX calls to OpenGL
|   (gx2gl.c/h)     |
+-------------------+
        |
+-------------------+
|   OpenGL 3.3      |  GLFW + GLAD
|   + GLSL 330      |
+-------------------+
```

### 8.2 Shim File Structure

```
src/pcport/
    gx2gl.c          -- GX function shim implementations
    gx2gl.h          -- GX function declarations matching SDK prototypes
    gx2gl_shader.c   -- TEV-to-GLSL shader generator and cache
    gx2gl_texture.c  -- GCN texture decode and GL upload
    gx2gl_state.c    -- GL state tracking (avoid redundant state changes)
    vi2glfw.c        -- VI/framebuffer -> GLFW window
    mtx2gl.c         -- MTX library -> glm or custom math
    pcport_main.c    -- Entry point, GLFW window init, main loop
```

### 8.3 Core API Mapping Table

| GX Function | OpenGL 3.3 Equivalent |
|---|---|
| **Initialization** | |
| `GXInit` | `glfwInit` + `gladLoadGL` + GL state defaults |
| `GXSetDrawDoneCallback` | (not needed -- GL is synchronous) |
| `GXSetDispCopyGamma` | (handled by window system / sRGB framebuffer) |
| **Viewport / Scissor** | |
| `GXSetViewport` | `glViewport(x, y, w, h)` + `glDepthRange(near, far)` |
| `GXSetScissor` | `glScissor(x, y, w, h)` + `glEnable(GL_SCISSOR_TEST)` |
| **Projection / Matrix** | |
| `GXSetProjection` | Upload to `u_projMatrix` uniform |
| `GXLoadPosMtxImm` | Upload to `u_modelViewMatrix` uniform |
| `GXLoadNrmMtxImm` | Upload to `u_normalMatrix` uniform (inverse transpose) |
| **TEV / Material** | |
| `GXSetTevOp` | Select pre-compiled GLSL shader variant |
| `GXSetTevColorIn` / `GXSetTevAlphaIn` | Shader generator input |
| `GXSetTevColorOp` / `GXSetTevAlphaOp` | Shader generator input |
| `GXSetTevColor` | `glUniform4f(u_tevColor[n], ...)` |
| `GXSetTevKColor` | `glUniform4f(u_tevKonst[n], ...)` |
| `GXSetNumTevStages` | Shader variant selection key |
| **Blend / Alpha / Z** | |
| `GXSetBlendMode` | `glBlendFunc` + `glBlendEquation` |
| `GXSetAlphaCompare` | Fragment shader `discard` |
| `GXSetZMode` | `glDepthFunc` + `glDepthMask` + `glEnable(GL_DEPTH_TEST)` |
| `GXSetZCompLoc` | (before/after texture -- affects discard placement) |
| **Fog** | |
| `GXSetFog` | Fog uniforms in fragment shader |
| **Texture** | |
| `GXInitTexObj` | `glGenTextures` + `glTexParameteri` (wrap, filter) |
| `GXInitTlutObj` | (Decode TLUT on CPU -- no GL equivalent) |
| `GXLoadTexObj` | `glActiveTexture` + `glBindTexture` |
| `GXInvalidateTexAll` | (no-op on PC -- texture cache is coherent) |
| `GXCopyTex` | Render to FBO, `glCopyTexSubImage2D` or read FBO attachment |
| **Lighting** | |
| `GXInitLightObj` | Set per-light uniform struct |
| `GXLoadLightObj` | Upload light data to uniform buffer |
| `GXSetChanCtrl` | Enable/disable per-channel lighting in shader |
| `GXSetChanAmbColor` | `glUniform4f(u_ambientColor, ...)` |
| `GXSetChanMatColor` | `glUniform4f(u_matColor, ...)` |
| **Drawing** | |
| `GXBegin(prim, vtxfmt, count)` | Begin accumulating into dynamic VBO |
| `GXPosition3f32` / `GXColor4u8` / `GXTexCoord2f32` | Write to VBO |
| `GXEnd` | `glDrawArrays(translatePrim(prim), 0, count)` |
| `GXCallDisplayList` | `glDrawArrays` from pre-built static VBO |
| `GXSetCullMode` | `glEnable(GL_CULL_FACE)` + `glCullFace` |
| **Framebuffer** | |
| `GXCopyDisp` | `glfwSwapBuffers` |
| `GXSetCopyFilter` | (not needed -- multisampling handled differently) |
| `GXPixModeSync` | `glFinish` (or `glMemoryBarrier` -- rarely needed) |

### 8.4 Vertex Shader Template

```glsl
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec4 a_color0;
layout(location = 3) in vec4 a_color1;
layout(location = 4) in vec2 a_texcoord0;
layout(location = 5) in vec2 a_texcoord1;
// ... up to texcoord7

uniform mat4 u_projMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat3 u_normalMatrix;

// Lighting uniforms
struct Light {
    vec3 position;
    vec3 direction;
    vec4 color;
    float cosAtten;      // spot cutoff
    float distAtten;     // distance attenuation
};
uniform Light u_lights[8];
uniform int u_numLights;
uniform vec4 u_ambientColor;
uniform vec4 u_matColor;

out vec4 v_color0;
out vec4 v_color1;
out vec2 v_texcoord[8];
out vec3 v_normal;
out vec3 v_viewPos;

void main() {
    vec4 viewPos = u_modelViewMatrix * vec4(a_position, 1.0);
    gl_Position = u_projMatrix * viewPos;
    v_viewPos = viewPos.xyz;
    v_normal = u_normalMatrix * a_normal;

    // Channel 0: diffuse lighting (when enabled)
    v_color0 = a_color0; // or computed from lighting
    v_color1 = a_color1;

    v_texcoord[0] = a_texcoord0;
    v_texcoord[1] = a_texcoord1;
    // ... generated per vertex format
}
```

### 8.5 Dependencies

| Library | Purpose | Version |
|---|---|---|
| GLFW 3.3+ | Window creation, GL context, input events | 3.3.8 |
| GLAD | OpenGL 3.3 function loader | Generated for GL 3.3 core |
| SDL2 | Audio output, game controller input | 2.28+ |
| stb_image | (Optional) texture format debugging | Latest |

### 8.6 Build System Integration

Add a `PCPORT=1` define to the existing configure.py. When set:
- Compile `src/pcport/*.c` instead of `src/dolphin/` SDK stubs
- Link against GLFW, GLAD, SDL2 instead of GCN SDK libraries
- Define `PCPORT` preprocessor macro for conditional compilation in shared code

---

## 9. Audio Replacement (JAudio2 to SDL2)

### 9.1 Current Sound Architecture

The sound system is fully decompiled and matching (66 functions across
sound.c, sound_bgm.c, sound_se.c). It wraps Nintendo's JAudio2 middleware:

**Public API (sound.h):**
- `sndInit(numBgm, numSe, numBgmRes, numSe3d, numStreams)` -- allocates pools, inits JAudio
- `sndShutdown()` -- stops all sounds, frees resources
- `sndPlayBgm(sndId, fadeTime, volume)` / `sndPlayBgmStream(sndId, fadeTime, volume)`
- `sndPlaySe(listenerParams)` / `sndPlaySe3D(sndId, pos)` / `sndPlaySe3DFull(...)`
- `sndStop(handle, sndId)` / `sndStopAll()` / `sndStopAllSe()`
- `sndSetMasterVolume(volume, pan, applyBgm, applySe)`
- `sndSetSurroundMode(mode)` / `sndSetDspMix(chorus, reverb, delay, wet)`

**Internal state:**
- `SndWork` entries (0x0C bytes each) track per-sound metadata
- BGM categories: `SND_CATEGORY_BGM` (0x0406), `SND_CATEGORY_SE` (0x0407), `SND_CATEGORY_STREAM` (0x0408)
- Crossfade system with timed fade loops (`_sndFadeBgm`)
- 3D positional audio via `SndListener` structs
- Wave data loaded from FSYS archives (WAVE format)

### 9.2 JAudio2 Functions to Replace

The sound system calls approximately 20 JAudio2 functions. These are the
low-level functions that actually interact with the DSP hardware:

| JAudio2 Function | Address | Replacement |
|---|---|---|
| JAudio channel alloc | fn_800F9318 | `SDL_mixer` channel allocation |
| JAudio channel release | fn_800F9378 | Free SDL channel |
| JAudio channel free | fn_800F9210 | `Mix_HaltChannel` |
| JAudio start playback | fn_800F9418 | `Mix_PlayChannel` |
| JAudio stop | fn_8014D598 | `Mix_HaltChannel` |
| JAudio is playing | fn_8014D5C8 | `Mix_Playing(channel)` |
| JAudio pause | fn_8014D648 | `Mix_Pause` |
| JAudio set volume | fn_8014D6D8 | `Mix_Volume(channel, vol)` |
| JAudio resume | fn_8014D880 | `Mix_Resume` |
| JAudio set callback | fn_8014D8C0 | SDL audio callback |
| JAudio set params | fn_8014D8C8 | Volume/pan/effect params |
| JAudio set DSP mix | fn_8014D928 | SDL_mixer effects chain |
| JAudio flush | fn_8014D9BC | (no-op or `Mix_ExpireChannel`) |
| JAudio register update | fn_8014DAA8 | Register SDL audio thread callback |

### 9.3 SDL2_mixer Translation Plan

```
src/pcport/
    snd2sdl.c    -- JAudio2 function shims -> SDL2_mixer
    snd2sdl.h    -- Function declarations matching JAudio prototypes
```

1. **Init:** Replace JAudio init in `sndInit` with:
   ```c
   SDL_Init(SDL_INIT_AUDIO);
   Mix_OpenAudio(48000, AUDIO_S16SYS, 2, 2048);
   Mix_AllocateChannels(64);  // match JAudio maxVoices=64
   ```

2. **Wave data:** The game's WAVE data (loaded from FSYS archives) is in a
   Nintendo-specific ADPCM format. Decode to PCM16 at load time, then create
   `Mix_Chunk` objects.

3. **Streaming BGM:** Replace JAudio streaming with `Mix_LoadMUS` for streaming
   audio. The 64KB streaming buffer (`SND_STREAM_BUFFER_SIZE`) maps to
   SDL_mixer's built-in streaming.

4. **3D audio:** The game's 3D audio is implemented in software (`SndListener`
   with position, direction, falloff). Keep the existing attenuation math and
   just convert the final volume/pan values to SDL_mixer calls.

5. **DSP effects:** Chorus/reverb/delay from `sndSetDspMix` can be approximated
   with SDL_mixer's `Mix_RegisterEffect` or dropped initially (low priority for
   a first playable build).

6. **Crossfade:** The existing `_sndFadeBgm` / `_sndCrossfadeOut` logic works
   at the game logic level and only needs the JAudio volume/stop shims to work.

### 9.4 Audio Format Conversion

GCN audio resources use Nintendo ADPCM encoding. At build time or first-run:
1. Extract all `.aw` / `.bms` / WAVE resources from the FSYS archives
2. Decode ADPCM to PCM16 WAV files
3. Load as `Mix_Chunk` (SE) or `Mix_Music` (BGM)

The `dspadpcm` decoder from the GCN SDK documentation (or open-source
equivalents like vgmstream) can handle this conversion.

---

## 10. Input Replacement (PAD to SDL2)

### 10.1 Current Input Architecture

The input system has source coverage in `input.c`, but the current tree still
retains active asm-backed wrappers there. It wraps the Dolphin SDK PAD
functions:

**SDK functions called:**
- `PADRead(PADStatus[4])` -- read all 4 controllers
- `PADReset(mask)` -- reset specified pads
- `PADControlMotor(chan, cmd)` -- rumble motor control (0=stop, 1=on, 2=brake)
- `PADRecalibrate(mask)` -- re-zero analog sticks
- `SIGetType(chan)` -- detect controller type (for motor capability)

**Game wrapper API (input.h):**
- `PADInput_Init(16)` -- allocate state for 16 pads
- `PADInput_ReadButtons(padIdx)` -- per-frame update with edge detection
- `PADInput_GetHeld/GetPressed/GetReleased(padIdx)` -- button queries
- `PADInput_GetStickX/GetStickY(padIdx)` -- analog stick with dead zone
- `PADInput_SetDeadzone(padIdx, 7)` -- configurable dead zone
- `PADInput_SetStickMode(padIdx, 1)` -- digital stick mode
- `PADInput_SetRumbleMode/ResetRumble/InitMotor` -- rumble management
- `PADInput_IsConnected(padIdx)` -- connection state

**State per controller (PADInputState, 0x1C bytes):**
- `buttonsHeld`, `buttonsPrev`, `buttonsPressed`, `buttonsReleased` (u16 each)
- `stickX`, `stickY`, `cStickX`, `cStickY` (s8 each)
- `triggerL`, `triggerR` (u8 each)
- `connected`, `stickMode`, `deadzone`, `rumbleMode`, `rumbleActive`, `rumbleTimer`, `motorInitialized`

### 10.2 GCN-to-SDL Button Mapping

| GCN Button | Mask | SDL GameController | Notes |
|---|---|---|---|
| `PAD_BUTTON_A` | 0x0100 | `SDL_CONTROLLER_BUTTON_A` | Primary action |
| `PAD_BUTTON_B` | 0x0200 | `SDL_CONTROLLER_BUTTON_B` | Cancel/back |
| `PAD_BUTTON_X` | 0x0400 | `SDL_CONTROLLER_BUTTON_X` | |
| `PAD_BUTTON_Y` | 0x0800 | `SDL_CONTROLLER_BUTTON_Y` | |
| `PAD_BUTTON_START` | 0x1000 | `SDL_CONTROLLER_BUTTON_START` | |
| `PAD_TRIGGER_Z` | 0x0010 | `SDL_CONTROLLER_BUTTON_RIGHTSHOULDER` | Z trigger |
| `PAD_TRIGGER_L` | 0x0040 | `SDL_CONTROLLER_AXIS_TRIGGERLEFT` | Analog trigger |
| `PAD_TRIGGER_R` | 0x0020 | `SDL_CONTROLLER_AXIS_TRIGGERRIGHT` | Analog trigger |
| `PAD_BUTTON_UP` | 0x0008 | `SDL_CONTROLLER_BUTTON_DPAD_UP` | |
| `PAD_BUTTON_DOWN` | 0x0004 | `SDL_CONTROLLER_BUTTON_DPAD_DOWN` | |
| `PAD_BUTTON_LEFT` | 0x0001 | `SDL_CONTROLLER_BUTTON_DPAD_LEFT` | |
| `PAD_BUTTON_RIGHT` | 0x0002 | `SDL_CONTROLLER_BUTTON_DPAD_RIGHT` | |
| Main stick | | `SDL_CONTROLLER_AXIS_LEFTX/Y` | Scale -32768..32767 to -128..127 |
| C-stick | | `SDL_CONTROLLER_AXIS_RIGHTX/Y` | Scale similarly |

### 10.3 SDL2 GameController Translation Plan

```
src/pcport/
    pad2sdl.c    -- PAD function shims -> SDL2 GameController
    pad2sdl.h    -- Function declarations matching PAD SDK prototypes
```

Replace the 5 SDK functions:

```c
void PADRead(PADStatus* status) {
    SDL_GameControllerUpdate();
    for (int i = 0; i < 4; i++) {
        SDL_GameController* gc = g_controllers[i];
        if (gc == NULL || !SDL_GameControllerGetAttached(gc)) {
            status[i].err = -1;
            continue;
        }
        status[i].err = 0;
        status[i].button = mapButtons(gc);
        status[i].stickX = (s8)(SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX) >> 8);
        status[i].stickY = (s8)(SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY) >> 8);
        status[i].substickX = (s8)(SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_RIGHTX) >> 8);
        status[i].substickY = (s8)(SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_RIGHTY) >> 8);
        status[i].triggerLeft = (u8)(SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERLEFT) >> 7);
        status[i].triggerRight = (u8)(SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) >> 7);
    }
}

void PADControlMotor(u32 chan, u32 cmd) {
    SDL_GameController* gc = g_controllers[chan];
    if (gc == NULL) return;
    switch (cmd) {
        case 0: SDL_GameControllerRumble(gc, 0, 0, 0); break;           // stop
        case 1: SDL_GameControllerRumble(gc, 0xFFFF, 0xFFFF, 100); break; // on
        case 2: SDL_GameControllerRumble(gc, 0x4000, 0x4000, 50); break;  // brake
    }
}

u32 PADRecalibrate(u32 mask) {
    // No hardware recalibration needed on PC -- SDL handles this
    return 0;
}

u32 PADReset(u32 mask) {
    // Re-open controllers if needed
    return 0;
}

u32 SIGetType(s32 chan) {
    // Return standard controller type with motor bit
    if (g_controllers[chan] != NULL) {
        return 0x29000000; // standard GCN controller with motor
    }
    return 0;
}
```

### 10.4 Keyboard Fallback

Add an optional keyboard mapping for players without a gamepad:

| Key | GCN Button |
|---|---|
| Arrow keys | D-Pad |
| Z | A button |
| X | B button |
| A | X button |
| S | Y button |
| Enter | Start |
| Space | Z trigger |
| Q / E | L / R triggers |
| WASD | Main stick |
| IJKL | C-stick |

This is implemented as an alternative input source that feeds into the same
`PADStatus` structure, so the game's edge detection and dead zone logic works
unchanged.

---

## Appendix A: Phased Implementation Plan

### Phase 3a: Window and Input (1-2 weeks)
- GLFW window creation, GL 3.3 context
- PAD->SDL2 shim (pad2sdl.c)
- Keyboard fallback
- Clear screen to solid color, verify main loop runs

### Phase 3b: Geometry Pipeline (2-3 weeks)
- GX vertex format -> VAO/VBO conversion
- Display list parser
- Basic vertex + fragment shader (single texture, no TEV)
- Render first meshes (probably garbled colors/textures)

### Phase 3c: TEV Shader Generator (2-3 weeks)
- TEV state -> GLSL fragment shader compiler
- Shader cache
- Material color uniforms
- Render with correct materials

### Phase 3d: Texture Decode (1-2 weeks)
- GCN texture format decoders (de-tile + format convert)
- CMPR/DXT1 block swap
- CI palette lookup
- Mipmap upload

### Phase 3e: Lighting and Effects (1-2 weeks)
- Per-vertex lighting in vertex shader
- Fog in fragment shader
- Alpha test discard
- Blend mode state tracking

### Phase 3f: Framebuffer and Post-Processing (1 week)
- FBO for render-to-texture
- EFB copy emulation
- Effect system integration (blur, distortion, aura)

### Phase 3g: Audio (1-2 weeks)
- JAudio2->SDL2_mixer shim (snd2sdl.c)
- ADPCM decoder
- BGM streaming
- 3D audio panning

### Phase 3h: Polish (ongoing)
- Widescreen support (16:9 projection matrix override)
- Resolution scaling
- VSync / frame pacing
- Save file format conversion (GCN memory card -> PC file I/O)
