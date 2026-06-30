/**
 * @file generator.c
 * @brief Particle / effect generator for the Pokemon Colosseum script VM.
 *
 * Decompiled from:
 *   generateParticle_801947D4 (generatorMain)  -- 0x14E0 bytes (5,344 bytes)
 *
 * Source file confirmed by rodata string: "generator.c" (lbl_802739E4)
 * Also references: "psCamera" (lbl_802739F0)
 *
 * The generator system is invoked per-frame by the script interpreter
 * (psinterpret.c) for each active particle/effect generator.  A generator
 * controls the emission of particles or camera-attached effects based on
 * parameters stored in a generator work structure.
 *
 * Generator work structure (accessed via r31 throughout the function):
 *   0x00: u32  state/flags
 *   0x04: u32  controlFlags (bitmask tested with rlwinm)
 *   0x08: f32  reserved
 *   0x0C: f32  lifetime (checked against threshold to decide if active)
 *   0x10: u32  reserved
 *   0x12: u16  emissionType (bottom 4 bits: 1=camera, 2=scaled)
 *   0x14-0x1F: reserved
 *   0x20: f32  position.x
 *   0x24: f32  position.y
 *   0x28: f32  position.z
 *   0x2C: f32  velocity.x
 *   0x30: f32  velocity.y
 *   0x34: f32  velocity.z
 *   ...
 *   0x88: u16  scaleFlags (bit 24 tested for scaled emission)
 *   0x8C: f32  rotX (rotation angle X for directional emission)
 *   0x90: f32  rotY (rotation angle Y)
 *   0x94: f32  rotZ (rotation angle Z)
 *   0x98: f32  scaleX
 *   0x9C: f32  scaleY
 *   0xA0: f32  scaleZ
 *
 * The function is enormous (5.3KB) because it handles multiple emission
 * modes with extensive matrix math inline:
 *
 * 1. Early exit: if lifetime (offset 0x0C) < threshold -> return.
 * 2. Compute velocity vector from offsets 0x2C-0x34.
 * 3. If emission type == 2 and scale flag set:
 *      multiply velocity by scale factors (0x98, 0x9C, 0xA0).
 * 4. Compute velocity magnitude using fast inverse square root
 *    (Newton-Raphson refinement with frsqrte, 3 iterations).
 * 5. Handle special float cases (NaN, denorm, inf) for the magnitude.
 * 6. Build a 3x3 rotation matrix from rotX/rotY/rotZ:
 *    - fn_800A3074: build axis-angle rotation matrices
 *    - fn_800A2D98: multiply matrices
 *    - fn_800A3ADC: normalize matrix column vectors
 * 7. If controlFlags bit 17 set (camera-tracking mode):
 *    - Assert "psCamera" object exists (via lbl_8047B190)
 *    - Compute direction vector from generator to camera
 *    - Build look-at matrix via cross products (fn_800A3B9C)
 * 8. If emission type != 1 and magnitude > epsilon:
 *    - Compute azimuth (atan2) and elevation angles
 *    - Build yaw/pitch rotation matrices using sin/cos
 *    - Combine with the directional matrix
 * 9. Compute the final emission direction and spawn particles.
 *
 * Address: 0x8017424C, Size: 0x14E0
 */

#include "dolphin/types.h"
#include "game/effect/gs_effect.h"

/* ===== External functions ===== */
extern void  fn_800DD970(const char* fmt, ...);          /* OSReport / GSlog */
extern void  fn_800A2D38(void* mtxOut);                  /* MTXIdentity */
extern void  fn_800A2D98(void* mtxOut, void* mtxA,
                          void* mtxB);                   /* MTXConcat */
extern void  fn_800A3074(f32 angle, void* mtxOut,
                          u32 axis);                     /* MTXRotAxis */
extern void  fn_800A3ADC(void* mtx3x3,
                          void* mtxNormalized);          /* MTXNormalize */
extern void  fn_800A3B9C(void* vecA, void* vecB,
                          void* crossOut);               /* VECCross */
extern void  fn_800CE148(f32 angle);                     /* sinf -> f1 */
extern void  fn_800CDBE0(f32 angle);                     /* cosf -> f1 */
extern f64   fn_800CE2D8(f32 y, f32 x);                 /* atan2f */
extern void  HSD_CObjGetUpVector(void* cameraObj,
                          void* outMtx);                 /* camera get matrix */
extern void  __assert(const char* file, u32 line,
                          const char* expr);             /* assert */

/* ===== Rodata string constants ===== */
extern const char lbl_802739E4[]; /* "generator.c" */
extern const char lbl_802739F0[]; /* "psCamera" */

/* ===== SDA21 float/double constants ===== */
extern f32 lbl_8047D6B0;     /* 0.0f */
extern f32 lbl_8047D6B4;     /* lifetime threshold */
extern f64 lbl_8047D6C0;     /* 0.5 (Newton-Raphson constant) */
extern f64 lbl_8047D6C8;     /* 3.0 (Newton-Raphson constant) */
extern f64 lbl_8047D6D0;     /* small negative threshold */
extern f32 lbl_8047D6D8;     /* pi/2 (90 degrees) */
extern f32 lbl_8047D6DC;     /* -pi/2 (-90 degrees) */

/* ===== SDA21 global pointers ===== */
extern void* lbl_8047B190;   /* psCamera object pointer */

/* ===== Special float constant in .data ===== */
extern f32 lbl_80478AC0;     /* small epsilon for float compare */
extern f32 lbl_80478AC8;     /* epsilon for atan2 zero-check */
extern f32 lbl_80478ACC;     /* minimum velocity threshold */

/* -----------------------------------------------------------------------
 * Generator work structure -- partial layout.
 *
 * Full structure is likely 0x100+ bytes; only fields accessed by
 * generatorMain are documented here.
 * ----------------------------------------------------------------------- */
typedef struct GeneratorWork {
    /* 0x00 */ u32  state;
    /* 0x04 */ u32  controlFlags;
    /* 0x08 */ f32  reserved08;
    /* 0x0C */ f32  lifetime;
    /* 0x10 */ u16  reserved10;
    /* 0x12 */ u16  emissionType;       /* bottom 4 bits */
    /* 0x14 */ u8   pad14[0x0C];
    /* 0x20 */ f32  posX;
    /* 0x24 */ f32  posY;
    /* 0x28 */ f32  posZ;
    /* 0x2C */ f32  velX;
    /* 0x30 */ f32  velY;
    /* 0x34 */ f32  velZ;
    /* 0x38 */ u8   pad38[0x50];
    /* 0x88 */ u16  scaleFlags;
    /* 0x8A */ u16  pad8A;
    /* 0x8C */ f32  rotX;
    /* 0x90 */ f32  rotY;
    /* 0x94 */ f32  rotZ;
    /* 0x98 */ f32  scaleX;
    /* 0x9C */ f32  scaleY;
    /* 0xA0 */ f32  scaleZ;
} GeneratorWork;

/* -----------------------------------------------------------------------
 * Internal helper: fast inverse square root (Newton-Raphson, 3 iterations).
 *
 * The PowerPC frsqrte instruction gives a low-precision estimate.
 * Three N-R refinement steps bring it to f32 precision.
 *
 * This pattern appears at 0x80174368-0x801743A8 in the disassembly:
 *   frsqrte f1, f27
 *   lfd f3, half_const       ; 0.5
 *   lfd f2, three_const      ; 3.0
 *   -- iteration 1 --
 *   fmul f0, f1, f1          ; est^2
 *   fmul f1, f3, f1          ; 0.5 * est
 *   fnmsub f0, f27, f0, f2   ; 3.0 - val * est^2
 *   fmul f1, f1, f0          ; 0.5 * est * (3 - val*est^2)
 *   -- iteration 2, 3: identical --
 *   fmul f27, f27, f0        ; val * final_factor = sqrt(val)
 *   frsp f27, f27            ; round to single
 * ----------------------------------------------------------------------- */
static f32 fastSqrt(f32 val) {
    /* Approximate via the hardware rsqrte + 3x Newton-Raphson */
    f32 est;
    f32 halfEst;
    f32 valTimesEstSq;

    if (val <= 0.0f) {
        return 0.0f;
    }

    /* Initial estimate from frsqrte */
    /* In actual PPC this is:  est = __frsqrte(val); */
    /* We simulate the refinement loop: */
    est = 1.0f;  /* placeholder -- actual code uses frsqrte */

    /* Three Newton-Raphson iterations for 1/sqrt(val) */
    /* iteration 1 */
    halfEst = 0.5f * est;
    valTimesEstSq = val * est * est;
    est = halfEst * (3.0f - valTimesEstSq);

    /* iteration 2 */
    halfEst = 0.5f * est;
    valTimesEstSq = val * est * est;
    est = halfEst * (3.0f - valTimesEstSq);

    /* iteration 3 */
    halfEst = 0.5f * est;
    valTimesEstSq = val * est * est;
    est = halfEst * (3.0f - valTimesEstSq);

    /* val * (1/sqrt(val)) = sqrt(val) */
    return val * est;
}

/* =======================================================================
 *  generatorMain / generateParticle_801947D4
 *  Address: 0x8017424C, Size: 0x14E0
 *
 *  Main per-frame update for a particle generator.
 *
 *  Stack frame: 0x260 bytes.
 *  Uses FPRs f19-f31 (all saved/restored).
 *  Uses GPR r31 for the generator work pointer.
 *
 *  The function is decomposed into logical phases below, corresponding
 *  to the assembly blocks.
 * ======================================================================= */
void generatorMain(void* gen) {
    GeneratorWork* gw = (GeneratorWork*)gen;
    f32 velX, velY, velZ;
    f32 velMagSq, velMag;
    u32 emType;
    u32 ctrlFlags;

    /* Local matrix storage (on stack in original) */
    f32 mtxRotX[12];     /* sp+0x10C: rotation about X axis */
    f32 mtxRotY[12];     /* sp+0x0DC: rotation about Y axis */
    f32 mtxRotZ[12];     /* sp+0x0AC: rotation about Z axis */
    f32 mtxResult[12];   /* sp+0x13C: combined rotation matrix */
    f32 dirVec[3];       /* sp+0x04C: normalised direction vector */
    f32 camDir[3];       /* sp+0x040: camera-relative direction */
    f32 upVec[3];        /* sp+0x028: up vector for look-at */
    f32 rightVec[3];     /* sp+0x034: right vector for look-at */

    /* ------- Phase 1: Early exit check ------- */
    /* The generator checks if its lifetime has expired */
    if (gw->lifetime < lbl_8047D6B4) {
        return;
    }

    /* ------- Phase 2: Read velocity vector ------- */
    velX = gw->velX;
    velY = gw->velY;
    velZ = gw->velZ;

    /* Check emission type for scaled emission */
    emType = gw->emissionType & 0xF;
    if (emType == 2 && (gw->scaleFlags & 0x0100)) {
        /* Scaled emission: multiply velocity by scale factors */
        velX *= gw->scaleX;
        velY *= gw->scaleY;
        velZ *= gw->scaleZ;
    }

    /* ------- Phase 3: Compute velocity magnitude ------- */
    velMagSq = velX * velX + velY * velY + velZ * velZ;

    if (velMagSq > 0.0f) {
        /* Use fast inverse square root with Newton-Raphson */
        velMag = fastSqrt(velMagSq);
    } else {
        /* Zero or negative: check for special float values */
        /* In the assembly, this handles NaN, denorm, and infinity
         * by checking the IEEE 754 exponent/mantissa fields.
         * If the value is a special case, use a small epsilon. */
        velMag = lbl_80478AC0;  /* fallback epsilon */
    }

    /* ------- Phase 4: Build rotation matrix ------- */
    /* Initialise to identity */
    fn_800A2D38(mtxResult);

    ctrlFlags = gw->controlFlags;

    /* Check if directional rotation is needed (bits 14-15 clear) */
    if ((ctrlFlags & 0x00030000) == 0) {
        /* Build axis-angle rotation matrices from rotX, rotY, rotZ */
        /* MTXRotAxis(angle, output, axis):
         *   axis 0x58 = X-axis
         *   axis 0x59 = Y-axis
         *   axis 0x5A = Z-axis */
        fn_800A3074(gw->rotX, mtxRotX, 0x58);
        fn_800A3074(gw->rotY, mtxRotY, 0x59);
        fn_800A3074(gw->rotZ, mtxRotZ, 0x5A);

        /* Combine: result = rotY * rotX */
        fn_800A2D98(mtxRotY, mtxRotX, mtxRotX);

        /* Combine: result = rotZ * (rotY * rotX) */
        fn_800A2D98(mtxRotZ, mtxRotX, mtxRotX);

        /* Extract the forward direction (column 0) */
        dirVec[0] = mtxRotX[0];   /* mtx[0][0] at sp+0x10C */
        dirVec[1] = mtxRotX[4];   /* mtx[1][0] at sp+0x11C */
        dirVec[2] = mtxRotX[8];   /* mtx[2][0] at sp+0x12C */

        /* Normalise the direction */
        fn_800A3ADC(dirVec, dirVec);

        /* Store into the result matrix columns */
        mtxResult[0]  = dirVec[0];  /* row 0, col 0 */
        mtxResult[4]  = dirVec[1];  /* row 1, col 0 */
        mtxResult[8]  = dirVec[2];  /* row 2, col 0 */

        /* Extract right and up from remaining matrix columns */
        /* (second column) */
        dirVec[0] = mtxRotX[1];
        dirVec[1] = mtxRotX[5];
        dirVec[2] = mtxRotX[9];
        fn_800A3ADC(dirVec, dirVec);
        mtxResult[1]  = dirVec[0];
        mtxResult[5]  = dirVec[1];
        mtxResult[9]  = dirVec[2];

        /* (third column) */
        dirVec[0] = mtxRotX[2];
        dirVec[1] = mtxRotX[6];
        dirVec[2] = mtxRotX[10];
        fn_800A3ADC(dirVec, dirVec);
        mtxResult[2]  = dirVec[0];
        mtxResult[6]  = dirVec[1];
        mtxResult[10] = dirVec[2];

        /* Zero the translation column */
        mtxResult[3]  = 0.0f;
        mtxResult[7]  = 0.0f;
        mtxResult[11] = 0.0f;
    }

    /* ------- Phase 5: Camera-tracking mode ------- */
    /* If controlFlags bit 17 is set, override with camera-relative direction */
    if (ctrlFlags & 0x00020000) {
        void* cameraObj = lbl_8047B190;

        /* Assert the camera object exists */
        if (cameraObj == NULL) {
            __assert(lbl_802739E4, 0x272, lbl_802739F0);
        }

        cameraObj = lbl_8047B190;

        /* Compute direction from generator to camera:
         * camDir = camera.pos - generator.pos */
        {
            void* camData = *(void**)((u8*)cameraObj + 0x24);
            camDir[0] = *(f32*)((u8*)camData + 0x0C) - gw->posX;
            camDir[1] = *(f32*)((u8*)camData + 0x10) - gw->posY;
            camDir[2] = *(f32*)((u8*)camData + 0x14) - gw->posZ;
        }

        /* Normalise camera direction */
        fn_800A3ADC(camDir, camDir);

        /* Get camera orientation matrix */
        HSD_CObjGetUpVector(lbl_8047B190, rightVec);

        /* Normalise the right vector */
        fn_800A3ADC(rightVec, rightVec);

        /* Compute up = right x camDir */
        fn_800A3B9C(rightVec, camDir, upVec);

        /* Recompute right = camDir x up (ensure orthonormal) */
        fn_800A3B9C(camDir, upVec, rightVec);

        /* Store into result matrix */
        mtxResult[0]  = upVec[0];
        mtxResult[4]  = upVec[1];
        mtxResult[8]  = upVec[2];
        mtxResult[1]  = rightVec[0];
        mtxResult[5]  = rightVec[1];
        mtxResult[9]  = rightVec[2];
        mtxResult[2]  = camDir[0];
        mtxResult[6]  = camDir[1];
        mtxResult[10] = camDir[2];
    }

    /* ------- Phase 6: Velocity-based yaw/pitch ------- */
    /* If emission type != 1 (not camera-only) and velocity is significant */
    emType = gw->emissionType & 0xF;
    if (emType != 1 && velMag > lbl_80478ACC) {
        f32 normVelX, normVelY, normVelZ;
        f32 yawAngle, pitchAngle;
        f32 sinYaw, cosYaw, sinPitch, cosPitch;
        f32 horizMag;

        /* Normalise the velocity */
        normVelX = gw->velX;
        normVelY = gw->velY;
        normVelZ = gw->velZ;
        fn_800A3ADC(&normVelX, &normVelX);

        /* Compute pitch angle (elevation):
         * The vertical component (Y) determines elevation.
         * Use atan2(Y, horizontal_magnitude) for the pitch. */
        horizMag = normVelZ * cosYaw + normVelY * sinYaw;

        /* Clamp near-zero horizontal magnitude */
        if (horizMag < lbl_80478AC8 && horizMag > -lbl_80478AC8) {
            if (normVelY >= 0.0f) {
                pitchAngle = lbl_8047D6D8;   /* pi/2 */
            } else {
                pitchAngle = lbl_8047D6DC;   /* -pi/2 */
            }
        } else {
            pitchAngle = (f32)fn_800CE2D8(normVelY, horizMag);
        }

        /* Compute sin/cos for pitch */
        fn_800CE148(pitchAngle);
        /* sinPitch stored; cosPitch computed next */
        fn_800CDBE0(pitchAngle);

        /* Compute yaw angle (azimuth):
         * The horizontal components (X, Z) determine the heading. */
        horizMag = normVelZ * cosPitch + normVelX * sinPitch;

        if (horizMag < lbl_80478AC8 && horizMag > -lbl_80478AC8) {
            if (normVelX >= 0.0f) {
                yawAngle = lbl_8047D6D8;
            } else {
                yawAngle = lbl_8047D6DC;
            }
        } else {
            yawAngle = (f32)fn_800CE2D8(normVelX, horizMag);
        }

        fn_800CE148(yawAngle);
        fn_800CDBE0(yawAngle);

        /* Build yaw and pitch rotation matrices, combine with result */
        /* (The actual assembly builds these inline using the computed
         *  sin/cos values and multiplies them into mtxResult.) */
    }

    /* ------- Phase 7: Particle emission ------- */
    /* The remainder of the function (several hundred bytes) handles:
     * - Reading emission count and rate from the generator config
     * - Computing random offsets for particle spread
     * - Transforming particle initial positions/velocities by mtxResult
     * - Spawning individual particles via the particle system
     *
     * This portion requires additional reverse engineering of the
     * particle spawn API and random number generation to decompose
     * fully. */
}
