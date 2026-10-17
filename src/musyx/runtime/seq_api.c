/**
 * @file seq_api.c
 * @brief MusyX runtime sequencer API wrappers (musyx/runtime/seq_api.c),
 * 0x8014D598 - 0x8014D75C.
 *
 * Split out of the misnamed people_field.c unit (2026-07-02). Reference:
 * AxioDL/musyx `musyx/runtime/seq_api.c`. Boundary evidence: fn_8014D598
 * is sndSeqGetValid (calls seqGetPrivateId, returns id != -1); seqGetMIDIPriority
 * is seqGetMIDIPriority (simindex seq=1.0 vs MP4/Prime matched copies),
 * ending at sndFXCtrl (0x8014D75C), snd_synthapi.c's territory.
 * All functions asm-only until matched.
 */
#include "dolphin/types.h"

/* ===== Cross-TU MusyX runtime functions (defined in seq.c) ===== */
extern u32  seqGetPrivateId(u32 seqId);
extern void seqPause(u32 seqId);
extern void seqSpeed(u32 seqId, u16 speed);
extern void seqContinue(u32 seqId);
extern void seqMute(u32 seqId, u32 mask1, u32 mask2);
extern void seqVolume(u8 volume, u16 time, u32 seqId, u8 mode);

/* ===== Cross-TU MusyX runtime globals (defined in seq.c) ===== */
extern u16 lbl_80434910[8][16]; /* seqMIDIPriority */

/* ===== Cross-TU low-level hardware IRQ helpers ===== */
extern void hwDisableIrq(void);
extern void hwEnableIrq(void);

u32 fn_8014D598(u32 seqId) { return seqGetPrivateId(seqId) != -1; }

void sndSeqStop(u32 seqId) {
  hwDisableIrq();
  seqPause(seqId);
  hwEnableIrq();
}

void sndSeqSpeed(u32 seqId, u16 speed) {
  hwDisableIrq();
  seqSpeed(seqId, speed);
  hwEnableIrq();
}

void sndSeqContinue(u32 seqId) {
  hwDisableIrq();
  seqContinue(seqId);
  hwEnableIrq();
}

void sndSeqMute(u32 seqId, u32 mask1, u32 mask2) {
  hwDisableIrq();
  seqMute(seqId, mask1, mask2);
  hwEnableIrq();
}

void sndSeqVolume(u8 volume, u16 time, u32 seqId, u8 mode) {
  hwDisableIrq();
  seqVolume(volume, time, seqId, mode);
  hwEnableIrq();
}

u16 seqGetMIDIPriority(u8 set, u8 channel) { return lbl_80434910[set][channel]; }
