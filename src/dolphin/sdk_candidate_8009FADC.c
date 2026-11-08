/**
 * @file sdk_candidate_8009FADC.c
 * @brief OSReboot.c's Run (Run_8009FADC), 0x8009FADC - 0x8009FAEC: jumps to
 *        the reboot code at the given address.
 */
#include "dolphin/types.h"

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/os_primitives.md */
asm void Run_8009FADC(register u32 addr) {
    nofralloc
    sync
    isync
    mtlr    addr
    blr
}
