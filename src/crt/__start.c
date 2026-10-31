/**
 * @file __start.c
 * @brief MetroWerks runtime init (.init section), 0x80003100 - 0x80005544.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) - mixed-block split pass, 2026-07-01.
 * __start, __init_registers, __init_hardware and __flush_cache are the
 * Dolphin SDK's hand-written assembly (bootstrap register state, MSR and
 * cache-block instructions); see docs/asm_evidence/start.md. All other
 * functions below are real C.
 */
#include "dolphin/types.h"

typedef struct __rom_copy_info {
    void* rom;  /* source address in ROM */
    void* addr; /* destination address in RAM */
    u32 size;   /* copy size in bytes */
} __rom_copy_info;

typedef struct __bss_init_info {
    void* addr; /* start address */
    u32 size;   /* section size in bytes */
} __bss_init_info;

extern void __OSPSInit(void);
extern void __OSFPRInit(void);
extern void __OSCacheInit(void);
extern __rom_copy_info _rom_copy_info[];
extern __bss_init_info _bss_init_info[];

extern void OSResetSystem(int reset, int resetCode, int forceMenu);
extern void __flush_cache(void* addr, u32 size);
extern void TRK_fill_mem_800D6430(void* dest, int val, u32 count);

__declspec(section ".init") void* memset(void* dest, int val, u32 count);
__declspec(section ".init") void* memcpy(void* dst, const void* src, u32 count);
__declspec(section ".init") void __fill_mem(void* dest, int val, u32 count);

/* __start.c proper (0x80003100 - 0x80003458) is built with GC/1.2.5n.
   The two GC/1.3.2 memory-helper units surround the MetroTRK vector assembly
   at 0x800034AC - 0x800053E0, reconstructed in interrupt_vectors.s. */
#if !defined(START_MEM_PART)
extern void InitMetroTRK(void);
extern void InitMetroTRK_BBA(void);
extern void DBInit(void);
extern void OSInit(void);
extern void __init_user(void);
extern int main(int argc, char* argv[]);
extern void exit(int status);

/* Linker-generated symbols (__ppc_eabi_linker.h). */
extern char _stack_addr[];
extern char _SDA_BASE_[];
extern char _SDA2_BASE_[];

__declspec(section ".init") static void __set_debug_bba(void);
__declspec(section ".init") static u8 __get_debug_bba(void);
__declspec(section ".init") static void __init_registers(void);
__declspec(section ".init") static void __init_data(void);
__declspec(section ".init") void __init_hardware(void);

/* Debug_BBA - .sbss:0x8047A770 | size: 0x1 scope:local */
static u8 Debug_BBA;

/* __check_pad3 - 0x80003100 | size: 0x40 */
__declspec(section ".init") static void __check_pad3(void) {
    if ((*(volatile u16*)0x800030E4 & 0xEEF) == 0xEEF) {
        OSResetSystem(0, 0, 0);
    }
}

/* __set_debug_bba - 0x80003140 | size: 0xC */
__declspec(section ".init") static void __set_debug_bba(void) {
    Debug_BBA = 1;
}

/* __get_debug_bba - 0x8000314C | size: 0x8 */
__declspec(section ".init") static u8 __get_debug_bba(void) {
    return Debug_BBA;
}

/* __start - 0x80003154 | size: 0x15C | scope:weak */
/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/start.md */
__declspec(section ".init") __declspec(weak)
asm void __start(void) {
    nofralloc
    bl __init_registers
    bl __init_hardware
    li r0, -1
    stwu r1, -8(r1)
    stw r0, 4(r1)
    stw r0, 0(r1)
    bl __init_data
    li r0, 0
    lis r6, 0x8000
    addi r6, r6, 0x0044
    stw r0, 0(r6)
    lis r6, 0x8000
    addi r6, r6, 0x00F4
    lwz r6, 0(r6)
    cmplwi r6, 0
    beq _no_bi2
    lwz r7, 0xc(r6)
    b _check_debugger
_no_bi2:
    lis r5, 0x8000
    addi r5, r5, 0x0034
    lwz r5, 0(r5)
    cmplwi r5, 0
    beq _skip_trk
    lis r7, 0x8000
    addi r7, r7, 0x30E8
    lwz r7, 0(r7)
_check_debugger:
    li r5, 0
    cmplwi r7, 2
    beq _init_trk
    cmplwi r7, 3
    li r5, 1
    beq _init_trk
    cmplwi r7, 4
    bne _skip_trk
    li r5, 2
    bl __set_debug_bba
    b _skip_trk
_init_trk:
    lis r6, InitMetroTRK@ha
    addi r6, r6, InitMetroTRK@l
    mtlr r6
    blrl
_skip_trk:
    lis r6, 0x8000
    addi r6, r6, 0x00F4
    lwz r5, 0(r6)
    cmplwi r5, 0
    beq+ _no_args
    lwz r6, 8(r5)
    cmplwi r6, 0
    beq+ _no_args
    add r6, r5, r6
    lwz r14, 0(r6)
    cmplwi r14, 0
    beq _no_args
    addi r15, r6, 4
    mtctr r14
_arg_loop:
    addi r6, r6, 4
    lwz r7, 0(r6)
    add r7, r7, r5
    stw r7, 0(r6)
    bdnz _arg_loop
    lis r5, 0x8000
    addi r5, r5, 0x0034
    rlwinm r7, r15, 0, 0, 26
    stw r7, 0(r5)
    b _init_os
_no_args:
    li r14, 0
    li r15, 0
_init_os:
    bl DBInit
    bl OSInit
    lis r4, 0x8000
    addi r4, r4, 0x30E6
    lhz r3, 0(r4)
    andi. r5, r3, 0x8000
    beq _check_pad
    andi. r3, r3, 0x7fff
    cmplwi r3, 1
    bne _check_bba
_check_pad:
    bl __check_pad3
_check_bba:
    bl __get_debug_bba
    cmplwi r3, 1
    bne _call_main
    bl InitMetroTRK_BBA
_call_main:
    bl __init_user
    mr r3, r14
    mr r4, r15
    bl main
    b exit
}

/* __init_registers - 0x800032B0 | size: 0x90 | scope:local */
/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/start.md */
__declspec(section ".init") static
asm void __init_registers(void) {
    nofralloc
    li r0, 0
    li r3, 0
    li r4, 0
    li r5, 0
    li r6, 0
    li r7, 0
    li r8, 0
    li r9, 0
    li r10, 0
    li r11, 0
    li r12, 0
    li r14, 0
    li r15, 0
    li r16, 0
    li r17, 0
    li r18, 0
    li r19, 0
    li r20, 0
    li r21, 0
    li r22, 0
    li r23, 0
    li r24, 0
    li r25, 0
    li r26, 0
    li r27, 0
    li r28, 0
    li r29, 0
    li r30, 0
    li r31, 0
    lis r1, _stack_addr@h
    ori r1, r1, _stack_addr@l
    lis r2, _SDA2_BASE_@h
    ori r2, r2, _SDA2_BASE_@l
    lis r13, _SDA_BASE_@h
    ori r13, r13, _SDA_BASE_@l
    blr
}

/* __init_data - 0x80003340 | size: 0xC0 */
inline static void __copy_rom_section(void* dst, const void* src, u32 size)
{
    if (size && (dst != src)) {
        memcpy(dst, src, size);
        __flush_cache(dst, size);
    }
}

inline static void __init_bss_section(void* dst, u32 size)
{
    if (size) {
        memset(dst, 0, size);
    }
}

__declspec(section ".init") static void __init_data(void) {
    __rom_copy_info* dci;
    __bss_init_info* bii;

    dci = _rom_copy_info;
    while (TRUE) {
        if (dci->size == 0)
            break;
        __copy_rom_section(dci->addr, dci->rom, dci->size);
        dci++;
    }

    bii = _bss_init_info;
    while (TRUE) {
        if (bii->size == 0)
            break;
        __init_bss_section(bii->addr, bii->size);
        bii++;
    }
}

/* __init_hardware - 0x80003400 | size: 0x24 */
/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/start.md */
__declspec(section ".init")
asm void __init_hardware(void) {
    nofralloc
    mfmsr r0
    ori r0, r0, 0x2000
    mtmsr r0
    mflr r31
    bl __OSPSInit
    bl __OSFPRInit
    bl __OSCacheInit
    mtlr r31
    blr
}

/* __flush_cache - 0x80003424 | size: 0x34 */
/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/start.md */
__declspec(section ".init")
asm void __flush_cache(void* addr, u32 size) {
    nofralloc
    lis r5, 0xFFFF
    ori r5, r5, 0xFFF1
    and r5, r5, r3
    subf r3, r5, r3
    add r4, r4, r3
_flush_loop:
    dcbst r0, r5
    sync
    icbi r0, r5
    addic r5, r5, 0x8
    subic. r4, r4, 0x8
    bge _flush_loop
    isync
    blr
}

#endif /* !START_MEM_PART */

#if defined(START_MEM_PART)
#if !defined(START_MEM_SUFFIX)
/* TRK_memset - 0x80003458 | size: 0x30 */
__declspec(section ".init") void* TRK_memset(void* dest, int val, u32 count) {
    TRK_fill_mem_800D6430(dest, val, count);
    return dest;
}

/* TRK_memcpy - 0x80003488 | size: 0x24 */
__declspec(section ".init") void* TRK_memcpy(void* dst, const void* src, int len) {
    const u8* s;
    u8* d;
    int n;

    s = (const u8*)src - 1;
    d = (u8*)dst - 1;
    n = len + 1;
    while (--n) {
        *++d = *++s;
    }
    return dst;
}

#endif /* !START_MEM_SUFFIX */

#if !defined(START_MEM_PREFIX)
/* fn_800053E0 - 0x800053E0 | size: 0x2C */
__declspec(section ".init") void fn_800053E0(void) {
    OSResetSystem(0, 0, 0);
}

/* memset - 0x8000540C | size: 0x30 */
__declspec(section ".init") void* memset(void* dest, int val, u32 count) {
    __fill_mem(dest, val, count);
    return dest;
}

/* __fill_mem - 0x8000543C | size: 0xB8 */
__declspec(section ".init") void __fill_mem(void* dest, int val, u32 count) {
    u8* dst;
    u32 v;
    u32 numBlocks;
    u32 numWords;
    u32* wp;
    u32 align;

    dst = (u8*)dest - 1;
    v = (u8)val;

    if (count >= 0x20) {
        align = ~(u32)dst & 3;
        if (align != 0) {
            count -= align;
            do {
                *++dst = (u8)v;
            } while (--align != 0);
        }

        if (v != 0) {
            v = (v << 24) | (v << 16) | (v << 8) | v;
        }

        {
            wp = (u32*)(dst - 3);
            if ((numBlocks = count >> 5) != 0) {
                do {
                    wp[1] = v;
                    wp[2] = v;
                    wp[3] = v;
                    wp[4] = v;
                    wp[5] = v;
                    wp[6] = v;
                    wp[7] = v;
                    *(wp += 8) = v;
                } while (--numBlocks != 0);
            }

            if ((numWords = (count >> 2) & 7) != 0) {
                do {
                    *(wp += 1) = v;
                } while (--numWords != 0);
            }

            dst = (u8*)wp + 3;
        }

        count = count & 3u;
    }

    if (count != 0) {
        do {
            *++dst = (u8)v;
        } while (--count != 0);
    }
}

/* memcpy - 0x800054F4 | size: 0x50 */
__declspec(section ".init") void* memcpy(void* dst, const void* src, u32 count) {
    const u8* s;
    u8* d;
    u32 n;

    if ((const u8*)src >= (u8*)dst) {
        s = (const u8*)src - 1;
        d = (u8*)dst - 1;
        n = count + 1;
        while (--n) {
            *++d = *++s;
        }
    } else {
        s = (const u8*)src + count;
        d = (u8*)dst + count;
        n = count + 1;
        while (--n) {
            *--d = *--s;
        }
    }

    return dst;
}
#endif /* !START_MEM_PREFIX */
#endif /* START_MEM_PART */
