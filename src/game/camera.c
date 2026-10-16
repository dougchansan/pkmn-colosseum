/**
 * @file camera.c
 * @brief The scene camera: state, target/offset animation, timed moves,
 *        pad-driven debug cameras, per-floor defaults and save state.
 *
 * Whole-TU source. Linked (2026-10-03) through camera_801765F4.c, which
 * defines CAMERA_801765F4_ONLY and owns .text 0x801765F4-0x80179DFC (every
 * function up to and including cameraInit), cameraInit's .rodata image
 * 0x80273D98-0x80273DC8, all of .data and the whole .sdata2 pool. The
 * save-state handlers after it link from camera_exact_80179DFC.c and
 * camera_candidate_80179E04.c. In that build the state block and its
 * pointers are extern (their data units own them), and two .rodata objects
 * that retail places after the stripped cameraDispInfo's strings (the zero
 * vector at 0x80273DC8 and the cameraWaitSyncAnime message at 0x80273F34)
 * are read through extern names, a listed rule exception. The notes below
 * on the remaining .rodata gap and the blocker statement are the record
 * that this link resolves for every section except that .rodata tail.
 *
 * TU extent (text and data):
 *   .text   0x801765F4-0x80179F4C  fn_801765F4 .. _cameraRestoreStateData;
 *                                  gs_spline.c ends at 0x801765F4, the
 *                                  unoptimised fn_80179F4C after it is not
 *                                  camera code (gs_range_80179F4C.c)
 *   .rodata 0x80273D98-0x80273F70  cameraInit's initialisers, zero vectors,
 *                                  15 debug strings, the
 *                                  cameraWaitSyncAnime message; fsys'
 *                                  "gsfsys.toc" starts at 0x80273F70
 *   .data   0x8036C248-0x8036C29C  the up vector, then cameraUpdate's two
 *                                  switch tables
 *   .bss    0x80452EC8-0x80452FC8  the camera state
 *   .sdata  0x80478C40-0x80478C48  the state pointer (+pad)
 *   .sbss   0x8047B1A8-0x8047B1B0  the floor-entry table and its handle
 *   .sdata2 0x8047D720-0x8047D790  30.0f and 1.0f, then the float pool
 *
 * The file is written in reverse address order and built with
 * -inline auto,deferred: MWCC then emits the functions back in retail order
 * and inlines the camera's own setters wherever retail expands them
 * (cameraReturn is GSscene_SetMode, cameraSetHeight/Distance/Fov,
 * cameraMoveTarget, cameraMoveRotationXYZ and cameraMoveEndCheck inlined).
 * The unit flags are GC/1.3.2 (cameraInit's initialiser copy loads all
 * twelve words before storing, which GC/1.3 does not do), -inline
 * auto,deferred and -str reuse,readonly (the strings live in .rodata); no
 * local pragmas.
 *
 * Recomp status (user decision, 2026-09-28): the recomp ports this unit
 * natively and row 43 is cleared as a decomp blocker; the decomp returns to
 * linking it later. The findings below are what that work starts from.
 *
 * Why the whole TU (with all of its .rodata) does not link:
 * - .rodata: retail keeps 15 debug strings ("TargetFollow" .. "Far Z:(%.2f)",
 *   0x80273DF8-0x80273F34) and three zero vectors (0x80273DD4-0x80273DF8)
 *   from a function the linker stripped. Nothing here produces them, so the
 *   unit's .rodata is 0x160 bytes short in the middle of the range (0x77 of
 *   0x1D8 bytes); every other section is byte-identical up to end padding.
 *   The function is cameraDispInfo (lane B43, 2026-09-28): the Pokemon XD
 *   JP demo's linker map (NXXJ01 Master.MAP, published as NXXJ01.map in
 *   StarsMmd/Colo-XD-PBR-symbol-maps 6b51d3af) lists in camera.o
 *   "UNUSED 0x3EC cameraDispInfo", placed right after cameraSetMirrorFlag
 *   (our fn_801765F4) and cameraGetAddSize, and its .rodata as 15 UNUSED
 *   strings @2606, @2614, @2617..@2629 whose sizes (0xD, 0xD, 0xC, 0x19,
 *   0x1B, 0xF, 0xF, 0x15, 0x1B, 0x1D, 0x1B, 0x15, 0xE, 0xE, 0xD) are exactly
 *   these 15 strings in this order; the seven short type names between
 *   "TargetFollow" and "Offset Anime" (@2607..@2613) are .sdata2 strings and
 *   the 9-entry name table (@2630, 0x24) is in .data. Its code is stripped
 *   in every known build (XD demo, XD retail, this one), so no source gives
 *   its body: a reconstruction would be guessed code, and it stays out.
 *   The survival itself is natural. Controlled test with this unit's
 *   compiler and flags and the project linker (GC/1.2.5n): MWCC emits the
 *   strings and initializer images of an uncalled global (and of an
 *   unreferenced plain static; a static inline emits nothing); mwld then
 *   strips them object by object, unless a live function in the same object
 *   addresses .rodata from its section symbol "...rodata.0". cameraInit
 *   does exactly that (lbl_80273D98 = this unit's first .rodata byte, then
 *   offsets), so the whole .rodata section, the stripped function's strings
 *   and zero images included, is kept, while its code, a .data table and
 *   .sdata2 names are stripped. A research shape (three zero-initialized
 *   GSSceneVec3 locals, a static 9-name table, the 13 format strings in
 *   order, placed just above fn_801765F4) makes this unit's .rodata
 *   byte-identical to retail apart from the relocated handler words; it is
 *   not committed because its body is not evidenced.
 * - cameraUpdate: exact with the XD-named view-mode helpers
 *   (_cameraFollowUpdate, _cameraLookAtUpdate, _cameraFreeUpdate,
 *   _cameraDynamicUpdate, with the perspective helper as _cameraUpdateFov),
 *   admitted as a listed rule exception (user decision, 2026-10-03; see
 *   docs/RULE_EXCEPTIONS.md). Written in place, retail's frame differs:
 *   the NaN check slots of the first switch's two sqrtf expansions sit at
 *   76/80, above the four perspective blocks (12-72), but in-place code puts
 *   them at 12/16 (99.96%). Moving the mode 0/1-2/3/7 views into the XD-named
 *   static inlines gives exactly retail's frame, and none of them survives
 *   as a symbol (fact 3 below has the evidence and why the written
 *   sister-title clause alone did not admit them).
 * Every other function is exact.
 *
 * LINK BLOCKER STATEMENT (lane C43, 2026-09-28). Under the written policy
 * (docs/CAMPAIGN_OPERATIONS.md, "Strict acceptance policy" and
 * "Reconstructed inline helpers") this unit cannot be linked, and with it
 * cameraPlayAnime, cameraPlayOffsetAnime, cameraUpdate, cameraInit,
 * cameraSetFloorDefault, fn_80179748, cameraResetFloor, cameraSetGScamera,
 * the pad/offset updaters and the save-state handlers stay unlinked. Four
 * facts, each checked against retail:
 *
 * 1. The whole TU is the smallest linkable unit. cameraPlayAnime's chunk
 *    (0x80176C78) reads pool literals 0x8047D724 (1.0f, also read by
 *    cameraInit) and 0x8047D738 (also read by cameraUpdate and cameraInit).
 *    MWCC emits one .sdata2 pool per TU, so a carve holding cameraPlayAnime
 *    must also hold cameraInit and cameraUpdate, or read the literals through
 *    extern declarations, which is the rejected synthetic-compiler-data form
 *    (windowOpen, 7402d254). cameraInit then brings the whole .rodata
 *    section: it addresses its initializer images from the section symbol
 *    (lbl_80273D98 plus offsets), so .rodata cannot be split off or declared
 *    extern either.
 *
 * 2. .rodata needs cameraDispInfo's body, and no build or public source has
 *    it. Where we looked:
 *    - Pokemon XD JP demo map (NXXJ01.map, StarsMmd/Colo-XD-PBR-symbol-maps
 *      6b51d3af): "UNUSED 0x3EC cameraDispInfo" in camera.o and
 *      "UNUSED 0x24 dbgMenuCameraDispInfo" in dbgMenuCamera.o. Its only
 *      caller is itself stripped, so the demo DOL has no code for either.
 *    - Pokemon XD retail (TeamOrre/xd-decomp 4989794e symbols.txt,
 *      trevor403/xd-asm b1087f18): camera.o's text is contiguous from
 *      cameraSetMirrorFlag (0x80196A74, size 0x24) to cameraSetOffsetScale
 *      (0x80196A98), with no 0x3EC gap. Stripped.
 *    - Colosseum: no function refers to it. dbgMenuCameraChangeDisp
 *      (0x80006654) only opens window 6 (menuOpenCustom(6, 0, 0, 0, 1, 0),
 *      then menuSetPosition(6, 20, 260)), and so does the pad camera's
 *      button 0x400. The menu-bios entry for window 6 (0x802E2DB8 + 6 * 0x1C)
 *      has no callback, so retail opens the window empty. The draw routine
 *      that filled it is the stripped one.
 *    - Pokemon Battle Revolution (Genius Sonority, Wii): its public
 *      disassemblies, pret/pokerevo ae02670e and bgsamm/pbr-dtk 09af9f9a,
 *      cover main.dol only (the GS engine), not the app code that holds
 *      camera.o. The camera names in StarsMmd's RPB*.map files are bad
 *      matches: RPBP01.map's "cameraResumeAnime" at 0x802A8F3C is
 *      WPADGetDataFormat in pbr-dtk.
 *    - Pokemon Box Ruby & Sapphire is not a Genius Sonority title (no GS
 *      engine). Pokemon Channel is Ambrella's.
 *    - GitHub code search for "cameraDispInfo", "_cameraFollowUpdate" and
 *      the format strings ("TargetOfs:(", "Field of View:(", "Near Z:(",
 *      "Far Z:(") finds nothing from these games.
 *    Without the body, the 0x160 bytes (three zero vectors and 13 format
 *    strings between cameraInit's images and the cameraWaitSyncAnime
 *    message) can only come from invented code or data. The policy rejects
 *    both: stand-in functions or data placed for pool or string order, and
 *    guessed semantics. The Melee precedent (tobj.c 0631ea73) admits a real
 *    function taken from another decomp; no decomp has this one.
 *
 * 3. cameraUpdate's frame proves inline nesting that in-place C cannot
 *    produce. MWCC (this unit's compiler and flags) gives frame slots to
 *    inlined locals by inline depth: every depth-1 local first, in source
 *    order and from the top of the frame down, then depth 2, then depth 3.
 *    Depth here means the sqrtf expansion's inner __fpclassifyf parameter
 *    (the NaN-check slot, S) and _cameraUpdateFov's four floats (P).
 *    Retail from the top of the frame down: S1=80, S2=76 (the first
 *    switch's two sqrtf), P0=60, P12=44, P3=28, P7=12 (the four view modes),
 *    S3=8 (the case-1/2 sqrtf). In-place code gives P0..P7 at depth 1 and
 *    S1..S3 at depth 2, so all four P blocks sit above S1/S2 whatever the
 *    declaration order. Writing the perspective code out (depth 0) does the
 *    same. A controlled test built all 16 subsets of {mode 0, modes 1-2,
 *    mode 3, mode 7} with that view's body in a static inline. Each subset
 *    put exactly its wrapped P blocks (and S3, when modes 1-2 are wrapped)
 *    one level deeper, as the model predicts. Only the full set gives
 *    retail's order. So retail's code has each of the four view bodies
 *    inside an inlined function, with the perspective code nested inside
 *    those. Pokemon XD retail's cameraUpdate (0x80198100, xd-asm) has the
 *    same slots: S 0x50/0x4C, P 0x3C/0x2C/0x1C/0x0C, S3 0x08.
 *    Why that evidence still does not admit the helpers under the written
 *    clauses:
 *    - Repeated expansion: none. find_inline_expansions finds each view
 *      body once, in cameraUpdate, in Colosseum and in XD alike (best other
 *      matches: _cameraOffsetAnimeUpdate at 0.72, mode 0 against mode 7 at
 *      0.66).
 *    - Listed fingerprints: none. Frame-slot order is stack allocation,
 *      which the policy rejects as sole evidence for a single-use helper.
 *    - Same-engine sister-title clause: XD names the functions only in the
 *      demo map, as UNUSED (inlined everywhere, no standalone body):
 *      _cameraFollowUpdate__FP9_GScamera 0x194,
 *      _cameraLookAtUpdate__FP9_GScamera 0x218,
 *      _cameraFreeUpdate__FP9_GScamera 0x9C,
 *      _cameraDynamicUpdate__FP9_GScamera 0x174,
 *      _cameraUpdateFov__FP9_GScamera 0x58.
 *      Compiled out of line with this unit's compiler, our bodies are
 *      _cameraUpdateFov 0x58 and mode 3 0x9C (both match XD), and
 *      mode 0 0x100, modes 1-2 0x1E0, mode 7 0x100 (XD 0x194/0x218/0x174).
 *      XD's inlined mode-3 block makes the same calls in the same order with
 *      the same arguments (its PSVECSubtract is our fn_800E0168, a GSvec
 *      wrapper around PSVECSubtract that XD inlines), but its mode 0/1-2/7
 *      blocks call PSMTX/PSVEC routines directly and do more work. So for
 *      three of the four helpers the "same calls in the same order" test
 *      fails, and none of the four has an XD function body to compare
 *      against. All four are needed (fact 3), so cameraUpdate cannot reach
 *      100% under the policy. Lane B43's research form is b734a41f.
 *
 * 4. Nothing else blocks the unit: with a real cameraDispInfo body and the
 *    four view helpers, every section is byte-identical (B43 and this lane's
 *    research builds). The decision the unit needs is outside the written
 *    policy. Either a user ruling admits the view helpers on the XD demo map
 *    plus the frame-depth proof and accepts some form for the stripped
 *    function's .rodata, or cameraPlayAnime and the rest of this TU are
 *    ported natively.
 *
 * lbl_8047D720 (30.0f) and lbl_8047D724 (1.0f) come before the float pool,
 * which is otherwise in first-use order starting with cameraSetFov's 3.0f and
 * 120.0f. MWCC places named constants defined after their uses there, so they
 * are named file-scope constants here, defined at the end.
 */

#include "game/camera_types.h"
#include "game/gs_render_util.h"
#include "crt/math_ppc.h"

typedef struct CameraFloorEntry {
    /* 0x00 */ s32 initialized;
    /* 0x04 */ void* floor;
    /* 0x08 */ f32 defaultHeight;
    /* 0x0C */ f32 defaultDistance;
    /* 0x10 */ f32 defaultRotationY;
    /* 0x14 */ f32 defaultFov;
    /* 0x18 */ f32 height;
    /* 0x1C */ f32 distance;
    /* 0x20 */ f32 rotationY;
    /* 0x24 */ f32 fov;
} CameraFloorEntry;

/* What the save-state handlers store: the camera state, then the render
 * camera's snapshot (428 bytes, _cameraGetStateSize). */
typedef struct CameraSaveData {
    /* 0x000 */ CameraPadState state;
    /* 0x0FC */ GSRenderCameraSnapshot camera;
} CameraSaveData;

typedef struct CameraStateHandlers {
    void (*restore)(void* data);
    void (*make)(void* data);
    u32 (*getSize)(void);
} CameraStateHandlers;

extern void GScameraSetAnimRate(GSRenderCamera* camera, f32 rate);
extern void GScameraStartAnimation(GSRenderCamera* camera);
extern u8 GScameraHasAnimationEnded(GSRenderCamera* camera);
extern u8 GScameraIsAnimating(GSRenderCamera* camera);
extern f32 GScameraGetAnimFrame(GSRenderCamera* camera);
extern void GScameraSetAnimIndex(void* camera, s32 index);
extern void GScameraSetAnimFrame(void* camera, f32 frame);
extern void fn_800D1858(void* camera, s32 loop);
extern GSRenderCamera* GScameraGetActiveCamera(void);
extern u32 GSthreadGetCurrentThread(void);
extern void clear__5GSvecFv(void* vector);
extern void* GSmodelGetPart(void* model, s32 partIndex);
extern void GSpartGetTransform(void* part, void* transform, u32 arg2, u32 arg3);
extern void GSpartFree(void* part);
extern void GSvecSub(void* dst, void* lhs, void* rhs);
extern void GSlerpGetLinearInterpolationVector(void* dst, const void* start,
                                               const void* end, f32 t);
extern u8 fn_801174C4(void);
extern u8 fn_801174EC(void);
extern void fn_80117500(void);
extern void fn_80117330(f32 duration);
extern s32 fn_800D37CC(void);
extern s32 GSgetFrameStep(void);
extern u8 dbgMenuIsOpen(void);
extern u32 fn_800F7AF0(s32 pad);
extern u32 fn_800F7BC4(s32 pad);
extern s8 fn_800F7A7C(s32 pad, s32 mode);
extern s8 fn_800F7A08(s32 pad, s32 mode);
extern s8 fn_800F7994(s32 pad, s32 mode);
extern s8 fn_800F7920(s32 pad, s32 mode);
extern void menuClose(u32 id);
extern void menuSetPosition(u32 id, s32 x, s32 y);
extern void fn_800E0718(void* out, const void* axis, f32 angle);
extern void fn_800E0738(void* out, const void* a, const void* b);
extern void GSvecTransformQuat(void* out, const void* quat, const void* vec);
extern void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void);
extern u32 _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u16 handle);
extern void* fn_800D29A0(void);
extern void GSresRegisterResource(void* resource, u32 group, u32 id, u32 flags);
extern void fn_800FF4D4(void* data, u8 typeId);
extern const GSSceneVec3 lbl_80315540;
extern const GSSceneVec3 lbl_8031554C;
extern const GSSceneVec3 lbl_80315558;
extern u8* lbl_80478FBC;

void cameraSetFov(f32 fov);
void cameraSetRotY(f32 angle);
void cameraSetDistance(f32 distance);
void cameraSetHeight(f32 height);
void cameraStopAnime(void* object);
u8 cameraMoveEndCheck(u8 wait);
void cameraMoveRotationXYZ(f32 x, f32 y, f32 z, f32 duration);
void cameraMoveRotation(void* unused, GSSceneVec3* rotation, f32 duration);
void cameraMovePosition(void* unused, GSSceneVec3* position, f32 duration);
void cameraMoveTargetPos(void* unused, GSSceneVec3* target, f32 duration);
void cameraMoveTarget(void* unused, u32 group, u32 id, f32 duration);
void GSscene_SetCameraRotationVector(GSSceneVec3* rotation);
void GSscene_SetCameraDirectionVector(GSSceneVec3* direction);
void GSscene_SetCameraPositionVector(GSSceneVec3* position);
void GSscene_SetCameraViewVector(GSSceneVec3* view);
void cameraSetTarget(u32 group, u32 id);
u32 GSscene_SetMode(u32 mode);
void cameraRefreshTargetPos(void);
void _cameraOffsetAnimeUpdate__FP9_GScamera(GSRenderCamera* camera);
void _cameraPadRotateUpdate__FP9_GScamera(void* camera);
void _cameraPadMoveUpdate__FP9_GScamera(void* camera);
void fn_80179748(f32 height, f32 distance, f32 rotationY, f32 fov);
u32 _cameraGetStateSize(void);
void _cameraMakeStateData(void* data);
void _cameraRestoreStateData(void* data);

extern const f32 lbl_8047D720;
extern const f32 lbl_8047D724;

#ifdef CAMERA_801765F4_ONLY
/* The linked text unit (camera_801765F4.c): the state block and its
 * pointers stay with the data units that own them. */
extern CameraPadState lbl_80452EC8;
extern void* lbl_80478C40;
extern u16 lbl_8047B1AC;
extern void* lbl_8047B1A8;
/* RULE-EXCEPTION(title-path): extern named stand-ins for two of this TU's
 * own .rodata objects (cameraMoveTarget's zero initialiser and the
 * cameraWaitSyncAnime message), which sit after the stripped cameraDispInfo
 * strings the linked unit cannot own - see docs/RULE_EXCEPTIONS.md */
extern const GSSceneVec3 lbl_80273DC8;
extern const char lbl_80273F34[];
#else
static CameraPadState lbl_80452EC8;
void* lbl_80478C40 = &lbl_80452EC8;
u16 lbl_8047B1AC;
void* lbl_8047B1A8;
#endif
static GSSceneVec3 lbl_8036C248 = { 0.0f, 1.0f, 0.0f };

/*
 * Inline helpers. Each is expanded at several call sites in retail with the
 * same instruction sequence (policy: repeated expansion) and none survives
 * as a symbol.
 */

/* The current floor's entry in the per-floor camera table. */
static inline CameraFloorEntry* cameraFindFloorEntry(void* floor)
{
    CameraFloorEntry* entries = (CameraFloorEntry*)lbl_8047B1A8;
    u32 i;

    for (i = 0; i < *(u32*)lbl_80478FB8; i++) {
        if (floor == entries[i].floor) {
            return &entries[i];
        }
    }
    return 0;
}

/* The camera animation resource the state points at. */
static inline GSRenderCamera* cameraGetCurrentAnimation(void)
{
    CameraPadState* state = lbl_80478C40;
    GSRenderCamera* animation = GSresGetResource(
        state->animationGroup, state->animationId);

    if (animation == NULL) {
        animation = fn_800F92D4(state->animationId);
    }
    return animation;
}

/* Re-applies the state's fov, keeping the camera's other frustum values
 * (the end of every view mode and of both pad cameras). */
static inline void _cameraUpdateFov(GSRenderCamera* camera)
{
    f32 fov;
    f32 aspect;
    f32 near;
    f32 far;

    GScameraGetPerspective(camera, &fov, &aspect, &near, &far);
    GScameraSetPerspective(camera, ((CameraPadState*)lbl_80478C40)->fov,
                           aspect, near, far);
}

/*
 * The fov getter both pad cameras read before cameraSetFov: its result is
 * formed in a temporary and copied into cameraSetFov's parameter, the
 * inline return-value fingerprint (fadds f1; fmr f29,f1; fcmpo on f1).
 */
static inline f32 cameraGetFov(void)
{
    return ((CameraPadState*)lbl_80478C40)->fov;
}


#ifndef CAMERA_801765F4_ONLY
/* Linked from camera_candidate_80179E04.c and camera_exact_80179DFC.c. */
void _cameraRestoreStateData(void* data)
{
    CameraSaveData* save = data;
    GSRenderCamera* camera;

    memcpy(lbl_80478C40, &save->state, sizeof(CameraPadState));
    camera = GSresGetResource(0, 0);
    if (((CameraPadState*)lbl_80478C40)->mode == 4 ||
        ((CameraPadState*)lbl_80478C40)->mode == 8) {
        camera = cameraGetCurrentAnimation();
    }
    fn_800D13C8(camera, &save->camera);
    fn_800D258C(camera);
}

void _cameraMakeStateData(void* data)
{
    CameraSaveData* save = data;
    GSRenderCamera* camera = GSresGetResource(0, 0);

    if (((CameraPadState*)lbl_80478C40)->mode == 4 ||
        ((CameraPadState*)lbl_80478C40)->mode == 8) {
        camera = cameraGetCurrentAnimation();
    }
    memcpy(&save->state, lbl_80478C40, sizeof(CameraPadState));
    fn_800D1674(camera, &save->camera);
}

u32 _cameraGetStateSize(void)
{
    return sizeof(CameraSaveData);
}
#endif

void cameraInit(void) {
    GSSceneVec3 view = { 0.0f, 14.0f, 0.0f };
    GSSceneVec3 up = { 0.0f, 1.0f, 0.0f };
    GSSceneVec3 eye = { 0.0f, 0.0f, 100.0f };
    CameraStateHandlers handlers = {
        _cameraRestoreStateData, _cameraMakeStateData, _cameraGetStateSize
    };
    void* camera;
    u32 i;
    u16 handle;

    memset(&lbl_80452EC8, 0, sizeof(CameraPadState));
    lbl_80478C40 = &lbl_80452EC8;
    camera = fn_800D29A0();
    GSresRegisterResource(camera, 0, 0, 0);

    GSscene_SetMode(0);
    cameraSetTarget(0, 100);
    GSscene_SetCameraViewVector(&view);

    handle = _toolentryAlloc__FUl(*(u32*)lbl_80478FB8 *
                                  sizeof(CameraFloorEntry));
    lbl_8047B1AC = handle;
    lbl_8047B1A8 = fn_800E27B0(handle);
    memset(lbl_8047B1A8, 0, *(u32*)lbl_80478FB8 * sizeof(CameraFloorEntry));
    for (i = 0; i < *(u32*)lbl_80478FB8; i++) {
        ((CameraFloorEntry*)lbl_8047B1A8)[i].initialized = 0;
        ((CameraFloorEntry*)lbl_8047B1A8)[i].floor =
            *(void**)(lbl_80478FBC + 0x0C + i * 0x4C);
    }

    clear__5GSvecFv(&((CameraPadState*)lbl_80478C40)->offsetPosition);
    clear__5GSvecFv(&((CameraPadState*)lbl_80478C40)->offsetRotation);
    set__5GSvecFfff(&((CameraPadState*)lbl_80478C40)->offsetScale,
                    lbl_8047D724, lbl_8047D724, lbl_8047D724);
    ((CameraPadState*)lbl_80478C40)->fov = lbl_8047D720;
    fn_800FF4D4(&handlers, 1);
    fn_800FF4D4(&handlers, 2);
    GScameraLookAt((GSRenderCamera*)camera,
                   (const GSRenderVec3*)&up,
                   (const GSRenderVec3*)&view);
    GScameraSetPosition(camera, &eye);
    fn_800D258C(camera);
    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
}

void cameraSetFloorDefault(f32 height, f32 distance, f32 rotationY) {
    CameraFloorEntry* floorEntry;

    floorEntry = cameraFindFloorEntry(std092_getCurrentRoomID());
    if (floorEntry == NULL) {
        return;
    }

    floorEntry->defaultHeight = height;
    floorEntry->defaultDistance = distance;
    floorEntry->defaultRotationY = rotationY;
    if (floorEntry->initialized != 1) {
        return;
    }

    floorEntry->initialized = 2;
    cameraSetHeight(height);
    cameraSetDistance(distance);
    cameraSetRotY(rotationY);
}

void fn_80179748(f32 height, f32 distance, f32 rotationY, f32 fov) {
    CameraFloorEntry* floorEntry;

    floorEntry = cameraFindFloorEntry(std092_getCurrentRoomID());
    if (floorEntry != 0) {
        if (floorEntry->initialized != 0) {
            height = floorEntry->height;
            distance = floorEntry->distance;
            rotationY = floorEntry->rotationY;
            fov = floorEntry->fov;
        } else {
            if (height < 0.0f) {
                height = 0.0f;
            }
            if (distance < 10.0f) {
                distance = lbl_8047D720;
                height = 0.0f;
            } else if (distance > 500.0f) {
                distance = 500.0f;
            }
            floorEntry->initialized = 1;
            floorEntry->defaultHeight = height;
            floorEntry->defaultDistance = distance;
            floorEntry->defaultRotationY = rotationY;
            floorEntry->defaultFov = fov;
        }
    }

    cameraSetHeight(height);
    cameraSetDistance(distance);
    cameraSetRotY(rotationY);
    cameraSetFov(fov);
}

void cameraResetFloor(void) {
    CameraFloorEntry* defaults;

    if (((CameraPadState*)lbl_80478C40)->mode == 6) {
        GSscene_SetMode(0);
    }

    defaults = cameraFindFloorEntry(std092_getCurrentRoomID());
    if (defaults == NULL) {
        return;
    }
    cameraSetHeight(defaults->defaultHeight);
    cameraSetDistance(defaults->defaultDistance);
    cameraSetRotY(defaults->defaultRotationY);
    cameraSetFov(defaults->defaultFov);
}

void cameraSetGScamera(void* camera) {
    GSRenderCamera* source;
    GSRenderCamera* target;
    GSRenderVec3 dist;
    f32 perspective;
    f32 aspect;
    f32 near;
    f32 far;

    if (camera == 0) {
        return;
    }

    source = (GSRenderCamera*)camera;
    target = (GSRenderCamera*)GSresGetResource(0, 0);

    GScameraGetPerspective(camera, &perspective, &aspect, &near, &far);
    GScameraSetPerspective(target, perspective, aspect, near, far);
    GScameraGetDistanceVector(camera, &dist);

    GSscene_SetMode(3);

    target->interest = source->interest;
    target->eye = source->eye;
    fn_80179748(dist.y, dist.z, (f32)atan2(dist.x, dist.z), perspective);
}

void _cameraPadMoveUpdate__FP9_GScamera(void* camera) {
    GSSceneVec3 transformed;
    f32 quatX[4];
    f32 quatY[4];
    f32 quatZ[4];
    f32 quat[4];
    GSSceneVec3 movement;
    s32 frameDelta;
    f32 fov;
    f32 moveX;
    f32 moveZ;
    f32 rotateX;
    f32 rotateY;
    f32 movementDivisor;

    frameDelta = GSgetFrameStep();
    movementDivisor = 64.0f;
    if (dbgMenuIsOpen()) {
        return;
    }

    if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x400) != 0) {
        if ((u8)menuIsCheck(6)) {
            menuClose(6);
        } else {
            menuOpenCustom(6, 0, 0, 0, 1, 0);
            menuSetPosition(6, 20, 260);
        }
    }

    fn_800E0718(quatX, &lbl_80315540,
                 ((CameraPadState*)lbl_80478C40)->rotation.x);
    fn_800E0718(quatY, &lbl_8031554C,
                 ((CameraPadState*)lbl_80478C40)->rotation.y);
    fn_800E0718(quatZ, &lbl_80315558,
                 ((CameraPadState*)lbl_80478C40)->rotation.z);
    fn_800E0738(quat, quatX, quatZ);
    fn_800E0738(quat, quatY, quat);

    moveX = fn_800F7A7C(1, 0) * frameDelta;
    moveZ = fn_800F7A08(1, 0) * frameDelta;
    rotateX = fn_800F7994(1, 0) * frameDelta;
    rotateY = fn_800F7920(1, 0) * frameDelta;

    if ((fn_800F7BC4(1) & 0x40) != 0) {
        movementDivisor *= 4.0f;
    }

    if ((fn_800F7BC4(1) & 0x800) != 0) {
        set__5GSvecFfff(&movement, moveX / movementDivisor,
                        -moveZ / movementDivisor,
                        0.0f);
    } else {
        set__5GSvecFfff(&movement, moveX / movementDivisor,
                        0.0f, moveZ / movementDivisor);
    }
    GSvecTransformQuat(&transformed, quat, &movement);
    GSvecAdd(&((CameraPadState*)lbl_80478C40)->direction,
             &((CameraPadState*)lbl_80478C40)->direction, &transformed);

    if ((fn_800F7BC4(1) & 0x20) != 0) {
        fov = rotateY / movementDivisor + cameraGetFov();
        cameraSetFov(fov);
    } else {
        ((CameraPadState*)lbl_80478C40)->rotation.x -=
            rotateY / (32.0f * movementDivisor);
    }

    ((CameraPadState*)lbl_80478C40)->rotation.y -=
        rotateX / (32.0f * movementDivisor);
    GScameraSetPosition(camera, &((CameraPadState*)lbl_80478C40)->direction);
    GScameraSetRotation(
        camera,
        (const GSRenderVec3*)&((CameraPadState*)lbl_80478C40)->rotation);
    _cameraUpdateFov(camera);
}

void _cameraPadRotateUpdate__FP9_GScamera(void* camera) {
    GSSceneVec3 interest;
    GSSceneVec3 direction;
    GSRenderMtx rotation;
    s32 rotateX;
    s32 rotateY;
    f32 distance;
    f32 angle;
    f32 fov;

    if (dbgMenuIsOpen()) {
        return;
    }

    rotateX = fn_800F7994(1, 1) * GSgetFrameStep();
    rotateY = fn_800F7920(1, 1) * GSgetFrameStep();

    angle = atan2f(((CameraPadState*)lbl_80478C40)->height,
                   ((CameraPadState*)lbl_80478C40)->distance);
    distance = sqrtf(((CameraPadState*)lbl_80478C40)->height *
                   ((CameraPadState*)lbl_80478C40)->height +
               ((CameraPadState*)lbl_80478C40)->distance *
                   ((CameraPadState*)lbl_80478C40)->distance);

    if ((fn_800F7BC4(1) & 0x20) != 0) {
        fov = rotateY / 64.0f + cameraGetFov();
        cameraSetFov(fov);
    } else if ((fn_800F7BC4(1) & 0x40) != 0) {
        distance += rotateY / 64.0f;
    } else {
        angle -= rotateY / 4096.0f;
    }

    ((CameraPadState*)lbl_80478C40)->height = distance * (f32)sin(angle);
    ((CameraPadState*)lbl_80478C40)->distance = distance * (f32)cos(angle);
    if (((CameraPadState*)lbl_80478C40)->distance < 10.0f) {
        ((CameraPadState*)lbl_80478C40)->distance = 10.0f;
    } else if (((CameraPadState*)lbl_80478C40)->distance >
               500.0f) {
        ((CameraPadState*)lbl_80478C40)->distance = 500.0f;
    }

    cameraSetHeight(((CameraPadState*)lbl_80478C40)->height);
    cameraSetDistance(((CameraPadState*)lbl_80478C40)->distance);

    set__5GSvecFfff(&direction, 0.0f,
                    ((CameraPadState*)lbl_80478C40)->height,
                    ((CameraPadState*)lbl_80478C40)->distance);

    ((CameraPadState*)lbl_80478C40)->rotation.y +=
        rotateX / 2048.0f;
    cameraSetRotY(((CameraPadState*)lbl_80478C40)->rotation.y);

    GSmtxMakeYRotation(rotation, ((CameraPadState*)lbl_80478C40)->rotation.y);
    GSvecTransform(&direction, rotation, &direction);
    GSvecAdd(&interest, &((CameraPadState*)lbl_80478C40)->position,
             &((CameraPadState*)lbl_80478C40)->view);
    GSvecAdd(&((CameraPadState*)lbl_80478C40)->direction, &interest,
             &direction);
    GScameraSetPosition(camera, &((CameraPadState*)lbl_80478C40)->direction);
    GScameraLookAt(camera, (const GSRenderVec3*)&lbl_8036C248,
                   (const GSRenderVec3*)&interest);
    ((CameraPadState*)lbl_80478C40)->rotation.x =
        -(f32)atan2(((CameraPadState*)lbl_80478C40)->height,
                    ((CameraPadState*)lbl_80478C40)->distance);
    _cameraUpdateFov(camera);
}

void _cameraOffsetAnimeUpdate__FP9_GScamera(GSRenderCamera* camera) {
    f32 fov;
    f32 aspect;
    f32 near;
    f32 far;
    GSRenderVec3 eye;
    GSRenderVec3 interest;
    GSRenderVec3 up;
    GSRenderMtx rotation;
    GSRenderCamera* animation;
    CameraPadState* state;

    state = lbl_80478C40;
    animation = GSresGetResource(state->animationGroup, state->animationId);
    if (animation == 0) {
        animation = fn_800F92D4(state->animationId);
    }
    if (animation == 0 || animation == camera) {
        return;
    }

    GScameraGetPerspective(animation, &fov, &aspect, &near, &far);
    GScameraSetPerspective(camera, fov, aspect, near, far);
    GScameraGetLookAt(animation, &up, &interest);
    set__5GSvecFfff(&up, 0.0f, lbl_8047D724, 0.0f);
    GScameraGetPosition(animation, &eye);

    fn_800E0108(&eye, &eye,
                &((CameraPadState*)lbl_80478C40)->offsetScale);
    fn_800E0108(&interest, &interest,
                &((CameraPadState*)lbl_80478C40)->offsetScale);
    GSmtxMakeXRotation(
        rotation, ((CameraPadState*)lbl_80478C40)->offsetRotation.x);
    fn_800E032C(rotation,
                 ((CameraPadState*)lbl_80478C40)->offsetRotation.y);
    fn_800E02E8(rotation,
                 ((CameraPadState*)lbl_80478C40)->offsetRotation.z);
    GSvecTransform(&eye, rotation, &eye);
    GSvecTransform(&interest, rotation, &interest);
    fn_800E0238(rotation, rotation);
    GSvecTransform(&up, rotation, &up);

    if ((((CameraPadState*)lbl_80478C40)->flags[2] & 1) != 0) {
        interest.x *= -1.0f;
        up.x *= -1.0f;
        eye.x *= -1.0f;
    }
    if ((((CameraPadState*)lbl_80478C40)->flags[2] & 2) != 0) {
        interest.y *= -1.0f;
        up.y *= -1.0f;
        eye.y *= -1.0f;
    }
    if ((((CameraPadState*)lbl_80478C40)->flags[2] & 4) != 0) {
        interest.z *= -1.0f;
        up.z *= -1.0f;
        eye.z *= -1.0f;
    }

    GSvecAdd(&eye, &eye,
             &((CameraPadState*)lbl_80478C40)->offsetPosition);
    GSvecAdd(&interest, &interest,
             &((CameraPadState*)lbl_80478C40)->offsetPosition);
    GScameraSetPosition(camera, &eye);
    GScameraLookAt(camera, &up, &interest);

    GSvecCopy(&((CameraPadState*)lbl_80478C40)->direction, &eye);
    GSvecSub(&((CameraPadState*)lbl_80478C40)->position, &interest,
                 &((CameraPadState*)lbl_80478C40)->view);
    GSvecSub(&eye, &eye,
                 &((CameraPadState*)lbl_80478C40)->position);
    ((CameraPadState*)lbl_80478C40)->height = eye.y;
    ((CameraPadState*)lbl_80478C40)->distance = sqrtf(eye.x * eye.x + eye.z * eye.z);
    ((CameraPadState*)lbl_80478C40)->rotation.y =
        (f32)atan2(eye.x, eye.z);
    ((CameraPadState*)lbl_80478C40)->rotation.x =
        -(f32)atan2(((CameraPadState*)lbl_80478C40)->height,
                    ((CameraPadState*)lbl_80478C40)->distance);
}

void cameraRefreshTargetPos(void)
{
    void* model;
    void* part;

    model = GSresGetResource(
        ((CameraPadState*)lbl_80478C40)->targetGroup,
        ((CameraPadState*)lbl_80478C40)->targetId);
    if (model != 0) {
        GSmodelGetPosition(
            model, &((CameraPadState*)lbl_80478C40)->position);
        if (((CameraPadState*)lbl_80478C40)->targetSubId >= 0) {
            part = GSmodelGetPart(
                model, ((CameraPadState*)lbl_80478C40)->targetSubId);
            if (part != 0) {
                GSpartGetTransform(
                    part, &((CameraPadState*)lbl_80478C40)->view, 0, 0);
                GSvecSub(
                    &((CameraPadState*)lbl_80478C40)->view,
                    &((CameraPadState*)lbl_80478C40)->view,
                    &((CameraPadState*)lbl_80478C40)->position);
                GSpartFree(part);
            }
        }
    }
}

/*
 * The view-mode updaters cameraUpdate expands. The names are the Pokemon XD
 * JP demo map's (NXXJ01.map: _cameraFollowUpdate, _cameraLookAtUpdate,
 * _cameraFreeUpdate, _cameraDynamicUpdate and _cameraUpdateFov, all in
 * camera.o, all inlined). Retail's frame layout needs each view body one
 * inline level deep (see the header, fact 3).
 * RULE-EXCEPTION(title-path): single-use inline helpers reproducing the
 * inline depth of cameraUpdate's frame - see docs/RULE_EXCEPTIONS.md
 */
/* Mode 0: orbit the target at the state height, distance and yaw. */
static inline void _cameraFollowUpdate(GSRenderCamera* camera)
{
    GSSceneVec3 interest;
    GSRenderMtx rotation;
    GSSceneVec3 offset;

    set__5GSvecFfff(&offset, 0.0f,
                    ((CameraPadState*)lbl_80478C40)->height,
                    ((CameraPadState*)lbl_80478C40)->distance);
    GSmtxMakeYRotation(rotation,
                       ((CameraPadState*)lbl_80478C40)->rotation.y);
    GSvecTransform(&offset, rotation, &offset);
    GSvecAdd(&((CameraPadState*)lbl_80478C40)->direction,
             &((CameraPadState*)lbl_80478C40)->position, &offset);
    GScameraSetPosition(camera,
                        &((CameraPadState*)lbl_80478C40)->direction);
    GSvecAdd(&interest, &((CameraPadState*)lbl_80478C40)->position,
             &((CameraPadState*)lbl_80478C40)->view);
    GScameraLookAt(camera, (const GSRenderVec3*)&lbl_8036C248,
                   (const GSRenderVec3*)&interest);
    ((CameraPadState*)lbl_80478C40)->rotation.x =
        -(f32)atan2(((CameraPadState*)lbl_80478C40)->height,
                    ((CameraPadState*)lbl_80478C40)->distance);
    _cameraUpdateFov(camera);
}

/* Modes 1 and 2: look at the target from the state position. */
static inline void _cameraLookAtUpdate(GSRenderCamera* camera)
{
    GSSceneVec3 interest;
    GSSceneVec3 eye;

    GSvecAdd(&interest,
             &((CameraPadState*)lbl_80478C40)->position,
             &((CameraPadState*)lbl_80478C40)->view);
    GScameraSetPosition(camera,
                        &((CameraPadState*)lbl_80478C40)->direction);
    GScameraLookAt(camera, (const GSRenderVec3*)&lbl_8036C248,
                   (const GSRenderVec3*)&interest);
    GSvecSub(&eye, &((CameraPadState*)lbl_80478C40)->direction,
                &interest);
    ((CameraPadState*)lbl_80478C40)->height = eye.y;
    ((CameraPadState*)lbl_80478C40)->distance =
        sqrtf(eye.x * eye.x + eye.z * eye.z);
    ((CameraPadState*)lbl_80478C40)->rotation.y =
        (f32)atan2(eye.x, eye.z);
    ((CameraPadState*)lbl_80478C40)->rotation.x =
        -(f32)atan2(((CameraPadState*)lbl_80478C40)->height,
                    ((CameraPadState*)lbl_80478C40)->distance);
    _cameraUpdateFov(camera);
}

/* Mode 3: free camera from the state position and rotation. */
static inline void _cameraFreeUpdate(GSRenderCamera* camera)
{
    GSSceneVec3 interest;
    GSSceneVec3 up;

    GScameraSetPosition(camera,
                        &((CameraPadState*)lbl_80478C40)->direction);
    GScameraSetRotation(
        camera,
        (const GSRenderVec3*)&((CameraPadState*)lbl_80478C40)->rotation);
    GScameraGetLookAt(camera, (GSRenderVec3*)&up,
                      (GSRenderVec3*)&interest);
    GSvecSub(&((CameraPadState*)lbl_80478C40)->position, &interest,
                &((CameraPadState*)lbl_80478C40)->view);
    _cameraUpdateFov(camera);
}

/* Mode 7: orbit, interest taken before the camera is placed. */
static inline void _cameraDynamicUpdate(GSRenderCamera* camera)
{
    GSSceneVec3 interest;
    GSRenderMtx rotation;
    GSSceneVec3 offset;

    set__5GSvecFfff(&offset, 0.0f,
                    ((CameraPadState*)lbl_80478C40)->height,
                    ((CameraPadState*)lbl_80478C40)->distance);
    GSmtxMakeYRotation(rotation,
                       ((CameraPadState*)lbl_80478C40)->rotation.y);
    GSvecTransform(&offset, rotation, &offset);
    GSvecAdd(&((CameraPadState*)lbl_80478C40)->direction,
             &((CameraPadState*)lbl_80478C40)->position, &offset);
    GSvecAdd(&interest, &((CameraPadState*)lbl_80478C40)->position,
             &((CameraPadState*)lbl_80478C40)->view);
    GScameraSetPosition(camera,
                        &((CameraPadState*)lbl_80478C40)->direction);
    GScameraLookAt(camera, (const GSRenderVec3*)&lbl_8036C248,
                   (const GSRenderVec3*)&interest);
    ((CameraPadState*)lbl_80478C40)->rotation.x =
        -(f32)atan2(((CameraPadState*)lbl_80478C40)->height,
                    ((CameraPadState*)lbl_80478C40)->distance);
    _cameraUpdateFov(camera);
}

void cameraUpdate(u32 captureIndex) {
    GSRenderCamera* camera;
    GSRenderCamera* animation;
    GSSceneVec3 moveVec;

    camera = GSresGetResource(0, 0);
    if (((CameraPadState*)lbl_80478C40)->flags[0] == 1) {
        if (((CameraPadState*)lbl_80478C40)->targetMoveActive != 0) {
            ((CameraPadState*)lbl_80478C40)->targetMoveTime +=
                (f32)(u32)GSgetFrameStep() / (f32)fn_800D37CC();
            if (((CameraPadState*)lbl_80478C40)->targetMoveTime >=
                ((CameraPadState*)lbl_80478C40)->targetMoveDuration) {
                GSvecCopy(&((CameraPadState*)lbl_80478C40)->position,
                          &((CameraPadState*)lbl_80478C40)->targetMoveEnd);
                ((CameraPadState*)lbl_80478C40)->targetMoveTime = 0.0f;
                ((CameraPadState*)lbl_80478C40)->targetMoveActive = 0;
            } else {
                GSlerpGetLinearInterpolationVector(
                    &((CameraPadState*)lbl_80478C40)->position,
                    &((CameraPadState*)lbl_80478C40)->targetMoveStart,
                    &((CameraPadState*)lbl_80478C40)->targetMoveEnd,
                    ((CameraPadState*)lbl_80478C40)->targetMoveTime /
                        ((CameraPadState*)lbl_80478C40)->targetMoveDuration);
            }
        }
        if (((CameraPadState*)lbl_80478C40)->targetOffsetMoveActive != 0) {
            ((CameraPadState*)lbl_80478C40)->targetOffsetMoveTime +=
                (f32)(u32)GSgetFrameStep() / (f32)fn_800D37CC();
            if (((CameraPadState*)lbl_80478C40)->targetOffsetMoveTime >=
                ((CameraPadState*)lbl_80478C40)->targetOffsetMoveDuration) {
                GSvecCopy(
                    &((CameraPadState*)lbl_80478C40)->view,
                    &((CameraPadState*)lbl_80478C40)->targetOffsetMoveEnd);
                ((CameraPadState*)lbl_80478C40)->targetOffsetMoveTime =
                    0.0f;
                ((CameraPadState*)lbl_80478C40)->targetOffsetMoveActive = 0;
            } else {
                GSlerpGetLinearInterpolationVector(
                    &((CameraPadState*)lbl_80478C40)->view,
                    &((CameraPadState*)lbl_80478C40)->targetOffsetMoveStart,
                    &((CameraPadState*)lbl_80478C40)->targetOffsetMoveEnd,
                    ((CameraPadState*)lbl_80478C40)->targetOffsetMoveTime /
                        ((CameraPadState*)lbl_80478C40)
                            ->targetOffsetMoveDuration);
            }
        }
        if (((CameraPadState*)lbl_80478C40)->positionMoveActive != 0) {
            ((CameraPadState*)lbl_80478C40)->positionMoveTime +=
                (f32)(u32)GSgetFrameStep() / (f32)fn_800D37CC();
            if (((CameraPadState*)lbl_80478C40)->positionMoveTime >=
                ((CameraPadState*)lbl_80478C40)->positionMoveDuration) {
                GSvecCopy(&((CameraPadState*)lbl_80478C40)->direction,
                          &((CameraPadState*)lbl_80478C40)->positionMoveEnd);
                ((CameraPadState*)lbl_80478C40)->positionMoveTime =
                    0.0f;
                ((CameraPadState*)lbl_80478C40)->positionMoveActive = 0;
            } else {
                GSlerpGetLinearInterpolationVector(
                    &((CameraPadState*)lbl_80478C40)->direction,
                    &((CameraPadState*)lbl_80478C40)->positionMoveStart,
                    &((CameraPadState*)lbl_80478C40)->positionMoveEnd,
                    ((CameraPadState*)lbl_80478C40)->positionMoveTime /
                        ((CameraPadState*)lbl_80478C40)->positionMoveDuration);
            }
        }
        if (((CameraPadState*)lbl_80478C40)->rotationMoveActive != 0) {
            ((CameraPadState*)lbl_80478C40)->rotationMoveTime +=
                (f32)(u32)GSgetFrameStep() / (f32)fn_800D37CC();
            if (((CameraPadState*)lbl_80478C40)->rotationMoveTime >=
                ((CameraPadState*)lbl_80478C40)->rotationMoveDuration) {
                GSvecCopy(&((CameraPadState*)lbl_80478C40)->rotation,
                          &((CameraPadState*)lbl_80478C40)->rotationMoveEnd);
                ((CameraPadState*)lbl_80478C40)->rotationMoveTime =
                    0.0f;
                ((CameraPadState*)lbl_80478C40)->rotationMoveActive = 0;
            } else {
                GSlerpGetLinearInterpolationVector(
                    &((CameraPadState*)lbl_80478C40)->rotation,
                    &((CameraPadState*)lbl_80478C40)->rotationMoveStart,
                    &((CameraPadState*)lbl_80478C40)->rotationMoveEnd,
                    ((CameraPadState*)lbl_80478C40)->rotationMoveTime /
                        ((CameraPadState*)lbl_80478C40)->rotationMoveDuration);
            }
        }

        switch (((CameraPadState*)lbl_80478C40)->mode) {
        case 0:
        case 1:
        case 5:
            if (fn_801174C4() != 0 && fn_801174EC() != 0) {
                GSvecAdd(&moveVec,
                         &((CameraPadState*)lbl_80478C40)->position,
                         &((CameraPadState*)lbl_80478C40)->view);
                GSvecSub(&moveVec,
                            &((CameraPadState*)lbl_80478C40)->direction,
                            &moveVec);
                ((CameraPadState*)lbl_80478C40)->height = moveVec.y;
                ((CameraPadState*)lbl_80478C40)->distance =
                    sqrtf(moveVec.x * moveVec.x + moveVec.z * moveVec.z);
            }
            break;
        case 7:
            GSvecSub(&moveVec,
                        &((CameraPadState*)lbl_80478C40)->direction,
                        &((CameraPadState*)lbl_80478C40)->position);
            ((CameraPadState*)lbl_80478C40)->height = moveVec.y;
            ((CameraPadState*)lbl_80478C40)->distance =
                sqrtf(moveVec.x * moveVec.x + moveVec.z * moveVec.z);
            if (((CameraPadState*)lbl_80478C40)->rotationMoveActive == 0 &&
                ((CameraPadState*)lbl_80478C40)->positionMoveActive != 0) {
                ((CameraPadState*)lbl_80478C40)->rotation.y =
                    (f32)atan2(moveVec.x, moveVec.z);
            }
            break;
        case 8:
            animation = cameraGetCurrentAnimation();
            if (animation != NULL && GScameraIsAnimating(animation) &&
                !GScameraHasAnimationEnded(animation)) {
                break;
            }
            return;
        }

        if (cameraMoveEndCheck(0) == 0) {
            ((CameraPadState*)lbl_80478C40)->flags[0] = 0;
        }
    } else {
        switch (((CameraPadState*)lbl_80478C40)->mode) {
        case 0:
        case 1:
        case 5:
        case 7:
            cameraRefreshTargetPos();
            break;
        case 2:
        case 3:
        case 6:
        case 8:
            break;
        case 4:
            camera = cameraGetCurrentAnimation();
            if (camera == 0) {
                ((CameraPadState*)lbl_80478C40)->animationGroup = 0;
                ((CameraPadState*)lbl_80478C40)->animationId = 0;
                GSscene_SetMode(((CameraPadState*)lbl_80478C40)->flags[1]);
                cameraSetTarget(0, 100);
                camera = GSresGetResource(0, 0);
            }
            break;
        }
    }

    switch (((CameraPadState*)lbl_80478C40)->mode) {
    case 0:
        _cameraFollowUpdate(camera);
        break;
    case 1:
    case 2:
        _cameraLookAtUpdate(camera);
        break;
    case 6:
        _cameraPadMoveUpdate__FP9_GScamera(camera);
        break;
    case 5:
        _cameraPadRotateUpdate__FP9_GScamera(camera);
        break;
    case 3:
        _cameraFreeUpdate(camera);
        break;
    case 8:
        _cameraOffsetAnimeUpdate__FP9_GScamera(camera);
        _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
        return;
    case 7:
        _cameraDynamicUpdate(camera);
        break;
    }

    fn_800D258C(camera);
    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
}

u32 GSscene_SetMode(u32 mode)
{
    CameraPadState* state = (CameraPadState*)lbl_80478C40;
    u32 previous;

    if (state->mode == (u8)mode) {
        return mode;
    }
    previous = state->mode;
    state->mode = (u8)mode;
    return previous;
}

u32 GSscene_GetMode(void)
{
    return *(u8*)lbl_80478C40;
}

GSRenderCamera* cameraGetActive(void)
{
    GSRenderCamera* camera = GScameraGetActiveCamera();

    if (camera != 0) {
        return camera;
    }
    camera = (GSRenderCamera*)GSresGetResource(0, 0);
    fn_800D258C(camera);
    return camera;
}

void cameraSetTarget(u32 group, u32 id)
{
    ((CameraPadState*)lbl_80478C40)->targetGroup = group;
    ((CameraPadState*)lbl_80478C40)->targetId = id;
    ((CameraPadState*)lbl_80478C40)->targetSubId = -1;
}

void cameraSetTargetExt(u32 group, u32 id, s32 subId)
{
    ((CameraPadState*)lbl_80478C40)->targetGroup = group;
    ((CameraPadState*)lbl_80478C40)->targetId = id;
    ((CameraPadState*)lbl_80478C40)->targetSubId = subId;
}

void GSscene_SetCameraViewVector(GSSceneVec3* view)
{
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->view, view);
}

void GSscene_GetCameraViewVector(GSSceneVec3* view)
{
    GSvecCopy(view, &((CameraPadState*) lbl_80478C40)->view);
}

void GSscene_SetCameraPositionVector(GSSceneVec3* position)
{
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->position, position);
}

void GSscene_GetCameraPositionVector(GSSceneVec3* position)
{
    GSvecCopy(position, &((CameraPadState*) lbl_80478C40)->position);
}

void GSscene_SetCameraDirectionVector(GSSceneVec3* direction)
{
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->direction, direction);
}

void GSscene_GetCameraDirectionVector(GSSceneVec3* direction)
{
    GSvecCopy(direction, &((CameraPadState*) lbl_80478C40)->direction);
}

void GSscene_SetCameraRotationVector(GSSceneVec3* rotation)
{
    GSRenderCamera* camera;

    camera = (GSRenderCamera*) GSresGetResource(0, 0);
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->rotation, rotation);
    GScameraSetRotation(camera, (const GSRenderVec3*) rotation);
}

void GSscene_GetCameraRotationVector(GSSceneVec3* rotation)
{
    GSvecCopy(rotation, &((CameraPadState*) lbl_80478C40)->rotation);
}

void cameraMoveTarget(void* unused, u32 group, u32 id, f32 duration)
{
#ifdef CAMERA_801765F4_ONLY
    GSSceneVec3 target = lbl_80273DC8;
#else
    GSSceneVec3 target = { 0.0f, 0.0f, 0.0f };
#endif
    void* model;

    cameraSetTarget(group, id);
    model = GSresGetResource(group, id);
    if (model != NULL) {
        GSmodelGetPosition(model, &target);
    }
    cameraMoveTargetPos(NULL, &target, duration);
}

void cameraMoveTargetPos(void* unused, GSSceneVec3* target, f32 duration)
{
    ((CameraPadState*) lbl_80478C40)->flags[0] = 1;
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->targetMoveEnd, target);
    ((CameraPadState*) lbl_80478C40)->targetMoveTime = 0.0f;
    ((CameraPadState*) lbl_80478C40)->targetMoveDuration = duration;
    ((CameraPadState*) lbl_80478C40)->targetMoveActive = 1;
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->targetMoveStart,
              &((CameraPadState*) lbl_80478C40)->position);
}

void cameraMoveTargetOfs(void* unused, GSSceneVec3* offset, f32 duration)
{
    ((CameraPadState*) lbl_80478C40)->flags[0] = 1;
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->targetOffsetMoveEnd, offset);
    ((CameraPadState*) lbl_80478C40)->targetOffsetMoveTime = 0.0f;
    ((CameraPadState*) lbl_80478C40)->targetOffsetMoveDuration = duration;
    ((CameraPadState*) lbl_80478C40)->targetOffsetMoveActive = 1;
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->targetOffsetMoveStart,
              &((CameraPadState*) lbl_80478C40)->view);
}

void cameraMoveTargetXYZ(f32 x, f32 y, f32 z, f32 duration)
{
    GSSceneVec3 target;

    set__5GSvecFfff(&target, x, y, z);
    cameraMoveTargetPos(NULL, &target, duration);
}

void cameraMovePosition(void* unused, GSSceneVec3* position, f32 duration)
{
    ((CameraPadState*) lbl_80478C40)->flags[0] = 1;
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->positionMoveEnd, position);
    ((CameraPadState*) lbl_80478C40)->positionMoveTime = 0.0f;
    ((CameraPadState*) lbl_80478C40)->positionMoveDuration = duration;
    ((CameraPadState*) lbl_80478C40)->positionMoveActive = 1;
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->positionMoveStart,
              &((CameraPadState*) lbl_80478C40)->direction);
}

void cameraMovePositionXYZ(f32 x, f32 y, f32 z, f32 duration)
{
    GSSceneVec3 position;

    set__5GSvecFfff(&position, x, y, z);
    cameraMovePosition(NULL, &position, duration);
}

void cameraMoveRotation(void* unused, GSSceneVec3* rotation, f32 duration)
{
    ((CameraPadState*) lbl_80478C40)->flags[0] = 1;
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->rotationMoveEnd, rotation);
    ((CameraPadState*) lbl_80478C40)->rotationMoveTime = 0.0f;
    ((CameraPadState*) lbl_80478C40)->rotationMoveDuration = duration;
    ((CameraPadState*) lbl_80478C40)->rotationMoveActive = 1;
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->rotationMoveStart,
              &((CameraPadState*) lbl_80478C40)->rotation);
}

void cameraMoveRotationXYZ(f32 x, f32 y, f32 z, f32 duration)
{
    GSSceneVec3 rotation;

    set__5GSvecFfff(&rotation, x, y, z);
    cameraMoveRotation(NULL, &rotation, duration);
}

void cameraReturn(u8 wait, f32 duration)
{
    CameraFloorEntry* defaults;

    GSscene_SetMode(0);
    if (fn_801174C4() != 0) {
        fn_80117500();
        fn_80117330(duration);
    } else {
        defaults = cameraFindFloorEntry(std092_getCurrentRoomID());
        if (defaults != NULL) {
            cameraSetHeight(defaults->defaultHeight);
            cameraSetDistance(defaults->defaultDistance);
            cameraSetFov(defaults->defaultFov);
        }
        cameraMoveTarget(NULL, 0, 100, duration);
        /* Read without the NULL check above, as in retail. */
        cameraMoveRotationXYZ(0.0f, defaults->defaultRotationY, 0.0f, duration);
    }
    cameraMoveEndCheck(wait);
}

u8 cameraMoveEndCheck(u8 wait)
{
    CameraPadState* state;

    for (;;) {
        state = lbl_80478C40;
        if (state->targetMoveActive == 0 &&
            state->targetOffsetMoveActive == 0 &&
            state->positionMoveActive == 0 &&
            state->rotationMoveActive == 0) {
            return 0;
        }
        if (wait != 0) {
            _threadSwitch();
        } else {
            return 1;
        }
    }
}

u8 cameraMoveEndCheckSpecial(u8 wait)
{
    CameraPadState* state;

    for (;;) {
        state = lbl_80478C40;
        if (state->targetMoveActive == 0 &&
            state->positionMoveActive == 0 &&
            state->rotationMoveActive == 0) {
            return 0;
        }
        if (wait != 0) {
            _threadSwitch();
        } else {
            return 1;
        }
    }
}

void cameraMoveStop(void)
{
    ((CameraPadState*) lbl_80478C40)->targetMoveActive = 0;
    ((CameraPadState*) lbl_80478C40)->targetOffsetMoveActive = 0;
    ((CameraPadState*) lbl_80478C40)->positionMoveActive = 0;
    ((CameraPadState*) lbl_80478C40)->rotationMoveActive = 0;
    ((CameraPadState*) lbl_80478C40)->flags[0] = 0;
}

void cameraPlayAnime(u32 groupId, u32 animationId, s32 frame, u8 loop)
{
    CameraPadState* state;
    void* animation;
    u8 previousMode;
    CameraPadState* current;

    state = lbl_80478C40;
    if (state->animationGroup != 0 || state->animationId != 0) {
        animation = GSresGetResource(state->animationGroup, state->animationId);
        if (animation == NULL) {
            animation = fn_800F92D4(state->animationId);
        }
        ((CameraPadState*)lbl_80478C40)->animationGroup = 0;
        ((CameraPadState*)lbl_80478C40)->animationId = 0;
        if (animation != NULL) {
            GScameraStopAnimation(animation);
        }
    }

    previousMode = GSscene_SetMode(4);
    ((CameraPadState*)lbl_80478C40)->animationGroup = groupId;
    ((CameraPadState*)lbl_80478C40)->animationId = animationId;
    ((CameraPadState*)lbl_80478C40)->flags[1] = previousMode;

    current = lbl_80478C40;
    animation = GSresGetResource(current->animationGroup, current->animationId);
    if (animation == NULL) {
        animation = fn_800F92D4(current->animationId);
    }
    if (animation == NULL) {
        return;
    }
    GScameraSetAnimIndex(animation, 0);
    if (loop != 0) {
        fn_800D1858(animation, 1);
    } else {
        fn_800D1858(animation, 0);
    }
    GScameraSetAnimRate(animation, 0.5f);
    GScameraSetAnimFrame(animation, (f32)frame);
    GScameraStartAnimation(animation);
    fn_800D258C(animation);
}

void cameraPlayOffsetAnime(u32 groupId, u32 animationId, s32 frame, u8 loop)
{
    CameraPadState* state;
    void* animation;
    u8 previousMode;
    CameraPadState* current;
    void* camera;

    state = lbl_80478C40;
    if (state->animationGroup != 0 || state->animationId != 0) {
        animation = GSresGetResource(state->animationGroup, state->animationId);
        if (animation == NULL) {
            animation = fn_800F92D4(state->animationId);
        }
        ((CameraPadState*)lbl_80478C40)->animationGroup = 0;
        ((CameraPadState*)lbl_80478C40)->animationId = 0;
        if (animation != NULL) {
            GScameraStopAnimation(animation);
        }
    }

    previousMode = GSscene_SetMode(8);

    ((CameraPadState*)lbl_80478C40)->animationGroup = groupId;
    ((CameraPadState*)lbl_80478C40)->animationId = animationId;
    ((CameraPadState*)lbl_80478C40)->flags[1] = previousMode;

    current = lbl_80478C40;
    animation = GSresGetResource(current->animationGroup, current->animationId);
    if (animation == NULL) {
        animation = fn_800F92D4(current->animationId);
    }
    if (animation == NULL) {
        return;
    }
    GScameraSetAnimIndex(animation, 0);
    if (loop != 0) {
        fn_800D1858(animation, 1);
    } else {
        fn_800D1858(animation, 0);
    }
    GScameraSetAnimRate(animation, 0.5f);
    GScameraSetAnimFrame(animation, (f32)frame);
    GScameraStartAnimation(animation);
    clear__5GSvecFv(&((CameraPadState*)lbl_80478C40)->offsetPosition);
    clear__5GSvecFv(&((CameraPadState*)lbl_80478C40)->offsetRotation);
    set__5GSvecFfff(&((CameraPadState*)lbl_80478C40)->offsetScale,
                    lbl_8047D724, lbl_8047D724, lbl_8047D724);
    camera = GSresGetResource(0, 0);
    fn_800D258C(camera);
}

s32 fn_80176C04(u32 group, u32 id)
{
    GSRenderCamera* animation;

    if (group == 0 || id == 0) {
        return 0;
    }

    animation = (GSRenderCamera*) GSresGetResource(group, id);
    if (animation == NULL) {
        animation = (GSRenderCamera*) fn_800F92D4(id);
    }
    if (animation == NULL) {
        return 0;
    }

    return (s32) GScameraGetAnimFrame(animation);
}

s32 cameraWaitSyncAnime(s32 sync)
{
    GSRenderCamera* animation;

    animation = cameraGetCurrentAnimation();
    if (animation == NULL) {
        return 0;
    }

    if ((u8)sync != 0) {
        for (;;) {
            if (GScameraHasAnimationEnded(animation) != 0) {
                break;
            }
            if (GSthreadGetCurrentThread() == 0) {
#ifdef CAMERA_801765F4_ONLY
                GSlogWrite(lbl_80273F34);
#else
                GSlogWrite("cameraWaitSyncAnime()  スレッドから呼ぶようにしてください\n");
#endif
                break;
            }
            _threadSwitch();
        }
    } else if (GScameraHasAnimationEnded(animation) == 0) {
        return 1;
    }

    return 0;
}

void cameraStopAnime(void* object)
{
    GSRenderCamera* animation;

    animation = cameraGetCurrentAnimation();
    ((CameraPadState*) lbl_80478C40)->animationGroup = 0;
    ((CameraPadState*) lbl_80478C40)->animationId = 0;
    if (animation != NULL) {
        GScameraStopAnimation(animation);
    }
}

void cameraStopAnimation(void)
{
    GSRenderCamera* animation;

    animation = cameraGetCurrentAnimation();
    if (animation != NULL) {

        GScameraStopAnimation(animation);
    }
}

void cameraStartAnimation(void)
{
    GSRenderCamera* animation;

    animation = cameraGetCurrentAnimation();
    if (animation != NULL) {

        GScameraStartAnimation(animation);
    }
}

void cameraSetAnimeRate(f32 rate)
{
    GSRenderCamera* animation;

    animation = cameraGetCurrentAnimation();
    if (animation != NULL) {

        GScameraSetAnimRate(animation, rate);
    }
}

void cameraSetTargetOfsXYZ(f32 x, f32 y, f32 z)
{
    GSSceneVec3 view;

    set__5GSvecFfff(&view, x, y, z);
    GSscene_SetCameraViewVector(&view);
}

void cameraSetTargetPosXYZ(f32 x, f32 y, f32 z)
{
    GSSceneVec3 position;

    set__5GSvecFfff(&position, x, y, z);
    GSscene_SetCameraPositionVector(&position);
}

void std040_cameraSetPosition(f32 x, f32 y, f32 z)
{
    GSSceneVec3 direction;

    set__5GSvecFfff(&direction, x, y, z);
    GSscene_SetCameraDirectionVector(&direction);
}

void cameraSetRotation(f32 x, f32 y, f32 z)
{
    GSSceneVec3 rotation;

    set__5GSvecFfff(&rotation, x, y, z);
    GSscene_SetCameraRotationVector(&rotation);
}

void cameraSetHeight(f32 height)
{
    CameraFloorEntry* floorEntry;

    floorEntry = cameraFindFloorEntry(std092_getCurrentRoomID());
    if (floorEntry != NULL) {
        floorEntry->height = height;
    }
    ((CameraPadState*) lbl_80478C40)->height = height;
}

void cameraSetDistance(f32 distance)
{
    CameraFloorEntry* floorEntry;

    floorEntry = cameraFindFloorEntry(std092_getCurrentRoomID());
    if (floorEntry != NULL) {
        floorEntry->distance = distance;
    }
    ((CameraPadState*) lbl_80478C40)->distance = distance;
}

void cameraSetRotY(f32 angle)
{
    CameraFloorEntry* floorEntry;

    floorEntry = cameraFindFloorEntry(std092_getCurrentRoomID());
    if (floorEntry != NULL) {
        floorEntry->rotationY = angle;
    }
    ((CameraPadState*) lbl_80478C40)->rotation.y = angle;
}

void cameraSetFov(f32 fov)
{
    CameraFloorEntry* floorEntry;

    if (fov < 3.0f) {
        fov = 3.0f;
    }
    if (fov > 120.0f) {
        fov = 120.0f;
    }

    floorEntry = cameraFindFloorEntry(std092_getCurrentRoomID());
    if (floorEntry != NULL) {
        floorEntry->fov = fov;
    }
    ((CameraPadState*) lbl_80478C40)->fov = fov;
}

f32 cameraGetHeight(void)
{
    return ((CameraPadState*) lbl_80478C40)->height;
}

f32 cameraGetDistance(void)
{
    return ((CameraPadState*) lbl_80478C40)->distance;
}

f32 cameraGetRotY(void)
{
    return ((CameraPadState*) lbl_80478C40)->rotation.y;
}

void cameraSetOffsetPosition(GSSceneVec3* position)
{
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->offsetPosition, position);
}

void cameraSetOffsetRotation(GSSceneVec3* rotation)
{
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->offsetRotation, rotation);
}

void cameraSetOffsetScale(GSSceneVec3* scale)
{
    GSvecCopy(&((CameraPadState*) lbl_80478C40)->offsetScale, scale);
}

void fn_801765F4(s32 value)
{
    ((CameraPadState*) lbl_80478C40)->flags[2] = value;
}

/* Defined after their uses so MWCC keeps the names (see the file header). */
const f32 lbl_8047D720 = 30.0f;
const f32 lbl_8047D724 = 1.0f;
