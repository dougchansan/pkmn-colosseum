/**
 * @file gba_misc_r58_8008C6FC_suffix.c
 * @brief gba_misc.c carve: fn_8008C6FC, fn_8008C700 and fn_8008C78C,
 * 0x8008C6FC - 0x8008C7B0.
 *
 * Standalone: gba_misc.c's declarations plus only these bodies, copied
 * from gba_misc.c.
 */

#include "dolphin/types.h"

typedef struct GbaMiscContext {
    u8 unk_0000[0x4000];
    u8 state_4000;       /* 0x4000 */
    u8 unk_4001[0x135];
    u8 tableKey_4136;    /* 0x4136 */
} GbaMiscContext;


/*
 * Layout of a single Pokemon record as the GBA cartridge stores it.
 * Multi-byte fields are kept in the GBA's little-endian order, so every
 * scalar written from GameCube data is byte-swapped on the way in.
 */
typedef struct GbaPokemonOrigins {
    /* 0x00 */ u16 otGender : 1;
    /* 0x00 */ u16 ballId : 4;
    /* 0x00 */ u16 gameId : 4;
    /* 0x00 */ u16 metLevel : 7;
    /* 0x02 */ u8 metLocation;
    /* 0x03 */ u8 pokerus;
} GbaPokemonOrigins;

typedef struct GbaPokemonIvs {
    /* 0x00 */ u32 altAbility : 1;
    /* 0x00 */ u32 isEgg : 1;
    /* 0x00 */ u32 spDefenseIv : 5;
    /* 0x00 */ u32 spAttackIv : 5;
    /* 0x00 */ u32 speedIv : 5;
    /* 0x00 */ u32 defenseIv : 5;
    /* 0x00 */ u32 attackIv : 5;
    /* 0x00 */ u32 hpIv : 5;
} GbaPokemonIvs;

typedef struct GbaPokemonRibbons {
    /* 0x00 */ u32 eventGet : 1;
    /* 0x00 */ u32 amariRibbon : 4;
    /* 0x00 */ u32 worldRibbon : 1;
    /* 0x00 */ u32 earthRibbon : 1;
    /* 0x00 */ u32 nationalRibbon : 1;
    /* 0x00 */ u32 countryRibbon : 1;
    /* 0x00 */ u32 skyRibbon : 1;
    /* 0x00 */ u32 landRibbon : 1;
    /* 0x00 */ u32 marineRibbon : 1;
    /* 0x00 */ u32 ganbaRibbon : 1;
    /* 0x00 */ u32 bromideRibbon : 1;
    /* 0x00 */ u32 victoryRibbon : 1;
    /* 0x00 */ u32 winningRibbon : 1;
    /* 0x00 */ u32 champRibbon : 1;
    /* 0x00 */ u32 strongMedal : 3;
    /* 0x00 */ u32 cleverMedal : 3;
    /* 0x00 */ u32 cuteMedal : 3;
    /* 0x00 */ u32 beautifulMedal : 3;
    /* 0x00 */ u32 styleMedal : 3;
} GbaPokemonRibbons;

/* The three words that make up the GBA "misc" substruct, in GameCube order. */
typedef struct GbaPokemonMisc {
    /* 0x00 */ GbaPokemonRibbons ribbons;
    /* 0x04 */ GbaPokemonIvs ivs;
    /* 0x08 */ GbaPokemonOrigins origins;
} GbaPokemonMisc;

typedef struct GbaPokemon {
    /* 0x00 */ u32 personality;
    /* 0x04 */ u32 otId;
    /* 0x08 */ u8 nickname[10];
    /* 0x12 */ u8 language;
    /* 0x13 */ u8 amariFlags : 5;
    /* 0x13 */ u8 isEgg : 1;
    /* 0x13 */ u8 hasSpecies : 1;
    /* 0x13 */ u8 isBadEgg : 1;
    /* 0x14 */ u8 otName[7];
    /* 0x1B */ u8 markings;
    /* 0x1C */ u16 checksum;
    /* 0x1E */ u16 amari;
    /* 0x20 */ u16 species;
    /* 0x22 */ u16 heldItem;
    /* 0x24 */ u32 experience;
    /* 0x28 */ u8 ppBonuses;
    /* 0x29 */ u8 friendship;
    /* 0x2A */ u16 growthAmari;
    /* 0x2C */ u16 moves[4];
    /* 0x34 */ u8 pp[4];
    /* 0x38 */ u8 hpEffort;
    /* 0x39 */ u8 phyAtkEffort;
    /* 0x3A */ u8 phyDefEffort;
    /* 0x3B */ u8 nimblenessEffort;
    /* 0x3C */ u8 speAtkEffort;
    /* 0x3D */ u8 speDefEffort;
    /* 0x3E */ u8 style;
    /* 0x3F */ u8 beautiful;
    /* 0x40 */ u8 cute;
    /* 0x41 */ u8 clever;
    /* 0x42 */ u8 strong;
    /* 0x43 */ u8 fur;
    /* 0x44 */ u32 miscWord0;
    /* 0x48 */ u32 miscWord1;
    /* 0x4C */ u32 miscWord2;
    /* 0x50 */ u32 status;
    /* 0x54 */ u8 level;
    /* 0x55 */ u8 mailId;
    /* 0x56 */ u16 hp;
    /* 0x58 */ u16 maxHp;
    /* 0x5A */ u16 phyAtk;
    /* 0x5C */ u16 phyDef;
    /* 0x5E */ u16 nimbleness;
    /* 0x60 */ u16 speAtk;
    /* 0x62 */ u16 speDef;
} GbaPokemon;

/*
 * Source-level names only: the macros preserve the original fn_* linker
 * symbols for objdiff while documenting the recovered behavior.
 */
#define GbaMisc_GetMappedContextByte std213_gbaGetLinkedGameId
#define GbaMisc_HasActiveContextState std212_gbaIsLinkActive
#define GbaMisc_PollEntryStatusA fn_80089CA8
#define GbaMisc_ResetEntryStatusA fn_80089D30
#define GbaMisc_GetEntryStatus fn_8008A9E4
#define GbaMisc_SendPackedEntryStatus fn_8008AB20
#define GbaMisc_SetEntryState gbaCommandSetKeyState
#define GbaMisc_RunFlagDispatch fn_8008C700

#define GBA_MISC_ENTRY_WORD_OFFSET(idx) ((idx) << 2)
#define GBA_MISC_ENTRY_HALF_OFFSET(idx) ((idx) << 1)
#define GbaMisc_EntryStateAtWordOffset(offset) \
    (*(s32*)((u8*)&lbl_803FB318 + (offset) + (-4)))
#define GbaMisc_EntryCachedStatusAtWordOffset(offset) \
    (*(s32*)((u8*)&lbl_803FB308 + (offset) + (-4)))
#define GbaMisc_EntryCounterAAtHalfOffset(offset) \
    (*(u16*)((u8*)&lbl_8047A684 + (offset) + (-2)))
#define GbaMisc_EntryCounterBAtHalfOffset(offset) \
    (*(u16*)((u8*)&lbl_8047A67C + (offset) + (-2)))
#define GbaMisc_EntryState(idx) \
    GbaMisc_EntryStateAtWordOffset(GBA_MISC_ENTRY_WORD_OFFSET(idx))
#define GbaMisc_EntryCachedStatus(idx) \
    GbaMisc_EntryCachedStatusAtWordOffset(GBA_MISC_ENTRY_WORD_OFFSET(idx))
#define GbaMisc_EntryCounterA(idx) \
    GbaMisc_EntryCounterAAtHalfOffset(GBA_MISC_ENTRY_HALF_OFFSET(idx))
#define GbaMisc_EntryCounterB(idx) \
    GbaMisc_EntryCounterBAtHalfOffset(GBA_MISC_ENTRY_HALF_OFFSET(idx))

/* ===== External function declarations ===== */
extern void std011_dialogDisplayYesNoMenu();
extern void fn_80071700();
extern void fn_800719A8();
extern s32 fn_80071AE4();
extern void fn_800722A0();
extern void fn_80072548();
extern s32 fn_800726A8();
extern void fn_80072A00();
extern void fn_80072C74();
extern void fn_80072D58();
extern s32 _AGB_EntryGetStatus__FlPUl(s32, u32*);
extern void fn_800730F8();
extern void fn_800733D0();
extern void fn_80073990();
extern void fn_80073A44();
extern void fn_800830A4();
extern void fn_80083BF8();
extern GbaMiscContext* fn_80083CFC();
extern void fn_80083D30();
extern void fn_80083ECC();
extern void __cvt_fp2unsigned();
extern void memmove();
extern void GSgetFrameStep();
extern void fn_800D37CC();
extern void GSmodelSetGSparticleLinkAttachMode();
extern void GSmodelLinkToGSparticleBank();
extern void GSmodelSetShadowTextureSize();
extern void GSmodelSetShadowLight();
extern void GSmodelSetShadowSurface();
extern void GSmodelSetShadowFlags();
extern void GSmodelGetFrameCount();
extern void GSmodelStartAnimation();
extern void GSmodelSetAnimFrame();
extern void GSmodelSetAnimType();
extern void GSmodelSetAnimIndex();
extern void _threadSwitch();
extern void GSresGetResource();
extern void fn_800F9AEC();
extern void fn_800F9C04();
extern void floorRequestChange();
extern void floorSetFadeScript();
extern void std093_getCurrentGroupID();
extern void fn_80118874();
extern void pokemonBiosSetEventGetFlag();
extern void pokemonBiosSetFightTrainerPokemonDataId();
extern void pokemonBiosSetPara1Amari();
extern void pokemonBiosSetAmari();
extern void pokemonBiosSetMailId();
extern void pokemonBiosSetPcboxMark();
extern void pokemonBiosSetFlagAmari();
extern void pokemonBiosSetFuseiFlag();
extern void pokemonBiosSetTokuseiFlag();
extern void pokemonBiosSetTamagoFlag();
extern void pokemonBiosSetPokerus();
extern void pokemonBiosSetAmariRibbon();
extern void pokemonBiosSetWorldRibbon();
extern void pokemonBiosSetEarthRibbon();
extern void pokemonBiosSetNationalRibbon();
extern void pokemonBiosSetCountryRibbon();
extern void pokemonBiosSetSkyRibbon();
/* ... and 210 more external functions */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_80478960;
extern u8 lbl_8047A670;
extern u8 lbl_8047A674;
extern u8 lbl_8047A678;
extern u8 lbl_8047A67C;
extern u8 lbl_8047A684;
extern u8 lbl_8047A690;
extern u8 lbl_8047A694;
extern u8 lbl_8047C1D0;
extern u8 lbl_8047C1D4;
extern u8 lbl_8047C1D8;
extern u8 lbl_8047C1DC;
extern u8 lbl_8047C1E0;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EEBB8[];
extern u8 jumptable_802EEBE0[];
extern u8 jumptable_802EEC10[];
extern u8 jumptable_802EEC30[];
extern u8 lbl_802EEB98[];
extern u8 lbl_802EEC70[];
extern u8 lbl_803FB308[];
extern u8 lbl_803FB318[];

/* ===== Forward declarations ===== */
void fn_800895A4(void);
u32 fn_800896B8(void);
u32 fn_800896C0(void);
u32 fn_800896C8(void);
void fn_800896D0(u32 v);
void fn_800896D8(u32 v);
void fn_800896E0(u32 v);
void std215_battleStartTrainerWithPrompt(void);
void fn_80089978(void);
u8 GbaMisc_GetMappedContextByte(void);
s32 GbaMisc_HasActiveContextState(void);
s32 std220_gbaUnk_80083bf8(void);
void fn_80089C84(s32 param);
s32 GbaMisc_PollEntryStatusA(s32 r31);
s32 GbaMisc_ResetEntryStatusA(s32 param);
void fn_80089D74(s32 param);
s32 fn_80089D98(s32 r31);
s32 fn_80089E20(s32 idx, void* obj, u32 packedStatus, u32 highHalf);
u32 fn_80089F58(u32 v);
u32 fn_80089F60(u32 v);
u32 fn_80089F68(u32 v);
u32 fn_80089F70(u32 v);
u32 fn_80089F78(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
s32 fn_8008A99C(void);
int gbaCommandEntryPokemon(u32 r3, u8* r4);
s32 GbaMisc_GetEntryStatus(s32 idx, u32* out);
void GbaMisc_SendPackedEntryStatus(s32 param0, u32 param1, u32 param2);
void gbaCommandSendWazaText(s32 param0, s32 param1);
s32 fn_8008AB8C(s32 r3);
u8 fn_8008ABA0(s32 idx);
s32 GbaMisc_SetEntryState(s32 idx, s32 value);
void fn_8008AC34(void);
void fn_8008AE18(void* src, GbaPokemon* dst);
void fn_8008BBDC(void* gc, GbaPokemon* src);
void gbaPokemonConditonFromGC(void);
void fn_8008C6FC(void);
void GbaMisc_RunFlagDispatch(void);
s32 fn_8008C78C(void);
void fn_8008C7B0(void);
void fn_8008CACC(void);
void fn_8008CDD8(void);
void fn_8008D0A0(void);
void fn_8008D348(void);
void fn_8008D938(void);
void fn_8008E320(void);
void fn_8008E7D4(void);
void fn_8008EC28(void);
void fn_8008EED0(void);
void fn_8008F190(void);
void fn_8008F524(void);
void fn_8008F91C(void);
void fn_8008FBF4(void);
void fn_8008FE94(void);
void fn_80090100(void);
void fn_80090720(void);
void fn_800909E4(void);
void fn_80090D34(void);
void fn_8009100C(void);
void fn_80091564(void);
void fn_80091774(void);
void fn_80091984(void);
void fn_80091B94(void);
void fn_80091DA4(void);
void fn_80091F48(void);
void fn_80092140(void);
void fn_80092498(void);
void fn_80092664(void);
void fn_800929BC(void);
void fn_80092B2C(void);

/* ===== Function implementations ===== */


/* 0x8008C6FC | size: 0x4 */
void fn_8008C6FC(void) {
}

/* 0x8008C700 | size: 0x8C */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void GbaMisc_RunFlagDispatch(void) {
    extern s32 std093_getCurrentGroupID(void);
    extern s32 std082_flagGetValue(s32);
    extern void _flagSet(s32, s32);
    s32 arg;
    u32 state;
    u32 offset;
    void (*handler)(s32);
    s32 nextState;

    *(u32*)&lbl_8047A694 = 0;
    *(u32*)((u8*)&lbl_8047A694 + 0x4) = 0;
    *(u32*)&lbl_8047A690 = 0;
    arg = std093_getCurrentGroupID();
    state = std082_flagGetValue(0xb5d);
    offset = state << 2;
    handler = *(void (**)(s32))((u8*)lbl_802EEC70 + offset);
    handler(arg);
    nextState = state + 1;
    if ((u32)nextState >= 0x1f) {
        nextState = 0;
    }
    _flagSet(0xb5d, nextState);
}
#pragma pop

/* 0x8008C78C | size: 0x24 */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; scheduling off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma scheduling off
s32 fn_8008C78C(void) {
    extern s32 std082_flagGetValue(s32);
    return std082_flagGetValue(0xb5d);
}
#pragma pop

