#include "dolphin/types.h"

#pragma section ".data"

extern u8 lbl_8038FFFC[];
extern void* lbl_80399F58[];
extern void* jumptable_80399FF4[];
extern void* jumptable_8039A028[];
extern void* jumptable_8039A048[];
extern void* jumptable_8039A068[];
extern void* jumptable_8039A088[];
extern void* jumptable_8039A0A8[];
extern void* jumptable_8039A0F0[];
extern void* jumptable_8039A110[];
extern void* jumptable_8039A130[];
extern void* jumptable_8039A1C8[];
extern void* jumptable_8039A200[];
extern void* jumptable_8039A220[];
extern void* jumptable_8039A2F4[];
extern void* jumptable_8039A314[];
extern void* jumptable_8039A388[];
extern void* jumptable_8039A3A8[];
extern void* jumptable_8039A3C8[];
extern void* jumptable_8039A478[];
extern void* jumptable_8039A578[];
extern void* jumptable_8039A598[];
extern void* jumptable_8039A5B8[];
extern void* jumptable_8039A5D8[];
extern u8 lbl_8039A648[];
extern u8 lbl_8039A664[];

extern u8 fightSeqCondChgActTypeToPokemonStatusId[];
extern u8 fightTrainerAiWazaValueHimitunotikara[];
extern u8 wsCmdEF[];
extern u8 wsCmdB4[];
extern u8 wsStatBuffChange[];
extern u8 wsCmd76[];
extern u8 wsCmd49[];
extern u8 wsCmd48[];
extern u8 wsPlayStatChangeAnimation[];
extern u8 wsPlayAnimation[];
extern u8 wsCmd20[];
extern u8 fightSeqSetTsuikaKouka[];
extern u8 fightSeqSelectTarget[];
extern u8 _fightOutPokemonSoubiItemEffectSub[];
extern u8 fightOutPokemonDoSoubiItemCheck[];
extern u8 _wsTokuseiAfterDamage[];
extern u8 _wsHitCheckJoutaiSub[];
extern u8 fn_8023B498[];
extern u8 fightTrainerAiGetWazaDamageValue[];
extern u8 fightTrainerAiGetWazaHitValue[];
extern u8 lbl_80379452[];
extern u8 lbl_80379478[];
extern u8 lbl_8037948D[];
extern u8 lbl_803794A2[];
extern u8 lbl_803794B7[];
extern u8 lbl_803794CC[];
extern u8 lbl_803794E0[];
extern u8 lbl_803794F5[];
extern u8 lbl_80379509[];
extern u8 lbl_8037951B[];
extern u8 lbl_8037952F[];

/*
 * Residual data around the compiler-owned jump tables emitted with
 * wsPlayStatChangeAnimation and wsPlayAnimation.
 */

#if !defined(DATA_8039A200_ONLY)
void* jumptable_8039A048[8] = {
    (void*)((u8*)wsStatBuffChange + 0x104),
    (void*)((u8*)wsStatBuffChange + 0xCC),
    (void*)((u8*)wsStatBuffChange + 0xD4),
    (void*)((u8*)wsStatBuffChange + 0xDC),
    (void*)((u8*)wsStatBuffChange + 0xE4),
    (void*)((u8*)wsStatBuffChange + 0xEC),
    (void*)((u8*)wsStatBuffChange + 0xF4),
    (void*)((u8*)wsStatBuffChange + 0xFC),
};

void* jumptable_8039A068[8] = {
    (void*)((u8*)fightSeqCondChgActTypeToPokemonStatusId + 0x5C),
    (void*)((u8*)fightSeqCondChgActTypeToPokemonStatusId + 0x24),
    (void*)((u8*)fightSeqCondChgActTypeToPokemonStatusId + 0x2C),
    (void*)((u8*)fightSeqCondChgActTypeToPokemonStatusId + 0x34),
    (void*)((u8*)fightSeqCondChgActTypeToPokemonStatusId + 0x3C),
    (void*)((u8*)fightSeqCondChgActTypeToPokemonStatusId + 0x44),
    (void*)((u8*)fightSeqCondChgActTypeToPokemonStatusId + 0x4C),
    (void*)((u8*)fightSeqCondChgActTypeToPokemonStatusId + 0x54),
};

void* jumptable_8039A088[8] = {
    (void*)((u8*)wsCmd76 + 0x6C),
    (void*)((u8*)wsCmd76 + 0x78),
    (void*)((u8*)wsCmd76 + 0x25C),
    (void*)((u8*)wsCmd76 + 0x154),
    (void*)((u8*)wsCmd76 + 0x194),
    (void*)((u8*)wsCmd76 + 0x25C),
    (void*)((u8*)wsCmd76 + 0x1CC),
    (void*)((u8*)wsCmd76 + 0x24C),
};

void* jumptable_8039A0A8[18] = {
    (void*)((u8*)wsCmd49 + 0x1C0),
    (void*)((u8*)wsCmd49 + 0x290),
    (void*)((u8*)wsCmd49 + 0x370),
    (void*)((u8*)wsCmd49 + 0x38C),
    (void*)((u8*)wsCmd49 + 0x3B4),
    (void*)((u8*)wsCmd49 + 0x3DC),
    (void*)((u8*)wsCmd49 + 0x3F8),
    (void*)((u8*)wsCmd49 + 0x4F0),
    (void*)((u8*)wsCmd49 + 0x518),
    (void*)((u8*)wsCmd49 + 0x540),
    (void*)((u8*)wsCmd49 + 0x55C),
    (void*)((u8*)wsCmd49 + 0x5C4),
    (void*)((u8*)wsCmd49 + 0x664),
    (void*)((u8*)wsCmd49 + 0x6E4),
    (void*)((u8*)wsCmd49 + 0x70C),
    (void*)((u8*)wsCmd49 + 0xA68),
    (void*)((u8*)wsCmd49 + 0xAF0),
    (void*)((u8*)wsCmd49 + 0xC08),
};

void* jumptable_8039A0F0[8] = {
    (void*)((u8*)wsCmd48 + 0x244),
    (void*)((u8*)wsCmd48 + 0x20C),
    (void*)((u8*)wsCmd48 + 0x214),
    (void*)((u8*)wsCmd48 + 0x21C),
    (void*)((u8*)wsCmd48 + 0x224),
    (void*)((u8*)wsCmd48 + 0x22C),
    (void*)((u8*)wsCmd48 + 0x234),
    (void*)((u8*)wsCmd48 + 0x23C),
};

void* jumptable_8039A110[8] = {
    (void*)((u8*)wsCmd48 + 0xEC),
    (void*)((u8*)wsCmd48 + 0xB4),
    (void*)((u8*)wsCmd48 + 0xBC),
    (void*)((u8*)wsCmd48 + 0xC4),
    (void*)((u8*)wsCmd48 + 0xCC),
    (void*)((u8*)wsCmd48 + 0xD4),
    (void*)((u8*)wsCmd48 + 0xDC),
    (void*)((u8*)wsCmd48 + 0xE4),
};
#endif

#if defined(DATA_8039A200_ONLY)
#if !defined(DATA_8039A220_ONLY)
void* jumptable_8039A200[8] = {
    (void*)((u8*)wsCmd20 + 0x88),
    (void*)((u8*)wsCmd20 + 0x50),
    (void*)((u8*)wsCmd20 + 0x58),
    (void*)((u8*)wsCmd20 + 0x60),
    (void*)((u8*)wsCmd20 + 0x68),
    (void*)((u8*)wsCmd20 + 0x70),
    (void*)((u8*)wsCmd20 + 0x78),
    (void*)((u8*)wsCmd20 + 0x80),
};
#endif

#if !defined(DATA_8039A538_ONLY)
/* 0x8039A220 - 0x8039A388 is owned by fight_range_8022B2CC.c. */
#if !defined(DATA_8039A388_ONLY)
void* jumptable_8039A220[53] = {
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xBC8),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xC50),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xE1C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xD04),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xD88),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xE70),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xEB4),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xF70),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x11F0),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1234),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x126C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1450),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1494),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x14CC),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x14F0),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1514),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1584),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x15A8),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1610),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1668),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x175C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x175C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x175C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x175C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1738),
};

void* jumptable_8039A2F4[8] = {
    (void*)((u8*)fightSeqSelectTarget + 0xE4),
    (void*)((u8*)fightSeqSelectTarget + 0x26C),
    (void*)((u8*)fightSeqSelectTarget + 0x2D4),
    (void*)((u8*)fightSeqSelectTarget + 0x2AC),
    (void*)((u8*)fightSeqSelectTarget + 0x26C),
    (void*)((u8*)fightSeqSelectTarget + 0x2D4),
    (void*)((u8*)fightSeqSelectTarget + 0x26C),
    (void*)((u8*)fightSeqSelectTarget + 0x26C),
};

void* jumptable_8039A314[29] = {
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x9C),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x208),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0xE0),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x180),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x1C4),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x258),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x304),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x4B0),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x520),
    (void*)((u8*)_fightOutPokemonSoubiItemEffectSub + 0x29C),
};

#endif

/* 0x8039A388 - 0x8039A478 (fightOutPokemonDoSoubiItemCheck's three switch tables) is owned by
 * fight_range_8022BE2C.c. */

#endif

/* 0x8039A478 - 0x8039A538 is _wsTokuseiAfterDamage's switch table, owned by
 * fight_range_8022D6BC.c. */
#if !defined(DATA_8039A220_ONLY)
void* jumptable_8039A478[48] = {
    (void*)((u8*)_wsTokuseiAfterDamage + 0x3E0),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x15C),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x218),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x298),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x350),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x470),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x5E4),
    (void*)((u8*)_wsTokuseiAfterDamage + 0x500),
};

#endif

#if !defined(DATA_8039A220_PREFIX_ONLY)
#if !defined(DATA_8039A648_ONLY)
/* 0x8039A538 - 0x8039A578 (_wsHitCheckJoutaiSub's switch table) is owned by
 * fight_range_8022F2F8.c. */

/* 0x8039A578 - 0x8039A5D8 (the switch tables of fn_8023B498, fightTrainerAiGetWazaDamageValue and
 * fightTrainerAiGetWazaHitValue) is owned by fight_range_8023B498.c. */

#endif

/* 0x8039A5D8 - 0x8039A648 is fightTrainerAiWazaValueHimitunotikara's switch
 * table, owned by fight_trainer_ai_waza_value_candidate_80245FC4.c. */

#if defined(DATA_8039A648_ONLY)
u8 lbl_8039A648[28] = {
    0x43, 0xFA, 0x00, 0x00, 0x44, 0x7A, 0x00, 0x00, 0x44, 0xBB, 0x80, 0x00,
    0x44, 0xFA, 0x00, 0x00, 0x45, 0x3B, 0x80, 0x00, 0x45, 0x9C, 0x40, 0x00,
    0x42, 0xC8, 0x00, 0x00,
};

u8 lbl_8039A664[44] = {
    0x00, 0x00, 0x00, 0x00, 0x42, 0x48, 0x00, 0x00, 0x42, 0xC8, 0x00, 0x00,
    0x43, 0x48, 0x00, 0x00, 0x43, 0xC8, 0x00, 0x00, 0x44, 0x48, 0x00, 0x00,
    0x44, 0xC8, 0x00, 0x00, 0x44, 0xE1, 0x00, 0x00, 0x44, 0xFA, 0x00, 0x00,
    0x45, 0x3B, 0x80, 0x00, 0x45, 0x9C, 0x40, 0x00,
};
#endif
#endif
#endif
