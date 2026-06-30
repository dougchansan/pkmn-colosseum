/*
 * fn_8012BEB4 @ 0x8012BEB4 | gs_field_world.c | size 0x200 (139 inc lines)
 * Transform / delta helper used by fn_8012EBD4.
 *
 * Computes per-frame position deltas for up to 4 tracked objects and
 * writes scaled delta scalars into the caller's output buffer.
 *
 * Arguments:
 *   outBuf   (u8*) — output buffer:
 *                    [0x00..0x0F] 4 × f32 scaled delta scalars
 *                    [0x10..0x3F] 4 × vec3 (f32[3]) raw position records
 *   objHandle(u32) — the "reference" object handle (passed to fn_800E3D98)
 *   srcBuf   (u8*) — input/output: 4 × s32 object IDs (stride 4);
 *                    negative = skip; updated with new y-delta scalars
 *   stateBuf (u8*) — per-player state: 4 × f32 previous y-delta scalars
 *
 * Algorithm (reconstructed from the pseudo-register #else body):
 *   1. Compute frame-time delta:
 *        f30 = (f64)fn_800D3088() - lbl_8047D068  (frame tick bias)
 *   2. Sample the reference object's world position via fn_800E3D98 into
 *      a local buffer (sp+0x24).
 *   3. For each of the 4 slots (i = 0..3):
 *        outBuf[0x10 + i*0xC .. +0x18] = 0.0f  (zero position record)
 *        scalar[i] = 0.0f
 *        id = *(s32*)(srcBuf + i*4)
 *        if id < 0: continue (skip)
 *        fn_800EE150(objHandle, id)  — activate / select the sub-object.
 *        if id != 0:
 *          fn_800EE3BC(objHandle, &curPos, 0, 0) — read sub-object position.
 *          fn_800EE828(prevBuf)                  — read previous position.
 *          outBuf[0x10 + i*0xC] = curPos (x,y,z copied raw)
 *          scalar[i] = curPos.y - prevPos.y
 *   4. For each of the 4 slots:
 *        f3 = stateBuf[i]  (previous scalar)
 *        f1 = scalar[i]    (current scalar)
 *        if f3 == lbl_8047D040 && f1 < lbl_8047D040:
 *          outBuf[i*4] = (f3 - f1) / f30
 *        else:
 *          outBuf[i*4] = 0.0f
 *   5. Update stateBuf[i] = scalar[i] for i = 0..3.
 *
 * ENDIAN-QA: lbl_8047D038 = 0.0f, lbl_8047D040 = the "target" scalar.
 * The frame-tick bias lbl_8047D068 is a f64 constant.
 * r27 in the pseudo-register body is an unresolved callee-save register
 * (likely the stateBuf or a sub-buffer of it); fn_800EE828 is called with
 * it to read the previous position. For the pcport we pass stateBuf.
 */
void fn_8012BEB4(u8* outBuf, u32 objHandle, u8* srcBuf, u8* stateBuf)
{
    extern f32 lbl_8047D038;  /* 0.0f */
    extern f32 lbl_8047D040;  /* target scalar (reference value) */
    extern f64 lbl_8047D068;  /* frame-tick bias */
    extern u32  fn_800D3088(void);
    extern void fn_800E3D98(u32 obj, f32* outVec3);
    extern void fn_800EE150(u32 obj, s32 id);
    extern void fn_800EE3BC(u32 obj, f32* outPos, u32 a, u32 b);
    extern void fn_800EE828(u8* prevBuf);

    f32 curPos[3];   /* current sub-object position (sp+0x18 in original) */
    f32 prevPos[3];  /* previous position read by fn_800EE828 (sp+0x24 area) */
    f32 scalar[4];   /* per-slot y-delta scalars (sp+0x8..0x14 in original) */
    f32 f30;         /* frame-time delta */
    s32 id;
    s32 i;

    /* Step 1: compute frame-time delta */
    f30 = (f32)((f64)fn_800D3088() - lbl_8047D068);

    /* Step 2: sample reference object position (result unused directly;
     * fn_800E3D98 may have side effects / update internal state) */
    fn_800E3D98(objHandle, prevPos);

    /* Step 3: per-slot position sampling */
    for (i = 0; i < 4; i++) {
        u8* slot = outBuf + 0x10 + i * 0xC;

        /* Zero the position record */
        *(f32*)(slot + 0x00) = lbl_8047D038;
        *(f32*)(slot + 0x04) = lbl_8047D038;
        *(f32*)(slot + 0x08) = lbl_8047D038;
        scalar[i] = lbl_8047D038;

        id = *(s32*)(srcBuf + i * 4);
        if (id < 0) {
            continue;
        }

        fn_800EE150(objHandle, id);

        if (id != 0) {
            fn_800EE3BC(objHandle, curPos, 0, 0);
            fn_800EE828(stateBuf);

            /* Copy raw position into outBuf position record */
            *(f32*)(slot + 0x00) = curPos[0];
            *(f32*)(slot + 0x04) = curPos[1];
            *(f32*)(slot + 0x08) = curPos[2];

            /* y-delta: curPos.y - prevPos.y (prevPos.y is at sp+0x28 in original,
             * which is the y component of the reference position sampled in step 2) */
            scalar[i] = curPos[1] - prevPos[1];
        }
    }

    /* Step 4: compute scaled output scalars */
    {
        f32 f2 = lbl_8047D040;
        f32 f0 = lbl_8047D038;

        {
            f32 f3 = *(f32*)(stateBuf + 0x0);
            f32 f1 = scalar[0];
            if (f3 == f2 && f1 < f2) {
                *(f32*)(outBuf + 0x0) = (f3 - f1) / f30;
            } else {
                *(f32*)(outBuf + 0x0) = f0;
            }
        }
        {
            f32 f3 = *(f32*)(stateBuf + 0x4);
            f32 f1 = scalar[1];
            if (f3 == f2 && f1 < f2) {
                *(f32*)(outBuf + 0x4) = (f3 - f1) / f30;
            } else {
                *(f32*)(outBuf + 0x4) = f0;
            }
        }
        {
            f32 f3 = *(f32*)(stateBuf + 0x8);
            f32 f1 = scalar[2];
            if (f3 == f2 && f1 < f2) {
                *(f32*)(outBuf + 0x8) = (f3 - f1) / f30;
            } else {
                *(f32*)(outBuf + 0x8) = f0;
            }
        }
        {
            f32 f3 = *(f32*)(stateBuf + 0xC);
            f32 f1 = scalar[3];
            if (f3 == f2 && f1 < f2) {
                *(f32*)(outBuf + 0xC) = (f3 - f1) / f30;
            } else {
                *(f32*)(outBuf + 0xC) = f0;
            }
        }
    }

    /* Step 5: update stateBuf with new scalars */
    *(f32*)(stateBuf + 0x0) = scalar[0];
    *(f32*)(stateBuf + 0x4) = scalar[1];
    *(f32*)(stateBuf + 0x8) = scalar[2];
    *(f32*)(stateBuf + 0xC) = scalar[3];
}
