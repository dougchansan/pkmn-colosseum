/*
 * fn_8012E7B8 -- per-player field "delta / turn-amount" computation.
 *
 * Called once per frame from fn_8012EBD4 as `delta = fn_8012E7B8(player_idx)`.
 *
 * Behaviour reconstructed from gs_field_world_fn_8012E7B8.inc:
 *   1. If both pad buttons in mask 0x100 are held (pressed & held) AND the
 *      per-player gate fn_8012C0B4(playerIdx) is set, bail out returning 0.0f.
 *   2. Resolve the player's tracked object handle via a 2-entry selector
 *      array {lbl_8047D030, lbl_8047D034} indexed by playerIdx, then read its
 *      world position (fn_800F9318 -> object, fn_800E3D98 -> vec3).
 *   3. fn_8012E388 fills an aux scalar (sp+0x0C) for this player.
 *   4. Compute the per-frame motion delta vector (fn_800A3A9C diff), scale it
 *      by lbl_8047D080 / (frameTick - lbl_8047D068) using the engine tick from
 *      fn_800D3088, and feed it to fn_801887D8 which returns a turn magnitude.
 *   5. Clamp / select the turn magnitude against lbl_8047D0D0 and the aux
 *      scalar, then drive fn_8012C660 (turn-zone path selector).
 *   6. Maintain a 20-slot ring buffer of recent positions in the global field
 *      state lbl_80426BD0 (write index at +0x44, fill count at +0x48, vec3
 *      slots starting at +0x4C, stride 0xC): a new sample is appended only
 *      when it is far enough (sqrt distance > lbl_8047D0D4) from the previous.
 *   7. Returns the scalar produced by fn_800A3C00 over the two endpoint
 *      vectors (f1 at the normal exit), or 0.0f on the early-out path.
 *
 * The 0x43300000 magic-double int->float trick is replaced by a direct cast,
 * and the frsqrte + Newton-Raphson refinement blocks are replaced by sqrtf().
 */
f32 fn_8012E7B8(u32 playerIdx) {
    /* sdata2 float/double constants and the sdata clamp value */
    extern f32 lbl_8047D030;   /* selector[0] */
    extern f32 lbl_8047D034;   /* selector[1] */
    extern f32 lbl_8047D038;   /* 0.0f sentinel / epsilon base */
    extern f64 lbl_8047D068;   /* tick bias (subtracted from frame tick) */
    extern f32 lbl_8047D080;   /* scale numerator */
    extern f32 lbl_8047D0D0;   /* turn-magnitude lower clamp */
    extern f32 lbl_8047D0D4;   /* ring-buffer min-distance threshold */
    extern u8 lbl_80478AC0[];  /* canonical; per-site reinterpret cast */

    /* global field state block (.bss, size 0x420) */
    extern u8 lbl_80426BD0[];

    /* pad state */
    extern u32 fn_800F7AF0(s32 port);   /* buttons pressed-this-frame mask */
    extern u32 fn_800F7BC4(s32 port);   /* buttons held mask */
    extern u32 fn_8012C0B4(u32 playerIdx);

    /* object resolve / vector math */
    extern u8*  fn_800F9318(u32 a, u32 handle);          /* resolve object */
    extern void fn_800E3D98(u8* obj, f32* outVec3);      /* read object world pos */
    extern void fn_8012E388(u32 playerIdx, f32* outAux); /* aux scalar (sp+0xC) */
    extern void fn_800A3A9C(f32* a, f32* b, f32* outDiff);/* vec diff */
    extern u32  fn_800D3088(void);                       /* engine frame tick */
    extern void fn_800A3AC0(f32* ioVec, f32* ioVec2, f32 scale); /* vec scale */
    extern f32  fn_801887D8(u32 a, u8* obj, f32* vec);   /* turn magnitude */
    extern void fn_8012C660(void* obj, s32 playerIdx, f32 turnAmount);
    extern f32  fn_800A3C00(f32* a, f32* b);             /* endpoint scalar */

    /* fixed 2-entry selector table built on the stack in the original */
    f32   selector[2];
    u32   handle;
    u8*   obj;

    f32   curPos[3];     /* sp+0x68 */
    f32   prevPos[3];    /* sp+0x5c */
    f32   deltaVec[3];   /* sp+0x50 */
    f32   aux;           /* sp+0x0c, from fn_8012E388 */
    f32   scale;
    f32   turnMag;       /* f31 */
    f32   tick;
    f32   result;        /* f1 at exit */

    u32   buttonsPressed;
    u32   buttonsHeld;

    selector[0] = lbl_8047D030;
    selector[1] = lbl_8047D034;

    /* --- early-out gate: both buttons in 0x100 held AND per-player flag set --- */
    buttonsPressed = fn_800F7AF0(1);
    buttonsHeld    = fn_800F7BC4(1);
    if (((buttonsHeld & buttonsPressed) & 0x00000100u) != 0) {
        if ((fn_8012C0B4(playerIdx) & 0xFF) != 0) {
            return lbl_8047D038; /* 0.0f */
        }
    }

    /* --- resolve current object position --- */
    handle = 0;
    if ((s32)playerIdx >= 0 && (s32)playerIdx < 2)
        handle = *(u32*)((u8*)selector + playerIdx * 4);
    obj = fn_800F9318(0, handle);
    fn_800E3D98(obj, curPos);

    /* aux scalar for this player */
    fn_8012E388(playerIdx, &aux);

    /* previous object position (same selector resolves prev-frame slot) */
    handle = 0;
    if ((s32)playerIdx >= 0 && (s32)playerIdx < 2)
        handle = *(u32*)((u8*)selector + playerIdx * 4);
    obj = fn_800F9318(0, handle);
    fn_800E3D98(obj, prevPos);

    /* deltaVec = diff(prevPos, curPos) */
    fn_800A3A9C(prevPos, curPos, deltaVec);

    /* scale = lbl_8047D080 / (frameTick - lbl_8047D068) */
    tick  = (f32)((f64)fn_800D3088() - lbl_8047D068); /* ENDIAN-QA: was 0x43300000 magic-double cast */
    scale = lbl_8047D080 / tick;
    fn_800A3AC0(deltaVec, deltaVec, scale);

    /* turn magnitude from the scaled delta against the third resolved object */
    handle = 0;
    if ((s32)playerIdx >= 0 && (s32)playerIdx < 2)
        handle = *(u32*)((u8*)selector + playerIdx * 4);
    obj     = fn_800F9318(0, handle);
    turnMag = fn_801887D8(0, obj, deltaVec);

    /* clamp/select turnMag: below floor -> if aux>=floor keep aux else use floor */
    if (turnMag < lbl_8047D0D0) {
        if (aux >= lbl_8047D0D0)
            turnMag = aux;
        else
            turnMag = lbl_8047D0D0;
    }
    /* else turnMag already holds the computed (non-clamped) magnitude */

    /* drive turn-zone path selector for this player */
    handle = 0;
    if ((s32)playerIdx >= 0 && (s32)playerIdx < 2)
        handle = *(u32*)((u8*)selector + playerIdx * 4);
    obj = fn_800F9318(0, handle);
    fn_8012C660(0, (s32)playerIdx, turnMag);

    /* --- ring-buffer of recent positions in the global field state ---
     * Layout in lbl_80426BD0:
     *   +0x00 : current player/object index (u32)
     *   +0x44 : ring write index (u32, wraps at 20)
     *   +0x48 : ring fill count   (u32, saturates at 20)
     *   +0x4C : vec3 slots, stride 0xC
     */
    {
        u8*  fs        = lbl_80426BD0;
        u32  ringIndex = *(u32*)(fs + 0x00);   /* index for the sampled vector */
        f32  sampPos[3];                       /* sp+0x44, the just-read sample */
        f32  prevSlot[3];                       /* sp+0x38, previous ring entry */
        u32  fillCount = *(u32*)(fs + 0x48);
        s32  appendNew = 1;                    /* r31: default append unless too close */
        u32  selHandle;

        /* resolve and read the sample position to push */
        selHandle = 0;
        if ((s32)ringIndex >= 0 && (s32)ringIndex < 2)
            selHandle = *(u32*)((u8*)selector + ringIndex * 4);
        obj = fn_800F9318(0, selHandle);
        fn_800E3D98(obj, sampPos);

        if ((s32)fillCount > 0) {
            /* previous ring slot = (writeIndex - 1), wrapping to 19 */
            s32 prevIdx = (s32)*(u32*)(fs + 0x44) - 1;
            if (prevIdx < 0)
                prevIdx += 20;
            {
                u8* prevEntry = fs + (u32)prevIdx * 0xC;
                prevSlot[0] = *(f32*)(prevEntry + 0x4C);
                prevSlot[1] = *(f32*)(prevEntry + 0x50);
                prevSlot[2] = *(f32*)(prevEntry + 0x54);
            }

            {
                f32 dx   = prevSlot[0] - sampPos[0];
                f32 dz   = prevSlot[2] - sampPos[2];
                f32 dist = dx * dx + dz * dz;

                if (dist > lbl_8047D038) {
                    dist = sqrtf(dist); /* frsqrte + 3x Newton-Raphson in original */
                } else if (dist < lbl_8047D038) {
                    dist = *(f32*)lbl_80478AC0;
                }
                /* (==0 / NaN handling collapses to the same value) */

                appendNew = (dist > lbl_8047D0D4) ? 1 : 0;
            }
        }
        /* fillCount == 0 -> always append the first sample */

        if (appendNew & 0xFF) {
            u32 w = *(u32*)(fs + 0x44);
            u8* slot = fs + w * 0xC;
            slot[0x4C/1] = 0; /* placeholder removed below */
            *(f32*)(slot + 0x4C) = sampPos[0];
            *(f32*)(slot + 0x50) = sampPos[1];
            *(f32*)(slot + 0x54) = sampPos[2];

            w++;
            if ((s32)w >= 20)
                w = 0;
            *(u32*)(fs + 0x44) = w;

            if ((s32)fillCount < 20)
                *(u32*)(fs + 0x48) = fillCount + 1;
        }
    }

    /* endpoint scalar over the two captured world positions */
    result = fn_800A3C00(curPos, prevPos);
    return result;
}
