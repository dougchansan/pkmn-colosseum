/**
 * @file __start.c
 * @brief MetroWerks runtime init (.init section), 0x80003100 - 0x80005544.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) - mixed-block split pass, 2026-07-01.
 * __start, __init_registers, __init_hardware and __flush_cache require
 * privileged/cache-management instructions and exact bootstrap register
 * state; they remain asm-only. All other functions below are real C.
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

/* __check_pad3 - 0x80003100 | size: 0x40 */
__declspec(section ".init") void __check_pad3(void) {
    if ((*(volatile u16*)0x800030E4 & 0xEEF) == 0xEEF) {
        OSResetSystem(0, 0, 0);
    }
}

/* Debug_BBA - .sbss:0x8047A770 | size: 0x1 scope:local */
static u8 Debug_BBA;

/* 0x80003400 | .init | size: 0x24 */
__declspec(section ".init") asm void __init_hardware(void) {
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

/* 0x80003424 | .init | size: 0x34 */
__declspec(section ".init") asm void __flush_cache(void* addr, u32 size) {
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

/* __set_debug_bba - 0x80003140 | size: 0xC */
__declspec(section ".init") void __set_debug_bba(void) {
    Debug_BBA = 1;
}

/* __get_debug_bba - 0x8000314C | size: 0x8 */
__declspec(section ".init") u8 __get_debug_bba(void) {
    return Debug_BBA;
}

/*
 * __init_data - Copy ROM sections to RAM and clear BSS sections.
 *
 * Iterates through the _rom_copy_info table to copy initialized data
 * from ROM to RAM, then iterates through _bss_init_info to zero out
 * uninitialized data sections.
 */
/* __init_data - 0x80003340 | size: 0xC0 */
__declspec(section ".init") void __init_data(void) {
    void* addr;
    u32 size;
    __rom_copy_info* rcp;
    __bss_init_info* bip;

    rcp = _rom_copy_info;
    while ((size = rcp->size) != 0) {
        addr = rcp->addr;
        if (addr != NULL && addr != rcp->rom) {
            memcpy(addr, rcp->rom, size);
            __flush_cache(addr, size);
        }
        rcp++;
    }

    bip = _bss_init_info;
    while (bip->size != 0) {
        if (bip->addr != NULL) {
            memset(bip->addr, 0, bip->size);
        }
        bip++;
    }
}

/* fn_80003458 - 0x80003458 | size: 0x30 */
__declspec(section ".init") void* fn_80003458(void* dest, int val, u32 count) {
    TRK_fill_mem_800D6430(dest, val, count);
    return dest;
}

/* fn_80003488 - 0x80003488 | size: 0x24 */
__declspec(section ".init") void* fn_80003488(void* dst, const void* src, int len) {
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
