/**
 * @file mail_exact_801D167C.c
 * @brief Mail: set / get the current mailbox handle, 0x801D167C - 0x801D16F0.
 *
 * fn_801D167C stores the handle in the save data's mailbox scratch when it
 * is below the mailbox count; mailGetCurrentIndex reads it back. Carved out of the
 * mail.c range at the function boundaries. Text-only: the count
 * (lbl_80478CB8, .sdata) belongs to an auto-generated data unit and stays
 * extern.
 *
 * Built with the mail unit's flags, which include -opt nopeephole (see
 * configure.py): with the peephole pass on, MWCC moves "li r4,10" (and in
 * mailGetCurrentIndex also "li r3,0") above the link-register store; retail keeps
 * the prologue store first in both.
 */
#include "game/battle/battle_waza_types.h"

extern s32 lbl_80478CB8;

void fn_801D167C(u8 handle)
{
    WazaPartyScratch* party = savedataGetStatus(0, 0x0A);

    if (handle < lbl_80478CB8) {
        party->currentHandle = handle;
    }
}

u8 mailGetCurrentIndex(void)
{
    WazaPartyScratch* party = savedataGetStatus(0, 0x0A);

    return party->currentHandle;
}
