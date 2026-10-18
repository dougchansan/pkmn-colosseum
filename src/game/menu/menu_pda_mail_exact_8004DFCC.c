#include "dolphin/types.h"

extern u8 lbl_8047A524;
extern u32 lbl_8047A528;
extern u32 lbl_8047A52C;

extern u32 soundPauseMusic(void);
extern u32 GSresAllocResourceAlign(u32 size, u32 align, u32 arg2, u32 group, u32 arg4);
extern void GSresRegisterResource(u32 buffer, u32 arg1, u32 group, u32 arg3);
extern void fn_800F9210(u32 arg0, u32 group);
extern void soundResumeMusic(u32 state);
extern u32 mailGetEntryField0(u8 index);
extern void fn_801654E0(u32 id, u32 buffer, u32 size);
extern void soundAttachWork(u32 id, u32 arg1, u32 arg2);
extern void soundStart(s32 id, s32 a, s32 b, s32 c);
extern s32 menuOpenCustom(s32 menuId, ...);
extern u32 windowGetActiveID(void);
extern s32 mailGetMailCount(void);
extern void menuClose(s32 menuId);
extern void menuCloseSync(s32 menuId, s32 flag);
extern void soundStopRaw(u32 id, u32 arg1, u32 arg2);
extern void GSsndGetStatus(u32 id);
extern void soundKill(u32 id);

/* RULE-EXCEPTION(user-approved): local peephole setting preserves this callback's codegen; see docs/RULE_EXCEPTIONS.md. */
#pragma peephole off
u8 pdaMailPickerRun(u8 initialSelection)
{
    s32 selection;
    u32 state;
    u32 soundId;
    u8 result;
    s32 choice;

    selection = initialSelection;
    state = soundPauseMusic();
    lbl_8047A52C = GSresAllocResourceAlign(0x10000, 0x20, 0, 0x408, 0);
    GSresRegisterResource(lbl_8047A52C, 0, 0x408, 0);
    lbl_8047A524 = initialSelection;
    soundId = mailGetEntryField0(initialSelection);
    if (soundId != 0) {
        fn_801654E0(soundId, lbl_8047A52C, 0x10000);
        soundAttachWork(soundId, 0, 0x408);
        soundStart(soundId, 0, 0xFF, 0);
        lbl_8047A528 = 0;
    }

    choice = menuOpenCustom(0x76, windowGetActiveID(), &selection, 0, 1, 0);
    if (choice < 0 || choice >= mailGetMailCount()) {
        result = 0xFF;
    } else {
        result = choice;
    }
    menuClose(0x76);
    menuCloseSync(0x76, 1);

    soundId = mailGetEntryField0(lbl_8047A524);
    if (soundId != 0) {
        soundStopRaw(soundId, 0, 0);
    }
    soundId = mailGetEntryField0(lbl_8047A524);
    if (soundId != 0) {
        GSsndGetStatus(soundId);
    }
    soundId = mailGetEntryField0(lbl_8047A524);
    if (soundId != 0) {
        soundKill(soundId);
    }
    fn_800F9210(0, 0x408);
    soundResumeMusic(state);
    return result;
}
#pragma peephole reset
