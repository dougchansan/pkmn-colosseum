#ifndef GAME_WINDOW_SEARCH_H
#define GAME_WINDOW_SEARCH_H

#include "dolphin/types.h"

/*
 * Window lookup by menu ID over the open-window list (lbl_80404ACC + 0x0C,
 * linked through +0x10, ID at +0x04). This is windowSearchID's body
 * (0x80104704); the window TU expands the same sequence inline in
 * windowGetValue, windowSetCursorById, windowGetCursor, windowCheckCursor and
 * windowClose (same "id <= 0" guard, same walk, same NULL tail).
 */
extern u8 lbl_80404ACC[];

static inline u8* windowSearchIDInline(s32 id)
{
    u8* window;

    if (id <= 0) {
        return NULL;
    }
    window = *(u8**)(lbl_80404ACC + 0x0C);
    while (window != NULL) {
        if (*(s32*)(window + 0x04) == id) {
            return window;
        }
        window = *(u8**)(window + 0x10);
    }
    return NULL;
}

#endif /* GAME_WINDOW_SEARCH_H */
