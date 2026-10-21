/* RULE-EXCEPTION(user-approved): constant_import (temporary) — remove when this file is merged back into one unit — see docs/RULE_EXCEPTIONS.md */
/*
 * Name-entry character-set cursor callbacks, 0x80026FEC-0x8002749C.
 * The three are identical: each draws its sprite only when the sprite id
 * matches the entry of lbl_8047B928 (0x040A, 0x040B, 0x040B, 0x040C) for the
 * selected set, scaling its rectangle about the centre and fading it out as
 * the scale grows.
 */
#include "dolphin/types.h"

typedef union NameEntryCharSetIds {
    u16 id[4];
    u32 word[2];
} NameEntryCharSetIds;

extern const u32 lbl_8047B928, lbl_8047B92C;
extern u8 lbl_802EF0A8[];
extern f32 lbl_8047B93C, lbl_8047B934, lbl_8047B940, lbl_8047B938;

s32 menuNameEntryDrawItem14(void* window, u8* draw)
{
    f32 dh;
    s16 bottom;
    s16* rect;
    f32 height;
    f32 dw;
    s16 cur;
    s32 set;
    f32 width;
    u16 id;
    u8* ctx;
    s16 right;
    u8 alpha;
    s16 left;
    f32 scale;
    s16 top;
    NameEntryCharSetIds ids;

    ctx = *(u8**)((u8*)window + 0x60);
    ids.word[0] = lbl_8047B928;
    ids.word[1] = lbl_8047B92C;
    set = **(s32**)(ctx + 0x2c);
    if (**(s32**)(ctx + 0x28) < 0xf) {
        id = 0xffff;
    } else if (set < 0 || set >= 4) {
        id = 0xffff;
    } else {
        id = ids.id[set];
    }
    cur = *(s16*)(draw + 6);
    if (id == cur) {
        rect = (s16*)(lbl_802EF0A8 + cur * 0x1c);
        width = rect[3];
        height = rect[4];
        scale = **(f32**)(ctx + 0x30);
        dw = lbl_8047B93C * (width * scale);
        alpha = lbl_8047B938 * (lbl_8047B934 - scale);
        dh = lbl_8047B93C * (height * scale);
        left = rect[1] - dw * lbl_8047B940;
        top = rect[2] - dh * lbl_8047B940;
        right = width + dw;
        bottom = height + dh;
        *(s16*)(draw + 0x50) = left;
        *(s16*)(draw + 0x52) = top;
        *(s16*)(draw + 0x54) = right;
        *(s16*)(draw + 0x56) = bottom;
    } else {
        alpha = 0;
    }
    draw[0x67] = alpha;
    return 0;
}

s32 menuNameEntryDrawItem15(void* window, u8* draw)
{
    f32 dh;
    s16 bottom;
    s16* rect;
    f32 height;
    f32 dw;
    s16 cur;
    s32 set;
    f32 width;
    u16 id;
    u8* ctx;
    s16 right;
    u8 alpha;
    s16 left;
    f32 scale;
    s16 top;
    NameEntryCharSetIds ids;

    ctx = *(u8**)((u8*)window + 0x60);
    ids.word[0] = lbl_8047B928;
    ids.word[1] = lbl_8047B92C;
    set = **(s32**)(ctx + 0x2c);
    if (**(s32**)(ctx + 0x28) < 0xf) {
        id = 0xffff;
    } else if (set < 0 || set >= 4) {
        id = 0xffff;
    } else {
        id = ids.id[set];
    }
    cur = *(s16*)(draw + 6);
    if (id == cur) {
        rect = (s16*)(lbl_802EF0A8 + cur * 0x1c);
        width = rect[3];
        height = rect[4];
        scale = **(f32**)(ctx + 0x30);
        dw = lbl_8047B93C * (width * scale);
        alpha = lbl_8047B938 * (lbl_8047B934 - scale);
        dh = lbl_8047B93C * (height * scale);
        left = rect[1] - dw * lbl_8047B940;
        top = rect[2] - dh * lbl_8047B940;
        right = width + dw;
        bottom = height + dh;
        *(s16*)(draw + 0x50) = left;
        *(s16*)(draw + 0x52) = top;
        *(s16*)(draw + 0x54) = right;
        *(s16*)(draw + 0x56) = bottom;
    } else {
        alpha = 0;
    }
    draw[0x67] = alpha;
    return 0;
}

s32 menuNameEntryDrawItem16(void* window, u8* draw)
{
    f32 dh;
    s16 bottom;
    s16* rect;
    f32 height;
    f32 dw;
    s16 cur;
    s32 set;
    f32 width;
    u16 id;
    u8* ctx;
    s16 right;
    u8 alpha;
    s16 left;
    f32 scale;
    s16 top;
    NameEntryCharSetIds ids;

    ctx = *(u8**)((u8*)window + 0x60);
    ids.word[0] = lbl_8047B928;
    ids.word[1] = lbl_8047B92C;
    set = **(s32**)(ctx + 0x2c);
    if (**(s32**)(ctx + 0x28) < 0xf) {
        id = 0xffff;
    } else if (set < 0 || set >= 4) {
        id = 0xffff;
    } else {
        id = ids.id[set];
    }
    cur = *(s16*)(draw + 6);
    if (id == cur) {
        rect = (s16*)(lbl_802EF0A8 + cur * 0x1c);
        width = rect[3];
        height = rect[4];
        scale = **(f32**)(ctx + 0x30);
        dw = lbl_8047B93C * (width * scale);
        alpha = lbl_8047B938 * (lbl_8047B934 - scale);
        dh = lbl_8047B93C * (height * scale);
        left = rect[1] - dw * lbl_8047B940;
        top = rect[2] - dh * lbl_8047B940;
        right = width + dw;
        bottom = height + dh;
        *(s16*)(draw + 0x50) = left;
        *(s16*)(draw + 0x52) = top;
        *(s16*)(draw + 0x54) = right;
        *(s16*)(draw + 0x56) = bottom;
    } else {
        alpha = 0;
    }
    draw[0x67] = alpha;
    return 0;
}

