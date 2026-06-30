#include "dolphin/types.h"

/*
 * MetroWerks CodeWarrior CRT startup for GameCube (PowerPC).
 * These functions handle system initialization before main() is called.
 *
 * __start and __init_registers are written as inline asm because
 * the compiler cannot generate matching code for the bootstrap sequence.
 * __init_hardware and __flush_cache are also asm because they use
 * privileged instructions and cache management ops.
 */

/* Linker-defined symbols */
extern u32 _stack_addr[];
extern u32 _SDA_BASE_[];
extern u32 _SDA2_BASE_[];

typedef struct __rom_copy_info {
    void* rom;  /* source address in ROM */
    void* addr; /* destination address in RAM */
    u32 size;   /* copy size in bytes */
} __rom_copy_info;

typedef struct __bss_init_info {
    void* addr; /* start address */
    u32 size;   /* section size in bytes */
} __bss_init_info;

extern __rom_copy_info _rom_copy_info[];
extern __bss_init_info _bss_init_info[];

/* External function declarations */
/* __init_registers - 0x800032B0 | size: 0x90 */
void __init_registers(void);
/* __init_hardware - 0x80003400 | size: 0x24 */
void __init_hardware(void);
/* __init_data - 0x80003340 | size: 0xc0 */
void __init_data(void);
/* __flush_cache - 0x80003424 | size: 0x34 */
void __flush_cache(void* addr, u32 size);

extern void DBInit(void);
extern void OSInit(void);
extern void __init_user(void);
extern void __check_pad3(void);
extern void __set_debug_bba(void);
extern u8 __get_debug_bba(void);
extern void InitMetroTRK(u32 debugArg);
extern void InitMetroTRK_BBA(void);
extern void OSResetSystem(int, int, int);
extern int main(int argc, char** argv);
extern void exit(int);
extern void* memset(void*, int, u32);
extern void* memcpy(void*, const void*, u32);

extern void __OSPSInit(void);
extern void __OSFPRInit(void);
extern void __OSCacheInit(void);

/*
 * __start - Entry point for the GameCube application.
 *
 * This is the first function executed. It initializes registers, hardware,
 * copies data sections, sets up debug/TRK support, initializes the OS,
 * then calls main(). Written as asm for exact register matching.
 */
/* __start - 0x80003154 | size: 0x15c */
asm void __start(void) {
    nofralloc
    bl __init_registers
    bl __init_hardware

    /* Create initial stack frame with sentinel values */
    li r0, -1
    stwu r1, -8(r1)
    stw r0, 4(r1)
    stw r0, 0(r1)

    bl __init_data

    /* Clear boot info flag at 0x80000044 */
    li r0, 0
    lis r6, 0x8000
    addi r6, r6, 0x44
    stw r0, 0(r6)

    /* Check reset info at 0x800000F4 */
    lis r6, 0x8000
    addi r6, r6, 0xF4
    lwz r6, 0(r6)
    cmplwi r6, 0
    beq _no_reset_info

    /* Reset info exists: read boot type from offset 0xC */
    lwz r7, 0xC(r6)
    b _check_boot_type

_no_reset_info:
    /* No reset info: check arena lo at 0x80000034 */
    lis r5, 0x8000
    addi r5, r5, 0x34
    lwz r5, 0(r5)
    cmplwi r5, 0
    beq _skip_trk

    /* Read boot type from 0x800030E8 */
    lis r7, 0x8000
    addi r7, r7, 0x30E8
    lwz r7, 0(r7)

_check_boot_type:
    li r5, 0
    cmplwi r7, 2
    beq _init_metro_trk
    cmplwi r7, 3
    li r5, 1
    beq _init_metro_trk
    cmplwi r7, 4
    bne _skip_trk
    li r5, 2
    bl __set_debug_bba
    b _skip_trk

_init_metro_trk:
    lis r6, InitMetroTRK@ha
    addi r6, r6, InitMetroTRK@l
    mtlr r6
    blrl

_skip_trk:
    /* Process relocation table from reset info */
    lis r6, 0x8000
    addi r6, r6, 0xF4
    lwz r5, 0(r6)
    cmplwi r5, 0
    beq+ _no_reloc
    lwz r6, 8(r5)
    cmplwi r6, 0
    beq+ _no_reloc
    add r6, r5, r6
    lwz r14, 0(r6)
    cmplwi r14, 0
    beq _no_reloc
    addi r15, r6, 4
    mtctr r14

_reloc_loop:
    addi r6, r6, 4
    lwz r7, 0(r6)
    add r7, r7, r5
    stw r7, 0(r6)
    bdnz _reloc_loop

    lis r5, 0x8000
    addi r5, r5, 0x34
    clrrwi r7, r15, 5
    stw r7, 0(r5)
    b _after_reloc

_no_reloc:
    li r14, 0
    li r15, 0

_after_reloc:
    bl DBInit
    bl OSInit

    /* Check pad status at 0x800030E6 */
    lis r4, 0x8000
    addi r4, r4, 0x30E6
    lhz r3, 0(r4)
    andi. r5, r3, 0x8000
    beq _do_check_pad3
    andi. r3, r3, 0x7FFF
    cmplwi r3, 1
    bne _skip_pad3

_do_check_pad3:
    bl __check_pad3

_skip_pad3:
    bl __get_debug_bba
    cmplwi r3, 1
    bne _skip_bba
    bl InitMetroTRK_BBA

_skip_bba:
    bl __init_user
    mr r3, r14
    mr r4, r15
    bl main
    b exit
}

/*
 * __init_registers - Zero all general purpose registers and set up
 * the stack pointer (r1), SDA2 base (r2), and SDA base (r13).
 */
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

/*
 * __init_data - Copy ROM sections to RAM and clear BSS sections.
 *
 * Iterates through the _rom_copy_info table to copy initialized data
 * from ROM to RAM, then iterates through _bss_init_info to zero out
 * uninitialized data sections.
 */
void __init_data(void) {
    __rom_copy_info* rcp;
    __bss_init_info* bip;

    /* Copy ROM data to RAM */
    rcp = _rom_copy_info;
    while (rcp->size != 0) {
        if (rcp->addr && rcp->addr != rcp->rom) {
            memcpy(rcp->addr, rcp->rom, rcp->size);
            __flush_cache(rcp->addr, rcp->size);
        }
        rcp++;
    }

    /* Clear BSS sections */
    bip = _bss_init_info;
    while (bip->size != 0) {
        if (bip->addr != NULL) {
            memset(bip->addr, 0, bip->size);
        }
        bip++;
    }
}

/*
 * __init_hardware - Initialize the PowerPC hardware.
 *
 * Enables the FPU by setting MSR[FP], then initializes the paired
 * singles unit, floating point registers, and caches.
 */
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

/*
 * __flush_cache - Flush data cache and invalidate instruction cache
 * for the given memory range.
 *
 * @param addr  Start address of the range (r3)
 * @param size  Size of the range in bytes (r4)
 *
 * Aligns the start address down to a cache-line boundary, then iterates
 * through the range performing dcbst + sync + icbi for each 8-byte block.
 */
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
    addic r5, r5, 8
    subic. r4, r4, 8
    bge _flush_loop
    isync
    blr
}

/* ===== Phase 2 recovery stubs ===== */

/* __check_pad3 - 0x80003100 | size: 0x40 */

/* __set_debug_bba - 0x80003140 | size: 0xc */

/* __get_debug_bba - 0x8000314C | size: 0x8 */

/* fn_80003458 - 0x80003458 | size: 0x30 */
extern void fn_800C0DA8(void);
#if 0
asm void fn_80003458(void) {
#include "src/init/__start_fn_80003458.inc"
}
#else
u32 fn_80003458(u32 arg) {
    fn_800C0DA8();
    return arg;
}
#endif

/* fn_80003488 - 0x80003488 | size: 0x24 */
void fn_80003488(u8 *src, u8 *dst, int len) {
    while (len-- > 0) {
        *dst++ = *src++;
    }
}

/* fn_800053E0 - 0x800053E0 | size: 0x2c */
extern void OSResetSystem(int, int, int);
void fn_800053E0(void) {
    int r3 = 0;
    int r4 = 0;
    int r5 = 0;
    OSResetSystem(r3, r4, r5);
}

/* memset - 0x8000540C | size: 0x30 */

/* __fill_mem - 0x8000543C | size: 0xb8 */

/* memcpy - 0x800054F4 | size: 0x50 */

