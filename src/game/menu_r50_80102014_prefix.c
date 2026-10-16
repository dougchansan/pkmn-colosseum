/**
 * @file menu_r50_80102014_prefix.c
 * @brief menu engine, 0x80102014-0x80102F38: off-screen helpers, cursor and
 *        item queries, open/close, and the per-frame menuDaemon.
 *
 * Standalone source for this split range, in retail address order. The
 * retail menu TU was built with the peephole optimizer off as a whole
 * (configure.py: -opt nopeephole); no function here needs a local
 * compiler-control pragma. -inline auto expands menuCloseSync,
 * menuCloseCustom and menuGetCursor into their later callers (menuClose,
 * menuCloseCustom, menuOpenCustom) as retail does.
 */
#include "dolphin/types.h"

typedef struct __va_list_struct {
    u8 gpr;
    u8 fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} __va_list_struct;
typedef __va_list_struct va_list[1];
#define va_start(ap, last) ((void)last, __builtin_va_info(&ap))
#define va_end(ap) ((void)0)

typedef struct MenuWindow MenuWindow;
typedef void (*MenuWindowCallback)(MenuWindow* window);

/* Visible cursor: row offset of the list plus the cursor within it. */
typedef struct MenuCursor {
    s8 base;
    s8 offset;
} MenuCursor;

/* Static menu definition returned by menuDataBiosGetPtr. */
typedef struct MenuData {
    u8 soundGroup;
    u8 actionType;
    u8 pad_02;
    u8 cursorSlot;
    s16 firstItem;
    u8 pad_06[0x06];
    MenuWindowCallback cursorCallback;
    MenuWindowCallback buttonCallback;
    MenuWindowCallback tickCallback;
    MenuWindowCallback drawCallback;
} MenuData;

/* Menu item definition returned by menuItemBiosGetPtr. */
typedef struct MenuItem {
    u8 selectable : 1;
    u8 last : 1;
    u8 pad_00 : 6;
    u8 pad_01[0x17];
    s16 next;
} MenuItem;

typedef struct MenuSprite {
    struct MenuSprite* next;
} MenuSprite;

/* Open window (tagWINDOW_WORK). */
struct MenuWindow {
    s8 flags;
    s8 phase;
    u8 phaseFrame;
    u8 pad_03;
    u32 id;
    u8 pad_08;
    s8 fadePriority;
    u8 transitionDone;
    u8 pad_0B[5];
    MenuWindow* next;
    u8 pad_14[5];
    s8 cursorCount;
    u8 cursorMode;
    u8 cursorFlags;
    MenuSprite* sprites;
    MenuSprite* overlaySprites;
    u8 pad_24[0x60];
    s16 x;
    s16 y;
    u8 pad_88[0x0C];
    MenuCursor cursor;
    MenuCursor previousCursor;
    u8 close;
    u8 back;
};

typedef struct MenuKeyInfo {
    u16 keys;
    u16 previousKeys;
    u16 pressedKeys;
    u16 repeatKeys;
    u16 heldKeys;
} MenuKeyInfo;

/* Window system work (WINDOW_SYS_WORK). */
typedef struct MenuSystem {
    u8 pad_00[0x0C];
    MenuWindow* head;
    u8 pad_10[0x84];
    u32 cursorChanged;
    u32 activeId;
} MenuSystem;

extern MenuSystem lbl_80404ACC;

/* Shared menu constants owned by game/gs_model_sdata2_8047CD98. */
extern const f32 lbl_8047CDC0;
extern const f32 lbl_8047CDC4;

/* GSlogWrite format and module name for the close-wait timeout. */
extern const char lbl_80271E10[];
extern const char lbl_8035B060[];

extern void GSlogWrite(const char* fmt, ...);
extern u32 GSthreadGetCurrentThread(void);
extern void _threadSwitch(void);
extern u32 GSgetFrameStep(void);
extern void fn_800D9E4C(s32);
extern void fn_800D9ED8(s32);
extern void gfxSetBlendMode(s32, s32, s32);
extern void fn_801D2404(void);
extern void fadeDaemon(void);
extern void fn_800FE6D0();
extern void fn_800FE35C(void);
extern void spriteSetEnv(void);
extern u16 fn_8005D798(void*, s32);
extern void cmsRunSync(void*, u32, ...);
extern int soundStartSE(u16);

extern const MenuData* menuDataBiosGetPtr(u32 id);
extern const MenuItem* menuItemBiosGetPtr(u32 itemId);
extern void* menuSeBiosGetPtr(s32 group);
extern u32 cursorBiosSetPos(u16 slot, MenuCursor* cursor);

extern u8 menuOffScreenCheckEnable(u8 param);
extern void menuOffScreenFadeSet(f32 from, f32 to);
extern u8 menuOffScreenFadeSync(u8 param);
extern void menuOffScreenRelease(void);
extern u8 menuOffScreenCreate(u32 param);
extern u8 menuOffScreenSetDisp(u8 val);
extern u8 menuOffScreenSetPriority(u8 val);

extern MenuWindow* windowSearchID(u32 id);
extern u32 windowGetActiveID(void);
extern MenuKeyInfo* windowGetKeyInfo(void);
extern void windowClose(MenuWindow* window, u32 mode);
extern void windowCloseMain(MenuWindow* window);
extern MenuWindow* windowOpen(s32* cursor_out, u32 menu_id, u32 parent_id,
                              s32 close_flags, s32 open_param, va_list args);
extern void windowCheckCursor(u32 id, u8 check);
extern s32 windowGetValue(u32 id);
extern void windowCreateCursorSprite(MenuWindow* window);
extern void winSpriteSetDisp(MenuSprite* sprite, u32 enable);
extern void winSpriteDraw(MenuWindow* window, MenuSprite* sprite);
extern void _menuUpdateKeyInfo__FP15WINDOW_SYS_WORK(MenuSystem* system);
extern void fn_801093C8(void);
extern void fn_80106F98(u32 id);
extern s32 winSeqCheckMove(u32 id);
extern void winSeqMoveMenu(MenuWindow* window);
extern void menuCursorNormal(MenuWindow* window);

/* 0x80102014 | 0x24 */
void menuGetOffScreenFlag(void) {
    menuOffScreenCheckEnable(0);
}

/* 0x80102038 | 0x34 */
void menuReleaseOffScreen(f32 time) {
    menuOffScreenFadeSet(lbl_8047CDC0, time);
    menuOffScreenFadeSync(1);
    menuOffScreenRelease();
}

/* 0x8010206C | 0x54 */
void menuCreateOffScreen(f32 time) {
    menuOffScreenCreate(1);
    menuOffScreenSetDisp(1);
    menuOffScreenSetPriority(0);
    menuOffScreenFadeSet(lbl_8047CDC4, time);
}

/* 0x801020C0 | 0x78 */
s32 menuGetSelectItemNum(u32 id) {
    s32 count = 0;
    const MenuData* data;
    const MenuItem* item;
    s16 index;

    data = menuDataBiosGetPtr(id);
    if (data == NULL) {
        return 0;
    }

    index = data->firstItem;
    for (;;) {
        item = menuItemBiosGetPtr(index);
        if (item->selectable) {
            count++;
        }
        if (item->last) {
            break;
        }
        index = item->next;
    }
    return count;
}

/* 0x80102138 | 0xC0 */
s32 menuGetCursorFromItemID(u32 id, u32 itemId) {
    const MenuData* data;
    const MenuItem* item;
    s16 index;
    s32 cursor;

    data = menuDataBiosGetPtr(id);
    if (data == NULL) {
        return -3;
    }

    index = data->firstItem;
    cursor = 0;
    for (;;) {
        item = menuItemBiosGetPtr(index);
        if (index == itemId) {
            if (item->selectable) {
                return cursor;
            }
            return -1;
        }
        if (item->selectable) {
            cursor++;
        }
        if (item->last) {
            break;
        }
        index = item->next;
    }
    return -2;
}

/* 0x801021F8 | 0x5C */
void fn_801021F8(u32 id, u32 enable) {
    MenuWindow* window = windowSearchID(id);
    MenuSprite* sprite;

    if (window == NULL) {
        return;
    }
    for (sprite = window->overlaySprites; sprite != NULL; sprite = sprite->next) {
        winSpriteSetDisp(sprite, enable);
    }
}

/* 0x80102254 | 0x64 */
void menuSetDisp(u32 id, u8 enable) {
    MenuWindow* window = windowSearchID(id);

    if (window == NULL) {
        return;
    }
    if (enable) {
        window->flags |= 2;
    } else {
        window->flags &= ~2;
    }
}

/* 0x801022B8 | 0xE0 */
s32 menuGetCursorItemID(u32 id) {
    MenuWindow* window;
    const MenuData* data;
    const MenuItem* item;
    s32 cursor;
    s16 index;
    s32 count;

    window = windowSearchID(id);
    if (window != NULL) {
        cursor = window->cursor.offset + window->cursor.base;
    } else {
        cursor = -1;
    }
    if (cursor == -1) {
        return 0;
    }

    data = menuDataBiosGetPtr(id);
    if (data == NULL) {
        return 0;
    }

    index = data->firstItem;
    count = 0;
    for (;;) {
        item = menuItemBiosGetPtr(index);
        if (item->selectable) {
            if (count == cursor) {
                return index;
            }
            count++;
        }
        if (item->last) {
            break;
        }
        index = item->next;
    }
    return 0;
}

/* 0x80102398 | 0x4C */
s32 menuSetCursor(u32 id, s32 cursor) {
    MenuWindow* window = windowSearchID(id);

    if (window != NULL) {
        window->cursor.offset = cursor;
    } else {
        return -1;
    }
    return 0;
}

/* 0x801023E4 | 0x44 */
s32 menuGetCursor(u32 id) {
    MenuWindow* window = windowSearchID(id);

    if (window != NULL) {
        return window->cursor.offset + window->cursor.base;
    }
    return -1;
}

/* 0x80102428 | 0x98 */
s32 menuCloseSync(u32 id, u8 wait) {
    if (wait) {
        for (;;) {
            if (windowSearchID(id) == NULL) {
                return 0;
            }
            if (GSthreadGetCurrentThread() == 0) {
                GSlogWrite(lbl_80271E10, lbl_8035B060, id);
                break;
            }
            _threadSwitch();
        }
    } else {
        return windowSearchID(id) != NULL;
    }
    return 0;
}

/* 0x801024C0 | 0x28 */
void menuCloseFloor(void) {
    windowClose(NULL, 4);
}

/* 0x801024E8 | 0x28 */
void fn_801024E8(void) {
    windowClose(NULL, 4);
}

/* 0x80102510 | 0x58 */
void menuClose(s32 id) {
    MenuWindow* window;

    if (id == 0) {
        id = windowGetActiveID();
    }
    window = windowSearchID(id);
    if (window != NULL) {
        windowClose(window, 0);
        menuCloseSync(id, 0);
    }
}

/* 0x80102568 | 0xB8 */
s32 menuCloseCustom(u32 id, u32 mode, u8 wait) {
    MenuWindow* window = windowSearchID(id);

    if (window == NULL) {
        return 1;
    }
    windowClose(window, mode);
    menuCloseSync(id, wait);
    return 0;
}

/* 0x80102620 | 0x2C */
s32 menuIsCheck(u32 id) {
    return windowSearchID(id) != NULL;
}

s32 menuOpenCustom(u32 menu_id, u32 parent_id, s32* cursor_out,
                   u32 close_flags, u8 check_cursor, s32 open_param, ...);

/* 0x8010264C | 0x58 */
void menuOpen(u32 menu_id, u8 check_cursor) {
    menuOpenCustom(menu_id, windowGetActiveID(), NULL, 0, check_cursor, 0);
}

/* 0x801026A4 | 0x1C4 */
s32 menuOpenCustom(u32 menu_id, u32 parent_id, s32* cursor_out,
                   u32 close_flags, u8 check_cursor, s32 open_param, ...) {
    va_list args;
    MenuWindow* window;
    const MenuData* data;
    MenuCursor cursor;
    s32 value = 0;

    va_start(args, open_param);
    window = windowOpen(cursor_out, menu_id, parent_id, close_flags,
                        open_param, args);

    if (check_cursor) {
        windowCheckCursor(menu_id, check_cursor);
        value = windowGetValue(menu_id);
        data = menuDataBiosGetPtr(menu_id);
        if (data->cursorSlot != 0) {
            cursor = window->cursor;
            cursorBiosSetPos(data->cursorSlot, &cursor);
        }
        if (cursor_out != NULL) {
            *cursor_out = menuGetCursor(menu_id);
        }
    }

    if (close_flags & 1) {
        menuCloseCustom(menu_id, 0, check_cursor);
    }
    return value;
}

/* 0x80102868 | 0x48 */
void menuSetPosition(u32 id, s16 x, s16 y) {
    MenuWindow* window = windowSearchID(id);

    if (window == NULL) {
        return;
    }
    window->x = x;
    window->y = y;
}

/* Plays the menu's sound effect for the given event, if it has one. */
#define MENU_PLAY_SE(window, event)                                         \
    do {                                                                    \
        void* sound = menuSeBiosGetPtr(                                     \
            menuDataBiosGetPtr((window)->id)->soundGroup & 7);              \
        if (sound != NULL) {                                                \
            u32 se = fn_8005D798(sound, (event));                           \
            if (se != 0) {                                                  \
                soundStartSE(se);                                            \
            }                                                               \
        }                                                                   \
    } while (0)

/* 0x801028B0 | 0x624 */
void menuDaemon(void) {
    MenuSystem* system;
    MenuWindow* active;
    MenuWindow* node;
    const MenuData* data;
    MenuSprite* sprite;
    u32 i;
    u16 keys;
    u8 doFade = 1;

    fn_801D2404();
    active = windowSearchID(windowGetActiveID());
    _menuUpdateKeyInfo__FP15WINDOW_SYS_WORK(&lbl_80404ACC);
    fn_800D9E4C(0);
    fn_801093C8();
    fn_800D9ED8(1);
    system = &lbl_80404ACC;

    for (node = system->head; node != NULL; node = node->next) {
        data = menuDataBiosGetPtr(node->id);
        switch (node->phase) {
        case 0:
            node->phase = 1;
            node->phaseFrame = 0;
            break;
        case 1:
            if ((u8)winSeqCheckMove(node->id) == 0) {
                if ((u8)((data->actionType >> 2) & 7) == 0) {
                    node->close = 1;
                }
                node->phase = 2;
                node->phaseFrame = 0;
            }
            break;
        case 2:
            if (node->transitionDone != 0) {
                MENU_PLAY_SE(node, 5);
                node->phase = 3;
                node->phaseFrame = 0;
            }
            break;
        case 3:
            if ((u8)winSeqCheckMove(node->id) == 0) {
                node->phase = 4;
                node->phaseFrame = 0;
            }
            break;
        case 4:
            break;
        }

        if (active != NULL && node == active) {
            if (node->close == 0) {
                switch (node->phase) {
                case 2:
                    switch ((data->actionType >> 2) & 7) {
                    case 0:
                        node->close = 1;
                        break;
                    case 1:
                        /* menuButtonNormal(node), expanded in place */
                        if (node != NULL) {
                            keys = windowGetKeyInfo()->pressedKeys;
                            if (keys & 0x10) {
                                node->close = 1;
                            }
                            if (keys & 0x20) {
                                node->close = 1;
                                node->back = 1;
                            }
                        }
                        break;
                    case 3:
                        cmsRunSync(data->buttonCallback, 0);
                        break;
                    case 4:
                        data->buttonCallback(node);
                        break;
                    }

                    if ((node->cursorFlags & 8) != 0 && node->back != 0) {
                        node->back = 0;
                        node->close = 0;
                    }

                    if (node->close != 0) {
                        system->cursorChanged = 0;
                        system->activeId = node->id;
                    } else if ((windowGetKeyInfo()->keys & 0x8000) != 0) {
                        system->cursorChanged = 1;
                        system->activeId = node->id;
                        if ((node->cursorFlags & 0x10) == 0) {
                            node->close = 1;
                            node->back = 1;
                        }
                    }
                    break;
                case 3:
                    break;
                }

                if (node->back != 0) {
                    MENU_PLAY_SE(node, 3);
                } else if (node->close != 0) {
                    MENU_PLAY_SE(node, 2);
                }
                if (node->close != 0) {
                    fn_80106F98(node->id);
                }
            }

            if (node->close == 0) {
                MenuCursor oldPos;
                MenuCursor newPos;
                u8 moved;

                switch (node->phase) {
                case 2:
                    node->previousCursor = node->cursor;
                    if (node->cursorCount > 0) {
                        switch (node->cursorMode) {
                        case 1:
                        case 2:
                            menuCursorNormal(node);
                            break;
                        case 3:
                            cmsRunSync(data->cursorCallback, 0);
                            break;
                        case 4:
                            data->cursorCallback(node);
                            break;
                        }
                        oldPos = node->previousCursor;
                        newPos = node->cursor;
                        if (oldPos.base == newPos.base &&
                            oldPos.offset == newPos.offset) {
                            moved = 0;
                        } else {
                            moved = 1;
                        }
                        if (moved) {
                            windowCreateCursorSprite(node);
                            MENU_PLAY_SE(node, 1);
                        }
                    }
                    break;
                }
            }
        }

        if ((node->flags & 4) != 0) {
            winSeqMoveMenu(node);
            if (data->tickCallback != NULL) {
                for (i = 0; i < GSgetFrameStep(); i++) {
                    data->tickCallback(node);
                }
            }
        }
    }

    for (node = system->head; node != NULL; node = node->next) {
        if (node->phase == 4) {
            windowCloseMain(node);
        }
    }

    for (node = system->head; node != NULL; node = node->next) {
        if (doFade != 0 && node->fadePriority >= 0x50) {
            doFade = 0;
            fadeDaemon();
            fn_800D9ED8(1);
        }
        if ((node->flags & 2) != 0) {
            fn_800FE6D0(node->x, node->y);
            fn_800FE35C();
            spriteSetEnv();
            for (sprite = node->sprites; sprite != NULL; sprite = sprite->next) {
                winSpriteDraw(node, sprite);
            }
            for (sprite = node->overlaySprites; sprite != NULL;
                 sprite = sprite->next) {
                winSpriteDraw(node, sprite);
            }
            data = menuDataBiosGetPtr(node->id);
            if (data->drawCallback != NULL) {
                data->drawCallback(node);
            }
        }
    }

    fn_800FE6D0(0, 0);
    fn_800FE35C();
    gfxSetBlendMode(1, 6, 7);
    if (doFade != 0) {
        fadeDaemon();
    }
    fn_800D9ED8(0);
    fn_800D9E4C(1);
}

/* 0x80102ED4 | 0x64 */
void menuButtonNormal(MenuWindow* window) {
    u16 keys;

    if (window == NULL) {
        return;
    }
    keys = windowGetKeyInfo()->pressedKeys;
    if (keys & 0x10) {
        window->close = 1;
    }
    if (keys & 0x20) {
        window->close = 1;
        window->back = 1;
    }
}
