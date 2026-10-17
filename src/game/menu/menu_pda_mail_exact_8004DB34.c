/** Exact PDA-mail modal callbacks, 0x8004DB34 - 0x8004DDC0. */
#include "dolphin/types.h"

typedef struct PdaMailWindowB {
    u8 pad00[0x60];
    s32* field_0x60;
} PdaMailWindowB;

typedef struct PdaMailPhaseWidget {
    u8 pad00;
    s8 phase;
    s8 guard;
    u8 pad03;
    s32 msgObj;
} PdaMailPhaseWidget;

typedef struct PdaMailOutC {
    u8 pad00[6];
    s16 msgId;
    u8 pad08[0x44];
    u32 field_0x4c;
} PdaMailOutC;

extern void winSpriteSetDisp(void* fieldHandle, s32 value);
extern void winSeqSetMenu(s32 ctx, s32 id);
extern s32 menuOpenCustom(s32 menuId, ...);
extern u32 windowGetActiveID(void);
extern void menuClose(s32 menuId);
extern void menuCloseSync(s32 menuId, s32 flag);
extern u32 mailGetEntryField4(u32 idx);
extern const u32 lbl_802672F0[12];

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (scheduling off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma scheduling off
s32 pdaMailSortMenuDraw(PdaMailWindowB* window, void* fieldHandle)
{
    if (*window->field_0x60 != 0) {
        winSpriteSetDisp(fieldHandle, 0);
    } else {
        winSpriteSetDisp(fieldHandle, 1);
    }
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (scheduling reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma scheduling reset

#pragma peephole off
s32 fn_8004DB80(PdaMailPhaseWidget* w)
{
    switch (w->phase) {
    case 0:
        if (w->guard == 0) {
            winSeqSetMenu(w->msgObj, 0x1c2);
            w->guard = 1;
        }
        break;
    case 3:
        if (w->guard == 0) {
            winSeqSetMenu(w->msgObj, 0x1c6);
            w->guard = 1;
        }
        break;
    }
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
s32 fn_8004DC18(s32 a)
{
    s32 out = 0;
    s32 choice =
        menuOpenCustom(0x75, windowGetActiveID(), &a, 0, 1, 1, &out);
    if (choice != -1 && choice != a) {
        out = 1;
    }
    menuClose(0x75);
    menuCloseSync(0x75, 1);
    if (choice < 0 || choice >= 4) {
        return -1;
    }
    return choice;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
s32 fn_8004DCC0(void* unused, PdaMailOutC* window)
{
    s32 table[11];
    s32 i;

    table[0] = (s32)lbl_802672F0[0];
    table[1] = (s32)lbl_802672F0[1];
    table[2] = (s32)lbl_802672F0[2];
    table[3] = (s32)lbl_802672F0[3];
    table[4] = (s32)lbl_802672F0[4];
    table[5] = (s32)lbl_802672F0[5];
    table[6] = (s32)lbl_802672F0[6];
    table[7] = (s32)lbl_802672F0[7];
    table[8] = (s32)lbl_802672F0[8];
    table[9] = (s32)lbl_802672F0[9];
    table[10] = (s32)lbl_802672F0[10];

    for (i = 0; i < 11; i++) {
        if (window->msgId == table[i]) {
            break;
        }
    }
    if (i >= 11) {
        return 0;
    }
    {
        u32 result = mailGetEntryField4((u8)i);
        if (result != 0) {
            window->field_0x4c = result;
        } else {
            window->field_0x4c = 0x36CD;
        }
    }
    return 0;
}
#pragma peephole reset
