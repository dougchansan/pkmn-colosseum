/**
 * @file dbgMenu_r61_middle_8013327C.c
 * @brief dbgMenu carve, 0x8013327C - 0x801333AC: dbgMenuGSmemCheck,
 *        dbgMenuFrameRate20/30, dbgMenuSendAllMail and dbgMenuSendMail.
 *
 * Text plus the check's two strings (.rodata 0x80272AE0 - 0x80272B08;
 * retail keeps dbgMenu's string literals in .rodata, so the unit is built
 * with -str reuse,readonly). GC/1.3 -O4,p, no pragmas.
 */
#include "dolphin/types.h"

extern u32 GSmemDumpMap(u32, u32);
extern void GSlogWrite(const char* format, ...);
extern void fn_800D3074(u32 rate);
extern void mailMainSendAllMail(void);
extern void mailMainReceiveStart(void);

u32 dbgMenuGSmemCheck(void)
{
    if ((u8)GSmemDumpMap(1, 0) == 1) {
        GSlogWrite("GSmem state OK\n");
    } else {
        GSlogWrite("GSmem state broken!\n");
    }
    return 0;
}

u32 dbgMenuFrameRate20(void)
{
    fn_800D3074(2);
    return 0;
}

u32 dbgMenuFrameRate30(void)
{
    fn_800D3074(1);
    return 0;
}

u32 dbgMenuSendAllMail(void)
{
    mailMainSendAllMail();
    return 0;
}

u32 dbgMenuSendMail(void)
{
    extern s32 menuOpen(u32, u32);
    extern s32 mailChkReceiveMail(s32);
    extern void mailAddMailbox(s32);
    s32 slot;

    while ((slot = menuOpen(2, 1)) != -1) {
        if (mailChkReceiveMail(slot) == 0) {
            mailAddMailbox(slot);
            mailMainReceiveStart();
        }
    }
    return 0;
}
