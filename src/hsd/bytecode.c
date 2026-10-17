/**
 * @file bytecode.c
 * @brief HAL bytecode.c: HSD_ByteCodeEval, the HSD byte code interpreter,
 *        0x801920E4 - 0x80193748.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/bytecode.c) and built with the sysdolphin library
 * flags (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
 * -str reuse,readonly), no local pragmas. The whole TU is this one function
 * plus its data:
 *   .rodata 0x802744F0 - 0x80274590 ("bytecode.c", "operand < nb_args", the
 *           OSReport/panic messages, "stack->next")
 *   .sdata2 0x8047D908 - 0x8047D950 (0.0f, "", "stack", DEG_TO_RAD,
 *           RAD_TO_DEG, the sqrtf constants, +-pi/2, the int-to-float bias)
 * The unit owns those ranges and links; the last 3 bytes of .rodata are
 * alignment padding.
 *
 * Colosseum's HAL version differs from Melee's (read from retail):
 *  - operands are read into the named temporaries: floats into f1 (every
 *    unary opcode, and the top of the stack after the pop in binary ones),
 *    ints into d0, so they sit in f1 / r22 as in retail;
 *  - opcode 0x26 answers x == 0 with +-pi/2 before scaling to degrees
 *    (bcAtan2, the same expansion as mtx.c's HSD_MtxGetRotation);
 *  - assert line numbers are those of Colosseum's file.
 *
 * Opcode 0x16's inlined sqrtf keeps the operand and result in f1 only
 * because the unary opcodes before it (0x10-0x14) also read into f1. MWCC
 * coalesces copies in block order. A user variable is outside the coalesce
 * window, so it only joins another value once it is tied to a physical
 * register. The f1 -> first-argument copy in 0x10-0x14 ties f1 to physical
 * f1 before 0x16, so the sqrtf's copies of x and its result coalesce into
 * f1 too. With the operands read directly into the calls, f1 is first tied
 * at 0x1B's fmod, too late: the sqrtf value is coloured last, in f8, and
 * the whole block shifts by one register (JackPriceBurns/mwcc,
 * src/backend/SpillCode.c, SpillCode_CanCoalesce).
 */
#include "dolphin/types.h"
#include "crt/math.h"
#include "crt/math_ppc.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_forward.h"

extern void HSD_Panic(const char* file, u32 line, const char* msg);
extern void OSReport(const char* fmt, ...);

extern HSD_SList* HSD_SListRemove(HSD_SList* node);   /* HSD_SListRemove  */
extern HSD_SList* HSD_SListPrepend(HSD_SList* next, void* data);
extern s32 HSD_Randi(s32 range);                /* HSD_Randi        */
extern f32 HSD_Randf(void);                     /* HSD_Randf        */

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

#define BC_DEG_TO_RAD 0.017453292519943295
#define BC_RAD_TO_DEG 57.29577951308232

typedef union ByteCodeVal {
    void* p;
    int i;
    f32 f;
} ByteCodeVal;

/*
 * atan2f(y, x) with the x == 0 case answered directly (+-pi/2 by the sign of
 * y). The same expansion, x loaded once and reused as atan2's argument, is
 * in mtx.c's HSD_MtxGetRotation (three times); here it feeds opcode 0x26.
 */
static inline f32 bcAtan2(f32 y, f32 x)
{
    if (x == 0.0f) {
        if (y >= 0.0f) {
            return 1.5707964f;
        } else {
            return -1.5707964f;
        }
    } else {
        return (f32) atan2(y, x);
    }
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
            operand = (operand << 8) | *bytecode++;

            if (operand_count != 0) {
                continue;
            }

            switch (last_command) {
            case 2:
                HSD_ASSERT(281, operand < nb_args);
                stack = HSD_SListPrepend(
                    stack, (void*) ((ByteCodeVal*) &args[operand])->i);
                break;
            case 5:
                for (i = 0; i < operand; i++) {
                    stack = HSD_SListRemove(stack);
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
                    HSD_Panic(__FILE__, 299, "");
                } else {
                    stack = HSD_SListPrepend(stack, list->data);
                }
                break;
            case 3:
                HSD_ASSERT(307, stack);
                if ((int) stack->data != 0) {
                    bytecode += operand;
                }
                stack = HSD_SListRemove(stack);
                break;
            case 4:
                bytecode += operand;
                break;
            case 6:
                stack = HSD_SListPrepend(stack, (void*) operand);
                break;
            case 0xFF:
                HSD_Panic(__FILE__, 323, "not yet implemented.\n");
                /* fallthrough */
            default:
                HSD_Panic(__FILE__, 326, "unexpected byte code.\n");
                break;
            }
            continue;
        }

        last_command = *bytecode++;
        switch (last_command) {
        case 0:
            break;
        case 1:
            HSD_ASSERT(339, stack);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            while (stack != NULL) {
                stack = HSD_SListRemove(stack);
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
            HSD_ASSERT(376, stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            ((ByteCodeVal*) &stack->data)->i = (int) f1;
            break;
        case 8:
            HSD_ASSERT(381, stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            fv = (f32) d0;
            stack->data = *(void**) &fv;
            break;
        case 9:
            HSD_ASSERT(387, stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = -f1;
            stack->data = *(void**) &fv;
            break;
        case 0x0A:
            HSD_ASSERT(393, stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = -d0;
            break;
        case 0x0B:
            HSD_ASSERT(399, stack);
            ((ByteCodeVal*) &stack->data)->i = HSD_Randi(2);
            break;
        case 0x0C:
            HSD_ASSERT(405, stack);
            fv = HSD_Randf();
            stack->data = *(void**) &fv;
            break;
        case 0x0D:
            HSD_ASSERT(411, stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = (f32) sin((f32) (BC_DEG_TO_RAD * f1));
            stack->data = *(void**) &fv;
            break;
        case 0x0E:
            HSD_ASSERT(417, stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = (f32) cos((f32) (BC_DEG_TO_RAD * f1));
            stack->data = *(void**) &fv;
            break;
        case 0x0F:
            HSD_ASSERT(423, stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = (f32) tan((f32) (BC_DEG_TO_RAD * f1));
            stack->data = *(void**) &fv;
            break;
        case 0x10:
            HSD_ASSERT(429, stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = (f32) (BC_RAD_TO_DEG * (f32) asin(f1));
            stack->data = *(void**) &fv;
            break;
        case 0x11:
            HSD_ASSERT(435, stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = (f32) (BC_RAD_TO_DEG * (f32) acos(f1));
            stack->data = *(void**) &fv;
            break;
        case 0x12:
            HSD_ASSERT(441, stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = (f32) (BC_RAD_TO_DEG * (f32) atan(f1));
            stack->data = *(void**) &fv;
            break;
        case 0x13:
            HSD_ASSERT(447, stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = (f32) log(f1);
            stack->data = *(void**) &fv;
            break;
        case 0x14:
            HSD_ASSERT(453, stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = (f32) exp(f1);
            stack->data = *(void**) &fv;
            break;
        case 0x15:
            HSD_ASSERT(459, stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            if (f1 < 0.0f) {
                fv = -f1;
                stack->data = *(void**) &fv;
            }
            break;
        case 0x28:
            HSD_ASSERT(467, stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            if (d0 < 0) {
                ((ByteCodeVal*) &stack->data)->i = -d0;
            }
            break;
        case 0x16:
            HSD_ASSERT(474, stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = sqrtf(f1);
            stack->data = *(void**) &fv;
            break;
        case 0x31:
            HSD_ASSERT(480, stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            stack->data = (void*) !d0;
            break;
        case 0x17:
            HSD_ASSERT(501, stack);
            HSD_ASSERT(501, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = f1 + f0;
            stack->data = *(void**) &fv;
            break;
        case 0x18:
            HSD_ASSERT(507, stack);
            HSD_ASSERT(507, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = f1 - f0;
            stack->data = *(void**) &fv;
            break;
        case 0x19:
            HSD_ASSERT(513, stack);
            HSD_ASSERT(513, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = f1 * f0;
            stack->data = *(void**) &fv;
            break;
        case 0x1A:
            HSD_ASSERT(519, stack);
            HSD_ASSERT(519, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = f1 / f0;
            stack->data = *(void**) &fv;
            break;
        case 0x1B:
            HSD_ASSERT(525, stack);
            HSD_ASSERT(525, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = (f32) fmod(f1, f0);
            stack->data = *(void**) &fv;
            break;
        case 0x1C:
            HSD_ASSERT(531, stack);
            HSD_ASSERT(531, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 + d1;
            break;
        case 0x1D:
            HSD_ASSERT(536, stack);
            HSD_ASSERT(536, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 - d1;
            break;
        case 0x1E:
            HSD_ASSERT(541, stack);
            HSD_ASSERT(541, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 * d1;
            break;
        case 0x1F:
            HSD_ASSERT(546, stack);
            HSD_ASSERT(546, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 / d1;
            break;
        case 0x20:
            HSD_ASSERT(551, stack);
            HSD_ASSERT(551, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 % d1;
            break;
        case 0x21:
            HSD_ASSERT(556, stack);
            HSD_ASSERT(556, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = (f32) pow(f1, f0);
            stack->data = *(void**) &fv;
            break;
        case 0x22:
            HSD_ASSERT(562, stack);
            HSD_ASSERT(562, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            if (((ByteCodeVal*) &stack->data)->f > f0) {
                stack->data = *(void**) &f0;
            }
            break;
        case 0x23:
            HSD_ASSERT(569, stack);
            HSD_ASSERT(569, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            if (((ByteCodeVal*) &stack->data)->f < f0) {
                stack->data = *(void**) &f0;
            }
            break;
        case 0x24:
            HSD_ASSERT(576, stack);
            HSD_ASSERT(576, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            if (d0 > d1) {
                ((ByteCodeVal*) &stack->data)->i = d1;
            }
            break;
        case 0x25:
            HSD_ASSERT(583, stack);
            HSD_ASSERT(583, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            if (d0 < d1) {
                ((ByteCodeVal*) &stack->data)->i = d1;
            }
            break;
        case 0x26:
            HSD_ASSERT(590, stack);
            HSD_ASSERT(590, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            fv = (f32) (BC_RAD_TO_DEG * bcAtan2(f1, f0));
            stack->data = *(void**) &fv;
            break;
        case 0x33:
            HSD_ASSERT(596, stack);
            HSD_ASSERT(596, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            ((ByteCodeVal*) &stack->data)->i = f1 < f0;
            break;
        case 0x34:
            HSD_ASSERT(601, stack);
            HSD_ASSERT(601, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            ((ByteCodeVal*) &stack->data)->i = f1 > f0;
            break;
        case 0x35:
            HSD_ASSERT(606, stack);
            HSD_ASSERT(606, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            ((ByteCodeVal*) &stack->data)->i = f1 <= f0;
            break;
        case 0x36:
            HSD_ASSERT(611, stack);
            HSD_ASSERT(611, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            ((ByteCodeVal*) &stack->data)->i = f1 >= f0;
            break;
        case 0x37:
            HSD_ASSERT(616, stack);
            HSD_ASSERT(616, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            ((ByteCodeVal*) &stack->data)->i = f1 == f0;
            break;
        case 0x38:
            HSD_ASSERT(621, stack);
            HSD_ASSERT(621, stack->next);
            f0 = ((ByteCodeVal*) &stack->data)->f;
            stack = HSD_SListRemove(stack);
            f1 = ((ByteCodeVal*) &stack->data)->f;
            ((ByteCodeVal*) &stack->data)->i = f1 != f0;
            break;
        case 0x29:
            HSD_ASSERT(626, stack);
            HSD_ASSERT(626, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 < d1;
            break;
        case 0x2A:
            HSD_ASSERT(631, stack);
            HSD_ASSERT(631, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 > d1;
            break;
        case 0x2B:
            HSD_ASSERT(636, stack);
            HSD_ASSERT(636, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 <= d1;
            break;
        case 0x2C:
            HSD_ASSERT(641, stack);
            HSD_ASSERT(641, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 >= d1;
            break;
        case 0x2D:
            HSD_ASSERT(646, stack);
            HSD_ASSERT(646, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 == d1;
            break;
        case 0x2E:
            HSD_ASSERT(651, stack);
            HSD_ASSERT(651, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 != d1;
            break;
        case 0x2F:
            HSD_ASSERT(656, stack);
            HSD_ASSERT(656, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 != 0 && d1 != 0;
            break;
        case 0x30:
            HSD_ASSERT(661, stack);
            HSD_ASSERT(661, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 != 0 || d1 != 0;
            break;
        case 0x32:
            HSD_ASSERT(666, stack);
            HSD_ASSERT(666, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i =
                (d0 == 0 && d1 != 0) || (d0 != 0 && d1 == 0);
            break;
        case 0x39:
            HSD_ASSERT(671, stack);
            HSD_ASSERT(671, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            stack->data = (void*) (d0 & d1);
            break;
        case 0x3A:
            HSD_ASSERT(676, stack);
            HSD_ASSERT(676, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            stack->data = (void*) (d0 | d1);
            break;
        case 0x3B:
            HSD_ASSERT(681, stack);
            HSD_ASSERT(681, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            stack->data = (void*) (d0 ^ d1);
            break;
        case 0x27:
            HSD_ASSERT(687, stack);
            HSD_ASSERT(687, stack->next);
            d1 = ((ByteCodeVal*) &stack->data)->i;
            stack = HSD_SListRemove(stack);
            d0 = ((ByteCodeVal*) &stack->data)->i;
            ((ByteCodeVal*) &stack->data)->i = d0 + HSD_Randi((d1 - d0) + 1);
            break;
        default:
            OSReport("unexpected opcode 0x%x.\n", last_command);
            HSD_Panic(__FILE__, 693, "");
            break;
        }
    }
}
