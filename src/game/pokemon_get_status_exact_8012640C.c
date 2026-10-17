/**
 * @file pokemon_get_status_exact_8012640C.c
 * @brief pokemonGetStatus, 0x8012640C - 0x8012795C, and its jump table.
 *
 * Function-boundary carve of the pokemon TU (the unit that also owns
 * pokemonSetStatus and its jump table at 0x8035E028). The selector switch
 * owns jumptable_8035E4B0 (0x124 entries), which follows pokemonSetStatus's
 * table directly in .data, so this object carries that one table as its own
 * .data, exactly as pokemon_set_status_exact_801254B4.c does for its table.
 * Everything else the function touches is a call.
 *
 * Three cases carry bodies the pokemon TU also has out of line, expanded by
 * the base -inline auto:
 *  - 0xBA is pokemonGetSex (0x801231A4) instruction for instruction, with
 *    the u8 result routed through r0 into the caller's cast;
 *  - 0xC1 is pokemonCheckRare (0x80125390) the same way;
 *  - both sex paths expand the species' fixed-sex lookup (NULL -> 2, then
 *    the three sexGetPokemonSexRaitoKotei compares, else -1). That lookup is
 *    expanded six times across the TU (here, pokemonGetSex, and four times
 *    in pokemonCreate / pokemonCreateRndFit) and never exists out of line,
 *    so it is recovered as the static inline pokemonGetSexKotei.
 * The two TU functions are declared `inline` here so this carve emits no
 * second copy; pokemon_range_exact_801229F4.c and
 * pokemon_range_exact_801248C4.c own the out-of-line symbols.
 *
 * 0xC0 (level from experience) has the same return-temp shape as the two
 * expansions above; see pokemonGetLevelFromExp.
 *
 * Flags: the pokemon TU's GC/1.3 set (-O4,p -inline auto, lmw/stmw,
 * -sdata 8 -sdata2 8), as for the linked pokemonSetStatus carve.
 */
#include "dolphin/types.h"

u32 pokemonGetStatus(u8* obj, u32 id, u32 selector, u32 d);

extern void* pokemonDataBiosGetPtr(u32 idx);
extern u32  pokemonDataBiosGetName(u8* ptr);
extern u16  pokemonDataBiosGetBasisMaxHp(u8* ptr);
extern u16  pokemonDataBiosGetBasisPhyAtk(u8* ptr);
extern u16  pokemonDataBiosGetBasisPhyDef(u8* ptr);
extern u16  pokemonDataBiosGetBasisSpeAtk(u8* ptr);
extern u16  pokemonDataBiosGetBasisSpeDef(u8* ptr);
extern u16  pokemonDataBiosGetBasisNimbleness(u8* ptr);
extern u16  pokemonDataBiosGetGiveMaxHpEffort(u8* ptr);
extern u16  pokemonDataBiosGetGivePhyAtkEffort(u8* ptr);
extern u16  pokemonDataBiosGetGivePhyDefEffort(u8* ptr);
extern u16  pokemonDataBiosGetGiveSpeAtkEffort(u8* ptr);
extern u16  pokemonDataBiosGetGiveSpeDefEffort(u8* ptr);
extern u16  pokemonDataBiosGetGiveNimblenessEffort(u8* ptr);
extern u16  pokemonDataBiosGetGiveExp(u8* ptr);
extern u8   pokemonDataBiosGetGrowDataId(u8* ptr);
extern u8   pokemonDataBiosGetGet(u8* ptr);
extern u8   pokemonDataBiosGetSexRatio(u8* ptr);
extern u16  pokemonDataBiosGetInitFriend(u8* ptr);
extern u16  pokemonDataBiosGetItemDataId(u8* ptr, u32 idx);
extern u8   pokemonDataBiosGetZokuseiDataId(u8* ptr, u32 idx);
extern u8   pokemonDataBiosGetTokuseiDataId(u8* ptr, u32 idx);
extern u8   pokemonDataBiosGetSinkaKind(u8* ptr, u32 idx);
extern u16  pokemonDataBiosGetSinkaBuff(u8* ptr, u32 idx);
extern u16  pokemonDataBiosGetSinkaPokemonDataId(u8* ptr, u32 idx);
extern u8   pokemonDataBiosGetGetWazaLevel(u8* ptr, u32 idx);
extern u16  pokemonDataBiosGetGetWazaDataId(u8* ptr, u32 idx);
extern u8   pokemonDataBiosGetWazaMcn(u8* ptr, u32 idx);
extern u32  pokemonDataBiosGetPokebodyId(u8* ptr, u32 val);
extern u32  pokemonDataBiosGetStatusFaceMenuSpriteId(u8* ptr, u32 val);
extern u8   pokemonDataBiosGetColor(u8* ptr, u32 val);
extern u32  pokemonDataBiosGetTypeName(u8* ptr);
extern u16  pokemonDataBiosGetHeight(u8* ptr);
extern u16  pokemonDataBiosGetWeight(u8* ptr);
extern u32  pokemonDataBiosGetDoc(u8* ptr);
extern u16  pokemonDataBiosGetVoice(u8* ptr);
extern u32  pokemonDataBiosGetMitaFlag(u8* ptr);
extern u32  pokemonDataBiosGetTukamaetaFlag(u8* ptr);
extern u16  pokemonDataBiosGetNumZukan(u8* ptr);
extern u16  pokemonDataBiosGetNumPokemon(u8* ptr);
extern u32  pokemonDataBiosGetPkxDataId(u8* ptr);
extern u16  pokemonDataBiosGetKowaza(u8* ptr, u32 idx);
extern u8   fn_8011E048(u8* ptr, u32 idx);
extern u16  fn_8011E030(u8* ptr);
extern u8   fn_8011E018(u8* ptr);
extern u8   fn_8011E000(u8* ptr);
extern u16  pokemonBiosGetPokemonDataId(u8* ptr);
extern u32  pokemonBiosGetRnd(u8* ptr);
extern u32  pokemonBiosGetAttest(u8* ptr);
extern u16  pokemonBiosGetCatchFloorId(u8* ptr);
extern u8   pokemonBiosGetCatchLevel(u8* ptr);
extern u8   pokemonBiosGetCatchBallId(u8* ptr);
extern u8   pokemonBiosGetCatchTrainerSex(u8* ptr);
extern u32  pokemonBiosGetCatchTrainerRnd(u8* ptr);
extern u32  pokemonBiosGetCatchTrainerNamePtr(u8* ptr);
extern u32  pokemonBiosGetNicknamePtr(u8* ptr);
extern u32  pokemonBiosGetNicknameOrgPtr(u8* ptr);
extern u32  pokemonBiosGetExp(u8* ptr);
extern u8   pokemonBiosGetLevel(u8* ptr);
extern u32  pokemonBiosGetJoutaiPtr(u8* ptr, u32 val);
extern u32  pokemonBiosGetConditionAmari(u8* ptr);
extern u16  pokemonBiosGetPokemonWazaDataId(u8* ptr, u32 val);
extern u8   pokemonBiosGetPokemonWazaPp(u8* ptr, u32 val);
extern u8   pokemonBiosGetPokemonWazaPpCount(u8* ptr, u32 val);
extern u16  pokemonBiosGetItemDataId(u8* ptr);
extern u16  pokemonBiosGetHp(u8* ptr);
extern u16  pokemonBiosGetMaxHp(u8* ptr);
extern u16  pokemonBiosGetPhyAtk(u8* ptr);
extern u16  pokemonBiosGetPhyDef(u8* ptr);
extern u16  pokemonBiosGetSpeAtk(u8* ptr);
extern u16  pokemonBiosGetSpeDef(u8* ptr);
extern u16  pokemonBiosGetNimbleness(u8* ptr);
extern u16  pokemonBiosGetMaxHpEffort(u8* ptr);
extern u16  pokemonBiosGetPhyAtkEffort(u8* ptr);
extern u16  pokemonBiosGetPhyDefEffort(u8* ptr);
extern u16  pokemonBiosGetSpeAtkEffort(u8* ptr);
extern u16  pokemonBiosGetSpeDefEffort(u8* ptr);
extern u16  pokemonBiosGetNimblenessEffort(u8* ptr);
extern u16  pokemonBiosGetMaxHpRnd(u8* ptr);
extern u16  pokemonBiosGetPhyAtkRnd(u8* ptr);
extern u16  pokemonBiosGetPhyDefRnd(u8* ptr);
extern u16  pokemonBiosGetSpeAtkRnd(u8* ptr);
extern u16  pokemonBiosGetSpeDefRnd(u8* ptr);
extern u16  pokemonBiosGetNimblenessRnd(u8* ptr);
extern u16  pokemonBiosGetFriend(u8* ptr);
extern u8   pokemonBiosGetStyle(u8* ptr);
extern u8   pokemonBiosGetBeautiful(u8* ptr);
extern u8   pokemonBiosGetCute(u8* ptr);
extern u8   pokemonBiosGetClever(u8* ptr);
extern u8   pokemonBiosGetStrong(u8* ptr);
extern u8   pokemonBiosGetFur(u8* ptr);
extern u8   pokemonBiosGetChampRibbon(u8* ptr);
extern u8   pokemonBiosGetWinningRibbon(u8* ptr);
extern u8   pokemonBiosGetVictoryRibbon(u8* ptr);
extern u8   pokemonBiosGetBromideRibbon(u8* ptr);
extern u8   pokemonBiosGetGanbaRibbon(u8* ptr);
extern u8   pokemonBiosGetMarineRibbon(u8* ptr);
extern u8   pokemonBiosGetLandRibbon(u8* ptr);
extern u8   pokemonBiosGetSkyRibbon(u8* ptr);
extern u8   pokemonBiosGetCountryRibbon(u8* ptr);
extern u8   pokemonBiosGetNationalRibbon(u8* ptr);
extern u8   pokemonBiosGetEarthRibbon(u8* ptr);
extern u8   pokemonBiosGetWorldRibbon(u8* ptr);
extern u8   pokemonBiosGetAmariRibbon(u8* ptr);
extern u8   pokemonBiosGetStyleMedal(u8* ptr);
extern u8   pokemonBiosGetBeautifulMedal(u8* ptr);
extern u8   pokemonBiosGetCuteMedal(u8* ptr);
extern u8   pokemonBiosGetCleverMedal(u8* ptr);
extern u8   pokemonBiosGetStrongMedal(u8* ptr);
extern u8   pokemonBiosGetPokerus(u8* ptr);
extern u8   pokemonBiosGetTamagoFlag(u8* ptr);
extern u8   pokemonBiosGetTokuseiFlag(u8* ptr);
extern u8   pokemonBiosGetFuseiFlag(u8* ptr);
extern u8   pokemonBiosGetFlagAmari(u8* ptr);
extern u8   pokemonBiosGetPcboxMark(u8* ptr);
extern u8   pokemonBiosGetMailId(u8* ptr);
extern u16  pokemonBiosGetPara1Amari(u8* ptr);
extern u16  pokemonBiosGetAmari(u8* ptr);
extern u16  pokemonBiosGetFightTrainerPokemonDataId(u8* ptr);
extern u16  pokemonBiosGetDarkpokemonDataId(u8* ptr);
extern u32  pokemonBiosGetInitDp(u8* ptr);
extern u32  pokemonBiosGetDp(u8* ptr);
extern u32  pokemonBiosGetPoolExp(u8* ptr);
extern u16  pokemonBiosGetPoolFriend(u8* ptr);
extern u8   pokemonBiosGetDarkFlag(u8* ptr);
extern u32  pokemonBiosGetDarkJoutaiPtr(u8* ptr, u32 val);
extern void* pokemonGrowDataBiosGetPtr(u32 idx);
extern u32  pokemonGrowDataBiosGetExp(void* tbl, u8 level);
extern u8   sexGetPokemonSexRaitoKotei(u32 idx);
extern u32  wazaGetStatus(u32 a, u32 b, u32 c, u32 d);
extern u32  fightPokemonBiosGetMotoPokemonPtr(u8* ptr);
extern u32  fightPokemonBiosGetPokemonBuffPtr(u8* ptr);
extern u32  fightPokemonBiosGetFightJoutaiPtr(u8* ptr, u32 val);
extern u32  fightPokemonBiosGetEntryId(u8* ptr);
extern u8   fightPokemonBiosGetCatchEntryFlag(u8* ptr);
extern u8   fightPokemonBiosGetLevelUpFlag(u8* ptr);
extern u8   fightPokemonBiosGetDarkOutFlag(u8* ptr);
extern u8   fightPokemonBiosGetHokakuFlag(u8* ptr);
extern u32  fightOutPokemonBiosGetMotoFightPokemonPtr(u8* ptr);
extern u32  fightOutPokemonBiosGetFightPokemonPtr(u8* ptr);
extern u32  fightOutPokemonBiosGetFightPokemonHensinBuffPtr(u8* ptr);
extern u32  fightOutPokemonBiosGetFightoutJoutaiPtr(u8* ptr, u32 val);
extern u32  fightOutPokemonBiosGetFightWazaPtr(u8* ptr);
extern u16  fightOutPokemonGetUseWazaDataId(u8* ptr);
extern u16  fightOutPokemonGetMotoWazaDataId(u8* ptr);
extern u8   fightWazaIsJoutaiDataId(u32 ctx, u32 param);
extern u8   fightWazaIsHit(u32 ctx);
extern u32  fightOutPokemonBiosGetFightItemPtr(u8* ptr);
extern u8   fightOutPokemonBiosGetAbicntPhyAtk(u8* ptr);
extern u8   fightOutPokemonBiosGetAbicntPhyDef(u8* ptr);
extern u8   fightOutPokemonBiosGetAbicntSpeAtk(u8* ptr);
extern u8   fightOutPokemonBiosGetAbicntSpeDef(u8* ptr);
extern u8   fightOutPokemonBiosGetAbicntNimbleness(u8* ptr);
extern u8   fightOutPokemonBiosGetAbicntAverage(u8* ptr);
extern u8   fightOutPokemonBiosGetAbicntAvoid(u8* ptr);
extern u16  fightOutPokemonBiosGetFightoutTurnCount(u8* ptr);
extern u32  fightOutPokemonBiosGetSequencePtr(u8* ptr);
extern u16  fightOutPokemonBiosGetSketchWazaDataId(u8* ptr);
extern u16  fightOutPokemonBiosGetLastSelectWazaDataId(u8* ptr);
extern u16  fightOutPokemonBiosGetLastUseWazaDataId(u8* ptr);
extern u16  fightOutPokemonBiosGetLastReceiveWazaTargetDataId(u8* ptr);
extern u16  fightOutPokemonBiosGetHitWazaDataId(u8* ptr);
extern u16  fightOutPokemonBiosGetHitWazaZokuseiDataId(u8* ptr);
extern s16  fightOutPokemonBiosGetGamanDamageValue(u8* ptr);
extern u16  fightOutPokemonBiosGetGamanDamageTargetId(u8* ptr);
extern u16  fightOutPokemonBiosGetOumuWazaDataId(u8* ptr);
extern u32  fightOutPokemonBiosGetKeepFightWazaPtr(u8* ptr);
extern u8   fightOutPokemonBiosGetNamakeFlag(u8* ptr);
extern u16  fightOutPokemonBiosGetUsedItemDataId(u8* ptr);
extern u16  fightOutPokemonBiosGetStockItemDataId(u8* ptr);
extern u8   fightOutPokemonBiosGetSuccessCnt(u8* ptr);
extern s16  fightOutPokemonBiosGetMeetEnemyFightPokemonEntryId(u8* ptr, u8 idx);
extern u32  fightOutPokemonBiosGetFightActionBuffPtr(u8* ptr);
extern u16  fightOutPokemonBiosGetZokuseiDataId(u8* ptr, u8 idx);
extern u16  fightOutPokemonBiosGetTokuseiDataId(u8* ptr);
extern u32  fightOutPokemonBiosGetWazaMenuCurPtr(u8* ptr);
extern s16  fightOutPokemonBiosGetDamageAtkValue(u8* ptr);
extern u16  fightOutPokemonBiosGetDamageAtkTargetId(u8* ptr);
extern s16  fightOutPokemonBiosGetDamageSpeValue(u8* ptr);
extern u16  fightOutPokemonBiosGetDamageSpeTargetId(u8* ptr);
extern u8   fightOutPokemonBiosGetMahiNoAttackFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetKonranMyselfAttackFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetOutWazaKoukanaiFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetTameWazaFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetItemNigeruFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetHuuinNoAttackFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetMeroMeroNoAttackFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetKanashibariNoAttackFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetChouhatsuNoAttackFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetIchamonNoAttackFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetHirumuNoAttackFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetPassPpdecFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetFightActionFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetDoClearbodyFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetReceivesWazaHiraishinFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetVanishoffFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetDoIkakuFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetDoTraceFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetNoPressureFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetIrekaetaFlag(u8* ptr);
extern u8   fightOutPokemonBiosGetItemKoraetaFlag(u8* ptr);
extern u32  fightOutPokemonBiosGetKaigaraDamageValue(u8* ptr);
extern s16  fightOutPokemonBiosGetMyselfDamageAtkValue(u8* ptr);
extern u16  fightOutPokemonBiosGetMyselfDamageAtkTargetId(u8* ptr);
extern s16  fightOutPokemonBiosGetMyselfDamageSpeValue(u8* ptr);
extern u16  fightOutPokemonBiosGetMyselfDamageSpeTargetId(u8* ptr);
extern u8   fightOutPokemonBiosGetKizetuFlag(u8* ptr);
extern s16  fightOutPokemonBiosGetIrekaeTargetEntryId(u8* ptr);
extern u32  fightOutPokemonBiosGetFightOutPokemonEnemyPtr(u8* ptr, u32 val);
extern u8   fightOutPokemonCheckFightOut(u8* ptr);

/*
 * Fixed sex of obj's species: 0/1/2 when the species' sex ratio is one of
 * the three fixed ratios, -1 when the sex follows from the personality.
 */
static inline s8 pokemonGetSexKotei(u8* obj)
{
    u16 ratio;

    if (obj == NULL) {
        return 2;
    }
    ratio = (u16)pokemonGetStatus(NULL, (u16)pokemonGetStatus(obj, 0, 0x6E, 0), 0x13, 0);
    if (ratio == sexGetPokemonSexRaitoKotei(0)) {
        return 0;
    }
    if (ratio == sexGetPokemonSexRaitoKotei(1)) {
        return 1;
    }
    if (ratio == sexGetPokemonSexRaitoKotei(2)) {
        return 2;
    }
    return -1;
}

inline u8 pokemonGetSex(u8* obj)
{
    u32 rnd;
    u16 ratio;
    s8 sex;

    if (obj == NULL) {
        return 2;
    }
    rnd = pokemonGetStatus(obj, 0, 0x6F, 0);
    ratio = (u16)pokemonGetStatus(NULL, (u16)pokemonGetStatus(obj, 0, 0x6E, 0), 0x13, 0);
    sex = pokemonGetSexKotei(obj);
    if (sex < 0) {
        if (ratio > (rnd & 0xFF)) {
            sex = 1;
        } else {
            sex = 0;
        }
    }
    return sex;
}

inline u32 pokemonCheckRare(u8* obj)
{
    u32 id;
    u32 rnd;

    if (obj == NULL) {
        return 0;
    }
    id = pokemonGetStatus(obj, 0, 0x75, 0);
    rnd = pokemonGetStatus(obj, 0, 0x6F, 0);
    return ((id >> 16) ^ (id & 0xFFFF) ^ (rnd >> 16) ^ (rnd & 0xFFFF)) < 8;
}

/*
 * Level reached with obj's experience total: the last level of the
 * species' growth table whose threshold the total meets. Retail routes its
 * result through r0 into the caller's u8 cast (the same return-temp shape
 * as the pokemonGetSex and pokemonCheckRare expansions beside it), and no
 * out-of-line copy exists, so it is a static TU helper.
 */
static inline u8 pokemonGetLevelFromExp(u8* obj)
{
    s32 level;
    u8 grow;
    u32 exp;
    void* growData;

    grow = (u8)pokemonGetStatus(NULL, (u16)pokemonGetStatus(obj, 0, 0x6E, 0), 0x11, 0);
    exp = pokemonGetStatus(obj, 0, 0x79, 0);
    growData = pokemonGrowDataBiosGetPtr(grow);
    if (growData == NULL) {
        return 0;
    }
    for (level = 1; level < 101; level++) {
        if (pokemonGrowDataBiosGetExp(growData, (u8)level) > exp) {
            break;
        }
    }
    return level - 1;
}

u32 pokemonGetStatus(u8* obj, u32 id, u32 selector, u32 d)
{
    if ((u16)selector == 0 || (u16)selector >= 0x124) {
        return 0;
    }

    if ((u16)selector < 0x6D) {
        obj = (u8*)pokemonDataBiosGetPtr(id);
        if (obj == NULL) {
            return 0;
        }
    } else {
        if (obj == NULL) {
            return 0;
        }
    }

    switch ((u16)selector) {
    case 0x01: return pokemonDataBiosGetName(obj);
    case 0x03: return (u32)(u16)pokemonDataBiosGetBasisMaxHp(obj);
    case 0x04: return (u32)(u16)pokemonDataBiosGetBasisPhyAtk(obj);
    case 0x05: return (u32)(u16)pokemonDataBiosGetBasisPhyDef(obj);
    case 0x06: return (u32)(u16)pokemonDataBiosGetBasisSpeAtk(obj);
    case 0x07: return (u32)(u16)pokemonDataBiosGetBasisSpeDef(obj);
    case 0x08: return (u32)(u16)pokemonDataBiosGetBasisNimbleness(obj);
    case 0x0A: return (u32)(u16)pokemonDataBiosGetGiveMaxHpEffort(obj);
    case 0x0B: return (u32)(u16)pokemonDataBiosGetGivePhyAtkEffort(obj);
    case 0x0C: return (u32)(u16)pokemonDataBiosGetGivePhyDefEffort(obj);
    case 0x0D: return (u32)(u16)pokemonDataBiosGetGiveSpeAtkEffort(obj);
    case 0x0E: return (u32)(u16)pokemonDataBiosGetGiveSpeDefEffort(obj);
    case 0x0F: return (u32)(u16)pokemonDataBiosGetGiveNimblenessEffort(obj);
    case 0x10: return (u32)(u16)pokemonDataBiosGetGiveExp(obj);
    case 0x11: return (u32)(u8)pokemonDataBiosGetGrowDataId(obj);
    case 0x12: return (u32)(u8)pokemonDataBiosGetGet(obj);
    case 0x13: return (u32)(u8)pokemonDataBiosGetSexRatio(obj);
    case 0x14: return (u32)(u16)pokemonDataBiosGetInitFriend(obj);
    case 0x15: return (u32)(u16)pokemonDataBiosGetItemDataId(obj, d);
    case 0x16: return (u32)(u8)pokemonDataBiosGetZokuseiDataId(obj, d);
    case 0x17: return (u32)(u8)pokemonDataBiosGetTokuseiDataId(obj, d);
    case 0x19: return (u32)(u8)pokemonDataBiosGetSinkaKind(obj, d);
    case 0x1A: return (u32)(u16)pokemonDataBiosGetSinkaBuff(obj, d);
    case 0x1B: return (u32)(u16)pokemonDataBiosGetSinkaPokemonDataId(obj, d);
    case 0x1D: return (u32)(u8)pokemonDataBiosGetGetWazaLevel(obj, d);
    case 0x1E: return (u32)(u16)pokemonDataBiosGetGetWazaDataId(obj, d);
    case 0x20: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x00);
    case 0x21: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x01);
    case 0x22: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x02);
    case 0x23: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x03);
    case 0x24: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x04);
    case 0x25: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x05);
    case 0x26: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x06);
    case 0x27: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x07);
    case 0x28: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x08);
    case 0x29: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x09);
    case 0x2A: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x0A);
    case 0x2B: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x0B);
    case 0x2C: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x0C);
    case 0x2D: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x0D);
    case 0x2E: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x0E);
    case 0x2F: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x0F);
    case 0x30: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x10);
    case 0x31: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x11);
    case 0x32: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x12);
    case 0x33: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x13);
    case 0x34: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x14);
    case 0x35: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x15);
    case 0x36: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x16);
    case 0x37: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x17);
    case 0x38: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x18);
    case 0x39: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x1A); /* 0x39 maps to slot 0x1A, skips 0x19 */
    case 0x3A: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x1B);
    case 0x3B: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x1C);
    case 0x3C: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x1D);
    case 0x3D: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x1E);
    case 0x3E: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x1F);
    case 0x3F: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x20);
    case 0x40: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x21);
    case 0x41: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x22);
    case 0x42: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x23);
    case 0x43: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x24);
    case 0x44: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x25);
    case 0x45: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x26);
    case 0x46: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x27);
    case 0x47: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x28);
    case 0x48: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x29);
    case 0x49: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x2A);
    case 0x4A: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x2B);
    case 0x4B: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x2C);
    case 0x4C: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x2D);
    case 0x4D: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x2E);
    case 0x4E: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x2F);
    case 0x4F: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x30);
    case 0x50: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x31);
    case 0x51: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x32);
    case 0x52: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x33);
    case 0x53: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x34);
    case 0x54: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x35);
    case 0x55: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x36);
    case 0x56: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x37);
    case 0x57: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x38);
    case 0x58: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x39);
    case 0x59: return (u32)(u8)pokemonDataBiosGetWazaMcn(obj, 0x3A);
    case 0x5A: return pokemonDataBiosGetPokebodyId(obj, d);
    case 0x5B: return pokemonDataBiosGetStatusFaceMenuSpriteId(obj, d);
    case 0x5C: return (u32)(u8)pokemonDataBiosGetColor(obj, d);
    case 0x5D: return pokemonDataBiosGetTypeName(obj);
    case 0x5E: return (u32)(u16)pokemonDataBiosGetHeight(obj);
    case 0x5F: return (u32)(u16)pokemonDataBiosGetWeight(obj);
    case 0x60: return pokemonDataBiosGetDoc(obj);
    case 0x61: return (u32)(u16)pokemonDataBiosGetVoice(obj);
    case 0x62: return pokemonDataBiosGetMitaFlag(obj);
    case 0x63: return pokemonDataBiosGetTukamaetaFlag(obj);
    case 0x64: return (u32)(u16)pokemonDataBiosGetNumZukan(obj);
    case 0x65: return (u32)(u16)pokemonDataBiosGetNumPokemon(obj);
    case 0x66: return pokemonDataBiosGetPkxDataId(obj);
    case 0x68: return (u32)(u16)pokemonDataBiosGetKowaza(obj, d);
    case 0x69: return (u32)(u8)fn_8011E048(obj, d);
    case 0x6A: return (u32)(u16)fn_8011E030(obj);
    case 0x6B: return (u32)(u8)fn_8011E018(obj);
    case 0x6C: return (u32)(u8)fn_8011E000(obj);
    case 0x6E: return (u32)(u16)pokemonBiosGetPokemonDataId(obj);
    case 0x6F: return pokemonBiosGetRnd(obj);
    case 0x70: return pokemonBiosGetAttest(obj);
    case 0x71: return (u32)(u16)pokemonBiosGetCatchFloorId(obj);
    case 0x72: return (u32)(u8)pokemonBiosGetCatchLevel(obj);
    case 0x73: return (u32)(u8)pokemonBiosGetCatchBallId(obj);
    case 0x74: return (u32)(u8)pokemonBiosGetCatchTrainerSex(obj);
    case 0x75: return pokemonBiosGetCatchTrainerRnd(obj);
    case 0x76: return pokemonBiosGetCatchTrainerNamePtr(obj);
    case 0x77: return pokemonBiosGetNicknamePtr(obj);
    case 0x78: return pokemonBiosGetNicknameOrgPtr(obj);
    case 0x79: return pokemonBiosGetExp(obj);
    case 0x7A: return (u32)(u8)pokemonBiosGetLevel(obj);

    case 0x7B: { /* fainted: current HP (0x83) is zero */
        u16 hp = (u16)pokemonGetStatus(obj, 0, 0x83, 0);
        return (u32)(u8)(hp == 0 ? 1 : 0);
    }

    case 0x7C: return pokemonBiosGetJoutaiPtr(obj, d);
    case 0x7D: return pokemonBiosGetConditionAmari(obj);
    case 0x7F: return (u32)(u16)pokemonBiosGetPokemonWazaDataId(obj, d);
    case 0x80: return (u32)(u8)pokemonBiosGetPokemonWazaPp(obj, d);
    case 0x81: return (u32)(u8)pokemonBiosGetPokemonWazaPpCount(obj, d);
    case 0x82: return (u32)(u16)pokemonBiosGetItemDataId(obj);
    case 0x83: return (u32)(u16)pokemonBiosGetHp(obj);
    case 0x87: return (u32)(u16)pokemonBiosGetMaxHp(obj);
    case 0x88: return (u32)(u16)pokemonBiosGetPhyAtk(obj);
    case 0x89: return (u32)(u16)pokemonBiosGetPhyDef(obj);
    case 0x8A: return (u32)(u16)pokemonBiosGetSpeAtk(obj);
    case 0x8B: return (u32)(u16)pokemonBiosGetSpeDef(obj);
    case 0x8C: return (u32)(u16)pokemonBiosGetNimbleness(obj);
    case 0x8D: return (u32)(u16)pokemonBiosGetMaxHpEffort(obj);
    case 0x8E: return (u32)(u16)pokemonBiosGetPhyAtkEffort(obj);
    case 0x8F: return (u32)(u16)pokemonBiosGetPhyDefEffort(obj);
    case 0x90: return (u32)(u16)pokemonBiosGetSpeAtkEffort(obj);
    case 0x91: return (u32)(u16)pokemonBiosGetSpeDefEffort(obj);
    case 0x92: return (u32)(u16)pokemonBiosGetNimblenessEffort(obj);
    case 0x93: return (u32)(u16)pokemonBiosGetMaxHpRnd(obj);
    case 0x94: return (u32)(u16)pokemonBiosGetPhyAtkRnd(obj);
    case 0x95: return (u32)(u16)pokemonBiosGetPhyDefRnd(obj);
    case 0x96: return (u32)(u16)pokemonBiosGetSpeAtkRnd(obj);
    case 0x97: return (u32)(u16)pokemonBiosGetSpeDefRnd(obj);
    case 0x98: return (u32)(u16)pokemonBiosGetNimblenessRnd(obj);
    case 0x99: return (u32)(u16)pokemonBiosGetFriend(obj);
    case 0x9C: return (u32)(u8)pokemonBiosGetStyle(obj);
    case 0x9D: return (u32)(u8)pokemonBiosGetBeautiful(obj);
    case 0x9E: return (u32)(u8)pokemonBiosGetCute(obj);
    case 0x9F: return (u32)(u8)pokemonBiosGetClever(obj);
    case 0xA0: return (u32)(u8)pokemonBiosGetStrong(obj);
    case 0xA1: return (u32)(u8)pokemonBiosGetFur(obj);
    case 0xA3: return (u32)(u8)pokemonBiosGetChampRibbon(obj);
    case 0xA4: return (u32)(u8)pokemonBiosGetWinningRibbon(obj);
    case 0xA5: return (u32)(u8)pokemonBiosGetVictoryRibbon(obj);
    case 0xA6: return (u32)(u8)pokemonBiosGetBromideRibbon(obj);
    case 0xA7: return (u32)(u8)pokemonBiosGetGanbaRibbon(obj);
    case 0xA8: return (u32)(u8)pokemonBiosGetMarineRibbon(obj);
    case 0xA9: return (u32)(u8)pokemonBiosGetLandRibbon(obj);
    case 0xAA: return (u32)(u8)pokemonBiosGetSkyRibbon(obj);
    case 0xAB: return (u32)(u8)pokemonBiosGetCountryRibbon(obj);
    case 0xAC: return (u32)(u8)pokemonBiosGetNationalRibbon(obj);
    case 0xAD: return (u32)(u8)pokemonBiosGetEarthRibbon(obj);
    case 0xAE: return (u32)(u8)pokemonBiosGetWorldRibbon(obj);
    case 0xAF: return (u32)(u8)pokemonBiosGetAmariRibbon(obj);
    case 0xB0: return (u32)(u8)pokemonBiosGetStyleMedal(obj);
    case 0xB1: return (u32)(u8)pokemonBiosGetBeautifulMedal(obj);
    case 0xB2: return (u32)(u8)pokemonBiosGetCuteMedal(obj);
    case 0xB3: return (u32)(u8)pokemonBiosGetCleverMedal(obj);
    case 0xB4: return (u32)(u8)pokemonBiosGetStrongMedal(obj);
    case 0xB5: return (u32)(u8)pokemonBiosGetPokerus(obj);
    case 0xB6: return (u32)(u8)pokemonBiosGetTamagoFlag(obj);
    case 0xB7: return (u32)(u8)pokemonBiosGetTokuseiFlag(obj);
    case 0xB8: return (u32)(u8)pokemonBiosGetFuseiFlag(obj);
    case 0xB9: return (u32)(u8)pokemonBiosGetFlagAmari(obj);

    case 0xBA: return pokemonGetSex(obj);

    case 0xBB: return (u32)(u8)pokemonBiosGetPcboxMark(obj);
    case 0xBC: return (u32)(u8)pokemonBiosGetMailId(obj);
    case 0xBD: return (u32)(u16)pokemonBiosGetPara1Amari(obj);
    case 0xBE: return (u32)(u16)pokemonBiosGetAmari(obj);

    case 0xBF: { /* nature = personality % 25 */
        u32 pv = pokemonGetStatus(obj, 0, 0x6F, 0);
        return (u32)(u8)(pv % 25);
    }

    case 0xC0: return pokemonGetLevelFromExp(obj);

    case 0xC1: return (u8)pokemonCheckRare(obj);

    /* Retail's table sends 0xC9 to the first of these bodies and 0xC2 /
     * 0xC8 to the last two (pokemonSetStatus lists 0xC9 first as well). */
    case 0xC9: return (u32)(u16)pokemonBiosGetFightTrainerPokemonDataId(obj);
    case 0xC3: return (u32)(u16)pokemonBiosGetDarkpokemonDataId(obj);
    case 0xC4: return pokemonBiosGetInitDp(obj);
    case 0xC5: return pokemonBiosGetDp(obj);
    case 0xC6: return pokemonBiosGetPoolExp(obj);
    case 0xC7: return (u32)(u16)pokemonBiosGetPoolFriend(obj);
    case 0xC2: return (u32)(u8)pokemonBiosGetDarkFlag(obj);
    case 0xC8: return pokemonBiosGetDarkJoutaiPtr(obj, d);
    case 0xCB: return fightPokemonBiosGetMotoPokemonPtr(obj);
    case 0xCC: return fightPokemonBiosGetPokemonBuffPtr(obj);
    case 0xCD: return fightPokemonBiosGetFightJoutaiPtr(obj, d);
    case 0xCE: return (u32)(s32)(s16)fightPokemonBiosGetEntryId(obj); /* extsh */
    case 0xCF: return (u32)(u8)fightPokemonBiosGetCatchEntryFlag(obj);
    case 0xD0: return (u32)(u8)fightPokemonBiosGetLevelUpFlag(obj);
    case 0xD1: return (u32)(u8)fightPokemonBiosGetDarkOutFlag(obj);
    case 0xD2: return (u32)(u8)fightPokemonBiosGetHokakuFlag(obj);
    case 0xD5: return fightOutPokemonBiosGetMotoFightPokemonPtr(obj);
    case 0xD6: return fightOutPokemonBiosGetFightPokemonPtr(obj);
    case 0xD7: return fightOutPokemonBiosGetFightPokemonHensinBuffPtr(obj);
    case 0xD8: return fightOutPokemonBiosGetFightoutJoutaiPtr(obj, d);
    case 0xD9: return fightOutPokemonBiosGetFightWazaPtr(obj);
    case 0xDA: return (u32)(u16)fightOutPokemonGetUseWazaDataId(obj);
    case 0xDB: return (u32)(u16)fightOutPokemonGetMotoWazaDataId(obj);

    case 0xDC: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return wazaGetStatus(ctx, 0, 0x2C, 0);
    }
    case 0xDD: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return wazaGetStatus(ctx, 0, 0x2B, 0);
    }
    case 0xDE: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return (u32)(u8)fightWazaIsJoutaiDataId(ctx, d);
    }
    case 0xE0: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return (u32)(u8)fightWazaIsHit(ctx);
    }
    case 0xE1: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return wazaGetStatus(ctx, 0, 0x2D, 0);
    }
    case 0xE2: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return wazaGetStatus(ctx, 0, 0x2F, 0);
    }
    case 0xE3: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return wazaGetStatus(ctx, 0, 0x29, 0);
    }
    case 0xE4: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return wazaGetStatus(ctx, 0, 0x2E, 0);
    }

    case 0xE5: return fightOutPokemonBiosGetFightItemPtr(obj);
    case 0xE6: return (u32)(u8)fightOutPokemonBiosGetAbicntPhyAtk(obj);
    case 0xE7: return (u32)(u8)fightOutPokemonBiosGetAbicntPhyDef(obj);
    case 0xE8: return (u32)(u8)fightOutPokemonBiosGetAbicntSpeAtk(obj);
    case 0xE9: return (u32)(u8)fightOutPokemonBiosGetAbicntSpeDef(obj);
    case 0xEA: return (u32)(u8)fightOutPokemonBiosGetAbicntNimbleness(obj);
    case 0xEB: return (u32)(u8)fightOutPokemonBiosGetAbicntAverage(obj);
    case 0xEC: return (u32)(u8)fightOutPokemonBiosGetAbicntAvoid(obj);
    case 0xED: return (u32)(u16)fightOutPokemonBiosGetFightoutTurnCount(obj);
    case 0xEE: return fightOutPokemonBiosGetSequencePtr(obj);
    case 0xEF: return (u32)(u16)fightOutPokemonBiosGetSketchWazaDataId(obj);
    case 0xF0: return (u32)(u16)fightOutPokemonBiosGetLastSelectWazaDataId(obj);
    case 0xF1: return (u32)(u16)fightOutPokemonBiosGetLastUseWazaDataId(obj);
    case 0xF2: return (u32)(u16)fightOutPokemonBiosGetLastReceiveWazaTargetDataId(obj);
    case 0xF3: return (u32)(u16)fightOutPokemonBiosGetHitWazaDataId(obj);
    case 0xF4: return (u32)(u16)fightOutPokemonBiosGetHitWazaZokuseiDataId(obj);
    case 0xF5: return (u32)(s32)(s16)fightOutPokemonBiosGetGamanDamageValue(obj); /* extsh */
    case 0xF6: return (u32)(u16)fightOutPokemonBiosGetGamanDamageTargetId(obj);
    case 0xF7: return (u32)(u16)fightOutPokemonBiosGetOumuWazaDataId(obj);
    case 0xF8: return fightOutPokemonBiosGetKeepFightWazaPtr(obj);
    case 0xF9: return (u32)(u8)fightOutPokemonBiosGetNamakeFlag(obj);
    case 0xFA: return (u32)(u16)fightOutPokemonBiosGetUsedItemDataId(obj);
    case 0xFB: return (u32)(u16)fightOutPokemonBiosGetStockItemDataId(obj);
    case 0xFC: return (u32)(u8)fightOutPokemonBiosGetSuccessCnt(obj);
    case 0xFD: return (u32)(s32)(s16)fightOutPokemonBiosGetMeetEnemyFightPokemonEntryId(obj, (u8)d); /* extsh */
    case 0xFE: return fightOutPokemonBiosGetFightActionBuffPtr(obj);
    case 0xFF: return (u32)(u16)fightOutPokemonBiosGetZokuseiDataId(obj, (u8)d);
    case 0x100: return (u32)(u16)fightOutPokemonBiosGetTokuseiDataId(obj);
    case 0x101: return fightOutPokemonBiosGetWazaMenuCurPtr(obj);
    case 0x102: return (u32)(s32)(s16)fightOutPokemonBiosGetDamageAtkValue(obj); /* extsh */
    case 0x103: return (u32)(u16)fightOutPokemonBiosGetDamageAtkTargetId(obj);
    case 0x104: return (u32)(s32)(s16)fightOutPokemonBiosGetDamageSpeValue(obj); /* extsh */
    case 0x105: return (u32)(u16)fightOutPokemonBiosGetDamageSpeTargetId(obj);
    case 0x106: return (u32)(u8)fightOutPokemonBiosGetMahiNoAttackFlag(obj);
    case 0x107: return (u32)(u8)fightOutPokemonBiosGetKonranMyselfAttackFlag(obj);
    case 0x108: return (u32)(u8)fightOutPokemonBiosGetOutWazaKoukanaiFlag(obj);
    case 0x109: return (u32)(u8)fightOutPokemonBiosGetTameWazaFlag(obj);
    case 0x10A: return (u32)(u8)fightOutPokemonBiosGetItemNigeruFlag(obj);
    case 0x10B: return (u32)(u8)fightOutPokemonBiosGetHuuinNoAttackFlag(obj);
    case 0x10C: return (u32)(u8)fightOutPokemonBiosGetMeroMeroNoAttackFlag(obj);
    case 0x10D: return (u32)(u8)fightOutPokemonBiosGetKanashibariNoAttackFlag(obj);
    case 0x10E: return (u32)(u8)fightOutPokemonBiosGetChouhatsuNoAttackFlag(obj);
    case 0x10F: return (u32)(u8)fightOutPokemonBiosGetIchamonNoAttackFlag(obj);
    case 0x110: return (u32)(u8)fightOutPokemonBiosGetHirumuNoAttackFlag(obj);
    case 0x111: return (u32)(u8)fightOutPokemonBiosGetPassPpdecFlag(obj);
    case 0x112: return (u32)(u8)fightOutPokemonBiosGetFightActionFlag(obj);
    case 0x113: return (u32)(u8)fightOutPokemonBiosGetDoClearbodyFlag(obj);
    case 0x114: return (u32)(u8)fightOutPokemonBiosGetReceivesWazaHiraishinFlag(obj);
    case 0x115: return (u32)(u8)fightOutPokemonBiosGetVanishoffFlag(obj);
    case 0x116: return (u32)(u8)fightOutPokemonBiosGetDoIkakuFlag(obj);
    case 0x117: return (u32)(u8)fightOutPokemonBiosGetDoTraceFlag(obj);
    case 0x118: return (u32)(u8)fightOutPokemonBiosGetNoPressureFlag(obj);
    case 0x119: return (u32)(u8)fightOutPokemonBiosGetIrekaetaFlag(obj);
    case 0x11A: return (u32)(u8)fightOutPokemonBiosGetItemKoraetaFlag(obj);
    case 0x11B: return fightOutPokemonBiosGetKaigaraDamageValue(obj);
    case 0x11C: return (u32)(s32)(s16)fightOutPokemonBiosGetMyselfDamageAtkValue(obj); /* extsh */
    case 0x11D: return (u32)(u16)fightOutPokemonBiosGetMyselfDamageAtkTargetId(obj);
    case 0x11E: return (u32)(s32)(s16)fightOutPokemonBiosGetMyselfDamageSpeValue(obj); /* extsh */
    case 0x11F: return (u32)(u16)fightOutPokemonBiosGetMyselfDamageSpeTargetId(obj);
    case 0x120: return (u32)(u8)fightOutPokemonBiosGetKizetuFlag(obj);
    case 0x121: return (u32)(s32)(s16)fightOutPokemonBiosGetIrekaeTargetEntryId(obj); /* extsh */
    case 0x122: return fightOutPokemonBiosGetFightOutPokemonEnemyPtr(obj, d);
    case 0x123: return (u32)(u8)fightOutPokemonCheckFightOut(obj);

    default:
        return 0;
    }
}
