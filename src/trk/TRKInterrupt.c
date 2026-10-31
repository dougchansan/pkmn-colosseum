#include "dolphin/types.h"

typedef struct TRKExceptionStatus {
    u32 words[3];
    u8 pad_0C;
    u8 exceptionDetected;
    u8 pad_0E[2];
} TRKExceptionStatus;

extern TRKExceptionStatus gTRKExceptionStatus_80313824;
extern void TRK__read_aram(void* data, u32 address, u32* length);
extern void TRK__write_aram(void* data, u32 address, u32* length);

extern u8 gTRKState[];
extern u8 gTRKCPUState[];
extern u8 gTRKSaveState[];
extern u16 TRK_saved_exceptionID_803FE7D8;
extern void TRKUARTInterruptHandler(void);
extern void TRKPostInterruptEvent(void);
extern void TRKSaveExtended1Block(void);
extern void TRKRestoreExtended1Block(void);
void TRKExceptionHandler(void);
void TRKInterruptHandlerEnableInterrupts(void);

/* TRKInterruptHandler - 0x800C0EAC | size: 0x194
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_interrupt.md */
asm void TRKInterruptHandler(void) {
    nofralloc
    mtsrr0 r2
    mtsrr1 r4
    mfsprg r4, 3
    mfcr r2
    mtsprg 3, r2
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    lwz r2, 0x8c(r2)
    ori r2, r2, 0x8002
    xori r2, r2, 0x8002
    sync
    mtmsr r2
    sync
    lis r2, TRK_saved_exceptionID_803FE7D8@h
    ori r2, r2, TRK_saved_exceptionID_803FE7D8@l
    sth r3, 0x0(r2)
    cmpwi r3, 0x500
    bne _L800C0F74
    lis r2, gTRKCPUState@h
    ori r2, r2, gTRKCPUState@l
    mflr r3
    stw r3, 0x42c(r2)
    bl TRKUARTInterruptHandler
    lis r2, gTRKCPUState@h
    ori r2, r2, gTRKCPUState@l
    lwz r3, 0x42c(r2)
    mtlr r3
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    lwz r2, 0xa0(r2)
    lbz r2, 0x0(r2)
    cmpwi r2, 0x0
    beq _L800C0F58
    lis r2, gTRKExceptionStatus_80313824@h
    ori r2, r2, gTRKExceptionStatus_80313824@l
    lbz r2, 0xc(r2)
    cmpwi r2, 0x1
    beq _L800C0F58
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    li r3, 0x1
    stb r3, 0x9c(r2)
    b _L800C0F74
_L800C0F58:
    lis r2, gTRKSaveState@h
    ori r2, r2, gTRKSaveState@l
    lwz r3, 0x88(r2)
    mtcrf 255, r3
    lwz r3, 0xc(r2)
    lwz r2, 0x8(r2)
    rfi
_L800C0F74:
    lis r2, TRK_saved_exceptionID_803FE7D8@h
    ori r2, r2, TRK_saved_exceptionID_803FE7D8@l
    lhz r3, 0x0(r2)
    lis r2, gTRKExceptionStatus_80313824@h
    ori r2, r2, gTRKExceptionStatus_80313824@l
    lbz r2, 0xc(r2)
    cmpwi r2, 0x0
    bne TRKExceptionHandler
    lis r2, gTRKCPUState@h
    ori r2, r2, gTRKCPUState@l
    stw r0, 0x0(r2)
    stw r1, 0x4(r2)
    mfsprg r0, 1
    stw r0, 0x8(r2)
    sth r3, 0x2f8(r2)
    sth r3, 0x2fa(r2)
    mfsprg r0, 2
    stw r0, 0xc(r2)
    stmw r4, 0x10(r2)
    mfsrr0 r27
    mflr r28
    mfsprg r29, 3
    mfctr r30
    mfxer r31
    stmw r27, 0x80(r2)
    bl TRKSaveExtended1Block
    lis r2, gTRKExceptionStatus_80313824@h
    ori r2, r2, gTRKExceptionStatus_80313824@l
    li r3, 0x1
    stb r3, 0xc(r2)
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    lwz r0, 0x8c(r2)
    sync
    mtmsr r0
    sync
    lwz r0, 0x80(r2)
    mtlr r0
    lwz r0, 0x84(r2)
    mtctr r0
    lwz r0, 0x88(r2)
    mtxer r0
    lwz r0, 0x94(r2)
    mtdsisr r0
    lwz r0, 0x90(r2)
    mtdar r0
    lmw r3, 0xc(r2)
    lwz r0, 0x0(r2)
    lwz r1, 0x4(r2)
    lwz r2, 0x8(r2)
    b TRKPostInterruptEvent
}

/* TRKExceptionHandler - 0x800C1040 | size: 0x9C
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_interrupt.md */
asm void TRKExceptionHandler(void) {
    nofralloc
    lis r2, gTRKExceptionStatus_80313824@h
    ori r2, r2, gTRKExceptionStatus_80313824@l
    sth r3, 0x8(r2)
    mfsrr0 r3
    stw r3, 0x0(r2)
    lhz r3, 0x8(r2)
    cmpwi r3, 0x200
    beq _L800C10AC
    cmpwi r3, 0x300
    beq _L800C10AC
    cmpwi r3, 0x400
    beq _L800C10AC
    cmpwi r3, 0x600
    beq _L800C10AC
    cmpwi r3, 0x700
    beq _L800C10AC
    cmpwi r3, 0x800
    beq _L800C10AC
    cmpwi r3, 0x1000
    beq _L800C10AC
    cmpwi r3, 0x1100
    beq _L800C10AC
    cmpwi r3, 0x1200
    beq _L800C10AC
    cmpwi r3, 0x1300
    beq _L800C10AC
    b _L800C10B8
_L800C10AC:
    mfsrr0 r3
    addi r3, r3, 0x4
    mtsrr0 r3
_L800C10B8:
    lis r2, gTRKExceptionStatus_80313824@h
    ori r2, r2, gTRKExceptionStatus_80313824@l
    li r3, 0x1
    stb r3, 0xd(r2)
    mfsprg r3, 3
    mtcrf 255, r3
    mfsprg r2, 1
    mfsprg r3, 2
    rfi
}

/* TRKSwapAndGo - 0x800C10DC | size: 0xC4
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_interrupt.md */
asm void TRKSwapAndGo(void) {
    nofralloc
    lis r3, gTRKState@h
    ori r3, r3, gTRKState@l
    stmw r0, 0x0(r3)
    mfmsr r0
    stw r0, 0x8c(r3)
    mflr r0
    stw r0, 0x80(r3)
    mfctr r0
    stw r0, 0x84(r3)
    mfxer r0
    stw r0, 0x88(r3)
    mfdsisr r0
    stw r0, 0x94(r3)
    mfdar r0
    stw r0, 0x90(r3)
    li r1, -0x7ffe
    nor r1, r1, r1
    mfmsr r3
    and r3, r3, r1
    mtmsr r3
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    lwz r2, 0xa0(r2)
    lbz r2, 0x0(r2)
    cmpwi r2, 0x0
    beq _L800C1158
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    li r3, 0x1
    stb r3, 0x9c(r2)
    b TRKInterruptHandlerEnableInterrupts
_L800C1158:
    lis r2, gTRKExceptionStatus_80313824@h
    ori r2, r2, gTRKExceptionStatus_80313824@l
    li r3, 0x0
    stb r3, 0xc(r2)
    bl TRKRestoreExtended1Block
    lis r2, gTRKCPUState@h
    ori r2, r2, gTRKCPUState@l
    lmw r27, 0x80(r2)
    mtsrr0 r27
    mtlr r28
    mtcrf 255, r29
    mtctr r30
    mtxer r31
    lmw r3, 0xc(r2)
    lwz r0, 0x0(r2)
    lwz r1, 0x4(r2)
    lwz r2, 0x8(r2)
    rfi
}

/* TRKInterruptHandlerEnableInterrupts - 0x800C11A0 | size: 0x54
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_interrupt.md */
asm void TRKInterruptHandlerEnableInterrupts(void) {
    nofralloc
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    lwz r0, 0x8c(r2)
    sync
    mtmsr r0
    sync
    lwz r0, 0x80(r2)
    mtlr r0
    lwz r0, 0x84(r2)
    mtctr r0
    lwz r0, 0x88(r2)
    mtxer r0
    lwz r0, 0x94(r2)
    mtdsisr r0
    lwz r0, 0x90(r2)
    mtdar r0
    lmw r3, 0xc(r2)
    lwz r0, 0x0(r2)
    lwz r1, 0x4(r2)
    lwz r2, 0x8(r2)
    b TRKPostInterruptEvent
}

/* ReadFPSCR - 0x800C11F4 | size: 0x24
 * MetroTRK targimpl.c ReadFPSCR (upstream name; the symbol keeps its address).
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_fpscr.md */
asm void ReadFPSCR(register f64* data) {
    nofralloc
    stwu r1, -0x40(r1)
    stfd f31, 0x10(r1)
    psq_st f31, 0x20(r1), 0, 0
    mffs f31
    stfd f31, 0(r3)
    psq_l f31, 0x20(r1), 0, 0
    lfd f31, 0x10(r1)
    addi r1, r1, 0x40
    blr
}

/* WriteFPSCR - 0x800C1218 | size: 0x24
 * MetroTRK targimpl.c WriteFPSCR (upstream name; the symbol keeps its address).
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_fpscr.md */
asm void WriteFPSCR(register f64* data) {
    nofralloc
    stwu r1, -0x40(r1)
    stfd f31, 0x10(r1)
    psq_st f31, 0x20(r1), 0, 0
    lfd f31, 0(r3)
    mtfsf 0xff, f31
    psq_l f31, 0x20(r1), 0, 0
    lfd f31, 0x10(r1)
    addi r1, r1, 0x40
    blr
}

s32 TRKTargetAccessARAM(void* data, u32 address, u32* length, BOOL read)
{
    s32 error = 0;
    TRKExceptionStatus saved = gTRKExceptionStatus_80313824;

    gTRKExceptionStatus_80313824.exceptionDetected = FALSE;
    if (read) {
        TRK__read_aram(data, address, length);
    } else {
        TRK__write_aram(data, address, length);
    }
    if (gTRKExceptionStatus_80313824.exceptionDetected) {
        *length = 0;
        error = 0x702;
    }
    gTRKExceptionStatus_80313824 = saved;
    return error;
}
