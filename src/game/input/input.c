/**
 * @file input.c
 * @brief GS script VM context pool setup and the game pad manager.
 *
 * Address range: 0x800F7758 - 0x800F915C, .sdata2 0x8047CCC8 - 0x8047CD00,
 * .bss 0x80401C10 - 0x80401DE0.
 *
 * cms_InitThreadPool allocates the script VM context pool (GSVMPool at lbl_80401BF8,
 * contexts of 0x16C bytes). The rest of the range is the pad manager: four
 * InputPad slots in lbl_80401C10, each bound to a PAD channel, read through
 * small accessors that look the slot up by id, the per-frame update
 * (GSinputRead), the one-time init (GSinputInit), the PAD sampling callback
 * (inputPadCallback) with its rumble driver (inputUpdateMotors), and the stick filter
 * and output stage the update calls (inputSmoothStick, inputClampSticks).
 *
 * Unit boundary: the functions up to GSinputInit use two .sdata2 literals,
 * the int-to-float bias (GSinputResetRightSmoothing/GSinputResetLeftSmoothing) and 0.0f (GSinputInit).
 * Retail has exactly that pair at 0x8047CCC8/0x8047CCD0, and the only other
 * code that references them is 0x800F8268-0x800F915C, whose own literals
 * follow at 0x8047CCD4-0x8047CCF8 in first-use order (0.25f, 0.5f, 1.0f and
 * the unsigned bias from inputSmoothStick, then sqrtf's 0.5, 3.0 and 0.0 from
 * inputClampSticks). Compiler literals cannot be shared between objects, so the
 * two ranges are one translation unit. The three registry-release functions
 * that used to follow (0x800F915C-0x800F9318) belong to GSres (gs_res.c).
 *
 * The pad slots, the rumble timers and the motor commands are three objects
 * this unit defines. GSinputInit and inputUpdateMotors address all three from one
 * pooled .bss base (slots +0, timers +0x1B0, commands +0x1C0; the slots come
 * out as `addi rN,base,0`, which MWCC only emits for pooled unit data), so
 * they are defined here rather than extern.
 *
 * Build settings (configure.py): GC/1.3.2 with -inline deferred.
 * - GC/1.3 turns the pooled `base + 0` into a register copy; GC/1.3.2 keeps
 *   the add, as retail does (same behaviour as the GSgfx string pool).
 * - With normal inlining MWCC orders pooled .bss by first reference, which
 *   puts the motor commands before the timers (GSinputInit references them
 *   first). Deferred inlining lays the pool out in reverse definition order,
 *   which gives retail's layout from plain definitions. It also emits the
 *   functions in reverse definition order, so they are listed here from the
 *   highest address down; the static inline helpers come first.
 *
 * Dolphin PAD calls that have no symbol name yet, identified by their order
 * in the SDK's Pad.c and by their arguments here:
 *   fn_800AAD34 PADReset(mask)            PADRecalibrate PADRecalibrate(mask)
 *   PADInit PADInit()                 fn_800AB4FC PADControlAllMotors(cmds)
 */

#include "game/input/input.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/si/SI.h"

extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 n);
extern void GSlogWritef(const char* fmt, ...);
extern u16 _toolentryAlloc__FUl(u32 size);   /* GSmemAllocRaw */
extern void* GSmemLock(u16 handle);         /* GSmemGetPtr */
extern u32 fn_800D3094(void);
extern void fn_800AAD34(u32 mask);            /* PADReset */
extern BOOL PADRecalibrate(u32 mask);            /* PADRecalibrate */
extern BOOL PADInit(void);                /* PADInit */
extern void fn_800AB4FC(const u32* commands); /* PADControlAllMotors */
extern void PADSetAnalogMode(u32 mode);
extern u32 PADRead(PADStatus* status);    /* PADRead */
extern u32 SIProbe(s32 chan);             /* SI type/status probe */
extern f64 __frsqrte(f64 value);
extern const f32 lbl_80478AC0[];              /* NaN */

extern GSVMPool lbl_80401BF8;
extern GSVMPool* lbl_80478B00;
extern const char lbl_802712E4[];

/* Rumble state. The PAD sampling callback (inputPadCallback, run from the SI
 * interrupt) drives the motors from these through inputUpdateMotors, while the game
 * thread sets them up in GSinputInit; retail treats them as volatile (the
 * timer and command accesses in inputUpdateMotors are never cached or reordered
 * across each other, and GSinputInit's command stores keep their order). */
volatile u32 lbl_80401DD0[INPUT_PAD_COUNT]; /* PADControlAllMotors commands */
volatile s32 lbl_80401DC0[INPUT_PAD_COUNT]; /* rumble pulse accumulators */
InputPad lbl_80401C10[INPUT_PAD_COUNT];

/* Sampling-callback frame counter. */
extern u32 lbl_8047AC48;
/* Channels waiting for PADReset. The PAD sampling callback (inputPadCallback)
 * sets and clears these bits, so GSinputRead re-reads it on every use. */
extern volatile u32 lbl_8047AC4C;
/* fn_800D3094 value at the last PADReset. */
extern u32 lbl_8047AC50;

static inline InputPad* InputFindPad(s32 id) {
    InputPad* pad = lbl_80401C10;
    s32 i;

    for (i = 0; i < INPUT_PAD_COUNT; i++, pad++) {
        if (pad->id == id) {
            return pad;
        }
    }
    return NULL;
}

/* MSL <math.h> fpclassify/sqrtf, inlined four times into inputClampSticks (the
 * same frsqrte + three Newton steps + NaN fallbacks each time). */
static inline s32 InputFpClassify(f32 value) {
    switch (*(s32*)&value & 0x7F800000) {
    case 0x7F800000:
        if (*(s32*)&value & 0x007FFFFF) {
            return 1;
        }
        return 2;
    case 0:
        if (*(s32*)&value & 0x007FFFFF) {
            return 5;
        }
        return 3;
    }
    return 4;
}

static inline f32 InputSqrtf(f32 value) {
    if (value > 0.0f) {
        f64 guess = __frsqrte(value);
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        value = (f32)(value * guess);
    } else if ((f64)value < 0.0) {
        value = lbl_80478AC0[0];
    } else if (InputFpClassify(value) == 1) {
        value = lbl_80478AC0[0];
    }
    return value;
}

/* Apply the +-10 dead zone to one stick and limit it to a circle of
 * `radius`. inputClampSticks expands this four times: both axes are sign-extended
 * up front (argument evaluation), then the same dead zone, squared-length
 * test, inlined sqrtf and rescale follow with radius 56 or 44. */
static inline void InputClampStick(s8 inX, s8 inY, s8* outX, s8* outY, s32 radius) {
    s32 x = inX;
    s32 y = inY;
    s32 sq;
    f32 len;

    if (x > -10 && x < 10) {
        x = 0;
    } else if (x > 0) {
        x -= 10;
    } else {
        x += 10;
    }
    if (y > -10 && y < 10) {
        y = 0;
    } else if (y > 0) {
        y -= 10;
    } else {
        y += 10;
    }
    sq = x * x + y * y;
    if (sq > radius * radius) {
        len = InputSqrtf(sq);
        x = x * radius / (s32)len;
        y = y * radius / (s32)len;
    }
    *outX = x;
    *outY = y;
}

/* Produce the pad's output stick values. Mode 0 passes the filtered sticks
 * through; mode 2 applies the dead zone and circle limit to the raw sticks
 * in place and to the filtered sticks into the outputs (radius 56 for the
 * main stick, 44 for the C stick). */
void inputClampSticks(InputPad* pad) {
    switch (pad->outputMode) {
    case 0:
        pad->outStickX = pad->stickX;
        pad->outStickY = pad->stickY;
        pad->outSubstickX = pad->substickX;
        pad->outSubstickY = pad->substickY;
        break;
    case 2:
        InputClampStick(pad->current.stickX, pad->current.stickY,
                        &pad->current.stickX, &pad->current.stickY, 56);
        InputClampStick(pad->current.substickX, pad->current.substickY,
                        &pad->current.substickX, &pad->current.substickY, 44);
        InputClampStick(pad->stickX, pad->stickY,
                        &pad->outStickX, &pad->outStickY, 56);
        InputClampStick(pad->substickX, pad->substickY,
                        &pad->outSubstickX, &pad->outSubstickY, 44);
        break;
    }
}

/* Stick smoothing filter for one stick: when the raw position moves by more
 * than 2, recompute the per-frame step towards it, then advance the filtered
 * position without overshooting. */
void inputSmoothStick(InputPad* pad, s8 x, s8 y, s8* lastX, s8* lastY,
                 f32* stepX, f32* stepY, f32* posX, f32* posY) {
    if (*lastX < x - 2 || *lastX > x + 2 || *lastY < y - 2 || *lastY > y + 2) {
        if (pad->smoothMode == 0) {
            *stepX = (x - *posX) / pad->smoothFrames;
            *stepY = (y - *posY) / pad->smoothFrames;
        } else if (pad->smoothMode == 1) {
            /* Mode 1 spreads the move over a quarter of the frames. Retail's
             * fmuls takes the frame count first and the 0.25 second, which
             * is `frames * rate` with the factor in a named local; a bare
             * literal is emitted as the first operand instead. */
            f32 rate = 0.25f;
            *stepX = (x - *posX) / (pad->smoothFrames * rate);
            *stepY = (y - *posY) / (pad->smoothFrames * rate);
        }
        pad->smoothCount = 0;
    }
    *lastX = x;
    *lastY = y;
    *posX += *stepX;
    *posY += *stepY;

    if (pad->smoothMode == 1) {
        pad->smoothCount++;
        if (pad->smoothCount < pad->smoothFrames) {
            *stepX *= 0.5f;
            *stepY *= 0.5f;
            if ((*stepX > 0.0f ? *stepX : -*stepX) < 1.0f) {
                *stepX = *stepX > 0.0f ? 1 : -1;
            }
            /* Retail tests *stepX here too (lfs 0(r8)), not *stepY. */
            if ((*stepY > 0.0f ? *stepY : -*stepY) < 1.0f) {
                *stepY = *stepX > 0.0f ? 1 : -1;
            }
        }
    }

    if (*stepX < 0.0f) {
        if (*posX < *lastX) {
            *posX = *lastX;
        }
    } else {
        if (*posX > *lastX) {
            *posX = *lastX;
        }
    }
    if (*stepY < 0.0f) {
        if (*posY < *lastY) {
            *posY = *lastY;
        }
    } else {
        if (*posY > *lastY) {
            *posY = *lastY;
        }
    }
}

/* Drive the rumble motors from each connected slot's rumble request. */
void inputUpdateMotors(void) {
    u8 changed = FALSE;
    int chan;
    InputPad* pad;

    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++) {
        pad = InputFindPad(chan + 1);
        if (pad == NULL || pad->status != 0) {
            continue;
        }
        switch (pad->rumbleMode) {
        case 1:
            if (pad->rumbleStrength > 3600 && lbl_80401DD0[chan] != PAD_MOTOR_RUMBLE) {
                changed = TRUE;
                lbl_80401DD0[chan] = PAD_MOTOR_RUMBLE;
            } else if (pad->rumbleStrength < 100 && lbl_80401DD0[chan] != PAD_MOTOR_STOP) {
                changed = TRUE;
                lbl_80401DD0[chan] = PAD_MOTOR_STOP;
            } else {
                lbl_80401DC0[chan] += pad->rumbleStrength;
                if (lbl_80401DC0[chan] > 3600) {
                    lbl_80401DC0[chan] = 0;
                    if (lbl_80401DD0[chan] != PAD_MOTOR_RUMBLE) {
                        changed = TRUE;
                        lbl_80401DD0[chan] = PAD_MOTOR_RUMBLE;
                    }
                } else if (lbl_80401DD0[chan] != PAD_MOTOR_STOP) {
                    changed = TRUE;
                    lbl_80401DD0[chan] = PAD_MOTOR_STOP;
                }
            }
            break;
        case 2:
            if (lbl_80401DD0[chan] != PAD_MOTOR_STOP) {
                changed = TRUE;
                lbl_80401DD0[chan] = PAD_MOTOR_STOP;
            }
            break;
        case 3:
            if (lbl_80401DD0[chan] != PAD_MOTOR_STOP_HARD) {
                changed = TRUE;
                lbl_80401DD0[chan] = PAD_MOTOR_STOP_HARD;
            }
            break;
        }
        if (pad->rumbleFrames != 0) {
            if (--pad->rumbleFrames == 0) {
                pad->rumbleMode = 2;
            }
        }
        if (pad->rumbleDecay != 0) {
            if (pad->rumbleStrength < pad->rumbleDecay) {
                pad->rumbleMode = 2;
                pad->rumbleDecay = 0;
            } else {
                pad->rumbleStrength -= pad->rumbleDecay;
            }
        }
    }
    if (changed) {
        fn_800AB4FC((u32*)lbl_80401DD0);
    }
}

/* PAD sampling callback: read all four channels, latch each bound slot's
 * status, track connection changes, then update the rumble motors. */
void inputPadCallback(void) {
    PADStatus status[PAD_MAX_CONTROLLERS];
    PADStatus* s;
    InputPad* pad;
    int chan;
    u32 type;

    PADRead(status);
    s = status;
    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++, s++) {
        pad = InputFindPad(chan + 1);
        if (pad == NULL) {
            continue;
        }
        switch (s->err) {
        case PAD_ERR_NONE:
            if (pad->status == 3) {
                switch (SIProbe(chan)) {
                case 0x09000000: /* SI_GC_CONTROLLER */
                    pad->type = 0;
                    break;
                default:
                    pad->type = 2;
                    break;
                }
                pad->status = 0;
            }
            s->stickY = -s->stickY;
            s->substickY = -s->substickY;
            memcpy(&pad->latched, s, sizeof(PADStatus));
            lbl_8047AC4C &= ~(PAD_CHAN0_BIT >> chan);
            break;
        case PAD_ERR_NO_CONTROLLER:
            type = SIProbe(chan);
            if (type == 8) { /* SI_ERROR_NO_RESPONSE */
                pad->status = 3;
            } else if (type == 0x40) {
                pad->status = 4;
            }
            memset(&pad->latched, 0, sizeof(PADStatus));
            lbl_8047AC4C |= PAD_CHAN0_BIT >> chan;
            break;
        case PAD_ERR_NOT_READY:
            break;
        }
    }
    inputUpdateMotors();
    lbl_8047AC48++;
}

void GSinputInit(void) {
    InputPad* pad = lbl_80401C10;
    volatile u32* motor = lbl_80401DD0;
    volatile s32* timer = lbl_80401DC0;
    int i;

    lbl_8047AC48 = 0;
    lbl_8047AC4C = PAD_CHAN0_BIT | PAD_CHAN1_BIT | PAD_CHAN2_BIT | PAD_CHAN3_BIT;
    lbl_8047AC50 = 0;

    for (i = 0; i < INPUT_PAD_COUNT; i++, pad++) {
        pad->id = 0;
        pad->outputMode = 0;
        pad->status = 3;
        pad->smoothFrames = 0;
        pad->smoothMode = 0;
        pad->prevButton = 0;
        memset(&pad->latched, 0, sizeof(PADStatus));
        memset(&pad->current, 0, sizeof(PADStatus));
        pad->lastStickX = 0;
        pad->lastStickY = 0;
        pad->lastSubstickX = 0;
        pad->lastSubstickY = 0;
        pad->stickX = 0.0f;
        pad->stickY = 0.0f;
        pad->substickX = 0.0f;
        pad->substickY = 0.0f;
        pad->rumbleMode = 3;
        pad->rumbleStrength = 0;
        pad->rumbleFrames = 0;
        pad->rumbleDecay = 0;
        *motor++ = PAD_MOTOR_STOP_HARD;
        *timer++ = 0;
    }

    PADSetAnalogMode(0);
    PADInit();
    fn_800AB4FC((u32*)lbl_80401DD0);
    PADRecalibrate(lbl_8047AC4C);
    SISetSamplingRate(11);
    PADSetSamplingCallback(inputPadCallback);
}

/* Bind a free slot to PAD channel `id` - 1. */
s32 GSinputOpen(s32 id) {
    InputPad* pad = InputFindPad(0);

    if (pad == NULL) {
        return 4;
    }
    switch (id) {
    case 1:
    case 2:
    case 3:
    case 4:
        pad->id = id;
        break;
    default:
        return 1;
    }
    return 0;
}

/* Per-frame update: latch the sampled status, service pending PAD resets,
 * and run the stick filter. Returns the pad's connection status. */
u32 GSinputRead(s32 id) {
    InputPad* pad = InputFindPad(id);
    BOOL level;
    u32 now;

    if (pad == NULL) {
        return 2;
    }

    pad->prevButton = pad->current.button;
    level = OSDisableInterrupts();
    now = fn_800D3094();
    if (lbl_8047AC4C != 0 && now != lbl_8047AC50) {
        fn_800AAD34(lbl_8047AC4C);
        lbl_8047AC50 = now;
    }
    memcpy(&pad->current, &pad->latched, sizeof(PADStatus));
    OSRestoreInterrupts(level);

    inputSmoothStick(pad, pad->current.stickX, pad->current.stickY,
                &pad->lastStickX, &pad->lastStickY,
                &pad->stickStepX, &pad->stickStepY, &pad->stickX, &pad->stickY);
    inputSmoothStick(pad, pad->current.substickX, pad->current.substickY,
                &pad->lastSubstickX, &pad->lastSubstickY,
                &pad->substickStepX, &pad->substickStepY, &pad->substickX, &pad->substickY);
    inputClampSticks(pad);
    return pad->status;
}

u8 GSinputIsConnected(s32 id) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return FALSE;
    }
    return pad->status == 0;
}

void GSinputSetClamping(s32 id, u32 value) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return;
    }
    pad->outputMode = value;
}

void GSinputSetSmoothingRate(s32 id, u8 frames) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return;
    }
    pad->smoothFrames = frames;
}

void GSinputSetSmoothingFunc(s32 id, u32 mode) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return;
    }
    pad->smoothMode = mode;
}

/* Snap the filtered main stick to a position. */
void GSinputResetLeftSmoothing(s32 id, s8 x, s8 y) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return;
    }
    pad->lastStickX = x;
    pad->lastStickY = y;
    pad->stickX = x;
    pad->stickY = y;
}

/* Snap the filtered substick to a position. */
void GSinputResetRightSmoothing(s32 id, s8 x, s8 y) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return;
    }
    pad->lastSubstickX = x;
    pad->lastSubstickY = y;
    pad->substickX = x;
    pad->substickY = y;
}

u32 GSinputGetSubType(s32 id) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 2;
    }
    return pad->type;
}

u16 fn_800F7BC4(s32 id) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    return pad->current.button;
}

u32 fn_800F7B5C(s32 id) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    return ~pad->current.button;
}

/* Buttons whose state changed since the previous update. */
u32 GSinputButtonsChanged(s32 id) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    return pad->current.button ^ pad->prevButton;
}

s8 GSinputGetLeftStickXData(s32 id, s32 filtered) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    if (filtered == 1) {
        return pad->outStickX;
    }
    return pad->current.stickX;
}

s8 GSinputGetLeftStickYData(s32 id, s32 filtered) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    if (filtered == 1) {
        return pad->outStickY;
    }
    return pad->current.stickY;
}

s8 GSinputGetRightStickXData(s32 id, s32 filtered) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    if (filtered == 1) {
        return pad->outSubstickX;
    }
    return pad->current.substickX;
}

s8 GSinputGetRightStickYData(s32 id, s32 filtered) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    if (filtered == 1) {
        return pad->outSubstickY;
    }
    return pad->current.substickY;
}

void GSinputMotorStart(s32 id, u8 motor, u8 strength, u32 frames, u8 decay) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return;
    }
    if (motor != 0) {
        return;
    }
    pad->rumbleMode = 1;
    pad->rumbleStrength = strength * 15;
    pad->rumbleFrames = frames;
    pad->rumbleDecay = decay;
}

/* Allocate `count` script VM contexts and reset the pool. */
s32 cms_InitThreadPool(u16 count) {
    GSVMContext* ctx;
    s32 i;

    memset(&lbl_80401BF8, 0, sizeof(GSVMPool));
    lbl_80478B00 = &lbl_80401BF8;
    lbl_80478B00->handle = _toolentryAlloc__FUl(count * sizeof(GSVMContext));
    if (lbl_80478B00->handle == 0) {
        GSlogWritef(lbl_802712E4);
        return -1;
    }

    lbl_80478B00->contexts = GSmemLock(lbl_80478B00->handle);
    ctx = lbl_80478B00->contexts;
    for (i = 0; i < count; i++) {
        ctx->script = NULL;
        ctx->state = 0;
        ctx++;
    }
    lbl_80478B00->count = count;
    return 0;
}
