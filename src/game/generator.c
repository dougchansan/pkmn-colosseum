/**
 * @file generator.c
 * @brief HAL's particle generator module (sysdolphin generator.c) in the
 *        Genius Sonority fork, 0x80173624 - 0x80175F6C.
 *
 * The whole translation unit. It owns:
 *   .text   0x80173624 - 0x80175F6C
 *   .rodata 0x802739B0 - 0x80273A00 (object.h's ref_INC assert, __FILE__
 *           "generator.c", the "psCamera" assert expression)
 *   .data   0x8036C1E0 - 0x8036C248 (the three switch jump tables)
 *   .sdata  0x80478C38 - 0x80478C40 (the last generator id)
 *   .sbss   0x8047B180 - 0x8047B1A0 (the generator lists and callbacks)
 *   .sdata2 0x8047D6B0 - 0x8047D720 (the float literal pool, "jobj.h",
 *           "jobj")
 * psinterpret.c's data ends where each of these ranges starts; the
 * gs_xfb_capture unit follows.
 *
 * Reference: Melee's sysdolphin/baselib/generator.c (doldecomp/melee):
 * psGetNewIDNum = hsd_8039D1EC, genPosUpdate = hsd_8039D214,
 * psInitGenerator = hsd_8039D354, psDeleteGenerator = hsd_8039D3AC,
 * psKillGenerator = hsd_8039D4DC, psGeneratorAlloc = hsd_8039D9C8,
 * generateParticle = hsd_8039DAD4, psExecGenerator = hsd_8039EE24,
 * psCreateGeneratorID = hsd_8039F05C. The fork recycles generators through a
 * free list instead of HSD_ObjFree, keeps a per-generator rotation and
 * scale block (+0x88..+0xB0) instead of reading a JObj, has 64 banks, and
 * adds psSetBillboardCamera here (GNT4's generator.c has it too; Melee keeps
 * only its orphaned assert strings).
 *
 * Compiler: GC/1.3.2 -O4,p -inline auto,deferred -use_lmw_stmw on -sdata 8
 * -sdata2 8 -str reuse,readonly, unit-wide, no local pragmas (the pslist.c
 * and psInitParticle flags). Deferred inlining emits the TU in reverse
 * definition order, which is retail's address order (psGetNewIDNum last,
 * psSetBillboardCamera first), and lets later definitions inline into
 * earlier ones: psRemoveGenerator expands psKillAllGenerator (which expands
 * psKillGenerator) and psSetBillboardCamera(NULL); psExecGenerator expands
 * genPosUpdate and psDeleteGenerator; psCreateGeneratorID expands
 * psGeneratorAlloc. The standalone psGeneratorAlloc is never referenced and
 * is dead-stripped at link time.
 *
 * Reconstructed inlines (see the policy's "Reconstructed inline helpers"):
 * - psDeleteGenerator is Melee's hsd_8039D3AC. Retail expands it three
 *   times (psKillGenerator, psKillGeneratorID, psExecGenerator, and through
 *   psKillGenerator in psKillAllGenerator/psRemoveGenerator) with the same
 *   sequence. Each expansion keeps the returned predecessor in its own
 *   register and copies gen into it on the two "keep" paths (mr r29,r31 in
 *   psExecGenerator), which is the helper's result variable.
 * - HSD_CObjUnref is the cobj.h counterpart of HAL's HSD_WObjUnref (Melee
 *   wobj.h: NULL guard, ref_DEC, hsdDelete). psSetBillboardCamera carries
 *   its fingerprint: the guard is tested again on the stale CR right after
 *   the caller's own NULL test (beq/beq), and the same expansion inlined
 *   into psRemoveGenerator puts the camera in the helper's own register.
 *
 * The symbols keep their address names; see the declarations below.
 */
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "crt/float.h"
#include "crt/math_ppc.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_cobj.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_wobj.h"
#include "sysdolphin/baselib/jobj.h"
#include "sysdolphin/baselib/object.h"


#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923

typedef struct PSAppSRT {
    /* 0x00 */ struct PSAppSRT* next;
    /* 0x04 */ struct psGenerator* gp;
    /* 0x08 */ u8 pad08[0x2A];
    /* 0x32 */ u16 usedCount;
    /* 0x34 */ u8 pad34[0x3E];
    /* 0x72 */ u8 isGenerator;
} PSAppSRT;

typedef struct PSCmdList {
    u16 type;     /* 0x00 */
    u16 texGroup; /* 0x02 */
    u16 genLife;  /* 0x04 */
    u16 life;     /* 0x06 */
    u32 kind;     /* 0x08 */
    f32 grav;     /* 0x0C */
    f32 fric;     /* 0x10 */
    f32 vx;       /* 0x14 */
    f32 vy;       /* 0x18 */
    f32 vz;       /* 0x1C */
    f32 radius;   /* 0x20 */
    f32 angle;    /* 0x24 */
    f32 random;   /* 0x28 */
    f32 size;     /* 0x2C */
    f32 param1;   /* 0x30 */
    f32 param2;   /* 0x34 */
    f32 param3;   /* 0x38 */
    u8 cmdList[1]; /* 0x3C */
} PSCmdList;

typedef struct PSTexGroup {
    u8 pad00[0x16];
    u16 palflag; /* 0x16 */
} PSTexGroup;

typedef struct psGenerator {
    struct psGenerator* next; /* 0x00 */
    u32 kind;                 /* 0x04 */
    f32 random;               /* 0x08 */
    f32 count;                /* 0x0C */
    u16 genLife;              /* 0x10 */
    u16 type;                 /* 0x12 */
    u8 bank;                  /* 0x14 */
    u8 linkNo;                /* 0x15 */
    u8 texGroup;              /* 0x16 */
    u8 x17;                   /* 0x17 */
    u16 idnum;                /* 0x18 */
    u16 life;                 /* 0x1A */
    u8* cmdList;              /* 0x1C */
    Vec pos;                  /* 0x20 */
    Vec vel;                  /* 0x2C */
    f32 grav;                 /* 0x38 */
    f32 fric;                 /* 0x3C */
    f32 size;                 /* 0x40 */
    f32 radius;               /* 0x44 */
    f32 angle;                /* 0x48 */
    u32 numChild;             /* 0x4C */
    PSAppSRT* appsrt;         /* 0x50 */
    union {
        struct {
            f32 minAngle; /* 0x54 */
            f32 maxAngle; /* 0x58 */
            f32 height;   /* 0x5C */
        } cone;
        struct {
            f32 x2; /* 0x54 */
            f32 y2; /* 0x58 */
            f32 z2; /* 0x5C */
        } line;
        struct {
            f32 x, y, z;    /* 0x54 */
            f32 xx, xy, xz; /* 0x60 */
            f32 yx, yy, yz; /* 0x6C */
            f32 zx, zy, zz; /* 0x78 */
            u16 flag;       /* 0x84 */
        } rect;
        struct {
            f32 speed;    /* 0x54 */
            f32 latMid;   /* 0x58 */
            f32 latRange; /* 0x5C */
            f32 lonMid;   /* 0x60 */
            f32 lonRange; /* 0x64 */
        } sphere;
    } aux;
    u16 posFlags;             /* 0x88 */
    u16 gfxIdx;               /* 0x8A */
    Vec rot;                  /* 0x8C */
    Vec scale;                /* 0x98 */
    HSD_JObj* jobj;           /* 0xA4 */
    Vec scale2;               /* 0xA8 */
} psGenerator; /* 0xB4 */

extern void* memset(void* dst, int val, u32 size);
extern void* HSD_MemAlloc(u32 size);             /* HSD_MemAlloc */
extern void HSD_Free(void* p);               /* HSD_Free */
extern HSD_SList* HSD_SListRemove(HSD_SList* list); /* HSD_SListRemove */
extern f32 HSD_Randf(void);                   /* HSD_Randf */

extern void PSMTXIdentity(Mtx m);
extern void PSMTXConcat(const Mtx a, const Mtx b, Mtx ab);
extern void PSMTXRotRad(Mtx m, char axis, f32 rad);
extern void PSVECNormalize(const Vec* src, Vec* unit);
extern void PSVECCrossProduct(const Vec* a, const Vec* b, Vec* axb);
extern void HSD_CObjGetUpVector(HSD_CObj* cobj, Vec* up);

extern void psKillAllParticle(void);
extern void psKillGeneratorChild(psGenerator* gen);
extern s32 psRemoveGeneratorAppSRT(psGenerator* gen);
extern PSAppSRT* psAddGeneratorAppSRT(psGenerator* gen, u8 type);
extern void* psGenerateParticle(u8 linkNo, u8 bank, u32 id, u32 kind,
                                u8 texGroup, u8* cmdList, u16 life,
                                void* userdata, f32 x, f32 y, f32 z, f32 vx,
                                f32 vy, f32 vz, f32 size, f32 grav, f32 fric,
                                psGenerator* gen);

extern u16 lbl_8047B112; /* peak generator count */
extern u16 lbl_8047B118; /* live generator count */

extern PSTexGroup** lbl_804529C8[64]; /* per-bank texture groups */
extern PSCmdList** lbl_80452AC8[64];  /* per-bank generator records */
extern s32 lbl_80452CC8[64];          /* per-bank record count */

/* Defined in reverse address order (see the file header). */
void (*lbl_8047B198)(psGenerator* gen, Mtx rot); /* custom-shape emitter */
void (*lbl_8047B194)(psGenerator* gen);          /* custom-shape set-up */
HSD_CObj* lbl_8047B190;                          /* psCamera */
psGenerator* lbl_8047B18C;                       /* free generators */
psGenerator* lbl_8047B188;                       /* active generators */
psGenerator* lbl_8047B184;                       /* list cursor */
HSD_SList* lbl_8047B180;                         /* pending generators */

u16 lbl_80478C38 = 0x100; /* last generator id */

#define psCamera lbl_8047B190

void psKillGenerator(psGenerator* gen);
void psKillAllGenerator(void);
void psSetBillboardCamera(HSD_CObj* cobj);

u16 psGetNewIDNum(void)
{
    lbl_80478C38++;
    if (lbl_80478C38 < 256) {
        lbl_80478C38 = 256;
    }
    return lbl_80478C38;
}

void genPosUpdate(psGenerator* gen)
{
    HSD_JObj* jobj;

    if (gen != NULL && !(gen->posFlags & 2) && (gen->posFlags & 1)) {
        jobj = gen->jobj;
        if (jobj != NULL) {
            HSD_JObjSetupMatrix(jobj);
            gen->pos.x = gen->jobj->mtx[0][3];
            gen->pos.y = gen->jobj->mtx[1][3];
            gen->pos.z = gen->jobj->mtx[2][3];
        }
    }
}

void psInitGenerator(s32 count)
{
    s32 i;
    psGenerator* gen;

    lbl_8047B188 = NULL;
    lbl_8047B18C = NULL;
    for (i = count - 1; i >= 0; i--) {
        gen = HSD_MemAlloc(sizeof(psGenerator));
        memset(gen, 0, sizeof(psGenerator));
        if (gen == NULL) {
            return;
        }
        gen->next = lbl_8047B18C;
        lbl_8047B18C = gen;
    }
    lbl_8047B118 = 0;
    lbl_8047B112 = 0;
    lbl_8047B180 = NULL;
    lbl_8047B190 = NULL;
    lbl_8047B198 = NULL;
    lbl_8047B194 = NULL;
    lbl_8047B184 = NULL;
}

/*
 * Unlinks gen (whose predecessor is prev) and returns it to the free list,
 * unless children or a shared application SRT still need it: then it is
 * left to expire next frame and becomes the new predecessor.
 */
static inline psGenerator* psDeleteGenerator(psGenerator* gen,
                                             psGenerator* prev)
{
    psGenerator* result = prev;

    if (gen->type & 0x80) {
        psKillGeneratorChild(gen);
    }
    if (gen->numChild != 0) {
        gen->random = 0.0f;
        gen->genLife = 1;
        result = gen;
    } else if ((gen->type & 0x3800) && gen->appsrt != NULL &&
               gen->appsrt->gp == gen && gen->appsrt->usedCount != 1) {
        gen->random = 0.0f;
        gen->genLife = 1;
        result = gen;
    } else {
        if (prev == NULL) {
            lbl_8047B188 = gen->next;
        } else {
            prev->next = gen->next;
        }
        if (gen->appsrt != NULL) {
            psRemoveGeneratorAppSRT(gen);
        }
        gen->next = lbl_8047B18C;
        lbl_8047B18C = gen;
        lbl_8047B118--;
    }
    return result;
}

void psRemoveGenerator(void)
{
    psGenerator* gen;

    psKillAllParticle();
    psKillAllGenerator();

    gen = lbl_8047B18C;
    while (gen != NULL) {
        psGenerator* next = gen->next;

        HSD_Free(gen);
        gen = next;
    }
    lbl_8047B18C = NULL;
    psSetBillboardCamera(NULL);
}

void psKillGenerator(psGenerator* gen)
{
    psGenerator* cur;

    cur = lbl_8047B188;
    lbl_8047B184 = NULL;
    while (cur != NULL) {
        if (cur == gen) {
            lbl_8047B184 = psDeleteGenerator(gen, lbl_8047B184);
            if (lbl_8047B184 != NULL) {
                while (lbl_8047B184->next != NULL) {
                    lbl_8047B184 = lbl_8047B184->next;
                }
            } else if (lbl_8047B188 != NULL) {
                lbl_8047B184 = lbl_8047B188;
                while (lbl_8047B184->next != NULL) {
                    lbl_8047B184 = lbl_8047B184->next;
                }
            }
            return;
        }
        lbl_8047B184 = cur;
        cur = cur->next;
    }
}

void psKillGeneratorID(s32 id)
{
    psGenerator* gen = lbl_8047B188;

    lbl_8047B184 = NULL;
    while (gen != NULL) {
        psGenerator* next = gen->next;

        if (gen->idnum == (u16) id) {
            lbl_8047B184 = psDeleteGenerator(gen, lbl_8047B184);
        } else {
            lbl_8047B184 = gen;
        }
        gen = next;
    }
}

void psKillAllGenerator(void)
{
    psGenerator* gen = lbl_8047B188;

    while (gen != NULL) {
        psGenerator* next = gen->next;

        psKillGenerator(gen);
        gen = next;
    }
    while (lbl_8047B180 != NULL) {
        lbl_8047B180 = HSD_SListRemove(lbl_8047B180);
    }
}

/*
 * Emits the generator's particles for this frame (Melee hsd_8039DAD4). The
 * fork builds the emission basis from the generator's own rotation block
 * (+0x8C) unless it is billboarded, scales every emitted velocity by the
 * scale block (+0x98), and draws sphere emitters (type 8) over a latitude
 * range instead of Melee's hemisphere test.
 */
f32 generateParticle_8017424C(psGenerator* gen)
{
    Vec vel_copy;
    Vec emit_pos;
    Vec tmpvec;
    Vec vec;
    Mtx rot_mtx;
    Mtx mtx_x;
    Mtx mtx_y;
    Mtx mtx_z;
    Vec look_dir;
    Vec cam_up;
    Vec cross1;
    Vec vel_norm;
    Mtx trig_mtx;
    f32 cur_angle;
    f32 angle_step;
    f32 scale_x;
    f32 scale_y;
    f32 scale_z;
    f32 vel_mag;
    f32 angle1;
    f32 cone_angle;
    f32 dist;
    f32 radius;
    f32 elevation;
    f32 angle3;

    angle3 = angle1 = 0.0f;
    if (gen->count < 1.0f) {
        return gen->count;
    }

    vel_copy.x = gen->vel.x;
    vel_copy.y = gen->vel.y;
    vel_copy.z = gen->vel.z;
    scale_x = gen->scale.x;
    scale_y = gen->scale.y;
    scale_z = gen->scale.z;
    if ((gen->type & 0xF) == 2 && (gen->posFlags & 0x80)) {
        vel_copy.x *= scale_x;
        vel_copy.y *= scale_y;
        vel_copy.z *= scale_z;
    }

    vel_mag = sqrtf(vel_copy.z * vel_copy.z +
                    (vel_copy.x * vel_copy.x + vel_copy.y * vel_copy.y));

    PSMTXIdentity(rot_mtx);

    if (!(gen->kind & 0x30000)) {
        PSMTXRotRad(mtx_x, 'X', gen->rot.x);
        PSMTXRotRad(mtx_y, 'Y', gen->rot.y);
        PSMTXRotRad(mtx_z, 'Z', gen->rot.z);
        PSMTXConcat(mtx_y, mtx_x, mtx_x);
        PSMTXConcat(mtx_z, mtx_x, mtx_x);

        vec.x = mtx_x[0][0];
        vec.y = mtx_x[1][0];
        vec.z = mtx_x[2][0];
        PSVECNormalize(&vec, &vec);
        rot_mtx[0][0] = vec.x;
        rot_mtx[1][0] = vec.y;
        rot_mtx[2][0] = vec.z;

        vec.x = mtx_x[0][1];
        vec.y = mtx_x[1][1];
        vec.z = mtx_x[2][1];
        PSVECNormalize(&vec, &vec);
        rot_mtx[0][1] = vec.x;
        rot_mtx[1][1] = vec.y;
        rot_mtx[2][1] = vec.z;

        vec.x = mtx_x[0][2];
        vec.y = mtx_x[1][2];
        vec.z = mtx_x[2][2];
        PSVECNormalize(&vec, &vec);
        rot_mtx[0][2] = vec.x;
        rot_mtx[1][2] = vec.y;
        rot_mtx[2][2] = vec.z;

        rot_mtx[2][3] = 0.0f;
        rot_mtx[1][3] = 0.0f;
        rot_mtx[0][3] = 0.0f;
    }

    if (gen->kind & 0x10000) {
        HSD_ASSERT(626, psCamera);
        look_dir.x = psCamera->eyepos->pos.x - gen->pos.x;
        look_dir.y = psCamera->eyepos->pos.y - gen->pos.y;
        look_dir.z = psCamera->eyepos->pos.z - gen->pos.z;
        PSVECNormalize(&look_dir, &look_dir);
        HSD_CObjGetUpVector(psCamera, &cam_up);
        PSVECNormalize(&cam_up, &cam_up);
        PSVECCrossProduct(&cam_up, &look_dir, &cross1);
        PSVECCrossProduct(&look_dir, &cross1, &cam_up);

        rot_mtx[0][0] = cross1.x;
        rot_mtx[1][0] = cross1.y;
        rot_mtx[2][0] = cross1.z;
        rot_mtx[0][1] = cam_up.x;
        rot_mtx[1][1] = cam_up.y;
        rot_mtx[2][1] = cam_up.z;
        rot_mtx[0][2] = look_dir.x;
        rot_mtx[1][2] = look_dir.y;
        rot_mtx[2][2] = look_dir.z;
    }

    if ((gen->type & 0xF) != 1 && vel_mag > FLT_EPSILON) {
        f32 yaw;
        f32 sin_yaw;
        f32 cos_yaw;
        f32 pitch;
        f32 sin_pitch;
        f32 cos_pitch;

        vel_norm.x = gen->vel.x;
        vel_norm.y = gen->vel.y;
        vel_norm.z = gen->vel.z;
        PSVECNormalize(&vel_norm, &vel_norm);

        if (fabs(vel_norm.z) < FLT_MIN) {
            if (vel_norm.y >= 0.0f) {
                yaw = 1.5707964f;
            } else {
                yaw = -1.5707964f;
            }
        } else {
            yaw = atan2f(vel_norm.y, vel_norm.z);
        }
        sin_yaw = sinf(yaw);
        cos_yaw = cosf(yaw);

        {
            f32 projected_z = vel_norm.y * sin_yaw + vel_norm.z * cos_yaw;

            if (fabs(projected_z) < FLT_MIN) {
                if (vel_norm.x >= 0.0f) {
                    pitch = 1.5707964f;
                } else {
                    pitch = -1.5707964f;
                }
            } else {
                pitch = atan2f(vel_norm.x, projected_z);
            }
        }
        sin_pitch = sinf(pitch);
        cos_pitch = cosf(pitch);

        trig_mtx[0][0] = cos_pitch;
        trig_mtx[0][1] = 0.0f;
        trig_mtx[0][2] = sin_pitch;
        trig_mtx[0][3] = 0.0f;
        trig_mtx[1][0] = -sin_yaw * sin_pitch;
        trig_mtx[1][1] = cos_yaw;
        trig_mtx[1][2] = sin_yaw * cos_pitch;
        trig_mtx[1][3] = 0.0f;
        trig_mtx[2][0] = -cos_yaw * sin_pitch;
        trig_mtx[2][1] = -sin_yaw;
        trig_mtx[2][2] = cos_yaw * cos_pitch;
        trig_mtx[2][3] = 0.0f;
        PSMTXConcat(rot_mtx, trig_mtx, rot_mtx);
    }
    elevation = vel_mag;

    if ((gen->type & 0xF) == 2) {
        if (fabs(rot_mtx[2][2]) < FLT_MIN) {
            if (rot_mtx[1][2] >= 0.0f) {
                angle1 = 1.5707964f;
            } else {
                angle1 = -1.5707964f;
            }
        } else {
            angle1 = atan2f(rot_mtx[1][2], rot_mtx[2][2]);
        }
        {
            f32 comb = rot_mtx[1][2] * sinf(angle1) + rot_mtx[2][2] * cosf(angle1);

            if (fabs(comb) < FLT_MIN) {
                if (rot_mtx[0][2] >= 0.0f) {
                    angle3 = 1.5707964f;
                } else {
                    angle3 = -1.5707964f;
                }
            } else {
                angle3 = atan2f(rot_mtx[0][2], comb);
            }
        }
    }

    /*
     * Even angular spacing. In both disc/cone cases retail divides into a
     * scratch register, copies it to angle_step and seeds cur_angle from the
     * scratch copy (fdivs f0; fmr f30,f0; fmadds f31,f0,...): the step is a
     * value of its own, not angle_step read back.
     */
    if (gen->angle < 0.0f) {
        switch (gen->type & 0xF) {
        case 0:
        case 3:
        case 4: {
            f32 min_a = gen->aux.cone.minAngle;
            f32 rnd = HSD_Randf();
            f32 step = (gen->aux.cone.maxAngle - min_a) / (f32) (s32) gen->count;

            angle_step = step;
            cur_angle = step * rnd + min_a;
            break;
        }
        case 6:
        case 7: {
            f32 min_a = gen->aux.cone.minAngle;
            f32 rnd = HSD_Randf();
            f32 step = (gen->aux.cone.maxAngle - min_a) / (f32) (s32) gen->count;

            angle_step = step;
            cur_angle = step * rnd + min_a;
            break;
        }
        default:
            cur_angle = (f32) (2.0 * (M_PI * HSD_Randf()));
            angle_step = (f32) ((2.0 * M_PI) / (s32) gen->count);
            break;
        }
    }

    while (gen->count >= 1.0f) {
        switch (gen->type & 0xF) {
        case 0:
        case 3:
        case 4:
        case 6:
        case 7:
            if (gen->radius < 0.0f) {
                dist = -gen->radius;
                radius = 1.0f;
            } else {
                radius = HSD_Randf();
                if ((gen->type & 0xF) == 3 || (gen->type & 0xF) == 4) {
                    radius = sqrtf(radius);
                }
                dist = radius * gen->radius;
            }

            switch (gen->type & 0xF) {
            case 6:
                if (gen->angle < 0.0f) {
                    cur_angle += angle_step;
                    if (fabs(dist) < FLT_MIN) {
                        if (gen->aux.cone.height >= 0.0f) {
                            cone_angle = -gen->angle;
                        } else {
                            cone_angle = (f32) (M_PI - gen->angle);
                        }
                    } else {
                        cone_angle =
                            (f32) (M_PI_2 - atan2f(gen->aux.cone.height, dist) -
                                   gen->angle);
                    }
                } else {
                    f32 min_angle = gen->aux.cone.minAngle;

                    cur_angle = (gen->aux.cone.maxAngle - min_angle) *
                                    HSD_Randf() +
                                min_angle;
                    if (fabs(dist) < FLT_MIN) {
                        if (gen->aux.cone.height >= 0.0f) {
                            cone_angle = gen->angle;
                        } else {
                            cone_angle = (f32) (M_PI + gen->angle);
                        }
                    } else {
                        cone_angle =
                            (f32) (gen->angle +
                                   (M_PI_2 -
                                    atan2f(gen->aux.cone.height, dist)));
                    }
                }
                break;
            case 7:
                if (gen->angle < 0.0f) {
                    cone_angle = (f32) (M_PI_2 - gen->angle);
                    cur_angle += angle_step;
                } else {
                    cur_angle = gen->aux.cone.minAngle;
                    cur_angle = (gen->aux.cone.maxAngle - cur_angle) *
                                    HSD_Randf() +
                                cur_angle;
                    cone_angle = (f32) (M_PI_2 + gen->angle);
                }
                break;
            default:
                if (gen->angle < 0.0f) {
                    cone_angle = -gen->angle;
                    cur_angle += angle_step;
                    cone_angle = radius * cone_angle;
                } else {
                    cur_angle = gen->aux.cone.minAngle;
                    cur_angle = (gen->aux.cone.maxAngle - cur_angle) *
                                    HSD_Randf() +
                                cur_angle;
                    cone_angle = radius * gen->angle;
                }
                break;
            }

            emit_pos.x = dist * cosf(cur_angle);
            emit_pos.y = dist * sinf(cur_angle);

            if ((gen->type & 0xF) == 6 || (gen->type & 0xF) == 7) {
                emit_pos.z = HSD_Randf();
                if ((gen->type & 0xF) == 6) {
                    emit_pos.x *= 1.0f - emit_pos.z;
                    emit_pos.y *= 1.0f - emit_pos.z;
                }
                emit_pos.z *= gen->aux.cone.height;
            } else {
                emit_pos.z = 0.0f;
            }

            {
                f32 sin_ca = elevation * sinf(cone_angle);

                vec.x = sin_ca * cosf(cur_angle);
                vec.y = sin_ca * sinf(cur_angle);
                vec.z = elevation * cosf(cone_angle);
            }

            if ((gen->type & 0xF) == 3) {
                vec.x *= radius;
                vec.y *= radius;
                vec.z *= radius;
            }

            PSMTXMultVec(rot_mtx, &emit_pos, &emit_pos);
            emit_pos.x += gen->pos.x;
            emit_pos.y += gen->pos.y;
            emit_pos.z += gen->pos.z;
            PSMTXMultVec(rot_mtx, &vec, &vec);

            psGenerateParticle(gen->linkNo, gen->bank, 0, gen->kind,
                               gen->texGroup, gen->cmdList, gen->life, NULL,
                               emit_pos.x, emit_pos.y, emit_pos.z,
                               vec.x * scale_x, vec.y * scale_y,
                               vec.z * scale_z, gen->size, gen->grav,
                               gen->fric, gen);
            break;

        case 1: {
            f32 rnd = HSD_Randf();

            emit_pos.x = rnd * gen->aux.line.x2;
            emit_pos.y = rnd * gen->aux.line.y2;
            emit_pos.z = rnd * gen->aux.line.z2;

            PSMTXMultVec(rot_mtx, &emit_pos, &emit_pos);
            emit_pos.x += gen->pos.x;
            emit_pos.y += gen->pos.y;
            emit_pos.z += gen->pos.z;
            PSMTXMultVec(rot_mtx, &vel_copy, &vec);

            psGenerateParticle(gen->linkNo, gen->bank, 0, gen->kind,
                               gen->texGroup, gen->cmdList, gen->life, NULL,
                               emit_pos.x, emit_pos.y, emit_pos.z,
                               vec.x * scale_x, vec.y * scale_y,
                               vec.z * scale_z, gen->size, gen->grav,
                               gen->fric, gen);
            break;
        }

        case 2:
            if (gen->radius < 0.0f) {
                dist = 1.0f;
            } else {
                dist = HSD_Randf();
            }
            if (gen->angle < 0.0f) {
                cur_angle += angle_step;
            } else {
                cur_angle = (f32) (2.0 * (M_PI * HSD_Randf()));
            }
            gen->aux.line.x2 = elevation;
            psGenerateParticle(gen->linkNo, gen->bank, 0, gen->kind | 4,
                               gen->texGroup, gen->cmdList, gen->life, NULL,
                               0.0f, 0.0f, 0.0f, cur_angle, dist, 0.0f,
                               gen->size, angle1, angle3, gen);
            break;

        case 5:
            emit_pos.x = HSD_Randf();
            emit_pos.y = HSD_Randf();
            emit_pos.z = HSD_Randf();

            switch (gen->aux.rect.flag) {
            case 0:
                break;
            case 1:
                emit_pos.x = emit_pos.x > 0.5f ? 1.0f : 0.0f;
                break;
            case 2:
                emit_pos.y = emit_pos.y > 0.5f ? 1.0f : 0.0f;
                break;
            case 3: {
                f32 rnd = HSD_Randf();
                f32 a2 = gen->aux.rect.x;

                if (rnd > a2 / (a2 + gen->aux.rect.y)) {
                    emit_pos.y = emit_pos.y > 0.5f ? 1.0f : 0.0f;
                } else {
                    emit_pos.x = emit_pos.x > 0.5f ? 1.0f : 0.0f;
                }
                break;
            }
            case 4:
                emit_pos.z = emit_pos.z > 0.5f ? 1.0f : 0.0f;
                break;
            case 5: {
                f32 rnd = HSD_Randf();
                f32 a2 = gen->aux.rect.x;

                if (rnd > a2 / (a2 + gen->aux.rect.z)) {
                    emit_pos.z = emit_pos.z > 0.5f ? 1.0f : 0.0f;
                } else {
                    emit_pos.x = emit_pos.x > 0.5f ? 1.0f : 0.0f;
                }
                break;
            }
            case 6: {
                f32 rnd = HSD_Randf();
                f32 a2 = gen->aux.rect.y;

                if (rnd > a2 / (a2 + gen->aux.rect.z)) {
                    emit_pos.z = emit_pos.z > 0.5f ? 1.0f : 0.0f;
                } else {
                    emit_pos.y = emit_pos.y > 0.5f ? 1.0f : 0.0f;
                }
                break;
            }
            case 7: {
                f32 rnd = HSD_Randf();
                f32 a2 = gen->aux.rect.z;
                f32 b2 = gen->aux.rect.y;
                f32 c2 = gen->aux.rect.x;
                f32 r0 = 1.0f / (c2 * (b2 + a2) + b2 * a2);

                if (rnd < r0 * (c2 * b2)) {
                    emit_pos.z = emit_pos.z > 0.5f ? 1.0f : 0.0f;
                } else if (rnd > 1.0f - r0 * (c2 * a2)) {
                    emit_pos.y = emit_pos.y > 0.5f ? 1.0f : 0.0f;
                } else {
                    emit_pos.x = emit_pos.x > 0.5f ? 1.0f : 0.0f;
                }
                break;
            }
            }

            emit_pos.x -= 0.5f;
            emit_pos.y -= 0.5f;
            emit_pos.z -= 0.5f;

            tmpvec.x = gen->aux.rect.xx * emit_pos.x +
                       gen->aux.rect.yx * emit_pos.y +
                       gen->aux.rect.zx * emit_pos.z;
            tmpvec.y = gen->aux.rect.xy * emit_pos.x +
                       gen->aux.rect.yy * emit_pos.y +
                       gen->aux.rect.zy * emit_pos.z;
            tmpvec.z = gen->aux.rect.xz * emit_pos.x +
                       gen->aux.rect.yz * emit_pos.y +
                       gen->aux.rect.zz * emit_pos.z;

            PSMTXMultVec(rot_mtx, &tmpvec, &emit_pos);
            emit_pos.x += gen->pos.x;
            emit_pos.y += gen->pos.y;
            emit_pos.z += gen->pos.z;

            {
                f32 mag = sqrtf(gen->aux.rect.zx * gen->aux.rect.zx +
                                gen->aux.rect.zy * gen->aux.rect.zy +
                                gen->aux.rect.zz * gen->aux.rect.zz);
                f32 scale = elevation / mag;

                vec.x = gen->aux.rect.zx * scale;
                vec.y = gen->aux.rect.zy * scale;
                vec.z = gen->aux.rect.zz * scale;
            }

            PSMTXMultVec(rot_mtx, &vec, &vec);

            psGenerateParticle(gen->linkNo, gen->bank, 0, gen->kind,
                               gen->texGroup, gen->cmdList, gen->life, NULL,
                               emit_pos.x, emit_pos.y, emit_pos.z,
                               vec.x * scale_x, vec.y * scale_y,
                               vec.z * scale_z, gen->size, gen->grav,
                               gen->fric, gen);
            break;

        case 8: {
            f32 lat;
            f32 lon;

            if (0.0f == gen->aux.sphere.latRange) {
                lat = (f32) (M_PI * HSD_Randf());
            } else {
                lat = gen->aux.sphere.latRange * HSD_Randf();
            }
            lon = (f32) (2.0 * (M_PI * HSD_Randf()));
            {
                f32 r2 = gen->radius;

                if (r2 < 0.0f) {
                    dist = -r2;
                } else {
                    dist = r2 * HSD_Randf();
                }
            }

            vec.x = sinf(lat) * cosf(lon);
            vec.y = sinf(lat) * sinf(lon);
            vec.z = cosf(lat);

            PSMTXMultVec(rot_mtx, &vec, &emit_pos);

            vec.x = emit_pos.x * gen->aux.sphere.speed;
            vec.y = emit_pos.y * gen->aux.sphere.speed;
            vec.z = emit_pos.z * gen->aux.sphere.speed;

            if (gen->radius >= 0.0f && gen->aux.sphere.speed < 0.0f) {
                f32 scale = dist / gen->radius;

                vec.x *= scale;
                vec.y *= scale;
                vec.z *= scale;
            }

            emit_pos.x = dist * emit_pos.x + gen->pos.x;
            emit_pos.y = dist * emit_pos.y + gen->pos.y;
            emit_pos.z = dist * emit_pos.z + gen->pos.z;

            psGenerateParticle(gen->linkNo, gen->bank, 0, gen->kind,
                               gen->texGroup, gen->cmdList, gen->life, NULL,
                               emit_pos.x, emit_pos.y, emit_pos.z,
                               vec.x * scale_x, vec.y * scale_y,
                               vec.z * scale_z, gen->size, gen->grav,
                               gen->fric, gen);
            break;
        }

        default:
            if (lbl_8047B198 != NULL) {
                lbl_8047B198(gen, rot_mtx);
            }
            break;
        }

        gen->count -= 1.0f;
    }

    return gen->count;
}

void psExecGenerator(u32 mask)
{
    psGenerator* gen;

    while (lbl_8047B180 != NULL) {
        lbl_8047B180 = HSD_SListRemove(lbl_8047B180);
    }

    gen = lbl_8047B188;
    lbl_8047B184 = NULL;

    while (gen != NULL) {
        if (mask & (1 << (gen->linkNo + 16))) {
            lbl_8047B184 = gen;
            gen = gen->next;
            continue;
        }
        if (gen->kind & 0x800) {
            lbl_8047B184 = gen;
            gen = gen->next;
            continue;
        }
        genPosUpdate(gen);
        if (gen->random < 0.0f) {
            gen->count -= gen->random;
        } else {
            gen->count += gen->random * HSD_Randf();
        }
        if (gen->count >= 1.0f) {
            gen->count = generateParticle_8017424C(gen);
        }
        if (gen->genLife != 0) {
            if (--gen->genLife == 0) {
                if (gen->x17 == 0) lbl_8047B184 = psDeleteGenerator(gen, lbl_8047B184); else {
                    gen->kind |= 0x800;
                }
                if (lbl_8047B184 != NULL) {
                    gen = lbl_8047B184->next;
                } else {
                    gen = lbl_8047B188;
                }
                continue;
            }
        }
        lbl_8047B184 = gen;
        gen = gen->next;
    }
}

psGenerator* psGeneratorAlloc(void)
{
    psGenerator* gen;

    if (lbl_8047B18C == NULL) {
        lbl_8047B18C = HSD_MemAlloc(sizeof(psGenerator));
        memset(lbl_8047B18C, 0, sizeof(psGenerator));
    }
    gen = lbl_8047B18C;
    if (gen == NULL) {
        return NULL;
    }

    lbl_8047B118++;
    if (lbl_8047B118 > lbl_8047B112) {
        lbl_8047B112 = lbl_8047B118;
    }
    lbl_8047B18C = gen->next;

    if (lbl_8047B184 == NULL || lbl_8047B184->next == NULL) {
        if (lbl_8047B188 == NULL) {
            gen->next = NULL;
            lbl_8047B188 = gen;
        } else {
            gen->next = lbl_8047B188->next;
            lbl_8047B188->next = gen;
        }
    } else {
        gen->next = lbl_8047B184->next->next;
        lbl_8047B184->next->next = gen;
    }

    lbl_80478C38++;
    if (lbl_80478C38 < 0x100) {
        lbl_80478C38 = 0x100;
    }
    gen->idnum = lbl_80478C38;
    gen->appsrt = NULL;
    gen->x17 = 0;
    return gen;
}

psGenerator* psCreateGeneratorID(s32 linkNo, s32 bank, s32 idx)
{
    psGenerator* gen;

    if (bank >= 64) {
        return NULL;
    }
    if (linkNo >= 8) {
        return NULL;
    }
    if (idx >= lbl_80452CC8[bank]) {
        return NULL;
    }
    if (lbl_80452AC8[bank][idx] == NULL) {
        return NULL;
    }

    gen = psGeneratorAlloc();
    if (gen != NULL) {
        gen->type = lbl_80452AC8[bank][idx]->type;
        gen->bank = bank;
        gen->linkNo = linkNo;
        gen->gfxIdx = idx;
        gen->kind = lbl_80452AC8[bank][idx]->kind;
        gen->texGroup = lbl_80452AC8[bank][idx]->texGroup;
        gen->life = lbl_80452AC8[bank][idx]->life;
        gen->genLife = lbl_80452AC8[bank][idx]->genLife;
        gen->pos.z = 0.0f;
        gen->pos.y = 0.0f;
        gen->pos.x = 0.0f;
        gen->vel.x = lbl_80452AC8[bank][idx]->vx;
        gen->vel.y = lbl_80452AC8[bank][idx]->vy;
        gen->vel.z = lbl_80452AC8[bank][idx]->vz;
        gen->grav = lbl_80452AC8[bank][idx]->grav;
        gen->fric = lbl_80452AC8[bank][idx]->fric;
        gen->size = lbl_80452AC8[bank][idx]->size;
        gen->cmdList = lbl_80452AC8[bank][idx]->cmdList;
        gen->radius = lbl_80452AC8[bank][idx]->radius;
        gen->angle = lbl_80452AC8[bank][idx]->angle;
        gen->random = lbl_80452AC8[bank][idx]->random;

        if (gen->kind & 0x100) {
            if (gen->random < 0.0f) {
                gen->count = (1.0f + gen->random > FLT_EPSILON) ? 1.0f : 0.0f;
            } else {
                gen->count = 1.0f - FLT_EPSILON;
            }
        } else if (gen->random < 0.0f) {
            gen->count = 0.0f;
        } else {
            gen->count = HSD_Randf();
        }

        {
            PSTexGroup* tg = lbl_804529C8[bank][gen->texGroup];

            if (tg != NULL && tg->palflag != 0) {
                gen->kind |= 0x10;
            }
        }

        gen->numChild = 0;

        switch (gen->type & 0xF) {
        case 0:
        case 3:
        case 4: {
            PSCmdList* c = lbl_80452AC8[bank][idx];

            if (0.0f == c->param1 && 0.0f == c->param2) {
                gen->aux.cone.minAngle = 0.0f;
                gen->aux.cone.maxAngle = 6.2831855f;
            } else {
                gen->aux.cone.minAngle = c->param1;
                gen->aux.cone.maxAngle = lbl_80452AC8[bank][idx]->param2;
            }
            break;
        }
        case 1:
            gen->aux.line.x2 = lbl_80452AC8[bank][idx]->param1;
            gen->aux.line.y2 = lbl_80452AC8[bank][idx]->param2;
            gen->aux.line.z2 = lbl_80452AC8[bank][idx]->param3;
            break;
        case 2:
            break;
        case 6:
        case 7: {
            PSCmdList* c = lbl_80452AC8[bank][idx];

            if (0.0f == c->param1 && 0.0f == c->param2) {
                gen->aux.cone.minAngle = 0.0f;
                gen->aux.cone.maxAngle = 6.2831855f;
            } else {
                gen->aux.cone.minAngle = c->param1;
                gen->aux.cone.maxAngle = lbl_80452AC8[bank][idx]->param2;
            }
            gen->aux.cone.height = lbl_80452AC8[bank][idx]->param3;
            break;
        }
        case 5:
            gen->aux.rect.xx = gen->aux.rect.x = lbl_80452AC8[bank][idx]->param1;
            gen->aux.rect.yy = gen->aux.rect.y = lbl_80452AC8[bank][idx]->param2;
            gen->aux.rect.zz = gen->aux.rect.z = lbl_80452AC8[bank][idx]->param3;
            gen->aux.rect.zy = 0.0f;
            gen->aux.rect.zx = 0.0f;
            gen->aux.rect.yz = 0.0f;
            gen->aux.rect.yx = 0.0f;
            gen->aux.rect.xz = 0.0f;
            gen->aux.rect.xy = 0.0f;
            gen->aux.rect.flag = 0;
            if (lbl_80452AC8[bank][idx]->param1 < 0.0f) {
                gen->aux.rect.flag |= 1;
            }
            if (lbl_80452AC8[bank][idx]->param2 < 0.0f) {
                gen->aux.rect.flag |= 2;
            }
            if (lbl_80452AC8[bank][idx]->param3 < 0.0f) {
                gen->aux.rect.flag |= 4;
            }
            break;
        case 8: {
            f32 mag;

            gen->aux.sphere.speed = sqrtf(gen->vel.z * gen->vel.z +
                                          (gen->vel.x * gen->vel.x +
                                           gen->vel.y * gen->vel.y));
            mag = sqrtf(gen->vel.x * gen->vel.x + gen->vel.z * gen->vel.z);
            if (mag < FLT_MIN) {
                if (gen->vel.y >= 0.0f) {
                    gen->aux.sphere.latMid = 1.5707964f;
                } else {
                    gen->aux.sphere.latMid = -1.5707964f;
                }
            } else {
                gen->aux.sphere.latMid = atan2f(gen->vel.y, mag);
            }
            if (fabs(gen->vel.x) < FLT_MIN) {
                if (gen->vel.z >= 0.0f) {
                    gen->aux.sphere.lonMid = 1.5707964f;
                } else {
                    gen->aux.sphere.lonMid = -1.5707964f;
                }
            } else {
                gen->aux.sphere.lonMid = atan2f(gen->vel.z, gen->vel.x);
            }
            gen->aux.sphere.latRange = lbl_80452AC8[bank][idx]->param1;
            if (gen->aux.sphere.latRange < 0.0f) {
                gen->aux.sphere.latRange = -gen->aux.sphere.latRange;
                gen->aux.sphere.speed = -gen->aux.sphere.speed;
            }
            gen->aux.sphere.lonRange = lbl_80452AC8[bank][idx]->param2;
            break;
        }
        default:
            if (lbl_8047B194 != NULL) {
                lbl_8047B194(gen);
            }
            break;
        }

        if (gen->kind & 0x20000) {
            gen->type |= 0x800;
            psAddGeneratorAppSRT(gen, 0);
            if (gen->appsrt != NULL) {
                gen->appsrt->isGenerator = 1;
                gen->appsrt->gp = gen;
            }
        }

        gen->posFlags = 2;
        gen->jobj = NULL;
        gen->rot.x = gen->rot.y = gen->rot.z = 0.0f;
        gen->scale.x = gen->scale.y = gen->scale.z = 1.0f;
        gen->scale2.x = gen->scale2.y = gen->scale2.z = 1.0f;
    }
    return gen;
}

/* cobj.h: drop a reference and delete the camera when it was the last one
 * (the shape of HAL's HSD_WObjUnref; see the file header). */
static inline void HSD_CObjUnref(HSD_CObj* cobj)
{
    if (cobj == NULL) {
        return;
    }
    if (ref_DEC(cobj) != 0) {
        hsdDelete(cobj);
    }
}

void psSetBillboardCamera(HSD_CObj* cobj)
{
    if (cobj != lbl_8047B190) {
        if (lbl_8047B190 != NULL) {
            HSD_CObjUnref(lbl_8047B190);
        }
        if (cobj != NULL) {
            ref_INC(cobj);
        }
        lbl_8047B190 = cobj;
    }
}
