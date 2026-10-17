/**
 * @file menu_exact_80077A5C.c
 * @brief menuCBRule accessors, 0x80077A5C - 0x80077ED4.
 *
 * Function-boundary carve of the menu range bucket (menu_range_8007109C.c,
 * chunk menu_candidate_80075390): the tail of the battle-rule module, from
 * the blank-slot check to the rule copy/compare helpers. In Pokemon XD the
 * same run is menuCBRule_IsBlankPokemon .. menuCBRule_IsEquals, the end of
 * menuCBRule (TeamOrre/xd-decomp symbols.txt, 0x8004CCB0-0x8004D394);
 * menuCBRule_GetBattleTimeLimit, menuCBRule_CheckValidItem and
 * menuCBRule_ConstantRule keep their names here.
 *
 * No jump table (the switches are compare trees), no pooled constant. The
 * data is kept extern: the rule presets lbl_80268940 (.rodata, a named
 * 0x108-byte object in rodata_8026860C.c, three 0x54-byte rules), the
 * regulation item list lbl_802EE458 (.data) and its count lbl_80478928
 * (.sdata). GC/1.3 -O4,p with the chunk's unit-wide -opt nopeephole, no
 * pragmas.
 */
#include "dolphin/types.h"

extern s32 pokemonGetStatus(void* pokemon, s32 index, s32 field, s32 subindex);
extern s32* savedataGetStatus(s32 side, s32 slotType);
extern u8 fn_80142984(u16 item);
extern void* memcpy(void* dst, const void* src, u32 size);
extern s32 memcmp(const void* s1, const void* s2, u32 size);

/* fn_80077A5C (0x80077A5C): accept an empty slot or a zero species value. */
u8 fn_80077A5C(void* pokemon) {
    s32 result;

    result = 0;
    if (pokemon == 0 || pokemonGetStatus(pokemon, 0, 0x6E, 0) == 0) {
        result = 1;
    }
    return result;
}

extern u8* savedataBattleGetRulePtr(void);

/* fn_80077AAC..fn_80077B60 (0x80077AAC-0x80077B60): fixed-index byte
 * accessors into the savedataBattleGetRulePtr() record. */
u8 fn_80077AAC(void) { return savedataBattleGetRulePtr()[0x13]; }

u8 fn_80077AD0(void) { return savedataBattleGetRulePtr()[0x12]; }

u8 fn_80077AF4(void) { return savedataBattleGetRulePtr()[0x11]; }

u8 fn_80077B18(void) { return savedataBattleGetRulePtr()[0x10]; }

u8 fn_80077B3C(void) { return savedataBattleGetRulePtr()[0xf]; }

u8 fn_80077B60(void) { return savedataBattleGetRulePtr()[0xe]; }

/* menuCBRule_GetCommandTimeLimit (0x80077B84): fixed-index s16 accessor into the same record. */
s16 menuCBRule_GetCommandTimeLimit(void) { return ((s16*)savedataBattleGetRulePtr())[0xb]; }

/* menuCBRule_GetBattleTimeLimit (0x80077BA8): same shape, scaled by 0x3c. */
s32 menuCBRule_GetBattleTimeLimit(void) {
    return ((s16*)savedataBattleGetRulePtr())[0xa] * 0x3c;
}

/* fn_80077BD0 (0x80077BD0): accept initialized save-status values. */
u8 fn_80077BD0(void) {
    s32 value;

    value = savedataGetStatus(0, 0xE)[2];
    switch (value) {
    case 0:
    case 1:
    case 2:
        return 1;
    }
    return 0;
}

/* menuCBRule_CheckValidItem (0x80077C1C): handle sentinel item ids locally. */
u8 menuCBRule_CheckValidItem(u16 item) {
    switch (item) {
    case 0:
        return 1;
    case 0xAF:
        return 0;
    default:
        return fn_80142984(item);
    }
}

extern u32 lbl_80478928;
extern u16 lbl_802EE458[];

typedef struct MenuRuleItemRestrictions {
    u8 pad_00[8];
    s32 mode;
    u8 pad_0C[0xC];
    u8 item_disabled[0x3C];
} MenuRuleItemRestrictions;

/* fn_80077C68 (0x80077C68): apply the current rule's item restriction. */
u8 fn_80077C68(u16 item) {
    MenuRuleItemRestrictions* rule;
    u32 i;

    rule = (MenuRuleItemRestrictions*)savedataBattleGetRulePtr();
    if (menuCBRule_CheckValidItem(item) == 0) {
        return 0;
    }

    switch (rule->mode) {
    case 0:
        return 1;
    case 1:
        return item == 0;
    case 2:
        for (i = 0; i < lbl_80478928; i++) {
            if (item == lbl_802EE458[i]) {
                return rule->item_disabled[i] == 0;
            }
        }
        return 1;
    default:
        return 0;
    }
}

/* fn_80077D88 (0x80077D88): bounds-checked table lookup. */
u16 fn_80077D88(s32 index) {
    if (index < 0 || lbl_80478928 <= (u32)index) {
        return 0;
    }
    return lbl_802EE458[index];
}

/* savedataBattleGetEntryCount (0x80077DB8): map the current save state to a rule value. */
s32 savedataBattleGetEntryCount(void) {
    s32* entry;
    s32 state;

    entry = savedataGetStatus(0, 0xE);
    if (entry[0] == 2) {
        entry = savedataGetStatus(0, 0xE);
        if (entry[2] == 0) {
            return 6;
        }
    }

    entry = savedataGetStatus(0, 0xE);
    state = entry[1];
    switch (state) {
    case 0:
        return 3;
    case 1:
        return 4;
    case 2:
        break;
    default:
        break;
    }
    return 2;
}

extern u8 lbl_80268940[];

/* menuCBRule_ConstantRule (0x80077E50): fixed-slot table lookup, NULL out of range. */
void* menuCBRule_ConstantRule(s32 index) {
    switch (index) {
    case 0:
    case 1:
    case 2:
        return lbl_80268940 + (index * 0x54);
    }
    return (void*)0;
}

/* fn_80077E80 (0x80077E80): fixed-size record copy. */
void fn_80077E80(void* dst, void* src) {
    memcpy(dst, src, 0x54);
}

/* fn_80077EA4 (0x80077EA4): fixed-size record equality check. */
u8 fn_80077EA4(u16* s1, u16* s2) {
    return memcmp(s1, s2, 0x54) == 0;
}

