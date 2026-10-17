/**
 * @file gs_colsys_exact_8010D170.c
 * @brief GScolsys2 collision debug-draw initialisation, 0x8010D170 - 0x8010D20C.
 *
 * Carved out of the gs_range_8010CBD0.c candidate range so it can link on
 * its own; it is built with that range's flags (GC/1.3 -O3).
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

extern GSColSysState lbl_80404C68;

extern u8* gfxVtxDescAlloc(void);
extern void gfxVtxDescSetAttr(u8*, u32, u32, u32, u32, u8, u32, u8);

void fn_8010D170(void)
{
    u8* handle;

    lbl_80404C68.activeLayer = 0;
    lbl_80404C68.displayList = NULL;
    handle = gfxVtxDescAlloc();
    lbl_80404C68.gfxRenderHandle = (u32)handle;
    gfxVtxDescSetAttr(handle, 1, 0, 1, 4, 0, 0, 0);
    gfxVtxDescSetAttr((u8*)lbl_80404C68.gfxRenderHandle, 4, 0, 6, 10, 0, 0, 0);
    lbl_80404C68.displayList = NULL;
}
