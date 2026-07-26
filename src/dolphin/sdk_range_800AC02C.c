/**
 * @file sdk_range_800AC02C.c
 * @brief Dolphin SDK prefix, 0x800AC02C - 0x800ACBFC (28 functions).
 *
 * Mechanical address partition of the recovered Dolphin SDK source.
 */
#include "dolphin/types.h"
#include "crt/string.h"
#include "dolphin/ai/AI.h"
#include "dolphin/ar/AR.h"
#include "dolphin/dvd/dvd.h"
#include "dolphin/exi/EXI.h"
#include "dolphin/os/OSAlarm.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"
#include "dolphin/os/PPCArch.h"

typedef void (*CARDCallback)(s32 chan, s32 result);

typedef void (*CARDCallback)(s32 chan, s32 result);

typedef struct AIDmaRegisters {
    /* 0x00 */ u16 startHi;
    /* 0x02 */ u16 startLo;
    /* 0x04 */ u16 bytesLeft;
    /* 0x06 */ u16 control;
} AIDmaRegisters;

typedef struct AIRegisters {
    /* 0x00 */ u32 control;
    /* 0x04 */ u32 volume;
    /* 0x08 */ u32 sampleCount;
    /* 0x0C */ u32 interruptTiming;
} AIRegisters;

typedef struct DSPRegisters {
    /* 0x00 */ u16 mailToDspHi;
    /* 0x02 */ u16 mailToDspLo;
    /* 0x04 */ u16 mailFromDspHi;
    /* 0x06 */ u16 mailFromDspLo;
    /* 0x08 */ u16 control;
    /* 0x0A */ u16 dmaControl;
    /* 0x0C */ u16 arSize;
    /* 0x0E */ u16 _0e;
    /* 0x10 */ u16 arMode;
    /* 0x12 */ u16 _12;
    /* 0x14 */ u16 _14;
    /* 0x16 */ u16 arRefresh;
    /* 0x18 */ u16 _18;
    /* 0x1A */ u16 aramSize;
    /* 0x1C */ u16 _1c;
    /* 0x1E */ u16 _1e;
    /* 0x20 */ u16 arDmaMainMemAddrHi;
    /* 0x22 */ u16 arDmaMainMemAddrLo;
    /* 0x24 */ u16 arDmaAramAddrHi;
    /* 0x26 */ u16 arDmaAramAddrLo;
    /* 0x28 */ u16 arDmaCntHi;
    /* 0x2A */ u16 arDmaCntLo;
    /* 0x2C */ u16 _2c;
    /* 0x2E */ u16 _2e;
    /* 0x30 */ u16 aiDmaStartHi;
    /* 0x32 */ u16 aiDmaStartLo;
    /* 0x34 */ u16 aiDmaBytesLeft;
    /* 0x36 */ u16 aiDmaControl;
} DSPRegisters;

struct CARDFileInfo;

typedef struct CARDControl {
    /* 0x000 */ s32 attached;
    /* 0x004 */ s32 result;
    /* 0x008 */ u16 size;
    /* 0x00A */ u16 pageSize;
    /* 0x00C */ u32 sectorSize;
    /* 0x010 */ u16 cBlock;
    /* 0x012 */ u8 _012[0x12];
    /* 0x024 */ s32 field_24;
    /* 0x028 */ s32 formatStep;
    /* 0x02C */ u32 scramble;
    /* 0x030 */ u8 task[0x50];
    /* 0x080 */ void* workArea;
    /* 0x084 */ void* dirBlock;
    /* 0x088 */ void* fatBlock;
    /* 0x08C */ u8 _08C[0x08];
    /* 0x094 */ u8 cmd[5];
    /* 0x099 */ u8 _099[0x07];
    /* 0x0A0 */ s32 cmdLen;
    /* 0x0A4 */ s32 field_A4;
    /* 0x0A8 */ s32 field_A8;
    /* 0x0AC */ s32 repeat;
    /* 0x0B0 */ u32 addr;
    /* 0x0B4 */ u8* buffer;
    /* 0x0B8 */ u32 xferred;
    /* 0x0BC */ u16 freeNo;
    /* 0x0BE */ u16 startBlock;
    /* 0x0C0 */ struct CARDFileInfo* fileInfo;
    /* 0x0C4 */ CARDCallback extCallback;
    /* 0x0C8 */ CARDCallback txCallback;
    /* 0x0CC */ CARDCallback callback_CC;
    /* 0x0D0 */ CARDCallback apiCallback;
    /* 0x0D4 */ CARDCallback xferCallback;
    /* 0x0D8 */ CARDCallback updateCallback;
    /* 0x0DC */ CARDCallback unlockCallback;
    /* 0x0E0 */ OSAlarm alarm;
    /* 0x108 */ u8 _108[0x04];
    /* 0x10C */ void* diskId;
} CARDControl;

typedef struct CARDDirEntry {
    /* 0x00 */ u8 gameName[4];
    /* 0x04 */ u8 company[2];
    /* 0x06 */ u8 _06;
    /* 0x07 */ u8 bannerFormat;
    /* 0x08 */ char fileName[32];
    /* 0x28 */ u32 time;
    /* 0x2C */ u32 iconAddr;
    /* 0x30 */ u16 iconFormat;
    /* 0x32 */ u16 animationSpeed;
    /* 0x34 */ u8 permission;
    /* 0x35 */ u8 copyTimes;
    /* 0x36 */ u16 startBlock;
    /* 0x38 */ u16 length;
    /* 0x3A */ u8 _3A[2];
    /* 0x3C */ u32 commentAddr;
} CARDDirEntry;

typedef struct CARDStat {
    /* 0x00 */ char fileName[32];
    /* 0x20 */ u32 length;
    /* 0x24 */ u32 time;
    /* 0x28 */ u8 gameName[4];
    /* 0x2C */ u8 company[2];
    /* 0x2E */ u8 bannerFormat;
    /* 0x2F */ u8 _2F;
    /* 0x30 */ u32 iconAddr;
    /* 0x34 */ u16 iconFormat;
    /* 0x36 */ u16 iconSpeed;
    /* 0x38 */ u32 commentAddr;
    /* 0x3C */ u32 offsetBanner;
    /* 0x40 */ u32 offsetBannerTlut;
    /* 0x44 */ u32 offsetIcon[8];
    /* 0x64 */ u32 offsetIconTlut;
    /* 0x68 */ u32 offsetData;
} CARDStat;

typedef struct CARDMountControl {
    u8 _00[8];
    u16 size;
    u8 _0A[2];
    s32 sectorSize;
    u16 cBlock;
    u8 _12[2];
    u32 latency;
    u8 id[12];
    s32 mountStep;
    u8 _28[0x58];
    void* workArea;
    u8 _84[0x84];
    u32 cid;
    u8 _10C[4];
} CARDMountControl;

typedef struct CARDID {
    u8 serial[32];
    u16 deviceID;
    u16 size;
    u16 encode;
    u8 padding[470];
    u16 checkSum;
    u16 checkSumInv;
} CARDID;

typedef struct CARDDirCheck {
    u8 padding[0x3A];
    s16 checkCode;
    u16 checkSum;
    u16 checkSumInv;
} CARDDirCheck;

typedef struct OSSram {
    u16 checkSum;
    u16 checkSumInv;
    u32 ead0;
    u32 ead1;
    u32 counterBias;
    s8 displayOffsetH;
    u8 ntd;
    u8 language;
    u8 flags;
} OSSram;

typedef struct OSSramEx {
    u8 flashID[2][12];
    u8 rest[20];
} OSSramEx;

typedef struct CARDFileInfo {
    /* 0x00 */ s32 chan;
    /* 0x04 */ s32 fileNo;
    /* 0x08 */ u32 offset;
    /* 0x0C */ u32 length;
    /* 0x10 */ u16 startBlock;
} CARDFileInfo;

typedef struct GXTlutRegion {
    /* 0x00 */ u8 _00[0x10];
} GXTlutRegion;

typedef struct GXTexRegion {
    u32 unk[4];
} GXTexRegion;

typedef struct GXRenderModeObj {
    u32 viTVmode;
    u16 fbWidth;
    u16 efbHeight;
    u16 xfbHeight;
    u16 viXOrigin;
    u16 viYOrigin;
    u16 viWidth;
    u16 viHeight;
    u32 xfbMode;
    u8 field_rendering;
    u8 aa;
    u8 sample_pattern[12][2];
    u8 vfilter[7];
} GXRenderModeObj;

typedef struct GXFifoObj {
    /* 0x00 */ u8* base;
    /* 0x04 */ u8* top;
    /* 0x08 */ u32 size;
    /* 0x0C */ u32 hiWatermark;
    /* 0x10 */ u32 loWatermark;
    /* 0x14 */ void* rdPtr;
    /* 0x18 */ void* wrPtr;
    /* 0x1C */ s32 count;
    /* 0x20 */ u8 wrap;
    /* 0x21 */ u8 _21[3];
} GXFifoObj;

typedef struct GXData {
    /* 0x000 */ u8 _000[0x08];
    /* 0x008 */ u32 cpEnable;
    /* 0x00C */ u32 cpStatus;
    /* 0x010 */ u8 _010[0x2B8];
    /* 0x2C8 */ u32 nextTexRgn;
    /* 0x2CC */ u32 nextTexRgnCI;
    /* 0x2D0 */ GXTlutRegion defaultTlutRegions[20];
} GXData;

#define DSP_REGS    ((volatile DSPRegisters*)0xCC005000)
#define AI_REGS     ((volatile AIRegisters*)0xCC006C00)
#define GET_REG_FIELD(reg, size, shift) \
    ((int)((reg) >> (shift)) & ((1 << (size)) - 1))

#define AT_ADDRESS(addr) : addr

volatile u16 __DSPRegs[32] AT_ADDRESS(0xCC005000);
volatile u32 __AIRegs[4] AT_ADDRESS(0xCC006C00);

extern const char* lbl_80478A30;
extern const char* lbl_80478A28;
extern const char* lbl_80478A38;
extern const char* lbl_80478A40;
extern const char* lbl_80478A48;
extern volatile u32 lbl_80478A50;
extern AISCallback lbl_8047A8C8;
extern AIDCallback lbl_8047A8CC;
extern void* lbl_8047A8D0;
extern void* lbl_8047A8D4;
extern BOOL lbl_8047A8D8;
extern BOOL lbl_8047A8DC;
extern s64 lbl_8047A8E0;
extern s64 lbl_8047A8E8;
extern s64 lbl_8047A8F0;
extern s64 lbl_8047A8F8;
extern s64 lbl_8047A900;
extern ARCallback lbl_8047A908;
extern u32 lbl_8047A90C;
extern u32 lbl_8047A910;
extern u32 lbl_8047A914;
extern u32 lbl_8047A918;
extern u32 lbl_8047A91C;
extern u32* lbl_8047A920;
extern s32 lbl_8047A924;
typedef struct ARQRequest ARQRequest;
typedef void (*ARQCallback)(ARQRequest* request);
struct ARQRequest {
    /* 0x00 */ ARQRequest* next;
    /* 0x04 */ u32 owner;
    /* 0x08 */ u32 type;
    /* 0x0C */ u32 priority;
    /* 0x10 */ u32 source;
    /* 0x14 */ u32 dest;
    /* 0x18 */ u32 length;
    /* 0x1C */ ARQCallback callback;
};
extern ARQRequest* lbl_8047A928;
extern ARQRequest* lbl_8047A92C;
extern ARQRequest* lbl_8047A930;
extern ARQRequest* lbl_8047A934;
extern ARQRequest* lbl_8047A938;
extern ARQRequest* lbl_8047A93C;
extern ARQCallback lbl_8047A940;
extern ARQCallback lbl_8047A944;
extern u32 lbl_8047A948;
extern s32 lbl_8047A94C;
extern s32 lbl_8047A950;
extern u32 lbl_8047A960;
extern u16 lbl_8047A970;

typedef struct DSPTaskInfo DSPTaskInfo;
typedef void (*DSPCallback)(void* task);
struct DSPTaskInfo {
    /* 0x00 */ u32 state;
    /* 0x04 */ u32 priority;
    /* 0x08 */ u32 flags;
    /* 0x0C */ u16* iram_mmem_addr;
    /* 0x10 */ u32 iram_length;
    /* 0x14 */ u32 iram_addr;
    /* 0x18 */ u16* dram_mmem_addr;
    /* 0x1C */ u32 dram_length;
    /* 0x20 */ u32 dram_addr;
    /* 0x24 */ u16 dsp_init_vector;
    /* 0x26 */ u16 dsp_resume_vector;
    /* 0x28 */ DSPCallback init_cb;
    /* 0x2C */ DSPCallback res_cb;
    /* 0x30 */ DSPCallback done_cb;
    /* 0x34 */ DSPCallback req_cb;
    /* 0x38 */ DSPTaskInfo* next;
    /* 0x3C */ DSPTaskInfo* prev;
    /* 0x40 */ u8 _40[0x10];
};
typedef struct CARDDecParam {
    u8* inputAddr;
    u32 inputLength;
    u32 aramAddr;
    u8* outputAddr;
} CARDDecParam;
extern DSPTaskInfo* lbl_8047A964; /* __DSP_last_task */
extern DSPTaskInfo* lbl_8047A968; /* __DSP_first_task */
extern DSPTaskInfo* lbl_8047A96C; /* __DSP_curr_task */
extern s32 lbl_8047A958;          /* __DSP_rude_task_pending */
extern DSPTaskInfo* lbl_8047A95C; /* __DSP_rude_task */
extern CARDControl lbl_803FC620[2];
extern u8 lbl_803FC840[];
extern u8 lbl_80312800[];
extern u16* __cpReg;
extern OSThread* lbl_8047A9A8;
extern BOOL lbl_8047A9B0;
extern void (*lbl_8047A9B4)(void);
extern u32 lbl_8047A9B8;
extern u8 lbl_803125E8[];
extern u8 lbl_803127F0[];
extern u16 lbl_80478A58;
extern s32 lbl_80312960[];

extern s32 CARDCheckExAsync(s32 chan, s32* xferBytes, CARDCallback callback);
extern s32 CARDUnmount(s32 chan);
extern s32 __CARDFormatRegionAsync(s32 chan, u16 encode, CARDCallback callback);
extern s32 fn_800B57D0(s32 chan, s32 fileNo, CARDDirEntry* entry);
extern s32 fn_800B588C(s32 chan, s32 fileNo, CARDDirEntry* entry, void* callback);
extern void __ARQInterruptServiceRoutine(void);
extern void __ARHandler(__OSInterrupt interrupt, OSContext* context);
extern void __ARChecksize(void);
extern void __DSPHandler(__OSInterrupt interrupt, OSContext* context);
extern void __DSP_boot_task(DSPTaskInfo* task);
extern void __DSP_insert_task(DSPTaskInfo* task);
extern void __DSP_debug_printf(char* fmt, ...);
extern void __DSP_exec_task(DSPTaskInfo* current, DSPTaskInfo* next);
extern void __DSP_remove_task(DSPTaskInfo* task);
extern void fn_8009870C();
extern BOOL fn_80098944(s32 chan);
extern void fn_80098AE8(s32 chan);
s32 fn_800AF8A0(s32 chan);
void __CARDUnlockedHandler(s32 chan, OSContext* context);
extern void OSRegisterVersion(const char* version);
u16 __CARDGetFontEncode(void);
s32 __CARDPutControlBlock(CARDControl* card, s32 result);
s32 fn_800AFBDC(s32 chan, void* buf, void* callback);
s32 fn_800AFFE0(s32 chan, u32 addr, void* callback);
extern s32 __CARDGetControlBlock(s32 chan, CARDControl** pcard);
extern void* __OSLockSram(void);
extern void __OSUnlockSram(BOOL commit);
extern void* __OSLockSramEx(void);
extern void __OSUnlockSramEx(BOOL commit);
extern void __CARDCheckSum(void* ptr, u32 length, u16* checksum,
                           u16* checksumInv);
extern void FormatCallback(s32 chan, s32 result);
extern void DCStoreRange(void* addr, u32 length);
extern s64 OSGetTime(void);
extern s32 VerifyID(CARDControl* card);
extern s32 VerifyDir(CARDControl* card, s32* current);
extern s32 VerifyFAT(CARDControl* card, s32* current);
extern s32 __CARDUpdateDir(s32 chan, CARDCallback callback);
extern s32 __CARDUpdateFatBlock(s32 chan, u16* fat, CARDCallback callback);
extern u32 DummyLen(void);
extern s32 ReadArrayUnlock(s32 chan, u32 data, void* rbuf, s32 rlen,
                           s32 mode);

extern void DCFlushRange(void* addr, u32 length);
extern void DCInvalidateRange(void* addr, u32 length);
extern DSPTaskInfo* DSPAddTask(DSPTaskInfo* task);
extern void InitCallback(void* task);
extern void DoneCallback(void* task);
extern void __AIDHandler(__OSInterrupt interrupt, OSContext* context);
extern void __AISHandler(__OSInterrupt interrupt, OSContext* context);

#define OS_BUS_CLOCK_800AC02C   (*(u32*)0x800000F8)
#define OS_TIMER_CLOCK_800AC02C (OS_BUS_CLOCK_800AC02C / 4)
#define OSNanosecondsToTicks_800AC02C(nsec) \
    (((nsec) * (OS_TIMER_CLOCK_800AC02C / 125000)) / 8000)

#if defined(SDK_AC02C_EXACT_AI_800AC02C_800AC440) || \
    defined(SDK_AC02C_EXACT_AI_800AC5AC_800AC6D4) || \
    defined(SDK_AC02C_EXACT_AR_800AC910_800AC954) || \
    defined(SDK_AC02C_EXACT_AR_800AC954_800ACB44) || \
    defined(SDK_AC02C_EXACT_AR_800ACB44_800ACBFC)
#define SDK_AC02C_EXACT_ACTIVE
#endif

#if !defined(SDK_AC02C_EXACT_ACTIVE) || \
    defined(SDK_AC02C_EXACT_AI_800AC02C_800AC440)
AIDCallback AIRegisterDMACallback(AIDCallback callback) {
    AIDCallback old = lbl_8047A8CC;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    lbl_8047A8CC = callback;
    OSRestoreInterrupts(enabled);
    return old;
}

void AIInitDMA(u32 addr, u32 length) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    __DSPRegs[24] = (__DSPRegs[24] & ~0x3ff) | (addr >> 16);
    __DSPRegs[25] = (__DSPRegs[25] & ~0xffe0) | (addr & 0xffff);
    __DSPRegs[27] = (__DSPRegs[27] & ~0x7fff) | (u16)(length >> 5);
    OSRestoreInterrupts(enabled);
}

void AIStartDMA(void) {
    __DSPRegs[27] |= 0x8000;
}

void AIStopDMA(void) {
    __DSPRegs[27] &= ~0x8000;
}

u32 AIGetStreamPlayState(void);
u32 AIGetStreamSampleRate(void);
u32 AIGetStreamVolLeft(void);
u32 AIGetStreamVolRight(void);
void AISetStreamVolLeft(u32 volume);
void AISetStreamVolRight(u32 volume);
void __AI_SRC_INIT(void);

void AISetStreamPlayState(u32 state) {
    BOOL enabled;
    u32 volumeRight;
    u32 volumeLeft;

    if (state != AIGetStreamPlayState()) {
        if (AIGetStreamSampleRate() == 0 && state == 1) {
            volumeRight = AIGetStreamVolRight();
            volumeLeft = AIGetStreamVolLeft();
            AISetStreamVolRight(0);
            AISetStreamVolLeft(0);
            enabled = OSDisableInterrupts();
            __AI_SRC_INIT();
            __AIRegs[0] = (__AIRegs[0] & ~0x20) | 0x20;
            __AIRegs[0] = (__AIRegs[0] & ~1) | 1;
            OSRestoreInterrupts(enabled);
            AISetStreamVolLeft(volumeRight);
            AISetStreamVolRight(volumeLeft);
        } else {
            __AIRegs[0] = (__AIRegs[0] & ~1) | state;
        }
    }
}

u32 AIGetStreamPlayState(void) {
    return AI_REGS->control & 1;
}

u32 AIGetDSPSampleRate(void);

void AISetDSPSampleRate(u32 rate) {
    BOOL enabled;
    u32 oldVolL;
    u32 oldVolR;
    u32 oldStreamPlay;
    u32 oldStreamRate;

    if (rate == AIGetDSPSampleRate()) {
        return;
    }
    AI_REGS->control &= ~0x40;
    if (rate != 0) {
        return;
    }

    oldVolL = AIGetStreamVolLeft();
    oldVolR = AIGetStreamVolRight();
    oldStreamPlay = AIGetStreamPlayState();
    oldStreamRate = AIGetStreamSampleRate();

    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    enabled = OSDisableInterrupts();
    __AI_SRC_INIT();
    AI_REGS->control = (AI_REGS->control & ~0x20) | 0x20;
    AI_REGS->control = (AI_REGS->control & ~0x2) | (oldStreamRate << 1);
    AI_REGS->control = (AI_REGS->control & ~0x1) | oldStreamPlay;
    AI_REGS->control |= 0x40;
    OSRestoreInterrupts(enabled);
    AISetStreamVolLeft(oldVolL);
    AISetStreamVolRight(oldVolR);
}

u32 AIGetDSPSampleRate(void) {
    return ((AI_REGS->control >> 6) & 1) ^ 1;
}

void __AI_set_stream_sample_rate(u32 rate) {
    BOOL enabled;
    u32 playState;
    u32 volumeLeft;
    u32 volumeRight;
    u32 dspSampleRate;

    if (rate != AIGetStreamSampleRate()) {
        playState = AIGetStreamPlayState();
        volumeLeft = AIGetStreamVolLeft();
        volumeRight = AIGetStreamVolRight();
        AISetStreamVolRight(0);
        AISetStreamVolLeft(0);
        dspSampleRate = __AIRegs[0] & 0x40;
        __AIRegs[0] &= ~0x40;
        enabled = OSDisableInterrupts();
        __AI_SRC_INIT();
        __AIRegs[0] |= dspSampleRate;
        __AIRegs[0] = (__AIRegs[0] & ~0x20) | 0x20;
        __AIRegs[0] = (__AIRegs[0] & ~2) | (rate << 1);
        OSRestoreInterrupts(enabled);
        AISetStreamPlayState(playState);
        AISetStreamVolLeft(volumeLeft);
        AISetStreamVolRight(volumeRight);
    }
}

u32 AIGetStreamSampleRate(void) {
    return (AI_REGS->control >> 1) & 1;
}

void AISetStreamVolLeft(u32 volume) {
    __AIRegs[1] = (__AIRegs[1] & ~0xff) | (volume & 0xff);
}

u32 AIGetStreamVolLeft(void) {
    return AI_REGS->volume & 0xff;
}

void AISetStreamVolRight(u32 volume) {
    __AIRegs[1] = (__AIRegs[1] & ~0xff00) | ((volume & 0xff) << 8);
}

u32 AIGetStreamVolRight(void) {
    return (AI_REGS->volume >> 8) & 0xff;
}
#endif

#if !defined(SDK_AC02C_EXACT_ACTIVE) || \
    defined(SDK_AC02C_CANDIDATE_AIINIT_ONLY)
void AIInit(u8* stack) {
    if (lbl_8047A8D8 != TRUE) {
        OSRegisterVersion(lbl_80478A28);

        lbl_8047A8E0 = OSNanosecondsToTicks_800AC02C(31524);
        lbl_8047A8E8 = OSNanosecondsToTicks_800AC02C(42024);
        lbl_8047A8F0 = OSNanosecondsToTicks_800AC02C(42000);
        lbl_8047A8F8 = OSNanosecondsToTicks_800AC02C(63000);
        lbl_8047A900 = OSNanosecondsToTicks_800AC02C(3000);
        AISetStreamVolRight(0);
        AISetStreamVolLeft(0);
        __AIRegs[0] = (__AIRegs[0] & ~0x20) | 0x20;
        __AIRegs[1] &= ~0xFF;
        __AIRegs[2] = 0;
        __AIRegs[3] = 0;
        __AI_set_stream_sample_rate(1);
        AISetDSPSampleRate(0);
        lbl_8047A8C8 = NULL;
        lbl_8047A8CC = NULL;
        lbl_8047A8D0 = stack;
        __OSSetInterruptHandler(5, __AIDHandler);
        __OSUnmaskInterrupts(0x04000000);
        __OSSetInterruptHandler(8, __AISHandler);
        __OSUnmaskInterrupts(0x00800000);
        lbl_8047A8D8 = TRUE;
    }
}
#endif

#if !defined(SDK_AC02C_EXACT_ACTIVE) || \
    defined(SDK_AC02C_EXACT_AI_800AC5AC_800AC6D4)
void __AISHandler(__OSInterrupt interrupt, OSContext* context) {
    OSContext exceptionContext;

    __AIRegs[0] |= 8;
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    if (lbl_8047A8C8 != NULL) {
        lbl_8047A8C8(__AIRegs[2]);
    }
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

void __AICallbackStackSwitch(AIDCallback callback);

void __AIDHandler(__OSInterrupt interrupt, OSContext* context) {
    OSContext exceptionContext;
    u16 tmp;

    tmp = __DSPRegs[5];
    __DSPRegs[5] = (tmp & ~0xa0) | 8;
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    if (lbl_8047A8CC != NULL && lbl_8047A8DC == FALSE) {
        lbl_8047A8DC = TRUE;
        if (lbl_8047A8D0 != NULL) {
            __AICallbackStackSwitch(lbl_8047A8CC);
        } else {
            lbl_8047A8CC();
        }
        lbl_8047A8DC = FALSE;
    }
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}
#endif

#if !defined(SDK_AC02C_EXACT_ACTIVE)
void __AICallbackStackSwitch(AIDCallback callback) {
    *(void**)0x8047A8D4 = (void*)OSGetStackPointer();
    OSSwitchFiber((u32)callback, *(u32*)0x8047A8D0);
}

void __AI_SRC_INIT(void) {
    s64 rising32 = 0;
    s64 rising48 = 0;
    s64 diff = 0;
    s64 wait = 0;
    u32 sample;
    u32 done = 0;

    while (!done) {
        __AIRegs[0] = (__AIRegs[0] & ~0x20) | 0x20;
        __AIRegs[0] &= ~2U;
        __AIRegs[0] = (__AIRegs[0] & ~1U) | 1;
        sample = __AIRegs[2];
        while (sample == __AIRegs[2]) {
        }
        rising32 = OSGetTime();

        __AIRegs[0] = (__AIRegs[0] & ~2U) | 2;
        __AIRegs[0] = (__AIRegs[0] & ~1U) | 1;
        sample = __AIRegs[2];
        while (sample == __AIRegs[2]) {
        }
        rising48 = OSGetTime();
        diff = rising48 - rising32;

        __AIRegs[0] &= ~2U;
        __AIRegs[0] &= ~1U;
        if (diff < lbl_8047A8E0 - lbl_8047A900) {
            wait = lbl_8047A8F0;
            done = 1;
        } else if (diff >= lbl_8047A8E0 + lbl_8047A900 &&
                   diff < lbl_8047A8E8 - lbl_8047A900) {
            wait = lbl_8047A8F8;
            done = 1;
        }
    }
    while (rising48 + wait > OSGetTime()) {
    }
}
#endif

#if !defined(SDK_AC02C_EXACT_ACTIVE) || \
    defined(SDK_AC02C_EXACT_AR_800AC910_800AC954)
ARCallback ARRegisterDMACallback(ARCallback callback) {
    ARCallback old = lbl_8047A908;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    lbl_8047A908 = callback;
    OSRestoreInterrupts(enabled);
    return old;
}
#endif

#if !defined(SDK_AC02C_EXACT_ACTIVE) || \
    defined(SDK_AC02C_EXACT_AR_800ACB44_800ACBFC)
u32 ARGetBaseAddress(void) {
    return 0x4000;
}

u32 ARGetSize(void) {
    return lbl_8047A90C;
}

void __ARHandler(__OSInterrupt interrupt, OSContext* context) {
    OSContext exceptionContext;
    u16 tmp;

    tmp = __DSPRegs[5];
    __DSPRegs[5] = (tmp & ~0x88) | 0x20;
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    if (lbl_8047A908 != NULL) {
        lbl_8047A908();
    }
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

void __ARClearInterrupt(void) {
    u16 tmp = __DSPRegs[5];

    __DSPRegs[5] = (tmp & ~0x88) | 0x20;
}

u32 __ARGetInterruptStatus(void) {
    return DSP_REGS->dmaControl & 0x20;
}
#endif

#if !defined(SDK_AC02C_EXACT_ACTIVE)
static void __ARWaitForDMA(void) {
    while (__DSPRegs[5] & 0x200) {
    }
}

static void __ARWriteDMA(u32 mainMemoryAddress, u32 aramAddress, u32 length) {
    __DSPRegs[16] =
        (__DSPRegs[16] & ~0x3ff) | (u16)(mainMemoryAddress >> 16);
    __DSPRegs[17] =
        (__DSPRegs[17] & ~0xffe0) | (u16)(mainMemoryAddress & 0xffff);
    __DSPRegs[18] = (__DSPRegs[18] & ~0x3ff) | (u16)(aramAddress >> 16);
    __DSPRegs[19] = (__DSPRegs[19] & ~0xffe0) | (u16)(aramAddress & 0xffff);
    __DSPRegs[20] = __DSPRegs[20] & ~0x8000;
    __DSPRegs[20] = (__DSPRegs[20] & ~0x3ff) | (u16)(length >> 16);
    __DSPRegs[21] = (__DSPRegs[21] & ~0xffe0) | (u16)(length & 0xffff);
    __ARWaitForDMA();
    __ARClearInterrupt();
}

static void __ARReadDMA(u32 mainMemoryAddress, u32 aramAddress, u32 length) {
    __DSPRegs[16] =
        (__DSPRegs[16] & ~0x3ff) | (u16)(mainMemoryAddress >> 16);
    __DSPRegs[17] =
        (__DSPRegs[17] & ~0xffe0) | (u16)(mainMemoryAddress & 0xffff);
    __DSPRegs[18] = (__DSPRegs[18] & ~0x3ff) | (u16)(aramAddress >> 16);
    __DSPRegs[19] = (__DSPRegs[19] & ~0xffe0) | (u16)(aramAddress & 0xffff);
    __DSPRegs[20] = __DSPRegs[20] | 0x8000;
    __DSPRegs[20] = (__DSPRegs[20] & ~0x3ff) | (u16)(length >> 16);
    __DSPRegs[21] = (__DSPRegs[21] & ~0xffe0) | (u16)(length & 0xffff);
    __ARWaitForDMA();
    __ARClearInterrupt();
}
#endif

#if !defined(SDK_AC02C_EXACT_ACTIVE) || \
    defined(SDK_AC02C_EXACT_AR_800AC954_800ACB44)
u32 ARGetDMAStatus(void) {
    BOOL enabled;
    u32 status;

    enabled = OSDisableInterrupts();
    status = DSP_REGS->dmaControl & 0x200;
    OSRestoreInterrupts(enabled);
    return status;
}

#pragma dont_inline on
void ARStartDMA(u32 type, u32 mainMemoryAddress, u32 aramAddress, u32 length) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    __DSPRegs[16] = (__DSPRegs[16] & ~0x3ff) | (mainMemoryAddress >> 16);
    __DSPRegs[17] = (__DSPRegs[17] & ~0xffe0) | (u16)mainMemoryAddress;
    __DSPRegs[18] = (__DSPRegs[18] & ~0x3ff) | (aramAddress >> 16);
    __DSPRegs[19] = (__DSPRegs[19] & ~0xffe0) | (u16)aramAddress;
    __DSPRegs[20] = (__DSPRegs[20] & ~0x8000) | (type << 15);
    __DSPRegs[20] = (__DSPRegs[20] & ~0x3ff) | (length >> 16);
    __DSPRegs[21] = (__DSPRegs[21] & ~0xffe0) | (u16)length;
    OSRestoreInterrupts(enabled);
}
#pragma dont_inline off

u32 ARInit(u32* stack, u32 stackSize) {
    BOOL enabled;

    if (lbl_8047A924 == 1) {
        return 0x4000;
    }
    OSRegisterVersion(lbl_80478A30);
    enabled = OSDisableInterrupts();
    lbl_8047A908 = NULL;
    __OSSetInterruptHandler(6, __ARHandler);
    __OSUnmaskInterrupts(0x02000000);
    lbl_8047A91C = stackSize;
    lbl_8047A918 = 0x4000;
    lbl_8047A920 = stack;
    __DSPRegs[13] = (__DSPRegs[13] & 0xff) | (__DSPRegs[13] & ~0xff);
    __ARChecksize();
    lbl_8047A924 = 1;
    OSRestoreInterrupts(enabled);
    return lbl_8047A918;
}
#endif

#pragma dont_inline on

#pragma dont_inline off

#pragma optimize_for_size on

#pragma optimize_for_size reset

#pragma dont_inline on

#pragma dont_inline off

#pragma dont_inline on

#pragma dont_inline off

/*
 * This range combines multiple CARD objects. Keep the file-lookup helpers
 * before the directory accessor body so lookup retains the retail out-of-line
 * call while later directory-update functions can inline the accessor.
 */

void* __CARDGetDirBlock(CARDControl* card);

#pragma dont_inline on

#pragma dont_inline off

#pragma push
#pragma inline_depth(0)

#pragma pop

#pragma dont_inline on
void UpdateIconOffsets(CARDDirEntry* entry, CARDStat* stat);

#pragma dont_inline off

#pragma peephole off

#pragma peephole reset

#pragma peephole off

#pragma peephole reset

extern GXData* const gx;

void fn_800B7594(u8 overflow, u8 underflow);
void fn_800B75D0(u8 clearOverflow, u8 clearUnderflow);

void GXInitFifoPtrs(GXFifoObj* fifo, void* readPtr, void* writePtr);

#undef SDK_AC02C_EXACT_ACTIVE
