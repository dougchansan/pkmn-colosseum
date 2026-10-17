/**
 * @file snd_init.c
 * @brief MusyX sndInit/sndQuit, 0x8015FE88 - 0x8015FFDC.
 *
 * Follows the reference MusyX runtime's snd_init.c (AxioDL/musyx) in its
 * 2.0.0-and-earlier form (DoInit takes the ARAM size, one global data
 * stack). DoInit is the reference's static helper, inlined into sndInit
 * as in retail. sndSetMaxVoices is never called by the game and is
 * dead-stripped. The state it touches stays extern.
 */
#include "dolphin/types.h"

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

extern SynthInfo lbl_80434C50;               /* synthInfo */
extern u8 lbl_8047AF18;                      /* sndActive */
extern u8 lbl_8047AF50;                      /* synthIdleWaitActive */

extern s32 hwInit(u32* frq, u16 numVoices, u16 numStudios, u32 flags);
extern void hwExit(void);
extern void dataInitStack(void);               /* dataInitStack */
extern void dataInit(u32 smpBase, u32 smpLength);
extern void dataExit(void);
extern void seqInit(void);
extern void synthInit(u32 mixFrq, u32 numVoices);
extern void synthExit(void);
extern void streamInit(void);               /* streamInit */
extern void vsInit(void);
extern void s3dInit(u32 flags);          /* s3dInit */
extern void s3dExit(void);               /* s3dExit */

static s32 DoInit(u32 mixFrq, u32 aramSize, u32 numVoices, u32 flags)
{
    BOOL ret;

    ret = FALSE;

    dataInitStack();
    dataInit(0, aramSize);
    seqInit();
    lbl_8047AF50 = 0;
    synthInit(mixFrq, numVoices);
    streamInit();
    vsInit();
    s3dInit(flags);
    lbl_8047AF18 = 1;

    return ret;
}

s32 fn_8015FE88(u8 voices, u8 music, u8 sfx, u8 studios, u32 flags, u32 aramSize) /* sndInit */
{
    s32 ret;
    u32 frq;

    ret = 0;
    lbl_8047AF18 = 0;
    if (voices <= 64) {
        lbl_80434C50.voiceNum = voices;
    } else {
        lbl_80434C50.voiceNum = 64;
    }
    if (studios <= 8) {
        lbl_80434C50.studioNum = studios;
    } else {
        lbl_80434C50.studioNum = 8;
    }

    lbl_80434C50.maxMusic = music;
    lbl_80434C50.maxSFX = sfx;
    frq = 32000;
    if ((ret = hwInit(&frq, lbl_80434C50.voiceNum, lbl_80434C50.studioNum, flags)) == 0) {
        ret = DoInit(32000, aramSize, lbl_80434C50.voiceNum, flags);
    }

    return ret;
}

void sndQuit(void)
{
    hwExit();
    dataExit();
    s3dExit();
    synthExit();
    lbl_8047AF18 = 0;
}

u8 fn_8015FFD4(void) /* sndIsInstalled */
{
    return lbl_8047AF18;
}
