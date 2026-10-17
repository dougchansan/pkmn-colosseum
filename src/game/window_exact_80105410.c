/**
 * @file window_exact_80105410.c
 * @brief windowInit (0x80105410 - 0x801054B8).
 *
 * Clears the window system work (lbl_80404ACC), allocates the window
 * table (count * 0xB4 bytes) and initialises the sprite pool.
 *
 * Window TU flags (GC/2.5, -O4,p, "-opt nopeephole"; see
 * window_exact_80104318.c): exact without the local optimization_level /
 * peephole pragmas it used to need. Carved so it can link while its
 * neighbours stay candidates; data stays extern.
 */
#include "dolphin/types.h"

typedef struct WindowSystemWork {
    u16 count;       /* 0x00 */
    u16 tableHandle; /* 0x02 */
    u32 activeID;    /* 0x04 */
    void* table;     /* 0x08 */
    void* windows;   /* 0x0C: open-window list */
    u8 pad_10[0x9C - 0x10];
} WindowSystemWork;

extern WindowSystemWork lbl_80404ACC;
extern char lbl_80271EC4[]; /* "windowInit" + "initialisation failed\n" (SJIS) */

extern void* memset(void* dst, int value, u32 size);
extern void GSlogWrite(const char* fmt, ...);
extern u16 _toolentryAlloc__FUl(u32 size); /* GSmemAllocRaw */
extern void* GSmemLock(u16 handle);      /* GSmemGetPtr */
extern void winSpriteInit(void);

/* 0x80105410 | 0xA8 */
void windowInit(u16 count)
{
    u32 size;

    memset(&lbl_80404ACC, 0, sizeof(lbl_80404ACC));
    size = count * 0xB4;
    lbl_80404ACC.tableHandle = _toolentryAlloc__FUl(size);
    if (lbl_80404ACC.tableHandle == 0) {
        GSlogWrite(lbl_80271EC4);
    } else {
        lbl_80404ACC.table = GSmemLock(lbl_80404ACC.tableHandle);
        lbl_80404ACC.count = count;
        memset(lbl_80404ACC.table, 0, size);
        winSpriteInit();
    }
}
