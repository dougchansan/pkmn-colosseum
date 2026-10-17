/**
 * @file evolution.c
 * @brief game/pxdvs/app/evolution/evolution.cpp -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x802600E4-0x80261388, 7 fns.
 *
 * XD source unit: game/pxdvs/app/evolution/evolution.cpp
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
extern u32 fn_801DE190(u16 index, u32 rnd, u32 variant);
extern void fn_801DA9E8(u32 sequence, u16 moveID, s32 variant);
extern u8 fn_801DA94C(u32 sequence, u16 moveID, s32 variant);
extern void fn_801DA8C4(u32 sequence, u16 moveID, s32 variant);

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
extern void fadeSet(f32 duration, u32 mode);
extern int  wazaSequenceSysRelease();
extern int  fn_801DADC0();
extern void OSRegisterResetFunction();
extern void OSInitAlarm();
extern void OSInitThreadQueue();
extern void* memcpy();

/* Forward declarations for converted functions */
int evolutionWazaLearn(u8 *r3, u32 r4, u8 *r5, int r6, void *r7, u32 r8);
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3);
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2);

typedef struct EvoWazaSeq {
    u16 id;
    int kind;
} EvoWazaSeq;

extern EvoWazaSeq lbl_8027A488[];
extern u8 fn_801DDD28();

static inline u32 evolutionCreateModel(u32 pkm, int kind)
{
  u16 no;
  u32 shiny;
  EvoWazaSeq *seq;
  u32 rnd;
  int i;
  u32 model;

  no = (int)pokemonGetStatus(pkm,0,0x6e,0);
  if (no == 0) {
    no = 0xffff;
  }
  else {
    no = (int)pokemonGetStatus(0,no,0x66,0);
    if (no == 0) {
      no = 0xffff;
    }
  }
  if (no == 0xffff) {
    return 0;
  }
  rnd = pokemonBiosGetRnd(pkm);
  shiny = (int)pokemonGetStatus(pkm,0,0xc1,0);
  shiny = (-shiny | shiny) >> 0x1f;
  model = fn_801DE190(no,rnd,shiny);
  if (model == 0) {
    return 0;
  }
  i = 0;
  seq = lbl_8027A488;
  do {
    if (seq->kind == kind && fn_801DDD28(model,seq->id,4,0) == 0) {
      break;
    }
    i++;
    seq++;
  } while (i < 5);
  if (i < 5) {
    return 0;
  }
  return model;
}

/* Address: 0x8026045C | Size: 0x27C | Ghidra import */
int cbWazaForget(u32 r3,u32 r4,int r5)

{
    extern int fn_80097A38();
    extern int GScameraGetPerspective();
    extern u32 GScameraGetActiveCamera();
    extern int GSmodelSetAnimType();
    extern int fn_801766A8();
    extern int GSscene_GetCameraRotationVector();
    extern int GSscene_SetCameraRotationVector();
    extern int GSscene_GetCameraDirectionVector();
    extern int GSscene_SetCameraDirectionVector();
    extern int GSscene_GetCameraPositionVector();
    extern int GSscene_SetCameraPositionVector();
    extern int GSscene_GetCameraViewVector();
    extern int GSscene_SetCameraViewVector();
    extern int GSscene_SetMode();
    extern int wazaSequenceSysRelease();
    extern int fn_801DADC0();
    extern const f32 lbl_8047E6C0;

  u32 uVar1;
  u32 *savedRotationPtr;
  u32 *savedPositionPtr;
  u32 *savedViewPtr;
  int iVar2;
  u32 iVar7;
  u32 savedView[4];
  u32 savedPosition[3];
  u32 savedRotation[3];
  u32 savedDirection[3];
  u32 view[3];
  u32 position[3];
  u32 rotation[3];
  u32 direction[3];
  u8 auStack_8c [4];
  u8 auStack_90 [4];
  u8 auStack_94 [4];
  float local_98;
  
  fadeSet(lbl_8047E6C0,3);
  fadeCheck(1);
  GSscene_GetCameraDirectionVector(direction);
  GSscene_GetCameraRotationVector(rotation);
  GSscene_GetCameraPositionVector(position);
  GSscene_GetCameraViewVector(view);
  uVar1 = GScameraGetActiveCamera();
  GScameraGetPerspective(uVar1,&local_98,auStack_94,auStack_90,auStack_8c);
  savedDirection[0] = direction[0];
  savedDirection[1] = direction[1];
  savedDirection[2] = direction[2];
  savedRotationPtr = savedRotation;
  savedPositionPtr = savedPosition;
  savedViewPtr = savedView;
  savedRotation[0] = rotation[0];
  savedRotation[1] = rotation[1];
  savedRotation[2] = rotation[2];
  savedPosition[0] = position[0];
  savedPosition[1] = position[1];
  savedPosition[2] = position[2];
  savedView[0] = view[0];
  savedView[1] = view[1];
  savedView[2] = view[2];
  ((volatile float *)savedView)[3] = local_98;
  wazaSequenceSysRelease();
  iVar2 = fn_80097A38(r3,r4);
  if (iVar2 >= 4) {
    iVar2 = -1;
  }
  fn_801DADC0(2);
  iVar7 = evolutionCreateModel(r3,1);
  if (iVar7 != 0) {
    *(int *)(r5 + 4) = iVar7;
  }
  uVar1 = fn_801DAC3C(*(u32 *)(r5 + 4));
  GSmodelSetAnimType(uVar1,1);
  fn_801DA4E8(*(u32 *)(r5 + 4),1);
  GSscene_SetMode(2);
  GSscene_SetCameraDirectionVector(savedDirection);
  GSscene_SetCameraRotationVector(savedRotationPtr);
  GSscene_SetCameraPositionVector(savedPositionPtr);
  GSscene_SetCameraViewVector(savedViewPtr);
  cameraSetFov((double)((volatile float *)savedView)[3]);
  fadeSet(lbl_8047E6C0,2);
  fadeCheck(1);
  return (int)(signed char)iVar2;
}

/* Address: 0x802606D8 | Size: 0x238 | Ghidra import */
int doWazaSequence(u32 *r3,int r4,int r5,u32 r6)

{
    extern int msgGetCharsPerFrame();
    extern u32 fn_800F7AF0();
    extern u32 fn_800F7BC4();
    extern const f32 lbl_8047E6C0;
  u32 *savedRotationPtr;
  u32 *savedPositionPtr;
  u32 *savedViewPtr;
  u16 uVar1;
  u32 uVar9;
  int iVar8;
  BOOL bVar2;
  u8 cVar7;
  int iVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  u32 savedView[4];
  u32 savedPosition[3];
  u32 savedRotation[3];
  u32 savedDirection[3];
  u32 view[3];
  u32 position[3];
  u32 rotation[3];
  u32 direction[3];
  u8 auStack_9c [4];
  u8 auStack_a0 [4];
  u8 auStack_a4 [4];
  float local_a8;
  
  iVar8 = 0;
  bVar2 = 0;
  if ((r4 < 0) || (r4 >= 5)) {
    iVar8 = 0;
  }
  else {
    uVar1 = lbl_8027A488[r4].id;
    if (lbl_8027A488[r4].kind != 0) {
      uVar9 = r3[1];
    }
    else {
      uVar9 = *r3;
    }
    fn_801DA9E8(uVar9,uVar1,4);
    savedRotationPtr = savedRotation;
    savedPositionPtr = savedPosition;
    savedViewPtr = savedView;
    while (!bVar2) {
      fn_801DB088();
      if ((r6 & 10) != 0) {
        uVar6 = 4;
        if ((r6 & 2) != 0) {
          uVar6 = 2;
        }
        fadeSet(lbl_8047E6C0,uVar6);
        r6 = r6 & 0xfffffff5;
      }
      cVar7 = fn_801DA94C(uVar9,uVar1,4);
      if (cVar7 == '\0') break;
      _threadSwitch();
      GSscene_GetCameraDirectionVector(direction);
      GSscene_GetCameraRotationVector(rotation);
      GSscene_GetCameraPositionVector(position);
      GSscene_GetCameraViewVector(view);
      uVar6 = GScameraGetActiveCamera();
      GScameraGetPerspective(uVar6,&local_a8,auStack_a4,auStack_a0,auStack_9c);
      savedDirection[0] = direction[0];
      savedDirection[1] = direction[1];
      savedDirection[2] = direction[2];
      savedRotation[0] = rotation[0];
      savedRotation[1] = rotation[1];
      savedRotation[2] = rotation[2];
      savedPosition[0] = position[0];
      savedPosition[1] = position[1];
      savedPosition[2] = position[2];
      savedView[0] = view[0];
      savedView[1] = view[1];
      savedView[2] = view[2];
      ((volatile float *)savedView)[3] = local_a8;
      iVar3 = msgGetCharsPerFrame();
      iVar8 = iVar8 + iVar3;
      if (r5 != 0) {
        uVar4 = fn_800F7AF0(1);
        uVar5 = fn_800F7BC4(1);
        if ((uVar5 & uVar4 & 0x200) != 0) {
          bVar2 = 1;
        }
      }
    }
    GSscene_SetMode(2);
    GSscene_SetCameraDirectionVector(savedDirection);
    GSscene_SetCameraRotationVector(savedRotationPtr);
    GSscene_SetCameraPositionVector(savedPositionPtr);
    GSscene_SetCameraViewVector(savedViewPtr);
    cameraSetFov((double)((volatile float *)savedView)[3]);
    if ((r6 & 0x14) != 0) {
      uVar6 = 5;
      if ((r6 & 4) != 0) {
        uVar6 = 3;
      }
      fadeSet(lbl_8047E6C0,uVar6);
      fadeCheck(1);
    }
    fn_801DA8C4(uVar9,uVar1,4);
    if (bVar2 != 0) {
      return iVar8;
    }
    iVar8 = -1;
  }
  return iVar8;
}

typedef struct EvoWork {
    u32 model[2];
    u32 seq;
    u32 bgm;
    u32 se;
    u32 bgmVolume;
    u32 seVolume;
} EvoWork;

static inline u8 evolutionSetModel(u32 pkm, int kind, u32 *out)
{
  u32 model;

  model = evolutionCreateModel(pkm,kind);
  if (model == 0) {
    return 0;
  }
  *out = model;
  return 1;
}

static inline BOOL evolutionSetModels(u32 *models, u32 before, u32 after)
{
  if (evolutionSetModel(before,0,&models[0]) && evolutionSetModel(after,1,&models[1])) {
    return 1;
  }
  wazaSequenceSysRelease();
  return 0;
}

static inline BOOL evolutionRun(EvoWork *work, u32 r5, u32 before, u32 after,
                                u16 *wazaList, int wazaNum, u8 *slotList)
{
    extern int pokemonBiosCopy();
    extern int scriptSoundStop();
    extern int evolutionDemo();
    extern const f32 lbl_8047E6C0;
  u32 bgmVolume;
  u32 seVolume;
  u32 bgm;
  u32 se;
  int i;
  u16 waza;
  int result;
  u8 slot;
  u8 pokemon[320];
  u16 *wp = wazaList;
  u8 *sp = slotList;

  bgm = std228_soundGetCurrentBackgroundMusic();
  if (bgm != 0) {
    bgmVolume = fn_801656D8();
    std104_soundSetBackgroundMusic(1,0x32,0xff);
  }
  else {
    bgmVolume = 0;
  }
  se = fn_801653BC();
  if (se != 0) {
    seVolume = fn_801656D8();
    scriptSoundStop(0x32);
  }
  else {
    seVolume = 0;
  }
  work->bgm = bgm;
  work->se = se;
  work->bgmVolume = bgmVolume;
  work->seVolume = seVolume;
  result = evolutionDemo(work,r5,before,after);
  if (work->bgm != 0) {
    std104_soundSetBackgroundMusic(work->bgm,0x32,(u8)work->bgmVolume);
  }
  if (work->se != 0) {
    std105_soundPlayEffect(work->se,0x32,(u8)work->seVolume);
  }
  if (result == 0) {
    return FALSE;
  }
  pokemonBiosCopy(pokemon,after);
  for (i = 0; i < wazaNum; i++) {
    waza = *wp;
    if (evolutionWazaLearn(pokemon,waza,&slot,0,cbWazaForget,(u32)work) != 0) {
      pokemonWazaCreate(pokemon,slot,waza);
    }
    else {
      slot = 0xff;
    }
    wp++;
    *sp = slot;
    sp++;
  }
  fadeSet(lbl_8047E6C0,3);
  fadeCheck(1);
  return TRUE;
}

/* Address: 0x80260EBC | Size: 0x414 | Ghidra import */
u32
evolutionStart(u32 r3,u32 r4,u32 r5,u16 *r6,
            int r7,u8 *r8)

{
  BOOL ok;
  EvoWork work;

  fn_801DADC0(2);
  work.model[0] = 0;
  work.model[1] = 0;
  if (!evolutionSetModels(work.model,r3,r4)) {
    return 2;
  }
  ok = evolutionRun(&work,r5,r3,r4,r6,r7,r8);
  wazaSequenceSysRelease();
  if (ok) {
    return 0;
  }
  return 1;
}

/* Address: 0x8026132C | Size: 0x5C | Ghidra import */
u32 evolutionOpen(u32 r3, u32 r4, u32 r5, u16 *r6, u32 r7, u8 *r8)
{
    extern u32 lbl_804787E0[];
    extern void std098_mapWarpToMenuRoom(u32);
    extern void floorSetFadeScript(u32, u32);
    extern void _threadSwitch(void);
    u32 *base;

    base = lbl_804787E0;
    base[0] = r3;
    base[1] = r4;
    base[2] = r5;
    base[3] = r7;
    base[4] = (u32)r6;
    base[5] = (u32)r8;
    std098_mapWarpToMenuRoom(0x386);
    floorSetFadeScript(0, 0);
    _threadSwitch();
    return base[6];
}

/* Address: 0x802612D0 | Size: 0x5C | Ghidra import */

void evolution(void)
{
    extern u32 lbl_804787E0[];
    extern u32 evolutionStart();
    extern void fn_800FF660();
    extern void floorSetFadeScript();
    u32 *base = lbl_804787E0;
    base[6] = evolutionStart(base[0], base[1], base[2], (u16*)base[4], base[3], (u8*)base[5]);
    fn_800FF660();
    floorSetFadeScript(0, 0);
}

/* Address: 0x802600E4 | Size: 0x378 | Ghidra import */
extern s8 menuSubOpenYesNo();
extern int winMsgClose();
extern int winMsgOpen();

static inline int evolutionAskYesNo(int r6, u32 msg)
{
  int sel;

  if (r6 == 0) {
    winMsgOpenField(msg,1,0);
  }
  else {
    winMsgOpen(2,msg,1,0);
  }
  if (r6 == 0) {
    sel = (s8)std011_dialogDisplayYesNoMenu();
  }
  else {
    sel = menuSubOpenYesNo(0,0xffffffff,0xffffffff,0);
  }
  if (r6 == 0) {
    winMsgCloseField(1);
  }
  else {
    winMsgClose(1);
  }
  switch (sel) {
  case 0:
    return 0;
  case 1:
    return 1;
  default:
    return 2;
  }
}

int
evolutionWazaLearn(u8 *r3,u32 r4,u8 *r5,int r6,void *r7,
            u32 r8)

{
  int slot;
  u32 uVar2;
  u32 waza;
  u16 sVar3;
  u16 uVar4;

  slot = 0;
  do {
    sVar3 = pokemonBiosGetPokemonWazaDataId(r3,slot & 0xffff);
    if (sVar3 == 0) break;
    slot = slot + 1;
  } while (slot < 4);
  if (slot < 4) goto LAB_0025d3c4;

  uVar2 = pokemonBiosGetNicknamePtr(r3);
  msgctrlSetValue(0x32,uVar2);
  waza = r4 & 0xffff;
  msgctrlSetValue(0x39,waza);
  do {
    if (evolutionAskYesNo(r6,0x4243) == 0) {
      if (r7 != (void *)0x0) {
        slot = ((s8 (*)())r7)(r3,r4,r8);
      }
      else {
        slot = 0;
      }
      if (0 <= slot) goto LAB_waza_set;
    }
    msgctrlSetValue(0x32,uVar2);
    msgctrlSetValue(0x39,waza);
  } while (evolutionAskYesNo(r6,0x4242) != 0);
  if (r6 == 0) {
    winMsgOpenField(0x4241,1,0);
  }
  else {
    winMsgOpen(2,0x4241,1,0);
  }
  if (r6 == 0) {
    winMsgCloseField(1);
  }
  else {
    winMsgClose(1);
  }
  return 0;

LAB_waza_set:
  uVar2 = pokemonBiosGetNicknamePtr(r3);
  msgctrlSetValue(0x32,uVar2);
  msgctrlSetValue(0x5d,0x468);
  uVar4 = pokemonBiosGetPokemonWazaDataId(r3,slot & 0xffff);
  msgctrlSetValue(0x39,uVar4);
  if (r6 == 0) {
    winMsgOpenField(0x4248,1,0);
  }
  else {
    winMsgOpen(2,0x4248,1,0);
  }
LAB_0025d3c4:
  std130_soundUnk_801653cc(0x4ca,0,0xff);
  uVar2 = pokemonBiosGetNicknamePtr(r3);
  msgctrlSetValue(0x32,uVar2);
  msgctrlSetValue(0x39,r4 & 0xffff);
  if (r6 == 0) {
    winMsgOpenField(0x423d,1,0);
  }
  else {
    winMsgOpen(2,0x423d,1,0);
  }
  if (r6 == 0) {
    winMsgCloseField(1);
  }
  else {
    winMsgClose(1);
  }
  *r5 = (char)slot;
  return 1;
}

extern u8 pokemonCheckValid();
extern u32 pokemonDataBiosGetPtr(u16 id);
extern u16 pokemonDataBiosGetVoice();

static inline u32 evolutionGetPokemonData(u32 pkm)
{
  if (pkm == 0) {
    return 0;
  }
  if (!pokemonCheckValid(pkm)) {
    return 0;
  }
  return pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(pkm));
}

static inline u32 evolutionPlayVoice(u32 pkm)
{
  u32 data;
  u32 voice;

  data = evolutionGetPokemonData(pkm);
  if (data == 0) {
    voice = 0;
  }
  else {
    voice = pokemonDataBiosGetVoice(data);
    soundStartSE(voice);
  }
  return voice;
}

static inline u32 evolutionGetSeqModel(EvoWork *work, int seq)
{
  if (lbl_8027A488[seq].kind != 0) {
    return work->model[1];
  }
  return work->model[0];
}

typedef struct EvoSkipSeq {
    u32 time;
    int seq;
} EvoSkipSeq;

static inline void evolutionSeqStart(EvoWork *work, int seq)
{
  u16 id;

  id = lbl_8027A488[seq].id;
  fn_801DA9E8(evolutionGetSeqModel(work,seq),id,4);
  fn_801DB088();
  work->seq = seq;
}

static inline BOOL evolutionSeqUpdate(EvoWork *work)
{
  u32 model;
  u16 id;
  u32 playing;

  model = evolutionGetSeqModel(work,work->seq);
  id = lbl_8027A488[work->seq].id;
  fn_801DB088();
  playing = fn_801DA94C(model,id,4);
  return (-playing | playing) >> 31;
}

/* Address: 0x80260910 | Size: 0x5AC */
u32 evolutionDemo(EvoWork *work,int canSkip,u32 before,u32 after)
{
    extern EvoSkipSeq lbl_8027A4B0[];
    extern const f32 lbl_8047E6B0;
    extern const f32 lbl_8047E6B4;
    extern const f32 lbl_8047E6C0;
    extern const f32 lbl_8047E6C4;
    extern int msgGetCharsPerFrame();
    extern u32 fn_800F7AF0();
    extern u32 fn_800F7BC4();
  u32 voice;
  u32 nickname;
  u16 dataId;
  u32 trigger;
  u32 held;
  BOOL playing;
  BOOL first;
  int state;
  u32 time;
  BOOL skipped;
  int wait;
  int level;
  f32 t;

  fn_801DA4E8(work->model[0],1);
  nickname = pokemonBiosGetNicknamePtr(before);
  msgctrlSetValue(0x32,nickname);
  winMsgOpenField(0x4401,1,0);
  voice = evolutionPlayVoice(before);
  evolutionSeqStart(work,0);
  first = TRUE;
  skipped = FALSE;
  state = 0;
  time = 0;
  while (time < 0x23a) {
    if (canSkip != 0 && time >= 0x78 && state >= 2) {
      trigger = fn_800F7AF0(1);
      held = fn_800F7BC4(1);
      if ((held & trigger & 0x200) != 0) {
        skipped = TRUE;
        break;
      }
    }
    playing = evolutionSeqUpdate(work);
    if (!playing) {
      break;
    }
    switch (state) {
    case 0:
      if (GSsndGetStatus(voice) != 2) {
        std104_soundSetBackgroundMusic(0x3d3,0,0xff);
        wait = 0;
        state = 1;
      }
      break;
    case 1:
      if (GSsndGetStatus(0x3d3) != 2) {
        wait += msgGetCharsPerFrame();
        if (wait >= 0x1e) {
          std104_soundSetBackgroundMusic(0x3d4,0,0xff);
          state = 2;
        }
      }
      break;
    }
    if (first) {
      fadeSet(lbl_8047E6C0,2);
      first = FALSE;
    }
    _threadSwitch();
    time += msgGetCharsPerFrame();
  }
  fadeSet(lbl_8047E6C0,5);
  while ((s8)fadeCheck(0) != 0) {
    evolutionSeqUpdate(work);
    _threadSwitch();
  }
  fn_801DA8C4(evolutionGetSeqModel(work,work->seq),lbl_8027A488[work->seq].id,4);
  t = lbl_8047E6B0;
  while (t < lbl_8047E6B4) {
    _threadSwitch();
    t += (u32)msgGetCharsPerFrame();
  }
  if (skipped) {
    winMsgCloseField(1);
    soundStop(0x3d4,0x32);
    level = 0;
    if (lbl_8027A4B0[0].time < time) {
      level = 1;
      if (lbl_8027A4B0[1].time < time) {
        level = 2;
      }
    }
    doWazaSequence((u32 *)work,lbl_8027A4B0[level].seq,0,8);
    voice = evolutionPlayVoice(before);
    while (GSsndGetStatus(voice) == 2) {
      _threadSwitch();
    }
    msgctrlSetValue(0x32,pokemonBiosGetNicknamePtr(before));
    winMsgOpenField(0x43ff,1,0);
    winMsgCloseField(1);
    fadeSet(lbl_8047E6C0,3);
    fadeCheck(1);
    return 0;
  }
  fn_801DA4E8(work->model[0],0);
  fn_801DA4E8(work->model[1],1);
  doWazaSequence((u32 *)work,1,0,8);
  winMsgCloseField(1);
  soundStop(0x3d4,0x32);
  voice = evolutionPlayVoice(after);
  while (GSsndGetStatus(voice) == 2) {
    _threadSwitch();
  }
  msgctrlSetValue(0x32,pokemonBiosGetNicknamePtr(before));
  dataId = pokemonBiosGetPokemonDataId(after);
  msgctrlSetValue(0x4e,dataId);
  msgctrlSetValue(0x5d,0x3d2);
  winMsgOpenField(0x4400,1,0);
  winMsgCloseField(1);
  t = lbl_8047E6B0;
  while (t < lbl_8047E6C4) {
    _threadSwitch();
    t += (u32)msgGetCharsPerFrame();
  }
  return 1;
}
