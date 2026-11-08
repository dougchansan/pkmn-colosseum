#ifndef GAME_HERO_MOVE_H
#define GAME_HERO_MOVE_H

#include "dolphin/types.h"

/*
 * Field hero-move state (.bss lbl_80426BD0, 0x420 bytes).
 *
 * Layout derived from the hero_move text (0x8012AC9C-0x80130660):
 * - leader: s32 compared signed against member indices and used as
 *   member[leader] (partnerSetFollowState stores its argument there with stwu).
 * - member[2], stride 0x20 from +0x04: bit 0 of the u16 flags marks a party
 *   member (heroMoveIsMember, heroMoveDismissMember clears it with
 *   rlwinm 16,30); spacing is the f32 follow distance written by the
 *   spacing pass; neckMode is the 32-bit heroMoveSetNeckMode state (0/1,
 *   2 when not a member); timer is reset to 300 by initFloor and
 *   counted down in fn_8012DE94; the four f32s are zeroed by initFloor.
 * - history: 20-entry ring of positions (head +0x44, count +0x48 capped
 *   at 20, entries +0x4C stride 12).
 * - stepAccum +0x13C: f32 step accumulator drained by heroMoveMain.
 * - stepCallback[8] +0x140: heroMoveAddStepCallback slots; heroMoveMain
 *   calls func(arg) for every non-NULL slot.
 * - poisonSteps +0x180 / friendSteps +0x184: step counters of
 *   cbPoison and cbTsureFriend.
 * - lockFrame +0x188: heroMoveSetLockFrame, decremented by heroMoveMain.
 * - autoEvent[5] +0x18C: heroMoveAddAutoEvent arguments.
 * - eventList[3] +0x1A0 (0xD0 bytes each) and eventValue[3] +0x410:
 *   heroMoveSetEventList copies a list per type and stores its value.
 */

/* Foot heights of a member's four feet (XD FOOTWORK, from the mangled
 * getStep__FP8FOOTSTEPP8_GSmodelPiP8FOOTWORK and initFootWork__FP8FOOTWORK). */
typedef struct FOOTWORK {
    f32 height[4];
} FOOTWORK;

typedef enum HeroMoveNeckMode {
    HERO_MOVE_NECK_OFF = 0,
    HERO_MOVE_NECK_ON = 1,
    HERO_MOVE_NECK_NONE = 2
} HeroMoveNeckMode;

typedef struct HeroMoveMember {
    /* 0x00 */ u16 flags;
    /* 0x04 */ f32 spacing;
    /* 0x08 */ HeroMoveNeckMode neckMode;
    /* 0x0C */ s32 timer;
    /* 0x10 */ FOOTWORK footwork;
} HeroMoveMember; /* size 0x20 */

typedef struct HeroMoveVec {
    f32 x, y, z;
} HeroMoveVec;

/* One entry of an event list (GScolsys2CheckGetEventID output, 0x34 bytes);
 * heroMoveMain passes id to fn_80116D30. */
typedef struct HeroMoveEvent {
    /* 0x00 */ u8 unk00[0x30];
    /* 0x30 */ u16 id;
    /* 0x32 */ u8 pad32[2];
} HeroMoveEvent; /* size 0x34 */

typedef struct HeroMoveStepCallback {
    /* 0x00 */ void (*func)(s32 arg);
    /* 0x04 */ s32 arg;
} HeroMoveStepCallback;

typedef struct HeroMoveWork {
    /* 0x000 */ s32 leader;
    /* 0x004 */ HeroMoveMember member[2];
    /* 0x044 */ u32 historyHead;
    /* 0x048 */ u32 historyCount;
    /* 0x04C */ HeroMoveVec history[20];
    /* 0x13C */ f32 stepAccum;
    /* 0x140 */ HeroMoveStepCallback stepCallback[8];
    /* 0x180 */ s32 poisonSteps;
    /* 0x184 */ s32 friendSteps;
    /* 0x188 */ s32 lockFrame;
    /* 0x18C */ u32 autoEvent[5];
    /* 0x1A0 */ HeroMoveEvent eventList[3][4];
    /* 0x410 */ s32 eventValue[3];
    /* 0x41C */ u8 pad41C[4];
} HeroMoveWork; /* size 0x420 */

#endif /* GAME_HERO_MOVE_H */
