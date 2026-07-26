#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_jobj.h"
#include "hsd/hsd_robj.h"

extern u8 lbl_80465688[];
extern u8 lbl_804656B4[];
extern f32* lbl_8047B308;
extern u32 lbl_8047B30C;
extern void* HSD_ObjAlloc(void* alloc_data);
extern void HSD_ObjFree(void* alloc_data, void* object);
extern void* memset(void* dst, int value, u32 size);
extern void* fn_801A6928(u32 size);
extern s32 HSD_GetNbBits(u32 value);
extern void OSReport(const char* format, ...);
extern void HSD_Panic(const char* file, u32 line, const char* message);
extern f32 fn_801AE000(void*);
extern f32 HSD_ByteCodeEval(u8* bytecode, f32* args, s32 count);
extern void HSD_JObjSetupMatrix(HSD_JObj* jobj);
extern void HSD_MtxGetRotation(f32 mtx[3][4], Vec* rotation);
extern void HSD_MtxGetScale(f32 mtx[3][4], Vec* scale);

void fn_801AE008(HSD_Exp* exp, u32 type, void* object,
                 HSD_ObjUpdateFunc update)
{
    HSD_Rvalue* rvalue;
    f32* arg;

    if (exp->nb_args == -1) {
        u32 count = 0;

        for (rvalue = exp->rvalue; rvalue != NULL; rvalue = rvalue->next) {
            count += HSD_GetNbBits(rvalue->flags);
        }
        exp->nb_args = count;
    }

    if (lbl_8047B308 == NULL) {
        if (lbl_8047B30C == 0) {
            lbl_8047B30C = 100;
        }
        lbl_8047B308 = fn_801A6928(lbl_8047B30C * sizeof(f32));
    }
    if (lbl_8047B30C < exp->nb_args) {
        OSReport(
            "Number of argment of expression exceeds the argument buffer\n"
            "size. (requested num of arg %d, allocated %d)\n",
            exp->nb_args, lbl_8047B30C);
        HSD_Panic("robj.c", 0x41B, "");
    }

    arg = lbl_8047B308;
    for (rvalue = exp->rvalue; rvalue != NULL; rvalue = rvalue->next) {
        HSD_JObj* jobj = rvalue->jobj;
        u32 bit;

        if (jobj == NULL) {
            __assert("robj.c", 0x424, "jobj");
        }
        HSD_JObjSetupMatrix(jobj);
        for (bit = 1; bit != 0 && bit <= rvalue->flags; bit <<= 1) {
            Vec rotation;
            Vec scale;

            switch (rvalue->flags & bit) {
            case 0x1:
                *arg++ = 57.29578f * jobj->rotate_x;
                break;
            case 0x2:
                *arg++ = 57.29578f * jobj->rotate_y;
                break;
            case 0x4:
                *arg++ = 57.29578f * jobj->rotate_z;
                break;
            case 0x10:
                *arg++ = jobj->translate_x;
                break;
            case 0x20:
                *arg++ = jobj->translate_y;
                break;
            case 0x40:
                *arg++ = jobj->translate_z;
                break;
            case 0x80:
                *arg++ = jobj->scale_x;
                break;
            case 0x100:
                *arg++ = jobj->scale_y;
                break;
            case 0x200:
                *arg++ = jobj->scale_z;
                break;
            case 0x10000:
                HSD_MtxGetRotation(jobj->mtx, &rotation);
                *arg++ = 57.29578f * rotation.x;
                break;
            case 0x20000:
                HSD_MtxGetRotation(jobj->mtx, &rotation);
                *arg++ = 57.29578f * rotation.y;
                break;
            case 0x40000:
                HSD_MtxGetRotation(jobj->mtx, &rotation);
                *arg++ = 57.29578f * rotation.z;
                break;
            case 0x100000:
                *arg++ = jobj->mtx[0][3];
                break;
            case 0x200000:
                *arg++ = jobj->mtx[1][3];
                break;
            case 0x400000:
                *arg++ = jobj->mtx[2][3];
                break;
            case 0x800000:
                HSD_MtxGetScale(jobj->mtx, &scale);
                *arg++ = scale.x;
                break;
            case 0x1000000:
                HSD_MtxGetScale(jobj->mtx, &scale);
                *arg++ = scale.y;
                break;
            case 0x2000000:
                HSD_MtxGetScale(jobj->mtx, &scale);
                *arg++ = scale.z;
                break;
            }
        }
    }

    {
        HSD_ObjData value;

        if (exp->is_bytecode) {
            value.fv = HSD_ByteCodeEval(exp->expr.bytecode, lbl_8047B308,
                                        exp->nb_args);
        } else {
            value.fv = exp->expr.func(lbl_8047B308);
        }
        if (type - 1 <= 2) {
            value.fv *= 0.017453292f;
        }
        update(object, type, &value);
    }
}

HSD_RObj* fn_801AE4B0(void)
{
    HSD_RObj* robj = HSD_ObjAlloc(lbl_804656B4);

    if (robj == NULL) {
        __assert("robj.c", 0x3C5, "new");
    }
    memset(robj, 0, sizeof(HSD_RObj));
    return robj;
}

void fn_801AE50C(HSD_RObj* robj)
{
    HSD_RObj* next;

    for (; robj != NULL; robj = next) {
        HSD_Rvalue* rvalue;
        HSD_Rvalue* rvalue_next;

        next = robj->next;
        if ((robj->flags & ROBJ_TYPE_MASK) == REFTYPE_JOBJ) {
            HSD_JObjUnrefThis(robj->u.jobj);
        } else if ((robj->flags & ROBJ_TYPE_MASK) == REFTYPE_EXP) {
            for (rvalue = robj->u.exp.rvalue; rvalue != NULL;
                 rvalue = rvalue_next)
            {
                rvalue_next = rvalue->next;
                HSD_JObjUnrefThis(rvalue->jobj);
                HSD_ObjFree(lbl_80465688, rvalue);
            }
        }
        HSD_AObjRemove(robj->aobj);
        HSD_ObjFree(lbl_804656B4, robj);
    }
}

/*
 * These helpers precede HSD_RObjLoadDesc in the original robj.c.  Keeping
 * their definitions visible here restores the original auto-inlining lost at
 * the candidate object's address split.
 */
static HSD_Rvalue* RObjLoadRvalueDesc(HSD_RvalueList* list)
{
    HSD_Rvalue* result = NULL;
    HSD_Rvalue** cursor = &result;

    if (list == NULL) {
        return NULL;
    }

    while (list->joint != NULL) {
        HSD_Rvalue* rvalue = HSD_ObjAlloc(lbl_80465688);

        if (rvalue == NULL) {
            __assert("robj.c", 0x485, "rvalue");
        }
        memset(rvalue, 0, sizeof(HSD_Rvalue));
        *cursor = rvalue;
        rvalue->flags = list->flags;
        list++;
        cursor = &rvalue->next;
    }
    return result;
}

static void RObjExpLoadDesc(HSD_Exp* exp, HSD_ExpDesc* desc)
{
    memset(exp, 0, sizeof(HSD_Exp));
    if (desc != NULL) {
        if (desc->func != NULL) {
            exp->expr.func = desc->func;
        } else {
            exp->expr.func = fn_801AE000;
        }
        exp->rvalue = RObjLoadRvalueDesc(desc->rvalue);
        exp->nb_args = -1;
    }
}

static void RObjByteCodeExpLoadDesc(HSD_Exp* exp,
                                    HSD_ByteCodeExpDesc* desc)
{
    memset(exp, 0, sizeof(HSD_Exp));
    if (desc != NULL) {
        if (desc->bytecode != NULL) {
            exp->expr.bytecode = desc->bytecode;
        } else {
            exp->expr.bytecode = NULL;
        }
        exp->rvalue = RObjLoadRvalueDesc(desc->rvalue);
        exp->nb_args = -1;
        exp->is_bytecode = 1;
    }
}

HSD_RObj* HSD_RObjLoadDesc(HSD_RObjDesc* desc)
{
    HSD_RObj* robj;

    if (desc == NULL) {
        return NULL;
    }

    robj = fn_801AE4B0();
    robj->next = HSD_RObjLoadDesc(desc->next);
    robj->flags = desc->flags;

    switch (robj->flags & ROBJ_TYPE_MASK) {
    case REFTYPE_JOBJ:
        break;
    case REFTYPE_LIMIT:
        switch (robj->flags & 0x0FFFFFFF) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            robj->u.limit = 0.017453292f * desc->u.limit;
            break;
        default:
            robj->u.limit = desc->u.limit;
            break;
        }
        break;
    case REFTYPE_EXP:
        RObjExpLoadDesc(&robj->u.exp, desc->u.exp);
        break;
    case REFTYPE_BYTECODE:
        RObjByteCodeExpLoadDesc(&robj->u.exp, desc->u.bcexp);
        robj->flags &= ~ROBJ_TYPE_MASK;
        break;
    case REFTYPE_IKHINT:
        robj->u.ik_hint.bone_length = desc->u.ik_hint->bone_length;
        robj->u.ik_hint.rotate_x = desc->u.ik_hint->rotate_x;
        break;
    default:
        HSD_Panic("robj.c", 0x37D, "unexpected type of robj.\n");
        break;
    }
    return robj;
}
