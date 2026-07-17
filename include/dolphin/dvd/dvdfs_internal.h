#ifndef DOLPHIN_DVD_DVDFS_INTERNAL_H
#define DOLPHIN_DVD_DVDFS_INTERNAL_H

#include "dolphin/types.h"

/*
 * The active splits keep these SDK strings in standalone data objects rather
 * than in the DVDFs text object. Centralize their declarations here so the
 * source can reference the canonical storage without duplicating literals.
 */
extern char lbl_803119F0[0x34];
extern char lbl_80311A24[0x2F];
extern char lbl_80311A54[0x27];
extern char lbl_804789C0[8];

void fn_800060F0(const char* file, s32 line, const char* expr, ...);

#endif /* DOLPHIN_DVD_DVDFS_INTERNAL_H */
