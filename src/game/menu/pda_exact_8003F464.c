/* RULE-EXCEPTION(user-approved): constant_import (temporary) - remove when this file is merged back into one unit - see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_exact_8003F464.c
 * @brief PDA summary-page widget layout and visibility, 0x8003F464 - 0x80040018.
 *
 * pdaMemoMainDrawText is copied from pda_range_80037158.c. Its float literals are
 * redirected by the temporary constant_import build step to the shared
 * lbl_8047BCxx pool.
 */
#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x4];
    s8 flags;
    u8 pad05;
    s16 eventId;
    u8 pad08[0x44];
    s32 messageId;
    s16 field_50;
    s16 field_52;
    s16 x;
    s16 y;
    u8 pad58[0xc];
    u8 colorR;
    u8 colorG;
    u8 colorB;
    u8 alpha;
    u8 pad68[0x8];
    f32 value;
    u8 pad74[0x17];
    u8 alphaByte;
    u8 pad8c[9];
    s8 selectedIndex;
} PdaSprite;

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0xC];
    s32 field_10;
    u8 pad14[0x14];
    s32 field_28;
    u8 pad2C[0x14];
    f32 angle;
    u8 pad44[8];
    f32 alphaScale;
    u8 pad50[0x1F8];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */

extern PdaSceneWork lbl_803A6818;
extern u8 lbl_802EF0A8[];
extern u32 GSmsgGetRect(s32 id);
extern void GSmsgPrintColor(s32 arg0, s32 arg1, s32 arg2, void* data);
extern void fn_800FE38C(s32 x1, s32 y1, s32 x2, s32 y2);
extern void fn_800FE35C(void);

extern s32 spriteSetOrigin(s16 x, s16 y);
extern void spriteSetEnv(s32 env);
extern u32 GSmsgGetGSchar(u32 msg);
extern void msgctrlSetValue(s32 id, u32 value);

/*
 * The summary page's .data, 0x802E52C8 - 0x802E61D8 (the switch table below
 * closes it at 0x802E60C4). pdaMemoMainDrawText reads its tab tables off this
 * section's base, so they are statics of this unit.
 */
extern u8 lbl_8047BB08[];
extern u8 lbl_8047BB0C[];
extern u8 lbl_8047BB10[];
extern u8 lbl_8047BB14[];
extern u8 lbl_8047BB18[];
extern u8 lbl_8047BB1C[];
extern u8 lbl_8047BB20[];
extern u8 lbl_8047BB24[];
extern u8 lbl_8047BB28[];
extern u8 lbl_8047BB2C[];
extern u8 lbl_8047BB30[];
extern u8 lbl_8047BB34[];
extern u8 lbl_8047BB38[];
extern u8 lbl_8047BB3C[];
extern u8 lbl_8047BB40[];
extern u8 lbl_8047BB44[];
extern u8 lbl_8047BB48[];
extern u8 lbl_8047BB4C[];
extern u8 lbl_8047BB50[];
extern u8 lbl_8047BB54[];
extern u8 lbl_8047BB58[];
extern u8 lbl_8047BB5C[];
extern u8 lbl_8047BB60[];
extern u8 lbl_8047BB64[];
extern u8 lbl_8047BB68[];
extern u8 lbl_8047BB6C[];
extern u8 lbl_8047BB70[];
extern u8 lbl_8047BB74[];
extern u8 lbl_8047BB78[];
extern u8 lbl_8047BB7C[];
extern u8 lbl_8047BB80[];
extern u8 lbl_8047BB84[];
extern u8 lbl_8047BB88[];
extern u8 lbl_8047BB8C[];
extern u8 lbl_8047BB90[];
extern u8 lbl_8047BB94[];
extern u8 lbl_8047BB98[];
extern u8 lbl_8047BB9C[];
extern u8 lbl_8047BBA0[];
extern u8 lbl_8047BBA4[];
extern u8 lbl_8047BBA8[];
extern u8 lbl_8047BBAC[];
extern u8 lbl_8047BBB0[];
extern u8 lbl_8047BBB4[];
extern u8 lbl_8047BBB8[];
extern u8 lbl_8047BBBC[];
extern u8 lbl_8047BBC0[];
extern u8 lbl_8047BBC4[];
extern u8 lbl_8047BBC8[];
extern u8 lbl_8047BBCC[];
extern u8 lbl_8047BBD0[];
extern u8 lbl_8047BBD4[];
extern u8 lbl_8047BBD8[];
extern u8 lbl_8047BBDC[];
extern u8 lbl_8047BBE0[];
extern u8 lbl_8047BBE4[];
extern u8 lbl_8047BBE8[];
extern u8 lbl_8047BBEC[];
extern u8 lbl_8047BBF0[];
extern u8 lbl_8047BBF4[];
extern u8 lbl_8047BBF8[];
extern u8 lbl_8047BBFC[];
extern u8 lbl_8047BC00[];
extern u8 lbl_8047BC04[];
extern u8 lbl_8047BC08[];
extern u8 lbl_8047BC0C[];
extern u8 lbl_8047BC10[];
extern u8 lbl_8047BC14[];
extern u8 lbl_8047BC18[];
extern u8 lbl_8047BC1C[];
extern u8 lbl_8047BC20[];
extern u8 lbl_8047BC24[];
extern u8 lbl_8047BC28[];
extern u8 lbl_8047BC2C[];
extern u8 lbl_8047BC30[];
extern u8 lbl_8047BC34[];
extern u8 lbl_8047BC38[];
extern u8 lbl_8047BC3C[];
extern u8 lbl_8047BC40[];
extern u8 lbl_8047BC44[];
extern u8 lbl_8047BC48[];
extern u8 lbl_8047BC4C[];
extern u8 lbl_8047BC50[];
extern u8 lbl_8047BC54[];
extern u8 lbl_8047BC58[];
extern u8 lbl_8047BC5C[];
extern u8 lbl_8047BC60[];
extern u8 lbl_8047BC64[];
extern u8 lbl_8047BC68[];
extern u8 lbl_8047BC6C[];
extern u8 lbl_8047BC70[];
extern u8 lbl_8047BC74[];
extern u8 lbl_8047BC78[];
extern u8 lbl_8047BC7C[];
extern u8 lbl_8047BC80[];
extern u8 lbl_8047BC84[];
extern u8 lbl_8047BC88[];
extern u8 lbl_8047BC8C[];
extern u8 lbl_8047BC90[];

u8 lbl_802E52C8[300] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x33, 0x00, 0x00, 0xFF, 0x01, 0x04, 0x05, 0x0F, 0x01, 0x07, 0x00, 0x07,
};

u8 lbl_802E53F4[12] = {
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

u8 lbl_802E5400[12] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

u8 lbl_802E540C[12] = {
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

u8 lbl_802E5418[12] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

u8 lbl_802E5424[12] = {
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

u8 lbl_802E5430[12] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

u8 lbl_802E543C[12] = {
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

u8 lbl_802E5448[12] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

u32 lbl_802E5454[19] = {
    0x3713,
    0x0BB9,
    0x0BBF,
    0x0BC2,
    0x0BC0,
    0x0BC1,
    0x0BC5,
    0x0BC4,
    0x0BC6,
    0x0BC9,
    0x0BBA,
    0x0BBB,
    0x0BBD,
    0x0BBC,
    0x0BC3,
    0x0BBE,
    0x0BC7,
    0x0BC8,
    0x0000,
};

u32 lbl_802E54A0[11] = {
    0x36EE,
    0x36EF,
    0x36F0,
    0x36F1,
    0x36F2,
    0x36F3,
    0x36F4,
    0x36F5,
    0x36F6,
    0x36F7,
    0x0000,
};

u32 lbl_802E54CC[12] = {
    0x36EE,
    0x36F8,
    0x36F9,
    0x36FA,
    0x36FB,
    0x36FC,
    0x36FD,
    0x36FE,
    0x36FF,
    0x3716,
    0x3700,
    0x0000,
};

u32 lbl_802E54FC[6] = {
    0x3702,
    0x3703,
    0x3704,
    0x3705,
    0x3706,
    0x0000,
};

/* Message lists of the summary page's four tabs (0x802E5514). */
static u32* lbl_802E5514[4] = {
    lbl_802E54A0,
    lbl_802E54CC,
    lbl_802E5454,
    lbl_802E54FC,
};

/* Sort-order captions (0x802E5524). */
static u32 lbl_802E5524[5] = {
    0x370C,
    0x370D,
    0x370E,
    0x370F,
    0x3710,
};

/* Page titles (0x802E5538). */
static u32 lbl_802E5538[5] = {
    0x3707,
    0x3708,
    0x3709,
    0x370A,
    0x3711,
};

u8 lbl_802E554C[36] = {
    0x00, 0x56, 0x00, 0x58, 0x00, 0x54, 0x00, 0x4C, 0x00, 0x5A, 0x00, 0x51,
    0x00, 0x55, 0x00, 0x52, 0x00, 0x50, 0x00, 0x5D, 0x00, 0x59, 0x00, 0x5B,
    0x00, 0x5C, 0x00, 0x4F, 0x00, 0x53, 0x00, 0x4D, 0x00, 0x4E, 0x00, 0x57,
};

void* lbl_802E5570[144] = {
    (void*)((u8*)lbl_8047BB08),
    (void*)((u8*)lbl_8047BB0C),
    (void*)((u8*)lbl_8047BB10),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BB18),
    (void*)((u8*)lbl_8047BB1C),
    (void*)((u8*)lbl_8047BB20),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BB24),
    (void*)((u8*)lbl_8047BB28),
    (void*)((u8*)lbl_8047BB2C),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BB30),
    (void*)((u8*)lbl_8047BB34),
    (void*)((u8*)lbl_8047BB38),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB3C),
    (void*)((u8*)lbl_8047BB40),
    (void*)((u8*)lbl_8047BB44),
    (void*)((u8*)lbl_8047BB48),
    (void*)((u8*)lbl_8047BB4C),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BB50),
    (void*)((u8*)lbl_8047BB54),
    (void*)((u8*)lbl_8047BB58),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BB5C),
    (void*)((u8*)lbl_8047BB60),
    (void*)((u8*)lbl_8047BB64),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB68),
    (void*)((u8*)lbl_8047BB6C),
    (void*)((u8*)lbl_8047BB70),
    (void*)((u8*)lbl_8047BB74),
    (void*)((u8*)lbl_8047BB78),
    (void*)((u8*)lbl_8047BB7C),
    (void*)((u8*)lbl_8047BB80),
    (void*)((u8*)lbl_8047BB84),
    (void*)((u8*)lbl_8047BB88),
    (void*)((u8*)lbl_8047BB8C),
    (void*)0x0000000F,
    (void*)((u8*)lbl_8047BB90),
    (void*)((u8*)lbl_8047BB94),
    (void*)((u8*)lbl_8047BB98),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BB9C),
    (void*)((u8*)lbl_8047BBA0),
    (void*)((u8*)lbl_8047BBA4),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BBA8),
    (void*)((u8*)lbl_8047BBAC),
    (void*)((u8*)lbl_8047BBB0),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BBB4),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000006,
};

void* lbl_802E57B0[144] = {
    (void*)((u8*)lbl_8047BBB8),
    (void*)((u8*)lbl_8047BBBC),
    (void*)((u8*)lbl_8047BBC0),
    (void*)((u8*)lbl_8047BBC4),
    (void*)((u8*)lbl_8047BBC8),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BBCC),
    (void*)((u8*)lbl_8047BBD0),
    (void*)((u8*)lbl_8047BBD4),
    (void*)((u8*)lbl_8047BBD8),
    (void*)((u8*)lbl_8047BBDC),
    (void*)((u8*)lbl_8047BBE0),
    (void*)((u8*)lbl_8047BBE4),
    (void*)((u8*)lbl_8047BBE8),
    (void*)((u8*)lbl_8047BBEC),
    (void*)((u8*)lbl_8047BBF0),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BBF4),
    (void*)((u8*)lbl_8047BBF8),
    (void*)((u8*)lbl_8047BBFC),
    (void*)((u8*)lbl_8047BC00),
    (void*)((u8*)lbl_8047BC04),
    (void*)((u8*)lbl_8047BC08),
    (void*)((u8*)lbl_8047BC0C),
    (void*)((u8*)lbl_8047BC10),
    (void*)((u8*)lbl_8047BC14),
    (void*)((u8*)lbl_8047BC18),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BC1C),
    (void*)((u8*)lbl_8047BC20),
    (void*)((u8*)lbl_8047BC24),
    (void*)((u8*)lbl_8047BC28),
    (void*)((u8*)lbl_8047BC2C),
    (void*)((u8*)lbl_8047BB3C),
    (void*)((u8*)lbl_8047BB40),
    (void*)((u8*)lbl_8047BB44),
    (void*)((u8*)lbl_8047BB48),
    (void*)((u8*)lbl_8047BB4C),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BC30),
    (void*)((u8*)lbl_8047BC34),
    (void*)((u8*)lbl_8047BC38),
    (void*)((u8*)lbl_8047BC3C),
    (void*)((u8*)lbl_8047BC40),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BC44),
    (void*)((u8*)lbl_8047BC48),
    (void*)((u8*)lbl_8047BC4C),
    (void*)((u8*)lbl_8047BC50),
    (void*)((u8*)lbl_8047BC54),
    (void*)((u8*)lbl_8047BB68),
    (void*)((u8*)lbl_8047BB6C),
    (void*)((u8*)lbl_8047BB70),
    (void*)((u8*)lbl_8047BB74),
    (void*)((u8*)lbl_8047BB78),
    (void*)((u8*)lbl_8047BB7C),
    (void*)((u8*)lbl_8047BB80),
    (void*)((u8*)lbl_8047BB84),
    (void*)((u8*)lbl_8047BB88),
    (void*)((u8*)lbl_8047BB8C),
    (void*)0x0000000F,
    (void*)((u8*)lbl_8047BC58),
    (void*)((u8*)lbl_8047BC5C),
    (void*)((u8*)lbl_8047BC60),
    (void*)((u8*)lbl_8047BC64),
    (void*)((u8*)lbl_8047BC68),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BC6C),
    (void*)((u8*)lbl_8047BC70),
    (void*)((u8*)lbl_8047BC74),
    (void*)((u8*)lbl_8047BC78),
    (void*)((u8*)lbl_8047BC7C),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BC80),
    (void*)((u8*)lbl_8047BC84),
    (void*)((u8*)lbl_8047BC88),
    (void*)((u8*)lbl_8047BC8C),
    (void*)((u8*)lbl_8047BC90),
    (void*)((u8*)lbl_8047BBB4),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000006,
};

void* lbl_802E59F0[144] = {
    (void*)((u8*)lbl_8047BBB8),
    (void*)((u8*)lbl_8047BBBC),
    (void*)((u8*)lbl_8047BBC0),
    (void*)((u8*)lbl_8047BBC4),
    (void*)((u8*)lbl_8047BBC8),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BBCC),
    (void*)((u8*)lbl_8047BBD0),
    (void*)((u8*)lbl_8047BBD4),
    (void*)((u8*)lbl_8047BBD8),
    (void*)((u8*)lbl_8047BBDC),
    (void*)((u8*)lbl_8047BBE0),
    (void*)((u8*)lbl_8047BBE4),
    (void*)((u8*)lbl_8047BBE8),
    (void*)((u8*)lbl_8047BBEC),
    (void*)((u8*)lbl_8047BBF0),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BBF4),
    (void*)((u8*)lbl_8047BBF8),
    (void*)((u8*)lbl_8047BBFC),
    (void*)((u8*)lbl_8047BC00),
    (void*)((u8*)lbl_8047BC04),
    (void*)((u8*)lbl_8047BC08),
    (void*)((u8*)lbl_8047BC0C),
    (void*)((u8*)lbl_8047BC10),
    (void*)((u8*)lbl_8047BC14),
    (void*)((u8*)lbl_8047BC18),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BC1C),
    (void*)((u8*)lbl_8047BC20),
    (void*)((u8*)lbl_8047BC24),
    (void*)((u8*)lbl_8047BC28),
    (void*)((u8*)lbl_8047BC2C),
    (void*)((u8*)lbl_8047BB3C),
    (void*)((u8*)lbl_8047BB40),
    (void*)((u8*)lbl_8047BB44),
    (void*)((u8*)lbl_8047BB48),
    (void*)((u8*)lbl_8047BB4C),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BC30),
    (void*)((u8*)lbl_8047BC34),
    (void*)((u8*)lbl_8047BC38),
    (void*)((u8*)lbl_8047BC3C),
    (void*)((u8*)lbl_8047BC40),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BC44),
    (void*)((u8*)lbl_8047BC48),
    (void*)((u8*)lbl_8047BC4C),
    (void*)((u8*)lbl_8047BC50),
    (void*)((u8*)lbl_8047BC54),
    (void*)((u8*)lbl_8047BB68),
    (void*)((u8*)lbl_8047BB6C),
    (void*)((u8*)lbl_8047BB70),
    (void*)((u8*)lbl_8047BB74),
    (void*)((u8*)lbl_8047BB78),
    (void*)((u8*)lbl_8047BB7C),
    (void*)((u8*)lbl_8047BB80),
    (void*)((u8*)lbl_8047BB84),
    (void*)((u8*)lbl_8047BB88),
    (void*)((u8*)lbl_8047BB8C),
    (void*)0x0000000F,
    (void*)((u8*)lbl_8047BC58),
    (void*)((u8*)lbl_8047BC5C),
    (void*)((u8*)lbl_8047BC60),
    (void*)((u8*)lbl_8047BC64),
    (void*)((u8*)lbl_8047BC68),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BC6C),
    (void*)((u8*)lbl_8047BC70),
    (void*)((u8*)lbl_8047BC74),
    (void*)((u8*)lbl_8047BC78),
    (void*)((u8*)lbl_8047BC7C),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BC80),
    (void*)((u8*)lbl_8047BC84),
    (void*)((u8*)lbl_8047BC88),
    (void*)((u8*)lbl_8047BC8C),
    (void*)((u8*)lbl_8047BC90),
    (void*)((u8*)lbl_8047BBB4),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000006,
};

void* lbl_802E5C30[144] = {
    (void*)((u8*)lbl_8047BBB8),
    (void*)((u8*)lbl_8047BBBC),
    (void*)((u8*)lbl_8047BBC0),
    (void*)((u8*)lbl_8047BBC4),
    (void*)((u8*)lbl_8047BBC8),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BBCC),
    (void*)((u8*)lbl_8047BBD0),
    (void*)((u8*)lbl_8047BBD4),
    (void*)((u8*)lbl_8047BBD8),
    (void*)((u8*)lbl_8047BBDC),
    (void*)((u8*)lbl_8047BBE0),
    (void*)((u8*)lbl_8047BBE4),
    (void*)((u8*)lbl_8047BBE8),
    (void*)((u8*)lbl_8047BBEC),
    (void*)((u8*)lbl_8047BBF0),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BBF4),
    (void*)((u8*)lbl_8047BBF8),
    (void*)((u8*)lbl_8047BBFC),
    (void*)((u8*)lbl_8047BC00),
    (void*)((u8*)lbl_8047BC04),
    (void*)((u8*)lbl_8047BC08),
    (void*)((u8*)lbl_8047BC0C),
    (void*)((u8*)lbl_8047BC10),
    (void*)((u8*)lbl_8047BC14),
    (void*)((u8*)lbl_8047BC18),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BC1C),
    (void*)((u8*)lbl_8047BC20),
    (void*)((u8*)lbl_8047BC24),
    (void*)((u8*)lbl_8047BC28),
    (void*)((u8*)lbl_8047BC2C),
    (void*)((u8*)lbl_8047BB3C),
    (void*)((u8*)lbl_8047BB40),
    (void*)((u8*)lbl_8047BB44),
    (void*)((u8*)lbl_8047BB48),
    (void*)((u8*)lbl_8047BB4C),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BC30),
    (void*)((u8*)lbl_8047BC34),
    (void*)((u8*)lbl_8047BC38),
    (void*)((u8*)lbl_8047BC3C),
    (void*)((u8*)lbl_8047BC40),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BC44),
    (void*)((u8*)lbl_8047BC48),
    (void*)((u8*)lbl_8047BC4C),
    (void*)((u8*)lbl_8047BC50),
    (void*)((u8*)lbl_8047BC54),
    (void*)((u8*)lbl_8047BB68),
    (void*)((u8*)lbl_8047BB6C),
    (void*)((u8*)lbl_8047BB70),
    (void*)((u8*)lbl_8047BB74),
    (void*)((u8*)lbl_8047BB78),
    (void*)((u8*)lbl_8047BB7C),
    (void*)((u8*)lbl_8047BB80),
    (void*)((u8*)lbl_8047BB84),
    (void*)((u8*)lbl_8047BB88),
    (void*)((u8*)lbl_8047BB8C),
    (void*)0x0000000F,
    (void*)((u8*)lbl_8047BC58),
    (void*)((u8*)lbl_8047BC5C),
    (void*)((u8*)lbl_8047BC60),
    (void*)((u8*)lbl_8047BC64),
    (void*)((u8*)lbl_8047BC68),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BC6C),
    (void*)((u8*)lbl_8047BC70),
    (void*)((u8*)lbl_8047BC74),
    (void*)((u8*)lbl_8047BC78),
    (void*)((u8*)lbl_8047BC7C),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BC80),
    (void*)((u8*)lbl_8047BC84),
    (void*)((u8*)lbl_8047BC88),
    (void*)((u8*)lbl_8047BC8C),
    (void*)((u8*)lbl_8047BC90),
    (void*)((u8*)lbl_8047BBB4),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000006,
};

void* lbl_802E5E70[144] = {
    (void*)((u8*)lbl_8047BBB8),
    (void*)((u8*)lbl_8047BBBC),
    (void*)((u8*)lbl_8047BBC0),
    (void*)((u8*)lbl_8047BBC4),
    (void*)((u8*)lbl_8047BBC8),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BBCC),
    (void*)((u8*)lbl_8047BBD0),
    (void*)((u8*)lbl_8047BBD4),
    (void*)((u8*)lbl_8047BBD8),
    (void*)((u8*)lbl_8047BBDC),
    (void*)((u8*)lbl_8047BBE0),
    (void*)((u8*)lbl_8047BBE4),
    (void*)((u8*)lbl_8047BBE8),
    (void*)((u8*)lbl_8047BBEC),
    (void*)((u8*)lbl_8047BBF0),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BBF4),
    (void*)((u8*)lbl_8047BBF8),
    (void*)((u8*)lbl_8047BBFC),
    (void*)((u8*)lbl_8047BC00),
    (void*)((u8*)lbl_8047BC04),
    (void*)((u8*)lbl_8047BC08),
    (void*)((u8*)lbl_8047BC0C),
    (void*)((u8*)lbl_8047BC10),
    (void*)((u8*)lbl_8047BC14),
    (void*)((u8*)lbl_8047BC18),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BC1C),
    (void*)((u8*)lbl_8047BC20),
    (void*)((u8*)lbl_8047BC24),
    (void*)((u8*)lbl_8047BC28),
    (void*)((u8*)lbl_8047BC2C),
    (void*)((u8*)lbl_8047BB3C),
    (void*)((u8*)lbl_8047BB40),
    (void*)((u8*)lbl_8047BB44),
    (void*)((u8*)lbl_8047BB48),
    (void*)((u8*)lbl_8047BB4C),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x0000000A,
    (void*)((u8*)lbl_8047BC30),
    (void*)((u8*)lbl_8047BC34),
    (void*)((u8*)lbl_8047BC38),
    (void*)((u8*)lbl_8047BC3C),
    (void*)((u8*)lbl_8047BC40),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BC44),
    (void*)((u8*)lbl_8047BC48),
    (void*)((u8*)lbl_8047BC4C),
    (void*)((u8*)lbl_8047BC50),
    (void*)((u8*)lbl_8047BC54),
    (void*)((u8*)lbl_8047BB68),
    (void*)((u8*)lbl_8047BB6C),
    (void*)((u8*)lbl_8047BB70),
    (void*)((u8*)lbl_8047BB74),
    (void*)((u8*)lbl_8047BB78),
    (void*)((u8*)lbl_8047BB7C),
    (void*)((u8*)lbl_8047BB80),
    (void*)((u8*)lbl_8047BB84),
    (void*)((u8*)lbl_8047BB88),
    (void*)((u8*)lbl_8047BB8C),
    (void*)0x0000000F,
    (void*)((u8*)lbl_8047BC58),
    (void*)((u8*)lbl_8047BC5C),
    (void*)((u8*)lbl_8047BC60),
    (void*)((u8*)lbl_8047BC64),
    (void*)((u8*)lbl_8047BC68),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BC6C),
    (void*)((u8*)lbl_8047BC70),
    (void*)((u8*)lbl_8047BC74),
    (void*)((u8*)lbl_8047BC78),
    (void*)((u8*)lbl_8047BC7C),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000005,
    (void*)((u8*)lbl_8047BC80),
    (void*)((u8*)lbl_8047BC84),
    (void*)((u8*)lbl_8047BC88),
    (void*)((u8*)lbl_8047BC8C),
    (void*)((u8*)lbl_8047BC90),
    (void*)((u8*)lbl_8047BBB4),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)((u8*)lbl_8047BB14),
    (void*)0x00000006,
};

void* lbl_802E60B0[5] = {
    (void*)((u8*)lbl_802E5570),
    (void*)((u8*)lbl_802E57B0),
    (void*)((u8*)lbl_802E59F0),
    (void*)((u8*)lbl_802E5C30),
    (void*)((u8*)lbl_802E5E70),
};

/* Anchor a PDA widget to its layout entry and hand the sprite to the drawer. */
static inline void pdaPlaceSprite(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    s16 base = *(s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c + 2);
    sprite->field_50 = (s16)(base + (s32)*(f32*)((u8*)&lbl_803A6818 + 0x1dc));
    spriteSetEnv(spriteSetOrigin(
        (s16)(*(s16*)((u8*)alphaSprite + 0x84) + sprite->field_50),
        (s16)(*(s16*)((u8*)alphaSprite + 0x86) + sprite->field_52)));
}

static inline s32 pdaTint(PdaSprite* alphaSprite, s32 tint)
{
    return alphaSprite->alphaByte | tint;
}

/* Draw the open tab's message list from row i on, one row every 29 pixels;
 * only the rows around the visible window are drawn. */
/* RULE-EXCEPTION(user-approved): single-use inline helper - the start row is a parameter, so its zero is copied into the strength-reduced word offset (retail li r29,0; mr r28,r29) - see docs/RULE_EXCEPTIONS.md */
static inline void pdaDrawTabList(PdaSprite* alphaSprite, u8* S, s8 i)
{
    f32 pos;
    f32 stride;
    pos = *(f32*)(S + 0x1d0);
    stride = 29.0f;
    for (; i < *(s8*)(S + 0x160); i++) {
        if (i >= *(s8*)(S + 0x1c8) - 1 && i <= *(s8*)(S + 0x1c9) + 1) {
            GSmsgPrintColor(0, (s32)pos, ((s64)alphaSprite->alphaByte | -0x100),
                        (void*)lbl_802E5514[*(s8*)(S + 0x15d)][i]);
        }
        pos = pos + stride;
    }
}

/* Per-widget layout and visibility for the PDA's summary page. */
void pdaMemoMainDrawText(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern f64 sin(f64 x);
    u8* S = (u8*)&lbl_803A6818;
    s32 slot;
    s32 msg;

    alphaSprite->alphaByte = 255.0f * lbl_803A6818.alphaScale;
    switch (sprite->eventId) {
    case 0xd47:
    case 0xd48:
    case 0xd49:
    case 0xd4a:
    case 0xd4b:
        pdaPlaceSprite(alphaSprite, sprite);
        break;
    case 0xd89:
        pdaPlaceSprite(alphaSprite, sprite);
        {
            s16* row = (s16*)(lbl_802EF0A8 + 0x17b00);
            slot = *(s8*)(S + 0x15f) - *(s8*)(S + 0x1c8);
            if (slot >= 0x1d) {
                slot = 0x1c;
            }
            sprite->field_52 = (s16)(*row + (s16)(slot * 0x1d));
        }

        break;
    case 0xd8a:
        pdaPlaceSprite(alphaSprite, sprite);
        if (*(s8*)((u8*)&lbl_803A6818 + 0x1c8) > 0) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
            break;
        }
        sprite->field_52 =
            (s16)(2.0f *
                      (f32)sin(3.1415927f *
                                   (2.0f *
                                    (2.0f * lbl_803A6818.angle)) +
                               3.1415927f) +
                  (f32)*(s16*)(lbl_802EF0A8 + 0x17b1c));
        break;
    case 0xd8b:
        pdaPlaceSprite(alphaSprite, sprite);
        if (*(s8*)(S + 0x1c9) >= 10) {
            if (*(s8*)(S + 0x160) != *(s8*)(S + 0x1c9)) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
                break;
            }
        } else {
            sprite->flags &= ~2;
            break;
        }
        sprite->field_52 =
            (s16)(2.0f *
                      (f32)sin(3.1415927f *
                               (2.0f *
                                (2.0f * lbl_803A6818.angle))) +
                  (f32)*(s16*)(lbl_802EF0A8 + 0x17b38));
        break;
    case 0xd66:
    case 0xd67:
    case 0xd68:
    case 0xd69:
    case 0xd6a:
    case 0xd6b:
    case 0xd6c:
    case 0xd6d:
    case 0xd6e:
        pdaPlaceSprite(alphaSprite, sprite);
        break;
    case 0xd6f:
        pdaPlaceSprite(alphaSprite, sprite);
        {
            /* Clip to the list window, layout entry 0xd8c, relative to this widget. */
            s16* e = (s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c);
            s16* r = (s16*)(lbl_802EF0A8 + 0xd8c * 0x1c);
            fn_800FE38C(r[1] - e[1], r[2] - e[2], r[3], r[4]);
        }
        if (*(s8*)(S + 0x15d) != 4) {
            pdaDrawTabList(alphaSprite, S, 0);
        }
        fn_800FE35C();
        break;
    case 0xd79:
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x36ed) >> 16)) / 2), -2,
                    ((s64)alphaSprite->alphaByte | -0x100), (void*)0x36ed);
        break;
    case 0xd7a:
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x36ec) >> 16)) / 2), -2,
                    ((s64)alphaSprite->alphaByte | -0x100), (void*)0x36ec);
        break;
    case 0xd7b:
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x36eb) >> 16)) / 2), -2,
                    ((s64)alphaSprite->alphaByte | -0x100), (void*)0x36eb);
        break;
    case 0xd7c:
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x36ea) >> 16)) / 2), -2,
                    ((s64)alphaSprite->alphaByte | -0x100), (void*)0x36ea);
        break;
    case 0xd7d:
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x36e9) >> 16)) / 2), -2,
                    ((s64)alphaSprite->alphaByte | -0x100), (void*)0x36e9);
        break;
    case 0xd74:
        GSmsgPrintColor(0, 0, ((s64)alphaSprite->alphaByte | -0x100),
                    (void*)(lbl_802E5514[3])[*(s8*)(S + 0x15c)]);
        break;
    case 0xd75:
        {
            u32 ch = GSmsgGetGSchar((lbl_802E5514[2])[*(s8*)(S + 0x15b)]);
            msgctrlSetValue(0x37, ch);
        }
        GSmsgPrintColor(0, 0, ((s64)alphaSprite->alphaByte | -0x100), (void*)0xe7);
        break;
    case 0xd76:
        {
            u32 ch = GSmsgGetGSchar((lbl_802E5514[2])[*(s8*)(S + 0x15a)]);
            msgctrlSetValue(0x37, ch);
        }
        GSmsgPrintColor(0, 0, ((s64)alphaSprite->alphaByte | -0x100), (void*)0xe7);
        break;
    case 0xd77:
        GSmsgPrintColor(0, 0, ((s64)alphaSprite->alphaByte | -0x100),
                    (void*)(lbl_802E5514[1])[*(s8*)(S + 0x159)]);
        break;
    case 0xd78:
        GSmsgPrintColor(0, 0, ((s64)alphaSprite->alphaByte | -0x100),
                    (void*)(lbl_802E5514[0])[*(s8*)(S + 0x158)]);
        break;
    case 0xd54:
        if (*(s8*)(S + 0x15d) == 0) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd55:
        if (*(s8*)(S + 0x15d) == 1) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd56:
        if (*(s8*)(S + 0x15d) == 2) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd57:
        if (*(s8*)(S + 0x15d) == 3) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd58:
        if (*(s8*)(S + 0x15d) == 4) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd73:
        switch (*(s32*)(S + 0x154)) {
        case 0:
            if (*(s8*)(S + 0x15d) == 4) {
                slot = *(s32*)(S + 0x1e4);
                if (slot == 0) {
                    msg = lbl_802E5538[*(s8*)(S + 0x15d)];
                }
                if (slot == 2) {
                    msg = 0x3712;
                }
                if (slot == 1) {
                    msg = 0x3714;
                }
            } else {
                msg = lbl_802E5538[*(s8*)(S + 0x15d)];
            }
            GSmsgPrintColor(0, 0, ((s64)alphaSprite->alphaByte | -0x100), (void*)msg);
            break;
        case 1:
            if (*(s8*)(S + 0x15d) == 3) {
                GSmsgPrintColor(0, 0, ((s64)alphaSprite->alphaByte | -0x100),
                            (void*)lbl_802E5524[*(s8*)(S + 0x15f)]);
            }
            break;
        default:
            break;
        }
        break;
    case 0xd59:
    case 0xd5a:
        if (*(s8*)(S + 0x15d) != 2) {
            if (*(s8*)(S + 0x15d) == 0) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd5b:
    case 0xd5c:
        if (*(s8*)(S + 0x15d) != 2) {
            if (*(s8*)(S + 0x15d) == 1) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd5d:
    case 0xd5e:
        if (*(s8*)(S + 0x15d) != 2) {
            if (*(s8*)(S + 0x15d) == 2) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        } else {
            if (*(s8*)(S + 0x15e) == 0) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        }
        break;
    case 0xd5f:
    case 0xd60:
        if (*(s8*)(S + 0x15d) != 2) {
            if (*(s8*)(S + 0x15d) == 2) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        } else {
            if (*(s8*)(S + 0x15e) == 1) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        }
        break;
    case 0xd61:
    case 0xd62:
        if (*(s8*)(S + 0x15d) != 2) {
            if (*(s8*)(S + 0x15d) == 3) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        } else {
            sprite->flags &= ~2;
        }
        break;
    default:
        break;
    }
}
