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

#if 0
asm void msgctrlSndWait(void) {
#include "src/game/effect/effect_util_fn_80131588.inc"
}
#else
#pragma peephole off
#pragma scheduling on
u32 msgctrlSndWait(EffectUtilCommandObj* obj) {
    if (obj->activeFlag == 0) {
        u16 handle = lbl_8047AEA4;
        if (handle == 0) {
            return 0;
        }
        if (fn_801666BC(handle) == 2) {
            obj->stream = obj->stream - 3;
        }
    }
    return 1;
}
#pragma scheduling off
#pragma peephole on
#endif

#if 0
asm void msgctrlSndPlay(void) {
#include "src/game/effect/effect_util_fn_801315EC.inc"
}
#else
#pragma peephole off
u32 msgctrlSndPlay(EffectUtilCommandObj* obj) {
    if (obj->activeFlag == 0) {
        u16 handle = lbl_8047AEA4;
        if (handle != 0) {
            fn_80165A20(handle, 0, 0xFF);
        }
    }
    return 0;
}
#pragma peephole on
#endif


/* 0x80131630 | 0x30 -- read byte from stream, store extsb to obj+0x43 if flag set */
#pragma push
#pragma optimization_level 2
u32 msgctrlBaseLineBias(EffectUtilCommandObj* obj) {
    u8* stream;
    if (obj->activeFlag != 0) {
        stream = obj->stream;
        obj->field_43 = (u8)(s8)*stream;
    }
    stream = obj->stream;
    obj->stream = stream + 1;
    return 0;
}
#pragma pop


/* 0x80131660 | 0x30 -- read byte from stream, store extsb to obj+0x42 if flag set */
#pragma push
#pragma optimization_level 2
u32 msgctrlLineSpace(EffectUtilCommandObj* obj) {
    u8* stream;
    if (obj->activeFlag != 0) {
        stream = obj->stream;
        obj->field_42 = (u8)(s8)*stream;
    }
    stream = obj->stream;
    obj->stream = stream + 1;
    return 0;
}
#pragma pop


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
#pragma optimization_level 4
#pragma push
#pragma peephole off
#pragma scheduling on
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
#pragma scheduling off
#pragma pop
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
#pragma peephole off
#pragma scheduling on
void msgctrlMenuMoney(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80426FF0, 0x10, lbl_8047AE94, 4);
}
#pragma scheduling off
#pragma peephole on
#endif


/* 0x801317C8 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80427010, 0x10, lbl_8047AE68, 5) */
#if 0
asm void msgctrlMenuFullDigit(void) {
#include "src/game/effect/effect_util_fn_801317C8.inc"
}
#else
#pragma peephole off
#pragma scheduling on
void msgctrlMenuFullDigit(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427010, 0x10, lbl_8047AE68, 5);
}
#pragma scheduling off
#pragma peephole on
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
#pragma peephole off
#pragma scheduling on
void msgctrlMoney(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427070, 0x10, lbl_8047AE80, 4);
}
#pragma scheduling off
#pragma peephole on
#endif


/* 0x80131A68 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80427090, 0x10, lbl_8047AE6C, 2) */
#if 0
asm void msgctrlMenuZDigit2(void) {
#include "src/game/effect/effect_util_fn_80131A68.inc"
}
#else
#pragma peephole off
#pragma scheduling on
void msgctrlMenuZDigit2(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427090, 0x10, lbl_8047AE6C, 2);
}
#pragma scheduling off
#pragma peephole on
#endif


/* 0x80131A9C | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_804270B0, 0x10, lbl_8047AE68, 2) */
#if 0
asm void msgctrlMenuZDigit(void) {
#include "src/game/effect/effect_util_fn_80131A9C.inc"
}
#else
#pragma peephole off
#pragma scheduling on
void msgctrlMenuZDigit(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_804270B0, 0x10, lbl_8047AE68, 2);
}
#pragma scheduling off
#pragma peephole on
#endif


/* 0x80131AD0 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_804270D0, 0x10, lbl_8047AE68, 3) */
#if 0
asm void msgctrlMenuHex2(void) {
#include "src/game/effect/effect_util_fn_80131AD0.inc"
}
#else
#pragma peephole off
#pragma scheduling on
void msgctrlMenuHex2(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_804270D0, 0x10, lbl_8047AE68, 3);
}
#pragma scheduling off
#pragma peephole on
#endif


/* 0x80131B04 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_804270F0, 0x10, lbl_8047AE68, 3) */
#if 0
asm void msgctrlMenuHex(void) {
#include "src/game/effect/effect_util_fn_80131B04.inc"
}
#else
#pragma peephole off
#pragma scheduling on
void msgctrlMenuHex(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_804270F0, 0x10, lbl_8047AE68, 3);
}
#pragma scheduling off
#pragma peephole on
#endif


/* 0x80131B38 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80427110, 0x10, lbl_8047AE6C, 1) */
#if 0
asm void msgctrlMenuUDigit2(void) {
#include "src/game/effect/effect_util_fn_80131B38.inc"
}
#else
#pragma peephole off
#pragma scheduling on
void msgctrlMenuUDigit2(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427110, 0x10, lbl_8047AE6C, 1);
}
#pragma scheduling off
#pragma peephole on
#endif


/* 0x80131B6C | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80427130, 0x10, lbl_8047AE68, 1) */
#if 0
asm void msgctrlMenuUDigit(void) {
#include "src/game/effect/effect_util_fn_80131B6C.inc"
}
#else
#pragma peephole off
#pragma scheduling on
void msgctrlMenuUDigit(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427130, 0x10, lbl_8047AE68, 1);
}
#pragma scheduling off
#pragma peephole on
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
#pragma peephole off
void msgctrlSideDefenceNameno(void) {
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE4C, 2);
}
#pragma peephole on
#endif


/* 0x80131C20 | 0x28 -- _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE48, 1) */
#if 0
asm void msgctrlSideDefenceNamewo(void) {
#include "src/game/effect/effect_util_fn_80131C20.inc"
}
#else
#pragma peephole off
void msgctrlSideDefenceNamewo(void) {
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE48, 1);
}
#pragma peephole on
#endif


/* 0x80131C48 | 0x28 -- _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE44, 0) */
#if 0
asm void msgctrlSideDefenceNameha(void) {
#include "src/game/effect/effect_util_fn_80131C48.inc"
}
#else
#pragma peephole off
void msgctrlSideDefenceNameha(void) {
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE44, 0);
}
#pragma peephole on
#endif


/* 0x80131C70 | 0x28 -- _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE1C, 2) */
#if 0
asm void msgctrlSideAttackNameno(void) {
#include "src/game/effect/effect_util_fn_80131C70.inc"
}
#else
#pragma peephole off
void msgctrlSideAttackNameno(void) {
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE1C, 2);
}
#pragma peephole on
#endif


/* 0x80131C98 | 0x28 -- _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE18, 1) */
#if 0
asm void msgctrlSideAttackNamewo(void) {
#include "src/game/effect/effect_util_fn_80131C98.inc"
}
#else
#pragma peephole off
void msgctrlSideAttackNamewo(void) {
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE18, 1);
}
#pragma peephole on
#endif


/* 0x80131CC0 | 0x28 -- _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE14, 0) */
#if 0
asm void msgctrlSideAttackNameha(void) {
#include "src/game/effect/effect_util_fn_80131CC0.inc"
}
#else
#pragma peephole off
void msgctrlSideAttackNameha(void) {
    _msgctrlSideName__FP15FightOutPokemonUc(lbl_8047AE14, 0);
}
#pragma peephole on
#endif


/* 0x80131CE8 | 0x21C */
#pragma push
#pragma optimization_level 1
void _msgctrlSideName__FP15FightOutPokemonUc(u32 arg1, u32 arg2) {
    extern void GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern void fightTargetIsHostSide();
    extern void fightTargetGetPtrAsNowFightType();
    extern void fn_801F18DC();
    extern void fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern void fightFloorGetStatus();
    extern void fightSideGetValidFightTrainerPtr();
    extern void fightTrainerGetNamePtr();
    extern void fightTrainerCheckDoFight();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r24 = r3;
    r31 = r4;
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x14;
    r6 = 0x0;
    fightFloorGetStatus();
    r30 = r3 & 0xFFFF;
    r4 = r24;
    r3 = 0x2;
    fightTargetGetPtrAsNowFightType();
    r29 = r3;
    r4 = r24;
    r3 = 0x0;
    fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    r26 = 0x0;
    r28 = r3;
    r25 = 0x0;
    while (1) {
        r0 = r25 & 0xFFFF;
        if (r0 >= (u32)0x2) break;
        r3 = r29;
        r4 = r25;
        fightSideGetValidFightTrainerPtr();
        r27 = r3;
        do {
            if (r27 == (u32)0x0) break;
            fightTrainerCheckDoFight();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x0) break;
            r0 = r26 & 0xFFFF;
            if (r0 == (u32)0x0) {
                r3 = r27;
                fightTrainerGetNamePtr();
                r4 = r3;
                r3 = 0x4d;
                msgctrlSetValue();

            } else {
                if (r0 == (u32)0x1) {
                    r3 = r27;
                    fightTrainerGetNamePtr();
                    r4 = r3;
                    r3 = 0x57;
                    msgctrlSetValue();
                }
            }
            r26 = r26 + 0x1;
        } while (0);
        r25 = r25 + 0x1;

    }
    r3 = 0x0;
    fn_801F18DC();
    r0 = r3 & 0xFF;
    if ((r0 == (u32)0x1) && (r28 != (u32)0x0)) {

        r0 = r26 & 0xFFFF;
        if (r0 <= (u32)0x1) {
            r3 = r28;
            fightTrainerGetNamePtr();
            r4 = r3;
            r3 = 0x4d;
            msgctrlSetValue();
            r0 = r31 & 0xFF;
            if (r0 == (u32)0x0) {
                r3 = 0x7722;
                GSmsgGetGSchar();
                return;
            }
            if (r0 == (u32)0x1) {
                r3 = 0x7725;
                GSmsgGetGSchar();
                return;
            }
            r3 = 0x7727;
            GSmsgGetGSchar();
            return;
        }
        r0 = r31 & 0xFF;
        if (r0 == (u32)0x0) {
            r3 = 0x7724;
            GSmsgGetGSchar();
            return;
        }
        if (r0 == (u32)0x1) {
            r3 = 0x7726;
            GSmsgGetGSchar();
            return;
        }
        r3 = 0x7728;
        GSmsgGetGSchar();
        return;
    }
    r3 = r24;
    r4 = r30;
    fightTargetIsHostSide();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = r31 & 0xFF;
        if (r0 == (u32)0x0) {
            r3 = 0x768a;
            GSmsgGetGSchar();
            return;
        }
        if (r0 == (u32)0x1) {
            r3 = 0x768c;
            GSmsgGetGSchar();
            return;
        }
        r3 = 0x7688;
        GSmsgGetGSchar();
        return;
    }
    r0 = r31 & 0xFF;
    if (r0 == (u32)0x0) {
        r3 = 0x7689;
        GSmsgGetGSchar();
        return;
    }
    if (r0 == (u32)0x1) {
        r3 = 0x768b;
        GSmsgGetGSchar();
        return;
    }
    r3 = 0x7687;
    GSmsgGetGSchar();

    return;
}
#pragma pop


/* 0x80131F04 | 0x98 */
#if 0
asm void msgctrlClientnowork(void) {
#include "src/game/effect/effect_util_fn_80131F04.inc"
}
#else
#pragma push
#pragma peephole off
#pragma scheduling on
#pragma push
#pragma optimization_level 1
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
#pragma pop
#pragma scheduling off
#pragma pop
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
#pragma push
#pragma peephole off
#pragma scheduling on
#pragma push
#pragma optimization_level 1
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
#pragma pop
#pragma scheduling off
#pragma pop
#endif


/* 0x8013208C | 0x98 */
#if 0
asm void msgctrlClientMos(void) {
#include "src/game/effect/effect_util_fn_8013208C.inc"
}
#else
#pragma push
#pragma peephole off
#pragma scheduling on
#pragma push
#pragma optimization_level 1
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
#pragma pop
#pragma scheduling off
#pragma pop
#endif


/* 0x80132124 | 0x98 */
#if 0
asm void msgctrlDeffenceMons(void) {
#include "src/game/effect/effect_util_fn_80132124.inc"
}
#else
#pragma push
#pragma peephole off
#pragma scheduling on
#pragma push
#pragma optimization_level 1
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
#pragma pop
#pragma scheduling off
#pragma pop
#endif


/* 0x801321BC | 0x98 */
#if 0
asm void msgctrlAttackMons(void) {
#include "src/game/effect/effect_util_fn_801321BC.inc"
}
#else
#pragma push
#pragma peephole off
#pragma scheduling on
#pragma push
#pragma optimization_level 1
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
#pragma pop
#pragma scheduling off
#pragma pop
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
#pragma peephole off
void msgctrlWaza(void) {
    wazaDataBiosGetPtr(lbl_8047AE7C);
    wazaDataBiosGetName();
}
#pragma peephole on
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
#pragma peephole off
#pragma scheduling on
void msgctrlMenuDigit2(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427150, 0x10, lbl_8047AE6C, 0);
}
#pragma scheduling off
#pragma peephole on
#endif


/* 0x801322E0 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_80427170, 0x10, lbl_8047AE68, 0) */
#if 0
asm void msgctrlMenuDigit(void) {
#include "src/game/effect/effect_util_fn_801322E0.inc"
}
#else
#pragma peephole off
#pragma scheduling on
void msgctrlMenuDigit(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427170, 0x10, lbl_8047AE68, 0);
}
#pragma scheduling off
#pragma peephole on
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
#pragma peephole off
#pragma scheduling on
void msgctrlDigit2(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_80427190, 0x10, lbl_8047AE58, 0);
}
#pragma scheduling off
#pragma peephole on
#endif


/* 0x80132360 | 0x34 -- _msgctrlMakeDigit__FPUslUll(lbl_804271B0, 0x10, lbl_8047AE54, 0) */
#if 0
asm void msgctrlDigit(void) {
#include "src/game/effect/effect_util_fn_80132360.inc"
}
#else
#pragma peephole off
#pragma scheduling on
void msgctrlDigit(void) {
    _msgctrlMakeDigit__FPUslUll(lbl_804271B0, 0x10, lbl_8047AE54, 0);
}
#pragma scheduling off
#pragma peephole on
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
#pragma peephole off
u32 msgctrlHizuki(void) {
    savedataGetStatus(0, 2);
    return heroBiosGetHizukiNamePtr();
}
#pragma peephole on


/* 0x80132428 | 0x2C -- savedataGetStatus(0, 2) then heroBiosGetNamePtr */
#pragma peephole off
u32 msgctrlHero(void) {
    savedataGetStatus(0, 2);
    return heroBiosGetNamePtr();
}

#if 0
asm void msgctrlWait(void) {
#include "src/game/effect/effect_util_fn_80132454.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
s32 msgctrlWait(EffectUtilCommandObj* obj) {
    u8* stream;
    s16 counter;
    if (obj->activeFlag != 0) {
        goto doneIncrement;
    }
    if (obj->waitCounter == 0) {
        stream = obj->stream;
        obj->waitCounter = (s16)((s16)stream[0] + 1);
    }
    counter = obj->waitCounter;
    counter = (s16)(counter - 1);
    obj->waitCounter = counter;
    if (counter <= 0) {
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
#pragma peephole on
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
#pragma optimization_level 4
#pragma push
#pragma peephole off
#pragma scheduling on
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
#pragma scheduling off
#pragma pop
#endif


/* 0x801325C4 | 0x68 -- read byte command into u16 field and apply */
#pragma push
#pragma optimization_level 4
#pragma peephole off
#pragma scheduling on
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
#pragma scheduling off
#pragma pop


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
#pragma optimization_level 4
#pragma push
#pragma peephole off
#pragma scheduling on
s32 msgctrlRubyStart(EffectUtilCommandObj* obj) {
    extern void GSmsgInitRuby(void*);
    if (obj->activeFlag != 0) {
        GSmsgInitRuby(obj);
    }
    obj->field_4B = 1;
    return 0;
}
#pragma scheduling off
#pragma pop
#endif


/* 0x80132690 | 0xCC -- effect tick with flag-based logic */
#pragma push
#pragma peephole off
#pragma scheduling on
u32 msgctrlKeyWait(EffectUtilCommandObj* obj) {
    extern void fn_80166A28(u32);
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
                fn_80166A28(0x24);
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
#pragma scheduling off
#pragma pop


/* 0x8013275C | 0x84 */
#if 0
asm void msgctrlKeyEnd(void) {
#include "src/game/effect/effect_util_fn_8013275C.inc"
}
#else
#pragma peephole off
#pragma scheduling on
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
#pragma scheduling off
#pragma peephole on
#endif

#if 0
asm void msgctrlCR(void) {
#include "src/game/effect/effect_util_fn_801327E0.inc"
}
#else
#pragma scheduling on
u32 msgctrlCR(void* obj) {
    u8* p = (u8*)obj;
    f32 diff;
    *(f32*)(p + 0x0C) = *(f32*)(p + 0x04);
    diff = (f32)((s32)(s8)p[0x42] + (s32)(u8)p[0x23]);
    *(f32*)(p + 0x10) += *(f32*)(p + 0x64) * diff;
    return 0;
}
#pragma scheduling off
#endif


/* 0x80132834 | 0x204 */
#pragma push
#pragma optimization_level 1
void* _msgctrlMakeDigit__FPUslUll(void* table, u32 stride, u32 count, u32 type) {
    extern u8 lbl_803635F0[];
    extern u8 lbl_80363610[];
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = (u32)table;
    u32 r4 = stride;
    u32 r5 = count;
    u32 r6 = type;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    *(u32*)(sp + 0x8) = r5;
    r9 = r4 - 1;
    r0 = r9 << 1;
    r4 = 0x0;
    *(u16*)(r3 + r0) = r4;
    r0 = 0x0;
    r4 = 0xa;
    r5 = 0x0;
    r7 = 0x1;
    if ((s32)r6 != (s32)0x3) {
        if ((s32)r6 < (s32)0x3) {
            if ((s32)r6 == (s32)0x1) goto L_801328BC;
            if ((s32)r6 < (s32)0x1) {
                if ((s32)r6 < (s32)0x0) {
                    goto L_801328BC;
                }
                if ((s32)r6 != (s32)0x5) {
                    goto L_801328BC;
                }
                }
            r8 = *(u32*)(sp + 0x8);
            if ((s32)r8 < (s32)0x0) {
                r8 = -r8;
                r0 = 0x1;
            }
            goto L_801328BC;
            }
        r7 = 0xa;

    } else {
        r4 = 0x10;
        r7 = 0x8;
    }
L_801328BC: ;
    if ((s32)r6 == (s32)0x5) {
        r8 = (u32)lbl_80363610;
        r8 = (u32)lbl_80363610;
    } else {
        r8 = (u32)lbl_803635F0;
        r8 = (u32)lbl_803635F0;
    }
    r10 = r9 << 1;
    while (1) {
        r11 = *(u32*)(sp + 0x8);
        if (r11 == (u32)0x0) break;
        if ((s32)r6 == (s32)0x4) {
            if ((s32)r5 != (s32)0x0) {
                r11 = (0x5555 << 16);
                r11 = r11 + 0x5556;
                r12 = (s32)((s64)r11 * (s64)r5 >> 32);
                r11 = (u32)r12 >> 31;
                r11 = r12 + r11;
                r11 = r11 * 0x3;
                r11 = r5 - r11;
                if ((s32)r11 == (s32)0x0) {
                    r11 = 0x2c;
                    *(u16*)(r3 + r10) = r11;
        }
        }
        }
        r31 = *(u32*)(sp + 0x8);
        r5 = r5 + 0x1;
        r12 = (u32)r31 / (u32)r4;
        r11 = r12 * r4;
        r11 = r31 - r11;
        r11 = r11 << 1;
        r11 = *(u16*)(r8 + r11);
        *(u16*)(r3 + r10) = r11;

    }
    r6 = r9 << 1;
    r5 = r7 - r5;
    do {
        if ((s32)r5 >= (s32)r7) break;
        r4 = (u32)r5 >> 3;
        ctr_fn = (void(*)(void))r4;
        if (r4 != (u32)0x0) {
            do {
                r4 = *(u16*)((u8*)r8 + 0x0);
                *(u16*)(r3 + r6) = r4;
                r4 = *(u16*)((u8*)r8 + 0x0);
                *(u16*)(r3 + r6) = r4;
                r4 = *(u16*)((u8*)r8 + 0x0);
                *(u16*)(r3 + r6) = r4;
                r4 = *(u16*)((u8*)r8 + 0x0);
                *(u16*)(r3 + r6) = r4;
                r4 = *(u16*)((u8*)r8 + 0x0);
                *(u16*)(r3 + r6) = r4;
                r4 = *(u16*)((u8*)r8 + 0x0);
                *(u16*)(r3 + r6) = r4;
                r4 = *(u16*)((u8*)r8 + 0x0);
                *(u16*)(r3 + r6) = r4;
                r4 = *(u16*)((u8*)r8 + 0x0);
                *(u16*)(r3 + r6) = r4;
            } while (--ctr != 0);
            r5 = r5 & 0x7;
            if (r4 == (u32)0x0) break;
        }
        ctr_fn = (void(*)(void))r5;
        do {
            r4 = *(u16*)((u8*)r8 + 0x0);
            *(u16*)(r3 + r6) = r4;
        } while (--ctr != 0);
    } while (0);
    r0 = r0 & 0xFF;
    if (r0 != (u32)0x0) {
        r4 = 0x2d;
        r0 = r9 << 1;
        *(u16*)(r3 + r0) = r4;
    }
    r0 = r9 << 1;
    r3 = r3 + r0;
    r31 = *(u32*)(sp + 0x1C);
    return (void*)r3;
}
#pragma pop


/* 0x80132A38 | 0x210 */
#if 0
asm void msgctrlSetValue(void) {
#include "src/game/effect/effect_util_fn_80132A38.inc"
}
#else
#pragma push
#pragma peephole off
#pragma scheduling on
void msgctrlSetValue(u32 id, u32 value) {
    switch (id) {
    case 0x0D: lbl_8047AE50 = (u16)value; return;
    case 0x0E: lbl_8047AE52 = (u16)value; return;
    case 0x0F: lbl_8047AE54 = value; return;
    case 0x10: lbl_8047AE58 = value; return;
    case 0x11: lbl_8047AE5C = value; return;
    case 0x12: lbl_8047AE60 = value; return;
    case 0x13: lbl_8047AE64 = value; return;
    case 0x14: lbl_8047AE68 = value; return;
    case 0x15: lbl_8047AE6C = value; return;
    case 0x16: lbl_8047AE70 = value; return;
    case 0x17: lbl_8047AE74 = value; return;
    case 0x18: lbl_8047AE78 = value; return;
    case 0x19: lbl_8047AE7C = (u16)value; return;
    case 0x1A: lbl_8047AE88 = value; return;
    case 0x1B: lbl_8047AE8C = value; return;
    case 0x1C: lbl_8047AE90 = (u16)value; return;
    case 0x1D: lbl_8047AE80 = value; return;
    case 0x1E: lbl_8047AE84 = value; return;
    case 0x1F: lbl_8047AE94 = value; return;
    case 0x20: lbl_8047AE98 = value; return;
    case 0x21: lbl_8047AE9C = value; return;
    case 0x22: lbl_8047AEA0 = (u16)value; return;
    case 0x23: lbl_8047AEA2 = (u16)value; return;
    case 0x24: lbl_8047AEA4 = (u16)value; return;
    case 0x3C: lbl_8047ADC8 = value; return;
    case 0x3D: lbl_8047ADCC = value; return;
    case 0x3E: lbl_8047ADD0 = value; return;
    case 0x3F: lbl_8047ADD4 = value; return;
    case 0x40: lbl_8047ADD8 = value; return;
    case 0x41: lbl_8047ADDC = value; return;
    case 0x42: lbl_8047ADE0 = value; return;
    case 0x43: lbl_8047ADE4 = value; return;
    case 0x44: lbl_8047ADE8 = value; return;
    case 0x45: lbl_8047ADEC = value; return;
    case 0x46: lbl_8047ADF0 = value; return;
    case 0x47: lbl_8047ADF4 = value; return;
    case 0x48: lbl_8047ADF8 = value; return;
    case 0x49: lbl_8047ADFC = value; return;
    case 0x4A: lbl_8047AE00 = value; return;
    case 0x4B: lbl_8047AE04 = value; return;
    case 0x4C: lbl_8047AE08 = value; return;
    case 0x4D: lbl_8047AE0C = value; return;
    case 0x4E: lbl_8047AE10 = value; return;
    case 0x4F: lbl_8047AE14 = value; return;
    case 0x50: lbl_8047AE18 = value; return;
    case 0x51: lbl_8047AE1C = value; return;
    case 0x52: lbl_8047AE20 = value; return;
    case 0x53: lbl_8047AE24 = value; return;
    case 0x54: lbl_8047AE28 = value; return;
    case 0x55: lbl_8047AE2C = value; return;
    case 0x56: lbl_8047AE30 = value; return;
    case 0x57: lbl_8047AE34 = value; return;
    case 0x58: lbl_8047AE38 = value; return;
    case 0x59: lbl_8047AE3C = value; return;
    case 0x5A: lbl_8047AE40 = value; return;
    case 0x5B: lbl_8047AE44 = value; return;
    case 0x5C: lbl_8047AE48 = value; return;
    case 0x5D: lbl_8047AE4C = value; return;
    }
}
#pragma scheduling off
#pragma pop
#endif


/* 0x80132C48 | 36 bytes | multi_sda_store */
#if 0
asm void msgctrlInitValue(void) {
#include "src/game/effect/effect_util_fn_80132C48.inc"
}
#else
void msgctrlInitValue(void) {
    lbl_8047AE70 = 0;
    lbl_8047AE74 = 0;
    lbl_8047AE78 = 0;
    lbl_8047AE60 = 0;
    lbl_8047AE64 = 0;
    lbl_8047AE88 = 0;
    lbl_8047AE8C = 0;
}
#endif
