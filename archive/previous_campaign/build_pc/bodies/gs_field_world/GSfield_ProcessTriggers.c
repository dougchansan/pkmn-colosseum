/* GSfield_ProcessTriggers (fn_8012CB..) -- field movement / heading processor.
 *
 * Reconstructed from the pseudo-register transcription at gs_field_world.c:3544.
 * The transcription is the byte-faithful asm rendering of an algorithm that is
 * structurally identical to the already-completed functional decomp fn_8012CA84
 * (gs_field_world.c:26250), so this is mapped onto that proven shape.
 *
 * Entry pseudo-regs: r23=r3=playerIdx, r24=r4=dirVec (analog/desired move vector,
 * x at +0x0, z at +0x8), r25=r5=fwdVec (current facing/forward vector, x at +0x0,
 * z at +0x8). Result turn amount (f28) is fed to fn_8012C660 at the tail.
 *
 * lbl_8047D030/34 hold the two player object-handle words (read as raw u32 bits
 * into a 2-entry table indexed by player). lbl_80426BD0 is the per-player field
 * state array (stride 0x20): +0x0 = global "face-target" player index, +0x4 = u16
 * flags (bit0 = has-target), +0xC = u32 mode. The inline frsqrte + 3x
 * Newton-Raphson sqrt approximations are replaced by sqrtf (x86-equivalent).
 */
void GSfield_ProcessTriggers(s32 playerIdx, f32* dirVec, f32* fwdVec) {
    extern u8   lbl_80478AC0[];     /* sdata 0.0f constant */
    extern f32  lbl_8047D030;       /* player[0] object-handle word (raw bits) */
    extern f32  lbl_8047D034;       /* player[1] object-handle word (raw bits) */
    extern f32  lbl_8047D038;       /* 0.0f */
    extern f32  lbl_8047D060;       /* small-magnitude (facing) threshold */
    extern f32  lbl_8047D080;       /* 1.0f (speed clamp / face-target speed) */
    extern f32  lbl_8047D094;       /* TWO_PI */
    extern f32  lbl_8047D098;       /* angle-wrap lower bound (~ -TWO_PI edge) */
    extern f32  lbl_8047D09C;       /* -PI */
    extern f32  lbl_8047D0A0;       /* +PI */
    extern f32  lbl_8047D0A4;       /* minimum turn threshold */
    extern u32  fn_800D3088(void);                 /* frame / vsync counter */
    extern void fn_800A3A9C(void* a, void* b, void* out);  /* VECSubtract(a-b -> out) */
    extern void fn_800A3AC0(void* in, void* out, f32 s);   /* VECScale(in*s -> out) */
    extern f32  fn_800A3B7C(void* a, void* b);             /* VECDotProduct(a,b) */
    extern f64  fn_800CE2D8(f32 y, f32 x);                 /* atan2f(y, x) */
    extern void fn_800E3D6C(void* obj, void* outRot);      /* getRotation(obj -> out[3]) */
    extern void fn_800E3D98(void* obj, void* outPos);      /* getPosition(obj -> out[3]) */
    extern void* fn_800F9318(u32 group, u32 handle);       /* resolveHandle(group,id) -> obj */
    extern void fn_8012C660(void* obj, s32 idx, f32 amt);  /* applyTurnResult(obj,idx,amt) */
    extern void fn_8018790C(u32 group, u32 handle);        /* stopMovement(grp,hdl) */
    extern void fn_8018805C(u32 group, u32 handle, f32 angle, f32 speed); /* setHeading */
    extern void fn_801885C4(u32 group, u32 handle, f32* dir, u32 flags);  /* setMoveDirection */
    extern f32  fn_801887D8(u32 group, u32 handle, f32* dir);             /* computeTurnAmount */
    extern void fn_8018D998(u32 group, u32 handle);        /* selectEntity(grp,hdl) */
    extern void* fn_8018D928(void);                        /* getSelectedEntityData() */
    extern void* fn_8018F6F4(s32 param);                   /* getAngleConfig(param) -> obj */
    extern f32  fn_8018F678(void* obj);                    /* getMaxTurnRate (+ direction) */
    extern f32  fn_8018F658(void* obj);                    /* getMaxTurnRate (- direction) */
    extern u8   lbl_80426BD0[];                            /* per-player field state, stride 0x20 */
    extern f32  sqrtf(f32 x);                              /* host CRT */

    u32  htbl[2];
    u32  entityHandle = 0u;
    u32  finalHandle  = 0u;
    f32  turnAmount   = 0.0f;
    f32  frameTime;
    f32  dirMag;
    void* obj;
    f32  posA[3];
    f32  posB[3];
    f32  diffVec[3];
    f32  scaledDir[3];
    f32  targetPos[3];
    f32  playerPos[3];
    f32  rotation[3];

    frameTime = (f32)(u32)fn_800D3088();

    /* default entity handle for this player */
    htbl[0] = *(u32*)&lbl_8047D030;
    htbl[1] = *(u32*)&lbl_8047D034;
    if (playerIdx >= 0 && playerIdx < 2) {
        entityHandle = htbl[playerIdx];
    }

    /* magnitude of the desired move vector in the XZ plane */
    {
        f32 sq = dirVec[0] * dirVec[0] + dirVec[2] * dirVec[2];
        dirMag = (sq > lbl_8047D038) ? sqrtf(sq) : *(f32*)lbl_80478AC0;
    }

    if (dirMag > lbl_8047D038) {
        /* ============ MOVING: direction-driven heading ============ */
        f32 clampedSpeed;
        u32 h = 0u;

        htbl[0] = *(u32*)&lbl_8047D030;
        htbl[1] = *(u32*)&lbl_8047D034;
        if (playerIdx >= 0 && playerIdx < 2) {
            h = htbl[playerIdx];
        }
        obj = fn_800F9318(0u, h);
        fn_800E3D98(obj, posA);
        posA[1] = lbl_8047D038;

        fn_801885C4(0u, entityHandle, dirVec, 0u);

        h = 0u;
        htbl[0] = *(u32*)&lbl_8047D030;
        htbl[1] = *(u32*)&lbl_8047D034;
        if (playerIdx >= 0 && playerIdx < 2) {
            h = htbl[playerIdx];
        }
        obj = fn_800F9318(0u, h);
        fn_800E3D98(obj, posB);
        posB[1] = lbl_8047D038;

        fn_800A3A9C(posB, posA, diffVec);

        clampedSpeed = dirMag / frameTime;
        if (clampedSpeed > lbl_8047D080) {
            clampedSpeed = lbl_8047D080;
        }

        fn_800A3AC0(diffVec, scaledDir, clampedSpeed / dirMag);

        {
            f32 fwdSq  = fwdVec[0] * fwdVec[0] + fwdVec[2] * fwdVec[2];
            f32 fwdMag = (fwdSq > lbl_8047D038) ? sqrtf(fwdSq) : *(f32*)lbl_80478AC0;

            if (fwdMag > lbl_8047D060) {
                f32 angle = (f32)fn_800CE2D8(fwdVec[0], fwdVec[2]);
                fn_8018805C(0u, entityHandle, angle, clampedSpeed);
            }
        }

        turnAmount = fn_801887D8(0u, entityHandle, scaledDir);

        if (fn_800A3B7C(diffVec, fwdVec) < lbl_8047D038) {
            turnAmount = -turnAmount;
        }

    } else {
        /* ============ STATIONARY: face-target or idle ============ */
        u8  hasTarget = 0u;
        u32 mode;

        if (playerIdx >= 0 && playerIdx < 2) {
            /* ENDIAN-QA: u16 half-read of the per-player flags field */
            u16 flags = *(u16*)(&lbl_80426BD0[playerIdx * 0x20] + 0x4);
            hasTarget = (u8)(flags & 0x1);
        }

        if (playerIdx == 2) {
            mode = 2u;
        } else {
            mode = hasTarget
                 ? *(u32*)(&lbl_80426BD0[playerIdx * 0x20] + 0xC)
                 : 2u;
        }

        if (mode != 1u) {
            /* no face-target behavior -> halt and report zero turn */
            fn_8018790C(0u, entityHandle);
            turnAmount = lbl_8047D038;
        } else {
            /* ---- FACE-TARGET: rotate toward the global target entity ---- */
            s32  targetIdx;
            u32  tgtH = 0u, plrH = 0u, rotH = 0u, turnH = 0u;
            f32  heading, targetAngle, angleDiff;
            f32  newAngle = 0.0f;
            f32  maxPos, maxNeg;
            void* angleObj;
            void* tPtr;
            s32  turnParam;
            u8   shouldTurn = 0u;
            u8   turnValid  = 0u;

            /* global "who to face" player index lives at the table base */
            targetIdx = *(s32*)lbl_80426BD0;
            htbl[0] = *(u32*)&lbl_8047D030;
            htbl[1] = *(u32*)&lbl_8047D034;
            if (targetIdx >= 0 && targetIdx < 2) {
                tgtH = htbl[targetIdx];
            }
            obj = fn_800F9318(0u, tgtH);
            fn_800E3D98(obj, targetPos);

            htbl[0] = *(u32*)&lbl_8047D030;
            htbl[1] = *(u32*)&lbl_8047D034;
            if (playerIdx >= 0 && playerIdx < 2) {
                plrH = htbl[playerIdx];
            }
            obj = fn_800F9318(0u, plrH);
            fn_800E3D98(obj, playerPos);

            htbl[0] = *(u32*)&lbl_8047D030;
            htbl[1] = *(u32*)&lbl_8047D034;
            if (playerIdx >= 0 && playerIdx < 2) {
                rotH = htbl[playerIdx];
            }
            obj = fn_800F9318(0u, rotH);
            fn_800E3D6C(obj, rotation);

            /* normalize current heading into (lbl_8047D098, lbl_8047D094) */
            heading = rotation[1];
            while (heading >= lbl_8047D094) { heading -= lbl_8047D094; }
            while (heading <= lbl_8047D098) { heading += lbl_8047D094; }
            rotation[1] = heading;

            targetAngle = (f32)fn_800CE2D8(
                targetPos[0] - playerPos[0],
                targetPos[2] - playerPos[2]);

            angleDiff = targetAngle - heading;
            if (angleDiff < lbl_8047D09C) {
                angleDiff += lbl_8047D094;
            } else if (angleDiff > lbl_8047D0A0) {
                angleDiff -= lbl_8047D094;
            }

            htbl[0] = *(u32*)&lbl_8047D030;
            htbl[1] = *(u32*)&lbl_8047D034;
            if (playerIdx >= 0 && playerIdx < 2) {
                turnH = htbl[playerIdx];
                turnValid = 1u;
            }

            if (!turnValid) {
                turnParam = -1;
            } else {
                fn_8018D998(0u, turnH);
                tPtr = fn_8018D928();
                if (tPtr != NULL) {
                    turnParam = *(s32*)((u8*)tPtr + 0x30);
                } else {
                    turnParam = -1;
                }
            }

            angleObj = fn_8018F6F4(turnParam);

            {
                f32 r = fn_8018F678(angleObj);
                maxPos = (r > lbl_8047D038) ? fn_8018F678(angleObj)
                                            : -fn_8018F678(angleObj);
            }
            {
                f32 r = fn_8018F658(angleObj);
                maxNeg = (r > lbl_8047D038) ? fn_8018F658(angleObj)
                                            : -fn_8018F658(angleObj);
            }

            if (angleDiff < lbl_8047D038) {
                f32 absDiff = -angleDiff;
                if (absDiff > maxNeg) {
                    newAngle = targetAngle + maxNeg;
                    if (newAngle >= lbl_8047D094) {
                        newAngle -= lbl_8047D094;
                    }
                    shouldTurn = 1u;
                }
            } else {
                if (angleDiff > maxPos) {
                    f32 adj = targetAngle - maxPos;
                    if (adj < lbl_8047D09C) {
                        adj += lbl_8047D094;
                    } else if (adj > lbl_8047D0A0) {
                        adj -= lbl_8047D094;
                    }
                    newAngle = adj;
                    shouldTurn = 1u;
                }
            }

            if (shouldTurn) {
                f32 delta   = newAngle - heading;
                f32 wrapped = delta;
                f32 magnitude;

                if (delta < lbl_8047D09C) {
                    wrapped = delta + lbl_8047D094;
                } else if (delta > lbl_8047D0A0) {
                    wrapped = delta - lbl_8047D094;
                }

                if (wrapped > lbl_8047D038) {
                    if (delta < lbl_8047D09C) {
                        magnitude = delta + lbl_8047D094;
                    } else if (delta > lbl_8047D0A0) {
                        magnitude = delta - lbl_8047D094;
                    } else {
                        magnitude = delta;
                    }
                } else {
                    if (delta < lbl_8047D09C) {
                        delta += lbl_8047D094;
                    } else if (delta > lbl_8047D0A0) {
                        delta -= lbl_8047D094;
                    }
                    magnitude = -delta;
                }

                if (magnitude < lbl_8047D0A4) {
                    newAngle   = heading;
                    shouldTurn = 0u;
                }
            }

            if (shouldTurn) {
                fn_8018805C(0u, entityHandle, newAngle, lbl_8047D080);
                turnAmount = lbl_8047D080;
            } else {
                fn_8018790C(0u, entityHandle);
                turnAmount = lbl_8047D038;
            }
        }
    }

    /* push the computed turn amount back into the player's field object */
    htbl[0] = *(u32*)&lbl_8047D030;
    htbl[1] = *(u32*)&lbl_8047D034;
    if (playerIdx >= 0 && playerIdx < 2) {
        finalHandle = htbl[playerIdx];
    }
    obj = fn_800F9318(0u, finalHandle);
    fn_8012C660(obj, playerIdx, turnAmount);
}
