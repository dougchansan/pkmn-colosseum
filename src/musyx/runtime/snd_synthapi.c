/**
 * @file snd_synthapi.c
 * @brief MusyX runtime synth API (musyx/runtime/snd_synthapi.c),
 * 0x8014D75C - 0x8014DDD8.
 *
 * Split out of the misnamed people_field.c unit (2026-07-02). Reference:
 * AxioDL/musyx `musyx/runtime/snd_synthapi.c`. Boundary evidence:
 * synthDeactivateStudio is synthDeactivateStudio (simindex seq=1.0 vs the MP4
 * matched copy); the two 0x20 wrappers synthAddStudioInput/synthRemoveStudioInput are
 * snd_synthapi tail one-call wrappers (stream.c has no functions before
 * streamInit, which is confirmed at 0x8014DDD8 at seq=1.0).
 * All functions asm-only until matched.
 */
#include "dolphin/types.h"

/* Local mirrors of synth.c's file-local SYNTH_VOICE/SynthInfo layouts
 * (opaque padding for fields this TU doesn't touch; sizes/offsets for
 * id/vidList/cFlags/studio/voiceNum verified against the target asm). */
typedef struct VID_LIST_HDR {
  u8 pad_00[8];
  u32 vid; /* 0x8 */
} VID_LIST_HDR;

#pragma pack(4)
typedef struct SYNTH_VOICE {
  u8 pad_000[0xF4];
  u32 id;               /* 0xF4 */
  VID_LIST_HDR* vidList; /* 0xF8 */
  u8 pad_0FC[0x114 - 0xFC];
  u64 cFlags; /* 0x114 */
  u8 pad_11C[0x11F - 0x11C];
  u8 studio; /* 0x11F */
  u8 pad_120[0x404 - 0x120];
} SYNTH_VOICE; /* size 0x404 */
#pragma pack()

typedef struct SynthInfo {
  u8 pad_000[0x210];
  u8 voiceNum; /* 0x210 */
} SynthInfo;

/* ===== Cross-TU MusyX runtime functions (defined in synth.c) ===== */
extern u32 synthFXSetCtrl(u32 vid, u8 ctrl, u8 value);   /* synthFXSetCtrl */
extern u32 synthSendKeyOff(u32 id);
extern u32 synthFXStart(u16 fid, u8 vol, u8 pan, u8 studio, u32 itd); /* synthFXStart */
extern u32 vidGetInternalId(u32 id);
extern void synthVolume(u8 volume, u16 time, u8 vGroup, u8 seqMode, u32 seqId);
extern u32 hwIsActive(u32 voice); /* hwIsActive */
extern void voiceKillSound(u32 id);

/* ===== Cross-TU MusyX runtime globals (defined in synth.c) ===== */
extern u8 lbl_80434C50[]; /* synthInfo */
extern SYNTH_VOICE* lbl_8047AF48;                  /* synthVoice */
extern u8 lbl_8047AF1C[8];                         /* synthAuxBMIDISet */
extern u8 lbl_8047AF24[8];                         /* synthAuxBMIDI */
extern u8 lbl_8047AF2C[8];                         /* synthAuxAMIDISet */
extern u8 lbl_8047AF34[8];                         /* synthAuxAMIDI */
extern u32 lbl_8047AF44;                           /* synthFlags */
extern void* lbl_8047AF4C;                         /* synthMessageCallback */
extern void* lbl_80435624[8];                      /* synthAuxAUser */
extern void (*lbl_80435644[8])(u32, void*, void*); /* synthAuxACallback */
extern void* lbl_80435664[8];                      /* synthAuxBUser */
extern void (*lbl_80435684[8])(u32, void*, void*); /* synthAuxBCallback */
extern u8 lbl_804356A4[16];                        /* synthITDDefault: {music,sfx} byte pairs */

/* ===== Cross-TU MusyX seq functions (defined in seq.c) ===== */
extern u32 seqGetPrivateId(u32 seqId);

/* ===== Cross-TU low-level hardware helpers ===== */
extern void hwDisableIrq(void);
extern void hwEnableIrq(void);
extern void hwSetAUXProcessingCallbacks(u8 studio, void* auxA, void* userA, void* auxB, void* userB);
extern void streamOutputModeChanged(void);
extern void hwDisableHRTF(void); /* hwDisableHRTF */
extern void hwActivateStudio(u8 studio, u32 isMaster, u32 type); /* hwActivateStudio */
extern void hwOff(u32 index);                         /* hwOff */
extern void hwDeactivateStudio(u8 studio);                         /* hwDeactivateStudio */
extern void hwAddInput(u8 studio, u32 arg1);
extern void hwRemoveInput(u8 studio, u32 arg1);

u32 sndFXCtrl(u32 vid, u8 ctrl, u8 value) {
  u32 ret;
  hwDisableIrq();
  ret = synthFXSetCtrl(vid, ctrl, value);
  hwEnableIrq();
  return ret;
}

u32 sndFXKeyOff(u32 vid) {
  u32 ret;
  hwDisableIrq();
  ret = synthSendKeyOff(vid);
  hwEnableIrq();
  return ret;
}

u32 sndFXStartEx(u16 fid, u8 vol, u8 pan, u8 studio) {
  u32 v;
  hwDisableIrq();
  v = synthFXStart(fid, vol, pan, studio, lbl_804356A4[studio * 2 + 1]);
  hwEnableIrq();
  return v;
}

u32 sndFXCheck(u32 vid) { return vidGetInternalId(vid) != -1 ? vid : -1; }

void sndSetReceiveMessageCallback(void* callback) { lbl_8047AF4C = callback; }

void sndVolume(u8 volume, u16 time, u8 volgroup) {
  hwDisableIrq();
  synthVolume(volume, time, volgroup, 0, -1);
  hwEnableIrq();
}

void sndMasterVolume(u8 volume, u16 time, u8 music, u8 fx) {
  hwDisableIrq();
  if (music != 0)
    synthVolume(volume, time, 0x15, 0, -1);

  if (fx != 0)
    synthVolume(volume, time, 0x16, 0, -1);
  hwEnableIrq();
}

void sndOutputMode(u32 output) {
  u32 i;
  u32 oldFlags;
  oldFlags = lbl_8047AF44;

  switch (output) {
  case 0: /* SND_OUTPUTMODE_MONO */
    lbl_8047AF44 |= 1;
    lbl_8047AF44 &= ~2;
    hwDisableHRTF();
    break;
  case 1: /* SND_OUTPUTMODE_STEREO */
    lbl_8047AF44 &= ~1;
    lbl_8047AF44 &= ~2;
    hwDisableHRTF();
    break;
  case 2: /* SND_OUTPUTMODE_SURROUND */
    lbl_8047AF44 &= ~1;
    lbl_8047AF44 |= 2;
    hwDisableHRTF();
    break;
  default:
    break;
  }

  if (oldFlags == lbl_8047AF44) {
    return;
  }

  for (i = 0; i < ((SynthInfo*)lbl_80434C50)->voiceNum; ++i) {
    lbl_8047AF48[i].cFlags |= 0x0000200000000000ULL;
  }
  streamOutputModeChanged();
}

// clang-format off
void sndSetAuxProcessingCallbacks(u8 studio,
                                  void (*auxA)(u32, void*, void*), void* userA, u8 midiA, u32 seqIDA,
                                  void (*auxB)(u32, void*, void*), void* userB, u8 midiB, u32 seqIDB) {
  // clang-format on
  hwDisableIrq();
  if (auxA != NULL) {

    if ((lbl_8047AF34[studio] = midiA) != 0xFF) {
      lbl_8047AF2C[studio] = seqGetPrivateId(seqIDA);
      lbl_80435644[studio] = auxA;
      lbl_80435624[studio] = userA;
    }
  } else {
    lbl_80435644[studio] = NULL;
    lbl_8047AF34[studio] = 0xff;
  }

  if (auxB != NULL) {

    if ((lbl_8047AF24[studio] = midiB) != 0xFF) {
      lbl_8047AF1C[studio] = seqGetPrivateId(seqIDB);
      lbl_80435684[studio] = auxB;
      lbl_80435664[studio] = userB;
    }
  } else {
    lbl_80435684[studio] = NULL;
    lbl_8047AF24[studio] = 0xff;
  }

  hwSetAUXProcessingCallbacks(studio, auxA, userA, auxB, userB);
  hwEnableIrq();
}

void synthActivateStudio(u8 studio, u32 isMaster, u32 type) {
  hwDisableIrq();
  lbl_80435644[studio] = NULL;
  lbl_80435684[studio] = NULL;
  lbl_8047AF34[studio] = 0xFF;
  lbl_8047AF24[studio] = 0xFF;
  lbl_804356A4[studio * 2 + 1] = 0;
  lbl_804356A4[studio * 2] = 0;
  hwActivateStudio(studio, isMaster, type);
  hwEnableIrq();
}

void synthDeactivateStudio(u8 studio) {
  u32 i;

  for (i = 0; i < ((SynthInfo*)lbl_80434C50)->voiceNum; ++i) {

    if (studio == lbl_8047AF48[i].studio) {
      if (lbl_8047AF48[i].id != 0xFFFFFFFF) {
        voiceKillSound(lbl_8047AF48[i].vidList->vid);
      } else if (hwIsActive(i)) {
        hwOff(i);
      }
    }
  }

  hwDisableIrq();

  lbl_80435644[studio] = 0;
  lbl_80435684[studio] = 0;
  lbl_8047AF34[studio] = 0xFF;
  lbl_8047AF24[studio] = 0xFF;

  hwEnableIrq();

  hwDeactivateStudio(studio);
}

void synthAddStudioInput(u8 studio, u32 arg1) { hwAddInput(studio, arg1); }

void synthRemoveStudioInput(u8 studio, u32 arg1) { hwRemoveInput(studio, arg1); }
