/**
 * @file synth.c
 * @brief MusyX synthesizer core, 0x8014A23C - 0x8014D598.
 *
 * The reference MusyX runtime's synth.c (AxioDL/musyx) in its 2.0.0 form,
 * built as one translation unit that owns its data. MWCC lays the data out
 * exactly as retail from the reference's declaration order:
 *   .bss    0x80434A10 - 0x80435FF8  synthTicksPerSecond, synthInfo,
 *           synthMasterFader, synthTrackVolume, synthJobTable, the aux
 *           callback/user tables, synthITDDefault, synthGlobalVariable,
 *           inpAuxB, inpAuxA
 *   .sbss   0x8047AF18 - 0x8047AF60  sndActive ... synthRealTime
 *   .sdata2 0x8047D370 - 0x8047D3B0  the literal pool
 * Retail reaches the .bss objects as offsets from one base
 * (synthInit: synthInfo at +0x240, synthMasterFader at +0x454, ...), which
 * is how MWCC addresses a TU's own data, so the TU must define them. The
 * globals other TUs use keep their symbol-map labels (reference names in
 * the comments); the reference statics keep their names.
 *
 * The static helpers retail expands (apply_portamento, check_portamento,
 * unblockAllAllocatedVoices, convert_cents, UpdateTimeMIDICtrl,
 * EventHandler, synthInitJobQueue, HandleJobQueue, HandleVoices,
 * HandleFaderTermination, SetupFader, synthFXVolume) are inlined as in
 * retail; functions the game never calls (synthGetVolume,
 * synthPauseVolume) are dead-stripped. Their out-of-line copies are
 * unreferenced and stripped at link.
 *
 * Built with -fp_contract off: retail keeps multiplies and adds separate
 * (ZeroOffsetHandler's volume/pan ramps, synthHandle's fader
 * interpolation); ZeroOffsetHandler is 95.6% with contraction on, and all
 * 26 functions are exact with that one unit-wide setting.
 */
#include "dolphin/types.h"
#include "musyx/runtime/synth_voice.h"

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

typedef struct SYNTH_JOBTAB {
    SYNTH_QUEUE* lowPrecision;
    SYNTH_QUEUE* event;
    SYNTH_QUEUE* zeroOffset;
} SYNTH_JOBTAB;

typedef enum {
    SYNTH_JOBTYPE_LOW = 0,
    SYNTH_JOBTYPE_ZERO = 1,
    SYNTH_JOBTYPE_EVENT = 2,
} SYNTH_JOBTYPE;

typedef struct LAYER {
    u16 id;
    u8 keyLow;
    u8 keyHigh;
    s8 transpose;
    u8 volume;
    s16 prioOffset;
    u8 panning;
    u8 reserved[3];
} LAYER;

typedef struct KEYMAP {
    u16 id;
    s8 transpose;
    u8 panning;
    s16 prioOffset;
    u8 reserved[2];
} KEYMAP;

typedef struct FX_TAB {
    u16 id;
    u16 macro;
    u8 maxVoices;
    u8 priority;
    u8 volume;
    u8 panning;
    u8 key;
    u8 vGroup;
} FX_TAB;

typedef struct SYNTHMasterFader {
    f32 volume;
    f32 target;
    f32 start;
    f32 time;
    f32 deltaTime;
    f32 pauseVol;
    f32 pauseTarget;
    f32 pauseStart;
    f32 pauseTime;
    f32 pauseDeltaTime;
    u32 seqId;
    u8 seqMode;
    u8 type;
} SYNTHMasterFader;

typedef struct synthITDInfo {
    u8 music;
    u8 sfx;
} synthITDInfo;

#define SND_AUX_NUMPARAMETERS 4
#define SND_AUX_REASON_PARAMETERUPDATE 1

typedef struct SND_AUX_INFO {
    union SND_AUX_DATA {
        struct SND_AUX_BUFFERUPDATE {
            s32* left;
            s32* right;
            s32* surround;
        } bufferUpdate;
        struct SND_AUX_PARAMETERUPDATE {
            u16 para[SND_AUX_NUMPARAMETERS];
        } parameterUpdate;
    } data;
} SND_AUX_INFO;

typedef void (*SND_AUX_CALLBACK)(u8 reason, SND_AUX_INFO* info, void* user);

#define SND_ID_ERROR 0xFFFFFFFF
#define SND_USERMUSIC_VOLGROUPS 0xFA
#define SND_USERFX_VOLGROUPS 0xFB
#define SND_USERALL_VOLGROUPS 0xFC
#define SND_MUSIC_VOLGROUPS 0xFD
#define SND_FX_VOLGROUPS 0xFE
#define SND_ALL_VOLGROUPS 0xFF
#define SND_MIDICTRL_VOLUME 0x07
#define SND_MIDICTRL_PANNING 0x0A
#define SND_MIDICTRL_REVERB 0x5B
#define SND_MIDICTRL_PITCHBEND 0x80
#define SND_MIDICTRL_DOPPLER 0x84
#define SND_MIDICTRL_PORTAMENTO 0x41
#define CLAMP(value, min, max) ((value) > (max) ? (max) : (value) < (min) ? (min) : (value))
#define CLAMP_INV(value, min, max) ((value) < (min) ? (min) : (value) > (max) ? (max) : (value))

/* The TU's own data. Other TUs reach the globals through their
 * symbol-map labels, so those keep the labels (reference names in the
 * comments); the two reference statics keep their names. */
static u32 synthTicksPerSecond[9][16];
static SYNTH_JOBTAB synthJobTable[32];
CTRL_DEST lbl_80435B74[8][4];                /* lbl_80435B74 */
CTRL_DEST lbl_804356F4[8][4];                /* lbl_804356F4 */
s32 lbl_804356B4[16];                        /* lbl_804356B4 */
synthITDInfo lbl_804356A4[8];                /* lbl_804356A4 */
void* lbl_80435664[8];                       /* lbl_80435664 */
SND_AUX_CALLBACK lbl_80435684[8];            /* lbl_80435684 */
void* lbl_80435624[8];                       /* lbl_80435624 */
SND_AUX_CALLBACK lbl_80435644[8];            /* lbl_80435644 */
u8 lbl_80435464[64];                         /* lbl_80435464 */
SYNTHMasterFader lbl_80434E64[32];           /* lbl_80434E64 */
SynthInfo lbl_80434C50;                      /* lbl_80434C50 */

u8 lbl_8047AF18 = 0;                         /* lbl_8047AF18 */
static u8 synthJobTableIndex = 0;
u64 lbl_8047AF58;                            /* lbl_8047AF58 */
u8 lbl_8047AF50;                             /* lbl_8047AF50 */
void* lbl_8047AF4C;                          /* lbl_8047AF4C */
SYNTH_VOICE* lbl_8047AF48;                   /* lbl_8047AF48 */
u32 lbl_8047AF44;                            /* lbl_8047AF44 */
u32 lbl_8047AF40;                            /* lbl_8047AF40 */
u32 lbl_8047AF3C;                            /* lbl_8047AF3C */
u8 lbl_8047AF34[8];                          /* lbl_8047AF34 */
u8 lbl_8047AF2C[8];                          /* lbl_8047AF2C */
u8 lbl_8047AF24[8];                          /* lbl_8047AF24 */
u8 lbl_8047AF1C[8];                          /* lbl_8047AF1C */

extern void* memset(void* dst, int val, u32 size);
extern void vidRemoveVoiceReferences(SYNTH_VOICE* svoice);            /* vidRemoveVoiceReferences */
extern u32 vidMakeNew(SYNTH_VOICE* svoice, u32 isMaster); /* vidMakeNew */
extern u32 vidMakeRoot(SYNTH_VOICE* svoice);             /* vidMakeRoot */
extern u32 vidGetInternalId(u32 vid);
extern void vidInit(void);
extern void voiceSetLastStarted(SYNTH_VOICE* svoice);
extern void voiceInitLastStarted(void);
extern void voiceKill(u32 vi);
extern void synthInitAllocationAids(void);
extern void inpSetMidiLastNote(u8 midi, u8 midiSet, u8 key);
extern u16 inpGetMidiCtrl(u8 ctrl, u8 channel, u8 set);
extern void inpSetMidiCtrl(u8 ctrl, u8 channel, u8 set, u8 value); /* inpSetMidiCtrl */
extern void inpSetMidiCtrl14(u8 ctrl, u8 channel, u8 set, u16 value);
extern void inpFXCopyCtrl(u8 ctrl, SYNTH_VOICE* dvoice, SYNTH_VOICE* svoice);
extern u16 inpGetVolume(SYNTH_VOICE* svoice);
extern u16 inpGetPanning(SYNTH_VOICE* svoice);
extern u16 inpGetSurroundPanning(SYNTH_VOICE* svoice);
extern u16 inpGetPitchBend(SYNTH_VOICE* svoice);
extern u16 inpGetDoppler(SYNTH_VOICE* svoice);
extern u16 inpGetModulation(SYNTH_VOICE* svoice);
extern u16 inpGetPedal(SYNTH_VOICE* svoice);
extern u16 inpGetPreAuxA(SYNTH_VOICE* svoice);
extern u16 inpGetReverb(SYNTH_VOICE* svoice);
extern u16 inpGetPreAuxB(SYNTH_VOICE* svoice);
extern u16 inpGetPostAuxB(SYNTH_VOICE* svoice);
extern u16 inpGetTremolo(SYNTH_VOICE* svoice);
extern u16 inpGetAuxA(u8 studio, u8 index, u8 midi, u8 midiSet); /* inpGetAuxA */
extern u16 inpGetAuxB(u8 studio, u8 index, u8 midi, u8 midiSet); /* inpGetAuxB */
extern void inpInit(SYNTH_VOICE* svoice);            /* inpInit */
extern LAYER* dataGetLayer(u16 cid, u16* n);
extern KEYMAP* dataGetKeymap(u16 cid);
extern FX_TAB* dataGetFX(u16 fid);
extern u32 macStart(u16 macid, u8 priority, u8 maxVoices, u16 allocId, u8 key, u8 vol, u8 panning,
                    u8 midi, u8 midiSet, u8 section, u16 step, u16 trackid, u8 new_vid, u8 vGroup,
                    u8 studio, u32 itd);
extern void macHandle(u32 deltaTime);
extern void macInit(void);
extern void macSampleEndNotify(SYNTH_VOICE* sv);
extern void macSetExternalKeyoff(SYNTH_VOICE* sv);
extern void macSetPedalState(SYNTH_VOICE* svoice, u32 state);
extern u32 sndGetPitch(u8 key, u32 sInfo);
extern s32 sndPitchUpOne(u16 note);
extern s16 sndSin(u16 angle);
extern void sndConvertMs(u32* ms);                         /* sndConvertMs */
extern u32 adsrHandleLowPrecision(ADSR_VARS* adsr, u16* adsr_start, u16* adsr_delta);
extern u32 adsrRelease(ADSR_VARS* adsr);
extern u32 hwIsActive(u32 v);                            /* hwIsActive */
extern u8 hwGetTimeOffset(void);                              /* hwGetTimeOffset */
extern void hwSetPriority(u32 v, u32 prio);                 /* hwSetPriority */
extern void hwSetMesgCallback(u32 (*callback)(u32, u32));       /* hwSetMesgCallback */
extern void hwFrameDone(void);                            /* hwFrameDone */
extern void hwKeyOff(u32 v);
extern void hwSetPitch(u32 v, u16 speed);
extern void hwSetVolume(u32 v, u8 table, f32 vol, u32 pan, u32 span, f32 auxa, f32 auxb);
extern void hwStart(u32 v, u8 studio);
extern u32 hwGetVirtualSampleID(u32 v);
extern void* salMalloc(u32 size);                       /* salMalloc */
extern void salFree(void* ptr);                       /* salFree */
extern void vsSampleEndNotify(u32 pubID);
extern u32 vsSampleStartNotify(u8 voice);                         /* vsSampleStartNotify */
extern void seqStop(u32 seqId);
extern void seqPause(u32 seqId);
extern void seqMute(u32 seqId, u32 mask1, u32 mask2);

void synthSetBpm(u32 bpm, u8 set, u8 section)
{
    if (set == 0xFF) {
        set = 8;
    }
    synthTicksPerSecond[set][section] = ((bpm << 3) * 1536) / 240;
}

u32 synthGetTicksPerSecond(SYNTH_VOICE* svoice)
{
    return synthTicksPerSecond[svoice->midiSet == 0xFF ? 8 : svoice->midiSet][svoice->section];
}

static void synthAddJob(SYNTH_VOICE* svoice, SYNTH_JOBTYPE jobType, u32 deltaTime);

static u32 apply_portamento(SYNTH_VOICE* svoice, u32 ccents, u32 deltaTime) {
  u32 old_portCurPitch; // r31

  if ((svoice->cFlags & 0x400) != 0 && (int)((svoice->portDuration - svoice->portTime) >> 8) > 0) {

    old_portCurPitch = svoice->portCurPitch;
    svoice->portCurPitch += (int)deltaTime * ((int)(ccents - svoice->portCurPitch) >> 8) /
                            (int)((svoice->portDuration - svoice->portTime) >> 8);

    if ((old_portCurPitch < ccents && svoice->portCurPitch < ccents) ||
        (old_portCurPitch > ccents && (svoice->portCurPitch > ccents))) {
      ccents = svoice->portCurPitch;
      svoice->portTime += deltaTime;
    } else {
      svoice->portTime = svoice->portDuration;
    }
  }
  return ccents;
}

void synthInitPortamento(SYNTH_VOICE* svoice) {
  if (svoice->cFlags & 0x20000) {
    return;
  }

  if (svoice->portType == 1) {
    if (!(svoice->cFlags & 0x1000)) {
      svoice->portTime = 0;
    } else {
      svoice->portTime = svoice->portDuration;
    }
  } else {
    svoice->portTime = svoice->portDuration;
  }

  svoice->portCurPitch = svoice->lastNote << 16;
}

static u32 do_voice_portamento(u8 key, u8 midi, u8 midiSet, u32 isMaster, u32* rejected) {
  u32 i;                      // r30
  u32 vid;                    // r29
  u32 id;                     // r27
  SYNTH_VOICE* sv;            // r31
  SYNTH_VOICE* last_sv;       // r28
  u32 legatoVoiceIsStarting; // r26

  legatoVoiceIsStarting = FALSE;
  vid = SND_ID_ERROR;

  for (i = 0, sv = lbl_8047AF48; i < lbl_80434C50.voiceNum; ++i, sv++) {
    if (
        sv->block == 0 &&
        sv->id != SND_ID_ERROR && sv->midi == midi && sv->midiSet == midiSet) {
      if ((sv->cFlags & 2) != 0) {
        legatoVoiceIsStarting = TRUE;
      }
      if ((sv->cFlags & 0x10) != 0 && (sv->cFlags & 0x10000000008) != 0x8 && hwIsActive(i)) {
        if (vid == SND_ID_ERROR && (sv->cFlags & 0x20002) == 0x20002) {
          *rejected = TRUE;
          return SND_ID_ERROR;
        }
        last_sv = sv;
        sv->portCurPitch = (sv->curNote * 65536) + (sv->curDetune * 65536) / 100;
        sv->lastNote = sv->curNote;
        sv->curNote = key + ((sv->curNote & 0xff) - sv->orgNote);
        sv->orgNote = key;
        sv->curDetune = 0;
        sv->portTime = 0;
        sv->cFlags |= 0x20000;
        vidRemoveVoiceReferences(&lbl_8047AF48[i]);
        if (vid == SND_ID_ERROR) {
          sv->child = SND_ID_ERROR;
          sv->parent = SND_ID_ERROR;
          vid = vidMakeNew(&lbl_8047AF48[i], isMaster);
          id = sv->id;
        } else {
          lbl_8047AF48[id & 0xff].child = sv->id;
          sv->parent = id;
          id = sv->id;
          vidMakeNew(&lbl_8047AF48[i], FALSE);
        }
      }
    }
  }

  if (vid != SND_ID_ERROR) {
    voiceSetLastStarted(last_sv);
    inpSetMidiLastNote(last_sv->midi, last_sv->midiSet, last_sv->curNote);
    *rejected = FALSE;
  } else {
    *rejected = legatoVoiceIsStarting;
  }
  return vid;
}

static u32 check_portamento(u8 key, u8 midi, u8 midiSet, u32 newVID, u32* vid) {
  u32 rejected; // r1+0x14

  if (inpGetMidiCtrl(65 /* TODO SND_MIDICTRL_? */, midi, midiSet) > 8064) {
    *vid = do_voice_portamento(key & 0x7f, midi, midiSet, newVID, &rejected);
    return !rejected;
  }
  *vid = 0xFFFFFFFF;
  return 1;
}

static u32 StartKeymap(u16 keymapID, s16 prio, u8 maxVoices,
                       u16 allocId,
                       u8 key, u8 vol, u8 panning, u8 midi, u8 midiSet, u8 section, u16 step,
                       u16 trackid, u32 vidFlag, u8 vGroup, u8 studio, u32 itd);

static u32 StartLayer(u16 layerID, s16 prio, u8 maxVoices,
                      u16 allocId,
                      u8 key, u8 vol, u8 panning, u8 midi, u8 midiSet, u8 section, u16 step,
                      u16 trackid, u32 vidFlag, u8 vGroup, u8 studio, u32 itd) {
  u16 n;      // r1+0x38
  u32 vid;    // r26
  u32 new_id; // r1+0x34
  u32 id;     // r27
  LAYER* l;   // r31
  s32 p;      // r30
  s32 k;      // r29
  u8 v;       // r25
  u8 mKey;    // r24

  vid = SND_ID_ERROR;
  if ((l = dataGetLayer(layerID, &n)) == NULL) {
    goto end;
  }

  mKey = key & 0x7f;
  for (; n != 0; --n, l++) {
    if (l->id == 0xffff || l->keyLow > mKey || l->keyHigh < mKey) {
      continue;
    }

    k = mKey + l->transpose;
    k = CLAMP(k, 0, 127);

    if ((l->id & 0xC000) == 0) {
      if (check_portamento(k, midi, midiSet,
                           0,
                           &new_id)) {
        if (new_id != 0xFFFFFFFF) {
          goto apply_new_id;
        } else {
          goto start_new_id;
        }
      }
      continue;
    }

  start_new_id:
    if ((l->panning & 0x80) == 0) {
      p = l->panning - 0x40;
      p += panning;
      // TODO
      // p = CLAMP(p, 0, 0x7f);
      p = CLAMP_INV(p, 0, 0x7f);
    } else {
      p = 0x80;
    }

    v = (vol * l->volume) / 0x7f;
    prio += l->prioOffset;
    prio = CLAMP(prio, 0, 0xff);

    switch (l->id & 0xC000) {
    case 0:
      new_id = macStart(l->id, prio, maxVoices, allocId, k | (key & 0x80), v, p, midi, midiSet,
                        section, step, trackid, 0, vGroup, studio, itd);
      break;
    case 0x4000:
      new_id = StartKeymap(l->id, prio, maxVoices, allocId, k | (key & 0x80), v, p, midi, midiSet,
                           section, step, trackid, 0, vGroup, studio, itd);
      break;
    case 0x8000:
      new_id = StartLayer(l->id, prio, maxVoices, allocId, k | (key & 0x80), v, p, midi, midiSet,
                          section, step, trackid, 0, vGroup, studio, itd);
      break;
    }

    if (new_id != SND_ID_ERROR) {
    apply_new_id:
      if (vid == SND_ID_ERROR) {
        if (vidFlag != 0) {
          vid = vidMakeRoot(&lbl_8047AF48[new_id & 0xff]);
        } else {
          vid = new_id;
        }
      } else {
        lbl_8047AF48[id & 0xff].child = new_id;
        lbl_8047AF48[new_id & 0xff].parent = id;
      }
      id = new_id;
      while (lbl_8047AF48[id & 0xff].child != SND_ID_ERROR) {
        lbl_8047AF48[id & 0xff].block = 1;
        id = lbl_8047AF48[id & 0xff].child;
      }
      lbl_8047AF48[id & 0xff].block = 1;
    }
  }

end:
  return vid;
}

static u32 StartKeymap(u16 keymapID, s16 prio, u8 maxVoices,
                       u16 allocId,
                       u8 key, u8 vol, u8 panning, u8 midi, u8 midiSet, u8 section, u16 step,
                       u16 trackid, u32 vidFlag, u8 vGroup, u8 studio, u32 itd) {
  u8 o;           // r30
  KEYMAP* keymap; // r31
  s32 p;          // r26
  s32 k;          // r29
  u32 vid;        // r1+0x34

  if ((keymap = dataGetKeymap(keymapID)) != NULL) {
    o = key & 0x7f;
    if (keymap[o].id != 0xffff && (keymap[o].id & 0xc000) != 0x4000) {
      if ((keymap[o].panning & 0x80) == 0) {
        p = (keymap[key].panning - 0x40);
        p += panning;
        if (p < 0) {
          panning = 0;
        } else if (p > 0x7f) {
          panning = 0x7f;
        } else {
          panning = p;
        }
      } else {
        panning = 0x80;
      }

      k = (key & 0x7f) + keymap[o].transpose;
      k = CLAMP(k, 0, 127);

      prio += keymap[o].prioOffset;
      prio = CLAMP(prio, 0, 0xff);

      if ((keymap[o].id & 0xc000) == 0) {
        if (!check_portamento(k & 0xff, midi, midiSet, vidFlag, &vid)) {
          return 0xffffffff;
        }
        if (vid != 0xffffffff) {
          return vid;
        }
        return macStart(keymap[o].id, prio, maxVoices, allocId, k | (key & 0x80), vol, panning, midi,
                        midiSet, section, step, trackid, vidFlag, vGroup, studio, itd);
      }

      return StartLayer(keymap[o].id, prio, maxVoices, allocId, k | (key & 0x80), vol, panning, midi,
                        midiSet, section, step, trackid, vidFlag & 0xff, vGroup, studio, itd);
    }
  }

  return SND_ID_ERROR;
}

static void unblockAllAllocatedVoices(u32 vid) {
  u32 id; // r31

  id = vidGetInternalId(vid);
  while (id != SND_ID_ERROR) {
    lbl_8047AF48[id & 0xff].block = 0;
    id = lbl_8047AF48[id & 0xff].child;
  }
}

u32 synthStartSound(u16 id, u8 prio, u8 max,
                    u8 key, u8 vol, u8 panning, u8 midi, u8 midiSet, u8 section, u16 step,
                    u16 trackid, u8 vGroup, s16 prioOffset, u8 studio, u32 itd) {
  prio += prioOffset;
  prio = CLAMP(prio, 0, 0xff);

  switch (id & 0xC000) {
  case 0: {
    u32 vid; // r1+0x34
    if (!check_portamento(key, midi, midiSet, 1, &vid)) {
      return SND_ID_ERROR;
    }
    if (vid != SND_ID_ERROR) {
      return vid;
    }
    return macStart(id, prio, max,
                    id,
                    key, vol, panning, midi, midiSet, section, step, trackid, 1, vGroup, studio,
                    itd);
  }
  case 0x4000: {
    u32 vid = StartKeymap(id, prio, max,
                          id,
                          key, vol, panning, midi, midiSet, section, step, trackid, 1, vGroup,
                          studio, itd);
    if (vid != SND_ID_ERROR) {
      unblockAllAllocatedVoices(vid);
    }
    return vid;
  }
  case 0x8000: {
    u32 vid = StartLayer(id, prio, max,
                         id,
                         key, vol, panning, midi, midiSet, section, step, trackid, 1, vGroup,
                         studio, itd);
    if (vid != SND_ID_ERROR) {
      unblockAllAllocatedVoices(vid);
    }
    return vid;
  }
  default:
    return SND_ID_ERROR;
  }
}

static u32 convert_cents(SYNTH_VOICE* svoice, u32 ccents) {
  u32 curDetune; // r30
  u32 cpitch;    // r31

  cpitch = sndGetPitch(ccents / 65536, svoice->sInfo) * 65536;
  if ((curDetune = ccents & 0xffff) != 0) {
    cpitch += curDetune * ((sndPitchUpOne(cpitch / 65536) & 0xffff) - (cpitch / 65536));
  }
  return cpitch;
}

static void UpdateTimeMIDICtrl(SYNTH_VOICE* sv) {
  if (!sv->timeUsedByInput) {
    return;
  }

  sv->timeUsedByInput = 0;
  sv->midiDirtyFlags = 0x1fff;
}

static void LowPrecisionHandler(u32 i) {
  u32 j;            // r30
  s32 pbend;        // r29
  u32 ccents;       // r28
  u32 cpitch;       // r26
  u16 Modulation;   // r24
  u16 portamento;   // r25
  u32 lowDeltaTime; // r27
  SYNTH_VOICE* sv;  // r31
  u32 cntDelta;     // r20
  u32 addFactor;    // r19
  u16 adsr_start;   // r1+0xE
  u16 adsr_delta;   // r1+0xC
  s32 vrange;       // r23
  s32 voff;         // r22

  sv = &lbl_8047AF48[i];
  if (!hwIsActive(i) && sv->addr == NULL) {
    goto end;
  }

  lowDeltaTime = lbl_8047AF58 - sv->lastLowCallTime;
  sv->lastLowCallTime = lbl_8047AF58;
  for (j = 0; j < 2; ++j) {
    if (sv->lfo[j].period == 0) {
      continue;
    }
    sv->lfo[j].time += lowDeltaTime;
    sv->lfo[j].value =
        sndSin((sv->lfo[j].time % sv->lfo[j].period * 16) / (sv->lfo[j].period / 256));
    if (sv->lfo[j].value != sv->lfo[j].lastValue) {
      sv->lfo[j].lastValue = sv->lfo[j].value;
      if (sv->lfoUsedByInput[j]) {
        sv->lfoUsedByInput[j] = 0;
        sv->midiDirtyFlags |= 0x1fff;
      }
    }
  }

  if ((sv->cFlags & 0x2000) != 0) {
    sv->vibCurTime += lowDeltaTime;
    sv->vibCurOffset = sndSin((sv->vibCurTime % sv->vibPeriod * 16) / (sv->vibPeriod / 256));
  }

  if (sv->sweepNum[0] | sv->sweepNum[1]) {
    cntDelta = (lowDeltaTime << 8) >> 4;
    addFactor = (lowDeltaTime << 4) >> 4;
    for (j = 0; j < 2; ++j) {
      if (sv->sweepNum[j] == 0) {
        continue;
      }
      sv->sweepCnt[j] -= cntDelta;
      if (sv->sweepCnt[j] <= 0) {
        sv->sweepCnt[j] = sv->sweepNum[j] << 16;
        sv->sweepOff[j] = 0;
      } else {
        sv->sweepOff[j] += (sv->sweepAdd[j] >> 12) * addFactor;
      }
    }
  }

  for (j = 0; j < 2; ++j) {
    if (sv->panning[j] == sv->panTarget[j]) {
      continue;
    }
    sv->panTime[j] -= lowDeltaTime;
    if ((s32)sv->panTime[j] <= 0) {
      sv->panning[j] = sv->panTarget[j];
      sv->panTime[j] = 0;
    } else {
      sv->panning[j] = sv->panTarget[j] - (sv->panTime[j] / 256) * sv->panDelta[j];
      sv->panning[j] = CLAMP_INV((s32)sv->panning[j], 0, 0x7f0000u);
    }

    sv->cFlags |= 0x200000000000;
  }

  if ((sv->cFlags & 0x20000000000) != 0 &&
      adsrHandleLowPrecision(&sv->pitchADSR, &adsr_start, &adsr_delta)) {
    sv->cFlags &= ~0x20000000000;
  }

  ccents = sv->curNote * 65536 + (sv->curDetune * 65536) / 100;
  if ((sv->cFlags &
       0x10030
       ) != 0) {
    if (sv->midi != 0xff) {
      pbend = inpGetPitchBend(sv);
      sv->pbLast = pbend;
      goto pbend_adjust;
    }
  } else {
    pbend = sv->pbLast;
  pbend_adjust:
    if (pbend != 0x2000) {
      pbend -= 0x2000;
      if (pbend < 0) {
        ccents += sv->pbLowerKeyRange * pbend * 8;
      } else {
        ccents += sv->pbUpperKeyRange * pbend * 8;
      }
    }
  }

  if ((sv->cFlags & 0x2000) != 0) {
    Modulation = inpGetModulation(sv);
    vrange = sv->vibKeyRange * 256 + (sv->vibCentRange * 256) / 100;
    if (sv->vibModAddScale != 0) {
      vrange += (sv->vibModAddScale * ((Modulation & 0x1ffff) >> 7)) >> 7;
    }
    if ((sv->cFlags & 0x4000) != 0) {
      voff = (sv->vibCurOffset * ((Modulation & 0x1ffff) >> 7)) >> 7;
    } else {
      voff = sv->vibCurOffset;
    }
    ccents += (vrange * voff) >> 4;
  }

  if (sv->midi != 0xff) {
    portamento = inpGetMidiCtrl(SND_MIDICTRL_PORTAMENTO, sv->midi, sv->midiSet);
    if (portamento != sv->portLastCtrlState || (sv->cFlags & 0x21000) == 0x20000) {
      if (portamento <= 0x1f80) {
        sv->cFlags &= ~0x400;
      } else {
        if ((sv->cFlags & 0x400) == 0) {
          synthInitPortamento(sv);
        }
        sv->cFlags |= 0x400;
      }
      sv->cFlags |= 0x1000;
      sv->portLastCtrlState = portamento;
    }
  }

  ccents = apply_portamento(sv, ccents, lowDeltaTime);
  if ((sv->cFlags & 0x20000000000) != 0) {
    ccents += sv->pitchADSRRange * (sv->pitchADSR.currentVolume >> 16) >> 7;
  }

  cpitch = convert_cents(sv, ccents);
  cpitch += sv->sweepOff[0] + sv->sweepOff[1];
  cpitch = ((cpitch >> 16) * inpGetDoppler(sv)) >> 13;
  sv->curPitch = cpitch;

  hwSetPitch(i, cpitch);
  synthAddJob(sv, 0, 0xf00);

end:
  UpdateTimeMIDICtrl(sv);
}

static void ZeroOffsetHandler(u32 i) {
  SYNTH_VOICE* sv;  // r31
  u32 lowDeltaTime; // r26
  u16 Modulation;   // r25
  f32 vol;          // f30
  f32 auxa;         // f25
  f32 auxb;         // f24
  f32 f;            // f27
  f32 voiceVol;     // f28
  u32 volUpdate;   // r30
  f32 lfo;          // f23
  f32 scale;        // f31
  f32 mscale;       // f22
  s32 pan;          // r28
  f32 preVol;       // f26
  f32 postVol;      // f29

  sv = &lbl_8047AF48[i];
  if (!hwIsActive(i) && sv->addr == NULL) {
    goto end;
  }

  lowDeltaTime = lbl_8047AF58 - sv->lastZeroCallTime;
  sv->lastZeroCallTime = lbl_8047AF58;

  if ((sv->cFlags & 0x8000) != 0) {
    sv->envCurrent += sv->envDelta * (lowDeltaTime >> 8);
    if (sv->envDelta < 0) {
      if ((s32)sv->envTarget >= (s32)sv->envCurrent) {
        sv->envCurrent = sv->envTarget;
        sv->cFlags &= ~0x8000;
      }
    } else if ((s32)sv->envTarget <= (s32)sv->envCurrent) {
      sv->envCurrent = sv->envTarget;
      sv->cFlags &= ~0x8000;
    }
    sv->volume = sv->envCurrent;
    volUpdate = TRUE;
  } else {
    volUpdate = (sv->cFlags & 0x100000000000) != 0;
  }

  sv->cFlags &= ~0x100000000000;

  f = lbl_80434E64[sv->vGroup].pauseVol * lbl_80434E64[sv->vGroup].volume *
      lbl_80434E64[sv->fxFlag ? 22 : 21].volume;

  if (sv->track != 0xff) {
    vol = f * (f32)lbl_80435464[sv->track] * (1.f / 127.f);
  } else {
    vol = f;
  }

  if (vol != sv->lastVolFaderScale) {
    sv->lastVolFaderScale = vol;
    volUpdate = TRUE;
  }

  voiceVol = (f32)sv->volume * (1.f / (8192.f * 1016.f) /* 1.201479e-07 */);

  if ((sv->treScale | sv->treModAddScale) != 0) {
    Modulation = inpGetModulation(sv);
    lfo = (f32)(8192 - ((8192 - ((s16)inpGetTremolo(sv) - 8192)) >> 1)) * (1.f / 8192.f);
    mscale = 1.f - (f32)Modulation * (4096 - sv->treModAddScale) * 1.490207e-08f /* 1/(8192^2)? */;
    scale = (f32)sv->treScale * mscale * (1.f / 4096.f);
    if (sv->treCurScale < scale) {
      if ((sv->treCurScale += 0.2f) > scale) {
        sv->treCurScale = scale;
      }
    } else if (sv->treCurScale > scale) {
      if ((sv->treCurScale -= 0.2f) < scale) {
        sv->treCurScale = scale;
      }
    }
    voiceVol *= 1.f - lfo * (1.f - sv->treCurScale);
    volUpdate = TRUE;
  }

  if ((lbl_8047AF44 & 1) == 0) {
    if ((sv->cFlags & 0x200000000000) != 0 || (sv->midiDirtyFlags & 0x6) != 0) {
      sv->cFlags &= ~0x200000000000;
      pan = sv->panning[0] + (inpGetPanning(sv) - 8192) * 0x200;
      sv->lastPan = CLAMP_INV(pan, 0, 0x7f0000);

      if ((lbl_8047AF44 & 2) != 0) {
        if ((sv->lastSPan = sv->panning[1] + inpGetSurroundPanning(sv) * 512) > 0x7f0000) {
          sv->lastSPan = 0x7f0000;
        }
      } else {
        sv->lastSPan = 0;
      }

      volUpdate = TRUE;
    } else if ((lbl_8047AF44 & 2) == 0) {
      sv->lastSPan = 0;
    }
  } else {
    sv->lastPan = 0x400000;
    sv->lastSPan = 0;
    volUpdate |= (sv->cFlags & 0x200000000000) != 0;
    sv->cFlags &= ~0x200000000000;
  }

  if (volUpdate || (sv->midiDirtyFlags & 0xf01) != 0) {
    preVol = voiceVol;
    postVol = voiceVol * vol * (f32)inpGetVolume(sv) * (1.f / 16383.f) /* 1/16384? */;
    auxa = ((f32)sv->revVolOffset * (1.f / 127.f)) +
           ((preVol * (f32)inpGetPreAuxA(sv) * (1.f / 16383.f) /* 1/16384? */) +
            ((f32)sv->revVolScale *
             (postVol * (f32)inpGetReverb(sv) * (1.f / 16383.f) /* 1/16384? */) * (1.f / 127.f)));
    auxb = (preVol * (f32)inpGetPreAuxB(sv) * (1.f / 16383.f) /* 1/16384? */) +
           (postVol * (f32)inpGetPostAuxB(sv) * (1.f / 16383.f) /* 1/16384? */);
    sv->curOutputVolume = (u16)(postVol * 32767.f);
    hwSetVolume(i, sv->volTable, postVol, sv->lastPan, sv->lastSPan, auxa, auxb);
  }
  if (sv->age != 0) {
    if ((s32)(sv->age -= sv->ageSpeed * lowDeltaTime) < 0) {
      sv->age = 0;
    }
    hwSetPriority(i, sv->prio << 24 | sv->age >> 15);
  }

  synthAddJob(sv, SYNTH_JOBTYPE_ZERO, (5 - hwGetTimeOffset()) * 256);

end:
  UpdateTimeMIDICtrl(sv);
}

static void EventHandler(u32 i) {
  SYNTH_VOICE* sv; // r31

  sv = &lbl_8047AF48[i];
  if (!hwIsActive(i) && sv->addr == NULL) {
    goto end;
  }

  macSetPedalState(sv, inpGetPedal(sv) > 0x1f80);

  if ((sv->cFlags & 0x20) != 0) {
    sv->cFlags &= ~0x20;
    sv->cFlags |= 0x10;
    hwStart(i, sv->studio);
  }

  if ((sv->cFlags & 0x10000000090) == 0x90) {
    sv->cFlags &= ~0x90;
    hwKeyOff(i);
    if ((sv->cFlags & 0x20000000000) != 0 && adsrRelease(&sv->pitchADSR)) {
      sv->cFlags &= ~0x20000000000;
    }
  }

end:
  UpdateTimeMIDICtrl(sv);
}

static void synthInitJobQueue() {
  u8 i; // r31

  for (i = 0; i < 32; ++i) {
    synthJobTable[i].lowPrecision = NULL;
    synthJobTable[i].event = NULL;
    synthJobTable[i].zeroOffset = NULL;
  }

  synthJobTableIndex = 0;
}

static void synthAddJob(SYNTH_VOICE* svoice, SYNTH_JOBTYPE jobType, u32 deltaTime) {
  SYNTH_QUEUE* newJq;   // r31
  SYNTH_QUEUE** root;   // r30
  u8 jobTabIndex;       // r29
  SYNTH_JOBTAB* jobTab; // r28

  jobTabIndex = ((deltaTime / 256) + synthJobTableIndex) & 0x1f;
  jobTab = &synthJobTable[jobTabIndex];

  switch (jobType) {
  case SYNTH_JOBTYPE_LOW:
    newJq = &svoice->lowPrecisionJob;
    if (newJq->jobTabIndex != 0xff) {
      if (newJq->jobTabIndex == jobTabIndex) {
        return;
      }
      if (newJq->next != NULL) {
        newJq->next->prev = newJq->prev;
      }
      if (newJq->prev != NULL) {
        newJq->prev->next = newJq->next;
      } else {
        synthJobTable[newJq->jobTabIndex].lowPrecision = newJq->next;
      }
    }
    root = &jobTab->lowPrecision;
    break;
  case SYNTH_JOBTYPE_ZERO:
    newJq = &svoice->zeroOffsetJob;
    if (newJq->jobTabIndex != 0xff) {
      if (newJq->jobTabIndex == jobTabIndex) {
        return;
      }
      if (newJq->next != NULL) {
        newJq->next->prev = newJq->prev;
      }
      if (newJq->prev != NULL) {
        newJq->prev->next = newJq->next;
      } else {
        synthJobTable[newJq->jobTabIndex].zeroOffset = newJq->next;
      }
    }
    root = &jobTab->zeroOffset;
    break;
  case SYNTH_JOBTYPE_EVENT:
    newJq = &svoice->eventJob;
    if (newJq->jobTabIndex != 0xff) {
      return;
    }
    root = &jobTab->event;
    break;
  default:
    break;
  }

  newJq->jobTabIndex = jobTabIndex;
  if ((newJq->next = *root) != NULL) {
    (*root)->prev = newJq;
  }
  newJq->prev = NULL;
  *root = newJq;
}

void synthStartSynthJobHandling(SYNTH_VOICE* svoice) {
  svoice->lastLowCallTime = lbl_8047AF58;
  svoice->lastZeroCallTime = lbl_8047AF58;
  synthAddJob(svoice, SYNTH_JOBTYPE_LOW, 0);
  synthAddJob(svoice, SYNTH_JOBTYPE_ZERO, 0);
}

void synthForceLowPrecisionUpdate(SYNTH_VOICE* svoice) {
  synthAddJob(svoice, SYNTH_JOBTYPE_LOW, 0);
  synthAddJob(svoice, SYNTH_JOBTYPE_ZERO, 0);
}

void synthKeyStateUpdate(SYNTH_VOICE* svoice) { synthAddJob(svoice, SYNTH_JOBTYPE_EVENT, 0); }

static void HandleJobQueue(SYNTH_QUEUE** queueRoot, void (*handler)(u32)) {
  SYNTH_QUEUE* jq;     // r31
  SYNTH_QUEUE* nextJq; // r30

  jq = *queueRoot;
  while (jq != NULL) {
    nextJq = jq->next;
    jq->jobTabIndex = 0xff;
    if (!lbl_8047AF48[jq->voice].block) {
      handler(jq->voice);
    }
    jq = nextJq;
  }

  *queueRoot = NULL;
}

static void HandleVoices() {
  SYNTH_JOBTAB* jTab = &synthJobTable[synthJobTableIndex]; // r31
  HandleJobQueue(&jTab->lowPrecision, LowPrecisionHandler);
  HandleJobQueue(&jTab->event, EventHandler);
  HandleJobQueue(&jTab->zeroOffset, ZeroOffsetHandler);
  synthJobTableIndex = (synthJobTableIndex + 1) & 0x1f;
}

static void HandleFaderTermination(SYNTHMasterFader* smf) {
  switch (smf->seqMode) {
  case 1:
    seqStop(smf->seqId);
    break;
  case 2:
    seqPause(smf->seqId);
    break;
  case 3:
    seqMute(smf->seqId, 0, 0);
    break;
  }
}

void synthHandle(u32 deltaTime) {
  u32 i;                 // r29
  u32 s;                 // r30
  SYNTHMasterFader* smf; // r31
  u32 testFlag;          // r27

  if (lbl_80434C50.numSamples == 0) {
    return;
  }

  macHandle(deltaTime);
  HandleVoices();

  if (hwGetTimeOffset() == 0) {
    if ((lbl_8047AF40 | lbl_8047AF3C) != 0) {
      for (i = 0, smf = lbl_80434E64, testFlag = 1; i < 32; testFlag <<= 1, ++i, ++smf) {
        if ((lbl_8047AF40 & testFlag) != 0) {
          smf->volume = smf->target - smf->time * (smf->target - smf->start);
          if ((smf->time -= smf->deltaTime) <= 0.f) {
            smf->volume = smf->target;
            HandleFaderTermination(smf);
            if ((lbl_8047AF40 &= ~testFlag) == 0 &&
                lbl_8047AF3C == 0) {
              break;
            }
          }
        }

        if ((lbl_8047AF3C & testFlag) != 0) {
          smf->pauseVol = smf->pauseTarget - smf->pauseTime * (smf->pauseTarget - smf->pauseStart);
          if ((smf->pauseTime -= smf->pauseDeltaTime) <= 0.f) {
            smf->pauseVol = smf->pauseTarget;
            if ((lbl_8047AF3C &= ~testFlag) == 0 &&
                lbl_8047AF40 == 0) {
              break;
            }
          }
        }
      }
    }

    for (s = 0; s < 8; ++s) {
      if (lbl_8047AF34[s] != 0xff) {
        SND_AUX_INFO info; // r1+0x18
        for (i = 0; i < SND_AUX_NUMPARAMETERS; ++i) {
          info.data.parameterUpdate.para[i] =
              inpGetAuxA(s, i, lbl_8047AF34[s], lbl_8047AF2C[s]);
        }
        lbl_80435644[s](SND_AUX_REASON_PARAMETERUPDATE, &info, lbl_80435624[s]);
      }

      if (lbl_8047AF24[s] != 0xff) {
        SND_AUX_INFO info; // r1+0xC
        for (i = 0; i < SND_AUX_NUMPARAMETERS; ++i) {
          info.data.parameterUpdate.para[i] =
              inpGetAuxB(s, i, lbl_8047AF24[s], lbl_8047AF1C[s]);
        }
        lbl_80435684[s](SND_AUX_REASON_PARAMETERUPDATE, &info, lbl_80435664[s]);
      }
    }
  }

  hwFrameDone();
  lbl_8047AF58 += deltaTime;
}

u8 synthFXGetMaxVoices(u16 fid) {
  FX_TAB* fx;
  if ((fx = dataGetFX(fid)) != NULL) {
    return fx->maxVoices;
  }

  return 0;
}

u32 synthFXStart(u16 fid,
                 u8 vol, u8 pan, u8 studio, u32 itd) {
  FX_TAB* fx;
  u32 v;
  v = 0xFFFFFFFF;
  if ((fx = dataGetFX(fid)) != NULL) {
    if (vol == 0xFF) {
      vol = fx->volume;
    }

    if (pan == 0xFF) {
      pan = fx->panning;
    }

    v = synthStartSound(fx->macro, fx->priority, fx->maxVoices,
                        fx->key | 0x80,
                        vol, pan, 0xFF, 0xFF, 0, 0, 0xFF, fx->vGroup, 0, studio, itd);
  }

  return v;
}


u32 synthFXSetCtrl(u32 vid, u8 ctrl, u8 value) {
  u32 i;   // r31
  u32 ret; // r29

  ret = FALSE;
  vid = vidGetInternalId(vid);

  while (vid != SND_ID_ERROR) {
    i = vid & 0xff;
    if (vid == lbl_8047AF48[i].id) {
      if ((lbl_8047AF48[i].cFlags & 0x2) != 0) {
        inpSetMidiCtrl(ctrl, i, lbl_8047AF48[i].setup_midiSet, value);
      } else {
        inpSetMidiCtrl(ctrl, i, lbl_8047AF48[i].midiSet, value);
      }

      vid = lbl_8047AF48[i].child;
      ret = TRUE;
    } else {
      return ret;
    }
  }

  return ret;
}

u32 synthFXSetCtrl14(u32 vid, u8 ctrl, u16 value) {
  u32 i;   // r31
  u32 ret; // r29

  ret = FALSE;
  vid = vidGetInternalId(vid);

  while (vid != SND_ID_ERROR) {
    i = vid & 0xff;
    if (vid == lbl_8047AF48[i].id) {
      if ((lbl_8047AF48[i].cFlags & 0x2) != 0) {
        inpSetMidiCtrl14(ctrl, i, lbl_8047AF48[i].setup_midiSet, value);
      } else {
        inpSetMidiCtrl14(ctrl, i, lbl_8047AF48[i].midiSet, value);
      }

      vid = lbl_8047AF48[i].child;
      ret = TRUE;
    } else {
      return ret;
    }
  }

  return ret;
}

void synthFXCloneMidiSetup(SYNTH_VOICE* dest, SYNTH_VOICE* src) {
  inpFXCopyCtrl(SND_MIDICTRL_VOLUME, dest, src);
  inpFXCopyCtrl(SND_MIDICTRL_PANNING, dest, src);
  inpFXCopyCtrl(SND_MIDICTRL_REVERB, dest, src);
  inpFXCopyCtrl(SND_MIDICTRL_PITCHBEND, dest, src);
  inpFXCopyCtrl(SND_MIDICTRL_DOPPLER, dest, src);
}

static u32 synthFXVolume(u32 vid, u8 vol) {
  u32 i;   // r31
  u32 ret; // r29

  ret = FALSE;
  vid = vidGetInternalId(vid);

  while (vid != SND_ID_ERROR) {
    i = vid & 0xff;
    if (vid == lbl_8047AF48[i].id) {
      if ((lbl_8047AF48[i].cFlags & 0x2) != 0) {
        inpSetMidiCtrl(SND_MIDICTRL_VOLUME, i, lbl_8047AF48[i].setup_midiSet, vol);
      } else {
        inpSetMidiCtrl(SND_MIDICTRL_VOLUME, i, lbl_8047AF48[i].midiSet, vol);
      }

      vid = lbl_8047AF48[i].child;
      ret = TRUE;
    } else {
      return ret;
    }
  }

  return ret;
}

u32 synthSendKeyOff(u32 voiceid) {
  u32 i;    // r30
  u32 ret; // r29

  ret = FALSE;

  if (lbl_8047AF18 != 0) {
    voiceid = vidGetInternalId(voiceid);

    while (voiceid != SND_ID_ERROR) {
      i = voiceid & 0xff;

      if (voiceid == lbl_8047AF48[i].id) {
        macSetExternalKeyoff(&lbl_8047AF48[i]);
        ret = TRUE;
      }

      voiceid = lbl_8047AF48[i].child;
    }
  }

  return ret;
}

u16 synthGetVolume(u32 vid) {
  u32 i; // r30

  vid = vidGetInternalId(vid);
  if (vid != SND_ID_ERROR) {
    i = vid & 0xff;
    if (vid == lbl_8047AF48[i].id && (lbl_8047AF48[i].cFlags & 0x2) == 0) {
      return lbl_8047AF48[i].curOutputVolume;
    }
  }

  return 0;
}

static void SetupFader(SYNTHMasterFader* smf, u8 volume, u32 time, u8 seqMode, u32 seqId) {
  smf->seqMode = seqMode;
  smf->seqId = seqId;
  if (time != 0) {
    smf->start = smf->volume;
    smf->target = (f32)volume * (1.f / 127.f);
    smf->time = 1.f;
    smf->deltaTime = 1280.f / (f32)time;
  } else {
    smf->volume = smf->target = (f32)volume * (1.f / 127.f);
    if (smf->seqId != SND_ID_ERROR) {
      HandleFaderTermination(smf);
    }
  }
}

void synthVolume(u8 volume, u16 time, u8 vGroup, u8 seqMode, u32 seqId) {
  u32 ltime;             // r1+0x14
  u32 i;                 // r30
  u8 type;               // r29
  SYNTHMasterFader* smf; // r31

  if ((ltime = time) != 0) {
    sndConvertMs(&ltime);
  }

  switch (vGroup) {
  case SND_ALL_VOLGROUPS:
    for (smf = lbl_80434E64, i = 0; i < 32; ++i, ++smf) {
      if (smf->type == 0 || smf->type == 1) {
        SetupFader(smf, volume, ltime, seqMode, SND_ID_ERROR);
        lbl_8047AF40 |= 1 << i;
      }
    }
    return;

  case SND_USERALL_VOLGROUPS:
    for (smf = lbl_80434E64, i = 0; i < 32; ++i, ++smf) {
      if (smf->type == 2 || smf->type == 3) {
        SetupFader(smf, volume, ltime, seqMode, SND_ID_ERROR);
        lbl_8047AF40 |= 1 << i;
      }
    }
    return;

  case SND_USERMUSIC_VOLGROUPS:
    type = 2;
    goto setup_type;

  case SND_USERFX_VOLGROUPS:
    type = 3;
    goto setup_type;

  case SND_MUSIC_VOLGROUPS:
    type = 0;
    goto setup_type;

  case SND_FX_VOLGROUPS:
    type = 1;
    goto setup_type;

  setup_type:
    for (smf = lbl_80434E64, i = 0; i < 32; ++i, ++smf) {
      if (smf->type == type) {
        SetupFader(smf, volume, ltime, seqMode, SND_ID_ERROR);
        lbl_8047AF40 |= 1 << i;
      }
    }
    return;

  default:
    SetupFader(&lbl_80434E64[vGroup], volume, ltime, seqMode, seqId);
    lbl_8047AF40 |= 1 << vGroup;
    return;
  }
}

u32 synthIsFadeOutActive(u8 vGroup) {
  if (lbl_80434E64[vGroup].type != 4 && (lbl_8047AF40 & (1 << vGroup)) != 0 &&
      lbl_80434E64[vGroup].start > lbl_80434E64[vGroup].target) {
    return TRUE;
  }
  return FALSE;
}

void synthPauseVolume(u8 volume, u16 time, u8 vGroup) {
  u32 i;                 // r30
  u32 ltime;             // r1+0x10
  u8 type;               // r28
  SYNTHMasterFader* smf; // r31

  if (time == 0) {
    ++time;
  }
  ltime = time & 0xffff;
  sndConvertMs(&ltime);

  switch (vGroup) {
  case SND_ALL_VOLGROUPS:
    for (smf = lbl_80434E64, i = 0; i < 32; ++i, ++smf) {
      if (lbl_80434E64[i].type == 0 || lbl_80434E64[i].type == 1) {
        smf->pauseStart = smf->pauseVol;
        smf->pauseTarget = (f32)volume * (1.f / 127.f);
        smf->pauseTime = 1.f;
        smf->pauseDeltaTime = 1280.f / (f32)ltime;
        lbl_8047AF40 |= 1 << i;
      }
    }
    return;

  case SND_USERALL_VOLGROUPS:
    for (smf = lbl_80434E64, i = 0; i < 32; ++i, ++smf) {
      if (lbl_80434E64[i].type == 2 || lbl_80434E64[i].type == 3) {
        smf->pauseStart = smf->pauseVol;
        smf->pauseTarget = (f32)volume * (1.f / 127.f);
        smf->pauseTime = 1.f;
        smf->pauseDeltaTime = 1280.f / (f32)ltime;
        lbl_8047AF40 |= 1 << i;
      }
    }
    return;

  case SND_USERMUSIC_VOLGROUPS:
    type = 2;
    goto setup_type;

  case SND_USERFX_VOLGROUPS:
    type = 3;
    goto setup_type;

  case SND_MUSIC_VOLGROUPS:
    type = 0;
    goto setup_type;

  case SND_FX_VOLGROUPS:
    type = 1;
    goto setup_type;

  setup_type:
    for (smf = lbl_80434E64, i = 0; i < 32; ++i, ++smf) {
      if (lbl_80434E64[i].type == type) {
        smf->pauseStart = smf->pauseVol;
        smf->pauseTarget = (f32)volume * (1.f / 127.f);
        smf->pauseTime = 1.f;
        smf->pauseDeltaTime = 1280.f / (f32)ltime;
        lbl_8047AF40 |= 1 << i;
      }
    }
    return;

  default:
    smf = &lbl_80434E64[vGroup];
    smf->pauseStart = smf->pauseVol;
    smf->pauseTarget = (f32)volume * (1.f / 127.f);
    smf->pauseTime = 1.f;
    smf->pauseDeltaTime = 1280.f / (f32)ltime;
    lbl_8047AF40 |= 1 << vGroup;
    return;
  }
}

void synthSetMusicVolumeType(u8 vGroup, u8 type) {
  if (lbl_8047AF18) {
    lbl_80434E64[vGroup].type = type;
  }
}

static u32 synthHWMessageHandler(u32 mesg, u32 voiceID) {
  u32 ret; // r30

  ret = FALSE;

  switch (mesg) {
  case 0:
    if (lbl_8047AF48[voiceID & 0xff].block != 0) {
      break;
    }
    vsSampleEndNotify(hwGetVirtualSampleID(voiceID & 0xff));
    if (voiceID != lbl_8047AF48[voiceID & 0xff].id) {
      break;
    }
    macSampleEndNotify(&lbl_8047AF48[voiceID & 0xff]);
    break;

  case 1:
    voiceKill(voiceID & 0xff);
    break;

  case 2:
    ret = vsSampleStartNotify(voiceID);
    break;

  case 3:
    vsSampleEndNotify(hwGetVirtualSampleID(voiceID & 0xff));
    break;

  default:
    break;
  }

  return ret;
}

void synthInit(u32 mixFrq, u32 numVoices) {
  u32 i; // r31

  lbl_8047AF58 = 0;
  lbl_80434C50.mixFrq = mixFrq;
  synthSetBpm(120, 255, 0);
  lbl_8047AF44 = 0;
  lbl_8047AF4C = NULL;

  lbl_8047AF48 = salMalloc(numVoices * sizeof(SYNTH_VOICE));
  if (lbl_8047AF48 == NULL) {
  }
  memset(lbl_8047AF48, 0, numVoices * sizeof(SYNTH_VOICE));

  for (i = 0; i < numVoices; ++i) {
    lbl_8047AF48[i].id = 0xffffffff;
    lbl_8047AF48[i].cFlags = 0;
    lbl_8047AF48[i].age = 0;
    lbl_8047AF48[i].prio = 0;
    lbl_8047AF48[i].midi = 0xff;
    lbl_8047AF48[i].volume = 0;
    lbl_8047AF48[i].volTable = 0;
    lbl_8047AF48[i].revVolScale = 128;
    lbl_8047AF48[i].revVolOffset = 0;
    lbl_8047AF48[i].panning[0] = lbl_8047AF48[i].panTarget[0] = 0x400000;
    lbl_8047AF48[i].panning[1] = lbl_8047AF48[i].panTarget[1] = 0;
    lbl_8047AF48[i].sweepOff[0] = 0;
    lbl_8047AF48[i].sweepOff[1] = 0;
    lbl_8047AF48[i].sweepNum[0] = 0;
    lbl_8047AF48[i].sweepNum[1] = 0;
    lbl_8047AF48[i].block = 0;
    lbl_8047AF48[i].vGroup = 23;
    lbl_8047AF48[i].keyGroup = 0;
    lbl_8047AF48[i].itdMode = 1;
    lbl_8047AF48[i].lfo[0].period = 0;
    lbl_8047AF48[i].lfo[0].value = 0;
    lbl_8047AF48[i].lfo[0].lastValue = 0x7fff;
    lbl_8047AF48[i].lfo[1].period = 0;
    lbl_8047AF48[i].lfo[1].value = 0;
    lbl_8047AF48[i].lfo[1].lastValue = 0x7fff;
    lbl_8047AF48[i].portTime = 25600;
    lbl_8047AF48[i].portType = 0;
    lbl_8047AF48[i].studio = 0;
    lbl_8047AF48[i].lowPrecisionJob.voice = i;
    lbl_8047AF48[i].lowPrecisionJob.jobTabIndex = 0xff;
    lbl_8047AF48[i].zeroOffsetJob.voice = i;
    lbl_8047AF48[i].zeroOffsetJob.jobTabIndex = 0xff;
    lbl_8047AF48[i].eventJob.voice = i;
    lbl_8047AF48[i].eventJob.jobTabIndex = 0xff;
  }

  for (i = 0; i < 32; ++i) {
    lbl_80434E64[i].volume = 0.f;
    lbl_80434E64[i].pauseVol = 1.f;
    lbl_80434E64[i].type = 4;
  }

  lbl_8047AF40 = 0;
  lbl_8047AF3C = 0;
  lbl_80434E64[31].type = 1;

  for (i = 0; i < 8; ++i) {
    lbl_80434E64[i + 23].type = 0;
  }

  lbl_80434E64[21].volume = 1.f;
  lbl_80434E64[22].volume = 1.f;
  inpInit(0);

  for (i = 0; i < 8; ++i) {
    lbl_80435644[i] = NULL;
    lbl_8047AF34[i] = 0xff;
    lbl_80435684[i] = NULL;
    lbl_8047AF24[i] = 0xff;
    lbl_804356A4[i].sfx = 0;
    lbl_804356A4[i].music = 0;
  }

  macInit();
  vidInit();
  synthInitAllocationAids();

  for (i = 0; i < 16; ++i) {
    lbl_804356B4[i] = 0;
  }

  voiceInitLastStarted();
  synthInitJobQueue();
  hwSetMesgCallback(synthHWMessageHandler);
}

void synthExit() { salFree(lbl_8047AF48); }
