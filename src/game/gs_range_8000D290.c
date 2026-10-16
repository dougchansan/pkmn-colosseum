/**
 * @file gs_range_8000D290.c
 * @brief gs-engine code, 0x8000D290 - 0x8000DAA8 (4 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

/* 0x8000D290 */
u32 fn_8000D290(void) {
    return 1;
}

extern void winSpriteSetDisp(void*, s32);
extern u8* menuItemBiosGetPtr(s16);
extern void windowDrawSprite(s32, s32, void*, u16, s32);
extern s32 menuGetCursorItemID(void*);
extern u32 GSmsgGetRect(void*);
extern void fn_800FB680(s16, s16, u32, void*);

void fn_8000D298(u8* menu, u8* item)
{
    u8* item_data;
    s16 x;
    s16 y;
    u32 color;

    winSpriteSetDisp(item, 0);
    item_data = menuItemBiosGetPtr(*(s16*)(item + 6));
    if (item_data != NULL) {
        windowDrawSprite(0, 0, menu, *(s16*)(item_data + 0xA), 0);
    }

    if (*(void**)(item + 0x4C) != NULL) {
        if (*(s16*)(item + 6) == menuGetCursorItemID(*(void**)(menu + 4))) {
            color = 0x1EF00000;
        } else {
            color = 0x1E525F00;
        }
        x = (*(s16*)(item + 0x54) -
             (s16)(GSmsgGetRect(*(void**)(item + 0x4C)) >> 16)) / 2;
        y = (*(s16*)(item + 0x56) -
             (s16)(u16)GSmsgGetRect(*(void**)(item + 0x4C))) / 2 - 2;
        fn_800FB680(x, y, color | menu[0x8B], *(void**)(item + 0x4C));
    }
}

typedef struct MenuSequenceMessageState {
    u8 pad0;
    s8 mode;
    s8 initialized;
    u8 pad3;
    s32 menuId;
    u8 pad8[0x8D];
    s8 selection;
    u8 pad96;
    s8 previousSelection;
} MenuSequenceMessageState;

s32 fn_8000D3AC(MenuSequenceMessageState* state)
{
    extern void winSeqSetMenu(s32 menuId, s32 state);
    extern void fn_801081F8(void* context, u32 messageId, s32 action);

    switch (state->mode) {
    case 0:
        if (state->initialized == 0) {
            winSeqSetMenu(state->menuId, 1);
            if (state->menuId == 0x41) {
                switch (state->selection) {
                case 0:
                    fn_801081F8(state, 0x65, 0x10);
                    break;
                case 1:
                    fn_801081F8(state, 0xDB0, 0x10);
                    break;
                case 2:
                    fn_801081F8(state, 0xDB1, 0x10);
                    break;
                case 3:
                    fn_801081F8(state, 0xDB2, 0x10);
                    break;
                }
            } else if (state->menuId == 0x109) {
                switch (state->selection) {
                case 0:
                    fn_801081F8(state, 0x12A9, 0x10);
                    break;
                case 1:
                    fn_801081F8(state, 0x12AA, 0x10);
                    break;
                case 2:
                    fn_801081F8(state, 0x12AB, 0x10);
                    break;
                }
            }
            state->initialized = 1;
        }
        break;
    case 2:
        if (state->previousSelection != state->selection) {
            if (state->menuId == 0x41) {
                switch (state->selection) {
                case 0:
                    fn_801081F8(state, 0x65, 0x10);
                    break;
                case 1:
                    fn_801081F8(state, 0xDB0, 0x10);
                    break;
                case 2:
                    fn_801081F8(state, 0xDB1, 0x10);
                    break;
                case 3:
                    fn_801081F8(state, 0xDB2, 0x10);
                    break;
                }
                switch (state->previousSelection) {
                case 0:
                    fn_801081F8(state, 0x65, 0x15);
                    break;
                case 1:
                    fn_801081F8(state, 0xDB0, 0x15);
                    break;
                case 2:
                    fn_801081F8(state, 0xDB1, 0x15);
                    break;
                case 3:
                    fn_801081F8(state, 0xDB2, 0x15);
                    break;
                }
            } else if (state->menuId == 0x109) {
                switch (state->selection) {
                case 0:
                    fn_801081F8(state, 0x12A9, 0x10);
                    break;
                case 1:
                    fn_801081F8(state, 0x12AA, 0x10);
                    break;
                case 2:
                    fn_801081F8(state, 0x12AB, 0x10);
                    break;
                }
                switch (state->previousSelection) {
                case 0:
                    fn_801081F8(state, 0x12A9, 0x15);
                    break;
                case 1:
                    fn_801081F8(state, 0x12AA, 0x15);
                    break;
                case 2:
                    fn_801081F8(state, 0x12AB, 0x15);
                    break;
                }
            }
        }
        break;
    case 3:
        if (state->initialized == 0) {
            winSeqSetMenu(state->menuId, 7);
            state->initialized = 1;
        }
        break;
    }
    return 0;
}

typedef struct MenuPendingEvent {
    u8 pad0[0x30];
    u16 eventId;
    u8 pad32[2];
} MenuPendingEvent;

s32 fn_8000D710(u8 mode)
{
    extern s32 heroMoveCheckEvent(MenuPendingEvent* events);
    extern void heroDispatchEvent(s8 kind, u32 eventId);
    extern void heroMoveInitEvent(void);
    extern void mailMainReceiveTerminate(void);
    extern void _threadSwitch(void);
    extern void GSmodelAllPauseAnimation(void);
    extern void GSmodelAllUnpauseAnimation(void);
    extern s32 fn_800D37CC(void);
    extern void menuCreateOffScreen(f32 rate);
    extern u8 menuGetOffScreenFlag(void);
    extern void soundDuck(s32, s32, s32, s32);
    extern void fn_800D3074(s32);
    extern u32 std082_flagGetValue(u32 flag);
    extern void* floorGetCurrentContext(void);
    extern u32 GSeffectGetUpdateGapp(void);
    extern void GSgappBlock(u32 task);
    extern void GSgappUnblock(u32 task);
    extern void GSthreadBlockGroup(void* stack);
    extern void GSthreadCreate(s32 priority, void* stack, u32 stackSize,
                               s32 usesFpu, s32 group, void* entry);
    extern void fn_8000DAB0(void);
    extern void menuClose(s32 menuId);
    extern void menuPokemonOpen(s32, s32, s32);
    extern void menuPdaOpen(void);
    extern u16 fn_80018F88(s32, u32* value, s32);
    extern u32 fn_80019070(u16 handle);
    extern void heroMoveAddAutoEvent(u32 eventId, u32 handle, u32 value,
                                     s32, s32);
    extern s32 std223_askYesNoThenWarp395(void);
    extern void menuOffScreenRelease(void);
    extern void soundUnduck(s32, s32, s32);
    extern void menuReleaseOffScreen(f32 rate);
    extern void menuCloseSync(s32 menuId, s32 wait);
    extern void heroMoveTermEvent(void);
    extern u8 lbl_8047A2A0;
    extern s32 lbl_8047A2A4;
    extern void* lbl_8047A2A8;
    extern void* lbl_8047A2AC;
    extern s32 lbl_8047A2B0;
    extern const f32 lbl_8047B6F0;
    MenuPendingEvent events[4];
    u32 eventValue;
    u16 selectionHandle;
    u32 eventId;
    s32 eventCount;
    s32 menuId;
    s32 i;

    eventCount = heroMoveCheckEvent(events);
    for (i = 0; i < eventCount; i++) {
        heroDispatchEvent(4, events[i].eventId);
    }
    heroMoveInitEvent();
    mailMainReceiveTerminate();
    _threadSwitch();
    GSmodelAllPauseAnimation();
    lbl_8047A2A0 = 1;
    menuCreateOffScreen(lbl_8047B6F0 / (f32)fn_800D37CC());
    while (!menuGetOffScreenFlag()) {
        _threadSwitch();
    }

    soundDuck(0x55, 0x1F4, 1, 1);
    fn_800D3074(1);
    if (std082_flagGetValue(0x8AE) == 0) {
        menuId = 0x41;
    } else {
        menuId = 0x109;
    }

    switch (mode) {
    case 0:
        if (std082_flagGetValue(0x8AE) == 0) {
            for (;;) {
                fn_800D3074(1);
                lbl_8047A2A4 = menuId;
                lbl_8047A2A8 = floorGetCurrentContext();
                GSgappBlock(GSeffectGetUpdateGapp());
                GSthreadBlockGroup(lbl_8047A2A8);
                lbl_8047A2AC = (u8*)lbl_8047A2A8 - 0x20;
                GSthreadCreate(0xF, lbl_8047A2AC, 0x4000, 1, 1,
                               fn_8000DAB0);
                _threadSwitch();
                GSgappUnblock(GSeffectGetUpdateGapp());

                if (lbl_8047A2B0 == 0) {
                    menuClose(menuId);
                    GSmodelAllUnpauseAnimation();
                    menuPokemonOpen(1, 0, 0);
                    GSmodelAllPauseAnimation();
                } else if (lbl_8047A2B0 == 1) {
                    menuClose(menuId);
                    GSmodelAllUnpauseAnimation();
                    menuPdaOpen();
                    GSmodelAllPauseAnimation();
                } else if (lbl_8047A2B0 == 2) {
                    menuClose(menuId);
                    GSmodelAllUnpauseAnimation();
                    selectionHandle = fn_80018F88(0, &eventValue, 0);
                    GSmodelAllPauseAnimation();
                    if (selectionHandle != 0) {
                        eventId = fn_80019070(selectionHandle);
                        if (eventId != -1) {
                            heroMoveAddAutoEvent(eventId, selectionHandle,
                                                 eventValue, 0, 0);
                        }
                        break;
                    }
                } else {
                    break;
                }
            }
        } else {
            for (;;) {
                fn_800D3074(1);
                lbl_8047A2A4 = menuId;
                lbl_8047A2A8 = floorGetCurrentContext();
                GSgappBlock(GSeffectGetUpdateGapp());
                GSthreadBlockGroup(lbl_8047A2A8);
                lbl_8047A2AC = (u8*)lbl_8047A2A8 - 0x20;
                GSthreadCreate(0xF, lbl_8047A2AC, 0x4000, 1, 1,
                               fn_8000DAB0);
                _threadSwitch();
                GSgappUnblock(GSeffectGetUpdateGapp());

                if (lbl_8047A2B0 == 0) {
                    menuClose(menuId);
                    menuPokemonOpen(1, 0, 0);
                } else if (lbl_8047A2B0 == 1) {
                    if (std223_askYesNoThenWarp395() == 1) {
                        menuOffScreenRelease();
                        lbl_8047A2A0 = 0;
                        _threadSwitch();
                    }
                } else {
                    break;
                }
            }
        }
        break;
    case 1:
        selectionHandle = fn_80018F88(0, &eventValue, 0);
        if (selectionHandle != 0) {
            eventId = fn_80019070(selectionHandle);
            if (eventId != -1) {
                heroMoveAddAutoEvent(eventId, selectionHandle, eventValue,
                                     0, 0);
            }
        }
        break;
    case 2:
        break;
    }

    fn_80019070(selectionHandle);
    soundUnduck(0x1F4, 1, 1);
    menuClose(menuId);
    menuReleaseOffScreen(lbl_8047B6F0 / (f32)fn_800D37CC());
    menuCloseSync(menuId, 1);
    fn_800D3074(2);
    heroMoveTermEvent();
    GSmodelAllUnpauseAnimation();
    lbl_8047A2A0 = 0;
    return 0;
}
