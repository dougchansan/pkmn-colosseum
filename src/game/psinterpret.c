/**
 * @file psinterpret.c
 * @brief HAL's particle command interpreter (sysdolphin psinterpret.c) in the
 *        Genius Sonority fork, 0x8016F430 - 0x80173624.
 *
 * The whole translation unit:
 *   .text   0x8016F430 - 0x80173624  psdisptev.c ends with psSetupTevCommon
 *           at 0x8016F430; generator.c starts with psSetBillboardCamera at
 *           0x80173624
 *   .rodata 0x802739A0 - 0x802739B0  __FILE__ "psinterpret.c"; psdisp.c's
 *           pool ends at 0x802739A0 and generator.c's starts at 0x802739B0
 *   .data   0x8036BFE0 - 0x8036C1E0  psInterpretParticle0's command jump
 *           table (0x80 - 0xFF); generator.c's tables start at 0x8036C1E0
 *   .sbss   0x8047B178 - 0x8047B180  getFloat's byte-assembly scratch
 *   .sdata2 0x8047D628 - 0x8047D6B0  "lastPP", the float literal pool,
 *           "jobj.h" and "jobj"; generator.c's pool starts at 0x8047D6B0
 *
 * Melee has no psinterpret.c source (its interpreter is in the asm); the
 * particle and generator layouts come from include/sysdolphin/baselib/
 * psstructs.h and the command semantics from the retail code.
 *
 * Compiler: the particle library flags (pslist.c, particle.c, psdisp.c,
 * psdisptev.c, generator.c): GC/1.3.2 -O4,p -inline auto,deferred
 * -use_lmw_stmw on -sdata 8 -sdata2 8 -str reuse,readonly, unit-wide, no
 * local pragmas. Deferred inlining emits the unit in reverse definition
 * order, so the functions are written from getFloat (highest address) to
 * psInterpretParticles (lowest). psInterpretParticle0 is too large for the
 * auto-inliner's budget: every helper it calls stays a call, and the
 * header inlines it calls (U8ClampAdd and jobj.h's HSD_JObjSetupMatrix /
 * HSD_JObjAddTx/Ty/Tz) are emitted out of line right after it, in reverse
 * order of first use.
 *
 * Status: linked as one object (every function, the .rodata __FILE__,
 * psInterpretParticle0's jump table, getFloat's .sbss scratch and the
 * .sdata2 pool). psInterpretParticle0 calls sinf, cosf, tanf and sqrtf out
 * of line; their weak copies are dead-stripped in favour of MSL's (see
 * crt/math_ppc.h), and U8ClampAdd, the TU's own inline, stays in place.
 * modifyDir and modifyDirGenBase reuse vx/vy/vz for the new local-frame
 * direction before rotating it back, as retail's allocation shows.
 *
 * psInterpretParticle0's register allocation (2026-09-29): the spawned
 * particle is a block-scoped local of each spawning command, and the
 * generator commands 0xEF/0xF0 read their kind byte into op, the dispatch
 * byte (dead once the switch has dispatched). With a function-scope child
 * or a separate kind-byte local, the command byte, spawned particle and
 * kind byte take r26/r27 the other way round from retail.
 *
 * SET_POSITION/ADD_POSITION/SET_VELOCITY/ADD_VELOCITY fill only the
 * components their opcode's low bits name; the others are read
 * uninitialised. Retail does the same (no store to those stack slots).
 */
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "crt/float.h"
#include "crt/math_ppc.h"
#include "sysdolphin/baselib/jobj.h"
#include "sysdolphin/baselib/psstructs.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923

extern f32 HSD_Randf(void);          /* HSD_Randf */
extern HSD_JObj* HSD_JObjAlloc(void);    /* HSD_JObjAlloc */
extern void HSD_JObjUnref(HSD_JObj* jobj); /* HSD_JObjUnref */
extern void HSD_MtxSRT(Mtx m, Vec* scale, Vec* rot, Vec* trans, Vec* scale2);
extern void PSMTXIdentity(Mtx m);
extern void PSMTXScale(Mtx m, f32 x, f32 y, f32 z);
extern void PSMTXRotRad(Mtx m, char axis, f32 rad);
extern void PSMTXConcat(const Mtx a, const Mtx b, Mtx ab);

extern HSD_Particle* _psListGetFirst(s32 linkNo);
extern void _psListDelete(HSD_Particle* pp, HSD_Particle* prev);
extern HSD_Particle* psGenerateParticleID0(HSD_Particle* p, s32 linkNo,
                                           s32 bank, s32 id, s32 flgInterpret);
extern void psSetPointJObj(s32 no, HSD_JObj* jobj);
extern void psDeletePntJObjwithParticle(HSD_Particle* pp);
extern void psSetGeneratorAngleRadiusScale(HSD_Generator* gp, f32* scale,
                                           u8 motion);
extern HSD_Generator* psCreateGeneratorID(s32 linkNo, s32 bank, s32 idx);
extern void genPosUpdate(HSD_Generator* gp);
extern s32 psRemoveParticleAppSRT(HSD_Particle* pp);
extern s32 psAttachParticleAppSRT(HSD_Particle* pp, HSD_psAppSRT* appsrt);
extern s32 psChangeParticleAppSRT(HSD_Particle* pp, HSD_psAppSRT* appsrt);
extern s32 psAttachGeneratorAppSRT(HSD_Generator* gp, HSD_psAppSRT* appsrt);
extern s32 psChangeGeneratorAppSRT(HSD_Generator* gp, HSD_psAppSRT* appsrt);

extern u32* lbl_804527C8[PS_NUM_BANK];             /* bank references */
extern HSD_PSTexGroup** lbl_804529C8[PS_NUM_BANK]; /* texture groups */
extern HSD_JObj* lbl_80452DC8[8];                  /* point JObjs */

#define psBankRef lbl_804527C8
#define psTexGroupArray lbl_804529C8
#define psPointJObj lbl_80452DC8

typedef union {
    u8 c[4];
    f32 f;
} PSFloatBytes;

PSFloatBytes lbl_8047B178; /* getFloat's byte-assembly scratch (.sbss) */

HSD_Particle* psInterpretParticle0(HSD_Particle* pp, HSD_Particle* prev);

/* Defined in reverse address order (see the file header). */

u8* getFloat(u8* cmdList, f32* val)
{
    lbl_8047B178.c[0] = *cmdList++;
    lbl_8047B178.c[1] = *cmdList++;
    lbl_8047B178.c[2] = *cmdList++;
    lbl_8047B178.c[3] = *cmdList++;
    *val = lbl_8047B178.f;
    return cmdList;
}

u8* getTime(u8* cmdList, u16* val)
{
    *val = *cmdList++;
    if (*val & 0x80) {
        *val = ((*val & 0x7F) << 8) + *cmdList++;
    }
    return cmdList;
}

/* Turns the velocity by a random direction on a cone of half-angle `angle`
 * around the generator velocity plus an offset. */
void modifyDirGenBase(HSD_Particle* pp, f32 angle, f32 x, f32 y, f32 z)
{
    f32 vx;
    f32 vy;
    f32 vz;
    f32 sx;
    f32 cx;
    f32 sy;
    f32 cy;
    f32 len;
    f32 rnd;
    f32 r;
    f32 ry;
    f32 rx;
    f32 w;

    vx = pp->gen->vel.x + x;
    vy = pp->gen->vel.y + y;
    vz = pp->gen->vel.z + z;
    if (fabs(vz) < FLT_MIN) {
        rx = vy >= 0.0f ? (f32) M_PI_2 : (f32) -M_PI_2;
    } else {
        rx = atan2f(vy, vz);
    }
    sx = sinf(rx);
    cx = cosf(rx);
    w = vy * sx + vz * cx;
    if (fabs(w) < FLT_MIN) {
        ry = vx >= 0.0f ? (f32) M_PI_2 : (f32) -M_PI_2;
    } else {
        ry = atan2f(vx, w);
    }
    sy = sinf(ry);
    cy = cosf(ry);
    len = sqrtf(pp->vel.x * pp->vel.x + pp->vel.y * pp->vel.y +
                pp->vel.z * pp->vel.z);
    rnd = 2.0 * (M_PI * HSD_Randf());
    r = len * sinf(angle);
    vx = r * cosf(rnd);
    vy = r * sinf(rnd);
    vz = len * cosf(angle);
    pp->vel.x = vx * cy + vz * sy;
    pp->vel.y = sy * (-vx * sx) + vy * cx + cy * (vz * sx);
    pp->vel.z = sy * (-vx * cx) - vy * sx + cy * (vz * cx);
}

/* Turns the velocity by a random direction on a cone of half-angle `angle`
 * around its current direction. */
void modifyDir(HSD_Particle* pp, f32 angle)
{
    f32 vx;
    f32 vy;
    f32 vz;
    f32 sx;
    f32 cx;
    f32 sy;
    f32 cy;
    f32 len;
    f32 rnd;
    f32 ry;
    f32 rx;
    f32 r;
    f32 w;

    vx = pp->vel.x;
    vy = pp->vel.y;
    vz = pp->vel.z;
    if (fabs(vz) < FLT_MIN) {
        rx = vy >= 0.0f ? (f32) M_PI_2 : (f32) -M_PI_2;
    } else {
        rx = atan2f(vy, vz);
    }
    sx = sinf(rx);
    cx = cosf(rx);
    w = vy * sx + vz * cx;
    if (fabs(w) < FLT_MIN) {
        ry = vx >= 0.0f ? (f32) M_PI_2 : (f32) -M_PI_2;
    } else {
        ry = atan2f(vx, w);
    }
    sy = sinf(ry);
    cy = cosf(ry);
    len = sqrtf(vx * vx + vy * vy + vz * vz);
    rnd = 2.0 * (M_PI * HSD_Randf());
    r = len * sinf(angle);
    vx = r * cosf(rnd);
    vy = r * sinf(rnd);
    vz = len * cosf(angle);
    pp->vel.x = vx * cy + vz * sy;
    pp->vel.y = sy * (-vx * sx) + vy * cx + cy * (vz * sx);
    pp->vel.z = sy * (-vx * cx) - vy * sx + cy * (vz * cx);
}

/* Aims the velocity at a JObj, keeping its speed. */
void setVelToJObj(HSD_Particle* pp, HSD_JObj* jobj)
{
    f32 dx;
    f32 dy;
    f32 dz;
    f32 speed;
    f32 len;

    if (jobj == NULL) {
        return;
    }
    HSD_JObjSetupMatrix(jobj);
    dx = jobj->mtx[0][3] - pp->pos.x;
    dy = jobj->mtx[1][3] - pp->pos.y;
    dz = jobj->mtx[2][3] - pp->pos.z;
    speed = sqrtf(pp->vel.x * pp->vel.x + pp->vel.y * pp->vel.y +
                  pp->vel.z * pp->vel.z);
    len = dx * dx + dy * dy + dz * dz;
    if (len == 0.0f) {
        return;
    }
    speed = speed / sqrtf(len);
    pp->vel.x = dx * speed;
    pp->vel.y = dy * speed;
    pp->vel.z = dz * speed;
}

/* Pulls the particle towards a JObj; returns whether it arrived. */
s32 applyForceJObj(HSD_Particle* pp, HSD_JObj* jobj, f32 force, f32 radius)
{
    f32 dx;
    f32 dy;
    f32 dz;
    f32 len;

    if (jobj == NULL || radius < 0.0f) {
        return 0;
    }
    HSD_JObjSetupMatrix(jobj);
    dx = jobj->mtx[0][3] - pp->pos.x;
    dy = jobj->mtx[1][3] - pp->pos.y;
    dz = jobj->mtx[2][3] - pp->pos.z;
    len = dx * dx + dy * dy + dz * dz;
    if (len <= radius * radius) {
        return 1;
    }
    if (0.0f == len) {
        return 0;
    }
    len = force / len;
    pp->vel.x += len * dx;
    pp->vel.y += len * dy;
    pp->vel.z += len * dz;
    return 0;
}

/* Rotates the particle's velocity by the generator's local rotation. */
void psApplyVelocityLocalRotation(HSD_Particle* pp)
{
    Mtx rot_x;
    Mtx rot_y;
    Mtx rot_z;
    Vec vel;

    if (pp->gen == NULL || !(pp->gen->posFlags & 4)) {
        return;
    }
    vel.x = pp->vel.x;
    vel.y = pp->vel.y;
    vel.z = pp->vel.z;
    PSMTXRotRad(rot_x, 'X', pp->gen->rot.x);
    PSMTXRotRad(rot_y, 'Y', pp->gen->rot.y);
    PSMTXRotRad(rot_z, 'Z', pp->gen->rot.z);
    PSMTXConcat(rot_y, rot_x, rot_x);
    PSMTXConcat(rot_z, rot_x, rot_x);
    PSMTXMultVec(rot_x, &vel, &vel);
    pp->vel.x = vel.x;
    pp->vel.y = vel.y;
    pp->vel.z = vel.z;
}

/* Transforms an offset by the generator's local rotation (and scale). */
void psApplyOffsetLocalRotation(HSD_Particle* pp, Vec* ofs)
{
    Mtx rot_x;
    Mtx rot_y;
    Mtx rot_z;
    Mtx scale;

    if (pp->gen == NULL || !(pp->gen->posFlags & 8)) {
        return;
    }
    if (pp->gen->posFlags & 0x10) {
        PSMTXIdentity(scale);
    } else {
        PSMTXScale(scale, pp->gen->scale.x, pp->gen->scale.y,
                   pp->gen->scale.z);
    }
    PSMTXRotRad(rot_x, 'X', pp->gen->rot.x);
    PSMTXRotRad(rot_y, 'Y', pp->gen->rot.y);
    PSMTXRotRad(rot_z, 'Z', pp->gen->rot.z);
    PSMTXConcat(rot_y, rot_x, rot_x);
    PSMTXConcat(rot_z, rot_x, rot_x);
    PSMTXConcat(scale, rot_x, rot_x);
    PSMTXMultVec(rot_x, ofs, ofs);
}

/* Copies the rotation/scale block, flags and JObj of a parent generator. */
void psCopyGeneratorData(HSD_Generator* gp, HSD_Generator* src)
{
    if (src == NULL || gp == NULL) {
        return;
    }
    if (src->kind & 0x20000000) {
        gp->kind |= 0x20000000;
    }
    gp->rot.x = src->rot.x;
    gp->rot.y = src->rot.y;
    gp->rot.z = src->rot.z;
    gp->scale.x = src->scale.x;
    gp->scale.y = src->scale.y;
    gp->scale.z = src->scale.z;
    gp->posFlags = src->posFlags;
    gp->posFlags &= ~2;
    gp->jobj = src->jobj;
    if (src->posFlags & 0x1000) {
        psSetGeneratorAngleRadiusScale(gp, &src->scale2.x, 1);
    } else {
        psSetGeneratorAngleRadiusScale(gp, &src->scale2.x, 0);
    }
}

HSD_Particle* _psListGetNext(HSD_Particle* pp)
{
    return pp->next;
}

void HSD_MTXSRT(Mtx m, Vec* scale, Vec* rot, Vec* trans, Vec* scale2)
{
    HSD_MtxSRT(m, scale, rot, trans, scale2);
}

/* Adds a random offset to a colour component, clamped to 0 - 255. */
inline u8 U8ClampAdd(u8 val, f32 add)
{
    f32 f = val + add;

    if (f < 0.0f) {
        f = 0.0f;
    }
    if (f > 255.0f) {
        f = 255.0f;
    }
    return f;
}

/* Freezes a running colour interpolation at its current value. */
#define PS_FREEZE_COLOR(col, target, remain, count)                          \
    if ((count) != 0) {                                                        \
        s32 t = ((remain) << 16) / (count);                                    \
        (col).r = (((target).r << 16) + t * ((col).r - (target).r)) >> 16;     \
        (col).g = (((target).g << 16) + t * ((col).g - (target).g)) >> 16;     \
        (col).b = (((target).b << 16) + t * ((col).b - (target).b)) >> 16;     \
        (col).a = (((target).a << 16) + t * ((col).a - (target).a)) >> 16;     \
    }

HSD_Particle* psInterpretParticle0(HSD_Particle* pp, HSD_Particle* prev)
{
    u8* cmdList;
    u8 cmd;
    u16 time;
    f32 val;
    f32 val2;
    HSD_Generator* gp;
    HSD_PSTexGroup* tg;
    s32 id;
    u8 op;

    if (pp->kind & 0x800) {
        return _psListGetNext(pp);
    }

    if (pp->sizeCount != 0) {
        pp->size += (pp->sizeTarget - pp->size) / pp->sizeCount;
        pp->sizeCount--;
    }
    if (pp->primColCount != 0) {
        pp->primColRemain--;
        if (pp->primColRemain == 0) {
            pp->primColCount = 0;
            pp->primCol.r = pp->primColTarget.r;
            pp->primCol.g = pp->primColTarget.g;
            pp->primCol.b = pp->primColTarget.b;
            pp->primCol.a = pp->primColTarget.a;
        }
    }
    if (pp->envColCount != 0) {
        pp->envColRemain--;
        if (pp->envColRemain == 0) {
            pp->envColCount = 0;
            pp->envCol.r = pp->envColTarget.r;
            pp->envCol.g = pp->envColTarget.g;
            pp->envCol.b = pp->envColTarget.b;
            pp->envCol.a = pp->envColTarget.a;
        }
    }
    if (pp->matColCount != 0) {
        pp->matColRemain--;
        if (pp->matColRemain == 0) {
            pp->matColCount = 0;
            pp->matRGB = pp->matRGBTarget;
            pp->matA = pp->matATarget;
        }
    }
    if (pp->ambColCount != 0) {
        pp->ambColRemain--;
        if (pp->ambColRemain == 0) {
            pp->ambColCount = 0;
            pp->ambRGB = pp->ambRGBTarget;
            pp->ambA = pp->ambATarget;
        }
    }
    if (pp->aCmpCount != 0) {
        pp->aCmpRemain--;
        if (pp->aCmpRemain == 0) {
            pp->aCmpCount = 0;
            pp->aCmpParam1 = pp->aCmpParam1Target;
            pp->aCmpParam2 = pp->aCmpParam2Target;
        }
    }
    if (pp->rotateCount != 0) {
        if (pp->x68) {
            pp->rotate += pp->rotateTarget;
            if (pp->rotateTarget >= 0.0f) {
                pp->rotateTarget += pp->x68;
            } else {
                pp->rotateTarget -= pp->x68;
            }
            if (--pp->rotateCount == 0) {
                pp->x68 = 0.0f;
                pp->rotateTarget = 0.0f;
            }
        } else {
            pp->rotate += (pp->rotateTarget - pp->rotate) / pp->rotateCount;
            pp->rotateCount--;
        }
    }

    if (pp->cmdWait != 0 && --pp->cmdWait == 0) {
        cmdList = pp->cmdList + pp->cmdPtr;
        do {
            cmd = *cmdList++;
            if (cmd < 0x80) {
                time = cmd & 0x1F;
                if (cmd & 0x20) {
                    time = (time << 8) + *cmdList++;
                }
                switch (cmd & 0xC0) {
                case 0x00:
                    break;
                case 0x40:
                    pp->poseNum = *cmdList++;
                    tg = psTexGroupArray[pp->bank][pp->texGroup];
                    if (tg != NULL && tg->texTable != NULL &&
                        tg->texTable[pp->poseNum] != NULL)
                    {
                        pp->kind |= 0x400;
                    }
                    break;
                }
            } else {
                time = 0;
                if ((op = cmd & 0xF8) <= 0x98) {
                } else if ((op = cmd & 0xF0) == 0xC0 || op == 0xD0) {
                } else {
                    op = cmd;
                }
                switch (op) {
                /* RULE-EXCEPTION(title-path): uninitialized reads - 0x80/0x88/0x90/0x98
                 * copy all three components although only the named ones were
                 * read, exactly as retail does - see docs/RULE_EXCEPTIONS.md */
                case 0x80: {
                    Vec pos;

                    if (cmd & 1) {
                        cmdList = getFloat(cmdList, &val);
                        pos.x = val;
                    }
                    if (cmd & 2) {
                        cmdList = getFloat(cmdList, &val);
                        pos.y = val;
                    }
                    if (cmd & 4) {
                        cmdList = getFloat(cmdList, &val);
                        pos.z = val;
                    }
                    psApplyOffsetLocalRotation(pp, &pos);
                    pp->pos.x = pos.x;
                    pp->pos.y = pos.y;
                    pp->pos.z = pos.z;
                    break;
                }
                case 0x88: {
                    Vec pos;

                    if (cmd & 1) {
                        cmdList = getFloat(cmdList, &val);
                        pos.x = val;
                    }
                    if (cmd & 2) {
                        cmdList = getFloat(cmdList, &val);
                        pos.y = val;
                    }
                    if (cmd & 4) {
                        cmdList = getFloat(cmdList, &val);
                        pos.z = val;
                    }
                    psApplyOffsetLocalRotation(pp, &pos);
                    pp->pos.x += pos.x;
                    pp->pos.y += pos.y;
                    pp->pos.z += pos.z;
                    break;
                }
                case 0x90: {
                    Vec vel;

                    if (cmd & 1) {
                        cmdList = getFloat(cmdList, &val);
                        vel.x = val;
                    }
                    if (cmd & 2) {
                        cmdList = getFloat(cmdList, &val);
                        vel.y = val;
                    }
                    if (cmd & 4) {
                        cmdList = getFloat(cmdList, &val);
                        vel.z = val;
                    }
                    psApplyOffsetLocalRotation(pp, &vel);
                    pp->vel.x = vel.x;
                    pp->vel.y = vel.y;
                    pp->vel.z = vel.z;
                    break;
                }
                case 0x98: {
                    Vec vel;

                    if (cmd & 1) {
                        cmdList = getFloat(cmdList, &val);
                        vel.x = val;
                    }
                    if (cmd & 2) {
                        cmdList = getFloat(cmdList, &val);
                        vel.y = val;
                    }
                    if (cmd & 4) {
                        cmdList = getFloat(cmdList, &val);
                        vel.z = val;
                    }
                    if (!(pp->kind & 4)) {
                        psApplyOffsetLocalRotation(pp, &vel);
                    } else if (pp->gen != NULL &&
                               (pp->gen->posFlags & 0x40))
                    {
                        f32 s = (pp->gen->scale.x + pp->gen->scale.y +
                                 pp->gen->scale.z) /
                                3.0f;
                        vel.x *= s;
                        vel.y *= s;
                        vel.z *= s;
                    }
                    pp->vel.x += vel.x;
                    pp->vel.y += vel.y;
                    pp->vel.z += vel.z;
                    break;
                }
                case 0xA0:
                    cmdList = getTime(cmdList, &pp->sizeCount);
                    cmdList = getFloat(cmdList, &pp->sizeTarget);
                    if (pp->sizeCount == 0) {
                        pp->size = pp->sizeTarget;
                    }
                    break;
                case 0xA1:
                    pp->kind &= ~0x400;
                    break;
                case 0xA2:
                    cmdList = getFloat(cmdList, &pp->grav);
                    if (0.0f == pp->grav) {
                        pp->kind &= ~1;
                    } else {
                        pp->kind |= 1;
                    }
                    if (pp->gen != NULL && (pp->gen->posFlags & 0x1000)) {
                        pp->grav *= (pp->gen->scale.x + pp->gen->scale.y +
                                     pp->gen->scale.z) /
                                    3.0f;
                    }
                    break;
                case 0xA3:
                    cmdList = getFloat(cmdList, &pp->fric);
                    if (1.0f == pp->fric) {
                        pp->kind &= ~2;
                    } else {
                        pp->kind |= 2;
                    }
                    if (pp->gen != NULL && (pp->gen->posFlags & 0x1000)) {
                        pp->fric *= (pp->gen->scale.x + pp->gen->scale.y +
                                     pp->gen->scale.z) /
                                    3.0f;
                    }
                    break;
                case 0xA4: {
                    HSD_Particle* child;

                    id = *cmdList++ << 8;
                    id += *cmdList++;
                    child = psGenerateParticleID0(pp, pp->linkNo, pp->bank,
                                                  id, 0);
                    if (child != NULL) {
                        child->idnum = pp->idnum;
                        child->gen = pp->gen;
                        if (pp->gen != NULL) {
                            pp->gen->numChild++;
                        }
                        if (pp->gen->kind & 0x20000000) {
                            child->kind |= 0x20000000;
                        }
                        psApplyVelocityLocalRotation(child);
                        if (pp->gen != NULL && (pp->gen->type & 0x2000)) {
                            psChangeParticleAppSRT(child, pp->appsrt);
                        } else {
                            psAttachParticleAppSRT(child, pp->appsrt);
                        }
                        child->pos.x = pp->pos.x;
                        child->pos.y = pp->pos.y;
                        child->pos.z = pp->pos.z;
                        psInterpretParticle0(child, pp);
                    }
                    break;
                }
                case 0xF1: {
                    HSD_Particle* child;

                    id = *cmdList++ << 8;
                    id += *cmdList++;
                    if (psBankRef[pp->bank] != NULL) {
                        id = psBankRef[pp->bank][id];
                    }
                    child = psGenerateParticleID0(pp, pp->linkNo, pp->bank,
                                                  id, 0);
                    if (child != NULL) {
                        child->idnum = pp->idnum;
                        child->gen = pp->gen;
                        if (pp->gen != NULL) {
                            pp->gen->numChild++;
                        }
                        if (pp->gen->kind & 0x20000000) {
                            child->kind |= 0x20000000;
                        }
                        psApplyVelocityLocalRotation(child);
                        if (pp->gen != NULL && (pp->gen->type & 0x2000)) {
                            psChangeParticleAppSRT(child, pp->appsrt);
                        } else {
                            psAttachParticleAppSRT(child, pp->appsrt);
                        }
                        child->pos.x = pp->pos.x;
                        child->pos.y = pp->pos.y;
                        child->pos.z = pp->pos.z;
                        psInterpretParticle0(child, pp);
                    }
                    break;
                }
                case 0xA5:
                    id = *cmdList++ << 8;
                    id += *cmdList++;
                    gp = psCreateGeneratorID(pp->linkNo, pp->bank, id);
                    if (gp != NULL) {
                        gp->idnum = pp->idnum;
                        psCopyGeneratorData(gp, pp->gen);
                        if (pp->appsrt != NULL) {
                            if (pp->gen != NULL && (pp->gen->type & 0x2000)) {
                                psChangeGeneratorAppSRT(gp, pp->appsrt);
                            } else {
                                psAttachGeneratorAppSRT(gp, pp->appsrt);
                            }
                        }
                        if (pp->appsrt != NULL) {
                            if (gp->appsrt != NULL) {
                                gp->pos.x = pp->pos.x;
                                gp->pos.y = pp->pos.y;
                                gp->pos.z = pp->pos.z;
                                if (gp->appsrt != pp->appsrt) {
                                    gp->appsrt->translate =
                                        pp->appsrt->translate;
                                }
                            }
                        } else if (gp->appsrt != NULL) {
                            gp->appsrt->translate.x = pp->pos.x;
                            gp->appsrt->translate.y = pp->pos.y;
                            gp->appsrt->translate.z = pp->pos.z;
                            gp->pos.x = pp->pos.x;
                            gp->pos.y = pp->pos.y;
                            gp->pos.z = pp->pos.z;
                        } else {
                            gp->pos.x = pp->pos.x;
                            gp->pos.y = pp->pos.y;
                            gp->pos.z = pp->pos.z;
                        }
                        if (gp->appsrt != pp->appsrt) {
                            if (pp->appsrt == NULL) {
                                if (gp->appsrt != NULL) {
                                    genPosUpdate(gp);
                                    gp->pos.x -= gp->appsrt->translate.x;
                                    gp->pos.y -= gp->appsrt->translate.y;
                                    gp->pos.z -= gp->appsrt->translate.z;
                                }
                            } else {
                                genPosUpdate(pp->appsrt->gp);
                                if (gp->appsrt != NULL) {
                                    genPosUpdate(gp);
                                    gp->pos.x += pp->appsrt->translate.x -
                                                 gp->appsrt->translate.x;
                                    gp->pos.y += pp->appsrt->translate.y -
                                                 gp->appsrt->translate.y;
                                    gp->pos.z += pp->appsrt->translate.z -
                                                 gp->appsrt->translate.z;
                                } else {
                                    gp->pos.x += pp->appsrt->translate.x;
                                    gp->pos.y += pp->appsrt->translate.y;
                                    gp->pos.z += pp->appsrt->translate.z;
                                }
                            }
                        }
                    }
                    break;
                case 0xEF: {
                    id = *cmdList++ << 8;
                    id += *cmdList++;
                    /* RULE-EXCEPTION(title-path): the kind byte reuses op (dead after
                     * dispatch); only register allocation shows it is one variable -
                     * see docs/RULE_EXCEPTIONS.md */
                    op = *cmdList++;
                    gp = psCreateGeneratorID(pp->linkNo, pp->bank, id);
                    if (gp != NULL) {
                        gp->idnum = pp->idnum;
                        psCopyGeneratorData(gp, pp->gen);
                        if (pp->appsrt != NULL) {
                            if (pp->gen != NULL && (pp->gen->type & 0x2000)) {
                                psChangeGeneratorAppSRT(gp, pp->appsrt);
                            } else {
                                psAttachGeneratorAppSRT(gp, pp->appsrt);
                            }
                        }
                        gp->kind &= ~0x0E000000;
                        gp->kind |= (op & 7) << 25;
                        if (pp->appsrt != NULL) {
                            if (gp->appsrt != NULL) {
                                gp->pos.x = pp->pos.x;
                                gp->pos.y = pp->pos.y;
                                gp->pos.z = pp->pos.z;
                                if (gp->appsrt != pp->appsrt) {
                                    gp->appsrt->translate =
                                        pp->appsrt->translate;
                                }
                            }
                        } else if (gp->appsrt != NULL) {
                            gp->appsrt->translate.x = pp->pos.x;
                            gp->appsrt->translate.y = pp->pos.y;
                            gp->appsrt->translate.z = pp->pos.z;
                            gp->pos.x = pp->pos.x;
                            gp->pos.y = pp->pos.y;
                            gp->pos.z = pp->pos.z;
                        } else {
                            gp->pos.x = pp->pos.x;
                            gp->pos.y = pp->pos.y;
                            gp->pos.z = pp->pos.z;
                        }
                        if (gp->appsrt != pp->appsrt) {
                            if (pp->appsrt == NULL) {
                                if (gp->appsrt != NULL) {
                                    genPosUpdate(gp);
                                    gp->pos.x -= gp->appsrt->translate.x;
                                    gp->pos.y -= gp->appsrt->translate.y;
                                    gp->pos.z -= gp->appsrt->translate.z;
                                }
                            } else {
                                genPosUpdate(pp->appsrt->gp);
                                if (gp->appsrt != NULL) {
                                    genPosUpdate(gp);
                                    gp->pos.x += pp->appsrt->translate.x -
                                                 gp->appsrt->translate.x;
                                    gp->pos.y += pp->appsrt->translate.y -
                                                 gp->appsrt->translate.y;
                                    gp->pos.z += pp->appsrt->translate.z -
                                                 gp->appsrt->translate.z;
                                } else {
                                    gp->pos.x += pp->appsrt->translate.x;
                                    gp->pos.y += pp->appsrt->translate.y;
                                    gp->pos.z += pp->appsrt->translate.z;
                                }
                            }
                        }
                    }
                    break;
                }
                case 0xF0: {
                    id = *cmdList++ << 8;
                    id += *cmdList++;
                    /* RULE-EXCEPTION(title-path): the kind byte reuses op (dead after
                     * dispatch); only register allocation shows it is one variable -
                     * see docs/RULE_EXCEPTIONS.md */
                    op = *cmdList++;
                    if (psBankRef[pp->bank] != NULL) {
                        id = psBankRef[pp->bank][id];
                    }
                    gp = psCreateGeneratorID(pp->linkNo, pp->bank, id);
                    if (gp != NULL) {
                        gp->idnum = pp->idnum;
                        psCopyGeneratorData(gp, pp->gen);
                        if (pp->appsrt != NULL) {
                            if (pp->gen != NULL && (pp->gen->type & 0x2000)) {
                                psChangeGeneratorAppSRT(gp, pp->appsrt);
                            } else {
                                psAttachGeneratorAppSRT(gp, pp->appsrt);
                            }
                        }
                        gp->kind &= ~0x0E000000;
                        gp->kind |= (op & 7) << 25;
                        if (pp->appsrt != NULL) {
                            if (gp->appsrt != NULL) {
                                gp->pos.x = pp->pos.x;
                                gp->pos.y = pp->pos.y;
                                gp->pos.z = pp->pos.z;
                                if (gp->appsrt != pp->appsrt) {
                                    gp->appsrt->translate =
                                        pp->appsrt->translate;
                                }
                            }
                        } else if (gp->appsrt != NULL) {
                            gp->appsrt->translate.x = pp->pos.x;
                            gp->appsrt->translate.y = pp->pos.y;
                            gp->appsrt->translate.z = pp->pos.z;
                            gp->pos.x = pp->pos.x;
                            gp->pos.y = pp->pos.y;
                            gp->pos.z = pp->pos.z;
                        } else {
                            gp->pos.x = pp->pos.x;
                            gp->pos.y = pp->pos.y;
                            gp->pos.z = pp->pos.z;
                        }
                        if (gp->appsrt != pp->appsrt) {
                            if (pp->appsrt == NULL) {
                                if (gp->appsrt != NULL) {
                                    genPosUpdate(gp);
                                    gp->pos.x -= gp->appsrt->translate.x;
                                    gp->pos.y -= gp->appsrt->translate.y;
                                    gp->pos.z -= gp->appsrt->translate.z;
                                }
                            } else {
                                genPosUpdate(pp->appsrt->gp);
                                if (gp->appsrt != NULL) {
                                    genPosUpdate(gp);
                                    gp->pos.x += pp->appsrt->translate.x -
                                                 gp->appsrt->translate.x;
                                    gp->pos.y += pp->appsrt->translate.y -
                                                 gp->appsrt->translate.y;
                                    gp->pos.z += pp->appsrt->translate.z -
                                                 gp->appsrt->translate.z;
                                } else {
                                    gp->pos.x += pp->appsrt->translate.x;
                                    gp->pos.y += pp->appsrt->translate.y;
                                    gp->pos.z += pp->appsrt->translate.z;
                                }
                            }
                        }
                    }
                    break;
                }
                case 0xA6: {
                    s32 base;
                    s32 range;

                    base = *cmdList++ << 8;
                    base += *cmdList++;
                    range = *cmdList++ << 8;
                    range += *cmdList++;
                    pp->life = base + (s32) (range * HSD_Randf());
                    break;
                }
                case 0xA7: {
                    s32 rate = *cmdList++;

                    if (rate >= (s32) (100.0f * HSD_Randf())) {
                        pp->life = 1;
                        goto exit;
                    }
                    break;
                }
                case 0xA8: {
                    Vec ofs;

                    cmdList = getFloat(cmdList, &val);
                    ofs.x = 2.0f * val * HSD_Randf() - val;
                    cmdList = getFloat(cmdList, &val);
                    ofs.y = 2.0f * val * HSD_Randf() - val;
                    cmdList = getFloat(cmdList, &val);
                    ofs.z = 2.0f * val * HSD_Randf() - val;
                    psApplyOffsetLocalRotation(pp, &ofs);
                    pp->pos.x += ofs.x;
                    pp->pos.y += ofs.y;
                    pp->pos.z += ofs.z;
                    break;
                }
                case 0xA9:
                    cmdList = getFloat(cmdList, &val);
                    modifyDir(pp, val);
                    break;
                case 0xF4: {
                    f32 y;
                    f32 z;

                    cmdList = getFloat(cmdList, &val2);
                    cmdList = getFloat(cmdList, &y);
                    cmdList = getFloat(cmdList, &z);
                    cmdList = getFloat(cmdList, &val);
                    if (pp->gen != NULL) {
                        modifyDirGenBase(pp, val, val2, y, z);
                    }
                    break;
                }
                case 0xF5:
                    if (pp->gen != NULL && pp->gen->appsrt != NULL) {
                        s32 done = pp->gen->type & 0x2000;

                        pp->gen->type |= 0x2000;
                        pp->gen->appsrt->x72 = 0;
                        if (!done) {
                            genPosUpdate(pp->gen);
                        }
                    }
                    break;
                case 0xF6:
                    if (pp->gen != NULL && pp->gen->appsrt != NULL) {
                        s32 done = pp->gen->type & 0x1000;

                        pp->gen->type |= 0x1000;
                        if (!done) {
                            genPosUpdate(pp->gen);
                        }
                    }
                    break;
                case 0xF7:
                    pp->kind |= 0x10000000;
                    break;
                case 0xAA: {
                    s32 range;
                    HSD_Particle* child;

                    id = *cmdList++ << 8;
                    id += *cmdList++;
                    range = *cmdList++ << 8;
                    range += *cmdList++;
                    id += (s32) (range * HSD_Randf());
                    if (psBankRef[pp->bank] != NULL) {
                        id = psBankRef[pp->bank][id];
                    }
                    child = psGenerateParticleID0(pp, pp->linkNo, pp->bank,
                                                  id, 0);
                    if (child != NULL) {
                        child->pos.x = pp->pos.x;
                        child->pos.y = pp->pos.y;
                        child->pos.z = pp->pos.z;
                        child->idnum = pp->idnum;
                        child->gen = pp->gen;
                        if (pp->gen != NULL) {
                            pp->gen->numChild++;
                        }
                        if (pp->gen->kind & 0x20000000) {
                            child->kind |= 0x20000000;
                        }
                        psApplyVelocityLocalRotation(child);
                        if (pp->appsrt != NULL) {
                            if (pp->gen != NULL && (pp->gen->type & 0x2000)) {
                                psChangeParticleAppSRT(child, pp->appsrt);
                            } else {
                                psAttachParticleAppSRT(child, pp->appsrt);
                            }
                        }
                        psInterpretParticle0(child, pp);
                    }
                    break;
                }
                case 0xAB:
                    cmdList = getFloat(cmdList, &val);
                    pp->vel.x *= val;
                    pp->vel.y *= val;
                    pp->vel.z *= val;
                    break;
                case 0xAC:
                    cmdList = getTime(cmdList, &pp->sizeCount);
                    cmdList = getFloat(cmdList, &pp->sizeTarget);
                    cmdList = getFloat(cmdList, &val);
                    pp->sizeTarget += val * HSD_Randf();
                    if (pp->sizeCount == 0) {
                        pp->size = pp->sizeTarget;
                    }
                    break;
                case 0xAD:
                    pp->kind |= 0x80;
                    break;
                case 0xAE:
                    pp->kind &= ~0x60;
                    break;
                case 0xAF:
                    pp->kind &= ~0x40;
                    pp->kind |= 0x20;
                    break;
                case 0xB0:
                    pp->kind &= ~0x20;
                    pp->kind |= 0x40;
                    break;
                case 0xB1:
                    pp->kind |= 0x60;
                    break;
                case 0xB2:
                    if (pp->appsrt != NULL && pp->appsrt->x72 == 0) {
                        f32 x;
                        f32 y;
                        f32 z;

                        genPosUpdate(pp->appsrt->gp);
                        HSD_MTXSRT(pp->appsrt->mmtx, &pp->appsrt->scale,
                                   &pp->appsrt->rot, &pp->appsrt->translate,
                                   NULL);
                        x = pp->appsrt->mmtx[0][0] * pp->pos.x +
                            pp->appsrt->mmtx[0][1] * pp->pos.y +
                            pp->appsrt->mmtx[0][2] * pp->pos.z +
                            pp->appsrt->mmtx[0][3];
                        y = pp->appsrt->mmtx[1][0] * pp->pos.x +
                            pp->appsrt->mmtx[1][1] * pp->pos.y +
                            pp->appsrt->mmtx[1][2] * pp->pos.z +
                            pp->appsrt->mmtx[1][3];
                        z = pp->appsrt->mmtx[2][0] * pp->pos.x +
                            pp->appsrt->mmtx[2][1] * pp->pos.y +
                            pp->appsrt->mmtx[2][2] * pp->pos.z +
                            pp->appsrt->mmtx[2][3];
                        pp->pos.x = x;
                        pp->pos.y = y;
                        pp->pos.z = z;
                        psRemoveParticleAppSRT(pp);
                    }
                    break;
                case 0xB3:
                    if (pp->aCmpCount != 0) {
                        s32 t = (pp->aCmpRemain << 16) / pp->aCmpCount;

                        pp->aCmpParam1 =
                            ((pp->aCmpParam1Target << 16) +
                             t * (pp->aCmpParam1 - pp->aCmpParam1Target)) >>
                            16;
                        pp->aCmpParam2 =
                            ((pp->aCmpParam2Target << 16) +
                             t * (pp->aCmpParam2 - pp->aCmpParam2Target)) >>
                            16;
                    }
                    cmdList = getTime(cmdList, &pp->aCmpCount);
                    pp->aCmpMode = cmdList[0];
                    pp->aCmpParam1Target = cmdList[1];
                    pp->aCmpParam2Target = cmdList[2];
                    cmdList += 3;
                    if (pp->aCmpCount == 0) {
                        pp->aCmpParam1 = pp->aCmpParam1Target;
                        pp->aCmpParam2 = pp->aCmpParam2Target;
                        pp->aCmpRemain = 0;
                        pp->aCmpCount = 0;
                    } else {
                        pp->aCmpRemain = pp->aCmpCount;
                    }
                    break;
                case 0xB4:
                    pp->kind |= 0x200;
                    break;
                case 0xB5:
                    pp->kind &= ~0x200;
                    break;
                case 0xB6:
                    cmdList = getTime(cmdList, &pp->rotateCount);
                    cmdList = getFloat(cmdList, &val);
                    pp->rotateTarget += val;
                    if (pp->rotateCount == 0) {
                        pp->rotate = pp->rotateTarget;
                    }
                    break;
                case 0xB7:
                    setVelToJObj(pp, psPointJObj[*cmdList++ + pp->pJObjOfs]);
                    break;
                case 0xB8: {
                    s32 no = *cmdList++ + pp->pJObjOfs;

                    cmdList = getFloat(cmdList, &val);
                    cmdList = getFloat(cmdList, &val2);
                    if (pp->gen != NULL && (pp->gen->posFlags & 0x100)) {
                        f32 s = (pp->gen->scale.x + pp->gen->scale.y +
                                 pp->gen->scale.z) /
                                3.0f;
                        val *= s;
                        val2 *= s;
                    }
                    if (applyForceJObj(pp, psPointJObj[no], val, val2)) {
                        pp->life = 1;
                        goto exit;
                    }
                    break;
                }
                case 0xB9: {
                    HSD_Particle* child;

                    id = *cmdList++ << 8;
                    id += *cmdList++;
                    child = psGenerateParticleID0(pp, pp->linkNo, pp->bank,
                                                  id, 0);
                    if (child != NULL) {
                        child->pos.x = pp->pos.x;
                        child->pos.y = pp->pos.y;
                        child->pos.z = pp->pos.z;
                        child->vel.x = pp->vel.x;
                        child->vel.y = pp->vel.y;
                        child->vel.z = pp->vel.z;
                        child->idnum = pp->idnum;
                        child->gen = pp->gen;
                        if (pp->gen != NULL) {
                            pp->gen->numChild++;
                        }
                        if (pp->gen->kind & 0x20000000) {
                            child->kind |= 0x20000000;
                        }
                        psApplyVelocityLocalRotation(child);
                        if (pp->appsrt != NULL) {
                            if (pp->gen != NULL && (pp->gen->type & 0x2000)) {
                                psChangeParticleAppSRT(child, pp->appsrt);
                            } else {
                                psAttachParticleAppSRT(child, pp->appsrt);
                            }
                        }
                        psInterpretParticle0(child, pp);
                    }
                    break;
                }
                case 0xF2: {
                    HSD_Particle* child;

                    id = *cmdList++ << 8;
                    id += *cmdList++;
                    if (psBankRef[pp->bank] != NULL) {
                        id = psBankRef[pp->bank][id];
                    }
                    child = psGenerateParticleID0(pp, pp->linkNo, pp->bank,
                                                  id, 0);
                    if (child != NULL) {
                        child->pos.x = pp->pos.x;
                        child->pos.y = pp->pos.y;
                        child->pos.z = pp->pos.z;
                        child->vel.x = pp->vel.x;
                        child->vel.y = pp->vel.y;
                        child->vel.z = pp->vel.z;
                        child->idnum = pp->idnum;
                        child->gen = pp->gen;
                        if (pp->gen != NULL) {
                            pp->gen->numChild++;
                        }
                        if (pp->gen->kind & 0x20000000) {
                            child->kind |= 0x20000000;
                        }
                        psApplyVelocityLocalRotation(child);
                        if (pp->appsrt != NULL) {
                            if (pp->gen != NULL && (pp->gen->type & 0x2000)) {
                                psChangeParticleAppSRT(child, pp->appsrt);
                            } else {
                                psAttachParticleAppSRT(child, pp->appsrt);
                            }
                        }
                        psInterpretParticle0(child, pp);
                    }
                    break;
                }
                case 0xBA:
                    PS_FREEZE_COLOR(pp->primCol, pp->primColTarget,
                                    pp->primColRemain, pp->primColCount);
                    val = (s8) *cmdList++ * 2 * HSD_Randf();
                    pp->primColTarget.r = U8ClampAdd(pp->primColTarget.r, val);
                    val = (s8) *cmdList++ * 2 * HSD_Randf();
                    pp->primColTarget.g = U8ClampAdd(pp->primColTarget.g, val);
                    val = (s8) *cmdList++ * 2 * HSD_Randf();
                    pp->primColTarget.b = U8ClampAdd(pp->primColTarget.b, val);
                    val = (s8) *cmdList++ * 2 * HSD_Randf();
                    pp->primColTarget.a = U8ClampAdd(pp->primColTarget.a, val);
                    if (pp->primColCount == 0) {
                        pp->primCol = pp->primColTarget;
                    } else {
                        pp->primColRemain = pp->primColCount;
                    }
                    break;
                case 0xBB:
                    PS_FREEZE_COLOR(pp->envCol, pp->envColTarget,
                                    pp->envColRemain, pp->envColCount);
                    val = (s8) *cmdList++ * 2 * HSD_Randf();
                    pp->envColTarget.r = U8ClampAdd(pp->envColTarget.r, val);
                    val = (s8) *cmdList++ * 2 * HSD_Randf();
                    pp->envColTarget.g = U8ClampAdd(pp->envColTarget.g, val);
                    val = (s8) *cmdList++ * 2 * HSD_Randf();
                    pp->envColTarget.b = U8ClampAdd(pp->envColTarget.b, val);
                    val = (s8) *cmdList++ * 2 * HSD_Randf();
                    pp->envColTarget.a = U8ClampAdd(pp->envColTarget.a, val);
                    if (pp->envColCount == 0) {
                        pp->envCol = pp->envColTarget;
                    } else {
                        pp->envColRemain = pp->envColCount;
                    }
                    break;
                case 0xBC:
                    pp->poseNum = cmdList[0];
                    val = cmdList[1];
                    cmdList += 2;
                    pp->poseNum += val * HSD_Randf();
                    tg = psTexGroupArray[pp->bank][pp->texGroup];
                    if (tg != NULL && tg->texTable != NULL &&
                        tg->texTable[pp->poseNum] != NULL)
                    {
                        pp->kind |= 0x400;
                    }
                    break;
                case 0xBD:
                    cmdList = getFloat(cmdList, &val);
                    cmdList = getFloat(cmdList, &val2);
                    if (pp->gen != NULL && (pp->gen->posFlags & 0x200)) {
                        f32 s = (pp->gen->scale.x + pp->gen->scale.y +
                                 pp->gen->scale.z) /
                                3.0f;
                        val *= s;
                        val2 *= s;
                    }
                    val = val + val2 * HSD_Randf();
                    val2 = sqrtf(pp->vel.x * pp->vel.x + pp->vel.y * pp->vel.y +
                                 pp->vel.z * pp->vel.z);
                    if (val2 > 1e-10f) {
                        val /= val2;
                        pp->vel.x *= val;
                        pp->vel.y *= val;
                        pp->vel.z *= val;
                    }
                    break;
                case 0xBE:
                    cmdList = getFloat(cmdList, &val);
                    pp->vel.x *= val;
                    cmdList = getFloat(cmdList, &val);
                    pp->vel.y *= val;
                    cmdList = getFloat(cmdList, &val);
                    pp->vel.z *= val;
                    break;
                case 0xBF: {
                    s32 no = *cmdList++;

                    pp->kind |= (((no + pp->pJObjOfs) & 7) << 12) | 0x8000;
                    break;
                }
                case 0xC0:
                    PS_FREEZE_COLOR(pp->primCol, pp->primColTarget,
                                    pp->primColRemain, pp->primColCount);
                    cmdList = getTime(cmdList, &pp->primColCount);
                    pp->primColTarget = pp->primCol;
                    if (cmd & 1) {
                        pp->primColTarget.r = *cmdList++;
                    }
                    if (cmd & 2) {
                        pp->primColTarget.g = *cmdList++;
                    }
                    if (cmd & 4) {
                        pp->primColTarget.b = *cmdList++;
                    }
                    if (cmd & 8) {
                        pp->primColTarget.a = *cmdList++;
                    }
                    if (pp->primColCount == 0) {
                        pp->primCol = pp->primColTarget;
                        pp->primColRemain = 0;
                    } else {
                        pp->primColRemain = pp->primColCount;
                    }
                    break;
                case 0xD0:
                    PS_FREEZE_COLOR(pp->envCol, pp->envColTarget,
                                    pp->envColRemain, pp->envColCount);
                    cmdList = getTime(cmdList, &pp->envColCount);
                    pp->envColTarget = pp->envCol;
                    if (cmd & 1) {
                        pp->envColTarget.r = *cmdList++;
                    }
                    if (cmd & 2) {
                        pp->envColTarget.g = *cmdList++;
                    }
                    if (cmd & 4) {
                        pp->envColTarget.b = *cmdList++;
                    }
                    if (cmd & 8) {
                        pp->envColTarget.a = *cmdList++;
                    }
                    if (pp->envColCount == 0) {
                        pp->envCol = pp->envColTarget;
                        pp->envColRemain = 0;
                    } else {
                        pp->envColRemain = pp->envColCount;
                    }
                    break;
                case 0xE0:
                    PS_FREEZE_COLOR(pp->primCol, pp->primColTarget,
                                    pp->primColRemain, pp->primColCount);
                    PS_FREEZE_COLOR(pp->envCol, pp->envColTarget,
                                    pp->envColRemain, pp->envColCount);
                    val = (s8) *cmdList++ * 2 * HSD_Randf();
                    pp->primColTarget.r = U8ClampAdd(pp->primColTarget.r, val);
                    pp->envColTarget.r = U8ClampAdd(pp->envColTarget.r, val);
                    val = (s8) *cmdList++ * 2 * HSD_Randf();
                    pp->primColTarget.g = U8ClampAdd(pp->primColTarget.g, val);
                    pp->envColTarget.g = U8ClampAdd(pp->envColTarget.g, val);
                    val = (s8) *cmdList++ * 2 * HSD_Randf();
                    pp->primColTarget.b = U8ClampAdd(pp->primColTarget.b, val);
                    pp->envColTarget.b = U8ClampAdd(pp->envColTarget.b, val);
                    val = (s8) *cmdList++ * 2 * HSD_Randf();
                    pp->primColTarget.a = U8ClampAdd(pp->primColTarget.a, val);
                    pp->envColTarget.a = U8ClampAdd(pp->envColTarget.a, val);
                    if (pp->primColCount == 0) {
                        pp->primCol = pp->primColTarget;
                    }
                    pp->primColRemain = pp->primColCount;
                    if (pp->envColCount == 0) {
                        pp->envCol = pp->envColTarget;
                    }
                    pp->envColRemain = pp->envColCount;
                    break;
                case 0xE9: {
                    u8 flags;
                    s32 steps;

                    PS_FREEZE_COLOR(pp->primCol, pp->primColTarget,
                                    pp->primColRemain, pp->primColCount);
                    PS_FREEZE_COLOR(pp->envCol, pp->envColTarget,
                                    pp->envColRemain, pp->envColCount);
                    steps = cmdList[1];
                    flags = cmdList[0];
                    cmdList += 2;
                    if (steps != 0) {
                        val = (s32) ((steps + 1) * HSD_Randf()) /
                              (f32) steps;
                    } else {
                        val = HSD_Randf();
                    }
                    if (flags & 1) {
                        val2 = val * ((s8) *cmdList++ * 2);
                        if (flags & 0x10) {
                            pp->primColTarget.r =
                                U8ClampAdd(pp->primColTarget.r, val2);
                        }
                        if (flags & 0x20) {
                            pp->envColTarget.r =
                                U8ClampAdd(pp->envColTarget.r, val2);
                        }
                    }
                    if (flags & 2) {
                        val2 = val * ((s8) *cmdList++ * 2);
                        if (flags & 0x10) {
                            pp->primColTarget.g =
                                U8ClampAdd(pp->primColTarget.g, val2);
                        }
                        if (flags & 0x20) {
                            pp->envColTarget.g =
                                U8ClampAdd(pp->envColTarget.g, val2);
                        }
                    }
                    if (flags & 4) {
                        val2 = val * ((s8) *cmdList++ * 2);
                        if (flags & 0x10) {
                            pp->primColTarget.b =
                                U8ClampAdd(pp->primColTarget.b, val2);
                        }
                        if (flags & 0x20) {
                            pp->envColTarget.b =
                                U8ClampAdd(pp->envColTarget.b, val2);
                        }
                    }
                    if (flags & 8) {
                        if (steps != 0) {
                            val = (s8) *cmdList++ * 2 *
                                  (f32) (s32) ((steps + 1) * HSD_Randf()) /
                                  steps;
                        } else {
                            val = (s8) *cmdList++ * 2 * HSD_Randf();
                        }
                        if (flags & 0x10) {
                            pp->primColTarget.a =
                                U8ClampAdd(pp->primColTarget.a, val);
                        }
                        if (flags & 0x20) {
                            pp->envColTarget.a =
                                U8ClampAdd(pp->envColTarget.a, val);
                        }
                    }
                    if (pp->primColCount == 0) {
                        pp->primCol = pp->primColTarget;
                    }
                    pp->primColRemain = pp->primColCount;
                    if (pp->envColCount == 0) {
                        pp->envCol = pp->envColTarget;
                    }
                    pp->envColRemain = pp->envColCount;
                    break;
                }
                case 0xE2:
                    pp->kind |= 8;
                    break;
                case 0xE3:
                    pp->palNum = *cmdList++;
                    break;
                case 0xE4:
                    switch (*cmdList++ & 3) {
                    case 0:
                        pp->kind &= ~0x40000;
                        break;
                    case 1:
                        pp->kind |= 0x40000;
                        break;
                    case 2:
                        pp->kind ^= 0x40000;
                        break;
                    case 3:
                        if (HSD_Randf() < 0.5f) {
                            pp->kind &= ~0x40000;
                        } else {
                            pp->kind |= 0x40000;
                        }
                        break;
                    }
                    break;
                case 0xE5:
                    switch (*cmdList++ & 3) {
                    case 0:
                        pp->kind &= ~0x80000;
                        break;
                    case 1:
                        pp->kind |= 0x80000;
                        break;
                    case 2:
                        pp->kind ^= 0x80000;
                        break;
                    case 3:
                        if (HSD_Randf() < 0.5f) {
                            pp->kind &= ~0x80000;
                        } else {
                            pp->kind |= 0x80000;
                        }
                        break;
                    }
                    break;
                case 0xE6:
                    pp->kind |= 0x200000;
                    break;
                case 0xE7:
                    pp->kind &= ~0x200000;
                    break;
                case 0xE8:
                    cmdList = getFloat(cmdList, &val);
                    if (val < 0.0f) {
                        pp->kind &= ~0x100000;
                    } else {
                        pp->kind |= 0x100000;
                        pp->trail = val;
                    }
                    break;
                case 0xEA: {
                    u8 flags;

                    if (pp->matColCount != 0) {
                        s32 t = (pp->matColRemain << 16) / pp->matColCount;

                        pp->matRGB = ((pp->matRGBTarget << 16) +
                                      t * (pp->matRGB - pp->matRGBTarget)) >>
                                     16;
                        pp->matA = ((pp->matATarget << 16) +
                                    t * (pp->matA - pp->matATarget)) >>
                                   16;
                    }
                    cmdList = getTime(cmdList, &pp->matColCount);
                    flags = *cmdList++;
                    pp->matRGBTarget = pp->matRGB;
                    if (flags & 1) {
                        pp->matRGBTarget = *cmdList++;
                    }
                    if (flags & 8) {
                        pp->matATarget = *cmdList++;
                    }
                    if (pp->matColCount == 0) {
                        pp->matRGB = pp->matRGBTarget;
                        pp->matColRemain = 0;
                    } else {
                        pp->matColRemain = pp->matColCount;
                    }
                    break;
                }
                case 0xEB: {
                    u8 flags;

                    if (pp->ambColCount != 0) {
                        s32 t = (pp->ambColRemain << 16) / pp->ambColCount;

                        pp->ambRGB = ((pp->ambRGBTarget << 16) +
                                      t * (pp->ambRGB - pp->ambRGBTarget)) >>
                                     16;
                        pp->ambA = ((pp->ambATarget << 16) +
                                    t * (pp->ambA - pp->ambATarget)) >>
                                   16;
                    }
                    cmdList = getTime(cmdList, &pp->ambColCount);
                    flags = *cmdList++;
                    pp->ambRGBTarget = pp->ambRGB;
                    if (flags & 1) {
                        pp->ambRGBTarget = *cmdList++;
                    }
                    if (flags & 8) {
                        pp->ambATarget = *cmdList++;
                    }
                    if (pp->ambColCount == 0) {
                        pp->ambRGB = pp->ambRGBTarget;
                        pp->ambColRemain = 0;
                    } else {
                        pp->ambColRemain = pp->ambColCount;
                    }
                    break;
                }
                case 0xED: {
                    s32 steps;

                    cmdList = getFloat(cmdList, &val);
                    cmdList = getFloat(cmdList, &val2);
                    steps = *cmdList++;
                    if (steps != 0) {
                        val = val + val2 *
                                        (s32) ((steps + 1) * HSD_Randf()) /
                                        (f32) steps;
                    } else {
                        val = val + val2 * HSD_Randf();
                    }
                    pp->rotateTarget += val;
                    pp->rotate += val;
                    break;
                }
                case 0xF3: {
                    s32 dir = *cmdList++;

                    cmdList = getFloat(cmdList, &pp->rotateTarget);
                    cmdList = getFloat(cmdList, &pp->x68);
                    cmdList = getTime(cmdList, &pp->rotateCount);
                    if (pp->rotateCount != 0) {
                        if (dir == 0) {
                            pp->rotateTarget += pp->x68 / 2.0;
                        } else {
                            pp->rotateTarget *= -1.0f;
                            pp->rotateTarget -= pp->x68 / 2.0;
                        }
                    } else {
                        pp->rotateTarget = 0.0f;
                        pp->x68 = 0.0f;
                    }
                    break;
                }
                case 0xFA:
                    pp->loopCount = *cmdList++;
                    pp->cmdLoopPtr = cmdList - pp->cmdList;
                    break;
                case 0xFB:
                    if (--pp->loopCount != 0) {
                        cmdList = pp->cmdList + pp->cmdLoopPtr;
                    }
                    break;
                case 0xFC:
                    pp->cmdMarkPtr = cmdList - pp->cmdList;
                    break;
                case 0xFD:
                    cmdList = pp->cmdList + pp->cmdMarkPtr;
                    break;
                case 0xFE:
                case 0xFF:
                    pp->life = 1;
                    goto exit;
                }
            }
        } while (time == 0);
    exit:
        pp->cmdPtr = cmdList - pp->cmdList;
        pp->cmdWait = time;
    }

    if (--pp->life == 0) {
        HSD_Particle* next;

        if (pp->gen != NULL) {
            pp->gen->numChild--;
        }
        next = _psListGetNext(pp);
        if (pp->appsrt != NULL && psRemoveParticleAppSRT(pp) == 0 &&
            prev == NULL && _psListGetFirst(pp->linkNo) != next)
        {
            next = _psListGetFirst(pp->linkNo);
        }
        psDeletePntJObjwithParticle(pp);
        _psListDelete(pp, prev);
        return next;
    }

    if (pp->kind & 4) {
        HSD_Generator* gen = pp->gen;
        f32 sa = sinf(pp->grav);
        f32 sb = sinf(pp->fric);
        f32 ca = cosf(pp->grav);
        f32 cb = cosf(pp->fric);
        f32 r;
        f32 x;
        f32 y;
        f32 z;

        pp->vel.z += gen->aux.tornado.vel;
        r = ABS(gen->radius);
        r += pp->vel.z * tanf(ABS(gen->angle));
        r *= pp->vel.y;
        pp->vel.x += gen->grav;
        x = r * cosf(pp->vel.x);
        y = r * sinf(pp->vel.x);
        z = pp->vel.z;
        pp->pos.x = gen->pos.x + (x * cb + z * sb);
        pp->pos.y = gen->pos.y + (sb * (-x * sa) + y * ca + cb * (z * sa));
        pp->pos.z = gen->pos.z + (sb * (-x * ca) - y * sa + cb * (z * ca));
    } else {
        if (pp->kind & 1) {
            pp->vel.y -= pp->grav;
        }
        if (pp->kind & 2) {
            pp->vel.x *= pp->fric;
            pp->vel.y *= pp->fric;
            pp->vel.z *= pp->fric;
        }
        pp->pos.x += pp->vel.x;
        pp->pos.y += pp->vel.y;
        pp->pos.z += pp->vel.z;
    }

    if (pp->kind & 0x8000) {
        u32 no = (pp->kind >> 12) & 7;

        if (psPointJObj[no] == NULL) {
            HSD_JObj* jobj = HSD_JObjAlloc();

            if (jobj != NULL) {
                psSetPointJObj(no + 1, jobj);
                HSD_JObjUnref(jobj);
            }
        }
        if (psPointJObj[no] != NULL) {
            HSD_JObjSetupMatrix(psPointJObj[no]);
            HSD_JObjAddTx(psPointJObj[no],
                          pp->pos.x - psPointJObj[no]->mtx[0][3]);
            HSD_JObjAddTy(psPointJObj[no],
                          pp->pos.y - psPointJObj[no]->mtx[1][3]);
            HSD_JObjAddTz(psPointJObj[no],
                          pp->pos.z - psPointJObj[no]->mtx[2][3]);
        }
    }
    return _psListGetNext(pp);
}

void psInterpretParticles(u32 mask)
{
    HSD_Particle* pp;
    HSD_Particle* next;
    HSD_Particle* lastPP;
    HSD_Particle* ret;
    s32 i;

    for (i = 0; i < PS_NUM_LINK; i++, mask >>= 1) {
        if (!(mask & 0x10000)) {
            lastPP = NULL;
            pp = _psListGetFirst(i);
            while (pp != NULL) {
                next = pp->next;
                ret = psInterpretParticle0(pp, lastPP);
                if (next != NULL) {
                    if (pp->next == next) {
                        lastPP = pp;
                    } else if (ret != next) {
                        lastPP = ret;
                        while (lastPP->next != next) {
                            HSD_ASSERT(0x810, lastPP);
                            lastPP = lastPP->next;
                        }
                    }
                }
                pp = next;
            }
        }
    }
}
