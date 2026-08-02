/**
 * @file hsd_range_801920E4.c
 * @brief hsd code, 0x801920E4 - 0x801938FC (4 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"
#include "crt/math.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_forward.h"

extern HSD_ClassInfo* lbl_8047B228;
extern void* HSD_HashSearch(void* table, void* key, u32* found);

/* ------------------------------------------------------------------------ */
/*  bytecode.c - HSD byte code interpreter (sysdolphin baselib)              */
/*                                                                           */
/*  The retail assert/panic line numbers embedded in this range (281, 299,   */
/*  307, 323, 326, 339, 376 ... 693) reproduce the sysdolphin bytecode.c     */
/*  source layout one-for-one, so the file name literal below is the         */
/*  original translation unit name rather than this split file's name.       */
/* ------------------------------------------------------------------------ */

extern void __assert(const char* file, u32 line, const char* expr);
extern void HSD_Panic(const char* file, u32 line, const char* msg);
extern void OSReport(const char* fmt, ...);

extern HSD_SList* fn_801A3E64(HSD_SList* node);   /* HSD_SListRemove  */
extern HSD_SList* HSD_SListPrepend(HSD_SList* next, void* data);
extern s32 fn_801ADC3C(s32 range);                /* HSD_Randi        */
extern f32 fn_801ADC7C(void);                     /* HSD_Randf        */

extern f64 sin(f64 x);
extern f64 cos(f64 x);
extern f64 tan(f64 x);
extern f64 asin(f64 x);
extern f64 acos(f64 x);
extern f64 atan(f64 x);
extern f64 log(f64 x);
extern f64 exp(f64 x);
extern f64 pow(f64 x, f64 y);
extern f64 fmod(f64 x, f64 y);

#define BC_FILE "bytecode.c"
#define BC_ASSERT(line, cond) \
    ((cond) ? ((void) 0) : __assert(BC_FILE, line, #cond))
#define BC_ASSERTMSG(line, cond, msg) \
    ((cond) ? ((void) 0) : __assert(BC_FILE, line, msg))

#define BC_DEG_TO_RAD 0.017453292519943295
#define BC_RAD_TO_DEG 57.29577951308232

typedef union ByteCodeVal {
    void* p;
    int i;
    f32 f;
} ByteCodeVal;

/*
 * MSL math.h supplies these as inlines. The target expands both at the
 * bytecode square-root opcode: three reciprocal-square-root refinements,
 * followed by the standard float classification fallback.
 */
static inline s32 __fpclassifyf(f32 value)
{
    switch (*(s32*) &value & 0x7F800000) {
    case 0x7F800000: {
        if (*(s32*) &value & 0x007FFFFF) {
            return 1;
        } else {
            return 2;
        }
        break;
    }
    case 0: {
        if (*(s32*) &value & 0x007FFFFF) {
            return 5;
        } else {
            return 3;
        }
        break;
    }
    }
    return 4;
}

static inline f32 sqrtf(f32 value)
{
    if (value > 0.0F) {
        f64 guess = __frsqrte(value);
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        return (f32) (value * guess);
    }
    if ((f64) value < 0.0) {
        return lbl_80478AC0[0];
    }
    if (__fpclassifyf(value) == 1) {
        return lbl_80478AC0[0];
    }
    return value;
}

f32 HSD_ByteCodeEval(u8* bytecode, f32* args, s32 nb_args)
{
    HSD_SList* stack;
    int i;
    u8 last_command;
    s32 operand_count;
    u32 operand;
    HSD_SList* list;
    f32 fv, f0, f1;
    s32 d0, d1;

    stack = NULL;
    operand_count = 0;

    if (bytecode == NULL) {
        return 0.0f;
    }

    for (;;) {
        if (operand_count > 0) {
            operand_count--;
            operand = (operand << 8) | *bytecode;
            bytecode++;

            if (operand_count != 0) {
                continue;
            }

            switch (last_command) {
            case 2:
                BC_ASSERT(281, operand < nb_args);
                stack = HSD_SListPrepend(
                    stack, (void*) ((ByteCodeVal*) &args[operand])->i);
                break;
            case 5:
                for (i = 0; i < operand; i++) {
                    stack = fn_801A3E64(stack);
                }
                break;
            case 0x3C:
                list = stack;
                i = 0;
                while (list != NULL && i < operand) {
                    list = list->next;
                    i++;
                }
                if (list == NULL) {
                    OSReport("specified stack doesn't exist (%d).\n", operand);
                    HSD_Panic(BC_FILE, 299, "");
                } else {
                    stack = HSD_SListPrepend(stack, list->data);
                }
                break;
            case 3:
                BC_ASSERT(307, stack);
                if ((int) stack->data != 0) {
                    bytecode += operand;
                }
                stack = fn_801A3E64(stack);
                break;
            case 4:
                bytecode += operand;
                break;
            case 6:
                stack = HSD_SListPrepend(stack, (void*) operand);
                break;
            case 0xFF:
                HSD_Panic(BC_FILE, 323, "not yet implemented.\n");
                /* fallthrough */
            default:
                HSD_Panic(BC_FILE, 326, "unexpected byte code.\n");
                break;
            }
            continue;
        }

        last_command = *bytecode++;
        switch (last_command) {
        case 0:
            break;
        case 1:
            BC_ASSERT(339, stack);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            while (stack != NULL) {
                stack = fn_801A3E64(stack);
            }
            return f0;
        case 5:
        case 0x3C:
        case 0xFF:
            operand_count = 1;
            operand = 0;
            break;
        case 2:
        case 3:
        case 4:
            operand_count = 2;
            operand = 0;
            break;
        case 6:
            operand_count = 4;
            operand = 0;
            break;
        case 7:
            BC_ASSERT(376, stack);
            ((ByteCodeVal*) &stack->data)->i =
                (int) ((ByteCodeVal*) &stack->data)->f;
            break;
        case 8:
            BC_ASSERT(381, stack);
            fv = (f32) ((ByteCodeVal*) &stack->data)->i;
            stack->data = *(void**) &fv;
            break;
        case 9:
            BC_ASSERT(387, stack);
            fv = -((ByteCodeVal*) &stack->data)->f;
            stack->data = *(void**) &fv;
            break;
        case 0x0A:
            BC_ASSERT(393, stack);
            ((ByteCodeVal*) &stack->data)->i =
                -((ByteCodeVal*) &stack->data)->i;
            break;
        case 0x0B:
            BC_ASSERT(399, stack);
            ((ByteCodeVal*) &stack->data)->i = fn_801ADC3C(2);
            break;
        case 0x0C:
            BC_ASSERT(405, stack);
            fv = fn_801ADC7C();
            stack->data = *(void**) &fv;
            break;
        case 0x0D:
            BC_ASSERT(411, stack);
            fv = (f32) sin((f32) (BC_DEG_TO_RAD *
                                 (f64) ((ByteCodeVal*) &stack->data)->f));
            stack->data = *(void**) &fv;
            break;
        case 0x0E:
            BC_ASSERT(417, stack);
            fv = (f32) cos((f32) (BC_DEG_TO_RAD *
                                 (f64) ((ByteCodeVal*) &stack->data)->f));
            stack->data = *(void**) &fv;
            break;
        case 0x0F:
            BC_ASSERT(423, stack);
            fv = (f32) tan((f32) (BC_DEG_TO_RAD *
                                 (f64) ((ByteCodeVal*) &stack->data)->f));
            stack->data = *(void**) &fv;
            break;
        case 0x10:
            BC_ASSERT(429, stack);
            fv = (f32) (BC_RAD_TO_DEG *
                        (f32) asin(((ByteCodeVal*) &stack->data)->f));
            stack->data = *(void**) &fv;
            break;
        case 0x11:
            BC_ASSERT(435, stack);
            fv = (f32) (BC_RAD_TO_DEG *
                        (f32) acos(((ByteCodeVal*) &stack->data)->f));
            stack->data = *(void**) &fv;
            break;
        case 0x12:
            BC_ASSERT(441, stack);
            fv = (f32) (BC_RAD_TO_DEG *
                        (f32) atan(((ByteCodeVal*) &stack->data)->f));
            stack->data = *(void**) &fv;
            break;
        case 0x13:
            BC_ASSERT(447, stack);
            fv = (f32) log(((ByteCodeVal*) &stack->data)->f);
            stack->data = *(void**) &fv;
            break;
        case 0x14:
            BC_ASSERT(453, stack);
            fv = (f32) exp(((ByteCodeVal*) &stack->data)->f);
            stack->data = *(void**) &fv;
            break;
        case 0x15:
            BC_ASSERT(459, stack);
            if (((ByteCodeVal*) &stack->data)->f < 0.0f) {
                fv = -((ByteCodeVal*) &stack->data)->f;
                stack->data = *(void**) &fv;
            }
            break;
        case 0x28:
            BC_ASSERTMSG(467, stack, "stack");
            d0 = ((ByteCodeVal*) &stack->data)->i;
            if (d0 < 0) {
                ((ByteCodeVal*) &stack->data)->i = -d0;
            }
            break;
        case 0x16:
            BC_ASSERT(474, stack);
            fv = sqrtf(((ByteCodeVal*) &stack->data)->f);
            stack->data = *(void**) &fv;
            break;
        case 0x31:
            BC_ASSERTMSG(480, stack, "stack");
            stack->data = (void*) !(s32) stack->data;
            break;
        case 0x17:
            BC_ASSERT(501, stack);
            BC_ASSERTMSG(501, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            fv = ((ByteCodeVal*) &stack->data)->f + f0;
            stack->data = *(void**) &fv;
            break;
        case 0x18:
            BC_ASSERT(507, stack);
            BC_ASSERTMSG(507, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            fv = ((ByteCodeVal*) &stack->data)->f - f0;
            stack->data = *(void**) &fv;
            break;
        case 0x19:
            BC_ASSERT(513, stack);
            BC_ASSERTMSG(513, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            fv = ((ByteCodeVal*) &stack->data)->f * f0;
            stack->data = *(void**) &fv;
            break;
        case 0x1A:
            BC_ASSERT(519, stack);
            BC_ASSERTMSG(519, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            fv = ((ByteCodeVal*) &stack->data)->f / f0;
            stack->data = *(void**) &fv;
            break;
        case 0x1B:
            BC_ASSERT(525, stack);
            BC_ASSERTMSG(525, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            fv = (f32) fmod(((ByteCodeVal*) &stack->data)->f, f0);
            stack->data = *(void**) &fv;
            break;
        case 0x1C:
            BC_ASSERT(531, stack);
            BC_ASSERTMSG(531, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            ((ByteCodeVal*) &stack->data)->i =
                ((ByteCodeVal*) &stack->data)->i + d1;
            break;
        case 0x1D:
            BC_ASSERT(536, stack);
            BC_ASSERTMSG(536, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            ((ByteCodeVal*) &stack->data)->i =
                ((ByteCodeVal*) &stack->data)->i - d1;
            break;
        case 0x1E:
            BC_ASSERT(541, stack);
            BC_ASSERTMSG(541, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            ((ByteCodeVal*) &stack->data)->i =
                ((ByteCodeVal*) &stack->data)->i * d1;
            break;
        case 0x1F:
            BC_ASSERT(546, stack);
            BC_ASSERTMSG(546, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            ((ByteCodeVal*) &stack->data)->i =
                ((ByteCodeVal*) &stack->data)->i / d1;
            break;
        case 0x20:
            BC_ASSERT(551, stack);
            BC_ASSERTMSG(551, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 % d1;
            break;
        case 0x21:
            BC_ASSERT(556, stack);
            BC_ASSERTMSG(556, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            fv = (f32) pow(((ByteCodeVal*) &stack->data)->f, f0);
            stack->data = *(void**) &fv;
            break;
        case 0x22:
            BC_ASSERT(562, stack);
            BC_ASSERTMSG(562, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            if (((ByteCodeVal*) &stack->data)->f > f0) {
                stack->data = *(void**) &f0;
            }
            break;
        case 0x23:
            BC_ASSERT(569, stack);
            BC_ASSERTMSG(569, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            if (((ByteCodeVal*) &stack->data)->f < f0) {
                stack->data = *(void**) &f0;
            }
            break;
        case 0x24:
            BC_ASSERT(576, stack);
            BC_ASSERTMSG(576, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            if (((ByteCodeVal*) &stack->data)->i > d1) {
                ((ByteCodeVal*) &stack->data)->i = d1;
            }
            break;
        case 0x25:
            BC_ASSERT(583, stack);
            BC_ASSERTMSG(583, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            if (((ByteCodeVal*) &stack->data)->i < d1) {
                ((ByteCodeVal*) &stack->data)->i = d1;
            }
            break;
        case 0x26:
            BC_ASSERT(590, stack);
            BC_ASSERTMSG(590, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = (f32) (BC_RAD_TO_DEG *
                        (0.0f == f0 ? (f1 >= 0.0f ? 1.5707964f : -1.5707964f)
                                    : (f32) atan2(f1, f0)));
            stack->data = *(void**) &fv;
            break;
        case 0x33:
            BC_ASSERT(596, stack);
            BC_ASSERTMSG(596, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            ((ByteCodeVal*) &stack->data)->i = f1 < f0;
            break;
        case 0x34:
            BC_ASSERT(601, stack);
            BC_ASSERTMSG(601, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            ((ByteCodeVal*) &stack->data)->i = f1 > f0;
            break;
        case 0x35:
            BC_ASSERT(606, stack);
            BC_ASSERTMSG(606, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            ((ByteCodeVal*) &stack->data)->i = f1 <= f0;
            break;
        case 0x36:
            BC_ASSERT(611, stack);
            BC_ASSERTMSG(611, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            ((ByteCodeVal*) &stack->data)->i = f1 >= f0;
            break;
        case 0x37:
            BC_ASSERT(616, stack);
            BC_ASSERTMSG(616, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            ((ByteCodeVal*) &stack->data)->i = f1 == f0;
            break;
        case 0x38:
            BC_ASSERT(621, stack);
            BC_ASSERTMSG(621, stack->next, "stack->next");
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = fn_801A3E64(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            ((ByteCodeVal*) &stack->data)->i = f1 != f0;
            break;
        case 0x29:
            BC_ASSERT(626, stack);
            BC_ASSERTMSG(626, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            ((ByteCodeVal*) &stack->data)->i =
                ((ByteCodeVal*) &stack->data)->i < d1;
            break;
        case 0x2A:
            BC_ASSERT(631, stack);
            BC_ASSERTMSG(631, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            ((ByteCodeVal*) &stack->data)->i =
                ((ByteCodeVal*) &stack->data)->i > d1;
            break;
        case 0x2B:
            BC_ASSERT(636, stack);
            BC_ASSERTMSG(636, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            ((ByteCodeVal*) &stack->data)->i =
                ((ByteCodeVal*) &stack->data)->i <= d1;
            break;
        case 0x2C:
            BC_ASSERT(641, stack);
            BC_ASSERTMSG(641, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            ((ByteCodeVal*) &stack->data)->i =
                ((ByteCodeVal*) &stack->data)->i >= d1;
            break;
        case 0x2D:
            BC_ASSERT(646, stack);
            BC_ASSERTMSG(646, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            ((ByteCodeVal*) &stack->data)->i =
                ((ByteCodeVal*) &stack->data)->i == d1;
            break;
        case 0x2E:
            BC_ASSERT(651, stack);
            BC_ASSERTMSG(651, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            ((ByteCodeVal*) &stack->data)->i =
                ((ByteCodeVal*) &stack->data)->i != d1;
            break;
        case 0x2F:
            BC_ASSERT(656, stack);
            BC_ASSERTMSG(656, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 != 0 && d1 != 0;
            break;
        case 0x30:
            BC_ASSERT(661, stack);
            BC_ASSERTMSG(661, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 != 0 || d1 != 0;
            break;
        case 0x32:
            BC_ASSERT(666, stack);
            BC_ASSERTMSG(666, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i =
                (d0 == 0 && d1 != 0) || (d0 != 0 && d1 == 0);
            break;
        case 0x39:
            BC_ASSERT(671, stack);
            BC_ASSERTMSG(671, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            stack->data = (void*) (((ByteCodeVal*) &stack->data)->i & d1);
            break;
        case 0x3A:
            BC_ASSERT(676, stack);
            BC_ASSERTMSG(676, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            stack->data = (void*) (((ByteCodeVal*) &stack->data)->i | d1);
            break;
        case 0x3B:
            BC_ASSERT(681, stack);
            BC_ASSERTMSG(681, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            stack->data = (void*) (((ByteCodeVal*) &stack->data)->i ^ d1);
            break;
        case 0x27:
            BC_ASSERT(687, stack);
            BC_ASSERTMSG(687, stack->next, "stack->next");
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = fn_801A3E64(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 + fn_801ADC3C((d1 - d0) + 1);
            break;
        default:
            OSReport("unexpected opcode 0x%x.\n", last_command);
            HSD_Panic(BC_FILE, 693, "");
            break;
        }
    }
}

HSD_ClassInfo* fn_80193748(const char* class_name)
{
    HSD_ClassInfo** hashTable;

    hashTable = &lbl_8047B228;
    if ((class_name && class_name) && class_name) {
        /* Preserve the original MWCC register allocation. */
    }
    if (*hashTable != NULL) {
        return (HSD_ClassInfo*) HSD_HashSearch((void*) lbl_8047B228, (void*) class_name, NULL);
    }
    return NULL;
}

BOOL fn_80193788(void* info, void* p)
{
    HSD_ClassInfo* c;
    HSD_ClassInfo* parent;

    c = (HSD_ClassInfo*)p;
    if (info == NULL || c == NULL) {
        return FALSE;
    }

    parent = c;
    c = (HSD_ClassInfo*)info;

    if (!(((HSD_ClassInfo*)info)->head.flags & 1)) {
        c->head.info_init();
    }
    if (!(parent->head.flags & 1)) {
        parent->head.info_init();
    }

    while (c != NULL) {
        if (c == parent) {
            return TRUE;
        }
        c = c->head.parent;
    }
    return FALSE;
}

void* fn_80193828(HSD_ClassInfo* i)
{
    extern void* memset(void* dst, int val, u32 size);
    HSD_ClassInfo* info = i;
    HSD_ClassInfo* alloc_info = info;
    HSD_Class* obj;

    if (!(alloc_info->head.flags & 1)) {
        alloc_info->head.info_init();
    }

    obj = info->alloc(alloc_info);
    if (obj == NULL) {
        return NULL;
    }

    if (!(info->head.flags & 1)) {
        info->head.info_init();
    }

    memset(obj, 0, info->head.obj_size);
    obj->class_info = info;

    if (info->init(obj) < 0) {
        info->destroy(obj);
        return NULL;
    }

    return obj;
}
