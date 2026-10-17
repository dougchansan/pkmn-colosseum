/**
 * @file gs_range_801653CC.c
 * @brief gs-engine sound suffix, 0x80166E88 - 0x80167040.
 *
 * fn_80166E88 initialises the MusyX wrapper. Its allocator hooks are a local
 * aggregate initializer, so MWCC emits the { alloc, free } pair as this
 * unit's own .sdata2 constant (0x8047D558 - 0x8047D560) and copies it to
 * the stack before handing it to sndSetHooks (sndSetHooks).
 */
#include "game/gs_range_801653CC_shared.h"

/* MusyX SND_HOOKS: allocator callbacks passed to sndSetHooks. */
typedef struct GSsndHooks {
    void* (*malloc)(u32 size);
    void (*free)(void* ptr);
} GSsndHooks;

extern u32 lbl_8047B0C4;
extern u32 lbl_8047B0C8;
extern u32 lbl_8047B0CC;
extern u32 lbl_8047B0D0;
extern u32 lbl_8047B0D4;
extern u32 lbl_8047B0D8;
extern u32 lbl_8047B0DC;
extern u32 lbl_8047B0E0;
extern void* _soundMemAlloc(u32 size);
extern void _soundMemFree(void* ptr);
extern void fn_80167A6C(void);
extern void fn_80167A14(void);
extern void fn_801679E4(void);
extern void _sndInitStack(void);
extern void sndSetHooks(GSsndHooks* hooks);
extern s32 fn_8015FE88(u32, u32, u32, u32, u32, u32);
extern void sndAuxCallbackPrepareReverbHI(void* work);
extern void sndAuxCallbackReverbHI(void);
extern void sndSetAuxProcessingCallbacks(u32, void*, void*, u32, u32, u32,
                                         u32, u32, u32);
extern u32 GSsndGetOutputMode(void);
extern void soundSetOutputMode(u32 mode);
extern void _soundReleaseFinishedVoice(void);
extern void sndSetReceiveMessageCallback(void* callback);

u32 soundInit(u32 stackCount, u32 workSize, u32 extraStackCount,
                u32 emitterCount, u32 emitter3dCount)
{
    GSsndHooks hooks = { _soundMemAlloc, _soundMemFree };

    lbl_8047B0E8 = *lbl_80478FA8;
    lbl_8047B0E4 = -1;
    lbl_8047B0E0 = stackCount + extraStackCount;

    lbl_8047B0DC = (u32)_soundMemAlloc(lbl_8047B0E0 * 0x14);
    if (lbl_8047B0DC == 0) {
        return 0;
    }
    fn_80167A6C();

    lbl_8047B0D8 = workSize;
    lbl_8047B0D4 = (u32)_soundMemAlloc(workSize);
    if (lbl_8047B0D4 == 0) {
        return 0;
    }
    _sndInitStack();

    lbl_8047B0D0 = emitterCount;
    lbl_8047B0CC = (u32)_soundMemAlloc(emitterCount * 0xD0);
    if (lbl_8047B0CC == 0) {
        return 0;
    }
    fn_80167A14();

    lbl_8047B0C8 = emitter3dCount;
    lbl_8047B0C4 = (u32)_soundMemAlloc(emitter3dCount * 0x78);
    if (lbl_8047B0C4 == 0) {
        return 0;
    }
    fn_801679E4();

    sndSetHooks(&hooks);
    if (fn_8015FE88(0x40, 0x30, 0x10, 1, 0, 0x9FC000) != 0) {
        return 0;
    }

    _sndSetReverbParm(0);
    sndAuxCallbackPrepareReverbHI(lbl_80452500);
    sndSetAuxProcessingCallbacks(0, sndAuxCallbackReverbHI, lbl_80452500,
                                 0xFF, 0, 0, 0, 0xFF, 0);
    soundMasterVolume(0x7F, 0, 1, 0);
    soundMasterVolume(0x64, 0, 0, 1);
    soundSetOutputMode(GSsndGetOutputMode());
    sndSetReceiveMessageCallback(_soundReleaseFinishedVoice);
    return 1;
}
