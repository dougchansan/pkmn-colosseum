/**
 * @file hsd_aobj.c
 * @brief HSD AObj - Animation object implementation.
 *
 * AObj wraps FObj with playback state for driving animations.
 *
 * Adapted from doldecomp/melee src/sysdolphin/baselib/aobj.c
 */

#include "hsd/hsd_aobj.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_memory.h"

/* This TU corresponds to HAL's bytecode.c + aobj.c; assertions in the
 * original embed the bytecode.c file string. */
#undef HSD_ASSERT
#define HSD_ASSERT(line, cond)     ((cond) ? ((void) 0) : __assert("bytecode.c", line, #cond))

extern void* memset(void* dst, int val, u32 size);

/* ========================================================================= */
/*  Flag accessors                                                           */
/* ========================================================================= */

u32 HSD_AObjGetFlags(HSD_AObj* aobj)
{
    HSD_ASSERT(0, aobj);
    return aobj->flags;
}

void HSD_AObjSetFlags(HSD_AObj* aobj, u32 flags)
{
    HSD_ASSERT(0, aobj);
    aobj->flags |= flags;
}

void HSD_AObjClearFlags(HSD_AObj* aobj, u32 flags)
{
    HSD_ASSERT(0, aobj);
    aobj->flags &= ~flags;
}

/* ========================================================================= */
/*  Playback control                                                         */
/* ========================================================================= */

void HSD_AObjReqAnim(HSD_AObj* aobj, f32 frame)
{
    if (aobj != NULL) {
        aobj->curr_frame = frame;
        aobj->flags |= AOBJ_FIRST_PLAY;
        aobj->flags &= ~AOBJ_NO_ANIM;
        HSD_FObjReqAnimAll(aobj->fobj, frame);
    }
}

void HSD_AObjStopAnim(HSD_AObj* aobj, void* obj, HSD_ObjUpdateFunc func)
{
    if (aobj != NULL) {
        aobj->flags |= AOBJ_NO_ANIM;
    }
}

void HSD_AObjSetRate(HSD_AObj* aobj, f32 rate)
{
    HSD_ASSERT(0, aobj);
    aobj->framerate = rate;
}

void HSD_AObjSetEndFrame(HSD_AObj* aobj, f32 frame)
{
    HSD_ASSERT(0, aobj);
    aobj->end_frame = frame;
}

void HSD_AObjSetCurrentFrame(HSD_AObj* aobj, f32 frame)
{
    HSD_ASSERT(0, aobj);
    aobj->curr_frame = frame;
}

/* ========================================================================= */
/*  Interpret animation                                                      */
/* ========================================================================= */

void HSD_AObjInterpretAnim(HSD_AObj* aobj, void* obj,
                           HSD_ObjUpdateFunc update_func)
{
    if (aobj == NULL) {
        return;
    }
    if (aobj->flags & AOBJ_NO_ANIM) {
        return;
    }
    HSD_FObjInterpretAnimAll(aobj->fobj, obj, update_func, aobj->framerate);
    aobj->curr_frame += aobj->framerate;
}

/* ========================================================================= */
/*  Load / Remove / Alloc                                                    */
/* ========================================================================= */

HSD_AObj* HSD_AObjLoadDesc(HSD_AObjDesc* aobjdesc)
{
    HSD_AObj* aobj;

    if (aobjdesc == NULL) {
        return NULL;
    }

    aobj = HSD_AObjAlloc();
    aobj->flags = aobjdesc->flags;
    aobj->end_frame = aobjdesc->end_frame;
    aobj->fobj = HSD_FObjLoadDesc(aobjdesc->fobjdesc);
    aobj->framerate = 1.0f;
    aobj->curr_frame = 0.0f;
    aobj->rewind_frame = 0.0f;

    return aobj;
}

void HSD_AObjRemove(HSD_AObj* aobj)
{
    if (aobj != NULL) {
        HSD_FObjRemoveAll(aobj->fobj);
        HSD_Free(aobj);
    }
}

HSD_AObj* HSD_AObjAlloc(void)
{
    HSD_AObj* aobj = (HSD_AObj*) HSD_MemAlloc(sizeof(HSD_AObj));
    if (aobj != NULL) {
        memset(aobj, 0, sizeof(HSD_AObj));
    }
    return aobj;
}

void HSD_AObjFree(HSD_AObj* aobj)
{
    if (aobj != NULL) {
        HSD_Free(aobj);
    }
}

/* 0x801920E4 | 0x1664 - HSD_ByteCodeEval (HAL bytecode.c stack machine) */
extern void OSReport(const char* fmt, ...);
extern f64 sin(f64);
extern f64 cos(f64);
extern f64 atan(f64);
extern f64 fn_800CE220(f64);      /* tan */
extern f64 fn_800CE298(f64);      /* acos */
extern f64 fn_800CE2B8(f64);      /* asin */
extern f64 fn_800CE2D8(f64, f64); /* atan2 */
extern f64 fn_800CE2F8(f64);      /* exp */
extern f64 fn_800CE318(f64, f64); /* fmod */
extern f64 fn_800CE338(f64);      /* log */
extern f64 fn_800CE358(f64, f64); /* pow */
extern void HSD_Panic(const char* file, s32 line, const char* msg);
extern void __assert(const char* file, u32 line, const char* expr);
extern s32 fn_801ADC3C(s32 range);
extern f32 fn_801ADC7C(void);
extern f64 __frsqrte(f64);

typedef struct BcStack {
    struct BcStack* next; /* 0x0 */
    s32 value;            /* 0x4 */
} BcStack;

typedef union BcValue {
    s32 i;
    f32 f;
} BcValue;

extern BcStack* fn_801A3E64(BcStack* stack);            /* pop */
extern BcStack* fn_801A3EB4(BcStack* stack, u32 value); /* push */

extern const f32 lbl_80478AC0[]; /* NaN */

#define BC_FILE "bytecode.c"
#define BC_EXPR_OPERAND "operand < nb_args"
#define BC_FMT_NOSTACK "specified stack doesn't exist (%d).\n"
#define BC_MSG_NOTIMPL "not yet implemented.\n"
#define BC_MSG_UNEXBC "unexpected byte code.\n"
#define BC_EXPR_NEXT "stack->next"
#define BC_FMT_UNEXOP "unexpected opcode 0x%x.\n"
#define BC_EXPR_STACK "stack"
#define BC_EMPTY ""

#define BC_CHK(line) \
    if (stack == NULL) __assert(BC_FILE, line, BC_EXPR_STACK)
#define BC_CHK2(line)                                          \
    if (stack == NULL)                                         \
        __assert(BC_FILE, line, BC_EXPR_STACK);             \
    if (stack->next == NULL)                                   \
        __assert(BC_FILE, line, BC_EXPR_NEXT)

static inline s32 bc_fpclassify(f32 x)
{
    switch (*(s32*) &x & 0x7f800000) {
    case 0x7f800000:
        return (*(s32*) &x & 0x007fffff) ? 1 : 2;
    case 0:
        return (*(s32*) &x & 0x007fffff) ? 5 : 3;
    default:
        return 4;
    }
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 1
asm void fn_801920E4(void) {
#include "src/hsd/hsd_aobj_fn_801920E4.inc"
}
#else
#pragma optimization_level 3
#pragma fp_contract on
f32 fn_801920E4(u8* p, u32* args, u32 nb_args)
{
    s32 b;
    u32 operand;
    s32 noperand;
    s32 opcode;
    BcStack* stack;
    u32 a;
    volatile BcValue wr;
    volatile BcValue rd;

    stack = NULL;
    noperand = 0;
    if (p == NULL) {
        return 0.0f;
    }
    while (1) {
        if (noperand > 0) {
            s32 c = *(volatile u8*) p;
            noperand--;
            operand = (operand << 8) | (c & 0xff);
            p++;
            if (noperand == 0) {
                switch (opcode & 0xff) {
                case 0x02:
                    if (!(operand < nb_args))
                        __assert(BC_FILE, 0x119, BC_EXPR_OPERAND);
                    stack = fn_801A3EB4(stack, args[operand]);
                    break;
                case 0x05:
                    for (a = 0; a < operand; a++) {
                        stack = fn_801A3E64(stack);
                    }
                    break;
                case 0x3C: {
                    BcStack* node = stack;
                    for (a = 0; node != NULL && a < operand; a++) {
                        node = node->next;
                    }
                    if (node == NULL) {
                        OSReport(BC_FMT_NOSTACK, operand);
                        HSD_Panic(BC_FILE, 0x12b, BC_EMPTY);
                    } else {
                        stack = fn_801A3EB4(stack, node->value);
                    }
                    break;
                }
                case 0x03:
                    BC_CHK(0x133);
                    if (stack->value != 0) {
                        p += operand;
                    }
                    stack = fn_801A3E64(stack);
                    break;
                case 0x04:
                    p += operand;
                    break;
                case 0x06:
                    stack = fn_801A3EB4(stack, operand);
                    break;
                case 0xFF:
                    HSD_Panic(BC_FILE, 0x143, BC_MSG_NOTIMPL);
                default:
                    HSD_Panic(BC_FILE, 0x146, BC_MSG_UNEXBC);
                    break;
                }
            }
        } else {
            opcode = *(volatile s8*) p;
            p++;
            switch (opcode & 0xff) {
            case 0x00:
                break;
            case 0x01:
                BC_CHK(0x153);
                rd.f = *(f32*) &stack->value;
                while (stack != NULL) {
                    stack = fn_801A3E64(stack);
                }
                return rd.f;
            case 0x05:
            case 0x3C:
            case 0xFF:
                noperand = 1;
                operand = 0;
                break;
            case 0x02:
            case 0x03:
            case 0x04:
                noperand = 2;
                operand = 0;
                break;
            case 0x06:
                noperand = 4;
                operand = 0;
                break;
            case 0x07:
                BC_CHK(0x178);
                stack->value = (s32) *(f32*) &stack->value;
                break;
            case 0x08:
                BC_CHK(0x17d);
                a = stack->value;
                wr.f = (s32) a;
                stack->value = wr.i;
                break;
            case 0x09:
                BC_CHK(0x183);
                wr.f = -*(f32*) &stack->value;
                stack->value = wr.i;
                break;
            case 0x0A:
                BC_CHK(0x189);
                a = stack->value;
                stack->value = -(s32) a;
                break;
            case 0x0B:
                BC_CHK(0x18f);
                stack->value = fn_801ADC3C(2);
                break;
            case 0x0C:
                BC_CHK(0x195);
                wr.f = fn_801ADC7C();
                stack->value = wr.i;
                break;
            case 0x0D:
                BC_CHK(0x19b);
                {
                    f32 x = *(f32*) &stack->value;
                    wr.f = (f32) sin((f32) (0.017453292519943295 * x));
                }
                stack->value = wr.i;
                break;
            case 0x0E:
                BC_CHK(0x1a1);
                {
                    f32 x = *(f32*) &stack->value;
                    wr.f = (f32) cos((f32) (0.017453292519943295 * x));
                }
                stack->value = wr.i;
                break;
            case 0x0F:
                BC_CHK(0x1a7);
                {
                    f32 x = *(f32*) &stack->value;
                    wr.f = (f32) fn_800CE220((f32) (0.017453292519943295 * x));
                }
                stack->value = wr.i;
                break;
            case 0x10:
                BC_CHK(0x1ad);
                wr.f = (f32) (57.29577951308232 *
                              (f32) fn_800CE2B8(*(f32*) &stack->value));
                stack->value = wr.i;
                break;
            case 0x11:
                BC_CHK(0x1b3);
                wr.f = (f32) (57.29577951308232 *
                              (f32) fn_800CE298(*(f32*) &stack->value));
                stack->value = wr.i;
                break;
            case 0x12:
                BC_CHK(0x1b9);
                wr.f = (f32) (57.29577951308232 *
                              (f32) atan(*(f32*) &stack->value));
                stack->value = wr.i;
                break;
            case 0x13:
                BC_CHK(0x1bf);
                wr.f = (f32) fn_800CE338(*(f32*) &stack->value);
                stack->value = wr.i;
                break;
            case 0x14:
                BC_CHK(0x1c5);
                wr.f = (f32) fn_800CE2F8(*(f32*) &stack->value);
                stack->value = wr.i;
                break;
            case 0x15:
                BC_CHK(0x1cb);
                if (*(f32*) &stack->value < 0.0f) {
                    wr.f = -*(f32*) &stack->value;
                    stack->value = wr.i;
                }
                break;
            case 0x28:
                BC_CHK(0x1d3);
                a = stack->value;
                if ((s32) a < 0) {
                    stack->value = -(s32) a;
                }
                break;
            case 0x16: {
                f32 x;
                BC_CHK(0x1da);
                x = *(f32*) &stack->value;
                if (x > 0.0f) {
                    f64 guess = __frsqrte(x);
                    guess = 0.5 * guess * (3.0 - x * (guess * guess));
                    guess = 0.5 * guess * (3.0 - x * (guess * guess));
                    guess = 0.5 * guess * (3.0 - x * (guess * guess));
                    x = (f32) (x * guess);
                } else if (x < 0.0) {
                    x = lbl_80478AC0[0];
                } else {
                    if (bc_fpclassify(x) == 1) {
                        x = lbl_80478AC0[0];
                    }
                }
                wr.f = x;
                stack->value = wr.i;
                break;
            }
            case 0x31:
                BC_CHK(0x1e0);
                a = stack->value;
                stack->value = a == 0;
                break;
            case 0x17:
                BC_CHK2(0x1f5);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                wr.f = *(f32*) &stack->value + rd.f;
                stack->value = wr.i;
                break;
            case 0x18:
                BC_CHK2(0x1fb);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                wr.f = *(f32*) &stack->value - rd.f;
                stack->value = wr.i;
                break;
            case 0x19:
                BC_CHK2(0x201);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                wr.f = *(f32*) &stack->value * rd.f;
                stack->value = wr.i;
                break;
            case 0x1A:
                BC_CHK2(0x207);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                wr.f = *(f32*) &stack->value / rd.f;
                stack->value = wr.i;
                break;
            case 0x1B:
                BC_CHK2(0x20d);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                wr.f = (f32) fn_800CE318(*(f32*) &stack->value, rd.f);
                stack->value = wr.i;
                break;
            case 0x1C:
                BC_CHK2(0x213);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = a + b;
                break;
            case 0x1D:
                BC_CHK2(0x218);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = a - b;
                break;
            case 0x1E:
                BC_CHK2(0x21d);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = a * b;
                break;
            case 0x1F:
                BC_CHK2(0x222);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = (s32) a / b;
                break;
            case 0x20:
                BC_CHK2(0x227);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = (s32) a % b;
                break;
            case 0x21:
                BC_CHK2(0x22c);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                wr.f = (f32) fn_800CE358(*(f32*) &stack->value, rd.f);
                stack->value = wr.i;
                break;
            case 0x22:
                BC_CHK2(0x232);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                if (*(f32*) &stack->value > rd.f) {
                    stack->value = rd.i;
                }
                break;
            case 0x23:
                BC_CHK2(0x239);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                if (*(f32*) &stack->value < rd.f) {
                    stack->value = rd.i;
                }
                break;
            case 0x24:
                BC_CHK2(0x240);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                if ((s32) a > b) {
                    stack->value = b;
                }
                break;
            case 0x25:
                BC_CHK2(0x247);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                if ((s32) a < b) {
                    stack->value = b;
                }
                break;
            case 0x26:
                BC_CHK2(0x24e);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                {
                    f32 t;
                    if (0.0f == rd.f) {
                        if (*(f32*) &stack->value >= 0.0f) {
                            t = 1.5707964f;
                        } else {
                            t = -1.5707964f;
                        }
                    } else {
                        t = (f32) fn_800CE2D8(*(f32*) &stack->value, rd.f);
                    }
                    wr.f = (f32) (57.29577951308232 * t);
                }
                stack->value = wr.i;
                break;
            case 0x33:
                BC_CHK2(0x254);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                stack->value = *(f32*) &stack->value < rd.f;
                break;
            case 0x34:
                BC_CHK2(0x259);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                stack->value = *(f32*) &stack->value > rd.f;
                break;
            case 0x35:
                BC_CHK2(0x25e);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                stack->value = *(f32*) &stack->value <= rd.f;
                break;
            case 0x36:
                BC_CHK2(0x263);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                stack->value = *(f32*) &stack->value >= rd.f;
                break;
            case 0x37:
                BC_CHK2(0x268);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                stack->value = *(f32*) &stack->value == rd.f;
                break;
            case 0x38:
                BC_CHK2(0x26d);
                rd.f = *(f32*) &stack->value;
                stack = fn_801A3E64(stack);
                stack->value = *(f32*) &stack->value != rd.f;
                break;
            case 0x29:
                BC_CHK2(0x272);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = (s32) a < b;
                break;
            case 0x2A:
                BC_CHK2(0x277);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = (s32) a > b;
                break;
            case 0x2B:
                BC_CHK2(0x27c);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = (s32) a <= b;
                break;
            case 0x2C:
                BC_CHK2(0x281);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = (s32) a >= b;
                break;
            case 0x2D:
                BC_CHK2(0x286);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = a == b;
                break;
            case 0x2E:
                BC_CHK2(0x28b);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = a != b;
                break;
            case 0x2F:
                BC_CHK2(0x290);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = a && b;
                break;
            case 0x30:
                BC_CHK2(0x295);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = a || b;
                break;
            case 0x32:
                BC_CHK2(0x29a);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = (a == 0 && b != 0) || (a != 0 && b == 0);
                break;
            case 0x39:
                BC_CHK2(0x29f);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = a & b;
                break;
            case 0x3A:
                BC_CHK2(0x2a4);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = a | b;
                break;
            case 0x3B:
                BC_CHK2(0x2a9);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = a ^ b;
                break;
            case 0x27:
                BC_CHK2(0x2af);
                b = stack->value;
                stack = fn_801A3E64(stack);
                a = stack->value;
                stack->value = a + fn_801ADC3C(b - a + 1);
                break;
            default:
                OSReport(BC_FMT_UNEXOP, opcode & 0xff);
                HSD_Panic(BC_FILE, 0x2b5, BC_EMPTY);
                break;
            }
        }
    }
}
#endif
#pragma pop
