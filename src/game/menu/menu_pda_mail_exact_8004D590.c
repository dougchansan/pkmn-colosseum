/**
 * @file menu_pda_mail_exact_8004D590.c
 * @brief Mail attachment visibility and message callbacks.
 */
#include "dolphin/types.h"

typedef struct PdaMailWindow {
    u8 pad00[0x60];
    s32** field_0x60;
} PdaMailWindow;

typedef struct PdaMailOutput {
    u8 pad00[0x4C];
    u32 field_0x4c;
} PdaMailOutput;

extern s32 pdaMailGetMailID(s32 index);
extern u32 mailGetAttachFileGroup(s32 id);
extern void winSpriteSetDisp(void* field, s32 visible);

s32 pdaMailDrawPicture(PdaMailWindow* window, PdaMailOutput* output)
{
    if (mailGetAttachFileGroup(pdaMailGetMailID(**window->field_0x60)) != 0) {
        output->field_0x4c = 0x36B9;
    } else {
        output->field_0x4c = 0;
    }
    return 0;
}

s32 fn_8004D5EC(PdaMailWindow* window, void* field)
{
    u8 visible;

    if (mailGetAttachFileGroup(pdaMailGetMailID(**window->field_0x60)) != 0) {
        visible = 1;
    } else {
        visible = 0;
    }
    winSpriteSetDisp(field, visible);
    return 0;
}

s32 fn_8004D64C(PdaMailWindow* window, void* field)
{
    u8 visible;

    if (mailGetAttachFileGroup(pdaMailGetMailID(**window->field_0x60)) != 0) {
        visible = 1;
    } else {
        visible = 0;
    }
    winSpriteSetDisp(field, visible);
    return 0;
}
