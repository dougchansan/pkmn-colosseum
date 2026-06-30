#include "dolphin/os/OSCache.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/PPCArch.h"
#include "dolphin/db/DB.h"

#define HID2 920

/*
 * OSCache.c - Data cache, instruction cache, and L2 cache management.
 *
 * Provides cache enable/disable/flush/invalidate operations, the L2 cache
 * global invalidate procedure, the DMA error handler, and cache init.
 *
 * Matches: 0x8009B290 - 0x8009B914
 */

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void DCEnable(void) {
    nofralloc
    sync
    mfspr   r3, HID0
    ori     r3, r3, 0x4000
    mtspr   HID0, r3
    blr
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void DCInvalidateRange(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi  r4, 0
    blelr
    clrlwi  r5, r3, 27
    add     r4, r4, r5
    addi    r4, r4, 31
    srwi    r4, r4, 5
    mtctr   r4
_loop_dci:
    dcbi    r0, r3
    addi    r3, r3, 32
    bdnz    _loop_dci
    blr
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void DCFlushRange(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi  r4, 0
    blelr
    clrlwi  r5, r3, 27
    add     r4, r4, r5
    addi    r4, r4, 31
    srwi    r4, r4, 5
    mtctr   r4
_loop_dcf:
    dcbf    r0, r3
    addi    r3, r3, 32
    bdnz    _loop_dcf
    sc
    blr
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void DCFlushRangeNoSync(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi  r4, 0
    blelr
    clrlwi  r5, r3, 27
    add     r4, r4, r5
    addi    r4, r4, 31
    srwi    r4, r4, 5
    mtctr   r4
_loop_dcfns:
    dcbf    r0, r3
    addi    r3, r3, 32
    bdnz    _loop_dcfns
    blr
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void ICInvalidateRange(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi  r4, 0
    blelr
    clrlwi  r5, r3, 27
    add     r4, r4, r5
    addi    r4, r4, 31
    srwi    r4, r4, 5
    mtctr   r4
_loop_ici:
    icbi    r0, r3
    addi    r3, r3, 32
    bdnz    _loop_ici
    sync
    isync
    blr
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void ICFlashInvalidate(void) {
    nofralloc
    mfspr   r3, HID0
    ori     r3, r3, 0x0800
    mtspr   HID0, r3
    blr
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void ICEnable(void) {
    nofralloc
    isync
    mfspr   r3, HID0
    ori     r3, r3, 0x8000
    mtspr   HID0, r3
    blr
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void LCDisable(void) {
    nofralloc
    lis     r3, 0xE000
    li      r4, 512
    mtctr   r4
_loop_lcd:
    dcbi    r0, r3
    addi    r3, r3, 32
    bdnz    _loop_lcd
    mfspr   r4, HID2
    rlwinm  r4, r4, 0, 4, 2   /* clear LC enable bit */
    mtspr   HID2, r4
    blr
}
#pragma pop

void L2GlobalInvalidate(void) {
    asm { sync }
    PPCMtl2cr(PPCMfl2cr() & 0x7FFFFFFF);
    asm { sync }

    PPCMtl2cr(PPCMfl2cr() | 0x00200000);

    while (PPCMfl2cr() & 0x00000001u)
        ;

    PPCMtl2cr(PPCMfl2cr() & ~0x00200000);

    while (PPCMfl2cr() & 0x00000001u) {
        DBPrintf(">>> L2 INVALIDATE : SHOULD NEVER HAPPEN\n");
    }
}

void DMAErrorHandler(u16 error, OSContext* context, ...) {
    u32 hid2;

    hid2 = PPCMfhid2();

    OSReport("Machine check received\n");
    OSReport("HID2 = 0x%08x   SRR1 = 0x%08x\n", hid2, context->srr1);

    if (!(hid2 & 0x00F00000) || !(context->srr1 & 0x00200000)) {
        OSReport("Unrecoverable DMA error\n");
        OSDumpContext(context);
        PPCHalt();
    }

    OSReport("DMA error handler: recovering\n");
    OSReport("Resetting write gather pipe\n");

    if (hid2 & 0x01000000) {
        OSReport("  Write gather pipe overflow\n");
    }
    if (hid2 & 0x00800000) {
        OSReport("  Write gather pipe underflow\n");
    }
    if (hid2 & 0x00400000) {
        OSReport("  Write gather pipe parity error\n");
    }
    if (hid2 & 0x00200000) {
        OSReport("  Write gather pipe error\n");
    }

    PPCMthid2(hid2);
}

void __OSCacheInit(void) {
    u32 hid0;

    /* Enable I-cache if not already enabled */
    hid0 = PPCMfhid0();
    if (!(hid0 & 0x8000)) {
        ICEnable();
        DBPrintf("L1 I-Cache has been enabled\n");
    }

    /* Enable D-cache if not already enabled */
    hid0 = PPCMfhid0();
    if (!(hid0 & 0x4000)) {
        DCEnable();
        DBPrintf("L1 D-Cache has been enabled\n");
    }

    /* Enable L2 cache if not already enabled */
    {
        u32 l2cr;
        l2cr = PPCMfl2cr();
        if (!(l2cr & 0x80000000)) {
            u32 msr;

            msr = PPCMfmsr();
            asm { sync }
            PPCMtmsr(0x30);
            asm { sync }
            asm { sync }

            /* Disable L2 */
            l2cr = PPCMfl2cr();
            l2cr &= 0x7FFFFFFF;
            PPCMtl2cr(l2cr);
            asm { sync }

            /* Do a global invalidate */
            L2GlobalInvalidate();

            /* Restore MSR */
            PPCMtmsr(msr);

            /* Enable L2 with data-only mode cleared */
            l2cr = PPCMfl2cr();
            l2cr = (l2cr | 0x80000000) & ~0x00200000;
            PPCMtl2cr(l2cr);

            DBPrintf("L2 Cache has been enabled\n");
        }
    }

    /* Install DMA error handler (error type 1 = machine check) */
    OSSetErrorHandler(1, (OSErrorHandler)DMAErrorHandler);
    DBPrintf("Locked cache machine check handler installed\n");
}

/* ===================================================================
 * Stub functions for coverage -- TODO: decompile
 * 9 function(s)
 * =================================================================== */

/*
 * DCStoreRange - Store (write back) a range of data cache lines.
 *
 * Writes back each 32-byte cache line covering [addr, addr+nBytes)
 * then issues a sync to ensure completion.
 *
 * 0x8009B300 | size: 0x30
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void DCStoreRange(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi  r4, 0
    blelr
    clrlwi  r5, r3, 27
    add     r4, r4, r5
    addi    r4, r4, 31
    srwi    r4, r4, 5
    mtctr   r4
_loop_dcs:
    dcbst   r0, r3
    addi    r3, r3, 32
    bdnz    _loop_dcs
    sc
    blr
}
#pragma pop

/*
 * DCStoreRangeNoSync - Store (write back) data cache lines without sync.
 *
 * Same as DCStoreRange but does not issue a sync instruction at the end.
 *
 * 0x8009B35C | size: 0x2C
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void DCStoreRangeNoSync(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi  r4, 0
    blelr
    clrlwi  r5, r3, 27
    add     r4, r4, r5
    addi    r4, r4, 31
    srwi    r4, r4, 5
    mtctr   r4
_loop_dcsns:
    dcbst   r0, r3
    addi    r3, r3, 32
    bdnz    _loop_dcsns
    blr
}
#pragma pop

/*
 * DCZeroRange - Zero out a range of data cache lines.
 *
 * Zeroes each 32-byte cache line covering [addr, addr+nBytes).
 *
 * 0x8009B388 | size: 0x2C
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void DCZeroRange(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi  r4, 0
    blelr
    clrlwi  r5, r3, 27
    add     r4, r4, r5
    addi    r4, r4, 31
    srwi    r4, r4, 5
    mtctr   r4
_loop_dcz:
    dcbz    r0, r3
    addi    r3, r3, 32
    bdnz    _loop_dcz
    blr
}
#pragma pop

/*
 * LCEnable - Enable the locked cache (L2 scratch area).
 *
 * Flushes the lower 32KB of cached memory, enables LC in HID2,
 * sets up DBAT3 for the LC address range 0xE0000000, and zeroes
 * all 512 locked cache lines.
 *
 * 0x8009B40C | size: 0xCC
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void LCEnable(void) {
    nofralloc
    mfmsr   r5
    ori     r5, r5, 0x1000
    mtmsr   r5
    lis     r3, 0x8000
    li      r4, 0x400
    mtctr   r4
_loop_lce_flush:
    dcbt    r0, r3
    dcbst   r0, r3
    addi    r3, r3, 32
    bdnz    _loop_lce_flush
    mfspr   r4, HID2
    oris    r4, r4, 0x100F
    mtspr   HID2, r4
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    lis     r3, 0xE000
    ori     r3, r3, 0x0002
    mtdbatl 3, r3
    ori     r3, r3, 0x01FE
    mtdbatu 3, r3
    isync
    lis     r3, 0xE000
    li      r6, 512
    mtctr   r6
    li      r6, 0
_loop_lce_zero:
    dcbz_l  r6, r3
    addi    r3, r3, 32
    bdnz    _loop_lce_zero
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    blr
}
#pragma pop

/*
 * LCEnableNoInterrupts - Enable locked cache with interrupts disabled.
 *
 * Wraps __LCEnable (exported as LCEnable in asm) with interrupt disable/restore.
 *
 * 0x8009B4D8 | size: 0x38
 */
void LCEnableNoInterrupts(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    LCEnable();
    OSRestoreInterrupts(enabled);
}

/*
 * LCLoadData - Initiate a locked cache DMA load.
 *
 * Constructs DMA_U/DMA_L register values from the source address,
 * destination address, and block count, then starts the transfer.
 *
 * 0x8009B538 | size: 0x24
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void LCLoadData(register void* destAddr, register void* srcAddr, register u32 nBlocks) {
    nofralloc
    extrwi  r6, r5, 5, 25
    clrlwi  r3, r3, 4
    or      r6, r6, r3
    mtspr   DMA_U, r6
    or      r6, r6, r4
    ori     r6, r6, 0x0002
    mtspr   DMA_L, r6
    blr
}
#pragma pop

/*
 * LCLoadBlocks - Load multiple DMA blocks into the locked cache.
 *
 * Splits large transfers into 128-block (0x1000 byte) chunks,
 * calling LCLoadData for each chunk. Returns the number of
 * full 128-block transfers performed.
 *
 * 0x8009B55C | size: 0xAC
 */
u32 LCLoadBlocks(void* destAddr, void* srcAddr, u32 nBytes) {
    u32 dest = (u32)destAddr;
    u32 src = (u32)srcAddr;
    u32 blocks = (nBytes + 0x1F) >> 5;
    u32 fullTransfers = (blocks + 0x7F) >> 7;

    while (blocks != 0) {
        if (blocks < 0x80) {
            LCLoadData((void*)dest, (void*)src, blocks);
            blocks = 0;
        } else {
            LCLoadData((void*)dest, (void*)src, 0);
            dest += 0x1000;
            src += 0x1000;
            blocks -= 0x80;
        }
    }

    return fullTransfers;
}

/*
 * LCQueueLength - Get the current locked cache DMA queue length.
 *
 * 0x8009B608 | size: 0xC
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm u32 LCQueueLength(void) {
    nofralloc
    mfspr   r4, HID2
    rlwinm  r3, r4, 8, 28, 31
    blr
}
#pragma pop

/*
 * LCQueueWait - Wait for the locked cache DMA queue to drain.
 *
 * Polls the DMA queue length field in HID2 until it is less than
 * or equal to the requested threshold.
 *
 * 0x8009B614 | size: 0x14
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void LCQueueWait(register u32 threshold) {
    nofralloc
_loop_lcqw:
    mfspr   r4, HID2
    extrwi  r4, r4, 4, 4
    cmpw    r4, r3
    bgt     _loop_lcqw
    blr
}
#pragma pop

