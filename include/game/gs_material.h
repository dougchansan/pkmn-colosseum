/**
 * @file gs_material.h
 * @brief GSmaterial -- Genius Sonority material/shader management system.
 *
 * GSmaterial is the bridge between the GS engine and HSD's MObj (material
 * object) rendering layer.  It manages a pool of fixed-size material entries
 * (0x170 bytes each) that hold diffuse/ambient/specular colors, texture
 * bindings, TEV stage configuration, environment-map state, and a matrix
 * transform block.
 *
 * Each material entry wraps an HSD_MObj and provides engine-level control
 * over:
 *   - Color animation via GXColor sub-structures at offsets 0x18/0x24/0x30
 *   - Per-material alpha transparency
 *   - Environment mapping (reflection / sphere map)
 *   - Pixel Engine (PE) descriptor overrides for custom blending
 *   - Shadow flag management
 *   - Texture binding through GStexture handles
 *   - Per-material transform matrix (offset 0x4C)
 *   - Callback hooks (offset 0x168/0x16C) for pre-render customization
 *
 * The pool is allocated from GSmem at init time.  Materials are referenced
 * by pointer into the pool; callers iterate the pool by index or search by
 * HSD_MObj pointer.
 *
 * Debug strings:
 *   "GSmaterialCreate: Run out of materials. Increase materialcount at
 *    initialisation"
 *   "GSmaterialSetPEdescr: Warning: already using a custom description!"
 *   "GSmaterial MObj"
 *   "GSmaterial: Unsupported texture format for environment map!"
 *   "GSmaterial: Error creating environment map: no texture defined!"
 *
 * Address range: 0x800E3604 - 0x800EE2C8 (~44KB, ~160 functions)
 *
 * Neighboring modules:
 *   Before: GSmem      (0x800E202C - 0x800E3604)
 *   After:  GStexture  (0x800EF098 - 0x800F07A8)
 *
 * Global state (sbss/sdata via SDA):
 *   lbl_8047AB74  GSmaterial* -- pool base pointer
 *   lbl_8047AB78  u32         -- pool capacity (max material count)
 *   lbl_8047AB80  u32         -- active callback state
 *   lbl_8047AB84  u32         -- callback parameter
 *   lbl_8047AB88  f32         -- squared distance threshold
 *   lbl_8047AB18  u16         -- GSmem handle for material pool
 *   lbl_8047AB1C  void*       -- material pool pointer (from GSmemGetPtr)
 *   lbl_8047AB20  u32         -- material pool max count
 *
 * HSD integration (lbl_80315490):
 *   The GSmaterial class descriptor is registered with HSD's object system
 *   at lbl_80315490.  _GSmaterialObjInit installs the class info, render callback
 *   (_matGSmatObjLoad), and setup callback (fn_800DF930) into this descriptor.
 */
#ifndef GS_MATERIAL_H
#define GS_MATERIAL_H

#include "dolphin/types.h"

/* ===================================================================
 * Material entry flag bits (stored at offset 0x00 of each entry)
 *
 * Recovered from bit-manipulation instructions across the accessor
 * functions in the 0x800E3604-0x800EE2C8 range.
 * =================================================================== */

/** Bit 0 (mask 0x00000001): Entry is active / in use */
#define GSMAT_FLAG_ACTIVE          0x00000001

/** Bit 1 (mask 0x00000002): Entry is fully valid (MObj initialized) */
#define GSMAT_FLAG_VALID           0x00000002

/** Bit 4 (mask 0x00000010): Enable vertex lighting / specular */
#define GSMAT_FLAG_SPECULAR        0x00000010

/** Bit 5 (mask 0x00000020): Render with transparency / alpha blend */
#define GSMAT_FLAG_BLEND           0x00000020

/** Bit 7 (mask 0x00000080): Render type selector (chooses between two MObj ptrs) */
#define GSMAT_FLAG_RENDERTYPE      0x00000080

/** Bit 8 (mask 0x00000100): Alpha test enable */
#define GSMAT_FLAG_ALPHATEST       0x00000100

/** Bit 9 (mask 0x00000200): Environment mapping enabled */
#define GSMAT_FLAG_ENVMAP          0x00000200

/** Bit 10 (mask 0x00000400): Shadow receiver flag */
#define GSMAT_FLAG_SHADOW          0x00000400

/** Bit 12 (mask 0x00001000): Force depth-test write */
#define GSMAT_FLAG_ZWRITE          0x00001000

/** Bit 13 (mask 0x00002000): Two-sided / no backface culling */
#define GSMAT_FLAG_TWOSIDED        0x00002000

/** Bit 20 (mask 0x00100000): Custom PE descriptor in use */
#define GSMAT_FLAG_CUSTOM_PE       0x00100000

/** Bit 22 (extrwi at bit 22): Texture stage index flag */
#define GSMAT_FLAG_TEXSTAGE        0x00200000

/** Bit 27 (mask 0x00000010 in hi-byte): Shadow-casting flag (checked via rlwinm) */
#define GSMAT_FLAG_SHADOW_CAST     0x08000000

/** Bits for fn_800E3604 bitmask checks */
#define GSMAT_MASK_ACTIVE_VALID    (GSMAT_FLAG_ACTIVE | GSMAT_FLAG_VALID)
#define GSMAT_MASK_ENVMAP_SHADOW   0x00400400  /* lis 0x40 + addi 0x400 */

/* ===================================================================
 * PE descriptor sentinel value
 *
 * The material system uses 0xFEFDFEFE as a "no descriptor" sentinel.
 * GSmaterialResetPEdescr checks addis r0, r6, 0x102 == 0xFEFE to detect this.
 * =================================================================== */
#define GSMAT_PE_NONE              0xFEFDFEFE

/* ===================================================================
 * GSmaterialEntry -- one entry in the material pool.
 *
 * Size: 0x170 bytes (368 bytes).
 * Pool stored at lbl_8047AB74, count at lbl_8047AB78.
 *
 * Field offsets recovered from the accessor functions in the range:
 *   GSmaterialSetShadowFlag  -- set/clear GSMAT_FLAG_SHADOW (bit 10) at +0x00
 *   GSmaterialFindByMObj  -- search pool by MObj pointer, stride 0x170
 *   GSmaterialGetUserData  -- return +0x148 (userdata)
 *   GSmaterialGetTexture  -- return +0x144 (texture)
 *   GSmaterialSetUserData  -- store r4 into +0x148
 *   GSmaterialSetTexture  -- set +0x144 (texture) and call env-map update
 *   GSmaterialGetTransformPtr  -- return +0x4C (transform matrix)
 *   GSmaterialSetCustomPEFlag  -- set/clear bit 20 (oris 0x10) at +0x00
 *   GSmaterialGetTexStage  -- extract bit 22 from +0x00
 *   GSmaterialSetEnvMapFlag  -- set/clear GSMAT_FLAG_ENVMAP (bit 9) at +0x00
 *   GSmaterialGetSpecularPtr  -- return +0x30 (specular color)
 *   GSmaterialGetAmbientPtr  -- return +0x24 (ambient color)
 *   GSmaterialGetDiffusePtr  -- return +0x18 (diffuse color)
 *   GSmaterialIsActive  -- extract bit 0 (active) from +0x00
 *   GSmaterialLerpPEColor  -- pass +0x3C to color-lerp fn (PE descriptor ptr)
 *   GSmaterialLerpSpecular  -- pass +0x30 to color-lerp fn (specular)
 *   GSmaterialLerpAmbient  -- pass +0x24 to color-lerp fn (ambient)
 *   GSmaterialLerpDiffuse  -- pass +0x18 to color-lerp fn (diffuse)
 *   fn_800E3DC4  -- large function accessing +0x08, +0x24, +0x114, +0x98
 * =================================================================== */
typedef struct GSmaterialEntry {
    /* 0x000 */ u32    flags;           /* GSMAT_FLAG_* bitmask */
    /* 0x004 */ u32    pad04;           /* unknown / reserved */
    /* 0x008 */ void*  mobjPrimary;     /* HSD_MObj* -- primary material (renderType=0) */
    /* 0x00C */ void*  mobjSecondary;   /* HSD_MObj* -- alternate material (renderType=1) */
    /* 0x010 */ void*  tevDesc;         /* TEV descriptor pointer (from HSD MObj) */
    /* 0x014 */ u32    lightMask;       /* GX light channel mask */

    /* Diffuse color (GXColor sub-structure, 12 bytes) */
    /* 0x018 */ u8     diffuse[12];     /* {r,g,b,a} + animation state */

    /* Ambient color */
    /* 0x024 */ u8     ambient[12];     /* {r,g,b,a} + animation state */

    /* Specular color */
    /* 0x030 */ u8     specular[12];    /* {r,g,b,a} + animation state */

    /* Custom PE (Pixel Engine) descriptor */
    /* 0x03C */ u32    peDescriptor;    /* PE blend mode or GSMAT_PE_NONE sentinel */
    /* 0x040 */ u32    peParam1;        /* PE parameter 1 */
    /* 0x044 */ u32    peParam2;        /* PE parameter 2 */
    /* 0x048 */ u32    peParam3;        /* PE parameter 3 */

    /* Per-material transform matrix (3x4 = 12 floats = 48 bytes) */
    /* 0x04C */ f32    matrix[12];      /* material UV / texgen transform */

    /* 0x07C-0x094 */ u8 pad7C[0x1C];  /* additional transform / texgen state */

    /* Alpha / transparency */
    /* 0x098 */ f32    alpha;           /* material alpha (0.0 = transparent, 1.0 = opaque) */

    /* 0x09C-0x113 */ u8 pad9C[0x78];  /* TEV stage config, color registers, etc. */

    /* State tracking */
    /* 0x114 */ u32    updateState;     /* non-zero: skip certain re-initialization */

    /* 0x118-0x143 */ u8 pad118[0x2C]; /* reserved / HSD integration data */

    /* Texture binding */
    /* 0x144 */ void*  texture;         /* GStextureHandle* or HSD_TObj* */

    /* User data */
    /* 0x148 */ void*  userData;        /* arbitrary caller-supplied pointer */

    /* 0x14C-0x167 */ u8 pad14C[0x1C]; /* reserved (includes env-map params) */

    /* Pre-render callbacks */
    /* 0x168 */ void*  callbackA;       /* pre-render callback pointer A */
    /* 0x16C */ void*  callbackB;       /* pre-render callback pointer B */
} GSmaterialEntry;

/* ===================================================================
 * Public API -- Material Pool Management
 * =================================================================== */

/**
 * GSmaterialInit -- Initialise the material pool.
 *
 * @param maxMaterials  Maximum number of material entries to allocate.
 *
 * Allocates maxMaterials * 0x40 bytes from GSmem (via GSmemAllocRaw),
 * stores the pool handle at lbl_8047AB18, resolves the pointer to
 * lbl_8047AB1C, and zeros each entry's first byte (marking inactive).
 * Also registers the GSmaterial class descriptor with HSD at lbl_80315490.
 *
 * Corresponds to GSmaterialInit.
 * (Note: init function is in gs_render.c; pool is consumed by this module.)
 */

/**
 * GSmaterialClassInit -- Register the GSmaterial MObj class with HSD.
 *
 * Sets up the HSD class descriptor at lbl_80315490 with:
 *   - Name string "GSmaterial MObj"
 *   - Class descriptor from lbl_8036CB30
 *   - Entry size 0x54 with alignment 0x24
 *   - Render callback _matGSmatObjLoad
 *   - Setup callback fn_800DF930
 *
 * Corresponds to _GSmaterialObjInit.
 */

/**
 * GSmaterialCreate -- Allocate and initialise a new material entry.
 *
 * Searches the material pool for an unused slot (first byte == 0),
 * initialises the flags, PE descriptor sentinel (0xFEFDFEFE),
 * default render state, and returns a pointer to the entry.
 *
 * If no free slot is available, prints:
 *   "GSmaterialCreate: Run out of materials. Increase materialcount at
 *    initialisation"
 * and returns NULL.
 *
 * Corresponds to fn_800DF7A4.
 */

/* ===================================================================
 * Public API -- Material Setup (0x800E3604-0x800E3DC4)
 * =================================================================== */

/**
 * GSmaterialApplyAll -- Iterate the material pool and apply rendering state.
 *
 * For each active+valid entry matching a given texture stage, acquires the
 * HSD render context, applies animation/color overrides, enables shadows
 * and environment maps as needed, then releases the render context.
 *
 * @param flags   Bitfield controlling which overrides to apply:
 *                  bit 5  = apply animation
 *                  bit 13 = apply environment map
 *                  bit 14 = apply shadow
 * @param stage   Texture stage index (matched against bit 22 of each entry's flags)
 *
 * Iterates the pool at stride 0x170 using lbl_8047AB74 and lbl_8047AB78.
 *
 * Corresponds to fn_800E3604. Size: 0x15C.
 */

/**
 * GSmaterialApplySingle -- Apply rendering state for a single material entry.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @param flags   Override bitfield (same as GSmaterialApplyAll).
 *
 * Corresponds to GSmodelDrawModel. Size: 0x124.
 */

/**
 * GSmaterialLerpColors -- Interpolate material colors over time.
 *
 * Performs float-based interpolation on the diffuse/ambient/specular
 * color sub-structures using the paired-singles unit.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @param stage   Texture stage / blend factor.
 *
 * Corresponds to GSmodelAdvanceAll. Size: 0xA4.
 */

/**
 * GSmodelUpdateAll -- Refresh active model state in three passes.
 *
 * Updates bound visibility, applies animation to visible or
 * particle-linked models, then refreshes attachments and bound centers.
 * The caller-supplied argument is unused.
 *
 * Corresponds to GSmodelApplyAll. Size: 0x1E0.
 */

/**
 * GSmaterialGetPoolCount -- Return the material pool capacity.
 *
 * @return  Value of lbl_8047AB78 (max materials).
 *
 * Corresponds to GSmaterialGetPoolCount. Size: 0x8.
 */
u32 GSmaterialGetPoolCount(void);

/**
 * GSmaterialSetShadowFlag -- Set or clear the shadow flag on a material.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @param enable  Non-zero to set GSMAT_FLAG_SHADOW, zero to clear it.
 *
 * Manipulates bit 10 (0x400) of entry->flags.
 *
 * Corresponds to GSmaterialSetShadowFlag. Size: 0x28.
 */
void GSmaterialSetShadowFlag(GSmaterialEntry* entry, u8 enable);

/**
 * GSmaterialFindByMObj -- Search the pool for the entry matching an HSD_MObj.
 *
 * @param mobj    Pointer to an HSD_MObj.
 * @return        Pointer to the GSmaterialEntry, or NULL if not found.
 *
 * Iterates the pool at stride 0x170, checking either +0x08 or +0x0C
 * depending on the GSMAT_FLAG_RENDERTYPE bit.
 *
 * Corresponds to GSmaterialFindByMObj. Size: 0x54.
 */
GSmaterialEntry* GSmaterialFindByMObj(void* mobj);

/**
 * GSmaterialGetGXTexGenSrc -- Get the texture's GX texgen source.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 *
 * Loads entry->texture (+0x144), and if non-NULL calls gsParticleKill
 * with parameter 1.
 *
 * Corresponds to GSmodelDestroyLinkedParticles. Size: 0x30.
 */
void GSmaterialGetGXTexGenSrc(GSmaterialEntry* entry);

/**
 * GSmaterialGetUserData -- Return the user-data pointer from a material entry.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @return        entry->userData at offset 0x148.
 *
 * Corresponds to GSmaterialGetUserData. Size: 0x8.
 */
void* GSmaterialGetUserData(GSmaterialEntry* entry);

/**
 * GSmaterialGetTexture -- Return the texture pointer from a material entry.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @return        entry->texture at offset 0x144.
 *
 * Corresponds to GSmaterialGetTexture. Size: 0x8.
 */
void* GSmaterialGetTexture(GSmaterialEntry* entry);

/**
 * GSmaterialSetUserData -- Store a user-data pointer in a material entry.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @param data    Arbitrary pointer to store.
 *
 * Corresponds to GSmaterialSetUserData. Size: 0x8.
 */
void GSmaterialSetUserData(GSmaterialEntry* entry, void* data);

/**
 * GSmaterialSetTexture -- Bind a texture to a material entry.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @param tex     GStextureHandle* or HSD_TObj* to bind.
 *
 * If the texture pointer changes, also calls the environment-map
 * update function (GSmodelSetAnimFrame) with the material's alpha value,
 * and calls fn_800EC134 to refresh the GX state.
 *
 * Corresponds to GSmaterialSetTexture. Size: 0x54.
 */
void GSmaterialSetTexture(GSmaterialEntry* entry, void* tex);

/**
 * GSmaterialGetTransformPtr -- Return a pointer to the material's
 * transform matrix at offset 0x4C.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @return        Pointer to the 3x4 matrix (12 floats).
 *
 * Corresponds to GSmaterialGetTransformPtr. Size: 0x8.
 */
f32* GSmaterialGetTransformPtr(GSmaterialEntry* entry);

/**
 * GSmaterialHasTransform -- Check if the material's transform matrix
 * is the identity (returns 0) or has been modified (returns 1).
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @return        1 if the transform matrix is non-identity.
 *
 * Calls fn_80191118 (MTXIsIdentity) on the matrix at +0x4C.
 *
 * Corresponds to GSmaterialHasTransform. Size: 0x30.
 */
u32 GSmaterialHasTransform(GSmaterialEntry* entry);

/**
 * GSmaterialSetCustomPEFlag -- Set or clear the custom PE descriptor flag.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @param enable  Non-zero to set (oris 0x10 = bit 20), zero to clear.
 *
 * Corresponds to GSmaterialSetCustomPEFlag. Size: 0x28.
 */
void GSmaterialSetCustomPEFlag(GSmaterialEntry* entry, u8 enable);

/**
 * GSmaterialGetTexStage -- Extract the texture stage index from the flags.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @return        Extracted single bit from position 22 of flags.
 *
 * Corresponds to GSmaterialGetTexStage. Size: 0xC.
 */
u32 GSmaterialGetTexStage(GSmaterialEntry* entry);

/**
 * GSmaterialSetEnvMapFlag -- Set or clear the environment-map flag.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @param enable  Non-zero to set GSMAT_FLAG_ENVMAP (0x200), zero to clear.
 *
 * Corresponds to GSmaterialSetEnvMapFlag. Size: 0x28.
 */
void GSmaterialSetEnvMapFlag(GSmaterialEntry* entry, u8 enable);

/**
 * GSmaterialGetSpecularPtr -- Return pointer to the specular color block.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @return        Pointer to the 12-byte specular color block at +0x30.
 *
 * Corresponds to GSmaterialGetSpecularPtr. Size: 0x8.
 */
void* GSmaterialGetSpecularPtr(GSmaterialEntry* entry);

/**
 * GSmaterialGetAmbientPtr -- Return pointer to the ambient color block.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @return        Pointer to the 12-byte ambient color block at +0x24.
 *
 * Corresponds to GSmaterialGetAmbientPtr. Size: 0x8.
 */
void* GSmaterialGetAmbientPtr(GSmaterialEntry* entry);

/**
 * GSmaterialGetDiffusePtr -- Return pointer to the diffuse color block.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @return        Pointer to the 12-byte diffuse color block at +0x18.
 *
 * Corresponds to GSmaterialGetDiffusePtr. Size: 0x8.
 */
void* GSmaterialGetDiffusePtr(GSmaterialEntry* entry);

/**
 * GSmaterialIsActive -- Check if a material entry is active.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @return        1 if bit 0 (GSMAT_FLAG_ACTIVE) is set, 0 otherwise.
 *
 * Corresponds to GSmaterialIsActive. Size: 0xC.
 */
u32 GSmaterialIsActive(GSmaterialEntry* entry);

/* ===================================================================
 * Public API -- Color Interpolation (GSmaterialLerpPEColor - GSmaterialLerpDiffuse)
 *
 * These four functions pass material sub-structures and an interpolation
 * parameter to GSvecCopy (a GXColor lerp utility).
 * =================================================================== */

/** Lerp custom PE descriptor color. Corresponds to GSmaterialLerpPEColor. */
void GSmaterialLerpPEColor(GSmaterialEntry* entry, void* param);

/** Lerp specular color. Corresponds to GSmaterialLerpSpecular. */
void GSmaterialLerpSpecular(GSmaterialEntry* entry, void* param);

/** Lerp ambient color. Corresponds to GSmaterialLerpAmbient. */
void GSmaterialLerpAmbient(GSmaterialEntry* entry, void* param);

/** Lerp diffuse color. Corresponds to GSmaterialLerpDiffuse. */
void GSmaterialLerpDiffuse(GSmaterialEntry* entry, void* param);

/* ===================================================================
 * Public API -- Advanced Material Config (0x800E3DC4 - 0x800E8EFC)
 *
 * This region contains the bulk of the material system: alpha and
 * color update, HSD MObj setup, TEV stage configuration, and the
 * environment-map pipeline.
 * =================================================================== */

/**
 * GSmaterialUpdateAlpha -- Update material alpha and HSD MObj transparency.
 *
 * Reads the alpha float at +0x98, applies it to the HSD MObj's
 * material color. If a texture is bound and the format requires it,
 * also sets up the appropriate GX alpha compare mode.
 *
 * This is a large function (0x250 bytes) that handles all the
 * GXColor interpolation and HSD state synchronization.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @param delta   Float delta / interpolation factor.
 *
 * Corresponds to fn_800E3DC4. Size: 0x250.
 */

/**
 * GSmaterialSetupTEVStages -- Configure TEV (Texture Environment) stages.
 *
 * This extremely large function (0x12BC bytes) sets up the complete
 * TEV pipeline for a material, configuring:
 *   - TEV stage count and ordering
 *   - Color/alpha combiners per stage
 *   - Texture coordinate generation
 *   - Indirect texture setup for environment maps
 *   - Kolor (constant color) registers
 *   - Swap mode table
 *
 * This is the core of the material -> GX hardware translation and is
 * critical for the PC port shader pipeline.
 *
 * Corresponds to GSmodelAddNull. Size: 0x12BC.
 */

/**
 * GSmaterialSetupLighting -- Configure GX lighting for a material.
 *
 * Another large function (0x878 bytes) that sets up the light
 * channels, attenuation, and material color sources for the GX
 * hardware lighting pipeline.
 *
 * Corresponds to modelShadowRender__FP10GSgfxLayer. Size: 0x878.
 */

/* ===================================================================
 * Public API -- Render State Helpers (0x800E8EFC - 0x800E93B8)
 * =================================================================== */

/**
 * GSmaterialResetRenderState -- Reset render state after material draw.
 *
 * Cleans up GX state (blend mode, z-mode, alpha compare) after
 * rendering with a custom material configuration.
 *
 * Corresponds to GSmodelFreeAllShadowTextures. Size: 0x6C.
 */

/**
 * GSmodelSetShadowBoundExpansion -- Store shadow-bound extent state.
 *
 * Call sites pass numeric extent/state pairs, including (0, 0) and (11, 7).
 *
 * Stores into lbl_8047AB84 and lbl_8047AB80 respectively.
 *
 * Corresponds to GSmodelSetShadowBoundExpansion. Size: 0xC.
 */
void GSmodelSetShadowBoundExpansion(u32 extent, u32 state);

/**
 * GSmaterialSetDistanceThreshold -- Set the squared distance threshold.
 *
 * @param dist   Distance value (squared internally via fmuls f0, f1, f1).
 *
 * Stored at lbl_8047AB88 for LOD / distance-based material switching.
 *
 * Corresponds to 0x800E8F74. Size: 0xC.
 */
void GSmaterialSetDistanceThreshold(f32 dist);

/**
 * GSmaterialCheckRenderSlot -- Check if a material entry is bound in
 * a render slot (lbl_80401490 table with stride 0x58).
 *
 * Used to determine if a material is currently active in the render
 * pipeline before modifying it.
 *
 * @param entry     Pointer to a GSmaterialEntry.
 * @param lockFlag  If non-zero, locks the slot.
 * @return          1 if the entry is in a render slot.
 *
 * Corresponds to modelShadowPrepare__FP8_GSmodelb. Size: 0x140.
 */

/* ===================================================================
 * Public API -- Environment Map System (0x800E93B8 - 0x800EA60C)
 *
 * These functions implement the reflection / environment map pipeline
 * for materials, coordinating between GStexture and HSD's TObj system.
 * =================================================================== */

/**
 * GSmaterialSetupEnvMap -- Full environment map setup.
 *
 * Configures the environment mapping pipeline for a material:
 *   1. Validates the material has a texture bound
 *   2. Checks the texture format is supported for env-mapping
 *   3. Creates the environment map texture (sphere/cube map)
 *   4. Sets up the GX indirect texture stages
 *   5. Configures the TEV stages for the reflection pass
 *
 * Prints error strings on failure:
 *   "GSmaterial: Unsupported texture format for environment map!"
 *   "GSmaterial: Error creating environment map: no texture defined!"
 *
 * Corresponds to _modelShadowAddAsNewReceiver__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound. Size: 0x5E0.
 */

/**
 * GSmaterialUpdateEnvMap -- Update environment map parameters.
 *
 * Called per-frame to update the environment map matrix based on the
 * current camera / view transform.
 *
 * Corresponds to _modelShadowFindValidReceiveModel__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound. Size: 0x194.
 */

/* ===================================================================
 * Public API -- Texture & TEV Binding (0x800EA60C - 0x800EC0E8)
 *
 * These functions handle texture loading into GX texture map slots,
 * TEV stage color/alpha combiner configuration, and the GX pipeline
 * state for drawing with materials.
 * =================================================================== */

/**
 * GSmaterialBindTextureToStage -- Bind a texture to a specific GX texture
 * map slot.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @param stage   GX texture map index.
 *
 * Corresponds to _modelParseSetupInstanceMtx__FP5GSmtxP9_HSD_JObjP5GSmtx. Size: 0x110.
 */

/**
 * GSmaterialConfigureTEVStage -- Set up a single TEV stage's color and
 * alpha combiners.
 *
 * @param tevStage   TEV stage index.
 * @param desc       TEV stage descriptor.
 *
 * Corresponds to _modelParseJObjDispSub__FP9_HSD_JObjP5GSmtxP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv. Size: 0x370.
 */

/**
 * GSmaterialConfigureBlend -- Set up the GX blend mode for a material.
 *
 * Configures source/dest blend factors, logic operation, and compare mode
 * based on the material's PE descriptor.
 *
 * Corresponds to _modelParseDObjDisp__FP9_HSD_DObjP5GSmtxP5GSmtxbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv. Size: 0x314.
 */

/**
 * GSmaterialConfigureZMode -- Configure Z-buffer test and write for a material.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 *
 * Corresponds to _modelParseLoadEnvelopeMatrix__FP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtx. Size: 0x284.
 */

/**
 * GSmaterialConfigureAlpha -- Set up alpha compare and alpha-to-coverage
 * for a material.
 *
 * Corresponds to fn_800EB268. Size: 0xD8.
 */

/* ===================================================================
 * Public API -- Material Property Accessors (0x800EC0E8 - 0x800EC5FC)
 * =================================================================== */

/**
 * GSmaterialSetAlpha -- Set the material's alpha value.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @param alpha   Float alpha value (0.0 = fully transparent, 1.0 = opaque).
 *
 * Corresponds to GSmodelForceAnimTransformUpdate. Size: 0x4C.
 */

/**
 * GSmaterialUpdateMObjColor -- Push the current diffuse/ambient/specular
 * colors from the material entry into the HSD MObj.
 *
 * Corresponds to fn_800EC134. Size: 0x20.
 */

/**
 * GSmaterialGetMObjPtr -- Return a pointer to the active HSD MObj.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @return        HSD_MObj* (either primary or secondary based on render type).
 *
 * Corresponds to GSmodelSetAnimEndedCallback. Size: 0xC.
 */

/**
 * GSmaterialSetDiffuseRGBA -- Set the diffuse color directly.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @param r,g,b,a Color components (0-255).
 *
 * Corresponds to GSmodelSet60fpsAnimFlag. Size: 0x28.
 */

/**
 * GSmaterialSetAmbientRGBA -- Set the ambient color directly.
 *
 * Corresponds to GSmodelLinkTexAnimToAnim. Size: 0x28.
 */

/**
 * GSmaterialGetDiffuseR -- Return the red component of diffuse color.
 *
 * Corresponds to GSmodelCanTexAnimate. Size: 0xC.
 */

/**
 * GSmaterialGetDiffuseG -- Return the green component of diffuse color.
 *
 * Corresponds to GSmodelCanAnimate. Size: 0xC.
 */

/**
 * GSmaterialGetDiffuseB -- Return the blue component of diffuse color.
 *
 * Corresponds to GSmodelHasTexAnimationEnded. Size: 0xC.
 */

/**
 * GSmaterialGetDiffuseA -- Return the alpha component of diffuse color.
 *
 * Corresponds to GSmodelStopTexAnimation. Size: 0x10.
 */

/* ===================================================================
 * Public API -- Render Pipeline Integration (0x800EC5FC - 0x800EE2C8)
 *
 * These functions drive the actual GX command emission for materials,
 * including the full draw pipeline, shadow pass, and post-processing.
 * =================================================================== */

/**
 * GSmaterialRenderSetup -- Full material render setup for a draw call.
 *
 * Configures the complete GX state for rendering with this material:
 *   - TEV stages
 *   - Texture binds
 *   - Lighting
 *   - Blend mode
 *   - Z mode
 *   - Alpha compare
 *
 * This is the primary entry point called by the model rendering system
 * when it needs to set up GX state for a material.
 *
 * Corresponds to GSmodelSetAnimBlend. Size: 0x2CC.
 */

/**
 * GSmaterialShadowSetup -- Configure the shadow-pass render state.
 *
 * Called during the shadow rendering pass to set up simplified TEV
 * stages for shadow map generation.
 *
 * Corresponds to GSmodelSetAnimIndex. Size: 0x428.
 */

/**
 * GSmaterialPostRender -- Restore GX state after material rendering.
 *
 * Corresponds to GSmodelAdvanceAnimation. Size: 0xFC.
 */

/**
 * GSmaterialFullPipeline -- Complete material rendering pipeline.
 *
 * The largest function in the tail of the module (0x5AC bytes).
 * Orchestrates the entire material rendering sequence: setup, draw,
 * shadow pass, environment map pass, and cleanup.
 *
 * Corresponds to _modelResetPartAnimMixes__FP8_GSmodel. Size: 0x5AC.
 */

/**
 * GSmaterialStoreResult -- Store a non-NULL result pointer.
 *
 * Simple helper: if r3 != NULL, stores r3 into the address given by r4.
 *
 * Corresponds to _modelGetAObjFunc__FP9_HSD_AObjPv. Size: 0x10.
 */

/**
 * GSmaterialSetRenderMode -- Configure a render mode via GX command.
 *
 * @param entry   Pointer to a GSmaterialEntry.
 * @param mode    If 0, calls HSD_AObjClearFlags; otherwise calls HSD_AObjSetFlags
 *                (renamed from fn_801C2A74/fn_801C2A90, naming pass 2026-07-07).
 *                Both use lis r4, 0x2000 as a flag parameter.
 *
 * Corresponds to _modelSetLoopFlag__FP9_HSD_AObjUl. Size: 0x38.
 */

/**
 * GSmaterialConfigureFog -- Set up fog parameters for a material.
 *
 * Corresponds to _modelGetEndFrame. Size: 0x5C.
 */

/**
 * GSmaterialConfigureScissor -- Set GX scissor for material rendering.
 *
 * Corresponds to GSmodelWalkJObjs. Size: 0x68.
 */

/**
 * GSmaterialApplyPEDescr -- Apply the PE (Pixel Engine) descriptor
 * settings to the GX pipeline.
 *
 * Corresponds to GSmodelGetPart. Size: 0xBC.
 */

/**
 * GSmaterialGetPEParam -- Return a PE parameter value.
 *
 * Corresponds to fn_800EE20C. Size: 0x20.
 */

/**
 * GSmaterialResetBlendMode -- Reset the GX blend mode to default.
 *
 * Corresponds to GSpartGetJObjIndex. Size: 0x5C.
 */

/**
 * GSmaterialFinalize -- Final cleanup for material state at end of frame.
 *
 * Corresponds to fn_800EE288. Size: 0x40.
 */

#endif /* GS_MATERIAL_H */
