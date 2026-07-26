#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "crt/math.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_jobj.h"
#include "hsd/hsd_robj.h"

extern void* HSD_IDGetDataFromTable(void* table, u32 id, s32* success);
extern void HSD_JObjSetupMatrix(HSD_JObj* jobj);
extern void HSD_JObjMakeMatrix(HSD_JObj* jobj);
extern void fn_801AE008(HSD_Exp* exp, u32 type, void* object,
                        HSD_ObjUpdateFunc update);
extern void PSMTXCopy(const Mtx src, Mtx dst);
extern f32 PSVECMag(const Vec* vec);
extern void PSVECSubtract(const Vec* a, const Vec* b, Vec* out);
extern f32 PSVECDotProduct(const Vec* a, const Vec* b);
extern void PSVECCrossProduct(const Vec* a, const Vec* b, Vec* out);
extern void PSVECScale(const Vec* src, Vec* dst, f32 scale);
void fn_801AEFF0(HSD_RObj* robj, HSD_JObj* jobj);
extern s32 fn_801AFCAC(HSD_RObj* robj, u32 type, Vec* out);

static inline s32 robj_fpclassifyf(f32 value)
{
    switch (*(s32*) &value & 0x7F800000) {
    case 0x7F800000:
        if (*(s32*) &value & 0x007FFFFF) {
            return 1;
        }
        return 2;
    case 0:
        if (*(s32*) &value & 0x007FFFFF) {
            return 5;
        }
        return 3;
    }
    return 4;
}

static inline f32 robj_sqrtf(f32 value)
{
    f64 guess;

    if (value > 0.0F) {
        guess = __frsqrte(value);
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        return (f32) (value * guess);
    }
    if ((f64) value < 0.0) {
        return lbl_80478AC0[0];
    }

    if (robj_fpclassifyf(value) == 1) {
        return lbl_80478AC0[0];
    }
    return value;
}

#define ROBJ_SQRTF_INLINE(result, expression)                               \
    do {                                                                    \
        f32 robj_sqrt_value = (expression);                                 \
        if (robj_sqrt_value > 0.0F) {                                       \
            f64 robj_sqrt_guess = __frsqrte(robj_sqrt_value);               \
            robj_sqrt_guess =                                               \
                0.5 * robj_sqrt_guess *                                    \
                (3.0 - robj_sqrt_value *                                   \
                           (robj_sqrt_guess * robj_sqrt_guess));             \
            robj_sqrt_guess =                                               \
                0.5 * robj_sqrt_guess *                                    \
                (3.0 - robj_sqrt_value *                                   \
                           (robj_sqrt_guess * robj_sqrt_guess));             \
            robj_sqrt_guess =                                               \
                0.5 * robj_sqrt_guess *                                    \
                (3.0 - robj_sqrt_value *                                   \
                           (robj_sqrt_guess * robj_sqrt_guess));             \
            (result) = (f32) (robj_sqrt_value * robj_sqrt_guess);           \
        } else if ((f64) robj_sqrt_value < 0.0) {                           \
            (result) = lbl_80478AC0[0];                                     \
        } else {                                                            \
            s32 robj_sqrt_class;                                            \
            switch (*(s32*) &robj_sqrt_value & 0x7F800000) {                \
            case 0x7F800000:                                                \
                robj_sqrt_class =                                           \
                    (*(s32*) &robj_sqrt_value & 0x007FFFFF) ? 1 : 2;         \
                break;                                                      \
            case 0:                                                         \
                robj_sqrt_class =                                           \
                    (*(s32*) &robj_sqrt_value & 0x007FFFFF) ? 5 : 3;         \
                break;                                                      \
            default:                                                        \
                robj_sqrt_class = 4;                                        \
                break;                                                      \
            }                                                               \
            (result) = robj_sqrt_class == 1                                 \
                           ? lbl_80478AC0[0]                                 \
                           : robj_sqrt_value;                                \
        }                                                                   \
    } while (0)

static inline f32 robj_absf(f32 value)
{
    return value < 0.0F ? -value : value;
}

static inline void RObjJObjRefThis(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        iref_INC(jobj);
    }
}

static inline s32 RObjGetGlobalPosition(HSD_RObj* robj, u32 type, Vec* out)
{
    HSD_RObj* current;
    Vec position = { 0.0F, 0.0F, 0.0F };
    s32 count = 0;

    if (robj == NULL) {
        return 0;
    }

    for (current = robj; current != NULL; current = current->next) {
        if ((current->flags & ROBJ_TYPE_MASK) == REFTYPE_JOBJ &&
            (current->flags & 0x80000000) != 0 &&
            type == (current->flags & 0x0FFFFFFF))
        {
            if (current->u.jobj == NULL) {
                __assert("robj.c", 0x1F2, "rp->u.jobj");
            }
            HSD_JObjSetupMatrix(current->u.jobj);
            count++;
            position.x += current->u.jobj->mtx[0][3];
            position.y += current->u.jobj->mtx[1][3];
            position.z += current->u.jobj->mtx[2][3];
        }
    }

    if (count != 0) {
        f32 reciprocal = 1.0F / count;

        out->x = reciprocal * position.x;
        out->y = reciprocal * position.y;
        out->z = reciprocal * position.z;
    }
    return count;
}

static inline void set_dirup_matrix(Vec* dir, Vec* up, Vec* scale,
                                    HSD_JObj* jobj,
                                    HSD_ObjUpdateFunc update)
{
    Vec z;
    Vec value;
    f32 z_scale;
    f32 dir_scale;

    PSVECCrossProduct(dir, up, &z);
    ROBJ_SQRTF_INLINE(
        dir_scale,
        1.0F /
            (1.00000001335e-10F + PSVECDotProduct(dir, dir)));
    PSVECScale(dir, dir, dir_scale);
    ROBJ_SQRTF_INLINE(
        z_scale,
        1.0F /
            (1.00000001335e-10F + PSVECDotProduct(&z, &z)));
    PSVECScale(&z, &z, z_scale);
    PSVECCrossProduct(&z, dir, up);

    value.x = dir->x * scale->x;
    value.y = dir->y * scale->x;
    value.z = dir->z * scale->x;
    update(jobj, 0x32, (HSD_ObjData*) &value);
    value.x = up->x * scale->y;
    value.y = up->y * scale->y;
    value.z = up->z * scale->y;
    update(jobj, 0x33, (HSD_ObjData*) &value);
    value.x = z.x * scale->z;
    value.y = z.y * scale->z;
    value.z = z.z * scale->z;
    update(jobj, 0x34, (HSD_ObjData*) &value);
    update(jobj, 0x37, NULL);
}

void fn_801AF560(HSD_RObj* robj, HSD_JObj* jobj,
                 HSD_ObjUpdateFunc update)
{
    Vec scale = { 1.0F, 1.0F, 1.0F };
    Vec up = { 0.0F, 1.0F, 0.0F };
    Vec position;
    Vec direction;
    f32 k;

    if (RObjGetGlobalPosition(robj, 2, &position) != 0) {
        direction.x = jobj->mtx[0][3];
        direction.y = jobj->mtx[1][3];
        direction.z = jobj->mtx[2][3];
        PSVECSubtract(&position, &direction, &position);

        if (RObjGetGlobalPosition(robj, 3, &up) != 0) {
            PSVECSubtract(&up, &direction, &up);
        } else {
            k = 1.0F - PSVECDotProduct(&position, &up);
            if (robj_absf(k) < 1.00000001335e-10F) {
                up.x = 0.0F;
                up.y = 0.0F;
                up.z = 1.0F;
            }
        }
        if (jobj->scl != NULL) {
            scale = *(Vec*) jobj->scl;
        }
        set_dirup_matrix(&position, &up, &scale, jobj, update);
    }
}

static inline HSD_RObj* RObjGetByType(HSD_RObj* robj, u32 type,
                                      u32 subtype)
{
    HSD_RObj* current;

    for (current = robj; current != NULL; current = current->next) {
        if ((current->flags & 0x80000000) != 0 &&
            (current->flags & ROBJ_TYPE_MASK) == type &&
            (subtype == 0 || subtype == (current->flags & 0x0FFFFFFF)))
        {
            return current;
        }
    }
    return NULL;
}

static inline f32 MtxColMag(MtxPtr mtx, s32 column)
{
    return robj_sqrtf(mtx[0][column] * mtx[0][column] +
                      mtx[1][column] * mtx[1][column] +
                      mtx[2][column] * mtx[2][column]);
}

void fn_801AF224(HSD_RObj* robj, HSD_JObj* jobj,
                 HSD_ObjUpdateFunc update)
{
    static s32 attributes[3] = { 0x32, 0x33, 0x34 };
    Mtx mtx;
    f32 magnitude;
    Vec value;
    HSD_RObj* orientation;
    s32 i;

    if (jobj == NULL) {
        __assert("robj.c", 0x276, "obj");
    }
    orientation = RObjGetByType(robj, REFTYPE_JOBJ, 4);
    if (orientation == NULL) {
        return;
    }

    if (orientation->u.jobj == NULL) {
        __assert("jobj.h", 0x47C, "jobj");
    }
    HSD_JObjSetupMatrix(orientation->u.jobj);
    PSMTXCopy(orientation->u.jobj->mtx, mtx);

    for (i = 0; i < 3; i++) {
        value.x = mtx[0][i];
        value.y = mtx[1][i];
        value.z = mtx[2][i];
        magnitude = PSVECMag(&value);
        if (magnitude > 1.00000001335e-10F) {
            magnitude = 1.0F / magnitude;
        }
        magnitude *= MtxColMag(jobj->mtx, i);
        value.x *= magnitude;
        value.y *= magnitude;
        value.z *= magnitude;
        update(jobj, attributes[i], (HSD_ObjData*) &value);
    }
    update(jobj, 0x37, NULL);
}

void fn_801AEBE4(HSD_RObj* robj, HSD_RObjDesc* desc)
{
    for (; robj != NULL && desc != NULL;
         robj = robj->next, desc = desc->next)
    {
        if ((robj->flags & ROBJ_TYPE_MASK) == REFTYPE_JOBJ) {
            HSD_JObjUnrefThis(robj->u.jobj);
            robj->u.jobj =
                HSD_IDGetDataFromTable(NULL, (u32) desc->u.joint, NULL);
            if (robj->u.jobj == NULL) {
                __assert("robj.c", 0x330, "robj->u.jobj");
            }
            RObjJObjRefThis(robj->u.jobj);
        } else if ((robj->flags & ROBJ_TYPE_MASK) == REFTYPE_EXP) {
            HSD_Rvalue* rvalue = robj->u.exp.rvalue;
            HSD_RvalueList* list = desc->u.exp->rvalue;

            while (rvalue != NULL && list->joint != NULL) {
                HSD_JObjUnrefThis(rvalue->jobj);
                rvalue->jobj =
                    HSD_IDGetDataFromTable(NULL, (u32) list->joint, NULL);
                if (rvalue->jobj == NULL) {
                    __assert("robj.c", 0x4F2, "rvalue->jobj");
                }
                RObjJObjRefThis(rvalue->jobj);
                rvalue = rvalue->next;
                list++;
            }
        }
    }
}

void fn_801AED88(HSD_RObj* robj, HSD_JObj* jobj,
                 HSD_ObjUpdateFunc update)
{
    HSD_RObj* current;
    Vec position;

    if (robj == NULL) {
        return;
    }

    if (RObjGetGlobalPosition(robj, 1, &position) != 0) {
        update(jobj, 0x35, (HSD_ObjData*) &position);
        update(jobj, 0x38, NULL);
    }
    fn_801AF560(robj, jobj, update);
    fn_801AF224(robj, jobj, update);
    fn_801AEFF0(robj, jobj);

    for (current = robj; current != NULL; current = current->next) {
        if ((current->flags & ROBJ_TYPE_MASK) == REFTYPE_EXP &&
            (current->flags & 0x80000000) != 0)
        {
            fn_801AE008(&current->u.exp, current->flags & 0x0FFFFFFF,
                        jobj, update);
        }
    }
}

void fn_801AEFF0(HSD_RObj* robj, HSD_JObj* jobj)
{
    HSD_RObj* current;
    BOOL changed = FALSE;

    if (jobj == NULL) {
        __assert("robj.c", 0x29E, "jobj");
    }

    for (current = robj; current != NULL; current = current->next) {
        if ((current->flags & ROBJ_TYPE_MASK) != REFTYPE_LIMIT) {
            continue;
        }

        switch (current->flags & 0x0FFFFFFF) {
        case 1:
            if (jobj->rotate_x < current->u.limit) {
                jobj->rotate_x = current->u.limit;
            }
            break;
        case 2:
            if (jobj->rotate_x > current->u.limit) {
                jobj->rotate_x = current->u.limit;
            }
            break;
        case 3:
            if (jobj->rotate_y < current->u.limit) {
                jobj->rotate_y = current->u.limit;
            }
            break;
        case 4:
            if (jobj->rotate_y > current->u.limit) {
                jobj->rotate_y = current->u.limit;
            }
            break;
        case 5:
            if (jobj->rotate_z < current->u.limit) {
                jobj->rotate_z = current->u.limit;
            }
            break;
        case 6:
            if (jobj->rotate_z > current->u.limit) {
                jobj->rotate_z = current->u.limit;
            }
            break;
        case 7:
            if (jobj->translate_x < current->u.limit) {
                jobj->translate_x = current->u.limit;
            }
            break;
        case 8:
            if (jobj->translate_x > current->u.limit) {
                jobj->translate_x = current->u.limit;
            }
            break;
        case 9:
            if (jobj->translate_y < current->u.limit) {
                jobj->translate_y = current->u.limit;
            }
            break;
        case 10:
            if (jobj->translate_y > current->u.limit) {
                jobj->translate_y = current->u.limit;
            }
            break;
        case 11:
            if (jobj->translate_y < current->u.limit) {
                jobj->translate_y = current->u.limit;
            }
            break;
        case 12:
            if (jobj->translate_y > current->u.limit) {
                jobj->translate_y = current->u.limit;
            }
            break;
        default:
            continue;
        }
        changed = TRUE;
    }

    if (changed) {
        HSD_JObjMakeMatrix(jobj);
    }
}

s32 fn_801AFCAC(HSD_RObj* robj, u32 type, Vec* out)
{
    HSD_RObj* current;
    Vec position = { 0.0f, 0.0f, 0.0f };
    s32 count = 0;

    if (robj == NULL) {
        return 0;
    }

    for (current = robj; current != NULL; current = current->next) {
        if ((current->flags & ROBJ_TYPE_MASK) == REFTYPE_JOBJ &&
            (current->flags & 0x80000000) != 0 &&
            type == (current->flags & 0x0FFFFFFF))
        {
            if (current->u.jobj == NULL) {
                __assert("robj.c", 0x1F2, "rp->u.jobj");
            }
            HSD_JObjSetupMatrix(current->u.jobj);
            count++;
            position.x += current->u.jobj->mtx[0][3];
            position.y += current->u.jobj->mtx[1][3];
            position.z += current->u.jobj->mtx[2][3];
        }
    }

    if (count != 0) {
        f32 reciprocal = 1.0f / count;

        out->x = reciprocal * position.x;
        out->y = reciprocal * position.y;
        out->z = reciprocal * position.z;
    }
    return count;
}

static inline void RObjAddAnim(HSD_RObj* robj, HSD_RObjAnimJoint* anim)
{
    if (robj == NULL || anim == NULL) {
        return;
    }
    if (robj->aobj != NULL) {
        HSD_AObjRemove(robj->aobj);
    }
    robj->aobj = HSD_AObjLoadDesc(anim->aobjdesc);
}

void fn_801AFE68(HSD_RObj* robj, HSD_RObjAnimJoint* anim)
{
    HSD_RObj* i;
    HSD_RObjAnimJoint* j;

    if (robj == NULL || anim == NULL) {
        return;
    }
    for (i = robj, j = anim; i != NULL && j != NULL;
         i = i->next, j = j->next)
    {
        RObjAddAnim(i, j);
    }
}

static inline void RObjReqAnim(HSD_RObj* robj, f32 frame)
{
    if (robj != NULL && robj->aobj != NULL) {
        HSD_AObjReqAnim(robj->aobj, frame);
    }
}

void fn_801AFEFC(HSD_RObj* robj, f32 frame)
{
    for (; robj != NULL; robj = robj->next) {
        RObjReqAnim(robj, frame);
    }
}

static inline void RObjReqAnimByFlags(HSD_RObj* robj, f32 frame, u32 flags)
{
    if (robj != NULL && robj->aobj != NULL && (flags & 0x80) != 0) {
        HSD_AObjReqAnim(robj->aobj, frame);
    }
}

void fn_801AFF64(HSD_RObj* robj, f32 frame, u32 flags)
{
    for (; robj != NULL; robj = robj->next) {
        RObjReqAnimByFlags(robj, frame, flags);
    }
}

static inline void RObjRemoveAnim(HSD_RObj* robj)
{
    if (robj != NULL && robj->aobj != NULL) {
        HSD_AObjRemove(robj->aobj);
        robj->aobj = NULL;
    }
}

void fn_801AFFE0(HSD_RObj* robj)
{
    for (; robj != NULL; robj = robj->next) {
        RObjRemoveAnim(robj);
    }
}

extern void RObjUpdateFunc(void* obj, u32 type, HSD_ObjData* val);

static inline void RObjAnim(HSD_RObj* robj)
{
    if (robj != NULL) {
        HSD_AObjInterpretAnim(robj->aobj, robj, RObjUpdateFunc);
    }
}

void fn_801B0040(HSD_RObj* robj)
{
    if (robj != NULL) {
        for (; robj != NULL; robj = robj->next) {
            RObjAnim(robj);
        }
    }
}
