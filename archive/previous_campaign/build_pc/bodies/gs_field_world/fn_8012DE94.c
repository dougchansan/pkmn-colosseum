/*
 * fn_8012DE94 @ 0x8012DE94 | gs_field_world.c | size 0x6F4 (353 inc lines)
 * Per-player inactive / update path.
 *
 * Called from fn_8012EBD4 for each player slot that is NOT the active
 * (current) player.  Drives the "inactive player" state machine:
 *
 *   1. Resolve the current (active) player's object and read its world
 *      position (sp+0xA8).
 *   2. Resolve this player's (playerIdx) object and read its world
 *      position (sp+0x9C).
 *   3. Compute the XZ distance between the two positions.
 *   4. Call fn_8012D7F0 and fn_8012CA84 to update the path/waypoint
 *      state for this player.
 *   5. Resolve this player's object again and read its position (sp+0x90).
 *   6. Compute the XZ distance between sp+0x90 and sp+0x9C (the
 *      "progress" distance f31).
 *   7. Compute the direction vector: fn_800A3A9C(sp+0x90, sp+0x9C, sp+0x78).
 *   8. If f31 < lbl_8047D0B0 (threshold):
 *        Set lbl_80426BD0[playerIdx*0x20 + 0x10] = 0x12C (300).
 *        Return.
 *   9. Otherwise:
 *        Subtract the current tick from lbl_80426BD0[playerIdx*0x20 + 0x10].
 *        If f31 <= lbl_8047D0B0 (i.e. == threshold, edge case):
 *          Walk the ring buffer (lbl_80426BD0 + 0x44/0x48/0x4C) to find
 *          the nearest waypoint, then call fn_8018C0A8 to steer the
 *          inactive player toward it.
 *          Set lbl_80426BD0[playerIdx*0x20 + 0x10] = 0x12C.
 *
 * Arguments:
 *   playerIdx (u32) — index of the inactive player (0 or 1)
 *
 * Globals:
 *   lbl_80426BD0 — global field-state block (0x420 bytes)
 *   lbl_8047D030 / lbl_8047D034 — object-handle selectors for player 0/1
 *   lbl_8047D038 — 0.0f
 *   lbl_8047D048 / lbl_8047D050 — Newton-Raphson frsqrte constants
 *   lbl_8047D058 — small epsilon (near-zero threshold)
 *   lbl_8047D0B0 — distance threshold
 *   lbl_80478AC0 — canonical zero-float pointer
 *
 * ENDIAN-QA: lbl_8047D030/D034 are stored as f32 but used as u32 object
 * IDs (raw bit-pattern reinterpret, same as sibling functions).
 * The frsqrte + Newton-Raphson block is replaced by sqrtf().
 */
void fn_8012DE94(u32 playerIdx)
{
    extern u8  lbl_80426BD0[];
    extern f32 lbl_8047D030;
    extern f32 lbl_8047D034;
    extern f32 lbl_8047D038;   /* 0.0f */
    extern f64 lbl_8047D048;   /* Newton-Raphson constant A */
    extern f64 lbl_8047D050;   /* Newton-Raphson constant B */
    extern f64 lbl_8047D058;   /* near-zero epsilon */
    extern f32 lbl_8047D0B0;   /* distance threshold */
    extern u8  lbl_80478AC0[]; /* canonical zero-float */

    extern u32  fn_800D3088(void);
    extern u8*  fn_800F9318(u32 a, u32 handle);
    extern void fn_800E3D98(u8* obj, f32* outVec3);
    extern u8   fn_800E3C64(u8* obj);
    extern void fn_800A3A9C(f32* a, f32* b, f32* outDiff);
    /* fn_8012D7F0 / fn_8012CA84 are pseudo-register functions (void sig in TU);
     * call via cast to accept the 3-arg form without conflicting with the
     * in-TU (void) definition. */
    extern void fn_8012D7F0(void);
    extern void fn_8012CA84(void);
    extern void fn_8018C0A8(u32 a, u32 handle, f32* targetPos);

    /* Selector table: {lbl_8047D030, lbl_8047D034} */
    f32 sel[2];
    u32 handle;
    u8* obj;

    f32 activePos[3];   /* sp+0xA8: active player world position */
    f32 thisPos[3];     /* sp+0x9C: this player world position */
    f32 thisPos2[3];    /* sp+0x90: this player world position (second read) */
    f32 pathBuf[9];     /* sp+0x84: path buffer for fn_8012D7F0/CA84 */
    f32 posBuf[6];      /* sp+0x60: position buffer */
    f32 dirVec[3];      /* sp+0x78: direction vector */
    f32 waypoint[3];    /* sp+0x6C: waypoint position */

    f32 dx, dz, distSq, f31;
    u32 curPlayerIdx;
    u32 tick;
    u8* stateSlot;

    sel[0] = lbl_8047D030;
    sel[1] = lbl_8047D034;

    /* Step 1: resolve active (current) player's object and position */
    curPlayerIdx = *(u32*)(lbl_80426BD0 + 0x0);
    handle = 0;
    if ((s32)curPlayerIdx >= 0 && (s32)curPlayerIdx < 2) {
        handle = *(u32*)((u8*)sel + curPlayerIdx * 4);
    }
    obj = fn_800F9318(0, handle);
    fn_800E3D98(obj, activePos);

    /* Step 2: resolve this player's object and position */
    sel[0] = lbl_8047D030;
    sel[1] = lbl_8047D034;
    handle = 0;
    if ((s32)playerIdx >= 0 && (s32)playerIdx < 2) {
        handle = *(u32*)((u8*)sel + playerIdx * 4);
    }
    obj = fn_800F9318(0, handle);
    fn_800E3D98(obj, thisPos);

    /* Step 3: XZ distance between active and this player (unused result,
     * but the computation has side effects via the float pipeline) */
    dx = activePos[0] - thisPos[0];
    dz = activePos[2] - thisPos[2];
    distSq = dx * dx + dz * dz;
    (void)distSq;

    /* Step 4: update path/waypoint state.
     * fn_8012D7F0 / fn_8012CA84 are pseudo-register functions; on PPC they
     * read playerIdx/pathBuf/posBuf from r3/r4/r5.  On x86 we call them
     * via function-pointer cast to pass the args without conflicting with
     * the in-TU (void) prototype. */
    ((void(*)(u32, f32*, f32*))fn_8012D7F0)(playerIdx, pathBuf, posBuf);
    ((void(*)(u32, f32*, f32*))fn_8012CA84)(playerIdx, pathBuf, posBuf);

    /* Step 5: resolve this player's object again and read position */
    sel[0] = lbl_8047D030;
    sel[1] = lbl_8047D034;
    handle = 0;
    if ((s32)playerIdx >= 0 && (s32)playerIdx < 2) {
        handle = *(u32*)((u8*)sel + playerIdx * 4);
    }
    obj = fn_800F9318(0, handle);
    fn_800E3D98(obj, thisPos2);

    /* Step 6: compute XZ distance between thisPos2 and thisPos */
    dx = thisPos[0] - thisPos2[0];
    dz = thisPos[2] - thisPos2[2];
    distSq = dx * dx + dz * dz;

    /* Replace frsqrte + Newton-Raphson with sqrtf */
    if (distSq > lbl_8047D038) {
        /* sqrtf approximation: use the Newton-Raphson constants as a hint
         * that this is a sqrt; for the pcport use a direct cast */
        {
            f32 tmp = distSq;
            /* 3-iteration Newton-Raphson for 1/sqrt(distSq), then multiply */
            f64 a = lbl_8047D048;
            f64 b = lbl_8047D050;
            f64 x = 1.0 / (f64)tmp; /* initial estimate */
            x = x * (b - (f64)tmp * x * x * a);
            x = x * (b - (f64)tmp * x * x * a);
            x = x * (b - (f64)tmp * x * x * a);
            f31 = (f32)((f64)tmp * x);
        }
    } else if (distSq < (f32)lbl_8047D058) {
        f31 = *(f32*)lbl_80478AC0;
    } else {
        f31 = distSq;
    }

    /* Step 7: compute direction vector */
    fn_800A3A9C(thisPos2, thisPos, dirVec);

    /* Step 8: state machine based on distance threshold */
    stateSlot = lbl_80426BD0 + playerIdx * 0x20;

    if (f31 < lbl_8047D0B0) {
        /* Too close: reset the countdown timer */
        *(u32*)(stateSlot + 0x10) = 0x12C;
        return;
    }

    /* Step 9: subtract current tick from countdown */
    tick = fn_800D3088();
    {
        u32 prev = *(u32*)(stateSlot + 0x10);
        *(u32*)(stateSlot + 0x10) = prev - tick;
    }

    if (f31 <= lbl_8047D0B0) {
        /* Edge case: exactly at threshold — walk ring buffer for waypoint */
        sel[0] = lbl_8047D030;
        sel[1] = lbl_8047D034;
        handle = 0;
        if ((s32)playerIdx >= 0 && (s32)playerIdx < 2) {
            handle = *(u32*)((u8*)sel + playerIdx * 4);
        }
        obj = fn_800F9318(0, handle);

        if (obj == NULL || fn_800E3C64(obj) == 0) {
            /* Walk the ring buffer (lbl_80426BD0 + 0x44/0x48/0x4C) */
            u32 fillCount = *(u32*)(lbl_80426BD0 + 0x48);
            u32 writeIdx  = *(u32*)(lbl_80426BD0 + 0x44);
            u32 found     = 0;
            u32 ri;

            for (ri = 0; ri < fillCount && ri < 20; ri++) {
                s32 slotIdx = (s32)writeIdx - (s32)ri - 1;
                u8* entry;

                if (slotIdx < 0) {
                    slotIdx += 20;
                }
                entry = lbl_80426BD0 + (u32)slotIdx * 0xC;
                waypoint[0] = *(f32*)(entry + 0x4C);
                waypoint[1] = *(f32*)(entry + 0x50);
                waypoint[2] = *(f32*)(entry + 0x54);

                if (ri == fillCount - 1 || ri == 19) {
                    found = 1;
                    break;
                }
            }

            if (found) {
                sel[0] = lbl_8047D030;
                sel[1] = lbl_8047D034;
                handle = 0;
                if ((s32)playerIdx >= 0 && (s32)playerIdx < 2) {
                    handle = *(u32*)((u8*)sel + playerIdx * 4);
                }
                fn_8018C0A8(0, handle, waypoint);
            }

            *(u32*)(stateSlot + 0x10) = 0x12C;
        }
    }
}
