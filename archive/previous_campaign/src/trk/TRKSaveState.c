#include "dolphin/types.h"

/*
 * TRKSaveState.c - Save and restore extended CPU state.
 *
 * These functions save/restore the full PowerPC state including
 * segment registers, BAT registers, SPRs, and GCN-specific
 * registers (GQRs, HID2, etc.). They must be pure assembly
 * because they directly manipulate privileged SPRs.
 */

extern u8 gTRKCPUState[];  /* 0x340+ bytes of saved CPU context */
extern u8 gTRKRestoreFlags[]; /* 2 bytes: [0] = restore TBL, [1] = restore DEC */

/*
 * TRKSaveExtended1Block - Save all extended CPU state.
 *
 * Saves:
 *   - Segment registers (SR0-SR15) at offset 0x1A8
 *   - Time base, HID0/1, SRR1, PVR, BAT registers at offset 0x1E8
 *   - SDR1, DAR, DSISR, SPRGs, IABR, EAR at offset 0x25C
 *   - GQR0-7, HID2, WPAR, DMA_U, DMA_L at offset 0x2FC
 *   - Performance monitor registers at offset 0x284
 */
asm void TRKSaveExtended1Block(void) {
    nofralloc

    /* Load gTRKCPUState base address into r2 */
    lis     r2, gTRKCPUState@h
    ori     r2, r2, gTRKCPUState@l

    /* Save segment registers SR0-SR15 into r16-r31, then store */
    mfsr    r16, 0
    mfsr    r17, 1
    mfsr    r18, 2
    mfsr    r19, 3
    mfsr    r20, 4
    mfsr    r21, 5
    mfsr    r22, 6
    mfsr    r23, 7
    mfsr    r24, 8
    mfsr    r25, 9
    mfsr    r26, 10
    mfsr    r27, 11
    mfsr    r28, 12
    mfsr    r29, 13
    mfsr    r30, 14
    mfsr    r31, 15
    stmw    r16, 0x1A8(r2)

    /* Save time base and system SPRs */
    mfspr   r10, 268
    mfspr   r11, 269
    mfspr   r12, HID0
    mfspr   r13, HID1
    mfsrr1  r14
    mfspr   r15, PVR
    mfibatu r16, 0
    mfibatl r17, 0
    mfibatu r18, 1
    mfibatl r19, 1
    mfibatu r20, 2
    mfibatl r21, 2
    mfibatu r22, 3
    mfibatl r23, 3
    mfdbatu r24, 0
    mfdbatl r25, 0
    mfdbatu r26, 1
    mfdbatl r27, 1
    mfdbatu r28, 2
    mfdbatl r29, 2
    mfdbatu r30, 3
    mfdbatl r31, 3
    stmw    r10, 0x1E8(r2)

    /* Save SDR1, DAR, DSISR, SPRGs, IABR, EAR */
    mfsdr1  r22
    mfdar   r23
    mfdsisr r24
    mfsprg  r25, 0
    mfsprg  r26, 1
    mfsprg  r27, 2
    mfsprg  r28, 3
    li      r29, 0
    mfspr   r30, IABR
    mfear   r31
    stmw    r22, 0x25C(r2)

    /* Save GCN-specific registers */
    mfspr   r20, GQR0
    mfspr   r21, GQR1
    mfspr   r22, GQR2
    mfspr   r23, GQR3
    mfspr   r24, GQR4
    mfspr   r25, GQR5
    mfspr   r26, GQR6
    mfspr   r27, GQR7
    mfspr   r28, HID2
    mfspr   r29, WPAR
    mfspr   r30, DMA_U
    mfspr   r31, DMA_L
    stmw    r20, 0x2FC(r2)

    /* Skip performance counter block (unreachable code in original) */
    b       @save_perf

    /* This block is unreachable in the original binary but present */
    mfspr   r16, 928
    mfspr   r17, 935
    mfspr   r18, UMMCR0
    mfspr   r19, UPMC1
    mfspr   r20, UPMC2
    mfspr   r21, USIA
    mfspr   r22, UMMCR1
    mfspr   r23, UPMC3
    mfspr   r24, UPMC4
    mfspr   r25, 947
    mfspr   r26, 944
    mfspr   r27, 951
    mfspr   r28, SDA
    mfspr   r29, 1014
    mfspr   r30, 1015
    mfspr   r31, 1023
    stmw    r16, 0x2B8(r2)

@save_perf:
    /* Save performance and thermal SPRs */
    mfspr   r19, DABR
    mfspr   r20, PMC1
    mfspr   r21, PMC2
    mfspr   r22, PMC3
    mfspr   r23, PMC4
    mfspr   r24, SIA
    mfspr   r25, MMCR0
    mfspr   r26, MMCR1
    mfspr   r27, THRM1
    mfspr   r28, THRM2
    mfspr   r29, THRM3
    mfspr   r30, ICTC
    mfspr   r31, L2CR
    stmw    r19, 0x284(r2)
    blr

    /* Unreachable: save IABR-related SPRs and DEC */
    mfspr   r25, 976
    mfspr   r26, 977
    mfspr   r27, 978
    mfspr   r28, 979
    mfspr   r29, 980
    mfspr   r30, 981
    mfspr   r31, 982
    stmw    r25, 0x240(r2)
    mfdec   r31
    stw     r31, 0x278(r2)
    blr
}

/*
 * TRKRestoreExtended1Block - Restore all extended CPU state.
 *
 * Conditionally restores time base and decrementer based on
 * gTRKRestoreFlags, then restores all other saved SPRs.
 */
asm void TRKRestoreExtended1Block(void) {
    nofralloc

    lis     r2, gTRKCPUState@h
    ori     r2, r2, gTRKCPUState@l

    /* Load restore flags */
    lis     r5, gTRKRestoreFlags@h
    ori     r5, r5, gTRKRestoreFlags@l
    lbz     r3, 0(r5)          /* restore TBL flag */
    lbz     r6, 1(r5)          /* restore DEC flag */

    /* Clear flags */
    li      r0, 0
    stb     r0, 0(r5)
    stb     r0, 1(r5)

    /* Conditionally restore time base */
    cmpwi   r3, 0
    beq     @skip_tbl
    lwz     r24, 0x1E8(r2)     /* TBL */
    lwz     r25, 0x1EC(r2)     /* TBU */
    mtspr   284, r24
    mtspr   285, r25

@skip_tbl:
    /* Restore GCN-specific registers */
    lmw     r20, 0x2FC(r2)
    mtspr   GQR0, r20
    mtspr   GQR1, r21
    mtspr   GQR2, r22
    mtspr   GQR3, r23
    mtspr   GQR4, r24
    mtspr   GQR5, r25
    mtspr   GQR6, r26
    mtspr   GQR7, r27
    mtspr   HID2, r28
    mtspr   DMA_U, r30
    mtspr   DMA_L, r31

    /* Skip unreachable block */
    b       @restore_perf

    /* Unreachable block */
    lmw     r26, 0x2E0(r2)
    mtspr   944, r26
    mtspr   951, r27
    mtspr   1014, r29
    mtspr   1015, r30
    mtspr   1023, r31

@restore_perf:
    /* Restore performance and thermal SPRs */
    lmw     r19, 0x284(r2)
    mtspr   DABR, r19
    mtspr   PMC1, r20
    mtspr   PMC2, r21
    mtspr   PMC3, r22
    mtspr   PMC4, r23
    mtspr   SIA, r24
    mtspr   MMCR0, r25
    mtspr   MMCR1, r26
    mtspr   THRM1, r27
    mtspr   THRM2, r28
    mtspr   THRM3, r29
    mtspr   ICTC, r30
    mtspr   L2CR, r31

    /* Skip unreachable block */
    b       @restore_sr

    /* Unreachable: conditionally restore DEC */
    cmpwi   r6, 0
    beq     @skip_dec
    lwz     r26, 0x278(r2)
    mtdec   r26
@skip_dec:
    lmw     r25, 0x240(r2)
    mtspr   976, r25
    mtspr   977, r26
    mtspr   978, r27
    mtspr   979, r28
    mtspr   980, r29
    mtspr   981, r30
    mtspr   982, r31

@restore_sr:
    /* Restore segment registers */
    lmw     r16, 0x1A8(r2)
    mtsr    0, r16
    mtsr    1, r17
    mtsr    2, r18
    mtsr    3, r19
    mtsr    4, r20
    mtsr    5, r21
    mtsr    6, r22
    mtsr    7, r23
    mtsr    8, r24
    mtsr    9, r25
    mtsr    10, r26
    mtsr    11, r27
    mtsr    12, r28
    mtsr    13, r29
    mtsr    14, r30
    mtsr    15, r31

    /* Restore system SPRs */
    lmw     r12, 0x1F0(r2)
    mtspr   HID0, r12
    mtspr   HID1, r13
    mtsrr1  r14
    /* PVR is read-only; skip restore */
    nop
    mtibatu 0, r16
    mtibatl 0, r17
    mtibatu 1, r18
    mtibatl 1, r19
    mtibatu 2, r20
    mtibatl 2, r21
    mtibatu 3, r22
    mtibatl 3, r23
    mtdbatu 0, r24
    mtdbatl 0, r25
    mtdbatu 1, r26
    mtdbatl 1, r27
    mtdbatu 2, r28
    mtdbatl 2, r29
    mtdbatu 3, r30
    mtdbatl 3, r31

    /* Restore SDR1, DAR, DSISR, SPRGs, IABR, EAR */
    lmw     r22, 0x25C(r2)
    mtsdr1  r22
    mtdar   r23
    mtdsisr r24
    mtsprg  0, r25
    mtsprg  1, r26
    mtsprg  2, r27
    mtsprg  3, r28
    mtspr   IABR, r30
    mtear   r31

    blr
}
