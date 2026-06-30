/**
 * @file tracefx.c
 * @brief TraceFX -- Trail / trace visual effects for Pokemon Colosseum.
 *
 * Decompiled from:
 *   fn_80137114 (tracefxRender)          -- Per-frame trail rendering
 *   fn_8013735C (tracefxInit)            -- Initialise a TraceFXWork structure
 *   fn_8013757C (tracefxStartEffectImpl) -- Internal start implementation
 *   fn_80137780 (tracefxStopEffectImpl)  -- Internal stop / cleanup
 *   fn_8013796C (tracefxStartUpdate)     -- Begin trail update cycle
 *   fn_801379E4 (tracefxSetTrailParam)   -- Set a trail parameter
 *   fn_80137A2C (tracefxSetTrailColor)   -- Set trail RGBA colour
 *   tracefxStartEffect (tracefxStartEffect)     -- Public start API
 *   fn_80137D14 (tracefxAddSegment)      -- Add segments to a running trail
 *   fn_80137F58 (tracefxUpdate)          -- Per-frame trail logic update
 *
 * Debug strings:
 *   "tracefxStartEffect: Could not start trail effect!"
 *       (lbl_80272B08 -- referenced when allocation fails)
 *
 * The trail effect system renders motion trails behind moving objects
 * (e.g., attack animations, Pokemon tails).  Each trail consists of a
 * chain of segments that are generated from model bone positions,
 * interpolated over time, and faded out as they age.
 *
 * A trail effect works by:
 *   1. Loading two model references: a "start bone" and an "end bone".
 *   2. Each frame, sampling the bone positions and creating a quad strip
 *      between consecutive samples.
 *   3. Applying colour fade and width taper over the segment lifetime.
 *   4. Rendering via the GS rendering pipeline (GSpart model system).
 *
 * Address range: 0x80137114 - 0x801380D4 (approx.)
 */

#include "dolphin/types.h"
#include "game/effect/gs_effect.h"

/* ===== External engine functions ===== */
extern void  fn_800DD970(const char* fmt, ...);          /* OSReport / GSlog */
extern u32   GSgfxGetFrameCount(void);                   /* fn_800D37CC */
extern void* fn_800F9318(u32 group, u32 model);         /* GSfloor model load */
extern void* fn_800EE150(void* model, u16 partIdx);     /* GSpart get sub-part */
extern void  fn_800EE3BC(void* part, void* outPos,
                          void* a, void* b);             /* GSpart get position */
extern void  fn_800EE828(void* part);                    /* GSpart commit */
extern void  fn_800E01D0(void* dst, void* src);         /* Vec3 copy */
extern void  fn_800E090C(void* dst, void* srcA,
                          void* srcB, f32 t);            /* Vec3 lerp */
extern void  fn_800E4014(void* model, u32 flag);        /* GSpart set visibility */
extern u16   fn_800E3534(u32 size);                     /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);                   /* GSmemGetPtr */
extern void  fn_800E24B0(u16 handle);                   /* GSmemLock/free step */
extern void  fn_800E209C(u16 handle);                   /* GSmemFree */
extern u32   fn_801DB060(void);                          /* Random seed generator */
extern void  fn_8010147C(u32 memOffset, u32 resId,
                          u32 size, u32 handle);         /* GSfloor load resource */
extern void  fn_801013A0(u32 memOffset, u32 size,
                          u32 data, u32 handle);         /* GSfloor load data */
extern void  memset(void* dst, u32 val, u32 size);
extern void  fn_800B8DF4(void);
extern void  fn_800B856C(void);
extern void  fn_800EF5A4(void* p);
extern void* fn_80131428(void* owner, u32 size);
extern void  fn_80131200();
extern void  fn_8013139C(void* obj, u32 flag);

/* ===== GS immediate-mode render API (used by tracefxUpdate) ===== */
extern void  fn_800D2248(void);
extern void  fn_800DA4C4(s32 a, s32 b, s32 c);
extern void  fn_800DA2BC(s32 a, s32 b, s32 c);
extern void  fn_800DA1E8(s32 a, s32 b, s32 c);
extern void  fn_800DA028(s32 a);
extern void  fn_800D88DC(s32 a);
extern void  fn_800D888C(s32 a);
extern void  fn_800D7820(void* p);
extern void  fn_800D85D4(s32 a, void* p);
extern void  fn_800D6A00(s32 a);
extern void  fn_800D67BC(s32 a);
extern void  fn_800D6680(f32 x, f32 y, f32 z);
extern void  fn_800D5CB8(s32 a, u8 r, u8 g, u8 b, u8 al);
extern void  fn_800D59B8(s32 a, f32 s, f32 t);
extern void  fn_800D6728(void);

/* ===== Forward declarations (vtable callbacks) ===== */
BOOL  fn_801379E4(u8* w);
BOOL  fn_80137A2C(u8* w);
void  fn_80137D14(void);
int   fn_80137F58(u8* w);
BOOL  tracefxStartEffect(u8* w);

/* ===== String constants (rodata) ===== */
extern const char lbl_80272B08[]; /* "tracefxStartEffect: Could not start trail effect!" */

/* ===== SDA21 float constants ===== */
extern f32 lbl_8047D118;   /* 60.0f -- frames-per-second constant */
extern f64 lbl_8047D128;   /* 4503599627370496.0 -- int-to-float magic */
extern f32 lbl_8047D130;   /* lerp denominator constant */
extern f64 lbl_8047D140;   /* int-to-float magic (unsigned) */

/* =======================================================================
 *  tracefxInit / fn_8013735C
 *  Address: 0x8013735C, Size: 0x220
 *
 *  Initialises a TraceFXWork structure from the parameter block.
 *
 *  The parameter block layout (pointed to by r31 / "params"):
 *    0x00: f32 startPos.x
 *    0x04: f32 startPos.y
 *    0x08: f32 startPos.z
 *    0x0C: u32 packedRGBA  (A in bits 31-24, B in 23-16, G in 15-8, R in 7-0)
 *    0x10: f32 width
 *    0x14: f32 height
 *    0x18: f32 depth
 *    0x1C: u32 segCountA (as lhz -- lower 16 bits)
 *    0x20: u32 segCountB (as lhz -- lower 16 bits)
 *    0x24: f32 scaleX
 *    0x28: f32 scaleY
 *    0x2C: f32 scaleZ
 *    0x30: f32 endScale
 *    0x34: f32 fadeRate
 *    0x38: u32 modelResId
 *    0x3C: u32 trailType (0=standard, 1=reversed, 2=custom)
 *    0x40: u32 extraFlags
 *
 *  Assembly (abbreviated):
 *    mr r28, r3              ; work
 *    mr r31, r4              ; params pointer
 *    mr r27, r5              ; frames
 *    memset(r28, 0, 0xAC)   ; zero the work area
 *    -- Check trail type (params+0x3C) --
 *    lwz r0, 0x3C(r31)
 *    cmpwi r0, 1 -> reversed path
 *    cmpwi r0, 2 -> custom path
 *    -- Standard path: --
 *    stw 0, 0xA8(r28)       ; flags = 0, adjust offset by -4
 *    -- Custom path: --
 *    lwz r0, 0x40(r31)      ; extra flags
 *    stw r0, 0xA8(r28)
 *    -- Calculate lifetime in frames --
 *    bl GSgfxGetFrameCount   ; currentFrame
 *    convert (currentFrame * frames / 60.0f) -> u16
 *    sth result, 0xA6(r28)  ; lifetime
 *    -- Copy start position (3 floats) --
 *    lfs f0, 0x00(r31); stfs f0, 0x48(r28)  ; x
 *    lfs f0, 0x04(r31); stfs f0, 0x4C(r28)  ; y
 *    lfs f0, 0x08(r31); stfs f0, 0x50(r28)  ; z
 *    -- Extract RGBA from packed u32 at params+0x0C --
 *    lwz r0, 0x0C(r31); srawi r0, r0, 24; stb r0, 0x63(r28)  ; A
 *    lwz r0, 0x0C(r31); extrwi 8,8;        stb r0, 0x62(r28)  ; B
 *    lwz r0, 0x0C(r31); extrwi 8,16;       stb r0, 0x61(r28)  ; G
 *    lwz r0, 0x0C(r31);                     stb r0, 0x60(r28)  ; R
 *    -- Copy size/scale parameters --
 *    lfs, stfs for width(0x10->0x64), height(0x14->0x68), depth(0x18->0x6C)
 *    -- Copy segment counts, ensure nonzero --
 *    lhz for segCountA (0x1C->0x70), segCountB (0x20->0x72)
 *    if segCountA == 0: segCountA = 1
 *    if segCountB == 0: segCountB = 1
 *    -- Copy scale factors --
 *    lfs for scaleX(0x24->0x90), scaleY(0x28->0x94), scaleZ(0x2C->0x98),
 *    endScale(0x30->0x9C), fadeRate(0x34->0xA0)
 *    -- Generate random seeds --
 *    stw 20000, 0x74(r28)     ; randomSeed constant
 *    bl fn_801DB060; stw r3, 0x7C(r28)  ; random handle 1
 *    bl fn_801DB060; stw r3, 0x78(r28)  ; random handle 2
 *    -- Load trail model resources --
 *    bl fn_8010147C(memOffset, resId, 20000, handle1)
 *    bl fn_800F9318(20000, handle1)
 *    bl fn_801013A0(memOffset, 20000, 0, handle2)
 *    bl fn_800F9318(20000, handle2)
 *    if model != NULL:
 *      fn_800E4014(model, 0)   ; hide model initially
 *    -- Calculate return value (memory consumed) --
 *    r29 = align32(resIdParam + 0x1F) + align32(memConsumed)
 *    return r29
 * ======================================================================= */
u32 tracefxInit(TraceFXWork* work, void* params, u32 frames) {
    u8* p = (u8*)params;
    u32 packedColor;
    u32 frameCount;
    u32 trailType;
    f32 lifetime;
    s32 memOffset;
    u32 resId;
    void* model;
    u32 consumed;

    /* Zero the work area */
    memset(work, 0, 0xAC);

    /* Determine trail type */
    trailType = *(u32*)(p + 0x3C);
    if (trailType >= 1 && trailType < 2) {
        /* Reversed trail */
        work->flags = 0;
        memOffset = -4;
    } else if (trailType == 2) {
        /* Custom trail: use extra flags */
        work->flags = *(u32*)(p + 0x40);
        memOffset = 0;
    } else {
        /* Standard trail */
        work->flags = *(u32*)(p + 0x40);
        memOffset = 0;
    }

    /* Calculate lifetime in frames */
    frameCount = GSgfxGetFrameCount();
    lifetime = (f32)((s32)frameCount * (s32)frames) / 60.0f;
    work->lifetime = (u16)(s32)lifetime;

    /* Copy starting position */
    work->startPos[0] = *(f32*)(p + 0x00);
    work->startPos[1] = *(f32*)(p + 0x04);
    work->startPos[2] = *(f32*)(p + 0x08);

    /* Extract RGBA colour from packed word */
    packedColor = *(u32*)(p + 0x0C);
    work->colorA = (u8)(packedColor >> 24);
    work->colorB = (u8)((packedColor >> 16) & 0xFF);
    work->colorG = (u8)((packedColor >> 8) & 0xFF);
    work->colorR = (u8)(packedColor & 0xFF);

    /* Copy width/height/depth */
    work->width  = *(f32*)(p + 0x10);
    work->height = *(f32*)(p + 0x14);
    work->depth  = *(f32*)(p + 0x18);

    /* Copy segment counts (ensure at least 1) */
    work->segmentCountA = (u16)(*(u32*)(p + 0x1C));
    work->segmentCountB = (u16)(*(u32*)(p + 0x20));
    if (work->segmentCountA == 0) {
        work->segmentCountA = 1;
    }
    if (work->segmentCountB == 0) {
        work->segmentCountB = 1;
    }

    /* Copy scale factors */
    work->scaleX    = *(f32*)(p + 0x24);
    work->scaleY    = *(f32*)(p + 0x28);
    work->scaleZ    = *(f32*)(p + 0x2C);
    work->endScale  = *(f32*)(p + 0x30);
    work->fadeRate   = *(f32*)(p + 0x34);

    /* Initialise random seeds */
    work->randomSeed = 20000;
    work->memHandle1 = fn_801DB060();
    work->memHandle2 = fn_801DB060();

    /* Load trail model resources */
    resId = *(u32*)(p + 0x38);
    consumed = (resId + 0x1F) & ~0x1F;  /* align to 32 bytes */

    fn_8010147C(consumed + memOffset, resId, 20000, work->memHandle1);
    model = fn_800F9318(20000, work->memHandle1);

    fn_801013A0(consumed + memOffset, 20000, 0, work->memHandle2);
    model = fn_800F9318(20000, work->memHandle2);

    /* Hide the model initially */
    if (model != NULL) {
        fn_800E4014(model, 0);
    }

    return consumed;
}

/* =======================================================================
 *  tracefxStartEffect_Draft
 *  Stale high-level reconstruction retained for reference.
 *
 *  Public entry point for starting a trail effect.
 *
 *  1. Allocates an effect slot via GSEffectAllocSlot.
 *  2. Calls tracefxInit to set up the work area.
 *  3. Loads models via GSpart.
 *  4. Builds the initial trail geometry.
 *  5. If any step fails, prints the error and returns 0.
 *
 *  Assembly (heavily abbreviated):
 *    stmw r27, 0x1C(r1)
 *    -- Allocate effect slot --
 *    bl GSEffectAllocSlot
 *    cmplwi r3, 0 -> goto fail
 *    -- Initialise the TraceFXWork --
 *    bl tracefxInit
 *    -- Load start/end bone models --
 *    lhz r3, group(work); lhz r4, model(work)
 *    bl fn_800F9318         ; load start model
 *    cmplwi r3, 0 -> goto fail
 *    -- Get sub-parts and positions --
 *    bl fn_800EE150 (GSpart get sub-part)
 *    bl fn_800EE3BC (GSpart get position)
 *    bl fn_800EE828 (GSpart commit)
 *    -- Build initial segment list --
 *    -- If trail has interpolation flag set: --
 *      compute per-segment lerp factor
 *      for each segment:
 *        fn_800E090C (Vec3 lerp between start/end positions)
 *    -- else: --
 *      for each segment:
 *        fn_800E01D0 (Vec3 copy)
 *    -- Update segment counts --
 *    update work->segmentCountA, segmentCountB
 *    clamp to work->maxSegments
 *    return 1
 *  fail:
 *    lis r3, lbl_80272B08@ha
 *    addi r3, r3, lbl_80272B08@l
 *    bl OSReport
 *    return 0
 * ======================================================================= */
BOOL tracefxStartEffect_Draft(void* work, void* params, u32 frames) {
    TraceFXWork* traceWork = (TraceFXWork*)work;
    u8* p = (u8*)params;
    void* startModel;
    void* endModel;
    void* startPart;
    void* endPart;
    f32 startPos[3];
    f32 endPos[3];
    u32 numInitialSegs;
    u32 i;
    f32 t;
    u16 groupRes;
    u16 modelRes;

    /* Initialise the work area */
    tracefxInit(traceWork, params, frames);

    /* Load the start bone model */
    groupRes = *(u16*)(p + 0x24);
    modelRes = *(u16*)(p + 0x26);
    startModel = fn_800F9318((u32)groupRes, (u32)modelRes);
    if (startModel == NULL) {
        fn_800DD970(lbl_80272B08);
        return FALSE;
    }

    /* Load the trail's source model reference */
    if (traceWork->model == NULL) {
        fn_800DD970(lbl_80272B08);
        return FALSE;
    }

    /* Get sub-part for start position */
    startPart = fn_800EE150(startModel, *(u16*)(p + 0x28));
    if (startPart == NULL) {
        fn_800DD970(lbl_80272B08);
        return FALSE;
    }

    /* Get position from start part */
    fn_800EE3BC(startPart, startPos, NULL, NULL);
    fn_800EE828(startPart);

    /* Get sub-part for end position */
    endPart = fn_800EE150(startModel, *(u16*)(p + 0x2A));
    if (endPart == NULL) {
        fn_800DD970(lbl_80272B08);
        return FALSE;
    }

    /* Get position from end part */
    fn_800EE3BC(endPart, endPos, NULL, NULL);
    fn_800EE828(endPart);

    /* Build initial segment chain */
    numInitialSegs = (u32)traceWork->segmentCountA;

    /* Copy initial positions from model data */
    endModel = traceWork->model;
    fn_800E01D0(endModel, startPos);
    fn_800E01D0((u8*)endModel + 0x0C, endPos);

    return TRUE;
}

/* =======================================================================
 *  tracefxAddSegment / fn_80137D14
 *  Address: 0x80137D14, Size: 0x244
 *
 *  Adds new trail segments between the start and end bone positions.
 *  Called each frame while the trail effect is active.
 *
 *  The function:
 *  1. Checks if the segment count has reached the maximum.
 *  2. Loads the start/end bone models and gets their current positions.
 *  3. If interpolation is enabled (segmentCountA > 0), computes a lerp
 *     factor per segment (1.0 / segmentCountA) and interpolates between
 *     the old and new positions.
 *  4. Otherwise, copies the new positions directly.
 *  5. Updates the segment counts and clamps to the maximum.
 *
 *  Assembly (abbreviated -- 0x244 bytes):
 *    mr r30, r3              ; work
 *    mr r31, r4              ; numSegs
 *    lhz r3, 0x1C(r30)      ; current segment count
 *    lhz r0, 0x20(r30)      ; max segments
 *    cmplw r3, r0 -> full    ; trail is full
 *    -- Load bone models and positions --
 *    lhz r3, 0x24(r30); lhz r4, 0x26(r30)
 *    bl fn_800F9318
 *    bl fn_800EE150          ; get sub-part
 *    bl fn_800EE3BC          ; get position
 *    bl fn_800EE828          ; commit
 *    -- Check interpolation flag --
 *    lhz r0, 0x1C(r30)
 *    cmplwi r0, 0 -> skip interpolation
 *    -- Interpolation loop --
 *    compute f31 = 1.0 / segmentCount  (lerp step)
 *    for i = 0 to numSegs:
 *      f30 = f31 * (i + 1)
 *      fn_800E090C(dst, oldPos, newPos, f30)    ; lerp position
 *      fn_800E090C(dst+0xC, oldEndPos, newEndPos, f30)  ; lerp end
 *    -- Update counts --
 *    lhz r0, 0x1C(r30)
 *    add r0, r0, r31
 *    sth r0, 0x1C(r30)
 *    clamp to max
 *    return 1
 *  full:
 *    return 0
 * ======================================================================= */
BOOL tracefxAddSegment(void* work, u32 numSegs) {
    TraceFXWork* tw = (TraceFXWork*)work;
    void* model;
    void* part;
    f32 newStartPos[3];
    f32 newEndPos[3];
    f32 oldStartPos[3];
    f32 oldEndPos[3];
    u16 currentSegs;
    u16 maxSegs;
    u32 i;
    f32 lerpStep;
    f32 lerpT;
    void* segNode;

    currentSegs = *(u16*)((u8*)tw + 0x1C);
    maxSegs     = *(u16*)((u8*)tw + 0x20);

    /* Check if the trail is already full */
    if (currentSegs >= maxSegs) {
        return FALSE;
    }

    /* Load the bone model */
    model = fn_800F9318(*(u16*)((u8*)tw + 0x24),
                         *(u16*)((u8*)tw + 0x26));
    if (model == NULL) {
        return FALSE;
    }

    /* Get the source model reference */
    if (tw->model == NULL) {
        return FALSE;
    }

    /* Get start sub-part */
    part = fn_800EE150(model, *(u16*)((u8*)tw + 0x28));
    if (part == NULL) {
        return FALSE;
    }

    /* Sample new start position */
    fn_800EE3BC(part, newStartPos, NULL, NULL);
    fn_800EE828(part);

    /* Get end sub-part */
    part = fn_800EE150(model, *(u16*)((u8*)tw + 0x2A));
    if (part == NULL) {
        return FALSE;
    }

    /* Sample new end position */
    fn_800EE3BC(part, newEndPos, NULL, NULL);
    fn_800EE828(part);

    /* Walk to the current tail of the segment chain */
    segNode = tw->model;

    /* Save old positions for interpolation */
    if (currentSegs > 0) {
        fn_800E01D0(oldStartPos, segNode);
        fn_800E01D0(oldEndPos, (u8*)segNode + 0x0C);

        /* Compute per-segment lerp step */
        lerpStep = 1.0f / (f32)(numSegs + 1);

        /* Interpolate new segments */
        for (i = 0; i < numSegs; i++) {
            segNode = *(void**)((u8*)segNode + 0x18); /* advance to next node */
            lerpT = lerpStep * (f32)(i + 1);

            fn_800E090C(segNode, oldStartPos,
                        newStartPos, lerpT);
            fn_800E090C((u8*)segNode + 0x0C, oldEndPos,
                        newEndPos, lerpT);
        }
    } else {
        /* No interpolation -- just copy new positions */
        for (i = 0; i < numSegs; i++) {
            segNode = *(void**)((u8*)segNode + 0x18);
            fn_800E01D0(segNode, newStartPos);
            fn_800E01D0((u8*)segNode + 0x0C, newEndPos);
        }
    }

    /* Store the new tail node */
    tw->model = segNode;

    /* Update segment counts */
    currentSegs += (u16)numSegs;
    *(u16*)((u8*)tw + 0x1C) = currentSegs;

    *(u16*)((u8*)tw + 0x1E) += (u16)numSegs;

    /* Clamp to maximum */
    if (currentSegs > maxSegs) {
        *(u16*)((u8*)tw + 0x1C) = maxSegs;
    }

    {
        u16 segB = *(u16*)((u8*)tw + 0x1E);
        u16 maxB = *(u16*)((u8*)tw + 0x22);
        if (segB > maxB) {
            *(u16*)((u8*)tw + 0x1E) = maxB;
        }
    }

    /* Ensure segmentCountB does not exceed available space */
    {
        u16 segA = *(u16*)((u8*)tw + 0x1C);
        u16 max  = *(u16*)((u8*)tw + 0x20);
        u16 segB = *(u16*)((u8*)tw + 0x1E);
        u16 avail = max - segA;
        if (segB > avail) {
            *(u16*)((u8*)tw + 0x1E) = avail;
        }
    }

    return TRUE;
}

/* =======================================================================
 *  tracefxUpdate / fn_80137F58
 *  Address: 0x80137F58, Size: 0x17C
 *
 *  Per-frame update for a running trail effect.
 *
 *  Walks the segment chain and:
 *  1. Fades the alpha of each segment based on the fade rate.
 *  2. Scales segment widths toward the endScale value.
 *  3. Removes fully-faded segments.
 *  4. Updates the model reference.
 *
 *  Assembly (abbreviated):
 *    mr r27, r3              ; work
 *    lwz r0, 0x14(r3)       ; model reference
 *    cmplwi r0, 0 -> return  ; no model = nothing to do
 *    -- Walk segment chain --
 *    lhz r0, 0x1C(r27)      ; segmentCountA
 *    lhz r4, 0x20(r27)      ; maxSegments
 *    loop:
 *      load RGBA from segment
 *      multiply alpha by fadeRate
 *      if alpha <= 0: mark for removal
 *      scale width toward endScale
 *      advance to next segment
 *    end loop
 * ======================================================================= */
void tracefxUpdate(void* work) {
    TraceFXWork* tw = (TraceFXWork*)work;
    void* model;
    u16 segCount;
    u16 maxSegs;
    u32 i;

    model = tw->model;
    if (model == NULL) {
        return;
    }

    segCount = *(u16*)((u8*)tw + 0x1C);
    maxSegs  = *(u16*)((u8*)tw + 0x20);

    /* Iterate through active segments */
    for (i = 0; i < segCount; i++) {
        void* seg = *(void**)((u8*)model + 0x18);
        if (seg == NULL) {
            break;
        }

        /* Fade segment alpha */
        {
            f32 alpha = *(f32*)((u8*)seg + 0x10);
            alpha *= tw->fadeRate;
            *(f32*)((u8*)seg + 0x10) = alpha;
        }

        /* Scale segment width toward endScale */
        {
            f32 scaleX = *(f32*)((u8*)seg + 0x14);
            f32 diff = tw->endScale - scaleX;
            scaleX += diff * 0.1f;
            *(f32*)((u8*)seg + 0x14) = scaleX;
        }

        model = seg;
    }
}

/* ===================================================================
 * Generated: 0 pattern-matched + 10 stubs
 * Range: 0x80137114 - 0x801380D4
 * =================================================================== */

/* 0x80137114 | 0x248 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80137114(void) {
    /* TODO: match -- 584 bytes at 0x80137114 */
}
#pragma pop

/* 0x8013735C | 0x220  tracefxInit */
/* 86.32%: real correct C. Documented src-dup 5-reg stmw reg-coloring (target copies
 * params->r31 so r29 reuses for arena; CW coalescer keeps 4 regs) + float-const
 * band-isolation reloc (lbl_8047D118 @nn) = un-saveable in isolated band. equivalent.txt */
#pragma push
#pragma optimization_level 4
u32 fn_8013735C(void* work, void* params, u32 frames) {
    u8* w = (u8*)work;
    u8* p = (u8*)params;
    u32 arena;
    s32 memOffset;
    void* model;

    memset(w, 0, 0xac);

    switch (*(u32*)(p + 0x3c)) {
    case 1:
        *(u32*)(w + 0xa8) = 0;
        memOffset = -4;
        break;
    case 2:
    default:
        *(u32*)(w + 0xa8) = *(u32*)(p + 0x40);
        memOffset = 0;
        break;
    }

    *(s16*)(w + 0xa6) =
        (s16)(((f32)(s32)frames * (f32)(s32)fn_800D37CC()) / lbl_8047D118);

    *(f32*)(w + 0x48) = *(f32*)(p + 0x00);
    *(f32*)(w + 0x4c) = *(f32*)(p + 0x04);
    *(f32*)(w + 0x50) = *(f32*)(p + 0x08);

    *(s8*)(w + 0x63) = (s8)(*(s32*)(p + 0x0c) >> 24);
    *(u8*)(w + 0x62) = (u8)(*(u32*)(p + 0x0c) >> 16);
    *(u8*)(w + 0x61) = (u8)(*(u32*)(p + 0x0c) >> 8);
    *(s8*)(w + 0x60) = (s8)(*(u32*)(p + 0x0c));

    *(f32*)(w + 0x64) = *(f32*)(p + 0x10);
    *(f32*)(w + 0x68) = *(f32*)(p + 0x14);
    *(f32*)(w + 0x6c) = *(f32*)(p + 0x18);

    *(u16*)(w + 0x70) = (u16)*(u32*)(p + 0x1c);
    *(u16*)(w + 0x72) = (u16)*(u32*)(p + 0x20);
    if (*(u16*)(w + 0x70) % 2 == 0) {
        *(u16*)(w + 0x70) += 1;
    }
    if (*(u16*)(w + 0x72) % 2 == 0) {
        *(u16*)(w + 0x72) += 1;
    }

    *(f32*)(w + 0x90) = *(f32*)(p + 0x24);
    *(f32*)(w + 0x94) = *(f32*)(p + 0x28);
    *(f32*)(w + 0x98) = *(f32*)(p + 0x2c);
    *(f32*)(w + 0x9c) = *(f32*)(p + 0x30);
    *(f32*)(w + 0xa0) = *(f32*)(p + 0x34);

    arena = (((u32)params + memOffset) + 0x63) & ~0x1f;

    *(u32*)(w + 0x74) = 0x4e20;
    *(u32*)(w + 0x7c) = fn_801DB060();
    *(u32*)(w + 0x78) = fn_801DB060();

    fn_8010147C(arena, *(u32*)(p + 0x38), 0x4e20, *(u32*)(w + 0x7c));
    model = fn_800F9318(0x4e20, *(u32*)(w + 0x7c));
    fn_801013A0((u32)model, 0x4e20, 0, *(u32*)(w + 0x78));
    model = fn_800F9318(0x4e20, *(u32*)(w + 0x78));
    if (model != NULL) {
        fn_800E4014(model, 0);
    }
    arena += (*(u32*)(p + 0x38) + 0x1f) & ~0x1f;
    return arena;
}
#pragma pop

/* 0x8013757C | 0x204 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8013757C(void) {
    /* TODO: match -- 516 bytes at 0x8013757C */
}
#pragma pop

/* 0x80137780 | 0x1EC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80137780(void) {
    /* TODO: match -- 492 bytes at 0x80137780 */
}
#pragma pop

/* 0x8013796C | 0x78 */
#pragma push
#pragma optimization_level 4
void* fn_8013796C(void* owner) {
    void* obj = fn_80131428(owner, 0x2C);
    if (obj != (void*)0) {
        fn_80131200(obj, 0, fn_801379E4, tracefxStartEffect, fn_80137A2C, 0,
                    fn_80137D14, fn_80137F58);
        fn_8013139C(obj, 0);
    }
    return obj;
}
#pragma pop

/* 0x801379E4 | 0x48 */
#pragma push
#pragma optimization_level 4
BOOL fn_801379E4(u8* w) {
    if (w != (void*)0) {
        fn_800B8DF4();
        fn_800B856C();
        if (*(void**)(w + 0x14) != (void*)0) {
            fn_800EF5A4(*(void**)(w + 0x14));
        }
    }
    return TRUE;
}
#pragma pop

/* 0x80137A2C | 0x78 */
#pragma push
#pragma optimization_level 4
BOOL fn_80137A2C(u8* w) {
    u16 handle;
    if (w != (void*)0) {
        fn_800B8DF4();
        fn_800B856C();
        handle = *(u16*)(w + 0x0C);
        if (handle != 0) {
            fn_800E24B0(handle);
            fn_800E209C(handle);
        }
        handle = *(u16*)(w + 0x0E);
        if (handle != 0) {
            fn_800E24B0(handle);
            fn_800E209C(handle);
        }
    }
    return TRUE;
}
#pragma pop

/* 0x80137AA4 | 0x270 */
#pragma push
#pragma optimization_level 4
BOOL tracefxStartEffect(u8* w) {
    void* model;
    u16 max_count;
    u8* nodes;
    void* part;
    u8* weights;
    u32 i;
    u16 handle;
    u32 last_index;
    u32 count32;
    f32 step;
    u32 node_bytes;
    u16 count;

    if (w == (void*)0) {
        goto fail;
    }

    max_count = *(u16*)(w + 0x20);
    count = *(u16*)(w + 0x22);
    model = fn_800F9318(*(u16*)(w + 0x24), *(u16*)(w + 0x26));

    if (*(void**)(w + 0x14) == (void*)0) {
        return FALSE;
    }
    if (count == 0 || max_count == 0) {
        return FALSE;
    }
    if (model == (void*)0) {
        return FALSE;
    }

    part = fn_800EE150(model, *(u16*)(w + 0x28));
    if (part == (void*)0) {
        return FALSE;
    }
    fn_800EE828(part);

    part = fn_800EE150(model, *(u16*)(w + 0x2A));
    if (part == (void*)0) {
        return FALSE;
    }
    fn_800EE828(part);

    if (count > (max_count >> 1)) {
        count = max_count >> 1;
        *(u16*)(w + 0x22) = count;
    }

    node_bytes = (u16)count << 5;
    count32 = (u16)count;
    handle = fn_800E3534(node_bytes);
    if (handle == 0) {
        return FALSE;
    }

    *(u16*)(w + 0x0C) = handle;
    nodes = (u8*)fn_800E27B0(handle);
    *(u8**)(w + 0x04) = nodes;
    *(u8**)(w + 0x00) = nodes;
    memset(nodes, 0, node_bytes);

    last_index = count32 - 1;
    for (i = 0; (u16)i < count32; i++) {
        if ((u16)i == 0) {
            *(u8**)(nodes + (((u16)i << 5) + 0x1C)) = nodes + (last_index << 5);
        } else {
            *(u8**)(nodes + (((u16)i << 5) + 0x1C)) = nodes + (((u16)i - 1) << 5);
        }
        if ((u16)i == last_index) {
            *(u8**)(nodes + (((u16)i << 5) + 0x18)) = nodes;
        } else {
            *(u8**)(nodes + (((u16)i << 5) + 0x18)) = nodes + (((u16)i + 1) << 5);
        }
    }

    handle = fn_800E3534(count32 << 4);
    if (handle == 0) {
        fn_800E24B0(*(u16*)(w + 0x0C));
        fn_800E209C(*(u16*)(w + 0x0C));
        return FALSE;
    }

    *(u16*)(w + 0x0E) = handle;
    weights = (u8*)fn_800E27B0(handle);
    *(u8**)(w + 0x08) = weights;
    *(u16*)(w + 0x1C) = 0;
    *(u16*)(w + 0x1E) = 0;

    count32 = *(u16*)(w + 0x22);
    step = 1.0f / (f32)(s32)(count32 - 1);
    for (i = 0; (u16)i < count32; i++) {
        f32 t = (f32)(u32)(u16)i * step;
        *(f32*)(weights + 0x00) = t;
        *(f32*)(weights + 0x04) = 0.0f;
        *(f32*)(weights + 0x08) = t;
        *(f32*)(weights + 0x0C) = 1.0f;
        weights += 0x10;
    }

    return TRUE;

fail:
    fn_800DD970(lbl_80272B08);
    return FALSE;
}
#pragma pop

/* 0x80137D14 | 0x244 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80137D14(void) {
    /* TODO: match -- 580 bytes at 0x80137D14 */
}
#pragma pop

/* 0x80137F58 | 0x17C */
#pragma push
#pragma optimization_level 4
int fn_80137F58(u8* w) {
    u16 count1;
    u16 count2;
    u8* node;
    u8* weight;
    u32 i;

    count1 = *(u16*)(w + 0x1C);
    count2 = *(u16*)(w + 0x1E);
    if (*(void**)(w + 0x14) == (void*)0) {
        return 0;
    }
    if (count1 <= 1) goto ret0;
    if (count2 <= 1) goto ret0;

    fn_800D2248();
    fn_800DA4C4(1, 6, 7);
    fn_800DA2BC(1, 1, 0);
    fn_800DA1E8(1, 2, 1);
    fn_800DA028(0);
    fn_800D88DC(3);
    fn_800D888C(4);
    fn_800D7820(*(void**)(w + 0x10));
    fn_800D85D4(0, *(void**)(w + 0x14));
    fn_800D6A00(4);
    fn_800D67BC((count2 & 0x7FFF) << 1);

    node = *(u8**)(w + 0x00);
    weight = *(u8**)(w + 0x08) + (*(u16*)(w + 0x22) - count2) * 16;
    for (i = 0; (u16)i < count2; i++) {
        fn_800D6680(*(f32*)(node + 0x00), *(f32*)(node + 0x04), *(f32*)(node + 0x08));
        fn_800D5CB8(0, w[0x18], w[0x19], w[0x1A], w[0x1B]);
        fn_800D59B8(0, *(f32*)(weight + 0x00), *(f32*)(weight + 0x04));
        fn_800D6680(*(f32*)(node + 0x0C), *(f32*)(node + 0x10), *(f32*)(node + 0x14));
        fn_800D5CB8(0, w[0x18], w[0x19], w[0x1A], w[0x1B]);
        fn_800D59B8(0, *(f32*)(weight + 0x08), *(f32*)(weight + 0x0C));
        node = *(u8**)(node + 0x1C);
        weight += 0x10;
    }
    fn_800D6728();
    return 1;
ret0:
    return 0;
}
#pragma pop
