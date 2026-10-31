/**
 * @file gs_thread_exact_800F0030.c
 * @brief GSthread register save/restore and context-switch primitives.
 *
 * Address range: 0x800F0030 - 0x800F036C (the head of the retail GSthread
 * translation unit), in address order: threadLoadGPRRegisters,
 * threadLoadFPRRegisters, threadSaveGPRRegisters, threadSaveFPRRegisters,
 * threadExecute, GSthreadGetArg, _threadSwitch.
 *
 * Every routine here was written in assembly by the original developers:
 * they save and restore r0-r31/f0-f31 wholesale, swap the stack pointer and
 * resume through LR/CTR. They are admitted as first-party assembly; the
 * evidence, including a compiler probe per routine, is
 * docs/asm_evidence/gs_thread.md (registered in
 * docs/asm_evidence/registry.json).
 *
 * gs_thread.c includes this file so the candidate units built from it keep
 * seeing the same definitions.
 */

#include "dolphin/types.h"

extern u32 lbl_8047AC10;
extern u32 lbl_8047AC14;
extern u32 lbl_8047AC18;
extern u32 lbl_8047AC1C;
extern u32 lbl_8047AC20;
extern u32 lbl_8047AC24;

void threadLoadGPRRegisters(void);
void threadLoadFPRRegisters(void);
void threadSaveGPRRegisters(void);
void threadSaveFPRRegisters(void);

/* First-party assembly; evidence: docs/asm_evidence/gs_thread.md */
asm void threadLoadGPRRegisters(void) {
    nofralloc
    stwu r1, -0x8(r1)
    stw r3, 0x8(r1)
    lwz r3, lbl_8047AC1C
    lwz r0, 0x0(r3)
    lwz r2, 0x8(r3)
    lwz r4, 0x10(r3)
    lwz r5, 0x14(r3)
    lwz r6, 0x18(r3)
    lwz r7, 0x1c(r3)
    lwz r8, 0x20(r3)
    lwz r9, 0x24(r3)
    lwz r10, 0x28(r3)
    lwz r11, 0x2c(r3)
    lwz r12, 0x30(r3)
    lwz r13, 0x34(r3)
    lwz r14, 0x38(r3)
    lwz r15, 0x3c(r3)
    lwz r16, 0x40(r3)
    lwz r17, 0x44(r3)
    lwz r18, 0x48(r3)
    lwz r19, 0x4c(r3)
    lwz r20, 0x50(r3)
    lwz r21, 0x54(r3)
    lwz r22, 0x58(r3)
    lwz r23, 0x5c(r3)
    lwz r24, 0x60(r3)
    lwz r25, 0x64(r3)
    lwz r26, 0x68(r3)
    lwz r27, 0x6c(r3)
    lwz r28, 0x70(r3)
    lwz r29, 0x74(r3)
    lwz r30, 0x78(r3)
    lwz r31, 0x7c(r3)
    lwz r3, 0x8(r1)
    addi r1, r1, 0x8
    blr
}

/* First-party assembly; evidence: docs/asm_evidence/gs_thread.md */
asm void threadLoadFPRRegisters(void) {
    nofralloc
    stwu r1, -0x8(r1)
    stw r3, 0x8(r1)
    lwz r3, lbl_8047AC1C
    addi r3, r3, 0x88
    lfd f0, 0x0(r3)
    lfd f1, 0x8(r3)
    lfd f2, 0x10(r3)
    lfd f3, 0x18(r3)
    lfd f4, 0x20(r3)
    lfd f5, 0x28(r3)
    lfd f6, 0x30(r3)
    lfd f7, 0x38(r3)
    lfd f8, 0x40(r3)
    lfd f9, 0x48(r3)
    lfd f10, 0x50(r3)
    lfd f11, 0x58(r3)
    lfd f12, 0x60(r3)
    lfd f13, 0x68(r3)
    lfd f14, 0x70(r3)
    lfd f15, 0x78(r3)
    lfd f16, 0x80(r3)
    lfd f17, 0x88(r3)
    lfd f18, 0x90(r3)
    lfd f19, 0x98(r3)
    lfd f20, 0xa0(r3)
    lfd f21, 0xa8(r3)
    lfd f22, 0xb0(r3)
    lfd f23, 0xb8(r3)
    lfd f24, 0xc0(r3)
    lfd f25, 0xc8(r3)
    lfd f26, 0xd0(r3)
    lfd f27, 0xd8(r3)
    lfd f28, 0xe0(r3)
    lfd f29, 0xe8(r3)
    lfd f30, 0xf0(r3)
    lfd f31, 0xf8(r3)
    lwz r3, 0x8(r1)
    addi r1, r1, 0x8
    blr
}

/* First-party assembly; evidence: docs/asm_evidence/gs_thread.md */
asm void threadSaveGPRRegisters(void) {
    nofralloc
    stwu r1, -0x8(r1)
    stw r3, 0x8(r1)
    lwz r3, lbl_8047AC1C
    stw r0, 0x0(r3)
    stw r2, 0x8(r3)
    stw r3, 0xc(r3)
    stw r4, 0x10(r3)
    stw r5, 0x14(r3)
    stw r6, 0x18(r3)
    stw r7, 0x1c(r3)
    stw r8, 0x20(r3)
    stw r9, 0x24(r3)
    stw r10, 0x28(r3)
    stw r11, 0x2c(r3)
    stw r12, 0x30(r3)
    stw r13, 0x34(r3)
    stw r14, 0x38(r3)
    stw r15, 0x3c(r3)
    stw r16, 0x40(r3)
    stw r17, 0x44(r3)
    stw r18, 0x48(r3)
    stw r19, 0x4c(r3)
    stw r20, 0x50(r3)
    stw r21, 0x54(r3)
    stw r22, 0x58(r3)
    stw r23, 0x5c(r3)
    stw r24, 0x60(r3)
    stw r25, 0x64(r3)
    stw r26, 0x68(r3)
    stw r27, 0x6c(r3)
    stw r28, 0x70(r3)
    stw r29, 0x74(r3)
    stw r30, 0x78(r3)
    stw r31, 0x7c(r3)
    lwz r3, 0x8(r1)
    addi r1, r1, 0x8
    blr
}

/* First-party assembly; evidence: docs/asm_evidence/gs_thread.md */
asm void threadSaveFPRRegisters(void) {
    nofralloc
    stwu r1, -0x8(r1)
    stw r3, 0x8(r1)
    lwz r3, lbl_8047AC1C
    addi r3, r3, 0x88
    stfd f0, 0x0(r3)
    stfd f1, 0x8(r3)
    stfd f2, 0x10(r3)
    stfd f3, 0x18(r3)
    stfd f4, 0x20(r3)
    stfd f5, 0x28(r3)
    stfd f6, 0x30(r3)
    stfd f7, 0x38(r3)
    stfd f8, 0x40(r3)
    stfd f9, 0x48(r3)
    stfd f10, 0x50(r3)
    stfd f11, 0x58(r3)
    stfd f12, 0x60(r3)
    stfd f13, 0x68(r3)
    stfd f14, 0x70(r3)
    stfd f15, 0x78(r3)
    stfd f16, 0x80(r3)
    stfd f17, 0x88(r3)
    stfd f18, 0x90(r3)
    stfd f19, 0x98(r3)
    stfd f20, 0xa0(r3)
    stfd f21, 0xa8(r3)
    stfd f22, 0xb0(r3)
    stfd f23, 0xb8(r3)
    stfd f24, 0xc0(r3)
    stfd f25, 0xc8(r3)
    stfd f26, 0xd0(r3)
    stfd f27, 0xd8(r3)
    stfd f28, 0xe0(r3)
    stfd f29, 0xe8(r3)
    stfd f30, 0xf0(r3)
    stfd f31, 0xf8(r3)
    lwz r3, 0x8(r1)
    addi r1, r1, 0x8
    blr
}

/* First-party assembly; evidence: docs/asm_evidence/gs_thread.md */
asm void threadExecute(void) {
    nofralloc
    stwu r1, -0xc(r1)
    stw r3, 0xc(r1)
    mflr r3
    stw r3, 0x8(r1)
    lwz r3, lbl_8047AC24
    stw r3, lbl_8047AC1C
    bl threadSaveGPRRegisters
    stw r1, 0x4(r3)
    lwz r3, lbl_8047AC20
    stw r3, lbl_8047AC1C
    bl threadLoadGPRRegisters
    lwz r5, lbl_8047AC10
    cmplwi r5, 0x0
    beq L_800F02C8
    bl threadLoadFPRRegisters
L_800F02C8:
    lwz r5, 0x84(r3)
    mtlr r5
    lwz r1, 0x4(r3)
    li r5, 0x0
    subi r5, r5, 0x1
    stw r5, 0x0(r1)
    lwz r5, 0x80(r3)
    mtctr r5
    lwz r5, 0x14(r3)
    lwz r3, 0xc(r3)
    bctr
}

/* First-party assembly; evidence: docs/asm_evidence/gs_thread.md */
asm void GSthreadGetArg(void) {
    nofralloc
    lwz r3, 0x8(r1)
    mtlr r3
    lwz r3, 0xc(r1)
    addi r1, r1, 0xc
    blr
}

/* First-party assembly; evidence: docs/asm_evidence/gs_thread.md */
asm void _threadSwitch(void) {
    nofralloc
    stw r3, lbl_8047AC18
    stw r5, lbl_8047AC14
    mflr r5
    lwz r3, lbl_8047AC20
    stw r3, lbl_8047AC1C
    bl threadSaveGPRRegisters
    stw r5, 0x80(r3)
    lwz r5, lbl_8047AC18
    stw r5, 0xc(r3)
    lwz r5, lbl_8047AC14
    stw r5, 0x14(r3)
    stw r1, 0x4(r3)
    lwz r5, lbl_8047AC10
    cmplwi r5, 0x0
    beq L_800F0348
    bl threadSaveFPRRegisters
L_800F0348:
    lwz r3, lbl_8047AC24
    stw r3, lbl_8047AC1C
    bl threadLoadGPRRegisters
    lwz r5, 0x84(r3)
    mtlr r5
    lwz r1, 0x4(r3)
    lwz r5, 0x14(r3)
    lwz r3, 0xc(r3)
    blr
}
