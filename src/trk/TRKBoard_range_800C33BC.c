#include "dolphin/types.h"

extern void OSReport(const char* fmt, ...);

/* Comm table */
extern u32 gDBCommTable[];

/* "%s\n" */
extern char lbl_8026FB94[];

#if defined(TRK_BOARD_800C33BC_800C3414)

/* Shared BSS starts at this word, preserving the original eight-byte owner
 * alignment. DTK's address-qualified alias keeps the local symbol traceable. */
extern s32 TRK_mainError_803FED60;

/* TRK_main - 0x800C33BC | size 0x58 | scope global */
void TRK_main(void) {
    s32 error;
    extern void MWTRACE(s32 level, const char* fmt, ...);
    extern s32 TRKInitializeNub(void);
    extern void TRKNubWelcome(void);
    extern void TRKNubMainLoop(void);
    extern s32 TRKTerminateNub(void);

    MWTRACE(1, "TRK_Main \n");
    error = TRKInitializeNub();
    TRK_mainError_803FED60 = error;
    if (error == 0) {
        TRKNubWelcome();
        TRKNubMainLoop();
    }
    error = TRKTerminateNub();
    TRK_mainError_803FED60 = error;
}

#endif

#if defined(TRK_BOARD_800C3414_800C349C)

extern void TRKInterruptHandler(void);

/* TRKLoadContext - 0x800C3414 | size: 0x88
 * MetroTRK dolphin_trk_glue.c: loads an OSContext and enters the debugger
 * through TRKInterruptHandler.
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_loadcontext.md */
asm void TRKLoadContext(register void* ctx, register u32 exceptionID) {
    nofralloc
    lwz r0, 0x0(r3)
    lwz r1, 0x4(r3)
    lwz r2, 0x8(r3)
    lhz r5, 0x1a2(r3)
    rlwinm. r6, r5, 0, 30, 30
    beq _load_volatile
    rlwinm r5, r5, 0, 31, 29
    sth r5, 0x1a2(r3)
    lmw r5, 0x14(r3)
    b _load_done
_load_volatile:
    lmw r13, 0x34(r3)
_load_done:
    mr r31, r3
    mr r3, r4
    lwz r4, 0x80(r31)
    mtcrf 255, r4
    lwz r4, 0x84(r31)
    mtlr r4
    lwz r4, 0x88(r31)
    mtctr r4
    lwz r4, 0x8c(r31)
    mtxer r4
    mfmsr r4
    rlwinm r4, r4, 0, 17, 15
    rlwinm r4, r4, 0, 31, 29
    mtmsr r4
    mtsprg 1, r2
    lwz r4, 0xc(r31)
    mtsprg 2, r4
    lwz r4, 0x10(r31)
    mtsprg 3, r4
    lwz r2, 0x198(r31)
    lwz r4, 0x19c(r31)
    lwz r31, 0x7c(r31)
    b TRKInterruptHandler
}

#endif

#if defined(TRK_BOARD_800C349C_800C3588)

/* MetroTRK program-end marker copied immediately after PPCHalt. */
const u32 EndofProgramInstruction[] = { 0x00454E44 };

/* TRKUARTInterruptHandler - 0x800C349C | size 0x4 | scope global (empty, returns void) */
void TRKUARTInterruptHandler(void) {
}

/* InitializeProgramEndTrap - 0x800C34A0 | size 0x58 | scope global */
void InitializeProgramEndTrap(void) {
    extern void TRK_memcpy(void* dst, const void* src, u32 size);
    extern void ICInvalidateRange(void* addr, u32 size);
    extern void DCFlushRange(void* addr, u32 size);
    extern void PPCHalt(void);
    u32* halt = (u32*)PPCHalt;

    TRK_memcpy(halt + 1, EndofProgramInstruction, 4);
    ICInvalidateRange(halt + 1, 4);
    DCFlushRange(halt + 1, 4);
}

/* TRK_board_display - 0x800C34F8 | size 0x30 | scope global */
void TRK_board_display(const char* msg) {
    OSReport(lbl_8026FB94, msg);
}

/* UnreserveEXI2Port - 0x800C3528 | size 0x30 | scope global */
void UnreserveEXI2Port(void) {
    typedef void (*CommFunc)(void);
    CommFunc func = (CommFunc)((u32*)gDBCommTable)[8]; /* offset 0x20 */
    func();
}

/* ReserveEXI2Port - 0x800C3558 | size 0x30 | scope global */
void ReserveEXI2Port(void) {
    typedef void (*CommFunc)(void);
    CommFunc func = (CommFunc)((u32*)gDBCommTable)[9]; /* offset 0x24 */
    func();
}

#endif
