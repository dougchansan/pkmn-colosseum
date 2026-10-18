#include "dolphin/types.h"

typedef struct PdaModelWindow {
    u8 pad00[0x94];
    s8 variant;
} PdaModelWindow;

typedef struct PdaListEntry {
    u16 field_00;
    u16 battleId;
} PdaListEntry;

extern PdaModelWindow lbl_803A6748;
extern u32 lbl_8047A4D0;
extern PdaListEntry* lbl_8047A4D4;
extern s32 GSmsgGetGSchar(s32 message);
extern void* fightTrainerDataBiosGetPtr();
extern u16 fightTrainerDataBiosGetKindDataId(void* trainer);
extern u32 fightTrainerKindDataBiosGetPrefixName(void* kind);
extern void* fightTrainerKindDataBiosGetPtr(u16 kind);
extern u8 darkPokemonSaveGet49C(u16 id);
extern s32 darkPokemonGetFlagListValue(u16 id, s8* variant);
extern u32 darkPokemonSaveGetC(u16 id);
extern u16 fn_801EEFAC(u16 id, s32 side);

/* RULE-EXCEPTION(user-approved): local peephole-control pragma - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 pdaSnagListGetTrainerName(s32 index)
{
    u16 battleId;
    void* kindData;
    void* trainer;
    u16 kind;
    s8* variant;
    u16 alternate;
    s32 message;
    if (lbl_8047A4D0 != 0) {
        battleId = lbl_8047A4D4[index].battleId;
        variant = &lbl_803A6748.variant;
        message = darkPokemonGetFlagListValue(battleId, variant);
        switch ((u8)*variant) {
        case 0:
        case 1:
            message = 0x3720;
            break;
        case 2:
            kind = fn_801EEFAC(battleId, 0);
            if (kind == 9) {
                kindData = fightTrainerKindDataBiosGetPtr(darkPokemonSaveGet49C(battleId));
            } else {
                alternate = fn_801EEFAC(battleId, 1);
                if (alternate != 0 && darkPokemonSaveGetC(battleId) != 3) {
                    kind = alternate;
                }
                trainer = fightTrainerDataBiosGetPtr(kind);
                kindData = fightTrainerKindDataBiosGetPtr(
                    fightTrainerDataBiosGetKindDataId(trainer));
            }
            if (battleId == 0x43) {
                message = 0x12C0;
            } else {
                message = fightTrainerKindDataBiosGetPrefixName(kindData);
            }
            break;
        }
        return GSmsgGetGSchar(message);
    }
    return 0;
}
#pragma pop
