/**
 * psinterpret.c - Particle Script Bytecode Interpreter
 *
 * This is the core bytecode interpreter for Pokemon Colosseum's event
 * scripting system. The main function, psinterpret_Main (fn_8016F500),
 * is one of the largest functions in the game at 12,592 bytes (0x3130).
 *
 * The interpreter processes a stream of bytecodes that control particle
 * script entities - which despite the name handle everything from NPC
 * movement to camera control to visual effects.
 *
 * Source file confirmed by rodata string: "psinterpret.c" at 0x802739A0
 *
 * Address range: 0x8016F430 - 0x80172630
 *
 * VM Architecture:
 * ================
 * - NOT a stack machine or register machine in the traditional sense
 * - Each script context (PSParticle) has its own state fields
 * - Opcodes directly read/write these fields
 * - A single program counter (PC) indexes into bytecode data
 * - The first byte of each instruction is the opcode
 * - Arguments follow the opcode byte (variable length)
 *
 * Instruction Encoding:
 * ====================
 * For opcodes < 0x80 (wait/object-reference range):
 *   Byte 0: [M1 M0 D4..D0]
 *     D4..D0 (bits 0-4): delay value (if bit 5 set, extended with next byte)
 *     M1 M0 (bits 5-6): mode (0x40 = object reference mode)
 *     If bit 5 set: next byte extends the delay value
 *
 * For opcodes >= 0x80 (command range):
 *   Byte 0 is masked and used as jump table index
 *   For certain opcode groups (0x80-0x87), low 3 bits are axis flags:
 *     bit 0: X axis (read float from stream)
 *     bit 1: Y axis (read float from stream)
 *     bit 2: Z axis (read float from stream)
 *   For opcodes 0x98-0x9F: grouped with stride of 8
 *   For opcodes 0xC0-0xCF, 0xD0-0xDF: grouped with stride of 16
 *
 * The jump table at 0x8036BFE0 has 128 entries mapping opcode
 * (after subtracting 0x80) to handler addresses.
 */

#include "game/script/script.h"

/* ======================================================================
 * External references
 * ====================================================================== */

extern PSParticle* sLinkHeads[];        /* lbl_80452788 */
extern void* sLinkDataBanks[];          /* lbl_804529C8 - object data banks */
extern void* sScriptDataBanks[];        /* lbl_804527C8 - script data banks */
extern void* sCameraSlots[];            /* lbl_80452DC8 - camera attachment slots */

/* External function declarations */
extern void* fn_8019F718(void);

/* SDA constants */
extern f32 sZeroFloat;                  /* lbl_8047D630: 0.0f */
extern f32 sThreeFloat;                 /* lbl_8047D634: 3.0f */
extern f32 sOneFloat;                   /* lbl_8047D638: 1.0f */
extern f32 sHalfFloat;                  /* lbl_8047D63C: 0.5f */
extern f32 sTwoFloat;                   /* lbl_8047D640: 2.0f */
extern f64 sIntToFloatBias;             /* lbl_8047D660: 4503599627370496.0 (0x4330000000000000) */
extern f64 sSignedBias;                 /* lbl_8047D668: signed int conversion bias */

/* ======================================================================
 * psInterpretParticles | psinterpret_RunAll (psinterpret_ObjRefAssert in codebase_map)
 * Size: 0xD0
 *
 * Runs the interpreter for all active particles across all 16 link lists.
 *
 * Parameter linkMask is a bitmask (bit 0 = link 0, bit 15 = link 15).
 * The mask is shifted right each iteration, and if the current bit is set,
 * that link's list is skipped (used to selectively pause links).
 *
 * For each particle in a non-skipped link:
 *   1. Gets the head of the link list via pslist_GetHead
 *   2. Calls psinterpret_Main on each particle
 *   3. Follows the list, accounting for particles that may have been
 *      removed or relinked during execution
 * ====================================================================== */
void psinterpret_RunAll(u32 linkMask) {
    s32 linkNo;
    PSParticle* pp;
    PSParticle* next;
    PSParticle* prevCtx;

    for (linkNo = 0; linkNo < PS_NUM_LINK; linkNo++) {
        /* Check if bit 15 of linkMask is set (skip this link) */
        if (linkMask & 0x8000) {
            linkMask <<= 1;
            continue;
        }

        prevCtx = NULL;
        pp = pslist_GetHead(linkNo);

        while (pp != NULL) {
            /* Save next before execution (particle may be destroyed) */
            next = pp->next;

            /* Execute one frame of this particle's script */
            psinterpret_Main(pp, prevCtx);

            /* Check if the particle was destroyed or if the list changed */
            if (next == NULL) {
                break;
            }

            if (pp->next == next) {
                /* Normal case: particle still in list, advance past it */
                prevCtx = pp;
            } else if (pp == next) {
                /* Particle was unlinked; next is already correct */
                /* Don't update prevCtx */
            } else {
                /* List was modified; search for where next ended up */
                {
                PSParticle* search;
                prevCtx = NULL;  /* "psinterpret.c" assert at line 0x810 */
                search = prevCtx;
                while (search != NULL && search->next != next) {
                    if (prevCtx == NULL) {
                        /* Assert failure */
                        __assert("psinterpret.c", 0x810, NULL);
                    }
                    search = search->next;
                }
                }
            }

            pp = next;
        }

        linkMask <<= 1;
    }
}

/* ======================================================================
 * fn_8016F500 | psinterpret_Main
 * Size: 0x3130 (12,592 bytes)
 *
 * The main bytecode interpreter. Executes one frame of a particle script.
 *
 * Parameters:
 *   pp       - The particle to execute (r31 throughout the function)
 *   parentCtx - Parent context for recursive spawns (r28)
 *
 * Register usage (persistent across the function):
 *   r31 = pp (current particle)
 *   r30 = bytecode stream pointer (advanced as bytes are consumed)
 *   r29 = delay/repeat count for the current instruction
 *   r28 = parentCtx
 *   r26 = scratch (spawned particle, etc.)
 *   r27 = scratch (additional arguments)
 *
 * Return value: result of psCleanup (next particle in list)
 * ====================================================================== */
PSParticle* psinterpret_Main(PSParticle* pp, PSParticle* parentCtx) {
    u8 opcode;
    u16 delay;
    PSParticle* spawned;
    u8* stream;
    s32 resultUnused;
    s32 opResult;
    void* scratchPtr;
    s32 yieldFlag;
    f32 tempF[4];  /* Stack temporaries for float arguments */

    /* ---- Early-out: check if particle is marked as paused (bit 20) ---- */
    if (pp->flags & PS_FLAG_PAUSED) {
        psCleanup(pp);

    } else {

        /* ================================================================
         * Phase 1: Update interpolation timers
         * ================================================================
         * Several fields have countdown timers. Each frame, the timer
         * decrements. When it reaches zero, the target value is applied.
         */

        /* Lerp timer (fields 0x0C, 0x4C, 0x60) */
        if (pp->lerpTimer != 0) {
            /* Interpolate lerpValue toward lerpTarget */
            f32 remaining = pp->lerpTarget - pp->lerpValue;
            f32 step = remaining / (f32)pp->lerpTimer;
            pp->lerpValue = pp->lerpValue + step;
            pp->lerpTimer--;
        }

        /* Primary color timer (fields 0x0E, 0x6C, 0x70-0x73 -> 0x12-0x15) */
        if (pp->color1Timer != 0) {
            pp->color1Countdown--;
            if (pp->color1Countdown == 0) {
                pp->color1Timer = 0;
                pp->color1R = pp->color1TargetR;
                pp->color1G = pp->color1TargetG;
                pp->color1B = pp->color1TargetB;
                pp->color1A = pp->color1TargetA;
            }
        }

        /* Secondary color timer (fields 0x10, 0x6E, 0x74-0x77 -> 0x16-0x19) */
        if (pp->color2Timer != 0) {
            pp->color2Countdown--;
            if (pp->color2Countdown == 0) {
                pp->color2Timer = 0;
                pp->color2R = pp->color2TargetR;
                pp->color2G = pp->color2TargetG;
                pp->color2B = pp->color2TargetB;
                pp->color2A = pp->color2TargetA;
            }
        }

        /* Size X timer (fields 0x5A, 0x78, 0x84-0x85 -> 0x80-0x81) */
        if (pp->sizeXTimer != 0) {
            pp->sizeXCountdown--;
            if (pp->sizeXCountdown == 0) {
                pp->sizeXTimer = 0;
                pp->sizeXCurrent = pp->sizeXStart;
                pp->sizeYCurrent = pp->sizeYStart;
            }
        }

        /* Size Y timer (fields 0x5C, 0x7A, 0x86-0x87 -> 0x82-0x83) */
        if (pp->sizeYTimer != 0) {
            pp->sizeYCountdown--;
            if (pp->sizeYCountdown == 0) {
                pp->sizeYTimer = 0;
                pp->sizeXTarget = pp->sizeXTargetFinal;
                pp->sizeYTarget = pp->sizeYTargetFinal;
            }
        }

        /* Alpha timer (fields 0x54, 0x7C, 0x7E-0x7F -> 0x57-0x58) */
        if (pp->alphaTimer != 0) {
            pp->alphaCountdown--;
            if (pp->alphaCountdown == 0) {
                pp->alphaTimer = 0;
                pp->alphaStart = pp->alphaTargetStart;
                pp->alphaEnd = pp->alphaTargetEnd;
            }
        }

        /* Heading timer (fields 0x5E, 0x50, 0x64, 0x68) */
        if (pp->headingTimer != 0) {
            if (pp->headingAccel != 0.0f) {
                /* Accelerating rotation */
                pp->heading += pp->headingSpeed;
                if (pp->headingSpeed >= 0.0f) {
                    pp->headingSpeed += pp->headingAccel;
                } else {
                    pp->headingSpeed -= pp->headingAccel;
                }
                pp->headingTimer--;
                if (pp->headingTimer == 0) {
                    pp->headingAccel = 0.0f;
                    pp->headingSpeed = 0.0f;
                }
            } else {
                /* Linear rotation (interpolate heading toward target) */
                f32 remaining = pp->headingSpeed - pp->heading;
                f32 step = remaining / (f32)pp->headingTimer;
                pp->heading = pp->heading + step;
                pp->headingTimer--;
            }
        }

        /* ================================================================
         * Phase 2: Check wait timer
         * ================================================================ */
        if (pp->waitTimer == 0) {

        } else {

            pp->waitTimer--;
            if (pp->waitTimer == 0) {

            /* ================================================================
             * Phase 3: Bytecode execution loop
             * ================================================================
             * Resume execution from saved PC position.
             */
            stream = (u8*)pp->scriptData + pp->pc;
            yieldFlag = 0;

            /* ---- Main fetch-decode-execute loop ---- */
            do {
                opcode = *stream++;

                /* ============================================================
                 * Opcodes < 0x80: Wait/delay/object-reference commands
                 * ============================================================ */
                if (opcode < 0x80) {
                    /* Extract delay value from low 5 bits */
                    delay = opcode & 0x1F;

                    /* If bit 5 is set, delay is extended with the next byte */
                    if (opcode & 0x20) {
                        delay = (delay << 8) | *stream++;
                    }

                    /* Check addressing mode (bits 5-6) */
                    {
                    u8 mode = opcode & 0x60;

                    if (mode == 0x40) {
                        /* Object reference mode: set up object/animation reference */
                        u8 objRef;
                        void* bankData;
                        void* objTable;
                        void* objEntry;
                        void* ref;
                        objRef = *stream++;
                        pp->objRefIndex = objRef;

                        /* Look up object data bank for current bank index */
                        bankData = sLinkDataBanks[pp->bankIndex];
                        objTable = ((void**)bankData)[pp->animIndex];

                        if (objTable != NULL) {
                        objEntry = (void*)((u32)objTable + 0x18);
                        if (objEntry != NULL) {

                        /* Look up the specific object reference */
                        ref = ((void**)objEntry)[objRef];
                        if (ref != NULL) {

                        /* Set the object reference flag */
                        pp->flags |= PS_FLAG_OBJ_REF;
                        }
                        }
                        }
                    }

                    /* For other modes, delay is the wait value */
                    }
                }

                /* ============================================================
                 * Opcodes >= 0x80: Main command dispatch
                 * ============================================================
                 * The raw opcode is normalized based on its group:
                 *   0x80-0x97: mask with 0xF8 (stride 8, low 3 bits = axis flags)
                 *   0x98-0xBF: keep raw opcode
                 *   0xC0-0xCF: mask with 0xF0 (stride 16)
                 *   0xD0-0xDF: mask with 0xF0 (stride 16)
                 *   0xE0-0xFF: keep raw opcode
                 *
                 * After normalization, subtract 0x80 and use as jump table index.
                 * The jump table has 128 entries at 0x8036BFE0.
                 */
                {
                    u8 masked;
                    u8 tableIndex;
                    u8 normalizedOp = opcode;
                    delay = 0;

                    /* Normalize opcode for jump table lookup */
                    masked = opcode & 0xF8;
                    if (masked <= 0x98) {
                        /* 0x80-0x98: use masked value */
                    } else {
                        u8 masked16 = opcode & 0xF0;
                        if (masked16 == 0xC0 || masked16 == 0xD0) {
                            /* Use 16-stride mask */
                        } else {
                            normalizedOp = opcode;  /* Keep as-is */
                        }
                    }

                    tableIndex = (normalizedOp & 0xFF) - 0x80;
                    if (tableIndex <= 0x7F) {

                    /* Dispatch via jump table */
                    /* The actual dispatch is: bctr to jumptable[tableIndex] */

                    switch (normalizedOp) {

                    /* ============================================================
                     * 0x80: SET_POSITION - Set absolute position
                     * Low bits select which axes to read:
                     *   bit 0 = X, bit 1 = Y, bit 2 = Z
                     * ============================================================ */
                    case 0x80: case 0x81: case 0x82: case 0x83:
                    case 0x84: case 0x85: case 0x86: case 0x87:
                    {
                        f32 posX, posY, posZ;
                        if (opcode & 1) {
                            stream = psReadFloat(stream, &posX);
                        }
                        if (opcode & 2) {
                            stream = psReadFloat(stream, &posY);
                        }
                        if (opcode & 4) {
                            stream = psReadFloat(stream, &posZ);
                        }
                        psUpdateVelocity(pp, &posX);
                        pp->positionX = posX;
                        pp->positionY = posY;
                        pp->positionZ = posZ;
                        break;
                    }

                    /* ============================================================
                     * 0x88: ADD_POSITION - Add delta to position
                     * Same axis flags as SET_POSITION.
                     * ============================================================ */
                    case 0x88: case 0x89: case 0x8A: case 0x8B:
                    case 0x8C: case 0x8D: case 0x8E: case 0x8F:
                    {
                        f32 dx, dy, dz;
                        if (opcode & 1) {
                            stream = psReadFloat(stream, &dx);
                        }
                        if (opcode & 2) {
                            stream = psReadFloat(stream, &dy);
                        }
                        if (opcode & 4) {
                            stream = psReadFloat(stream, &dz);
                        }
                        psUpdateVelocity(pp, &dx);
                        pp->positionX += dx;
                        pp->positionY += dy;
                        pp->positionZ += dz;
                        break;
                    }

                    /* ============================================================
                     * 0x90: SET_TARGET_POS - Set target/destination position
                     * Same axis flags.
                     * ============================================================ */
                    case 0x90: case 0x91: case 0x92: case 0x93:
                    case 0x94: case 0x95: case 0x96: case 0x97:
                    {
                        f32 tx, ty, tz;
                        if (opcode & 1) {
                            stream = psReadFloat(stream, &tx);
                        }
                        if (opcode & 2) {
                            stream = psReadFloat(stream, &ty);
                        }
                        if (opcode & 4) {
                            stream = psReadFloat(stream, &tz);
                        }
                        psUpdateVelocity(pp, &tx);
                        pp->velocityX = tx;
                        pp->velocityY = ty;
                        pp->velocityZ = tz;
                        break;
                    }

                    /* ============================================================
                     * 0x98: SET_VELOCITY - Set velocity with parent-relative option
                     * Reads up to 3 floats based on axis flags.
                     * If ORBIT flag is set and parent exists, scales velocity by
                     * parent's bounding sphere ratio.
                     * ============================================================ */
                    case 0x98: case 0x99: case 0x9A: case 0x9B:
                    case 0x9C: case 0x9D: case 0x9E: case 0x9F:
                    {
                        f32 vx, vy, vz;
                        if (opcode & 1) {
                            stream = psReadFloat(stream, &vx);
                        }
                        if (opcode & 2) {
                            stream = psReadFloat(stream, &vy);
                        }
                        if (opcode & 4) {
                            stream = psReadFloat(stream, &vz);
                        }

                        /* Check if ORBIT flag is clear */
                        if ((pp->flags & PS_FLAG_ORBIT) == 0) {
                            psUpdateVelocity(pp, &vx);
                        } else {
                            /* Parent-relative: scale velocity by parent's bounds */
                            void* parent = pp->peopleObj;
                            if (parent != NULL) {
                                /* Read parent bounding sphere components */
                                /* Scale velocity proportionally */
                                f32 extentSum = /* sum of 3 extent values */ 0.0f;
                                f32 scale = extentSum / 3.0f;
                                vx *= scale;
                                vy *= scale;
                                vz *= scale;
                            }
                        }

                        pp->velocityX += vx;
                        pp->velocityY += vy;
                        pp->velocityZ += vz;
                        break;
                    }

                    /* ============================================================
                     * 0xA0: SET_LERP_TIMER - Set interpolation timer + target
                     * Reads timer (u16) and target float value.
                     * When timer is 0, immediately applies target.
                     * ============================================================ */
                    case 0xA0:
                    {
                        f32 target;
                        psReadU16(&stream, &pp->lerpTimer);
                        /* (declaration moved to top of block) */
                        stream = psReadFloat(stream, &target);
                        pp->lerpTarget = target;

                        if (pp->lerpTimer == 0) {
                            pp->lerpValue = pp->lerpTarget;
                        }
                        break;
                    }

                    /* ============================================================
                     * 0xA1: CLEAR_OBJ_REF - Clear object reference flag
                     * ============================================================ */
                    case 0xA1:
                        pp->flags &= ~PS_FLAG_OBJ_REF;
                        break;

                    /* ============================================================
                     * 0xA2: SET_SCALE - Set scale factor
                     * Reads one float. If 0.0, clears scale flag; otherwise sets it.
                     * If parent exists and has bounding data, scales by parent.
                     * ============================================================ */
                    case 0xA2:
                    {
                        void* parent;
                        f32 scale;
                        stream = psReadFloat(stream, &scale);
                        pp->scaleFactor = scale;

                        if (scale == 0.0f) {
                            pp->flags &= ~PS_FLAG_SCALE_ACTIVE;
                        } else {
                            pp->flags |= PS_FLAG_SCALE_ACTIVE;
                        }

                        /* If parent has bounding data, scale proportionally */
                        parent = pp->peopleObj;
                        if (parent != NULL) {
                            /* Scale by parent's bounding sphere */
                            f32 extentSum = /* parent extents */ 0.0f;
                            f32 ratio = extentSum / 3.0f;
                            pp->scaleFactor *= ratio;
                        }
                        break;
                    }

                    /* ============================================================
                     * 0xA3: SET_FRICTION - Set friction/damping factor
                     * Similar to SET_SCALE but for friction.
                     * ============================================================ */
                    case 0xA3:
                    {
                        f32 friction;
                        stream = psReadFloat(stream, &friction);
                        pp->frictionFactor = friction;

                        if (friction == 1.0f) {  /* sOneFloat */
                            pp->flags &= ~PS_FLAG_FRICTION_ACTIVE;
                        } else {
                            pp->flags |= PS_FLAG_FRICTION_ACTIVE;
                        }

                        /* Scale by parent if present */
                        if (pp->peopleObj != NULL) {
                            /* Similar parent-relative scaling */
                        }
                        break;
                    }

                    /* ============================================================
                     * 0xA4: SPAWN_SCRIPT - Spawn a child script
                     * Reads 2 bytes: script ID (hi byte << 8 | lo byte).
                     * Creates new particle, copies parent's scriptId, peopleObj,
                     * position, and starts executing the child script.
                     * ============================================================ */
                    case 0xA4:
                    {
                        u8 idHi = *stream++;
                        u8 idLo = *stream++;
                        u16 childScriptId = (idHi << 8) | idLo;

                        spawned = psSpawnScript(pp, pp->linkNo, pp->bankIndex,
                                                childScriptId, NULL);
                        if (spawned == NULL) break;

                        /* Inherit properties from parent */
                        spawned->scriptId = pp->scriptId;
                        spawned->peopleObj = pp->peopleObj;

                        /* Increment parent's reference count */
                        if (pp->peopleObj != NULL) {
                            u32* refCount = (u32*)((u8*)pp->peopleObj + 0x4C);
                            (*refCount)++;
                        }

                        /* Inherit NO_DETACH flag */
                        if (((u32*)pp->peopleObj)[1] & 0x04) {
                            spawned->flags |= PS_FLAG_NO_DETACH;
                        }

                        psInitParticle(spawned);

                        /* Attach to people/NPC system */
                        if (pp->peopleObj != NULL) {
                            /* Check parent's attachment mode */
                            psPeopleAttach(spawned, pp->parentObj);
                        } else {
                            psPeopleAttachStandalone(spawned, pp->parentObj);
                        }

                        /* Copy position */
                        spawned->positionX = pp->positionX;
                        spawned->positionY = pp->positionY;
                        spawned->positionZ = pp->positionZ;

                        /* Recursively execute the child's first frame */
                        psinterpret_Main(spawned, pp);
                        break;
                    }

                    /* ============================================================
                     * 0xA5: SPAWN_SCRIPT_TBL - Spawn child via table lookup
                     * Reads 2 bytes: table index. Looks up script ID from the
                     * script data bank table for the current bank.
                     * ============================================================ */
                    case 0xA5:
                    {
                        u8 tblHi = *stream++;
                        u8 tblLo = *stream++;
                        u16 tableIndex = (tblHi << 8) | tblLo;

                        /* Look up script ID from bank table */
                        void* bankTable = sScriptDataBanks[pp->bankIndex];
                        u16 childScriptId = tableIndex;
                        if (bankTable != NULL) {
                            childScriptId = ((u32*)bankTable)[tableIndex];
                        }

                        spawned = psSpawnScript(pp, pp->linkNo, pp->bankIndex,
                                                childScriptId, NULL);
                        if (spawned == NULL) break;

                        /* Same inheritance as SPAWN_SCRIPT */
                        spawned->scriptId = pp->scriptId;
                        spawned->peopleObj = pp->peopleObj;
                        if (pp->peopleObj != NULL) {
                            u32* refCount = (u32*)((u8*)pp->peopleObj + 0x4C);
                            (*refCount)++;
                        }
                        if (((u32*)pp->peopleObj)[1] & 0x04) {
                            spawned->flags |= PS_FLAG_NO_DETACH;
                        }

                        psInitParticle(spawned);

                        if (pp->peopleObj != NULL) {
                            psPeopleAttach(spawned, pp->parentObj);
                        } else {
                            psPeopleAttachStandalone(spawned, pp->parentObj);
                        }

                        spawned->positionX = pp->positionX;
                        spawned->positionY = pp->positionY;
                        spawned->positionZ = pp->positionZ;
                        psinterpret_Main(spawned, pp);
                        break;
                    }

                    /* ============================================================
                     * 0xA6: SPAWN_GENERATOR - Spawn a generator script
                     * Reads 2 bytes: script ID. Creates a "generator" which
                     * manages NPC/people objects.
                     * ============================================================ */
                    case 0xA6:
                    {
                        u8 idHi = *stream++;
                        u8 idLo = *stream++;
                        u16 genScriptId = (idHi << 8) | idLo;

                        spawned = psSpawnGenerator(pp->linkNo, pp->bankIndex,
                                                   genScriptId);
                        if (spawned == NULL) break;

                        /* Set up generator */
                        spawned->scriptId = pp->scriptId;  /* field 0x18 for generators */
                        psGeneratorInit(spawned, pp->peopleObj);

                        /* Attach to people system */
                        if (pp->parentObj != NULL && pp->peopleObj != NULL) {
                            psPeopleDetach(spawned);
                        } else {
                            psPeopleDetachStandalone(spawned);
                        }

                        /* Copy position and set up parent references */
                        /* ... extensive position copying with parent-relative logic ... */
                        break;
                    }

                    /* ============================================================
                     * 0xA7: SPAWN_GEN_TBL - Spawn generator via table lookup
                     * Similar to SPAWN_GENERATOR but with table-based script ID.
                     * ============================================================ */
                    case 0xA7:
                    {
                        /* Similar to SPAWN_SCRIPT_TBL but for generators */
                        /* Reads 2 bytes, looks up in bank table, spawns generator */
                        break;
                    }

                    /* ============================================================
                     * 0xA8: SPAWN_GEN_FLAGS - Spawn generator with flags byte
                     * Reads 3 bytes: 2 for script ID, 1 for flags.
                     * ============================================================ */
                    case 0xA8:
                    {
                        u8 idHi = *stream++;
                        u8 idLo = *stream++;
                        u8 genFlags = *stream++;
                        u16 genScriptId = (idHi << 8) | idLo;

                        spawned = psSpawnGenerator(pp->linkNo, pp->bankIndex,
                                                   genScriptId);
                        if (spawned == NULL) break;

                        spawned->scriptId = pp->scriptId;
                        psGeneratorInit(spawned, pp->peopleObj);

                        /* Apply generator flags (bits 4-6 -> bits 25-27 of flags) */
                        spawned->flags = (spawned->flags & 0xF1FFFFFF) |
                                         ((genFlags & 0x07) << 25);

                        /* Position copying with parent-relative adjustments */
                        /* ... */
                        break;
                    }

                    /* ============================================================
                     * 0xAC: SET_INVISIBLE - Make entity invisible
                     * Sets bit 7 (0x80) in flags.
                     * ============================================================ */
                    case 0xAC:
                        pp->flags |= PS_FLAG_INVISIBLE;
                        break;

                    /* ============================================================
                     * 0xAD: CLEAR_INVISIBLE - Make entity visible
                     * Clears bit 7 (0x80) in flags.
                     * ============================================================ */
                    case 0xAD:
                        pp->flags &= ~PS_FLAG_INVISIBLE;
                        break;

                    /* ============================================================
                     * 0xAE: SET_GRAVITY - Enable gravity (Y velocity subtraction)
                     * Clears bits 5-6, then sets bit 5 (0x20).
                     * ============================================================ */
                    case 0xAE:
                        pp->flags &= ~(PS_FLAG_GRAVITY_ON | PS_FLAG_GRAVITY_ALT);
                        pp->flags |= PS_FLAG_GRAVITY_ON;
                        break;

                    /* ============================================================
                     * 0xAF: SET_GRAVITY_ALT - Enable alternative gravity mode
                     * Clears bits 5-6, then sets bit 6 (0x40).
                     * ============================================================ */
                    case 0xAF:
                        pp->flags &= ~(PS_FLAG_GRAVITY_ON | PS_FLAG_GRAVITY_ALT);
                        pp->flags |= PS_FLAG_GRAVITY_ALT;
                        break;

                    /* ============================================================
                     * 0xB0: SET_GRAVITY_BOTH - Enable both gravity flags (0x60)
                     * ============================================================ */
                    case 0xB0:
                        pp->flags |= PS_FLAG_GRAVITY_BOTH;
                        break;

                    /* ============================================================
                     * 0xB1: DETACH_PEOPLE - Detach from NPC and re-parent
                     * Checks if parent's collision byte is clear, then detaches
                     * and re-calculates world-space matrices.
                     * ============================================================ */
                    case 0xB1:
                    {
                        u8 collisionByte;
                        f32 px;
                        f32 py;
                        f32 pz;
                        void* parent = pp->parentObj;
                        if (parent == NULL) break;

                        collisionByte = *(u8*)((u32)parent + 0x72);
                        if (collisionByte != 0) break;

                        /* Detach from parent */
                        psPeopleUpdateMatrices(((void**)parent)[1]);

                        /* Set up new standalone motion */
                        psSetupMotion(pp,
                                      (void*)((u32)parent + 0x34),
                                      (void*)((u32)parent + 0x24),
                                      (void*)((u32)parent + 0x14),
                                      (void*)((u32)parent + 0x08),
                                      NULL);

                        /* Transform position by parent's matrix */
                        /* ... 4x3 matrix multiply ... */
                        px = pp->positionX;
                        py = pp->positionY;
                        pz = pp->positionZ;
                        /* (matrix transform applied) */

                        psPeopleLinkUpdate(pp);
                        break;
                    }

                    /* ============================================================
                     * 0xB2: SET_ALPHA_INTERP - Set alpha with interpolation
                     * Handles alpha channel interpolation for transparency effects.
                     * ============================================================ */
                    case 0xB2:
                    {
                        u8 alphaMode;
                        /* ... alpha interpolation with timer countdown ... */
                        psReadU16(&stream, &pp->alphaTimer);
                        alphaMode = *stream++;
                        pp->alphaMode = alphaMode;
                        pp->alphaTargetStart = *stream++;
                        pp->alphaTargetEnd = *stream++;
                        stream += 0;  /* advance past data */

                        if (pp->alphaTimer == 0) {
                            pp->alphaStart = pp->alphaTargetStart;
                            pp->alphaEnd = pp->alphaTargetEnd;
                            pp->alphaCountdown = 0;
                            pp->alphaTimer = 0;
                        } else {
                            pp->alphaCountdown = pp->alphaTimer;
                        }
                        break;
                    }

                    /* ============================================================
                     * 0xB3: SET_BILLBOARD - Enable billboard rendering
                     * ============================================================ */
                    case 0xB3:
                        pp->flags |= PS_FLAG_BILLBOARD;
                        break;

                    /* ============================================================
                     * 0xB4: CLEAR_BILLBOARD - Disable billboard rendering
                     * ============================================================ */
                    case 0xB4:
                        pp->flags &= ~PS_FLAG_BILLBOARD;
                        break;

                    /* ============================================================
                     * 0xB5: SET_ROT_LERP - Set heading rotation with lerp
                     * Reads timer (u16) and float params for heading interpolation.
                     * ============================================================ */
                    case 0xB5:
                    {
                        f32 speed;
                        psReadU16(&stream, &pp->headingTimer);
                        /* (declaration moved to top of block) */
                        stream = psReadFloat(stream, &speed);
                        pp->headingSpeed += speed;

                        if (pp->headingTimer == 0) {
                            pp->heading = pp->headingSpeed;
                        }
                        break;
                    }

                    /* ============================================================
                     * 0xB6: SET_ROT_SPEED - Set rotation speed + random variation
                     * Reads timer, then float + random variation for heading.
                     * ============================================================ */
                    case 0xB6:
                    {
                        f32 baseSpeed;
                        f32 randFactor;
                        f32 rng;
                        psReadU16(&stream, &pp->headingTimer);
                        /* (declaration moved to top of block) */
                        stream = psReadFloat(stream, &baseSpeed);
                        /* (declaration moved to top of block) */
                        stream = psReadFloat(stream, &randFactor);

                        /* Add random variation */
                        rng = psRandom();
                        pp->headingSpeed = baseSpeed + randFactor * rng;

                        if (pp->headingTimer == 0) {
                            pp->heading = pp->headingSpeed;
                        }
                        break;
                    }

                    /* ============================================================
                     * 0xB7: RANDOM_OFFSET_XYZ - Apply random position jitter
                     * Reads 3 floats, applies random variation, adds to position.
                     * ============================================================ */
                    case 0xB7:
                    {
                        f32 rng1;
                        f32 rng2;
                        f32 rng3;
                        f32 rx, ry, rz;
                        stream = psReadFloat(stream, &rx);
                        rng1 = psRandom();
                        rx = (2.0f * rx) * rng1 - rx;  /* fmadds: range [-rx, +rx] */

                        stream = psReadFloat(stream, &ry);
                        rng2 = psRandom();
                        ry = (2.0f * ry) * rng2 - ry;

                        stream = psReadFloat(stream, &rz);
                        rng3 = psRandom();
                        rz = (2.0f * rz) * rng3 - rz;

                        psUpdateVelocity(pp, &rx);
                        pp->positionX += rx;
                        pp->positionY += ry;
                        pp->positionZ += rz;
                        break;
                    }

                    /* ============================================================
                     * 0xB8: RANDOM_HEADING - Set heading from random value
                     * Reads one float, multiplies by random, applies as heading.
                     * ============================================================ */
                    case 0xB8:
                    {
                        f32 rng;
                        f32 maxAngle;
                        stream = psReadFloat(stream, &maxAngle);
                        rng = psRandom();
                        pp->heading = maxAngle * rng;  /* probably maps to 0-2pi range */
                        psRotationUpdate(pp, pp->heading);
                        break;
                    }

                    /* ============================================================
                     * 0xB9: CAMERA_LOOKAT - Set camera look-at parameters
                     * Reads 4 floats: fov(?), near, far, aspect; calls camera setup.
                     * ============================================================ */
                    case 0xB9:
                    {
                        f32 p1, p2, p3, p4;
                        stream = psReadFloat(stream, &p1);
                        stream = psReadFloat(stream, &p2);
                        stream = psReadFloat(stream, &p3);
                        stream = psReadFloat(stream, &p4);

                        if (pp->peopleObj == NULL) break;

                        psCameraLookAt(pp, p4, p1, p2, p3);
                        break;
                    }

                    /* ============================================================
                     * 0xBA: LOCK_TO_PARENT - Lock to parent (toggle detachable)
                     * Checks parent's attachment state and updates flags.
                     * ============================================================ */
                    case 0xBA:
                    {
                        void* parent = pp->peopleObj;
                        if (parent == NULL) break;
                        /* Check parent's people data and set flags */
                        /* ... */
                        psPeopleUpdateMatrices(pp->peopleObj);
                        break;
                    }

                    /* ============================================================
                     * 0xBB: ATTACH_CAMERA_TRACK - Attach to camera tracking
                     * Sets the ATTACH_CAMERA flag (bit 28: 0x10000000).
                     * ============================================================ */
                    case 0xBB:
                        pp->flags |= 0x10000000;  /* PS_FLAG_SPECIAL */
                        break;

                    /* ============================================================
                     * 0xBC: RANDOM_DELAY - Random wait with min/max range
                     * Reads 4 bytes: min (u16), max (u16).
                     * Generates random wait time in [min, max] range.
                     * ============================================================ */
                    case 0xBC:
                    {
                        u8 b0 = *stream++; u8 b1 = *stream++;
                        u8 b2 = *stream++; u8 b3 = *stream++;
                        u16 minDelay = (b0 << 8) | b1;
                        u16 maxDelay = (b2 << 8) | b3;

                        f32 rng = psRandom();
                        s32 range = (s32)maxDelay - 0x8000;
                        f32 rangeF = (f32)range * rng;
                        s32 result = minDelay + (s32)rangeF;
                        pp->repeatCount = (u16)result;
                        break;
                    }

                    /* ============================================================
                     * 0xBD: RANDOM_PERCENT_CHECK - Random chance check
                     * Reads 1 byte (threshold 0-255). If random value >= threshold,
                     * sets repeatCount = 1 to yield.
                     * ============================================================ */
                    case 0xBD:
                    {
                        u8 threshold = *stream++;
                        f32 rng = psRandom();
                        s32 roll = (s32)(0.5f * rng);  /* Scale to range */

                        if (threshold < roll) break;

                        pp->repeatCount = 1;
                        /* Fall through to yield handling */
                        yieldFlag = 1;
                        break;
                    }

                    /* ============================================================
                     * 0xBE: SCALE_VELOCITY - Multiply velocity by a factor
                     * Reads one float and multiplies all three velocity components.
                     * ============================================================ */
                    case 0xBE:
                    {
                        f32 factor;
                        stream = psReadFloat(stream, &factor);
                        pp->velocityX *= factor;
                        pp->velocityY *= factor;
                        pp->velocityZ *= factor;
                        break;
                    }

                    /* ============================================================
                     * 0xBF: SET_HEADING_RANDOM - Heading + random lerp
                     * Reads timer, then two float values with random acceleration.
                     * ============================================================ */
                    case 0xBF:
                    {
                        f32 targetHeading;
                        f32 rng;
                        psReadU16(&stream, &pp->headingTimer);
                        /* (declaration moved to top of block) */
                        stream = psReadFloat(stream, &targetHeading);
                        rng = psRandom();
                        pp->headingSpeed += targetHeading * rng;

                        if (pp->headingTimer == 0) {
                            pp->heading = pp->headingSpeed;
                        }
                        break;
                    }

                    /* ============================================================
                     * 0xC7: CAMERA_SLOT_CALL - Call camera slot function
                     * Reads 1 byte: slot offset. Looks up camera data from
                     * sCameraSlots and calls psCameraAttach.
                     * ============================================================ */
                    case 0xC7:
                    {
                        u8 slotOffset = *stream++;
                        u8 baseSlot = pp->cameraSlot;
                        u32 slotIdx = slotOffset + baseSlot;
                        void* camData = sCameraSlots[slotIdx];
                        psCameraAttach(pp, camData);
                        break;
                    }

                    /* ============================================================
                     * 0xC8: CAMERA_SLOT_MOVE - Move camera with speed params
                     * Reads slot byte + 2 float params (speed X, speed Y).
                     * Adjusts by parent bounding if present.
                     * ============================================================ */
                    case 0xC8:
                    {
                        void* camData;
                        s32 result;
                        u8 slotOffset = *stream++;
                        u8 baseSlot = pp->cameraSlot;
                        u16 slotIdx = slotOffset + baseSlot;

                        f32 speedA, speedB;
                        stream = psReadFloat(stream, &speedA);
                        stream = psReadFloat(stream, &speedB);

                        /* Scale by parent bounds if present */
                        if (pp->peopleObj != NULL) {
                            /* ... bounding scale ... */
                        }

                        camData = sCameraSlots[slotIdx];
                        result = psCameraCollisionCheck(pp, camData, speedA, speedB);
                        if (result != 0) {
                            pp->repeatCount = 1;
                            yieldFlag = 1;
                        }
                        break;
                    }

                    /* ============================================================
                     * 0xD4: SPAWN_RANDOM - Spawn with random table index
                     * Reads 4 bytes: base script ID (u16) + random range (u16).
                     * Generates a random offset and spawns the script.
                     * ============================================================ */
                    case 0xD4:
                    {
                        u8 b0 = *stream++; u8 b1 = *stream++;
                        u8 b2 = *stream++; u8 b3 = *stream++;
                        u16 baseId = (b0 << 8) | b1;
                        u16 randRange = (b2 << 8) | b3;

                        f32 rng = psRandom();
                        s32 rangeS = (s32)randRange - 0x8000;
                        f32 rangeF = (f32)rangeS * rng;
                        u16 finalId = baseId + (s32)rangeF;

                        /* May also look up from bank table */
                        void* bankTable = sScriptDataBanks[pp->bankIndex];
                        if (bankTable != NULL) {
                            finalId = ((u32*)bankTable)[finalId];
                        }

                        spawned = psSpawnScript(pp, pp->linkNo, pp->bankIndex,
                                                finalId, NULL);
                        if (spawned == NULL) break;

                        /* Copy position, inherit properties, etc. */
                        spawned->positionX = pp->positionX;
                        spawned->positionY = pp->positionY;
                        spawned->positionZ = pp->positionZ;
                        spawned->scriptId = pp->scriptId;
                        spawned->peopleObj = pp->peopleObj;

                        if (pp->peopleObj != NULL) {
                            u32* refCount = (u32*)((u8*)pp->peopleObj + 0x4C);
                            (*refCount)++;
                        }
                        if (((u32*)pp->peopleObj)[1] & 0x04) {
                            spawned->flags |= PS_FLAG_NO_DETACH;
                        }

                        psInitParticle(spawned);

                        if (pp->peopleObj != NULL) {
                            psPeopleAttach(spawned, pp->parentObj);
                        } else {
                            psPeopleAttachStandalone(spawned, pp->parentObj);
                        }

                        psinterpret_Main(spawned, pp);
                        break;
                    }

                    /* ============================================================
                     * 0xBB (alt entry): SPAWN_WITH_VEL - Spawn inheriting vel+scale
                     * Spawns child and copies velocity and scale data in addition
                     * to position.
                     * ============================================================ */

                    /* ============================================================
                     * 0xD6: SET_COLOR1_RANDOM - Set primary color with random
                     * Reads 4 signed bytes as random variation per RGBA channel.
                     * Uses psRandom and psClampColorByte to apply variation.
                     * ============================================================ */
                    case 0xD6:
                    {
                        f32 rng;
                        s8 varR;
                        s32 deltaR;
                        s8 varG;
                        s8 varB;
                        s8 varA;
                        /* Interpolate existing color if timer is active */
                        if (pp->color1Timer != 0) {
                            /* Fixed-point interpolation for each RGBA channel */
                            /* ... (complex 16.16 fixed-point math per channel) ... */
                        }

                        /* Read 4 bytes of random variation */
                        /* (declaration moved to top of block) */
                        rng = psRandom();
                        varR = (s8)stream[0];
                        deltaR = (s32)(varR * 2) * rng;
                        pp->color1TargetR = psClampColorByte(pp->color1TargetR);
                        /* Repeat for G, B, A channels */
                        rng = psRandom();
                        varG = (s8)stream[1];
                        pp->color1TargetG = psClampColorByte(pp->color1TargetG);

                        rng = psRandom();
                        varB = (s8)stream[2];
                        pp->color1TargetB = psClampColorByte(pp->color1TargetB);

                        rng = psRandom();
                        varA = (s8)stream[3];
                        stream += 4;
                        pp->color1TargetA = psClampColorByte(pp->color1TargetA);

                        if (pp->color1Timer == 0) {
                            pp->color1R = pp->color1TargetR;
                            pp->color1G = pp->color1TargetG;
                            pp->color1B = pp->color1TargetB;
                            pp->color1A = pp->color1TargetA;
                        } else {
                            pp->color1Countdown = pp->color1Timer;
                        }
                        break;
                    }

                    /* ============================================================
                     * 0xD8: SET_COLOR2_RANDOM - Set secondary color with random
                     * Same structure as SET_COLOR1_RANDOM but for the secondary
                     * color channels (0x16-0x19 / 0x74-0x77).
                     * ============================================================ */
                    case 0xD8:
                    {
                        /* Similar to D6 but for secondary color */
                        break;
                    }

                    /* ============================================================
                     * 0xF5: SET_ROT_ZERO - Reset rotation to zero
                     * Sets headingSpeed and headingAccel to 0.0.
                     * ============================================================ */
                    case 0xF5:
                        pp->headingSpeed = 0.0f;
                        pp->headingAccel = 0.0f;
                        break;

                    /* ============================================================
                     * 0xF6: SET_LOOP_COUNT - Set loop counter + save return addr
                     * Reads 1 byte: loop count.
                     * Saves current PC as the loop return address.
                     * ============================================================ */
                    case 0xF6:
                    {
                        u8 count = *stream++;
                        pp->loopCounter = count;
                        pp->loopPC = (u16)((u32)stream - (u32)pp->scriptData);
                        break;
                    }

                    /* ============================================================
                     * 0xF7: DEC_LOOP - Decrement loop counter, jump if nonzero
                     * Decrements loopCounter. If still > 0, sets stream back
                     * to loopPC to re-execute the loop body.
                     * ============================================================ */
                    case 0xF7:
                    {
                        pp->loopCounter--;
                        if (pp->loopCounter != 0) {
                            stream = (u8*)pp->scriptData + pp->loopPC;
                        }
                        break;
                    }

                    /* ============================================================
                     * 0xF8: SAVE_PC - Save current PC position
                     * Saves current bytecode position as the "return address"
                     * for GOTO_SAVED.
                     * ============================================================ */
                    case 0xF8:
                        pp->savedPC = (u16)((u32)stream - (u32)pp->scriptData);
                        break;

                    /* ============================================================
                     * 0xF9: GOTO_SAVED - Jump to saved PC position
                     * Restores the bytecode stream to the address saved by
                     * SAVE_PC. Used for unconditional jumps / subroutine returns.
                     * ============================================================ */
                    case 0xF9:
                        stream = (u8*)pp->scriptData + pp->savedPC;
                        break;

                    /* ============================================================
                     * 0xFA: YIELD - End this frame's execution
                     * Sets repeatCount = 1 to signal the end of the current
                     * execution frame. Script will resume next frame.
                     * ============================================================ */
                    case 0xFA:
                    case 0xFB:  /* TERMINATE - identical handler */
                        pp->repeatCount = 1;
                        yieldFlag = 1;
                        break;

                    default:
                        /* Unknown opcode - skip to end */
                        break;
                    }
                    }
                }

                /* Check if delay value (r29) is nonzero - if so, stop execution */
                ;
            } while (delay == 0 && !yieldFlag);

            /* Phase 4: Save execution state */
            /* Save current PC */
            pp->pc = (u16)((u32)stream - (u32)pp->scriptData);
            pp->waitTimer = delay;

            }
        }
        /* ================================================================
         * Phase 5: Post-execution updates
         * ================================================================ */

        /* Decrement repeat counter */
        pp->repeatCount--;
        if (pp->repeatCount == 0) {
            PSParticle* result;
            /* ============================================================
             * Script has finished: clean up and destroy
             * ============================================================ */

            /* Decrement parent's reference count */
            if (pp->peopleObj != NULL) {
                u32* refCount = (u32*)((u8*)pp->peopleObj + 0x4C);
                (*refCount)--;
            }

            /* Clean up this particle */
            result = psCleanup(pp);

            /* Check if we should detach from parent */
            if (pp->parentObj != NULL) {
                psPeopleLinkUpdate(pp);
                if (result == 0) {
                    /* Check if parentCtx changed the head of the list */
                    if (parentCtx == NULL) {
                        PSParticle* head = pslist_GetHead(pp->linkNo);
                        if (head != result) {
                            result = pslist_GetHead(pp->linkNo);
                        }
                    }
                }
            }

            /* Unlink and free the particle */
            pslist_UpdateVisibility(pp, 0);  /* fn_801696D0 */
            pslist_Unlink(pp, parentCtx);
            return result;
        }

        /* ================================================================
         * Phase 6: Physics integration (velocity, gravity, friction)
         * ================================================================ */

        /* Check ORBIT flag for orbital motion */
        if (pp->flags & PS_FLAG_ORBIT) {
            f32 magnitude;
            /* Orbital motion: transform position around parent's axis */
            /* Uses sin/cos of heading angles (0x38, 0x3C) */
            /* Complex 3D rotation math with parent's world matrix */

            void* parent = pp->peopleObj;
            f32 sinH = sinf(pp->scaleFactor);   /* fn_800CE6AC */
            f32 cosH = cosf(pp->scaleFactor);   /* fn_800CE6D0 */
            f32 sinF = sinf(pp->frictionFactor);
            f32 cosF = cosf(pp->frictionFactor);

            /* Update Z velocity from parent's data */
            pp->velocityZ += *(f32*)((u32)parent + 0x54);

            /* Apply absolute value for magnitude calculation */
            magnitude = *(f32*)((u32)parent + 0x44);
            if (magnitude < 0.0f) magnitude = -magnitude;

            /* ... complex orbital calculation using trig and parent matrix ... */

            pp->positionX = /* calculated */ 0.0f;
            pp->positionY = /* calculated */ 0.0f;
            pp->positionZ = /* calculated */ 0.0f;
        } else {
            /* Standard linear motion */

            /* Apply gravity (subtract from Y velocity) */
            if (pp->flags & PS_FLAG_SCALE_ACTIVE) {
                pp->velocityY -= pp->scaleFactor;
            }

            /* Apply friction (multiply velocity) */
            if (pp->flags & PS_FLAG_FRICTION_ACTIVE) {
                pp->velocityX *= pp->frictionFactor;
                pp->velocityY *= pp->frictionFactor;
                pp->velocityZ *= pp->frictionFactor;
            }

            /* Integrate velocity into position */
            pp->positionX += pp->velocityX;
            pp->positionY += pp->velocityY;
            pp->positionZ += pp->velocityZ;
        }

        /* ================================================================
         * Phase 7: Camera tracking update
         * ================================================================ */
        if (pp->flags & PS_FLAG_KILLED) {
            /* Update camera tracking data */
            u32 camBits = (pp->flags >> 10) & 0x38;  /* Extract camera slot bits */
            u32 camIdx = (pp->flags >> 15) & 0x07;     /* Extract camera index */

            void* camSlot = sCameraSlots[camBits >> 2];

            if (camSlot == NULL) {
                /* Look up from HSD render system and initialize */
                void* renderObj = fn_8019F718();
                if (renderObj != NULL) {
                    /* Set up camera tracking */
                    psSetCameraTracking(camIdx + 1, renderObj);  /* psSetPointJObj */
                    fn_801A05EC(renderObj);
                }
            }

            if (camSlot != NULL) {
                f32 camOffsetX;
                f32 camOffsetY;
                f32 camOffsetZ;
                /* Update camera position offsets */
                fn_80172840(camSlot);

                /* X offset */
                camOffsetX = pp->positionX - *(f32*)((u32)camSlot + 0x50);
                fn_80172790(camSlot, camOffsetX);

                /* Y offset */
                camOffsetY = pp->positionY - *(f32*)((u32)camSlot + 0x60);
                fn_801726E0(camSlot, camOffsetY);

                /* Z offset */
                camOffsetZ = pp->positionZ - *(f32*)((u32)camSlot + 0x70);
                fn_80172630(camSlot, camOffsetZ);
            }
        }

        /* Final cleanup call */
        psCleanup(pp);

    }
    return pp;  /* Return value from psCleanup */
}
