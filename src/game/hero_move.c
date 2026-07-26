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
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
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
extern void* fn_800FF56C(void);                             /* GSfloor get active */
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
void fn_80117C84(void);
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
extern void fn_801ED674(void);
void fn_801193BC(void);
extern s32 pokemonWazaGetMaxPP(u8* ptr, u16 idx);
extern void wazaGetStatus(void);
extern void pokemonBiosGetPokemonWazaPtr(void);
extern void pokemonResetBasisStatus(void* ptr);
void pokemonSetLevelBasisStatus(void);
extern void heroItemGetItemKindToItemAryPtr(void);
extern void heroSetStatus();
extern void heroGetStatus(void);
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
extern void fn_80119930(void);
extern void fn_80119BD0();
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
extern void fn_8012CA84();
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
extern s32 updateAnimation__Ff15HEROMOVE_MEMBER();
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
extern void fn_80113F48(void);
extern void fn_8018C1E8(u32, u32, u32);
extern void fn_801653CC(void);
extern void msgctrlSetValue(void);
extern void winMsgOpen(void);
extern void winMsgClose(void);
extern void pcboxDelItem(void);
extern void fn_8001E184(void);
void floorEventGetTresure(void);
extern void fn_8018B76C(void);
extern void fn_8018C7C8(void);
extern void fn_801902E0(void);
extern void fn_80166A28(void);
extern void peopleWaitSyncMotion(void);
extern void fn_80190528(void);
void floorEventCtrlTresure(void);
extern void floorGetResource(void);
extern void GSmodelSetAnimIndex(void);
extern void GSmodelSetAnimFrame(void);
extern void GSmodelSetAnimRate(void);
extern void GSmodelSetAnimType(void);
extern void GSmodelStartAnimation(void);
extern void _threadSwitch(void);
extern void GSmodelIsAnimating(void);
extern void GSmodelGetPart(void);
extern void GSpartGetTransform(void);
extern void GSpartFree();
extern void fn_8018AACC(void);
extern void peopleMoveCheck(u32 groupId, u32 index, u8 waitFlag);
extern void fn_8018805C(void);
extern void fn_80184470(void);
extern void fn_8018C0A8(void);
extern void fn_801669BC(void);
extern void GSmodelCanAnimate(void);
extern void fn_801845E4(void);
extern void fn_801860F8(void);
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
extern void fn_801903B0(void);
extern void fn_8018C558(void);
extern void fn_8018C8F4(void);
void floorEventCtrlDoor(void);
extern void EvlogSet__FScUl();
extern void scriptSetEventColID();
extern void fn_800F7434();
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
extern void fn_8016AB94();
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
void fn_801183EC(void);
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
void fn_801190DC(void);
extern void psInitDataBank(void);
extern void DCFlushRange();
extern u8 lbl_802727D8[];
void fn_801195AC(void);
extern void fn_8016A01C(void);
extern void psInitGenerator(void);
extern void fn_8016AAF4(void);
extern void fn_8019733C(void);
extern void fn_8019D618(void);
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
extern u32 fn_801906A0(u32);
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
extern void fn_800E0C54(void);
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
extern u8 lbl_80426BD0[];
extern void fadeEffectDokuStart(void);
extern void fn_8018C69C(void);
extern void fn_8018CA20(void);
extern void winMsgOpenField(void);
extern void winMsgCloseField(void);
extern void fn_801D0AFC(void);
extern void fn_80113FE8(void);
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f32 lbl_8047D038;
void cbPoison__Fl15FootStepCounterl(void);
extern u32 fn_8018D998();
extern u32 peopleSearchID();
extern void peopleInfoBiosGetPtr(void);
extern void fn_8018F5E4(void);
extern void fn_8010F320(void);
extern void PSVECScale(void);
extern void PSVECAdd(void);
extern void GScolsys2HumanCollision(void);
extern f32 lbl_8047D03C;
extern f32 lbl_8047D040;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f64 lbl_8047D058;
extern u8 lbl_80478AC0[4];
extern f32 lbl_8047D060;
u32 fn_8012B19C(s32 member, f32* start, f32* target, f32 extraRadius);
u32 heroMoveChkHinderClear(s32 member);
extern void fn_800D3088(void);
extern f64 lbl_8047D068;
void getStep__FP8FOOTSTEPP8_GSmodelPiP8FOOTWORK(f32*, void*, s32*, f32*);
extern u32 fn_8018CD08();
extern u32 fn_8018FCBC();
extern void fn_8018FC50(void);
extern void fn_800CE148(void);
extern void fn_800CDBE0(void);
extern s32 GScolsys2CheckGetEventID();
extern void fn_8018790C(void);
extern void fn_800F7D38(void);
extern void fn_800F7C8C(void);
extern void fn_8018BA04(void);
extern void fn_80187D48(void);
extern u8 fn_8018D7D0();
extern void fn_80183730(void);
extern u32 fn_8018397C();
extern void fn_801812E8(void);
extern void fn_80189490(void);
extern void fn_80183688(void);
extern f32 lbl_8047D070;
extern f32 lbl_8047D074;
extern f32 lbl_8047D078;
extern f32 lbl_8047D07C;
extern f32 lbl_8047D080;
u32 updateChat__F15HEROMOVE_MEMBER(s32 player);
s32 heroMoveCheckEvent(void* event);
extern void fn_8018F4C8(void);
extern void GSmodelGetAnimIndex(void);
extern void GSmodelGetAnimFrame(void);
extern void GSmodelSetAnimBlend(void);
extern void GSmodelSetBlendFactor(void);
extern f32 lbl_8047D084;
extern f32 lbl_8047D088;
extern f32 lbl_8047D08C;
extern f32 lbl_8047D090;
extern f32 lbl_8047D0A8;
extern s32 fn_8012D39C(void*, void*, void*, void*, void*, f32);
extern f32 lbl_8047D0AC;
void fn_8012D7F0(s32, void*, void*);
extern void fn_800E3C64(void);
extern f32 lbl_8047D0B0;
void fn_8012DE94(u32 playerIndex);
extern void fn_800F7A7C(void);
extern void fn_800F7A08(void);
extern void fn_800F7BC4(void);
extern void fn_80188214(void);
extern void fn_80166458(void);
extern f64 lbl_8047D0C8;
extern f32 lbl_8047D0B4;
extern f32 lbl_8047D0B8;
extern f32 lbl_8047D0BC;
extern f32 lbl_8047D0C0;
extern f32 lbl_8047D0C4;
void fn_8012E388(s32, f32*);
extern void fn_800F7AF0(void);
extern void fn_801887D8(void);
extern void PSVECDistance(void);
extern f32 lbl_8047D0D0;
extern f32 lbl_8047D0D4;
f32 moveLeader__F15HEROMOVE_MEMBER();
extern void dbgMenuIsOpen(void);
extern void menuIsCheck(void);
extern void fn_8018C424(void);
extern void fn_8000D710(void);
extern u8 lbl_80272A38[];
extern f32 lbl_8047D064;
void heroMoveMain(void);
extern void fn_80188AF4(u32, u32);
extern void fn_80188F78(u32, u32);
s32 fn_8012F1FC(s32);
s32 fn_8012F40C(s32);
extern void GSmodelGetRotation(void);
extern void fn_8010E138(void);
extern void GSmodelSetRotation(void);
extern f32 lbl_8047D0D8;
void initFloor__Fv(void);
extern u32 floorGetNextFloorID(void);
extern s32 fn_8006AE18(void);
extern u8 lbl_802729C0[];
extern u8 lbl_80272A10[];
u32 heroMoveGetKenObjID(void);
extern void fn_8018E050(u32, u32, u32);
extern void GSmodelEnableAnimBlend(void*);
extern void fn_8018CB5C(u32, u32);
extern void fn_80189328(u32, u32, u32);
extern void fn_8018BF24(u32, u32, void*);
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
extern void fn_801885C4(void);
extern void PSVECDotProduct(void);
extern void fn_8018F678(void);
extern void fn_8018F658(void);
extern f32 lbl_8047D094;
extern f32 lbl_8047D098;
extern f32 lbl_8047D09C;
extern f32 lbl_8047D0A0;
extern f32 lbl_8047D0A4;
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
extern u32 fn_80130CD8(void);
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
extern void fn_80116D30(u32 kind, u32 arg);
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
extern void fn_8011711C(u32 arg);
extern void fn_80117154(void);
extern void fn_80117164(void);
extern void fn_801171C8(void);
extern void fn_80117330(f32 arg);
extern u32 fn_801174C4(void);
extern void fn_801174F4(void);
extern void fn_80117500(void);
extern u32 fn_80117AD4(void);
extern u8 fn_80117AE4(u32 arg1);
extern void fn_80117C84(void);
extern void fn_80117D14(void);
extern void fn_80117E58(void* arg);
extern void fn_80118020(void);
extern void fn_80118070(void);
extern void fn_80118100(void);
extern void fn_80118104(u32 a, u8 b);
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
extern void fn_80119824(u32 count1, u32 count2);
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
extern void cbTsureFriend__Fl15FootStepCounterl(void);
extern void heroMoveSetLockFrame(s32 val);
extern void heroMoveAddAutoEvent(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void heroMoveSetEventList(u8 type, void* src, u32 val);
extern s32 heroMoveAddStepCallback(void (*callback)(void), s32 count);
extern u32 heroMoveSetNeckMode(s32 idx, s32 state);
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
extern u32 heroMoveGetResID(u32* out_zero, u32* out_val, s32 index);

/* 0x8012AC9C | 0xB4 */
extern u8 lbl_80426BD0[];
void fn_8013024C(void)
{
    extern void cbTsureFriend__Fl15FootStepCounterl(void);
    u32 resources[2];
    void (**callbacks)(void);
    u32* callback_counts;
    u32 active;
    f32 spacing;
    s32 i;

    *(u16*)(lbl_80426BD0 + 0x04) = 0;
    *(u16*)(lbl_80426BD0 + 0x24) = 0;
    *(u32*)(lbl_80426BD0 + 0x00) = 0;
    *(u32*)(lbl_80426BD0 + 0x188) = 0;

    *(u16*)(lbl_80426BD0 + 0x04) |= 1;
    resources[0] = *(u32*)&lbl_8047D030;
    resources[1] = *(u32*)&lbl_8047D034;
    if (*(s32*)(lbl_80426BD0 + 0x0C) == 1) {
        fn_80188AF4(0, resources[0]);
    }
    fn_80188F78(0, resources[0]);
    *(u32*)(lbl_80426BD0 + 0x0C) = 1;

    active = *(u32*)lbl_80426BD0;
    *(f32*)(lbl_80426BD0 + active * 0x20 + 8) = lbl_8047D038;
    spacing = lbl_8047D0D4;
    if ((*(u16*)(lbl_80426BD0 + 4) & 1) != 0 && active != 0) {
        *(f32*)(lbl_80426BD0 + 8) = spacing;
        spacing += spacing;
    }
    if ((*(u16*)(lbl_80426BD0 + 0x24) & 1) != 0 && active != 1) {
        *(f32*)(lbl_80426BD0 + 0x28) = spacing;
    }
    fn_8018C1E8(0, resources[0], 1);

    if ((*(u16*)(lbl_80426BD0 + 4) & 1) != 0) {
        *(u32*)lbl_80426BD0 = 0;
        if (*(u32*)(lbl_80426BD0 + 0x0C) == 1) {
            fn_80188AF4(0, resources[0]);
        }
        *(u32*)(lbl_80426BD0 + 0x0C) = 0;

        active = *(u32*)lbl_80426BD0;
        *(f32*)(lbl_80426BD0 + active * 0x20 + 8) = lbl_8047D038;
        spacing = lbl_8047D0D4;
        if ((*(u16*)(lbl_80426BD0 + 4) & 1) != 0 && active != 0) {
            *(f32*)(lbl_80426BD0 + 8) = spacing;
            spacing += spacing;
        }
        if ((*(u16*)(lbl_80426BD0 + 0x24) & 1) != 0 && active != 1) {
            *(f32*)(lbl_80426BD0 + 0x28) = spacing;
        }
    }

    *(u32*)(lbl_80426BD0 + 0x44) = 0;
    *(u32*)(lbl_80426BD0 + 0x48) = 0;
    *(u32*)(lbl_80426BD0 + 0x18C) = 0;
    *(f32*)(lbl_80426BD0 + 0x13C) = lbl_8047D038;

    callbacks = (void (**)(void))(lbl_80426BD0 + 0x140);
    callback_counts = (u32*)(lbl_80426BD0 + 0x144);
    for (i = 0; i < 9; i++) {
        callbacks[i * 2] = NULL;
    }

    for (i = 0; i < 8; i++) {
        if (callbacks[i * 2] == NULL) {
            break;
        }
    }
    if (i < 8) {
        callbacks[i * 2] = cbPoison__Fl15FootStepCounterl;
        callback_counts[i * 2] = 0;
    }

    *(u32*)(lbl_80426BD0 + 0x184) = 0;
    for (i = 0; i < 8; i++) {
        if (callbacks[i * 2] == NULL) {
            break;
        }
    }
    if (i < 8) {
        callbacks[i * 2] = cbTsureFriend__Fl15FootStepCounterl;
        callback_counts[i * 2] = 0;
    }
}

#if 0
asm void cbTsureFriend__Fl15FootStepCounterl(void) {
#include "src/game/gs_field_world_fn_8012AC9C.inc"
}
void heroMoveSyncWithHero(void);
#else
void cbTsureFriend__Fl15FootStepCounterl(void) {
    extern u32 heroGetStatus(u8* a, u32 b, u32 c);
    extern u32 pokemonCheckValid(u32 val);
    extern void pokemonBiosGetItemDataId(u32 val);
    extern void* itemDataBiosGetPtr(void);
    extern u32 itemDataBiosGetItemSoubiDataId(void* a);
    extern void pokemonGetFriendFormPokemonFriendFilterId(u32 a, u32 b, u32 c);
    u32* counter = (u32*)(lbl_80426BD0 + 0x184);
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
#endif
/* 0x8012AD50 | 0x434 */
extern void fadeEffectDokuStart(void);
extern void fn_8018C69C(void);
extern void fn_8018CA20(void);
extern void winMsgOpenField(void);
extern void winMsgCloseField(void);
extern void fn_801D0AFC(void);
extern void fn_80113FE8(void);
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f32 lbl_8047D038;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void cbPoison__Fl15FootStepCounterl(void);
extern f32 lbl_8047D038;
void cbPoison__Fl15FootStepCounterl(void) {
    extern u32 heroGetStatus(u8* a, u32 b, u32 c);
    extern u8 pokemonCheckValid(u32 mon);
    extern u32 pokemonGetStatus(u32 mon, u32 a, u32 b, u32 c);
    extern void pokemonSetStatus(u32 mon, u32 a, u32 b, u32 c, u32 val);
    extern u8 fn_80121ADC(u32 mon, u32 status);
    extern void* GSresGetResource(u32 group, u32 handle);
    extern void updateAnimation__Ff15HEROMOVE_MEMBER(void* model, s32 member, f32 frame);
    extern void fn_8018C7C8(u32 a, u32 handle, u32 flags);
    extern void fn_8018C69C(u32 a, u32 handle, u32 flags);
    extern void fn_8018CA20(u32 a, u32 handle, u32 flags);
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
    u32 handles[2];
    u32* expiredPtr;
    u32 mon;
    u32 itemId;
    u32 handle;
    u16 poison;
    s32 livingPoisoned;
    s32 expiredCount;
    s32 changed;
    s32 member;
    s32 slot;

    *(u32*)(lbl_80426BD0 + 0x180) = *(u32*)(lbl_80426BD0 + 0x180) + 1;
    if ((s32)*(u32*)(lbl_80426BD0 + 0x180) < 4) {
        return;
    }

    *(u32*)(lbl_80426BD0 + 0x180) = 0;
    expiredPtr = expiredSlots;
    livingPoisoned = 0;
    expiredCount = 0;
    changed = 0;

    for (slot = 0; slot < 6; slot++) {
        mon = heroGetStatus(NULL, 3, (u16)slot);
        if (mon != 0 && (u8)pokemonCheckValid(mon) != 0) {
            poison = (u16)pokemonGetStatus(mon, 0, 0x83, 0);
            if (poison != 0) {
                if ((u8)fn_80121ADC(mon, 3) != 0 || (u8)fn_80121ADC(mon, 4) != 0) {
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

    if ((u8)changed != 0) {
        fadeEffectDokuStart();
    }
    if (expiredCount <= 0) {
        return;
    }

    for (member = 0; member < 2; member++) {
        if ((*(u16*)(lbl_80426BD0 + (u32)member * 0x20 + 4) & 1) != 0) {
            handles[0] = *(u32*)&lbl_8047D030;
            handles[1] = *(u32*)&lbl_8047D034;
            handle = handles[member];

            updateAnimation__Ff15HEROMOVE_MEMBER(GSresGetResource(0, handle), member, lbl_8047D038);
            fn_8018C7C8(0, handle, 0x80000008);
            fn_8018C69C(0, handle, 0x100);
            fn_8018C69C(0, handle, 0x400);
            fn_8018CA20(0, handle, 0);
        }
    }

    expiredPtr = expiredSlots;
    for (slot = 0; slot < expiredCount; slot++) {
        mon = heroGetStatus(NULL, 3, (u16)*expiredPtr);
        msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(mon));
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
        expiredPtr++;
    }

    if (livingPoisoned <= 0) {
        s32 money;

        winMsgOpenField(0x444f, 1, 0);
        winMsgCloseField(1);
        money = (s32)heroGetStatus(NULL, 0xc, 0);
        heroDecPokedoru(0, (money + (s32)((u32)money >> 31)) >> 1);
        fn_801D0AFC(1);

        for (slot = 0; slot < 6; slot++) {
            mon = heroGetStatus(NULL, 3, (u16)slot);
            if (mon != 0 && (u8)pokemonCheckValid(mon) != 0 &&
                (u8)fn_80121ADC(mon, 0x3e) != 0) {
                fn_80121B4C(mon, 0x3e);
            }
        }
        fn_80113FE8();
    }

    for (member = 0; member < 2; member++) {
        if ((*(u16*)(lbl_80426BD0 + (u32)member * 0x20 + 4) & 1) != 0) {
            handles[0] = *(u32*)&lbl_8047D030;
            handles[1] = *(u32*)&lbl_8047D034;
            handle = handles[member];
            fn_8018CA20(0, handle, 1);
            fn_8018C7C8(0, handle, 0x700);
            fn_8018C69C(0, handle, 0x80000008);
        }
    }
}
/* 0x8012B184 | 0x18 */
extern u8 lbl_80426BD0[];
void heroMoveSetLockFrame(s32 val) {
    if (val < 0) { return; }
    *(u32*)(lbl_80426BD0 + 0x188) = (u32)val;
}
/* 0x8012B19C | 0x448 */
extern u32 fn_8018D998();
extern u32 peopleSearchID();
extern void peopleInfoBiosGetPtr(void);
extern void fn_8018F5E4(void);
extern void fn_8010F320(void);
extern void PSVECScale(void);
extern void PSVECAdd(void);
extern void GScolsys2HumanCollision(void);
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f32 lbl_8047D03C;
extern f32 lbl_8047D040;
extern f32 lbl_8047D038;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f64 lbl_8047D058;
extern u8 lbl_80478AC0[4];
extern f32 lbl_8047D060;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
u32 fn_8012B19C(s32 member, f32* start, f32* target, f32 extraRadius);
/* 0x8012B5E4 | 0x4EC */
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f32 lbl_8047D03C;
extern f32 lbl_8047D038;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f64 lbl_8047D058;
extern f32 lbl_8047D060;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
u32 heroMoveChkHinderClear(s32 member);
extern f32 lbl_8047D060;
u32 fn_8012B19C(s32 member, f32* start, f32* target, f32 extraRadius) {
    extern void* GSresGetResource(u32 group, u32 handle);
    extern void GSmodelGetPosition(void* model, void* out);
    extern void fn_8018D998(u32 group, u32 handle);
    extern u8* peopleSearchID(void);
    extern void* peopleInfoBiosGetPtr(s32 id);
    extern f32 fn_8018F5E4(void* info);
    extern s32 fn_8010F320(void* a, void* b, u32 flags);
    extern void PSVECSubtract(void* a, void* b, void* out);
    extern void PSVECScale(void* src, void* dst, f32 scale);
    extern void PSVECAdd(void* a, void* b, void* out);
    extern s32 GScolsys2HumanCollision(u32 col, void* from, void* to, u32 flags);

    u32 handles[2];
    u32 handle = 0;
    u32 handle2 = 0;
    u32 handle3 = 0;
    u32 col;
    u8* people;
    void* info;
    void* model;
    s32 infoId;
    f32 from[3];
    f32 to[3];
    f32 delta[3];
    f32 scaled[3];
    f32 baseRadius;
    f32 radius;
    f32 dx;
    f32 dz;
    f32 dist;

    if (member < 0 || member >= 2) {
        return 0;
    }
    if ((*(u16*)(lbl_80426BD0 + ((u32)member << 5) + 4) & 1) == 0) {
        return 0;
    }

    if (start != NULL) {
        from[0] = start[0];
        from[1] = start[1];
        from[2] = start[2];
    } else {
        handles[0] = *(u32*)&lbl_8047D030;
        handles[1] = *(u32*)&lbl_8047D034;
        handle = handles[member];
        model = GSresGetResource(0, handle);
        GSmodelGetPosition(model, from);
    }
    from[1] += lbl_8047D03C;

    to[0] = target[0];
    to[1] = target[1] + lbl_8047D03C;
    to[2] = target[2];

    handles[0] = *(u32*)&lbl_8047D030;
    handles[1] = *(u32*)&lbl_8047D034;
    handle2 = handles[member];
    fn_8018D998(0, handle2);
    people = peopleSearchID();
    if (people == NULL) {
        return 0;
    }
    infoId = *(s32*)(people + 0x30);
    if (infoId == -1) {
        return 0;
    }
    info = peopleInfoBiosGetPtr(infoId);
    if (info == NULL) {
        return 0;
    }

    baseRadius = lbl_8047D040 * fn_8018F5E4(info);
    if (fn_8010F320(from, to, 0) != 0) {
        return 0;
    }

    handles[0] = *(u32*)&lbl_8047D030;
    handles[1] = *(u32*)&lbl_8047D034;
    handle3 = handles[member];
    fn_8018D998(0, handle3);
    people = peopleSearchID();
    if (people == NULL) {
        return 0;
    }
    col = *(u32*)(people + 0x50);

    if (start != NULL) {
        from[0] = start[0];
        from[1] = start[1];
        from[2] = start[2];
    } else {
        handles[0] = *(u32*)&lbl_8047D030;
        handles[1] = *(u32*)&lbl_8047D034;
        handle = handles[member];
        model = GSresGetResource(0, handle);
        GSmodelGetPosition(model, from);
    }

    to[0] = target[0];
    to[1] = target[1];
    to[2] = target[2];
    PSVECSubtract(to, from, delta);

    radius = baseRadius + extraRadius;
    dx = from[0] - to[0];
    dz = from[2] - to[2];
    dist = dx * dx + dz * dz;
    if (dist > lbl_8047D038) {
        f64 inv;
        f64 a;
        f64 b;

        inv = 1.0 / (f64)dist;
        a = lbl_8047D048;
        b = lbl_8047D050;
        inv = inv * (b - (f64)dist * inv * inv * a);
        inv = inv * (b - (f64)dist * inv * inv * a);
        inv = inv * (b - (f64)dist * inv * inv * a);
        dist = (f32)((f64)dist * inv);
    } else if (dist < (f32)lbl_8047D058) {
        dist = *(f32*)lbl_80478AC0;
    }

    if (dist <= lbl_8047D038) {
        return 1;
    }
    if (dist <= radius) {
        return 1;
    }

    PSVECScale(delta, scaled, ((dist - radius) - lbl_8047D060) / dist);
    PSVECAdd(from, scaled, to);
    return GScolsys2HumanCollision(col, from, to, 0) == 6;
}
extern f32 lbl_8047D060;
u32 heroMoveChkHinderClear(s32 member) {
    extern u8 lbl_80426BD0[];
    extern u8 lbl_80478AC0[4];
    extern void* GSresGetResource(u32 group, u32 handle);
    extern void GSmodelGetPosition(void* model, void* out);
    extern void fn_8018D998(u32 group, u32 handle);
    extern u8* peopleSearchID(void);
    extern void* peopleInfoBiosGetPtr(s32 id);
    extern f32 fn_8018F5E4(void* info);
    extern s32 fn_8010F320(void* a, void* b, u32 flags);
    extern void PSVECSubtract(void* a, void* b, void* out);
    extern void PSVECScale(void* src, void* dst, f32 scale);
    extern void PSVECAdd(void* a, void* b, void* out);
    extern s32 GScolsys2HumanCollision(u32 col, void* from, void* to, u32 flags);

    u32 handles[2];
    u32 memberHandle = 0;
    u32 activeHandle = 0;
    s32 active;
    s32 infoId;
    u8* people;
    void* info;
    void* model;
    u32 col;
    f32 memberPos[3];
    f32 activePos[3];
    f32 delta[3];
    f32 scaled[3];
    f32 memberRadius;
    f32 activeRadius;
    f32 radiusSum;
    f32 dx;
    f32 dz;
    f32 dist;

    if (member < 0 || member >= 2) {
        return 0;
    }
    if ((*(u16*)(lbl_80426BD0 + ((u32)member << 5) + 4) & 1) == 0) {
        return 0;
    }

    handles[0] = *(u32*)&lbl_8047D030;
    handles[1] = *(u32*)&lbl_8047D034;
    memberHandle = handles[member];
    active = *(s32*)lbl_80426BD0;

    model = GSresGetResource(0, memberHandle);
    GSmodelGetPosition(model, memberPos);
    memberPos[1] += lbl_8047D03C;

    handles[0] = *(u32*)&lbl_8047D030;
    handles[1] = *(u32*)&lbl_8047D034;
    if (active >= 0 && active < 2) {
        activeHandle = handles[active];
    }
    model = GSresGetResource(0, activeHandle);
    GSmodelGetPosition(model, activePos);
    activePos[1] += lbl_8047D03C;

    fn_8018D998(0, memberHandle);
    people = peopleSearchID();
    if (people == NULL) {
        return 0;
    }
    infoId = *(s32*)(people + 0x30);
    if (infoId == -1) {
        return 0;
    }
    info = peopleInfoBiosGetPtr(infoId);
    if (info == NULL) {
        return 0;
    }
    memberRadius = fn_8018F5E4(info);

    if (fn_8010F320(memberPos, activePos, 0) != 0) {
        return 0;
    }

    fn_8018D998(0, activeHandle);
    people = peopleSearchID();
    if (people == NULL) {
        return 0;
    }
    infoId = *(s32*)(people + 0x30);
    if (infoId == -1) {
        return 0;
    }
    info = peopleInfoBiosGetPtr(infoId);
    if (info == NULL) {
        return 0;
    }
    activeRadius = fn_8018F5E4(info);

    fn_8018D998(0, memberHandle);
    people = peopleSearchID();
    if (people == NULL) {
        return 0;
    }
    col = *(u32*)(people + 0x50);

    model = GSresGetResource(0, memberHandle);
    GSmodelGetPosition(model, memberPos);
    model = GSresGetResource(0, activeHandle);
    GSmodelGetPosition(model, activePos);
    PSVECSubtract(activePos, memberPos, delta);

    radiusSum = memberRadius + activeRadius;
    dx = memberPos[0] - activePos[0];
    dz = memberPos[2] - activePos[2];
    dist = dx * dx + dz * dz;
    if (dist > lbl_8047D038) {
        f64 inv;
        f64 a;
        f64 b;

        inv = 1.0 / (f64)dist;
        a = lbl_8047D048;
        b = lbl_8047D050;
        inv = inv * (b - (f64)dist * inv * inv * a);
        inv = inv * (b - (f64)dist * inv * inv * a);
        inv = inv * (b - (f64)dist * inv * inv * a);
        dist = (f32)((f64)dist * inv);
    } else if (dist < (f32)lbl_8047D058) {
        dist = *(f32*)lbl_80478AC0;
    }

    if (dist <= lbl_8047D038) {
        return 1;
    }
    if (dist <= radiusSum) {
        return 1;
    }

    PSVECScale(delta, scaled, ((dist - radiusSum) - lbl_8047D060) / dist);
    PSVECAdd(memberPos, scaled, activePos);
    return GScolsys2HumanCollision(col, memberPos, activePos, 0) == 6;
}
/* 0x8012BAD0 | 0x20 */
void heroMoveAddAutoEvent(u32 a, u32 b, u32 c, u32 d, u32 e) {
    u8* base = lbl_80426BD0;
    *(u32*)(base + 0x18C) = a;
    *(u32*)(base + 0x190) = b;
    *(u32*)(base + 0x194) = c;
    *(u32*)(base + 0x198) = d;
    *(u32*)(base + 0x19C) = e;
}
/* 0x8012BAF0 | 0xB8 */
#if 0
asm void fn_8012BAF0(void) {
#include "src/game/gs_field_world_fn_8012BAF0.inc"
}
#else
void heroMoveSetEventList(u8 type, void* src, u32 val) {
    extern u8 lbl_80426BD0[];
    switch (type) {
        case 1:
            memcpy(lbl_80426BD0 + 0x270, src, 0xd0);
            *(u32*)(lbl_80426BD0 + 0x414) = val;
            break;
        case 2:
            memcpy(lbl_80426BD0 + 0x340, src, 0xd0);
            *(u32*)(lbl_80426BD0 + 0x418) = val;
            break;
        case 3:
            memcpy(lbl_80426BD0 + 0x1a0, src, 0xd0);
            *(u32*)(lbl_80426BD0 + 0x410) = val;
            break;
    }
}
#endif
/* 0x8012BBA8 | 0xFC */
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
#if 0
asm void heroMoveTermEvent(void) {
#include "src/game/gs_field_world_fn_8012BBA8.inc"
}
#else
void heroMoveTermEvent(void) {
    extern u8 lbl_80426BD0[];
    extern void fn_8018CA20(u32 a, u32 b, u32 c);
    extern void fn_8018C7C8(u32 a, u32 b, u32 c);
    extern void fn_8018C69C(u32 a, u32 b, u32 c);
    s32 i;
    s32 offset;
    s32 idx;
    u32 table[2];
    u32 val;
    u8 flag;
    i = 0;
    offset = 0;
    idx = 0;
    do {
        if (i >= 0 && i < 2) {
            if (*(u16*)(&lbl_80426BD0[offset] + 4) & 1) {
                flag = 1;
            } else {
                flag = 0;
            }
        } else {
            flag = 0;
        }
        if ((u8)flag != 0) {
            table[0] = *(u32*)&lbl_8047D030;
            table[1] = *(u32*)&lbl_8047D034;
            if (i >= 0 && i < 2) {
                val = table[idx / 4];
            }
            fn_8018CA20(0, val, 1);
            fn_8018C7C8(0, val, 0x700);
            fn_8018C69C(0, val, 0x80000008);
        }
        i++;
        offset += 0x20;
        idx += 4;
    } while (i < 2);
}
#endif
/* 0x8012BCA4 | 0x13C */
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f32 lbl_8047D038;
void heroMoveInitEvent(void)
{
    extern void* GSresGetResource(u32 group, u32 handle);
    extern void updateAnimation__Ff15HEROMOVE_MEMBER(void* model, s32 member,
                                                     f32 frame);
    extern void fn_8018C7C8(u32 group, u32 handle, u32 flags);
    extern void fn_8018C69C(u32 group, u32 handle, u32 flags);
    extern void fn_8018CA20(u32 group, u32 handle, u32 flags);
    u32 handles[2];
    s32 member;

    handles[0] = *(u32*)&lbl_8047D030;
    handles[1] = *(u32*)&lbl_8047D034;
    for (member = 0; member < 2; member++) {
        if ((*(u16*)(lbl_80426BD0 + member * 0x20 + 4) & 1) != 0) {
            updateAnimation__Ff15HEROMOVE_MEMBER(
                GSresGetResource(0, handles[member]), member, lbl_8047D038);
            fn_8018C7C8(0, handles[member], 0x80000008);
            fn_8018C69C(0, handles[member], 0x100);
            fn_8018C69C(0, handles[member], 0x400);
            fn_8018CA20(0, handles[member], 0);
        }
    }
}
/* 0x8012BDE0 | 0xD4 */
#if 0
asm void fn_8012BDE0(void) {
#include "src/game/gs_field_world_fn_8012BDE0.inc"
}
#else
#pragma push
#pragma optimization_level 1
s32 heroMoveAddStepCallback(void (*callback)(void), s32 count) {
    u32 entry;
    s32 i = 0;

    entry = (u32)&lbl_80426BD0;
    if (*(void**)(entry + 0x140) != NULL) {
        entry += 8;
        i = 1;
        if (*(void**)(entry + 0x140) != NULL) {
            entry += 8;
            i = 2;
            if (*(void**)(entry + 0x140) != NULL) {
                entry += 8;
                i = 3;
                if (*(void**)(entry + 0x140) != NULL) {
                    entry += 8;
                    i = 4;
                    if (*(void**)(entry + 0x140) != NULL) {
                        entry += 8;
                        i = 5;
                        if (*(void**)(entry + 0x140) != NULL) {
                            entry += 8;
                            i = 6;
                            if (*(void**)(entry + 0x140) != NULL) {
                                entry += 8;
                                i = 7;
                                if (*(void**)(entry + 0x140) != NULL) {
                                    i = 8;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (i >= 8) {
        return -1;
    }
    entry = (u32)&lbl_80426BD0 + i * 8;
    *(void**)(entry + 0x140) = callback;
    *(s32*)(entry + 0x144) = count;
    return i;
}
#pragma pop
#endif
/* 0x8012BEB4 | 0x200 */
extern void fn_800D3088(void);
extern f64 lbl_8047D068;
extern f32 lbl_8047D038;
extern f32 lbl_8047D040;
void getStep__FP8FOOTSTEPP8_GSmodelPiP8FOOTWORK(
    f32* step, void* model, s32* partIndices, f32* footwork)
{
    f32 modelPosition[3];
    f32 zero = lbl_8047D038;
    s32 i;

    fn_800D3088();
    GSmodelGetPosition(model, modelPosition);

    for (i = 0; i < 4; i++) {
        f32* position = step + 4 + i * 3;

        position[0] = zero;
        position[1] = zero;
        position[2] = zero;
        step[i] = zero;
    }

    for (i = 0; i < 3; i++) {
        if (partIndices[i] >= 0 && footwork[i] >= lbl_8047D040) {
            step[i] = footwork[i];
        }
    }
}
/* 0x8012C0B4 | 0x48C */
extern u32 fn_8018CD08();
extern u32 fn_8018FCBC();
extern void fn_8018FC50(void);
extern void fn_800CE148(void);
extern void fn_800CDBE0(void);
extern s32 GScolsys2CheckGetEventID();
extern void fn_8018790C(void);
extern void fn_800F7D38(void);
extern void fn_800F7C8C(void);
extern void fn_8018BA04(void);
extern void fn_80187D48(void);
extern u8 fn_8018D7D0();
extern void fn_80183730(void);
extern u32 fn_8018397C();
extern void fn_801812E8(void);
extern void fn_80189490(void);
extern void fn_80183688(void);
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f32 lbl_8047D070;
extern f32 lbl_8047D074;
extern f32 lbl_8047D078;
extern f32 lbl_8047D07C;
extern f32 lbl_8047D038;
extern f32 lbl_8047D080;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
u32 updateChat__F15HEROMOVE_MEMBER(s32 player);
/* 0x8012C540 | 0x120 */
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f32 lbl_8047D078;
extern f32 lbl_8047D07C;
extern f32 lbl_8047D038;
s32 heroMoveCheckEvent(void* event)
{
    extern void fn_8018D998(u32 group, u32 handle);
    extern void* peopleSearchID(void);
    s32 member = *(s32*)lbl_80426BD0;
    u32 handles[2];

    handles[0] = *(u32*)&lbl_8047D030;
    handles[1] = *(u32*)&lbl_8047D034;
    if ((u32)member < 2) {
        fn_8018D998(0, handles[member]);
    }
    if (peopleSearchID() == NULL) {
        return -1;
    }

    /*
     * The remaining target code offsets the active person's position and
     * performs the event collision query into event.
     */
    return -1;
}
/* 0x8012C660 | 0x424 */
extern void fn_8018F4C8(void);
extern void GSmodelGetAnimIndex(void);
extern void GSmodelGetAnimFrame(void);
extern void GSmodelSetAnimBlend(void);
extern void GSmodelSetBlendFactor(void);
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f32 lbl_8047D084;
extern f32 lbl_8047D088;
extern f32 lbl_8047D038;
extern f32 lbl_8047D080;
extern f32 lbl_8047D08C;
extern f32 lbl_8047D040;
extern f32 lbl_8047D090;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
extern s32 updateAnimation__Ff15HEROMOVE_MEMBER();
/* 0x8012D39C | 0x454 */
extern f32 lbl_8047D0A8;
extern f32 lbl_8047D038;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f64 lbl_8047D058;
extern f32 lbl_8047D080;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
extern s32 fn_8012D39C(void*, void*, void*, void*, void*, f32);
/* 0x8012D7F0 | 0x6A4 */
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f64 lbl_8047D068;
extern f32 lbl_8047D038;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f64 lbl_8047D058;
extern f32 lbl_8047D060;
extern f32 lbl_8047D0AC;
extern f32 lbl_8047D080;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void fn_8012D7F0(s32, void*, void*);
/* 0x8012DE94 | 0x4F4 */
extern void fn_800E3C64(void);
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f32 lbl_8047D038;
extern f64 lbl_8047D058;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f32 lbl_8047D0B0;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void fn_8012DE94(u32 playerIndex);
/* 0x8012E388 | 0x430 */
extern void fn_800F7A7C(void);
extern void fn_800F7A08(void);
extern void fn_800F7BC4(void);
extern void fn_80188214(void);
extern void fn_80166458(void);
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f64 lbl_8047D0C8;
extern f32 lbl_8047D0B4;
extern f32 lbl_8047D038;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f64 lbl_8047D058;
extern f32 lbl_8047D084;
extern f32 lbl_8047D0B8;
extern f32 lbl_8047D0BC;
extern f32 lbl_8047D078;
extern f32 lbl_8047D0C0;
extern f32 lbl_8047D0C4;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void fn_8012E388(s32, f32*);
/* 0x8012E7B8 | 0x41C */
extern void fn_800F7AF0(void);
extern void fn_801887D8(void);
extern void PSVECDistance(void);
extern f32 lbl_8047D038;
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f64 lbl_8047D068;
extern f32 lbl_8047D080;
extern f32 lbl_8047D0D0;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f64 lbl_8047D058;
extern f32 lbl_8047D0D4;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
f32 moveLeader__F15HEROMOVE_MEMBER();
/* 0x8012EBD4 | 0x3E4 */
extern void dbgMenuIsOpen(void);
extern void menuIsCheck(void);
extern void fn_8018C424(void);
extern void fn_8000D710(void);
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern u8 lbl_80272A38[];
extern f32 lbl_8047D064;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void heroMoveMain(void);
/* 0x8012F008 | 0x114 */
extern void fn_80188AF4(u32, u32);
extern void fn_80188F78(u32, u32);
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
s32 updateAnimation__Ff15HEROMOVE_MEMBER(model, member, amount)
void* model;
s32 member;
f32 amount;
{
    extern void fn_8018D998(u32, u32);
    extern void* peopleSearchID(void);
    extern void* peopleInfoBiosGetPtr(s32);
    extern void fn_8018F4C8(void*, s32, s32*, void*);
    extern void GSmodelGetAnimIndex(void*, s32*, s32*);
    extern f32 GSmodelGetAnimFrame(void*);
    extern void GSmodelGetFrameCount(void*, f32*, f32*);
    extern void GSmodelSetAnimIndex(void*, s32);
    extern void GSmodelSetAnimFrame(void*, f32);
    extern void GSmodelSetAnimRate(void*, f32);
    extern void GSmodelSetAnimBlend(void*, s32, s32);
    extern void GSmodelSetBlendFactor(void*, f32);
    u32 resources[2];
    u8 query[12];
    s32 anim1;
    s32 anim2;
    s32 anim3;
    s32 anim4;
    s32 current_anim;
    s32 blend_anim;
    f32 frame_count_a;
    f32 frame_count_b;
    f32 frame;
    void* person;
    void* info;

    resources[0] = *(u32*)&lbl_8047D030;
    resources[1] = *(u32*)&lbl_8047D034;
    if (member < 0 || member >= 2) {
        return -1;
    }
    fn_8018D998(0, resources[member]);
    person = peopleSearchID();
    if (person == NULL) {
        return -1;
    }
    info = peopleInfoBiosGetPtr(*(s32*)((u8*)person + 0x30));
    fn_8018F4C8(info, 1, &anim1, query);
    fn_8018F4C8(info, 2, &anim2, query);
    fn_8018F4C8(info, 3, &anim3, query);
    fn_8018F4C8(info, 4, &anim4, query);

    if (amount > lbl_8047D084) {
        amount = lbl_8047D084;
    }
    GSmodelGetAnimIndex(model, &current_anim, &blend_anim);

    if (amount < lbl_8047D088) {
        if (current_anim != anim4 || blend_anim != -1) {
            frame = lbl_8047D038;
            if (blend_anim != -1) {
                GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
                frame = (frame_count_a / frame_count_b) *
                        GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimIndex(model, anim4);
            GSmodelSetAnimFrame(model, frame);
            GSmodelSetAnimRate(model, lbl_8047D080);
        }
    } else if (amount < lbl_8047D038) {
        if (current_anim != anim4 || blend_anim != anim1) {
            frame = lbl_8047D038;
            if (blend_anim == -1) {
                frame = GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimBlend(model, anim4, anim1);
            GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
            GSmodelSetAnimFrame(model,
                                frame * (frame_count_b / frame_count_a));
        }
        GSmodelSetBlendFactor(model,
                              (amount - lbl_8047D088) / lbl_8047D08C);
        GSmodelSetAnimRate(model, lbl_8047D040);
    } else if (amount < lbl_8047D08C) {
        if (current_anim != anim2 || blend_anim != anim1) {
            frame = lbl_8047D038;
            if (blend_anim == -1) {
                frame = GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimBlend(model, anim2, anim1);
            GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
            GSmodelSetAnimFrame(model,
                                frame * (frame_count_b / frame_count_a));
        }
        GSmodelSetBlendFactor(model,
                              lbl_8047D080 - lbl_8047D090 * amount);
        GSmodelSetAnimRate(model, lbl_8047D040);
    } else if (amount < lbl_8047D080) {
        if (current_anim != anim2 || blend_anim != -1) {
            frame = lbl_8047D038;
            if (blend_anim != -1) {
                GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
                frame = (frame_count_a / frame_count_b) *
                        GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimIndex(model, anim2);
            GSmodelSetAnimFrame(model, frame);
            GSmodelSetAnimRate(model, lbl_8047D040);
        }
    } else {
        if (current_anim != anim2 || blend_anim != anim3) {
            frame = lbl_8047D038;
            if (blend_anim == -1) {
                frame = GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimBlend(model, anim2, anim3);
            GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
            GSmodelSetAnimFrame(model,
                                frame * (frame_count_b / frame_count_a));
        }
        GSmodelSetBlendFactor(model, amount - lbl_8047D080);
        GSmodelSetAnimRate(model, lbl_8047D040);
    }
    return 0;
}
/* 0x8012D39C | 0x454 */
extern f32 lbl_8047D0A8;
extern f32 lbl_8047D038;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f64 lbl_8047D058;
extern f32 lbl_8047D080;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
extern s32 fn_8012D39C(void*, void*, void*, void*, void*, f32);
/* 0x8012D7F0 | 0x6A4 */
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f64 lbl_8047D068;
extern f32 lbl_8047D038;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f64 lbl_8047D058;
extern f32 lbl_8047D060;
extern f32 lbl_8047D0AC;
extern f32 lbl_8047D080;
typedef struct HeroMoveVec3 {
    f32 x;
    f32 y;
    f32 z;
} HeroMoveVec3;

s32 fn_8012D39C(void* start_, void* end_, void* center_, void* reference_,
                void* result_, f32 radius)
{
    extern f32 sqrtf(f32);
    HeroMoveVec3* start = start_;
    HeroMoveVec3* end = end_;
    HeroMoveVec3* center = center_;
    HeroMoveVec3* reference = reference_;
    HeroMoveVec3* result = result_;
    f32 dz;
    f32 dx;
    f32 lengthSquared;
    f32 invLength;
    f32 dirX;
    f32 dirZ;
    f32 lineOffset;
    f32 normalLengthSquared;
    f32 normalLength;
    f32 originX;
    f32 originZ;
    f32 relX;
    f32 relZ;
    f32 cross;
    f32 discriminant;
    f32 projection;
    f32 root;
    f32 scale;
    f32 nearX;
    f32 nearZ;
    f32 farX;
    f32 farZ;
    f32 nearDx;
    f32 nearDz;
    f32 farDx;
    f32 farDz;

    dz = end->z - start->z;
    dx = end->x - start->x;
    lengthSquared = dx * dx + dz * dz;
    if (lengthSquared < lbl_8047D0A8) {
        return -1;
    }

    invLength = lbl_8047D080 / sqrtf(lengthSquared);
    dirX = -dz * invLength;
    dirZ = dx * invLength;
    lineOffset = invLength *
                 (start->x * end->z - end->x * start->z);

    normalLengthSquared = dirX * dirX + dirZ * dirZ;
    scale = lbl_8047D080 / normalLengthSquared;
    originX = dirX * (-lineOffset * scale);
    originZ = dirZ * (-lineOffset * scale);
    normalLength = sqrtf(scale);
    {
        f32 temp = -dirX;
        dirX = dirZ * normalLength;
        dirZ = temp * normalLength;
    }

    if (dirX * dirX + dirZ * dirZ < lbl_8047D0A8) {
        return 0;
    }

    relX = center->x - originX;
    relZ = center->z - originZ;
    cross = dirX * relZ - dirZ * relX;
    discriminant = normalLengthSquared * radius * radius - cross * cross;
    if (discriminant < -lbl_8047D0A8) {
        return 0;
    }

    projection = dirX * relX + dirZ * relZ;
    if (discriminant < projection) {
        scale = projection / normalLengthSquared;
        result->x = dirX * scale + originX;
        result->y = lbl_8047D038;
        result->z = dirZ * scale + originZ;
        return 1;
    }

    root = sqrtf(discriminant);
    scale = lbl_8047D080 / normalLengthSquared;
    nearX = dirX * (scale * (projection - root)) + originX;
    nearZ = dirZ * (scale * (projection - root)) + originZ;
    farX = dirX * (scale * (projection + root)) + originX;
    farZ = dirZ * (scale * (projection + root)) + originZ;

    nearDx = reference->x - nearX;
    nearDz = reference->z - nearZ;
    farDx = reference->x - farX;
    farDz = reference->z - farZ;
    if (nearDx * nearDx + nearDz * nearDz <
        farDx * farDx + farDz * farDz) {
        result->x = nearX;
        result->y = lbl_8047D038;
        result->z = nearZ;
    } else {
        result->x = farX;
        result->y = lbl_8047D038;
        result->z = farZ;
    }
    return 2;
}

void fn_8012D7F0(s32 playerIndex, void* velocityOut_, void* resultOut_)
{
    extern u32 fn_800D3088(void);
    extern void* GSresGetResource(u32, u32);
    extern void GSmodelGetPosition(void*, HeroMoveVec3*);
    extern void PSVECScale(HeroMoveVec3*, HeroMoveVec3*, f32);
    extern void PSVECAdd(HeroMoveVec3*, HeroMoveVec3*, HeroMoveVec3*);
    extern void PSVECSubtract(HeroMoveVec3*, HeroMoveVec3*, HeroMoveVec3*);
    extern f32 sqrtf(f32);

    HeroMoveVec3* velocityOut = (HeroMoveVec3*)velocityOut_;
    HeroMoveVec3* resultOut = (HeroMoveVec3*)resultOut_;
    HeroMoveVec3 targetPosition;
    HeroMoveVec3 currentPosition;
    HeroMoveVec3 direction;
    HeroMoveVec3 step;
    HeroMoveVec3 historyPosition;
    HeroMoveVec3 collisionPosition;
    HeroMoveVec3 candidatePosition;
    HeroMoveVec3 projectedPosition;
    HeroMoveVec3 difference;
    u32 resourceHandles[2];
    u32 playerHandles[2];
    u32 resourceId;
    u32 playerResourceId;
    s32 historyIndex;
    u8 hasHistory;
    f32 frameDistance;
    f32 radius;
    f32 targetDistance;
    f32 historyDistance;
    f32 scale;

    radius = *(f32*)(lbl_80426BD0 + playerIndex * 0x20 + 8);
    frameDistance = (f32)fn_800D3088();

    resourceHandles[0] = *(u32*)&lbl_8047D030;
    resourceHandles[1] = *(u32*)&lbl_8047D034;
    historyIndex = *(s32*)lbl_80426BD0;
    if (historyIndex >= 0 && historyIndex < 2) {
        resourceId = resourceHandles[historyIndex];
    }
    GSmodelGetPosition(GSresGetResource(0, resourceId), &targetPosition);

    if (*(s32*)(lbl_80426BD0 + 0x48) > 0) {
        historyIndex = *(s32*)(lbl_80426BD0 + 0x44) - 1;
        if (historyIndex < 0) {
            historyIndex += 20;
        }
        historyPosition =
            *(HeroMoveVec3*)(lbl_80426BD0 + 0x4C + historyIndex * 12);
        hasHistory = 1;
    } else {
        hasHistory = 0;
    }

    playerHandles[0] = *(u32*)&lbl_8047D030;
    playerHandles[1] = *(u32*)&lbl_8047D034;
    if (playerIndex >= 0 && playerIndex < 2) {
        playerResourceId = playerHandles[playerIndex];
    }
    GSmodelGetPosition(GSresGetResource(0, playerResourceId), &currentPosition);

    direction.x = targetPosition.x - currentPosition.x;
    direction.y = lbl_8047D038;
    direction.z = targetPosition.z - currentPosition.z;
    targetDistance = sqrtf(direction.x * direction.x + direction.z * direction.z);

    if (hasHistory) {
        f32 dx = currentPosition.x - historyPosition.x;
        f32 dz = currentPosition.z - historyPosition.z;
        historyDistance = sqrtf(dx * dx + dz * dz);
    } else {
        historyDistance = lbl_8047D038;
    }

    if (!hasHistory || targetDistance <= historyDistance ||
        targetDistance <= lbl_8047D060 + radius) {
        if (targetDistance > lbl_8047D060 + radius) {
            f32 amount = targetDistance - radius;
            if (amount > frameDistance) {
                amount = frameDistance;
            }
            scale = amount / targetDistance;
        } else if (targetDistance < lbl_8047D0AC) {
            f32 amount = lbl_8047D0AC - targetDistance;
            if (amount > frameDistance) {
                amount = frameDistance;
            }
            scale = -amount / lbl_8047D0AC;
        } else {
            scale = lbl_8047D038;
        }

        PSVECScale(&direction, velocityOut, scale);
        *resultOut = *velocityOut;
        return;
    }

    direction.x = historyPosition.x - currentPosition.x;
    direction.y = lbl_8047D038;
    direction.z = historyPosition.z - currentPosition.z;
    historyDistance = sqrtf(direction.x * direction.x + direction.z * direction.z);
    if (historyDistance > frameDistance) {
        PSVECScale(&direction, &step, frameDistance / historyDistance);
    } else {
        step = direction;
    }

    PSVECAdd(&currentPosition, &step, &candidatePosition);
    if (fn_8012D39C(&currentPosition, &candidatePosition, &targetPosition,
                    &currentPosition, &collisionPosition, radius) > 0) {
        f32 magnitudeSquared;

        candidatePosition.y = currentPosition.y;
        PSVECSubtract(&candidatePosition, &currentPosition, &difference);
        magnitudeSquared = difference.x * difference.x +
                           difference.y * difference.y +
                           difference.z * difference.z;
        if (magnitudeSquared == lbl_8047D038) {
            projectedPosition = candidatePosition;
        } else {
            f32 projection =
                (difference.x * (collisionPosition.x - currentPosition.x) +
                 difference.y * (collisionPosition.y - currentPosition.y) +
                 difference.z * (collisionPosition.z - currentPosition.z)) /
                magnitudeSquared;
            PSVECScale(&difference, &difference, projection);
            PSVECAdd(&difference, &currentPosition, &projectedPosition);
            if (projection >= lbl_8047D038 && projection <= lbl_8047D080) {
                PSVECSubtract(&collisionPosition, &currentPosition, &step);
            }
        }
    }

    *velocityOut = step;
    *resultOut = step;
}
/* 0x8012DE94 | 0x4F4 */
extern void fn_800E3C64(void);
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f32 lbl_8047D038;
extern f64 lbl_8047D058;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f32 lbl_8047D0B0;
void fn_8012DE94(u32 playerIndex)
{
    extern u32 fn_800D3088();
    extern u8 fn_800E3C64();
    extern void fn_8018C0A8();
    extern void PSVECSubtract(HeroMoveVec3*, HeroMoveVec3*, HeroMoveVec3*);
    extern f32 sqrtf(f32);

    HeroMoveVec3 targetPosition;
    HeroMoveVec3 playerPosition;
    HeroMoveVec3 velocity;
    HeroMoveVec3 movement;
    HeroMoveVec3 separation;
    HeroMoveVec3 collisionPosition;
    HeroMoveVec3 modelPosition;
    u32 resourceHandles[2];
    u32 resourceHandle;
    void* resource;
    s32 activePlayer;
    f32 distanceSquared;
    f32 distance;
    s32 historyIndex;
    s32 historyOffset;
    u32 historyCount;
    u32 historyHead;
    u32 i;
    u8 blocked;
    u8 haveHistory;

    resourceHandles[0] = *(u32*)&lbl_8047D030;
    resourceHandles[1] = *(u32*)&lbl_8047D034;
    activePlayer = *(s32*)lbl_80426BD0;
    if (activePlayer >= 0 && activePlayer < 2) {
        resourceHandle = resourceHandles[activePlayer];
    }
    resource = GSresGetResource(0, resourceHandle);
    GSmodelGetPosition(resource, &targetPosition);

    resourceHandles[0] = *(u32*)&lbl_8047D030;
    resourceHandles[1] = *(u32*)&lbl_8047D034;
    if ((s32)playerIndex >= 0 && playerIndex < 2) {
        resourceHandle = resourceHandles[playerIndex];
    }
    resource = GSresGetResource(0, resourceHandle);
    GSmodelGetPosition(resource, &playerPosition);

    separation.x = targetPosition.x - playerPosition.x;
    separation.z = targetPosition.z - playerPosition.z;
    distanceSquared =
        separation.x * separation.x + separation.z * separation.z;
    sqrtf(distanceSquared);

    fn_8012D7F0(playerIndex, &velocity, &movement);
    fn_8012CA84(playerIndex, &velocity, &movement);

    resourceHandles[0] = *(u32*)&lbl_8047D030;
    resourceHandles[1] = *(u32*)&lbl_8047D034;
    if (playerIndex < 2) {
        resourceHandle = resourceHandles[playerIndex];
    }
    resource = GSresGetResource(0, resourceHandle);
    GSmodelGetPosition(resource, &modelPosition);

    separation.x = targetPosition.x - modelPosition.x;
    separation.z = targetPosition.z - modelPosition.z;
    distanceSquared =
        separation.x * separation.x + separation.z * separation.z;
    distance = sqrtf(distanceSquared);
    PSVECSubtract(&modelPosition, &playerPosition, &separation);

    if (distance < lbl_8047D0B0) {
        *(u32*)(lbl_80426BD0 + playerIndex * 0x20 + 0x10) = 300;
        return;
    }

    *(s32*)(lbl_80426BD0 + playerIndex * 0x20 + 0x10) -=
        (s32)fn_800D3088();
    if (*(s32*)(lbl_80426BD0 + playerIndex * 0x20 + 0x10) > 0) {
        return;
    }

    resource = GSresGetResource(0, resourceHandle);
    if (resource == NULL || fn_800E3C64(resource) != 0) {
        return;
    }

    blocked = 0;
    i = 0;
    historyCount = *(u32*)(lbl_80426BD0 + 0x48);
    historyHead = *(u32*)(lbl_80426BD0 + 0x44);
    do {
        if (i >= historyCount || i >= 20) {
            haveHistory = 0;
        } else {
            historyIndex = (s32)historyHead - (s32)i - 1;
            if (historyIndex < 0) {
                historyIndex += 20;
            }
            historyOffset = historyIndex * 12;
            collisionPosition =
                *(HeroMoveVec3*)(lbl_80426BD0 + 0x4C + historyOffset);
            haveHistory = 1;
        }

        if (!haveHistory || blocked) {
            break;
        }

        resourceHandles[0] = *(u32*)&lbl_8047D030;
        resourceHandles[1] = *(u32*)&lbl_8047D034;
        if (playerIndex < 2) {
            resourceHandle = resourceHandles[playerIndex];
        }
        resource = GSresGetResource(0, resourceHandle);
        if (resource == NULL) {
            blocked = 1;
        } else {
            resource = GSresGetResource(0, resourceHandle);
            GSmodelGetPosition(resource, &modelPosition);
            fn_8018C0A8(0, resourceHandle, &collisionPosition);
            resource = GSresGetResource(0, resourceHandle);
            if (fn_800E3C64(resource) == 0) {
                blocked = 1;
            }
            fn_8018C0A8(0, resourceHandle, &modelPosition);
        }
        i++;
    } while (1);

    if (blocked) {
        fn_8018C0A8(0, resourceHandle, &collisionPosition);
        *(u32*)(lbl_80426BD0 + playerIndex * 0x20 + 0x10) = 300;
    }
}
/* 0x8012E388 | 0x430 */
extern void fn_800F7A7C(void);
extern void fn_800F7A08(void);
extern void fn_800F7BC4(void);
extern void fn_80188214(void);
extern void fn_80166458(void);
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f64 lbl_8047D0C8;
extern f32 lbl_8047D0B4;
extern f32 lbl_8047D038;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f64 lbl_8047D058;
extern f32 lbl_8047D084;
extern f32 lbl_8047D0B8;
extern f32 lbl_8047D0BC;
extern f32 lbl_8047D078;
extern f32 lbl_8047D0C0;
extern f32 lbl_8047D0C4;
void fn_8012E388(s32 playerIndex, f32* magnitudeOut)
{
    extern void* GSresGetResource(u32, u32);
    extern s32 fn_800F7A7C(u32, u32);
    extern s32 fn_800F7A08(u32, u32);
    extern u32 fn_800F7BC4(u32);
    extern u32 GSscene_GetMode(void);
    extern f64 sin(f32);
    extern f32 sqrtf(f32);
    extern f32 cameraGetRotY(void);
    extern void fn_8018805C(u32, u32, f32, f32);
    extern void fn_80188214(u32, u32, f32);
    extern void fn_8018790C(u32, u32);
    extern void fn_80166458(void*, void*);

    u32 resources[2];
    u32 modelHandle;
    s32 stickX;
    s32 stickY;
    s32 subX;
    s32 subY;
    f32 x;
    f32 y;
    f32 magnitude;
    f32 anglePart;
    f32 angle;
    u8 scratch[12];

    GSresGetResource(0, 2);
    resources[0] = *(u32*)&lbl_8047D030;
    resources[1] = *(u32*)&lbl_8047D034;
    modelHandle = resources[playerIndex];
    GSresGetResource(0, resources[playerIndex]);

    stickX = fn_800F7A7C(1, 1);
    stickY = fn_800F7A08(1, 1);
    subX = fn_800F7A7C(1, 0);
    subY = fn_800F7A08(1, 0);
    if ((s8)stickX == 0 && (s8)stickY == 0) {
        u32 buttons;
        buttons = fn_800F7BC4(1);
        if (buttons & 8) stickY = -0x38;
        buttons = fn_800F7BC4(1);
        if (buttons & 4) stickY = 0x38;
        buttons = fn_800F7BC4(1);
        if (buttons & 1) stickX = -0x38;
        buttons = fn_800F7BC4(1);
        if (buttons & 2) stickX = 0x38;
        subX = stickX;
        subY = stickY;
        if ((s8)stickX == 0 && (s8)stickY == 0) {
            GSscene_GetMode();
        }
    }

    if ((s8)stickX == 0 && (s8)stickY == 0) {
        *magnitudeOut = lbl_8047D038;
        fn_8018790C(0, modelHandle);
        return;
    }

    if ((s8)stickX > 0x38) stickX = 0x38;
    if ((s8)stickX < -0x38) stickX = -0x38;
    if ((s8)stickY > 0x38) stickY = 0x38;
    if ((s8)stickY < -0x38) stickY = -0x38;
    x = (f32)((s8)stickX < 0 ? -(s8)stickX : (s8)stickX) / lbl_8047D0B4;
    y = (f32)((s8)stickY < 0 ? -(s8)stickY : (s8)stickY) / lbl_8047D0B4;
    magnitude = sqrtf(x * x + y * y);
    *magnitudeOut = magnitude;
    if (*magnitudeOut > lbl_8047D084) {
        *magnitudeOut = lbl_8047D084;
    }

    if ((s8)subX > -2 && (s8)subX < 2 &&
        (s8)subY > -2 && (s8)subY < 2) {
        if (y < lbl_8047D0B8) {
            anglePart = lbl_8047D0BC;
        } else {
            f32 ratio = x / y;
            if (ratio > lbl_8047D078) ratio = lbl_8047D078;
            anglePart = lbl_8047D0BC *
                        (f32)sin(ratio / lbl_8047D0C0);
        }
        if ((s8)stickY >= 0) {
            angle = anglePart;
        } else {
            angle = lbl_8047D0C4 - anglePart;
        }
        if ((s8)stickX < 0) {
            if ((s8)stickY >= 0) {
                angle = lbl_8047D0C4 + (lbl_8047D0C4 - anglePart);
            } else {
                angle = lbl_8047D0C4 + anglePart;
            }
        }
        fn_8018805C(0, modelHandle, cameraGetRotY() + angle,
                    *magnitudeOut);
    }
    fn_80188214(0, modelHandle, *magnitudeOut);
    fn_80166458(GSresGetResource(0, 0x7D0), scratch);
}
/* 0x8012E7B8 | 0x41C */
extern void fn_800F7AF0(void);
extern void fn_801887D8(void);
extern void PSVECDistance(void);
extern f32 lbl_8047D038;
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f64 lbl_8047D068;
extern f32 lbl_8047D080;
extern f32 lbl_8047D0D0;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f64 lbl_8047D058;
extern f32 lbl_8047D0D4;
f32 moveLeader__F15HEROMOVE_MEMBER(member)
s32 member;
{
    extern u32 fn_800F7AF0(s32);
    extern u32 fn_800F7BC4(s32);
    extern u32 GSresGetResource(u32, u32);
    extern void GSmodelGetPosition(void*, HeroMoveVec3*);
    extern void PSVECSubtract(HeroMoveVec3*, HeroMoveVec3*, HeroMoveVec3*);
    extern void PSVECScale(HeroMoveVec3*, HeroMoveVec3*, f32);
    extern f32 PSVECDistance(HeroMoveVec3*, HeroMoveVec3*);
    extern f32 fn_801887D8(u32, u32, HeroMoveVec3*);
    extern u32 fn_800D3088(void);
    extern f32 sqrtf(f32);
    u32 resources[2];
    HeroMoveVec3 before;
    HeroMoveVec3 after;
    HeroMoveVec3 direction;
    HeroMoveVec3 active_position;
    HeroMoveVec3 previous_position;
    f32 input[2];
    u32 handle;
    u32 active_handle;
    u32 history_head;
    u32 history_count;
    s32 history_index;
    f32 turn;
    f32 distance_squared;
    f32 distance;
    u8 record_position;

    if ((fn_800F7AF0(1) & fn_800F7BC4(1) & 0x100) != 0 &&
        updateChat__F15HEROMOVE_MEMBER(member) != 0) {
        return lbl_8047D038;
    }

    resources[0] = *(u32*)&lbl_8047D030;
    resources[1] = *(u32*)&lbl_8047D034;
    if (member >= 0 && member < 2) {
        handle = resources[member];
    }
    GSmodelGetPosition((void*)GSresGetResource(0, handle), &before);
    fn_8012E388(member, input);

    resources[0] = *(u32*)&lbl_8047D030;
    resources[1] = *(u32*)&lbl_8047D034;
    if (member >= 0 && member < 2) {
        handle = resources[member];
    }
    GSmodelGetPosition((void*)GSresGetResource(0, handle), &after);
    PSVECSubtract(&after, &before, &direction);
    PSVECScale(&direction, &direction,
               lbl_8047D080 / (f32)fn_800D3088());

    resources[0] = *(u32*)&lbl_8047D030;
    resources[1] = *(u32*)&lbl_8047D034;
    if (member >= 0 && member < 2) {
        handle = resources[member];
    }
    turn = fn_801887D8(0, handle, &direction);
    if (turn < lbl_8047D0D0) {
        if (input[1] >= lbl_8047D0D0) {
            turn = lbl_8047D0D0;
        } else {
            turn = input[1];
        }
    }

    resources[0] = *(u32*)&lbl_8047D030;
    resources[1] = *(u32*)&lbl_8047D034;
    if (member >= 0 && member < 2) {
        handle = resources[member];
    }
    updateAnimation__Ff15HEROMOVE_MEMBER(
        (void*)GSresGetResource(0, handle), member, turn);

    resources[0] = *(u32*)&lbl_8047D030;
    resources[1] = *(u32*)&lbl_8047D034;
    active_handle = 0;
    if (*(s32*)lbl_80426BD0 >= 0 && *(s32*)lbl_80426BD0 < 2) {
        active_handle = resources[*(s32*)lbl_80426BD0];
    }
    GSmodelGetPosition((void*)GSresGetResource(0, active_handle),
                       &active_position);

    history_count = *(u32*)(lbl_80426BD0 + 0x48);
    if (history_count <= 0) {
        record_position = TRUE;
    } else {
        history_index = *(s32*)(lbl_80426BD0 + 0x44) - 1;
        if (history_index < 0) {
            history_index += 20;
        }
        previous_position = *(HeroMoveVec3*)(lbl_80426BD0 + 0x4C +
                                              history_index * 12);
        distance_squared =
            (previous_position.x - active_position.x) *
                (previous_position.x - active_position.x) +
            (previous_position.z - active_position.z) *
                (previous_position.z - active_position.z);
        distance = distance_squared > lbl_8047D038
                       ? sqrtf(distance_squared)
                       : lbl_8047D038;
        if (distance != distance) {
            distance = *(f32*)lbl_80478AC0;
        }
        record_position = distance > lbl_8047D0D4;
    }

    if (record_position) {
        history_head = *(u32*)(lbl_80426BD0 + 0x44);
        *(HeroMoveVec3*)(lbl_80426BD0 + 0x4C + history_head * 12) =
            active_position;
        history_head++;
        if (history_head >= 20) {
            history_head = 0;
        }
        *(u32*)(lbl_80426BD0 + 0x44) = history_head;
        if (*(u32*)(lbl_80426BD0 + 0x48) < 20) {
            (*(u32*)(lbl_80426BD0 + 0x48))++;
        }
    }

    return PSVECDistance(&before, &after);
}

#if 0
asm void heroMoveSetNeckMode(void) {
#include "src/game/gs_field_world_fn_8012F008.inc"
}
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
#else
u32 heroMoveSetNeckMode(s32 idx, s32 state)
{
    u32 values[2];
    u32 value;
    u8* entry;
    u32 valid;

    if (state < 0 || state >= 2) {
        return 0;
    }

    if (idx < 0 || idx >= 2) {
        valid = 0;
    } else {
        if ((*(u16*)(lbl_80426BD0 + idx * 0x20 + 4) & 1) == 0) {
            valid = 0;
        } else {
            valid = 1;
        }
    }
    if ((valid & 0xFF) == 0) {
        return 0;
    }

    values[0] = *(u32*)&lbl_8047D030;
    values[1] = *(u32*)&lbl_8047D034;
    if (idx >= 0 && idx < 2) {
        value = values[idx];
    }

    entry = lbl_80426BD0 + idx * 0x20;
    if (*(s32*)(entry + 0xC) == 1) {
        fn_80188AF4(0, value);
    }
    if (state == 1) {
        fn_80188F78(0, value);
    }
    *(u32*)(entry + 0xC) = state;
    return 1;
}
#endif
/* 0x8012F11C | 0x34 */
u32 heroMoveIsMember(s32 idx) {
    u8* ptr;
    u16 val;
    if (idx < 0 || idx >= 2) { return 0; }
    ptr = (u8*)lbl_80426BD0;
    ptr += (u32)idx * 32;
    val = *(u16*)(ptr + 4);
    return (u32)(val & 1);
}
/* 0x8012F150 | 0xAC */
extern f32 lbl_8047D038;
extern f32 lbl_8047D0D4;
#if 0
asm void heroMoveDismissMember(void) {
#include "src/game/gs_field_world_fn_8012F150.inc"
}
#else
s32 heroMoveDismissMember(s32 idx) {
    f32 f1, f2;
    s32 i;
    if (idx < 0 || idx >= 2) return 0;
    if (idx == (s32)*(u32*)lbl_80426BD0) return 0;
    f1 = lbl_8047D038;
    *(u16*)(lbl_80426BD0 + ((u32)idx << 5) + 4) &= 0xFFFE;
    f2 = lbl_8047D0D4;
    *(f32*)(lbl_80426BD0 + (*(u32*)lbl_80426BD0 << 5) + 8) = f1;
    for (i = 0; i < 2; i++) {
        if (*(u16*)(lbl_80426BD0 + (u32)i * 0x20 + 4) & 1) {
            if ((s32)*(u32*)lbl_80426BD0 != i) {
                *(f32*)(lbl_80426BD0 + (u32)i * 0x20 + 8) = f2;
                f2 = lbl_8047D0D4;
                f2 = f2 + f2;
            }
        }
    }
    return 1;
}
#endif
extern void fn_801885C4(void);
extern void PSVECDotProduct(void);
extern void fn_8018F678(void);
extern void fn_8018F658(void);
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
extern f64 lbl_8047D068;
extern f32 lbl_8047D038;
extern f64 lbl_8047D048;
extern f64 lbl_8047D050;
extern f64 lbl_8047D058;
extern f32 lbl_8047D080;
extern f32 lbl_8047D060;
extern f32 lbl_8047D094;
extern f32 lbl_8047D098;
extern f32 lbl_8047D09C;
extern f32 lbl_8047D0A0;
extern f32 lbl_8047D0A4;
#if 0
asm void fn_8012CA84(void) {
#include "src/game/gs_field_world_fn_8012CA84.inc"
}
#else
/* FUNCTIONAL decomp of fn_8012CA84
 * Field movement/heading processor - computes turn amount from direction input. */
void fn_8012CA84(s32 playerIdx, f32* dirVec, f32* fwdVec) {
    extern u8 lbl_80478AC0[4];       /* sdata constant 0.0f */
    extern f32 lbl_8047D030;        /* handle pair [0] (read as u32 bits) */
    extern f32 lbl_8047D034;        /* handle pair [1] (read as u32 bits) */
    extern f32 lbl_8047D038;        /* 0.0f */
    extern f32 lbl_8047D060;        /* small-magnitude threshold */
    extern f32 lbl_8047D080;        /* 1.0f */
    extern f32 lbl_8047D094;        /* TWO_PI */
    extern f32 lbl_8047D098;        /* negative angle-wrap bound */
    extern f32 lbl_8047D09C;        /* -PI */
    extern f32 lbl_8047D0A0;        /* PI */
    extern f32 lbl_8047D0A4;        /* minimum turn threshold */
    extern u32  fn_800D3088(void);                        /* frame count */
    extern void PSVECSubtract(void*, void*, void*);         /* VECSubtract(a, b, out) */
    extern void PSVECScale(void*, void*, f32);           /* VECScale(in, out, s) */
    extern f32  PSVECDotProduct(void*, void*);                /* VECDotProduct */
    extern f64  atan2(f32, f32);                    /* atan2f(y, x) */
    extern void GSmodelGetRotation(void*, void*);                /* getRotation(obj, out) */
    extern void GSmodelGetPosition(void*, void*);                /* getPosition(obj, out) */
    extern void* GSresGetResource(u32, u32);                   /* resolveHandle(group, id) */
    extern void updateAnimation__Ff15HEROMOVE_MEMBER(void*, s32, f32);             /* applyTurnResult(obj, idx, amt) */
    extern void fn_8018790C(u32, u32);                    /* stopMovement(group, handle) */
    extern void fn_8018805C(u32, u32, f32, f32);          /* setHeading(grp, hdl, angle, spd) */
    extern void fn_801885C4(u32, u32, f32*, u32);         /* setMoveDirection(grp, hdl, dir, flags) */
    extern f32  fn_801887D8(u32, u32, f32*);              /* computeTurnAmount(grp, hdl, dir) */
    extern void fn_8018D998(u32, u32);                    /* selectEntity(grp, hdl) */
    extern void* peopleSearchID(void);                       /* getEntityData() */
    extern void* peopleInfoBiosGetPtr(s32);                        /* getAngleConfig(param) */
    extern f32  fn_8018F678(void*);                       /* getMaxTurnRatePos(obj) */
    extern f32  fn_8018F658(void*);                       /* getMaxTurnRateNeg(obj) */
    extern u8   lbl_80426BD0[];                           /* player state array (stride 0x20) */
    extern f32  sqrtf(f32);                               /* host CRT */

    u32 htbl[2];
    u32 entityHandle = 0;
    u32 finalHandle  = 0;
    f32 turnAmount   = 0.0f;
    f32 frameTime;
    f32 dirMag;
    void* obj;
    f32 posA[3];
    f32 posB[3];
    f32 diffVec[3];
    f32 scaledDir[3];
    f32 targetPos[3];
    f32 playerPos[3];
    f32 rotation[3];

    frameTime = (f32)(u32)fn_800D3088();

    htbl[0] = *(u32*)&lbl_8047D030;
    htbl[1] = *(u32*)&lbl_8047D034;
    if (playerIdx >= 0 && playerIdx < 2) {
        entityHandle = htbl[playerIdx];
    }

    {
        f32 sq = dirVec[0] * dirVec[0] + dirVec[2] * dirVec[2];
        /* original is inline frsqrte + 3x Newton-Raphson; sqrtf is x86-equivalent */
        dirMag = (sq > 0.0f) ? sqrtf(sq) : 0.0f;
    }

    if (dirMag > lbl_8047D038) {
        /* ======== MOVING: direction-based heading ======== */
        f32 clampedSpeed;
        u32 h = 0;

        htbl[0] = *(u32*)&lbl_8047D030;
        htbl[1] = *(u32*)&lbl_8047D034;
        if (playerIdx >= 0 && playerIdx < 2) {
            h = htbl[playerIdx];
        }
        obj = GSresGetResource(0, h);
        GSmodelGetPosition(obj, posA);
        posA[1] = lbl_8047D038;

        fn_801885C4(0, entityHandle, dirVec, 0);

        h = 0;
        htbl[0] = *(u32*)&lbl_8047D030;
        htbl[1] = *(u32*)&lbl_8047D034;
        if (playerIdx >= 0 && playerIdx < 2) {
            h = htbl[playerIdx];
        }
        obj = GSresGetResource(0, h);
        GSmodelGetPosition(obj, posB);
        posB[1] = lbl_8047D038;

        PSVECSubtract(posB, posA, diffVec);

        clampedSpeed = dirMag / frameTime;
        if (clampedSpeed > lbl_8047D080) {
            clampedSpeed = lbl_8047D080;
        }

        PSVECScale(diffVec, scaledDir, clampedSpeed / dirMag);

        {
            f32 fwdSq = fwdVec[0] * fwdVec[0] + fwdVec[2] * fwdVec[2];
            f32 fwdMag = (fwdSq > 0.0f) ? sqrtf(fwdSq) : 0.0f;

            if (fwdMag > lbl_8047D060) {
                f32 angle = (f32)atan2(fwdVec[0], fwdVec[2]);
                fn_8018805C(0, entityHandle, angle, clampedSpeed);
            }
        }

        turnAmount = fn_801887D8(0, entityHandle, scaledDir);

        if (PSVECDotProduct(diffVec, fwdVec) < lbl_8047D038) {
            turnAmount = -turnAmount;
        }

    } else {
        /* ======== STATIONARY: face-target or idle ======== */
        u8 hasTarget = 0;
        u32 mode;

        if (playerIdx >= 0 && playerIdx < 2) {
            u16 flags = *(u16*)(&lbl_80426BD0[playerIdx * 0x20] + 4);
            hasTarget = (u8)(flags & 1);
        }

        mode = hasTarget
             ? *(u32*)(&lbl_80426BD0[playerIdx * 0x20] + 0xC)
             : 2u;

        if (mode != 1) {
            fn_8018790C(0, entityHandle);
            turnAmount = lbl_8047D038;
        } else {
            /* ---- FACE-TARGET: rotate toward another entity ---- */
            s32 targetIdx;
            u32 tgtH = 0, plrH = 0, rotH = 0, turnH = 0;
            f32 heading, targetAngle, angleDiff;
            f32 newAngle = 0.0f;
            f32 maxPos, maxNeg;
            void* angleObj;
            void* tPtr;
            s32 turnParam;
            u8 shouldTurn = 0;
            u8 turnValid  = 0;

            targetIdx = *(s32*)lbl_80426BD0;
            htbl[0] = *(u32*)&lbl_8047D030;
            htbl[1] = *(u32*)&lbl_8047D034;
            if (targetIdx >= 0 && targetIdx < 2) {
                tgtH = htbl[targetIdx];
            }
            obj = GSresGetResource(0, tgtH);
            GSmodelGetPosition(obj, targetPos);

            htbl[0] = *(u32*)&lbl_8047D030;
            htbl[1] = *(u32*)&lbl_8047D034;
            if (playerIdx >= 0 && playerIdx < 2) {
                plrH = htbl[playerIdx];
            }
            obj = GSresGetResource(0, plrH);
            GSmodelGetPosition(obj, playerPos);

            htbl[0] = *(u32*)&lbl_8047D030;
            htbl[1] = *(u32*)&lbl_8047D034;
            if (playerIdx >= 0 && playerIdx < 2) {
                rotH = htbl[playerIdx];
            }
            obj = GSresGetResource(0, rotH);
            GSmodelGetRotation(obj, rotation);

            heading = rotation[1];
            while (heading >= lbl_8047D094) { heading -= lbl_8047D094; }
            while (heading <= lbl_8047D098) { heading += lbl_8047D094; }
            rotation[1] = heading;

            targetAngle = (f32)atan2(
                targetPos[0] - playerPos[0],
                targetPos[2] - playerPos[2]
            );

            angleDiff = targetAngle - heading;
            if (angleDiff < lbl_8047D09C) {
                angleDiff += lbl_8047D094;
            } else if (angleDiff > lbl_8047D0A0) {
                angleDiff -= lbl_8047D094;
            }

            htbl[0] = *(u32*)&lbl_8047D030;
            htbl[1] = *(u32*)&lbl_8047D034;
            if (playerIdx >= 0 && playerIdx < 2) {
                turnH = htbl[playerIdx];
                turnValid = 1;
            }

            if (!turnValid) {
                turnParam = -1;
            } else {
                fn_8018D998(0, turnH);
                tPtr = peopleSearchID();
                if (tPtr != NULL) {
                    turnParam = *(s32*)((u8*)tPtr + 0x30);
                } else {
                    turnParam = -1;
                }
            }

            angleObj = peopleInfoBiosGetPtr(turnParam);

            {
                f32 r = fn_8018F678(angleObj);
                maxPos = (r > lbl_8047D038)
                       ? fn_8018F678(angleObj)
                       : -fn_8018F678(angleObj);
            }

            {
                f32 r = fn_8018F658(angleObj);
                maxNeg = (r > lbl_8047D038)
                       ? fn_8018F658(angleObj)
                       : -fn_8018F658(angleObj);
            }

            if (angleDiff < lbl_8047D038) {
                f32 absDiff = -angleDiff;
                if (absDiff > maxNeg) {
                    newAngle = targetAngle + maxNeg;
                    if (newAngle >= lbl_8047D094) {
                        newAngle -= lbl_8047D094;
                    }
                    shouldTurn = 1;
                }
            } else {
                if (angleDiff > maxPos) {
                    f32 adj = targetAngle - maxPos;
                    if (adj < lbl_8047D09C) {
                        adj += lbl_8047D094;
                    } else if (adj > lbl_8047D0A0) {
                        adj -= lbl_8047D094;
                    }
                    newAngle = adj;
                    shouldTurn = 1;
                }
            }

            if (shouldTurn) {
                f32 delta   = newAngle - heading;
                f32 wrapped = delta;
                f32 magnitude;

                if (delta < lbl_8047D09C) {
                    wrapped = delta + lbl_8047D094;
                } else if (delta > lbl_8047D0A0) {
                    wrapped = delta - lbl_8047D094;
                }

                if (wrapped > lbl_8047D038) {
                    if (delta < lbl_8047D09C) {
                        magnitude = delta + lbl_8047D094;
                    } else if (delta > lbl_8047D0A0) {
                        magnitude = delta - lbl_8047D094;
                    } else {
                        magnitude = delta;
                    }
                } else {
                    if (delta < lbl_8047D09C) {
                        delta += lbl_8047D094;
                    } else if (delta > lbl_8047D0A0) {
                        delta -= lbl_8047D094;
                    }
                    magnitude = -delta;
                }

                if (magnitude < lbl_8047D0A4) {
                    newAngle   = heading;
                    shouldTurn = 0;
                }
            }

            if (shouldTurn) {
                fn_8018805C(0, entityHandle, newAngle, lbl_8047D080);
                turnAmount = lbl_8047D080;
            } else {
                fn_8018790C(0, entityHandle);
                turnAmount = lbl_8047D038;
            }
        }
    }

    htbl[0] = *(u32*)&lbl_8047D030;
    htbl[1] = *(u32*)&lbl_8047D034;
    if (playerIdx >= 0 && playerIdx < 2) {
        finalHandle = htbl[playerIdx];
    }
    obj = GSresGetResource(0, finalHandle);
    updateAnimation__Ff15HEROMOVE_MEMBER(obj, playerIdx, turnAmount);
}
#endif
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
#if 0
asm void heroMoveGetHeroRot(void) {
#include "src/game/gs_field_world_fn_8012D2BC.inc"
}
#else
void heroMoveGetHeroRot(u32 param) {
    extern u8 lbl_80426BD0[];
    extern void* GSresGetResource(u32 a, u32 b);
    extern void GSmodelGetRotation(void* a, u32 b);
    s32 idx;
    u32 table[2];
    u32 val;
    void* result;

    idx = *(s32*)lbl_80426BD0;
    table[0] = *(u32*)&lbl_8047D030;
    table[1] = *(u32*)&lbl_8047D034;
    if (idx >= 0 && idx < 2) {
        val = table[idx];
    }
    result = GSresGetResource(0, val);
    GSmodelGetRotation(result, param);
}
#endif
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
#if 0
asm void heroMoveGetHeroPos(void) {
#include "src/game/gs_field_world_fn_8012D32C.inc"
}
#else
void heroMoveGetHeroPos(u32 param) {
    extern u8 lbl_80426BD0[];
    extern void* GSresGetResource(u32 a, u32 b);
    extern void GSmodelGetPosition(void* a, u32 b);
    s32 idx;
    u32 table[2];
    u32 val;
    void* result;

    idx = *(s32*)lbl_80426BD0;
    table[0] = *(u32*)&lbl_8047D030;
    table[1] = *(u32*)&lbl_8047D034;
    if (idx >= 0 && idx < 2) {
        val = table[idx];
    }
    result = GSresGetResource(0, val);
    GSmodelGetPosition(result, param);
}
#endif
extern f32 lbl_8047D030;
extern f32 lbl_8047D034;
#if 0
asm void heroMoveGetResID(void) {
#include "src/game/gs_field_world_fn_8012EFB8.inc"
}
#else
u32 heroMoveGetResID(u32* out_zero, u32* out_val, s32 index) {
    u32 local[2];
    local[0] = *(u32*)&lbl_8047D030;
    local[1] = *(u32*)&lbl_8047D034;
    if (index < 0 || index >= 2) { return 0; }
    *out_zero = 0;
    *out_val = local[index];
    return 1;
}

typedef struct HeroMoveMemberState {
    s32 field_00;
    u16 flags;
    u16 field_06;
    f32 spacing;
    s32 neckMode;
    u8 field_10[0x10];
} HeroMoveMemberState;

s32 fn_8012F1FC(s32 member)
{
    HeroMoveMemberState* state;

    if (member < 0 || member >= 2) {
        return 0;
    }

    state = (HeroMoveMemberState*)lbl_80426BD0 + member;
    if (state->flags & 1) {
        return 1;
    }

    state->flags |= 1;
    state->spacing = lbl_8047D038;
    return 1;
}

s32 fn_8012F40C(s32 member)
{
    HeroMoveMemberState* state;

    if (member < 0 || member >= 2) {
        return 0;
    }

    state = (HeroMoveMemberState*)lbl_80426BD0 + member;
    if (!(state->flags & 1)) {
        return 0;
    }

    *(s32*)lbl_80426BD0 = member;
    state->spacing = lbl_8047D038;
    return 1;
}

u32 heroMoveGetKenObjID(void)
{
    const u32* floorIds = (const u32*)lbl_802729C0;
    const u32* objectIds = (const u32*)lbl_80272A10;
    u32 floorId;
    s32 variant;
    s32 i;

    if (fn_801906A0(0x8AE) == 0) {
        return 0x00F70400;
    }

    floorId = floorGetNextFloorID();
    for (i = 0; i < 20; i++) {
        if (floorIds[i] == floorId) {
            break;
        }
    }
    if (i == 20) {
        return 0x00F70400;
    }

    variant = fn_8006AE18();
    for (i = 0; i < 5; i++) {
        if ((s32)objectIds[i * 2] == variant) {
            return objectIds[i * 2 + 1];
        }
    }
    return floorIds[1];
}

void heroMoveSyncWithHero(void)
{
    HeroMoveMemberState* follower;

    if (fn_801906A0(0x8AE) != 0) {
        return;
    }

    follower = (HeroMoveMemberState*)lbl_80426BD0 + 1;
    if (!(follower->flags & 1)) {
        follower->flags |= 1;
        follower->spacing = lbl_8047D038;
    }
}

/* 0x8012F610 | 0x4C8 */
void initFloor__Fv(void) {
    extern u8 lbl_80426BD0[];
    extern u8 fn_800FF548(void);
    extern void* GSresGetResource(u32 group, u32 handle);
    extern void GSmodelGetPosition(void* model, void* out);
    extern void GSmodelGetRotation(void* model, void* out);
    extern void GSmodelSetRotation(void* model, void* rot);
    extern f64 sin(f64 x);
    extern f64 cos(f64 x);
    extern s32 fn_8010E138(void* pos, void* out);
    extern void fn_8018C0A8(u32 group, u32 handle, void* pos);
    extern void fn_8018C1E8(u32 group, u32 handle);
    extern void fn_80188AF4(u32 group, u32 handle);
    extern void fn_80188F78(u32 group, u32 handle);
    extern f32 lbl_8047D07C;
    extern f32 lbl_8047D0AC;
    extern f32 lbl_8047D0D8;

    u32 handles[2];
    u32 handle;
    void* model;
    u8* member;
    s32 active;
    s32 i;
    s32 count;
    s32 valid;
    s32 state;
    f32 activePos[3];
    f32 rotation[3];
    f32 target[3];
    f32 floorHits[9];
    f32 spacing;
    f32 sinY;
    f32 cosY;
    f32 best;
    f32 fallback;
    f32 y;
    s32 found;

    if (fn_800FF548() == 0) {
        active = *(s32*)lbl_80426BD0;
        handle = 0;
        handles[0] = *(u32*)&lbl_8047D030;
        handles[1] = *(u32*)&lbl_8047D034;
        if (active >= 0 && active < 2) {
            handle = handles[active];
        }
        model = GSresGetResource(0, handle);
        GSmodelGetPosition(model, activePos);

        handle = 0;
        handles[0] = *(u32*)&lbl_8047D030;
        handles[1] = *(u32*)&lbl_8047D034;
        if (active >= 0 && active < 2) {
            handle = handles[active];
        }
        model = GSresGetResource(0, handle);
        GSmodelGetRotation(model, rotation);

        sinY = -(f32)sin(rotation[1]);
        cosY = -(f32)cos(rotation[1]);
        spacing = lbl_8047D0AC;
        member = lbl_80426BD0;

        for (i = 0; i < 2; i++, member += 0x20) {
            valid = 0;
            if (i >= 0 && i < 2 && (*(u16*)(member + 4) & 1) != 0) {
                valid = 1;
            }
            if (valid != 0 && i != *(s32*)lbl_80426BD0) {
                target[0] = activePos[0] + sinY * spacing;
                target[1] = activePos[1];
                target[2] = activePos[2] + cosY * spacing;

                count = fn_8010E138(target, floorHits);
                if (count > 0) {
                    if (count < 2) {
                        target[1] = floorHits[0];
                    } else {
                        best = lbl_8047D0D8;
                        fallback = lbl_8047D0D8;
                        found = 0;
                        for (; count > 0; count--) {
                            y = floorHits[(found + (s32)(floorHits - floorHits)) * 3];
                            if (fallback < y) {
                                fallback = y;
                            }
                            if (y - target[1] <= lbl_8047D07C && best < y) {
                                best = y;
                                found = 1;
                            }
                            floorHits[0] = floorHits[0];
                        }
                        if (found != 0) {
                            target[1] = best;
                        } else {
                            target[1] = fallback;
                        }
                    }
                }

                spacing += lbl_8047D0AC;

                handle = 0;
                handles[0] = *(u32*)&lbl_8047D030;
                handles[1] = *(u32*)&lbl_8047D034;
                if (i >= 0 && i < 2) {
                    handle = handles[i];
                }
                fn_8018C0A8(0, handle, target);

                handle = 0;
                handles[0] = *(u32*)&lbl_8047D030;
                handles[1] = *(u32*)&lbl_8047D034;
                if (i >= 0 && i < 2) {
                    handle = handles[i];
                }
                model = GSresGetResource(0, handle);
                GSmodelSetRotation(model, rotation);
            }
        }
    }

    member = lbl_80426BD0;
    for (i = 0; i < 2; i++, member += 0x20) {
        handle = 0;
        handles[0] = *(u32*)&lbl_8047D030;
        handles[1] = *(u32*)&lbl_8047D034;
        if (i >= 0 && i < 2) {
            handle = handles[i];
        }
        fn_8018C1E8(0, handle);
    }

    *(f32*)(lbl_80426BD0 + 0x14) = lbl_8047D038;
    *(f32*)(lbl_80426BD0 + 0x18) = lbl_8047D038;
    *(f32*)(lbl_80426BD0 + 0x1c) = lbl_8047D038;
    *(f32*)(lbl_80426BD0 + 0x20) = lbl_8047D038;
    *(f32*)(lbl_80426BD0 + 0x34) = lbl_8047D038;
    *(f32*)(lbl_80426BD0 + 0x38) = lbl_8047D038;
    *(f32*)(lbl_80426BD0 + 0x3c) = lbl_8047D038;
    *(f32*)(lbl_80426BD0 + 0x40) = lbl_8047D038;

    member = lbl_80426BD0;
    for (i = 0; i < 2; i++, member += 0x20) {
        valid = 0;
        if (i >= 0 && i < 2 && (*(u16*)(member + 4) & 1) != 0) {
            valid = 1;
        }
        if (valid != 0) {
            if (i >= 0 && i < 2 && (*(u16*)(member + 4) & 1) != 0) {
                state = *(s32*)(member + 0xc);
            } else {
                state = 2;
            }
            if (state >= 0 && state < 2) {
                handle = 0;
                handles[0] = *(u32*)&lbl_8047D030;
                handles[1] = *(u32*)&lbl_8047D034;
                if (i >= 0 && i < 2) {
                    handle = handles[i];
                }
                if (*(s32*)(member + 0xc) == 1) {
                    fn_80188AF4(0, handle);
                }
                if (state == 1) {
                    fn_80188F78(0, handle);
                }
                *(s32*)(member + 0xc) = state;
            }
        }
    }

    *(u32*)(lbl_80426BD0 + 0x10) = 0x12c;
    *(u32*)(lbl_80426BD0 + 0x30) = 0x12c;
    *(f32*)(lbl_80426BD0 + 0x13c) = lbl_8047D038;
}

typedef struct HeroMoveThemeTable {
    u32 words[10];
} HeroMoveThemeTable;

typedef struct HeroMoveFloorTable {
    u32 words[20];
} HeroMoveFloorTable;

/* Initialize the two field hero models and select the area's model theme. */
s32 heroMoveInit(void* position, void* rotation)
{
    extern void fn_8018D998(u32 group, u32 object);
    extern void fn_8018C8F4(u32 group, u32 object, u32 flags);
    extern void fn_8018C0A8(u32 group, u32 object, void* position);
    extern void* GSresGetResource(u32 group, u32 handle);
    extern s32 updateAnimation__Ff15HEROMOVE_MEMBER(void* model, s32 member, f32 amount);

    HeroMoveFloorTable floors;
    HeroMoveThemeTable themes;
    void* models[2];
    u32 handles[2];
    s32 i;
    u32* floorCursor;
    u32 floor;
    u32 theme;
    u32 handle;
    s32 area;
    u8 unavailable;

    if (fn_800FF548() == 0) {
        floors = *(HeroMoveFloorTable*)lbl_802729C0;
        themes = *(HeroMoveThemeTable*)lbl_80272A10;

        unavailable = fn_801906A0(0x8AE) == 0;
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

        fn_8018E050(0, 100, theme);
        fn_8018E050(0, 101, 0x00F30400);
    } else {
        fn_8018D998(0, 100);
        fn_8018D998(0, 101);
    }

    for (i = 0; i < 2; i++) {
        handles[0] = *(u32*)&lbl_8047D030;
        handles[1] = *(u32*)&lbl_8047D034;
        if (i >= 0 && i < 2) {
            handle = handles[i];
        }
        models[i] = GSresGetResource(0, handle);
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
        fn_8018C0A8(0, 100, position);
        fn_8018BF24(0, 100, rotation);
    }

    for (i = 0; i < 2; i++) {
        updateAnimation__Ff15HEROMOVE_MEMBER(models[i], i, lbl_8047D038);
    }

    initFloor__Fv();
    *(u32*)(lbl_80426BD0 + 0x44) = 0;
    *(u32*)(lbl_80426BD0 + 0x48) = 0;
    *(u32*)(lbl_80426BD0 + 0x414) = 0;
    *(u32*)(lbl_80426BD0 + 0x418) = 0;
    *(u32*)(lbl_80426BD0 + 0x410) = 0;
    *(u32*)(lbl_80426BD0 + 0x188) = 0;
    return 0;
}


/* Update the active field-chat target and its two hero models. */
typedef struct HeroChatVec3 {
    f32 x;
    f32 y;
    f32 z;
} HeroChatVec3;

u32 updateChat__F15HEROMOVE_MEMBER(s32 player)
{
    extern f64 sin(f32);
    extern f64 cos(f32);
    extern u32 peopleGetPosition();
    extern void PSVECAdd(HeroChatVec3*, HeroChatVec3*, HeroChatVec3*);
    extern void fn_8018C7C8(u32, u32, u32);
    extern void fn_8018C69C(u32, u32, u32);
    extern void fn_8018CA20(u32, u32, u32);
    extern void fn_800F7434(void*, u32, ...);
    extern void fn_8018790C(u32, u32);
    extern void fn_800F7D38(u32, u32, u32);
    extern void fn_800F7C8C(u32, u32, u32);
    extern void fn_8018BA04(u32, u32, HeroChatVec3*);
    extern void fn_80187D48(u32, u32, f32, f32, f32, f32);
    extern void floorEventCtrlTresure(u32, u32, u32);
    extern void fn_80183730(u32);
    extern void fn_801812E8(u32, u32, u32);
    extern void fn_80189490(u32, u32);
    extern void fn_80183688(u32);
    u32 handles[2];
    u32 eventObject;
    u32 handle;
    u32 resource;
    u32 person;
    u32 positionPtr;
    u32 rotationPtr;
    u32 interaction;
    u32 i;
    u8 collision[0xD0];
    HeroChatVec3 position;
    HeroChatVec3 offset;
    HeroChatVec3 interactionPosition;

    handles[0] = *(u32*)&lbl_8047D030;
    handles[1] = *(u32*)&lbl_8047D034;
    if (player >= 0 && player < 2) {
        handle = handles[player];
    }

    eventObject = fn_8018CD08(0, handle, lbl_8047D070, lbl_8047D074);
    if (eventObject == 0) {
        handles[0] = *(u32*)&lbl_8047D030;
        handles[1] = *(u32*)&lbl_8047D034;
        i = *(s32*)lbl_80426BD0;
        if ((s32)i >= 0 && (s32)i < 2) {
            resource = handles[i];
        }

        person = peopleSearchID(fn_8018D998(0, resource));
        interaction = 0;
        if (person != 0) {
            rotationPtr = fn_8018FCBC();
            positionPtr = peopleGetPosition(person);
            position = *(HeroChatVec3*)rotationPtr;
            position.y += lbl_8047D078;
            offset.x = lbl_8047D07C * (f32)sin(((HeroChatVec3*)positionPtr)->y);
            offset.y = lbl_8047D038;
            offset.z = lbl_8047D07C * (f32)cos(((HeroChatVec3*)positionPtr)->y);
            PSVECAdd(&position, &offset, &offset);
            interaction = GScolsys2CheckGetEventID(&position, &offset, collision);
        }

        memcpy(lbl_80426BD0 + 0x1A0, collision, 0xD0);
        *(u32*)(lbl_80426BD0 + 0x410) = interaction;
        if (interaction != 0) {
            fn_8018790C(0, 100);
            return 1;
        }
        return 0;
    }

    fn_800F7D38(1, 0, 0);
    fn_800F7C8C(1, 0, 0);
    for (i = 0; (s32)i < 2; i++) {
        if ((*(u16*)(lbl_80426BD0 + i * 0x20 + 4) & 1) != 0) {
            handles[0] = *(u32*)&lbl_8047D030;
            handles[1] = *(u32*)&lbl_8047D034;
            handle = handles[i];
            resource = (u32)GSresGetResource(0, handle);
            updateAnimation__Ff15HEROMOVE_MEMBER(resource, lbl_8047D038, i);
            fn_8018C7C8(0, handle, 0x80000008);
            fn_8018C69C(0, handle, 0x100);
            fn_8018C69C(0, handle, 0x400);
            fn_8018CA20(0, handle, 0);
        }
    }

    fn_8018BA04(*(u32*)(eventObject + 0x28),
                 *(u32*)(eventObject + 0x2C), &interactionPosition);
    fn_80187D48(0, handle, interactionPosition.x, interactionPosition.y,
                interactionPosition.z, lbl_8047D080);
    if (fn_8018D7D0(*(u32*)(eventObject + 0x28),
                    *(u32*)(eventObject + 0x2C)) != 0) {
        floorEventCtrlTresure(*(u32*)(eventObject + 0x28),
                              *(u32*)(eventObject + 0x2C), 2);
    } else {
        fn_80183730(fn_8018D998(*(u32*)(eventObject + 0x28),
                                *(u32*)(eventObject + 0x2C)));
        person = fn_8018397C(*(u32*)(eventObject + 0x28),
                             *(u32*)(eventObject + 0x2C));
        if (person != 0) {
            fn_801812E8(*(u32*)(eventObject + 0x28),
                        *(u32*)(eventObject + 0x2C), 1);
            fn_800F7434((void*)person, 4, *(u32*)(eventObject + 0x28),
                        *(u32*)(eventObject + 0x2C), 0, 0);
            fn_801812E8(*(u32*)(eventObject + 0x28),
                        *(u32*)(eventObject + 0x2C), 0);
            fn_80189490(*(u32*)(eventObject + 0x28),
                        *(u32*)(eventObject + 0x2C));
        }
        fn_80183688(fn_8018D998(*(u32*)(eventObject + 0x28),
                                *(u32*)(eventObject + 0x2C)));
    }

    for (i = 0; (s32)i < 2; i++) {
        if ((*(u16*)(lbl_80426BD0 + i * 0x20 + 4) & 1) != 0) {
            handles[0] = *(u32*)&lbl_8047D030;
            handles[1] = *(u32*)&lbl_8047D034;
            handle = handles[i];
            fn_8018CA20(0, handle, 1);
            fn_8018C7C8(0, handle, 0x700);
            fn_8018C69C(0, handle, 0x80000008);
        }
    }
    fn_800F7D38(1, 0, 0);
    fn_800F7C8C(1, 0, 0);
    return 1;
}
#endif
