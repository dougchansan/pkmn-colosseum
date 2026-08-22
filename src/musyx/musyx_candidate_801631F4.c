/**
 * @file musyx_candidate_801631F4.c
 * @brief Standalone owner for fn_801631F4 at 0x801631F4.
 */

#include "dolphin/types.h"
#include "musyx/runtime/hw_dspctrl.h"

typedef struct PeopleFieldMoveSlot {
    u8 pad_00[0x1C];       /* 0x00 */
    u32 field_1C;          /* 0x1C */
    u8 pad_20[0x4];        /* 0x20 */
    u32 flags_24[0x13];    /* 0x24 */
    u16 field_70;          /* 0x70 */
    u8 pad_72[0x1E];       /* 0x72 */
    u8 field_90;           /* 0x90 */
    u8 pad_91[0x3];        /* 0x91 */
    u32 field_94;          /* 0x94 */
    u32 field_98;          /* 0x98 */
    u8 field_9C;           /* 0x9C */
    u8 pad_9D[0x3];        /* 0x9D */
    u8 field_A0;           /* 0xA0 */
    u8 pad_A1[0x2B];       /* 0xA1 */
    u16 field_CC;          /* 0xCC */
    u16 field_CE;          /* 0xCE */
    u16 field_D0;          /* 0xD0 */
    u16 field_D2;          /* 0xD2 */
    u8 field_D4;           /* 0xD4 */
    u8 pad_D5[0x13];       /* 0xD5 */
    u32 field_E8;          /* 0xE8 */
    u8 active;             /* 0xEC */
    u8 field_ED;           /* 0xED */
    u8 field_EE;           /* 0xEE */
    u8 pad_EF;             /* 0xEF */
    u32 field_F0;          /* 0xF0 */
} PeopleFieldMoveSlot;

u32 fn_801631F4(u32 index) {
    PeopleFieldMoveSlot* entries = (PeopleFieldMoveSlot*)lbl_8047B024;
    u8 v = entries[index].active;
    u32 diff = 1 - v;
    return (u32)(diff == 0);
}
