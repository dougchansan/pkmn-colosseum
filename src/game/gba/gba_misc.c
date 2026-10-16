/**
 * @file gba_misc.c
 * @brief GBA miscellaneous communication support (0x800895A4-0x80092C90)
 *
 * Address range: 0x800895A4 - 0x80092C90
 * Total functions: 69
 */

#include "dolphin/types.h"

typedef struct GbaMiscKindPair {
    u8 kind;
    u8 key;
} GbaMiscKindPair;

typedef struct GbaMiscContext {
    u8 unk_0000[0x4000];
    u8 state_4000;       /* 0x4000 */
    u8 unk_4001[0x135];
    u8 tableKey_4136;    /* 0x4136 */
} GbaMiscContext;

#if !defined(GBA_MISC_80089E20_ONLY) && \
    !defined(GBA_MISC_80089B8C_ONLY)

/* GameCube records are big-endian; the GBA expects little-endian. */
static inline u16 GbaSwap16(u16 value) {
    return (value << 8) | (value >> 8);
}

static inline u32 GbaSwap32(u32 value) {
    return (value << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8) |
           (value >> 24);
}

#endif

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
extern u16 lbl_8047A67C[4];
extern u16 lbl_8047A684[4];
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
extern s32 lbl_803FB308[];
extern s32 lbl_803FB318[];

/* ===== Forward declarations ===== */
void fn_800895A4(void);
u32 fn_800896B8(void);
u32 fn_800896C0(void);
u32 fn_800896C8(void);
void fn_800896D0(u32 v);
void fn_800896D8(u32 v);
void fn_800896E0(u32 v);
s32 std215_battleStartTrainerWithPrompt(void* work, void* arg);
u8 fn_80089978(u8* pokemon, u8* entry);
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
u32 fn_80089F78(u32 port, u32 trainer, u32 slot, s32 irekae);
s32 fn_8008A99C(void);
int gbaCommandEntryPokemon(u32 r3, u8* r4);
s32 GbaMisc_GetEntryStatus(s32 idx, u32* out);
void GbaMisc_SendPackedEntryStatus(s32 param0, u32 param1, u32 param2);
void gbaCommandSendWazaText(s32 param0, s32 param1);
s32 fn_8008AB8C(s32 r3);
u8 fn_8008ABA0(s32 idx);
s32 GbaMisc_SetEntryState(s32 idx, s32 value);
void fn_8008AC34(void);
void fn_8008AE18(void* src, void* dst);
void fn_8008BBDC(void* gc, GbaPokemon* src);
u16 gbaPokemonConditonFromGC(void* pokemon);
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

#if !defined(GBA_MISC_80089E20_ONLY) && !defined(GBA_MISC_80089F78_ONLY)

#if !defined(GBA_MISC_80089B8C_ONLY)

/* 0x800895A4 | size: 0x114 */
void fn_800895A4(void) {
    extern void fn_8008BBDC();
    extern void heroBiosSetHomePlace();
    extern void heroBiosSetSexDataId();
    extern void heroBiosSetRnd();
    extern void heroBiosSetNamePtr();
    extern void heroBiosGetPokemonPtr();
    extern void fn_80135938();
    extern void exribbonSetNo();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;


    r30 = r3;
    r31 = r4;
    r0 = *(u8*)((u8*)r31 + 0x0);
    r0 = r0 & 0x00000004;
    if ((s32)r0 != 0x0) {
        r0 = 0x2;
    } else {

        r0 = 0x1;
    }
    r4 = r0 & 0xFF;
    heroBiosSetHomePlace();
    r3 = 0x0;
    r4 = 0x5;
    fn_80135938();
    r6 = r3;
    r3 = (u32)sp + 0x8;
    r4 = r31 + 0x4;
    r5 = 0x7;
    ((void(*)(void))fn_800F9C04)();
    r3 = r30;
    r4 = (u32)sp + 0x8;
    heroBiosSetNamePtr();
    r4 = *(u8*)((u8*)r31 + 0xC);
    r3 = r30;
    heroBiosSetSexDataId();
    r6 = *(u32*)((u8*)r31 + 0x10);
    r3 = r30;
    r0 = r6 & 0x0000FF00;
    r5 = r6 & 0x00FF0000;
    r4 = r6 << 24;
    r6 = (u32)r6 >> 24;
    r0 = r0 << 8;
    r5 = (u32)r5 >> 8;
    r0 = r4 | r0;
    r0 = r5 | r0;
    r4 = r6 | r0;
    heroBiosSetRnd();
    r29 = r31;
    r28 = 0x0;
    do {
        r3 = r30;
        r4 = r28 & 0xFFFF;
        heroBiosGetPokemonPtr();
        r4 = r29 + 0x14;
        r27 = r3;
        fn_8008BBDC();
        r3 = r27;
        r4 = r28 & 0xFFFF;
        ((void(*)(void))pokemonBiosSetFightTrainerPokemonDataId)();
        r29 = r29 + 0x64;
        r28 = r28 + 0x1;
    } while ((s32)r28 < 0x6);
    r29 = 0x0;
    do {
        r0 = r29 + 0x26c;
        r3 = r29;
        r4 = *(u8*)(r31 + r0);
        exribbonSetNo();
        r29 = r29 + 0x1;
    } while ((s32)r29 < 0xb);
    return;
}

/* 0x800896B8 | size: 0x8 */
u32 fn_800896B8(void) {
    return *(u32*)&lbl_80478960;
}

/* 0x800896C0 | size: 0x8 */
u32 fn_800896C0(void) {
    return *(u32*)&lbl_8047A674;
}

/* 0x800896C8 | size: 0x8 */
u32 fn_800896C8(void) {
    return *(u32*)&lbl_8047A670;
}

/* 0x800896D0 | size: 0x8 */
void fn_800896D0(u32 v) {
    *(u32*)&lbl_80478960 = v;
}

/* 0x800896D8 | size: 0x8 */
void fn_800896D8(u32 v) {
    *(u32*)&lbl_8047A674 = v;
}

/* 0x800896E0 | size: 0x8 */
void fn_800896E0(u32 v) {
    *(u32*)&lbl_8047A670 = v;
}

/* 0x800896E8 | size: 0x290 */
s32 std215_battleStartTrainerWithPrompt(void* work, void* arg) {
    extern void msgctrlSetValue(s32 id, void* value);
    extern void std054_characterTalk(void* work, void* arg, s32 size);
    extern void peopleSetVisible(void* work, u8 kind, s32 flag);
    extern void fadeCheck(s32 mode);
    extern s32 fieldStartEncount(s32 id, s32 a, s32 b);
    extern u8 fn_801EEAD0(u8 id);
    extern void fightTrainerPokemonDataBiosSetPokemonDataId(u8* pokemon, s32 id);
    extern u8* fightTrainerPokemonDataBiosGetPtr(void);
    extern void fn_801FCAFC(void* trainer, u8 value);
    extern void fn_801FCB40(void* trainer, u8 slot, u16 value);
    extern void fn_801FCB84(void* trainer, u16 value);
    extern void fn_801FCB94(void* trainer, u8 value);
    extern void fn_801FCC3C(void* trainer);
    extern void fightTrainerDataBiosSetKindDataId(void* trainer, u8 value);
    extern void* fightTrainerDataBiosGetPtr(s32 id);
    extern void* fightEncountDataBiosGetPtr(s32 id);
    extern s8 std011_dialogDisplayYesNoMenu(void);
    u8* ctx;
    /* RULE-EXCEPTION(user-approved): move-table walk through a named pointer, declaration order chosen for register allocation — see docs/RULE_EXCEPTIONS.md */
    u8* p;
    u8* pokemon;
    s32 count;
    s32 i;
    u32 result;
    u8 ok;

    ctx = (u8*)fn_80083CFC(0);
    if (ctx == NULL) {
        return 0;
    }
    if (ctx[0x4000] == 1) {
        msgctrlSetValue(0x4D, ctx + 0x4004);
        std054_characterTalk(work, arg, 0xE0);
    } else {
        msgctrlSetValue(0x4D, ctx + 0x4060);
        std054_characterTalk(work, arg, 0xE0);
        switch (std011_dialogDisplayYesNoMenu()) {
        case 0:
            break;
        case 1:
        case -1:
        default:
            return 0;
        }
    }

    ctx[0x4000] = 2;
    fightEncountDataBiosGetPtr(0x231);
    pokemon = fightTrainerDataBiosGetPtr(9);
    fn_801FCB94(pokemon, ctx[0x4124]);
    fightTrainerDataBiosSetKindDataId(pokemon, ctx[0x4125]);
    fn_801FCB84(pokemon, *(u16*)(ctx + 0x4134));
    fn_801FCAFC(pokemon, ctx[0x4136]);
    p = ctx;
    for (count = 0; count < 4; count++) {
        fn_801FCB40(pokemon, count, *(u16*)(p + 0x4126));
        p += 2;
    }
    fn_801FCC3C(pokemon);

    pokemon = fightTrainerPokemonDataBiosGetPtr();
    for (i = count = 0; i < 4; i++) {
        if (fn_80089978(pokemon, &((u8(*)[0x2A])&((GbaMiscContext*)ctx)->state_4000)[i][0x138]) == 1) {
            pokemon += 0x50;
            count++;
        }
    }
    for (; count < 6; count++) {
        fightTrainerPokemonDataBiosSetPokemonDataId(pokemon, 0);
        pokemon += 0x50;
    }

    *(u8**)&lbl_8047A670 = ctx + 0x4118;
    *(u32*)&lbl_80478960 = 9;
    *(u8**)&lbl_8047A674 = ctx + 0x40BC;
    result = fieldStartEncount(0x231, 1, 0);
    if (result == 2) {
        if (ctx[0x41E1] != 0 && fn_801EEAD0(ctx[0x41E1]) == 1) {
            ok = 1;
        } else {
            ok = 0;
        }
        if (ok == 1) {
            GbaMiscContext* cur;
            u32 key;
            s32 j;
            u8 kind;

            fn_800830A4(ctx);
            cur = fn_80083CFC(0);
            if (cur != NULL) {
                key = cur->tableKey_4136;
            } else {
                key = 0;
            }
            for (j = 0; j < 16; j++) {
                if (key == ((GbaMiscKindPair*)lbl_802EEB98)[j].key) {
                    kind = ((GbaMiscKindPair*)lbl_802EEB98)[j].kind;
                    goto found;
                }
            }
            kind = ((GbaMiscKindPair*)lbl_802EEB98)[0].kind;
        found:
            peopleSetVisible(work, kind, 0);
        }
    }
    fadeCheck(1);
    return result;
}

/* 0x80089978 | size: 0x214 */
u8 fn_80089978(u8* pokemon, u8* entry) {
    extern u8 fn_801EEAD0(u8 id);
    extern void fn_801EEE6C(u8 id, u8 value);
    extern void fightTrainerPokemonDataBiosSetPokemonDataId(u8* pokemon, u16 id);
    extern void fightTrainerPokemonDataBiosSetNickname(u8* pokemon, void* name);
    extern void fightTrainerPokemonDataBiosSetDarkPokemonFlag(u8* pokemon, u8 flag);
    extern void fightTrainerPokemonDataBiosSetLevel(u8* pokemon, u8 level);
    extern void fightTrainerPokemonDataBiosSetWazaDataId(u8* pokemon, u8 slot, u16 id);
    extern void fightTrainerPokemonDataBiosSetItemDataId(u8* pokemon, u16 id);
    extern void fightTrainerPokemonDataBiosSetTokuseiFlag(u8* pokemon, u8 flag);
    extern void fightTrainerPokemonDataBiosSetStatusRnd(u8* pokemon, s32 stat, u8 value);
    extern void fightTrainerPokemonDataBiosSetStatusEffort(u8* pokemon, s32 stat, s16 value);
    extern void fightTrainerPokemonDataBiosSetFriend(u8* pokemon, s16 value);
    extern void fightTrainerPokemonDataBiosSetSexDataId(u8* pokemon, s8 id);
    extern void fightTrainerPokemonDataBiosSetSeikakuDataId(u8* pokemon, u8 id);
    extern void fightTrainerPokemonDataBiosSetKeyPlayerFlag(u8* pokemon, u8 flag);
    extern void fightTrainerPokemonDataBiosSetPartDataId(u8* pokemon, u8 id);
    s32 i;
    u8 skip;

    if (entry[2] != 0 && fn_801EEAD0(entry[2]) == 1) {
        skip = 1;
    } else {
        skip = 0;
    }
    if (skip == 1) {
        fightTrainerPokemonDataBiosSetPokemonDataId(pokemon, 0);
        return 0;
    }

    fightTrainerPokemonDataBiosSetPokemonDataId(pokemon, *(u16*)(entry + 0x00));
    fightTrainerPokemonDataBiosSetNickname(pokemon, NULL);
    fightTrainerPokemonDataBiosSetDarkPokemonFlag(pokemon, entry[2]);
    if (entry[2] != 0) {
        fn_801EEE6C(entry[2], entry[0x28]);
    }
    fightTrainerPokemonDataBiosSetLevel(pokemon, entry[3]);
    for (i = 0; i < 4; i++) {
        fightTrainerPokemonDataBiosSetWazaDataId(pokemon, i, *(u16*)(entry + 4 + i * 2));
    }
    fightTrainerPokemonDataBiosSetItemDataId(pokemon, *(u16*)(entry + 0x0C));
    fightTrainerPokemonDataBiosSetTokuseiFlag(pokemon, entry[0x0E]);
    fightTrainerPokemonDataBiosSetStatusRnd(pokemon, 0, entry[0x0F]);
    fightTrainerPokemonDataBiosSetStatusRnd(pokemon, 1, entry[0x10]);
    fightTrainerPokemonDataBiosSetStatusRnd(pokemon, 2, entry[0x11]);
    fightTrainerPokemonDataBiosSetStatusRnd(pokemon, 3, entry[0x12]);
    fightTrainerPokemonDataBiosSetStatusRnd(pokemon, 4, entry[0x13]);
    fightTrainerPokemonDataBiosSetStatusRnd(pokemon, 5, entry[0x14]);
    fightTrainerPokemonDataBiosSetStatusEffort(pokemon, 0, *(s16*)(entry + 0x16));
    fightTrainerPokemonDataBiosSetStatusEffort(pokemon, 1, *(s16*)(entry + 0x18));
    fightTrainerPokemonDataBiosSetStatusEffort(pokemon, 2, *(s16*)(entry + 0x1A));
    fightTrainerPokemonDataBiosSetStatusEffort(pokemon, 3, *(s16*)(entry + 0x1C));
    fightTrainerPokemonDataBiosSetStatusEffort(pokemon, 4, *(s16*)(entry + 0x1E));
    fightTrainerPokemonDataBiosSetStatusEffort(pokemon, 5, *(s16*)(entry + 0x20));
    fightTrainerPokemonDataBiosSetFriend(pokemon, *(s16*)(entry + 0x22));
    fightTrainerPokemonDataBiosSetSexDataId(pokemon, (s8)entry[0x24]);
    fightTrainerPokemonDataBiosSetSeikakuDataId(pokemon, entry[0x25]);
    fightTrainerPokemonDataBiosSetKeyPlayerFlag(pokemon, entry[0x26]);
    fightTrainerPokemonDataBiosSetPartDataId(pokemon, entry[0x27]);
    return 1;
}

#endif

/* 0x80089B8C | size: 0x84 */
u8 GbaMisc_GetMappedContextByte(void) {
    GbaMiscContext* ptr;
    u32 value;
    u8* table;
    u32 index;
    u32 count;

    ptr = fn_80083CFC(0);
    table = &ptr->tableKey_4136;
    if (ptr != 0) {
        value = *table;
    } else {
        value = 0;
    }
    for (table = lbl_802EEB98, index = 0, count = 0x10; count != 0; count--) {
        if (value == table[1]) {
            return lbl_802EEB98[index << 1];
        }
        table += 2;
        index++;
    }
    return lbl_802EEB98[0];
}

/* 0x80089C10 | size: 0x44 */
s32 GbaMisc_HasActiveContextState(void) {
    GbaMiscContext* ptr;

    ptr = fn_80083CFC(0);
    if (ptr != 0) {
        if (ptr->state_4000 != 0) {
            return 1;
        }
    }
    return 0;
}

/* 0x80089C54 | size: 0x30 */
s32 std220_gbaUnk_80083bf8(void) {
    extern s32 fn_80083BF8(s32);
    return fn_80083BF8(0) > 0;
}

/* 0x80089C84 | size: 0x24 */
void fn_80089C84(s32 param) {
    extern void fn_80071700(s32);
    fn_80071700(param - 1);
}

#if !defined(GBA_MISC_80089B8C_ONLY)

/* 0x80089CA8 | size: 0x88 */
s32 GbaMisc_PollEntryStatusA(s32 r31) {
    extern s32 fn_800719A8(s32);
    s32 n;

    n = fn_800719A8(r31 - 1);
    if (n < 0) {
        u16 *base = (u16*)&lbl_8047A684;
        base[r31 - 1] = 0;
    } else if (n == 1 || n == 2) {
        u16 *base = (u16*)&lbl_8047A684;
        u32 v = base[r31 - 1] + 1;
        base[r31 - 1] = v;
        if ((u16)v <= 0xa) {
            n = -1;
        }
    }
    return n;
}

/* 0x80089D30 | size: 0x44 */
s32 GbaMisc_ResetEntryStatusA(s32 param) {
    extern s32 fn_80071AE4(s32);
    s32 ret;
    u32 tmp;
    u32 r4;

    ret = fn_80071AE4(param - 1);
    tmp = GBA_MISC_ENTRY_HALF_OFFSET(param);
    r4 = (u32)&lbl_8047A684;
    r4 = r4 + tmp;
    tmp = 0;
    *(u16*)((u8*)r4 + (-2)) = tmp;
    return ret;
}

/* 0x80089D74 | size: 0x24 */
void fn_80089D74(s32 param) {
    extern void fn_800722A0(s32);
    fn_800722A0(param - 1);
}

/* 0x80089D98 | size: 0x88 */
s32 fn_80089D98(s32 r31) {
    extern s32 fn_80072548(s32);
    s32 n;
    n = fn_80072548(r31 - 1);
    if (n < 0) {
        u16 *base = (u16*)&lbl_8047A684;
        base[r31 - 1] = 0;
    } else if (n == 1 || n == 2) {
        u16 *base = (u16*)&lbl_8047A684;
        u32 v = base[r31 - 1] + 1;
        base[r31 - 1] = v;
        if ((u16)v <= 0xa) {
            n = -1;
        }
    }
    return n;
}

#endif

#endif

#if !defined(GBA_MISC_80089B8C_ONLY) && !defined(GBA_MISC_80089F78_ONLY)

/* 0x80089E20 | size: 0x138 */
s32 fn_80089E20(s32 r30, void* r31, u32 r5, u32 r29) {
    extern u8 pokemonBiosGetTokuseiFlag(void*);
    extern u16 pokemonBiosGetPokemonDataId(void*);
    extern u8 exribbonGetNo(s32);
    u8 sp[0x78];
    u32 tmp;
    u32 r3;
    u32 r4;
    u32 r6;
    s32 ret;

    tmp = r5 & 0x0000FF00;
    r4 = r5 & 0x00FF0000;
    r3 = r5 << 24;
    r5 = (u32)r5 >> 24;
    tmp = tmp << 8;
    r4 = (u32)r4 >> 8;
    tmp = r3 | tmp;
    tmp = r4 | tmp;
    tmp = r5 | tmp;
    *(u32*)(sp + 0x0) = tmp;
    r3 = pokemonBiosGetPokemonDataId(r31);
    r3 = r3 & 0xFFFF;
    tmp = r29 << 16;
    r6 = tmp | r3;
    tmp = r6 & 0x0000FF00;
    r4 = r6 & 0x00FF0000;
    r5 = r6 << 24;
    tmp = tmp << 8;
    r6 = (u32)r6 >> 24;
    r4 = (u32)r4 >> 8;
    tmp = r5 | tmp;
    tmp = r4 | tmp;
    tmp = r6 | tmp;
    *(u32*)(sp + 0x4) = tmp;
    if (pokemonBiosGetPokemonDataId(r31) == 0x181) {
        tmp = pokemonBiosGetTokuseiFlag(r31);
        tmp = __cntlzw(tmp & 0xFF);
        tmp = (u32)tmp >> 5;
        r4 = tmp & 0xFF;
        pokemonBiosSetTokuseiFlag(r31, r4);
    }
    fn_8008AE18(r31, (GbaPokemon*)(sp + 0x8));
    r31 = sp + 0x6C;
    memset(r31, 0, 0xc);
    for (r29 = 0; (s32)r29 < 0xb; r31 = (u8*)r31 + 1, r29++) {
        *(u8*)r31 = exribbonGetNo(r29);
    }
    ret = fn_800726A8(r30 - 1, sp + 0x0);
    r3 = r30 << 1;
    r4 = (u32)&lbl_8047A684;
    r4 = r4 + r3;
    r3 = 0;
    *(u16*)((u8*)r4 + (-2)) = r3;
    return ret;
}

#endif

#if !defined(GBA_MISC_80089E20_ONLY) && \
    !defined(GBA_MISC_80089B8C_ONLY)

#if !defined(GBA_MISC_80089F78_ONLY)

/* 0x80089F58 | size: 0x8 */
u32 fn_80089F58(u32 v) {
    return v & 0xFFFF;
}

/* 0x80089F60 | size: 0x8 */
u32 fn_80089F60(u32 v) {
    return (v >> 8) & 0xFF;
}

/* 0x80089F68 | size: 0x8 */
u32 fn_80089F68(u32 v) {
    return v & 0xFF;
}

/* 0x80089F70 | size: 0x8 */
u32 fn_80089F70(u32 v) {
    return v >> 16;
}

#endif

/*
 * Battle snapshot sent to the GBA when the player opens the GBA command
 * menu. Multi-byte fields are stored in the GBA's little-endian order.
 */
typedef struct GbaBattleMon {
    /* 0x00 */ u16 hp;
    /* 0x02 */ u8 condition;
    /* 0x03 */ u8 hasItem : 1;
    /* 0x03 */ u8 hasMail : 1;
    /* 0x03 */ u8 pokerusStrain : 1;
    /* 0x03 */ u8 pokerusDays : 1;
    /* 0x03 */ u8 status : 4;
    /* 0x04 */ u16 item;
    /* 0x06 */ u16 pad_06;
    /* 0x08 */ u16 moves[4];
    /* 0x10 */ u8 pp[4];
    /* 0x14 */ u8 moveInfo[4][0x50];
} GbaBattleMon;

typedef struct GbaBattleFoe {
    /* 0x00 */ u8 name[10];
    /* 0x0A */ u8 sex : 2;
    /* 0x0A */ u8 isNidoranM : 1;
    /* 0x0A */ u8 isNidoranF : 1;
    /* 0x0A */ u8 isJapanese : 1;
    /* 0x0A */ u8 controller : 3;
    /* 0x0B */ u8 level;
} GbaBattleFoe;

typedef struct GbaBattleSnapshot {
    /* 0x000 */ u32 partyIds : 24;
    /* 0x003 */ u32 isIrekae : 1;
    /* 0x003 */ u32 activeSlot : 7;
    /* 0x004 */ u32 canSwitch : 24;
    /* 0x007 */ u8 fighterCount;
    /* 0x008 */ u32 rnd0;
    /* 0x00C */ u32 rnd1;
    /* 0x010 */ u16 species;
    /* 0x012 */ u16 form;
    /* 0x014 */ u16 moves[4];
    /* 0x01C */ u8 pp[4];
    /* 0x020 */ u16 moveFlags;
    /* 0x022 */ u8 lockedMove : 4;
    /* 0x022 */ u8 moveSelect : 4;
    /* 0x023 */ u8 targetIndex : 4;
    /* 0x023 */ u8 switchMode : 4;
    /* 0x024 */ GbaBattleMon party[6];
    /* 0x81C */ GbaBattleFoe foes[4];
} GbaBattleSnapshot;

/* Castform (species 0x181) form index from its current type: fire, water, ice. */
#define GBA_FORM_FROM_ZOKUSEI(z) ((z) == 0xA ? 1 : (z) == 0xB ? 2 : (z) == 0xF ? 3 : 0)
#define GBA_SWITCH_MODE_FROM_TOKUSEI(t) ((t) == 0x17 ? 2 : (t) == 0x2A ? 3 : (t) == 0x47 ? 4 : 0)
#define GBA_SWAP16(x) ((u16)(((x) << 8) | ((x) >> 8)))

/*
 * Build the battle snapshot for the GBA command menu, send it on the given
 * link port and wait for the GBA's reply. Returns the byte-swapped reply,
 * the transfer error (or'ed with 0x50000), 0x50000 if the battle was
 * interrupted, or 0x40000 if the command timer ran out.
 */
/* 0x80089F78 | size: 0xA24 */
u32 fn_80089F78(u32 port, u32 trainer, u32 slot, s32 irekae) {
    extern s32 fightFloorGetStatus(s32, s32, s32, s32);
    extern void* fightTypeDataBiosGetPtr(u16 type);
    extern u8 fightTypeDataBiosGetTrainerNum(void* type);
    extern u8 fightTypeDataBiosGetEntryPokemonNum(void* type);
    extern u8 fightTypeDataBiosGetFightoutPokemonNum(void* type);
    extern void* fightTargetGetPtr(u32 targetDataId, void* target, u16 fightType);
    extern void* fightTrainerGetValidFightOutPokemonPtr(void* trainer, u16 index);
    extern void* fightTrainerGetValidFightPokemonPtr(void* trainer, u16 index);
    extern u8 fightTrainer_GetControllerId(void* trainer);
    extern u8 fightTrainerCheckCanIrekaeFightPokemon(void* trainer, void* pokemon);
    extern void* fightPokemonGetPokemonPtr(void* pokemon);
    extern u16 fightPokemonGetSoubiItemDataId(void* pokemon);
    extern u16 pokemonBiosGetFightTrainerPokemonDataId(void* pokemon);
    extern u8 pokemonBiosGetPokerus(void* pokemon);
    extern u16 pokemonBiosGetHp(void* pokemon);
    extern u8 pokemonBiosGetMailId(void* pokemon);
    extern s32 pokemonBiosGetPokemonWazaDataId(void* pokemon, u16 slot);
    extern u8 pokemonBiosGetPokemonWazaPp(void* pokemon, u16 slot);
    extern u16 pokemonBiosGetPokemonDataId(void* pokemon);
    extern void* pokemonBiosGetAttest(void* pokemon);
    extern void* pokemonBiosGetNicknamePtr(void* pokemon);
    extern u8 pokemonBiosGetLevel(void* pokemon);
    extern u8 pokemonGetSex(void* pokemon);
    extern s32 gamedataAttestBiosGetLangareaId(void* attest);
    extern void* fightOutPokemonGetPokemonPtr(void* out);
    extern void fightOutPokemonGetRndStatus(void* out, u32* rnd0, u32* rnd1);
    extern u16 fightOutPokemonBiosGetZokuseiDataId(void* out, s32 index);
    extern u16 fightOutPokemonGetTokuseiDataId(void* out);
    extern u8 fightOutPokemonCheckFightActionWazaSelect(void* out, s32 arg);
    extern u8 fightOutPokemonCheckCanOutOkWazaBanme(void* out, u16 slot, s32 arg, u16* locked);
    extern void* fightFloorBiosGetFightFloorPtr(void);
    extern u8 fightFloorCheckFightActionFightOutPokemonIrekaeSelect(void* floor, void* out,
                                                                    void** target);
    extern u8 fightFloorIsUseFightTimerCommand(s32 arg);
    extern u8 fightTimerCommandIsOver(void);
    extern u16 std103_battleGetResult(void);
    extern void fn_8022B2CC(void* out, s32 waza, u16 fightType, void* callback, s32 a, s32 b,
                            s32 c);
    GbaBattleSnapshot snap;
    void* targets[4];
    u8 controllers[4];
    u32 rnd0;
    u32 rnd1;
    void* target;
    u32 reply;
    u16 locked;
    u16 fightType;
    void* type;
    u8 trainerNum;
    u8 entryNum;
    u8 outNum;
    u8 fighterNum;
    void* enemy;
    void* other;
    void* out;
    s32 i;
    u16 condition;
    s32 j;
    u32 partyIds;
    u32 canSwitch;
    void* pokemon;
    u8 pokerus;
    s32 waza;
    u8 count;
    void* outPokemon;
    u16 moveFlags;
    u16 lockedWaza;
    u8 result;
    u8 mode;
    s32 index;
    void* fightPokemon;
    void* attest;
    u8 lang;
    u16 species;
    u16 zokusei;
    u16 tokusei;
    s32 length;
    u32 size;
    s32 status;
    u32 channel;
    s32 wazaId;
    void* foePokemon;

    fightType = fightFloorGetStatus(0, 0, 0x14, 0);
    type = fightTypeDataBiosGetPtr(fightType);
    trainerNum = fightTypeDataBiosGetTrainerNum(type);
    entryNum = fightTypeDataBiosGetEntryPokemonNum(type);
    outNum = fightTypeDataBiosGetFightoutPokemonNum(type);
    fighterNum = trainerNum * outNum * 2;

    enemy = fightTargetGetPtr(0xB, (void*)trainer, fightType);
    targets[0] = fightTrainerGetValidFightOutPokemonPtr(enemy, 0);
    controllers[0] = 1;
    if (trainerNum == 2) {
        other = fightTargetGetPtr(7, enemy, fightType);
        targets[1] = fightTrainerGetValidFightOutPokemonPtr(other, 0);
        controllers[1] = fightTrainer_GetControllerId(other);
        other = fightTargetGetPtr(9, enemy, fightType);
        targets[2] = fightTrainerGetValidFightOutPokemonPtr(other, 0);
        controllers[2] = fightTrainer_GetControllerId(other);
        other = fightTargetGetPtr(10, enemy, fightType);
        targets[3] = fightTrainerGetValidFightOutPokemonPtr(other, 0);
        controllers[3] = fightTrainer_GetControllerId(other);
    } else if (outNum == 2) {
        targets[1] = fightTrainerGetValidFightOutPokemonPtr(enemy, 1);
        other = fightTargetGetPtr(9, enemy, fightType);
        targets[2] = fightTrainerGetValidFightOutPokemonPtr(other, 0);
        targets[3] = fightTrainerGetValidFightOutPokemonPtr(other, 1);
        controllers[1] = 1;
        controllers[2] = 2;
        controllers[3] = 2;
    } else {
        targets[1] = fightTrainerGetValidFightOutPokemonPtr(fightTargetGetPtr(9, enemy, fightType), 0);
        controllers[1] = 1;
    }

    out = fightTrainerGetValidFightOutPokemonPtr((void*)trainer, slot);
    snap.activeSlot = (u8)(outNum == 2 ? slot + 1 : 0);
    snap.isIrekae = (u8)irekae;
    snap.fighterCount = fighterNum;

    partyIds = 0;
    canSwitch = 0;
    for (i = 0; i < entryNum; i++) {
        fightPokemon = fightTrainerGetValidFightPokemonPtr((void*)trainer, i);
        if (fightPokemon == NULL) {
            break;
        }
        pokemon = fightPokemonGetPokemonPtr(fightPokemon);
        partyIds |= pokemonBiosGetFightTrainerPokemonDataId(pokemon) << (i * 4 + 8);
        canSwitch |= fightTrainerCheckCanIrekaeFightPokemon((void*)trainer, fightPokemon) << (i * 4 + 8);
        pokerus = pokemonBiosGetPokerus(pokemon);
        snap.party[i].hp = GbaSwap16(pokemonBiosGetHp(pokemon));
        condition = ((s32 (*)(void*))gbaPokemonConditonFromGC)(pokemon);
        snap.party[i].condition = condition;
        snap.party[i].hasItem = fightPokemonGetSoubiItemDataId(fightPokemon) != 0;
        snap.party[i].hasMail = pokemonBiosGetMailId(pokemon) != 0xFF;
        snap.party[i].pokerusDays = (pokerus & 0xF) != 0;
        snap.party[i].pokerusStrain = (pokerus & 0xF0) != 0;
        snap.party[i].status = condition >> 8;
        snap.party[i].item = GbaSwap16(fightPokemonGetSoubiItemDataId(fightPokemon));
        for (j = 0; j < 4; j++) {
            waza = pokemonBiosGetPokemonWazaDataId(pokemon, j);
            snap.party[i].moves[j] = GbaSwap16(waza);
            snap.party[i].pp[j] = pokemonBiosGetPokemonWazaPp(pokemon, j);
            fn_80083ECC(snap.party[i].moveInfo[j], waza);
        }
    }
    count = i;

    partyIds |= -(1 << (count * 4 + 8));
    snap.partyIds = GbaSwap32(partyIds);
    snap.canSwitch = GbaSwap32(canSwitch);

    moveFlags = 0;
    outPokemon = fightOutPokemonGetPokemonPtr(out);
    fightOutPokemonGetRndStatus(out, &rnd0, &rnd1);
    snap.rnd0 = GbaSwap32(rnd0);
    snap.rnd1 = GbaSwap32(rnd1);
    species = pokemonBiosGetPokemonDataId(outPokemon);
    snap.species = GbaSwap16(species);
    if (species == 0x181) {
        zokusei = fightOutPokemonBiosGetZokuseiDataId(out, 0);
        snap.form = GBA_SWAP16((u16)GBA_FORM_FROM_ZOKUSEI(zokusei));
    } else {
        snap.form = 0;
    }

    if (irekae != 0) {
        mode = 0;
        index = 0;
    } else {
        mode = fightFloorCheckFightActionFightOutPokemonIrekaeSelect(fightFloorBiosGetFightFloorPtr(),
                                                                     out, &target);
        if (mode == 2) {
            tokusei = fightOutPokemonGetTokuseiDataId(target);
            mode = GBA_SWITCH_MODE_FROM_TOKUSEI(tokusei);
        }
        for (index = fighterNum - 1; index > 0; index--) {
            if (target == targets[index]) {
                break;
            }
        }
    }
    snap.switchMode = mode;
    snap.targetIndex = index;
    snap.moveSelect = fightOutPokemonCheckFightActionWazaSelect(out, 0);

    for (j = 0; j < 4; j++) {
        lbl_8047A678 = 0;
        wazaId = pokemonBiosGetPokemonWazaDataId(outPokemon, j);
        snap.moves[j] = GbaSwap16(wazaId);
        snap.pp[j] = pokemonBiosGetPokemonWazaPp(outPokemon, j);
        fn_8022B2CC(out, wazaId, fightType, fn_8008A99C, 1, 0, -1);
        if (lbl_8047A678 != 0) {
            moveFlags |= 8 << (j * 4);
        }
        result = fightOutPokemonCheckCanOutOkWazaBanme(out, j, 1, &locked);
        moveFlags |= result << (j * 4);
        if (result == 5) {
            lockedWaza = locked;
        }
    }
    snap.moveFlags = GbaSwap16(moveFlags);
    for (index = 3; index > 0; index--) {
        if (snap.moves[index] == GbaSwap16(lockedWaza)) {
            break;
        }
    }
    snap.lockedMove = index;

    for (i = 0; i < fighterNum; i++) {
        memset(snap.foes[i].name, 0, sizeof(GbaBattleFoe));
        if (targets[i] == out) {
            snap.foes[i].name[0] = 0xFF;
        } else {
            foePokemon = fightOutPokemonGetPokemonPtr(targets[i]);
            attest = pokemonBiosGetAttest(foePokemon);
            lang = gamedataAttestBiosGetLangareaId(attest);
            length = ((s32 (*)(u8*, void*, u8))fn_800F9AEC)(snap.foes[i].name,
                                                          pokemonBiosGetNicknamePtr(foePokemon), lang);
            if (length < 10) {
                snap.foes[i].name[length] = 0xFF;
            }
            species = pokemonBiosGetPokemonDataId(foePokemon);
            snap.foes[i].sex = pokemonGetSex(foePokemon);
            snap.foes[i].isNidoranM = species == 0x20;
            snap.foes[i].isNidoranF = species == 0x1D;
            snap.foes[i].controller = controllers[i];
            if ((u8)gamedataAttestBiosGetLangareaId(attest) == 1) {
                snap.foes[i].isJapanese = 1;
            } else {
                snap.foes[i].isJapanese = 0;
            }
            snap.foes[i].level = pokemonBiosGetLevel(foePokemon);
        }
    }

    size = fighterNum * sizeof(GbaBattleFoe);
    memmove(&snap.party[count], snap.foes, size);
    channel = port - 1;
    fn_80072D58(channel, &snap, 0x24 + count * sizeof(GbaBattleMon) + size);
    while (TRUE) {
        if (std103_battleGetResult() == 1) {
            fn_80072A00(channel);
            return 0x50000;
        }
        if (fightFloorIsUseFightTimerCommand(0) == 1 && fightTimerCommandIsOver() == 1) {
            fn_80072A00(channel);
            return 0x40000;
        }
        status = ((s32 (*)(u32, u32*))fn_80072C74)(channel, &reply);
        if (status > 0) {
            return status | 0x50000;
        }
        if (status == 0) {
            return GbaSwap32(reply);
        }
        _threadSwitch();
    }
}

#if !defined(GBA_MISC_80089F78_ONLY)

/* 0x8008A99C | size: 0x10 */
s32 fn_8008A99C(void) {
    lbl_8047A678 = 1;
    return 0;
}

/* 0x8008A9AC | size: 0x38 */
int gbaCommandEntryPokemon(u32 r3, u8* r4) {
    r4[0] = r3 & 0xF;
    r4[1] = (r3 >> 4) & 0xF;
    r4[2] = (r3 >> 8) & 0xF;
    r4[3] = (r3 >> 12) & 0xF;
    r4[4] = (r3 >> 16) & 0xF;
    r4[5] = (r3 >> 20) & 0xF;
    return 0;
}

/* 0x8008A9E4 | size: 0x13C */
s32 GbaMisc_GetEntryStatus(s32 idx, u32* out) {
    u32 status;
    s32 ret;

    *out = 0x2000000;
    ret = _AGB_EntryGetStatus__FlPUl(idx - 1, &status);
    if (ret < 0) {
        status = 0x2000000;
        goto end;
    }
    if (ret != 0) {
        *out = 0x3000000;
        lbl_803FB318[idx - 1] = 1;
        lbl_803FB308[idx - 1] = 0;
        lbl_8047A684[idx - 1] = 0;
        lbl_8047A67C[idx - 1] = 0;
        return ret;
    }
    *out = GbaSwap32(status);
    if ((*out >> 24) == 0) {
        lbl_803FB318[idx - 1] = 1;
        lbl_803FB308[idx - 1] = 0;
        lbl_8047A684[idx - 1] = 0;
        lbl_8047A67C[idx - 1] = 0;
    }
end:
    return 0;
}

/* 0x8008AB20 | size: 0x2C */
void GbaMisc_SendPackedEntryStatus(s32 param0, u32 param1, u32 param2) {
    u32 packed;

    packed = param2 << 24;
    param0--;
    fn_800730F8(param0, packed | param1);
}

/* 0x8008AB4C | size: 0x40 */
void gbaCommandSendWazaText(s32 param0, s32 param1) {
    extern void fn_80083D30(s32, void*);
    extern void fn_800733D0(s32, void*);
    u8 buf[0x780];
    fn_80083D30(param1, buf);
    fn_800733D0(param0 - 1, buf);
}

/* 0x8008AB8C | size: 0x14 */
s32 fn_8008AB8C(s32 r3) {
    u16 *base = (u16*)&lbl_8047A67C;
    return base[r3 - 1];
}

/* 0x8008ABA0 | size: 0x44 */
u8 fn_8008ABA0(s32 idx) {
    u32 ret = 0;
    if (GbaMisc_EntryState(idx) != 0) {
        if (GbaMisc_EntryCachedStatus(idx) == 0) {
            ret = 1;
        }
    }
    return (u8)ret;
}

/* 0x8008ABE4 | size: 0x50 */
s32 GbaMisc_SetEntryState(s32 idx, s32 value) {
    u32 r0;
    u32 r5;
    u32 r6;
    u32 r7;
    u32 r8;
    u32 r9;
    u32 r10;
    s32 old;

    r6 = (u32)&lbl_803FB318;
    r7 = idx << 2;
    r0 = r6;
    r5 = (u32)&lbl_803FB308;
    r9 = r0 + r7;
    r10 = idx << 1;
    idx = r10;
    r9 = r9 - 4;
    r6 = r5;
    old = *(s32*)r9;
    r5 = (u32)&lbl_8047A684;
    r7 = r6 + r7;
    r8 = 0;
    r6 = r5 + idx;
    r0 = (u32)&lbl_8047A67C;
    r5 = r0 + idx;
    *(s32*)r9 = value;
    *(u32*)((u8*)r7 + (-4)) = r8;
    *(u16*)((u8*)r6 + (-2)) = r8;
    *(u16*)((u8*)r5 + (-2)) = r8;
    return old;
}

/* 0x8008AC34 | size: 0x1E4 */
void fn_8008AC34(void) {
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r29 = r3;
    r3 = (u32)&lbl_803FB318;
    r30 = r29 << 2;
    tmp = (u32)&lbl_803FB318;
    r31 = tmp + r30;
    tmp = *(u32*)((u8*)r31 + 0x0);
    if ((s32)tmp != 2) {
        if ((s32)tmp < 2) {
            if ((s32)tmp < 1 || (s32)tmp >= 4) {
                tmp = r29 << 1;
                r3 = (u32)&lbl_8047A67C;
                r3 = r3 + tmp;
                tmp = 0x0;
                *(u16*)((u8*)r3 + (-2)) = tmp;
                r3 = 0x1;
            } else {

            r4 = (u32)sp + 0x8;
            ((void(*)(void))fn_80073A44)();
            if ((s32)r3 == 0) {
                r7 = r29 << 1;
                r6 = (u32)&lbl_8047A684;
                tmp = (u32)&lbl_8047A67C;
                r5 = *(u16*)(sp + 0x8);
                r4 = tmp + r7;
                r6 = r6 + r7;
                tmp = 0x0;
                *(u16*)((u8*)r4 + (-2)) = r5;
                *(u16*)((u8*)r6 + (-2)) = tmp;
            } else {
            do {
                if ((s32)r3 > 2) break;
                tmp = r29 << 1;
                r4 = (u32)&lbl_8047A684;
                r5 = r4 + tmp;
                r4 = *(u16*)((u8*)r5 + (-2));
                r4 = r4 + 0x1;
                tmp = r4 & 0xFFFF;
                *(u16*)((u8*)r5 + (-2)) = r4;
                if (tmp > 0xa) break;
                r3 = 0x0;
            } while (0);

            r7 = r29 << 1;
            tmp = (u32)&lbl_8047A67C;
            r8 = tmp + r7;
            r6 = 0x0;
            r4 = (u32)&lbl_803FB308;
            *(u16*)((u8*)r8 + 0x0) = r6;
            r4 = (u32)&lbl_803FB308;
            tmp = (u32)&lbl_8047A684;
            r5 = r4 + r30;
            *(u32*)((u8*)r31 + 0x0) = r6;
            r4 = tmp + r7;
            *(u32*)((u8*)r5 + (-4)) = r6;
            *(u16*)((u8*)r4 + (-2)) = r6;
            *(u16*)((u8*)r8 + 0x0) = r6;
            }
            }
        } else {
        ((void(*)(void))fn_80073990)();
        if ((s32)r3 == 0) {
            tmp = r29 << 1;
            r4 = (u32)&lbl_8047A684;
            r4 = r4 + tmp;
            tmp = 0x0;
            *(u16*)((u8*)r4 + (-2)) = tmp;

        } else {
            do {
            if ((s32)r3 > 2) break;
                tmp = r29 << 1;
                r4 = (u32)&lbl_8047A684;
                r5 = r4 + tmp;
                r4 = *(u16*)((u8*)r5 + (-2));
                r4 = r4 + 0x1;
                tmp = r4 & 0xFFFF;
                *(u16*)((u8*)r5 + (-2)) = r4;
                if (tmp > 0xa) break;
                r3 = 0x0;
                break;
            } while (0);

            tmp = 0x0;
            *(u32*)((u8*)r31 + 0x0) = tmp;
        }
        tmp = r29 << 1;
        r4 = (u32)&lbl_8047A67C;
        r4 = r4 + tmp;
        tmp = 0x0;
        *(u16*)((u8*)r4 + (-2)) = tmp;
        }
    } else {
    tmp = r29 << 1;
    r3 = (u32)&lbl_8047A67C;
    r3 = r3 + tmp;
    tmp = 0x0;
    *(u16*)((u8*)r3 + (-2)) = tmp;
    r3 = 0x0;
    }
    r4 = (u32)&lbl_803FB308;
    tmp = (u32)&lbl_803FB308;
    r4 = tmp + r30;
    *(u32*)((u8*)r4 + (-4)) = r3;
    return;
}

/* 0x8008AE18 | size: 0xDC4 */
void fn_8008AE18(void* src, void* dst_) {
    extern u8 pokemonBiosGetEventGetFlag(void* pokemon);
    extern u16 pokemonBiosGetPara1Amari(void* pokemon);
    extern u16 pokemonBiosGetAmari(void* pokemon);
    extern u8 pokemonBiosGetMailId(void* pokemon);
    extern u8 pokemonBiosGetPcboxMark(void* pokemon);
    extern u8 pokemonBiosGetFlagAmari(void* pokemon);
    extern u8 pokemonBiosGetFuseiFlag(void* pokemon);
    extern u8 pokemonBiosGetTokuseiFlag(void* pokemon);
    extern u8 pokemonBiosGetTamagoFlag(void* pokemon);
    extern u8 pokemonBiosGetPokerus(void* pokemon);
    extern u8 pokemonBiosGetAmariRibbon(void* pokemon);
    extern u8 pokemonBiosGetWorldRibbon(void* pokemon);
    extern u8 pokemonBiosGetEarthRibbon(void* pokemon);
    extern u8 pokemonBiosGetNationalRibbon(void* pokemon);
    extern u8 pokemonBiosGetCountryRibbon(void* pokemon);
    extern u8 pokemonBiosGetSkyRibbon(void* pokemon);
    extern u8 pokemonBiosGetLandRibbon(void* pokemon);
    extern u8 pokemonBiosGetMarineRibbon(void* pokemon);
    extern u8 pokemonBiosGetGanbaRibbon(void* pokemon);
    extern u8 pokemonBiosGetBromideRibbon(void* pokemon);
    extern u8 pokemonBiosGetVictoryRibbon(void* pokemon);
    extern u8 pokemonBiosGetWinningRibbon(void* pokemon);
    extern u8 pokemonBiosGetChampRibbon(void* pokemon);
    extern u8 pokemonBiosGetFur(void* pokemon);
    extern u8 pokemonBiosGetStrongMedal(void* pokemon);
    extern u8 pokemonBiosGetCleverMedal(void* pokemon);
    extern s32 pokemonBiosGetCuteMedal(void* pokemon);
    extern u8 pokemonBiosGetBeautifulMedal(void* pokemon);
    extern u8 pokemonBiosGetStyleMedal(void* pokemon);
    extern u8 pokemonBiosGetStrong(void* pokemon);
    extern u8 pokemonBiosGetClever(void* pokemon);
    extern u8 pokemonBiosGetCute(void* pokemon);
    extern u8 pokemonBiosGetBeautiful(void* pokemon);
    extern u8 pokemonBiosGetStyle(void* pokemon);
    extern s32 pokemonBiosGetFriend(void* pokemon);
    extern s32 pokemonBiosGetNimblenessRnd(void* pokemon);
    extern s32 pokemonBiosGetSpeDefRnd(void* pokemon);
    extern u16 pokemonBiosGetSpeAtkRnd(void* pokemon);
    extern s32 pokemonBiosGetPhyDefRnd(void* pokemon);
    extern u16 pokemonBiosGetPhyAtkRnd(void* pokemon);
    extern s32 pokemonBiosGetMaxHpRnd(void* pokemon);
    extern s32 pokemonBiosGetNimblenessEffort(void* pokemon);
    extern s32 pokemonBiosGetSpeDefEffort(void* pokemon);
    extern s32 pokemonBiosGetSpeAtkEffort(void* pokemon);
    extern s32 pokemonBiosGetPhyDefEffort(void* pokemon);
    extern s32 pokemonBiosGetPhyAtkEffort(void* pokemon);
    extern s32 pokemonBiosGetMaxHpEffort(void* pokemon);
    extern u16 pokemonBiosGetNimbleness(void* pokemon);
    extern u16 pokemonBiosGetSpeDef(void* pokemon);
    extern u16 pokemonBiosGetSpeAtk(void* pokemon);
    extern u16 pokemonBiosGetPhyDef(void* pokemon);
    extern u16 pokemonBiosGetPhyAtk(void* pokemon);
    extern u16 pokemonBiosGetMaxHp(void* pokemon);
    extern u16 pokemonBiosGetHp(void* pokemon);
    extern u16 pokemonBiosGetItemDataId(void* pokemon);
    extern s32 pokemonBiosGetPokemonWazaPpCount(void* pokemon, u16 slot);
    extern u8 pokemonBiosGetPokemonWazaPp(void* pokemon, u16 slot);
    extern u16 pokemonBiosGetPokemonWazaDataId(void* pokemon, u16 slot);
    extern u32 pokemonBiosGetConditionAmari(void* pokemon);
    extern u8 pokemonBiosGetLevel(void* pokemon);
    extern u32 pokemonBiosGetExp(void* pokemon);
    extern void* pokemonBiosGetNicknameOrgPtr(void* pokemon);
    extern void* pokemonBiosGetCatchTrainerNamePtr(void* pokemon);
    extern u32 pokemonBiosGetCatchTrainerRnd(void* pokemon);
    extern u8 pokemonBiosGetCatchTrainerSex(void* pokemon);
    extern u8 pokemonBiosGetCatchBallId(void* pokemon);
    extern u8 pokemonBiosGetCatchLevel(void* pokemon);
    extern s32 pokemonBiosGetCatchFloorId(void* pokemon);
    extern void* pokemonBiosGetAttest(void* pokemon);
    extern u32 pokemonBiosGetRnd(void* pokemon);
    extern u16 pokemonBiosGetPokemonDataId(void* pokemon);
    extern s16 fn_80121984(void* pokemon, s32 kind);
    extern s8 fn_8012189C(void* pokemon, s32 kind);
    extern u8 fn_80121ADC(void* pokemon, s32 kind);
    extern u8 pokemonCheckValid(void* pokemon);
    extern u8 gamedataAttestBiosGetLangareaId(void* attest);
    extern u8 gamedataAttestBiosGetVerId(void* attest);
    extern s32 fn_800F9AEC(u8* out, void* name, s32 langId);
    extern void* memmove(void* dst, const void* src, u32 size);
    GbaPokemon* dst;
    u8 tmp[12];
    GbaPokemonMisc misc;
    u32 personality;
    u32 order;
    u32 span;
    u16 status;
    s32 length;
    int i;
    void* attest;
    u8* a;
    u8* b;
    s32 sum;

    dst = dst_;
    sum = 0;
    memset(dst_, 0, sizeof(GbaPokemon));
    if (pokemonCheckValid(src) == 0) {
        return;
    }

    dst->personality = GbaSwap32(pokemonBiosGetRnd(src));
    dst->otId = GbaSwap32(pokemonBiosGetCatchTrainerRnd(src));

    attest = pokemonBiosGetAttest(src);
    {
        switch (gamedataAttestBiosGetVerId(attest)) {
        case 8:  misc.origins.gameId = 1; break;
        case 9:  misc.origins.gameId = 2; break;
        case 10: misc.origins.gameId = 3; break;
        case 1:  misc.origins.gameId = 4; break;
        case 2:  misc.origins.gameId = 5; break;
        case 11: misc.origins.gameId = 15; break;
        default: misc.origins.gameId = 0; break;
        }

        switch (gamedataAttestBiosGetLangareaId(attest)) {
        case 1: dst->language = 1; break;
        case 2: dst->language = 2; break;
        case 3: dst->language = 5; break;
        case 4: dst->language = 3; break;
        case 5: dst->language = 4; break;
        case 6: dst->language = 7; break;
        case 8: dst->language = 2; break;
        case 9: dst->language = 2; break;
        default: dst->language = 0; break;
        }

        dst->hasSpecies = 1;
        dst->isEgg = pokemonBiosGetTamagoFlag(src) != 0;
        dst->amariFlags = pokemonBiosGetFlagAmari(src);
        dst->isBadEgg = pokemonBiosGetFuseiFlag(src) != 0;

        length = fn_800F9AEC(dst->nickname, pokemonBiosGetNicknameOrgPtr(src), gamedataAttestBiosGetLangareaId(attest));
        if (length < 10) {
            dst->nickname[length] = 0xFF;
            memset(dst->nickname + 1 + length, 0, 9 - length);
        }

        length = fn_800F9AEC(dst->otName, pokemonBiosGetCatchTrainerNamePtr(src), gamedataAttestBiosGetLangareaId(attest));
        if (length < 7) {
            dst->otName[length] = 0xFF;
            memset(dst->otName + 1 + length, 0, 6 - length);
        }
    }

    dst->markings = pokemonBiosGetPcboxMark(src);
    dst->amari = GbaSwap16(pokemonBiosGetAmari(src));
    dst->species = GbaSwap16(pokemonBiosGetPokemonDataId(src));
    dst->heldItem = GbaSwap16(pokemonBiosGetItemDataId(src));
    dst->experience = GbaSwap32(pokemonBiosGetExp(src));
    dst->friendship = (u8)pokemonBiosGetFriend(src);
    dst->growthAmari = pokemonBiosGetPara1Amari(src);

    dst->ppBonuses = 0;
    for (i = 0; i < 4; i++) {
        dst->moves[i] = GbaSwap16(pokemonBiosGetPokemonWazaDataId(src, i));
        dst->ppBonuses = (u8)(dst->ppBonuses | ((u8)pokemonBiosGetPokemonWazaPpCount(src, i) << (i * 2)));
        dst->pp[i] = pokemonBiosGetPokemonWazaPp(src, i);
    }

    dst->hpEffort = (u8)pokemonBiosGetMaxHpEffort(src);
    dst->phyAtkEffort = (u8)pokemonBiosGetPhyAtkEffort(src);
    dst->phyDefEffort = (u8)pokemonBiosGetPhyDefEffort(src);
    dst->nimblenessEffort = (u8)pokemonBiosGetNimblenessEffort(src);
    dst->speAtkEffort = (u8)pokemonBiosGetSpeAtkEffort(src);
    dst->speDefEffort = (u8)pokemonBiosGetSpeDefEffort(src);
    dst->style = pokemonBiosGetStyle(src);
    dst->beautiful = pokemonBiosGetBeautiful(src);
    dst->cute = pokemonBiosGetCute(src);
    dst->clever = pokemonBiosGetClever(src);
    dst->strong = pokemonBiosGetStrong(src);
    dst->fur = pokemonBiosGetFur(src);

    misc.origins.pokerus = pokemonBiosGetPokerus(src);
    misc.origins.metLocation = (u8)pokemonBiosGetCatchFloorId(src);
    misc.origins.metLevel = pokemonBiosGetCatchLevel(src);
    misc.origins.ballId = pokemonBiosGetCatchBallId(src);
    misc.origins.otGender = pokemonBiosGetCatchTrainerSex(src);

    misc.ivs.hpIv = (u8)pokemonBiosGetMaxHpRnd(src);
    misc.ivs.attackIv = pokemonBiosGetPhyAtkRnd(src);
    misc.ivs.defenseIv = (u8)pokemonBiosGetPhyDefRnd(src);
    misc.ivs.speedIv = (u16)pokemonBiosGetNimblenessRnd(src);
    misc.ivs.spAttackIv = pokemonBiosGetSpeAtkRnd(src);
    misc.ivs.spDefenseIv = (u8)pokemonBiosGetSpeDefRnd(src);
    misc.ivs.isEgg = pokemonBiosGetTamagoFlag(src) != 0;
    misc.ivs.altAbility = pokemonBiosGetTokuseiFlag(src) != 0;

    misc.ribbons.styleMedal = pokemonBiosGetStyleMedal(src);
    misc.ribbons.beautifulMedal = pokemonBiosGetBeautifulMedal(src);
    misc.ribbons.cuteMedal = (u8)pokemonBiosGetCuteMedal(src);
    misc.ribbons.cleverMedal = pokemonBiosGetCleverMedal(src);
    misc.ribbons.strongMedal = pokemonBiosGetStrongMedal(src);
    misc.ribbons.champRibbon = pokemonBiosGetChampRibbon(src) != 0;
    misc.ribbons.winningRibbon = pokemonBiosGetWinningRibbon(src) != 0;
    misc.ribbons.victoryRibbon = pokemonBiosGetVictoryRibbon(src) != 0;
    misc.ribbons.bromideRibbon = pokemonBiosGetBromideRibbon(src) != 0;
    misc.ribbons.ganbaRibbon = pokemonBiosGetGanbaRibbon(src) != 0;
    misc.ribbons.marineRibbon = pokemonBiosGetMarineRibbon(src) != 0;
    misc.ribbons.landRibbon = pokemonBiosGetLandRibbon(src) != 0;
    misc.ribbons.skyRibbon = pokemonBiosGetSkyRibbon(src) != 0;
    misc.ribbons.countryRibbon = pokemonBiosGetCountryRibbon(src) != 0;
    misc.ribbons.nationalRibbon = pokemonBiosGetNationalRibbon(src) != 0;
    misc.ribbons.earthRibbon = pokemonBiosGetEarthRibbon(src) != 0;
    misc.ribbons.worldRibbon = pokemonBiosGetWorldRibbon(src) != 0;
    misc.ribbons.amariRibbon = pokemonBiosGetAmariRibbon(src);
    misc.ribbons.eventGet = pokemonBiosGetEventGetFlag(src);

    status = 0;
    if (fn_80121ADC(src, 4) != 0) {
        status = (fn_80121984(src, 4) << 8) | 0x80;
    } else if (fn_80121ADC(src, 5) != 0) {
        status |= 0x40;
    } else if (fn_80121ADC(src, 7) != 0) {
        status |= 0x20;
    } else if (fn_80121ADC(src, 6) != 0) {
        status |= 0x10;
    } else if (fn_80121ADC(src, 3) != 0) {
        status |= 0x8;
    } else if (fn_80121ADC(src, 8) != 0) {
        status = (s16)fn_8012189C(src, 8);
    }
    {
        /* RULE-EXCEPTION(user-approved): widening copy so the status register is truncated in place — see docs/RULE_EXCEPTIONS.md */
        u32 value = status;
        dst->status = GbaSwap32(value) | (pokemonBiosGetConditionAmari(src) & 0xFFFFF000);
    }

    dst->level = pokemonBiosGetLevel(src);
    dst->mailId = pokemonBiosGetMailId(src);
    dst->maxHp = GbaSwap16(pokemonBiosGetMaxHp(src));
    dst->hp = GbaSwap16(pokemonBiosGetHp(src));
    dst->phyAtk = GbaSwap16(pokemonBiosGetPhyAtk(src));
    dst->phyDef = GbaSwap16(pokemonBiosGetPhyDef(src));
    dst->nimbleness = GbaSwap16(pokemonBiosGetNimbleness(src));
    dst->speAtk = GbaSwap16(pokemonBiosGetSpeAtk(src));
    dst->speDef = GbaSwap16(pokemonBiosGetSpeDef(src));

    dst->miscWord0 = GbaSwap32(*(u32*)&misc.origins);
    dst->miscWord1 = GbaSwap32(*(u32*)&misc.ivs);
    dst->miscWord2 = GbaSwap32(*(u32*)&misc.ribbons);

    for (i = 0; i < 24; i++) {
        sum = sum + GbaSwap16(((u16*)&dst->species)[i]);
    }
    dst->checksum = GbaSwap16(sum);

    for (i = 0; i < 12; i++) {
        ((u32*)&dst->species)[i] ^= (dst->personality ^ dst->otId);
    }

    personality = GbaSwap32(dst->personality);
    order = personality % 24;
    if (order / 6 != 0) {
        span = (order / 6) * 12;
        memcpy(tmp, (u8*)&dst->species + span, 12);
        memmove((u8*)dst->moves, &dst->species, span);
        memcpy(&dst->species, tmp, 12);
    }
    order = order % 6;
    if (order / 2 != 0) {
        span = (order / 2) * 12;
        memcpy(tmp, (u8*)dst->moves + span, 12);
        a = (u8*)dst->moves;
        memmove(&dst->hpEffort, a, span);
        memcpy(a, tmp, 12);
    }
    if ((order & 1) != 0) {
        a = (u8*)&dst->miscWord0;
        memcpy(tmp, a, 12);
        b = (u8*)&dst->hpEffort;
        memcpy(a, b, 12);
        memcpy(b, tmp, 12);
    }
}

/* 0x8008BBDC | size: 0x9F4 */
void fn_8008BBDC(void* gc, GbaPokemon* src) {
    extern void pokemonInit(void* pokemon);
    extern void* pokemonBiosGetAttest(void* pokemon);
    extern void* pokemonBiosGetCatchTrainerNamePtr(void* pokemon);
    extern void gamedataAttestCreate(void* attest, s32 verId, s32 kind, s32 region, s32 langId);
    extern void fn_800F9C04(void* out, const u8* name, s32 count, u8 langId);
    extern void pokemonBiosSetRnd(void* pokemon, u32 value);
    extern void pokemonBiosSetCatchTrainerRnd(void* pokemon, u32 value);
    extern void pokemonBiosSetNicknamePtr(void* pokemon, void* name);
    extern void pokemonBiosSetFlagAmari(void* pokemon, s32 value);
    extern void pokemonBiosSetFuseiFlag(void* pokemon, s32 value);
    extern void pokemonBiosSetPcboxMark(void* pokemon, s32 value);
    extern void pokemonBiosSetAmari(void* pokemon, u16 value);
    extern void pokemonBiosSetPokemonDataId(void* pokemon, u16 value);
    extern void pokemonBiosSetItemDataId(void* pokemon, u16 value);
    extern void pokemonBiosSetExp(void* pokemon, u32 value);
    extern void pokemonBiosSetFriend(void* pokemon, s32 value);
    extern void pokemonBiosSetPara1Amari(void* pokemon, s32 value);
    extern void pokemonBiosSetPokemonWazaDataId(void* pokemon, u16 slot, u16 value);
    extern void pokemonBiosSetPokemonWazaPpCount(void* pokemon, u16 slot, s32 value);
    extern void pokemonBiosSetPokemonWazaPp(void* pokemon, u16 slot, s32 value);
    extern void pokemonBiosSetMaxHpEffort(void* pokemon, s32 value);
    extern void pokemonBiosSetPhyAtkEffort(void* pokemon, s32 value);
    extern void pokemonBiosSetPhyDefEffort(void* pokemon, s32 value);
    extern void pokemonBiosSetNimblenessEffort(void* pokemon, s32 value);
    extern void pokemonBiosSetSpeAtkEffort(void* pokemon, s32 value);
    extern void pokemonBiosSetSpeDefEffort(void* pokemon, s32 value);
    extern void pokemonBiosSetStyle(void* pokemon, s32 value);
    extern void pokemonBiosSetBeautiful(void* pokemon, s32 value);
    extern void pokemonBiosSetCute(void* pokemon, s32 value);
    extern void pokemonBiosSetClever(void* pokemon, s32 value);
    extern void pokemonBiosSetStrong(void* pokemon, s32 value);
    extern void pokemonBiosSetFur(void* pokemon, s32 value);
    extern void pokemonBiosSetPokerus(void* pokemon, s32 value);
    extern void pokemonBiosSetCatchFloorId(void* pokemon, s32 value);
    extern void pokemonBiosSetCatchLevel(void* pokemon, s32 value);
    extern void pokemonBiosSetCatchBallId(void* pokemon, s32 value);
    extern void pokemonBiosSetCatchTrainerSex(void* pokemon, s32 value);
    extern void pokemonBiosSetMaxHpRnd(void* pokemon, s32 value);
    extern void pokemonBiosSetPhyAtkRnd(void* pokemon, s32 value);
    extern void pokemonBiosSetPhyDefRnd(void* pokemon, s32 value);
    extern void pokemonBiosSetNimblenessRnd(void* pokemon, s32 value);
    extern void pokemonBiosSetSpeAtkRnd(void* pokemon, s32 value);
    extern void pokemonBiosSetSpeDefRnd(void* pokemon, s32 value);
    extern void pokemonBiosSetTamagoFlag(void* pokemon, s32 value);
    extern void pokemonBiosSetTokuseiFlag(void* pokemon, s32 value);
    extern void pokemonBiosSetStyleMedal(void* pokemon, s32 value);
    extern void pokemonBiosSetBeautifulMedal(void* pokemon, s32 value);
    extern void pokemonBiosSetCuteMedal(void* pokemon, s32 value);
    extern void pokemonBiosSetCleverMedal(void* pokemon, s32 value);
    extern void pokemonBiosSetStrongMedal(void* pokemon, s32 value);
    extern void pokemonBiosSetChampRibbon(void* pokemon, s32 value);
    extern void pokemonBiosSetWinningRibbon(void* pokemon, s32 value);
    extern void pokemonBiosSetVictoryRibbon(void* pokemon, s32 value);
    extern void pokemonBiosSetBromideRibbon(void* pokemon, s32 value);
    extern void pokemonBiosSetGanbaRibbon(void* pokemon, s32 value);
    extern void pokemonBiosSetMarineRibbon(void* pokemon, s32 value);
    extern void pokemonBiosSetLandRibbon(void* pokemon, s32 value);
    extern void pokemonBiosSetSkyRibbon(void* pokemon, s32 value);
    extern void pokemonBiosSetCountryRibbon(void* pokemon, s32 value);
    extern void pokemonBiosSetNationalRibbon(void* pokemon, s32 value);
    extern void pokemonBiosSetEarthRibbon(void* pokemon, s32 value);
    extern void pokemonBiosSetWorldRibbon(void* pokemon, s32 value);
    extern void pokemonBiosSetAmariRibbon(void* pokemon, s32 value);
    extern void pokemonBiosSetEventGetFlag(void* pokemon, s32 value);
    extern void pokemonBiosSetConditionAmari(void* pokemon, u32 value);
    extern void pokemonBiosSetLevel(void* pokemon, s32 value);
    extern void pokemonBiosSetMailId(void* pokemon, s32 value);
    extern void pokemonBiosSetMaxHp(void* pokemon, u16 value);
    extern void pokemonBiosSetHp(void* pokemon, u16 value);
    extern void pokemonBiosSetPhyAtk(void* pokemon, u16 value);
    extern void pokemonBiosSetPhyDef(void* pokemon, u16 value);
    extern void pokemonBiosSetNimbleness(void* pokemon, u16 value);
    extern void pokemonBiosSetSpeAtk(void* pokemon, u16 value);
    extern void pokemonBiosSetSpeDef(void* pokemon, u16 value);
    extern void fn_8012173C(void* pokemon, s32 kind, s8 value);
    extern void fn_8012190C(void* pokemon, s32 kind, s16 value);
    extern void fn_801219F4(void* pokemon, s32 kind, s32 value);
    extern void* memmove(void* dst, const void* src, u32 size);
    u16 nickname[11]; /* 10 GBA characters and a terminator */
    u8 tmp[12];
    GbaPokemonMisc misc;
    u32 personality;
    u32 order;
    u32 span;
    u16 status;
    s32 verId;
    s32 region;
    s32 langId;
    u8 lang;
    u8* a;
    u8* b;
    int i;
    /* RULE-EXCEPTION(user-approved): second block pointer kept separate from `a` for register allocation — see docs/RULE_EXCEPTIONS.md */
    u8* moves;

    pokemonInit(gc);
    if (src->hasSpecies == 0 && src->isBadEgg == 0) {
        return;
    }

    for (i = 0; i < 12; i++) {
        ((u32*)&src->species)[i] ^= (src->personality ^ src->otId);
    }

    personality = GbaSwap32(src->personality);
    if ((personality & 1) != 0) {
        a = (u8*)&src->hpEffort;
        memcpy(tmp, a, 12);
        b = (u8*)&src->miscWord0;
        memcpy(a, b, 12);
        memcpy(b, tmp, 12);
    }
    order = (personality >> 1) % 3;
    if (order != 0) {
        moves = (u8*)src->moves;
        memcpy(tmp, moves, 12);
        span = order * 12;
        memmove(moves, &src->hpEffort, span);
        memcpy((u8*)src->moves + span, tmp, 12);
    }
    order = (personality / 6) & 3;
    if (order != 0) {
        memcpy(tmp, &src->species, 12);
        span = order * 12;
        memmove(&src->species, src->moves, span);
        memcpy((u8*)&src->species + span, tmp, 12);
    }

    *(u32*)&misc.origins = GbaSwap32(src->miscWord0);
    *(u32*)&misc.ivs = GbaSwap32(src->miscWord1);
    *(u32*)&misc.ribbons = GbaSwap32(src->miscWord2);
    pokemonBiosSetRnd(gc, GbaSwap32(src->personality));
    pokemonBiosSetCatchTrainerRnd(gc, GbaSwap32(src->otId));

    switch (misc.origins.gameId) {
    case 1:  verId = 8; break;
    case 2:  verId = 9; break;
    case 3:  verId = 10; break;
    case 4:  verId = 1; break;
    case 5:  verId = 2; break;
    case 15: verId = 11; break;
    default: verId = 0; break;
    }

    switch (src->language) {
    case 1: region = 1; langId = 1; break;
    case 2: region = 2; langId = 2; break;
    case 3: region = 3; langId = 4; break;
    case 4: region = 3; langId = 5; break;
    case 5: region = 3; langId = 3; break;
    case 7: region = 3; langId = 6; break;
    default: region = 0; langId = 0; break;
    }

    gamedataAttestCreate(pokemonBiosGetAttest(gc), verId, 3, region, langId);

    lang = langId;
    fn_800F9C04(nickname, src->nickname, 10, lang);
    pokemonBiosSetNicknamePtr(gc, nickname);
    pokemonBiosSetFlagAmari(gc, src->amariFlags);
    pokemonBiosSetFuseiFlag(gc, src->isBadEgg);
    fn_800F9C04(pokemonBiosGetCatchTrainerNamePtr(gc), src->otName, 7, lang);

    pokemonBiosSetPcboxMark(gc, src->markings);
    pokemonBiosSetAmari(gc, GbaSwap16(src->amari));
    pokemonBiosSetPokemonDataId(gc, GbaSwap16(src->species));
    pokemonBiosSetItemDataId(gc, GbaSwap16(src->heldItem));
    pokemonBiosSetExp(gc, GbaSwap32(src->experience));
    pokemonBiosSetFriend(gc, src->friendship);
    pokemonBiosSetPara1Amari(gc, src->growthAmari);

    for (i = 0; i < 4; i++) {
        pokemonBiosSetPokemonWazaDataId(gc, i, GbaSwap16(src->moves[i]));
        pokemonBiosSetPokemonWazaPpCount(gc, i, (src->ppBonuses >> (i * 2)) & 3);
        pokemonBiosSetPokemonWazaPp(gc, i, src->pp[i]);
    }

    pokemonBiosSetMaxHpEffort(gc, src->hpEffort);
    pokemonBiosSetPhyAtkEffort(gc, src->phyAtkEffort);
    pokemonBiosSetPhyDefEffort(gc, src->phyDefEffort);
    pokemonBiosSetNimblenessEffort(gc, src->nimblenessEffort);
    pokemonBiosSetSpeAtkEffort(gc, src->speAtkEffort);
    pokemonBiosSetSpeDefEffort(gc, src->speDefEffort);
    pokemonBiosSetStyle(gc, src->style);
    pokemonBiosSetBeautiful(gc, src->beautiful);
    pokemonBiosSetCute(gc, src->cute);
    pokemonBiosSetClever(gc, src->clever);
    pokemonBiosSetStrong(gc, src->strong);
    pokemonBiosSetFur(gc, src->fur);

    pokemonBiosSetPokerus(gc, misc.origins.pokerus);
    pokemonBiosSetCatchFloorId(gc, misc.origins.metLocation);
    pokemonBiosSetCatchLevel(gc, misc.origins.metLevel);
    pokemonBiosSetCatchBallId(gc, misc.origins.ballId);
    pokemonBiosSetCatchTrainerSex(gc, misc.origins.otGender);

    pokemonBiosSetMaxHpRnd(gc, misc.ivs.hpIv);
    pokemonBiosSetPhyAtkRnd(gc, misc.ivs.attackIv);
    pokemonBiosSetPhyDefRnd(gc, misc.ivs.defenseIv);
    pokemonBiosSetNimblenessRnd(gc, misc.ivs.speedIv);
    pokemonBiosSetSpeAtkRnd(gc, misc.ivs.spAttackIv);
    pokemonBiosSetSpeDefRnd(gc, misc.ivs.spDefenseIv);
    pokemonBiosSetTamagoFlag(gc, misc.ivs.isEgg);
    pokemonBiosSetTokuseiFlag(gc, misc.ivs.altAbility);

    pokemonBiosSetStyleMedal(gc, misc.ribbons.styleMedal);
    pokemonBiosSetBeautifulMedal(gc, misc.ribbons.beautifulMedal);
    pokemonBiosSetCuteMedal(gc, misc.ribbons.cuteMedal);
    pokemonBiosSetCleverMedal(gc, misc.ribbons.cleverMedal);
    pokemonBiosSetStrongMedal(gc, misc.ribbons.strongMedal);
    pokemonBiosSetChampRibbon(gc, misc.ribbons.champRibbon);
    pokemonBiosSetWinningRibbon(gc, misc.ribbons.winningRibbon);
    pokemonBiosSetVictoryRibbon(gc, misc.ribbons.victoryRibbon);
    pokemonBiosSetBromideRibbon(gc, misc.ribbons.bromideRibbon);
    pokemonBiosSetGanbaRibbon(gc, misc.ribbons.ganbaRibbon);
    pokemonBiosSetMarineRibbon(gc, misc.ribbons.marineRibbon);
    pokemonBiosSetLandRibbon(gc, misc.ribbons.landRibbon);
    pokemonBiosSetSkyRibbon(gc, misc.ribbons.skyRibbon);
    pokemonBiosSetCountryRibbon(gc, misc.ribbons.countryRibbon);
    pokemonBiosSetNationalRibbon(gc, misc.ribbons.nationalRibbon);
    pokemonBiosSetEarthRibbon(gc, misc.ribbons.earthRibbon);
    pokemonBiosSetWorldRibbon(gc, misc.ribbons.worldRibbon);
    pokemonBiosSetAmariRibbon(gc, misc.ribbons.amariRibbon);
    pokemonBiosSetEventGetFlag(gc, misc.ribbons.eventGet);

    status = GbaSwap32(src->status);
    if ((status & 0x80) != 0) {
        fn_801219F4(gc, 4, 0);
        fn_8012190C(gc, 4, (status & 0xF00) >> 8);
    } else if ((status & 0x40) != 0) {
        fn_801219F4(gc, 5, 0);
    } else if ((status & 0x20) != 0) {
        fn_801219F4(gc, 7, 0);
    } else if ((status & 0x10) != 0) {
        fn_801219F4(gc, 6, 0);
    } else if ((status & 0x8) != 0) {
        fn_801219F4(gc, 3, 0);
    } else if ((status & 0x7) != 0) {
        fn_801219F4(gc, 8, 0);
        fn_8012173C(gc, 8, status & 0x7);
    }
    pokemonBiosSetConditionAmari(gc, GbaSwap32(src->status) & 0xFFFFF000);

    pokemonBiosSetLevel(gc, src->level);
    pokemonBiosSetMailId(gc, src->mailId);
    pokemonBiosSetMaxHp(gc, GbaSwap16(src->maxHp));
    pokemonBiosSetHp(gc, GbaSwap16(src->hp));
    pokemonBiosSetPhyAtk(gc, GbaSwap16(src->phyAtk));
    pokemonBiosSetPhyDef(gc, GbaSwap16(src->phyDef));
    pokemonBiosSetNimbleness(gc, GbaSwap16(src->nimbleness));
    pokemonBiosSetSpeAtk(gc, GbaSwap16(src->speAtk));
    pokemonBiosSetSpeDef(gc, GbaSwap16(src->speDef));
}

/* 0x8008C5D4 | size: 0x128 */
u16 gbaPokemonConditonFromGC(void* pokemon) {
    extern s16 fn_80121984(void* pokemon, s32 kind);
    extern s8 fn_8012189C(void* pokemon, s32 kind);
    extern u8 fn_80121ADC(void* pokemon, s32 kind);
    u16 status = 0;

    if (fn_80121ADC(pokemon, 4) != 0) {
        status = (fn_80121984(pokemon, 4) << 8) | 0x80;
    } else if (fn_80121ADC(pokemon, 5) != 0) {
        status |= 0x40;
    } else if (fn_80121ADC(pokemon, 7) != 0) {
        status |= 0x20;
    } else if (fn_80121ADC(pokemon, 6) != 0) {
        status |= 0x10;
    } else if (fn_80121ADC(pokemon, 3) != 0) {
        status |= 0x08;
    } else if (fn_80121ADC(pokemon, 8) != 0) {
        status = (s16)fn_8012189C(pokemon, 8);
    }
    return status;
}

/* 0x8008C6FC | size: 0x4 */
void fn_8008C6FC(void) {
}

/* 0x8008C700 | size: 0x8C */
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

/* 0x8008C78C | size: 0x24 */
s32 fn_8008C78C(void) {
    extern s32 std082_flagGetValue(s32);
    return std082_flagGetValue(0xb5d);
}

/* 0x8008C7B0 | size: 0x31C */
void fn_8008C7B0(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std078_flagSetFalse();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r29;
    ((void(*)(void))GSmodelStartAnimation)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xB720000;
    r28 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r27 = tmp;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r4 = 0xD040000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xD0D0000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    r4 = 0xD0D0000;
    r29 = r3;
    r3 = r4 + 0x1001;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r30 = tmp;
    r4 = r28;
    r5 = r31;
    r6 = r29;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r30;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r27;
    r4 = 0x6;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x8d0;
    std078_flagSetFalse();
    r3 = 0x1;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008CACC | size: 0x30C */
void fn_8008CACC(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r29;
    ((void(*)(void))GSmodelStartAnimation)();
    r4 = 0x11210000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0xCE60000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0xCE60000;
    r4 = 0x2;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r30 = 0x64;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1DC;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0x6BC0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xD020000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xD0C0000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008CDD8 | size: 0x2C8 */
void fn_8008CDD8(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void scriptWaitSyncMotion();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r29;
    ((void(*)(void))GSmodelStartAnimation)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r4 = 0xD010000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xD0B0000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r28;
    r4 = 0xc;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008D0A0 | size: 0x2A8 */
void fn_8008D0A0(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r29;
    ((void(*)(void))GSmodelStartAnimation)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r4 = 0xD000000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xD0A0000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008D348 | size: 0x5F0 */
void fn_8008D348(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x50];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r22 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r22 = r3;
        if (r22 < 1) {
            r22 = 0x1;
    }
    }
    r21 = 0x0;
    while (r21 < r22) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r21 = r21 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r21 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r21 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r21 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r21 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r21;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r21;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r21;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r21;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r21;
    ((void(*)(void))GSmodelStartAnimation)();
    r4 = 0x11200000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0xCE60000;
    r21 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r21;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BC0000;
    r30 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r24 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r25 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r26 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r27 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r29 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r30;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r24;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r25;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r26;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r29;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r21 = r3;
    r3 = r31;
    r4 = r30;
    ((void(*)(void))GSresGetResource)();
    r4 = r21;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r3 = r31;
    r4 = r30;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r4 = 0xCFF0000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r22 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r22 = r3;
        if (r22 < 1) {
            r22 = 0x1;
    }
    }
    r21 = 0x0;
    while (r21 < r22) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r21 = r21 + r3;

    }
    r3 = 0xD090000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    r4 = 0xD090000;
    r23 = r3;
    r3 = r4 + 0x1001;
    std112_modelGetResourceID();
    r4 = 0xD090000;
    r22 = r3;
    r3 = r4 + 0x1006;
    std112_modelGetResourceID();
    r4 = 0xD090000;
    r21 = r3;
    r3 = r4 + 0x1002;
    std112_modelGetResourceID();
    r4 = 0xD090000;
    r20 = r3;
    r3 = r4 + 0x1003;
    std112_modelGetResourceID();
    r4 = 0xD090000;
    r19 = r3;
    r3 = r4 + 0x1004;
    std112_modelGetResourceID();
    r4 = 0xD090000;
    r18 = r3;
    r3 = r4 + 0x1005;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r17 = tmp;
    r4 = r30;
    r5 = r31;
    r6 = r23;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r24;
    r5 = r31;
    r6 = r22;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r25;
    r5 = r31;
    r6 = r21;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r26;
    r5 = r31;
    r6 = r20;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r19;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r29;
    r5 = r31;
    r6 = r18;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r28;
    r5 = r31;
    r6 = r17;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = 0xCE60000;
    r4 = 0x1;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r30;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r24;
    r4 = 0x7;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r25;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r26;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r27;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r29;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008D938 | size: 0x9E8 */
void fn_8008D938(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void scriptWaitSyncMotion();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r14 = 0;
    u32 r15 = 0;
    u32 r16 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r15 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r15;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r16 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r16 = r3;
        if (r16 < 1) {
            r16 = 0x1;
    }
    }
    r14 = 0x0;
    while (r14 < r16) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r14 = r14 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r15;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r14 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r14 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r14 + 0x144) = tmp;
    r3 = r15;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r14 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r14;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r14;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r14;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r14;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r14;
    ((void(*)(void))GSmodelStartAnimation)();
    r4 = 0x111B0000;
    r3 = r15;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0xCE60000;
    r14 = r3;
    r3 = r15;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r14;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0xCE60000;
    r3 = r15;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r16 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r16 = r3;
        if (r16 < 1) {
            r16 = 0x1;
    }
    }
    r14 = 0x0;
    while (r14 < r16) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r14 = r14 + r3;

    }
    r4 = 0x111F0000;
    r3 = r15;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0xCE60000;
    r14 = r3;
    r3 = r15;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r14;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0xCE60000;
    r3 = r15;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0x6BC0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r31 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD290000;
    r30 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r29 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r28 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD290000;
    r27 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r26 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r25 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r24 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r15;
    r23 = tmp;
    r4 = r31;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r30;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r29;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r27;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r26;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r25;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r24;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r23;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xCFE0000;
    r3 = r15;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r16 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r16 = r3;
        if (r16 < 1) {
            r16 = 0x1;
    }
    }
    r14 = 0x0;
    while (r14 < r16) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r14 = r14 + r3;

    }
    r3 = 0xD080000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    r4 = 0xD080000;
    r3 = r4 + 0x1008;
    std112_modelGetResourceID();
    r4 = 0xD080000;
    r14 = r3;
    r3 = r4 + 0x1001;
    std112_modelGetResourceID();
    r4 = 0xD080000;
    r22 = r3;
    r3 = r4 + 0x1002;
    std112_modelGetResourceID();
    r4 = 0xD080000;
    r21 = r3;
    r3 = r4 + 0x1003;
    std112_modelGetResourceID();
    r4 = 0xD080000;
    r20 = r3;
    r3 = r4 + 0x1004;
    std112_modelGetResourceID();
    r4 = 0xD080000;
    r19 = r3;
    r3 = r4 + 0x1005;
    std112_modelGetResourceID();
    r4 = 0xD080000;
    r18 = r3;
    r3 = r4 + 0x1006;
    std112_modelGetResourceID();
    r4 = 0xD080000;
    r17 = r3;
    r3 = r4 + 0x1007;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r15;
    r4 = r31;
    r16 = tmp;
    r5 = r15;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r30;
    r5 = r15;
    r6 = r14;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r29;
    r5 = r15;
    r6 = r22;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r28;
    r5 = r15;
    r6 = r21;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r27;
    r5 = r15;
    r6 = r20;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r26;
    r5 = r15;
    r6 = r19;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r25;
    r5 = r15;
    r6 = r18;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r24;
    r5 = r15;
    r6 = r17;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r23;
    r5 = r15;
    r6 = r16;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r31;
    r4 = 0x6;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r30;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r29;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r27;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r26;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r25;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r24;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r24;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r23;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r31;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r30;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r29;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r28;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r27;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r26;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r25;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r24;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r24;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r23;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r31;
    r4 = 0x7;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r30;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r29;
    r4 = 0xc;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r26;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r27;
    r4 = 0xc;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r25;
    r4 = 0xc;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r24;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r24;
    r4 = 0xc;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r23;
    r4 = 0xc;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r31;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r30;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r29;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r28;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r27;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r26;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r25;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r24;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r24;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r23;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r31;
    r4 = 0x6;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r30;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r29;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r27;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r26;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r25;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r24;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r24;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r23;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008E320 | size: 0x4B4 */
void fn_8008E320(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r27 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r27 = r3;
        if (r27 < 1) {
            r27 = 0x1;
    }
    }
    r26 = 0x0;
    while (r26 < r27) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r26 = r26 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r26 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r26 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r26 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r26 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r26;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r26;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r26;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r26;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r26;
    ((void(*)(void))GSmodelStartAnimation)();
    r4 = 0x111B0000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0xCE60000;
    r26 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r26;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r27 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r27 = r3;
        if (r27 < 1) {
            r27 = 0x1;
    }
    }
    r26 = 0x0;
    while (r26 < r27) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r26 = r26 + r3;

    }
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BC0000;
    r30 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r29 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r28 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r27 = tmp;
    r4 = r30;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r26 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r26;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r26;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r29;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r26 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r26;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r26;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r26 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r26;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r26;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r26 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r26;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r26;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r26 = r3;
    r3 = r31;
    r4 = r30;
    ((void(*)(void))GSresGetResource)();
    r4 = r26;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r3 = r31;
    r4 = r30;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r4 = 0xCFD0000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r25 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r25 = r3;
        if (r25 < 1) {
            r25 = 0x1;
    }
    }
    r26 = 0x0;
    while (r26 < r25) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r26 = r26 + r3;

    }
    r3 = 0xD070000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    r4 = 0xD070000;
    r25 = r3;
    r3 = r4 + 0x1001;
    std112_modelGetResourceID();
    r4 = 0xD070000;
    r26 = r3;
    r3 = r4 + 0x1003;
    std112_modelGetResourceID();
    r4 = 0xD070000;
    r24 = r3;
    r3 = r4 + 0x1002;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r23 = tmp;
    r4 = r30;
    r5 = r31;
    r6 = r25;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r29;
    r5 = r31;
    r6 = r26;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r28;
    r5 = r31;
    r6 = r24;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r23;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r30;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r29;
    r4 = 0x5;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r27;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008E7D4 | size: 0x454 */
void fn_8008E7D4(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void scriptWaitSyncMotion();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r28 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r28;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r27 = 0x0;
    while (r27 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r27 = r27 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r28;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r27 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r27 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r27 + 0x144) = tmp;
    r3 = r28;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r27 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r27;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r27;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r27;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r27;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r27;
    ((void(*)(void))GSmodelStartAnimation)();
    r4 = 0x111B0000;
    r3 = r28;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0xCE60000;
    r27 = r3;
    r3 = r28;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r27;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0xCE60000;
    r3 = r28;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r27 = 0x0;
    while (r27 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r27 = r27 + r3;

    }
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BC0000;
    r31 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r30 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r28;
    r29 = tmp;
    r4 = r31;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r27 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r27;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r27;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r28;
    r4 = r30;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r27 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r27;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r27;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r28;
    r4 = r29;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r27 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r27;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r27;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0x11510000;
    r3 = r28;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r27 = r3;
    r3 = r28;
    r4 = r31;
    ((void(*)(void))GSresGetResource)();
    r4 = r27;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r3 = r28;
    r4 = r31;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r4 = 0xCFC0000;
    r3 = r28;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r26 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r26 = r3;
        if (r26 < 1) {
            r26 = 0x1;
    }
    }
    r27 = 0x0;
    while (r27 < r26) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r27 = r27 + r3;

    }
    r3 = 0xD060000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    r4 = 0xD060000;
    r26 = r3;
    r3 = r4 + 0x1001;
    std112_modelGetResourceID();
    r4 = 0xD060000;
    r27 = r3;
    r3 = r4 + 0x1002;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r28;
    r25 = tmp;
    r4 = r31;
    r5 = r28;
    r6 = r26;
    r7 = 0x2;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = r30;
    r5 = r28;
    r6 = r27;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = r29;
    r5 = r28;
    r6 = r25;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = 0x7;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r31;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r31;
    r4 = 0x8;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r30;
    r4 = 0x5;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r29;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008EC28 | size: 0x2A8 */
void fn_8008EC28(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r29;
    ((void(*)(void))GSmodelStartAnimation)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r4 = 0xCFB0000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xD050000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = 0x7;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008EED0 | size: 0x2C0 */
void fn_8008EED0(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void scriptWaitSyncMotion();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r29;
    ((void(*)(void))GSmodelStartAnimation)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x4;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r4 = 0xCFA0000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xD030000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r28;
    r4 = 0x5;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008F190 | size: 0x394 */
void fn_8008F190(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void scriptWaitSyncMotion();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f8 = 0.0f;
    f32 f9 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r29;
    ((void(*)(void))GSmodelStartAnimation)();
    r4 = 0x111B0000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0xCE60000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r30 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x0;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r4 = 0xCF90000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xCF80000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = 0x1;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r28;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r28;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008F524 | size: 0x3F8 */
void fn_8008F524(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f7 = 0.0f;

    r28 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r28;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r28;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r29 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r29 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r29 + 0x144) = tmp;
    r3 = r28;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r29;
    ((void(*)(void))GSmodelStartAnimation)();
    r4 = 0x111B0000;
    r3 = r28;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0xCE60000;
    r29 = r3;
    r3 = r28;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r29;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0xCE60000;
    r3 = r28;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r30 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0x6BC0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r31 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r30 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r28;
    r29 = tmp;
    r4 = r31;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r27 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r27;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r27;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r28;
    r4 = r30;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r27 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r27;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r27;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r28;
    r4 = r29;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r27 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r27;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r27;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xCF70000;
    r3 = r28;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r26 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r26 = r3;
        if (r26 < 1) {
            r26 = 0x1;
    }
    }
    r27 = 0x0;
    while (r27 < r26) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r27 = r27 + r3;

    }
    r3 = 0xCEE0000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    r4 = 0xCEE0000;
    r26 = r3;
    r3 = r4 + 0x1002;
    std112_modelGetResourceID();
    r4 = 0xCEE0000;
    r27 = r3;
    r3 = r4 + 0x1001;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r28;
    r25 = tmp;
    r4 = r31;
    r5 = r28;
    r6 = r26;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = r30;
    r5 = r28;
    r6 = r27;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = r29;
    r5 = r28;
    r6 = r25;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r30;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r29;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008F91C | size: 0x2D8 */
void fn_8008F91C(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f6 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r29;
    ((void(*)(void))GSmodelStartAnimation)();
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BA0000;
    r28 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r27 = tmp;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xCF60000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xCED0000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    r4 = 0xCED0000;
    r30 = r3;
    r3 = r4 + 0x1001;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r29 = tmp;
    r4 = r27;
    r5 = r31;
    r6 = r30;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r28;
    r5 = r31;
    r6 = r29;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r27;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008FBF4 | size: 0x2A0 */
void fn_8008FBF4(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f5 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r29;
    ((void(*)(void))GSmodelStartAnimation)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x0;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r4 = 0xCF50000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xCEC0000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8008FE94 | size: 0x26C */
void fn_8008FE94(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f4 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r29;
    ((void(*)(void))GSmodelStartAnimation)();
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xCF40000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xCEB0000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x80090100 | size: 0x620 */
void fn_80090100(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r15 = 0;
    u32 r16 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f3 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r18 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r18 = r3;
        if (r18 < 1) {
            r18 = 0x1;
    }
    }
    r17 = 0x0;
    while (r17 < r18) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r17 = r17 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r18 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r18 = r3;
        if (r18 < 1) {
            r18 = 0x1;
    }
    }
    r17 = 0x0;
    while (r17 < r18) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r17 = r17 + r3;

    }
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r17 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r17 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r17 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x0;
    r17 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r17;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r17;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r17;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r17;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r17;
    ((void(*)(void))GSmodelStartAnimation)();
    r3 = 0x6BC0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xCEA0000;
    r23 = r3;
    r3 = r4 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r22 = tmp;
    r4 = r23;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = 0xD290000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r24 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r25 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r26 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r27 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD290000;
    r30 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r29 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r24;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r25;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r26;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r30;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r29;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xCF30000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r18 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r18 = r3;
        if (r18 < 1) {
            r18 = 0x1;
    }
    }
    r17 = 0x0;
    while (r17 < r18) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r17 = r17 + r3;

    }
    r3 = 0xCEA0000;
    r3 = r3 + 0x1006;
    std112_modelGetResourceID();
    r4 = 0xCEA0000;
    r21 = r3;
    r3 = r4 + 0x1007;
    std112_modelGetResourceID();
    r4 = 0xCEA0000;
    r20 = r3;
    r3 = r4 + 0x1001;
    std112_modelGetResourceID();
    r4 = 0xCEA0000;
    r19 = r3;
    r3 = r4 + 0x1002;
    std112_modelGetResourceID();
    r4 = 0xCEA0000;
    r18 = r3;
    r3 = r4 + 0x1003;
    std112_modelGetResourceID();
    r4 = 0xCEA0000;
    r17 = r3;
    r3 = r4 + 0x1004;
    std112_modelGetResourceID();
    r4 = 0xCEA0000;
    r16 = r3;
    r3 = r4 + 0x1005;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r15 = tmp;
    r4 = r23;
    r5 = r31;
    r6 = r22;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r24;
    r5 = r31;
    r6 = r21;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r25;
    r5 = r31;
    r6 = r20;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r26;
    r5 = r31;
    r6 = r19;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r18;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r30;
    r5 = r31;
    r6 = r17;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r29;
    r5 = r31;
    r6 = r16;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r28;
    r5 = r31;
    r6 = r15;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r23;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r24;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r25;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r26;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r27;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r30;
    r4 = 0x5;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r29;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x80090720 | size: 0x2C4 */
void fn_80090720(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void scriptWaitSyncMotion();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = 0xCE60000;
    r3 = r31;
    *(f32*)(sp + 0x8) = f0;
    r4 = r4 + 0x1004;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x1;
    r30 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r30;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r30;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x1;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r30;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r30;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r30;
    ((void(*)(void))GSmodelStartAnimation)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r3 = 0x6BC0000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xCF20000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xCE90000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = 0x1;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1004;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r28;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0x1;
    scriptWaitSyncMotion();
    r3 = r28;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x800909E4 | size: 0x350 */
void fn_800909E4(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = 0xCE60000;
    r3 = r31;
    *(f32*)(sp + 0x8) = f0;
    r4 = r4 + 0x1004;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x1;
    r30 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r30;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r30;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x1;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r30;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r30;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r30;
    ((void(*)(void))GSmodelStartAnimation)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0x111B0000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0xCE60000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r30 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BA0000;
    r28 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r27 = tmp;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xCF10000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xCE80000;
    r3 = r3 + 0x1001;
    std112_modelGetResourceID();
    r4 = 0xCE80000;
    r29 = r3;
    r3 = r4 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r30 = tmp;
    r4 = r28;
    r5 = r31;
    r6 = r29;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r30;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r27;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x80090D34 | size: 0x2D8 */
void fn_80090D34(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = 0xCE60000;
    r3 = r31;
    *(f32*)(sp + 0x8) = f0;
    r4 = r4 + 0x1004;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x1;
    r30 = r3;
    ((void(*)(void))GSmodelSetAnimIndex)();
    r3 = r30;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))GSmodelGetFrameCount)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r30;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x1;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))GSmodelSetAnimIndex)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r30;
    ((void(*)(void))GSmodelSetAnimFrame)();
    r3 = r30;
    r4 = 0x0;
    ((void(*)(void))GSmodelSetAnimType)();
    r3 = r30;
    ((void(*)(void))GSmodelStartAnimation)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r3 = 0x6BD0000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BA0000;
    r28 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r27 = tmp;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xCF00000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xCE70000;
    r3 = r3 + 0x1001;
    std112_modelGetResourceID();
    r4 = 0xCE70000;
    r29 = r3;
    r3 = r4 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r30 = tmp;
    r4 = r28;
    r5 = r31;
    r6 = r29;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r30;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r28;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r27;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x8009100C | size: 0x558 */
void fn_8009100C(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r14 = 0;
    u32 r15 = 0;
    u32 r16 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r15 = r3;
    r4 = 0x6DB0000;
    r4 = r4 + 0x1604;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DB0000;
    r3 = r15;
    r4 = r4 + 0x1001;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0x6BC0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r24 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r23 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD290000;
    r22 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r21 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r20 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD290000;
    r19 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r18 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6BE0000;
    r17 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r15;
    r16 = tmp;
    r4 = r24;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r23;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r22;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r21;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r20;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r19;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r18;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r17;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r15;
    r4 = r16;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xC390000;
    r3 = r15;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r25 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r25 = r3;
        if (r25 < 1) {
            r25 = 0x1;
    }
    }
    r14 = 0x0;
    while (r14 < r25) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r14 = r14 + r3;

    }
    r3 = 0xC380000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    r4 = 0xC380000;
    r3 = r4 + 0x1008;
    std112_modelGetResourceID();
    r4 = 0xC380000;
    r14 = r3;
    r3 = r4 + 0x1001;
    std112_modelGetResourceID();
    r4 = 0xC380000;
    r31 = r3;
    r3 = r4 + 0x1002;
    std112_modelGetResourceID();
    r4 = 0xC380000;
    r30 = r3;
    r3 = r4 + 0x1003;
    std112_modelGetResourceID();
    r4 = 0xC380000;
    r29 = r3;
    r3 = r4 + 0x1004;
    std112_modelGetResourceID();
    r4 = 0xC380000;
    r28 = r3;
    r3 = r4 + 0x1005;
    std112_modelGetResourceID();
    r4 = 0xC380000;
    r27 = r3;
    r3 = r4 + 0x1006;
    std112_modelGetResourceID();
    r4 = 0xC380000;
    r26 = r3;
    r3 = r4 + 0x1007;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r15;
    r4 = r24;
    r25 = tmp;
    r5 = r15;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r23;
    r5 = r15;
    r6 = r14;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r22;
    r5 = r15;
    r6 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r21;
    r5 = r15;
    r6 = r30;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r20;
    r5 = r15;
    r6 = r29;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r19;
    r5 = r15;
    r6 = r28;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r18;
    r5 = r15;
    r6 = r27;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r17;
    r5 = r15;
    r6 = r26;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r15;
    r4 = r16;
    r5 = r15;
    r6 = r25;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r24;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r23;
    r4 = 0x5;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r22;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r21;
    r4 = 0xf;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r20;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r19;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r18;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r17;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r16;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x80091564 | size: 0x210 */
void fn_80091564(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0x6DC0000;
    r4 = r4 + 0x1605;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1001;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0x6DC0000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0x11260000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x6DC0000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0x6DC0000;
    r4 = 0x4;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x64;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1DC;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x7;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xC420000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xC3D0000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x81;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x80091774 | size: 0x210 */
void fn_80091774(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0x6DC0000;
    r4 = r4 + 0x1605;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1001;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0x6DC0000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0x11250000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x6DC0000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0x6DC0000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x64;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1DC;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x6;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xC410000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xC3C0000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x82;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x80091984 | size: 0x210 */
void fn_80091984(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0x6DC0000;
    r4 = r4 + 0x1605;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1001;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0x6DC0000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0x11240000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x6DC0000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0x6DC0000;
    r4 = 0x2;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x64;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1DC;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x5;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xC400000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xC3B0000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x82;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x80091B94 | size: 0x210 */
void fn_80091B94(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0x6DC0000;
    r4 = r4 + 0x1605;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1001;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0x6DC0000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0x11220000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x6DC0000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r30;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0x6DC0000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r29 = 0x64;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1DC;
        ((void(*)(void))__cvt_fp2unsigned)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0x1;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xC3E0000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0x10490000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x82;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x80091DA4 | size: 0x1A4 */
void fn_80091DA4(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r29 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r29;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r3 = 0xCE60000;
    r4 = 0x1;
    r3 = r3 + 0x1004;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r4 = 0x111B0000;
    r3 = r29;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0xCE60000;
    r31 = r3;
    r3 = r29;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r31;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0xCE60000;
    r3 = r29;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r30 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r31 = 0x0;
    while (r31 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r31 = r31 + r3;

    }
    r4 = 0xCEF0000;
    r3 = r29;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r31 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r31 = r3;
        if (r31 < 1) {
            r31 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r31) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x82;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x80091F48 | size: 0x1F8 */
void fn_80091F48(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void scriptWaitSyncMotion();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r30 = r3;
    r4 = 0x6DD0000;
    r4 = r4 + 0x1604;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DD0000;
    r3 = r30;
    r4 = r4 + 0x1001;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0x6BB0000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r31 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r30;
    r27 = tmp;
    r4 = r31;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r30;
    r4 = r27;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xB890000;
    r3 = r30;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r28 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r28 = r3;
        if (r28 < 1) {
            r28 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r28) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r29 = r29 + r3;

    }
    r3 = 0xB860000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    r4 = 0xB860000;
    r29 = r3;
    r3 = r4 + 0x1001;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r30;
    r28 = tmp;
    r4 = r31;
    r5 = r30;
    r6 = r29;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r30;
    r4 = r27;
    r5 = r30;
    r6 = r28;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r27;
    r4 = 0x8;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r31;
    r4 = 0x8;
    r5 = 0x32;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = r31;
    r4 = 0x0;
    scriptWaitSyncMotion();
    r3 = r31;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x80092140 | size: 0x358 */
void fn_80092140(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0x6DD0000;
    r4 = r4 + 0x1604;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DD0000;
    r3 = r31;
    r4 = r4 + 0x1001;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xD240000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r30 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r29 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r28 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r27 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r26 = tmp;
    r4 = r30;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r29;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r26;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xB880000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r24 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r24 = r3;
        if (r24 < 1) {
            r24 = 0x1;
    }
    }
    r25 = 0x0;
    while (r25 < r24) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r25 = r25 + r3;

    }
    r3 = 0xB850000;
    r3 = r3 + 0x1004;
    std112_modelGetResourceID();
    r4 = 0xB850000;
    r24 = r3;
    r3 = r4 + 0x1003;
    std112_modelGetResourceID();
    r4 = 0xB850000;
    r25 = r3;
    r3 = r4 + 0x1001;
    std112_modelGetResourceID();
    r4 = 0xB850000;
    r23 = r3;
    r3 = r4 + 0x1002;
    std112_modelGetResourceID();
    r4 = 0xB850000;
    r22 = r3;
    r3 = r4 + 0x1003;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r21 = tmp;
    r4 = r30;
    r5 = r31;
    r6 = r24;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r29;
    r5 = r31;
    r6 = r25;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r28;
    r5 = r31;
    r6 = r23;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r22;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r26;
    r5 = r31;
    r6 = r21;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r30;
    r4 = 0x6;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r29;
    r4 = 0x6;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0x8;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r27;
    r4 = 0x8;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r26;
    r4 = 0x7;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x83;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x80092498 | size: 0x1CC */
void fn_80092498(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r29 = r3;
    r4 = 0xB630000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xB630000;
    r3 = r29;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xB630000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r31 = 0x0;
    while (r31 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r31 = r31 + r3;

    }
    r3 = 0xB630000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xB630000;
    r3 = r29;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r31 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r31 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0x112B0000;
    *(u32*)((u8*)r31 + 0x144) = tmp;
    r3 = r29;
    r4 = r4 + 0x1400;
    ((void(*)(void))GSresGetResource)();
    r4 = 0xB630000;
    r31 = r3;
    r3 = r29;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = r31;
    ((void(*)(void))GSmodelLinkToGSparticleBank)();
    r4 = 0xB630000;
    r3 = r29;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x4;
    ((void(*)(void))GSmodelSetGSparticleLinkAttachMode)();
    r4 = 0xB830000;
    r3 = r29;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r31 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r31 = r3;
        if (r31 < 1) {
            r31 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r31) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0xB630000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x83;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x80092664 | size: 0x358 */
void fn_80092664(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0x6DD0000;
    r4 = r4 + 0x1604;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DD0000;
    r3 = r31;
    r4 = r4 + 0x1001;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xD240000;
    r3 = r3 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r30 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r29 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r28 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    r4 = 0xD240000;
    r27 = r3;
    r3 = r4 + 0x400;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r26 = tmp;
    r4 = r30;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r29;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r3 = r31;
    r4 = r26;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    r4 = 0xB870000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    cameraPlayAnime();
    r24 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r24 = r3;
        if (r24 < 1) {
            r24 = 0x1;
    }
    }
    r25 = 0x0;
    while (r25 < r24) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r25 = r25 + r3;

    }
    r3 = 0xB840000;
    r3 = r3 + 0x1004;
    std112_modelGetResourceID();
    r4 = 0xB840000;
    r24 = r3;
    r3 = r4 + 0x1000;
    std112_modelGetResourceID();
    r4 = 0xB840000;
    r25 = r3;
    r3 = r4 + 0x1001;
    std112_modelGetResourceID();
    r4 = 0xB840000;
    r23 = r3;
    r3 = r4 + 0x1002;
    std112_modelGetResourceID();
    r4 = 0xB840000;
    r22 = r3;
    r3 = r4 + 0x1003;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r31;
    r21 = tmp;
    r4 = r30;
    r5 = r31;
    r6 = r24;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r29;
    r5 = r31;
    r6 = r25;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r28;
    r5 = r31;
    r6 = r23;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r22;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r31;
    r4 = r26;
    r5 = r31;
    r6 = r21;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = r30;
    r4 = 0x1;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r29;
    r4 = 0x1;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = 0x1;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r27;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r26;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x87;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x800929BC | size: 0x170 */
void fn_800929BC(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std116_mapObjectStopAnimation();
    extern void std115_mapObjectPlayAnimation();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r29 = r3;
    r4 = 0xB630000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xB630000;
    r3 = r29;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xB630000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    std115_mapObjectPlayAnimation();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r31 = 0x0;
    while (r31 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r31 = r31 + r3;

    }
    r3 = 0xB630000;
    r3 = r3 + 0x1000;
    std116_mapObjectStopAnimation();
    r4 = 0xB630000;
    r3 = r29;
    r4 = r4 + 0x1000;
    ((void(*)(void))GSresGetResource)();
    r31 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r31 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    *(u32*)((u8*)r31 + 0x144) = tmp;
    ((void(*)(void))std093_getCurrentGroupID)();
    r4 = 0xB660000;
    r5 = 0x0;
    r4 = r4 + 0x1800;
    r6 = 0x0;
    cameraPlayAnime();
    r31 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r31 = r3;
        if (r31 < 1) {
            r31 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r31) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r30 = r30 + r3;

    }
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x83;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

/* 0x80092B2C | size: 0x164 */
void fn_80092B2C(void) {
    extern void fn_80176B48();
    extern void cameraPlayAnime();
    extern void std070_characterAttachToCharacter();
    extern void std115_mapObjectPlayAnimation();
    extern void std112_modelGetResourceID();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r28 = r3;
    r4 = 0xB630000;
    r4 = r4 + 0x1602;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xB630000;
    r3 = r28;
    r4 = r4 + 0x1002;
    ((void(*)(void))GSresGetResource)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))GSmodelSetShadowTextureSize)();
    r3 = 0xB720000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    r4 = 0x6;
    r29 = r3;
    r5 = 0x0;
    r6 = 0x1;
    std115_mapObjectPlayAnimation();
    r3 = r28;
    r4 = r29;
    ((void(*)(void))GSresGetResource)();
    r4 = 0x2;
    r31 = r3;
    ((void(*)(void))GSmodelSetShadowFlags)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r31;
    ((void(*)(void))GSmodelSetShadowLight)();
    r3 = r31;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))GSmodelSetShadowSurface)();
    ((void(*)(void))std093_getCurrentGroupID)();
    r4 = 0xB650000;
    r5 = 0x0;
    r4 = r4 + 0x1800;
    r6 = 0x0;
    cameraPlayAnime();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))__cvt_fp2unsigned)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r31 = 0x0;
    while (r31 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))GSgetFrameStep)();
        r31 = r31 + r3;

    }
    r3 = 0xB730000;
    r3 = r3 + 0x1000;
    std112_modelGetResourceID();
    tmp = r3;
    r3 = r28;
    r6 = tmp;
    r4 = r29;
    r5 = r28;
    r7 = 0x0;
    std070_characterAttachToCharacter();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x87;
    ((void(*)(void))floorRequestChange)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))floorSetFadeScript)();
    return;
}

#endif /* !GBA_MISC_80089F78_ONLY */

#endif
