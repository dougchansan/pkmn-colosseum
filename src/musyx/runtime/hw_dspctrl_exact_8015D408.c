#include "musyx/runtime/hw_dspctrl.h"

typedef u32 (*SynthMessageCallback)(u32 message, u32 userValue);

u32 salSynthSendMessage(DSPvoice* voice, u32 message)
{
    SynthMessageCallback callback = (SynthMessageCallback) lbl_8047B028;

    if (callback == NULL) {
        return 0;
    }
    return callback(message, voice->mesgCallBackUserValue);
}

void salDeactivateVoice(DSPvoice* voice);

void salActivateVoice(DSPvoice* voice, u8 studio)
{
    if (voice->state != 0) {
        salDeactivateVoice(voice);
        voice->changed[0] |= 0x20;
    }
    voice->postBreak = 0;
    if ((voice->next = lbl_80447E60[studio].voiceRoot) != NULL) {
        voice->next->prev = voice;
    }
    voice->prev = NULL;
    lbl_80447E60[studio].voiceRoot = voice;
    voice->startupBreak = 0;
    voice->state = 1;
    voice->studio = studio;
}

void salDeactivateVoice(DSPvoice* voice)
{
    if (voice->state == 0) {
        return;
    }
    if (voice->prev != NULL) {
        voice->prev->next = voice->next;
    } else {
        lbl_80447E60[voice->studio].voiceRoot = voice->next;
    }
    if (voice->next != NULL) {
        voice->next->prev = voice->prev;
    }
    voice->state = 0;
}

u32 salAddStudioInput(DSPstudioinfo* studio, SND_STUDIO_INPUT* input)
{
    if (studio->numInputs < 7) {
        studio->in[studio->numInputs].studio = input->srcStudio;
        studio->in[studio->numInputs].vol =
            ((u16) input->vol << 8) | ((u16) input->vol << 1);
        studio->in[studio->numInputs].volA =
            ((u16) input->volA << 8) | ((u16) input->volA << 1);
        studio->in[studio->numInputs].volB =
            ((u16) input->volB << 8) | ((u16) input->volB << 1);
        studio->in[studio->numInputs].desc = input;
        studio->numInputs++;
        return 1;
    }
    return 0;
}

u32 salRemoveStudioInput(DSPstudioinfo* studio, SND_STUDIO_INPUT* input)
{
    long i;

    for (i = 0; i < studio->numInputs; i++) {
        if (studio->in[i].desc == input) {
            for (; i <= studio->numInputs - 2; i++) {
                studio->in[i] = studio->in[i + 1];
            }
            studio->numInputs--;
            return 1;
        }
    }
    return 0;
}

/* Last function of hw_dspctrl.c; hw_volconv.c starts at 0x8015D7D0. */
typedef struct SND_AUX_INFO {
    union {
        struct {
            s32* left;
            s32* right;
            s32* surround;
        } bufferUpdate;
    } data;
} SND_AUX_INFO;

typedef void (*SND_AUX_CALLBACK)(u8 reason, SND_AUX_INFO* info, void* user);

void salHandleAuxProcessing(void)
{
    u8 st;
    s32* work;
    DSPstudioinfo* sp;
    SND_AUX_INFO ai;

    sp = &lbl_80447E60[0];
    for (st = 0; st < lbl_8047B05C; ++st, ++sp) {
        if (sp->state != 1) {
            continue;
        }
        if (sp->auxAHandler != NULL) {
            work = sp->auxA[(lbl_8047B05E + 2) % 3];
            ai.data.bufferUpdate.left = work;
            ai.data.bufferUpdate.right = work + 0xA0;
            ai.data.bufferUpdate.surround = work + 0x140;
            ((SND_AUX_CALLBACK)sp->auxAHandler)(0, &ai, sp->auxAUser);
            DCFlushRangeNoSync(work, 0x780);
        }
        if (sp->type == 0 && sp->auxBHandler != NULL) {
            work = sp->auxB[(lbl_8047B05E + 2) % 3];
            ai.data.bufferUpdate.left = work;
            ai.data.bufferUpdate.right = work + 0xA0;
            ai.data.bufferUpdate.surround = work + 0x140;
            ((SND_AUX_CALLBACK)sp->auxBHandler)(0, &ai, sp->auxBUser);
            DCFlushRangeNoSync(work, 0x780);
        }
    }
}
