/*
 * MetroWerks CodeWarrior global destructor chain for GameCube.
 *
 * Manages the linked list of C++ global/static object destructors.
 * Objects are registered via __register_global_object (called from
 * static initializers) and destroyed in reverse order during
 * program shutdown via __destroy_global_chain.
 */

#include "dolphin/types.h"

typedef void (*DestructorFunc)(void* object, int mode);

typedef struct DestructorChainEntry {
    struct DestructorChainEntry* next;
    DestructorFunc destructor;
    void* object;
} DestructorChainEntry;

/*
 * Head of the global destructor chain. Stored in .sbss (small BSS)
 * for efficient SDA-relative access. Each entry is prepended to
 * the list during static initialization, so destruction happens
 * in reverse order of construction.
 */
DestructorChainEntry* __global_destructor_chain;

/*
 * __destroy_global_chain - Walk the destructor chain and invoke
 * each registered destructor.
 *
 * Called during program shutdown (via the .dtors section reference
 * in __init_cpp_exceptions.c). Each destructor receives its associated
 * object pointer and a mode of -1 (complete object destruction).
 *
 * The chain is consumed as it is walked: each entry's next pointer
 * is stored back to __global_destructor_chain before calling the
 * destructor, ensuring proper behavior even if a destructor triggers
 * further cleanup.
 */
void __destroy_global_chain(void) {
    DestructorChainEntry* entry;

    while ((entry = __global_destructor_chain) != NULL) {
        __global_destructor_chain = entry->next;
        entry->destructor(entry->object, -1);
    }
}

/* ========================================================== */
/* Runtime support functions                                  */
/* ========================================================== */

/*
 * __cvt_fp2unsigned - Convert a double to unsigned 32-bit int with saturation.
 *
 * Saturating double->u32 conversion used by the MSL CRT:
 *   val <  0.0            -> 0
 *   val >= 4294967296.0   -> 0xFFFFFFFF
 *   0 <= val < 2147483648.0 -> (u32)(s32)val
 *   2147483648.0 <= val < 4294967296.0 -> (u32)(s32)(val - 2147483648.0) + 0x80000000
 *
 * Constant table at lbl_8026FE58 (3 doubles):
 *   +0x00: 0.0           (low clamp)
 *   +0x08: 4294967296.0  (2^32 high clamp / saturation ceiling)
 *   +0x10: 2147483648.0  (2^31 mid-range threshold; subtracted to fit signed fctiwz)
 *
 * 0x800C46B0 | size: 0x5C
 *
 * NOTE: This is a CW Runtime helper of ASM origin (see docs/library_import_triage.md).
 * The target is hand-written PPC: it uses fcmpu (unordered, no NaN signalling),
 * direct bge/blt branches without cror, subi to materialize -1, and lis/ori address
 * materialization. CodeWarrior 1.3/1.3.2/2.0/1.2.5n all emit fcmpo + cror for C float
 * comparisons, so the exact target bytes are not C-reachable from any available CW
 * version. This is correct, real C left active (Equivalent), not byte-exact.
 */
u32 fn_800C46B0(f64 val) {
    extern f64 lbl_8026FE58[];
    u32 result;
    f64 tmp;

    result = 0;
    if (val < lbl_8026FE58[0]) {
        return result;
    }
    result = result - 1;
    if (val >= lbl_8026FE58[1]) {
        return result;
    }
    tmp = val;
    if (val >= lbl_8026FE58[2]) {
        tmp = val - lbl_8026FE58[2];
    }
    result = (u32)(s32)tmp;
    if (val < lbl_8026FE58[2]) {
        return result;
    }
    return result + 0x80000000u;
}

