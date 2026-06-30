/*
 * MetroWerks CodeWarrior __va_arg implementation for PowerPC.
 *
 * This function is called by the compiler to fetch the next variadic
 * argument from a va_list. The va_list structure on PPC contains:
 *
 *   struct __va_list_struct {
 *       char gpr;       // offset 0: next GPR index (0-8)
 *       char fpr;       // offset 1: next FPR index (0-8)
 *       char reserved[2];
 *       void* overflow; // offset 4: pointer into stack overflow area
 *       void* regs;     // offset 8: pointer to register save area
 *   };
 *
 * The register save area layout is:
 *   [0x00..0x1F] = GPR3..GPR10 (8 * 4 bytes)
 *   [0x20..0x5F] = FPR1..FPR8  (8 * 8 bytes)
 *
 * Type parameter values:
 *   0 = pointer/aggregate (result is dereferenced)
 *   1 = word (4 bytes, single GPR)
 *   2 = doubleword (8 bytes, GPR pair, even-aligned)
 *   3 = floating point (8 bytes, FPR)
 */

typedef struct __va_list_struct {
    char gpr;
    char fpr;
    char reserved[2];
    char* overflow_area;
    char* reg_save_area;
} __va_list_struct;

void* __va_arg(__va_list_struct* ap, int type) {
    char* result;
    char* counter_ptr;
    int reg_count;
    int max_regs;
    int slot_size;
    int increment;
    int align_offset;
    int area_offset;
    int slot_stride;

    /* Read GPR counter by default */
    reg_count = (signed char)ap->gpr;
    counter_ptr = &ap->gpr;
    max_regs = 8;
    slot_size = 4;
    increment = 1;
    align_offset = 0;
    area_offset = 0;
    slot_stride = 4;

    /* Float/double: use FPR counter and FPR save area */
    if (type == 3) {
        reg_count = (signed char)ap->fpr;
        counter_ptr = &ap->fpr;
        slot_size = 8;
        area_offset = 0x20;
        slot_stride = 8;
    }

    /* Doubleword: needs even GPR alignment, 8-byte slots */
    if (type == 2) {
        slot_size = 8;
        max_regs = 7;
        if (reg_count & 1) {
            align_offset = 1;
        }
        increment = 2;
    }

    if (reg_count < max_regs) {
        /* Argument is in a register save slot */
        reg_count += align_offset;
        result = ap->reg_save_area + area_offset + (reg_count * slot_stride);
        *counter_ptr = (char)(reg_count + increment);
    } else {
        /* Argument is in the overflow (stack) area */
        unsigned int align_mask;

        *counter_ptr = 8;
        align_mask = (unsigned int)slot_size - 1;
        result = (char*)(((unsigned int)ap->overflow_area + align_mask) & ~align_mask);
        ap->overflow_area = result + slot_size;
    }

    /* For pointer/aggregate types, dereference the result */
    if (type == 0) {
        result = *(char**)result;
    }

    return (void*)result;
}
