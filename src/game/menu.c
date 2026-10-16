/**
 * @file menu.c
 * @brief menu engine: the whole retail menu TU (.text 0x80102004-0x80103E68)
 *        in address order, as one reconstruction.
 *
 * Built like the menu units in configure.py (GC/1.3.2, -O4,p, -opt
 * nopeephole, -rostr, no local pragmas). -rostr is what retail's data shows:
 * menuCloseSync's GSlogWrite format is in .rodata (0x80271E10) while its
 * __FUNCTION__ string "menuCloseSync" is in .data (0x8035B060).
 *
 * The linked units are exact function-boundary carves of this TU:
 * menu_get_last_error_exact_80102004.c, menu_r50_80102014_prefix.c,
 * menu_r50_80102F38_o3.c (menuCursorNormal), menu_exact_80103484.c,
 * menu_exact_80103614.c (_menuGetGcKeyInfo, with the pad-id .rodata and the
 * pool entries only it uses), menu_exact_801038F8.c (_menuUpdateKeyInfo)
 * and menu_exact_80103BA8.c. This file is the whole-TU reconstruction they
 * are cut from (menu_candidate_80102014.c includes it).
 * - menuCursorNormal walks the items through menuItemNext and declares its
 *   locals in the carve's order. Both are needed for retail's register
 *   assignment (see the carve's header).
 *
 * Compiled whole, GC/1.3.2's -inline auto expands _menuGetAgbKeyInfo into
 * _menuUpdateKeyInfo, where retail calls it; the carve, where it is only
 * declared, matches.
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

typedef struct tagWINDOW_WORK MenuWindow;
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
    u8 pad_01;
    s16 x;
    s16 y;
    u8 pad_06[0x12];
    s16 next;
} MenuItem;

typedef struct MenuSprite {
    struct MenuSprite* next;
} MenuSprite;

/* Open window. */
struct tagWINDOW_WORK {
    s8 flags;
    s8 phase;
    u8 phaseFrame;
    u8 pad_03;
    u32 id;
    u8 pad_08;
    s8 fadePriority;
    u8 transitionDone;
    u8 keyPort; /* input port(s) the window reads */
    u8 pad_0C[4];
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
    s8 repeatTimer[16];
} MenuKeyInfo;

/* Window system work. */
typedef struct WINDOW_SYS_WORK {
    u8 pad_00[0x0C];
    MenuWindow* head;
    MenuKeyInfo keyInfo;        /* keys of the active window's port(s) */
    MenuKeyInfo portKeyInfo[4]; /* per-port state, updated every frame */
    u8 enablePort;
    s32 lastError;
    u32 activeId;
} MenuSystem;

extern MenuSystem lbl_80404ACC;
extern u32 lbl_8047AD00;   /* menu texture-resource group, created once */
extern u8 lbl_8047AD04[4]; /* input type per port: 0 none, 1 GC pad, 2 AGB, 3 AGB lost */
extern u8 lbl_8047AD08[4]; /* the same, one frame earlier */
extern u8 lbl_803156E0[];
extern u8 lbl_803254E0[];
extern u8 lbl_803357E0[];

void* memset(void* dst, int val, u32 size);
double atan2(double y, double x);
void GSlogWrite(const char* fmt, ...);
u32 GSthreadGetCurrentThread(void);
void _threadSwitch(void);
u32 GSgetFrameStep(void);
void fn_800D9E4C(s32);
void fn_800D9ED8(s32);
void gfxSetBlendMode(s32, s32, s32);
void fn_801D2404(void);
void fadeDaemon(void);
void fn_800FE6D0(s32 x, s32 y);
void fn_800FE35C(void);
void spriteSetEnv(void);
u16 fn_8005D798(void*, s32);
void cmsRunSync(void*, u32, ...);
int soundStartSE(u16);
u8 fn_8008ABA0(s32);
s32 fn_8008AB8C(s32);
u8 fn_800F7EF8(s32 pad_id);
u32 fn_800F7A08(s32 pad_id, s32 axis);
u32 fn_800F7A7C(s32 pad_id, s32 axis);
u32 fn_800F7BC4(s32 pad_id);
u32 fn_800D7894(void);
void fn_800D7868(u32, u32, u32, u32, u32, u32, u32, u32);
void* GStextureLoad(void* data);
void GSresRegisterResource(void* texture, u32, u32 resourceId, u32);

const MenuData* menuDataBiosGetPtr(u32 id);
const MenuItem* menuItemBiosGetPtr(u32 itemId);
void* menuSeBiosGetPtr(s32 group);
u32 cursorBiosSetPos(u16 slot, MenuCursor* cursor);
void cursorBiosInit(void);

u8 menuOffScreenCheckEnable(u8 param);
void menuOffScreenFadeSet(f32 from, f32 to);
u8 menuOffScreenFadeSync(u8 param);
void menuOffScreenRelease(void);
u8 menuOffScreenCreate(u32 param);
u8 menuOffScreenSetDisp(u8 val);
u8 menuOffScreenSetPriority(u8 val);
void menuOffScreenInit(void);

void windowInit(u16 count);
MenuWindow* windowSearchID(u32 id);
u32 windowGetActiveID(void);
MenuKeyInfo* windowGetKeyInfo(void);
MenuKeyInfo* windowGetPortKeyInfo(u8 ports);
const MenuItem* windowGetCursorToItem(MenuWindow* window);
void windowClose(MenuWindow* window, u32 mode);
void windowCloseMain(MenuWindow* window);
MenuWindow* windowOpen(s32* cursor_out, u32 menu_id, u32 parent_id,
                       s32 close_flags, s32 open_param, va_list args);
void windowCheckCursor(u32 id, u8 check);
s32 windowGetValue(u32 id);
void windowCreateCursorSprite(MenuWindow* window);
void winSpriteSetDisp(MenuSprite* sprite, u32 enable);
void winSpriteDraw(MenuWindow* window, MenuSprite* sprite);
void fn_801093C8(void);
void fn_80106F98(u32 id);
s32 winSeqCheckMove(u32 id);
void winSeqMoveMenu(MenuWindow* window);
void fn_8010C224(u32 count);

/* menu.h */
s32 menuGetLastError(void);
void menuGetOffScreenFlag(void);
void menuReleaseOffScreen(f32 time);
void menuCreateOffScreen(f32 time);
s32 menuGetSelectItemNum(u32 id);
s32 menuGetCursorFromItemID(u32 id, u32 itemId);
void fn_801021F8(u32 id, u32 enable);
void menuSetDisp(u32 id, u8 enable);
s32 menuGetCursorItemID(u32 id);
s32 menuSetCursor(u32 id, s32 cursor);
s32 menuGetCursor(u32 id);
s32 menuCloseSync(u32 id, u8 wait);
void menuCloseFloor(void);
void fn_801024E8(void);
void menuClose(s32 id);
s32 menuCloseCustom(u32 id, u32 mode, u8 wait);
s32 menuIsCheck(u32 id);
void menuOpen(u32 menu_id, u8 check_cursor);
s32 menuOpenCustom(u32 menu_id, u32 parent_id, s32* cursor_out,
                   u32 close_flags, u8 check_cursor, s32 open_param, ...);
void menuSetPosition(u32 id, s16 x, s16 y);
void menuDaemon(void);
void menuButtonNormal(MenuWindow* window);
void menuCursorNormal(MenuWindow* window);
void menuPlaySe(u32 id, s32 event);
void menuGetKeyInfo(MenuKeyInfo* out, s32 port);
u8 menuGetEnablePort(void);
u8 menuSetEnablePort(u8 enable);
void menuInit(u16 windowCount);

u8 _menuGetAgbKeyInfo__FlPUs(s32 port, u16* keys);
u8 _menuGetGcKeyInfo__FlPUs(s32 port, u16* keys);
void _menuUpdateKeyInfo__FP15WINDOW_SYS_WORK(MenuSystem* work);

/* 0x80102004 | 0x10 */
s32 menuGetLastError(void) {
    return lbl_80404ACC.lastError;
}

/* 0x80102014 | 0x24 */
void menuGetOffScreenFlag(void) {
    menuOffScreenCheckEnable(0);
}

/* 0x80102038 | 0x34 */
void menuReleaseOffScreen(f32 time) {
    menuOffScreenFadeSet(0.0f, time);
    menuOffScreenFadeSync(1);
    menuOffScreenRelease();
}

/* 0x8010206C | 0x54 */
void menuCreateOffScreen(f32 time) {
    menuOffScreenCreate(1);
    menuOffScreenSetDisp(1);
    menuOffScreenSetPriority(0);
    menuOffScreenFadeSet(50.0f, time);
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
                GSlogWrite("%s(0x%08x)  スレッドから呼ぶようにしてください\n", __FUNCTION__, id);
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
                        system->lastError = 0;
                        system->activeId = node->id;
                    } else if ((windowGetKeyInfo()->keys & 0x8000) != 0) {
                        system->lastError = 1;
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

static inline const MenuItem* menuItemNext(const MenuItem* item) {
    return menuItemBiosGetPtr(item->next);
}

/* 0x80102F38 | 0x54C */
/*
 * Moves the cursor to the nearest selectable item in the pressed direction:
 * items within a band around the current item's row/column are searched
 * first, and the band is widened until an item is found. Wrapping menus
 * (cursor mode 2) then jump to the farthest item on the opposite side.
 */
void menuCursorNormal(MenuWindow* window) {
    s8 index;
    s32 cross;
    s32 best;
    s32 up;
    s32 down;
    u16 wrap;
    const MenuData* data;
    s32 left;
    u16 keys;
    s32 xRange;
    const MenuItem* current;
    const MenuItem* item;
    s32 yRange;
    s32 delta;
    s32 right;

    wrap = 0;
    if (window == NULL) {
        return;
    }
    if (window->cursorMode == 2) {
        wrap = 1;
    }

    data = menuDataBiosGetPtr(window->id);
    current = windowGetCursorToItem(window);
    if (current == NULL) {
        window->cursor.offset = 0;
        return;
    }

    xRange = 16;
    yRange = 12;
    keys = windowGetKeyInfo()->repeatKeys;
    up = keys & 1;
    down = keys & 2;
    left = keys & 4;
    right = keys & 8;

    for (;;) {
        index = 0;
        item = menuItemBiosGetPtr(data->firstItem);
        if (up) {
            best = 480;
            for (;;) {
                if (item->selectable) {
                    delta = current->y - item->y;
                    if (delta > 0) {
                        cross = current->x - item->x;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < xRange && best > delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        } else if (down) {
            best = 480;
            for (;;) {
                if (item->selectable) {
                    delta = item->y - current->y;
                    if (delta > 0) {
                        cross = current->x - item->x;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < xRange && best > delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        } else if (left) {
            best = 640;
            for (;;) {
                if (item->selectable) {
                    delta = current->x - item->x;
                    if (delta > 0) {
                        cross = current->y - item->y;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < yRange && best > delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        } else if (right) {
            best = 640;
            for (;;) {
                if (item->selectable) {
                    delta = item->x - current->x;
                    if (delta > 0) {
                        cross = current->y - item->y;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < yRange && best > delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        }

        if (window->previousCursor.offset != window->cursor.offset) {
            break;
        }
        xRange += 16;
        yRange += 12;
        if (xRange >= 640) {
            xRange = 640;
        }
        if (yRange >= 480) {
            yRange = 480;
        }
        if (xRange == 640 && yRange == 480) {
            break;
        }
    }

    if (wrap == 0) {
        return;
    }

    xRange = 16;
    yRange = 12;
    for (;;) {
        if (window->previousCursor.offset != window->cursor.offset) {
            return;
        }
        item = menuItemBiosGetPtr(data->firstItem);
        index = 0;
        if (up) {
            best = 0;
            for (;;) {
                if (item->selectable) {
                    delta = item->y - current->y;
                    if (delta > 0) {
                        cross = current->x - item->x;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < xRange && best < delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        } else if (down) {
            best = 0;
            for (;;) {
                if (item->selectable) {
                    delta = current->y - item->y;
                    if (delta > 0) {
                        cross = current->x - item->x;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < xRange && best < delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        } else if (left) {
            best = 0;
            for (;;) {
                if (item->selectable) {
                    delta = item->x - current->x;
                    if (delta > 0) {
                        cross = current->y - item->y;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < yRange && best < delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        } else if (right) {
            best = 0;
            for (;;) {
                if (item->selectable) {
                    delta = current->x - item->x;
                    if (delta > 0) {
                        cross = current->y - item->y;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < yRange && best < delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        }

        xRange += 16;
        yRange += 12;
        if (xRange >= 640) {
            xRange = 640;
        }
        if (yRange >= 480) {
            yRange = 480;
        }
        if (xRange == 640 && yRange == 480) {
            break;
        }
    }
}

/* 0x80103484 | 0x58 */
void menuPlaySe(u32 id, s32 event) {
    void* sound = menuSeBiosGetPtr(menuDataBiosGetPtr(id)->soundGroup & 7);

    if (sound != NULL) {
        u32 se = fn_8005D798(sound, event);
        if (se != 0) {
            soundStartSE(se);
        }
    }
}

/* 0x801034DC | 0x138 */
u8 _menuGetAgbKeyInfo__FlPUs(s32 port, u16* keys) {
    s32 device;
    u16 buttons;
    u16 result;

    device = port + 1;
    result = 0;
    if (fn_8008ABA0(device) == 0) {
        return 0;
    }

    buttons = fn_8008AB8C(device);
    if (buttons & 0x40) result |= 0x1;
    if (buttons & 0x80) result |= 0x2;
    if (buttons & 0x20) result |= 0x4;
    if (buttons & 0x10) result |= 0x8;
    if (buttons & 0x1) result |= 0x10;
    if (buttons & 0x2) result |= 0x20;
    if (buttons & 0x4) result |= 0x100;
    if (buttons & 0x200) result |= 0x200;
    if (buttons & 0x100) result |= 0x400;
    if (buttons & 0x8) result |= 0x800;

    *keys = result;
    return 1;
}

/* 0x80103614 | 0x2E4 */
u8 _menuGetGcKeyInfo__FlPUs(s32 port, u16* keys) {
    s32 padIds[4] = { 1, 2, 3, 4 };
    u16 result = 0;
    s32 padId;
    u32 stickX;
    u32 stickY;
    u32 buttons;
    f32 angle;

    padId = padIds[port];
    if (fn_800F7EF8(padId) == 0) {
        return 0;
    }

    stickX = fn_800F7A08(padId, 0);
    stickY = fn_800F7A7C(padId, 0);
    if (((s8)stickY < 0 ? -(s8)stickY : (s8)stickY) > 0x20 ||
        ((s8)stickX < 0 ? -(s8)stickX : (s8)stickX) > 0x20) {
        angle = atan2((s8)stickY, (s8)stickX);
        if ((angle > 0.0f ? angle : -angle) < 0.9599311f) {
            result |= 2;
        } else if ((angle > 0.0f ? angle : -angle) > 2.1816616f) {
            result |= 1;
        }
        if (0.61086524f < (angle > 0.0f ? angle : -angle) &&
            (angle > 0.0f ? angle : -angle) < 2.5307274f) {
            if (angle < 0.0f) {
                result |= 4;
            } else {
                result |= 8;
            }
        }
    }

    buttons = fn_800F7BC4(padId);
    if (buttons & 0x8) result |= 0x1;
    if (buttons & 0x4) result |= 0x2;
    if (buttons & 0x1) result |= 0x4;
    if (buttons & 0x2) result |= 0x8;
    if (buttons & 0x100) result |= 0x10;
    if (buttons & 0x200) result |= 0x20;
    if (buttons & 0x400) result |= 0x40;
    if (buttons & 0x800) result |= 0x80;
    if (buttons & 0x10) result |= 0x100;
    if (buttons & 0x40) result |= 0x200;
    if (buttons & 0x20) result |= 0x400;
    if (buttons & 0x1000) result |= 0x800;

    *keys = result;
    return 1;
}

/* 0x801038F8 | 0x2B0 */
/* Retail ignores the work argument and updates the global directly. */
void _menuUpdateKeyInfo__FP15WINDOW_SYS_WORK(MenuSystem* work) {
    MenuWindow* window;
    MenuKeyInfo* keyInfo;
    s32 i;
    s32 port;
    u16 keys;
    u16 pressed;
    u16 repeat;
    u16 held;
    u16 bit;
    u8 type;

    for (port = 0; port < 4; port++) {
        keyInfo = &lbl_80404ACC.portKeyInfo[port];
        lbl_8047AD08[port] = lbl_8047AD04[port];
        keys = 0;
        if (_menuGetGcKeyInfo__FlPUs(port, &keys) == 0) {
            if (_menuGetAgbKeyInfo__FlPUs(port, &keys) == 0) {
                keys = 0;
                type = 0;
            } else {
                type = 2;
            }
        } else {
            type = 1;
        }

        /* The case lists are the values the retail compare trees test. */
        switch (type) {
        case 0:
            switch (lbl_8047AD08[port]) {
            case 0:
            case 1:
            default:
                lbl_8047AD04[port] = 0;
                break;
            case 2:
            case 3:
                /* An AGB was unplugged: report it once as key 0x8000. */
                lbl_8047AD04[port] = 3;
                keys = 0x8000;
                break;
            }
            break;
        case 1:
            lbl_8047AD04[port] = 1;
            break;
        case 2:
            switch (lbl_8047AD08[port]) {
            case 0:
            case 2:
            case 3:
            default:
                lbl_8047AD04[port] = 2;
                break;
            case 1:
                lbl_8047AD04[port] = 2;
                break;
            }
            break;
        default:
            lbl_8047AD04[port] = type;
            break;
        }

        keyInfo->previousKeys = keyInfo->keys;
        pressed = (keyInfo->previousKeys ^ 0xFFFF) & keys;
        repeat = 0;
        held = 0;
        for (i = 0; i < 16; i++) {
            bit = 1 << i;
            if (pressed & bit) {
                keyInfo->repeatTimer[i] = 15;
                repeat |= bit;
            } else if (keys & bit) {
                keyInfo->repeatTimer[i] -= GSgetFrameStep();
                if (keyInfo->repeatTimer[i] <= 0) {
                    keyInfo->repeatTimer[i] = 5;
                    repeat |= bit;
                    held |= bit;
                } else {
                    held |= bit & keyInfo->heldKeys;
                }
            }
        }
        if (pressed & 0xF) {
            keyInfo->repeatTimer[0] = 15;
            keyInfo->repeatTimer[1] = 15;
            keyInfo->repeatTimer[2] = 15;
            keyInfo->repeatTimer[3] = 15;
        }
        keyInfo->keys = keys;
        keyInfo->pressedKeys = pressed;
        keyInfo->repeatKeys = repeat;
        keyInfo->heldKeys = held;
    }

    window = windowSearchID(windowGetActiveID());
    if (window == NULL) {
        type = 1;
    } else {
        type = window->keyPort;
    }
    lbl_80404ACC.keyInfo = *windowGetPortKeyInfo(type);
}

/* 0x80103BA8 | 0x108 */
void menuGetKeyInfo(MenuKeyInfo* out, s32 port) {
    MenuKeyInfo info;
    u8 mask;

    switch (port) {
    case 1: mask = 1; break;
    case 2: mask = 2; break;
    case 3: mask = 4; break;
    case 4: mask = 8; break;
    default: mask = 0; break;
    }
    if (mask != 0) {
        info = *windowGetPortKeyInfo(mask);
    } else {
        memset(&info, 0, sizeof(info));
    }
    *out = info;
}

/* 0x80103CB0 | 0x10 */
u8 menuGetEnablePort(void) {
    return lbl_80404ACC.enablePort;
}

/* 0x80103CC0 | 0x18 */
u8 menuSetEnablePort(u8 enable) {
    u8 old = lbl_80404ACC.enablePort;
    lbl_80404ACC.enablePort = enable;
    return old;
}

/* 0x80103CD8 | 0x190 */
void menuInit(u16 windowCount) {
    void* texture;
    s32 port;

    windowInit(windowCount);
    cursorBiosInit();
    menuOffScreenInit();
    fn_8010C224(0x18);
    lbl_80404ACC.enablePort = 1;
    for (port = 0; port < 4; port++) {
        lbl_8047AD04[port] = 0;
        lbl_8047AD08[port] = 0;
    }

    if (lbl_8047AD00 == 0) {
        lbl_8047AD00 = fn_800D7894();
        fn_800D7868(lbl_8047AD00, 1, 0, 0, 3, 0, 0, 0);
        fn_800D7868(lbl_8047AD00, 4, 0, 6, 10, 0, 0, 0);
        fn_800D7868(lbl_8047AD00, 6, 0, 8, 4, 0, 0, 0);
        fn_800D7868(lbl_8047AD00, 7, 0, 8, 4, 0, 0, 0);
        fn_800D7868(lbl_8047AD00, 8, 0, 8, 4, 0, 0, 0);
    }

    texture = GStextureLoad(lbl_803156E0);
    GSresRegisterResource(texture, 0, 0x31A1200, 0);
    texture = GStextureLoad(lbl_803254E0);
    GSresRegisterResource(texture, 0, 0x6221200, 0);
    texture = GStextureLoad(lbl_803357E0);
    GSresRegisterResource(texture, 0, 0x6F71200, 0);
}
