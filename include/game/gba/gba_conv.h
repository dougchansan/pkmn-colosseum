#ifndef GAME_GBA_GBA_CONV_H
#define GAME_GBA_GBA_CONV_H

#include "dolphin/types.h"

/**
 * @file gba_conv.h
 * @brief Typed record layouts for the small number of raw offset-cast
 * sites that are genuinely part of src/game/gba/gba_conv.c's scored
 * address range (0x80088428-0x80089048).
 *
 * NOTE: src/game/gba/gba_conv.c textually also contains a large block of
 * functions (0x80083AF4-0x80087C64, including a ~12KB function with over
 * 400 offset-cast sites) that are NOT part of this translation unit's
 * real scope: per config/GC6E01/splits.txt, that address range belongs to
 * game/menu/cardesavedata.c, and decomp.dev's per-function report for the
 * "main/game/gba/gba_conv" unit only includes functions inside
 * 0x80088428-0x80089048. Those pre-existing duplicate functions were left
 * untouched by this refactor (see gba_conv.c function-level comments).
 */

/**
 * Small state blob referenced via &lbl_803FB2F8 (extern u8[]). Only the
 * fields actually touched by in-scope functions are named; the gap at
 * 0x6-0x7 is unobserved.
 */
typedef struct GbaConvChannelState {
    /* 0x00 */ u16 ids[3];    /* species ids checked by dnaKeyCodeCheck */
    /* 0x06 */ u8  _pad06[2];
    /* 0x08 */ u32 result;   /* written by fn_80088428 from fn_80087C64()'s return value */
} GbaConvChannelState;

/**
 * Player state memo captured before a GBA link session (position,
 * rotation, previous floor, and a handful of flag values). Populated by
 * menuSaveLoadStorePosition via its `u8* p` output parameter.
 */
typedef struct GbaConvPlayerMemo {
    /* 0x00 */ u8  valid;
    /* 0x01 */ u8  _pad01[3];
    /* 0x04 */ u32 unk04;        /* fn_800FF56C() result */
    /* 0x08 */ u32 prevFloorId;  /* floorGetPrevFloorID() result */
    /* 0x0C */ f32 posX;
    /* 0x10 */ f32 posY;
    /* 0x14 */ f32 posZ;
    /* 0x18 */ f32 rotX;
    /* 0x1C */ f32 rotY;
    /* 0x20 */ f32 rotZ;
    /* 0x24 */ u32 flagAfc;      /* fn_801906A0(0xafc) */
    /* 0x28 */ u32 flagAfd;      /* fn_801906A0(0xafd) */
    /* 0x2C */ u32 flagB11;      /* fn_801906A0(0xb11) */
    /* 0x30 */ u32 flagDe1;      /* fn_801906A0(0xde1) */
} GbaConvPlayerMemo; /* size 0x34 */

/**
 * 0x1C-byte header block at the start of the live save status, copied
 * whole to status + 0xC98C by menuSaveLoadSaveGame. Only the first word is read.
 */
typedef struct GbaConvSaveHeader {
    /* 0x00 */ s32 kind;
    /* 0x04 */ u32 unk04[6];
} GbaConvSaveHeader; /* size 0x1C */

#endif /* GAME_GBA_GBA_CONV_H */
