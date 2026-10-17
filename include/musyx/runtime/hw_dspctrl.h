#ifndef MUSYX_RUNTIME_HW_DSPCTRL_H
#define MUSYX_RUNTIME_HW_DSPCTRL_H

#include "dolphin/types.h"

typedef struct SND_STUDIO_INPUT {
    u8 vol;
    u8 volA;
    u8 volB;
    u8 srcStudio;
} SND_STUDIO_INPUT;

typedef struct SAMPLE_INFO {
    u32 info;
    void* addr;
    void* extraData;
    u32 offset;
    u32 length;
    u32 loop;
    u32 loopLength;
    u8 compType;
} SAMPLE_INFO;

typedef struct VSampleInfo {
    void* loopBufferAddr;
    u32 loopBufferLength;
    u8 inLoopBuffer;
} VSampleInfo;

typedef struct AdsrVars {
    u8 mode;
    u8 state;
    u32 cnt;
    s32 currentVolume;
    s32 currentIndex;
    s32 currentDelta;
    union {
        struct {
            u32 aTime;
            u32 dTime;
            u16 sLevel;
            u32 rTime;
            u16 cutOff;
            u8 aMode;
        } dls;
        struct {
            u32 aTime;
            u32 dTime;
            u16 sLevel;
            u32 rTime;
        } linear;
    } data;
} AdsrVars;

typedef struct _PBMIX {
    u16 vL, vDeltaL;
    u16 vR, vDeltaR;
    u16 vAuxAL, vDeltaAuxAL;
    u16 vAuxAR, vDeltaAuxAR;
    u16 vAuxBL, vDeltaAuxBL;
    u16 vAuxBR, vDeltaAuxBR;
    u16 vAuxBS, vDeltaAuxBS;
    u16 vS, vDeltaS;
    u16 vAuxAS, vDeltaAuxAS;
} _PBMIX;

typedef struct _PBITD {
    u16 flag;
    u16 bufferHi, bufferLo;
    u16 shiftL, shiftR;
    u16 targetShiftL, targetShiftR;
} _PBITD;

typedef struct _PBUPDATE {
    u16 updNum[5];
    u16 dataHi, dataLo;
} _PBUPDATE;

typedef struct _PBDPOP {
    u16 aL, aAuxAL, aAuxBL;
    u16 aR, aAuxAR, aAuxBR;
    u16 aS, aAuxAS, aAuxBS;
} _PBDPOP;

typedef struct _PBVE {
    u16 currentVolume;
    u16 currentDelta;
} _PBVE;

typedef struct _PBFIR {
    u16 numCoefs;
    u16 coefsHi, coefsLo;
} _PBFIR;

typedef struct _PBADDR {
    u16 loopFlag;
    u16 format;
    u16 loopAddressHi, loopAddressLo;
    u16 endAddressHi, endAddressLo;
    u16 currentAddressHi, currentAddressLo;
} _PBADDR;

typedef struct _PBADPCM {
    u16 a[8][2];
    u16 gain;
    u16 pred_scale;
    u16 yn1, yn2;
} _PBADPCM;

typedef struct _PBSRC {
    u16 ratioHi, ratioLo;
    u16 currentAddressFrac;
    u16 last_samples[4];
} _PBSRC;

typedef struct _PBADPCMLOOP {
    u16 loop_pred_scale;
    u16 loop_yn1, loop_yn2;
} _PBADPCMLOOP;

typedef struct _PB {
    u16 nextHi, nextLo;
    u16 currHi, currLo;
    u16 srcSelect;
    u16 coefSelect;
    u16 mixerCtrl;
    u16 state;
    u16 loopType;
    _PBMIX mix;
    _PBITD itd;
    _PBUPDATE update;
    _PBDPOP dpop;
    _PBVE ve;
    _PBFIR fir;
    _PBADDR addr;
    _PBADPCM adpcm;
    _PBSRC src;
    _PBADPCMLOOP adpcmLoop;
    u16 streamLoopCnt;
} _PB;

typedef struct DSPvoice {
    _PB* pb;
    void* patchData;
    void* itdBuffer;
    struct DSPvoice* next;
    struct DSPvoice* prev;
    struct DSPvoice* nextAlien;
    u32 mesgCallBackUserValue;
    u32 prio;
    u32 currentAddr;
    u32 changed[5];
    u32 pitch[5];
    u16 volL;
    u16 volR;
    u16 volS;
    u16 volLa;
    u16 volRa;
    u16 volSa;
    u16 volLb;
    u16 volRb;
    u16 volSb;
    u16 lastVolL;
    u16 lastVolR;
    u16 lastVolS;
    u16 lastVolLa;
    u16 lastVolRa;
    u16 lastVolSa;
    u16 lastVolLb;
    u16 lastVolRb;
    u16 lastVolSb;
    u16 smp_id;
    SAMPLE_INFO smp_info;
    VSampleInfo vSampleInfo;
    u8 streamLoopPS;
    AdsrVars adsr;
    u16 srcTypeSelect;
    u16 srcCoefSelect;
    u16 itdShiftL;
    u16 itdShiftR;
    u8 singleOffset;
    struct {
        u32 posHi;
        u32 posLo;
        u32 pitch;
    } playInfo;
    struct {
        u8 pitch;
        u8 vol;
        u8 volA;
        u8 volB;
    } lastUpdate;
    u32 virtualSampleID;
    u8 state;
    u8 postBreak;
    u8 startupBreak;
    u8 studio;
    u32 flags;
} DSPvoice;

typedef struct DSPhostDPop {
    s32 l;
    s32 r;
    s32 s;
    s32 lA;
    s32 rA;
    s32 sA;
    s32 lB;
    s32 rB;
    s32 sB;
} DSPhostDPop;

typedef struct DSPinput {
    u8 studio;
    u16 vol;
    u16 volA;
    u16 volB;
    SND_STUDIO_INPUT* desc;
} DSPinput;

typedef struct DSPstudioinfo {
    void* spb;
    DSPhostDPop hostDPopSum;
    s32* main[2];
    s32* auxA[3];
    s32* auxB[3];
    DSPvoice* voiceRoot;
    DSPvoice* alienVoiceRoot;
    u8 state;
    u8 isMaster;
    u8 numInputs;
    u8 pad_53;
    s32 type;
    DSPinput in[7];
    void* auxAHandler;
    void* auxBHandler;
    void* auxAUser;
    void* auxBUser;
} DSPstudioinfo;

extern DSPstudioinfo lbl_80447E60[];
extern DSPvoice* lbl_8047B024;
extern u16* lbl_8047B010;
extern u8* lbl_8047B018;
extern u16* lbl_8047B01C;
extern void* lbl_8047B020;
extern u32 lbl_8047B028;
extern u8 lbl_8047B05C;
extern u8 lbl_8047B05D;
extern u8 lbl_8047B05E; /* salAuxFrame */

void* memset(void* dst, int value, u32 size);
void DCFlushRangeNoSync(void* addr, u32 size);
void salFree(void* ptr);

#endif /* MUSYX_RUNTIME_HW_DSPCTRL_H */
