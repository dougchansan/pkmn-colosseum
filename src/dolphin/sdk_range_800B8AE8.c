/**
 * @file sdk_range_800B8AE8.c
 * @brief GXMisc.c: GXAbortFrame (GXAbortFrame), GXSetDrawSync (GXSetDrawSync)
 * and GXSetDrawDone, 0x800B8AE8 - 0x800B8DA8.
 *
 * GXFlush and __GXAbort are linked in gx/GX_exact_800B884C.c; retail inlines
 * both here, so this carve keeps static inline copies. Forms follow the
 * Dolphin SDK's GXMisc.c as decompiled in XD:
 * https://github.com/TeamOrre/xd-decomp/blob/4989794e6c6430684e033bc56f4bb97c9a921e73/src/dolphin/gx/GXMisc.c
 */

#include "dolphin/types.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/os/PPCArch.h"

typedef struct GXData_800B8AE8 {
    /* 0x000 */ u16 field_000;
    /* 0x002 */ u16 bpSentNot;
    /* 0x004 */ u8 pad_004[0x4EE];
    /* 0x4F2 */ u8 abtWaitPECopy;
    /* 0x4F3 */ u8 pad_4F3;
    /* 0x4F4 */ u32 dirtyState;
} GXData_800B8AE8;

extern GXData_800B8AE8* const gx;
extern void* __memReg;
extern volatile u8 lbl_8047A9C8; /* DrawDone */

extern u32 GXGetGPFifo(void); /* GXGetGPFifo */
extern void __GXCleanGPFifo(void);
extern void __GXSetDirtyState(void); /* __GXSetDirtyState */

typedef union GXWGPipe_800B8AE8 {
    u8 u8;
    u32 u32;
} GXWGPipe_800B8AE8;

volatile GXWGPipe_800B8AE8 GXWGFifo_800B8AE8 : 0xCC008000;
volatile u32 __PIRegs[12] : 0xCC003000;

#define GX_WRITE_U8(v)  (GXWGFifo_800B8AE8.u8 = (v))
#define GX_WRITE_U32(v) (GXWGFifo_800B8AE8.u32 = (v))
#define GX_WRITE_RAS_REG(v) \
    do {                    \
        GX_WRITE_U8(0x61);  \
        GX_WRITE_U32(v);    \
    } while (0)
#define GX_GET_MEM_REG(offset) (*(volatile u16*)((volatile u16*)__memReg + (offset)))

static inline void GXFlush(void)
{
    if (gx->dirtyState) {
        __GXSetDirtyState();
    }

    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);

    PPCSync();
}

static inline u32 __GXReadMEMCounterU32(u32 regAddrL, u32 regAddrH)
{
    u32 ctrH0;
    u32 ctrH1;
    u32 ctrL;

    ctrH0 = GX_GET_MEM_REG(regAddrH);
    do {
        ctrH1 = ctrH0;
        ctrL = GX_GET_MEM_REG(regAddrL);
        ctrH0 = GX_GET_MEM_REG(regAddrH);
    } while (ctrH0 != ctrH1);

    return (ctrH0 << 16) | ctrL;
}

static inline void __GXAbortWait(u32 clocks)
{
    OSTime time0;
    OSTime time1;

    time0 = OSGetTime();
    do {
        time1 = OSGetTime();
    } while (time1 - time0 <= (clocks / 4));
}

static inline void __GXAbortWaitPECopyDone(void)
{
    u32 peCnt0;
    u32 peCnt1;

    peCnt0 = __GXReadMEMCounterU32(0x28, 0x27);
    do {
        peCnt1 = peCnt0;
        __GXAbortWait(32);
        peCnt0 = __GXReadMEMCounterU32(0x28, 0x27);
    } while (peCnt0 != peCnt1);
}

static inline void __GXAbort(void)
{
    if (gx->abtWaitPECopy && GXGetGPFifo() != 0) {
        __GXAbortWaitPECopyDone();
    }

    __PIRegs[0x18 / 4] = 1;
    __GXAbortWait(200);
    __PIRegs[0x18 / 4] = 0;
    __GXAbortWait(20);
}

void GXAbortFrame(void)
{
    __GXAbort();
    __GXCleanGPFifo();
}

void GXSetDrawSync(u16 token)
{
    BOOL enabled;
    u32 reg;

    enabled = OSDisableInterrupts();
    reg = token | 0x48000000;
    GX_WRITE_RAS_REG(reg);
    reg = (reg & ~0xFFFFU) | token;
    reg = (reg & 0xFFFFFFU) | 0x47000000;
    GX_WRITE_RAS_REG(reg);
    GXFlush();
    OSRestoreInterrupts(enabled);
    gx->bpSentNot = 0;
}

void GXSetDrawDone(void)
{
    u32 reg;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    reg = 0x45000002;
    GX_WRITE_RAS_REG(reg);
    GXFlush();
    lbl_8047A9C8 = 0;
    OSRestoreInterrupts(enabled);
}
