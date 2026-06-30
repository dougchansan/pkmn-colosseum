#include "audio_shim.h"
#include "dvd_shim.h"
#include "field_motion_host.h"
#include "field_collision.h"
#include "gx_shim.h"
#include "gx_texture.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_jobj.h"
#include "hsd/hsd_memory.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_pobj.h"
#include "game/people/people.h"
#include "os_shim.h"
#include "pad_shim.h"
#include "pcport_font.h"
#include "pcport_window.h"
#include "real_content_host.h"
#include "thp_player.h"
#include "thp_audio.h"
#include "waveout_sink.h"
#include "bgm_host.h"
#include "musyx_wave.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "../third_party/stb_image.h"

#include <GLFW/glfw3.h>
#include <direct.h>   /* _chdir (host-only: locate the asset root at startup) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * Minimal bridge to one decompiled Dolphin SDK TU. This avoids pulling in
 * the full SDK header stack into the bootstrap main file.
 */
extern unsigned long CurrTvMode;
extern unsigned long VIGetTvFormat(void);
extern unsigned long __OSGetAppType(void);
extern void __OSSetAppType(unsigned long type);
extern void* TRK_memcpy(void* dst, const void* src, unsigned long n);
extern char* TRK_strcat(char* dst, const char* src);
extern unsigned long TRK_strlen(const char* s);
extern void GSgfxInit(unsigned int memSize, unsigned int fifoSize,
                      unsigned int mtxDepth, unsigned int lightCount,
                      unsigned int numBufs, unsigned int dlSize);
extern void GSgfx_BeginFrame(void);
extern void GSgfxSwapBuffers(unsigned int flag);
/* P-A engine-fibre spike (engine_spike.c). */
extern int RunFibreSelfTest(void);
extern int RunEngineSpike(GLFWwindow* window);
/* P-B engine scheduler boot (engine_boot.c). */
extern int RunSchedTest(void);
extern int RunEngineBoot(GLFWwindow* window);
extern unsigned int GSgfxGetFrameCount(void);
extern unsigned int GSgfxHostGetPreRetraceCount(void);
extern unsigned char GSgfxHostGetDrawDoneFlag(void);
extern void fn_800D9D68(unsigned int x1, unsigned int y1,
                        unsigned int x2, unsigned int y2);
extern void fn_800DAD10(void* obj);
extern void fn_801AA568(HSD_PObj* pobj);
extern void HSD_JObjResolveRefsAll(HSD_JObj* jobj, HSD_Joint* joint);
extern HSD_JObj* fn_801A0FBC(HSD_Joint* joint);
extern s32 fn_801A7D58(HSD_MObj* dst, HSD_MObj* src);
extern void MObjUpdateFunc(HSD_MObj* mobj, u32 type, HSD_ObjData* value);
extern void GSgfxHostClearPipelineState(unsigned int pipelineId);
extern void GSgfxHostSetPipelineBlend(unsigned int pipelineId,
                                      unsigned int type,
                                      unsigned int src_factor,
                                      unsigned int dst_factor,
                                      unsigned int op);
extern void GSgfxHostSetPipelineZ(unsigned int pipelineId,
                                  unsigned char compare_enable,
                                  unsigned int func,
                                  unsigned char update_enable);
extern void GSgfxHostSetPipelineAlphaCompare(unsigned int pipelineId,
                                             unsigned int comp0,
                                             unsigned char ref0,
                                             unsigned int op,
                                             unsigned int comp1,
                                             unsigned char ref1);
extern void GSgfxHostSetPipelineAlphaScale(unsigned int pipelineId,
                                           float alphaScale);
extern void GSgfxHostSetPipelineTexture(unsigned int pipelineId,
                                        const void* textureObject,
                                        unsigned char numTexGens,
                                        unsigned char tevMode,
                                        unsigned char textureCoordId,
                                        unsigned char textureMapId);
extern unsigned int GXHostGetLastSubmittedVertexCount(void);
extern unsigned int GXHostGetLastExpandedVertexCount(void);
extern unsigned int GXHostGetLastSubmittedPrimitive(void);

unsigned long CurrTvMode = 0;

#define PCPORT_WINDOW_WIDTH  640
#define PCPORT_WINDOW_HEIGHT 480
#define PCPORT_WINDOW_FRAMES 120
#define PCPORT_GSGFX_SWAPS   3
#define PCPORT_GX_SMOKE_SAMPLE_X 320
#define PCPORT_GX_SMOKE_SAMPLE_Y 240
#define PCPORT_GX_SCISSOR_X   160
#define PCPORT_GX_SCISSOR_Y   120
#define PCPORT_GX_SCISSOR_W   320
#define PCPORT_GX_SCISSOR_H   240
#define PCPORT_GX_SCISSOR_X2  (PCPORT_GX_SCISSOR_X + PCPORT_GX_SCISSOR_W - 1)
#define PCPORT_GX_SCISSOR_Y2  (PCPORT_GX_SCISSOR_Y + PCPORT_GX_SCISSOR_H - 1)
#define PCPORT_GX_OUTSIDE_X   40
#define PCPORT_GX_OUTSIDE_Y   40
#define PCPORT_REAL_CONTENT_ARCHIVE "orig/GC6E01/disc/files/topmenu.fsys"
#define PCPORT_REAL_CONTENT_MEMBER  "menu_bg00"

/* Static "Pokemon Colosseum" title wordmark: title.fsys -> member logo_demo
 * (HSD archive), texture index 01 = RGBA8 540x224 at decompressed-archive
 * offset 0xAC2E0. Drawn as an alpha-blended 2D overlay over menu_bg00. */
#define PCPORT_LOGO_ARCHIVE      "orig/GC6E01/disc/files/title.fsys"
#define PCPORT_LOGO_MEMBER       "logo_demo"
#define PCPORT_LOGO_IMAGE_OFFSET 0xAC2E0u
#define PCPORT_LOGO_WIDTH        540
#define PCPORT_LOGO_HEIGHT       224

/* Title-screen 2D sprite sheet in topmenu.fsys: copyright lines + PRESS START
 * (RGB5A3 428x122, a raw 0x80-header texture, not an HSD archive). UV bands:
 * copyright block v0.016..0.549, PRESS START (teal) v0.574..0.721. */
#define PCPORT_TITLE_PRESS_MEMBER "menu_018"

/* Main-menu panel sprite in topmenu.fsys: STORY MODE (Continue / New Game),
 * BATTLE MODE (Colosseum Battle / Battle Now), OPTIONS. Raw 0x80-header RGBA8
 * 276x574 sprite (same format as menu_018). Shown after START is pressed on the
 * title screen -- the first interactive screen transition in the port. */
#define PCPORT_MAIN_MENU_MEMBER   "menu_033"

/* Main-menu chrome sprite sheet in topmenu.fsys (raw 0x80-header RGBA8 774x139):
 * the pointing-hand cursor, the blue Quit button, grey buttons + the description
 * box top edge. The hand cursor (UV u 0.19..0.28, v 0.66..0.99) is drawn to the
 * left of the selected item; the Quit button is UV u 0.0..0.16, v 0.40..0.74. */
#define PCPORT_TOPMENU_CHROME_MEMBER "menu_032"

/* Main-menu blue-swirl background: topmenu.fsys member menu_bg00 (HSD archive),
 * texture index 00 = CMPR 640x480 at decompressed-archive offset 0x73C0. Baked
 * once and drawn full-screen behind the cards (covers the green GSgfx EFB clear
 * with the real artwork instead of the flat-blue stand-in). */
#define PCPORT_MENU_BG_MEMBER  "menu_bg00"
#define PCPORT_MENU_BG_OFFSET  0x73C0u
#define PCPORT_MENU_BG_WIDTH   640
#define PCPORT_MENU_BG_HEIGHT  480

/* The title-screen 3D scene (desert/ruins environment + the logo, all one HSD
 * archive) is title.fsys:logo_demo. Its scene_data layout is byte-compatible
 * with menu_bg00 and it carries a real perspective camera, so RunMenuScene
 * renders it with just this archive/member swap. */
#define PCPORT_TITLE_SCENE_ARCHIVE "orig/GC6E01/disc/files/title.fsys"
#define PCPORT_TITLE_SCENE_MEMBER  "logo_demo"
#define PCPORT_TITLE_SCENE_ROOT_JOINT 0x26108u
#define PCPORT_TITLE_SCENE_LOGO_DOBJ 40u
#define PCPORT_WORLDMAP_ARCHIVE    "orig/GC6E01/disc/files/world_map.fsys"

/* Sky/sand horizon backdrop texture inside title.fsys:logo_demo (CMPR 512x256
 * at archive offset 0x14A8E0): blue sky + clouds fading to tan sand. Drawn as a
 * full-screen 2D backdrop -- the reliable stand-in for the title scene's 3D
 * environment, whose animated-demo geometry does not render statically yet. */
#define PCPORT_TITLE_SKY_OFFSET 0x14A8E0u
#define PCPORT_TITLE_SKY_WIDTH  512
#define PCPORT_TITLE_SKY_HEIGHT 256
#define PCPORT_PDA_MENU_ARCHIVE     "orig/GC6E01/disc/files/pda_menu.fsys"
#define PCPORT_PDA2_BG_MEMBER       "pda2_bg"
#define PCPORT_SERIALIZED_JOINT_SIZE 0x40u
#define PCPORT_SERIALIZED_DOBJ_SIZE  0x10u
#define PCPORT_SERIALIZED_POBJ_SIZE  0x18u
#define PCPORT_REAL_MATERIAL_PIPELINE 1u
#define PCPORT_REAL_TEXTURED_PIPELINE 2u
#define PCPORT_REAL_SIBLING_TEXTURED_PIPELINE 3u
#define PCPORT_RENDER_NO_ZUPDATE 0x20000000u
#define PCPORT_RENDER_XLU        0x40000000u
#define PCPORT_TEXTURED_JOINT_OFFSET 0x6FE8u
#define PCPORT_TEXTURED_DOBJ_OFFSET  0x3B98u
#define PCPORT_TEXTURED_MOBJ_OFFSET  0x3900u
#define PCPORT_TEXTURED_POBJ_OFFSET  0x3B80u
#define PCPORT_SIBLING_JOINT_OFFSET  0x70E8u
#define PCPORT_SIBLING_DOBJ_OFFSET   0x6F98u

static int g_pcBattleSuppressControlPObjs = 0;
static unsigned int g_pcBattleMaterialLogBudget = 0;
static int g_pcBattleRenderSkin = 0;
/* Battle pkx (Pokemon) meshes store their vertices in MODEL space (bind pose),
 * NOT joint-local like the field character (ken_b1). Proven empirically: rendering
 * them with NO skin matrix (raw verts placed by the actor model matrix) yields a
 * clean, correct model, while the rigid/envelope palette scatters them. Until the
 * envelope-skin palette is fixed to resolve to identity-at-rest, submit pkx verts
 * model-space (the per-actor model matrix already places them in the scene). Scoped
 * to the pkx render loop so the (joint-local) field character path is unaffected. */
static int g_pcBattleModelSpaceVerts = 0;
/* Set per-pobj (whole mesh piece) inside RenderSkinnedPObj: 1 = this pobj's verts
 * are joint-local and must be placed by its bone matrix (e.g. Shedinja's halo);
 * 0 = model-space pass-through. Decided once per pobj so a triangle's corners are
 * never split across two transforms (which scattered the wings/Gulpin). */
static int g_skinPobjPlaceByBone = 0;
/* Cap (model units) for the halo-lift Y. The joint translations live in a larger
 * scale than the model-space body verts (body tops out ~y=5 but the halo bone is
 * y~15), so lifting to the raw bone Y floats the halo far above the head. Clamp it
 * to sit just above the body. Tunable live via PCPORT_HALO_MAXY. */
static f32 g_haloLiftMaxY = 7.0f;
/* The model's root-joint scale (read from game data). A joint-local lifted piece's
 * world position is divided by this to land in the body's (model-space) frame. */
static f32 g_haloRootScale = 1.0f;
/* Data-derived skeleton<->model scale (boneY/vertY of the body pieces), used to map
 * a lifted joint-local piece (halo) back into the body's frame. ~2.0 for Shedinja. */
static f32 g_haloModelRatio = 2.0f;
#define PCPORT_SIBLING_MOBJ_OFFSET   0x39CCu
#define PCPORT_SIBLING_POBJ_OFFSET   0x6F80u
#define PCPORT_PDA2_BG_OBJECT0_JOINT_OFFSET 0x22E8u
#define PCPORT_PDA2_BG_OBJECT0_DOBJ_OFFSET  0x1FB8u
#define PCPORT_PDA2_BG_OBJECT0_MOBJ_OFFSET  0x1A2Cu
#define PCPORT_PDA2_BG_OBJECT0_POBJ_OFFSET  0x1FA0u
#define PCPORT_PDA2_BG_OBJECT1_JOINT_OFFSET 0x2328u
#define PCPORT_PDA2_BG_OBJECT1_DOBJ_OFFSET  0x2298u
#define PCPORT_PDA2_BG_OBJECT1_MOBJ_OFFSET  0x1AECu
#define PCPORT_PDA2_BG_OBJECT1_POBJ_OFFSET  0x2280u

typedef struct {
    unsigned int reserved;
    const void* displayList;
    unsigned int displayListSize;
    unsigned int pipelineId;
    unsigned int totalVerts;
    unsigned int totalPrims;
} PCPortGSDrawObject;

static const float g_sceneLikePositions[][3] = {
    { -0.78f, -0.58f, 0.0f },
    {  0.78f, -0.58f, 0.0f },
    {  0.78f,  0.58f, 0.0f },
    { -0.78f,  0.58f, 0.0f },
    { -0.68f, -0.48f, 0.0f },
    {  0.68f, -0.48f, 0.0f },
    {  0.68f,  0.48f, 0.0f },
    { -0.68f,  0.48f, 0.0f },
    { -0.68f,  0.20f, 0.0f },
    {  0.68f,  0.20f, 0.0f },
    {  0.68f,  0.48f, 0.0f },
    { -0.68f,  0.48f, 0.0f }
};

static const unsigned char g_sceneLikeColors[][4] = {
    { 0x1B, 0x28, 0x3A, 0xFF },
    { 0x1B, 0x28, 0x3A, 0xFF },
    { 0x1B, 0x28, 0x3A, 0xFF },
    { 0x1B, 0x28, 0x3A, 0xFF },
    { 0xE6, 0xDC, 0xBC, 0xFF },
    { 0xE6, 0xDC, 0xBC, 0xFF },
    { 0xE6, 0xDC, 0xBC, 0xFF },
    { 0xE6, 0xDC, 0xBC, 0xFF },
    { 0xD0, 0x7A, 0x20, 0xFF },
    { 0xD0, 0x7A, 0x20, 0xFF },
    { 0xD0, 0x7A, 0x20, 0xFF },
    { 0xD0, 0x7A, 0x20, 0xFF }
};

static const float g_sceneLikeTexcoords[][2] = {
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f },
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f },
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f }
};

static const unsigned char g_sceneLikeDisplayList[] = {
    0x80, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
    0x02, 0x02, 0x02, 0x03, 0x03, 0x03,
    0x80, 0x00, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x07, 0x07, 0x07,
    0x80, 0x00, 0x04, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09,
    0x0A, 0x0A, 0x0A, 0x0B, 0x0B, 0x0B
};

static const PCPortGSDrawObject g_sceneLikeDrawObject = {
    0,
    g_sceneLikeDisplayList,
    sizeof(g_sceneLikeDisplayList),
    0,
    (unsigned int)(sizeof(g_sceneLikePositions) /
                   sizeof(g_sceneLikePositions[0])),
    3
};

static HSD_VtxDescList g_sceneLikePObjVerts[] = {
    { GX_VA_POS,  GX_INDEX8, GX_POS_XYZ, GX_F32,   0,
      sizeof(g_sceneLikePositions[0]), (void*)g_sceneLikePositions },
    { GX_VA_CLR0, GX_INDEX8, GX_CLR_RGBA, GX_RGBA8, 0,
      sizeof(g_sceneLikeColors[0]), (void*)g_sceneLikeColors },
    { GX_VA_TEX0, GX_INDEX8, GX_TEX_ST,  GX_F32,   0,
      sizeof(g_sceneLikeTexcoords[0]), (void*)g_sceneLikeTexcoords },
    { GX_VA_NULL, GX_NONE,   0,          0,        0, 0, NULL }
};

static void ConfigureSceneLikeDisplayListState(void) {
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
    GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
    GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetArray(GX_VA_POS, (void*)g_sceneLikePositions,
               sizeof(g_sceneLikePositions[0]));
    GXSetArray(GX_VA_CLR0, (void*)g_sceneLikeColors,
               sizeof(g_sceneLikeColors[0]));
    GXSetArray(GX_VA_TEX0, (void*)g_sceneLikeTexcoords,
               sizeof(g_sceneLikeTexcoords[0]));
}

static void InitSceneLikePObj(HSD_PObj* pobj) {
    memset(pobj, 0, sizeof(*pobj));
    pobj->verts = g_sceneLikePObjVerts;
    pobj->n_display = (u16)sizeof(g_sceneLikeDisplayList);
    pobj->display = (u8*)g_sceneLikeDisplayList;
}

static int ReadBackbufferPixelAt(int x, int y, unsigned char pixel[4]) {
    glReadBuffer(GL_BACK);
    glReadPixels(x,
                 y,
                 1,
                 1,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 pixel);
    return pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0;
}

/* Dump an RGBA framebuffer to a 24-bit BMP at an explicit path. GL's bottom-up
 * origin matches BMP's, so rows are written as-is. (PCPORT-only.) */
static void DumpFramebufferBMPTo(const unsigned char* rgba, int w, int h,
                                 const char* path) {
    FILE* f;
    int rowsize, imgsize, x, y;
    unsigned char hdr[54];
    unsigned char* row;
    unsigned int filesize;
    if (path == NULL || rgba == NULL) return;
    f = fopen(path, "wb");
    if (f == NULL) return;
    rowsize = (w * 3 + 3) & ~3;
    imgsize = rowsize * h;
    filesize = 54u + (unsigned int)imgsize;
    memset(hdr, 0, sizeof(hdr));
    hdr[0] = 'B'; hdr[1] = 'M';
    hdr[2] = filesize & 0xff; hdr[3] = (filesize >> 8) & 0xff;
    hdr[4] = (filesize >> 16) & 0xff; hdr[5] = (filesize >> 24) & 0xff;
    hdr[10] = 54; hdr[14] = 40;
    hdr[18] = w & 0xff; hdr[19] = (w >> 8) & 0xff;
    hdr[22] = h & 0xff; hdr[23] = (h >> 8) & 0xff;
    hdr[26] = 1; hdr[28] = 24;
    fwrite(hdr, 1, 54, f);
    row = (unsigned char*)malloc(rowsize);
    for (y = 0; y < h; y++) {
        memset(row, 0, rowsize);
        for (x = 0; x < w; x++) {
            const unsigned char* p = rgba + ((size_t)y * w + x) * 4;
            row[x * 3 + 0] = p[2]; row[x * 3 + 1] = p[1]; row[x * 3 + 2] = p[0];
        }
        fwrite(row, 1, rowsize, f);
    }
    free(row);
    fclose(f);
}

/* Dump to the path in env PCPORT_DUMP (the single end-of-run screenshot). */
static void DumpFramebufferBMP(const unsigned char* rgba, int w, int h) {
    DumpFramebufferBMPTo(rgba, w, h, getenv("PCPORT_DUMP"));
}

/* Read the back buffer and write it straight to `path` (within-run sequence
 * capture, used to verify the title's drifting animations headlessly). */
static void DumpBackbufferTo(const char* path) {
    unsigned char* px;
    if (path == NULL) return;
    px = (unsigned char*)malloc((size_t)PCPORT_WINDOW_WIDTH *
                                (size_t)PCPORT_WINDOW_HEIGHT * 4u);
    if (px == NULL) return;
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, PCPORT_WINDOW_WIDTH, PCPORT_WINDOW_HEIGHT,
                 GL_RGBA, GL_UNSIGNED_BYTE, px);
    DumpFramebufferBMPTo(px, PCPORT_WINDOW_WIDTH, PCPORT_WINDOW_HEIGHT, path);
    free(px);
}

static unsigned char* ReadBackbufferImage(void) {
    size_t pixelCount = (size_t)PCPORT_WINDOW_WIDTH * (size_t)PCPORT_WINDOW_HEIGHT;
    size_t bufferSize = pixelCount * 4u;
    unsigned char* pixels = (unsigned char*)malloc(bufferSize);

    if (pixels == NULL) {
        return NULL;
    }

    glReadBuffer(GL_BACK);
    glReadPixels(0,
                 0,
                 PCPORT_WINDOW_WIDTH,
                 PCPORT_WINDOW_HEIGHT,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 pixels);
    DumpFramebufferBMP(pixels, PCPORT_WINDOW_WIDTH, PCPORT_WINDOW_HEIGHT);
    return pixels;
}

static unsigned int CountFramebufferDiffPixels(const unsigned char* before,
                                               const unsigned char* after) {
    size_t pixelCount = (size_t)PCPORT_WINDOW_WIDTH * (size_t)PCPORT_WINDOW_HEIGHT;
    unsigned int diffCount = 0;
    size_t i;

    if (before == NULL || after == NULL) {
        return 0;
    }

    for (i = 0; i < pixelCount; ++i) {
        size_t base = i * 4u;

        if (before[base + 0] != after[base + 0] ||
            before[base + 1] != after[base + 1] ||
            before[base + 2] != after[base + 2] ||
            before[base + 3] != after[base + 3]) {
            ++diffCount;
        }
    }

    return diffCount;
}

static void TransformBoundsByMtx(const f32 minBounds[3],
                                 const f32 maxBounds[3],
                                 const f32 modelMatrix[3][4],
                                 f32 outMin[3],
                                 f32 outMax[3]) {
    u32 corner;

    for (corner = 0; corner < 8u; ++corner) {
        f32 x = (corner & 1u) != 0u ? maxBounds[0] : minBounds[0];
        f32 y = (corner & 2u) != 0u ? maxBounds[1] : minBounds[1];
        f32 z = (corner & 4u) != 0u ? maxBounds[2] : minBounds[2];
        f32 transformed[3];
        u32 axis;

        transformed[0] = (modelMatrix[0][0] * x) +
                         (modelMatrix[0][1] * y) +
                         (modelMatrix[0][2] * z) +
                         modelMatrix[0][3];
        transformed[1] = (modelMatrix[1][0] * x) +
                         (modelMatrix[1][1] * y) +
                         (modelMatrix[1][2] * z) +
                         modelMatrix[1][3];
        transformed[2] = (modelMatrix[2][0] * x) +
                         (modelMatrix[2][1] * y) +
                         (modelMatrix[2][2] * z) +
                         modelMatrix[2][3];

        for (axis = 0; axis < 3u; ++axis) {
            if (corner == 0u || transformed[axis] < outMin[axis]) {
                outMin[axis] = transformed[axis];
            }
            if (corner == 0u || transformed[axis] > outMax[axis]) {
                outMax[axis] = transformed[axis];
            }
        }
    }
}

static void ConcatAffineMtx(const f32 a[3][4],
                            const f32 b[3][4],
                            f32 out[3][4]) {
    f32 result[3][4];
    u32 row;
    u32 col;

    for (row = 0; row < 3u; ++row) {
        for (col = 0; col < 3u; ++col) {
            result[row][col] = (a[row][0] * b[0][col]) +
                               (a[row][1] * b[1][col]) +
                               (a[row][2] * b[2][col]);
        }

        result[row][3] = (a[row][0] * b[0][3]) +
                         (a[row][1] * b[1][3]) +
                         (a[row][2] * b[2][3]) +
                         a[row][3];
    }

    memcpy(out, result, sizeof(result));
}

static int ReadBackbufferPixel(unsigned char pixel[4]) {
    return ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                                 PCPORT_GX_SMOKE_SAMPLE_Y,
                                 pixel);
}

static void ClearBackbuffer(float r, float g, float b) {
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void ClearDepthBuffer(void) {
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
    glDisable(GL_SCISSOR_TEST);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
}

/* For watching headless/capped visual tests: if PCPORT_HOLD_SECS is set, keep
 * the window up (last rendered frame stays on the front buffer) for that many
 * wall-clock seconds before the scene returns, so a human can see the result.
 * Pumps events so the window stays responsive; closeable early. */
static void HoldWindowOpen(GLFWwindow* window) {
    const char* hs = getenv("PCPORT_HOLD_SECS");
    double secs, start;
    if (hs == NULL || window == NULL) {
        return;
    }
    secs = atof(hs);
    if (secs <= 0.0) {
        return;
    }
    printf("[pcport] holding window open %.1fs (PCPORT_HOLD_SECS)\n", secs);
    start = glfwGetTime();
    while (glfwGetTime() - start < secs) {
        if (glfwWindowShouldClose(window)) {
            break;
        }
        glfwPollEvents();
    }
}

static void LoadIdentityGXState(void) {
    static Mtx44 projection = {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f }
    };
    static Mtx modelView = {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f }
    };

    GXSetViewport(0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f);
    GXSetProjection(projection, GX_ORTHOGRAPHIC);
    GXLoadPosMtxImm(modelView, 0);
    GXSetCullMode(GX_CULL_NONE);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_COPY);
}

static void SubmitFullScreenGXQuad(unsigned char r,
                                   unsigned char g,
                                   unsigned char b,
                                   unsigned char a) {
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);

    GXColor4u8(r, g, b, a);
    GXPosition3f32(-1.0f, -1.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);

    GXColor4u8(r, g, b, a);
    GXPosition3f32(1.0f, -1.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);

    GXColor4u8(r, g, b, a);
    GXPosition3f32(1.0f, 1.0f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);

    GXColor4u8(r, g, b, a);
    GXPosition3f32(-1.0f, 1.0f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);

    GXEnd();
}

/* Draw the static title logo as an alpha-blended 2D overlay quad, composited
 * on top of whatever 3D scene was just rendered. Resets all the per-frame 3D
 * camera state (viewport/scissor/projection/modelview/depth) to a full-screen
 * orthographic identity so the quad is neither clipped to the camera scissor
 * nor depth-killed against the background. */
/* Set up the full-screen 2D orthographic overlay state once per frame: reset
 * the per-frame 3D camera viewport/scissor/projection/modelview, disable depth,
 * enable alpha blend, and configure a single textured (MODULATE) TEV stage so
 * the immediate path samples the bound texture. */
static void BeginMenuOverlay(void) {
    static Mtx44 orthoProjection = {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f }
    };
    static Mtx identityModelView = {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f }
    };

    GXSetViewport(0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f);
    GXSetScissor(0u, 0u, 640u, 480u);
    GXSetProjection(orthoProjection, GX_ORTHOGRAPHIC);
    GXLoadPosMtxImm(identityModelView, 0);
    GXSetCullMode(GX_CULL_NONE);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
    GXSetNumTexGens(1);
    GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXHostSetVertexAlphaScale(1.0f);
    /* 2D overlays (sky/logo/PRESS START/copyright) are unlit and full-bright. */
    GXHostSetLightingEnabled(GX_FALSE);
}

/* Draw a textured quad at a screen-space rectangle (origin top-left, 640x480),
 * sampling the texture sub-rect [u0,v0]-[u1,v1]. v increases downward so the
 * image renders upright. Call BeginMenuOverlay() first. White vertex colour
 * passes the (MODULATE) texel through unchanged. */
static void DrawTexturedScreenRect(GXTexObj* tex,
                                   f32 sx, f32 sy, f32 sw, f32 sh,
                                   f32 u0, f32 v0, f32 u1, f32 v1) {
    f32 ndcL = ((sx) / 640.0f) * 2.0f - 1.0f;
    f32 ndcR = ((sx + sw) / 640.0f) * 2.0f - 1.0f;
    f32 ndcT = 1.0f - ((sy) / 480.0f) * 2.0f;
    f32 ndcB = 1.0f - ((sy + sh) / 480.0f) * 2.0f;

    GXLoadTexObj(tex, GX_TEXMAP0);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);

    GXColor4u8(255, 255, 255, 255);
    GXPosition3f32(ndcL, ndcB, 0.0f);
    GXTexCoord2f32(u0, v1);

    GXColor4u8(255, 255, 255, 255);
    GXPosition3f32(ndcR, ndcB, 0.0f);
    GXTexCoord2f32(u1, v1);

    GXColor4u8(255, 255, 255, 255);
    GXPosition3f32(ndcR, ndcT, 0.0f);
    GXTexCoord2f32(u1, v0);

    GXColor4u8(255, 255, 255, 255);
    GXPosition3f32(ndcL, ndcT, 0.0f);
    GXTexCoord2f32(u0, v0);

    GXEnd();
}

/* Like DrawTexturedScreenRect but with a vertical alpha gradient: the top edge
 * is modulated by aTop, the bottom edge by aBottom (linearly interpolated
 * between). Lets a scrolling textured band (drifting clouds / sand-wind) feather
 * into whatever was drawn beneath it instead of ending on a hard seam. The
 * sampled texture's own alpha is multiplied by this gradient (MODULATE), so an
 * opaque sky texture takes the gradient directly and a low-alpha wisp texture
 * stays subtle. Call BeginMenuOverlay() first. */
static void DrawTexturedScreenRectA(GXTexObj* tex,
                                    f32 sx, f32 sy, f32 sw, f32 sh,
                                    f32 u0, f32 v0, f32 u1, f32 v1,
                                    u8 aTop, u8 aBottom) {
    f32 ndcL = ((sx) / 640.0f) * 2.0f - 1.0f;
    f32 ndcR = ((sx + sw) / 640.0f) * 2.0f - 1.0f;
    f32 ndcT = 1.0f - ((sy) / 480.0f) * 2.0f;
    f32 ndcB = 1.0f - ((sy + sh) / 480.0f) * 2.0f;

    GXLoadTexObj(tex, GX_TEXMAP0);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXColor4u8(255, 255, 255, aBottom);
    GXPosition3f32(ndcL, ndcB, 0.0f);
    GXTexCoord2f32(u0, v1);
    GXColor4u8(255, 255, 255, aBottom);
    GXPosition3f32(ndcR, ndcB, 0.0f);
    GXTexCoord2f32(u1, v1);
    GXColor4u8(255, 255, 255, aTop);
    GXPosition3f32(ndcR, ndcT, 0.0f);
    GXTexCoord2f32(u1, v0);
    GXColor4u8(255, 255, 255, aTop);
    GXPosition3f32(ndcL, ndcT, 0.0f);
    GXTexCoord2f32(u0, v0);
    GXEnd();
}

/* Synthesise a tileable "sand-wind" wisp texture: faint sandy horizontal streaks
 * on a transparent ground, for the drifting desert-wind overlay on the title.
 * The real game's wind is a particle/haze effect with no single source sprite,
 * so this is a self-contained procedural stand-in. Horizontal wavenumbers are
 * integers so the pattern tiles seamlessly under a GX_REPEAT scroll; a sin
 * vertical envelope fades it out at the top/bottom edges. Returns 1 on success. */
static int BuildSandWindTexture(GXTexObj* tex) {
    enum { WIND_W = 256, WIND_H = 64 };
    const f32 kPi = 3.14159265358979323846f;
    u8* px;
    int x;
    int y;
    const char* wi = getenv("PCPORT_WIND_INTENSITY");
    /* Peak wisp alpha. 30 was "barely visible"; ~90 reads as drifting blowing
     * sand over the desert (still translucent, not a sandstorm). */
    f32 windMax = (wi != NULL) ? (f32)atof(wi) : 90.0f;

    px = (u8*)malloc((size_t)WIND_W * WIND_H * 4u);
    if (px == NULL) {
        return 0;
    }
    for (y = 0; y < WIND_H; ++y) {
        f32 fy = (f32)y / (f32)(WIND_H - 1);
        f32 env = sinf(kPi * fy);          /* 0 at edges, 1 mid -> soft band */
        for (x = 0; x < WIND_W; ++x) {
            f32 fx = (f32)x / (f32)WIND_W;  /* 0..1, period = full width */
            f32 n;
            f32 a;
            int o = (y * WIND_W + x) * 4;

            /* A few integer-wavenumber sines, phase-shifted by height, give long
             * thin diagonal wisps that vary along their length. */
            n  = sinf(2.0f * kPi * (2.0f * fx) + 1.3f + 2.0f * fy);
            n += 0.6f * sinf(2.0f * kPi * (5.0f * fx) + 4.1f - 1.5f * fy);
            n += 0.4f * sinf(2.0f * kPi * (9.0f * fx) + 0.7f + 3.0f * fy);
            a = n * 0.25f + 0.5f;           /* ~0..1 */
            if (a < 0.0f) { a = 0.0f; }
            if (a > 1.0f) { a = 1.0f; }
            a = a * a;                      /* soft wispy gaps (not too sharp) */
            a *= env;

            px[o + 0] = 240;                /* warm pale sand */
            px[o + 1] = 228;
            px[o + 2] = 198;
            px[o + 3] = (u8)(a * windMax);  /* drifting blowing-sand intensity */
        }
    }
    memset(tex, 0, sizeof(*tex));
    GXHostInitTexObjRGBA8(tex, px, WIND_W, WIND_H, GX_REPEAT, GX_CLAMP);
    free(px);
    return 1;
}

/* Optional cloud diagnostic: roll an RGBA image left-to-right and heal the
 * center seam before GX_REPEAT scrolling. This is not the default title path;
 * the retail sky texture reads cleaner without the synthesized center band. */
static void MakeSeamlessHoriz(u8* px, int w, int h) {
    int half = w / 2;
    int band = 28;            /* heal half-width (px) around the central seam */
    int x;
    int y;
    int c;
    u8* tmp;

    if (px == NULL || w < 4 || h < 1) {
        return;
    }
    if (band > half - 1) { band = half - 1; }
    tmp = (u8*)malloc((size_t)w * h * 4u);
    if (tmp == NULL) {
        return;
    }
    for (y = 0; y < h; ++y) {
        for (x = 0; x < w; ++x) {
            int sx = (x + half) % w;
            for (c = 0; c < 4; ++c) {
                tmp[(y * w + x) * 4 + c] = px[(y * w + sx) * 4 + c];
            }
        }
    }
    /* Linear-blend the central band [half-band, half+band) between its two
     * endpoints, replacing the hard seam with a smooth ramp. */
    for (y = 0; y < h; ++y) {
        for (c = 0; c < 4; ++c) {
            int lo = tmp[(y * w + (half - band)) * 4 + c];
            int hi = tmp[(y * w + (half + band - 1)) * 4 + c];
            for (x = half - band; x < half + band; ++x) {
                f32 t = (f32)(x - (half - band)) / (f32)(2 * band - 1);
                tmp[(y * w + x) * 4 + c] = (u8)((f32)lo + ((f32)hi - (f32)lo) * t);
            }
        }
    }
    memcpy(px, tmp, (size_t)w * h * 4u);
    free(tmp);
}

/* Draw a solid-colour screen-space rect with no texture (GX_PASSCLR passes the
 * vertex colour straight through). Used for the menu backdrop that covers the
 * green GSgfx EFB clear, and as a cursor/highlight primitive. Restores the
 * textured MODULATE TEV op afterwards so following textured draws are unaffected.
 * Call BeginMenuOverlay() first (alpha blend + ortho state). */
static void DrawSolidScreenRect(f32 sx, f32 sy, f32 sw, f32 sh,
                                u8 r, u8 g, u8 b, u8 a) {
    f32 ndcL = ((sx) / 640.0f) * 2.0f - 1.0f;
    f32 ndcR = ((sx + sw) / 640.0f) * 2.0f - 1.0f;
    f32 ndcT = 1.0f - ((sy) / 480.0f) * 2.0f;
    f32 ndcB = 1.0f - ((sy + sh) / 480.0f) * 2.0f;

    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXColor4u8(r, g, b, a);
    GXPosition3f32(ndcL, ndcB, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXColor4u8(r, g, b, a);
    GXPosition3f32(ndcR, ndcB, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXColor4u8(r, g, b, a);
    GXPosition3f32(ndcR, ndcT, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXColor4u8(r, g, b, a);
    GXPosition3f32(ndcL, ndcT, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXEnd();

    GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
}

/* Lazily-built ASCII font atlas: white glyphs with coverage alpha from
 * pcport_font.h, uploaded once as an RGBA texture. The port has no native glyph
 * renderer, so this gives reusable text for the menu/save-prompt/etc. */
static GXTexObj g_fontTex;
static int g_fontReady = 0;

static void EnsureFontAtlas(void) {
    int n;
    int i;
    u8* rgba;

    if (g_fontReady) {
        return;
    }
    n = PCPORT_FONT_ATLAS_W * PCPORT_FONT_ATLAS_H;
    rgba = (u8*)malloc((size_t)n * 4u);
    if (rgba == NULL) {
        return;
    }
    for (i = 0; i < n; ++i) {
        rgba[i * 4 + 0] = 255;
        rgba[i * 4 + 1] = 255;
        rgba[i * 4 + 2] = 255;
        rgba[i * 4 + 3] = kPcportFontAlpha[i];
    }
    memset(&g_fontTex, 0, sizeof(g_fontTex));
    GXHostInitTexObjRGBA8(&g_fontTex, rgba,
                          PCPORT_FONT_ATLAS_W, PCPORT_FONT_ATLAS_H,
                          GX_CLAMP, GX_CLAMP);
    free(rgba);
    g_fontReady = 1;
}

/* Draw one line of ASCII text at screen (x,y), each glyph gw x gh px, tinted
 * (r,g,b,a) via MODULATE against the white atlas (monospace advance = gw).
 * Call BeginMenuOverlay() + EnsureFontAtlas() first. */
static void DrawTextScreen(f32 x, f32 y, f32 gw, f32 gh,
                           u8 r, u8 g, u8 b, u8 a, const char* s) {
    f32 cx = x;
    f32 du = (f32)PCPORT_FONT_CELL_W / (f32)PCPORT_FONT_ATLAS_W;
    f32 dv = (f32)PCPORT_FONT_CELL_H / (f32)PCPORT_FONT_ATLAS_H;

    if (!g_fontReady || s == NULL) {
        return;
    }
    GXLoadTexObj(&g_fontTex, GX_TEXMAP0);
    for (; *s != '\0'; ++s) {
        unsigned char ch = (unsigned char)*s;
        int idx;
        f32 u0, v0, u1, v1, ndcL, ndcR, ndcT, ndcB;

        if (ch == ' ' || ch < (unsigned char)PCPORT_FONT_FIRST ||
            ch > (unsigned char)PCPORT_FONT_LAST) {
            cx += gw;
            continue;
        }
        idx = (int)ch - PCPORT_FONT_FIRST;
        u0 = (f32)(idx % PCPORT_FONT_COLS) * du;
        v0 = (f32)(idx / PCPORT_FONT_COLS) * dv;
        u1 = u0 + du;
        v1 = v0 + dv;
        ndcL = (cx / 640.0f) * 2.0f - 1.0f;
        ndcR = ((cx + gw) / 640.0f) * 2.0f - 1.0f;
        ndcT = 1.0f - (y / 480.0f) * 2.0f;
        ndcB = 1.0f - ((y + gh) / 480.0f) * 2.0f;

        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        GXColor4u8(r, g, b, a); GXPosition3f32(ndcL, ndcB, 0.0f); GXTexCoord2f32(u0, v1);
        GXColor4u8(r, g, b, a); GXPosition3f32(ndcR, ndcB, 0.0f); GXTexCoord2f32(u1, v1);
        GXColor4u8(r, g, b, a); GXPosition3f32(ndcR, ndcT, 0.0f); GXTexCoord2f32(u1, v0);
        GXColor4u8(r, g, b, a); GXPosition3f32(ndcL, ndcT, 0.0f); GXTexCoord2f32(u0, v0);
        GXEnd();
        cx += gw;
    }
}

/* Greedy word-wrap: draw `s` as up to maxLines lines of <= maxChars glyphs,
 * starting at (x,y) with 1.2*gh line pitch. */
static void DrawTextWrapped(f32 x, f32 y, f32 gw, f32 gh, int maxChars, int maxLines,
                            u8 r, u8 g, u8 b, u8 a, const char* s) {
    char line[128];
    char word[80];
    int lineLen = 0;
    int wordLen = 0;
    int lines = 0;
    const char* p = s;
    int done = 0;

    if (s == NULL) {
        return;
    }
    while (!done && lines < maxLines) {
        char c = *p;
        int flushWord = 0;
        int atEnd = 0;

        if (c == '\0') {
            atEnd = 1;
            flushWord = (wordLen > 0);
        } else if (c == ' ') {
            flushWord = 1;
        } else if (wordLen < (int)sizeof(word) - 1) {
            word[wordLen++] = c;
        }

        if (flushWord) {
            int need = wordLen + (lineLen > 0 ? 1 : 0);
            int i;

            if (lineLen + need > maxChars && lineLen > 0) {
                line[lineLen] = '\0';
                DrawTextScreen(x, y + (f32)lines * gh * 1.2f, gw, gh, r, g, b, a, line);
                lines++;
                lineLen = 0;
            }
            if (lines < maxLines) {
                if (lineLen > 0 && lineLen < (int)sizeof(line) - 1) {
                    line[lineLen++] = ' ';
                }
                for (i = 0; i < wordLen && lineLen < (int)sizeof(line) - 1; ++i) {
                    line[lineLen++] = word[i];
                }
            }
            wordLen = 0;
        }
        if (atEnd) {
            if (lineLen > 0 && lines < maxLines) {
                line[lineLen] = '\0';
                DrawTextScreen(x, y + (f32)lines * gh * 1.2f, gw, gh, r, g, b, a, line);
            }
            done = 1;
        } else {
            ++p;
        }
    }
}

/* Load a raw topmenu sprite member (0x80-byte header: width@0, height@2 as
 * big-endian u16, bpp marker@4 with 0x20=RGBA8 else RGB5A3; texels at +0x80),
 * decode to RGBA, and upload as a host texture. Returns 1 on success. */
static int LoadRawMenuTexObj(const char* member, GXTexObj* outTex) {
    u8* data = NULL;
    u32 size = 0;
    GXDecodedTexture decoded;
    u16 w;
    u16 h;
    u32 format;

    if (!PCPort_LoadFsysMember(PCPORT_REAL_CONTENT_ARCHIVE, member,
                               &data, &size)) {
        return 0;
    }
    if (size < 0x80u) {
        PCPort_FreeBuffer(data);
        return 0;
    }

    w = (u16)(((u16)data[0] << 8) | data[1]);
    h = (u16)(((u16)data[2] << 8) | data[3]);
    format = (data[4] == 0x20u) ? (u32)GX_TF_RGBA8 : (u32)GX_TF_RGB5A3;

    memset(&decoded, 0, sizeof(decoded));
    if (gx_texture_decode(data + 0x80, w, h, (GXTexFmt)format,
                          NULL, GX_TL_IA8, 0, &decoded) != 0 ||
        decoded.data == NULL) {
        PCPort_FreeBuffer(data);
        return 0;
    }

    memset(outTex, 0, sizeof(*outTex));
    GXHostInitTexObjRGBA8(outTex, decoded.data, w, h, GX_CLAMP, GX_CLAMP);
    gx_texture_free(&decoded);
    PCPort_FreeBuffer(data);
    return 1;
}

/* Load a raw RGBA8 blob (BE u32 width, BE u32 height, then w*h*4 RGBA bytes) from
 * a repo-relative file and upload it as a texture. Used for the title's Espeon
 * cutout, whose on-disc source member isn't identifiable (loaded by numeric ID
 * via still-ASM sprite code; texels not found in any fsys member or raw file by
 * an exhaustive scan) -- so the authentic texture, extracted via a Dolphin
 * texture dump and bundled at tools/pcport_assets/, stands in until/if the disc
 * member is located. Returns 1 on success. */
static int LoadRawRGBABlobTexObj(const char* path, GXTexObj* outTex) {
    FILE* f = fopen(path, "rb");
    unsigned char hdr[8];
    u32 w, h, px;
    u8* pixels;

    if (f == NULL) {
        return 0;
    }
    if (fread(hdr, 1, 8, f) != 8) {
        fclose(f);
        return 0;
    }
    w = ((u32)hdr[0] << 24) | ((u32)hdr[1] << 16) | ((u32)hdr[2] << 8) | hdr[3];
    h = ((u32)hdr[4] << 24) | ((u32)hdr[5] << 16) | ((u32)hdr[6] << 8) | hdr[7];
    if (w == 0u || h == 0u || w > 4096u || h > 4096u) {
        fclose(f);
        return 0;
    }
    px = w * h * 4u;
    pixels = (u8*)malloc(px);
    if (pixels == NULL) {
        fclose(f);
        return 0;
    }
    if (fread(pixels, 1, px, f) != px) {
        free(pixels);
        fclose(f);
        return 0;
    }
    fclose(f);

    memset(outTex, 0, sizeof(*outTex));
    GXHostInitTexObjRGBA8(outTex, pixels, (u16)w, (u16)h, GX_CLAMP, GX_CLAMP);
    free(pixels);
    return 1;
}

/* Load a raw 0x80-header sprite from an arbitrary fsys archive, like
 * LoadRawMenuTexObj but with a caller-chosen archive and CMPR support. The
 * header byte at +4 marks the format: 0x20=RGBA8, 0x04=CMPR, else RGB5A3. Used
 * for the boot logos (nintendo_logo.fsys:logo_nintendo etc. are CMPR 640x480). */
static int LoadFsysSpriteTexObj(const char* archive, const char* member,
                                GXTexObj* outTex) {
    u8* data = NULL;
    u32 size = 0;
    GXDecodedTexture decoded;
    u16 w;
    u16 h;
    u32 format;

    if (!PCPort_LoadFsysMember(archive, member, &data, &size)) {
        return 0;
    }
    if (size < 0x80u) {
        PCPort_FreeBuffer(data);
        return 0;
    }

    w = (u16)(((u16)data[0] << 8) | data[1]);
    h = (u16)(((u16)data[2] << 8) | data[3]);
    if (data[4] == 0x20u) {
        format = (u32)GX_TF_RGBA8;
    } else if (data[4] == 0x04u) {
        format = (u32)GX_TF_CMPR;
    } else {
        format = (u32)GX_TF_RGB5A3;
    }

    memset(&decoded, 0, sizeof(decoded));
    if (gx_texture_decode(data + 0x80, w, h, (GXTexFmt)format,
                          NULL, GX_TL_IA8, 0, &decoded) != 0 ||
        decoded.data == NULL) {
        PCPort_FreeBuffer(data);
        return 0;
    }

    memset(outTex, 0, sizeof(*outTex));
    GXHostInitTexObjRGBA8(outTex, decoded.data, w, h, GX_CLAMP, GX_CLAMP);
    gx_texture_free(&decoded);
    PCPort_FreeBuffer(data);
    return 1;
}

static int LoadPngTexObj(const char* path, GXTexObj* outTex) {
    int w = 0;
    int h = 0;
    int n = 0;
    unsigned char* pixels;

    if (path == NULL || outTex == NULL) {
        return 0;
    }
    pixels = stbi_load(path, &w, &h, &n, 4);
    if (pixels == NULL || w <= 0 || h <= 0 || w > 4096 || h > 4096) {
        if (pixels != NULL) {
            stbi_image_free(pixels);
        }
        return 0;
    }
    memset(outTex, 0, sizeof(*outTex));
    GXHostInitTexObjRGBA8(outTex, pixels, (u16)w, (u16)h, GX_CLAMP, GX_CLAMP);
    stbi_image_free(pixels);
    return 1;
}

static int RunRawPrimitiveControl(void) {
    unsigned char pixel[4];

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    glBegin(GL_TRIANGLES);
    glColor3ub(0xFF, 0x00, 0xFF);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glColor3ub(0xFF, 0x00, 0xFF);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glColor3ub(0xFF, 0x00, 0xFF);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glFlush();

    return ReadBackbufferPixel(pixel);
}

static int RunRawScissorControl(void) {
    unsigned char inside[4];
    unsigned char outside[4];

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glScissor(PCPORT_GX_SCISSOR_X,
              PCPORT_GX_SCISSOR_Y,
              PCPORT_GX_SCISSOR_W,
              PCPORT_GX_SCISSOR_H);

    glBegin(GL_TRIANGLES);
    glColor3ub(0xFF, 0x80, 0x00);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glColor3ub(0xFF, 0x80, 0x00);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glColor3ub(0xFF, 0x80, 0x00);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glFlush();

    ReadBackbufferPixel(inside);
    ReadBackbufferPixelAt(PCPORT_GX_OUTSIDE_X, PCPORT_GX_OUTSIDE_Y, outside);
    glDisable(GL_SCISSOR_TEST);

    return (inside[0] != 0 || inside[1] != 0 || inside[2] != 0) &&
           outside[0] == 0 && outside[1] == 0 && outside[2] == 0;
}

static int RunGXPrimitiveSmoke(void) {
    unsigned char pixel[4];

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    LoadIdentityGXState();
    SubmitFullScreenGXQuad(0x20, 0xD0, 0x40, 0xFF);
    glFlush();

    if (!ReadBackbufferPixel(pixel)) {
        int rawWorked = RunRawPrimitiveControl();
        fprintf(stderr,
                "[pcport_bootstrap] GX primitive smoke failed (rgba=%u,%u,%u,%u rawControl=%d submitted=%u expanded=%u prim=0x%X)\n",
                (unsigned int)pixel[0],
                (unsigned int)pixel[1],
                (unsigned int)pixel[2],
                (unsigned int)pixel[3],
                rawWorked,
                GXHostGetLastSubmittedVertexCount(),
                GXHostGetLastExpandedVertexCount(),
                GXHostGetLastSubmittedPrimitive());
        return 0;
    }

    printf("[pcport_bootstrap] Host GX primitive smoke passed (rgba=%u,%u,%u,%u submitted=%u expanded=%u prim=0x%X)\n",
           (unsigned int)pixel[0],
           (unsigned int)pixel[1],
           (unsigned int)pixel[2],
           (unsigned int)pixel[3],
           GXHostGetLastSubmittedVertexCount(),
           GXHostGetLastExpandedVertexCount(),
           GXHostGetLastSubmittedPrimitive());
    return 1;
}

static int RunGXScissorSmoke(void) {
    unsigned char inside[4];
    unsigned char outside[4];

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    LoadIdentityGXState();
    GXSetScissor(PCPORT_GX_SCISSOR_X,
                 PCPORT_GX_SCISSOR_Y,
                 PCPORT_GX_SCISSOR_W,
                 PCPORT_GX_SCISSOR_H);
    SubmitFullScreenGXQuad(0x20, 0xA0, 0xF0, 0xFF);
    glFlush();

    ReadBackbufferPixel(inside);
    ReadBackbufferPixelAt(PCPORT_GX_OUTSIDE_X, PCPORT_GX_OUTSIDE_Y, outside);
    glDisable(GL_SCISSOR_TEST);

    if ((inside[0] == 0 && inside[1] == 0 && inside[2] == 0) ||
        outside[0] != 0 || outside[1] != 0 || outside[2] != 0) {
        int rawWorked = RunRawScissorControl();
        fprintf(stderr,
                "[pcport_bootstrap] GX scissor smoke failed (inside=%u,%u,%u,%u outside=%u,%u,%u,%u rawControl=%d submitted=%u expanded=%u prim=0x%X)\n",
                (unsigned int)inside[0],
                (unsigned int)inside[1],
                (unsigned int)inside[2],
                (unsigned int)inside[3],
                (unsigned int)outside[0],
                (unsigned int)outside[1],
                (unsigned int)outside[2],
                (unsigned int)outside[3],
                rawWorked,
                GXHostGetLastSubmittedVertexCount(),
                GXHostGetLastExpandedVertexCount(),
                GXHostGetLastSubmittedPrimitive());
        return 0;
    }

    printf("[pcport_bootstrap] Host GX scissor smoke passed (inside=%u,%u,%u,%u outside=%u,%u,%u,%u)\n",
           (unsigned int)inside[0],
           (unsigned int)inside[1],
           (unsigned int)inside[2],
           (unsigned int)inside[3],
           (unsigned int)outside[0],
           (unsigned int)outside[1],
           (unsigned int)outside[2],
           (unsigned int)outside[3]);
    return 1;
}

static void InitSmokeJoint(HSD_Joint* joint)
{
    memset(joint, 0, sizeof(*joint));
    joint->scale_x = 1.0f;
    joint->scale_y = 1.0f;
    joint->scale_z = 1.0f;
}

static int RunJObjInstanceSmoke(void)
{
    HSD_Joint rootJoint;
    HSD_Joint targetJoint;
    HSD_Joint instanceJoint;
    HSD_JObj* root;
    HSD_JObj* target;
    HSD_JObj* instance;
    HSD_JObj* instanceChild;
    int ok;

    InitSmokeJoint(&rootJoint);
    InitSmokeJoint(&targetJoint);
    InitSmokeJoint(&instanceJoint);

    rootJoint.child = &targetJoint;
    rootJoint.next = &instanceJoint;
    targetJoint.position_x = 10.0f;
    instanceJoint.flags = JOBJ_INSTANCE;
    instanceJoint.child = &targetJoint;
    instanceJoint.position_y = 5.0f;

    root = HSD_JObjLoadJoint(&rootJoint);
    if (root == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] JObj instance smoke failed: load returned NULL\n");
        return 0;
    }

    target = root->child;
    instance = root->next;
    instanceChild = instance != NULL ? instance->child : NULL;
    ok = target != NULL &&
         instance != NULL &&
         instanceChild == target &&
         target->parent == root;

    HSD_JObjSetFlagsAll(root, JOBJ_HIDDEN);
    HSD_JObjClearFlagsAll(root, JOBJ_HIDDEN);
    HSD_JObjAnimAll(root);
    HSD_JObjRemoveAll(root);

    if (!ok) {
        fprintf(stderr,
                "[pcport_bootstrap] JObj instance smoke failed: target=%p instance=%p instanceChild=%p\n",
                (void*)target,
                (void*)instance,
                (void*)instanceChild);
        return 0;
    }

    printf("[pcport_bootstrap] JObj instance smoke passed (shared child=%p)\n",
           (void*)target);
    return 1;
}

static int RunJObjResolveSmoke(void)
{
    HSD_Joint rootJoint;
    HSD_Joint targetJoint;
    HSD_Joint instanceJoint;
    HSD_JObj* root;
    HSD_JObj* target;
    HSD_JObj* instance;
    HSD_JObj* instanceChild;
    int ok;

    InitSmokeJoint(&rootJoint);
    InitSmokeJoint(&targetJoint);
    InitSmokeJoint(&instanceJoint);

    rootJoint.child = &targetJoint;
    rootJoint.next = &instanceJoint;
    instanceJoint.flags = JOBJ_INSTANCE;
    instanceJoint.child = &targetJoint;

    root = HSD_JObjAlloc();
    target = HSD_JObjAlloc();
    instance = HSD_JObjAlloc();
    if (root == NULL || target == NULL || instance == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] JObj resolve smoke failed: allocation failed\n");
        if (root != NULL) {
            HSD_JObjUnref(root);
        }
        if (target != NULL) {
            HSD_JObjUnref(target);
        }
        if (instance != NULL) {
            HSD_JObjUnref(instance);
        }
        return 0;
    }

    root->child = target;
    root->next = instance;
    target->parent = root;
    instance->flags = JOBJ_INSTANCE;
    instance->child = NULL;

    HSD_JObjResolveRefsAll(root, &rootJoint);

    instanceChild = instance->child;
    ok = instanceChild == target && target->parent == root;

    HSD_JObjSetFlagsAll(root, JOBJ_HIDDEN);
    HSD_JObjClearFlagsAll(root, JOBJ_HIDDEN);
    HSD_JObjAnimAll(root);
    HSD_JObjRemoveAll(root);

    if (!ok) {
        fprintf(stderr,
                "[pcport_bootstrap] JObj resolve smoke failed: target=%p instanceChild=%p\n",
                (void*)target,
                (void*)instanceChild);
        return 0;
    }

    printf("[pcport_bootstrap] JObj HSD_JObjResolveRefsAll resolve smoke passed (shared child=%p)\n",
           (void*)target);
    return 1;
}

static int RunJObjLoadWrapperSmoke(void)
{
    HSD_Joint rootJoint;
    HSD_Joint targetJoint;
    HSD_Joint instanceJoint;
    HSD_JObj* root;
    HSD_JObj* target;
    HSD_JObj* instance;
    HSD_JObj* instanceChild;
    int ok;

    InitSmokeJoint(&rootJoint);
    InitSmokeJoint(&targetJoint);
    InitSmokeJoint(&instanceJoint);

    rootJoint.child = &targetJoint;
    rootJoint.next = &instanceJoint;
    instanceJoint.flags = JOBJ_INSTANCE;
    instanceJoint.child = &targetJoint;

    root = fn_801A0FBC(&rootJoint);
    if (root == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] JObj load-wrapper smoke failed: load returned NULL\n");
        return 0;
    }

    target = root->child;
    instance = root->next;
    instanceChild = instance != NULL ? instance->child : NULL;
    ok = target != NULL &&
         instance != NULL &&
         instanceChild == target &&
         target->parent == root;

    HSD_JObjSetFlagsAll(root, JOBJ_HIDDEN);
    HSD_JObjClearFlagsAll(root, JOBJ_HIDDEN);
    HSD_JObjAnimAll(root);
    HSD_JObjRemoveAll(root);

    if (!ok) {
        fprintf(stderr,
                "[pcport_bootstrap] JObj load-wrapper smoke failed: target=%p instance=%p instanceChild=%p\n",
                (void*)target,
                (void*)instance,
                (void*)instanceChild);
        return 0;
    }

    printf("[pcport_bootstrap] JObj fn_801A0FBC load-wrapper smoke passed (shared child=%p)\n",
           (void*)target);
    return 1;
}

static int RunGSgfxVisibleAttempt(void) {
    unsigned char pixel[4];

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    VIWaitForRetrace_PC();
    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();
    glFlush();

    if (!ReadBackbufferPixel(pixel)) {
        fprintf(stderr,
                "[pcport_bootstrap] GSgfx_BeginFrame visible attempt failed (rgba=%u,%u,%u,%u submitted=%u expanded=%u prim=0x%X)\n",
                (unsigned int)pixel[0],
                (unsigned int)pixel[1],
                (unsigned int)pixel[2],
                (unsigned int)pixel[3],
                GXHostGetLastSubmittedVertexCount(),
                GXHostGetLastExpandedVertexCount(),
                GXHostGetLastSubmittedPrimitive());
        return 0;
    }

    printf("[pcport_bootstrap] GSgfx_BeginFrame visible attempt passed (rgba=%u,%u,%u,%u)\n",
           (unsigned int)pixel[0],
           (unsigned int)pixel[1],
           (unsigned int)pixel[2],
           (unsigned int)pixel[3]);
    return 1;
}

static int RunGSgfxScissorRetry(void) {
    unsigned char inside[4];
    unsigned char outside[4];

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    VIWaitForRetrace_PC();
    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    fn_800D9D68(PCPORT_GX_SCISSOR_X,
                PCPORT_GX_SCISSOR_Y,
                PCPORT_GX_SCISSOR_X2,
                PCPORT_GX_SCISSOR_Y2);
    GSgfx_BeginFrame();
    glFlush();

    ReadBackbufferPixel(inside);
    ReadBackbufferPixelAt(PCPORT_GX_OUTSIDE_X, PCPORT_GX_OUTSIDE_Y, outside);
    glDisable(GL_SCISSOR_TEST);

    if ((inside[0] == 0 && inside[1] == 0 && inside[2] == 0) ||
        outside[0] != 0 || outside[1] != 0 || outside[2] != 0) {
        fprintf(stderr,
                "[pcport_bootstrap] Game-owned fn_800D9D68 retry failed (inside=%u,%u,%u,%u outside=%u,%u,%u,%u submitted=%u expanded=%u prim=0x%X)\n",
                (unsigned int)inside[0],
                (unsigned int)inside[1],
                (unsigned int)inside[2],
                (unsigned int)inside[3],
                (unsigned int)outside[0],
                (unsigned int)outside[1],
                (unsigned int)outside[2],
                (unsigned int)outside[3],
                GXHostGetLastSubmittedVertexCount(),
                GXHostGetLastExpandedVertexCount(),
                GXHostGetLastSubmittedPrimitive());
        return 0;
    }

    printf("[pcport_bootstrap] Game-owned fn_800D9D68 retry passed (inside=%u,%u,%u,%u outside=%u,%u,%u,%u)\n",
           (unsigned int)inside[0],
           (unsigned int)inside[1],
           (unsigned int)inside[2],
           (unsigned int)inside[3],
           (unsigned int)outside[0],
           (unsigned int)outside[1],
           (unsigned int)outside[2],
           (unsigned int)outside[3]);
    return 1;
}

static int RunGSgfxSceneLikeSmoke(void) {
    unsigned char panel[4];
    unsigned char header[4];
    unsigned char background[4];

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    VIWaitForRetrace_PC();
    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();
    ConfigureSceneLikeDisplayListState();
    fn_800DAD10((void*)&g_sceneLikeDrawObject);
    glFlush();

    ReadBackbufferPixelAt(320, 240, panel);
    ReadBackbufferPixelAt(320, 320, header);
    ReadBackbufferPixelAt(PCPORT_GX_OUTSIDE_X, PCPORT_GX_OUTSIDE_Y, background);

    if (panel[0] != 0xE6 || panel[1] != 0xDC || panel[2] != 0xBC ||
        header[0] != 0xD0 || header[1] != 0x7A || header[2] != 0x20 ||
        background[0] != 0x30 || background[1] != 0xD5 ||
        background[2] != 0x5E) {
        fprintf(stderr,
                "[pcport_bootstrap] GSgfx scene-like display-list smoke failed (panel=%u,%u,%u,%u header=%u,%u,%u,%u bg=%u,%u,%u,%u submitted=%u expanded=%u prim=0x%X)\n",
                (unsigned int)panel[0],
                (unsigned int)panel[1],
                (unsigned int)panel[2],
                (unsigned int)panel[3],
                (unsigned int)header[0],
                (unsigned int)header[1],
                (unsigned int)header[2],
                (unsigned int)header[3],
                (unsigned int)background[0],
                (unsigned int)background[1],
                (unsigned int)background[2],
                (unsigned int)background[3],
                GXHostGetLastSubmittedVertexCount(),
                GXHostGetLastExpandedVertexCount(),
                GXHostGetLastSubmittedPrimitive());
        return 0;
    }

    printf("[pcport_bootstrap] GSgfx scene-like display-list smoke passed (panel=%u,%u,%u,%u header=%u,%u,%u,%u bg=%u,%u,%u,%u)\n",
           (unsigned int)panel[0],
           (unsigned int)panel[1],
           (unsigned int)panel[2],
           (unsigned int)panel[3],
           (unsigned int)header[0],
           (unsigned int)header[1],
           (unsigned int)header[2],
           (unsigned int)header[3],
           (unsigned int)background[0],
           (unsigned int)background[1],
           (unsigned int)background[2],
           (unsigned int)background[3]);
    return 1;
}

static int RunGSgfxPObjSmoke(void) {
    HSD_PObj pobj;
    PCPortGSDrawObject drawObject;
    unsigned char panel[4];
    unsigned char header[4];
    unsigned char background[4];

    InitSceneLikePObj(&pobj);
    memset(&drawObject, 0, sizeof(drawObject));
    drawObject.displayList = pobj.display;
    drawObject.displayListSize = pobj.n_display;
    drawObject.pipelineId = 0;
    drawObject.totalVerts = (unsigned int)(sizeof(g_sceneLikePositions) /
                                           sizeof(g_sceneLikePositions[0]));
    drawObject.totalPrims = 3;

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    VIWaitForRetrace_PC();
    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();
    fn_801AA568(&pobj);
    fn_800DAD10((void*)&drawObject);
    glFlush();

    ReadBackbufferPixelAt(320, 240, panel);
    ReadBackbufferPixelAt(320, 320, header);
    ReadBackbufferPixelAt(PCPORT_GX_OUTSIDE_X, PCPORT_GX_OUTSIDE_Y, background);

    if (panel[0] != 0xE6 || panel[1] != 0xDC || panel[2] != 0xBC ||
        header[0] != 0xD0 || header[1] != 0x7A || header[2] != 0x20 ||
        background[0] != 0x30 || background[1] != 0xD5 ||
        background[2] != 0x5E) {
        fprintf(stderr,
                "[pcport_bootstrap] GSgfx HSD_PObj smoke failed (panel=%u,%u,%u,%u header=%u,%u,%u,%u bg=%u,%u,%u,%u submitted=%u expanded=%u prim=0x%X)\n",
                (unsigned int)panel[0],
                (unsigned int)panel[1],
                (unsigned int)panel[2],
                (unsigned int)panel[3],
                (unsigned int)header[0],
                (unsigned int)header[1],
                (unsigned int)header[2],
                (unsigned int)header[3],
                (unsigned int)background[0],
                (unsigned int)background[1],
                (unsigned int)background[2],
                (unsigned int)background[3],
                GXHostGetLastSubmittedVertexCount(),
                GXHostGetLastExpandedVertexCount(),
                GXHostGetLastSubmittedPrimitive());
        return 0;
    }

    printf("[pcport_bootstrap] GSgfx HSD_PObj smoke passed (panel=%u,%u,%u,%u header=%u,%u,%u,%u bg=%u,%u,%u,%u)\n",
           (unsigned int)panel[0],
           (unsigned int)panel[1],
           (unsigned int)panel[2],
           (unsigned int)panel[3],
           (unsigned int)header[0],
           (unsigned int)header[1],
           (unsigned int)header[2],
           (unsigned int)header[3],
           (unsigned int)background[0],
           (unsigned int)background[1],
           (unsigned int)background[2],
           (unsigned int)background[3]);
    return 1;
}

static int RunRealContentParserSmoke(void) {
    PCPortHSDArchive archive;
    u8* memberData = NULL;
    u32 memberSize = 0;
    const u8* sceneData;
    u32 sceneOffset = 0;
    u32 sceneWord0;
    u32 sceneWord1;
    u32 sceneWord2;
    u32 sceneWord3;
    unsigned int hostPObjDescSize;
    int ok = 0;

    memset(&archive, 0, sizeof(archive));

    if (!PCPort_LoadFsysMember(PCPORT_REAL_CONTENT_ARCHIVE,
                               PCPORT_REAL_CONTENT_MEMBER,
                               &memberData,
                               &memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real content load failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    if (!PCPort_HSDArchiveParseBE(&archive, memberData, memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] HSD archive parse failed (%s:%s size=0x%X head=%08X,%08X,%08X,%08X)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER,
                memberSize,
                memberSize >= 0x04u ? PCPort_ReadBigEndianU32(memberData + 0x00) : 0u,
                memberSize >= 0x08u ? PCPort_ReadBigEndianU32(memberData + 0x04) : 0u,
                memberSize >= 0x0Cu ? PCPort_ReadBigEndianU32(memberData + 0x08) : 0u,
                memberSize >= 0x10u ? PCPort_ReadBigEndianU32(memberData + 0x0C) : 0u);
        goto cleanup;
    }

    if (archive.publicCount != 1u || archive.externCount != 0u) {
        fprintf(stderr,
                "[pcport_bootstrap] Unexpected HSD archive table counts (public=%u extern=%u)\n",
                archive.publicCount,
                archive.externCount);
        goto cleanup;
    }

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&archive,
                                                             "scene_data",
                                                             &sceneOffset);
    if (sceneData == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to resolve public symbol scene_data\n");
        goto cleanup;
    }

    sceneWord0 = PCPort_ReadBigEndianU32(sceneData + 0x00);
    sceneWord1 = PCPort_ReadBigEndianU32(sceneData + 0x04);
    sceneWord2 = PCPort_ReadBigEndianU32(sceneData + 0x08);
    sceneWord3 = PCPort_ReadBigEndianU32(sceneData + 0x0C);
    if (sceneWord0 < archive.dataOffset || sceneWord0 >= archive.storageSize ||
        sceneWord1 < archive.dataOffset || sceneWord1 >= archive.storageSize ||
        sceneWord2 < archive.dataOffset || sceneWord2 >= archive.storageSize ||
        sceneWord3 != 0u) {
        fprintf(stderr,
                "[pcport_bootstrap] scene_data root validation failed (w0=0x%X w1=0x%X w2=0x%X w3=0x%X size=0x%X)\n",
                sceneWord0,
                sceneWord1,
                sceneWord2,
                sceneWord3,
                archive.storageSize);
        goto cleanup;
    }

    hostPObjDescSize = (unsigned int)sizeof(HSD_PObjDesc);
    if (hostPObjDescSize <= 0x18u) {
        fprintf(stderr,
                "[pcport_bootstrap] Host HSD_PObjDesc size unexpectedly small (0x%X)\n",
                hostPObjDescSize);
        goto cleanup;
    }

    printf("[pcport_bootstrap] Real HSD archive parsed (%s:%s public=scene_data offset=0x%X root=%08X,%08X,%08X,%08X)\n",
           PCPORT_REAL_CONTENT_ARCHIVE,
           PCPORT_REAL_CONTENT_MEMBER,
           sceneOffset,
           sceneWord0,
           sceneWord1,
           sceneWord2,
           sceneWord3);
    printf("[pcport_bootstrap] Direct host HSD consumption still blocked (sizeof(HSD_PObjDesc)=0x%X, serialized GC descriptor layout is 0x18 bytes)\n",
           hostPObjDescSize);
    ok = 1;

cleanup:
    PCPort_HSDArchiveDestroy(&archive);
    PCPort_FreeBuffer(memberData);
    return ok;
}

static int ArchiveRangeValid(const PCPortHSDArchive* archive,
                             u32 offset, u32 size) {
    if (archive == NULL || archive->storage == NULL || offset < archive->dataOffset ||
        offset > archive->storageSize) {
        return 0;
    }

    return size <= archive->storageSize - offset;
}

static int ResolveFirstRenderablePObjDescFromJoint(const PCPortHSDArchive* archive,
                                                   u32 jointOffset,
                                                   u32* outJointOffset,
                                                   u32* outDObjOffset,
                                                   u32* outPObjOffset) {
    u32 dobjOffset;
    u32 childOffset;
    u32 nextOffset;

    if (!ArchiveRangeValid(archive, jointOffset, PCPORT_SERIALIZED_JOINT_SIZE)) {
        return 0;
    }

    dobjOffset = PCPort_ReadBigEndianU32(archive->storage + jointOffset + 0x10);
    if (ArchiveRangeValid(archive, dobjOffset, PCPORT_SERIALIZED_DOBJ_SIZE)) {
        u32 pobjOffset = PCPort_ReadBigEndianU32(archive->storage + dobjOffset + 0x0C);

        if (ArchiveRangeValid(archive, pobjOffset, PCPORT_SERIALIZED_POBJ_SIZE)) {
            u32 flagsAndDisplayCount = PCPort_ReadBigEndianU32(archive->storage + pobjOffset + 0x0C);
            u32 displayCount = flagsAndDisplayCount & 0xFFFFu;

            if (displayCount > 1u) {
                *outJointOffset = jointOffset;
                *outDObjOffset = dobjOffset;
                *outPObjOffset = pobjOffset;
                return 1;
            }
        }
    }

    childOffset = PCPort_ReadBigEndianU32(archive->storage + jointOffset + 0x08);
    if (childOffset != 0u &&
        ResolveFirstRenderablePObjDescFromJoint(archive, childOffset,
                                                outJointOffset, outDObjOffset,
                                                outPObjOffset)) {
        return 1;
    }

    nextOffset = PCPort_ReadBigEndianU32(archive->storage + jointOffset + 0x0C);
    if (nextOffset != 0u &&
        ResolveFirstRenderablePObjDescFromJoint(archive, nextOffset,
                                                outJointOffset, outDObjOffset,
                                                outPObjOffset)) {
        return 1;
    }

    return 0;
}

static void ConfigureTranslatedMaterialPipeline(unsigned int pipelineId,
                                                const PCPortTranslatedMaterial* material) {
    unsigned char zUpdate;

    GSgfxHostClearPipelineState(pipelineId);
    if (material == NULL) {
        return;
    }

    GSgfxHostSetPipelineAlphaScale(pipelineId, material->alpha);
    zUpdate = (unsigned char)((material->rendermode & PCPORT_RENDER_NO_ZUPDATE) == 0u);

    if ((material->rendermode & PCPORT_RENDER_XLU) != 0u) {
        if (material->hasPEDesc) {
            GSgfxHostSetPipelineBlend(pipelineId,
                                      material->peType,
                                      material->peSrcFactor,
                                      material->peDstFactor,
                                      material->peLogicOp);
        } else {
            /* XLU material with no explicit PEDesc -> the GX default translucent
             * blend (GX_BM_BLEND, src=SRCALPHA, dst=INVSRCALPHA). Without this,
             * XLU surfaces (e.g. the title's distance-haze planes, tex 0x293E0)
             * rendered fully OPAQUE and occluded the desert ruins behind them. */
            GSgfxHostSetPipelineBlend(pipelineId,
                                      GX_BM_BLEND,
                                      GX_BL_SRCALPHA,
                                      GX_BL_INVSRCALPHA,
                                      GX_LO_COPY);
            /* These XLU planes (distance-haze, full vertex alpha) are faded by
             * GameCube distance fog in the real game, which the port doesn't
             * implement -- so at full strength they read as dark occluding bands.
             * Render them at reduced opacity to approximate the subtle haze.
             * PCPORT_HAZE_ALPHA tunes it. */
            {
                const char* ha = getenv("PCPORT_HAZE_ALPHA");
                f32 hazeA = (ha != NULL) ? (f32)atof(ha) : 0.72f;
                GSgfxHostSetPipelineAlphaScale(pipelineId, material->alpha * hazeA);
            }
        }
    }

    GSgfxHostSetPipelineZ(pipelineId,
                          1,
                          material->hasPEDesc ? material->peZComp : GX_LEQUAL,
                          zUpdate);

    if (material->hasPEDesc) {
        GSgfxHostSetPipelineAlphaCompare(pipelineId,
                                         material->peAlphaComp0,
                                         material->peRef0,
                                         material->peAlphaOp,
                                         material->peAlphaComp1,
                                         material->peRef1);
    }
}

static void ConfigureTranslatedTexturedPipeline(unsigned int pipelineId,
                                                const PCPortTranslatedMaterial* material,
                                                const PCPortTranslatedTexture* texture,
                                                GXTexObj* textureObject,
                                                unsigned char textureMapId) {
    ConfigureTranslatedMaterialPipeline(pipelineId, material);
    if (texture != NULL && textureObject != NULL) {
        GSgfxHostSetPipelineTexture(pipelineId,
                                    textureObject,
                                    1,
                                    texture->tevMode,
                                    texture->hasCoordId ? texture->coordId : 0u,
                                    textureMapId);
    }
}

static void TranslateLiveMObjMaterial(const HSD_MObj* mobj,
                                      PCPortTranslatedMaterial* outMaterial) {
    if (outMaterial == NULL) {
        return;
    }

    memset(outMaterial, 0, sizeof(*outMaterial));
    outMaterial->alpha = 1.0f;
    if (mobj == NULL) {
        return;
    }

    outMaterial->rendermode = mobj->rendermode;
    if (mobj->mat != NULL) {
        outMaterial->ambient = mobj->mat->ambient;
        outMaterial->diffuse = mobj->mat->diffuse;
        outMaterial->specular = mobj->mat->specular;
        outMaterial->alpha = mobj->mat->alpha;
        outMaterial->shininess = mobj->mat->shininess;
    }
    if (mobj->pe != NULL) {
        outMaterial->hasPEDesc = TRUE;
        outMaterial->peFlags = mobj->pe->flags;
        outMaterial->peRef0 = mobj->pe->ref0;
        outMaterial->peRef1 = mobj->pe->ref1;
        outMaterial->peDstAlpha = mobj->pe->dst_alpha;
        outMaterial->peType = mobj->pe->type;
        outMaterial->peSrcFactor = mobj->pe->src_factor;
        outMaterial->peDstFactor = mobj->pe->dst_factor;
        outMaterial->peLogicOp = mobj->pe->logic_op;
        outMaterial->peZComp = mobj->pe->z_comp;
        outMaterial->peAlphaComp0 = mobj->pe->alpha_comp0;
        outMaterial->peAlphaOp = mobj->pe->alpha_op;
        outMaterial->peAlphaComp1 = mobj->pe->alpha_comp1;
    }
}

static void ForceMaterialDeltaBlend(PCPortTranslatedMaterial* material) {
    if (material == NULL) {
        return;
    }

    material->rendermode |= PCPORT_RENDER_XLU | PCPORT_RENDER_NO_ZUPDATE;
    material->hasPEDesc = FALSE;
}

static unsigned char* CaptureMaterialDeltaFrame(
    const PCPortTranslatedCamera* camera,
    const PCPortTranslatedPObj* pobj,
    const PCPortTranslatedMaterial* material,
    const f32 modelViewMatrix[3][4],
    const PCPortGSDrawObject* drawObject,
    unsigned char sample[4])
{
    if (camera == NULL || pobj == NULL || material == NULL ||
        modelViewMatrix == NULL || drawObject == NULL) {
        return NULL;
    }

    ConfigureTranslatedMaterialPipeline(PCPORT_REAL_MATERIAL_PIPELINE,
                                        material);
    GXHostSetVertexAlphaScale(1.0f);
    VIWaitForRetrace_PC();
    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();

    GXSetViewport((f32)camera->viewportLeft,
                  (f32)camera->viewportTop,
                  (f32)(camera->viewportRight - camera->viewportLeft),
                  (f32)(camera->viewportBottom - camera->viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)camera->scissorLeft,
                 (u32)camera->scissorTop,
                 (u32)(camera->scissorRight - camera->scissorLeft),
                 (u32)(camera->scissorBottom - camera->scissorTop));
    GXSetProjection(camera->projectionMatrix, GX_PERSPECTIVE);
    GXLoadPosMtxImm((f32 (*)[4])modelViewMatrix, 0);
    GXSetCurrentMtx(0);

    fn_801AA568((HSD_PObj*)&pobj->pobj);
    fn_800DAD10((void*)drawObject);
    glFlush();

    if (sample != NULL) {
        ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                              PCPORT_GX_SMOKE_SAMPLE_Y,
                              sample);
    }
    return ReadBackbufferImage();
}

static int PCPort_IsBattleControlPObj(const PCPortTranslatedPObj* pobj,
                                      const PCPortTranslatedMaterial* material,
                                      int haveMaterial,
                                      int haveTexture) {
    if (!g_pcBattleSuppressControlPObjs || haveTexture || !haveMaterial ||
        pobj == NULL || material == NULL) {
        return 0;
    }

    return pobj->pobj.flags == 0x8000u &&
           pobj->totalSubmittedVertices == 24u &&
           material->diffuse == 0xB3B3B3FFu &&
           material->alpha > 0.99f &&
           material->alpha < 1.01f;
}

static int RunRealContentTranslationSmoke(void) {
    PCPortHSDArchive archive;
    PCPortTranslatedCamera translatedCamera;
    PCPortTranslatedPObj translatedPObj;
    PCPortTranslatedJointTransform translatedJoint;
    PCPortGSDrawObject drawObject;
    u8* memberData = NULL;
    u32 memberSize = 0;
    const u8* sceneData;
    unsigned char* baselinePixels = NULL;
    unsigned char* drawnPixels = NULL;
    u32 sceneOffset = 0;
    u32 sceneBranchOffset;
    u32 cameraDescOffset;
    u32 sceneJointListOffset;
    u32 rootJointOffset;
    u32 jointOffset = 0;
    u32 dobjOffset = 0;
    u32 pobjOffset = 0;
    unsigned int diffPixels = 0;
    f32 modelViewMatrix[3][4];
    f32 transformedMin[3];
    f32 transformedMax[3];
    f32 viewMin[3];
    f32 viewMax[3];
    int ok = 0;

    memset(&archive, 0, sizeof(archive));
    memset(&translatedCamera, 0, sizeof(translatedCamera));
    memset(&translatedPObj, 0, sizeof(translatedPObj));
    memset(&translatedJoint, 0, sizeof(translatedJoint));
    memset(&drawObject, 0, sizeof(drawObject));

    if (!PCPort_LoadFsysMember(PCPORT_REAL_CONTENT_ARCHIVE,
                               PCPORT_REAL_CONTENT_MEMBER,
                               &memberData,
                               &memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real content translation load failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    if (!PCPort_HSDArchiveParseBE(&archive, memberData, memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real content translation archive parse failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&archive,
                                                             "scene_data",
                                                             &sceneOffset);
    if (sceneData == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real content translation failed to resolve scene_data\n");
        goto cleanup;
    }

    sceneBranchOffset = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&archive, sceneBranchOffset, 0x10u)) {
        fprintf(stderr,
                "[pcport_bootstrap] scene_data branch offset was invalid (0x%X)\n",
                sceneBranchOffset);
        goto cleanup;
    }

    cameraDescOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x08);
    if (!PCPort_TranslatePerspectiveCameraFromArchiveBE(&archive,
                                                        cameraDescOffset,
                                                        &translatedCamera)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate real scene camera (scene=0x%X camera=0x%X)\n",
                sceneOffset,
                cameraDescOffset);
        goto cleanup;
    }

    sceneJointListOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x00);
    rootJointOffset = PCPort_ReadBigEndianU32(archive.storage + sceneJointListOffset + 0x00);
    if (!ResolveFirstRenderablePObjDescFromJoint(&archive,
                                                 rootJointOffset,
                                                 &jointOffset,
                                                 &dobjOffset,
                                                 &pobjOffset)) {
        fprintf(stderr,
                "[pcport_bootstrap] No renderable PObjDesc was found under scene_data (rootJoint=0x%X)\n",
                rootJointOffset);
        goto cleanup;
    }

    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           pobjOffset,
                                           &translatedPObj)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate real PObjDesc (joint=0x%X dobj=0x%X pobj=0x%X)\n",
                jointOffset,
                dobjOffset,
                pobjOffset);
        goto cleanup;
    }

    if (!PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              jointOffset,
                                              &translatedJoint)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate real joint chain (root=0x%X target=0x%X)\n",
                rootJointOffset,
                jointOffset);
        goto cleanup;
    }

    drawObject.displayList = translatedPObj.pobj.display;
    drawObject.displayListSize = translatedPObj.pobj.n_display;
    drawObject.pipelineId = 0;
    drawObject.totalVerts = translatedPObj.totalSubmittedVertices;
    drawObject.totalPrims = translatedPObj.totalPrimitiveCommands;

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    VIWaitForRetrace_PC();
    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();

    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);
    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedJoint.modelMatrix,
                    modelViewMatrix);
    GXLoadPosMtxImm(modelViewMatrix, 0);
    GXSetCurrentMtx(0);

    baselinePixels = ReadBackbufferImage();
    if (baselinePixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture baseline framebuffer for real content translation\n");
        goto cleanup;
    }

    fn_801AA568(&translatedPObj.pobj);
    fn_800DAD10((void*)&drawObject);
    glFlush();

    drawnPixels = ReadBackbufferImage();
    if (drawnPixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture translated real-content framebuffer\n");
        goto cleanup;
    }

    TransformBoundsByMtx(translatedPObj.minPosition,
                         translatedPObj.maxPosition,
                         translatedJoint.modelMatrix,
                         transformedMin,
                         transformedMax);
    TransformBoundsByMtx(translatedPObj.minPosition,
                         translatedPObj.maxPosition,
                         modelViewMatrix,
                         viewMin,
                         viewMax);
    diffPixels = CountFramebufferDiffPixels(baselinePixels, drawnPixels);
    if (diffPixels == 0u) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene camera bridge reached fn_800DAD10 but changed no framebuffer pixels (scene=0x%X camera=0x%X eye=0x%X interest=0x%X joint=0x%X dobj=0x%X pobj=0x%X submitted=%u expanded=%u prim=0x%X local=[%.3f,%.3f,%.3f]-[%.3f,%.3f,%.3f] world=[%.3f,%.3f,%.3f]-[%.3f,%.3f,%.3f] view=[%.3f,%.3f,%.3f]-[%.3f,%.3f,%.3f] eye=(%.3f,%.3f,%.3f) at=(%.3f,%.3f,%.3f) fov=%.3f aspect=%.3f near=%.3f far=%.3f)\n",
                sceneOffset,
                translatedCamera.cameraArchiveOffset,
                translatedCamera.eyeArchiveOffset,
                translatedCamera.interestArchiveOffset,
                jointOffset,
                dobjOffset,
                pobjOffset,
                GXHostGetLastSubmittedVertexCount(),
                GXHostGetLastExpandedVertexCount(),
                GXHostGetLastSubmittedPrimitive(),
                translatedPObj.minPosition[0],
                translatedPObj.minPosition[1],
                translatedPObj.minPosition[2],
                translatedPObj.maxPosition[0],
                translatedPObj.maxPosition[1],
                translatedPObj.maxPosition[2],
                transformedMin[0],
                transformedMin[1],
                transformedMin[2],
                transformedMax[0],
                transformedMax[1],
                transformedMax[2],
                viewMin[0],
                viewMin[1],
                viewMin[2],
                viewMax[0],
                viewMax[1],
                viewMax[2],
                translatedCamera.eye[0],
                translatedCamera.eye[1],
                translatedCamera.eye[2],
                translatedCamera.interest[0],
                translatedCamera.interest[1],
                translatedCamera.interest[2],
                translatedCamera.fov,
                translatedCamera.aspect,
                translatedCamera.nearZ,
                translatedCamera.farZ);
        goto cleanup;
    }

    printf("[pcport_bootstrap] Real scene camera bridge smoke passed (scene=0x%X camera=0x%X joint=0x%X dobj=0x%X pobj=0x%X changedPixels=%u submitted=%u expanded=%u prim=0x%X local=[%.3f,%.3f,%.3f]-[%.3f,%.3f,%.3f] world=[%.3f,%.3f,%.3f]-[%.3f,%.3f,%.3f] view=[%.3f,%.3f,%.3f]-[%.3f,%.3f,%.3f])\n",
           sceneOffset,
           translatedCamera.cameraArchiveOffset,
           jointOffset,
           dobjOffset,
           pobjOffset,
           diffPixels,
           GXHostGetLastSubmittedVertexCount(),
           GXHostGetLastExpandedVertexCount(),
           GXHostGetLastSubmittedPrimitive(),
           translatedPObj.minPosition[0],
           translatedPObj.minPosition[1],
           translatedPObj.minPosition[2],
           translatedPObj.maxPosition[0],
           translatedPObj.maxPosition[1],
           translatedPObj.maxPosition[2],
           transformedMin[0],
           transformedMin[1],
           transformedMin[2],
           transformedMax[0],
           transformedMax[1],
           transformedMax[2],
           viewMin[0],
           viewMin[1],
           viewMin[2],
           viewMax[0],
           viewMax[1],
           viewMax[2]);
    ok = 1;

cleanup:
    free(drawnPixels);
    free(baselinePixels);
    PCPort_DestroyTranslatedPObj(&translatedPObj);
    PCPort_HSDArchiveDestroy(&archive);
    PCPort_FreeBuffer(memberData);
    return ok;
}

static int RunRealSceneSlice2Smoke(void) {
    PCPortHSDArchive archive;
    PCPortTranslatedCamera translatedCamera;
    PCPortTranslatedPObj translatedPObj;
    PCPortTranslatedJointTransform translatedJoint;
    PCPortTranslatedMaterial translatedMaterial;
    PCPortGSDrawObject opaqueDrawObject;
    PCPortGSDrawObject materialDrawObject;
    u8* memberData = NULL;
    u32 memberSize = 0;
    const u8* sceneData;
    unsigned char* opaquePixels = NULL;
    unsigned char* materialPixels = NULL;
    unsigned char opaqueSample[4] = { 0 };
    unsigned char materialSample[4] = { 0 };
    u32 sceneOffset = 0;
    u32 sceneBranchOffset;
    u32 cameraDescOffset;
    u32 sceneJointListOffset;
    u32 rootJointOffset;
    u32 jointOffset = 0;
    u32 dobjOffset = 0;
    u32 pobjOffset = 0;
    u32 mobjOffset = 0;
    unsigned int diffPixels = 0;
    f32 modelViewMatrix[3][4];
    int ok = 0;

    memset(&archive, 0, sizeof(archive));
    memset(&translatedCamera, 0, sizeof(translatedCamera));
    memset(&translatedPObj, 0, sizeof(translatedPObj));
    memset(&translatedJoint, 0, sizeof(translatedJoint));
    memset(&translatedMaterial, 0, sizeof(translatedMaterial));
    memset(&opaqueDrawObject, 0, sizeof(opaqueDrawObject));
    memset(&materialDrawObject, 0, sizeof(materialDrawObject));

    if (!PCPort_LoadFsysMember(PCPORT_REAL_CONTENT_ARCHIVE,
                               PCPORT_REAL_CONTENT_MEMBER,
                               &memberData,
                               &memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 2 load failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    if (!PCPort_HSDArchiveParseBE(&archive, memberData, memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 2 archive parse failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&archive,
                                                             "scene_data",
                                                             &sceneOffset);
    if (sceneData == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 2 failed to resolve scene_data\n");
        goto cleanup;
    }

    sceneBranchOffset = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&archive, sceneBranchOffset, 0x10u)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 2 branch offset was invalid (0x%X)\n",
                sceneBranchOffset);
        goto cleanup;
    }

    cameraDescOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x08);
    if (!PCPort_TranslatePerspectiveCameraFromArchiveBE(&archive,
                                                        cameraDescOffset,
                                                        &translatedCamera)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 2 failed to translate camera (scene=0x%X camera=0x%X)\n",
                sceneOffset,
                cameraDescOffset);
        goto cleanup;
    }

    sceneJointListOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x00);
    rootJointOffset = PCPort_ReadBigEndianU32(archive.storage + sceneJointListOffset + 0x00);
    if (!ResolveFirstRenderablePObjDescFromJoint(&archive,
                                                 rootJointOffset,
                                                 &jointOffset,
                                                 &dobjOffset,
                                                 &pobjOffset)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 2 found no renderable PObjDesc (rootJoint=0x%X)\n",
                rootJointOffset);
        goto cleanup;
    }

    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           pobjOffset,
                                           &translatedPObj)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 2 failed to translate PObjDesc (joint=0x%X dobj=0x%X pobj=0x%X)\n",
                jointOffset,
                dobjOffset,
                pobjOffset);
        goto cleanup;
    }

    if (!PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              jointOffset,
                                              &translatedJoint)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 2 failed to translate joint chain (root=0x%X target=0x%X)\n",
                rootJointOffset,
                jointOffset);
        goto cleanup;
    }

    mobjOffset = PCPort_ReadBigEndianU32(archive.storage + dobjOffset + 0x08);
    if (!PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               mobjOffset,
                                               &translatedMaterial)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 2 failed to translate MObjDesc (dobj=0x%X mobj=0x%X)\n",
                dobjOffset,
                mobjOffset);
        goto cleanup;
    }

    opaqueDrawObject.displayList = translatedPObj.pobj.display;
    opaqueDrawObject.displayListSize = translatedPObj.pobj.n_display;
    opaqueDrawObject.pipelineId = 0;
    opaqueDrawObject.totalVerts = translatedPObj.totalSubmittedVertices;
    opaqueDrawObject.totalPrims = translatedPObj.totalPrimitiveCommands;
    materialDrawObject = opaqueDrawObject;
    materialDrawObject.pipelineId = PCPORT_REAL_MATERIAL_PIPELINE;

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);

    GXHostSetVertexAlphaScale(1.0f);
    VIWaitForRetrace_PC();
    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();

    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);
    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedJoint.modelMatrix,
                    modelViewMatrix);
    GXLoadPosMtxImm(modelViewMatrix, 0);
    GXSetCurrentMtx(0);

    fn_801AA568(&translatedPObj.pobj);
    fn_800DAD10((void*)&opaqueDrawObject);
    glFlush();

    opaquePixels = ReadBackbufferImage();
    if (opaquePixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 2 failed to capture opaque framebuffer\n");
        goto cleanup;
    }
    ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                          PCPORT_GX_SMOKE_SAMPLE_Y,
                          opaqueSample);

    ConfigureTranslatedMaterialPipeline(PCPORT_REAL_MATERIAL_PIPELINE,
                                        &translatedMaterial);
    GXHostSetVertexAlphaScale(1.0f);
    VIWaitForRetrace_PC();
    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();

    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);
    GXLoadPosMtxImm(modelViewMatrix, 0);
    GXSetCurrentMtx(0);

    fn_801AA568(&translatedPObj.pobj);
    fn_800DAD10((void*)&materialDrawObject);
    glFlush();

    materialPixels = ReadBackbufferImage();
    if (materialPixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 2 failed to capture material framebuffer\n");
        goto cleanup;
    }
    ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                          PCPORT_GX_SMOKE_SAMPLE_Y,
                          materialSample);

    diffPixels = CountFramebufferDiffPixels(opaquePixels, materialPixels);
    if (diffPixels == 0u) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 2 material path produced no framebuffer delta (scene=0x%X camera=0x%X joint=0x%X dobj=0x%X mobj=0x%X pobj=0x%X opaque=%u,%u,%u,%u material=%u,%u,%u,%u alpha=%.3f blend=%u/%u/%u z=%u update=%u)\n",
                sceneOffset,
                translatedCamera.cameraArchiveOffset,
                jointOffset,
                dobjOffset,
                mobjOffset,
                pobjOffset,
                opaqueSample[0],
                opaqueSample[1],
                opaqueSample[2],
                opaqueSample[3],
                materialSample[0],
                materialSample[1],
                materialSample[2],
                materialSample[3],
                translatedMaterial.alpha,
                translatedMaterial.peType,
                translatedMaterial.peSrcFactor,
                translatedMaterial.peDstFactor,
                translatedMaterial.peZComp,
                (translatedMaterial.rendermode & PCPORT_RENDER_NO_ZUPDATE) == 0u);
        goto cleanup;
    }

    printf("[pcport_bootstrap] Real scene slice 2 smoke passed (scene=0x%X camera=0x%X joint=0x%X dobj=0x%X mobj=0x%X pobj=0x%X diffPixels=%u opaque=%u,%u,%u,%u material=%u,%u,%u,%u alpha=%.3f blend=%u/%u/%u z=%u update=%u submitted=%u expanded=%u prim=0x%X)\n",
           sceneOffset,
           translatedCamera.cameraArchiveOffset,
           jointOffset,
           dobjOffset,
           mobjOffset,
           pobjOffset,
           diffPixels,
           opaqueSample[0],
           opaqueSample[1],
           opaqueSample[2],
           opaqueSample[3],
           materialSample[0],
           materialSample[1],
           materialSample[2],
           materialSample[3],
           translatedMaterial.alpha,
           translatedMaterial.peType,
           translatedMaterial.peSrcFactor,
           translatedMaterial.peDstFactor,
           translatedMaterial.peZComp,
           (translatedMaterial.rendermode & PCPORT_RENDER_NO_ZUPDATE) == 0u,
           GXHostGetLastSubmittedVertexCount(),
           GXHostGetLastExpandedVertexCount(),
           GXHostGetLastSubmittedPrimitive());
    ok = 1;

cleanup:
    GSgfxHostClearPipelineState(PCPORT_REAL_MATERIAL_PIPELINE);
    GXHostSetVertexAlphaScale(1.0f);
    free(materialPixels);
    free(opaquePixels);
    PCPort_DestroyTranslatedPObj(&translatedPObj);
    PCPort_HSDArchiveDestroy(&archive);
    PCPort_FreeBuffer(memberData);
    return ok;
}

static int RunRealMaterialDeltaSmoke(void) {
    PCPortHSDArchive archive;
    PCPortTranslatedCamera translatedCamera;
    PCPortTranslatedPObj translatedPObj;
    PCPortTranslatedJointTransform translatedJoint;
    PCPortTranslatedMaterial fullMaterial;
    PCPortTranslatedMaterial fadedMaterial;
    PCPortGSDrawObject drawObject;
    HSD_MObj* liveMObj = NULL;
    HSD_TExp* loadTExpList = NULL;
    HSD_TExp* loadTExpRoot = NULL;
    HSD_TExp* setupTExpList = NULL;
    HSD_TExp* fullTExpList = NULL;
    HSD_TExp* fadedTExpList = NULL;
    HSD_TExp* fullTExpRoot = NULL;
    HSD_TExp* fadedTExpRoot = NULL;
    HSD_ObjData materialAnimValue;
    HSD_MObj copiedMObj;
    u8* memberData = NULL;
    u32 memberSize = 0;
    const u8* sceneData;
    unsigned char* fullPixels = NULL;
    unsigned char* fadedPixels = NULL;
    unsigned char fullSample[4] = { 0 };
    unsigned char fadedSample[4] = { 0 };
    u32 sceneOffset = 0;
    u32 sceneBranchOffset;
    u32 cameraDescOffset;
    u32 sceneJointListOffset;
    u32 rootJointOffset;
    u32 jointOffset = 0;
    u32 dobjOffset = 0;
    u32 pobjOffset = 0;
    u32 mobjOffset = 0;
    u32 materialAnimOriginalDiffuse = 0;
    f32 materialAnimOriginalAlpha = 0.0f;
    unsigned int materialAnimDiffuseR = 0;
    f32 materialAnimAlpha = 0.0f;
    unsigned int diffPixels = 0;
    f32 modelViewMatrix[3][4];
    int ok = 0;

    memset(&archive, 0, sizeof(archive));
    memset(&translatedCamera, 0, sizeof(translatedCamera));
    memset(&translatedPObj, 0, sizeof(translatedPObj));
    memset(&translatedJoint, 0, sizeof(translatedJoint));
    memset(&fullMaterial, 0, sizeof(fullMaterial));
    memset(&fadedMaterial, 0, sizeof(fadedMaterial));
    memset(&drawObject, 0, sizeof(drawObject));
    memset(&copiedMObj, 0, sizeof(copiedMObj));

    if (!PCPort_LoadFsysMember(PCPORT_REAL_CONTENT_ARCHIVE,
                               PCPORT_REAL_CONTENT_MEMBER,
                               &memberData,
                               &memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta load failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    if (!PCPort_HSDArchiveParseBE(&archive, memberData, memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta archive parse failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&archive,
                                                             "scene_data",
                                                             &sceneOffset);
    if (sceneData == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta failed to resolve scene_data\n");
        goto cleanup;
    }

    sceneBranchOffset = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&archive, sceneBranchOffset, 0x10u)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta branch offset was invalid (0x%X)\n",
                sceneBranchOffset);
        goto cleanup;
    }

    cameraDescOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x08);
    if (!PCPort_TranslatePerspectiveCameraFromArchiveBE(&archive,
                                                        cameraDescOffset,
                                                        &translatedCamera)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta failed to translate camera (scene=0x%X camera=0x%X)\n",
                sceneOffset,
                cameraDescOffset);
        goto cleanup;
    }

    sceneJointListOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x00);
    rootJointOffset = PCPort_ReadBigEndianU32(archive.storage + sceneJointListOffset + 0x00);
    if (!ResolveFirstRenderablePObjDescFromJoint(&archive,
                                                 rootJointOffset,
                                                 &jointOffset,
                                                 &dobjOffset,
                                                 &pobjOffset)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta found no renderable PObjDesc (rootJoint=0x%X)\n",
                rootJointOffset);
        goto cleanup;
    }

    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           pobjOffset,
                                           &translatedPObj)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta failed to translate PObjDesc (joint=0x%X dobj=0x%X pobj=0x%X)\n",
                jointOffset,
                dobjOffset,
                pobjOffset);
        goto cleanup;
    }

    if (!PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              jointOffset,
                                              &translatedJoint)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta failed to translate joint chain (root=0x%X target=0x%X)\n",
                rootJointOffset,
                jointOffset);
        goto cleanup;
    }

    mobjOffset = PCPort_ReadBigEndianU32(archive.storage + dobjOffset + 0x08);
    if (!ArchiveRangeValid(&archive, mobjOffset, 0x18u)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta MObj offset was invalid (dobj=0x%X mobj=0x%X)\n",
                dobjOffset,
                mobjOffset);
        goto cleanup;
    }

    if (PCPort_SwizzleSceneForHSD(&archive, sceneJointListOffset) == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta failed to swizzle scene for live HSD load (jointList=0x%X)\n",
                sceneJointListOffset);
        goto cleanup;
    }

    liveMObj = HSD_MObjLoadDesc((HSD_MObjDesc*)(archive.storage + mobjOffset));
    if (liveMObj == NULL || liveMObj->mat == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta live MObj load failed (mobj=0x%X live=%p mat=%p)\n",
                mobjOffset,
                (void*)liveMObj,
                liveMObj != NULL ? (void*)liveMObj->mat : NULL);
        goto cleanup;
    }

    liveMObj->rendermode |= PCPORT_RENDER_XLU | PCPORT_RENDER_NO_ZUPDATE;
    if (HSD_MOBJ_METHOD(liveMObj)->make_texp == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta live MObj has no make_texp method\n");
        goto cleanup;
    }
    materialAnimOriginalDiffuse = liveMObj->mat->diffuse;
    materialAnimOriginalAlpha = liveMObj->mat->alpha;
    materialAnimValue.fv = 0.125f;
    MObjUpdateFunc(liveMObj, 4u, &materialAnimValue);
    materialAnimDiffuseR = (liveMObj->mat->diffuse >> 24) & 0xFFu;
    materialAnimValue.fv = 0.75f;
    MObjUpdateFunc(liveMObj, 7u, &materialAnimValue);
    materialAnimAlpha = liveMObj->mat->alpha;
    if (materialAnimDiffuseR != 31u ||
        !(materialAnimAlpha > 0.249f && materialAnimAlpha < 0.251f)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta MObj anim dispatcher failed (mobj=0x%X diffuseR=%u alpha=%.3f)\n",
                mobjOffset,
                materialAnimDiffuseR,
                materialAnimAlpha);
        goto cleanup;
    }
    liveMObj->mat->diffuse = materialAnimOriginalDiffuse;
    liveMObj->mat->alpha = materialAnimOriginalAlpha;

    if (fn_801A7D58(&copiedMObj, liveMObj) != 0 ||
        copiedMObj.mat == NULL ||
        copiedMObj.mat == liveMObj->mat ||
        copiedMObj.mat->diffuse != liveMObj->mat->diffuse ||
        copiedMObj.tobj == liveMObj->tobj ||
        (liveMObj->tobj != NULL && copiedMObj.tobj == NULL) ||
        (copiedMObj.rendermode & RENDER_TOON) == 0u) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta MObj copy helper failed (mobj=0x%X srcMat=%p dstMat=%p srcTObj=%p dstTObj=%p dstMode=0x%X)\n",
                mobjOffset,
                (void*)liveMObj->mat,
                (void*)copiedMObj.mat,
                (void*)liveMObj->tobj,
                (void*)copiedMObj.tobj,
                copiedMObj.rendermode);
        goto cleanup;
    }

    liveMObj->texp = NULL;
    HSD_MObjSetup(liveMObj, liveMObj->rendermode);
    setupTExpList = liveMObj->texp;
    if (setupTExpList == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta live MObj setup produced no TExp list (mobj=0x%X)\n",
                mobjOffset);
        goto cleanup;
    }
    loadTExpRoot = HSD_MOBJ_METHOD(liveMObj)->make_texp(liveMObj,
                                                        liveMObj->tobj,
                                                        &loadTExpList);
    liveMObj->texp = loadTExpList;
    if (loadTExpRoot == NULL || liveMObj->texp == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta live MObj produced no TExp list (root=%p list=%p mobj=0x%X)\n",
                (void*)loadTExpRoot,
                (void*)liveMObj->texp,
                mobjOffset);
        goto cleanup;
    }

    HSD_MObjSetAlpha(liveMObj, 1.0f);
    fullTExpRoot = HSD_MOBJ_METHOD(liveMObj)->make_texp(liveMObj,
                                                        liveMObj->tobj,
                                                        &fullTExpList);
    if (fullTExpRoot == NULL || fullTExpList == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta full-alpha TExp rebuild failed (root=%p list=%p)\n",
                (void*)fullTExpRoot,
                (void*)fullTExpList);
        goto cleanup;
    }
    TranslateLiveMObjMaterial(liveMObj, &fullMaterial);
    fullMaterial.mobjArchiveOffset = mobjOffset;
    ForceMaterialDeltaBlend(&fullMaterial);

    drawObject.displayList = translatedPObj.pobj.display;
    drawObject.displayListSize = translatedPObj.pobj.n_display;
    drawObject.pipelineId = PCPORT_REAL_MATERIAL_PIPELINE;
    drawObject.totalVerts = translatedPObj.totalSubmittedVertices;
    drawObject.totalPrims = translatedPObj.totalPrimitiveCommands;

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedJoint.modelMatrix,
                    modelViewMatrix);

    fullPixels = CaptureMaterialDeltaFrame(&translatedCamera,
                                           &translatedPObj,
                                           &fullMaterial,
                                           modelViewMatrix,
                                           &drawObject,
                                           fullSample);
    if (fullPixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta failed to capture full-alpha framebuffer\n");
        goto cleanup;
    }

    HSD_MObjSetAlpha(liveMObj, 0.25f);
    fadedTExpRoot = HSD_MOBJ_METHOD(liveMObj)->make_texp(liveMObj,
                                                         liveMObj->tobj,
                                                         &fadedTExpList);
    if (fadedTExpRoot == NULL || fadedTExpList == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta faded-alpha TExp rebuild failed (root=%p list=%p)\n",
                (void*)fadedTExpRoot,
                (void*)fadedTExpList);
        goto cleanup;
    }
    TranslateLiveMObjMaterial(liveMObj, &fadedMaterial);
    fadedMaterial.mobjArchiveOffset = mobjOffset;
    ForceMaterialDeltaBlend(&fadedMaterial);

    if (!(fullMaterial.alpha > 0.99f && fadedMaterial.alpha < 0.26f)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta alpha setter did not update live material (full=%.3f faded=%.3f)\n",
                fullMaterial.alpha,
                fadedMaterial.alpha);
        goto cleanup;
    }

    fadedPixels = CaptureMaterialDeltaFrame(&translatedCamera,
                                            &translatedPObj,
                                            &fadedMaterial,
                                            modelViewMatrix,
                                            &drawObject,
                                            fadedSample);
    if (fadedPixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta failed to capture faded-alpha framebuffer\n");
        goto cleanup;
    }

    diffPixels = CountFramebufferDiffPixels(fullPixels, fadedPixels);
    if (diffPixels == 0u) {
        fprintf(stderr,
                "[pcport_bootstrap] Real material delta produced no framebuffer delta (scene=0x%X joint=0x%X dobj=0x%X mobj=0x%X pobj=0x%X full=%u,%u,%u,%u faded=%u,%u,%u,%u)\n",
                sceneOffset,
                jointOffset,
                dobjOffset,
                mobjOffset,
                pobjOffset,
                fullSample[0],
                fullSample[1],
                fullSample[2],
                fullSample[3],
                fadedSample[0],
                fadedSample[1],
                fadedSample[2],
                fadedSample[3]);
        goto cleanup;
    }

    printf("[pcport_bootstrap] Real material delta smoke passed (scene=0x%X camera=0x%X joint=0x%X dobj=0x%X mobj=0x%X pobj=0x%X diffPixels=%u full=%u,%u,%u,%u faded=%u,%u,%u,%u fullAlpha=%.3f fadedAlpha=%.3f animDiffuseR=%u animAlpha=%.3f setupTExp=%p loadTExp=%p fullTExp=%p fadedTExp=%p copyMat=%p copyTObj=%p submitted=%u expanded=%u prim=0x%X)\n",
           sceneOffset,
           translatedCamera.cameraArchiveOffset,
           jointOffset,
           dobjOffset,
           mobjOffset,
           pobjOffset,
           diffPixels,
           fullSample[0],
           fullSample[1],
           fullSample[2],
           fullSample[3],
           fadedSample[0],
           fadedSample[1],
           fadedSample[2],
           fadedSample[3],
           fullMaterial.alpha,
           fadedMaterial.alpha,
           materialAnimDiffuseR,
           materialAnimAlpha,
           (void*)setupTExpList,
           (void*)loadTExpRoot,
           (void*)fullTExpRoot,
           (void*)fadedTExpRoot,
           (void*)copiedMObj.mat,
           (void*)copiedMObj.tobj,
           GXHostGetLastSubmittedVertexCount(),
           GXHostGetLastExpandedVertexCount(),
           GXHostGetLastSubmittedPrimitive());
    ok = 1;

cleanup:
    GSgfxHostClearPipelineState(PCPORT_REAL_MATERIAL_PIPELINE);
    GXHostSetVertexAlphaScale(1.0f);
    HSD_TObjRemoveAll(copiedMObj.tobj);
    HSD_Free(copiedMObj.mat);
    HSD_Free(copiedMObj.pe);
    free(fadedPixels);
    free(fullPixels);
    PCPort_DestroyTranslatedPObj(&translatedPObj);
    PCPort_HSDArchiveDestroy(&archive);
    PCPort_FreeBuffer(memberData);
    return ok;
}

static int RunRealTexturedSceneSliceSmoke(void) {
    PCPortHSDArchive archive;
    PCPortTranslatedCamera translatedCamera;
    PCPortTranslatedPObj translatedBasePObj;
    PCPortTranslatedJointTransform translatedBaseJoint;
    PCPortTranslatedMaterial translatedBaseMaterial;
    PCPortTranslatedPObj translatedTexturedPObj;
    PCPortTranslatedJointTransform translatedTexturedJoint;
    PCPortTranslatedMaterial translatedTexturedMaterial;
    PCPortTranslatedTextureExp translatedTextureExp;
    PCPortGSDrawObject baseDrawObject;
    PCPortGSDrawObject texturedDrawObject;
    GXTexObj textureObject;
    u8* memberData = NULL;
    u8* bakedTexturePixels = NULL;
    u32 memberSize = 0;
    u32 bakedTextureSize = 0;
    const u8* sceneData;
    unsigned char* materialPixels = NULL;
    unsigned char* texturedPixels = NULL;
    unsigned char materialOutside[4] = { 0 };
    unsigned char texturedOutside[4] = { 0 };
    unsigned char texturedCenter[4] = { 0 };
    u32 sceneOffset = 0;
    u32 sceneBranchOffset;
    u32 cameraDescOffset;
    u32 sceneJointListOffset;
    u32 rootJointOffset;
    u32 baseJointOffset = 0;
    u32 baseDObjOffset = 0;
    u32 basePObjOffset = 0;
    u32 baseMObjOffset = 0;
    u32 texturedTObjOffset = 0;
    u32 textureMapId = 0;
    unsigned int diffPixels = 0;
    unsigned int texturedSubmitted = 0;
    unsigned int texturedExpanded = 0;
    unsigned int texturedPrimitive = 0;
    f32 baseModelViewMatrix[3][4];
    f32 texturedModelViewMatrix[3][4];
    int ok = 0;

    memset(&archive, 0, sizeof(archive));
    memset(&translatedCamera, 0, sizeof(translatedCamera));
    memset(&translatedBasePObj, 0, sizeof(translatedBasePObj));
    memset(&translatedBaseJoint, 0, sizeof(translatedBaseJoint));
    memset(&translatedBaseMaterial, 0, sizeof(translatedBaseMaterial));
    memset(&translatedTexturedPObj, 0, sizeof(translatedTexturedPObj));
    memset(&translatedTexturedJoint, 0, sizeof(translatedTexturedJoint));
    memset(&translatedTexturedMaterial, 0, sizeof(translatedTexturedMaterial));
    memset(&translatedTextureExp, 0, sizeof(translatedTextureExp));
    memset(&baseDrawObject, 0, sizeof(baseDrawObject));
    memset(&texturedDrawObject, 0, sizeof(texturedDrawObject));
    memset(&textureObject, 0, sizeof(textureObject));

    if (!PCPort_LoadFsysMember(PCPORT_REAL_CONTENT_ARCHIVE,
                               PCPORT_REAL_CONTENT_MEMBER,
                               &memberData,
                               &memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real textured scene slice load failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    if (!PCPort_HSDArchiveParseBE(&archive, memberData, memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real textured scene slice archive parse failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&archive,
                                                             "scene_data",
                                                             &sceneOffset);
    if (sceneData == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real textured scene slice failed to resolve scene_data\n");
        goto cleanup;
    }

    sceneBranchOffset = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&archive, sceneBranchOffset, 0x10u)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real textured scene slice branch offset was invalid (0x%X)\n",
                sceneBranchOffset);
        goto cleanup;
    }

    cameraDescOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x08);
    if (!PCPort_TranslatePerspectiveCameraFromArchiveBE(&archive,
                                                        cameraDescOffset,
                                                        &translatedCamera)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate real textured scene camera (scene=0x%X camera=0x%X)\n",
                sceneOffset,
                cameraDescOffset);
        goto cleanup;
    }

    sceneJointListOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x00);
    rootJointOffset = PCPort_ReadBigEndianU32(archive.storage + sceneJointListOffset + 0x00);
    if (!ResolveFirstRenderablePObjDescFromJoint(&archive,
                                                 rootJointOffset,
                                                 &baseJointOffset,
                                                 &baseDObjOffset,
                                                 &basePObjOffset)) {
        fprintf(stderr,
                "[pcport_bootstrap] No baseline renderable PObjDesc was found under scene_data (rootJoint=0x%X)\n",
                rootJointOffset);
        goto cleanup;
    }

    baseMObjOffset = PCPort_ReadBigEndianU32(archive.storage + baseDObjOffset + 0x08);
    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           basePObjOffset,
                                           &translatedBasePObj) ||
        !PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              baseJointOffset,
                                              &translatedBaseJoint) ||
        !PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               baseMObjOffset,
                                               &translatedBaseMaterial)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate baseline real-content path (joint=0x%X dobj=0x%X mobj=0x%X pobj=0x%X)\n",
                baseJointOffset,
                baseDObjOffset,
                baseMObjOffset,
                basePObjOffset);
        goto cleanup;
    }

    texturedTObjOffset = PCPort_ReadBigEndianU32(archive.storage + PCPORT_TEXTURED_MOBJ_OFFSET + 0x08);
    textureMapId = PCPort_ReadBigEndianU32(archive.storage + texturedTObjOffset + 0x08);
    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           PCPORT_TEXTURED_POBJ_OFFSET,
                                           &translatedTexturedPObj) ||
        !PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              PCPORT_TEXTURED_JOINT_OFFSET,
                                              &translatedTexturedJoint) ||
        !PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               PCPORT_TEXTURED_MOBJ_OFFSET,
                                               &translatedTexturedMaterial) ||
        !PCPort_TranslateTextureExpFromArchiveBE(&archive,
                                                 texturedTObjOffset,
                                                 &translatedTextureExp)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate textured front branch (joint=0x%X dobj=0x%X mobj=0x%X tobj=0x%X pobj=0x%X)\n",
                PCPORT_TEXTURED_JOINT_OFFSET,
                PCPORT_TEXTURED_DOBJ_OFFSET,
                PCPORT_TEXTURED_MOBJ_OFFSET,
                texturedTObjOffset,
                PCPORT_TEXTURED_POBJ_OFFSET);
        goto cleanup;
    }

    if (translatedTextureExp.kind != PCPORT_TEXTURE_EXP_KIND_DIRECT_SAMPLE ||
        translatedTextureExp.stageCount != 1u ||
        translatedTextureExp.stages[0].kind != PCPORT_TEXP_STAGE_DIRECT_SAMPLE) {
        fprintf(stderr,
                "[pcport_bootstrap] Real textured scene slice did not reach the expected shared direct-sample TExp boundary (texTObj=0x%X kind=%u stage0=%u stages=%u)\n",
                texturedTObjOffset,
                translatedTextureExp.kind,
                translatedTextureExp.stageCount != 0u ? translatedTextureExp.stages[0].kind : 0u,
                translatedTextureExp.stageCount);
        goto cleanup;
    }

    if (!PCPort_BakeTextureExpRGBAFromArchiveBE(&archive,
                                                &translatedTextureExp,
                                                &bakedTexturePixels,
                                                &bakedTextureSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to bake shared direct-sample TExp payload (texTObj=0x%X stage0=%u stages=%u)\n",
                texturedTObjOffset,
                translatedTextureExp.stages[0].kind,
                translatedTextureExp.stageCount);
        goto cleanup;
    }

    GXHostInitTexObjRGBA8(&textureObject,
                          bakedTexturePixels,
                          translatedTextureExp.stages[0].texture.width,
                          translatedTextureExp.stages[0].texture.height,
                          (GXTexWrapMode)translatedTextureExp.stages[0].texture.wrapS,
                          (GXTexWrapMode)translatedTextureExp.stages[0].texture.wrapT);

    baseDrawObject.displayList = translatedBasePObj.pobj.display;
    baseDrawObject.displayListSize = translatedBasePObj.pobj.n_display;
    baseDrawObject.pipelineId = PCPORT_REAL_MATERIAL_PIPELINE;
    baseDrawObject.totalVerts = translatedBasePObj.totalSubmittedVertices;
    baseDrawObject.totalPrims = translatedBasePObj.totalPrimitiveCommands;

    texturedDrawObject.displayList = translatedTexturedPObj.pobj.display;
    texturedDrawObject.displayListSize = translatedTexturedPObj.pobj.n_display;
    texturedDrawObject.pipelineId = PCPORT_REAL_TEXTURED_PIPELINE;
    texturedDrawObject.totalVerts = translatedTexturedPObj.totalSubmittedVertices;
    texturedDrawObject.totalPrims = translatedTexturedPObj.totalPrimitiveCommands;

    ConfigureTranslatedMaterialPipeline(PCPORT_REAL_MATERIAL_PIPELINE,
                                        &translatedBaseMaterial);
    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_TEXTURED_PIPELINE,
                                        &translatedTexturedMaterial,
                                        &translatedTextureExp.stages[0].texture,
                                        &textureObject,
                                        (unsigned char)textureMapId);

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    VIWaitForRetrace_PC();

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();

    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);
    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedBaseJoint.modelMatrix,
                    baseModelViewMatrix);
    GXLoadPosMtxImm(baseModelViewMatrix, 0);
    GXSetCurrentMtx(0);

    fn_801AA568(&translatedBasePObj.pobj);
    fn_800DAD10((void*)&baseDrawObject);
    glFlush();

    materialPixels = ReadBackbufferImage();
    if (materialPixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture baseline framebuffer for textured scene slice\n");
        goto cleanup;
    }

    ReadBackbufferPixelAt(PCPORT_GX_OUTSIDE_X,
                          PCPORT_GX_OUTSIDE_Y,
                          materialOutside);

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();

    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);

    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedTexturedJoint.modelMatrix,
                    texturedModelViewMatrix);
    GXLoadPosMtxImm(texturedModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedTexturedPObj.pobj);
    fn_800DAD10((void*)&texturedDrawObject);
    glFlush();

    texturedSubmitted = GXHostGetLastSubmittedVertexCount();
    texturedExpanded = GXHostGetLastExpandedVertexCount();
    texturedPrimitive = GXHostGetLastSubmittedPrimitive();

    GXLoadPosMtxImm(baseModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedBasePObj.pobj);
    fn_800DAD10((void*)&baseDrawObject);
    glFlush();

    texturedPixels = ReadBackbufferImage();
    if (texturedPixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture textured framebuffer for scene slice\n");
        goto cleanup;
    }

    ReadBackbufferPixelAt(PCPORT_GX_OUTSIDE_X,
                          PCPORT_GX_OUTSIDE_Y,
                          texturedOutside);
    ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                          PCPORT_GX_SMOKE_SAMPLE_Y,
                          texturedCenter);

    diffPixels = CountFramebufferDiffPixels(materialPixels, texturedPixels);
    if (diffPixels == 0u ||
        (materialOutside[0] == texturedOutside[0] &&
         materialOutside[1] == texturedOutside[1] &&
         materialOutside[2] == texturedOutside[2] &&
         materialOutside[3] == texturedOutside[3])) {
        fprintf(stderr,
                "[pcport_bootstrap] Real textured scene slice reached the draw bridge but did not produce a distinct textured result (scene=0x%X camera=0x%X baseJoint=0x%X baseDObj=0x%X baseMObj=0x%X basePObj=0x%X texJoint=0x%X texDObj=0x%X texMObj=0x%X texTObj=0x%X texImage=0x%X texData=0x%X texPObj=0x%X stage0=%u stages=%u diffPixels=%u baseOutside=%u,%u,%u,%u texturedOutside=%u,%u,%u,%u center=%u,%u,%u,%u tev=%u size=%ux%u format=%u baked=%u submitted=%u expanded=%u prim=0x%X)\n",
                sceneOffset,
                translatedCamera.cameraArchiveOffset,
                baseJointOffset,
                baseDObjOffset,
                baseMObjOffset,
                basePObjOffset,
                PCPORT_TEXTURED_JOINT_OFFSET,
                PCPORT_TEXTURED_DOBJ_OFFSET,
                PCPORT_TEXTURED_MOBJ_OFFSET,
                translatedTextureExp.stages[0].texture.tobjArchiveOffset,
                translatedTextureExp.stages[0].texture.imageArchiveOffset,
                translatedTextureExp.stages[0].texture.imageDataArchiveOffset,
                PCPORT_TEXTURED_POBJ_OFFSET,
                translatedTextureExp.stages[0].kind,
                translatedTextureExp.stageCount,
                diffPixels,
                materialOutside[0],
                materialOutside[1],
                materialOutside[2],
                materialOutside[3],
                texturedOutside[0],
                texturedOutside[1],
                texturedOutside[2],
                texturedOutside[3],
                texturedCenter[0],
                texturedCenter[1],
                texturedCenter[2],
                texturedCenter[3],
                translatedTextureExp.stages[0].texture.tevMode,
                translatedTextureExp.stages[0].texture.width,
                translatedTextureExp.stages[0].texture.height,
                translatedTextureExp.stages[0].texture.format,
                bakedTextureSize,
                texturedSubmitted,
                texturedExpanded,
                texturedPrimitive);
        goto cleanup;
    }

    printf("[pcport_bootstrap] Real textured scene slice smoke passed (scene=0x%X camera=0x%X baseJoint=0x%X baseDObj=0x%X baseMObj=0x%X basePObj=0x%X texJoint=0x%X texDObj=0x%X texMObj=0x%X texTObj=0x%X texImage=0x%X texData=0x%X texPObj=0x%X kind=%u stage0=%u stages=%u diffPixels=%u baseOutside=%u,%u,%u,%u texturedOutside=%u,%u,%u,%u center=%u,%u,%u,%u tev=%u size=%ux%u format=%u baked=%u submitted=%u expanded=%u prim=0x%X)\n",
           sceneOffset,
           translatedCamera.cameraArchiveOffset,
           baseJointOffset,
           baseDObjOffset,
           baseMObjOffset,
           basePObjOffset,
           PCPORT_TEXTURED_JOINT_OFFSET,
           PCPORT_TEXTURED_DOBJ_OFFSET,
           PCPORT_TEXTURED_MOBJ_OFFSET,
           translatedTextureExp.stages[0].texture.tobjArchiveOffset,
           translatedTextureExp.stages[0].texture.imageArchiveOffset,
           translatedTextureExp.stages[0].texture.imageDataArchiveOffset,
           PCPORT_TEXTURED_POBJ_OFFSET,
           translatedTextureExp.kind,
           translatedTextureExp.stages[0].kind,
           translatedTextureExp.stageCount,
           diffPixels,
           materialOutside[0],
           materialOutside[1],
           materialOutside[2],
           materialOutside[3],
           texturedOutside[0],
           texturedOutside[1],
           texturedOutside[2],
           texturedOutside[3],
           texturedCenter[0],
           texturedCenter[1],
           texturedCenter[2],
           texturedCenter[3],
           translatedTextureExp.stages[0].texture.tevMode,
           translatedTextureExp.stages[0].texture.width,
           translatedTextureExp.stages[0].texture.height,
           translatedTextureExp.stages[0].texture.format,
           bakedTextureSize,
           texturedSubmitted,
           texturedExpanded,
           texturedPrimitive);
    ok = 1;

cleanup:
    GSgfxHostClearPipelineState(PCPORT_REAL_TEXTURED_PIPELINE);
    GSgfxHostClearPipelineState(PCPORT_REAL_MATERIAL_PIPELINE);
    GXHostClearTextureBinding();
    GXHostSetVertexAlphaScale(1.0f);
    PCPort_FreeBuffer(bakedTexturePixels);
    free(texturedPixels);
    free(materialPixels);
    PCPort_DestroyTranslatedPObj(&translatedTexturedPObj);
    PCPort_DestroyTranslatedPObj(&translatedBasePObj);
    PCPort_HSDArchiveDestroy(&archive);
    PCPort_FreeBuffer(memberData);
    return ok;
}

static int RunRealSceneSlice3Smoke(void) {
    PCPortHSDArchive archive;
    PCPortTranslatedCamera translatedCamera;
    PCPortTranslatedPObj translatedBasePObj;
    PCPortTranslatedJointTransform translatedBaseJoint;
    PCPortTranslatedMaterial translatedBaseMaterial;
    PCPortTranslatedPObj translatedFrontPObj;
    PCPortTranslatedJointTransform translatedFrontJoint;
    PCPortTranslatedMaterial translatedFrontMaterial;
    PCPortTranslatedTexture translatedFrontTexture;
    PCPortTranslatedPObj translatedSiblingPObj;
    PCPortTranslatedJointTransform translatedSiblingJoint;
    PCPortTranslatedMaterial translatedSiblingMaterial;
    PCPortTranslatedTexture translatedSiblingTexture;
    PCPortGSDrawObject baseDrawObject;
    PCPortGSDrawObject frontDrawObject;
    PCPortGSDrawObject siblingDrawObject;
    GXTexObj frontTextureObject;
    GXTexObj siblingTextureObject;
    u8* memberData = NULL;
    u32 memberSize = 0;
    const u8* sceneData;
    unsigned char* baselinePixels = NULL;
    unsigned char* multiPixels = NULL;
    unsigned char baselineCenter[4] = { 0 };
    unsigned char multiCenter[4] = { 0 };
    u32 sceneOffset = 0;
    u32 sceneBranchOffset;
    u32 cameraDescOffset;
    u32 sceneJointListOffset;
    u32 rootJointOffset;
    u32 baseJointOffset = 0;
    u32 baseDObjOffset = 0;
    u32 basePObjOffset = 0;
    u32 baseMObjOffset = 0;
    u32 frontTObjOffset = 0;
    u32 frontTextureMapId = 0;
    u32 siblingTObjOffset = 0;
    u32 siblingTextureMapId = 0;
    unsigned int diffPixels = 0;
    unsigned int siblingSubmitted = 0;
    unsigned int siblingExpanded = 0;
    unsigned int siblingPrimitive = 0;
    f32 baseModelViewMatrix[3][4];
    f32 frontModelViewMatrix[3][4];
    f32 siblingModelViewMatrix[3][4];
    int ok = 0;

    memset(&archive, 0, sizeof(archive));
    memset(&translatedCamera, 0, sizeof(translatedCamera));
    memset(&translatedBasePObj, 0, sizeof(translatedBasePObj));
    memset(&translatedBaseJoint, 0, sizeof(translatedBaseJoint));
    memset(&translatedBaseMaterial, 0, sizeof(translatedBaseMaterial));
    memset(&translatedFrontPObj, 0, sizeof(translatedFrontPObj));
    memset(&translatedFrontJoint, 0, sizeof(translatedFrontJoint));
    memset(&translatedFrontMaterial, 0, sizeof(translatedFrontMaterial));
    memset(&translatedFrontTexture, 0, sizeof(translatedFrontTexture));
    memset(&translatedSiblingPObj, 0, sizeof(translatedSiblingPObj));
    memset(&translatedSiblingJoint, 0, sizeof(translatedSiblingJoint));
    memset(&translatedSiblingMaterial, 0, sizeof(translatedSiblingMaterial));
    memset(&translatedSiblingTexture, 0, sizeof(translatedSiblingTexture));
    memset(&baseDrawObject, 0, sizeof(baseDrawObject));
    memset(&frontDrawObject, 0, sizeof(frontDrawObject));
    memset(&siblingDrawObject, 0, sizeof(siblingDrawObject));
    memset(&frontTextureObject, 0, sizeof(frontTextureObject));
    memset(&siblingTextureObject, 0, sizeof(siblingTextureObject));

    if (!PCPort_LoadFsysMember(PCPORT_REAL_CONTENT_ARCHIVE,
                               PCPORT_REAL_CONTENT_MEMBER,
                               &memberData,
                               &memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 3 load failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    if (!PCPort_HSDArchiveParseBE(&archive, memberData, memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 3 archive parse failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&archive,
                                                             "scene_data",
                                                             &sceneOffset);
    if (sceneData == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 3 failed to resolve scene_data\n");
        goto cleanup;
    }

    sceneBranchOffset = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&archive, sceneBranchOffset, 0x10u)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 3 branch offset was invalid (0x%X)\n",
                sceneBranchOffset);
        goto cleanup;
    }

    cameraDescOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x08);
    if (!PCPort_TranslatePerspectiveCameraFromArchiveBE(&archive,
                                                        cameraDescOffset,
                                                        &translatedCamera)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate real scene slice 3 camera (scene=0x%X camera=0x%X)\n",
                sceneOffset,
                cameraDescOffset);
        goto cleanup;
    }

    sceneJointListOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x00);
    rootJointOffset = PCPort_ReadBigEndianU32(archive.storage + sceneJointListOffset + 0x00);
    if (!ResolveFirstRenderablePObjDescFromJoint(&archive,
                                                 rootJointOffset,
                                                 &baseJointOffset,
                                                 &baseDObjOffset,
                                                 &basePObjOffset)) {
        fprintf(stderr,
                "[pcport_bootstrap] No baseline renderable PObjDesc was found for scene slice 3 (rootJoint=0x%X)\n",
                rootJointOffset);
        goto cleanup;
    }

    baseMObjOffset = PCPort_ReadBigEndianU32(archive.storage + baseDObjOffset + 0x08);
    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           basePObjOffset,
                                           &translatedBasePObj) ||
        !PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              baseJointOffset,
                                              &translatedBaseJoint) ||
        !PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               baseMObjOffset,
                                               &translatedBaseMaterial)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate baseline branch for scene slice 3 (joint=0x%X dobj=0x%X mobj=0x%X pobj=0x%X)\n",
                baseJointOffset,
                baseDObjOffset,
                baseMObjOffset,
                basePObjOffset);
        goto cleanup;
    }

    frontTObjOffset = PCPort_ReadBigEndianU32(archive.storage + PCPORT_TEXTURED_MOBJ_OFFSET + 0x08);
    frontTextureMapId = PCPort_ReadBigEndianU32(archive.storage + frontTObjOffset + 0x08);
    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           PCPORT_TEXTURED_POBJ_OFFSET,
                                           &translatedFrontPObj) ||
        !PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              PCPORT_TEXTURED_JOINT_OFFSET,
                                              &translatedFrontJoint) ||
        !PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               PCPORT_TEXTURED_MOBJ_OFFSET,
                                               &translatedFrontMaterial) ||
        !PCPort_TranslateTextureFromArchiveBE(&archive,
                                              frontTObjOffset,
                                              &translatedFrontTexture)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate front textured branch for scene slice 3 (joint=0x%X dobj=0x%X mobj=0x%X tobj=0x%X pobj=0x%X)\n",
                PCPORT_TEXTURED_JOINT_OFFSET,
                PCPORT_TEXTURED_DOBJ_OFFSET,
                PCPORT_TEXTURED_MOBJ_OFFSET,
                frontTObjOffset,
                PCPORT_TEXTURED_POBJ_OFFSET);
        goto cleanup;
    }

    siblingTObjOffset = PCPort_ReadBigEndianU32(archive.storage + PCPORT_SIBLING_MOBJ_OFFSET + 0x08);
    siblingTextureMapId = PCPort_ReadBigEndianU32(archive.storage + siblingTObjOffset + 0x08);
    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           PCPORT_SIBLING_POBJ_OFFSET,
                                           &translatedSiblingPObj) ||
        !PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              PCPORT_SIBLING_JOINT_OFFSET,
                                              &translatedSiblingJoint) ||
        !PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               PCPORT_SIBLING_MOBJ_OFFSET,
                                               &translatedSiblingMaterial) ||
        !PCPort_TranslateTextureFromArchiveBE(&archive,
                                              siblingTObjOffset,
                                              &translatedSiblingTexture)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate sibling branch for scene slice 3 (joint=0x%X dobj=0x%X mobj=0x%X tobj=0x%X pobj=0x%X)\n",
                PCPORT_SIBLING_JOINT_OFFSET,
                PCPORT_SIBLING_DOBJ_OFFSET,
                PCPORT_SIBLING_MOBJ_OFFSET,
                siblingTObjOffset,
                PCPORT_SIBLING_POBJ_OFFSET);
        goto cleanup;
    }

    GXInitTexObj(&frontTextureObject,
                 archive.storage + translatedFrontTexture.imageDataArchiveOffset,
                 translatedFrontTexture.width,
                 translatedFrontTexture.height,
                 (GXTexFmt)translatedFrontTexture.format,
                 (GXTexWrapMode)translatedFrontTexture.wrapS,
                 (GXTexWrapMode)translatedFrontTexture.wrapT,
                 translatedFrontTexture.mipmap);

    GXInitTexObj(&siblingTextureObject,
                 archive.storage + translatedSiblingTexture.imageDataArchiveOffset,
                 translatedSiblingTexture.width,
                 translatedSiblingTexture.height,
                 (GXTexFmt)translatedSiblingTexture.format,
                 (GXTexWrapMode)translatedSiblingTexture.wrapS,
                 (GXTexWrapMode)translatedSiblingTexture.wrapT,
                 translatedSiblingTexture.mipmap);

    baseDrawObject.displayList = translatedBasePObj.pobj.display;
    baseDrawObject.displayListSize = translatedBasePObj.pobj.n_display;
    baseDrawObject.pipelineId = PCPORT_REAL_MATERIAL_PIPELINE;
    baseDrawObject.totalVerts = translatedBasePObj.totalSubmittedVertices;
    baseDrawObject.totalPrims = translatedBasePObj.totalPrimitiveCommands;

    frontDrawObject.displayList = translatedFrontPObj.pobj.display;
    frontDrawObject.displayListSize = translatedFrontPObj.pobj.n_display;
    frontDrawObject.pipelineId = PCPORT_REAL_TEXTURED_PIPELINE;
    frontDrawObject.totalVerts = translatedFrontPObj.totalSubmittedVertices;
    frontDrawObject.totalPrims = translatedFrontPObj.totalPrimitiveCommands;

    siblingDrawObject.displayList = translatedSiblingPObj.pobj.display;
    siblingDrawObject.displayListSize = translatedSiblingPObj.pobj.n_display;
    siblingDrawObject.pipelineId = PCPORT_REAL_SIBLING_TEXTURED_PIPELINE;
    siblingDrawObject.totalVerts = translatedSiblingPObj.totalSubmittedVertices;
    siblingDrawObject.totalPrims = translatedSiblingPObj.totalPrimitiveCommands;

    ConfigureTranslatedMaterialPipeline(PCPORT_REAL_MATERIAL_PIPELINE,
                                        &translatedBaseMaterial);
    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_TEXTURED_PIPELINE,
                                        &translatedFrontMaterial,
                                        &translatedFrontTexture,
                                        &frontTextureObject,
                                        (unsigned char)frontTextureMapId);
    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_SIBLING_TEXTURED_PIPELINE,
                                        &translatedSiblingMaterial,
                                        &translatedSiblingTexture,
                                        &siblingTextureObject,
                                        (unsigned char)siblingTextureMapId);

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    VIWaitForRetrace_PC();

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();

    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);

    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedFrontJoint.modelMatrix,
                    frontModelViewMatrix);
    GXLoadPosMtxImm(frontModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedFrontPObj.pobj);
    fn_800DAD10((void*)&frontDrawObject);
    glFlush();

    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedBaseJoint.modelMatrix,
                    baseModelViewMatrix);
    GXLoadPosMtxImm(baseModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedBasePObj.pobj);
    fn_800DAD10((void*)&baseDrawObject);
    glFlush();

    baselinePixels = ReadBackbufferImage();
    if (baselinePixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture baseline framebuffer for scene slice 3\n");
        goto cleanup;
    }

    ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                          PCPORT_GX_SMOKE_SAMPLE_Y,
                          baselineCenter);

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();

    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);

    GXLoadPosMtxImm(frontModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedFrontPObj.pobj);
    fn_800DAD10((void*)&frontDrawObject);
    glFlush();

    GXLoadPosMtxImm(baseModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedBasePObj.pobj);
    fn_800DAD10((void*)&baseDrawObject);
    glFlush();

    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedSiblingJoint.modelMatrix,
                    siblingModelViewMatrix);
    GXLoadPosMtxImm(siblingModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedSiblingPObj.pobj);
    fn_800DAD10((void*)&siblingDrawObject);
    glFlush();

    siblingSubmitted = GXHostGetLastSubmittedVertexCount();
    siblingExpanded = GXHostGetLastExpandedVertexCount();
    siblingPrimitive = GXHostGetLastSubmittedPrimitive();

    multiPixels = ReadBackbufferImage();
    if (multiPixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture multi-object framebuffer for scene slice 3\n");
        goto cleanup;
    }

    ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                          PCPORT_GX_SMOKE_SAMPLE_Y,
                          multiCenter);

    diffPixels = CountFramebufferDiffPixels(baselinePixels, multiPixels);
    if (diffPixels == 0u &&
        baselineCenter[0] == multiCenter[0] &&
        baselineCenter[1] == multiCenter[1] &&
        baselineCenter[2] == multiCenter[2] &&
        baselineCenter[3] == multiCenter[3]) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 3 reached the sibling draw path but did not change the current textured scene (scene=0x%X camera=0x%X baseJoint=0x%X frontJoint=0x%X siblingJoint=0x%X siblingDObj=0x%X siblingMObj=0x%X siblingTObj=0x%X siblingImage=0x%X siblingData=0x%X siblingPObj=0x%X baselineCenter=%u,%u,%u,%u multiCenter=%u,%u,%u,%u format=%u diffPixels=%u submitted=%u expanded=%u prim=0x%X)\n",
                sceneOffset,
                translatedCamera.cameraArchiveOffset,
                baseJointOffset,
                PCPORT_TEXTURED_JOINT_OFFSET,
                PCPORT_SIBLING_JOINT_OFFSET,
                PCPORT_SIBLING_DOBJ_OFFSET,
                PCPORT_SIBLING_MOBJ_OFFSET,
                translatedSiblingTexture.tobjArchiveOffset,
                translatedSiblingTexture.imageArchiveOffset,
                translatedSiblingTexture.imageDataArchiveOffset,
                PCPORT_SIBLING_POBJ_OFFSET,
                baselineCenter[0],
                baselineCenter[1],
                baselineCenter[2],
                baselineCenter[3],
                multiCenter[0],
                multiCenter[1],
                multiCenter[2],
                multiCenter[3],
                translatedSiblingTexture.format,
                diffPixels,
                siblingSubmitted,
                siblingExpanded,
                siblingPrimitive);
        goto cleanup;
    }

    printf("[pcport_bootstrap] Real scene slice 3 smoke passed (scene=0x%X camera=0x%X frontJoint=0x%X frontDObj=0x%X frontMObj=0x%X frontTObj=0x%X frontPObj=0x%X baseJoint=0x%X baseDObj=0x%X baseMObj=0x%X basePObj=0x%X siblingJoint=0x%X siblingDObj=0x%X siblingMObj=0x%X siblingTObj=0x%X siblingImage=0x%X siblingData=0x%X siblingPObj=0x%X diffPixels=%u baselineCenter=%u,%u,%u,%u multiCenter=%u,%u,%u,%u siblingFmt=%u siblingSize=%ux%u submitted=%u expanded=%u prim=0x%X)\n",
           sceneOffset,
           translatedCamera.cameraArchiveOffset,
           PCPORT_TEXTURED_JOINT_OFFSET,
           PCPORT_TEXTURED_DOBJ_OFFSET,
           PCPORT_TEXTURED_MOBJ_OFFSET,
           translatedFrontTexture.tobjArchiveOffset,
           PCPORT_TEXTURED_POBJ_OFFSET,
           baseJointOffset,
           baseDObjOffset,
           baseMObjOffset,
           basePObjOffset,
           PCPORT_SIBLING_JOINT_OFFSET,
           PCPORT_SIBLING_DOBJ_OFFSET,
           PCPORT_SIBLING_MOBJ_OFFSET,
           translatedSiblingTexture.tobjArchiveOffset,
           translatedSiblingTexture.imageArchiveOffset,
           translatedSiblingTexture.imageDataArchiveOffset,
           PCPORT_SIBLING_POBJ_OFFSET,
           diffPixels,
           baselineCenter[0],
           baselineCenter[1],
           baselineCenter[2],
           baselineCenter[3],
           multiCenter[0],
           multiCenter[1],
           multiCenter[2],
           multiCenter[3],
           translatedSiblingTexture.format,
           translatedSiblingTexture.width,
           translatedSiblingTexture.height,
           siblingSubmitted,
           siblingExpanded,
           siblingPrimitive);
    ok = 1;

cleanup:
    GSgfxHostClearPipelineState(PCPORT_REAL_SIBLING_TEXTURED_PIPELINE);
    GSgfxHostClearPipelineState(PCPORT_REAL_TEXTURED_PIPELINE);
    GSgfxHostClearPipelineState(PCPORT_REAL_MATERIAL_PIPELINE);
    GXHostClearTextureBinding();
    GXHostSetVertexAlphaScale(1.0f);
    free(multiPixels);
    free(baselinePixels);
    PCPort_DestroyTranslatedPObj(&translatedSiblingPObj);
    PCPort_DestroyTranslatedPObj(&translatedFrontPObj);
    PCPort_DestroyTranslatedPObj(&translatedBasePObj);
    PCPort_HSDArchiveDestroy(&archive);
    PCPort_FreeBuffer(memberData);
    return ok;
}

static int RunRealTevSceneSliceSmoke(void) {
    PCPortHSDArchive archive;
    PCPortTranslatedCamera translatedCamera;
    PCPortTranslatedPObj translatedBasePObj;
    PCPortTranslatedJointTransform translatedBaseJoint;
    PCPortTranslatedMaterial translatedBaseMaterial;
    PCPortTranslatedPObj translatedFrontPObj;
    PCPortTranslatedJointTransform translatedFrontJoint;
    PCPortTranslatedMaterial translatedFrontMaterial;
    PCPortTranslatedTexture translatedFrontTexture;
    PCPortTranslatedPObj translatedSiblingPObj;
    PCPortTranslatedJointTransform translatedSiblingJoint;
    PCPortTranslatedMaterial translatedSiblingMaterial;
    PCPortTranslatedTexture translatedSiblingTexture;
    PCPortGSDrawObject baseDrawObject;
    PCPortGSDrawObject frontDrawObject;
    PCPortGSDrawObject siblingDrawObject;
    GXTexObj frontTextureObject;
    GXTexObj siblingRawTextureObject;
    GXTexObj siblingTevTextureObject;
    u8* memberData = NULL;
    u8* bakedSiblingPixels = NULL;
    u32 memberSize = 0;
    u32 bakedSiblingSize = 0;
    const u8* sceneData;
    unsigned char* baselinePixels = NULL;
    unsigned char* tevPixels = NULL;
    unsigned char baselineCenter[4] = { 0 };
    unsigned char tevCenter[4] = { 0 };
    u32 sceneOffset = 0;
    u32 sceneBranchOffset;
    u32 cameraDescOffset;
    u32 sceneJointListOffset;
    u32 rootJointOffset;
    u32 baseJointOffset = 0;
    u32 baseDObjOffset = 0;
    u32 basePObjOffset = 0;
    u32 baseMObjOffset = 0;
    u32 frontTObjOffset = 0;
    u32 frontTextureMapId = 0;
    u32 siblingTObjOffset = 0;
    u32 siblingTextureMapId = 0;
    unsigned int diffPixels = 0;
    unsigned int tevSubmitted = 0;
    unsigned int tevExpanded = 0;
    unsigned int tevPrimitive = 0;
    f32 baseModelViewMatrix[3][4];
    f32 frontModelViewMatrix[3][4];
    f32 siblingModelViewMatrix[3][4];
    int ok = 0;

    memset(&archive, 0, sizeof(archive));
    memset(&translatedCamera, 0, sizeof(translatedCamera));
    memset(&translatedBasePObj, 0, sizeof(translatedBasePObj));
    memset(&translatedBaseJoint, 0, sizeof(translatedBaseJoint));
    memset(&translatedBaseMaterial, 0, sizeof(translatedBaseMaterial));
    memset(&translatedFrontPObj, 0, sizeof(translatedFrontPObj));
    memset(&translatedFrontJoint, 0, sizeof(translatedFrontJoint));
    memset(&translatedFrontMaterial, 0, sizeof(translatedFrontMaterial));
    memset(&translatedFrontTexture, 0, sizeof(translatedFrontTexture));
    memset(&translatedSiblingPObj, 0, sizeof(translatedSiblingPObj));
    memset(&translatedSiblingJoint, 0, sizeof(translatedSiblingJoint));
    memset(&translatedSiblingMaterial, 0, sizeof(translatedSiblingMaterial));
    memset(&translatedSiblingTexture, 0, sizeof(translatedSiblingTexture));
    memset(&baseDrawObject, 0, sizeof(baseDrawObject));
    memset(&frontDrawObject, 0, sizeof(frontDrawObject));
    memset(&siblingDrawObject, 0, sizeof(siblingDrawObject));
    memset(&frontTextureObject, 0, sizeof(frontTextureObject));
    memset(&siblingRawTextureObject, 0, sizeof(siblingRawTextureObject));
    memset(&siblingTevTextureObject, 0, sizeof(siblingTevTextureObject));

    if (!PCPort_LoadFsysMember(PCPORT_REAL_CONTENT_ARCHIVE,
                               PCPORT_REAL_CONTENT_MEMBER,
                               &memberData,
                               &memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice load failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    if (!PCPort_HSDArchiveParseBE(&archive, memberData, memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice archive parse failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&archive,
                                                             "scene_data",
                                                             &sceneOffset);
    if (sceneData == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice failed to resolve scene_data\n");
        goto cleanup;
    }

    sceneBranchOffset = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&archive, sceneBranchOffset, 0x10u)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice branch offset was invalid (0x%X)\n",
                sceneBranchOffset);
        goto cleanup;
    }

    cameraDescOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x08);
    if (!PCPort_TranslatePerspectiveCameraFromArchiveBE(&archive,
                                                        cameraDescOffset,
                                                        &translatedCamera)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate real TEV scene camera (scene=0x%X camera=0x%X)\n",
                sceneOffset,
                cameraDescOffset);
        goto cleanup;
    }

    sceneJointListOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x00);
    rootJointOffset = PCPort_ReadBigEndianU32(archive.storage + sceneJointListOffset + 0x00);
    if (!ResolveFirstRenderablePObjDescFromJoint(&archive,
                                                 rootJointOffset,
                                                 &baseJointOffset,
                                                 &baseDObjOffset,
                                                 &basePObjOffset)) {
        fprintf(stderr,
                "[pcport_bootstrap] No baseline renderable PObjDesc was found for real TEV scene slice (rootJoint=0x%X)\n",
                rootJointOffset);
        goto cleanup;
    }

    baseMObjOffset = PCPort_ReadBigEndianU32(archive.storage + baseDObjOffset + 0x08);
    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           basePObjOffset,
                                           &translatedBasePObj) ||
        !PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              baseJointOffset,
                                              &translatedBaseJoint) ||
        !PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               baseMObjOffset,
                                               &translatedBaseMaterial)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate baseline branch for real TEV scene slice (joint=0x%X dobj=0x%X mobj=0x%X pobj=0x%X)\n",
                baseJointOffset,
                baseDObjOffset,
                baseMObjOffset,
                basePObjOffset);
        goto cleanup;
    }

    frontTObjOffset = PCPort_ReadBigEndianU32(archive.storage + PCPORT_TEXTURED_MOBJ_OFFSET + 0x08);
    frontTextureMapId = PCPort_ReadBigEndianU32(archive.storage + frontTObjOffset + 0x08);
    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           PCPORT_TEXTURED_POBJ_OFFSET,
                                           &translatedFrontPObj) ||
        !PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              PCPORT_TEXTURED_JOINT_OFFSET,
                                              &translatedFrontJoint) ||
        !PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               PCPORT_TEXTURED_MOBJ_OFFSET,
                                               &translatedFrontMaterial) ||
        !PCPort_TranslateTextureFromArchiveBE(&archive,
                                              frontTObjOffset,
                                              &translatedFrontTexture)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate front textured branch for real TEV scene slice (joint=0x%X dobj=0x%X mobj=0x%X tobj=0x%X pobj=0x%X)\n",
                PCPORT_TEXTURED_JOINT_OFFSET,
                PCPORT_TEXTURED_DOBJ_OFFSET,
                PCPORT_TEXTURED_MOBJ_OFFSET,
                frontTObjOffset,
                PCPORT_TEXTURED_POBJ_OFFSET);
        goto cleanup;
    }

    siblingTObjOffset = PCPort_ReadBigEndianU32(archive.storage + PCPORT_SIBLING_MOBJ_OFFSET + 0x08);
    siblingTextureMapId = PCPort_ReadBigEndianU32(archive.storage + siblingTObjOffset + 0x08);
    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           PCPORT_SIBLING_POBJ_OFFSET,
                                           &translatedSiblingPObj) ||
        !PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              PCPORT_SIBLING_JOINT_OFFSET,
                                              &translatedSiblingJoint) ||
        !PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               PCPORT_SIBLING_MOBJ_OFFSET,
                                               &translatedSiblingMaterial) ||
        !PCPort_TranslateTextureFromArchiveBE(&archive,
                                              siblingTObjOffset,
                                              &translatedSiblingTexture)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate sibling branch for real TEV scene slice (joint=0x%X dobj=0x%X mobj=0x%X tobj=0x%X pobj=0x%X)\n",
                PCPORT_SIBLING_JOINT_OFFSET,
                PCPORT_SIBLING_DOBJ_OFFSET,
                PCPORT_SIBLING_MOBJ_OFFSET,
                siblingTObjOffset,
                PCPORT_SIBLING_POBJ_OFFSET);
        goto cleanup;
    }

    if (translatedSiblingTexture.tev.kind != PCPORT_TRANSLATED_TEV_I8_COLOR_RAMP) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice reached the visible sibling branch but found no supported narrow TEV payload (siblingTObj=0x%X tev=0x%X format=%u)\n",
                translatedSiblingTexture.tobjArchiveOffset,
                translatedSiblingTexture.tev.archiveOffset,
                translatedSiblingTexture.format);
        goto cleanup;
    }

    if (!PCPort_BakeTextureRGBAFromArchiveBE(&archive,
                                             &translatedSiblingTexture,
                                             &bakedSiblingPixels,
                                             &bakedSiblingSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to bake narrow real TEV texture payload (siblingTObj=0x%X tev=0x%X)\n",
                translatedSiblingTexture.tobjArchiveOffset,
                translatedSiblingTexture.tev.archiveOffset);
        goto cleanup;
    }

    GXInitTexObj(&frontTextureObject,
                 archive.storage + translatedFrontTexture.imageDataArchiveOffset,
                 translatedFrontTexture.width,
                 translatedFrontTexture.height,
                 (GXTexFmt)translatedFrontTexture.format,
                 (GXTexWrapMode)translatedFrontTexture.wrapS,
                 (GXTexWrapMode)translatedFrontTexture.wrapT,
                 translatedFrontTexture.mipmap);

    GXInitTexObj(&siblingRawTextureObject,
                 archive.storage + translatedSiblingTexture.imageDataArchiveOffset,
                 translatedSiblingTexture.width,
                 translatedSiblingTexture.height,
                 (GXTexFmt)translatedSiblingTexture.format,
                 (GXTexWrapMode)translatedSiblingTexture.wrapS,
                 (GXTexWrapMode)translatedSiblingTexture.wrapT,
                 translatedSiblingTexture.mipmap);

    GXHostInitTexObjRGBA8(&siblingTevTextureObject,
                          bakedSiblingPixels,
                          translatedSiblingTexture.width,
                          translatedSiblingTexture.height,
                          (GXTexWrapMode)translatedSiblingTexture.wrapS,
                          (GXTexWrapMode)translatedSiblingTexture.wrapT);

    baseDrawObject.displayList = translatedBasePObj.pobj.display;
    baseDrawObject.displayListSize = translatedBasePObj.pobj.n_display;
    baseDrawObject.pipelineId = PCPORT_REAL_MATERIAL_PIPELINE;
    baseDrawObject.totalVerts = translatedBasePObj.totalSubmittedVertices;
    baseDrawObject.totalPrims = translatedBasePObj.totalPrimitiveCommands;

    frontDrawObject.displayList = translatedFrontPObj.pobj.display;
    frontDrawObject.displayListSize = translatedFrontPObj.pobj.n_display;
    frontDrawObject.pipelineId = PCPORT_REAL_TEXTURED_PIPELINE;
    frontDrawObject.totalVerts = translatedFrontPObj.totalSubmittedVertices;
    frontDrawObject.totalPrims = translatedFrontPObj.totalPrimitiveCommands;

    siblingDrawObject.displayList = translatedSiblingPObj.pobj.display;
    siblingDrawObject.displayListSize = translatedSiblingPObj.pobj.n_display;
    siblingDrawObject.pipelineId = PCPORT_REAL_SIBLING_TEXTURED_PIPELINE;
    siblingDrawObject.totalVerts = translatedSiblingPObj.totalSubmittedVertices;
    siblingDrawObject.totalPrims = translatedSiblingPObj.totalPrimitiveCommands;

    ConfigureTranslatedMaterialPipeline(PCPORT_REAL_MATERIAL_PIPELINE,
                                        &translatedBaseMaterial);
    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_TEXTURED_PIPELINE,
                                        &translatedFrontMaterial,
                                        &translatedFrontTexture,
                                        &frontTextureObject,
                                        (unsigned char)frontTextureMapId);
    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_SIBLING_TEXTURED_PIPELINE,
                                        &translatedSiblingMaterial,
                                        &translatedSiblingTexture,
                                        &siblingRawTextureObject,
                                        (unsigned char)siblingTextureMapId);

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    VIWaitForRetrace_PC();

    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedFrontJoint.modelMatrix,
                    frontModelViewMatrix);
    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedBaseJoint.modelMatrix,
                    baseModelViewMatrix);
    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedSiblingJoint.modelMatrix,
                    siblingModelViewMatrix);

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();
    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);

    GXLoadPosMtxImm(frontModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedFrontPObj.pobj);
    fn_800DAD10((void*)&frontDrawObject);
    glFlush();

    GXLoadPosMtxImm(baseModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedBasePObj.pobj);
    fn_800DAD10((void*)&baseDrawObject);
    glFlush();

    GXLoadPosMtxImm(siblingModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedSiblingPObj.pobj);
    fn_800DAD10((void*)&siblingDrawObject);
    glFlush();

    baselinePixels = ReadBackbufferImage();
    if (baselinePixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture baseline framebuffer for real TEV scene slice\n");
        goto cleanup;
    }

    ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                          PCPORT_GX_SMOKE_SAMPLE_Y,
                          baselineCenter);

    ConfigureTranslatedMaterialPipeline(PCPORT_REAL_MATERIAL_PIPELINE,
                                        &translatedBaseMaterial);
    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_TEXTURED_PIPELINE,
                                        &translatedFrontMaterial,
                                        &translatedFrontTexture,
                                        &frontTextureObject,
                                        (unsigned char)frontTextureMapId);
    ConfigureTranslatedMaterialPipeline(PCPORT_REAL_SIBLING_TEXTURED_PIPELINE,
                                        &translatedSiblingMaterial);
    GSgfxHostSetPipelineTexture(PCPORT_REAL_SIBLING_TEXTURED_PIPELINE,
                                &siblingTevTextureObject,
                                1,
                                translatedSiblingTexture.tevMode,
                                translatedSiblingTexture.hasCoordId
                                    ? translatedSiblingTexture.coordId
                                    : 0u,
                                (unsigned char)siblingTextureMapId);

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();
    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);

    GXLoadPosMtxImm(frontModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedFrontPObj.pobj);
    fn_800DAD10((void*)&frontDrawObject);
    glFlush();

    GXLoadPosMtxImm(baseModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedBasePObj.pobj);
    fn_800DAD10((void*)&baseDrawObject);
    glFlush();

    GXLoadPosMtxImm(siblingModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedSiblingPObj.pobj);
    fn_800DAD10((void*)&siblingDrawObject);
    glFlush();

    tevSubmitted = GXHostGetLastSubmittedVertexCount();
    tevExpanded = GXHostGetLastExpandedVertexCount();
    tevPrimitive = GXHostGetLastSubmittedPrimitive();

    tevPixels = ReadBackbufferImage();
    if (tevPixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture TEV framebuffer for real TEV scene slice\n");
        goto cleanup;
    }

    ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                          PCPORT_GX_SMOKE_SAMPLE_Y,
                          tevCenter);
    diffPixels = CountFramebufferDiffPixels(baselinePixels, tevPixels);
    if (diffPixels == 0u &&
        baselineCenter[0] == tevCenter[0] &&
        baselineCenter[1] == tevCenter[1] &&
        baselineCenter[2] == tevCenter[2] &&
        baselineCenter[3] == tevCenter[3]) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice reached the sibling draw path but did not change the framebuffer (scene=0x%X camera=0x%X siblingJoint=0x%X siblingDObj=0x%X siblingMObj=0x%X siblingTObj=0x%X siblingTev=0x%X baselineCenter=%u,%u,%u,%u tevCenter=%u,%u,%u,%u light=%u,%u,%u,%u dark=%u,%u,%u,%u diffPixels=%u submitted=%u expanded=%u prim=0x%X)\n",
                sceneOffset,
                translatedCamera.cameraArchiveOffset,
                PCPORT_SIBLING_JOINT_OFFSET,
                PCPORT_SIBLING_DOBJ_OFFSET,
                PCPORT_SIBLING_MOBJ_OFFSET,
                translatedSiblingTexture.tobjArchiveOffset,
                translatedSiblingTexture.tev.archiveOffset,
                baselineCenter[0],
                baselineCenter[1],
                baselineCenter[2],
                baselineCenter[3],
                tevCenter[0],
                tevCenter[1],
                tevCenter[2],
                tevCenter[3],
                translatedSiblingTexture.tev.rampLight[0],
                translatedSiblingTexture.tev.rampLight[1],
                translatedSiblingTexture.tev.rampLight[2],
                translatedSiblingTexture.tev.rampLight[3],
                translatedSiblingTexture.tev.rampDark[0],
                translatedSiblingTexture.tev.rampDark[1],
                translatedSiblingTexture.tev.rampDark[2],
                translatedSiblingTexture.tev.rampDark[3],
                diffPixels,
                tevSubmitted,
                tevExpanded,
                tevPrimitive);
        goto cleanup;
    }

    printf("[pcport_bootstrap] Real TEV scene slice smoke passed (scene=0x%X camera=0x%X siblingJoint=0x%X siblingDObj=0x%X siblingMObj=0x%X siblingTObj=0x%X siblingTev=0x%X siblingImage=0x%X siblingData=0x%X diffPixels=%u baselineCenter=%u,%u,%u,%u tevCenter=%u,%u,%u,%u light=%u,%u,%u,%u dark=%u,%u,%u,%u bakedBytes=%u submitted=%u expanded=%u prim=0x%X)\n",
           sceneOffset,
           translatedCamera.cameraArchiveOffset,
           PCPORT_SIBLING_JOINT_OFFSET,
           PCPORT_SIBLING_DOBJ_OFFSET,
           PCPORT_SIBLING_MOBJ_OFFSET,
           translatedSiblingTexture.tobjArchiveOffset,
           translatedSiblingTexture.tev.archiveOffset,
           translatedSiblingTexture.imageArchiveOffset,
           translatedSiblingTexture.imageDataArchiveOffset,
           diffPixels,
           baselineCenter[0],
           baselineCenter[1],
           baselineCenter[2],
           baselineCenter[3],
           tevCenter[0],
           tevCenter[1],
           tevCenter[2],
           tevCenter[3],
           translatedSiblingTexture.tev.rampLight[0],
           translatedSiblingTexture.tev.rampLight[1],
           translatedSiblingTexture.tev.rampLight[2],
           translatedSiblingTexture.tev.rampLight[3],
           translatedSiblingTexture.tev.rampDark[0],
           translatedSiblingTexture.tev.rampDark[1],
           translatedSiblingTexture.tev.rampDark[2],
           translatedSiblingTexture.tev.rampDark[3],
           bakedSiblingSize,
           tevSubmitted,
           tevExpanded,
           tevPrimitive);
    ok = 1;

cleanup:
    GSgfxHostClearPipelineState(PCPORT_REAL_SIBLING_TEXTURED_PIPELINE);
    GSgfxHostClearPipelineState(PCPORT_REAL_TEXTURED_PIPELINE);
    GSgfxHostClearPipelineState(PCPORT_REAL_MATERIAL_PIPELINE);
    GXHostClearTextureBinding();
    GXHostSetVertexAlphaScale(1.0f);
    free(tevPixels);
    free(baselinePixels);
    PCPort_FreeBuffer(bakedSiblingPixels);
    PCPort_DestroyTranslatedPObj(&translatedSiblingPObj);
    PCPort_DestroyTranslatedPObj(&translatedFrontPObj);
    PCPort_DestroyTranslatedPObj(&translatedBasePObj);
    PCPort_HSDArchiveDestroy(&archive);
    PCPort_FreeBuffer(memberData);
    return ok;
}

static int RunRealTevSceneSlice2Smoke(void) {
    PCPortHSDArchive archive;
    PCPortTranslatedCamera translatedCamera;
    PCPortTranslatedPObj translatedBasePObj;
    PCPortTranslatedJointTransform translatedBaseJoint;
    PCPortTranslatedMaterial translatedBaseMaterial;
    PCPortTranslatedTexture translatedBaseTexture;
    PCPortTranslatedPObj translatedFrontPObj;
    PCPortTranslatedJointTransform translatedFrontJoint;
    PCPortTranslatedMaterial translatedFrontMaterial;
    PCPortTranslatedTexture translatedFrontTexture;
    PCPortTranslatedPObj translatedSiblingPObj;
    PCPortTranslatedJointTransform translatedSiblingJoint;
    PCPortTranslatedMaterial translatedSiblingMaterial;
    PCPortTranslatedTexture translatedSiblingTexture;
    PCPortGSDrawObject baseDrawObject;
    PCPortGSDrawObject frontDrawObject;
    PCPortGSDrawObject siblingDrawObject;
    GXTexObj frontTextureObject;
    GXTexObj baseTevTextureObject;
    GXTexObj siblingTevTextureObject;
    u8* memberData = NULL;
    u8* bakedBasePixels = NULL;
    u8* bakedSiblingPixels = NULL;
    u32 memberSize = 0;
    u32 bakedBaseSize = 0;
    u32 bakedSiblingSize = 0;
    const u8* sceneData;
    unsigned char* baselinePixels = NULL;
    unsigned char* broaderPixels = NULL;
    unsigned char baselineCenter[4] = { 0 };
    unsigned char broaderCenter[4] = { 0 };
    u32 sceneOffset = 0;
    u32 sceneBranchOffset;
    u32 cameraDescOffset;
    u32 sceneJointListOffset;
    u32 rootJointOffset;
    u32 baseJointOffset = 0;
    u32 baseDObjOffset = 0;
    u32 basePObjOffset = 0;
    u32 baseMObjOffset = 0;
    u32 baseTObjOffset = 0;
    u32 baseTextureMapId = 0;
    u32 frontTObjOffset = 0;
    u32 frontTextureMapId = 0;
    u32 siblingTObjOffset = 0;
    u32 siblingTextureMapId = 0;
    unsigned int diffPixels = 0;
    unsigned int broaderSubmitted = 0;
    unsigned int broaderExpanded = 0;
    unsigned int broaderPrimitive = 0;
    f32 baseModelViewMatrix[3][4];
    f32 frontModelViewMatrix[3][4];
    f32 siblingModelViewMatrix[3][4];
    int ok = 0;

    memset(&archive, 0, sizeof(archive));
    memset(&translatedCamera, 0, sizeof(translatedCamera));
    memset(&translatedBasePObj, 0, sizeof(translatedBasePObj));
    memset(&translatedBaseJoint, 0, sizeof(translatedBaseJoint));
    memset(&translatedBaseMaterial, 0, sizeof(translatedBaseMaterial));
    memset(&translatedBaseTexture, 0, sizeof(translatedBaseTexture));
    memset(&translatedFrontPObj, 0, sizeof(translatedFrontPObj));
    memset(&translatedFrontJoint, 0, sizeof(translatedFrontJoint));
    memset(&translatedFrontMaterial, 0, sizeof(translatedFrontMaterial));
    memset(&translatedFrontTexture, 0, sizeof(translatedFrontTexture));
    memset(&translatedSiblingPObj, 0, sizeof(translatedSiblingPObj));
    memset(&translatedSiblingJoint, 0, sizeof(translatedSiblingJoint));
    memset(&translatedSiblingMaterial, 0, sizeof(translatedSiblingMaterial));
    memset(&translatedSiblingTexture, 0, sizeof(translatedSiblingTexture));
    memset(&baseDrawObject, 0, sizeof(baseDrawObject));
    memset(&frontDrawObject, 0, sizeof(frontDrawObject));
    memset(&siblingDrawObject, 0, sizeof(siblingDrawObject));
    memset(&frontTextureObject, 0, sizeof(frontTextureObject));
    memset(&baseTevTextureObject, 0, sizeof(baseTevTextureObject));
    memset(&siblingTevTextureObject, 0, sizeof(siblingTevTextureObject));

    if (!PCPort_LoadFsysMember(PCPORT_REAL_CONTENT_ARCHIVE,
                               PCPORT_REAL_CONTENT_MEMBER,
                               &memberData,
                               &memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice 2 load failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    if (!PCPort_HSDArchiveParseBE(&archive, memberData, memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice 2 archive parse failed (%s:%s)\n",
                PCPORT_REAL_CONTENT_ARCHIVE,
                PCPORT_REAL_CONTENT_MEMBER);
        goto cleanup;
    }

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&archive,
                                                             "scene_data",
                                                             &sceneOffset);
    if (sceneData == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice 2 failed to resolve scene_data\n");
        goto cleanup;
    }

    sceneBranchOffset = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&archive, sceneBranchOffset, 0x10u)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice 2 branch offset was invalid (0x%X)\n",
                sceneBranchOffset);
        goto cleanup;
    }

    cameraDescOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x08);
    if (!PCPort_TranslatePerspectiveCameraFromArchiveBE(&archive,
                                                        cameraDescOffset,
                                                        &translatedCamera)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate real TEV scene 2 camera (scene=0x%X camera=0x%X)\n",
                sceneOffset,
                cameraDescOffset);
        goto cleanup;
    }

    sceneJointListOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x00);
    rootJointOffset = PCPort_ReadBigEndianU32(archive.storage + sceneJointListOffset + 0x00);
    if (!ResolveFirstRenderablePObjDescFromJoint(&archive,
                                                 rootJointOffset,
                                                 &baseJointOffset,
                                                 &baseDObjOffset,
                                                 &basePObjOffset)) {
        fprintf(stderr,
                "[pcport_bootstrap] No baseline renderable PObjDesc was found for real TEV scene slice 2 (rootJoint=0x%X)\n",
                rootJointOffset);
        goto cleanup;
    }

    baseMObjOffset = PCPort_ReadBigEndianU32(archive.storage + baseDObjOffset + 0x08);
    baseTObjOffset = PCPort_ReadBigEndianU32(archive.storage + baseMObjOffset + 0x08);
    baseTextureMapId = PCPort_ReadBigEndianU32(archive.storage + baseTObjOffset + 0x08);
    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           basePObjOffset,
                                           &translatedBasePObj) ||
        !PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              baseJointOffset,
                                              &translatedBaseJoint) ||
        !PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               baseMObjOffset,
                                               &translatedBaseMaterial) ||
        !PCPort_TranslateTextureFromArchiveBE(&archive,
                                              baseTObjOffset,
                                              &translatedBaseTexture)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate base branch for real TEV scene slice 2 (joint=0x%X dobj=0x%X mobj=0x%X tobj=0x%X pobj=0x%X)\n",
                baseJointOffset,
                baseDObjOffset,
                baseMObjOffset,
                baseTObjOffset,
                basePObjOffset);
        goto cleanup;
    }

    frontTObjOffset = PCPort_ReadBigEndianU32(archive.storage + PCPORT_TEXTURED_MOBJ_OFFSET + 0x08);
    frontTextureMapId = PCPort_ReadBigEndianU32(archive.storage + frontTObjOffset + 0x08);
    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           PCPORT_TEXTURED_POBJ_OFFSET,
                                           &translatedFrontPObj) ||
        !PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              PCPORT_TEXTURED_JOINT_OFFSET,
                                              &translatedFrontJoint) ||
        !PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               PCPORT_TEXTURED_MOBJ_OFFSET,
                                               &translatedFrontMaterial) ||
        !PCPort_TranslateTextureFromArchiveBE(&archive,
                                              frontTObjOffset,
                                              &translatedFrontTexture)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate front textured branch for real TEV scene slice 2 (joint=0x%X dobj=0x%X mobj=0x%X tobj=0x%X pobj=0x%X)\n",
                PCPORT_TEXTURED_JOINT_OFFSET,
                PCPORT_TEXTURED_DOBJ_OFFSET,
                PCPORT_TEXTURED_MOBJ_OFFSET,
                frontTObjOffset,
                PCPORT_TEXTURED_POBJ_OFFSET);
        goto cleanup;
    }

    siblingTObjOffset = PCPort_ReadBigEndianU32(archive.storage + PCPORT_SIBLING_MOBJ_OFFSET + 0x08);
    siblingTextureMapId = PCPort_ReadBigEndianU32(archive.storage + siblingTObjOffset + 0x08);
    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           PCPORT_SIBLING_POBJ_OFFSET,
                                           &translatedSiblingPObj) ||
        !PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              PCPORT_SIBLING_JOINT_OFFSET,
                                              &translatedSiblingJoint) ||
        !PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               PCPORT_SIBLING_MOBJ_OFFSET,
                                               &translatedSiblingMaterial) ||
        !PCPort_TranslateTextureFromArchiveBE(&archive,
                                              siblingTObjOffset,
                                              &translatedSiblingTexture)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate sibling branch for real TEV scene slice 2 (joint=0x%X dobj=0x%X mobj=0x%X tobj=0x%X pobj=0x%X)\n",
                PCPORT_SIBLING_JOINT_OFFSET,
                PCPORT_SIBLING_DOBJ_OFFSET,
                PCPORT_SIBLING_MOBJ_OFFSET,
                siblingTObjOffset,
                PCPORT_SIBLING_POBJ_OFFSET);
        goto cleanup;
    }

    if (translatedBaseTexture.tev.kind != PCPORT_TRANSLATED_TEV_I8_COLOR_RAMP ||
        translatedSiblingTexture.tev.kind != PCPORT_TRANSLATED_TEV_I8_COLOR_RAMP) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice 2 found no shared translated TEV payload boundary (baseTev=0x%X baseKind=%u siblingTev=0x%X siblingKind=%u)\n",
                translatedBaseTexture.tev.archiveOffset,
                translatedBaseTexture.tev.kind,
                translatedSiblingTexture.tev.archiveOffset,
                translatedSiblingTexture.tev.kind);
        goto cleanup;
    }

    if (!PCPort_BakeTextureRGBAFromArchiveBE(&archive,
                                             &translatedBaseTexture,
                                             &bakedBasePixels,
                                             &bakedBaseSize) ||
        !PCPort_BakeTextureRGBAFromArchiveBE(&archive,
                                             &translatedSiblingTexture,
                                             &bakedSiblingPixels,
                                             &bakedSiblingSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to bake shared translated TEV payloads (baseTev=0x%X siblingTev=0x%X)\n",
                translatedBaseTexture.tev.archiveOffset,
                translatedSiblingTexture.tev.archiveOffset);
        goto cleanup;
    }

    GXInitTexObj(&frontTextureObject,
                 archive.storage + translatedFrontTexture.imageDataArchiveOffset,
                 translatedFrontTexture.width,
                 translatedFrontTexture.height,
                 (GXTexFmt)translatedFrontTexture.format,
                 (GXTexWrapMode)translatedFrontTexture.wrapS,
                 (GXTexWrapMode)translatedFrontTexture.wrapT,
                 translatedFrontTexture.mipmap);

    GXHostInitTexObjRGBA8(&baseTevTextureObject,
                          bakedBasePixels,
                          translatedBaseTexture.width,
                          translatedBaseTexture.height,
                          (GXTexWrapMode)translatedBaseTexture.wrapS,
                          (GXTexWrapMode)translatedBaseTexture.wrapT);

    GXHostInitTexObjRGBA8(&siblingTevTextureObject,
                          bakedSiblingPixels,
                          translatedSiblingTexture.width,
                          translatedSiblingTexture.height,
                          (GXTexWrapMode)translatedSiblingTexture.wrapS,
                          (GXTexWrapMode)translatedSiblingTexture.wrapT);

    baseDrawObject.displayList = translatedBasePObj.pobj.display;
    baseDrawObject.displayListSize = translatedBasePObj.pobj.n_display;
    baseDrawObject.pipelineId = PCPORT_REAL_MATERIAL_PIPELINE;
    baseDrawObject.totalVerts = translatedBasePObj.totalSubmittedVertices;
    baseDrawObject.totalPrims = translatedBasePObj.totalPrimitiveCommands;

    frontDrawObject.displayList = translatedFrontPObj.pobj.display;
    frontDrawObject.displayListSize = translatedFrontPObj.pobj.n_display;
    frontDrawObject.pipelineId = PCPORT_REAL_TEXTURED_PIPELINE;
    frontDrawObject.totalVerts = translatedFrontPObj.totalSubmittedVertices;
    frontDrawObject.totalPrims = translatedFrontPObj.totalPrimitiveCommands;

    siblingDrawObject.displayList = translatedSiblingPObj.pobj.display;
    siblingDrawObject.displayListSize = translatedSiblingPObj.pobj.n_display;
    siblingDrawObject.pipelineId = PCPORT_REAL_SIBLING_TEXTURED_PIPELINE;
    siblingDrawObject.totalVerts = translatedSiblingPObj.totalSubmittedVertices;
    siblingDrawObject.totalPrims = translatedSiblingPObj.totalPrimitiveCommands;

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    VIWaitForRetrace_PC();

    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedFrontJoint.modelMatrix,
                    frontModelViewMatrix);
    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedBaseJoint.modelMatrix,
                    baseModelViewMatrix);
    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedSiblingJoint.modelMatrix,
                    siblingModelViewMatrix);

    ConfigureTranslatedMaterialPipeline(PCPORT_REAL_MATERIAL_PIPELINE,
                                        &translatedBaseMaterial);
    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_TEXTURED_PIPELINE,
                                        &translatedFrontMaterial,
                                        &translatedFrontTexture,
                                        &frontTextureObject,
                                        (unsigned char)frontTextureMapId);
    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_SIBLING_TEXTURED_PIPELINE,
                                        &translatedSiblingMaterial,
                                        &translatedSiblingTexture,
                                        &siblingTevTextureObject,
                                        (unsigned char)siblingTextureMapId);

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();
    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);

    GXLoadPosMtxImm(frontModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedFrontPObj.pobj);
    fn_800DAD10((void*)&frontDrawObject);
    glFlush();

    GXLoadPosMtxImm(baseModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedBasePObj.pobj);
    fn_800DAD10((void*)&baseDrawObject);
    glFlush();

    GXLoadPosMtxImm(siblingModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedSiblingPObj.pobj);
    fn_800DAD10((void*)&siblingDrawObject);
    glFlush();

    baselinePixels = ReadBackbufferImage();
    if (baselinePixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture baseline framebuffer for real TEV scene slice 2\n");
        goto cleanup;
    }

    ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                          PCPORT_GX_SMOKE_SAMPLE_Y,
                          baselineCenter);

    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_MATERIAL_PIPELINE,
                                        &translatedBaseMaterial,
                                        &translatedBaseTexture,
                                        &baseTevTextureObject,
                                        (unsigned char)baseTextureMapId);
    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_TEXTURED_PIPELINE,
                                        &translatedFrontMaterial,
                                        &translatedFrontTexture,
                                        &frontTextureObject,
                                        (unsigned char)frontTextureMapId);
    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_SIBLING_TEXTURED_PIPELINE,
                                        &translatedSiblingMaterial,
                                        &translatedSiblingTexture,
                                        &siblingTevTextureObject,
                                        (unsigned char)siblingTextureMapId);

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();
    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);

    GXLoadPosMtxImm(frontModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedFrontPObj.pobj);
    fn_800DAD10((void*)&frontDrawObject);
    glFlush();

    GXLoadPosMtxImm(baseModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedBasePObj.pobj);
    fn_800DAD10((void*)&baseDrawObject);
    glFlush();

    GXLoadPosMtxImm(siblingModelViewMatrix, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedSiblingPObj.pobj);
    fn_800DAD10((void*)&siblingDrawObject);
    glFlush();

    broaderSubmitted = GXHostGetLastSubmittedVertexCount();
    broaderExpanded = GXHostGetLastExpandedVertexCount();
    broaderPrimitive = GXHostGetLastSubmittedPrimitive();

    broaderPixels = ReadBackbufferImage();
    if (broaderPixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture broader TEV framebuffer for real TEV scene slice 2\n");
        goto cleanup;
    }

    ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                          PCPORT_GX_SMOKE_SAMPLE_Y,
                          broaderCenter);
    diffPixels = CountFramebufferDiffPixels(baselinePixels, broaderPixels);
    if (diffPixels == 0u &&
        baselineCenter[0] == broaderCenter[0] &&
        baselineCenter[1] == broaderCenter[1] &&
        baselineCenter[2] == broaderCenter[2] &&
        baselineCenter[3] == broaderCenter[3]) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice 2 reached the shared base+sibling TEV draw path but did not change the framebuffer (scene=0x%X camera=0x%X baseTObj=0x%X baseTev=0x%X siblingTObj=0x%X siblingTev=0x%X baselineCenter=%u,%u,%u,%u broaderCenter=%u,%u,%u,%u baseLight=%u,%u,%u,%u baseDark=%u,%u,%u,%u diffPixels=%u submitted=%u expanded=%u prim=0x%X)\n",
                sceneOffset,
                translatedCamera.cameraArchiveOffset,
                translatedBaseTexture.tobjArchiveOffset,
                translatedBaseTexture.tev.archiveOffset,
                translatedSiblingTexture.tobjArchiveOffset,
                translatedSiblingTexture.tev.archiveOffset,
                baselineCenter[0],
                baselineCenter[1],
                baselineCenter[2],
                baselineCenter[3],
                broaderCenter[0],
                broaderCenter[1],
                broaderCenter[2],
                broaderCenter[3],
                translatedBaseTexture.tev.rampLight[0],
                translatedBaseTexture.tev.rampLight[1],
                translatedBaseTexture.tev.rampLight[2],
                translatedBaseTexture.tev.rampLight[3],
                translatedBaseTexture.tev.rampDark[0],
                translatedBaseTexture.tev.rampDark[1],
                translatedBaseTexture.tev.rampDark[2],
                translatedBaseTexture.tev.rampDark[3],
                diffPixels,
                broaderSubmitted,
                broaderExpanded,
                broaderPrimitive);
        goto cleanup;
    }

    printf("[pcport_bootstrap] Real TEV scene slice 2 smoke passed (scene=0x%X camera=0x%X baseJoint=0x%X baseDObj=0x%X baseMObj=0x%X baseTObj=0x%X baseTev=0x%X siblingJoint=0x%X siblingDObj=0x%X siblingMObj=0x%X siblingTObj=0x%X siblingTev=0x%X diffPixels=%u baselineCenter=%u,%u,%u,%u broaderCenter=%u,%u,%u,%u baseLight=%u,%u,%u,%u baseDark=%u,%u,%u,%u baseBakedBytes=%u siblingBakedBytes=%u submitted=%u expanded=%u prim=0x%X)\n",
           sceneOffset,
           translatedCamera.cameraArchiveOffset,
           baseJointOffset,
           baseDObjOffset,
           baseMObjOffset,
           translatedBaseTexture.tobjArchiveOffset,
           translatedBaseTexture.tev.archiveOffset,
           PCPORT_SIBLING_JOINT_OFFSET,
           PCPORT_SIBLING_DOBJ_OFFSET,
           PCPORT_SIBLING_MOBJ_OFFSET,
           translatedSiblingTexture.tobjArchiveOffset,
           translatedSiblingTexture.tev.archiveOffset,
           diffPixels,
           baselineCenter[0],
           baselineCenter[1],
           baselineCenter[2],
           baselineCenter[3],
           broaderCenter[0],
           broaderCenter[1],
           broaderCenter[2],
           broaderCenter[3],
           translatedBaseTexture.tev.rampLight[0],
           translatedBaseTexture.tev.rampLight[1],
           translatedBaseTexture.tev.rampLight[2],
           translatedBaseTexture.tev.rampLight[3],
           translatedBaseTexture.tev.rampDark[0],
           translatedBaseTexture.tev.rampDark[1],
           translatedBaseTexture.tev.rampDark[2],
           translatedBaseTexture.tev.rampDark[3],
           bakedBaseSize,
           bakedSiblingSize,
           broaderSubmitted,
           broaderExpanded,
           broaderPrimitive);
    ok = 1;

cleanup:
    GSgfxHostClearPipelineState(PCPORT_REAL_SIBLING_TEXTURED_PIPELINE);
    GSgfxHostClearPipelineState(PCPORT_REAL_TEXTURED_PIPELINE);
    GSgfxHostClearPipelineState(PCPORT_REAL_MATERIAL_PIPELINE);
    GXHostClearTextureBinding();
    GXHostSetVertexAlphaScale(1.0f);
    free(broaderPixels);
    free(baselinePixels);
    PCPort_FreeBuffer(bakedSiblingPixels);
    PCPort_FreeBuffer(bakedBasePixels);
    PCPort_DestroyTranslatedPObj(&translatedSiblingPObj);
    PCPort_DestroyTranslatedPObj(&translatedFrontPObj);
    PCPort_DestroyTranslatedPObj(&translatedBasePObj);
    PCPort_HSDArchiveDestroy(&archive);
    PCPort_FreeBuffer(memberData);
    return ok;
}

static int RunRealTevSceneSlice3Smoke(void) {
    PCPortHSDArchive archive;
    PCPortTranslatedCamera translatedCamera;
    PCPortTranslatedPObj translatedObject1PObj;
    PCPortTranslatedJointTransform translatedObject1Joint;
    PCPortTranslatedMaterial translatedObject1Material;
    PCPortTranslatedTextureExp translatedObject1TextureExp;
    PCPortGSDrawObject drawObject1;
    GXTexObj rawTextureObject1;
    GXTexObj tevTextureObject1;
    u8* memberData = NULL;
    u8* bakedPixels1 = NULL;
    u32 memberSize = 0;
    u32 bakedSize1 = 0;
    const u8* sceneData;
    unsigned char* baselinePixels = NULL;
    unsigned char* tevPixels = NULL;
    unsigned char baselineCenter[4] = { 0 };
    unsigned char tevCenter[4] = { 0 };
    u32 sceneOffset = 0;
    u32 sceneBranchOffset;
    u32 cameraDescOffset;
    u32 sceneJointListOffset;
    u32 rootJointOffset;
    u32 object1TObjOffset = 0;
    u32 object1TextureMapId = 0;
    unsigned int diffPixels = 0;
    unsigned int tevSubmitted = 0;
    unsigned int tevExpanded = 0;
    unsigned int tevPrimitive = 0;
    f32 modelViewMatrix1[3][4];
    int ok = 0;

    memset(&archive, 0, sizeof(archive));
    memset(&translatedCamera, 0, sizeof(translatedCamera));
    memset(&translatedObject1PObj, 0, sizeof(translatedObject1PObj));
    memset(&translatedObject1Joint, 0, sizeof(translatedObject1Joint));
    memset(&translatedObject1Material, 0, sizeof(translatedObject1Material));
    memset(&translatedObject1TextureExp, 0, sizeof(translatedObject1TextureExp));
    memset(&drawObject1, 0, sizeof(drawObject1));
    memset(&rawTextureObject1, 0, sizeof(rawTextureObject1));
    memset(&tevTextureObject1, 0, sizeof(tevTextureObject1));

    if (!PCPort_LoadFsysMember(PCPORT_PDA_MENU_ARCHIVE,
                               PCPORT_PDA2_BG_MEMBER,
                               &memberData,
                               &memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice 3 load failed (%s:%s)\n",
                PCPORT_PDA_MENU_ARCHIVE,
                PCPORT_PDA2_BG_MEMBER);
        goto cleanup;
    }

    if (!PCPort_HSDArchiveParseBE(&archive, memberData, memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice 3 archive parse failed (%s:%s)\n",
                PCPORT_PDA_MENU_ARCHIVE,
                PCPORT_PDA2_BG_MEMBER);
        goto cleanup;
    }

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&archive,
                                                             "scene_data",
                                                             &sceneOffset);
    if (sceneData == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice 3 failed to resolve scene_data\n");
        goto cleanup;
    }

    sceneBranchOffset = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&archive, sceneBranchOffset, 0x10u)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice 3 branch offset was invalid (0x%X)\n",
                sceneBranchOffset);
        goto cleanup;
    }

    cameraDescOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x08);
    if (!PCPort_TranslatePerspectiveCameraFromArchiveBE(&archive,
                                                        cameraDescOffset,
                                                        &translatedCamera)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate real TEV scene 3 camera (scene=0x%X camera=0x%X)\n",
                sceneOffset,
                cameraDescOffset);
        goto cleanup;
    }

    sceneJointListOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x00);
    rootJointOffset = PCPort_ReadBigEndianU32(archive.storage + sceneJointListOffset + 0x00);

    object1TObjOffset = PCPort_ReadBigEndianU32(archive.storage + PCPORT_PDA2_BG_OBJECT1_MOBJ_OFFSET + 0x08);
    object1TextureMapId = PCPort_ReadBigEndianU32(archive.storage + object1TObjOffset + 0x08);
    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           PCPORT_PDA2_BG_OBJECT1_POBJ_OFFSET,
                                           &translatedObject1PObj) ||
        !PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              PCPORT_PDA2_BG_OBJECT1_JOINT_OFFSET,
                                              &translatedObject1Joint) ||
        !PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               PCPORT_PDA2_BG_OBJECT1_MOBJ_OFFSET,
                                               &translatedObject1Material) ||
        !PCPort_TranslateTextureExpFromArchiveBE(&archive,
                                                 object1TObjOffset,
                                                 &translatedObject1TextureExp)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate pda2_bg object 1 (joint=0x%X dobj=0x%X mobj=0x%X tobj=0x%X pobj=0x%X)\n",
                PCPORT_PDA2_BG_OBJECT1_JOINT_OFFSET,
                PCPORT_PDA2_BG_OBJECT1_DOBJ_OFFSET,
                PCPORT_PDA2_BG_OBJECT1_MOBJ_OFFSET,
                object1TObjOffset,
                PCPORT_PDA2_BG_OBJECT1_POBJ_OFFSET);
        goto cleanup;
    }

    if (translatedObject1TextureExp.kind != PCPORT_TEXTURE_EXP_KIND_I8_RAMP ||
        translatedObject1TextureExp.stageCount != 1u ||
        translatedObject1TextureExp.stages[0].kind != PCPORT_TEXP_STAGE_I8_RAMP_SAMPLE) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice 3 did not reach the expected shared TExp boundary (obj1TObj=0x%X kind=%u stage0=%u stages=%u obj1Tev=0x%X)\n",
                translatedObject1TextureExp.stages[0].texture.tobjArchiveOffset,
                translatedObject1TextureExp.kind,
                translatedObject1TextureExp.stages[0].kind,
                translatedObject1TextureExp.stageCount,
                translatedObject1TextureExp.stages[0].texture.tev.archiveOffset);
        goto cleanup;
    }

    if (!PCPort_BakeTextureExpRGBAFromArchiveBE(&archive,
                                                &translatedObject1TextureExp,
                                                &bakedPixels1,
                                                &bakedSize1)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to bake pda2_bg translated TEV payload (obj1Tev=0x%X)\n",
                translatedObject1TextureExp.stages[0].texture.tev.archiveOffset);
        goto cleanup;
    }

    GXInitTexObj(&rawTextureObject1,
                 archive.storage + translatedObject1TextureExp.stages[0].texture.imageDataArchiveOffset,
                 translatedObject1TextureExp.stages[0].texture.width,
                 translatedObject1TextureExp.stages[0].texture.height,
                 (GXTexFmt)translatedObject1TextureExp.stages[0].texture.format,
                 (GXTexWrapMode)translatedObject1TextureExp.stages[0].texture.wrapS,
                 (GXTexWrapMode)translatedObject1TextureExp.stages[0].texture.wrapT,
                 translatedObject1TextureExp.stages[0].texture.mipmap);

    GXHostInitTexObjRGBA8(&tevTextureObject1,
                          bakedPixels1,
                          translatedObject1TextureExp.stages[0].texture.width,
                          translatedObject1TextureExp.stages[0].texture.height,
                          (GXTexWrapMode)translatedObject1TextureExp.stages[0].texture.wrapS,
                          (GXTexWrapMode)translatedObject1TextureExp.stages[0].texture.wrapT);

    drawObject1.displayList = translatedObject1PObj.pobj.display;
    drawObject1.displayListSize = translatedObject1PObj.pobj.n_display;
    drawObject1.pipelineId = PCPORT_REAL_TEXTURED_PIPELINE;
    drawObject1.totalVerts = translatedObject1PObj.totalSubmittedVertices;
    drawObject1.totalPrims = translatedObject1PObj.totalPrimitiveCommands;

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    VIWaitForRetrace_PC();

    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedObject1Joint.modelMatrix,
                    modelViewMatrix1);

    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_TEXTURED_PIPELINE,
                                        &translatedObject1Material,
                                        &translatedObject1TextureExp.stages[0].texture,
                                        &rawTextureObject1,
                                        (unsigned char)object1TextureMapId);

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();
    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);

    GXLoadPosMtxImm(modelViewMatrix1, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedObject1PObj.pobj);
    fn_800DAD10((void*)&drawObject1);
    glFlush();

    baselinePixels = ReadBackbufferImage();
    if (baselinePixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture baseline framebuffer for real TEV scene slice 3\n");
        goto cleanup;
    }

    ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                          PCPORT_GX_SMOKE_SAMPLE_Y,
                          baselineCenter);

    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_TEXTURED_PIPELINE,
                                        &translatedObject1Material,
                                        &translatedObject1TextureExp.stages[0].texture,
                                        &tevTextureObject1,
                                        (unsigned char)object1TextureMapId);

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();
    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);

    GXLoadPosMtxImm(modelViewMatrix1, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedObject1PObj.pobj);
    fn_800DAD10((void*)&drawObject1);
    glFlush();

    tevSubmitted = GXHostGetLastSubmittedVertexCount();
    tevExpanded = GXHostGetLastExpandedVertexCount();
    tevPrimitive = GXHostGetLastSubmittedPrimitive();

    tevPixels = ReadBackbufferImage();
    if (tevPixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture TEV framebuffer for real TEV scene slice 3\n");
        goto cleanup;
    }

    ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                          PCPORT_GX_SMOKE_SAMPLE_Y,
                          tevCenter);
    diffPixels = CountFramebufferDiffPixels(baselinePixels, tevPixels);
    if (diffPixels == 0u &&
        baselineCenter[0] == tevCenter[0] &&
        baselineCenter[1] == tevCenter[1] &&
        baselineCenter[2] == tevCenter[2] &&
        baselineCenter[3] == tevCenter[3]) {
        fprintf(stderr,
                "[pcport_bootstrap] Real TEV scene slice 3 reached the distinct pda2_bg TEV draw path but did not change the framebuffer (scene=0x%X camera=0x%X obj1Tev=0x%X baselineCenter=%u,%u,%u,%u tevCenter=%u,%u,%u,%u obj1Light=%u,%u,%u,%u obj1Dark=%u,%u,%u,%u diffPixels=%u submitted=%u expanded=%u prim=0x%X)\n",
                sceneOffset,
                translatedCamera.cameraArchiveOffset,
                translatedObject1TextureExp.stages[0].texture.tev.archiveOffset,
                baselineCenter[0],
                baselineCenter[1],
                baselineCenter[2],
                baselineCenter[3],
                tevCenter[0],
                tevCenter[1],
                tevCenter[2],
                tevCenter[3],
                translatedObject1TextureExp.stages[0].texture.tev.rampLight[0],
                translatedObject1TextureExp.stages[0].texture.tev.rampLight[1],
                translatedObject1TextureExp.stages[0].texture.tev.rampLight[2],
                translatedObject1TextureExp.stages[0].texture.tev.rampLight[3],
                translatedObject1TextureExp.stages[0].texture.tev.rampDark[0],
                translatedObject1TextureExp.stages[0].texture.tev.rampDark[1],
                translatedObject1TextureExp.stages[0].texture.tev.rampDark[2],
                translatedObject1TextureExp.stages[0].texture.tev.rampDark[3],
                diffPixels,
                tevSubmitted,
                tevExpanded,
                tevPrimitive);
        goto cleanup;
    }

    printf("[pcport_bootstrap] Real TEV scene slice 3 smoke passed (scene=0x%X camera=0x%X obj1Joint=0x%X obj1DObj=0x%X obj1MObj=0x%X obj1TObj=0x%X kind=%u stage0=%u stages=%u obj1Tev=0x%X diffPixels=%u baselineCenter=%u,%u,%u,%u tevCenter=%u,%u,%u,%u obj1Light=%u,%u,%u,%u obj1Dark=%u,%u,%u,%u baked1=%u submitted=%u expanded=%u prim=0x%X)\n",
           sceneOffset,
           translatedCamera.cameraArchiveOffset,
           PCPORT_PDA2_BG_OBJECT1_JOINT_OFFSET,
           PCPORT_PDA2_BG_OBJECT1_DOBJ_OFFSET,
           PCPORT_PDA2_BG_OBJECT1_MOBJ_OFFSET,
           translatedObject1TextureExp.stages[0].texture.tobjArchiveOffset,
           translatedObject1TextureExp.kind,
           translatedObject1TextureExp.stages[0].kind,
           translatedObject1TextureExp.stageCount,
           translatedObject1TextureExp.stages[0].texture.tev.archiveOffset,
           diffPixels,
           baselineCenter[0],
           baselineCenter[1],
           baselineCenter[2],
           baselineCenter[3],
           tevCenter[0],
           tevCenter[1],
           tevCenter[2],
           tevCenter[3],
           translatedObject1TextureExp.stages[0].texture.tev.rampLight[0],
           translatedObject1TextureExp.stages[0].texture.tev.rampLight[1],
           translatedObject1TextureExp.stages[0].texture.tev.rampLight[2],
           translatedObject1TextureExp.stages[0].texture.tev.rampLight[3],
           translatedObject1TextureExp.stages[0].texture.tev.rampDark[0],
           translatedObject1TextureExp.stages[0].texture.tev.rampDark[1],
           translatedObject1TextureExp.stages[0].texture.tev.rampDark[2],
           translatedObject1TextureExp.stages[0].texture.tev.rampDark[3],
           bakedSize1,
           tevSubmitted,
           tevExpanded,
           tevPrimitive);
    ok = 1;

cleanup:
    GSgfxHostClearPipelineState(PCPORT_REAL_TEXTURED_PIPELINE);
    GXHostClearTextureBinding();
    GXHostSetVertexAlphaScale(1.0f);
    free(tevPixels);
    free(baselinePixels);
    PCPort_FreeBuffer(bakedPixels1);
    PCPort_DestroyTranslatedPObj(&translatedObject1PObj);
    PCPort_HSDArchiveDestroy(&archive);
    PCPort_FreeBuffer(memberData);
    return ok;
}

static int RunRealSceneSlice4Smoke(void) {
    PCPortHSDArchive archive;
    PCPortTranslatedCamera translatedCamera;
    PCPortTranslatedPObj translatedObject0PObj;
    PCPortTranslatedJointTransform translatedObject0Joint;
    PCPortTranslatedMaterial translatedObject0Material;
    PCPortTranslatedTextureExp translatedObject0TextureExp;
    PCPortTranslatedPObj translatedObject1PObj;
    PCPortTranslatedJointTransform translatedObject1Joint;
    PCPortTranslatedMaterial translatedObject1Material;
    PCPortTranslatedTextureExp translatedObject1TextureExp;
    PCPortGSDrawObject drawObject0;
    PCPortGSDrawObject drawObject1;
    GXTexObj tevTextureObject0;
    GXTexObj tevTextureObject1;
    u8* memberData = NULL;
    u8* bakedPixels0 = NULL;
    u8* bakedPixels1 = NULL;
    u32 memberSize = 0;
    u32 bakedSize0 = 0;
    u32 bakedSize1 = 0;
    const u8* sceneData;
    unsigned char* baselinePixels = NULL;
    unsigned char* richerPixels = NULL;
    unsigned char baselineCenter[4] = { 0 };
    unsigned char richerCenter[4] = { 0 };
    u32 sceneOffset = 0;
    u32 sceneBranchOffset;
    u32 cameraDescOffset;
    u32 sceneJointListOffset;
    u32 rootJointOffset;
    u32 object0TObjOffset = 0;
    u32 object1TObjOffset = 0;
    u32 object0TextureMapId = 0;
    u32 object1TextureMapId = 0;
    u32 object0NextTObjOffset = 0;
    u32 object0RawSrc0 = 0;
    u32 object0RawSrc1 = 0;
    unsigned char object0CoordId0 = 0;
    unsigned char object0CoordId1 = 0;
    unsigned int diffPixels = 0;
    unsigned int richerSubmitted = 0;
    unsigned int richerExpanded = 0;
    unsigned int richerPrimitive = 0;
    f32 modelViewMatrix0[3][4];
    f32 modelViewMatrix1[3][4];
    int ok = 0;

    memset(&archive, 0, sizeof(archive));
    memset(&translatedCamera, 0, sizeof(translatedCamera));
    memset(&translatedObject0PObj, 0, sizeof(translatedObject0PObj));
    memset(&translatedObject0Joint, 0, sizeof(translatedObject0Joint));
    memset(&translatedObject0Material, 0, sizeof(translatedObject0Material));
    memset(&translatedObject0TextureExp, 0, sizeof(translatedObject0TextureExp));
    memset(&translatedObject1PObj, 0, sizeof(translatedObject1PObj));
    memset(&translatedObject1Joint, 0, sizeof(translatedObject1Joint));
    memset(&translatedObject1Material, 0, sizeof(translatedObject1Material));
    memset(&translatedObject1TextureExp, 0, sizeof(translatedObject1TextureExp));
    memset(&drawObject0, 0, sizeof(drawObject0));
    memset(&drawObject1, 0, sizeof(drawObject1));
    memset(&tevTextureObject0, 0, sizeof(tevTextureObject0));
    memset(&tevTextureObject1, 0, sizeof(tevTextureObject1));

    if (!PCPort_LoadFsysMember(PCPORT_PDA_MENU_ARCHIVE,
                               PCPORT_PDA2_BG_MEMBER,
                               &memberData,
                               &memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 4 load failed (%s:%s)\n",
                PCPORT_PDA_MENU_ARCHIVE,
                PCPORT_PDA2_BG_MEMBER);
        goto cleanup;
    }

    if (!PCPort_HSDArchiveParseBE(&archive, memberData, memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 4 archive parse failed (%s:%s)\n",
                PCPORT_PDA_MENU_ARCHIVE,
                PCPORT_PDA2_BG_MEMBER);
        goto cleanup;
    }

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&archive,
                                                             "scene_data",
                                                             &sceneOffset);
    if (sceneData == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 4 failed to resolve scene_data\n");
        goto cleanup;
    }

    sceneBranchOffset = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&archive, sceneBranchOffset, 0x10u)) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 4 branch offset was invalid (0x%X)\n",
                sceneBranchOffset);
        goto cleanup;
    }

    cameraDescOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x08);
    if (!PCPort_TranslatePerspectiveCameraFromArchiveBE(&archive,
                                                        cameraDescOffset,
                                                        &translatedCamera)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate real scene slice 4 camera (scene=0x%X camera=0x%X)\n",
                sceneOffset,
                cameraDescOffset);
        goto cleanup;
    }

    sceneJointListOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x00);
    rootJointOffset = PCPort_ReadBigEndianU32(archive.storage + sceneJointListOffset + 0x00);

    object0TObjOffset = PCPort_ReadBigEndianU32(archive.storage + PCPORT_PDA2_BG_OBJECT0_MOBJ_OFFSET + 0x08);
    object1TObjOffset = PCPort_ReadBigEndianU32(archive.storage + PCPORT_PDA2_BG_OBJECT1_MOBJ_OFFSET + 0x08);
    object0NextTObjOffset = PCPort_ReadBigEndianU32(archive.storage + object0TObjOffset + 0x04);
    object0RawSrc0 = PCPort_ReadBigEndianU32(archive.storage + object0TObjOffset + 0x0C);
    object0RawSrc1 = PCPort_ReadBigEndianU32(archive.storage + object0NextTObjOffset + 0x0C);
    object0TextureMapId = PCPort_ReadBigEndianU32(archive.storage + object0TObjOffset + 0x08);
    object1TextureMapId = PCPort_ReadBigEndianU32(archive.storage + object1TObjOffset + 0x08);

    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           PCPORT_PDA2_BG_OBJECT0_POBJ_OFFSET,
                                           &translatedObject0PObj)) {
        fprintf(stderr,
                "[pcport_bootstrap] pda2_bg object 0 PObj translation failed after TEX1 bridge changes (pobj=0x%X)\n",
                PCPORT_PDA2_BG_OBJECT0_POBJ_OFFSET);
        goto cleanup;
    }

    if (!PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              PCPORT_PDA2_BG_OBJECT0_JOINT_OFFSET,
                                              &translatedObject0Joint)) {
        fprintf(stderr,
                "[pcport_bootstrap] pda2_bg object 0 joint translation failed (root=0x%X joint=0x%X)\n",
                rootJointOffset,
                PCPORT_PDA2_BG_OBJECT0_JOINT_OFFSET);
        goto cleanup;
    }

    if (!PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               PCPORT_PDA2_BG_OBJECT0_MOBJ_OFFSET,
                                               &translatedObject0Material)) {
        fprintf(stderr,
                "[pcport_bootstrap] pda2_bg object 0 material translation failed (mobj=0x%X)\n",
                PCPORT_PDA2_BG_OBJECT0_MOBJ_OFFSET);
        goto cleanup;
    }

    if (!PCPort_TranslateTextureExpFromArchiveBE(&archive,
                                                 object0TObjOffset,
                                                 &translatedObject0TextureExp)) {
        fprintf(stderr,
                "[pcport_bootstrap] pda2_bg object 0 texture translation failed (tobj=0x%X nextTObj=0x%X rawSrc0=%u rawSrc1=%u)\n",
                object0TObjOffset,
                object0NextTObjOffset,
                object0RawSrc0,
                object0RawSrc1);
        goto cleanup;
    }

    if (!PCPort_TranslatePObjFromArchiveBE(&archive,
                                           PCPORT_PDA2_BG_OBJECT1_POBJ_OFFSET,
                                           &translatedObject1PObj) ||
        !PCPort_TranslateJointChainToMatrixBE(&archive,
                                              rootJointOffset,
                                              PCPORT_PDA2_BG_OBJECT1_JOINT_OFFSET,
                                              &translatedObject1Joint) ||
        !PCPort_TranslateMaterialFromArchiveBE(&archive,
                                               PCPORT_PDA2_BG_OBJECT1_MOBJ_OFFSET,
                                               &translatedObject1Material) ||
        !PCPort_TranslateTextureExpFromArchiveBE(&archive,
                                                 object1TObjOffset,
                                                 &translatedObject1TextureExp)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to translate pda2_bg baseline object 1 for scene slice 4\n");
        goto cleanup;
    }

    object0CoordId0 = translatedObject0TextureExp.stages[0].coordId;
    object0CoordId1 = translatedObject0TextureExp.stages[1].coordId;
    if (translatedObject0TextureExp.kind != PCPORT_TEXTURE_EXP_KIND_I8_RAMP_MASK ||
        translatedObject0TextureExp.stageCount != 2u ||
        translatedObject0TextureExp.stages[0].kind != PCPORT_TEXP_STAGE_I8_RAMP_SAMPLE ||
        translatedObject0TextureExp.stages[1].kind != PCPORT_TEXP_STAGE_I8_MASK_MODULATE ||
        translatedObject1TextureExp.kind != PCPORT_TEXTURE_EXP_KIND_I8_RAMP ||
        translatedObject1TextureExp.stageCount != 1u ||
        translatedObject1TextureExp.stages[0].kind != PCPORT_TEXP_STAGE_I8_RAMP_SAMPLE ||
        object0CoordId0 != 0u || object0CoordId1 != 1u) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 4 did not reach the expected shared TExp boundary (kind0=%u stage00=%u stage01=%u stages0=%u rawSrc0=%u coord0=%u rawSrc1=%u coord1=%u kind1=%u stage10=%u stages1=%u)\n",
                translatedObject0TextureExp.kind,
                translatedObject0TextureExp.stages[0].kind,
                translatedObject0TextureExp.stages[1].kind,
                translatedObject0TextureExp.stageCount,
                object0RawSrc0,
                object0CoordId0,
                object0RawSrc1,
                object0CoordId1,
                translatedObject1TextureExp.kind,
                translatedObject1TextureExp.stages[0].kind,
                translatedObject1TextureExp.stageCount);
        goto cleanup;
    }

    if (PCPort_ReadBigEndianU32(archive.storage + object0NextTObjOffset + 0x04) != 0u ||
        (translatedObject0TextureExp.stages[0].texture.flags & 0x0Fu) != 0u ||
        (translatedObject0TextureExp.stages[1].texture.flags & 0x0Fu) != 0u ||
        (translatedObject0TextureExp.stages[1].texture.flags & 0x00F00000u) != 0x00300000u ||
        translatedObject0TextureExp.stages[1].texture.tev.rampLight[0] != 0u ||
        translatedObject0TextureExp.stages[1].texture.tev.rampLight[1] != 0u ||
        translatedObject0TextureExp.stages[1].texture.tev.rampLight[2] != 0u ||
        translatedObject0TextureExp.stages[1].texture.tev.rampDark[0] != 0u ||
        translatedObject0TextureExp.stages[1].texture.tev.rampDark[1] != 0u ||
        translatedObject0TextureExp.stages[1].texture.tev.rampDark[2] != 0u) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 4 did not match the expected narrow chained TObj shape (next1=0x%X flags0=0x%X flags1=0x%X light1=%u,%u,%u dark1=%u,%u,%u)\n",
                PCPort_ReadBigEndianU32(archive.storage + object0NextTObjOffset + 0x04),
                translatedObject0TextureExp.stages[0].texture.flags,
                translatedObject0TextureExp.stages[1].texture.flags,
                translatedObject0TextureExp.stages[1].texture.tev.rampLight[0],
                translatedObject0TextureExp.stages[1].texture.tev.rampLight[1],
                translatedObject0TextureExp.stages[1].texture.tev.rampLight[2],
                translatedObject0TextureExp.stages[1].texture.tev.rampDark[0],
                translatedObject0TextureExp.stages[1].texture.tev.rampDark[1],
                translatedObject0TextureExp.stages[1].texture.tev.rampDark[2]);
        goto cleanup;
    }

    if (translatedObject0TextureExp.stages[0].texture.tev.kind != PCPORT_TRANSLATED_TEV_I8_COLOR_RAMP ||
        translatedObject1TextureExp.stages[0].texture.tev.kind != PCPORT_TRANSLATED_TEV_I8_COLOR_RAMP) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 4 did not reach the expected base TEV payload boundary (obj0Tev0=0x%X kind0=%u obj0Tev1=0x%X kind1=%u obj1Tev=0x%X kind2=%u)\n",
                translatedObject0TextureExp.stages[0].texture.tev.archiveOffset,
                translatedObject0TextureExp.stages[0].texture.tev.kind,
                translatedObject0TextureExp.stages[1].texture.tev.archiveOffset,
                translatedObject0TextureExp.stages[1].texture.tev.kind,
                translatedObject1TextureExp.stages[0].texture.tev.archiveOffset,
                translatedObject1TextureExp.stages[0].texture.tev.kind);
        goto cleanup;
    }

    if (!PCPort_BakeTextureExpRGBAFromArchiveBE(&archive,
                                                &translatedObject0TextureExp,
                                                &bakedPixels0,
                                                &bakedSize0) ||
        !PCPort_BakeTextureExpRGBAFromArchiveBE(&archive,
                                                &translatedObject1TextureExp,
                                                &bakedPixels1,
                                                &bakedSize1)) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to bake pda2_bg chained TObj payloads for scene slice 4 (obj0Tev0=0x%X obj0Tev1=0x%X obj1Tev=0x%X)\n",
                translatedObject0TextureExp.stages[0].texture.tev.archiveOffset,
                translatedObject0TextureExp.stages[1].texture.tev.archiveOffset,
                translatedObject1TextureExp.stages[0].texture.tev.archiveOffset);
        goto cleanup;
    }

    GXHostInitTexObjRGBA8(&tevTextureObject0,
                          bakedPixels0,
                          translatedObject0TextureExp.stages[0].texture.width,
                          translatedObject0TextureExp.stages[0].texture.height,
                          (GXTexWrapMode)translatedObject0TextureExp.stages[0].texture.wrapS,
                          (GXTexWrapMode)translatedObject0TextureExp.stages[0].texture.wrapT);
    GXHostInitTexObjRGBA8(&tevTextureObject1,
                          bakedPixels1,
                          translatedObject1TextureExp.stages[0].texture.width,
                          translatedObject1TextureExp.stages[0].texture.height,
                          (GXTexWrapMode)translatedObject1TextureExp.stages[0].texture.wrapS,
                          (GXTexWrapMode)translatedObject1TextureExp.stages[0].texture.wrapT);

    drawObject0.displayList = translatedObject0PObj.pobj.display;
    drawObject0.displayListSize = translatedObject0PObj.pobj.n_display;
    drawObject0.pipelineId = PCPORT_REAL_SIBLING_TEXTURED_PIPELINE;
    drawObject0.totalVerts = translatedObject0PObj.totalSubmittedVertices;
    drawObject0.totalPrims = translatedObject0PObj.totalPrimitiveCommands;

    drawObject1.displayList = translatedObject1PObj.pobj.display;
    drawObject1.displayListSize = translatedObject1PObj.pobj.n_display;
    drawObject1.pipelineId = PCPORT_REAL_TEXTURED_PIPELINE;
    drawObject1.totalVerts = translatedObject1PObj.totalSubmittedVertices;
    drawObject1.totalPrims = translatedObject1PObj.totalPrimitiveCommands;

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    VIWaitForRetrace_PC();

    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedObject0Joint.modelMatrix,
                    modelViewMatrix0);
    ConcatAffineMtx(translatedCamera.viewMatrix,
                    translatedObject1Joint.modelMatrix,
                    modelViewMatrix1);

    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_TEXTURED_PIPELINE,
                                        &translatedObject1Material,
                                        &translatedObject1TextureExp.stages[0].texture,
                                        &tevTextureObject1,
                                        (unsigned char)object1TextureMapId);

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();
    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);
    GXLoadPosMtxImm(modelViewMatrix1, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedObject1PObj.pobj);
    fn_800DAD10((void*)&drawObject1);
    glFlush();

    baselinePixels = ReadBackbufferImage();
    if (baselinePixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture baseline framebuffer for real scene slice 4\n");
        goto cleanup;
    }

    ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                          PCPORT_GX_SMOKE_SAMPLE_Y,
                          baselineCenter);

    ConfigureTranslatedTexturedPipeline(PCPORT_REAL_SIBLING_TEXTURED_PIPELINE,
                                        &translatedObject0Material,
                                        &translatedObject0TextureExp.stages[0].texture,
                                        &tevTextureObject0,
                                        (unsigned char)object0TextureMapId);

    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();
    GXSetViewport((f32)translatedCamera.viewportLeft,
                  (f32)translatedCamera.viewportTop,
                  (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                  (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                  0.0f,
                  1.0f);
    GXSetScissor((u32)translatedCamera.scissorLeft,
                 (u32)translatedCamera.scissorTop,
                 (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                 (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
    GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);

    GXLoadPosMtxImm(modelViewMatrix1, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedObject1PObj.pobj);
    fn_800DAD10((void*)&drawObject1);

    GXLoadPosMtxImm(modelViewMatrix0, 0);
    GXSetCurrentMtx(0);
    fn_801AA568(&translatedObject0PObj.pobj);
    fn_800DAD10((void*)&drawObject0);
    glFlush();

    richerSubmitted = GXHostGetLastSubmittedVertexCount();
    richerExpanded = GXHostGetLastExpandedVertexCount();
    richerPrimitive = GXHostGetLastSubmittedPrimitive();

    richerPixels = ReadBackbufferImage();
    if (richerPixels == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Failed to capture richer framebuffer for real scene slice 4\n");
        goto cleanup;
    }

    ReadBackbufferPixelAt(PCPORT_GX_SMOKE_SAMPLE_X,
                          PCPORT_GX_SMOKE_SAMPLE_Y,
                          richerCenter);
    diffPixels = CountFramebufferDiffPixels(baselinePixels, richerPixels);
    if (diffPixels == 0u &&
        baselineCenter[0] == richerCenter[0] &&
        baselineCenter[1] == richerCenter[1] &&
        baselineCenter[2] == richerCenter[2] &&
        baselineCenter[3] == richerCenter[3]) {
        fprintf(stderr,
                "[pcport_bootstrap] Real scene slice 4 admitted object 0 but did not change the baseline scene (scene=0x%X camera=0x%X obj0TexCoordSrc=%u obj0Tev=0x%X baselineCenter=%u,%u,%u,%u richerCenter=%u,%u,%u,%u diffPixels=%u submitted=%u expanded=%u prim=0x%X)\n",
                sceneOffset,
                translatedCamera.cameraArchiveOffset,
                object0RawSrc0,
                translatedObject0TextureExp.stages[0].texture.tev.archiveOffset,
                baselineCenter[0],
                baselineCenter[1],
                baselineCenter[2],
                baselineCenter[3],
                richerCenter[0],
                richerCenter[1],
                richerCenter[2],
                richerCenter[3],
                diffPixels,
                richerSubmitted,
                richerExpanded,
                richerPrimitive);
        goto cleanup;
    }

    printf("[pcport_bootstrap] Real scene slice 4 smoke passed (scene=0x%X camera=0x%X obj0Joint=0x%X obj0DObj=0x%X obj0MObj=0x%X obj0TObj0=0x%X obj0TObj1=0x%X kind0=%u stage00=%u stage01=%u stages0=%u kind1=%u stage10=%u stages1=%u obj0Tev0=0x%X obj0Tev1=0x%X rawSrc0=%u coord0=%u rawSrc1=%u coord1=%u obj1Tev=0x%X diffPixels=%u baselineCenter=%u,%u,%u,%u richerCenter=%u,%u,%u,%u obj0Light=%u,%u,%u,%u obj0Dark=%u,%u,%u,%u baked0=%u baked1=%u submitted=%u expanded=%u prim=0x%X)\n",
           sceneOffset,
           translatedCamera.cameraArchiveOffset,
           PCPORT_PDA2_BG_OBJECT0_JOINT_OFFSET,
           PCPORT_PDA2_BG_OBJECT0_DOBJ_OFFSET,
           PCPORT_PDA2_BG_OBJECT0_MOBJ_OFFSET,
           translatedObject0TextureExp.stages[0].texture.tobjArchiveOffset,
           translatedObject0TextureExp.stages[1].texture.tobjArchiveOffset,
           translatedObject0TextureExp.kind,
           translatedObject0TextureExp.stages[0].kind,
           translatedObject0TextureExp.stages[1].kind,
           translatedObject0TextureExp.stageCount,
           translatedObject1TextureExp.kind,
           translatedObject1TextureExp.stages[0].kind,
           translatedObject1TextureExp.stageCount,
           translatedObject0TextureExp.stages[0].texture.tev.archiveOffset,
           translatedObject0TextureExp.stages[1].texture.tev.archiveOffset,
           object0RawSrc0,
           object0CoordId0,
           object0RawSrc1,
           object0CoordId1,
           translatedObject1TextureExp.stages[0].texture.tev.archiveOffset,
           diffPixels,
           baselineCenter[0],
           baselineCenter[1],
           baselineCenter[2],
           baselineCenter[3],
           richerCenter[0],
           richerCenter[1],
           richerCenter[2],
           richerCenter[3],
           translatedObject0TextureExp.stages[0].texture.tev.rampLight[0],
           translatedObject0TextureExp.stages[0].texture.tev.rampLight[1],
           translatedObject0TextureExp.stages[0].texture.tev.rampLight[2],
           translatedObject0TextureExp.stages[0].texture.tev.rampLight[3],
           translatedObject0TextureExp.stages[0].texture.tev.rampDark[0],
           translatedObject0TextureExp.stages[0].texture.tev.rampDark[1],
           translatedObject0TextureExp.stages[0].texture.tev.rampDark[2],
           translatedObject0TextureExp.stages[0].texture.tev.rampDark[3],
           bakedSize0,
           bakedSize1,
           richerSubmitted,
           richerExpanded,
           richerPrimitive);
    ok = 1;

cleanup:
    GSgfxHostClearPipelineState(PCPORT_REAL_SIBLING_TEXTURED_PIPELINE);
    GSgfxHostClearPipelineState(PCPORT_REAL_TEXTURED_PIPELINE);
    GXHostClearTextureBinding();
    GXHostSetVertexAlphaScale(1.0f);
    free(richerPixels);
    free(baselinePixels);
    PCPort_FreeBuffer(bakedPixels0);
    PCPort_FreeBuffer(bakedPixels1);
    PCPort_DestroyTranslatedPObj(&translatedObject0PObj);
    PCPort_DestroyTranslatedPObj(&translatedObject1PObj);
    PCPort_HSDArchiveDestroy(&archive);
    PCPort_FreeBuffer(memberData);
    return ok;
}

static int HasArg(int argc, char** argv, const char* arg) {
    int i;

    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], arg) == 0) {
            return 1;
        }
    }

    return 0;
}

static int IsEnvEnabled(const char* name) {
    const char* value = getenv(name);

    return value != NULL && value[0] != '\0' && value[0] != '0';
}

static void TraceGLStage(const char* stage) {
    if (IsEnvEnabled("PCPORT_GL_TRACE")) {
        fprintf(stderr, "[pcport_gl] %s\n", stage);
    }
}

static GLFWwindow* CreateSmokeWindow(void) {
    GLFWwindow* window;
    int offscreen = IsEnvEnabled("PCPORT_OFFSCREEN") ||
                    IsEnvEnabled("PCPORT_HEADLESS_GL") ||
                    getenv("PCPORT_DUMP") != NULL;

    TraceGLStage("glfwInit:start");
    if (!glfwInit()) {
        fprintf(stderr, "[pcport_bootstrap] glfwInit failed\n");
        return NULL;
    }
    TraceGLStage("glfwInit:done");

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    if (offscreen) {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    }
    TraceGLStage("glfwCreateWindow:start");

    window = glfwCreateWindow(PCPORT_WINDOW_WIDTH,
                              PCPORT_WINDOW_HEIGHT,
                              "Pokemon Colosseum PC Bootstrap",
                              NULL,
                              NULL);
    if (window == NULL) {
        fprintf(stderr, "[pcport_bootstrap] glfwCreateWindow failed\n");
        glfwTerminate();
        return NULL;
    }
    TraceGLStage("glfwCreateWindow:done");

    TraceGLStage("glfwMakeContextCurrent:start");
    glfwMakeContextCurrent(window);
    TraceGLStage("glfwMakeContextCurrent:done");
    glfwSwapInterval(1);
    if (offscreen) {
        printf("[pcport_bootstrap] headless GL window created (hidden)\n");
    }
    return window;
}

static void DestroySmokeWindow(GLFWwindow* window) {
    if (window != NULL) {
        glfwDestroyWindow(window);
    }

    glfwTerminate();
}

static int RunWindowSmokeLoop(GLFWwindow* window) {
    int frame;
    int framesRan = 0;

    for (frame = 0; frame < PCPORT_WINDOW_FRAMES; ++frame) {
        int fbWidth;
        int fbHeight;
        float colorBias;

        if (glfwWindowShouldClose(window)) {
            break;
        }

        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        colorBias = (float)frame / (float)PCPORT_WINDOW_FRAMES;

        glViewport(0, 0, fbWidth, fbHeight);
        glClearColor(0.08f + (0.10f * colorBias),
                     0.12f + (0.06f * colorBias),
                     0.18f + (0.08f * colorBias),
                     1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
        VIWaitForRetrace_PC();
        ++framesRan;
    }

    printf("[pcport_bootstrap] Window smoke loop completed (%d frames)\n",
           framesRan);
    return framesRan > 0;
}

static int RunGSgfxSmoke(void) {
    unsigned int frameCount;
    unsigned int preRetraceCount;
    unsigned char drawDoneFlag;
    int i;

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    frameCount = GSgfxGetFrameCount();
    if (frameCount != 0xFFFFFFFFu) {
        fprintf(stderr,
                "[pcport_bootstrap] GSgfxInit verification failed (frameCount=%u)\n",
                frameCount);
        return 0;
    }

    for (i = 0; i < PCPORT_GSGFX_SWAPS; ++i) {
        VIWaitForRetrace_PC();
        GSgfxSwapBuffers(1);
    }

    preRetraceCount = GSgfxHostGetPreRetraceCount();
    if (preRetraceCount != PCPORT_GSGFX_SWAPS) {
        fprintf(stderr,
                "[pcport_bootstrap] GSgfx pre-retrace verification failed (count=%u)\n",
                preRetraceCount);
        return 0;
    }

    drawDoneFlag = GSgfxHostGetDrawDoneFlag();
    if (drawDoneFlag == 0) {
        fprintf(stderr,
                "[pcport_bootstrap] GSgfx draw-done verification failed\n");
        return 0;
    }

    printf("[pcport_bootstrap] GSgfx smoke path completed (%d swaps, %u pre-retrace callbacks)\n",
           PCPORT_GSGFX_SWAPS,
           preRetraceCount);
    return 1;
}

/* Counters threaded through the recursive menu-scene draw walk so the caller
 * can report coverage (PCPORT-only diagnostic state). */
typedef struct {
    unsigned int joints;
    unsigned int dobjs;
    unsigned int drawn;
    unsigned int skipped;
    unsigned int textured;
    unsigned int materialOnly;
    unsigned int battleControlPObjSuppressed;
    unsigned int battleMaterialLogged;
} MenuTreeStats;

/*
 * Generalization of ResolveFirstRenderablePObjDescFromJoint: instead of
 * stopping at the first renderable PObjDesc, walk every joint (child at
 * joint+0x08, sibling at joint+0x0C), every DObj on each joint (list head at
 * joint+0x10, next at dobj+0x00), and draw each DObj's PObj (dobj+0x0C) with
 * the translate+draw template proven in RunRealSceneSlice2/3/4Smoke.
 * Every archive read is range-guarded so a malformed/missing node is skipped,
 * never dereferenced. Each PObj is drawn TEXTURED when its material (MObj at
 * dobj+0x08) carries a texture (TObj at mobj+0x08) that the host translators
 * understand: the TObj chain is translated (PCPort_TranslateTextureExpFromArchiveBE),
 * baked to RGBA (PCPort_BakeTextureExpRGBAFromArchiveBE) and uploaded
 * (GXHostInitTexObjRGBA8) then bound through ConfigureTranslatedTexturedPipeline --
 * the exact path proven by the slice-3/slice-4 textured smokes but with NO
 * byte-exact format asserts. On ANY failure (no TObj, unsupported format, range
 * fail, translate/bake fail) the node falls back to the material-only pipeline
 * so flat-shaded geometry still renders and nothing aborts. Per-node baked
 * buffers are freed and the texture binding is cleared after every draw.
 */
/* One-time env override of the directional sun used by scene lighting.
 * PCPORT_LIGHT_DIR="x,y,z" (view space) and PCPORT_LIGHT_AMB=<0..1> tune the
 * ruin-column shading without a rebuild; unset = the gx_tev built-in defaults. */
static void PCPortApplyLightEnv(void) {
    static int applied = 0;
    const char* d;
    const char* a;
    if (applied) {
        return;
    }
    applied = 1;
    d = getenv("PCPORT_LIGHT_DIR");
    a = getenv("PCPORT_LIGHT_AMB");
    if (d != NULL || a != NULL) {
        float dx = 0.55f, dy = 0.50f, dz = 0.55f, amb = 0.18f;
        if (d != NULL) {
            sscanf(d, "%f,%f,%f", &dx, &dy, &dz);
        }
        if (a != NULL) {
            amb = (float)atof(a);
        }
        GXHostSetLightParams(dx, dy, dz, amb);
        printf("[pcport_light] sun=(%.2f,%.2f,%.2f) ambient=%.2f\n",
               dx, dy, dz, amb);
    }
}

/* Cycle guard for RenderJointTree (see its body). Reset on the outermost call. */
#define PCPORT_RJT_VISITED_MAX 16384
static u32 g_rjtVisited[PCPORT_RJT_VISITED_MAX];
static int g_rjtVisitedCount;
static int g_rjtDepth;

/* Per-archive texture cache for RenderJointTree: avoids decoding + re-uploading
 * the same GX texture (same archive image offset + same diffuse modulation) on
 * every frame.  The cache is persistent across frames as long as the archive
 * storage pointer stays the same; it is cleared whenever a new map is loaded
 * (PCPort_EngineFieldSetup changes a->storage).  This makes exterior scenes
 * like S1_out (which re-render many large shared textures per frame) finish
 * within the same ~2s/120-frame budget as smaller interior maps. */
#define PCPORT_RJT_TEX_CACHE_MAX 256
typedef struct {
    u32      archiveOffset; /* baseTexture->imageDataArchiveOffset */
    u32      diffuse;       /* translatedMaterial.diffuse baked into pixels */
    GXTexObj texObj;        /* uploaded GL texture (glTexId valid) */
} PCPortRJTTexCacheEntry;
static PCPortRJTTexCacheEntry g_rjtTexCache[PCPORT_RJT_TEX_CACHE_MAX];
static int                   g_rjtTexCacheCount  = 0;
static const u8*             g_rjtTexCacheArchive = NULL; /* a->storage sentinel */

/* CPU skinned-mesh render for type-2 (envelope) PObjs. The envelope (pobj+0x14)
 * is a null-terminated array of per-matrix-slot pointers; each slot points to a
 * {jobj, weight} list. We build a palette of per-slot joint WORLD matrices, then
 * walk the display list transforming each vertex's position by its PNMTXIDX-slot
 * matrix and submitting it immediate-mode in MODEL space (the view matrix is
 * loaded as the GX pos matrix, so GX applies view+projection). Material/texture
 * pipeline must already be configured by the caller. Returns 1 if it drew.
 * Gated by PCPORT_SKIN. */
#define PCPORT_SKIN_MAX_SLOTS 32

/* out = A * B, where A and B are 3x4 affine matrices (implicit bottom row
 * [0 0 0 1]). Matches PSMTXConcat(A, B). out may not alias A or B. */
static void PCPortMulAffine3x4(const f32 A[3][4], const f32 B[3][4], f32 out[3][4]) {
    int r, c;
    for (r = 0; r < 3; ++r) {
        for (c = 0; c < 3; ++c) {
            out[r][c] = A[r][0]*B[0][c] + A[r][1]*B[1][c] + A[r][2]*B[2][c];
        }
        out[r][3] = A[r][0]*B[0][3] + A[r][1]*B[1][3] + A[r][2]*B[2][3] + A[r][3];
    }
}

/* Invert a 3x4 affine matrix [R|t] -> [R^-1 | -R^-1 t]. R is the top-left 3x3
 * (rotation+scale); t is column 3 (translation). Returns 1 on success, 0 (and
 * fills identity) if R is singular. Alias-safe (writes through a temp). */
static int PCPortInvertAffine3x4(const f32 M[3][4], f32 out[3][4]) {
    f32 inv[3][4];
    f32 det;
    f32 c00 = M[1][1]*M[2][2] - M[1][2]*M[2][1];
    f32 c01 = M[1][2]*M[2][0] - M[1][0]*M[2][2];
    f32 c02 = M[1][0]*M[2][1] - M[1][1]*M[2][0];
    det = M[0][0]*c00 + M[0][1]*c01 + M[0][2]*c02;
    if (det > -1.0e-12f && det < 1.0e-12f) {
        int r, c;
        for (r = 0; r < 3; ++r) for (c = 0; c < 4; ++c) out[r][c] = (r==c)?1.0f:0.0f;
        return 0;
    }
    {
        f32 id = 1.0f / det;
        /* R^-1 = adjugate(R)^T / det */
        inv[0][0] = c00 * id;
        inv[0][1] = (M[0][2]*M[2][1] - M[0][1]*M[2][2]) * id;
        inv[0][2] = (M[0][1]*M[1][2] - M[0][2]*M[1][1]) * id;
        inv[1][0] = c01 * id;
        inv[1][1] = (M[0][0]*M[2][2] - M[0][2]*M[2][0]) * id;
        inv[1][2] = (M[0][2]*M[1][0] - M[0][0]*M[1][2]) * id;
        inv[2][0] = c02 * id;
        inv[2][1] = (M[0][1]*M[2][0] - M[0][0]*M[2][1]) * id;
        inv[2][2] = (M[0][0]*M[1][1] - M[0][1]*M[1][0]) * id;
        /* translation: -R^-1 * t */
        inv[0][3] = -(inv[0][0]*M[0][3] + inv[0][1]*M[1][3] + inv[0][2]*M[2][3]);
        inv[1][3] = -(inv[1][0]*M[0][3] + inv[1][1]*M[1][3] + inv[1][2]*M[2][3]);
        inv[2][3] = -(inv[2][0]*M[0][3] + inv[2][1]*M[1][3] + inv[2][2]*M[2][3]);
    }
    {
        int r, c;
        for (r = 0; r < 3; ++r) for (c = 0; c < 4; ++c) out[r][c] = inv[r][c];
    }
    return 1;
}

static u32 g_skinHist[PCPORT_SKIN_MAX_SLOTS];
static u32 g_skinHistOob;
static int g_skinVtxPosN;
static int g_skinPobjSeq; /* per-frame skinned-PObj counter for PCPORT_SKIN_POBJ */
static f32 g_locMin[3], g_locMax[3], g_wMin[3], g_wMax[3];
static int g_slotEnv[PCPORT_SKIN_MAX_SLOTS]; /* 1 = slot is envelope/blend, 0 = rigid */
static int g_slotInfl[PCPORT_SKIN_MAX_SLOTS]; /* influence count per slot */

/* Set by the main menu to hand off after RunMenuScene returns.
 * g_pcEnterFieldWalk also acts as the "real game session" flag: when set, the
 * field walk auto-loads the real skinned player (Wes/ken_b1) and enables the
 * envelope-skin path WITHOUT the PCPORT_FIELD_WES / PCPORT_SKIN dev env flags.
 * Declared here (ahead of RenderJointTree) so the skin gate can read it. */
static int g_pcEnterFieldWalk = 0;
static int g_pcEnterBattleColosseum = 0;

typedef struct PCPortStoryFieldSmokeState {
    int active;
    int sawMenuHandoff;
    int targetFloor;
    int currentFloor;
    int sawOutskirtLoad;
    int sawShopLoad;
    int mapLoaded;
    int colTris;
    int exitCount;
    int spawnSet;
    int frames;
    int charLoaded;
    int charAnimReady;
    int moved;
    int doorStoryReady;
    int doorStoryOpened;
    int doorStoryAdvanced;
    int doorStoryClosed;
    int doorStoryWarpFloor;
    int doorStoryNpcDrawn;
    u32 doorStoryNpcIndex;
    u32 storyStep;
    u32 cutsceneState;
    f32 spawn[3];
    f32 finalPos[3];
    char mapPath[128];
} PCPortStoryFieldSmokeState;

static PCPortStoryFieldSmokeState g_pcStoryFieldSmoke;
static int g_pcFieldWarpSmokeActive;
static f32 g_pcFieldWarpSmokeStickY;

static void PCPort_StoryFieldSmokeBegin(void) {
    memset(&g_pcStoryFieldSmoke, 0, sizeof(g_pcStoryFieldSmoke));
    g_pcStoryFieldSmoke.active = 1;
    g_pcStoryFieldSmoke.targetFloor = -1;
    g_pcStoryFieldSmoke.currentFloor = -1;
    g_pcStoryFieldSmoke.doorStoryWarpFloor = -1;
}

/* Build the per-slot skinning-matrix palette for an envelope PObj, replicating
 * the GameCube envelope-skin display (hsd_pobj_disp fn_801AAEA8). Each matrix
 * slot's envelope is a list of {jobj, weight} influences; the FIRST influence's
 * weight selects the path (the asm's `fcmpo; cror eq,gt,eq; bne` against ~1.0):
 *
 *   weight >= 1.0  -> RIGID single bone. Verts are JOINT-LOCAL; palette is the
 *       bone's world matrix (jobj->mtx, +0x44) applied directly. Legs/boots.
 *   weight <  1.0  -> ENVELOPE BLEND. Verts are MODEL (bind-pose) space; palette
 *       is Sum_i w_i*(jointWorld_i * invBind_i), invBind = inverse(bindWorld).
 *       Blend weights sum to 1.0, so at the rest pose every term is w_i*I and the
 *       palette is identity -> the model-space vert passes through. Coat/torso.
 *
 * We render the static rest pose, so invBind = inverse(jointWorld_rest) is
 * derived directly from the joint chain (no FIFO, no per-character bake, scales
 * to the whole cast). Returns the slot count (palette[0..count) valid). */
static int BuildSkinPalette(const PCPortHSDArchive* a, u32 envOff, u32 rootJoint,
                            f32 palette[PCPORT_SKIN_MAX_SLOTS][3][4]) {
    const char* rwEnv = getenv("PCPORT_SKIN_RIGID_W");
    f32 rigidW = (rwEnv != NULL && rwEnv[0]) ? (f32)atof(rwEnv) : 1.0f;
    int slot;
    for (slot = 0; slot < PCPORT_SKIN_MAX_SLOTS; ++slot) {
        u32 entry = PCPort_ReadBigEndianU32(a->storage + envOff + (u32)slot * 4u);
        f32 (*M)[4] = palette[slot];
        int r, c, any = 0, k, isEnvelope;
        union { u32 u; f32 f; } w0;
        for (r = 0; r < 3; ++r) for (c = 0; c < 4; ++c) M[r][c] = 0.0f;
        if (!ArchiveRangeValid(a, entry, 0x8u)) {
            break; /* null terminator -> palette size = slot */
        }
        w0.u = PCPort_ReadBigEndianU32(a->storage + entry + 4u);
        isEnvelope = (w0.f < rigidW); /* first-entry weight < ~1.0 -> blend */
        {
        int influences = 0;
        for (k = 0; k < 16; ++k) { /* {jobj,weight} list, jobj==0 ends it */
            u32 jobj = PCPort_ReadBigEndianU32(a->storage + entry + (u32)k * 8u + 0u);
            union { u32 u; f32 f; } w;
            PCPortTranslatedJointTransform jt;
            if (!ArchiveRangeValid(a, jobj, PCPORT_SERIALIZED_JOINT_SIZE)) {
                break;
            }
            ++influences;
            w.u = PCPort_ReadBigEndianU32(a->storage + entry + (u32)k * 8u + 4u);
            memset(&jt, 0, sizeof(jt));
            if (PCPort_TranslateJointChainToMatrixBE(a, rootJoint, jobj, &jt)) {
                if (getenv("PCPORT_SKIN_PAL") != NULL && slot < 8) {
                    fprintf(stderr, "  [infl] slot %d jobj=0x%X w=%.3f env=%d\n",
                            slot, jobj, (double)w.f, isEnvelope);
                }
                if (isEnvelope) {
                    /* term = w * (jointWorld * inverse(bindWorld)); at rest
                     * inverse(bindWorld) = inverse(jointWorld) -> term = w*I. */
                    f32 invBind[3][4], skinned[3][4];
                    if (PCPortInvertAffine3x4(jt.modelMatrix, invBind)) {
                        PCPortMulAffine3x4(jt.modelMatrix, invBind, skinned);
                    } else {
                        for (r = 0; r < 3; ++r) for (c = 0; c < 4; ++c)
                            skinned[r][c] = (r==c)?1.0f:0.0f;
                    }
                    for (r = 0; r < 3; ++r)
                        for (c = 0; c < 4; ++c)
                            M[r][c] += w.f * skinned[r][c];
                    any = 1;
                } else {
                    /* RIGID single bone: jointWorld applied to joint-local
                     * verts. Use the first influence only and stop. */
                    for (r = 0; r < 3; ++r)
                        for (c = 0; c < 4; ++c)
                            M[r][c] = jt.modelMatrix[r][c];
                    any = 1;
                    break;
                }
            }
        }
        if (!any) {
            for (r = 0; r < 3; ++r) for (c = 0; c < 4; ++c) M[r][c] = 0.0f;
            M[0][0] = M[1][1] = M[2][2] = 1.0f;
        }
        if (slot < PCPORT_SKIN_MAX_SLOTS) {
            g_slotEnv[slot] = isEnvelope ? 1 : 0;
            g_slotInfl[slot] = influences;
        }
        if (getenv("PCPORT_SKIN_PAL") != NULL && slot < 8) {
            fprintf(stderr,
                "[skinpal] slot %d entry=0x%X infl=%d env=%d resolved=%d "
                "translate=(%.2f %.2f %.2f)\n",
                slot, entry, influences, isEnvelope, any,
                M[0][3], M[1][3], M[2][3]);
        }
        }
    }
    return slot;
}

/* Walk an envelope PObj's display list and accumulate the WORLD-space AABB of
 * its skinned vertices (palette[slot] * local). This is the true rendered bound,
 * unlike the PObj's stored model-space min/max (which for verts on rigid slots
 * is meaningless -- they get re-placed at their bone). Used to frame the
 * model-view auto-camera correctly for skinned characters. */
static void AccumulateSkinnedPObjWorldAABB(const PCPortHSDArchive* a, u32 pobjOffset,
                                           const PCPortTranslatedPObj* tp, u32 rootJoint,
                                           f32 outMin[3], f32 outMax[3], int* any) {
    f32 palette[PCPORT_SKIN_MAX_SLOTS][3][4];
    int slot;
    const u8* dl = tp->displayList;
    const u8* end;
    u32 envOff;
    if (tp->verts == NULL || dl == NULL) return;
    envOff = PCPort_ReadBigEndianU32(a->storage + pobjOffset + 0x14);
    if (envOff == 0u || !ArchiveRangeValid(a, envOff, 0x4u)) return;
    end = dl + tp->pobj.n_display;
    slot = BuildSkinPalette(a, envOff, rootJoint, palette);
    if (slot == 0) return;
    while (dl + 3 <= end) {
        u8 cmd = dl[0];
        u16 vcount, vi;
        if (cmd == 0u) { dl += 1; continue; } /* skip GX NOP pad (see submit loop) */
        if ((cmd & 0xF8u) == 0u) break;
        vcount = (u16)(((u16)dl[1] << 8) | dl[2]);
        dl += 3;
        if (vcount == 0u) break;
        for (vi = 0; vi < vcount; ++vi) {
            u32 curSlot = 0u;
            f32 px = 0.0f, py = 0.0f, pz = 0.0f;
            const HSD_VtxDescList* a2;
            for (a2 = tp->verts; a2->attr != GX_VA_NULL; ++a2) {
                u32 idx; int isz;
                if (a2->attr <= GX_VA_TEX7MTXIDX) {
                    if (dl + 1 > end) return;
                    if (a2->attr == GX_VA_PNMTXIDX) curSlot = (u32)dl[0] / 3u;
                    dl += 1; continue;
                }
                isz = (a2->attr_type == GX_INDEX16) ? 2 : 1;
                if (dl + isz > end) return;
                idx = (isz == 2) ? (u32)(((u16)dl[0] << 8) | dl[1]) : (u32)dl[0];
                dl += isz;
                if (a2->attr == GX_VA_POS && tp->positionData != NULL) {
                    const f32* p = (const f32*)((const u8*)tp->positionData + (size_t)idx * a2->stride);
                    px = p[0]; py = p[1];
                    pz = (a2->comp_cnt == GX_POS_XYZ) ? p[2] : 0.0f;
                }
            }
            if (curSlot >= (u32)slot) curSlot = 0u;
            {
                f32 (*M)[4] = palette[curSlot];
                f32 wx = M[0][0]*px + M[0][1]*py + M[0][2]*pz + M[0][3];
                f32 wy = M[1][0]*px + M[1][1]*py + M[1][2]*pz + M[1][3];
                f32 wz = M[2][0]*px + M[2][1]*py + M[2][2]*pz + M[2][3];
                if (!*any) {
                    outMin[0]=outMax[0]=wx; outMin[1]=outMax[1]=wy; outMin[2]=outMax[2]=wz;
                    *any = 1;
                } else {
                    if (wx<outMin[0])outMin[0]=wx; if (wx>outMax[0])outMax[0]=wx;
                    if (wy<outMin[1])outMin[1]=wy; if (wy>outMax[1])outMax[1]=wy;
                    if (wz<outMin[2])outMin[2]=wz; if (wz>outMax[2])outMax[2]=wz;
                }
            }
        }
    }
}

static int RenderSkinnedPObj(const PCPortHSDArchive* a, u32 pobjOffset,
                             const PCPortTranslatedPObj* tp, u32 rootJoint,
                             const PCPortTranslatedCamera* cam,
                             int haveTexture, GXTexObj* textureObject,
                             u8 textureMapId, u8 textureTevMode) {
    u32 envOff = PCPort_ReadBigEndianU32(a->storage + pobjOffset + 0x14);
    f32 palette[PCPORT_SKIN_MAX_SLOTS][3][4];
    int slot;
    const HSD_VtxDescList* v;
    const HSD_VtxDescList* posD = NULL;
    const HSD_VtxDescList* nrmD = NULL;
    const HSD_VtxDescList* clrD = NULL;
    const HSD_VtxDescList* texD = NULL;
    const u8* dl = tp->displayList;
    const u8* end;
    /* PCPORT_SKIN_POBJ=N renders only the Nth skinned PObj this frame (others are
     * skipped) to isolate a single mesh; PCPORT_SKIN_POBJTINT colours each PObj
     * distinctly. Diagnostic for the per-pobj torso/limb placement. */
    {
        const char* only = getenv("PCPORT_SKIN_POBJ");
        int idx = g_skinPobjSeq++;
        if (only != NULL && only[0] != '\0' && atoi(only) != idx) {
            return 1; /* skip drawing this pobj, but count it as handled */
        }
    }

    if (envOff == 0u || tp->verts == NULL || dl == NULL ||
        !ArchiveRangeValid(a, envOff, 0x4u)) {
        if (getenv("PCPORT_SKIN_BAIL") != NULL)
            fprintf(stderr, "[skinbail] pobj@0x%X envOff=0x%X verts=%p dl=%p -> head check\n",
                    pobjOffset, envOff, (void*)tp->verts, (void*)dl);
        return 0;
    }
    end = dl + tp->pobj.n_display;

    /* Build the per-slot skinning-matrix palette (the exact GameCube envelope-skin
     * math; see BuildSkinPalette). */
    slot = BuildSkinPalette(a, envOff, rootJoint, palette);
    if (slot == 0) {
        if (getenv("PCPORT_SKIN_BAIL") != NULL)
            fprintf(stderr, "[skinbail] pobj@0x%X envOff=0x%X -> slot==0 (no valid envelope entry)\n",
                    pobjOffset, envOff);
        return 0;
    }

    for (v = tp->verts; v->attr != GX_VA_NULL; ++v) {
        if (v->attr == GX_VA_POS)  posD = v;
        else if (v->attr == GX_VA_NRM) nrmD = v;
        else if (v->attr == GX_VA_CLR0) clrD = v;
        else if (v->attr == GX_VA_TEX0) texD = v;
    }
    if (getenv("PCPORT_MIRROR_DBG") != NULL) {
        /* position-array offset within the archive: two PObjs sharing the SAME
         * offset reuse the SAME vertex data (a mirror/instanced mesh). */
        long posOff = (posD != NULL && tp->positionData != NULL)
                        ? (long)((const u8*)tp->positionData - a->storage) : -1;
        fprintf(stderr, "[share] pobj@0x%X posArray=0x%lX stride=%u verts=%u dl=%u\n",
                pobjOffset, posOff, posD ? posD->stride : 0,
                tp->pobj.n_display, tp->pobj.n_display);
    }
    if (posD == NULL) {
        if (getenv("PCPORT_SKIN_BAIL") != NULL)
            fprintf(stderr, "[skinbail] pobj@0x%X -> no POS vtx descriptor\n", pobjOffset);
        return 0;
    }

    /* Load the camera view as the GX position matrix; submit model-space verts. */
    GXLoadPosMtxImm(cam->viewMatrix, 0);
    GXSetCurrentMtx(0);

    /* Texture binding for the IMMEDIATE-mode submit.
     *
     * The rigid draw path (fn_800DAD10) binds the node texture by REPLAYING the
     * GSgfx pipeline state that ConfigureTranslatedTexturedPipeline registered
     * via GSgfxHostSetPipelineTexture -- but the immediate GXBegin/GXEnd path
     * below does NOT run that pipeline, so the GX shim's immediate-mode texture
     * slot (g_boundTextureId / g_numTexGens / stage-0 TEV mode) is whatever the
     * previous draw left (typically cleared to 0 -> the mesh sampled NO texture
     * and rendered solid white). Bind the node texture DIRECTLY here so the
     * immediate submit (GXSubmitVertices -> GXSubmitViaShader) sees a live
     * texture + a textured TEV mode (it only samples when g_boundTextureId != 0
     * && g_numTexGens != 0 && tevMode != GX_PASSCLR). When the node has no
     * texture fall back to vertex-colour-only (PASSCLR). */
    /* Diagnostics for the skinned-mesh fidelity work:
     *   PCPORT_SKIN_REPLACE  -> force GX_REPLACE (texture only, ignore vtx colour)
     *   PCPORT_SKIN_WHITE    -> force white vertex colour (isolate texture decode)
     * Lets us separate "texture decodes dark" from "baked vertex colours are dark
     * (model expects runtime lighting we don't yet drive)". */
    int dbgReplace = (getenv("PCPORT_SKIN_REPLACE") != NULL);
    int dbgWhite   = (getenv("PCPORT_SKIN_WHITE") != NULL);
    int dbgNoMtx   = (getenv("PCPORT_SKIN_NOMTX") != NULL) || g_pcBattleModelSpaceVerts;
    int dbgVtxPos  = (getenv("PCPORT_SKIN_VTXPOS") != NULL);
    f32 dbgViewMin[3] = {0.0f, 0.0f, 0.0f};
    f32 dbgViewMax[3] = {0.0f, 0.0f, 0.0f};
    f32 dbgNdcMin[3] = {0.0f, 0.0f, 0.0f};
    f32 dbgNdcMax[3] = {0.0f, 0.0f, 0.0f};
    int dbgViewAny = 0;
    int dbgNearFail = 0;
    memset(g_skinHist, 0, sizeof(g_skinHist));
    g_skinHistOob = 0;
    g_skinVtxPosN = 0;

    /* The character meshes carry a dark albedo (navy coat, black boots) that the
     * GameCube lit at runtime; we don't yet drive GX dynamic lights for the skin
     * submit, so an unlit sample reads near-black. Lift non-battle character
     * skins two ways for a recognizable, lit-looking model:
     *   - derivative face-normal lambert (u_lightingEnabled) gives 3D form, and
     *   - an exposure gain (>1) brightens the dark albedo toward the lit art.
     * Battle PKX textures are already bright; the same gain clips Zangoose into
     * a pink/white blob, so keep battle skin exposure neutral unless overridden.
     * Both are tunable live and restored to neutral after the submit so other
     * draws (title/field/menu) are unaffected. */
    {
        const char* expEnv = getenv("PCPORT_EXPOSURE");
        const char* battleExpEnv = getenv("PCPORT_BATTLE_SKIN_EXPOSURE");
        const char* ambEnv = getenv("PCPORT_SKIN_AMB");
        f32 defaultExposure = g_pcBattleRenderSkin ? 1.0f : 2.4f;
        f32 exposure = (expEnv != NULL && expEnv[0] != '\0')
            ? (f32)atof(expEnv)
            : defaultExposure;
        if (g_pcBattleRenderSkin &&
            battleExpEnv != NULL && battleExpEnv[0] != '\0') {
            exposure = (f32)atof(battleExpEnv);
        }
        f32 ambient  = (ambEnv != NULL && ambEnv[0] != '\0') ? (f32)atof(ambEnv) : 0.55f;
        GXHostSetExposure(exposure);
        GXHostSetLightingEnabled(GX_TRUE);
        /* Key light from the upper-front-left in view space (matches the
         * reference's three-quarter key). */
        GXHostSetLightParams(-0.4f, 0.7f, 0.6f, ambient);
    }
    if (haveTexture && textureObject != NULL) {
        GXTexMapID mapId = (GXTexMapID)textureMapId;
        if (mapId == GX_TEXMAP_NULL) mapId = GX_TEXMAP0;
        GXLoadTexObj(textureObject, mapId);
        GXSetNumTexGens(1);
        /* The translated TObj carries the node's TEV mode (MODULATE/REPLACE/...);
         * if it decoded as PASSCLR (no texture) force MODULATE so the bound
         * texture is actually sampled and modulated by the vertex colour. */
        GXSetTevOp(GX_TEVSTAGE0,
                   dbgReplace ? GX_REPLACE :
                   (textureTevMode == (u8)GX_PASSCLR)
                       ? GX_MODULATE
                       : (GXTevMode)textureTevMode);
    } else {
        GXSetNumTexGens(0);
        GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    }

    /* Per-pobj joint-local detection (e.g. Shedinja's halo): a halo is authored at
     * the model origin and lifted above the head by ONE bone. Pre-walk this pobj's
     * verts for their centroid + dominant matrix slot; if the centroid sits near the
     * origin but its rigid bone is far away, place the WHOLE pobj by that bone
     * matrix. Decided once per pobj so a triangle is NEVER split across two
     * transforms (the per-vertex version scattered the wings/Gulpin). Model-space
     * body/wing pobjs sit AT their bone -> fail the test -> pass through unchanged.
     * Disable with PCPORT_BATTLE_NO_HALO_LIFT. */
    g_skinPobjPlaceByBone = 0;
    /* Root scale: joint world matrices are in the skeleton frame, which for these
     * models is a larger scale than the model-space body verts (joints reach y~15
     * but the body tops out ~y=5). The model's ROOT joint carries that scale, so a
     * lifted (joint-local) piece must be divided by it to land in the body's frame.
     * Read it from the game data (root joint SRT: scale @ +0x20, BE float). */
    {
        union { u32 u; f32 f; } rs;
        const char* hs;
        f32 ds;
        rs.u = PCPort_ReadBigEndianU32(a->storage + rootJoint + 0x20u);
        ds = (rs.f > 0.05f && rs.f < 100.0f) ? rs.f : 1.0f;
        /* The root joint scale is ~1 for these models, yet the skeleton joints are
         * authored ~2x the model-space verts (body tops ~y=5, head joint ~y=10).
         * Use the data root scale when it is meaningful (>1.2); otherwise fall back
         * to the body<->skeleton frame ratio, tunable via PCPORT_HALO_SCALE. */
        hs = getenv("PCPORT_HALO_SCALE");
        if (hs != NULL && hs[0]) g_haloRootScale = (f32)atof(hs);   /* manual override */
        else if (ds > 1.2f) g_haloRootScale = ds;                   /* real root scale */
        else g_haloRootScale = g_haloModelRatio;                    /* data-derived ratio */
    }
    if (dbgNoMtx && posD != NULL && tp->positionData != NULL &&
        getenv("PCPORT_BATTLE_NO_HALO_LIFT") == NULL) {
        const u8* pdl = dl;
        f32 cx = 0.0f, cy = 0.0f, cz = 0.0f, maxMag2 = 0.0f;
        int cn = 0, domN = 0;
        u32 cur = 0u, domSlot = 0u;
        u32 scount[PCPORT_SKIN_MAX_SLOTS];
        memset(scount, 0, sizeof(scount));
        while (pdl + 3 <= end) {
            u8 c = pdl[0];
            u16 vc; u32 k;
            if (c == 0u) { pdl += 1; continue; }
            if ((c & 0xF8u) == 0u) break;
            vc = (u16)(((u16)pdl[1] << 8) | pdl[2]);
            pdl += 3;
            if (vc == 0u) break;
            for (k = 0; k < vc; ++k) {
                const HSD_VtxDescList* ad;
                for (ad = tp->verts; ad->attr != GX_VA_NULL; ++ad) {
                    u32 ix; int isz;
                    if (ad->attr <= GX_VA_TEX7MTXIDX) {
                        if (pdl + 1 > end) { pdl = end; break; }
                        if (ad->attr == GX_VA_PNMTXIDX) cur = (u32)pdl[0] / 3u;
                        pdl += 1; continue;
                    }
                    isz = (ad->attr_type == GX_INDEX16) ? 2 : 1;
                    if (pdl + isz > end) { pdl = end; break; }
                    ix = (isz == 2) ? (u32)(((u16)pdl[0] << 8) | pdl[1]) : (u32)pdl[0];
                    pdl += isz;
                    if (ad->attr == GX_VA_POS) {
                        const f32* pp = (const f32*)((const u8*)tp->positionData +
                                                     (size_t)ix * ad->stride);
                        f32 vz2 = (ad->comp_cnt == GX_POS_XYZ) ? pp[2] : 0.0f;
                        f32 m2 = pp[0]*pp[0] + pp[1]*pp[1] + vz2*vz2;
                        if (m2 > maxMag2) maxMag2 = m2;
                        cx += pp[0]; cy += pp[1]; cz += vz2;
                        ++cn;
                    }
                }
                if (cur < PCPORT_SKIN_MAX_SLOTS &&
                    (int)(++scount[cur]) > domN) { domN = (int)scount[cur]; domSlot = cur; }
            }
        }
        if (cn > 0 && domSlot < (u32)slot && g_slotEnv[domSlot] == 0) {
            f32 (*Mp)[4] = palette[domSlot];
            f32 tx = Mp[0][3], ty = Mp[1][3], tz = Mp[2][3];
            f32 tmag = tx*tx + ty*ty + tz*tz;
            f32 ccx = cx / (f32)cn, ccy = cy / (f32)cn, ccz = cz / (f32)cn;
            f32 cmag2 = ccx*ccx + ccy*ccy + ccz*ccz;
            f32 domFrac = (f32)domN / (f32)cn;
            /* Halo signature: a SMALL, SINGLE-bone piece whose verts all sit near the
             * model origin (maxMag2/cmag2 small) while its bone is lifted high (tmag).
             * Spread model-space body/wing pieces have large maxMag2 -> excluded, so
             * the wings/Gulpin stay model-space (clean) and only the halo lifts. */
            if (getenv("PCPORT_HALO_DBG") != NULL) {
                fprintf(stderr, "[halo] v=%d centroid=(%.1f,%.1f,%.1f) cR=%.1f maxR=%.1f "
                        "domSlot=%u domFrac=%.2f boneY=%.1f tmag=%.0f rootScale=%.2f "
                        "-> liftY=%.1f\n",
                        cn, ccx, ccy, ccz, sqrtf(cmag2), sqrtf(maxMag2),
                        domSlot, domFrac, ty, tmag, g_haloRootScale,
                        ty / (g_haloRootScale > 0.05f ? g_haloRootScale : 1.0f));
            }
            /* Data-derived skeleton<->model scale: a MODEL-SPACE body piece sits with
             * its verts at model scale but its bone at skeleton scale, so boneY/vertY
             * is exactly the model's skeleton/model ratio (Shedinja: head joint ~10 /
             * head vert ~5 = 2.0). Average it over the body pieces; the lifted halo is
             * then divided by THIS data ratio (no guessed constant). Only single-bone
             * pieces clear of the origin contribute (clean joint<->vert correspondence). */
            if (cmag2 >= 1.0f && ccy > 1.5f && ty > 1.5f && domFrac > 0.6f) {
                f32 r = ty / ccy;
                if (r > 1.1f && r < 6.0f)
                    g_haloModelRatio = (g_haloModelRatio <= 0.0f)
                                           ? r : (g_haloModelRatio * 0.85f + r * 0.15f);
            }
            /* Measured (Shedinja): halo pieces have centroid AT the origin (cR~0.2,
             * cmag2~0.04) and are compact (maxR<2.5); every body/wing piece has
             * cR>=1.0 (cmag2>=1.0). So cmag2<0.5 is a clean separator -- lift only
             * the origin-centred, single-bone, high-bone (lifted) pieces. */
            if (cmag2 < 0.5f && maxMag2 < 9.0f && domFrac > 0.85f && tmag > 16.0f)
                g_skinPobjPlaceByBone = 1;
        }
    }

    while (dl + 3 <= end) {
        u8 cmd = dl[0];
        u16 vcount;
        u32 vi;
        if (cmd == 0u) {
            /* GX NOP byte (0x00): display lists are padded with NOPs for 32-byte
             * alignment, and one can appear BETWEEN primitives. Skip it -- do NOT
             * treat it as end-of-list, or every primitive after the pad is dropped
             * (this was silently losing the mirrored half of shared-vertex meshes,
             * e.g. mania's left chest/coat). */
            dl += 1;
            continue;
        }
        if ((cmd & 0xF8u) == 0u) {
            break; /* genuinely malformed opcode (0x01-0x07) -> stop */
        }
        vcount = (u16)(((u16)dl[1] << 8) | dl[2]);
        dl += 3;
        if (vcount == 0u) {
            break;
        }
        {
            /* PCPORT_SKIN_SKIPCMD=0xNN skips drawing primitives of that GX type
             * (still consumes their verts) to isolate which primitive produces
             * the bad geometry. */
            const char* sc = getenv("PCPORT_SKIN_SKIPCMD");
            if (sc != NULL && sc[0] != '\0' &&
                (u32)strtol(sc, NULL, 0) == (u32)(cmd & 0xF8u)) {
                u32 vskip;
                for (vskip = 0; vskip < vcount; ++vskip) {
                    const HSD_VtxDescList* as;
                    for (as = tp->verts; as->attr != GX_VA_NULL; ++as) {
                        int sz = (as->attr <= GX_VA_TEX7MTXIDX) ? 1
                               : ((as->attr_type == GX_INDEX16) ? 2 : 1);
                        if (dl + sz > end) break;
                        dl += sz;
                    }
                }
                continue;
            }
        }
        if (dbgVtxPos) {
            fprintf(stderr, "[dl] prim cmd=0x%02X (%s) vcount=%u\n",
                    cmd & 0xF8u,
                    (cmd & 0xF8u) == 0x98u ? "TRISTRIP" :
                    (cmd & 0xF8u) == 0xA0u ? "TRIFAN" :
                    (cmd & 0xF8u) == 0x90u ? "TRIANGLES" :
                    (cmd & 0xF8u) == 0x80u ? "QUADS" : "?",
                    vcount);
        }
        GXBegin((GXPrimitive)(cmd & 0xF8u), GX_VTXFMT0, vcount);
        for (vi = 0; vi < vcount; ++vi) {
            u32 curSlot = 0u;
            f32 px = 0.0f, py = 0.0f, pz = 0.0f, u = 0.0f, vv = 0.0f;
            f32 nx = 0.0f, ny = 0.0f, nz = 0.0f;
            u8 cr = 255, cg = 255, cb = 255, ca = 255;
            int haveTex = 0, haveCol = 0, haveNrm = 0;
            const HSD_VtxDescList* a2;
            for (a2 = tp->verts; a2->attr != GX_VA_NULL; ++a2) {
                u32 idx;
                int isz;
                if (a2->attr <= GX_VA_TEX7MTXIDX) {
                    /* 1-byte inline matrix index. PNMTXIDX selects the skinning
                     * palette slot (byte/3); the TEXnMTXIDX bytes are consumed
                     * but not yet used for host texgen. */
                    if (dl + 1 > end) { GXEnd(); return 1; }
                    if (a2->attr == GX_VA_PNMTXIDX) curSlot = (u32)dl[0] / 3u;
                    dl += 1;
                    continue;
                }
                isz = (a2->attr_type == GX_INDEX16) ? 2 : 1;
                if (dl + isz > end) { GXEnd(); return 1; }
                idx = (isz == 2) ? (u32)(((u16)dl[0] << 8) | dl[1]) : (u32)dl[0];
                dl += isz;
                if (a2->attr == GX_VA_POS && tp->positionData != NULL) {
                    const f32* p = (const f32*)((const u8*)tp->positionData + (size_t)idx * a2->stride);
                    px = p[0]; py = p[1];
                    pz = (a2->comp_cnt == GX_POS_XYZ) ? p[2] : 0.0f;
                    if (dbgVtxPos && g_skinVtxPosN < 9999) {
                        u32 cs = (curSlot < PCPORT_SKIN_MAX_SLOTS) ? curSlot : 0u;
                        fprintf(stderr, "[idx] v#%d slot=%u posIdx=%u env=%d infl=%d local=(%.3f,%.3f,%.3f)\n",
                                g_skinVtxPosN, curSlot, idx,
                                g_slotEnv[cs], g_slotInfl[cs], px, py, pz);
                    }
                } else if (a2->attr == GX_VA_NRM && tp->normalData != NULL && nrmD != NULL) {
                    const f32* n = (const f32*)((const u8*)tp->normalData + (size_t)idx * a2->stride);
                    nx = n[0]; ny = n[1]; nz = n[2]; haveNrm = 1;
                } else if (a2->attr == GX_VA_TEX0 && tp->texcoordData != NULL && texD != NULL) {
                    const f32* t = (const f32*)((const u8*)tp->texcoordData + (size_t)idx * a2->stride);
                    u = t[0]; vv = t[1]; haveTex = 1;
                } else if (a2->attr == GX_VA_CLR0 && tp->colorData != NULL && clrD != NULL) {
                    const u8* cptr = (const u8*)tp->colorData + (size_t)idx * a2->stride;
                    cr = cptr[0]; cg = cptr[1]; cb = cptr[2];
                    ca = (a2->comp_type == GX_RGBA8) ? cptr[3] : 255;
                    haveCol = 1;
                }
            }
            if (curSlot >= (u32)slot) { curSlot = 0u; ++g_skinHistOob; }
            if (curSlot < PCPORT_SKIN_MAX_SLOTS) ++g_skinHist[curSlot];
            if (dbgVtxPos) {
                f32 (*Mp)[4] = palette[curSlot];
                f32 wx = Mp[0][0]*px+Mp[0][1]*py+Mp[0][2]*pz+Mp[0][3];
                f32 wy = Mp[1][0]*px+Mp[1][1]*py+Mp[1][2]*pz+Mp[1][3];
                f32 wz = Mp[2][0]*px+Mp[2][1]*py+Mp[2][2]*pz+Mp[2][3];
                if (g_skinVtxPosN == 0) {
                    g_locMin[0]=g_locMax[0]=px; g_locMin[1]=g_locMax[1]=py; g_locMin[2]=g_locMax[2]=pz;
                    g_wMin[0]=g_wMax[0]=wx; g_wMin[1]=g_wMax[1]=wy; g_wMin[2]=g_wMax[2]=wz;
                } else {
                    if(px<g_locMin[0])g_locMin[0]=px; if(px>g_locMax[0])g_locMax[0]=px;
                    if(py<g_locMin[1])g_locMin[1]=py; if(py>g_locMax[1])g_locMax[1]=py;
                    if(pz<g_locMin[2])g_locMin[2]=pz; if(pz>g_locMax[2])g_locMax[2]=pz;
                    if(wx<g_wMin[0])g_wMin[0]=wx; if(wx>g_wMax[0])g_wMax[0]=wx;
                    if(wy<g_wMin[1])g_wMin[1]=wy; if(wy>g_wMax[1])g_wMax[1]=wy;
                    if(wz<g_wMin[2])g_wMin[2]=wz; if(wz>g_wMax[2])g_wMax[2]=wz;
                }
                /* flag any vert whose LOCAL coords are large (model-space, not bone-local) */
                if (px*px+py*py+pz*pz > 25.0f && g_skinVtxPosN < 9999)
                    fprintf(stderr, "[vtxpos] BIG-LOCAL #%d slot=%u local=(%.1f,%.1f,%.1f) world=(%.1f,%.1f,%.1f)\n",
                            g_skinVtxPosN, curSlot, px,py,pz, wx,wy,wz);
                ++g_skinVtxPosN;
            }
            {
                /* Apply the per-vertex skinning matrix palette[curSlot] uniformly.
                 * The palette build already encodes the right transform for each
                 * slot's space: the bone's world matrix for RIGID (joint-local)
                 * slots, and the identity for ENVELOPE (model-space) slots at the
                 * rest pose (jointWorld*invBind == I). So one multiply is correct
                 * for both -- no per-vertex magnitude heuristic, no flung arms.
                 * PCPORT_SKIN_NOMTX submits raw local coords for A/B comparison. */
                if (dbgWhite || !haveCol) GXColor4u8(255,255,255,255); else GXColor4u8(cr, cg, cb, ca);
                if (dbgNoMtx) {
                    /* Model-space pass-through for the whole mesh. Joint-local
                     * accessory pieces (e.g. Shedinja's halo) are lifted PER-POBJ
                     * before this loop (g_skinPobjPlaceByBone) -- never per-vertex,
                     * which would split a triangle's corners apart and scatter the
                     * wings. So here it is a clean pass-through or a uniform bone
                     * placement decided once for the whole pobj. */
                    if (g_skinPobjPlaceByBone) {
                        /* Place the joint-local piece by its bone, then divide by the
                         * model root scale so it lands in the body's frame (the joint
                         * frame is root-scaled larger than the model verts). This is
                         * the game-data position, no guessed cap. */
                        f32 (*Mp)[4] = palette[curSlot];
                        f32 inv = (g_haloRootScale > 0.05f) ? (1.0f / g_haloRootScale) : 1.0f;
                        f32 wx = (Mp[0][0]*px + Mp[0][1]*py + Mp[0][2]*pz + Mp[0][3]) * inv;
                        f32 wy = (Mp[1][0]*px + Mp[1][1]*py + Mp[1][2]*pz + Mp[1][3]) * inv;
                        f32 wz = (Mp[2][0]*px + Mp[2][1]*py + Mp[2][2]*pz + Mp[2][3]) * inv;
                        if (haveNrm) {
                            f32 tx = Mp[0][0]*nx + Mp[0][1]*ny + Mp[0][2]*nz;
                            f32 ty = Mp[1][0]*nx + Mp[1][1]*ny + Mp[1][2]*nz;
                            f32 tz = Mp[2][0]*nx + Mp[2][1]*ny + Mp[2][2]*nz;
                            f32 tl = sqrtf(tx*tx + ty*ty + tz*tz);
                            if (tl > 1e-6f) { nx = tx / tl; ny = ty / tl; nz = tz / tl; }
                        }
                        GXPosition3f32(wx, wy, wz);
                    } else {
                        GXPosition3f32(px, py, pz);
                    }
                } else {
                    f32 (*Mx)[4] = palette[curSlot];
                    f32 wx = Mx[0][0]*px + Mx[0][1]*py + Mx[0][2]*pz + Mx[0][3];
                    f32 wy = Mx[1][0]*px + Mx[1][1]*py + Mx[1][2]*pz + Mx[1][3];
                    f32 wz = Mx[2][0]*px + Mx[2][1]*py + Mx[2][2]*pz + Mx[2][3];
                    if (haveNrm) {
                        f32 tx = Mx[0][0]*nx + Mx[0][1]*ny + Mx[0][2]*nz;
                        f32 ty = Mx[1][0]*nx + Mx[1][1]*ny + Mx[1][2]*nz;
                        f32 tz = Mx[2][0]*nx + Mx[2][1]*ny + Mx[2][2]*nz;
                        f32 tl = sqrtf(tx*tx + ty*ty + tz*tz);
                        if (tl > 1e-6f) { nx = tx / tl; ny = ty / tl; nz = tz / tl; }
                    }
                    if (dbgVtxPos) {
                        f32 vx = cam->viewMatrix[0][0]*wx + cam->viewMatrix[0][1]*wy +
                                 cam->viewMatrix[0][2]*wz + cam->viewMatrix[0][3];
                        f32 vy = cam->viewMatrix[1][0]*wx + cam->viewMatrix[1][1]*wy +
                                 cam->viewMatrix[1][2]*wz + cam->viewMatrix[1][3];
                        f32 vz = cam->viewMatrix[2][0]*wx + cam->viewMatrix[2][1]*wy +
                                 cam->viewMatrix[2][2]*wz + cam->viewMatrix[2][3];
                        f32 cx = cam->projectionMatrix[0][0]*vx +
                                 cam->projectionMatrix[0][1]*vy +
                                 cam->projectionMatrix[0][2]*vz +
                                 cam->projectionMatrix[0][3];
                        f32 cy = cam->projectionMatrix[1][0]*vx +
                                 cam->projectionMatrix[1][1]*vy +
                                 cam->projectionMatrix[1][2]*vz +
                                 cam->projectionMatrix[1][3];
                        f32 cz = cam->projectionMatrix[2][0]*vx +
                                 cam->projectionMatrix[2][1]*vy +
                                 cam->projectionMatrix[2][2]*vz +
                                 cam->projectionMatrix[2][3];
                        f32 cw = cam->projectionMatrix[3][0]*vx +
                                 cam->projectionMatrix[3][1]*vy +
                                 cam->projectionMatrix[3][2]*vz +
                                 cam->projectionMatrix[3][3];
                        if (vz > -1.0f) {
                            ++dbgNearFail;
                        }
                        if (cw != 0.0f) {
                            cx /= cw;
                            cy /= cw;
                            cz /= cw;
                        }
                        if (!dbgViewAny) {
                            dbgViewMin[0]=dbgViewMax[0]=vx;
                            dbgViewMin[1]=dbgViewMax[1]=vy;
                            dbgViewMin[2]=dbgViewMax[2]=vz;
                            dbgNdcMin[0]=dbgNdcMax[0]=cx;
                            dbgNdcMin[1]=dbgNdcMax[1]=cy;
                            dbgNdcMin[2]=dbgNdcMax[2]=cz;
                            dbgViewAny = 1;
                        } else {
                            if (vx<dbgViewMin[0]) dbgViewMin[0]=vx; if (vx>dbgViewMax[0]) dbgViewMax[0]=vx;
                            if (vy<dbgViewMin[1]) dbgViewMin[1]=vy; if (vy>dbgViewMax[1]) dbgViewMax[1]=vy;
                            if (vz<dbgViewMin[2]) dbgViewMin[2]=vz; if (vz>dbgViewMax[2]) dbgViewMax[2]=vz;
                            if (cx<dbgNdcMin[0]) dbgNdcMin[0]=cx; if (cx>dbgNdcMax[0]) dbgNdcMax[0]=cx;
                            if (cy<dbgNdcMin[1]) dbgNdcMin[1]=cy; if (cy>dbgNdcMax[1]) dbgNdcMax[1]=cy;
                            if (cz<dbgNdcMin[2]) dbgNdcMin[2]=cz; if (cz>dbgNdcMax[2]) dbgNdcMax[2]=cz;
                        }
                    }
                    GXPosition3f32(wx, wy, wz);
                }
                if (haveNrm) {
                    GXNormal3f32(nx, ny, nz);
                }
                GXTexCoord2f32(haveTex ? u : 0.0f, haveTex ? vv : 0.0f);
            }
        }
        GXEnd();
    }
    if (getenv("PCPORT_DL_DBG") != NULL) {
        fprintf(stderr, "[dl] pobj@0x%X consumed %ld of %u bytes (stopped at cmd=0x%02X)\n",
                pobjOffset, (long)(dl - tp->displayList), (unsigned)tp->pobj.n_display,
                (dl + 3 <= end) ? dl[0] : 0xFF);
    }
    if (getenv("PCPORT_SKIN_VTXPOS") != NULL && g_skinVtxPosN > 0) {
        fprintf(stderr, "[vtxpos] LOCAL bbox=[%.1f,%.1f,%.1f .. %.1f,%.1f,%.1f] "
                "WORLD bbox=[%.1f,%.1f,%.1f .. %.1f,%.1f,%.1f] (n=%d)\n",
                g_locMin[0],g_locMin[1],g_locMin[2], g_locMax[0],g_locMax[1],g_locMax[2],
                g_wMin[0],g_wMin[1],g_wMin[2], g_wMax[0],g_wMax[1],g_wMax[2], g_skinVtxPosN);
        if (dbgViewAny) {
            fprintf(stderr, "[vtxclip] VIEW bbox=[%.1f,%.1f,%.1f .. %.1f,%.1f,%.1f] "
                    "NDC bbox=[%.2f,%.2f,%.2f .. %.2f,%.2f,%.2f] nearFail=%d/%d\n",
                    dbgViewMin[0],dbgViewMin[1],dbgViewMin[2],
                    dbgViewMax[0],dbgViewMax[1],dbgViewMax[2],
                    dbgNdcMin[0],dbgNdcMin[1],dbgNdcMin[2],
                    dbgNdcMax[0],dbgNdcMax[1],dbgNdcMax[2],
                    dbgNearFail, g_skinVtxPosN);
        }
    }
    if (getenv("PCPORT_SKIN_HIST") != NULL) {
        int s; u32 tot = 0;
        fprintf(stderr, "[skinhist] slots=%d oob=%u hist=", slot, g_skinHistOob);
        for (s = 0; s < slot && s < PCPORT_SKIN_MAX_SLOTS; ++s) {
            fprintf(stderr, "%u:%u ", s, g_skinHist[s]);
            tot += g_skinHist[s];
        }
        fprintf(stderr, "(total=%u)\n", tot);
    }
    /* Restore neutral shading state so subsequent draws are unaffected. */
    GXHostSetExposure(1.0f);
    GXHostSetLightingEnabled(GX_FALSE);
    return 1;
}

/* ---- Model-bounds walker for the model-view auto-camera --------------------
 *
 * Walks a joint tree (same shape as RenderJointTree: dobj/pobj chains, sibling
 * iteration, cycle guard) and accumulates a WORLD-space AABB over every PObj's
 * geometry. For rigid (type-0/1) PObjs the local min/max corners are transformed
 * by the joint's world matrix; for envelope (type-2 skinned) PObjs the vertices
 * are already in model/bind space (RenderSkinnedPObj submits them as-is), so the
 * local bounds are used directly. The result is the union over all walked nodes.
 * Used only to frame the camera -- an approximation is fine. */
static u32 g_aabbVisited[PCPORT_RJT_VISITED_MAX];
static int g_aabbVisitedCount;
static int g_aabbDepth;

static void AccumulateModelAABB(const PCPortHSDArchive* a,
                                u32 rootJoint, u32 joint,
                                f32 outMin[3], f32 outMax[3], int* any) {
    u32 dobjOffset;
    int vi;

    if (g_aabbDepth++ == 0) {
        g_aabbVisitedCount = 0;
    }
    for (;;) {
        if (!ArchiveRangeValid(a, joint, PCPORT_SERIALIZED_JOINT_SIZE)) {
            g_aabbDepth--; return;
        }
        for (vi = 0; vi < g_aabbVisitedCount; ++vi) {
            if (g_aabbVisited[vi] == joint) { g_aabbDepth--; return; }
        }
        if (g_aabbVisitedCount < PCPORT_RJT_VISITED_MAX) {
            g_aabbVisited[g_aabbVisitedCount++] = joint;
        }

        dobjOffset = PCPort_ReadBigEndianU32(a->storage + joint + 0x10);
        while (dobjOffset != 0u &&
               ArchiveRangeValid(a, dobjOffset, PCPORT_SERIALIZED_DOBJ_SIZE)) {
            u32 pobjOffset = PCPort_ReadBigEndianU32(a->storage + dobjOffset + 0x0C);
            int pobjGuard = 0;
            while (ArchiveRangeValid(a, pobjOffset, PCPORT_SERIALIZED_POBJ_SIZE)) {
                PCPortTranslatedPObj tp;
                PCPortTranslatedJointTransform jt;
                if (++pobjGuard > 4096) break; /* cyclic PObj chain guard */
                memset(&tp, 0, sizeof(tp));
                memset(&jt, 0, sizeof(jt));
                if (PCPort_TranslatePObjFromArchiveBE(a, pobjOffset, &tp) &&
                    tp.totalSubmittedVertices > 0u) {
                    int isEnvelope = (((tp.pobj.flags >> 12) & 3u) == 2u);
                    if (isEnvelope) {
                        /* Skinned PObj: the stored model-space min/max is wrong for
                         * the camera (verts on rigid slots get re-placed at their
                         * bone), so compute the TRUE world bounds by skinning each
                         * vertex through the same palette the render uses. */
                        AccumulateSkinnedPObjWorldAABB(a, pobjOffset, &tp, rootJoint,
                                                       outMin, outMax, any);
                    } else {
                        int cx, cy, cz;
                        if (!PCPort_TranslateJointChainToMatrixBE(a, rootJoint, joint, &jt)) {
                            /* fall back to identity (local-space bounds) */
                            memset(&jt, 0, sizeof(jt));
                            jt.modelMatrix[0][0] = jt.modelMatrix[1][1] =
                                jt.modelMatrix[2][2] = 1.0f;
                        }
                        /* transform all 8 AABB corners by the joint world matrix */
                        for (cx = 0; cx < 2; ++cx)
                        for (cy = 0; cy < 2; ++cy)
                        for (cz = 0; cz < 2; ++cz) {
                            f32 lx = cx ? tp.maxPosition[0] : tp.minPosition[0];
                            f32 ly = cy ? tp.maxPosition[1] : tp.minPosition[1];
                            f32 lz = cz ? tp.maxPosition[2] : tp.minPosition[2];
                            f32 (*M)[4] = jt.modelMatrix;
                            f32 wx = M[0][0]*lx + M[0][1]*ly + M[0][2]*lz + M[0][3];
                            f32 wy = M[1][0]*lx + M[1][1]*ly + M[1][2]*lz + M[1][3];
                            f32 wz = M[2][0]*lx + M[2][1]*ly + M[2][2]*lz + M[2][3];
                            if (!*any) {
                                outMin[0] = outMax[0] = wx;
                                outMin[1] = outMax[1] = wy;
                                outMin[2] = outMax[2] = wz;
                                *any = 1;
                            } else {
                                if (wx < outMin[0]) outMin[0] = wx;
                                if (wy < outMin[1]) outMin[1] = wy;
                                if (wz < outMin[2]) outMin[2] = wz;
                                if (wx > outMax[0]) outMax[0] = wx;
                                if (wy > outMax[1]) outMax[1] = wy;
                                if (wz > outMax[2]) outMax[2] = wz;
                            }
                        }
                    }
                }
                /* walk the PObj `next` chain (split skinned meshes); pobj next@+0x04 */
                {
                    u32 pn = PCPort_ReadBigEndianU32(a->storage + pobjOffset + 0x04);
                    if (pn == pobjOffset) break;
                    pobjOffset = pn;
                }
            }
            /* dobj `next` @ +0x04 (matches RenderJointTree) */
            {
                u32 dn = PCPort_ReadBigEndianU32(a->storage + dobjOffset + 0x04);
                if (dn == dobjOffset) break;
                dobjOffset = dn;
            }
        }

        {
            u32 childOffset = PCPort_ReadBigEndianU32(a->storage + joint + 0x08);
            if (childOffset != 0u && childOffset != joint) {
                AccumulateModelAABB(a, rootJoint, childOffset, outMin, outMax, any);
            }
        }
        {
            u32 nextOffset = PCPort_ReadBigEndianU32(a->storage + joint + 0x0C);
            if (nextOffset == 0u || nextOffset == joint) break;
            joint = nextOffset;
        }
    }
    g_aabbDepth--;
}

static void RenderJointTree(const PCPortHSDArchive* a,
                            u32 rootJoint,
                            u32 joint,
                            const PCPortTranslatedCamera* cam,
                            int pipelineId,
                            MenuTreeStats* stats) {
    u32 dobjOffset;
    u32 childOffset;
    u32 nextOffset;
    int vi;

    /* Sibling (next) chain iterates, not recurses. Plus a cycle guard: field-map
     * joint graphs are small (~40 joints) but CONTAIN CYCLES (shared sub-trees /
     * back-references) that as naive recursion overflowed the stack. Skip any
     * joint already visited this walk; the visited set resets on the outermost
     * call (g_rjtDepth 0->1). Children stay recursive (shallow); siblings loop. */
    if (g_rjtDepth++ == 0) {
        g_rjtVisitedCount = 0;
        PCPortApplyLightEnv();
        /* Invalidate the texture cache when the archive changes (new map loaded). */
        if (a->storage != g_rjtTexCacheArchive) {
            g_rjtTexCacheCount  = 0;
            g_rjtTexCacheArchive = a->storage;
        }
    }
    for (;;) {

    if (!ArchiveRangeValid(a, joint, PCPORT_SERIALIZED_JOINT_SIZE)) {
        g_rjtDepth--;
        return;
    }
    for (vi = 0; vi < g_rjtVisitedCount; ++vi) {
        if (g_rjtVisited[vi] == joint) {   /* cycle / shared node: stop */
            g_rjtDepth--;
            return;
        }
    }
    if (g_rjtVisitedCount < PCPORT_RJT_VISITED_MAX) {
        g_rjtVisited[g_rjtVisitedCount++] = joint;
    }
    stats->joints++;

    dobjOffset = PCPort_ReadBigEndianU32(a->storage + joint + 0x10);
    while (dobjOffset != 0u &&
           ArchiveRangeValid(a, dobjOffset, PCPORT_SERIALIZED_DOBJ_SIZE)) {
        u32 pobjOffset = PCPort_ReadBigEndianU32(a->storage + dobjOffset + 0x0C);
        u32 mobjOffset = PCPort_ReadBigEndianU32(a->storage + dobjOffset + 0x08);
        int pobjChainGuard = 0;

        stats->dobjs++;

        /* Walk the PObj `next` chain (+0x04), not just the first PObj. A DObj's
         * skinned body splits its mesh across several PObjs (head / arms / torso);
         * rendering only the first dropped the arms + torso for high-detail models
         * like ken_a1 (the battle Wes). The model-bounds walker already does this,
         * so the auto-camera framed geometry the render never drew. */
        while (ArchiveRangeValid(a, pobjOffset, PCPORT_SERIALIZED_POBJ_SIZE)) {
            PCPortTranslatedPObj translatedPObj;
            PCPortTranslatedJointTransform translatedJoint;
            PCPortTranslatedMaterial translatedMaterial;
            PCPortTranslatedTextureExp translatedTextureExp;
            PCPortGSDrawObject drawObject;
            GXTexObj nodeTextureObject;
            f32 modelViewMatrix[3][4];
            u8* bakedPixels = NULL;
            if (++pobjChainGuard > 4096) break; /* cyclic PObj chain guard */
            u32 bakedSize = 0u;
            u32 tobjOffset = 0u;
            u32 textureMapId = 0u;
            int haveMaterial = 0;
            int haveTexture = 0;
            int battleControlPObj = 0;
            int isLogoTex = 0;

            memset(&translatedPObj, 0, sizeof(translatedPObj));
            memset(&translatedJoint, 0, sizeof(translatedJoint));
            memset(&translatedMaterial, 0, sizeof(translatedMaterial));
            memset(&translatedTextureExp, 0, sizeof(translatedTextureExp));
            memset(&drawObject, 0, sizeof(drawObject));
            memset(&nodeTextureObject, 0, sizeof(nodeTextureObject));

            if (PCPort_TranslatePObjFromArchiveBE(a, pobjOffset, &translatedPObj) &&
                PCPort_TranslateJointChainToMatrixBE(a, rootJoint, joint,
                                                     &translatedJoint)) {
                if (ArchiveRangeValid(a, mobjOffset, 0x4u)) {
                    haveMaterial = PCPort_TranslateMaterialFromArchiveBE(
                        a, mobjOffset, &translatedMaterial);
                }

                /* Textured path: the MObj->TObj link lives at mobj+0x08 (same
                 * read the slice-3/slice-4 smokes use) and the texture-map id at
                 * tobj+0x08. Translate the TObj chain (PCPort_TranslateTextureExpFromArchiveBE)
                 * and bake it to a linear RGBA buffer (PCPort_BakeTextureExpRGBAFromArchiveBE,
                 * the slice-4 template) which the bake function fills for the
                 * I8-ramp / I8-ramp+mask families AND, for plain non-TEV nodes,
                 * by decoding the native GX format (CMPR/RGBA8/I8/...) through
                 * DecodeTextureToRGBA. The RGBA is uploaded with GXHostInitTexObjRGBA8
                 * and bound through ConfigureTranslatedTexturedPipeline -- exactly
                 * the path proven by RunRealSceneSlice4Smoke (129-colour textured
                 * bg) but with NO byte-exact texture-shape asserts. On ANY failure
                 * (no link, range fail, unsupported format, translate/bake fail,
                 * zero extent) haveTexture stays 0 and the node falls back to the
                 * material-only pipeline so flat-shaded geometry still renders.
                 * Never aborts. */
                if (haveMaterial && ArchiveRangeValid(a, mobjOffset, 0x0Cu)) {
                    int texRangeOk;
                    int texExpOk = 0;
                    int texBakeOk = 0;
                    tobjOffset =
                        PCPort_ReadBigEndianU32(a->storage + mobjOffset + 0x08);
                    texRangeOk = ArchiveRangeValid(a, tobjOffset, 0x0Cu);
                    if (texRangeOk) {
                        texExpOk = PCPort_TranslateTextureExpFromArchiveBE(
                            a, tobjOffset, &translatedTextureExp);
                    }
                    if (texExpOk && translatedTextureExp.stageCount != 0u &&
                        translatedTextureExp.stages[0].texture.width != 0u &&
                        translatedTextureExp.stages[0].texture.height != 0u) {
                        /* Texture cache lookup: skip decode + GPU upload when the
                         * same (archiveOffset, diffuse) was already uploaded this
                         * map session.  Cuts per-frame cost for large exterior maps
                         * (S1_out) from ~0.9 s/frame down to ~D1_garage_1F levels. */
                        u32 cacheOff  = translatedTextureExp.stages[0].texture.imageDataArchiveOffset;
                        u32 cacheDiff = haveMaterial ? translatedMaterial.diffuse : 0xFFFFFFFFu;
                        int ci, cacheHit = 0;
                        for (ci = 0; ci < g_rjtTexCacheCount; ++ci) {
                            if (g_rjtTexCache[ci].archiveOffset == cacheOff &&
                                g_rjtTexCache[ci].diffuse      == cacheDiff) {
                                memcpy(&nodeTextureObject, &g_rjtTexCache[ci].texObj,
                                       sizeof(GXTexObj));
                                textureMapId = PCPort_ReadBigEndianU32(
                                    a->storage + tobjOffset + 0x08);
                                haveTexture = 1;
                                cacheHit = 1;
                                break;
                            }
                        }
                        if (!cacheHit) {
                            texBakeOk = PCPort_BakeTextureExpRGBAFromArchiveBE(
                                a, &translatedTextureExp, &bakedPixels, &bakedSize);
                        }
                    }
                    if (texBakeOk && bakedPixels != NULL) {
                        const PCPortTranslatedTexture* baseTexture =
                            &translatedTextureExp.stages[0].texture;

                        /* Diagnostic: PCPORT_DUMP_TEX=0x293E0 writes the raw
                         * decoded (pre-diffuse-modulation) RGBA of the matching
                         * texture once, so the actual sandstone detail can be
                         * inspected apart from lighting. */
                        {
                            static int dumpedTex = 0;
                            const char* dt = getenv("PCPORT_DUMP_TEX");
                            if (dt != NULL && !dumpedTex &&
                                baseTexture->imageDataArchiveOffset ==
                                    (u32)strtoul(dt, NULL, 0)) {
                                char tp[256];
                                snprintf(tp, sizeof(tp), "build_pc/tex_%X.bmp",
                                         baseTexture->imageDataArchiveOffset);
                                DumpFramebufferBMPTo(bakedPixels,
                                                     (int)baseTexture->width,
                                                     (int)baseTexture->height, tp);
                                dumpedTex = 1;
                                printf("[texdump] wrote %s (%ux%u)\n", tp,
                                       baseTexture->width, baseTexture->height);
                            }
                        }

                        /* The crisp logo is drawn by the 2D overlay; skip the
                         * scene's own logo billboard AND its two glow billboards
                         * (810x336 @ 0x693E0/0x8AB60) so they don't ghost over
                         * the crisp 2D logo. */
                        isLogoTex =
                            (baseTexture->imageDataArchiveOffset == PCPORT_LOGO_IMAGE_OFFSET ||
                             baseTexture->imageDataArchiveOffset == 0x693E0u ||
                             baseTexture->imageDataArchiveOffset == 0x8AB60u);
                        /* Debug: PCPORT_SKIP_TEX=0xNNNNN skips every mesh using that
                         * texture (e.g. 0x293E0 = the tan haze/ground planes) so the
                         * geometry behind them can be inspected. */
                        {
                            const char* sk = getenv("PCPORT_SKIP_TEX");
                            if (sk != NULL &&
                                baseTexture->imageDataArchiveOffset ==
                                    (u32)strtoul(sk, NULL, 0)) {
                                isLogoTex = 1;
                            }
                        }
                        if (getenv("PCPORT_HAZE_DEBUG") != NULL && haveMaterial) {
                            printf("[mat] tex=0x%X rmode=0x%X xlu=%d hasPE=%d alpha=%.2f "
                                   "peType=%u src=%u dst=%u zComp=%u aComp0=%u ref0=%u\n",
                                   baseTexture->imageDataArchiveOffset,
                                   translatedMaterial.rendermode,
                                   (translatedMaterial.rendermode & PCPORT_RENDER_XLU) ? 1 : 0,
                                   translatedMaterial.hasPEDesc, translatedMaterial.alpha,
                                   translatedMaterial.peType, translatedMaterial.peSrcFactor,
                                   translatedMaterial.peDstFactor, translatedMaterial.peZComp,
                                   translatedMaterial.peAlphaComp0, translatedMaterial.peRef0);
                        }

                        textureMapId = PCPort_ReadBigEndianU32(
                            a->storage + tobjOffset + 0x08);

                        /* Modulate the baked texture by the material diffuse
                         * colour (texture x diffuse). The desert ground/ruins
                         * texture with a near-white haze/glow; their real stone/
                         * sand tone is the material diffuse, so without this they
                         * render washed-out at full brightness. White-diffuse
                         * materials are left unchanged. */
                        if (haveMaterial) {
                            u32 dr = (translatedMaterial.diffuse >> 24) & 0xFFu;
                            u32 dg = (translatedMaterial.diffuse >> 16) & 0xFFu;
                            u32 db = (translatedMaterial.diffuse >> 8) & 0xFFu;
                            if (dr != 0xFFu || dg != 0xFFu || db != 0xFFu) {
                                u32 px = (u32)baseTexture->width *
                                         (u32)baseTexture->height;
                                u32 i;
                                for (i = 0; i < px; ++i) {
                                    bakedPixels[(i * 4u) + 0u] = (u8)(
                                        (bakedPixels[(i * 4u) + 0u] * dr) / 255u);
                                    bakedPixels[(i * 4u) + 1u] = (u8)(
                                        (bakedPixels[(i * 4u) + 1u] * dg) / 255u);
                                    bakedPixels[(i * 4u) + 2u] = (u8)(
                                        (bakedPixels[(i * 4u) + 2u] * db) / 255u);
                                }
                            }
                        }

                        GXHostInitTexObjRGBA8(
                            &nodeTextureObject,
                            bakedPixels,
                            baseTexture->width,
                            baseTexture->height,
                            (GXTexWrapMode)baseTexture->wrapS,
                            (GXTexWrapMode)baseTexture->wrapT);
                        haveTexture = 1;
                        /* Cache the uploaded GL texture for reuse across frames. */
                        if (g_rjtTexCacheCount < PCPORT_RJT_TEX_CACHE_MAX) {
                            g_rjtTexCache[g_rjtTexCacheCount].archiveOffset =
                                baseTexture->imageDataArchiveOffset;
                            g_rjtTexCache[g_rjtTexCacheCount].diffuse =
                                haveMaterial ? translatedMaterial.diffuse : 0xFFFFFFFFu;
                            memcpy(&g_rjtTexCache[g_rjtTexCacheCount].texObj,
                                   &nodeTextureObject, sizeof(GXTexObj));
                            g_rjtTexCacheCount++;
                        }
                    } else if (getenv("PCPORT_TEX_DEBUG") != NULL &&
                               tobjOffset != 0u) {
                        u32 imageDebug = 0u;
                        u32 formatDebug = 0xFFFFFFFFu;
                        u16 widthDebug = 0u;
                        u16 heightDebug = 0u;
                        if (ArchiveRangeValid(a, tobjOffset, 0x50u)) {
                            imageDebug = PCPort_ReadBigEndianU32(
                                a->storage + tobjOffset + 0x4C);
                            if (ArchiveRangeValid(a, imageDebug, 0x0Cu)) {
                                widthDebug = (u16)(
                                    ((u16)a->storage[imageDebug + 0x04] << 8) |
                                    (u16)a->storage[imageDebug + 0x05]);
                                heightDebug = (u16)(
                                    ((u16)a->storage[imageDebug + 0x06] << 8) |
                                    (u16)a->storage[imageDebug + 0x07]);
                                formatDebug = PCPort_ReadBigEndianU32(
                                    a->storage + imageDebug + 0x08);
                            }
                        }
                        printf("[tex-debug] fail joint=0x%X dobj=0x%X "
                               "mobj=0x%X pobj=0x%X tobj=0x%X img=0x%X "
                               "fmt=0x%X size=%ux%u range=%d exp=%d "
                               "stages=%u bake=%d pixels=%d\n",
                               joint, dobjOffset, mobjOffset, pobjOffset,
                               tobjOffset, imageDebug, formatDebug,
                               widthDebug, heightDebug, texRangeOk, texExpOk,
                               translatedTextureExp.stageCount, texBakeOk,
                               bakedPixels != NULL ? 1 : 0);
                        fflush(stdout);
                    }
                }

                drawObject.displayList = translatedPObj.pobj.display;
                drawObject.displayListSize = translatedPObj.pobj.n_display;
                drawObject.pipelineId =
                    haveTexture ? PCPORT_REAL_TEXTURED_PIPELINE
                                : (unsigned int)pipelineId;
                drawObject.totalVerts = translatedPObj.totalSubmittedVertices;
                drawObject.totalPrims = translatedPObj.totalPrimitiveCommands;
                battleControlPObj = PCPort_IsBattleControlPObj(
                    &translatedPObj, &translatedMaterial, haveMaterial,
                    haveTexture);

                if (haveTexture) {
                    /* Cloud / scene UV-scroll: if the field-anim TexAnim drove
                     * a UV translation for this TObj, apply it as a texture
                     * matrix so the GLSL shader shifts the UV coordinates.
                     * coordId 0 = GX_TEXCOORD0 (v_texcoord0 in the shader). */
                    {
                        f32 animU = 0.0f, animV = 0.0f;
                        if (PCPort_FieldAnimGetTexUV(tobjOffset, &animU, &animV)) {
                            /* 3x4 row-major UV translation matrix.
                             * gx_tev_set_tex_matrix uses only the upper 3x3:
                             *   [1  0  animU]       (u,v,1) · this = (u+animU, v+animV)
                             *   [0  1  animV]
                             *   [0  0  1    ] */
                            f32 uvMtx[3][4] = {
                                { 1.0f, 0.0f, animU, 0.0f },
                                { 0.0f, 1.0f, animV, 0.0f },
                                { 0.0f, 0.0f, 1.0f, 0.0f }
                            };
                            u32 coordSlot = haveTexture
                                ? (u32)translatedTextureExp.stages[0].texture.coordId
                                : 0u;
                            if (coordSlot >= 8u) coordSlot = 0u;
                            GXHostSetTexMatrix(coordSlot, (const f32(*)[4])uvMtx);
                        }
                    }
                    ConfigureTranslatedTexturedPipeline(
                        PCPORT_REAL_TEXTURED_PIPELINE,
                        &translatedMaterial,
                        &translatedTextureExp.stages[0].texture,
                        &nodeTextureObject,
                        (unsigned char)textureMapId);
                    stats->textured++;
                } else {
                    if (battleControlPObj) {
                        u32 tobjDebug = 0u;
                        if (ArchiveRangeValid(a, mobjOffset, 0x0Cu)) {
                            tobjDebug = PCPort_ReadBigEndianU32(
                                a->storage + mobjOffset + 0x08);
                        }
                        if (g_pcBattleMaterialLogBudget > 0u) {
                            printf("[battle-material] suppress-control pobj=%u joint=0x%X "
                                   "dobj=0x%X mobj=0x%X pobj=0x%X flags=0x%X "
                                   "verts=%u mat=%d tobj=0x%X alpha=%.2f "
                                   "diffuse=0x%08X reason=no-tobj-gray-24v\n",
                                   stats->dobjs, joint, dobjOffset, mobjOffset,
                                   pobjOffset, translatedPObj.pobj.flags,
                                   translatedPObj.totalSubmittedVertices,
                                   haveMaterial, tobjDebug,
                                   translatedMaterial.alpha,
                                   translatedMaterial.diffuse);
                            fflush(stdout);
                            g_pcBattleMaterialLogBudget--;
                        }
                        stats->battleControlPObjSuppressed++;
                    }
                    stats->materialOnly++;
                    if (!battleControlPObj) {
                        ConfigureTranslatedMaterialPipeline(
                            (unsigned int)pipelineId,
                            haveMaterial ? &translatedMaterial : NULL);
                    }
                }
                GXHostSetVertexAlphaScale(1.0f);

                ConcatAffineMtx(cam->viewMatrix,
                                translatedJoint.modelMatrix,
                                modelViewMatrix);
                GXLoadPosMtxImm(modelViewMatrix, 0);
                GXSetCurrentMtx(0);
                /* Apply this PObj's vertex descriptor + arrays to the GX shim so
                 * the indexed display-list replay (fn_800BD0FC -> GXCallDisplayList)
                 * decodes real positions, colours AND texcoords for this node.
                 * Without this the shim's g_vtxDescState stays unset and every
                 * replayed vertex collapses to the origin with degenerate (0,0)
                 * texcoords -- which is why textured nodes previously rendered as
                 * a flat material colour. The translated arrays live on the
                 * PObj's verts list (LE, host-resident); the list is GX_VA_NULL
                 * terminated. */
                if (translatedPObj.pobj.verts != NULL) {
                    HSD_VtxDescList* vtx = translatedPObj.pobj.verts;

                    GXClearVtxDesc();
                    while (vtx->attr != GX_VA_NULL) {
                        GXSetVtxDesc((GXAttr)vtx->attr, (GXAttrType)vtx->attr_type);
                        GXSetVtxAttrFmt(GX_VTXFMT0, (GXAttr)vtx->attr,
                                        (GXCompCnt)vtx->comp_cnt,
                                        (GXCompType)vtx->comp_type, vtx->frac);
                        GXSetArray((GXAttr)vtx->attr, vtx->vertex, (u8)vtx->stride);
                        ++vtx;
                    }
                }
                /* Skip the demo's full-screen fade/flash overlay: a material-
                 * only quad whose material alpha is ~0 (fully transparent).
                 * In-game it is an animated alpha fade that ends transparent;
                 * drawn opaque it would cover the whole title scene. Keying on
                 * alpha (not camera-space position) is camera-independent. */
                int debugFlatChar = 0;
                int skinIsolateSkip = 0;
                if (rootJoint == PCPORT_TITLE_SCENE_ROOT_JOINT &&
                    stats->dobjs == PCPORT_TITLE_SCENE_LOGO_DOBJ &&
                    getenv("PCPORT_TITLE_SHOW_ARCHIVE_LOGO") == NULL) {
                    /* Current host rendering only shows the archive Pokemon-logo
                     * billboard without the full Colosseum composition. Keep it
                     * suppressed by default; the old host 2D logo remains opt-in
                     * for diagnostics. */
                    isLogoTex = 1;
                }
                {
                    static int isoChars = -1;
                    static int isoLo = 6, isoHi = 9;
                    if (isoChars < 0) {
                        const char* e = getenv("PCPORT_ISOLATE_CHARS");
                        isoChars = (e != NULL) ? 1 : 0;
                        if (e != NULL && e[0] != '\0' && e[0] != '1') {
                            /* "lo-hi" range, e.g. "6-9" or single "7" */
                            int a = 0, b = 0;
                            if (sscanf(e, "%d-%d", &a, &b) == 2) { isoLo = a; isoHi = b; }
                            else if (sscanf(e, "%d", &a) == 1) { isoLo = isoHi = a; }
                        }
                    }
                    if (isoChars) {
                        if (stats->dobjs < (u32)isoLo || stats->dobjs > (u32)isoHi) {
                            isLogoTex = 1; /* skip everything outside the range */
                        } else if (getenv("PCPORT_ISOLATE_FLAT") != NULL) {
                            debugFlatChar = 1; /* draw bright + unlit */
                        }
                    }
                    /* Geometry-vs-texture probe: draw every scene mesh as an
                     * opaque solid (no texture, unlit). If the ruins APPEAR, the
                     * geometry/display-list replay is fine and the bug is in the
                     * textured (CMPR) path; if they stay invisible, it's geometry. */
                    if (getenv("PCPORT_RUINS_SOLID") != NULL && !isLogoTex) {
                        debugFlatChar = 1;
                    }
                }
                if (debugFlatChar) {
                    /* Distinct bright flat colour per dobj, lighting OFF, so the
                     * isolated mesh's silhouette + screen position is unambiguous. */
                    static const u32 kDbgColors[4] = {
                        0xFF3030FFu, 0x30FF30FFu, 0x3060FFFFu, 0xFFFF30FFu };
                    PCPortTranslatedMaterial dbgMat;
                    memset(&dbgMat, 0, sizeof(dbgMat));
                    dbgMat.alpha = 1.0f;
                    dbgMat.diffuse = kDbgColors[(stats->dobjs - (u32)6) & 3u];
                    dbgMat.ambient = dbgMat.diffuse;
                    ConfigureTranslatedMaterialPipeline((unsigned int)pipelineId, &dbgMat);
                    GXHostSetVertexAlphaScale(1.0f);
                    drawObject.pipelineId = (unsigned int)pipelineId;
                    fn_801AA568(&translatedPObj.pobj);
                    GXHostSetLightingEnabled(GX_FALSE);
                    fn_800DAD10((void*)&drawObject);
                    stats->drawn++;
                } else if (!((haveTexture == 0 && haveMaterial &&
                       translatedMaterial.alpha < 0.01f) ||
                      isLogoTex ||
                      battleControlPObj)) {
                    /* Enable directional lighting only when the material's
                     * rendermode has RENDER_DIFFUSE (bit 2 = 0x4) set, matching
                     * the GCN HSD material channel control.  Unlit surfaces
                     * (sky, clouds, prelit props) clear this bit and must render
                     * full-bright; applying lambert to them was the root cause of
                     * the dim/grey S1_out sky (Defect A). */
#define PCPORT_RENDER_DIFFUSE 0x04u /* hsd_mobj.h RENDER_DIFFUSE = 1<<2 */
                    fn_801AA568(&translatedPObj.pobj);
                    {
                        int wantLight = haveMaterial
                            ? ((translatedMaterial.rendermode & PCPORT_RENDER_DIFFUSE) != 0u)
                            : 0;
                        GXHostSetLightingEnabled(
                            (getenv("PCPORT_SCENE_NOLIGHT") != NULL || !wantLight)
                                ? GX_FALSE : GX_TRUE);
                    }
                    /* Debug: override the jobj-derived cull so backface-culled
                     * (wrong-winding) geometry can be ruled in/out. fn_801AA568
                     * sets cull from the jobj flags, so override AFTER it. */
                    if (getenv("PCPORT_NO_CULL") != NULL) {
                        GXSetCullMode(GX_CULL_NONE);
                    }
                    if (getenv("PCPORT_NO_ZTEST") != NULL) {
                        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
                    }
                    /* Per-pobj isolation: PCPORT_SKIN_ONLY=<n> renders only the
                     * nth pobj (1-based, matches the [rjt] pobj#N labels) so each
                     * mesh piece can be identified in isolation. Mismatched pobjs
                     * are skipped (not drawn) but the walk proceeds normally. */
                    {
                        const char* so = getenv("PCPORT_SKIN_ONLY");
                        skinIsolateSkip = (so != NULL && so[0] != '\0' &&
                                           (u32)atoi(so) != stats->dobjs);
                    }
                    /* Skinned (type-2 envelope) PObjs: CPU-skin + immediate-mode
                     * submit (PCPORT_SKIN). fn_800DAD10 would draw them rigidly
                     * (jumbled). Falls back to the rigid draw if skinning bails. */
                    if (skinIsolateSkip) {
                        /* isolation: suppress this mesh's draw */
                    } else if ((getenv("PCPORT_SKIN") != NULL || g_pcEnterFieldWalk ||
                        getenv("PCPORT_FIELD_WALK") != NULL ||
                        g_pcBattleRenderSkin) &&
                        ((translatedPObj.pobj.flags >> 12) & 3u) == 2u &&
                        RenderSkinnedPObj(a, pobjOffset, &translatedPObj, rootJoint, cam,
                                          haveTexture,
                                          haveTexture ? &nodeTextureObject : NULL,
                                          (u8)textureMapId,
                                          haveTexture
                                              ? (u8)translatedTextureExp.stages[0].texture.tevMode
                                              : (u8)GX_PASSCLR)) {
                        /* drawn by the skinned path */
                    } else {
                        fn_800DAD10((void*)&drawObject);
                    }
                    GXHostSetLightingEnabled(GX_FALSE);
                    stats->drawn++;
                } else {
                    stats->skipped++;
                }

                if (haveTexture) {
                    /* Reset the texture matrix to identity so any UV-scroll
                     * applied for this TObj does not bleed into the next draw. */
                    {
                        u32 coordSlot = (u32)translatedTextureExp.stages[0].texture.coordId;
                        if (coordSlot >= 8u) coordSlot = 0u;
                        GXHostSetTexMatrix(coordSlot, NULL);
                    }
                    GXHostClearTextureBinding();
                    GSgfxHostClearPipelineState(PCPORT_REAL_TEXTURED_PIPELINE);
                }

                {
                    static int rjtDbg = -1;
                    if (rjtDbg < 0) {
                        rjtDbg = (getenv("PCPORT_RENDER_DEBUG") != NULL) ? 1 : 0;
                    }
                    if (rjtDbg && stats->dobjs <= 44u) {
                        const PCPortTranslatedTexture* bt =
                            haveTexture ? &translatedTextureExp.stages[0].texture : NULL;
                        u32 posType = 0, posCnt = 0, posAttrType = 0, posStride = 0;
                        if (translatedPObj.pobj.verts != NULL) {
                            HSD_VtxDescList* v = translatedPObj.pobj.verts;
                            while (v->attr != GX_VA_NULL) {
                                if (v->attr == GX_VA_POS) {
                                    posCnt = v->comp_cnt; posType = v->comp_type;
                                    posAttrType = v->attr_type; posStride = v->stride;
                                    break;
                                }
                                ++v;
                            }
                        }
                        {
                            /* Per-axis scale = length of each model-matrix basis row. */
                            f32 (*M)[4] = translatedJoint.modelMatrix;
                            f32 sx = sqrtf(M[0][0]*M[0][0]+M[0][1]*M[0][1]+M[0][2]*M[0][2]);
                            f32 sy = sqrtf(M[1][0]*M[1][0]+M[1][1]*M[1][1]+M[1][2]*M[1][2]);
                            f32 sz = sqrtf(M[2][0]*M[2][0]+M[2][1]*M[2][1]+M[2][2]*M[2][2]);
                            printf("[rjt] pobj#%u %s verts=%u fmt=%u POS{attrType=%u stride=%u} bbox=[%.0f,%.0f,%.0f..%.0f,%.0f,%.0f] jointScale=(%.3f,%.3f,%.3f) cam=(%.0f,%.0f,%.0f)\n",
                               stats->dobjs, haveTexture ? "TEX" : "MAT",
                               drawObject.totalVerts,
                               bt ? (unsigned)bt->format : 0u,
                               posAttrType, posStride,
                               translatedPObj.minPosition[0], translatedPObj.minPosition[1],
                               translatedPObj.minPosition[2], translatedPObj.maxPosition[0],
                               translatedPObj.maxPosition[1], translatedPObj.maxPosition[2],
                               sx, sy, sz,
                               modelViewMatrix[0][3], modelViewMatrix[1][3],
                               modelViewMatrix[2][3]);
                        }
                        (void)posCnt; (void)posType;
                        if (getenv("PCPORT_VTXDESC") != NULL &&
                            translatedPObj.pobj.verts != NULL) {
                            HSD_VtxDescList* v = translatedPObj.pobj.verts;
                            printf("   [vtxdesc pobj#%u]", stats->dobjs);
                            while (v->attr != GX_VA_NULL) {
                                printf(" attr=%u(type=%u cnt=%u comp=%u frac=%u stride=%u)",
                                       v->attr, v->attr_type, v->comp_cnt,
                                       v->comp_type, v->frac, v->stride);
                                ++v;
                            }
                            printf("\n");
                        }
                    }
                }

                {
                    static int skinDbg = -1;
                    if (skinDbg < 0) {
                        skinDbg = (getenv("PCPORT_SKIN_POSE_DEBUG") != NULL) ? 1 : 0;
                    }
                    if (skinDbg && stats->dobjs <= 40u) {
                        u16 pflags = translatedPObj.pobj.flags;
                        u32 ptype = (u32)((pflags >> 12) & 3u);
                        u32 uField = PCPort_ReadBigEndianU32(a->storage + pobjOffset + 0x14);
                        /* dobj joint world-space origin (bind) */
                        f32 djx = translatedJoint.modelMatrix[0][3];
                        f32 djy = translatedJoint.modelMatrix[1][3];
                        f32 djz = translatedJoint.modelMatrix[2][3];
                        printf("[skinpose] pobj#%u flags=0x%04X type=%u verts=%u "
                               "uField=0x%X dobjJoint@0x%X bind=(%.1f,%.1f,%.1f) "
                               "aabbMin=(%.1f,%.1f,%.1f) aabbMax=(%.1f,%.1f,%.1f)",
                               stats->dobjs, pflags, ptype,
                               translatedPObj.totalSubmittedVertices,
                               uField, joint, djx, djy, djz,
                               translatedPObj.minPosition[0],
                               translatedPObj.minPosition[1],
                               translatedPObj.minPosition[2],
                               translatedPObj.maxPosition[0],
                               translatedPObj.maxPosition[1],
                               translatedPObj.maxPosition[2]);
                        printf(" mat=%d diff=0x%08X amb=0x%08X alpha=%.2f tex=%d texId=0x%X rmode=0x%X",
                               haveMaterial, translatedMaterial.diffuse,
                               translatedMaterial.ambient, translatedMaterial.alpha,
                               haveTexture, textureMapId, translatedMaterial.rendermode);
                        if (uField != 0u &&
                            ArchiveRangeValid(a, uField, PCPORT_SERIALIZED_JOINT_SIZE)) {
                            PCPortTranslatedJointTransform skinJoint;
                            memset(&skinJoint, 0, sizeof(skinJoint));
                            if (PCPort_TranslateJointChainToMatrixBE(a, rootJoint,
                                                                     uField, &skinJoint)) {
                                printf(" skinJoint@0x%X bind=(%.1f,%.1f,%.1f)",
                                       uField,
                                       skinJoint.modelMatrix[0][3],
                                       skinJoint.modelMatrix[1][3],
                                       skinJoint.modelMatrix[2][3]);
                            } else {
                                printf(" skinJoint@0x%X (chain-resolve FAILED)", uField);
                            }
                        }
                        /* PCPORT_ENV_DUMP: for type-2 (envelope) PObjs, uField is
                         * the envelope-array pointer (one entry per matrix slot).
                         * Dump it + the first entries it points to, to decode the
                         * slot -> {jobj, weight} skinning palette. */
                        if (getenv("PCPORT_ENV_DUMP") != NULL && ptype == 2u &&
                            uField != 0u && ArchiveRangeValid(a, uField, 0x20u)) {
                            u32 ei;
                            printf("\n[env] pobj#%u envelope@0x%X:", stats->dobjs, uField);
                            for (ei = 0; ei < 0x20u; ei += 4u) {
                                u32 v = PCPort_ReadBigEndianU32(a->storage + uField + ei);
                                int isp = ArchiveRangeValid(a, v, 0x4u);
                                printf(" +%X=0x%X%s", ei, v, isp ? "*" : "");
                            }
                            /* follow slot 0's entry pointer (a {jobj,weight} list) */
                            {
                                u32 e0 = PCPort_ReadBigEndianU32(a->storage + uField + 0x0);
                                if (ArchiveRangeValid(a, e0, 0x10u)) {
                                    union { u32 u; f32 f; } w;
                                    u32 jb = PCPort_ReadBigEndianU32(a->storage + e0 + 0x0);
                                    w.u = PCPort_ReadBigEndianU32(a->storage + e0 + 0x4);
                                    printf("  slot0@0x%X: jobj=0x%X weight=%.3f", e0, jb, (double)w.f);
                                }
                            }
                        }
                        printf("\n");
                    }
                }
            } else {
                if (getenv("PCPORT_SKIP_DBG") != NULL) {
                    u32 uF = PCPort_ReadBigEndianU32(a->storage + pobjOffset + 0x14);
                    PCPortTranslatedPObj tpp; PCPortTranslatedJointTransform jtp;
                    int pOk, jOk;
                    memset(&tpp, 0, sizeof(tpp)); memset(&jtp, 0, sizeof(jtp));
                    pOk = PCPort_TranslatePObjFromArchiveBE(a, pobjOffset, &tpp);
                    jOk = PCPort_TranslateJointChainToMatrixBE(a, rootJoint, joint, &jtp);
                    fprintf(stderr, "[skip] pobj@0x%X SKIPPED joint@0x%X uField=0x%X "
                            "translatePObj=%d jointChain=%d verts=%u\n",
                            pobjOffset, joint, uF, pOk, jOk, tpp.totalSubmittedVertices);
                    PCPort_DestroyTranslatedPObj(&tpp);
                }
                stats->skipped++;
            }

            PCPort_FreeBuffer(bakedPixels);
            PCPort_DestroyTranslatedPObj(&translatedPObj);

            /* advance to the next PObj in this DObj's chain (+0x04) */
            {
                u32 pn = PCPort_ReadBigEndianU32(a->storage + pobjOffset + 0x04);
                if (pn == pobjOffset) break;
                pobjOffset = pn;
            }
        }

        /* Serialized HSD_DObjDesc layout: +0x00 class_name, +0x04 next,
         * +0x08 mobj, +0x0C pobj (confirmed src/hsd/hsd_dobj.c:493 reads the
         * next subdesc from +4, and the mobj/pobj reads above use +0x08/+0x0C).
         * Reading "next" from +0x00 (class_name) only ever yielded the first
         * dobj of each joint's chain -- the title's character body meshes are
         * later dobjs in those chains, so the host saw 22/43 dobjs and the
         * characters never drew. */
        nextOffset = PCPort_ReadBigEndianU32(a->storage + dobjOffset + 0x04);
        if (nextOffset == dobjOffset) {
            break;
        }
        dobjOffset = nextOffset;
    }

    childOffset = PCPort_ReadBigEndianU32(a->storage + joint + 0x08);
    if (childOffset != 0u && childOffset != joint) {
        RenderJointTree(a, rootJoint, childOffset, cam, pipelineId, stats);
    }

    nextOffset = PCPort_ReadBigEndianU32(a->storage + joint + 0x0C);
    if (nextOffset == 0u || nextOffset == joint) {
        g_rjtDepth--;
        return;
    }
    joint = nextOffset;   /* tail-iterate to the next sibling */
    }
}

/* World->camera 3x4 view matrix (GameCube convention: camera looks down -Z)
 * from eye/interest/up, used to override a scene camera with a known pose. */
static void BuildViewMatrixLookAt(const f32 eye[3], const f32 interest[3],
                                  const f32 up[3], f32 outView[3][4]) {
    f32 zx = eye[0] - interest[0];
    f32 zy = eye[1] - interest[1];
    f32 zz = eye[2] - interest[2];
    f32 zl = (f32)sqrt((double)((zx * zx) + (zy * zy) + (zz * zz)));
    f32 xx, xy, xz, xl, yx, yy, yz;

    if (zl < 1e-6f) { zl = 1.0f; }
    zx /= zl; zy /= zl; zz /= zl;

    xx = (up[1] * zz) - (up[2] * zy);
    xy = (up[2] * zx) - (up[0] * zz);
    xz = (up[0] * zy) - (up[1] * zx);
    xl = (f32)sqrt((double)((xx * xx) + (xy * xy) + (xz * xz)));
    if (xl < 1e-6f) { xl = 1.0f; }
    xx /= xl; xy /= xl; xz /= xl;

    yx = (zy * xz) - (zz * xy);
    yy = (zz * xx) - (zx * xz);
    yz = (zx * xy) - (zy * xx);

    outView[0][0] = xx; outView[0][1] = xy; outView[0][2] = xz;
    outView[0][3] = -((xx * eye[0]) + (xy * eye[1]) + (xz * eye[2]));
    outView[1][0] = yx; outView[1][1] = yy; outView[1][2] = yz;
    outView[1][3] = -((yx * eye[0]) + (yy * eye[1]) + (yz * eye[2]));
    outView[2][0] = zx; outView[2][1] = zy; outView[2][2] = zz;
    outView[2][3] = -((zx * eye[0]) + (zy * eye[1]) + (zz * eye[2]));
}

/*
 * Host front-end states. The seed of the RunGame() state machine: the title
 * screen reacts to START by advancing to the main-menu panel. More states
 * (save prompt, mode select) plug into this same enum + present loop.
 */
typedef enum PCPortSceneState {
    PCPORT_SCENE_TITLE = 0,
    PCPORT_SCENE_MAIN_MENU = 1,
    PCPORT_SCENE_DIALOG = 2,
    PCPORT_SCENE_SAVE_PROMPT = 3
} PCPortSceneState;

/* Draw a modal message box (dark teal panel + lighter border, white text) at the
 * bottom centre, matching the real game's dialog style. Dims the screen behind.
 * yesNo!=0 draws YES/NO with the selected option highlighted; else an [A] OK hint.
 * Call BeginMenuOverlay() + EnsureFontAtlas() first. */
static void DrawDialogBox(const char* text, int yesNo, int cursor) {
    DrawSolidScreenRect(0.0f, 0.0f, 640.0f, 480.0f, 0, 0, 0, 110);
    DrawSolidScreenRect(72.0f, 300.0f, 496.0f, 138.0f, 96, 124, 142, 250);
    DrawSolidScreenRect(78.0f, 306.0f, 484.0f, 126.0f, 24, 42, 56, 252);
    DrawTextWrapped(98.0f, 320.0f, 11.0f, 17.0f, 36, 3, 228, 236, 244, 255, text);
    if (yesNo) {
        if (cursor == 0) {
            DrawSolidScreenRect(196.0f, 398.0f, 70.0f, 28.0f, 250, 206, 92, 255);
            DrawTextScreen(212.0f, 402.0f, 14.0f, 20.0f, 28, 38, 52, 255, "YES");
            DrawTextScreen(372.0f, 402.0f, 14.0f, 20.0f, 200, 212, 224, 255, "NO");
        } else {
            DrawSolidScreenRect(360.0f, 398.0f, 56.0f, 28.0f, 250, 206, 92, 255);
            DrawTextScreen(212.0f, 402.0f, 14.0f, 20.0f, 200, 212, 224, 255, "YES");
            DrawTextScreen(372.0f, 402.0f, 14.0f, 20.0f, 28, 38, 52, 255, "NO");
        }
    } else {
        DrawTextScreen(296.0f, 404.0f, 10.0f, 15.0f, 170, 185, 205, 255, "[A] OK");
    }
}

typedef struct PCPortMessageBoxState {
    const char** pages;
    int pageCount;
    int pageIndex;
    int visibleChars;
    int charsPerFrame;
    int active;
    int fastForwardCount;
    int advanceCount;
    int closeCount;
} PCPortMessageBoxState;

#define PCPORT_MSGBOX_CONTEXT_MAGIC 0x4D534358u /* "MSCX" */
#define PCPORT_MSGBOX_STATE_SIZE 0x2A0u
#define PCPORT_MSGBOX_SLOT_SIZE 0x138u
#define PCPORT_MSGBOX_SLOT_BASE 0x08u
#define PCPORT_MSGBOX_ACTIVE_SLOT_OFF 0x278u
#define PCPORT_MSGBOX_X_OFF 0x27Cu
#define PCPORT_MSGBOX_Y_OFF 0x280u

typedef struct PCPortScriptMsgContext {
    u32 magic;
    u32 speakerId;
    u32 pageCount;
    u32 pageIndex;
    u32 visibleChars;
    u32 active;
    u32 storyStep;
    u32 cutsceneState;
    char preview[96];
} PCPortScriptMsgContext;

extern u8 lbl_803A9768[];
extern void fn_80056C54(u8* out, u8* src, u32 slot);
extern void* fn_80057270(void);
extern u8 lbl_803A95E8[];
extern u8 lbl_803A9720[];
extern void fn_80053778(void);

u8 lbl_803A9768[PCPORT_MSGBOX_STATE_SIZE];
u8 lbl_803A95E8[0x138];
u8 lbl_803A9720[0x48];

void fn_80056C54(u8* out, u8* src, u32 slot) {
    u32 activeSlot = slot % 2u;
    u8* dst = lbl_803A9768 + PCPORT_MSGBOX_SLOT_BASE +
        activeSlot * PCPORT_MSGBOX_SLOT_SIZE;

    if (src != NULL) {
        memcpy(dst, src, sizeof(PCPortScriptMsgContext));
    }
    if (out != NULL) {
        memcpy(out, dst, sizeof(PCPortScriptMsgContext));
    }
    *(u32*)(lbl_803A9768 + PCPORT_MSGBOX_ACTIVE_SLOT_OFF) = activeSlot;
}

void* fn_80057270(void) {
    u32 activeSlot = *(u32*)(lbl_803A9768 + PCPORT_MSGBOX_ACTIVE_SLOT_OFF) % 2u;
    return lbl_803A9768 + PCPORT_MSGBOX_SLOT_BASE +
        activeSlot * PCPORT_MSGBOX_SLOT_SIZE;
}

void fn_80053778(void) {
    void* msgCtx = fn_80057270();

    *(void**)lbl_803A95E8 = msgCtx;
    *(u32*)(lbl_803A95E8 + 8) = 1u;
    *(void**)lbl_803A9720 = msgCtx;
    *(u32*)(lbl_803A9720 + 8) = 1u;
}

static int PCPort_TextLen(const char* text) {
    return text != NULL ? (int)strlen(text) : 0;
}

static void PCPort_CopyTextPrefix(char* dst, size_t dstCap,
                                  const char* text, int visibleChars) {
    int i;
    if (dst == NULL || dstCap == 0u) {
        return;
    }
    dst[0] = '\0';
    if (text == NULL || visibleChars <= 0) {
        return;
    }
    for (i = 0; text[i] != '\0' && i < visibleChars &&
                (size_t)i + 1u < dstCap; ++i) {
        dst[i] = text[i];
    }
    dst[i] = '\0';
}

static void PCPort_MessageBoxInit(PCPortMessageBoxState* msg,
                                  const char** pages, int pageCount,
                                  int charsPerFrame) {
    if (msg == NULL) {
        return;
    }
    memset(msg, 0, sizeof(*msg));
    msg->pages = pages;
    msg->pageCount = pageCount;
    msg->charsPerFrame = charsPerFrame > 0 ? charsPerFrame : 4;
    msg->active = (pages != NULL && pageCount > 0);
}

static PCPortScriptMsgContext* PCPort_MessageBoxScriptContext(void) {
    return (PCPortScriptMsgContext*)fn_80057270();
}

static void PCPort_MessageBoxSeedScriptContext(
    const PCPortMessageBoxState* msg,
    u32 speakerId,
    const char* preview) {
    PCPortScriptMsgContext seed;

    memset(lbl_803A9768, 0, PCPORT_MSGBOX_STATE_SIZE);
    *(u32*)(lbl_803A9768 + PCPORT_MSGBOX_ACTIVE_SLOT_OFF) = 0u;
    *(f32*)(lbl_803A9768 + PCPORT_MSGBOX_X_OFF) = 58.0f;
    *(f32*)(lbl_803A9768 + PCPORT_MSGBOX_Y_OFF) = 276.0f;

    memset(&seed, 0, sizeof(seed));
    seed.magic = PCPORT_MSGBOX_CONTEXT_MAGIC;
    seed.speakerId = speakerId;
    seed.pageCount = msg != NULL ? (u32)msg->pageCount : 0u;
    seed.active = (msg != NULL && msg->active) ? 1u : 0u;
    if (preview != NULL) {
        snprintf(seed.preview, sizeof(seed.preview), "%s", preview);
    }
    fn_80056C54(NULL, (u8*)&seed, 0u);
}

static void PCPort_MessageBoxSyncScriptContext(
    const PCPortMessageBoxState* msg,
    u32 storyStep,
    u32 cutsceneState) {
    PCPortScriptMsgContext* ctx = PCPort_MessageBoxScriptContext();
    if (ctx == NULL || ctx->magic != PCPORT_MSGBOX_CONTEXT_MAGIC) {
        return;
    }
    ctx->pageIndex = msg != NULL ? (u32)msg->pageIndex : 0u;
    ctx->visibleChars = msg != NULL ? (u32)msg->visibleChars : 0u;
    ctx->active = (msg != NULL && msg->active) ? 1u : 0u;
    ctx->storyStep = storyStep;
    ctx->cutsceneState = cutsceneState;
}

static int PCPort_MessageBoxPageLen(const PCPortMessageBoxState* msg) {
    if (msg == NULL || !msg->active || msg->pages == NULL ||
        msg->pageIndex < 0 || msg->pageIndex >= msg->pageCount) {
        return 0;
    }
    return PCPort_TextLen(msg->pages[msg->pageIndex]);
}

static void PCPort_MessageBoxTick(PCPortMessageBoxState* msg, u16 pressed) {
    int pageLen;
    if (msg == NULL || !msg->active) {
        return;
    }
    pageLen = PCPort_MessageBoxPageLen(msg);
    if (pressed & (GCN_PAD_BUTTON_A | GCN_PAD_BUTTON_START)) {
        if (msg->visibleChars < pageLen) {
            msg->visibleChars = pageLen;
            msg->fastForwardCount++;
            return;
        }
        if (msg->pageIndex + 1 < msg->pageCount) {
            msg->pageIndex++;
            msg->visibleChars = 0;
            msg->advanceCount++;
            return;
        }
        msg->active = 0;
        msg->closeCount++;
        return;
    }
    if (pressed & GCN_PAD_BUTTON_B) {
        msg->active = 0;
        msg->closeCount++;
        return;
    }
    if (msg->visibleChars < pageLen) {
        msg->visibleChars += msg->charsPerFrame;
        if (msg->visibleChars > pageLen) {
            msg->visibleChars = pageLen;
        }
    }
}

static void DrawFieldMessageBox(const PCPortMessageBoxState* msg,
                                const char* speaker) {
    char visible[256];
    char hint[64];
    int pageLen;

    if (msg == NULL || !msg->active) {
        return;
    }
    pageLen = PCPort_MessageBoxPageLen(msg);
    PCPort_CopyTextPrefix(visible, sizeof(visible),
                          msg->pages[msg->pageIndex],
                          msg->visibleChars);

    BeginMenuOverlay();
    EnsureFontAtlas();
    DrawSolidScreenRect(34.0f, 292.0f, 572.0f, 156.0f, 94, 122, 142, 245);
    DrawSolidScreenRect(42.0f, 300.0f, 556.0f, 140.0f, 20, 38, 54, 252);
    if (speaker != NULL && speaker[0] != '\0') {
        DrawSolidScreenRect(58.0f, 276.0f, 118.0f, 28.0f, 230, 210, 124, 250);
        DrawTextScreen(70.0f, 281.0f, 9.0f, 15.0f,
                       24, 36, 48, 255, speaker);
    }
    DrawTextWrapped(66.0f, 320.0f, 10.0f, 16.0f, 48, 4,
                    232, 240, 248, 255, visible);

    if (msg->visibleChars >= pageLen) {
        snprintf(hint, sizeof(hint), "[A] %s %d/%d",
                 (msg->pageIndex + 1 < msg->pageCount) ? "NEXT" : "OK",
                 msg->pageIndex + 1,
                 msg->pageCount);
        DrawTextScreen(448.0f, 414.0f, 8.0f, 13.0f,
                       174, 194, 214, 255, hint);
    } else {
        DrawTextScreen(548.0f, 414.0f, 8.0f, 13.0f,
                       174, 194, 214, 255, "...");
    }
}

typedef struct PCPortFieldStartMenuState {
    int active;
    int cursor;
    int openCount;
    int closeCount;
    int moveCount;
    int selectCount;
} PCPortFieldStartMenuState;

static const char* kFieldStartMenuItems[] = {
    "POKEMON",
    "ITEMS",
    "PDA",
    "SAVE",
    "OPTIONS"
};
#define PCPORT_FIELD_START_MENU_COUNT \
    ((int)(sizeof(kFieldStartMenuItems) / sizeof(kFieldStartMenuItems[0])))

static void PCPort_FieldStartMenuInit(PCPortFieldStartMenuState* menu) {
    if (menu != NULL) {
        memset(menu, 0, sizeof(*menu));
    }
}

static void PCPort_FieldStartMenuTick(PCPortFieldStartMenuState* menu,
                                      u16 pressed) {
    if (menu == NULL) {
        return;
    }
    if (!menu->active) {
        if (pressed & GCN_PAD_BUTTON_START) {
            menu->active = 1;
            menu->cursor = 0;
            menu->openCount++;
        }
        return;
    }

    if (pressed & GCN_PAD_BUTTON_DOWN) {
        menu->cursor = (menu->cursor + 1) % PCPORT_FIELD_START_MENU_COUNT;
        menu->moveCount++;
    }
    if (pressed & GCN_PAD_BUTTON_UP) {
        menu->cursor = (menu->cursor + PCPORT_FIELD_START_MENU_COUNT - 1) %
                       PCPORT_FIELD_START_MENU_COUNT;
        menu->moveCount++;
    }
    if (pressed & GCN_PAD_BUTTON_A) {
        menu->selectCount++;
    }
    if (pressed & (GCN_PAD_BUTTON_START | GCN_PAD_BUTTON_B)) {
        menu->active = 0;
        menu->closeCount++;
    }
}

static void DrawFieldStartMenuOverlay(const PCPortFieldStartMenuState* menu) {
    int i;
    if (menu == NULL || !menu->active) {
        return;
    }

    BeginMenuOverlay();
    EnsureFontAtlas();
    DrawSolidScreenRect(0.0f, 0.0f, 640.0f, 480.0f, 0, 0, 0, 72);
    DrawSolidScreenRect(404.0f, 56.0f, 190.0f, 286.0f, 96, 124, 142, 246);
    DrawSolidScreenRect(412.0f, 64.0f, 174.0f, 270.0f, 22, 40, 56, 252);
    DrawTextScreen(438.0f, 84.0f, 11.0f, 17.0f,
                   238, 242, 246, 255, "FIELD MENU");

    for (i = 0; i < PCPORT_FIELD_START_MENU_COUNT; ++i) {
        f32 y = 126.0f + (f32)i * 34.0f;
        if (i == menu->cursor) {
            DrawSolidScreenRect(428.0f, y - 5.0f, 138.0f, 26.0f,
                                244, 208, 92, 255);
            DrawTextScreen(444.0f, y, 9.0f, 15.0f,
                           28, 38, 52, 255, kFieldStartMenuItems[i]);
        } else {
            DrawTextScreen(444.0f, y, 9.0f, 15.0f,
                           206, 220, 232, 255, kFieldStartMenuItems[i]);
        }
    }
    DrawTextScreen(424.0f, 308.0f, 7.0f, 11.0f,
                   162, 184, 204, 255, "START/B CLOSE");
}

/* What a dialog's confirm ("Yes" / A on an info box) does. */
#define PCPORT_DLG_INFO      0   /* info only: A or B dismisses back to the menu */
#define PCPORT_DLG_QUIT      1   /* Yes -> close the window */
#define PCPORT_DLG_CONTINUE  2   /* Yes -> load saved game (not yet implemented) */
#define PCPORT_DLG_NEWGAME   3   /* Yes -> start a new game (not yet implemented) */

/* Host-side save-data presence check. The GC save / memory-card subsystem has
 * no decompiled C (a black box), so save state is reimplemented host-side. For
 * now this only tests whether a save blob exists at PCPORT_SAVE_PATH (env-
 * overridable) -- no GCI container or SHA-1; matches the game's own "no save
 * data" fallback when absent. */
#define PCPORT_SAVE_PATH_DEFAULT "build_pc/colosseum.sav"
static int PCPort_SaveExists(void) {
    const char* path = getenv("PCPORT_SAVE_PATH");
    FILE* f;

    if (path == NULL || path[0] == '\0') {
        path = PCPORT_SAVE_PATH_DEFAULT;
    }
    f = fopen(path, "rb");
    if (f != NULL) {
        fclose(f);
        return 1;
    }
    return 0;
}

/* Main-menu selectable items, in D-pad up/down traversal order. handX/handY is
 * the top-left screen position (640x480) of the pointing-hand cursor sprite for
 * that item. Positions are tuned against the menu_033 card layout (STORY card
 * left, BATTLE card right, OPTIONS + Quit buttons below). */
typedef struct PCPortMenuItem {
    f32 handX;
    f32 handY;
    const char* label;
    const char* desc;
} PCPortMenuItem;

static const PCPortMenuItem kMainMenuItems[] = {
    {  74.0f, 214.0f, "CONTINUE",         "Continue the Story Mode from where it was last saved." },
    {  74.0f, 250.0f, "NEW GAME",         "Start a new Story Mode adventure from the beginning." },
    { 372.0f, 214.0f, "COLOSSEUM BATTLE", "Take on the Colosseum tournaments and challenges." },
    { 372.0f, 250.0f, "BATTLE NOW",       "Set up a quick custom battle with your own rules." },
    { 312.0f, 326.0f, "OPTIONS",          "Adjust game settings such as rumble and sound." },
    { 506.0f, 328.0f, "QUIT",             "Quit the game and return to the title screen." }
};
#define PCPORT_MENU_ITEM_COUNT ((int)(sizeof(kMainMenuItems) / sizeof(kMainMenuItems[0])))

/* Show a single static boot logo (a raw 0x80-header fsys sprite) full-screen.
 * Holds for `seconds` (paced via glfwGetTime), skippable on START/A. Returns 0 if
 * the window was closed (abort the whole sequence), else 1. With dumpFrame>=0 it
 * renders one frame, dumps, and returns (headless verification). prev carries the
 * shared input edge-state across boot items. */
static int BootShowLogo(GLFWwindow* window, const char* archive, const char* member,
                        double seconds, int dumpFrame, u16* prev) {
    GXTexObj tex;
    PADStatus pads[4];
    double start;

    memset(&tex, 0, sizeof(tex));
    memset(pads, 0, sizeof(pads));
    if (!LoadFsysSpriteTexObj(archive, member, &tex)) {
        fprintf(stderr, "[boot] logo load failed: %s:%s (skipping)\n", archive, member);
        return 1;
    }
    printf("[boot] logo %s:%s\n", archive, member);

    if (dumpFrame >= 0) {
        ClearBackbuffer(0.0f, 0.0f, 0.0f);
        GSgfx_BeginFrame();
        BeginMenuOverlay();
        DrawTexturedScreenRect(&tex, 0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 0.0f, 1.0f, 1.0f);
        {
            unsigned char* px = ReadBackbufferImage();
            free(px);
        }
        GSgfxSwapBuffers(1);
        return 1;
    }

    start = glfwGetTime();
    for (;;) {
        u16 held;
        u16 pressed;

        if (window != NULL && glfwWindowShouldClose(window)) {
            return 0;
        }
        VIWaitForRetrace_PC();
        PADRead(pads);
        held = pads[0].button;
        pressed = (u16)(held & ~(*prev));
        *prev = held;
        if (pressed & (GCN_PAD_BUTTON_START | GCN_PAD_BUTTON_A)) {
            break;
        }
        {
            /* Fade the logo IN over the first fadeDur, hold, then fade OUT over the
             * last fadeDur (over black) -- the real boot logos fade both ways. */
            double t = glfwGetTime() - start;
            double fadeDur = 0.45;
            double aIn  = t / fadeDur;
            double aOut = (seconds - t) / fadeDur;
            double a;
            u8 ai;
            if (t >= seconds) {
                break;
            }
            if (aIn  > 1.0) aIn  = 1.0;
            if (aOut > 1.0) aOut = 1.0;
            if (aOut < 0.0) aOut = 0.0;
            a = (aIn < aOut) ? aIn : aOut;
            ai = (u8)(a * 255.0);
            ClearBackbuffer(0.0f, 0.0f, 0.0f);
            GSgfx_BeginFrame();
            BeginMenuOverlay();
            DrawTexturedScreenRectA(&tex, 0.0f, 0.0f, 640.0f, 480.0f,
                                    0.0f, 0.0f, 1.0f, 1.0f, ai, ai);
            GSgfxSwapBuffers(1);
        }
    }
    return 1;
}

static void BootDrainAudio(void) {
    double start = glfwGetTime();
    while (WaveOutSink_IsPlaying() && (glfwGetTime() - start) < 1.0) {
        VIWaitForRetrace_PC();
    }
}

/* Play one THP movie full-screen, decoded by thp_player and presented via the 2D
 * quad path. Paced to the movie's fps via glfwGetTime (frames held between vsyncs);
 * START/A skips, window close returns 0. dumpFrame>=0 decodes to that frame, dumps,
 * and returns (headless verification). */
static int BootPlayTHP(GLFWwindow* window, const char* path, int dumpFrame, u16* prev) {
    PCPortTHP* thp = PCPortTHP_Open(path);
    GXTexObj frameTex;
    const unsigned char* rgba = NULL;
    PADStatus pads[4];
    int vw;
    int vh;
    int total;
    int decoded = 0;
    float fps;
    double startTime;
    int audioOn = 0;

    if (thp == NULL) {
        fprintf(stderr, "[boot] cannot open %s (skipping)\n", path);
        return 1;
    }
    memset(&frameTex, 0, sizeof(frameTex));
    memset(pads, 0, sizeof(pads));
    vw = PCPortTHP_Width(thp);
    vh = PCPortTHP_Height(thp);
    total = PCPortTHP_FrameCount(thp);
    fps = PCPortTHP_Fps(thp);
    if (fps <= 0.0f) {
        fps = 29.97f;
    }
    printf("[boot] playing %s (%dx%d, %d frames, %.2f fps)\n", path, vw, vh, total, fps);

    if (PCPortTHP_HasAudio(thp)) {
        audioOn = WaveOutSink_Open(PCPortTHP_AudioSampleRate(thp),
                                   PCPortTHP_AudioChannels(thp), 16);
        printf("[boot] audio %s (%u Hz, %d ch)\n",
               audioOn ? "enabled" : "unavailable",
               PCPortTHP_AudioSampleRate(thp),
               PCPortTHP_AudioChannels(thp));
    }

    if (dumpFrame >= 0) {
        int f;
        for (f = 0; f <= dumpFrame; ++f) {
            if (!PCPortTHP_NextFrameRGBA(thp, &rgba)) {
                break;
            }
        }
        if (rgba != NULL) {
            GXHostUpdateTexObjRGBA8(&frameTex, rgba, (u16)vw, (u16)vh);
            ClearBackbuffer(0.0f, 0.0f, 0.0f);
            GSgfx_BeginFrame();
            BeginMenuOverlay();
            DrawTexturedScreenRect(&frameTex, 0.0f, 0.0f, 640.0f, 480.0f,
                                   0.0f, 0.0f, 1.0f, 1.0f);
            {
                unsigned char* px = ReadBackbufferImage();
                free(px);
            }
            GSgfxSwapBuffers(1);
        }
        if (audioOn) WaveOutSink_Close();
        PCPortTHP_Close(thp);
        return 1;
    }

    startTime = glfwGetTime();
    for (;;) {
        double elapsed;
        int wantFrame;
        u16 held;
        u16 pressed;

        if (window != NULL && glfwWindowShouldClose(window)) {
            if (audioOn) WaveOutSink_Close();
            PCPortTHP_Close(thp);
            return 0;
        }
        VIWaitForRetrace_PC();
        PADRead(pads);
        held = pads[0].button;
        pressed = (u16)(held & ~(*prev));
        *prev = held;
        if (pressed & (GCN_PAD_BUTTON_START | GCN_PAD_BUTTON_A)) {
            break;
        }
        elapsed = glfwGetTime() - startTime;
        wantFrame = (int)(elapsed * fps);
        while (decoded <= wantFrame) {
            if (!PCPortTHP_NextFrameRGBA(thp, &rgba)) {
                rgba = NULL;
                break;
            }
            GXHostUpdateTexObjRGBA8(&frameTex, rgba, (u16)vw, (u16)vh);
            ++decoded;
            if (audioOn) {
                const short* pcm = NULL;
                unsigned int nfr = 0;
                if (PCPortTHP_NextFrameAudioPCM(thp, &pcm, &nfr) && pcm && nfr) {
                    WaveOutSink_Submit(pcm, nfr);
                }
            }
        }
        if (rgba == NULL && decoded >= total) {
            break;
        }
        ClearBackbuffer(0.0f, 0.0f, 0.0f);
        GSgfx_BeginFrame();
        if (decoded > 0) {
            BeginMenuOverlay();
            DrawTexturedScreenRect(&frameTex, 0.0f, 0.0f, 640.0f, 480.0f,
                                   0.0f, 0.0f, 1.0f, 1.0f);
        }
        GSgfxSwapBuffers(1);
    }
    if (audioOn) {
        BootDrainAudio();
        WaveOutSink_Close();
    }
    PCPortTHP_Close(thp);
    return 1;
}

/*
 * Boot sequence before the title: Nintendo logo (static) -> The Pokemon Company
 * (tpc.thp) -> Genius Sonority (gs_logo.thp) -> opening demo (openingdemo.thp).
 * Static logos come from the *_logo.fsys CMPR sprites; the rest are THP movies.
 * START/A skips the current item; window close aborts. PCPORT_NO_BOOT=1 skips the
 * whole sequence; PCPORT_BOOT_DUMP_FRAME=N dumps one item (GL path) for
 * headless verification, with PCPORT_BOOT_DUMP_ITEM selecting the boot item
 * index (0=Nintendo, 1=TPC, 2=Genius Sonority). Must run after GSgfxInit.
 */
static int RunBootSequence(GLFWwindow* window) {
    static const struct {
        int isThp;
        const char* archive;
        const char* member;
    } kBoot[] = {
        /* Boot logos only. The opening-demo movie is NOT part of boot -- on the real
         * game it plays as an attract loop AFTER idling on the title (see the title
         * idle handler in RunMenuScene), not before the title. */
        { 0, "orig/GC6E01/disc/files/nintendo_logo.fsys", "logo_nintendo" },
        { 1, "orig/GC6E01/disc/files/movie/tpc.thp",        NULL },
        { 1, "orig/GC6E01/disc/files/movie/gs_logo.thp",    NULL }
    };
    int n = (int)(sizeof(kBoot) / sizeof(kBoot[0]));
    int i;
    u16 prev = 0;
    int dumpFrame = -1;
    int dumpItem = 0;
    const char* e;

    if (g_pcStoryFieldSmoke.active || getenv("PCPORT_NO_BOOT") != NULL) {
        return 1;
    }
    e = getenv("PCPORT_BOOT_DUMP_FRAME");
    if (e != NULL && e[0] != '\0') {
        dumpFrame = atoi(e);
    }
    e = getenv("PCPORT_BOOT_DUMP_ITEM");
    if (e != NULL && e[0] != '\0') {
        dumpItem = atoi(e);
        if (dumpItem < 0 || dumpItem >= n) {
            dumpItem = 0;
        }
    }

    for (i = 0; i < n; ++i) {
        int rc;
        int itemDumpFrame = (dumpFrame >= 0 && i == dumpItem) ? dumpFrame : -1;
        if (dumpFrame >= 0 && i != dumpItem) {
            continue;
        }
        if (kBoot[i].isThp) {
            rc = BootPlayTHP(window, kBoot[i].archive, itemDumpFrame, &prev);
        } else {
            rc = BootShowLogo(window, kBoot[i].archive, kBoot[i].member,
                              2.6, itemDumpFrame, &prev);
        }
        if (rc == 0) {
            GXHostClearTextureBinding();
            return 0;  /* window closed during boot */
        }
        if (dumpFrame >= 0) {
            GXHostClearTextureBinding();
            return 1;  /* verification: rendered the first item, stop */
        }
    }

    GXHostClearTextureBinding();
    return 1;
}

/* --- Title-screen posed cast -------------------------------------------------
 * The real title composites a set of pre-rendered 2D character/Pokemon cutouts
 * over the desert scene and cycles between sets when idle. These are NOT 3D
 * models (confirmed static in Dolphin) -- the existing 2D textured-quad path
 * (DrawTexturedScreenRect) draws them directly. The cutouts live in
 * `title.fsys` as `t_vs_*` members (0x80-header, format byte@4 = 0x10 = RGB5A3
 * with a soft alpha edge); LoadFsysSpriteTexObj decodes them (non-0x20 -> RGB5A3).
 * Identified set 1 ("c" group): t_vs_c3 = Wes, t_vs_c4 = Rui, t_vs_c2 = Umbreon
 * (t_vs_c1 = Espeon is not in title.fsys -- the default-set 4th cutout is loaded
 * from elsewhere; TBD, fail-loads gracefully until located). Reference layout:
 * both humans lower-LEFT (facing right), both Pokemon lower-RIGHT (facing left).
 * hflip swaps u0/u1 to mirror a cutout so it faces inward. */
typedef struct PCPortTitleCastMember {
    const char* member;     /* title.fsys member name */
    f32 x, y, w, h;         /* screen-space rect (640x480, origin top-left) */
    int hflip;              /* mirror horizontally (face inward) */
    const char* blob;       /* repo-relative raw-RGBA fallback if member absent */
} PCPortTitleCastMember;

/* Entries are drawn IN ORDER (back-to-front), so the in-front cutout of an
 * overlapping pair comes later. The t_vs art is authored already facing the
 * correct title direction (native: Wes/Rui face right, Umbreon/Espeon face
 * left) so hflip stays 0 -- the game does NOT mirror them. Rects measured
 * against the Dolphin reference frame. Left pair: Rui behind, Wes in front;
 * right pair: Umbreon behind, Espeon in front. */
/* Rects EXTRACTED by template-matching each cutout (alpha-weighted, occlusion-
 * masked, front-to-back) against a CLEAN borderless Dolphin F9 set-1 frame --
 * build_pc/logo_probe/match_cutouts.py (CLEAN=1). Order is back-to-front. */
static const PCPortTitleCastMember kTitleCastSet1[] = {
    { "t_vs_c4",  40.0f, 254.0f, 139.0f, 221.0f, 0, NULL },  /* Rui : behind, right-of-Wes */
    { "t_vs_c3",   4.0f, 262.0f, 133.0f, 214.0f, 0, NULL },  /* Wes : front, far-left      */
    { "t_vs_c2", 456.0f, 254.0f, 163.0f, 220.0f, 0, NULL },  /* Umbreon : behind, left     */
    /* Espeon: 135x192 RGB5A3, dumped from Dolphin (not found as a disc member);
     * loaded from the bundled raw-RGBA blob. Front of the right pair. */
    { "t_vs_c1", 508.0f, 292.0f, 129.0f, 184.0f, 0,
      "tools/pcport_assets/title_espeon.rgba" },           /* Espeon : front, right */
};
#define PCPORT_TITLE_CAST_MAX 8
#define PCPORT_TITLE_CAST_ARCHIVE "orig/GC6E01/disc/files/title.fsys"

/* The real title cycles through several cast SETS while idle. Each set is a list
 * of cutouts (back-to-front). Set 1 (protagonists) is template-matched against a
 * clean F9 frame and is exact; the others are provisional placements from the
 * t_vs group IDs + the corner layout, to be refined per-set with a clean F9 shot
 * run through build_pc/logo_probe/discover_set.py. */
static const PCPortTitleCastMember kTitleCastSet2[] = {  /* legendaries */
    { "t_vs_a4",  70.0f, 288.0f, 139.0f, 140.0f, 0, NULL },  /* Kyogre  : behind, left  */
    { "t_vs_a5", -10.0f, 276.0f, 190.0f, 196.0f, 0, NULL },  /* Groudon : front, left   */
    { "t_vs_a3", 438.0f, 256.0f, 152.0f, 205.0f, 0, NULL },  /* Suicune : behind, right */
    { "t_vs_a1", 504.0f, 300.0f, 128.0f, 159.0f, 0, NULL },  /* Raikou  : front, right  */
};
static const PCPortTitleCastMember kTitleCastSet3[] = {  /* starters (partial: only b2/b3 on disc) */
    { "t_vs_b2",  20.0f, 280.0f, 170.0f, 167.0f, 0, NULL },  /* Meganium   : left  */
    { "t_vs_b3", 470.0f, 286.0f, 135.0f, 188.0f, 0, NULL },  /* Feraligatr : right */
};

typedef struct PCPortTitleSet {
    const PCPortTitleCastMember* members;
    int count;
    const char* name;
} PCPortTitleSet;

#define PCPORT_TITLE_SET_COUNT_(arr) ((int)(sizeof(arr)/sizeof((arr)[0])))
static const PCPortTitleSet kTitleSets[] = {
    { kTitleCastSet1, 4, "protagonists" },
    { kTitleCastSet2, 4, "legendaries" },
    { kTitleCastSet3, 2, "starters" },
};
#define PCPORT_TITLE_NUM_SETS ((int)(sizeof(kTitleSets)/sizeof(kTitleSets[0])))
#define PCPORT_TITLE_MAX_SETS 8   /* array dim; must be >= number of kTitleSets entries */

/*
 * Route B boot path: load+parse the top-menu scene once, then present its full
 * joint tree every frame in a persistent window loop. Reads the keyboard/pad
 * each frame (host edge-detector) so START advances title -> main menu. The
 * loop runs until the window is closed; an explicit PCPORT_MENU_FRAMES cap (or
 * PCPORT_DUMP) keeps the headless screenshot path finite. Reuses the resolve/
 * camera/draw primitives proven by the slice smokes.
 */
static int RunMenuScene(GLFWwindow* window) {
    PCPortHSDArchive archive;
    PCPortTranslatedCamera translatedCamera;
    /* Title intro camera pan-out (GameCube renders the ruins in 3D then pulls the
     * camera out). One-shot: lerp eye/interest start->end over panSecs (ease-out),
     * then hold the wide title end pose. PCPORT_NO_PAN disables; PCPORT_PAN_SECS sets
     * the duration. The real cam_logo_demo_start/stop barely differ (a ~10u dolly --
     * the dramatic Orre flythrough is the opening-demo movie), so this is a slightly
     * more cinematic pull-out anchored on the authentic end pose. */
    f32 panStartEye[3] = { 0.0f, 20.0f, 140.0f };
    f32 panStartInt[3] = { 0.0f, 52.0f, -70.0f };
    f32 panEndEye[3]   = { 0.0f, 38.905f, 409.812f };
    f32 panEndInt[3]   = { 0.0f, 39.6514f, 1.5625f };
    f32 panUp[3]       = { 0.0f, 1.0f, 0.0f };
    int titlePanOn     = 0;
    double panSecs     = 4.5;
    u8* memberData = NULL;
    u32 memberSize = 0;
    const u8* sceneData;
    u32 sceneOffset = 0;
    u32 sceneBranchOffset;
    u32 cameraDescOffset;
    u32 sceneJointListOffset;
    u32 rootJointOffset;
    const char* capEnv;
    int frameCap;
    int frame;
    int dumpRequested;
    int ok = 0;
    const char* menuMember;
    const char* sceneArchive;
    PCPortHSDArchive logoArchive;
    u8* logoData = NULL;
    u32 logoSize = 0;
    u8* logoPixels = NULL;
    u32 logoPxSize = 0;
    GXTexObj logoTex;
    int haveLogo = 0;
    GXTexObj menu018Tex;
    int haveMenu018 = 0;
    GXTexObj menu033Tex;
    int haveMenu033 = 0;
    GXTexObj menu032Tex;
    int haveMenu032 = 0;
    int menuCursor = 0;
    PCPortHSDArchive menuBgArchive;
    u8* menuBgData = NULL;
    u32 menuBgSize = 0;
    GXTexObj menuBgTex;
    int haveMenuBg = 0;
    GXTexObj skyTex;
    int haveSky = 0;
    GXTexObj cloudTex;             /* idx19 sky band, GX_REPEAT for the drift scroll */
    int haveCloud = 0;
    GXTexObj windTex;              /* procedural sand-wind wisps, GX_REPEAT */
    int haveWind = 0;
    f32 cloudSpeed = 0.010f;       /* texture-units/sec the clouds drift left */
    f32 cloudSpanX = 1.0f;         /* texture widths across the screen */
    f32 windSpeed = 0.060f;        /* texture-units/sec the sand-wind blows left */
    f32 cloudBandH = 190.0f;       /* sky-band height in px (clouds fade out below) */
    int cloudsEnabled = 0;
    int windEnabled = 0;
    double animTimeForced = -1.0;  /* PCPORT_ANIM_TIME pins the anim clock (headless) */
    GXTexObj titleCastTex[PCPORT_TITLE_MAX_SETS][PCPORT_TITLE_CAST_MAX];
    int titleCastOk[PCPORT_TITLE_MAX_SETS][PCPORT_TITLE_CAST_MAX];
    int titleCastIdx;
    int titleSetI;
    int titleSetIndex = 0;
    int titleSetForced = -1;       /* PCPORT_TITLE_SET=N pins a set (headless capture) */
    double titleCycleSecs = 7.0;   /* PCPORT_CYCLE_SECS overrides */
    double titleCycleStart = 0.0;
    double demoIdleStart = 0.0;    /* attract: idle this long on the title -> opening demo */
    double demoIdleSecs = 22.0;    /* PCPORT_ATTRACT_SECS overrides; PCPORT_NO_ATTRACT off */
    int render3D = 0;
    PADStatus pads[4];
    u16 padHeld = 0;
    u16 padPrev = 0;
    u16 padPressed = 0;
    PCPortSceneState sceneState = PCPORT_SCENE_TITLE;
    int saveExists = 0;
    int dialogKind = PCPORT_DLG_INFO;
    int dialogYesNo = 0;
    int dialogCursor = 0;
    const char* dialogText = NULL;
    int debugStartFrame = -1;
    const char* debugStartEnv;
    int debugCursor = -1;
    const char* debugCursorEnv;
    int debugAFrame = -1;
    const char* debugAEnv;
    const char* seqBase = NULL;    /* PCPORT_DUMP_SEQ: within-run sequence capture */
    int seqEvery = 10;
    int capExplicit;
    int titleHostUi = 0;
    int titleHostCam = 0;
    int titleSceneOverlays = 0;
    int titleCastEnabled = 0;
    int haveTitleCast = 0;

    memset(&archive, 0, sizeof(archive));
    memset(&translatedCamera, 0, sizeof(translatedCamera));
    memset(&logoArchive, 0, sizeof(logoArchive));
    memset(&logoTex, 0, sizeof(logoTex));
    memset(&menu018Tex, 0, sizeof(menu018Tex));
    memset(&menu033Tex, 0, sizeof(menu033Tex));
    memset(&menu032Tex, 0, sizeof(menu032Tex));
    memset(&menuBgArchive, 0, sizeof(menuBgArchive));
    memset(&menuBgTex, 0, sizeof(menuBgTex));
    memset(&skyTex, 0, sizeof(skyTex));
    memset(&cloudTex, 0, sizeof(cloudTex));
    memset(&windTex, 0, sizeof(windTex));
    memset(titleCastTex, 0, sizeof(titleCastTex));
    memset(titleCastOk, 0, sizeof(titleCastOk));
    memset(pads, 0, sizeof(pads));

    /* Default to the title scene (desert/ruins environment + logo) in
     * title.fsys:logo_demo. Env overrides PCPORT_MENU_ARCHIVE / PCPORT_MENU_MEMBER
     * select any other fsys scene member (e.g. topmenu.fsys / menu_bg00 for the
     * post-start main-menu background). */
    sceneArchive = getenv("PCPORT_MENU_ARCHIVE");
    if (sceneArchive == NULL || sceneArchive[0] == '\0') {
        sceneArchive = PCPORT_TITLE_SCENE_ARCHIVE;
    }
    menuMember = getenv("PCPORT_MENU_MEMBER");
    if (menuMember == NULL || menuMember[0] == '\0') {
        menuMember = PCPORT_TITLE_SCENE_MEMBER;
    }
    titleHostUi = getenv("PCPORT_TITLE_HOST_UI") != NULL;
    titleHostCam = getenv("PCPORT_TITLE_HOST_CAM") != NULL;
    titleSceneOverlays =
        strcmp(sceneArchive, PCPORT_TITLE_SCENE_ARCHIVE) == 0 &&
        strcmp(menuMember, PCPORT_TITLE_SCENE_MEMBER) == 0;
    titleCastEnabled = titleSceneOverlays &&
        getenv("PCPORT_NO_TITLE_CAST") == NULL;
    cloudsEnabled = titleSceneOverlays;
    windEnabled = titleSceneOverlays;

    if (!PCPort_LoadFsysMember(sceneArchive, menuMember,
                               &memberData, &memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Menu scene load failed (%s:%s)\n",
                sceneArchive, menuMember);
        goto cleanup;
    }

    if (!PCPort_HSDArchiveParseBE(&archive, memberData, memberSize)) {
        fprintf(stderr,
                "[pcport_bootstrap] Menu scene archive parse failed (%s:%s)\n",
                sceneArchive, menuMember);
        goto cleanup;
    }

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&archive,
                                                             "scene_data",
                                                             &sceneOffset);
    if (sceneData == NULL) {
        fprintf(stderr,
                "[pcport_bootstrap] Menu scene failed to resolve scene_data\n");
        goto cleanup;
    }

    sceneBranchOffset = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&archive, sceneBranchOffset, 0x10u)) {
        fprintf(stderr,
                "[pcport_bootstrap] Menu scene branch offset was invalid (0x%X)\n",
                sceneBranchOffset);
        goto cleanup;
    }

    cameraDescOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x08);
    if (!PCPort_TranslatePerspectiveCameraFromArchiveBE(&archive,
                                                        cameraDescOffset,
                                                        &translatedCamera)) {
        fprintf(stderr,
                "[pcport_bootstrap] Menu scene failed to translate camera (scene=0x%X camera=0x%X)\n",
                sceneOffset,
                cameraDescOffset);
        goto cleanup;
    }

    /* The real archive camera is the default. The old host-composited title
     * camera/pan is kept only as an opt-in diagnostic because it is not proven
     * against the game's title code path. */
    if (titleHostCam) {
        static const f32 titleEye[3] = { 0.0f, 38.905f, 409.812f };
        static const f32 titleInt[3] = { 0.0f, 39.6514f, 1.5625f };
        static const f32 titleUp[3]  = { 0.0f, 1.0f, 0.0f };

        BuildViewMatrixLookAt(titleEye, titleInt, titleUp,
                              translatedCamera.viewMatrix);
        /* enable the diagnostic intro pan-out (held at this end pose once it completes) */
        if (getenv("PCPORT_NO_PAN") == NULL) {
            const char* ps = getenv("PCPORT_PAN_SECS");
            if (ps != NULL && atof(ps) > 0.0) panSecs = atof(ps);
            titlePanOn = 1;
        }
    }

    /* Experimental manual camera: PCPORT_CAM_EYE / PCPORT_CAM_INT ("x,y,z") +
     * optional PCPORT_CAM_UP override the view via look-at, for dialing in the
     * framing. */
    {
        const char* ce = getenv("PCPORT_CAM_EYE");
        const char* ci = getenv("PCPORT_CAM_INT");
        const char* cu = getenv("PCPORT_CAM_UP");
        if (ce != NULL && ci != NULL) {
            f32 e[3] = { 0.0f, 0.0f, 0.0f };
            f32 in[3] = { 0.0f, 0.0f, 0.0f };
            f32 u[3] = { 0.0f, 1.0f, 0.0f };

            sscanf(ce, "%f,%f,%f", &e[0], &e[1], &e[2]);
            sscanf(ci, "%f,%f,%f", &in[0], &in[1], &in[2]);
            if (cu != NULL) {
                sscanf(cu, "%f,%f,%f", &u[0], &u[1], &u[2]);
            }
            BuildViewMatrixLookAt(e, in, u, translatedCamera.viewMatrix);
            printf("[cam] override eye=(%.1f,%.1f,%.1f) int=(%.1f,%.1f,%.1f) up=(%.1f,%.1f,%.1f)\n",
                   e[0], e[1], e[2], in[0], in[1], in[2], u[0], u[1], u[2]);
        }
    }

    sceneJointListOffset = PCPort_ReadBigEndianU32(archive.storage + sceneBranchOffset + 0x00);
    rootJointOffset = PCPort_ReadBigEndianU32(archive.storage + sceneJointListOffset + 0x00);
    if (!ArchiveRangeValid(&archive, rootJointOffset, PCPORT_SERIALIZED_JOINT_SIZE)) {
        fprintf(stderr,
                "[pcport_bootstrap] Menu scene root joint was invalid (0x%X)\n",
                rootJointOffset);
        goto cleanup;
    }

    /* Loop cap policy: an explicit PCPORT_MENU_FRAMES (or a PCPORT_DUMP request)
     * bounds the loop so the headless screenshot path stays finite; otherwise
     * the loop is uncapped (frameCap==0) and runs until the window is closed --
     * the interactive front-end. */
    capEnv = getenv("PCPORT_MENU_FRAMES");
    capExplicit = (capEnv != NULL && capEnv[0] != '\0');
    frameCap = capExplicit ? atoi(capEnv) : 0;
    if (capExplicit && frameCap <= 0) {
        frameCap = PCPORT_WINDOW_FRAMES;
    }
    dumpRequested = getenv("PCPORT_DUMP") != NULL;

    /* Debug affordance: inject a one-frame START press at PCPORT_DEBUG_START_FRAME
     * so the headless dump can exercise the real edge-detector + title->menu
     * transition without a physical keypress. Parsed before the dump-cap fallback
     * so the fallback can guarantee the loop runs long enough to reach it. */
    debugStartEnv = getenv("PCPORT_DEBUG_START_FRAME");
    if (debugStartEnv != NULL && debugStartEnv[0] != '\0') {
        debugStartFrame = atoi(debugStartEnv);
    }
    /* Debug affordance: pin the main-menu cursor to a fixed item (overrides nav)
     * so a headless dump can verify the hand-cursor position at any item. */
    debugCursorEnv = getenv("PCPORT_DEBUG_CURSOR");
    if (debugCursorEnv != NULL && debugCursorEnv[0] != '\0') {
        debugCursor = atoi(debugCursorEnv);
    }
    /* Debug affordance: inject a one-frame A press at PCPORT_DEBUG_A_FRAME so a
     * headless dump can open/confirm a dialog without a physical keypress. */
    debugAEnv = getenv("PCPORT_DEBUG_A_FRAME");
    if (debugAEnv != NULL && debugAEnv[0] != '\0') {
        debugAFrame = atoi(debugAEnv);
    }
    saveExists = PCPort_SaveExists();

    /* Debug affordance: PCPORT_DUMP_SEQ=<base> writes <base>_<frame>.bmp every
     * PCPORT_DUMP_SEQ_EVERY frames (default 10) within a single run, so the title
     * drift animations can be verified for actual motion from one process. */
    seqBase = getenv("PCPORT_DUMP_SEQ");
    if (seqBase != NULL && seqBase[0] != '\0') {
        const char* ev = getenv("PCPORT_DUMP_SEQ_EVERY");
        if (ev != NULL && atoi(ev) > 0) { seqEvery = atoi(ev); }
    } else {
        seqBase = NULL;
    }

    if (dumpRequested && frameCap <= 0) {
        /* A dump needs a finite "last frame". Make the fallback cap late enough
         * to both reach an injected debug START and capture the frame after it,
         * else the dump would silently grab the pre-transition title. */
        frameCap = PCPORT_GSGFX_SWAPS;
        if (debugStartFrame >= 0 && debugStartFrame + 2 > frameCap) {
            frameCap = debugStartFrame + 2;
        }
    }
    /* Default to rendering the real 3D title scene from title.fsys:logo_demo.
     * The legacy host logo/prompt composite is opt-in via PCPORT_TITLE_HOST_UI;
     * set PCPORT_NO_RENDER_3D=1 to fall back to the flat 2D sky backdrop. */
    render3D = getenv("PCPORT_NO_RENDER_3D") == NULL;

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);

    printf("[pcport_bootstrap] Menu scene loaded (scene=0x%X camera=0x%X rootJoint=0x%X frameCap=%d dump=%d)\n",
           sceneOffset,
           cameraDescOffset,
           rootJointOffset,
           frameCap,
           dumpRequested);
    if (!titleHostUi) {
        printf("[pcport_bootstrap] Legacy title logo/prompt disabled; title cast/cloud/sand overlays remain retail-backed (set PCPORT_TITLE_HOST_UI=1 for old composite)\n");
    }
    if (!titleHostCam) {
        printf("[pcport_bootstrap] Title host camera disabled; using archive camera (set PCPORT_TITLE_HOST_CAM=1 for legacy camera/pan)\n");
    }

    /* Real title HSD animation drive (FObj interpreter): build + arm a live
     * animated HSD_JObj tree from the title scene so the game's own anim pipeline
     * (HSD_JObjAnimAll -> HSD_TObjAnim/HSD_JObjAnim host overrides -> FObj interp)
     * runs each frame and updates the live HSD_TObj/JObj SRT fields. Gated by
     * PCPORT_TITLE_ANIM (the raw-BE RenderJointTree path does not yet read the
     * live tree, so this is the verified animation engine drive, not yet visible
     * -- see PCPort_TitleAnimSetup notes + the lane report). */
    if (getenv("PCPORT_TITLE_ANIM") != NULL) {
        PCPort_TitleAnimSetup(sceneArchive, menuMember);
    }

    /* Boot movies play first (skippable via PCPORT_NO_BOOT). */
    if (!RunBootSequence(window)) {
        ok = 1;  /* window closed during the boot sequence -> clean exit */
        goto cleanup;
    }
    if (getenv("PCPORT_BOOT_DUMP_FRAME") != NULL) {
        ok = 1;  /* boot frame captured for verification -> skip the title */
        goto cleanup;
    }

    /* Legacy host-composited title foreground. Disabled by default because the
     * real title code owns this composition; keep it only for comparison work. */
    if (titleHostUi &&
        PCPort_LoadFsysMember(PCPORT_LOGO_ARCHIVE, PCPORT_LOGO_MEMBER,
                              &logoData, &logoSize) &&
        PCPort_HSDArchiveParseBE(&logoArchive, logoData, logoSize)) {
        PCPortTranslatedTexture logoDesc;

        memset(&logoDesc, 0, sizeof(logoDesc));
        logoDesc.imageDataArchiveOffset = PCPORT_LOGO_IMAGE_OFFSET;
        logoDesc.format = GX_TF_RGBA8;
        logoDesc.width = PCPORT_LOGO_WIDTH;
        logoDesc.height = PCPORT_LOGO_HEIGHT;

        if (PCPort_BakeTextureRGBAFromArchiveBE(&logoArchive, &logoDesc,
                                                &logoPixels, &logoPxSize)) {
            GXHostInitTexObjRGBA8(&logoTex, logoPixels,
                                  PCPORT_LOGO_WIDTH, PCPORT_LOGO_HEIGHT,
                                  GX_CLAMP, GX_CLAMP);
            haveLogo = 1;
            printf("[pcport_bootstrap] Title logo loaded (%dx%d, %u bytes)\n",
                   PCPORT_LOGO_WIDTH, PCPORT_LOGO_HEIGHT, logoPxSize);
        }
    }
    if (titleHostUi && !haveLogo) {
        fprintf(stderr,
                "[pcport_bootstrap] Title logo unavailable (continuing without it)\n");
    }

    /* Title-screen 2D overlay: copyright lines + PRESS START (raw RGB5A3 sprite). */
    if (titleHostUi) {
        haveMenu018 = LoadRawMenuTexObj(PCPORT_TITLE_PRESS_MEMBER, &menu018Tex);
        if (haveMenu018) {
            printf("[pcport_bootstrap] Title overlay (PRESS START + copyright) loaded\n");
        }
    }

    /* Main-menu panel (shown after START). Loaded up front so the title->menu
     * transition is instant. */
    haveMenu033 = LoadRawMenuTexObj(PCPORT_MAIN_MENU_MEMBER, &menu033Tex);
    if (haveMenu033) {
        printf("[pcport_bootstrap] Main-menu panel (menu_033) loaded\n");
    } else {
        fprintf(stderr,
                "[pcport_bootstrap] Main-menu panel unavailable (START keeps the title)\n");
    }

    /* Main-menu chrome sheet (hand cursor + Quit button). */
    haveMenu032 = LoadRawMenuTexObj(PCPORT_TOPMENU_CHROME_MEMBER, &menu032Tex);
    if (haveMenu032) {
        printf("[pcport_bootstrap] Main-menu chrome (menu_032: hand cursor + Quit) loaded\n");
    }

    if (titleCastEnabled) {
        /* Title posed cast: load EVERY set's cutouts once (RGBA8/RGB5A3, alpha).
         * The title cycles through kTitleSets[] while idle. */
        for (titleSetI = 0; titleSetI < PCPORT_TITLE_NUM_SETS; ++titleSetI) {
            const PCPortTitleSet* set = &kTitleSets[titleSetI];
            int cnt = set->count;
            if (cnt > PCPORT_TITLE_CAST_MAX) {
                cnt = PCPORT_TITLE_CAST_MAX;
            }
            for (titleCastIdx = 0; titleCastIdx < cnt; ++titleCastIdx) {
                const PCPortTitleCastMember* cm = &set->members[titleCastIdx];
                memset(&titleCastTex[titleSetI][titleCastIdx], 0,
                       sizeof(titleCastTex[titleSetI][titleCastIdx]));
                titleCastOk[titleSetI][titleCastIdx] = LoadFsysSpriteTexObj(
                    PCPORT_TITLE_CAST_ARCHIVE, cm->member,
                    &titleCastTex[titleSetI][titleCastIdx]);
                if (!titleCastOk[titleSetI][titleCastIdx] && cm->blob != NULL) {
                    titleCastOk[titleSetI][titleCastIdx] = LoadRawRGBABlobTexObj(
                        cm->blob, &titleCastTex[titleSetI][titleCastIdx]);
                }
                printf("[pcport_bootstrap] Title cast[%s] %s: %s\n",
                       set->name, cm->member,
                       titleCastOk[titleSetI][titleCastIdx] ? "loaded" : "FAILED");
                if (titleCastOk[titleSetI][titleCastIdx]) {
                    haveTitleCast = 1;
                }
            }
        }
    }
    {
        const char* fs = getenv("PCPORT_TITLE_SET");
        const char* cs = getenv("PCPORT_CYCLE_SECS");
        if (fs != NULL) {
            titleSetForced = atoi(fs);
            if (titleSetForced >= 0 && titleSetForced < PCPORT_TITLE_NUM_SETS) {
                titleSetIndex = titleSetForced;
            }
        }
        if (cs != NULL) {
            double v = atof(cs);
            if (v > 0.5) { titleCycleSecs = v; }
        }
    }
    {
        /* Title ambient-animation tuning: cloud drift + sand-wind. */
        const char* e;
        if (getenv("PCPORT_NO_CLOUDS") != NULL) { cloudsEnabled = 0; }
        if (getenv("PCPORT_NO_WIND") != NULL) { windEnabled = 0; }
        e = getenv("PCPORT_CLOUD_SPEED");
        if (e != NULL) { cloudSpeed = (f32)atof(e); }
        e = getenv("PCPORT_WIND_SPEED");
        if (e != NULL) { windSpeed = (f32)atof(e); }
        e = getenv("PCPORT_CLOUD_H");
        if (e != NULL) { f32 v = (f32)atof(e); if (v > 10.0f) { cloudBandH = v; } }
        e = getenv("PCPORT_CLOUD_SPAN");
        if (e != NULL) { f32 v = (f32)atof(e); if (v > 0.2f) { cloudSpanX = v; } }
        /* When set, drive the drift off a fixed clock instead of wall-time, so the
         * headless fast-loop (which advances glfwGetTime by ~nothing per frame)
         * can capture a chosen point in the animation deterministically. */
        e = getenv("PCPORT_ANIM_TIME");
        if (e != NULL) { animTimeForced = atof(e); }
    }

    /* Bake the blue-swirl main-menu background (menu_bg00, CMPR 640x480). */
    if (PCPort_LoadFsysMember(PCPORT_REAL_CONTENT_ARCHIVE, PCPORT_MENU_BG_MEMBER,
                              &menuBgData, &menuBgSize) &&
        PCPort_HSDArchiveParseBE(&menuBgArchive, menuBgData, menuBgSize)) {
        PCPortTranslatedTexture bgDesc;
        u8* bgPixels = NULL;
        u32 bgPxSize = 0;

        memset(&bgDesc, 0, sizeof(bgDesc));
        bgDesc.imageDataArchiveOffset = PCPORT_MENU_BG_OFFSET;
        bgDesc.format = GX_TF_CMPR;
        bgDesc.width = PCPORT_MENU_BG_WIDTH;
        bgDesc.height = PCPORT_MENU_BG_HEIGHT;
        if (PCPort_BakeTextureRGBAFromArchiveBE(&menuBgArchive, &bgDesc,
                                                &bgPixels, &bgPxSize)) {
            GXHostInitTexObjRGBA8(&menuBgTex, bgPixels,
                                  PCPORT_MENU_BG_WIDTH, PCPORT_MENU_BG_HEIGHT,
                                  GX_CLAMP, GX_CLAMP);
            PCPort_FreeBuffer(bgPixels);
            haveMenuBg = 1;
            printf("[pcport_bootstrap] Main-menu background (menu_bg00 swirl) baked (%dx%d)\n",
                   PCPORT_MENU_BG_WIDTH, PCPORT_MENU_BG_HEIGHT);
        }
    }
    if (!haveMenuBg) {
        fprintf(stderr,
                "[pcport_bootstrap] Main-menu background unavailable (flat-blue stand-in)\n");
    }

    /* Bake the desert sky/sand backdrop (CMPR) from the title scene archive. */
    if (!render3D) {
        PCPortTranslatedTexture skyDesc;
        u8* skyPixels = NULL;
        u32 skyPxSize = 0;

        memset(&skyDesc, 0, sizeof(skyDesc));
        skyDesc.imageDataArchiveOffset = PCPORT_TITLE_SKY_OFFSET;
        skyDesc.format = GX_TF_CMPR;
        skyDesc.width = PCPORT_TITLE_SKY_WIDTH;
        skyDesc.height = PCPORT_TITLE_SKY_HEIGHT;
        if (PCPort_BakeTextureRGBAFromArchiveBE(&archive, &skyDesc,
                                                &skyPixels, &skyPxSize)) {
            GXHostInitTexObjRGBA8(&skyTex, skyPixels,
                                  PCPORT_TITLE_SKY_WIDTH, PCPORT_TITLE_SKY_HEIGHT,
                                  GX_CLAMP, GX_CLAMP);
            PCPort_FreeBuffer(skyPixels);
            haveSky = 1;
            printf("[pcport_bootstrap] Title sky backdrop loaded (%dx%d)\n",
                   PCPORT_TITLE_SKY_WIDTH, PCPORT_TITLE_SKY_HEIGHT);
        }
    }

    /* Retail-backed drifting-cloud layer: the same sky texture (blue + clouds fading to tan),
     * but uploaded GX_REPEAT on S so the title can scroll its U over time and the
     * clouds wrap seamlessly. Drawn as a 2D band over the top sky region in the
     * 3D title path (where the scene's own sky reads as flat blue), so the clouds
     * are actually visible and animate without reviving the old host logo. */
    if (cloudsEnabled) {
        PCPortTranslatedTexture cloudDesc;
        u8* cloudPixels = NULL;
        u32 cloudPxSize = 0;

        memset(&cloudDesc, 0, sizeof(cloudDesc));
        cloudDesc.imageDataArchiveOffset = PCPORT_TITLE_SKY_OFFSET;
        cloudDesc.format = GX_TF_CMPR;
        cloudDesc.width = PCPORT_TITLE_SKY_WIDTH;
        cloudDesc.height = PCPORT_TITLE_SKY_HEIGHT;
        if (PCPort_BakeTextureRGBAFromArchiveBE(&archive, &cloudDesc,
                                                &cloudPixels, &cloudPxSize)) {
            if (getenv("PCPORT_CLOUD_SEAM_HEAL") != NULL) {
                MakeSeamlessHoriz(cloudPixels, PCPORT_TITLE_SKY_WIDTH,
                                  PCPORT_TITLE_SKY_HEIGHT);
            }
            GXHostInitTexObjRGBA8(&cloudTex, cloudPixels,
                                  PCPORT_TITLE_SKY_WIDTH, PCPORT_TITLE_SKY_HEIGHT,
                                  GX_REPEAT, GX_CLAMP);
            PCPort_FreeBuffer(cloudPixels);
            haveCloud = 1;
            printf("[pcport_bootstrap] Title drifting-cloud layer baked (%dx%d, repeat-S)\n",
                   PCPORT_TITLE_SKY_WIDTH, PCPORT_TITLE_SKY_HEIGHT);
        }
    }

    /* Retail-backed sand-wind layer: procedural tileable wisps, scrolled across the desert. */
    if (windEnabled && BuildSandWindTexture(&windTex)) {
        haveWind = 1;
        printf("[pcport_bootstrap] Title sand-wind layer built (procedural wisps)\n");
    }

    /* HSD-pipeline bring-up: verify the BE->LE scene swizzle produces sane values
     * (root joint SRT, material colors/alpha, TObj) before wiring the real load. */
    if (getenv("PCPORT_HSD_SWIZ_TEST") != NULL) {
        /* PCPORT_SWIZ_ARCHIVE/PCPORT_SWIZ_MEMBER point the probe at any member
         * (e.g. a people_archive character model) to inspect its HSD layout. */
        const char* sa = getenv("PCPORT_SWIZ_ARCHIVE");
        const char* sm = getenv("PCPORT_SWIZ_MEMBER");
        PCPort_HSDSwizzleSmoke(sa != NULL ? sa : PCPORT_LOGO_ARCHIVE,
                               sm != NULL ? sm : PCPORT_LOGO_MEMBER);
        ok = 1;
        goto cleanup;
    }
    /* Decisive probe: does a character archive carry real joint motion? */
    if (getenv("PCPORT_CHARANIM_PROBE") != NULL) {
        const char* sa = getenv("PCPORT_SWIZ_ARCHIVE");
        const char* sm = getenv("PCPORT_SWIZ_MEMBER");
        int frames = atoi(getenv("PCPORT_CHARANIM_PROBE"));
        PCPort_CharAnimProbe(
            sa != NULL ? sa : "orig/GC6E01/disc/files/field_common.fsys",
            sm != NULL ? sm : "ken_b1", frames);
        ok = 1;
        goto cleanup;
    }
    /* Headless verify of the New Game -> field hand-off (skips menu navigation). */
    if (g_pcStoryFieldSmoke.active || getenv("PCPORT_DEBUG_NEWGAME") != NULL) {
        printf("[pcport_bootstrap] %s -> entering field\n",
               g_pcStoryFieldSmoke.active ? "STORY_FIELD_SMOKE"
                                          : "DEBUG_NEWGAME");
        if (g_pcStoryFieldSmoke.active) {
            g_pcStoryFieldSmoke.sawMenuHandoff = 1;
        }
        g_pcEnterFieldWalk = 1;
        ok = 1;
        goto cleanup;
    }
    /* Headless verify of the Colosseum Battle menu hand-off. */
    if (getenv("PCPORT_DEBUG_COLOSSEUM_BATTLE") != NULL) {
        printf("[pcport_bootstrap] DEBUG_COLOSSEUM_BATTLE -> entering Battle Colosseum\n");
        g_pcEnterBattleColosseum = 1;
        ok = 1;
        goto cleanup;
    }

    /* Build the ASCII font atlas once (GL context ready) for menu/prompt text. */
    EnsureFontAtlas();

    for (frame = 0; ; ++frame) {
        MenuTreeStats stats;

        if (window != NULL && glfwWindowShouldClose(window)) {
            break;
        }
        if (frameCap > 0 && frame >= frameCap) {
            break;
        }

        memset(&stats, 0, sizeof(stats));

        VIWaitForRetrace_PC();   /* pumps glfwPollEvents -> fresh key state */

        /* Host input + edge detection: read the pad, derive this frame's
         * newly-pressed buttons (held & ~prev), then advance the front-end. */
        PADRead(pads);
        padHeld = pads[0].button;
        if (debugStartFrame >= 0 && frame == debugStartFrame) {
            padHeld = (u16)(padHeld | GCN_PAD_BUTTON_START);
        }
        if (debugAFrame >= 0 && frame == debugAFrame) {
            padHeld = (u16)(padHeld | GCN_PAD_BUTTON_A);
        }
        padPressed = (u16)(padHeld & ~padPrev);
        padPrev = padHeld;

        /* Cast cycling: advance to the next set every titleCycleSecs, CONTINUOUSLY
         * (like the real attract title) -- NOT gated on idle/input. The previous
         * "reset on any input" made the cast appear stuck whenever input arrived
         * each frame, and only the menu round-trip (which froze the timer) ever
         * advanced it. Runs every frame regardless of scene state; only rendered on
         * the title. PCPORT_TITLE_SET pins a set for headless capture. */
        {
            double nowT = glfwGetTime();
            if (titleCycleStart == 0.0) {
                titleCycleStart = nowT;
            }
            if (titleSetForced < 0 && (nowT - titleCycleStart) >= titleCycleSecs) {
                titleSetIndex = (titleSetIndex + 1) % PCPORT_TITLE_NUM_SETS;
                titleCycleStart = nowT;
                printf("[pcport_bootstrap] title cast -> set %d (%s)\n",
                       titleSetIndex, kTitleSets[titleSetIndex].name);
            }
        }

        /* Advance the real title HSD animation (when armed via PCPORT_TITLE_ANIM).
         * Runs the game's HSD_JObjAnimAll over the live tree -> FObj interpreter
         * updates the live HSD_TObj/JObj SRT fields. No-op unless set up. */
        if (sceneState == PCPORT_SCENE_TITLE) {
            PCPort_TitleAnimTick();
        }

        if (sceneState == PCPORT_SCENE_TITLE) {
            double nowT = glfwGetTime();
            /* Attract loop: after a longer idle (no input) on the title, play the
             * opening-demo movie, then return to the title -- this is where the
             * real game shows the demo (NOT during boot). PCPORT_NO_ATTRACT off,
             * PCPORT_ATTRACT_SECS sets the idle threshold. */
            if (demoIdleStart == 0.0) {
                const char* as = getenv("PCPORT_ATTRACT_SECS");
                demoIdleStart = nowT;
                if (as != NULL && atof(as) > 0.0) demoIdleSecs = atof(as);
            }
            if (padPressed != 0) {
                demoIdleStart = nowT;
            } else if (getenv("PCPORT_NO_ATTRACT") == NULL &&
                       (nowT - demoIdleStart) >= demoIdleSecs) {
                printf("[pcport_bootstrap] title idle %.0fs -> attract demo (openingdemo)\n",
                       demoIdleSecs);
                if (!BootPlayTHP(window,
                        "orig/GC6E01/disc/files/movie/openingdemo.thp", -1, &padPrev)) {
                    goto cleanup;   /* window closed during the demo */
                }
                GXHostClearTextureBinding();
                demoIdleStart = glfwGetTime();
                titleCycleStart = demoIdleStart;
                padPrev = 0;
            }
            if ((padPressed & GCN_PAD_BUTTON_START) && haveMenu033) {
                /* The real game checks the memory card on START before the menu. */
                sceneState = PCPORT_SCENE_SAVE_PROMPT;
                printf("[pcport_bootstrap] START pressed (frame %d) -> save prompt\n",
                       frame);
            }
        } else if (sceneState == PCPORT_SCENE_SAVE_PROMPT) {
            if (padPressed & (GCN_PAD_BUTTON_START | GCN_PAD_BUTTON_A |
                              GCN_PAD_BUTTON_B)) {
                sceneState = PCPORT_SCENE_MAIN_MENU;
                menuCursor = 0;
                printf("[pcport_bootstrap] save prompt dismissed -> main menu\n");
            }
        } else if (sceneState == PCPORT_SCENE_MAIN_MENU) {
            if (padPressed & GCN_PAD_BUTTON_DOWN) {
                menuCursor = (menuCursor + 1) % PCPORT_MENU_ITEM_COUNT;
            }
            if (padPressed & GCN_PAD_BUTTON_UP) {
                menuCursor = (menuCursor + PCPORT_MENU_ITEM_COUNT - 1) %
                             PCPORT_MENU_ITEM_COUNT;
            }
            if (debugCursor >= 0) {
                menuCursor = debugCursor % PCPORT_MENU_ITEM_COUNT; /* pin for headless capture */
            }
            if (padPressed & GCN_PAD_BUTTON_A) {
                /* Open the dialog appropriate to the selected item. */
                dialogCursor = 0;
                switch (menuCursor) {
                case 0: /* CONTINUE */
                    if (saveExists) {
                        dialogYesNo = 1; dialogKind = PCPORT_DLG_CONTINUE;
                        dialogText = "Load the saved game and continue Story Mode?";
                    } else {
                        dialogYesNo = 0; dialogKind = PCPORT_DLG_INFO;
                        dialogText = "There is no saved game data to continue.";
                    }
                    break;
                case 1: /* NEW GAME */
                    if (saveExists) {
                        dialogYesNo = 1; dialogKind = PCPORT_DLG_NEWGAME;
                        dialogText = "A saved game already exists. Overwrite it and start a new adventure?";
                    } else {
                        /* No save -> start the new adventure: enter the first field
                         * (Wes's hideout, D1_garage_1F) via the walkable field path. */
                        printf("[pcport_bootstrap] New Game -> entering field\n");
                        g_pcEnterFieldWalk = 1;
                        ok = 1;
                        goto cleanup;
                    }
                    break;
                case 2: /* COLOSSEUM BATTLE */
                    printf("[pcport_bootstrap] Colosseum Battle -> entering battle scene\n");
                    g_pcEnterBattleColosseum = 1;
                    ok = 1;
                    goto cleanup;
                case 3: /* BATTLE NOW */
                    dialogYesNo = 0; dialogKind = PCPORT_DLG_INFO;
                    dialogText = "Battle Now is not yet available in this port.";
                    break;
                case 4: /* OPTIONS */
                    dialogYesNo = 0; dialogKind = PCPORT_DLG_INFO;
                    dialogText = "Options are not yet available in this port.";
                    break;
                default: /* QUIT */
                    dialogYesNo = 1; dialogKind = PCPORT_DLG_QUIT;
                    dialogText = "Quit the game?";
                    break;
                }
                sceneState = PCPORT_SCENE_DIALOG;
                printf("[pcport_bootstrap] Selected %s -> dialog\n",
                       kMainMenuItems[menuCursor].label);
            }
            if (padPressed & GCN_PAD_BUTTON_B) {
                sceneState = PCPORT_SCENE_TITLE;
                printf("[pcport_bootstrap] B pressed -> back to title\n");
            }
        } else { /* PCPORT_SCENE_DIALOG */
            if (dialogYesNo) {
                if (padPressed & GCN_PAD_BUTTON_LEFT) {
                    dialogCursor = 0; /* Yes */
                }
                if (padPressed & GCN_PAD_BUTTON_RIGHT) {
                    dialogCursor = 1; /* No */
                }
                if (padPressed & GCN_PAD_BUTTON_A) {
                    if (dialogCursor == 0) { /* Yes */
                        if (dialogKind == PCPORT_DLG_QUIT) {
                            if (window != NULL) {
                                glfwSetWindowShouldClose(window, GLFW_TRUE);
                            }
                            printf("[pcport_bootstrap] Quit confirmed -> closing\n");
                        } else if (dialogKind == PCPORT_DLG_NEWGAME ||
                                   dialogKind == PCPORT_DLG_CONTINUE) {
                            /* New Game (overwrite) or Continue -> enter the field. */
                            printf("[pcport_bootstrap] %s -> entering field\n",
                                   dialogKind == PCPORT_DLG_NEWGAME ? "New Game" : "Continue");
                            g_pcEnterFieldWalk = 1;
                            ok = 1;
                            goto cleanup;
                        } else {
                            printf("[pcport_bootstrap] Confirmed (not yet implemented)\n");
                        }
                    }
                    sceneState = PCPORT_SCENE_MAIN_MENU;
                }
                if (padPressed & GCN_PAD_BUTTON_B) {
                    sceneState = PCPORT_SCENE_MAIN_MENU;
                }
            } else { /* info box: A or B dismisses */
                if (padPressed & (GCN_PAD_BUTTON_A | GCN_PAD_BUTTON_B)) {
                    sceneState = PCPORT_SCENE_MAIN_MENU;
                }
            }
        }

        /* Blue backdrop for the main menu (the real game's swirl background is a
         * separate topmenu.fsys member, not yet located -- placeholder for now);
         * black behind the 3D title. */
        if (sceneState == PCPORT_SCENE_MAIN_MENU) {
            ClearBackbuffer(0.16f, 0.22f, 0.45f);
        } else {
            ClearBackbuffer(0.0f, 0.0f, 0.0f);
        }
        GSgfx_BeginFrame();

        /* GSgfx_BeginFrame paints a green EFB clear-quad; the 3D title used to be
         * fully covered by the (opaque) desert planes so it never showed, but now
         * those planes are translucent (XLU haze) the green bleeds through. Wipe it
         * to a pale desert-haze tone so the translucent haze reads as bright desert
         * (matches the Dolphin look) instead of green. PCPORT_TITLE_BG overrides. */
        if (render3D && (sceneState == PCPORT_SCENE_TITLE ||
                         sceneState == PCPORT_SCENE_SAVE_PROMPT)) {
            const char* tbg = getenv("PCPORT_TITLE_BG");
            f32 br = 0.80f, bg = 0.76f, bb = 0.68f;
            if (tbg != NULL) sscanf(tbg, "%f,%f,%f", &br, &bg, &bb);
            ClearBackbuffer(br, bg, bb);
        }

        if (render3D && (sceneState == PCPORT_SCENE_TITLE ||
                         sceneState == PCPORT_SCENE_SAVE_PROMPT)) {
            GXSetViewport((f32)translatedCamera.viewportLeft,
                          (f32)translatedCamera.viewportTop,
                          (f32)(translatedCamera.viewportRight - translatedCamera.viewportLeft),
                          (f32)(translatedCamera.viewportBottom - translatedCamera.viewportTop),
                          0.0f,
                          1.0f);
            GXSetScissor((u32)translatedCamera.scissorLeft,
                         (u32)translatedCamera.scissorTop,
                         (u32)(translatedCamera.scissorRight - translatedCamera.scissorLeft),
                         (u32)(translatedCamera.scissorBottom - translatedCamera.scissorTop));
            GXSetProjection(translatedCamera.projectionMatrix, GX_PERSPECTIVE);
            /* Intro pan-out: ease the camera from the close start pose to the wide
             * title end over panSecs, then hold. Disabled by PCPORT_NO_PAN or while a
             * manual PCPORT_CAM_EYE override is active. */
            if (titlePanOn && getenv("PCPORT_CAM_EYE") == NULL) {
                double aT = (animTimeForced >= 0.0) ? animTimeForced : glfwGetTime();
                double tt = aT / panSecs;
                double e;
                f32 ce[3], ci[3];
                int k;
                if (tt < 0.0) tt = 0.0;
                if (tt > 1.0) tt = 1.0;
                e = 1.0 - pow(1.0 - tt, 3.0);   /* ease-out cubic */
                for (k = 0; k < 3; ++k) {
                    ce[k] = panStartEye[k] + (f32)((panEndEye[k] - panStartEye[k]) * e);
                    ci[k] = panStartInt[k] + (f32)((panEndInt[k] - panStartInt[k]) * e);
                }
                BuildViewMatrixLookAt(ce, ci, panUp, translatedCamera.viewMatrix);
            } else if (getenv("PCPORT_CAM_EYE") != NULL) {
                /* Manual title-camera override (debug): place eye + interest
                 * freely, e.g. to frame the whole ruins volume. */
                f32 ce[3] = { translatedCamera.eye[0], translatedCamera.eye[1],
                              translatedCamera.eye[2] };
                f32 ci[3] = { translatedCamera.interest[0], translatedCamera.interest[1],
                              translatedCamera.interest[2] };
                const char* ie = getenv("PCPORT_CAM_INT");
                sscanf(getenv("PCPORT_CAM_EYE"), "%f,%f,%f", &ce[0], &ce[1], &ce[2]);
                if (ie != NULL) sscanf(ie, "%f,%f,%f", &ci[0], &ci[1], &ci[2]);
                BuildViewMatrixLookAt(ce, ci, panUp, translatedCamera.viewMatrix);
            }
            if (frame == 0 && getenv("PCPORT_RENDER_DEBUG") != NULL) {
                printf("[cam] eye=(%.1f,%.1f,%.1f) interest=(%.1f,%.1f,%.1f) fov=%.1f aspect=%.2f near=%.2f far=%.2f\n",
                       translatedCamera.eye[0], translatedCamera.eye[1], translatedCamera.eye[2],
                       translatedCamera.interest[0], translatedCamera.interest[1], translatedCamera.interest[2],
                       translatedCamera.fov, translatedCamera.aspect,
                       translatedCamera.nearZ, translatedCamera.farZ);
            }
            /* Depth-test the scene so the large ground plane does not paint over
             * the standing ruin pillars (which are drawn before it). */
            GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

            RenderJointTree(&archive,
                            rootJointOffset,
                            rootJointOffset,
                            &translatedCamera,
                            (int)PCPORT_REAL_MATERIAL_PIPELINE,
                            &stats);
        }

        /* 2D overlay, selected by the current front-end state. The sky/sand
         * backdrop (when 3D is off) is shared by both states; the title draws
         * retail-backed cast/cloud/sand overlays by default, and the old host
         * logo/prompt only when PCPORT_TITLE_HOST_UI is set. */
        if (haveSky || haveCloud || haveWind || haveLogo ||
            haveTitleCast || haveMenu018 || haveMenu033 || haveMenu032) {
            BeginMenuOverlay();
            if (sceneState == PCPORT_SCENE_TITLE ||
                sceneState == PCPORT_SCENE_SAVE_PROMPT) {
                if (haveSky) {
                    DrawTexturedScreenRect(&skyTex, 0.0f, 0.0f, 640.0f, 480.0f,
                                           0.0f, 0.0f, 1.0f, 1.0f);
                }
                /* Drifting clouds: scroll the sky band's U to the left over time
                 * (GX_REPEAT wraps it). Drawn as an opaque upper band plus a lower
                 * feather strip that fades into the 3D desert, so there is no hard
                 * horizon seam. The texture's own tan-fading bottom helps the blend. */
                if (haveCloud) {
                    double animT = (animTimeForced >= 0.0)
                                       ? animTimeForced : glfwGetTime();
                    f32 cu0 = (f32)(animT * (double)cloudSpeed);
                    f32 cu1 = cu0 + cloudSpanX;  /* >1 tile = smaller, less-stretched clouds */
                    f32 bandH = cloudBandH;
                    f32 feat = 70.0f;            /* long fade into the warm-tan horizon */
                    f32 vMid;
                    /* Opaque upper band so the translucent 3D haze planes (which
                     * extend up into the sky region) don't show through as vertical
                     * seams. The lower feather strip still fades into the desert. */
                    u8 aMain = 255;
                    if (feat > bandH) { feat = bandH; }
                    vMid = (bandH - feat) / bandH;
                    DrawTexturedScreenRectA(&cloudTex, 0.0f, 0.0f, 640.0f, bandH - feat,
                                            cu0, 0.0f, cu1, vMid, aMain, aMain);
                    DrawTexturedScreenRectA(&cloudTex, 0.0f, bandH - feat, 640.0f, feat,
                                            cu0, vMid, cu1, 1.0f, aMain, 0);
                }
                /* Sand-wind: faint sandy wisps drifting left over the desert,
                 * feathered top and bottom so they sit subtly on the ground. */
                if (haveWind) {
                    double animT = (animTimeForced >= 0.0)
                                       ? animTimeForced : glfwGetTime();
                    f32 wu0 = (f32)(animT * (double)windSpeed);
                    f32 wu1 = wu0 + 2.5f;        /* tile ~2.5x for finer streaks */
                    DrawTexturedScreenRectA(&windTex, 0.0f, 196.0f, 640.0f, 236.0f,
                                            wu0, 0.0f, wu1, 1.0f, 255, 255);
                }
                /* Posed cast cutouts for the active cycling set, over the desert
                 * but UNDER the logo (heads may tuck behind the centre logo). */
                {
                    /* GameCube intro: the foreground UI appears AFTER the camera
                     * pan settles, and the Pokemon Colosseum logo BOUNCES in (scale
                     * overshoot), then the cast + PRESS START follow. During the pan
                     * only the 3D ruins show. PCPORT_NO_PAN -> everything immediate. */
                    double aT = (animTimeForced >= 0.0) ? animTimeForced : glfwGetTime();
                    int panDone = (!titlePanOn) || (aT >= panSecs);
                    double bt = titlePanOn ? (aT - panSecs) / 0.55 : 1.0;
                    double bp, logoScale;
                    int uiAfter;
                    /* PCPORT_SCENE_ONLY: render just the 3D world (ruins/sand/clouds +
                     * camera pan) -- suppress the 2D logo + cast cutouts + PRESS START,
                     * so the 3D title scene can be developed against the Dolphin ref. */
                    int sceneOnly = getenv("PCPORT_SCENE_ONLY") != NULL;
                    if (bt < 0.0) bt = 0.0;
                    if (bt > 1.0) bt = 1.0;
                    bp = bt - 1.0;
                    logoScale = 1.0 + 2.70158 * bp * bp * bp + 1.70158 * bp * bp; /* back-out overshoot */
                    uiAfter = (!titlePanOn) || (aT >= panSecs + 0.45);

                    /* cast cutouts (under the logo) -- appear once the logo lands */
                    if (titleCastEnabled && haveTitleCast && uiAfter && !sceneOnly) {
                        const PCPortTitleSet* aset = &kTitleSets[titleSetIndex];
                        int acnt = aset->count;
                        if (acnt > PCPORT_TITLE_CAST_MAX) { acnt = PCPORT_TITLE_CAST_MAX; }
                        for (titleCastIdx = 0; titleCastIdx < acnt; ++titleCastIdx) {
                            const PCPortTitleCastMember* cm = &aset->members[titleCastIdx];
                            f32 cu0 = cm->hflip ? 1.0f : 0.0f;
                            f32 cu1 = cm->hflip ? 0.0f : 1.0f;
                            if (!titleCastOk[titleSetIndex][titleCastIdx]) {
                                continue;
                            }
                            DrawTexturedScreenRect(&titleCastTex[titleSetIndex][titleCastIdx],
                                                   cm->x, cm->y, cm->w, cm->h,
                                                   cu0, 0.0f, cu1, 1.0f);
                        }
                    }
                    if (haveLogo && panDone && !sceneOnly) {
                        /* logo, top-centre (115,34,410,170), bounce-scaled about its centre */
                        f32 lw = (f32)(410.0 * logoScale);
                        f32 lh = (f32)(170.0 * logoScale);
                        f32 lx = (115.0f + 205.0f) - lw * 0.5f;
                        f32 ly = (34.0f + 85.0f) - lh * 0.5f;
                        DrawTexturedScreenRect(&logoTex, lx, ly, lw, lh,
                                               0.0f, 0.0f, 1.0f, 1.0f);
                    }
                    if (haveMenu018 && uiAfter && !sceneOnly) {
                        if (sceneState == PCPORT_SCENE_TITLE) {
                            DrawTexturedScreenRect(&menu018Tex, 188.0f, 268.0f, 264.0f, 30.0f,
                                                   0.0f, 0.574f, 1.0f, 0.721f);
                        }
                        DrawTexturedScreenRect(&menu018Tex, 28.0f, 392.0f, 300.0f, 58.0f,
                                               0.0f, 0.016f, 1.0f, 0.549f);
                    }
                }
                /* Memory-card read prompt over the title (matches the real boot). */
                if (sceneState == PCPORT_SCENE_SAVE_PROMPT) {
                    DrawDialogBox("The Memory Card in Slot A has been read!", 0, 0);
                }
            } else { /* PCPORT_SCENE_MAIN_MENU: composite the real layout from the sheets */
                /* Opaque backdrop FIRST -- the game's draw path leaves a green EFB
                 * clear that shows through everywhere; this covers it. Use the real
                 * menu_bg00 blue-swirl artwork when baked, else a flat-blue quad. */
                if (haveMenuBg) {
                    DrawTexturedScreenRect(&menuBgTex, 0.0f, 0.0f, 640.0f, 480.0f,
                                           0.0f, 0.0f, 1.0f, 1.0f);
                } else {
                    DrawSolidScreenRect(0.0f, 0.0f, 640.0f, 480.0f, 28, 44, 92, 255);
                }
                if (haveMenu033) {
                    /* STORY MODE card (sheet v 0.00-0.402) -> screen left (centred,
                     * narrower so it isn't clipped at the window edge) */
                    DrawTexturedScreenRect(&menu033Tex, 44.0f, 34.0f, 256.0f, 254.0f,
                                           0.0f, 0.000f, 1.0f, 0.402f);
                    /* BATTLE MODE card (sheet v 0.408-0.863) -> screen right */
                    DrawTexturedScreenRect(&menu033Tex, 344.0f, 34.0f, 256.0f, 254.0f,
                                           0.0f, 0.408f, 1.0f, 0.863f);
                    /* OPTIONS green pill (sheet full pill u 0.015-0.875, v 0.872-0.949) */
                    DrawTexturedScreenRect(&menu033Tex, 336.0f, 320.0f, 188.0f, 42.0f,
                                           0.015f, 0.872f, 0.875f, 0.949f);
                }
                if (haveMenu032) {
                    /* Quit button (chrome sheet u 0.0-0.162, v 0.40-0.74) */
                    DrawTexturedScreenRect(&menu032Tex, 528.0f, 322.0f, 92.0f, 42.0f,
                                           0.0f, 0.40f, 0.162f, 0.74f);
                    /* pointing-hand cursor at the selected item (u 0.185-0.285, v 0.66-0.99) */
                    DrawTexturedScreenRect(&menu032Tex,
                                           kMainMenuItems[menuCursor].handX,
                                           kMainMenuItems[menuCursor].handY,
                                           48.0f, 38.0f,
                                           0.185f, 0.66f, 0.285f, 0.99f);
                }
                /* Bottom description box: dark teal panel + lighter border with
                 * white text for the selected item (matches the real game). */
                DrawSolidScreenRect(46.0f, 380.0f, 548.0f, 84.0f, 96, 124, 142, 240);
                DrawSolidScreenRect(50.0f, 384.0f, 540.0f, 76.0f, 26, 44, 58, 245);
                DrawTextWrapped(68.0f, 400.0f, 12.0f, 18.0f, 40, 2,
                                228, 236, 244, 255,
                                kMainMenuItems[menuCursor].desc);

                /* Selection dialog over the (dimmed) menu. */
                if (sceneState == PCPORT_SCENE_DIALOG) {
                    DrawDialogBox(dialogText, dialogYesNo, dialogCursor);
                }
            }
        }

        /* On the final frame, capture the framebuffer (BMP dump happens inside
         * ReadBackbufferImage via DumpFramebufferBMP when PCPORT_DUMP is set)
         * before presenting. */
        if (dumpRequested && frameCap > 0 && frame == frameCap - 1) {
            unsigned char* pixels = ReadBackbufferImage();
            free(pixels);
        }
        if (seqBase != NULL && (frame % seqEvery) == 0) {
            char seqPath[1024];
            snprintf(seqPath, sizeof(seqPath), "%s_%04d.bmp", seqBase, frame);
            DumpBackbufferTo(seqPath);
        }

        GSgfxSwapBuffers(1);

        if (frame == 0) {
            printf("[pcport_bootstrap] Menu scene frame 0 walked (joints=%u dobjs=%u drawn=%u skipped=%u textured=%u materialOnly=%u)\n",
                   stats.joints,
                   stats.dobjs,
                   stats.drawn,
                   stats.skipped,
                   stats.textured,
                   stats.materialOnly);
        }
    }

    GSgfxHostClearPipelineState(PCPORT_REAL_TEXTURED_PIPELINE);
    GSgfxHostClearPipelineState(PCPORT_REAL_MATERIAL_PIPELINE);
    GXHostClearTextureBinding();
    GXHostSetVertexAlphaScale(1.0f);
    ok = 1;

cleanup:
    HoldWindowOpen(window);
    PCPort_HSDArchiveDestroy(&archive);
    PCPort_FreeBuffer(memberData);
    PCPort_HSDArchiveDestroy(&logoArchive);
    PCPort_FreeBuffer(logoData);
    PCPort_FreeBuffer(logoPixels);
    PCPort_HSDArchiveDestroy(&menuBgArchive);
    PCPort_FreeBuffer(menuBgData);
    return ok;
}

/* Make the working directory the asset root so the game's relative asset paths
 * (orig/GC6E01/disc/files/...) resolve no matter where the exe is launched from
 * (double-clicked, or run from build_pc/). Walks up from the exe's own directory
 * looking for orig/GC6E01/disc/files/title.fsys and chdir()s there. */
static void PCPort_ChdirToAssetRoot(const char* argv0) {
    char dir[1024];
    char probe[1152];
    size_t len;
    int i;
    int slash;

    if (argv0 == NULL) {
        return;
    }
    len = strlen(argv0);
    if (len == 0 || len >= sizeof(dir)) {
        return;
    }
    memcpy(dir, argv0, len + 1);
    slash = -1;
    for (i = (int)len - 1; i >= 0; --i) {
        if (dir[i] == '/' || dir[i] == '\\') {
            slash = i;
            break;
        }
    }
    if (slash < 0) {
        dir[0] = '.';
        dir[1] = '\0';
    } else {
        dir[slash] = '\0';   /* directory containing the exe */
    }

    for (i = 0; i < 7; ++i) {
        FILE* f;
        size_t dl;

        snprintf(probe, sizeof(probe),
                 "%s/orig/GC6E01/disc/files/title.fsys", dir);
        f = fopen(probe, "rb");
        if (f != NULL) {
            fclose(f);
            if (_chdir(dir) == 0) {
                printf("[pcport_bootstrap] asset root: %s\n", dir);
            }
            return;
        }
        dl = strlen(dir);
        if (dl + 4 >= sizeof(dir)) {
            return;
        }
        memcpy(dir + dl, "/..", 4);   /* go up one level */
    }
}

/* THP decode smoke (no GL): decode one frame of a movie to RGBA via thp_player +
 * stb_image and write it as a PPM, to verify the decode path in the host build
 * independent of the GL present path. PCPORT_THP_FILE selects the movie (default
 * gs_logo), PCPORT_THP_FRAME the frame index, PCPORT_THP_OUT the output path. */
static int RunTHPSmoke(void) {
    const char* path = getenv("PCPORT_THP_FILE");
    const char* frameEnv = getenv("PCPORT_THP_FRAME");
    const char* outPath = getenv("PCPORT_THP_OUT");
    int wantFrame = (frameEnv != NULL && frameEnv[0] != '\0') ? atoi(frameEnv) : 0;
    const unsigned char* rgba = NULL;
    PCPortTHP* thp;
    FILE* out;
    int w;
    int h;
    int f;

    if (path == NULL || path[0] == '\0') {
        path = "orig/GC6E01/disc/files/movie/gs_logo.thp";
    }
    if (outPath == NULL || outPath[0] == '\0') {
        outPath = "build_pc/thp_smoke.ppm";
    }
    thp = PCPortTHP_Open(path);
    if (thp == NULL) {
        fprintf(stderr, "[thp-smoke] failed to open %s\n", path);
        return 0;
    }
    w = PCPortTHP_Width(thp);
    h = PCPortTHP_Height(thp);
    printf("[thp-smoke] %s: %dx%d, %d frames, %.2f fps\n",
           path, w, h, PCPortTHP_FrameCount(thp), PCPortTHP_Fps(thp));
    for (f = 0; f <= wantFrame; ++f) {
        if (!PCPortTHP_NextFrameRGBA(thp, &rgba)) {
            fprintf(stderr, "[thp-smoke] decode stopped before frame %d\n", wantFrame);
            PCPortTHP_Close(thp);
            return 0;
        }
    }
    out = fopen(outPath, "wb");
    if (out == NULL) {
        fprintf(stderr, "[thp-smoke] cannot write %s\n", outPath);
        PCPortTHP_Close(thp);
        return 0;
    }
    {
        int i;
        fprintf(out, "P6\n%d %d\n255\n", w, h);
        for (i = 0; i < w * h; ++i) {
            fwrite(rgba + (size_t)i * 4, 1, 3, out); /* RGB, drop alpha */
        }
    }
    fclose(out);
    printf("[thp-smoke] wrote %s (frame %d, %dx%d)\n", outPath, wantFrame, w, h);
    PCPortTHP_Close(thp);
    return 1;
}

/* BGM streaming smoke (no GL): init the BGM subsystem, locate + start a music
 * member from an FSYS archive, and pump the per-frame update loop so the
 * waveOut device streams it. PCPORT_BGM_FSYS / PCPORT_BGM_MEMBER select the
 * source (defaults to common.fsys / snd_music_atmos_pool); PCPORT_BGM_SECS sets
 * how long to pump (default 3s). Verifies the bgm_host module links + runs end
 * to end against real disc data. Honest about the MusyX gap (plays the first
 * pool wave, not a full sequenced track) -- see bgm_host.h. */
static int RunBGMSmoke(void) {
    const char* fsysPath = getenv("PCPORT_BGM_FSYS");
    const char* member   = getenv("PCPORT_BGM_MEMBER");
    const char* secsEnv  = getenv("PCPORT_BGM_SECS");
    int secs = (secsEnv != NULL && secsEnv[0] != '\0') ? atoi(secsEnv) : 3;
    int i, iters;
    if (fsysPath == NULL || fsysPath[0] == '\0') {
        fsysPath = "orig/GC6E01/disc/files/common.fsys";
    }
    if (member == NULL || member[0] == '\0') {
        member = "snd_music_atmos_pool";
    }
    if (secs < 1) secs = 1;
    if (!PCPortBGM_Init()) {
        fprintf(stderr, "[bgm-smoke] PCPortBGM_Init failed (no audio device?)\n");
        return 0;
    }
    if (!PCPortBGM_PlayFromFsys(fsysPath, member)) {
        fprintf(stderr, "[bgm-smoke] PlayFromFsys(%s, %s) failed -- member not found "
                "or pool format not parseable (MusyX gap, expected for some archives)\n",
                fsysPath, member);
        return 0;
    }
    printf("[bgm-smoke] streaming %s :: %s\n", fsysPath, member);
    /* Pump the update loop to exercise the decode/submit path end to end. The
     * waveOut sink plays asynchronously; this verifies the module links + runs +
     * keeps the buffer fed without crashing. (Real-time-audible playback is
     * driven from the game frame loop, not this headless smoke.) */
    iters = secs * 60;
    for (i = 0; i < iters && PCPortBGM_IsPlaying(); ++i) {
        PCPortBGM_Update();
    }
    PCPortBGM_Stop();
    printf("[bgm-smoke] done (%d update pumps, member streamed OK)\n", i);
    return 1;
}

/* MusyX wave smoke (no GL): decode + play the first wave of a MusyX sdir/samp
 * group via the standalone musyx_wave module. Defaults to snd_se_motion (the
 * verified single-entry group); PCPORT_MUSYX_FSYS / _SDIR / _POOL / _SAMP and
 * PCPORT_MUSYX_SECS override. Verifies the .sdir parse + .samp DSP-ADPCM decode
 * + WaveOut submit end to end against real disc data. */
static int RunMusyXSmoke(void) {
    const char* fsysPath = getenv("PCPORT_MUSYX_FSYS");
    const char* sdir     = getenv("PCPORT_MUSYX_SDIR");
    const char* pool     = getenv("PCPORT_MUSYX_POOL");
    const char* samp     = getenv("PCPORT_MUSYX_SAMP");
    const char* secsEnv  = getenv("PCPORT_MUSYX_SECS");
    int secs = (secsEnv != NULL && secsEnv[0]) ? atoi(secsEnv) : 3;
    volatile double spin; long i;
    if (fsysPath == NULL || fsysPath[0] == '\0') fsysPath = "orig/GC6E01/disc/files/common.fsys";
    if (sdir == NULL || sdir[0] == '\0') sdir = "snd_se_motion_sdir";
    if (pool == NULL || pool[0] == '\0') pool = "snd_se_motion_pool";
    if (samp == NULL || samp[0] == '\0') samp = "orig/GC6E01/disc/files/sound/snd_se_motion.samp";
    if (secs < 1) secs = 1;
    if (!MusyX_PlayWave(fsysPath, sdir, pool, samp)) {
        fprintf(stderr, "[musyx-smoke] MusyX_PlayWave failed\n");
        return 0;
    }
    printf("[musyx-smoke] streaming %s :: %s + %s (~%ds)\n", fsysPath, sdir, samp, secs);
    /* Keep the process alive briefly so the async WaveOut buffer plays out (no
     * portable sleep here; a coarse CPU spin is fine for a one-shot smoke). */
    spin = 0.0;
    for (i = 0; i < (long)secs * 40000000L; ++i) { spin += (double)i * 1e-9; }
    (void)spin;
    MusyX_StopWave();
    printf("[musyx-smoke] done\n");
    return 1;
}

/* THP audio decode smoke (no GL): decode ALL frames' audio of a movie to a WAV,
 * to verify the GC DSP-ADPCM decode without needing to listen. PCPORT_THP_FILE
 * selects the movie (default tpc.thp = the first audio-bearing boot movie),
 * PCPORT_THP_OUT the WAV path. Reports duration + RMS + clip% so correctness is
 * checkable from the numbers. */
static int RunTHPAudioSmoke(void) {
    const char* path = getenv("PCPORT_THP_FILE");
    const char* outPath = getenv("PCPORT_THP_OUT");
    const unsigned char* rgba = NULL;
    PCPortTHP* thp;
    short* acc;
    unsigned int total, written = 0, ch, sr, i;
    double sumsq = 0.0;
    unsigned int clipped = 0, nsamp;

    if (path == NULL || path[0] == '\0') {
        path = "orig/GC6E01/disc/files/movie/tpc.thp";
    }
    if (outPath == NULL || outPath[0] == '\0') {
        outPath = "build_pc/thp_audio_smoke.wav";
    }
    thp = PCPortTHP_Open(path);
    if (thp == NULL) {
        fprintf(stderr, "[thp-audio] failed to open %s\n", path);
        return 0;
    }
    if (!PCPortTHP_HasAudio(thp)) {
        fprintf(stderr, "[thp-audio] %s has no audio component\n", path);
        PCPortTHP_Close(thp);
        return 0;
    }
    ch = (unsigned int)PCPortTHP_AudioChannels(thp);
    sr = PCPortTHP_AudioSampleRate(thp);
    total = PCPortTHP_AudioTotalSamples(thp);
    printf("[thp-audio] %s: %u ch, %u Hz, %u total samples (%.2fs)\n",
           path, ch, sr, total, sr ? (double)total / (double)sr : 0.0);
    acc = (short*)malloc((size_t)(total + 4096u) * ch * sizeof(short));
    if (acc == NULL) { PCPortTHP_Close(thp); return 0; }
    while (PCPortTHP_NextFrameRGBA(thp, &rgba)) {  /* must run first per frame */
        const short* pcm = NULL;
        unsigned int nfr = 0;
        if (PCPortTHP_NextFrameAudioPCM(thp, &pcm, &nfr) && pcm && nfr) {
            if (written + nfr <= total + 4096u) {
                memcpy(acc + (size_t)written * ch, pcm, (size_t)nfr * ch * sizeof(short));
                written += nfr;
            }
        }
    }
    nsamp = written * ch;
    for (i = 0; i < nsamp; ++i) {
        sumsq += (double)acc[i] * (double)acc[i];
        if (acc[i] >= 32767 || acc[i] <= -32767) ++clipped;
    }
    thp_audio_write_wav(outPath, acc, written, (int)ch, sr);
    free(acc);
    PCPortTHP_Close(thp);
    printf("[thp-audio] wrote %s: %u frames (%.2fs), RMS=%.0f, clip=%.2f%%\n",
           outPath, written, sr ? (double)written / (double)sr : 0.0,
           nsamp ? sqrt(sumsq / (double)nsamp) : 0.0,
           nsamp ? 100.0 * (double)clipped / (double)nsamp : 0.0);
    return 1;
}

/* ===================================================================
 * P-B inc2: real title-scene render driven by the engine scheduler.
 *
 * Additive, self-contained title load + per-frame 3D render so a GStask callback
 * (engine_boot.c BootTaskVBlank) can present the REAL title scene graph through
 * the game's own draw bridge (RenderJointTree -> fn_800DAD10) — i.e. real engine
 * render code executing under the host GStask/GSthread scheduler. RunMenuScene is
 * left untouched; these reuse the same static helpers + camera/joint setup.
 * =================================================================== */
static PCPortHSDArchive       g_engTitleArchive;
static PCPortTranslatedCamera g_engTitleCamera;
static u32  g_engTitleRootJoint;
static int  g_engTitleReady;
/* Field player character (the skinned Wes avatar, ken_b1). Loaded once into its
 * own archive; rendered each frame at the walk-loop player position. */
static PCPortHSDArchive       g_engCharArchive;
static u32  g_engCharRoot;
static int  g_engCharLoaded;
static char g_engCharFsysPath[260];
static char g_engCharMember[64];
/* First real field NPC model loaded from a floor archive dependency list. Kept
 * separate from Wes because the current character animation host owns one
 * global motion-bank state. */
static PCPortHSDArchive       g_engNpcArchive;
static u32  g_engNpcRoot;
static int  g_engNpcLoaded;
static char g_engNpcFsysPath[260];
static char g_engNpcMember[64];
/* Field maps pack SEVERAL model sets in the scene branch (e.g. D1_garage_1F:
 * +0 room walls/props, +4 + +8 additional sets incl. the FLOOR). We render the
 * primary (g_engTitleRootJoint) plus every additional model root here. */
#define PCPORT_MAX_SCENE_MODELS 8
static u32  g_engExtraRootJoints[PCPORT_MAX_SCENE_MODELS];
static int  g_engExtraRootJointCount;
/* Model-view auto-camera (PCPort_EngineFieldSetup computes a framed eye/center
 * for a loaded character model; RunFieldScene seeds its free-fly camera from
 * these so the model is centered instead of the free-fly default eye). */
static int  g_engModelView;
static f32  g_engModelViewEye[3];
static f32  g_engModelViewCenter[3];
static int  g_engFieldGfxInitialized;
/* Field mode: GSgfx_BeginFrame paints a green EFB clear-quad that, on a sparse field
 * map, shows through where geometry doesn't cover (the title covers it with its sky/
 * ground). In field mode we re-clear to a chosen background AFTER GSgfx_BeginFrame. */
static int  g_engFieldMode;
static f32  g_engFieldBg[3] = { 0.04f, 0.05f, 0.08f }; /* near-black; PCPORT_FIELD_BG overrides */

static int PCPort_FieldAnimAllowedForArchive(const char* archivePath) {
    if (getenv("PCPORT_NO_FIELD_ANIM") != NULL) {
        return 0;
    }
    /* S1_shop_1F's scene renders statically, but its ambient animjoint path
     * currently trips the host swizzle/JObj animation setup. Keep the playtest
     * warp gate moving and revisit this when field-anim coverage broadens past
     * the verified S1_out sign/cloud path. */
    if (archivePath != NULL && strstr(archivePath, "S1_shop_1F") != NULL) {
        return 0;
    }
    return 1;
}

static void PCPort_EngineSceneRelease(void) {
    PCPort_FieldAnimRelease();
    if (g_engTitleArchive.storage != NULL) {
        PCPort_HSDArchiveDestroy(&g_engTitleArchive);
    }
    memset(&g_engTitleCamera, 0, sizeof(g_engTitleCamera));
    g_engTitleRootJoint = 0;
    g_engExtraRootJointCount = 0;
    g_engModelView = 0;
    g_engTitleReady = 0;
}

int PCPort_EngineTitleSetup(void) {
    u8* memberData = NULL;
    u32 memberSize = 0;
    const u8* sceneData;
    u32 sceneOffset = 0;
    u32 sceneBranchOffset, cameraDescOffset, sceneJointListOffset;
    /* title END pose (cam_logo_demo_stop end-frame), same as RunMenuScene. */
    static const f32 titleEye[3] = { 0.0f, 38.905f, 409.812f };
    static const f32 titleInt[3] = { 0.0f, 39.6514f, 1.5625f };
    static const f32 titleUp[3]  = { 0.0f, 1.0f, 0.0f };

    if (g_engTitleReady && !g_engFieldMode) {
        return 1;
    }
    PCPort_EngineSceneRelease();
    g_engFieldMode = 0;   /* title covers the screen; keep the engine's own clear */

    if (!PCPort_LoadFsysMember(PCPORT_TITLE_SCENE_ARCHIVE, PCPORT_TITLE_SCENE_MEMBER,
                               &memberData, &memberSize)) {
        fprintf(stderr, "[boot] title scene load failed\n");
        return 0;
    }
    if (!PCPort_HSDArchiveParseBE(&g_engTitleArchive, memberData, memberSize)) {
        fprintf(stderr, "[boot] title scene parse failed\n");
        return 0;
    }
    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&g_engTitleArchive,
                                                             "scene_data", &sceneOffset);
    if (sceneData == NULL) {
        fprintf(stderr, "[boot] title scene_data unresolved\n");
        return 0;
    }
    sceneBranchOffset = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&g_engTitleArchive, sceneBranchOffset, 0x10u)) {
        fprintf(stderr, "[boot] title scene branch invalid (0x%X)\n", sceneBranchOffset);
        return 0;
    }
    cameraDescOffset = PCPort_ReadBigEndianU32(g_engTitleArchive.storage + sceneBranchOffset + 0x08);
    if (!PCPort_TranslatePerspectiveCameraFromArchiveBE(&g_engTitleArchive,
                                                        cameraDescOffset, &g_engTitleCamera)) {
        fprintf(stderr, "[boot] title camera translate failed\n");
        return 0;
    }
    BuildViewMatrixLookAt(titleEye, titleInt, titleUp, g_engTitleCamera.viewMatrix);

    sceneJointListOffset = PCPort_ReadBigEndianU32(g_engTitleArchive.storage + sceneBranchOffset + 0x00);
    g_engTitleRootJoint  = PCPort_ReadBigEndianU32(g_engTitleArchive.storage + sceneJointListOffset + 0x00);
    g_engExtraRootJointCount = 0; /* title = single model set */
    if (!ArchiveRangeValid(&g_engTitleArchive, g_engTitleRootJoint, PCPORT_SERIALIZED_JOINT_SIZE)) {
        fprintf(stderr, "[boot] title root joint invalid (0x%X)\n", g_engTitleRootJoint);
        return 0;
    }

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    g_engFieldGfxInitialized = 0;
    g_engTitleReady = 1;
    printf("[boot] title scene loaded for engine render (rootJoint=0x%X)\n", g_engTitleRootJoint);
    return 1;
}

int PCPort_EngineTitleReady(void) {
    return g_engTitleReady;
}

/* Public wrapper so engine_boot.c can dump the backbuffer for headless verify. */
void PCPort_DumpBackbuffer(const char* path) {
    DumpBackbufferTo(path);
}

/* ---- P-C step 1: field/overworld map static render ----
 * Field maps (D1_, D2_, M1_ ... .fsys) are HSD scene archives just like the title:
 * the renderable geometry is the largest member exposing a "scene_data" public
 * symbol. Load it into the SAME g_engTitle* state + reuse PCPort_EngineTitleRenderFrame
 * so the existing RenderJointTree -> fn_800DAD10 path draws the map. This is the
 * first step toward a walkable overworld (collision/WZX + player update come later). */
void PCPort_EngineTitleRenderFrame(void); /* defined just below; reused for the field */
int PCPort_EngineFieldSetup(const char* archivePath) {
    u8* memberData = NULL;
    u32 memberSize = 0;
    const u8* sceneData;
    u32 sceneOffset = 0;
    u32 sceneBranchOffset, cameraDescOffset, sceneJointListOffset;
    int haveCam = 0;

    PCPort_EngineSceneRelease();
    g_engFieldMode = 1;   /* re-clear GSgfx_BeginFrame's green EFB quad to the bg */
    {
        const char* bg = getenv("PCPORT_FIELD_BG");
        if (bg != NULL) {
            sscanf(bg, "%f,%f,%f", &g_engFieldBg[0], &g_engFieldBg[1], &g_engFieldBg[2]);
        }
    }

    /* PCPORT_FIELD_MEMBER loads a specific named member (e.g. a people_archive
     * character model) instead of the auto-selected largest scene_data member. */
    {
        const char* fm = getenv("PCPORT_FIELD_MEMBER");
        BOOL loaded;
        if (fm != NULL && fm[0] != '\0') {
            loaded = PCPort_LoadFsysMember(archivePath, fm, &memberData, &memberSize);
        } else {
            loaded = PCPort_LoadFsysSceneMember(archivePath, &memberData, &memberSize);
        }
        if (!loaded) {
            fprintf(stderr, "[field] no scene_data member in %s\n", archivePath);
            return 0;
        }
    }
    if (!PCPort_HSDArchiveParseBE(&g_engTitleArchive, memberData, memberSize)) {
        fprintf(stderr, "[field] archive parse failed (%s)\n", archivePath);
        return 0;
    }
    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&g_engTitleArchive,
                                                             "scene_data", &sceneOffset);
    if (sceneData == NULL) {
        fprintf(stderr, "[field] scene_data unresolved\n");
        return 0;
    }
    sceneBranchOffset = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&g_engTitleArchive, sceneBranchOffset, 0x10u)) {
        fprintf(stderr, "[field] scene branch invalid (0x%X)\n", sceneBranchOffset);
        return 0;
    }

    /* One-shot scene-branch audit (PCPORT_FIELD_DEBUG): the GS scene branch packs
     * several slots; we render only slot +0 (jointList->rootJoint). If the floor
     * is a SEPARATE model set in another slot, it shows up here. For each word
     * that looks like a data pointer, probe whether it's a jointList (its +0 is a
     * valid joint = a candidate model set) so additional models can be rendered. */
    if (getenv("PCPORT_FIELD_DEBUG") != NULL) {
        u32 w;
        printf("[field-branch] branch@0x%X:\n", sceneBranchOffset);
        for (w = 0; w < 0x28u; w += 4u) {
            u32 v = PCPort_ReadBigEndianU32(g_engTitleArchive.storage + sceneBranchOffset + w);
            int isPtr = ArchiveRangeValid(&g_engTitleArchive, v, 0x4u);
            printf("[field-branch]   +0x%-2X = 0x%08X%s", w, v, isPtr ? " *" : "");
            if (isPtr) {
                /* treat v as a jointList: jointList+0 = rootJoint candidate */
                u32 rj = PCPort_ReadBigEndianU32(g_engTitleArchive.storage + v + 0x00);
                if (ArchiveRangeValid(&g_engTitleArchive, rj, PCPORT_SERIALIZED_JOINT_SIZE)) {
                    u32 rjDobj = PCPort_ReadBigEndianU32(g_engTitleArchive.storage + rj + 0x10);
                    printf("  -> [jointList? rootJoint=0x%X dobj=0x%X]", rj, rjDobj);
                }
            }
            printf("\n");
        }
    }

    /* Camera: try the map's embedded camera (scene branch +0x08, same layout as
     * the title). If it doesn't resolve, synthesize a generous default look-at so
     * something draws; PCPORT_CAM_EYE/INT/UP override either way (for framing). */
    /* The scene branch is a NULL-terminated list of model-set jointLists from
     * +0; the camera desc immediately follows the terminator. D1_garage_1F has
     * 3 models (+0/+4/+8), null at +0xC, camera at +0x10; the title has 1 model
     * (+0), null at +4, camera at +8. The old code hardcoded +0x08, which only
     * framed single-model scenes -- multi-model field maps fell to the default. */
    {
        u32 modelSlots = 0u, s;
        for (s = 0u; s <= 0x20u; s += 0x4u) {
            u32 jl = PCPort_ReadBigEndianU32(g_engTitleArchive.storage + sceneBranchOffset + s);
            u32 rj;
            if (!ArchiveRangeValid(&g_engTitleArchive, jl, 0x4u)) break;
            rj = PCPort_ReadBigEndianU32(g_engTitleArchive.storage + jl + 0x00);
            if (!ArchiveRangeValid(&g_engTitleArchive, rj, PCPORT_SERIALIZED_JOINT_SIZE)) break;
            modelSlots++;
        }
        cameraDescOffset = PCPort_ReadBigEndianU32(
            g_engTitleArchive.storage + sceneBranchOffset +
            (modelSlots > 0u ? (modelSlots + 1u) * 4u : 0x08u));
    }
    if (cameraDescOffset != 0 &&
        ArchiveRangeValid(&g_engTitleArchive, cameraDescOffset, 0x4u) &&
        PCPort_TranslatePerspectiveCameraFromArchiveBE(&g_engTitleArchive,
                                                       cameraDescOffset, &g_engTitleCamera)) {
        haveCam = 1;
    }
    if (!haveCam) {
        /* Default perspective + viewport for a bare map (no embedded camera). */
        f32 eye[3] = { 0.0f, 250.0f, 600.0f };
        f32 interest[3] = { 0.0f, 0.0f, 0.0f };
        f32 up[3] = { 0.0f, 1.0f, 0.0f };
        memset(&g_engTitleCamera, 0, sizeof(g_engTitleCamera));
        g_engTitleCamera.viewportLeft = 0; g_engTitleCamera.viewportTop = 0;
        g_engTitleCamera.viewportRight = (u16)PCPORT_WINDOW_WIDTH;
        g_engTitleCamera.viewportBottom = (u16)PCPORT_WINDOW_HEIGHT;
        g_engTitleCamera.scissorLeft = 0; g_engTitleCamera.scissorTop = 0;
        g_engTitleCamera.scissorRight = (u16)PCPORT_WINDOW_WIDTH;
        g_engTitleCamera.scissorBottom = (u16)PCPORT_WINDOW_HEIGHT;
        /* Standard GX perspective (MTXPerspective form), GXSetProjection-ready. */
        {
            f32 aspect = (f32)PCPORT_WINDOW_WIDTH / (f32)PCPORT_WINDOW_HEIGHT;
            f32 nz = 1.0f, fz = 20000.0f;
            f32 cot = 1.0f / (f32)tan(45.0 * 0.5 * 3.14159265358979 / 180.0);
            f32 (*m)[4] = g_engTitleCamera.projectionMatrix;
            memset(m, 0, sizeof(g_engTitleCamera.projectionMatrix));
            m[0][0] = cot / aspect;
            m[1][1] = cot;
            m[2][2] = fz / (nz - fz);
            m[2][3] = (fz * nz) / (nz - fz);
            m[3][2] = -1.0f;
        }
        BuildViewMatrixLookAt(eye, interest, up, g_engTitleCamera.viewMatrix);
        printf("[field] no embedded camera -> default look-at (tune with PCPORT_CAM_EYE/INT)\n");
    }

    /* PCPORT_CAM_EYE / PCPORT_CAM_INT (+ optional _UP) override the view (tuning). */
    {
        const char* ce = getenv("PCPORT_CAM_EYE");
        const char* ci = getenv("PCPORT_CAM_INT");
        const char* cu = getenv("PCPORT_CAM_UP");
        if (ce != NULL && ci != NULL) {
            f32 e[3] = {0,0,0}, in[3] = {0,0,0}, u[3] = {0,1,0};
            sscanf(ce, "%f,%f,%f", &e[0], &e[1], &e[2]);
            sscanf(ci, "%f,%f,%f", &in[0], &in[1], &in[2]);
            if (cu != NULL) sscanf(cu, "%f,%f,%f", &u[0], &u[1], &u[2]);
            BuildViewMatrixLookAt(e, in, u, g_engTitleCamera.viewMatrix);
            printf("[field] camera override eye=(%.1f,%.1f,%.1f) int=(%.1f,%.1f,%.1f)\n",
                   e[0], e[1], e[2], in[0], in[1], in[2]);
        }
    }

    sceneJointListOffset = PCPort_ReadBigEndianU32(g_engTitleArchive.storage + sceneBranchOffset + 0x00);
    g_engTitleRootJoint  = PCPort_ReadBigEndianU32(g_engTitleArchive.storage + sceneJointListOffset + 0x00);
    if (!ArchiveRangeValid(&g_engTitleArchive, g_engTitleRootJoint, PCPORT_SERIALIZED_JOINT_SIZE)) {
        fprintf(stderr, "[field] root joint invalid (0x%X)\n", g_engTitleRootJoint);
        return 0;
    }

    /* The scene branch holds CONTIGUOUS leading jointList pointers, one per model
     * set (D1_garage_1F: +0 walls/props, +4 + +8 more sets incl. the floor). The
     * field originally rendered only slot +0; collect every additional slot whose
     * target is a valid jointList (its +0 is a valid joint) so the whole room --
     * floor included -- renders. Stop at the first non-jointList slot. */
    g_engExtraRootJointCount = 0;
    {
        u32 slot;
        for (slot = 0x4u; slot <= 0x20u; slot += 0x4u) {
            u32 jl = PCPort_ReadBigEndianU32(g_engTitleArchive.storage + sceneBranchOffset + slot);
            u32 rj;
            if (!ArchiveRangeValid(&g_engTitleArchive, jl, 0x4u)) {
                break; /* contiguous run of model-set pointers ended */
            }
            rj = PCPort_ReadBigEndianU32(g_engTitleArchive.storage + jl + 0x00);
            if (!ArchiveRangeValid(&g_engTitleArchive, rj, PCPORT_SERIALIZED_JOINT_SIZE)) {
                break; /* not a jointList -> reached camera/light/fog slots */
            }
            if (g_engExtraRootJointCount < PCPORT_MAX_SCENE_MODELS) {
                g_engExtraRootJoints[g_engExtraRootJointCount++] = rj;
            }
        }
    }

    /* MODEL-VIEW AUTO-CAMERA: when viewing a specific archive member
     * (PCPORT_FIELD_MEMBER, e.g. a people_archive character) and the user has
     * NOT supplied a manual camera (PCPORT_CAM_EYE), automatically frame the
     * camera on the rendered model's center so any character model is centered
     * regardless of its origin (feet, hips, scale). The embedded field camera is
     * for full rooms, not single character models, so this overrides it for the
     * member-view case. Compute the world-space AABB over the primary rootJoint
     * AND every extra model-set root, then place the eye on +Z (slightly above
     * center) at a distance that fits the AABB to the vertical FOV. */
    g_engModelView = 0;
    if (getenv("PCPORT_FIELD_MEMBER") != NULL &&
        getenv("PCPORT_FIELD_MEMBER")[0] != '\0' &&
        getenv("PCPORT_CAM_EYE") == NULL) {
        f32 mn[3] = {0,0,0}, mx[3] = {0,0,0};
        int any = 0;
        int i;
        g_aabbDepth = 0; g_aabbVisitedCount = 0;
        AccumulateModelAABB(&g_engTitleArchive, g_engTitleRootJoint,
                            g_engTitleRootJoint, mn, mx, &any);
        for (i = 0; i < g_engExtraRootJointCount; ++i) {
            AccumulateModelAABB(&g_engTitleArchive, g_engExtraRootJoints[i],
                                g_engExtraRootJoints[i], mn, mx, &any);
        }
        if (any) {
            f32 center[3];
            f32 ext[3];
            f32 radius;
            f32 dist;
            f32 eye[3], up[3] = {0.0f, 1.0f, 0.0f};
            /* vertical half-FOV of the default 45-deg projection */
            f32 halfFovY = (f32)(45.0 * 0.5 * 3.14159265358979 / 180.0);
            f32 tanHalf = (f32)tan((double)halfFovY);
            center[0] = (mn[0] + mx[0]) * 0.5f;
            center[1] = (mn[1] + mx[1]) * 0.5f;
            center[2] = (mn[2] + mx[2]) * 0.5f;
            ext[0] = (mx[0] - mn[0]) * 0.5f;
            ext[1] = (mx[1] - mn[1]) * 0.5f;
            ext[2] = (mx[2] - mn[2]) * 0.5f;
            radius = ext[0];
            if (ext[1] > radius) radius = ext[1];
            if (ext[2] > radius) radius = ext[2];
            if (radius < 0.001f) radius = 1.0f;
            /* distance so the bounding sphere fits the vertical FOV, *1.6 margin */
            dist = (radius / (tanHalf > 0.001f ? tanHalf : 0.5f)) * 1.6f;
            if (dist < radius * 2.0f) dist = radius * 2.0f;
            /* PCPORT_CAM_AZ=<deg> orbits the framing eye around the model in the
             * XZ plane (diagnostic; 0 = straight-on front). Keeps the computed
             * distance + projection so the model stays framed at any angle. */
            {
                const char* azEnv = getenv("PCPORT_CAM_AZ");
                f32 az = (azEnv != NULL && azEnv[0]) ?
                         (f32)(atof(azEnv) * 3.14159265358979 / 180.0) : 0.0f;
                eye[0] = center[0] + dist * (f32)sin((double)az);
                eye[1] = center[1] + radius * 0.25f; /* slight elevation */
                eye[2] = center[2] + dist * (f32)cos((double)az);
            }
            /* Rebuild a clean default perspective projection (the member's own
             * embedded camera projection may be tuned for a different scene). */
            {
                f32 aspect = (f32)PCPORT_WINDOW_WIDTH / (f32)PCPORT_WINDOW_HEIGHT;
                f32 nz = (radius * 0.05f < 0.5f) ? 0.5f : radius * 0.05f;
                f32 fz = dist + radius * 8.0f + 100.0f;
                f32 cot = 1.0f / tanHalf;
                f32 (*m)[4] = g_engTitleCamera.projectionMatrix;
                memset(m, 0, sizeof(g_engTitleCamera.projectionMatrix));
                m[0][0] = cot / aspect;
                m[1][1] = cot;
                m[2][2] = fz / (nz - fz);
                m[2][3] = (fz * nz) / (nz - fz);
                m[3][2] = -1.0f;
                g_engTitleCamera.viewportLeft = 0; g_engTitleCamera.viewportTop = 0;
                g_engTitleCamera.viewportRight = (u16)PCPORT_WINDOW_WIDTH;
                g_engTitleCamera.viewportBottom = (u16)PCPORT_WINDOW_HEIGHT;
                g_engTitleCamera.scissorLeft = 0; g_engTitleCamera.scissorTop = 0;
                g_engTitleCamera.scissorRight = (u16)PCPORT_WINDOW_WIDTH;
                g_engTitleCamera.scissorBottom = (u16)PCPORT_WINDOW_HEIGHT;
            }
            BuildViewMatrixLookAt(eye, center, up, g_engTitleCamera.viewMatrix);
            /* Stash for RunFieldScene's free-fly camera (which otherwise rebuilds
             * the view from its own default eye each frame and hides the model). */
            g_engModelView = 1;
            g_engModelViewEye[0] = eye[0]; g_engModelViewEye[1] = eye[1]; g_engModelViewEye[2] = eye[2];
            g_engModelViewCenter[0] = center[0]; g_engModelViewCenter[1] = center[1]; g_engModelViewCenter[2] = center[2];
            printf("[field] model-view auto-camera: aabb=[%.2f,%.2f,%.2f .. "
                   "%.2f,%.2f,%.2f] center=(%.2f,%.2f,%.2f) radius=%.2f "
                   "eye=(%.2f,%.2f,%.2f) dist=%.2f\n",
                   mn[0], mn[1], mn[2], mx[0], mx[1], mx[2],
                   center[0], center[1], center[2], radius,
                   eye[0], eye[1], eye[2], dist);
        } else {
            printf("[field] model-view auto-camera: no geometry bounds found, "
                   "using default/embedded camera\n");
        }
    }

    /* Scene-ambient animation (signpost swing, cloud UV scroll, etc.):
     * load a SEPARATE copy of the same scene archive and arm the animjoint
     * tree so the live HSD_JObj tick does not corrupt the raw-BE storage that
     * RenderJointTree reads.  No-op (returns 0) for static maps.
     * After setup, register the render-side archive so PCPort_FieldAnimTick
     * can write updated SRT back into the storage RenderJointTree reads. */
    PCPort_FieldAnimRelease();
    if (PCPort_FieldAnimAllowedForArchive(archivePath)) {
        u32 animRootOff = 0u;
        const char* fm = getenv("PCPORT_FIELD_MEMBER");
        /* Use the same member selection logic as the render-side load above. */
        if (PCPort_FieldAnimSetup(archivePath,
                                  (fm != NULL && fm[0] != '\0') ? fm : NULL,
                                  &animRootOff) && animRootOff != 0u) {
            PCPort_FieldAnimSetRenderTarget(&g_engTitleArchive, animRootOff);
        }
    } else if (getenv("PCPORT_FIELD_DEBUG") != NULL) {
        printf("[field-anim] skipped for %s\n", archivePath);
    }

    if (!g_engFieldGfxInitialized) {
        GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
        g_engFieldGfxInitialized = 1;
    }
    g_engTitleReady = 1;
    printf("[field] map loaded for render: %s (scene member 0x%X bytes, rootJoint=0x%X, "
           "extraModels=%d, cam=%s)\n",
           archivePath, memberSize, g_engTitleRootJoint, g_engExtraRootJointCount,
           haveCam ? "embedded" : "default");
    return 1;
}

/* --field: static field-map render loop. PCPORT_FIELD_ARCHIVE picks the map
 * (default D1_garage_1F = Wes's hideout, the game's start). */
/* Debug overlay: draw the loaded WZX collision mesh as world-space wireframe
 * lines over the field render, so the parsed triangles can be visually checked
 * against the rendered room geometry. Walkable surfaces (floor/slope/extfloor)
 * are green, blocking surfaces (walls/boundary) pink, ceiling grey. Uses the
 * current camera's view matrix (model = identity); depth-tests against the scene
 * but does not write depth. Gated by PCPORT_COL_WIRE. */
static void DrawFieldCollisionWire(void) {
    int n = PCPort_FieldColTriCount();
    int i, cat;
    f32 t9[9];

    if (n <= 0) {
        return;
    }

    GXSetProjection(g_engTitleCamera.projectionMatrix, GX_PERSPECTIVE);
    GXLoadPosMtxImm(g_engTitleCamera.viewMatrix, 0);
    GXSetCullMode(GX_CULL_NONE);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_COPY);
    GXSetNumTexGens(1);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXHostSetLightingEnabled(GX_FALSE);
    GXHostSetVertexAlphaScale(1.0f);

    GXBegin(GX_LINES, GX_VTXFMT0, (u16)(n * 6));
    for (i = 0; i < n; ++i) {
        u8 r, g, b;
        int e;
        if (!PCPort_FieldColGetTri(i, t9, &cat)) {
            continue;
        }
        if (cat == PCPORT_COLCAT_WALL || cat == PCPORT_COLCAT_BOUND) {
            r = 255; g = 60; b = 200;       /* blocking -> pink */
        } else if (cat == PCPORT_COLCAT_CEIL) {
            r = 130; g = 130; b = 130;      /* ceiling -> grey */
        } else {
            r = 60; g = 255; b = 90;        /* walkable -> green */
        }
        for (e = 0; e < 3; ++e) {
            int a0 = e, a1 = (e + 1) % 3;
            GXColor4u8(r, g, b, 255);
            GXPosition3f32(t9[a0 * 3 + 0], t9[a0 * 3 + 1], t9[a0 * 3 + 2]);
            GXTexCoord2f32(0.0f, 0.0f);
            GXColor4u8(r, g, b, 255);
            GXPosition3f32(t9[a1 * 3 + 0], t9[a1 * 3 + 1], t9[a1 * 3 + 2]);
            GXTexCoord2f32(0.0f, 0.0f);
        }
    }
    GXEnd();

    GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
}

/* Placeholder player avatar: an oriented box (a stand-in until the real Wes
 * overworld model is ported). Drawn in world space via the immediate path; the
 * front face is brightened so the facing direction reads. yaw=0 faces -Z. */
/* =================================================================== *
 *  MAP WARPS: floor-id -> fsys-name table + door-trigger reload
 * =================================================================== *
 * Walk through a door -> load the connected map. The trigger geometry (door
 * positions / approach cones) and the floor-id<->map mapping are RE-derived
 * (see the MAP WARPS scope plan + field_collision.h). The real per-room exit
 * records are NOT statically locatable in the fsys (probe_exits.py found no
 * coherent 0x2C-stride table -- they're a runtime SDA r13 array filled by the
 * asm people subsystem). So the MVP drives warps from a HOST-SPECIFIED exit
 * list per map (a trigger box at a known door position), proving the warp
 * mechanism end-to-end; PCPort_LoadFsysExitData stays the integration point
 * for the future real exit parse. */

typedef struct {
    int          floorId;     /* host floor id (table index identity) */
    const char*  fsysName;    /* bare member name -> orig/.../<name>.fsys */
    /* hand-specified exits for this map (test triggers). */
    PCPortFieldExit exits[4];
    int          exitCount;
    f32          defaultSpawn[3];  /* fallback spawn if an exit gives none */
} PCPortWarpMapEntry;

/* Floor-id <-> map table. Hand-built for the reachable D1_garage_* cluster
 * (Wes's hideout, the game start). Door positions use the WZX collision bounds
 * for each map (D1_garage_1F = X[-106,105] Y[-24,21] Z[-68,78]). Reciprocal
 * exits let you warp 1F <-> B1 and back. Tunable; the real door data is a
 * follow-up. PCPORT_WARP_TUNE can override the first exit at runtime. */
enum {
    PC_FLOOR_GARAGE_1F = 0,
    PC_FLOOR_GARAGE_B1 = 1,
    PC_FLOOR_OUTSKIRT = 2,
    PC_FLOOR_OUTSKIRT_SHOP = 3,
    PC_FLOOR_PYRITE = 4,
    PC_FLOOR_AGATE = 5
};

static const PCPortWarpMapEntry g_pcWarpMaps[] = {
    {
        /* The Outskirt Stand exterior — the game's first walkable scene and the
         * New Game spawn map. The train-car diner sits to one side; Wes spawns in
         * the open desert plaza in front of it. The host shop-door trigger is
         * represented by overlapping samples: the original smoke autorun path,
         * the decoded door/NPC marker lane, and the visible train-car doorway
         * threshold. The real exit table remains a follow-up.
         * Collision bounds X[-146,231] Y[-10,64] Z[-127,127]. */
        PC_FLOOR_OUTSKIRT, "S1_out",
        {
            { { 69.0f, 0.0f, -30.0f }, 0.0f,
              18.0f, 0.0f, PC_FLOOR_OUTSKIRT_SHOP, { 0.0f, 0.0f, 35.0f } },
            { { 56.5f, 0.0f, 10.6f }, 0.0f,
              16.0f, 0.0f, PC_FLOOR_OUTSKIRT_SHOP, { 0.0f, 0.0f, 35.0f } },
            { { 82.0f, 0.0f, 8.0f }, 0.0f,
              24.0f, 0.0f, PC_FLOOR_OUTSKIRT_SHOP, { 0.0f, 0.0f, 35.0f } },
            { { 106.0f, 0.0f, -4.0f }, 0.0f,
              28.0f, 0.0f, PC_FLOOR_OUTSKIRT_SHOP, { 0.0f, 0.0f, 35.0f } }
        },
        4,
        { 47.7f, 0.0f, 79.9f }   /* spawn point: just behind the gas-pump foundation
                                   * (concrete pad under the green pump, right side of
                                   * view).  Dialled in by the user walking Wes to the
                                   * spot in the live port; final coords from the
                                   * [field/walk] exit log were (47.7, 0.0, 79.9). */
    },
    {
        PC_FLOOR_GARAGE_1F, "D1_garage_1F",
        {
            /* a door at the far (+Z) edge of the room -> down to B1. Spawning
             * INTO 1F (from B1) lands at Z=-30, well clear of this door's
             * radius so it doesn't instantly re-trigger. */
            { { 0.0f, 0.0f, 65.0f }, 0.0f /*any approach dir (MVP)*/,
              26.0f, 0.0f, PC_FLOOR_GARAGE_B1, { 0.0f, 0.0f, 0.0f } }
        },
        1,
        { 0.0f, 0.0f, -30.0f }   /* spawn point when arriving in 1F */
    },
    {
        PC_FLOOR_GARAGE_B1, "D1_garage_B1",
        {
            /* a door back up to 1F near the room's near (-Z) edge. */
            { { 0.0f, 0.0f, -55.0f }, 0.0f, 26.0f, 0.0f,
              PC_FLOOR_GARAGE_1F, { 0.0f, 0.0f, 0.0f } }
        },
        1,
        { 0.0f, 0.0f, 0.0f }     /* spawn point when arriving in B1 */
    },
    {
        PC_FLOOR_OUTSKIRT_SHOP, "S1_shop_1F",
        {
            /* Interior doorway back to the Outskirt exterior. Static render +
             * WZX collision are verified; ambient field animation is skipped for
             * this map until its animjoint setup is recovered. */
            { { 0.0f, 0.0f, -30.0f }, 0.0f, 18.0f, 0.0f,
              PC_FLOOR_OUTSKIRT, { 69.0f, 0.0f, -48.0f } }
        },
        1,
        { 0.0f, 0.0f, 35.0f }    /* spawn point when arriving in the shop */
    },
    {
        /* Pyrite Town exterior. Verified render/collision target for the
         * worldmap travel smoke: M2_out has 2125 WZX triangles. Real exits are
         * still future floor-event data, so this entry intentionally has no
         * host exit triggers yet. */
        PC_FLOOR_PYRITE, "M2_out",
        { { { 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f, 0.0f, -1,
              { 0.0f, 0.0f, 0.0f } } },
        0,
        { 0.0f, 0.0f, 0.0f }
    },
    {
        /* Agate/The Under map-group exterior. M3_out renders and has WZX
         * collision; real story availability and exit records remain follow-up. */
        PC_FLOOR_AGATE, "M3_out",
        { { { 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f, 0.0f, -1,
              { 0.0f, 0.0f, 0.0f } } },
        0,
        { 0.0f, 0.0f, 0.0f }
    }
};
static const int g_pcWarpMapCount =
    (int)(sizeof(g_pcWarpMaps) / sizeof(g_pcWarpMaps[0]));
static int g_pcCurrentFieldFloor = -1;

static const PCPortWarpMapEntry* PCPort_WarpFindFloor(int floorId) {
    int i;
    for (i = 0; i < g_pcWarpMapCount; ++i) {
        if (g_pcWarpMaps[i].floorId == floorId) {
            return &g_pcWarpMaps[i];
        }
    }
    return NULL;
}

/* Resolve a bare map name to its disc fsys path. */
static void PCPort_WarpResolvePath(const char* name, char* out, size_t n) {
    if (strchr(name, '/') != NULL || strchr(name, '\\') != NULL) {
        snprintf(out, n, "%s", name);
    } else {
        snprintf(out, n, "orig/GC6E01/disc/files/%s.fsys", name);
    }
}

/* Tear down the current field map + load the target floor's map: scene
 * geometry (PCPort_EngineFieldSetup re-inits archive/camera/render state),
 * WZX collision, and the new map's exit triggers. Returns 1 on success and
 * writes the player's spawn position to outSpawn[3]; 0 on failure (caller
 * should keep the old map). */
static int PCPort_FieldWarpTo(int targetFloor, f32 outSpawn[3]) {
    const PCPortWarpMapEntry* dst = PCPort_WarpFindFloor(targetFloor);
    char path[512];
    int colTris;

    if (dst == NULL) {
        fprintf(stderr, "[warp] no map for floor id %d (no table entry)\n",
                targetFloor);
        return 0;
    }
    PCPort_WarpResolvePath(dst->fsysName, path, sizeof(path));
    printf("[warp] -> floor %d (%s)\n", targetFloor, path);

    /* Tear down the old map's collision + exits, load the new scene. */
    PCPort_FieldColUnload();
    PCPort_FieldExitUnload();

    if (!PCPort_EngineFieldSetup(path)) {
        fprintf(stderr, "[warp] EngineFieldSetup failed for %s\n", path);
        return 0;
    }
    colTris = PCPort_FieldColLoad(path);
    if (colTris > 0) {
        f32 cmin[3], cmax[3];
        PCPort_FieldColBounds(cmin, cmax);
        printf("[warp] collision: %d tris  X[%.0f,%.0f] Y[%.0f,%.0f] Z[%.0f,%.0f]\n",
               colTris, cmin[0], cmax[0], cmin[1], cmax[1], cmin[2], cmax[2]);
    }

    /* Load the new map's exit triggers. Try the (future) real parse first; it
     * currently always reports "not found", so we fall back to the hand list. */
    {
        u8* exitRaw = NULL;
        u32 exitCount = 0;
        if (PCPort_LoadFsysExitData(path, &exitRaw, &exitCount) && exitCount > 0) {
            /* (future) translate the raw 0x2C records into PCPortFieldExit. */
            PCPort_FreeBuffer(exitRaw);
        }
        if (PCPort_FieldExitCount() == 0 && dst->exitCount > 0) {
            PCPort_FieldExitSet(dst->exits, dst->exitCount);
        }
    }

    if (g_pcStoryFieldSmoke.active) {
        g_pcStoryFieldSmoke.currentFloor = targetFloor;
        if (targetFloor == PC_FLOOR_OUTSKIRT) {
            g_pcStoryFieldSmoke.sawOutskirtLoad = 1;
            g_pcStoryFieldSmoke.targetFloor = targetFloor;
            g_pcStoryFieldSmoke.mapLoaded = 1;
            g_pcStoryFieldSmoke.colTris = colTris;
            g_pcStoryFieldSmoke.exitCount = PCPort_FieldExitCount();
            snprintf(g_pcStoryFieldSmoke.mapPath,
                     sizeof(g_pcStoryFieldSmoke.mapPath), "%s", path);
        } else if (targetFloor == PC_FLOOR_OUTSKIRT_SHOP) {
            g_pcStoryFieldSmoke.sawShopLoad = 1;
        }
    }
    g_pcCurrentFieldFloor = targetFloor;

    if (outSpawn != NULL) {
        outSpawn[0] = dst->defaultSpawn[0];
        outSpawn[1] = dst->defaultSpawn[1];
        outSpawn[2] = dst->defaultSpawn[2];
    }
    return 1;
}

static void DrawFieldAvatar(f32 px, f32 py, f32 pz, f32 yaw,
                            f32 height, f32 radius) {
    f32 rt[3], fw[3];        /* local right / forward unit vectors (XZ) */
    f32 c[8][3];             /* 8 box corners: index = uy*4 + fz*2 + rx bits */
    int rxB, fzB, uyB;

    rt[0] = cosf(yaw);  rt[1] = 0.0f;  rt[2] = sinf(yaw);
    fw[0] = sinf(yaw);  fw[1] = 0.0f;  fw[2] = -cosf(yaw);

    for (uyB = 0; uyB < 2; ++uyB) {
        for (fzB = 0; fzB < 2; ++fzB) {
            for (rxB = 0; rxB < 2; ++rxB) {
                int idx = uyB * 4 + fzB * 2 + rxB;
                f32 sr = rxB ? radius : -radius;
                f32 sf = fzB ? radius : -radius;
                f32 uy = uyB ? height : 0.0f;
                c[idx][0] = px + rt[0] * sr + fw[0] * sf;
                c[idx][1] = py + uy;
                c[idx][2] = pz + rt[2] * sr + fw[2] * sf;
            }
        }
    }

    GXSetProjection(g_engTitleCamera.projectionMatrix, GX_PERSPECTIVE);
    GXLoadPosMtxImm(g_engTitleCamera.viewMatrix, 0);
    GXSetCullMode(GX_CULL_NONE);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_COPY);
    GXSetNumTexGens(1);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXHostSetLightingEnabled(GX_FALSE);
    GXHostSetVertexAlphaScale(1.0f);

    /* 6 quads (faces); winding doesn't matter (cull none). Each face by 4 corner
     * indices + an RGB tint. Front (fz=1) bright cyan; others steel blue. */
    {
        static const int faces[6][4] = {
            { 2, 3, 7, 6 },   /* front  (fz=1) */
            { 0, 1, 5, 4 },   /* back   (fz=0) */
            { 1, 3, 7, 5 },   /* right  (rx=1) */
            { 0, 2, 6, 4 },   /* left   (rx=0) */
            { 4, 5, 7, 6 },   /* top    (uy=1) */
            { 0, 1, 3, 2 }    /* bottom (uy=0) */
        };
        int f;
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6 * 6);
        for (f = 0; f < 6; ++f) {
            u8 r, g, b;
            int tri, vi;
            static const int order[6] = { 0, 1, 2, 0, 2, 3 };
            if (f == 0)      { r = 90;  g = 230; b = 255; }   /* front */
            else if (f == 4) { r = 120; g = 170; b = 230; }   /* top   */
            else             { r = 60;  g = 110; b = 190; }   /* sides */
            for (tri = 0; tri < 6; ++tri) {
                vi = faces[f][order[tri]];
                GXColor4u8(r, g, b, 255);
                GXPosition3f32(c[vi][0], c[vi][1], c[vi][2]);
                GXTexCoord2f32(0.0f, 0.0f);
            }
        }
        GXEnd();
    }

    GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
}

/* Load the skinned player character (ken_b1 from field_common.fsys by default)
 * into its own archive once, deriving its root joint the same way as a field
 * scene member (scene_data -> branch+0 -> jointList -> rootJoint). Returns 1 on
 * success (and on a no-op repeat call). PCPORT_WES_ARCHIVE / PCPORT_WES_MEMBER
 * override the source. */
static int PCPort_LoadFieldCharacter(void) {
    const char* fsysPath = getenv("PCPORT_WES_ARCHIVE");
    const char* member   = getenv("PCPORT_WES_MEMBER");
    u8* data = NULL;
    u32 size = 0;
    const u8* sceneData;
    u32 sceneOff = 0, branchOff, jlOff;
    if (g_engCharLoaded) return 1;
    if (fsysPath == NULL || fsysPath[0] == '\0')
        fsysPath = "orig/GC6E01/disc/files/field_common.fsys";
    if (member == NULL || member[0] == '\0')
        member = "ken_b1";
    if (!PCPort_LoadFsysMember(fsysPath, member, &data, &size) || data == NULL) {
        fprintf(stderr, "[field/wes] cannot load %s :: %s\n", fsysPath, member);
        return 0;
    }
    memset(&g_engCharArchive, 0, sizeof(g_engCharArchive));
    if (!PCPort_HSDArchiveParseBE(&g_engCharArchive, data, size)) {
        fprintf(stderr, "[field/wes] archive parse failed\n");
        return 0;
    }
    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&g_engCharArchive,
                                                             "scene_data", &sceneOff);
    if (sceneData == NULL) { fprintf(stderr, "[field/wes] scene_data unresolved\n"); return 0; }
    branchOff = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&g_engCharArchive, branchOff, 0x10u)) return 0;
    jlOff = PCPort_ReadBigEndianU32(g_engCharArchive.storage + branchOff + 0x00);
    g_engCharRoot = PCPort_ReadBigEndianU32(g_engCharArchive.storage + jlOff + 0x00);
    if (!ArchiveRangeValid(&g_engCharArchive, g_engCharRoot, PCPORT_SERIALIZED_JOINT_SIZE)) {
        fprintf(stderr, "[field/wes] root joint invalid (0x%X)\n", g_engCharRoot);
        return 0;
    }
    snprintf(g_engCharFsysPath, sizeof(g_engCharFsysPath), "%s", fsysPath);
    snprintf(g_engCharMember, sizeof(g_engCharMember), "%s", member);
    g_engCharLoaded = 1;
    printf("[field/wes] loaded %s :: %s (rootJoint=0x%X)\n", fsysPath, member, g_engCharRoot);

    /* Build the live animated HSD tree (a second, swizzled copy of the same
     * archive) so the character's real motion bank drives the BE skinning. The
     * motion data was reverse-engineered (Resource+0x4 = array of HSD_AnimJoint
     * motions); the walk loop selects idle/walk via PCPort_CharAnimSetMotion.
     * On by default; PCPORT_NO_CHAR_ANIM disables (falls back to static pose). */
    if (getenv("PCPORT_NO_CHAR_ANIM") == NULL && PCPort_CharAnimSetup(fsysPath, member)) {
        printf("[field/wes] motion bank armed\n");
    }
    return 1;
}

/* Render the loaded player character at world (px,py,pz) facing yaw, uniformly
 * scaled. Composes the orbit camera's view with a placement matrix
 * (translate * rotateY(yaw) * scale) and walks the character's skinned joint
 * tree. Needs PCPORT_SKIN for the envelope skin path.
 * frameStep: real-time elapsed seconds * 60.0f (pass 1.0f if not available). */
static void RenderFieldCharacter(f32 px, f32 py, f32 pz, f32 yaw, f32 scale,
                                 f32 frameStep) {
    PCPortTranslatedCamera tcam;
    MenuTreeStats stats;
    f32 P[3][4];
    f32 cy = cosf(yaw), sy = sinf(yaw);
    if (!g_engCharLoaded) return;
    /* Advance the active motion and write the animated joint SRT into the BE
     * archive the skinning reads (on by default; PCPORT_NO_CHAR_ANIM disables). */
    if (getenv("PCPORT_NO_CHAR_ANIM") == NULL) {
        PCPort_CharAnimStepAndApply(&g_engCharArchive, g_engCharRoot, frameStep);
    }
    /* placement = translate(p) * rotateY(yaw) * uniformScale(scale) */
    P[0][0] =  scale * cy; P[0][1] = 0.0f;  P[0][2] =  scale * sy; P[0][3] = px;
    P[1][0] =  0.0f;       P[1][1] = scale; P[1][2] =  0.0f;       P[1][3] = py;
    P[2][0] = -scale * sy; P[2][1] = 0.0f;  P[2][2] =  scale * cy; P[2][3] = pz;
    memset(&tcam, 0, sizeof(tcam));
    PCPortMulAffine3x4(g_engTitleCamera.viewMatrix, P, tcam.viewMatrix);
    memcpy(tcam.projectionMatrix, g_engTitleCamera.projectionMatrix,
           sizeof(tcam.projectionMatrix));
    memset(&stats, 0, sizeof(stats));
    GXSetProjection(g_engTitleCamera.projectionMatrix, GX_PERSPECTIVE);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    RenderJointTree(&g_engCharArchive, g_engCharRoot, g_engCharRoot, &tcam,
                    (int)PCPORT_REAL_MATERIAL_PIPELINE, &stats);
}

static void PCPort_FieldNpcModelRelease(void) {
    if (g_engNpcArchive.storage != NULL) {
        PCPort_HSDArchiveDestroy(&g_engNpcArchive);
    }
    memset(&g_engNpcArchive, 0, sizeof(g_engNpcArchive));
    g_engNpcRoot = 0;
    g_engNpcLoaded = 0;
    g_engNpcFsysPath[0] = '\0';
    g_engNpcMember[0] = '\0';
}

static int PCPort_LoadFieldNpcModel(const char* fsysPath,
                                    const char* member) {
    u8* data = NULL;
    u32 size = 0;
    const u8* sceneData;
    u32 sceneOff = 0;
    u32 branchOff;
    u32 jointListOff;

    if (fsysPath == NULL || member == NULL ||
        fsysPath[0] == '\0' || member[0] == '\0') {
        return 0;
    }

    PCPort_FieldNpcModelRelease();
    if (!PCPort_LoadFsysMember(fsysPath, member, &data, &size) ||
        data == NULL) {
        fprintf(stderr, "[field/npc-model] cannot load %s :: %s\n",
                fsysPath, member);
        return 0;
    }
    if (!PCPort_HSDArchiveParseBE(&g_engNpcArchive, data, size)) {
        fprintf(stderr, "[field/npc-model] archive parse failed for %s\n",
                member);
        PCPort_FreeBuffer(data);
        PCPort_FieldNpcModelRelease();
        return 0;
    }
    PCPort_FreeBuffer(data);

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(
        &g_engNpcArchive, "scene_data", &sceneOff);
    if (sceneData == NULL) {
        fprintf(stderr, "[field/npc-model] scene_data unresolved for %s\n",
                member);
        PCPort_FieldNpcModelRelease();
        return 0;
    }
    branchOff = PCPort_ReadBigEndianU32(sceneData + 0x00);
    if (!ArchiveRangeValid(&g_engNpcArchive, branchOff, 0x10u)) {
        fprintf(stderr, "[field/npc-model] scene branch invalid for %s (0x%X)\n",
                member, branchOff);
        PCPort_FieldNpcModelRelease();
        return 0;
    }
    jointListOff = PCPort_ReadBigEndianU32(g_engNpcArchive.storage + branchOff);
    if (!ArchiveRangeValid(&g_engNpcArchive, jointListOff, 0x4u)) {
        fprintf(stderr, "[field/npc-model] joint list invalid for %s (0x%X)\n",
                member, jointListOff);
        PCPort_FieldNpcModelRelease();
        return 0;
    }
    g_engNpcRoot = PCPort_ReadBigEndianU32(g_engNpcArchive.storage + jointListOff);
    if (!ArchiveRangeValid(&g_engNpcArchive, g_engNpcRoot,
                           PCPORT_SERIALIZED_JOINT_SIZE)) {
        fprintf(stderr, "[field/npc-model] root joint invalid for %s (0x%X)\n",
                member, g_engNpcRoot);
        PCPort_FieldNpcModelRelease();
        return 0;
    }

    snprintf(g_engNpcFsysPath, sizeof(g_engNpcFsysPath), "%s", fsysPath);
    snprintf(g_engNpcMember, sizeof(g_engNpcMember), "%s", member);
    g_engNpcLoaded = 1;
    printf("[field/npc-model] loaded %s :: %s (rootJoint=0x%X)\n",
           fsysPath, member, g_engNpcRoot);
    return 1;
}

static unsigned int RenderFieldStaticArchive(const PCPortHSDArchive* archive,
                                             u32 rootJoint,
                                             f32 px, f32 py, f32 pz,
                                             f32 yaw, f32 scale) {
    PCPortTranslatedCamera tcam;
    MenuTreeStats stats;
    f32 P[3][4];
    f32 cy = cosf(yaw);
    f32 sy = sinf(yaw);

    if (archive == NULL || archive->storage == NULL ||
        rootJoint == 0u ||
        !ArchiveRangeValid(archive, rootJoint, PCPORT_SERIALIZED_JOINT_SIZE)) {
        return 0;
    }

    P[0][0] =  scale * cy; P[0][1] = 0.0f;  P[0][2] =  scale * sy; P[0][3] = px;
    P[1][0] =  0.0f;       P[1][1] = scale; P[1][2] =  0.0f;       P[1][3] = py;
    P[2][0] = -scale * sy; P[2][1] = 0.0f;  P[2][2] =  scale * cy; P[2][3] = pz;
    memset(&tcam, 0, sizeof(tcam));
    PCPortMulAffine3x4(g_engTitleCamera.viewMatrix, P, tcam.viewMatrix);
    memcpy(tcam.projectionMatrix, g_engTitleCamera.projectionMatrix,
           sizeof(tcam.projectionMatrix));
    memset(&stats, 0, sizeof(stats));
    GXSetProjection(g_engTitleCamera.projectionMatrix, GX_PERSPECTIVE);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    RenderJointTree(archive, rootJoint, rootJoint, &tcam,
                    (int)PCPORT_REAL_MATERIAL_PIPELINE, &stats);
    return stats.drawn;
}

static int PCPort_StringEndsWith(const char* text, const char* suffix) {
    size_t textLen;
    size_t suffixLen;
    if (text == NULL || suffix == NULL) {
        return 0;
    }
    textLen = strlen(text);
    suffixLen = strlen(suffix);
    if (suffixLen > textLen) {
        return 0;
    }
    return strcmp(text + textLen - suffixLen, suffix) == 0;
}

static int PCPort_FieldNpcResolveDependencyName(const char* token,
                                                char* outName,
                                                size_t outNameSize) {
    static const char* kS1OutMembers[] = {
        "niku_m_b3",
        "hunter_m_b2",
        "hunter_m_b3",
        "caster_a_b1",
        "agent_m_b1",
        "truck_b1",
        "bike_pokemon",
        "rider_m_b1"
    };
    int i;

    if (token == NULL || outName == NULL || outNameSize == 0u) {
        return 0;
    }

    for (i = 0; i < (int)(sizeof(kS1OutMembers) / sizeof(kS1OutMembers[0])); ++i) {
        if (strcmp(kS1OutMembers[i], token) == 0) {
            snprintf(outName, outNameSize, "%s", kS1OutMembers[i]);
            return 1;
        }
    }
    for (i = 0; i < (int)(sizeof(kS1OutMembers) / sizeof(kS1OutMembers[0])); ++i) {
        if (PCPort_StringEndsWith(kS1OutMembers[i], token)) {
            snprintf(outName, outNameSize, "%s", kS1OutMembers[i]);
            return 1;
        }
    }
    return 0;
}

static int PCPort_FieldNpcReadDependencyList(const char* fsysPath,
                                             char resolved[][64],
                                             int maxResolved,
                                             int* outRawCount,
                                             char* outSummary,
                                             size_t summarySize) {
    u8* data = NULL;
    u32 size = 0;
    u32 pos = 0;
    int rawCount = 0;
    int resolvedCount = 0;

    if (outRawCount != NULL) {
        *outRawCount = 0;
    }
    if (outSummary != NULL && summarySize > 0u) {
        outSummary[0] = '\0';
    }
    if (!PCPort_LoadFsysMember(fsysPath, "FSYS", &data, &size) ||
        data == NULL || size == 0u) {
        return 0;
    }

    while (pos < size) {
        char token[64];
        char member[64];
        u32 len = 0;
        while (pos + len < size && data[pos + len] != '\0') {
            ++len;
        }
        if (len > 0u) {
            size_t copyLen = (len < sizeof(token) - 1u)
                ? (size_t)len
                : sizeof(token) - 1u;
            memcpy(token, data + pos, copyLen);
            token[copyLen] = '\0';
            ++rawCount;
            if (PCPort_FieldNpcResolveDependencyName(token, member,
                                                     sizeof(member))) {
                if (resolvedCount < maxResolved) {
                    snprintf(resolved[resolvedCount],
                             sizeof(resolved[resolvedCount]), "%s", member);
                }
                ++resolvedCount;
                if (outSummary != NULL && summarySize > 0u) {
                    size_t used = strlen(outSummary);
                    if (used < summarySize - 1u) {
                        snprintf(outSummary + used, summarySize - used,
                                 "%s%s->%s",
                                 used != 0u ? "," : "",
                                 token, member);
                    }
                }
            } else if (outSummary != NULL && summarySize > 0u) {
                size_t used = strlen(outSummary);
                if (used < summarySize - 1u) {
                    snprintf(outSummary + used, summarySize - used,
                             "%s%s->?",
                             used != 0u ? "," : "",
                             token);
                }
            }
        }
        pos += len + 1u;
    }

    PCPort_FreeBuffer(data);
    if (outRawCount != NULL) {
        *outRawCount = rawCount;
    }
    return resolvedCount;
}

typedef struct PCPortFieldNpcScriptPlacement {
    u32 tokenOffset;
    u32 actorToken;
    u32 actionWord;
    f32 pos[3];
    f32 yaw;
} PCPortFieldNpcScriptPlacement;

#define PCPORT_S1_OUT_NPC_ACTOR_TOKEN 0x025D0000u

static f32 PCPort_ReadBigEndianF32Local(const u8* data) {
    union {
        u32 u;
        f32 f;
    } v;
    v.u = PCPort_ReadBigEndianU32(data);
    return v.f;
}

static int PCPort_FieldNpcPlacementPlausible(const f32 pos[3]) {
    if (pos == NULL) {
        return 0;
    }
    if (!isfinite(pos[0]) || !isfinite(pos[1]) || !isfinite(pos[2])) {
        return 0;
    }
    if (pos[0] == 0.0f && pos[1] == 0.0f && pos[2] == 0.0f) {
        return 0;
    }
    return pos[0] >= -200.0f && pos[0] <= 300.0f &&
           pos[1] >= -20.0f && pos[1] <= 80.0f &&
           pos[2] >= -160.0f && pos[2] <= 160.0f;
}

static int PCPort_FieldNpcReadScriptPlacement(
    const char* fsysPath,
    PCPortFieldNpcScriptPlacement* outPlacement) {
    u8* data = NULL;
    u32 size = 0;
    u32 off;

    if (outPlacement != NULL) {
        memset(outPlacement, 0, sizeof(*outPlacement));
    }
    if (!PCPort_LoadFsysMember(fsysPath, "S1_out", &data, &size) ||
        data == NULL || size < 0x20u) {
        return 0;
    }

    for (off = 0; off + 0x1Cu <= size; off += 4u) {
        f32 pos[3];
        u32 token = PCPort_ReadBigEndianU32(data + off);
        if (token != PCPORT_S1_OUT_NPC_ACTOR_TOKEN) {
            continue;
        }

        pos[0] = PCPort_ReadBigEndianF32Local(data + off + 0x10u);
        pos[1] = PCPort_ReadBigEndianF32Local(data + off + 0x14u);
        pos[2] = PCPort_ReadBigEndianF32Local(data + off + 0x18u);
        if (!PCPort_FieldNpcPlacementPlausible(pos)) {
            continue;
        }

        if (outPlacement != NULL) {
            outPlacement->tokenOffset = off;
            outPlacement->actorToken = token;
            outPlacement->actionWord =
                PCPort_ReadBigEndianU32(data + off + 0x0Cu);
            outPlacement->pos[0] = pos[0];
            outPlacement->pos[1] = pos[1];
            outPlacement->pos[2] = pos[2];
            outPlacement->yaw = 0.0f;
        }
        PCPort_FreeBuffer(data);
        return 1;
    }

    PCPort_FreeBuffer(data);
    return 0;
}

#define PCPORT_HOST_PEOPLE_MAX 8
#define PCPORT_PEOPLE_SNAPSHOT_MAX_RECORDS 0x40u
#define PCPORT_PEOPLE_SNAPSHOT_COPY_SIZE 0xBCu
#define PCPORT_PEOPLE_RECORD_VISIBLE_OFF 0x00u
#define PCPORT_PEOPLE_RECORD_FLAGS_OFF 0x04u
#define PCPORT_PEOPLE_RECORD_GROUP_OFF 0x08u
#define PCPORT_PEOPLE_RECORD_INDEX_OFF 0x0Cu
#define PCPORT_PEOPLE_RECORD_SCRIPT_REF_OFF 0x10u
#define PCPORT_PEOPLE_RECORD_TALK_RANGE_OFF 0x1Cu
#define PCPORT_PEOPLE_RECORD_STATE_OFF 0x34u
#define PCPORT_PEOPLE_RECORD_PREV_STATE_OFF 0x37u
#define PCPORT_PEOPLE_RECORD_MOVE_SPEED_OFF 0x38u
#define PCPORT_PEOPLE_RECORD_COLLISION_POS_OFF 0x54u
#define PCPORT_PEOPLE_RECORD_MOTION_INDEX_OFF 0x70u
#define PCPORT_PEOPLE_RECORD_MOVE_TYPE_OFF 0x76u
#define PCPORT_PEOPLE_RECORD_TRANSFORM_OFF 0x7Cu
#define PCPORT_PEOPLE_RECORD_TARGET_POS_OFF 0x88u
#define PCPORT_PEOPLE_RECORD_MODEL_POS_OFF 0xBCu
#define PCPORT_PEOPLE_RECORD_MODEL_ROT_OFF 0xC8u
#define PCPORT_PEOPLE_RECORD_MODEL_SCALE_OFF 0xD4u
#define PCPORT_PEOPLE_RECORD_MODEL_AUX_OFF 0xE0u

static PeopleEntry g_pcHostPeople[PCPORT_HOST_PEOPLE_MAX];
static s32 g_pcHostPeopleCount;

typedef struct PCPortPeopleOpenPlacement {
    int valid;
    u32 groupId;
    u32 index;
    u32 actionWord;
    u32 actorToken;
    u32 tokenOffset;
    f32 pos[3];
    f32 yaw;
} PCPortPeopleOpenPlacement;

static PCPortPeopleOpenPlacement g_pcPeopleOpenPlacement;
static u32 g_pcPeopleOpenSetupCount;

static void PCPort_PeopleHostClear(void) {
    memset(g_pcHostPeople, 0, sizeof(g_pcHostPeople));
    g_pcHostPeopleCount = 0;
    memset(&g_pcPeopleOpenPlacement, 0, sizeof(g_pcPeopleOpenPlacement));
    g_pcPeopleOpenSetupCount = 0u;
}

static void PCPort_WriteHostU32(u8* data, u32 value) {
    memcpy(data, &value, sizeof(value));
}

static u32 PCPort_ReadHostU32(const u8* data) {
    u32 value;
    memcpy(&value, data, sizeof(value));
    return value;
}

static void PCPort_WriteHostF32(u8* data, f32 value) {
    memcpy(data, &value, sizeof(value));
}

static f32 PCPort_ReadHostF32(const u8* data) {
    f32 value;
    memcpy(&value, data, sizeof(value));
    return value;
}

static void PCPort_WriteHostVec3(u8* data, const f32 pos[3]) {
    PCPort_WriteHostF32(data + 0x0u, pos[0]);
    PCPort_WriteHostF32(data + 0x4u, pos[1]);
    PCPort_WriteHostF32(data + 0x8u, pos[2]);
}

static int PCPort_F32Near(f32 a, f32 b) {
    return fabsf(a - b) < 0.001f;
}

static int PCPort_Vec3Near(const f32 a[3], const f32 b[3]) {
    return PCPort_F32Near(a[0], b[0]) &&
           PCPort_F32Near(a[1], b[1]) &&
           PCPort_F32Near(a[2], b[2]);
}

static PeopleEntry* PCPort_PeopleHostAllocRaw(void) {
    PeopleEntry* entry;
    if (g_pcHostPeopleCount >= PCPORT_HOST_PEOPLE_MAX) {
        return NULL;
    }
    entry = &g_pcHostPeople[g_pcHostPeopleCount++];
    memset(entry, 0, sizeof(*entry));
    entry->active = 1;
    entry->selfPtr = entry;
    entry->shadowId = -1;
    entry->moveSpeed = 1.0f;
    return entry;
}

static void PCPort_PeopleHostSetPosition(PeopleEntry* entry,
                                         const f32 pos[3]) {
    if (entry == NULL || pos == NULL) {
        return;
    }
    memcpy(entry->collisionData, pos, sizeof(f32) * 3u);
    memcpy(entry->transform, pos, sizeof(f32) * 3u);
    entry->targetX = pos[0];
    entry->targetY = pos[1];
    entry->targetZ = pos[2];
}

static void PCPort_PeopleHostGetPosition(const PeopleEntry* entry,
                                         f32 outPos[3]) {
    if (outPos == NULL) {
        return;
    }
    if (entry == NULL) {
        outPos[0] = 0.0f;
        outPos[1] = 0.0f;
        outPos[2] = 0.0f;
        return;
    }
    outPos[0] = entry->targetX;
    outPos[1] = entry->targetY;
    outPos[2] = entry->targetZ;
}

static PeopleEntry* PCPort_PeopleHostAdd(u32 groupId, u32 index, u8 state) {
    PeopleEntry* entry = PCPort_PeopleHostAllocRaw();
    if (entry == NULL) {
        return NULL;
    }
    entry->visible = 1;
    entry->flags = PEOPLE_FLAG_ACTIVE | PEOPLE_FLAG_TALKABLE;
    entry->groupId = groupId;
    entry->index = index;
    entry->state = state;
    entry->prevState = state;
    entry->subState = 0;
    entry->animBlendFactor = 0.0f;
    return entry;
}

static PeopleEntry* PCPort_PeopleHostEntryAt(s32 slot) {
    if (slot < 0 || slot >= g_pcHostPeopleCount) {
        return NULL;
    }
    return &g_pcHostPeople[slot];
}

static PeopleEntry* PCPort_PeopleHostResolveSelf(void* selfPtr) {
    s32 i;
    if (selfPtr == NULL) {
        return NULL;
    }
    for (i = 0; i < g_pcHostPeopleCount; ++i) {
        PeopleEntry* entry = PCPort_PeopleHostEntryAt(i);
        if (entry != NULL && entry->active != 0 && entry->selfPtr == selfPtr) {
            return entry;
        }
    }
    return NULL;
}

static PeopleEntry* PCPort_PeopleHostFindByGroupIndex(u32 groupId, u32 index) {
    s32 i;
    void* selfPtr = NULL;

    for (i = 0; i < g_pcHostPeopleCount; ++i) {
        PeopleEntry* entry = PCPort_PeopleHostEntryAt(i);
        if (entry != NULL && entry->active != 0 &&
            entry->groupId == groupId && entry->index == index) {
            selfPtr = entry->selfPtr;
            break;
        }
    }

    if (selfPtr == NULL) {
        for (i = 0; i < g_pcHostPeopleCount; ++i) {
            PeopleEntry* entry = PCPort_PeopleHostEntryAt(i);
            if (entry != NULL && entry->active != 0 && entry->index == index) {
                printf("[people] Warining: people[%u,%u] group is different!!\n",
                       groupId, index);
                selfPtr = entry->selfPtr;
                break;
            }
        }
    }

    return PCPort_PeopleHostResolveSelf(selfPtr);
}

static PeopleEntry* PCPort_PeopleHostFindExact(u32 groupId, u32 index) {
    s32 i;
    for (i = 0; i < g_pcHostPeopleCount; ++i) {
        PeopleEntry* entry = PCPort_PeopleHostEntryAt(i);
        if (entry != NULL && entry->active != 0 &&
            entry->groupId == groupId && entry->index == index) {
            return entry;
        }
    }
    return NULL;
}

static void PCPort_PeopleOpenClearPlacement(void) {
    memset(&g_pcPeopleOpenPlacement, 0, sizeof(g_pcPeopleOpenPlacement));
}

static void PCPort_PeopleOpenSetPlacement(
    u32 groupId,
    u32 index,
    const PCPortFieldNpcScriptPlacement* placement) {
    PCPort_PeopleOpenClearPlacement();
    if (placement == NULL) {
        return;
    }
    g_pcPeopleOpenPlacement.valid = 1;
    g_pcPeopleOpenPlacement.groupId = groupId;
    g_pcPeopleOpenPlacement.index = index;
    g_pcPeopleOpenPlacement.actionWord = placement->actionWord;
    g_pcPeopleOpenPlacement.actorToken = placement->actorToken;
    g_pcPeopleOpenPlacement.tokenOffset = placement->tokenOffset;
    g_pcPeopleOpenPlacement.pos[0] = placement->pos[0];
    g_pcPeopleOpenPlacement.pos[1] = placement->pos[1];
    g_pcPeopleOpenPlacement.pos[2] = placement->pos[2];
    g_pcPeopleOpenPlacement.yaw = placement->yaw;
}

void fn_8018E1C4(PeopleEntry* entry, u32 groupId, u32 index, u32 actionWord) {
    f32 zero[3] = { 0.0f, 0.0f, 0.0f };

    if (entry == NULL) {
        return;
    }

    entry->active = 1;
    entry->selfPtr = entry;
    entry->visible = 1;
    entry->animId = 1;
    entry->shadowAnimId = 0;
    entry->flags = PEOPLE_FLAG_ACTIVE | PEOPLE_FLAG_TALKABLE;
    entry->groupId = groupId;
    entry->index = index;
    entry->scriptRef = (void*)(unsigned long)actionWord;
    entry->field_34 = 0u;
    entry->field_38 = actionWord;
    entry->talkRange = 5.0f;
    entry->walkTargetNode = -1;
    entry->shadowId = -1;
    entry->state = PEOPLE_STATE_IDLE;
    entry->prevState = PEOPLE_STATE_IDLE;
    entry->subState = 0;
    entry->talkLock = 0;
    entry->moveSpeed = 1.0f;
    entry->animBlendFactor = 0.0f;
    entry->motionIndex = actionWord;
    entry->isTalkable = 1;
    entry->moveType = PEOPLE_MOVE_NONE;
    entry->targetX = 0.0f;
    entry->targetY = 0.0f;
    entry->targetZ = 0.0f;
    PCPort_PeopleHostSetPosition(entry, zero);

    if (g_pcPeopleOpenPlacement.valid &&
        g_pcPeopleOpenPlacement.groupId == groupId &&
        g_pcPeopleOpenPlacement.index == index &&
        g_pcPeopleOpenPlacement.actionWord == actionWord) {
        entry->field_34 = g_pcPeopleOpenPlacement.actorToken;
        entry->scriptRef =
            (void*)(unsigned long)g_pcPeopleOpenPlacement.tokenOffset;
        entry->state = PEOPLE_STATE_INTERACTING;
        entry->prevState = PEOPLE_STATE_INTERACTING;
        entry->flags |= PEOPLE_FLAG_HAS_MODEL;
        PCPort_PeopleHostSetPosition(entry, g_pcPeopleOpenPlacement.pos);
    }

    ++g_pcPeopleOpenSetupCount;
}

void fn_8018E050(u32 groupId, u32 index, u32 actionWord) {
    PeopleEntry* entry = PCPort_PeopleHostFindExact(groupId, index);
    if (entry == NULL) {
        entry = PCPort_PeopleHostAllocRaw();
    }
    fn_8018E1C4(entry, groupId, index, actionWord);
}

u32 fn_8018F730(void) {
    u32 total = 0u;
    s32 i;
    for (i = 0; i < g_pcHostPeopleCount; ++i) {
        PeopleEntry* entry = PCPort_PeopleHostEntryAt(i);
        if (entry != NULL && entry->active != 0) {
            total += PEOPLE_SPAWN_DATA_SIZE;
        }
    }
    return total;
}

void fn_8018F788(void* outBuffer, u32 byteSize) {
    u8* out = (u8*)outBuffer;
    u32 written = 0u;
    s32 i;

    if (out == NULL || byteSize < PEOPLE_SPAWN_DATA_SIZE) {
        return;
    }

    for (i = 0; i < g_pcHostPeopleCount; ++i) {
        PeopleEntry* entry = PCPort_PeopleHostEntryAt(i);
        f32 pos[3];
        f32 zero[3] = { 0.0f, 0.0f, 0.0f };
        f32 one[3] = { 1.0f, 1.0f, 1.0f };
        u8* record;

        if (entry == NULL || entry->active == 0) {
            continue;
        }
        if (written + PEOPLE_SPAWN_DATA_SIZE > byteSize) {
            return;
        }

        record = out + written;
        memset(record, 0, PEOPLE_SPAWN_DATA_SIZE);
        memcpy(record, ((const u8*)entry) + 0x20u,
               PCPORT_PEOPLE_SNAPSHOT_COPY_SIZE);
        PCPort_PeopleHostGetPosition(entry, pos);
        PCPort_WriteHostVec3(record + PCPORT_PEOPLE_RECORD_MODEL_POS_OFF, pos);
        PCPort_WriteHostVec3(record + PCPORT_PEOPLE_RECORD_MODEL_ROT_OFF, zero);
        PCPort_WriteHostVec3(record + PCPORT_PEOPLE_RECORD_MODEL_SCALE_OFF, one);
        PCPort_WriteHostVec3(record + PCPORT_PEOPLE_RECORD_MODEL_AUX_OFF, zero);
        written += PEOPLE_SPAWN_DATA_SIZE;
    }
}

void fn_8018F87C(void* snapshot, u32 byteSize) {
    const u8* record = (const u8*)snapshot;
    u32 count;
    u32 i;

    PCPort_PeopleHostClear();
    if (record == NULL ||
        byteSize == 0u ||
        (byteSize % PEOPLE_SPAWN_DATA_SIZE) != 0u) {
        return;
    }

    count = byteSize / PEOPLE_SPAWN_DATA_SIZE;
    if (count > PCPORT_PEOPLE_SNAPSHOT_MAX_RECORDS ||
        count > PCPORT_HOST_PEOPLE_MAX) {
        return;
    }

    for (i = 0u; i < count; ++i) {
        PeopleEntry* entry = PCPort_PeopleHostAllocRaw();
        f32 modelPos[3];
        if (entry == NULL) {
            break;
        }

        memcpy(((u8*)entry) + 0x20u, record,
               PCPORT_PEOPLE_SNAPSHOT_COPY_SIZE);
        modelPos[0] =
            PCPort_ReadHostF32(record + PCPORT_PEOPLE_RECORD_MODEL_POS_OFF);
        modelPos[1] =
            PCPort_ReadHostF32(record + PCPORT_PEOPLE_RECORD_MODEL_POS_OFF + 0x4u);
        modelPos[2] =
            PCPort_ReadHostF32(record + PCPORT_PEOPLE_RECORD_MODEL_POS_OFF + 0x8u);
        if (PCPort_FieldNpcPlacementPlausible(modelPos)) {
            PCPort_PeopleHostSetPosition(entry, modelPos);
        }
        record += PEOPLE_SPAWN_DATA_SIZE;
    }
}

/* PC-port mirror of retail fn_801812E8 (0x190 bytes). The full people.c TU is
 * not host-linkable yet, but gs_field_world already calls this symbol from the
 * recovered field interaction path. Keep this body faithful to the retail state
 * transitions so the link no longer satisfies the call with an auto-stub. */
u32 fn_801812E8(u32 groupId, u32 index, u32 doInteract) {
    PeopleEntry* entry = PCPort_PeopleHostFindByGroupIndex(groupId, index);
    u8 state;
    u8 prev;

    if (entry == NULL) {
        return 0u;
    }

    state = entry->state;
    if (state == PEOPLE_STATE_IDLE) {
        return 1u;
    }

    if ((doInteract & 0xFFu) != 0u) {
        entry->prevState = state;
        if (state >= PEOPLE_STATE_INTERACTING &&
            state < PEOPLE_STATE_INACTIVE) {
            entry->state = PEOPLE_STATE_IDLE;
        }
    } else {
        prev = entry->prevState;
        if (prev >= PEOPLE_STATE_INTERACTING &&
            prev < PEOPLE_STATE_INACTIVE) {
            entry->state = prev;
        }
    }

    entry->subState = 0;
    entry->animBlendFactor = 0.0f;
    return 1u;
}

typedef enum PCPortFieldMotionRole {
    PCPORT_FIELD_MOTION_IDLE = 0,
    PCPORT_FIELD_MOTION_WALK = 1,
    PCPORT_FIELD_MOTION_RUN  = 2
} PCPortFieldMotionRole;

typedef struct PCPortFieldMotionMap {
    int idle;
    int walk;
    int run;
    int idleLoop;
    int walkLoop;
    int runLoop;
    int actionMotion[9];
    int actionLoop[9];
    int actionValid[9];
    f32 turnStepPos;
    f32 turnStepNeg;
    int fromRecord;
    int fromHeuristic;
    int fromEnv;
    u32 key;
} PCPortFieldMotionMap;

typedef struct PCPortFieldMotionChoice {
    int fromSlot;
    int toSlot;
    f32 blend;
    const char* zoneName;
} PCPortFieldMotionChoice;

#define PCPORT_FIELD_MOTION_RECORD_STRIDE 0x2Cu
#define PCPORT_FIELD_MOTION_RECORD_KEY_OFF 0x0Cu
#define PCPORT_FIELD_MOTION_TURN_POS_OFF 0x1Cu
#define PCPORT_FIELD_MOTION_TURN_NEG_OFF 0x20u
#define PCPORT_FIELD_MOTION_DEFAULT_KEY 0x00F30400u
#define PCPORT_FIELD_MOTION_PARTNER_KEY 0x00F70400u
#define PCPORT_FIELD_TURN_CLAMP_POS 2.0f
#define PCPORT_FIELD_TURN_HARD_NEG (-0.4000000059604645f)
#define PCPORT_FIELD_TURN_NEAR_POS 0.4000000059604645f
#define PCPORT_FIELD_TURN_ONE 1.0f
#define PCPORT_FIELD_TURN_STRAIGHT_SCALE 2.5f
#define PCPORT_FIELD_TURN_DEG_TO_RAD 0.017453292519943295f
#define PCPORT_FIELD_TURN_DEFAULT_STEP 1.30899694f

static const u8* g_pcFieldMotionRecordTable;
static u32       g_pcFieldMotionRecordCount;

/* Retail common_rel rows recovered from build_pc/logo_probe/common/common_rel.bin.
 * They are the raw lbl_80478E7C-style 0x2c action records used by
 * fn_8018F6F4(key) + fn_8018F4C8(record, slot, outMotion, outLoop).
 * Offsets in that dumped common_rel: 0x13FCE4 for 0x00F30400, 0x13FCB8
 * for 0x00F70400. Installed tables from a live loader still take priority. */
static const u8 kPCPortFieldMotionRetailRecords[][PCPORT_FIELD_MOTION_RECORD_STRIDE] = {
    {
        0x54, 0x01, 0x02, 0x03, 0x04, 0xFF, 0xFF, 0xFF, 0xFF, 0x06, 0x00,
        0x00, 0x00, 0xF3, 0x04, 0x00, 0x41, 0x10, 0x00, 0x00, 0x41, 0x50,
        0x00, 0x00, 0x40, 0x80, 0x00, 0x00, 0xC2, 0x96, 0x00, 0x00, 0x42,
        0x96, 0x00, 0x00, 0xC1, 0xF0, 0x00, 0x00, 0x42, 0x48, 0x00, 0x00
    },
    {
        0x34, 0x01, 0x02, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x00,
        0x00, 0x00, 0xF7, 0x04, 0x00, 0x41, 0x10, 0x00, 0x00, 0x41, 0x50,
        0x00, 0x00, 0x40, 0x80, 0x00, 0x00, 0xC2, 0x96, 0x00, 0x00, 0x42,
        0x96, 0x00, 0x00, 0xC1, 0xF0, 0x00, 0x00, 0x42, 0x48, 0x00, 0x00
    }
};

void PCPort_FieldMotionInstallRecordTable(const void* table, u32 count) {
    g_pcFieldMotionRecordTable = (const u8*)table;
    g_pcFieldMotionRecordCount = count;
}

void PCPort_FieldMotionClearRecordTable(void) {
    g_pcFieldMotionRecordTable = NULL;
    g_pcFieldMotionRecordCount = 0;
}

static int PCPort_ParseIntEnv(const char* name, int* outValue) {
    const char* e = getenv(name);
    if (e == NULL || e[0] == '\0' || outValue == NULL) {
        return 0;
    }
    *outValue = atoi(e);
    return 1;
}

static int PCPort_ParseU32Env(const char* name, u32* outValue) {
    const char* e = getenv(name);
    char* endp;
    unsigned long v;
    if (e == NULL || e[0] == '\0' || outValue == NULL) {
        return 0;
    }
    v = strtoul(e, &endp, 0);
    if (endp == e) {
        return 0;
    }
    *outValue = (u32)v;
    return 1;
}

static int PCPort_FieldMotionActionSlot(int actionStateIndex,
                                        int* outOffset,
                                        int* outLoopFlag) {
    int offset = -1;
    int loop = 0;
    switch (actionStateIndex) {
    case 1: offset = 0x1; loop = 1; break;
    case 2: offset = 0x2; loop = 1; break;
    case 3: offset = 0x3; loop = 1; break;
    case 4: offset = 0x4; loop = 0; break;
    case 5: offset = 0x1; loop = 1; break;
    case 6: offset = 0x6; loop = 0; break;
    case 7: offset = 0x7; loop = 1; break;
    case 8: offset = 0x8; loop = 0; break;
    default: return 0;
    }
    if (outOffset != NULL) *outOffset = offset;
    if (outLoopFlag != NULL) *outLoopFlag = loop;
    return 1;
}

/* Host mirror of fn_8018F4C8: record bytes are signed motion ids, and -1 means
 * "no motion". Loop flag is owned by the action slot, not the byte value. */
static int PCPort_FieldMotionRecordReadAction(const u8* record,
                                              int actionStateIndex,
                                              int* outMotion,
                                              int* outLoopFlag) {
    int offset, loop;
    int motion;
    if (record == NULL || !PCPort_FieldMotionActionSlot(actionStateIndex,
                                                       &offset, &loop)) {
        if (outMotion != NULL) *outMotion = -1;
        if (outLoopFlag != NULL) *outLoopFlag = 0;
        return 0;
    }
    motion = (int)(signed char)record[offset];
    if (outMotion != NULL) *outMotion = motion;
    if (outLoopFlag != NULL) *outLoopFlag = loop;
    return motion >= 0;
}

static const u8* PCPort_FieldMotionFindRecordInTable(const u8* p,
                                                     u32 count,
                                                     u32 key) {
    u32 i;
    if (p == NULL || count == 0u) {
        return NULL;
    }
    for (i = 0u; i < count; ++i) {
        if (PCPort_ReadBigEndianU32(p + PCPORT_FIELD_MOTION_RECORD_KEY_OFF) == key) {
            return p;
        }
        p += PCPORT_FIELD_MOTION_RECORD_STRIDE;
    }
    return NULL;
}

static const u8* PCPort_FieldMotionFindRecordByKey(u32 key) {
    const u8* record;
    record = PCPort_FieldMotionFindRecordInTable(g_pcFieldMotionRecordTable,
                                                 g_pcFieldMotionRecordCount,
                                                 key);
    if (record != NULL) {
        return record;
    }
    return PCPort_FieldMotionFindRecordInTable(
        &kPCPortFieldMotionRetailRecords[0][0],
        (u32)(sizeof(kPCPortFieldMotionRetailRecords) /
              sizeof(kPCPortFieldMotionRetailRecords[0])),
        key);
}

void fn_8018F4C8(void* record, u32 actionStateIndex, s32* outMotion,
                 u8* outLoopFlag) {
    u8 slot;
    int offset;
    int loop;

    if (record == NULL) {
        return;
    }
    slot = (u8)actionStateIndex;
    if (slot > 8u) {
        return;
    }
    if (!PCPort_FieldMotionActionSlot((int)slot, &offset, &loop)) {
        return;
    }
    *outMotion = (s32)(signed char)*((u8*)record + offset);
    *outLoopFlag = (u8)loop;
}

void* fn_8018F6F4(u32 key) {
    return (void*)PCPort_FieldMotionFindRecordByKey(key);
}

f32 fn_8018F658(void* record) {
    if (record != NULL) {
        return PCPort_ReadBigEndianF32Local((const u8*)record +
                                            PCPORT_FIELD_MOTION_TURN_NEG_OFF) *
               PCPORT_FIELD_TURN_DEG_TO_RAD;
    }
    return 0.0f;
}

f32 fn_8018F678(void* record) {
    if (record != NULL) {
        return PCPort_ReadBigEndianF32Local((const u8*)record +
                                            PCPORT_FIELD_MOTION_TURN_POS_OFF) *
               PCPORT_FIELD_TURN_DEG_TO_RAD;
    }
    return 0.0f;
}

static u32 PCPort_FieldMotionDefaultKey(void) {
    return PCPORT_FIELD_MOTION_DEFAULT_KEY;
}

static int PCPort_ParseFieldMotionRecordEnv(u8 outRecord[PCPORT_FIELD_MOTION_RECORD_STRIDE]) {
    const char* e = getenv("PCPORT_FIELD_MOTION_RECORD_BYTES");
    int values[9];
    int n = 0;
    const char* p;
    if (e == NULL || e[0] == '\0' || outRecord == NULL) {
        return 0;
    }
    memset(outRecord, 0, PCPORT_FIELD_MOTION_RECORD_STRIDE);
    p = e;
    while (*p != '\0' && n < 9) {
        char* endp;
        long v = strtol(p, &endp, 0);
        if (endp == p) {
            break;
        }
        values[n++] = (int)v;
        p = endp;
        while (*p == ',' || *p == ' ' || *p == '\t') {
            ++p;
        }
    }
    if (n != 8 && n != 9) {
        return 0;
    }
    if (n == 8) {
        int i;
        for (i = 0; i < 8; ++i) {
            outRecord[i + 1] = (u8)(signed char)values[i];
        }
    } else {
        int i;
        for (i = 0; i < 9; ++i) {
            outRecord[i] = (u8)(signed char)values[i];
        }
    }
    return 1;
}

static int PCPort_FieldMotionMapFromRecord(const u8* record,
                                           PCPortFieldMotionMap* map) {
    int idle = -1, walk = -1, run = -1;
    int idleLoop = 0, walkLoop = 0, runLoop = 0;
    int i;
    if (map == NULL || record == NULL) {
        return 0;
    }
    for (i = 1; i <= 8; ++i) {
        int motion = -1;
        int loop = 0;
        int valid = PCPort_FieldMotionRecordReadAction(record, i, &motion, &loop);
        map->actionMotion[i] = motion;
        map->actionLoop[i] = loop;
        map->actionValid[i] = valid;
    }
    PCPort_FieldMotionRecordReadAction(record, 1, &idle, &idleLoop);
    PCPort_FieldMotionRecordReadAction(record, 2, &walk, &walkLoop);
    PCPort_FieldMotionRecordReadAction(record, 3, &run, &runLoop);
    if (idle < 0 && walk < 0 && run < 0) {
        return 0;
    }
    map->idle = idle;
    map->walk = walk;
    map->run = run;
    map->idleLoop = idleLoop;
    map->walkLoop = walkLoop;
    map->runLoop = runLoop;
    map->turnStepPos =
        PCPort_ReadBigEndianF32Local(record + PCPORT_FIELD_MOTION_TURN_POS_OFF) *
        PCPORT_FIELD_TURN_DEG_TO_RAD;
    map->turnStepNeg =
        PCPort_ReadBigEndianF32Local(record + PCPORT_FIELD_MOTION_TURN_NEG_OFF) *
        PCPORT_FIELD_TURN_DEG_TO_RAD;
    if (map->turnStepPos < 0.0f) map->turnStepPos = -map->turnStepPos;
    if (map->turnStepNeg < 0.0f) map->turnStepNeg = -map->turnStepNeg;
    if (map->turnStepPos <= 0.0f) map->turnStepPos = PCPORT_FIELD_TURN_DEFAULT_STEP;
    if (map->turnStepNeg <= 0.0f) map->turnStepNeg = PCPORT_FIELD_TURN_DEFAULT_STEP;
    map->fromRecord = 1;
    return 1;
}

static void PCPort_FieldMotionRecordProbe(const u8* record, u32 key) {
    int i;
    int idle = -1, walk = -1, run = -1;
    int loop = 0;
    if (record == NULL) {
        return;
    }
    printf("[field/motion-record] key=0x%08X bytes:", key);
    for (i = 0; i <= 8; ++i) {
        printf(" %d", (int)(signed char)record[i]);
    }
    printf("\n");
    for (i = 1; i <= 8; ++i) {
        int motion = -1, loop = 0;
        int valid = PCPort_FieldMotionRecordReadAction(record, i, &motion, &loop);
        printf("[field/motion-record] action[%d] motion=%d loop=%d%s\n",
               i, motion, loop, valid ? "" : " skip");
    }
    PCPort_FieldMotionRecordReadAction(record, 1, &idle, &loop);
    PCPort_FieldMotionRecordReadAction(record, 2, &walk, &loop);
    PCPort_FieldMotionRecordReadAction(record, 3, &run, &loop);
    printf("[field/motion-record] host role projection action1/2/3 -> idle=%d walk=%d run=%d\n",
           idle, walk, run);
    fflush(stdout);
}

/* Role-to-motion indirection. If a real 0x2C per-character action record is
 * installed or recovered from retail common_rel, use it. The current host walk
 * loop uses the retail fn_8012C660 turn-zone selector when those slots are
 * present. If no record is available, classify the loaded model's motion bank
 * as a visual fallback only. Env overrides remain diagnostic. */
static PCPortFieldMotionMap PCPort_LoadFieldMotionMap(void) {
    PCPortFieldMotionMap map;
    const char* packed;
    u8 envRecord[PCPORT_FIELD_MOTION_RECORD_STRIDE];
    u32 key = 0u;
    int haveKey = 0;
    const u8* record = NULL;
    map.idle = -1;
    map.walk = -1;
    map.run = -1;
    map.idleLoop = 0;
    map.walkLoop = 0;
    map.runLoop = 0;
    memset(map.actionMotion, 0xFF, sizeof(map.actionMotion));
    memset(map.actionLoop, 0, sizeof(map.actionLoop));
    memset(map.actionValid, 0, sizeof(map.actionValid));
    map.turnStepPos = PCPORT_FIELD_TURN_DEFAULT_STEP;
    map.turnStepNeg = PCPORT_FIELD_TURN_DEFAULT_STEP;
    map.fromRecord = 0;
    map.fromHeuristic = 0;
    map.fromEnv = 0;
    map.key = 0u;

    if (PCPort_ParseU32Env("PCPORT_FIELD_MOTION_KEY", &key) ||
        PCPort_ParseU32Env("PCPORT_FIELD_CHAR_KEY", &key)) {
        haveKey = 1;
    }

    if (PCPort_ParseFieldMotionRecordEnv(envRecord)) {
        record = envRecord;
        if (haveKey) {
            map.key = key;
        }
    } else {
        if (!haveKey) {
            key = PCPort_FieldMotionDefaultKey();
            haveKey = 1;
        }
        record = PCPort_FieldMotionFindRecordByKey(key);
        if (record != NULL) {
            map.key = key;
        }
    }
    if (record != NULL) {
        PCPort_FieldMotionMapFromRecord(record, &map);
        if (getenv("PCPORT_FIELD_MOTION_RECORD_PROBE") != NULL) {
            PCPort_FieldMotionRecordProbe(record, map.key);
        }
    }

    if (g_engCharLoaded &&
        g_engCharFsysPath[0] != '\0' &&
        g_engCharMember[0] != '\0') {
        PCPortLocomotionSuggestion s;
        if (PCPort_CharAnimSuggestLocomotionMapEx(g_engCharFsysPath,
                                                  g_engCharMember,
                                                  40, &s)) {
            int usedHeuristic = 0;
            if (!map.fromRecord || map.idle < 0 || s.idle >= 0) {
                map.idle = s.idle;
                map.idleLoop = 1;
                usedHeuristic = 1;
            }
            if (!map.fromRecord || map.walk < 0 || s.walk >= 0) {
                map.walk = s.walk;
                map.walkLoop = 1;
                usedHeuristic = 1;
            }
            if (!map.fromRecord || map.run < 0 || s.run >= 0) {
                map.run = s.run;
                map.runLoop = 1;
                usedHeuristic = 1;
            }
            if (usedHeuristic) {
                map.fromHeuristic = 1;
            }
            if (getenv("PCPORT_MOTION_DEBUG") != NULL) {
                printf("[field/motion] heuristic %s :: %s idle=%d walk=%d "
                       "run=%d confirmed=%s cyclic=%u/%u%s\n",
                       g_engCharFsysPath, g_engCharMember,
                       s.idle, s.walk, s.run,
                       s.allConfirmed ? "yes" : "no",
                       s.varyingCyclicCount, s.motionCount,
                       usedHeuristic ? "" : " [diagnostic-only]");
            }
        }
    }

    packed = getenv("PCPORT_FIELD_MOTION_MAP");
    if (packed != NULL && packed[0] != '\0') {
        int a, b, c;
        if (sscanf(packed, "%d,%d,%d", &a, &b, &c) == 3) {
            map.idle = a;
            map.walk = b;
            map.run = c;
            map.fromEnv = 1;
        }
    }
    if (PCPort_ParseIntEnv("PCPORT_IDLE_MOTION", &map.idle)) {
        map.fromEnv = 1;
    }
    if (PCPort_ParseIntEnv("PCPORT_WALK_MOTION", &map.walk)) {
        map.fromEnv = 1;
    }
    if (PCPort_ParseIntEnv("PCPORT_RUN_MOTION", &map.run)) {
        map.fromEnv = 1;
    }
    if (getenv("PCPORT_MOTION_DEBUG") != NULL) {
        printf("[field/motion] map idle=%d(loop=%d) walk=%d(loop=%d) "
               "run=%d(loop=%d)%s%s%s key=0x%08X\n",
               map.idle, map.idleLoop, map.walk, map.walkLoop,
               map.run, map.runLoop,
               map.fromRecord ? " [record]" : "",
               map.fromHeuristic ? " [heuristic]" : "",
               map.fromEnv ? " [env]" : "",
               map.key);
    }
    return map;
}

static const char* PCPort_FieldMotionMapSourceName(const PCPortFieldMotionMap* map) {
    if (map == NULL) {
        return "missing";
    }
    if (map->fromEnv) {
        return "env-override";
    }
    if (map->fromRecord && map->fromHeuristic) {
        return "record+heuristic-fill";
    }
    if (map->fromRecord) {
        return "game-record";
    }
    if (map->fromHeuristic) {
        return "motion-bank-heuristic";
    }
    return "missing";
}

static int PCPort_FieldMotionMapIsGameBacked(const PCPortFieldMotionMap* map) {
    return map != NULL && map->fromRecord && !map->fromEnv;
}

static int PCPort_FieldMotionMapHasActionSlots(const PCPortFieldMotionMap* map) {
    return map != NULL && map->actionValid[1] && map->actionValid[2] &&
           map->actionValid[3] && map->actionValid[4];
}

static int PCPort_FieldMotionForActionSlot(const PCPortFieldMotionMap* map,
                                           int actionSlot) {
    if (map == NULL || actionSlot < 1 || actionSlot > 8 ||
        !map->actionValid[actionSlot]) {
        return -1;
    }
    return map->actionMotion[actionSlot];
}

static int PCPort_FieldMotionForRole(const PCPortFieldMotionMap* map,
                                     PCPortFieldMotionRole role) {
    if (map == NULL) {
        return -1;
    }
    switch (role) {
    case PCPORT_FIELD_MOTION_IDLE: return map->idle;
    case PCPORT_FIELD_MOTION_WALK: return map->walk;
    case PCPORT_FIELD_MOTION_RUN:  return map->run;
    default:                       return -1;
    }
}

static const char* PCPort_FieldMotionRoleName(PCPortFieldMotionRole role) {
    switch (role) {
    case PCPORT_FIELD_MOTION_IDLE: return "idle";
    case PCPORT_FIELD_MOTION_WALK: return "walk";
    case PCPORT_FIELD_MOTION_RUN:  return "run";
    default:                       return "unknown";
    }
}

static void PCPort_FieldComputeMoveVector(f32 camYaw, f32 sx, f32 sy,
                                          f32* outMvx, f32* outMvz,
                                          f32* outMag) {
    f32 fwdX = sinf(camYaw);
    f32 fwdZ = -cosf(camYaw);
    f32 rightX = cosf(camYaw);
    f32 rightZ = sinf(camYaw);
    f32 mvx = fwdX * sy + rightX * sx;
    f32 mvz = fwdZ * sy + rightZ * sx;
    f32 moveMag = sqrtf(mvx * mvx + mvz * mvz);

    if (moveMag > 1.0f) {
        mvx /= moveMag;
        mvz /= moveMag;
        moveMag = 1.0f;
    }
    if (outMvx != NULL) *outMvx = mvx;
    if (outMvz != NULL) *outMvz = mvz;
    if (outMag != NULL) *outMag = moveMag;
}

static PCPortFieldMotionRole PCPort_FieldMotionRoleForInput(f32 moveMag,
                                                            u16 btn,
                                                            u8 triggerRight,
                                                            int forceRun,
                                                            int forceWalk,
                                                            f32 runThreshold,
                                                            f32 walkDeadzone) {
    if (moveMag <= walkDeadzone) {
        return PCPORT_FIELD_MOTION_IDLE;
    }
    if ((((moveMag >= runThreshold) ||
          (btn & (GCN_PAD_BUTTON_B | GCN_PAD_TRIGGER_R)) ||
          triggerRight >= 200u ||
          forceRun) &&
         !forceWalk)) {
        return PCPORT_FIELD_MOTION_RUN;
    }
    return PCPORT_FIELD_MOTION_WALK;
}

static f32 PCPort_FieldYawForMove(f32 mvx, f32 mvz) {
    /* Model facing matches the live walk-loop convention: forward/north
     * movement uses yaw PI, backward/south uses yaw 0. */
    return atan2f(-mvx, mvz);
}

static f32 PCPort_FieldAngleAbsDelta(f32 a, f32 b) {
    f32 d = a - b;
    while (d > 3.14159265f) d -= 6.28318530f;
    while (d < -3.14159265f) d += 6.28318530f;
    return d < 0.0f ? -d : d;
}

static f32 PCPort_FieldAngleDelta(f32 fromYaw, f32 toYaw) {
    f32 d = toYaw - fromYaw;
    while (d > 3.14159265f) d -= 6.28318530f;
    while (d < -3.14159265f) d += 6.28318530f;
    return d;
}

static f32 PCPort_FieldTurnAmountForYawChange(f32 fromYaw, f32 toYaw) {
    return PCPort_FieldAngleDelta(fromYaw, toYaw) / 1.57079633f;
}

static void PCPort_FieldMotionChoiceForTurnAmount(f32 turnAmount,
                                                  PCPortFieldMotionChoice* outChoice) {
    PCPortFieldMotionChoice c;
    if (turnAmount > PCPORT_FIELD_TURN_CLAMP_POS) {
        turnAmount = PCPORT_FIELD_TURN_CLAMP_POS;
    }
    c.fromSlot = 3;
    c.toSlot = 1;
    c.blend = PCPORT_FIELD_TURN_ONE;
    c.zoneName = "near-zero";
    if (turnAmount < PCPORT_FIELD_TURN_HARD_NEG) {
        c.fromSlot = 4;
        c.toSlot = -1;
        c.blend = PCPORT_FIELD_TURN_ONE;
        c.zoneName = "hard-left";
    } else if (turnAmount < 0.0f) {
        c.fromSlot = 4;
        c.toSlot = 1;
        c.blend = (turnAmount - PCPORT_FIELD_TURN_HARD_NEG) /
                  PCPORT_FIELD_TURN_NEAR_POS;
        c.zoneName = "left-to-straight";
    } else if (turnAmount < PCPORT_FIELD_TURN_NEAR_POS) {
        c.fromSlot = 3;
        c.toSlot = 1;
        c.blend = PCPORT_FIELD_TURN_ONE -
                  PCPORT_FIELD_TURN_STRAIGHT_SCALE * turnAmount;
        c.zoneName = "straight-to-right";
    } else if (turnAmount < PCPORT_FIELD_TURN_ONE) {
        c.fromSlot = 3;
        c.toSlot = -1;
        c.blend = 0.0f;
        c.zoneName = "right";
    } else {
        c.fromSlot = 3;
        c.toSlot = 2;
        c.blend = turnAmount - PCPORT_FIELD_TURN_ONE;
        c.zoneName = "right-to-hard-right";
    }
    if (c.blend < 0.0f) c.blend = 0.0f;
    if (c.blend > 1.0f) c.blend = 1.0f;
    if (outChoice != NULL) {
        *outChoice = c;
    }
}

static int PCPort_FieldMotionChoiceDominantSlot(const PCPortFieldMotionChoice* c) {
    if (c == NULL) {
        return -1;
    }
    if (c->toSlot < 0) {
        return c->fromSlot;
    }
    return c->blend >= 0.5f ? c->toSlot : c->fromSlot;
}

static int PCPort_FieldMotionSelectLive(const PCPortFieldMotionMap* map,
                                        PCPortFieldMotionRole role,
                                        f32 turnAmount,
                                        PCPortFieldMotionChoice* outChoice,
                                        int* outActionSlot) {
    PCPortFieldMotionChoice choice;
    int actionSlot = -1;
    int motion = -1;

    memset(&choice, 0, sizeof(choice));
    choice.fromSlot = -1;
    choice.toSlot = -1;
    choice.zoneName = "role";

    if (role == PCPORT_FIELD_MOTION_WALK &&
        map != NULL && !map->fromEnv &&
        PCPort_FieldMotionMapHasActionSlots(map)) {
        PCPort_FieldMotionChoiceForTurnAmount(turnAmount, &choice);
        actionSlot = PCPort_FieldMotionChoiceDominantSlot(&choice);
        if (actionSlot >= 2 && actionSlot <= 4) {
            motion = PCPort_FieldMotionForActionSlot(map, actionSlot);
        } else {
            actionSlot = -1;
        }
    }
    if (motion < 0) {
        actionSlot = -1;
        motion = PCPort_FieldMotionForRole(map, role);
    }
    if (outChoice != NULL) {
        *outChoice = choice;
    }
    if (outActionSlot != NULL) {
        *outActionSlot = actionSlot;
    }
    return motion;
}

static f32 PCPort_FieldNormalizeYaw(f32 yaw) {
    while (yaw > 3.14159265f) yaw -= 6.28318530f;
    while (yaw < -3.14159265f) yaw += 6.28318530f;
    return yaw;
}

static f32 PCPort_FieldMotionTurnStep(const PCPortFieldMotionMap* map,
                                      f32 delta,
                                      f32 frameStep) {
    f32 step = (delta < 0.0f) ?
        ((map != NULL) ? map->turnStepNeg : PCPORT_FIELD_TURN_DEFAULT_STEP) :
        ((map != NULL) ? map->turnStepPos : PCPORT_FIELD_TURN_DEFAULT_STEP);
    if (step <= 0.0f) {
        step = PCPORT_FIELD_TURN_DEFAULT_STEP;
    }
    if (frameStep < 1.0f) frameStep = 1.0f;
    if (frameStep > 4.0f) frameStep = 4.0f;
    step *= frameStep;
    if (step > 3.14159265f) step = 3.14159265f;
    return step;
}

static f32 PCPort_FieldStepYawToward(const PCPortFieldMotionMap* map,
                                     f32 fromYaw,
                                     f32 toYaw,
                                     f32 frameStep) {
    f32 delta = PCPort_FieldAngleDelta(fromYaw, toYaw);
    f32 step = PCPort_FieldMotionTurnStep(map, delta, frameStep);
    if (delta > step) {
        return PCPort_FieldNormalizeYaw(fromYaw + step);
    }
    if (delta < -step) {
        return PCPort_FieldNormalizeYaw(fromYaw - step);
    }
    return PCPort_FieldNormalizeYaw(toYaw);
}

typedef struct PCPortFieldDoorStoryState {
    int enabled;
    int ready;
    int active;
    int done;
    int pendingWarpFloor;
    int sawOpen;
    int sawAdvance;
    int sawClose;
    int msgCtxLinked;
    int scriptCbLinked;
    u32 npcIndex;
    u32 storyStep;
    u32 cutsceneState;
    u32 talkResult;
    u32 npcDrawn;
    f32 npcPos[3];
    PCPortFieldNpcScriptPlacement npcPlacement;
    PCPortMessageBoxState msg;
    PCPortScriptMsgContext* msgCtx;
} PCPortFieldDoorStoryState;

static const char* kPCPortOutskirtDoorStoryPages[] = {
    "Welcome to Outskirt Stand. The opening story event is now linked to this door.",
    "The NPC interaction closes by recording the next story and cutscene markers."
};

static int PCPort_FieldDoorStoryEnabledForCurrentMap(void) {
    if (g_pcCurrentFieldFloor != PC_FLOOR_OUTSKIRT) {
        return 0;
    }
    return g_pcStoryFieldSmoke.active ||
           (g_pcEnterFieldWalk && !g_pcFieldWarpSmokeActive) ||
           getenv("PCPORT_FIELD_STORY_EVENTS") != NULL;
}

static void PCPort_FieldDoorStoryShutdown(PCPortFieldDoorStoryState* story) {
    if (story == NULL || !story->enabled) {
        return;
    }
    PCPort_FieldNpcModelRelease();
    PCPort_PeopleHostClear();
    memset(story, 0, sizeof(*story));
    story->pendingWarpFloor = -1;
}

static int PCPort_FieldDoorStoryPrepare(PCPortFieldDoorStoryState* story) {
    static const char* kArchive = "orig/GC6E01/disc/files/S1_out.fsys";
    PCPortFieldNpcScriptPlacement npcPlacement;
    PeopleEntry* npc;
    char deps[8][64];
    char depSummary[256];
    char* npcMember = NULL;
    u32 npcIndex;
    int rawDepCount = 0;
    int resolvedDepCount;
    int storedDepCount;
    int i;

    if (story == NULL) {
        return 0;
    }
    memset(story, 0, sizeof(*story));
    story->pendingWarpFloor = -1;
    if (!PCPort_FieldDoorStoryEnabledForCurrentMap()) {
        return 0;
    }
    story->enabled = 1;

    memset(deps, 0, sizeof(deps));
    depSummary[0] = '\0';
    resolvedDepCount = PCPort_FieldNpcReadDependencyList(
        kArchive, deps, (int)(sizeof(deps) / sizeof(deps[0])),
        &rawDepCount, depSummary, sizeof(depSummary));
    storedDepCount = resolvedDepCount;
    if (storedDepCount > (int)(sizeof(deps) / sizeof(deps[0]))) {
        storedDepCount = (int)(sizeof(deps) / sizeof(deps[0]));
    }
    if (rawDepCount < 3 || resolvedDepCount < 3) {
        printf("[field/story] S1_out dependency list unavailable raw=%d resolved=%d summary=%s\n",
               rawDepCount, resolvedDepCount, depSummary);
        return 0;
    }

    for (i = 0; i < storedDepCount; ++i) {
        if (strstr(deps[i], "truck") == NULL &&
            strstr(deps[i], "bike") == NULL) {
            npcMember = deps[i];
            break;
        }
    }
    if (npcMember == NULL && storedDepCount > 0) {
        npcMember = deps[0];
    }
    if (npcMember == NULL || npcMember[0] == '\0') {
        printf("[field/story] no renderable S1_out NPC dependency from %s\n",
               depSummary);
        return 0;
    }

    if (!PCPort_FieldNpcReadScriptPlacement(kArchive, &npcPlacement)) {
        printf("[field/story] no S1_out NPC placement marker 0x%08X\n",
               PCPORT_S1_OUT_NPC_ACTOR_TOKEN);
        return 0;
    }
    npcIndex = (npcPlacement.actorToken >> 16) & 0xFFFFu;
    if (npcIndex == 0u) {
        npcIndex = 1u;
    }

    PCPort_PeopleHostClear();
    PCPort_PeopleOpenSetPlacement(PC_FLOOR_OUTSKIRT, npcIndex, &npcPlacement);
    fn_8018E050(PC_FLOOR_OUTSKIRT, npcIndex, npcPlacement.actionWord);
    PCPort_PeopleOpenClearPlacement();

    npc = PCPort_PeopleHostFindExact(PC_FLOOR_OUTSKIRT, npcIndex);
    if (npc == NULL ||
        npc->field_34 != npcPlacement.actorToken ||
        npc->field_38 != npcPlacement.actionWord ||
        npc->scriptRef != (void*)(unsigned long)npcPlacement.tokenOffset ||
        npc->state != PEOPLE_STATE_INTERACTING) {
        printf("[field/story] NPC open/setup failed index=%u setupCount=%u\n",
               npcIndex, g_pcPeopleOpenSetupCount);
        PCPort_PeopleHostClear();
        return 0;
    }
    PCPort_PeopleHostGetPosition(npc, story->npcPos);
    if (!PCPort_Vec3Near(story->npcPos, npcPlacement.pos)) {
        printf("[field/story] NPC placement mismatch at (%.1f,%.1f,%.1f)\n",
               story->npcPos[0], story->npcPos[1], story->npcPos[2]);
        PCPort_PeopleHostClear();
        return 0;
    }
    if (!PCPort_LoadFieldNpcModel(kArchive, npcMember)) {
        PCPort_PeopleHostClear();
        return 0;
    }

    story->npcPlacement = npcPlacement;
    story->npcIndex = npcIndex;
    story->ready = 1;
    if (g_pcStoryFieldSmoke.active) {
        g_pcStoryFieldSmoke.doorStoryReady = 1;
        g_pcStoryFieldSmoke.doorStoryNpcIndex = npcIndex;
    }
    printf("[field/story] S1_out door story ready: npc=%s index=%u marker=0x%08X pos=(%.1f,%.1f,%.1f)\n",
           npcMember,
           npcIndex,
           npcPlacement.actorToken,
           story->npcPos[0],
           story->npcPos[1],
           story->npcPos[2]);
    return 1;
}

static int PCPort_FieldDoorStoryBegin(PCPortFieldDoorStoryState* story,
                                      int pendingWarpFloor) {
    PeopleEntry* npc;
    if (story == NULL || !story->ready || story->active || story->done) {
        return 0;
    }
    npc = PCPort_PeopleHostFindExact(PC_FLOOR_OUTSKIRT, story->npcIndex);
    if (npc == NULL) {
        return 0;
    }
    story->talkResult = fn_801812E8(PC_FLOOR_OUTSKIRT, story->npcIndex, 1u);
    if (story->talkResult != 1u) {
        printf("[field/story] talk start failed for npc index=%u result=%u\n",
               story->npcIndex, story->talkResult);
        return 0;
    }

    PCPort_MessageBoxInit(
        &story->msg,
        kPCPortOutskirtDoorStoryPages,
        (int)(sizeof(kPCPortOutskirtDoorStoryPages) /
              sizeof(kPCPortOutskirtDoorStoryPages[0])),
        5);
    PCPort_MessageBoxSeedScriptContext(
        &story->msg,
        story->npcIndex,
        kPCPortOutskirtDoorStoryPages[0]);
    story->msgCtx = PCPort_MessageBoxScriptContext();
    story->msgCtxLinked =
        story->msgCtx != NULL &&
        story->msgCtx == (PCPortScriptMsgContext*)(lbl_803A9768 +
                                                   PCPORT_MSGBOX_SLOT_BASE) &&
        story->msgCtx->magic == PCPORT_MSGBOX_CONTEXT_MAGIC &&
        story->msgCtx->speakerId == story->npcIndex &&
        story->msgCtx->pageCount == (u32)story->msg.pageCount &&
        story->msgCtx->active == 1u;
    memset(lbl_803A95E8, 0, 0x138u);
    memset(lbl_803A9720, 0, 0x48u);
    fn_80053778();
    story->scriptCbLinked =
        *(void**)lbl_803A95E8 == (void*)story->msgCtx &&
        *(void**)lbl_803A9720 == (void*)story->msgCtx &&
        *(u32*)(lbl_803A95E8 + 8) == 1u &&
        *(u32*)(lbl_803A9720 + 8) == 1u;
    if (!story->msgCtxLinked || !story->scriptCbLinked) {
        printf("[field/story] message/script context not linked msg=%d cb=%d\n",
               story->msgCtxLinked, story->scriptCbLinked);
        return 0;
    }

    story->pendingWarpFloor = pendingWarpFloor;
    story->active = 1;
    story->sawOpen = 1;
    if (g_pcStoryFieldSmoke.active) {
        g_pcStoryFieldSmoke.doorStoryOpened = 1;
    }
    printf("[field/story] train door opened NPC dialogue; pending floor %d\n",
           pendingWarpFloor);
    return 1;
}

static void PCPort_FieldDoorStoryTick(PCPortFieldDoorStoryState* story,
                                      u16 pressed,
                                      int autoAdvance) {
    int beforeAdvance;
    int beforeClose;
    int pageLen;

    if (story == NULL || !story->active) {
        return;
    }
    beforeAdvance = story->msg.advanceCount;
    beforeClose = story->msg.closeCount;
    pageLen = PCPort_MessageBoxPageLen(&story->msg);

    if (autoAdvance) {
        if (story->msg.visibleChars >= pageLen) {
            pressed |= GCN_PAD_BUTTON_A;
        }
    }

    PCPort_MessageBoxTick(&story->msg, pressed);
    if (story->msg.advanceCount > beforeAdvance) {
        story->sawAdvance = 1;
        if (g_pcStoryFieldSmoke.active) {
            g_pcStoryFieldSmoke.doorStoryAdvanced = 1;
        }
    }
    if (story->msg.closeCount > beforeClose) {
        story->sawClose = 1;
        story->done = 1;
        story->active = 0;
        story->storyStep = 1u;
        story->cutsceneState = 2u;
        if (g_pcStoryFieldSmoke.active) {
            g_pcStoryFieldSmoke.doorStoryClosed = 1;
            g_pcStoryFieldSmoke.storyStep = story->storyStep;
            g_pcStoryFieldSmoke.cutsceneState = story->cutsceneState;
            g_pcStoryFieldSmoke.doorStoryWarpFloor = story->pendingWarpFloor;
        }
    }
    PCPort_MessageBoxSyncScriptContext(
        &story->msg,
        story->storyStep,
        story->cutsceneState);
}

static void PCPort_FieldDoorStoryDraw(PCPortFieldDoorStoryState* story) {
    if (story == NULL || !story->ready) {
        return;
    }
    story->npcDrawn += RenderFieldStaticArchive(&g_engNpcArchive,
                                                g_engNpcRoot,
                                                story->npcPos[0],
                                                story->npcPos[1],
                                                story->npcPos[2],
                                                story->npcPlacement.yaw,
                                                1.15f);
    if (g_pcStoryFieldSmoke.active) {
        g_pcStoryFieldSmoke.doorStoryNpcDrawn = (int)story->npcDrawn;
    }
    DrawFieldMessageBox(&story->msg, story->active ? "NPC" : NULL);
}

/* Third-person "walk the room" mode for --field (PCPORT_FIELD_WALK). The player
 * is a box avatar that moves on the WZX floor with wall blocking; the camera
 * orbits behind. Left stick walks (camera-relative), arrows/C-stick orbit, the
 * avatar snaps to the floor each step and slides along walls. */
/* Returns >=0 = a target floor id to warp to (the loop tripped an exit
 * trigger), or -1 if the window was closed / frame cap hit without a warp.
 * `spawn` seeds the player XZ/Y position (snapped to the floor); pass NULL to
 * use PCPORT_CAM_EYE / origin. */
static int RunFieldWalkLoop(GLFWwindow* window, const char* dumpPath,
                            int frameCap, int colWire, const f32 spawn[3]) {
    PADStatus pads[4];
    f32 ppos[3] = { 0.0f, 0.0f, 0.0f };
    /* pyaw=PI rotates the ken_b1 model 180 degrees (model faces -Z at yaw=PI).
     * camYaw=0 puts the camera at +Z behind him, looking toward -Z (toward the
     * Outskirt Stand diner), matching the real game's S1_out entry framing. */
    f32 pyaw = 3.14159265f;
    f32 camYaw = 0.0f, camPitch = 0.20f;
    const f32 up[3] = { 0.0f, 1.0f, 0.0f };
    const f32 ORBIT = 0.04f;
    const f32 PLAYER_H = 30.0f, PLAYER_R = 10.0f;
    const char* dEnv = getenv("PCPORT_CAM_DIST");
    const char* hEnv = getenv("PCPORT_CAM_HEIGHT");
    const char* walkSpeedEnv = getenv("PCPORT_WALK_SPEED");
    const char* runSpeedEnv = getenv("PCPORT_RUN_SPEED");
    const char* runThreshEnv = getenv("PCPORT_RUN_THRESHOLD");
    const char* deadzoneEnv = getenv("PCPORT_WALK_DEADZONE");
    /* Camera defaults tuned to match the real game's Outskirt Stand entry framing:
     * Wes seen from behind at roughly head height, moderate distance.
     * camDist=75, camHigh=25, camPitch=0.20 rad: eye ~38 units above floor (just
     * above Wes's head at ~22 units, scale 1.3), 73 units behind at pitch=0.20. */
    f32 camDist = (dEnv != NULL) ? (f32)atof(dEnv) : 75.0f;
    f32 camHigh = (hEnv != NULL) ? (f32)atof(hEnv) : 25.0f;
    f32 walkSpeed = (walkSpeedEnv != NULL && walkSpeedEnv[0]) ? (f32)atof(walkSpeedEnv) : 4.0f;
    f32 runSpeed = (runSpeedEnv != NULL && runSpeedEnv[0]) ? (f32)atof(runSpeedEnv) : 7.0f;
    f32 runThreshold = (runThreshEnv != NULL && runThreshEnv[0]) ? (f32)atof(runThreshEnv) : 0.85f;
    f32 walkDeadzone = (deadzoneEnv != NULL && deadzoneEnv[0]) ? (f32)atof(deadzoneEnv) : 0.05f;
    int autopan = g_pcStoryFieldSmoke.active ||
                  g_pcFieldWarpSmokeActive ||
                  getenv("PCPORT_FIELD_AUTOPAN") != NULL;
    int forceRun = g_pcStoryFieldSmoke.active ||
                   g_pcFieldWarpSmokeActive ||
                   getenv("PCPORT_FORCE_RUN") != NULL ||
                   getenv("PCPORT_FIELD_AUTORUN") != NULL;
    int forceWalk = getenv("PCPORT_FORCE_WALK") != NULL;
    int motionDebug = getenv("PCPORT_MOTION_DEBUG") != NULL;
    int currentMotion = -999;
    PCPortFieldMotionMap motionMap;
    int reportedMissingMotionMap = 0;
    int frame = 0;
    int warpTo = -1;
    int graceFrames = (g_pcFieldWarpSmokeActive ||
                       g_pcStoryFieldSmoke.active) ? 0 : 30;
                            /* ignore exit triggers right after a (re)spawn */
    f32 spawnY;
    double g_walkPrevTime = 0.0; /* real-time clock for animation frame-step */
    u16 padPrev = 0u;
    PCPortFieldStartMenuState startMenu;
    PCPortFieldDoorStoryState doorStory;

    if (walkSpeed < 0.0f) walkSpeed = 0.0f;
    if (runSpeed < walkSpeed) runSpeed = walkSpeed;
    if (runThreshold < 0.0f) runThreshold = 0.0f;
    if (runThreshold > 1.0f) runThreshold = 1.0f;
    if (walkDeadzone < 0.0f) walkDeadzone = 0.0f;

    memset(pads, 0, sizeof(pads));
    memset(&doorStory, 0, sizeof(doorStory));
    doorStory.pendingWarpFloor = -1;
    PCPort_FieldStartMenuInit(&startMenu);
    if (getenv("PCPORT_FIELD_WES") != NULL || g_pcEnterFieldWalk ||
        getenv("PCPORT_FIELD_WALK") != NULL) {
        PCPort_LoadFieldCharacter();   /* loads ken_b1 once; no-op on later maps */
    }
    motionMap = PCPort_LoadFieldMotionMap();
    if (g_engCharLoaded && !PCPort_FieldMotionMapHasActionSlots(&motionMap)) {
        printf("[field/motion] source=%s; retail field action-slot selection is unavailable\n",
               PCPort_FieldMotionMapSourceName(&motionMap));
    }
    if (spawn != NULL) {
        ppos[0] = spawn[0]; ppos[1] = spawn[1]; ppos[2] = spawn[2];
    }
    { /* PCPORT_CAM_EYE x,_,z still overrides the spawn XZ (tuning). */
        const char* ce = getenv("PCPORT_CAM_EYE");
        if (ce != NULL) { f32 yy; sscanf(ce, "%f,%f,%f", &ppos[0], &yy, &ppos[2]); }
    }
    if (PCPort_FieldColFloorAt(ppos[0], ppos[2], 1.0e5f, 1.0e9f, &spawnY)) {
        ppos[1] = spawnY;
    }
    if (g_pcStoryFieldSmoke.active) {
        g_pcStoryFieldSmoke.spawnSet = 1;
        g_pcStoryFieldSmoke.spawn[0] = ppos[0];
        g_pcStoryFieldSmoke.spawn[1] = ppos[1];
        g_pcStoryFieldSmoke.spawn[2] = ppos[2];
    }
    printf("[field/walk] spawn=(%.1f,%.1f,%.1f). Left stick walks, arrows/C-stick "
           "orbit camera. (%d exit trigger(s))%s\n", ppos[0], ppos[1], ppos[2],
           PCPort_FieldExitCount(), autopan ? " [AUTOPAN]" : "");
    PCPort_FieldDoorStoryPrepare(&doorStory);

    g_walkPrevTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        f32 mvx, mvz, eye[3], interest[3], floorY;
        f32 moveMag, moveSpeed;
        f32 prevx, prevz;
        f32 frameStep;
        f32 desiredYaw;
        f32 turnAmount = 0.0f;
        double nowTime;
        u16 btn;
        u16 padPressed;
        f32 sx, sy;
        int moving, running;
        PCPortFieldMotionRole role;
        int storyWasActive;

        VIWaitForRetrace_PC();
        /* Real-time frame step: elapsed seconds * 60 = game frames elapsed.
         * This keeps animation at GC speed (1 frame/tick) even when rendering
         * takes longer than 1/60 s (e.g. large exterior scenes like S1_out).
         * Fall back to 1.0 (one GC tick) when glfwGetTime is frozen/unavailable
         * (headless mode, first frame, or very fast loops where delta rounds to 0). */
        nowTime = glfwGetTime();
        frameStep = (f32)((nowTime - g_walkPrevTime) * 60.0);
        if (frameStep < 0.001f) frameStep = 1.0f;   /* frozen clock fallback */
        g_walkPrevTime = nowTime;

        /* Advance scene-ambient animation (signpost swing, cloud UV scroll) each
         * frame at real-time speed.  No-op if the map has no animjoint (static
         * maps).  Harvest follows immediately so the UV table is current before
         * the next RenderJointTree call. */
        PCPort_FieldAnimTick(frameStep);
        PCPort_FieldAnimHarvestTexUV(&g_engTitleArchive, g_engTitleRootJoint);

        PADRead(pads);
        btn = pads[0].button;
        padPressed = (u16)(btn & ~padPrev);
        padPrev = btn;
        storyWasActive = doorStory.active;
        if (doorStory.active) {
            PCPort_FieldDoorStoryTick(&doorStory,
                                      padPressed,
                                      g_pcStoryFieldSmoke.active ||
                                      getenv("PCPORT_FIELD_STORY_AUTO") != NULL);
        } else {
            PCPort_FieldStartMenuTick(&startMenu, padPressed);
        }
        sx = (f32)pads[0].stickX / 112.0f;
        sy = (f32)pads[0].stickY / 112.0f;

        if (storyWasActive || doorStory.active || startMenu.active) {
            sx = 0.0f;
            sy = 0.0f;
            btn = 0u;
        } else {
            /* Camera orbit (arrows + C-stick). */
            if (btn & GCN_PAD_BUTTON_LEFT)  camYaw   -= ORBIT;
            if (btn & GCN_PAD_BUTTON_RIGHT) camYaw   += ORBIT;
            if (btn & GCN_PAD_BUTTON_UP)    camPitch += ORBIT;
            if (btn & GCN_PAD_BUTTON_DOWN)  camPitch -= ORBIT;
            camYaw   += (f32)pads[0].substickX / 112.0f * ORBIT;
            camPitch += (f32)pads[0].substickY / 112.0f * ORBIT;
            if (camPitch >  1.30f) camPitch =  1.30f;
            if (camPitch < -0.20f) camPitch = -0.20f;
            if (autopan) {
                sy = g_pcFieldWarpSmokeActive ? g_pcFieldWarpSmokeStickY : 0.7f;
                camYaw += 0.01f;
            }
        }

        PCPort_FieldComputeMoveVector(camYaw, sx, sy, &mvx, &mvz, &moveMag);
        role = PCPort_FieldMotionRoleForInput(moveMag, btn,
                                              pads[0].triggerRight,
                                              forceRun, forceWalk,
                                              runThreshold, walkDeadzone);
        moving = role != PCPORT_FIELD_MOTION_IDLE;
        running = role == PCPORT_FIELD_MOTION_RUN;
        moveSpeed = running ? runSpeed : walkSpeed;

        prevx = ppos[0]; prevz = ppos[2];
        desiredYaw = pyaw;
        if (moving) {
            desiredYaw = PCPort_FieldYawForMove(mvx, mvz);
            turnAmount = PCPort_FieldTurnAmountForYawChange(pyaw, desiredYaw);
            ppos[0] += mvx * moveSpeed;
            ppos[2] += mvz * moveSpeed;
            /* Ease toward the move direction so fn_8012C660 turn nodes remain
             * visible across direction changes. */
            pyaw = PCPort_FieldStepYawToward(&motionMap, pyaw, desiredYaw,
                                             frameStep);
        }

        /* Drive turns from the retail fn_8012C660 action-record node selector
         * when present. Straight idle/walk/run stay on the visible role cycles
         * from the character motion bank until the exact HSD blend path lands. */
        if (getenv("PCPORT_NO_CHAR_ANIM") == NULL && g_engCharLoaded) {
            PCPortFieldMotionChoice choice;
            int actionSlot = -1;
            int nextMotion = PCPort_FieldMotionSelectLive(&motionMap, role,
                                                          turnAmount,
                                                          &choice,
                                                          &actionSlot);
            if (nextMotion < 0) {
                if (!reportedMissingMotionMap && motionDebug) {
                    printf("[field/motion] role %d actionSlot=%d has no mapped motion "
                           "(motion-bank heuristic failed; env motion overrides "
                           "are diagnostic)\n", (int)role, actionSlot);
                    reportedMissingMotionMap = 1;
                }
            } else if (nextMotion != currentMotion) {
                if (motionDebug) {
                    if (actionSlot >= 0) {
                        printf("[field/motion] turn=%.2f zone=%s slots=%d->%d blend=%.2f dominant=%d motion=%d (mag=%.2f speed=%.2f)\n",
                               turnAmount, choice.zoneName, choice.fromSlot,
                               choice.toSlot, choice.blend, actionSlot,
                               nextMotion, moveMag, moveSpeed);
                    } else {
                        printf("[field/motion] %s -> motion %d (mag=%.2f speed=%.2f)\n",
                               PCPort_FieldMotionRoleName(role),
                               nextMotion, moveMag, moveSpeed);
                    }
                }
                PCPort_CharAnimSetMotion(nextMotion);
                currentMotion = nextMotion;
            }
        }

        /* Block at walls (slide), then snap to the floor under the new spot. If
         * there is no floor there (walked off a ledge), revert the step. */
        PCPort_FieldColResolveXZ(&ppos[0], &ppos[2],
                                 ppos[1] + 2.0f, ppos[1] + PLAYER_H, PLAYER_R);
        if (PCPort_FieldColFloorAt(ppos[0], ppos[2], ppos[1] + PLAYER_H,
                                   PLAYER_H, &floorY)) {
            ppos[1] = floorY;
        } else {
            ppos[0] = prevx; ppos[2] = prevz;
        }

        /* Door / exit trigger: if the player walked into an exit's approach
         * zone (after a short post-spawn grace), break out and warp. */
        if (startMenu.active || doorStory.active) {
            /* Menus pause field trigger crossing until control returns. */
        } else if (graceFrames > 0) {
            graceFrames--;
        } else {
            int hit = PCPort_FieldExitCheck(ppos[0], ppos[1], ppos[2], mvx, mvz);
            if (hit >= 0) {
                PCPortFieldExit ex;
                if (PCPort_FieldExitGet(hit, &ex)) {
                    printf("[field/walk] exit %d tripped at (%.1f,%.1f,%.1f) "
                           "-> floor %d\n", hit, ppos[0], ppos[1], ppos[2],
                           ex.targetFloor);
                    if (doorStory.ready && !doorStory.done &&
                        ex.targetFloor == PC_FLOOR_OUTSKIRT_SHOP &&
                        PCPort_FieldDoorStoryBegin(&doorStory,
                                                   ex.targetFloor)) {
                        /* Story dialogue owns the transition; the warp happens
                         * after the message closes. */
                    } else {
                        warpTo = ex.targetFloor;
                        break;
                    }
                }
            }
        }
        if (doorStory.done && doorStory.pendingWarpFloor >= 0) {
            warpTo = doorStory.pendingWarpFloor;
            break;
        }

        /* Orbit camera behind the player. */
        eye[0] = ppos[0] - sinf(camYaw) * cosf(camPitch) * camDist;
        eye[2] = ppos[2] + cosf(camYaw) * cosf(camPitch) * camDist;
        eye[1] = ppos[1] + camHigh + sinf(camPitch) * camDist;
        interest[0] = ppos[0];
        interest[1] = ppos[1] + PLAYER_H * 0.6f;
        interest[2] = ppos[2];
        BuildViewMatrixLookAt(eye, interest, up, g_engTitleCamera.viewMatrix);

        PCPort_EngineTitleRenderFrame();
        /* Player avatar: the real skinned Wes model (ken_b1), loaded whenever
         * PCPORT_FIELD_WALK or PCPORT_FIELD_WES is set (or g_pcEnterFieldWalk).
         * The model is ~17 units tall in its own space; PCPORT_WES_SCALE maps it
         * to the room scale (PLAYER_H=30), PCPORT_WES_YOFF lifts the feet to the
         * floor, PCPORT_WES_YAWOFF aligns its facing with the move direction. */
        if (g_engCharLoaded) {
            const char* sEnv = getenv("PCPORT_WES_SCALE");
            const char* yEnv = getenv("PCPORT_WES_YOFF");
            const char* aEnv = getenv("PCPORT_WES_YAWOFF");
            /* Scale 1.3: the ken_b1 model is ~17 units tall; at 1.3 it stands ~22
             * units, matching the real game's overworld character proportions (the
             * real root-joint scale observed by comparing Wes's height relative to
             * the Outskirt Stand diner and gas pump in the sxs_dolphin reference).
             * wYoff scaled proportionally from the old 1.8/6.0 pairing: 6*(1.3/1.8)≈4.3 */
            f32 wScale = (sEnv != NULL && sEnv[0]) ? (f32)atof(sEnv) : 1.3f;
            f32 wYoff  = (yEnv != NULL && yEnv[0]) ? (f32)atof(yEnv) : 4.3f;
            f32 wYaw   = (aEnv != NULL && aEnv[0]) ? (f32)atof(aEnv) : 0.0f;
            RenderFieldCharacter(ppos[0], ppos[1] + wYoff, ppos[2], pyaw + wYaw, wScale,
                                 frameStep);
        } else {
            DrawFieldAvatar(ppos[0], ppos[1], ppos[2], pyaw, PLAYER_H, PLAYER_R);
        }
        PCPort_FieldDoorStoryDraw(&doorStory);
        if (colWire) {
            DrawFieldCollisionWire();
        }
        DrawFieldStartMenuOverlay(&startMenu);
        if (dumpPath != NULL && dumpPath[0] != '\0' &&
            (frameCap > 0 ? frame == frameCap - 1 : frame == 2)) {
            DumpBackbufferTo(dumpPath);
            printf("[field/walk] dumped frame %d (player=%.1f,%.1f,%.1f) to %s\n",
                   frame, ppos[0], ppos[1], ppos[2], dumpPath);
        }
        GSgfxSwapBuffers(0);
        frame++;
        if (frameCap > 0 && frame >= frameCap) {
            break;
        }
    }
    if (g_pcStoryFieldSmoke.active) {
        f32 dx = ppos[0] - g_pcStoryFieldSmoke.spawn[0];
        f32 dz = ppos[2] - g_pcStoryFieldSmoke.spawn[2];
        g_pcStoryFieldSmoke.frames = frame;
        g_pcStoryFieldSmoke.finalPos[0] = ppos[0];
        g_pcStoryFieldSmoke.finalPos[1] = ppos[1];
        g_pcStoryFieldSmoke.finalPos[2] = ppos[2];
        g_pcStoryFieldSmoke.charLoaded = g_engCharLoaded;
        g_pcStoryFieldSmoke.charAnimReady = PCPort_CharAnimReady();
        g_pcStoryFieldSmoke.moved = (dx * dx + dz * dz) > 16.0f;
        g_pcStoryFieldSmoke.exitCount = PCPort_FieldExitCount();
    }
    PCPort_FieldDoorStoryShutdown(&doorStory);
    printf("[field/walk] %d frames (final player=%.1f,%.1f,%.1f)%s\n",
           frame, ppos[0], ppos[1], ppos[2],
           warpTo >= 0 ? " [WARP]" : "");
    if (warpTo < 0) {
        /* Only hold the window when the loop ended (not on a warp; the caller
         * immediately re-enters with the next map). */
        HoldWindowOpen(window);
    }
    return warpTo;
}

static int RunFieldScene(GLFWwindow* window) {
    const char* archive = getenv("PCPORT_FIELD_ARCHIVE");
    const char* capEnv = getenv("PCPORT_MENU_FRAMES");
    const char* dumpPath = getenv("PCPORT_DUMP");
    int frameCap = (capEnv != NULL && atoi(capEnv) > 0) ? atoi(capEnv) : 0;
    int frame = 0;
    int autopan = getenv("PCPORT_FIELD_AUTOPAN") != NULL;
    char path[512];
    PADStatus pads[4];
    /* Free-fly camera state. yaw=0 looks toward -Z (into the room). */
    f32 eye[3] = { 0.0f, 90.0f, 300.0f };
    f32 yaw = 0.0f, pitch = -0.12f;
    const f32 MOVE = 5.0f, TURN = 0.035f, LOOK = 0.045f;
    const f32 up[3] = { 0.0f, 1.0f, 0.0f };
    { /* PCPORT_CAM_PITCH=<rad> pins the look pitch (e.g. -1.4 = look down at the
       * floor) for headless geometry inspection. */
        const char* cp = getenv("PCPORT_CAM_PITCH");
        if (cp != NULL) pitch = (f32) atof(cp);
    }
    if (g_pcStoryFieldSmoke.active && frameCap < 120) {
        frameCap = 120;
    }

    if (archive == NULL || archive[0] == '\0') {
        /* New Game / Continue starts at The Outskirt Stand exterior (S1_out) —
         * the game's first walkable overworld scene. The dev --field path (no
         * menu handoff) still defaults to Wes's hideout interior. */
        archive = g_pcEnterFieldWalk
                      ? "orig/GC6E01/disc/files/S1_out.fsys"
                      : "orig/GC6E01/disc/files/D1_garage_1F.fsys";
    } else if (strchr(archive, '/') == NULL && strchr(archive, '\\') == NULL) {
        /* bare name -> resolve under the disc files dir */
        snprintf(path, sizeof(path), "orig/GC6E01/disc/files/%s.fsys", archive);
        archive = path;
    }

    if (getenv("PCPORT_FIELD_WALK") != NULL || g_pcEnterFieldWalk) {
        /* Third-person walkable mode with MAP WARPS: load -> walk -> (door)
         * warp -> load... until the window closes. The first map is the one
         * named by PCPORT_FIELD_ARCHIVE (or D1_garage_1F); subsequent maps come
         * from the exit triggers' target floor ids via the warp table.
         * (g_pcEnterFieldWalk = New Game menu handoff into the field.) */
        int colWire = getenv("PCPORT_COL_WIRE") != NULL;
        static f32 firstSpawn[3];       /* first map's table spawn (warp default) */
        const f32* spawn = NULL;        /* first map: table spawn / PCPORT_CAM_EYE */
        int floor;

        /* Resolve the starting floor from the archive base name (so its exits
         * load). Fall back to floor 0 (D1_garage_1F). */
        floor = PC_FLOOR_GARAGE_1F;
        {
            int i;
            const char* base = archive;
            const char* s;
            for (s = archive; *s; ++s) {
                if (*s == '/' || *s == '\\') base = s + 1;
            }
            for (i = 0; i < g_pcWarpMapCount; ++i) {
                if (strncmp(base, g_pcWarpMaps[i].fsysName,
                            strlen(g_pcWarpMaps[i].fsysName)) == 0) {
                    floor = g_pcWarpMaps[i].floorId;
                    break;
                }
            }
        }

        /* Load the first map (scene + collision + exits) via the warp path so
         * the load/teardown is uniform. */
        {
            if (!PCPort_FieldWarpTo(floor, firstSpawn)) {
                /* table miss: load whatever archive was named, no exits. */
                if (!PCPort_EngineFieldSetup(archive)) {
                    return 0;
                }
                PCPort_FieldColLoad(archive);
            } else {
                /* spawn at the map's table position (PCPORT_CAM_EYE still
                 * overrides the XZ inside RunFieldWalkLoop for tuning). */
                spawn = firstSpawn;
            }
        }

        for (;;) {
            int next = RunFieldWalkLoop(window, dumpPath, frameCap, colWire,
                                        spawn);
            if (next < 0) {
                break;          /* window closed / frame cap, no warp */
            }
            {
                static f32 warpSpawn[3];
                if (!PCPort_FieldWarpTo(next, warpSpawn)) {
                    break;      /* unknown floor -> stop */
                }
                spawn = warpSpawn;   /* next loop spawns at the warp target */
            }
            if (frameCap > 0) {
                /* In a bounded (headless) run, one warp is enough to prove it. */
                HoldWindowOpen(window);
                break;
            }
        }
        return 1;
    }

    /* Free-fly (non-walk) path: single static map load. */
    if (!PCPort_EngineFieldSetup(archive)) {
        return 0;
    }

    { /* Load the field WZX collision mesh (floor-clamp + debug wireframe). */
        int colTris = PCPort_FieldColLoad(archive);
        if (colTris > 0) {
            f32 cmin[3], cmax[3];
            PCPort_FieldColBounds(cmin, cmax);
            printf("[field] collision: %d triangles  bounds X[%.1f,%.1f] "
                   "Y[%.1f,%.1f] Z[%.1f,%.1f]\n", colTris,
                   cmin[0], cmax[0], cmin[1], cmax[1], cmin[2], cmax[2]);
        } else {
            printf("[field] collision: no WZX mesh found in %s\n", archive);
        }
    }

    { /* optional initial eye placement */
        const char* ce = getenv("PCPORT_CAM_EYE");
        if (ce != NULL) sscanf(ce, "%f,%f,%f", &eye[0], &eye[1], &eye[2]);
    }
    /* Model-view: seed the free-fly camera from the auto-camera framing computed
     * in PCPort_EngineFieldSetup, so the loaded character/model is centered
     * (the loop rebuilds the view from eye/yaw/pitch each frame, so set those,
     * not just the one-shot view matrix). */
    if (g_engModelView) {
        f32 dx = g_engModelViewCenter[0] - g_engModelViewEye[0];
        f32 dy = g_engModelViewCenter[1] - g_engModelViewEye[1];
        f32 dz = g_engModelViewCenter[2] - g_engModelViewEye[2];
        f32 len = (f32)sqrt((double)(dx*dx + dy*dy + dz*dz));
        eye[0] = g_engModelViewEye[0];
        eye[1] = g_engModelViewEye[1];
        eye[2] = g_engModelViewEye[2];
        yaw = (f32)atan2((double)dx, (double)(-dz));
        pitch = (len > 0.001f) ? (f32)asin((double)(dy / len)) : 0.0f;
    }
    memset(pads, 0, sizeof(pads));
    printf("[field] interactive free-fly: W/S forward, A/D strafe (sticks), arrows turn/look, "
           "Z=A rise / X=B sink. Close window to quit.%s\n",
           autopan ? " [AUTOPAN]" : "");

    {
    double freeFlyPrevTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        f32 fwd[3], right[3], interest[3], mv, st, sx, sy;
        f32 frameStep;
        double nowTime;
        u16 btn;

        VIWaitForRetrace_PC();          /* pumps glfwPollEvents -> fresh key state */
        nowTime = glfwGetTime();
        frameStep = (f32)((nowTime - freeFlyPrevTime) * 60.0);
        if (frameStep < 0.001f) frameStep = 1.0f;
        freeFlyPrevTime = nowTime;
        PCPort_FieldAnimTick(frameStep);  /* advance scene-ambient anim (signpost, cloud UV) */
        PCPort_FieldAnimHarvestTexUV(&g_engTitleArchive, g_engTitleRootJoint);

        PADRead(pads);
        btn = pads[0].button;
        sx = (f32)pads[0].stickX / 112.0f;   /* A/D */
        sy = (f32)pads[0].stickY / 112.0f;   /* W/S */

        if (autopan) { sy = 0.6f; yaw += 0.012f; }  /* headless: drift forward + pan */

        if (btn & GCN_PAD_BUTTON_LEFT)  yaw   -= TURN;
        if (btn & GCN_PAD_BUTTON_RIGHT) yaw   += TURN;
        if (btn & GCN_PAD_BUTTON_UP)    pitch += LOOK;
        if (btn & GCN_PAD_BUTTON_DOWN)  pitch -= LOOK;
        yaw   += (f32)pads[0].substickX / 112.0f * LOOK;   /* C-stick (IJKL) looks */
        pitch += (f32)pads[0].substickY / 112.0f * LOOK;
        if (pitch >  1.4f) pitch =  1.4f;
        if (pitch < -1.4f) pitch = -1.4f;

        fwd[0] =  sinf(yaw) * cosf(pitch);
        fwd[1] =  sinf(pitch);
        fwd[2] = -cosf(yaw) * cosf(pitch);
        right[0] = cosf(yaw); right[1] = 0.0f; right[2] = sinf(yaw);

        mv = sy * MOVE; st = sx * MOVE;
        eye[0] += fwd[0] * mv + right[0] * st;
        eye[1] += fwd[1] * mv;
        eye[2] += fwd[2] * mv + right[2] * st;
        if (btn & GCN_PAD_BUTTON_A) eye[1] += MOVE;
        if (btn & GCN_PAD_BUTTON_B) eye[1] -= MOVE;

        /* Floor-clamp: keep the camera at least eyeHeight above the walkable
         * surface under it, so you can't sink through the floor. PCPORT_NO_FLOORCLAMP
         * keeps the old free-fly; PCPORT_EYE_HEIGHT tunes the standing height. */
        if (getenv("PCPORT_NO_FLOORCLAMP") == NULL) {
            const char* ehEnv = getenv("PCPORT_EYE_HEIGHT");
            f32 eyeHeight = (ehEnv != NULL) ? (f32)atof(ehEnv) : 25.0f;
            f32 floorY;
            if (PCPort_FieldColFloorAt(eye[0], eye[2], eye[1], 1.0e9f, &floorY) &&
                eye[1] < floorY + eyeHeight) {
                eye[1] = floorY + eyeHeight;
            }
        }

        interest[0] = eye[0] + fwd[0];
        interest[1] = eye[1] + fwd[1];
        interest[2] = eye[2] + fwd[2];
        BuildViewMatrixLookAt(eye, interest, up, g_engTitleCamera.viewMatrix);

        PCPort_EngineTitleRenderFrame();   /* same scene_data -> RenderJointTree path */
        if (getenv("PCPORT_COL_WIRE") != NULL) {
            DrawFieldCollisionWire();      /* debug overlay: collision mesh wireframe */
        }
        if (dumpPath != NULL && dumpPath[0] != '\0' &&
            (frameCap > 0 ? frame == frameCap - 1 : frame == 2)) {
            DumpBackbufferTo(dumpPath);
            printf("[field] dumped frame %d (eye=%.0f,%.0f,%.0f yaw=%.2f) to %s\n",
                   frame, eye[0], eye[1], eye[2], yaw, dumpPath);
        }
        GSgfxSwapBuffers(0);
        frame++;
        if (frameCap > 0 && frame >= frameCap) {
            break;
        }
    }
    printf("[field] explored %d frames (final eye=%.0f,%.0f,%.0f yaw=%.2f)\n",
           frame, eye[0], eye[1], eye[2], yaw);
    HoldWindowOpen(window);
    } /* end freeFlyPrevTime scope */
    return 1;
}

static int RunStoryFieldSmoke(GLFWwindow* window) {
    if (window == NULL) {
        fprintf(stderr,
                "[story-field-smoke] failed: no native window/GL context\n");
        return 0;
    }

    PCPort_StoryFieldSmokeBegin();
    g_pcEnterFieldWalk = 0;
    g_pcEnterBattleColosseum = 0;

    if (!RunMenuScene(window)) {
        fprintf(stderr, "[story-field-smoke] failed: menu handoff path failed\n");
        return 0;
    }
    if (!g_pcStoryFieldSmoke.sawMenuHandoff || !g_pcEnterFieldWalk) {
        fprintf(stderr,
                "[story-field-smoke] failed: New Game menu handoff was not observed\n");
        return 0;
    }
    if (!RunFieldScene(window)) {
        fprintf(stderr, "[story-field-smoke] failed: field scene did not run\n");
        return 0;
    }

    if (!g_pcStoryFieldSmoke.mapLoaded ||
        !g_pcStoryFieldSmoke.sawOutskirtLoad ||
        g_pcStoryFieldSmoke.targetFloor != PC_FLOOR_OUTSKIRT) {
        fprintf(stderr,
                "[story-field-smoke] failed: expected S1_out floor %d, got floor %d mapLoaded=%d sawOutskirt=%d path=%s\n",
                PC_FLOOR_OUTSKIRT,
                g_pcStoryFieldSmoke.targetFloor,
                g_pcStoryFieldSmoke.mapLoaded,
                g_pcStoryFieldSmoke.sawOutskirtLoad,
                g_pcStoryFieldSmoke.mapPath);
        return 0;
    }
    if (g_pcStoryFieldSmoke.colTris <= 0) {
        fprintf(stderr,
                "[story-field-smoke] failed: no field collision loaded for %s\n",
                g_pcStoryFieldSmoke.mapPath);
        return 0;
    }
    if (!g_pcStoryFieldSmoke.spawnSet || g_pcStoryFieldSmoke.frames < 30) {
        fprintf(stderr,
                "[story-field-smoke] failed: spawn/frame gate missed (spawn=%d frames=%d)\n",
                g_pcStoryFieldSmoke.spawnSet,
                g_pcStoryFieldSmoke.frames);
        return 0;
    }
    if (!g_pcStoryFieldSmoke.charLoaded ||
        !g_pcStoryFieldSmoke.charAnimReady) {
        fprintf(stderr,
                "[story-field-smoke] failed: Wes character/animation not ready (loaded=%d anim=%d)\n",
                g_pcStoryFieldSmoke.charLoaded,
                g_pcStoryFieldSmoke.charAnimReady);
        return 0;
    }
    if (!g_pcStoryFieldSmoke.moved) {
        fprintf(stderr,
                "[story-field-smoke] failed: player did not move from spawn %.1f,%.1f,%.1f to %.1f,%.1f,%.1f\n",
                g_pcStoryFieldSmoke.spawn[0],
                g_pcStoryFieldSmoke.spawn[1],
                g_pcStoryFieldSmoke.spawn[2],
                g_pcStoryFieldSmoke.finalPos[0],
                g_pcStoryFieldSmoke.finalPos[1],
                g_pcStoryFieldSmoke.finalPos[2]);
        return 0;
    }
    if (!g_pcStoryFieldSmoke.doorStoryReady ||
        !g_pcStoryFieldSmoke.doorStoryOpened ||
        !g_pcStoryFieldSmoke.doorStoryAdvanced ||
        !g_pcStoryFieldSmoke.doorStoryClosed ||
        g_pcStoryFieldSmoke.storyStep != 1u ||
        g_pcStoryFieldSmoke.cutsceneState != 2u ||
        g_pcStoryFieldSmoke.doorStoryNpcDrawn <= 0) {
        fprintf(stderr,
                "[story-field-smoke] failed: door story ready=%d open=%d advance=%d close=%d story=%u cutscene=%u npcDrawn=%d\n",
                g_pcStoryFieldSmoke.doorStoryReady,
                g_pcStoryFieldSmoke.doorStoryOpened,
                g_pcStoryFieldSmoke.doorStoryAdvanced,
                g_pcStoryFieldSmoke.doorStoryClosed,
                g_pcStoryFieldSmoke.storyStep,
                g_pcStoryFieldSmoke.cutsceneState,
                g_pcStoryFieldSmoke.doorStoryNpcDrawn);
        return 0;
    }
    if (!g_pcStoryFieldSmoke.sawShopLoad ||
        g_pcStoryFieldSmoke.doorStoryWarpFloor != PC_FLOOR_OUTSKIRT_SHOP) {
        fprintf(stderr,
                "[story-field-smoke] failed: door warp missed shop sawShop=%d warpFloor=%d expected=%d current=%d\n",
                g_pcStoryFieldSmoke.sawShopLoad,
                g_pcStoryFieldSmoke.doorStoryWarpFloor,
                PC_FLOOR_OUTSKIRT_SHOP,
                g_pcStoryFieldSmoke.currentFloor);
        return 0;
    }

    printf("[story-field-smoke] passed: menu->New Game->%s floor=%d colTris=%d "
           "spawn=(%.1f,%.1f,%.1f) final=(%.1f,%.1f,%.1f) frames=%d "
           "wes=%d anim=%d exits=%d doorNpc=%u story=%u cutscene=%u warp=%d\n",
           g_pcStoryFieldSmoke.mapPath,
           g_pcStoryFieldSmoke.targetFloor,
           g_pcStoryFieldSmoke.colTris,
           g_pcStoryFieldSmoke.spawn[0],
           g_pcStoryFieldSmoke.spawn[1],
           g_pcStoryFieldSmoke.spawn[2],
           g_pcStoryFieldSmoke.finalPos[0],
           g_pcStoryFieldSmoke.finalPos[1],
           g_pcStoryFieldSmoke.finalPos[2],
           g_pcStoryFieldSmoke.frames,
           g_pcStoryFieldSmoke.charLoaded,
           g_pcStoryFieldSmoke.charAnimReady,
           g_pcStoryFieldSmoke.exitCount,
           g_pcStoryFieldSmoke.doorStoryNpcIndex,
           g_pcStoryFieldSmoke.storyStep,
           g_pcStoryFieldSmoke.cutsceneState,
           g_pcStoryFieldSmoke.doorStoryWarpFloor);
    return 1;
}

static int RunFieldRoomWarpSmoke(GLFWwindow* window) {
    const PCPortWarpMapEntry* map1f;
    const PCPortWarpMapEntry* mapB1;
    PCPortFieldExit ex1f;
    PCPortFieldExit exB1;
    char path1f[512];
    char pathB1[512];
    int tris1f;
    int trisB1;
    int hit;

    if (window == NULL) {
        fprintf(stderr,
                "[field-room-warp-smoke] failed: no native window/GL context\n");
        return 0;
    }

    map1f = PCPort_WarpFindFloor(PC_FLOOR_GARAGE_1F);
    mapB1 = PCPort_WarpFindFloor(PC_FLOOR_GARAGE_B1);
    if (map1f == NULL || mapB1 == NULL) {
        fprintf(stderr,
                "[field-room-warp-smoke] failed: garage warp table entries missing\n");
        return 0;
    }
    PCPort_WarpResolvePath(map1f->fsysName, path1f, sizeof(path1f));
    PCPort_WarpResolvePath(mapB1->fsysName, pathB1, sizeof(pathB1));

    tris1f = PCPort_FieldColLoad(path1f);
    PCPort_FieldExitSet(map1f->exits, map1f->exitCount);
    if (tris1f <= 0 || PCPort_FieldExitCount() <= 0 ||
        !PCPort_FieldExitGet(0, &ex1f) ||
        ex1f.targetFloor != PC_FLOOR_GARAGE_B1) {
        fprintf(stderr,
                "[field-room-warp-smoke] failed: garage 1F exit invalid (tris=%d exits=%d target=%d)\n",
                tris1f,
                PCPort_FieldExitCount(),
                PCPort_FieldExitGet(0, &ex1f) ? ex1f.targetFloor : -1);
        return 0;
    }
    hit = PCPort_FieldExitCheck(ex1f.pos[0], ex1f.pos[1], ex1f.pos[2],
                                0.0f, -1.0f);
    if (hit != 0) {
        fprintf(stderr,
                "[field-room-warp-smoke] failed: garage 1F trigger check returned %d\n",
                hit);
        return 0;
    }

    PCPort_FieldColUnload();
    PCPort_FieldExitUnload();

    trisB1 = PCPort_FieldColLoad(pathB1);
    PCPort_FieldExitSet(mapB1->exits, mapB1->exitCount);
    if (trisB1 <= 0 || PCPort_FieldExitCount() <= 0 ||
        !PCPort_FieldExitGet(0, &exB1) ||
        exB1.targetFloor != PC_FLOOR_GARAGE_1F) {
        fprintf(stderr,
                "[field-room-warp-smoke] failed: garage B1 exit invalid (tris=%d exits=%d target=%d)\n",
                trisB1,
                PCPort_FieldExitCount(),
                PCPort_FieldExitGet(0, &exB1) ? exB1.targetFloor : -1);
        return 0;
    }
    hit = PCPort_FieldExitCheck(exB1.pos[0], exB1.pos[1], exB1.pos[2],
                                0.0f, 1.0f);
    if (hit != 0) {
        fprintf(stderr,
                "[field-room-warp-smoke] failed: garage B1 trigger check returned %d\n",
                hit);
        return 0;
    }

    PCPort_FieldColUnload();
    PCPort_FieldExitUnload();

    printf("[field-room-warp-smoke] passed: %s tris=%d exit->%d, "
           "%s tris=%d exit->%d\n",
           path1f,
           tris1f,
           ex1f.targetFloor,
           pathB1,
           trisB1,
           exB1.targetFloor);
    return 1;
}

static int RunFieldRoomReloadSmoke(GLFWwindow* window) {
    PCPortFieldExit ex1f;
    PCPortFieldExit exB1;
    f32 start[3];
    f32 spawnB1[3];
    int prevEnterFieldWalk;
    int next;

    if (window == NULL) {
        fprintf(stderr,
                "[field-room-reload-smoke] failed: no native window/GL context\n");
        return 0;
    }

    if (!PCPort_FieldWarpTo(PC_FLOOR_GARAGE_1F, NULL)) {
        fprintf(stderr,
                "[field-room-reload-smoke] failed: could not load garage 1F\n");
        return 0;
    }
    if (PCPort_FieldColTriCount() <= 0 || PCPort_FieldExitCount() <= 0 ||
        !PCPort_FieldExitGet(0, &ex1f) ||
        ex1f.targetFloor != PC_FLOOR_GARAGE_B1) {
        fprintf(stderr,
                "[field-room-reload-smoke] failed: garage 1F was not warp-ready (tris=%d exits=%d target=%d)\n",
                PCPort_FieldColTriCount(),
                PCPort_FieldExitCount(),
                PCPort_FieldExitGet(0, &ex1f) ? ex1f.targetFloor : -1);
        return 0;
    }

    start[0] = ex1f.pos[0];
    start[1] = ex1f.pos[1];
    start[2] = ex1f.pos[2] - ex1f.radius - 4.0f;
    PCPort_FieldColFloorAt(start[0], start[2], 1.0e5f, 1.0e9f, &start[1]);

    prevEnterFieldWalk = g_pcEnterFieldWalk;
    g_pcEnterFieldWalk = 1;
    g_pcFieldWarpSmokeStickY = -0.7f;
    g_pcFieldWarpSmokeActive = 1;
    next = RunFieldWalkLoop(window, NULL, 8, 0, start);
    g_pcFieldWarpSmokeActive = 0;
    g_pcEnterFieldWalk = prevEnterFieldWalk;

    if (next != PC_FLOOR_GARAGE_B1) {
        fprintf(stderr,
                "[field-room-reload-smoke] failed: walk loop returned floor %d, expected %d\n",
                next, PC_FLOOR_GARAGE_B1);
        return 0;
    }
    if (!PCPort_FieldWarpTo(next, spawnB1)) {
        fprintf(stderr,
                "[field-room-reload-smoke] failed: could not reload target floor %d\n",
                next);
        return 0;
    }
    if (PCPort_FieldColTriCount() <= 0 || PCPort_FieldExitCount() <= 0 ||
        !PCPort_FieldExitGet(0, &exB1) ||
        exB1.targetFloor != PC_FLOOR_GARAGE_1F) {
        fprintf(stderr,
                "[field-room-reload-smoke] failed: garage B1 was not ready after reload (tris=%d exits=%d target=%d)\n",
                PCPort_FieldColTriCount(),
                PCPort_FieldExitCount(),
                PCPort_FieldExitGet(0, &exB1) ? exB1.targetFloor : -1);
        return 0;
    }

    PCPort_EngineTitleRenderFrame();
    GSgfxSwapBuffers(0);

    printf("[field-room-reload-smoke] passed: player triggered %d->%d at "
           "start=(%.1f,%.1f,%.1f); reloaded B1 tris=%d exit->%d "
           "spawn=(%.1f,%.1f,%.1f)\n",
           PC_FLOOR_GARAGE_1F,
           next,
           start[0], start[1], start[2],
           PCPort_FieldColTriCount(),
           exB1.targetFloor,
           spawnB1[0], spawnB1[1], spawnB1[2]);
    return 1;
}

static int RunFieldWorldWarpSmoke(GLFWwindow* window) {
    PCPortFieldExit exOut;
    PCPortFieldExit exOutDoor;
    PCPortFieldExit exShop;
    f32 start[3];
    f32 spawnShop[3];
    int prevEnterFieldWalk;
    int outExitCount;
    int hit;
    int i;
    int next;

    if (window == NULL) {
        fprintf(stderr,
                "[field-world-warp-smoke] failed: no native window/GL context\n");
        return 0;
    }

    if (!PCPort_FieldWarpTo(PC_FLOOR_OUTSKIRT, NULL)) {
        fprintf(stderr,
                "[field-world-warp-smoke] failed: could not load S1_out\n");
        return 0;
    }
    outExitCount = PCPort_FieldExitCount();
    if (PCPort_FieldColTriCount() <= 0 || outExitCount <= 0 ||
        !PCPort_FieldExitGet(0, &exOut) ||
        exOut.targetFloor != PC_FLOOR_OUTSKIRT_SHOP) {
        fprintf(stderr,
                "[field-world-warp-smoke] failed: S1_out was not shop-warp-ready (tris=%d exits=%d target=%d)\n",
                PCPort_FieldColTriCount(),
                outExitCount,
                PCPort_FieldExitGet(0, &exOut) ? exOut.targetFloor : -1);
        return 0;
    }
    if (outExitCount < 4 ||
        !PCPort_FieldExitGet(outExitCount - 1, &exOutDoor)) {
        fprintf(stderr,
                "[field-world-warp-smoke] failed: S1_out doorway trigger set was incomplete (exits=%d)\n",
                outExitCount);
        return 0;
    }
    for (i = 0; i < outExitCount; ++i) {
        PCPortFieldExit sample;
        PCPortFieldExit hitEx;
        if (!PCPort_FieldExitGet(i, &sample) ||
            sample.targetFloor != PC_FLOOR_OUTSKIRT_SHOP) {
            fprintf(stderr,
                    "[field-world-warp-smoke] failed: S1_out sample %d was not a shop trigger (target=%d)\n",
                    i, PCPort_FieldExitGet(i, &sample) ? sample.targetFloor : -1);
            return 0;
        }
        hit = PCPort_FieldExitCheck(sample.pos[0], sample.pos[1],
                                    sample.pos[2], 0.0f, 0.0f);
        if (hit < 0 ||
            !PCPort_FieldExitGet(hit, &hitEx) ||
            hitEx.targetFloor != PC_FLOOR_OUTSKIRT_SHOP) {
            fprintf(stderr,
                    "[field-world-warp-smoke] failed: S1_out sample %d at (%.1f,%.1f,%.1f) hit %d instead of a shop trigger\n",
                    i, sample.pos[0], sample.pos[1], sample.pos[2], hit);
            return 0;
        }
    }

    start[0] = exOut.pos[0];
    start[1] = exOut.pos[1];
    start[2] = exOut.pos[2];
    PCPort_FieldColFloorAt(start[0], start[2], 1.0e5f, 1.0e9f, &start[1]);

    prevEnterFieldWalk = g_pcEnterFieldWalk;
    g_pcEnterFieldWalk = 1;
    g_pcFieldWarpSmokeStickY = 0.0f;
    g_pcFieldWarpSmokeActive = 1;
    next = RunFieldWalkLoop(window, NULL, 2, 0, start);
    g_pcFieldWarpSmokeActive = 0;
    g_pcEnterFieldWalk = prevEnterFieldWalk;

    if (next != PC_FLOOR_OUTSKIRT_SHOP) {
        fprintf(stderr,
                "[field-world-warp-smoke] failed: walk loop returned floor %d, expected %d\n",
                next, PC_FLOOR_OUTSKIRT_SHOP);
        return 0;
    }
    if (!PCPort_FieldWarpTo(next, spawnShop)) {
        fprintf(stderr,
                "[field-world-warp-smoke] failed: could not reload target floor %d\n",
                next);
        return 0;
    }
    if (PCPort_FieldColTriCount() <= 0 || PCPort_FieldExitCount() <= 0 ||
        !PCPort_FieldExitGet(0, &exShop) ||
        exShop.targetFloor != PC_FLOOR_OUTSKIRT) {
        fprintf(stderr,
                "[field-world-warp-smoke] failed: S1_shop_1F was not ready after reload (tris=%d exits=%d target=%d)\n",
                PCPort_FieldColTriCount(),
                PCPort_FieldExitCount(),
                PCPort_FieldExitGet(0, &exShop) ? exShop.targetFloor : -1);
        return 0;
    }

    PCPort_EngineTitleRenderFrame();
    GSgfxSwapBuffers(0);

    printf("[field-world-warp-smoke] passed: player triggered %d->%d at "
           "start=(%.1f,%.1f,%.1f); doorwaySamples=%d far=(%.1f,%.1f,%.1f) "
           "r=%.1f; reloaded S1_shop_1F tris=%d exit->%d "
           "spawn=(%.1f,%.1f,%.1f)\n",
           PC_FLOOR_OUTSKIRT,
           next,
           start[0], start[1], start[2],
           outExitCount,
           exOutDoor.pos[0], exOutDoor.pos[1], exOutDoor.pos[2],
           exOutDoor.radius,
           PCPort_FieldColTriCount(),
           exShop.targetFloor,
           spawnShop[0], spawnShop[1], spawnShop[2]);
    return 1;
}

typedef struct PCPortFieldLocomotionSmokeCase {
    const char* name;
    f32 startYaw;
    f32 sx;
    f32 sy;
    PCPortFieldMotionRole wantRole;
    int wantSlot;
    f32 wantYaw;
    int checkYaw;
} PCPortFieldLocomotionSmokeCase;

static int RunFieldLocomotionSmoke(GLFWwindow* window) {
    static const PCPortFieldLocomotionSmokeCase cases[] = {
        { "idle-straight",      3.14159265f,  0.0f,  0.0f, PCPORT_FIELD_MOTION_IDLE, -1, 3.14159265f, 0 },
        { "walk-straight",      3.14159265f,  0.0f,  0.5f, PCPORT_FIELD_MOTION_WALK, -1, 3.14159265f, 1 },
        { "turn-right",         3.14159265f,  0.5f,  0.0f, PCPORT_FIELD_MOTION_WALK, 3, -1.57079633f, 1 },
        { "turn-left",          3.14159265f, -0.5f,  0.0f, PCPORT_FIELD_MOTION_WALK, 4, 1.57079633f, 1 },
        { "hard-right-cross",  -1.57079633f, -0.5f,  0.0f, PCPORT_FIELD_MOTION_WALK, 2, 1.57079633f, 1 },
        { "run-straight",       3.14159265f,  0.0f,  1.0f, PCPORT_FIELD_MOTION_RUN, -1, 3.14159265f, 1 }
    };
    PCPortFieldMotionMap motionMap;
    f32 spawn[3] = { 0.0f, 0.0f, 0.0f };
    int i;
    int slot1Motion;
    int slot2Motion;
    int slot3Motion;
    int slot4Motion;

    if (window == NULL) {
        fprintf(stderr,
                "[field-locomotion-smoke] failed: no native window/GL context\n");
        return 0;
    }
    if (!PCPort_FieldWarpTo(PC_FLOOR_OUTSKIRT, spawn)) {
        fprintf(stderr,
                "[field-locomotion-smoke] failed: could not load S1_out\n");
        return 0;
    }
    if (!PCPort_LoadFieldCharacter()) {
        fprintf(stderr,
                "[field-locomotion-smoke] failed: could not load field character\n");
        return 0;
    }
    if (!PCPort_CharAnimReady()) {
        fprintf(stderr,
                "[field-locomotion-smoke] failed: field character animation bank is not ready\n");
        return 0;
    }

    motionMap = PCPort_LoadFieldMotionMap();
    slot1Motion = PCPort_FieldMotionForActionSlot(&motionMap, 1);
    slot2Motion = PCPort_FieldMotionForActionSlot(&motionMap, 2);
    slot3Motion = PCPort_FieldMotionForActionSlot(&motionMap, 3);
    slot4Motion = PCPort_FieldMotionForActionSlot(&motionMap, 4);
    if (slot1Motion < 0 || slot2Motion < 0 ||
        slot3Motion < 0 || slot4Motion < 0) {
        fprintf(stderr,
                "[field-locomotion-smoke] failed: invalid action slots 1=%d 2=%d 3=%d 4=%d\n",
                slot1Motion, slot2Motion, slot3Motion, slot4Motion);
        return 0;
    }
    if (getenv("PCPORT_FIELD_REQUIRE_GAME_MOTION") != NULL &&
        !PCPort_FieldMotionMapIsGameBacked(&motionMap)) {
        fprintf(stderr,
                "[field-locomotion-smoke] failed: source=%s is not game action-record backed\n",
                PCPort_FieldMotionMapSourceName(&motionMap));
        return 0;
    }
    if (!PCPort_FieldMotionMapHasActionSlots(&motionMap)) {
        fprintf(stderr,
                "[field-locomotion-smoke] failed: source=%s has no retail action slots\n",
                PCPort_FieldMotionMapSourceName(&motionMap));
        return 0;
    }
    if (motionMap.idle != 1 || motionMap.walk != 5 || motionMap.run != 8) {
        fprintf(stderr,
                "[field-locomotion-smoke] failed: expected ken_b1 role motions idle=1 walk=5 run=8, got %d/%d/%d\n",
                motionMap.idle, motionMap.walk, motionMap.run);
        return 0;
    }

    for (i = 0; i < (int)(sizeof(cases) / sizeof(cases[0])); ++i) {
        const PCPortFieldLocomotionSmokeCase* c = &cases[i];
        f32 mvx;
        f32 mvz;
        f32 moveMag;
        f32 yaw = c->startYaw;
        f32 desiredYaw = yaw;
        f32 turnAmount = 0.0f;
        PCPortFieldMotionChoice choice;
        PCPortFieldMotionRole gotRole;
        int actionSlot;
        int motion;

        PCPort_FieldComputeMoveVector(0.0f, c->sx, c->sy,
                                      &mvx, &mvz, &moveMag);
        gotRole = PCPort_FieldMotionRoleForInput(moveMag, 0u, 0u,
                                                 0, 0, 0.85f, 0.05f);
        if (gotRole != c->wantRole) {
            fprintf(stderr,
                    "[field-locomotion-smoke] failed: %s role=%s expected=%s mag=%.2f\n",
                    c->name,
                    PCPort_FieldMotionRoleName(gotRole),
                    PCPort_FieldMotionRoleName(c->wantRole),
                    moveMag);
            return 0;
        }
        if (c->checkYaw) {
            desiredYaw = PCPort_FieldYawForMove(mvx, mvz);
            turnAmount = PCPort_FieldTurnAmountForYawChange(c->startYaw,
                                                            desiredYaw);
            yaw = desiredYaw;
            if (PCPort_FieldAngleAbsDelta(yaw, c->wantYaw) > 0.03f) {
                fprintf(stderr,
                        "[field-locomotion-smoke] failed: %s yaw=%.3f expected=%.3f\n",
                        c->name, yaw, c->wantYaw);
                return 0;
            }
        }
        motion = PCPort_FieldMotionSelectLive(&motionMap, gotRole,
                                              turnAmount, &choice,
                                              &actionSlot);
        if (actionSlot != c->wantSlot) {
            fprintf(stderr,
                    "[field-locomotion-smoke] failed: %s actionSlot=%d expected=%d turn=%.2f zone=%s\n",
                    c->name, actionSlot, c->wantSlot, turnAmount,
                    choice.zoneName);
            return 0;
        }
        if (actionSlot < 0) {
            int roleMotion = PCPort_FieldMotionForRole(&motionMap, gotRole);
            if (motion != roleMotion) {
                fprintf(stderr,
                        "[field-locomotion-smoke] failed: %s role motion=%d expected=%d\n",
                        c->name, motion, roleMotion);
                return 0;
            }
        } else {
            int slotMotion = PCPort_FieldMotionForActionSlot(&motionMap,
                                                             actionSlot);
            if (motion != slotMotion) {
                fprintf(stderr,
                        "[field-locomotion-smoke] failed: %s actionSlot=%d motion=%d expected=%d\n",
                        c->name, actionSlot, motion, slotMotion);
                return 0;
            }
        }
        if (motion < 0 || !PCPort_CharAnimSetMotion(motion)) {
            fprintf(stderr,
                    "[field-locomotion-smoke] failed: %s actionSlot=%d motion %d could not be set\n",
                    c->name, actionSlot, motion);
            return 0;
        }

        PCPort_EngineTitleRenderFrame();
        RenderFieldCharacter(spawn[0], spawn[1], spawn[2],
                             yaw, 1.3f, 1.0f);
        GSgfxSwapBuffers(0);
        printf("[field-locomotion-smoke] %s role=%s turn=%.2f zone=%s slots=%d->%d blend=%.2f dominant=%d motion=%d yaw=%.3f mag=%.2f\n",
               c->name,
               PCPort_FieldMotionRoleName(gotRole),
               turnAmount,
               choice.zoneName,
               choice.fromSlot,
               choice.toSlot,
               choice.blend,
               actionSlot,
               motion,
               yaw,
               moveMag);
    }

    printf("[field-locomotion-smoke] passed: S1_out player motion plumbing source=%s key=0x%08X roles=%d/%d/%d actionSlots=1:%d 2:%d 3:%d 4:%d turnCases=%d spawn=(%.1f,%.1f,%.1f)\n",
           PCPort_FieldMotionMapSourceName(&motionMap),
           motionMap.key,
           motionMap.idle,
           motionMap.walk,
           motionMap.run,
           slot1Motion,
           slot2Motion,
           slot3Motion,
           slot4Motion,
           (int)(sizeof(cases) / sizeof(cases[0])),
           spawn[0], spawn[1], spawn[2]);
    return 1;
}

static int RunFieldMessageSmoke(GLFWwindow* window) {
    const char* pages[] = {
        "Welcome to Outskirt Stand. This field message opens over the live world scene and reveals text over time.",
        "Pressing A finishes the current page, pressing A again advances, and the final A closes the message box."
    };
    PCPortMessageBoxState msg;
    PCPortFieldMotionMap motionMap;
    f32 spawn[3] = { 0.0f, 0.0f, 0.0f };
    int idleMotion;
    int frame;
    int renderedFrames = 0;
    int sawPage0Partial = 0;
    int sawPage1Partial = 0;
    int sawFastForward = 0;
    int sawAdvance = 0;
    int sawClose = 0;
    int closeFrame = -1;

    if (window == NULL) {
        fprintf(stderr,
                "[field-message-smoke] failed: no native window/GL context\n");
        return 0;
    }
    if (!PCPort_FieldWarpTo(PC_FLOOR_OUTSKIRT, spawn)) {
        fprintf(stderr,
                "[field-message-smoke] failed: could not load S1_out\n");
        return 0;
    }
    if (!PCPort_LoadFieldCharacter() || !PCPort_CharAnimReady()) {
        fprintf(stderr,
                "[field-message-smoke] failed: could not load animated field character\n");
        return 0;
    }
    motionMap = PCPort_LoadFieldMotionMap();
    idleMotion = PCPort_FieldMotionForRole(&motionMap, PCPORT_FIELD_MOTION_IDLE);
    if (idleMotion < 0 || !PCPort_CharAnimSetMotion(idleMotion)) {
        fprintf(stderr,
                "[field-message-smoke] failed: idle motion %d could not be set\n",
                idleMotion);
        return 0;
    }

    PCPort_MessageBoxInit(&msg, pages,
                          (int)(sizeof(pages) / sizeof(pages[0])), 6);

    for (frame = 0; frame < 80; ++frame) {
        u16 pressed = 0u;
        int beforeFastForward = msg.fastForwardCount;
        int beforeAdvance = msg.advanceCount;
        int beforeClose = msg.closeCount;
        int pageLen = PCPort_MessageBoxPageLen(&msg);

        if (msg.active && msg.pageIndex == 0 &&
            msg.visibleChars > 0 && msg.visibleChars < pageLen) {
            sawPage0Partial = 1;
        }
        if (msg.active && msg.pageIndex == 1 &&
            msg.visibleChars > 0 && msg.visibleChars < pageLen) {
            sawPage1Partial = 1;
        }

        if (msg.active && msg.pageIndex == 0 && frame == 3) {
            pressed = GCN_PAD_BUTTON_A;     /* partial page -> full page */
        } else if (msg.active && msg.pageIndex == 0 &&
                   msg.visibleChars >= pageLen) {
            pressed = GCN_PAD_BUTTON_A;     /* full page -> next page */
        } else if (msg.active && msg.pageIndex == 1 &&
                   msg.visibleChars >= pageLen && frame > 5) {
            pressed = GCN_PAD_BUTTON_A;     /* final page -> close */
        }

        VIWaitForRetrace_PC();
        PCPort_MessageBoxTick(&msg, pressed);
        if (msg.fastForwardCount > beforeFastForward) {
            sawFastForward = 1;
        }
        if (msg.advanceCount > beforeAdvance) {
            sawAdvance = 1;
        }
        if (msg.closeCount > beforeClose) {
            sawClose = 1;
            closeFrame = frame;
        }

        PCPort_FieldAnimTick(1.0f);
        PCPort_FieldAnimHarvestTexUV(&g_engTitleArchive, g_engTitleRootJoint);
        PCPort_EngineTitleRenderFrame();
        RenderFieldCharacter(spawn[0], spawn[1], spawn[2],
                             3.14159265f, 1.3f, 1.0f);
        DrawFieldMessageBox(&msg, "WES");
        GSgfxSwapBuffers(0);
        renderedFrames++;

        if (!msg.active && sawClose) {
            break;
        }
    }

    if (msg.active || !sawPage0Partial || !sawPage1Partial ||
        !sawFastForward || !sawAdvance || !sawClose ||
        msg.fastForwardCount != 1 || msg.advanceCount != 1 ||
        msg.closeCount != 1) {
        fprintf(stderr,
                "[field-message-smoke] failed: active=%d p0partial=%d p1partial=%d fast=%d advance=%d close=%d counts=%d/%d/%d frame=%d\n",
                msg.active,
                sawPage0Partial,
                sawPage1Partial,
                sawFastForward,
                sawAdvance,
                sawClose,
                msg.fastForwardCount,
                msg.advanceCount,
                msg.closeCount,
                frame);
        return 0;
    }

    printf("[field-message-smoke] passed: pages=%d fastForward=%d advance=%d close=%d frames=%d closeFrame=%d idleMotion=%d spawn=(%.1f,%.1f,%.1f)\n",
           msg.pageCount,
           msg.fastForwardCount,
           msg.advanceCount,
           msg.closeCount,
           renderedFrames,
           closeFrame,
           idleMotion,
           spawn[0], spawn[1], spawn[2]);
    return 1;
}

static int RunFieldNpcTalkSmoke(GLFWwindow* window) {
    const char* pages[] = {
        "This host-seeded NPC uses the recovered people talk-state entry before the field message opens."
    };
    PCPortMessageBoxState msg;
    PCPortFieldMotionMap motionMap;
    PeopleEntry* talkNpc;
    PeopleEntry* restoreNpc;
    f32 spawn[3] = { 0.0f, 0.0f, 0.0f };
    int idleMotion;
    int frame;
    int renderedFrames = 0;
    int sawPartial = 0;
    int sawFastForward = 0;
    int sawClose = 0;
    u32 talkResult;
    u32 idleResult;
    u32 restoreResult;
    u32 missingResult;

    if (window == NULL) {
        fprintf(stderr,
                "[field-npc-talk-smoke] failed: no native window/GL context\n");
        return 0;
    }
    if (!PCPort_FieldWarpTo(PC_FLOOR_OUTSKIRT, spawn)) {
        fprintf(stderr,
                "[field-npc-talk-smoke] failed: could not load S1_out\n");
        return 0;
    }
    if (!PCPort_LoadFieldCharacter() || !PCPort_CharAnimReady()) {
        fprintf(stderr,
                "[field-npc-talk-smoke] failed: could not load animated field character\n");
        return 0;
    }
    motionMap = PCPort_LoadFieldMotionMap();
    idleMotion = PCPort_FieldMotionForRole(&motionMap, PCPORT_FIELD_MOTION_IDLE);
    if (idleMotion < 0 || !PCPort_CharAnimSetMotion(idleMotion)) {
        fprintf(stderr,
                "[field-npc-talk-smoke] failed: idle motion %d could not be set\n",
                idleMotion);
        return 0;
    }

    PCPort_PeopleHostClear();
    talkNpc = PCPort_PeopleHostAdd(PC_FLOOR_OUTSKIRT, 7u,
                                   PEOPLE_STATE_INTERACTING);
    restoreNpc = PCPort_PeopleHostAdd(PC_FLOOR_OUTSKIRT, 8u,
                                      PEOPLE_STATE_RESERVED1);
    if (talkNpc == NULL || restoreNpc == NULL) {
        fprintf(stderr,
                "[field-npc-talk-smoke] failed: could not seed host people entries\n");
        PCPort_PeopleHostClear();
        return 0;
    }

    talkNpc->subState = 3;
    talkNpc->animBlendFactor = 0.75f;
    restoreNpc->prevState = PEOPLE_STATE_CUTSCENE;
    restoreNpc->subState = 4;
    restoreNpc->animBlendFactor = 0.5f;

    talkResult = fn_801812E8(PC_FLOOR_OUTSKIRT, 7u, 1u);
    idleResult = fn_801812E8(PC_FLOOR_OUTSKIRT, 7u, 0u);
    restoreResult = fn_801812E8(PC_FLOOR_OUTSKIRT, 8u, 0u);
    missingResult = fn_801812E8(PC_FLOOR_OUTSKIRT, 99u, 1u);

    if (talkResult != 1u || idleResult != 1u || restoreResult != 1u ||
        missingResult != 0u ||
        talkNpc->state != PEOPLE_STATE_IDLE ||
        talkNpc->prevState != PEOPLE_STATE_INTERACTING ||
        talkNpc->subState != 0 ||
        talkNpc->animBlendFactor != 0.0f ||
        restoreNpc->state != PEOPLE_STATE_CUTSCENE ||
        restoreNpc->subState != 0 ||
        restoreNpc->animBlendFactor != 0.0f) {
        fprintf(stderr,
                "[field-npc-talk-smoke] failed: talk=%u idle=%u restore=%u missing=%u talkState=%u prev=%u sub=%u blend=%.2f restoreState=%u restoreSub=%u restoreBlend=%.2f\n",
                talkResult,
                idleResult,
                restoreResult,
                missingResult,
                talkNpc->state,
                talkNpc->prevState,
                talkNpc->subState,
                talkNpc->animBlendFactor,
                restoreNpc->state,
                restoreNpc->subState,
                restoreNpc->animBlendFactor);
        PCPort_PeopleHostClear();
        return 0;
    }

    PCPort_MessageBoxInit(&msg, pages,
                          (int)(sizeof(pages) / sizeof(pages[0])), 5);
    for (frame = 0; frame < 40; ++frame) {
        u16 pressed = 0u;
        int beforeFastForward = msg.fastForwardCount;
        int beforeClose = msg.closeCount;
        int pageLen = PCPort_MessageBoxPageLen(&msg);

        if (msg.active && msg.visibleChars > 0 && msg.visibleChars < pageLen) {
            sawPartial = 1;
        }
        if (msg.active && frame == 2) {
            pressed = GCN_PAD_BUTTON_A;
        } else if (msg.active && msg.visibleChars >= pageLen && frame > 2) {
            pressed = GCN_PAD_BUTTON_A;
        }

        VIWaitForRetrace_PC();
        PCPort_MessageBoxTick(&msg, pressed);
        if (msg.fastForwardCount > beforeFastForward) {
            sawFastForward = 1;
        }
        if (msg.closeCount > beforeClose) {
            sawClose = 1;
        }

        PCPort_FieldAnimTick(1.0f);
        PCPort_FieldAnimHarvestTexUV(&g_engTitleArchive, g_engTitleRootJoint);
        PCPort_EngineTitleRenderFrame();
        RenderFieldCharacter(spawn[0], spawn[1], spawn[2],
                             3.14159265f, 1.3f, 1.0f);
        RenderFieldCharacter(spawn[0] + 4.0f, spawn[1], spawn[2] - 3.0f,
                             0.0f, 1.15f, 1.0f);
        DrawFieldMessageBox(&msg, "NPC");
        GSgfxSwapBuffers(0);
        renderedFrames++;

        if (!msg.active && sawClose) {
            break;
        }
    }

    if (msg.active || !sawPartial || !sawFastForward || !sawClose ||
        msg.fastForwardCount != 1 || msg.closeCount != 1) {
        fprintf(stderr,
                "[field-npc-talk-smoke] failed: active=%d partial=%d fast=%d close=%d counts=%d/%d frame=%d\n",
                msg.active,
                sawPartial,
                sawFastForward,
                sawClose,
                msg.fastForwardCount,
                msg.closeCount,
                frame);
        PCPort_PeopleHostClear();
        return 0;
    }

    printf("[field-npc-talk-smoke] passed: fn_801812E8 talk=%u idle=%u restore=%u missing=%u talkState=%u prev=%u restoreState=%u frames=%d idleMotion=%d spawn=(%.1f,%.1f,%.1f)\n",
           talkResult,
           idleResult,
           restoreResult,
           missingResult,
           talkNpc->state,
           talkNpc->prevState,
           restoreNpc->state,
           renderedFrames,
           idleMotion,
           spawn[0], spawn[1], spawn[2]);
    PCPort_PeopleHostClear();
    return 1;
}

static int RunFieldNpcModelSmoke(GLFWwindow* window) {
    static const char* kArchive = "orig/GC6E01/disc/files/S1_out.fsys";
    PCPortFieldMotionMap motionMap;
    char deps[8][64];
    char depSummary[256];
    char* npcMember = NULL;
    PCPortFieldNpcScriptPlacement npcPlacement;
    f32 spawn[3] = { 0.0f, 0.0f, 0.0f };
    int rawDepCount = 0;
    int resolvedDepCount;
    int storedDepCount;
    int idleMotion;
    int frame;
    int renderedFrames = 0;
    unsigned int npcDrawn = 0;
    int i;

    if (window == NULL) {
        fprintf(stderr,
                "[field-npc-model-smoke] failed: no native window/GL context\n");
        return 0;
    }

    memset(deps, 0, sizeof(deps));
    depSummary[0] = '\0';
    resolvedDepCount = PCPort_FieldNpcReadDependencyList(
        kArchive, deps, (int)(sizeof(deps) / sizeof(deps[0])),
        &rawDepCount, depSummary, sizeof(depSummary));
    storedDepCount = resolvedDepCount;
    if (storedDepCount > (int)(sizeof(deps) / sizeof(deps[0]))) {
        storedDepCount = (int)(sizeof(deps) / sizeof(deps[0]));
    }
    if (rawDepCount < 3 || resolvedDepCount < 3) {
        fprintf(stderr,
                "[field-npc-model-smoke] failed: dependency list raw=%d resolved=%d summary=%s\n",
                rawDepCount,
                resolvedDepCount,
                depSummary);
        return 0;
    }

    for (i = 0; i < storedDepCount; ++i) {
        if (strstr(deps[i], "truck") == NULL &&
            strstr(deps[i], "bike") == NULL) {
            npcMember = deps[i];
            break;
        }
    }
    if (npcMember == NULL && storedDepCount > 0) {
        npcMember = deps[0];
    }
    if (npcMember == NULL || npcMember[0] == '\0') {
        fprintf(stderr,
                "[field-npc-model-smoke] failed: no renderable dependency member from %s\n",
                depSummary);
        return 0;
    }

    if (!PCPort_FieldNpcReadScriptPlacement(kArchive, &npcPlacement)) {
        fprintf(stderr,
                "[field-npc-model-smoke] failed: no real S1_out NPC placement marker 0x%08X\n",
                PCPORT_S1_OUT_NPC_ACTOR_TOKEN);
        return 0;
    }

    if (!PCPort_FieldWarpTo(PC_FLOOR_OUTSKIRT, spawn)) {
        fprintf(stderr,
                "[field-npc-model-smoke] failed: could not load S1_out\n");
        return 0;
    }
    if (!PCPort_LoadFieldCharacter() || !PCPort_CharAnimReady()) {
        fprintf(stderr,
                "[field-npc-model-smoke] failed: could not load animated field character\n");
        return 0;
    }
    motionMap = PCPort_LoadFieldMotionMap();
    idleMotion = PCPort_FieldMotionForRole(&motionMap, PCPORT_FIELD_MOTION_IDLE);
    if (idleMotion < 0 || !PCPort_CharAnimSetMotion(idleMotion)) {
        fprintf(stderr,
                "[field-npc-model-smoke] failed: idle motion %d could not be set\n",
                idleMotion);
        return 0;
    }

    if (!PCPort_LoadFieldNpcModel(kArchive, npcMember)) {
        PCPort_FieldNpcModelRelease();
        return 0;
    }

    for (frame = 0; frame < 6; ++frame) {
        VIWaitForRetrace_PC();
        PCPort_FieldAnimTick(1.0f);
        PCPort_FieldAnimHarvestTexUV(&g_engTitleArchive, g_engTitleRootJoint);
        PCPort_EngineTitleRenderFrame();
        RenderFieldCharacter(spawn[0], spawn[1], spawn[2],
                             3.14159265f, 1.3f, 1.0f);
        npcDrawn += RenderFieldStaticArchive(&g_engNpcArchive, g_engNpcRoot,
                                             npcPlacement.pos[0],
                                             npcPlacement.pos[1],
                                             npcPlacement.pos[2],
                                             npcPlacement.yaw, 1.15f);
        GSgfxSwapBuffers(0);
        ++renderedFrames;
    }

    if (!g_engNpcLoaded || g_engNpcRoot == 0u ||
        npcDrawn == 0u || renderedFrames != 6) {
        fprintf(stderr,
                "[field-npc-model-smoke] failed: loaded=%d root=0x%X npcDrawn=%u frames=%d\n",
                g_engNpcLoaded,
                g_engNpcRoot,
                npcDrawn,
                renderedFrames);
        PCPort_FieldNpcModelRelease();
        return 0;
    }

    printf("[field-npc-model-smoke] passed: deps raw=%d resolved=%d [%s], rendered %s rootJoint=0x%X npcDrawn=%u frames=%d spawn=(%.1f,%.1f,%.1f) npcMarker=0x%08X@0x%X action=0x%08X npc=(%.1f,%.1f,%.1f)\n",
           rawDepCount,
           resolvedDepCount,
           depSummary,
           g_engNpcMember,
           g_engNpcRoot,
           npcDrawn,
           renderedFrames,
           spawn[0], spawn[1], spawn[2],
           npcPlacement.actorToken,
           npcPlacement.tokenOffset,
           npcPlacement.actionWord,
           npcPlacement.pos[0],
           npcPlacement.pos[1],
           npcPlacement.pos[2]);
    PCPort_FieldNpcModelRelease();
    return 1;
}

static int RunFieldPeopleSnapshotSmoke(GLFWwindow* window) {
    static const char* kArchive = "orig/GC6E01/disc/files/S1_out.fsys";
    PCPortFieldMotionMap motionMap;
    PCPortFieldNpcScriptPlacement npcPlacement;
    PeopleEntry* seedNpc;
    PeopleEntry* importedNpc;
    char deps[8][64];
    char depSummary[256];
    char* npcMember = NULL;
    u8 snapshot[PEOPLE_SPAWN_DATA_SIZE];
    f32 spawn[3] = { 0.0f, 0.0f, 0.0f };
    f32 importedPos[3];
    f32 snapshotPos[3];
    u32 npcIndex;
    u32 snapshotBytes;
    u32 talkResult;
    int rawDepCount = 0;
    int resolvedDepCount;
    int storedDepCount;
    int idleMotion;
    int frame;
    int renderedFrames = 0;
    unsigned int npcDrawn = 0;
    int i;

    if (window == NULL) {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: no native window/GL context\n");
        return 0;
    }

    memset(deps, 0, sizeof(deps));
    depSummary[0] = '\0';
    resolvedDepCount = PCPort_FieldNpcReadDependencyList(
        kArchive, deps, (int)(sizeof(deps) / sizeof(deps[0])),
        &rawDepCount, depSummary, sizeof(depSummary));
    storedDepCount = resolvedDepCount;
    if (storedDepCount > (int)(sizeof(deps) / sizeof(deps[0]))) {
        storedDepCount = (int)(sizeof(deps) / sizeof(deps[0]));
    }
    if (rawDepCount < 3 || resolvedDepCount < 3) {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: dependency list raw=%d resolved=%d summary=%s\n",
                rawDepCount,
                resolvedDepCount,
                depSummary);
        return 0;
    }
    for (i = 0; i < storedDepCount; ++i) {
        if (strstr(deps[i], "truck") == NULL &&
            strstr(deps[i], "bike") == NULL) {
            npcMember = deps[i];
            break;
        }
    }
    if (npcMember == NULL && storedDepCount > 0) {
        npcMember = deps[0];
    }
    if (npcMember == NULL || npcMember[0] == '\0') {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: no renderable dependency member from %s\n",
                depSummary);
        return 0;
    }

    if (!PCPort_FieldNpcReadScriptPlacement(kArchive, &npcPlacement)) {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: no real S1_out NPC placement marker 0x%08X\n",
                PCPORT_S1_OUT_NPC_ACTOR_TOKEN);
        return 0;
    }

    npcIndex = (npcPlacement.actorToken >> 16) & 0xFFFFu;
    if (npcIndex == 0u) {
        npcIndex = 1u;
    }

    PCPort_PeopleHostClear();
    seedNpc = PCPort_PeopleHostAdd(PC_FLOOR_OUTSKIRT, npcIndex,
                                   PEOPLE_STATE_INTERACTING);
    if (seedNpc == NULL) {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: could not seed people snapshot entry\n");
        return 0;
    }

    seedNpc->visible = 1;
    seedNpc->animId = 1;
    seedNpc->flags = PEOPLE_FLAG_ACTIVE | PEOPLE_FLAG_TALKABLE |
                     PEOPLE_FLAG_HAS_MODEL;
    seedNpc->scriptRef = (void*)(unsigned long)npcPlacement.tokenOffset;
    seedNpc->field_34 = npcPlacement.actorToken;
    seedNpc->field_38 = npcPlacement.actionWord;
    seedNpc->talkRange = 5.0f;
    seedNpc->prevState = PEOPLE_STATE_INTERACTING;
    seedNpc->motionIndex = npcPlacement.actionWord;
    seedNpc->isTalkable = 1;
    seedNpc->moveType = PEOPLE_MOVE_NONE;
    PCPort_PeopleHostSetPosition(seedNpc, npcPlacement.pos);

    snapshotBytes = fn_8018F730();
    if (snapshotBytes != PEOPLE_SPAWN_DATA_SIZE) {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: snapshot size=%u expected=%u\n",
                snapshotBytes,
                (u32)PEOPLE_SPAWN_DATA_SIZE);
        PCPort_PeopleHostClear();
        return 0;
    }

    memset(snapshot, 0xA5, sizeof(snapshot));
    fn_8018F788(snapshot, sizeof(snapshot));
    snapshotPos[0] =
        PCPort_ReadHostF32(snapshot + PCPORT_PEOPLE_RECORD_MODEL_POS_OFF);
    snapshotPos[1] =
        PCPort_ReadHostF32(snapshot + PCPORT_PEOPLE_RECORD_MODEL_POS_OFF + 0x4u);
    snapshotPos[2] =
        PCPort_ReadHostF32(snapshot + PCPORT_PEOPLE_RECORD_MODEL_POS_OFF + 0x8u);
    if (PCPort_ReadHostU32(snapshot + PCPORT_PEOPLE_RECORD_GROUP_OFF) !=
            PC_FLOOR_OUTSKIRT ||
        PCPort_ReadHostU32(snapshot + PCPORT_PEOPLE_RECORD_INDEX_OFF) !=
            npcIndex ||
        PCPort_ReadHostU32(snapshot + PCPORT_PEOPLE_RECORD_SCRIPT_REF_OFF) !=
            npcPlacement.tokenOffset ||
        snapshot[PCPORT_PEOPLE_RECORD_VISIBLE_OFF] == 0u ||
        snapshot[PCPORT_PEOPLE_RECORD_STATE_OFF] != PEOPLE_STATE_INTERACTING ||
        snapshot[PCPORT_PEOPLE_RECORD_PREV_STATE_OFF] !=
            PEOPLE_STATE_INTERACTING ||
        snapshot[PCPORT_PEOPLE_RECORD_MOVE_TYPE_OFF] != PEOPLE_MOVE_NONE ||
        !PCPort_Vec3Near(snapshotPos, npcPlacement.pos)) {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: exported record group=%u index=%u script=0x%X visible=%u state=%u prev=%u move=%u pos=(%.1f,%.1f,%.1f)\n",
                PCPort_ReadHostU32(snapshot + PCPORT_PEOPLE_RECORD_GROUP_OFF),
                PCPort_ReadHostU32(snapshot + PCPORT_PEOPLE_RECORD_INDEX_OFF),
                PCPort_ReadHostU32(snapshot + PCPORT_PEOPLE_RECORD_SCRIPT_REF_OFF),
                snapshot[PCPORT_PEOPLE_RECORD_VISIBLE_OFF],
                snapshot[PCPORT_PEOPLE_RECORD_STATE_OFF],
                snapshot[PCPORT_PEOPLE_RECORD_PREV_STATE_OFF],
                snapshot[PCPORT_PEOPLE_RECORD_MOVE_TYPE_OFF],
                snapshotPos[0],
                snapshotPos[1],
                snapshotPos[2]);
        PCPort_PeopleHostClear();
        return 0;
    }

    fn_8018F87C(snapshot, snapshotBytes);
    importedNpc =
        PCPort_PeopleHostFindByGroupIndex(PC_FLOOR_OUTSKIRT, npcIndex);
    if (importedNpc == NULL || fn_8018F730() != PEOPLE_SPAWN_DATA_SIZE) {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: importer did not restore group=%u index=%u size=%u\n",
                (u32)PC_FLOOR_OUTSKIRT,
                npcIndex,
                fn_8018F730());
        PCPort_PeopleHostClear();
        return 0;
    }

    PCPort_PeopleHostGetPosition(importedNpc, importedPos);
    if (importedNpc->field_34 != npcPlacement.actorToken ||
        importedNpc->field_38 != npcPlacement.actionWord ||
        importedNpc->scriptRef != (void*)(unsigned long)npcPlacement.tokenOffset ||
        importedNpc->state != PEOPLE_STATE_INTERACTING ||
        !PCPort_Vec3Near(importedPos, npcPlacement.pos)) {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: imported marker=0x%08X action=0x%08X script=%p state=%u pos=(%.1f,%.1f,%.1f)\n",
                importedNpc->field_34,
                importedNpc->field_38,
                importedNpc->scriptRef,
                importedNpc->state,
                importedPos[0],
                importedPos[1],
                importedPos[2]);
        PCPort_PeopleHostClear();
        return 0;
    }

    talkResult = fn_801812E8(PC_FLOOR_OUTSKIRT, npcIndex, 1u);
    if (talkResult != 1u ||
        importedNpc->state != PEOPLE_STATE_IDLE ||
        importedNpc->prevState != PEOPLE_STATE_INTERACTING) {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: imported talk result=%u state=%u prev=%u\n",
                talkResult,
                importedNpc->state,
                importedNpc->prevState);
        PCPort_PeopleHostClear();
        return 0;
    }

    if (!PCPort_FieldWarpTo(PC_FLOOR_OUTSKIRT, spawn)) {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: could not load S1_out\n");
        PCPort_PeopleHostClear();
        return 0;
    }
    if (!PCPort_LoadFieldCharacter() || !PCPort_CharAnimReady()) {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: could not load animated field character\n");
        PCPort_PeopleHostClear();
        return 0;
    }
    motionMap = PCPort_LoadFieldMotionMap();
    idleMotion = PCPort_FieldMotionForRole(&motionMap, PCPORT_FIELD_MOTION_IDLE);
    if (idleMotion < 0 || !PCPort_CharAnimSetMotion(idleMotion)) {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: idle motion %d could not be set\n",
                idleMotion);
        PCPort_PeopleHostClear();
        return 0;
    }
    if (!PCPort_LoadFieldNpcModel(kArchive, npcMember)) {
        PCPort_FieldNpcModelRelease();
        PCPort_PeopleHostClear();
        return 0;
    }

    for (frame = 0; frame < 4; ++frame) {
        VIWaitForRetrace_PC();
        PCPort_FieldAnimTick(1.0f);
        PCPort_FieldAnimHarvestTexUV(&g_engTitleArchive, g_engTitleRootJoint);
        PCPort_EngineTitleRenderFrame();
        RenderFieldCharacter(spawn[0], spawn[1], spawn[2],
                             3.14159265f, 1.3f, 1.0f);
        npcDrawn += RenderFieldStaticArchive(&g_engNpcArchive, g_engNpcRoot,
                                             importedPos[0],
                                             importedPos[1],
                                             importedPos[2],
                                             npcPlacement.yaw, 1.15f);
        GSgfxSwapBuffers(0);
        ++renderedFrames;
    }

    if (npcDrawn == 0u || renderedFrames != 4) {
        fprintf(stderr,
                "[field-people-snapshot-smoke] failed: npcDrawn=%u frames=%d\n",
                npcDrawn,
                renderedFrames);
        PCPort_FieldNpcModelRelease();
        PCPort_PeopleHostClear();
        return 0;
    }

    printf("[field-people-snapshot-smoke] passed: fn_8018F730=%u fn_8018F788/87C roundtrip group=%u index=%u marker=0x%08X@0x%X action=0x%08X npc=(%.1f,%.1f,%.1f) talk=%u npcDrawn=%u frames=%d\n",
           snapshotBytes,
           (u32)PC_FLOOR_OUTSKIRT,
           npcIndex,
           npcPlacement.actorToken,
           npcPlacement.tokenOffset,
           npcPlacement.actionWord,
           importedPos[0],
           importedPos[1],
           importedPos[2],
           talkResult,
           npcDrawn,
           renderedFrames);

    PCPort_FieldNpcModelRelease();
    PCPort_PeopleHostClear();
    return 1;
}

static int RunFieldNpcOpenSmoke(GLFWwindow* window) {
    static const char* kArchive = "orig/GC6E01/disc/files/S1_out.fsys";
    const char* pages[] = {
        "Welcome to Outskirt Stand. The story path can now open an NPC dialogue.",
        "The interaction closes with a progression marker for the next script gate."
    };
    PCPortFieldMotionMap motionMap;
    PCPortFieldNpcScriptPlacement npcPlacement;
    PCPortMessageBoxState msg;
    PCPortScriptMsgContext* msgCtx = NULL;
    PeopleEntry* npc;
    char deps[8][64];
    char depSummary[256];
    char* npcMember = NULL;
    f32 spawn[3] = { 0.0f, 0.0f, 0.0f };
    f32 npcPos[3];
    u32 npcIndex;
    u32 talkResult;
    u32 storyStep = 0u;
    u32 cutsceneState = 0u;
    int rawDepCount = 0;
    int resolvedDepCount;
    int storedDepCount;
    int idleMotion;
    int frame;
    int renderedFrames = 0;
    int sawOpen = 0;
    int sawAdvance = 0;
    int sawClose = 0;
    int msgCtxLinked = 0;
    int scriptCbLinked = 0;
    unsigned int npcDrawn = 0;
    int i;

    if (window == NULL) {
        fprintf(stderr,
                "[field-npc-open-smoke] failed: no native window/GL context\n");
        return 0;
    }

    memset(deps, 0, sizeof(deps));
    depSummary[0] = '\0';
    resolvedDepCount = PCPort_FieldNpcReadDependencyList(
        kArchive, deps, (int)(sizeof(deps) / sizeof(deps[0])),
        &rawDepCount, depSummary, sizeof(depSummary));
    storedDepCount = resolvedDepCount;
    if (storedDepCount > (int)(sizeof(deps) / sizeof(deps[0]))) {
        storedDepCount = (int)(sizeof(deps) / sizeof(deps[0]));
    }
    if (rawDepCount < 3 || resolvedDepCount < 3) {
        fprintf(stderr,
                "[field-npc-open-smoke] failed: dependency list raw=%d resolved=%d summary=%s\n",
                rawDepCount,
                resolvedDepCount,
                depSummary);
        return 0;
    }

    for (i = 0; i < storedDepCount; ++i) {
        if (strstr(deps[i], "truck") == NULL &&
            strstr(deps[i], "bike") == NULL) {
            npcMember = deps[i];
            break;
        }
    }
    if (npcMember == NULL && storedDepCount > 0) {
        npcMember = deps[0];
    }
    if (npcMember == NULL || npcMember[0] == '\0') {
        fprintf(stderr,
                "[field-npc-open-smoke] failed: no renderable dependency member from %s\n",
                depSummary);
        return 0;
    }

    if (!PCPort_FieldNpcReadScriptPlacement(kArchive, &npcPlacement)) {
        fprintf(stderr,
                "[field-npc-open-smoke] failed: no real S1_out NPC placement marker 0x%08X\n",
                PCPORT_S1_OUT_NPC_ACTOR_TOKEN);
        return 0;
    }

    npcIndex = (npcPlacement.actorToken >> 16) & 0xFFFFu;
    if (npcIndex == 0u) {
        npcIndex = 1u;
    }

    PCPort_PeopleHostClear();
    PCPort_PeopleOpenSetPlacement(PC_FLOOR_OUTSKIRT, npcIndex, &npcPlacement);
    fn_8018E050(PC_FLOOR_OUTSKIRT, npcIndex, npcPlacement.actionWord);
    PCPort_PeopleOpenClearPlacement();

    npc = PCPort_PeopleHostFindExact(PC_FLOOR_OUTSKIRT, npcIndex);
    if (npc == NULL || g_pcPeopleOpenSetupCount != 1u) {
        fprintf(stderr,
                "[field-npc-open-smoke] failed: fn_8018E050 did not create NPC index=%u setupCount=%u\n",
                npcIndex,
                g_pcPeopleOpenSetupCount);
        PCPort_PeopleHostClear();
        return 0;
    }
    PCPort_PeopleHostGetPosition(npc, npcPos);
    if (npc->field_34 != npcPlacement.actorToken ||
        npc->field_38 != npcPlacement.actionWord ||
        npc->scriptRef != (void*)(unsigned long)npcPlacement.tokenOffset ||
        npc->state != PEOPLE_STATE_INTERACTING ||
        !PCPort_Vec3Near(npcPos, npcPlacement.pos)) {
        fprintf(stderr,
                "[field-npc-open-smoke] failed: marker=0x%08X action=0x%08X script=%p state=%u pos=(%.1f,%.1f,%.1f)\n",
                npc->field_34,
                npc->field_38,
                npc->scriptRef,
                npc->state,
                npcPos[0],
                npcPos[1],
                npcPos[2]);
        PCPort_PeopleHostClear();
        return 0;
    }

    talkResult = fn_801812E8(PC_FLOOR_OUTSKIRT, npcIndex, 1u);
    if (talkResult != 1u ||
        npc->state != PEOPLE_STATE_IDLE ||
        npc->prevState != PEOPLE_STATE_INTERACTING) {
        fprintf(stderr,
                "[field-npc-open-smoke] failed: talk=%u state=%u prev=%u\n",
                talkResult,
                npc->state,
                npc->prevState);
        PCPort_PeopleHostClear();
        return 0;
    }

    if (!PCPort_FieldWarpTo(PC_FLOOR_OUTSKIRT, spawn)) {
        fprintf(stderr,
                "[field-npc-open-smoke] failed: could not load S1_out\n");
        PCPort_PeopleHostClear();
        return 0;
    }
    if (!PCPort_LoadFieldCharacter() || !PCPort_CharAnimReady()) {
        fprintf(stderr,
                "[field-npc-open-smoke] failed: could not load animated field character\n");
        PCPort_PeopleHostClear();
        return 0;
    }
    motionMap = PCPort_LoadFieldMotionMap();
    idleMotion = PCPort_FieldMotionForRole(&motionMap, PCPORT_FIELD_MOTION_IDLE);
    if (idleMotion < 0 || !PCPort_CharAnimSetMotion(idleMotion)) {
        fprintf(stderr,
                "[field-npc-open-smoke] failed: idle motion %d could not be set\n",
                idleMotion);
        PCPort_PeopleHostClear();
        return 0;
    }
    if (!PCPort_LoadFieldNpcModel(kArchive, npcMember)) {
        PCPort_FieldNpcModelRelease();
        PCPort_PeopleHostClear();
        return 0;
    }

    PCPort_MessageBoxInit(&msg, pages,
                          (int)(sizeof(pages) / sizeof(pages[0])), 5);
    PCPort_MessageBoxSeedScriptContext(&msg, npcIndex, pages[0]);
    msgCtx = PCPort_MessageBoxScriptContext();
    msgCtxLinked =
        msgCtx == (PCPortScriptMsgContext*)(lbl_803A9768 +
                                            PCPORT_MSGBOX_SLOT_BASE) &&
        msgCtx->magic == PCPORT_MSGBOX_CONTEXT_MAGIC &&
        msgCtx->speakerId == npcIndex &&
        msgCtx->pageCount == (u32)msg.pageCount &&
        msgCtx->active == 1u;
    if (!msgCtxLinked) {
        fprintf(stderr,
                "[field-npc-open-smoke] failed: fn_80056C54/fn_80057270 msg context not linked ctx=%p magic=0x%08X speaker=%u pages=%u active=%u\n",
                (void*)msgCtx,
                msgCtx != NULL ? msgCtx->magic : 0u,
                msgCtx != NULL ? msgCtx->speakerId : 0u,
                msgCtx != NULL ? msgCtx->pageCount : 0u,
                msgCtx != NULL ? msgCtx->active : 0u);
        PCPort_FieldNpcModelRelease();
        PCPort_PeopleHostClear();
        return 0;
    }
    memset(lbl_803A95E8, 0, 0x138u);
    memset(lbl_803A9720, 0, 0x48u);
    fn_80053778();
    scriptCbLinked =
        *(void**)lbl_803A95E8 == (void*)msgCtx &&
        *(void**)lbl_803A9720 == (void*)msgCtx &&
        *(u32*)(lbl_803A95E8 + 8) == 1u &&
        *(u32*)(lbl_803A9720 + 8) == 1u;
    if (!scriptCbLinked) {
        fprintf(stderr,
                "[field-npc-open-smoke] failed: fn_80053778 script callback did not resolve msg context cbA=%p cbB=%p countA=%u countB=%u msg=%p\n",
                *(void**)lbl_803A95E8,
                *(void**)lbl_803A9720,
                *(u32*)(lbl_803A95E8 + 8),
                *(u32*)(lbl_803A9720 + 8),
                (void*)msgCtx);
        PCPort_FieldNpcModelRelease();
        PCPort_PeopleHostClear();
        return 0;
    }

    for (frame = 0; frame < 48; ++frame) {
        u16 pressed = 0u;
        int beforeAdvance = msg.advanceCount;
        int beforeClose = msg.closeCount;
        int pageLen = PCPort_MessageBoxPageLen(&msg);

        if (msg.active) {
            sawOpen = 1;
        }
        if (msg.active && frame == 2) {
            pressed = GCN_PAD_BUTTON_A;
        } else if (msg.active && msg.visibleChars >= pageLen && frame > 2) {
            pressed = GCN_PAD_BUTTON_A;
        }

        VIWaitForRetrace_PC();
        PCPort_MessageBoxTick(&msg, pressed);
        if (msg.advanceCount > beforeAdvance) {
            sawAdvance = 1;
        }
        if (msg.closeCount > beforeClose) {
            sawClose = 1;
            storyStep = 1u;
            cutsceneState = 2u;
        }
        PCPort_MessageBoxSyncScriptContext(&msg, storyStep, cutsceneState);

        PCPort_FieldAnimTick(1.0f);
        PCPort_FieldAnimHarvestTexUV(&g_engTitleArchive, g_engTitleRootJoint);
        PCPort_EngineTitleRenderFrame();
        RenderFieldCharacter(spawn[0], spawn[1], spawn[2],
                             3.14159265f, 1.3f, 1.0f);
        npcDrawn += RenderFieldStaticArchive(&g_engNpcArchive, g_engNpcRoot,
                                             npcPos[0],
                                             npcPos[1],
                                             npcPos[2],
                                             npcPlacement.yaw, 1.15f);
        DrawFieldMessageBox(&msg, "NPC");
        GSgfxSwapBuffers(0);
        ++renderedFrames;

        if (!msg.active && sawClose) {
            break;
        }
    }

    if (msg.active || !sawOpen || !sawAdvance || !sawClose ||
        !msgCtxLinked || !scriptCbLinked || msgCtx == NULL ||
        msgCtx->active != 0u ||
        msgCtx->storyStep != 1u || msgCtx->cutsceneState != 2u ||
        msgCtx->pageIndex != 1u ||
        storyStep != 1u || cutsceneState != 2u ||
        npcDrawn == 0u || renderedFrames == 0) {
        fprintf(stderr,
                "[field-npc-open-smoke] failed: active=%d open=%d advance=%d close=%d msgCtx=%d scriptCb=%d ctxActive=%u ctxStory=%u ctxCutscene=%u ctxPage=%u story=%u cutscene=%u npcDrawn=%u frames=%d\n",
                msg.active,
                sawOpen,
                sawAdvance,
                sawClose,
                msgCtxLinked,
                scriptCbLinked,
                msgCtx != NULL ? msgCtx->active : 0u,
                msgCtx != NULL ? msgCtx->storyStep : 0u,
                msgCtx != NULL ? msgCtx->cutsceneState : 0u,
                msgCtx != NULL ? msgCtx->pageIndex : 0u,
                storyStep,
                cutsceneState,
                npcDrawn,
                renderedFrames);
        PCPort_FieldNpcModelRelease();
        PCPort_PeopleHostClear();
        return 0;
    }

    printf("[field-npc-open-smoke] passed: fn_8018E050/E1C4 setup=%u fn_80056C54=1 fn_80057270=1 fn_80053778=1 group=%u index=%u marker=0x%08X@0x%X action=0x%08X npc=(%.1f,%.1f,%.1f) talk=%u dialogueOpen=%d advance=%d close=%d storyStep=%u cutscene=%u npcDrawn=%u frames=%d\n",
           g_pcPeopleOpenSetupCount,
           (u32)PC_FLOOR_OUTSKIRT,
           npcIndex,
           npcPlacement.actorToken,
           npcPlacement.tokenOffset,
           npcPlacement.actionWord,
           npcPos[0],
           npcPos[1],
           npcPos[2],
           talkResult,
           sawOpen,
           sawAdvance,
           sawClose,
           storyStep,
           cutsceneState,
           npcDrawn,
           renderedFrames);

    PCPort_FieldNpcModelRelease();
    PCPort_PeopleHostClear();
    return 1;
}

static int RunFieldStartMenuSmoke(GLFWwindow* window) {
    PCPortFieldStartMenuState menu;
    PCPortFieldMotionMap motionMap;
    f32 spawn[3] = { 0.0f, 0.0f, 0.0f };
    int idleMotion;
    int frame;
    int renderedFrames = 0;
    int sawOpen = 0;
    int sawCursorTwo = 0;
    int sawClose = 0;
    int closedCursor = -1;

    if (window == NULL) {
        fprintf(stderr,
                "[field-start-menu-smoke] failed: no native window/GL context\n");
        return 0;
    }
    if (!PCPort_FieldWarpTo(PC_FLOOR_OUTSKIRT, spawn)) {
        fprintf(stderr,
                "[field-start-menu-smoke] failed: could not load S1_out\n");
        return 0;
    }
    if (!PCPort_LoadFieldCharacter() || !PCPort_CharAnimReady()) {
        fprintf(stderr,
                "[field-start-menu-smoke] failed: could not load animated field character\n");
        return 0;
    }
    motionMap = PCPort_LoadFieldMotionMap();
    idleMotion = PCPort_FieldMotionForRole(&motionMap, PCPORT_FIELD_MOTION_IDLE);
    if (idleMotion < 0 || !PCPort_CharAnimSetMotion(idleMotion)) {
        fprintf(stderr,
                "[field-start-menu-smoke] failed: idle motion %d could not be set\n",
                idleMotion);
        return 0;
    }

    PCPort_FieldStartMenuInit(&menu);
    for (frame = 0; frame < 12; ++frame) {
        u16 pressed = 0u;
        int wasActive = menu.active;

        if (frame == 0) {
            pressed = GCN_PAD_BUTTON_START;
        } else if (frame == 1 || frame == 2) {
            pressed = GCN_PAD_BUTTON_DOWN;
        } else if (frame == 5) {
            pressed = GCN_PAD_BUTTON_START;
        }

        VIWaitForRetrace_PC();
        PCPort_FieldStartMenuTick(&menu, pressed);
        if (!wasActive && menu.active) {
            sawOpen = 1;
        }
        if (menu.active && menu.cursor == 2) {
            sawCursorTwo = 1;
        }
        if (wasActive && !menu.active) {
            sawClose = 1;
            closedCursor = menu.cursor;
        }

        PCPort_FieldAnimTick(1.0f);
        PCPort_FieldAnimHarvestTexUV(&g_engTitleArchive, g_engTitleRootJoint);
        PCPort_EngineTitleRenderFrame();
        RenderFieldCharacter(spawn[0], spawn[1], spawn[2],
                             3.14159265f, 1.3f, 1.0f);
        DrawFieldStartMenuOverlay(&menu);
        GSgfxSwapBuffers(0);
        renderedFrames++;

        if (sawClose) {
            break;
        }
    }

    if (menu.active || !sawOpen || !sawCursorTwo || !sawClose ||
        menu.openCount != 1 || menu.moveCount != 2 ||
        menu.closeCount != 1 || closedCursor != 2) {
        fprintf(stderr,
                "[field-start-menu-smoke] failed: active=%d open=%d cursor2=%d close=%d counts=%d/%d/%d closedCursor=%d frame=%d\n",
                menu.active,
                sawOpen,
                sawCursorTwo,
                sawClose,
                menu.openCount,
                menu.moveCount,
                menu.closeCount,
                closedCursor,
                frame);
        return 0;
    }

    printf("[field-start-menu-smoke] passed: open=%d move=%d close=%d closedCursor=%d frames=%d idleMotion=%d spawn=(%.1f,%.1f,%.1f)\n",
           menu.openCount,
           menu.moveCount,
           menu.closeCount,
           closedCursor,
           renderedFrames,
           idleMotion,
           spawn[0], spawn[1], spawn[2]);
    return 1;
}

static int RunWorldMapHandoffSmoke(GLFWwindow* window) {
    const char* archive = getenv("PCPORT_WORLDMAP_ARCHIVE");

    if (window == NULL) {
        fprintf(stderr,
                "[worldmap-handoff-smoke] failed: no native window/GL context\n");
        return 0;
    }
    if (archive == NULL || archive[0] == '\0') {
        archive = PCPORT_WORLDMAP_ARCHIVE;
    }

    printf("[worldmap-handoff-smoke] field START/worldmap handoff -> %s\n",
           archive);

    PCPort_FieldColUnload();
    PCPort_FieldExitUnload();

    if (!PCPort_EngineFieldSetup(archive)) {
        fprintf(stderr,
                "[worldmap-handoff-smoke] failed: could not load %s\n",
                archive);
        return 0;
    }
    if (!PCPort_EngineTitleReady()) {
        fprintf(stderr,
                "[worldmap-handoff-smoke] failed: worldmap render scene not ready\n");
        return 0;
    }

    PCPort_EngineTitleRenderFrame();
    GSgfxSwapBuffers(0);

    printf("[worldmap-handoff-smoke] passed: loaded/rendered %s "
           "rootJoint=0x%X extraModels=%d collisionTris=%d\n",
           archive,
           g_engTitleRootJoint,
           g_engExtraRootJointCount,
           PCPort_FieldColTriCount());
    return 1;
}

typedef enum PCPortWorldMapMenuState {
    PCPORT_WORLDMAP_SELECT = 0,
    PCPORT_WORLDMAP_CONFIRM = 1,
    PCPORT_WORLDMAP_ACCEPTED = 2,
    PCPORT_WORLDMAP_UNAVAILABLE = 3
} PCPortWorldMapMenuState;

typedef struct PCPortWorldMapLocation {
    const char* name;
    const char* desc;
    f32 x;
    f32 y;
    int floorId;
} PCPortWorldMapLocation;

static const PCPortWorldMapLocation kWorldMapLocations[] = {
    {
        "OUTSKIRT STAND",
        "Return to the desert train stop where Story Mode starts.",
        104.0f, 334.0f,
        PC_FLOOR_OUTSKIRT
    },
    {
        "PHENAC CITY",
        "Route pending: M1_out renders, but no WZX collision mesh is linked yet.",
        405.0f, 186.0f,
        -1
    },
    {
        "PYRITE TOWN",
        "Travel to Pyrite Town exterior through the verified M2_out field target.",
        338.0f, 270.0f,
        PC_FLOOR_PYRITE
    },
    {
        "AGATE VILLAGE",
        "Travel to the verified M3_out exterior target; story gating follows.",
        184.0f, 142.0f,
        PC_FLOOR_AGATE
    }
};
#define PCPORT_WORLDMAP_LOCATION_COUNT \
    ((int)(sizeof(kWorldMapLocations) / sizeof(kWorldMapLocations[0])))

static void DrawWorldMapMenuOverlay(int cursor,
                                    PCPortWorldMapMenuState state,
                                    int confirmCursor) {
    int i;
    char line[128];
    const PCPortWorldMapLocation* loc = &kWorldMapLocations[cursor];

    BeginMenuOverlay();
    EnsureFontAtlas();

    DrawSolidScreenRect(18.0f, 18.0f, 230.0f, 156.0f, 13, 29, 44, 208);
    DrawSolidScreenRect(24.0f, 24.0f, 218.0f, 144.0f, 31, 58, 76, 232);
    DrawTextScreen(38.0f, 36.0f, 8.0f, 13.0f, 232, 242, 248, 255,
                   "WORLD MAP");
    DrawTextWrapped(38.0f, 64.0f, 7.0f, 11.0f, 28, 4,
                    190, 216, 228, 255,
                    "Choose a destination, press A, then confirm travel.");

    for (i = 0; i < PCPORT_WORLDMAP_LOCATION_COUNT; ++i) {
        const PCPortWorldMapLocation* m = &kWorldMapLocations[i];
        int selected = (i == cursor);
        u8 r = selected ? 248 : (m->floorId >= 0 ? 92 : 124);
        u8 g = selected ? 214 : (m->floorId >= 0 ? 198 : 132);
        u8 b = selected ? 92 : (m->floorId >= 0 ? 220 : 140);
        DrawSolidScreenRect(m->x - 8.0f, m->y - 8.0f, 16.0f, 16.0f,
                            r, g, b, 245);
        DrawSolidScreenRect(m->x - 4.0f, m->y - 4.0f, 8.0f, 8.0f,
                            16, 28, 38, 245);
        if (selected) {
            DrawSolidScreenRect(m->x - 14.0f, m->y - 14.0f, 28.0f, 4.0f,
                                248, 214, 92, 245);
            DrawSolidScreenRect(m->x - 14.0f, m->y + 10.0f, 28.0f, 4.0f,
                                248, 214, 92, 245);
            DrawSolidScreenRect(m->x - 14.0f, m->y - 14.0f, 4.0f, 28.0f,
                                248, 214, 92, 245);
            DrawSolidScreenRect(m->x + 10.0f, m->y - 14.0f, 4.0f, 28.0f,
                                248, 214, 92, 245);
        }
    }

    DrawSolidScreenRect(44.0f, 372.0f, 552.0f, 86.0f, 96, 124, 142, 230);
    DrawSolidScreenRect(50.0f, 378.0f, 540.0f, 74.0f, 20, 39, 54, 244);
    snprintf(line, sizeof(line), "%s", loc->name);
    DrawTextScreen(70.0f, 390.0f, 9.0f, 14.0f, 248, 246, 224, 255, line);
    DrawTextWrapped(70.0f, 416.0f, 7.0f, 11.0f, 62, 2,
                    206, 222, 232, 255, loc->desc);

    if (state == PCPORT_WORLDMAP_CONFIRM) {
        char prompt[160];
        snprintf(prompt, sizeof(prompt), "Travel to %s?", loc->name);
        DrawDialogBox(prompt, 1, confirmCursor);
    } else if (state == PCPORT_WORLDMAP_ACCEPTED) {
        char prompt[160];
        snprintf(prompt, sizeof(prompt), "Travel confirmed for %s.", loc->name);
        DrawDialogBox(prompt, 0, 0);
    } else if (state == PCPORT_WORLDMAP_UNAVAILABLE) {
        char prompt[160];
        snprintf(prompt, sizeof(prompt), "%s is not travel-ready yet.", loc->name);
        DrawDialogBox(prompt, 0, 0);
    }
}

static void DrawWorldMapTravelResultOverlay(const PCPortWorldMapLocation* loc,
                                            int floorId,
                                            int colTris) {
    char title[128];
    char detail[160];

    BeginMenuOverlay();
    EnsureFontAtlas();

    DrawSolidScreenRect(44.0f, 350.0f, 552.0f, 96.0f, 96, 124, 142, 230);
    DrawSolidScreenRect(50.0f, 356.0f, 540.0f, 84.0f, 20, 39, 54, 244);
    snprintf(title, sizeof(title), "ARRIVED: %s", loc->name);
    snprintf(detail, sizeof(detail),
             "Worldmap accepted travel and loaded floor %d with %d collision triangles.",
             floorId, colTris);
    DrawTextScreen(70.0f, 370.0f, 9.0f, 14.0f, 248, 246, 224, 255, title);
    DrawTextWrapped(70.0f, 400.0f, 7.0f, 11.0f, 62, 2,
                    206, 222, 232, 255, detail);
}

static int RunWorldMapMenuScene(GLFWwindow* window, int smokeMode) {
    const char* archive = getenv("PCPORT_WORLDMAP_ARCHIVE");
    const char* capEnv = getenv("PCPORT_WORLDMAP_FRAMES");
    int frameCap = smokeMode ? 18 : 0;
    int frame;
    int cursor = 0;
    int moved = 0;
    int openedConfirm = 0;
    int accepted = 0;
    int acceptedFrame = -1;
    int acceptedFloor = -1;
    int targetLoaded = 0;
    int targetTris = 0;
    f32 targetSpawn[3] = { 0.0f, 0.0f, 0.0f };
    int confirmCursor = 0;
    u16 padHeld = 0;
    u16 padPrev = 0;
    u16 padPressed = 0;
    PADStatus pads[4];
    PCPortWorldMapMenuState state = PCPORT_WORLDMAP_SELECT;

    if (window == NULL) {
        fprintf(stderr,
                "[worldmap-menu-smoke] failed: no native window/GL context\n");
        return 0;
    }
    if (archive == NULL || archive[0] == '\0') {
        archive = PCPORT_WORLDMAP_ARCHIVE;
    }
    if (capEnv != NULL && atoi(capEnv) > 0) {
        frameCap = atoi(capEnv);
    }

    memset(pads, 0, sizeof(pads));
    PCPort_FieldColUnload();
    PCPort_FieldExitUnload();

    if (!PCPort_EngineFieldSetup(archive)) {
        fprintf(stderr,
                "[worldmap-menu-smoke] failed: could not load %s\n",
                archive);
        return 0;
    }
    if (!PCPort_EngineTitleReady()) {
        fprintf(stderr,
                "[worldmap-menu-smoke] failed: worldmap render scene not ready\n");
        return 0;
    }
    EnsureFontAtlas();

    printf("[worldmap-menu] opened %s with %d selectable destinations%s\n",
           archive,
           PCPORT_WORLDMAP_LOCATION_COUNT,
           smokeMode ? " [SMOKE]" : "");

    for (frame = 0; ; ++frame) {
        if (window != NULL && glfwWindowShouldClose(window)) {
            break;
        }
        if (frameCap > 0 && frame >= frameCap) {
            break;
        }

        VIWaitForRetrace_PC();
        PADRead(pads);
        padHeld = pads[0].button;

        if (smokeMode) {
            if (frame == 1) {
                padHeld = (u16)(padHeld | GCN_PAD_BUTTON_RIGHT);
            } else if (frame == 3) {
                padHeld = (u16)(padHeld | GCN_PAD_BUTTON_DOWN);
            } else if (frame == 5 || frame == 7) {
                padHeld = (u16)(padHeld | GCN_PAD_BUTTON_A);
            }
        }

        padPressed = (u16)(padHeld & ~padPrev);
        padPrev = padHeld;

        if (state == PCPORT_WORLDMAP_SELECT) {
            int oldCursor = cursor;
            if (padPressed & (GCN_PAD_BUTTON_RIGHT | GCN_PAD_BUTTON_DOWN)) {
                cursor = (cursor + 1) % PCPORT_WORLDMAP_LOCATION_COUNT;
            }
            if (padPressed & (GCN_PAD_BUTTON_LEFT | GCN_PAD_BUTTON_UP)) {
                cursor = (cursor + PCPORT_WORLDMAP_LOCATION_COUNT - 1) %
                         PCPORT_WORLDMAP_LOCATION_COUNT;
            }
            if (cursor != oldCursor) {
                moved = 1;
                printf("[worldmap-menu] cursor %d -> %s\n",
                       cursor, kWorldMapLocations[cursor].name);
            }
            if (padPressed & GCN_PAD_BUTTON_A) {
                if (kWorldMapLocations[cursor].floorId < 0) {
                    state = PCPORT_WORLDMAP_UNAVAILABLE;
                    printf("[worldmap-menu] select %s -> route unavailable\n",
                           kWorldMapLocations[cursor].name);
                } else {
                    state = PCPORT_WORLDMAP_CONFIRM;
                    confirmCursor = 0;
                    openedConfirm = 1;
                    printf("[worldmap-menu] select %s -> travel confirm\n",
                           kWorldMapLocations[cursor].name);
                }
            }
            if (!smokeMode && (padPressed & (GCN_PAD_BUTTON_B |
                                             GCN_PAD_BUTTON_START))) {
                break;
            }
        } else if (state == PCPORT_WORLDMAP_CONFIRM) {
            if (padPressed & GCN_PAD_BUTTON_LEFT) {
                confirmCursor = 0;
            }
            if (padPressed & GCN_PAD_BUTTON_RIGHT) {
                confirmCursor = 1;
            }
            if (padPressed & GCN_PAD_BUTTON_B) {
                state = PCPORT_WORLDMAP_SELECT;
                printf("[worldmap-menu] travel confirm cancelled\n");
            }
            if (padPressed & GCN_PAD_BUTTON_A) {
                if (confirmCursor == 0) {
                    acceptedFloor = kWorldMapLocations[cursor].floorId;
                    if (!PCPort_FieldWarpTo(acceptedFloor, targetSpawn)) {
                        fprintf(stderr,
                                "[worldmap-menu-smoke] failed: could not load accepted floor %d (%s)\n",
                                acceptedFloor,
                                kWorldMapLocations[cursor].name);
                        return 0;
                    }
                    targetLoaded = 1;
                    targetTris = PCPort_FieldColTriCount();
                    accepted = 1;
                    acceptedFrame = frame;
                    state = PCPORT_WORLDMAP_ACCEPTED;
                    printf("[worldmap-menu] travel accepted: %s floor=%d tris=%d spawn=(%.1f,%.1f,%.1f)\n",
                           kWorldMapLocations[cursor].name,
                           acceptedFloor,
                           targetTris,
                           targetSpawn[0],
                           targetSpawn[1],
                           targetSpawn[2]);
                } else {
                    state = PCPORT_WORLDMAP_SELECT;
                    printf("[worldmap-menu] travel declined\n");
                }
            }
        } else if (state == PCPORT_WORLDMAP_ACCEPTED) {
            if (!smokeMode && (padPressed & (GCN_PAD_BUTTON_A |
                                             GCN_PAD_BUTTON_B |
                                             GCN_PAD_BUTTON_START))) {
                break;
            }
            if (smokeMode && acceptedFrame >= 0 && frame > acceptedFrame + 1) {
                break;
            }
        } else if (state == PCPORT_WORLDMAP_UNAVAILABLE) {
            if (padPressed & (GCN_PAD_BUTTON_A | GCN_PAD_BUTTON_B |
                              GCN_PAD_BUTTON_START)) {
                state = PCPORT_WORLDMAP_SELECT;
            }
        }

        PCPort_EngineTitleRenderFrame();
        if (targetLoaded) {
            DrawWorldMapTravelResultOverlay(&kWorldMapLocations[cursor],
                                            acceptedFloor, targetTris);
        } else {
            DrawWorldMapMenuOverlay(cursor, state, confirmCursor);
        }
        GSgfxSwapBuffers(0);
    }

    if (smokeMode) {
        if (!moved || !openedConfirm || !accepted || cursor != 2 ||
            acceptedFloor != PC_FLOOR_PYRITE || !targetLoaded ||
            targetTris <= 0) {
            fprintf(stderr,
                    "[worldmap-menu-smoke] failed: moved=%d confirm=%d accepted=%d cursor=%d floor=%d loaded=%d tris=%d\n",
                    moved,
                    openedConfirm,
                    accepted,
                    cursor,
                    acceptedFloor,
                    targetLoaded,
                    targetTris);
            return 0;
        }
        printf("[worldmap-menu-smoke] passed: cursor->%s confirm=yes floor=%d tris=%d frames=%d\n",
               kWorldMapLocations[cursor].name,
               acceptedFloor,
               targetTris,
               frame);
    }
    return 1;
}

void PCPort_EngineTitleRenderFrame(void) {
    MenuTreeStats stats;
    if (!g_engTitleReady) {
        return;
    }
    memset(&stats, 0, sizeof(stats));

    /* Same per-frame 3D sequence RunMenuScene uses for the title scene. */
    ClearBackbuffer(0.0f, 0.0f, 0.0f);
    GSgfx_BeginFrame();
    /* Field maps don't cover the whole screen, so wipe GSgfx_BeginFrame's green EFB
     * clear-quad to the chosen background before the geometry draws. */
    if (g_engFieldMode) {
        ClearBackbuffer(g_engFieldBg[0], g_engFieldBg[1], g_engFieldBg[2]);
    }

    GXSetViewport((f32)g_engTitleCamera.viewportLeft,
                  (f32)g_engTitleCamera.viewportTop,
                  (f32)(g_engTitleCamera.viewportRight - g_engTitleCamera.viewportLeft),
                  (f32)(g_engTitleCamera.viewportBottom - g_engTitleCamera.viewportTop),
                  0.0f, 1.0f);
    GXSetScissor((u32)g_engTitleCamera.scissorLeft,
                 (u32)g_engTitleCamera.scissorTop,
                 (u32)(g_engTitleCamera.scissorRight - g_engTitleCamera.scissorLeft),
                 (u32)(g_engTitleCamera.scissorBottom - g_engTitleCamera.scissorTop));
    GXSetProjection(g_engTitleCamera.projectionMatrix, GX_PERSPECTIVE);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

    RenderJointTree(&g_engTitleArchive, g_engTitleRootJoint, g_engTitleRootJoint,
                    &g_engTitleCamera, (int)PCPORT_REAL_MATERIAL_PIPELINE, &stats);

    /* Render every additional scene model set (field maps split the room across
     * several jointLists -- the floor lives in one of these, not the primary). */
    {
        int mi;
        for (mi = 0; mi < g_engExtraRootJointCount; ++mi) {
            RenderJointTree(&g_engTitleArchive, g_engExtraRootJoints[mi],
                            g_engExtraRootJoints[mi], &g_engTitleCamera,
                            (int)PCPORT_REAL_MATERIAL_PIPELINE, &stats);
        }
    }

    /* One-shot coverage report (PCPORT_RENDER_DEBUG) for the field/title scene:
     * joints/dobjs walked and how many drew vs skipped vs textured. */
    if (getenv("PCPORT_RENDER_DEBUG") != NULL) {
        static int reported = 0;
        if (!reported) {
            reported = 1;
            printf("[render] scene walk: joints=%u dobjs=%u drawn=%u skipped=%u "
                   "textured=%u materialOnly=%u rootJoint=0x%X\n",
                   stats.joints, stats.dobjs, stats.drawn, stats.skipped,
                   stats.textured, stats.materialOnly, g_engTitleRootJoint);
        }
    }
}

typedef struct PCPortBattleRenderActor {
    const char* label;
    const char* member;
    const char* displayName;
    u16 trainerId;
    u8 teamSlot;
    u16 speciesId;
    u8 level;
    u16 tableMoves[4];
    f32 x;
    f32 y;
    f32 z;
    f32 yaw;
    f32 scale;
    PCPortHSDArchive archive;
    PCPortHostMotionBank motionBank;
    HSD_JObj* liveRoot;
    u32 rootJoint;
    u32 stanceMotion;
    u32 attackMotion;
    u32 damageMotion;
    u32 activeMotion;
    f32 motionTime;
    f32 motionEndFrame;
    f32 prevChecksum;
    f32 lastChecksum;
    int loaded;
    int motionLoaded;
} PCPortBattleRenderActor;

typedef struct PCPortBattleSpeciesMember {
    u16 speciesId;
    const char* member;
    const char* displayName;
} PCPortBattleSpeciesMember;

typedef struct PCPortBattleTableSetup {
    u16 playerTrainerId;
    u16 enemyTrainerId;
    u16 playerMoveId;
    u16 enemyMoveId;
    u16 playerTextId;
    u16 enemyTextId;
    int playerDamage;
    int enemyDamage;
    char playerMoveName[32];
    char enemyMoveName[32];
    int commonRelLoaded;
} PCPortBattleTableSetup;

#define PCPORT_COMMON_REL_MOVE_DATA_OFFSET       0x11E048u
#define PCPORT_COMMON_REL_TRAINER_DATA_OFFSET    0x092ED0u
#define PCPORT_COMMON_REL_TRAINER_POKEMON_OFFSET 0x09FE28u
#define PCPORT_COMMON_REL_MOVE_DATA_SIZE         0x38u
#define PCPORT_COMMON_REL_TRAINER_DATA_SIZE      0x34u
#define PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE   0x50u
#define PCPORT_COMMON_REL_MAX_TRAINER_POKEMON    5510u

static const PCPortBattleSpeciesMember kPcportBattleSpeciesMembers[] = {
    { 25u,  "pikachu",   "Pikachu" },
    { 196u, "eifie",     "Eifie" },
    { 197u, "blacky",    "Blacky" },
    { 292u, "nukenin",   "Nukenin" },
    { 316u, "gokulin",   "Gokulin" },
    { 317u, "marunoom",  "Marunoom" },
    { 335u, "zangoose",  "Zangoose" },
    { 336u, "habunake",  "Habunake" },
    { 359u, "absol",     "Absol" }
};

static u16 PCPort_ReadBigEndianU16Local(const u8* data) {
    return (u16)(((u16)data[0] << 8) | (u16)data[1]);
}

static int PCPort_BattleRangeValid(u32 base, u32 stride, u32 index,
                                   u32 need, u32 size) {
    u32 off;
    if (stride != 0u && index > (0xFFFFFFFFu - base) / stride) {
        return 0;
    }
    off = base + index * stride;
    return off <= size && need <= size - off;
}

static int PCPort_BattleParseU16Env(const char* name, u16* outValue) {
    u32 value;
    if (outValue == NULL || !PCPort_ParseU32Env(name, &value)) {
        return 0;
    }
    *outValue = (u16)value;
    return 1;
}

static int PCPort_BattleParseIntEnvWithDefault(const char* name, int fallback) {
    int value;
    return PCPort_ParseIntEnv(name, &value) ? value : fallback;
}

static const PCPortBattleSpeciesMember*
PCPort_BattleFindSpeciesMember(u16 speciesId) {
    u32 i;
    for (i = 0u; i < sizeof(kPcportBattleSpeciesMembers) /
                    sizeof(kPcportBattleSpeciesMembers[0]); ++i) {
        if (kPcportBattleSpeciesMembers[i].speciesId == speciesId) {
            return &kPcportBattleSpeciesMembers[i];
        }
    }
    return NULL;
}

static int PCPort_BattleTrainerPokemonValid(const u8* commonRel,
                                            u32 commonRelSize,
                                            u32 index) {
    (void)commonRel;
    return index < PCPORT_COMMON_REL_MAX_TRAINER_POKEMON &&
           PCPort_BattleRangeValid(PCPORT_COMMON_REL_TRAINER_POKEMON_OFFSET,
                                   PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE,
                                   index,
                                   PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE,
                                   commonRelSize);
}

static u16 PCPort_BattleTrainerFirstPokemon(const u8* commonRel,
                                            u32 commonRelSize,
                                            u16 trainerId,
                                            int* outValid) {
    u32 off;
    if (!PCPort_BattleRangeValid(PCPORT_COMMON_REL_TRAINER_DATA_OFFSET,
                                 PCPORT_COMMON_REL_TRAINER_DATA_SIZE,
                                 trainerId,
                                 PCPORT_COMMON_REL_TRAINER_DATA_SIZE,
                                 commonRelSize)) {
        if (outValid != NULL) {
            *outValid = 0;
        }
        return 0u;
    }
    off = PCPORT_COMMON_REL_TRAINER_DATA_OFFSET +
          (u32)trainerId * PCPORT_COMMON_REL_TRAINER_DATA_SIZE;
    if (outValid != NULL) {
        *outValid = 1;
    }
    return PCPort_ReadBigEndianU16Local(commonRel + off + 0x04);
}

static u16 PCPort_BattleTrainerPokemonSpecies(const u8* commonRel,
                                              u32 index) {
    u32 off = PCPORT_COMMON_REL_TRAINER_POKEMON_OFFSET +
              index * PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE;
    return PCPort_ReadBigEndianU16Local(commonRel + off + 0x0A);
}

static u8 PCPort_BattleTrainerPokemonLevel(const u8* commonRel, u32 index) {
    u32 off = PCPORT_COMMON_REL_TRAINER_POKEMON_OFFSET +
              index * PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE;
    return commonRel[off + 0x04];
}

static u16 PCPort_BattleTrainerPokemonMove(const u8* commonRel,
                                           u32 index,
                                           u32 moveSlot) {
    u32 off = PCPORT_COMMON_REL_TRAINER_POKEMON_OFFSET +
              index * PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE;
    if (moveSlot >= 4u) {
        return 0u;
    }
    return PCPort_ReadBigEndianU16Local(commonRel + off + 0x34 + moveSlot * 2u);
}

static u16 PCPort_BattleFirstUsableMove(const PCPortBattleRenderActor* actor) {
    u32 i;
    if (actor == NULL) {
        return 0u;
    }
    for (i = 0u; i < 4u; ++i) {
        u16 move = actor->tableMoves[i];
        if (move != 0u && move != 0xFFFFu && move < 512u) {
            return move;
        }
    }
    return 0u;
}

static int PCPort_BattleMovePower(const u8* commonRel,
                                  u32 commonRelSize,
                                  u16 moveId,
                                  u16* outNameText,
                                  u16* outAnim) {
    u32 moveIndex;
    u32 off;
    if (moveId == 0u) {
        return -1;
    }
    moveIndex = (u32)moveId - 1u;
    if (!PCPort_BattleRangeValid(PCPORT_COMMON_REL_MOVE_DATA_OFFSET,
                                 PCPORT_COMMON_REL_MOVE_DATA_SIZE,
                                 moveIndex,
                                 PCPORT_COMMON_REL_MOVE_DATA_SIZE,
                                 commonRelSize)) {
        return -1;
    }
    off = PCPORT_COMMON_REL_MOVE_DATA_OFFSET +
          moveIndex * PCPORT_COMMON_REL_MOVE_DATA_SIZE;
    if (outNameText != NULL) {
        *outNameText = PCPort_ReadBigEndianU16Local(commonRel + off + 0x22);
    }
    if (outAnim != NULL) {
        *outAnim = PCPort_ReadBigEndianU16Local(commonRel + off + 0x32);
    }
    return commonRel[off + 0x17];
}

static int PCPort_BattleApplyTrainerTeam(const u8* commonRel,
                                         u32 commonRelSize,
                                         PCPortBattleRenderActor actors[4],
                                         u32 actorBase,
                                         u16 trainerId,
                                         const char* sideLabel,
                                         const char* env0,
                                         const char* env1,
                                         u16* outMoveId,
                                         int allowMoveDerive) {
    int trainerValid = 0;
    u16 firstPokemon;
    u32 slot;
    if (actors == NULL || actorBase + 1u >= 4u) {
        return 0;
    }
    if ((getenv(env0) != NULL && getenv(env0)[0] != '\0') ||
        (getenv(env1) != NULL && getenv(env1)[0] != '\0')) {
        printf("[battle-table] side=%s status=actor-env-override trainer=0x%04X\n",
               sideLabel, trainerId);
        return 0;
    }
    firstPokemon = PCPort_BattleTrainerFirstPokemon(commonRel, commonRelSize,
                                                    trainerId, &trainerValid);
    if (!trainerValid) {
        printf("[battle-table] side=%s status=trainer-oob trainer=0x%04X\n",
               sideLabel, trainerId);
        return 0;
    }
    for (slot = 0u; slot < 2u; ++slot) {
        u32 tableIndex = (u32)firstPokemon + slot;
        u16 species;
        const PCPortBattleSpeciesMember* member;
        u32 moveSlot;
        PCPortBattleRenderActor* actor;
        if (!PCPort_BattleTrainerPokemonValid(commonRel, commonRelSize,
                                              tableIndex)) {
            printf("[battle-table] side=%s status=team-oob trainer=0x%04X "
                   "firstPokemon=%u slot=%u\n",
                   sideLabel, trainerId, firstPokemon, slot);
            return 0;
        }
        species = PCPort_BattleTrainerPokemonSpecies(commonRel, tableIndex);
        member = PCPort_BattleFindSpeciesMember(species);
        if (member == NULL) {
            printf("[battle-table] side=%s status=unmapped-species "
                   "trainer=0x%04X firstPokemon=%u slot=%u species=%u\n",
                   sideLabel, trainerId, firstPokemon, slot, species);
            return 0;
        }
        actor = &actors[actorBase + slot];
        actor->trainerId = trainerId;
        actor->teamSlot = (u8)slot;
        actor->speciesId = species;
        actor->member = member->member;
        actor->displayName = member->displayName;
        actor->level = PCPort_BattleTrainerPokemonLevel(commonRel, tableIndex);
        for (moveSlot = 0u; moveSlot < 4u; ++moveSlot) {
            actor->tableMoves[moveSlot] =
                PCPort_BattleTrainerPokemonMove(commonRel, tableIndex,
                                                moveSlot);
        }
    }
    if (allowMoveDerive && outMoveId != NULL) {
        u16 derivedMove = PCPort_BattleFirstUsableMove(&actors[actorBase]);
        if (derivedMove != 0u) {
            *outMoveId = derivedMove;
        }
    }
    printf("[battle-table] side=%s status=trainer-applied trainer=0x%04X "
           "firstPokemon=%u members=%s/%s species=%u/%u levels=%u/%u "
           "moves0=%u/%u/%u/%u\n",
           sideLabel, trainerId, firstPokemon,
           actors[actorBase].member, actors[actorBase + 1u].member,
           actors[actorBase].speciesId, actors[actorBase + 1u].speciesId,
           actors[actorBase].level, actors[actorBase + 1u].level,
           actors[actorBase].tableMoves[0], actors[actorBase].tableMoves[1],
           actors[actorBase].tableMoves[2], actors[actorBase].tableMoves[3]);
    return 1;
}

static void PCPort_BattleInitSetup(PCPortBattleTableSetup* setup) {
    if (setup == NULL) {
        return;
    }
    memset(setup, 0, sizeof(*setup));
    setup->playerTrainerId = 0x0001u;
    setup->enemyTrainerId = 0x0200u;
    setup->playerMoveId = 129u;
    setup->enemyMoveId = 44u;
    setup->playerTextId = 0x8001u;
    setup->enemyTextId = 0x8002u;
    setup->playerDamage = 32;
    setup->enemyDamage = 21;
    snprintf(setup->playerMoveName, sizeof(setup->playerMoveName), "Swift");
    snprintf(setup->enemyMoveName, sizeof(setup->enemyMoveName), "Bite");
    PCPort_BattleParseU16Env("PCPORT_BATTLE_PLAYER_TRAINER",
                             &setup->playerTrainerId);
    PCPort_BattleParseU16Env("PCPORT_BATTLE_ENEMY_TRAINER",
                             &setup->enemyTrainerId);
    PCPort_BattleParseU16Env("PCPORT_BATTLE_PLAYER_MOVE_ID",
                             &setup->playerMoveId);
    PCPort_BattleParseU16Env("PCPORT_BATTLE_ENEMY_MOVE_ID",
                             &setup->enemyMoveId);
    PCPort_BattleParseU16Env("PCPORT_BATTLE_PLAYER_TEXT_ID",
                             &setup->playerTextId);
    PCPort_BattleParseU16Env("PCPORT_BATTLE_ENEMY_TEXT_ID",
                             &setup->enemyTextId);
    setup->playerDamage = PCPort_BattleParseIntEnvWithDefault(
        "PCPORT_BATTLE_PLAYER_DAMAGE", setup->playerDamage);
    setup->enemyDamage = PCPort_BattleParseIntEnvWithDefault(
        "PCPORT_BATTLE_ENEMY_DAMAGE", setup->enemyDamage);
    if (getenv("PCPORT_BATTLE_PLAYER_MOVE") != NULL &&
        getenv("PCPORT_BATTLE_PLAYER_MOVE")[0] != '\0') {
        snprintf(setup->playerMoveName, sizeof(setup->playerMoveName), "%s",
                 getenv("PCPORT_BATTLE_PLAYER_MOVE"));
    }
    if (getenv("PCPORT_BATTLE_ENEMY_MOVE") != NULL &&
        getenv("PCPORT_BATTLE_ENEMY_MOVE")[0] != '\0') {
        snprintf(setup->enemyMoveName, sizeof(setup->enemyMoveName), "%s",
                 getenv("PCPORT_BATTLE_ENEMY_MOVE"));
    }
}

static void PCPort_BattleApplyCommonRelSetup(PCPortBattleTableSetup* setup,
                                             PCPortBattleRenderActor actors[4]) {
    u8* commonRel = NULL;
    u32 commonRelSize = 0u;
    u16 playerNameText = 0u, enemyNameText = 0u;
    u16 playerAnim = 0u, enemyAnim = 0u;
    int playerPower, enemyPower;

    if (setup == NULL || actors == NULL) {
        return;
    }
    if (!PCPort_LoadFsysMember("orig/GC6E01/disc/files/common.fsys",
                               "pcommon_rel", &commonRel, &commonRelSize) &&
        !PCPort_LoadFsysMember("orig/GC6E01/disc/files/common.fsys",
                               "common_rel", &commonRel, &commonRelSize)) {
        printf("[battle-table] common_rel load=failed source=common.fsys\n");
        return;
    }
    setup->commonRelLoaded = 1;
    PCPort_BattleApplyTrainerTeam(
        commonRel, commonRelSize, actors, 0u, setup->playerTrainerId,
        "player", "PCPORT_BATTLE_P0", "PCPORT_BATTLE_P1",
        &setup->playerMoveId, getenv("PCPORT_BATTLE_PLAYER_MOVE_ID") == NULL);
    PCPort_BattleApplyTrainerTeam(
        commonRel, commonRelSize, actors, 2u, setup->enemyTrainerId,
        "enemy", "PCPORT_BATTLE_E0", "PCPORT_BATTLE_E1",
        &setup->enemyMoveId, getenv("PCPORT_BATTLE_ENEMY_MOVE_ID") == NULL);

    if (getenv("PCPORT_BATTLE_PLAYER_MOVE") == NULL &&
        setup->playerMoveId != 129u) {
        snprintf(setup->playerMoveName, sizeof(setup->playerMoveName),
                 "Table Move %u", setup->playerMoveId);
    }
    if (getenv("PCPORT_BATTLE_ENEMY_MOVE") == NULL &&
        setup->enemyMoveId != 44u) {
        snprintf(setup->enemyMoveName, sizeof(setup->enemyMoveName),
                 "Table Move %u", setup->enemyMoveId);
    }

    playerPower = PCPort_BattleMovePower(commonRel, commonRelSize,
                                         setup->playerMoveId,
                                         &playerNameText, &playerAnim);
    enemyPower = PCPort_BattleMovePower(commonRel, commonRelSize,
                                       setup->enemyMoveId,
                                       &enemyNameText, &enemyAnim);
    if (getenv("PCPORT_BATTLE_PLAYER_DAMAGE") == NULL && playerPower > 0) {
        setup->playerDamage = playerPower / 2 + 2;
    }
    if (getenv("PCPORT_BATTLE_ENEMY_DAMAGE") == NULL && enemyPower > 0) {
        setup->enemyDamage = enemyPower / 2 + 2;
    }
    printf("[battle-table] common_rel load=ok size=0x%X playerMove=%u "
           "enemyMove=%u playerPower=%d enemyPower=%d playerNameText=0x%04X "
           "enemyNameText=0x%04X playerAnim=%u enemyAnim=%u\n",
           commonRelSize, setup->playerMoveId, setup->enemyMoveId,
           playerPower, enemyPower, playerNameText, enemyNameText,
           playerAnim, enemyAnim);
    PCPort_FreeBuffer(commonRel);
}

#define PCPORT_PKX_VIEWER_MAX_MODELS 640
#define PCPORT_PKX_VIEWER_NAME_MAX 64

typedef struct PCPortPkxViewerState {
    int enabled;
    int selectedSlot;
    int selectedModel[4];
    int selectedMotion[4];
    f32 inspectYaw[4];
    f32 baseScale;
    int autoSpin;
    int startBattle;
    int singleDebug;
    int sweepEnabled;
    int sweepSlot;
    int sweepNextModel;
    int sweepFrameStep;
    int sweepSuccesses;
    int sweepFailures;
    int sweepDone;
    int modelCount;
    char modelNames[PCPORT_PKX_VIEWER_MAX_MODELS][PCPORT_PKX_VIEWER_NAME_MAX];
    int prevTab, prevLeft, prevRight, prevUp, prevDown;
    int prevOne, prevTwo, prevThree, prevFour;
    int prevQ, prevE, prevR, prevEnter, prevZ;
} PCPortPkxViewerState;

static const char* kPcportPkxViewerFallbackModels[] = {
    "zangoose", "gokulin", "nukenin", "boober", "bangiras",
    "absol", "raichu", "arbok", "mew", "nendoll", "sakurabyss"
};

static int PCPort_PkxViewerAddModel(PCPortPkxViewerState* viewer,
                                    const char* name) {
    int i;
    if (viewer == NULL || name == NULL || name[0] == '\0' ||
        viewer->modelCount >= PCPORT_PKX_VIEWER_MAX_MODELS) {
        return 0;
    }
    for (i = 0; i < viewer->modelCount; ++i) {
        if (strcmp(viewer->modelNames[i], name) == 0) {
            return 0;
        }
    }
    snprintf(viewer->modelNames[viewer->modelCount],
             sizeof(viewer->modelNames[viewer->modelCount]), "%s", name);
    viewer->modelCount++;
    return 1;
}

static int PCPort_PkxAuditRowLooksPokemon(const char* member,
                                          const char* materialOnly) {
    if (member == NULL || materialOnly == NULL) {
        return 0;
    }
    if (strcmp(member, "egg") == 0 || strcmp(member, "pkx_egg") == 0) {
        return 0;
    }
    if (strcmp(member, "yukiwarashi") == 0) {
        return 1;
    }
    return atoi(materialOnly) > 0;
}

static void PCPort_PkxViewerLoadModelList(PCPortPkxViewerState* viewer) {
    FILE* f;
    char line[512];
    int auditRows = 0;
    int auditKept = 0;
    int auditSkipped = 0;
    int i;
    if (viewer == NULL) {
        return;
    }
    f = fopen("build_pc/pkx_model_audit.csv", "rb");
    if (f != NULL) {
        while (fgets(line, sizeof(line), f) != NULL) {
            char* archive;
            char* member;
            char* status;
            char* materialOnly;
            char* comma0;
            char* comma1;
            char* comma2;
            char* field;
            int col;
            if (strncmp(line, "archive,", 8) == 0) {
                continue;
            }
            auditRows++;
            archive = line;
            comma0 = strchr(archive, ',');
            if (comma0 == NULL) {
                continue;
            }
            *comma0 = '\0';
            member = comma0 + 1;
            comma1 = strchr(member, ',');
            if (comma1 == NULL) {
                continue;
            }
            *comma1 = '\0';
            status = comma1 + 1;
            comma2 = strchr(status, ',');
            if (comma2 != NULL) {
                *comma2 = '\0';
            }
            materialOnly = NULL;
            field = comma2 != NULL ? comma2 + 1 : NULL;
            for (col = 4; field != NULL && col <= 13; ++col) {
                char* next = strchr(field, ',');
                if (next != NULL) {
                    *next = '\0';
                }
                if (col == 13) {
                    materialOnly = field;
                    break;
                }
                field = next != NULL ? next + 1 : NULL;
            }
            if (strncmp(archive, "pkx_", 4) == 0 &&
                strcmp(status, "fail") != 0 &&
                PCPort_PkxAuditRowLooksPokemon(member, materialOnly)) {
                if (PCPort_PkxViewerAddModel(viewer, member)) {
                    auditKept++;
                }
            } else if (strncmp(archive, "pkx_", 4) == 0) {
                auditSkipped++;
            }
        }
        fclose(f);
    }
    if (viewer->modelCount == 0) {
        for (i = 0; i < (int)(sizeof(kPcportPkxViewerFallbackModels) /
                              sizeof(kPcportPkxViewerFallbackModels[0])); ++i) {
            PCPort_PkxViewerAddModel(viewer, kPcportPkxViewerFallbackModels[i]);
        }
    }
    printf("[pkx-viewer] audit rows=%d keptPokemon=%d skippedNonPokemon=%d "
           "first=%s last=%s\n",
           auditRows, auditKept, auditSkipped,
           viewer->modelCount > 0 ? viewer->modelNames[0] : "-",
           viewer->modelCount > 0 ?
               viewer->modelNames[viewer->modelCount - 1] : "-");
}

static int PCPort_PkxViewerFindModel(const PCPortPkxViewerState* viewer,
                                     const char* name) {
    int i;
    if (viewer == NULL || name == NULL) {
        return 0;
    }
    for (i = 0; i < viewer->modelCount; ++i) {
        if (strcmp(viewer->modelNames[i], name) == 0) {
            return i;
        }
    }
    return 0;
}

static int PCPort_LoadPkxRenderActor(PCPortBattleRenderActor* actor) {
    char fsysPath[320];
    u8* memberData = NULL;
    u32 memberSize = 0u;
    u32 off;

    if (actor == NULL || actor->member == NULL) {
        return 0;
    }
    snprintf(fsysPath, sizeof(fsysPath),
             "orig/GC6E01/disc/files/pkx_%s.fsys", actor->member);
    if (!PCPort_LoadFsysMember(fsysPath, actor->member, &memberData,
                               &memberSize)) {
        fprintf(stderr, "[battle-scene] load failed %s :: %s\n",
                fsysPath, actor->member);
        return 0;
    }

    for (off = 0u; off + 0x20u <= memberSize && off <= 0x400u; off += 4u) {
        u32 fileSize = PCPort_ReadBigEndianU32(memberData + off + 0x00);
        PCPortHSDArchive candidate;
        const u8* sceneData;
        u32 sceneOffset = 0u;
        u32 branchOff;
        u32 jointListOff;
        u32 rootJoint;

        if (fileSize < 0x20u || fileSize > memberSize - off) {
            continue;
        }
        memset(&candidate, 0, sizeof(candidate));
        if (!PCPort_HSDArchiveParseBE(&candidate, memberData + off, fileSize)) {
            continue;
        }
        sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(
            &candidate, "scene_data", &sceneOffset);
        if (sceneData == NULL) {
            PCPort_HSDArchiveDestroy(&candidate);
            continue;
        }
        branchOff = PCPort_ReadBigEndianU32(sceneData + 0x00);
        if (!ArchiveRangeValid(&candidate, branchOff, 0x04u)) {
            PCPort_HSDArchiveDestroy(&candidate);
            continue;
        }
        jointListOff = PCPort_ReadBigEndianU32(candidate.storage + branchOff);
        if (!ArchiveRangeValid(&candidate, jointListOff, 0x04u)) {
            PCPort_HSDArchiveDestroy(&candidate);
            continue;
        }
        rootJoint = PCPort_ReadBigEndianU32(candidate.storage + jointListOff);
        if (!ArchiveRangeValid(&candidate, rootJoint,
                               PCPORT_SERIALIZED_JOINT_SIZE)) {
            PCPort_HSDArchiveDestroy(&candidate);
            continue;
        }

        actor->archive = candidate;
        actor->rootJoint = rootJoint;
        actor->loaded = 1;
        if (PCPort_HostMotionBankLoad(fsysPath, actor->member,
                                      &actor->motionBank, 0)) {
            actor->motionLoaded = 1;
        } else {
            fprintf(stderr,
                    "[battle-scene] motion bank unavailable %s :: %s\n",
                    fsysPath, actor->member);
        }
        PCPort_FreeBuffer(memberData);
        printf("[battle-scene] actor=%s member=%s root=0x%X wrapper=0x%X "
               "motions=%u\n",
               actor->label, actor->member, actor->rootJoint, off,
               actor->motionLoaded ? actor->motionBank.motionCount : 0u);
        return 1;
    }

    PCPort_FreeBuffer(memberData);
    fprintf(stderr, "[battle-scene] no scene_data HSD payload for %s\n",
            actor->member);
    return 0;
}

static int PCPort_BattleActorSetMotion(PCPortBattleRenderActor* actor,
                                       u32 motionIdx,
                                       int frame,
                                       const char* reason);

static void PCPort_FreeBattleRenderActors(PCPortBattleRenderActor* actors,
                                          int count) {
    int i;
    if (actors == NULL) {
        return;
    }
    for (i = 0; i < count; ++i) {
        if (actors[i].liveRoot != NULL) {
            HSD_JObjRemoveAll(actors[i].liveRoot);
            actors[i].liveRoot = NULL;
        }
        if (actors[i].motionLoaded) {
            PCPort_HostMotionBankRelease(&actors[i].motionBank);
            actors[i].motionLoaded = 0;
        }
        if (actors[i].loaded) {
            PCPort_HSDArchiveDestroy(&actors[i].archive);
            actors[i].loaded = 0;
        }
    }
}

static int PCPort_ReloadBattleRenderActor(PCPortBattleRenderActor* actor,
                                          const char* member,
                                          u32 motionIdx,
                                          int frame,
                                          const char* reason) {
    PCPortBattleRenderActor temp;
    if (actor == NULL || member == NULL || member[0] == '\0') {
        return 0;
    }

    memset(&temp, 0, sizeof(temp));
    temp.label = actor->label;
    temp.member = member;
    temp.displayName = member;
    temp.x = actor->x;
    temp.y = actor->y;
    temp.z = actor->z;
    temp.yaw = actor->yaw;
    temp.scale = actor->scale;
    temp.stanceMotion = actor->stanceMotion;
    temp.attackMotion = actor->attackMotion;
    temp.damageMotion = actor->damageMotion;
    if (!PCPort_LoadPkxRenderActor(&temp)) {
        fprintf(stderr, "[pkx-viewer] reload failed slot=%s member=%s\n",
                actor->label != NULL ? actor->label : "-", member);
        return 0;
    }
    if (temp.motionLoaded && temp.motionBank.motionCount > 0u) {
        if (motionIdx >= temp.motionBank.motionCount) {
            motionIdx = 0u;
        }
        if (!PCPort_BattleActorSetMotion(&temp, motionIdx, frame, reason)) {
            PCPort_FreeBattleRenderActors(&temp, 1);
            fprintf(stderr, "[pkx-viewer] reload motion failed slot=%s member=%s "
                    "motion=%u\n",
                    actor->label != NULL ? actor->label : "-", member,
                    motionIdx);
            return 0;
        }
    }
    PCPort_FreeBattleRenderActors(actor, 1);
    *actor = temp;
    return 1;
}

static int PCPort_BattleActorSetMotion(PCPortBattleRenderActor* actor,
                                       u32 motionIdx,
                                       int frame,
                                       const char* reason) {
    if (actor == NULL || !actor->motionLoaded ||
        motionIdx >= actor->motionBank.motionCount) {
        return 0;
    }
    if (actor->liveRoot != NULL && actor->activeMotion == motionIdx) {
        return 1;
    }
    if (actor->liveRoot != NULL) {
        HSD_JObjRemoveAll(actor->liveRoot);
        actor->liveRoot = NULL;
    }
    actor->motionTime = 0.0f;
    actor->motionEndFrame = 0.0f;
    actor->activeMotion = motionIdx;
    actor->liveRoot = PCPort_HostMotionCreateRoot(&actor->motionBank,
                                                  motionIdx,
                                                  &actor->motionEndFrame);
    if (actor->liveRoot == NULL) {
        fprintf(stderr,
                "[battle-anim] frame=%d actor=%s motion=%u setup=failed\n",
                frame, actor->label, motionIdx);
        return 0;
    }
    actor->prevChecksum = PCPort_HostMotionSRTChecksum(actor->liveRoot);
    actor->lastChecksum = actor->prevChecksum;
    printf("[battle-anim] frame=%d actor=%s member=%s motion=%u reason=%s "
           "end=%.1f checksum=%.4f\n",
           frame, actor->label, actor->member, motionIdx,
           reason != NULL ? reason : "state", actor->motionEndFrame,
           actor->lastChecksum);
    fflush(stdout);
    return 1;
}

static void PCPort_BattleActorStep(PCPortBattleRenderActor* actor) {
    if (actor == NULL || actor->liveRoot == NULL || !actor->loaded) {
        return;
    }
    actor->prevChecksum = actor->lastChecksum;
    PCPort_HostMotionStepAndApply(actor->liveRoot, &actor->archive,
                                  actor->rootJoint, &actor->motionTime,
                                  actor->motionEndFrame,
                                  getenv("PCPORT_BATTLE_ROOT_TRANSLATE") != NULL);
    actor->lastChecksum = PCPort_HostMotionSRTChecksum(actor->liveRoot);
}

static void PCPort_BattleLogMotionSample(PCPortBattleRenderActor* actors,
                                         int actorCount,
                                         int frame,
                                         const char* stateText) {
    int i;
    printf("[battle-anim-sample] frame=%d state=\"%s\"",
           frame, stateText != NULL ? stateText : "");
    for (i = 0; i < actorCount; ++i) {
        f32 delta = fabsf(actors[i].lastChecksum - actors[i].prevChecksum);
        printf(" %s{motion=%u checksum=%.4f delta=%.5f}",
               actors[i].label, actors[i].activeMotion,
               actors[i].lastChecksum, delta);
    }
    printf("\n");
    fflush(stdout);
}

static void PCPort_BuildActorModelMatrix(const PCPortBattleRenderActor* actor,
                                         f32 out[3][4]) {
    f32 c = cosf(actor != NULL ? actor->yaw : 0.0f);
    f32 s = sinf(actor != NULL ? actor->yaw : 0.0f);
    f32 scale = (actor != NULL && actor->scale > 0.0f) ? actor->scale : 1.0f;
    memset(out, 0, sizeof(f32) * 12u);
    out[0][0] = c * scale;
    out[0][2] = s * scale;
    out[1][1] = scale;
    out[2][0] = -s * scale;
    out[2][2] = c * scale;
    if (actor != NULL) {
        out[0][3] = actor->x;
        out[1][3] = actor->y;
        out[2][3] = actor->z;
    }
}

static void PCPort_SetBattleCameraView(PCPortTranslatedCamera* camera, int frame) {
    /* Env-tunable while we dial in battle framing (the diamond is x=+/-30, z=+/-50;
     * enemies at z=+50 project low and were hiding under the 2D UI panels). Defaults
     * pulled back + up so all 4 actors frame above the command UI.
     *   PCPORT_BATTLE_CAM = "eyeX,eyeY,eyeZ,atX,atY,atZ" overrides everything. */
    f32 orbit = sinf((f32)frame * 0.010f) * 4.0f;
    f32 lift = cosf((f32)frame * 0.007f) * 1.5f;
    f32 eye[3] = { orbit, 40.0f + lift, 138.0f };
    f32 interest[3] = { 0.0f, 15.0f, 0.0f };
    f32 up[3] = { 0.0f, 1.0f, 0.0f };
    const char* cam = getenv("PCPORT_BATTLE_CAM");
    if (cam != NULL && cam[0] != '\0') {
        float e0,e1,e2,a0,a1,a2;
        if (sscanf(cam, "%f,%f,%f,%f,%f,%f", &e0,&e1,&e2,&a0,&a1,&a2) == 6) {
            eye[0]=e0+orbit; eye[1]=e1+lift; eye[2]=e2;
            interest[0]=a0; interest[1]=a1; interest[2]=a2;
        }
    }
    BuildViewMatrixLookAt(eye, interest, up, camera->viewMatrix);
}

static void DrawBattleBackdrop2D(GXTexObj* battleBackdropTex,
                                 int haveBattleBackdrop) {
    BeginMenuOverlay();
    DrawSolidScreenRect(0.0f, 0.0f, 640.0f, 148.0f, 20, 28, 40, 255);
    DrawSolidScreenRect(0.0f, 148.0f, 640.0f, 78.0f, 42, 50, 56, 255);
    DrawSolidScreenRect(0.0f, 226.0f, 640.0f, 82.0f, 72, 76, 70, 255);
    DrawSolidScreenRect(0.0f, 252.0f, 640.0f, 34.0f, 100, 104, 92, 210);
    /* The loaded fight_common backdrop is the wrong art (a GameCube controller
     * button-reference image: R/L/X/Y/C/B/START/1P glyphs) and just clutters the
     * field. Keep the clean gradient bands above; only draw the texture if asked
     * via PCPORT_BATTLE_BACKDROP=1 (for when the correct arena art is wired). */
    if (haveBattleBackdrop && getenv("PCPORT_BATTLE_BACKDROP") != NULL) {
        DrawTexturedScreenRectA(battleBackdropTex, 0.0f, 230.0f,
                                640.0f, 108.0f, 0.0f, 0.0f,
                                1.0f, 1.0f, 48, 132);
    }
}

static void DrawBattleArenaFloor(void) {
    int i;
    GXHostSetLightingEnabled(GX_FALSE);
    GXSetNumTexGens(0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetCullMode(GX_CULL_NONE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_COPY);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXColor4u8(88, 92, 96, 255);
    GXPosition3f32(-190.0f, -2.0f, -118.0f);
    GXColor4u8(88, 92, 96, 255);
    GXPosition3f32(190.0f, -2.0f, -118.0f);
    GXColor4u8(46, 54, 66, 255);
    GXPosition3f32(190.0f, 78.0f, -126.0f);
    GXColor4u8(46, 54, 66, 255);
    GXPosition3f32(-190.0f, 78.0f, -126.0f);
    GXEnd();

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXColor4u8(118, 122, 112, 255);
    GXPosition3f32(-170.0f, -2.0f, -105.0f);
    GXColor4u8(118, 122, 112, 255);
    GXPosition3f32(170.0f, -2.0f, -105.0f);
    GXColor4u8(80, 86, 82, 255);
    GXPosition3f32(170.0f, -2.0f, 105.0f);
    GXColor4u8(80, 86, 82, 255);
    GXPosition3f32(-170.0f, -2.0f, 105.0f);
    GXEnd();

    for (i = -3; i <= 3; ++i) {
        f32 x = (f32)i * 42.0f;
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        GXColor4u8(148, 154, 140, 255);
        GXPosition3f32(x - 0.65f, -1.8f, -105.0f);
        GXColor4u8(148, 154, 140, 255);
        GXPosition3f32(x + 0.65f, -1.8f, -105.0f);
        GXColor4u8(118, 126, 118, 255);
        GXPosition3f32(x + 0.65f, -1.8f, 105.0f);
        GXColor4u8(118, 126, 118, 255);
        GXPosition3f32(x - 0.65f, -1.8f, 105.0f);
        GXEnd();
    }
    for (i = -2; i <= 2; ++i) {
        f32 z = (f32)i * 42.0f;
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        GXColor4u8(136, 144, 132, 255);
        GXPosition3f32(-170.0f, -1.7f, z - 0.65f);
        GXColor4u8(136, 144, 132, 255);
        GXPosition3f32(170.0f, -1.7f, z - 0.65f);
        GXColor4u8(136, 144, 132, 255);
        GXPosition3f32(170.0f, -1.7f, z + 0.65f);
        GXColor4u8(136, 144, 132, 255);
        GXPosition3f32(-170.0f, -1.7f, z + 0.65f);
        GXEnd();
    }
}

static void PCPort_BattleApplyGridPlacement(PCPortBattleRenderActor actors[4]) {
    /* Mirrors src/game/battle/battle_grid.c:fn_801C27F4's double-battle
     * diamond: slot X = +/-3, slot Z = +/-5. Host scale expands that grid to the
     * current PKX render units until the real battleGrid state is wired through. */
    static const f32 kGridX[4] = { -3.0f, 3.0f, -3.0f, 3.0f };
    static const f32 kGridZ[4] = { -5.0f, -5.0f, 5.0f, 5.0f };
    f32 unit = 5.0f;  /* tighter default so the +/-Z rows both frame above the UI */
    f32 yawP = 3.14159f, yawE = 0.0f;
    int i;
    /* Live tuning while we dial in battle placement:
     *   PCPORT_BATTLE_UNIT=<f>   grid spacing multiplier (default 10)
     *   PCPORT_BATTLE_YAW=<p,e>  player/enemy facing radians (default pi,0) */
    { const char* u = getenv("PCPORT_BATTLE_UNIT");
      if (u != NULL && u[0]) { f32 v = (f32)atof(u); if (v > 0.0f) unit = v; } }
    { const char* y = getenv("PCPORT_BATTLE_YAW");
      if (y != NULL && y[0]) { float p, e; if (sscanf(y, "%f,%f", &p, &e) == 2) { yawP = p; yawE = e; } } }

    for (i = 0; i < 4; ++i) {
        actors[i].x = kGridX[i] * unit;
        actors[i].y = 0.0f;
        actors[i].z = kGridZ[i] * unit;
        actors[i].yaw = (i < 2) ? yawP : yawE;
    }
    printf("[battle-grid] source=src/game/battle/battle_grid.c:fn_801C27F4 "
           "center=(0,0) unit=%.1f slots={P0(%.1f,%.1f),P1(%.1f,%.1f),"
           "E0(%.1f,%.1f),E1(%.1f,%.1f)}\n",
           unit,
           actors[0].x, actors[0].z, actors[1].x, actors[1].z,
           actors[2].x, actors[2].z, actors[3].x, actors[3].z);
}

typedef enum PCPortBattleFlowState {
    PCPORT_BATTLE_FLOW_COMMAND_MENU = 0,
    PCPORT_BATTLE_FLOW_MOVE_MENU,
    PCPORT_BATTLE_FLOW_PLAYER_ATTACK,
    PCPORT_BATTLE_FLOW_ENEMY_DAMAGE,
    PCPORT_BATTLE_FLOW_ENEMY_ATTACK,
    PCPORT_BATTLE_FLOW_PLAYER_DAMAGE,
    PCPORT_BATTLE_FLOW_END_TURN
} PCPortBattleFlowState;

typedef struct PCPortBattleInputState {
    int left;
    int right;
    int up;
    int down;
    int a;
    int b;
} PCPortBattleInputState;

typedef struct PCPortBattleFlow {
    PCPortBattleFlowState state;
    int stateFrame;
    int turn;
    int commandIndex;
    int moveIndex;
    int playerHP;
    int enemyHP;
    int autoplay;
    int requestPokemonSetup;
    u16 playerMoveId;
    u16 enemyMoveId;
    u16 playerTextId;
    u16 enemyTextId;
    int playerDamage;
    int enemyDamage;
    char moveNames[2][32];
    char commandText[96];
    char messageText[128];
    char statusText[96];
    PCPortBattleInputState prevInput;
} PCPortBattleFlow;

static const char* kPcportBattleCommandNames[4] = {
    "FIGHT", "POKEMON", "BAG", "RUN"
};

static void DrawBattleUI(GXTexObj* messageTex,
                         int haveMessageTex,
                         GXTexObj* commandTex,
                         int haveCommandTex,
                         const PCPortBattleFlow* battleFlow) {
    /* Clean game-style battle HUD (640x480 ortho):
     *   - left:  message box (battle text) + HP line
     *   - right: 2x2 command grid FIGHT / POKeMON / BAG / RUN, selection lit
     * Replaces the old crammed layout where message/command/status text overlapped.
     * (void)commandTex/messageTex: we draw the chrome procedurally so the text aligns
     * to the boxes exactly regardless of the loaded art. */
    const f32 BOXY = 388.0f, BOXH = 84.0f;
    int i;
    (void)messageTex; (void)haveMessageTex; (void)commandTex; (void)haveCommandTex;
    BeginMenuOverlay();

    /* --- message box (bottom-left) --- */
    DrawSolidScreenRect(10.0f, BOXY, 388.0f, BOXH, 16, 32, 64, 240);
    DrawSolidScreenRect(10.0f, BOXY, 388.0f, 3.0f, 96, 160, 232, 255);     /* top accent */
    DrawSolidScreenRect(10.0f, BOXY + BOXH - 3.0f, 388.0f, 3.0f, 40, 80, 140, 255);
    DrawTextScreen(26.0f, BOXY + 16.0f, 10.0f, 16.0f, 248, 250, 252, 255,
                   battleFlow->messageText);
    DrawTextScreen(26.0f, BOXY + 52.0f, 8.0f, 13.0f, 170, 224, 180, 255,
                   battleFlow->statusText);

    /* --- command / move grid (bottom-right) --- */
    DrawSolidScreenRect(406.0f, BOXY, 224.0f, BOXH, 232, 238, 244, 240);
    DrawSolidScreenRect(406.0f, BOXY, 224.0f, 3.0f, 200, 210, 220, 255);
    {
        const f32 cellX[4] = { 414.0f, 522.0f, 414.0f, 522.0f };
        const f32 cellY[4] = { BOXY + 8.0f,  BOXY + 8.0f,
                               BOXY + 44.0f, BOXY + 44.0f };
        const char* names[4];
        int itemCount = 4;
        int selected = battleFlow->commandIndex;
        if (battleFlow->state == PCPORT_BATTLE_FLOW_MOVE_MENU) {
            names[0] = battleFlow->moveNames[0];
            names[1] = battleFlow->moveNames[1];
            names[2] = "-";
            names[3] = "-";
            itemCount = 2;
            selected = battleFlow->moveIndex;
        } else {
            names[0] = kPcportBattleCommandNames[0];
            names[1] = kPcportBattleCommandNames[1];
            names[2] = kPcportBattleCommandNames[2];
            names[3] = kPcportBattleCommandNames[3];
        }
        for (i = 0; i < 4; ++i) {
            int active = i < itemCount;
            int sel = active && (selected == i);
            f32 cx = cellX[i], cy = cellY[i];
            if (sel) {
                DrawSolidScreenRect(cx - 6.0f, cy - 4.0f, 104.0f, 28.0f,
                                    64, 132, 214, 255);     /* selection highlight */
            }
            DrawTextScreen(cx, cy, 9.0f, 16.0f,
                           sel ? 255 : (active ? 36 : 150),
                           sel ? 255 : (active ? 48 : 158),
                           sel ? 255 : (active ? 66 : 166),
                           active ? 255 : 140,
                           names[i]);
        }
    }
}

static const char* PCPort_BattleIconPathForMember(const char* member) {
    if (member == NULL) {
        return NULL;
    }
    if (strcmp(member, "zangoose") == 0) {
        return "sprites/pokemon_icons/sprite_0335.png";
    }
    if (strcmp(member, "gokulin") == 0) {
        return "sprites/pokemon_icons/sprite_0316.png";
    }
    if (strcmp(member, "nukenin") == 0) {
        return "sprites/pokemon_icons/sprite_0292.png";
    }
    return NULL;
}

static void DrawBattleActorIconFallback(PCPortBattleRenderActor actors[4],
                                        GXTexObj iconTex[4],
                                        int haveIcon[4],
                                        int frame) {
    static const f32 kIconX[4] = { 132.0f, 238.0f, 390.0f, 496.0f };
    static const f32 kIconY[4] = { 230.0f, 246.0f, 150.0f, 134.0f };
    int i;

    BeginMenuOverlay();
    for (i = 0; i < 4; ++i) {
        f32 bob;
        f32 x;
        f32 y;
        if (!haveIcon[i]) {
            continue;
        }
        bob = sinf(((f32)frame * 0.08f) + (f32)i) * 3.0f;
        x = kIconX[i];
        y = kIconY[i] + bob;
        DrawTexturedScreenRect(&iconTex[i], x, y, 72.0f, 72.0f,
                               0.0f, 0.0f, 1.0f, 1.0f);
        DrawTextScreen(x - 4.0f, y + 72.0f, 7.0f, 11.0f,
                       236, 242, 248, 230, actors[i].displayName);
    }
}

static const char* PCPort_BattleFlowStateName(PCPortBattleFlowState state) {
    switch (state) {
    case PCPORT_BATTLE_FLOW_COMMAND_MENU: return "command-menu";
    case PCPORT_BATTLE_FLOW_MOVE_MENU: return "move-menu";
    case PCPORT_BATTLE_FLOW_PLAYER_ATTACK: return "player-attack";
    case PCPORT_BATTLE_FLOW_ENEMY_DAMAGE: return "enemy-damage";
    case PCPORT_BATTLE_FLOW_ENEMY_ATTACK: return "enemy-attack";
    case PCPORT_BATTLE_FLOW_PLAYER_DAMAGE: return "player-damage";
    case PCPORT_BATTLE_FLOW_END_TURN: return "end-turn";
    default: return "unknown";
    }
}

static int PCPort_BattleKeyDown(GLFWwindow* window, int key) {
    return window != NULL && glfwGetKey(window, key) == GLFW_PRESS;
}

static void PCPort_BattlePollInput(GLFWwindow* window,
                                   PCPortBattleFlow* flow,
                                   PCPortBattleInputState* pressed) {
    PCPortBattleInputState now;

    memset(&now, 0, sizeof(now));
    memset(pressed, 0, sizeof(*pressed));
    now.left = PCPort_BattleKeyDown(window, GLFW_KEY_LEFT);
    now.right = PCPort_BattleKeyDown(window, GLFW_KEY_RIGHT);
    now.up = PCPort_BattleKeyDown(window, GLFW_KEY_UP);
    now.down = PCPort_BattleKeyDown(window, GLFW_KEY_DOWN);
    now.a = PCPort_BattleKeyDown(window, GLFW_KEY_Z) ||
            PCPort_BattleKeyDown(window, GLFW_KEY_ENTER);
    now.b = PCPort_BattleKeyDown(window, GLFW_KEY_X) ||
            PCPort_BattleKeyDown(window, GLFW_KEY_BACKSPACE);

    pressed->left = now.left && !flow->prevInput.left;
    pressed->right = now.right && !flow->prevInput.right;
    pressed->up = now.up && !flow->prevInput.up;
    pressed->down = now.down && !flow->prevInput.down;
    pressed->a = now.a && !flow->prevInput.a;
    pressed->b = now.b && !flow->prevInput.b;
    flow->prevInput = now;
}

static void PCPort_BattleFlowRefreshText(PCPortBattleFlow* flow) {
    if (flow->state == PCPORT_BATTLE_FLOW_COMMAND_MENU) {
        snprintf(flow->commandText, sizeof(flow->commandText),
                 "%s%s%s  %s%s%s  %s%s%s  %s%s%s",
                 flow->commandIndex == 0 ? "[" : "", kPcportBattleCommandNames[0],
                 flow->commandIndex == 0 ? "]" : "",
                 flow->commandIndex == 1 ? "[" : "", kPcportBattleCommandNames[1],
                 flow->commandIndex == 1 ? "]" : "",
                 flow->commandIndex == 2 ? "[" : "", kPcportBattleCommandNames[2],
                 flow->commandIndex == 2 ? "]" : "",
                 flow->commandIndex == 3 ? "[" : "", kPcportBattleCommandNames[3],
                 flow->commandIndex == 3 ? "]" : "");
    } else if (flow->state == PCPORT_BATTLE_FLOW_MOVE_MENU) {
        snprintf(flow->commandText, sizeof(flow->commandText),
                 "%s%s%s  %s%s%s",
                 flow->moveIndex == 0 ? "[" : "", flow->moveNames[0],
                 flow->moveIndex == 0 ? "]" : "",
                 flow->moveIndex == 1 ? "[" : "", flow->moveNames[1],
                 flow->moveIndex == 1 ? "]" : "");
    } else {
        snprintf(flow->commandText, sizeof(flow->commandText),
                 "TURN %d  PLAYER MOVE %u  ENEMY MOVE %u", flow->turn,
                 flow->playerMoveId, flow->enemyMoveId);
    }
    snprintf(flow->statusText, sizeof(flow->statusText),
             "Player HP %-3d   Enemy HP %-3d", flow->playerHP, flow->enemyHP);
}

static void PCPort_BattleFlowEnter(PCPortBattleFlow* flow,
                                   PCPortBattleFlowState state,
                                   int frame,
                                   PCPortBattleRenderActor actors[4],
                                   const char* reason) {
    const char* playerName = (actors != NULL && actors[0].member != NULL)
                                 ? actors[0].displayName : "Pokemon";
    const char* enemyName = (actors != NULL && actors[2].member != NULL)
                               ? actors[2].displayName : "opponent";
    flow->state = state;
    flow->stateFrame = 0;

    switch (state) {
    case PCPORT_BATTLE_FLOW_COMMAND_MENU:
        flow->commandIndex = 0;
        snprintf(flow->messageText, sizeof(flow->messageText),
                 "Choose a command.");
        if (actors != NULL) {
            PCPort_BattleActorSetMotion(&actors[0], actors[0].stanceMotion,
                                        frame, "command-menu");
            PCPort_BattleActorSetMotion(&actors[2], actors[2].stanceMotion,
                                        frame, "command-menu");
        }
        printf("[battle-flow] frame=%d state=command-menu selected=FIGHT "
               "text=\"FIGHT  POKEMON  BAG  RUN\" reason=%s\n",
               frame, reason != NULL ? reason : "-");
        break;
    case PCPORT_BATTLE_FLOW_MOVE_MENU:
        flow->moveIndex = 0;
        snprintf(flow->messageText, sizeof(flow->messageText),
                 "Select a move.");
        printf("[battle-flow] frame=%d state=move-menu selected=\"%s\" "
               "moveId=%u textId=0x%04X target=enemy-left reason=%s\n",
               frame, flow->moveNames[0], flow->playerMoveId,
               flow->playerTextId, reason != NULL ? reason : "-");
        break;
    case PCPORT_BATTLE_FLOW_PLAYER_ATTACK:
        snprintf(flow->messageText, sizeof(flow->messageText),
                 "%s used %s!", playerName, flow->moveNames[0]);
        if (actors != NULL) {
            PCPort_BattleActorSetMotion(&actors[0], actors[0].attackMotion,
                                        frame, "player-attack");
        }
        printf("[battle-flow] frame=%d state=player-attack actor=player-left "
               "moveId=%u textId=0x%04X damage=%d reason=%s\n",
               frame, flow->playerMoveId, flow->playerTextId,
               flow->playerDamage, reason != NULL ? reason : "-");
        break;
    case PCPORT_BATTLE_FLOW_ENEMY_DAMAGE:
        flow->enemyHP -= flow->playerDamage;
        if (flow->enemyHP < 0) flow->enemyHP = 0;
        snprintf(flow->messageText, sizeof(flow->messageText),
                 "The opposing %s took damage.", enemyName);
        if (actors != NULL) {
            PCPort_BattleActorSetMotion(&actors[2], actors[2].damageMotion,
                                        frame, "enemy-damage");
        }
        printf("[battle-flow] frame=%d state=enemy-damage target=enemy-left "
               "enemyHP=%d reason=%s\n",
               frame, flow->enemyHP, reason != NULL ? reason : "-");
        break;
    case PCPORT_BATTLE_FLOW_ENEMY_ATTACK:
        snprintf(flow->messageText, sizeof(flow->messageText),
                 "%s used %s!", enemyName, flow->moveNames[1]);
        if (actors != NULL) {
            PCPort_BattleActorSetMotion(&actors[2], actors[2].attackMotion,
                                        frame, "enemy-attack");
        }
        printf("[battle-flow] frame=%d state=enemy-attack actor=enemy-left "
               "moveId=%u textId=0x%04X damage=%d reason=%s\n",
               frame, flow->enemyMoveId, flow->enemyTextId,
               flow->enemyDamage, reason != NULL ? reason : "-");
        break;
    case PCPORT_BATTLE_FLOW_PLAYER_DAMAGE:
        flow->playerHP -= flow->enemyDamage;
        if (flow->playerHP < 0) flow->playerHP = 0;
        snprintf(flow->messageText, sizeof(flow->messageText),
                 "%s took damage.", playerName);
        if (actors != NULL) {
            PCPort_BattleActorSetMotion(&actors[0], actors[0].damageMotion,
                                        frame, "player-damage");
        }
        printf("[battle-flow] frame=%d state=player-damage target=player-left "
               "playerHP=%d reason=%s\n",
               frame, flow->playerHP, reason != NULL ? reason : "-");
        break;
    case PCPORT_BATTLE_FLOW_END_TURN:
        snprintf(flow->messageText, sizeof(flow->messageText),
                 "Next: command menu.");
        if (actors != NULL) {
            PCPort_BattleActorSetMotion(&actors[0], actors[0].stanceMotion,
                                        frame, "end-turn");
            PCPort_BattleActorSetMotion(&actors[2], actors[2].stanceMotion,
                                        frame, "end-turn");
        }
        printf("[battle-flow] frame=%d state=end-turn playerHP=%d enemyHP=%d "
               "next=command-menu reason=%s\n",
               frame, flow->playerHP, flow->enemyHP,
               reason != NULL ? reason : "-");
        flow->turn++;
        break;
    }
    PCPort_BattleFlowRefreshText(flow);
    fflush(stdout);
}

static void PCPort_BattleFlowInit(PCPortBattleFlow* flow,
                                  PCPortBattleRenderActor actors[4],
                                  const PCPortBattleTableSetup* setup) {
    memset(flow, 0, sizeof(*flow));
    flow->playerHP = 100;
    flow->enemyHP = 100;
    flow->turn = 1;
    flow->playerMoveId = setup != NULL ? setup->playerMoveId : 129u;
    flow->enemyMoveId = setup != NULL ? setup->enemyMoveId : 44u;
    flow->playerTextId = setup != NULL ? setup->playerTextId : 0x8001u;
    flow->enemyTextId = setup != NULL ? setup->enemyTextId : 0x8002u;
    flow->playerDamage = setup != NULL ? setup->playerDamage : 32;
    flow->enemyDamage = setup != NULL ? setup->enemyDamage : 21;
    snprintf(flow->moveNames[0], sizeof(flow->moveNames[0]), "%s",
             setup != NULL ? setup->playerMoveName : "Swift");
    snprintf(flow->moveNames[1], sizeof(flow->moveNames[1]), "%s",
             setup != NULL ? setup->enemyMoveName : "Bite");
    flow->autoplay = getenv("PCPORT_BATTLE_AUTOPLAY") == NULL ||
                     strcmp(getenv("PCPORT_BATTLE_AUTOPLAY"), "0") != 0;
    printf("[battle-flow] source=common_rel playerTrainer=0x%04X "
           "enemyTrainer=0x%04X playerMove=%u enemyMove=%u "
           "playerDamage=%d enemyDamage=%d autoplay=%d\n",
           setup != NULL ? setup->playerTrainerId : 0x0001u,
           setup != NULL ? setup->enemyTrainerId : 0x0200u,
           flow->playerMoveId, flow->enemyMoveId,
           flow->playerDamage, flow->enemyDamage, flow->autoplay);
    printf("[battle-flow] input keyboard=arrows+Z/Enter+B/X "
           "states=command-menu,move-menu,attack,damage,end-turn\n");
    PCPort_BattleFlowEnter(flow, PCPORT_BATTLE_FLOW_COMMAND_MENU, 0,
                           actors, "opening");
}

static void PCPort_BattleFlowHandleInput(PCPortBattleFlow* flow,
                                         PCPortBattleInputState* pressed,
                                         int frame,
                                         PCPortBattleRenderActor actors[4],
                                         const char* source) {
    if (pressed->left || pressed->right) {
        if (flow->state == PCPORT_BATTLE_FLOW_COMMAND_MENU) {
            int delta = pressed->left ? -1 : 1;
            flow->commandIndex = (flow->commandIndex + delta + 4) % 4;
            snprintf(flow->messageText, sizeof(flow->messageText),
                     "Choose a command.");
            printf("[battle-input] frame=%d source=%s action=%s state=%s "
                   "selected=%s\n", frame, source,
                   pressed->left ? "LEFT" : "RIGHT",
                   PCPort_BattleFlowStateName(flow->state),
                   kPcportBattleCommandNames[flow->commandIndex]);
        }
    }
    if (pressed->up || pressed->down) {
        if (flow->state == PCPORT_BATTLE_FLOW_MOVE_MENU) {
            flow->moveIndex = (flow->moveIndex + 1) % 2;
            snprintf(flow->messageText, sizeof(flow->messageText),
                     "Select a move.");
            printf("[battle-input] frame=%d source=%s action=%s state=%s "
                   "selected=\"%s\"\n", frame, source,
                   pressed->up ? "UP" : "DOWN",
                   PCPort_BattleFlowStateName(flow->state),
                   flow->moveNames[flow->moveIndex]);
        }
    }
    if (pressed->b && flow->state == PCPORT_BATTLE_FLOW_MOVE_MENU) {
        printf("[battle-input] frame=%d source=%s action=B state=move-menu "
               "next=command-menu\n", frame, source);
        PCPort_BattleFlowEnter(flow, PCPORT_BATTLE_FLOW_COMMAND_MENU, frame,
                               actors, "input-back");
        return;
    }
    if (pressed->a) {
        if (flow->state == PCPORT_BATTLE_FLOW_COMMAND_MENU) {
            printf("[battle-input] frame=%d source=%s action=A state=command-menu "
                   "selected=%s\n", frame, source,
                   kPcportBattleCommandNames[flow->commandIndex]);
            if (flow->commandIndex == 0) {
                PCPort_BattleFlowEnter(flow, PCPORT_BATTLE_FLOW_MOVE_MENU,
                                       frame, actors, "input-fight");
            } else if (flow->commandIndex == 1) {
                const char* p0 = (actors != NULL && actors[0].member != NULL)
                                     ? actors[0].member : "Pokemon";
                const char* p1 = (actors != NULL && actors[1].member != NULL)
                                     ? actors[1].member : "Pokemon";
                const char* p2 = (actors != NULL && actors[2].member != NULL)
                                     ? actors[2].member : "Pokemon";
                const char* p3 = (actors != NULL && actors[3].member != NULL)
                                     ? actors[3].member : "Pokemon";
                flow->requestPokemonSetup = 1;
                snprintf(flow->messageText, sizeof(flow->messageText),
                         "Choose Pokemon from your party.");
                printf("[battle-input] frame=%d source=%s action=A state=command-menu "
                       "selected=POKEMON partySlots=6 activeSlots=2 "
                       "party=%s,%s,%s,%s,slot5,slot6 next=colosseum-setup\n",
                       frame, source, p0, p1, p2, p3);
            } else if (flow->commandIndex == 2) {
                snprintf(flow->messageText, sizeof(flow->messageText),
                         "Items cannot be used in this Colosseum battle.");
                printf("[battle-input] frame=%d source=%s action=A state=command-menu "
                       "selected=BAG policy=colosseum-no-items\n",
                       frame, source);
            } else if (flow->commandIndex == 3) {
                snprintf(flow->messageText, sizeof(flow->messageText),
                         "No running from a Colosseum battle.");
                printf("[battle-input] frame=%d source=%s action=A state=command-menu "
                       "selected=RUN policy=colosseum-no-run\n",
                       frame, source);
            } else {
                snprintf(flow->messageText, sizeof(flow->messageText),
                         "%s is not wired in this host slice.",
                         kPcportBattleCommandNames[flow->commandIndex]);
            }
            return;
        }
        if (flow->state == PCPORT_BATTLE_FLOW_MOVE_MENU) {
            printf("[battle-input] frame=%d source=%s action=A state=move-menu "
                   "selected=\"%s\" moveId=%u\n", frame, source,
                   flow->moveNames[flow->moveIndex],
                   flow->moveIndex == 0 ? flow->playerMoveId :
                                          flow->enemyMoveId);
            PCPort_BattleFlowEnter(flow, PCPORT_BATTLE_FLOW_PLAYER_ATTACK,
                                   frame, actors, "input-move");
            return;
        }
    }
    PCPort_BattleFlowRefreshText(flow);
}

static void PCPort_BattleFlowUpdate(PCPortBattleFlow* flow,
                                    GLFWwindow* window,
                                    int frame,
                                    PCPortBattleRenderActor actors[4]) {
    PCPortBattleInputState pressed;

    PCPort_BattlePollInput(window, flow, &pressed);
    if (flow->autoplay && flow->turn == 1) {
        if (flow->state == PCPORT_BATTLE_FLOW_COMMAND_MENU &&
            flow->stateFrame == 40) {
            pressed.a = 1;
            PCPort_BattleFlowHandleInput(flow, &pressed, frame, actors, "auto");
        } else if (flow->state == PCPORT_BATTLE_FLOW_MOVE_MENU &&
                   flow->stateFrame == 35) {
            pressed.a = 1;
            PCPort_BattleFlowHandleInput(flow, &pressed, frame, actors, "auto");
        } else {
            PCPort_BattleFlowHandleInput(flow, &pressed, frame, actors,
                                         "keyboard");
        }
    } else {
        PCPort_BattleFlowHandleInput(flow, &pressed, frame, actors,
                                     "keyboard");
    }

    if (flow->state == PCPORT_BATTLE_FLOW_PLAYER_ATTACK &&
        flow->stateFrame >= 100) {
        PCPort_BattleFlowEnter(flow, PCPORT_BATTLE_FLOW_ENEMY_DAMAGE,
                               frame, actors, "turn-timer");
    } else if (flow->state == PCPORT_BATTLE_FLOW_ENEMY_DAMAGE &&
               flow->stateFrame >= 60) {
        PCPort_BattleFlowEnter(flow, PCPORT_BATTLE_FLOW_ENEMY_ATTACK,
                               frame, actors, "turn-timer");
    } else if (flow->state == PCPORT_BATTLE_FLOW_ENEMY_ATTACK &&
               flow->stateFrame >= 120) {
        PCPort_BattleFlowEnter(flow, PCPORT_BATTLE_FLOW_PLAYER_DAMAGE,
                               frame, actors, "turn-timer");
    } else if (flow->state == PCPORT_BATTLE_FLOW_PLAYER_DAMAGE &&
               flow->stateFrame >= 80) {
        PCPort_BattleFlowEnter(flow, PCPORT_BATTLE_FLOW_END_TURN,
                               frame, actors, "turn-timer");
    } else if (flow->state == PCPORT_BATTLE_FLOW_END_TURN &&
               flow->stateFrame >= 80) {
        PCPort_BattleFlowEnter(flow, PCPORT_BATTLE_FLOW_COMMAND_MENU,
                               frame, actors, "next-turn");
    }

    flow->stateFrame++;
    PCPort_BattleFlowRefreshText(flow);
}

static int PCPort_PkxViewerPressed(GLFWwindow* window,
                                   int key,
                                   int* prev) {
    int now;
    if (window == NULL || prev == NULL) {
        return 0;
    }
    now = glfwGetKey(window, key) == GLFW_PRESS;
    if (now && !*prev) {
        *prev = now;
        return 1;
    }
    *prev = now;
    return 0;
}

static int PCPort_PkxViewerReloadSelected(PCPortPkxViewerState* viewer,
                                          PCPortBattleRenderActor actors[4],
                                          int slot,
                                          int frame,
                                          const char* reason) {
    const char* member;
    u32 motionIdx;
    if (viewer == NULL || actors == NULL || slot < 0 || slot >= 4 ||
        viewer->modelCount <= 0) {
        return 0;
    }
    if (viewer->selectedModel[slot] < 0 ||
        viewer->selectedModel[slot] >= viewer->modelCount) {
        viewer->selectedModel[slot] = 0;
    }
    member = viewer->modelNames[viewer->selectedModel[slot]];
    motionIdx = (u32)((viewer->selectedMotion[slot] < 0) ? 0 :
                      viewer->selectedMotion[slot]);
    if (!PCPort_ReloadBattleRenderActor(&actors[slot], member, motionIdx,
                                        frame, reason)) {
        return 0;
    }
    if (!actors[slot].motionLoaded || actors[slot].motionBank.motionCount == 0u) {
        viewer->selectedMotion[slot] = 0;
    } else if ((u32)viewer->selectedMotion[slot] >=
               actors[slot].motionBank.motionCount) {
        viewer->selectedMotion[slot] = 0;
        PCPort_BattleActorSetMotion(&actors[slot], 0u, frame, "viewer-clamp");
    }
    printf("[pkx-viewer] frame=%d slot=%d member=%s motion=%d/%u reason=%s\n",
           frame, slot, actors[slot].member, viewer->selectedMotion[slot],
           actors[slot].motionLoaded ? actors[slot].motionBank.motionCount : 0u,
           reason != NULL ? reason : "-");
    fflush(stdout);
    return 1;
}

static void PCPort_PkxViewerInit(PCPortPkxViewerState* viewer,
                                 PCPortBattleRenderActor actors[4]) {
    int i;
    const char* scaleEnv;
    if (viewer == NULL || actors == NULL) {
        return;
    }
    memset(viewer, 0, sizeof(*viewer));
    viewer->enabled = 1;
    viewer->baseScale = actors[0].scale > 0.0f ? actors[0].scale : 1.8f;
    scaleEnv = getenv("PCPORT_PKX_VIEWER_SCALE");
    if (scaleEnv != NULL && scaleEnv[0] != '\0') {
        f32 v = (f32)atof(scaleEnv);
        if (v > 0.0f) {
            viewer->baseScale = v;
        }
    }
    PCPort_PkxViewerLoadModelList(viewer);
    if (getenv("PCPORT_PKX_VIEWER_SWEEP") != NULL) {
        const char* stepEnv = getenv("PCPORT_PKX_VIEWER_SWEEP_STEP");
        const char* slotEnv = getenv("PCPORT_PKX_VIEWER_SWEEP_SLOT");
        viewer->sweepEnabled = 1;
        viewer->sweepSlot = slotEnv != NULL ? atoi(slotEnv) : 0;
        if (viewer->sweepSlot < 0 || viewer->sweepSlot >= 4) {
            viewer->sweepSlot = 0;
        }
        viewer->sweepFrameStep = stepEnv != NULL ? atoi(stepEnv) : 2;
        if (viewer->sweepFrameStep <= 0) {
            viewer->sweepFrameStep = 1;
        }
        viewer->selectedSlot = viewer->sweepSlot;
        printf("[pkx-sweep] enabled slot=%d step=%d models=%d\n",
               viewer->sweepSlot, viewer->sweepFrameStep,
               viewer->modelCount);
        fflush(stdout);
    }
    for (i = 0; i < 4; ++i) {
        char scaleSlotEnvName[32];
        char motionSlotEnvName[32];
        const char* scaleSlotEnv;
        const char* motionSlotEnv;
        viewer->selectedModel[i] =
            PCPort_PkxViewerFindModel(viewer, actors[i].member);
        viewer->selectedMotion[i] = 0;
        viewer->inspectYaw[i] = 0.0f;
        actors[i].scale = viewer->baseScale;
        snprintf(scaleSlotEnvName, sizeof(scaleSlotEnvName),
                 "PCPORT_PKX_VIEWER_SCALE%d", i);
        scaleSlotEnv = getenv(scaleSlotEnvName);
        if (scaleSlotEnv != NULL && scaleSlotEnv[0] != '\0') {
            f32 v = (f32)atof(scaleSlotEnv);
            if (v > 0.0f) {
                actors[i].scale = v;
            }
        }
        snprintf(motionSlotEnvName, sizeof(motionSlotEnvName),
                 "PCPORT_PKX_VIEWER_MOTION%d", i);
        motionSlotEnv = getenv(motionSlotEnvName);
        if (motionSlotEnv != NULL && motionSlotEnv[0] != '\0') {
            int v = atoi(motionSlotEnv);
            if (v >= 0) {
                viewer->selectedMotion[i] = v;
            }
        }
        if (viewer->modelCount > 0) {
            actors[i].member = viewer->modelNames[viewer->selectedModel[i]];
            actors[i].displayName = actors[i].member;
        }
    }
    printf("[pkx-viewer] models=%d source=build_pc/pkx_model_audit.csv "
           "keys=1-4/TAB slot LEFT-RIGHT Pokemon UP-DOWN motion A-D rotate "
           "Q-E scale R autospin ENTER/Z start-battle\n", viewer->modelCount);
    fflush(stdout);
}

static void PCPort_PkxViewerUpdate(PCPortPkxViewerState* viewer,
                                   GLFWwindow* window,
                                   PCPortBattleRenderActor actors[4],
                                   int frame) {
    int slot;
    if (viewer == NULL || !viewer->enabled || actors == NULL) {
        return;
    }
    slot = viewer->selectedSlot;
    if (!viewer->singleDebug &&
        PCPort_PkxViewerPressed(window, GLFW_KEY_TAB, &viewer->prevTab)) {
        viewer->selectedSlot = (viewer->selectedSlot + 1) & 3;
    }
    if (!viewer->singleDebug) {
        if (PCPort_PkxViewerPressed(window, GLFW_KEY_1, &viewer->prevOne)) viewer->selectedSlot = 0;
        if (PCPort_PkxViewerPressed(window, GLFW_KEY_2, &viewer->prevTwo)) viewer->selectedSlot = 1;
        if (PCPort_PkxViewerPressed(window, GLFW_KEY_3, &viewer->prevThree)) viewer->selectedSlot = 2;
        if (PCPort_PkxViewerPressed(window, GLFW_KEY_4, &viewer->prevFour)) viewer->selectedSlot = 3;
    }
    slot = viewer->selectedSlot;

    if (viewer->modelCount > 0 &&
        (PCPort_PkxViewerPressed(window, GLFW_KEY_LEFT, &viewer->prevLeft) ||
         PCPort_PkxViewerPressed(window, GLFW_KEY_RIGHT, &viewer->prevRight))) {
        int oldModel = viewer->selectedModel[slot];
        int right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
        int delta = right ? 1 : -1;
        viewer->selectedModel[slot] =
            (viewer->selectedModel[slot] + delta + viewer->modelCount) %
            viewer->modelCount;
        viewer->selectedMotion[slot] = 0;
        if (!PCPort_PkxViewerReloadSelected(viewer, actors, slot, frame,
                                            "viewer-model-select")) {
            viewer->selectedModel[slot] = oldModel;
            viewer->selectedMotion[slot] = 0;
        }
    }
    if (actors[slot].motionLoaded && actors[slot].motionBank.motionCount > 0u &&
        (PCPort_PkxViewerPressed(window, GLFW_KEY_UP, &viewer->prevUp) ||
         PCPort_PkxViewerPressed(window, GLFW_KEY_DOWN, &viewer->prevDown))) {
        int up = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
        int count = (int)actors[slot].motionBank.motionCount;
        int delta = up ? 1 : -1;
        viewer->selectedMotion[slot] =
            (viewer->selectedMotion[slot] + delta + count) % count;
        PCPort_BattleActorSetMotion(&actors[slot],
                                    (u32)viewer->selectedMotion[slot],
                                    frame, "viewer-motion-select");
        printf("[pkx-viewer] frame=%d slot=%d member=%s motion=%d/%d\n",
               frame, slot, actors[slot].member,
               viewer->selectedMotion[slot], count);
        fflush(stdout);
    }
    if (PCPort_PkxViewerPressed(window, GLFW_KEY_R, &viewer->prevR)) {
        viewer->autoSpin = !viewer->autoSpin;
        printf("[pkx-viewer] frame=%d autospin=%d\n", frame, viewer->autoSpin);
        fflush(stdout);
    }
    if (!viewer->singleDebug &&
        (PCPort_PkxViewerPressed(window, GLFW_KEY_ENTER, &viewer->prevEnter) ||
         PCPort_PkxViewerPressed(window, GLFW_KEY_Z, &viewer->prevZ))) {
        viewer->startBattle = 1;
        printf("[pkx-viewer] frame=%d start-battle slots=%s,%s,%s,%s\n",
               frame,
               actors[0].member != NULL ? actors[0].member : "-",
               actors[1].member != NULL ? actors[1].member : "-",
               actors[2].member != NULL ? actors[2].member : "-",
               actors[3].member != NULL ? actors[3].member : "-");
        fflush(stdout);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        actors[slot].yaw -= 0.035f;
        viewer->inspectYaw[slot] -= 0.035f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        actors[slot].yaw += 0.035f;
        viewer->inspectYaw[slot] += 0.035f;
    }
    if (PCPort_PkxViewerPressed(window, GLFW_KEY_Q, &viewer->prevQ)) {
        actors[slot].scale *= 0.90f;
        if (actors[slot].scale < 0.2f) actors[slot].scale = 0.2f;
    }
    if (PCPort_PkxViewerPressed(window, GLFW_KEY_E, &viewer->prevE)) {
        actors[slot].scale *= 1.10f;
        if (actors[slot].scale > 12.0f) actors[slot].scale = 12.0f;
    }
    if (viewer->autoSpin) {
        actors[slot].yaw += 0.020f;
        viewer->inspectYaw[slot] += 0.020f;
    }
}

static void PCPort_PkxViewerSweepUpdate(PCPortPkxViewerState* viewer,
                                        PCPortBattleRenderActor actors[4],
                                        int frame) {
    int slot;
    int oldModel;
    const char* member;
    int ok;
    if (viewer == NULL || actors == NULL || !viewer->enabled ||
        !viewer->sweepEnabled || viewer->sweepDone) {
        return;
    }
    if (viewer->modelCount <= 0) {
        viewer->sweepDone = 1;
        return;
    }
    if (viewer->sweepFrameStep <= 0) {
        viewer->sweepFrameStep = 1;
    }
    if (frame % viewer->sweepFrameStep != 0) {
        return;
    }
    if (viewer->sweepNextModel >= viewer->modelCount) {
        viewer->sweepDone = 1;
        printf("[pkx-sweep] done frame=%d slot=%d ok=%d fail=%d total=%d\n",
               frame, viewer->sweepSlot, viewer->sweepSuccesses,
               viewer->sweepFailures, viewer->modelCount);
        fflush(stdout);
        return;
    }
    slot = viewer->sweepSlot;
    if (slot < 0 || slot >= 4) {
        slot = 0;
    }
    oldModel = viewer->selectedModel[slot];
    viewer->selectedSlot = slot;
    viewer->selectedModel[slot] = viewer->sweepNextModel;
    viewer->selectedMotion[slot] = 0;
    member = viewer->modelNames[viewer->sweepNextModel];
    printf("[pkx-sweep] begin frame=%d slot=%d model=%d/%d member=%s\n",
           frame, slot, viewer->sweepNextModel + 1, viewer->modelCount,
           member != NULL ? member : "-");
    fflush(stdout);
    ok = PCPort_PkxViewerReloadSelected(viewer, actors, slot, frame,
                                        "viewer-sweep");
    if (ok) {
        viewer->sweepSuccesses++;
        printf("[pkx-sweep] ok frame=%d slot=%d model=%d/%d member=%s "
               "motions=%u\n",
               frame, slot, viewer->sweepNextModel + 1, viewer->modelCount,
               actors[slot].member != NULL ? actors[slot].member : "-",
               actors[slot].motionLoaded ?
                   actors[slot].motionBank.motionCount : 0u);
    } else {
        viewer->sweepFailures++;
        viewer->selectedModel[slot] = oldModel;
        printf("[pkx-sweep] fail frame=%d slot=%d model=%d/%d member=%s "
               "kept=%s\n",
               frame, slot, viewer->sweepNextModel + 1, viewer->modelCount,
               member != NULL ? member : "-",
               actors[slot].member != NULL ? actors[slot].member : "-");
    }
    fflush(stdout);
    viewer->sweepNextModel++;
}

static void DrawPkxViewerUI(const PCPortPkxViewerState* viewer,
                            const PCPortBattleRenderActor actors[4]) {
    int i;
    if (viewer == NULL || !viewer->enabled || actors == NULL) {
        return;
    }
    BeginMenuOverlay();
    DrawSolidScreenRect(0.0f, 0.0f, 640.0f, 76.0f, 12, 20, 32, 225);
    DrawSolidScreenRect(0.0f, 384.0f, 640.0f, 96.0f, 10, 18, 30, 235);
    if (viewer->singleDebug) {
        char line[192];
        u32 motionCount = actors[0].motionLoaded ?
            actors[0].motionBank.motionCount : 0u;
        DrawTextScreen(14.0f, 10.0f, 7.0f, 12.0f, 230, 238, 246, 255,
                       "DEBUG MENU   Pokemon Models");
        snprintf(line, sizeof(line),
                 "Pokemon Models  model %d/%d %s  motion %d/%u  yaw %.2f scale %.2f  all-PObj visible",
                 viewer->selectedModel[0] + 1, viewer->modelCount,
                 actors[0].member != NULL ? actors[0].member : "-",
                 viewer->selectedMotion[0], motionCount,
                 actors[0].yaw, actors[0].scale);
        DrawTextScreen(14.0f, 396.0f, 7.0f, 12.0f,
                       255, 244, 120, 255, line);
        DrawTextScreen(14.0f, 468.0f, 6.0f, 10.0f, 160, 188, 210, 255,
                       "Single-model inspector; battle parties remain six Pokemon with two active in Double Battle.");
        return;
    }
    DrawTextScreen(14.0f, 10.0f, 7.0f, 12.0f, 230, 238, 246, 255,
                   "BATTLE COLOSSEUM SETUP   1-4/TAB slot   LEFT/RIGHT Pokemon   UP/DOWN motion   A/D rotate   Q/E scale   ENTER start");
    for (i = 0; i < 4; ++i) {
        char line[192];
        int selected = (i == viewer->selectedSlot);
        u32 motionCount = actors[i].motionLoaded ? actors[i].motionBank.motionCount : 0u;
        snprintf(line, sizeof(line),
                 "%s%d %s  model %d/%d %s  motion %d/%u  yaw %.2f scale %.2f%s",
                 selected ? ">" : " ", i + 1,
                 actors[i].label != NULL ? actors[i].label : "-",
                 viewer->selectedModel[i] + 1, viewer->modelCount,
                 actors[i].member != NULL ? actors[i].member : "-",
                 viewer->selectedMotion[i], motionCount,
                 actors[i].yaw, actors[i].scale,
                 selected ? "  SELECTED" : "");
        DrawTextScreen(14.0f, 396.0f + (f32)i * 18.0f,
                       7.0f, 12.0f,
                       selected ? 255 : 190, selected ? 244 : 206,
                       selected ? 120 : 218, 255, line);
    }
    DrawTextScreen(14.0f, 468.0f, 6.0f, 10.0f, 160, 188, 210, 255,
                   "Selections carry into the battle scene; commands there use FIGHT / POKEMON / BAG / RUN.");
}

static int RunBattleScene(GLFWwindow* window, int viewerMode) {
    PCPortBattleRenderActor actors[4] = {
        { "player-left",  "zangoose", "Zangoose", 0x0001u, 0u, 335u, 100u,
          { 0u, 0u, 0u, 0u }, -48.0f, 16.0f,  30.0f, 3.14159f, 1.8f },
        { "player-right", "zangoose", "Zangoose", 0x0001u, 1u, 335u, 100u,
          { 0u, 0u, 0u, 0u },  48.0f, 16.0f,  30.0f, 3.14159f, 1.8f },
        { "enemy-left",   "gokulin",  "Gokulin",  0x0200u, 0u, 316u, 50u,
          { 0u, 0u, 0u, 0u }, -48.0f, 16.0f, -36.0f, 0.0f,    1.8f },
        { "enemy-right",  "nukenin",  "Nukenin",  0x0200u, 1u, 292u, 50u,
          { 0u, 0u, 0u, 0u },  48.0f, 16.0f, -36.0f, 0.0f,    1.8f }
    };
    PCPortTranslatedCamera camera;
    const char* scaleEnv = getenv("PCPORT_BATTLE_SCALE");
    GXTexObj battleBackdropTex;
    GXTexObj battleMessageTex;
    GXTexObj battleCommandTex;
    GXTexObj battleIconTex[4];
    int haveBattleIcon[4] = { 0, 0, 0, 0 };
    PCPortBattleFlow battleFlow;
    PCPortPkxViewerState pkxViewer;
    int haveBattleBackdrop = 0;
    int haveBattleMessageTex = 0;
    int haveBattleCommandTex = 0;
    int haveBattleStage = 0;
    int drawBattleStage = 0;
    int drawPkxMesh = 0;
    int disablePkxDepth = 0;
    int frameCap = 0;
    int frame;
    int i;
    int setupDisablesAutoplay;
    int prevDebugMenuKey = 0;
    PCPortBattleTableSetup battleSetup;

    memset(&pkxViewer, 0, sizeof(pkxViewer));
    PCPort_BattleInitSetup(&battleSetup);
    setupDisablesAutoplay =
        viewerMode && getenv("PCPORT_BATTLE_AUTOPLAY") == NULL;
    if (getenv("PCPORT_BATTLE_P0") != NULL) {
        actors[0].member = getenv("PCPORT_BATTLE_P0");
        actors[0].displayName = actors[0].member;
    }
    if (getenv("PCPORT_BATTLE_P1") != NULL) {
        actors[1].member = getenv("PCPORT_BATTLE_P1");
        actors[1].displayName = actors[1].member;
    }
    if (getenv("PCPORT_BATTLE_E0") != NULL) {
        actors[2].member = getenv("PCPORT_BATTLE_E0");
        actors[2].displayName = actors[2].member;
    }
    if (getenv("PCPORT_BATTLE_E1") != NULL) {
        actors[3].member = getenv("PCPORT_BATTLE_E1");
        actors[3].displayName = actors[3].member;
    }
    actors[0].stanceMotion = 0u; actors[0].attackMotion = 3u; actors[0].damageMotion = 0u;
    actors[1].stanceMotion = 0u; actors[1].attackMotion = 3u; actors[1].damageMotion = 0u;
    actors[2].stanceMotion = 0u; actors[2].attackMotion = 3u; actors[2].damageMotion = 0u;
    actors[3].stanceMotion = 0u; actors[3].attackMotion = 2u; actors[3].damageMotion = 0u;
    PCPort_BattleApplyCommonRelSetup(&battleSetup, actors);
    if (scaleEnv != NULL && scaleEnv[0] != '\0') {
        f32 scale = (f32)atof(scaleEnv);
        if (scale > 0.0f) {
            for (i = 0; i < 4; ++i) {
                actors[i].scale = scale;
            }
        }
    }
    if (getenv("PCPORT_DEBUG_POKEMON_MODE") != NULL) {
        viewerMode = 1;
    }
    if (viewerMode) {
        PCPort_PkxViewerInit(&pkxViewer, actors);
        if (getenv("PCPORT_DEBUG_POKEMON_MODE") != NULL) {
            pkxViewer.singleDebug = 1;
            pkxViewer.selectedSlot = 0;
            printf("[debug-ui] opened tab=pokemon-models frame=0 "
                   "partySlots=6 activeSlots=2 model=%s reason=env\n",
                   actors[0].member != NULL ? actors[0].member : "-");
        }
    }
    PCPort_BattleApplyGridPlacement(actors);
    if (viewerMode && getenv("PCPORT_BATTLE_UNIT") == NULL) {
        static const f32 kViewerX[4] = { -54.0f, -18.0f, 18.0f, 54.0f };
        static const f32 kViewerZ[4] = { 18.0f, 18.0f, 18.0f, 18.0f };
        for (i = 0; i < 4; ++i) {
            actors[i].x = kViewerX[i];
            actors[i].z = kViewerZ[i];
            actors[i].yaw = 0.0f;
        }
        if (pkxViewer.singleDebug) {
            actors[0].x = 0.0f;
            actors[0].z = 18.0f;
        }
        printf("[pkx-viewer] placement=compact-inspection slots={P0(%.1f,%.1f),P1(%.1f,%.1f),E0(%.1f,%.1f),E1(%.1f,%.1f)}\n",
               actors[0].x, actors[0].z, actors[1].x, actors[1].z,
               actors[2].x, actors[2].z, actors[3].x, actors[3].z);
    }
    if (getenv("PCPORT_BATTLE_FRAMES") != NULL) {
        frameCap = atoi(getenv("PCPORT_BATTLE_FRAMES"));
    }
    if (frameCap <= 0) {
        frameCap = 0;
    }

    memset(&camera, 0, sizeof(camera));
    camera.viewportLeft = 0;
    camera.viewportTop = 0;
    camera.viewportRight = (u16)PCPORT_WINDOW_WIDTH;
    camera.viewportBottom = (u16)PCPORT_WINDOW_HEIGHT;
    camera.scissorLeft = 0;
    camera.scissorTop = 0;
    camera.scissorRight = (u16)PCPORT_WINDOW_WIDTH;
    camera.scissorBottom = (u16)PCPORT_WINDOW_HEIGHT;
    {
        f32 fov = 45.0f * 3.14159265f / 180.0f;
        f32 aspect = (f32)PCPORT_WINDOW_WIDTH / (f32)PCPORT_WINDOW_HEIGHT;
        f32 nearZ = 20.0f;
        f32 farZ = 500.0f;
        f32 f = 1.0f / tanf(fov * 0.5f);
        memset(camera.projectionMatrix, 0, sizeof(camera.projectionMatrix));
        camera.projectionMatrix[0][0] = f / aspect;
        camera.projectionMatrix[1][1] = f;
        camera.projectionMatrix[2][2] = -(farZ + nearZ) / (farZ - nearZ);
        camera.projectionMatrix[2][3] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
        camera.projectionMatrix[3][2] = -1.0f;
    }
    PCPort_SetBattleCameraView(&camera, 0);

    for (i = 0; i < 4; ++i) {
        if (!PCPort_LoadPkxRenderActor(&actors[i])) {
            PCPort_FreeBattleRenderActors(actors, 4);
            return 0;
        }
    }
    for (i = 0; i < 4; ++i) {
        u32 motionIdx = viewerMode ? (u32)pkxViewer.selectedMotion[i] :
                                     actors[i].stanceMotion;
        if (actors[i].motionLoaded &&
            motionIdx >= actors[i].motionBank.motionCount) {
            motionIdx = 0u;
            if (viewerMode) {
                pkxViewer.selectedMotion[i] = 0;
            }
        }
        PCPort_BattleActorSetMotion(&actors[i], motionIdx, 0,
                                    viewerMode ? "viewer-start" : "stance");
    }
    PCPort_BattleFlowInit(&battleFlow, actors, &battleSetup);
    if (setupDisablesAutoplay) {
        battleFlow.autoplay = 0;
        printf("[battle-flow] autoplay=0 reason=interactive-colosseum-setup\n");
    }

    GSgfxInit(0x7DDD0, 0x10, 0x8, 0x20, 1, 0x1E0);
    EnsureFontAtlas();
    memset(&battleBackdropTex, 0, sizeof(battleBackdropTex));
    memset(&battleMessageTex, 0, sizeof(battleMessageTex));
    memset(&battleCommandTex, 0, sizeof(battleCommandTex));
    memset(battleIconTex, 0, sizeof(battleIconTex));
    haveBattleBackdrop = LoadFsysSpriteTexObj(
        "orig/GC6E01/disc/files/fight_common.fsys", "menu_048",
        &battleBackdropTex);
    if (!haveBattleBackdrop) {
        haveBattleBackdrop = LoadFsysSpriteTexObj(
            "orig/GC6E01/disc/files/fight_common.fsys", "menu001",
            &battleBackdropTex);
    }
    haveBattleMessageTex = LoadFsysSpriteTexObj(
        "orig/GC6E01/disc/files/fight_common.fsys", "menu001",
        &battleMessageTex);
    haveBattleCommandTex = LoadFsysSpriteTexObj(
        "orig/GC6E01/disc/files/colosseumbattle_menu.fsys", "menu_076",
        &battleCommandTex);
    if (!haveBattleCommandTex) {
        haveBattleCommandTex = LoadFsysSpriteTexObj(
            "orig/GC6E01/disc/files/toolbattle_menu.fsys", "menu001",
            &battleCommandTex);
    }
    if (getenv("PCPORT_BATTLE_NO_STAGE") == NULL) {
        const char* stageArchive = getenv("PCPORT_BATTLE_STAGE_ARCHIVE");
        if (stageArchive == NULL || stageArchive[0] == '\0') {
            stageArchive = "orig/GC6E01/disc/files/M1_stadium_1F.fsys";
        }
        haveBattleStage = PCPort_EngineFieldSetup(stageArchive);
        drawBattleStage = getenv("PCPORT_BATTLE_DRAW_STAGE") != NULL;
        printf("[battle-arena] stage=%s draw=%s source=%s models=%d "
               "placement=grid-derived reason=%s\n",
               haveBattleStage ? "loaded" : "fallback-floor",
               (haveBattleStage && drawBattleStage) ? "enabled-debug" : "suppressed",
               stageArchive, haveBattleStage ? (g_engExtraRootJointCount + 1) : 0,
               (haveBattleStage && !drawBattleStage)
                   ? "field-map-camera-scale-not-battle-framed"
                   : "-");
    }
    if (getenv("PCPORT_BATTLE_NO_ICON_FALLBACK") == NULL) {
        for (i = 0; i < 4; ++i) {
            const char* iconPath = PCPort_BattleIconPathForMember(actors[i].member);
            haveBattleIcon[i] = LoadPngTexObj(iconPath, &battleIconTex[i]);
            printf("[battle-icon] actor=%s member=%s icon=%s loaded=%d "
                   "policy=manual-fallback-PCPORT_BATTLE_ICONS\n",
                   actors[i].label, actors[i].member,
                   iconPath != NULL ? iconPath : "-", haveBattleIcon[i]);
        }
    }
    drawPkxMesh = getenv("PCPORT_BATTLE_NO_PKX_MESH") == NULL;
    disablePkxDepth = drawPkxMesh &&
                      getenv("PCPORT_BATTLE_PKX_NO_ZTEST") != NULL;
    printf("[battle-pkx] meshDraw=%s reason=%s%s next=RenderSkinnedPObj-model-space/display-list\n",
           drawPkxMesh ? "enabled" : "suppressed-debug",
           drawPkxMesh ? "default-headed-pkx-model-space" :
                         "PCPORT_BATTLE_NO_PKX_MESH",
           getenv("PCPORT_BATTLE_SHOW_PKX_MESH") != NULL
               ? " deprecated-env-PCPORT_BATTLE_SHOW_PKX_MESH-ignored"
               : "");
    if (drawPkxMesh) {
        printf("[battle-pkx] depth=%s reason=%s\n",
               disablePkxDepth ? "disabled-debug" : "enabled-debug",
               disablePkxDepth
                   ? "PCPORT_BATTLE_PKX_NO_ZTEST"
                   : "arena-depth-cleared-before-pkx");
    }
    printf("[battle-arena] backdrop=%s source=%s controlPObjPolicy=%s "
           "skin=%s\n",
           haveBattleBackdrop ? "loaded" : "fallback",
           haveBattleBackdrop ? "fight_common.fsys" : "procedural",
           getenv("PCPORT_BATTLE_SHOW_CONTROL_POBJS") == NULL
               ? "suppress"
               : "show-debug",
           drawPkxMesh ? "enabled" : "suppressed-with-mesh");
    printf("[battle-ui] message=%s(fight_common.fsys:menu001) "
           "command=%s(colosseumbattle_menu.fsys:menu_076/toolbattle_menu.fsys:menu001) "
           "glyphs=host-bitmap-pending-game-font\n",
           haveBattleMessageTex ? "loaded" : "fallback",
           haveBattleCommandTex ? "loaded" : "fallback");
    printf("[battle-camera] mode=host-colosseum-angled fov=45 near=20 far=500 "
           "eye0=(0.0,55.5,132.0) interest=(0.0,10.0,-6.0) "
           "orbit=deterministic arenaCandidate=M1_stadium_1F.fsys\n");
    fflush(stdout);
    g_pcBattleSuppressControlPObjs =
        (getenv("PCPORT_BATTLE_SHOW_CONTROL_POBJS") == NULL) ? 1 : 0;
    g_pcBattleMaterialLogBudget = g_pcBattleSuppressControlPObjs ? 12u : 0u;
    g_pcBattleRenderSkin = drawPkxMesh;
    printf("[battle-scene] loaded actors=4 frameCap=%d mode=%s\n",
           frameCap, viewerMode ? "pkx-viewer" : "battle");
    printf("[battle-state] source=common_rel playerTrainer=0x%04X "
           "enemyTrainer=0x%04X playerMove=%u enemyMove=%u "
           "actors=%s,%s,%s,%s\n",
           battleSetup.playerTrainerId, battleSetup.enemyTrainerId,
           battleSetup.playerMoveId, battleSetup.enemyMoveId,
           actors[0].member, actors[1].member, actors[2].member,
           actors[3].member);
    printf("[battle-state] text command=\"FIGHT  POKEMON  BAG  RUN\" "
           "player=\"%s used %s!\" enemy=\"%s used %s!\"\n",
           actors[0].displayName, battleSetup.playerMoveName,
           actors[2].displayName, battleSetup.enemyMoveName);
    fflush(stdout);

    for (frame = 0; ; ++frame) {
        MenuTreeStats stats;
        int debugMenuKey;
        if (window != NULL && glfwWindowShouldClose(window)) {
            break;
        }
        if (frameCap > 0 && frame >= frameCap) {
            break;
        }
        debugMenuKey = PCPort_BattleKeyDown(window, GLFW_KEY_GRAVE_ACCENT);
        if (debugMenuKey && !prevDebugMenuKey) {
            if (!viewerMode || !pkxViewer.singleDebug) {
                viewerMode = 1;
                PCPort_PkxViewerInit(&pkxViewer, actors);
                pkxViewer.singleDebug = 1;
                pkxViewer.selectedSlot = 0;
                actors[0].x = 0.0f;
                actors[0].z = 18.0f;
                actors[0].yaw = 0.0f;
                printf("[debug-ui] opened tab=pokemon-models frame=%d "
                       "partySlots=6 activeSlots=2 model=%s\n",
                       frame, actors[0].member != NULL ? actors[0].member : "-");
            } else {
                pkxViewer.enabled = 0;
                pkxViewer.singleDebug = 0;
                viewerMode = 0;
                PCPort_BattleApplyGridPlacement(actors);
                PCPort_BattleFlowInit(&battleFlow, actors, &battleSetup);
                printf("[debug-ui] closed frame=%d\n", frame);
            }
            fflush(stdout);
        }
        prevDebugMenuKey = debugMenuKey;
        memset(&stats, 0, sizeof(stats));
        if (viewerMode) {
            PCPort_PkxViewerUpdate(&pkxViewer, window, actors, frame);
            PCPort_PkxViewerSweepUpdate(&pkxViewer, actors, frame);
            if (getenv("PCPORT_PKX_VIEWER_AUTOSTART") != NULL && frame >= 2) {
                pkxViewer.startBattle = 1;
            }
            if (pkxViewer.startBattle) {
                f32 battleScale = 1.8f;
                if (scaleEnv != NULL && scaleEnv[0] != '\0') {
                    f32 v = (f32)atof(scaleEnv);
                    if (v > 0.0f) {
                        battleScale = v;
                    }
                }
                viewerMode = 0;
                pkxViewer.enabled = 0;
                for (i = 0; i < 4; ++i) {
                    actors[i].scale = battleScale;
                }
                PCPort_BattleApplyGridPlacement(actors);
                for (i = 0; i < 4; ++i) {
                    PCPort_BattleActorSetMotion(&actors[i],
                                                actors[i].stanceMotion,
                                                frame,
                                                "viewer-start-battle");
                }
                PCPort_BattleFlowInit(&battleFlow, actors, &battleSetup);
                if (setupDisablesAutoplay) {
                    battleFlow.autoplay = 0;
                    printf("[battle-flow] autoplay=0 reason=interactive-colosseum-battle\n");
                }
                printf("[battle-scene] setup-started-battle frame=%d actors=%s,%s,%s,%s\n",
                       frame,
                       actors[0].member != NULL ? actors[0].member : "-",
                       actors[1].member != NULL ? actors[1].member : "-",
                       actors[2].member != NULL ? actors[2].member : "-",
                       actors[3].member != NULL ? actors[3].member : "-");
                fflush(stdout);
            }
        } else {
            PCPort_BattleFlowUpdate(&battleFlow, window, frame, actors);
            {
                const char* dbgPokemonFrame = getenv("PCPORT_BATTLE_DEBUG_POKEMON_FRAME");
                if (dbgPokemonFrame != NULL && atoi(dbgPokemonFrame) == frame) {
                    battleFlow.requestPokemonSetup = 1;
                    printf("[battle-debug] frame=%d request=POKEMON setup\n", frame);
                }
            }
            if (battleFlow.requestPokemonSetup) {
                viewerMode = 1;
                PCPort_PkxViewerInit(&pkxViewer, actors);
                pkxViewer.prevZ = PCPort_BattleKeyDown(window, GLFW_KEY_Z);
                pkxViewer.prevEnter = PCPort_BattleKeyDown(window, GLFW_KEY_ENTER);
                if (getenv("PCPORT_BATTLE_UNIT") == NULL) {
                    static const f32 kViewerX[4] = { -54.0f, -18.0f, 18.0f, 54.0f };
                    static const f32 kViewerZ[4] = { 18.0f, 18.0f, 18.0f, 18.0f };
                    for (i = 0; i < 4; ++i) {
                        actors[i].x = kViewerX[i];
                        actors[i].z = kViewerZ[i];
                        actors[i].yaw = 0.0f;
                    }
                }
                battleFlow.requestPokemonSetup = 0;
                printf("[battle-scene] pokemon-command-opened-setup frame=%d actors=%s,%s,%s,%s\n",
                       frame,
                       actors[0].member != NULL ? actors[0].member : "-",
                       actors[1].member != NULL ? actors[1].member : "-",
                       actors[2].member != NULL ? actors[2].member : "-",
                       actors[3].member != NULL ? actors[3].member : "-");
                fflush(stdout);
            }
        }
        for (i = 0; i < 4; ++i) {
            PCPort_BattleActorStep(&actors[i]);
        }
        if (frame == 0 || frame == 1 || frame == 30 || frame == 90 ||
            frame == 141 || frame == 180 || frame == 241 || frame == 301 ||
            frame == 360 || frame == 421 || frame == 500) {
            PCPort_BattleLogMotionSample(actors, 4, frame,
                                         battleFlow.messageText);
        }
        VIWaitForRetrace_PC();
        ClearBackbuffer(0.03f, 0.04f, 0.06f);
        GSgfx_BeginFrame();
        ClearBackbuffer(0.08f, 0.10f, 0.13f);
        DrawBattleBackdrop2D(&battleBackdropTex, haveBattleBackdrop);
        PCPort_SetBattleCameraView(&camera, frame);
        GXSetViewport(0.0f, 0.0f, (f32)PCPORT_WINDOW_WIDTH,
                      (f32)PCPORT_WINDOW_HEIGHT, 0.0f, 1.0f);
        GXSetScissor(0u, 0u, PCPORT_WINDOW_WIDTH, PCPORT_WINDOW_HEIGHT);
        GXSetProjection(camera.projectionMatrix, GX_PERSPECTIVE);
        GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
        GXLoadPosMtxImm(camera.viewMatrix, 0);
        if (haveBattleStage && drawBattleStage) {
            RenderJointTree(&g_engTitleArchive, g_engTitleRootJoint,
                            g_engTitleRootJoint, &camera,
                            (int)PCPORT_REAL_MATERIAL_PIPELINE, &stats);
            for (i = 0; i < g_engExtraRootJointCount; ++i) {
                RenderJointTree(&g_engTitleArchive, g_engExtraRootJoints[i],
                                g_engExtraRootJoints[i], &camera,
                                (int)PCPORT_REAL_MATERIAL_PIPELINE, &stats);
            }
        } else {
            DrawBattleArenaFloor();
        }

        if (drawPkxMesh) {
            /* The host arena/backdrop pass is a temporary placement aid; keep its
             * depth writes from clipping the PKX actor self-depth pass. */
            ClearDepthBuffer();
            if (disablePkxDepth) {
                GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
            }
            /* PKX verts are model-space; submit them through the identity path so
             * the actor model matrix (folded into actorCam) places them correctly.
             * The rigid/envelope palette is still available only as an explicit
             * failure-repro knob because it double-transforms model-space PKX verts. */
            g_pcBattleModelSpaceVerts =
                (getenv("PCPORT_BATTLE_FORCE_SKINPAL") == NULL);
            if (frame == 0) {
                printf("[battle-pkx] vertexSpace=%s palette=%s reason=%s%s\n",
                       g_pcBattleModelSpaceVerts ? "model-space" : "skin-palette",
                       g_pcBattleModelSpaceVerts ? "bypassed" : "forced-debug",
                       g_pcBattleModelSpaceVerts
                           ? "pkx-verts-match-meshdump-model-space"
                           : "PCPORT_BATTLE_FORCE_SKINPAL",
                       getenv("PCPORT_BATTLE_SKINPAL") != NULL
                           ? " deprecated-env-PCPORT_BATTLE_SKINPAL-ignored"
                           : "");
            }
            { const char* hy = getenv("PCPORT_HALO_MAXY");
              if (hy != NULL && hy[0]) g_haloLiftMaxY = (f32)atof(hy); }
            {
                int savedSuppress = g_pcBattleSuppressControlPObjs;
                int renderActors = (viewerMode && pkxViewer.singleDebug) ? 1 : 4;
                if (viewerMode && pkxViewer.singleDebug) {
                    g_pcBattleSuppressControlPObjs = 0;
                }
                for (i = 0; i < renderActors; ++i) {
                PCPortTranslatedCamera actorCam = camera;
                f32 actorMtx[3][4];
                PCPort_BuildActorModelMatrix(&actors[i], actorMtx);
                PCPortMulAffine3x4(camera.viewMatrix, actorMtx,
                                   actorCam.viewMatrix);
                RenderJointTree(&actors[i].archive, actors[i].rootJoint,
                                actors[i].rootJoint, &actorCam,
                                (int)PCPORT_REAL_MATERIAL_PIPELINE, &stats);
                }
                g_pcBattleSuppressControlPObjs = savedSuppress;
            }
            g_pcBattleModelSpaceVerts = 0;
            if (disablePkxDepth) {
                GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
            }
        }

        /* 2D Pokemon icons were a fallback for when the 3D pkx meshes didn't
         * render; the skinned pkx now render, so icons are OFF by default.
         * Set PCPORT_BATTLE_ICONS=1 to bring them back. */
        if (getenv("PCPORT_BATTLE_ICONS") != NULL) {
            DrawBattleActorIconFallback(actors, battleIconTex, haveBattleIcon, frame);
        }
        if (viewerMode) {
            DrawPkxViewerUI(&pkxViewer, actors);
        } else {
            DrawBattleUI(&battleMessageTex, haveBattleMessageTex,
                         &battleCommandTex, haveBattleCommandTex, &battleFlow);
        }

        if ((getenv("PCPORT_RENDER_DEBUG") != NULL ||
             getenv("PCPORT_BATTLE_LOG_MOTION") != NULL) && frame == 0) {
            printf("[battle-scene] render stats joints=%u dobjs=%u drawn=%u "
                   "skipped=%u textured=%u materialOnly=%u suppressedControlPObj=%u\n",
                   stats.joints, stats.dobjs, stats.drawn, stats.skipped,
                   stats.textured, stats.materialOnly,
                   stats.battleControlPObjSuppressed);
        }
        GSgfxSwapBuffers(1);
        if (viewerMode && pkxViewer.sweepDone) {
            break;
        }
        if (frameCap == 0) {
            continue;
        }
    }
    printf("[battle-scene] frames=%d\n", frame);
    HoldWindowOpen(window);
    g_pcBattleSuppressControlPObjs = 0;
    g_pcBattleMaterialLogBudget = 0;
    g_pcBattleRenderSkin = 0;
    PCPort_FreeBattleRenderActors(actors, 4);
    return 1;
}

int main(int argc, char** argv) {
    int audioInitialized = 0;
    int runRealContentParserSmoke;
    int runJObjInstanceSmoke;
    int runJObjResolveSmoke;
    int runJObjLoadWrapperSmoke;
    int runRealSceneSlice4Smoke;
    int runRealTevSceneSlice3Smoke;
    int runRealTevSceneSlice2Smoke;
    int runRealTevSceneSliceSmoke;
    int runRealSceneSlice3Smoke;
    int runRealSceneSlice2Smoke;
    int runRealMaterialDeltaSmoke;
    int runRealTexturedSceneSliceSmoke;
    int runRealContentTranslationSmoke;
    int runGSgfxPObjSmoke;
    int runGSgfxScissorRetry;
    int runGSgfxSceneLikeSmoke;
    int runGSgfxVisibleSmoke;
    int runGXPrimitiveSmoke;
    int runGXScissorSmoke;
    int osInitialized = 0;
    int runGsGfxSmoke;
    int runWindowSmoke;
    int runStoryFieldSmoke;
    int runFieldRoomWarpSmoke;
    int runFieldRoomReloadSmoke;
    int runFieldWorldWarpSmoke;
    int runFieldLocomotionSmoke;
    int runFieldMessageSmoke;
    int runFieldNpcTalkSmoke;
    int runFieldNpcModelSmoke;
    int runFieldNpcOpenSmoke;
    int runFieldPeopleSnapshotSmoke;
    int runFieldStartMenuSmoke;
    int runWorldMapHandoffSmoke;
    int runWorldMapMenuSmoke;
    int runWorldMapMenu;
    int runMenu;
    int runEngine;
    int runEngineBoot;
    int runField;
    int runBattle;
    int runPkxViewer;
    int exitCode = 0;
    char trkBuffer[32];
    char trkSuffix[8];
    unsigned long appType;
    unsigned long trkLen;
    unsigned long tvFormat;
    GLFWwindow* window = NULL;

    runWindowSmoke = HasArg(argc, argv, "--window-smoke");
    runStoryFieldSmoke = HasArg(argc, argv, "--story-field-smoke");
    runFieldRoomWarpSmoke = HasArg(argc, argv, "--field-room-warp-smoke");
    runFieldRoomReloadSmoke = HasArg(argc, argv, "--field-room-reload-smoke");
    runFieldWorldWarpSmoke = HasArg(argc, argv, "--field-world-warp-smoke");
    runFieldLocomotionSmoke = HasArg(argc, argv, "--field-locomotion-smoke");
    runFieldMessageSmoke = HasArg(argc, argv, "--field-message-smoke");
    runFieldNpcTalkSmoke = HasArg(argc, argv, "--field-npc-talk-smoke");
    runFieldNpcModelSmoke = HasArg(argc, argv, "--field-npc-model-smoke");
    runFieldNpcOpenSmoke = HasArg(argc, argv, "--field-npc-open-smoke");
    runFieldPeopleSnapshotSmoke =
        HasArg(argc, argv, "--field-people-snapshot-smoke");
    runFieldStartMenuSmoke = HasArg(argc, argv, "--field-start-menu-smoke");
    runWorldMapHandoffSmoke = HasArg(argc, argv, "--worldmap-handoff-smoke");
    runWorldMapMenuSmoke = HasArg(argc, argv, "--worldmap-menu-smoke");
    runWorldMapMenu = HasArg(argc, argv, "--worldmap-menu");
    runGsGfxSmoke = HasArg(argc, argv, "--gsgfx-smoke");
    runJObjInstanceSmoke = HasArg(argc, argv, "--jobj-instance-smoke");
    runJObjResolveSmoke = HasArg(argc, argv, "--jobj-resolve-smoke");
    runJObjLoadWrapperSmoke = HasArg(argc, argv, "--jobj-load-wrapper-smoke");
    runRealContentParserSmoke = HasArg(argc, argv, "--real-content-parser-smoke");
    runRealSceneSlice4Smoke = HasArg(argc, argv, "--real-scene-slice-4-smoke");
    runRealTevSceneSlice3Smoke = HasArg(argc, argv, "--real-tev-scene-slice-3-smoke");
    runRealTevSceneSlice2Smoke = HasArg(argc, argv, "--real-tev-scene-slice-2-smoke");
    runRealTevSceneSliceSmoke = HasArg(argc, argv, "--real-tev-scene-slice-smoke");
    runRealSceneSlice3Smoke = HasArg(argc, argv, "--real-scene-slice-3-smoke");
    runRealSceneSlice2Smoke = HasArg(argc, argv, "--real-scene-slice-2-smoke");
    runRealMaterialDeltaSmoke = HasArg(argc, argv, "--real-material-delta-smoke");
    runRealTexturedSceneSliceSmoke = HasArg(argc, argv, "--real-textured-scene-slice-smoke");
    runRealContentTranslationSmoke = HasArg(argc, argv, "--real-content-translation-smoke");
    runGSgfxPObjSmoke = HasArg(argc, argv, "--gsgfx-pobj-smoke");
    runGSgfxScissorRetry = HasArg(argc, argv, "--gsgfx-scissor-retry");
    runGSgfxSceneLikeSmoke = HasArg(argc, argv, "--gsgfx-scene-like-smoke");
    runGSgfxVisibleSmoke = HasArg(argc, argv, "--gsgfx-visible-smoke");
    runGXPrimitiveSmoke = HasArg(argc, argv, "--gx-primitive-smoke");
    runGXScissorSmoke = HasArg(argc, argv, "--gx-scissor-smoke");
    runMenu = HasArg(argc, argv, "--menu");
    runEngine = HasArg(argc, argv, "--engine");
    runEngineBoot = HasArg(argc, argv, "--engine-boot");
    runField = HasArg(argc, argv, "--field");
    runBattle = HasArg(argc, argv, "--battle");
    runPkxViewer = HasArg(argc, argv, "--pkx-viewer");

    printf("[pcport_bootstrap] Starting stub native bootstrap\n");
    fflush(stdout);

    {
        const char* batchFrames = getenv("PCPORT_MOTION_BATCH_PROBE");
        const char* motionFrames = getenv("PCPORT_MOTION_PROBE");
        const char* bankFrames = getenv("PCPORT_CHARANIM_BANK_PROBE");
        if (batchFrames != NULL) {
            printf("[pcport_bootstrap] MOTION_BATCH_PROBE requested\n");
            fflush(stdout);
            PCPort_HeadlessMotionBatchProbe(atoi(batchFrames));
            return 0;
        }
        if (motionFrames != NULL || bankFrames != NULL) {
            const char* sa = getenv("PCPORT_SWIZ_ARCHIVE");
            const char* sm = getenv("PCPORT_SWIZ_MEMBER");
            printf("[pcport_bootstrap] %s requested\n",
                   motionFrames != NULL ? "MOTION_PROBE"
                                        : "CHARANIM_BANK_PROBE");
            fflush(stdout);
            PCPort_MotionProbe(
                sa != NULL ? sa : "orig/GC6E01/disc/files/field_common.fsys",
                sm != NULL ? sm : "ken_b1",
                atoi(motionFrames != NULL ? motionFrames : bankFrames));
            return 0;
        }
    }

    /* Resolve assets relative to the exe, so launching by double-click / from any
     * directory works (assets are loaded via repo-relative paths). */
    PCPort_ChdirToAssetRoot(argc > 0 ? argv[0] : NULL);

    /* FSYS member enumeration (no window/GL): PCPORT_FSYS_LIST=<path> lists the
     * members of an archive (used to locate character models etc.). */
    {
        const char* fl = getenv("PCPORT_FSYS_LIST");
        if (fl != NULL && fl[0] != '\0') {
            PCPort_FsysListMembers(fl);
            return 0;
        }
    }

    /* THP decode smoke: pure decode -> PPM, no window/GL. Verifies thp_player +
     * stb_image in the host build. */
    if (HasArg(argc, argv, "--bgm-smoke")) {
        return RunBGMSmoke() ? 0 : 1;
    }
    if (HasArg(argc, argv, "--musyx-smoke")) {
        return RunMusyXSmoke() ? 0 : 1;
    }
    if (HasArg(argc, argv, "--thp-audio-smoke")) {
        return RunTHPAudioSmoke() ? 0 : 1;
    }
    if (HasArg(argc, argv, "--thp-smoke")) {
        return RunTHPSmoke() ? 0 : 1;
    }

    /* P-A spike: headless cooperative-fibre scheduler self-test. No window/GL —
     * proves the fn_800F0308 vsync-yield semantics run natively on host fibres. */
    if (HasArg(argc, argv, "--fibre-test")) {
        return RunFibreSelfTest() ? 0 : 1;
    }

    /* P-B: headless unit test of the host GStask/GSthread scheduler (gs_sched_host). */
    if (HasArg(argc, argv, "--sched-test")) {
        return RunSchedTest() ? 0 : 1;
    }

    if (runWindowSmoke || runGsGfxSmoke || runRealContentParserSmoke ||
        runRealSceneSlice4Smoke ||
        runRealTevSceneSlice3Smoke ||
        runRealTevSceneSlice2Smoke ||
        runRealTevSceneSliceSmoke ||
        runRealSceneSlice3Smoke ||
        runRealSceneSlice2Smoke ||
        runRealMaterialDeltaSmoke ||
        runRealTexturedSceneSliceSmoke ||
        runRealContentTranslationSmoke ||
        runGSgfxPObjSmoke ||
        runGSgfxScissorRetry || runGSgfxSceneLikeSmoke ||
        runGSgfxVisibleSmoke ||
        runGXPrimitiveSmoke || runGXScissorSmoke ||
        runStoryFieldSmoke ||
        runFieldRoomWarpSmoke ||
        runFieldRoomReloadSmoke ||
        runFieldWorldWarpSmoke ||
        runFieldLocomotionSmoke ||
        runFieldMessageSmoke ||
        runFieldNpcTalkSmoke ||
        runFieldNpcModelSmoke ||
        runFieldNpcOpenSmoke ||
        runFieldPeopleSnapshotSmoke ||
        runFieldStartMenuSmoke ||
        runWorldMapHandoffSmoke ||
        runWorldMapMenuSmoke ||
        runWorldMapMenu ||
        runMenu || runEngine || runEngineBoot || runField || runBattle ||
        runPkxViewer ||
        argc <= 1) {
        window = CreateSmokeWindow();
        if (window == NULL) {
            return 1;
        }

        PCPort_SetHostWindow(window);
        printf("[pcport_bootstrap] Native window + GL context created\n");
    }

    OSInit_PC();
    osInitialized = 1;
    PADInit();

    if (!JAudio_Init()) {
        fprintf(stderr, "[pcport_bootstrap] JAudio_Init failed\n");
        exitCode = 1;
        goto cleanup;
    }
    audioInitialized = 1;

    if (!DVDInit_PC()) {
        fprintf(stderr, "[pcport_bootstrap] DVDInit_PC failed\n");
        exitCode = 1;
        goto cleanup;
    }

    TraceGLStage("GXInit:main-call:start");
    GXInit(NULL, 0);
    TraceGLStage("GXInit:main-call:done");

    CurrTvMode = 2;
    tvFormat = VIGetTvFormat();
    if (tvFormat != 1) {
        fprintf(stderr,
                "[pcport_bootstrap] VIGetTvFormat returned %lu, expected 1\n",
                tvFormat);
        exitCode = 1;
        goto cleanup;
    }

    TRK_memcpy(trkSuffix, "trk", 4);
    TRK_memcpy(trkBuffer, "bridge:", 8);
    TRK_strcat(trkBuffer, trkSuffix);
    trkLen = TRK_strlen(trkBuffer);
    if (trkLen != 10) {
        fprintf(stderr,
                "[pcport_bootstrap] TRK utility verification failed (len=%lu)\n",
                trkLen);
        exitCode = 1;
        goto cleanup;
    }

    __OSSetAppType(0x12345678UL);
    appType = __OSGetAppType();
    if (appType != 0x12345678UL) {
        fprintf(stderr,
                "[pcport_bootstrap] OSStateFlags verification failed (appType=%lu)\n",
                appType);
        exitCode = 1;
        goto cleanup;
    }

    printf("[pcport_bootstrap] Stub subsystems initialized\n");
    printf("[pcport_bootstrap] Linked decomp TU VIGetTvFormat verified (mode=%lu)\n",
           tvFormat);
    printf("[pcport_bootstrap] Linked decomp TU TRKUtil verified (%s, len=%lu)\n",
           trkBuffer, trkLen);
    printf("[pcport_bootstrap] Linked decomp TU OSStateFlags verified (appType=0x%08lX)\n",
           appType);
    if (runJObjLoadWrapperSmoke) {
        if (!RunJObjLoadWrapperSmoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] JObj fn_801A0FBC direct loader wrapper exercised\n");
    } else if (runJObjResolveSmoke) {
        if (!RunJObjResolveSmoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] JObj HSD_JObjResolveRefsAll direct resolver exercised\n");
    } else if (runJObjInstanceSmoke) {
        if (!RunJObjInstanceSmoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] JObj instance child resolved to canonical live child\n");
    } else if (runRealContentParserSmoke) {
        if (!RunRealContentParserSmoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Real FSYS member parsed through host HSD archive bridge\n");
    } else if (runRealSceneSlice4Smoke) {
        if (!RunRealSceneSlice4Smoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Real GX_VA_TEX1 scene slice exercised through the existing game-owned draw bridge\n");
    } else if (runRealTevSceneSlice3Smoke) {
        if (!RunRealTevSceneSlice3Smoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Second distinct real TEV scene slice exercised through the existing game-owned draw bridge\n");
    } else if (runRealTevSceneSlice2Smoke) {
        if (!RunRealTevSceneSlice2Smoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Broader real TEV scene slice exercised through the existing game-owned draw bridge\n");
    } else if (runRealTevSceneSliceSmoke) {
        if (!RunRealTevSceneSliceSmoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Real TEV-interpreted scene slice exercised through the existing game-owned draw bridge\n");
    } else if (runRealSceneSlice3Smoke) {
        if (!RunRealSceneSlice3Smoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Real multi-object scene slice exercised through the existing game-owned draw bridge\n");
    } else if (runRealTexturedSceneSliceSmoke) {
        if (!RunRealTexturedSceneSliceSmoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Real textured repository-content scene slice exercised through the existing game-owned draw bridge\n");
    } else if (runRealSceneSlice2Smoke) {
        if (!RunRealSceneSlice2Smoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Real scene slice 2 exercised through the existing game-owned draw bridge\n");
    } else if (runRealMaterialDeltaSmoke) {
        if (!RunRealMaterialDeltaSmoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Real material alpha delta exercised through live HSD MObj load and game-owned draw bridge\n");
    } else if (runRealContentTranslationSmoke) {
        if (!RunRealContentTranslationSmoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Real FSYS member translated into host HSD_PObj state\n");
    } else if (runGSgfxPObjSmoke) {
        if (!RunGSgfxPObjSmoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Game-owned fn_800DAD10 path exercised via HSD_PObj-backed state\n");
    } else if (runGSgfxScissorRetry) {
        if (!RunGSgfxScissorRetry()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Game-owned fn_800D9D68 path exercised\n");
    } else if (runGSgfxSceneLikeSmoke) {
        if (!RunGSgfxSceneLikeSmoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Game-owned fn_800DAD10 display-list path exercised\n");
    } else if (runGSgfxVisibleSmoke) {
        if (!RunGSgfxVisibleAttempt()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Visible GSgfx_BeginFrame path exercised directly\n");
    } else if (runGXPrimitiveSmoke) {
        if (!RunGXPrimitiveSmoke()) {
            exitCode = 1;
            goto cleanup;
        }
        if (!RunGSgfxVisibleAttempt()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Host GX primitive path and unchanged GSgfx_BeginFrame attempt both executed\n");
    } else if (runGXScissorSmoke) {
        if (!RunGXScissorSmoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Host GX scissor path exercised\n");
    } else if (runGsGfxSmoke) {
        if (!RunGSgfxSmoke()) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Real game-owned GSgfx callback/render path exercised\n");
    } else if (runWindowSmoke) {
        if (!RunWindowSmokeLoop(window)) {
            fprintf(stderr,
                    "[pcport_bootstrap] Window smoke loop failed to present a frame\n");
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] No game code, assets, or decompiled frame path started\n");
    } else if (runBattle || runPkxViewer) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --battle/--pkx-viewer requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunBattleScene(window, runPkxViewer)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] %s rendered through the existing draw bridge\n",
               runPkxViewer ? "PKX model verifier" :
                              "Battle Colosseum host scene");
    } else if (runField) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --field requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunFieldScene(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Field map rendered through the game-owned draw bridge\n");
    } else if (runEngineBoot) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --engine-boot requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunEngineBoot(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Engine-boot: GStask/GSthread scheduler drove the frame loop\n");
    } else if (runEngine) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --engine requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunEngineSpike(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Engine-fibre spike: host<->engine cooperative round-trip ticked frames\n");
    } else if (runWorldMapMenuSmoke || runWorldMapMenu) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --worldmap-menu requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunWorldMapMenuScene(window, runWorldMapMenuSmoke)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Worldmap menu scene rendered; smoke gates cursor/select/confirm\n");
    } else if (runWorldMapHandoffSmoke) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --worldmap-handoff-smoke requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunWorldMapHandoffSmoke(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Worldmap handoff smoke rendered world_map.fsys through the field scene bridge\n");
    } else if (runFieldNpcTalkSmoke) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --field-npc-talk-smoke requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunFieldNpcTalkSmoke(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Field NPC talk smoke exercised fn_801812E8 and message close\n");
    } else if (runFieldNpcModelSmoke) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --field-npc-model-smoke requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunFieldNpcModelSmoke(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Field NPC model smoke rendered a real S1_out dependency member\n");
    } else if (runFieldNpcOpenSmoke) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --field-npc-open-smoke requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunFieldNpcOpenSmoke(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Field NPC open smoke exercised NPC setup, dialogue, and story marker\n");
    } else if (runFieldPeopleSnapshotSmoke) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --field-people-snapshot-smoke requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunFieldPeopleSnapshotSmoke(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Field people snapshot smoke round-tripped a 0x158 NPC record\n");
    } else if (runFieldStartMenuSmoke) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --field-start-menu-smoke requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunFieldStartMenuSmoke(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Field START-menu smoke exercised open, navigation, and close\n");
    } else if (runFieldMessageSmoke) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --field-message-smoke requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunFieldMessageSmoke(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Field message smoke exercised text reveal, page advance, and close\n");
    } else if (runFieldLocomotionSmoke) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --field-locomotion-smoke requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunFieldLocomotionSmoke(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Field locomotion smoke exercised retail action-slot turn zones and facing cases\n");
    } else if (runFieldWorldWarpSmoke) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --field-world-warp-smoke requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunFieldWorldWarpSmoke(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Field world-warp smoke exercised S1_out trigger crossing and shop reload\n");
    } else if (runFieldRoomReloadSmoke) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --field-room-reload-smoke requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunFieldRoomReloadSmoke(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Field room-reload smoke exercised live trigger crossing and map reload\n");
    } else if (runFieldRoomWarpSmoke) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --field-room-warp-smoke requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunFieldRoomWarpSmoke(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Field room-warp smoke exercised trigger lookup and map reloads\n");
    } else if (runStoryFieldSmoke) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] --story-field-smoke requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunStoryFieldSmoke(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Story Mode field smoke exercised New Game handoff, spawn, collision, animation, and movement\n");
    } else if (runMenu || window != NULL) {
        if (window == NULL) {
            fprintf(stderr,
                    "[pcport_bootstrap] Menu scene requested but no window/GL context available\n");
            exitCode = 1;
            goto cleanup;
        }

        if (!RunMenuScene(window)) {
            exitCode = 1;
            goto cleanup;
        }

        printf("[pcport_bootstrap] Top-menu scene graph rendered through the existing game-owned draw bridge\n");

        /* Main-menu handoffs keep one native window alive while switching from
         * title/menu into the selected game mode. */
        if (g_pcEnterBattleColosseum && window != NULL &&
            !glfwWindowShouldClose(window)) {
            printf("[pcport_bootstrap] Entering Battle Colosseum setup\n");
            if (!RunBattleScene(window, 1)) {
                exitCode = 1;
                goto cleanup;
            }
        } else if (g_pcEnterFieldWalk && window != NULL &&
            !glfwWindowShouldClose(window)) {
            printf("[pcport_bootstrap] Entering Story Mode field (walkable)\n");
            if (!RunFieldScene(window)) {
                exitCode = 1;
                goto cleanup;
            }
        }
    } else {
        printf("[pcport_bootstrap] No game code, assets, or render loop started\n");
    }

cleanup:
    if (audioInitialized) {
        JAudio_Shutdown();
    }

    if (osInitialized) {
        OSShutdown_PC();
    }

    if (window != NULL) {
        PCPort_SetHostWindow(NULL);
        DestroySmokeWindow(window);
    }

    if (exitCode == 0) {
        printf("[pcport_bootstrap] Shutdown complete\n");
    }

    return exitCode;
}
