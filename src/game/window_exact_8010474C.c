/**
 * @file window_exact_8010474C.c
 * @brief windowCloseMain (0x8010474C - 0x80104828).
 *
 * Runs the menu's close callback (state 5), unlinks the window from the
 * open-window list at lbl_80404ACC + 0x0C, releases its two sprite lists
 * and its work memory, and clears the slot.
 *
 * Window TU flags (GC/2.5, -O4,p, "-opt nopeephole"; see
 * window_exact_80104318.c): exact without the local optimization_level /
 * peephole pragmas it used to need. Carved so it can link while
 * windowClose stays a candidate; data stays extern.
 */
#include "dolphin/types.h"

extern u8 lbl_80404ACC[];

extern u8* menuDataBiosGetPtr(void* key);
extern void winSpriteRelease(void* head);
extern void* fn_800E24B0(u16 handle); /* GSmemLock */
extern void GSmemFree(u16 handle);  /* GSmemFree */

typedef void (*WindowCallback)(u8* window);

/* 0x8010474C | 0xDC */
void windowCloseMain(u8* window)
{
    u8* menuData;
    u8* next;

    menuData = menuDataBiosGetPtr(*(void**)(window + 0x04));
    if (*(WindowCallback*)(menuData + 0x14) != NULL) {
        window[1] = 5;
        (*(WindowCallback*)(menuData + 0x14))(window);
    }
    if (window != NULL) {
        if (*(u8**)(window + 0x14) == NULL) {
            *(u8**)(lbl_80404ACC + 0x0C) = *(u8**)(window + 0x10);
        } else {
            *(u8**)(*(u8**)(window + 0x14) + 0x10) = *(u8**)(window + 0x10);
        }
        next = *(u8**)(window + 0x10);
        if (next != NULL) {
            *(u8**)(next + 0x14) = *(u8**)(window + 0x14);
        }
        winSpriteRelease(window + 0x1C);
        winSpriteRelease(window + 0x20);
        if (*(u16*)(window + 0xAC) != 0) {
            fn_800E24B0(*(u16*)(window + 0xAC));
            GSmemFree(*(u16*)(window + 0xAC));
            *(u32*)(window + 0xB0) = 0;
            *(u16*)(window + 0xAC) = 0;
        }
        window[0] = 0;
        *(u32*)(window + 0x04) = 0;
    }
}
