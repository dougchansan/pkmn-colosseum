/**
 * @file menu_exact_8007B6A4.c
 * @brief Run the GBA boot worker and publish completion.
 */
#include "dolphin/types.h"

extern void gbaBootTransfer(u8* context);

void _gbaBootThread(u8* context)
{
    u8* saved_context = context;

    gbaBootTransfer(saved_context);
    saved_context[0x345] = 1;
}
