/**
 * @file gs_range_80265EC4.c
 * @brief gs-engine code, 0x80265EC4 - 0x8026635C (6 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). The range name stays honest until internal
 * TU structure is proven.
 */
#include "dolphin/types.h"

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

extern u32 pokemonGetStatus(u8* obj, u32 id, u32 selector, u32 d);
extern u32 savedataGetStatus();
extern s8 fadeCheck(u8 wait);
extern void fadeSet(u16 type, f32 duration);
extern void _threadSwitch(void);
extern void msgctrlSetValue(u32 id, u32 value);
extern s32 winMsgOpenField(u32 message, u32 wait, u8 pause);
extern void winMsgCloseField(u32 wait);
extern void wazaSequenceSysUpdate(void);
extern void* wazaSequencePokemonCreate(u16 index, void* model, u8 variant);
extern BOOL wazaSequenceOwnerLoadWaza(void* owner, u16 group, u16 index, u8 variant);
extern void wazaSequenceOwnerSetVisibility(void* effect, u32 visible);
extern void wazaSequenceSysStartWaza(void* sequence, u16 moveID, u16 variant);
extern u8 wazaSequenceSysIsPlayingWaza(void* obj, u16 search_key1, u16 search_key2);
extern void floorSetFadeScript();

#if !defined(GS_RANGE_80265EC4_SPLIT) || defined(GS_RANGE_80265EC4_PREFIX)
/* Address: 0x80265EC4 | Size: 0x50 */
void exribbonSetNo(int no, u8 value)
{
    u8* data = (u8*)savedataGetStatus(0, 0x10);

    if (data[no] == 0 && value != 0) {
        data[no] = value;
    }
}

/* Address: 0x80265F14 | Size: 0x38 */
u8 exribbonGetNo(int r3)
{
    int iVar1;

    iVar1 = savedataGetStatus(0, 0x10);
    return *(u8 *)(iVar1 + r3);
}

/* Address: 0x80265F4C | Size: 0x48 */
void exribbonInit(u8* data)
{
    int i;

    if (data == NULL) {
        data = (u8*)savedataGetStatus(0, 0x10);
    }

    for (i = 0; i < 11; i++) {
        *data++ = 0;
    }
}
#endif

#if !defined(GS_RANGE_80265EC4_SPLIT) || defined(GS_RANGE_80265F94_SUFFIX)
extern s8 winMsgCheck(void);

static inline u8 fn_80265F94_msgOpen(void)
{
    if (winMsgCheck() == 1) {
        return 1;
    }
    return 0;
}

static inline void fn_80265F94_update(void* model, u16 id)
{
    wazaSequenceSysUpdate();
    if (wazaSequenceSysIsPlayingWaza(model, id, 4) == 0) {
        wazaSequenceSysStartWaza(model, id, 4);
    }
    _threadSwitch();
}

extern const u16 lbl_8047E6F8;

/* RULE-EXCEPTION(user-approved): single-use inline helper; returning the model through it gives retail's register order - see docs/RULE_EXCEPTIONS.md */
static inline void* fn_80265F94_create(u16 species)
{
    void* model = wazaSequencePokemonCreate(species, NULL, 0);
    wazaSequenceOwnerLoadWaza(model, lbl_8047E6F8, 4, 0);
    wazaSequenceOwnerSetVisibility(model, 1);
    return model;
}

/* Address: 0x80265F94 | Size: 0x2BC */
void d2presentPlayGiftScene(int r3)
{
    extern u16 pokemonDataBiosGetVoice(u8* data);
    extern void* pokemonDataBiosGetPtr(u16 species);
    extern s32 GSsndGetStatus(u16 voice);
    extern void soundStartSE(u16 voice);
    extern const f32 lbl_8047E6FC;
    u16 voice;
    void* model;
    u16 status;
    u16 id;
    u16 species;
    u8* data;
    u32 msg;

    status = pokemonGetStatus(NULL, 0xfa, 0x66, 0);
    species = 0xFFFF;
    if (status != 0) {
        species = status;
    }
    model = fn_80265F94_create(species);
    id = lbl_8047E6F8;
    wazaSequenceSysStartWaza(model, id, 4);
    fadeSet(2, lbl_8047E6FC);
    while (fadeCheck(0) != 0) {
        fn_80265F94_update(model, id);
    }
    data = pokemonDataBiosGetPtr(0xfa);
    if (data == NULL) {
        voice = 0;
    } else {
        /* RULE-EXCEPTION(user-approved): no-op (u16) cast on the u16 result makes the truncation into voice explicit, as retail's clrlwi - see docs/RULE_EXCEPTIONS.md */
        voice = (u16)pokemonDataBiosGetVoice(data);
        soundStartSE(voice);
    }
    while (GSsndGetStatus(voice) == 2) {
        fn_80265F94_update(model, id);
    }
    winMsgOpenField(0x44ba, 0, 0);
    while (fn_80265F94_msgOpen()) {
        fn_80265F94_update(model, id);
    }
    winMsgCloseField(1);
    switch (r3) {
    case 0:
        msgctrlSetValue(0x5d, 0x3d2);
        msg = 0x44bc;
        break;
    case 1:
        msgctrlSetValue(0x5d, 0x3d2);
        msg = 0x44bb;
        break;
    default:
        msg = 0x44b9;
        break;
    }
    winMsgOpenField(msg, 0, 0);
    while (fn_80265F94_msgOpen()) {
        fn_80265F94_update(model, id);
    }
    winMsgCloseField(1);
    fadeSet(3, lbl_8047E6FC);
    while (fadeCheck(0) != 0) {
        fn_80265F94_update(model, id);
    }
}

#endif

#if !defined(GS_RANGE_80265EC4_SPLIT) || defined(GS_RANGE_80266250_EXACT)
/* RULE-EXCEPTION(user-approved): redundant masks preserve retail's saved-volume register moves; see docs/RULE_EXCEPTIONS.md. */
/* Address: 0x80266250 | Size: 0xD0 | Ghidra import */
void evolutionSequenceEnd(void)
{
    extern int floorBattleEndTerminateScripts();
    extern u32 lbl_8047B680;
    extern int soundGetCurrentSubSound();
    extern int std228_soundGetCurrentBackgroundMusic();
    extern u32 soundGetSlotVoiceState(u32 id);
    extern u32 std104_soundSetBackgroundMusic(u32 id, u32 volume, u32 priority);
    extern void scriptSoundStop();
    extern u32 std105_soundPlayEffect(u32 id, u32 volume, u32 priority);
    extern void wazaSequenceSysInitEx();
    extern void wazaSequenceSysRelease();
    extern void d2presentPlayGiftScene();
    u32 uVar5;
    u8 uVar4;
    u8 uVar3;
    u32 iVar2;
    u32 iVar1;

    uVar5 = lbl_8047B680;
    iVar1 = std228_soundGetCurrentBackgroundMusic();
    if (iVar1 != 0) {
        uVar3 = soundGetSlotVoiceState(iVar1) & 0xFFFFFFFFFFFFFFFFu;
        std104_soundSetBackgroundMusic(1, 0x32, 0xff);
    } else {
        uVar3 = 0;
    }
    iVar2 = soundGetCurrentSubSound();
    if (iVar2 != 0) {
        uVar4 = soundGetSlotVoiceState(iVar2) & 0xFFFFFFFFFFFFFFFFu;
        scriptSoundStop(0x32);
    } else {
        uVar4 = 0;
    }
    wazaSequenceSysInitEx(1);
    d2presentPlayGiftScene(uVar5);
    wazaSequenceSysRelease();
    if (iVar1 != 0) {
        std104_soundSetBackgroundMusic(iVar1, 0x32, uVar3);
    }
    if (iVar2 != 0) {
        std105_soundPlayEffect(iVar2, 0x32, uVar4);
    }
    floorBattleEndTerminateScripts();
    floorSetFadeScript(0, 0x5960008);
}

/* Address: 0x80266320 | Size: 0x3C */
void d2presentOpen(int r3)
{
    extern void std098_mapWarpToMenuRoom(u32);
    extern u32 lbl_8047B680;

    lbl_8047B680 = r3;
    std098_mapWarpToMenuRoom(0x8f);
    floorSetFadeScript((0x596 << 16) | 0x9, 0);
    _threadSwitch();
}
#endif
