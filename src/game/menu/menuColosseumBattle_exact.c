/**
 * @file menuColosseumBattle_exact.c
 * @brief Byte-exact menuColosseumBattle.cpp range, 0x80058754 - 0x80058DCC.
 */
#include "dolphin/types.h"

typedef struct MenuKeyInfo {
    u8 pad_00[4];
    u16 buttons;
    u16 repeatButtons;
} MenuKeyInfo;

typedef struct MenuWindow {
    u8 pad_00[0x4C];
    s32 nextMenu;
    u8 pad_50[0x10];
    void* partyState;
    u8 pad_64[0x31];
    s8 cursor;
    u8 pad_96;
    s8 previousCursor;
    u8 accepted;
    u8 canceled;
} MenuWindow;

typedef struct MenuCursorItem {
    u8 pad_00[2];
    s16 field_02;
    s16 field_04;
} MenuCursorItem;

typedef struct ColosseumBattleConnectState {
    s32 active;
    s32 connected;
    s32 busyRequest;
    s32 reserved;
} ColosseumBattleConnectState;

typedef struct ColosseumMessageBuffer {
    u16 text[0x24];
} ColosseumMessageBuffer;

typedef struct ColosseumRosterRow {
    s32 mode;
    s32 menuId;
    s32 partyIndex;
    s32 action;
} ColosseumRosterRow;

typedef struct ColosseumPokemonBlob {
    u8 bytes[0x138];
} ColosseumPokemonBlob;

s32 fn_800566E8(void);
extern s32 fn_800573C0(void);
s32 fn_80057694(void);
void fn_800576A4(s32);
void fn_80057830(s32, s32, s32);
extern void msgctrlSetValue();
extern void fn_800F96E4(void*, s32, void*);
extern void menuCloseCustom();
extern s32 menuOpen(s32, s32);
MenuCursorItem* windowGetCursorToItem(MenuWindow*);
extern u8* windowGetKeyInfo(void);
extern u8 pokemonCheckValid(void*);
extern void menuButtonNormal(void*);
extern void* heroGetStatus(void*, s32, u16);
extern s32 fn_800576B4(void);
extern s32 fn_80057538(void);
extern void* fn_800574E0(void);
extern void fn_800574FC(void*);
extern void fn_80057400(void);
extern void fn_800576C4(s32);
extern void pokemonInit(void*);
extern void fn_800574A8(void);

extern s32 lbl_8047A598;
extern s32 lbl_8047A59C;
extern const ColosseumRosterRow lbl_802677D0[7];
extern u8 lbl_803A9A18[];

/* Address: 0x80058754 | Size: 0x44 */
#pragma push
#pragma peephole off
s32 fn_80058754(void* unused, MenuWindow* window) {
    ColosseumMessageBuffer* message = (ColosseumMessageBuffer*)lbl_803A9A18;

    (void)unused;
    msgctrlSetValue(0x37, message);
    window->nextMenu = 0xCE;
    return 0;
}
#pragma pop

/* Address: 0x80058798 | Size: 0x40 */
void fn_80058798(MenuWindow* window) {
    MenuKeyInfo* keys = (MenuKeyInfo*)windowGetKeyInfo();

    if (keys->buttons != 0) {
        window->accepted = 1;
    }
}

/* Address: 0x800587D8 | Size: 0x2C */
#pragma push
#pragma scheduling off
void fn_800587D8(void) {
    menuCloseCustom(0x9B, 2, 1);
}
#pragma pop

/* Address: 0x80058804 | Size: 0x7C */
#pragma push
#pragma peephole off
void fn_80058804(void* text, s32 closeAfterOpen) {
    if (text == NULL) {
        ColosseumMessageBuffer* message = (ColosseumMessageBuffer*)lbl_803A9A18;

        message->text[0] = 0;
    } else {
        ColosseumMessageBuffer* message = (ColosseumMessageBuffer*)lbl_803A9A18;

        fn_800F96E4(message, 0x21, text);
    }

    menuOpen(0x9B, (u8)closeAfterOpen);
    if (closeAfterOpen != 0) {
        menuCloseCustom(0x9B, 2, 1);
    }
}
#pragma pop

/* Address: 0x80058880 | Size: 0x230 */
#pragma push
#pragma peephole off
s32 fn_80058880(MenuWindow* window) {
    MenuKeyInfo* keys;
    s32 cursor;
    s32 action;

    keys = (MenuKeyInfo*)windowGetKeyInfo();
    if (fn_800573C0() != 0) {
        return 0;
    }
    if (fn_800566E8() != 0) {
        return 0;
    }

    if ((keys->repeatButtons & 0xC0) != 0) {
        if (fn_80057694() != 0) {
            cursor = 0;
        } else {
            cursor = 1;
        }
        fn_800576A4(cursor);
    }

    if ((keys->repeatButtons & 2) != 0) {
        if (++window->cursor >= 7) {
            window->cursor = 0;
        }
    }

    if ((keys->repeatButtons & 1) != 0) {
        if (--window->cursor < 0) {
            window->cursor = 6;
        }
    }

    if (keys->repeatButtons == 8) {
        cursor = window->cursor;
        if (cursor < 0 || cursor >= 7) {
            action = 2;
        } else {
            action = lbl_802677D0[cursor].action;
        }

        switch (action) {
        case 0:
            window->cursor = (s8)lbl_8047A598;
            break;
        case 1:
            window->accepted = 1;
            window->canceled = 1;
            break;
        }
    }

    if (keys->repeatButtons == 4) {
        cursor = window->cursor;
        if (cursor < 0 || cursor >= 7) {
            action = 2;
        } else {
            action = lbl_802677D0[cursor].action;
        }

        if (action == 1) {
            window->cursor = (s8)lbl_8047A59C;
        }
    }

    cursor = (u8)window->cursor;
    if ((s8)cursor != window->previousCursor) {
        cursor = (s8)cursor;
        if (cursor < 0 || cursor >= 7) {
            action = 2;
        } else {
            action = lbl_802677D0[cursor].action;
        }

        if (action == 0) {
            lbl_8047A59C = cursor;
        }
        if (action != 0) {
            lbl_8047A598 = cursor;
        }
        {
            MenuCursorItem* item = windowGetCursorToItem(window);
            fn_80057830(item->field_02, item->field_04, 0);
        }
    }
    return 0;
}
#pragma pop

/* Address: 0x80058AB0 | Size: 0x40 */
void fn_80058AB0(MenuWindow* window) {
    windowGetKeyInfo();
    if (fn_800573C0() == 0) {
        menuButtonNormal(window);
    }
}

/* Address: 0x80058AF0 | Size: 0x2DC */
#pragma push
#pragma peephole off
static inline void fn_80058AF0_impl(MenuWindow* window,
                                    ColosseumBattleConnectState* state,
                                    u32* result) {
    s32 partyIndex;
    ColosseumPokemonBlob* pokemon;
    s32 i;
    ColosseumPokemonBlob* previousPokemon;
    ColosseumPokemonBlob* alternatePokemon;
    s32 foundEmpty;

    *result = 1;
    switch (fn_800576B4()) {
    case 1:
        if (fn_80057538() == 0) {
            break;
        }
        if (window->cursor >= 0 && window->cursor < 7) {
            partyIndex = lbl_802677D0[window->cursor].partyIndex;
        }
        fn_800574FC(heroGetStatus(NULL, 3, (u16)partyIndex));
        if (partyIndex >= 0 && partyIndex < 6) {
            for (i = partyIndex + 1; i < 6; i++) {
                pokemon = heroGetStatus(NULL, 3, (u16)i);
                if (pokemon == NULL) {
                    break;
                }
                previousPokemon = heroGetStatus(NULL, 3, (u16)(i - 1));
                if (previousPokemon == NULL) {
                    break;
                }
                *previousPokemon = *pokemon;
            }
            if (i >= 6) {
                pokemonInit(heroGetStatus(NULL, 3, 5));
            }
        }
        fn_800576C4(2);
        break;
    case 2:
        if (fn_80057538() != 0) {
            fn_800576C4(3);
        }
        break;
    case 3:
        if (state->active != 0) {
            fn_800576C4(4);
        }
        break;
    case 4:
        if (fn_80057538() == 0) {
            break;
        }
        alternatePokemon = fn_800574E0();
        foundEmpty = 0;
        for (i = 0; i < 6 && foundEmpty == 0; i++) {
            pokemon = heroGetStatus(NULL, 3, (u16)i);
            if (pokemon == NULL) {
                break;
            }
            if (pokemonCheckValid(pokemon) == 0) {
                foundEmpty = 1;
            }
        }
        if (foundEmpty != 0) {
            *pokemon = *alternatePokemon;
        }
        fn_800574A8();
        fn_800576C4(5);
        break;
    case 5:
        if (fn_80057538() != 0) {
            fn_800576C4(0);
            if (state->active != 0) {
                *result = 0;
            }
        }
        break;
    case 6:
        if (fn_80057538() == 0) {
            break;
        }
        if (window->cursor >= 0 && window->cursor < 7) {
            partyIndex = lbl_802677D0[window->cursor].partyIndex;
        }
        alternatePokemon = fn_800574E0();
        pokemon = heroGetStatus(NULL, 3, (u16)partyIndex);
        *pokemon = *alternatePokemon;
        fn_800574A8();
        fn_80057400();
        fn_800576C4(3);
        break;
    }
}

u32 fn_80058AF0(MenuWindow* window) {
    ColosseumBattleConnectState* state;
    u32 result;

    state = (ColosseumBattleConnectState*)window->partyState;
    fn_80058AF0_impl(window, state, &result);
    return result;
}
#pragma pop
