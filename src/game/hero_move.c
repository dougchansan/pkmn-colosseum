/**
 * @file hero_move.c
 * @brief GSfield world segment -- split from gs_field_world.c.
 *
 * XD source unit: heroMove* (XD heroMove.cpp)
 * Address range: 0x8012AC9C - 0x80130660 (~13 functions)
 *
 * Split from src/game/gs_field_world.c (physical XD source-unit split of
 * the 734-function field-world bucket into its 12 constituent XD source
 * units). See gs_field_world.c split history for the address-range
 * evidence (anchor-name monotonicity checks) used to place this boundary.
 *
 * Linked as one unit (2026-10-03): .text 0x8012AC9C-0x80130660 and its
 * .sdata2 literal pool 0x8047D030-0x8047D0E0, GC/1.3.2 -O4,p with
 * -inline auto,deferred. Deferred mode emits functions, and pools their
 * literals, in reverse definition order, so the functions below are defined
 * in reverse address order (cbPoison inlines the later heroMoveInitEvent/
 * heroMoveTermEvent). Every constant is a literal; the pool order is first
 * use in address order, with 7.0f (0x8047D064) pooled by proc1Step (XD
 * proc1Step__Ff), a static function defined between heroMoveAddStepCallback
 * and getStep that is only inlined (into heroMoveMain); its standalone copy
 * is unreferenced and dead-stripped at link. 0.01f is spelled 0.010000001f
 * (retail's 0x3C23D70B). .rodata (floors/themes/parts tables) and .bss
 * lbl_80426BD0 stay extern.
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/hero_move.h"

/* Field hero-move state; layout in game/hero_move.h. */
extern HeroMoveWork lbl_80426BD0;
/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const char* fmt, ...);         /* OSReport / GSlog */
extern void* memcpy(void* dst, const void* src, u32 n);
extern void* memset(void* dst, int val, u32 size);
/* GSmem */
extern u16   _toolentryAlloc__FUl(u32 size);                     /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);                   /* GSmemGetPtr */
extern void* fn_800E24B0(u16 handle);                   /* GSmemLock */
extern void  fn_800E209C(u16 handle);                   /* GSmemFree */
/* Matrix / vector */
extern void  PSMTXMultVec(void* mtx, void* vec, void* out); /* MTXMultVec3 */
extern void  PSVECSubtract(void* out, void* in, f32 s);     /* VEC normalize */
/* GSgfx renderer */
extern void  fn_800D7868(void* handle, u32 a, u32 b, u32 c,
                          u32 d, u32 e, u32 f, u32 g);     /* GSgfx draw setup */
extern void* fn_800D7894(void);                             /* GSgfx create render obj */
/* Real named labels referenced by remaining inline-asm wrappers in this TU. */
extern void _modelSetRotateEulerToQuatAll__FP9_HSD_JObj();
extern void cos();
extern void gamedataAttestCheckValid();
extern void gamedataAttestCreate();
extern void gamedataAttestInit();
extern void gamedataCreate();
extern void gamedataGetStatus();
extern void GScharCmp();
extern void GScharLenCpy();
extern void GSflagClear();
extern void GSmsgFontClose();
extern void itemGetStatus();
extern void LCStoreData();
extern void memoDataSet();
extern void menuSubGetPokemonSexForFightDisp();
extern void OSGetTick();
extern void psInitAppSRT();
extern void psInitParticle();
extern void psSetGeneratorAngleRadiusScale();
extern void psSetParticleVisibility();
extern void sin();
extern void statusGetStatus();
/* GSfloor / GScolsys */
extern void* std092_getCurrentRoomID(void);                             /* GSfloor get active */
extern void  GScolsys2GetObjEnable(u32 triIdx, void* outFlag);        /* GScolsys query */
/* ===== Index lookup globals ===== */
extern u8 lbl_8035BBA8[];  /* NPC table (BSS) */
extern u8 lbl_8035C430[];  /* field obj table (BSS) */
extern u32 lbl_80478B48;  /* NPC count (SDA) */
extern u32 lbl_80478B50;  /* field obj count (SDA) */
extern u32 lbl_80478F90;  /* obj header ptr (SDA) */
extern u32 lbl_80478F94;  /* obj data base (SDA) */
/* Field subsystems -- forward declarations (defined below) */
u32  _unloadFlare__FPvUlUl(void);
u32 floorDataBiosGetFileGroupID(u8* entry);
void tvRelease(void);
void pokemonResetBasisStatus(void* ptr);
/* ===== String constants (rodata) ===== */
extern const char lbl_80272770[]; /* "floorUpdateFieldCamera: error updating..." */
extern const char lbl_802724E8[]; /* "floorReadMapPreFunc: can't alloc..." */
extern const char lbl_80272520[]; /* "floorReadScriptPreFunc(): can't alloc..." */
extern const char lbl_8027255C[]; /* "floorReadFontPreFunc(): can't alloc..." */
extern const char lbl_80272594[]; /* "floorReadMsgPreFunc(): can't alloc..." */
extern const char lbl_802725CC[]; /* "floorReadNormalPreFunc(): can't alloc..." */
/* ===== Combined cross-segment declarations (file-scope forward
 * declarations duplicated across all gs_field_world.c split segments;
 * de-duplicated by identifier from the whole original TU -- includes
 * both the original file's own top-level externs/prototypes AND a
 * declaration synthesized from every function's own definition
 * signature, since in the original single-TU file a definition also
 * served as a forward declaration for any later same-file caller; now
 * that callers may live in a sibling segment file they need an
 * explicit declaration. Function-local (per-call-site) extern
 * declarations are NOT hoisted here -- they travel with their own
 * function body. ===== */
extern u8 lbl_8047AD71;
extern u32 lbl_8047ADC0;
extern u32 lbl_8047ADB8;
extern u8 lbl_804083D0[0x30];
extern u32 lbl_8047AD68;
extern u32 lbl_8047AD6C;
extern void flareRelease(void);
void fn_801193BC(void);
extern s32 pokemonWazaGetMaxPP(u8* ptr, u16 idx);
extern void wazaGetStatus(void);
extern void pokemonBiosGetPokemonWazaPtr(void);
extern void pokemonResetBasisStatus(void* ptr);
void pokemonSetLevelBasisStatus(void);
extern void heroItemGetItemKindToItemAryPtr(void);
extern void heroSetStatus();
extern u32 heroGetStatus(void*, u32, u32);
extern void* GSresAllocResourceAlign(); /* K&R: called with 5 args, returns void* */
extern u8 fn_800FF548(void);
extern u32 _unloadScript__FPvUlUl(); /* K&R: asm void wrapper, used as function pointer */
extern u32 _unloadFont__FPvUlUl(); /* K&R: asm void wrapper, used as function pointer */
extern u32 _unloadMsg__FPvUlUl(); /* K&R: asm void wrapper, used as function pointer */
extern void GStextureFree(void);
extern const char lbl_80272608[];
extern const char lbl_8027262C[];
extern u8 lbl_8035BB30[];
extern u8 lbl_8035BB50[];
extern u8 lbl_8035BB10[];
extern u8 lbl_8035BAF4[];
extern const char lbl_80272658[];
extern u8 lbl_8035BAD8[];
extern const char lbl_80272680[];
extern u8 lbl_8035BABC[];
extern u32 lbl_80478FB8;
extern u32 lbl_80478FBC;
extern u8 lbl_802726D4[];
extern u8 lbl_8035B8A0[];
extern void* floorDataBiosGetFieldCameraListPtr();
extern u32 floorDataBiosGetGroupID();
extern void* floorDataBiosGetPtr(u32 key);
extern void fn_8011791C(void);
extern void gsParticleKillOldest(void);
extern void gsParticlePtclHook();
extern u8 fn_80119D90(u16 idx);
extern u8 fn_80119DD0(u16 idx);
extern u16 fn_80119E10(u16 idx);
extern u8 fn_80119E50(u16 idx);
extern u8 fn_80119E90(u16 idx);
extern u16 fn_80119ED0(u16 idx);
extern u8 fn_80119F10(u16 idx);
extern u32 fn_80119F50(u16 idx);
extern void wazaDataBiosSetFightWazaWzxVariationFuncPtr(u8* ptr, u32 val);
extern void wazaDataBiosSetFightWazaWzxTypeFuncPtr(u8* ptr, u32 val);
extern u32 wazaDataBiosGetFightWazaWzxVariationFuncPtr(u8* ptr);
extern u8 wazaDataBiosGetTypeId(u8* ptr, u16 idx);
extern u32 wazaDataBiosGetFightWazaWzxTypeFuncPtr(u8* ptr);
extern void wazaDataBiosSetFightTrainerAiWazaDamageFuncPtr(u8* ptr, u32 val);
extern void wazaDataBiosSetFightTrainerAiWazaHitFuncPtr(u8* ptr, u32 val);
extern void wazaDataBiosSetFightTrainerAiWazaValueFuncPtr(u8* ptr, u32 val);
extern void wazaDataBiosSetTypeId(u8* ptr, u16 idx, u8 val);
extern u32 wazaDataBiosGetFightTrainerAiWazaDamageFuncPtr(u8* ptr);
extern u32 wazaDataBiosGetFightTrainerAiWazaHitFuncPtr(u8* ptr);
extern u32 wazaDataBiosGetFightTrainerAiWazaValueFuncPtr(u8* ptr);
extern void pokemonGetDarkPokemonLevel(void);
extern u32 pokemonDataCheckValid(u32 a, u16 key);
extern u8 fn_80121ADC(u8* ptr, u32 slot);
extern void pokemonSetWazaStatus(void);
extern u32 pokemonWazaCheckValid(u8* ptr, u32 arg2);
extern void pokemonInit(u8* ptr);
extern void pokemonEvolutionCreateAddPokemon(void);
extern void pokemonEvolution(void);
extern void savedataInit(void);
extern void heroAddPokedoru(u8* ptr, u32 offset);
extern s32 heroItemAddItemDataId(u8* ptr, u32 arg2, u32 arg3, u32 arg4);
extern u32 heroAddPokemon(u8* ptr, void* arg2);
extern void heroCreate(u8* ptr, u32 arg2, u8 arg3);
extern void heroInit();
extern void heroBiosSetPokecouponAll(); /* K&R: typed impl or conflict */
extern void heroBiosSetPokecoupon(); /* K&R: typed impl or conflict */
extern void heroBiosSetPokedoru(); /* K&R: typed impl or conflict */
extern void heroBiosSetHizukiNamePtr(); /* K&R: typed impl or conflict */
extern void heroBiosSetNamePtr(); /* K&R: typed impl or conflict */
extern void heroMoveTermEvent(void);
extern void heroMoveInitEvent(void);
extern void partnerApplyMove();
extern void heroPokemonGetEifie(u32 arg1);
extern void heroPokemonGetBlacky(u32 arg1);
extern s32 psGetGeneratorChildMaxLife(u32);
extern void* wazaDataBiosGetPtr(u16 idx);
extern u32 pokemonGetStatus();
extern void pokemonSetStatus();
extern u16 pokemonDataBiosGetSinkaPokemonDataId(u8* ptr, u16 idx);
extern u16 pokemonDataBiosGetSinkaBuff(u8* ptr, u16 idx);
extern u8 pokemonDataBiosGetSinkaKind(u8* ptr, u16 idx);
extern u8 floorUpdateFieldCamera();
void updateAnimation__Ff15HEROMOVE_MEMBER(void* model, s32 member, f32 amount);
extern void* heroBiosGetPokemonPtr(u8* ptr, u16 idx);
extern void* heroBiosGetHizukiNamePtr(void* ptr);
extern void* heroBiosGetHizukiItemPtr(u8* ptr, u16 idx);
extern void* heroBiosGetItemKoronPtr(u8* ptr, u16 idx);
extern void* heroBiosGetItemSeedPtr(u8* ptr, u16 idx);
extern void* heroBiosGetItemSkillPtr(u8* ptr, u16 idx);
extern void* heroBiosGetItemBallPtr(u8* ptr, u16 idx);
extern void* heroBiosGetExtraItemPtr(u8* ptr, u16 idx);
extern void* heroBiosGetItemNormalPtr(u8* ptr, u16 idx);
extern u32 heroBiosGetNamePtr(void* ptr);
extern u32 lbl_80478EBC;
extern u32 lbl_80478EB8;
extern void std093_getCurrentGroupID(void);
extern void peopleSetVisible(u32 group, u32 id, u8 visible);
extern void soundPlayFanfare(void);
extern void msgctrlSetValue(void);
extern void winMsgOpen(void);
extern void winMsgClose(void);
extern void pcboxDelItem(void);
extern void std011_dialogDisplayYesNoMenu(void);
void floorEventGetTresure(void);
extern void std051_characterSetAnimation(void);
extern void peopleSetFlagsByIndex(void);
extern void std081_flagCheckTrue(void);
extern void soundStartSE(void);
extern void peopleWaitSyncMotion(void);
extern void std078_flagSetFalse(void);
void floorEventCtrlTresure(void);
extern void floorGetResource(void);
extern void GSmodelSetAnimIndex(void);
extern void GSmodelSetAnimFrame(void);
extern void GSmodelSetAnimRate(void);
extern void GSmodelSetAnimType(void);
extern void GSmodelStartAnimation(void);
extern void _threadSwitch(void);
extern void GSmodelIsAnimating(void);
extern void* GSmodelGetPart(void* model, s32 partIndex);
extern void GSpartGetTransform(void* part, void* transform, void* arg2, void* arg3);
extern void GSpartFree();
extern void fn_8018AACC(void);
extern void peopleMoveCheck(u32 groupId, u32 index, u8 waitFlag);
extern void peopleStartTurn(void);
extern void std168_characterDetachFromCharacter(void);
extern void peopleSetPositionByIndex(void);
extern void soundStopSE(void);
extern void GSmodelCanAnimate(void);
extern void std070_characterAttachToCharacter(void);
extern void std059_characterSetPosition(void);
extern void GSmodelGetFrameCount(void);
extern void fn_800D37CC(void);
extern f32 lbl_8047CFA0;
extern u32 lbl_80478EC8;
extern u32 lbl_80478ECC;
extern f32 lbl_8047CFA4;
extern f32 lbl_8047CFA8;
extern f32 lbl_8047CFAC;
extern f64 lbl_8047CFB8;
extern f32 lbl_8047CFB4;
extern f32 lbl_8047CFB0;
void floorEventCtrlElevator(void);
extern void scriptSetCol(void);
extern void std079_flagSetTrue(void);
extern void fn_8018C558(void);
extern void fn_8018C8F4(void);
void floorEventCtrlDoor(void);
extern void EvlogSet__FScUl();
extern void scriptSetEventColID();
extern void cmsRunSync();
extern u8 lbl_80272708[];
extern u32 lbl_80478EC4;
extern u32 lbl_80478EC0;
extern void* peopleInfoBiosGetPtrFromIndex(u16);
extern f32 lbl_8047CFD0;
extern u8 lbl_8047AD70;
extern f32 lbl_8047AD74;
extern f32 lbl_8047AD78;
extern f32 lbl_8047AD7C;
extern u8 GSscene_GetMode(void);
extern void cameraSetHeight(f32);
extern void cameraSetDistance(f32);
extern void cameraSetRotY(f32);
extern void GSscene_GetCameraPositionVector(void*);
extern f32 lbl_8047CFD8;
extern f32 lbl_8047CFD4;
extern void* GSresGetResource();
extern void GSmodelGetPosition(void*, void*);
extern void GSscene_GetCameraViewVector(void*);
extern f32 cameraGetHeight(void);
extern f32 cameraGetDistance(void);
extern f32 cameraGetRotY(void);
extern void set__5GSvecFfff(void* obj, f32 f1, f32 f2, f32 f3);
extern void GSmtxMakeYRotation(void*, f32);
extern void GSvecTransform(void*, void*, void*);
extern void GSvecAdd(void*, void*, void*);
extern f64 atan2(f32, f32);
extern void cameraMoveTargetPos(u32, void*, f32);
extern void cameraMovePosition(u32, void*, f32);
extern void cameraMoveRotation(u32, void*, f32);
extern void GSgappTerminate(void);
extern void GSgappCreate(void);
void fn_801176C8(void);
extern void* GScameraGetActiveCamera();
extern void GSvecCopy();
extern u8 lbl_802727B8[];
extern void fn_80177A38(void); /* referenced by asm .inc wrappers (fn_801171C8/80117330/8011791C/8012E388/8012EBD4); was undefined -> broke the TU parse */
extern void GSmodelResetTextureChange(void);
extern void GSmodelFree(void);
extern void GStextureCreate(void);
extern void floorOpenModel(void);
extern void GSmodelSetVisibility(void);
extern void GSmodelLinkTexAnimToAnim(void);
extern void GSmodelSetTextureChange(void);
extern u32 lbl_80478B40;
extern u32 lbl_8047AD88;
extern u32 lbl_8047AD8C;
extern u32 lbl_8047AD90;
extern u32 lbl_8047AD94;
extern u32 lbl_8047AD80;
extern u32 lbl_8047AD84;
extern void fn_800EC134(u32);
extern void fn_800D4604();
extern void fn_800D377C();
extern void fn_800D3410();
extern void fn_800D9B24();
extern void fn_800D9AF0();
extern void fn_800D258C();
extern void fn_800D9D68();
extern void fn_800D9C24();
extern void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void);
extern void GSmodelDrawModel();
extern void fn_800D3190(void);
extern void fn_800FF4D4(void);
extern u32 lbl_802727C8[];
extern void psSetBillboardCamera();
extern void psDispMain();
extern void psGetParticleChildCount(void* ptr);
extern void psKillFamily();
extern void GSmodelSet60fpsAnimFlag();
extern void psUnlinkChildGensFromJObj();
extern void psKillGenerator();
extern u32 lbl_8047AD9C;
extern u32 lbl_8047ADA0;
void fn_801181B0(void);
extern void fn_800E06EC(void);
extern void GSvecTransformQuat(void);
extern void fn_800E0108(void);
extern void psInterpretParticles(void);
extern void psExecGenerator(void);
extern void fn_800057A0(void);
extern void jumptable_8035BB88();
extern u8 lbl_8047ADB0;
void gsParticleUpdate(void);
void fn_80118874(void);
extern void psSetParticleVisibility(); /* K&R: called with 0 or 1 args */
extern void psSetGeneratorAngleRadiusScale(void);
extern void psLinkChildGensToJObj(void);
extern f32 lbl_8047CFE8;
extern f32 lbl_8047CFEC;
extern void psCreateGeneratorID(void);
extern void fn_800D3094(void);
extern u32 lbl_8047ADAC;
extern u32 lbl_8047ADA8;
void gsParticleCreate(void);
extern void psInitDataBank(void);
extern void DCFlushRange();
extern u8 lbl_802727D8[];
void gsParticleBankLoad(void);
extern void fn_8016A01C(void);
extern void psInitGenerator(void);
extern void fn_8016AAF4(void);
extern void HSD_JObjSetSPtclCallback(void);
extern void HSD_JObjSetPtclCallback(void);
extern void psSetPointJObjNodup(void);
extern void fn_8019D610(void);
extern u16 lbl_8047AD98;
extern u16 lbl_8047ADA4;
extern void GSmodelSearchModelList(void);
extern void GSmodelGetLinkedGSparticleBank(void);
extern void GSmodelIsRootNullAdded(void);
extern void GSpartGetJObjIndex(void);
extern void fn_800E3CBC(void);
extern void GSmodelGetGSparticleLinkAttachMode(void);
extern void GSmodelGetVisibility(void);
extern void fn_80135E44(void);
void fn_8011A0A8(void);
extern s32 kaisuuGetKaisuu(u32);
extern void jumptable_8035C260();
extern void fightWazaBiosSetWazaBanme(void);
extern void fightWazaBiosSetMotoWazaDataId(void);
extern void fightWazaBiosSetUseWazaDataId(void);
extern void fightWazaBiosSetTargetDataId(void);
extern void fightWazaBiosSetCritical(void);
extern void fightWazaBiosSetDamageValue(void);
extern void fightWazaBiosSetDamage(void);
extern void fightWazaBiosSetHitDamage(void);
extern void fightWazaBiosSetIryoku(void);
extern void fightWazaBiosSetZokusei(void);
extern void fightWazaBiosSetKaisuu(void);
extern void fightWazaBiosSetAutoMakeFlag(void);
extern void jumptable_8035C290();
void wazaSetStatus(void);
extern void fightWazaBiosGetWazaBanme(void);
extern void fightWazaBiosGetMotoWazaDataId(void);
extern void fightWazaBiosGetUseWazaDataId(void);
extern void fightWazaBiosGetTargetDataId(void);
extern void fightWazaBiosGetJoutaiPtr(void);
extern void fightWazaBiosGetCritical(void);
extern void fightWazaBiosGetDamageValue(void);
extern void fightWazaBiosGetDamage(void);
extern void fightWazaBiosGetHitDamage(void);
extern void fightWazaBiosGetIryoku(void);
extern void fightWazaBiosGetZokusei(void);
extern void fightWazaBiosGetKaisuu(void);
extern void fightWazaBiosGetAutoMakeFlag(void);
extern void jumptable_8035C35C();
extern u32 lbl_80478DF8;
extern u32 lbl_80478DFC;
extern u32 lbl_80478B78;
extern u32 lbl_8035F9A8[];
extern u32 lbl_80478E68;
extern u32 lbl_80478E6C;
extern u32 lbl_80478B70;
extern u8 lbl_8035F988[];
extern u32 lbl_80478B68;
extern u8 lbl_8035F5E0[];
extern u32 lbl_80478E58;
extern u32 lbl_80478E5C;
extern u32 lbl_80478E60;
extern u32 lbl_80478E64;
extern u32 lbl_80478B60;
extern u8 lbl_8035E940[];
extern void _flagSet(u32);
extern u32 std082_flagGetValue(u32);
extern u32 lbl_80478F90;
extern u32 lbl_80478F94;
extern u8 lbl_80478B58[4];
extern u8 lbl_80478B5C[4];
extern f32 lbl_8047CFF0;
extern f64 lbl_8047D008;
extern f32 lbl_8047CFF4;
extern f64 lbl_8047D010;
extern f32 lbl_8047CFF8;
extern f32 lbl_8047CFFC;
extern f32 lbl_8047D000;
extern f32 lbl_8047D004;
extern void itemDataBiosGetPtr(void);
extern void itemDataBiosGetKind(void);
extern void itemDataBiosGetBuff(void);
extern f32 lbl_8047D018;
void pokemonAddDpFormPokemonDpFilterId(void);
extern void* fn_801EEEB8();
void pokemonSetDarkPokemonStatus(void);
extern void GScharCpy(void);
extern void fn_8010BBB8(void);
extern void fn_8001D994(void);
void pokemonToMenuPokemonStatus(void);
extern void GSmsgGetGSchar(void);
extern void fn_8010C4D4(void);
extern void fn_8010C46C(void);
extern void GSrand(void);
extern u8 lbl_8027296C[];
void pokemonCheckSetMonohiroi(void);
void pokemonAllKaihuku(void);
extern u32 fn_801DE190(u32 idx, u32 base, u32 flag);
extern void fn_801DA3CC(void);
extern void fn_801DA36C(void);
extern u32 lbl_80478F90; /* obj header ptr (SDA) */
extern void fn_80135530(void);
void pokemonGetFriendFormPokemonFriendFilterId(void);
extern u8 lbl_80272948[];
void pokemonGetEffortFromPokemon(void);
extern void fn_80008154(void);
extern void fn_80142CF4(void);
extern u32 sexGetPokemonSexRaitoKotei(u32);
extern void fn_801EE958(void);
extern void fn_801EEB34(void);
extern void memoDataSet(void);
void pokemonCheckFightOut(void);
extern void gamedataAttestBiosCopy(void);
void pokemonCreate(void);
void pokemonCreateRndFit(void);
extern void fadeSet(void);
extern void fadeCheck(void);
extern void evolutionOpen(void);
extern f32 lbl_8047D020;
void pokemonEvolutionAll(void);
extern u8 lbl_802729A4[];
extern u8 lbl_80272998[];
extern void fn_800F9EE4(void);
extern void jumptable_80363468();
void getEvoPokemonLevelUp(void);
extern void itemDataBiosGetItemSoubiDataId(void);
void pokemonEvolutionCheck(void);
extern u8 lbl_80408400[];
extern void fn_8013528C(void);
extern void GScharMakeFromSJIS(void);
extern void gamedatasaveSetStatus(void);
extern u8 lbl_8047D028[8];
void savedataCreate(void);
extern void gamedataInit(void);
extern void pcboxInit(void);
extern void fn_801908D4(void);
extern void mailInitMailbox(void);
extern void sodateyaInit(void);
extern void fn_8006B6B4(void);
extern void memoInit(void);
extern void fn_80083CBC(void);
extern void fn_801EF128(void);
extern void exribbonInit(void);
extern void jumptable_803634A8();
extern void fn_80140A9C(void);
extern void fn_80140ACC(void);
extern void fn_80141308(void);
extern void fn_80142368(void);
extern void fn_80140588(void);
extern void pcboxAddPokemon(void);
extern void fn_80142A88(void);
extern void jumptable_803634F0();
extern void jumptable_80363558();
extern void fadeEffectDokuStart(void);
extern void peopleClearFlagsByIndex(void);
extern void std151_characterFaceDirection(void);
extern void winMsgOpenField(void);
extern void winMsgCloseField(void);
extern void fn_801D0AFC(void);
extern void fn_80113FE8(void);
void cbPoison__Fl15FootStepCounterl(s32 arg);
extern u32 peopleIsInGroup();
extern u32 peopleSearchID();
extern void peopleInfoBiosGetPtr(void);
extern void fn_8018F5E4(void);
extern void colsysWallSweep(void);
extern void PSVECScale(void);
extern void PSVECAdd(void);
extern void GScolsys2HumanCollision(void);
extern u8 lbl_80478AC0[];
u32 fn_8012B19C(s32 member, HeroMoveVec* start, HeroMoveVec* target, f32 extraRadius);
u32 heroMoveChkHinderClear(s32 member);
extern u32 GSgetFrameStep(void);
void getStep__FP8FOOTSTEPP8_GSmodelPiP8FOOTWORK(f32*, void*, s32*, f32*);
extern u32 peopleFindTalkTarget(u32, u32, f32, f32);
extern u32 peopleGetPositionPtr();
extern void fn_8018FC50(void);
extern void fn_800CE148(void);
extern void fn_800CDBE0(void);
extern s32 GScolsys2CheckGetEventID();
extern void peopleStopTurn(void);
extern void fn_800F7D38(void);
extern void fn_800F7C8C(void);
extern void fn_8018BA04(void);
extern void peopleTurnToPoint(void);
extern u8 fn_8018D7D0();
extern void fn_80183730(void);
extern u32 fn_8018397C();
extern void peopleTalkPause(void);
extern void std055_characterEndSpeech(void);
extern void fn_80183688(void);
u8 updateChat__F15HEROMOVE_MEMBER(s32 player);
s32 heroMoveCheckEvent(void* event);
extern void fn_8018F4C8(void);
extern void GSmodelGetAnimIndex(void);
extern void GSmodelGetAnimFrame(void);
extern void GSmodelSetAnimBlend(void);
extern void GSmodelSetBlendFactor(void);
s32 circleLineIntersect(HeroMoveVec*, HeroMoveVec*, HeroMoveVec*, HeroMoveVec*, HeroMoveVec*, f32);
void partnerGetTarget(s32, HeroMoveVec*, HeroMoveVec*);
extern void fn_800E3C64(void);
void partnerMove(s32 member);
extern void fn_800F7A7C(void);
extern void fn_800F7A08(void);
extern void fn_800F7BC4(void);
extern void peopleSetSpeedFromAmount(void);
extern void fn_80166458(void);
void heroReadStick(s32, f32*);
extern void fn_800F7AF0(void);
extern void peopleAmountFromMove(void);
extern void PSVECDistance(void);
f32 moveLeader__F15HEROMOVE_MEMBER();
extern u8 dbgMenuIsOpen(void);
extern u8 menuIsCheck(u32);
extern u8 fn_8018C424(u32, u32, u32);
extern void fn_8000D710(u32);
extern u8 lbl_80272A38[];
u32 heroMoveMain(void);
extern void std067_characterStopLookAt(u32, u32);
extern void std065_characterLookAtPlayer(u32, u32);
s32 fn_8012F1FC(s32);
s32 fn_8012F40C(s32);
extern void GSmodelGetRotation(void);
extern void fn_8010E138(void);
extern void GSmodelSetRotation(void);
void initFloor__Fv(void);
extern u32 floorGetNextFloorID(void);
extern s32 fn_8006AE18(void);
extern u8 lbl_802729C0[];
extern u8 lbl_80272A10[];
u32 heroMoveGetKenObjID(void);
extern void peopleOpen(u32, u32, u32);
extern void GSmodelEnableAnimBlend(void*);
extern void fn_8018CB5C(u32, u32);
extern void fn_80189328(u32, u32, u32);
extern void peopleSetRotation(u32, u32, void*);
s32 heroMoveInit(void*, void*);
void heroMoveSyncWithHero(void);
void fn_8013024C(void);
extern void gamedataGetStatus(void);
extern void gamedataAttestCreate(void);
extern void fn_800F76E4();
extern void fn_80112700(void);
extern void GSmsgFontOpen();
extern void GSmsgOpen();
extern u8 fn_800FF554(void);
extern void fn_800F760C();
extern void fn_800FC2A8();
extern void GSmsgClose();
extern u8 lbl_8035BA98[];
extern u8 lbl_8035BA7C[];
extern u8 lbl_8035BA60[];
extern u8 lbl_8035BA48[];
extern u8 lbl_8035BA2C[];
extern u8 lbl_8035BA10[];
extern u8 lbl_8035B9F8[];
extern u8 lbl_8035B9DC[];
extern u8 lbl_8035B9C0[];
extern u8 lbl_8035B9A4[];
extern u8 lbl_8035B988[];
extern u8 lbl_8035B96C[];
extern u8 lbl_8035B950[];
extern u8 lbl_8035B938[];
extern u8 lbl_8035B904[];
extern u16 fn_801EF624();
extern u8 lbl_8035B8E8[];
extern const char lbl_802726AC[];
extern const char lbl_8035B8CC[];
extern u8 lbl_8035B8B4[];
extern u8 lbl_8035BB70[];
extern u32 lbl_8047CFC0;
extern u32 lbl_8047CFC8;
extern u32 lbl_8047CFC4;
extern void GSvecSquareDistance(void);
extern f32 lbl_8047CFDC;
extern f32 lbl_8047CFE0;
extern u32 lbl_80478B48; /* NPC count (SDA) */
extern u32 lbl_80478F94; /* obj data base (SDA) */
extern void* tasteDataGetPtr(void* ptr);
extern void tasteDataGetAisyou(void* ptr, u8 val);
extern void fn_80135708(void);
extern void fightPokemonBiosSetMotoPokemonPtr(void);
extern void fightPokemonBiosSetEntryId(void);
extern void fightPokemonBiosSetCatchEntryFlag(void);
extern void fightPokemonBiosSetLevelUpFlag(void);
extern void fightPokemonBiosSetDarkOutFlag(void);
extern void fightPokemonBiosSetHokakuFlag(void);
extern void fightOutPokemonBiosSetMotoFightPokemonPtr(void);
extern void fightOutPokemonBiosSetFightPokemonPtr(void);
extern void fightWazaCheckWriteJoutaiDataId(void);
extern void fightWazaWriteJoutaiDataId(void);
extern void fightWazaIsJoutaiDataId(void);
extern void fightWazaInitJoutaiDataId(void);
extern void fightWazaInitJoutai(void);
extern void fightOutPokemonBiosSetAbicntPhyAtk(void);
extern void fightOutPokemonBiosSetAbicntPhyDef(void);
extern void fightOutPokemonBiosSetAbicntSpeAtk(void);
extern void fightOutPokemonBiosSetAbicntSpeDef(void);
extern void fightOutPokemonBiosSetAbicntNimbleness(void);
extern void fightOutPokemonBiosSetAbicntAverage(void);
extern void fightOutPokemonBiosSetAbicntAvoid(void);
extern void fightOutPokemonBiosSetFightoutTurnCount(void);
extern void fightOutPokemonBiosSetSequencePtr(void);
extern void fightOutPokemonBiosSetSketchWazaDataId(void);
extern void fightOutPokemonBiosSetLastSelectWazaDataId(void);
extern void fightOutPokemonBiosSetLastUseWazaDataId(void);
extern void fightOutPokemonBiosSetLastReceiveWazaTargetDataId(void);
extern void fightOutPokemonBiosSetHitWazaDataId(void);
extern void fightOutPokemonBiosSetHitWazaZokuseiDataId(void);
extern void fightOutPokemonBiosSetGamanDamageValue(void);
extern void fightOutPokemonBiosSetGamanDamageTargetId(void);
extern void fightOutPokemonBiosSetOumuWazaDataId(void);
extern void fightOutPokemonBiosSetNamakeFlag(void);
extern void fightOutPokemonBiosSetUsedItemDataId(void);
extern void fightOutPokemonBiosSetStockItemDataId(void);
extern void fightOutPokemonBiosSetSuccessCnt(void);
extern void fightOutPokemonBiosSetMeetEnemyFightPokemonEntryId(void);
extern void fightOutPokemonBiosSetZokuseiDataId(void);
extern void fightOutPokemonBiosSetTokuseiDataId(void);
extern void fightOutPokemonBiosSetDamageAtkValue(void);
extern void fightOutPokemonBiosSetDamageAtkTargetId(void);
extern void fightOutPokemonBiosSetDamageSpeValue(void);
extern void fightOutPokemonBiosSetDamageSpeTargetId(void);
extern void fightOutPokemonBiosSetMahiNoAttackFlag(void);
extern void fightOutPokemonBiosSetKonranMyselfAttackFlag(void);
extern void fightOutPokemonBiosSetOutWazaKoukanaiFlag(void);
extern void fightOutPokemonBiosSetTameWazaFlag(void);
extern void fightOutPokemonBiosSetItemNigeruFlag(void);
extern void fightOutPokemonBiosSetHuuinNoAttackFlag(void);
extern void fightOutPokemonBiosSetMeroMeroNoAttackFlag(void);
extern void fightOutPokemonBiosSetKanashibariNoAttackFlag(void);
extern void fightOutPokemonBiosSetChouhatsuNoAttackFlag(void);
extern void fightOutPokemonBiosSetIchamonNoAttackFlag(void);
extern void fightOutPokemonBiosSetHirumuNoAttackFlag(void);
extern void fightOutPokemonBiosSetPassPpdecFlag(void);
extern void fightOutPokemonBiosSetFightActionFlag(void);
extern void fightOutPokemonBiosSetDoClearbodyFlag(void);
extern void fightOutPokemonBiosSetReceivesWazaHiraishinFlag(void);
extern void fightOutPokemonBiosSetVanishoffFlag(void);
extern void fightOutPokemonBiosSetDoIkakuFlag(void);
extern void fightOutPokemonBiosSetDoTraceFlag(void);
extern void fightOutPokemonBiosSetNoPressureFlag(void);
extern void fightOutPokemonBiosSetIrekaetaFlag(void);
extern void fightOutPokemonBiosSetItemKoraetaFlag(void);
extern void fightOutPokemonBiosSetKaigaraDamageValue(void);
extern void fightOutPokemonBiosSetMyselfDamageAtkValue(void);
extern void fightOutPokemonBiosSetMyselfDamageAtkTargetId(void);
extern void fightOutPokemonBiosSetMyselfDamageSpeValue(void);
extern void fightOutPokemonBiosSetMyselfDamageSpeTargetId(void);
extern void fightOutPokemonBiosSetKizetuFlag(void);
extern void fightOutPokemonBiosSetIrekaeTargetEntryId(void);
extern void jumptable_8035E028();
extern void pokemonDataBiosGetItemDataId(void);
extern void fightPokemonBiosGetMotoPokemonPtr(void);
extern void fightPokemonBiosGetPokemonBuffPtr(void);
extern void fightPokemonBiosGetFightJoutaiPtr(void);
extern void fightPokemonBiosGetEntryId(void);
extern void fightPokemonBiosGetCatchEntryFlag(void);
extern void fightPokemonBiosGetLevelUpFlag(void);
extern void fightPokemonBiosGetDarkOutFlag(void);
extern void fightPokemonBiosGetHokakuFlag(void);
extern void fightOutPokemonBiosGetMotoFightPokemonPtr(void);
extern void fightOutPokemonBiosGetFightPokemonPtr(void);
extern void fightOutPokemonBiosGetFightPokemonHensinBuffPtr(void);
extern void fightOutPokemonBiosGetFightoutJoutaiPtr(void);
extern void fightOutPokemonBiosGetFightWazaPtr(void);
extern void fightOutPokemonGetUseWazaDataId(void);
extern void fightOutPokemonGetMotoWazaDataId(void);
extern void fightWazaIsHit(void);
extern void fightOutPokemonBiosGetFightItemPtr(void);
extern void fightOutPokemonBiosGetAbicntPhyAtk(void);
extern void fightOutPokemonBiosGetAbicntPhyDef(void);
extern void fightOutPokemonBiosGetAbicntSpeAtk(void);
extern void fightOutPokemonBiosGetAbicntSpeDef(void);
extern void fightOutPokemonBiosGetAbicntNimbleness(void);
extern void fightOutPokemonBiosGetAbicntAverage(void);
extern void fightOutPokemonBiosGetAbicntAvoid(void);
extern void fightOutPokemonBiosGetFightoutTurnCount(void);
extern void fightOutPokemonBiosGetSequencePtr(void);
extern void fightOutPokemonBiosGetSketchWazaDataId(void);
extern void fightOutPokemonBiosGetLastSelectWazaDataId(void);
extern void fightOutPokemonBiosGetLastUseWazaDataId(void);
extern void fightOutPokemonBiosGetLastReceiveWazaTargetDataId(void);
extern void fightOutPokemonBiosGetHitWazaDataId(void);
extern void fightOutPokemonBiosGetHitWazaZokuseiDataId(void);
extern void fightOutPokemonBiosGetGamanDamageValue(void);
extern void fightOutPokemonBiosGetGamanDamageTargetId(void);
extern void fightOutPokemonBiosGetOumuWazaDataId(void);
extern void fightOutPokemonBiosGetKeepFightWazaPtr(void);
extern void fightOutPokemonBiosGetNamakeFlag(void);
extern void fightOutPokemonBiosGetUsedItemDataId(void);
extern void fightOutPokemonBiosGetStockItemDataId(void);
extern void fightOutPokemonBiosGetSuccessCnt(void);
extern void fightOutPokemonBiosGetMeetEnemyFightPokemonEntryId(void);
extern void fightOutPokemonBiosGetFightActionBuffPtr(void);
extern void fightOutPokemonBiosGetZokuseiDataId(void);
extern void fightOutPokemonBiosGetTokuseiDataId(void);
extern void fightOutPokemonBiosGetWazaMenuCurPtr(void);
extern void fightOutPokemonBiosGetDamageAtkValue(void);
extern void fightOutPokemonBiosGetDamageAtkTargetId(void);
extern void fightOutPokemonBiosGetDamageSpeValue(void);
extern void fightOutPokemonBiosGetDamageSpeTargetId(void);
extern void fightOutPokemonBiosGetMahiNoAttackFlag(void);
extern void fightOutPokemonBiosGetKonranMyselfAttackFlag(void);
extern void fightOutPokemonBiosGetOutWazaKoukanaiFlag(void);
extern void fightOutPokemonBiosGetTameWazaFlag(void);
extern void fightOutPokemonBiosGetItemNigeruFlag(void);
extern void fightOutPokemonBiosGetHuuinNoAttackFlag(void);
extern void fightOutPokemonBiosGetMeroMeroNoAttackFlag(void);
extern void fightOutPokemonBiosGetKanashibariNoAttackFlag(void);
extern void fightOutPokemonBiosGetChouhatsuNoAttackFlag(void);
extern void fightOutPokemonBiosGetIchamonNoAttackFlag(void);
extern void fightOutPokemonBiosGetHirumuNoAttackFlag(void);
extern void fightOutPokemonBiosGetPassPpdecFlag(void);
extern void fightOutPokemonBiosGetFightActionFlag(void);
extern void fightOutPokemonBiosGetDoClearbodyFlag(void);
extern void fightOutPokemonBiosGetReceivesWazaHiraishinFlag(void);
extern void fightOutPokemonBiosGetVanishoffFlag(void);
extern void fightOutPokemonBiosGetDoIkakuFlag(void);
extern void fightOutPokemonBiosGetDoTraceFlag(void);
extern void fightOutPokemonBiosGetNoPressureFlag(void);
extern void fightOutPokemonBiosGetIrekaetaFlag(void);
extern void fightOutPokemonBiosGetItemKoraetaFlag(void);
extern void fightOutPokemonBiosGetKaigaraDamageValue(void);
extern void fightOutPokemonBiosGetMyselfDamageAtkValue(void);
extern void fightOutPokemonBiosGetMyselfDamageAtkTargetId(void);
extern void fightOutPokemonBiosGetMyselfDamageSpeValue(void);
extern void fightOutPokemonBiosGetMyselfDamageSpeTargetId(void);
extern void fightOutPokemonBiosGetKizetuFlag(void);
extern void fightOutPokemonBiosGetIrekaeTargetEntryId(void);
extern void fightOutPokemonBiosGetFightOutPokemonEnemyPtr(void);
extern void fightOutPokemonCheckFightOut(void);
extern void jumptable_8035E4B0();
extern void peopleMoveByVector(void);
extern void PSVECDotProduct(void);
extern void charModelGetHeadYawMin(void);
extern void charModelGetHeadYawMax(void);
extern u8 fn_801174EC(void);
extern void fn_80119F90(u8* ptr, u16 val);
extern void fn_80119FA0(u8* ptr, u32 val);
extern void fn_80119FB0(u8* ptr, u8 val);
extern void fn_80119FC0(u8* ptr, u8 val);
extern void fn_80119FD0(u8* ptr, u8 val);
extern void fn_80119FE0(u8* ptr, u16 val);
extern void fn_80119FF0(u8* ptr, u16 val);
extern u16 fn_8011A000(u8* ptr);
extern u32 fn_8011A018(u8* ptr);
extern u8 fn_8011A030(u8* ptr);
extern u8 fn_8011A048(u8* ptr);
extern u8 fn_8011A060(u8* ptr);
extern u16 fn_8011A078(u8* ptr);
extern u16 fn_8011A090(u8* ptr);
extern void wazaDataBiosSetRiskFlag(u8* ptr, u8 val);
extern u8 wazaDataBiosGetRiskFlag(u8* ptr);
extern void wazaDataBiosSetHidenFlag(u8* ptr, u8 val);
extern u8 wazaDataBiosGetHidenFlag(u8* ptr);
extern void fn_8011C5E0(u8* ptr, u8 val);
extern void fn_8011C5F0(u8* ptr, u8 val);
extern void wazaDataBiosSetDoc(u8* ptr, u32 val);
extern void wazaDataBiosSetWazawzxdataId(u8* ptr, u32 val);
extern void wazaDataBiosSetPressure(u8* ptr, u8 val);
extern void wazaDataBiosSetBouon(u8* ptr, u8 val);
extern void wazaDataBiosSetNegoto(u8* ptr, u8 val);
extern void wazaDataBiosSetNekonote(u8* ptr, u8 val);
extern void wazaDataBiosSetMonomane(u8* ptr, u8 val);
extern void wazaDataBiosSetYubiwohuru(u8* ptr, u8 val);
extern void wazaDataBiosSetOujanosirusi(u8* ptr, u8 val);
extern void wazaDataBiosSetOumugaesi(u8* ptr, u8 val);
extern void wazaDataBiosSetYokodori(u8* ptr, u8 val);
extern void wazaDataBiosSetMajikku(u8* ptr, u8 val);
extern void wazaDataBiosSetMamoru(u8* ptr, u8 val);
extern void wazaDataBiosSetDageki(u8* ptr, u8 val);
extern void wazaDataBiosSetAddFightKoukaAvg(u8* ptr, u8 val);
extern void wazaDataBiosSetFightAttackMsgId(u8* ptr, u32 val);
extern void wazaDataBiosSetSeqId(u8* ptr, u16 val);
extern void wazaDataBiosSetFightKoukaDataId(u8* ptr, u16 val);
extern void wazaDataBiosSetIryoku(u8* ptr, u16 val);
extern void wazaDataBiosSetAvg(u8* ptr, u8 val);
extern void wazaDataBiosSetRangeId(u8* ptr, u8 val);
extern void wazaDataBiosSetPri(u8* ptr, u8 val);
extern void wazaDataBiosSetZokuseiDataId(u8* ptr, u8 val);
extern void wazaDataBiosSetPp(u8* ptr, u8 val);
extern void wazaDataBiosSetName(u8* ptr, u32 val);
extern u8 fn_8011C790(u8* ptr);
extern u8 fn_8011C7A8(u8* ptr);
extern u32 wazaDataBiosGetDoc(u8* ptr);
extern u32 wazaDataBiosGetWazawzxdataId(u8* ptr);
extern u8 wazaDataBiosGetPressure(u8* ptr);
extern u8 wazaDataBiosGetBouon(u8* ptr);
extern u8 wazaDataBiosGetNegoto(u8* ptr);
extern u8 wazaDataBiosGetNekonote(u8* ptr);
extern u8 wazaDataBiosGetMonomane(u8* ptr);
extern u8 wazaDataBiosGetYubiwohuru(u8* ptr);
extern u8 wazaDataBiosGetOujanosirusi(u8* ptr);
extern u8 wazaDataBiosGetOumugaesi(u8* ptr);
extern u8 wazaDataBiosGetYokodori(u8* ptr);
extern u8 wazaDataBiosGetMajikku(u8* ptr);
extern u8 wazaDataBiosGetMamoru(u8* ptr);
extern u8 wazaDataBiosGetDageki(u8* ptr);
extern u8 wazaDataBiosGetAddFightKoukaAvg(u8* ptr);
extern u32 wazaDataBiosGetFightAttackMsgId(u8* ptr);
extern u32 wazaDataBiosGetFightAttackTunagiMsgId(u8* ptr);
extern u16 wazaDataBiosGetSeqId(u8* ptr);
extern u16 wazaDataBiosGetFightKoukaDataId(u8* ptr);
extern s16 wazaDataBiosGetIryoku(u8* ptr);
extern u8 wazaDataBiosGetAvg(u8* ptr);
extern u8 wazaDataBiosGetRangeId(u8* ptr);
extern s32 wazaDataBiosGetPri(u8* ptr);
extern u8 wazaDataBiosGetZokuseiDataId(u8* ptr);
extern u8 wazaDataBiosGetPp(u8* ptr);
extern u32 wazaDataBiosGetName(u8* ptr);
extern s32 pokemonDpFilterDataBiosGetValue(u8* ptr);
extern u32 pokemonTokuseiDataBiosGetDoc(u8* ptr);
extern u32 pokemonTokuseiDataBiosGetName(u8* ptr);
extern u8 pokemonSeikakuRateDataBiosGetWaru(u8* ptr);
extern u8 pokemonSeikakuRateDataBiosGetKake(u8* ptr);
extern u8 fn_8011CC54(u8* ptr);
extern u8 fn_8011CC6C(u8* ptr);
extern u8 fn_8011CC84(u8* ptr);
extern u8 fn_8011CC9C(u8* ptr);
extern u8 fn_8011CCB4(u8* ptr);
extern u8 fn_8011CCCC(u8* ptr);
extern u8 fn_8011CCE4(u8* ptr);
extern s32 pokemonSeikakuDataBiosGetReliveNadenade(u8* ptr);
extern s32 pokemonSeikakuDataBiosGetReliveSodateya(u8* ptr);
extern s32 pokemonSeikakuDataBiosGetReliveCall(u8* ptr);
extern s32 pokemonSeikakuDataBiosGetReliveWalk(u8* ptr);
extern s32 pokemonSeikakuDataBiosGetReliveFightout(u8* ptr);
extern u8 pokemonSeikakuDataBiosGetNimblenessRateDataId(u8* ptr);
extern u8 pokemonSeikakuDataBiosGetSpeDefRateDataId(u8* ptr);
extern u8 pokemonSeikakuDataBiosGetSpeAtkRateDataId(u8* ptr);
extern u8 pokemonSeikakuDataBiosGetPhyDefRateDataId(u8* ptr);
extern u8 pokemonSeikakuDataBiosGetPhyAtkRateDataId(u8* ptr);
extern u32 pokemonSeikakuDataBiosGetName(u8* ptr);
extern void fn_8011CEA0(u8* ptr, u8 val);
extern void fn_8011CEB0(u8* ptr, u8 val);
extern void fn_8011CEC0(u8* ptr, u16 val);
extern void pokemonDataBiosSetPkxDataId(u8* ptr, u32 val);
extern void pokemonDataBiosSetNumPokemon(u8* ptr, u16 val);
extern void pokemonDataBiosSetNumZukan(u8* ptr, u16 val);
extern void pokemonDataBiosSetVoice(u8* ptr, u16 val);
extern void pokemonDataBiosSetDoc(u8* ptr, u32 val);
extern void pokemonDataBiosSetWeight(u8* ptr, u16 val);
extern void pokemonDataBiosSetHeight(u8* ptr, u16 val);
extern void pokemonDataBiosSetTypeName(u8* ptr, u32 val);
extern void pokemonDataBiosSetInitFriend(u8* ptr, u16 val);
extern void pokemonDataBiosSetSexRatio(u8* ptr, u8 val);
extern void pokemonDataBiosSetGet(u8* ptr, u8 val);
extern void pokemonDataBiosSetGrowDataId(u8* ptr, u8 val);
extern void pokemonDataBiosSetGiveExp(u8* ptr, u16 val);
extern void pokemonDataBiosSetName(u8* ptr, u32 val);
extern void pokemonBiosSetFightTrainerPokemonDataId(u8* ptr, u16 val);
extern void pokemonBiosSetPara1Amari(u8* ptr, u16 val);
extern void pokemonBiosSetAmari(u8* ptr, u16 val);
extern void pokemonBiosSetMailId(u8* ptr, u8 val);
extern void pokemonBiosSetPcboxMark(u8* ptr, u8 val);
extern void pokemonBiosSetFlagAmari(u8* ptr, u8 val);
extern void pokemonBiosSetFuseiFlag(u8* ptr, u8 val);
extern void pokemonBiosSetTamagoFlag(u8* ptr, u8 val);
extern void pokemonBiosSetPokerus(u8* ptr, u8 val);
extern void pokemonBiosSetFur(u8* ptr, u8 val);
extern void pokemonBiosSetDarkpokemonDataId(u8* ptr, u16 val);
extern void pokemonBiosSetItemDataId(u8* ptr, u16 val);
extern void pokemonBiosSetConditionAmari(u8* ptr, u32 val);
extern void pokemonBiosSetPoolExp(u8* ptr, u32 val);
extern void pokemonBiosSetExp(u8* ptr, u32 val);
extern void pokemonBiosSetCatchTrainerRnd(u8* ptr, u32 val);
extern void pokemonBiosSetCatchTrainerSex(u8* ptr, u8 val);
extern void pokemonBiosSetCatchBallId(u8* ptr, u8 val);
extern void pokemonBiosSetCatchLevel(u8* ptr, u8 val);
extern void pokemonBiosSetCatchFloorId(u8* ptr, u16 val);
extern void pokemonBiosSetRnd(u8* ptr, u32 val);
extern void pokemonBiosSetPokemonDataId(u8* ptr, u16 val);
extern u8 fn_8011E000(u8* ptr);
extern u8 fn_8011E018(u8* ptr);
extern u16 fn_8011E030(u8* ptr);
extern u32 pokemonDataBiosGetPkxDataId(u8* ptr);
extern u16 pokemonDataBiosGetNumPokemon(u8* ptr);
extern u16 pokemonDataBiosGetNumZukan(u8* ptr);
extern u16 pokemonDataBiosGetVoice(u8* ptr);
extern u32 pokemonDataBiosGetDoc(u8* ptr);
extern u16 pokemonDataBiosGetWeight(u8* ptr);
extern u16 pokemonDataBiosGetHeight(u8* ptr);
extern u32 pokemonDataBiosGetTypeName(u8* ptr);
extern u16 pokemonDataBiosGetInitFriend(u8* ptr);
extern u8 pokemonDataBiosGetSexRatio(u8* ptr);
extern u8 pokemonDataBiosGetGet(u8* ptr);
extern u8 pokemonDataBiosGetGrowDataId(u8* ptr);
extern u16 pokemonDataBiosGetGiveExp(u8* ptr);
extern u32 pokemonDataBiosGetName(u8* ptr);
extern u16 pokemonBiosGetFightTrainerPokemonDataId(u8* ptr);
extern u16 pokemonBiosGetPara1Amari(u8* ptr);
extern u16 pokemonBiosGetAmari(u8* ptr);
extern u8 pokemonBiosGetMailId(u8* ptr);
extern u8 pokemonBiosGetPcboxMark(u8* ptr);
extern u8 pokemonBiosGetFlagAmari(u8* ptr);
extern u8 pokemonBiosGetFuseiFlag(u8* ptr);
extern u8 pokemonBiosGetTamagoFlag(u8* ptr);
extern u8 pokemonBiosGetPokerus(u8* ptr);
extern u8 pokemonBiosGetFur(u8* ptr);
extern u16 pokemonBiosGetPoolFriend(u8* ptr);
extern u32 pokemonBiosGetPoolExp(u8* ptr);
extern u32 pokemonBiosGetDp(u8* ptr);
extern u16 pokemonBiosGetDarkpokemonDataId(u8* ptr);
extern u16 pokemonBiosGetFriend(u8* ptr);
extern u16 pokemonBiosGetHp(u8* ptr);
extern u16 pokemonBiosGetItemDataId(u8* ptr);
extern u32 pokemonBiosGetConditionAmari(u8* ptr);
extern u8 pokemonBiosGetLevel(u8* ptr);
extern u32 pokemonBiosGetExp(u8* ptr);
extern u32 pokemonBiosGetCatchTrainerRnd(u8* ptr);
extern u8 pokemonBiosGetCatchTrainerSex(u8* ptr);
extern u8 pokemonBiosGetCatchBallId(u8* ptr);
extern u8 pokemonBiosGetCatchLevel(u8* ptr);
extern u16 pokemonBiosGetCatchFloorId(u8* ptr);
extern u32 pokemonBiosGetRnd(u8* ptr);
extern void heroBiosSetHomePlace(u8* ptr, u8 val);
extern u8 heroBiosGetHomePlace(u8* ptr);
extern u8 heroBiosGetHizukiFlag(u8* ptr);
extern void heroBiosSetHizukiFlag(u8* ptr, u8 val);
extern u32 heroBiosGetPokecouponAll(u8* ptr);
extern u32 heroBiosGetPokecoupon(u8* ptr);
extern u32 heroBiosGetPokedoru(u8* ptr);
extern u8 heroBiosGetBadge08Flag(u8* ptr);
extern u8 heroBiosGetBadge07Flag(u8* ptr);
extern u8 heroBiosGetBadge06Flag(u8* ptr);
extern u8 heroBiosGetBadge05Flag(u8* ptr);
extern u8 heroBiosGetBadge04Flag(u8* ptr);
extern u8 heroBiosGetBadge03Flag(u8* ptr);
extern u8 heroBiosGetBadge02Flag(u8* ptr);
extern u8 heroBiosGetBadge01Flag(u8* ptr);
extern void heroBiosSetBadge08Flag(u8* ptr, u8 val);
extern void heroBiosSetBadge07Flag(u8* ptr, u8 val);
extern void heroBiosSetBadge06Flag(u8* ptr, u8 val);
extern void heroBiosSetBadge05Flag(u8* ptr, u8 val);
extern void heroBiosSetBadge04Flag(u8* ptr, u8 val);
extern void heroBiosSetBadge03Flag(u8* ptr, u8 val);
extern void heroBiosSetBadge02Flag(u8* ptr, u8 val);
extern void heroBiosSetBadge01Flag(u8* ptr, u8 val);
extern u8 heroBiosGetSexDataId(u8* ptr);
extern void heroBiosSetSexDataId(u8* ptr, u8 val);
extern void heroBiosSetRnd(u8* ptr, u32 val);
extern u32 heroBiosGetRnd(u8* ptr);
extern u32 GSeffectGetUpdateGapp(void);
extern u16 pokemonBiosGetPokemonDataId(u8* ptr);
extern u32 fn_80128E24(void);
extern void* floorReadMapPreFunc(void* owner, u32 param, u32 alloc_size);
extern void* floorReadScriptPreFunc(void* owner, u32 param, u32 alloc_size);
extern void* floorReadFontPreFunc(void* owner, u32 param, u32 alloc_size);
extern void* floorReadMsgPreFunc(void* owner, u32 param, u32 alloc_size);
extern u32 _unloadFlare__FPvUlUl(void);
extern u32 _unloadParticles__FPvUlUl(void);
extern u32 _unloadCamera__FPvUlUl(void);
extern u32 _unloadLight__FPvUlUl(void);
extern u32 _unloadColsys__FPvUlUl(void);
extern u32 _unloadTexture__FPvUlUl(void);
extern u32 floorReadMakeFogResID(u32 val);
extern u32 floorReadMakeCameraResID(u32 val);
extern u32 floorReadMakeLightResID(u32 val);
extern u32 floorReadMakeModelResID(u32 val);
extern u32 floorDataBiosGetShadowReciveNum(void* ptr);
extern void* floorDataBiosGetShadowReciveID(void* ptr, u32 idx);
extern void* floorDataBiosGetShadowLightID(void* ptr);
extern void* floorDataBiosGetSunResID(void* ptr);
extern void fn_8011553C(void* obj, u32 val);
extern void floorDataBiosSetMapResID(void* obj, u32 val);
extern u32 floorDataBiosGetFileGroupID(u8* entry);
extern void* floorDataBiosGetCurrentPtr(void);
extern void* fn_80115CB4(u32 param);
extern void fn_80115D64(u32 r25, u32 r26);
extern void heroDispatchEvent(s8 kind, u32 arg);
extern void floorCharacterBiosSetVisibility(u8* ptr, u8 val);
extern void floorCharacterBiosSetPos(u8* dst, f32* src);
extern u32 floorCharacterBiosGetTalkSctID(void* ptr);
extern u32 floorCharacterBiosGetMoveSctID(void* ptr);
extern u32 floorCharacterBiosGetNameID(void* ptr);
extern u32 floorCharacterBiosGetTalkWallThrough(u8* ptr);
extern u32 floorCharacterBiosGetTalkEndType(u8* ptr);
extern u32 floorCharacterBiosGetTalkStartType(u8* ptr);
extern u32 floorCharacterBiosGetMoveType(u8* ptr);
extern u32 floorCharacterBiosGetLoadInit(u8* ptr);
extern u32 floorCharacterBiosGetVisibility(u8* ptr);
extern void* floorCharacterBiosGetPeopleInfoPtr(u8* ptr);
extern void floorCharacterBiosGetByIndex(u32 arg);
extern void fn_80117154(void);
extern void fn_80117164(void);
extern void fn_801171C8(void);
extern void fn_80117330(f32 arg);
extern u32 fn_801174C4(void);
extern void std136_fieldClearFlag_8047ad71(void);
extern void fn_80117500(void);
extern u32 fn_80117AD4(void);
extern u8 std171_mapSetTextureChange(u32 arg1);
extern void tvRelease(void);
extern void tvRender(void);
extern void tvInitFloor(void* arg);
extern void fn_80118020(void);
extern void tvRelease2(void);
extern void fn_80118100(void);
extern void gsParticleDraw(u32 a, u8 b);
extern void fn_80118A68(u8* obj, u32 notify);
extern void fn_80118C20(u8* arg1, void* arg2, u32 arg3, u32 arg4, u32 arg5);
extern void fn_80118C88(void* obj);
extern void fn_80118CAC(void* obj);
extern void fn_80118CD0(void* obj);
extern void fn_80118CF4(void* obj);
extern void fn_80118D18(void* obj);
extern void fn_80118D3C(void* obj);
extern void fn_80118D60(void* obj);
extern void fn_80118D84(void* obj);
extern s32 fn_80118DA8(u8* ptr);
extern void fn_80118DE0(u8* arg1, f32* arg2, u32 arg3, u32 arg4);
extern void fn_80118E8C(u8* arg1, f32* arg2, u32 arg3, u32 arg4, u32 arg5);
extern void fn_80118F04(u8* arg1, f32* arg2, u32 arg3, u32 arg4, u32 arg5);
extern void fn_80118F7C(u8* obj, void* arg);
extern void fn_80118FB0(u8* obj, u8* desc, u32 state, u32 byte5, u32 init_from_zero, u32 attach_model);
extern void gsParticleInit(u32 count1, u32 count2);
extern void fn_8011A280(u8* arg1, u16 arg2, u32 arg3);
extern s32 fn_8011A3E4(void* obj, u16 val);
extern void fn_8011A570(u8* arg1, u16 arg2, u32 arg3);
extern s32 fn_8011A6D4(void* obj, u16 val);
extern s32 fn_8011A860(void* obj, u16 val);
extern void fn_8011A9EC(u8* arg1, u16 arg2, u32 arg3);
extern void fn_8011AB50(u8* arg1, u16 arg2, u32 arg3);
extern s32 fn_8011ACB4(void* obj, u16 val);
extern s32 fn_8011AE40(void* obj, u16 val);
extern void fn_8011AFCC(u8* arg1, u16 arg2, u32 arg3);
extern s32 fn_8011B130(void* obj, u16 val);
extern void fn_8011B2C0(void* obj, u16 id, u16 arg3);
extern s32 fn_8011B444(void* obj, u16 val);
extern s32 fn_8011B67C(void* obj, u16 val);
extern void fn_8011B788(void* obj, u16 id);
extern void fn_8011B950(u8* base, u16 count);
extern u32 fn_8011BA0C(u8 type);
extern u32 wazaIsWazaTypeId(u32 key, u8 target);
extern u8 wazaGetMaxPP(u32 arg1, u8 arg2);
extern u32 pokemonNakigoeDataBiosGetDataAddress(u8* ptr);
extern void* pokemonDpFilterDataBiosGetPtr(u16 idx);
extern s8 pokemonFriendFilterDataBiosGetValue(u8* ptr, u8 idx);
extern void* pokemonFriendFilterDataBiosGetPtr(u16 idx);
extern void* pokemonTokuseiDataBiosGetPtr(u16 idx);
extern void* pokemonSeikakuRateDataBiosGetPtr(u8 idx);
extern u8 fn_8011CBF4(u8* ptr, u8 idx);
extern u8 fn_8011CC24(u8* ptr, u8 idx);
extern void* pokemonSeikakuDataBiosGetPtr(u8 idx);
extern u32 pokemonGrowDataBiosGetExp(u8* ptr, u8 idx);
extern void* pokemonGrowDataBiosGetPtr(u8 idx);
extern void fn_8011CED0(u8* ptr, u16 idx, u8 val);
extern void pokemonDataBiosSetKowaza(u8* ptr, u16 idx, u16 val);
extern void pokemonDataBiosSetTukamaetaFlag(u8* ptr);
extern void pokemonDataBiosSetMitaFlag(u8* ptr);
extern void pokemonDataBiosSetColor(u8* ptr, u16 idx, u8 val);
extern void pokemonDataBiosSetStatusFaceMenuSpriteId(u8* ptr, u16 idx, u16 val);
extern void pokemonDataBiosSetPokebodyId(u8* ptr, u16 idx, u32 val);
extern void pokemonDataBiosSetWazaMcn(u8* ptr, u16 idx, u8 val);
extern void pokemonDataBiosSetGetWazaDataId(u8* ptr, u16 idx, u16 val);
extern void pokemonDataBiosSetGetWazaLevel(u8* ptr, u16 idx, u8 val);
extern void pokemonDataBiosSetSinkaPokemonDataId(u8* ptr, u16 idx, u16 val);
extern void pokemonDataBiosSetSinkaBuff(u8* ptr, u16 idx, u16 val);
extern void pokemonDataBiosSetSinkaKind(u8* ptr, u16 idx, u8 val);
extern void pokemonDataBiosSetTokuseiDataId(u8* ptr, u16 idx, u8 val);
extern void pokemonDataBiosSetZokuseiDataId(u8* ptr, u16 idx, u8 val);
extern void pokemonDataBiosSetItemDataId(u8* ptr, u16 idx, u16 val);
extern void pokemonDataBiosSetGiveNimblenessEffort(void* ptr, u16 val);
extern void pokemonDataBiosSetGiveSpeDefEffort(void* ptr, u16 val);
extern void pokemonDataBiosSetGiveSpeAtkEffort(void* ptr, u16 val);
extern void pokemonDataBiosSetGivePhyDefEffort(void* ptr, u16 val);
extern void pokemonDataBiosSetGivePhyAtkEffort(void* ptr, u16 val);
extern void pokemonDataBiosSetGiveMaxHpEffort(void* ptr, u16 val);
extern void pokemonDataBiosSetBasisNimbleness(void* ptr, u16 val);
extern void pokemonDataBiosSetBasisSpeDef(void* ptr, u16 val);
extern void pokemonDataBiosSetBasisSpeAtk(void* ptr, u16 val);
extern void pokemonDataBiosSetBasisPhyDef(void* ptr, u16 val);
extern void pokemonDataBiosSetBasisPhyAtk(void* ptr, u16 val);
extern void pokemonDataBiosSetBasisMaxHp(void* ptr, u16 val);
extern void pokemonBiosSetEventGetFlag(u8* ptr, u8 val);
extern void pokemonBiosSetTokuseiFlag(u8* ptr, u8 val);
extern void pokemonBiosSetAmariRibbon(void* ptr, u8 val);
extern void pokemonBiosSetWorldRibbon(void* ptr, u8 val);
extern void pokemonBiosSetEarthRibbon(void* ptr, u8 val);
extern void pokemonBiosSetNationalRibbon(void* ptr, u8 val);
extern void pokemonBiosSetCountryRibbon(void* ptr, u8 val);
extern void pokemonBiosSetSkyRibbon(void* ptr, u8 val);
extern void pokemonBiosSetLandRibbon(void* ptr, u8 val);
extern void pokemonBiosSetMarineRibbon(void* ptr, u8 val);
extern void pokemonBiosSetGanbaRibbon(void* ptr, u8 val);
extern void pokemonBiosSetBromideRibbon(void* ptr, u8 val);
extern void pokemonBiosSetVictoryRibbon(void* ptr, u8 val);
extern void pokemonBiosSetWinningRibbon(void* ptr, u8 val);
extern void pokemonBiosSetChampRibbon(void* ptr, u8 val);
extern void pokemonBiosSetStrongMedal(void* ptr, u8 val);
extern void pokemonBiosSetCleverMedal(void* ptr, u8 val);
extern void pokemonBiosSetCuteMedal(void* ptr, u8 val);
extern void pokemonBiosSetBeautifulMedal(void* ptr, u8 val);
extern void pokemonBiosSetStyleMedal(void* ptr, u8 val);
extern void pokemonBiosSetStrong(void* ptr, u8 val);
extern void pokemonBiosSetClever(void* ptr, u8 val);
extern void pokemonBiosSetCute(void* ptr, u8 val);
extern void pokemonBiosSetBeautiful(void* ptr, u8 val);
extern void pokemonBiosSetStyle(void* ptr, u8 val);
extern void pokemonBiosSetDp(u8* ptr, s32 val);
extern void pokemonBiosSetFriend(u8* ptr, u16 val);
extern void pokemonBiosSetNimblenessRnd(u8* ptr, u16 val);
extern void pokemonBiosSetSpeDefRnd(u8* ptr, u16 val);
extern void pokemonBiosSetSpeAtkRnd(u8* ptr, u16 val);
extern void pokemonBiosSetPhyDefRnd(u8* ptr, u16 val);
extern void pokemonBiosSetPhyAtkRnd(u8* ptr, u16 val);
extern void pokemonBiosSetMaxHpRnd(u8* ptr, u16 val);
extern void pokemonBiosSetNimblenessEffort(u8* ptr, u16 val);
extern void pokemonBiosSetSpeDefEffort(u8* ptr, u16 val);
extern void pokemonBiosSetSpeAtkEffort(u8* ptr, u16 val);
extern void pokemonBiosSetPhyDefEffort(u8* ptr, u16 val);
extern void pokemonBiosSetPhyAtkEffort(u8* ptr, u16 val);
extern void pokemonBiosSetMaxHpEffort(u8* ptr, u16 val);
extern void pokemonBiosSetNimbleness(void* ptr, u16 val);
extern void pokemonBiosSetSpeDef(void* ptr, u16 val);
extern void pokemonBiosSetSpeAtk(void* ptr, u16 val);
extern void pokemonBiosSetPhyDef(void* ptr, u16 val);
extern void pokemonBiosSetPhyAtk(void* ptr, u16 val);
extern void pokemonBiosSetMaxHp(void* ptr, u16 val);
extern void pokemonBiosSetHp(u8* ptr, u16 val);
extern void pokemonBiosSetPokemonWazaPpCount(u8* ptr, u32 arg2, u8 arg3);
extern void pokemonBiosSetPokemonWazaPp(u32 arg1, s32 arg2, u8 maxVal);
extern void pokemonBiosSetPokemonWazaDataId(void* ctx, u32 p1, u32 value);
extern void pokemonBiosSetLevel(u8* ptr, u8 val);
extern void pokemonBiosSetPoolFriend(u8* ptr, u16 val);
extern void pokemonBiosSetNicknameOrgPtr(u8* ptr, void* src);
extern void pokemonBiosSetNicknamePtr(u8* ptr, void* src);
extern void pokemonBiosSetCatchTrainerNamePtr(u8* ptr, void* src);
extern u8 fn_8011E048(u8* ptr, u16 idx);
extern u16 pokemonDataBiosGetKowaza(u8* ptr, u16 idx);
extern void* pokemonDataBiosGetTukamaetaFlag(u8* ptr);
extern void* pokemonDataBiosGetMitaFlag(u8* ptr);
extern u8 pokemonDataBiosGetWazaMcn(u8* ptr, u16 idx);
extern u8 pokemonDataBiosGetTokuseiDataId(u8* ptr, u16 idx);
extern u8 pokemonDataBiosGetZokuseiDataId(u8* ptr, u16 idx);
extern u16 pokemonDataBiosGetGiveSpeDefEffort(void* ptr);
extern u16 pokemonDataBiosGetGiveSpeAtkEffort(void* ptr);
extern u16 pokemonDataBiosGetGivePhyDefEffort(void* ptr);
extern u16 pokemonDataBiosGetGivePhyAtkEffort(void* ptr);
extern u16 pokemonDataBiosGetGiveMaxHpEffort(void* ptr);
extern u16 pokemonDataBiosGetBasisNimbleness(void* ptr);
extern u16 pokemonDataBiosGetBasisSpeDef(void* ptr);
extern u16 pokemonDataBiosGetBasisSpeAtk(void* ptr);
extern u16 pokemonDataBiosGetBasisPhyDef(void* ptr);
extern u16 pokemonDataBiosGetBasisPhyAtk(void* ptr);
extern u16 pokemonDataBiosGetBasisMaxHp(void* ptr);
extern void* pokemonDataBiosGetPtr(u16 idx);
extern u8 pokemonBiosGetEventGetFlag(u8* ptr);
extern u8 pokemonBiosGetAmariRibbon(void* ptr);
extern u8 pokemonBiosGetWorldRibbon(void* ptr);
extern u8 pokemonBiosGetEarthRibbon(void* ptr);
extern u8 pokemonBiosGetNationalRibbon(void* ptr);
extern u8 pokemonBiosGetCountryRibbon(void* ptr);
extern u8 pokemonBiosGetSkyRibbon(void* ptr);
extern u8 pokemonBiosGetLandRibbon(void* ptr);
extern u8 pokemonBiosGetMarineRibbon(void* ptr);
extern u8 pokemonBiosGetGanbaRibbon(void* ptr);
extern u8 pokemonBiosGetBromideRibbon(void* ptr);
extern u8 pokemonBiosGetVictoryRibbon(void* ptr);
extern u8 pokemonBiosGetWinningRibbon(void* ptr);
extern u8 pokemonBiosGetChampRibbon(void* ptr);
extern u8 pokemonBiosGetStrongMedal(void* ptr);
extern u8 pokemonBiosGetCleverMedal(void* ptr);
extern u8 pokemonBiosGetCuteMedal(void* ptr);
extern u8 pokemonBiosGetBeautifulMedal(void* ptr);
extern u8 pokemonBiosGetStyleMedal(void* ptr);
extern u8 pokemonBiosGetStrong(void* ptr);
extern u8 pokemonBiosGetClever(void* ptr);
extern u8 pokemonBiosGetCute(void* ptr);
extern u8 pokemonBiosGetBeautiful(void* ptr);
extern u8 pokemonBiosGetStyle(void* ptr);
extern void* fn_8011EDC4(u8* ptr, u16 idx);
extern u16 pokemonBiosGetNimblenessRnd(void* ptr);
extern u16 pokemonBiosGetSpeDefRnd(void* ptr);
extern u16 pokemonBiosGetSpeAtkRnd(void* ptr);
extern u16 pokemonBiosGetPhyDefRnd(void* ptr);
extern u16 pokemonBiosGetPhyAtkRnd(void* ptr);
extern u16 pokemonBiosGetMaxHpRnd(void* ptr);
extern u16 pokemonBiosGetNimblenessEffort(void* ptr);
extern u16 pokemonBiosGetSpeDefEffort(void* ptr);
extern u16 pokemonBiosGetSpeAtkEffort(void* ptr);
extern u16 pokemonBiosGetPhyDefEffort(void* ptr);
extern u16 pokemonBiosGetPhyAtkEffort(void* ptr);
extern u16 pokemonBiosGetMaxHpEffort(void* ptr);
extern u16 pokemonBiosGetNimbleness(void* ptr);
extern u16 pokemonBiosGetSpeDef(void* ptr);
extern u16 pokemonBiosGetSpeAtk(void* ptr);
extern u16 pokemonBiosGetPhyDef(void* ptr);
extern u16 pokemonBiosGetPhyAtk(void* ptr);
extern u16 pokemonBiosGetMaxHp(void* ptr);
extern u8 pokemonBiosGetPokemonWazaPpCount(void* ctx, u32 p1);
extern u8 pokemonBiosGetPokemonWazaPp(void* ctx, u32 p1);
extern u16 pokemonBiosGetPokemonWazaDataId(void* ctx, u32 p1);
extern void* fn_8011F474(u8* ptr, u16 idx);
extern void* pokemonBiosGetNicknameOrgPtr(void* ptr);
extern void* pokemonBiosGetNicknamePtr(void* ptr);
extern void* pokemonBiosGetCatchTrainerNamePtr(void* ptr);
extern void* pokemonBiosGetAttest(void* ptr);
extern void pokemonWazaBiosCopy(u32* dst, u32* src);
extern void pokemonBiosCopy(u32* dst, u32* src);
extern u32 fn_8011F634(u8* ptr);
extern u32 fn_8011F6D8(u8* ptr);
extern void pokemonSetDp(u8* ptr, f32 f1);
extern f32 pokemonGetDp(u8* ptr);
extern u8 pokemonIsDarkPokemon(u32 arg);
extern void pokemonToMenuPokemonStatusSubBar(u8* ptr, u8* out);
extern void pokemonToMenuWazaStatus(u8* ptr, u8* out);
extern void pokemonGetMezamerupower(u8* ptr, u16* out1, u16* out2);
extern u32 pokemonGetNowLevelToExp(u8* ptr);
extern u32 pokemonGetJoutaiMsgId(u8* ptr);
extern u32 pokemonGetJoutaiMenuSpriteId(u8* ptr);
extern u32 pokemonGetJoutaiDataId(u8* ptr);
extern void fn_80121484(void* obj, u32 arg2, u32 arg3);
extern void fn_801214FC(void* obj, u32 arg2, u32 arg3);
extern void fn_801215E4(void* obj, u32 arg2, u32 arg3);
extern void fn_8012173C(void* obj, u32 arg2, u32 arg3);
extern void fn_801217B4(void* obj, u32 arg2, u32 arg3);
extern void fn_8012190C(void* obj, u32 arg2, u32 arg3);
extern void fn_801219F4(void* obj, u32 arg2, u32 arg3);
extern void fn_80121B4C(void* obj, u32 arg2);
extern void pokemonReplace(u32* arg1, u32* arg2);
extern void* pokemonCreateSequence(void* arg);
extern void pokemonSetSequenceStatus(u8* ptr, void* obj);
extern u8 pokemonIsNokoriHpFollowing(u8* ptr, s32 b);
extern u32 pokemonIsJoutaiKaragenki(u8* ptr);
extern u32 pokemonIsJoutaiNormal(u8* ptr);
extern u16 pokemonGetSoubiItemBuff(u8* ptr);
extern u16 pokemonGetSoubiItemDataId(u32 arg);
extern u32 pokemonDoItemSoubi(u8* ptr, register u32 arg2, u8 flag);
extern u8 pokemonGetSex(u8* ptr);
extern void pokemonSetOnDarkPokemonFlag(u8* ptr, u8 flag);
extern void pokemonSetOnZukanFlag(u8* ptr, u8 flag);
extern u32 pokemonGetOboeWazaDataBanme(u8* ptr, u32 arg2);
extern s32 pokemonOboeWaza(u8* ptr, u8 target, u8* buf_ptr, u8* counter_ptr);
extern u16 pokemonGetOboeWazaDataId(u8* ptr, u8 arg2, u8* counter_ptr);
extern s32 pokemonSearchWazaDataId(u8* ptr, u16 target);
extern void pokemonWazaReplace(void* ptr, u32 idx, u32 arg);
extern void pokemonWazaCreate(u8* ptr, u32 slot, u32 val);
extern void pokemonSetCatchStatus(u8* arg1, u32 arg2, u8 arg3, u16 arg4, u8 arg5, u32 arg6, u32 arg7);
extern u32 pokemonCheckValid(u8* ptr);
extern u16 pokemonGetTokuseiDataId(u8* ptr);
extern void pokemonInitAry(u8* ptr, u16 count);
extern void pokemonInitDarkPokemon(u8* ptr);
extern void pokemonInitJoutai(u8* ptr);
extern void pokemonWazaInit(u8* ptr, u32 arg2);
extern u32 pokemonCheckRare(void* ctx);
extern void pokemonGrowBasisStatus(void* ptr, u32 arg2);
extern void* fn_80128CC0(void* ptr);
extern void* fn_80128CDC(void* ptr);
extern void* fn_80128CF8(void* ptr);
extern void* fn_80128D14(void* ptr);
extern void* fn_80128D30(void* ptr);
extern void* fn_80128D4C(void* ptr);
extern void* fn_80128D68(void* ptr);
extern void* fn_80128D80(void* ptr);
extern void* fn_80128D9C(void* ptr);
extern void* fn_80128DB8(void* ptr);
extern void* fn_80128DD4(void* ptr);
extern void* fn_80128DEC(void* ptr);
extern u32 fn_80128E04(void* ptr);
extern void fn_80128E14(void* ptr);
extern void* fn_80128E2C(void);
extern u32 savedataGetStatus(u8* arg1, u16 arg2);
extern void heroDecPokecoupon(u8* ptr, s32 offset);
extern void heroAddPokecoupon(u8* ptr, s32 offset);
extern void heroDecPokedoru(u8* ptr, u32 offset);
extern void fn_80129514(u8* ptr, s32 arg2, s32 arg3);
extern s32 fn_8012959C(u8* ptr, u32 arg2, u32 arg3, u32 arg4);
extern s32 fn_80129650(u8* ptr, u32 arg2, u32 arg3, u32 arg4);
extern u32 fn_80129718(u8* ptr, u32 arg2);
extern u32 heroHizukiItemGetItemAryPtr(u8* ptr, u16* out_a, u16* out_b, u8* out_c, u8* out_d);
extern void heroCheckSetMonohiroiAllTemotiPokemon(u8* arg1);
extern s32 heroItemCheckAddItemDataId(u8* ptr, u32 arg2);
extern void fn_80129948(u8* arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5, u32 arg6, u32 arg7);
extern s32 heroItemDecItemDataId(u8* ptr, u32 arg2, u32 arg3, u32 arg4);
extern u32 heroItemCheckHaveItemDataId(u8* ptr, u32 arg2);
extern u32 fn_80129D64(u8* ptr, u8* arg2);
extern s32 heroGetPokemon(u8* ptr, void* buf, u8 flag);
extern s32 heroCatchPokemon(u8* ptr, u8* buf, u32 arg3, u16 arg4, u8 flag);
extern void heroBiosCopy(u32* dst, u32* src);
extern void cbTsureFriend__Fl15FootStepCounterl(s32 arg);
extern void heroMoveSetLockFrame(s32 val);
extern void heroMoveAddAutoEvent(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void heroMoveSetEventList(u8 type, void* src, u32 val);
extern s32 heroMoveAddStepCallback(void (*func)(s32 arg), s32 arg);
extern u32 heroMoveSetNeckMode(s32 idx, HeroMoveNeckMode state);
extern u32 heroMoveIsMember(s32 idx);
extern s32 heroMoveDismissMember(s32 idx);
extern void heroPokemonGetCelebi(u8* arg1);
extern void fn_80130770(u8* arg1);
extern void heroPokemonGetHouou(u8* arg1);
extern void fn_801309A0(u32 arg1);
extern void* floorReadScriptPostFunc(u32 a, u32 b);
extern void* floorReadFontPostFunc(u32 a, u32 b);
extern void* floorReadMsgPostFunc(u32 a, u32 b);
extern void* floorReadNormalPreFunc(u32 a, u32 b, u32 size);
extern void* floorDataBiosGetPosListPtr(u8* ptr);
extern void* floorDataBiosGetCharInfo(u8* ptr, u32 idx);
extern u32 floorDataBiosGetCharNum(u8* ptr);
extern u32 floorDataBiosGetPostFunc(u8* ptr);
extern u32 floorDataBiosGetMainFunc(u8* ptr);
extern u32 floorDataBiosGetPreFunc(u8* ptr);
extern u32 fn_80115840(u8* ptr);
extern u32 fn_80115888(u8* ptr);
extern u32 fn_801158D0(u8* ptr);
extern u32 fn_80115918(u8* ptr);
extern u32 fn_80115960(u8* ptr);
extern u32 fn_801159A8(u8* ptr);
extern u32 floorDataBiosGetFloorID(u8* ptr);
extern u8 floorDataBiosGetFloorKind(u8* ptr);
extern u32 floorDataBiosGetMapResID(u8* ptr);
extern u8 floorDataBiosGetArea(u8* ptr);
extern s32 floorEventChangeTresure(u32 index, u16 val, u8 byte);
extern s32 floorCharacterBiosGetRot(void* a, void* b);
extern u32 floorCharacterBiosGetPos(u8* ptr, void* obj);
extern u32 fn_801170A4(u8* arg1, u32 arg2);
extern u8 pokemonDataBiosGetColor(u8* ptr, s32 idx);
extern u16 pokemonDataBiosGetStatusFaceMenuSpriteId(u8* ptr, s32 idx);
extern u32 pokemonDataBiosGetPokebodyId(u8* ptr, s32 idx);
extern u16 pokemonDataBiosGetGetWazaDataId(u8* ptr, s32 idx);
extern u8 pokemonDataBiosGetGetWazaLevel(u8* ptr, s32 idx);
extern u16 pokemonDataBiosGetGiveNimblenessEffort(u8* ptr);
extern u8 pokemonBiosGetTokuseiFlag(u8* ptr);
extern void* pokemonBiosGetInitDp(u8* ptr);
extern u32 pokemonBiosGetDarkFlag(u8* ptr);
extern s32 pokemonGetTasteLike(u8* ptr, void* arg2);
extern s32 pokemonIsHpMantan(u8* ptr);
extern s32 fn_80121574(void* a, u16 b);
extern s32 fn_8012165C(void* a, u16 b);
extern s32 fn_801216CC(void* a, u16 b);
extern s32 fn_8012182C(void* a, u16 b);
extern s32 fn_8012189C(void* a, u16 b);
extern s32 fn_80121984(void* a, u16 b);
extern s32 fn_80121A6C(void* a, u16 b);
extern u8 pokemonGetAnnonKatati(u32 val);
extern u32 pokemonGetLevelToExp(u8* ptr, u8 idx);
extern s32 pokemonGetNowHpPercentage(u8* ptr);
extern u16 pokemonGetNowHpWaruValue(u8* ptr, s32 b);
extern u16 pokemonGetMaxHpWaruValue(u8* ptr, s32 b);
extern u16 pokemonGetSoubiItemSoubiDataId(u8* ptr);
extern void pokemonSetTokuseiFlag(u8* ptr, u32 arg2);
extern u32 heroCheckValid(u8* ptr);
extern void heroMoveGetHeroRot(u32 param);
extern void heroMoveGetHeroPos(u32 param);
extern u8 heroMoveGetResID(u32* out_zero, u32* out_val, s32 index);

/*
 * Resource IDs of the two members' field models, copied to the stack as one
 * block from the TU's .sdata2 pool (the 8-byte {100, 101} at lbl_8047D030).
 * Retail keeps that block copy even where the member index is a constant
 * (fn_8013024C, heroMoveSyncWithHero): the stores stay and only the loaded
 * ID is forwarded. A plain u32[2] initializer or element-wise copy is split
 * into scalars by MWCC and loses those stores; an aggregate-typed table is
 * copied as a block, like the floor and theme tables below.
 *
 * The entries are s32 while GetResID's out-parameter is u32 (IDs 100/101,
 * so the conversion is value-preserving). The element type must differ from
 * *id's: with identical types, `*id = ids.id[0]` is a plain copy of the
 * table's first word, and MWCC's frontend copy propagation (IRO log: "Found
 * propagatable assignment" for the id store) replaces the ID with a stack
 * read at each use, so fn_8013024C reloads it after fn_80188AF4 (lwz
 * r4,0x18(r1)). Retail keeps it in r30 (mr r4,r30 at both calls), which is
 * what the converting assignment gives.
 */
typedef struct HeroMoveResIDTable {
    s32 id[2];
} HeroMoveResIDTable;

/*
 * Party member index. XD's mangled names type it as an enum,
 * HEROMOVE_MEMBER (getResID__FPUlPUl15HEROMOVE_MEMBER,
 * getModel__F15HEROMOVE_MEMBER, procStep__F15HEROMOVE_MEMBER, ...;
 * TeamOrre/xd-decomp config/GXXE01/symbols.txt), and Colosseum keeps the
 * same names (updateAnimation__Ff15HEROMOVE_MEMBER,
 * moveLeader__F15HEROMOVE_MEMBER). The enumerator names are not known.
 *
 * The type is visible in the loops of heroMoveTermEvent/heroMoveInitEvent:
 * with an enum-typed counter MWCC strength-reduces only the scaled indices
 * (member*0x20 and member*4 kept as offsets, the base re-materialised and
 * added, lwzx from the table), as retail does; an s32 counter is reduced to
 * pointers instead (lhz 4(rN), lwz 0(rN)).
 *
 * getResID and getModel take the member as `int` here. Their C++ parameter
 * was HEROMOVE_MEMBER; a reduced test of heroMoveInitEvent compiled with
 * GC/1.3 -O4,p gives the same code in C++ with HEROMOVE_MEMBER parameters
 * as in C with int parameters, while a C enum parameter does not (the
 * second table lookup is not reduced: slwi/lwzx per iteration). initFloor
 * (s32 counter, pointer-reduced lookups in retail) is unchanged by int.
 */
typedef enum HEROMOVE_MEMBER {
    HEROMOVE_MEMBER_0,
    HEROMOVE_MEMBER_1,
    HEROMOVE_MEMBER_NUM
} HEROMOVE_MEMBER;

/*
 * Resource group/ID of a party member's field model (group 0, IDs 100/101).
 * XD getResID__FPUlPUl15HEROMOVE_MEMBER (0x801520F4; TeamOrre/xd-decomp
 * symbols.txt, trevor403/xd-asm): the table lookup behind the exported
 * heroMoveGetResID, which XD implements as a call to it. XD's setPos,
 * setDisp, getModel, getObjID and heroMoveSetNeckMode call it directly.
 */
static inline u8 getResID(u32* group, u32* id, int member)
{
    HeroMoveResIDTable ids = {100, 101};

    if (member < 0 || member >= 2) {
        return FALSE;
    }
    *group = 0;
    *id = ids.id[member];
    return TRUE;
}

/*
 * Party-membership test as the callers below inline it: a u8 result built
 * by branches (li 0 / li 1, then clrlwi 24), unlike the exported
 * heroMoveIsMember, which returns the flag bit itself.
 */
static inline u8 heroMoveCheckMember(s32 member)
{
    if (member < 0 || member >= 2) {
        return FALSE;
    }
    if (!(lbl_80426BD0.member[member].flags & 1)) {
        return FALSE;
    }
    return TRUE;
}

/* Neck mode of a member; 2 when it is not in the party. */
static inline HeroMoveNeckMode heroMoveGetNeckMode(s32 member)
{
    if (heroMoveCheckMember(member)) {
        return lbl_80426BD0.member[member].neckMode;
    }
    return HERO_MOVE_NECK_NONE;
}

/* Field model of a party member (XD getModel__F15HEROMOVE_MEMBER, 0x80150298). */
static inline void* heroMoveGetModel(int member)
{
    extern void* GSresGetResource(u32 group, u32 id);
    u32 group;
    u32 id;

    getResID(&group, &id, member);
    return GSresGetResource(group, id);
}

/*
 * Object ID of a party member's person (XD getObjID__F15HEROMOVE_MEMBER,
 * 0x80152094; TeamOrre/xd-decomp symbols.txt, trevor403/xd-asm): getResID,
 * the person lookup, then the person's object ID, -1 when either fails.
 * XD reads the ID at +0x1C of its person record, Colosseum at +0x30.
 */
static inline s32 getObjID(s32 member)
{
    extern u32 peopleIsInGroup(u32 group, u32 id);
    extern void* peopleSearchID(u32 id);
    u32 group;
    u32 id;
    void* person;

    if (!getResID(&group, &id, member)) {
        return -1;
    }
    person = peopleSearchID(peopleIsInGroup(group, id));
    if (person == NULL) {
        return -1;
    }
    return *(s32*)((u8*)person + 0x30);
}

/*
 * Model position/rotation accessors (XD heroMove.cpp: getPos__FP5GSvec15HEROMOVE_MEMBER
 * 0x8015015C, getRot__FP5GSvec15HEROMOVE_MEMBER 0x801500DC,
 * setPos__F15HEROMOVE_MEMBERP5GSvec 0x80150114, setRot__F15HEROMOVE_MEMBERP5GSvec
 * 0x801500A8; TeamOrre/xd-decomp symbols.txt, trevor403/xd-asm). getPos and
 * getRot also expand in heroMoveGetHeroPos / heroMoveGetHeroRot (same table,
 * range check, GSresGetResource, GSmodelGetPosition/Rotation sequence);
 * Colosseum's getPos has no NULL check on the model.
 */
static inline void getPos(HeroMoveVec* pos, int member)
{
    extern void GSmodelGetPosition(void* model, void* out);

    GSmodelGetPosition(heroMoveGetModel(member), pos);
}

static inline void getRot(HeroMoveVec* rot, int member)
{
    extern void GSmodelGetRotation(void* model, void* out);

    GSmodelGetRotation(heroMoveGetModel(member), rot);
}

static inline void setPos(s32 member, HeroMoveVec* pos)
{
    extern void peopleSetPositionByIndex(u32 group, u32 id, void* position);
    u32 group;
    u32 id;

    getResID(&group, &id, member);
    peopleSetPositionByIndex(group, id, pos);
}

static inline void setRot(s32 member, HeroMoveVec* rot)
{
    extern void GSmodelSetRotation(void* model, void* rot);

    GSmodelSetRotation(heroMoveGetModel(member), rot);
}

#define HERO_MOVE_ABS(x) ((x) > 0.0f ? (x) : -(x))

static inline f32 heroMoveWrapAngle(f32 angle)
{
    if (angle < -3.141592f) {
        angle += 6.283184f;
    } else if (angle > 3.141592f) {
        angle -= 6.283184f;
    }
    return angle;
}

static inline void heroMoveAnimate(s32 member, f32 amount)
{
    updateAnimation__Ff15HEROMOVE_MEMBER(heroMoveGetModel(member), member, amount);
}

/*
 * MSL's __float_nan (0x80478AC0) is an incomplete array: every retail use in
 * this TU addresses it absolutely (lis/lfs @l), which a sized declaration
 * turns into an sda21 load (see crt/math_ppc.h). heroMoveSqrt below is
 * MSL's inline sqrtf: its 0.0f/0.5/3.0/0.0 are literals, pooled by the TU
 * (0x8047D038-0x8047D058). Reading them through the lbl_ symbols changes
 * the float register colouring around the expansion (heroMoveChkHinderClear
 * and fn_8012B19C put the argument in f2 instead of retail's f4).
 */
extern u8 lbl_80478AC0[];

typedef union HeroMoveFloatShape {
    f32 value;
    u32 bits;
} HeroMoveFloatShape;

static inline f32 heroMoveSqrt(f32 value)
{
    HeroMoveFloatShape shape;
    f64 estimate;
    u32 exponent;
    s32 fpclass;

    if (value > 0.0f) {
        estimate = __frsqrte(value);
        estimate = 0.5 * estimate *
                   (3.0 - value * (estimate * estimate));
        estimate = 0.5 * estimate *
                   (3.0 - value * (estimate * estimate));
        estimate = 0.5 * estimate *
                   (3.0 - value * (estimate * estimate));
        return (f32)(value * estimate);
    }
    if (value < 0.0) {
        return *(f32*)lbl_80478AC0;
    }

    shape.value = value;
    exponent = shape.bits & 0x7F800000;
    switch (exponent) {
    case 0x7F800000:
        if ((shape.bits & 0x007FFFFF) != 0) {
            fpclass = 1;
        } else {
            fpclass = 2;
        }
        break;
    case 0:
        if ((shape.bits & 0x007FFFFF) != 0) {
            fpclass = 5;
        } else {
            fpclass = 3;
        }
        break;
    default:
        fpclass = 4;
        break;
    }
    if (fpclass == 1) {
        return *(f32*)lbl_80478AC0;
    }
    return value;
}

/*
 * Horizontal (XZ) distance between two positions (XD GSvecDistanceXZ__FP5GSvecP5GSvec,
 * 0x8014E70C, a static function of XD's heroMove.cpp; TeamOrre/xd-decomp
 * config/GXXE01/symbols.txt). Colosseum expands it inline with the same
 * instruction sequence in fn_8012B19C and heroMoveChkHinderClear.
 */
static inline f32 GSvecDistanceXZ(HeroMoveVec* a, HeroMoveVec* b)
{
    return heroMoveSqrt((a->x - b->x) * (a->x - b->x) + (a->z - b->z) * (a->z - b->z));
}

/*
 * 0x8012AD50 | 0x434: poison step callback.
 *
 * Lane H1 (2026-09-28): XD's structure (cbPoison__Fl15FootStepCounterl,
 * 0x8014EBF8, trevor403/xd-asm func_FUN_8014ebf8.s): count, fade, then
 * heroMoveInitEvent(), the fainted-member messages, heroMoveTermEvent().
 * XD calls both event functions; Colosseum's retail has their bodies
 * expanded here with the marks of inlining: the InitEvent loop starts with
 * `li r30,0; li r24,0; mr r23,r30` (the member counter materialised and
 * copied into the offset induction variable), which the standalone
 * heroMoveInitEvent does not have, and its resource IDs take the first
 * saved registers (r31/r29/r28), as inlined bodies' locals do. MWCC inlines
 * a function defined later in the file only with `-inline deferred`, and
 * deferred mode emits functions (and pools literals) in reverse definition
 * order (tested on GC/1.0-2.6), so the unit was built with deferred
 * inlining from a source in reverse address order. The candidate chunk is
 * compiled with -inline auto,deferred; every other function in the TU gives
 * identical code with and without it.
 * The read and write cursors are two pointers set to expiredSlots (as in
 * XD: addi r31 then mr r26,r31); the nickname is named before the call
 * (mr r0,r3; li r3,0x32; mr r4,r0 in both games).
 * The inlined heroMoveInitEvent below is a copy whose checkMember argument
 * is cast to HEROMOVE_MEMBER: the cast's temporary moves the member counter
 * ahead of the two inlined IDs in MWCC's colouring order, giving retail's
 * r30 counter (shared with `changed`) and r28/r29 IDs. Expanding the
 * standalone heroMoveInitEvent leaves the counter in r28 (99.5%).
 */
/* RULE-EXCEPTION(user-approved): inline copy of heroMoveInitEvent for cbPoison, with a same-type cast chosen for register allocation - see docs/RULE_EXCEPTIONS.md */
static inline void heroMoveInitEventPoison(void)
{
    extern void updateAnimation__Ff15HEROMOVE_MEMBER(void* model, s32 member,
                                                     f32 frame);
    extern void peopleSetFlagsByIndex(u32 group, u32 handle, u32 flags);
    extern void peopleClearFlagsByIndex(u32 group, u32 handle, u32 flags);
    extern void std151_characterFaceDirection(u32 group, u32 handle, u32 flags);
    HEROMOVE_MEMBER member;
    u32 group;
    u32 id;

    for (member = 0; member < 2; member++) {
        if (heroMoveCheckMember((HEROMOVE_MEMBER)member)) {
            getResID(&group, &id, member);
            updateAnimation__Ff15HEROMOVE_MEMBER(heroMoveGetModel(member), member,
                                                 0.0f);
            peopleSetFlagsByIndex(group, id, 0x80000008);
            peopleClearFlagsByIndex(group, id, 0x100);
            peopleClearFlagsByIndex(group, id, 0x400);
            std151_characterFaceDirection(group, id, 0);
        }
    }
}

/* RULE-EXCEPTION(user-approved): inline copy of heroMoveInitEvent for the callers that inline it (updateChat; an s32 counter here), free to differ in local order/shape from the linked standalone - see docs/RULE_EXCEPTIONS.md */
static inline void heroMoveInitEventChat(void)
{
    extern void updateAnimation__Ff15HEROMOVE_MEMBER(void* model, s32 member,
                                                     f32 frame);
    extern void peopleSetFlagsByIndex(u32 group, u32 handle, u32 flags);
    extern void peopleClearFlagsByIndex(u32 group, u32 handle, u32 flags);
    extern void std151_characterFaceDirection(u32 group, u32 handle, u32 flags);
    s32 member;
    u32 group;
    u32 id;

    for (member = 0; member < 2; member++) {
        if (heroMoveCheckMember(member)) {
            getResID(&group, &id, member);
            updateAnimation__Ff15HEROMOVE_MEMBER(heroMoveGetModel(member), member,
                                                 0.0f);
            peopleSetFlagsByIndex(group, id, 0x80000008);
            peopleClearFlagsByIndex(group, id, 0x100);
            peopleClearFlagsByIndex(group, id, 0x400);
            std151_characterFaceDirection(group, id, 0);
        }
    }
}

/* 0x8012B184 | 0x18 */
/*
 * 0x8012B19C | 0x448: can the member walk from start (its own position when
 * NULL) to target without hitting anything? Colosseum's variant of
 * heroMoveChkHinderClear (not in XD) with a caller position and an extra
 * radius. Instruction-exact under the TU flags (lane H1; 65.3% before):
 * the same helper expansions as heroMoveChkHinderClear, fn_8010F320 takes
 * the scaled radius in f1 (fmr f1,f30; its definition in
 * gs_colsys_exact_8010EFE4.c is (start, end, radius, result)), and the
 * object ID is compared unsigned (addis/cmplwi 0xFFFF). The locals' stack
 * slots (pos 0x2C, goal 0x38, ofs 0x44, dir 0x50) give their declaration
 * order. Only the .sdata2 relocations still name lbl_8047D0xx where the
 * literals of heroMoveSqrt are pooled as @N.
 */
/*
 * 0x8012B5E4 | 0x4EC: is the path from the member to the leader clear?
 * Instruction-exact under the TU flags (lane H1; 57.9% before, scored at
 * -O4,s). XD heroMoveChkHinderClear (0x8014F070, trevor403/xd-asm
 * func_FUN_8014f070.s) gives the structure call for call: heroMoveIsMember,
 * the leader, getPos of both plus 8.5, getObjID/peopleInfo radius of the
 * member, fn_8010F320 (XD 0x8011A5C4), the leader's radius, getResID and
 * the person's collision word, getPos again, PSVECSubtract,
 * GSvecDistanceXZ, the two early returns, PSVECScale/PSVECAdd and the
 * collision test (`!= 6`: subfic/subi/or/srwi 31). The object ID is
 * compared unsigned (addis r0,r3,1; cmplwi r0,0xFFFF in both games).
 */
/* 0x8012BAD0 | 0x20 */
/* 0x8012BAF0 | 0xB8 */
/*
 * 0x8012BBA8 | 0xFC: end the event pose of every party member.
 * Instruction-exact under the TU flags (lane H1; 87.6% before, scored at
 * -O2). XD heroMoveTermEvent (0x8014F518, trevor403/xd-asm
 * func_FUN_8014f518.s): heroMoveIsMember, getResID, then the three model
 * flag calls; XD's AnalogStickEmu reset after the loop is not in
 * Colosseum. The HEROMOVE_MEMBER counter gives retail's offset induction
 * (see HEROMOVE_MEMBER); declaring it first gives retail's registers.
 */
/*
 * 0x8012BCA4 | 0x13C: put every party member into the event pose.
 * Instruction-exact under the TU flags (lane H1; 46.1% before, scored at
 * -O2). XD heroMoveInitEvent (0x8014F5C0, trevor403/xd-asm
 * func_FUN_8014f5c0.s): heroMoveIsMember, getResID, getModel,
 * updateAnimation(0.0), then four model flag calls. Colosseum inlines
 * getResID twice (the direct call and the one inside getModel), which is
 * why the pooled {100, 101} table is copied twice per member.
 */
/* 0x8012BDE0 | 0xD4 */
/* 0x8012BEB4 | 0x200 */
/* 0x8012C0B4 | 0x48C */
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
/* 0x8012C540 | 0x120 */

/*
 * 0x8012C540 | 0x120: GScolsys2CheckGetEventID for the point in front of the
 * leader. Exact (lane B7c; 96.5% before): the leader's
 * resource ID comes from getResID, and like retail neither the ID (out of
 * range leader) nor the result (no person) is initialised: retail has no
 * initialising instruction on those paths and returns r30 as left.
 */
/* 0x8012C660 | 0x424 */
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
/* 0x8012D39C | 0x454 */
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
/* 0x8012D7F0 | 0x6A4 */
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
/* 0x8012DE94 | 0x4F4 */
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
/* 0x8012E388 | 0x430 */
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
/* 0x8012E7B8 | 0x41C */
extern u8 dbgMenuIsOpen(void);
extern u8 menuIsCheck(u32);
extern u8 fn_8018C424(u32, u32, u32);
extern void fn_8000D710(u32);
extern u8 lbl_80272A38[];
/*
 * Sample the member's foot heights for this frame (XD procStep__F15HEROMOVE_MEMBER,
 * 0x8014F9F8: part table copy, the frame-time call, getModel, then getStep
 * when the model exists). The part indices are the rodata table at
 * lbl_80272A38.
 */
typedef struct HeroMovePartTable {
    s32 part[2][4];
} HeroMovePartTable;

static inline void procStep(s32 member)
{
    HeroMovePartTable parts = *(HeroMovePartTable*)lbl_80272A38;
    f32 footstep[0x10];
    void* model;

    GSgetFrameStep();
    model = heroMoveGetModel(member);
    if (model != NULL) {
        getStep__FP8FOOTSTEPP8_GSmodelPiP8FOOTWORK(footstep, model, parts.part[member],
                                                  lbl_80426BD0.member[member].footwork.height);
    }
}

/*
 * 0x8012EBD4 | 0x3E4: per-frame party update.
 *
 * Exact (lane B7c, 2026-09-28; 95.6% before, written with gotos and raw
 * offsets). Follows XD's heroMoveMain (0x80150D80): early-outs, the queued
 * auto event, the three event-list loops, then moveLeader and procStep for
 * the leader. Colosseum's step accumulator calls every registered step
 * callback each 7.0 units (XD moved that into proc1Step with counters).
 * The leader's resource ID is read uninitialised when the index is out of
 * range, exactly as retail (no initialising instruction; lwzx only on the
 * in-range path).
 */
/* 0x8012F008 | 0x114 */
/* 0x8012D39C | 0x454 */
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
/* 0x8012D7F0 | 0x6A4 */
typedef struct HeroMoveVec3 {
    f32 x;
    f32 y;
    f32 z;
} HeroMoveVec3;


/* Line a*x + b*z + c = 0 through two points in the XZ plane, (a, b) of unit
 * length; FALSE when the points coincide. */
/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_8012D39C) — see docs/RULE_EXCEPTIONS.md */
static inline BOOL heroMoveGetLine(HeroMoveVec* start, HeroMoveVec* end, f32* a, f32* b, f32* c)
{
    f32 dz = end->z - start->z;
    f32 dx = end->x - start->x;
    f32 lengthSquared = dx * dx + dz * dz;
    f32 invLength;

    if (lengthSquared < 1e-7f) {
        return FALSE;
    }
    invLength = 1.0f / heroMoveSqrt(lengthSquared);
    *a = -dz * invLength;
    *b = dx * invLength;
    *c = invLength * (start->x * end->z - end->x * start->z);
    return TRUE;
}

/* Retail computes the sums of squares below as separate fmuls + fadds,
 * never fused into fmadds. */
/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_8012D39C) — see docs/RULE_EXCEPTIONS.md */
static inline f32 heroMoveSq(f32 x)
{
    return x * x;
}

/* XD getCpLineCircle: crossing of the line start-end with the circle of
 * radius around center, nearest to reference when there are two. */

/* XD getLeaderLog__FP5GSveci (GXXE01 0x801515B4): the same count and
 * capacity guards, ring-index wrap, position copy, and Boolean result occur
 * here and in moveLeader. Only this function uses the helper in this pass.
 * The existing u32 fields are read as signed ring counters; valid values are
 * 0..20, and the subtraction must test a negative index before wrapping. */
static inline u8 getLeaderLog(HeroMoveVec* pos, s32 i)
{
    s32 index;

    if (i >= (s32)lbl_80426BD0.historyCount) {
        return FALSE;
    }
    if (i >= 20) {
        return FALSE;
    }
    index = (s32)lbl_80426BD0.historyHead - i - 1;
    if (index < 0) {
        index += 20;
    }
    *pos = lbl_80426BD0.history[index];
    return TRUE;
}

/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_8012D7F0) — see docs/RULE_EXCEPTIONS.md */
static inline f32 heroMoveVecSquareMag(HeroMoveVec* v)
{
    return v->x * v->x + v->y * v->y + v->z * v->z;
}

/* 0x8012DE94 | 0x4F4 */
/*
 * Collision test of a member's model, optionally with the model moved to
 * pos for the test and put back afterwards.
 */
static inline u8 heroMoveCheckHit(s32 member, HeroMoveVec* pos)
{
    extern u8 fn_800E3C64(void* model);
    HeroMoveVec old;
    void* model;
    u8 hit;

    model = heroMoveGetModel(member);
    if (model == NULL) {
        return FALSE;
    }
    if (pos != NULL) {
        getPos(&old, member);
        setPos(member, pos);
    }
    hit = fn_800E3C64(model);
    if (pos != NULL) {
        setPos(member, &old);
    }
    return hit;
}

/* 0x8012E388 | 0x430 */
/* XD updateLeaderMovement source shape; reviewed from H3 commit 86a8dafe. */
/* 0x8012E7B8 | 0x41C */

#if 1
/* 0x8012EFB8 | 0x50 */
/* Resource group/ID of a party member's field model (group 0, IDs 100/101). */

/* 0x8012F008 | 0x114 */

/* 0x8012F11C | 0x34 */

/*
 * The leader walks at distance 0 and each other member 12 units further
 * back. Expanded identically in heroMoveDismissMember, fn_8012F1FC,
 * fn_8012F40C, heroMoveSyncWithHero and fn_8013024C. The step is the TU's
 * pooled 12.0f literal (0x8047D0D4).
 */
static inline void heroMoveUpdateSpacing(void)
{
    f32 spacing;
    s32 i;

    lbl_80426BD0.member[lbl_80426BD0.leader].spacing = 0.0f;
    spacing = 12.0f;
    for (i = 0; i < 2; i++) {
        if ((lbl_80426BD0.member[i].flags & 1) && lbl_80426BD0.leader != i) {
            lbl_80426BD0.member[i].spacing = spacing;
            spacing += 12.0f;
        }
    }
}

/* 0x8012F150 | 0xAC */

static inline void heroMoveSetModelVisible(s32 member, u8 visible)
{
    u32 group;
    u32 id;

    getResID(&group, &id, member);
    peopleSetVisible(group, id, visible);
}

/* 0x8012F1FC | 0x210: add a member to the party (neck tracking on). */


/* 0x8012F40C | 0x204: make a party member the leader. */

/* Local copies of the .rodata floor list and area/model pairs. */
typedef struct HeroMoveThemeTable {
    u32 words[10];
} HeroMoveThemeTable;

typedef struct HeroMoveFloorTable {
    u32 words[20];
} HeroMoveFloorTable;

/* 0x8012FAD8 | 0x1FC: field model of the partner for the next floor. */

/* One floor height hit from fn_8010E138. */
typedef struct HeroMoveFloorHit {
    f32 height;
    f32 unk04;
    f32 unk08;
} HeroMoveFloorHit;

/* Clear a member's foot heights (XD initFootWork__FP8FOOTWORK, 0x8014FCB0). */
static inline void initFootWork(FOOTWORK* footwork)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        footwork->height[i] = 0.0f;
    }
}

/*
 * 0x8012F610 | 0x4C8: place the followers behind the leader on the floor.
 *
 * 99.9% (lane B7c, 2026-09-28; 95.0% before). Structure follows XD's
 * initFloor__Fv (0x801517F8): getPos/getRot of the leader, sin and cos of
 * its heading computed before either is negated (XD and Colosseum both keep
 * sin in f31 across the cos call and negate afterwards), the follower loop
 * with setPos/setRot, then setDisp, initFootWork and the neck-mode loops.
 * Both games also store the leader's height into position.y ahead of the
 * loop (lfs 0x48/stfs 0x3C here, lfs 0x18/stfs 0xC in XD) as well as inside
 * it; the statement before the loop reproduces that store.
 * The helper nesting fixes the stack slots: MWCC creates each inlined
 * getResID table when it expands getResID, pass by pass, so the tables of
 * setPos/setDisp/neck mode (getResID one level down) sit above those of
 * getPos/getRot/setRot (two levels down, through getModel), as in retail.
 * initFootWork's loop keeps the footwork stores on the setDisp loop's base
 * register (a straight-line body re-materializes the address).
 * getPos and getRot take the member as `int` (like getModel): with s32,
 * getRot's resource ID is coloured after setRot's (r25/r28 instead of
 * retail's r28/r27).
 */

/* Initialize the two field hero models and select the area's model theme. */


/* 0x80130054 | 0x1F8: the partner follows the hero unless flag 0x8AE is set. */

/* 0x8013024C | 0x414: reset the party to the hero alone and register the
 * poison and friendship step callbacks.
 *
 * 95.8% (93.9% before round 3). The block-copied heroMoveGetResID table
 * restores retail's stack stores (frame 0x30), and its s32 entries keep the
 * first inlined ID in r30 across the fn_80188AF4 call (see the table). One
 * difference remains, shared with fn_8012F1FC and heroMoveSyncWithHero (the
 * same add-member expansion): retail keeps heroMoveSetNeckMode's mode (1)
 * in r29 and still emits its range check and switch compare (cmpwi r29,0 /
 * cmpwi r29,2 / cmpwi r29,1) while storing the mode with li r0,1. MWCC
 * folds all of these for a literal argument (round 3 below explains the
 * mechanism and the exact form); the literal 0
 * in fn_8012F40C is folded in retail too. Tested without success on a
 * reduced party TU: GC/1.0-3.0a5; C and C++; -inline auto/all/deferred
 * with the party functions in either order; -opt no{deadstore,cse,
 * lifetimes,loop,strength,prop}; the mode as TRUE/true/enum/cast/const
 * object/local/static const/inline return value/struct or array member.
 * A local temp or inline return value leaves the range compares but folds
 * the switch; only a non-const file-scope variable leaves both, as a load.
 * It is linked as part of the whole TU (with its pooled 12.0f and
 * heroMoveAddStepCallback, inlined twice here).
 *
 * Round 2 (lane B7, 2026-09-28). The fingerprint is a constant the
 * optimizer learned only after branch and switch folding: a `li` into a
 * register that the compares then test. It occurs nowhere else in the
 * retail DOL (scan of every li rN / cmpwi rN pair with no label between:
 * only fn_8012F1FC, heroMoveSyncWithHero and this function). The same
 * shape appears for later-known values elsewhere in this TU: the unrolled
 * spacing loop's index (li r5,1; cmpw r0,r5), and the flags value
 * forwarded from initHeroMove's stores above (li r4,0; clrlwi. r0,r4,31).
 * On GC/1.3 -O4,p the literal is substituted into the inline and
 * everything folds. A value known one step later (inline-return temp,
 * out-parameter, modified parameter) leaves only the range compares and
 * still folds the switch. A load (non-const global, const reference,
 * struct or array initializer, const defined after use) leaves every
 * compare but also the stw of the register. fn_8012F40C's literal 0 is
 * substituted in retail: a temp 0 leaves its range compares too. Also
 * tested without success: C++ with HEROMOVE_MEMBER/neck-mode enums, bool,
 * casts to enum, class/namespace constants, default argument, template
 * constant; the IsMember check as the real heroMoveIsMember (bool/u8,
 * branchy body, which also matches standalone); nested or out-of-line
 * range/switch helpers; reverse definition order with -inline deferred;
 * -inline smart/level=N/all; -opt no{propagation,cse,loop}; GC/1.0-3.0a5.
 * XD reference: the XD JP demo linker map (StarsMmd/Colo-XD-PBR-symbol-maps
 * @6b51d3af, NXXJ01.map, heroMove.o) and TeamOrre/xd-decomp symbols.txt
 * (GXXE01) name this function heroMoveAllInit (0x80151F8C: initHeroMove,
 * heroMoveJoinMember(0), heroMoveAppointLeader(0), then the step
 * callbacks), fn_8012F1FC heroMoveJoinMember (0x80151420, passes li r4,1
 * to heroMoveSetNeckMode) and fn_8012F40C heroMoveAppointLeader
 * (0x80151548). The map also lists the TU's local helpers getResID,
 * remakeFormation, setDisp, getModel, initLeaderLog, initHeroMove and the
 * exported heroMoveGetNeckMode (stripped from Colosseum). Because XD
 * doesn't inline, its li r4,1 shows only that the value is constant, not
 * that the source used a literal.
 *
 * Round 3 (lane B7b, 2026-09-28): traced MWCC's passes. GC/2.6 replays this
 * TU's fn_8012F1FC and fn_8013024C byte-for-byte, so its dumps are exact.
 * GC/2.6 is a release build whose frontend-optimizer (IRO) log is compiled
 * in but disabled: IRO_Optimizer clears IRO_Log (byte 0x5eb3b9) at 0x42ddb9
 * and opens <source>.log at 0x42ddc0 when it is set; the per-phase
 * flowgraph dump IRO_DumpAfterPhase (0x454d90) runs only when its second
 * argument is set (all callers pass 0; NOPing the je at 0x454d95 forces it).
 * Setting the byte through retrowin32's gdb stub gives a full IRO trace.
 * Findings:
 * - The ID reload was frontend copy propagation of `*id = ids.id[0]`; fixed
 *   by the s32 table above (93.9 -> 95.8%).
 * - IRO_CopyAndConstantPropagation substitutes a constant only into a use
 *   read with the same type as the definition. A compare `mode < 0` reads a
 *   long (s32) variable as int, so a temp or local s32 mode keeps its range
 *   compares; the switch reads it as long and the store as long, so both
 *   fold. That is B7's "late-known value" row.
 * - Retail's mix (range and switch compares kept, store folded) is exactly
 *   what a mode of ENUM type gives: the compares and the switch read the
 *   enum as int (not propagated), the store into an enum-typed neckMode
 *   reads it as the enum (propagated, so li r0,1). Verified on a reduced TU
 *   and in this TU: neckMode and heroMoveSetNeckMode's mode typed as a
 *   3-value enum (off/on/none = 0/1/2; the TU's other enum, HEROMOVE_MEMBER,
 *   is in its mangled names), GetNeckMode returning it, and fn_8012F1FC
 *   calling `mode = <on>; heroMoveSetNeckMode(member, mode);` with an
 *   enum-typed local assigned right after `flags |= 1` makes fn_8012F1FC,
 *   heroMoveSyncWithHero and fn_8013024C exact (only the pooled @1126/@1197
 *   names differ); initializing the local at its declaration instead
 *   schedules li r29,1 one slot later. Nothing else in the TU changes.
 * - A literal argument is substituted at inline time for every type and
 *   language (C and C++, enum constant, cast, TRUE, default argument): all
 *   folds. An enum-typed inline return value also leaves the compares but
 *   is scheduled differently (fn_8012F1FC 99.1%).
 * The exact form is now applied as a tagged title-path rule exception: the
 * local names a constant and is not a strict campaign win. The enum typing
 * alone changes nothing while the argument is a literal. */

/* Update the active field-chat target and its two hero models. */
typedef struct HeroChatTarget {
    /* 0x00 */ u8 unk00[0x28];
    /* 0x28 */ u32 group;
    /* 0x2C */ u32 id;
} HeroChatTarget;

#endif


/* ===== Functions, in reverse retail address order: with "-inline auto,deferred"
 * MWCC emits them last-to-first, i.e. in address order (0x8012AC9C - 0x80130660). ===== */

void fn_8013024C(void)
{
    s32 i;

    lbl_80426BD0.member[0].flags = 0;
    lbl_80426BD0.member[1].flags = 0;
    lbl_80426BD0.leader = 0;
    lbl_80426BD0.lockFrame = 0;
    fn_8012F1FC(0);
    fn_8012F40C(0);
    lbl_80426BD0.autoEvent[0] = 0;
    lbl_80426BD0.stepAccum = 0.0f;
    for (i = 0; i < 8; i++) {
        lbl_80426BD0.stepCallback[i].func = NULL;
    }
    lbl_80426BD0.poisonSteps = 0;
    heroMoveAddStepCallback(cbPoison__Fl15FootStepCounterl, 0);
    lbl_80426BD0.friendSteps = 0;
    heroMoveAddStepCallback(cbTsureFriend__Fl15FootStepCounterl, 0);
}

void heroMoveSyncWithHero(void)
{
    u8 follow = FALSE;
    u8 flagClear = std082_flagGetValue(0x8AE) == 0;

    if (flagClear && (s32)heroGetStatus(0, 0x18, 0) != 0) {
        follow = TRUE;
    }
    if (follow) {
        fn_8012F1FC(1);
    } else {
        heroMoveDismissMember(1);
    }
}

s32 heroMoveInit(void* position, void* rotation)
{
    extern void peopleIsInGroup(u32 group, u32 object);
    extern void fn_8018C8F4(u32 group, u32 object, u32 flags);
    extern void peopleSetPositionByIndex(u32 group, u32 object, void* position);
    extern void* GSresGetResource(u32 group, u32 handle);
    extern void updateAnimation__Ff15HEROMOVE_MEMBER(void* model, s32 member, f32 amount);

    HeroMoveFloorTable floors;
    HeroMoveThemeTable themes;
    void* models[2];
    s32 i;
    u32* floorCursor;
    u32 floor;
    u32 theme;
    s32 area;
    u8 unavailable;

    if (fn_800FF548() == 0) {
        floors = *(HeroMoveFloorTable*)lbl_802729C0;
        themes = *(HeroMoveThemeTable*)lbl_80272A10;

        unavailable = std082_flagGetValue(0x8AE) == 0;
        if (unavailable != 0) {
            theme = 0x00F70400;
        } else {
            floor = floorGetNextFloorID();
            floorCursor = floors.words;
            i = 0;
            while (i < 20) {
                if (floor == *floorCursor) {
                    break;
                }
                floorCursor++;
                i++;
            }

            if (i >= 20) {
                theme = 0x00F70400;
            } else {
                area = fn_8006AE18();
                for (i = 0; i < 5; i++) {
                    if (area == (s32)themes.words[i * 2]) {
                        break;
                    }
                }
                theme = themes.words[i * 2 + 1];
            }
        }

        peopleOpen(0, 100, theme);
        peopleOpen(0, 101, 0x00F30400);
    } else {
        peopleIsInGroup(0, 100);
        peopleIsInGroup(0, 101);
    }

    for (i = 0; i < 2; i++) {
        models[i] = heroMoveGetModel(i);
        GSmodelEnableAnimBlend(models[i]);
    }

    fn_8018CB5C(0, 100);
    fn_8018CB5C(0, 101);

    if (fn_800FF548() == 0) {
        fn_8018C8F4(0, 100, 0x40000F00);
        fn_8018C8F4(0, 101, 0x701);
    }

    fn_80189328(0, 101, 1);

    if (fn_800FF548() == 0) {
        peopleSetPositionByIndex(0, 100, position);
        peopleSetRotation(0, 100, rotation);
    }

    for (i = 0; i < 2; i++) {
        updateAnimation__Ff15HEROMOVE_MEMBER(models[i], i, 0.0f);
    }

    initFloor__Fv();
    lbl_80426BD0.historyHead = 0;
    lbl_80426BD0.historyCount = 0;
    lbl_80426BD0.eventValue[1] = 0;
    lbl_80426BD0.eventValue[2] = 0;
    lbl_80426BD0.eventValue[0] = 0;
    lbl_80426BD0.lockFrame = 0;
    return 0;
}

u32 heroMoveGetKenObjID(void)
{
    HeroMoveFloorTable floors = *(HeroMoveFloorTable*)lbl_802729C0;
    HeroMoveThemeTable themes = *(HeroMoveThemeTable*)lbl_80272A10;
    u8 flagClear = std082_flagGetValue(0x8AE) == 0;
    u32 floor;
    s32 area;
    s32 i;

    if (flagClear) {
        return 0x00F70400;
    }
    floor = floorGetNextFloorID();
    for (i = 0; i < 20; i++) {
        if (floor == floors.words[i]) {
            break;
        }
    }
    if (i >= 20) {
        return 0x00F70400;
    }
    area = fn_8006AE18();
    for (i = 0; i < 5; i++) {
        if (area == (s32)themes.words[i * 2]) {
            break;
        }
    }
    return themes.words[i * 2 + 1];
}

void initFloor__Fv(void)
{
    extern f64 sin(f64 x);
    extern f64 cos(f64 x);
    extern s32 fn_8010E138(void* position, HeroMoveFloorHit* hits);

    HeroMoveVec rotation;
    HeroMoveVec leaderPos;
    HeroMoveVec position;
    HeroMoveFloorHit hits[8];
    f32 sinY;
    f32 cosY;
    f32 dirX;
    f32 dirZ;
    f32 distance;
    f32 bestAny;
    f32 bestStep;
    BOOL foundStep;
    s32 count;
    s32 j;
    s32 i;

    if (!fn_800FF548()) {
        getPos(&leaderPos, lbl_80426BD0.leader);
        getRot(&rotation, lbl_80426BD0.leader);
        sinY = sin(rotation.y);
        cosY = cos(rotation.y);
        dirX = -sinY;
        dirZ = -cosY;
        position.y = leaderPos.y;
        distance = 9.0f;
        for (i = 0; i < 2; i++) {
            if (heroMoveCheckMember(i) && i != lbl_80426BD0.leader) {
                position.x = leaderPos.x + dirX * distance;
                position.y = leaderPos.y;
                position.z = leaderPos.z + dirZ * distance;
                count = fn_8010E138(&position, hits);
                if (count > 0) {
                    if (count >= 2) {
                        bestStep = -1000000.0f;
                        bestAny = bestStep;
                        foundStep = FALSE;
                        for (j = 0; j < count; j++) {
                            if (bestAny < hits[j].height) {
                                bestAny = hits[j].height;
                            }
                            if (hits[j].height - position.y >= 10.0f) {
                                continue;
                            }
                            if (bestStep < hits[j].height) {
                                bestStep = hits[j].height;
                                foundStep = TRUE;
                            }
                        }
                        if (foundStep) {
                            position.y = bestStep;
                        } else {
                            position.y = bestAny;
                        }
                    } else {
                        position.y = hits[0].height;
                    }
                }
                distance += 9.0f;
                setPos(i, &position);
                setRot(i, &rotation);
            }
        }
    }

    for (i = 0; i < 2; i++) {
        heroMoveSetModelVisible(i, heroMoveCheckMember(i));
    }
    for (i = 0; i < 2; i++) {
        initFootWork(&lbl_80426BD0.member[i].footwork);
    }
    for (i = 0; i < 2; i++) {
        if (heroMoveCheckMember(i)) {
            heroMoveSetNeckMode(i, heroMoveGetNeckMode(i));
        }
    }
    lbl_80426BD0.member[0].timer = 300;
    lbl_80426BD0.member[1].timer = 300;
    lbl_80426BD0.stepAccum = 0.0f;
}

s32 fn_8012F40C(s32 member)
{
    if (!heroMoveCheckMember(member)) {
        return FALSE;
    }
    lbl_80426BD0.leader = member;
    if (heroMoveGetNeckMode(member) == 1) {
        heroMoveSetNeckMode(member, 0);
    }
    heroMoveUpdateSpacing();
    lbl_80426BD0.historyHead = 0;
    lbl_80426BD0.historyCount = 0;
    return TRUE;
}

s32 fn_8012F1FC(s32 member)
{
    u32 group;
    u32 id;
    HeroMoveNeckMode mode;

    if (member < 0 || member >= 2) {
        return FALSE;
    }
    if (heroMoveCheckMember(member)) {
        return TRUE;
    }
    lbl_80426BD0.member[member].flags |= 1;
    /* RULE-EXCEPTION(title-path): constant-only local for MWCC propagation - see docs/RULE_EXCEPTIONS.md */
    mode = HERO_MOVE_NECK_ON;
    heroMoveSetNeckMode(member, mode);
    heroMoveUpdateSpacing();
    heroMoveSetModelVisible(member, TRUE);
    return TRUE;
}

s32 heroMoveDismissMember(s32 member)
{
    if (member < 0 || member >= 2) {
        return FALSE;
    }
    if (member == lbl_80426BD0.leader) {
        return FALSE;
    }
    lbl_80426BD0.member[member].flags &= ~1;
    heroMoveUpdateSpacing();
    return TRUE;
}

u32 heroMoveIsMember(s32 member)
{
    if (member < 0 || member >= 2) {
        return FALSE;
    }
    return lbl_80426BD0.member[member].flags & 1;
}

u32 heroMoveSetNeckMode(s32 member, HeroMoveNeckMode mode)
{
    u32 group;
    u32 id;

    if (mode < 0 || mode >= 2) {
        return FALSE;
    }
    if (!heroMoveCheckMember(member)) {
        return FALSE;
    }
    getResID(&group, &id, member);
    switch (lbl_80426BD0.member[member].neckMode) {
    case 1:
        std067_characterStopLookAt(group, id);
        break;
    }
    switch (mode) {
    case 1:
        std065_characterLookAtPlayer(group, id);
        break;
    }
    lbl_80426BD0.member[member].neckMode = mode;
    return TRUE;
}

u8 heroMoveGetResID(u32* group, u32* id, s32 member)
{
    return getResID(group, id, member);
}

static void proc1Step(f32 distance);

u32 heroMoveMain(void)
{
    extern u32 fn_800F7AF0(s32);
    extern u32 fn_800F7BC4(s32);
    extern u32 std082_flagGetValue(u32);
    extern void cmsRunSync(void*, u32, ...);
    u32 group;
    u32 id;
    u32 event;
    f32 distance;
    s32 i;
    u8 flagClear;

    if (dbgMenuIsOpen()) {
        return 0;
    }
    if (menuIsCheck(0xCA)) {
        return 0;
    }
    if (GSscene_GetMode() == 6) {
        return 0;
    }
    if (lbl_80426BD0.lockFrame > 0) {
        lbl_80426BD0.lockFrame--;
        return 0;
    }
    getResID(&group, &id, lbl_80426BD0.leader);
    if (fn_8018C424(group, id, 0x80000000)) {
        return 0;
    }

    event = lbl_80426BD0.autoEvent[0];
    if (event != 0) {
        lbl_80426BD0.autoEvent[0] = 0;
        cmsRunSync((void*)event, 4, lbl_80426BD0.autoEvent[1],
                    lbl_80426BD0.autoEvent[2], lbl_80426BD0.autoEvent[3],
                    lbl_80426BD0.autoEvent[4]);
        return 0;
    }

    for (i = 0; i < lbl_80426BD0.eventValue[0]; i++) {
        heroDispatchEvent(3, lbl_80426BD0.eventList[0][i].id);
    }
    for (i = 0; i < lbl_80426BD0.eventValue[2]; i++) {
        heroDispatchEvent(2, lbl_80426BD0.eventList[2][i].id);
    }
    for (i = 0; i < lbl_80426BD0.eventValue[1]; i++) {
        heroDispatchEvent(1, lbl_80426BD0.eventList[1][i].id);
    }
    lbl_80426BD0.eventValue[1] = 0;
    lbl_80426BD0.eventValue[2] = 0;
    lbl_80426BD0.eventValue[0] = 0;
    if (lbl_80426BD0.autoEvent[0] != 0) {
        return 0;
    }

    if (fn_800F7BC4(1) & fn_800F7AF0(1) & 0x1C00) {
        fn_8000D710(0);
        return 0;
    }

    for (i = 0; i < 2; i++) {
        if (heroMoveCheckMember(i) && i != lbl_80426BD0.leader) {
            partnerMove(i);
        }
    }
    distance = moveLeader__F15HEROMOVE_MEMBER(lbl_80426BD0.leader);
    procStep(lbl_80426BD0.leader);

    flagClear = std082_flagGetValue(0x8AE) == 0;
    if (flagClear) {
        proc1Step(distance);
    }
    return 0;
}

f32 moveLeader__F15HEROMOVE_MEMBER(s32 member)
{
    extern u32 fn_800F7AF0(s32);
    extern u32 fn_800F7BC4(s32);
    extern void PSVECSubtract(HeroMoveVec*, HeroMoveVec*, HeroMoveVec*);
    extern void PSVECScale(HeroMoveVec*, HeroMoveVec*, f32);
    extern f32 PSVECDistance(HeroMoveVec*, HeroMoveVec*);
    extern f32 peopleAmountFromMove(u32, u32, HeroMoveVec*);
    extern u32 GSgetFrameStep(void);
    HeroMoveVec before;
    HeroMoveVec after;
    HeroMoveVec direction;
    HeroMoveVec current;
    HeroMoveVec last;
    f32 speed;
    u8 record;
    u32 group;
    u32 id;
    f32 turn;

    if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x100) != 0 &&
        updateChat__F15HEROMOVE_MEMBER(member) != 0) {
        return 0.0f;
    }

    getPos(&before, member);
    heroReadStick(member, &speed);
    getPos(&after, member);
    PSVECSubtract(&after, &before, &direction);
    PSVECScale(&direction, &direction, 1.0f / (f32)GSgetFrameStep());

    getResID(&group, &id, member);
    turn = peopleAmountFromMove(group, id, &direction);
    if (turn < 0.2f) {
        if (speed >= 0.2f) {
            turn = 0.2f;
        } else {
            turn = speed;
        }
    }
    heroMoveAnimate(member, turn);

    record = FALSE;
    getPos(&current, lbl_80426BD0.leader);
    if (getLeaderLog(&last, 0)) {
        if (GSvecDistanceXZ(&last, &current) > 12.0f) {
            record = TRUE;
        }
    } else {
        record = TRUE;
    }

    if (record) {
        lbl_80426BD0.history[lbl_80426BD0.historyHead] = current;
        if ((s32)++lbl_80426BD0.historyHead >= 20) {
            lbl_80426BD0.historyHead = 0;
        }
        if ((s32)lbl_80426BD0.historyCount < 20) {
            lbl_80426BD0.historyCount++;
        }
    }

    return PSVECDistance(&before, &after);
}

void heroReadStick(s32 member, f32* speed)
{
    extern void* GSresGetResource(u32, u32);
    extern s8 fn_800F7A7C(u32, u32);
    extern s8 fn_800F7A08(u32, u32);
    extern u32 fn_800F7BC4(u32);
    extern u32 GSscene_GetMode(void);
    extern f64 sin(f64);
    extern f32 cameraGetRotY(void);
    extern void peopleStartTurn(u32, u32, f32, f32);
    extern void peopleSetSpeedFromAmount(u32, u32, f32);
    extern void peopleStopTurn(u32, u32);
    extern void fn_80166458(void*, void*);

    s8 x;
    s8 y;
    s8 subX;
    s8 subY;
    u32 group;
    u32 id;
    f32 fx;
    f32 fy;
    f32 anglePart;
    f32 angle;
    u8 work[12];

    GSresGetResource(0, 2);
    getResID(&group, &id, member);
    heroMoveGetModel(member);

    x = fn_800F7A7C(1, 1);
    y = fn_800F7A08(1, 1);
    subX = fn_800F7A7C(1, 0);
    subY = fn_800F7A08(1, 0);
    if (x == 0 && y == 0) {
        if (fn_800F7BC4(1) & 8) {
            y = -56;
        }
        if (fn_800F7BC4(1) & 4) {
            y = 56;
        }
        if (fn_800F7BC4(1) & 1) {
            x = -56;
        }
        if (fn_800F7BC4(1) & 2) {
            x = 56;
        }
        subX = x;
        subY = y;
        if (x == 0 && y == 0) {
            GSscene_GetMode();
        }
    }

    if (x != 0 || y != 0) {
        if (x > 56) {
            x = 56;
        } else if (x < -56) {
            x = -56;
        }
        if (y > 56) {
            y = 56;
        } else if (y < -56) {
            y = -56;
        }
        fx = (x > 0 ? x : -x) / 28.0f;
        fy = (y > 0 ? y : -y) / 28.0f;
        *speed = heroMoveSqrt(fx * fx + fy * fy);
        if (*speed > 2.0f) {
            *speed = 2.0f;
        }

        if (!(subX > -2 && subX < 2 && subY > -2 && subY < 2)) {
            if (fy < 0.001f) {
                anglePart = 1.5707964f;
            } else {
                f32 ratio = fx / fy;
                if (ratio > 5.0f) {
                    ratio = 5.0f;
                }
                anglePart = 1.5707964f * (f32)sin(ratio / 3.1830988f);
            }
            if (y >= 0) {
                angle = anglePart;
            } else {
                angle = 3.1415927f - anglePart;
            }
            if (x < 0) {
                if (y >= 0) {
                    angle = 3.1415927f + (3.1415927f - anglePart);
                } else {
                    angle = 3.1415927f + anglePart;
                }
            }
            peopleStartTurn(group, id, cameraGetRotY() + angle, *speed);
        }
        peopleSetSpeedFromAmount(group, id, *speed);
        fn_80166458(GSresGetResource(0, 2000), work);
    } else {
        *speed = 0.0f;
        peopleStopTurn(group, id);
    }
}

/* RULE-EXCEPTION(user-approved): single-use inline copy of heroMoveSqrt whose negative branch returns an intrinsic (__fabs) instead of the NaN load; the result is discarded, and only then does the `value < 0.0` compare survive as in retail - see docs/RULE_EXCEPTIONS.md */
static inline f32 heroMoveSqrtDE94(f32 value)
{
    f64 estimate;

    if (value > 0.0f) {
        estimate = __frsqrte(value);
        estimate = 0.5 * estimate *
                   (3.0 - value * (estimate * estimate));
        estimate = 0.5 * estimate *
                   (3.0 - value * (estimate * estimate));
        estimate = 0.5 * estimate *
                   (3.0 - value * (estimate * estimate));
        return (f32)(value * estimate);
    }
    if (value < 0.0) {
        return (f32)__fabs(value);
    }
    return value;
}

void partnerMove(s32 member)
{
    extern u32 GSgetFrameStep(void);
    extern void PSVECSubtract(HeroMoveVec*, HeroMoveVec*, HeroMoveVec*);

    HeroMoveVec leaderPos;
    HeroMoveVec pos;
    HeroMoveVec current;
    HeroMoveVec velocity;
    HeroMoveVec move;
    HeroMoveVec log;
    HeroMoveVec result;
    f32 distance;
    s32 i;
    u8 found;

    getPos(&leaderPos, lbl_80426BD0.leader);
    getPos(&pos, member);
    {
        f32 dx = leaderPos.x - pos.x;
        f32 dz = leaderPos.z - pos.z;

        /* RULE-EXCEPTION(user-approved): discarded computation kept from retail — see docs/RULE_EXCEPTIONS.md */
        heroMoveSqrtDE94(dx * dx + dz * dz);
    }
    partnerGetTarget(member, &velocity, &result);
    partnerApplyMove(member, &velocity, &result);
    getPos(&current, member);
    distance = GSvecDistanceXZ(&leaderPos, &current);
    PSVECSubtract(&current, &pos, &move);
    if (distance < 100.0f) {
        lbl_80426BD0.member[member].timer = 300;
        return;
    }

    if ((lbl_80426BD0.member[member].timer -= GSgetFrameStep()) > 0) {
        return;
    }
    if (heroMoveCheckHit(member, NULL)) {
        return;
    }

    found = FALSE;
    for (i = 0; getLeaderLog(&log, i) && !found; i++) {
        if (!heroMoveCheckHit(member, &log)) {
            found = TRUE;
        }
    }
    if (found) {
        setPos(member, &log);
        lbl_80426BD0.member[member].timer = 300;
    }
}

void partnerGetTarget(s32 member, HeroMoveVec* velocity, HeroMoveVec* result)
{
    extern u32 GSgetFrameStep(void);
    extern void PSVECScale(HeroMoveVec*, HeroMoveVec*, f32);
    extern void PSVECAdd(HeroMoveVec*, HeroMoveVec*, HeroMoveVec*);
    extern void PSVECSubtract(HeroMoveVec*, HeroMoveVec*, HeroMoveVec*);

    HeroMoveVec leaderPos;
    HeroMoveVec pos;
    HeroMoveVec direction;
    HeroMoveVec step;
    HeroMoveVec logPos;
    HeroMoveVec hit;
    HeroMoveVec next;
    HeroMoveVec projected;
    HeroMoveVec segment;
    u8 hasLog;
    f32 spacing;
    f32 frames;
    f32 leaderDistance;
    f32 logDistance;
    f32 scale;

    /* RULE-EXCEPTION(user-approved): raw byte-offset read keeps retail's address order — see docs/RULE_EXCEPTIONS.md */
    spacing = *(f32*)((u8*)&lbl_80426BD0 + member * 0x20 + 8);
    frames = (f32)GSgetFrameStep();

    getPos(&leaderPos, lbl_80426BD0.leader);
    hasLog = getLeaderLog(&logPos, 0);
    getPos(&pos, member);

    direction.x = leaderPos.x - pos.x;
    direction.y = 0.0f;
    direction.z = leaderPos.z - pos.z;
    leaderDistance =
        heroMoveSqrt(direction.x * direction.x + direction.z * direction.z);

    if (hasLog) {
        logDistance = heroMoveSqrt((pos.x - logPos.x) * (pos.x - logPos.x) +
                                   (pos.z - logPos.z) * (pos.z - logPos.z));
    } else {
        logDistance = 0.0f;
    }

    if (!hasLog || leaderDistance <= logDistance ||
        leaderDistance <= 0.010000001f + spacing) {
        if (leaderDistance > 0.010000001f + spacing) {
            f32 amount = leaderDistance - spacing;
            if (amount > frames) {
                amount = frames;
            }
            scale = amount / leaderDistance;
        } else if (leaderDistance < 9.0f) {
            f32 amount = 9.0f - leaderDistance;
            if (amount > frames) {
                amount = frames;
            }
            scale = -amount / 9.0f;
        } else {
            scale = 0.0f;
        }

        PSVECScale(&direction, velocity, scale);
        *result = direction;
        return;
    }

    direction.x = logPos.x - pos.x;
    direction.y = 0.0f;
    direction.z = logPos.z - pos.z;
    logDistance =
        heroMoveSqrt(direction.x * direction.x + direction.z * direction.z);
    if (logDistance > frames) {
        PSVECScale(&direction, &step, frames / logDistance);
    } else {
        step = direction;
    }

    PSVECAdd(&pos, &step, &next);
    if (circleLineIntersect(&pos, &next, &leaderPos, &pos, &hit, spacing) > 0) {
        f32 lengthSquared;
        f32 t;

        hit.y = pos.y;
        PSVECSubtract(&next, &pos, &segment);
        lengthSquared = heroMoveVecSquareMag(&segment);
        t = 0.0f;
        if (lengthSquared == 0.0f) {
            projected.x = pos.x;
            projected.y = pos.y;
            projected.z = pos.z;
        } else {
            t = (segment.x * (hit.x - pos.x) + segment.y * (hit.y - pos.y) +
                 segment.z * (hit.z - pos.z)) /
                lengthSquared;
            PSVECScale(&segment, &segment, t);
            PSVECAdd(&segment, &pos, &projected);
        }
        if (t >= 0.0f && t <= 1.0f) {
            PSVECSubtract(&hit, &pos, &step);
        }
    }

    *velocity = step;
    *result = step;
}

s32 circleLineIntersect(HeroMoveVec* start, HeroMoveVec* end, HeroMoveVec* center,
                HeroMoveVec* reference, HeroMoveVec* result, f32 radius)
{
    f32 t;
    f32 discriminant;
    f32 relX;
    f32 originX;
    f32 farZ;
    f32 nearZ;
    f32 originZ;
    f32 farDz;
    f32 farDx;
    f32 relZ;
    f32 nearDz;
    f32 root;
    f32 nearX;
    f32 normalLength;
    f32 a;
    f32 cross;
    f32 nearDx;
    f32 projection;
    f32 b;
    f32 refZ;
    f32 farX;
    f32 dirX;
    f32 dirZ;
    f32 lengthSquared;
    f32 scale;
    f32 c;
    f32 epsilon = 1e-7f;

    if (!heroMoveGetLine(start, end, &a, &b, &c)) {
        return -1;
    }

    scale = 1.0f / (a * a + b * b);
    originX = a * (-c * scale);
    originZ = b * (-c * scale);
    normalLength = heroMoveSqrt(scale);
    dirX = b * normalLength;
    dirZ = -a * normalLength;

    lengthSquared = heroMoveSq(dirX) + heroMoveSq(dirZ);
    if (lengthSquared < epsilon) {
        return 0;
    }

    relX = center->x - originX;
    relZ = center->z - originZ;
    cross = dirX * relZ - dirZ * relX;
    discriminant = lengthSquared * (radius * radius) - cross * cross;
    if (discriminant < -epsilon) {
        return 0;
    }

    projection = dirX * relX + dirZ * relZ;
    if (discriminant < epsilon) {
        t = projection / lengthSquared;
        result->x = dirX * t + originX;
        result->y = 0.0f;
        result->z = dirZ * t + originZ;
        return 1;
    }

    root = heroMoveSqrt(lengthSquared * (radius * radius) - cross * cross);
    scale = 1.0f / lengthSquared;
    t = scale * (projection - root);
    nearX = dirX * t + originX;
    nearZ = dirZ * t + originZ;
    t = scale * (projection + root);
    farX = dirX * t + originX;
    farZ = dirZ * t + originZ;

    refZ = reference->z;
    nearDx = reference->x - nearX;
    nearDz = refZ - nearZ;
    farDx = reference->x - farX;
    farDz = refZ - farZ;
    if (heroMoveSq(nearDx) + heroMoveSq(nearDz) <
        heroMoveSq(farDx) + heroMoveSq(farDz)) {
        result->x = nearX;
        result->y = 0.0f;
        result->z = nearZ;
    } else {
        result->x = farX;
        result->y = 0.0f;
        result->z = farZ;
    }
    return 2;
}

void heroMoveGetHeroPos(u32 param) {
    extern void GSmodelGetPosition(void* a, u32 b);

    GSmodelGetPosition(heroMoveGetModel(lbl_80426BD0.leader), param);
}

void heroMoveGetHeroRot(u32 param) {
    extern void GSmodelGetRotation(void* a, u32 b);

    GSmodelGetRotation(heroMoveGetModel(lbl_80426BD0.leader), param);
}

void partnerApplyMove(s32 member, HeroMoveVec* dir, HeroMoveVec* fwd)
{
    extern u32 GSgetFrameStep(void);
    extern void PSVECSubtract(void*, void*, void*);
    extern void PSVECScale(void*, void*, f32);
    extern f32 PSVECDotProduct(void*, void*);
    extern void peopleStopTurn(u32, u32);
    extern void peopleStartTurn(u32, u32, f32, f32);
    extern void peopleMoveByVector(u32, u32, HeroMoveVec*, u32);
    extern f32 peopleAmountFromMove(u32, u32, HeroMoveVec*);
    extern void* peopleInfoBiosGetPtr(s32);
    extern f32 charModelGetHeadYawMin(void*);
    extern f32 charModelGetHeadYawMax(void*);

    HeroMoveVec rot;
    HeroMoveVec target;
    HeroMoveVec self;
    HeroMoveVec scaled;
    HeroMoveVec move;
    HeroMoveVec before;
    HeroMoveVec after;
    u32 group;
    u32 id;
    f32 speed;
    f32 heading;
    f32 frames;
    f32 angle;
    f32 turn;
    f32 length;
    f32 diff;
    f32 limitPos;
    f32 limitNeg;
    void* info;
    u8 rotate;

    frames = (f32)GSgetFrameStep();
    getResID(&group, &id, member);
    length = heroMoveSqrt(dir->x * dir->x + dir->z * dir->z);

    if (length > 0.0f) {
        getPos(&before, member);
        before.y = 0.0f;
        peopleMoveByVector(group, id, dir, 0);
        getPos(&after, member);
        after.y = 0.0f;
        PSVECSubtract(&after, &before, &move);
        speed = length / frames;
        if (speed > 1.0f) {
            speed = 1.0f;
        }
        PSVECScale(&move, &scaled, speed / length);
        if (heroMoveSqrt(fwd->x * fwd->x + fwd->z * fwd->z) > 0.010000001f) {
            peopleStartTurn(group, id, (f32)atan2(fwd->x, fwd->z), speed);
        }
        turn = peopleAmountFromMove(group, id, &scaled);
        if (PSVECDotProduct(&move, fwd) < 0.0f) {
            turn = -turn;
        }
    } else if (heroMoveGetNeckMode(member) != HERO_MOVE_NECK_ON) {
        peopleStopTurn(group, id);
        turn = 0.0f;
    } else {
        getPos(&target, lbl_80426BD0.leader);
        getPos(&self, member);
        getRot(&rot, member);

        {
            f32 y = rot.y;

            while (y >= 6.283184f) {
                y -= 6.283184f;
            }
            while (y <= -6.283184f) {
                y += 6.283184f;
            }
            rot.y = y;
        }

        angle = (f32)atan2(target.x - self.x, target.z - self.z);
        diff = heroMoveWrapAngle(angle - rot.y);

        info = peopleInfoBiosGetPtr(getObjID(member));
        limitPos = HERO_MOVE_ABS(charModelGetHeadYawMin(info));
        limitNeg = HERO_MOVE_ABS(charModelGetHeadYawMax(info));

        rotate = FALSE;
        if (diff < 0.0f) {
            diff = HERO_MOVE_ABS(diff);
            if (diff > limitNeg) {
                heading = angle + limitNeg;
                if (heading >= 6.283184f) {
                    heading -= 6.283184f;
                }
                rotate = TRUE;
            }
        } else if (diff > limitPos) {
            heading = heroMoveWrapAngle(angle - limitPos);
            rotate = TRUE;
        }

        if (rotate && HERO_MOVE_ABS(heroMoveWrapAngle(heading - rot.y)) < 0.0017453289f) {
            heading = rot.y;
            rotate = FALSE;
        }

        if (rotate) {
            peopleStartTurn(group, id, heading, 1.0f);
            turn = 1.0f;
        } else {
            peopleStopTurn(group, id);
            turn = 0.0f;
        }
    }

    heroMoveAnimate(member, turn);
}

void updateAnimation__Ff15HEROMOVE_MEMBER(void* model, s32 member, f32 amount)
{
    extern void* peopleInfoBiosGetPtr(s32);
    extern void fn_8018F4C8(void*, u8, s32*, u8*);
    extern void GSmodelGetAnimIndex(void*, s32*, s32*);
    extern f32 GSmodelGetAnimFrame(void*);
    extern void GSmodelGetFrameCount(void*, f32*, f32*);
    extern void GSmodelSetAnimIndex(void*, s32);
    extern void GSmodelSetAnimFrame(void*, f32);
    extern void GSmodelSetAnimRate(void*, f32);
    extern void GSmodelSetAnimBlend(void*, s32, s32);
    extern void GSmodelSetBlendFactor(void*, f32);
    s32 current_anim;
    s32 blend_anim;
    s32 anim1;
    s32 anim2;
    s32 anim3;
    s32 anim4;
    f32 frame_count_a;
    f32 frame_count_b;
    u8 loop;
    f32 frame;
    void* info;

    info = peopleInfoBiosGetPtr(getObjID(member));
    fn_8018F4C8(info, 1, &anim1, &loop);
    fn_8018F4C8(info, 2, &anim2, &loop);
    fn_8018F4C8(info, 3, &anim3, &loop);
    fn_8018F4C8(info, 4, &anim4, &loop);

    if (amount > 2.0f) {
        amount = 2.0f;
    }
    GSmodelGetAnimIndex(model, &current_anim, &blend_anim);

    if (amount < -0.4f) {
        if (current_anim != anim4 || blend_anim != -1) {
            frame = 0.0f;
            if (blend_anim != -1) {
                GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
                frame = (frame_count_a / frame_count_b) *
                        GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimIndex(model, anim4);
            GSmodelSetAnimFrame(model, frame);
        }
        GSmodelSetAnimRate(model, 1.0f);
    } else if (amount < 0.0f) {
        if (current_anim != anim4 || blend_anim != anim1) {
            frame = 0.0f;
            if (blend_anim == -1) {
                frame = GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimBlend(model, anim4, anim1);
            GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
            GSmodelSetAnimFrame(model,
                                frame * (frame_count_b / frame_count_a));
        }
        amount = (amount - -0.4f) / 0.4f;
        GSmodelSetBlendFactor(model, amount);
        GSmodelSetAnimRate(model, 0.5f);
    } else if (amount < 0.4f) {
        if (current_anim != anim2 || blend_anim != anim1) {
            frame = 0.0f;
            if (blend_anim == -1) {
                frame = GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimBlend(model, anim2, anim1);
            GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
            GSmodelSetAnimFrame(model,
                                frame * (frame_count_b / frame_count_a));
        }
        GSmodelSetBlendFactor(model, 1.0f - 2.5f * amount);
        GSmodelSetAnimRate(model, 0.5f);
    } else if (amount < 1.0f) {
        if (current_anim != anim2 || blend_anim != -1) {
            frame = 0.0f;
            if (blend_anim != -1) {
                GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
                frame = (frame_count_a / frame_count_b) *
                        GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimIndex(model, anim2);
            GSmodelSetAnimFrame(model, frame);
        }
        GSmodelSetAnimRate(model, 0.5f);
    } else {
        if (current_anim != anim2 || blend_anim != anim3) {
            frame = 0.0f;
            if (blend_anim == -1) {
                frame = GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimBlend(model, anim2, anim3);
            GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
            GSmodelSetAnimFrame(model,
                                frame * (frame_count_b / frame_count_a));
        }
        amount = amount - 1.0f;
        GSmodelSetBlendFactor(model, amount);
        GSmodelSetAnimRate(model, 0.5f);
    }
}

s32 heroMoveCheckEvent(void* event)
{
    typedef struct HeroMoveEventVec {
        f32 x;
        f32 y;
        f32 z;
    } HeroMoveEventVec;
    extern u32 peopleIsInGroup(u32 group, u32 handle);
    extern void* peopleSearchID(u32 id);
    extern HeroMoveEventVec* peopleGetPositionPtr(void* person);
    extern HeroMoveEventVec* peopleGetPosition(void* person);
    extern f64 sin(f64 angle);
    extern f64 cos(f64 angle);
    extern void PSVECAdd(HeroMoveEventVec* dst, const HeroMoveEventVec* lhs,
                         const HeroMoveEventVec* rhs);
    extern s32 GScolsys2CheckGetEventID(const HeroMoveEventVec* position,
                                        const HeroMoveEventVec* offset,
                                        void* event);
    u32 group;
    u32 id;
    HeroMoveEventVec* rotation;
    HeroMoveEventVec* position;
    HeroMoveEventVec origin;
    HeroMoveEventVec offset;
    void* person;
    s32 result;

    getResID(&group, &id, lbl_80426BD0.leader);
    person = peopleSearchID(peopleIsInGroup(group, id));
    if (person != NULL) {
        position = peopleGetPositionPtr(person);
        rotation = peopleGetPosition(person);
        origin = *position;
        origin.y += 5.0f;
        offset.x = 10.0f * (f32)sin(rotation->y);
        offset.y = 0.0f;
        offset.z = 10.0f * (f32)cos(rotation->y);
        PSVECAdd(&origin, &offset, &offset);
        result = GScolsys2CheckGetEventID(&origin, &offset, event);
    }

    return result;
}

/* RULE-EXCEPTION(user-approved): local size-optimisation pragma (updateChat only, retail -O4,s codegen inside the -O4,p TU) - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma optimize_for_size on
u8 updateChat__F15HEROMOVE_MEMBER(s32 member)
{
    extern void cmsRunSync(void*, u32, ...);
    extern void peopleStopTurn(u32, u32);
    extern void fn_800F7D38(u32, u32, u32);
    extern void fn_800F7C8C(u32, u32, u32);
    extern void fn_8018BA04(u32, u32, HeroMoveVec*);
    extern void peopleTurnToPoint(u32, u32, f32, f32, f32, f32);
    extern void floorEventCtrlTresure(u32, u32, u32);
    extern void fn_80183730(u32);
    extern void peopleTalkPause(u32, u32, u32);
    extern void std055_characterEndSpeech(u32, u32);
    extern void fn_80183688(u32);
    u8 collision[0xD0];
    HeroMoveVec talkPos;
    HeroChatTarget* target;
    u32 group;
    u32 person;
    s32 eventID;
    u32 id;

    getResID(&group, &id, member);
    target = (HeroChatTarget*)peopleFindTalkTarget(group, id, 1.2f, 40.0f);
    if (target == NULL) {
        eventID = heroMoveCheckEvent(collision);
        memcpy(lbl_80426BD0.eventList[0], collision, 0xD0);
        lbl_80426BD0.eventValue[0] = eventID;
        if (eventID != 0) {
            peopleStopTurn(0, 100);
            return TRUE;
        }
        return FALSE;
    }

    fn_800F7D38(1, 0, 0);
    fn_800F7C8C(1, 0, 0);
    heroMoveInitEventChat();

    fn_8018BA04(target->group, target->id, &talkPos);
    peopleTurnToPoint(group, id, talkPos.x, talkPos.y, talkPos.z, 1.0f);
    if (fn_8018D7D0(target->group, target->id)) {
        floorEventCtrlTresure(target->group, target->id, 2);
    } else {
        fn_80183730(peopleIsInGroup(target->group, target->id));
        person = fn_8018397C(target->group, target->id);
        if (person != 0) {
            peopleTalkPause(target->group, target->id, 1);
            cmsRunSync((void*)person, 4, target->group, target->id, 0, 0);
            peopleTalkPause(target->group, target->id, 0);
            std055_characterEndSpeech(target->group, target->id);
        }
        fn_80183688(peopleIsInGroup(target->group, target->id));
    }

    heroMoveTermEvent();
    fn_800F7D38(1, 0, 0);
    fn_800F7C8C(1, 0, 0);
    return TRUE;
}
#pragma pop

void getStep__FP8FOOTSTEPP8_GSmodelPiP8FOOTWORK(
    f32* step, void* model, s32* partIndices, f32* footwork)
{
    typedef struct HeroMoveVec3 {
        f32 x;
        f32 y;
        f32 z;
    } HeroMoveVec3;
    HeroMoveVec3 modelPosition;
    HeroMoveVec3 transform;
    f32 heights[4];
    f32* height;
    f32 delta = (f32)GSgetFrameStep();
    void* part;
    s32 i;

    GSmodelGetPosition(model, &modelPosition);

    height = heights;
    for (i = 0; i < 4; i++) {
        HeroMoveVec3* position;

        height[i] = 0.0f;
        position = (HeroMoveVec3*)(step + 4 + i * 3);
        position->x = 0.0f;
        position->y = 0.0f;
        position->z = 0.0f;
        if (partIndices[i] >= 0) {
            part = GSmodelGetPart(model, partIndices[i]);
            if (part != NULL) {
                GSpartGetTransform(part, &transform, NULL, NULL);
                GSpartFree(part);
                height[i] = transform.y - modelPosition.y;
                *position = transform;
            }
        }
    }

    for (i = 0; i < 4; i++) {
        if (footwork[i] >= 0.5f && height[i] < 0.5f) {
            step[i] = (footwork[i] - height[i]) / delta;
        } else {
            step[i] = 0.0f;
        }
    }
    for (i = 0; i < 4; i++) {
        footwork[i] = heights[i];
    }
}

/*
 * XD proc1Step__Ff: run the step callbacks once per 7.0 units walked. In
 * Colosseum it is only ever inlined (into heroMoveMain), but it is defined
 * here, between heroMoveAddStepCallback and getStep as in XD, and its 7.0f
 * is pooled at that point (0x8047D064, between fn_8012B19C's constants and
 * getStep's).
 */
static void proc1Step(f32 distance)
{
    s32 i;

    lbl_80426BD0.stepAccum += distance;
    while (lbl_80426BD0.stepAccum >= 7.0f) {
        for (i = 0; i < 8; i++) {
            if (lbl_80426BD0.stepCallback[i].func != NULL) {
                lbl_80426BD0.stepCallback[i].func(lbl_80426BD0.stepCallback[i].arg);
            }
        }
        lbl_80426BD0.stepAccum -= 7.0f;
    }
}

s32 heroMoveAddStepCallback(void (*func)(s32 arg), s32 arg)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        if (lbl_80426BD0.stepCallback[i].func == NULL) {
            break;
        }
    }
    if (i >= 8) {
        return -1;
    }
    lbl_80426BD0.stepCallback[i].func = func;
    lbl_80426BD0.stepCallback[i].arg = arg;
    return i;
}

void heroMoveInitEvent(void)
{
    extern void updateAnimation__Ff15HEROMOVE_MEMBER(void* model, s32 member,
                                                     f32 frame);
    extern void peopleSetFlagsByIndex(u32 group, u32 handle, u32 flags);
    extern void peopleClearFlagsByIndex(u32 group, u32 handle, u32 flags);
    extern void std151_characterFaceDirection(u32 group, u32 handle, u32 flags);
    HEROMOVE_MEMBER member;
    u32 group;
    u32 id;

    for (member = 0; member < 2; member++) {
        if (heroMoveCheckMember(member)) {
            getResID(&group, &id, member);
            updateAnimation__Ff15HEROMOVE_MEMBER(heroMoveGetModel(member), member,
                                                 0.0f);
            peopleSetFlagsByIndex(group, id, 0x80000008);
            peopleClearFlagsByIndex(group, id, 0x100);
            peopleClearFlagsByIndex(group, id, 0x400);
            std151_characterFaceDirection(group, id, 0);
        }
    }
}

void heroMoveTermEvent(void)
{
    extern void std151_characterFaceDirection(u32 group, u32 handle, u32 flags);
    extern void peopleSetFlagsByIndex(u32 group, u32 handle, u32 flags);
    extern void peopleClearFlagsByIndex(u32 group, u32 handle, u32 flags);
    HEROMOVE_MEMBER member;
    u32 group;
    u32 id;

    for (member = 0; member < 2; member++) {
        if (heroMoveCheckMember(member)) {
            getResID(&group, &id, member);
            std151_characterFaceDirection(group, id, 1);
            peopleSetFlagsByIndex(group, id, 0x700);
            peopleClearFlagsByIndex(group, id, 0x80000008);
        }
    }
}

void heroMoveSetEventList(u8 type, void* src, u32 val) {
    switch (type) {
        case 1:
            memcpy(lbl_80426BD0.eventList[1], src, 0xd0);
            lbl_80426BD0.eventValue[1] = val;
            break;
        case 2:
            memcpy(lbl_80426BD0.eventList[2], src, 0xd0);
            lbl_80426BD0.eventValue[2] = val;
            break;
        case 3:
            memcpy(lbl_80426BD0.eventList[0], src, 0xd0);
            lbl_80426BD0.eventValue[0] = val;
            break;
    }
}

void heroMoveAddAutoEvent(u32 a, u32 b, u32 c, u32 d, u32 e) {
    u8* base = ((u8*)&lbl_80426BD0);
    *(u32*)(base + 0x18C) = a;
    *(u32*)(base + 0x190) = b;
    *(u32*)(base + 0x194) = c;
    *(u32*)(base + 0x198) = d;
    *(u32*)(base + 0x19C) = e;
}

u32 heroMoveChkHinderClear(s32 member) {
    extern u32 peopleIsInGroup(u32 group, u32 id);
    extern u8* peopleSearchID(u32 id);
    extern void* peopleInfoBiosGetPtr(s32 id);
    extern f32 fn_8018F5E4(void* info);
    extern s32 colsysWallSweep(void* start, void* end, f32 radius, void* result);
    extern void PSVECSubtract(void* a, void* b, void* out);
    extern void PSVECScale(void* src, void* dst, f32 scale);
    extern void PSVECAdd(void* a, void* b, void* out);
    extern s32 GScolsys2HumanCollision(u32 col, void* from, void* to, u32 flags);

    HeroMoveVec memberPos;
    HeroMoveVec leaderPos;
    HeroMoveVec dir;
    HeroMoveVec ofs;
    s32 leader;
    u32 objID;
    void* info;
    u8* people;
    u32 col;
    u32 group;
    u32 id;
    f32 memberRadius;
    f32 leaderRadius;
    f32 radiusSum;
    f32 dx;
    f32 dz;
    f32 dist;

    if (!heroMoveCheckMember(member)) {
        return 0;
    }
    leader = lbl_80426BD0.leader;
    getPos(&memberPos, member);
    memberPos.y += 8.5f;
    getPos(&leaderPos, leader);
    leaderPos.y += 8.5f;

    objID = getObjID(member);
    if (objID == -1) {
        return 0;
    }
    info = peopleInfoBiosGetPtr(objID);
    if (info == NULL) {
        return 0;
    }
    memberRadius = fn_8018F5E4(info);
    if (colsysWallSweep(&memberPos, &leaderPos, memberRadius, NULL) != 0) {
        return 0;
    }

    objID = getObjID(leader);
    if (objID == -1) {
        return 0;
    }
    info = peopleInfoBiosGetPtr(objID);
    if (info == NULL) {
        return 0;
    }
    leaderRadius = fn_8018F5E4(info);

    getResID(&group, &id, member);
    people = peopleSearchID(peopleIsInGroup(group, id));
    if (people == NULL) {
        return 0;
    }
    col = *(u32*)(people + 0x50);

    getPos(&memberPos, member);
    getPos(&leaderPos, leader);
    PSVECSubtract(&leaderPos, &memberPos, &dir);
    radiusSum = memberRadius + leaderRadius;
    dist = GSvecDistanceXZ(&memberPos, &leaderPos);
    if (dist <= 0.0f) {
        return 1;
    }
    if (dist <= radiusSum) {
        return 1;
    }

    PSVECScale(&dir, &ofs, ((dist - radiusSum) - 0.010000001f) / dist);
    PSVECAdd(&memberPos, &ofs, &leaderPos);
    return GScolsys2HumanCollision(col, &memberPos, &leaderPos, 0) != 6;
}

u32 fn_8012B19C(s32 member, HeroMoveVec* start, HeroMoveVec* target, f32 extraRadius) {
    extern u32 peopleIsInGroup(u32 group, u32 id);
    extern u8* peopleSearchID(u32 id);
    extern void* peopleInfoBiosGetPtr(s32 id);
    extern f32 fn_8018F5E4(void* info);
    extern s32 colsysWallSweep(void* start, void* end, f32 radius, void* result);
    extern void PSVECSubtract(void* a, void* b, void* out);
    extern void PSVECScale(void* src, void* dst, f32 scale);
    extern void PSVECAdd(void* a, void* b, void* out);
    extern s32 GScolsys2HumanCollision(u32 col, void* from, void* to, u32 flags);

    HeroMoveVec dir;
    HeroMoveVec ofs;
    HeroMoveVec goal;
    HeroMoveVec pos;
    u32 objID;
    void* info;
    u8* people;
    u32 col;
    u32 group;
    u32 id;
    f32 radius;
    f32 radiusSum;
    f32 dist;

    if (!heroMoveCheckMember(member)) {
        return 0;
    }
    if (start != NULL) {
        pos.x = start->x;
        pos.y = start->y;
        pos.z = start->z;
    } else {
        getPos(&pos, member);
    }
    pos.y += 8.5f;
    goal.x = target->x;
    goal.y = 8.5f + target->y;
    goal.z = target->z;

    objID = getObjID(member);
    if (objID == -1) {
        return 0;
    }
    info = peopleInfoBiosGetPtr(objID);
    if (info == NULL) {
        return 0;
    }
    radius = 0.5f * fn_8018F5E4(info);
    if (colsysWallSweep(&pos, &goal, radius, NULL) != 0) {
        return 0;
    }

    getResID(&group, &id, member);
    people = peopleSearchID(peopleIsInGroup(group, id));
    if (people == NULL) {
        return 0;
    }
    col = *(u32*)(people + 0x50);

    if (start != NULL) {
        pos.x = start->x;
        pos.y = start->y;
        pos.z = start->z;
    } else {
        getPos(&pos, member);
    }
    goal.x = target->x;
    goal.y = target->y;
    goal.z = target->z;
    PSVECSubtract(&goal, &pos, &dir);
    radiusSum = radius + extraRadius;
    dist = GSvecDistanceXZ(&pos, &goal);
    if (dist <= 0.0f) {
        return 1;
    }
    if (dist <= radiusSum) {
        return 1;
    }

    PSVECScale(&dir, &ofs, ((dist - radiusSum) - 0.010000001f) / dist);
    PSVECAdd(&pos, &ofs, &goal);
    return GScolsys2HumanCollision(col, &pos, &goal, 0) != 6;
}

void heroMoveSetLockFrame(s32 val) {
    if (val < 0) { return; }
    lbl_80426BD0.lockFrame = (u32)val;
}

void cbPoison__Fl15FootStepCounterl(s32 arg) {
    extern u32 heroGetStatus(u8* a, u32 b, u32 c);
    extern u8 pokemonCheckValid(u32 mon);
    extern u32 pokemonGetStatus(u32 mon, u32 a, u32 b, u32 c);
    extern void pokemonSetStatus(u32 mon, u32 a, u32 b, u32 c, u32 val);
    extern u8 fn_80121ADC(u32 mon, u32 status);
    extern void* pokemonBiosGetNicknamePtr(u32 mon);
    extern void msgctrlSetValue(u32 id, void* value);
    extern void winMsgOpenField(u32 msg, u32 a, u32 b);
    extern void winMsgCloseField(u32 a);
    extern void pokemonBiosGetItemDataId(u32 mon);
    extern void* itemDataBiosGetPtr(void);
    extern u32 itemDataBiosGetItemSoubiDataId(void* item);
    extern void pokemonGetFriendFormPokemonFriendFilterId(u32 mon, u32 itemId, u32 filter);
    extern void heroDecPokedoru(u32 hero, s32 amount);
    extern void fn_801D0AFC(s32 arg);
    extern void fn_80121B4C(u32 mon, u32 status);

    u32 expiredSlots[6];
    u32* readPtr;
    u32* expiredPtr;
    s32 slot;
    u32 itemId;
    u32 mon;
    u16 poison;
    s32 livingPoisoned;
    s32 expiredCount;
    u8 changed;
    void* nickname;

    livingPoisoned = 0;
    expiredCount = 0;
    changed = 0;
    lbl_80426BD0.poisonSteps++;
    if (lbl_80426BD0.poisonSteps < 4) {
        return;
    }
    lbl_80426BD0.poisonSteps = 0;

    readPtr = expiredSlots;
    expiredPtr = expiredSlots;
    for (slot = 0; slot < 6; slot++) {
        mon = heroGetStatus(NULL, 3, (u16)slot);
        if (mon != 0 && pokemonCheckValid(mon)) {
            poison = pokemonGetStatus(mon, 0, 0x83, 0);
            if (poison != 0) {
                if (fn_80121ADC(mon, 3) || fn_80121ADC(mon, 4)) {
                    poison--;
                    pokemonSetStatus(mon, 0, 0x83, 0, poison);
                    changed = 1;
                    if (poison == 0) {
                        *expiredPtr++ = slot;
                        expiredCount++;
                    }
                }
                if (poison != 0) {
                    livingPoisoned++;
                }
            }
        }
    }

    if (changed) {
        fadeEffectDokuStart();
    }
    if (expiredCount <= 0) {
        return;
    }

    heroMoveInitEventPoison();

    for (slot = 0; slot < expiredCount; slot++) {
        mon = heroGetStatus(NULL, 3, (u16)*readPtr);
        nickname = pokemonBiosGetNicknamePtr(mon);
        msgctrlSetValue(0x32, nickname);
        winMsgOpenField(0x444e, 1, 0);
        winMsgCloseField(1);

        pokemonBiosGetItemDataId(mon);
        {
            void* item = itemDataBiosGetPtr();
            if (item == NULL) {
                itemId = 0;
            } else {
                itemId = itemDataBiosGetItemSoubiDataId(item);
            }
        }
        pokemonGetFriendFormPokemonFriendFilterId(mon, itemId, 7);
        readPtr++;
    }

    if (livingPoisoned <= 0) {
        s32 money;

        winMsgOpenField(0x444f, 1, 0);
        winMsgCloseField(1);
        money = (s32)heroGetStatus(NULL, 0xc, 0);
        heroDecPokedoru(0, money / 2);
        fn_801D0AFC(1);

        for (slot = 0; slot < 6; slot++) {
            mon = heroGetStatus(NULL, 3, (u16)slot);
            if (mon != 0 && pokemonCheckValid(mon) && fn_80121ADC(mon, 0x3e)) {
                fn_80121B4C(mon, 0x3e);
            }
        }
        fn_80113FE8();
    }

    heroMoveTermEvent();
}

void cbTsureFriend__Fl15FootStepCounterl(s32 arg) {
    extern u32 heroGetStatus(u8* a, u32 b, u32 c);
    extern u32 pokemonCheckValid(u32 val);
    extern void pokemonBiosGetItemDataId(u32 val);
    extern void* itemDataBiosGetPtr(void);
    extern u32 itemDataBiosGetItemSoubiDataId(void* a);
    extern void pokemonGetFriendFormPokemonFriendFilterId(u32 a, u32 b, u32 c);
    u32* counter = (u32*)(((u8*)&lbl_80426BD0) + 0x184);
    u32 val;
    s32 i;
    u32 obj;
    void* result;

    *counter = *counter + 1;
    if ((s32)*counter < 0x100) { return; }
    *counter = 0;
    i = 0;
    do {
        obj = heroGetStatus(NULL, 3, (u16)i);
        if (obj != 0) {
            if ((u8)pokemonCheckValid(obj) != 0) {
                pokemonBiosGetItemDataId(obj);
                result = itemDataBiosGetPtr();
                if (result == NULL) {
                    val = 0;
                } else {
                    val = itemDataBiosGetItemSoubiDataId(result);
                }
                pokemonGetFriendFormPokemonFriendFilterId(obj, val, 5);
            }
        }
        i++;
    } while (i < 6);
}
