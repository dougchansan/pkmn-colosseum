/**
 * @file gs_texture_exact_800EFFC0.c
 * @brief GStexture pool initialisation, 0x800EFFC0 - 0x800F0030.
 *
 * The last function of retail GStexture.cpp. It is linked on its own while
 * GStextureCreate / GStextureLoad (gs_texture_candidate_800EF5FC.c) are
 * still being matched.
 */

#include "dolphin/types.h"
#include "game/gs_texture.h"

extern u16 _toolentryAlloc__FUl(u32 size); /* GSmemAllocRaw */
extern void* GSmemLock(u16 handle);      /* GSmemGetPtr */

extern u16 lbl_8047ABF0;              /* texture pool GSmem handle */
extern GStextureHandle* lbl_8047ABF4; /* texture pool */
extern u32 lbl_8047ABF8;              /* texture pool size */

void GStextureInit(u32 count)
{
    u32 i;

    lbl_8047ABF8 = count;
    lbl_8047ABF0 = _toolentryAlloc__FUl(count * sizeof(GStextureHandle));
    if (lbl_8047ABF0 == 0) {
        return;
    }

    lbl_8047ABF4 = GSmemLock(lbl_8047ABF0);
    for (i = 0; i < lbl_8047ABF8; i++) {
        lbl_8047ABF4[i].inUse = 0;
    }
}
