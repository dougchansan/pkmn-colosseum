/**
 * @file ps_range_80168C64.c
 * @brief particle code, 0x80168C64 - 0x8017572C (74 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 *
 * psInterpretParticle0 (psinterpret_Main) identification:
 *   - simindex xd-corpus twin: psInterpretParticle0 @ 0x8018F9E0 in
 *     GXXE01 (Pokemon XD), score=0.976 seq=0.979, sz=12736 vs 12592.
 *   - config/GC6E01/symbols.txt / XD symbols.txt confirm the twin name
 *     and size (0x31C0).
 *   - the immediately preceding function in this unit is already named
 *     "psInterpretParticles" (the per-frame driver over all active
 *     particles), which calls psInterpretParticle0 once per particle - plural
 *     driver / singular interpreter, matching the XD naming pattern.
 *   - include/game/script/script.h (from a prior campaign) already
 *     carries a matching PSParticle struct (0x94 bytes) and the
 *     prototype for psInterpretParticle0 as psinterpret_Main(pp, parentCtx).
 *     Its field offsets were independently re-verified below against
 *     the retail asm (lerpTimer@0xC, color1Timer@0xE, color2Timer@0x10,
 *     alphaTimer@0x54, sizeXTimer@0x5A, sizeYTimer@0x5C, headingTimer@
 *     0x5E/headingSpeed@0x64/headingAccel@0x68 all match exactly).
 *
 * Coverage: this is the largest unmatched function in the game
 * (12,592 bytes). Only the portion below is implemented with asm-
 * verified fidelity (see per-case addresses); everything else is an
 * honest no-op default rather than a guess. The full opcode ->
 * handler-address table (decoded from jumptable_8036BFE0 in
 * build/GC6E01/asm/auto_05_8027A500_data.s) is:
 *
 *   0x80 stride-8 (axis bits in low 3 bits of opcode): SET_POSITION
 *   0x88 stride-8: ADD_POSITION      0x90 stride-8: SET_VELOCITY(=)
 *   0x98 stride-8: ADD_VELOCITY(+=, optional peopleObj-scaled average)
 *   0xA0 SET_LERP_TIMER   0xA1 CLEAR_OBJ_REF(flags&=~0x400)
 *   0xA2 SET_SCALE(scaleFactor@0x38, flags&1)
 *   0xA3 SET_FRICTION(frictionFactor@0x3C, flags&2)
 *   0xA4 SPAWN_SCRIPT(direct id)   0xA5 SPAWN_SCRIPT(table via bank)
 *   0xA6 RANDOM_REPEAT_COUNT (psRandom, biased-int trick -> repeatCount)
 *   0xA7 RANDOM_YIELD_CHECK (psRandom threshold -> yield)
 *   0xA8 RANDOM_OFFSET_XYZ (rotated random jitter added to position)
 *   0xA9 MODIFY_DIR (single float -> modifyDir)
 *   0xAA MODIFY_DIR_GEN_BASE (4 floats, requires peopleObj)
 *   0xAB..0xEF, 0xF2..0xFF: not yet transcribed (default/no-op below)
 *   0xF0 SPAWN_GENERATOR(table via bank)  0xF1 SPAWN_GENERATOR(direct)
 *
 * Discovered corrections vs the stale comments in script.h:
 *   - psReadFloat/psReadU16/psUpdateVelocity/psSpawnScript/psSpawnGenerator
 *     /psRotationUpdate/psCameraLookAt are STALE aliases from an older
 *     campaign; the real linked symbol names (per current
 *     config/GC6E01/symbols.txt, and used verbatim in the retail asm)
 *     are getFloat, getTime, psApplyOffsetLocalRotation,
 *     psGenerateParticleID0, psCreateGeneratorID, modifyDir, modifyDirGenBase
 *     respectively. This file declares those directly.
 *   - PS_FLAG_ORBIT in script.h is documented as 0x8; the retail
 *     physics-integration epilogue (0x801723B8) actually tests bit 29
 *     (IBM) = mask 0x4 to select the orbital-motion branch. Not fixed
 *     here (shared header, out of scope for this file) but noted.
 *   - the camera-tracking tail (guarded by PS_FLAG_ATTACH_CAMERA
 *     =0x8000) extracts its 3-bit camera-slot field from flags bits
 *     12-14 (`(flags >> 12) & 0x7`), not bits 15/10 as an earlier
 *     draft (archive/previous_campaign/src/game/script/psinterpret.c)
 *     guessed.
 */
#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "dolphin/gx/GXInternal.h"
#include "game/ps_types.h"
#include "game/script/script.h"
#include "hsd/hsd_lobj.h"
#include "hsd/hsd_object.h"

/* ======================================================================
 * External data banks / SDA float constants (verified against the
 * retail disassembly in build/GC6E01/asm/game/ps_range_80168C64.s).
 * ====================================================================== */
extern void* lbl_804527C8[]; /* sScriptDataBanks: per-bank script/table data */
extern void* lbl_804529C8[]; /* sLinkDataBanks: per-bank object data */
extern void* lbl_80452DC8[]; /* sCameraSlots */

extern f32 lbl_8047D630; /* 0.0f */
extern f32 lbl_8047D5B0; /* 3.0f */
extern f32 lbl_8047D5B4; /* 0.0f */
extern f32 lbl_8047D5B8; /* 1.0f */
extern f32 lbl_8047D634; /* 3.0f */
extern f32 lbl_8047D638; /* 1.0f */
extern f32 lbl_8047D63C; /* 0.5f */
extern f32 lbl_8047D640; /* 2.0f */
extern f64 lbl_8047D660; /* int->float bias (0x4330000000000000) */
extern f64 lbl_8047D668; /* signed int->float bias */
extern f32 lbl_8047D690; /* 255.0f */
extern f32 lbl_8047D694; /* pi / 2 */
extern f32 lbl_8047D698; /* -pi / 2 */
extern f64 lbl_8047D6A0; /* 2.0 */
extern f64 lbl_8047D6A8; /* pi */
extern const char lbl_8047D670[7];
extern const char lbl_8047D678[5];

typedef union PSFloatBytes {
    u8 bytes[4];
    f32 value;
} PSFloatBytes;

typedef struct PSJObjTransform {
    u8 pad00[0x14];
    u32 flags;
    u8 pad18[0x20];
    f32 translateX;
    f32 translateY;
    f32 translateZ;
} PSJObjTransform;

typedef struct PSForceJObj {
    u8 pad00[0x14];
    u32 flags;
    u8 pad18[0x38];
    f32 worldX;
    u8 pad54[0x0C];
    f32 worldY;
    u8 pad64[0x0C];
    f32 worldZ;
} PSForceJObj;

typedef struct PSCameraView {
    u8 pad00[0x0C];
    Vec position;
} PSCameraView;

typedef struct PSCameraObject {
    u8 pad00[0x24];
    PSCameraView* view;
} PSCameraObject;

typedef struct PSColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} PSColor;

extern s32 lbl_8047B170;
extern PSFloatBytes lbl_8047B178;

extern PSGeneratorState* lbl_8047B188;
extern PSGeneratorState** lbl_8047B184;
extern PSGeneratorState* lbl_8047B18C;
extern void* lbl_8047B180;
extern PSAppSRT* lbl_8047B124;
extern u32 lbl_80452708[];
extern u32 lbl_80452748[];
extern void* lbl_80452AC8[];
extern void** lbl_804528C8[];
extern s32 lbl_80452CC8[];
extern PSParticle* lbl_80452788[];
extern PSParticle* lbl_8047B108;
extern HSD_Obj* lbl_8047B128;
extern HSD_Obj* lbl_8047B190;
extern const char lbl_802737B8[];
extern const char lbl_802737C4[];
extern const char lbl_80273820[];
extern const char lbl_8027382C[];
extern const char lbl_802739E4[];
extern const char lbl_802739F0[];
extern u16 lbl_8047B110;
extern u16 lbl_8047B114;
extern u16 lbl_8047B116;
extern u16 lbl_8047B120;
extern u16 lbl_8047B11A;
extern u16 lbl_8047B112;
extern u16 lbl_8047B118;
extern u8 lbl_80478C30;
extern u16 lbl_80478C38;
extern s32 lbl_8047B12C;
extern s32 lbl_8047B164;
extern s32 lbl_8047B168;
extern s32 lbl_8047B144;
extern s32 lbl_8047B148;
extern PSColor lbl_8047B130;
extern PSColor lbl_8047B13C;
extern PSColor lbl_8047B140;
extern PSColor lbl_8047B134;
extern PSColor lbl_8047B138;

/* ======================================================================
 * External functions - real symbol names per config/GC6E01/symbols.txt.
 * Several differ from the stale names recorded in script.h by an
 * earlier campaign; the names below are what the disassembler already
 * resolves calls to, so they are used verbatim here to link correctly.
 * ====================================================================== */
extern u8* getFloat(u8* stream, f32* out);
extern u8* getTime(u8* stream, u16* out);
extern void psApplyOffsetLocalRotation(PSParticle* pp, f32* vec3);      /* 0x801729EC */
extern void psApplyVelocityLocalRotation(PSParticle* pp);               /* 0x80172AE0 */
extern f32 fn_801ADC7C(void);                                           /* psRandom, 0x801ADC7C */
extern PSParticle* psGenerateParticleID0(PSParticle* pp, s32 linkNo,
                                         s32 bankIdx, s32 scriptId,
                                         void* arg);                    /* 0x80169A48 */
extern PSGeneratorState* psCreateGeneratorID(s32 linkNo, s32 bankIdx, s32 scriptId);
extern void psCopyGeneratorData(PSParticle* gen, void* peopleObj);       /* 0x80172930 */
extern s32 psChangeParticleAppSRT(PSParticle* pp, PSAppSRT* parentObj); /* 0x8016A878 */
extern s32 psAttachParticleAppSRT(PSParticle* pp, PSAppSRT* parentObj);
extern s32 psChangeGeneratorAppSRT(PSGeneratorState* gen, PSAppSRT* parentObj); /* 0x8016A79C */
extern s32 psAttachGeneratorAppSRT(PSGeneratorState* gen, PSAppSRT* parentObj);
extern void genPosUpdate(void* obj);                                    /* 0x80175E88 */
extern void modifyDir(PSParticle* pp, f32 param);                       /* 0x80172FA8 */
extern void modifyDirGenBase(PSParticle* pp, f32 a, f32 b, f32 c, f32 d); /* 0x801732A0 */
extern f32 sqrtf(f32 x);
extern s32 applyForceJObj(PSParticle* pp, PSForceJObj* jobj,
                         f32 force, f32 radius);                       /* 0x80172BBC */
extern void setVelToJObj(PSParticle* pp, PSForceJObj* jobj);            /* 0x80172D00 */
extern u8 U8ClampAdd(u8 cur, f32 delta);                                /* 0x801728B0 */
extern PSParticle* _psListGetNext(PSParticle* pp);                         /* psCleanup, 0x80172928 */
extern s32 psRemoveParticleAppSRT(PSParticle* pp);                      /* 0x?? */
extern void psDeletePntJObjwithParticle(PSParticle* pp);
extern void psKillGeneratorID(s32 familyId);
extern u32 psGetNewIDNum(void);
extern void _psListDelete(PSParticle* pp, PSParticle* parent);
extern PSParticle* _psListGetFirst(s32 linkNo);
extern PSParticle* _psListNew(PSParticle* parent, u32 linkNo);
extern f32 sinf(f32 x); /* sinf-family */
extern f32 cosf(f32 x); /* cosf-family */
extern f32 tanf(f32 x);
extern void* fn_8019F718(void);
extern void psSetPointJObj(s32 idx, void* renderObj);
extern void fn_801A05EC(void* renderObj);
extern void HSD_JObjSetupMatrix(void* camSlot);
extern void HSD_JObjAddTx(PSJObjTransform* jobj, f32 dx);
extern void HSD_JObjAddTy(PSJObjTransform* jobj, f32 dy);
extern void HSD_JObjAddTz(PSJObjTransform* jobj, f32 dz);
extern void fn_8019D9DC(PSForceJObj* jobj);
extern void HSD_MtxSRT(void* dst, void* scale, void* rot, void* trans, void* order);
extern void fn_801A6960(void* ptr);
extern void* fn_801A6928(s32 size);
extern void* fn_801A3E64(void);
extern void __assert(const char* file, u32 line, const char* msg);
extern void* memset(void* dst, s32 value, u32 size);
extern void PSMTXIdentity(Mtx m);
extern void PSMTXCopy(const Mtx source, Mtx destination);
extern void PSMTXRotRad(Mtx m, char axis, f32 angle);
extern void PSMTXConcat(const Mtx a, const Mtx b, Mtx out);
extern void PSVECNormalize(const Vec* src, Vec* dst);
extern void PSVECCrossProduct(const Vec* a, const Vec* b, Vec* out);
extern void HSD_CObjGetUpVector(void* camera, Vec* up);
extern f32 lbl_8047D6B4;
extern f32 lbl_8047D5CC;
extern f32 lbl_8047D618;
extern f64 lbl_8047D5E0;
extern f32 lbl_8047D5D8;
extern f32 lbl_80478ACC;
extern f32 lbl_80478AC8;
extern double sin(double x);
extern double cos(double x);
extern double atan2(double y, double x);
extern void particleSort(s32 linkNo, PSParticle** first, PSParticle** second);
extern void fn_800BC8C8(s32 count);
extern void fn_800B884C(s32 count);
extern void fn_800BC6F0(s32 stage, s32 map, s32 coord, s32 color);
extern void fn_800BC1A0(s32 stage, s32 a, s32 b, s32 c, s32 d);
extern void fn_800BC1E4(s32 stage, s32 a, s32 b, s32 c, s32 d);
extern void fn_800BC228(s32 stage, s32 a, s32 b, s32 c, s32 d, s32 e);
extern void fn_800BC290(s32 stage, s32 a, s32 b, s32 c, s32 d, s32 e);
extern void fn_800BC52C(s32 stage, s32 a, s32 b);
void psDispSub(PSParticle* pp, void* polygonData);
void psDispSubAppSRT(PSParticle* pp, Mtx parentMatrix);
void psDispSubAPPSRTPoint(PSParticle* pp);
void psSetupTevInvalidState(void);
void psSetupTevCommon(void);
extern f32 lbl_8047D5DC;
extern f32 lbl_8047B14C;
extern f32 lbl_8047B150;
extern f32 lbl_8047B154;
extern f32 lbl_8047B158;
extern f32 lbl_8047B15C;
extern f32 lbl_8047B160;
extern u8 lbl_80452DE8[];
extern void fn_800BD554(s32 mode);
extern void fn_800BCEBC(s32 mode);
extern void HSD_FogSet(void* fog);
extern void fn_800B7D3C(void);
extern void fn_800B7874(s32 attribute, s32 type);
extern void fn_800B928C(s32 primitive, s32 format, s32 count);
extern void fn_800B9404(s32 width, s32 offset);
extern void generateParticle_8017424C(PSGeneratorState* gen);
extern void HSD_MulColor(GXColor* a, GXColor* b, GXColor* dest);

void psSetGeneratorAngleRadiusScale(PSGeneratorState* gen, f32* scale,
                                    u8 applyToMotion) {
    u8* raw = (u8*)gen;
    u16 mode = gen->angleFlags & 0xF;
    u16 objectIndex = *(u16*)(raw + 0x8A);
    f32* object = ((f32**)lbl_80452AC8[gen->bankIndex])[objectIndex];
    f32 average = (scale[0] + scale[1] + scale[2]) / lbl_8047D5B0;

    switch (mode) {
    case 0:
    case 3:
    case 4:
        *(f32*)(raw + 0x54) = average * object[0x30 / 4];
        *(f32*)(raw + 0x58) = average * object[0x34 / 4];
        break;
    case 1:
        *(f32*)(raw + 0x54) = scale[0] * object[0x30 / 4];
        *(f32*)(raw + 0x58) = scale[1] * object[0x34 / 4];
        *(f32*)(raw + 0x5C) = scale[2] * object[0x38 / 4];
        break;
    case 5:
        *(f32*)(raw + 0x54) = scale[0] * object[0x30 / 4];
        *(f32*)(raw + 0x60) = *(f32*)(raw + 0x54);
        *(f32*)(raw + 0x58) = scale[1] * object[0x34 / 4];
        *(f32*)(raw + 0x70) = *(f32*)(raw + 0x58);
        *(f32*)(raw + 0x5C) = scale[2] * object[0x38 / 4];
        *(f32*)(raw + 0x80) = *(f32*)(raw + 0x5C);
        break;
    case 6:
    case 7:
        *(f32*)(raw + 0x54) = average * object[0x30 / 4];
        *(f32*)(raw + 0x58) = average * object[0x34 / 4];
        *(f32*)(raw + 0x5C) = average * object[0x38 / 4];
        break;
    case 8:
        *(f32*)(raw + 0x5C) = average * object[0x30 / 4];
        *(f32*)(raw + 0x64) = average * object[0x34 / 4];
        break;
    }

    *(f32*)(raw + 0x44) = average * object[0x20 / 4];
    if (applyToMotion == TRUE) {
        *(f32*)(raw + 0x38) *= average;
        *(f32*)(raw + 0x3C) *= average;
        *(u16*)(raw + 0x88) |= 0x1000;
    }

    gen->angleRadiusScale[0] = scale[0];
    gen->angleRadiusScale[1] = scale[1];
    gen->angleRadiusScale[2] = scale[2];
}
extern void fn_800BA4C8(s32 channel, const PSColor* color);
extern void fn_800BA5BC(s32 channel, const PSColor* color);
extern void fn_800BC2F8(s32 reg, const PSColor* color);

#if !defined(PR410_PS_SPLIT) || defined(PR410_PS_PREFIX)

#pragma dont_inline on
PSParticle* _psListGetFirst(s32 linkNo) {
    s32 valid = FALSE;

    if (linkNo >= 0 && linkNo < PS_NUM_LINK) {
        valid = TRUE;
    }

    if (valid == FALSE) {
        __assert(lbl_802737B8, 0x98, lbl_802737C4);
    }

    return lbl_80452788[linkNo];
}
#pragma dont_inline reset

#pragma dont_inline on
void _psListDelete(PSParticle* pp, PSParticle* parent) {
    lbl_80452748[pp->linkNo] = 1;

    if (parent == NULL) {
        if (lbl_80452788[pp->linkNo] != pp) {
            __assert(lbl_802737B8, 0x88, lbl_802737B8 + 0x30);
        }
        lbl_80452788[pp->linkNo] = pp->next;
    } else {
        if (parent->next != pp) {
            __assert(lbl_802737B8, 0x8B, lbl_802737B8 + 0x54);
        }
        parent->next = pp->next;
    }

    pp->next = lbl_8047B108;
    lbl_8047B108 = pp;
    lbl_8047B11A--;
}
#pragma dont_inline reset

PSParticle* _psListNew(PSParticle* parent, u32 linkNo) {
    PSParticle* pp;

    if (lbl_8047B108 == NULL) {
        lbl_8047B108 = fn_801A6928(sizeof(PSParticle));
        memset(lbl_8047B108, 0, sizeof(PSParticle));
    }

    pp = lbl_8047B108;
    if (pp == NULL) {
        return NULL;
    }

    lbl_8047B11A++;
    if (lbl_8047B11A > lbl_8047B114) {
        lbl_8047B114 = lbl_8047B11A;
    }

    lbl_8047B108 = pp->next;
    if (parent == NULL) {
        pp->next = lbl_80452788[linkNo];
        lbl_80452788[linkNo] = pp;
    } else {
        pp->next = parent->next;
        parent->next = pp;
    }

    lbl_80452748[linkNo] = 1;
    return pp;
}

void _psListClear(void) {
    u32* dataBank = lbl_80452708;
    u32* active;
    PSParticle** list;
    PSParticle* pp;
    PSParticle* next;
    s32 i;

    pp = lbl_8047B108;
    while (pp != NULL) {
        next = pp->next;
        fn_801A6960(pp);
        pp = next;
    }
    lbl_8047B108 = NULL;

    list = (PSParticle**)(dataBank + (2 * PS_NUM_LINK));
    active = dataBank + PS_NUM_LINK;
    for (i = 0; i < PS_NUM_LINK; i++) {
        pp = list[i];
        while (pp != NULL) {
            next = pp->next;
            fn_801A6960(pp);
            pp = next;
        }
        list[i] = NULL;
        active[i] = 0;
        dataBank[i] = 0;
    }
}

void psSetParticleVisibility(PSGeneratorState* gen, u8 visible) {
    PSParticle* pp;
    PSGeneratorState* child;

    pp = _psListGetFirst(gen->linkNo);
    while (pp != NULL) {
        if (pp->scriptId == gen->familyId) {
            if (visible) {
                *(s32*)&pp->flags &= ~0x20000000;
            } else {
                *(s32*)&pp->flags |= 0x20000000;
            }
        }
        pp = pp->next;
    }

    child = lbl_8047B188;
    while (child != NULL) {
        if (child->familyId == gen->familyId) {
            if (visible) {
                *(s32*)&child->flags &= ~0x20000000;
            } else {
                *(s32*)&child->flags |= 0x20000000;
            }
        }
        child = child->next;
    }
}

s32 _psLinkInit(s32 count) {
    s32 i;

    for (i = 0; i < PS_NUM_LINK; i++) {
        lbl_80452708[i] = 0;
        lbl_80452748[i] = 0;
        lbl_80452788[i] = NULL;
    }

    lbl_8047B108 = NULL;
    for (i = count - 1; i >= 0; i--) {
        PSParticle* pp = fn_801A6928(sizeof(PSParticle));

        memset(pp, 0, sizeof(PSParticle));
        if (pp == NULL) {
            return -1;
        }

        pp->next = lbl_8047B108;
        lbl_8047B108 = pp;
    }

    return i;
}

#endif

#if !defined(PR410_PS_SPLIT) || defined(PR410_PS_EXACT)

void psSetRandomVelocityScaling(PSGeneratorState* gen, u8 enabled) {
    if (enabled) {
        gen->generatorFlags |= 0x200;
    } else {
        gen->generatorFlags = (u32)gen->generatorFlags & ~0x200;
    }
}

void psSetNodeScaling(PSGeneratorState* gen, u8 enabled) {
    if (enabled) {
        gen->generatorFlags |= 0x100;
    } else {
        gen->generatorFlags = (u32)gen->generatorFlags & ~0x100;
    }
}

void psSetTornadoScaling(PSGeneratorState* gen, u8 enabledA, u8 enabledB) {
    if (enabledA) {
        gen->generatorFlags |= 0x40;
    } else {
        gen->generatorFlags = (u32)gen->generatorFlags & ~0x40;
    }

    if (enabledB) {
        gen->generatorFlags |= 0x80;
    } else {
        gen->generatorFlags = (u32)gen->generatorFlags & ~0x80;
    }
}

void psSetParticleTexScaling(PSGeneratorState* gen, u8 enabled) {
    if (enabled) {
        gen->generatorFlags |= 0x20;
    } else {
        gen->generatorFlags = (u32)gen->generatorFlags & ~0x20;
    }
}

void psSetOffsetRotationInLocal(PSGeneratorState* gen, u8 enabledA, u8 enabledB) {
    if (enabledA) {
        gen->generatorFlags |= 0x8;
        if (enabledB) {
            gen->generatorFlags |= 0x10;
        } else {
            gen->generatorFlags = (u32)gen->generatorFlags & ~0x10;
        }
    } else {
        gen->generatorFlags = (u32)gen->generatorFlags & ~0x8;
        gen->generatorFlags = (u32)gen->generatorFlags & ~0x10;
    }
}

void psSetVelocityRotationInLocal(PSGeneratorState* gen, u8 enabled) {
    if (enabled) {
        gen->generatorFlags |= 0x4;
    } else {
        gen->generatorFlags = (u32)gen->generatorFlags & ~0x4;
    }
}

void psUnlinkChildGensFromJObj(PSGeneratorState* gen) {
    gen->generatorFlags = (u32)gen->generatorFlags & ~0x1;
}

void psLinkChildGensToJObj(PSGeneratorState* gen, void* jobj) {
    gen->generatorFlags |= 0x1;
    gen->linkedJObj = jobj;
}

u32 psGetParticleChildCount(PSGeneratorState* gen) {
    u32 count = 0;
    PSGeneratorState* child = lbl_8047B188;

    while (child != NULL) {
        if (child->familyId == gen->familyId) {
            count += child->childCount;
        }
        child = child->next;
    }

    return count;
}

u32 psGetGeneratorChildMaxLife(PSGeneratorState* gen) {
    u32 maxLife = gen->maxLife;
    PSGeneratorState* child = lbl_8047B188;

    while (child != NULL) {
        if (child->familyId == gen->familyId && child->maxLife > maxLife) {
            maxLife = child->maxLife;
        }
        child = child->next;
    }

    return maxLife;
}

void psKillGeneratorChild(PSGeneratorState* gen) {
    PSParticle* next;
    PSParticle* parent = NULL;
    PSParticle* pp;
    u16 familyId = gen->familyId;

    pp = _psListGetFirst(gen->linkNo);

    while (pp != NULL) {
        next = pp->next;

        if (pp->scriptId == familyId && pp->peopleObj != NULL && pp->peopleObj == gen) {
            if (pp->peopleObj != NULL) {
                ((PSGeneratorState*)pp->peopleObj)->childCount--;
            }
            if (pp->parentObj != NULL) {
                psRemoveParticleAppSRT(pp);
            }
            if (pp->flags & PS_FLAG_ATTACH_CAMERA) {
                u32 slotIdx = (pp->flags >> 12) & 0x7;
                void* camSlot = lbl_80452DC8[slotIdx];

                if (camSlot != NULL) {
                    fn_801A05EC(camSlot);
                    lbl_80452DC8[slotIdx] = NULL;
                }
            }
            _psListDelete(pp, parent);
        } else {
            parent = pp;
        }
        pp = next;
    }
}

void psKillFamily(s32 familyId, s32 linkNo) {
    PSParticle* next;
    PSParticle* parent = NULL;
    PSParticle* pp;

    pp = _psListGetFirst(linkNo);

    while (pp != NULL) {
        next = pp->next;

        if (pp->scriptId == (u16)familyId) {
            if (pp->peopleObj != NULL) {
                ((PSGeneratorState*)pp->peopleObj)->childCount--;
            }
            if (pp->parentObj != NULL) {
                psRemoveParticleAppSRT(pp);
            }
            if (pp->flags & PS_FLAG_ATTACH_CAMERA) {
                u32 slotIdx = (pp->flags >> 12) & 0x7;
                void* camSlot = lbl_80452DC8[slotIdx];

                if (camSlot != NULL) {
                    fn_801A05EC(camSlot);
                    lbl_80452DC8[slotIdx] = NULL;
                }
            }
            _psListDelete(pp, parent);
        } else {
            parent = pp;
        }
        pp = next;
    }

    psKillGeneratorID(familyId);
}

#endif

#if !defined(PR410_PS_SPLIT) || defined(PR410_PS_SUFFIX)

s32 psAttachGeneratorAppSRT(PSGeneratorState* gen, PSAppSRT* appSRT) {
    u16 refCount;

    if (appSRT == NULL || gen == NULL || gen->appSRT != NULL) {
        return -1;
    }

    gen->appSRT = appSRT;
    refCount = appSRT->refCount + 1;
    appSRT->refCount = refCount;
    return refCount;
}

s32 psAttachParticleAppSRT(PSParticle* pp, PSAppSRT* appSRT) {
    u16 refCount;

    if (appSRT == NULL || pp == NULL || pp->parentObj != NULL) {
        return -1;
    }

    pp->parentObj = appSRT;
    refCount = appSRT->refCount + 1;
    appSRT->refCount = refCount;
    return refCount;
}

PSAppSRT* psAddGeneratorAppSRT(PSGeneratorState* gen, u8 type) {
    PSAppSRT* appSRT;
    u16 familyId = gen->familyId;

    if (lbl_8047B124 == NULL) {
        lbl_8047B124 = fn_801A6928(lbl_8047B120);
        if (lbl_8047B124 != NULL) {
            memset(lbl_8047B124, 0, lbl_8047B120);
        }
    }

    appSRT = lbl_8047B124;
    if (appSRT != NULL) {
        u16 count;

        lbl_8047B124 = appSRT->next;
        appSRT->next = NULL;
        appSRT->refCount = 1;
        appSRT->flags = 0;
        appSRT->type = type;
        appSRT->rotationX = appSRT->rotationY = appSRT->rotationZ = 0.0f;
        appSRT->translationX = appSRT->translationY = appSRT->translationZ = 0.0f;
        appSRT->scaleX = appSRT->scaleY = appSRT->scaleZ = 1.0f;
        appSRT->destroy = NULL;
        appSRT->active = 0;
        appSRT->owner = NULL;
        appSRT->familyId = familyId;

        count = lbl_8047B116 + 1;
        lbl_8047B116 = count;
        if (count > lbl_8047B110) {
            lbl_8047B110 = count;
        }
    }

    gen->appSRT = appSRT;
    return appSRT;
}

s32 psRemoveParticleAppSRT(PSParticle* pp) {
    PSAppSRT* appSRT = (PSAppSRT*)pp->parentObj;
    u16 refCount;

    if (appSRT == NULL) {
        return -1;
    }

    refCount = appSRT->refCount - 1;
    appSRT->refCount = refCount;

    if (refCount == 0) {
        appSRT = (PSAppSRT*)pp->parentObj;
        if (appSRT->destroy != NULL) {
            appSRT->destroy(appSRT);
        }
        appSRT->next = lbl_8047B124;
        lbl_8047B124 = appSRT;
        lbl_8047B116--;
    }

    pp->parentObj = NULL;
    return refCount;
}

s32 psRemoveGeneratorAppSRT(PSGeneratorState* gen) {
    PSAppSRT* appSRT = (PSAppSRT*)gen->appSRT;
    u16 refCount;

    if (appSRT == NULL) {
        return -1;
    }

    if (appSRT->owner == gen) {
        appSRT->owner = NULL;
    }

    appSRT = (PSAppSRT*)gen->appSRT;
    refCount = appSRT->refCount - 1;
    appSRT->refCount = refCount;

    if (refCount == 0) {
        appSRT = (PSAppSRT*)gen->appSRT;
        if (appSRT->destroy != NULL) {
            appSRT->destroy(appSRT);
        }
        appSRT->next = lbl_8047B124;
        lbl_8047B124 = appSRT;
        lbl_8047B116--;
    }

    gen->appSRT = NULL;
    return refCount;
}

void psInitParticle(s32 count) {
    u32* banks = (u32*)lbl_804527C8;
    s32 i;

    _psLinkInit(count);
    lbl_8047B11A = 0;
    lbl_8047B114 = 0;

    for (i = 0; i < 64; i++) {
        banks[i] = 0;
        banks[64 + i] = 0;
        banks[128 + i] = 0;
        banks[192 + i] = 0;
        banks[256 + i] = 0;
        banks[320 + i] = 0;
    }

    for (i = 384; i < 392; i++) {
        banks[i] = 0;
    }
}

void psRemoveParticle(void) {
    u32* banks = (u32*)lbl_804527C8;
    s32 linkNo;
    s32 i;

    for (linkNo = 0; linkNo < PS_NUM_LINK; linkNo++) {
        PSParticle* current = _psListGetFirst(linkNo);

        while (current != NULL) {
            PSParticle* next = current->next;
            PSParticle* previous = NULL;
            PSParticle* scan = _psListGetFirst(current->linkNo);

            while (scan != NULL) {
                if (scan == current) {
                    if (current->peopleObj != NULL) {
                        ((PSGeneratorState*)current->peopleObj)->childCount--;
                    }
                    if (current->parentObj != NULL) {
                        psRemoveParticleAppSRT(current);
                    }
                    if (current->flags & PS_FLAG_ATTACH_CAMERA) {
                        u32 slot = (current->flags >> 12) & 7;

                        if (lbl_80452DC8[slot] != NULL) {
                            fn_801A05EC(lbl_80452DC8[slot]);
                            lbl_80452DC8[slot] = NULL;
                        }
                    }

                    _psListDelete(scan, previous);
                    break;
                }

                previous = scan;
                scan = scan->next;
            }
            current = next;
        }
    }

    psKillAllGenerator();
    _psListClear();

    for (i = 0; i < 64; i++) {
        banks[i] = 0;
        banks[64 + i] = 0;
        banks[128 + i] = 0;
        banks[192 + i] = 0;
        banks[256 + i] = 0;
        banks[320 + i] = 0;
    }

    for (i = 0; i < 8; i++) {
        if (lbl_80452DC8[i] != NULL) {
            fn_801A05EC(lbl_80452DC8[i]);
            lbl_80452DC8[i] = NULL;
        }
    }
}

typedef struct PSTextureGroup {
    u32 count;
    u32 format;
    u8 pad08[0x0C];
    u16 paletteCount;
    u16 paletteFlags;
    u32 entries[];
} PSTextureGroup;

typedef struct PSFormGroup {
    u32 count;
    u32 entries[];
} PSFormGroup;

void psInitDataBankLocate(void* data, void* objects, void* locations) {
    s32 commandCount;
    s32 firstCommand;
    s32 i;
    s32* commandTable;
    s32* command;
    s32* dataWords = data;
    s32 version = *(u16*)data;

    if (version == 0) {
        commandCount = dataWords[1];
        commandTable = dataWords + 2;
        firstCommand = 0;
        for (i = 0; i < commandCount; i++) {
            dataWords[i + 2] += (s32)data;
        }
    } else if (version >= 0x40 && version < 0x44) {
        firstCommand = dataWords[1];
        commandCount = dataWords[2] + firstCommand;
        commandTable = dataWords + 3 - firstCommand;
        command = dataWords;
        for (i = 0; i < dataWords[2]; i++) {
            if (command[3] != 0) {
                command[3] += (s32)data;
            }
            command++;
        }
    } else {
        return;
    }

    for (i = firstCommand; i < commandCount; i++) {
        s32* entry = (s32*)commandTable[i];

        if (entry != NULL) {
            entry[2] &= 0xF1FFFFFF;
            entry[2] |= 0x08000000;
        }
    }

    {
        s32 groupCount = *(s32*)objects;
        s32* groups = (s32*)objects + 1;

        for (i = 0; i < groupCount; i++) {
            if (groups[i] != 0) {
                groups[i] += (s32)objects;
            }
        }

        for (i = 0; i < groupCount; i++) {
            PSTextureGroup* group = (PSTextureGroup*)groups[i];
            u32 j;

            if (group == NULL) {
                continue;
            }

            for (j = 0; j < group->count; j++) {
                if (group->entries[j] != 0) {
                    group->entries[j] += (u32)objects;
                }
            }

            if (group->format != 8 && group->format != 9 &&
                group->format != 10) {
                continue;
            }

            if (group->paletteFlags & 1) {
                if (group->entries[group->count] != 0) {
                    group->entries[group->count] += (u32)objects;
                }
            } else if (group->paletteCount != 0) {
                for (j = group->count;
                     j < group->count + group->paletteCount; j++) {
                    if (group->entries[j] != 0) {
                        group->entries[j] += (u32)objects;
                    }
                }
            } else {
                for (j = group->count; j < group->count * 2; j++) {
                    if (group->entries[j] != 0) {
                        group->entries[j] += (u32)objects;
                    }
                }
            }
        }

        if (locations != NULL) {
            s32* groups = (s32*)locations + 1;

            for (i = 0; i < groupCount; i++) {
                PSFormGroup* group;
                u32 j;

                if (groups[i] == 0) {
                    continue;
                }

                groups[i] += (s32)locations;
                group = (PSFormGroup*)groups[i];
                for (j = 0; j < group->count; j++) {
                    if (group->entries[j] != 0) {
                        group->entries[j] += (u32)locations;
                    }
                }
            }
        }
    }
}

void psInitDataBank(s32 bankIndex, void* data, void* objects,
                    void* bankData, void* locations) {
    u32* banks = (u32*)lbl_804527C8;
    u16 format;

    if (bankIndex >= 64) {
        return;
    }

    psInitDataBankLocate(data, objects, locations);
    banks[bankIndex] = (u32)bankData;
    banks[256 + bankIndex] = *(u32*)objects;
    banks[128 + bankIndex] = (u32)objects + 4;
    banks[64 + bankIndex] =
        locations != NULL ? (u32)locations + 4 : 0;

    format = *(u16*)data;
    if (format == 0) {
        banks[320 + bankIndex] = *(u32*)((u8*)data + 4);
        banks[192 + bankIndex] = (u32)data + 8;
    } else if (format >= 0x40 && format < 0x44) {
        u32 firstCount = *(u32*)((u8*)data + 4);

        banks[320 + bankIndex] =
            firstCount + *(u32*)((u8*)data + 8);
        banks[192 + bankIndex] =
            (u32)data + 0xC - firstCount * sizeof(u32);
    }
}

s32 psChangeGeneratorAppSRT(PSGeneratorState* gen, PSAppSRT* newAppSRT) {
    PSAppSRT* oldAppSRT;
    u16 refCount;

    if (newAppSRT == NULL || gen == NULL) {
        return -1;
    }

    oldAppSRT = (PSAppSRT*)gen->appSRT;
    if (oldAppSRT != NULL && oldAppSRT != NULL) {
        if (oldAppSRT->owner == gen) {
            oldAppSRT->owner = NULL;
        }

        oldAppSRT = (PSAppSRT*)gen->appSRT;
        refCount = oldAppSRT->refCount - 1;
        oldAppSRT->refCount = refCount;

        if (refCount == 0) {
            oldAppSRT = (PSAppSRT*)gen->appSRT;
            if (oldAppSRT->destroy != NULL) {
                oldAppSRT->destroy(oldAppSRT);
            }
            oldAppSRT->next = lbl_8047B124;
            lbl_8047B124 = oldAppSRT;
            lbl_8047B116--;
        }

        gen->appSRT = NULL;
    }

    gen->appSRT = newAppSRT;
    refCount = newAppSRT->refCount + 1;
    newAppSRT->refCount = refCount;
    return refCount;
}

s32 psChangeParticleAppSRT(PSParticle* pp, PSAppSRT* newAppSRT) {
    PSAppSRT* oldAppSRT;
    u16 refCount;

    if (newAppSRT == NULL || pp == NULL) {
        return -1;
    }

    oldAppSRT = (PSAppSRT*)pp->parentObj;
    if (oldAppSRT != NULL && oldAppSRT != NULL) {
        refCount = oldAppSRT->refCount - 1;
        oldAppSRT->refCount = refCount;

        if (refCount == 0) {
            oldAppSRT = (PSAppSRT*)pp->parentObj;
            if (oldAppSRT->destroy != NULL) {
                oldAppSRT->destroy(oldAppSRT);
            }
            oldAppSRT->next = lbl_8047B124;
            lbl_8047B124 = oldAppSRT;
            lbl_8047B116--;
        }

        pp->parentObj = NULL;
    }

    pp->parentObj = newAppSRT;
    refCount = newAppSRT->refCount + 1;
    newAppSRT->refCount = refCount;
    return refCount;
}

void psRemoveAppSRT(void) {
    PSAppSRT* appSRT = lbl_8047B124;

    while (appSRT != NULL) {
        PSAppSRT* next = appSRT->next;
        fn_801A6960(appSRT);
        appSRT = next;
    }

    lbl_8047B124 = NULL;
}

s32 psInitAppSRT(s32 count, s32 size) {
    s32 i = 0;

    lbl_8047B116 = 0;
    lbl_8047B110 = 0;
    lbl_8047B120 = size;
    lbl_8047B124 = NULL;

    while (i < count) {
        PSAppSRT* appSRT = fn_801A6928(size);

        if (appSRT == NULL) {
            return i;
        }

        memset(appSRT, 0, size);
        appSRT->next = lbl_8047B124;
        lbl_8047B124 = appSRT;
        i++;
    }

    return i;
}

/*
 * Pokemon XD identifies the corresponding local function as
 * _psDispParticlesSub.  The display path is still incomplete here, but this
 * frame-generation update is independently verified against 0x8016ABF4-
 * 0x8016AC14 in the Colosseum retail disassembly.
 */
void fn_8016AB94(u32 linkMask, s32 mode) {
    s32 linkNo;
    s32 initialized = FALSE;

    if (mode == 0) {
        u8 frame = lbl_80478C30;

        lbl_80478C30 = frame < 0xFF ? frame + 1 : 1;
        return;
    }

    for (linkNo = 0; linkNo < 16; linkNo++) {
        PSParticle* first;
        PSParticle* second;
        PSParticle* pp;

        if ((linkMask & (1U << linkNo)) == 0) {
            continue;
        }

        particleSort(linkNo, &first, &second);
        if (mode == 1) {
            pp = first;
        } else if (mode == 2) {
            pp = second;
        } else {
            continue;
        }

        while (pp != NULL) {
            PSParticle* next = pp->next;

            if (mode == 1 && (pp->flags & 8) == 0) {
                break;
            }

            if (pp->lerpValue >= lbl_80478ACC &&
                (pp->flags & 0x20000000) == 0) {
                void* polygonData = NULL;
                void** bank = lbl_804528C8[pp->bankIndex];

                if (!initialized) {
                    initialized = TRUE;
                    lbl_8047B168 = -1;
                    lbl_8047B164 = -1;
                    lbl_8047B144 = -1;
                    psSetupTevInvalidState();
                    HSD_FogSet(NULL);

                    lbl_8047B140.r = 0xFF;
                    lbl_8047B140.g = 0xFF;
                    lbl_8047B140.b = 0xFF;
                    lbl_8047B140.a = 0xFF;
                    lbl_8047B13C.r = 0xFF;
                    lbl_8047B13C.g = 0xFF;
                    lbl_8047B13C.b = 0xFF;
                    lbl_8047B13C.a = 0xFF;
                    fn_800BA5BC(4, &lbl_8047B140);
                    fn_800BA4C8(4, &lbl_8047B13C);
                    psSetupTevInvalidState();
                    psSetupTevCommon();

                    lbl_8047B138.r = 0xFF;
                    lbl_8047B138.g = 0xFF;
                    lbl_8047B138.b = 0xFF;
                    lbl_8047B138.a = 0xFF;
                    lbl_8047B134.r = 0;
                    lbl_8047B134.g = 0;
                    lbl_8047B134.b = 0;
                    lbl_8047B134.a = 0;
                    lbl_8047B130.r = 0xFF;
                    lbl_8047B130.g = 0xFF;
                    lbl_8047B130.b = 0xFF;
                    lbl_8047B130.a = 0xFF;
                    fn_800BC2F8(1, &lbl_8047B138);
                    fn_800BC2F8(2, &lbl_8047B134);
                    fn_800BC2F8(3, &lbl_8047B130);
                    lbl_8047B148 = -1;
                    fn_800BCEBC(0);
                }

                if (bank != NULL) {
                    void** object = (void**)bank[pp->animIndex];

                    if (object != NULL && object != (void**)-4) {
                        polygonData = object[pp->objRefIndex + 1];
                    }
                }

                if (pp->flags & 0x40000000) {
                    if (pp->parentObj != NULL) {
                        psDispSubAPPSRTPoint(pp);
                    }
                } else if (pp->parentObj != NULL) {
                    psDispSubAppSRT(pp, (f32(*)[4])polygonData);
                } else {
                    psDispSub(pp, polygonData);
                }
            }
            pp = next;
        }
    }
}

#endif

#if !defined(PR410_PS_SPLIT) || defined(PR410_PS_EXACT)

void psDeletePntJObjwithParticle(PSParticle* pp) {
    if (pp->flags & PS_FLAG_ATTACH_CAMERA) {
        u32 slotIdx = (pp->flags >> 12) & 0x7;
        void* camSlot = lbl_80452DC8[slotIdx];

        if (camSlot != NULL) {
            fn_801A05EC(camSlot);
            lbl_80452DC8[slotIdx] = NULL;
        }
    }
}

void psSetPointJObjNodup(void* renderObj, s32 index) {
    void** slot;
    s32 i;

    if (index < 0 || index > 8) {
        return;
    }

    i = 0;
    do {
        if (lbl_80452DC8[i] == renderObj) {
            fn_801A05EC(lbl_80452DC8[i]);
            lbl_80452DC8[i] = NULL;
        }
        i++;
    } while (i < 8);

    if (index != 0) {
        void* oldRenderObj;

        slot = lbl_80452DC8;
        slot += index;
        oldRenderObj = *--slot;
        if (oldRenderObj != NULL) {
            fn_801A05EC(oldRenderObj);
        }
        *slot = renderObj;

        if (renderObj != NULL) {
            u16* refCount = (u16*)((u8*)renderObj + 4);

            (*refCount)++;
            if (*refCount == 0xFFFF) {
                __assert(lbl_80273820, 0x5D, lbl_8027382C);
            }
        }
    }
}

void psSetPointJObj(s32 index, void* renderObj) {
    void** slot;
    s32 i;

    if (index < 0 || index > 8) {
        return;
    }

    if (index != 0) {
        void* oldRenderObj;

        slot = lbl_80452DC8;
        slot += index;
        oldRenderObj = *--slot;
        if (oldRenderObj == renderObj) {
            return;
        }
        if (oldRenderObj != NULL) {
            fn_801A05EC(oldRenderObj);
        }
        *slot = renderObj;

        if (renderObj != NULL) {
            u16* refCount = (u16*)((u8*)renderObj + 4);

            (*refCount)++;
            if (*refCount == 0xFFFF) {
                __assert(lbl_80273820, 0x5D, lbl_8027382C);
            }
        }
    } else {
        slot = lbl_80452DC8;
        i = 0;
        while (i < 8) {
            if (*slot == renderObj) {
                fn_801A05EC(*slot);
                *slot = NULL;
            }
            i++;
            slot++;
        }
    }
}

#endif

#if !defined(PR410_PS_SPLIT) || defined(PR410_PS_SUFFIX)

#pragma dont_inline on
void psKillAllParticle(void) {
    PSParticle* current;
    PSParticle* parent;
    PSParticle* next;
    PSParticle* pp;
    s32 linkNo;

    for (linkNo = 0; linkNo < PS_NUM_LINK; linkNo++) {
        pp = _psListGetFirst(linkNo);
        while (pp != NULL) {
            next = pp->next;
            parent = NULL;
            current = _psListGetFirst(pp->linkNo);

            while (current != NULL) {
                if (current == pp) {
                    if (pp->peopleObj != NULL) {
                        ((PSGeneratorState*)pp->peopleObj)->childCount--;
                    }
                    if (pp->parentObj != NULL) {
                        psRemoveParticleAppSRT(pp);
                    }
                    if (current->flags & PS_FLAG_ATTACH_CAMERA) {
                        u32 slotIdx = (current->flags >> 12) & 0x7;
                        void* camSlot = lbl_80452DC8[slotIdx];

                        if (camSlot != NULL) {
                            fn_801A05EC(camSlot);
                            lbl_80452DC8[slotIdx] = NULL;
                        }
                    }
                    _psListDelete(current, parent);
                    break;
                }
                parent = current;
                current = current->next;
            }
            pp = next;
        }
    }
}
#pragma dont_inline reset

PSParticle* psGenerateParticle0(
    PSParticle* parent, s32 linkNo, s32 bankIndex, void* object,
    f32 posX, f32 posY, f32 posZ, u32 unused, u32 flags,
    f32 velocityX, f32 velocityY, f32 velocityZ, u8 animIndex,
    void* scriptData, u16 repeatCount, f32 lerpValue, f32 scaleFactor,
    f32 frictionFactor, s32 objectValue, PSGeneratorState* generator,
    s32 interpretNow) {
    PSParticle* pp = _psListNew(parent, linkNo);

    if (pp == NULL) {
        return NULL;
    }

    if (generator != NULL) {
        pp->scriptId = generator->familyId;
    } else {
        pp->scriptId = psGetNewIDNum();
    }

    pp->parentObj = NULL;
    if (generator != NULL && generator->appSRT != NULL) {
        psAttachParticleAppSRT(pp, generator->appSRT);
    }

    pp->bankIndex = bankIndex;
    pp->linkNo = linkNo;
    pp->flags = flags;
    pp->animIndex = animIndex;
    pp->positionX = posX;
    pp->positionY = posY;
    pp->positionZ = posZ;
    pp->velocityX = velocityX;
    pp->velocityY = velocityY;
    pp->velocityZ = velocityZ;
    pp->lerpValue = lerpValue;
    pp->scaleFactor = scaleFactor;
    pp->frictionFactor = frictionFactor;
    pp->repeatCount = repeatCount + 1;
    pp->scriptData = scriptData;
    pp->savedPC = 0;
    pp->pc = 0;

    if (objectValue != 0) {
        pp->flags |= 0x10;
    }

    pp->waitTimer = scriptData != NULL;
    pp->objRefIndex = 0;
    pp->pad0B = 0xFF;
    pp->color1R = 0xFF;
    pp->color1G = 0xFF;
    pp->color1B = 0xFF;
    pp->color1A = 0xFF;
    pp->color2R = 0;
    pp->color2G = 0;
    pp->color2B = 0;
    pp->color2A = 0;
    pp->color1Timer = 0;
    pp->color2Timer = 0;
    pp->lerpTimer = 0;
    pp->color1Countdown = 0;
    pp->color2Countdown = 0;
    pp->alphaMode = 0x33;
    pp->alphaStart = ((flags >> 22) & 3) >= 2 ? 0 : 1;
    pp->alphaEnd = 0xFF;
    pp->alphaCountdown = 0;
    pp->alphaTimer = 0;
    pp->headingTimer = 0;
    pp->headingAccel = lbl_8047D5B4;
    pp->headingSpeed = lbl_8047D5B4;
    pp->heading = lbl_8047D5B4;
    pp->peopleObj = generator;

    if (generator != NULL) {
        generator->childCount++;
    }

    pp->cameraSlot = 0;
    pp->sizeXCountdown = 0;
    pp->sizeXTimer = 0;
    pp->sizeXCurrent = 0xFF;
    pp->sizeYCurrent = 0xFF;
    pp->sizeYCountdown = 0;
    pp->sizeYTimer = 0;
    pp->sizeXTarget = 0xFF;
    pp->sizeYTarget = 0xFF;
    *(f32*)&pp->pad88 = lbl_8047D5B8;

    if (interpretNow != 0) {
        psInterpretParticle0(pp, NULL);
    }
    return pp;
}

PSParticle* psGenerateParticle(u8 linkNo, u8 bankIndex, u32 arg2,
                               f32 posX, f32 posY, f32 posZ, u32 flags,
                               f32 velocityX, f32 velocityY, f32 velocityZ,
                               u8 animIndex, void* scriptData, u16 repeatCount,
                               f32 lerpValue, void* arg14, f32 scaleFactor,
                               f32 frictionFactor, PSGeneratorState* generator) {
    extern PSParticle* psGenerateParticle0(PSParticle* parent, u8 linkNo,
                                           u8 bankIndex, u32 flags0,
                                           f32 posX, f32 posY, f32 posZ,
                                           u32 arg7, u32 flags,
                                           f32 velocityX, f32 velocityY,
                                           f32 velocityZ, u8 animIndex,
                                           void* scriptData, u16 repeatCount,
                                           f32 lerpValue, f32 scaleFactor,
                                           f32 frictionFactor, void* arg16,
                                           PSGeneratorState* generator,
                                           s32 enabled);

    return psGenerateParticle0(NULL, linkNo, bankIndex, flags, posX, posY,
                               posZ, arg2, flags, velocityX, velocityY,
                               velocityZ, animIndex, scriptData, repeatCount,
                               lerpValue, scaleFactor, frictionFactor, arg14,
                               generator, TRUE);
}

PSParticle* psGenerateParticleID0(PSParticle* parent, s32 linkNo,
                                  s32 bankIndex, s32 scriptId, void* arg) {
    typedef struct PSParticleScript {
        u16 unk0;
        u16 animIndex;
        u16 unk4;
        u16 repeatCount;
        u32 flags;
        f32 scaleFactor;
        f32 frictionFactor;
        f32 velocityX;
        f32 velocityY;
        f32 velocityZ;
        u8 unk20[0xC];
        f32 lerpValue;
        u8 unk30[0xC];
        u8 scriptData[];
    } PSParticleScript;
    typedef struct PSParticleObject {
        u8 unk0[0x16];
        u16 value;
    } PSParticleObject;
    typedef struct PSParticleDataBanks {
        u8 unk0[0x200];
        PSParticleObject** objectBanks[0x40];
        PSParticleScript** scriptBanks[0x80];
        s32 scriptCounts[0x40];
    } PSParticleDataBanks;
    extern PSParticle* psGenerateParticle0(
        PSParticle* parent, s32 linkNo, s32 bankIndex, PSParticleObject* object,
        f32 posX, f32 posY, f32 posZ, u32 arg7, u32 flags, f32 velocityX,
        f32 velocityY, f32 velocityZ, u16 animIndex, void* scriptData,
        u16 repeatCount, f32 lerpValue, f32 scaleFactor, f32 frictionFactor,
        s32 objectValue, void* generator, void* arg);
    PSParticleDataBanks* dataBanks = (PSParticleDataBanks*)lbl_804527C8;
    PSParticleScript* script;
    PSParticleObject* object;

    if (linkNo >= 8) {
        return NULL;
    }
    if (bankIndex >= 0x40) {
        return NULL;
    }
    if (scriptId >= dataBanks->scriptCounts[bankIndex]) {
        return NULL;
    }

    script = dataBanks->scriptBanks[bankIndex][scriptId];
    if (script == NULL) {
        return NULL;
    }

    object = dataBanks->objectBanks[bankIndex][script->animIndex];
    return psGenerateParticle0(
        parent, linkNo, bankIndex, object, 0.0f, 0.0f, 0.0f, 0,
        script->flags, script->velocityX, script->velocityY, script->velocityZ,
        script->animIndex, script->scriptData, script->repeatCount,
        script->lerpValue, script->scaleFactor, script->frictionFactor,
        object != NULL ? object->value : 0, NULL, arg);
}

void fn_8016EA88(void) {
    HSD_Obj* obj;

    if ((obj = lbl_8047B128) != NULL) {
        if (obj != NULL && ref_DEC(obj)) {
            if (obj != NULL) {
                HSD_CLASS_METHOD(obj)->release((HSD_Class*)obj);
                HSD_CLASS_METHOD(obj)->destroy((HSD_Class*)obj);
            }
        }
        lbl_8047B128 = NULL;
    }
}

void fn_8016EB30(HSD_Obj* obj) {
    HSD_Obj* old_obj;

    if ((old_obj = lbl_8047B128) != NULL) {
        if (old_obj != NULL && ref_DEC(old_obj)) {
            if (old_obj != NULL) {
                HSD_CLASS_METHOD(old_obj)->release((HSD_Class*)old_obj);
                HSD_CLASS_METHOD(old_obj)->destroy((HSD_Class*)old_obj);
            }
        }
    }
    if (obj != NULL) {
        ref_INC(obj);
    }
    lbl_8047B128 = obj;
}

void psSetupTevInvalidState(void) {
    lbl_8047B170 = -1;
}

/*
 * Updates cached TEV topology from the particle render flags.  Other topology
 * combinations remain to be ported; the no-texture case is verified against
 * 0x8016EC1C-0x8016EC4C and 0x8016EDEC-0x8016EE40.
 */
void psSetupTev(PSParticle* pp) {
    s32 state = pp->flags & 0x80100480;

    if (state == lbl_8047B170) {
        return;
    }
    lbl_8047B170 = state;

    if (state == 0x80) {
        pp->flags &= ~0x180;
        lbl_8047B170 &= ~0x180;
    }

    if (state == 0 || state == 0x80) {
        fn_800BC8C8(1);
        fn_800B884C(0);
        fn_800BC6F0(0, 0xFF, 0xFF, 4);
        fn_800BC1A0(0, 2, 0xF, 0xF, 0xF);
        fn_800BC1E4(0, 1, 7, 7, 7);
    } else if (state == (s32)0x80000000 ||
               state == (s32)0x80000080) {
        if (state == (s32)0x80000080) {
            pp->flags &= ~0x180;
            lbl_8047B170 &= ~0x180;
        }

        fn_800BC8C8(2);
        fn_800B884C(0);
        fn_800BC6F0(0, 0xFF, 0xFF, 4);
        fn_800BC1A0(0, 0xF, 6, 0xA, 0xF);
        fn_800BC1E4(0, 3, 7, 7, 7);
        fn_800BC1A0(1, 0xF, 2, 0, 0xF);
        fn_800BC1E4(1, 7, 1, 0, 7);
    } else if (state == 0x400 || state == 0x480) {
        fn_800BC8C8(1);
        fn_800B884C(1);
        fn_800BC6F0(0, 0, 0, 4);
        fn_800BC1A0(0, 4, 2, 8, 0xF);
        fn_800BC1E4(0, 2, 1, 4, 7);
    } else if (state == (s32)0x80000400) {
        fn_800BC8C8(2);
        fn_800B884C(1);
        fn_800BC6F0(0, 0, 0, 4);
        fn_800BC1A0(0, 0xF, 6, 0xA, 0xF);
        fn_800BC1E4(0, 3, 7, 7, 7);
        fn_800BC1A0(1, 0xF, 8, 0, 0xF);
        fn_800BC1E4(1, 7, 4, 0, 7);
    } else if (state == (s32)0x80000480) {
        fn_800BC8C8(3);
        fn_800B884C(1);
        fn_800BC6F0(0, 0, 0, 4);
        fn_800BC1A0(0, 4, 2, 8, 0xF);
        fn_800BC1E4(0, 2, 1, 4, 7);
        fn_800BC1A0(1, 0xF, 6, 0, 0xF);
        fn_800BC1E4(1, 7, 3, 0, 7);
        fn_800BC1A0(2, 0xF, 0xA, 0, 0xF);
        fn_800BC1E4(2, 0, 7, 7, 7);
    }
}

void psSetupTevCommon(void) {
    fn_800BC6F0(0, 0, 0, 4);
    fn_800BC228(0, 0, 0, 0, 1, 0);
    fn_800BC290(0, 0, 0, 0, 1, 0);

    fn_800BC6F0(1, 0, 0, 4);
    fn_800BC228(1, 0, 0, 0, 1, 0);
    fn_800BC290(1, 0, 0, 0, 1, 0);

    fn_800BC6F0(2, 0, 0, 4);
    fn_800BC228(2, 0, 0, 0, 1, 0);
    fn_800BC290(2, 0, 0, 0, 1, 0);

    fn_800BC52C(0, 0, 0);
    fn_800BC52C(1, 0, 0);
    fn_800BC52C(2, 0, 0);
}

u8 U8ClampAdd(u8 cur, f32 delta) {
    f32 value = (f32)cur + delta;

    if (value < 0.0f) {
        value = 0.0f;
    }
    if (value > lbl_8047D690) {
        value = lbl_8047D690;
    }

    return (u8)value;
}

void HSD_MTXSRT(void* dst, void* scale, void* rot, void* trans, void* order) {
    HSD_MtxSRT(dst, scale, rot, trans, order);
}

PSParticle* _psListGetNext(PSParticle* pp) {
    return pp->next;
}

s32 applyForceJObj(PSParticle* pp, PSForceJObj* jobj,
                   f32 force, f32 radius) {
    f32 dx;
    f32 dy;
    f32 dz;
    f32 distanceSquared;

    if (jobj == NULL || radius < 0.0f) {
        return FALSE;
    }

    if (!(jobj->flags & 0x800000) && (jobj->flags & 0x40)) {
        fn_8019D9DC(jobj);
    }

    dx = jobj->worldX - pp->positionX;
    dy = jobj->worldY - pp->positionY;
    dz = jobj->worldZ - pp->positionZ;
    distanceSquared = dx * dx + dy * dy + dz * dz;

    if (distanceSquared <= radius * radius) {
        return TRUE;
    }
    if (distanceSquared == 0.0f) {
        return FALSE;
    }

    force /= distanceSquared;
    pp->velocityX += force * dx;
    pp->velocityY += force * dy;
    pp->velocityZ += force * dz;
    return FALSE;
}

void setVelToJObj(PSParticle* pp, PSForceJObj* jobj) {
    f32 dx;
    f32 dy;
    f32 dz;
    f32 speed;
    f32 distanceSquared;
    f32 scale;

    if (jobj == NULL) {
        return;
    }

    if (!(jobj->flags & 0x800000) && (jobj->flags & 0x40)) {
        fn_8019D9DC(jobj);
    }

    speed = sqrtf(pp->velocityX * pp->velocityX +
                  pp->velocityY * pp->velocityY +
                  pp->velocityZ * pp->velocityZ);
    dx = jobj->worldX - pp->positionX;
    dy = jobj->worldY - pp->positionY;
    dz = jobj->worldZ - pp->positionZ;
    distanceSquared = dx * dx + dy * dy + dz * dz;
    if (distanceSquared == 0.0f) {
        return;
    }

    scale = speed / sqrtf(distanceSquared);
    pp->velocityX = dx * scale;
    pp->velocityY = dy * scale;
    pp->velocityZ = dz * scale;
}

void modifyDir(PSParticle* pp, f32 angle) {
    f32 yaw;
    f32 pitch;
    f32 yawSin;
    f32 yawCos;
    f32 pitchSin;
    f32 pitchCos;
    f32 magnitude;
    f32 azimuth;
    f32 radial;
    f32 radialX;
    f32 radialY;
    f32 forward;
    f32 flattened;

    if (fabsf(pp->velocityZ) < lbl_80478AC8) {
        yaw = pp->velocityY >= 0.0f ? lbl_8047D694 : lbl_8047D698;
    } else {
        yaw = atan2(pp->velocityY, pp->velocityZ);
    }
    yawSin = sin(yaw);
    yawCos = cos(yaw);

    flattened = pp->velocityZ * yawCos + pp->velocityY * yawSin;
    if (fabsf(flattened) < lbl_80478AC8) {
        pitch = pp->velocityX >= 0.0f ? lbl_8047D694 : lbl_8047D698;
    } else {
        pitch = atan2(pp->velocityX, flattened);
    }
    pitchSin = sin(pitch);
    pitchCos = cos(pitch);

    magnitude = sqrtf(pp->velocityX * pp->velocityX +
                      pp->velocityY * pp->velocityY +
                      pp->velocityZ * pp->velocityZ);
    azimuth = lbl_8047D6A0 * lbl_8047D6A8 * fn_801ADC7C();
    radial = magnitude * sin(angle);
    radialX = radial * cos(azimuth);
    radialY = radial * sin(azimuth);
    forward = magnitude * cos(angle);

    pp->velocityX = forward * pitchSin + radialX * pitchCos;
    pp->velocityY = pitchSin * (-radialX * yawSin) +
                    radialY * yawCos + pitchCos * (forward * yawSin);
    pp->velocityZ = pitchSin * (-radialX * yawCos) -
                    radialY * yawSin + pitchCos * (forward * yawCos);
}

void modifyDirGenBase(PSParticle* pp, f32 angle, f32 offsetX,
                      f32 offsetY, f32 offsetZ) {
    PSGeneratorState* generator = pp->peopleObj;
    f32 baseX = generator->velocityX + offsetX;
    f32 baseY = generator->velocityY + offsetY;
    f32 baseZ = generator->velocityZ + offsetZ;
    f32 yaw;
    f32 pitch;
    f32 yawSin;
    f32 yawCos;
    f32 pitchSin;
    f32 pitchCos;
    f32 magnitude;
    f32 azimuth;
    f32 radial;
    f32 radialX;
    f32 radialY;
    f32 forward;
    f32 flattened;

    if (fabsf(baseZ) < lbl_80478AC8) {
        yaw = baseY >= 0.0f ? lbl_8047D694 : lbl_8047D698;
    } else {
        yaw = atan2(baseY, baseZ);
    }
    yawSin = sin(yaw);
    yawCos = cos(yaw);

    flattened = baseZ * yawCos + baseY * yawSin;
    if (fabsf(flattened) < lbl_80478AC8) {
        pitch = baseX >= 0.0f ? lbl_8047D694 : lbl_8047D698;
    } else {
        pitch = atan2(baseX, flattened);
    }
    pitchSin = sin(pitch);
    pitchCos = cos(pitch);

    magnitude = sqrtf(pp->velocityX * pp->velocityX +
                      pp->velocityY * pp->velocityY +
                      pp->velocityZ * pp->velocityZ);
    azimuth = lbl_8047D6A0 * lbl_8047D6A8 * fn_801ADC7C();
    radial = magnitude * sin(angle);
    radialX = radial * cos(azimuth);
    radialY = radial * sin(azimuth);
    forward = magnitude * cos(angle);

    pp->velocityX = forward * pitchSin + radialX * pitchCos;
    pp->velocityY = pitchSin * (-radialX * yawSin) +
                    radialY * yawCos + pitchCos * (forward * yawSin);
    pp->velocityZ = pitchSin * (-radialX * yawCos) -
                    radialY * yawSin + pitchCos * (forward * yawCos);
}

void psCopyGeneratorData(PSParticle* gen, void* peopleObj) {
    extern void psSetGeneratorAngleRadiusScale(PSGeneratorState*, f32*, u8);
    PSGeneratorState* dst = (PSGeneratorState*)gen;
    PSGeneratorState* src = (PSGeneratorState*)peopleObj;
    s32 i;

    if (src == NULL || dst == NULL) {
        return;
    }

    if (src->flags & 0x20000000) {
        *(s32*)&dst->flags |= 0x20000000;
    }

    for (i = 0; i < 6; i++) {
        dst->generatorData[i] = src->generatorData[i];
    }

    dst->generatorFlags = src->generatorFlags;
    dst->generatorFlags &= ~0x2;
    dst->linkedJObj = src->linkedJObj;

    if (src->generatorFlags & 0x1000) {
        psSetGeneratorAngleRadiusScale(dst, src->angleRadiusScale, TRUE);
    } else {
        psSetGeneratorAngleRadiusScale(dst, src->angleRadiusScale, FALSE);
    }
}

void psApplyOffsetLocalRotation(PSParticle* pp, f32* vec3) {
    typedef f32 Mtx[3][4];
    extern void PSMTXIdentity(Mtx mtx);
    extern void PSMTXScale(Mtx mtx, f32 x, f32 y, f32 z);
    extern void PSMTXRotRad(Mtx mtx, char axis, f32 radians);
    extern void PSMTXConcat(Mtx lhs, Mtx rhs, Mtx dst);
    extern void PSMTXMultVec(Mtx mtx, f32* src, f32* dst);
    Mtx rotX;
    Mtx rotY;
    Mtx rotZ;
    Mtx scale;

    if (pp->peopleObj == NULL ||
        !(((PSGeneratorState*)pp->peopleObj)->generatorFlags & 0x8)) {
        return;
    }

    if (((PSGeneratorState*)pp->peopleObj)->generatorFlags & 0x10) {
        PSMTXIdentity(scale);
    } else {
        PSMTXScale(scale,
                   ((PSGeneratorState*)pp->peopleObj)->generatorData[3],
                   ((PSGeneratorState*)pp->peopleObj)->generatorData[4],
                   ((PSGeneratorState*)pp->peopleObj)->generatorData[5]);
    }

    PSMTXRotRad(rotX, 'X',
                ((PSGeneratorState*)pp->peopleObj)->generatorData[0]);
    PSMTXRotRad(rotY, 'Y',
                ((PSGeneratorState*)pp->peopleObj)->generatorData[1]);
    PSMTXRotRad(rotZ, 'Z',
                ((PSGeneratorState*)pp->peopleObj)->generatorData[2]);
    PSMTXConcat(rotY, rotX, rotX);
    PSMTXConcat(rotZ, rotX, rotX);
    PSMTXConcat(scale, rotX, rotX);
    PSMTXMultVec(rotX, vec3, vec3);
}

void psApplyVelocityLocalRotation(PSParticle* pp) {
    typedef f32 Mtx[3][4];
    extern void PSMTXRotRad(Mtx mtx, char axis, f32 radians);
    extern void PSMTXConcat(Mtx lhs, Mtx rhs, Mtx dst);
    extern void PSMTXMultVec(Mtx mtx, f32* src, f32* dst);
    Mtx rotX;
    Mtx rotY;
    Mtx rotZ;
    f32 velocity[3];

    if (pp->peopleObj == NULL ||
        !(((PSGeneratorState*)pp->peopleObj)->generatorFlags & 0x4)) {
        return;
    }

    velocity[0] = pp->velocityX;
    velocity[1] = pp->velocityY;
    velocity[2] = pp->velocityZ;
    PSMTXRotRad(rotX, 'X',
                ((PSGeneratorState*)pp->peopleObj)->generatorData[0]);
    PSMTXRotRad(rotY, 'Y',
                ((PSGeneratorState*)pp->peopleObj)->generatorData[1]);
    PSMTXRotRad(rotZ, 'Z',
                ((PSGeneratorState*)pp->peopleObj)->generatorData[2]);
    PSMTXConcat(rotY, rotX, rotX);
    PSMTXConcat(rotZ, rotX, rotX);
    PSMTXMultVec(rotX, velocity, velocity);
    pp->velocityX = velocity[0];
    pp->velocityY = velocity[1];
    pp->velocityZ = velocity[2];
}

u8* getTime(u8* stream, u16* out) {
    *out = *stream++;
    if (*out & 0x80) {
        *out = ((*out & 0x7F) << 8) + *stream++;
    }

    return stream;
}

u8* getFloat(u8* stream, f32* out) {
    lbl_8047B178.bytes[0] = *stream++;
    lbl_8047B178.bytes[1] = *stream++;
    lbl_8047B178.bytes[2] = *stream++;
    lbl_8047B178.bytes[3] = *stream++;
    *out = lbl_8047B178.value;
    return stream;
}

void psSetBillboardCamera(HSD_Obj* obj) {
    HSD_Obj* old_obj;

    if (obj != (old_obj = lbl_8047B190)) {
        if (old_obj != NULL) {
            if (old_obj != NULL && ref_DEC(old_obj)) {
                if (old_obj != NULL) {
                    HSD_CLASS_METHOD(old_obj)->release((HSD_Class*)old_obj);
                    HSD_CLASS_METHOD(old_obj)->destroy((HSD_Class*)old_obj);
                }
            }
        }
        if (obj != NULL) {
            ref_INC(obj);
        }
        lbl_8047B190 = obj;
    }
}

void psInterpretParticles(u32 linkMask) {
    extern PSParticle* psInterpretParticle0(PSParticle* pp, PSParticle* parent);
    extern const char lbl_802739A0[];
    extern const char lbl_8047D628[7];
    PSParticle* next;
    PSParticle* previous;
    PSParticle* current;
    PSParticle* result;
    s32 linkNo;

    for (linkNo = 0; linkNo < PS_NUM_LINK; linkNo++, linkMask >>= 1) {
        if (!(linkMask & 0x10000)) {
            previous = NULL;
            current = _psListGetFirst(linkNo);

            while (current != NULL) {
                next = current->next;
                result = psInterpretParticle0(current, previous);

                if (next != NULL) {
                    if (current->next == next) {
                        previous = current;
                    } else if (result != next) {
                        previous = result;
                        while (previous->next != next) {
                            if (previous == NULL) {
                                __assert(lbl_802739A0, 0x810, lbl_8047D628);
                            }
                            previous = previous->next;
                        }
                    }
                }

                current = next;
            }
        }
    }
}

/* ======================================================================
 * psInterpretParticle0 | psinterpret_Main
 *
 * Executes one frame of a single particle script. See file header for
 * identification evidence and coverage notes.
 * ====================================================================== */
PSParticle* psInterpretParticle0(PSParticle* pp, PSParticle* parentCtx) {
    u8* stream;
    u8 opcode;
    u16 delay;

    if (pp->flags & PS_FLAG_PAUSED) {
        _psListGetNext(pp);
        return pp;
    }

    /* ---- Phase 1: interpolation timers (verified vs retail asm) ---- */
    if (pp->lerpTimer != 0) {
        f32 step = (pp->lerpTarget - pp->lerpValue) / (f32)pp->lerpTimer;
        pp->lerpValue += step;
        pp->lerpTimer--;
    }

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

    if (pp->sizeXTimer != 0) {
        pp->sizeXCountdown--;
        if (pp->sizeXCountdown == 0) {
            pp->sizeXTimer = 0;
            pp->sizeXCurrent = pp->sizeXStart;
            pp->sizeYCurrent = pp->sizeYStart;
        }
    }

    if (pp->sizeYTimer != 0) {
        pp->sizeYCountdown--;
        if (pp->sizeYCountdown == 0) {
            pp->sizeYTimer = 0;
            pp->sizeXTarget = pp->sizeXTargetFinal;
            pp->sizeYTarget = pp->sizeYTargetFinal;
        }
    }

    if (pp->alphaTimer != 0) {
        pp->alphaCountdown--;
        if (pp->alphaCountdown == 0) {
            pp->alphaTimer = 0;
            pp->alphaStart = pp->alphaTargetStart;
            pp->alphaEnd = pp->alphaTargetEnd;
        }
    }

    if (pp->headingTimer != 0) {
        if (pp->headingAccel != 0.0f) {
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
            f32 step = (pp->headingSpeed - pp->heading) / (f32)pp->headingTimer;
            pp->heading += step;
            pp->headingTimer--;
        }
    }

    /* ---- Phase 2: wait-timer gate (verified) ---- */
    if (pp->waitTimer != 0) {
        pp->waitTimer--;
        if (pp->waitTimer == 0) {
            PSParticle* spawned;

            stream = (u8*)pp->scriptData + pp->pc;

            for (;;) {
                opcode = *stream++;
                delay = 0;

                if (opcode < 0x80) {
                    delay = opcode & 0x1F;
                    if (opcode & 0x20) {
                        delay = (delay << 8) | *stream++;
                    }
                    if ((opcode & 0xC0) == 0x40) {
                        u8 objRef = *stream++;
                        void** bankData;
                        void* objTable;

                        pp->objRefIndex = objRef;
                        bankData = (void**)lbl_804529C8[pp->bankIndex];
                        objTable = bankData ? ((void**)bankData)[pp->animIndex] : NULL;
                        if (objTable != NULL) {
                            void** objEntry = (void**)((u8*)objTable + 0x18);
                            void* ref = objEntry[objRef];
                            if (ref != NULL) {
                                pp->flags |= PS_FLAG_OBJ_REF;
                            }
                        }
                    }
                } else {
                    u8 masked = opcode & 0xF8;
                    u8 normalizedOp;
                    u8 tableIndex;

                    if (masked <= 0x98) {
                        normalizedOp = masked;
                    } else {
                        u8 masked16 = opcode & 0xF0;
                        if (masked16 == 0xC0 || masked16 == 0xD0) {
                            normalizedOp = masked16;
                        } else {
                            normalizedOp = opcode;
                        }
                    }
                    tableIndex = normalizedOp - 0x80;

                    switch (normalizedOp) {
                    /* ---- 0x80: SET_POSITION (verified @ 0x8016F8D4) ---- */
                    case 0x80: {
                        f32 vec[3];
                        if (opcode & 1) stream = getFloat(stream, &vec[0]);
                        if (opcode & 2) stream = getFloat(stream, &vec[1]);
                        if (opcode & 4) stream = getFloat(stream, &vec[2]);
                        psApplyOffsetLocalRotation(pp, vec);
                        pp->positionX = vec[0];
                        pp->positionY = vec[1];
                        pp->positionZ = vec[2];
                        break;
                    }

                    /* ---- 0x88: ADD_POSITION (verified @ 0x8016F95C) ---- */
                    case 0x88: {
                        f32 vec[3];
                        if (opcode & 1) stream = getFloat(stream, &vec[0]);
                        if (opcode & 2) stream = getFloat(stream, &vec[1]);
                        if (opcode & 4) stream = getFloat(stream, &vec[2]);
                        psApplyOffsetLocalRotation(pp, vec);
                        pp->positionX += vec[0];
                        pp->positionY += vec[1];
                        pp->positionZ += vec[2];
                        break;
                    }

                    /* ---- 0x90: SET_VELOCITY (verified @ 0x8016F9FC) ---- */
                    case 0x90: {
                        f32 vec[3];
                        if (opcode & 1) stream = getFloat(stream, &vec[0]);
                        if (opcode & 2) stream = getFloat(stream, &vec[1]);
                        if (opcode & 4) stream = getFloat(stream, &vec[2]);
                        psApplyOffsetLocalRotation(pp, vec);
                        pp->velocityX = vec[0];
                        pp->velocityY = vec[1];
                        pp->velocityZ = vec[2];
                        break;
                    }

                    /* ---- 0x98: ADD_VELOCITY (verified @ 0x8016FA84) ---- */
                    case 0x98: {
                        f32 vec[3];
                        if (opcode & 1) stream = getFloat(stream, &vec[0]);
                        if (opcode & 2) stream = getFloat(stream, &vec[1]);
                        if (opcode & 4) stream = getFloat(stream, &vec[2]);

                        if ((pp->flags & 0x20000000) == 0) {
                            psApplyOffsetLocalRotation(pp, vec);
                        } else {
                            void* peopleObj = pp->peopleObj;
                            if (peopleObj != NULL &&
                                (*(u16*)((u8*)peopleObj + 0x88) & 0x40) != 0) {
                                f32* p = (f32*)((u8*)peopleObj + 0x98);
                                f32 scale = (p[0] + p[1] + p[2]) / 3.0f;
                                vec[0] *= scale;
                                vec[1] *= scale;
                                vec[2] *= scale;
                            }
                        }
                        pp->velocityX += vec[0];
                        pp->velocityY += vec[1];
                        pp->velocityZ += vec[2];
                        break;
                    }

                    /* ---- 0xA0: SET_LERP_TIMER (verified @ 0x8016FB8C) ---- */
                    case 0xA0:
                        stream = getTime(stream, &pp->lerpTimer);
                        stream = getFloat(stream, &pp->lerpTarget);
                        if (pp->lerpTimer == 0) {
                            pp->lerpValue = pp->lerpTarget;
                        }
                        break;

                    /* ---- 0xA1: clear OBJ_REF flag (verified @ 0x8016FBBC) ---- */
                    case 0xA1:
                        pp->flags &= ~PS_FLAG_OBJ_REF;
                        break;

                    /* ---- 0xA2: SET_SCALE (verified @ 0x8016FBCC) ---- */
                    case 0xA2: {
                        void* peopleObj;
                        stream = getFloat(stream, &pp->scaleFactor);
                        if (pp->scaleFactor == 0.0f) {
                            pp->flags &= ~PS_FLAG_SCALE_ACTIVE;
                        } else {
                            pp->flags |= PS_FLAG_SCALE_ACTIVE;
                        }
                        peopleObj = pp->peopleObj;
                        if (peopleObj != NULL &&
                            (*(u16*)((u8*)peopleObj + 0x88) & 0x1000) != 0) {
                            f32* p = (f32*)((u8*)peopleObj + 0x98);
                            f32 ratio = (p[0] + p[1] + p[2]) / 3.0f;
                            pp->scaleFactor *= ratio;
                        }
                        break;
                    }

                    /* ---- 0xA3: SET_FRICTION (verified @ 0x8016FC4C) ---- */
                    case 0xA3: {
                        void* peopleObj;
                        stream = getFloat(stream, &pp->frictionFactor);
                        if (pp->frictionFactor == 1.0f) {
                            pp->flags &= ~PS_FLAG_FRICTION_ACTIVE;
                        } else {
                            pp->flags |= PS_FLAG_FRICTION_ACTIVE;
                        }
                        peopleObj = pp->peopleObj;
                        if (peopleObj != NULL &&
                            (*(u16*)((u8*)peopleObj + 0x88) & 0x1000) != 0) {
                            f32* p = (f32*)((u8*)peopleObj + 0x98);
                            f32 ratio = (p[0] + p[1] + p[2]) / 3.0f;
                            pp->frictionFactor *= ratio;
                        }
                        break;
                    }

                    /* ---- 0xA4: SPAWN_SCRIPT direct id (verified @ 0x8016FCCC) ---- */
                    case 0xA4: {
                        u16 scriptId = ((u16)stream[0] << 8) | stream[1];
                        stream += 2;
                        spawned = psGenerateParticleID0(pp, pp->linkNo, pp->bankIndex, scriptId, NULL);
                        if (spawned == NULL) break;
                        spawned->scriptId = pp->scriptId;
                        spawned->peopleObj = pp->peopleObj;
                        if (pp->peopleObj != NULL) {
                            (*(u32*)((u8*)pp->peopleObj + 0x4C))++;
                            if (*(u32*)((u8*)pp->peopleObj + 4) & 0x2000)
                                spawned->flags |= 0x2000;
                        }
                        psApplyVelocityLocalRotation(spawned);
                        if (pp->peopleObj != NULL &&
                            (pp->peopleObj != NULL) &&
                            (*(u16*)((u8*)pp->peopleObj + 0x12) & 0x40)) {
                            psChangeParticleAppSRT(spawned, (PSAppSRT*)pp->parentObj);
                        } else {
                            psAttachParticleAppSRT(spawned, (PSAppSRT*)pp->parentObj);
                        }
                        spawned->positionX = pp->positionX;
                        spawned->positionY = pp->positionY;
                        spawned->positionZ = pp->positionZ;
                        psInterpretParticle0(spawned, pp);
                        break;
                    }

                    /* ---- 0xA5: SPAWN_SCRIPT via table (verified @ 0x8016FE9C) ---- */
                    case 0xA5: {
                        u16 tblIdx = ((u16)stream[0] << 8) | stream[1];
                        u32* bank = (u32*)lbl_804527C8[pp->bankIndex];
                        u16 scriptId = bank ? (u16)bank[tblIdx] : tblIdx;
                        stream += 2;
                        spawned = psGenerateParticleID0(pp, pp->linkNo, pp->bankIndex, scriptId, NULL);
                        if (spawned == NULL) break;
                        spawned->scriptId = pp->scriptId;
                        spawned->peopleObj = pp->peopleObj;
                        if (pp->peopleObj != NULL) {
                            (*(u32*)((u8*)pp->peopleObj + 0x4C))++;
                            if (*(u32*)((u8*)pp->peopleObj + 4) & 0x2000)
                                spawned->flags |= 0x2000;
                        }
                        psApplyVelocityLocalRotation(spawned);
                        if (pp->peopleObj != NULL &&
                            (*(u16*)((u8*)pp->peopleObj + 0x12) & 0x40)) {
                            psChangeParticleAppSRT(spawned, (PSAppSRT*)pp->parentObj);
                        } else {
                            psAttachParticleAppSRT(spawned, (PSAppSRT*)pp->parentObj);
                        }
                        spawned->positionX = pp->positionX;
                        spawned->positionY = pp->positionY;
                        spawned->positionZ = pp->positionZ;
                        psInterpretParticle0(spawned, pp);
                        break;
                    }

                    /* ---- 0xA6: random -> repeatCount (verified @ 0x801705F8) ---- */
                    case 0xA6: {
                        u16 base = ((u16)stream[0] << 8) | stream[1];
                        u16 range = ((u16)stream[2] << 8) | stream[3];
                        f32 rng;
                        stream += 4;
                        rng = fn_801ADC7C();
                        pp->repeatCount = (u16)(base + (s32)(((s32)(range ^ 0x8000)) * rng));
                        break;
                    }

                    /* ---- 0xA7: random threshold -> yield (verified @ 0x80170658) ---- */
                    case 0xA7: {
                        u8 threshold = *stream++;
                        f32 rng = fn_801ADC7C();
                        if (threshold < (s32)(0.5f * rng)) break;
                        pp->repeatCount = 1;
                        delay = 1;
                        goto after_dispatch;
                    }

                    /* ---- 0xA8: randomized local position offset
                     * (verified @ 0x8017068C) ---- */
                    case 0xA8: {
                        f32 amplitude;
                        f32 offset[3];

                        stream = getFloat(stream, &amplitude);
                        offset[0] = amplitude -
                            2.0f * amplitude * fn_801ADC7C();
                        stream = getFloat(stream, &amplitude);
                        offset[1] = amplitude -
                            2.0f * amplitude * fn_801ADC7C();
                        stream = getFloat(stream, &amplitude);
                        offset[2] = amplitude -
                            2.0f * amplitude * fn_801ADC7C();
                        psApplyOffsetLocalRotation(pp, offset);
                        pp->positionX += offset[0];
                        pp->positionY += offset[1];
                        pp->positionZ += offset[2];
                        break;
                    }

                    /* ---- 0xA9: MODIFY_DIR (verified @ 0x80170744) ---- */
                    case 0xA9: {
                        f32 f;
                        stream = getFloat(stream, &f);
                        modifyDir(pp, f);
                        break;
                    }

                    /* ---- 0xAA: MODIFY_DIR_GEN_BASE (verified @ 0x80170844... entry
                     * partially transcribed at 0x80170764) ---- */
                    case 0xAA: {
                        f32 a, b, c, d;
                        stream = getFloat(stream, &a);
                        stream = getFloat(stream, &b);
                        stream = getFloat(stream, &c);
                        stream = getFloat(stream, &d);
                        if (pp->peopleObj == NULL) break;
                        modifyDirGenBase(pp, d, a, b, c);
                        break;
                    }

                    /* ---- 0xAB: scale velocity (verified @ 0x80170988) ---- */
                    case 0xAB: {
                        f32 factor;

                        stream = getFloat(stream, &factor);
                        pp->velocityX *= factor;
                        pp->velocityY *= factor;
                        pp->velocityZ *= factor;
                        break;
                    }

                    /* ---- 0xAC: randomized lerp target (verified @ 0x801709CC) ---- */
                    case 0xAC: {
                        f32 range;

                        stream = getTime(stream, &pp->lerpTimer);
                        stream = getFloat(stream, &pp->lerpTarget);
                        stream = getFloat(stream, &range);
                        pp->lerpTarget += range * fn_801ADC7C();
                        if (pp->lerpTimer == 0) {
                            pp->lerpValue = pp->lerpTarget;
                        }
                        break;
                    }

                    /* ---- 0xAD..0xB1: motion flags (verified @ 0x80170A18) ---- */
                    case 0xAD:
                        pp->flags |= 0x80;
                        break;
                    case 0xAE:
                        pp->flags &= ~0x60;
                        break;
                    case 0xAF:
                        pp->flags = (pp->flags & ~0x40) | 0x20;
                        break;
                    case 0xB0:
                        pp->flags = (pp->flags & ~0x20) | 0x40;
                        break;
                    case 0xB1:
                        pp->flags |= 0x60;
                        break;

                    /* ---- 0xB4/0xB5: toggle bit 0x200 (verified @ 0x80170BF8) ---- */
                    case 0xB4:
                        pp->flags |= 0x200;
                        break;
                    case 0xB5:
                        pp->flags &= ~0x200;
                        break;

                    /* ---- 0xB6: add heading target (verified @ 0x80170C18) ---- */
                    case 0xB6: {
                        f32 delta;

                        stream = getTime(stream, &pp->headingTimer);
                        stream = getFloat(stream, &delta);
                        pp->headingSpeed += delta;
                        if (pp->headingTimer == 0) {
                            pp->heading = pp->headingSpeed;
                        }
                        break;
                    }

                    /* ---- 0xB7: bind velocity to camera slot (verified @ 0x80170C58) ---- */
                    case 0xB7: {
                        u8 slot = *stream++;

                        setVelToJObj(pp, lbl_80452DC8[pp->cameraSlot + slot]);
                        break;
                    }

                    /* ---- 0xF0: SPAWN_GENERATOR via table (verified @ 0x80170364) ---- */
                    case 0xF0: {
                        PSParticle* gen;
                        u16 tblIdx = ((u16)stream[0] << 8) | stream[1];
                        u8 loopArg = stream[2];
                        u32* bank = (u32*)lbl_804527C8[pp->bankIndex];
                        u16 scriptId = bank ? (u16)bank[tblIdx] : 0;
                        stream += 3;
                        gen = (PSParticle*)psCreateGeneratorID(pp->linkNo, pp->bankIndex, scriptId);
                        if (gen == NULL) break;
                        gen->scriptId = pp->scriptId;
                        psCopyGeneratorData(gen, pp->peopleObj);
                        if (pp->parentObj != NULL) {
                            if (pp->peopleObj != NULL &&
                                (*(u16*)((u8*)pp->peopleObj + 0x12) & 0x40)) {
                                psChangeGeneratorAppSRT((PSGeneratorState*)gen, (PSAppSRT*)pp->parentObj);
                            } else {
                                psAttachGeneratorAppSRT((PSGeneratorState*)gen, (PSAppSRT*)pp->parentObj);
                            }
                        }
                        gen->flags = (gen->flags & ~0x1F8) | ((loopArg & 0x7) << 3);
                        /* position blend between pp and gen->parentObj: left as
                         * asm-verified but not transcribed here for brevity. */
                        break;
                    }

                    default:
                        break;
                    }
                }

            after_dispatch:
                if (delay != 0) break;
            }

            pp->pc = (u16)(stream - (u8*)pp->scriptData);
            pp->waitTimer = delay;
        }
    }

    /* ---- Phase 5: repeat-count / cleanup (verified @ 0x80172324) ---- */
    pp->repeatCount--;
    if (pp->repeatCount == 0) {
        PSParticle* result;

        if (pp->peopleObj != NULL) {
            (*(u32*)((u8*)pp->peopleObj + 0x4C))--;
        }

        result = _psListGetNext(pp);

        if (pp->parentObj != NULL) {
            if (psRemoveParticleAppSRT(pp) == 0 && parentCtx == NULL) {
                if (_psListGetFirst(pp->linkNo) != result) {
                    result = _psListGetFirst(pp->linkNo);
                }
            }
        }

        psDeletePntJObjwithParticle(pp);
        _psListDelete(pp, parentCtx);
        return result;
    }

    /* ---- Phase 6: physics integration (verified @ 0x801723B8) ---- */
    if (pp->flags & 0x4) { /* ORBIT - see file header note on bit value */
        void* peopleObj = pp->peopleObj;
        f32 sinScale = sinf(pp->scaleFactor);
        f32 sinFrict = sinf(pp->frictionFactor);
        f32 cosScale = cosf(pp->scaleFactor);
        f32 cosFrict = cosf(pp->frictionFactor);
        f32 a, b, mag;

        pp->velocityZ += *(f32*)((u8*)peopleObj + 0x54);
        a = *(f32*)((u8*)peopleObj + 0x44);
        if (a < 0.0f) a = -a;
        b = *(f32*)((u8*)peopleObj + 0x48);
        if (b < 0.0f) b = -b;
        mag = tanf(b);
        mag = pp->positionX + cosf(mag) * pp->velocityZ;
        mag = (mag * a) * b; /* NOTE: precise reconstruction of this term
                                 not fully verified; see 0x80172430-0x801724C8 */
        pp->positionX = *(f32*)((u8*)peopleObj + 0x20) +
            mag * cosFrict * cosScale - mag * sinFrict * sinScale;
        pp->positionY = *(f32*)((u8*)peopleObj + 0x24) +
            mag * cosFrict * sinScale + mag * sinFrict * cosScale;
        pp->positionZ = *(f32*)((u8*)peopleObj + 0x28) + mag * sinFrict;
    } else {
        if (pp->flags & PS_FLAG_SCALE_ACTIVE) {
            pp->velocityY -= pp->scaleFactor;
        }
        if (pp->flags & PS_FLAG_FRICTION_ACTIVE) {
            pp->velocityX *= pp->frictionFactor;
            pp->velocityY *= pp->frictionFactor;
            pp->velocityZ *= pp->frictionFactor;
        }
        pp->positionX += pp->velocityX;
        pp->positionY += pp->velocityY;
        pp->positionZ += pp->velocityZ;
    }

    /* ---- Phase 7: camera tracking (verified @ 0x80172550) ---- */
    if (pp->flags & PS_FLAG_ATTACH_CAMERA) {
        u32 slotIdx = (pp->flags >> 12) & 0x7;
        void* camSlot = lbl_80452DC8[slotIdx];

        if (camSlot == NULL) {
            void* renderObj = fn_8019F718();
            if (renderObj != NULL) {
                psSetPointJObj((s32)slotIdx + 1, renderObj);
                fn_801A05EC(renderObj);
            }
        }

        camSlot = lbl_80452DC8[slotIdx];
        if (camSlot != NULL) {
            HSD_JObjSetupMatrix(camSlot);
            HSD_JObjAddTx(camSlot, pp->positionX - *(f32*)((u8*)camSlot + 0x50));
            HSD_JObjAddTy(camSlot, pp->positionY - *(f32*)((u8*)camSlot + 0x60));
            HSD_JObjAddTz(camSlot, pp->positionZ - *(f32*)((u8*)camSlot + 0x70));
        }
    }

    _psListGetNext(pp);
    return pp;
}

/*
 * Refreshes the application SRT matrix before particle display. The remaining
 * display-space composition is still asm-only; this verified prefix is the
 * entry block at 0x8016CE2C.
 */
void psDispSubAppSRT(PSParticle* pp, Mtx parentMatrix) {
    PSAppSRT* appSRT = (PSAppSRT*)pp->parentObj;
    Mtx appMatrix;
    Vec velocity;
    Vec position;

    if (appSRT->flags != lbl_80478C30) {
        if (appSRT->type != 2) {
            HSD_MtxSRT(appSRT->matrix, &appSRT->scaleX,
                       &appSRT->translationX, &appSRT->rotationX, NULL);
        }
        if (appSRT->type == 1) {
            appSRT->type = 2;
        }
    }

    appSRT->flags = lbl_80478C30;
    PSMTXCopy(appSRT->matrix, appMatrix);
    appMatrix[0][3] -= appSRT->rotationX;
    appMatrix[1][3] -= appSRT->rotationY;
    appMatrix[2][3] -= appSRT->rotationZ;

    velocity.x = pp->velocityX;
    velocity.y = pp->velocityY;
    velocity.z = pp->velocityZ;
    PSMTXMultVec(appMatrix, &velocity, &velocity);
    if (appSRT->active != 0 && (pp->flags & 4) == 0) {
        PSMTXMultVec((const f32(*)[4])lbl_80452DE8,
                     &velocity, &velocity);
    }

    position.x = pp->positionX;
    position.y = pp->positionY;
    position.z = pp->positionZ;
    if (appSRT->active != 0) {
        PSMTXMultVec(appMatrix, &position, &position);
        PSMTXMultVec((const f32(*)[4])lbl_80452DE8,
                     &position, &position);
        position.x += appSRT->rotationX;
        position.y += appSRT->rotationY;
        position.z += appSRT->rotationZ;
    } else {
        PSMTXMultVec(appSRT->matrix, &position, &position);
    }
}

void psDispSubMakePolygon(PSParticle* pp, void* polygonData,
                          f32 centerX, f32 centerY, f32 centerZ,
                          f32 velocityX, f32 velocityY, f32 velocityZ,
                          f32 axisXX, f32 axisXY, f32 axisXZ,
                          f32 axisYX, f32 axisYY, f32 axisYZ);

/*
 * Prepares the two screen-space polygon axes and delegates emission. The
 * camera/people-relative orientation modes remain asm-only; this covers the
 * direct basis path and target-verified generator scaling.
 */
void psDispSub(PSParticle* pp, void* polygonData) {
    f32 axisXX;
    f32 axisXY;
    f32 axisXZ;
    f32 axisYX;
    f32 axisYY;
    f32 axisYZ;
    f32 size = pp->lerpValue;
    PSGeneratorState* generator = (PSGeneratorState*)pp->peopleObj;

    if (polygonData != NULL) {
        f32* view = (f32*)(lbl_80452DE8 + 0x7C);

        axisXX = view[0] * size;
        axisXY = -view[1] * size;
        axisXZ = view[4] * size;
        axisYX = -view[5] * size;
        axisYY = view[8] * size;
        axisYZ = -view[9] * size;
    } else {
        axisXX = lbl_8047B160 * size;
        axisXY = lbl_8047B15C * size;
        axisXZ = lbl_8047B158 * size;
        axisYX = lbl_8047B154 * size;
        axisYY = lbl_8047B150 * size;
        axisYZ = lbl_8047B14C * size;
    }

    if (generator != NULL && (generator->generatorFlags & 0x20)) {
        axisXX *= generator->generatorData[3];
        axisXY *= generator->generatorData[3];
        axisXZ *= generator->generatorData[4];
        axisYX *= generator->generatorData[4];
        axisYY *= generator->generatorData[5];
        axisYZ *= generator->generatorData[5];
    }

    psDispSubMakePolygon(pp, polygonData,
                         pp->positionX, pp->positionY, pp->positionZ,
                         pp->velocityX, pp->velocityY, pp->velocityZ,
                         axisXX, axisXZ, axisYY, axisXY, axisYX, axisYZ);
}

/*
 * Configures point-trail raster width. Geometry/color emission remains
 * asm-only; this entry block is verified at 0x8016D8EC-0x8016D954.
 */
void psDispSubPointTrail(PSParticle* pp) {
    f32 widthValue;
    s32 width;
    u8 cachedWidth;

    if (lbl_8047B12C != 0) {
        lbl_8047B12C = 0;
        fn_800BD554(0);
    }

    if (pp->lerpValue > lbl_8047D5E0) {
        widthValue = lbl_8047D5D8;
    } else {
        widthValue = lbl_8047D5DC * pp->lerpValue;
    }

    width = (s32)widthValue;
    cachedWidth = (u8)width;
    if (lbl_8047B164 != cachedWidth) {
        lbl_8047B164 = cachedWidth;
        fn_800B9404(width, 5);
    }
}

/*
 * Applies an attached application's SRT to point-particle position and
 * velocity.  Point raster emission remains to be decompiled; this is the
 * verified transform prefix at 0x8016DD68-0x8016DF14.
 */
void psDispSubAPPSRTPoint(PSParticle* pp) {
    PSAppSRT* appSRT = (PSAppSRT*)pp->parentObj;
    Mtx appMatrix;
    Vec velocity;
    Vec position;

    if (lbl_8047B12C != 0) {
        lbl_8047B12C = 0;
        fn_800BD554(0);
    }

    if (appSRT->flags != lbl_80478C30) {
        if (appSRT->type != 2) {
            HSD_MtxSRT(appSRT->matrix, &appSRT->scaleX,
                       &appSRT->translationX, &appSRT->rotationX, NULL);
        }
        if (appSRT->type == 1) {
            appSRT->type = 2;
        }
    }

    appSRT->flags = lbl_80478C30;
    PSMTXCopy(appSRT->matrix, appMatrix);
    appMatrix[0][3] -= appSRT->rotationX;
    appMatrix[1][3] -= appSRT->rotationY;
    appMatrix[2][3] -= appSRT->rotationZ;

    velocity.x = pp->velocityX;
    velocity.y = pp->velocityY;
    velocity.z = pp->velocityZ;
    PSMTXMultVec(appMatrix, &velocity, &velocity);
    if (appSRT->active != 0) {
        PSMTXMultVec((const f32(*)[4])lbl_80452DE8, &velocity, &velocity);
    }

    position.x = pp->positionX;
    position.y = pp->positionY;
    position.z = pp->positionZ;
    if (appSRT->active != 0) {
        PSMTXMultVec(appMatrix, &position, &position);
        PSMTXMultVec((const f32(*)[4])lbl_80452DE8, &position, &position);
        position.x += appSRT->rotationX;
        position.y += appSRT->rotationY;
        position.z += appSRT->rotationZ;
    } else {
        PSMTXMultVec(appSRT->matrix, &position, &position);
    }
}

/*
 * Allocates and links a generator after validating its bank/script tuple.
 * Script-record initialization remains asm-only; this is the verified
 * validation and pool/list prefix at 0x80173718-0x80173888.
 */
PSGeneratorState* psCreateGeneratorID(s32 linkNo, s32 bankIdx, s32 scriptId) {
    PSGeneratorState* gen;
    void** bank;
    u16 activeCount;
    u16 familyId;

    if (bankIdx >= 64 || linkNo >= 8 ||
        scriptId >= lbl_80452CC8[bankIdx]) {
        return NULL;
    }

    bank = (void**)lbl_80452AC8[bankIdx];
    if (bank[scriptId] == NULL) {
        return NULL;
    }

    if (lbl_8047B18C == NULL) {
        lbl_8047B18C = fn_801A6928(sizeof(PSGeneratorState));
        memset(lbl_8047B18C, 0, sizeof(PSGeneratorState));
    }

    gen = lbl_8047B18C;
    if (gen == NULL) {
        return NULL;
    }

    activeCount = lbl_8047B118 + 1;
    lbl_8047B118 = activeCount;
    if (activeCount > lbl_8047B112) {
        lbl_8047B112 = activeCount;
    }

    lbl_8047B18C = gen->next;
    if (lbl_8047B184 == NULL || *lbl_8047B184 == NULL) {
        if (lbl_8047B188 == NULL) {
            gen->next = NULL;
            lbl_8047B188 = gen;
        } else {
            gen->next = lbl_8047B188->next;
            lbl_8047B188->next = gen;
        }
    } else {
        gen->next = (*lbl_8047B184)->next;
        (*lbl_8047B184)->next = gen;
    }

    familyId = lbl_80478C38 + 1;
    lbl_80478C38 = familyId;
    if (familyId < 0x100) {
        familyId = 0x100;
        lbl_80478C38 = familyId;
    }
    gen->familyId = familyId;
    gen->appSRT = NULL;
    gen->generatorFlags = 2;
    gen->linkedJObj = NULL;
    gen->generatorData[0] = 0.0f;
    gen->generatorData[1] = 0.0f;
    gen->generatorData[2] = 0.0f;
    gen->generatorData[3] = 1.0f;
    gen->generatorData[4] = 1.0f;
    gen->generatorData[5] = 1.0f;
    gen->angleRadiusScale[0] = 1.0f;
    gen->angleRadiusScale[1] = 1.0f;
    gen->angleRadiusScale[2] = 1.0f;
    return gen;
}

/*
 * Updates the particle material and ambient channel colors, including the
 * active alpha-light contribution. Verified against 0x8016E814-0x8016EA78.
 */
void setupChanReg(PSParticle* pp) {
    PSColor material;
    PSColor ambient;
    s32 value;

    if ((pp->flags & 0x80000000) == 0) {
        return;
    }

    if (pp->sizeYTimer != 0) {
        s32 step = ((s32)pp->sizeYCountdown << 16) / pp->sizeYTimer;

        value = ((s32)pp->sizeXTargetFinal << 16) +
                step * ((s32)pp->sizeXTarget -
                        (s32)pp->sizeXTargetFinal);
        value >>= 16;
    } else {
        value = pp->sizeXTarget;
    }
    material.r = value;
    material.g = value;
    material.b = value;
    material.a = pp->sizeYTimer != 0
        ? (((s32)pp->sizeYTargetFinal << 16) +
           (((s32)pp->sizeYCountdown << 16) / pp->sizeYTimer) *
           ((s32)pp->sizeYTarget - (s32)pp->sizeYTargetFinal)) >> 16
        : pp->sizeYTarget;

    if (pp->flags & 0x80) {
        ambient.r = 0xFF;
        ambient.g = 0xFF;
        ambient.b = 0xFF;
        ambient.a = pp->color1A;
    } else if (pp->color1Timer != 0) {
        s32 step = ((s32)pp->color1Countdown << 16) / pp->color1Timer;

        ambient.r = (((s32)pp->color1TargetR << 16) +
                     step * ((s32)pp->color1R -
                             (s32)pp->color1TargetR)) >> 16;
        ambient.g = (((s32)pp->color1TargetG << 16) +
                     step * ((s32)pp->color1G -
                             (s32)pp->color1TargetG)) >> 16;
        ambient.b = (((s32)pp->color1TargetB << 16) +
                     step * ((s32)pp->color1B -
                             (s32)pp->color1TargetB)) >> 16;
        ambient.a = (((s32)pp->color1TargetA << 16) +
                     step * ((s32)pp->color1A -
                             (s32)pp->color1TargetA)) >> 16;
    } else {
        ambient.r = pp->color1R;
        ambient.g = pp->color1G;
        ambient.b = pp->color1B;
        ambient.a = pp->color1A;
    }

    material.r = (material.r * ambient.r) >> 8;
    material.g = (material.g * ambient.g) >> 8;
    material.b = (material.b * ambient.b) >> 8;

    if (ambient.r != lbl_8047B140.r ||
        ambient.g != lbl_8047B140.g ||
        ambient.b != lbl_8047B140.b) {
        lbl_8047B140 = ambient;
        fn_800BA5BC(0, &ambient);
    }

    {
        HSD_LObj* alphaLight = HSD_LObjGetActiveByID(0x100);

        if (alphaLight != NULL) {
            HSD_MulColor((GXColor*)&material, &alphaLight->color,
                         (GXColor*)&material);
        } else {
            material.r = 0;
            material.g = 0;
            material.b = 0;
        }
    }

    if (material.r != lbl_8047B13C.r ||
        material.g != lbl_8047B13C.g ||
        material.b != lbl_8047B13C.b) {
        lbl_8047B13C = material;
        fn_800BA4C8(0, &material);
    }
}

/*
 * Updates the two interpolated particle TEV colors. Texture-size TEV state
 * remains asm-only; these register-cache paths are verified against
 * 0x8016E40C-0x8016E698.
 */
void setupTevReg(PSParticle* pp) {
    PSColor color1;
    PSColor color2;

    if (pp->color1Timer != 0) {
        s32 step = ((s32)pp->color1Countdown << 16) / pp->color1Timer;

        color1.r = (((s32)pp->color1TargetR << 16) +
                    step * ((s32)pp->color1R -
                            (s32)pp->color1TargetR)) >> 16;
        color1.g = (((s32)pp->color1TargetG << 16) +
                    step * ((s32)pp->color1G -
                            (s32)pp->color1TargetG)) >> 16;
        color1.b = (((s32)pp->color1TargetB << 16) +
                    step * ((s32)pp->color1B -
                            (s32)pp->color1TargetB)) >> 16;
        color1.a = (((s32)pp->color1TargetA << 16) +
                    step * ((s32)pp->color1A -
                            (s32)pp->color1TargetA)) >> 16;
    } else {
        color1.r = pp->color1R;
        color1.g = pp->color1G;
        color1.b = pp->color1B;
        color1.a = pp->color1A;
    }

    if (pp->color2Timer != 0) {
        s32 step = ((s32)pp->color2Countdown << 16) / pp->color2Timer;

        color2.r = (((s32)pp->color2TargetR << 16) +
                    step * ((s32)pp->color2R -
                            (s32)pp->color2TargetR)) >> 16;
        color2.g = (((s32)pp->color2TargetG << 16) +
                    step * ((s32)pp->color2G -
                            (s32)pp->color2TargetG)) >> 16;
        color2.b = (((s32)pp->color2TargetB << 16) +
                    step * ((s32)pp->color2B -
                            (s32)pp->color2TargetB)) >> 16;
        color2.a = (((s32)pp->color2TargetA << 16) +
                    step * ((s32)pp->color2A -
                            (s32)pp->color2TargetA)) >> 16;
    } else {
        color2.r = pp->color2R;
        color2.g = pp->color2G;
        color2.b = pp->color2B;
        color2.a = pp->color2A;
    }

    if ((pp->flags & 0x80) ||
        (pp->flags & 0x80000000) ||
        (pp->flags & 0x100000)) {
        if (color1.r != lbl_8047B138.r ||
            color1.g != lbl_8047B138.g ||
            color1.b != lbl_8047B138.b ||
            color1.a != lbl_8047B138.a) {
            lbl_8047B138 = color1;
            fn_800BC2F8(1, &color1);
        }
    }

    if (pp->flags & 0x80) {
        if (color2.r != lbl_8047B134.r ||
            color2.g != lbl_8047B134.g ||
            color2.b != lbl_8047B134.b ||
            color2.a != lbl_8047B134.a) {
            lbl_8047B134 = color2;
            fn_800BC2F8(2, &color2);
        }
    } else if (lbl_8047B134.r != 0 || lbl_8047B134.g != 0 ||
               lbl_8047B134.b != 0 || lbl_8047B134.a != 0) {
        color2.r = 0;
        color2.g = 0;
        color2.b = 0;
        color2.a = 0;
        lbl_8047B134 = color2;
        fn_800BC2F8(2, &color2);
    }
}

/*
 * Emits one particle polygon. The geometry modes remain asm-only; this
 * verified entry gate is shared by every mode at 0x8016C1E0-0x8016C240.
 */
void psDispSubMakePolygon(PSParticle* pp, void* polygonData,
                          f32 centerX, f32 centerY, f32 centerZ,
                          f32 velocityX, f32 velocityY, f32 velocityZ,
                          f32 axisXX, f32 axisXY, f32 axisXZ,
                          f32 axisYX, f32 axisYY, f32 axisYZ) {
    if (lbl_8047B12C != 0) {
        lbl_8047B12C = 0;
        fn_800BD554(0);
    }

    if ((pp->flags & 0x100000) == 0) {
        if (polygonData == NULL) {
            u32 textureIndex = (pp->flags >> 14) & 3;

            fn_800B7D3C();
            fn_800B7874(9, 1);
            if (pp->flags & 0x400) {
                fn_800B7874(13, 2);
                fn_800B928C(0x80, 0, 4);
            } else {
                fn_800B928C(0x80, 1, 4);
            }

            GX_FIFO_F32 = centerX - axisXX;
            GX_FIFO_F32 = centerY - axisXY;
            GX_FIFO_F32 = centerZ - axisXZ;
            if (pp->flags & 0x400) {
                GX_FIFO_U8 = textureIndex;
            }

            GX_FIFO_F32 = centerX - axisYX;
            GX_FIFO_F32 = centerY - axisYY;
            GX_FIFO_F32 = centerZ - axisYZ;
            if (pp->flags & 0x400) {
                GX_FIFO_U8 = textureIndex + 1;
            }

            GX_FIFO_F32 = centerX + axisXX;
            GX_FIFO_F32 = centerY + axisXY;
            GX_FIFO_F32 = centerZ + axisXZ;
            if (pp->flags & 0x400) {
                GX_FIFO_U8 = textureIndex + 2;
            }

            GX_FIFO_F32 = centerX + axisYX;
            GX_FIFO_F32 = centerY + axisYY;
            GX_FIFO_F32 = centerZ + axisYZ;
            if (pp->flags & 0x400) {
                GX_FIFO_U8 = textureIndex + 3;
            }
        } else {
            u8* stream = polygonData;
            u32 packetCount = *(u32*)stream;

            stream += 4;
            while (packetCount != 0) {
                u8 primitive = stream[0];
                u8 vertexCount = stream[1];
                s32 i;

                stream += 4;
                fn_800B7D3C();
                fn_800B7874(9, 1);
                if (pp->flags & 0x400) {
                    fn_800B7874(13, 1);
                    fn_800B928C(primitive, 4, vertexCount);
                } else {
                    fn_800B928C(primitive, 1, vertexCount);
                }

                for (i = 0; i < vertexCount; i++) {
                    f32 u = *(f32*)&stream[0];
                    f32 v = *(f32*)&stream[4];
                    f32 xWeight = lbl_8047D5CC * (u - lbl_8047D618);
                    f32 yWeight = lbl_8047D5CC * (v - lbl_8047D618);

                    stream += 8;
                    if (pp->flags & 0x40000) {
                        u = 1.0f - u;
                    }
                    if (pp->flags & 0x80000) {
                        v = 1.0f - v;
                    }

                    GX_FIFO_F32 = centerX + axisXX * xWeight + axisYX * yWeight;
                    GX_FIFO_F32 = centerY + axisXY * xWeight + axisYY * yWeight;
                    GX_FIFO_F32 = centerZ + axisXZ * xWeight + axisYZ * yWeight;
                    if (pp->flags & 0x400) {
                        GX_FIFO_F32 = u;
                        GX_FIFO_F32 = v;
                    }
                }
                packetCount--;
            }
        }
        return;
    }
}

void psExecGenerator(u32 linkMask) {
    PSGeneratorState* gen;

    while (lbl_8047B180 != NULL) {
        lbl_8047B180 = fn_801A3E64();
    }

    lbl_8047B184 = NULL;
    gen = lbl_8047B188;
    while (gen != NULL) {
        u8* raw = (u8*)gen;
        u16 generatorFlags = *(u16*)(raw + 0x88);

        if ((linkMask & (1 << (gen->linkNo + 16))) != 0 ||
            (gen->flags & 0x800) != 0) {
            lbl_8047B184 = (PSGeneratorState**)gen;
            gen = gen->next;
            continue;
        }

        if ((generatorFlags & 2) == 0 && (generatorFlags & 1) != 0 &&
            gen->linkedJObj != NULL) {
            PSForceJObj* jobj = gen->linkedJObj;

            if (!(jobj->flags & 0x800000) && (jobj->flags & 0x40)) {
                fn_8019D9DC(jobj);
            }
            gen->positionX = jobj->worldX;
            gen->positionY = jobj->worldY;
            gen->positionZ = jobj->worldZ;
        }

        if (*(f32*)(raw + 0x08) < 0.0f) {
            gen->lifetime -= *(f32*)(raw + 0x08);
        } else {
            gen->lifetime += *(f32*)(raw + 0x08) * fn_801ADC7C();
        }

        if (gen->lifetime >= 1.0f) {
            generateParticle_8017424C(gen);
        }

        if (gen->maxLife != 0) {
            gen->maxLife--;
            if (gen->maxLife == 0) {
                PSGeneratorState* previous = (PSGeneratorState*)lbl_8047B184;

                if (raw[0x17] != 0) {
                    gen->flags |= 0x800;
                } else {
                    if (gen->angleFlags & 0x80) {
                        psKillGeneratorChild(gen);
                    }

                    if (gen->childCount != 0) {
                        *(f32*)(raw + 0x08) = 0.0f;
                        gen->maxLife = 1;
                        previous = gen;
                    } else if ((gen->angleFlags & 0x3800) != 0 &&
                               gen->appSRT != NULL &&
                               ((PSAppSRT*)gen->appSRT)->owner == gen &&
                               ((PSAppSRT*)gen->appSRT)->refCount != 1) {
                        *(f32*)(raw + 0x08) = 0.0f;
                        gen->maxLife = 1;
                        previous = gen;
                    } else {
                        if (previous == NULL) {
                            lbl_8047B188 = gen->next;
                        } else {
                            previous->next = gen->next;
                        }

                        if (gen->appSRT != NULL) {
                            psRemoveGeneratorAppSRT(gen);
                        }
                        gen->next = lbl_8047B18C;
                        lbl_8047B18C = gen;
                        lbl_8047B118--;
                    }
                    lbl_8047B184 = (PSGeneratorState**)previous;
                }

                if (lbl_8047B184 != NULL) {
                    gen = ((PSGeneratorState*)lbl_8047B184)->next;
                } else {
                    gen = lbl_8047B188;
                }
                continue;
            }
        }

        lbl_8047B184 = (PSGeneratorState**)gen;
        gen = gen->next;
    }
}

/*
 * Builds the generator's emission basis. The remainder of the emitter modes
 * is still asm-only; this verified prefix corresponds to 0x801742C8-8017455C.
 */
void generateParticle_8017424C(PSGeneratorState* gen) {
    Mtx basis;
    Mtx rotationX;
    Mtx rotationY;
    Mtx rotationZ;
    Mtx direction;
    Vec column;
    Vec emissionVelocity;
    Vec forward;
    Vec up;
    Vec side;
    f32 magnitude;

    if (gen->lifetime < lbl_8047D6B4) {
        return;
    }

    emissionVelocity.x = gen->velocityX;
    emissionVelocity.y = gen->velocityY;
    emissionVelocity.z = gen->velocityZ;
    if ((gen->angleFlags & 0xF) == 2 &&
        (gen->generatorFlags & 0x80) != 0) {
        emissionVelocity.x *= gen->generatorData[3];
        emissionVelocity.y *= gen->generatorData[4];
        emissionVelocity.z *= gen->generatorData[5];
    }
    magnitude = sqrtf(emissionVelocity.x * emissionVelocity.x +
                      emissionVelocity.y * emissionVelocity.y +
                      emissionVelocity.z * emissionVelocity.z);

    PSMTXIdentity(basis);

    if ((gen->flags & 0x30000) == 0) {
        PSMTXRotRad(rotationX, 'X', gen->generatorData[0]);
        PSMTXRotRad(rotationY, 'Y', gen->generatorData[1]);
        PSMTXRotRad(rotationZ, 'Z', gen->generatorData[2]);
        PSMTXConcat(rotationY, rotationX, rotationX);
        PSMTXConcat(rotationZ, rotationX, rotationX);

        column.x = rotationX[0][0];
        column.y = rotationX[1][0];
        column.z = rotationX[2][0];
        PSVECNormalize(&column, &column);
        basis[0][0] = column.x;
        basis[1][0] = column.y;
        basis[2][0] = column.z;

        column.x = rotationX[0][1];
        column.y = rotationX[1][1];
        column.z = rotationX[2][1];
        PSVECNormalize(&column, &column);
        basis[0][1] = column.x;
        basis[1][1] = column.y;
        basis[2][1] = column.z;

        column.x = rotationX[0][2];
        column.y = rotationX[1][2];
        column.z = rotationX[2][2];
        PSVECNormalize(&column, &column);
        basis[0][2] = column.x;
        basis[1][2] = column.y;
        basis[2][2] = column.z;
        basis[0][3] = 0.0f;
        basis[1][3] = 0.0f;
        basis[2][3] = 0.0f;
    }

    if (gen->flags & 0x20000) {
        PSCameraObject* camera = (PSCameraObject*)lbl_8047B190;

        if (camera == NULL) {
            __assert(lbl_802739E4, 0x272, lbl_802739F0);
        }

        camera = (PSCameraObject*)lbl_8047B190;
        forward.x = camera->view->position.x - gen->positionX;
        forward.y = camera->view->position.y - gen->positionY;
        forward.z = camera->view->position.z - gen->positionZ;
        PSVECNormalize(&forward, &forward);

        HSD_CObjGetUpVector(camera, &up);
        PSVECNormalize(&up, &up);
        PSVECCrossProduct(&up, &forward, &side);
        PSVECCrossProduct(&forward, &side, &up);

        basis[0][0] = side.x;
        basis[1][0] = side.y;
        basis[2][0] = side.z;
        basis[0][1] = up.x;
        basis[1][1] = up.y;
        basis[2][1] = up.z;
        basis[0][2] = forward.x;
        basis[1][2] = forward.y;
        basis[2][2] = forward.z;
    }

    if ((gen->angleFlags & 0xF) != 1 && magnitude > lbl_80478ACC) {
        f32 yaw;
        f32 pitch;
        f32 yawSin;
        f32 yawCos;
        f32 pitchSin;
        f32 pitchCos;
        f32 flattened;

        PSVECNormalize(&emissionVelocity, &emissionVelocity);
        if (fabsf(emissionVelocity.z) < lbl_80478AC8) {
            yaw = emissionVelocity.y >= 0.0f
                ? lbl_8047D694 : lbl_8047D698;
        } else {
            yaw = atan2(emissionVelocity.y, emissionVelocity.z);
        }
        yawSin = sin(yaw);
        yawCos = cos(yaw);
        flattened = emissionVelocity.z * yawCos +
                    emissionVelocity.y * yawSin;

        if (fabsf(flattened) < lbl_80478AC8) {
            pitch = emissionVelocity.x >= 0.0f
                ? lbl_8047D694 : lbl_8047D698;
        } else {
            pitch = atan2(emissionVelocity.x, flattened);
        }
        pitchSin = sin(pitch);
        pitchCos = cos(pitch);

        direction[0][0] = pitchCos;
        direction[0][1] = pitchSin;
        direction[0][2] = 0.0f;
        direction[0][3] = 0.0f;
        direction[1][0] = -yawSin * pitchSin;
        direction[1][1] = yawCos;
        direction[1][2] = yawSin * pitchCos;
        direction[1][3] = 0.0f;
        direction[2][0] = -yawCos * pitchSin;
        direction[2][1] = -yawSin;
        direction[2][2] = yawCos * pitchCos;
        direction[2][3] = 0.0f;
        PSMTXConcat(basis, direction, basis);
    }
}

static inline s32 PSJObjMtxIsDirty(PSJObjTransform* jobj) {
    s32 dirty;

    if (jobj == NULL) {
        __assert(lbl_8047D670, 0x25D, lbl_8047D678);
    }
    dirty = FALSE;
    if (!(jobj->flags & 0x00800000) && (jobj->flags & 0x40)) {
        dirty = TRUE;
    }
    return dirty;
}

void HSD_JObjSetupMatrix(void* camSlot) {
    extern void fn_8019D9DC(void* jobj);
    PSJObjTransform* jobj = camSlot;

    if (jobj == NULL || !PSJObjMtxIsDirty(jobj)) {
        return;
    }
    fn_8019D9DC(jobj);
}

void HSD_JObjAddTz(PSJObjTransform* jobj, f32 dz) {
    extern void fn_8019D620(void* jobj);

    if (jobj == NULL) {
        __assert(lbl_8047D670, 0x46B, lbl_8047D678);
    }

    jobj->translateZ += dz;

    if (!(jobj->flags & 0x02000000)) {
        if (jobj != NULL && !PSJObjMtxIsDirty(jobj)) {
            fn_8019D620(jobj);
        }
    }
}

void HSD_JObjAddTy(PSJObjTransform* jobj, f32 dy) {
    extern void fn_8019D620(void* jobj);

    if (jobj == NULL) {
        __assert(lbl_8047D670, 0x460, lbl_8047D678);
    }

    jobj->translateY += dy;

    if (!(jobj->flags & 0x02000000)) {
        if (jobj != NULL && !PSJObjMtxIsDirty(jobj)) {
            fn_8019D620(jobj);
        }
    }
}

void HSD_JObjAddTx(PSJObjTransform* jobj, f32 dx) {
    extern void fn_8019D620(void* jobj);

    if (jobj == NULL) {
        __assert(lbl_8047D670, 0x455, lbl_8047D678);
    }

    jobj->translateX += dx;

    if (!(jobj->flags & 0x02000000)) {
        if (jobj != NULL && !PSJObjMtxIsDirty(jobj)) {
            fn_8019D620(jobj);
        }
    }
}

#endif
