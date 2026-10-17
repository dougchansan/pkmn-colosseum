/**
 * @file mail_exact_801D1B10.c
 * @brief mail TU, 0x801D1B10 - 0x801D1E50: the mailbox selection byte, the
 *        per-mail received flags and the mailbox append (mailSetFlag442,
 *        mailGetSortMode, fn_801D1B78, fn_801D1C20, mailChkReceiveMail,
 *        mailAddMailbox).
 *
 * Function-boundary carve of mail.c, built with the mail group's flags
 * (GC/1.3, -opt nopeephole); text only. The flag helper computes the mask
 * before the byte index and leaves the mask unset for an out-of-range id,
 * which is what gives the three flag users retail's code.
 */
#include "game/battle/battle_waza_types.h"


/**
 * mailSetFlag442 - Waza set byte in battle party by handle.
 * Address: 0x801D1B10 | Size: 0x3C
 * Calls savedataGetStatus(0, 0xA), stores (handle & 0xFF) to result+0x442.
 */
void mailSetFlag442(s32 handle) {
    WazaPartyScratch* party = (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    party->selectedHandle = (u8)handle;
}

/**
 * mailGetSortMode - Waza get byte from battle party at offset 0x442.
 * Address: 0x801D1B4C | Size: 0x2C
 */
u8 mailGetSortMode(void) {
    WazaPartyScratch* party = (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    return party->selectedHandle;
}

static inline s32 mailGetFlagPos(s32 mailId, u8* mask)
{
    if (mailId < 0 || mailId >= 0x200) {
        return -1;
    }
    *mask = 1 << (7 - mailId % 8);
    return mailId / 8;
}

/**
 * Test whether a mailbox ID has already been received.
 * Address: 0x801D1B78 | Size: 0xA8
 */
BOOL fn_801D1B78(s32 mailId) {
    WazaPartyScratch* party =
        (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    u8 mask;
    s32 index = mailGetFlagPos(mailId, &mask);

    if (index < 0) {
        return FALSE;
    }
    return (party->receivedFlags[index] & mask) != 0;
}

/**
 * Mark a mailbox ID as received.
 * Address: 0x801D1C20 | Size: 0xA4
 */
BOOL fn_801D1C20(s32 mailId) {
    WazaPartyScratch* party =
        (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    u8 mask;
    s32 index = mailGetFlagPos(mailId, &mask);

    if (index < 0) {
        return FALSE;
    }
    party->receivedFlags[index] |= mask;
    return TRUE;
}

/**
 * mailChkReceiveMail - Waza effect trajectory calculation.
 * Address: 0x801D1CC4 | Size: 0x94
 */
BOOL mailChkReceiveMail(s32 idx) {
    WazaPartyScratch* party = (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    u16 count;
    s32 i;
    u16* entry;

    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        return FALSE;
    }

    count = party->count;
    entry = party->seqIds;
    for (i = 0; i < count; i++) {
        if (idx == *entry) {
            break;
        }
        entry++;
    }

    if (i >= count) {
        return FALSE;
    }
    return TRUE;
}

/**
 * Append a mailbox ID and clear its received flag.
 * Address: 0x801D1D58 | Size: 0xF8
 */
BOOL mailAddMailbox(s32 mailId) {
    WazaPartyScratch* party =
        (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    s32 index;
    u8 mask;

    if (mailId < 0 || (u32)mailId >= *lbl_80478E98) {
        return FALSE;
    }
    if (party->count >= 0x200) {
        return FALSE;
    }

    party->seqIds[party->count] = mailId;
    party->count++;

    party = (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    index = mailGetFlagPos(mailId, &mask);
    if (index >= 0) {
        party->receivedFlags[index] &= ~mask;
    }
    return TRUE;
}
