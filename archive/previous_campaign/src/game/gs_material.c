/**
 * @file gs_material.c
 * @brief GSmaterial -- Material creation, TEV setup, texture binding.
 *
 * This module sits between GSmem and GStexture in the link order and
 * implements the bridge between the Genius Sonority engine and HSD's
 * MObj (material object) rendering layer.  It is the core rendering
 * state manager for the game.
 *
 * Decompiled from ~160 functions in range 0x800E3604 - 0x800EE2C8.
 *
 * Sub-modules identified within this range:
 *
 * 1. Material pool iteration / apply (0x800E3604 - 0x800E3B08)
 *    - fn_800E3604: GSmaterialApplyAll (0x15C bytes)
 *      Iterates the material pool at stride 0x170, checking flags for
 *      active+valid+texture-stage match. For each matching entry:
 *        - Acquires HSD render context (GScameraGetActiveCamera)
 *        - Calls fn_800E9148 (render slot check)
 *        - Selects MObj pointer from +0x08 or +0x0C based on render type
 *        - Applies animation (fn_801A13CC with params 0,1,0)
 *        - Applies env map (fn_801A13CC with params 0,4,0)
 *        - Applies shadow (fn_801A13CC with params 0,2,0)
 *        - Calls callback at +0x168/+0x16C via fn_800D6A5C
 *        - Releases render context (fn_800E9148 with param 0)
 *        - Calls fn_80195A48 (HSD render end)
 *      Then calls fn_800D87AC(-1) to reset internal render mode.
 *
 *    - fn_800E3760: GSmaterialApplySingle (0x124 bytes)
 *      Same as above but for a single entry pointer. Also checks bit 10
 *      (shadow) in the entry flags and calls fn_80190E60 with +0x4C offset
 *      (the transform matrix) if the shadow flag is set.
 *
 * 2. Color interpolation and update (0x800E3884 - 0x800E3B08)
 *    - fn_800E3884: GSmaterialLerpColors (0xA4 bytes)
 *      Float-based color interpolation using paired-singles.
 *    - fn_800E3928: GSmaterialUpdateColors (0x1E0 bytes)
 *      Full color/alpha update pipeline to HSD MObj.
 *    - fn_800E3B08: Helper (0x34 bytes) -- reads sdata2 float constants.
 *
 * 3. Accessor functions (0x800E3B3C - 0x800E3D08)
 *    Small getters/setters (mostly 0x08-0x54 bytes each):
 *    - GSmaterialGetPoolCount: GetPoolCount       -- lwz lbl_8047AB78
 *    - GSmaterialSetShadowFlag: SetShadowFlag      -- bit 10 (0x400) set/clear
 *    - GSmaterialFindByMObj: FindByMObj         -- pool search, stride 0x170
 *    - fn_800E3BC0: GetGXTexGenSrc
 *    - GSmaterialGetUserData: GetUserData        -- lwz +0x148
 *    - GSmaterialGetTexture: GetTexture         -- lwz +0x144
 *    - GSmaterialSetUserData: SetUserData        -- stw +0x148
 *    - GSmaterialSetTexture: SetTexture         -- stw +0x144, env-map update
 *    - GSmaterialGetTransformPtr: GetTransformPtr    -- addi +0x4C
 *    - GSmaterialHasTransform: HasTransform       -- MTXIsIdentity on +0x4C
 *    - GSmaterialSetCustomPEFlag: SetCustomPEFlag    -- bit 20 set/clear (oris 0x10)
 *    - GSmaterialGetTexStage: GetTexStage        -- extrwi bit 22
 *    - GSmaterialSetEnvMapFlag: SetEnvMapFlag      -- bit 9 (0x200) set/clear
 *    - GSmaterialGetSpecularPtr: GetSpecularPtr     -- addi +0x30
 *    - GSmaterialGetAmbientPtr: GetAmbientPtr      -- addi +0x24
 *    - GSmaterialGetDiffusePtr: GetDiffusePtr      -- addi +0x18
 *    - GSmaterialIsActive: IsActive           -- extrwi bit 0
 *
 * 4. Color lerp helpers (0x800E3D14 - 0x800E3D98)
 *    Four functions that pass sub-structure offsets to fn_800E01D0:
 *    - GSmaterialLerpPEColor: LerpPEColor    -- offset +0x3C
 *    - GSmaterialLerpSpecular: LerpSpecular   -- offset +0x30
 *    - GSmaterialLerpAmbient: LerpAmbient    -- offset +0x24
 *    - GSmaterialLerpDiffuse: LerpDiffuse    -- offset +0x18
 *
 * 5. Alpha/color update pipeline (0x800E3DC4 - 0x800E4598)
 *    - fn_800E3DC4: UpdateAlpha (0x250 bytes)
 *      Reads alpha at +0x98, applies to HSD MObj, handles texture alpha.
 *    - fn_800E4014: Helper (0x68 bytes) -- extracts MObj flags.
 *    - fn_800E407C: Helper (0xF4 bytes) -- alpha compare setup.
 *    - fn_800E4170: SetupAlphaBlend (0x234 bytes) -- GXSetBlendMode.
 *    - fn_800E43A4: SetupZMode (0x170 bytes) -- GXSetZMode.
 *    - fn_800E4514: Helper (0x84 bytes) -- Z compare function.
 *    - fn_800E4598: Helper (0xC4 bytes) -- blend factor selection.
 *
 * 6. TEV configuration core (0x800E465C - 0x800E8EFC)
 *    - fn_800E465C: TEVStageSetup (0x464 bytes)
 *      Configures individual TEV stage color/alpha inputs and ops.
 *    - fn_800E4AC0: TEVColorRegister (0x134 bytes)
 *      Sets GX TEV constant color registers.
 *    - fn_800E4BF4: TEVSwapMode (0xA4 bytes)
 *      Configures TEV swap mode table entries.
 *    - fn_800E4C98-fn_800E5188: TEV helpers (various sizes)
 *      Small functions for individual TEV parameters.
 *    - fn_800E51A4: TEVMultiStageSetup (0x3AC bytes)
 *      Configures multi-stage TEV for complex shading (bump, detail).
 *    - fn_800E5550-fn_800E60F0: Texture coordinate generation
 *      Functions for setting up GXTexCoordGen, GXSetTexCoordGen2.
 *    - fn_800E61BC-fn_800E6BC8: Material color / light channel config
 *      Functions that configure GXSetChanCtrl, GXSetChanMatColor.
 *    - fn_800E6DC0: Bridge (0xC bytes) -- simple forwarding function.
 *    - fn_800E6DCC: Large TEV descriptor builder (0x4C4 bytes)
 *      Builds a complete TEV descriptor from material properties.
 *    - fn_800E7290: TEV stage count calculator (0x9C bytes)
 *      Determines how many TEV stages are needed.
 *    - fn_800E732C: FULL TEV PIPELINE (0x12BC bytes)
 *      The largest function in the module. Configures the complete
 *      TEV pipeline: stage count, all combiners, texcoord gen, indirect
 *      texture, kolor registers, and swap mode. THIS IS THE CRITICAL
 *      FUNCTION for the PC port shader pipeline translation.
 *    - fn_800E85E8: TEV post-config (0x9C bytes)
 *      Final TEV state after the main pipeline setup.
 *    - fn_800E8684: LIGHTING SETUP (0x878 bytes)
 *      Second-largest function. Full GX lighting pipeline: light channels,
 *      attenuation, diffuse/specular material color sources.
 *    - fn_800E8EFC: RenderStateReset (0x6C bytes)
 *      Cleanup after material rendering.
 *
 * 7. Callback / distance / render-slot system (0x800E8F68 - 0x800E9358)
 *    - GSmaterialSetCallback: SetCallback (0xC bytes) -- stores lbl_8047AB84/80
 *    - GSmaterialSetDistanceThreshold: stores squared distance threshold
 *    - fn_800E8F80: Helper (0x20 bytes)
 *    - fn_800E8FA0: Helper (0x48 bytes)
 *    - fn_800E8FE8: Helper (0x24 bytes)
 *    - fn_800E900C: Distance check (0xBC bytes)
 *    - fn_800E90C8: Slot helper (0x40 bytes)
 *    - fn_800E9108: Slot helper (0x40 bytes)
 *    - fn_800E9148: CheckRenderSlot (0x140 bytes)
 *      Uses lbl_80401490 table with stride 0x58 to track active renders.
 *    - fn_800E9288: Slot release (0x50 bytes)
 *    - fn_800E92D8: Slot config (0x80 bytes)
 *    - fn_800E9358: Slot cleanup (0x60 bytes)
 *
 * 8. Environment map system (0x800E93B8 - 0x800EA60C)
 *    - fn_800E93B8: SetupEnvMap (0x5E0 bytes)
 *      Full env-map creation pipeline. Validates texture, checks format,
 *      creates reflection texture, configures indirect TEV stages.
 *    - fn_800E9998: UpdateEnvMap (0x194 bytes)
 *      Per-frame env-map matrix update from camera view.
 *    - fn_800E9B2C: EnvMap texcoord (0x140 bytes)
 *    - fn_800E9C6C: EnvMap TEV stage (0x1C8 bytes)
 *    - fn_800E9E34: EnvMap helper (0x5C bytes)
 *    - fn_800E9E90: EnvMap pipeline (0x77C bytes)
 *      Large environment map rendering pipeline.
 *
 * 9. GX state emission / render pipeline (0x800EA60C - 0x800EE2C8)
 *    - HSD_JObjMtxIsDirty: GX state query (0x58 bytes)
 *    - fn_800EA664: GX state helper (0x70 bytes)
 *    - fn_800EA6D4: BindTextureToStage (0x110 bytes)
 *    - fn_800EA7E4: Helper (0x3C bytes)
 *    - fn_800EA820: Texture load (0x140 bytes)
 *    - fn_800EA960: ConfigureTEVStage (0x370 bytes)
 *    - fn_800EACD0: ConfigureBlend (0x314 bytes)
 *    - fn_800EAFE4: ConfigureZMode (0x284 bytes)
 *    - fn_800EB268: ConfigureAlpha (0xD8 bytes)
 *    - fn_800EB340: ConfigureAlphaTest (0xD4 bytes)
 *    - fn_800EB414: Helper (0x50 bytes)
 *    - fn_800EB464: Helper (0xBC bytes)
 *    - fn_800EB520: Simple getter (0x8 bytes)
 *    - fn_800EB528: Helper (0x78 bytes)
 *    - fn_800EB5A0: GX command batch (0x140 bytes)
 *    - fn_800EB6E0: Texture setup (0x224 bytes)
 *    - fn_800EB904: Large render setup (0x5E8 bytes)
 *    - _modelSetRotateEulerToQuatAll__FP9_HSD_JObj: Render config (0x1FC bytes)
 *    - fn_800EC0E8: SetAlpha (0x4C bytes)
 *    - fn_800EC134: UpdateMObjColor (0x20 bytes)
 *    - fn_800EC154: GetMObjPtr (0xC bytes)
 *    - fn_800EC160: SetDiffuseRGBA (0x28 bytes)
 *    - fn_800EC188: SetAmbientRGBA (0x28 bytes)
 *    - fn_800EC1B0: GetDiffuseR (0xC bytes)
 *    - fn_800EC1BC: GetDiffuseG (0xC bytes)
 *    - fn_800EC1C8: GetDiffuseB (0xC bytes)
 *    - fn_800EC1D4: GetDiffuseA (0x10 bytes)
 *    - fn_800EC1E4: Color helper (0x24 bytes)
 *    - fn_800EC208: Color config (0x9C bytes)
 *    - fn_800EC2A4: Helper (0x64 bytes)
 *    - fn_800EC308: Helper (0x54 bytes)
 *    - fn_800EC35C: PE descriptor setup (0x174 bytes)
 *    - fn_800EC4D0: PE helper (0x6C bytes)
 *    - GSmodelGetAnimFrame: PE helper (0x2C bytes)
 *    - fn_800EC568: Simple getter (0x8 bytes)
 *    - fn_800EC570: Simple getter (0x8 bytes)
 *    - fn_800EC578: PE config (0x34 bytes)
 *    - fn_800EC5AC: PE helper (0xC bytes)
 *    - fn_800EC5B8: Pre-render setup (0x44 bytes)
 *    - fn_800EC5FC: RenderSetup (0x2CC bytes) -- primary entry point
 *    - fn_800EC8C8: Helper (0x14 bytes)
 *    - fn_800EC8DC: Helper (0x3C bytes)
 *    - fn_800EC918: Helper (0x3C bytes)
 *    - fn_800EC954: Simple (0xC bytes)
 *    - fn_800EC960: Simple (0xC bytes)
 *    - fn_800EC96C: Helper (0x24 bytes)
 *    - fn_800EC990: Helper (0x4C bytes)
 *    - fn_800EC9DC: Helper (0x9C bytes)
 *    - fn_800ECA78: EnvMap alpha apply (0xFC bytes)
 *    - fn_800ECB74: EnvMap texture update (0x134 bytes)
 *    - fn_800ECCA8: ShadowSetup (0x428 bytes) -- shadow pass pipeline
 *    - fn_800ED0D0: PostRender (0xFC bytes) -- state restore
 *    - fn_800ED1CC: Post-render chain (0x308 bytes)
 *    - fn_800ED4D4: Post-render chain (0x1D8 bytes)
 *    - fn_800ED6AC: Helper (0x38 bytes)
 *    - fn_800ED6E4: Helper (0x100 bytes)
 *    - fn_800ED7E4: Helper (0xE0 bytes)
 *    - fn_800ED8C4: Pipeline stage (0x1D4 bytes)
 *    - fn_800EDA98: FullPipeline (0x5AC bytes) -- complete draw sequence
 *    - fn_800EE044: StoreResult (0x10 bytes)
 *    - fn_800EE054: SetRenderMode (0x38 bytes)
 *    - _modelGetEndFrame: ConfigureFog (0x5C bytes)
 *    - fn_800EE0E8: ConfigureScissor (0x68 bytes)
 *    - fn_800EE150: ApplyPEDescr (0xBC bytes)
 *    - fn_800EE20C: GetPEParam (0x20 bytes)
 *    - fn_800EE22C: ResetBlendMode (0x5C bytes)
 *    - fn_800EE288: Finalize (0x40 bytes)
 *
 * Architecture notes for PC port:
 *   The TEV pipeline (fn_800E732C) is the most critical function to
 *   understand for shader translation. On GCN, TEV stages are configured
 *   per-draw-call through GX register writes. For a PC port, these must
 *   be translated into GLSL/HLSL shader programs.
 *
 *   Key mapping:
 *     GX TEV stage -> GLSL fragment shader combiner operation
 *     GX TexCoordGen -> GLSL vertex shader texture coordinate output
 *     GX IndirectTex -> GLSL bump/normal map sampling
 *     GX BlendMode -> OpenGL/Vulkan blend state
 *     GX ZMode -> OpenGL/Vulkan depth state
 *     GX AlphaCompare -> GLSL discard / alpha test
 *     GX LitChannel -> GLSL per-vertex lighting calculation
 *
 *   The environment map system (fn_800E93B8) uses GCN-specific
 *   indirect texture hardware, which must be emulated in the fragment
 *   shader for the PC port.
 *
 * Address range: 0x800E3604 - 0x800EE2C8 (~44KB, ~160 functions)
 */

#include "dolphin/types.h"
#include "game/gs_material.h"

/* ===== External SDK / engine functions ===== */
extern void  fn_800DD970(const char* fmt, ...);        /* OSReport / GSlog */
extern void* GScameraGetActiveCamera(void);                        /* HSD_StartRender (acquire context) */
extern void  fn_800D87AC(s32 mode);                    /* GSgfx_SetInternalMode */
extern void  GSlightSetupLights(void* renderObj);
extern void  fn_800D6A5C(void* callbackA, void* callbackB); /* callback dispatch */

/* GSmem */
extern u16   fn_800E3534(u32 size);                    /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);                  /* GSmemGetPtr */
extern void* fn_800E24B0(u16 handle);                  /* GSmemLock */
extern void  fn_800E209C(u16 handle);                  /* GSmemFree */

/* HSD object system */
extern void  hsdInitClassInfo(void* classDesc, void* parent, void* name,
                              u32 entrySize, u32 alignment, void* nameDup);
extern void  fn_801A13CC(void* obj, u32 a, u32 b, u32 c); /* HSD_MObjAnim */
extern s32   fn_80195A6C(void* renderCtx);             /* HSD render context check */
extern void  fn_80195A48(void);                        /* HSD render end */
extern void  fn_80190E60(void* mtx);                   /* MTX operation (shadow) */
extern s32   fn_80191118(void* mtx);                    /* MTXIsIdentity */
extern void  fn_80118874(void* tex, u32 param);        /* GX texgen source */
extern void  __assert(void* a, u32 line, void* b);  /* HSD assert */
extern void  fn_8019D620(void* mobj);                  /* HSD MObj color update */
extern void  fn_801A6FF0(void* obj);                   /* HSD object update */
extern void  fn_801A7CFC(void* classDesc);             /* HSD class register */
extern u32   HSD_MObjGetFlags(void* mobj);             /* HSD MObj flag query */
extern void  fn_801B25C4(u32 priority);                /* HSD render priority */
extern void  fn_801B6DC0(void* obj, u32 a, u32 b, u32 c, u32 d); /* HSD TEV config */
extern u32   fn_801B6F5C(void* desc, u32 a, u32 b, u32 c); /* HSD TEV stage */
extern void* fn_801B707C(void* stage);                 /* HSD TEV resolve */
extern void  fn_801B7C60(void* obj);                   /* HSD TEV type check */

/* GX functions (via SDK wrappers) */
extern void  fn_801C2A74(void* a, u32 flags);          /* GX render mode A */
extern void  fn_801C2A90(void* a, u32 flags);          /* GX render mode B */

/* Color interpolation */
extern void  fn_800E01D0(void* dst, void* src);        /* GXColor copy/lerp */
extern void  fn_800E019C(void* a, void* b, void* c);   /* GXColor multiply */

/* Internal material functions (used by SetTexture and others) */
extern void  fn_800ECA78(void* entry, f32 alpha);       /* env-map alpha apply */
extern void  fn_800EC134(void* entry);                  /* update MObj color */

/* ===== String constants (rodata) ===== */
extern const char lbl_80270528[]; /* "GSmaterialSetPEdescr: Warning: already using a custom description!\n" */
extern const char lbl_8027056C[]; /* "GSmaterialCreate: Run out of materials. Increase materialcount at initialisation" */
extern const char lbl_802705C0[]; /* "GSmaterial MObj" */
extern const char lbl_802705D0[]; /* "GSmaterial: Unsupported texture format for environment map!\n" */
extern const char lbl_80270610[]; /* "GSmaterial: Error creating environment map: no texture defined!\n" */
extern char lbl_80270E28[]; /* "!(jobj->flags & JOBJ_USE_QUATERNION)" */

/* ===== Assert file/line strings (sdata2) ===== */
extern u8 lbl_8047CB58[8];  /* sdata2: f64 constant */
extern u8 lbl_8047CB60[7];  /* sdata2: assert file string */
extern u8 lbl_8047CB68[8];  /* sdata2: assert tag string */

/* ===== BSS tables ===== */
extern u8 lbl_80401490[];  /* render slot table (stride 0x58) */

/* ===== HSD class descriptor ===== */
extern u8 lbl_80315490[];  /* GSmaterial class descriptor */
extern u8 lbl_8036CB30[];  /* GSmaterial parent class descriptor */

/* ===== Global state (sbss via SDA) ===== */
/* Material pool (used by GSmaterialApplyAll and accessor functions) */
/* lbl_8047AB74 : GSmaterialEntry* -- pool base pointer */
static GSmaterialEntry* gsMatPool;          /* @sda21 lbl_8047AB74 */
/* lbl_8047AB78 : u32 -- pool capacity (max count) */
static u32 gsMatPoolCount;                  /* @sda21 lbl_8047AB78 */
/* lbl_8047AB80 : u32 -- active callback state */
static u32 gsMatCallbackState;              /* @sda21 lbl_8047AB80 */
/* lbl_8047AB84 : u32 -- callback parameter */
static u32 gsMatCallbackParam;              /* @sda21 lbl_8047AB84 */
/* lbl_8047AB88 : f32 -- squared distance threshold */
static f32 gsMatDistThresholdSq;            /* @sda21 lbl_8047AB88 */

/* Material creation pool (separate from main pool) */
/* lbl_8047AB18 : u16 -- GSmem handle for material creation pool */
static u16  gsMatCreatePoolHandle;          /* @sda21 lbl_8047AB18 */
/* lbl_8047AB1C : void* -- creation pool pointer */
static void* gsMatCreatePool;               /* @sda21 lbl_8047AB1C */
/* lbl_8047AB20 : u32 -- creation pool max count */
static u32  gsMatCreatePoolCount;           /* @sda21 lbl_8047AB20 */

/* =======================================================================
 *  Forward declarations for internal callbacks
 * ======================================================================= */
extern void fn_800DF930(void* a, void* b, void* c, void* d, void* e);
extern void fn_800DFE98(void* a);

/* Forward declarations for converted functions */
void fn_800E9148(void*, u8);


/* =======================================================================
 *  GSmaterialGetPoolCount / GSmaterialGetPoolCount
 *  Address: 0x800E3B3C, Size: 0x8
 *
 *  Assembly:
 *    lwz r3, lbl_8047AB78@sda21(r0)
 *    blr
 * ======================================================================= */
u32 GSmaterialGetPoolCount(void) {
    return gsMatPoolCount;
}

/* =======================================================================
 *  GSmaterialSetShadowFlag / GSmaterialSetShadowFlag
 *  Address: 0x800E3B44, Size: 0x28
 *
 *  Assembly:
 *    clrlwi. r0, r4, 24         ; mask enable to byte
 *    beq clear
 *    lwz r0, 0x0(r3)           ; load flags
 *    ori r0, r0, 0x400          ; set bit 10
 *    stw r0, 0x0(r3)
 *    blr
 *  clear:
 *    lwz r0, 0x0(r3)
 *    rlwinm r0, r0, 0, 22, 20  ; clear bit 10
 *    stw r0, 0x0(r3)
 *    blr
 * ======================================================================= */
void GSmaterialSetShadowFlag(GSmaterialEntry* entry, u8 enable) {
    if (enable != 0) {
        entry->flags |= GSMAT_FLAG_SHADOW;
    } else {
        entry->flags &= ~GSMAT_FLAG_SHADOW;
    }
}

/* =======================================================================
 *  GSmaterialFindByMObj / GSmaterialFindByMObj
 *  Address: 0x800E3B6C, Size: 0x54
 *
 *  Assembly:
 *    lwz r0, lbl_8047AB78@sda21(r0)    ; pool count
 *    lwz r4, lbl_8047AB74@sda21(r0)    ; pool base
 *    mtctr r0
 *    cmplwi r0, 0
 *    ble notFound
 *  loop:
 *    lwz r5, 0x0(r4)                   ; load flags
 *    clrlwi. r0, r5, 31                ; test bit 0 (active)
 *    beq next
 *    rlwinm. r0, r5, 0, 24, 24         ; test bit 7 (renderType)
 *    beq usePrimary
 *    lwz r0, 0xC(r4)                   ; mobjSecondary
 *    b compare
 *  usePrimary:
 *    lwz r0, 0x8(r4)                   ; mobjPrimary
 *  compare:
 *    cmplw r0, r3                      ; compare with target MObj
 *    bne next
 *    mr r3, r4                          ; found it
 *    blr
 *  next:
 *    addi r4, r4, 0x170                ; stride to next entry
 *    bdnz loop
 *  notFound:
 *    li r3, 0
 *    blr
 * ======================================================================= */
GSmaterialEntry* GSmaterialFindByMObj(void* mobj) {
    u32 count = gsMatPoolCount;
    GSmaterialEntry* entry = gsMatPool;
    u32 i;

    for (i = 0; i < count; i++) {
        u32 flags = entry->flags;

        if (flags & GSMAT_FLAG_ACTIVE) {
            void* entryMObj;

            if (flags & GSMAT_FLAG_RENDERTYPE) {
                entryMObj = entry->mobjSecondary;
            } else {
                entryMObj = entry->mobjPrimary;
            }

            if (entryMObj == mobj) {
                return entry;
            }
        }

        entry = (GSmaterialEntry*)((u8*)entry + 0x170);
    }

    return NULL;
}

/* =======================================================================
 *  GSmaterialGetUserData / GSmaterialGetUserData
 *  Address: 0x800E3BF0, Size: 0x8
 *
 *  Assembly:
 *    lwz r3, 0x148(r3)
 *    blr
 * ======================================================================= */
void* GSmaterialGetUserData(GSmaterialEntry* entry) {
    return entry->userData;
}

/* =======================================================================
 *  GSmaterialGetTexture / GSmaterialGetTexture
 *  Address: 0x800E3BF8, Size: 0x8
 *
 *  Assembly:
 *    lwz r3, 0x144(r3)
 *    blr
 * ======================================================================= */
void* GSmaterialGetTexture(GSmaterialEntry* entry) {
    return entry->texture;
}

/* =======================================================================
 *  GSmaterialSetUserData / GSmaterialSetUserData
 *  Address: 0x800E3C00, Size: 0x8
 *
 *  Assembly:
 *    stw r4, 0x148(r3)
 *    blr
 * ======================================================================= */
void GSmaterialSetUserData(GSmaterialEntry* entry, void* data) {
    entry->userData = data;
}

/* =======================================================================
 *  GSmaterialSetTexture / GSmaterialSetTexture
 *  Address: 0x800E3C08, Size: 0x54
 *
 *  Assembly:
 *    lwz r0, 0x144(r3)         ; old texture
 *    cmplw r0, r4              ; same?
 *    beq done
 *    stw r4, 0x144(r31)        ; store new texture
 *    lwz r0, 0x144(r31)        ; reload
 *    cmplwi r0, 0              ; NULL?
 *    beq done
 *    lfs f1, 0x98(r31)         ; load alpha
 *    bl fn_800ECA78            ; env-map alpha apply
 *    mr r3, r31
 *    bl fn_800EC134            ; update MObj color
 *  done:
 * ======================================================================= */
void GSmaterialSetTexture(GSmaterialEntry* entry, void* tex) {
    if (entry->texture == tex) {
        return;
    }

    entry->texture = tex;

    if (entry->texture != NULL) {
        fn_800ECA78(entry, *(f32*)((u8*)entry + 0x98));  /* env-map alpha apply */
        fn_800EC134(entry);  /* update MObj color */
    }
}

/* =======================================================================
 *  GSmaterialGetTransformPtr / GSmaterialGetTransformPtr
 *  Address: 0x800E3C5C, Size: 0x8
 *
 *  Assembly:
 *    addi r3, r3, 0x4C
 *    blr
 * ======================================================================= */
f32* GSmaterialGetTransformPtr(GSmaterialEntry* entry) {
    return entry->matrix;
}

/* =======================================================================
 *  GSmaterialHasTransform / GSmaterialHasTransform
 *  Address: 0x800E3C64, Size: 0x30
 *
 *  Assembly:
 *    addi r3, r3, 0x4C         ; matrix pointer
 *    bl fn_80191118            ; MTXIsIdentity
 *    neg r0, r3
 *    or r0, r0, r3
 *    srwi r3, r0, 31           ; convert to boolean (0 or 1)
 *    blr
 * ======================================================================= */
u32 GSmaterialHasTransform(GSmaterialEntry* entry) {
    s32 result = (s32)fn_80191118(entry->matrix);
    /* Convert non-zero to 1 */
    return (u32)((-result | result) >> 31) & 1;
}

/* =======================================================================
 *  GSmaterialSetCustomPEFlag / GSmaterialSetCustomPEFlag
 *  Address: 0x800E3C94, Size: 0x28
 *
 *  Assembly:
 *    clrlwi. r0, r4, 24
 *    beq clear
 *    lwz r0, 0x0(r3)
 *    oris r0, r0, 0x10         ; set bit 20
 *    stw r0, 0x0(r3)
 *    blr
 *  clear:
 *    lwz r0, 0x0(r3)
 *    rlwinm r0, r0, 0, 12, 10  ; clear bit 20
 *    stw r0, 0x0(r3)
 *    blr
 * ======================================================================= */
void GSmaterialSetCustomPEFlag(GSmaterialEntry* entry, u8 enable) {
    if (enable != 0) {
        entry->flags |= GSMAT_FLAG_CUSTOM_PE;
    } else {
        entry->flags &= ~GSMAT_FLAG_CUSTOM_PE;
    }
}

/* =======================================================================
 *  GSmaterialGetTexStage / GSmaterialGetTexStage
 *  Address: 0x800E3CBC, Size: 0xC
 *
 *  Assembly:
 *    lwz r0, 0x0(r3)
 *    extrwi r3, r0, 1, 22      ; extract bit 22
 *    blr
 * ======================================================================= */
u32 GSmaterialGetTexStage(GSmaterialEntry* entry) {
    return (entry->flags >> 9) & 1;  /* bit 22 from MSB = bit 9 from LSB in extrwi encoding */
}

/* =======================================================================
 *  GSmaterialSetEnvMapFlag / GSmaterialSetEnvMapFlag
 *  Address: 0x800E3CC8, Size: 0x28
 *
 *  Assembly:
 *    clrlwi. r0, r4, 24
 *    beq clear
 *    lwz r0, 0x0(r3)
 *    ori r0, r0, 0x200          ; set bit 9
 *    stw r0, 0x0(r3)
 *    blr
 *  clear:
 *    lwz r0, 0x0(r3)
 *    rlwinm r0, r0, 0, 23, 21  ; clear bit 9
 *    stw r0, 0x0(r3)
 *    blr
 * ======================================================================= */
void GSmaterialSetEnvMapFlag(GSmaterialEntry* entry, u8 enable) {
    if (enable != 0) {
        entry->flags |= GSMAT_FLAG_ENVMAP;
    } else {
        entry->flags &= ~GSMAT_FLAG_ENVMAP;
    }
}

/* =======================================================================
 *  GSmaterialGetSpecularPtr / GSmaterialGetSpecularPtr
 *  Address: 0x800E3CF0, Size: 0x8
 *
 *  Assembly:
 *    addi r3, r3, 0x30
 *    blr
 * ======================================================================= */
void* GSmaterialGetSpecularPtr(GSmaterialEntry* entry) {
    return (void*)entry->specular;
}

/* =======================================================================
 *  GSmaterialGetAmbientPtr / GSmaterialGetAmbientPtr
 *  Address: 0x800E3CF8, Size: 0x8
 *
 *  Assembly:
 *    addi r3, r3, 0x24
 *    blr
 * ======================================================================= */
void* GSmaterialGetAmbientPtr(GSmaterialEntry* entry) {
    return (void*)entry->ambient;
}

/* =======================================================================
 *  GSmaterialGetDiffusePtr / GSmaterialGetDiffusePtr
 *  Address: 0x800E3D00, Size: 0x8
 *
 *  Assembly:
 *    addi r3, r3, 0x18
 *    blr
 * ======================================================================= */
void* GSmaterialGetDiffusePtr(GSmaterialEntry* entry) {
    return (void*)entry->diffuse;
}

/* =======================================================================
 *  GSmaterialIsActive / GSmaterialIsActive
 *  Address: 0x800E3D08, Size: 0xC
 *
 *  Assembly:
 *    lwz r0, 0x0(r3)
 *    extrwi r3, r0, 1, 30      ; extract bit 0 (LSB)
 *    blr
 * ======================================================================= */
u32 GSmaterialIsActive(GSmaterialEntry* entry) {
    return entry->flags & GSMAT_FLAG_ACTIVE;
}

/* =======================================================================
 *  GSmaterialLerpPEColor / GSmaterialLerpPEColor
 *  Address: 0x800E3D14, Size: 0x2C
 *
 *  Assembly:
 *    mr r5, r3                  ; entry
 *    mr r3, r4                  ; param
 *    addi r4, r5, 0x3C          ; &entry->peDescriptor
 *    bl fn_800E01D0             ; GXColor copy/lerp
 * ======================================================================= */
void GSmaterialLerpPEColor(GSmaterialEntry* entry, void* param) {
    fn_800E01D0(param, (void*)&entry->peDescriptor);
}

/* =======================================================================
 *  GSmaterialLerpSpecular / GSmaterialLerpSpecular
 *  Address: 0x800E3D40, Size: 0x2C
 *
 *  Assembly:
 *    mr r5, r3
 *    mr r3, r4
 *    addi r4, r5, 0x30          ; &entry->specular
 *    bl fn_800E01D0
 * ======================================================================= */
void GSmaterialLerpSpecular(GSmaterialEntry* entry, void* param) {
    fn_800E01D0(param, (void*)entry->specular);
}

/* =======================================================================
 *  GSmaterialLerpAmbient / GSmaterialLerpAmbient
 *  Address: 0x800E3D6C, Size: 0x2C
 *
 *  Assembly:
 *    mr r5, r3
 *    mr r3, r4
 *    addi r4, r5, 0x24          ; &entry->ambient
 *    bl fn_800E01D0
 * ======================================================================= */
void GSmaterialLerpAmbient(GSmaterialEntry* entry, void* param) {
    fn_800E01D0(param, (void*)entry->ambient);
}

/* =======================================================================
 *  GSmaterialLerpDiffuse / GSmaterialLerpDiffuse
 *  Address: 0x800E3D98, Size: 0x2C
 *
 *  Assembly:
 *    mr r5, r3
 *    mr r3, r4
 *    addi r4, r5, 0x18          ; &entry->diffuse
 *    bl fn_800E01D0
 * ======================================================================= */
void GSmaterialLerpDiffuse(GSmaterialEntry* entry, void* param) {
    fn_800E01D0(param, (void*)entry->diffuse);
}

/* =======================================================================
 *  GSmaterialSetCallback / GSmaterialSetCallback
 *  Address: 0x800E8F68, Size: 0xC
 *
 *  Assembly:
 *    stw r3, lbl_8047AB84@sda21(r0)
 *    stw r4, lbl_8047AB80@sda21(r0)
 *    blr
 * ======================================================================= */
void GSmaterialSetCallback(void* callback, void* state) {
    gsMatCallbackParam = (u32)callback;
    gsMatCallbackState = (u32)state;
}

/* ===================================================================
 * STUB FUNCTIONS -- All remaining functions in 0x800E3604-0x800EE2C8
 * =================================================================== */

/* fn_800E3604 -- GSmaterialApplyAll | Size: 0x15C */
extern u32 lbl_8047AB74;
extern u32 lbl_8047AB78;
/* Forward declarations for self-referencing asm blocks */
extern void fn_800EE044(void* result, void** dest);
extern void* fn_800E51A4(void*, void*);
extern void fn_800E6B20(void* entry);
extern void fn_800E6DCC();
extern void fn_800E85E8(void* obj);
extern void fn_800E9288();
extern void fn_800E92D8(void);
extern void fn_800E9358(void*, u8);
extern void fn_800E93B8(void);
extern void* fn_800E9998(void*, void*, void*, void*);
extern void fn_800E9E90(void);
extern s32 HSD_JObjMtxIsDirty();
extern void PSMTXConcat(void* A, void* B, void* AB);
extern void PSMTXInverse(void* src, void* dst);
extern void fn_800EA664();
extern void fn_800EA6D4();
extern void fn_800EA7E4();
extern void fn_800EA820();
extern void fn_800EA960(void*, void*, void*, s32, void*, void*, void*);
extern void fn_800EACD0(void*, void*, void*, u8, void*, void*);
extern void fn_800EAFE4(void*, void*, void*, void*);
extern void fn_800EB340(void* entry);
extern void fn_800EB414();
extern void fn_800EB6E0(void*, void*, void*, void*, f64);
extern void fn_800EB904(void);
extern void _modelSetRotateEulerToQuatAll__FP9_HSD_JObj(void*);
extern void fn_800EC1E4();
extern void fn_800EC208();
extern void fn_800EC2A4(void*, f32);
extern void fn_800EC308(void*, f32);
extern void fn_800EC35C(void* entry, u32 idx);
extern void fn_800EC990();
extern void fn_800EC9DC(void* entry, f32 val);
extern void fn_800ECB74(void* entry, s32 mode);
extern void fn_800ECCA8(void* entry, s32 idx);
extern void fn_800ED0D0(void* entry, f32 val);
extern void fn_800ED1CC(void* entry);
extern void fn_800ED4D4(void* entry);
extern s32 fn_800ED6AC(GSmaterialEntry* entry);
extern void fn_800ED6E4(void*, u8);
extern void fn_800ED7E4(void*, f32);
extern f32 fn_800ED8C4(s32, u8, f32, f32, f32, f32);
extern void fn_800EDA98(void*);
extern void fn_800EE054();
extern void _modelGetEndFrame();
extern u32 fn_800EE0E8(void*);
extern void* fn_800EE150(void* entry, u32 param);
extern void fn_800EE20C();

#if 0
asm void fn_800E3604(void) {
#include "src/game/gs_material_fn_800E3604.inc"
}
#else
void fn_800E3604(u32 flags, u8 slot) {
    void* mobj;
    s32 offset;
    s32 slotMatch;
    s32 animFlag;
    s32 envFlag;
    s32 shadowFlag;
    u32 i;

    fn_801B25C4(0x7f);
    if ((mobj = GScameraGetActiveCamera()) != NULL) {
        if (fn_80195A6C(*(void**)((u8*)mobj + 0xc)) != 0) {
            GSlightSetupLights(*(void**)((u8*)mobj + 0xc));
            slotMatch = (u8)slot;
            animFlag = flags & 0x10;
            envFlag = flags & 0x1000;
            shadowFlag = flags & 0x2000;
            i = 0;
            offset = 0;
            while (i < lbl_8047AB78) {
                u8* entry = (u8*)(lbl_8047AB74 + offset);
                {
                    u32 f = *(u32*)entry;
                    if ((f & 1) && (f & 2) && (f & 0x400400)) {
                        if ((s32)((f >> 9) & 1) == slotMatch) {
                            fn_800E9148(entry, 1);
                            if (*(u32*)entry & 0x80) {
                                mobj = *(void**)(entry + 0xc);
                            } else {
                                mobj = *(void**)(entry + 0x8);
                            }
                            if (animFlag != 0) {
                                fn_801A13CC(mobj, 0, 1, 0);
                                fn_800D6A5C(*(void**)(entry + 0x168), *(void**)(entry + 0x16c));
                            }
                            if (envFlag != 0) {
                                fn_801A13CC(mobj, 0, 4, 0);
                            }
                            if (shadowFlag != 0) {
                                fn_801A13CC(mobj, 0, 2, 0);
                            }
                            fn_800E9148(entry, 0);
                        }
                    }
                }
                offset += 0x170;
                i++;
            }
            fn_80195A48();
        }
    }
    fn_800D87AC(-1);
}
#endif

/* fn_800E3760 -- GSmaterialApplySingle | Size: 0x124 */
#if 0
asm void fn_800E3760(void) {
#include "src/game/gs_material_fn_800E3760.inc"
}
#else
void fn_800E3760(void* entry, u32 r4) {
    if (!(*(u32*)entry & 1)) return;
    fn_801B25C4(0x7f);
    {
        void* r31 = GScameraGetActiveCamera();
        if (r31 != NULL) {
            if (fn_80195A6C(*(void**)((u8*)r31 + 0xc)) != 0) {
                GSlightSetupLights(*(void**)((u8*)r31 + 0xc));
                fn_800E9148(entry, 1);
                {
                    void* mobj;
                    if (*(u32*)entry & 0x80) {
                        mobj = *(void**)((u8*)entry + 0xc);
                    } else {
                        mobj = *(void**)((u8*)entry + 0x8);
                    }
                    if (r4 & 0x10) {
                        fn_801A13CC(mobj, 0, 1, 0);
                        fn_800D6A5C(*(void**)((u8*)entry + 0x168), *(void**)((u8*)entry + 0x16c));
                    }
                    if (r4 & 0x1000) {
                        fn_801A13CC(mobj, 0, 4, 0);
                    }
                    if (r4 & 0x2000) {
                        fn_801A13CC(mobj, 0, 2, 0);
                    }
                }
                fn_800E9148(entry, 0);
                fn_80195A48();
            }
            if (*(u32*)entry & 0x200000) {
                fn_80190E60((u8*)entry + 0x4c);
            }
        }
    }
    fn_800D87AC(-1);
}
#endif

/* fn_800E3884 -- GSmaterialLerpColors | Size: 0xA4 */
extern void fn_800ED0D0(void* entry, f32 val);
extern u32 lbl_8047AB74;
extern u32 lbl_8047AB78;
#if 0
asm void fn_800E3884(void) {
#include "src/game/gs_material_fn_800E3884.inc"
}
#else
void fn_800E3884(s32 lerpVal, u8 typeFilter) {
    f64 f31 = *(f64*)lbl_8047CB58;
    u32 r28 = 0;
    u32 r29 = 0;
    while (r28 < lbl_8047AB78) {
        void* r27 = (u8*)(u32)lbl_8047AB74 + r29;
        if (*(u32*)r27 & 1) {
            if ((u8)fn_800ED6AC(r27) == typeFilter) {
                fn_800ED0D0(r27, (f32)((f64)(u32)lerpVal - f31));
            }
        }
        r29 += 0x170;
        r28++;
    }
}
#endif

/* fn_800E3928 -- GSmaterialUpdateColors | Size: 0x1E0 */
extern void fn_800EE3BC(void* obj, u8* buf0, u8* buf1, u8* buf2);
extern void fn_800EE828(void* obj);
extern void* memcpy(void* dst, const void* src, u32 n);
extern u32 lbl_8047AB74;
extern u32 lbl_8047AB78;
#if 0
asm void fn_800E3928(void) {
#include "src/game/gs_material_fn_800E3928.inc"
}
#else
void fn_800E3928(void) {
    u32 r29;
    u32 r30;
    u32 r31;
    u32 r28;
    /* Pass 1: update color init flags */
    r31 = 0;
    r28 = 0;
    while (r28 < lbl_8047AB78) {
        void* r29p = (void*)(lbl_8047AB74 + r31);
        u32 flags = *(u32*)r29p;
        if (flags & 0x1) {
            if (!(flags & 0x2)) {
                /* bit 1 clear: clear 0x400000 */
                flags &= ~0x400000u;
                *(u32*)r29p = flags;
            } else if (flags & 0x100000) {
                /* bits 1 and 20 set: call fn_80191118 */
                s32 res = ((s32(*)(void*))fn_80191118)((u8*)r29p + 0x4c);
                if (res >= 0) {
                    *(u32*)r29p |= 0x400000;
                } else {
                    *(u32*)r29p &= ~0x400000u;
                }
            } else {
                /* bit 1 set, bit 20 clear: set 0x400000 */
                *(u32*)r29p |= 0x400000;
            }
        }
        r31 += 0x170;
        r28++;
    }
    /* Pass 2: call fn_800ED1CC on entries with bits 0x400400 */
    {
        u32 mask = 0x400400;
        r29 = 0;
        r30 = 0;
        while (r29 < lbl_8047AB78) {
            void* entry = (void*)(lbl_8047AB74 + r30);
            u32 flags = *(u32*)entry;
            if ((flags & 0x1) && (flags & mask)) {
                fn_800ED1CC(entry);
            }
            r30 += 0x170;
            r29++;
        }
    }
    /* Pass 3: call fn_800ED4D4, then z-depth update, then fn_80190E60 if needed */
    r29 = 0;
    r30 = 0;
    while (r29 < lbl_8047AB78) {
        void* r28p = (void*)(lbl_8047AB74 + r30);
        if (*(u32*)r28p & 0x1) {
            void* r31p;
            fn_800ED4D4(r28p);
            if (*(u32*)r28p & 0x80) {
                memcpy((u8*)r28p + 0x3c, (u8*)r28p + 0x18, 0xc);
            } else {
                r31p = fn_800EE150(r28p, 1);
                if (r31p == NULL) {
                    memcpy((u8*)r28p + 0x3c, (u8*)r28p + 0x18, 0xc);
                } else {
                    fn_800EE3BC(r31p, (u8*)r28p + 0x3c, 0, 0);
                    fn_800EE828(r31p);
                    *(f32*)((u8*)r28p + 0x40) = *(f32*)((u8*)r28p + 0x40) - *(f32*)((u8*)r28p + 0x48);
                }
            }
            {
                u32 f2 = *(u32*)r28p;
                if ((f2 & 0x400000) && (f2 & 0x200000)) {
                    ((void(*)(void*))fn_80190E60)((u8*)r28p + 0x4c);
                }
            }
        }
        r30 += 0x170;
        r29++;
    }
}
#endif

/* fn_800E3B08 | Size: 0x34 */
extern u32 lbl_8047AB78;
extern u32 lbl_8047AB74;
#if 0
asm void fn_800E3B08(void) {
#include "src/game/gs_material_fn_800E3B08.inc"
}
#else
GSmaterialEntry* fn_800E3B08(u32 index) {
    GSmaterialEntry* entry;
    if (index >= lbl_8047AB78) {
        return NULL;
    }
    entry = (GSmaterialEntry*)((u8*)(u32)lbl_8047AB74 + index * 0x170);
    if (entry->flags & GSMAT_FLAG_ACTIVE) {
        return entry;
    }
    return NULL;
}
#endif

/* fn_800E3BC0 -- GSmaterialGetGXTexGenSrc | Size: 0x30 */
#if 0
asm void fn_800E3BC0(void) {
#include "src/game/gs_material_fn_800E3BC0.inc"
}
#else
void fn_800E3BC0(GSmaterialEntry* entry) {
    if (entry->texture != NULL) {
        fn_80118874(entry->texture, 1);
    }
}
#endif

/* fn_800E3DC4 -- GSmaterialUpdateAlpha | Size: 0x250 */
#if 0
asm void fn_800E3DC4(void) {
#include "src/game/gs_material_fn_800E3DC4.inc"
}
#else
void fn_800E3DC4(void* entry, void* param) {
    f32 stk[3];
    fn_800E01D0(stk, (u8*)entry + 0x24);
    fn_800E019C(stk, stk, param);
    if (*(s32*)((u8*)entry + 0x114) == 0) {
        void* r30;
        fn_800E01D0((u8*)entry + 0x24, stk);
        r30 = *(void**)((u8*)entry + 0x8);
        {
            f32 f31 = stk[0];
            if (r30 == NULL) __assert(lbl_8047CB60, 0x2a4, lbl_8047CB68);
            if (*(s32*)((u8*)r30 + 0x14) & 0x20000) __assert(lbl_8047CB60, 0x2a5, lbl_80270E28);
            *(f32*)((u8*)r30 + 0x1c) = f31;
            if ((*(s32*)((u8*)r30 + 0x14) & 0x2000000) && r30 != NULL) {
                s32 active;
                u32 flags;
                if (r30 == NULL) __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
                flags = *(s32*)((u8*)r30 + 0x14);
                if ((flags & 0x800000) || !(flags & 0x40)) active = 0; else active = 1;
                if (!active) fn_8019D620(r30);
            }
        }
        r30 = *(void**)((u8*)entry + 0x8);
        {
            f32 f31 = stk[1];
            if (r30 == NULL) __assert(lbl_8047CB60, 0x2b8, lbl_8047CB68);
            if (*(s32*)((u8*)r30 + 0x14) & 0x20000) __assert(lbl_8047CB60, 0x2b9, lbl_80270E28);
            *(f32*)((u8*)r30 + 0x20) = f31;
            if ((*(s32*)((u8*)r30 + 0x14) & 0x2000000) && r30 != NULL) {
                s32 active;
                u32 flags;
                if (r30 == NULL) __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
                flags = *(s32*)((u8*)r30 + 0x14);
                if ((flags & 0x800000) || !(flags & 0x40)) active = 0; else active = 1;
                if (!active) fn_8019D620(r30);
            }
        }
        r30 = *(void**)((u8*)entry + 0x8);
        {
            f32 f31 = stk[2];
            if (r30 == NULL) __assert(lbl_8047CB60, 0x2cc, lbl_8047CB68);
            if (*(s32*)((u8*)r30 + 0x14) & 0x20000) __assert(lbl_8047CB60, 0x2cd, lbl_80270E28);
            *(f32*)((u8*)r30 + 0x24) = f31;
            if ((*(s32*)((u8*)r30 + 0x14) & 0x2000000) && r30 != NULL) {
                s32 active;
                u32 flags;
                if (r30 == NULL) __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
                flags = *(s32*)((u8*)r30 + 0x14);
                if ((flags & 0x800000) || !(flags & 0x40)) active = 0; else active = 1;
                if (!active) fn_8019D620(r30);
            }
        }
    } else {
        fn_800E01D0((u8*)entry + 0x12c, stk);
    }
}
#endif

/* fn_800E4014 | Size: 0x68 */
extern void fn_80118C20(void);
#if 0
asm void fn_800E4014(void) {
#include "src/game/gs_material_fn_800E4014.inc"
}
#else
void fn_800E4014(GSmaterialEntry* entry, u8 enable) {
    extern void fn_80118C20(void* tex, u32 flag);
    if (enable) {
        entry->flags |= 0x2u;
        if (entry->texture != NULL) {
            fn_80118C20(entry->texture, 1);
        }
    } else {
        entry->flags &= ~0x2u;
        if (entry->texture != NULL) {
            fn_80118C20(entry->texture, 0);
        }
    }
}
#endif

/* fn_800E407C | Size: 0xF4 */
extern u8 lbl_8047CB70[6];
#if 0
asm void fn_800E407C(void) {
#include "src/game/gs_material_fn_800E407C.inc"
}
#else
void fn_800E407C(void* entry, void* r4) {
    if (*(s32*)((u8*)entry + 0x114) == 0) {
        void* r31;
        fn_800E01D0((u8*)entry + 0x30, r4);
        r31 = *(void**)((u8*)entry + 0x8);
        if (r31 == NULL) __assert(lbl_8047CB60, 0x316, lbl_8047CB68);
        if (r4 == NULL) __assert(lbl_8047CB60, 0x317, lbl_8047CB70);
        {
            struct S { u32 a, b, c; };
            *(struct S*)((u8*)r31 + 0x2c) = *(struct S*)((u8*)r4 + 0x0);
        }
        if (!(*(s32*)((u8*)r31 + 0x14) & 0x2000000) && r31 != NULL) {
            s32 r3;
            u32 flags;
            if (r31 == NULL) __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
            r3 = 0;
            flags = *(s32*)((u8*)r31 + 0x14);
            if (!(flags & 0x800000)) {
                if (flags & 0x40) { r3 = 1; }
            }
            if (r3 == 0) fn_8019D620(r31);
        }
    } else {
        fn_800E01D0((u8*)entry + 0x138, r4);
    }
}
#endif

/* fn_800E4170 -- SetupAlphaBlend | Size: 0x234 */
#if 0
asm void fn_800E4170(void) {
#include "src/game/gs_material_fn_800E4170.inc"
}
#else
void fn_800E4170(void* entry, f32* param) {
    void* r29;
    if (*(s32*)((u8*)entry + 0x114) != 0) goto _do_else_800E4170;
    fn_800E01D0((u8*)entry + 0x24, param);
    r29 = *(void**)((u8*)entry + 0x8);
    {
        f32 f31 = param[0];
        if (r29 == NULL) __assert(lbl_8047CB60, 0x2a4, lbl_8047CB68);
        if (*(s32*)((u8*)r29 + 0x14) & 0x20000) __assert(lbl_8047CB60, 0x2a5, lbl_80270E28);
        *(f32*)((u8*)r29 + 0x1c) = f31;
        if (!(*(s32*)((u8*)r29 + 0x14) & 0x2000000)) {
            if (r29 != NULL) {
                s32 active;
                u32 f2;
                if (r29 == NULL) __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
                f2 = *(u32*)((u8*)r29 + 0x14);
                active = 0;
                if (!(f2 & 0x800000)) {
                    if (f2 & 0x40) active = 1;
                }
                if (!active) fn_8019D620(r29);
            }
        }
    }
    r29 = *(void**)((u8*)entry + 0x8);
    {
        f32 f31 = param[1];
        if (r29 == NULL) __assert(lbl_8047CB60, 0x2b8, lbl_8047CB68);
        if (*(s32*)((u8*)r29 + 0x14) & 0x20000) __assert(lbl_8047CB60, 0x2b9, lbl_80270E28);
        *(f32*)((u8*)r29 + 0x20) = f31;
        if (!(*(s32*)((u8*)r29 + 0x14) & 0x2000000)) {
            if (r29 != NULL) {
                s32 active;
                u32 f2;
                if (r29 == NULL) __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
                f2 = *(u32*)((u8*)r29 + 0x14);
                active = 0;
                if (!(f2 & 0x800000)) {
                    if (f2 & 0x40) active = 1;
                }
                if (!active) fn_8019D620(r29);
            }
        }
    }
    r29 = *(void**)((u8*)entry + 0x8);
    {
        f32 f31 = param[2];
        if (r29 == NULL) __assert(lbl_8047CB60, 0x2cc, lbl_8047CB68);
        if (*(s32*)((u8*)r29 + 0x14) & 0x20000) __assert(lbl_8047CB60, 0x2cd, lbl_80270E28);
        *(f32*)((u8*)r29 + 0x24) = f31;
        if (!(*(s32*)((u8*)r29 + 0x14) & 0x2000000)) {
            if (r29 != NULL) {
                s32 active;
                u32 f2;
                if (r29 == NULL) __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
                f2 = *(u32*)((u8*)r29 + 0x14);
                active = 0;
                if (!(f2 & 0x800000)) {
                    if (f2 & 0x40) active = 1;
                }
                if (!active) fn_8019D620(r29);
            }
        }
    }
    return;
_do_else_800E4170:
    fn_800E01D0((u8*)entry + 0x12c, param);
}
#endif

/* fn_800E43A4 -- SetupZMode | Size: 0x170 */
extern u8 lbl_80270E50[];
#if 0
asm void fn_800E43A4(void) {
#include "src/game/gs_material_fn_800E43A4.inc"
}
#else
void fn_800E43A4(void* entry, void* param) {
    void* r30;
    if (*(s32*)((u8*)entry + 0x114) == 0) {
        fn_800E01D0((u8*)entry + 0x18, param);
        r30 = *(void**)((u8*)entry + 0x8);
        if (r30 == NULL) {
            __assert(lbl_8047CB60, 0x3a9, lbl_8047CB68);
        }
        if (param == NULL) {
            __assert(lbl_8047CB60, 0x3aa, lbl_80270E50);
        }
        {
            struct ZModeAB { s32 a; s32 b; };
            *(struct ZModeAB*)((u8*)r30 + 0x38) = *(struct ZModeAB*)((u8*)param + 0x0);
        }
        *(s32*)((u8*)r30 + 0x40) = *(s32*)((u8*)param + 0x8);
        {
            u32 flags = *(s32*)((u8*)r30 + 0x14);
            if (!(flags & 0x2000000)) {
                if (r30 != NULL) {
                    s32 active;
                    u32 f2;
                    if (r30 == NULL) {
                        __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
                    }
                    f2 = *(u32*)((u8*)r30 + 0x14);
                    active = 0;
                    if (!(f2 & 0x800000)) {
                        if (f2 & 0x40) {
                            active = 1;
                        }
                    }
                    if (!active) {
                        fn_8019D620(r30);
                    }
                }
            }
        }
    } else {
        fn_800E01D0((u8*)entry + 0x120, param);
    }
    {
        u32 flags0 = *(s32*)((u8*)entry + 0x0);
        if (flags0 & 0x80) {
            memcpy((u8*)entry + 0x3c, (u8*)entry + 0x18, 0xc);
        } else {
            r30 = fn_800EE150(entry, 1);
            if (r30 == NULL) {
                memcpy((u8*)entry + 0x3c, (u8*)entry + 0x18, 0xc);
            } else {
                fn_800EE3BC(r30, (u8*)entry + 0x3c, 0, 0);
                fn_800EE828(r30);
                *(f32*)((u8*)entry + 0x40) = *(f32*)((u8*)entry + 0x40) - *(f32*)((u8*)entry + 0x48);
            }
        }
    }
}
#endif

/* fn_800E4514 | Size: 0x84 */
extern void fn_8019D9DC(void* mobj);
#if 0
asm void fn_800E4514(void) {
#include "src/game/gs_material_fn_800E4514.inc"
}
#else
void* fn_800E4514(void* entry) {
    void* mobj = *(void**)((u8*)entry + 0x8);
    if (mobj != NULL) {
        u32 flags = *(u32*)((u8*)mobj + 0x14);
        s32 r3 = 0;
        if (!(flags & 0x800000)) {
            if (flags & 0x40) {
                r3 = 1;
            }
        }
        if (r3 != 0) {
            fn_8019D9DC(mobj);
        }
    }
    return (u8*)*(void**)((u8*)entry + 0x8) + 0x44;
}
#endif

/* fn_800E4598 | Size: 0xC4 */
extern void fn_800A2D64(void* dst, u8* src);
extern u8 lbl_8047CB78[4];
#if 0
asm void fn_800E4598(void) {
#include "src/game/gs_material_fn_800E4598.inc"
}
#else
void fn_800E4598(void* entry, void* r4) {
    void* r31 = *(void**)((u8*)entry + 0x8);
    if (r31 == NULL) {
        __assert(lbl_8047CB60, 0x495, lbl_8047CB68);
    }
    if (r4 == NULL) {
        __assert(lbl_8047CB60, 0x496, lbl_8047CB78);
    }
    fn_800A2D64(r4, (u8*)r31 + 0x44);
    {
        u32 flags = *(u32*)((u8*)r31 + 0x14);
        flags |= 0x3800000;
        *(u32*)((u8*)r31 + 0x14) = flags;
        if (r31 != NULL) {
            s32 active;
            u32 f2;
            if (r31 == NULL) {
                __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
            }
            f2 = *(u32*)((u8*)r31 + 0x14);
            active = 0;
            if (!(f2 & 0x800000)) {
                if (f2 & 0x40) {
                    active = 1;
                }
            }
            if (!active) {
                fn_8019D620(r31);
            }
        }
    }
}
#endif

/* fn_800E465C -- TEVStageSetup | Size: 0x464 */
#if 0
asm void fn_800E465C(void) {
#include "src/game/gs_material_fn_800E465C.inc"
}
#else
void fn_800E465C(void* entry, u8 r4) {
    void* r30;
    if (*(s32*)((u8*)entry + 0x114) == 0) return;
    *(u32*)((u8*)entry + 0x118) = 0;
    *(u32*)((u8*)entry + 0x11c) = 0;
    *(u32*)entry &= ~0xC0000u;
    *(u32*)((u8*)entry + 0x114) = 0;
    if (!r4) return;
    /* Section 1: entry+0x120 ? mobj+0x38/3c/40 */
    if (*(s32*)((u8*)entry + 0x114) == 0) {
        fn_800E01D0((u8*)entry + 0x18, (u8*)entry + 0x120);
        r30 = *(void**)((u8*)entry + 0x8);
        if (r30 == NULL) __assert(lbl_8047CB60, 0x3a9, lbl_8047CB68);
        if (((u8*)entry + 0x120) == NULL) __assert(lbl_8047CB60, 0x3aa, lbl_80270E50);
        *(u32*)((u8*)r30 + 0x38) = *(u32*)((u8*)entry + 0x120);
        *(u32*)((u8*)r30 + 0x3c) = *(u32*)((u8*)entry + 0x124);
        *(u32*)((u8*)r30 + 0x40) = *(u32*)((u8*)entry + 0x128);
        if ((*(u32*)((u8*)r30 + 0x14) & 0x2000000) && r30 != NULL) {
            s32 active;
            u32 flags;
            if (r30 == NULL) __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
            flags = *(u32*)((u8*)r30 + 0x14);
            if ((flags & 0x800000) || !(flags & 0x40)) active = 0; else active = 1;
            if (!active) fn_8019D620(r30);
        }
    } else {
        fn_800E01D0((u8*)entry + 0x120, (u8*)entry + 0x120);
    }
    /* Z-depth */
    if (*(u32*)entry & 0x80) {
        memcpy((u8*)entry + 0x3c, (u8*)entry + 0x18, 0xc);
    } else {
        r30 = fn_800EE150(entry, 1);
        if (r30 == NULL) {
            memcpy((u8*)entry + 0x3c, (u8*)entry + 0x18, 0xc);
        } else {
            fn_800EE3BC(r30, (u8*)entry + 0x3c, 0, 0);
            fn_800EE828(r30);
            *(f32*)((u8*)entry + 0x40) = *(f32*)((u8*)entry + 0x40) - *(f32*)((u8*)entry + 0x48);
        }
    }
    /* Section 2: entry+0x12c ? mobj+0x1c/20/24 */
    if (*(s32*)((u8*)entry + 0x114) == 0) {
        fn_800E01D0((u8*)entry + 0x24, (u8*)entry + 0x12c);
        r30 = *(void**)((u8*)entry + 0x8);
        {
            f32 f31 = *(f32*)((u8*)entry + 0x12c);
            if (r30 == NULL) __assert(lbl_8047CB60, 0x2a4, lbl_8047CB68);
            if (*(u32*)((u8*)r30 + 0x14) & 0x20000) __assert(lbl_8047CB60, 0x2a5, lbl_80270E28);
            *(f32*)((u8*)r30 + 0x1c) = f31;
            if ((*(u32*)((u8*)r30 + 0x14) & 0x2000000) && r30 != NULL) {
                s32 active;
                u32 flags;
                if (r30 == NULL) __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
                flags = *(u32*)((u8*)r30 + 0x14);
                if ((flags & 0x800000) || !(flags & 0x40)) active = 0; else active = 1;
                if (!active) fn_8019D620(r30);
            }
        }
        r30 = *(void**)((u8*)entry + 0x8);
        {
            f32 f31 = *(f32*)((u8*)entry + 0x130);
            if (r30 == NULL) __assert(lbl_8047CB60, 0x2b8, lbl_8047CB68);
            if (*(u32*)((u8*)r30 + 0x14) & 0x20000) __assert(lbl_8047CB60, 0x2b9, lbl_80270E28);
            *(f32*)((u8*)r30 + 0x20) = f31;
            if ((*(u32*)((u8*)r30 + 0x14) & 0x2000000) && r30 != NULL) {
                s32 active;
                u32 flags;
                if (r30 == NULL) __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
                flags = *(u32*)((u8*)r30 + 0x14);
                if ((flags & 0x800000) || !(flags & 0x40)) active = 0; else active = 1;
                if (!active) fn_8019D620(r30);
            }
        }
        r30 = *(void**)((u8*)entry + 0x8);
        {
            f32 f31 = *(f32*)((u8*)entry + 0x134);
            if (r30 == NULL) __assert(lbl_8047CB60, 0x2cc, lbl_8047CB68);
            if (*(u32*)((u8*)r30 + 0x14) & 0x20000) __assert(lbl_8047CB60, 0x2cd, lbl_80270E28);
            *(f32*)((u8*)r30 + 0x24) = f31;
            if ((*(u32*)((u8*)r30 + 0x14) & 0x2000000) && r30 != NULL) {
                s32 active;
                u32 flags;
                if (r30 == NULL) __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
                flags = *(u32*)((u8*)r30 + 0x14);
                if ((flags & 0x800000) || !(flags & 0x40)) active = 0; else active = 1;
                if (!active) fn_8019D620(r30);
            }
        }
    } else {
        fn_800E01D0((u8*)entry + 0x12c, (u8*)entry + 0x12c);
    }
    /* Section 3: entry+0x138 ? mobj+0x2c/30/34 */
    if (*(s32*)((u8*)entry + 0x114) == 0) {
        fn_800E01D0((u8*)entry + 0x30, (u8*)entry + 0x138);
        r30 = *(void**)((u8*)entry + 0x8);
        if (r30 == NULL) __assert(lbl_8047CB60, 0x316, lbl_8047CB68);
        if (((u8*)entry + 0x138) == NULL) __assert(lbl_8047CB60, 0x317, lbl_8047CB70);
        *(u32*)((u8*)r30 + 0x2c) = *(u32*)((u8*)entry + 0x138);
        *(u32*)((u8*)r30 + 0x30) = *(u32*)((u8*)entry + 0x13c);
        *(u32*)((u8*)r30 + 0x34) = *(u32*)((u8*)entry + 0x140);
        if ((*(u32*)((u8*)r30 + 0x14) & 0x2000000) && r30 != NULL) {
            s32 active;
            u32 flags;
            if (r30 == NULL) __assert(lbl_8047CB60, 0x25d, lbl_8047CB68);
            flags = *(u32*)((u8*)r30 + 0x14);
            if ((flags & 0x800000) || !(flags & 0x40)) active = 0; else active = 1;
            if (!active) fn_8019D620(r30);
        }
    } else {
        fn_800E01D0((u8*)entry + 0x138, (u8*)entry + 0x138);
    }
}
#endif

/* fn_800E4AC0 -- TEVColorRegister | Size: 0x134 */
extern void fn_800E01F4(void*, f32, f32, f32);
extern f32 lbl_8047CB7C;
extern f32 lbl_8047CB80;
#if 0
asm void fn_800E4AC0(void) {
#include "src/game/gs_material_fn_800E4AC0.inc"
}
#else
void fn_800E4AC0(void* entry, void* r4, s32 r5, s32 r6, u8 r7) {
    if (*(s32*)((u8*)entry + 0x114) != 0) return;
    if (r5 == 0) return;
    *(s32*)((u8*)entry + 0x118) = *(s32*)((u8*)r4 + 0x4);
    *(s32*)((u8*)entry + 0x11c) = *(u16*)((u8*)r4 + 0x2);
    if (r7 == 1) {
        fn_800E01F4((u8*)entry + 0x120, lbl_8047CB7C, lbl_8047CB7C, lbl_8047CB7C);
        fn_800E01F4((u8*)entry + 0x12c, lbl_8047CB7C, lbl_8047CB7C, lbl_8047CB7C);
        fn_800E01F4((u8*)entry + 0x138, lbl_8047CB80, lbl_8047CB80, lbl_8047CB80);
    } else {
        fn_800E01D0((u8*)entry + 0x120, (u8*)entry + 0x18);
        fn_800E01D0((u8*)entry + 0x12c, (u8*)entry + 0x24);
        fn_800E01D0((u8*)entry + 0x138, (u8*)entry + 0x30);
    }
    fn_800E01F4((u8*)entry + 0x18, lbl_8047CB7C, lbl_8047CB7C, lbl_8047CB7C);
    fn_800E01F4((u8*)entry + 0x24, lbl_8047CB7C, lbl_8047CB7C, lbl_8047CB7C);
    fn_800E01F4((u8*)entry + 0x30, lbl_8047CB80, lbl_8047CB80, lbl_8047CB80);
    *(s32*)((u8*)entry + 0x114) = r5;
    {
        u32 flags = *(s32*)entry;
        flags |= 0x80000;
        *(s32*)entry = flags;
        if (r6 & 0xff) {
            flags = *(s32*)entry;
            flags |= 0x40000;
            *(s32*)entry = flags;
        }
    }
}
#endif

/* fn_800E4BF4 -- TEVSwapMode | Size: 0xA4 */
extern void fn_801A05EC(void* obj);
extern void* memset(void* dst, int val, u32 n);
#if 0
asm void fn_800E4BF4(void) {
#include "src/game/gs_material_fn_800E4BF4.inc"
}
#else
void fn_800E4BF4(void* entry) {
    fn_800E9288(entry);
    fn_800E85E8(entry);
    fn_800E6DCC(entry);
    if (*(u8*)((u8*)entry + 0x80) != 0) {
        *(u8*)((u8*)entry + 0x80) = 1;
        fn_800E6B20(entry);
    }
    fn_801A05EC(*(void**)((u8*)entry + 0x8));
    if (*(void**)((u8*)entry + 0xc) != NULL) {
        fn_801A05EC(*(void**)((u8*)entry + 0xc));
        fn_801A05EC(*(void**)((u8*)entry + 0x10));
        fn_801A05EC(*(void**)((u8*)entry + 0x14));
    }
    if (*(u32*)((u8*)entry + 0x144) != 0) {
        fn_80118874(*(void**)((u8*)entry + 0x144), 1);
    }
    memset(entry, 0, 0x170);
}
#endif

/* fn_800E4C98 | Size: 0x80 */
extern f32 lbl_8047CB7C;
extern f32 lbl_8047CB80;
#if 0
asm void fn_800E4C98(void) {
#include "src/game/gs_material_fn_800E4C98.inc"
}
#else
void fn_800E4C98(void* entry) {
    struct {
        u32 pad0;
        u32 flags;
        void* ptr;
        u32 pad1;
        u32 pad2;
        f32 f0, f1, f2;
        f32 f3, f4, f5;
        f32 f6, f7, f8;
        u32 pad3;
        u32 pad4;
    } desc;
    void* inner = *(void**)((u8*)entry + 0x4);
    void* obj = *(void**)inner;
    desc.pad0  = 0;
    desc.flags = *(u32*)((u8*)obj + 0x4) | 0x1000;
    desc.ptr   = obj;
    desc.pad1  = 0;
    desc.pad2  = 0;
    desc.f0 = desc.f1 = desc.f2 = lbl_8047CB7C;
    desc.f3 = desc.f4 = desc.f5 = lbl_8047CB80;
    desc.f6 = desc.f7 = desc.f8 = lbl_8047CB7C;
    desc.pad3  = 0;
    desc.pad4  = 0;
    ((void(*)(void*, void*))fn_800E51A4)(inner, &desc);
}
#endif

/* fn_800E4D18 | Size: 0x24 */
#if 0
asm void fn_800E4D18(void) {
#include "src/game/gs_material_fn_800E4D18.inc"
}
#else
void fn_800E4D18(GSmaterialEntry* entry) {
    fn_800E51A4(entry, (void*)entry->flags);
}
#endif

/* fn_800E4D3C | Size: 0x74 */
extern u32 lbl_8047AB78;
extern u16 lbl_8047AB70;
extern u32 lbl_8047AB74;
#if 0
asm void fn_800E4D3C(void) {
#include "src/game/gs_material_fn_800E4D3C.inc"
}
#else
void fn_800E4D3C(u32 count) {
    u16 handle;
    u32 i;
    lbl_8047AB78 = count;
    handle = fn_800E3534(count * 0x170);
    lbl_8047AB70 = handle;
    if (handle == 0) {
        return;
    }
    lbl_8047AB74 = (u32)fn_800E27B0((u32)handle);
    for (i = 0; i < lbl_8047AB78; i++) {
        *(u32*)((u8*)(u32)lbl_8047AB74 + i * 0x170) = 0;
    }
    fn_800E92D8();
}
#endif

/* fn_800E4DB0 | Size: 0xDC */
#if 0
asm void fn_800E4DB0(void) {
#include "src/game/gs_material_fn_800E4DB0.inc"
}
#else
void fn_800E4DB0(void* entry, void* r4) {
    fn_800E01D0((u8*)entry + 0x30, r4);
    {
        void* r31 = *(void**)((u8*)entry + 0x8);
        if (r31 == NULL) {
            __assert(lbl_8047CB60, 0x316, lbl_8047CB68);
        }
        if (r4 == NULL) {
            __assert(lbl_8047CB60, 0x317, lbl_8047CB70);
        }
        *(u32*)((u8*)r31 + 0x2c) = *(u32*)((u8*)r4 + 0x0);
        *(u32*)((u8*)r31 + 0x30) = *(u32*)((u8*)r4 + 0x4);
        *(u32*)((u8*)r31 + 0x34) = *(u32*)((u8*)r4 + 0x8);
        if (!(*(u32*)((u8*)r31 + 0x14) & 0x2000000)) {
            u32 flags = *(u32*)((u8*)r31 + 0x14);
            s32 r3 = 0;
            if (!(flags & 0x800000)) {
                if (flags & 0x40) {
                    r3 = 1;
                }
            }
            if (r3 == 0) {
                fn_8019D620(r31);
            }
        }
    }
}
#endif

/* fn_800E4E8C | Size: 0x21C */
#if 0
asm void fn_800E4E8C(void) {
#include "src/game/gs_material_fn_800E4E8C.inc"
}
#else
void fn_800E4E8C(void* entry, f32* r4) {
    void* r29;
    fn_800E01D0((u8*)entry + 0x24, r4);

    r29 = *(void**)((u8*)entry + 0x8);
    if (r29 == NULL) __assert(lbl_8047CB60, 0x2a4, lbl_8047CB68);
    if (*(u32*)((u8*)r29 + 0x14) & 0x20000)
        __assert(lbl_8047CB60, 0x2a5, lbl_80270E28);
    *(f32*)((u8*)r29 + 0x1c) = r4[0];
    if (!(*(u32*)((u8*)r29 + 0x14) & 0x2000000)) {
        u32 flags = *(u32*)((u8*)r29 + 0x14);
        s32 r3 = 0;
        if (!(flags & 0x800000)) {
            if (flags & 0x40) { r3 = 1; }
        }
        if (r3 == 0) fn_8019D620(r29);
    }

    r29 = *(void**)((u8*)entry + 0x8);
    if (r29 == NULL) __assert(lbl_8047CB60, 0x2b8, lbl_8047CB68);
    if (*(u32*)((u8*)r29 + 0x14) & 0x20000)
        __assert(lbl_8047CB60, 0x2b9, lbl_80270E28);
    *(f32*)((u8*)r29 + 0x20) = r4[1];
    if (!(*(u32*)((u8*)r29 + 0x14) & 0x2000000)) {
        u32 flags = *(u32*)((u8*)r29 + 0x14);
        s32 r3 = 0;
        if (!(flags & 0x800000)) {
            if (flags & 0x40) { r3 = 1; }
        }
        if (r3 == 0) fn_8019D620(r29);
    }

    r29 = *(void**)((u8*)entry + 0x8);
    if (r29 == NULL) __assert(lbl_8047CB60, 0x2cc, lbl_8047CB68);
    if (*(u32*)((u8*)r29 + 0x14) & 0x20000)
        __assert(lbl_8047CB60, 0x2cd, lbl_80270E28);
    *(f32*)((u8*)r29 + 0x24) = r4[2];
    if (!(*(u32*)((u8*)r29 + 0x14) & 0x2000000)) {
        u32 flags = *(u32*)((u8*)r29 + 0x14);
        s32 r3 = 0;
        if (!(flags & 0x800000)) {
            if (flags & 0x40) { r3 = 1; }
        }
        if (r3 == 0) fn_8019D620(r29);
    }
}
#endif

/* fn_800E50A8 | Size: 0xE0 */
#if 0
asm void fn_800E50A8(void) {
#include "src/game/gs_material_fn_800E50A8.inc"
}
#else
void fn_800E50A8(void* entry, void* r4) {
    fn_800E01D0((u8*)entry + 0x18, r4);
    {
        void* r31 = *(void**)((u8*)entry + 0x8);
        if (r31 == NULL) {
            __assert(lbl_8047CB60, 0x3a9, lbl_8047CB68);
        }
        if (r4 == NULL) {
            __assert(lbl_8047CB60, 0x3aa, lbl_80270E50);
        }
        *(u32*)((u8*)r31 + 0x38) = *(u32*)((u8*)r4 + 0x0);
        *(u32*)((u8*)r31 + 0x3c) = *(u32*)((u8*)r4 + 0x4);
        *(u32*)((u8*)r31 + 0x40) = *(u32*)((u8*)r4 + 0x8);
        if (!(*(u32*)((u8*)r31 + 0x14) & 0x2000000)) {
            u32 flags = *(u32*)((u8*)r31 + 0x14);
            s32 r3 = 0;
            if (!(flags & 0x800000)) {
                if (flags & 0x40) {
                    r3 = 1;
                }
            }
            if (r3 == 0) {
                fn_8019D620(r31);
            }
        }
    }
}
#endif

/* fn_800E5188 | Size: 0x1C */
#if 0
asm void fn_800E5188(void) {
#include "src/game/gs_material_fn_800E5188.inc"
}
#else
void* fn_800E5188(GSmaterialEntry* entry) {
    if (entry->flags & GSMAT_FLAG_RENDERTYPE) {
        return entry->mobjSecondary;
    }
    return entry->mobjPrimary;
}
#endif

/* fn_800E51A4 -- TEVMultiStageSetup | Size: 0x3AC */
extern void* fn_801A0FBC(void*);
extern void fn_8019147C(void*, void*);
extern void fn_80191474(void*, void*);
extern void fn_8019146C(void*, void*);
extern void fn_8019F1C4(void*, void*, void*);
extern u32 lbl_8047AB78;
extern u32 lbl_8047AB74;
extern f32 lbl_8047CB84;
extern f32 lbl_8047CB7C;
#if 0
asm void fn_800E51A4(void) {
#include "src/game/gs_material_fn_800E51A4.inc"
}
#else
void* fn_800E51A4(void* mobj_param, void* r30_param) {
    void* r31;
    /* Find first free pool slot */
    {
        u32 count = lbl_8047AB78;
        r31 = (void*)lbl_8047AB74;
        if (count > 0) {
            u32 i;
            for (i = 0; i < count; i++, r31 = (u8*)r31 + 0x170) {
                if (!(*(u32*)r31 & 0x1)) break;
            }
            if (i == count) r31 = NULL;
        } else {
            r31 = NULL;
        }
    }
    if (r31 == NULL) return NULL;
    /* Initialize the entry */
    memset(r31, 0, 0x170);
    *(u32*)r31 = 0x2001;
    *(u32*)((u8*)r31 + 0x84) = 0;
    *(u32*)((u8*)r31 + 0x88) = 0;
    *(u8*)((u8*)r31 + 0x80) = 0;
    *(u32*)((u8*)r31 + 0xdc) = 0;
    *(u16*)((u8*)r31 + 0x150) = 0;
    *(u32*)((u8*)r31 + 0x14c) = 0;
    *(u16*)((u8*)r31 + 0x152) = 0;
    *(u32*)((u8*)r31 + 0x158) = 0;
    *(u32*)((u8*)r31 + 0x15c) = 0;
    *(u32*)((u8*)r31 + 0x160) = 0;
    *(u16*)((u8*)r31 + 0x164) = 0;
    *(u32*)((u8*)r31 + 0x114) = 0;
    *(u32*)((u8*)r31 + 0x118) = 0;
    *(u32*)((u8*)r31 + 0x11c) = 0;
    *(u32*)((u8*)r31 + 0x148) = 1;
    *(void**)((u8*)r31 + 0x4) = mobj_param;
    {
        void* mobj = ((void*(*)(void*))fn_801A0FBC)(r30_param);
        *(void**)((u8*)r31 + 0x8) = mobj;
        *(u32*)((u8*)r31 + 0xc) = 0;
        fn_800E01F4((u8*)r31 + 0x18, *(f32*)((u8*)mobj + 0x38), *(f32*)((u8*)mobj + 0x3c), *(f32*)((u8*)mobj + 0x40));
        mobj = *(void**)((u8*)r31 + 0x8);
        fn_800E01F4((u8*)r31 + 0x24, *(f32*)((u8*)mobj + 0x1c), *(f32*)((u8*)mobj + 0x20), *(f32*)((u8*)mobj + 0x24));
        mobj = *(void**)((u8*)r31 + 0x8);
        fn_800E01F4((u8*)r31 + 0x30, *(f32*)((u8*)mobj + 0x2c), *(f32*)((u8*)mobj + 0x30), *(f32*)((u8*)mobj + 0x34));
        mobj = *(void**)((u8*)r31 + 0x8);
        fn_800E01F4((u8*)r31 + 0x3c, *(f32*)((u8*)mobj + 0x38), *(f32*)((u8*)mobj + 0x3c), *(f32*)((u8*)mobj + 0x40));
    }
    *(u32*)((u8*)r31 + 0xe4) = 0;
    *(u32*)((u8*)r31 + 0xe8) = (u32)-1;
    *(u32*)((u8*)r31 + 0xf0) = 0;
    *(u32*)((u8*)r31 + 0xf4) = (u32)-1;
    *(u32*)((u8*)r31 + 0xfc) = 0;
    *(u32*)((u8*)r31 + 0x100) = (u32)-1;
    *(u32*)((u8*)r31 + 0x108) = 0;
    *(u32*)((u8*)r31 + 0x10c) = (u32)-1;
    /* Check for textures (mobj->0xc) */
    {
        void* mobj = *(void**)((u8*)r31 + 0x4);
        if (*(u32*)((u8*)mobj + 0xc) != 0) {
            *(u32*)r31 |= 0x10;
        }
    }
    /* Check for texture array (mobj->0x4) */
    {
        void* mobj = *(void**)((u8*)r31 + 0x4);
        if (*(u32*)((u8*)mobj + 0x4) != 0) {
            void** texarray;
            u32 count;
            *(u32*)r31 |= 0x4;
            *(u32*)((u8*)r31 + 0x90) = (u32)-1;
            count = 0;
            texarray = *(void***)((u8*)mobj + 0x4);
            while (texarray[count] != NULL) count++;
            *(u32*)((u8*)r31 + 0x84) = count;
            fn_800ECCA8(r31, 0);
            ((void(*)(void*, f32))fn_800EC9DC)(r31, lbl_8047CB84);
            fn_800ECB74(r31, 1);
        }
    }
    /* Check for second array (mobj->0x8) */
    {
        void* mobj = *(void**)((u8*)r31 + 0x4);
        if (*(u32*)((u8*)mobj + 0x8) != 0) {
            void** arr2;
            u32 count;
            *(u32*)r31 |= 0x8;
            *(u32*)((u8*)r31 + 0xa8) = (u32)-1;
            count = 0;
            arr2 = *(void***)((u8*)mobj + 0x8);
            while (arr2[count] != NULL) count++;
            *(u32*)((u8*)r31 + 0x88) = count;
            fn_800EC35C(r31, 0);
            ((void(*)(void*, f32))fn_800EC308)(r31, lbl_8047CB84);
            fn_800EC208(r31, 1);
        }
    }
    /* Initialize pivot/rotation data */
    fn_8019147C((u8*)r31 + 0x4c, (u8*)r31 + 0x3c);
    fn_80191474((u8*)r31 + 0x4c, (u8*)r31 + 0x24);
    fn_8019146C((u8*)r31 + 0x4c, (u8*)r31 + 0x30);
    fn_800EB340(r31);
    *(f32*)((u8*)r31 + 0x48) = lbl_8047CB7C;
    /* Z-depth computation #1 */
    {
        void* r30;
        if (*(u32*)r31 & 0x80) {
            memcpy((u8*)r31 + 0x3c, (u8*)r31 + 0x18, 0xc);
        } else {
            r30 = fn_800EE150(r31, 1);
            if (r30 == NULL) {
                memcpy((u8*)r31 + 0x3c, (u8*)r31 + 0x18, 0xc);
            } else {
                fn_800EE3BC(r30, (u8*)r31 + 0x3c, 0, 0);
                fn_800EE828(r30);
                *(f32*)((u8*)r31 + 0x40) = *(f32*)((u8*)r31 + 0x40) - *(f32*)((u8*)r31 + 0x48);
            }
        }
    }
    /* Save z-depth and redo computation #2 */
    *(f32*)((u8*)r31 + 0x48) = *(f32*)((u8*)r31 + 0x40);
    {
        void* r30;
        if (*(u32*)r31 & 0x80) {
            memcpy((u8*)r31 + 0x3c, (u8*)r31 + 0x18, 0xc);
        } else {
            r30 = fn_800EE150(r31, 1);
            if (r30 == NULL) {
                memcpy((u8*)r31 + 0x3c, (u8*)r31 + 0x18, 0xc);
            } else {
                fn_800EE3BC(r30, (u8*)r31 + 0x3c, 0, 0);
                fn_800EE828(r30);
                *(f32*)((u8*)r31 + 0x40) = *(f32*)((u8*)r31 + 0x40) - *(f32*)((u8*)r31 + 0x48);
            }
        }
    }
    /* Register callback */
    fn_8019F1C4(*(void**)((u8*)r31 + 0x8), (u8*)r31 + 0x168, (u8*)r31 + 0x16c);
    return r31;
}
#endif

/* fn_800E5550 | Size: 0xEC */
extern void fn_800DEFC8(void* obj);
extern void fn_800DF608(void* obj);
#if 0
asm void fn_800E5550(void) {
#include "src/game/gs_material_fn_800E5550.inc"
}
#else
void fn_800E5550(void* obj) {
    u16 count2 = *(u16*)((u8*)obj + 0x152);
    if (count2 == 0) return;
    {
        s32 idx;
        s32 cnt;
        void** ptr;
        ptr = *(void***)((u8*)obj + 0x14c);
        count2--;
        cnt = *(u16*)((u8*)obj + 0x150);
        idx = 0;
        *(u16*)((u8*)obj + 0x152) = count2;
        while (idx < cnt) {
            void* p = *ptr;
            if (p != NULL) {
                fn_800DEFC8(p);
            }
            idx++;
            ptr++;
        }
        if (*(u16*)((u8*)obj + 0x152) != 0) return;
        cnt = *(u16*)((u8*)obj + 0x150);
        idx = 0;
        ptr = *(void***)((u8*)obj + 0x14c);
        while (idx < cnt) {
            void* p = *ptr;
            if (p != NULL) {
                fn_800DF608(p);
            }
            idx++;
            ptr++;
        }
        {
            u16 handle = *(u16*)((u8*)obj + 0x154);
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(handle);
            }
        }
        *(u16*)((u8*)obj + 0x154) = 0;
        *(u16*)((u8*)obj + 0x150) = 0;
        *(u32*)((u8*)obj + 0x14c) = 0;
    }
}
#endif

/* fn_800E563C | Size: 0x154 */
extern void fn_800EE758(void);
extern void* fn_800EE6B4(void* obj, s32 idx);
extern void fn_800DF028(void);
#if 0
asm void fn_800E563C(void) {
#include "src/game/gs_material_fn_800E563C.inc"
}
#else
void fn_800E563C(void* entry, void* param) {
    u16 r24;
    if (*(u16*)((u8*)entry + 0x152) == 0) {
        s32 r26 = 0;
        s32 r28 = ((s32(*)(void*))fn_800EE0E8)(entry);
        s32 r29;
        for (r29 = 0; r29 < r28; r29++) {
            void* r27 = fn_800EE150(entry, r29);
            r26 += ((s32(*)(void*))fn_800EE758)(r27);
            fn_800EE828(r27);
        }
        if (r26 == 0) {
            r26 = 0;
        } else {
            u16 r29h = ((u16(*)(s32))fn_800E3534)(r26 * 4);
            if ((u16)r29h == 0) {
                r26 = 0;
            } else {
                void** r25 = fn_800E27B0(r29h);
                s32 r24i = 0;
                *(void**)((u8*)entry + 0x14c) = r25;
                *(u16*)((u8*)entry + 0x154) = r29h;
                *(u16*)((u8*)entry + 0x150) = (u16)r26;
                for (; r24i < r28; r24i++) {
                    void* r23 = fn_800EE150(entry, r24i);
                    s32 r29i = ((s32(*)(void*))fn_800EE758)(r23);
                    s32 r27i;
                    for (r27i = 0; r27i < r29i; r27i++) {
                        *r25++ = fn_800EE6B4(r23, r27i);
                    }
                    fn_800EE828(r23);
                }
            }
        }
        r24 = (u16)r26;
    } else {
        r24 = *(u16*)((u8*)entry + 0x150);
    }
    *(u16*)((u8*)entry + 0x152) = *(u16*)((u8*)entry + 0x152) + 1;
    {
        void** r25 = *(void***)((u8*)entry + 0x14c);
        s32 r23 = 0;
        while (r23 < r24) {
            void* p = r25[r23];
            if (p != NULL) {
                ((void(*)(void*, void*))fn_800DF028)(p, param);
            }
            r23++;
            r25++;
        }
    }
}
#endif

/* fn_800E5790 | Size: 0xBC */
#if 0
asm void fn_800E5790(void) {
#include "src/game/gs_material_fn_800E5790.inc"
}
#else
void fn_800E5790(void* obj) {
    u16 count2 = *(u16*)((u8*)obj + 0x152);
    if (count2 == 0) return;
    *(u16*)((u8*)obj + 0x152) = (u16)(count2 - 1);
    if (*(u16*)((u8*)obj + 0x152) != 0) return;
    {
        void** r31;
        s32 r30 = *(u16*)((u8*)obj + 0x150);
        s32 r29 = 0;
        r31 = *(void***)((u8*)obj + 0x14c);
        while (r29 < r30) {
            void* p = *r31;
            if (p != NULL) {
                fn_800DF608(p);
            }
            r29++;
            r31++;
        }
        {
            u16 r29_h = *(u16*)((u8*)obj + 0x154);
            if (r29_h != 0) {
                fn_800E24B0(r29_h);
                fn_800E209C(r29_h);
            }
        }
        *(u16*)((u8*)obj + 0x154) = 0;
        *(u16*)((u8*)obj + 0x150) = 0;
        *(u32*)((u8*)obj + 0x14c) = 0;
    }
}
#endif

/* fn_800E584C | Size: 0x12C */
#if 0
asm void fn_800E584C(void) {
#include "src/game/gs_material_fn_800E584C.inc"
}
#else
void* fn_800E584C(void* entry, u32* outCount) {
    if (*(u16*)((u8*)entry + 0x152) != 0) {
        *outCount = *(u16*)((u8*)entry + 0x150);
    } else {
        s32 r26 = 0;
        s32 r28 = ((u32(*)(void*))fn_800EE0E8)(entry);
        s32 r29;
        {
            s32 r29i = 0;
            while (r29i < r28) {
                void* r27 = fn_800EE150(entry, r29i);
                r26 += ((s32(*)(void*))fn_800EE758)(r27);
                fn_800EE828(r27);
                r29i++;
            }
        }
        if (r26 != 0) {
            u16 handle = ((u16(*)(s32))fn_800E3534)(r26 * 4);
            r29 = handle;
            if ((u16)r29 != 0) {
                void** r25 = fn_800E27B0(handle);
                *(void**)((u8*)entry + 0x14c) = r25;
                *(u16*)((u8*)entry + 0x154) = (u16)r29;
                *(u16*)((u8*)entry + 0x150) = (u16)r26;
                {
                    s32 r24 = 0;
                    while (r24 < r28) {
                        void* r23 = fn_800EE150(entry, r24);
                        s32 r29j = ((s32(*)(void*))fn_800EE758)(r23);
                        s32 r27i = 0;
                        while (r27i < r29j) {
                            *r25++ = fn_800EE6B4(r23, r27i);
                            r27i++;
                        }
                        fn_800EE828(r23);
                        r24++;
                    }
                }
            } else {
                r26 = 0;
            }
        }
        *outCount = r26;
    }
    *(u16*)((u8*)entry + 0x152) = *(u16*)((u8*)entry + 0x152) + 1;
    return *(void**)((u8*)entry + 0x14c);
}
#endif

/* fn_800E5978 | Size: 0x50 */
extern void fn_800DF240(void);
#if 0
asm void fn_800E5978(void) {
#include "src/game/gs_material_fn_800E5978.inc"
}
#else
u32 fn_800E5978(void* p) {
    u16 handle = *(u16*)((u8*)p + 0x150);
    void* ptr;
    if (handle == 0) {
        return 0;
    }
    ptr = *(void**)((u8*)*(void**)((u8*)p + 0x14c));
    if (ptr == NULL) {
        return 0;
    }
    {
        extern u32 fn_800DF240(void*);
        return fn_800DF240(ptr) & 1;
    }
}
#endif

/* fn_800E59C8 | Size: 0xAC */
extern void fn_800DF11C(void* obj, u8* color);
#if 0
asm void fn_800E59C8(void) {
#include "src/game/gs_material_fn_800E59C8.inc"
}
#else
void fn_800E59C8(void* obj, u8* color) {
    extern u32 fn_800DF240(void*);
    u8 r0;
    if (*(u16*)((u8*)obj + 0x150) == 0) {
        r0 = 0;
    } else {
        void* p = *(void**)*(void**)((u8*)obj + 0x14c);
        if (p == NULL) {
            r0 = 0;
        } else {
            if (fn_800DF240(p) & 1) r0 = 1; else r0 = 0;
        }
    }
    if (r0 & 0xff) {
        fn_800DF11C(*(void**)*(void**)((u8*)obj + 0x14c), color);
    } else {
        color[2] = 0x7f;
        color[1] = 0x7f;
        color[0] = 0x7f;
        color[3] = 0xff;
    }
}
#endif

/* fn_800E5A74 | Size: 0xF4 */
extern void fn_800DF248(void* obj, void* param);
#if 0
asm void fn_800E5A74(void) {
#include "src/game/gs_material_fn_800E5A74.inc"
}
#else
void fn_800E5A74(void* obj) {
    u16 count2 = *(u16*)((u8*)obj + 0x152);
    if (count2 == 0) return;
    {
        void** r31 = *(void***)((u8*)obj + 0x14c);
        s32 r30 = *(u16*)((u8*)obj + 0x150);
        count2--;
        *(u16*)((u8*)obj + 0x152) = count2;
        if (*(u16*)((u8*)obj + 0x152) != 0) {
            s32 r29 = 0;
            while (r29 < r30) {
                void* p = r31[r29];
                if (p != NULL) {
                    fn_800DF248(p, (void*)1);
                }
                r29++;
            }
            return;
        }
        r30 = *(u16*)((u8*)obj + 0x150);
        r31 = *(void***)((u8*)obj + 0x14c);
        {
            s32 r29 = 0;
            while (r29 < r30) {
                void* p = r31[r29];
                if (p != NULL) {
                    fn_800DF608(p);
                }
                r29++;
            }
        }
        {
            u16 handle = *(u16*)((u8*)obj + 0x154);
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(handle);
            }
        }
        *(u16*)((u8*)obj + 0x154) = 0;
        *(u16*)((u8*)obj + 0x150) = 0;
        *(u32*)((u8*)obj + 0x14c) = 0;
    }
}
#endif

/* fn_800E5B68 | Size: 0x78 */
extern void fn_800DF1E4(void* obj, void* param);
#if 0
asm void fn_800E5B68(void) {
#include "src/game/gs_material_fn_800E5B68.inc"
}
#else
void fn_800E5B68(void* obj, void* param) {
    u16 i = 0;
    u16 count = *(u16*)((u8*)obj + 0x150);
    void** arr = *(void***)((u8*)obj + 0x14c);
    while (i < count) {
        void* p = *arr;
        if (p != NULL) {
            fn_800DF1E4(p, param);
        }
        i++;
        arr++;
    }
}
#endif

/* fn_800E5BE0 | Size: 0x160 */
extern void fn_800DF384(void);
#if 0
asm void fn_800E5BE0(void) {
#include "src/game/gs_material_fn_800E5BE0.inc"
}
#else
void fn_800E5BE0(void* entry, void* param) {
    u16 r24;
    if (*(u16*)((u8*)entry + 0x152) == 0) {
        s32 r26 = 0;
        s32 r28 = ((s32(*)(void*))fn_800EE0E8)(entry);
        s32 r29;
        for (r29 = 0; r29 < r28; r29++) {
            void* r27 = fn_800EE150(entry, r29);
            r26 += ((s32(*)(void*))fn_800EE758)(r27);
            fn_800EE828(r27);
        }
        if (r26 == 0) {
            r26 = 0;
        } else {
            u16 r29h = ((u16(*)(s32))fn_800E3534)(r26 * 4);
            if ((u16)r29h == 0) {
                r26 = 0;
            } else {
                void** r25 = fn_800E27B0(r29h);
                s32 r24i = 0;
                *(void**)((u8*)entry + 0x14c) = r25;
                *(u16*)((u8*)entry + 0x154) = r29h;
                *(u16*)((u8*)entry + 0x150) = (u16)r26;
                for (; r24i < r28; r24i++) {
                    void* r23 = fn_800EE150(entry, r24i);
                    s32 r29i = ((s32(*)(void*))fn_800EE758)(r23);
                    s32 r27i;
                    for (r27i = 0; r27i < r29i; r27i++) {
                        *r25++ = fn_800EE6B4(r23, r27i);
                    }
                    fn_800EE828(r23);
                }
            }
        }
        r24 = (u16)r26;
    } else {
        r24 = *(u16*)((u8*)entry + 0x150);
    }
    *(u16*)((u8*)entry + 0x152) = *(u16*)((u8*)entry + 0x152) + 1;
    {
        void** r25 = *(void***)((u8*)entry + 0x14c);
        s32 r23 = 0;
        while (r23 < r24) {
            void* p = r25[r23];
            if (p != NULL) {
                ((void(*)(void*, void*))fn_800DF1E4)(p, param);
                ((void(*)(void*, s32))fn_800DF384)(p, 1);
            }
            r23++;
            r25++;
        }
    }
}
#endif

/* fn_800E5D40 | Size: 0xF4 */
#if 0
asm void fn_800E5D40(void) {
#include "src/game/gs_material_fn_800E5D40.inc"
}
#else
void fn_800E5D40(void* obj) {
    u16 count2 = *(u16*)((u8*)obj + 0x152);
    if (count2 == 0) return;
    {
        void** r31 = *(void***)((u8*)obj + 0x14c);
        s32 r30 = *(u16*)((u8*)obj + 0x150);
        count2--;
        *(u16*)((u8*)obj + 0x152) = count2;
        if (*(u16*)((u8*)obj + 0x152) != 0) {
            s32 r29 = 0;
            while (r29 < r30) {
                void* p = r31[r29];
                if (p != NULL) { fn_800DF248(p, (void*)2); }
                r29++;
            }
            return;
        }
        r30 = *(u16*)((u8*)obj + 0x150);
        r31 = *(void***)((u8*)obj + 0x14c);
        {
            s32 r29 = 0;
            while (r29 < r30) {
                void* p = r31[r29];
                if (p != NULL) { fn_800DF608(p); }
                r29++;
            }
        }
        {
            u16 handle = *(u16*)((u8*)obj + 0x154);
            if (handle != 0) { fn_800E24B0(handle); fn_800E209C(handle); }
        }
        *(u16*)((u8*)obj + 0x154) = 0;
        *(u16*)((u8*)obj + 0x150) = 0;
        *(u32*)((u8*)obj + 0x14c) = 0;
    }
}
#endif

/* fn_800E5E34 | Size: 0x178 */
extern void fn_800DF208(void);
#if 0
asm void fn_800E5E34(void) {
#include "src/game/gs_material_fn_800E5E34.inc"
}
#else
void fn_800E5E34(void* entry, void* p2, void* p3, void* p4, void* p5) {
    u16 r21;
    if (*(u16*)((u8*)entry + 0x152) == 0) {
        s32 r23 = 0;
        s32 r25 = ((s32(*)(void*))fn_800EE0E8)(entry);
        s32 r26;
        for (r26 = 0; r26 < r25; r26++) {
            void* r24 = fn_800EE150(entry, r26);
            r23 += ((s32(*)(void*))fn_800EE758)(r24);
            fn_800EE828(r24);
        }
        if (r23 == 0) {
            r23 = 0;
        } else {
            u16 r26h = ((u16(*)(s32))fn_800E3534)(r23 * 4);
            if ((u16)r26h == 0) {
                r23 = 0;
            } else {
                void** r22 = fn_800E27B0(r26h);
                s32 r21i = 0;
                *(void**)((u8*)entry + 0x14c) = r22;
                *(u16*)((u8*)entry + 0x154) = r26h;
                *(u16*)((u8*)entry + 0x150) = (u16)r23;
                for (; r21i < r25; r21i++) {
                    void* r20 = fn_800EE150(entry, r21i);
                    s32 r26i = ((s32(*)(void*))fn_800EE758)(r20);
                    s32 r24i;
                    for (r24i = 0; r24i < r26i; r24i++) {
                        *r22++ = fn_800EE6B4(r20, r24i);
                    }
                    fn_800EE828(r20);
                }
            }
        }
        r21 = (u16)r23;
    } else {
        r21 = *(u16*)((u8*)entry + 0x150);
    }
    *(u16*)((u8*)entry + 0x152) = *(u16*)((u8*)entry + 0x152) + 1;
    {
        void** r22 = *(void***)((u8*)entry + 0x14c);
        s32 r20 = 0;
        while (r20 < r21) {
            void* p = r22[r20];
            if (p != NULL) {
                ((void(*)(void*, void*, void*, void*, void*))fn_800DF208)(p, p2, p3, p4, p5);
                ((void(*)(void*, s32))fn_800DF384)(p, 2);
            }
            r20++;
            r22++;
        }
    }
}
#endif

/* fn_800E5FAC | Size: 0x50 */
#if 0
asm void fn_800E5FAC(void) {
#include "src/game/gs_material_fn_800E5FAC.inc"
}
#else
u32 fn_800E5FAC(void* p) {
    u16 handle = *(u16*)((u8*)p + 0x150);
    void* ptr;
    if (handle == 0) {
        return 0;
    }
    ptr = *(void**)((u8*)*(void**)((u8*)p + 0x14c));
    if (ptr == NULL) {
        return 0;
    }
    {
        extern u32 fn_800DF240(void*);
        return (fn_800DF240(ptr) >> 2) & 1;
    }
}
#endif

/* fn_800E5FFC | Size: 0xF4 */
#if 0
asm void fn_800E5FFC(void) {
#include "src/game/gs_material_fn_800E5FFC.inc"
}
#else
void fn_800E5FFC(void* obj) {
    u16 count2 = *(u16*)((u8*)obj + 0x152);
    if (count2 == 0) return;
    {
        void** r31 = *(void***)((u8*)obj + 0x14c);
        s32 r30 = *(u16*)((u8*)obj + 0x150);
        count2--;
        *(u16*)((u8*)obj + 0x152) = count2;
        if (*(u16*)((u8*)obj + 0x152) != 0) {
            s32 r29 = 0;
            while (r29 < r30) {
                void* p = r31[r29];
                if (p != NULL) { fn_800DF248(p, (void*)4); }
                r29++;
            }
            return;
        }
        r30 = *(u16*)((u8*)obj + 0x150);
        r31 = *(void***)((u8*)obj + 0x14c);
        {
            s32 r29 = 0;
            while (r29 < r30) {
                void* p = r31[r29];
                if (p != NULL) { fn_800DF608(p); }
                r29++;
            }
        }
        {
            u16 handle = *(u16*)((u8*)obj + 0x154);
            if (handle != 0) { fn_800E24B0(handle); fn_800E209C(handle); }
        }
        *(u16*)((u8*)obj + 0x154) = 0;
        *(u16*)((u8*)obj + 0x150) = 0;
        *(u32*)((u8*)obj + 0x14c) = 0;
    }
}
#endif

/* fn_800E60F0 | Size: 0xCC */
extern void fn_800DF1B8(void* obj, f32 val);
#if 0
asm void fn_800E60F0(void) {
#include "src/game/gs_material_fn_800E60F0.inc"
}
#else
void fn_800E60F0(void* obj, f32 val) {
    u8 r0;
    if (*(u16*)((u8*)obj + 0x150) == 0) {
        r0 = 0;
    } else {
        void* p = *(void**)*(void**)((u8*)obj + 0x14c);
        if (p == NULL) {
            r0 = 0;
        } else {
            extern u32 fn_800DF240(void*);
            if (fn_800DF240(p) & 4) {
                r0 = 1;
            } else {
                r0 = 0;
            }
        }
    }
    if (!(r0 & 0xff)) return;
    {
        s32 r31;
        s32 r30 = *(u16*)((u8*)obj + 0x150);
        void** r29;
        if (r30 == 0) return;
        r29 = *(void***)((u8*)obj + 0x14c);
        r31 = 0;
        while (r31 < r30) {
            void* p = *r29;
            if (p != NULL) {
                fn_800DF1B8(p, val);
            }
            r31++;
            r29++;
        }
    }
}
#endif

/* fn_800E61BC | Size: 0x1D0 */
extern void fn_800DF1D0(void);
#if 0
asm void fn_800E61BC(void) {
#include "src/game/gs_material_fn_800E61BC.inc"
}
#else
void fn_800E61BC(void* entry, void* p2, void* p3, void* p4, f64 f1) {
    u8 visible;
    if (*(u16*)((u8*)entry + 0x150) == 0) {
        visible = 0;
    } else {
        void* first = *(void**)*(void***)((u8*)entry + 0x14c);
        if (first == NULL) {
            visible = 0;
        } else {
            s32 r3 = ((s32(*)(void*))fn_800DF240)(first);
            visible = (r3 & 0x4) ? 1 : 0;
        }
    }
    if (visible) return;
    {
        u16 r22;
        if (*(u16*)((u8*)entry + 0x152) == 0) {
            s32 r24 = 0;
            s32 r26 = ((s32(*)(void*))fn_800EE0E8)(entry);
            s32 r27;
            for (r27 = 0; r27 < r26; r27++) {
                void* r25 = fn_800EE150(entry, r27);
                r24 += ((s32(*)(void*))fn_800EE758)(r25);
                fn_800EE828(r25);
            }
            if (r24 == 0) {
                r24 = 0;
            } else {
                u16 r27h = ((u16(*)(s32))fn_800E3534)(r24 * 4);
                if ((u16)r27h == 0) {
                    r24 = 0;
                } else {
                    void** r23 = fn_800E27B0(r27h);
                    s32 r22i = 0;
                    *(void**)((u8*)entry + 0x14c) = r23;
                    *(u16*)((u8*)entry + 0x154) = r27h;
                    *(u16*)((u8*)entry + 0x150) = (u16)r24;
                    for (; r22i < r26; r22i++) {
                        void* r21 = fn_800EE150(entry, r22i);
                        s32 r27i = ((s32(*)(void*))fn_800EE758)(r21);
                        s32 r25i;
                        for (r25i = 0; r25i < r27i; r25i++) {
                            *r23++ = fn_800EE6B4(r21, r25i);
                        }
                        fn_800EE828(r21);
                    }
                }
            }
            r22 = (u16)r24;
        } else {
            r22 = *(u16*)((u8*)entry + 0x150);
        }
        *(u16*)((u8*)entry + 0x152) = *(u16*)((u8*)entry + 0x152) + 1;
        {
            void** r23 = *(void***)((u8*)entry + 0x14c);
            s32 r21 = 0;
            while (r21 < r22) {
                void* p = r23[r21];
                if (p != NULL) {
                    ((void(*)(void*, f64, void*, void*, void*))fn_800DF1D0)(p, f1, p2, p3, p4);
                    ((void(*)(void*, s32))fn_800DF384)(p, 4);
                }
                r21++;
                r23++;
            }
        }
    }
}
#endif

/* fn_800E638C | Size: 0xEC */
extern void fn_800DF470(void* obj);
#if 0
asm void fn_800E638C(void) {
#include "src/game/gs_material_fn_800E638C.inc"
}
#else
void fn_800E638C(void* obj) {
    u16 count2 = *(u16*)((u8*)obj + 0x152);
    if (count2 == 0) return;
    {
        s32 idx;
        s32 cnt;
        void** ptr;
        ptr = *(void***)((u8*)obj + 0x14c);
        count2--;
        cnt = *(u16*)((u8*)obj + 0x150);
        idx = 0;
        *(u16*)((u8*)obj + 0x152) = count2;
        while (idx < cnt) {
            void* p = *ptr;
            if (p != NULL) {
                fn_800DF470(p);
            }
            idx++;
            ptr++;
        }
        if (*(u16*)((u8*)obj + 0x152) != 0) return;
        cnt = *(u16*)((u8*)obj + 0x150);
        idx = 0;
        ptr = *(void***)((u8*)obj + 0x14c);
        while (idx < cnt) {
            void* p = *ptr;
            if (p != NULL) {
                fn_800DF608(p);
            }
            idx++;
            ptr++;
        }
        {
            u16 handle = *(u16*)((u8*)obj + 0x154);
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(handle);
            }
        }
        *(u16*)((u8*)obj + 0x154) = 0;
        *(u16*)((u8*)obj + 0x150) = 0;
        *(u32*)((u8*)obj + 0x14c) = 0;
    }
}
#endif

/* fn_800E6478 | Size: 0x154 */
extern void GSmaterialSetPEdescr(void);
#if 0
asm void fn_800E6478(void) {
#include "src/game/gs_material_fn_800E6478.inc"
}
#else
void fn_800E6478(void* entry, void* param) {
    u16 r24;
    if (*(u16*)((u8*)entry + 0x152) == 0) {
        s32 r26 = 0;
        s32 r28 = ((s32(*)(void*))fn_800EE0E8)(entry);
        s32 r29;
        for (r29 = 0; r29 < r28; r29++) {
            void* r27 = fn_800EE150(entry, r29);
            r26 += ((s32(*)(void*))fn_800EE758)(r27);
            fn_800EE828(r27);
        }
        if (r26 == 0) {
            r26 = 0;
        } else {
            u16 r29h = ((u16(*)(s32))fn_800E3534)(r26 * 4);
            if ((u16)r29h == 0) {
                r26 = 0;
            } else {
                void** r25 = fn_800E27B0(r29h);
                s32 r24i = 0;
                *(void**)((u8*)entry + 0x14c) = r25;
                *(u16*)((u8*)entry + 0x154) = r29h;
                *(u16*)((u8*)entry + 0x150) = (u16)r26;
                for (; r24i < r28; r24i++) {
                    void* r23 = fn_800EE150(entry, r24i);
                    s32 r29i = ((s32(*)(void*))fn_800EE758)(r23);
                    s32 r27i;
                    for (r27i = 0; r27i < r29i; r27i++) {
                        *r25++ = fn_800EE6B4(r23, r27i);
                    }
                    fn_800EE828(r23);
                }
            }
        }
        r24 = (u16)r26;
    } else {
        r24 = *(u16*)((u8*)entry + 0x150);
    }
    *(u16*)((u8*)entry + 0x152) = *(u16*)((u8*)entry + 0x152) + 1;
    {
        void** r25 = *(void***)((u8*)entry + 0x14c);
        s32 r23 = 0;
        while (r23 < r24) {
            void* p = r25[r23];
            if (p != NULL) {
                ((void(*)(void*, void*))GSmaterialSetPEdescr)(p, param);
            }
            r23++;
            r25++;
        }
    }
}
#endif

/* fn_800E65CC | Size: 0xEC */
extern void fn_800DF140(void* obj);
#if 0
asm void fn_800E65CC(void) {
#include "src/game/gs_material_fn_800E65CC.inc"
}
#else
void fn_800E65CC(void* obj) {
    u16 count2 = *(u16*)((u8*)obj + 0x152);
    if (count2 == 0) return;
    {
        s32 idx;
        s32 cnt;
        void** ptr;
        ptr = *(void***)((u8*)obj + 0x14c);
        count2--;
        cnt = *(u16*)((u8*)obj + 0x150);
        idx = 0;
        *(u16*)((u8*)obj + 0x152) = count2;
        while (idx < cnt) {
            void* p = *ptr;
            if (p != NULL) { fn_800DF140(p); }
            idx++;
            ptr++;
        }
        if (*(u16*)((u8*)obj + 0x152) != 0) return;
        cnt = *(u16*)((u8*)obj + 0x150);
        idx = 0;
        ptr = *(void***)((u8*)obj + 0x14c);
        while (idx < cnt) {
            void* p = *ptr;
            if (p != NULL) { fn_800DF608(p); }
            idx++;
            ptr++;
        }
        {
            u16 handle = *(u16*)((u8*)obj + 0x154);
            if (handle != 0) { fn_800E24B0(handle); fn_800E209C(handle); }
        }
        *(u16*)((u8*)obj + 0x154) = 0;
        *(u16*)((u8*)obj + 0x150) = 0;
        *(u32*)((u8*)obj + 0x14c) = 0;
    }
}
#endif

/* fn_800E66B8 | Size: 0x14C */
extern void fn_800DF188(void* obj);
#if 0
asm void fn_800E66B8(void) {
#include "src/game/gs_material_fn_800E66B8.inc"
}
#else
void fn_800E66B8(void* entry) {
    u16 r25;
    if (*(u16*)((u8*)entry + 0x152) == 0) {
        s32 r27 = 0;
        s32 r29 = ((u32(*)(void*))fn_800EE0E8)(entry);
        s32 r30;
        for (r30 = 0; r30 < r29; r30++) {
            void* r28 = fn_800EE150(entry, r30);
            r27 += ((s32(*)(void*))fn_800EE758)(r28);
            fn_800EE828(r28);
        }
        if (r27 != 0) {
            u16 r30h = ((u16(*)(s32))fn_800E3534)(r27 * 4);
            if ((u16)r30h != 0) {
                void** r26 = fn_800E27B0(r30h);
                *(void**)((u8*)entry + 0x14c) = r26;
                *(u16*)((u8*)entry + 0x154) = r30h;
                *(u16*)((u8*)entry + 0x150) = (u16)r27;
                {
                    s32 r25i;
                    for (r25i = 0; r25i < r29; r25i++) {
                        void* r24 = fn_800EE150(entry, r25i);
                        s32 r30i = ((s32(*)(void*))fn_800EE758)(r24);
                        s32 r28i;
                        for (r28i = 0; r28i < r30i; r28i++) {
                            *r26++ = fn_800EE6B4(r24, r28i);
                        }
                        fn_800EE828(r24);
                    }
                }
            } else {
                r27 = 0;
            }
        }
        r25 = (u16)r27;
    } else {
        r25 = *(u16*)((u8*)entry + 0x150);
    }
    *(u16*)((u8*)entry + 0x152) = *(u16*)((u8*)entry + 0x152) + 1;
    {
        void** r26 = *(void***)((u8*)entry + 0x14c);
        s32 r24 = 0;
        while (r24 < r25) {
            void* p = r26[r24];
            if (p != NULL) fn_800DF188(p);
            r24++;
        }
    }
}
#endif

/* fn_800E6804 | Size: 0xD4 */
extern void fn_800DF21C(void* obj, f32 val);
extern f64 lbl_8047CB90;
extern f32 lbl_8047CB88;
#if 0
asm void fn_800E6804(void) {
#include "src/game/gs_material_fn_800E6804.inc"
}
#else
void fn_800E6804(void* obj, f32 val) {
    if (*(u16*)((u8*)obj + 0x152) == 0) return;
    {
        s32 r29 = *(u16*)((u8*)obj + 0x150);
        s32 r30 = 0;
        void** r28 = *(void***)((u8*)obj + 0x14c);
        while (r30 < r29) {
            void* p = r28[r30];
            if (p != NULL) {
                u8 b = *(u8*)((u8*)p + 0x1);
                f32 f1 = (f32)((f64)b - lbl_8047CB90) * val / lbl_8047CB88;
                fn_800DF21C(p, f1);
            }
            r30++;
        }
    }
}
#endif

/* fn_800E68D8 | Size: 0xEC */
extern void fn_800DF504(void* obj);
#if 0
asm void fn_800E68D8(void) {
#include "src/game/gs_material_fn_800E68D8.inc"
}
#else
void fn_800E68D8(void* obj) {
    u16 count2 = *(u16*)((u8*)obj + 0x152);
    if (count2 == 0) return;
    {
        s32 idx;
        s32 cnt;
        void** ptr;
        ptr = *(void***)((u8*)obj + 0x14c);
        count2--;
        cnt = *(u16*)((u8*)obj + 0x150);
        idx = 0;
        *(u16*)((u8*)obj + 0x152) = count2;
        while (idx < cnt) {
            void* p = *ptr;
            if (p != NULL) { fn_800DF504(p); }
            idx++;
            ptr++;
        }
        if (*(u16*)((u8*)obj + 0x152) != 0) return;
        cnt = *(u16*)((u8*)obj + 0x150);
        idx = 0;
        ptr = *(void***)((u8*)obj + 0x14c);
        while (idx < cnt) {
            void* p = *ptr;
            if (p != NULL) { fn_800DF608(p); }
            idx++;
            ptr++;
        }
        {
            u16 handle = *(u16*)((u8*)obj + 0x154);
            if (handle != 0) { fn_800E24B0(handle); fn_800E209C(handle); }
        }
        *(u16*)((u8*)obj + 0x154) = 0;
        *(u16*)((u8*)obj + 0x150) = 0;
        *(u32*)((u8*)obj + 0x14c) = 0;
    }
}
#endif

/* fn_800E69C4 | Size: 0x15C */
extern void fn_800DF3F0(void);
extern void GSmaterialSetFlags(void);
#if 0
asm void fn_800E69C4(void) {
#include "src/game/gs_material_fn_800E69C4.inc"
}
#else
void fn_800E69C4(void* entry, s32 param) {
    u16 r24;
    if (*(u16*)((u8*)entry + 0x152) == 0) {
        s32 r26 = 0;
        s32 r28 = ((s32(*)(void*))fn_800EE0E8)(entry);
        s32 r29;
        for (r29 = 0; r29 < r28; r29++) {
            void* r27 = fn_800EE150(entry, r29);
            r26 += ((s32(*)(void*))fn_800EE758)(r27);
            fn_800EE828(r27);
        }
        if (r26 == 0) {
            r26 = 0;
        } else {
            u16 r29h = ((u16(*)(s32))fn_800E3534)(r26 * 4);
            if ((u16)r29h == 0) {
                r26 = 0;
            } else {
                void** r25 = fn_800E27B0(r29h);
                s32 r24i = 0;
                *(void**)((u8*)entry + 0x14c) = r25;
                *(u16*)((u8*)entry + 0x154) = r29h;
                *(u16*)((u8*)entry + 0x150) = (u16)r26;
                for (; r24i < r28; r24i++) {
                    void* r23 = fn_800EE150(entry, r24i);
                    s32 r29i = ((s32(*)(void*))fn_800EE758)(r23);
                    s32 r27i;
                    for (r27i = 0; r27i < r29i; r27i++) {
                        *r25++ = fn_800EE6B4(r23, r27i);
                    }
                    fn_800EE828(r23);
                }
            }
        }
        r24 = (u16)r26;
    } else {
        r24 = *(u16*)((u8*)entry + 0x150);
    }
    *(u16*)((u8*)entry + 0x152) = *(u16*)((u8*)entry + 0x152) + 1;
    {
        void** r25 = *(void***)((u8*)entry + 0x14c);
        s32 r23 = 0;
        while (r23 < r24) {
            void* p = r25[r23];
            if (p != NULL) {
                s32 r3 = ((s32(*)(void*))fn_800DF3F0)(p);
                ((void(*)(void*, s32))GSmaterialSetFlags)(p, param | r3);
            }
            r23++;
            r25++;
        }
    }
}
#endif

/* fn_800E6B20 | Size: 0xA8 */
extern void* fn_801A02B0(void* obj);
#if 0
asm void fn_800E6B20(void) {
#include "src/game/gs_material_fn_800E6B20.inc"
}
#else
void fn_800E6B20(void* entry) {
    u8 r4 = *(u8*)((u8*)entry + 0x80);
    if (r4 == 0) return;
    r4--;
    *(u8*)((u8*)entry + 0x80) = r4;
    if (*(u8*)((u8*)entry + 0x80) != 0) return;
    {
        u32 flags = *(u32*)entry;
        void* r3 = *(void**)((u8*)entry + 0x8);
        if (flags & 0x20000) {
            r3 = *(void**)((u8*)r3 + 0x10);
        }
        r3 = (r3 != NULL) ? *(void**)((u8*)r3 + 0x10) : NULL;
        if (r3 == NULL) return;
        {
            void* r0;
            do {
                r0 = (r3 != NULL) ? *(void**)((u8*)r3 + 0x8) : NULL;
                if (r0 != NULL) {
                    r3 = (r3 != NULL) ? *(void**)((u8*)r3 + 0x8) : NULL;
                }
            } while (r0 != NULL);
        }
        fn_801A02B0(r3);
    }
}
#endif

/* fn_800E6BC8 | Size: 0x1F8 */
extern void* fn_8019F718(void);
extern void fn_801A015C(void);
extern void fn_8019FF74(void);
extern void* fn_8019FF30(void* obj);
extern void fn_8019FE8C(void* obj, s32 flags);
extern void fn_800E090C(void);
extern f32 lbl_8047CB98;
extern u32 lbl_8047CB9C;
extern u32 lbl_8047CBA4;
extern u8 lbl_80270E60[];
#if 0
asm void fn_800E6BC8(void) {
#include "src/game/gs_material_fn_800E6BC8.inc"
}
#else
s32 fn_800E6BC8(void* entry) {
    if (*(u8*)((u8*)entry + 0x80) != 0) {
        *(u8*)((u8*)entry + 0x80) = *(u8*)((u8*)entry + 0x80) + 1;
        return 1;
    }
    {
        void* r29 = *(void**)((u8*)entry + 0x8);
        void* r31 = fn_8019F718();
        f32 stk[3];
        if (r31 == NULL) return 0;
        if (*(u32*)entry & 0x20000) r29 = *(void**)((u8*)r29 + 0x10);
        r29 = (r29 != NULL) ? *(void**)((u8*)r29 + 0x10) : NULL;
        if (r29 == NULL) {
            ((void(*)(void*, void*))fn_801A015C)(NULL, r31);
        } else {
            r29 = *(void**)((u8*)r29 + 0x10);
            /* Find tail of linked list via ->0x8 */
            while (r29 != NULL && *(void**)((u8*)r29 + 0x8) != NULL) {
                r29 = *(void**)((u8*)r29 + 0x8);
            }
            ((void(*)(void*, void*))fn_8019FF74)(r29, r31);
        }
        {
            void* r4 = fn_8019FF30(r29);
            ((void(*)(void*, void*))fn_8019FE8C)(r31, r4);
        }
        ((void(*)(void*, void*, void*, f32))fn_800E090C)(stk, (u8*)entry + 0x5c, (u8*)entry + 0x68, lbl_8047CB98);
        if (r31 == NULL) __assert(&lbl_8047CB9C, 0x3a9, &lbl_8047CBA4);
        if ((u8*)stk == NULL) __assert(&lbl_8047CB9C, 0x3aa, lbl_80270E60);
        *(u32*)((u8*)r31 + 0x38) = *(u32*)&stk[0];
        *(u32*)((u8*)r31 + 0x3c) = *(u32*)&stk[1];
        *(u32*)((u8*)r31 + 0x40) = *(u32*)&stk[2];
        if ((*(u32*)((u8*)r31 + 0x14) & 0x2000000) && r31 != NULL) {
            s32 active;
            u32 flags;
            if (r31 == NULL) __assert(&lbl_8047CB9C, 0x25d, &lbl_8047CBA4);
            flags = *(u32*)((u8*)r31 + 0x14);
            if ((flags & 0x800000) || !(flags & 0x40)) active = 0; else active = 1;
            if (!active) fn_8019D620(r31);
        }
        if (r31 != NULL) {
            u32 flags = *(u32*)((u8*)r31 + 0x14);
            s32 active = 0;
            if (!(flags & 0x800000) && (flags & 0x40)) active = 1;
            if (active) fn_8019D9DC(r31);
        } else {
            __assert(&lbl_8047CB9C, 0x25d, &lbl_8047CBA4);
        }
    }
    *(u8*)((u8*)entry + 0x80) = *(u8*)((u8*)entry + 0x80) + 1;
    return 1;
}
#endif

/* fn_800E6DC0 | Size: 0xC */
#if 0
asm void fn_800E6DC0(void) {
#include "src/game/gs_material_fn_800E6DC0.inc"
}
#else
u32 fn_800E6DC0(void* p) {
    return (*(u32*)p >> 17) & 1;
}
#endif

/* fn_800E6DCC -- TEV descriptor builder | Size: 0x4C4 */
extern u32 lbl_8047CBAC;
extern u8 lbl_80270E6C[];
#if 0
asm void fn_800E6DCC(void) {
#include "src/game/gs_material_fn_800E6DCC.inc"
}
#else
void fn_800E6DCC(void* r29) {
    void* r30;
    void* r31;
    f32 fA;
    f32 fB;
    f32 fC;
    u32 stk0[3]; /* at sp+0x14 */
    u32 stk1[3]; /* at sp+0x08 */

    if (!(*(u32*)r29 & 0x20000)) {
        return;
    }

    r30 = *(void**)((u8*)r29 + 0x8);
    if (r30 == NULL) {
        r31 = NULL;
    } else {
        r31 = *(void**)((u8*)r30 + 0x10);
    }

    if (r30 == NULL) {
        __assert(&lbl_8047CB9C, 0x3e4, &lbl_8047CBA4);
    }
    if ((u32)&stk0[0] == 0) {
        __assert(&lbl_8047CB9C, 0x3e5, lbl_80270E60);
    }

    stk0[0] = *(u32*)((u8*)r30 + 0x38);
    stk0[1] = *(u32*)((u8*)r30 + 0x3c);
    stk0[2] = *(u32*)((u8*)r30 + 0x40);

    if (r30 == NULL) {
        __assert(&lbl_8047CB9C, 0x2ec, &lbl_8047CBA4);
    }
    fA = *(f32*)((u8*)r30 + 0x1c);

    if (r30 == NULL) {
        __assert(&lbl_8047CB9C, 0x2fa, &lbl_8047CBA4);
    }
    fB = *(f32*)((u8*)r30 + 0x20);

    if (r30 == NULL) {
        __assert(&lbl_8047CB9C, 0x308, &lbl_8047CBA4);
    }
    fC = *(f32*)((u8*)r30 + 0x24);

    if ((u32)&stk1[0] == 0) {
        __assert(&lbl_8047CB9C, 0x352, &lbl_8047CBAC);
    }

    stk1[0] = *(u32*)((u8*)r30 + 0x2c);
    stk1[1] = *(u32*)((u8*)r30 + 0x30);
    stk1[2] = *(u32*)((u8*)r30 + 0x34);

    if (r31 == NULL) {
        __assert(&lbl_8047CB9C, 0x3a9, &lbl_8047CBA4);
    }
    if ((u32)&stk0[0] == 0) {
        __assert(&lbl_8047CB9C, 0x3aa, lbl_80270E60);
    }

    *(u32*)((u8*)r31 + 0x38) = stk0[0];
    *(u32*)((u8*)r31 + 0x3c) = stk0[1];
    *(u32*)((u8*)r31 + 0x40) = stk0[2];

    if (!(*(u32*)((u8*)r31 + 0x14) & 0x02000000)) {
        if (r31 != NULL) {
            u32 fl;
            s32 check;
            if (r31 == NULL) {
                __assert(&lbl_8047CB9C, 0x25d, &lbl_8047CBA4);
            }
            fl = *(u32*)((u8*)r31 + 0x14);
            check = 0;
            if (!(fl & 0x800000) && (fl & 0x40)) {
                check = 1;
            }
            if (!check) {
                fn_8019D620(r31);
            }
        }
    }

    if (r31 == NULL) {
        __assert(&lbl_8047CB9C, 0x2a4, &lbl_8047CBA4);
    }
    if (*(u32*)((u8*)r31 + 0x14) & 0x20000) {
        __assert(&lbl_8047CB9C, 0x2a5, lbl_80270E6C);
    }
    *(f32*)((u8*)r31 + 0x1c) = fA;

    if (!(*(u32*)((u8*)r31 + 0x14) & 0x02000000)) {
        if (r31 != NULL) {
            u32 fl;
            s32 check;
            if (r31 == NULL) {
                __assert(&lbl_8047CB9C, 0x25d, &lbl_8047CBA4);
            }
            fl = *(u32*)((u8*)r31 + 0x14);
            check = 0;
            if (!(fl & 0x800000) && (fl & 0x40)) {
                check = 1;
            }
            if (!check) {
                fn_8019D620(r31);
            }
        }
    }

    if (r31 == NULL) {
        __assert(&lbl_8047CB9C, 0x2b8, &lbl_8047CBA4);
    }
    if (*(u32*)((u8*)r31 + 0x14) & 0x20000) {
        __assert(&lbl_8047CB9C, 0x2b9, lbl_80270E6C);
    }
    *(f32*)((u8*)r31 + 0x20) = fB;

    if (!(*(u32*)((u8*)r31 + 0x14) & 0x02000000)) {
        if (r31 != NULL) {
            u32 fl;
            s32 check;
            if (r31 == NULL) {
                __assert(&lbl_8047CB9C, 0x25d, &lbl_8047CBA4);
            }
            fl = *(u32*)((u8*)r31 + 0x14);
            check = 0;
            if (!(fl & 0x800000) && (fl & 0x40)) {
                check = 1;
            }
            if (!check) {
                fn_8019D620(r31);
            }
        }
    }

    if (r31 == NULL) {
        __assert(&lbl_8047CB9C, 0x2cc, &lbl_8047CBA4);
    }
    if (*(u32*)((u8*)r31 + 0x14) & 0x20000) {
        __assert(&lbl_8047CB9C, 0x2cd, lbl_80270E6C);
    }
    *(f32*)((u8*)r31 + 0x24) = fC;

    if (!(*(u32*)((u8*)r31 + 0x14) & 0x02000000)) {
        if (r31 != NULL) {
            u32 fl;
            s32 check;
            if (r31 == NULL) {
                __assert(&lbl_8047CB9C, 0x25d, &lbl_8047CBA4);
            }
            fl = *(u32*)((u8*)r31 + 0x14);
            check = 0;
            if (!(fl & 0x800000) && (fl & 0x40)) {
                check = 1;
            }
            if (!check) {
                fn_8019D620(r31);
            }
        }
    }

    if (r31 == NULL) {
        __assert(&lbl_8047CB9C, 0x316, &lbl_8047CBA4);
    }
    if ((u32)&stk1[0] == 0) {
        __assert(&lbl_8047CB9C, 0x317, &lbl_8047CBAC);
    }

    *(u32*)((u8*)r31 + 0x2c) = stk1[0];
    *(u32*)((u8*)r31 + 0x30) = stk1[1];
    *(u32*)((u8*)r31 + 0x34) = stk1[2];

    if (!(*(u32*)((u8*)r31 + 0x14) & 0x02000000)) {
        if (r31 != NULL) {
            u32 fl;
            s32 check;
            if (r31 == NULL) {
                __assert(&lbl_8047CB9C, 0x25d, &lbl_8047CBA4);
            }
            fl = *(u32*)((u8*)r31 + 0x14);
            check = 0;
            if (!(fl & 0x800000) && (fl & 0x40)) {
                check = 1;
            }
            if (!check) {
                fn_8019D620(r31);
            }
        }
    }

    r30 = fn_801A02B0(r30);

    if (r31 != NULL) {
        u32 fl;
        s32 check;
        if (r31 == NULL) {
            __assert(&lbl_8047CB9C, 0x25d, &lbl_8047CBA4);
        }
        fl = *(u32*)((u8*)r31 + 0x14);
        check = 0;
        if (!(fl & 0x800000) && (fl & 0x40)) {
            check = 1;
        }
        if (check) {
            fn_8019D9DC(r31);
        }
    }

    *(u32*)r29 &= ~0x20000;
    *(void**)((u8*)r29 + 0x8) = r30;
}
#endif

/* fn_800E7290 -- TEV stage count | Size: 0x9C */
extern void fn_800E0204(void* out);
#if 0
asm void fn_800E7290(void) {
#include "src/game/gs_material_fn_800E7290.inc"
}
#else
#pragma push
#pragma scheduling on
void fn_800E7290(void* entry, void* out) {
    u32 flags = *(u32*)entry;
    void* mobj = *(void**)((u8*)entry + 0x8);
    if (flags & 0x20000) {
        void* r31 = *(void**)((u8*)mobj + 0x10);
        if (r31 == NULL) __assert(&lbl_8047CB9C, 0x3e4, &lbl_8047CBA4);
        if (out == NULL) __assert(&lbl_8047CB9C, 0x3e5, lbl_80270E60);
        {
            struct s3w { u32 a, b, c; };
            *(struct s3w*)out = *(struct s3w*)((u8*)r31 + 0x38);
        }
    } else {
        fn_800E0204(out);
    }
}
#pragma pop
#endif

/* fn_800E732C -- FULL TEV PIPELINE | Size: 0x12BC */
extern u32 lbl_8047CBB4;
extern u32 lbl_8047CBB8;
#if 1
asm void fn_800E732C(void* entry, void* p1, void* p2, void* p3) {
#include "src/game/gs_material_fn_800E732C.inc"
}
#else
void fn_800E732C(void* entry, void* p1, void* p2, void* p3) {
    /* FULL TEV PIPELINE (0x12BC bytes) */
}
#endif

/* fn_800E85E8 -- TEV post-config | Size: 0x9C */
#if 0
asm void fn_800E85E8(void) {
#include "src/game/gs_material_fn_800E85E8.inc"
}
#else
void fn_800E85E8(void* obj) {
    s32 idx = 0;
    s32 cnt = *(u16*)((u8*)obj + 0x150);
    void** ptr = *(void***)((u8*)obj + 0x14c);
    while (idx < cnt) {
        void* p = *ptr;
        if (p != NULL) fn_800DF608(p);
        idx++;
        ptr++;
    }
    idx = *(u16*)((u8*)obj + 0x154);
    if ((u32)idx != 0) { fn_800E24B0(idx); fn_800E209C(idx); }
    *(u16*)((u8*)obj + 0x154) = 0;
    *(u16*)((u8*)obj + 0x150) = 0;
    *(u32*)((u8*)obj + 0x14c) = 0;
}
#endif

/* fn_800E8684 -- LIGHTING SETUP | Size: 0x878 */
extern void fn_80190E34(void* obj, f32* out);
extern void fn_800CDA74(void);
extern void fn_800C46B0(void);
extern void fn_801B06DC(u32 val);
extern void fn_801B07D4(void);
extern void fn_800E3D14(void* entry, f32* out);
extern void HSD_CObjSetProjectionType(void);
extern void fn_800E00AC(void);
extern void HSD_CObjSetInterest(void);
extern void fn_801B0A98(void);
extern void HSD_CObjSetNear(void);
extern void HSD_CObjSetFar(void);
extern void HSD_CObjGetEyeDistance(void);
extern void fn_801B073C(void);
extern void HSD_CObjGetEyePosition(void);
extern void HSD_CObjGetUpVector(void);
extern void fn_801B0408(void);
extern void fn_800E3B6C(void);
extern void fn_801B019C(void);
extern void fn_801B03A0(void);
extern void fn_801B04E0(void);
extern void fn_801B1524(void);
extern void fn_8019C708(void);
extern void fn_801B16C0(void);
extern void fn_801B0EB8(void);
extern void fn_801B0BD8(void);
extern void fn_8019C6FC(void);
extern u32 lbl_8047AB74;
extern f32 lbl_8047CBC0;
extern u32 lbl_8047CBC4;
extern u32 lbl_8047AB78;
extern f32 lbl_8047CBC8;
extern u32 lbl_8047CBE0;
extern u32 lbl_8047CBCC;
extern u32 lbl_8047CBD0;
extern u32 lbl_8047CBD4;
extern f32 lbl_80478AC0;
extern u32 lbl_8047AB80;
extern u32 lbl_8047AB84;
extern u8  lbl_8047AB94;
extern u8 lbl_80270E98[];
extern u8 lbl_8047CBD8[7];
extern u32 lbl_8047AB90;
extern u32 lbl_8047AB8C;
#if 0
asm void fn_800E8684(void) {
#include "src/game/gs_material_fn_800E8684.inc"
}
#else
void fn_800E8684(void) {
    /* LIGHTING SETUP (0x878 bytes) */
}
#endif

/* fn_800E8EFC -- RenderStateReset | Size: 0x6C */
extern void fn_801B06DC(u32 val);
extern void fn_801B0880();
#if 0
asm void fn_800E8EFC(void) {
#include "src/game/gs_material_fn_800E8EFC.inc"
}
#else
void fn_800E8EFC(void) {
    u8* slot = lbl_80401490;
    u32 i;
    for (i = 0; i < 6; i++) {
        fn_801B06DC(*(u32*)(slot + 0x54));
        fn_801B0880(*(u32*)(slot + 0x54), 0);
        *(u8*)(slot + 0x50) = 0;
        slot += 0x58;
    }
}
#endif

/* GSmaterialSetDistanceThreshold (0x800E8F74) | Size: 0xC */
extern f32 lbl_8047AB88;
#if 0
asm void GSmaterialSetDistanceThreshold(void) {
#include "src/game/gs_material_GSmaterialSetDistanceThreshold.inc"
}
#else
void GSmaterialSetDistanceThreshold(f32 dist) {
    lbl_8047AB88 = dist * dist;
}
#endif

/* fn_800E8F80 | Size: 0x20 */
extern void fn_801B06D4(void);
#if 0
asm void fn_800E8F80(void) {
#include "src/game/gs_material_fn_800E8F80.inc"
}
#else
void fn_800E8F80(void) {
    fn_801B06D4();
}
#endif

/* fn_800E8FA0 | Size: 0x48 */
extern u32 lbl_8047AB90;
extern u32 lbl_8047AB8C;
#if 0
asm void fn_800E8FA0(void) {
#include "src/game/gs_material_fn_800E8FA0.inc"
}
#else
void fn_800E8FA0(s32 w, s32 h) {
    if (w & 1) w++;
    if (h & 1) h++;
    if (w < 2) return;
    if (h < 2) return;
    if (w > 0x280) return;
    if (h > 0x1e0) return;
    lbl_8047AB90 = (u32)w;
    lbl_8047AB8C = (u32)h;
}
#endif

/* fn_800E8FE8 | Size: 0x24 */
#if 0
asm void fn_800E8FE8(void) {
#include "src/game/gs_material_fn_800E8FE8.inc"
}
#else
void fn_800E8FE8(void* p, void* obj) {
    if (obj != NULL) {
        void* inner = *(void**)((u8*)obj + 0xc);
        u16 val = *(u16*)((u8*)inner + 0x8);
        if (!(val & 3)) {
            obj = NULL;
        }
    }
    *(void**)((u8*)p + 0x160) = obj;
}
#endif

/* fn_800E900C -- Distance check | Size: 0xBC */
#if 0
asm void fn_800E900C(void) {
#include "src/game/gs_material_fn_800E900C.inc"
}
#else
void fn_800E900C(void* entry, s32 newCount, void* src) {
    if ((u32)newCount != *(u32*)((u8*)entry + 0x158)) {
        if (*(u16*)((u8*)entry + 0x164) != 0) {
            fn_800E24B0(*(u16*)((u8*)entry + 0x164));
            fn_800E209C(*(u16*)((u8*)entry + 0x164));
            *(u16*)((u8*)entry + 0x164) = 0;
            *(u32*)((u8*)entry + 0x158) = 0;
            *(u32*)((u8*)entry + 0x15c) = 0;
        }
        if (newCount == 0 || src == NULL) return;
        *(u32*)((u8*)entry + 0x158) = newCount;
        *(u16*)((u8*)entry + 0x164) = fn_800E3534(*(u32*)((u8*)entry + 0x158) * 4);
        *(u32*)((u8*)entry + 0x15c) = (u32)fn_800E27B0(*(u16*)((u8*)entry + 0x164));
    }
    memcpy(*(void**)((u8*)entry + 0x15c), src, *(u32*)((u8*)entry + 0x158) * 4);
}
#endif

/* fn_800E90C8 | Size: 0x40 */
#if 0
asm void fn_800E90C8(void) {
#include "src/game/gs_material_fn_800E90C8.inc"
}
#else
void fn_800E90C8(void* p, u32 mask) {
    if (mask & 1) {
        ((GSmaterialEntry*)p)->flags &= ~0x10000000u;
    }
    if (mask & 2) {
        ((GSmaterialEntry*)p)->flags &= ~0x20000000u;
    }
    if (mask & 4) {
        ((GSmaterialEntry*)p)->flags &= ~0x40000000u;
    }
}
#endif

/* fn_800E9108 | Size: 0x40 */
#if 0
asm void fn_800E9108(void) {
#include "src/game/gs_material_fn_800E9108.inc"
}
#else
void fn_800E9108(void* p, u32 mask) {
    if (mask & 1) {
        ((GSmaterialEntry*)p)->flags |= 0x10000000u;
    }
    if (mask & 2) {
        ((GSmaterialEntry*)p)->flags |= 0x20000000u;
    }
    if (mask & 4) {
        ((GSmaterialEntry*)p)->flags |= 0x40000000u;
    }
}
#endif

/* fn_800E9148 -- CheckRenderSlot | Size: 0x140 */
extern void fn_801A3918();
#if 1
asm void fn_800E9148(void* entry, u8 enable) {
#include "src/game/gs_material_fn_800E9148.inc"
}
#else
void fn_800E9148(void* entry, u8 enable) {
    u8* base;
    u8* p;
    u32 found;
    u32 en;
    u32 i;
    found = 0;
    base = (u8*)lbl_80401490;

    if (*(void**)base == entry) {
        found = 1;
    } else {
        p = base + 0x58;
        if (*(void**)p == entry) {
            found = 1;
        } else {
            p += 0x58;
            if (*(void**)p == entry) {
                found = 1;
            } else {
                p += 0x58;
                if (*(void**)p == entry) {
                    found = 1;
                } else {
                    p += 0x58;
                    if (*(void**)p == entry) {
                        found = 1;
                    } else if (*(void**)(p + 0x58) == entry) {
                        found = 1;
                    }
                }
            }
        }
    }

    if ((u8)found) {
        void* mobj = fn_800E5188(entry);
        if ((u8)enable == 0) {
            fn_801A3918(mobj, fn_800E9358, 0);
        } else {
            fn_801A3918(mobj, fn_800E9358, 1);
        }
        en = (u8)enable;
        i = 0;
        do {
            if (*(u8*)(base + 0x50) != 0 && *(void**)base == entry) {
                fn_801B0880(*(void**)(base + 0x54), en);
            }
            i++;
            base += 0x58;
        } while (i < 6);
    }
}
#endif

/* fn_800E9288 | Size: 0x50 */
#if 0
asm void fn_800E9288(void) {
#include "src/game/gs_material_fn_800E9288.inc"
}
#else
void fn_800E9288(GSmaterialEntry* entry) {
    u16 handle = *(u16*)((u8*)entry + 0x164);
    if (handle != 0) {
        fn_800E24B0(handle);
        handle = *(u16*)((u8*)entry + 0x164);
        fn_800E209C(handle);
        *(u16*)((u8*)entry + 0x164) = 0;
        *(u32*)((u8*)entry + 0x158) = 0;
        *(u32*)((u8*)entry + 0x15c) = 0;
    }
}
#endif

/* fn_800E92D8 | Size: 0x80 */
extern u32 fn_801B1730(void);
extern f32 lbl_8047CBC8;
extern u8  lbl_8047AB94;
extern u32 lbl_8047AB90;
extern u32 lbl_8047AB8C;
extern f32 lbl_8047AB88;
extern u32 lbl_8047AB84;
#if 0
asm void fn_800E92D8(void) {
#include "src/game/gs_material_fn_800E92D8.inc"
}
#else
void fn_800E92D8(void) {
    u8* slot = lbl_80401490;
    s32 i;
    lbl_8047AB94 = 0x80;
    lbl_8047AB90 = 0x180;
    lbl_8047AB8C = 0x180;
    lbl_8047AB88 = lbl_8047CBC8;
    lbl_8047AB84 = 0;
    for (i = 0; i < 6; i++) {
        u32 val = fn_801B1730();
        *(u32*)(slot + 0x54) = val;
        fn_801B0880(*(u32*)(slot + 0x54), 0);
        slot += 0x58;
    }
}
#endif

/* fn_800E9358 | Size: 0x60 */
#if 0
asm void fn_800E9358(void) {
#include "src/game/gs_material_fn_800E9358.inc"
}
#else
void fn_800E9358(void* entry, u8 enable) {
    void* node;
    if (*(u32*)((u8*)entry + 0x14) & 0x4020) {
        return;
    }
    node = *(void**)((u8*)entry + 0x18);
    while (node != NULL) {
        void* child = *(void**)((u8*)node + 0x8);
        if (child != NULL) {
            u32* flags = (u32*)((u8*)child + 0x4);
            if (enable) {
                *flags |= 0x04000000;
            } else {
                *flags &= ~0x04000000;
            }
        }
        node = *(void**)((u8*)node + 0x4);
    }
}
#endif

/* fn_800E93B8 -- SetupEnvMap | Size: 0x5E0 */
extern f32 lbl_8047CBC0;
extern double ceil(double);
#if 1
asm void fn_800E93B8(void) {
#include "src/game/gs_material_fn_800E93B8.inc"
}
#else
void fn_800E93B8(void) {
    /* SetupEnvMap (0x5E0 bytes) */
}
#endif

/* fn_800E9998 -- UpdateEnvMap | Size: 0x194 */
extern void fn_800E0020(f32* a, f32* b);
extern f32 lbl_8047CBC0;
extern f32 lbl_8047AB88;
extern f32 lbl_8047CBC8;
#if 0
asm void fn_800E9998(void) {
#include "src/game/gs_material_fn_800E9998.inc"
}
#else
void* fn_800E9998(void* r26, void* r27, void* r28, void* r29) {
    extern u8 lbl_80401490[];
    void* r31 = (void*)lbl_80401490;
    s32 r30 = 0;
    while (r30 < 6) {
        if (*(void**)r31 == r26 && *(void**)((u8*)r31 + 0x4) == r28) {
            s32 r3_lod;
            u8 r24 = 1;
            if (r29 != NULL) {
                f32 stk[3];
                f32 f1;
                fn_80190E34(r29, stk);
                f1 = stk[0];
                if (stk[1] > f1) f1 = stk[1];
                if (stk[2] > f1) f1 = stk[2];
                {
                    f32* r3c = *(f32**)((u8*)r29 + 0xc);
                    f32 f2 = r3c[0] + r3c[1] + r3c[2];
                    f32 f0 = f2 / lbl_8047CBC0;
                    f1 = f1 * f0;
                }
                r3_lod = ((s32(*)(f64))fn_800CDA74)((f64)f1);
                r3_lod = ((s32(*)(s32))fn_800C46B0)(r3_lod);
            } else {
                r3_lod = 0;
                goto do_anim;
            }
            {
                u32 r48 = *(u32*)((u8*)r31 + 0x48);
                u32 r4c = *(u32*)((u8*)r31 + 0x4c);
                if ((u32)r3_lod < r48) {
                    /* divide r4c by 3 */
                    u32 div3 = (u32)(((u64)0xAAAAAAABu * (u64)r4c) >> 33);
                    if ((u32)r3_lod < div3) { r30++; r31 = (u8*)r31 + 0x58; continue; }
                } else {
                    u32 x3 = r48 * 3;
                    if ((u32)r3_lod > x3) { r30++; r31 = (u8*)r31 + 0x58; continue; }
                }
            }
            do_anim:
            if (lbl_8047AB88 > lbl_8047CBC8) {
                f32 stk20[3];
                fn_800E3D14(r27, stk20);
                r24 = 1;
                {
                    s32 r25 = 0;
                    while (r25 < 0x10) {
                        void* entry8 = *(void**)((u8*)r31 + 0x8 + r25 * 4);
                        if (entry8 != NULL) {
                            f32 stk14[3];
                            f32 f1;
                            fn_800E3D14(entry8, stk14);
                            fn_800E0020(stk20, stk14);
                            f1 = lbl_8047AB88;
                            if (f1 > lbl_8047CBC8) {
                                r24 = 0;
                                r25 = 0x10;
                            }
                        }
                        r25++;
                    }
                }
                if (!r24) { r30++; r31 = (u8*)r31 + 0x58; continue; }
            }
            return r31;
        }
        r30++;
        r31 = (u8*)r31 + 0x58;
    }
    return NULL;
}
#endif

/* fn_800E9B2C | Size: 0x140 */
#if 0
asm void fn_800E9B2C(void) {
#include "src/game/gs_material_fn_800E9B2C.inc"
}
#else
void fn_800E9B2C(void* entry, void* param) {
    u32 r0 = *(u32*)entry & 0xFFCDD49Du;
    *(u32*)entry = r0;
    *(u32*)entry = *(u32*)entry | *(u32*)param;
    fn_800E43A4(entry, (u8*)param + 0x4);
    fn_800E4170(entry, (f32*)((u8*)param + 0x10));
    fn_800E407C(entry, (u8*)param + 0x1c);
    if (*(u32*)param & 0x20000) {
        *(u32*)entry ^= 0x20000;
        fn_800E732C(entry, (u8*)param + 0x48, (u8*)param + 0x54, (u8*)param + 0x60);
    }
    *(s32*)((u8*)entry + 0x90) = -1;
    *(s32*)((u8*)entry + 0xa8) = -1;
    fn_800ECCA8(entry, *(u32*)((u8*)param + 0x28));
    fn_800ECA78(entry, *(f32*)((u8*)param + 0x30));
    fn_800EC9DC(entry, *(f32*)((u8*)param + 0x34));
    fn_800ECB74(entry, *(u32*)((u8*)param + 0x40));
    fn_800EC35C(entry, *(u32*)((u8*)param + 0x2c));
    fn_800EC2A4(entry, *(f32*)((u8*)param + 0x38));
    fn_800EC308(entry, *(f32*)((u8*)param + 0x3c));
    fn_800EC208(entry, *(s32*)((u8*)param + 0x44));
    if (*(u32*)param & 0x4000000) {
        fn_800EC990(entry);
    }
    if (*(u32*)param & 0x2000000) {
        fn_800EC1E4(entry);
    }
    fn_800ED1CC(entry);
}
#endif

/* fn_800E9C6C | Size: 0x1C8 */
extern void* fn_800E3D00(void* entry);
extern void* fn_800E3CF8(void* entry);
extern void* fn_800E3CF0(void* entry);
extern u8 lbl_8047CBE8[7];
extern u8 lbl_8047CBF0[5];
extern u8 lbl_80270EA8[];
extern u8 lbl_8047CBF8[6];
#if 0
asm void fn_800E9C6C(void) {
#include "src/game/gs_material_fn_800E9C6C.inc"
}
#else
void fn_800E9C6C(void* entry, void* dest) {
    u32 r6 = *(u32*)entry;
    *(u32*)dest = r6 & 0x322B62u;
    fn_800E01D0((u8*)dest + 0x4, fn_800E3D00(entry));
    fn_800E01D0((u8*)dest + 0x10, fn_800E3CF8(entry));
    fn_800E01D0((u8*)dest + 0x1c, fn_800E3CF0(entry));
    *(u32*)((u8*)dest + 0x28) = *(u32*)((u8*)entry + 0x90);
    *(u32*)((u8*)dest + 0x2c) = *(u32*)((u8*)entry + 0xa8);
    *(f32*)((u8*)dest + 0x30) = *(f32*)((u8*)entry + 0x98);
    *(f32*)((u8*)dest + 0x34) = *(f32*)((u8*)entry + 0x94);
    *(u32*)((u8*)dest + 0x40) = *(u32*)((u8*)entry + 0x8c);
    *(f32*)((u8*)dest + 0x38) = *(f32*)((u8*)entry + 0xb0);
    *(f32*)((u8*)dest + 0x3c) = *(f32*)((u8*)entry + 0xac);
    *(u32*)((u8*)dest + 0x44) = *(u32*)((u8*)entry + 0xa4);
    if (*(u32*)dest & 0x20000) {
        void* r3 = *(void**)((u8*)entry + 0x8);
        void* r30 = *(void**)((u8*)r3 + 0x10);
        if (r30 == NULL) __assert(lbl_8047CBE8, 0x3e4, lbl_8047CBF0);
        if ((u8*)dest + 0x48 == NULL) __assert(lbl_8047CBE8, 0x3e5, lbl_80270EA8);
        *(u32*)((u8*)dest + 0x48) = *(u32*)((u8*)r30 + 0x38);
        if (r30 == NULL) __assert(lbl_8047CBE8, 0x2ec, lbl_8047CBF0);
        *(u32*)((u8*)dest + 0x4c) = *(u32*)((u8*)r30 + 0x3c);
        *(u32*)((u8*)dest + 0x50) = *(u32*)((u8*)r30 + 0x40);
        if (r30 == NULL) __assert(lbl_8047CBE8, 0x2fa, lbl_8047CBF0);
        *(f32*)((u8*)dest + 0x54) = *(f32*)((u8*)r30 + 0x1c);
        if (r30 == NULL) __assert(lbl_8047CBE8, 0x308, lbl_8047CBF0);
        *(f32*)((u8*)dest + 0x58) = *(f32*)((u8*)r30 + 0x20);
        if (r30 == NULL) __assert(lbl_8047CBE8, 0x351, lbl_8047CBF0);
        *(f32*)((u8*)dest + 0x5c) = *(f32*)((u8*)r30 + 0x24);
        if ((u8*)dest + 0x60 == NULL) __assert(lbl_8047CBE8, 0x352, lbl_8047CBF8);
        *(u32*)((u8*)dest + 0x60) = *(u32*)((u8*)r30 + 0x2c);
        *(u32*)((u8*)dest + 0x64) = *(u32*)((u8*)r30 + 0x30);
        *(u32*)((u8*)dest + 0x68) = *(u32*)((u8*)r30 + 0x34);
    }
}
#endif

/* fn_800E9E34 | Size: 0x5C */
#if 0
asm void fn_800E9E34(void) {
#include "src/game/gs_material_fn_800E9E34.inc"
}
#else
void fn_800E9E34(GSmaterialEntry* entry, void* a, void* b, void* c) {
    extern void fn_800E9E90(void*, u32, u32, void*, void*, void*);
    void* mobj = fn_800E5188(entry);
    fn_800E9E90(mobj, 0, 7, a, b, c);
}
#endif

/* fn_800E9E90 -- EnvMap pipeline | Size: 0x77C */
extern void fn_800A2EB4(u8* dst, void* src);
extern void PSMTXConcat(void* a, void* b, void* c);
extern void fn_800E064C(u8* data);
extern void fn_80197B6C(void*, void*, void*);
extern void fn_8019F024();
extern void fn_801AB63C(s32, s32);
extern void HSD_DObjSetCurrent();
extern u32 lbl_8047CC00;
extern u32 lbl_8047CC08;
extern u8 lbl_804016A0[];
#if 1
asm void fn_800E9E90(void) {
#include "src/game/gs_material_fn_800E9E90.inc"
}
#else
void fn_800E9E90(void) {
    /* EnvMap pipeline (0x77C bytes) */
}
#endif

/* HSD_JObjMtxIsDirty | Size: 0x58 */
#if 0
asm void HSD_JObjMtxIsDirty(void) {
#include "src/game/gs_material_HSD_JObjMtxIsDirty.inc"
}
#else
s32 HSD_JObjMtxIsDirty(void* obj) {
    u32 flags;
    if (obj == NULL) {
        __assert(&lbl_8047CC00, 0x25d, &lbl_8047CC08);
    }
    flags = *(u32*)((u8*)obj + 0x14);
    if (flags & 0x800000) {
        return 0;
    }
    if (flags & 0x40) {
        return 1;
    }
    return 0;
}
#endif

/* fn_800EA664 | Size: 0x70 */
#if 0
asm void fn_800EA664(void) {
#include "src/game/gs_material_fn_800EA664.inc"
}
#else
void fn_800EA664(void* obj) {
    if (obj == NULL) {
        __assert(&lbl_8047CC00, 0x25d, &lbl_8047CC08);
    }
    {
        u32 flags = *(u32*)((u8*)obj + 0x14);
        s32 r3 = 0;
        if (!(flags & 0x800000)) {
            if (flags & 0x40) {
                r3 = 1;
            }
        }
        if (r3 != 0) {
            fn_8019D9DC(obj);
        }
    }
}
#endif

/* fn_800EA6D4 -- BindTextureToStage | Size: 0x110 */
#if 0
asm void fn_800EA6D4(void) {
#include "src/game/gs_material_fn_800EA6D4.inc"
}
#else
void fn_800EA6D4(void* entry, void* tex, void* r5) {
    void* r29 = tex;
    if (r29 != NULL) {
        u32 flags = *(u32*)((u8*)r29 + 0x14);
        s32 r3 = 0;
        if (!(flags & 0x800000)) {
            if (flags & 0x40) { r3 = 1; }
        }
        if (r3 != 0) fn_8019D9DC(r29);
    }
    {
        void* r31 = *(void**)((u8*)r29 + 0x10);
        if (r31 != NULL) {
            u32 flags = *(u32*)((u8*)r31 + 0x14);
            s32 r3 = 0;
            if (!(flags & 0x800000)) {
                if (flags & 0x40) { r3 = 1; }
            }
            if (r3 != 0) fn_8019D9DC(r31);
        }
        fn_800A2EB4((u8*)*(void**)((u8*)r29 + 0x10) + 0x44, r5);
        PSMTXConcat((u8*)r29 + 0x44, r5, r5);
        if (entry != NULL) {
            PSMTXConcat(entry, r5, r5);
        }
    }
}
#endif

/* fn_800EA7E4 | Size: 0x3C */
#if 0
asm void fn_800EA7E4(void) {
#include "src/game/gs_material_fn_800EA7E4.inc"
}
#else
void fn_800EA7E4(void* obj) {
    if (obj != NULL && !(*(u32*)((u8*)obj + 0x14) & 0x4020)) {
        ((void(*)(void*))fn_800EA820)(obj);
    }
}
#endif

/* fn_800EA820 | Size: 0x140 */
#if 0
asm void fn_800EA820(void) {
#include "src/game/gs_material_fn_800EA820.inc"
}
#else
void fn_800EA820(void* entry, void* tex, u32 mask_shift, void* a4, void* a5, void* a6) {
    u8 buf[48]; /* sp+8 */
    u32 flags = *(u32*)((u8*)entry + 0x14);
    if (!(flags & 0x10)) {
        u32 r27 = flags & (mask_shift << 18);
        if (r27 != 0) {
            if (entry == NULL) goto _end;
            if (entry != NULL) goto _skip_null_err;
            __assert(&lbl_8047CC00, 0x25d, &lbl_8047CC08);
            _skip_null_err:
            {
                s32 r3 = 0;
                u32 f2 = *(u32*)((u8*)entry + 0x14);
                if (!(f2 & 0x800000)) {
                    if (f2 & 0x40) { r3 = 1; }
                }
                if (r3 != 0) fn_8019D9DC(entry);
            }
            _end:
            if (tex == NULL) {
                fn_800E064C(lbl_804016A0);
                tex = lbl_804016A0;
            }
            fn_80197B6C(entry, tex, buf);
            if (r27 & 0x40000) {
                fn_800EA960(entry, tex, buf, 1, a4, a5, a6);
            }
            if (r27 & 0x100000) {
                fn_800EA960(entry, tex, buf, 4, a4, a5, a6);
            }
            if (r27 & 0x80000) {
                fn_800EA960(entry, tex, buf, 2, a4, a5, a6);
            }
        }
    }
}
#endif

/* fn_800EA960 -- ConfigureTEVStage | Size: 0x370 */
extern void* fn_8019F01C(void*);
extern void fn_801AB538(s32, void*, void*);
extern void fn_801AB5F8(s32, void*, s32);
extern void fn_800E0628(void*, void*);
extern u8 lbl_804016D0[];
#if 1
asm void fn_800EA960(void* obj, void* mtx, void* dst, s32 typeFlag, void* doBlend, void* callback, void* cbArg) {
#include "src/game/gs_material_fn_800EA960.inc"
}
#else
void fn_800EA960(void* obj, void* mtx, void* dst, s32 typeFlag, void* doBlend, void* callback, void* cbArg) {
    void* outerNode;
    void* node;
    void* texObj;
    void* retVal;
    u8* base;
    u8* baseOfs;
    u32 typeMask;
    u32 stk0[2];
    u32 stk1[2];
    u32 stk2[12];
    u8 changed0;
    u8 changed1;

    fn_8019F024();
    typeMask = (u32)typeFlag << 1;
    fn_801AB63C(0, 0);
    base = (u8*)lbl_804016D0;
    outerNode = *(void**)((u8*)obj + 0x18);
    baseOfs = base + 0x30;

    while (outerNode != NULL) {
        u32 outerFlags = *(u32*)((u8*)outerNode + 0x14);
        if (!(outerFlags & 1) && (outerFlags & typeMask)) {
            HSD_DObjSetCurrent(outerNode);
            node = *(void**)((u8*)outerNode + 0xc);

            while (node != NULL) {
                u16 flags = *(u16*)((u8*)node + 0xc);
                if (!(flags & 0x800)) {
                    if ((u8)doBlend != 0) {
                        u32 type = flags & 0x3000;
                        if (type == 0x0000) {
                            if (*(void**)((u8*)node + 0x14) == NULL) {
                                retVal = fn_8019F01C(node);
                                fn_801AB538(0, (void*)&stk0[0], (void*)&stk0[1]);
                                if (stk0[0] != (u32)retVal || stk0[1] != 1) {
                                    fn_801AB5F8(0, retVal, 1);
                                    fn_800E0628(base, dst);
                                }
                            } else {
                                changed0 = 0;
                                changed1 = 0;
                                retVal = fn_8019F01C(node);
                                texObj = retVal;
                                fn_801AB538(0, (void*)&stk0[0], (void*)&stk0[1]);
                                if (stk0[0] != (u32)texObj || stk0[1] != 1) {
                                    changed0 = 1;
                                }
                                fn_801AB5F8(0, texObj, 1);
                                fn_801AB538(1, (void*)&stk0[0], (void*)&stk0[1]);
                                if (stk0[0] == (u32)*(void**)((u8*)node + 0x14) && stk0[1] != 1) {
                                    changed1 = 1;
                                }
                                fn_801AB5F8(1, (void*)1, 1);
                                if ((u8)changed0 == 0 && (u8)changed1 == 0) {
                                    goto post_dispatch;
                                }
                                if ((u8)changed0 != 0) {
                                    fn_800E0628(base, dst);
                                }
                                if ((u8)changed1 != 0) {
                                    texObj = *(void**)((u8*)node + 0x14);
                                    if (texObj != NULL) {
                                        u32 fl;
                                        s32 check;
                                        fl = *(u32*)((u8*)texObj + 0x14);
                                        check = 0;
                                        if (!(fl & 0x800000) && (fl & 0x40)) {
                                            check = 1;
                                        }
                                        if (check) {
                                            fn_8019D9DC(texObj);
                                        }
                                    }
                                    PSMTXConcat(mtx, (u8*)*(void**)((u8*)node + 0x14) + 0x44, (void*)stk2);
                                    fn_800E0628(baseOfs, (void*)stk2);
                                }
                            }
                        } else if (type == 0x1000) {
                            retVal = fn_8019F01C(node);
                            fn_801AB538(0, (void*)&stk0[0], (void*)&stk0[1]);
                            if (stk0[0] != (u32)retVal || stk0[1] != 1) {
                                fn_801AB5F8(0, retVal, 1);
                                fn_800E0628(base, dst);
                            }
                        } else if (type == 0x2000) {
                            fn_800EAFE4(node, mtx, dst, base);
                        }
                    }
                }
                post_dispatch:
                {
                    u32 type2;
                    type2 = *(u16*)((u8*)node + 0xc) & 0x3000;
                    if (type2 == 0x1000) {
                        ((void(*)(void*, void*, void*, void*, void*))callback)(node, mtx, dst, (void*)0, cbArg);
                    } else if (callback != NULL) {
                        if ((u8)doBlend != 0) {
                            ((void(*)(void*, void*, void*, void*, void*))callback)(node, mtx, dst, base, cbArg);
                        } else {
                            ((void(*)(void*, void*, void*, void*, void*))callback)(node, mtx, dst, (void*)0, cbArg);
                        }
                    }
                }
                node = *(void**)((u8*)node + 0x4);
            }
        }
        outerNode = *(void**)((u8*)outerNode + 0x4);
    }
    HSD_DObjSetCurrent(0);
    fn_8019F024(0);
}
#endif

/* fn_800EACD0 -- ConfigureBlend | Size: 0x314 */
#if 1
asm void fn_800EACD0(void* r3, void* r4, void* r5, u8 r6, void* r7, void* r8) {
#include "src/game/gs_material_fn_800EACD0.inc"
}
#else
void fn_800EACD0(void* obj, void* mtx, void* dst, u8 doBlend, void* callback, void* cbArg) {
    void* node;
    u8* base;
    u8* baseOfs;
    void* texObj;
    void* retVal;
    u32 stk0[2];
    u32 stk1[2];
    u32 stk2[12];
    u8 changed0;
    u8 changed1;

    node = *(void**)((u8*)obj + 0xc);
    base = (u8*)lbl_804016D0;
    baseOfs = base + 0x30;

    while (node != NULL) {
        u16 flags = *(u16*)((u8*)node + 0xc);
        if (!(flags & 0x800)) {
            if ((u8)doBlend != 0) {
                u32 type = flags & 0x3000;
                if (type == 0x0000) {
                    if (*(void**)((u8*)node + 0x14) == NULL) {
                        retVal = fn_8019F01C(node);
                        fn_801AB538(0, (void*)&stk0[0], (void*)&stk0[1]);
                        if (stk0[0] != (u32)retVal || stk0[1] != 1) {
                            fn_801AB5F8(0, retVal, 1);
                            fn_800E0628(base, dst);
                        }
                    } else {
                        changed0 = 0;
                        changed1 = 0;
                        retVal = fn_8019F01C(node);
                        texObj = retVal;
                        fn_801AB538(0, (void*)&stk0[0], (void*)&stk0[1]);
                        if (stk0[0] != (u32)texObj || stk0[1] != 1) {
                            changed0 = 1;
                        }
                        fn_801AB5F8(0, texObj, 1);
                        fn_801AB538(1, (void*)&stk0[0], (void*)&stk0[1]);
                        if (stk0[0] == (u32)*(void**)((u8*)node + 0x14) && stk0[1] != 1) {
                            changed1 = 1;
                        }
                        fn_801AB5F8(1, (void*)1, 1);
                        if ((u8)changed0 == 0 && (u8)changed1 == 0) {
                            goto post_dispatch;
                        }
                        if ((u8)changed0 != 0) {
                            fn_800E0628(base, dst);
                        }
                        if ((u8)changed1 != 0) {
                            texObj = *(void**)((u8*)node + 0x14);
                            if (texObj != NULL) {
                                u32 fl;
                                s32 check;
                                fl = *(u32*)((u8*)texObj + 0x14);
                                check = 0;
                                if (!(fl & 0x800000) && (fl & 0x40)) {
                                    check = 1;
                                }
                                if (check) {
                                    fn_8019D9DC(texObj);
                                }
                            }
                            PSMTXConcat(mtx, (u8*)*(void**)((u8*)node + 0x14) + 0x44, (void*)stk2);
                            fn_800E0628(baseOfs, (void*)stk2);
                        }
                    }
                } else if (type == 0x1000) {
                    retVal = fn_8019F01C(node);
                    fn_801AB538(0, (void*)&stk0[0], (void*)&stk0[1]);
                    if (stk0[0] != (u32)retVal || stk0[1] != 1) {
                        fn_801AB5F8(0, retVal, 1);
                        fn_800E0628(base, dst);
                    }
                } else if (type == 0x2000) {
                    fn_800EAFE4(node, mtx, dst, base);
                }
            }
        }
        post_dispatch:
        {
            u32 type2;
            type2 = *(u16*)((u8*)node + 0xc) & 0x3000;
            if (type2 == 0x1000) {
                ((void(*)(void*, void*, void*, void*, void*))callback)(node, mtx, dst, (void*)0, cbArg);
            } else if (callback != NULL) {
                if ((u8)doBlend != 0) {
                    ((void(*)(void*, void*, void*, void*, void*))callback)(node, mtx, dst, base, cbArg);
                } else {
                    ((void(*)(void*, void*, void*, void*, void*))callback)(node, mtx, dst, (void*)0, cbArg);
                }
            }
        }
        node = *(void**)((u8*)node + 0x4);
    }
}
#endif

/* fn_800EAFE4 -- ConfigureZMode | Size: 0x284 */
extern void* fn_80197A64(void*, void*);
extern void HSD_MtxScaledAdd(void*, void*, void*, f32);
extern u8 lbl_80270EB8[];
extern u32 lbl_8047CC18;
extern u32 lbl_8047CC10;
extern u32 lbl_8047CC1C;
extern u32 lbl_8047CC20;
#if 0
asm void fn_800EAFE4(void* obj, void* dst, void* unused, void* output) {
#include "src/game/gs_material_fn_800EAFE4.inc"
}
#else
void fn_800EAFE4(void* obj, void* dst, void* unused, void* output) {
    f32 mtxStack[12];
    f32 mtxBuf[12];
    f32 mtxTmp[12];
    u8* assertStr;
    u8* outPtr;
    void* node;
    s32 i;
    void* texObj;
    f32 threshold;

    assertStr = lbl_80270EB8;
    texObj = fn_8019F01C(obj);
    fn_801AB63C(0, 2);
    texObj = fn_80197A64(texObj, (void*)mtxStack);
    node = *(void**)((u8*)obj + 0x14);
    obj = texObj;
    threshold = *(f32*)&lbl_8047CC18;
    outPtr = (u8*)output;
    i = 0;
    while (i < 10 && node != NULL) {
        void* nodeData = *(void**)((u8*)node + 0x4);
        if (nodeData == NULL) {
            __assert(&lbl_8047CC10, 0x65, assertStr);
        }
        if (*(f32*)((u8*)nodeData + 0x8) >= threshold) {
            texObj = *(void**)((u8*)nodeData + 0x4);
            if (texObj != NULL) {
                s32 check;
                u32 flags;
                if (texObj == NULL) {
                    __assert(&lbl_8047CC00, 0x25d, &lbl_8047CC08);
                }
                flags = *(u32*)((u8*)texObj + 0x14);
                check = 0;
                if (!(flags & 0x800000) && (flags & 0x40)) {
                    check = 1;
                }
                if (check) {
                    fn_8019D9DC(texObj);
                }
            }
            if (obj != NULL) {
                void* tex = *(void**)((u8*)nodeData + 0x4);
                PSMTXConcat((u8*)tex + 0x44, *(void**)((u8*)tex + 0x78), (void*)mtxBuf);
                texObj = (void*)mtxBuf;
            } else {
                void* tex = *(void**)((u8*)nodeData + 0x4);
                texObj = (void*)((u8*)tex + 0x44);
            }
        } else {
            f32 zero;
            zero = *(f32*)&lbl_8047CC1C;
            mtxBuf[11] = zero;
            mtxBuf[10] = zero;
            mtxBuf[9] = zero;
            mtxBuf[8] = zero;
            mtxBuf[7] = zero;
            mtxBuf[6] = zero;
            mtxBuf[5] = zero;
            mtxBuf[4] = zero;
            mtxBuf[3] = zero;
            mtxBuf[2] = zero;
            mtxBuf[1] = zero;
            mtxBuf[0] = zero;
            while (nodeData != NULL) {
                if (*(void**)((u8*)nodeData + 0x4) == NULL) {
                    __assert(&lbl_8047CC10, 0x7e, assertStr + 0xc);
                }
                texObj = *(void**)((u8*)nodeData + 0x4);
                if (texObj != NULL) {
                    s32 check2;
                    u32 flags2;
                    if (texObj == NULL) {
                        __assert(&lbl_8047CC00, 0x25d, &lbl_8047CC08);
                    }
                    flags2 = *(u32*)((u8*)texObj + 0x14);
                    check2 = 0;
                    if (!(flags2 & 0x800000) && (flags2 & 0x40)) {
                        check2 = 1;
                    }
                    if (check2) {
                        fn_8019D9DC(texObj);
                    }
                }
                if ((u32)texObj + 0x44 == 0) {
                    __assert(&lbl_8047CC10, 0x81, &lbl_8047CC20);
                }
                if (*(void**)((u8*)texObj + 0x78) == NULL) {
                    __assert(&lbl_8047CC10, 0x82, assertStr + 0x1c);
                }
                PSMTXConcat((u8*)texObj + 0x44, *(void**)((u8*)texObj + 0x78), (void*)mtxTmp);
                HSD_MtxScaledAdd((void*)mtxTmp, (void*)mtxBuf, (void*)mtxBuf, *(f32*)((u8*)nodeData + 0x8));
                nodeData = *(void**)nodeData;
            }
            texObj = (void*)mtxBuf;
        }
        if (obj != NULL) {
            PSMTXConcat(texObj, obj, (void*)mtxBuf);
        }
        PSMTXConcat(dst, texObj, (void*)mtxTmp);
        fn_800E0628(outPtr, (void*)mtxTmp);
        node = *(void**)node;
        outPtr += 0x30;
        i++;
    }
}
#endif

/* fn_800EB268 -- ConfigureAlpha | Size: 0xD8 */
extern void fn_80191460(u8* mtx);
extern f32 lbl_8047CC28;
extern u8 lbl_8047CC2C[7];
extern u8 lbl_8047CC34[5];
#if 0
asm void fn_800EB268(void) {
#include "src/game/gs_material_fn_800EB268.inc"
}
#else
void fn_800EB268(void* entry, s32 r4) {
    u32 r31;
    void* r30;
    fn_800ECCA8(entry, r4);
    fn_80191460((u8*)entry + 0x4c);
    r31 = *(u32*)((u8*)entry + 0x144);
    *(u32*)((u8*)entry + 0x144) = 0;
    if (*(u32*)entry & 0x4) {
        fn_800ECA78(entry, lbl_8047CC28);
        fn_800EC134(entry);
        r30 = *(void**)((u8*)entry + 0x8);
        if (r30 != NULL) {
            s32 active;
            if (r30 == NULL) {
                __assert(lbl_8047CC2C, 0x25d, lbl_8047CC34);
            }
            active = 0;
            {
                u32 flags = *(u32*)((u8*)r30 + 0x14);
                if (!(flags & 0x800000)) {
                    if (flags & 0x40) {
                        active = 1;
                    }
                }
            }
            if (active) fn_8019D9DC(r30);
        }
    }
    fn_800E9E34(entry, (void*)1, fn_800EB414, entry);
    *(u32*)((u8*)entry + 0x144) = r31;
}
#endif

/* fn_800EB340 | Size: 0xD4 */
extern f32 lbl_8047CC28;
#if 0
asm void fn_800EB340(void) {
#include "src/game/gs_material_fn_800EB340.inc"
}
#else
void fn_800EB340(void* entry) {
    fn_80191460((u8*)entry + 0x4c);
    {
        void* r30 = *(void**)((u8*)entry + 0x144);
        *(u32*)((u8*)entry + 0x144) = 0;
        if (*(u32*)entry & 0x4) {
            fn_800ECA78(entry, lbl_8047CC28);
            fn_800EC134(entry);
            {
                void* r31 = *(void**)((u8*)entry + 0x8);
                if (r31 != NULL) {
                    u32 flags = *(u32*)((u8*)r31 + 0x14);
                    s32 r3 = 0;
                    if (!(flags & 0x800000)) {
                        if (flags & 0x40) {
                            r3 = 1;
                        }
                    }
                    if (r3 != 0) {
                        fn_8019D9DC(r31);
                    }
                }
            }
        }
        fn_800E9E34(entry, (void*)1, fn_800EB414, entry);
        *(void**)((u8*)entry + 0x144) = r30;
    }
}
#endif

/* fn_800EB414 | Size: 0x50 */
extern void fn_800DA578(void);
extern u8 lbl_80315598[];
#if 0
asm void fn_800EB414(void) {
#include "src/game/gs_material_fn_800EB414.inc"
}
#else
void fn_800EB414(void* p, void* a2, void* a3, void* a4, void* a5, void* a6) {
    void* args[3];
    args[0] = a6;
    args[1] = a3;
    args[2] = a4;
    ((void(*)(void*, void*, u32, u8*, void*, void*))fn_800DA578)(
        *(void**)((u8*)p + 0x8),
        *(void**)((u8*)p + 0x10),
        (u32)(*(u16*)((u8*)p + 0xe)) << 5,
        lbl_80315598,
        args,
        p
    );
}
#endif

/* fn_800EB464 | Size: 0xBC */
extern void fn_800DFF98(void*, void*, void*);
extern void* fn_800E3C5C(void* entry);
extern void fn_80191358(f32 x, f32 y, f32 z);
extern u32 lbl_8047AB98;
#if 0
asm void fn_800EB464(void) {
#include "src/game/gs_material_fn_800EB464.inc"
}
#else
void fn_800EB464(s32 flags, f32* arr, void* obj) {
    if (flags & 1) {
        u8 n = *(u8*)arr;
        lbl_8047AB98 = (s32)(((s64)0x55555556 * n) >> 32) + (u32)(((s32)(((s64)0x55555556 * n) >> 32)) >> 31);
    }
    if (flags & 2) {
        void* r31;
        u8 vec[12];
        if (*(u32*)((u8*)obj + 0xc) & 1) {
            r31 = (u8*)*(void**)((u8*)obj + 0x8) + lbl_8047AB98 * 0x30;
        } else {
            r31 = *(void**)((u8*)obj + 0x4);
        }
        fn_800E01F4(vec, arr[0], arr[1], arr[2]);
        fn_800DFF98(vec, r31, vec);
        fn_800E3C5C(*(void**)obj);
        fn_80191358(*(f32*)(vec+0), *(f32*)(vec+4), *(f32*)(vec+8));
    }
}
#endif

/* fn_800EB520 | Size: 0x8 */
#if 0
asm void fn_800EB520(void) {
#include "src/game/gs_material_fn_800EB520.inc"
}
#else
void fn_800EB520(void* a, void* b, u32 c, void* d) {
    *(u32*)((u8*)d + 0xc) = c;
}
#endif

/* fn_800EB528 | Size: 0x78 */
#if 0
asm void fn_800EB528(void) {
#include "src/game/gs_material_fn_800EB528.inc"
}
#else
void fn_800EB528(void* entry) {
    void* src;
    if (*(void**)((u8*)entry + 0xc) != NULL) {
        return;
    }
    if (*(u32*)entry & 0x20000) {
        return;
    }
    src = *(void**)*(void**)((u8*)entry + 0x4);
    *(u32*)((u8*)entry + 0xc) = (u32)((void*(*)(void*))fn_801A0FBC)(src);
    src = *(void**)*(void**)((u8*)entry + 0x4);
    *(u32*)((u8*)entry + 0x10) = (u32)((void*(*)(void*))fn_801A0FBC)(src);
    src = *(void**)*(void**)((u8*)entry + 0x4);
    *(u32*)((u8*)entry + 0x14) = (u32)((void*(*)(void*))fn_801A0FBC)(src);
    ((void(*)(void*))_modelSetRotateEulerToQuatAll__FP9_HSD_JObj)(*(void**)((u8*)entry + 0xc));
}
#endif

/* fn_800EB5A0 | Size: 0x140 */
extern void fn_800E0560(void* dst, u8* src);
extern void fn_800E053C(void);
extern void fn_800E0518(void);
extern void fn_800E04F4(void);
extern void fn_800E042C(void* dst, u8* src);
extern void fn_800E0290(void* dst, void* src, void* mtx);
extern u32 lbl_8047ABA0;
extern u8 lbl_8047CC40[7];
extern u8 lbl_8047CC48[5];
#if 0
asm void fn_800EB5A0(void) {
#include "src/game/gs_material_fn_800EB5A0.inc"
}
#else
void fn_800EB5A0(void* src) {
    void* r31;
    char stk_c8[0x30];
    char stk_98[0x30];
    char stk_68[0x30];
    char stk_38[0x30];
    char stk_8[0x30];
    lbl_8047ABA0 = 0;
    ((void(*)(void*, void*, void*, void*, f32))fn_800EB6E0)(src, *(void**)((u8*)src + 0x10), *(void**)((u8*)src + 0x14), *(void**)((u8*)src + 0xc), *(f32*)((u8*)src + 0xd4));
    r31 = *(void**)((u8*)src + 0xc);
    if (r31 == NULL) __assert(lbl_8047CC40, 0x47c, lbl_8047CC48);
    if (r31 != NULL) {
        u32 flags = *(u32*)((u8*)r31 + 0x14);
        s32 active = 0;
        if (!(flags & 0x800000) && (flags & 0x40)) active = 1;
        if (active) fn_8019D9DC(r31);
    }
    r31 = (u8*)r31 + 0x44;
    fn_800E0560(stk_c8, (u8*)src + 0x18);
    ((void(*)(void*, f32))fn_800E053C)(stk_98, *(f32*)((u8*)src + 0x24));
    ((void(*)(void*, f32))fn_800E0518)(stk_68, *(f32*)((u8*)src + 0x28));
    ((void(*)(void*, f32))fn_800E04F4)(stk_38, *(f32*)((u8*)src + 0x2c));
    fn_800E042C(stk_8, (u8*)src + 0x30);
    fn_800E0290(r31, r31, stk_8);
    fn_800E0290(r31, r31, stk_c8);
    fn_800E0290(r31, r31, stk_98);
    fn_800E0290(r31, r31, stk_68);
    fn_800E0290(r31, r31, stk_38);
}
#endif

/* fn_800EB6E0 | Size: 0x224 */
#if 0
asm void fn_800EB6E0(void) {
#include "src/game/gs_material_fn_800EB6E0.inc"
}
#else
void fn_800EB6E0(void* r28, void* r18, void* r17, void* r16, f64 f1) {
    void* r31;
    void* r30;
    void* r29;
    void* r25;
    void* r26;
    void* r27;
    void* r22;
    void* r23;
    void* r24;
    void* r19;
    void* r20;
    void* r21;
    if (r18 == NULL) return;
    if (r17 == NULL) return;
    if (r16 == NULL) return;
    ((void(*)(void*, void*, void*, void*, f64))fn_800EB904)(r28, r18, r17, r16, f1);
    if (*(u32*)((u8*)r18 + 0x14) & 0x80000) return;
    r31 = *(void**)((u8*)r18 + 0x10);
    r30 = *(void**)((u8*)r17 + 0x10);
    r29 = *(void**)((u8*)r16 + 0x10);
    while (r31 != NULL) {
        if (r30 == NULL) goto next1;
        if (r29 == NULL) goto next1;
        ((void(*)(void*, void*, void*, void*, f64))fn_800EB904)(r28, r31, r30, r29, f1);
        if (!(*(u32*)((u8*)r31 + 0x14) & 0x80000)) {
            r25 = *(void**)((u8*)r31 + 0x10);
            r26 = *(void**)((u8*)r30 + 0x10);
            r27 = *(void**)((u8*)r29 + 0x10);
            while (r25 != NULL) {
                if (r26 == NULL) goto next2;
                if (r27 == NULL) goto next2;
                ((void(*)(void*, void*, void*, void*, f64))fn_800EB904)(r28, r25, r26, r27, f1);
                if (!(*(u32*)((u8*)r25 + 0x14) & 0x80000)) {
                    r22 = *(void**)((u8*)r25 + 0x10);
                    r23 = *(void**)((u8*)r26 + 0x10);
                    r24 = *(void**)((u8*)r27 + 0x10);
                    while (r22 != NULL) {
                        if (r23 == NULL) goto next3;
                        if (r24 == NULL) goto next3;
                        ((void(*)(void*, void*, void*, void*, f64))fn_800EB904)(r28, r22, r23, r24, f1);
                        if (!(*(u32*)((u8*)r22 + 0x14) & 0x80000)) {
                            r19 = *(void**)((u8*)r22 + 0x10);
                            r20 = *(void**)((u8*)r23 + 0x10);
                            r21 = *(void**)((u8*)r24 + 0x10);
                            while (r19 != NULL) {
                                if (r20 == NULL) goto next4;
                                if (r21 == NULL) goto next4;
                                ((void(*)(void*, void*, void*, void*, f64))fn_800EB904)(r28, r19, r20, r21, f1);
                                if (!(*(u32*)((u8*)r19 + 0x14) & 0x80000)) {
                                    r16 = *(void**)((u8*)r19 + 0x10);
                                    r17 = *(void**)((u8*)r20 + 0x10);
                                    r18 = *(void**)((u8*)r21 + 0x10);
                                    while (r16 != NULL) {
                                        ((void(*)(void*, void*, void*, void*, f64))fn_800EB6E0)(r28, r16, r17, r18, f1);
                                        r16 = *(void**)((u8*)r16 + 0x8);
                                        r17 = *(void**)((u8*)r17 + 0x8);
                                        r18 = *(void**)((u8*)r18 + 0x8);
                                    }
                                }
                                next4:
                                r19 = *(void**)((u8*)r19 + 0x8);
                                r20 = *(void**)((u8*)r20 + 0x8);
                                r21 = *(void**)((u8*)r21 + 0x8);
                            }
                        }
                        next3:
                        r22 = *(void**)((u8*)r22 + 0x8);
                        r23 = *(void**)((u8*)r23 + 0x8);
                        r24 = *(void**)((u8*)r24 + 0x8);
                    }
                }
                next2:
                r25 = *(void**)((u8*)r25 + 0x8);
                r26 = *(void**)((u8*)r26 + 0x8);
                r27 = *(void**)((u8*)r27 + 0x8);
            }
        }
        next1:
        r31 = *(void**)((u8*)r31 + 0x8);
        r30 = *(void**)((u8*)r30 + 0x8);
        r29 = *(void**)((u8*)r29 + 0x8);
    }
}
#endif

/* fn_800EB904 -- Large render setup | Size: 0x5E8 */
extern void fn_801ADAAC(void);
extern void fn_801AD7CC(void);
extern u32 lbl_8047CC50;
extern u32 lbl_8047ABA0;
#if 1
asm void fn_800EB904(void) {
#include "src/game/gs_material_fn_800EB904.inc"
}
#else
void fn_800EB904(void) {
    /* Large render setup (0x5E8 bytes) */
}
#endif

/* _modelSetRotateEulerToQuatAll__FP9_HSD_JObj -- Render config | Size: 0x1FC */
#if 0
asm void _modelSetRotateEulerToQuatAll__FP9_HSD_JObj(void) {
#include "src/game/gs_material_fn_800EBEEC.inc"
}
#else
void _modelSetRotateEulerToQuatAll__FP9_HSD_JObj(void* r23) {
    void* r31;
    void* r30;
    void* r29;
    void* r28;
    void* r27;
    void* r26;
    void* r25;
    void* r24;
    if (r23 == NULL) return;
    fn_8019FE8C(r23, 0x20000);
    if (*(u32*)((u8*)r23 + 0x14) & 0x80000) return;
    r31 = *(void**)((u8*)r23 + 0x10);
    while (r31 != NULL) {
        fn_8019FE8C(r31, 0x20000);
        if (!(*(u32*)((u8*)r31 + 0x14) & 0x80000)) {
            r30 = *(void**)((u8*)r31 + 0x10);
            while (r30 != NULL) {
                fn_8019FE8C(r30, 0x20000);
                if (!(*(u32*)((u8*)r30 + 0x14) & 0x80000)) {
                    r29 = *(void**)((u8*)r30 + 0x10);
                    while (r29 != NULL) {
                        fn_8019FE8C(r29, 0x20000);
                        if (!(*(u32*)((u8*)r29 + 0x14) & 0x80000)) {
                            r28 = *(void**)((u8*)r29 + 0x10);
                            while (r28 != NULL) {
                                fn_8019FE8C(r28, 0x20000);
                                if (!(*(u32*)((u8*)r28 + 0x14) & 0x80000)) {
                                    r27 = *(void**)((u8*)r28 + 0x10);
                                    while (r27 != NULL) {
                                        fn_8019FE8C(r27, 0x20000);
                                        if (!(*(u32*)((u8*)r27 + 0x14) & 0x80000)) {
                                            r26 = *(void**)((u8*)r27 + 0x10);
                                            while (r26 != NULL) {
                                                fn_8019FE8C(r26, 0x20000);
                                                if (!(*(u32*)((u8*)r26 + 0x14) & 0x80000)) {
                                                    r25 = *(void**)((u8*)r26 + 0x10);
                                                    while (r25 != NULL) {
                                                        fn_8019FE8C(r25, 0x20000);
                                                        if (!(*(u32*)((u8*)r25 + 0x14) & 0x80000)) {
                                                            r24 = *(void**)((u8*)r25 + 0x10);
                                                            while (r24 != NULL) {
                                                                fn_8019FE8C(r24, 0x20000);
                                                                if (!(*(u32*)((u8*)r24 + 0x14) & 0x80000)) {
                                                                    r23 = *(void**)((u8*)r24 + 0x10);
                                                                    while (r23 != NULL) {
                                                                        ((void(*)(void*))_modelSetRotateEulerToQuatAll__FP9_HSD_JObj)(r23);
                                                                        r23 = *(void**)((u8*)r23 + 0x8);
                                                                    }
                                                                }
                                                                r24 = *(void**)((u8*)r24 + 0x8);
                                                            }
                                                        }
                                                        r25 = *(void**)((u8*)r25 + 0x8);
                                                    }
                                                }
                                                r26 = *(void**)((u8*)r26 + 0x8);
                                            }
                                        }
                                        r27 = *(void**)((u8*)r27 + 0x8);
                                    }
                                }
                                r28 = *(void**)((u8*)r28 + 0x8);
                            }
                        }
                        r29 = *(void**)((u8*)r29 + 0x8);
                    }
                }
                r30 = *(void**)((u8*)r30 + 0x8);
            }
        }
        r31 = *(void**)((u8*)r31 + 0x8);
    }
}
#endif

/* fn_800EC0E8 -- SetAlpha | Size: 0x4C */
#if 0
asm void fn_800EC0E8(void) {
#include "src/game/gs_material_fn_800EC0E8.inc"
}
#else
void fn_800EC0E8(GSmaterialEntry* entry) {
    u32 flags = entry->flags;
    if (!(flags & 0x10000) || !(flags & 0x1000)) {
        fn_800ED1CC(entry);
        fn_800ED4D4(entry);
    }
}
#endif

/* fn_800EC134 -- UpdateMObjColor | Size: 0x20 */
void fn_800EC134_impl(void) {
    /* GSmaterialUpdateMObjColor: Push colors to HSD MObj (0x20 bytes) */
}

/* fn_800EC154 -- GetMObjPtr | Size: 0xC */
#if 0
asm void fn_800EC154(void) {
#include "src/game/gs_material_fn_800EC154.inc"
}
#else
void fn_800EC154(void* p, void* a, void* b) {
    *(u32*)((u8*)p + 0xdc) = (u32)a;
    *(u32*)((u8*)p + 0xe0) = (u32)b;
}
#endif

/* fn_800EC160 -- SetDiffuseRGBA | Size: 0x28 */
#if 0
asm void fn_800EC160(void) {
#include "src/game/gs_material_fn_800EC160.inc"
}
#else
void fn_800EC160(void* p, u8 enable) {
    if (enable) {
        ((GSmaterialEntry*)p)->flags |= 0x800;
    } else {
        ((GSmaterialEntry*)p)->flags &= ~0x800u;
    }
}
#endif

/* fn_800EC188 -- SetAmbientRGBA | Size: 0x28 */
#if 0
asm void fn_800EC188(void) {
#include "src/game/gs_material_fn_800EC188.inc"
}
#else
void fn_800EC188(void* p, u8 enable) {
    if (enable) {
        ((GSmaterialEntry*)p)->flags |= 0x2000;
    } else {
        ((GSmaterialEntry*)p)->flags &= ~0x2000u;
    }
}
#endif

/* fn_800EC1B0 -- GetDiffuseR | Size: 0xC */
#if 0
asm void fn_800EC1B0(void) {
#include "src/game/gs_material_fn_800EC1B0.inc"
}
#else
u32 fn_800EC1B0(void* p) {
    return (*(u32*)p >> 3) & 1;
}
#endif

/* fn_800EC1BC -- GetDiffuseG | Size: 0xC */
#if 0
asm void fn_800EC1BC(void) {
#include "src/game/gs_material_fn_800EC1BC.inc"
}
#else
u32 fn_800EC1BC(void* p) {
    return (*(u32*)p >> 2) & 1;
}
#endif

/* fn_800EC1C8 -- GetDiffuseB | Size: 0xC */
#if 0
asm void fn_800EC1C8(void) {
#include "src/game/gs_material_fn_800EC1C8.inc"
}
#else
u32 fn_800EC1C8(void* p) {
    return (*(u32*)p >> 15) & 1;
}
#endif

/* fn_800EC1D4 -- GetDiffuseA | Size: 0x10 */
#if 0
asm void fn_800EC1D4(void) {
#include "src/game/gs_material_fn_800EC1D4.inc"
}
#else
void fn_800EC1D4(void* p) {
    ((GSmaterialEntry*)p)->flags &= ~0x40u;
}
#endif

/* fn_800EC1E4 | Size: 0x24 */
#if 0
asm void fn_800EC1E4(void) {
#include "src/game/gs_material_fn_800EC1E4.inc"
}
#else
void fn_800EC1E4(void* p) {
    u32 flags = *(u32*)p;
    if (!(flags & 0x8)) {
        return;
    }
    *(u32*)p = flags | 0x40;
    ((GSmaterialEntry*)p)->flags &= ~0x8000u;
}
#endif

/* fn_800EC208 | Size: 0x9C */
extern void HSD_ForeachAnim();
#if 0
asm void fn_800EC208(void) {
#include "src/game/gs_material_fn_800EC208.inc"
}
#else
void fn_800EC208(void* entry, s32 mode) {
    extern void HSD_ForeachAnim(void*, s32, s32, void*, s32, ...);
    void* mobj = *(void**)((u8*)entry + 0x8);
    if (*(u32*)entry & 0x20000) {
        mobj = *(void**)((u8*)mobj + 0x10);
    }
    *(s32*)((u8*)entry + 0xa4) = mode;
    switch (*(s32*)((u8*)entry + 0xa4)) {
    case 0:
        HSD_ForeachAnim(mobj, 6, 0x64db, fn_800EE054, 3, 0);
        break;
    case 1:
        HSD_ForeachAnim(mobj, 6, 0x64db, fn_800EE054, 3, 1);
        break;
    }
}
#endif

/* fn_800EC2A4 | Size: 0x64 */
extern void fn_801A32A0();
#if 0
asm void fn_800EC2A4(void) {
#include "src/game/gs_material_fn_800EC2A4.inc"
}
#else
void fn_800EC2A4(void* entry, f32 val) {
    u32 flags = *(u32*)entry;
    void* mobj = *(void**)((u8*)entry + 0x8);
    if (!(flags & 0x8)) {
        return;
    }
    if (flags & 0x20000) {
        mobj = *(void**)((u8*)mobj + 0x10);
    }
    *(f32*)((u8*)entry + 0xb0) = val;
    *(f32*)((u8*)entry + 0xb4) = val;
    fn_801A32A0(mobj, 0x634, *(f32*)((u8*)entry + 0xb4));
    flags = *(u32*)entry;
    *(u32*)entry = flags & ~0x8000;
}
#endif

/* fn_800EC308 | Size: 0x54 */
extern s32 fn_800D37CC(void);
extern f32 lbl_8047CC58;
#if 0
asm void fn_800EC308(void) {
#include "src/game/gs_material_fn_800EC308.inc"
}
#else
void fn_800EC308(void* entry, f32 val) {
    u32 flags = *(u32*)entry;
    if (!(flags & 0x8)) {
        return;
    }
    *(f32*)((u8*)entry + 0xac) = val;
    if (fn_800D37CC() == 0x32) {
        f32 stored = *(f32*)((u8*)entry + 0xac);
        *(f32*)((u8*)entry + 0xac) = stored * lbl_8047CC58;
    }
}
#endif

/* fn_800EC35C -- PE descriptor setup | Size: 0x174 */
extern void fn_801A2B5C();
extern f32 lbl_8047CC5C;
#if 0
asm void fn_800EC35C(void) {
#include "src/game/gs_material_fn_800EC35C.inc"
}
#else
void fn_800EC35C(void* entry, u32 idx) {
    u32 flags = *(u32*)entry;
    void* mobj2 = *(void**)((u8*)entry + 0x8);
    s32 mode;
    if (!(flags & 0x8)) {
        return;
    }
    if (idx >= *(u32*)((u8*)entry + 0x88)) {
        return;
    }
    if (idx != *(u32*)((u8*)entry + 0xa8)) {
        if (flags & 0x20000) {
            mobj2 = *(void**)((u8*)mobj2 + 0x10);
        }
        *(u32*)((u8*)entry + 0xa8) = idx;
        {
            void* tbl = *(void**)((u8*)*(void**)((u8*)entry + 0x4) + 0x8);
            void* pe = *(void**)((u8*)tbl + idx * 4);
            fn_801A2B5C(mobj2, 0, pe);
        }
        *(f32*)((u8*)entry + 0xb8) = lbl_8047CC5C;
        HSD_ForeachAnim(mobj2, 6, 0x64db, _modelGetEndFrame, 2, (u8*)entry + 0xb8);
    }
    flags = *(u32*)entry;
    mobj2 = *(void**)((u8*)entry + 0x8);
    if (flags & 0x8) {
        if (flags & 0x20000) {
            mobj2 = *(void**)((u8*)mobj2 + 0x10);
        }
        *(f32*)((u8*)entry + 0xb0) = lbl_8047CC5C;
        *(f32*)((u8*)entry + 0xb4) = lbl_8047CC5C;
        fn_801A32A0(mobj2, 0x634, *(f32*)((u8*)entry + 0xb4));
        *(u32*)entry = flags & ~0x8000;
    }
    flags = *(u32*)entry;
    mode = *(s32*)((u8*)entry + 0xa4);
    mobj2 = *(void**)((u8*)entry + 0x8);
    if (flags & 0x20000) {
        mobj2 = *(void**)((u8*)mobj2 + 0x10);
    }
    *(s32*)((u8*)entry + 0xa4) = mode;
    if (mode == 0) {
        HSD_ForeachAnim(mobj2, 6, 0x64db, fn_800EE054, 3, 0);
    } else if (mode == 1) {
        HSD_ForeachAnim(mobj2, 6, 0x64db, fn_800EE054, 3, 1);
    }
}
#endif

/* fn_800EC4D0 | Size: 0x6C */
extern f32 lbl_8047CC60;
extern f32 lbl_8047CC64;
#if 0
asm void fn_800EC4D0(void) {
#include "src/game/gs_material_fn_800EC4D0.inc"
}
#else
void fn_800EC4D0(void* entry, f32* out0, f32* out1) {
    if (*(u32*)entry & GSMAT_FLAG_RENDERTYPE) {
        if (out0 != NULL) {
            *out0 = lbl_8047CC60 + *(f32*)((u8*)entry + 0xcc);
        }
        if (out1 == NULL) return;
        *out1 = lbl_8047CC60 + *(f32*)((u8*)entry + 0xd0);
    } else {
        if (out0 != NULL) {
            *out0 = lbl_8047CC60 + *(f32*)((u8*)entry + 0xa0);
        }
        if (out1 == NULL) return;
        *out1 = lbl_8047CC64;
    }
}
#endif

/* GSmodelGetAnimFrame | Size: 0x2C */
extern f32 lbl_8047CC5C;
#if 0
asm void GSmodelGetAnimFrame(void) {
#include "src/game/gs_material_GSmodelGetAnimFrame.inc"
}
#else
f32 GSmodelGetAnimFrame(void* p) {
    u32 flags = *(u32*)p;
    if (!(flags & 0x4)) {
        return *(f32*)&lbl_8047CC5C;
    }
    if (flags & GSMAT_FLAG_RENDERTYPE) {
        return *(f32*)((u8*)p + 0xc8);
    }
    return *(f32*)((u8*)p + 0x98);
}
#endif

/* fn_800EC568 | Size: 0x8 */
#if 0
asm void fn_800EC568(void) {
#include "src/game/gs_material_fn_800EC568.inc"
}
#else
u32 fn_800EC568(void* p) {
    return *(u32*)((u8*)p + 0x8c);
}
#endif

/* fn_800EC570 | Size: 0x8 */
#if 0
asm void fn_800EC570(void) {
#include "src/game/gs_material_fn_800EC570.inc"
}
#else
f32 fn_800EC570(void* p) {
    return *(f32*)((u8*)p + 0x94);
}
#endif

/* fn_800EC578 | Size: 0x34 */
#if 0
asm void fn_800EC578(void) {
#include "src/game/gs_material_fn_800EC578.inc"
}
#else
void fn_800EC578(void* p, u32* outA, u32* outB) {
    if (!(*(u32*)p & GSMAT_FLAG_RENDERTYPE)) {
        *outA = *(u32*)((u8*)p + 0x90);
        *outB = (u32)-1;
    } else {
        *outA = *(u32*)((u8*)p + 0xbc);
        *outB = *(u32*)((u8*)p + 0xc0);
    }
}
#endif

/* fn_800EC5AC | Size: 0xC */
#if 0
asm void fn_800EC5AC(void) {
#include "src/game/gs_material_fn_800EC5AC.inc"
}
#else
u32 fn_800EC5AC(void* p) {
    return (*(u32*)p >> 7) & 1;
}
#endif

/* fn_800EC5B8 | Size: 0x44 */
extern f32 lbl_8047CC5C;
extern f32 lbl_8047CC60;
#if 0
asm void fn_800EC5B8(void) {
#include "src/game/gs_material_fn_800EC5B8.inc"
}
#else
void fn_800EC5B8(void* p, f32 val) {
    u32 flags = *(u32*)p;
    if (!(flags & 0x4)) {
        return;
    }
    if (!(flags & GSMAT_FLAG_RENDERTYPE)) {
        return;
    }
    {
        f32 lo = *(f32*)&lbl_8047CC5C;
        f32 hi = *(f32*)&lbl_8047CC60;
        if (val < lo) {
            val = lo;
        } else if (val > hi) {
            val = hi;
        }
    }
    *(f32*)((u8*)p + 0xd4) = val;
}
#endif

/* fn_800EC5FC -- RenderSetup | Size: 0x2CC */
extern void fn_801A1B40(void*);
extern f32 lbl_8047CC5C;
extern f32 lbl_8047CC60;
#if 0
asm void fn_800EC5FC(void) {
#include "src/game/gs_material_fn_800EC5FC.inc"
}
#else
void fn_800EC5FC(void* entry, u32 start, u32 end) {
    u32 flags;
    void* dobj;
    void* data;
    void** tbl;
    f32 fval;

    flags = *(u32*)entry;
    if (!(flags & 0x4)) {
        goto done;
    }
    if (start > *(u32*)((u8*)entry + 0x84)) {
        goto done;
    }
    if (end > *(u32*)((u8*)entry + 0x84)) {
        goto done;
    }

    *(u32*)entry = flags | 0x80;
    *(u32*)((u8*)entry + 0xbc) = start;
    *(u32*)((u8*)entry + 0xc0) = end;

    /* Setup first anim object (entry->0x10) */
    data = *(void**)((u8*)entry + 0x04);
    tbl = *(void***)((u8*)data + 0x04);
    fn_801A2B5C(*(void**)((u8*)entry + 0x10), tbl[*(u32*)((u8*)entry + 0xbc)], 0, 0);

    /* Setup second anim object (entry->0x14) */
    data = *(void**)((u8*)entry + 0x04);
    tbl = *(void***)((u8*)data + 0x04);
    fn_801A2B5C(*(void**)((u8*)entry + 0x14), tbl[*(u32*)((u8*)entry + 0xc0)], 0, 0);

    /* Set frame values */
    fval = *(f32*)&lbl_8047CC5C;
    *(f32*)((u8*)entry + 0xd4) = fval;
    *(f32*)((u8*)entry + 0xc4) = fval;
    *(f32*)((u8*)entry + 0xc8) = fval;

    fn_801A32A0(*(void**)((u8*)entry + 0x10), 0x1cb, *(f32*)((u8*)entry + 0xc4));
    fn_801A32A0(*(void**)((u8*)entry + 0x14), 0x1cb, *(f32*)((u8*)entry + 0xc8));
    fn_801A1B40(*(void**)((u8*)entry + 0x10));
    fn_801A1B40(*(void**)((u8*)entry + 0x14));

    /* Setup timer callbacks for entry->0x10 */
    fval = *(f32*)&lbl_8047CC5C;
    *(f32*)((u8*)entry + 0xcc) = fval;
    HSD_ForeachAnim(*(void**)((u8*)entry + 0x10), 6, 0x9B2F, (void*)_modelGetEndFrame, 2, (u8*)entry + 0xcc);

    /* Setup timer callbacks for entry->0x14 */
    fval = *(f32*)&lbl_8047CC5C;
    *(f32*)((u8*)entry + 0xd0) = fval;
    HSD_ForeachAnim(*(void**)((u8*)entry + 0x14), 6, 0x9B2F, (void*)_modelGetEndFrame, 2, (u8*)entry + 0xd0);

    /* Compute blend ratio */
    {
        f32 bias = *(f32*)&lbl_8047CC60;
        f32 v1 = *(f32*)((u8*)entry + 0xcc);
        f32 v2 = *(f32*)((u8*)entry + 0xd0);
        *(f32*)((u8*)entry + 0xd8) = (bias + v1) / (bias + v2);
    }

    /* Check secondary animation flags */
    flags = *(u32*)entry;
    if (!(flags & 0x2000)) {
        goto clear_flag;
    }
    if (!(flags & 0x8)) {
        goto clear_flag;
    }
    dobj = *(void**)((u8*)entry + 0x08);
    if (end >= *(u32*)((u8*)entry + 0x88)) {
        goto clear_flag;
    }
    if (end != *(u32*)((u8*)entry + 0xa8)) {
        if (flags & 0x20000) {
            dobj = *(void**)((u8*)dobj + 0x10);
        }
        *(u32*)((u8*)entry + 0xa8) = end;
        data = *(void**)((u8*)entry + 0x04);
        tbl = *(void***)((u8*)data + 0x08);
        fn_801A2B5C(dobj, tbl[*(u32*)((u8*)entry + 0xa8)], 0, 0);
        fval = *(f32*)&lbl_8047CC5C;
        *(f32*)((u8*)entry + 0xb8) = fval;
        HSD_ForeachAnim(dobj, 6, 0x64db, (void*)_modelGetEndFrame, 2, (u8*)entry + 0xb8);
    }

    /* Part 2: handle blend/anim state */
    {
        u32 fl = *(u32*)entry;
        void* obj2 = *(void**)((u8*)entry + 0x08);
        if (fl & 0x8) {
            if (fl & 0x20000) {
                obj2 = *(void**)((u8*)obj2 + 0x10);
            }
            fval = *(f32*)&lbl_8047CC5C;
            *(f32*)((u8*)entry + 0xb0) = fval;
            *(f32*)((u8*)entry + 0xb4) = fval;
            fn_801A32A0(obj2, 0x634, *(f32*)((u8*)entry + 0xb4));
            *(u32*)entry = *(u32*)entry & ~0x4000;
        }
    }

    /* Handle entry->0xa4 switch */
    {
        u32 fl2 = *(u32*)entry;
        s32 mode = *(s32*)((u8*)entry + 0xa4);
        void* obj3 = *(void**)((u8*)entry + 0x08);
        if (fl2 & 0x20000) {
            obj3 = *(void**)((u8*)obj3 + 0x10);
        }
        *(s32*)((u8*)entry + 0xa4) = mode;
        switch (mode) {
        case 0:
            HSD_ForeachAnim(obj3, 6, 0x64db, (void*)fn_800EE054, 3, 0);
            break;
        case 1:
            HSD_ForeachAnim(obj3, 6, 0x64db, (void*)fn_800EE054, 3, 1);
            break;
        }
    }

clear_flag:
    *(u32*)entry = *(u32*)entry & ~0x1000;
done:
    ;
}
#endif

/* fn_800EC8C8 | Size: 0x14 */
extern f32 lbl_8047CC5C;
#if 0
asm void fn_800EC8C8(void) {
#include "src/game/gs_material_fn_800EC8C8.inc"
}
#else
void fn_800EC8C8(void* p, f32 a, f32 b) {
    *(f32*)((u8*)p + 0xd8) = *(f32*)&lbl_8047CC5C;
    *(f32*)((u8*)p + 0xc4) = a;
    *(f32*)((u8*)p + 0xc8) = b;
}
#endif

/* fn_800EC8DC | Size: 0x3C */
extern u32 lbl_8047AB74;
extern u32 lbl_8047AB78;
#if 0
asm void fn_800EC8DC(void) {
#include "src/game/gs_material_fn_800EC8DC.inc"
}
#else
void fn_800EC8DC(void) {
    u32 i;
    u32 count = gsMatPoolCount;
    for (i = 0; i < count; i++) {
        GSmaterialEntry* entry = (GSmaterialEntry*)((u8*)gsMatPool + i * 0x170);
        if (entry->flags & GSMAT_FLAG_ACTIVE) {
            entry->flags &= ~GSMAT_FLAG_ALPHATEST;
        }
    }
}
#endif

/* fn_800EC918 | Size: 0x3C */
extern u32 lbl_8047AB74;
extern u32 lbl_8047AB78;
#if 0
asm void fn_800EC918(void) {
#include "src/game/gs_material_fn_800EC918.inc"
}
#else
void fn_800EC918(void) {
    u32 i = 0;
    while (i < lbl_8047AB78) {
        GSmaterialEntry* entry = (GSmaterialEntry*)((u8*)lbl_8047AB74 + i * 0x170);
        if (entry->flags & GSMAT_FLAG_ACTIVE) {
            entry->flags |= GSMAT_FLAG_ALPHATEST;
        }
        i++;
    }
}
#endif

/* fn_800EC954 | Size: 0xC */
#if 0
asm void fn_800EC954(void) {
#include "src/game/gs_material_fn_800EC954.inc"
}
#else
u32 fn_800EC954(void* p) {
    return (*(u32*)p >> 14) & 1;
}
#endif

/* fn_800EC960 | Size: 0xC */
#if 0
asm void fn_800EC960(void) {
#include "src/game/gs_material_fn_800EC960.inc"
}
#else
u32 fn_800EC960(void* p) {
    return (*(u32*)p >> 5) & 1;
}
#endif

/* fn_800EC96C | Size: 0x24 */
#if 0
asm void fn_800EC96C(void) {
#include "src/game/gs_material_fn_800EC96C.inc"
}
#else
void fn_800EC96C(void* p) {
    u32 flags;
    ((GSmaterialEntry*)p)->flags &= ~0x20u;
    flags = *(u32*)p;
    if (!(flags & GSMAT_FLAG_TWOSIDED)) {
        return;
    }
    *(u32*)p = flags & ~0x40;
}
#endif

/* fn_800EC990 | Size: 0x4C */
#if 0
asm void fn_800EC990(void) {
#include "src/game/gs_material_fn_800EC990.inc"
}
#else
void fn_800EC990(void* p) {
    u32 flags = *(u32*)p;
    if (!(flags & 0x4)) {
        return;
    }
    flags |= 0x20;
    *(u32*)p = flags;
    flags = *(u32*)p;
    *(u32*)p = flags & ~0x4000;
    flags = *(u32*)p;
    if (!(flags & GSMAT_FLAG_TWOSIDED)) {
        return;
    }
    if (!(flags & 0x8)) {
        return;
    }
    flags |= 0x40;
    *(u32*)p = flags;
    flags = *(u32*)p;
    *(u32*)p = flags & ~0x8000;
}
#endif

/* fn_800EC9DC | Size: 0x9C */
extern f32 lbl_8047CC58;
#if 0
asm void fn_800EC9DC(void) {
#include "src/game/gs_material_fn_800EC9DC.inc"
}
#else
void fn_800EC9DC(void* entry, f32 val) {
    if (*(u32*)entry & 0x4) {
        *(f32*)((u8*)entry + 0x94) = val;
        if (fn_800D37CC() == 0x32) {
            f32 stored = *(f32*)((u8*)entry + 0x94);
            *(f32*)((u8*)entry + 0x94) = stored * *(f32*)&lbl_8047CC58;
        }
        {
            u32 flags = *(u32*)entry;
            if (!(flags & 0x2000)) {
                return;
            }
            if (!(flags & 0x8)) {
                return;
            }
        }
        *(f32*)((u8*)entry + 0xac) = val;
        if (fn_800D37CC() == 0x32) {
            f32 stored = *(f32*)((u8*)entry + 0xac);
            *(f32*)((u8*)entry + 0xac) = stored * *(f32*)&lbl_8047CC58;
        }
    }
}
#endif

/* fn_800ECA78 -- EnvMap alpha apply | Size: 0xFC */
void fn_800ECA78_impl(void) {
    /* GSmaterialEnvMapAlphaApply: Env-map alpha apply (0xFC bytes) */
}

/* fn_800ECB74 | Size: 0x134 */
#if 0
asm void fn_800ECB74(void) {
#include "src/game/gs_material_fn_800ECB74.inc"
}
#else
void fn_800ECB74(void* entry, s32 mode) {
    void* mobj = *(void**)((u8*)entry + 0x8);
    if (*(u32*)entry & 0x20000) {
        mobj = *(void**)((u8*)mobj + 0x10);
    }
    *(s32*)((u8*)entry + 0x8c) = mode;
    if (mode == 0) {
        HSD_ForeachAnim(mobj, 6, 0x9b2f, fn_800EE054, 3, 0);
    } else if (mode == 1) {
        HSD_ForeachAnim(mobj, 6, 0x9b2f, fn_800EE054, 3, 1);
    }
    {
        u32 flags = *(u32*)entry;
        if (flags & 0x2000) {
            void* mobj2 = *(void**)((u8*)entry + 0x8);
            if (flags & 0x20000) {
                mobj2 = *(void**)((u8*)mobj2 + 0x10);
            }
            *(s32*)((u8*)entry + 0xa4) = mode;
            if (mode == 0) {
                HSD_ForeachAnim(mobj2, 6, 0x64db, fn_800EE054, 3, 0);
            } else if (mode == 1) {
                HSD_ForeachAnim(mobj2, 6, 0x64db, fn_800EE054, 3, 1);
            }
        }
    }
}
#endif

/* fn_800ECCA8 -- ShadowSetup | Size: 0x428 */
extern f32 lbl_8047CC5C;
#if 0
asm void fn_800ECCA8(void* entry, s32 idx) {
#include "src/game/gs_material_fn_800ECCA8.inc"
}
#else
void fn_800ECCA8(void* entry, s32 idx) {
    u32 flags;
    void* dobj;
    void* data;
    void** tbl;
    f32 fval;
    s32 mode;
    void* r6val;

    flags = *(u32*)entry;
    dobj = *(void**)((u8*)entry + 0x08);
    if (!(flags & 0x4)) {
        goto done;
    }
    if ((u32)idx >= *(u32*)((u8*)entry + 0x84)) {
        goto done;
    }

    if ((u32)idx == *(u32*)((u8*)entry + 0x90)) {
        if (!(flags & 0x80)) {
            goto part2;
        }
    }

    if (flags & 0x20000) {
        dobj = *(void**)((u8*)dobj + 0x10);
    }

    *(u32*)entry = *(u32*)entry & ~0x100;
    *(u32*)((u8*)entry + 0x90) = idx;

    if (*(u32*)entry & 0x10) {
        data = *(void**)((u8*)entry + 0x04);
        tbl = *(void***)((u8*)data + 0x0c);
        r6val = tbl[*(u32*)((u8*)entry + 0x90)];
    } else {
        r6val = NULL;
    }
    data = *(void**)((u8*)entry + 0x04);
    tbl = *(void***)((u8*)data + 0x04);
    fn_801A2B5C(dobj, tbl[*(u32*)((u8*)entry + 0x90)], 0, r6val);

    fval = *(f32*)&lbl_8047CC5C;
    *(f32*)((u8*)entry + 0xa0) = fval;
    HSD_ForeachAnim(dobj, 6, 0x9B2F, (void*)_modelGetEndFrame, 2, (u8*)entry + 0xa0);

part2:
    flags = *(u32*)entry;
    dobj = *(void**)((u8*)entry + 0x08);
    if (!(flags & 0x4)) {
        goto section3;
    }
    if (flags & 0x80) {
        fval = *(f32*)&lbl_8047CC5C;
        *(f32*)((u8*)entry + 0xc4) = fval * *(f32*)((u8*)entry + 0xd8);
        *(f32*)((u8*)entry + 0xc8) = fval;
        fn_801A32A0(*(void**)((u8*)entry + 0x10), 0x1cb, *(f32*)((u8*)entry + 0xc4));
        fn_801A32A0(*(void**)((u8*)entry + 0x14), 0x1cb, *(f32*)((u8*)entry + 0xc8));
    } else {
        if (flags & 0x20000) {
            dobj = *(void**)((u8*)dobj + 0x10);
        }
        fval = *(f32*)&lbl_8047CC5C;
        *(f32*)((u8*)entry + 0x98) = fval;
        *(f32*)((u8*)entry + 0x9c) = fval;
        fn_801A32A0(dobj, 0x1cb, *(f32*)((u8*)entry + 0x9c));
    }

    {
        s32 mask = -0x5001;
        *(u32*)entry = *(u32*)entry & mask;
    }

    flags = *(u32*)entry;
    if (!(flags & 0x2000)) {
        goto section3;
    }
    if (!(flags & 0x8)) {
        goto section3;
    }
    dobj = *(void**)((u8*)entry + 0x08);
    if (flags & 0x20000) {
        dobj = *(void**)((u8*)dobj + 0x10);
    }
    fval = *(f32*)&lbl_8047CC5C;
    *(f32*)((u8*)entry + 0xb0) = fval;
    *(f32*)((u8*)entry + 0xb4) = fval;
    fn_801A32A0(dobj, 0x634, *(f32*)((u8*)entry + 0xb4));
    *(u32*)entry = *(u32*)entry & ~0x4000;

section3:
    {
        void* mobj;
        mode = *(s32*)((u8*)entry + 0x8c);
        mobj = *(void**)((u8*)entry + 0x08);
        if (*(u32*)entry & 0x20000) {
            mobj = *(void**)((u8*)mobj + 0x10);
        }
        *(s32*)((u8*)entry + 0x8c) = mode;
        if (mode == 0) {
            HSD_ForeachAnim(mobj, 6, 0x9B2F, (void*)fn_800EE054, 3, 0);
        } else if (mode == 1) {
            HSD_ForeachAnim(mobj, 6, 0x9B2F, (void*)fn_800EE054, 3, 1);
        }
    }

    flags = *(u32*)entry;
    if (!(flags & 0x2000)) {
        goto clear_flag;
    }
    if (!(flags & 0x8)) {
        goto clear_flag;
    }
    dobj = *(void**)((u8*)entry + 0x08);
    if ((u32)idx >= *(u32*)((u8*)entry + 0x88)) {
        goto clear_flag;
    }
    if ((u32)idx != *(u32*)((u8*)entry + 0xa8)) {
        if (flags & 0x20000) {
            dobj = *(void**)((u8*)dobj + 0x10);
        }
        *(u32*)((u8*)entry + 0xa8) = idx;
        data = *(void**)((u8*)entry + 0x04);
        tbl = *(void***)((u8*)data + 0x08);
        fn_801A2B5C(dobj, tbl[*(u32*)((u8*)entry + 0xa8)], 0, 0);
        fval = *(f32*)&lbl_8047CC5C;
        *(f32*)((u8*)entry + 0xb8) = fval;
        HSD_ForeachAnim(dobj, 6, 0x64db, (void*)_modelGetEndFrame, 2, (u8*)entry + 0xb8);
    }

    {
        u32 fl = *(u32*)entry;
        void* obj2 = *(void**)((u8*)entry + 0x08);
        if (fl & 0x8) {
            if (fl & 0x20000) {
                obj2 = *(void**)((u8*)obj2 + 0x10);
            }
            fval = *(f32*)&lbl_8047CC5C;
            *(f32*)((u8*)entry + 0xb0) = fval;
            *(f32*)((u8*)entry + 0xb4) = fval;
            fn_801A32A0(obj2, 0x634, *(f32*)((u8*)entry + 0xb4));
            *(u32*)entry = *(u32*)entry & ~0x4000;
        }
    }

    {
        u32 fl2;
        void* mobj2;
        fl2 = *(u32*)entry;
        mobj2 = *(void**)((u8*)entry + 0x08);
        if (fl2 & 0x20000) {
            mobj2 = *(void**)((u8*)mobj2 + 0x10);
        }
        *(s32*)((u8*)entry + 0xa4) = *(s32*)((u8*)entry + 0xa4);
        mode = *(s32*)((u8*)entry + 0xa4);
        if (mode == 0) {
            HSD_ForeachAnim(mobj2, 6, 0x64db, (void*)fn_800EE054, 3, 0);
        } else if (mode == 1) {
            HSD_ForeachAnim(mobj2, 6, 0x64db, (void*)fn_800EE054, 3, 1);
        }
    }

clear_flag:
    *(u32*)entry = *(u32*)entry & ~0x1000;
done:
    ;
}
#endif

/* fn_800ED0D0 -- PostRender | Size: 0xFC */
#if 0
asm void fn_800ED0D0(void) {
#include "src/game/gs_material_fn_800ED0D0.inc"
}
#else
void fn_800ED0D0(void* entry, f32 scale) {
    u32 flags = *(u32*)entry;
    if (flags & 0x100) {
        return;
    }
    if (flags & 0x4000) {
        ((void(*)(void*, u8))fn_800ED6E4)(entry, 0);
    }
    flags = *(u32*)entry;
    if (flags & 0x8000) {
        ((void(*)(void*, u8))fn_800ED6E4)(entry, 1);
    }
    flags = *(u32*)entry;
    if (flags & GSMAT_FLAG_RENDERTYPE) {
        if (flags & 0x20) {
            f32 val = *(f32*)((u8*)entry + 0x94) * scale;
            ((void(*)(void*, f32, u8))fn_800ED7E4)(entry, val, 0);
            *(f32*)((u8*)entry + 0xc4) = *(f32*)((u8*)entry + 0xc8) * *(f32*)((u8*)entry + 0xd8);
        }
    } else {
        if (flags & 0x20) {
            f32 val = *(f32*)((u8*)entry + 0x94) * scale;
            ((void(*)(void*, f32, u8))fn_800ED7E4)(entry, val, 0);
        }
        flags = *(u32*)entry;
        if (flags & 0x40) {
            f32 val = *(f32*)((u8*)entry + 0xac) * scale;
            ((void(*)(void*, f32, u8))fn_800ED7E4)(entry, val, 1);
        }
    }
    *(u32*)entry &= ~0x10000;
}
#endif

/* fn_800ED1CC | Size: 0x308 */
extern void fn_801C027C(void);
extern u32 lbl_8047CC68;
extern u32 lbl_8047CC70;
#if 0
asm void fn_800ED1CC(void) {
#include "src/game/gs_material_fn_800ED1CC.inc"
}
#else
void fn_800ED1CC(void* entry) {
    extern void HSD_ForeachAnim(void*, int, int, void*, int, ...);
    u32 flags = *(u32*)entry;
    void* r31;
    f32 f31, f30;
    if (flags & 0x1000) {
        return;
    }
    if (flags & GSMAT_FLAG_RENDERTYPE) {
        fn_801A32A0(*(void**)((u8*)entry + 0x10), 0x1cb, *(f32*)((u8*)entry + 0xc4));
        fn_801A32A0(*(void**)((u8*)entry + 0x14), 0x1cb, *(f32*)((u8*)entry + 0xc8));
        fn_801A1B40(*(void**)((u8*)entry + 0x10));
        fn_801A1B40(*(void**)((u8*)entry + 0x14));
        fn_800EB5A0(entry);
    } else {
        r31 = *(void**)((u8*)entry + 0x8);
        if (flags & 0x20000) {
            r31 = *(void**)((u8*)r31 + 0x10);
        }
        f31 = *(f32*)&lbl_8047CC5C;
        f30 = f31;
        if (flags & 0x20) {
            u8 r4;
            if (((flags & 0x800) == 0x800) || (*(s32*)((u8*)entry + 0x114) != 0 && *(u32*)((u8*)entry + 0x118) != 0)) {
                r4 = 1;
            } else {
                r4 = 0;
            }
            f31 = fn_800ED8C4(
                *(s32*)((u8*)entry + 0x8c), r4,
                *(f32*)((u8*)entry + 0x98), *(f32*)((u8*)entry + 0x9c),
                *(f32*)((u8*)entry + 0xa0), *(f32*)((u8*)entry + 0x94));
        }
        if (*(u32*)entry & 0x40) {
            u8 r4;
            flags = *(u32*)entry;
            if (((flags & 0x800) == 0x800) || (*(s32*)((u8*)entry + 0x114) != 0 && *(u32*)((u8*)entry + 0x118) != 0)) {
                r4 = 1;
            } else {
                r4 = 0;
            }
            f30 = fn_800ED8C4(
                *(s32*)((u8*)entry + 0xa4), r4,
                *(f32*)((u8*)entry + 0xb0), *(f32*)((u8*)entry + 0xb4),
                *(f32*)((u8*)entry + 0xb8), *(f32*)((u8*)entry + 0xac));
        }
        HSD_ForeachAnim(r31, 6, 0x9b2f, fn_801C027C, 1, f31);
        HSD_ForeachAnim(r31, 6, 0x64db, fn_801C027C, 1, f30);
        fn_801A1B40(r31);
        if (*(u32*)entry & 0x20) {
            void* result = NULL;
            HSD_ForeachAnim(r31, 6, 0x20, fn_800EE044, 2, &result);
            *(f32*)((u8*)entry + 0x9c) = *(f32*)((u8*)result + 0x4);
        }
        if (*(u32*)entry & 0x40) {
            void* result = NULL;
            HSD_ForeachAnim(r31, 6, 0x480, fn_800EE044, 2, &result);
            *(f32*)((u8*)entry + 0xb4) = *(f32*)((u8*)result + 0x4);
        }
        if (*(u32*)entry & 0x20) {
            fn_800EDA98(entry);
        }
        if (*(u32*)entry & 0x20000) {
            void* tev = *(void**)((u8*)*(void**)((u8*)entry + 0x8) + 0x10);
            if (tev != NULL) {
                s32 r3;
                u32 f;
                if (tev == NULL) {
                    __assert(&lbl_8047CC68, 0x25d, &lbl_8047CC70);
                }
                f = *(u32*)((u8*)tev + 0x14);
                r3 = 0;
                if (!(f & 0x800000)) {
                    if (f & 0x40) {
                        r3 = 1;
                    }
                }
                if (r3 != 0) {
                    fn_8019D9DC(tev);
                }
            }
        }
        {
            void* mobj = *(void**)((u8*)entry + 0x8);
            if (mobj != NULL) {
                s32 r3;
                u32 f;
                if (mobj == NULL) {
                    __assert(&lbl_8047CC68, 0x25d, &lbl_8047CC70);
                }
                f = *(u32*)((u8*)mobj + 0x14);
                r3 = 0;
                if (!(f & 0x800000)) {
                    if (f & 0x40) {
                        r3 = 1;
                    }
                }
                if (r3 != 0) {
                    fn_8019D9DC(mobj);
                }
            }
        }
    }
    *(u32*)entry |= 0x1000;
}
#endif

/* fn_800ED4D4 | Size: 0x1D8 */
extern void fn_800E06EC(void*, void*);
extern void fn_800DFEEC(void*, void*, void*);
extern void fn_800E0108(void*, void*, void*);
extern void jumptable_803155B0();
extern void fn_800E50A8();
extern void fn_800E4E8C();
extern void fn_800E4DB0();
#if 0
asm void fn_800ED4D4(void) {
#include "src/game/gs_material_fn_800ED4D4.inc"
}
#else
void fn_800ED4D4(void* entry) {
    u8 buf0[12]; /* sp+0x3c */
    u8 buf1[12]; /* sp+0x30 */
    u8 buf2[12]; /* sp+0x24 */
    u8 tmp0[12]; /* sp+0x14 */
    u8 tmp1[12]; /* sp+0x8 */
    void* r31;
    u32 flags;
    u32 updateState;

    updateState = *(u32*)((u8*)entry + 0x114);
    if (updateState == 0) goto done;
    flags = *(u32*)entry;
    if (!(flags & 0x80000)) goto done;

    r31 = fn_800EE150(*(void**)((u8*)entry + 0x118), *(u32*)((u8*)entry + 0x11c));
    fn_800EE3BC(r31, buf0, buf1, buf2);
    fn_800EE828(r31);

    fn_800E06EC(tmp0, buf1);
    fn_800DFEEC(tmp1, tmp0, (u8*)entry + 0x120);
    fn_800E019C(buf0, buf0, tmp1);
    fn_800E019C(buf1, buf1, (u8*)entry + 0x12c);
    fn_800E0108(buf2, buf2, (u8*)entry + 0x138);

    fn_800E50A8(entry, (u8*)entry + 0x120);
    fn_800E4E8C(entry, (u8*)entry + 0x12c);
    fn_800E4DB0(entry, (u8*)entry + 0x138);

    switch (updateState) {
    case 0:
        fn_800E50A8(entry, buf0);
        break;
    case 1:
        fn_800E4E8C(entry, buf1);
        break;
    case 2:
        fn_800E4DB0(entry, buf2);
        break;
    case 3:
        fn_800E50A8(entry, buf0);
        fn_800E4E8C(entry, buf1);
        break;
    case 4:
        fn_800E4E8C(entry, buf1);
        fn_800E4DB0(entry, buf2);
        break;
    case 5:
        fn_800E50A8(entry, buf0);
        fn_800E4DB0(entry, buf2);
        break;
    case 6:
        fn_800E50A8(entry, buf0);
        fn_800E4E8C(entry, buf1);
        fn_800E4DB0(entry, buf2);
        break;
    case 7:
        break;
    default:
        break;
    }

    flags = *(u32*)entry;
    if (flags & 0x40000) {
        *(u32*)entry = flags & ~0x80000u;
    } else {
        *(u32*)entry = flags | 0x80000;
    }
done:
    *(u32*)entry |= 0x10000;
}
#endif

/* fn_800ED6AC | Size: 0x38 */
#if 0
asm void fn_800ED6AC(void) {
#include "src/game/gs_material_fn_800ED6AC.inc"
}
#else
s32 fn_800ED6AC(GSmaterialEntry* entry) {
    if ((*(u32*)entry & 0x800) == 0x800) {
        goto _ret1;
    }
    if (*(s32*)((u8*)entry + 0x114) == 0) {
        goto _ret0;
    }
    if (*(u32*)((u8*)entry + 0x118) == 0) {
        goto _ret0;
    }
_ret1:
    return 1;
_ret0:
    return 0;
}
#endif

/* fn_800ED6E4 | Size: 0x100 */
#if 0
asm void fn_800ED6E4(void) {
#include "src/game/gs_material_fn_800ED6E4.inc"
}
#else
void fn_800ED6E4(void* entry, u8 enable) {
    u32 r5, r6, r7, r8;
    void (*cb)(void*, void*);
    if (*(u32*)entry & GSMAT_FLAG_RENDERTYPE) {
        if (enable == 0) {
            r8 = *(u32*)((u8*)entry + 0xc0);
            r5 = 0x4000;
            r7 = *(u32*)((u8*)entry + 0x8c);
            r6 = 0x20;
        } else {
            r8 = *(u32*)((u8*)entry + 0xa8);
            r5 = 0x8000;
            r7 = *(u32*)((u8*)entry + 0xa4);
            r6 = 0x40;
        }
    } else {
        if (enable == 0) {
            r8 = *(u32*)((u8*)entry + 0x90);
            r5 = 0x4000;
            r7 = *(u32*)((u8*)entry + 0x8c);
            r6 = 0x20;
        } else {
            r8 = *(u32*)((u8*)entry + 0xa8);
            r5 = 0x8000;
            r7 = *(u32*)((u8*)entry + 0xa4);
            r6 = 0x40;
        }
    }
    if (r7 == 0) {
        *(u32*)entry &= ~r6;
    } else if (r7 == 1) {
        *(u32*)entry &= ~r5;
    }
    cb = (void(*)(void*, void*))*(void**)((u8*)entry + 0xdc);
    if (cb != NULL) {
        struct {
            u32 flags;
            u32 val;
            u32 e0;
        } args;
        args.flags = enable ? 2 : 1;
        if (r7 == 1) {
            args.flags |= 4;
        }
        args.val = r8;
        args.e0 = *(u32*)((u8*)entry + 0xe0);
        cb(entry, &args);
    }
}
#endif

/* fn_800ED7E4 | Size: 0xE0 */
extern f32 lbl_8047CC78;
#if 0
asm void fn_800ED7E4(void) {
#include "src/game/gs_material_fn_800ED7E4.inc"
}
#else
void fn_800ED7E4(void* entry, f32 delta) {
    f32* p;
    f32* q;
    f32 target;
    u32 r4;
    u32 r7;
    f32 threshold;
    if (*(u32*)entry & GSMAT_FLAG_RENDERTYPE) {
        if (delta == 0.0f) {
            p      = (f32*)((u8*)entry + 0xc8);
            target = *(f32*)((u8*)entry + 0xd0);
            r7     = *(u32*)((u8*)entry + 0x8c);
            q      = p;
            r4     = 0x4000;
        } else {
            p      = (f32*)((u8*)entry + 0xb0);
            target = *(f32*)((u8*)entry + 0xb8);
            r7     = *(u32*)((u8*)entry + 0xa4);
            q      = (f32*)((u8*)entry + 0xb4);
            r4     = 0x8000;
        }
    } else {
        if (delta == 0.0f) {
            p      = (f32*)((u8*)entry + 0x98);
            target = *(f32*)((u8*)entry + 0xa0);
            r7     = *(u32*)((u8*)entry + 0x8c);
            q      = (f32*)((u8*)entry + 0x9c);
            r4     = 0x4000;
        } else {
            p      = (f32*)((u8*)entry + 0xb0);
            target = *(f32*)((u8*)entry + 0xb8);
            r7     = *(u32*)((u8*)entry + 0xa4);
            q      = (f32*)((u8*)entry + 0xb4);
            r4     = 0x8000;
        }
    }
    threshold = target - lbl_8047CC78;
    *p = *p + delta;
    if (*p >= threshold) {
        if (r7 == 0) {
            *p = target;
        } else if (r7 == 1) {
            *p = *p - target;
        }
    }
    if (*q >= threshold) {
        if (r7 != 0) return;
        *(u32*)entry |= r4;
    }
}
#endif

/* fn_800ED8C4 | Size: 0x1D4 */
extern s32 fn_800D3088(void);
extern f32 fn_800CE318(f64 val);
extern u8  lbl_80478AF8;
extern f32 lbl_8047CC5C;
extern f64 lbl_8047CC80;
extern f32 lbl_8047CC78;
extern f64 lbl_8047CC88;
extern f32 lbl_8047CC60;
#if 0
asm void fn_800ED8C4(void) {
#include "src/game/gs_material_fn_800ED8C4.inc"
}
#else
f32 fn_800ED8C4(s32 mode, u8 enable, f32 cur, f32 threshold, f32 target, f32 step) {
    f32 f1 = cur;
    if (lbl_80478AF8 == 1 && fn_800D37CC() == 0x3c) {
        if (enable == 0) {
            /* Convert cur to int and back, check delta */
            f64 fconv = (f64)(s32)cur;
            f64 fbase = lbl_8047CC80;
            f32 delta = lbl_8047CC78;
            f32 diff = (f32)(cur - (f32)(fconv - fbase));
            if (diff > delta) {
                s32 frames = fn_800D3088() - 1;
                f64 fframes = (f64)frames;
                f64 fbase2 = lbl_8047CC88;
                f32 fstep = (f32)((fframes - fbase2) * step);
                if (fstep >= lbl_8047CC60) {
                    /* ok */
                } else {
                    fstep = lbl_8047CC5C;
                }
                f1 = cur + fstep;
            } else {
                f1 = cur;
            }
        } else {
            f1 = cur;
        }
    }
    if (target > lbl_8047CC5C) {
        f32 limit = target - lbl_8047CC78;
        if (f1 >= limit) {
            if (mode == 0) {
                f1 = target;
            } else if (mode == 1) {
                f1 = (f32)fn_800CE318((f64)target);
            }
        }
    }
    if (f1 != threshold) {
        if (f1 < threshold) {
            if (mode == 1) {
                f32 f31 = f1 + (target - threshold);
                if (f31 < lbl_8047CC5C) f31 = lbl_8047CC5C;
                return f31;
            } else {
                f32 f31 = lbl_8047CC5C;
                if (f31 < lbl_8047CC5C) f31 = lbl_8047CC5C;
                return f31;
            }
        } else {
            f32 f31 = f1 - threshold;
            if (f31 < lbl_8047CC5C) f31 = lbl_8047CC5C;
            return f31;
        }
    }
    {
        f32 f31 = f1 - threshold;
        if (f31 < lbl_8047CC5C) f31 = lbl_8047CC5C;
        return f31;
    }
}
#endif

/* fn_800EDA98 -- FullPipeline | Size: 0x5AC */
extern u8 lbl_80270EE8[];
#if 1
asm void fn_800EDA98(void* entry) {
#include "src/game/gs_material_fn_800EDA98.inc"
}
#else
extern void fn_800EE828(void*);
void fn_800EDA98(void* entry) {
    s32 count;
    void* result;
    u8* ptr;
    void* mobj;
    f32 val;

    count = 3;
    ptr = (u8*)entry + 0xe4;
    do {
        if (*(s32*)ptr != 0) {
            result = fn_800EE150(entry, *(u32*)(ptr + 4));
            if (result != NULL) {
                switch (*(s32*)ptr) {
                case 1:
                    mobj = *(void**)((u8*)result + 0x8);
                    val = *(f32*)*(void**)(ptr + 0x8);
                    if (mobj == NULL) {
                        __assert(&lbl_8047CC68, 0x2a4, &lbl_8047CC70);
                    }
                    if (*(u32*)((u8*)mobj + 0x14) & 0x20000) {
                        __assert(&lbl_8047CC68, 0x2a5, lbl_80270EE8);
                    }
                    *(f32*)((u8*)mobj + 0x1c) = val;
                    if (!(*(u32*)((u8*)mobj + 0x14) & 0x02000000)) {
                        if (mobj != NULL) {
                            u32 fl;
                            s32 ck;
                            if (mobj == NULL) {
                                __assert(&lbl_8047CC68, 0x25d, &lbl_8047CC70);
                            }
                            fl = *(u32*)((u8*)mobj + 0x14);
                            ck = 0;
                            if (!(fl & 0x800000) && (fl & 0x40)) {
                                ck = 1;
                            }
                            if (!ck) {
                                fn_8019D620(mobj);
                            }
                        }
                    }

                    mobj = *(void**)((u8*)result + 0x8);
                    val = ((f32*)*(void**)(ptr + 0x8))[1];
                    if (mobj == NULL) {
                        __assert(&lbl_8047CC68, 0x2b8, &lbl_8047CC70);
                    }
                    if (*(u32*)((u8*)mobj + 0x14) & 0x20000) {
                        __assert(&lbl_8047CC68, 0x2b9, lbl_80270EE8);
                    }
                    *(f32*)((u8*)mobj + 0x20) = val;
                    if (!(*(u32*)((u8*)mobj + 0x14) & 0x02000000)) {
                        if (mobj != NULL) {
                            u32 fl;
                            s32 ck;
                            if (mobj == NULL) {
                                __assert(&lbl_8047CC68, 0x25d, &lbl_8047CC70);
                            }
                            fl = *(u32*)((u8*)mobj + 0x14);
                            ck = 0;
                            if (!(fl & 0x800000) && (fl & 0x40)) {
                                ck = 1;
                            }
                            if (!ck) {
                                fn_8019D620(mobj);
                            }
                        }
                    }

                    mobj = *(void**)((u8*)result + 0x8);
                    val = ((f32*)*(void**)(ptr + 0x8))[2];
                    if (mobj == NULL) {
                        __assert(&lbl_8047CC68, 0x2cc, &lbl_8047CC70);
                    }
                    if (*(u32*)((u8*)mobj + 0x14) & 0x20000) {
                        __assert(&lbl_8047CC68, 0x2cd, lbl_80270EE8);
                    }
                    *(f32*)((u8*)mobj + 0x24) = val;
                    if (!(*(u32*)((u8*)mobj + 0x14) & 0x02000000)) {
                        if (mobj != NULL) {
                            u32 fl;
                            s32 ck;
                            if (mobj == NULL) {
                                __assert(&lbl_8047CC68, 0x25d, &lbl_8047CC70);
                            }
                            fl = *(u32*)((u8*)mobj + 0x14);
                            ck = 0;
                            if (!(fl & 0x800000) && (fl & 0x40)) {
                                ck = 1;
                            }
                            if (!ck) {
                                fn_8019D620(mobj);
                            }
                        }
                    }
                    break;

                case 2:
                    mobj = *(void**)((u8*)result + 0x8);
                    val = *(f32*)*(void**)(ptr + 0x8);
                    if (mobj == NULL) {
                        __assert(&lbl_8047CC68, 0x412, &lbl_8047CC70);
                    }
                    *(f32*)((u8*)mobj + 0x1c) = *(f32*)((u8*)mobj + 0x1c) + val;
                    if (!(*(u32*)((u8*)mobj + 0x14) & 0x02000000)) {
                        if (mobj != NULL) {
                            u32 fl;
                            s32 ck;
                            if (mobj == NULL) {
                                __assert(&lbl_8047CC68, 0x25d, &lbl_8047CC70);
                            }
                            fl = *(u32*)((u8*)mobj + 0x14);
                            ck = 0;
                            if (!(fl & 0x800000) && (fl & 0x40)) {
                                ck = 1;
                            }
                            if (!ck) {
                                fn_8019D620(mobj);
                            }
                        }
                    }

                    mobj = *(void**)((u8*)result + 0x8);
                    val = ((f32*)*(void**)(ptr + 0x8))[1];
                    if (mobj == NULL) {
                        __assert(&lbl_8047CC68, 0x41d, &lbl_8047CC70);
                    }
                    *(f32*)((u8*)mobj + 0x20) = *(f32*)((u8*)mobj + 0x20) + val;
                    if (!(*(u32*)((u8*)mobj + 0x14) & 0x02000000)) {
                        if (mobj != NULL) {
                            u32 fl;
                            s32 ck;
                            if (mobj == NULL) {
                                __assert(&lbl_8047CC68, 0x25d, &lbl_8047CC70);
                            }
                            fl = *(u32*)((u8*)mobj + 0x14);
                            ck = 0;
                            if (!(fl & 0x800000) && (fl & 0x40)) {
                                ck = 1;
                            }
                            if (!ck) {
                                fn_8019D620(mobj);
                            }
                        }
                    }

                    mobj = *(void**)((u8*)result + 0x8);
                    val = ((f32*)*(void**)(ptr + 0x8))[2];
                    if (mobj == NULL) {
                        __assert(&lbl_8047CC68, 0x428, &lbl_8047CC70);
                    }
                    *(f32*)((u8*)mobj + 0x24) = *(f32*)((u8*)mobj + 0x24) + val;
                    if (!(*(u32*)((u8*)mobj + 0x14) & 0x02000000)) {
                        if (mobj != NULL) {
                            u32 fl;
                            s32 ck;
                            if (mobj == NULL) {
                                __assert(&lbl_8047CC68, 0x25d, &lbl_8047CC70);
                            }
                            fl = *(u32*)((u8*)mobj + 0x14);
                            ck = 0;
                            if (!(fl & 0x800000) && (fl & 0x40)) {
                                ck = 1;
                            }
                            if (!ck) {
                                fn_8019D620(mobj);
                            }
                        }
                    }
                    break;

                case 3:
                    mobj = *(void**)((u8*)result + 0x8);
                    val = *(f32*)*(void**)(ptr + 0x8);
                    if (mobj == NULL) {
                        __assert(&lbl_8047CC68, 0x412, &lbl_8047CC70);
                    }
                    *(f32*)((u8*)mobj + 0x1c) = *(f32*)((u8*)mobj + 0x1c) + val;
                    if (!(*(u32*)((u8*)mobj + 0x14) & 0x02000000)) {
                        if (mobj != NULL) {
                            u32 fl;
                            s32 ck;
                            if (mobj == NULL) {
                                __assert(&lbl_8047CC68, 0x25d, &lbl_8047CC70);
                            }
                            fl = *(u32*)((u8*)mobj + 0x14);
                            ck = 0;
                            if (!(fl & 0x800000) && (fl & 0x40)) {
                                ck = 1;
                            }
                            if (!ck) {
                                fn_8019D620(mobj);
                            }
                        }
                    }

                    mobj = *(void**)((u8*)result + 0x8);
                    val = ((f32*)*(void**)(ptr + 0x8))[1];
                    if (mobj == NULL) {
                        __assert(&lbl_8047CC68, 0x2b8, &lbl_8047CC70);
                    }
                    if (*(u32*)((u8*)mobj + 0x14) & 0x20000) {
                        __assert(&lbl_8047CC68, 0x2b9, lbl_80270EE8);
                    }
                    *(f32*)((u8*)mobj + 0x20) = val;
                    if (!(*(u32*)((u8*)mobj + 0x14) & 0x02000000)) {
                        if (mobj != NULL) {
                            u32 fl;
                            s32 ck;
                            if (mobj == NULL) {
                                __assert(&lbl_8047CC68, 0x25d, &lbl_8047CC70);
                            }
                            fl = *(u32*)((u8*)mobj + 0x14);
                            ck = 0;
                            if (!(fl & 0x800000) && (fl & 0x40)) {
                                ck = 1;
                            }
                            if (!ck) {
                                fn_8019D620(mobj);
                            }
                        }
                    }

                    mobj = *(void**)((u8*)result + 0x8);
                    val = ((f32*)*(void**)(ptr + 0x8))[2];
                    if (mobj == NULL) {
                        __assert(&lbl_8047CC68, 0x428, &lbl_8047CC70);
                    }
                    *(f32*)((u8*)mobj + 0x24) = *(f32*)((u8*)mobj + 0x24) + val;
                    if (!(*(u32*)((u8*)mobj + 0x14) & 0x02000000)) {
                        if (mobj != NULL) {
                            u32 fl;
                            s32 ck;
                            if (mobj == NULL) {
                                __assert(&lbl_8047CC68, 0x25d, &lbl_8047CC70);
                            }
                            fl = *(u32*)((u8*)mobj + 0x14);
                            ck = 0;
                            if (!(fl & 0x800000) && (fl & 0x40)) {
                                ck = 1;
                            }
                            if (!ck) {
                                fn_8019D620(mobj);
                            }
                        }
                    }
                    break;

                default:
                    break;
                }
                fn_800EE828(result);
            }
        }
        ptr += 0xc;
    } while (count-- != 0);
}
#endif

/* fn_800EE044 -- StoreResult | Size: 0x10 */
void fn_800EE044(void* result, void** dest) {
    if (result != NULL) {
        *dest = result;
    }
}

/* fn_800EE054 -- SetRenderMode | Size: 0x38 */
#if 0
asm void fn_800EE054(void) {
#include "src/game/gs_material_fn_800EE054.inc"
}
#else
void fn_800EE054(void* obj, u32 mode) {
    if (mode == 0) {
        fn_801C2A74(obj, 0x20000000);
    } else {
        fn_801C2A90(obj, 0x20000000);
    }
}
#endif

/* _modelGetEndFrame -- ConfigureFog | Size: 0x5C */
extern u8 lbl_8047CC90[7];
extern u8 lbl_8047CC98[5];
#if 0
asm void _modelGetEndFrame(void) {
#include "src/game/gs_material__modelGetEndFrame.inc"
}
#else
void _modelGetEndFrame(void* obj, f32* ptr) {
    f32 val;
    if (obj == NULL) {
        __assert(lbl_8047CC90, 0xab, lbl_8047CC98);
    }
    val = *(f32*)((u8*)obj + 0xc);
    if (val > *ptr) {
        *ptr = val;
    }
}
#endif

/* fn_800EE0E8 -- ConfigureScissor | Size: 0x68 */
extern u32 lbl_8047ABB0;
extern u32 lbl_8047ABAC;
#if 0
asm void fn_800EE0E8(void) {
#include "src/game/gs_material_fn_800EE0E8.inc"
}
#else
u32 fn_800EE0E8(void* entry) {
    lbl_8047ABB0 = 0;
    lbl_8047ABAC = (u32)-1;
    fn_801A3918(*(void**)((u8*)entry + 0x8), fn_800EE20C, 0);
    if (*(u32*)entry & 0x20000) {
        lbl_8047ABB0 -= 1;
    }
    return lbl_8047ABB0;
}
#endif

/* fn_800EE150 -- ApplyPEDescr | Size: 0xBC */
extern void* fn_800EE834(void);
extern u32 lbl_8047ABAC;
extern u32 lbl_8047ABB0;
extern u32 lbl_8047ABA8;
#if 0
asm void fn_800EE150(void) {
#include "src/game/gs_material_fn_800EE150.inc"
}
#else
void* fn_800EE150(void* entry, u32 param) {
    void* mobj;
    u32 max = param;
    void* result;
    if (*(u32*)entry & 0x20000) {
        max = param + 1;
    }
    lbl_8047ABAC = max;
    lbl_8047ABB0 = 0;
    lbl_8047ABA8 = 0;
    mobj = ((void*(*)(void*))fn_800E5188)(entry);
    if (max == 0) {
        lbl_8047ABA8 = (u32)mobj;
    } else {
        fn_801A3918(mobj, fn_800EE20C, 0);
        if (lbl_8047ABA8 == 0) {
            return NULL;
        }
    }
    result = fn_800EE834();
    if (result == NULL) {
        return NULL;
    }
    *(void**)((u8*)result + 0x4) = entry;
    *(u32*)((u8*)result + 0x8) = lbl_8047ABA8;
    *(u16*)((u8*)result + 0x2) = (u16)param;
    return result;
}
#endif

/* fn_800EE20C -- GetPEParam | Size: 0x20 */
extern u32 lbl_8047ABB0;
extern u32 lbl_8047ABAC;
extern u32 lbl_8047ABA8;
#if 0
asm void fn_800EE20C(void) {
#include "src/game/gs_material_fn_800EE20C.inc"
}
#else
void fn_800EE20C(u32 val) {
    u32 counter = lbl_8047ABB0;
    u32 max = lbl_8047ABAC;
    lbl_8047ABB0 = counter + 1;
    if (counter == max) {
        lbl_8047ABA8 = val;
    }
}
#endif

/* fn_800EE22C -- ResetBlendMode | Size: 0x5C */
extern void fn_800EE8F0(void);
extern u8  lbl_8047ABC4;
extern u32 lbl_8047ABD0;
extern u32 lbl_8047ABC8;
extern u32 lbl_8047ABCC;
#if 0
asm void fn_800EE22C(void) {
#include "src/game/gs_material_fn_800EE22C.inc"
}
#else
s32 fn_800EE22C(void* a, void* b) {
    if (a == b) {
        return 0;
    }
    lbl_8047ABC4 = 0;
    lbl_8047ABD0 = (u32)b;
    lbl_8047ABC8 = 0;
    lbl_8047ABCC = (u32)-1;
    fn_801A3918(a, fn_800EE8F0, 0);
    return (s32)lbl_8047ABCC;
}
#endif

/* fn_800EE288 -- Finalize | Size: 0x40 */
#if 0
asm void fn_800EE288(void) {
#include "src/game/gs_material_fn_800EE288.inc"
}
#else
void fn_800EE288(void* p) {
    void* tbl = *(void**)((u8*)p + 4);
    s32 i = 4;
    while (i-- != 0) {
        u32 val = *(u32*)((u8*)tbl + 0xe8);
        u16 key = *(u16*)((u8*)p + 2);
        if (val == (u32)key) {
            *(u32*)((u8*)tbl + 0xe4) = 0;
            *(u32*)((u8*)tbl + 0xe8) = (u32)-1;
            return;
        }
    }
}
#endif
#if 0
asm void fn_800EC134(void) {
#include "src/game/gs_material_fn_800EC134.inc"
}
#else
void fn_800EC134(void* entry) {
    fn_800ED1CC(entry);
}
#endif
#if 0
asm void fn_800ECA78(void) {
#include "src/game/gs_material_fn_800ECA78.inc"
}
#else
void fn_800ECA78(void* entry, f32 f1) {
    u32 r4 = *(u32*)entry;
    void* r3 = *(void**)((u8*)entry + 0x8);
    if (!(r4 & 0x4)) return;
    if (r4 & 0x80) {
        *(f32*)((u8*)entry + 0xc4) = f1 * *(f32*)((u8*)entry + 0xd8);
        *(f32*)((u8*)entry + 0xc8) = f1;
        ((void(*)(void*, s32, f32))fn_801A32A0)(*(void**)((u8*)entry + 0x10), 0x1cb, *(f32*)((u8*)entry + 0xc4));
        ((void(*)(void*, s32, f32))fn_801A32A0)(*(void**)((u8*)entry + 0x14), 0x1cb, *(f32*)((u8*)entry + 0xc8));
    } else {
        if (r4 & 0x20000) r3 = *(void**)((u8*)r3 + 0x10);
        *(f32*)((u8*)entry + 0x98) = f1;
        *(f32*)((u8*)entry + 0x9c) = f1;
        ((void(*)(void*, s32, f32))fn_801A32A0)(r3, 0x1cb, *(f32*)((u8*)entry + 0x9c));
    }
    {
        u32 flags = *(u32*)entry & ~0x5001u;
        *(u32*)entry = flags;
        flags = *(u32*)entry;
        if ((flags & 0x2000) && (flags & 0x8)) {
            r3 = *(void**)((u8*)entry + 0x8);
            if (flags & 0x20000) r3 = *(void**)((u8*)r3 + 0x10);
            *(f32*)((u8*)entry + 0xb0) = f1;
            *(f32*)((u8*)entry + 0xb4) = f1;
            ((void(*)(void*, s32, f32))fn_801A32A0)(r3, 0x634, *(f32*)((u8*)entry + 0xb4));
            *(u32*)entry &= ~0x8000u;
        }
    }
}
#endif
