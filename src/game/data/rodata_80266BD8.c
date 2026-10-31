#include "dolphin/types.h"

#pragma section ".rodata"
#define RODATA __declspec(section ".rodata")

typedef struct TitleMessagePairEntry {
    u16 id;
    u16 pad;
    u32 messageId;
} TitleMessagePairEntry;

typedef struct TitleMessageChoiceEntry {
    u16 id;
    u16 pad;
    u32 messageId;
    u32 followupMessageId;
} TitleMessageChoiceEntry;

/*
 * Mixed game UI .rodata tables referenced from gs_event_exec.c, gs_pcbox.c,
 * and gs_title.c. The pcbox/title users copy these as 32-bit words.
 */
RODATA const u32 lbl_80266BD8[6] = {
    0x66, 0, 0,
    0x67, 0, 0,
};

RODATA const u32 lbl_80266BF0[4] = {
    0x278, 0x277, 0x276, 0x275,
};

RODATA const u32 lbl_80266C00[4] = {
    0x27F, 0x27D, 0x27B, 0x279,
};

RODATA const u32 lbl_80266C10[4] = {
    0x280, 0x27E, 0x27C, 0x27A,
};

/* 0x80266C20-0x80266C30 is windowDrawFrame's colour initializer, owned by the
 * menuSub TU (src/game/menuSub.c). */
