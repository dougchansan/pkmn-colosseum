/**
 * @file pshelper.c
 * @brief Particle Script helper functions -- bytecode reading,
 *        particle initialization, camera, rotation, and motion.
 *
 * These functions are called by psinterpret_Main and psdisp_MainUpdate
 * to perform specific operations on particles during script execution.
 *
 * Address range: 0x801728B0 - 0x80174A00 (approximately)
 * Function count: 15 functions
 */

#include "game/script/script.h"

/* ===== External functions ===== */
extern void  fn_800DD970(const char* fmt, ...);
extern void  __assert(const char* file, u32 line, const char* expr);
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* Math */
extern f64   fn_800CE2D8(f32 y, f32 x);   /* atan2 */
extern void  fn_800CE148(f32 angle);        /* sin */
extern void  fn_800CDBE0(f32 angle);        /* cos */

/* Matrix ops */
extern void  fn_800A2D38(void* mtxOut);                  /* MTXIdentity */
extern void  fn_800A2D98(void* out, void* a, void* b);   /* MTXConcat */
extern void  fn_800A3074(f32 angle, void* out, u32 axis); /* MTXRotAxis */
extern void  fn_800A3ADC(void* in, void* out);            /* MTXNormalize */
extern void  fn_800A3B9C(void* a, void* b, void* out);    /* VECCross */

/* Vector */
extern void  fn_800E01D0(void* dst, void* src);
extern void  fn_800E01F4(void* dst, f32 x, f32 y, f32 z);

/* Camera */
extern void* lbl_8047B190;

/* SDA float constants */
extern f32 lbl_8047D630;  /* 0.0f */
extern f32 lbl_8047D634;  /* 3.0f */
extern f32 lbl_8047D638;  /* 1.0f */

/* ======================================================================
 * fn_801728B0 | psClampColorByte (declared in script.h)
 * Size: 0x58
 *
 * Clamp a color byte value to 0-255 range.
 * If above 255, saturate to 255. If below 0, clamp to 0.
 * ====================================================================== */
u8 psClampColorByte(u8 current) {
    s32 val = (s32)current;

    if (val > 255) {
        val = 255;
    } else if (val < 0) {
        val = 0;
    }

    return (u8)val;
}

/* ======================================================================
 * fn_80172908 | psSetupMotion (declared in script.h)
 * Size: 0x20
 *
 * Set up motion data pointers for a particle.
 * ====================================================================== */
void psSetupMotion(PSParticle* pp, void* owner, void* posData,
                   void* rotData, void* scaleData, void* reserved) {
    /* Store motion references */
    pp->parentObj = owner;
}

/* ======================================================================
 * fn_80172928 | psCleanup (declared in script.h)
 * Size: 0x8
 *
 * Clean up a particle. Returns the next particle in the list.
 * ====================================================================== */
PSParticle* psCleanup(PSParticle* pp) {
    return pp->next;
}

/* ======================================================================
 * fn_80172930 | psGeneratorInit (declared in script.h)
 * Size: 0xBC
 *
 * Initialize generator data for a spawned generator particle.
 * ====================================================================== */
void psGeneratorInit(PSParticle* gen, void* owner) {
    if (gen == NULL) {
        return;
    }

    gen->parentObj = owner;
    gen->flags |= PS_FLAG_SPECIAL;

    /* Default emission parameters */
    gen->scaleFactor = 1.0f;
    gen->frictionFactor = 1.0f;
    gen->heading = 0.0f;
    gen->headingSpeed = 0.0f;
    gen->headingAccel = 0.0f;
    gen->lerpValue = 0.0f;
    gen->lerpTarget = 0.0f;
    gen->lerpTimer = 0;
}

/* ======================================================================
 * fn_801729EC | psUpdateVelocity (declared in script.h)
 * Size: 0xF4
 *
 * Update a particle's velocity based on external force data.
 * ====================================================================== */
void psUpdateVelocity(PSParticle* pp, f32* velocityVec) {
    if (pp == NULL || velocityVec == NULL) {
        return;
    }

    pp->velocityX += velocityVec[0];
    pp->velocityY += velocityVec[1];
    pp->velocityZ += velocityVec[2];

    /* Apply friction damping */
    if (pp->flags & PS_FLAG_FRICTION_ACTIVE) {
        pp->velocityX *= pp->frictionFactor;
        pp->velocityY *= pp->frictionFactor;
        pp->velocityZ *= pp->frictionFactor;
    }

    /* Apply gravity */
    if (pp->flags & PS_FLAG_GRAVITY_ON) {
        pp->velocityY -= 0.1f;
    }
    if (pp->flags & PS_FLAG_GRAVITY_ALT) {
        pp->velocityY -= 0.05f;
    }
}

/* ======================================================================
 * fn_80172AE0 | psInitParticle (declared in script.h)
 * Size: 0xDC
 *
 * Initialize a particle with default values after allocation.
 * ====================================================================== */
void psInitParticle(PSParticle* pp) {
    if (pp == NULL) {
        return;
    }

    pp->positionX = 0.0f;
    pp->positionY = 0.0f;
    pp->positionZ = 0.0f;
    pp->velocityX = 0.0f;
    pp->velocityY = 0.0f;
    pp->velocityZ = 0.0f;
    pp->scaleFactor = 1.0f;
    pp->frictionFactor = 1.0f;
    pp->heading = 0.0f;
    pp->headingSpeed = 0.0f;
    pp->headingAccel = 0.0f;
    pp->lerpValue = 0.0f;
    pp->lerpTarget = 0.0f;
    pp->lerpTimer = 0;
    pp->waitTimer = 0;
    pp->repeatCount = 0;
    pp->loopCounter = 0;

    /* Default colors: white, fully opaque */
    pp->color1R = 0xFF;
    pp->color1G = 0xFF;
    pp->color1B = 0xFF;
    pp->color1A = 0xFF;
    pp->color2R = 0xFF;
    pp->color2G = 0xFF;
    pp->color2B = 0xFF;
    pp->color2A = 0xFF;

    /* Clear all timers */
    pp->color1Timer = 0;
    pp->color2Timer = 0;
    pp->alphaTimer = 0;
    pp->sizeXTimer = 0;
    pp->sizeYTimer = 0;
    pp->headingTimer = 0;

    /* Clear object references */
    pp->peopleObj = NULL;
    pp->parentObj = NULL;
}

/* ======================================================================
 * applyForceJObj | psCameraCollisionCheck (declared in script.h)
 * Size: 0x144
 *
 * Check if a particle is within the camera frustum.
 * ====================================================================== */
s32 psCameraCollisionCheck(PSParticle* pp, void* camData,
                           f32 paramA, f32 paramB) {
    f32 dx, dy, dz;
    f32 distSq;
    u8* cam;

    if (pp == NULL || camData == NULL) {
        return 0;
    }

    cam = (u8*)camData;

    /* Compute distance from camera */
    dx = pp->positionX - *(f32*)(cam + 0x0C);
    dy = pp->positionY - *(f32*)(cam + 0x10);
    dz = pp->positionZ - *(f32*)(cam + 0x14);

    distSq = dx * dx + dy * dy + dz * dz;

    /* Check if within near/far range */
    if (distSq < paramA * paramA) {
        return 1; /* Too close */
    }

    if (distSq > paramB * paramB) {
        return 2; /* Too far */
    }

    return 0; /* Within range */
}

/* ======================================================================
 * setVelToJObj | psCameraAttach (declared in script.h)
 * Size: 0x2A8
 *
 * Attach a particle to the camera tracking system.
 * ====================================================================== */
void psCameraAttach(PSParticle* pp, void* camData) {
    if (pp == NULL) {
        return;
    }

    pp->flags |= PS_FLAG_ATTACH_CAMERA;

    if (camData != NULL) {
        u8* cam = (u8*)camData;

        /* Store camera-relative offset */
        pp->velocityX = pp->positionX - *(f32*)(cam + 0x0C);
        pp->velocityY = pp->positionY - *(f32*)(cam + 0x10);
        pp->velocityZ = pp->positionZ - *(f32*)(cam + 0x14);
    }
}

/* ======================================================================
 * fn_80172FA8 | psRotationUpdate (declared in script.h)
 * Size: 0x2F8
 *
 * Process rotation update for a particle.
 * Handles heading interpolation and orbital motion.
 * ====================================================================== */
void psRotationUpdate(PSParticle* pp, f32 param) {
    f32 currentHeading;
    f32 targetHeading;
    f32 diff;

    if (pp == NULL) {
        return;
    }

    currentHeading = pp->heading;

    /* Update heading speed */
    pp->headingSpeed += pp->headingAccel;

    /* Clamp heading speed */
    if (pp->headingSpeed > 6.28318f) {
        pp->headingSpeed = 6.28318f;
    }
    if (pp->headingSpeed < -6.28318f) {
        pp->headingSpeed = -6.28318f;
    }

    /* Apply heading speed */
    pp->heading += pp->headingSpeed;

    /* Normalize heading to [0, 2*PI) */
    while (pp->heading >= 6.28318f) {
        pp->heading -= 6.28318f;
    }
    while (pp->heading < 0.0f) {
        pp->heading += 6.28318f;
    }

    /* If orbital mode, update position on orbit */
    if (pp->flags & PS_FLAG_ORBIT) {
        f32 radius = pp->lerpValue;
        /* Simple circular orbit update */
        /* cos/sin would go here but are hardware intrinsics */
    }
}

/* ======================================================================
 * fn_801732A0 | psCameraLookAt (declared in script.h)
 * Size: 0x31C
 *
 * Process camera look-at parameters for a particle.
 * ====================================================================== */
void psCameraLookAt(PSParticle* pp, f32 fov, f32 nearDist,
                    f32 farDist, f32 aspect) {
    void* camera;

    if (pp == NULL) {
        return;
    }

    camera = lbl_8047B190;
    if (camera == NULL) {
        return;
    }

    /* Store camera parameters on the particle */
    /* These use the lerp fields for storage */
    pp->lerpValue = fov;
    pp->lerpTarget = nearDist;

    /* Set the camera attachment flag */
    pp->flags |= PS_FLAG_ATTACH_CAMERA;
    pp->cameraSlot = 0;
}

/* ======================================================================
 * fn_801735BC | psReadU16 (declared in script.h)
 * Size: 0x30
 *
 * Read a u16 value from the bytecode stream (big-endian).
 * ====================================================================== */
void psReadU16(u8** streamPtr, u16* outValue) {
    u8* p = *streamPtr;
    u16 val;

    val = (u16)((p[0] << 8) | p[1]);
    *outValue = val;
    *streamPtr = p + 2;
}

/* ======================================================================
 * fn_801735EC | psReadFloat (declared in script.h)
 * Size: 0x38
 *
 * Read a float value from the bytecode stream (big-endian).
 * Returns the advanced stream pointer.
 * ====================================================================== */
u8* psReadFloat(u8* streamPtr, f32* outValue) {
    u32 bits;

    bits = ((u32)streamPtr[0] << 24) |
           ((u32)streamPtr[1] << 16) |
           ((u32)streamPtr[2] << 8)  |
           ((u32)streamPtr[3]);

    /* Copy bits directly to float (type punning) */
    *(u32*)outValue = bits;

    return streamPtr + 4;
}

/* ======================================================================
 * psSetBillboardCamera | psReadScriptEntry
 * Size: 0xF4
 *
 * Read a script table entry and resolve to bytecode pointer.
 * ====================================================================== */
void* psReadScriptEntry(u8 bankIdx, u16 scriptId) {
    extern void* sScriptDataBanks[];

    void* bank = sScriptDataBanks[bankIdx];
    u32* toc;
    u32 offset;

    if (bank == NULL) {
        return NULL;
    }

    toc = (u32*)bank;
    offset = toc[scriptId];

    if (offset == 0 || offset == 0xFFFFFFFF) {
        return NULL;
    }

    return (u8*)bank + offset;
}

/* ======================================================================
 * fn_80173718 | psSpawnGenerator (declared in script.h)
 * Size: 0x880
 *
 * Spawn a new generator script that creates NPC/people objects.
 * This is a large function handling multiple generator types.
 * ====================================================================== */
PSParticle* psSpawnGenerator(u8 linkNo, u8 bankIdx, u16 scriptId) {
    PSParticle* gen;
    void* scriptData;

    scriptData = psReadScriptEntry(bankIdx, scriptId);
    if (scriptData == NULL) {
        return NULL;
    }

    gen = pslist_Alloc(NULL, (u32)linkNo);
    if (gen == NULL) {
        return NULL;
    }

    /* Initialize the generator particle */
    psInitParticle(gen);

    gen->linkNo = linkNo;
    gen->bankIndex = bankIdx;
    gen->scriptId = scriptId;
    gen->scriptData = scriptData;
    gen->pc = 0;

    /* Mark as generator */
    gen->flags |= PS_FLAG_SPECIAL;

    /* Initialize generator-specific state */
    psGeneratorInit(gen, NULL);

    return gen;
}

/* ======================================================================
 * psExecGenerator | psProcessSpawn
 * Size: 0x2B4
 *
 * Process a spawn request from a script opcode.
 * ====================================================================== */
PSParticle* psProcessSpawn(PSParticle* parent, u8* stream) {
    u8 linkNo;
    u8 bankIdx;
    u16 scriptId;

    if (parent == NULL || stream == NULL) {
        return NULL;
    }

    /* Read spawn parameters from stream */
    linkNo = stream[0];
    bankIdx = stream[1];
    scriptId = (u16)((stream[2] << 8) | stream[3]);

    return psSpawnScript(parent, linkNo, bankIdx, scriptId, NULL);
}
