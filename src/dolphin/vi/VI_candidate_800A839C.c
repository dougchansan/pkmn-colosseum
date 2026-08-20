/** Candidate-only owner for 0x800A839C - 0x800A880C. */
#include "src/dolphin/vi/VI_range_800A8178.c"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"

typedef struct {
    u8 diskID[0x20];
    u8 padding[0x18];
    void* fstLocation;
    u32 fstMaxLength;
} FSTBootInfo;

typedef struct {
    u32 bootFilePosition;
    u32 fstPosition;
    u32 fstLength;
    u32 fstMaxLength;
    void* fstAddress;
} FSTBootBlock;

extern u8 bb2Buf[];
extern DVDCommandBlock fstLoadBlock;
extern FSTBootBlock* bb2_8047A83C;
extern DVDDiskID* idTmp_8047A840;
extern void* OSGetArenaHi(void);
extern void OSSetArenaHi(void* arenaHi);
extern void OSReport(const char* format, ...);
extern void* memcpy(void* destination, const void* source, u32 length);

typedef struct VIContextCandidate {
    u16 viRegs[59];
    u8 _76[2];
    u16 viShdwRegs[59];
    u8 _EE[2];
    SomeVIStruct HorVer;
} VIContextCandidate;

extern VIContextCandidate lbl_803FC488;
extern volatile u64 lbl_8047A880; /* shdwChanged */
#define shdwRegs lbl_803FC488.viShdwRegs
#define __VIRegs ((volatile u16*)0xCC002000)

void __fstLoad(void)
{
    FSTBootInfo* bootInfo;
    DVDDiskID* id;
    u8 idTmpBuffer[sizeof(DVDDiskID) + 31];
    void* arenaHi;

    arenaHi = OSGetArenaHi();
    bootInfo = (FSTBootInfo*)0x80000000;
    idTmp_8047A840 =
        (DVDDiskID*)(((u32)idTmpBuffer + 31) & ~31);
    bb2_8047A83C = (FSTBootBlock*)(((u32)bb2Buf + 31) & ~31);
    DVDReset();
    DVDReadDiskID(&fstLoadBlock, idTmp_8047A840, cb);
    while (DVDGetDriveStatus() != 0) {
    }
    bootInfo->fstLocation = bb2_8047A83C->fstAddress;
    bootInfo->fstMaxLength = bb2_8047A83C->fstMaxLength;
    id = (DVDDiskID*)bootInfo;
    memcpy(id, idTmp_8047A840, sizeof(DVDDiskID));
    OSReport("\n");
    OSReport("  Game Name ... %c%c%c%c\n", id->gameName[0], id->gameName[1],
             id->gameName[2], id->gameName[3]);
    OSReport("  Company ..... %c%c\n", id->company[0], id->company[1]);
    OSReport("  Disk # ...... %d\n", id->diskNumber);
    OSReport("  Game ver .... %d\n", id->gameVersion);
    OSReport("  Streaming ... %s\n", id->streaming == 0 ? "OFF" : "ON");
    OSReport("\n");
    OSSetArenaHi(bb2_8047A83C->fstAddress);
}

void fn_800A85DC(__OSInterrupt unused, OSContext* context)
{
    extern volatile u32 lbl_8047A84C;  /* retraceCount */
    extern volatile u32 lbl_8047A850;  /* flushFlag */
    extern OSThreadQueue lbl_8047A854; /* retraceQueue */
    extern void (*lbl_8047A85C)(u32);  /* PreCB */
    extern void (*lbl_8047A860)(u32);  /* PostCB */
    extern volatile u32 lbl_8047A878;  /* shdwChangeMode */
    extern volatile u64 lbl_8047A880;  /* shdwChanged */
    extern VITiming* lbl_8047A888;     /* CurrTiming */
    extern u32 CurrTvMode_8047A88C;
    extern u32 lbl_8047A890;
    extern u32 lbl_8047A894;
    extern void fn_800D104C(void);
    extern u32 getCurrentFieldEvenOdd(void);
    OSContext exceptionContext;
    u16 reg;
    u32 inter;
    s32 regIndex;
    BOOL flushed;

    (void)unused;

    inter = 0;

    reg = __VIRegs[0x18];
    if (reg & 0x8000) {
        __VIRegs[0x18] = reg & ~0x8000;
        inter |= 1;
    }

    reg = __VIRegs[0x1A];
    if (reg & 0x8000) {
        __VIRegs[0x1A] = reg & ~0x8000;
        inter |= 2;
    }

    reg = __VIRegs[0x1C];
    if (reg & 0x8000) {
        __VIRegs[0x1C] = reg & ~0x8000;
        inter |= 4;
    }

    reg = __VIRegs[0x1E];
    if (reg & 0x8000) {
        __VIRegs[0x1E] = reg & ~0x8000;
        inter |= 8;
    }

    if ((inter & 4) || (inter & 8)) {
        OSSetCurrentContext(context);
        return;
    }

    lbl_8047A84C += 1;
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);

    if (lbl_8047A85C != NULL) {
        lbl_8047A85C(lbl_8047A84C);
    }

    if (lbl_8047A850 != 0) {
        flushed = TRUE;
        if (lbl_8047A878 == 1 && getCurrentFieldEvenOdd() == 0) {
            flushed = FALSE;
        } else {
            while (lbl_8047A880 != 0) {
                regIndex = cntlzd(lbl_8047A880);
                __VIRegs[regIndex] = shdwRegs[regIndex];
                lbl_8047A880 &= ~((u64)1 << (63 - regIndex));
            }
            lbl_8047A878 = 0;
            lbl_8047A888 = lbl_803FC488.HorVer.timing;
            CurrTvMode_8047A88C = lbl_803FC488.HorVer.tv;
            lbl_8047A894 = lbl_8047A890;
        }

        if (flushed) {
            lbl_8047A850 = 0;
            fn_800D104C();
        }
    }

    if (lbl_8047A860 != NULL) {
        OSClearContext(&exceptionContext);
        lbl_8047A860(lbl_8047A84C);
    }

    OSWakeupThread(&lbl_8047A854);
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}
