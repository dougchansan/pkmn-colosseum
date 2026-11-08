/* Standalone copy of fn_8003B2D8 from pda_range_80037158.c, linked on its own. */
#include "dolphin/types.h"

typedef struct PdaListEntry {
    u16 field_00;
    u16 battleId;
} PdaListEntry;

extern f32 lbl_8047BAC0;
extern u8 lbl_803A6748[];
extern u32 lbl_8047A4D0;
extern PdaListEntry* lbl_8047A4D4;

extern u32 GSmsgGetGSchar(u32 msg);
extern void msgctrlSetValue(s32 id, u32 value);
extern void GSmsgPrintColor(s32 arg0, s32 arg1, s32 arg2, void* data);
extern u32 darkPokemonGetFlagListValue(u16 id, u8* variant);
extern u16 darkPokemonGetSpecies(u16 id, s32 side);
extern u8* darkPokemonSaveGetName(u16 id);
extern u32 fightTrainerDataBiosGetStatus03(u8* ptr);
extern void* fightTrainerDataBiosGetPtr(u16 slot);
extern s32 pdaSnagListGetTrainerName(s32 index);
extern s32 darkPokemonGetFlag1C(u16 id);
extern u32 darkPokemonSaveGet8(u16 id);
extern u8 darkPokemonCheckFlag16(u16 id);
extern u8 darkPokemonCheckFlag12(u16 id);
extern u8 darkPokemonCheckFlag14(u16 id);
extern u32 gamedataGetStatus(s32 a, s32 b);
extern u32 pokemonGetSex();
extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetDarkpokemonDataId(u32 work, s32 id);
extern void pokemonBiosSetDp(u32 work, s32 dp);

/* The owner caption: a trainer-specific name, or the battle message text for the record. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003B2D8_OwnerText in this standalone copy (fn_8003B2D8) - see docs/RULE_EXCEPTIONS.md */
static inline u32 pda_8003B2D8_OwnerText(void)
{
    u8* slot;
    u16 kind;
    u32 text;
    u32 ownerText;
    u8 special;
    u16 index;
    u32 trainerId;

    special = 0;
    index = (u16)*(u32*)lbl_803A6748;
    if (lbl_8047A4D0 != 0) {
        slot = lbl_803A6748 + 0x94;
        kind = lbl_8047A4D4[index].battleId;
        text = darkPokemonGetFlagListValue(kind, slot);
        switch (*slot) {
        case 0:
            text = 0x371F;
            break;
        case 1:
        case 2:
            if (kind == 0x43) {
                text = 0x12B0;
            } else if ((trainerId = darkPokemonGetSpecies(kind, 0)) == 9) {
                ownerText = (u32)darkPokemonSaveGetName(kind);
                special = 1;
            } else {
                text = fightTrainerDataBiosGetStatus03(fightTrainerDataBiosGetPtr(trainerId));
            }
            break;
        }
        if (special != 0) {
            return ownerText;
        }
        return GSmsgGetGSchar(text);
    }
    return 0;
}

void pdaSnagListDrawEntry(u8* context)
{
    u32 ownerText;
    u32 owner;
    u32 caption;

    context[0x8B] = lbl_8047BAC0 * *(f32*)(lbl_803A6748 + 0x44);
    ownerText = pda_8003B2D8_OwnerText();
    owner = ownerText;
    if (ownerText == 0) {
        owner = GSmsgGetGSchar(1);
    }
    caption = pdaSnagListGetTrainerName(*(u32*)lbl_803A6748);
    if (caption == 0) {
        caption = GSmsgGetGSchar(1);
    }
    msgctrlSetValue(0x37, caption);
    GSmsgPrintColor(0, 0, context[0x8B] | -0x100, (void*)0xE7);
    msgctrlSetValue(0x37, owner);
    GSmsgPrintColor(0xB4, 0, context[0x8B] | -0x100, (void*)0xE7);
}
