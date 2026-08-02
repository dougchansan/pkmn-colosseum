/**
 * @file sdk_range_800AE3F0.c
 * @brief Dolphin SDK suffix, 0x800AE3F0 - 0x800B71F0 (117 functions).
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
    /* 0x00C */ s32 sectorSize;
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
    u32 wirelessKeyboardID;
    u16 wirelessPadID[4];
    u8 dvdErrorCode;
    u8 _padding0;
    u8 flashIDCheckSum[2];
    u16 gbs;
    u8 _padding1[2];
} OSSramEx;

typedef struct CARDFileInfo {
    /* 0x00 */ s32 chan;
    /* 0x04 */ s32 fileNo;
    /* 0x08 */ s32 offset;
    /* 0x0C */ s32 length;
    /* 0x10 */ u16 startBlock;
} CARDFileInfo;

typedef struct GXTlutRegion {
    /* 0x00 */ u8 _00[0x10];
} GXTlutRegion;

typedef struct GXTexRegion {
    u32 unk[4];
} GXTexRegion;

typedef struct GXColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor;

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
volatile u16 __DSPRegs[32] : 0xCC005000;
#define AI_REGS     ((volatile AIRegisters*)0xCC006C00)
#define GET_REG_FIELD(reg, size, shift) \
    ((int)((reg) >> (shift)) & ((1 << (size)) - 1))

#define AT_ADDRESS(addr) : addr

volatile u16 __DSPRegs[32] AT_ADDRESS(0xCC005000);
volatile u32 __AIRegs[4] AT_ADDRESS(0xCC006C00);

extern const char* lbl_80478A30;
extern const char* lbl_80478A38;
extern const char* lbl_80478A40;
extern const char* lbl_80478A48;
extern volatile u32 lbl_80478A50;
extern AISCallback lbl_8047A8C8;
extern AIDCallback lbl_8047A8CC;
extern void* lbl_8047A8D0;
extern void* lbl_8047A8D4;
extern BOOL lbl_8047A8DC;
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
extern s32 fn_800B588C(s32 chan, s32 fileNo, CARDDirEntry* entry,
                       CARDCallback callback);
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
extern void __CARDCheckSum(void* ptr, s32 length, u16* checksum,
                           u16* checksumInv);
extern void FormatCallback(s32 chan, s32 result);
extern void DCStoreRange(void* addr, u32 length);
extern s64 OSGetTime(void);
extern s32 VerifyID(CARDControl* card);
extern s32 VerifyDir(CARDControl* card, s32* current);
extern s32 VerifyFAT(CARDControl* card, s32* current);
extern s32 __CARDUpdateDir(s32 chan, CARDCallback callback);
extern s32 __CARDUpdateFatBlock(s32 chan, u16* fat, CARDCallback callback);
extern void __CARDDefaultApiCallback(s32 chan, s32 result);
extern BOOL __CARDCompareFileName(CARDDirEntry* entry, const char* fileName);
extern s32 fn_800B4270(CARDControl* card, CARDDirEntry* entry);
extern s32 fn_800B4308(CARDDirEntry* entry);
extern s32 __CARDGetFileNo(CARDControl* card, const char* fileName, s32* pfileNo);
extern void* __CARDGetDirBlock(CARDControl* card);
extern u32 DummyLen(void);
extern s32 ReadArrayUnlock(s32 chan, u32 data, void* rbuf, s32 rlen,
                           s32 mode);
extern u32 strlen(const char* str);
extern char* strncpy(char* dst, const char* src, u32 length);
extern void* __CARDGetFatBlock(CARDControl* card);
extern s32 __CARDAllocBlock(s32 chan, u32 blocks, CARDCallback callback);
extern s32 __CARDRead(s32 chan, u32 addr, u32 length, void* buffer,
                      CARDCallback callback);
extern void EraseCallback(s32 chan, s32 result);

#if defined(SDK_CANDIDATE_800B5228)
void UpdateIconOffsets(CARDDirEntry* entry, CARDStat* stat)
{
    u32 offset;
    BOOL iconTlut;
    s32 i;

    offset = entry->iconAddr;
    if (offset == 0xFFFFFFFF) {
        stat->bannerFormat = 0;
        stat->iconFormat = 0;
        stat->iconSpeed = 0;
        offset = 0;
    }

    iconTlut = FALSE;
    switch (entry->bannerFormat & 3) {
    case 1:
        stat->offsetBanner = offset;
        offset += 96 * 32;
        stat->offsetBannerTlut = offset;
        offset += 2 * 256;
        break;
    case 2:
        stat->offsetBanner = offset;
        offset += 2 * 96 * 32;
        stat->offsetBannerTlut = 0xFFFFFFFF;
        break;
    default:
        stat->offsetBanner = 0xFFFFFFFF;
        stat->offsetBannerTlut = 0xFFFFFFFF;
        break;
    }

    for (i = 0; i < 8; i++) {
        switch ((entry->iconFormat >> (2 * i)) & 3) {
        case 1:
            stat->offsetIcon[i] = offset;
            offset += 32 * 32;
            iconTlut = TRUE;
            break;
        case 2:
            stat->offsetIcon[i] = offset;
            offset += 2 * 32 * 32;
            break;
        default:
            stat->offsetIcon[i] = 0xFFFFFFFF;
            break;
        }
    }

    if (iconTlut) {
        stat->offsetIconTlut = offset;
        offset += 2 * 256;
    } else {
        stat->offsetIconTlut = 0xFFFFFFFF;
    }
    stat->offsetData = offset;
}

s32 fn_800B5530(s32 chan, s32 fileNo, CARDStat* stat)
{
    CARDControl* card;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    s32 result;

    if (fileNo < 0 || fileNo >= 127) {
        return -128;
    }

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileNo];
    result = fn_800B4270(card, entry);
    if (result == -10) {
        result = fn_800B4308(entry);
    }

    if (result >= 0) {
        memcpy(stat->gameName, entry->gameName, 4);
        memcpy(stat->company, entry->company, 2);
        stat->length = (u32)entry->length * card->sectorSize;
        memcpy(stat->fileName, entry->fileName, 32);
        stat->time = entry->time;
        stat->bannerFormat = entry->bannerFormat;
        stat->iconAddr = entry->iconAddr;
        stat->iconFormat = entry->iconFormat;
        stat->iconSpeed = entry->animationSpeed;
        stat->commentAddr = entry->commentAddr;
        UpdateIconOffsets(entry, stat);
    }

    return __CARDPutControlBlock(card, result);
}

s32 fn_800B588C(s32 chan, s32 fileNo, CARDDirEntry* input,
                CARDCallback callback)
{
    CARDControl* card;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    s32 result;
    u8* p;
    s32 i;

    if (fileNo < 0 || fileNo >= 127 ||
        (u8)input->fileName[0] == 0xFF || (u8)input->fileName[0] == 0) {
        return -128;
    }

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileNo];
    result = fn_800B4270(card, entry);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }

    for (p = (u8*)input->fileName; p < (u8*)&input->time; p++) {
        if (*p != 0) {
            continue;
        }
        while (++p < (u8*)&input->time) {
            *p = 0;
        }
        break;
    }

    if (memcmp(entry->fileName, input->fileName, 32) != 0 ||
        memcmp(entry->gameName, input->gameName, 4) != 0 ||
        memcmp(entry->company, input->company, 2) != 0) {
        for (i = 0; i < 127; i++) {
            if (i != fileNo) {
                CARDDirEntry* entry = &dir[i];

                if ((u8)entry->gameName[0] != 0xFF &&
                    memcmp(entry->gameName, input->gameName, 4) == 0 &&
                    memcmp(entry->company, input->company, 2) == 0 &&
                    memcmp(entry->fileName, input->fileName, 32) == 0) {
                    return __CARDPutControlBlock(card, -7);
                }
            }
        }
        memcpy(entry->fileName, input->fileName, 32);
        memcpy(entry->gameName, input->gameName, 4);
        memcpy(entry->company, input->company, 2);
    }

    entry->time = input->time;
    entry->bannerFormat = input->bannerFormat;
    entry->iconAddr = input->iconAddr;
    entry->iconFormat = input->iconFormat;
    entry->animationSpeed = input->animationSpeed;
    entry->commentAddr = input->commentAddr;
    entry->permission = input->permission;
    entry->copyTimes = input->copyTimes;

    result = __CARDUpdateDir(chan, callback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;
}
#endif

#if defined(SDK_EXACT_800B2070_800B2968)
void __CARDCheckSum(void* ptr, s32 length, u16* checksum, u16* checksumInv)
{
    u16* p;
    s32 i;

    length /= sizeof(u16);
    *checksum = *checksumInv = 0;
    for (i = 0, p = ptr; i < length; i++, p++) {
        *checksum += *p;
        *checksumInv += ~*p;
    }

    if (*checksum == 0xFFFF) {
        *checksum = 0;
    }
    if (*checksumInv == 0xFFFF) {
        *checksumInv = 0;
    }
}

s32 VerifyID(CARDControl* card)
{
    CARDID* id;
    u16 checksum;
    u16 checksumInv;
    OSSramEx* sramEx;
    s64 rand;
    s32 i;

    id = card->workArea;

    if (id->deviceID != 0 || id->size != card->size) {
        return -6;
    }

    __CARDCheckSum(id, sizeof(CARDID) - sizeof(u32), &checksum, &checksumInv);
    if (id->checkSum != checksum || id->checkSumInv != checksumInv) {
        return -6;
    }

    rand = *(s64*)&id->serial[12];
    sramEx = __OSLockSramEx();
    for (i = 0; i < 12; i++) {
        rand = (rand * 1103515245 + 12345) >> 16;
        if (id->serial[i] !=
            (u8)(sramEx->flashID[card - lbl_803FC620][i] + rand)) {
            __OSUnlockSramEx(FALSE);
            return -6;
        }
        rand = ((rand * 1103515245 + 12345) >> 16) & 0x7FFF;
    }

    __OSUnlockSramEx(FALSE);

    if (id->encode != __CARDGetFontEncode()) {
        return -13;
    }
    return 0;
}

s32 VerifyDir(CARDControl* card, s32* currentOut)
{
    CARDDirEntry* dir[2];
    CARDDirCheck* check[2];
    u16 checksum;
    u16 checksumInv;
    s32 i;
    s32 errors;
    s32 current;

    current = errors = 0;
    for (i = 0; i < 2; i++) {
        dir[i] =
            (CARDDirEntry*)((u8*)card->workArea + (1 + i) * 0x2000);
        check[i] = (CARDDirCheck*)((u8*)dir[i] + 0x1FC0);
        __CARDCheckSum(dir[i], 0x1FFC, &checksum, &checksumInv);
        if (check[i]->checkSum != checksum ||
            check[i]->checkSumInv != checksumInv) {
            ++errors;
            current = i;
            card->dirBlock = NULL;
        }
    }

    if (errors == 0) {
        if (card->dirBlock == NULL) {
            if (check[0]->checkCode - check[1]->checkCode < 0) {
                current = 0;
            } else {
                current = 1;
            }
            card->dirBlock = dir[current];
            memcpy(dir[current], dir[current ^ 1], 0x2000);
        } else {
            current = card->dirBlock == dir[0] ? 0 : 1;
        }
    }

    if (currentOut != NULL) {
        *currentOut = current;
    }
    return errors;
}

s32 VerifyFAT(CARDControl* card, s32* currentOut)
{
    u16* fat[2];
    u16* fatp;
    u16 block;
    u16 freeBlocks;
    s32 i;
    u16 checksum;
    u16 checksumInv;
    s32 errors;
    s32 current;

    current = errors = 0;
    for (i = 0; i < 2; i++) {
        fatp = fat[i] =
            (u16*)((u8*)card->workArea + (3 + i) * 0x2000);
        __CARDCheckSum(&fatp[2], 0x1FFC, &checksum, &checksumInv);
        if (fatp[0] != checksum || fatp[1] != checksumInv) {
            ++errors;
            current = i;
            card->fatBlock = NULL;
            continue;
        }

        freeBlocks = 0;
        for (block = 5; block < card->cBlock; block++) {
            if (fatp[block] == 0) {
                freeBlocks++;
            }
        }
        if (freeBlocks != fatp[3]) {
            ++errors;
            current = i;
            card->fatBlock = NULL;
            continue;
        }
    }

    if (errors == 0) {
        if (card->fatBlock == NULL) {
            if ((s16)fat[0][2] - (s16)fat[1][2] < 0) {
                current = 0;
            } else {
                current = 1;
            }
            card->fatBlock = fat[current];
            memcpy(fat[current], fat[current ^ 1], 0x2000);
        } else {
            current = card->fatBlock == fat[0] ? 0 : 1;
        }
    }

    if (currentOut != NULL) {
        *currentOut = current;
    }
    return errors;
}
#endif

#if defined(SDK_EXACT_800B4644_800B4DC4)
#define CARD_IS_VALID_BLOCK(card, block) \
    ((u16)(block) >= 5 && (u16)(block) < (card)->cBlock)
#define CARD_TRUNC(value, align) (((u32)(value)) & ~((align) - 1))

static void CreateCallbackFat(s32 chan, s32 result)
{
    CARDControl* card;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    CARDCallback callback;

    card = &lbl_803FC620[chan];
    callback = card->apiCallback;
    card->apiCallback = NULL;

    if (result >= 0) {
        dir = __CARDGetDirBlock(card);
        entry = &dir[card->freeNo];
        memcpy(entry->gameName, ((DVDDiskID*)card->diskId)->gameName, 4);
        memcpy(entry->company, ((DVDDiskID*)card->diskId)->company, 2);
        entry->permission = 4;
        entry->copyTimes = 0;
        entry->startBlock = card->startBlock;
        entry->bannerFormat = 0;
        entry->iconAddr = -1;
        entry->iconFormat = 0;
        entry->animationSpeed = 0;
        entry->commentAddr = -1;
        entry->animationSpeed = (entry->animationSpeed & ~3) | 1;

        card->fileInfo->offset = 0;
        card->fileInfo->startBlock = entry->startBlock;
        entry->time = (u32)(OSGetTime() / (*(u32*)0x800000F8 / 4));
        result = __CARDUpdateDir(chan, callback);
        if (result >= 0) {
            return;
        }
    }

    __CARDPutControlBlock(card, result);
    if (callback != NULL) {
        callback(chan, result);
    }
}

s32 CARDCreateAsync(s32 chan, const char* fileName, u32 size,
                    CARDFileInfo* fileInfo, CARDCallback callback)
{
    CARDControl* card;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    u16 fileNo;
    u16 freeNo;
    u16* fat;
    s32 result;

    if (strlen(fileName) > 32) {
        return -12;
    }

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    if (size == 0 || size % card->sectorSize != 0) {
        return -128;
    }

    freeNo = (u16)-1;
    dir = __CARDGetDirBlock(card);
    for (fileNo = 0; fileNo < 127; fileNo++) {
        entry = &dir[fileNo];
        if (entry->gameName[0] == 0xFF) {
            if (freeNo == (u16)-1) {
                freeNo = fileNo;
            }
        } else if (
            memcmp(entry->gameName, ((DVDDiskID*)card->diskId)->gameName, 4) ==
                0 &&
            memcmp(entry->company, ((DVDDiskID*)card->diskId)->company, 2) ==
                0 &&
            __CARDCompareFileName(entry, fileName)) {
            return __CARDPutControlBlock(card, -7);
        }
    }

    if (freeNo == (u16)-1) {
        return __CARDPutControlBlock(card, -8);
    }

    fat = __CARDGetFatBlock(card);
    if (card->sectorSize * fat[3] < size) {
        return __CARDPutControlBlock(card, -9);
    }

    card->apiCallback =
        callback != NULL ? callback : __CARDDefaultApiCallback;
    card->freeNo = freeNo;
    entry = &dir[freeNo];
    entry->length = (u16)(size / card->sectorSize);
    strncpy(entry->fileName, fileName, 32);

    card->fileInfo = fileInfo;
    fileInfo->chan = chan;
    fileInfo->fileNo = freeNo;

    result =
        __CARDAllocBlock(chan, size / card->sectorSize, CreateCallbackFat);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }
    return result;
}

s32 __CARDSeek(CARDFileInfo* fileInfo, s32 length, s32 offset,
               CARDControl** cardOut)
{
    CARDControl* card;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    s32 result;
    u16* fat;

    result = __CARDGetControlBlock(fileInfo->chan, &card);
    if (result < 0) {
        return result;
    }

    if (!CARD_IS_VALID_BLOCK(card, fileInfo->startBlock) ||
        card->cBlock * card->sectorSize <= fileInfo->offset) {
        return __CARDPutControlBlock(card, -128);
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileInfo->fileNo];
    if (entry->length * card->sectorSize <= offset ||
        entry->length * card->sectorSize < offset + length) {
        return __CARDPutControlBlock(card, -11);
    }

    card->fileInfo = fileInfo;
    fileInfo->length = length;
    if (offset < fileInfo->offset) {
        fileInfo->offset = 0;
        fileInfo->startBlock = entry->startBlock;
        if (!CARD_IS_VALID_BLOCK(card, fileInfo->startBlock)) {
            return __CARDPutControlBlock(card, -6);
        }
    }

    fat = __CARDGetFatBlock(card);
    while (fileInfo->offset <
           ((u32)offset & ~(card->sectorSize - 1))) {
        fileInfo->offset += card->sectorSize;
        fileInfo->startBlock = fat[fileInfo->startBlock];
        if (!CARD_IS_VALID_BLOCK(card, fileInfo->startBlock)) {
            return __CARDPutControlBlock(card, -6);
        }
    }

    fileInfo->offset = offset;
    *cardOut = card;
    return 0;
}

static void ReadCallback(s32 chan, s32 result)
{
    CARDControl* card;
    CARDCallback callback;
    u16* fat;
    CARDFileInfo* fileInfo;
    s32 length;

    card = &lbl_803FC620[chan];
    if (result < 0) {
        goto error;
    }

    fileInfo = card->fileInfo;
    if (fileInfo->length < 0) {
        result = -14;
        goto error;
    }

    length =
        CARD_TRUNC(fileInfo->offset + card->sectorSize, card->sectorSize) -
        fileInfo->offset;
    fileInfo->length -= length;
    if (fileInfo->length <= 0) {
        goto error;
    }

    fat = __CARDGetFatBlock(card);
    fileInfo->offset += length;
    fileInfo->startBlock = fat[fileInfo->startBlock];
    if (!CARD_IS_VALID_BLOCK(card, fileInfo->startBlock)) {
        result = -6;
        goto error;
    }

    result = __CARDRead(
        chan, card->sectorSize * (u32)fileInfo->startBlock,
        fileInfo->length < card->sectorSize ? fileInfo->length
                                            : card->sectorSize,
        card->buffer, ReadCallback);
    if (result >= 0) {
        return;
    }

error:
    callback = card->apiCallback;
    card->apiCallback = NULL;
    __CARDPutControlBlock(card, result);
    callback(chan, result);
}

s32 fn_800B4C7C(CARDFileInfo* fileInfo, void* buffer, s32 length, s32 offset,
                CARDCallback callback)
{
    CARDControl* card;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    s32 result;

    if ((offset & 0x1FF) != 0 || (length & 0x1FF) != 0) {
        return -128;
    }

    result = __CARDSeek(fileInfo, length, offset, &card);
    if (result < 0) {
        return result;
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileInfo->fileNo];
    result = fn_800B4270(card, entry);
    if (result == -10) {
        result = fn_800B4308(entry);
    }
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }

    DCInvalidateRange(buffer, (u32)length);
    card->apiCallback =
        callback != NULL ? callback : __CARDDefaultApiCallback;

    offset = fileInfo->offset & (card->sectorSize - 1);
    length =
        length < card->sectorSize - offset ? length
                                           : card->sectorSize - offset;
    result = __CARDRead(
        fileInfo->chan,
        card->sectorSize * (u32)fileInfo->startBlock + offset, length,
        buffer, ReadCallback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;
}
#endif

#if defined(SDK_EXACT_800B4E50_800B4FC0)
void WriteCallback(s32 chan, s32 result)
{
    CARDControl* card;
    CARDCallback callback;
    u16* fat;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    CARDFileInfo* fileInfo;

    card = &lbl_803FC620[chan];
    if (result >= 0) {
        fileInfo = card->fileInfo;
        if (fileInfo->length < 0) {
            result = -14;
            goto error;
        }

        fileInfo->length -= card->sectorSize;
        if (fileInfo->length <= 0) {
            dir = __CARDGetDirBlock(card);
            entry = &dir[fileInfo->fileNo];
            entry->time =
                (u32)(OSGetTime() / (*(u32*)0x800000F8 / 4));
            callback = card->apiCallback;
            card->apiCallback = NULL;
            result = __CARDUpdateDir(chan, callback);
        } else {
            fat = __CARDGetFatBlock(card);
            fileInfo->offset += card->sectorSize;
            fileInfo->startBlock = fat[fileInfo->startBlock];
            if (fileInfo->startBlock < 5 ||
                fileInfo->startBlock >= card->cBlock) {
                result = -6;
                goto error;
            }
            result = fn_800AFFE0(
                chan, card->sectorSize * fileInfo->startBlock,
                EraseCallback);
        }

        if (result >= 0) {
            return;
        }
    }

error:
    callback = card->apiCallback;
    card->apiCallback = NULL;
    __CARDPutControlBlock(card, result);
    callback(chan, result);
}
#endif

#if defined(SDK_EXACT_800B5070_800B5184)
s32 CARDWriteAsync(CARDFileInfo* fileInfo, void* buffer, s32 length,
                   s32 offset, CARDCallback callback)
{
    CARDControl* card;
    s32 result;
    CARDDirEntry* dir;
    CARDDirEntry* entry;

    result = __CARDSeek(fileInfo, length, offset, &card);
    if (result < 0) {
        return result;
    }

    if ((offset & (card->sectorSize - 1)) != 0 ||
        (length & (card->sectorSize - 1)) != 0) {
        return __CARDPutControlBlock(card, -128);
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileInfo->fileNo];
    result = fn_800B4270(card, entry);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }

    DCStoreRange(buffer, (u32)length);
    card->apiCallback =
        callback != NULL ? callback : __CARDDefaultApiCallback;
    card->buffer = buffer;

    result = fn_800AFFE0(
        fileInfo->chan, card->sectorSize * fileInfo->startBlock,
        EraseCallback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;
}
#endif

#if defined(SDK_EXACT_800AE3F0_800AE9FC) || \
    defined(SDK_EXACT_800AF474_800AF8A0) || \
    defined(SDK_EXACT_800B1788_800B2070) || \
    defined(SDK_EXACT_800B2070_800B2968) || \
    defined(SDK_EXACT_800B4644_800B4DC4) || \
    defined(SDK_EXACT_800B4E50_800B4FC0) || \
    defined(SDK_EXACT_800B5070_800B5184)
#define SDK_RANGE_EXACT_ACTIVE
#endif

#if !defined(CARD_EXACT_800B3B68_ONLY)
#if !defined(SDK_RANGE_EXACT_ACTIVE)
s32 CARDGetSerialNo(s32 chan, u64* serialNo) {
    CARDControl* card;
    CARDID* id;
    int i;
    u64 code;
    s32 result;

    if (!(0 <= chan && chan < 2)) {
        return -128;
    }
    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    id = (CARDID*)card->workArea;
    for (code = 0, i = 0; i < sizeof(id->serial) / sizeof(u64); ++i) {
        code ^= *(u64*)&id->serial[sizeof(u64) * i];
    }
    *serialNo = code;
    return __CARDPutControlBlock(card, 0);
}

static u32 bitrev(u32 data)
{
    u32 work;
    u32 i;
    u32 k = 0;
    u32 j = 1;

    work = 0;
    for (i = 0; i < 32; i++) {
        if (i > 15) {
            if (i == 31) {
                work |= (((data & (1 << 31)) >> 31) & 1);
            } else {
                work |= ((data & (1 << i)) >> j);
                j += 2;
            }
        } else {
            work |= ((data & (1 << i)) << (31 - i - k));
            k++;
        }
    }

    return work;
}
extern void DCFlushRange(void* addr, u32 length);
extern void DCInvalidateRange(void* addr, u32 length);
extern DSPTaskInfo* DSPAddTask(DSPTaskInfo* task);
extern void InitCallback(void* task);
extern void DoneCallback(void* task);

static inline void cardCheckSumInline(void* ptr, s32 length, u16* checksum,
                                      u16* checksumInv)
{
    u16* p;
    s32 i;

    length /= sizeof(u16);
    *checksum = *checksumInv = 0;
    for (i = 0, p = ptr; i < length; i++, p++) {
        *checksum += *p;
        *checksumInv += ~*p;
    }
    if (*checksum == 0xFFFF) {
        *checksum = 0;
    }
    if (*checksumInv == 0xFFFF) {
        *checksumInv = 0;
    }
}

static u32 cardExnorFirst(u32 data, u32 rshift) {
    u32 feedback;
    u32 work;
    u32 i;

    work = data;
    for (i = 0; i < rshift; i++) {
        feedback = ~(work ^ (work >> 7) ^ (work >> 15) ^ (work >> 23));
        work = (work >> 1) | ((feedback << 30) & 0x40000000);
    }
    return work;
}

static u32 cardExnor(u32 data, u32 lshift) {
    u32 feedback;
    u32 work;
    u32 i;

    work = data;
    for (i = 0; i < lshift; i++) {
        feedback = ~(work ^ (work << 7) ^ (work << 15) ^ (work << 23));
        work = (work << 1) | ((feedback >> 30) & 2);
    }
    return work;
}

static s32 cardRand(void) {
    lbl_80478A50 = lbl_80478A50 * 0x41C64E6D + 0x3039;
    return (lbl_80478A50 >> 16) & 0x7FFF;
}

static void cardSrand(u32 seed) {
    lbl_80478A50 = seed;
}

static u32 cardGetInitValue(void) {
    u32 value;
    u32 tick;

    tick = OSGetTick();
    cardSrand(tick);
    value = 0x7FEC8000;
    value |= cardRand();
    value &= 0xFFFFF000;
    return value;
}

s32 __CARDUnlock(s32 chan, u8 flashID[12]) {
    u32 initValue;
    u32 data;
    s32 dummy;
    s32 readLength;
    u32 shift;
    u8 fsts;
    u32 work;
    u32 feedback;
    u32 answer1 = 0;
    u32 answer2 = 0;
    u32* words;
    u8 readBuffer[64];
    u32 parameter1A = 0;
    u32 parameter1B = 0;
    u32 parameter2A = 0;
    u32 parameter2B = 0;
    CARDControl* card;
    DSPTaskInfo* task;
    CARDDecParam* parameter;
    u8* input;
    u8* output;

    card = &lbl_803FC620[chan];
    task = (DSPTaskInfo*)card->task;
    parameter = (CARDDecParam*)card->workArea;
    input = (u8*)parameter + sizeof(CARDDecParam);
    input = (u8*)(((u32)input + 31) & ~31);
    output = input + 32;

    fsts = 0;
    initValue = cardGetInitValue();

    dummy = DummyLen();
    readLength = dummy;
    if (ReadArrayUnlock(chan, initValue, readBuffer, readLength, 0) < 0) {
        return -3;
    }

    shift = dummy * 8 + 1;
    work = cardExnorFirst(initValue, shift);
    feedback = ~(work ^ (work >> 7) ^ (work >> 15) ^ (work >> 23));
    card->scramble = work | ((feedback << 31) & 0x80000000);
    card->scramble = bitrev(card->scramble);

    dummy = DummyLen();
    readLength = 20 + dummy;
    data = 0;
    if (ReadArrayUnlock(chan, data, readBuffer, readLength, 1) < 0) {
        return -3;
    }

    words = (u32*)readBuffer;
    parameter1A = *words++;
    parameter1B = *words++;
    answer1 = *words++;
    parameter2A = *words++;
    parameter2B = *words++;

    parameter1A ^= card->scramble;
    shift = 32;
    work = cardExnor(card->scramble, shift);
    feedback = ~(work ^ (work << 7) ^ (work << 15) ^ (work << 23));
    card->scramble = work | ((feedback >> 31) & 1);

    parameter1B ^= card->scramble;
    shift = 32;
    work = cardExnor(card->scramble, shift);
    feedback = ~(work ^ (work << 7) ^ (work << 15) ^ (work << 23));
    card->scramble = work | ((feedback >> 31) & 1);

    answer1 ^= card->scramble;
    shift = 32;
    work = cardExnor(card->scramble, shift);
    feedback = ~(work ^ (work << 7) ^ (work << 15) ^ (work << 23));
    card->scramble = work | ((feedback >> 31) & 1);

    parameter2A ^= card->scramble;
    shift = 32;
    work = cardExnor(card->scramble, shift);
    feedback = ~(work ^ (work << 7) ^ (work << 15) ^ (work << 23));
    card->scramble = work | ((feedback >> 31) & 1);

    parameter2B ^= card->scramble;
    shift = dummy * 8;
    work = cardExnor(card->scramble, shift);
    feedback = ~(work ^ (work << 7) ^ (work << 15) ^ (work << 23));
    card->scramble = work | ((feedback >> 31) & 1);

    shift = 32 + 1;
    work = cardExnor(card->scramble, shift);
    feedback = ~(work ^ (work << 7) ^ (work << 15) ^ (work << 23));
    card->scramble = work | ((feedback >> 31) & 1);

    *(u32*)&input[0] = parameter2A;
    *(u32*)&input[4] = parameter2B;
    parameter->inputAddr = input;
    parameter->inputLength = 8;
    parameter->outputAddr = output;
    parameter->aramAddr = 0;

    DCFlushRange(input, 8);
    DCInvalidateRange(output, 4);
    DCFlushRange(parameter, sizeof(CARDDecParam));

    task->priority = 255;
    task->iram_mmem_addr = (u16*)((u32)lbl_80312800 - 0x80000000);
    task->iram_length = 0x160;
    task->iram_addr = 0;
    task->dsp_init_vector = 0x10;
    task->init_cb = InitCallback;
    task->res_cb = 0;
    task->done_cb = DoneCallback;
    task->req_cb = 0;
    DSPAddTask(task);

    words = (u32*)flashID;
    *words++ = parameter1A;
    *words++ = parameter1B;
    *words = answer1;
    return 0;
}

u32 AIGetStreamPlayState(void);
u32 AIGetStreamSampleRate(void);
u32 AIGetStreamVolLeft(void);
u32 AIGetStreamVolRight(void);
void AISetStreamVolLeft(u32 volume);
void AISetStreamVolRight(u32 volume);
void __AI_SRC_INIT(void);

u32 AIGetDSPSampleRate(void);

void __AICallbackStackSwitch(AIDCallback callback);

#pragma dont_inline on

#pragma dont_inline off

#endif

#if !defined(SDK_RANGE_EXACT_ACTIVE) || \
    defined(SDK_EXACT_800AE3F0_800AE9FC)
void __ARQServiceQueueLo(void) {
    ARQRequest* request;

    if (lbl_8047A93C == NULL && lbl_8047A930 != NULL) {
        lbl_8047A93C = lbl_8047A930;
        lbl_8047A930 = lbl_8047A930->next;
    }

    request = lbl_8047A93C;
    if (request != NULL) {
        if (request->length <= lbl_8047A948) {
            if (request->type == 0) {
                ARStartDMA(request->type, request->source, request->dest, request->length);
            } else {
                ARStartDMA(request->type, request->dest, request->source, request->length);
            }
            lbl_8047A944 = lbl_8047A93C->callback;
        } else if (request->type == 0) {
            ARStartDMA(request->type, request->source, request->dest, lbl_8047A948);
        } else {
            ARStartDMA(request->type, request->dest, request->source, lbl_8047A948);
        }
        lbl_8047A93C->length -= lbl_8047A948;
        lbl_8047A93C->source += lbl_8047A948;
        lbl_8047A93C->dest += lbl_8047A948;
    }
}

void __ARQCallbackHack(ARQRequest* request) {
}

void __ARQInterruptServiceRoutine(void) {
    ARQRequest* request;

    if (lbl_8047A940 != NULL) {
        lbl_8047A940(lbl_8047A938);
        lbl_8047A938 = NULL;
        lbl_8047A940 = NULL;
    } else if (lbl_8047A944 != NULL) {
        lbl_8047A944(lbl_8047A93C);
        lbl_8047A93C = NULL;
        lbl_8047A944 = NULL;
    }

    request = lbl_8047A928;
    if (request != NULL) {
        if (request->type == 0) {
            ARStartDMA(request->type, request->source, request->dest, request->length);
        } else {
            ARStartDMA(request->type, request->dest, request->source, request->length);
        }
        lbl_8047A940 = lbl_8047A928->callback;
        lbl_8047A938 = lbl_8047A928;
        lbl_8047A928 = lbl_8047A928->next;
    }
    if (lbl_8047A938 == NULL) {
        __ARQServiceQueueLo();
    }
}

#pragma dont_inline on
void ARQInit(void) {
    if (lbl_8047A94C != 1) {
        OSRegisterVersion(lbl_80478A38);
        lbl_8047A930 = NULL;
        lbl_8047A928 = NULL;
        lbl_8047A948 = 0x1000;
        ARRegisterDMACallback(__ARQInterruptServiceRoutine);
        lbl_8047A938 = NULL;
        lbl_8047A93C = NULL;
        lbl_8047A940 = NULL;
        lbl_8047A944 = NULL;
        lbl_8047A94C = 1;
    }
}
#pragma dont_inline off

void ARQPostRequest(ARQRequest* request, u32 owner, u32 type, u32 priority,
                    u32 source, u32 dest, u32 length, ARQCallback callback) {
    BOOL enabled;

    request->next = NULL;
    request->owner = owner;
    request->type = type;
    request->source = source;
    request->dest = dest;
    request->length = length;
    if (callback != NULL) {
        request->callback = callback;
    } else {
        request->callback = __ARQCallbackHack;
    }

    enabled = OSDisableInterrupts();
    switch (priority) {
    case 0:
        if (lbl_8047A930 != NULL) {
            lbl_8047A934->next = request;
        } else {
            lbl_8047A930 = request;
        }
        lbl_8047A934 = request;
        break;
    case 1:
        if (lbl_8047A928 != NULL) {
            lbl_8047A92C->next = request;
        } else {
            lbl_8047A928 = request;
        }
        lbl_8047A92C = request;
        break;
    }

    if (lbl_8047A938 == NULL && lbl_8047A93C == NULL) {
        if (lbl_8047A928 != NULL) {
            if (lbl_8047A928->type == 0) {
                ARStartDMA(lbl_8047A928->type, lbl_8047A928->source,
                           lbl_8047A928->dest, lbl_8047A928->length);
            } else {
                ARStartDMA(lbl_8047A928->type, lbl_8047A928->dest,
                           lbl_8047A928->source, lbl_8047A928->length);
            }
            lbl_8047A940 = lbl_8047A928->callback;
            lbl_8047A938 = lbl_8047A928;
            lbl_8047A928 = lbl_8047A928->next;
        }
        if (lbl_8047A938 == NULL) {
            __ARQServiceQueueLo();
        }
    }

    OSRestoreInterrupts(enabled);
}

u32 ARQGetChunkSize(void) {
    return lbl_8047A948;
}

u32 fn_800AE794(void) {
    return (DSP_REGS->mailToDspHi >> 15) & 1;
}

u32 fn_800AE7A4(void) {
    return (DSP_REGS->mailFromDspHi >> 15) & 1;
}

u32 DSPReadMailFromDSP(void) {
    volatile u16* dsp = __DSPRegs;

    return ((u32)dsp[2] << 16) | dsp[3];
}

void DSPSendMailToDSP(u32 mail) {
    volatile u16* dsp = __DSPRegs;

    dsp[0] = mail >> 16;
    dsp[1] = mail;
}

void DSPInit(void) {
    BOOL enabled;
    u16 tmp;

    __DSP_debug_printf((char*)&lbl_803125E8[0x48], &lbl_803125E8[0x68], &lbl_803125E8[0x74]);
    if (lbl_8047A950 != 1) {
        OSRegisterVersion(lbl_80478A40);
        enabled = OSDisableInterrupts();
        __OSSetInterruptHandler(7, __DSPHandler);
        __OSUnmaskInterrupts(0x01000000);
        __DSPRegs[5] = (__DSPRegs[5] & ~0xa8) | 0x800;
        tmp = __DSPRegs[5];
        __DSPRegs[5] = tmp & ~0xac;
        lbl_8047A960 = 0;
        lbl_8047A96C = NULL;
        lbl_8047A964 = NULL;
        lbl_8047A968 = NULL;
        lbl_8047A950 = 1;
        OSRestoreInterrupts(enabled);
    }
}

void fn_800AE8A4(void) {
    BOOL enabled;
    u16 tmp;

    enabled = OSDisableInterrupts();
    tmp = __DSPRegs[5];
    __DSPRegs[5] = (tmp & ~0xa8) | 0x801;
    lbl_8047A950 = 0;
    OSRestoreInterrupts(enabled);
}

void fn_800AE8EC(void) {
    BOOL enabled;
    u16 tmp;

    enabled = OSDisableInterrupts();
    tmp = __DSPRegs[5];
    __DSPRegs[5] = (tmp & ~0xa8) | 4;
    OSRestoreInterrupts(enabled);
}

u32 fn_800AE92C(void) {
    return DSP_REGS->dmaControl & 0x200;
}

DSPTaskInfo* DSPAddTask(DSPTaskInfo* task) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    __DSP_insert_task(task);
    task->state = 0;
    task->flags = 1;
    OSRestoreInterrupts(enabled);
    if (task == lbl_8047A968) {
        __DSP_boot_task(task);
    }
    return task;
}

void __DSP_debug_printf(char* fmt, ...) {
}
#endif

#if !defined(SDK_RANGE_EXACT_ACTIVE)
void __DSPHandler(__OSInterrupt interrupt, OSContext* context)
{
    u8 unused[4];
    OSContext exception_context;
    u16 control;
    u32 mail;

    control = __DSPRegs[5];
    control = (control & ~0x28) | 0x80;
    __DSPRegs[5] = control;
    OSClearContext(&exception_context);
    OSSetCurrentContext(&exception_context);

    while (fn_800AE7A4() == 0) {
    }
    mail = DSPReadMailFromDSP();
    if ((lbl_8047A96C->flags & 2) && (mail + 0x232F0000) == 2) {
        mail = 0xDCD10003;
    }

    switch (mail) {
    case 0xDCD10000:
        lbl_8047A96C->state = 1;
        if (lbl_8047A96C->init_cb != NULL) {
            lbl_8047A96C->init_cb(lbl_8047A96C);
        }
        break;
    case 0xDCD10001:
        lbl_8047A96C->state = 1;
        if (lbl_8047A96C->res_cb != NULL) {
            lbl_8047A96C->res_cb(lbl_8047A96C);
        }
        break;
    case 0xDCD10002:
        if (lbl_8047A958) {
            if (lbl_8047A96C == lbl_8047A95C) {
                DSPSendMailToDSP(0xCDD10003);
                while (fn_800AE794() != 0) {
                }
                lbl_8047A95C = NULL;
                lbl_8047A958 = 0;
                if (lbl_8047A96C->res_cb != NULL) {
                    lbl_8047A96C->res_cb(lbl_8047A96C);
                }
            } else {
                DSPSendMailToDSP(0xCDD10001);
                while (fn_800AE794() != 0) {
                }
                __DSP_exec_task(lbl_8047A96C, lbl_8047A95C);
                lbl_8047A96C->state = 2;
                lbl_8047A96C = lbl_8047A95C;
                lbl_8047A95C = NULL;
                lbl_8047A958 = 0;
            }
        } else if (lbl_8047A96C->next == NULL) {
            if (lbl_8047A96C == lbl_8047A968) {
                DSPSendMailToDSP(0xCDD10003);
                while (fn_800AE794() != 0) {
                }
                if (lbl_8047A96C->res_cb != NULL) {
                    lbl_8047A96C->res_cb(lbl_8047A96C);
                }
            } else {
                DSPSendMailToDSP(0xCDD10001);
                while (fn_800AE794() != 0) {
                }
                __DSP_exec_task(lbl_8047A96C, lbl_8047A968);
                lbl_8047A96C->state = 2;
                lbl_8047A96C = lbl_8047A968;
            }
        } else {
            DSPSendMailToDSP(0xCDD10001);
            while (fn_800AE794() != 0) {
            }
            __DSP_exec_task(lbl_8047A96C, lbl_8047A96C->next);
            lbl_8047A96C->state = 2;
            lbl_8047A96C = lbl_8047A96C->next;
        }
        break;
    case 0xDCD10003:
        if (lbl_8047A958) {
            if (lbl_8047A96C->done_cb != NULL) {
                lbl_8047A96C->done_cb(lbl_8047A96C);
            }
            DSPSendMailToDSP(0xCDD10001);
            while (fn_800AE794() != 0) {
            }
            __DSP_exec_task(NULL, lbl_8047A95C);
            __DSP_remove_task(lbl_8047A96C);
            lbl_8047A96C = lbl_8047A95C;
            lbl_8047A95C = NULL;
            lbl_8047A958 = 0;
        } else if (lbl_8047A96C->next == NULL) {
            if (lbl_8047A96C == lbl_8047A968) {
                if (lbl_8047A96C->done_cb != NULL) {
                    lbl_8047A96C->done_cb(lbl_8047A96C);
                }
                DSPSendMailToDSP(0xCDD10002);
                while (fn_800AE794() != 0) {
                }
                lbl_8047A96C->state = 3;
                __DSP_remove_task(lbl_8047A96C);
            } else {
                if (lbl_8047A96C->done_cb != NULL) {
                    lbl_8047A96C->done_cb(lbl_8047A96C);
                }
                DSPSendMailToDSP(0xCDD10001);
                while (fn_800AE794() != 0) {
                }
                lbl_8047A96C->state = 3;
                __DSP_exec_task(NULL, lbl_8047A968);
                lbl_8047A96C = lbl_8047A968;
                __DSP_remove_task(lbl_8047A964);
            }
        } else {
            if (lbl_8047A96C->done_cb != NULL) {
                lbl_8047A96C->done_cb(lbl_8047A96C);
            }
            DSPSendMailToDSP(0xCDD10001);
            while (fn_800AE794() != 0) {
            }
            lbl_8047A96C->state = 3;
            __DSP_exec_task(NULL, lbl_8047A96C->next);
            lbl_8047A96C = lbl_8047A96C->next;
            __DSP_remove_task(lbl_8047A96C->prev);
        }
        break;
    case 0xDCD10004:
        if (lbl_8047A96C->req_cb != NULL) {
            lbl_8047A96C->req_cb(lbl_8047A96C);
        }
        break;
    }

    OSClearContext(&exception_context);
    OSSetCurrentContext(context);
}
void __DSP_insert_task(DSPTaskInfo* task) {
    DSPTaskInfo* current;

    if (lbl_8047A968 == NULL) {
        lbl_8047A96C = task;
        lbl_8047A964 = task;
        lbl_8047A968 = task;
        task->prev = NULL;
        task->next = NULL;
        return;
    }

    current = lbl_8047A968;
    while (current != NULL) {
        if (task->priority < current->priority) {
            task->prev = current->prev;
            current->prev = task;
            task->next = current;
            if (task->prev == NULL) {
                lbl_8047A968 = task;
            } else {
                task->prev->next = task;
            }
            break;
        }
        current = current->next;
    }

    if (current == NULL) {
        lbl_8047A964->next = task;
        task->next = NULL;
        task->prev = lbl_8047A964;
        lbl_8047A964 = task;
    }
}

void __DSP_remove_task(DSPTaskInfo* task) {
    task->flags = 0;
    task->state = 3;

    if (lbl_8047A968 == task) {
        if (task->next != NULL) {
            lbl_8047A968 = task->next;
            task->next->prev = NULL;
        } else {
            lbl_8047A96C = NULL;
            lbl_8047A964 = NULL;
            lbl_8047A968 = NULL;
        }
        return;
    }

    if (lbl_8047A964 == task) {
        lbl_8047A964 = task->prev;
        task->prev->next = NULL;
        lbl_8047A96C = lbl_8047A968;
        return;
    }

    lbl_8047A96C = task->next;
    task->prev->next = task->next;
    task->next->prev = task->prev;
}

s32 fn_800B31F4(s32 chan)
{
    extern s32 lbl_80312960[8];
    extern u32 lbl_80312980[8];
    extern u16 lbl_80478A58;
    extern s32 fn_80099400(s32, s32, u32*);
    extern s32 IsCard(u32);
    extern s32 __CARDClearStatus(s32);
    extern s32 fn_800AF660(s32, u8*);
    extern s32 fn_80098944(s32);
    extern s32 __CARDUnlock(s32, u8*);
    extern s32 __CARDEnableInterrupt(s32, s32);
    extern void __CARDExiHandler(void);
    extern void fn_8009870C(s32, void*);
    extern void EXIUnlock(s32);
    extern s32 __CARDRead(s32, u32, u32, void*, CARDCallback);
    extern void __CARDMountCallback(s32, s32);
    extern void DoUnmount(s32, s32);
    CARDMountControl* card =
        (CARDMountControl*)&lbl_803FC620[chan];
    u32 id;
    u8 status;
    s32 result;
    OSSramEx* sram;
    s32 i;
    u8 checksum;
    s32 step;

    if (card->mountStep == 0) {
        if (fn_80099400(chan, 0, &id) == 0) {
            result = -3;
        } else if (IsCard(id)) {
            result = 0;
        } else {
            result = -2;
        }
        if (result < 0) {
            goto error;
        }

        card->cid = id;
        card->size = id & 0xFC;
        card->sectorSize = lbl_80312960[(id & 0x3800) >> 11];
        card->cBlock = (card->size * 1024 * 1024 / 8) / card->sectorSize;
        card->latency = lbl_80312980[(id & 0x700) >> 8];

        result = __CARDClearStatus(chan);
        if (result < 0) {
            goto error;
        }
        result = fn_800AF660(chan, &status);
        if (result < 0) {
            goto error;
        }
        if (!fn_80098944(chan)) {
            result = -3;
            goto error;
        }

        if (!(status & 0x40)) {
            result = __CARDUnlock(chan, card->id);
            if (result < 0) {
                goto error;
            }
            checksum = 0;
            sram = __OSLockSramEx();
            for (i = 0; i < 12; i++) {
                sram->flashID[chan][i] = card->id[i];
                checksum += card->id[i];
            }
            sram->flashIDCheckSum[chan] = (u8)~checksum;
            __OSUnlockSramEx(TRUE);
            return result;
        }

        card->mountStep = 1;
        checksum = 0;
        sram = __OSLockSramEx();
        for (i = 0; i < 12; i++) {
            checksum += sram->flashID[chan][i];
        }
        __OSUnlockSramEx(FALSE);
        if (sram->flashIDCheckSum[chan] != (u8)~checksum) {
            result = -5;
            goto error;
        }
    }

    if (card->mountStep == 1) {
        if (card->cid == 0x80000004) {
            u16 vendor;

            sram = __OSLockSramEx();
            vendor = *(u16*)sram->flashID[chan];
            __OSUnlockSramEx(FALSE);
            if (lbl_80478A58 != 0xFFFF && vendor != lbl_80478A58) {
                result = -2;
                goto error;
            }
        }
        card->mountStep = 2;
        result = __CARDEnableInterrupt(chan, TRUE);
        if (result < 0) {
            goto error;
        }
        fn_8009870C(chan, __CARDExiHandler);
        EXIUnlock(chan);
        DCInvalidateRange(card->workArea, 0xA000);
    }

    step = card->mountStep - 2;
    result = __CARDRead(chan, card->sectorSize * step, 0x2000,
                        (u8*)card->workArea + step * 0x2000,
                        __CARDMountCallback);
    if (result < 0) {
        __CARDPutControlBlock((CARDControl*)card, result);
    }
    return result;

error:
    EXIUnlock(chan);
    DoUnmount(chan, result);
    return result;
}
void __CARDDefaultApiCallback(s32 chan, s32 result) {
}
#endif
#endif

#if defined(CARD_EXACT_800B3B68_ONLY)
s32 __CARDFormatRegionAsync(s32 chan, u16 encode, CARDCallback callback)
{
    CARDControl* card;
    CARDID* id;
    u8* dir;
    u16* fat;
    s16 i;
    s32 result;
    OSSram* sram;
    OSSramEx* sramEx;
    u16 viDTVStatus;
    s64 time;
    s64 rand;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    id = (CARDID*)card->workArea;
    memset(id, 0xFF, 0x2000);
    viDTVStatus = *(volatile u16*)0xCC00206E;
    id->encode = encode;

    sram = (OSSram*)__OSLockSram();
    *(u32*)&id->serial[20] = sram->counterBias;
    *(u32*)&id->serial[24] = sram->language;
    __OSUnlockSram(FALSE);

    rand = time = OSGetTime();

    sramEx = (OSSramEx*)__OSLockSramEx();
    for (i = 0; i < 12; i++) {
        rand = (rand * 1103515245 + 12345) >> 16;
        id->serial[i] = (u8)(sramEx->flashID[chan][i] + rand);
        rand = ((rand * 1103515245 + 12345) >> 16) & 0x7FFF;
    }
    __OSUnlockSramEx(FALSE);

    *(u32*)&id->serial[28] = viDTVStatus;
    *(s64*)&id->serial[12] = time;
    id->deviceID = 0;
    id->size = card->size;
    __CARDCheckSum(id, sizeof(CARDID) - sizeof(u32), &id->checkSum,
                   &id->checkSumInv);

    for (i = 0; i < 2; i++) {
        CARDDirCheck* check;

        dir = (u8*)card->workArea + (1 + i) * 0x2000;
        memset(dir, 0xFF, 0x2000);
        check = (CARDDirCheck*)(dir + 0x1FC0);
        check->checkCode = i;
        __CARDCheckSum(dir, 0x1FFC, &check->checkSum,
                       &check->checkSumInv);
    }

    for (i = 0; i < 2; i++) {
        fat = (u16*)((u8*)card->workArea + (3 + i) * 0x2000);
        memset(fat, 0, 0x2000);
        fat[2] = (u16)i;
        fat[3] = (u16)(card->cBlock - 5);
        fat[4] = 4;
        __CARDCheckSum(&fat[2], 0x1FFC, &fat[0], &fat[1]);
    }

    card->apiCallback = callback ? callback : __CARDDefaultApiCallback;
    DCStoreRange(card->workArea, 0xA000);

    card->formatStep = 0;
    result = fn_800AFFE0(chan, card->sectorSize * card->formatStep,
                         FormatCallback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;
}
#endif

#if !defined(CARD_EXACT_800B3B68_ONLY)
#if !defined(SDK_RANGE_EXACT_ACTIVE)
s32 CARDCheckExAsync(s32 chan, s32* xferBytes, CARDCallback callback)
{
    CARDControl* card;
    CARDDirEntry* dir[2];
    u16* fat[2];
    u16* map;
    s32 result;
    s32 errors;
    s32 currentFat;
    s32 currentDir;
    s32 fileNo;
    u16 iBlock;
    u16 cBlock;
    u16 cFree;
    BOOL updateFat = FALSE;
    BOOL updateDir = FALSE;
    BOOL updateOrphan = FALSE;

    if (xferBytes != NULL) {
        *xferBytes = 0;
    }

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    result = VerifyID(card);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }

    errors = VerifyDir(card, &currentDir);
    errors += VerifyFAT(card, &currentFat);
    if (errors > 1) {
        return __CARDPutControlBlock(card, -6);
    }

    dir[0] = (CARDDirEntry*)((u8*)card->workArea + 0x2000);
    dir[1] = (CARDDirEntry*)((u8*)card->workArea + 0x4000);
    fat[0] = (u16*)((u8*)card->workArea + 0x6000);
    fat[1] = (u16*)((u8*)card->workArea + 0x8000);

    switch (errors) {
    case 0:
        break;
    case 1:
        if (card->dirBlock == NULL) {
            card->dirBlock = dir[currentDir];
            memcpy(dir[currentDir], dir[currentDir ^ 1], 0x2000);
            updateDir = TRUE;
        } else {
            card->fatBlock = fat[currentFat];
            memcpy(fat[currentFat], fat[currentFat ^ 1], 0x2000);
            updateFat = TRUE;
        }
        break;
    }

    map = fat[currentFat ^ 1];
    memset(map, 0, 0x2000);

    for (fileNo = 0; fileNo < 127; fileNo++) {
        CARDDirEntry* entry;

        entry = &((CARDDirEntry*)card->dirBlock)[fileNo];
        if (entry->gameName[0] == 0xFF) {
            continue;
        }

        for (iBlock = entry->startBlock, cBlock = 0;
             iBlock != 0xFFFF && cBlock < entry->length;
             iBlock = ((u16*)card->fatBlock)[iBlock], cBlock++) {
            if (iBlock < 5 || iBlock >= card->cBlock ||
                ++map[iBlock] > 1) {
                return __CARDPutControlBlock(card, -6);
            }
        }
        if (cBlock != entry->length || iBlock != 0xFFFF) {
            return __CARDPutControlBlock(card, -6);
        }
    }

    cFree = 0;
    for (iBlock = 5; iBlock < card->cBlock; iBlock++) {
        u16 nextBlock;

        nextBlock = ((u16*)card->fatBlock)[iBlock];
        if (map[iBlock] == 0) {
            if (nextBlock != 0) {
                ((u16*)card->fatBlock)[iBlock] = 0;
                updateOrphan = TRUE;
            }
            cFree++;
        } else if ((nextBlock < 5 || nextBlock >= card->cBlock) &&
                   nextBlock != 0xFFFF) {
            return __CARDPutControlBlock(card, -6);
        }
    }

    if (cFree != ((u16*)card->fatBlock)[3]) {
        ((u16*)card->fatBlock)[3] = cFree;
        updateOrphan = TRUE;
    }
    if (updateOrphan) {
        cardCheckSumInline(&((u16*)card->fatBlock)[2], 0x1FFC,
                           &((u16*)card->fatBlock)[0],
                           &((u16*)card->fatBlock)[1]);
    }

    memcpy(fat[currentFat ^ 1], fat[currentFat], 0x2000);

    if (updateDir) {
        if (xferBytes != NULL) {
            *xferBytes = 0x2000;
        }
        return __CARDUpdateDir(chan, callback);
    }

    if (updateFat | updateOrphan) {
        if (xferBytes != NULL) {
            *xferBytes = 0x2000;
        }
        return __CARDUpdateFatBlock(chan, card->fatBlock, callback);
    }

    __CARDPutControlBlock(card, 0);
    if (callback != NULL) {
        BOOL enabled;

        enabled = OSDisableInterrupts();
        callback(chan, 0);
        OSRestoreInterrupts(enabled);
    }
    return 0;
}

void __CARDExtHandler(s32 chan) {
    CARDControl* card = &lbl_803FC620[chan];
    CARDCallback callback;

    if (card->attached != 0) {
        card->attached = 0;
        fn_8009870C(chan, 0);
        OSCancelAlarm(&card->alarm);
        callback = card->callback_CC;
        if (callback != NULL) {
            card->callback_CC = NULL;
            callback(chan, -3);
        }
        if (card->result != -1) {
            card->result = -3;
        }
        callback = card->extCallback;
        if (callback != NULL && card->field_24 >= 7) {
            card->extCallback = NULL;
            callback(chan, -3);
        }
    }
}
#endif

#if !defined(SDK_RANGE_EXACT_ACTIVE) || \
    defined(SDK_EXACT_800AF474_800AF8A0)
#pragma optimize_for_size on
void __CARDTxHandler(s32 chan) {
    BOOL error;
    BOOL unlocked = FALSE;
    CARDControl* card;
    CARDCallback callback;

    card = &lbl_803FC620[chan];
    error = !EXIDeselect(chan);
    EXIUnlock(chan);
    callback = card->txCallback;
    if (callback != NULL) {
        card->txCallback = NULL;
        callback(chan, (!error && fn_80098944(chan)) ? 0 : -3);
    }
}
#pragma optimize_for_size reset

#if !defined(SDK_EXACT_800AF474_800AF8A0)
void UnlockedCallback(s32 chan, s32 result) {
    CARDCallback callback;
    CARDControl* card;

    card = &lbl_803FC620[chan];
    if (result >= 0) {
        card->unlockCallback = UnlockedCallback;
        if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
            result = 0;
        } else {
            card->unlockCallback = NULL;
            result = fn_800AF8A0(chan);
        }
    }

    if (result < 0) {
        switch (card->cmd[0]) {
        case 0x52:
            callback = card->txCallback;
            if (callback != NULL) {
                card->txCallback = NULL;
                callback(chan, result);
            }
            break;
        case 0xF2:
        case 0xF4:
        case 0xF1:
            callback = card->callback_CC;
            if (callback != NULL) {
                card->callback_CC = NULL;
                callback(chan, result);
            }
            break;
        }
    }
}
#endif

void __CARDUnlockedHandler(s32 chan, OSContext* context) {
    CARDControl* card = &lbl_803FC620[chan];
    CARDCallback callback = card->unlockCallback;

    if (callback != NULL) {
        card->unlockCallback = NULL;
        callback(chan, fn_80098944(chan) ? 1 : -3);
    }
}

s32 __CARDEnableInterrupt(s32 chan, BOOL enable) {
    u32 cmd;
    BOOL err;

    if (!EXISelect(chan, 0, 4)) {
        return -3;
    }
    cmd = enable ? 0x81010000 : 0x81000000;
    err = !EXIImm(chan, &cmd, 2, 1, NULL);
    err |= !EXISync(chan);
    err |= !EXIDeselect(chan);
    return err ? -3 : 0;
}

s32 fn_800AF660(s32 chan, u8* status) {
    u32 cmd;
    s32 err;

    if (!EXISelect(chan, 0, 4)) {
        return -3;
    }
    cmd = 0x83000000;
    err  = !EXIImm(chan, &cmd, 2, 1, NULL);
    err |= !EXISync(chan);
    err |= !EXIImm(chan, status, 1, 0, NULL);
    err |= !EXISync(chan);
    err |= !EXIDeselect(chan);
    if (err) {
        return -3;
    }
    return 0;
}

s32 __CARDClearStatus(s32 chan) {
    u32 cmd;
    BOOL err;

    if (!EXISelect(chan, 0, 4)) {
        return -3;
    }
    cmd = 0x89000000;
    err = !EXIImm(chan, &cmd, 1, 1, NULL);
    err |= !EXISync(chan);
    err |= !EXIDeselect(chan);
    return err ? -3 : 0;
}

void TimeoutHandler(OSAlarm* alarm) {
    s32 chan;
    CARDControl* card;
    CARDCallback callback;

    for (chan = 0; chan < 2; ++chan) {
        card = &lbl_803FC620[chan];
        if (alarm == &card->alarm) {
            break;
        }
    }
    if (card->attached != 0) {
        fn_8009870C(chan, 0);
        callback = card->callback_CC;
        if (callback != NULL) {
            card->callback_CC = NULL;
            callback(chan, -5);
        }
    }
}
#endif

#if !defined(SDK_RANGE_EXACT_ACTIVE)
s32 CARDCheckAsync(s32 chan, void* callback) {
    s32 xferBytes;

    return CARDCheckExAsync(chan, &xferBytes, callback);
}
#endif
#endif

#if defined(CARD_EXACT_800B3B68_ONLY)
s32 CARDFormatAsync(s32 chan, void* callback) {
    return __CARDFormatRegionAsync(chan, __CARDGetFontEncode(), callback);
}

BOOL __CARDCompareFileName(CARDDirEntry* entry, const char* fileName) {
    char* entryName = entry->fileName;
    char entryChar;
    char c;
    s32 count = 32;

    while (--count >= 0) {
        entryChar = *entryName++;
        c = *fileName++;
        if (entryChar != c) {
            return FALSE;
        }
        if (c == 0) {
            return TRUE;
        }
    }
    if (*fileName == 0) {
        return TRUE;
    }
    return FALSE;
}

s32 fn_800B4270(CARDControl* card, CARDDirEntry* entry) {
    if (entry->gameName[0] == 0xff) {
        return -4;
    }
    if (card->diskId == lbl_803FC840 ||
        (memcmp(entry->gameName, ((DVDDiskID*)card->diskId)->gameName, 4) == 0 &&
         memcmp(entry->company, ((DVDDiskID*)card->diskId)->company, 2) == 0)) {
        return 0;
    }
    return -10;
}

s32 fn_800B4308(CARDDirEntry* entry) {
    if (entry->gameName[0] == 0xff) {
        return -4;
    }
    if (entry->permission & 4) {
        return 0;
    }
    return -10;
}

s32 __CARDGetFileNo(CARDControl* card, const char* fileName, s32* pfileNo) {
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    s32 fileNo;
    s32 result;

    if (card->attached == 0) {
        return -3;
    }

    dir = __CARDGetDirBlock(card);
    for (fileNo = 0; fileNo < 127; fileNo++) {
        entry = &dir[fileNo];
        result = fn_800B4270(card, entry);
        if (result < 0) {
            continue;
        }
        if (__CARDCompareFileName(entry, fileName)) {
            *pfileNo = fileNo;
            return 0;
        }
    }

    return -4;
}
#endif

#if !defined(CARD_EXACT_800B3B68_ONLY)
#if !defined(SDK_RANGE_EXACT_ACTIVE)
u16 __CARDGetFontEncode(void) {
    return lbl_8047A970;
}

void CARDInit(void) {
    CARDControl* card = lbl_803FC620;
    s32 chan;
    extern u16 fn_8009D820(void);
    extern void OSInitThreadQueue(void* queue);
    extern void OSRegisterResetFunction(void* info);
    extern void __CARDSetDiskID(void* diskId);

    if (card[0].diskId == NULL || card[1].diskId == NULL) {
        lbl_8047A970 = fn_8009D820();
        OSRegisterVersion(lbl_80478A48);
        DSPInit();
        OSInitAlarm();
        for (chan = 0; chan < 2; ++chan, ++card) {
            card->result = -3;
            OSInitThreadQueue(&card->_08C);
            OSCreateAlarm(&card->alarm);
        }
        __CARDSetDiskID((void*)0x80000000);
        OSRegisterResetFunction(lbl_803127F0);
    }
}

u32 DummyLen(void) {
    u32 shift = 1;
    u32 count = 0;
    s32 len;
    u32 tick;
    volatile u32 scratch[4];
    extern u32 OSGetTick(void);

    lbl_80478A50 = OSGetTick();
    lbl_80478A50 = lbl_80478A50 * 0x41c64e6d + 0x3039;
    len = ((lbl_80478A50 >> 16) & 0x1f) + 1;
    while (len < 4 && count < 10) {
        tick = OSGetTick() << shift;
        if (++shift > 16) {
            shift = 1;
        }
        lbl_80478A50 = tick;
        ++count;
        lbl_80478A50 = lbl_80478A50 * 0x41c64e6d + 0x3039;
        len = ((lbl_80478A50 >> 16) & 0x1f) + 1;
    }
    if (len < 4) {
        len = 4;
    }
    return len;
}

#pragma dont_inline on
void InitCallback(void* task) {
    CARDControl* card;
    s32 chan;
    u32 workArea;

    for (chan = 0; chan < 2; ++chan) {
        card = &lbl_803FC620[chan];
        if (card->task == task) {
            break;
        }
    }
    workArea = (u32)card->workArea;
    DSPSendMailToDSP(0xff000000);
    while (fn_800AE794() != 0) {
    }
    DSPSendMailToDSP(workArea);
    while (fn_800AE794() != 0) {
    }
}
#pragma dont_inline off

#endif

#if !defined(SDK_RANGE_EXACT_ACTIVE) || \
    defined(SDK_EXACT_800B1788_800B2070)
void BlockReadCallback(s32 chan, s32 result) {
    CARDControl* card = &lbl_803FC620[chan];
    CARDCallback callback;
    extern s32 __CARDReadSegment(s32 chan, CARDCallback callback);

    if (result >= 0) {
        card->xferred += 0x200;
        card->addr += 0x200;
        card->buffer += 0x200;
        if (--card->repeat > 0) {
            result = __CARDReadSegment(chan, BlockReadCallback);
            if (result >= 0) {
                return;
            }
        }
    }
    if (card->apiCallback == NULL) {
        __CARDPutControlBlock(card, result);
    }
    callback = card->xferCallback;
    if (callback != NULL) {
        card->xferCallback = NULL;
        callback(chan, result);
    }
}

s32 __CARDRead(s32 chan, u32 addr, u32 length, void* buffer, CARDCallback callback) {
    CARDControl* card = &lbl_803FC620[chan];
    extern s32 __CARDReadSegment(s32 chan, CARDCallback callback);

    if (card->attached == 0) {
        return -3;
    }
    card->xferCallback = callback;
    card->repeat = length >> 9;
    card->addr = addr;
    card->buffer = buffer;
    return __CARDReadSegment(chan, BlockReadCallback);
}

void fn_800B18C8(s32 chan, s32 result) {
    CARDControl* card = &lbl_803FC620[chan];
    CARDCallback callback;
    extern s32 fn_800AFEC4(s32 chan, CARDCallback callback);

    if (result >= 0) {
        card->xferred += 0x80;
        card->addr += 0x80;
        card->buffer += 0x80;
        if (--card->repeat > 0) {
            result = fn_800AFEC4(chan, fn_800B18C8);
            if (result >= 0) {
                return;
            }
        }
    }
    if (card->apiCallback == NULL) {
        __CARDPutControlBlock(card, result);
    }
    callback = card->xferCallback;
    if (callback != NULL) {
        card->xferCallback = NULL;
        callback(chan, result);
    }
}

#pragma dont_inline on
s32 fn_800B19A4(s32 chan, u32 addr, u32 length, void* buffer, CARDCallback callback) {
    CARDControl* card = &lbl_803FC620[chan];
    extern s32 fn_800AFEC4(s32 chan, CARDCallback callback);

    if (card->attached == 0) {
        return -3;
    }
    card->xferCallback = callback;
    card->repeat = length >> 7;
    card->addr = addr;
    card->buffer = buffer;
    return fn_800AFEC4(chan, fn_800B18C8);
}
#pragma dont_inline off

void* __CARDGetFatBlock(CARDControl* card) {
    return card->fatBlock;
}

void WriteCallback_800C1C98(s32 chan, s32 result) {
    CARDControl* card = &lbl_803FC620[chan];
    CARDCallback callback;
    void* first;
    void* second;
    extern void* memcpy(void* dst, const void* src, u32 length);

    if (result >= 0) {
        first = (u8*)card->workArea + 0x6000;
        second = (u8*)card->workArea + 0x8000;
        if (card->fatBlock == first) {
            card->fatBlock = second;
            memcpy(second, first, 0x2000);
        } else {
            card->fatBlock = first;
            memcpy(first, second, 0x2000);
        }
    }
    if (card->apiCallback == NULL) {
        __CARDPutControlBlock(card, result);
    }
    callback = card->updateCallback;
    if (callback != NULL) {
        card->updateCallback = NULL;
        callback(chan, result);
    }
}

void EraseCallback_800C1D6C(s32 chan, s32 result) {
    CARDControl* card = &lbl_803FC620[chan];
    CARDCallback callback;
    void* fat;
    u32 addr;
    u32 scratch[2];

    if (result >= 0) {
        fat = __CARDGetFatBlock(card);
        addr = card->sectorSize * (((u32)fat - (u32)card->workArea) >> 13);
        result = fn_800B19A4(chan, addr, 0x2000, fat, WriteCallback_800C1C98);
        if (result >= 0) {
            return;
        }
    }
    if (card->apiCallback == NULL) {
        __CARDPutControlBlock(card, result);
    }
    callback = card->updateCallback;
    if (callback != NULL) {
        card->updateCallback = NULL;
        callback(chan, result);
    }
}

static inline u16* cardGetFatBlockInline(CARDControl* card) {
    return card->fatBlock;
}

s32 __CARDAllocBlock(s32 chan, u32 cBlock, CARDCallback callback) {
    CARDControl* card;
    u16* fat;
    u16 iBlock;
    u16 startBlock;
    u16 prevBlock;
    u16 count;

    card = &lbl_803FC620[chan];
    if (!card->attached) {
        return -3;
    }

    fat = cardGetFatBlockInline(card);
    if (fat[3] < cBlock) {
        return -9;
    }

    fat[3] -= cBlock;
    startBlock = 0xFFFF;
    iBlock = fat[4];
    count = 0;
    while (0 < cBlock) {
        if (card->cBlock - 5 < ++count) {
            return -6;
        }

        iBlock++;
        if (iBlock < 5 || iBlock >= card->cBlock) {
            iBlock = 5;
        }

        if (fat[iBlock] == 0) {
            if (startBlock == 0xFFFF) {
                startBlock = iBlock;
            } else {
                fat[prevBlock] = iBlock;
            }
            prevBlock = iBlock;
            fat[iBlock] = 0xFFFF;
            --cBlock;
        }
    }

    fat[4] = iBlock;
    card->startBlock = startBlock;
    return __CARDUpdateFatBlock(chan, fat, callback);
}

inline void* inline_fn(void* arg0) {
    return arg0;
}

s32 __CARDFreeBlock(s32 chan, u16 block, CARDCallback callback) {
    u16* fat;
    CARDControl* card = &lbl_803FC620[chan];
    extern s32 __CARDUpdateFatBlock(s32 chan, u16* fat, CARDCallback callback);
    u16 next;

    if (card->attached == 0) {
        return -3;
    }
    fat = inline_fn(card->fatBlock);
    while (block != 0xffff) {
        if (block < 5 || block >= card->cBlock) {
            return -6;
        }
        next = fat[block];
        fat[block] = 0;
        fat[3]++;
        block = next;
    }
    return __CARDUpdateFatBlock(chan, fat, callback);
}

s32 __CARDUpdateFatBlock(s32 chan, u16* fat, CARDCallback callback) {
    CARDControl* card = &lbl_803FC620[chan];
    extern void __CARDCheckSum(void* data, u32 length, u16* checksum, u16* checksumInv);
    extern void DCStoreRange(void* addr, u32 length);
    extern s32 fn_800AFFE0(s32 chan, u32 addr, void* callback);

    fat[2]++;
    __CARDCheckSum(&fat[2], 0x1ffc, &fat[0], &fat[1]);
    DCStoreRange(fat, 0x2000);
    card->updateCallback = callback;
    return fn_800AFFE0(chan,
                       card->sectorSize * (((u32)fat - (u32)card->workArea) >> 13),
                       EraseCallback_800C1D6C);
}

/* Exact lookup bodies moved to card_exact_800B3B68.c. */

void* __CARDGetDirBlock(CARDControl* card) {
    return card->dirBlock;
}

void WriteCallback_800C209C(s32 chan, s32 result) {
    CARDControl* card = &lbl_803FC620[chan];
    CARDCallback callback;
    void* first;
    void* second;
    extern void* memcpy(void* dst, const void* src, u32 length);

    if (result >= 0) {
        first = (u8*)card->workArea + 0x2000;
        second = (u8*)card->workArea + 0x4000;
        if (card->dirBlock == first) {
            card->dirBlock = second;
            memcpy(second, first, 0x2000);
        } else {
            card->dirBlock = first;
            memcpy(first, second, 0x2000);
        }
    }
    if (card->apiCallback == NULL) {
        __CARDPutControlBlock(card, result);
    }
    callback = card->updateCallback;
    if (callback != NULL) {
        card->updateCallback = NULL;
        callback(chan, result);
    }
}

void EraseCallback_800C216C(s32 chan, s32 result) {
    CARDControl* card = &lbl_803FC620[chan];
    CARDCallback callback;
    void* dir;
    u32 addr;
    u32 scratch[2];

    if (result >= 0) {
        dir = __CARDGetDirBlock(card);
        addr = card->sectorSize * (((u32)dir - (u32)card->workArea) >> 13);
        result = fn_800B19A4(chan, addr, 0x2000, dir, WriteCallback_800C209C);
        if (result >= 0) {
            return;
        }
    }
    if (card->apiCallback == NULL) {
        __CARDPutControlBlock(card, result);
    }
    callback = card->updateCallback;
    if (callback != NULL) {
        card->updateCallback = NULL;
        callback(chan, result);
    }
}

s32 __CARDUpdateDir(s32 chan, CARDCallback callback) {
    CARDControl* card;
    CARDDirCheck* check;
    u32 tmp[2];
    u32 addr;
    u8* dir;
    extern void __CARDCheckSum(void* data, u32 length, u16* checksum, u16* checksumInv);
    extern void DCStoreRange(void* addr, u32 length);

    card = &lbl_803FC620[chan];
    if (card->attached == 0) {
        return -3;
    }
    dir = __CARDGetDirBlock(card);
    check = (CARDDirCheck*)(dir + 0x1fc0);
    check->checkCode++;
    __CARDCheckSum(dir, 0x1ffc, &check->checkSum, &check->checkSumInv);
    DCStoreRange(dir, 0x2000);
    card->updateCallback = callback;
    addr = (((u32)dir - (u32)card->workArea) >> 13) * card->sectorSize;
    return fn_800AFFE0(chan, addr, EraseCallback_800C216C);
}
#endif

#if !defined(SDK_RANGE_EXACT_ACTIVE)
s32 __CARDVerify(CARDControl* card) {
    s32 result;
    s32 dirResult;
    extern s32 VerifyID(CARDControl* card);
    extern s32 VerifyDir(CARDControl* card, s32* checkCode);
    extern s32 VerifyFAT(CARDControl* card, s32* checkCode);

    result = VerifyID(card);
    if (result < 0) {
        return result;
    }
    dirResult = VerifyDir(card, NULL);
    switch (dirResult + VerifyFAT(card, NULL)) {
    case 0:
        return 0;
    case 1:
        return -6;
    default:
        return -6;
    }
}

BOOL IsCard(u32 id) {
    s32 sectorSize;
    u32 cardSize;

    if ((id & 0xffff0000) != 0) {
        if (id != 0x80000004 || lbl_80478A58 == 0xffff) {
            return FALSE;
        }
    }
    if ((id & 3) != 0) {
        return FALSE;
    }
    cardSize = id & 0xfc;
    switch (cardSize) {
    case 4:
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
        break;
    default:
        return FALSE;
    }
    sectorSize = lbl_80312960[(id >> 11) & 7];
    if (sectorSize == 0) {
        return FALSE;
    }
    if (((cardSize << 17) & 0x1ffe0000) / sectorSize < 8) {
        return FALSE;
    }
    return TRUE;
}

void DoUnmount(s32 chan, s32 result) {
    CARDControl* card = &lbl_803FC620[chan];
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (card->attached != 0) {
        fn_8009870C(chan, 0);
        fn_80098AE8(chan);
        OSCancelAlarm(&card->alarm);
        card->attached = 0;
        card->result = result;
        card->field_24 = 0;
    }
    OSRestoreInterrupts(enabled);
}

#pragma dont_inline on
s32 CARDUnmount(s32 chan) {
    CARDControl* card;
    BOOL enabled;
    CARDControl* selected;
    CARDControl* control;
    s32 result;
    extern s32 __CARDGetControlBlock(s32 chan, CARDControl** card);

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    selected = &lbl_803FC620[chan];
    control = selected;
    enabled = OSDisableInterrupts();
    if (control->attached != 0) {
        fn_8009870C(chan, 0);
        fn_80098AE8(chan);
        OSCancelAlarm(&control->alarm);
        control->attached = 0;
        control->result = -3;
        control->field_24 = 0;
    }
    OSRestoreInterrupts(enabled);
    return 0;
}
#pragma dont_inline off

s32 CARDCancel(CARDFileInfo* fileInfo) {
    CARDFileInfo* file = fileInfo;
    CARDControl* card;
    BOOL enabled;
    s32 result;

    enabled = OSDisableInterrupts();
    card = &lbl_803FC620[file->chan];
    result = 0;
    if (card->attached == 0) {
        result = -3;
    } else if (card->result == -1 && card->fileInfo == file) {
        file->length = -1;
        result = -14;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

void EraseCallback(s32 chan, s32 result) {
    CARDControl* card = &lbl_803FC620[chan];
    CARDCallback callback;
    CARDFileInfo* fileInfo;
    extern s32 fn_800B19A4(s32 chan, u32 addr, u32 length, void* buffer,
                           CARDCallback callback);
    extern void WriteCallback(s32 chan, s32 result);

    if (result >= 0) {
        fileInfo = card->fileInfo;
        result = fn_800B19A4(chan, card->sectorSize * fileInfo->startBlock,
                            card->sectorSize, card->buffer, WriteCallback);
        if (result >= 0) {
            return;
        }
    }
    callback = card->apiCallback;
    card->apiCallback = NULL;
    __CARDPutControlBlock(card, result);
    callback(chan, result);
}

void DeleteCallback(s32 chan, s32 result) {
    CARDControl* card = &lbl_803FC620[chan];
    CARDCallback callback = card->apiCallback;
    extern s32 __CARDFreeBlock(s32 chan, u16 block, CARDCallback callback);

    card->apiCallback = NULL;
    if (result >= 0) {
        result = __CARDFreeBlock(chan, card->startBlock, callback);
        if (result >= 0) {
            return;
        }
    }
    __CARDPutControlBlock(card, result);
    if (callback != NULL) {
        callback(chan, result);
    }
}

#pragma push
#pragma inline_depth(0)
s32 CARDDeleteAsync(s32 chan, const char* fileName, CARDCallback callback) {
    CARDControl* card;
    s32 fileNo;
    s32 result;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    extern s32 __CARDGetFileNo(CARDControl* card, const char* fileName,
                               s32* fileNo);
    extern BOOL __CARDIsOpened();
    extern void* memset(void* dst, s32 value, u32 size);

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    result = __CARDGetFileNo(card, fileName, &fileNo);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }
    if (__CARDIsOpened(card, fileNo)) {
        return __CARDPutControlBlock(card, -1);
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileNo];
    card->startBlock = entry->startBlock;
    memset(entry, 0xff, sizeof(CARDDirEntry));

    card->apiCallback = callback ? callback : __CARDDefaultApiCallback;
    result = __CARDUpdateDir(chan, DeleteCallback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;
}
#pragma pop

void __CARDSetDiskID(void* diskId) {
    CARDControl* card = lbl_803FC620;

    card[0].diskId = diskId ? diskId : (void*)&card[2];
    card[1].diskId = diskId ? diskId : (void*)&card[2];
}

void* fn_800B01AC(s32 chan) {
    return lbl_803FC620[chan].diskId;
}

s32 fn_800B01C4(s32 chan, void* diskId) {
    CARDControl* card = &lbl_803FC620[chan];
    void* id;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (card->result == -1) {
        return -1;
    }
    if (diskId != NULL) {
        id = diskId;
    } else {
        id = (void*)0x80000000;
    }
    card->diskId = id;
    OSRestoreInterrupts(enabled);
    return 0;
}

s32 __CARDGetControlBlock(s32 chan, CARDControl** pcard) {
    s32 result;
    CARDControl* card = &lbl_803FC620[chan];
    BOOL enabled;

    if (chan < 0 || chan >= 2 || card->diskId == NULL) {
        return -128;
    }
    enabled = OSDisableInterrupts();
    if (card->attached == 0) {
        result = -3;
    } else if (card->result == -1) {
        result = -1;
    } else {
        card->result = -1;
        card->apiCallback = NULL;
        *pcard = card;
        result = 0;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

s32 CARDClose(CARDFileInfo* fileInfo) {
    CARDControl* card;
    s32 result;

    result = __CARDGetControlBlock(fileInfo->chan, &card);
    if (result < 0) {
        return result;
    }
    fileInfo->chan = -1;
    return __CARDPutControlBlock(card, 0);
}

s32 __CARDPutControlBlock(CARDControl* card, s32 result) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (card->attached) {
        card->result = result;
    } else if (card->result == -1) {
        card->result = result;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

BOOL OnReset_800C0734(BOOL final) {
    if (!final) {
        if (CARDUnmount(0) == -1 || CARDUnmount(1) == -1) {
            return FALSE;
        }
    }
    return TRUE;
}

s32 CARDGetResultCode(s32 chan) {
    CARDControl* card;

    if (chan < 0 || chan >= 2) {
        return -128;
    }
    card = &lbl_803FC620[chan];
    return card->result;
}

s32 CARDFreeBlocks(s32 chan, s32* byteNotUsed, s32* filesNotUsed) {
    CARDControl* card;
    s32 result;
    u16* fat;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    u16 fileNo;
    void* __CARDGetFatBlock(CARDControl* card);
    void* __CARDGetDirBlock(CARDControl* card);

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    fat = __CARDGetFatBlock(card);
    dir = __CARDGetDirBlock(card);
    if (fat == NULL || dir == NULL) {
        return __CARDPutControlBlock(card, -6);
    }

    if (byteNotUsed != NULL) {
        *byteNotUsed = (s32)(card->sectorSize * fat[3]);
    }

    if (filesNotUsed != NULL) {
        *filesNotUsed = 0;
        for (fileNo = 0; fileNo < 127; fileNo++) {
            entry = &dir[fileNo];
            if ((u8)entry->fileName[0] == 0xFF) {
                ++*filesNotUsed;
            }
        }
    }

    return __CARDPutControlBlock(card, 0);
}

/* Exact permission helper moved to card_exact_800B3B68.c. */

BOOL __CARDIsOpened(void) {
    return FALSE;
}

s32 CARDGetAttributes(s32 chan, s32 fileNo, u8* attr) {
    s32 result;
    CARDDirEntry entry;

    result = fn_800B57D0(chan, fileNo, &entry);
    if (result == 0) {
        *attr = entry.permission;
    }
    return result;
}

#pragma dont_inline on
void UpdateIconOffsets(CARDDirEntry* entry, CARDStat* stat);

s32 CARDSetStatusAsync(s32 chan, s32 fileNo, CARDStat* stat,
                       CARDCallback callback)
{
    CARDControl* card;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    s32 result;

    if (fileNo < 0 || fileNo >= 127 ||
        (stat->iconAddr != 0xFFFFFFFF && stat->iconAddr >= 0x200) ||
        (stat->commentAddr != 0xFFFFFFFF &&
         stat->commentAddr % 0x2000 > 0x1FC0)) {
        return -128;
    }

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileNo];
    result = fn_800B4270(card, entry);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }

    entry->bannerFormat = stat->bannerFormat;
    entry->iconAddr = stat->iconAddr;
    entry->iconFormat = stat->iconFormat;
    entry->animationSpeed = stat->iconSpeed;
    entry->commentAddr = stat->commentAddr;
    UpdateIconOffsets(entry, stat);

    if (entry->iconAddr == 0xFFFFFFFF) {
        entry->animationSpeed =
            (entry->animationSpeed & ~3) | 1;
    }

    entry->time = (u32)(OSGetTime() / (*(u32*)0x800000F8 / 4));
    result = __CARDUpdateDir(chan, callback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;
}

s32 fn_800B57D0(s32 chan, s32 fileNo, CARDDirEntry* entry) {
    CARDControl* card;
    s32 result;
    CARDDirEntry* dirEntry;
    extern s32 __CARDGetControlBlock(s32 chan, CARDControl** card);
    extern CARDDirEntry* __CARDGetDirBlock(CARDControl* card);
    extern s32 fn_800B4270(CARDControl* card, CARDDirEntry* entry);
    extern s32 fn_800B4308(CARDDirEntry* entry);
    extern void* memcpy(void* dst, const void* src, u32 length);

    if (fileNo < 0 || fileNo >= 127) {
        return -128;
    }
    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    dirEntry = &__CARDGetDirBlock(card)[fileNo];
    result = fn_800B4270(card, dirEntry);
    if (result == -10) {
        result = fn_800B4308(dirEntry);
    }
    if (result >= 0) {
        memcpy(entry, dirEntry, sizeof(CARDDirEntry));
    }
    return __CARDPutControlBlock(card, result);
}
#pragma dont_inline off

s32 fn_800B5BE4(s32 chan, s32 fileNo, u8 attr, void* callback) {
    s32 result;
    CARDDirEntry entry;

    result = fn_800B57D0(chan, fileNo, &entry);
    if (result < 0) {
        return result;
    }
    entry.permission = attr;
    return fn_800B588C(chan, fileNo, &entry, callback);
}

#pragma peephole off
void* fn_800B5C5C(void* object) {
    extern GXData* gx;
    s32 format;
    u32 count;
    extern u32 fn_800BAE5C(void* object);

    format = fn_800BAE5C(object);
    if (format != 8 && format != 9 && format != 10) {
        u8* data = (u8*)gx;
        count = *(u32*)(data + 0x2c8);
        *(u32*)(data + 0x2c8) = count + 1;
        return data + ((count & 7) << 4) + 0x208;
    } else {
        u8* data = (u8*)gx;
        count = *(u32*)(data + 0x2cc);
        *(u32*)(data + 0x2cc) = count + 1;
        return data + ((count & 3) << 4) + 0x288;
    }
}
#pragma peephole reset

s32 fn_800AFEC4(s32 chan, CARDCallback callback) {
    CARDControl* card;
    s32 result;

    card = &lbl_803FC620[chan];
    card->cmd[0] = 0xF2;
    card->cmd[1] = (card->addr >> 17) & 0x7F;
    card->cmd[2] = (card->addr >> 9) & 0xFF;
    card->cmd[3] = (card->addr >> 7) & 3;
    card->cmd[4] = card->addr & 0x7F;
    card->cmdLen = 5;
    card->field_A4 = 1;
    card->field_A8 = 3;

    result = fn_800AFBDC(chan, NULL, callback);
    if (result == -1) {
        result = 0;
    } else if (result >= 0) {
        if (!fn_80098368(chan, card->cmd, card->cmdLen, 1) ||
            !EXIDma(chan, card->buffer, 0x80, card->field_A4,
                    (EXICallback)__CARDTxHandler)) {
            card->callback_CC = NULL;
            EXIDeselect(chan);
            EXIUnlock(chan);
            result = -3;
        } else {
            result = 0;
        }
    }
    return result;
}

s32 fn_800AFFE0(s32 chan, u32 addr, void* callback) {
    CARDControl* card = &lbl_803FC620[chan];
    s32 result;

    card->cmd[0] = 0xf1;
    card->cmd[1] = (addr >> 17) & 0x7f;
    card->cmd[2] = (addr >> 9) & 0xff;
    card->cmdLen = 3;
    card->field_A4 = -1;
    card->field_A8 = 3;
    result = fn_800AFBDC(chan, NULL, callback);
    if (result == -1) {
        result = 0;
    } else if (result >= 0) {
        if (fn_80098368(chan, card->cmd, card->cmdLen, 1) == 0) {
            card->callback_CC = NULL;
            result = -3;
        } else {
            result = 0;
        }
        EXIDeselect(chan);
        EXIUnlock(chan);
    }
    return result;
}

#pragma peephole off
GXTlutRegion* __GXDefaultTlutRegionCallback(u32 index) {
    extern GXData* gx;
    GXTlutRegion* region;

    if (index >= 20) {
        region = NULL;
    } else {
        region = &gx->defaultTlutRegions[index];
    }
    return region;
}

void __GXInitGX(void) {
    extern GXData* gx;
    extern GXRenderModeObj lbl_80312D30;
    extern GXRenderModeObj lbl_80312F4C;
    extern GXRenderModeObj lbl_803130F0;
    extern GXRenderModeObj lbl_80313294;
    extern u8 lbl_80312AB4[];
    extern u8 lbl_803129E4[];
    extern void fn_800B5C5C();
    extern void fn_800B9BDC(GXColor, u32);
    extern void fn_800B857C();
    extern void fn_800B884C();
    extern void fn_800B7D3C();
    extern void fn_800B856C();
    extern void fn_800B84E0();
    extern void fn_800B80CC();
    extern void fn_800B9404();
    extern void fn_800B944C();
    extern void fn_800B9494();
    extern void GXLoadPosMtxImm();
    extern void GXLoadNrmMtxImm();
    extern void fn_800BD554();
    extern void GXLoadTexMtxImm();
    extern void fn_800BD744(f32, f32, f32, f32, f32, f32);
    extern void fn_800BD394();
    extern void fn_800B953C();
    extern void fn_800B94F0();
    extern void GXSetClipMode();
    extern void fn_800BD7A0();
    extern void fn_800BD830();
    extern void fn_800BA6B0();
    extern void fn_800BA6F4();
    extern void fn_800BA4C8(u32, GXColor);
    extern void fn_800BA5BC(u32, GXColor);
    extern void GXInvalidateTexAll();
    extern void fn_800BB2E4();
    extern void fn_800BB2F8();
    extern void fn_800BC6F0();
    extern void fn_800BC8C8();
    extern void GXSetTevOp();
    extern void fn_800BC618();
    extern void fn_800BC66C();
    extern void fn_800BC454();
    extern void fn_800BC4C0();
    extern void fn_800BC52C();
    extern void fn_800BC580();
    extern void fn_800BBC34();
    extern void fn_800BBC0C();
    extern void fn_800BB97C();
    extern void fn_800BC8F8(u32, f32, f32, f32, f32, GXColor);
    extern void fn_800BCCDC();
    extern void GXSetBlendMode();
    extern void fn_800BCE30();
    extern void fn_800BCE5C();
    extern void GXSetZMode();
    extern void fn_800BCEBC();
    extern void fn_800BCFDC();
    extern void GXSetDstAlpha();
    extern void fn_800BCEF4();
    extern void fn_800BD044();
    extern void fn_800BD07C();
    extern void fn_800B959C();
    extern void fn_800B96BC();
    extern void fn_800B9B14(f32);
    extern void fn_800B9874();
    extern void fn_800B9C44();
    extern void fn_800B9E6C();
    extern void fn_800B984C();
    extern void GXClearBoundingBox();
    extern void fn_800B8F64();
    extern void fn_800B8EC0();
    extern void fn_800B8F94();
    extern void fn_800B8EDC();
    extern void fn_800B8E98();
    extern void fn_800B8EAC();
    extern void fn_800B8F80();
    extern void fn_800B8FB0();
    extern void fn_800BD91C();
    extern void fn_800BE30C();

    f32 identity[3][4];
    GXColor clear = { 64, 64, 64, 255 };
    GXColor black = { 0, 0, 0, 0 };
    GXColor white = { 255, 255, 255, 255 };
    GXRenderModeObj* rmode;
    u32 i;

    switch (VIGetTvFormat()) {
    case 0:
        rmode = &lbl_80312D30;
        break;
    case 1:
        rmode = &lbl_803130F0;
        break;
    case 5:
        rmode = &lbl_80313294;
        break;
    case 2:
        rmode = &lbl_80312F4C;
        break;
    default:
        rmode = &lbl_80312D30;
        break;
    }

    fn_800B9BDC(clear, 0xFFFFFF);
    fn_800B857C(0, 1, 4, 0x3C, 0, 0x7D);
    fn_800B857C(1, 1, 5, 0x3C, 0, 0x7D);
    fn_800B857C(2, 1, 6, 0x3C, 0, 0x7D);
    fn_800B857C(3, 1, 7, 0x3C, 0, 0x7D);
    fn_800B857C(4, 1, 8, 0x3C, 0, 0x7D);
    fn_800B857C(5, 1, 9, 0x3C, 0, 0x7D);
    fn_800B857C(6, 1, 10, 0x3C, 0, 0x7D);
    fn_800B857C(7, 1, 11, 0x3C, 0, 0x7D);
    fn_800B884C(1);
    fn_800B7D3C();
    fn_800B856C();
    for (i = 9; i <= 24; i++) {
        fn_800B84E0(i, gx, 0);
    }
    for (i = 0; i < 8; i++) {
        fn_800B80CC(i, lbl_803129E4);
    }
    fn_800B9404(6, 0);
    fn_800B944C(6, 0);
    fn_800B9494(0, 0, 0);
    fn_800B9494(1, 0, 0);
    fn_800B9494(2, 0, 0);
    fn_800B9494(3, 0, 0);
    fn_800B9494(4, 0, 0);
    fn_800B9494(5, 0, 0);
    fn_800B9494(6, 0, 0);
    fn_800B9494(7, 0, 0);

    identity[0][0] = 1.0f;
    identity[0][1] = 0.0f;
    identity[0][2] = 0.0f;
    identity[0][3] = 0.0f;
    identity[1][0] = 0.0f;
    identity[1][1] = 1.0f;
    identity[1][2] = 0.0f;
    identity[1][3] = 0.0f;
    identity[2][0] = 0.0f;
    identity[2][1] = 0.0f;
    identity[2][2] = 1.0f;
    identity[2][3] = 0.0f;
    GXLoadPosMtxImm(identity, 0);
    GXLoadNrmMtxImm(identity, 0);
    fn_800BD554(0);
    GXLoadTexMtxImm(identity, 0x3C, 0);
    GXLoadTexMtxImm(identity, 0x7D, 0);

    fn_800BD744(0.0f, 0.0f, (f32)rmode->fbWidth,
                (f32)rmode->xfbHeight, 0.0f, 1.0f);
    fn_800BD394(lbl_80312AB4);
    fn_800B953C(0);
    fn_800B94F0(2);
    GXSetClipMode(0);
    fn_800BD7A0(0, 0, rmode->fbWidth, rmode->efbHeight);
    fn_800BD830(0, 0);
    fn_800BA6B0(0);
    fn_800BA6F4(4, 0, 0, 1, 0, 0, 2);
    fn_800BA4C8(4, black);
    fn_800BA5BC(4, white);
    fn_800BA6F4(5, 0, 0, 1, 0, 0, 2);
    fn_800BA4C8(5, black);
    fn_800BA5BC(5, white);
    GXInvalidateTexAll();

    {
        GXData* gxState = gx;
        gxState->nextTexRgn = 0;
        gxState->nextTexRgnCI = 0;
    }
    fn_800BB2E4(fn_800B5C5C);
    fn_800BB2F8(__GXDefaultTlutRegionCallback);

    fn_800BC6F0(0, 0, 0, 4);
    fn_800BC6F0(1, 1, 1, 4);
    fn_800BC6F0(2, 2, 2, 4);
    fn_800BC6F0(3, 3, 3, 4);
    fn_800BC6F0(4, 4, 4, 4);
    fn_800BC6F0(5, 5, 5, 4);
    fn_800BC6F0(6, 6, 6, 4);
    fn_800BC6F0(7, 7, 7, 4);
    fn_800BC6F0(8, 0xFF, 0xFF, 0xFF);
    fn_800BC6F0(9, 0xFF, 0xFF, 0xFF);
    fn_800BC6F0(10, 0xFF, 0xFF, 0xFF);
    fn_800BC6F0(11, 0xFF, 0xFF, 0xFF);
    fn_800BC6F0(12, 0xFF, 0xFF, 0xFF);
    fn_800BC6F0(13, 0xFF, 0xFF, 0xFF);
    fn_800BC6F0(14, 0xFF, 0xFF, 0xFF);
    fn_800BC6F0(15, 0xFF, 0xFF, 0xFF);
    fn_800BC8C8(1);
    GXSetTevOp(0, 3);
    fn_800BC618(7, 0, 0, 7, 0);
    fn_800BC66C(0, 0x11, 0);
    for (i = 0; i < 16; i++) {
        fn_800BC454(i, 6);
        fn_800BC4C0(i, 0);
        fn_800BC52C(i, 0, 0);
    }
    fn_800BC580(0, 0, 1, 2, 3);
    fn_800BC580(1, 0, 0, 0, 3);
    fn_800BC580(2, 1, 1, 1, 3);
    fn_800BC580(3, 2, 2, 2, 3);
    for (i = 0; i < 16; i++) {
        fn_800BBC34(i);
    }
    fn_800BBC0C(0);
    fn_800BB97C(0, 0, 0);
    fn_800BB97C(1, 0, 0);
    fn_800BB97C(2, 0, 0);
    fn_800BB97C(3, 0, 0);

    fn_800BC8F8(0, 0.0f, 1.0f, 0.1f, 1.0f, black);
    fn_800BCCDC(0, 0, 0);
    GXSetBlendMode(0, 4, 5, 0);
    fn_800BCE30(1);
    fn_800BCE5C(1);
    GXSetZMode(1, 3, 1);
    fn_800BCEBC(1);
    fn_800BCFDC(1);
    GXSetDstAlpha(0, 0);
    fn_800BCEF4(0, 0);
    fn_800BD044(1, 1);
    fn_800BD07C(rmode->field_rendering,
                rmode->viHeight == 2 * rmode->xfbHeight ? 1 : 0);

    fn_800B959C(0, 0, rmode->fbWidth, rmode->efbHeight);
    fn_800B96BC(rmode->fbWidth, rmode->efbHeight);
    fn_800B9B14((f32)rmode->xfbHeight / (f32)rmode->efbHeight);
    fn_800B9874(3);
    fn_800B9C44(rmode->aa, rmode->sample_pattern, 1, rmode->vfilter);
    fn_800B9E6C(0);
    fn_800B984C(0);
    GXClearBoundingBox();

    fn_800B8F64(1);
    fn_800B8EC0(1);
    fn_800B8F94(0);
    fn_800B8EDC(0, 0, 1, 15);
    fn_800B8E98(7, 0);
    fn_800B8EAC(1);
    fn_800B8F80(0, 0);
    fn_800B8FB0(1, 7, 1);
    fn_800BD91C(0x23, 0x16);
    fn_800BE30C();
}

void* GXInit(void* base, u32 size) {
    extern GXData* gx;
    extern const char* __GXVersion;
    extern u8 gxData_803FC860[];
    extern u8 GXResetFuncInfo_80312AD0[];
    extern u32 __OSBusClock;
    extern u16* __memReg;
    extern u16* __peReg;
    extern u32* __piReg;
    extern void OSRegisterVersion();
    extern void OSRegisterResetFunction();
    extern u32 PPCMfhid2();
    extern void PPCMtwpar();
    extern void PPCMthid2();
    extern void GXSetMisc();
    extern void __GXFifoInit();
    extern void GXInitFifoBase();
    extern void GXSetCPUFifo();
    extern void GXSetGPFifo();
    extern void __GXPEInit();
    extern void __GXFlushTextureState();
    extern void GXInitTexCacheRegion();
    extern void GXInitTlutRegion();
    extern void __GXSetTmemConfig();

    volatile u8* fifo8 = (volatile u8*)0xCC008000;
    volatile u32* fifo32 = (volatile u32*)0xCC008000;
    u8* state = (u8*)gx;
    u32 i;
    u32 reg;
    u32 freqBase;
    u32 hid2;
    u32 half;

    OSRegisterVersion(__GXVersion);
    state[0x4F0] = 0;
    state[0x4F1] = 1;
    state[0x4F2] = 1;
    *(u32*)(state + 0x4DC) = 0;
    *(u32*)(state + 0x4E0) = 0;
    GXSetMisc(1, 0);

    __cpReg = (u16*)0xCC000000;
    __piReg = (u32*)0xCC003000;
    __peReg = (u16*)0xCC001000;
    __memReg = (u16*)0xCC004000;
    __GXFifoInit();
    GXInitFifoBase((GXFifoObj*)(gxData_803FC860 + 0x4F8), base, size);
    GXSetCPUFifo((GXFifoObj*)(gxData_803FC860 + 0x4F8));
    GXSetGPFifo((GXFifoObj*)(gxData_803FC860 + 0x4F8));
    if (*(u32*)0x8047A99C == 0) {
        OSRegisterResetFunction(GXResetFuncInfo_80312AD0);
        *(u32*)0x8047A99C = 1;
    }
    __GXPEInit();
    hid2 = (u32)PPCMfhid2();
    PPCMtwpar(0x0C008000);
    PPCMthid2(hid2 | 0x40000000);

    *(u32*)(state + 0x204) = 0;
    *(u32*)(state + 0x204) &= 0x00FFFFFF;
    *(u32*)(state + 0x124) = 0x0F000000;
    *(u32*)(state + 0x7C) = 0x22000000;
    for (i = 0; i < 16; i++) {
        half = i >> 1;
        *(u32*)(state + 0x130 + i * 4) = 0;
        *(u32*)(state + 0x170 + i * 4) = 0;
        *(u32*)(state + 0x100 + half * 4) = 0;
        *(u32*)(state + 0x49C + i * 4) = 0xFF;
        *(u32*)(state + 0x130 + i * 4) =
            (*(u32*)(state + 0x130 + i * 4) & 0x00FFFFFF) |
            ((0xC0 + i * 2) << 24);
        *(u32*)(state + 0x170 + i * 4) =
            (*(u32*)(state + 0x170 + i * 4) & 0x00FFFFFF) |
            ((0xC1 + i * 2) << 24);
        *(u32*)(state + 0x1B0 + half * 4) =
            (*(u32*)(state + 0x1B0 + half * 4) & 0x00FFFFFF) |
            ((0xF6 + half) << 24);
        *(u32*)(state + 0x100 + half * 4) =
            (*(u32*)(state + 0x100 + half * 4) & 0x00FFFFFF) |
            ((0x28 + half) << 24);
    }
    *(u32*)(state + 0x120) = 0x27000000;
    for (i = 0; i < 8; i++) {
        *(u32*)(state + 0xB8 + i * 4) = 0x30000000 + i * 0x02000000;
        *(u32*)(state + 0xD8 + i * 4) = 0x31000000 + i * 0x02000000;
    }
    *(u32*)(state + 0xF8) = 0x20000000;
    *(u32*)(state + 0xFC) = 0x21000000;
    *(u32*)(state + 0x1D0) = 0x41000000;
    *(u32*)(state + 0x1D4) = 0x42000000;
    *(u32*)(state + 0x1D8) = 0x40000000;
    *(u32*)(state + 0x1DC) = 0x43000000;
    *(u32*)(state + 0x1FC) &= ~0x01800000;
    *(u32*)(state + 0x4F4) = 0;
    state[0x4F3] = 0;

    freqBase = __OSBusClock / 500;
    __GXFlushTextureState();
    reg = (freqBase >> 11) | 0x69000400;
    *fifo8 = 0x61;
    *fifo32 = reg;
    __GXFlushTextureState();
    reg = (freqBase / 0x1080) | 0x46000200;
    *fifo8 = 0x61;
    *fifo32 = reg;

    for (i = 0; i < 8; i++) {
        *(u32*)(state + 0x1C + i * 4) |= 2;
        *(u32*)(state + 0x3C + i * 4) |= 1;
        *fifo8 = 8;
        *fifo8 = i | 0x80;
        *fifo32 = *(u32*)(state + 0x3C + i * 4);
    }
    *fifo8 = 0x10;
    *fifo32 = 0x1000;
    *fifo32 = 0x3F;
    *fifo8 = 0x10;
    *fifo32 = 0x1012;
    *fifo32 = 1;
    *fifo8 = 0x61;
    *fifo32 = 0x5800000F;

    for (i = 0; i < 8; i++) {
        GXInitTexCacheRegion((GXTexRegion*)(state + 0x208 + i * 0x10), 0,
                             i * 0x8000, 0, 0x80000 + i * 0x8000, 0);
    }
    for (i = 0; i < 4; i++) {
        GXInitTexCacheRegion((GXTexRegion*)(state + 0x288 + i * 0x10), 0,
                             (i * 2 + 8) * 0x8000, 0,
                             (i * 2 + 9) * 0x8000, 0);
    }
    for (i = 0; i < 16; i++) {
        GXInitTlutRegion((GXTlutRegion*)(state + 0x2D0 + i * 0x10),
                         0xC0000 + i * 0x2000, 16);
    }
    for (i = 0; i < 4; i++) {
        GXInitTlutRegion((GXTlutRegion*)(state + 0x3D0 + i * 0x10),
                         0xE0000 + i * 0x8000, 64);
    }

    __cpReg[3] = 0;
    *(u32*)(state + 0x4EC) &= ~0x0F000000;
    *fifo8 = 8;
    *fifo8 = 0x20;
    *fifo32 = *(u32*)(state + 0x4EC);
    *fifo8 = 0x10;
    *fifo32 = 0x1006;
    *fifo32 = 0;
    *fifo8 = 0x61;
    *fifo32 = 0x23000000;
    *fifo8 = 0x61;
    *fifo32 = 0x24000000;
    *fifo8 = 0x61;
    *fifo32 = 0x67000000;
    __GXSetTmemConfig(0);
    __GXInitGX();
    return gxData_803FC860 + 0x4F8;
}
#pragma peephole reset

extern GXData* const gx;

void fn_800B7594(u8 overflow, u8 underflow);
void fn_800B75D0(u8 clearOverflow, u8 clearUnderflow);

static void GXOverflowHandler(s16 interrupt, OSContext* context) {
    lbl_8047A9B8++;
    fn_800B7594(0, 1);
    fn_800B75D0(1, 0);
    lbl_8047A9B0 = TRUE;
    OSSuspendThread(lbl_8047A9A8);
}

static void GXUnderflowHandler(s16 interrupt, OSContext* context) {
    OSResumeThread(lbl_8047A9A8);
    lbl_8047A9B0 = FALSE;
    fn_800B75D0(1, 1);
    fn_800B7594(1, 0);
}

static void GXBreakPointHandler(s16 interrupt, OSContext* context) {
    OSContext exceptionContext;

    gx->cpEnable = gx->cpEnable & 0xFFFFFFDF;
    __cpReg[1] = gx->cpEnable;
    if (lbl_8047A9B4 != NULL) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        lbl_8047A9B4();
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

void GXCPInterruptHandler(s16 interrupt, OSContext* context) {
    gx->cpStatus = __cpReg[0];
    if (GET_REG_FIELD(gx->cpEnable, 1, 3) &&
        GET_REG_FIELD(gx->cpStatus, 1, 1)) {
        GXUnderflowHandler(interrupt, context);
    }
    if (GET_REG_FIELD(gx->cpEnable, 1, 2) &&
        GET_REG_FIELD(gx->cpStatus, 1, 0)) {
        GXOverflowHandler(interrupt, context);
    }
    if (GET_REG_FIELD(gx->cpEnable, 1, 5) &&
        GET_REG_FIELD(gx->cpStatus, 1, 4)) {
        GXBreakPointHandler(interrupt, context);
    }
}

void GXInitFifoPtrs(GXFifoObj* fifo, void* readPtr, void* writePtr);

void GXInitFifoBase(GXFifoObj* fifo, void* base, u32 size) {
    extern void fn_800B71F0(GXFifoObj* fifo, u32 hiWatermark, u32 loWatermark);

    fifo->base = base;
    fifo->top = (u8*)base + size - 4;
    fifo->size = size;
    fifo->count = 0;
    fn_800B71F0(fifo, size - 0x4000, (size >> 1) & ~0x1f);
    GXInitFifoPtrs(fifo, base, base);
}

void GXInitFifoPtrs(GXFifoObj* fifo, void* readPtr, void* writePtr) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    fifo->rdPtr = readPtr;
    fifo->wrPtr = writePtr;
    fifo->count = (u8*)writePtr - (u8*)readPtr;
    if (fifo->count < 0) {
        fifo->count += fifo->size;
    }
    OSRestoreInterrupts(enabled);
}
#endif
#endif

#undef SDK_RANGE_EXACT_ACTIVE
