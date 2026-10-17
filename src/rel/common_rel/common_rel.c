/**
 * @file common_rel.c
 * @brief REL 125 (common_rel): _prolog, _epilog, _unresolved and the table
 *        counts.
 *
 * Module 125 .text 0x0 - 0xA9C and .data 0x14A654 - 0x14A714. Built with SN
 * ProDG (GCC 2.95) at -O0 -G0, like the rest of the module (see
 * docs/REL_MODULES.md).
 *
 * The module is a set of global data tables. main.dol reaches each one
 * through a pair of its own .sbss pointers: _prolog points the first at the
 * table and the second at the table's element count (defined below), and
 * _epilog clears both. The comment on each pair in _prolog names a main.dol
 * function that reads it. The count at 0x14A6C4 (2451) is defined but never
 * published.
 */
#include "rel/common_rel.h"

/* main.dol .sbss: table pointer, count pointer */
extern void* lbl_80478E8C;
extern u32* lbl_80478E88;
extern void* lbl_80478EAC;
extern u32* lbl_80478EA8;
extern void* lbl_80478E74;
extern u32* lbl_80478E70;
extern void* lbl_80478F84;
extern u32* lbl_80478F80;
extern void* lbl_80478E0C;
extern u32* lbl_80478E08;
extern void* lbl_80478EF4;
extern u32* lbl_80478EF0;
extern void* lbl_80478EEC;
extern u32* lbl_80478EE8;
extern void* lbl_80478FBC;
extern u32* lbl_80478FB8;
extern void* lbl_80478ED4;
extern u32* lbl_80478ED0;
extern void* lbl_80478EE4;
extern u32* lbl_80478EE0;
extern void* lbl_80478EDC;
extern u32* lbl_80478ED8;
extern void* lbl_80478EFC;
extern u32* lbl_80478EF8;
extern void* lbl_80478F1C;
extern u32* lbl_80478F18;
extern void* lbl_80478F3C;
extern u32* lbl_80478F38;
extern void* lbl_80478F4C;
extern u32* lbl_80478F48;
extern void* lbl_80478ECC;
extern u32* lbl_80478EC8;
extern void* lbl_80478F44;
extern u32* lbl_80478F40;
extern void* lbl_80478E2C;
extern u32* lbl_80478E28;
extern void* lbl_80478E14;
extern u32* lbl_80478E10;
extern void* lbl_80478E24;
extern u32* lbl_80478E20;
extern void* lbl_80478E1C;
extern u32* lbl_80478E18;
extern void* lbl_80478F04;
extern u32* lbl_80478F00;
extern void* lbl_80478F24;
extern u32* lbl_80478F20;
extern void* lbl_80478F34;
extern u32* lbl_80478F30;
extern void* lbl_80478F14;
extern u32* lbl_80478F10;
extern void* lbl_80478F54;
extern u32* lbl_80478F50;
extern void* lbl_80478E34;
extern u32* lbl_80478E30;
extern void* lbl_80478F9C;
extern u32* lbl_80478F98;
extern void* lbl_80478F2C;
extern u32* lbl_80478F28;
extern void* lbl_80478F0C;
extern u32* lbl_80478F08;
extern void* lbl_80478EBC;
extern u32* lbl_80478EB8;
extern void* lbl_80478DFC;
extern u32* lbl_80478DF8;
extern void* lbl_80478E64;
extern u32* lbl_80478E60;
extern void* lbl_80478E6C;
extern u32* lbl_80478E68;
extern void* lbl_80478F94;
extern u32* lbl_80478F90;
extern void* lbl_80478E5C;
extern u32* lbl_80478E58;
extern void* lbl_80478E7C;
extern u32* lbl_80478E78;
extern void* lbl_80478FAC;
extern u32* lbl_80478FA8;
extern void* lbl_80478FA4;
extern u32* lbl_80478FA0;
extern void* lbl_80478FB4;
extern u32* lbl_80478FB0;
extern void* lbl_80478F6C;
extern u32* lbl_80478F68;
extern void* lbl_80478F64;
extern u32* lbl_80478F60;
extern void* lbl_80478F5C;
extern u32* lbl_80478F58;
extern void* lbl_80478EC4;
extern u32* lbl_80478EC0;
extern void* lbl_80478F7C;
extern u32* lbl_80478F78;
extern void* lbl_80478F74;
extern u32* lbl_80478F70;
extern void* lbl_80478E94;
extern u32* lbl_80478E90;

/* Tables defined by the other units of this module (the sound tables are
 * declared in rel/common_rel.h) */
extern u8 lbl_125_rodata_0[];
extern u8 lbl_125_rodata_8[];
extern u8 lbl_125_rodata_24[];
extern u8 lbl_125_rodata_2C[];
extern u8 lbl_125_rodata_5C[];
extern u8 lbl_125_data_0[];
extern u8 lbl_125_data_18[];
extern u8 lbl_125_data_44[];
extern u8 lbl_125_data_1A34[];
extern u8 lbl_125_data_8BCB4[];
extern u8 lbl_125_data_8BCD4[];
extern u8 lbl_125_data_8BCF4[];
extern u8 lbl_125_data_8F724[];
extern u8 lbl_125_data_8F910[];
extern u8 lbl_125_data_8F974[];
extern u8 lbl_125_data_8FE24[];
extern u8 lbl_125_data_90F4C[];
extern u8 lbl_125_data_911AC[];
extern u8 lbl_125_data_91246[];
extern u8 lbl_125_data_91334[];
extern u8 lbl_125_data_91346[];
extern u8 lbl_125_data_9164C[];
extern u8 lbl_125_data_91684[];
extern u8 lbl_125_data_9BD14[];
extern u8 lbl_125_data_9E5DC[];
extern u8 lbl_125_data_109FBC[];
extern u8 lbl_125_data_111E04[];
extern u8 lbl_125_data_118E4C[];
extern u8 lbl_125_data_11B980[];
extern u8 lbl_125_data_11BFAC[];
extern u8 lbl_125_data_11C7C4[];
extern u8 lbl_125_data_121614[];
extern u8 lbl_125_data_1219FC[];
extern u8 lbl_125_data_121A04[];
extern u8 lbl_125_data_13E430[];
extern u8 lbl_125_data_13E440[];
extern u8 lbl_125_data_13FE08[];
extern u8 lbl_125_data_1437F8[];
extern u8 lbl_125_data_1439D8[];
extern u8 lbl_125_data_144F10[];
extern u8 lbl_125_data_145ED8[];
extern u8 lbl_125_data_146364[];
extern u8 lbl_125_data_14A45C[];
extern u8 lbl_125_data_14A5F4[];
extern u8 lbl_125_data_14A602[];

/* Element counts of the tables above */
u32 lbl_125_data_14A654 = 6;
u32 lbl_125_data_14A658 = 22;
u32 lbl_125_data_14A65C = 830;
u32 lbl_125_data_14A660 = 607;
u32 lbl_125_data_14A664 = 8;
u32 lbl_125_data_14A668 = 2;
u32 lbl_125_data_14A66C = 4;
u32 lbl_125_data_14A670 = 196;
u32 lbl_125_data_14A674 = 7;
u32 lbl_125_data_14A678 = 4;
u32 lbl_125_data_14A67C = 4;
u32 lbl_125_data_14A680 = 54;
u32 lbl_125_data_14A684 = 41;
u32 lbl_125_data_14A688 = 5;
u32 lbl_125_data_14A68C = 50;
u32 lbl_125_data_14A690 = 183;
u32 lbl_125_data_14A694 = 19;
u32 lbl_125_data_14A698 = 77;
u32 lbl_125_data_14A69C = 119;
u32 lbl_125_data_14A6A0 = 9;
u32 lbl_125_data_14A6A4 = 386;
u32 lbl_125_data_14A6A8 = 7;
u32 lbl_125_data_14A6AC = 820;
u32 lbl_125_data_14A6B0 = 261;
u32 lbl_125_data_14A6B4 = 5510;
u32 lbl_125_data_14A6B8 = 566;
u32 lbl_125_data_14A6BC = 79;
u32 lbl_125_data_14A6C0 = 3593;
u32 lbl_125_data_14A6C4 = 2451;
u32 lbl_125_data_14A6C8 = 553;
u32 lbl_125_data_14A6CC = 79;
u32 lbl_125_data_14A6D0 = 74;
u32 lbl_125_data_14A6D4 = 358;
u32 lbl_125_data_14A6D8 = 25;
u32 lbl_125_data_14A6DC = 6;
u32 lbl_125_data_14A6E0 = 413;
u32 lbl_125_data_14A6E4 = 8;
u32 lbl_125_data_14A6E8 = 150;
u32 lbl_125_data_14A6EC = 1236;
u32 lbl_125_data_14A6F0 = 12;
u32 lbl_125_data_14A6F4 = 8;
u32 lbl_125_data_14A6F8 = 97;
u32 lbl_125_data_14A6FC = 505;
u32 lbl_125_data_14A700 = 97;
u32 lbl_125_data_14A704 = 594;
u32 lbl_125_data_14A708 = 51;
u32 lbl_125_data_14A70C = 7;
u32 lbl_125_data_14A710 = 8;

void _prolog(void)
{
    /* msgctrlPalette */
    lbl_80478E8C = lbl_125_data_0;
    lbl_80478E88 = &lbl_125_data_14A654;
    /* heroCountPartyInList */
    lbl_80478EAC = lbl_125_data_18;
    lbl_80478EA8 = &lbl_125_data_14A658;
    /* _menuFaceBiosGetPtr__FUs */
    lbl_80478E74 = lbl_125_data_44;
    lbl_80478E70 = &lbl_125_data_14A65C;
    /* charNameBiosGetHearFlag */
    lbl_80478F84 = lbl_125_data_1A34;
    lbl_80478F80 = &lbl_125_data_14A660;
    /* tableResBiosGetResPtr */
    lbl_80478E0C = lbl_125_data_8BCB4;
    lbl_80478E08 = &lbl_125_data_14A664;
    /* fn_8018FE30 */
    lbl_80478EF4 = lbl_125_rodata_0;
    lbl_80478EF0 = &lbl_125_data_14A668;
    /* GSflagClear */
    lbl_80478EEC = lbl_125_data_8BCD4;
    lbl_80478EE8 = &lbl_125_data_14A66C;
    /* cameraFindFloorEntry */
    lbl_80478FBC = lbl_125_data_8BCF4;
    lbl_80478FB8 = &lbl_125_data_14A670;
    /* fn_8018FE30 */
    lbl_80478ED4 = lbl_125_rodata_8;
    lbl_80478ED0 = &lbl_125_data_14A674;
    /* fn_8018FE30 */
    lbl_80478EE4 = lbl_125_rodata_24;
    lbl_80478EE0 = &lbl_125_data_14A678;
    lbl_80478EDC = lbl_125_rodata_2C;
    lbl_80478ED8 = &lbl_125_data_14A67C;
    /* fn_80009178 */
    lbl_80478EFC = lbl_125_rodata_5C;
    lbl_80478EF8 = &lbl_125_data_14A680;
    /* fightTrainerKindDataBiosGetPtr */
    lbl_80478F1C = lbl_125_data_8F724;
    lbl_80478F18 = &lbl_125_data_14A684;
    /* fightSideDataBiosGetPtr */
    lbl_80478F3C = lbl_125_data_8F910;
    lbl_80478F38 = &lbl_125_data_14A688;
    /* _dbgMenuFightGetFightFloorDataIdSub */
    lbl_80478F4C = lbl_125_data_8F974;
    lbl_80478F48 = &lbl_125_data_14A68C;
    /* _floorInitialize__FUi14FloorEnterMode */
    lbl_80478ECC = lbl_125_data_8FE24;
    lbl_80478EC8 = &lbl_125_data_14A690;
    /* _dbgMenuFightGetFightKindDataIdSub */
    lbl_80478F44 = lbl_125_data_90F4C;
    lbl_80478F40 = &lbl_125_data_14A694;
    /* fn_8000C92C */
    lbl_80478E2C = lbl_125_data_911AC;
    lbl_80478E28 = &lbl_125_data_14A698;
    /* dbgMenuSoundTestSE2 */
    lbl_80478E14 = lbl_125_data_91246;
    lbl_80478E10 = &lbl_125_data_14A69C;
    /* dbgMenuSoundTestSEExclusive */
    lbl_80478E24 = lbl_125_data_91334;
    lbl_80478E20 = &lbl_125_data_14A6A0;
    /* dbgMenuSoundTestSE */
    lbl_80478E1C = lbl_125_data_91346;
    lbl_80478E18 = &lbl_125_data_14A6A4;
    /* fightTypeDataBiosGetPtr */
    lbl_80478F04 = lbl_125_data_9164C;
    lbl_80478F00 = &lbl_125_data_14A6A8;
    /* fightTrainerDataBiosGetPtr */
    lbl_80478F24 = lbl_125_data_91684;
    lbl_80478F20 = &lbl_125_data_14A6AC;
    /* fightTrainerAiDataBiosGetPtr */
    lbl_80478F34 = lbl_125_data_9BD14;
    lbl_80478F30 = &lbl_125_data_14A6B0;
    /* fightTrainerPokemonDataBiosGetPtr */
    lbl_80478F14 = lbl_125_data_9E5DC;
    lbl_80478F10 = &lbl_125_data_14A6B4;
    /* fightEncountDataBiosGetPtr */
    lbl_80478F54 = lbl_125_data_109FBC;
    lbl_80478F50 = &lbl_125_data_14A6B8;
    /* fn_801653CC */
    lbl_80478E34 = lbl_125_data_111B8C;
    lbl_80478E30 = &lbl_125_data_14A6BC;
    /* GStaskGetLinkedEntryId */
    lbl_80478F9C = lbl_125_data_111E04;
    lbl_80478F98 = &lbl_125_data_14A6C0;
    /* _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub */
    lbl_80478F2C = lbl_125_data_118E4C;
    lbl_80478F28 = &lbl_125_data_14A6C8;
    /* _dbgMenuFightGetFightTrainerPokemonPartDataIdSub */
    lbl_80478F0C = lbl_125_data_11B980;
    lbl_80478F08 = &lbl_125_data_14A6CC;
    /* floorEventChangeTresure */
    lbl_80478EBC = lbl_125_data_11BFAC;
    lbl_80478EB8 = &lbl_125_data_14A6D0;
    /* _fightMenuFightTrainerGcHeroOpenMenuSubWaza__FP13FIGHT_TRAINERP15FightOutPokemonUs */
    lbl_80478DFC = lbl_125_data_11C7C4;
    lbl_80478DF8 = &lbl_125_data_14A6D4;
    /* dbgMenuPokemonEdit */
    lbl_80478E64 = lbl_125_data_121614;
    lbl_80478E60 = &lbl_125_data_14A6D8;
    /* pokemonDpFilterDataBiosGetPtr */
    lbl_80478E6C = lbl_125_data_1219FC;
    lbl_80478E68 = &lbl_125_data_14A6DC;
    /* dbgMenuPokemonEdit */
    lbl_80478F94 = lbl_125_data_121A04;
    lbl_80478F90 = &lbl_125_data_14A6E0;
    /* pokemonSeikakuRateDataBiosGetPtr */
    lbl_80478E5C = lbl_125_data_13E430;
    lbl_80478E58 = &lbl_125_data_14A6E4;
    /* peopleInfoBiosGetPtr */
    lbl_80478E7C = lbl_125_data_13E440;
    lbl_80478E78 = &lbl_125_data_14A6E8;
    /* _sndSetVolumeWork */
    lbl_80478FAC = lbl_125_data_13FE08;
    lbl_80478FA8 = &lbl_125_data_14A6EC;
    /* _sndSetReverbParm */
    lbl_80478FA4 = lbl_125_data_1437F8;
    lbl_80478FA0 = &lbl_125_data_14A6F0;
    /* soundOpenWaveArchive */
    lbl_80478FB4 = lbl_125_data_143918;
    lbl_80478FB0 = &lbl_125_data_14A6F4;
    /* dbgMenuPokemonEdit */
    lbl_80478F6C = lbl_125_data_1439D8;
    lbl_80478F68 = &lbl_125_data_14A6F8;
    /* darkPokemonGetFlagListValue */
    lbl_80478F64 = lbl_125_data_144F10;
    lbl_80478F60 = &lbl_125_data_14A6FC;
    lbl_80478F5C = lbl_125_data_145ED8;
    lbl_80478F58 = &lbl_125_data_14A700;
    /* fn_80116D30 */
    lbl_80478EC4 = lbl_125_data_146364;
    lbl_80478EC0 = &lbl_125_data_14A704;
    /* darkPokemonGetTableEntry */
    lbl_80478F7C = lbl_125_data_14A45C;
    lbl_80478F78 = &lbl_125_data_14A708;
    /* darkPokemonGetIdByIndex */
    lbl_80478F74 = lbl_125_data_14A5F4;
    lbl_80478F70 = &lbl_125_data_14A70C;
    /* menuSeBiosGetPtr */
    lbl_80478E94 = lbl_125_data_14A602;
    lbl_80478E90 = &lbl_125_data_14A710;
}

void _epilog(void)
{
    lbl_80478E8C = NULL;
    lbl_80478E88 = NULL;
    lbl_80478EAC = NULL;
    lbl_80478EA8 = NULL;
    lbl_80478E74 = NULL;
    lbl_80478E70 = NULL;
    lbl_80478F84 = NULL;
    lbl_80478F80 = NULL;
    lbl_80478E0C = NULL;
    lbl_80478E08 = NULL;
    lbl_80478EF4 = NULL;
    lbl_80478EF0 = NULL;
    lbl_80478EEC = NULL;
    lbl_80478EE8 = NULL;
    lbl_80478FBC = NULL;
    lbl_80478FB8 = NULL;
    lbl_80478ED4 = NULL;
    lbl_80478ED0 = NULL;
    lbl_80478EE4 = NULL;
    lbl_80478EE0 = NULL;
    lbl_80478EDC = NULL;
    lbl_80478ED8 = NULL;
    lbl_80478EFC = NULL;
    lbl_80478EF8 = NULL;
    lbl_80478F1C = NULL;
    lbl_80478F18 = NULL;
    lbl_80478F3C = NULL;
    lbl_80478F38 = NULL;
    lbl_80478F4C = NULL;
    lbl_80478F48 = NULL;
    lbl_80478ECC = NULL;
    lbl_80478EC8 = NULL;
    lbl_80478F44 = NULL;
    lbl_80478F40 = NULL;
    lbl_80478E2C = NULL;
    lbl_80478E28 = NULL;
    lbl_80478E14 = NULL;
    lbl_80478E10 = NULL;
    lbl_80478E24 = NULL;
    lbl_80478E20 = NULL;
    lbl_80478E1C = NULL;
    lbl_80478E18 = NULL;
    lbl_80478F04 = NULL;
    lbl_80478F00 = NULL;
    lbl_80478F24 = NULL;
    lbl_80478F20 = NULL;
    lbl_80478F34 = NULL;
    lbl_80478F30 = NULL;
    lbl_80478F14 = NULL;
    lbl_80478F10 = NULL;
    lbl_80478F54 = NULL;
    lbl_80478F50 = NULL;
    lbl_80478E34 = NULL;
    lbl_80478E30 = NULL;
    lbl_80478F9C = NULL;
    lbl_80478F98 = NULL;
    lbl_80478F2C = NULL;
    lbl_80478F28 = NULL;
    lbl_80478F0C = NULL;
    lbl_80478F08 = NULL;
    lbl_80478EBC = NULL;
    lbl_80478EB8 = NULL;
    lbl_80478DFC = NULL;
    lbl_80478DF8 = NULL;
    lbl_80478E64 = NULL;
    lbl_80478E60 = NULL;
    lbl_80478E6C = NULL;
    lbl_80478E68 = NULL;
    lbl_80478F94 = NULL;
    lbl_80478F90 = NULL;
    lbl_80478E5C = NULL;
    lbl_80478E58 = NULL;
    lbl_80478E7C = NULL;
    lbl_80478E78 = NULL;
    lbl_80478FAC = NULL;
    lbl_80478FA8 = NULL;
    lbl_80478FA4 = NULL;
    lbl_80478FA0 = NULL;
    lbl_80478FB4 = NULL;
    lbl_80478FB0 = NULL;
    lbl_80478F6C = NULL;
    lbl_80478F68 = NULL;
    lbl_80478F64 = NULL;
    lbl_80478F60 = NULL;
    lbl_80478F5C = NULL;
    lbl_80478F58 = NULL;
    lbl_80478EC4 = NULL;
    lbl_80478EC0 = NULL;
    lbl_80478F7C = NULL;
    lbl_80478F78 = NULL;
    lbl_80478F74 = NULL;
    lbl_80478F70 = NULL;
    lbl_80478E94 = NULL;
    lbl_80478E90 = NULL;
}

void _unresolved(void)
{
}
