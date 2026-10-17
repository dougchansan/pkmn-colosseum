/**
 * @file hardware.c
 * @brief MusyX hardware abstraction layer, 0x8016221C - 0x80163214.
 *
 * The reference MusyX runtime's hardware.c (AxioDL/musyx) in its 2.0.0
 * form, built as one translation unit. Functions the game never calls
 * (hwGlobalActivity, hwChangeStudioMix, hwIsStudioActive, hwChangeStudio,
 * hwPrepareStreamBuffer, hwEnableHRTF) are dead-stripped from the DOL;
 * they are kept as in the reference and stripped again at link, as are
 * the copies of the static helpers MWCC inlines (SetupITD,
 * convert_length). The retained functions keep their symbol-map names.
 *
 * The TU owns its data, which MWCC lays out exactly as retail: itdOffTab
 * (.rodata 0x80273448 - 0x80273548; `static volatile const` in the
 * reference, so every read goes to memory), the dspSRCType/dspCoefSel
 * function statics (.sdata 0x80478BF8 - 0x80478C08) and the literal pool
 * (.sdata2 0x8047D4D8 - 0x8047D4F0). The sal* globals it defines in the
 * reference (salFrame, salAuxFrame, salNumVoices, salMaxStudioNum,
 * salHooks, salTimeOffset) stay extern under their symbol-map labels
 * because other TUs reference them by those names.
 */
#include "dolphin/types.h"
#include "musyx/runtime/hw_dspctrl.h"

typedef struct SND_PLAYBACKINFO {
    u32 frq;
    u8 stereo;
    u8 bits;
    s8 deviceName[256];
    s8 versionText[256];
} SND_PLAYBACKINFO;

typedef struct SynthInfo {
    u32 mixFrq;
    u32 numSamples;
    SND_PLAYBACKINFO pbInfo;
    u8 voiceNum;
    u8 maxMusic;
    u8 maxSFX;
    u8 studioNum;
} SynthInfo;

typedef struct SAL_VOLINFO {
    f32 volL;
    f32 volR;
    f32 volS;
    f32 volAuxAL;
    f32 volAuxAR;
    f32 volAuxAS;
    f32 volAuxBL;
    f32 volAuxBR;
    f32 volAuxBS;
} SAL_VOLINFO;

typedef struct ADSR_INFO {
    union {
        struct {
            s32 atime;
            s32 dtime;
            u16 slevel;
            u16 rtime;
            s32 ascale;
            s32 dscale;
        } dls;
        struct {
            u16 atime;
            u16 dtime;
            u16 slevel;
            u16 rtime;
        } linear;
    } data;
} ADSR_INFO;

typedef struct SND_HOOKS {
    void* (*malloc)(u32 len);
    void (*free)(void* addr);
} SND_HOOKS;

typedef u32 SND_STUDIO_TYPE;
#define SND_STUDIO_TYPE_DPL2 1
typedef void (*SND_AUX_CALLBACK)(u8 reason, void* info, void* user);
typedef void* (*ARAMUploadCallback)(u32, u32);
typedef void (*SND_MESSAGE_CALLBACK)(u32 mesg, u32 voiceID);

extern SynthInfo lbl_80434C50;               /* lbl_80434C50 */
extern u8 lbl_8047AF18;                      /* lbl_8047AF18 */
extern u8 lbl_8047B05F;                      /* lbl_8047B05F */
extern u8 lbl_8047B05E;                      /* lbl_8047B05E */
extern u32 lbl_8047B014;                     /* lbl_8047B014 */
extern u8 lbl_8047B050;                      /* lbl_8047B050 */
extern SND_HOOKS lbl_8047B054;               /* lbl_8047B054 */
extern u8 lbl_8036944C[];                    /* lbl_8036944C */

extern void streamCorrectLoops(void);               /* streamCorrectLoops */
extern u32 hwIRQEnterCritical(void);                /* hwIRQEnterCritical */
extern u32 hwIRQLeaveCritical(void);                /* hwIRQLeaveCritical */
extern void salCtrlDsp(void* dest);
extern void* salAiGetDest(void);
extern void salHandleAuxProcessing(void);
extern void seqHandle(u32 deltaTime);      /* seqHandle */
extern void synthHandle(u32 deltaTime);
extern void s3dHandle(void);               /* s3dHandle */
extern void streamHandle(void);               /* streamHandle */
extern void vsSampleUpdates(void);
extern void hwInitIrq(void);
extern u32 salInitAi(void (*callback)(void), u32 unk, u32* outFreq);
extern u32 salInitDspCtrl(u8 numVoices, u8 numStudios, u32 defaultStudioDPL2);
extern u32 salInitDsp(u32 flags);           /* salInitDsp */
extern void hwEnableIrq(void);
extern void hwDisableIrq(void);
extern void salStartAi(void);               /* salStartAi */
extern u32 salExitDsp(void);                /* salExitDsp */
extern u32 salExitDspCtrl(void);
extern u32 salExitAi(void);
extern void hwExitIrq(void);               /* hwExitIrq */
extern void salActivateVoice(DSPvoice* dsp_vptr, u8 studio);
extern void salDeactivateVoice(DSPvoice* dsp_vptr);
extern void salActivateStudio(u8 studio, u32 isMaster, SND_STUDIO_TYPE type);
extern void salDeactivateStudio(u8 studio);          /* salDeactivateStudio */
extern u32 salAddStudioInput(DSPstudioinfo* stp, SND_STUDIO_INPUT* desc); /* salAddStudioInput */
extern u32 salRemoveStudioInput(DSPstudioinfo* stp, SND_STUDIO_INPUT* desc); /* salRemoveStudioInput */
extern void salCalcVolume(u8 voltab_index, SAL_VOLINFO* vi, f32 vol, u32 pan, u32 span, f32 auxa,
                          f32 auxb, u32 itd, u32 dpl2);
extern u32 adsrConvertTimeCents(s32 tc);
extern u32 aramGetStreamBufferAddress(u8 id, u32* len);
extern void DCStoreRange(void* addr, u32 size);
extern void aramUploadData(void* mram, u32 aram, u32 len, u32 highPrio, void (*callback)(u32),
                           u32 user);
extern u8 aramAllocateStreamBuffer(u32 len);              /* aramAllocateStreamBuffer */
extern void aramFreeStreamBuffer(u8 id);
extern void aramInit(u32 length);         /* aramInit */
extern void aramExit(void);               /* aramExit */
extern void* aramStoreData(void* src, u32 len); /* aramStoreData */
extern void aramSetUploadCallback(ARAMUploadCallback callback, u32 chunckSize);
extern void aramRemoveData(void* aram, u32 len); /* aramRemoveData */
extern void aramSyncTransferQueue(void);               /* aramSyncTransferQueue */
extern void salInitHRTFBuffer(void);

void hwSetTimeOffset(u8 offset);
void hwSetSRCType(u32 v, u8 salSRCType);
void hwSetPolyPhaseFilter(u32 v, u8 salCoefSel);
void hwSetITDMode(u32 v, u8 mode);

static volatile const u16 itdOffTab[128] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,
    9,  9,  9,  10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 17,
    17, 17, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25,
    25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30,
    31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
};

// SND_PROFILE_INFO prof;


void snd_handle_irq(void) {
  u8 r; // r31
  u8 i; // r30
  u8 v; // r29
  if (lbl_8047AF18 == 0) {
    return;
  }

  streamCorrectLoops();
  hwIRQEnterCritical();
  // sndProfStartPCM(&prof.dspCtrl);
  salCtrlDsp(salAiGetDest());
  // sndProfStopPMC(&prof.dspCtrl);
  hwIRQLeaveCritical();
  hwIRQEnterCritical();
  // sndProfStartPCM(&prof.auxProcessing);
  salHandleAuxProcessing();
  // sndProfStopPMC(&prof.auxProcessing);
  hwIRQLeaveCritical();
  hwIRQEnterCritical();
  lbl_8047B05F ^= 1;
  lbl_8047B05E = (lbl_8047B05E + 1) % 3;

  for (v = 0; v < lbl_8047B05D; ++v) {
    for (i = 0; i < 5; ++i) {
      lbl_8047B024[v].changed[i] = 0;
    }
  }

  // sndProfStartPMC(&prof.sequencer);
  // sndProfPausePMC(&prof.sequencer);
  // sndProfStartPMC(&prof.synthesizer);
  // sndProfPausePMC(&prof.synthesizer);
  hwIRQLeaveCritical();
  for (r = 0; r < 5; ++r) {
    hwIRQEnterCritical();
    hwSetTimeOffset(r);
    // sndProfStartPMC(&prof.sequencer);
    seqHandle(256);
    // sndProfPausePMC(&prof.sequencer);
    // sndProfStartPMC(&prof.synthesizer);
    synthHandle(256);
    // sndProfPausePMC(&prof.synthesizer);
    hwIRQLeaveCritical();
  }

  // sndProfStopPMC(&prof.sequencer);
  // sndProfStopPMC(&prof.synthesizer);
  hwIRQEnterCritical();
  hwSetTimeOffset(0);
  // sndProfStartPMC(&prof.emitters);
  s3dHandle();
  // sndProfStopPMC(&prof.emitters);
  hwIRQLeaveCritical();
  hwIRQEnterCritical();
  // sndProfStartPMC(&prof.streaming);
  streamHandle();
  // sndProfStopPMC(&prof.streaming);
  hwIRQLeaveCritical();
  hwIRQEnterCritical();
  vsSampleUpdates();
  hwIRQLeaveCritical();
  // sndProfUpdateMisc(&prof);

  // if (sndProfUserCallback) {
  //   sndProfUserCallback(&prof);
  // }
}

s32 hwInit(u32* frq, u16 numVoices, u16 numStudios, u32 flags) {
  hwInitIrq();
  lbl_8047B05F = 0;
  lbl_8047B05E = 0;
  lbl_8047B028 = 0;
  if (salInitAi(snd_handle_irq, flags, frq) != 0) {
    if (salInitDspCtrl(numVoices, numStudios, (flags & 1) != 0) != 0) {
      if (salInitDsp(flags)) {
        hwEnableIrq();
        salStartAi();
        return 0;
      }
    } else {
    }
  } else {
  }
  return -1;
}

void hwExit() {
  hwDisableIrq();
  salExitDsp();
  salExitDspCtrl();
  salExitAi();
  hwEnableIrq();
  hwExitIrq();
}

void hwSetTimeOffset(u8 offset) { lbl_8047B050 = offset; }

u8 hwGetTimeOffset() { return lbl_8047B050; }

u32 hwIsActive(u32 v) { return lbl_8047B024[v].state != 0; }

void hwSetMesgCallback(SND_MESSAGE_CALLBACK callback) { lbl_8047B028 = (u32)callback; } /* hwSetMesgCallback */

void hwSetPriority(u32 v, u32 prio) { lbl_8047B024[v].prio = prio; }

void hwInitSamplePlayback(u32 v, u16 smpID, void* newsmp, u32 set_defadsr, u32 prio,
                          u32 callbackUserValue, u32 setSRC, u8 itdMode) {
  unsigned char i;  // r30
  unsigned long bf; // r29
  bf = 0;
  for (i = 0; i <= lbl_8047B050; ++i) {
    bf |= lbl_8047B024[v].changed[i] & 0x20;
    lbl_8047B024[v].changed[i] = 0;
  }

  lbl_8047B024[v].changed[0] = bf;
  lbl_8047B024[v].prio = prio;
  lbl_8047B024[v].mesgCallBackUserValue = callbackUserValue;
  lbl_8047B024[v].flags = 0;
  lbl_8047B024[v].smp_id = smpID;
  lbl_8047B024[v].smp_info = *(SAMPLE_INFO*)newsmp;

  if (set_defadsr != 0) {
    lbl_8047B024[v].adsr.mode = 0;
    lbl_8047B024[v].adsr.data.dls.aTime = 0;
    lbl_8047B024[v].adsr.data.dls.dTime = 0;
    lbl_8047B024[v].adsr.data.dls.sLevel = 0x7FFF;
    lbl_8047B024[v].adsr.data.dls.rTime = 0;
  }

  lbl_8047B024[v].lastUpdate.pitch = 0xff;
  lbl_8047B024[v].lastUpdate.vol = 0xff;
  lbl_8047B024[v].lastUpdate.volA = 0xff;
  lbl_8047B024[v].lastUpdate.volB = 0xff;

  if (setSRC != 0) {
    hwSetSRCType(v, 0);
    hwSetPolyPhaseFilter(v, 1);
  }

  hwSetITDMode(v, itdMode);
}

void hwBreak(s32 vid) {
  if (lbl_8047B024[vid].state == 1 && lbl_8047B050 == 0) {
    lbl_8047B024[vid].startupBreak = 1;
  }

  lbl_8047B024[vid].changed[lbl_8047B050] |= 0x20;
}

void hwSetADSR(u32 v, void* _adsr, u8 mode) {
  u32 sl;                              // r29
  ADSR_INFO* adsr = (ADSR_INFO*)_adsr; // r30

  switch (mode) {
  case 0: {
    lbl_8047B024[v].adsr.mode = 0;
    lbl_8047B024[v].adsr.data.linear.aTime = adsr->data.linear.atime;
    lbl_8047B024[v].adsr.data.linear.dTime = adsr->data.linear.dtime;
    sl = adsr->data.linear.slevel << 3;
    if (sl > 0x7fff) {
      sl = 0x7fff;
    }

    lbl_8047B024[v].adsr.data.linear.sLevel = sl;
    lbl_8047B024[v].adsr.data.linear.rTime = adsr->data.linear.rtime;
    break;
  }
  case 1:
  case 2:
    lbl_8047B024[v].adsr.mode = 1;
    lbl_8047B024[v].adsr.data.dls.aMode = 0;
    if (mode == 1) {
      lbl_8047B024[v].adsr.data.dls.aTime = adsrConvertTimeCents(adsr->data.dls.atime) & 0xFFFF;
      lbl_8047B024[v].adsr.data.dls.dTime = adsrConvertTimeCents(adsr->data.dls.dtime) & 0xFFFF;

      sl = adsr->data.dls.slevel >> 2;
      if (sl > 0x3ff) {
        sl = 0x3ff;
      }

      lbl_8047B024[v].adsr.data.dls.sLevel = 193 - lbl_8036944C[sl];
    } else {
      lbl_8047B024[v].adsr.data.dls.aTime = adsr->data.dls.atime & 0xFFFF;
      lbl_8047B024[v].adsr.data.dls.dTime = adsr->data.dls.dtime & 0xFFFF;
      lbl_8047B024[v].adsr.data.dls.sLevel = adsr->data.dls.slevel;
    }

    lbl_8047B024[v].adsr.data.dls.rTime = adsr->data.dls.rtime;
  }

  lbl_8047B024[v].changed[0] |= 0x10;
}

void fn_80162858(u32 voice, void* addr, u32 len) {
  lbl_8047B024[voice].vSampleInfo.loopBufferAddr = addr;
  lbl_8047B024[voice].vSampleInfo.loopBufferLength = len;
}

u32 hwGetVirtualSampleState(u32 voice) { return lbl_8047B024[voice].vSampleInfo.inLoopBuffer; }

u8 fn_8016288C(u32 voice) { return lbl_8047B024[voice].smp_info.compType; }

u16 fn_801628A0(u32 voice) { return lbl_8047B024[voice].smp_id; }


void hwSetStreamLoopPS(u32 voice, u8 ps) { lbl_8047B024[voice].streamLoopPS = ps; }

void hwStart(u32 v, u8 studio) {
  lbl_8047B024[v].singleOffset = lbl_8047B050;
  salActivateVoice(&lbl_8047B024[v], studio);
}

void hwKeyOff(u32 v) { lbl_8047B024[v].changed[lbl_8047B050] |= 0x40; }

void hwSetPitch(u32 v, u16 speed) {
  DSPvoice* dsp_vptr = &lbl_8047B024[v];

  if (speed >= 0x4000) {
    speed = 0x3fff;
  }

  if (dsp_vptr->lastUpdate.pitch != 0xff &&
      dsp_vptr->pitch[dsp_vptr->lastUpdate.pitch] == speed * 16) {
    return;
  }

  dsp_vptr->pitch[lbl_8047B050] = speed * 16;
  dsp_vptr->changed[lbl_8047B050] |= 8;
  dsp_vptr->lastUpdate.pitch = lbl_8047B050;
}

void hwSetSRCType(u32 v, u8 salSRCType) {
  static u16 dspSRCType[3] = {0, 1, 2};
  struct DSPvoice* dsp_vptr = &lbl_8047B024[v];
  dsp_vptr->srcTypeSelect = dspSRCType[salSRCType];
  dsp_vptr->changed[0] |= 0x100;
}

void hwSetPolyPhaseFilter(u32 v, u8 salCoefSel) {
  static u16 dspCoefSel[3] = {0, 1, 2};
  DSPvoice* dsp_vptr = &lbl_8047B024[v];
  dsp_vptr->srcCoefSelect = dspCoefSel[salCoefSel];
  dsp_vptr->changed[0] |= 0x80;
}


static void SetupITD(DSPvoice* dsp_vptr, u8 pan) {
  dsp_vptr->itdShiftL = itdOffTab[pan];
  dsp_vptr->itdShiftR = 32 - itdOffTab[pan];
  dsp_vptr->changed[0] |= 0x200;
}

void hwSetITDMode(u32 v, u8 mode) {
  if (!mode) {
    lbl_8047B024[v].flags |= 0x80000000;
    lbl_8047B024[v].itdShiftL = 16;
    lbl_8047B024[v].itdShiftR = 16;
    return;
  }
  lbl_8047B024[v].flags &= ~0x80000000;
}

#define hwGetITDMode(dsp_vptr) (dsp_vptr->flags & 0x80000000)

void hwSetVolume(u32 v, u8 table, float vol, u32 pan, u32 span, float auxa, float auxb) {
  SAL_VOLINFO vi;                    // r1+0x24
  u16 il;                            // r30
  u16 ir;                            // r29
  u16 is;                            // r28
  DSPvoice* dsp_vptr = &lbl_8047B024[v]; // r31
  if (vol >= 1.f) {
    vol = 1.f;
  }

  if (auxa >= 1.f) {
    auxa = 1.f;
  }

  if (auxb >= 1.f) {
    auxb = 1.f;
  }

  salCalcVolume(table, &vi, vol, pan, span, auxa, auxb, hwGetITDMode(dsp_vptr) != 0,
                lbl_80447E60[dsp_vptr->studio].type == SND_STUDIO_TYPE_DPL2);

  il = 32767.f * vi.volL;
  ir = 32767.f * vi.volR;
  is = 32767.f * vi.volS;

  if (dsp_vptr->lastUpdate.vol == 0xff || dsp_vptr->volL != il || dsp_vptr->volR != ir ||
      dsp_vptr->volS != is) {
    dsp_vptr->volL = il;
    dsp_vptr->volR = ir;
    dsp_vptr->volS = is;
    dsp_vptr->changed[0] |= 1;
    dsp_vptr->lastUpdate.vol = 0;
  }

  il = 32767.f * vi.volAuxAL;
  ir = 32767.f * vi.volAuxAR;
  is = 32767.f * vi.volAuxAS;

  if (dsp_vptr->lastUpdate.volA == 0xff || dsp_vptr->volLa != il || dsp_vptr->volRa != ir ||
      dsp_vptr->volSa != is) {
    dsp_vptr->volLa = il;
    dsp_vptr->volRa = ir;
    dsp_vptr->volSa = is;
    dsp_vptr->changed[0] |= 2;
    dsp_vptr->lastUpdate.volA = 0;
  }

  il = 32767.f * vi.volAuxBL;
  ir = 32767.f * vi.volAuxBR;
  is = 32767.f * vi.volAuxBS;

  if (dsp_vptr->lastUpdate.volB == 0xff || dsp_vptr->volLb != il || dsp_vptr->volRb != ir ||
      dsp_vptr->volSb != is) {
    dsp_vptr->volLb = il;
    dsp_vptr->volRb = ir;
    dsp_vptr->volSb = is;
    dsp_vptr->changed[0] |= 4;
    dsp_vptr->lastUpdate.volB = 0;
  }

  if (hwGetITDMode(dsp_vptr)) {
    SetupITD(dsp_vptr, (pan >> 16));
  }
}

void hwOff(s32 vid) { salDeactivateVoice(&lbl_8047B024[vid]); }

void hwSetAUXProcessingCallbacks(u8 studio, SND_AUX_CALLBACK auxA, void* userA,
                                 SND_AUX_CALLBACK auxB, void* userB) {
  lbl_80447E60[studio].auxAHandler = auxA;
  lbl_80447E60[studio].auxAUser = userA;
  lbl_80447E60[studio].auxBHandler = auxB;
  lbl_80447E60[studio].auxBUser = userB;
}

void hwActivateStudio(u8 studio, u32 isMaster, SND_STUDIO_TYPE type) {
  salActivateStudio(studio, isMaster, type);
}

void hwDeactivateStudio(u8 studio) { salDeactivateStudio(studio); }

void hwChangeStudioMix(u8 studio, u32 isMaster) { lbl_80447E60[studio].isMaster = isMaster; }

u32 hwIsStudioActive(u8 studio) { return lbl_80447E60[studio].state == 1; }

u32 hwAddInput(u8 studio, SND_STUDIO_INPUT* in_desc) {
  return salAddStudioInput(&lbl_80447E60[studio], in_desc);
}

u32 hwRemoveInput(u8 studio, SND_STUDIO_INPUT* in_desc) {
  return salRemoveStudioInput(&lbl_80447E60[studio], in_desc);
}

void hwChangeStudio(u32 v, u8 studio) { salReconnectVoice(&lbl_8047B024[v], studio); }

u32 hwGetPos(u32 v) {
  unsigned long pos; // r31
  unsigned long off; // r30
  if (lbl_8047B024[v].state != 2) {
    return 0;
  }

  switch (lbl_8047B024[v].smp_info.compType) {
  case 0:
  case 1:
  case 4:
  case 5:
    pos = ((lbl_8047B024[v].currentAddr - (u32)lbl_8047B024[v].smp_info.addr * 2) / 16) * 14;
    off = lbl_8047B024[v].currentAddr & 0xf;
    if (off >= 2) {
      pos += off - 2;
    }
    break;
  case 3:
    pos = lbl_8047B024[v].currentAddr - (u32)lbl_8047B024[v].smp_info.addr;
    break;
  case 2:
    pos = lbl_8047B024[v].currentAddr - ((u32)lbl_8047B024[v].smp_info.addr / 2);
    break;
  }

  return pos;
}

void hwFlushStream(void* base, u32 offset, u32 bytes, u8 hwStreamHandle, void (*callback)(u32),
                   u32 user) {
  u32 aram; // r28
  u32 mram; // r29
  u32 len;
  aram = aramGetStreamBufferAddress(hwStreamHandle, &len);
  bytes += (offset & 31);
  offset &= ~31;
  bytes = (bytes + 31) & ~31;
  mram = (u32)base + offset;
  DCStoreRange((void*)mram, bytes);
  // TODO: Platform specific audio memory handling
  aramUploadData((void*)mram, aram + offset, bytes, 1, callback, user);
}

void hwPrepareStreamBuffer() {}
u8 hwInitStream(u32 len) {
  // TODO: Platform specific audio memory handling
  return aramAllocateStreamBuffer(len);
}

void hwExitStream(u8 id) {
  // TODO: Platform specific audio memory handling
  aramFreeStreamBuffer(id);
}

void* hwGetStreamPlayBuffer(u8 hwStreamHandle) {
  // TODO: Platform specific audio memory handling
  return (void*)aramGetStreamBufferAddress(hwStreamHandle, NULL);
}


void* hwTransAddr(void* samples) { return samples; }

u32 hwFrq2Pitch(u32 frq) { return (frq * 4096.f) / lbl_80434C50.mixFrq; }

void hwInitSampleMem(u32 baseAddr, u32 length) {
  // TODO: Platform specific audio memory handling
  aramInit(length);
}

void hwExitSampleMem() { aramExit(); }

static u32 convert_length(u32 len, u8 type) {
  switch (type) {
  case 0:
  case 1:
  case 4:
  case 5:
    len = (((u32)((len + 13) / 14))) * 8;
    break;
  case 2:
    len *= 2;
    break;
  }
  return len;
}

// TODO: Platform specific audio memory handling
void hwSaveSample(void* header, void* data
) {
  u32 len = ((u32*)*((u32*)header))[1] & 0xFFFFFF;
  u8 type = ((u32*)*((u32*)header))[1] >> 0x18;
  len = convert_length(len, type);
  *((u32*)data) = (u32)aramStoreData((void*)*((u32*)data), len
  );
}

// TODO: Platform specific audio memory handling

// TODO: Platform specific audio memory handling
void hwSetSaveSampleCallback(ARAMUploadCallback callback, unsigned long chunckSize) {
  aramSetUploadCallback(callback, chunckSize);
}

// TODO: Platform specific audio memory handling
void hwRemoveSample(void* header, void* data
) {
  u8 type = (((u32*)header))[1] >> 0x18;
  u32 len = convert_length((((u32*)header))[1] & 0xFFFFFF, type);
  aramRemoveData(data, len);
}

// TODO: Platform specific audio memory handling
void hwSyncSampleMem() { aramSyncTransferQueue(); }

void hwFrameDone() {}

void sndSetHooks(SND_HOOKS* hooks) {
  lbl_8047B054 = *hooks;
}

void hwEnableHRTF() {
  if (lbl_8047B014 != FALSE) {
    return;
  }
  lbl_8047B014 = TRUE;
  salInitHRTFBuffer();
}
void hwDisableHRTF() { lbl_8047B014 = FALSE; }


u32 hwGetVirtualSampleID(u32 v) {
  if (lbl_8047B024[v].state == 0) {
    return 0xFFFFFFFF;
  }

  return lbl_8047B024[v].virtualSampleID;
}

u32 hwVoiceInStartup(u32 v) { return lbl_8047B024[v].state == 1; }
