/**
 * @file mail_exact_801D1338.c
 * @brief Mail: sort mode and sequence-entry type, 0x801D1338 - 0x801D13E4.
 *
 * mailBiosGetCounter / fn_801D1364 read and store the u16 at +0x444 of the save
 * data's mailbox scratch; mailDataBiosGet08 returns a sequence entry's type
 * (0xFFFF when the index is out of range). Carved out of the mail.c range
 * at the function boundaries. Text-only: the entry table and its count
 * (.sdata) belong to auto-generated data units and stay extern.
 *
 * Built with the mail unit's flags, which include -opt nopeephole (see
 * configure.py); mailBiosGetCounter matches only with it (45% with the peephole
 * pass on).
 */
#include "game/battle/battle_waza_types.h"

typedef struct MailPartyScratchExt {
    u8 pad_000[0x444];
    u16 sortMode; /* offset 0x444 */
} MailPartyScratchExt;

u16 mailBiosGetCounter(void)
{
    return ((MailPartyScratchExt*) savedataGetStatus(0, 0x0A))->sortMode;
}

/* Prototype from battle_waza_types.h. */
void* fn_801D1364(u16 mode, s32 idx)
{
    MailPartyScratchExt* party = savedataGetStatus(0, 0x0A);

    party->sortMode = mode;
    return party;
}

u32 mailDataBiosGet08(s32 idx)
{
    WazaEntry* entry;

    if (idx < 0 || (u32) idx >= *lbl_80478E98) {
        entry = NULL;
    } else {
        entry = &lbl_80478E9C[idx];
    }
    if (entry == NULL) {
        return 0xFFFF;
    }
    return *(u16*) ((u8*) entry + 0x08);
}
