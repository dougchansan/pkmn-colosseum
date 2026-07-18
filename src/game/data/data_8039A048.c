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
extern void* jumptable_8039A538[];
extern void* jumptable_8039A578[];
extern void* jumptable_8039A598[];
extern void* jumptable_8039A5B8[];
extern void* jumptable_8039A5D8[];
extern u8 lbl_8039A648[];
extern u8 lbl_8039A664[];

extern u8 fightSeqCondChgActTypeToPokemonStatusId[];
extern u8 fightTrainerAiWazaValueHimitunotikara[];
extern u8 fn_80213E94[];
extern u8 fn_80217D34[];
extern u8 fn_8021B910[];
extern u8 fn_8021D40C[];
extern u8 fn_8021FAD4[];
extern u8 fn_80220868[];
extern u8 fn_80220B8C[];
extern u8 fn_80221104[];
extern u8 fn_80223AF4[];
extern u8 fn_802249B8[];
extern u8 fn_8022B2CC[];
extern u8 fn_8022B5C8[];
extern u8 fn_8022BE2C[];
extern u8 fn_8022D6BC[];
extern u8 fn_8022F2F8[];
extern u8 fn_8023B498[];
extern u8 fn_8023C370[];
extern u8 fn_8023C530[];
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

/* Auto-carved .data unit 0x8039A048..0x8039A690 (23 objects). */

void* jumptable_8039A048[8] = {
    (void*)((u8*)fn_8021B910 + 0x104),
    (void*)((u8*)fn_8021B910 + 0xCC),
    (void*)((u8*)fn_8021B910 + 0xD4),
    (void*)((u8*)fn_8021B910 + 0xDC),
    (void*)((u8*)fn_8021B910 + 0xE4),
    (void*)((u8*)fn_8021B910 + 0xEC),
    (void*)((u8*)fn_8021B910 + 0xF4),
    (void*)((u8*)fn_8021B910 + 0xFC),
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
    (void*)((u8*)fn_8021D40C + 0x6C),
    (void*)((u8*)fn_8021D40C + 0x78),
    (void*)((u8*)fn_8021D40C + 0x25C),
    (void*)((u8*)fn_8021D40C + 0x154),
    (void*)((u8*)fn_8021D40C + 0x194),
    (void*)((u8*)fn_8021D40C + 0x25C),
    (void*)((u8*)fn_8021D40C + 0x1CC),
    (void*)((u8*)fn_8021D40C + 0x24C),
};

void* jumptable_8039A0A8[18] = {
    (void*)((u8*)fn_8021FAD4 + 0x1C0),
    (void*)((u8*)fn_8021FAD4 + 0x290),
    (void*)((u8*)fn_8021FAD4 + 0x370),
    (void*)((u8*)fn_8021FAD4 + 0x38C),
    (void*)((u8*)fn_8021FAD4 + 0x3B4),
    (void*)((u8*)fn_8021FAD4 + 0x3DC),
    (void*)((u8*)fn_8021FAD4 + 0x3F8),
    (void*)((u8*)fn_8021FAD4 + 0x4F0),
    (void*)((u8*)fn_8021FAD4 + 0x518),
    (void*)((u8*)fn_8021FAD4 + 0x540),
    (void*)((u8*)fn_8021FAD4 + 0x55C),
    (void*)((u8*)fn_8021FAD4 + 0x5C4),
    (void*)((u8*)fn_8021FAD4 + 0x664),
    (void*)((u8*)fn_8021FAD4 + 0x6E4),
    (void*)((u8*)fn_8021FAD4 + 0x70C),
    (void*)((u8*)fn_8021FAD4 + 0xA68),
    (void*)((u8*)fn_8021FAD4 + 0xAF0),
    (void*)((u8*)fn_8021FAD4 + 0xC08),
};

void* jumptable_8039A0F0[8] = {
    (void*)((u8*)fn_80220868 + 0x244),
    (void*)((u8*)fn_80220868 + 0x20C),
    (void*)((u8*)fn_80220868 + 0x214),
    (void*)((u8*)fn_80220868 + 0x21C),
    (void*)((u8*)fn_80220868 + 0x224),
    (void*)((u8*)fn_80220868 + 0x22C),
    (void*)((u8*)fn_80220868 + 0x234),
    (void*)((u8*)fn_80220868 + 0x23C),
};

void* jumptable_8039A110[8] = {
    (void*)((u8*)fn_80220868 + 0xEC),
    (void*)((u8*)fn_80220868 + 0xB4),
    (void*)((u8*)fn_80220868 + 0xBC),
    (void*)((u8*)fn_80220868 + 0xC4),
    (void*)((u8*)fn_80220868 + 0xCC),
    (void*)((u8*)fn_80220868 + 0xD4),
    (void*)((u8*)fn_80220868 + 0xDC),
    (void*)((u8*)fn_80220868 + 0xE4),
};

void* jumptable_8039A130[38] = {
    (void*)((u8*)fn_80220B8C + 0x150),
    (void*)((u8*)fn_80220B8C + 0x1C0),
    (void*)((u8*)fn_80220B8C + 0x230),
    (void*)((u8*)fn_80220B8C + 0x150),
    (void*)((u8*)fn_80220B8C + 0x1C0),
    (void*)((u8*)fn_80220B8C + 0x2A0),
    (void*)((u8*)fn_80220B8C + 0x1C0),
    (void*)((u8*)fn_80220B8C + 0x310),
    (void*)((u8*)fn_80220B8C + 0x380),
    (void*)((u8*)fn_80220B8C + 0x3F0),
    (void*)((u8*)fn_80220B8C + 0x310),
    (void*)((u8*)fn_80220B8C + 0x380),
    (void*)((u8*)fn_80220B8C + 0x460),
    (void*)((u8*)fn_80220B8C + 0x380),
    (void*)((u8*)fn_80220B8C + 0x4CC),
    (void*)((u8*)fn_80220B8C + 0x4CC),
    (void*)((u8*)fn_80220B8C + 0x4CC),
    (void*)((u8*)fn_80220B8C + 0x4CC),
    (void*)((u8*)fn_80220B8C + 0x4CC),
    (void*)((u8*)fn_80220B8C + 0x4CC),
    (void*)((u8*)fn_80220B8C + 0x4CC),
    (void*)((u8*)fn_80220B8C + 0x4CC),
    (void*)((u8*)fn_80220B8C + 0x4CC),
    (void*)((u8*)fn_80220B8C + 0x4CC),
    (void*)((u8*)fn_80220B8C + 0x150),
    (void*)((u8*)fn_80220B8C + 0x1C0),
    (void*)((u8*)fn_80220B8C + 0x230),
    (void*)((u8*)fn_80220B8C + 0x150),
    (void*)((u8*)fn_80220B8C + 0x1C0),
    (void*)((u8*)fn_80220B8C + 0x2A0),
    (void*)((u8*)fn_80220B8C + 0x1C0),
    (void*)((u8*)fn_80220B8C + 0x310),
    (void*)((u8*)fn_80220B8C + 0x380),
    (void*)((u8*)fn_80220B8C + 0x3F0),
    (void*)((u8*)fn_80220B8C + 0x310),
    (void*)((u8*)fn_80220B8C + 0x380),
    (void*)((u8*)fn_80220B8C + 0x460),
    (void*)((u8*)fn_80220B8C + 0x380),
};

void* jumptable_8039A1C8[14] = {
    (void*)((u8*)fn_80221104 + 0xEC),
    (void*)((u8*)fn_80221104 + 0xF4),
    (void*)((u8*)fn_80221104 + 0xFC),
    (void*)((u8*)fn_80221104 + 0xEC),
    (void*)((u8*)fn_80221104 + 0xF4),
    (void*)((u8*)fn_80221104 + 0x104),
    (void*)((u8*)fn_80221104 + 0xF4),
    (void*)((u8*)fn_80221104 + 0x10C),
    (void*)((u8*)fn_80221104 + 0x114),
    (void*)((u8*)fn_80221104 + 0x11C),
    (void*)((u8*)fn_80221104 + 0x10C),
    (void*)((u8*)fn_80221104 + 0x114),
    (void*)((u8*)fn_80221104 + 0x124),
    (void*)((u8*)fn_80221104 + 0x114),
};

void* jumptable_8039A200[8] = {
    (void*)((u8*)fn_80223AF4 + 0x88),
    (void*)((u8*)fn_80223AF4 + 0x50),
    (void*)((u8*)fn_80223AF4 + 0x58),
    (void*)((u8*)fn_80223AF4 + 0x60),
    (void*)((u8*)fn_80223AF4 + 0x68),
    (void*)((u8*)fn_80223AF4 + 0x70),
    (void*)((u8*)fn_80223AF4 + 0x78),
    (void*)((u8*)fn_80223AF4 + 0x80),
};

void* jumptable_8039A220[53] = {
    (void*)((u8*)fn_802249B8 + 0xBC8),
    (void*)((u8*)fn_802249B8 + 0xC50),
    (void*)((u8*)fn_802249B8 + 0xE1C),
    (void*)((u8*)fn_802249B8 + 0xD04),
    (void*)((u8*)fn_802249B8 + 0xD88),
    (void*)((u8*)fn_802249B8 + 0xE70),
    (void*)((u8*)fn_802249B8 + 0xEB4),
    (void*)((u8*)fn_802249B8 + 0xF70),
    (void*)((u8*)fn_802249B8 + 0x1020),
    (void*)((u8*)fn_802249B8 + 0x1020),
    (void*)((u8*)fn_802249B8 + 0x1020),
    (void*)((u8*)fn_802249B8 + 0x1020),
    (void*)((u8*)fn_802249B8 + 0x1020),
    (void*)((u8*)fn_802249B8 + 0x1020),
    (void*)((u8*)fn_802249B8 + 0x1020),
    (void*)((u8*)fn_802249B8 + 0x1094),
    (void*)((u8*)fn_802249B8 + 0x1094),
    (void*)((u8*)fn_802249B8 + 0x1094),
    (void*)((u8*)fn_802249B8 + 0x1094),
    (void*)((u8*)fn_802249B8 + 0x1094),
    (void*)((u8*)fn_802249B8 + 0x1094),
    (void*)((u8*)fn_802249B8 + 0x1094),
    (void*)((u8*)fn_802249B8 + 0x11F0),
    (void*)((u8*)fn_802249B8 + 0x1234),
    (void*)((u8*)fn_802249B8 + 0x126C),
    (void*)((u8*)fn_802249B8 + 0x1450),
    (void*)((u8*)fn_802249B8 + 0x1494),
    (void*)((u8*)fn_802249B8 + 0x14CC),
    (void*)((u8*)fn_802249B8 + 0x14F0),
    (void*)((u8*)fn_802249B8 + 0x1514),
    (void*)((u8*)fn_802249B8 + 0x1584),
    (void*)((u8*)fn_802249B8 + 0x15A8),
    (void*)((u8*)fn_802249B8 + 0x1108),
    (void*)((u8*)fn_802249B8 + 0x1108),
    (void*)((u8*)fn_802249B8 + 0x1108),
    (void*)((u8*)fn_802249B8 + 0x1108),
    (void*)((u8*)fn_802249B8 + 0x1108),
    (void*)((u8*)fn_802249B8 + 0x1108),
    (void*)((u8*)fn_802249B8 + 0x1108),
    (void*)((u8*)fn_802249B8 + 0x117C),
    (void*)((u8*)fn_802249B8 + 0x117C),
    (void*)((u8*)fn_802249B8 + 0x117C),
    (void*)((u8*)fn_802249B8 + 0x117C),
    (void*)((u8*)fn_802249B8 + 0x117C),
    (void*)((u8*)fn_802249B8 + 0x117C),
    (void*)((u8*)fn_802249B8 + 0x117C),
    (void*)((u8*)fn_802249B8 + 0x1610),
    (void*)((u8*)fn_802249B8 + 0x1668),
    (void*)((u8*)fn_802249B8 + 0x175C),
    (void*)((u8*)fn_802249B8 + 0x175C),
    (void*)((u8*)fn_802249B8 + 0x175C),
    (void*)((u8*)fn_802249B8 + 0x175C),
    (void*)((u8*)fn_802249B8 + 0x1738),
};

void* jumptable_8039A2F4[8] = {
    (void*)((u8*)fn_8022B2CC + 0xE4),
    (void*)((u8*)fn_8022B2CC + 0x26C),
    (void*)((u8*)fn_8022B2CC + 0x2D4),
    (void*)((u8*)fn_8022B2CC + 0x2AC),
    (void*)((u8*)fn_8022B2CC + 0x26C),
    (void*)((u8*)fn_8022B2CC + 0x2D4),
    (void*)((u8*)fn_8022B2CC + 0x26C),
    (void*)((u8*)fn_8022B2CC + 0x26C),
};

void* jumptable_8039A314[29] = {
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x9C),
    (void*)((u8*)fn_8022B5C8 + 0x208),
    (void*)((u8*)fn_8022B5C8 + 0xE0),
    (void*)((u8*)fn_8022B5C8 + 0x180),
    (void*)((u8*)fn_8022B5C8 + 0x1C4),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x258),
    (void*)((u8*)fn_8022B5C8 + 0x304),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x4B0),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x520),
    (void*)((u8*)fn_8022B5C8 + 0x29C),
};

void* jumptable_8039A388[8] = {
    (void*)((u8*)fn_8022BE2C + 0xCE4),
    (void*)((u8*)fn_8022BE2C + 0xCAC),
    (void*)((u8*)fn_8022BE2C + 0xCB4),
    (void*)((u8*)fn_8022BE2C + 0xCBC),
    (void*)((u8*)fn_8022BE2C + 0xCC4),
    (void*)((u8*)fn_8022BE2C + 0xCCC),
    (void*)((u8*)fn_8022BE2C + 0xCD4),
    (void*)((u8*)fn_8022BE2C + 0xCDC),
};

void* jumptable_8039A3A8[8] = {
    (void*)((u8*)fn_8022BE2C + 0xBF4),
    (void*)((u8*)fn_8022BE2C + 0xBBC),
    (void*)((u8*)fn_8022BE2C + 0xBC4),
    (void*)((u8*)fn_8022BE2C + 0xBCC),
    (void*)((u8*)fn_8022BE2C + 0xBD4),
    (void*)((u8*)fn_8022BE2C + 0xBDC),
    (void*)((u8*)fn_8022BE2C + 0xBE4),
    (void*)((u8*)fn_8022BE2C + 0xBEC),
};

void* jumptable_8039A3C8[44] = {
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x134),
    (void*)((u8*)fn_8022BE2C + 0xD54),
    (void*)((u8*)fn_8022BE2C + 0xEC0),
    (void*)((u8*)fn_8022BE2C + 0xD98),
    (void*)((u8*)fn_8022BE2C + 0xE38),
    (void*)((u8*)fn_8022BE2C + 0xE7C),
    (void*)((u8*)fn_8022BE2C + 0x1AC),
    (void*)((u8*)fn_8022BE2C + 0xF10),
    (void*)((u8*)fn_8022BE2C + 0xF54),
    (void*)((u8*)fn_8022BE2C + 0x420),
    (void*)((u8*)fn_8022BE2C + 0x4F0),
    (void*)((u8*)fn_8022BE2C + 0x5C0),
    (void*)((u8*)fn_8022BE2C + 0x690),
    (void*)((u8*)fn_8022BE2C + 0x760),
    (void*)((u8*)fn_8022BE2C + 0x830),
    (void*)((u8*)fn_8022BE2C + 0x8B8),
    (void*)((u8*)fn_8022BE2C + 0x940),
    (void*)((u8*)fn_8022BE2C + 0x9C8),
    (void*)((u8*)fn_8022BE2C + 0xA50),
    (void*)((u8*)fn_8022BE2C + 0xAD8),
    (void*)((u8*)fn_8022BE2C + 0xB38),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x32C),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x112C),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x1190),
    (void*)((u8*)fn_8022BE2C + 0x3A0),
};

void* jumptable_8039A478[48] = {
    (void*)((u8*)fn_8022D6BC + 0x3E0),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x15C),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x218),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x298),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x350),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x470),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x5E4),
    (void*)((u8*)fn_8022D6BC + 0x500),
};

void* jumptable_8039A538[16] = {
    (void*)((u8*)fn_8022F2F8 + 0xDC),
    (void*)((u8*)fn_8022F2F8 + 0x104),
    (void*)((u8*)fn_8022F2F8 + 0x270),
    (void*)((u8*)fn_8022F2F8 + 0x314),
    (void*)((u8*)fn_8022F2F8 + 0x3A0),
    (void*)((u8*)fn_8022F2F8 + 0x3F8),
    (void*)((u8*)fn_8022F2F8 + 0x468),
    (void*)((u8*)fn_8022F2F8 + 0x504),
    (void*)((u8*)fn_8022F2F8 + 0x57C),
    (void*)((u8*)fn_8022F2F8 + 0x5F0),
    (void*)((u8*)fn_8022F2F8 + 0x734),
    (void*)((u8*)fn_8022F2F8 + 0x7B8),
    (void*)((u8*)fn_8022F2F8 + 0x884),
    (void*)((u8*)fn_8022F2F8 + 0x9FC),
    (void*)((u8*)fn_8022F2F8 + 0xA58),
    (void*)((u8*)fn_8022F2F8 + 0xAC8),
};

void* jumptable_8039A578[8] = {
    (void*)((u8*)fn_8023B498 + 0x784),
    (void*)((u8*)fn_8023B498 + 0x7E8),
    (void*)((u8*)fn_8023B498 + 0x7A8),
    (void*)((u8*)fn_8023B498 + 0x784),
    (void*)((u8*)fn_8023B498 + 0x7A8),
    (void*)((u8*)fn_8023B498 + 0x7E8),
    (void*)((u8*)fn_8023B498 + 0x7A8),
    (void*)((u8*)fn_8023B498 + 0x7E8),
};

void* jumptable_8039A598[8] = {
    (void*)((u8*)fn_8023C370 + 0x100),
    (void*)((u8*)fn_8023C370 + 0x164),
    (void*)((u8*)fn_8023C370 + 0x124),
    (void*)((u8*)fn_8023C370 + 0x100),
    (void*)((u8*)fn_8023C370 + 0x124),
    (void*)((u8*)fn_8023C370 + 0x164),
    (void*)((u8*)fn_8023C370 + 0x124),
    (void*)((u8*)fn_8023C370 + 0x164),
};

void* jumptable_8039A5B8[8] = {
    (void*)((u8*)fn_8023C530 + 0xF0),
    (void*)((u8*)fn_8023C530 + 0x3F4),
    (void*)((u8*)fn_8023C530 + 0x258),
    (void*)((u8*)fn_8023C530 + 0xF0),
    (void*)((u8*)fn_8023C530 + 0x258),
    (void*)((u8*)fn_8023C530 + 0x3F4),
    (void*)((u8*)fn_8023C530 + 0x258),
    (void*)((u8*)fn_8023C530 + 0x3F4),
};

void* jumptable_8039A5D8[28] = {
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x1FC),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x64),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0xED8),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0xCB0),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0xEBC),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x86C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x64C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0xA30),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x106C),
    (void*)((u8*)fightTrainerAiWazaValueHimitunotikara + 0x410),
};

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
