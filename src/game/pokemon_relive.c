/**
 * @file pokemon_relive.c
 * @brief game/pxdvs/app/pokemon/pokemonRelive.cpp (possibly + pokemonWazaSequenceDisplay.cpp tail) -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x8025DC2C-0x8025F2FC, 19 fns.
 *
 * XD source unit: game/pxdvs/app/pokemon/pokemonRelive.cpp (possibly + pokemonWazaSequenceDisplay.cpp tail)
 * Physically split out of the pre/post-battle mega-file by address
 * (functions located and bucketed by name via config/GC6E01/symbols.txt,
 * since this TU uses plain named C bodies with no address-comment
 * markers).
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

/* =========================================================================
 * Duplicated declarations (verbatim from the original colosseum_battle.c
 * preamble, present in every split segment so each TU keeps the same
 * external visibility it had before the split)
 * ========================================================================= */
extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();

/* Battle system functions */
extern void fn_801EF8F4();

/* Sound functions */
extern void soundStop();     /* Stop sound */
extern void std104_soundSetBackgroundMusic();     /* Fade out music */
extern void std105_soundPlayEffect();     /* Start BGM */

/* SDA2 float constants used by asm wrappers */
extern f32 lbl_8047E678;
extern f32 lbl_8047E67C;

/* SDA1 globals used by asm wrappers */
extern u32 lbl_8047B668;
extern u32 lbl_8047B66C;
extern u32 lbl_8047B670;

/* Data labels used by asm wrappers */
extern u8  lbl_8039A6B8[];
extern u8  lbl_8039A6A8[];
extern int lbl_804782BC[];
extern u8  lbl_804782E0[];
extern u8  lbl_804783E0[];

/* Forward declarations for functions used as addresses in asm wrappers */
void ShortCommandProc(int r3);
void ReadProc(int r3);
void WriteProc(int r3);
void __GBASyncCallback(int r3);
u32  __GBASync(int r3);
u32  __GBATransfer(int r3, u32 r4, u32 r5, u32 r6);

/* Forward declarations for asm wrapper bl targets (use () form for compat) */
extern void DSPInit();
extern void set__5GSvecFfff();
extern int  _fadeEffectGetRandom__FUl();
extern u32  pokemonBiosGetCatchTrainerRnd();
extern u32  pokemonBiosGetRnd();
extern u16  pokemonBiosGetPokemonDataId();
extern u32  savedataGetStatus();
extern int  fadeCheck();
extern int  fadeSet();
extern int  wazaSequenceSysRelease();
extern int  fn_801DADC0();
extern void OSRegisterResetFunction();
extern void OSInitAlarm();
extern void OSInitThreadQueue();
extern void* memcpy();

/* Forward declarations for converted functions */
u32 evolutionWazaLearn();
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3);
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2);

/* Address: 0x8025DCBC | Size: 0x58 | Ghidra import */
void fn_8025DCBC(int *param)
{
    u32 *r3 = (u32 *)param;
    if (*r3 != 0) {
        std104_soundSetBackgroundMusic(*r3, 0x32, 0xff);
    }
    if (r3[1] != 0) {
        std105_soundPlayEffect(r3[1], 0x32, 0xff);
    }
}

/* Address: 0x8025DD14 | Size: 0x98 | Ghidra import */
#pragma push
#pragma use_lmw_stmw off
#pragma optimize_for_size off
void fn_8025DD14(int *r3)
{
    extern int fn_801653BC();
    extern int std228_soundGetCurrentBackgroundMusic();
    extern int fn_801656D8();
    extern void std104_soundSetBackgroundMusic();
    int iVar1;
    int iVar2;
    int iVar3;
    int iVar4;

    iVar1 = std228_soundGetCurrentBackgroundMusic();
    if (iVar1 != 0) {
        iVar2 = fn_801656D8();
        std104_soundSetBackgroundMusic(1, 0x32, 0xff);
    } else {
        iVar2 = 0;
    }
    iVar3 = fn_801653BC();
    if (iVar3 != 0) {
        iVar4 = fn_801656D8();
    } else {
        iVar4 = 0;
    }
    *r3 = iVar1;
    r3[1] = iVar3;
    r3[2] = iVar2;
    r3[3] = iVar4;
}
#pragma pop

/* Address: 0x8025DDAC | Size: 0x48 | Ghidra import */
void fn_8025DDAC(u32 *r3,u32 r4)
{
    extern u32 GSmodelSetRotation();
    extern u32 fn_801DAC3C(u32);
  u32 iVar1;
  r3 = (u32*)*r3;
  if (r3 == 0) return;
  iVar1 = fn_801DAC3C((u32)r3);
  if (iVar1 == 0) return;
  GSmodelSetRotation(iVar1,r4);
}

/* Address: 0x8025DDF4 | Size: 0x18 | Ghidra import */
void fn_8025DDF4(u32 *r3)

{
  if (*r3 == 0) {
    return;
  }
  *r3 = 0;
  return;
}

/* Address: 0x8025DE0C | Size: 0x48 | Ghidra import */
void fn_8025DE0C(u32 *r3,u32 r4)
{
    extern u32 GSmodelSetPosition();
    extern u32 fn_801DAC3C(u32);
  u32 iVar1;
  r3 = (u32*)*r3;
  if (r3 == 0) return;
  iVar1 = fn_801DAC3C((u32)r3);
  if (iVar1 == 0) return;
  GSmodelSetPosition(iVar1,r4);
}

/* Address: 0x8025DE54 | Size: 0xE4 | Ghidra import */
void fn_8025DE54(u32 *r3,u16 *r4,int r5,int r6,int r7,
                 int r8)

{
    extern int _threadSwitch();
    extern int fadeSet();
    extern int fn_801DA4E8();
    extern int fn_801DA8C4();
    extern int fn_801DA914();
    extern u8 fn_801DA94C();
    extern int fn_801DA9E8();
    extern int fn_801DB088();
    extern f32 lbl_8047E670;

  u16 *p;
  int iVar2;

  if (r7 == 1) {
    fadeSet((double)lbl_8047E670,2);
  }
  fn_801DA4E8(*r3,1);
  p = r4;
  for (iVar2 = 0; iVar2 < r5; iVar2 = iVar2 + 1) {
    if (r6 == 0) {
      fn_801DA914(*r3,*p,p[1]);
    }
    fn_801DA9E8(*r3,*p,p[1]);
    while (fn_801DA94C(*r3,*p,p[1]) != 0) {
      fn_801DB088();
      _threadSwitch();
    }
    if (r8 == 1) {
      fn_801DA4E8(*r3,0);
    }
    fn_801DA8C4(*r3,*p,p[1]);
    p = p + 2;
  }
  return;
}

/* Address: 0x8025DF38 | Size: 0x178 | Ghidra import */
u32 fn_8025DF38(u32* r3, u32 r4, u16* r5, int r6)
{
    extern u32 _fadeEffectGetRandom__FUl(u32);
    extern void GSmodelSetBoundCheck(void* model, s32 on);
    extern void GSmodelSetShadowLight(void* model, u32 light);
    extern void GSmodelSetShadowSurface(void* model, s32 count, u32* surfaces);
    extern void GSmodelClearShadowFlags(void* model, s32 flags);
    extern void GSmodelSetShadowFlags(void* model, s32 flags);
    extern u32 std092_getCurrentRoomID(void);
    extern u32 floorGetResource(u32 floor, u32 id);
    extern u32 floorDataBiosGetShadowReciveNum(void* floor);
    extern u32 floorDataBiosGetShadowReciveID(void* floor, u32 index);
    extern void* floorDataBiosGetCurrentPtr(void);
    extern u32 fn_8018F470(u32 index);
    extern u8 fn_801DDD28(u32 handle, u16 a, u16 b, s32 c);
    extern u32 fn_801DE190(u32 id, u32 random, s32 flags);
    extern void* fn_801DAC3C(u32 handle);
    s32 n;
    void* floor;
    u32 i;
    void* model;
    int k;
    u32 light;
    u32 surface;
    u32 surfaces[13];
    u32 count;
    u32 handle;
    u16* pair;

    handle = fn_801DE190(r4, _fadeEffectGetRandom__FUl(0xFFFFFFFF), 0);
    *r3 = handle;
    if (handle == 0) {
        return 0;
    }
    pair = r5;
    for (k = 0; k < r6; k++) {
        if (fn_801DDD28(*r3, pair[0], pair[1], 0) == 0) {
            return 0;
        }
        pair += 2;
    }
    if (r3 != NULL && *r3 != 0 && (model = fn_801DAC3C(*r3)) != NULL) {
        GSmodelClearShadowFlags(model, 1);
        floor = floorDataBiosGetCurrentPtr();
        if (floor != NULL) {
            light = fn_8018F470(1);
            count = floorDataBiosGetShadowReciveNum(floor);
            n = 0;
            if (count == 1) {
                for (i = 0; i < count; i++) {
                    surface = floorGetResource(std092_getCurrentRoomID(), floorDataBiosGetShadowReciveID(floor, i));
                    if (surface != 0) {
                        surfaces[n] = surface;
                        n++;
                    }
                }
                GSmodelSetShadowFlags(model, 1);
                GSmodelSetShadowLight(model, light);
                GSmodelSetShadowSurface(model, n, surfaces);
                GSmodelSetBoundCheck(model, 1);
            }
        }
    }
    return 1;
}

/* Address: 0x8025E0B0 | Size: 0x10C | Ghidra import */
u32 loadSequence(int *r3,u32 r4,u16 *r5,int r6)

{
    extern u32 pokemonBiosGetRnd();
  u32 uVar1;
  u32 uVar2;
  u16 sVar4;
  u16 sVar6;
  u32 iResult;
  int iVar3;
  u8 cVar5;

  sVar4 = (int)pokemonGetStatus(r4,0,0x6e,0);
  if (sVar4 == 0) {
    uVar1 = 0;
  }
  else {
    sVar6 = (int)pokemonGetStatus(0,sVar4,0x66,0);
    if (sVar6 == 0) {
      uVar1 = 0;
    }
    else {
      uVar1 = pokemonBiosGetRnd(r4);
      uVar2 = (int)pokemonGetStatus(r4,0,0xc1,0);
      uVar2 = (-uVar2 | uVar2) >> 0x1f;
      iResult = fn_801DE190(sVar6,uVar1,uVar2);
      *r3 = iResult;
      if (iResult == 0) {
        uVar1 = 0;
      }
      else {
        u16 *p = r5;
        for (iVar3 = 0; iVar3 < r6; iVar3 = iVar3 + 1) {
          cVar5 = fn_801DDD28(*r3,*p,p[1],0);
          if (cVar5 == '\0') {
            return 0;
          }
          p = p + 2;
        }
        uVar1 = 1;
      }
    }
  }
  return uVar1;
}

#if 0
asm void fn_8025E1BC(void) {
#include "src/game/colosseum_battle_fn_8025E1BC.inc"
}
#endif
static inline u32 *validptr_8025E1BC(u32 *p) {

  u32 *q = p;

  if (q != (u32 *)0 && *q != 0) return q;

  return (u32 *)0;

}

#pragma push
#pragma optimize_for_size off
void std244_spawnEffectAtPosition(float f1, float f2, float f3, float f4, float f5, float f6) {
  extern u32 lbl_8047B658;
  extern u16 lbl_80478DA0;
  extern f32 lbl_8047E678;
  extern f32 lbl_8047E67C;
  extern void fn_8025DDF4();
  extern u8 fn_8025DF38();
  extern void fn_8025DE0C();
  extern void fn_8025DDAC();
  extern void fn_8025DE54();
  extern void set__5GSvecFfff();
  u32 *r31;
  u32 *r30;
  float out_buf[3];
  float scale_buf[3];
  float sv1, sv2, sv3, sv4, sv5, sv6;
  float fscale;
  sv1 = f1; sv2 = f2; sv3 = f3;
  sv4 = f4; sv5 = f5; sv6 = f6;
  fn_801DADC0(1);
  fn_8025DDF4(&lbl_8047B658);
  if ((u8)(fn_8025DF38(&lbl_8047B658, 0x3f, &lbl_80478DA0, 1) == 1) == 1) {
    r31 = validptr_8025E1BC(&lbl_8047B658);
    r30 = validptr_8025E1BC(&lbl_8047B658);
    if (r30 != (u32 *)0 && *r30 != 0) {
      set__5GSvecFfff(sv1, out_buf, sv2, sv3);
      fn_8025DE0C(r30, (u32)out_buf);
    }
    {
      u32 *r3;
      r3 = validptr_8025E1BC(&lbl_8047B658);
      if (r3 != (u32 *)0 && *r3 != 0) {
        fscale = lbl_8047E678;
        scale_buf[0] = fscale * sv4;
        scale_buf[1] = fscale * sv5;
        scale_buf[2] = fscale * sv6;
        fn_8025DDAC(r3, (u32)scale_buf);
      }
    }
    if (r31 != (u32 *)0 && *r31 != 0) {
      fn_8025DE54(r31, &lbl_80478DA0, 1, 0, 0, 1);
      fadeSet(5, lbl_8047E67C);
      fadeCheck(1);
    }
  }
  wazaSequenceSysRelease();
}
#pragma pop

/* Address: 0x8025E534 | Size: 0xC0 | Ghidra import */
void _preReliveDisplayHokoraParticleThreadFunc__Fv(void)
{
    extern u32 lbl_8027A478[];
    extern u32 GSresGetResource();
    extern u32 std093_getCurrentGroupID();
    extern int fn_80118A68();
    extern int fn_80118F04();
    extern u32 gsParticleCreate();
    extern int waitTime__Ff();
    extern u32 lbl_8047B660;
    extern u32 lbl_8047B664;
    extern f32 lbl_8047E684;
    float fVar1;
    u32 uVar2;
    u32 state;
    u32 local_buf[3];

    local_buf[0] = lbl_8027A478[0];
    local_buf[1] = lbl_8027A478[1];
    local_buf[2] = lbl_8027A478[2];
    state = lbl_8047B664;
    if ((s32)state != 1) goto skip_cleanup;
    if ((s32)state != 1) goto skip_cleanup;
    fn_80118A68(lbl_8047B660, 1);
    lbl_8047B660 = 0;
    lbl_8047B664 = 0;
skip_cleanup:
    uVar2 = std093_getCurrentGroupID();
    uVar2 = GSresGetResource(uVar2, 0x108a1400);
    uVar2 = gsParticleCreate(uVar2, 0, 0);
    lbl_8047B660 = uVar2;
    fn_80118F04(uVar2, local_buf);
    fVar1 = lbl_8047E684;
    lbl_8047B664 = 1;
    waitTime__Ff(fVar1);
    if ((s32)lbl_8047B664 == 1) {
        fn_80118A68(lbl_8047B660, 1);
        lbl_8047B660 = 0;
        lbl_8047B664 = 0;
    }
}

/* Address: 0x8025E5F4 | Size: 0x4C | Ghidra import */
void preReliveDisplayHokoraParticle(void)
{
    extern int GSthreadCreate();
    extern u32 floorGetCurrentContext();
    extern void _preReliveDisplayHokoraParticleThreadFunc__Fv();
    extern u32 scriptGetDarkPointZeroPokemonNum();
  u32 uVar1;
  u32 uVar2;

  uVar1 = scriptGetDarkPointZeroPokemonNum();
  if ((uVar1 & 0xFFFF) != 0) {
    uVar2 = floorGetCurrentContext();
    GSthreadCreate(1, uVar2, 0x4000, 1, 1, (u32)_preReliveDisplayHokoraParticleThreadFunc__Fv);
  }
}

/* Address: 0x8025E640 | Size: 0x37C | Ghidra import */
typedef struct ReliveLevelUpStatus {
    u8 mode;
    u8 pad;
    s16 hp;
    s16 attack;
    s16 defense;
    s16 speed;
    s16 spAttack;
    s16 spDefense;
} ReliveLevelUpStatus;

int _cbWazaForget__FP7PokemonUsl(u32 r3, u32 r4);

u32 _expRecover__FP7PokemonUl(u32 r3,u32 r4)
{
    extern void winMsgCloseLevelUpStatus(s32);
    extern void winMsgOpenLevelUpStatus(ReliveLevelUpStatus*, s32);
    extern void winMsgOpenField(s32, s32, s32);
    extern void pokemonBiosSetExp(u32, u32);
    extern u32 pokemonBiosGetLevel(u32);
    extern void pokemonGetFriendFormPokemonFriendFilterId(u32, u32, s32);
    extern u32 pokemonGetLevelToExp(u32, u8);
    extern u32 pokemonGetSoubiItemSoubiDataId(u32);
    extern u32 pokemonGetOboeWazaDataId(u32, u32, u8*);
    extern s8 pokemonSearchWazaDataId(u32, u32);
    extern void pokemonWazaCreate(u32, u8, u32);
    extern void pokemonResetBasisStatus(u32);
    extern void msgctrlSetValue(s32, u32);
    extern void std130_soundUnk_801653cc(s32, s32, s32);
    u8 slot;
    u32 next;
    ReliveLevelUpStatus gain;
    s16 hp;
    s16 attack;
    s16 defense;
    u8 level;
    s16 spAttack;
    s16 spDefense;
    s16 speed;
    ReliveLevelUpStatus status;
    u32 newLevel;
    u32 levelUp;
    u32 exp;
    u8 wazaIndex;
    u32 waza;
    u32 item;

    levelUp = 0;
    wazaIndex = 0;
    pokemonGetStatus(r3, 0, 0x7A, 0);
    for (;;) {
        if (r4 == 0) {
            pokemonSetStatus(r3, 0, 0xC6, 0, r4);
            break;
        }
        level = (u8)pokemonGetStatus(r3, 0, 0x7A, 0);
        if (level >= 100) {
            break;
        }
        exp = (u32)pokemonGetStatus(r3, 0, 0x79, 0);
        next = pokemonGetLevelToExp(r3, level + 1);
        hp = (s16)pokemonGetStatus(r3, 0, 0x87, 0);
        attack = (s16)pokemonGetStatus(r3, 0, 0x88, 0);
        defense = (s16)pokemonGetStatus(r3, 0, 0x89, 0);
        spAttack = (s16)pokemonGetStatus(r3, 0, 0x8A, 0);
        spDefense = (s16)pokemonGetStatus(r3, 0, 0x8B, 0);
        speed = (s16)pokemonGetStatus(r3, 0, 0x8C, 0);
        exp += r4;
        if (exp >= next) {
        r4 = exp - next;
        levelUp = 1;
        pokemonBiosSetExp(r3, next);
        pokemonResetBasisStatus(r3);
        item = pokemonGetSoubiItemSoubiDataId(r3);
        pokemonGetFriendFormPokemonFriendFilterId(r3, item, 0);
        newLevel = pokemonBiosGetLevel(r3);
        std130_soundUnk_801653cc(0x4CA, 0, 0xFF);
        msgctrlSetValue(0x2F, (u8)newLevel);
        winMsgOpenField(0x44CE, 1, 0);
        status.hp = (s16)pokemonGetStatus(r3, 0, 0x87, 0);
        status.attack = (s16)pokemonGetStatus(r3, 0, 0x88, 0);
        status.defense = (s16)pokemonGetStatus(r3, 0, 0x89, 0);
        status.spAttack = (s16)pokemonGetStatus(r3, 0, 0x8A, 0);
        status.spDefense = (s16)pokemonGetStatus(r3, 0, 0x8B, 0);
        status.speed = (s16)pokemonGetStatus(r3, 0, 0x8C, 0);
        gain.hp = status.hp - hp;
        gain.attack = status.attack - attack;
        gain.defense = status.defense - defense;
        gain.spAttack = status.spAttack - spAttack;
        gain.spDefense = status.spDefense - spDefense;
        gain.speed = status.speed - speed;
        gain.mode = 1;
        winMsgOpenLevelUpStatus(&gain, 1);
        status.mode = 0;
        winMsgOpenLevelUpStatus(&status, 1);
        winMsgCloseLevelUpStatus(1);
        wazaIndex = 0;
        while ((u16)(waza = pokemonGetOboeWazaDataId(r3, newLevel, &wazaIndex)) != 0) {
            if (pokemonSearchWazaDataId(r3, waza) == -1 &&
                (s32)evolutionWazaLearn(r3, waza, &slot, 0, _cbWazaForget__FP7PokemonUsl, 0) != 0) {
                pokemonWazaCreate(r3, slot, waza);
            }
            wazaIndex++;
        }
        } else {
            r4 = 0;
            pokemonSetStatus(r3, 0, 0x79, 0, exp);
        }
    }
    return levelUp;
}

#pragma push
#pragma optimize_for_size off
/* Address: 0x8025EF58 | Size: 0x354 | Ghidra import */

typedef struct ReliveSelection {
    s32 mode;
    u32 index;
} ReliveSelection;

typedef struct ReliveTarget {
    s32 mode;
    s16 species;
    s16 pad;
    u32 index;
} ReliveTarget;

void preReliveMain(void)
{
    extern void fn_8025DCBC(int* param);
    extern s32 menuPokemonOpen(s32, s32, s32);
    extern void std098_mapWarpToMenuRoom(s32);
    extern void winMsgCloseField(s32);
    extern void winMsgOpenField(s32, s32, s32);
    extern void floorSetFadeScript(s32, s32);
    extern u16 pokemonBiosGetDarkpokemonDataId(void*);
    extern int heroItemCheckHaveItemDataId(s32, s32);
    extern void* heroGetStatus(s32, s32, u16);
    extern u8 fn_801EEC74(u16);
    extern void* heroBiosGetPokemonPtr(void*, u16);
    extern u8 pokemonCheckValid(void*);
    extern int pokemonIsDarkPokemon(void*);
    extern u32 scriptGetDarkPointZeroPokemonNum(void);
    extern void fn_80118A68(u32, s32);
    extern void std130_soundUnk_801653cc(s32, s32, s32);
    extern void _threadSwitch(void);
    extern u32 lbl_8047B660;
    extern u32 lbl_8047B664;
    extern f32 lbl_8047E680;
    extern f32 lbl_8047E68C;
    s16 species;
    u16 zeroNum;
    void* party;
    void* pokemon;
    u16 remaining;
    s32 i;
    ReliveSelection* selection;
    u16 mode;
    s32 selMode;
    u16 action;
    s32 picked;
    u8 work[24];

    if (((ReliveSelection*)&lbl_8047B668)->mode == -1) {
        return;
    }
    remaining = 0;
    savedataGetStatus(0, 0);
    party = (void*)savedataGetStatus(0, 2);
    for (i = 0; (u16)i < 6; i++) {
        pokemon = heroBiosGetPokemonPtr(party, (u16)i);
        if (pokemonCheckValid(pokemon) && (s32)(u8)pokemonIsDarkPokemon(pokemon) == 1 &&
            fn_801EEC74(pokemonBiosGetDarkpokemonDataId(pokemon)) == 0) {
            remaining++;
        }
    }
    zeroNum = scriptGetDarkPointZeroPokemonNum();
    savedataGetStatus(0, 0);
    savedataGetStatus(0, 2);
    if ((s32)(u8)heroItemCheckHaveItemDataId(0, 0x219) == 1) {
        if (remaining != 0) {
            if (zeroNum != 0) {
                mode = 2;
            } else {
                mode = 1;
            }
        } else {
            mode = 3;
        }
    } else if (zeroNum != 0) {
        mode = 4;
    } else {
        mode = 5;
    }
    selMode = ((ReliveSelection*)&lbl_8047B668)->mode;
    action = mode;
    if (selMode == 0) {
        if (mode == 1) {
            action = 5;
        } else if (mode == 2) {
            action = 4;
        } else if (mode == 3) {
            action = 5;
        }
    } else if (selMode == 1) {
        action = 2;
    }

    if (action == 2) {
        fn_8025DD14((int*)work);
        std130_soundUnk_801653cc(0x3C8, 0, 0xFF);
        selection = (ReliveSelection*)&lbl_8047B668;
        species = (s16)pokemonGetStatus(heroGetStatus(0, 3, selection->index), 0, 0x6E, 0);
        ((ReliveTarget*)lbl_804782BC)->mode = 1;
        ((ReliveTarget*)lbl_804782BC)->species = species;
        ((ReliveTarget*)lbl_804782BC)->index = selection->index;
        fadeSet(3, lbl_8047E680);
        fadeCheck(1);
        std098_mapWarpToMenuRoom(0x385);
        floorSetFadeScript(0, 0);
        _threadSwitch();
        fadeSet(2, lbl_8047E68C);
        fadeCheck(1);
        fn_8025DCBC((int*)work);
    } else if (action == 4) {
        if ((s32)lbl_8047B664 == 1) {
            fn_80118A68(lbl_8047B660, 1);
            lbl_8047B660 = 0;
            lbl_8047B664 = 0;
        }
        if ((u16)scriptGetDarkPointZeroPokemonNum() == 0) {
            lbl_8047B668 = 0xFFFFFFFF;
            ((u32*)&lbl_8047B668)[1] = 0xFFFFFFFF;
        } else {
            winMsgOpenField(0x3B0F, 1, 0);
            winMsgCloseField(1);
            picked = menuPokemonOpen(7, 0, 0);
            if (picked != -1) {
                species = (s16)pokemonGetStatus(heroGetStatus(0, 3, picked), 0, 0x6E, 0);
                ((ReliveTarget*)lbl_804782BC)->mode = 0;
                ((ReliveTarget*)lbl_804782BC)->species = species;
                ((ReliveTarget*)lbl_804782BC)->index = picked;
                fadeSet(3, lbl_8047E680);
                fadeCheck(1);
                fn_8025DD14((int*)work);
                std098_mapWarpToMenuRoom(0x385);
                floorSetFadeScript(0, 0);
                _threadSwitch();
                fadeSet(2, lbl_8047E68C);
                fadeCheck(1);
                fn_8025DCBC((int*)work);
            }
        }
    } else {
        lbl_8047B668 = 0xFFFFFFFF;
        ((u32*)&lbl_8047B668)[1] = 0xFFFFFFFF;
    }
}
#pragma pop

#if 0
asm void preReliveInit(void) {
#include "src/game/colosseum_battle_fn_8025F2AC.inc"
}
#else
void preReliveInit(void) {
  u32 *p = &lbl_8047B668;
  p[0] = 0xFFFFFFFF;
  p[1] = 0xFFFFFFFF;
}
#endif

/* Address: 0x8025F2C0 | Size: 0x3C | Ghidra import */
void preReliveSetParameter(int r3,u32 r4)

{
    extern u32 lbl_8047B668;
    u32 *p = &lbl_8047B668;
    p[0] = r3;
    p[1] = r4;
  if ((r3 == 0) || (r3 == 1)) {
    preReliveMain();
  }
  return;
}
#pragma push
#pragma optimize_for_size off
/* Address: 0x8025E3B0 | Size: 0x184 | Ghidra import */

int _cbWazaForget__FP7PokemonUsl(u32 r3,u32 r4)

{
    extern u8 lbl_80478288[];
    extern u32 lbl_80478DB0;
    extern const f32 lbl_8047E680;

  u32 uVar1;
  int iVar2;
  int iVar3;
  u32 *snapshot;

  u32 view[3];
  u32 position[3];
  u32 rotation[3];
  u32 direction[3];
  u8 auStack_48 [4];
  u8 auStack_4c [4];
  u8 auStack_50 [4];
  f32 local_54;
  u8 auStack_58 [4];
  
  fadeSet((double)lbl_8047E680,3);
  fadeCheck(1);
  GSscene_GetCameraDirectionVector(direction);
  GSscene_GetCameraRotationVector(rotation);
  GSscene_GetCameraPositionVector(position);
  GSscene_GetCameraViewVector(view);
  uVar1 = GScameraGetActiveCamera();
  GScameraGetPerspective(uVar1,&local_54,auStack_50,auStack_4c,auStack_48);
  snapshot = (u32 *)lbl_80478288;
  snapshot[0] = direction[0];
  snapshot[1] = direction[1];
  snapshot[2] = direction[2];
  snapshot[3] = rotation[0];
  snapshot[4] = rotation[1];
  snapshot[5] = rotation[2];
  snapshot[6] = position[0];
  snapshot[7] = position[1];
  snapshot[8] = position[2];
  snapshot[9] = view[0];
  snapshot[10] = view[1];
  snapshot[11] = view[2];
  ((f32 *)snapshot)[12] = local_54;
  wazaSequenceSysRelease();
  iVar2 = fn_80097A38(r3,r4);
  if (iVar2 >= 4) {
    iVar2 = -1;
  }
  pokemonGetStatus(r3,0,0x6e,0);
  fn_801DADC0(1);
  iVar3 = loadSequence((int*)auStack_58,r3,(u16*)&lbl_80478DB0,1);
  if (iVar3 == 1) {
    fadeSet((double)lbl_8047E680,2);
    fn_8025DE54((u32*)auStack_58,(u16*)&lbl_80478DB0,1,1,1,0);
  }
  return (int)(signed char)iVar2;
}
#pragma pop
#pragma push
#pragma optimize_for_size off
/* Address: 0x8025E9BC | Size: 0x390 | Ghidra import */
void reliveCeremonyAll(u32 r3)

{
    extern s8 std011_dialogDisplayYesNoMenu();
    extern int menuNameEntryOpenNoFade();
    extern int pokemonBiosSetFriend();
    extern int pokemonBiosSetPoolFriend();
    extern u16 pokemonBiosGetPoolFriend();
    extern u32 pokemonBiosGetPoolExp();
    extern u16 pokemonBiosGetFriend();
    extern u16 pokemonBiosGetPokemonWazaDataId();
    extern u32 pokemonBiosGetNicknamePtr();
    extern int pokemonSetDp();
    extern u8 fn_80121ADC();
    extern int fn_80121B4C();
    extern int pokemonEvolutionAll();
    extern u32 pokemonEvolutionCheck();
    extern u8 pokemonCheckValid();
    extern int fn_801EECD8();
    extern u32 lbl_80478DB0;
    extern f32 lbl_8047E680;
    extern f32 lbl_8047E688;

  u32 uVar1;
  s8 cVar7;
  u8 uVar8;
  u32 uVar2;
  u16 sVar5;
  u16 sVar6;
  u16 sVar7;
  int iVar3;
  u32 uVar4;
  u32 evo;
  u32 nick;

  u8 auStack_18 [8];
  u8 auStack_1c [4];
  u8 auStack_20 [4];
  u8 auStack_24 [4];
  u16 local_28 [2];

  local_28[0] = 0;
  uVar1 = heroGetStatus(0,3,r3 & 0xffff);
  uVar8 = pokemonCheckValid();
  if (uVar8 == 1) {
    pokemonGetStatus(uVar1,0,0x6e,0);
    uVar2 = pokemonBiosGetDarkpokemonDataId(uVar1);
    fn_801EECD8(uVar2,1);
    uVar8 = fn_80121ADC(uVar1,0x3e);
    if (uVar8 != 0) {
      fn_80121B4C(uVar1,0x3e);
    }
    pokemonSetDp((double)lbl_8047E688,uVar1);
    std130_soundUnk_801653cc(0x3f7,0,0xff);
    sVar5 = pokemonBiosGetPokemonWazaDataId(uVar1,0);
    if (sVar5 != 0) {
      nick = pokemonBiosGetNicknamePtr(uVar1);
      msgctrlSetValue(0x32,nick);
      msgctrlSetValue(0x39,sVar5);
      winMsgOpenField(0x3b10,1,0);
    }
    uVar4 = pokemonBiosGetPoolExp(uVar1);
    uVar2 = uVar4;
    msgctrlSetValue(0x2f,uVar2);
    winMsgOpenField(0x3b0b,1,0);
    winMsgCloseField(1);
    sVar7 = pokemonBiosGetFriend(uVar1);
    pokemonBiosSetFriend(uVar1,(u16)(sVar7 + pokemonBiosGetPoolFriend(uVar1) + 0x46));
    pokemonBiosSetPoolFriend(uVar1,0);
    iVar3 = _expRecover__FP7PokemonUl(uVar1,uVar2);
    if (iVar3 == 1) {
      evo = pokemonEvolutionCheck(uVar1,0,0,local_28,auStack_18);
      if (((evo & 0xffff) != 0) && ((evo & 0xffff) != 0xffff)) {
        fadeSet((double)lbl_8047E680,3);
        fadeCheck(1);
        wazaSequenceSysRelease();
        iVar3 = pokemonEvolutionAll(uVar1,evo,local_28[0],auStack_18,0,1,1,0);
        if (iVar3 == 0) {
          fn_801DADC0(1);
          iVar3 = loadSequence((int*)auStack_1c,uVar1,(u16*)&lbl_80478DB0,1);
          if (iVar3 == 1) {
            fadeSet((double)lbl_8047E680,2);
            fn_8025DE54((u32*)auStack_1c,(u16*)&lbl_80478DB0,1,1,1,0);
          }
          uVar2 = pokemonBiosGetNicknamePtr(uVar1);
          msgctrlSetValue(0x32,uVar2);
        }
        else {
          uVar2 = pokemonBiosGetNicknamePtr(uVar1);
          msgctrlSetValue(0x32,uVar2);
          fn_801DADC0(1);
          iVar3 = loadSequence((int*)auStack_20,uVar1,(u16*)&lbl_80478DB0,1);
          if (iVar3 == 1) {
            fadeSet((double)lbl_8047E680,2);
            fn_8025DE54((u32*)auStack_20,(u16*)&lbl_80478DB0,1,1,1,0);
          }
        }
      }
    }
    std130_soundUnk_801653cc(0x3ca,0,0xff);
    exribbonSetNarionalRibbon(uVar1);
    winMsgOpenField(0x3b0c,1,0);
    winMsgOpenField(0x3b0d,1,1);
    cVar7 = std011_dialogDisplayYesNoMenu();
    if (cVar7 != '\0') {
      winMsgCloseField(1);
    }
    else {
      winMsgCloseField(1);
      fadeSet((double)lbl_8047E680,3);
      fadeCheck(1);
      wazaSequenceSysRelease();
      menuNameEntryOpenNoFade(2,r3);
      fn_801DADC0(1);
      iVar3 = loadSequence((int*)auStack_24,uVar1,(u16*)&lbl_80478DB0,1);
      if (iVar3 == 1) {
        fadeSet((double)lbl_8047E680,2);
        fn_8025DE54((u32*)auStack_24,(u16*)&lbl_80478DB0,1,1,1,0);
      }
    }
  }
  return;
}
#pragma pop

#pragma push
#pragma optimize_for_size off
/* Address: 0x8025ED4C | Ghidra import */

void reliveMain(void)

{
    extern u32 lbl_80478DA8;
    extern u32 lbl_8047B668;
    extern u32 lbl_8047B66C;
    extern f32 lbl_8047E680;
  BOOL bVar1;

		  u32 uVar2;
		  u8 cVar4;
		  int iVar3;

  u8 auStack_18 [16];
  
  if ((lbl_804782BC[0] == 0) || (lbl_804782BC[0] == 1)) {
    fn_801DADC0(1);
    if (lbl_804782BC[0] == 0) {
      uVar2 = heroGetStatus(0,3,lbl_804782BC[2] & 0xffff);
      cVar4 = pokemonCheckValid();
      if (cVar4 != '\x01') {
        bVar1 = 0;
      }
      else {
        pokemonGetStatus(uVar2,0,0x6e,0);
        iVar3 = loadSequence((int*)auStack_18,uVar2,(u16*)&lbl_80478DA8,2);
        if (iVar3 == 1) {
          fn_8025DE54((u32*)auStack_18,(u16*)&lbl_80478DA8,2,1,1,0);
          bVar1 = 1;
        }
        else {
          bVar1 = 0;
        }
      }
      if (bVar1 == 1) {
        reliveCeremonyAll(lbl_804782BC[2]);
      }
    }
    else if (lbl_804782BC[0] == 1) {
      uVar2 = heroGetStatus(0,3,lbl_804782BC[2] & 0xffff);
      cVar4 = pokemonCheckValid();
      if (cVar4 != '\x01') {
        bVar1 = 0;
      }
      else {
        pokemonSetStatus(uVar2,0,0xc5,0,0);
        pokemonGetStatus(uVar2,0,0x6e,0);
        iVar3 = loadSequence((int*)auStack_18,uVar2,(u16*)lbl_8039A6A8,4);
        if (iVar3 == 1) {
          fn_8025DE54((u32*)auStack_18,(u16*)lbl_8039A6A8,4,1,1,0);
          bVar1 = 1;
        }
        else {
          bVar1 = 0;
        }
      }
      if (bVar1 == 1) {
        reliveCeremonyAll(lbl_804782BC[2]);
      }
    }
    fadeSet((double)lbl_8047E680,3);
    fadeCheck(1);
				    wazaSequenceSysRelease();
						    lbl_804782BC[0] = -1;
						    lbl_8047B668 = 0xffffffff;
						    lbl_8047B66C = 0xffffffff;
				    *(u16 *)((u8 *)lbl_804782BC + 4) = 0;
    lbl_804782BC[2] = 0;
    fn_800FF660();
    floorSetFadeScript(0,0);
    _threadSwitch();
  }
  return;
}
#pragma pop


/* Address: 0x8025DC2C | Size: 0x90 | Ghidra import (PSQ removed) */


void waitTime__Ff(float r3)

{
    extern f32 lbl_8047E660;
    extern f32 lbl_8047E664;
    extern f64 lbl_8047E668;
  u32 uVar2;

  double dVar4;
  float fVar1;

  dVar4 = (double)(float)(lbl_8047E664 * r3);
  fVar1 = lbl_8047E660;
  while (fVar1 < dVar4) {
    _threadSwitch();
    uVar2 = GSgetFrameStep();
    fVar1 += (float)uVar2;
  }

  return;
}
