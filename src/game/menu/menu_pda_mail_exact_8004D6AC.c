/**
 * @file menu_pda_mail_exact_8004D6AC.c
 * @brief Resolve the selected mail body message.
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
extern u32 mailGetContents(s32 id);

s32 pdaMailDrawContents(PdaMailWindow* window, PdaMailOutput* output)
{
    output->field_0x4c =
        mailGetContents(pdaMailGetMailID(**window->field_0x60));
    return 0;
}
