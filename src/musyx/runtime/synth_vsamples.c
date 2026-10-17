/**
 * @file synth_vsamples.c
 * @brief MusyX virtual (streamed) samples, 0x80159494 - 0x80159C48.
 *
 * Follows the reference MusyX runtime's synth_vsamples.c (AxioDL/musyx) in
 * its 2.0.0 form (vsSampleStartNotify takes the hardware voice and always
 * reports the start; vsSampleUpdates handles both end states). The game
 * never calls the sndVirtualSample* API, so the linker dead-strips it;
 * only the functions retail keeps are here. vs itself stays extern (.bss,
 * lbl_80446F10).
 *
 * vsNewInstanceID, vsAllocateBuffer and vsFreeBuffer are the reference's
 * helpers; retail inlines them into vsSampleStartNotify and
 * vsSampleEndNotify (vsFreeBuffer at both sites).
 */
#include "dolphin/types.h"
#include "musyx/runtime/synth_voice.h"

typedef struct SND_VIRTUALSAMPLE_INFO {
    u16 smpID;
    u16 instID;
    union {
        struct {
            u32 off1;
            u32 len1;
            u32 off2;
            u32 len2;
        } update;
    } data;
} SND_VIRTUALSAMPLE_INFO;

typedef struct VS_BUFFER {
    u8 state;
    u8 hwId;
    u8 smpType;
    u8 voice;
    u32 last;
    u32 finalGoodSamples;
    u32 finalLast;
    SND_VIRTUALSAMPLE_INFO info;
} VS_BUFFER;

typedef struct VS {
    u8 numBuffers;
    u32 bufferLength;
    VS_BUFFER streamBuffer[64];
    u8 voices[64];
    u16 nextInstID;
    u32 (*callback)(u8 reason, const SND_VIRTUALSAMPLE_INFO* info);
} VS;

extern VS lbl_80446F10;                      /* vs */

extern SYNTH_VOICE* lbl_8047AF48;            /* synthVoice */

extern u32 aramGetStreamBufferAddress(u8 id, u32* len);
extern void fn_80162858(u32 voice, void* addr, u32 len); /* hwSetVirtualSampleLoopBuffer */
extern u16 fn_801628A0(u32 voice);           /* hwGetSampleID */
extern u8 fn_8016288C(u32 voice);            /* hwGetSampleType */
extern u32 hwGetVirtualSampleState(u32 voice);           /* hwGetVirtualSampleState */
extern u32 hwGetPos(u32 voice);           /* hwGetPos */
extern u32 hwGetVirtualSampleID(u32 voice);
extern u32 hwVoiceInStartup(u32 voice);           /* hwVoiceInStartup */
extern void hwBreak(u32 voice);
extern void macSampleEndNotify(SYNTH_VOICE* svoice);
extern void voiceKill(u32 voice);

void vsInit(void)
{
    u32 i;

    lbl_80446F10.numBuffers = 0;
    for (i = 0; i < 64; i++) {
        lbl_80446F10.voices[i] = 0xFF;
    }

    lbl_80446F10.nextInstID = 0;
    lbl_80446F10.callback = NULL;
}

static u16 vsNewInstanceID(void)
{
    u8 i;
    u16 instID;

    do {
        instID = lbl_80446F10.nextInstID++;
        for (i = 0; i < lbl_80446F10.numBuffers; ++i) {
            if (lbl_80446F10.streamBuffer[i].state != 0 && lbl_80446F10.streamBuffer[i].info.instID == instID) {
                break;
            }
        }
    } while (i != lbl_80446F10.numBuffers);

    return instID;
}

static u8 vsAllocateBuffer(void)
{
    u8 i;

    for (i = 0; i < lbl_80446F10.numBuffers; ++i) {
        if (lbl_80446F10.streamBuffer[i].state != 0) {
            continue;
        }
        lbl_80446F10.streamBuffer[i].state = 1;
        lbl_80446F10.streamBuffer[i].last = 0;
        return i;
    }

    return 0xFF;
}

static void vsFreeBuffer(u8 bufferIndex)
{
    lbl_80446F10.streamBuffer[bufferIndex].state = 0;
    lbl_80446F10.voices[lbl_80446F10.streamBuffer[bufferIndex].voice] = 0xFF;
}

u32 vsSampleStartNotify(u8 voice) /* vsSampleStartNotify */
{
    u8 sb;
    u8 i;
    u32 addr;

    for (i = 0; i < lbl_80446F10.numBuffers; ++i) {
        if (lbl_80446F10.streamBuffer[i].state != 0 && lbl_80446F10.streamBuffer[i].voice == voice) {
            vsFreeBuffer(i);
        }
    }

    sb = lbl_80446F10.voices[voice] = vsAllocateBuffer();
    if (sb != 0xFF) {
        addr = aramGetStreamBufferAddress(lbl_80446F10.voices[voice], 0);
        fn_80162858(voice, (void*)addr, lbl_80446F10.bufferLength);
        lbl_80446F10.streamBuffer[sb].info.smpID = fn_801628A0(voice);
        lbl_80446F10.streamBuffer[sb].info.instID = vsNewInstanceID();
        lbl_80446F10.streamBuffer[sb].smpType = fn_8016288C(voice);
        lbl_80446F10.streamBuffer[sb].voice = voice;
        if (lbl_80446F10.callback != NULL) {
            lbl_80446F10.callback(0, &lbl_80446F10.streamBuffer[sb].info);
            return (lbl_80446F10.streamBuffer[sb].info.instID << 8) | voice;
        }
        fn_80162858(voice, 0, 0);
    } else {
        fn_80162858(voice, 0, 0);
    }

    return 0xFFFFFFFF;
}

void vsSampleEndNotify(u32 pubID)
{
    u8 sb;

    if (pubID != 0xFFFFFFFF) {
        u8 id = (u8)pubID;
        sb = lbl_80446F10.voices[id];
        if (sb != 0xFF) {
            if (lbl_80446F10.streamBuffer[sb].info.instID == ((pubID >> 8) & 0xFFFF)) {
                if (lbl_80446F10.callback != NULL) {
                    lbl_80446F10.callback(2, &lbl_80446F10.streamBuffer[sb].info);
                }
                vsFreeBuffer(sb);
            }
        }
    }
}

void vsUpdateBuffer(VS_BUFFER* sb, u32 cpos) /* vsUpdateBuffer */
{
    u32 len;

    if (sb->last == cpos) {
        return;
    }
    if ((s32)sb->last < cpos) {
        switch (sb->smpType) {
        case 5: {
            u32 off = (sb->last / 14) * 8;
            sb->info.data.update.off1 = off;
            sb->info.data.update.len1 = cpos - sb->last;
            sb->info.data.update.off2 = 0;
            sb->info.data.update.len2 = 0;
            if ((len = lbl_80446F10.callback(1, &sb->info)) != 0) {
                sb->last = (sb->last + len) % lbl_80446F10.bufferLength;
            }
        } break;
        default:
            break;
        }
    } else if (cpos == 0) {
        switch (sb->smpType) {
        case 5: {
            u32 off = (sb->last / 14) * 8;
            sb->info.data.update.off1 = off;
            sb->info.data.update.len1 = lbl_80446F10.bufferLength - sb->last;
            sb->info.data.update.off2 = 0;
            sb->info.data.update.len2 = 0;
            if ((len = lbl_80446F10.callback(1, &sb->info)) != 0) {
                sb->last = (sb->last + len) % lbl_80446F10.bufferLength;
            }
        } break;
        default:
            break;
        }
    } else {
        switch (sb->smpType) {
        case 5: {
            u32 off = (sb->last / 14) * 8;
            sb->info.data.update.off1 = off;
            sb->info.data.update.len1 = lbl_80446F10.bufferLength - sb->last;
            sb->info.data.update.off2 = 0;
            sb->info.data.update.len2 = cpos;
            if ((len = lbl_80446F10.callback(1, &sb->info)) != 0) {
                sb->last = (sb->last + len) % lbl_80446F10.bufferLength;
            }
        } break;
        default:
            break;
        }
    }
}

void vsSampleUpdates(void)
{
    u32 i;
    u32 cpos;
    u32 realCPos;
    VS_BUFFER* sb;
    u32 nextSamples;

    if (lbl_80446F10.callback == NULL) {
        return;
    }

    for (i = 0; i < 64; ++i) {
        if (lbl_80446F10.voices[i] != 0xFF && hwGetVirtualSampleState(i) != 0) {
            sb = &lbl_80446F10.streamBuffer[lbl_80446F10.voices[i]];
            realCPos = hwGetPos(i);
            if (sb->smpType == 5) {
                cpos = (realCPos / 14) * 14;
            } else {
                cpos = realCPos;
            }

            switch (sb->state) {
            case 1:
                vsUpdateBuffer(sb, cpos);
                break;
            case 2:
            case 3:
                if (((sb->info.instID << 8) | sb->voice) == hwGetVirtualSampleID(sb->voice)) {
                    vsUpdateBuffer(sb, cpos);

                    if (realCPos >= sb->finalLast) {
                        sb->finalGoodSamples -= (realCPos - sb->finalLast);
                    } else {
                        sb->finalGoodSamples -= (lbl_80446F10.bufferLength - (sb->finalLast - realCPos));
                    }

                    sb->finalLast = realCPos;
                    nextSamples = (lbl_8047AF48[sb->voice].curPitch * 160 + 0xFFF) / 4096;
                    if ((s32)nextSamples > (s32)sb->finalGoodSamples) {
                        if (!hwVoiceInStartup(sb->voice)) {
                            if (sb->state == 2) {
                                hwBreak(sb->voice);
                                macSampleEndNotify(&lbl_8047AF48[sb->voice]);
                            } else {
                                voiceKill(sb->voice);
                            }
                        }

                        sb->state = 0;
                        lbl_80446F10.voices[sb->voice] = 0xFF;
                    }
                } else {
                    sb->state = 0;
                    lbl_80446F10.voices[sb->voice] = 0xFF;
                }
                break;
            }
        }
    }
}
