/**
 * @file mail.c
 * @brief Mail system: mailGet-prefixed and mailChk-prefixed accessor and
 * helper functions.
 *
 * Split from the former game/battle/battle_waza.c CodeCandidate bucket
 * (0x801D1470-0x801DE698); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit (this TU was extended down to
 * 0x801D1338 to absorb fn_801D1338..fn_801D142C, which previously lived,
 * misattributed, in battle_waza.c -- see config/GC6E01/splits.txt for the
 * corresponding shrink of game/battle/battle_range_801D0AA0.c). Shared
 * typedefs and cross-TU forward declarations live in
 * include/game/battle/battle_waza_types.h.
 */

#include "game/battle/battle_waza_types.h"

/* =========================================================================
 * WAZA DATA ACCESS HELPERS (0x801D1338 - 0x801D1618)
 *
 * Small getter/setter functions for waza sequence data fields.
 * ========================================================================= */

/**
 * fn_801D1338 - Waza get sequence count.
 * Address: 0x801D1338 | Size: 0x2C
 */
s32 fn_801D1338(void* wazaCtx) {
    if (wazaCtx == NULL) return 0;
    return *(s32*)((u8*)wazaCtx + 0x00);
}

/* Extended view of party scratch to access u16 field at offset 0x444.
 * TODO: fold field_444 into WazaPartyScratch when full layout confirmed. */
typedef struct MailPartyScratchExt {
    u8 pad_000[0x444];
    u16 sortMode; /* offset 0x444 */
} MailPartyScratchExt;

/**
 * fn_801D1364 - Set u16 handle in battle party scratch at offset 0x444.
 * Address: 0x801D1364 | Size: 0x38
 * Stores handle as u16 at party+0x444 via savedataGetStatus(0, 0xA).
 */
#pragma push
#pragma peephole off
void* fn_801D1364(u16 handle, s32 idx) {
    MailPartyScratchExt* party = (MailPartyScratchExt*)savedataGetStatus(0, 0x0A);
    party->sortMode = handle;
    return party;
}
#pragma pop

/**
 * fn_801D139C - Waza get entry type.
 * Address: 0x801D139C | Size: 0x48
 */
s32 fn_801D139C(void* entry) {
    if (entry == NULL) return -1;
    return *(s32*)((u8*)entry + 0x00);
}

/**
 * fn_801D13E4 - Waza get entry field at +0x04 (u16), by index.
 * Address: 0x801D13E4 | Size: 0x48
 */
u32 fn_801D13E4(void* entry) {
    s32 idx;
    WazaEntry* sequenceEntry;

    idx = (s32)entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        sequenceEntry = NULL;
    } else {
        sequenceEntry = (WazaEntry*)(lbl_80478E9C + (u32)idx * sizeof(WazaEntry));
    }

    if (sequenceEntry == NULL) return 0xFFFF;
    return *(u16*)&sequenceEntry->startFrame;
}

/**
 * fn_801D142C - Waza get entry duration.
 * Address: 0x801D142C | Size: 0x44
 */
f32 fn_801D142C(void* entry) {
    if (entry == NULL) return 0.0f;
    return *(f32*)((u8*)entry + 0x08);
}

/**
 * mailGetNbMailData - Waza get entry flags.
 * Address: 0x801D1470 | Size: 0xC
 */
extern u32* lbl_80478E98; /* waza context pointer */
extern u32 lbl_80478E9C; /* waza entry array pointer */
u32 mailGetNbMailData(void* entry) {
    return *lbl_80478E98;
}

/**
 * fn_801D147C - Waza get entry resource ID.
 * Address: 0x801D147C | Size: 0x44
 */
s32 fn_801D147C(void* entry) {
    s32 idx;
    WazaEntry* sequenceEntry;

    idx = (s32)entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        sequenceEntry = NULL;
    } else {
        sequenceEntry = (WazaEntry*)(lbl_80478E9C + (u32)idx * sizeof(WazaEntry));
    }

    if (sequenceEntry == NULL) return -1;
    return sequenceEntry->resourceID;
}

/**
 * fn_801D14C0 - Waza get entry field 0x24 by index.
 * Address: 0x801D14C0 | Size: 0x44
 * Bounds-checks idx against lbl_80478E98->count, then
 * returns entry[idx*0x2C + 0x24] from lbl_80478E9C.
 */
s32 fn_801D14C0(s32 idx) {
    WazaEntry* entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        entry = NULL;
    } else {
        entry = (WazaEntry*)(lbl_80478E9C + (u32)idx * sizeof(WazaEntry));
    }

    if (entry == NULL) return -1;
    return entry->field_24;
}

/**
 * fn_801D1504 - Waza get entry field at +0x20 (s32), by index.
 * Address: 0x801D1504 | Size: 0x44
 */
s32 fn_801D1504(void* entry) {
    s32 idx;
    void* sequenceEntry;

    idx = (s32)entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        sequenceEntry = NULL;
    } else {
        sequenceEntry = (void*)(lbl_80478E9C + (u32)idx * sizeof(WazaEntry));
    }

    if (sequenceEntry == NULL) return -1;
    return *(s32*)((u8*)sequenceEntry + 0x20);
}

/**
 * mailGetSendCondition - Waza get entry field at +0x1C (s32), by index.
 * Address: 0x801D1548 | Size: 0x44
 */
s32 mailGetSendCondition(void* entry) {
    s32 idx;
    void* sequenceEntry;

    idx = (s32)entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        sequenceEntry = NULL;
    } else {
        sequenceEntry = (void*)(lbl_80478E9C + (u32)idx * sizeof(WazaEntry));
    }

    if (sequenceEntry == NULL) return -1;
    return *(s32*)((u8*)sequenceEntry + 0x1C);
}

/**
 * mailGetSendCondType - Waza get entry field at +0x00 (u8), by index.
 * Address: 0x801D158C | Size: 0x44
 */
u32 mailGetSendCondType(void* entry) {
    s32 idx;
    void* sequenceEntry;

    idx = (s32)entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        sequenceEntry = NULL;
    } else {
        sequenceEntry = (void*)(lbl_80478E9C + (u32)idx * sizeof(WazaEntry));
    }

    if (sequenceEntry == NULL) return 0xFF;
    return *(u8*)((u8*)sequenceEntry + 0x00);
}

/**
 * mailGetSendRate - Waza get entry field at +0x06 (u16), by index.
 * Address: 0x801D15D0 | Size: 0x48
 */
u32 mailGetSendRate(void* entry) {
    s32 idx;
    void* sequenceEntry;

    idx = (s32)entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        sequenceEntry = NULL;
    } else {
        sequenceEntry = (void*)(lbl_80478E9C + (u32)idx * sizeof(WazaEntry));
    }

    if (sequenceEntry == NULL) return 0xFFFF;
    return *(u16*)((u8*)sequenceEntry + 0x06);
}

/**
 * fn_801D1618 - Waza get entry active flag.
 * Address: 0x801D1618 | Size: 0x8
 */
extern s32 lbl_80478CB8;
u32 fn_801D1618(void) {
    return lbl_80478CB8;
}

/**
 * fn_801D1620 - Waza handle table lookup (second word of pair).
 * Address: 0x801D1620 | Size: 0x30
 */
extern u32 lbl_8036E0E0[];
u32 fn_801D1620(u32 idx) {
    s32 slot = idx & 0xFF;
    if (slot >= lbl_80478CB8) {
        return 0;
    }
    return lbl_8036E0E0[slot * 2 + 1];
}

/**
 * fn_801D1650 - Waza handle table lookup.
 * Address: 0x801D1650 | Size: 0x2C
 */
extern u32 lbl_8036E0E0[];
u32 fn_801D1650(u32 idx) {
    s32 slot = idx & 0xFF;
    if (slot >= lbl_80478CB8) {
        return 0;
    }
    return lbl_8036E0E0[slot * 2];
}

/**
 * fn_801D167C - Waza set current handle.
 * Address: 0x801D167C | Size: 0x48
 */
#pragma push
#pragma peephole off
void fn_801D167C(u8 handle) {
    s32 slot = handle & 0xFF;
    WazaPartyScratch* party = (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    if (slot < lbl_80478CB8) {
        party->currentHandle = handle;
    }
}
#pragma pop

/**
 * fn_801D16C4 - Waza get current handle.
 * Address: 0x801D16C4 | Size: 0x2C
 */
#pragma scheduling off
u8 fn_801D16C4(void) {
    WazaPartyScratch* party = (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    return party->currentHandle;
}
#pragma scheduling on

/**
 * mailGetAttachFileGroup - Waza entry get field 0x18 by index.
 * Address: 0x801D16F0 | Size: 0x44
 */
u32 mailGetAttachFileGroup(s32 idx) {
    WazaEntry* entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        entry = NULL;
    } else {
        entry = (WazaEntry*)(lbl_80478E9C + (u32)idx * sizeof(WazaEntry));
    }
    if (entry == NULL) return 0;
    return *(u32*)&entry->posZ;
}

/* =========================================================================
 * WAZA ANIMATION CONTROLLERS (0x801D1734 - 0x801D2C6C)
 *
 * Mid-level functions controlling move animation playback.
 * ========================================================================= */

/**
 * fn_801D1734 - Waza animation play with target tracking.
 * Address: 0x801D1734 | Size: 0x130
 */
void fn_801D1734(s32 attackerSlot, s32 targetSlot, s32 moveID) {
    /* TODO: Waza animation with target tracking (0x130 bytes) */
}

/**
 * fn_801D1864 - Waza animation play with camera.
 * Address: 0x801D1864 | Size: 0x140
 */
void fn_801D1864(s32 attackerSlot, s32 targetSlot, s32 moveID, s32 cameraMode) {
    /* TODO: Waza animation with camera (0x140 bytes) */
}

/**
 * fn_801D19A4 - Waza animation speed control.
 * Address: 0x801D19A4 | Size: 0xA0
 */
void fn_801D19A4(s32 seqHandle, f32 speed) {
    /* TODO: Waza animation speed control (0xA0 bytes) */
}

/**
 * mailGetContents - Waza entry get field 0x14 by index.
 * Address: 0x801D1A44 | Size: 0x44
 * Bounds-checks idx against lbl_80478E98->count, then
 * returns entry[idx*0x2C + 0x14] from lbl_80478E9C.
 */
u32 mailGetContents(s32 idx) {
    WazaEntry* entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        entry = NULL;
    } else {
        entry = (WazaEntry*)(lbl_80478E9C + (u32)idx * sizeof(WazaEntry));
    }
    if (entry == NULL) return 0;
    return *(u32*)&entry->posY;
}

/**
 * mailGetSenderName - Waza entry get field 0x0C by index.
 * Address: 0x801D1A88 | Size: 0x44
 */
u32 mailGetSenderName(s32 idx) {
    WazaEntry* entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        entry = NULL;
    } else {
        entry = (WazaEntry*)(lbl_80478E9C + (u32)idx * sizeof(WazaEntry));
    }
    if (entry == NULL) return 0;
    return entry->field_0C;
}

/**
 * mailGetSubject - Waza entry get field 0x10 by index.
 * Address: 0x801D1ACC | Size: 0x44
 */
u32 mailGetSubject(s32 idx) {
    WazaEntry* entry;
    if (idx < 0 || (u32)idx >= *lbl_80478E98) {
        entry = NULL;
    } else {
        entry = (WazaEntry*)(lbl_80478E9C + (u32)idx * sizeof(WazaEntry));
    }
    if (entry == NULL) return 0;
    return entry->field_10;
}

/**
 * fn_801D1B10 - Waza set byte in battle party by handle.
 * Address: 0x801D1B10 | Size: 0x3C
 * Calls savedataGetStatus(0, 0xA), stores (handle & 0xFF) to result+0x442.
 */
#pragma push
#pragma peephole off
void fn_801D1B10(s32 handle) {
    WazaPartyScratch* party = (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    party->selectedHandle = (u8)handle;
}
#pragma pop

/**
 * mailGetSortMode - Waza get byte from battle party at offset 0x442.
 * Address: 0x801D1B4C | Size: 0x2C
 */
#pragma scheduling off
u8 mailGetSortMode(void) {
    WazaPartyScratch* party = (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    return party->selectedHandle;
}
#pragma scheduling on

/**
 * fn_801D1B78 - Waza effect position update (attacker-relative).
 * Address: 0x801D1B78 | Size: 0xA8
 */
void fn_801D1B78(s32 seqHandle) {
    /* TODO: Effect position update (0xA8 bytes) */
}

/**
 * fn_801D1C20 - Waza effect position update (target-relative).
 * Address: 0x801D1C20 | Size: 0xA4
 */
void fn_801D1C20(s32 seqHandle) {
    /* TODO: Effect position update target-relative (0xA4 bytes) */
}

/**
 * mailChkReceiveMail - Waza effect trajectory calculation.
 * Address: 0x801D1CC4 | Size: 0x94
 */
#pragma push
#pragma peephole off
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
#pragma pop

/**
 * mailAddMailbox - Waza projectile update.
 * Address: 0x801D1D58 | Size: 0xF8
 */
void mailAddMailbox(s32 seqHandle) {
    /* TODO: Waza projectile update (0xF8 bytes) */
}

/**
 * mailGetReceiveNumber - Mail receive-order query by mailbox ID.
 * Address: 0x801D1E50 | Size: 0xBC
 */
s32 mailGetReceiveNumber(s32 mailId)
{
    WazaPartyScratch* party;
    s32 idx;
    BOOL found;
    s32 currentId;

    found = FALSE;
    idx = 0;

    while (idx < (s32)((WazaPartyScratch*)savedataGetStatus(0, 0x0A))->count && !found) {
        party = (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
        if (idx < 0 ||
            idx >= (s32)((WazaPartyScratch*)savedataGetStatus(0, 0x0A))->count) {
            currentId = -1;
        } else {
            currentId = party->seqIds[idx];
        }

        if (mailId == currentId) {
            found = TRUE;
        }

        idx++;
    }

    if (!found) {
        return -1;
    }
    return idx;
}

/**
 * mailGetMailIDInMailbox - Waza get party entry by index from u16 array.
 * Address: 0x801D1F0C | Size: 0x70
 * Gets party via savedataGetStatus(0, 0xA), bounds-checks idx against party+0x400,
 * returns u16 at party[idx*2].
 */
#pragma push
#pragma peephole off
s32 mailGetMailIDInMailbox(s32 idx) {
    WazaPartyScratch* party;
    WazaPartyScratch* countParty;
    u16 count;
    party = (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    if (idx < 0) goto neg1;
    countParty = (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    count = countParty->count;
    if (idx < (s32)count) goto load;
neg1:
    return -1;
load:
    return party->seqIds[idx];
}
#pragma pop

/**
 * mailGetNbMailInMailbox - Waza get active effect count from party+0x400.
 * Address: 0x801D1F7C | Size: 0x2C
 */
#pragma scheduling off
u16 mailGetNbMailInMailbox(void) {
    WazaPartyScratch* party = (WazaPartyScratch*)savedataGetStatus(0, 0x0A);
    return party->count;
}
#pragma scheduling on

/**
 * mailInitMailbox - Waza effect color modulation.
 * Address: 0x801D1FA8 | Size: 0xD8
 */
void mailInitMailbox(s32 seqHandle, u32 color) {
    /* TODO: Effect color modulation (0xD8 bytes) */
}
