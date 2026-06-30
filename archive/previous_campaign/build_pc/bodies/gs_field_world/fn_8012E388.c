/* Per-player field input processor.
   Resolves the active player handle, turns analog/button input into a movement
   scalar and heading update, and stops the entity when no movement is present. */
void fn_8012E388(s32 playerIdx, f32* outAux)
{
    extern f32 lbl_8047D030;
    extern f32 lbl_8047D034;
    extern f32 lbl_8047D038;
    extern f32 lbl_8047D078;
    extern f32 lbl_8047D084;
    extern f32 lbl_8047D0B4;
    extern f32 lbl_8047D0B8;
    extern f32 lbl_8047D0BC;
    extern f32 lbl_8047D0C0;
    extern f32 lbl_8047D0C4;
    extern u8 lbl_80478AC0[];
    extern f32 sqrtf(f32 x);
    extern f32 sinf(f32 x);
    extern void fn_80177A38(void);

    f32 selector[2];
    u32 handle = 0;
    s32 analog_x;
    s32 analog_y;
    s32 aim_x;
    s32 aim_y;

    ((void* (*)(u32, u32))fn_800F9318)(0, 2);

    selector[0] = lbl_8047D030;
    selector[1] = lbl_8047D034;
    if (playerIdx >= 0 && playerIdx < 2) {
        handle = *(u32*)((u8*)selector + playerIdx * 4);
    }

    ((void* (*)(u32, u32))fn_800F9318)(0, handle);

    analog_x = (s8)((u32 (*)(s32, s32))fn_800F7A7C)(1, 1);
    analog_y = (s8)((u32 (*)(s32, s32))fn_800F7A08)(1, 1);
    aim_x = (s8)((u32 (*)(s32, s32))fn_800F7A7C)(1, 0);
    aim_y = (s8)((u32 (*)(s32, s32))fn_800F7A08)(1, 0);

    if ((s8)analog_x == 0 && (s8)analog_y == 0) {
        if (((u32 (*)(s32))fn_800F7BC4)(1) & 0x08u) {
            analog_y = -0x38;
        }
        if (((u32 (*)(s32))fn_800F7BC4)(1) & 0x04u) {
            analog_y = 0x38;
        }
        if (((u32 (*)(s32))fn_800F7BC4)(1) & 0x01u) {
            analog_x = -0x38;
        }
        if (((u32 (*)(s32))fn_800F7BC4)(1) & 0x02u) {
            analog_x = 0x38;
        }

        aim_x = analog_x;
        aim_y = analog_y;

        if ((s8)analog_x == 0 && (s8)analog_y == 0) {
            ((void (*)(void))fn_80177A38)();
        }
    }

    if ((s8)analog_x == 0 && (s8)analog_y == 0) {
        *outAux = lbl_8047D038;
        ((void (*)(u32, u32))fn_8018790C)(0, handle);
        return;
    }

    if ((s8)analog_x > 0x38) {
        analog_x = 0x38;
    } else if ((s8)analog_x < -0x38) {
        analog_x = -0x38;
    }

    if ((s8)analog_y > 0x38) {
        analog_y = 0x38;
    } else if ((s8)analog_y < -0x38) {
        analog_y = -0x38;
    }

    {
        f32 x_mag = (f32)(((s8)analog_x < 0) ? -(s8)analog_x : (s8)analog_x) / lbl_8047D0B4;
        f32 y_mag = (f32)(((s8)analog_y < 0) ? -(s8)analog_y : (s8)analog_y) / lbl_8047D0B4;
        f32 mag_sq = x_mag * x_mag + y_mag * y_mag;
        f32 mag;

        if (mag_sq > lbl_8047D038) {
            mag = sqrtf(mag_sq);
        } else if (mag_sq < lbl_8047D038) {
            mag = *(f32*)lbl_80478AC0;
        } else {
            mag = mag_sq;
        }

        *outAux = mag;
        if (*outAux > lbl_8047D084) {
            *outAux = lbl_8047D084;
        }

        if ((s8)aim_x <= -2 || (s8)aim_x >= 2 ||
            (s8)aim_y <= -2 || (s8)aim_y >= 2) {
            f32 quadrant_angle;
            f32 angle;

            if (y_mag < lbl_8047D0B8) {
                quadrant_angle = lbl_8047D0BC;
            } else {
                f32 ratio = x_mag / y_mag;
                if (ratio > lbl_8047D078) {
                    ratio = lbl_8047D078;
                }
                quadrant_angle = lbl_8047D0BC * sinf(ratio / lbl_8047D0C0);
            }

            if ((s8)analog_y >= 0) {
                angle = quadrant_angle;
            } else {
                angle = lbl_8047D0C4 - quadrant_angle;
            }

            if ((s8)analog_x < 0) {
                if ((s8)analog_y >= 0) {
                    angle = lbl_8047D0C4 + (lbl_8047D0C4 - quadrant_angle);
                } else {
                    angle = lbl_8047D0C4 + quadrant_angle;
                }
            }

            angle += ((f32 (*)(void))fn_80176684)();
            ((void (*)(u32, u32, f32, f32))fn_8018805C)(0, handle, angle, *outAux);
        }
    }

    ((void (*)(u32, u32, f32))fn_80188214)(0, handle, *outAux);

    {
        f32 sound_pos[3];
        void* listener;

        sound_pos[0] = lbl_8047D038;
        sound_pos[1] = lbl_8047D038;
        sound_pos[2] = lbl_8047D038;
        listener = ((void* (*)(u32, u32))fn_800F9318)(0, 0x7D0);
        ((void (*)(void*, void*))fn_80166458)(listener, sound_pos);
    }
}
