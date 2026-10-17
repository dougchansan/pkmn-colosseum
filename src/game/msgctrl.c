/**
 * @file msgctrl.c
 * @brief Decompiled functions.
 *
 * Address range: 0x80131588 - 0x80132C6C
 *
 * Split out of the former game/effect/effect_util.c CodeCandidate
 * bucket (0x8013151C - 0x80137114); see effect_util_types.h for
 * shared cross-TU declarations.
 */

#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

/*
 * 0x80131588 - 0x80131690: the unit game/msgctrl.c links these four
 * functions. The TU was built at -O4,p with the peephole pass off (one
 * unit-wide flag in configure.py; see msgctrl_exact_80132A38.c). The rest of
 * the TU is linked through its carves (msgctrl_exact_*.c,
 * msgctrl_candidate_80131714.c, msgctrl_r49_80131A34_suffix.c); the
 * candidate chunks still compile it whole by defining MSGCTRL_WHOLE_TU.
 */

/* 0x80131588 | 0x64 */
u32 msgctrlSndWait(EffectUtilCommandObj* obj) {
    if (obj->activeFlag == 0) {
        u16 handle = lbl_8047AEA4;
        if (handle == 0) {
            return 0;
        }
        if (GSsndGetStatus(handle) == 2) {
            obj->stream = obj->stream - 3;
        }
    }
    return 1;
}

/* 0x801315EC | 0x44 */
u32 msgctrlSndPlay(EffectUtilCommandObj* obj) {
    if (obj->activeFlag == 0) {
        u16 handle = lbl_8047AEA4;
        if (handle != 0) {
            std104_soundSetBackgroundMusic(handle, 0, 0xFF);
        }
    }
    return 0;
}

/* 0x80131630 | 0x30 -- read a signed byte from the stream into the baseline bias if flag set */
u32 msgctrlBaseLineBias(EffectUtilCommandObj* obj) {
    u8* stream;
    if (obj->activeFlag != 0) {
        stream = obj->stream;
        obj->field_43 = (s8)*stream;
    }
    stream = obj->stream;
    obj->stream = stream + 1;
    return 0;
}

/* 0x80131660 | 0x30 -- read a signed byte from the stream into the line spacing if flag set */
u32 msgctrlLineSpace(EffectUtilCommandObj* obj) {
    u8* stream;
    if (obj->activeFlag != 0) {
        stream = obj->stream;
        obj->field_42 = (s8)*stream;
    }
    stream = obj->stream;
    obj->stream = stream + 1;
    return 0;
}

#if defined(MSGCTRL_WHOLE_TU)
/* 0x80131690 | 16 bytes | set_field_return */
u32 msgctrlIndentOff(EffectUtilCommandObj* obj) {
    obj->field_41 = 1;
    return 0;
}


/* 0x801316A0 | 0x8 | sda_getter */
u16 msgctrlNpc(void) { return lbl_8047AEA2; }


/* 0x801316A8 | 0x28 -- calls fightTrainerKindDataBiosGetPtr with lbl_8047AEA0 then fightTrainerKindDataBiosGetPrefixName */
/* renamed symbols referenced by asm incs (symbolmap port) */
/* Forward declarations for self-referencing asm blocks */
/* _msgctrlSideName__FP15FightOutPokemonUc already declared at top with typed params */





#if 0
asm void msgctrlTribe(void) {
#include "src/game/effect/effect_util_fn_801316A8.inc"
}
#else
void msgctrlTribe(void) {
    fightTrainerKindDataBiosGetPtr(lbl_8047AEA0);
    fightTrainerKindDataBiosGetPrefixName();
}
#endif


/* 0x801316D0 | 0x8 | sda_getter */
u32 msgctrlString2(void) { return lbl_8047AE8C; }


/* 0x801316D8 | 0x8 | sda_getter */
u32 msgctrlMenuMsgID2(void) { return lbl_8047AE9C; }


/* 0x801316E0 | 0x8 | sda_getter */
u32 msgctrlMenuMsgID(void) { return lbl_8047AE98; }


/* 0x801316E8 | 0x2C -- read byte from stream, store to obj+0x03 if flag clear */
u32 msgctrlTalkSE(EffectUtilCommandObj* obj) {
    u8* stream;
    if (obj->activeFlag == 0) {
        stream = obj->stream;
        obj->field_03 = *stream;
    }
    stream = obj->stream;
    obj->stream = stream + 1;
    return 0;
}

#if 0
asm void msgctrlAlign(void) {
#include "src/game/effect/effect_util_fn_80131714.inc"
}
#else
s32 msgctrlAlign(EffectUtilCommandObj* obj) {
    extern void GSmsgAdjustAlign(void*);
    u8* stream;
    if (obj->activeFlag != 0) {
        stream = obj->stream;
        obj->alignMode = stream[0];
        GSmsgAdjustAlign(obj);
    }
    stream = obj->stream;
    obj->stream = stream + 1;
    return 0;
}
#endif


/* 0x80131768 | 0x2C -- read byte from stream, store to obj+0x02 if flag set */
u32 msgctrlShadow(EffectUtilCommandObj* obj) {
    u8* stream;
    if (obj->activeFlag != 0) {
        stream = obj->stream;
        obj->field_02 = *stream;
    }
    stream = obj->stream;
    obj->stream = stream + 1;
    return 0;
}


/* 0x80131794 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80426FF0, 0x10, lbl_8047AE94, 4) */
#if 0
asm void msgctrlMenuMoney(void) {
#include "src/game/effect/effect_util_fn_80131794.inc"
}
#else
void msgctrlMenuMoney(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80426FF0, 0x10, lbl_8047AE94, 4);
}
#endif


/* 0x801317C8 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80427010, 0x10, lbl_8047AE68, 5) */
#if 0
asm void msgctrlMenuFullDigit(void) {
#include "src/game/effect/effect_util_fn_801317C8.inc"
}
#else
void msgctrlMenuFullDigit(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427010, 0x10, lbl_8047AE68, 5);
}
#endif


/* 0x801317FC | 0x28 -- calls pokemonDataBiosGetPtr(lbl_8047AE90) then pokemonDataBiosGetName */
#if 0
asm void msgctrlPokemonID(void) {
#include "src/game/effect/effect_util_fn_801317FC.inc"
}
#else
void msgctrlPokemonID(void) {
    pokemonDataBiosGetPtr(lbl_8047AE90);
    pokemonDataBiosGetName();
}
#endif


/* 0x80131824 | 0x8 | sda_getter */
u32 msgctrlString(void) { return lbl_8047AE88; }


/* 0x8013182C | 0x208 */
#if 0
asm void msgctrlTime(void) {
#include "src/game/effect/effect_util_fn_8013182C.inc"
}
#else
void msgctrlTime(void) {
    u32 seconds = lbl_8047AE84 / 3600;
    u32 minutes = (lbl_8047AE84 - seconds * 3600) / 60;
    u32 outIndex = 0;
    u16* glyph;

    if (seconds >= 100) {
        glyph = (u16*)_msgctrlMakeDigit__FPUslUll(lbl_80427030, 0x10, seconds / 100, 0);
        lbl_8047AEA8 = (u32)glyph;
        ((u16*)lbl_80427050)[outIndex++] = glyph[0];
        seconds -= (seconds / 100) * 100;
    }

    glyph = (u16*)_msgctrlMakeDigit__FPUslUll(lbl_80427030, 0x10, seconds / 10, 0);
    lbl_8047AEA8 = (u32)glyph;
    ((u16*)lbl_80427050)[outIndex++] = glyph[0];

    glyph = (u16*)_msgctrlMakeDigit__FPUslUll(lbl_80427030, 0x10, seconds - (seconds / 10) * 10, 0);
    lbl_8047AEA8 = (u32)glyph;
    ((u16*)lbl_80427050)[outIndex++] = glyph[0];

    ((u16*)lbl_80427050)[outIndex++] = 0x3A;

    glyph = (u16*)_msgctrlMakeDigit__FPUslUll(lbl_80427030, 0x10, minutes / 10, 0);
    lbl_8047AEA8 = (u32)glyph;
    ((u16*)lbl_80427050)[outIndex++] = glyph[0];

    glyph = (u16*)_msgctrlMakeDigit__FPUslUll(lbl_80427030, 0x10, minutes - (minutes / 10) * 10, 0);
    lbl_8047AEA8 = (u32)glyph;
    ((u16*)lbl_80427050)[outIndex++] = glyph[0];
    ((u16*)lbl_80427050)[outIndex] = 0;
}
#endif


/* 0x80131A34 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80427070, 0x10, lbl_8047AE80, 4) */
#if 0
asm void msgctrlMoney(void) {
#include "src/game/effect/effect_util_fn_80131A34.inc"
}
#else
void msgctrlMoney(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427070, 0x10, lbl_8047AE80, 4);
}
#endif


/* 0x80131A68 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80427090, 0x10, lbl_8047AE6C, 2) */
#if 0
asm void msgctrlMenuZDigit2(void) {
#include "src/game/effect/effect_util_fn_80131A68.inc"
}
#else
void msgctrlMenuZDigit2(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427090, 0x10, lbl_8047AE6C, 2);
}
#endif


/* 0x80131A9C | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_804270B0, 0x10, lbl_8047AE68, 2) */
#if 0
asm void msgctrlMenuZDigit(void) {
#include "src/game/effect/effect_util_fn_80131A9C.inc"
}
#else
void msgctrlMenuZDigit(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_804270B0, 0x10, lbl_8047AE68, 2);
}
#endif


/* 0x80131AD0 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_804270D0, 0x10, lbl_8047AE68, 3) */
#if 0
asm void msgctrlMenuHex2(void) {
#include "src/game/effect/effect_util_fn_80131AD0.inc"
}
#else
void msgctrlMenuHex2(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_804270D0, 0x10, lbl_8047AE68, 3);
}
#endif


/* 0x80131B04 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_804270F0, 0x10, lbl_8047AE68, 3) */
#if 0
asm void msgctrlMenuHex(void) {
#include "src/game/effect/effect_util_fn_80131B04.inc"
}
#else
void msgctrlMenuHex(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_804270F0, 0x10, lbl_8047AE68, 3);
}
#endif


/* 0x80131B38 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80427110, 0x10, lbl_8047AE6C, 1) */
#if 0
asm void msgctrlMenuUDigit2(void) {
#include "src/game/effect/effect_util_fn_80131B38.inc"
}
#else
void msgctrlMenuUDigit2(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427110, 0x10, lbl_8047AE6C, 1);
}
#endif


/* 0x80131B6C | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80427130, 0x10, lbl_8047AE68, 1) */
#if 0
asm void msgctrlMenuUDigit(void) {
#include "src/game/effect/effect_util_fn_80131B6C.inc"
}
#else
void msgctrlMenuUDigit(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427130, 0x10, lbl_8047AE68, 1);
}
#endif


/* 0x80131BA0 | 0x8 | return_const */
u32 msgctrlSeOn(void) { return 0; }


/* 0x80131BA8 | 0x8 | return_const */
u32 msgctrlSeOff(void) { return 0; }


/* 0x80131BB0 | 0x8 | sda_getter */
u32 msgctrlPasoName(void) { return lbl_8047AE40; }


/* 0x80131BB8 | 0x8 | sda_getter */
u32 msgctrlItemName(void) { return lbl_8047AE3C; }


/* 0x80131BC0 | 0x8 | sda_getter */
u32 msgctrlWazaName(void) { return lbl_8047AE38; }


/* 0x80131BC8 | 0x8 | sda_getter */
u32 msgctrlTrainerClientno(void) { return lbl_8047AE34; }


/* 0x80131BD0 | 0x8 | sda_getter */
u32 msgctrlTrainerEnename2(void) { return lbl_8047AE30; }


/* 0x80131BD8 | 0x8 | sda_getter */
u32 msgctrlTrainerEnename(void) { return lbl_8047AE2C; }


/* 0x80131BE0 | 0x8 | sda_getter */
u32 msgctrlTrainerLose(void) { return lbl_8047AE28; }


/* 0x80131BE8 | 0x8 | sda_getter */
u32 msgctrlTrainerName(void) { return lbl_8047AE24; }


/* 0x80131BF0 | 0x8 | sda_getter */
u32 msgctrlTrainerType(void) { return lbl_8047AE20; }


/* 0x80131BF8 | 0x28 -- _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE4C, 2) */
#if 0
asm void msgctrlSideDefenceNameno(void) {
#include "src/game/effect/effect_util_fn_80131BF8.inc"
}
#else
void msgctrlSideDefenceNameno(void) {
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE4C, 2);
}
#endif


/* 0x80131C20 | 0x28 -- _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE48, 1) */
#if 0
asm void msgctrlSideDefenceNamewo(void) {
#include "src/game/effect/effect_util_fn_80131C20.inc"
}
#else
void msgctrlSideDefenceNamewo(void) {
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE48, 1);
}
#endif


/* 0x80131C48 | 0x28 -- _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE44, 0) */
#if 0
asm void msgctrlSideDefenceNameha(void) {
#include "src/game/effect/effect_util_fn_80131C48.inc"
}
#else
void msgctrlSideDefenceNameha(void) {
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE44, 0);
}
#endif


/* 0x80131C70 | 0x28 -- _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE1C, 2) */
#if 0
asm void msgctrlSideAttackNameno(void) {
#include "src/game/effect/effect_util_fn_80131C70.inc"
}
#else
void msgctrlSideAttackNameno(void) {
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE1C, 2);
}
#endif


/* 0x80131C98 | 0x28 -- _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE18, 1) */
#if 0
asm void msgctrlSideAttackNamewo(void) {
#include "src/game/effect/effect_util_fn_80131C98.inc"
}
#else
void msgctrlSideAttackNamewo(void) {
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE18, 1);
}
#endif


/* 0x80131CC0 | 0x28 -- _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE14, 0) */
#if 0
asm void msgctrlSideAttackNameha(void) {
#include "src/game/effect/effect_util_fn_80131CC0.inc"
}
#else
void msgctrlSideAttackNameha(void) {
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE14, 0);
}
#endif


/* 0x80131CE8 | 0x21C
 * Put the side's trainer name(s) into message values 0x4D/0x57, then fetch
 * the side-name string for this message kind (0, 1, other).
 */
void _msgctrlSideName__FP15FightOutPokemonUc(u32 pokemon, u32 kind) {
    extern void* GSmsgGetGSchar(u32 id);
    extern void msgctrlSetValue(u32 id, u32 value);
    extern u8 fightTargetIsHostSide(u32 pokemon, u16 status);
    extern void* fightTargetGetPtrAsNowFightType(u32 target, u32 pokemon);
    extern u8 fn_801F18DC(u32 arg);
    extern void* fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32 arg, u32 pokemon);
    extern u32 fightFloorGetStatus(u32 a, u32 b, u32 id, u32 d);
    extern void* fightSideGetValidFightTrainerPtr(void* side, u16 index);
    extern u32 fightTrainerGetNamePtr(void* trainer);
    extern u8 fightTrainerCheckDoFight(void* trainer);
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

    if (fn_801F18DC(0) == 1 && ownTrainer != NULL) {
        if (count <= 1) {
            msgctrlSetValue(0x4D, fightTrainerGetNamePtr(ownTrainer));
            if ((u8)kind == 0) {
                GSmsgGetGSchar(0x7722);
                return;
            } else if ((u8)kind == 1) {
                GSmsgGetGSchar(0x7725);
                return;
            } else {
                GSmsgGetGSchar(0x7727);
                return;
            }
        }
        if ((u8)kind == 0) {
            GSmsgGetGSchar(0x7724);
            return;
        } else if ((u8)kind == 1) {
            GSmsgGetGSchar(0x7726);
            return;
        } else {
            GSmsgGetGSchar(0x7728);
            return;
        }
    }

    if (fightTargetIsHostSide(pokemon, status) == 1) {
        if ((u8)kind == 0) {
            GSmsgGetGSchar(0x768A);
            return;
        } else if ((u8)kind == 1) {
            GSmsgGetGSchar(0x768C);
            return;
        } else {
            GSmsgGetGSchar(0x7688);
            return;
        }
    }
    if ((u8)kind == 0) {
        GSmsgGetGSchar(0x7689);
        return;
    } else if ((u8)kind == 1) {
        GSmsgGetGSchar(0x768B);
        return;
    } else {
        GSmsgGetGSchar(0x7687);
        return;
    }
}


/* 0x80131F04 | 0x98 */
#if 0
asm void msgctrlClientnowork(void) {
#include "src/game/effect/effect_util_fn_80131F04.inc"
}
#else
void msgctrlClientnowork(void) {
    extern u32 lbl_8047AE10;
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fn_801F18DC(u32);
    extern u32 fightTrainerGetNamePtr(u32);
    extern u32 fightOutPokemonGetNicknamePtr(u32);
    extern void msgctrlSetValue(u32, u32);
    extern void GSmsgGetGSchar(u32);
    int new_var;
    u32 result;
    u32 val = lbl_8047AE10;
    result = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, val);
    new_var = 0;
    if ((((u8) ((new_var, fn_801F18DC(0)))) == 1) && (result != 0)) {
        msgctrlSetValue(0x4D, fightTrainerGetNamePtr(result));
        msgctrlSetValue(0x57, fightOutPokemonGetNicknamePtr(val));
        GSmsgGetGSchar(0x7721);
    } else {
        fightOutPokemonGetNicknamePtr(val);
    }
}
#endif


/* 0x80131F9C | 0x8 | sda_getter */
u32 msgctrlSpeabiNamet(void) { return lbl_8047AE0C; }


/* 0x80131FA4 | 0x8 | sda_getter */
u32 msgctrlSpeabiNamec(void) { return lbl_8047AE08; }


/* 0x80131FAC | 0x8 | sda_getter */
u32 msgctrlSpeabiNamed(void) { return lbl_8047AE04; }


/* 0x80131FB4 | 0x8 | sda_getter */
u32 msgctrlSpeabiNamea(void) { return lbl_8047AE00; }


/* 0x80131FBC | 0x8 | sda_getter */
u32 msgctrlEnemyTmons2(void) { return lbl_8047ADFC; }


/* 0x80131FC4 | 0x8 | sda_getter */
u32 msgctrlEnemyTmons(void) { return lbl_8047ADF8; }


/* 0x80131FCC | 0x8 | sda_getter */
u32 msgctrlEnemyMons2(void) { return lbl_8047ADF4; }


/* 0x80131FD4 | 0x8 | sda_getter */
u32 msgctrlEnemyMons(void) { return lbl_8047ADF0; }


/* 0x80131FDC | 0x8 | sda_getter */
u32 msgctrlMyMons2(void) { return lbl_8047ADEC; }


/* 0x80131FE4 | 0x8 | sda_getter */
u32 msgctrlMyMons(void) { return lbl_8047ADE8; }


/* 0x80131FEC | 0x8 | sda_getter */
u32 msgctrlMyName(void) { return lbl_8047ADE4; }


/* 0x80131FF4 | 0x98 */
#if 0
asm void msgctrlTsuikaMons(void) {
#include "src/game/effect/effect_util_fn_80131FF4.inc"
}
#else
void msgctrlTsuikaMons(void) {
    extern u32 lbl_8047ADE0;
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    u32 result;
    extern u32 fn_801F18DC(u32);
    extern u32 fightTrainerGetNamePtr(u32);
    extern u32 fightOutPokemonGetNicknamePtr(u32);
    u32 new_var;
    extern void msgctrlSetValue(u32, u32);
    extern void GSmsgGetGSchar(u32);
    u32 val = lbl_8047ADE0;
    result = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, val);
    new_var = val;
    if ((((u8) fn_801F18DC(0)) == 1) && (result != 0)) {
        msgctrlSetValue(0x4D, fightTrainerGetNamePtr(result));
        msgctrlSetValue(0x57, fightOutPokemonGetNicknamePtr(new_var));
        do { GSmsgGetGSchar(0x7721); } while (0);
    } else {
        fightOutPokemonGetNicknamePtr(val);
    }
}
#endif


/* 0x8013208C | 0x98 */
#if 0
asm void msgctrlClientMos(void) {
#include "src/game/effect/effect_util_fn_8013208C.inc"
}
#else
void msgctrlClientMos(void) {
    extern u32 lbl_8047ADDC;
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fn_801F18DC(u32);
    extern u32 fightTrainerGetNamePtr(u32);
    extern u32 fightOutPokemonGetNicknamePtr(u32);
    extern void msgctrlSetValue(u32, u32);
    extern void GSmsgGetGSchar(u32);
    u32 result;
    u32 val = lbl_8047ADDC;
    u32 new_var;
    result = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, val);
    new_var = val;
    if ((((u8) fn_801F18DC(0)) == 1) && (result != 0)) {
        msgctrlSetValue(0x4D, fightTrainerGetNamePtr(result));
        msgctrlSetValue(0x57, fightOutPokemonGetNicknamePtr(new_var));
        GSmsgGetGSchar(0x7721);
    } else {
        fightOutPokemonGetNicknamePtr(val);
    }
}
#endif


/* 0x80132124 | 0x98 */
#if 0
asm void msgctrlDeffenceMons(void) {
#include "src/game/effect/effect_util_fn_80132124.inc"
}
#else
void msgctrlDeffenceMons(void) {
    extern u32 lbl_8047ADD8;
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fn_801F18DC(u32);
    extern u32 fightTrainerGetNamePtr(u32);
    extern u32 fightOutPokemonGetNicknamePtr(u32);
    extern void msgctrlSetValue(u32, u32);
    extern void GSmsgGetGSchar(u32);
    u32 result;
    u32 val = lbl_8047ADD8;
    u32 new_var;
    result = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, val);
    new_var = val;
    if ((((u8) fn_801F18DC(0)) == 1) && (result != 0)) {
        msgctrlSetValue(0x4D, fightTrainerGetNamePtr(result));
        msgctrlSetValue(0x57, fightOutPokemonGetNicknamePtr(new_var));
        GSmsgGetGSchar(0x7721);
    } else {
        fightOutPokemonGetNicknamePtr(val);
    }
}
#endif


/* 0x801321BC | 0x98 */
#if 0
asm void msgctrlAttackMons(void) {
#include "src/game/effect/effect_util_fn_801321BC.inc"
}
#else
void msgctrlAttackMons(void) {
    extern u32 lbl_8047ADD4;
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fn_801F18DC(u32);
    extern u32 fightTrainerGetNamePtr(u32);
    extern u32 fightOutPokemonGetNicknamePtr(u32);
    extern void msgctrlSetValue(u32, u32);
    extern void GSmsgGetGSchar(u32);
    u32 result;
    u32 new_var;
    u32 val = lbl_8047ADD4;
    result = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, val);
    new_var = val;
    if ((((u8) fn_801F18DC(0)) == 1) && (result != 0)) {
        msgctrlSetValue(0x4D, fightTrainerGetNamePtr(result));
        msgctrlSetValue(0x57, fightOutPokemonGetNicknamePtr(new_var));
        GSmsgGetGSchar(0x7721);
    } else {
        fightOutPokemonGetNicknamePtr(new_var);
    }
}
#endif


/* 0x80132254 | 0x8 | sda_getter */
u32 msgctrlEvStrBuf2(void) { return lbl_8047ADD0; }


/* 0x8013225C | 0x8 | sda_getter */
u32 msgctrlEvStrBuf1(void) { return lbl_8047ADCC; }


/* 0x80132264 | 0x8 | sda_getter */
u32 msgctrlEvStrBuf0(void) { return lbl_8047ADC8; }


/* 0x8013226C | 0x28 -- calls wazaDataBiosGetPtr(lbl_8047AE7C) then wazaDataBiosGetName */
#if 0
asm void msgctrlWaza(void) {
#include "src/game/effect/effect_util_fn_8013226C.inc"
}
#else
void msgctrlWaza(void) {
    wazaDataBiosGetPtr(lbl_8047AE7C);
    wazaDataBiosGetName();
}
#endif


/* 0x80132294 | 0x8 | sda_getter */
u32 msgctrlMenuMsg2(void) { return lbl_8047AE78; }


/* 0x8013229C | 0x8 | sda_getter */
u32 msgctrlMenuMsg(void) { return lbl_8047AE74; }


/* 0x801322A4 | 0x8 | sda_getter */
u32 msgctrlMenuPokemon(void) { return lbl_8047AE70; }


/* 0x801322AC | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80427150, 0x10, lbl_8047AE6C, 0) */
#if 0
asm void msgctrlMenuDigit2(void) {
#include "src/game/effect/effect_util_fn_801322AC.inc"
}
#else
void msgctrlMenuDigit2(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427150, 0x10, lbl_8047AE6C, 0);
}
#endif


/* 0x801322E0 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80427170, 0x10, lbl_8047AE68, 0) */
#if 0
asm void msgctrlMenuDigit(void) {
#include "src/game/effect/effect_util_fn_801322E0.inc"
}
#else
void msgctrlMenuDigit(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427170, 0x10, lbl_8047AE68, 0);
}
#endif


/* 0x80132314 | 0x8 | sda_getter */
u32 msgctrlPokemon2(void) { return lbl_8047AE64; }


/* 0x8013231C | 0x8 | sda_getter */
u32 msgctrlPokemon(void) { return lbl_8047AE60; }


/* 0x80132324 | 0x8 | sda_getter */
u32 msgctrlMsgID(void) { return lbl_8047AE5C; }


/* 0x8013232C | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80427190, 0x10, lbl_8047AE58, 0) */
#if 0
asm void msgctrlDigit2(void) {
#include "src/game/effect/effect_util_fn_8013232C.inc"
}
#else
void msgctrlDigit2(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427190, 0x10, lbl_8047AE58, 0);
}
#endif


/* 0x80132360 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_804271B0, 0x10, lbl_8047AE54, 0) */
#if 0
asm void msgctrlDigit(void) {
#include "src/game/effect/effect_util_fn_80132360.inc"
}
#else
void msgctrlDigit(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_804271B0, 0x10, lbl_8047AE54, 0);
}
#endif


/* 0x80132394 | 0x34 -- calls itemDataBiosGetPtr(lbl_8047AE52) then itemDataBiosGetName, default to 0x2B6E */
u32 msgctrlItem2(void) {
    u32 result;
    itemDataBiosGetPtr(lbl_8047AE52);
    result = itemDataBiosGetName();
    if (result == 0) { result = 0x2B6E; }
    return result;
}


/* 0x801323C8 | 0x34 -- calls itemDataBiosGetPtr(lbl_8047AE50) then itemDataBiosGetName, default to 0x2B6E */
u32 msgctrlItem(void) {
    u32 result;
    itemDataBiosGetPtr(lbl_8047AE50);
    result = itemDataBiosGetName();
    if (result == 0) { result = 0x2B6E; }
    return result;
}


/* 0x801323FC | 0x2C -- savedataGetStatus(0, 2) then heroBiosGetHizukiNamePtr */
u32 msgctrlHizuki(void) {
    savedataGetStatus(0, 2);
    return heroBiosGetHizukiNamePtr();
}


/* 0x80132428 | 0x2C -- savedataGetStatus(0, 2) then heroBiosGetNamePtr */
u32 msgctrlHero(void) {
    savedataGetStatus(0, 2);
    return heroBiosGetNamePtr();
}

#if 0
asm void msgctrlWait(void) {
#include "src/game/effect/effect_util_fn_80132454.inc"
}
#else
s32 msgctrlWait(EffectUtilCommandObj* obj) {
    u8* stream;
    s32 counter;
    if (obj->activeFlag != 0) {
        goto doneIncrement;
    }
    if (obj->waitCounter == 0) {
        stream = obj->stream;
        obj->waitCounter = (s16)((s16)stream[0] + 1);
    }
    counter = obj->waitCounter;
    if ((obj->waitCounter = --counter) <= 0) {
        obj->waitCounter = 0;
        goto doneIncrement;
    }
    stream = obj->stream;
    obj->stream = stream - 3;
    return 1;
doneIncrement:
    stream = obj->stream;
    obj->stream = stream + 1;
    return 0;
}
#endif


/* 0x801324CC | 0xA4 -- read color index from stream, look up RGBA, apply */
void msgctrlPalette(EffectUtilCommandObj* obj) {
    extern u32* lbl_80478E88;
    extern u32 lbl_80478E8C;
    extern void GSmsgSetColor(void*);
    u8* stream;
    u8 idx;
    u8* colorPtr;
    u32 color;
    u32 maxIdx;

    if (obj->activeFlag != 0) {
        stream = obj->stream;
        idx = *stream;
        maxIdx = *lbl_80478E88;
        if (idx >= maxIdx) {
            idx = 0;
        }
        colorPtr = (u8*)(lbl_80478E8C + (u32)idx * 4);
        color = (u32)colorPtr[0] << 24;
        color |= (u32)colorPtr[1] << 16;
        color |= (u32)colorPtr[2] << 8;
        color |= colorPtr[3];
        obj->colorRgba = color;
        GSmsgSetColor(obj);
    }
    /* Advance stream pointer */
    obj->stream = obj->stream + 1;
}

#if 0
asm void msgctrlColor(void) {
#include "src/game/effect/effect_util_fn_80132570.inc"
}
#else
s32 msgctrlColor(EffectUtilCommandObj* obj) {
    extern void GSmsgSetColor(void*);
    u8* stream;
    if (obj->activeFlag != 0) {
        stream = obj->stream;
        obj->colorRgba = *(u32*)stream;
        GSmsgSetColor(obj);
    }
    stream = obj->stream;
    obj->stream = stream + 4;
    return 0;
}
#endif


/* 0x801325C4 | 0x68 -- read byte command into u16 field and apply */
s32 msgctrlFont(EffectUtilCommandObj* obj) {
    u8* stream;

    if (obj->activeFlag == 0) {
        stream = obj->stream;
        obj->commandValue = stream[0];
        GSmsgSetFontInfo(obj);
    } else {
        stream = obj->stream;
        obj->commandValue = stream[0];
        GSmsgSetFontInfo(obj);
    }
    stream = obj->stream;
    obj->stream = stream + 1;
    return 0;
}


/* 0x8013262C | 16 bytes | set_field_return */
u32 msgctrlRubyEnd(EffectUtilCommandObj* obj) {
    obj->field_4B = 0;
    return 0;
}


/* 0x8013263C | 16 bytes | set_field_return */
u32 msgctrlRubyTop(EffectUtilCommandObj* obj) {
    obj->field_4B = 2;
    return 0;
}

#if 0
asm void msgctrlRubyStart(void) {
#include "src/game/effect/effect_util_fn_8013264C.inc"
}
#else
s32 msgctrlRubyStart(EffectUtilCommandObj* obj) {
    extern void GSmsgInitRuby(void*);
    if (obj->activeFlag != 0) {
        GSmsgInitRuby(obj);
    }
    obj->field_4B = 1;
    return 0;
}
#endif


/* 0x80132690 | 0xCC -- effect tick with flag-based logic */
u32 msgctrlKeyWait(EffectUtilCommandObj* obj) {
    extern void soundStartSE(u32);
    u8* stream;
    u8 done;

    if (obj->flags & 0x02) {
        obj->pendingFlag = 1;
    }
    if ((u32)(menuIsCheck(0x0A) & 0xFF) != 0) {
        obj->pendingFlag = 0;
    }

    if (obj->activeFlag == 0) {
        if (obj->pendingFlag != 0) {
            obj->pendingFlag = 0;
            obj->savedStream = obj->stream;
            if ((obj->flags & 0x02) == 0) {
                soundStartSE(0x24);
            }
        } else {
            stream = obj->stream;
            done = 1;
            obj->stream = stream - 3;
            obj->doneFlag = done;
        }
    } else {
        obj->field_0C = obj->field_04;
        obj->field_10 = obj->field_08;
    }
    return 1;
}


/* 0x8013275C | 0x84 */
#if 0
asm void msgctrlKeyEnd(void) {
#include "src/game/effect/effect_util_fn_8013275C.inc"
}
#else
u32 msgctrlKeyEnd(EffectUtilCommandObj* obj) {
    if (obj->flags & 0x02) {
        obj->pendingFlag = 1;
    }
    if ((u32)(menuIsCheck(0x0A) & 0xFF) != 0) {
        obj->pendingFlag = 0;
    }
    if (obj->pendingFlag != 0) {
        obj->pendingFlag = 0;
    } else {
        obj->stream = obj->stream - 3;
    }
    return 1;
}
#endif

#if 0
asm void msgctrlCR(void) {
#include "src/game/effect/effect_util_fn_801327E0.inc"
}
#else
u32 msgctrlCR(void* obj) {
    u8* p = (u8*)obj;
    f32 diff;
    *(f32*)(p + 0x0C) = *(f32*)(p + 0x04);
    diff = (f32)((s32)(u8)p[0x23] + (s32)(s8)p[0x42]);
    *(f32*)(p + 0x10) += *(f32*)(p + 0x64) * diff;
    return 0;
}
#endif


/* 0x80132834 - 0x80132A38: shared with the standalone matching owner. */
#include "src/game/msgctrl_candidate_80132834.c"


/* 0x80132A38 - 0x80132C6C: msgctrlSetValue and msgctrlInitValue are in
 * msgctrl_exact_80132A38.c. */
#endif /* MSGCTRL_WHOLE_TU */
