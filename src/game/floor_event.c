/**
 * @file floor_event.c
 * @brief GSfield world segment -- split from gs_field_world.c.
 *
 * XD source unit: floorEvent* (floor event module)
 * Address range: 0x80115CB4 - 0x80116D30 (~3 functions)
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
extern void updateAnimation__Ff15HEROMOVE_MEMBER(void);
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
extern void* fn_80113F48(void);
extern void fn_8018C1E8(void*, u32, u32);
extern void fn_801653CC(u32, u32, u32);
extern void msgctrlSetValue(u32, u32);
extern void winMsgOpen(u32, u32, u32, u32);
extern void winMsgClose(u32);
extern u16 pcboxDelItem(u8*, u16, u16);
extern s8 fn_8001E184(void);
s32 floorEventGetTresure(u8, u32, s32);
extern void fn_8018B76C(void*, u32, u32, u32, u32);
extern void fn_8018C7C8(void*, u32, u32);
extern u32 fn_801902E0(u16);
extern void fn_80166A28(u32);
extern void peopleWaitSyncMotion(void*, u32, u32);
extern void fn_80190528(u16);
s32 floorEventCtrlTresure(void*, u32, u8);
extern void* floorGetResource(void*, u32);
extern void GSmodelSetAnimIndex(void*, s32);
extern void GSmodelSetAnimFrame(void*, f32);
extern void GSmodelSetAnimRate(void*, f32);
extern void GSmodelSetAnimType(void*, u32);
extern void GSmodelStartAnimation(void*);
extern void _threadSwitch(void);
extern u8 GSmodelIsAnimating(void*);
extern void* GSmodelGetPart(void*, u8);
extern void GSpartGetTransform(void*, void*, u32, u32);
extern void GSpartFree(void*);
extern void fn_8018AACC(u32, u32, u32, void*);
extern u32 peopleMoveCheck(u32 groupId, u32 index, u8 waitFlag);
extern void fn_8018805C(u32, u32, f32, f32);
extern void fn_80184470(u32, u32);
extern void fn_8018C0A8(u32, u32, void*);
extern void fn_801669BC(u32);
extern u32 GSmodelCanAnimate(void*);
extern void fn_801845E4(u32, u32, u32, u32, u8);
extern void fn_801860F8(u32, u32, f32, f32, f32);
extern void GSmodelGetFrameCount(void*, f32*, u32);
extern s32 fn_800D37CC(void);
extern f32 lbl_8047CFA0;
extern u32 lbl_80478EC8;
extern u32 lbl_80478ECC;
extern f32 lbl_8047CFA4;
extern f32 lbl_8047CFA8;
extern f32 lbl_8047CFAC;
extern f64 lbl_8047CFB8;
extern f32 lbl_8047CFB4;
extern f32 lbl_8047CFB0;
s32 floorEventCtrlElevator(void*, u32, u16, u32, u32);
extern void scriptSetCol(s16, u32);
extern void fn_801903B0(u16);
extern u32 fn_8018C558(u32, u32);
extern void fn_8018C8F4(u32, u32, u32);
s32 floorEventCtrlDoor(void*, u32, u8);
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
extern void* fn_801906A0(u32);
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
extern void fn_8018D998(void);
extern void peopleSearchID(void);
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
void fn_8012B19C(void);
void heroMoveChkHinderClear(void);
extern void fn_800D3088(void);
extern f64 lbl_8047D068;
void getStep__FP8FOOTSTEPP8_GSmodelPiP8FOOTWORK(void);
extern void fn_8018CD08(void);
extern void fn_8018FCBC(void);
extern void fn_8018FC50(void);
extern void fn_800CE148(void);
extern void fn_800CDBE0(void);
extern void GScolsys2CheckGetEventID(void);
extern void fn_8018790C(void);
extern void fn_800F7D38(void);
extern void fn_800F7C8C(void);
extern void fn_8018BA04(void);
extern void fn_80187D48(void);
extern void fn_8018D7D0(void);
extern void fn_80183730(void);
extern void fn_8018397C(void);
extern void fn_801812E8(void);
extern void fn_80189490(void);
extern void fn_80183688(void);
extern f32 lbl_8047D070;
extern f32 lbl_8047D074;
extern f32 lbl_8047D078;
extern f32 lbl_8047D07C;
extern f32 lbl_8047D080;
void updateChat__F15HEROMOVE_MEMBER(void);
void heroMoveCheckEvent(void);
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
void fn_8012D39C(void);
extern f32 lbl_8047D0AC;
void fn_8012D7F0(void);
extern void fn_800E3C64(void);
extern f32 lbl_8047D0B0;
void fn_8012DE94(void);
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
void fn_8012E388(void);
extern void fn_800F7AF0(void);
extern void fn_801887D8(void);
extern void PSVECDistance(void);
extern f32 lbl_8047D0D0;
extern f32 lbl_8047D0D4;
void moveLeader__F15HEROMOVE_MEMBER(void);
extern void dbgMenuIsOpen(void);
extern void menuIsCheck(void);
extern void fn_8018C424(void);
extern void fn_8000D710(void);
extern u8 lbl_80272A38[];
extern f32 lbl_8047D064;
void heroMoveMain(void);
extern void fn_80188AF4(u32, u32);
extern void fn_80188F78(u32, u32);
void fn_8012F1FC(void);
void fn_8012F40C(void);
extern void GSmodelGetRotation(void);
extern void fn_8010E138(void);
extern void GSmodelSetRotation(void);
extern f32 lbl_8047D0D8;
void initFloor__Fv(void);
extern void floorGetNextFloorID(void);
extern void fn_8006AE18(void);
extern u8 lbl_802729C0[];
extern u8 lbl_80272A10[];
void heroMoveGetKenObjID(void);
extern void fn_8018E050(void);
extern void GSmodelEnableAnimBlend(void);
extern void fn_8018CB5C(void);
extern void fn_80189328(void);
extern void fn_8018BF24(void);
void heroMoveInit(void);
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
extern void fn_8012BAF0(u8 type, void* src, u32 val);
extern u32 fn_8012BDE0(u32 r3, u32 r4);
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

/* 0x80115CB4 | 0xB0 */
extern u32 lbl_80478EBC;
extern u32 lbl_80478EB8;

void* floorEventGetTresureList(u32 param)
{
    u32 type = param & 0x7FFF0000;
    u32 target;
    u32 found = 0;
    u32 index;
    u8* entry = 0;

    if (type != 0x7FFF0000) {
        return 0;
    }

    target = param & 0x1FF;
    for (index = 0; index < *(u32*)lbl_80478EB8; index++) {
        entry = (u8*)lbl_80478EBC + index * 0x1C;
        if (*(u16*)(entry + 4) == (u32)fn_800FF56C()) {
            found++;
            if (target == found - 1) {
                break;
            }
        }
    }

    if (index == *(u32*)lbl_80478EB8) {
        return 0;
    }
    return entry;
}

/* 0x80115D64 | 0xA0 */
void floorEventSetTresureDisp(u32 rawIndex, u32 display)
{
    u8* entry;
    u32 index;
    u32 found = 0;
    u32 encoded;

    for (index = 0; index < *(u32*)lbl_80478EB8; index++) {
        entry = (u8*)lbl_80478EBC + index * 0x1C;
        if (*(u16*)(entry + 4) == (u32)fn_800FF56C()) {
            if (index == rawIndex) {
                encoded = found | 0x7FFF0000;
                break;
            }
            found++;
        }
    }

    if (index != *(u32*)lbl_80478EB8) {
        fn_8018C1E8(fn_80113F48(), encoded, display);
    }
}

/* 0x80115E6C | 0x2F8 */
s32 floorEventGetTresure(u8 type, u32 item, s32 count)
{
    s32 result;
    u32 message = 0;

    fn_801653CC(0x3CA, 0, 0xFF);

    switch (type) {
    case 2:
        msgctrlSetValue(0x4B, item);
        winMsgOpen(3, 0x3CB5, 1, 0);
        winMsgClose(1);
        heroAddPokedoru(0, item);
        winMsgOpen(3, 0x3CB7, 1, 0);
        winMsgClose(1);
        break;

    case 1:
    case 4:
        if (count <= 0) {
            break;
        }

        msgctrlSetValue(0x2D, item);
        msgctrlSetValue(0x2F, count);
        if (type == 1) {
            if (count != 1) {
                goto plural_found_message;
            }
            message = 0x3CB4;
            goto open_found_message;
plural_found_message:
            message = 0x3CB9;
open_found_message:
            winMsgOpen(3, message, 1, 0);
            winMsgClose(1);
        }

        result = heroItemAddItemDataId(0, (u16)item, (u16)count, -1);
        if (result == 0) {
            if (count == 1) {
                message = 0x3CB8;
            } else {
                message = 0x3CBD;
            }
        } else if (result > 0) {
            result = pcboxDelItem(0, (u16)item, (u16)result);
            if (count == 1) {
                message = 0x3CBA;
            } else {
                message = 0x3CBB;
            }
        }
        winMsgOpen(3, message, 1, 0);
        winMsgClose(1);
        break;

    case 3:
        msgctrlSetValue(0x2D, item);
        winMsgOpen(3, 0x3CBC, 1, 0);
        winMsgClose(1);
        heroItemAddItemDataId(0, (u16)item, 1, -1);
        winMsgOpen(3, 0x3CB6, 1, 0);
        winMsgClose(1);

        switch (item) {
        case 0x21A:
            message = 0x3B33;
            break;
        case 0x21D:
            message = 0x3B35;
            break;
        case 0x21B:
            message = 0x3B39;
            break;
        case 0x21C:
            message = 0x3B37;
            break;
        case 0x223:
            message = 0x44C4;
            break;
        }

        winMsgOpen(3, message, 1, 0);
        result = fn_8001E184();
        winMsgClose(1);
        if (result != 0) {
            return 0;
        }

        switch (item) {
        case 0x21A:
            message = 0x3B34;
            break;
        case 0x21D:
            message = 0x3B36;
            break;
        case 0x21B:
            message = 0x3B30;
            break;
        case 0x21C:
            message = 0x3B38;
            break;
        case 0x223:
            message = 0x44C5;
            break;
        }

        winMsgOpen(3, message, 1, 0);
        winMsgClose(1);
        break;
    }

    return result;
}

static inline void* floorEventFindTresureEntry(u32 param)
{
    u32 type = param & 0x7FFF0000;
    u32 target;
    u32 found = 0;
    u32 index;
    u8* entry = 0;

    if (type != 0x7FFF0000) {
        return 0;
    }

    target = param & 0x1FF;
    for (index = 0; index < *(u32*)lbl_80478EB8; index++) {
        entry = (u8*)lbl_80478EBC + index * 0x1C;
        if (*(u16*)(entry + 4) == (u32)fn_800FF56C()) {
            found++;
            if (target == found - 1) {
                break;
            }
        }
    }

    if (index == *(u32*)lbl_80478EB8) {
        return 0;
    }
    return entry;
}

typedef struct FloorTreasureEntry {
    u8 flags;
    u8 count;
    u8 pad02[2];
    u16 floor;
    u16 eventFlag;
    u32 field08;
    u32 item;
    u8 pad10[0xC];
} FloorTreasureEntry;

/* 0x80116164 | 0x30C */
s32 floorEventCtrlTresure(void* character, u32 param, u8 mode)
{
    FloorTreasureEntry* entry;
    FloorTreasureEntry* displayEntry;

    entry = (FloorTreasureEntry*)floorEventFindTresureEntry(param);
    if (entry == 0) {
        return -1;
    }

    switch (mode) {
    case 0:
        switch ((entry->flags >> 5) & 7) {
        case 1:
            fn_8018B76C(character, param, 2, 0, 1);
            fn_8018C7C8(character, param, 1);
            break;
        case 2:
        case 3:
            fn_8018C1E8(character, param, 0);
            break;
        }
        break;

    case 1:
        switch ((entry->flags >> 5) & 7) {
        case 1:
            fn_8018B76C(character, param, 0, 0, 1);
        case 2:
        case 3:
            fn_8018B76C(character, param, 0, 0, 1);
            break;
        }
        break;

    case 2:
        if (entry->eventFlag != 0 && fn_801902E0(entry->eventFlag) != 0) {
            return 0;
        }

        if (((entry->flags >> 5) & 7) == 1) {
            fn_8018B76C(character, param, 1, 0, 0);
            fn_80166A28(0x3C2);
            peopleWaitSyncMotion(character, param, 1);
        }

        displayEntry = (FloorTreasureEntry*)floorEventFindTresureEntry(param);
        if (displayEntry != 0) {
            switch ((displayEntry->flags >> 5) & 7) {
            case 1:
                fn_8018B76C(character, param, 2, 0, 1);
                fn_8018C7C8(character, param, 1);
                break;
            case 2:
            case 3:
                fn_8018C1E8(character, param, 0);
                break;
            }
        }

        fn_80190528(entry->eventFlag);
        floorEventGetTresure((entry->flags >> 2) & 7, entry->item, entry->count);
        break;
    }

    return 0;
}

extern u32 lbl_80478EB8;
extern u8 lbl_8035BB70[];
extern u32 lbl_80478EBC;
#if 0
asm void floorEventChangeTresure(void) {
#include "src/game/gs_field_world_floorEventChangeTresure.inc"
}
#else
s32 floorEventChangeTresure(u32 index, u16 val, u8 byte) {
    extern u32 lbl_80478EB8;
    extern u32 lbl_80478EBC;
    extern u8 lbl_8035BB70[];
    extern u8 lbl_80272708[];
    u32 count;
    u8* entry;
    count = *(u32*)lbl_80478EB8;
    if (index >= count) {
        GSlogWrite((const char*)lbl_80272708, lbl_8035BB70);
        return -1;
    }
    entry = (u8*)lbl_80478EBC + index * 0x1c;
    *(u32*)(entry + 0xc) = val;
    *(u8*)(entry + 0x1) = byte;
    return 0;
}
#endif

typedef struct FloorDoorEntry {
    s8 openAnim;
    s8 closeAnim;
    s8 openEndAnim;
    s8 closeEndAnim;
    u8 pad04;
    s8 alternateAnimA;
    s8 alternateAnimB;
    u8 doorType;
    u8 partIndex;
    u8 pad09;
    s16 collisionId;
    u8 pad0C[2];
    u16 openFlag;
    u16 closeFlag;
    u8 pad12[2];
    u32 resourceId;
} FloorDoorEntry;

static inline void floorEventStartAnim(void* model, s16 animation, f32 frame)
{
    if (model != 0) {
        GSmodelSetAnimIndex(model, animation);
        GSmodelSetAnimFrame(model, frame);
        GSmodelSetAnimRate(model, lbl_8047CFA4);
        GSmodelSetAnimType(model, 0);
        GSmodelStartAnimation(model);
    }
}

static inline void floorEventWaitAnim(void* model)
{
    if (model != 0) {
        while (GSmodelIsAnimating(model) != 0) {
            _threadSwitch();
        }
    }
}

/* 0x80116958 | 0x3D8 */
s32 floorEventCtrlDoor(void* floor, u32 index, u8 mode)
{
    FloorDoorEntry* door;
    void* model;
    s16 animation = -1;
    u32 sound = 0;
    u32 savedHero;
    u32 savedPartner;
    f32 frame = 0.0f;

    if (index >= *(u32*)lbl_80478EC8) {
        return -1;
    }
    door = (FloorDoorEntry*)lbl_80478ECC + index;

    switch (mode) {
    case 0:
        if (door->openFlag != 0 && fn_801902E0(door->openFlag) != 0) {
            return 0;
        }
        if (door->closeFlag != 0 && fn_801902E0(door->closeFlag) == 0) {
            return 1;
        }
    case 1:
        if (door->openFlag != 0) {
            fn_80190528(door->openFlag);
        }
        scriptSetCol(door->collisionId, 0);
        break;

    case 2:
        if (door->openFlag != 0 && fn_801902E0(door->openFlag) == 0) {
            return 0;
        }
    case 3:
        if (door->openFlag != 0) {
            fn_801903B0(door->openFlag);
        }
        scriptSetCol(door->collisionId, 1);
        break;
    }

    if (door->resourceId == 0) {
        return -1;
    }
    model = floorGetResource(floor, door->resourceId);
    if (model == 0) {
        return -1;
    }

    switch (mode) {
    case 0:
        animation = door->openAnim;
        switch (door->doorType) {
        case 1:
            sound = 0x44;
            break;
        case 2:
            sound = 0x44;
            break;
        case 3:
            sound = 0x4BE;
            break;
        }
        break;

    case 1:
        animation = door->openEndAnim;
        if (animation < 0) {
            animation = door->openAnim;
            if (animation >= 0) {
                GSmodelSetAnimIndex(model, animation);
                GSmodelGetFrameCount(model, &frame, 0);
                frame -= 1.0f;
            }
        }
        break;

    case 2:
        animation = door->closeAnim;
        switch (door->doorType) {
        case 1:
            sound = 0x44;
            break;
        case 2:
            sound = 0x44;
            break;
        case 3:
            sound = 0x4BE;
            break;
        }
        break;

    case 3:
        animation = door->closeEndAnim;
        if (animation < 0) {
            animation = door->closeAnim;
            if (animation >= 0) {
                GSmodelSetAnimIndex(model, animation);
                GSmodelGetFrameCount(model, &frame, 0);
                frame -= 1.0f;
            }
        }
        break;
    }

    if (animation < 0 || GSmodelCanAnimate(model) == 0) {
        return -1;
    }

    GSmodelSetAnimIndex(model, animation);
    GSmodelSetAnimFrame(model, frame);
    GSmodelSetAnimRate(model, 0.5f);
    GSmodelSetAnimType(model, 0);
    GSmodelStartAnimation(model);
    if (sound != 0) {
        fn_80166A28(sound);
    }

    if (mode == 0 || mode == 2) {
        savedHero = fn_8018C558(0, 0x64);
        savedPartner = fn_8018C558(0, 0x65);
        heroMoveInitEvent();
        while (GSmodelIsAnimating(model) != 0) {
            _threadSwitch();
        }
        heroMoveTermEvent();
        fn_8018C8F4(0, 0x64, savedHero);
        fn_8018C8F4(0, 0x65, savedPartner);
    }

    return 0;
}

typedef struct FloorEventVec {
    f32 x;
    f32 y;
    f32 z;
} FloorEventVec;

/* 0x80116470 | 0x4E8 */
s32 floorEventCtrlElevator(void* floor, u32 index, u16 command,
                           u32 group, u32 person)
{
    FloorDoorEntry* elevator;
    FloorEventVec position;
    void* model;
    void* part;
    u32 floorGroup;
    s16 animation = -1;
    f32 frame = lbl_8047CFA0;

    if (index >= *(u32*)lbl_80478EC8) {
        return -1;
    }
    elevator = (FloorDoorEntry*)lbl_80478ECC + index;
    if (elevator->resourceId == 0) {
        return -1;
    }
    model = floorGetResource(floor, elevator->resourceId);
    if (model == 0) {
        return -1;
    }

    floorGroup = floorDataBiosGetGroupID(floorDataBiosGetPtr((u32)floor));

    switch (command) {
    case 1:
    case 2:
    case 0x81:
    case 0x82:
        animation = elevator->openAnim;
        if (animation >= 0) {
            floorEventStartAnim(model, animation, frame);
        }
        if (command & 0x80) {
            fn_80166A28(0x44);
        }
        floorEventWaitAnim(model);

        part = GSmodelGetPart(model, elevator->partIndex);
        GSpartGetTransform(part, &position, 0, 0);
        GSpartFree(part);
        if (command & 1) {
            fn_8018AACC(group, person, 1, &position);
            peopleMoveCheck(group, person, 1);
            fn_8018805C(group, person, lbl_8047CFA0, lbl_8047CFA4);
            peopleMoveCheck(group, person, 1);
        } else {
            fn_80184470(group, person);
            part = GSmodelGetPart(model, elevator->partIndex);
            GSpartGetTransform(part, &position, 0, 0);
            GSpartFree(part);
            fn_8018C0A8(group, person, &position);
            position.z += lbl_8047CFA8;
            fn_8018AACC(group, person, 1, &position);
            peopleMoveCheck(group, person, 1);
        }

        animation = elevator->closeAnim;
        if (animation >= 0) {
            floorEventStartAnim(model, animation, frame);
        }
        if (command & 0x80) {
            fn_80166A28(0x44);
        }
        floorEventWaitAnim(model);
        break;

    case 0xC0:
        floorEventWaitAnim(model);
        fn_801669BC(0x45);
        fn_80166A28(0x46);
        break;

    default:
        if (command & 4) {
            animation = elevator->alternateAnimA;
        } else if (command & 8) {
            animation = elevator->alternateAnimB;
        }
        if (animation < 0 || GSmodelCanAnimate(model) == 0) {
            return -1;
        }

        if (command & 0x20) {
            fn_801845E4(group, person, floorGroup, elevator->resourceId,
                        elevator->partIndex);
            fn_801860F8(group, person, 0.0f, 0.0f, 0.0f);
        }
        if (command & 0x10) {
            GSmodelSetAnimIndex(model, animation);
            GSmodelGetFrameCount(model, &frame, 0);
            frame -= 1.0f;
        }

        floorEventStartAnim(model, animation, frame);

        if (command & 0x40) {
            floorEventCtrlElevator(floor, index, 0xC0, group, person);
        } else if (command & 0x80) {
            fn_80166A28(0x45);
        }
        break;
    }

    GSmodelGetFrameCount(model, &frame, 0);
    return (s32)(100.0f * ((2.0f * frame) / (f32)fn_800D37CC()));
}
