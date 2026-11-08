/**
 * @file msgctrl_candidate_80131BF8.c
 * @brief msgctrl.c carve, 0x80131BF8 - 0x80131F9C: the six side-name
 *        control codes, _msgctrlSideName and msgctrlClientnowork.
 *
 * Built with the TU's -O4,p and the peephole pass off (one unit-wide flag in
 * configure.py, as for the other msgctrl.c carves).
 */
#include "dolphin/types.h"

extern u32 lbl_8047AE10;
extern u32 lbl_8047AE14;
extern u32 lbl_8047AE18;
extern u32 lbl_8047AE1C;
extern u32 lbl_8047AE44;
extern u32 lbl_8047AE48;
extern u32 lbl_8047AE4C;

extern void* GSmsgGetGSchar(u32 id);
extern void msgctrlSetValue(u32 id, u32 value);
extern u8 fightTargetIsHostSide(u32 pokemon, u16 status);
extern void* fightTargetGetPtrAsNowFightType(u32 target, u32 pokemon);
extern u8 fightFloorIsKind10(u32 arg);
extern void* fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32 arg, u32 pokemon);
extern u32 fightFloorGetStatus(u32 a, u32 b, u32 id, u32 d);
extern void* fightSideGetValidFightTrainerPtr(void* side, u16 index);
extern u32 fightTrainerGetNamePtr(void* trainer);
extern u8 fightTrainerCheckDoFight(void* trainer);
extern u32 fightOutPokemonGetNicknamePtr(u32 pokemon);

void* _msgctrlSideName__FP15FightOutPokemonUc(u32 pokemon, u8 kind);

void msgctrlSideDefenceNameno(void)
{
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE4C, 2);
}

void msgctrlSideDefenceNamewo(void)
{
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE48, 1);
}

void msgctrlSideDefenceNameha(void)
{
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE44, 0);
}

void msgctrlSideAttackNameno(void)
{
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE1C, 2);
}

void msgctrlSideAttackNamewo(void)
{
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE18, 1);
}

void msgctrlSideAttackNameha(void)
{
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE14, 0);
}

/*
 * Put the side's trainer name(s) into message values 0x4D/0x57, then fetch
 * the side-name string for this message kind (0, 1, other).
 */
void* _msgctrlSideName__FP15FightOutPokemonUc(u32 pokemon, u8 kind)
{
    u16 status;
    void* side;
    void* ownTrainer;
    void* trainer;
    u16 count;
    u16 i;

    status = fightFloorGetStatus(0, 0, 0x14, 0);
    side = fightTargetGetPtrAsNowFightType(2, pokemon);
    ownTrainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, pokemon);
    count = 0;
    for (i = 0; i < 2; i++) {
        trainer = fightSideGetValidFightTrainerPtr(side, i);
        if (trainer != NULL && fightTrainerCheckDoFight(trainer) != 0) {
            if (count == 0) {
                msgctrlSetValue(0x4D, fightTrainerGetNamePtr(trainer));
            } else if (count == 1) {
                msgctrlSetValue(0x57, fightTrainerGetNamePtr(trainer));
            }
            count++;
        }
    }

    if (fightFloorIsKind10(0) == 1 && ownTrainer != NULL) {
        if (count <= 1) {
            msgctrlSetValue(0x4D, fightTrainerGetNamePtr(ownTrainer));
            if (kind == 0) {
                return GSmsgGetGSchar(0x7722);
            } else if (kind == 1) {
                return GSmsgGetGSchar(0x7725);
            } else {
                return GSmsgGetGSchar(0x7727);
            }
        }
        if (kind == 0) {
            return GSmsgGetGSchar(0x7724);
        } else if (kind == 1) {
            return GSmsgGetGSchar(0x7726);
        } else {
            return GSmsgGetGSchar(0x7728);
        }
    }

    if (fightTargetIsHostSide(pokemon, status) == 1) {
        if (kind == 0) {
            return GSmsgGetGSchar(0x768A);
        } else if (kind == 1) {
            return GSmsgGetGSchar(0x768C);
        } else {
            return GSmsgGetGSchar(0x7688);
        }
    }
    if (kind == 0) {
        return GSmsgGetGSchar(0x7689);
    } else if (kind == 1) {
        return GSmsgGetGSchar(0x768B);
    } else {
        return GSmsgGetGSchar(0x7687);
    }
}

/* The trainer-or-nickname body msgctrl_candidate_80131FF4.c's *Mons getters
 * share, for the client Pokemon. */
static inline void* msgctrlOutPokemonName(u32 pokemon)
{
    void* trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, pokemon);

    if (fightFloorIsKind10(0) == 1 && trainer != NULL) {
        msgctrlSetValue(0x4D, fightTrainerGetNamePtr(trainer));
        msgctrlSetValue(0x57, fightOutPokemonGetNicknamePtr(pokemon));
        return GSmsgGetGSchar(0x7721);
    }
    return (void*)fightOutPokemonGetNicknamePtr(pokemon);
}

void* msgctrlClientnowork(void)
{
    return msgctrlOutPokemonName(lbl_8047AE10);
}
