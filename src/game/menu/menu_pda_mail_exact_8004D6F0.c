/**
 * @file menu_pda_mail_exact_8004D6F0.c
 * @brief Mail metadata, cursor, and modal-list callbacks.
 */
#include "dolphin/types.h"

typedef struct PdaMailWindow {
    u8 pad00;
    s8 phase;
    s8 guard;
    u8 pad03;
    s32 msgObj;
    u8 pad08[0x58];
    s32** field_0x60;
} PdaMailWindow;

typedef struct PdaMailOutput {
    u8 pad00[0x4C];
    u32 field_0x4c;
} PdaMailOutput;

typedef struct PdaMailPhaseWidget {
    u8 pad00;
    s8 phase;
    s8 guard;
    u8 pad03;
    s32 msgObj;
} PdaMailPhaseWidget;

extern s32 pdaMailGetMailID(s32 index);
extern u32 mailGetSenderName(s32 id);
extern u32 mailGetSubject(s32 id);
extern u32 mailGetAttachFileGroup(s32 id);
extern void* GSmsgGetGSchar(u32 id);
extern void msgctrlSetValue(s32 slot, void* value);
extern s32 mailGetNbMailInMailbox(void);
extern u8* windowGetKeyInfo(void);
extern void menuButtonNormal(void* window);
extern void soundStart(s32 id, s32 arg1, s32 arg2, s32 arg3);
extern s32 fn_801D1B78(s32 mailId);
extern void fn_801D1C20(s32 mailId);
extern void fn_801D228C(u16 mailId);
extern void winSeqSetMenu(s32 context, s32 id);
extern s32 lbl_8047A518;
extern s32 menuOpenCustom(s32, s32, s32, s32, s32, s32, void*, ...);
extern s32 windowGetActiveID(void);
extern void fn_8004E9C0(s32 mailId);
extern void menuClose(s32 id);
extern void menuCloseSync(s32 id, s32 wait);

s32 pdaMailDrawSender(PdaMailWindow* window, PdaMailOutput* output)
{
    u32 value = mailGetSenderName(pdaMailGetMailID(**window->field_0x60));

    if (value != 0) {
        void* message = GSmsgGetGSchar(value);
        msgctrlSetValue(0x37, message);
        output->field_0x4c = 0xE7;
    } else {
        output->field_0x4c = 0;
    }
    return 0;
}

s32 pdaMailDrawSubject(PdaMailWindow* window, PdaMailOutput* output)
{
    u32 value = mailGetSubject(pdaMailGetMailID(**window->field_0x60));

    if (value != 0) {
        void* message = GSmsgGetGSchar(value);
        msgctrlSetValue(0x37, message);
        output->field_0x4c = 0xE7;
    } else {
        output->field_0x4c = 0;
    }
    return 0;
}

s32 pdaMailViewKey(PdaMailWindow* window)
{
    s32** field = window->field_0x60;
    u8* state = windowGetKeyInfo();
    s32 index = **field;
    s32 mailId;
    s32 current = index;
    u16 flags;

    flags = *(u16*)(state + 6);
    if (flags & 2) {
        s32 count = mailGetNbMailInMailbox();
        index++;
        if (index >= count) {
            index = 0;
        }
    }
    flags = *(u16*)(state + 6);
    if (flags & 1) {
        index--;
        if (index < 0) {
            index = mailGetNbMailInMailbox() - 1;
        }
    }
    if (index != current) {
        soundStart(0x23, 0, 0xFF, 0);
        **field = index;
    }
    mailId = pdaMailGetMailID(index);
    if (fn_801D1B78(mailId) == 0) {
        fn_801D1C20(mailId);
        fn_801D228C((u16)mailId);
    }
    return 0;
}

void fn_8004D8BC(PdaMailWindow* window)
{
    u8* state = windowGetKeyInfo();

    if (mailGetAttachFileGroup(pdaMailGetMailID(**window->field_0x60)) != 0 ||
        (*(u16*)state & 0x10) == 0) {
        menuButtonNormal(window);
    }
}

s32 fn_8004D928(PdaMailPhaseWidget* widget)
{
    switch (widget->phase) {
    case 0:
        if (widget->guard == 0) {
            winSeqSetMenu(widget->msgObj, 0x1C2);
            widget->guard = 1;
        }
        break;
    case 3:
        if (widget->guard == 0) {
            winSeqSetMenu(widget->msgObj, 0x1C6);
            widget->guard = 1;
        }
        break;
    }
    return 0;
}

s32 fn_8004D9C0(s32 selection)
{
    lbl_8047A518 = selection;
    for (;;) {
        s32* config = &lbl_8047A518;
        s32 choice = menuOpenCustom(0x74, windowGetActiveID(), 0, 0, 1, 1,
                                    (s32*)&config);
        if (choice == -1) {
            break;
        }
        {
            s32 mailId = pdaMailGetMailID(lbl_8047A518);
            if (mailGetAttachFileGroup(mailId) != 0) {
                fn_8004E9C0(mailId);
            }
        }
    }
    menuClose(0x74);
    menuCloseSync(0x74, 1);
    return lbl_8047A518;
}
