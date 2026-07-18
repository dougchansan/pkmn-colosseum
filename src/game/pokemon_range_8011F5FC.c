/**
 * @file pokemon_range_8011F5FC.c
 * @brief GSfield world segment -- split from gs_field_world.c.
 *
 * XD source unit: pokemon main code (+ fightOut / darkPokemon sub-blocks)
 * Address range: 0x8011F5FC - 0x80128E38 (~86 functions)
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
extern u8 menuSubGetPokemonSexForFightDisp();
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
void pokemonSetLevelBasisStatus(u8* ptr, u32 level);
extern void heroItemGetItemKindToItemAryPtr(void);
extern void heroSetStatus();
extern u32 heroGetStatus(u8* ptr, u32 selector, u16 index);
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
extern void pokemonEvolutionCreateAddPokemon();
extern s32 pokemonEvolution();
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
extern void fn_80113F48(void);
extern void fn_8018C1E8(void);
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
void wazaSetStatus();
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
extern u8* itemDataBiosGetPtr(u16 itemDataId);
extern u8 itemDataBiosGetKind(u8* ptr);
extern u32 itemDataBiosGetBuff(u8* ptr);
extern f32 lbl_8047D018;
void pokemonAddDpFormPokemonDpFilterId(u8* ptr, u16 itemDataId, u16 filterId);
extern void* fn_801EEEB8();
void pokemonSetDarkPokemonStatus(void);
extern void GScharCpy();
extern u16 fn_8010BBB8();
extern void fn_8001D994(void);
void pokemonToMenuPokemonStatus();
extern void GSmsgGetGSchar(void);
extern void fn_8010C4D4(void);
extern void fn_8010C46C(void);
extern u32 fn_800E0C54(void);
extern u8 lbl_8027296C[];
void pokemonCheckSetMonohiroi(void);
void pokemonAllKaihuku(void);
extern u32 fn_801DE190(u32 idx, u32 base, u32 flag);
extern void fn_801DA3CC(void);
extern void fn_801DA36C(void);
extern u32 lbl_80478F90; /* obj header ptr (SDA) */
extern void fn_80135530(void);
void pokemonGetFriendFormPokemonFriendFilterId(u8* obj, u16 item_id, u32 filter_id);
extern u8 lbl_80272948[];
void pokemonGetEffortFromPokemon(void);
extern void fn_80008154(void);
extern void fn_80142CF4(void);
extern u32 sexGetPokemonSexRaitoKotei(u32);
extern void fn_801EE958(void);
extern void fn_801EEB34(void);
extern void memoDataSet();
u32 pokemonCheckFightOut(u8* ptr);
extern void gamedataAttestBiosCopy(void);
void pokemonCreate(void);
u32 pokemonCreateRndFit(u8*, s32, s32, s32, u32);
extern void fadeSet(f32 duration, u32 mode);
extern void fadeCheck(u32 wait);
extern s32 evolutionOpen();
extern f32 lbl_8047D020;
s32 pokemonEvolutionAll(u8*, u32, u32, void*, u8*, s32, s32, s32);
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
extern void fightPokemonBiosSetMotoPokemonPtr();
extern void fightPokemonBiosSetEntryId();
extern void fightPokemonBiosSetCatchEntryFlag();
extern void fightPokemonBiosSetLevelUpFlag();
extern void fightPokemonBiosSetDarkOutFlag();
extern void fightPokemonBiosSetHokakuFlag();
extern void fightOutPokemonBiosSetMotoFightPokemonPtr();
extern void fightOutPokemonBiosSetFightPokemonPtr();
extern u8 fightWazaCheckWriteJoutaiDataId(u32 fight_waza, u16 joutai_id);
extern void fightWazaWriteJoutaiDataId(u32 fight_waza, u16 joutai_id, u32 arg);
extern u8 fightWazaIsJoutaiDataId(u32 fight_waza, u16 joutai_id);
extern void fightWazaInitJoutaiDataId(u32 fight_waza, u16 joutai_id);
extern void fightWazaInitJoutai(u32 fight_waza);
extern void fightOutPokemonBiosSetAbicntPhyAtk();
extern void fightOutPokemonBiosSetAbicntPhyDef();
extern void fightOutPokemonBiosSetAbicntSpeAtk();
extern void fightOutPokemonBiosSetAbicntSpeDef();
extern void fightOutPokemonBiosSetAbicntNimbleness();
extern void fightOutPokemonBiosSetAbicntAverage();
extern void fightOutPokemonBiosSetAbicntAvoid();
extern void fightOutPokemonBiosSetFightoutTurnCount();
extern void fightOutPokemonBiosSetSequencePtr();
extern void fightOutPokemonBiosSetSketchWazaDataId();
extern void fightOutPokemonBiosSetLastSelectWazaDataId();
extern void fightOutPokemonBiosSetLastUseWazaDataId();
extern void fightOutPokemonBiosSetLastReceiveWazaTargetDataId();
extern void fightOutPokemonBiosSetHitWazaDataId();
extern void fightOutPokemonBiosSetHitWazaZokuseiDataId();
extern void fightOutPokemonBiosSetGamanDamageValue();
extern void fightOutPokemonBiosSetGamanDamageTargetId();
extern void fightOutPokemonBiosSetOumuWazaDataId();
extern void fightOutPokemonBiosSetNamakeFlag();
extern void fightOutPokemonBiosSetUsedItemDataId();
extern void fightOutPokemonBiosSetStockItemDataId();
extern void fightOutPokemonBiosSetSuccessCnt();
extern void fightOutPokemonBiosSetMeetEnemyFightPokemonEntryId(u8* ptr, u8 index, u16 entry_id);
extern void fightOutPokemonBiosSetZokuseiDataId();
extern void fightOutPokemonBiosSetTokuseiDataId();
extern void fightOutPokemonBiosSetDamageAtkValue();
extern void fightOutPokemonBiosSetDamageAtkTargetId();
extern void fightOutPokemonBiosSetDamageSpeValue();
extern void fightOutPokemonBiosSetDamageSpeTargetId();
extern void fightOutPokemonBiosSetMahiNoAttackFlag();
extern void fightOutPokemonBiosSetKonranMyselfAttackFlag();
extern void fightOutPokemonBiosSetOutWazaKoukanaiFlag();
extern void fightOutPokemonBiosSetTameWazaFlag();
extern void fightOutPokemonBiosSetItemNigeruFlag();
extern void fightOutPokemonBiosSetHuuinNoAttackFlag();
extern void fightOutPokemonBiosSetMeroMeroNoAttackFlag();
extern void fightOutPokemonBiosSetKanashibariNoAttackFlag();
extern void fightOutPokemonBiosSetChouhatsuNoAttackFlag();
extern void fightOutPokemonBiosSetIchamonNoAttackFlag();
extern void fightOutPokemonBiosSetHirumuNoAttackFlag();
extern void fightOutPokemonBiosSetPassPpdecFlag();
extern void fightOutPokemonBiosSetFightActionFlag();
extern void fightOutPokemonBiosSetDoClearbodyFlag();
extern void fightOutPokemonBiosSetReceivesWazaHiraishinFlag();
extern void fightOutPokemonBiosSetVanishoffFlag();
extern void fightOutPokemonBiosSetDoIkakuFlag();
extern void fightOutPokemonBiosSetDoTraceFlag();
extern void fightOutPokemonBiosSetNoPressureFlag();
extern void fightOutPokemonBiosSetIrekaetaFlag();
extern void fightOutPokemonBiosSetItemKoraetaFlag();
extern void fightOutPokemonBiosSetKaigaraDamageValue();
extern void fightOutPokemonBiosSetMyselfDamageAtkValue();
extern void fightOutPokemonBiosSetMyselfDamageAtkTargetId();
extern void fightOutPokemonBiosSetMyselfDamageSpeValue();
extern void fightOutPokemonBiosSetMyselfDamageSpeTargetId();
extern void fightOutPokemonBiosSetKizetuFlag();
extern void fightOutPokemonBiosSetIrekaeTargetEntryId();
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
extern void* pokemonFriendFilterDataBiosGetPtr(u32 idx);
extern void* pokemonTokuseiDataBiosGetPtr(u16 idx);
extern void* pokemonSeikakuRateDataBiosGetPtr(u8 idx);
extern u8 fn_8011CBF4(u8* ptr, u8 idx);
extern u8 fn_8011CC24(u8* ptr, u8 idx);
extern void* pokemonSeikakuDataBiosGetPtr(u8 idx);
extern u32 pokemonGrowDataBiosGetExp(u8* ptr, u8 idx);
extern void* pokemonGrowDataBiosGetPtr(u8 idx);
extern void fn_8011CED0(u8* ptr, u32 idx, u8 val);
extern void pokemonDataBiosSetKowaza(u8* ptr, u32 idx, u16 val);
extern void pokemonDataBiosSetTukamaetaFlag(u8* ptr, u32 val);
extern void pokemonDataBiosSetMitaFlag(u8* ptr, u32 val);
extern void pokemonDataBiosSetColor(u8* ptr, u32 idx, u8 val);
extern void pokemonDataBiosSetStatusFaceMenuSpriteId(u8* ptr, u32 idx, u16 val);
extern void pokemonDataBiosSetPokebodyId(u8* ptr, u32 idx, u32 val);
extern void pokemonDataBiosSetWazaMcn(u8* ptr, u16 idx, u8 val);
extern void pokemonDataBiosSetGetWazaDataId(u8* ptr, u32 idx, u16 val);
extern void pokemonDataBiosSetGetWazaLevel(u8* ptr, u32 idx, u8 val);
extern void pokemonDataBiosSetSinkaPokemonDataId(u8* ptr, u32 idx, u16 val);
extern void pokemonDataBiosSetSinkaBuff(u8* ptr, u32 idx, u16 val);
extern void pokemonDataBiosSetSinkaKind(u8* ptr, u32 idx, u8 val);
extern void pokemonDataBiosSetTokuseiDataId(u8* ptr, u32 idx, u8 val);
extern void pokemonDataBiosSetZokuseiDataId(u8* ptr, u32 idx, u8 val);
extern void pokemonDataBiosSetItemDataId(u8* ptr, u32 idx, u16 val);
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
extern void pokemonBiosSetPokemonWazaPp(u8* arg1, s32 arg2, u8 maxVal);
extern void pokemonBiosSetPokemonWazaDataId(void* ctx, u32 p1, u16 value);
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
extern void* pokemonDataBiosGetPtr(u32 idx);
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

#ifndef POKEMON_SET_STATUS_EXACT
/* Address: 0x80128E24 | Size: 0x8 | Pattern: sda_getter */
u32 fn_80128E24(void) {
    return lbl_8047ADB8;
}
/* 0x8011F634 | 0xA4 */
#if 0
asm void fn_8011F634(void) {
#include "src/game/gs_field_world_fn_8011F634.inc"
}
#else
u32 fn_8011F634(u8* ptr) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    extern u32 pokemonGetDarkPokemonLevel(u8* a);
    extern void* pokemonSeikakuDataBiosGetPtr(u32 val);
    extern u32 fn_8011CBF4(void* a, u32 b);
    u8 byte1;
    u32 val2;
    if (ptr == NULL) { return 0; }
    if ((u8)pokemonGetStatus(ptr, 0, 0xc2, 0) == 0) { return 0; }
    byte1 = (u8)pokemonGetStatus(ptr, 0, 0xbf, 0);
    val2 = pokemonGetDarkPokemonLevel(ptr);
    if ((u8)val2 <= 7) {
        return fn_8011CBF4(pokemonSeikakuDataBiosGetPtr(byte1), val2);
    }
    return 0;
}
#endif
/* 0x8011F6D8 | 0xA4 */
#if 0
asm void fn_8011F6D8(void) {
#include "src/game/gs_field_world_fn_8011F6D8.inc"
}
#else
u32 fn_8011F6D8(u8* ptr) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    extern u32 pokemonGetDarkPokemonLevel(u8* a);
    extern void* pokemonSeikakuDataBiosGetPtr(u32 val);
    extern u32 fn_8011CC24(void* a, u32 b);
    u8 byte1;
    u32 val2;
    if (ptr == NULL) { return 0; }
    if ((u8)pokemonGetStatus(ptr, 0, 0xc2, 0) == 0) { return 0; }
    byte1 = (u8)pokemonGetStatus(ptr, 0, 0xbf, 0);
    val2 = pokemonGetDarkPokemonLevel(ptr);
    if ((u8)val2 <= 7) {
        return fn_8011CC24(pokemonSeikakuDataBiosGetPtr(byte1), val2);
    }
    return 0;
}
#endif
/* 0x8011F77C | 0x194 */
extern f32 lbl_8047CFF0;
extern f64 lbl_8047D008;
extern f32 lbl_8047CFF4;
extern f64 lbl_8047D010;
extern f32 lbl_8047CFF8;
extern f32 lbl_8047CFFC;
extern f32 lbl_8047D000;
extern f32 lbl_8047D004;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void pokemonGetDarkPokemonLevel(void);
/* 0x8011F910 | 0x2BC */
extern u8* itemDataBiosGetPtr(u16 itemDataId);
extern u8 itemDataBiosGetKind(u8* ptr);
extern u32 itemDataBiosGetBuff(u8* ptr);
extern f64 lbl_8047D008;
extern f64 lbl_8047D010;
extern f32 lbl_8047CFF0;
extern f32 lbl_8047CFF4;
extern f32 lbl_8047D018;
void pokemonAddDpFormPokemonDpFilterId(u8* ptr, u16 itemDataId, u16 filterId) {
    u8* seikakuData;
    u8* rateData;
    u8* itemData;
    s32 rateDataId;
    u8 kake;
    u8 waru;
    f32 dp;
    f32 currentDp;
    f32 newDp;
    s32 rawDp;

    if ((u8)pokemonGetStatus(ptr, 0, 0xC2, 0) == 0) {
        return;
    }

    seikakuData = pokemonSeikakuDataBiosGetPtr(
        (u8)pokemonGetStatus(ptr, 0, 0xBF, 0));
    if (seikakuData == NULL) {
        return;
    }

    dp = (f32)(s8)pokemonDpFilterDataBiosGetValue(
        pokemonDpFilterDataBiosGetPtr(filterId));

    if (filterId == 4) {
        itemData = itemDataBiosGetPtr(itemDataId);
        if (itemData == NULL) {
            return;
        }
        if (itemDataBiosGetKind(itemData) != 6) {
            return;
        }
        dp *= (f32)itemDataBiosGetBuff(itemData);
    }

    if (filterId == 5) {
        if (ptr == NULL) {
            currentDp = lbl_8047CFF0;
        } else {
            rawDp = pokemonGetStatus(ptr, 0, 0xC5, 0);
            currentDp = (f32)rawDp / lbl_8047CFF4;
        }
        dp = lbl_8047D018 * currentDp;
    }

    if (filterId == 0) {
        rateDataId = pokemonSeikakuDataBiosGetReliveFightout(seikakuData);
    } else if (filterId == 1) {
        rateDataId = pokemonSeikakuDataBiosGetReliveWalk(seikakuData);
    } else if (filterId == 2) {
        rateDataId = pokemonSeikakuDataBiosGetReliveCall(seikakuData);
    } else if (filterId == 3) {
        rateDataId = pokemonSeikakuDataBiosGetReliveSodateya(seikakuData);
    } else if (filterId == 4) {
        rateDataId = pokemonSeikakuDataBiosGetReliveNadenade(seikakuData);
    }

    rateData = pokemonSeikakuRateDataBiosGetPtr((u8)rateDataId);
    if (rateData == NULL) {
        return;
    }

    kake = pokemonSeikakuRateDataBiosGetKake(rateData);
    waru = pokemonSeikakuRateDataBiosGetWaru(rateData);
    if (waru != 0) {
        dp *= (f32)kake;
        dp /= (f32)waru;
    } else {
        return;
    }

    if (ptr != NULL) {
        if (ptr == NULL) {
            currentDp = lbl_8047CFF0;
        } else {
            rawDp = pokemonGetStatus(ptr, 0, 0xC5, 0);
            currentDp = (f32)rawDp / lbl_8047CFF4;
        }
        newDp = currentDp + dp;
        if (newDp < lbl_8047CFF0) {
            newDp = lbl_8047CFF0;
        }
        if (ptr != NULL) {
            pokemonSetStatus(ptr, 0, 0xC5, 0,
                             (u32)(s32)(lbl_8047CFF4 * newDp));
        }
    }
}
/* 0x48 | pokemonSetDp | generic */
extern f32 lbl_8047CFF4;
#if 0
asm void pokemonSetDp(void) {
#include "src/game/gs_field_world_fn_8011FBCC.inc"
}
#else
void pokemonSetDp(u8* ptr, f32 f1) {
    extern void pokemonSetStatus(u8* ptr, u32 a, u32 b, u32 c, u32 d);
    if (ptr == NULL) { return; }
    pokemonSetStatus(ptr, 0, 0xc5, 0, (u32)(s32)(lbl_8047CFF4 * f1));
}
#endif
/* 0x60 | pokemonGetDp | generic */
extern f32 lbl_8047CFF0;
extern f64 lbl_8047D008;
extern f32 lbl_8047CFF4;
#if 0
asm void pokemonGetDp(void) {
#include "src/game/gs_field_world_fn_8011FC14.inc"
}
#else
f32 pokemonGetDp(u8* ptr) {
    extern s32 pokemonGetStatus(u8* ptr, u32 a, u32 b, u32 c);
    s32 val;
    if (ptr == NULL) { return lbl_8047CFF0; }
    val = pokemonGetStatus(ptr, 0, 0xc5, 0);
    return (f32)val / lbl_8047CFF4;
}
#endif
/* 0x8011FC74 | 0x30 */
u8 pokemonIsDarkPokemon(u32 arg) {
    extern u32 pokemonGetStatus(u32 a, u32 b, u32 c, u32 d);
    return (u8)pokemonGetStatus(arg, 0, 0xC2, 0);
}
/* 0x8011FCA4 | 0x124 */
extern void* fn_801EEEB8();
extern f64 lbl_8047D010;
extern f32 lbl_8047CFF4;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void pokemonSetDarkPokemonStatus(void);
/* 0x8011FDC8 | 0x504 */
extern void GScharCpy();
extern u16 fn_8010BBB8();
extern void fn_8001D994(void);
extern f64 lbl_8047D010;
extern f32 lbl_8047CFF0;
extern f64 lbl_8047D008;
extern f32 lbl_8047CFF4;
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void pokemonToMenuPokemonStatus();
/* 0x801202CC | 0x1DC */
extern f64 lbl_8047D010;
extern f32 lbl_8047CFF0;
extern f64 lbl_8047D008;
extern f32 lbl_8047CFF4;
static inline u32 pokemonGetLevelToExpInline(u32 pokemon, u8 level)
{
    u16 dataId;
    u8 growId;
    void* growData;

    if (pokemon == 0) {
        return 0;
    }
    dataId = (u16)pokemonGetStatus(pokemon, 0, 0x6E, 0);
    growId = (u8)pokemonGetStatus(0, dataId, 0x11, 0);
    growData = pokemonGrowDataBiosGetPtr(growId);
    if (growData == NULL) {
        return 0;
    }
    return pokemonGrowDataBiosGetExp(growData, level);
}

void pokemonToMenuPokemonStatus(u32 pokemon, u8* output)
{
    typedef struct MenuPokemonStatus {
        u8 name[0x17];
        u8 level;
        u16 maxHp;
        u16 hp;
        union {
            u32 integer;
            f32 real;
        } progressMax;
        union {
            u32 integer;
            f32 real;
        } progress;
        u16 conditionIcon;
        u16 displayValue;
        u8 sex;
        u8 darkState;
        u16 status82;
    } MenuPokemonStatus;
    extern u8 fn_8011B67C();
    extern u32 pokemonGetStatus();

#define MENU ((MenuPokemonStatus*)output)
    u8 level;
    u32 baseExp;
    u32 nextExp;
    u32 convert[2];
    u16 icon;

    pokemonGetStatus(pokemon, 0, 0x6E, 0);
    GScharCpy(MENU->name, pokemonGetStatus(pokemon, 0, 0x77, 0));
    MENU->level = (u8)pokemonGetStatus(pokemon, 0, 0x7A, 0);
    MENU->maxHp = (u16)pokemonGetStatus(pokemon, 0, 0x87, 0);
    MENU->hp = (u16)pokemonGetStatus(pokemon, 0, 0x83, 0);

    level = (u8)pokemonGetStatus(pokemon, 0, 0x7A, 0);
    baseExp = pokemonGetLevelToExpInline(pokemon, level);

    if ((u8)pokemonGetStatus(pokemon, 0, 0xC2, 0) == 0) {
        nextExp = pokemonGetLevelToExpInline(pokemon, ++level);
        MENU->progressMax.integer = nextExp - baseExp;
        MENU->progress.integer =
            pokemonGetStatus(pokemon, 0, 0x79, 0) - baseExp;
    } else {
        u16 darkMax;

        darkMax = (u16)(u32)fn_801EEEB8(
            (u16)pokemonGetStatus(pokemon, 0, 0xC3, 0));
        convert[0] = 0x43300000;
        convert[1] = darkMax;
        MENU->progressMax.real = *(f64*)convert - lbl_8047D010;
        if (pokemon == 0) {
            MENU->progress.real = lbl_8047CFF0;
        } else {
            convert[0] = 0x43300000;
            convert[1] =
                pokemonGetStatus(pokemon, 0, 0xC5, 0) ^ 0x80000000;
            MENU->progress.real =
                (*(f64*)convert - lbl_8047D008) / lbl_8047CFF4;
        }
    }

    if ((fn_80119ED0(3) == 0x7C || fn_80119ED0(3) == 0xC8) &&
        fn_8011B67C(pokemon, 3) == 1) {
        icon = 0x3A;
    } else if ((fn_80119ED0(4) == 0x7C || fn_80119ED0(4) == 0xC8) &&
               fn_8011B67C(pokemon, 4) == 1) {
        icon = 0x3A;
    } else if ((fn_80119ED0(5) == 0x7C || fn_80119ED0(5) == 0xC8) &&
               fn_8011B67C(pokemon, 5) == 1) {
        icon = 0x3B;
    } else if ((fn_80119ED0(6) == 0x7C || fn_80119ED0(6) == 0xC8) &&
               fn_8011B67C(pokemon, 6) == 1) {
        icon = 0x3C;
    } else if ((fn_80119ED0(7) == 0x7C || fn_80119ED0(7) == 0xC8) &&
               fn_8011B67C(pokemon, 7) == 1) {
        icon = 0x3D;
    } else if ((fn_80119ED0(8) == 0x7C || fn_80119ED0(8) == 0xC8) &&
               fn_8011B67C(pokemon, 8) == 1) {
        icon = 0x3E;
    } else {
        icon = 0;
    }
    MENU->conditionIcon = icon;
    MENU->displayValue = fn_8010BBB8(pokemon);
    MENU->sex = menuSubGetPokemonSexForFightDisp(pokemon);

    if ((u8)pokemonGetStatus(pokemon, 0, 0xC2, 0) == 0) {
        MENU->darkState = 0;
    } else if ((fn_80119ED0(0x3E) == 0x7C ||
                fn_80119ED0(0x3E) == 0xC8) &&
               fn_8011B67C(pokemon, 0x3E) != 0) {
        MENU->darkState = 2;
    } else {
        MENU->darkState = 1;
    }
    MENU->status82 = (u16)pokemonGetStatus(pokemon, 0, 0x82, 0);
#undef MENU
}

#if 0
asm void pokemonToMenuPokemonStatusSubBar(void) {
#include "src/game/gs_field_world_fn_801202CC.inc"
}

#else
void pokemonToMenuPokemonStatusSubBar(u8* ptr, u8* out) {
    extern u32 pokemonGetStatus(u8* ptr, u32 a, u32 b, u32 c);
    extern void* pokemonGrowDataBiosGetPtr(u8 idx);
    extern u32 pokemonGrowDataBiosGetExp(void* ptr, u8 idx);
    u8 idx;
    u32 base;
    void* table;
    u32 next;
    u32 sp[2];
    u32 val;
    f32 scale;

    idx = (u8)pokemonGetStatus(ptr, 0, 0x7A, 0);
    if (ptr == NULL) {
        base = 0;
    } else {
        table = pokemonGrowDataBiosGetPtr((u8)pokemonGetStatus(0, (u16)pokemonGetStatus(ptr, 0, 0x6E, 0), 0x11, 0));
        if (table == NULL) {
            base = 0;
        } else {
            base = pokemonGrowDataBiosGetExp(table, idx);
        }
    }

    if ((u8)pokemonGetStatus(ptr, 0, 0xC2, 0) == 0) {
        if (ptr == NULL) {
            next = 0;
        } else {
            table = pokemonGrowDataBiosGetPtr((u8)pokemonGetStatus(0, (u16)pokemonGetStatus(ptr, 0, 0x6E, 0), 0x11, 0));
            if (table == NULL) {
                next = 0;
            } else {
                next = pokemonGrowDataBiosGetExp(table, (u8)(idx + 1));
            }
        }
        *(u32*)(out + 0x1C) = next - base;
        *(u32*)(out + 0x20) = pokemonGetStatus(ptr, 0, 0x79, 0) - base;
    } else {
        val = (u16)(u32)fn_801EEEB8((u16)pokemonGetStatus(ptr, 0, 0xC3, 0));
        sp[0] = 0x43300000;
        sp[1] = val;
        *(f32*)(out + 0x1C) = *(f64*)sp - lbl_8047D010;
        if (ptr == NULL) {
            scale = lbl_8047CFF0;
        } else {
            sp[0] = 0x43300000;
            sp[1] = pokemonGetStatus(ptr, 0, 0xC5, 0) ^ 0x80000000;
            scale = (*(f64*)sp - lbl_8047D008) / lbl_8047CFF4;
        }
        *(f32*)(out + 0x20) = scale;
    }
}
#endif
/* 0x801204A8 | 0x1CC */
extern void GSmsgGetGSchar(void);
extern void fn_8010C4D4(void);
extern void fn_8010C46C(void);
#if 0
asm void pokemonToMenuWazaStatus(void) {
#include "src/game/gs_field_world_fn_801204A8.inc"
}
#else
void pokemonToMenuWazaStatus(u8* ptr, u8* out) {
    extern u32 GSmsgGetGSchar();
    extern u32 fn_8010C46C();
    extern u32 fn_8010C4D4();
    extern u32 wazaGetMaxPP();
    extern u32 wazaGetStatus();
    extern u32 pokemonGetStatus();
    u8 i;
    u8 valid;
    u32 slot2;
    u16 id;
    u16 resolved;
    u16 extra;
    u32 byte;
    u8* slot;

    memset(out, 0, 0x48);
    *(u32*)out = pokemonGetStatus(ptr, 0, 0x77, 0);
    for (i = 0; i < 4; i++) {
        slot = out + i * 0xC + 4;
        if (ptr == NULL) {
            valid = 0;
        } else if ((s32)pokemonGetStatus(ptr, 0, 0x7F, i) == 0) {
            valid = 0;
        } else if ((s32)pokemonGetStatus(ptr, 0, 0x7F, i) == 0x163) {
            valid = 0;
        } else {
            valid = 1;
        }
        if (valid == 0) {
            *(u32*)(slot + 0x0) = 0;
            *(u32*)(slot + 0x4) = 0;
            *(u8*)(slot + 0xA) = 0;
            *(u8*)(slot + 0xB) = 0;
        } else {
            id = (u16)pokemonGetStatus(ptr, 0, 0x7F, i);
            resolved = (u16)wazaGetStatus(0, id, 3, 0);
            *(u32*)(slot + 0x0) = GSmsgGetGSchar(wazaGetStatus(0, id, 1, 0));
            *(u32*)(slot + 0x4) = GSmsgGetGSchar(fn_8010C4D4(resolved));
            *(u16*)(slot + 0x8) = (u16)fn_8010C46C(resolved);
            if (ptr == NULL) {
                byte = 0;
            } else {
                slot2 = i + 4;
                extra = (u16)pokemonGetStatus(ptr, 0, 0x7F, slot2);
                byte = wazaGetMaxPP(extra, (u8)pokemonGetStatus(ptr, 0, 0x81, slot2));
            }
            *(u8*)(slot + 0xA) = byte;
            *(u8*)(slot + 0xB) = (u8)pokemonGetStatus(ptr, 0, 0x80, i);
        }
    }
}
#endif
/* 0x80120674 | 0x1F8 */
extern u32 fn_800E0C54(void);
extern u8 lbl_8027296C[];
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void pokemonCheckSetMonohiroi(void);
/* 0x8012086C | 0x294 */
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void pokemonAllKaihuku(void);
/* 0x80120B00 | 0x16C */
#if 0
asm void pokemonGetMezamerupower(void) {
#include "src/game/gs_field_world_fn_80120B00.inc"
}
#else
void pokemonGetMezamerupower(u8* ptr, u16* out1, u16* out2) {
    extern u32 pokemonGetStatus(u8*, u32, u32, u32);
    u16 v93 = (u16)pokemonGetStatus(ptr, 0, 0x93, 0);
    u16 v94 = (u16)pokemonGetStatus(ptr, 0, 0x94, 0);
    u16 v95 = (u16)pokemonGetStatus(ptr, 0, 0x95, 0);
    u16 v96 = (u16)pokemonGetStatus(ptr, 0, 0x96, 0);
    u16 v97 = (u16)pokemonGetStatus(ptr, 0, 0x97, 0);
    u16 v98 = (u16)pokemonGetStatus(ptr, 0, 0x98, 0);
    u8 hi = (u8)(((v93 & 2) >> 1) | (v94 & 2) | ((v95 << 1) & 4) |
                 ((v98 << 2) & 8) | ((v96 << 3) & 0x10) | ((v97 << 4) & 0x20));
    u8 lo = (u8)((v93 & 1) | ((v94 & 1) << 1) | ((v95 & 1) << 2) |
                 ((v98 & 1) << 3) | ((v96 & 1) << 4) | ((v97 & 1) << 5));
    u16 val1 = (u16)((s32)(hi * 0x28) / 0x3F + 0x1E);
    u32 val2 = (u8)((s32)(lo * 0xF) / 0x3F + 1);

    if ((u8)val2 >= 9) {
        val2 = (u16)((u8)val2 + 1);
    }
    if (out1 != NULL) {
        *out1 = val1;
    }
    if (out2 != NULL) {
        *out2 = (u16)val2;
    }
}
#endif
/* 0x80120CDC | 0x90 */
#if 0
asm void pokemonGetNowLevelToExp(void) {
#include "src/game/gs_field_world_fn_80120CDC.inc"
}
#else
u32 pokemonGetNowLevelToExp(u8* ptr) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    extern void* pokemonGrowDataBiosGetPtr(u32 val);
    extern u32 pokemonGrowDataBiosGetExp(void* a, u32 b);
    u8 byte1, byte2;
    void* result;
    byte1 = (u8)pokemonGetStatus(NULL, (u16)pokemonGetStatus(ptr, 0, 0x6e, 0), 0x11, 0);
    byte2 = (u8)pokemonGetStatus(ptr, 0, 0x7a, 0);
    result = pokemonGrowDataBiosGetPtr(byte1);
    if (result == NULL) { return 0; }
    return pokemonGrowDataBiosGetExp(result, byte2);
}
#endif
/* 0x80120DD0 | 0x210 */
#if 0
asm void pokemonGetJoutaiMsgId(void) {
#include "src/game/gs_field_world_fn_80120DD0.inc"
}
#else
u32 pokemonGetJoutaiMsgId(u8* ptr) {
    extern u16 fn_80119ED0(u32 val);
    extern u8 fn_8011B67C(u8* ptr, u32 val);
    extern void fn_80119F50(u32 val);
    u8 result;
    u32 acc = 0;
    if (ptr == NULL) { return 0; }

    if ((u16)fn_80119ED0(3) != 0x7c) { if ((u16)fn_80119ED0(3) != 0xc8) { result = 0; goto check1; } }
    result = fn_8011B67C(ptr, 3);
    check1: if ((u8)result == 1) { acc = 3; }

    if ((u16)fn_80119ED0(4) != 0x7c) { if ((u16)fn_80119ED0(4) != 0xc8) { result = 0; goto check2; } }
    result = fn_8011B67C(ptr, 4);
    check2: if ((u8)result == 1) { acc = 4; }

    if ((u16)fn_80119ED0(8) != 0x7c) { if ((u16)fn_80119ED0(8) != 0xc8) { result = 0; goto check3; } }
    result = fn_8011B67C(ptr, 8);
    check3: if ((u8)result == 1) { acc = 8; }

    if ((u16)fn_80119ED0(5) != 0x7c) { if ((u16)fn_80119ED0(5) != 0xc8) { result = 0; goto check4; } }
    result = fn_8011B67C(ptr, 5);
    check4: if ((u8)result == 1) { acc = 5; }

    if ((u16)fn_80119ED0(6) != 0x7c) { if ((u16)fn_80119ED0(6) != 0xc8) { result = 0; goto check5; } }
    result = fn_8011B67C(ptr, 6);
    check5: if ((u8)result == 1) { acc = 6; }

    if ((u16)fn_80119ED0(7) != 0x7c) { if ((u16)fn_80119ED0(7) != 0xc8) { result = 0; goto check6; } }
    result = fn_8011B67C(ptr, 7);
    check6: if ((u8)result == 1) { acc = 7; }

    fn_80119F50(acc);
}
#endif
/* 0x80120FE0 | 0x218 */
#if 0
asm void pokemonGetJoutaiMenuSpriteId(void) {
#include "src/game/gs_field_world_fn_80120FE0.inc"
}
#else
u32 pokemonGetJoutaiMenuSpriteId(u8* ptr) {
    extern u16 fn_80119ED0(u32 val);
    extern u32 fn_8011B67C(u8* ptr, u32 val);
    u32 result;
    if (ptr == NULL) { return 0; }

    if ((u16)fn_80119ED0(3) != 0x7c) { if ((u16)fn_80119ED0(3) != 0xc8) { result = 0; goto check3; } }
    result = fn_8011B67C(ptr, 3);
    check3: if ((u8)result == 1) { return 0x3a; }

    if ((u16)fn_80119ED0(4) != 0x7c) { if ((u16)fn_80119ED0(4) != 0xc8) { result = 0; goto check4; } }
    result = fn_8011B67C(ptr, 4);
    check4: if ((u8)result == 1) { return 0x3a; }

    if ((u16)fn_80119ED0(5) != 0x7c) { if ((u16)fn_80119ED0(5) != 0xc8) { result = 0; goto check5; } }
    result = fn_8011B67C(ptr, 5);
    check5: if ((u8)result == 1) { return 0x3b; }

    if ((u16)fn_80119ED0(6) != 0x7c) { if ((u16)fn_80119ED0(6) != 0xc8) { result = 0; goto check6; } }
    result = fn_8011B67C(ptr, 6);
    check6: if ((u8)result == 1) { return 0x3c; }

    if ((u16)fn_80119ED0(7) != 0x7c) { if ((u16)fn_80119ED0(7) != 0xc8) { result = 0; goto check7; } }
    result = fn_8011B67C(ptr, 7);
    check7: if ((u8)result == 1) { return 0x3d; }

    if ((u16)fn_80119ED0(8) != 0x7c) { if ((u16)fn_80119ED0(8) != 0xc8) { result = 0; goto check8; } }
    result = fn_8011B67C(ptr, 8);
    check8: if ((u8)result == 1) { return 0x3e; }

    return 0;
}
#endif
/* 0x801211F8 | 0x218 */
#if 0
asm void pokemonGetJoutaiDataId(void) {
#include "src/game/gs_field_world_fn_801211F8.inc"
}
#else
u32 pokemonGetJoutaiDataId(u8* ptr) {
    extern u16 fn_80119ED0(u32 val);
    extern u32 fn_8011B67C(u8* ptr, u32 val);
    u32 result;
    if (ptr == NULL) { return 0; }

    if ((u16)fn_80119ED0(3) != 0x7c) { if ((u16)fn_80119ED0(3) != 0xc8) { result = 0; goto check3; } }
    result = fn_8011B67C(ptr, 3);
    check3: if ((u8)result == 1) { return 0x3; }

    if ((u16)fn_80119ED0(4) != 0x7c) { if ((u16)fn_80119ED0(4) != 0xc8) { result = 0; goto check4; } }
    result = fn_8011B67C(ptr, 4);
    check4: if ((u8)result == 1) { return 0x4; }

    if ((u16)fn_80119ED0(5) != 0x7c) { if ((u16)fn_80119ED0(5) != 0xc8) { result = 0; goto check5; } }
    result = fn_8011B67C(ptr, 5);
    check5: if ((u8)result == 1) { return 0x5; }

    if ((u16)fn_80119ED0(6) != 0x7c) { if ((u16)fn_80119ED0(6) != 0xc8) { result = 0; goto check6; } }
    result = fn_8011B67C(ptr, 6);
    check6: if ((u8)result == 1) { return 0x6; }

    if ((u16)fn_80119ED0(7) != 0x7c) { if ((u16)fn_80119ED0(7) != 0xc8) { result = 0; goto check7; } }
    result = fn_8011B67C(ptr, 7);
    check7: if ((u8)result == 1) { return 0x7; }

    if ((u16)fn_80119ED0(8) != 0x7c) { if ((u16)fn_80119ED0(8) != 0xc8) { result = 0; goto check8; } }
    result = fn_8011B67C(ptr, 8);
    check8: if ((u8)result == 1) { return 0x8; }

    return 0;
}
#endif
/* 0x78 | fn_80121484 | status_guarded_call */
void fn_80121484(void* obj, u32 arg2, u32 arg3) {
    extern u16 fn_80119ED0();
    extern void fn_8011A0A8();
    if ((u16)fn_80119ED0(arg3) == 0x7C ||
        (u16)fn_80119ED0(arg3) == 0xC8) {
        fn_8011A0A8(obj, arg2, arg3);
    }
}
/* 0x78 | fn_801214FC | status_guarded_call */
void fn_801214FC(void* obj, u32 arg2, u32 arg3) {
    extern u16 fn_80119ED0();
    extern void fn_8011A280();
    if ((u16)fn_80119ED0(arg2) == 0x7C ||
        (u16)fn_80119ED0(arg2) == 0xC8) {
        fn_8011A280(obj, arg2, arg3);
    }
}
/* 0x78 | fn_801215E4 | status_guarded_call */
void fn_801215E4(void* obj, u32 arg2, u32 arg3) {
    extern u16 fn_80119ED0();
    extern void fn_8011A570();
    if ((u16)fn_80119ED0(arg2) == 0x7C ||
        (u16)fn_80119ED0(arg2) == 0xC8) {
        fn_8011A570(obj, arg2, arg3);
    }
}
/* 0x78 | fn_8012173C | status_guarded_call */
void fn_8012173C(void* obj, u32 arg2, u32 arg3) {
    extern u16 fn_80119ED0();
    extern void fn_8011A9EC();
    if ((u16)fn_80119ED0(arg2) == 0x7C ||
        (u16)fn_80119ED0(arg2) == 0xC8) {
        fn_8011A9EC(obj, arg2, arg3);
    }
}
/* 0x78 | fn_801217B4 | status_guarded_call */
void fn_801217B4(void* obj, u32 arg2, u32 arg3) {
    extern u16 fn_80119ED0();
    extern void fn_8011AB50();
    if ((u16)fn_80119ED0(arg2) == 0x7C ||
        (u16)fn_80119ED0(arg2) == 0xC8) {
        fn_8011AB50(obj, arg2, arg3);
    }
}
/* 0x78 | fn_8012190C | status_guarded_call */
void fn_8012190C(void* obj, u32 arg2, u32 arg3) {
    extern u16 fn_80119ED0();
    extern void fn_8011AFCC();
    if ((u16)fn_80119ED0(arg2) == 0x7C ||
        (u16)fn_80119ED0(arg2) == 0xC8) {
        fn_8011AFCC(obj, arg2, arg3);
    }
}
/* 0x78 | fn_801219F4 | status_guarded_call */
void fn_801219F4(void* obj, u32 arg2, u32 arg3) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    if ((u16)fn_80119ED0(arg2) == 0x7C ||
        (u16)fn_80119ED0(arg2) == 0xC8) {
        fn_8011B2C0(obj, arg2, arg3);
    }
}
/* 0x68 | fn_80121B4C | status_guarded_call */
void fn_80121B4C(void* obj, u32 arg2) {
    extern u16 fn_80119ED0();
    extern void fn_8011B788();
    if ((u16)fn_80119ED0(arg2) == 0x7C ||
        (u16)fn_80119ED0(arg2) == 0xC8) {
        fn_8011B788(obj, arg2);
    }
}
/* 0x64 | pokemonReplace | generic -- depends on pokemonBiosCopy signature */
void pokemonReplace(u32* arg1, u32* arg2) {
    u32 tmp[0x4e];
    if (arg1 == NULL) { return; }
    if (arg2 == NULL) { return; }
    pokemonBiosCopy(tmp, arg1);
    pokemonBiosCopy(arg1, arg2);
    pokemonBiosCopy(arg2, tmp);
}
/* 0x8011F5FC | 0x38 */
#ifndef PCPORT
typedef struct { u32 data[0x4E]; } GfwBuf0x138;
#endif
void pokemonBiosCopy(u32* dst, u32* src) {
#ifdef PCPORT
    u32 i;
#endif
    if (dst == NULL) { return; }
    if (src == NULL) { return; }
#ifdef PCPORT
    for (i = 0; i < 0x4E; i++) {
        dst[i] = src[i];
    }
#else
    *(GfwBuf0x138*)dst = *(GfwBuf0x138*)src;
#endif
}
/* 0x80121C18 | 0x428 */
extern u32 fn_801DE190(u32 idx, u32 base, u32 flag);
extern void fn_801DA3CC(void);
extern void fn_801DA36C(void);
extern u32 lbl_80478F90;  /* obj header ptr (SDA) */
#if 0
asm void pokemonCreateSequence(void) {
#include "src/game/gs_field_world_fn_80121C18.inc"
}
#else
void* pokemonCreateSequence(void* arg) {
    extern u32 pokemonGetStatus(void* obj, u32 a, u32 b, u32 c);
    extern u16 fn_80119ED0(u32 val);
    extern u32 fn_8011B67C(u8* ptr, u32 val);
    extern u32 fn_801DE190(u32 idx, u32 base, u32 flag);
    extern void fn_801DA36C(void* obj, u32 val);
    extern void fn_801DA3CC(void* obj, u32 val);
    u32 r30, r28, r29, r5, tmp, xv, tmp2;
    u32 result, acc;
    u8 valid;

    r30 = pokemonGetStatus(arg, 0, 0x6f, 0);
    r28 = (u16)pokemonGetStatus(arg, 0, 0x6e, 0);

    if (r28 == 0) {
        valid = 0;
    } else {
        if (pokemonGetStatus(NULL, r28, 1, 0) == 0) { valid = 0; }
        else if (r28 >= *(u32*)(u32)lbl_80478F90) { valid = 0; }
        else { valid = 1; }
    }
    if ((u8)valid == 0) { return 0; }

    r28 = pokemonGetStatus(NULL, r28, 0x66, 0);

    if (arg == NULL) {
        r5 = 0;
    } else {
        r29 = pokemonGetStatus(arg, 0, 0x75, 0);
        tmp = pokemonGetStatus(arg, 0, 0x6f, 0);
        xv = (r29 >> 16) ^ (r29 & 0xFFFF);
        tmp2 = (tmp >> 16) ^ (tmp & 0xFFFF);
        xv = xv ^ tmp2 ^ 8;
        xv = __cntlzw(xv);
        r5 = (u32)(8u << xv) >> 31;
    }

    r30 = fn_801DE190((u16)r28, r30, r5);

    if ((u16)fn_80119ED0(8) != 0x7c) { if ((u16)fn_80119ED0(8) != 0xc8) { result = 0; goto check8a; } }
    result = fn_8011B67C((u8*)arg, 8);
    check8a: if ((u8)result == 1) { fn_801DA3CC((void*)r30, 1); }

    if ((u16)fn_80119ED0(7) != 0x7c) { if ((u16)fn_80119ED0(7) != 0xc8) { result = 0; goto check7a; } }
    result = fn_8011B67C((u8*)arg, 7);
    check7a: if ((u8)result == 1) { fn_801DA3CC((void*)r30, 2); }

    if (arg == NULL) { acc = 0; goto done; }

    if ((u16)fn_80119ED0(3) != 0x7c) { if ((u16)fn_80119ED0(3) != 0xc8) { result = 0; goto check3; } }
    result = fn_8011B67C((u8*)arg, 3);
    check3: if ((u8)result == 1) { acc = 0; goto done; }

    if ((u16)fn_80119ED0(4) != 0x7c) { if ((u16)fn_80119ED0(4) != 0xc8) { result = 0; goto check4; } }
    result = fn_8011B67C((u8*)arg, 4);
    check4: if ((u8)result == 1) { acc = 0; goto done; }

    if ((u16)fn_80119ED0(5) != 0x7c) { if ((u16)fn_80119ED0(5) != 0xc8) { result = 0; goto check5; } }
    result = fn_8011B67C((u8*)arg, 5);
    check5: if ((u8)result == 1) { acc = 0; goto done; }

    if ((u16)fn_80119ED0(6) != 0x7c) { if ((u16)fn_80119ED0(6) != 0xc8) { result = 0; goto check6; } }
    result = fn_8011B67C((u8*)arg, 6);
    check6: if ((u8)result == 1) { acc = 0; goto done; }

    if ((u16)fn_80119ED0(7) != 0x7c) { if ((u16)fn_80119ED0(7) != 0xc8) { result = 0; goto check7b; } }
    result = fn_8011B67C((u8*)arg, 7);
    check7b: if ((u8)result == 1) { acc = 0; goto done; }

    if ((u16)fn_80119ED0(8) != 0x7c) { if ((u16)fn_80119ED0(8) != 0xc8) { result = 0; goto check8b; } }
    result = fn_8011B67C((u8*)arg, 8);
    check8b: if ((u8)result == 1) { acc = 0; goto done; }

    acc = 1;
    done:
    if ((u8)acc == 1) {
        fn_801DA36C((void*)r30, 1);
        fn_801DA36C((void*)r30, 2);
    }
    return (void*)r30;
}
#if 0
void fn_80121C18_old(void) {
    extern void fn_80119ED0();
    extern void fn_8011B67C();
    extern u32 pokemonGetStatus();
    extern void fn_801DA36C();
    extern void fn_801DA3CC();
    extern void fn_801DE190();
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    r4 = 0x0;
    r5 = 0x6f;
    r6 = 0x0;
    r31 = r3;
    pokemonGetStatus();
    r0 = r3;
    r3 = r31;
    r30 = r0;
    r4 = 0x0;
    r5 = 0x6e;
    r6 = 0x0;
    pokemonGetStatus();
    r28 = r3 & 0xFFFF;
    if ((s32)r0 == (s32)0) {
        r0 = 0x0;
        goto L_80121CB4;
    }
    r4 = r28;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x0;
    pokemonGetStatus();
    if (r3 == (u32)0x0) {
        r0 = 0x0;
        goto L_80121CB4;
    }
    r3 = lbl_80478F90;
    r0 = *(u32*)((u8*)r3 + 0x0);
    if (r28 >= (u32)r0) {
        r0 = 0x0;
        goto L_80121CB4;
    }
    r0 = 0x1;
L_80121CB4: ;
do {
    r0 = r0 & 0xFF;
    if (r28 == (u32)r0) {
        r3 = 0x0;
        return;
    }
    r4 = r28;
    r3 = 0x0;
    r5 = 0x66;
    r6 = 0x0;
    pokemonGetStatus();
    r28 = r3;
    if (r31 == (u32)0x0) {
        r5 = 0x0;
    } else {
        r3 = r31;
        r4 = 0x0;
        r5 = 0x75;
        r6 = 0x0;
        pokemonGetStatus();
        r29 = r3;
        r3 = r31;
        r4 = 0x0;
        r5 = 0x6f;
        r6 = 0x0;
        pokemonGetStatus();
        r4 = (u32)r29 >> 16;
        r0 = r29 & 0xFFFF;
        r5 = (u32)r3 >> 16;
        r6 = r3 & 0xFFFF;
        r0 = r4 ^ r0;
        r3 = 0x8;
        r0 = r5 ^ r0;
        r0 = r6 ^ r0;
        r0 = r3 ^ r0;
        r0 = __cntlzw(r0);
        r0 = r3 << r0;
        r5 = (u32)r0 >> 31;
    }
    r4 = r30;
    r3 = r28 & 0xFFFF;
    fn_801DE190();
    r0 = r3;
    r3 = 0x8;
    r30 = r0;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x7c || ((r3 = 0x8, fn_80119ED0(), r0 = r3 & 0xFFFF), r0 == (u32)0xc8)) {
        r3 = r31;
        r4 = 0x8;
        fn_8011B67C();
    } else {
        r3 = 0x0;
    }
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = 0x1;
        fn_801DA3CC();
    }
    r3 = 0x7;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x7c || ((r3 = 0x7, fn_80119ED0(), r0 = r3 & 0xFFFF), r0 == (u32)0xc8)) {
        r3 = r31;
        r4 = 0x7;
        fn_8011B67C();
    } else {
        r3 = 0x0;
    }
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = 0x2;
        fn_801DA3CC();
    }
    if (r31 == (u32)0x0) {
        r0 = 0x0;
        break;
    }
    r3 = 0x3;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x7c || ((r3 = 0x3, fn_80119ED0(), r0 = r3 & 0xFFFF), r0 == (u32)0xc8)) {
        r3 = r31;
        r4 = 0x3;
        fn_8011B67C();
    } else {
        r3 = 0x0;
    }
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;
        break;
    }
    r3 = 0x4;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x7c || ((r3 = 0x4, fn_80119ED0(), r0 = r3 & 0xFFFF), r0 == (u32)0xc8)) {
        r3 = r31;
        r4 = 0x4;
        fn_8011B67C();
    } else {
        r3 = 0x0;
    }
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;
        break;
    }
    r3 = 0x5;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x7c || ((r3 = 0x5, fn_80119ED0(), r0 = r3 & 0xFFFF), r0 == (u32)0xc8)) {
        r3 = r31;
        r4 = 0x5;
        fn_8011B67C();
    } else {
        r3 = 0x0;
    }
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;
        break;
    }
    r3 = 0x6;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x7c || ((r3 = 0x6, fn_80119ED0(), r0 = r3 & 0xFFFF), r0 == (u32)0xc8)) {
        r3 = r31;
        r4 = 0x6;
        fn_8011B67C();
    } else {
        r3 = 0x0;
    }
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;
        break;
    }
    r3 = 0x7;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x7c || ((r3 = 0x7, fn_80119ED0(), r0 = r3 & 0xFFFF), r0 == (u32)0xc8)) {
        r3 = r31;
        r4 = 0x7;
        fn_8011B67C();
    } else {
        r3 = 0x0;
    }
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;
        break;
    }
    r3 = 0x8;
    fn_80119ED0();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x7c || ((r3 = 0x8, fn_80119ED0(), r0 = r3 & 0xFFFF), r0 == (u32)0xc8)) {
        r3 = r31;
        r4 = 0x8;
        fn_8011B67C();
    } else {
        r3 = 0x0;
    }
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;
        break;
    }
    r0 = 0x1;
} while (0);
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = 0x1;
        fn_801DA36C();
        r3 = r30;
        r4 = 0x2;
        fn_801DA36C();
    }
    r3 = r30;
    return;
}
#endif
#endif
/* 0x80122040 | 0x2F4 */
#if 0
asm void pokemonSetSequenceStatus(void) {
#include "src/game/gs_field_world_fn_80122040.inc"
}
#else
void pokemonSetSequenceStatus(u8* ptr, void* obj) {
    extern u16 fn_80119ED0(u32 val);
    extern u32 fn_8011B67C(u8* ptr, u32 val);
    extern void fn_801DA36C(void* obj, u32 val);
    extern void fn_801DA3CC(void* obj, u32 val);
    u32 result;
    u32 acc;

    if ((u16)fn_80119ED0(8) != 0x7c) { if ((u16)fn_80119ED0(8) != 0xc8) { result = 0; goto check8a; } }
    result = fn_8011B67C(ptr, 8);
    check8a: if ((u8)result == 1) { fn_801DA3CC(obj, 1); }

    if ((u16)fn_80119ED0(7) != 0x7c) { if ((u16)fn_80119ED0(7) != 0xc8) { result = 0; goto check7a; } }
    result = fn_8011B67C(ptr, 7);
    check7a: if ((u8)result == 1) { fn_801DA3CC(obj, 2); }

    if (ptr == NULL) { acc = 0; goto done; }

    if ((u16)fn_80119ED0(3) != 0x7c) { if ((u16)fn_80119ED0(3) != 0xc8) { result = 0; goto check3; } }
    result = fn_8011B67C(ptr, 3);
    check3: if ((u8)result == 1) { acc = 0; goto done; }

    if ((u16)fn_80119ED0(4) != 0x7c) { if ((u16)fn_80119ED0(4) != 0xc8) { result = 0; goto check4; } }
    result = fn_8011B67C(ptr, 4);
    check4: if ((u8)result == 1) { acc = 0; goto done; }

    if ((u16)fn_80119ED0(5) != 0x7c) { if ((u16)fn_80119ED0(5) != 0xc8) { result = 0; goto check5; } }
    result = fn_8011B67C(ptr, 5);
    check5: if ((u8)result == 1) { acc = 0; goto done; }

    if ((u16)fn_80119ED0(6) != 0x7c) { if ((u16)fn_80119ED0(6) != 0xc8) { result = 0; goto check6; } }
    result = fn_8011B67C(ptr, 6);
    check6: if ((u8)result == 1) { acc = 0; goto done; }

    if ((u16)fn_80119ED0(7) != 0x7c) { if ((u16)fn_80119ED0(7) != 0xc8) { result = 0; goto check7b; } }
    result = fn_8011B67C(ptr, 7);
    check7b: if ((u8)result == 1) { acc = 0; goto done; }

    if ((u16)fn_80119ED0(8) != 0x7c) { if ((u16)fn_80119ED0(8) != 0xc8) { result = 0; goto check8b; } }
    result = fn_8011B67C(ptr, 8);
    check8b: if ((u8)result == 1) { acc = 0; goto done; }

    acc = 1;
    done:
    if ((u8)acc == 1) {
        fn_801DA36C(obj, 1);
        fn_801DA36C(obj, 2);
    }
}
#endif
/* 0x80122370 | 0x360 */
extern void fn_80135530(void);
extern u32 lbl_80478F90;  /* obj header ptr (SDA) */
void pokemonGetFriendFormPokemonFriendFilterId(u8* obj, u16 item_id, u32 filter_id)
{
    extern u32 pokemonGetDarkPokemonLevel(u8* obj);
    extern u32 gamedataAttestCheckValid(u32 value);
    s8 raw_delta;
    s32 friendship;
    s32 delta;
    s32 adjusted_delta;
    u8 valid;
    u8 tier;

    if (obj == NULL) {
        return;
    }

    if (obj == NULL) {
        valid = 0;
    } else {
        friendship = (u16)pokemonGetStatus(obj, 0, 0x6E, 0);
        if (friendship == 0) {
            valid = 0;
        } else {
            if (friendship == 0) {
                valid = 0;
            } else if (pokemonGetStatus(NULL, friendship, 1, 0) == 0) {
                valid = 0;
            } else if ((u32)friendship >= *(u32*)lbl_80478F90) {
                valid = 0;
            } else {
                valid = 1;
            }
            if (valid == 0) {
                valid = 0;
            } else if ((u8)gamedataAttestCheckValid(
                           pokemonGetStatus(obj, 0, 0x70, 0)) == 0) {
                valid = 0;
            } else if ((u8)pokemonGetStatus(obj, 0, 0xB8, 0) == 1) {
                valid = 0;
            } else {
                valid = 1;
            }
        }
    }
    if (valid == 0) {
        valid = 0;
    } else if ((s32)pokemonGetStatus(obj, 0, 0x6E, 0) == 0x19C) {
        valid = 0;
    } else if ((u8)pokemonGetStatus(obj, 0, 0xB6, 0) == 1) {
        valid = 0;
    } else {
        if ((u16)filter_id == 6) {
            goto filter_valid;
        }
        if ((u16)filter_id == 7) {
            goto filter_valid;
        }
        if ((u16)filter_id == 8) {
            goto filter_valid;
        }
        if ((u8)pokemonGetStatus(obj, 0, 0x7B, 0) == 1) {
            valid = 0;
            goto validity_done;
        }
filter_valid:
        valid = 1;
    }
validity_done:
    if (valid == 0) {
        return;
    }

    friendship = (s32)pokemonGetStatus(obj, 0, 0x99, 0);
    if ((u8)pokemonGetStatus(obj, 0, 0xC2, 0) == 1) {
        friendship += (s32)pokemonGetStatus(obj, 0, 0xC7, 0);
    }

    tier = 0;
    if (friendship >= 100) {
        tier = 1;
    }
    if (friendship >= 200) {
        tier = (u8)((u8)tier + 1);
    }

    if ((u16)filter_id == 5) {
        if (fn_800E0C54() & 1) {
            return;
        }
    }

    raw_delta = pokemonFriendFilterDataBiosGetValue(
        pokemonFriendFilterDataBiosGetPtr(filter_id), tier);
    delta = raw_delta;
    if (delta > 0) {
        if (item_id == 0x1B) {
            delta = (delta * 150) / 100;
        }
    }

    adjusted_delta = delta;
    if (delta > 0) {
        if ((u16)pokemonGetStatus(obj, 0, 0x73, 0) == 0xB) {
            adjusted_delta = delta + 1;
        }
    }

    if ((u8)pokemonGetStatus(obj, 0, 0xC2, 0) == 1) {
        s32 new_value;

        if ((u8)pokemonGetDarkPokemonLevel(obj) < 3) {
            return;
        }
        new_value = (s32)pokemonGetStatus(obj, 0, 0xC7, 0);
        new_value += adjusted_delta;
        if (new_value < 0) {
            new_value = 0;
        }
        if (new_value > 0xFF) {
            new_value = 0xFF;
        }
        pokemonSetStatus(obj, 0, 0xC7, 0, (u32)new_value);
        return;
    }

    friendship += adjusted_delta;
    if (friendship < 0) {
        friendship = 0;
    }
    if (friendship > 0xFF) {
        friendship = 0xFF;
    }
    pokemonSetStatus(obj, 0, 0x99, 0, (u32)friendship);
}
/* 0x801226D0 | 0x324 */
extern u8 lbl_80272948[];
extern u32 lbl_80478F90;  /* obj header ptr (SDA) */
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void pokemonGetEffortFromPokemon(void);
/* 0x80122BC0 | 0xA4 */
#if 0
asm void pokemonIsNokoriHpFollowing(void) {
#include "src/game/gs_field_world_fn_80122BC0.inc"
}
#else
#pragma optimization_level 4
u8 pokemonIsNokoriHpFollowing(u8* ptr, s32 b) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    s32 val1;
    s32 val2;

    if (!(u16)b) {
        return 0;
    }
    if (ptr == NULL) {
        return 0;
    }
    val1 = (s32)(u16)pokemonGetStatus(ptr, 0, 0x83, 0);
    val2 = (s32)(u16)pokemonGetStatus(ptr, 0, 0x87, 0);
    return (u8)((s32)val2 / (s32)(u16)b >= val1);
}
#endif
/* 0x80122C64 | 0x178 */
#if 0
asm void pokemonIsJoutaiKaragenki(void) {
#include "src/game/gs_field_world_fn_80122C64.inc"
}
#else
u32 pokemonIsJoutaiKaragenki(u8* ptr) {
    extern u16 fn_80119ED0(u32 val);
    extern u8 fn_8011B67C(u8* ptr, u32 val);
    u8 result;
    if (ptr == NULL) { return 0; }

    if ((u16)fn_80119ED0(3) != 0x7c) { if ((u16)fn_80119ED0(3) != 0xc8) { result = 0; goto check1; } }
    result = fn_8011B67C(ptr, 3);
    check1: if ((u8)result == 1) { return 1; }

    if ((u16)fn_80119ED0(5) != 0x7c) { if ((u16)fn_80119ED0(5) != 0xc8) { result = 0; goto check2; } }
    result = fn_8011B67C(ptr, 5);
    check2: if ((u8)result == 1) { return 1; }

    if ((u16)fn_80119ED0(6) != 0x7c) { if ((u16)fn_80119ED0(6) != 0xc8) { result = 0; goto check3; } }
    result = fn_8011B67C(ptr, 6);
    check3: if ((u8)result == 1) { return 1; }

    if ((u16)fn_80119ED0(4) != 0x7c) { if ((u16)fn_80119ED0(4) != 0xc8) { result = 0; goto check4; } }
    result = fn_8011B67C(ptr, 4);
    check4: if ((u8)result == 1) { return 1; }

    return 0;
}
#endif
/* 0x80122DDC | 0x218 */
#if 0
asm void pokemonIsJoutaiNormal(void) {
#include "src/game/gs_field_world_fn_80122DDC.inc"
}
#else
u32 pokemonIsJoutaiNormal(u8* ptr) {
    extern u16 fn_80119ED0(u32 val);
    extern u32 fn_8011B67C(u8* ptr, u32 val);
    u32 result;
    if (ptr == NULL) { return 0; }

    if ((u16)fn_80119ED0(3) != 0x7c) { if ((u16)fn_80119ED0(3) != 0xc8) { result = 0; goto check3; } }
    result = fn_8011B67C(ptr, 3);
    check3: if ((u8)result == 1) { return 0x0; }

    if ((u16)fn_80119ED0(4) != 0x7c) { if ((u16)fn_80119ED0(4) != 0xc8) { result = 0; goto check4; } }
    result = fn_8011B67C(ptr, 4);
    check4: if ((u8)result == 1) { return 0x0; }

    if ((u16)fn_80119ED0(5) != 0x7c) { if ((u16)fn_80119ED0(5) != 0xc8) { result = 0; goto check5; } }
    result = fn_8011B67C(ptr, 5);
    check5: if ((u8)result == 1) { return 0x0; }

    if ((u16)fn_80119ED0(6) != 0x7c) { if ((u16)fn_80119ED0(6) != 0xc8) { result = 0; goto check6; } }
    result = fn_8011B67C(ptr, 6);
    check6: if ((u8)result == 1) { return 0x0; }

    if ((u16)fn_80119ED0(7) != 0x7c) { if ((u16)fn_80119ED0(7) != 0xc8) { result = 0; goto check7; } }
    result = fn_8011B67C(ptr, 7);
    check7: if ((u8)result == 1) { return 0x0; }

    if ((u16)fn_80119ED0(8) != 0x7c) { if ((u16)fn_80119ED0(8) != 0xc8) { result = 0; goto check8; } }
    result = fn_8011B67C(ptr, 8);
    check8: if ((u8)result == 1) { return 0x0; }

    return 1;
}
#endif
/* 0x80122FF4 | 0x9C */
extern void fn_80008154(void);
extern void fn_80142CF4(void);
#if 0
asm void pokemonGetSoubiItemBuff(void) {
#include "src/game/gs_field_world_fn_80122FF4.inc"
}
#else
u16 pokemonGetSoubiItemBuff(u8* ptr) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    extern u32 fn_80008154(void);
    extern u32 itemGetStatus(u32 a, u32 b, u32 c, u32 d);
    u16 val;
    u16 check;
    val = (u16)pokemonGetStatus(ptr, 0, 0x82, 0);
    if (val == 0) { return 0; }
    if ((u8)fn_80008154() == 1) {
        check = (u16)itemGetStatus(0, val, 7, 0);
        if (check == 0x1a || check == 0x1e) {
            return 0x63;
        }
    }
    return (u16)itemGetStatus(0, val, 0xa, 0);
}
#endif
/* 0x801230E0 | 0x30 */
u16 pokemonGetSoubiItemDataId(u32 arg) {
    extern u32 pokemonGetStatus(u32 a, u32 b, u32 c, u32 d);
    return (u16)pokemonGetStatus(arg, 0, 0x82, 0);
}
/* 0x80123110 | 0x94 */
#if 0
asm void pokemonDoItemSoubi(void) {
#include "src/game/gs_field_world_fn_80123110.inc"
}
#else
u32 pokemonDoItemSoubi(u8* ptr, register u32 arg2, u8 flag) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    extern void pokemonSetStatus(u8* a, u32 b, u32 c, u32 d, u16 e);
    if (ptr == NULL) { return 0; }
    if ((u8)flag == 0) {
        arg2 = (u16)pokemonGetStatus(ptr, 0, 0x82, 0);
        pokemonSetStatus(ptr, 0, 0x82, 0, 0);
    } else {
        pokemonSetStatus(ptr, 0, 0x82, 0, arg2);
    }
    return arg2;
}
#endif
/* 0x801231A4 | 0x13C */
extern u32 sexGetPokemonSexRaitoKotei(u32);
#if 0
asm void pokemonGetSex(void) {
#include "src/game/gs_field_world_fn_801231A4.inc"
}
#else
u8 pokemonGetSex(u8* ptr)
{
    u32 count;
    u16 threshold;
    u16 wanted;
    s32 result;

    if (ptr == NULL) {
        return 2;
    }

    count = pokemonGetStatus(ptr, 0, 0x6F, 0);
    threshold = (u16)pokemonGetStatus(0, (u16)pokemonGetStatus(ptr, 0, 0x6E, 0), 0x13, 0);
    if (ptr == NULL) {
        result = 2;
    } else {
        wanted = (u16)pokemonGetStatus(0, (u16)pokemonGetStatus(ptr, 0, 0x6E, 0), 0x13, 0);
        if ((s32)wanted == (s32)(u8)sexGetPokemonSexRaitoKotei(0)) {
            result = 0;
        } else if ((s32)wanted == (s32)(u8)sexGetPokemonSexRaitoKotei(1)) {
            result = 1;
        } else if ((s32)wanted == (s32)(u8)sexGetPokemonSexRaitoKotei(2)) {
            result = 2;
        } else {
            result = -1;
        }
    }
    if ((s8)result >= 0) {
        goto done;
    }
    if ((u16)threshold <= (u16)(u8)count) {
        goto zero;
    }
    result = 1;
    goto done;
zero:
    result = 0;
done:
    return (u8)result;
}
#endif
/* 0x801232E0 | 0x88 */
extern void fn_801EE958(void);
extern void fn_801EEB34(void);
#if 0
asm void pokemonSetOnDarkPokemonFlag(void) {
#include "src/game/gs_field_world_fn_801232E0.inc"
}
#else
void pokemonSetOnDarkPokemonFlag(u8* ptr, u8 flag) {
    extern u32 pokemonGetStatus(u8* ptr, u32 a, u32 b, u32 c);
    extern void fn_801EE958(u16 val, u32 a);
    extern void fn_801EEB34(u16 val, u32 a);
    u16 val;
    if (ptr == NULL) { return; }
    if ((u8)pokemonGetStatus(ptr, 0, 0xc2, 0) == 0) { return; }
    val = (u16)pokemonGetStatus(ptr, 0, 0xc3, 0);
    fn_801EE958(val, 1);
    if ((u8)flag == 0) { return; }
    fn_801EEB34(val, 1);
}
#endif
/* 0x80123368 | 0x8C */
extern void memoDataSet();
#if 0
asm void pokemonSetOnZukanFlag(void) {
#include "src/game/gs_field_world_fn_80123368.inc"
}
#else
void pokemonSetOnZukanFlag(u8* ptr, u8 flag) {
    extern u32 pokemonGetStatus(u8* ptr, u32 a, u32 b, u32 c);
    extern void memoDataSet(u32 a, u8* b);
    extern void pokemonSetStatus(u8* ptr, u32 a, u32 b, u32 c, u32 d);
    if (ptr == NULL) { return; }
    pokemonGetStatus(ptr, 0, 0x6e, 0);
    memoDataSet(0, ptr);
    pokemonSetStatus(ptr, 0, 0x62, 0, 1);
    if ((u8)flag == 0) { return; }
    pokemonSetStatus(ptr, 0, 0x63, 0, 1);
}
#endif
/* 0x801233F4 | 0x190 */
extern u32 lbl_80478F90;  /* obj header ptr (SDA) */
typedef struct PokemonRangeObjHeader {
    u32 count;
} PokemonRangeObjHeader;

u32 pokemonCheckFightOut(u8* ptr) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    extern u32 gamedataAttestCheckValid(u32 val);
    PokemonRangeObjHeader* header;
    u16 val;
    u8 flag;

    if (ptr == NULL) {
        flag = 0;
    } else {
        val = (u16)pokemonGetStatus(ptr, 0, 0x6e, 0);
        if (val == 0) {
            flag = 0;
        } else {
            if (val == 0) {
                flag = 0;
            } else if ((u32)pokemonGetStatus(NULL, val, 1, 0) == 0) {
                flag = 0;
            } else if (val >= (header = (PokemonRangeObjHeader*)lbl_80478F90)->count) {
                flag = 0;
            } else {
                flag = 1;
            }
            if ((u8)flag == 0) {
                flag = 0;
            } else if ((u8)gamedataAttestCheckValid(pokemonGetStatus(ptr, 0, 0x70, 0)) == 0) {
                flag = 0;
            } else if ((u8)pokemonGetStatus(ptr, 0, 0xb8, 0) == 1) {
                flag = 0;
            } else {
                flag = 1;
            }
        }
    }
    if ((u8)flag == 0) { return 0; }
    if ((s32)pokemonGetStatus(ptr, 0, 0x6e, 0) == 0x19c) { return 0; }
    if ((u8)pokemonGetStatus(ptr, 0, 0xb6, 0) == 1) { return 0; }
    {
        u32 value = (u8)pokemonGetStatus(ptr, 0, 0x7b, 0);
        return ((1 - value) | (value - 1)) >> 31;
    }
}
/* 0x80123584 | 0x98 */
#if 0
asm void pokemonGetOboeWazaDataBanme(void) {
#include "src/game/gs_field_world_fn_80123584.inc"
}
#else
u32 pokemonGetOboeWazaDataBanme(u8* ptr, u32 arg2) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    u32 val;
    u8 target;
    u32 i;
    if (ptr == NULL) { return 0; }
    val = pokemonGetStatus(ptr, 0, 0x6e, 0) & 0xFFFF;
    target = (u8)arg2;
    i = 0;
    while ((u8)i < 0x14) {
        if ((s32)pokemonGetStatus(NULL, val, 0x1d, i & 0xFF) == (s32)target) { break; }
        i++;
    }
    return i;
}
#endif
inline u32 inline_fn(u16 val, u8* counter_ptr) {
    return pokemonGetStatus(NULL, val, 0x1e, *counter_ptr);
}

/* 0x8012361C | 0xDC */
#if 0
asm void pokemonOboeWaza(void) {
#include "src/game/gs_field_world_fn_8012361C.inc"
}
#else
s32 pokemonOboeWaza(u8* ptr, u8 target, u8* buf_ptr, u8* counter_ptr) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    extern s32 pokemonSetWazaStatus(u8* a, u16 b, u8* c);
    u8* buf = buf_ptr;
    u8* counter = counter_ptr;
    u8 waza = target;
    u16 val;
    u16 result;
    if (ptr == NULL) { return -2; }
    if (ptr == NULL || counter == NULL) {
        result = 0;
        goto _check;
    }
    val = pokemonGetStatus(ptr, 0, 0x6e, 0) & 0xFFFF;
    while (*counter < 0x14) {
        if ((s32)pokemonGetStatus(NULL, val, 0x1d, *counter) == (s32)waza) {
            result = (u16)inline_fn(val, counter);
            goto _check;
        }
        (*counter)++;
    }
    result = 0;
_check:
    if ((u16)result == 0) { return -3; }
    return pokemonSetWazaStatus(ptr, result, buf);
}
#endif
/* 0x801236F8 | 0xC0 */
#if 0
asm void pokemonGetOboeWazaDataId(void) {
#include "src/game/gs_field_world_fn_801236F8.inc"
}
#else
u16 pokemonGetOboeWazaDataId(u8* ptr, u8 arg2, u8* counter_ptr) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    u16 val;
    if (ptr == NULL || counter_ptr == NULL) { return 0; }
    val = (u16)pokemonGetStatus(ptr, 0, 0x6e, 0);
    arg2 = (0, (u8)arg2);
    while (*counter_ptr < 0x14) {
        if ((s32)pokemonGetStatus(NULL, val, 0x1d, *counter_ptr) == (s32)arg2) {
            return (u16)inline_fn(val, counter_ptr);
        }
        (*counter_ptr)++;
    }
    return 0;
}
#endif
/* 0x801237B8 | 0x3A4 */
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void pokemonSetWazaStatus(void);
/* 0x80123B5C | 0xF8 */
#if 0
asm void pokemonSearchWazaDataId(void) {
#include "src/game/gs_field_world_fn_80123B5C.inc"
}
#else
s32 pokemonSearchWazaDataId(u8* ptr, u16 target) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    s8 i;
    s32 ext_i;
    u8 flag;
    if (ptr == NULL) { return -1; }
    for (i = 0; (s8)i < 4; i++) {
        ext_i = (s8)i;
        if (ptr == NULL) {
            flag = 0;
        } else {
            if ((s32)pokemonGetStatus(ptr, 0, 0x7f, ext_i) == 0) {
                flag = 0;
            } else if ((s32)pokemonGetStatus(ptr, 0, 0x7f, ext_i) == 0x163) {
                flag = 0;
            } else {
                flag = 1;
            }
        }
        if ((u8)flag != 0) {
            if ((s32)pokemonGetStatus(ptr, 0, 0x7f, (s8)i) == (s32)target) {
                return i;
            }
        }
    }
    return -1;
}
#endif
/* 0x80 | pokemonWazaReplace | generic */
void pokemonWazaReplace(void* ptr, u32 idx, u32 arg) {
    extern u32* pokemonBiosGetPokemonWazaPtr(void* a, u32 b, u32 c);
    extern void pokemonWazaBiosCopy(u32* dst, u32* src);
    u32 tmp;
    u32* val1;
    u32* val2;
    if (ptr == NULL) { return; }
    val1 = pokemonBiosGetPokemonWazaPtr(ptr, idx, 0);
    val2 = pokemonBiosGetPokemonWazaPtr(ptr, arg, 0);
    pokemonWazaBiosCopy(&tmp, val1);
    pokemonWazaBiosCopy(val1, val2);
    pokemonWazaBiosCopy(val2, &tmp);
}
/* 0x80123CD4 | 0x84 */
#if 0
asm void pokemonWazaCheckValid(void) {
#include "src/game/gs_field_world_fn_80123CD4.inc"
}
#else
u32 pokemonWazaCheckValid(u8* ptr, u32 arg2) {
    extern u32 pokemonGetStatus(u8* ptr, u32 a, u32 b, u32 c);
    if (ptr == NULL) { return 0; }
    if ((s32)pokemonGetStatus(ptr, 0, 0x7f, arg2) == 0) { return 0; }
    return pokemonGetStatus(ptr, 0, 0x7f, arg2) != 0x163;
}
#endif
/* 0x80123D58 | 0x118 */
#if 0
asm void pokemonWazaCreate(void) {
#include "src/game/gs_field_world_fn_80123D58.inc"
}
#else
void pokemonWazaCreate(u8* ptr, u32 slot, u32 val) {
    extern void pokemonSetStatus(u8* a, u32 b, u32 c, u32 d, u32 e);
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    extern u32 wazaGetMaxPP(u16 a, u8 b);
    u16 slot2;
    u16 result;
    u8 byte;
    if (ptr == NULL) { return; }
    if (ptr != NULL) {
        pokemonSetStatus(ptr, 0, 0x7f, slot, 0);
        pokemonSetStatus(ptr, 0, 0x80, slot, 0);
        pokemonSetStatus(ptr, 0, 0x81, slot, 0);
    }
    pokemonSetStatus(ptr, 0, 0x7f, slot, val);
    if (ptr == NULL) {
        byte = 0;
    } else {
        slot2 = (u16)slot + 4;
        result = (u16)pokemonGetStatus(ptr, 0, 0x7f, slot2);
        byte = (u8)wazaGetMaxPP(result, (u8)pokemonGetStatus(ptr, 0, 0x81, slot2));
    }
    pokemonSetStatus(ptr, 0, 0x80, slot, (u8)byte);
}
#endif
/* 0x80123EF0 | 0xCC */
#if 0
asm void pokemonSetCatchStatus(void) {
#include "src/game/gs_field_world_fn_80123EF0.inc"
}
#else
void pokemonSetCatchStatus(u8* arg1, u32 arg2, u8 arg3, u16 arg4, u8 arg5, u32 arg6, u32 arg7) {
    extern void pokemonSetStatus(u8* ptr, u32 a, u32 b, u32 c, u32 d);
    if (arg1 == NULL) { return; }
    pokemonSetStatus(arg1, 0, 0x71, 0, arg2);
    pokemonSetStatus(arg1, 0, 0x72, 0, (u32)arg3);
    pokemonSetStatus(arg1, 0, 0x73, 0, (u32)arg4);
    pokemonSetStatus(arg1, 0, 0x74, 0, (u32)arg5);
    pokemonSetStatus(arg1, 0, 0x75, 0, arg6);
    pokemonSetStatus(arg1, 0, 0x76, 0, arg7);
}
#endif
/* 0x80123FBC | 0x108 */
extern u32 lbl_80478F90;  /* obj header ptr (SDA) */
#if 0
asm void pokemonCheckValid(void) {
#include "src/game/gs_field_world_fn_80123FBC.inc"
}
#else
u8 pokemonCheckValid(u8* ptr) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    extern u32 gamedataAttestCheckValid(u32 val);
    u16 val;
    u8 flag;
    if (ptr == NULL) { return 0; }
    val = (u16)pokemonGetStatus(ptr, 0, 0x6e, 0);
    if (val == 0) { return 0; }
    flag = 0;
    if (val == 0) {
        flag = 0;
    } else {
        if ((u32)pokemonGetStatus(NULL, val, 1, 0) == 0) { flag = 0; }
        else if (val >= *(u32*)(u32)lbl_80478F90) { flag = 0; }
        else { flag = 1; }
    }
    if ((u8)flag == 0) { return 0; }
    if ((u8)gamedataAttestCheckValid(pokemonGetStatus(ptr, 0, 0x70, 0)) == 0) { return 0; }
    return (u8)pokemonGetStatus(ptr, 0, 0xb8, 0) != 1;
}
#endif
/* 0x801240C4 | 0x34C */
extern void gamedataAttestBiosCopy(void);
extern u32 lbl_80478F90;  /* obj header ptr (SDA) */
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void pokemonCreate(void);
/* 0x80124410 | 0x4B4 */
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
u32 pokemonCreateRndFit(u8*, s32, s32, s32, u32);
/* 0x801248C4 | 0xB4 */
static u32 pokemonCreateRndFitRand32(void) {
    u32 hi;
    u32 lo;

    hi = fn_800E0C54() << 16;
    lo = fn_800E0C54() & 0xFFFF;
    return hi | lo;
}

static s32 pokemonCreateRndFitCurrentGroup(u8* ptr) {
    extern u8 fn_80131574(u32);
    u16 species;
    u16 group;

    if (ptr == NULL) {
        return 2;
    }

    species = (u16)pokemonGetStatus(ptr, 0, 0x6E, 0);
    group = (u16)pokemonGetStatus(NULL, species, 0x13, 0);

    if ((s32)group == (s32)(u8)fn_80131574(0)) {
        return 0;
    }
    if ((s32)group == (s32)(u8)fn_80131574(1)) {
        return 1;
    }
    if ((s32)group == (s32)(u8)fn_80131574(2)) {
        return 2;
    }
    return -1;
}

static u8 pokemonCreateRndFitGroupOrRandom(u8* ptr, u32 value) {
    s32 group;
    u16 species;
    u16 threshold;

    group = pokemonCreateRndFitCurrentGroup(ptr);
    if (group >= 0) {
        return (u8)group;
    }

    species = (u16)pokemonGetStatus(ptr, 0, 0x6E, 0);
    threshold = (u16)pokemonGetStatus(NULL, species, 0x13, 0);
    if ((u32)threshold > (value & 0xFF)) {
        return 1;
    }
    return 0;
}

static u8 pokemonCreateRndFitSeedHit(u32 value, u32 seed_xor) {
    u32 x;

    x = (value >> 16) ^ (value & 0xFFFF) ^ seed_xor ^ 8;
    return (x >= 8 && x < 16) ? 1 : 0;
}

u32 pokemonCreateRndFit(u8* ptr, s32 group_arg, s32 mod_arg, s32 seed_mode_arg, u32 seed) {
    s8 group_filter;
    s8 mod_filter;
    s8 seed_filter;
    u32 seed_xor;

    if (ptr == NULL) {
        return pokemonCreateRndFitRand32();
    }

    group_filter = (s8)group_arg;
    mod_filter = (s8)mod_arg;
    seed_filter = (s8)seed_mode_arg;
    seed_xor = (seed >> 16) ^ (seed & 0xFFFF);

    for (;;) {
        u32 value;

        value = pokemonCreateRndFitRand32();

        if (group_filter >= 0) {
            s32 group;

            group = pokemonCreateRndFitCurrentGroup(ptr);
            if (group < 0) {
                if (group_filter != 2) {
                    if ((u8)group_filter != pokemonCreateRndFitGroupOrRandom(ptr, value)) {
                        continue;
                    }
                } else {
                    u32 current;
                    u8 lhs;
                    u8 rhs;

                    current = pokemonGetStatus(ptr, 0, 0x6F, 0);
                    lhs = pokemonCreateRndFitGroupOrRandom(ptr, current);
                    rhs = pokemonCreateRndFitGroupOrRandom(ptr, value);
                    if (lhs != rhs) {
                        continue;
                    }
                }
            }
        }

        if (mod_filter >= 0 && (u8)mod_filter != (u8)(value % 25)) {
            continue;
        }

        if (seed_filter >= 0) {
            u8 hit;

            hit = pokemonCreateRndFitSeedHit(value, seed_xor);
            if (seed_filter != 0) {
                if (hit == 0) {
                    continue;
                }
            } else if (hit == 1) {
                continue;
            }
        }

        return value;
    }
}

u16 pokemonGetTokuseiDataId(u8* ptr);
/* body moved to pokemon_range_exact_801248C4.c: pokemonGetTokuseiDataId */
void pokemonInitAry(u8* ptr, u16 count);
/* body moved to pokemon_range_exact_801248C4.c: pokemonInitAry */
void pokemonInitDarkPokemon(u8* ptr);
/* body moved to pokemon_range_exact_801248C4.c: pokemonInitDarkPokemon */
void pokemonInitJoutai(u8* ptr);
/* body moved to pokemon_range_exact_801248C4.c: pokemonInitJoutai */
void pokemonWazaInit(u8* ptr, u32 arg2);
/* body moved to pokemon_range_exact_801248C4.c: pokemonWazaInit */
u32 pokemonCheckRare(void* ctx);
/* body moved to pokemon_range_exact_801248C4.c: pokemonCheckRare */
void pokemonGrowBasisStatus(void* ptr, u32 arg2);
/* body moved to pokemon_range_exact_801248C4.c: pokemonGrowBasisStatus */
/* body moved to pokemon_range_exact_801248C4.c: pokemonResetBasisStatus */
/* 0x8012795C | 0x700 */
extern u8 pokemonSeikakuDataBiosGetPhyAtkRateDataId();
extern u8 pokemonSeikakuDataBiosGetPhyDefRateDataId();
extern u8 pokemonSeikakuDataBiosGetNimblenessRateDataId();
extern u8 pokemonSeikakuDataBiosGetSpeAtkRateDataId();
extern u8 pokemonSeikakuDataBiosGetSpeDefRateDataId();
extern void* pokemonSeikakuDataBiosGetPtr(u8 idx);
extern void* pokemonSeikakuRateDataBiosGetPtr(u8 idx);
extern u8 pokemonSeikakuRateDataBiosGetKake();
extern u8 pokemonSeikakuRateDataBiosGetWaru();

#define POKEMON_LEVEL_STAT(base_field, iv_field, ev_field, out_field, rate_getter) \
    do {                                                                          \
        u16 species_;                                                             \
        u8 nature_;                                                               \
        u16 base_;                                                                \
        u16 iv_;                                                                  \
        u32 ev_;                                                                  \
        u32 value_;                                                               \
        void* natureData_;                                                        \
        void* rateData_;                                                          \
        species_ = (u16)pokemonGetStatus(ptr, 0, 0x6e, 0);                        \
        nature_ = (u8)pokemonGetStatus(ptr, 0, 0xbf, 0);                          \
        base_ = (u16)pokemonGetStatus(0, species_, (base_field), 0);              \
        iv_ = (u16)pokemonGetStatus(ptr, 0, (iv_field), 0);                       \
        ev_ = ((u32)pokemonGetStatus(ptr, 0, (ev_field), 0) >> 2) & 0x3fff;       \
        value_ = ((((u8)level) * (iv_ + ((base_ << 1) + ev_))) / 100) + 5;        \
        natureData_ = pokemonSeikakuDataBiosGetPtr(nature_);                      \
        if (natureData_ != NULL) {                                                \
            rateData_ = pokemonSeikakuRateDataBiosGetPtr(rate_getter(natureData_)); \
            if (rateData_ == NULL) {                                              \
                value_ = 0;                                                       \
            } else {                                                              \
                u8 kake_ = pokemonSeikakuRateDataBiosGetKake(rateData_);          \
                u8 waru_ = pokemonSeikakuRateDataBiosGetWaru(rateData_);          \
                value_ *= kake_;                                                  \
                if (waru_ != 0) {                                                 \
                    value_ /= waru_;                                              \
                }                                                                 \
            }                                                                     \
        }                                                                         \
        pokemonSetStatus(ptr, 0, (out_field), 0, value_);                         \
    } while (0)
/* 0x8012795C | 0x700 */
void pokemonSetLevelBasisStatus(u8* obj, u32 level) {
    extern u32 pokemonGetStatus(u8* obj, u32 id, u32 selector, u32 subindex);
    extern void pokemonSetStatus(void* obj, u32 id, u32 selector, u32 subindex, u32 value);
    u16 old_total;
    u16 species_id;
    u16 new_total;
    u8 nature_idx;
    u16 base_stat;
    u16 ev;
    u16 iv;
    s32 stat;
    u8* entry;
    u8* modifier;
    u8 num;
    u8 den;
    u16 current_hp;

    species_id = (u16)pokemonGetStatus(obj, 0, 0x6E, 0);
    old_total = (u16)pokemonGetStatus(obj, 0, 0x87, 0);
    pokemonSetStatus(obj, 0, 0x7A, 0, (u8)level);

    if (species_id == 0x12F) {
        new_total = 1;
        pokemonSetStatus(obj, 0, 0x87, 0, 1);
    } else {
        base_stat = (u16)pokemonGetStatus(obj, 0, 0x6E, 0);
        nature_idx = (u8)pokemonGetStatus(obj, 0, 0xBF, 0);
        base_stat = (u16)pokemonGetStatus(NULL, base_stat, 3, 0);
        ev = (u16)pokemonGetStatus(obj, 0, 0x93, 0);
        iv = (pokemonGetStatus(obj, 0, 0x8D, 0) >> 2) & 0x3FFF;
        stat = (s32)((u8)level * (u32)(ev + base_stat * 2 + iv)) / 100;
        stat = (u8)level + stat + 10;
        pokemonSeikakuDataBiosGetPtr(nature_idx);
        pokemonSetStatus(obj, 0, 0x87, 0, stat);
        new_total = (u16)stat;
    }

    base_stat = (u16)pokemonGetStatus(obj, 0, 0x6E, 0);
    nature_idx = (u8)pokemonGetStatus(obj, 0, 0xBF, 0);
    base_stat = (u16)pokemonGetStatus(NULL, base_stat, 4, 0);
    ev = (u16)pokemonGetStatus(obj, 0, 0x94, 0);
    iv = (pokemonGetStatus(obj, 0, 0x8E, 0) >> 2) & 0x3FFF;
    stat = (s32)((u8)level * (u32)(ev + base_stat * 2 + iv)) / 100 + 5;
    entry = pokemonSeikakuDataBiosGetPtr(nature_idx);
    if (entry != NULL) {
        modifier = pokemonSeikakuRateDataBiosGetPtr(
            pokemonSeikakuDataBiosGetPhyAtkRateDataId(entry));
        if (modifier == NULL) {
            stat = 0;
        } else {
            num = pokemonSeikakuRateDataBiosGetKake(modifier);
            den = pokemonSeikakuRateDataBiosGetWaru(modifier);
            stat *= num;
            if (den != 0) {
                stat /= den;
            }
        }
    }
    pokemonSetStatus(obj, 0, 0x88, 0, stat);

    base_stat = (u16)pokemonGetStatus(obj, 0, 0x6E, 0);
    nature_idx = (u8)pokemonGetStatus(obj, 0, 0xBF, 0);
    base_stat = (u16)pokemonGetStatus(NULL, base_stat, 5, 0);
    ev = (u16)pokemonGetStatus(obj, 0, 0x95, 0);
    iv = (pokemonGetStatus(obj, 0, 0x8F, 0) >> 2) & 0x3FFF;
    stat = (s32)((u8)level * (u32)(ev + base_stat * 2 + iv)) / 100 + 5;
    entry = pokemonSeikakuDataBiosGetPtr(nature_idx);
    if (entry != NULL) {
        modifier = pokemonSeikakuRateDataBiosGetPtr(
            pokemonSeikakuDataBiosGetPhyDefRateDataId(entry));
        if (modifier == NULL) {
            stat = 0;
        } else {
            num = pokemonSeikakuRateDataBiosGetKake(modifier);
            den = pokemonSeikakuRateDataBiosGetWaru(modifier);
            stat *= num;
            if (den != 0) {
                stat /= den;
            }
        }
    }
    pokemonSetStatus(obj, 0, 0x89, 0, stat);

    base_stat = (u16)pokemonGetStatus(obj, 0, 0x6E, 0);
    nature_idx = (u8)pokemonGetStatus(obj, 0, 0xBF, 0);
    base_stat = (u16)pokemonGetStatus(NULL, base_stat, 8, 0);
    ev = (u16)pokemonGetStatus(obj, 0, 0x98, 0);
    iv = (pokemonGetStatus(obj, 0, 0x92, 0) >> 2) & 0x3FFF;
    stat = (s32)((u8)level * (u32)(ev + base_stat * 2 + iv)) / 100 + 5;
    entry = pokemonSeikakuDataBiosGetPtr(nature_idx);
    if (entry != NULL) {
        modifier = pokemonSeikakuRateDataBiosGetPtr(
            pokemonSeikakuDataBiosGetNimblenessRateDataId(entry));
        if (modifier == NULL) {
            stat = 0;
        } else {
            num = pokemonSeikakuRateDataBiosGetKake(modifier);
            den = pokemonSeikakuRateDataBiosGetWaru(modifier);
            stat *= num;
            if (den != 0) {
                stat /= den;
            }
        }
    }
    pokemonSetStatus(obj, 0, 0x8C, 0, stat);

    base_stat = (u16)pokemonGetStatus(obj, 0, 0x6E, 0);
    nature_idx = (u8)pokemonGetStatus(obj, 0, 0xBF, 0);
    base_stat = (u16)pokemonGetStatus(NULL, base_stat, 6, 0);
    ev = (u16)pokemonGetStatus(obj, 0, 0x96, 0);
    iv = (pokemonGetStatus(obj, 0, 0x90, 0) >> 2) & 0x3FFF;
    stat = (s32)((u8)level * (u32)(ev + base_stat * 2 + iv)) / 100 + 5;
    entry = pokemonSeikakuDataBiosGetPtr(nature_idx);
    if (entry != NULL) {
        modifier = pokemonSeikakuRateDataBiosGetPtr(
            pokemonSeikakuDataBiosGetSpeAtkRateDataId(entry));
        if (modifier == NULL) {
            stat = 0;
        } else {
            num = pokemonSeikakuRateDataBiosGetKake(modifier);
            den = pokemonSeikakuRateDataBiosGetWaru(modifier);
            stat *= num;
            if (den != 0) {
                stat /= den;
            }
        }
    }
    pokemonSetStatus(obj, 0, 0x8A, 0, stat);

    base_stat = (u16)pokemonGetStatus(obj, 0, 0x6E, 0);
    nature_idx = (u8)pokemonGetStatus(obj, 0, 0xBF, 0);
    base_stat = (u16)pokemonGetStatus(NULL, base_stat, 7, 0);
    ev = (u16)pokemonGetStatus(obj, 0, 0x97, 0);
    iv = (pokemonGetStatus(obj, 0, 0x91, 0) >> 2) & 0x3FFF;
    stat = (s32)((u8)level * (u32)(ev + base_stat * 2 + iv)) / 100 + 5;
    entry = pokemonSeikakuDataBiosGetPtr(nature_idx);
    if (entry != NULL) {
        modifier = pokemonSeikakuRateDataBiosGetPtr(
            pokemonSeikakuDataBiosGetSpeDefRateDataId(entry));
        if (modifier == NULL) {
            stat = 0;
        } else {
            num = pokemonSeikakuRateDataBiosGetKake(modifier);
            den = pokemonSeikakuRateDataBiosGetWaru(modifier);
            stat *= num;
            if (den != 0) {
                stat /= den;
            }
        }
    }
    pokemonSetStatus(obj, 0, 0x8B, 0, stat);

    current_hp = (u16)pokemonGetStatus(obj, 0, 0x83, 0);
    if (current_hp != 0 || old_total == 0) {
        u16 new_hp;
        if (species_id == 0x12F) {
            new_hp = 1;
        } else {
            new_hp = (u16)(current_hp + new_total - old_total);
        }
        pokemonSetStatus(obj, 0, 0x83, 0, new_hp);
    }
}
#undef POKEMON_LEVEL_STAT
/* 0x8012805C | 0x2A4 */
extern void fadeSet(f32 duration, u32 mode);
extern void fadeCheck(u32 wait);
extern s32 evolutionOpen();
extern f32 lbl_8047D020;
/* Preserve retail cross-TU helper calls after source consolidation. */
#pragma dont_inline on
s32 pokemonEvolutionAll(u8* pokemon, u32 evolution_id, u32 add_species,
                        void* evolution_arg, u8* hero, s32 set_memo,
                        s32 evolution_mode, s32 use_fade)
{
    u8 move_slots[20];
    u16 learned_moves[20];
    u16 selected_moves[20];
    GfwBuf0x138 evolved_pokemon;
    GfwBuf0x138 added_pokemon;
    s32 move_count;
    s32 result;
    int i;
    s8 party_slot;

    if (!pokemonCheckValid(pokemon)) {
        return 2;
    }
    if ((u16)evolution_id == 0) {
        return 2;
    }

    move_count = pokemonEvolution(&evolved_pokemon, pokemon, evolution_id,
                                  evolution_arg, learned_moves);
    if (move_count < 0) {
        return 2;
    }

    if (use_fade != 0) {
        fadeSet(lbl_8047D020, 3);
        fadeCheck(1);
    }

    for (i = 0; i < move_count; i++) {
        selected_moves[i] = learned_moves[i];
    }
    result = evolutionOpen(pokemon, &evolved_pokemon, evolution_mode,
                           selected_moves, move_count, move_slots);

    if (use_fade != 0) {
        fadeSet(lbl_8047D020, 2);
        fadeCheck(1);
    }

    if (result == 2) {
        return 2;
    }
    if (result == 1) {
        return 1;
    }

    if (set_memo != 0) {
        memoDataSet(0, &evolved_pokemon);
    }
    pokemonBiosCopy((u32*)pokemon, evolved_pokemon.data);

    for (i = 0; i < move_count; i++) {
        if (move_slots[i] != 0xFF) {
            pokemonWazaCreate(pokemon, move_slots[i], learned_moves[i]);
        }
    }

    if ((u16)add_species != 0) {
        if ((u16)add_species != 0) {
            for (i = 0; i < 6; i++) {
                if (!pokemonCheckValid((u8*)heroGetStatus(hero, 3, (u16)i))) {
                    break;
                }
            }

            party_slot = (i < 6) ? (s8)i : -1;
            if (party_slot >= 0) {
                pokemonEvolutionCreateAddPokemon(&added_pokemon, pokemon,
                                                 add_species);
                heroAddPokemon(hero, &added_pokemon);
                if (set_memo != 0) {
                    memoDataSet(0, &added_pokemon);
                }
            }
        }
    }

    return 0;
}
#pragma dont_inline reset
/* 0x80128300 | 0x224 */
extern u8 lbl_802729A4[];
extern u8 lbl_80272998[];
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void pokemonEvolutionCreateAddPokemon();
/* 0x80128524 | 0x1A4 */
extern void fn_800F9EE4(void);
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
s32 pokemonEvolution();
/* 0x801286C8 | 0x39C */
extern void jumptable_80363468();
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void getEvoPokemonLevelUp(void);
/* 0x80128A64 | 0x25C */
extern void itemDataBiosGetItemSoubiDataId(void);
/* undecompiled: fn removed (ROM-derived asm), forward-declared for callers */
void pokemonEvolutionCheck(void);
/* 0x80128CC0 | 0x1C */
void* fn_80128CC0(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1C45C;
}
/* 0x80128CDC | 0x1C */
void* fn_80128CDC(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1B014;
}
/* 0x80128CF8 | 0x1C */
void* fn_80128CF8(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0xE3E8;
}
/* 0x80128D14 | 0x1C */
void* fn_80128D14(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x9A1C;
}
/* 0x80128D30 | 0x1C */
void* fn_80128D30(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x82A8;
}
/* 0x80128D4C | 0x1C */
void* fn_80128D4C(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x8168;
}
/* 0x80128D68 | 24 bytes | nc_addi_ptr */
void* fn_80128D68(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x7D20;
}
/* 0x80128D80 | 0x1C */
void* fn_80128D80(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1BE5C;
}
/* 0x80128D9C | 0x1C */
void* fn_80128D9C(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1BDDC;
}
/* 0x80128DB8 | 0x1C */
void* fn_80128DB8(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1BDBC;
}
/* 0x80128DD4 | 24 bytes | nc_addi_ptr */
void* fn_80128DD4(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0xB88;
}
/* 0x80128DEC | 24 bytes | nc_addi_ptr */
void* fn_80128DEC(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x70;
}
/* 0x80128E04 | 16 bytes | nc_bnelr */
u32 fn_80128E04(void* ptr) {
    if (ptr != NULL) { return (u32)ptr; }
    return 0;
}
/* 0x80128E14 | 0x10 */
void fn_80128E14(void* ptr) {
    if (ptr == NULL) { return; }
    lbl_8047ADB8 = (u32)ptr;
}
/* 0x80128E2C | 0xC */
extern u8 lbl_80408400[];
void* fn_80128E2C(void) {
    return (void*)lbl_80408400;
}
extern u32 lbl_80478F90;  /* obj header ptr (SDA) */
#if 0
asm void pokemonDataCheckValid(void) {
#include "src/game/gs_field_world_fn_80120C6C.inc"
}
#else
/* returns (obj-header[0] > key) after a pokemonGetStatus lookup; the obj-header ptr lbl_80478F90
 * is re-read via volatile cast. byte-match verified via objdiff. */
u32 pokemonDataCheckValid(u32 a, u16 key) {
    extern u32 pokemonGetStatus(u32 a, u16 key, u32 c, u32 d);
    if ((u16)key == 0) {
        return 0;
    }
    if (pokemonGetStatus(a, key, 1, 0) == 0) {
        return 0;
    }
    return (*(u32* volatile*)&lbl_80478F90)[0] > key;
}
#endif
extern void* tasteDataGetPtr(void* ptr);
extern void tasteDataGetAisyou(void* ptr, u8 val);
#if 0
asm void pokemonGetTasteLike(void) {
#include "src/game/gs_field_world_fn_80120D6C.inc"
}
#else
#pragma optimization_level 4
s32 pokemonGetTasteLike(u8* ptr, void* arg2) {
    u8 val; void* tmp;
    if (ptr == NULL) { return -1; }
    val = (u8)pokemonGetStatus(ptr, 0, 0xbf, 0);
    tmp = tasteDataGetPtr(arg2);
    tasteDataGetAisyou(tmp, val);
}
#endif
#if 0
asm void pokemonIsHpMantan(void) {
#include "src/game/gs_field_world_fn_80121410.inc"
}
#else
#pragma optimization_level 4
s32 pokemonIsHpMantan(u8* ptr) {
    u16 val1; u16 val2;
    if (ptr == NULL) { return 0; }
    val1 = (u16)pokemonGetStatus(ptr, 0, 0x83, 0);
    val2 = (u16)pokemonGetStatus(ptr, 0, 0x87, 0);
    return (u8)(val1 == val2);
}
#endif
#if 0
asm void fn_80121574(void) {
#include "src/game/gs_field_world_fn_80121574.inc"
}
#else
#pragma optimization_level 4
s32 fn_80121574(void* a, u16 b) {
    if ((u16)fn_80119ED0(b) != 0x7C && (u16)fn_80119ED0(b) != 0xC8) {
        return 0;
    }
    return (s32)fn_8011A3E4(a, b);
}
#endif
#if 0
asm void fn_8012165C(void) {
#include "src/game/gs_field_world_fn_8012165C.inc"
}
#else
#pragma optimization_level 4
s32 fn_8012165C(void* a, u16 b) {
    if ((u16)fn_80119ED0(b) != 0x7C && (u16)fn_80119ED0(b) != 0xC8) {
        return 0;
    }
    return fn_8011A6D4(a, b);
}
#endif
#if 0
asm void fn_801216CC(void) {
#include "src/game/gs_field_world_fn_801216CC.inc"
}
#else
#pragma optimization_level 4
s32 fn_801216CC(void* a, u16 b) {
    if ((u16)fn_80119ED0(b) != 0x7C && (u16)fn_80119ED0(b) != 0xC8) {
        return 0;
    }
    return fn_8011A860(a, b);
}
#endif
#if 0
asm void fn_8012182C(void) {
#include "src/game/gs_field_world_fn_8012182C.inc"
}
#else
#pragma optimization_level 4
s32 fn_8012182C(void* a, u16 b) {
    if ((u16)fn_80119ED0(b) != 0x7C && (u16)fn_80119ED0(b) != 0xC8) {
        return -1;
    }
    return fn_8011ACB4(a, b);
}
#endif
#if 0
asm void fn_8012189C(void) {
#include "src/game/gs_field_world_fn_8012189C.inc"
}
#else
#pragma optimization_level 4
s32 fn_8012189C(void* a, u16 b) {
    if ((u16)fn_80119ED0(b) != 0x7C && (u16)fn_80119ED0(b) != 0xC8) {
        return -1;
    }
    return fn_8011AE40(a, b);
}
#endif
#if 0
asm void fn_80121984(void) {
#include "src/game/gs_field_world_fn_80121984.inc"
}
#else
#pragma optimization_level 4
s32 fn_80121984(void* a, u16 b) {
    if ((u16)fn_80119ED0(b) != 0x7C && (u16)fn_80119ED0(b) != 0xC8) {
        return 0;
    }
    return fn_8011B130(a, b);
}
#endif
#if 0
asm void fn_80121A6C(void) {
#include "src/game/gs_field_world_fn_80121A6C.inc"
}
#else
#pragma optimization_level 4
s32 fn_80121A6C(void* a, u16 b) {
    if ((u16)fn_80119ED0(b) != 0x7C && (u16)fn_80119ED0(b) != 0xC8) {
        return 0;
    }
    return fn_8011B444(a, b);
}
#endif
#if 0
asm void fn_80121ADC(void) {
#include "src/game/gs_field_world_fn_80121ADC.inc"
}
#else
u8 fn_80121ADC(u8* ptr, u32 slot) {
    extern u16 fn_80119ED0(u32 val);
    extern u8 fn_8011B67C(u8* ptr, u32 val);
    if ((u16)fn_80119ED0(slot) != 0x7c) { if ((u16)fn_80119ED0(slot) != 0xc8) { return 0; } }
    return fn_8011B67C(ptr, slot);
}
#endif
#if 0
asm void pokemonGetAnnonKatati(void) {
#include "src/game/gs_field_world_fn_80122334.inc"
}
#else
#pragma optimization_level 2
u8 pokemonGetAnnonKatati(u32 val) {
    return (u8)((((val >> 18) & 0xC0) | ((val >> 12) & 0x30) | ((val >> 6) & 0x0C) | (val & 0x03)) % 28);
}
#pragma optimization_level reset
#endif
#if 0
asm void pokemonGetLevelToExp(void) {
#include "src/game/gs_field_world_fn_801229F4.inc"
}
#else
#pragma optimization_level 4
u32 pokemonGetLevelToExp(u8* ptr, u8 idx) {
    u16 val;
    u8 val2;
    void* sub;

    if (ptr == NULL) {
        return 0;
    }
    val = (u16)pokemonGetStatus(ptr, 0, 0x6E, 0);
    val2 = (u8)pokemonGetStatus(0, val, 0x11, 0);
    sub = pokemonGrowDataBiosGetPtr(val2);
    if (sub == NULL) {
        return 0;
    }
    return pokemonGrowDataBiosGetExp(sub, idx);
}
#endif
#if 0
asm void pokemonGetNowHpPercentage(void) {
#include "src/game/gs_field_world_fn_80122A70.inc"
}
#else
#pragma optimization_level 4
s32 pokemonGetNowHpPercentage(u8* ptr) {
    u16 val1;
    u16 val2;

    if (ptr == NULL) {
        return 0;
    }
    val1 = (u16)pokemonGetStatus(ptr, 0, 0x83, 0);
    val2 = (u16)pokemonGetStatus(ptr, 0, 0x87, 0);
    return (val1 * 100) / val2;
}
#endif
#if 0
asm void pokemonGetNowHpWaruValue(void) {
#include "src/game/gs_field_world_fn_80122AE0.inc"
}
#else
#pragma optimization_level 4
u16 pokemonGetNowHpWaruValue(u8* ptr, s32 b) {
    u16 val;

    if (ptr == NULL) {
        return 0;
    }
    if (!(u16)b) {
        return 0;
    }
    val = (s32)(u16)pokemonGetStatus(ptr, 0, 0x83, 0) / (s32)(u16)b;
    if (val == 0) {
        val = 1;
    }
    return val;
}
#endif
#if 0
asm void pokemonGetMaxHpWaruValue(void) {
#include "src/game/gs_field_world_fn_80122B50.inc"
}
#else
#pragma optimization_level 4
u16 pokemonGetMaxHpWaruValue(u8* ptr, s32 b) {
    u16 val;

    if (ptr == NULL) {
        return 0;
    }
    if (!(u16)b) {
        return 0;
    }
    val = (s32)(u16)pokemonGetStatus(ptr, 0, 0x87, 0) / (s32)(u16)b;
    if (val == 0) {
        val = 1;
    }
    return val;
}
#endif
#if 0
asm void pokemonGetSoubiItemSoubiDataId(void) {
#include "src/game/gs_field_world_fn_80123090.inc"
}
#else
#pragma optimization_level 4
u16 pokemonGetSoubiItemSoubiDataId(u8* ptr) {
    extern u32 itemGetStatus(u32 a, u16 b, u32 c, u32 d);
    u16 val;
    val = (u16)pokemonGetStatus(ptr, 0, 0x82, 0);
    if (val == 0) { return 0; }
    return (u16)itemGetStatus(0, val, 7, 0);
}
#endif
#if 0
asm void pokemonWazaGetMaxPP(void) {
#include "src/game/gs_field_world_fn_80123E70.inc"
}
#else
#pragma optimization_level 4
s32 pokemonWazaGetMaxPP(u8* ptr, u16 idx) {
    extern u8 wazaGetMaxPP(u16 type_id, u8 val);
    extern u32 pokemonGetStatus(u8* ptr, u32 a, u32 b, u16 c);
    u32 sub; u16 val1;
    if (ptr == NULL) { return 0; }
    sub = (u16)idx + 4;
    val1 = (u16)pokemonGetStatus(ptr, 0, 0x7f, (u16)sub);
    wazaGetMaxPP(val1, (u8)pokemonGetStatus(ptr, 0, 0x81, (u16)sub));
}
#endif
void pokemonSetTokuseiFlag(u8* ptr, u32 arg2);
/* body moved to pokemon_range_exact_801248C4.c: pokemonSetTokuseiFlag */
extern void fn_80135708(void);
void pokemonInit(u8* ptr);
/* body moved to pokemon_range_exact_801248C4.c: pokemonInit */

#endif
#pragma optimization_level 4
#ifdef POKEMON_SET_STATUS_EXACT
void pokemonSetStatus(u8* obj, u32 id, u32 selector, u32 subindex, u32 value)
{
    typedef struct PokemonStatusCopy16 {
        u32 word[4];
    } PokemonStatusCopy16;
    register u32 status_subindex = subindex;
    register u32 status_value = value;

    if ((u16)selector == 0 || (u16)selector >= 0x124) {
        return;
    }

    if ((u16)selector < 0x6D) {
        obj = pokemonDataBiosGetPtr(id);
        if (obj == NULL) {
            return;
        }
    } else if (obj == NULL) {
        return;
    }

    switch ((u16)selector) {
    case 0x01: pokemonDataBiosSetName(obj, status_value); break;
    case 0x03: pokemonDataBiosSetBasisMaxHp(obj, (u16)status_value); break;
    case 0x04: pokemonDataBiosSetBasisPhyAtk(obj, (u16)status_value); break;
    case 0x05: pokemonDataBiosSetBasisPhyDef(obj, (u16)status_value); break;
    case 0x06: pokemonDataBiosSetBasisSpeAtk(obj, (u16)status_value); break;
    case 0x07: pokemonDataBiosSetBasisSpeDef(obj, (u16)status_value); break;
    case 0x08: pokemonDataBiosSetBasisNimbleness(obj, (u16)status_value); break;
    case 0x0A: pokemonDataBiosSetGiveMaxHpEffort(obj, (u16)status_value); break;
    case 0x0B: pokemonDataBiosSetGivePhyAtkEffort(obj, (u16)status_value); break;
    case 0x0C: pokemonDataBiosSetGivePhyDefEffort(obj, (u16)status_value); break;
    case 0x0D: pokemonDataBiosSetGiveSpeAtkEffort(obj, (u16)status_value); break;
    case 0x0E: pokemonDataBiosSetGiveSpeDefEffort(obj, (u16)status_value); break;
    case 0x0F: pokemonDataBiosSetGiveNimblenessEffort(obj, (u16)status_value); break;
    case 0x10: pokemonDataBiosSetGiveExp(obj, (u16)status_value); break;
    case 0x11: pokemonDataBiosSetGrowDataId(obj, (u8)status_value); break;
    case 0x12: pokemonDataBiosSetGet(obj, (u8)status_value); break;
    case 0x13: pokemonDataBiosSetSexRatio(obj, (u8)status_value); break;
    case 0x14: pokemonDataBiosSetInitFriend(obj, (u16)status_value); break;
    case 0x15: pokemonDataBiosSetItemDataId(obj, status_subindex, (u16)status_value); break;
    case 0x16: pokemonDataBiosSetZokuseiDataId(obj, status_subindex, (u8)status_value); break;
    case 0x17: pokemonDataBiosSetTokuseiDataId(obj, status_subindex, (u8)status_value); break;
    case 0x19: pokemonDataBiosSetSinkaKind(obj, status_subindex, (u8)status_value); break;
    case 0x1A: pokemonDataBiosSetSinkaBuff(obj, status_subindex, (u16)status_value); break;
    case 0x1B: pokemonDataBiosSetSinkaPokemonDataId(obj, status_subindex, (u16)status_value); break;
    case 0x1D: pokemonDataBiosSetGetWazaLevel(obj, status_subindex, (u8)status_value); break;
    case 0x1E: pokemonDataBiosSetGetWazaDataId(obj, status_subindex, (u16)status_value); break;
    case 0x20: pokemonDataBiosSetWazaMcn(obj, 0x00, (u8)status_value); break;
    case 0x21: pokemonDataBiosSetWazaMcn(obj, 0x01, (u8)status_value); break;
    case 0x22: pokemonDataBiosSetWazaMcn(obj, 0x02, (u8)status_value); break;
    case 0x23: pokemonDataBiosSetWazaMcn(obj, 0x03, (u8)status_value); break;
    case 0x24: pokemonDataBiosSetWazaMcn(obj, 0x04, (u8)status_value); break;
    case 0x25: pokemonDataBiosSetWazaMcn(obj, 0x05, (u8)status_value); break;
    case 0x26: pokemonDataBiosSetWazaMcn(obj, 0x06, (u8)status_value); break;
    case 0x27: pokemonDataBiosSetWazaMcn(obj, 0x07, (u8)status_value); break;
    case 0x28: pokemonDataBiosSetWazaMcn(obj, 0x08, (u8)status_value); break;
    case 0x29: pokemonDataBiosSetWazaMcn(obj, 0x09, (u8)status_value); break;
    case 0x2A: pokemonDataBiosSetWazaMcn(obj, 0x0A, (u8)status_value); break;
    case 0x2B: pokemonDataBiosSetWazaMcn(obj, 0x0B, (u8)status_value); break;
    case 0x2C: pokemonDataBiosSetWazaMcn(obj, 0x0C, (u8)status_value); break;
    case 0x2D: pokemonDataBiosSetWazaMcn(obj, 0x0D, (u8)status_value); break;
    case 0x2E: pokemonDataBiosSetWazaMcn(obj, 0x0E, (u8)status_value); break;
    case 0x2F: pokemonDataBiosSetWazaMcn(obj, 0x0F, (u8)status_value); break;
    case 0x30: pokemonDataBiosSetWazaMcn(obj, 0x10, (u8)status_value); break;
    case 0x31: pokemonDataBiosSetWazaMcn(obj, 0x11, (u8)status_value); break;
    case 0x32: pokemonDataBiosSetWazaMcn(obj, 0x12, (u8)status_value); break;
    case 0x33: pokemonDataBiosSetWazaMcn(obj, 0x13, (u8)status_value); break;
    case 0x34: pokemonDataBiosSetWazaMcn(obj, 0x14, (u8)status_value); break;
    case 0x35: pokemonDataBiosSetWazaMcn(obj, 0x15, (u8)status_value); break;
    case 0x36: pokemonDataBiosSetWazaMcn(obj, 0x16, (u8)status_value); break;
    case 0x37: pokemonDataBiosSetWazaMcn(obj, 0x17, (u8)status_value); break;
    case 0x38: pokemonDataBiosSetWazaMcn(obj, 0x18, (u8)status_value); break;
    case 0x39: pokemonDataBiosSetWazaMcn(obj, 0x1A, (u8)status_value); break;
    case 0x3A: pokemonDataBiosSetWazaMcn(obj, 0x1B, (u8)status_value); break;
    case 0x3B: pokemonDataBiosSetWazaMcn(obj, 0x1C, (u8)status_value); break;
    case 0x3C: pokemonDataBiosSetWazaMcn(obj, 0x1D, (u8)status_value); break;
    case 0x3D: pokemonDataBiosSetWazaMcn(obj, 0x1E, (u8)status_value); break;
    case 0x3E: pokemonDataBiosSetWazaMcn(obj, 0x1F, (u8)status_value); break;
    case 0x3F: pokemonDataBiosSetWazaMcn(obj, 0x20, (u8)status_value); break;
    case 0x40: pokemonDataBiosSetWazaMcn(obj, 0x21, (u8)status_value); break;
    case 0x41: pokemonDataBiosSetWazaMcn(obj, 0x22, (u8)status_value); break;
    case 0x42: pokemonDataBiosSetWazaMcn(obj, 0x23, (u8)status_value); break;
    case 0x43: pokemonDataBiosSetWazaMcn(obj, 0x24, (u8)status_value); break;
    case 0x44: pokemonDataBiosSetWazaMcn(obj, 0x25, (u8)status_value); break;
    case 0x45: pokemonDataBiosSetWazaMcn(obj, 0x26, (u8)status_value); break;
    case 0x46: pokemonDataBiosSetWazaMcn(obj, 0x27, (u8)status_value); break;
    case 0x47: pokemonDataBiosSetWazaMcn(obj, 0x28, (u8)status_value); break;
    case 0x48: pokemonDataBiosSetWazaMcn(obj, 0x29, (u8)status_value); break;
    case 0x49: pokemonDataBiosSetWazaMcn(obj, 0x2A, (u8)status_value); break;
    case 0x4A: pokemonDataBiosSetWazaMcn(obj, 0x2B, (u8)status_value); break;
    case 0x4B: pokemonDataBiosSetWazaMcn(obj, 0x2C, (u8)status_value); break;
    case 0x4C: pokemonDataBiosSetWazaMcn(obj, 0x2D, (u8)status_value); break;
    case 0x4D: pokemonDataBiosSetWazaMcn(obj, 0x2E, (u8)status_value); break;
    case 0x4E: pokemonDataBiosSetWazaMcn(obj, 0x2F, (u8)status_value); break;
    case 0x4F: pokemonDataBiosSetWazaMcn(obj, 0x30, (u8)status_value); break;
    case 0x50: pokemonDataBiosSetWazaMcn(obj, 0x31, (u8)status_value); break;
    case 0x51: pokemonDataBiosSetWazaMcn(obj, 0x32, (u8)status_value); break;
    case 0x52: pokemonDataBiosSetWazaMcn(obj, 0x33, (u8)status_value); break;
    case 0x53: pokemonDataBiosSetWazaMcn(obj, 0x34, (u8)status_value); break;
    case 0x54: pokemonDataBiosSetWazaMcn(obj, 0x35, (u8)status_value); break;
    case 0x55: pokemonDataBiosSetWazaMcn(obj, 0x36, (u8)status_value); break;
    case 0x56: pokemonDataBiosSetWazaMcn(obj, 0x37, (u8)status_value); break;
    case 0x57: pokemonDataBiosSetWazaMcn(obj, 0x38, (u8)status_value); break;
    case 0x58: pokemonDataBiosSetWazaMcn(obj, 0x39, (u8)status_value); break;
    case 0x59: pokemonDataBiosSetWazaMcn(obj, 0x3A, (u8)status_value); break;
    case 0x5A: pokemonDataBiosSetPokebodyId(obj, status_subindex, status_value); break;
    case 0x5B: pokemonDataBiosSetStatusFaceMenuSpriteId(obj, status_subindex, (u16)status_value); break;
    case 0x5C: pokemonDataBiosSetColor(obj, status_subindex, (u8)status_value); break;
    case 0x5D: pokemonDataBiosSetTypeName(obj, status_value); break;
    case 0x5E: pokemonDataBiosSetHeight(obj, (u16)status_value); break;
    case 0x5F: pokemonDataBiosSetWeight(obj, (u16)status_value); break;
    case 0x60: pokemonDataBiosSetDoc(obj, status_value); break;
    case 0x61: pokemonDataBiosSetVoice(obj, (u16)status_value); break;
    case 0x62: pokemonDataBiosSetMitaFlag(obj, status_value); break;
    case 0x63: pokemonDataBiosSetTukamaetaFlag(obj, status_value); break;
    case 0x64: pokemonDataBiosSetNumZukan(obj, (u16)status_value); break;
    case 0x65: pokemonDataBiosSetNumPokemon(obj, (u16)status_value); break;
    case 0x66: pokemonDataBiosSetPkxDataId(obj, status_value); break;
    case 0x68: pokemonDataBiosSetKowaza(obj, status_subindex, (u16)status_value); break;
    case 0x69: fn_8011CED0(obj, status_subindex, (u8)status_value); break;
    case 0x6A: fn_8011CEC0(obj, (u16)status_value); break;
    case 0x6B: fn_8011CEB0(obj, (u8)status_value); break;
    case 0x6C: fn_8011CEA0(obj, (u8)status_value); break;
    case 0x6E: pokemonBiosSetPokemonDataId(obj, (u16)status_value); break;
    case 0x6F: pokemonBiosSetRnd(obj, status_value); break;
    case 0x71: pokemonBiosSetCatchFloorId(obj, (u16)status_value); break;
    case 0x72: pokemonBiosSetCatchLevel(obj, (u8)status_value); break;
    case 0x73: pokemonBiosSetCatchBallId(obj, (u8)status_value); break;
    case 0x74: pokemonBiosSetCatchTrainerSex(obj, (u8)status_value); break;
    case 0x75: pokemonBiosSetCatchTrainerRnd(obj, status_value); break;
    case 0x76: pokemonBiosSetCatchTrainerNamePtr(obj, (void*)status_value); break;
    case 0x77: pokemonBiosSetNicknamePtr(obj, (void*)status_value); break;
    case 0x78: pokemonBiosSetNicknameOrgPtr(obj, (void*)status_value); break;
    case 0x79: pokemonBiosSetExp(obj, status_value); break;
    case 0x7A: pokemonBiosSetLevel(obj, (u8)status_value); break;
    case 0x7C: {
        PokemonStatusCopy16* dst = (PokemonStatusCopy16*)pokemonGetStatus(obj, 0, 0x7C, 0);
        if (dst != NULL) {
            *dst = *(PokemonStatusCopy16*)status_value;
        }
        break;
    }
    case 0x7D: pokemonBiosSetConditionAmari(obj, status_value); break;
    case 0x7F: pokemonBiosSetPokemonWazaDataId(obj, status_subindex, status_value); break;
    case 0x80: pokemonBiosSetPokemonWazaPp(obj, status_subindex, (u8)status_value); break;
    case 0x81: pokemonBiosSetPokemonWazaPpCount(obj, status_subindex, (u8)status_value); break;
    case 0x82: pokemonBiosSetItemDataId(obj, (u16)status_value); break;
    case 0x83: pokemonBiosSetHp(obj, (u16)status_value); break;
    case 0x87: pokemonBiosSetMaxHp(obj, (u16)status_value); break;
    case 0x88: pokemonBiosSetPhyAtk(obj, (u16)status_value); break;
    case 0x89: pokemonBiosSetPhyDef(obj, (u16)status_value); break;
    case 0x8A: pokemonBiosSetSpeAtk(obj, (u16)status_value); break;
    case 0x8B: pokemonBiosSetSpeDef(obj, (u16)status_value); break;
    case 0x8C: pokemonBiosSetNimbleness(obj, (u16)status_value); break;
    case 0x8D: pokemonBiosSetMaxHpEffort(obj, (u16)status_value); break;
    case 0x8E: pokemonBiosSetPhyAtkEffort(obj, (u16)status_value); break;
    case 0x8F: pokemonBiosSetPhyDefEffort(obj, (u16)status_value); break;
    case 0x90: pokemonBiosSetSpeAtkEffort(obj, (u16)status_value); break;
    case 0x91: pokemonBiosSetSpeDefEffort(obj, (u16)status_value); break;
    case 0x92: pokemonBiosSetNimblenessEffort(obj, (u16)status_value); break;
    case 0x93: pokemonBiosSetMaxHpRnd(obj, (u16)status_value); break;
    case 0x94: pokemonBiosSetPhyAtkRnd(obj, (u16)status_value); break;
    case 0x95: pokemonBiosSetPhyDefRnd(obj, (u16)status_value); break;
    case 0x96: pokemonBiosSetSpeAtkRnd(obj, (u16)status_value); break;
    case 0x97: pokemonBiosSetSpeDefRnd(obj, (u16)status_value); break;
    case 0x98: pokemonBiosSetNimblenessRnd(obj, (u16)status_value); break;
    case 0x99: pokemonBiosSetFriend(obj, (u16)status_value); break;
    case 0x9C: pokemonBiosSetStyle(obj, (u8)status_value); break;
    case 0x9D: pokemonBiosSetBeautiful(obj, (u8)status_value); break;
    case 0x9E: pokemonBiosSetCute(obj, (u8)status_value); break;
    case 0x9F: pokemonBiosSetClever(obj, (u8)status_value); break;
    case 0xA0: pokemonBiosSetStrong(obj, (u8)status_value); break;
    case 0xA1: pokemonBiosSetFur(obj, (u8)status_value); break;
    case 0xA3: pokemonBiosSetChampRibbon(obj, (u8)status_value); break;
    case 0xA4: pokemonBiosSetWinningRibbon(obj, (u8)status_value); break;
    case 0xA5: pokemonBiosSetVictoryRibbon(obj, (u8)status_value); break;
    case 0xA6: pokemonBiosSetBromideRibbon(obj, (u8)status_value); break;
    case 0xA7: pokemonBiosSetGanbaRibbon(obj, (u8)status_value); break;
    case 0xA8: pokemonBiosSetMarineRibbon(obj, (u8)status_value); break;
    case 0xA9: pokemonBiosSetLandRibbon(obj, (u8)status_value); break;
    case 0xAA: pokemonBiosSetSkyRibbon(obj, (u8)status_value); break;
    case 0xAB: pokemonBiosSetCountryRibbon(obj, (u8)status_value); break;
    case 0xAC: pokemonBiosSetNationalRibbon(obj, (u8)status_value); break;
    case 0xAD: pokemonBiosSetEarthRibbon(obj, (u8)status_value); break;
    case 0xAE: pokemonBiosSetWorldRibbon(obj, (u8)status_value); break;
    case 0xAF: pokemonBiosSetAmariRibbon(obj, (u8)status_value); break;
    case 0xB0: pokemonBiosSetStyleMedal(obj, (u8)status_value); break;
    case 0xB1: pokemonBiosSetBeautifulMedal(obj, (u8)status_value); break;
    case 0xB2: pokemonBiosSetCuteMedal(obj, (u8)status_value); break;
    case 0xB3: pokemonBiosSetCleverMedal(obj, (u8)status_value); break;
    case 0xB4: pokemonBiosSetStrongMedal(obj, (u8)status_value); break;
    case 0xB5: pokemonBiosSetPokerus(obj, (u8)status_value); break;
    case 0xB6: pokemonBiosSetTamagoFlag(obj, (u8)status_value); break;
    case 0xB7: pokemonBiosSetTokuseiFlag(obj, (u8)status_value); break;
    case 0xB8: pokemonBiosSetFuseiFlag(obj, (u8)status_value); break;
    case 0xB9: pokemonBiosSetFlagAmari(obj, (u8)status_value); break;
    case 0xBB: pokemonBiosSetPcboxMark(obj, (u8)status_value); break;
    case 0xBC: pokemonBiosSetMailId(obj, (u8)status_value); break;
    case 0xBD: pokemonBiosSetPara1Amari(obj, (u16)status_value); break;
    case 0xBE: pokemonBiosSetAmari(obj, (u16)status_value); break;
    case 0xC9: pokemonBiosSetFightTrainerPokemonDataId(obj, (u16)status_value); break;
    case 0xC3: pokemonBiosSetDarkpokemonDataId(obj, (u16)status_value); break;
    case 0xC5: pokemonBiosSetDp(obj, status_value); break;
    case 0xC6: pokemonBiosSetPoolExp(obj, status_value); break;
    case 0xC7: pokemonBiosSetPoolFriend(obj, (u16)status_value); break;
    case 0xCB: fightPokemonBiosSetMotoPokemonPtr(obj, status_value); break;
    case 0xCD: {
        PokemonStatusCopy16* dst = (PokemonStatusCopy16*)pokemonGetStatus(obj, 0, 0xCD, 0);
        if (dst != NULL) {
            *dst = *(PokemonStatusCopy16*)status_value;
        }
        break;
    }
    case 0xCE: fightPokemonBiosSetEntryId(obj, (s16)status_value); break;
    case 0xCF: fightPokemonBiosSetCatchEntryFlag(obj, (u8)status_value); break;
    case 0xD0: fightPokemonBiosSetLevelUpFlag(obj, (u8)status_value); break;
    case 0xD1: fightPokemonBiosSetDarkOutFlag(obj, (u8)status_value); break;
    case 0xD2: fightPokemonBiosSetHokakuFlag(obj, (u8)status_value); break;
    case 0xD5: fightOutPokemonBiosSetMotoFightPokemonPtr(obj, status_value); break;
    case 0xD6: fightOutPokemonBiosSetFightPokemonPtr(obj, status_value); break;
    case 0xDA: wazaSetStatus(pokemonGetStatus(obj, 0, 0xD9, 0), 0, 0x28, 0, status_value); break;
    case 0xDB: wazaSetStatus(pokemonGetStatus(obj, 0, 0xD9, 0), 0, 0x27, 0, status_value); break;
    case 0xDC: wazaSetStatus(pokemonGetStatus(obj, 0, 0xD9, 0), 0, 0x2C, 0, status_value); break;
    case 0xDD: wazaSetStatus(pokemonGetStatus(obj, 0, 0xD9, 0), 0, 0x2B, 0, status_value); break;
    case 0xDE: {
        u32 fight_waza = pokemonGetStatus(obj, 0, 0xD9, 0);
        if (fightWazaCheckWriteJoutaiDataId(fight_waza, (u16)status_value) == 2) {
            fightWazaWriteJoutaiDataId(fight_waza, (u16)status_value, 0);
        }
        break;
    }
    case 0xDF: {
        u32 fight_waza = pokemonGetStatus(obj, 0, 0xD9, 0);
        if ((u16)status_value != 0) {
            if (fightWazaIsJoutaiDataId(fight_waza, (u16)status_value) == 1) {
                fightWazaInitJoutaiDataId(fight_waza, (u16)status_value);
            }
        } else {
            fightWazaInitJoutai(fight_waza);
        }
        break;
    }
    case 0xE1: wazaSetStatus(pokemonGetStatus(obj, 0, 0xD9, 0), 0, 0x2D, 0, status_value); break;
    case 0xE2: wazaSetStatus(pokemonGetStatus(obj, 0, 0xD9, 0), 0, 0x2F, 0, (u16)status_value); break;
    case 0xE3: wazaSetStatus(pokemonGetStatus(obj, 0, 0xD9, 0), 0, 0x29, 0, status_value); break;
    case 0xE4: wazaSetStatus(pokemonGetStatus(obj, 0, 0xD9, 0), 0, 0x2E, 0, status_value); break;
    case 0xE6: fightOutPokemonBiosSetAbicntPhyAtk(obj, (u8)status_value); break;
    case 0xE7: fightOutPokemonBiosSetAbicntPhyDef(obj, (u8)status_value); break;
    case 0xE8: fightOutPokemonBiosSetAbicntSpeAtk(obj, (u8)status_value); break;
    case 0xE9: fightOutPokemonBiosSetAbicntSpeDef(obj, (u8)status_value); break;
    case 0xEA: fightOutPokemonBiosSetAbicntNimbleness(obj, (u8)status_value); break;
    case 0xEB: fightOutPokemonBiosSetAbicntAverage(obj, (u8)status_value); break;
    case 0xEC: fightOutPokemonBiosSetAbicntAvoid(obj, (u8)status_value); break;
    case 0xED: fightOutPokemonBiosSetFightoutTurnCount(obj, (u16)status_value); break;
    case 0xEE: fightOutPokemonBiosSetSequencePtr(obj, status_value); break;
    case 0xEF: fightOutPokemonBiosSetSketchWazaDataId(obj, (u16)status_value); break;
    case 0xF0: fightOutPokemonBiosSetLastSelectWazaDataId(obj, (u16)status_value); break;
    case 0xF1: fightOutPokemonBiosSetLastUseWazaDataId(obj, (u16)status_value); break;
    case 0xF2: fightOutPokemonBiosSetLastReceiveWazaTargetDataId(obj, (u16)status_value); break;
    case 0xF3: fightOutPokemonBiosSetHitWazaDataId(obj, (u16)status_value); break;
    case 0xF4: fightOutPokemonBiosSetHitWazaZokuseiDataId(obj, (u16)status_value); break;
    case 0xF5: fightOutPokemonBiosSetGamanDamageValue(obj, (s16)status_value); break;
    case 0xF6: fightOutPokemonBiosSetGamanDamageTargetId(obj, (u16)status_value); break;
    case 0xF7: fightOutPokemonBiosSetOumuWazaDataId(obj, (u16)status_value); break;
    case 0xF9: fightOutPokemonBiosSetNamakeFlag(obj, (u8)status_value); break;
    case 0xFA: fightOutPokemonBiosSetUsedItemDataId(obj, (u16)status_value); break;
    case 0xFB: fightOutPokemonBiosSetStockItemDataId(obj, (u16)status_value); break;
    case 0xFC: fightOutPokemonBiosSetSuccessCnt(obj, (u8)status_value); break;
    case 0xFD: {
        u8 enemy_index = (u8)status_subindex;
        u16 entry_id = (s8)status_value;
        fightOutPokemonBiosSetMeetEnemyFightPokemonEntryId(obj, enemy_index, entry_id);
        break;
    }
    case 0xFF: fightOutPokemonBiosSetZokuseiDataId(obj, (u8)status_subindex, (u16)status_value); break;
    case 0x100: fightOutPokemonBiosSetTokuseiDataId(obj, (u16)status_value); break;
    case 0x102: fightOutPokemonBiosSetDamageAtkValue(obj, (s16)status_value); break;
    case 0x103: fightOutPokemonBiosSetDamageAtkTargetId(obj, (u16)status_value); break;
    case 0x104: fightOutPokemonBiosSetDamageSpeValue(obj, (s16)status_value); break;
    case 0x105: fightOutPokemonBiosSetDamageSpeTargetId(obj, (u16)status_value); break;
    case 0x106: fightOutPokemonBiosSetMahiNoAttackFlag(obj, (u8)status_value); break;
    case 0x107: fightOutPokemonBiosSetKonranMyselfAttackFlag(obj, (u8)status_value); break;
    case 0x108: fightOutPokemonBiosSetOutWazaKoukanaiFlag(obj, (u8)status_value); break;
    case 0x109: fightOutPokemonBiosSetTameWazaFlag(obj, (u8)status_value); break;
    case 0x10A: fightOutPokemonBiosSetItemNigeruFlag(obj, (u8)status_value); break;
    case 0x10B: fightOutPokemonBiosSetHuuinNoAttackFlag(obj, (u8)status_value); break;
    case 0x10C: fightOutPokemonBiosSetMeroMeroNoAttackFlag(obj, (u8)status_value); break;
    case 0x10D: fightOutPokemonBiosSetKanashibariNoAttackFlag(obj, (u8)status_value); break;
    case 0x10E: fightOutPokemonBiosSetChouhatsuNoAttackFlag(obj, (u8)status_value); break;
    case 0x10F: fightOutPokemonBiosSetIchamonNoAttackFlag(obj, (u8)status_value); break;
    case 0x110: fightOutPokemonBiosSetHirumuNoAttackFlag(obj, (u8)status_value); break;
    case 0x111: fightOutPokemonBiosSetPassPpdecFlag(obj, (u8)status_value); break;
    case 0x112: fightOutPokemonBiosSetFightActionFlag(obj, (u8)status_value); break;
    case 0x113: fightOutPokemonBiosSetDoClearbodyFlag(obj, (u8)status_value); break;
    case 0x114: fightOutPokemonBiosSetReceivesWazaHiraishinFlag(obj, (u8)status_value); break;
    case 0x115: fightOutPokemonBiosSetVanishoffFlag(obj, (u8)status_value); break;
    case 0x116: fightOutPokemonBiosSetDoIkakuFlag(obj, (u8)status_value); break;
    case 0x117: fightOutPokemonBiosSetDoTraceFlag(obj, (u8)status_value); break;
    case 0x118: fightOutPokemonBiosSetNoPressureFlag(obj, (u8)status_value); break;
    case 0x119: fightOutPokemonBiosSetIrekaetaFlag(obj, (u8)status_value); break;
    case 0x11A: fightOutPokemonBiosSetItemKoraetaFlag(obj, (u8)status_value); break;
    case 0x11B: fightOutPokemonBiosSetKaigaraDamageValue(obj, status_value); break;
    case 0x11C: fightOutPokemonBiosSetMyselfDamageAtkValue(obj, (s16)status_value); break;
    case 0x11D: fightOutPokemonBiosSetMyselfDamageAtkTargetId(obj, (u16)status_value); break;
    case 0x11E: fightOutPokemonBiosSetMyselfDamageSpeValue(obj, (s16)status_value); break;
    case 0x11F: fightOutPokemonBiosSetMyselfDamageSpeTargetId(obj, (u16)status_value); break;
    case 0x120: fightOutPokemonBiosSetKizetuFlag(obj, (u8)status_value); break;
    case 0x121: fightOutPokemonBiosSetIrekaeTargetEntryId(obj, (s16)status_value); break;
    }
}
#else
void pokemonSetStatus(u8* obj, u32 id, u32 selector, u32 subindex, u32 value);
/* body selected by pokemon_set_status_exact_801254B4.c */
#endif

#ifndef POKEMON_SET_STATUS_EXACT
u32 pokemonGetStatus(u8* obj, u32 id, u32 selector, u32 d) {
    extern void* fn_8011E778(u32 idx);
    extern u32  fn_8011E760(u8* ptr);
    extern u16  fn_8011E734(u8* ptr);
    extern u16  fn_8011E708(u8* ptr);
    extern u16  fn_8011E6DC(u8* ptr);
    extern u16  fn_8011E6B0(u8* ptr);
    extern u16  fn_8011E684(u8* ptr);
    extern u16  fn_8011E658(u8* ptr);
    extern u16  fn_8011E62C(u8* ptr);
    extern u16  fn_8011E600(u8* ptr);
    extern u16  fn_8011E5D4(u8* ptr);
    extern u16  fn_8011E5A8(u8* ptr);
    extern u16  fn_8011E57C(u8* ptr);
    extern u16  fn_8011E550(u8* ptr);
    extern u16  fn_8011E538(u8* ptr);
    extern u8   fn_8011E520(u8* ptr);
    extern u8   fn_8011E508(u8* ptr);
    extern u8   fn_8011E4F0(u8* ptr);
    extern u16  fn_8011E4D8(u8* ptr);
    extern u16  fn_8011E4A4(u8* ptr, u32 idx);
    extern u8   fn_8011E474(u8* ptr, u32 idx);
    extern u8   fn_8011E444(u8* ptr, u32 idx);
    extern u8   fn_8011E3FC(u8* ptr, u32 idx);
    extern u16  fn_8011E3B4(u8* ptr, u32 idx);
    extern u16  fn_8011E36C(u8* ptr, u32 idx);
    extern u8   fn_8011E324(u8* ptr, u32 idx);
    extern u16  fn_8011E2DC(u8* ptr, u32 idx);
    extern u8   fn_8011E2AC(u8* ptr, u32 idx);
    extern u32  fn_8011E264(u8* ptr, u32 val);
    extern u32  fn_8011E21C(u8* ptr, u32 val);
    extern u8   fn_8011E1D4(u8* ptr, u32 val);
    extern u32  fn_8011E1BC(u8* ptr);
    extern u16  fn_8011E1A4(u8* ptr);
    extern u16  fn_8011E18C(u8* ptr);
    extern u32  fn_8011E174(u8* ptr);
    extern u16  fn_8011E15C(u8* ptr);
    extern u32  fn_8011E128(u8* ptr);
    extern u32  fn_8011E0F4(u8* ptr);
    extern u16  fn_8011E0DC(u8* ptr);
    extern u16  fn_8011E0C4(u8* ptr);
    extern u32  fn_8011E0AC(u8* ptr);
    extern u16  fn_8011E078(u8* ptr, u32 idx);
    extern u8   fn_8011E048(u8* ptr, u32 idx);
    extern u16  fn_8011E030(u8* ptr);
    extern u8   fn_8011E018(u8* ptr);
    extern u8   fn_8011E000(u8* ptr);
    extern u16  fn_8011F5C8(u8* ptr);
    extern u32  fn_8011F5B0(u8* ptr);
    extern u32  fn_8011F598(u8* ptr);
    extern u16  fn_8011F580(u8* ptr);
    extern u8   fn_8011F568(u8* ptr);
    extern u8   fn_8011F550(u8* ptr);
    extern u8   fn_8011F538(u8* ptr);
    extern u32  fn_8011F520(u8* ptr);
    extern u32  fn_8011F508(u8* ptr);
    extern u32  fn_8011F4F0(u8* ptr);
    extern u32  fn_8011F4D8(u8* ptr);
    extern u32  fn_8011F4C0(u8* ptr);
    extern u8   fn_8011F4A8(u8* ptr);
    extern u32  fn_8011F474(u8* ptr, u32 val);
    extern u32  fn_8011F45C(u8* ptr);
    extern u16  fn_8011F228(u8* ptr, u32 val);
    extern u8   fn_8011F1F0(u8* ptr, u32 val);
    extern u8   fn_8011F1B8(u8* ptr, u32 val);
    extern u16  fn_8011F1A0(u8* ptr);
    extern u16  fn_8011F188(u8* ptr);
    extern u16  fn_8011F15C(u8* ptr);
    extern u16  fn_8011F130(u8* ptr);
    extern u16  fn_8011F104(u8* ptr);
    extern u16  fn_8011F0D8(u8* ptr);
    extern u16  fn_8011F0AC(u8* ptr);
    extern u16  fn_8011F080(u8* ptr);
    extern u16  fn_8011F054(u8* ptr);
    extern u16  fn_8011F028(u8* ptr);
    extern u16  fn_8011EFFC(u8* ptr);
    extern u16  fn_8011EFD0(u8* ptr);
    extern u16  fn_8011EFA4(u8* ptr);
    extern u16  fn_8011EF78(u8* ptr);
    extern u16  fn_8011EF4C(u8* ptr);
    extern u16  fn_8011EF20(u8* ptr);
    extern u16  fn_8011EEF4(u8* ptr);
    extern u16  fn_8011EEC8(u8* ptr);
    extern u16  fn_8011EE9C(u8* ptr);
    extern u16  fn_8011EE70(u8* ptr);
    extern u16  fn_8011EE58(u8* ptr);
    extern u8   fn_8011ECEC(u8* ptr);
    extern u8   fn_8011ECC0(u8* ptr);
    extern u8   fn_8011EC94(u8* ptr);
    extern u8   fn_8011EC68(u8* ptr);
    extern u8   fn_8011EC3C(u8* ptr);
    extern u8   fn_8011EB48(u8* ptr);
    extern u8   fn_8011EB1C(u8* ptr);
    extern u8   fn_8011EAF0(u8* ptr);
    extern u8   fn_8011EAC4(u8* ptr);
    extern u8   fn_8011EA98(u8* ptr);
    extern u8   fn_8011EA6C(u8* ptr);
    extern u8   fn_8011EA40(u8* ptr);
    extern u8   fn_8011EA14(u8* ptr);
    extern u8   fn_8011E9E8(u8* ptr);
    extern u8   fn_8011E9BC(u8* ptr);
    extern u8   fn_8011E990(u8* ptr);
    extern u8   fn_8011E964(u8* ptr);
    extern u8   fn_8011E938(u8* ptr);
    extern u8   fn_8011E90C(u8* ptr);
    extern u8   fn_8011EC10(u8* ptr);
    extern u8   fn_8011EBE4(u8* ptr);
    extern u8   fn_8011EBB8(u8* ptr);
    extern u8   fn_8011EB8C(u8* ptr);
    extern u8   fn_8011EB60(u8* ptr);
    extern u8   fn_8011E8F4(u8* ptr);
    extern u8   fn_8011E8DC(u8* ptr);
    extern u8   fn_8011E868(u8* ptr);
    extern u8   fn_8011E850(u8* ptr);
    extern u8   fn_8011E838(u8* ptr);
    extern u8   fn_8011E820(u8* ptr);
    extern u8   fn_8011E808(u8* ptr);
    extern u16  fn_8011E7D8(u8* ptr);
    extern u16  fn_8011E7F0(u8* ptr);
    extern u16  fn_8011E7C0(u8* ptr);
    extern u16  fn_8011EE40(u8* ptr);
    extern u32  fn_8011ED18(u8* ptr);
    extern u32  fn_8011EE28(u8* ptr);
    extern u32  fn_8011EE10(u8* ptr);
    extern u16  fn_8011EDF8(u8* ptr);
    extern u8   fn_8011ED68(u8* ptr);
    extern u32  fn_8011EDC4(u8* ptr, u32 val);
    extern void* fn_8011CE74(u32 idx);
    extern u32  fn_8011CE44(void* tbl, u8 level);
    extern u8   fn_80131574(u32 idx);
    extern u32  fn_8011BEB4(u32 a, u32 b, u32 c, u32 d);
    extern u32  fn_801FDB60(u8* ptr);
    extern u32  fn_801FDB48(u8* ptr);
    extern u32  fn_801FDB14(u8* ptr, u32 val);
    extern u32  fn_801FDAFC(u8* ptr);
    extern u8   fn_801FDAE4(u8* ptr);
    extern u8   fn_801FDACC(u8* ptr);
    extern u8   fn_801FDAB4(u8* ptr);
    extern u8   fn_801FD8E0(u8* ptr);
    extern u32  fn_801FDA9C(u8* ptr);
    extern u32  fn_801FDA84(u8* ptr);
    extern u32  fn_801FDA6C(u8* ptr);
    extern u32  fn_801FDA38(u8* ptr, u32 val);
    extern u32  fn_801FDA20(u8* ptr);
    extern u16  fn_80205184(u8* ptr);
    extern u16  fn_80205224(u8* ptr);
    extern u8   fn_8020990C(u32 ctx, u32 param);
    extern u8   fn_802096E8(u32 ctx);
    extern u32  fn_801FDA08(u8* ptr);
    extern u8   fn_801FD9F0(u8* ptr);
    extern u8   fn_801FD9D8(u8* ptr);
    extern u8   fn_801FD9C0(u8* ptr);
    extern u8   fn_801FD9A8(u8* ptr);
    extern u8   fn_801FD990(u8* ptr);
    extern u8   fn_801FD978(u8* ptr);
    extern u8   fn_801FD960(u8* ptr);
    extern u16  fn_801FD948(u8* ptr);
    extern u32  fn_801FD808(u8* ptr);
    extern u16  fn_801FD064(u8* ptr);
    extern u16  fn_801FD04C(u8* ptr);
    extern u16  fn_801FD034(u8* ptr);
    extern u16  fn_801FD01C(u8* ptr);
    extern u16  fn_801FD004(u8* ptr);
    extern u16  fn_801FCFEC(u8* ptr);
    extern s16  fn_801FCFD4(u8* ptr);
    extern u16  fn_801FCFBC(u8* ptr);
    extern u16  fn_801FCFA4(u8* ptr);
    extern u32  fn_801FCF8C(u8* ptr);
    extern u8   fn_801FD7E0(u8* ptr);
    extern u16  fn_801FD7C8(u8* ptr);
    extern u16  fn_801FD7B0(u8* ptr);
    extern u8   fn_801FD798(u8* ptr);
    extern s16  fn_801FD684(u8* ptr, u8 idx);
    extern u32  fn_801FD648(u8* ptr);
    extern u16  fn_801FD614(u8* ptr, u8 idx);
    extern u16  fn_801FD5D8(u8* ptr);
    extern u32  fn_801FD5B0(u8* ptr);
    extern s16  fn_801FD780(u8* ptr);
    extern u16  fn_801FD768(u8* ptr);
    extern s16  fn_801FD750(u8* ptr);
    extern u16  fn_801FD738(u8* ptr);
    extern u8   fn_801FD598(u8* ptr);
    extern u8   fn_801FD580(u8* ptr);
    extern u8   fn_801FD568(u8* ptr);
    extern u8   fn_801FD550(u8* ptr);
    extern u8   fn_801FD538(u8* ptr);
    extern u8   fn_801FD520(u8* ptr);
    extern u8   fn_801FD508(u8* ptr);
    extern u8   fn_801FD4F0(u8* ptr);
    extern u8   fn_801FD4D8(u8* ptr);
    extern u8   fn_801FD4C0(u8* ptr);
    extern u8   fn_801FD4A8(u8* ptr);
    extern u8   fn_801FD490(u8* ptr);
    extern u8   fn_801FD478(u8* ptr);
    extern u8   fn_801FD460(u8* ptr);
    extern u8   fn_801FD448(u8* ptr);
    extern u8   fn_801FD430(u8* ptr);
    extern u8   fn_801FD418(u8* ptr);
    extern u8   fn_801FD400(u8* ptr);
    extern u8   fn_801FD3E8(u8* ptr);
    extern u8   fn_801FD3D0(u8* ptr);
    extern u8   fn_801FD3B8(u8* ptr);
    extern u32  fn_801FD3A0(u8* ptr);
    extern s16  fn_801FD388(u8* ptr);
    extern u16  fn_801FD370(u8* ptr);
    extern s16  fn_801FD358(u8* ptr);
    extern u16  fn_801FD340(u8* ptr);
    extern u8   fn_801FD188(u8* ptr);
    extern s16  fn_801FD160(u8* ptr);
    extern u32  fn_801FD11C(u8* ptr, u32 val);
    extern u8   fn_802062FC(u8* ptr);

    if ((u16)selector == 0 || (u16)selector >= 0x124) {
        return 0;
    }

    if ((u16)selector < 0x6D) {
        obj = (u8*)fn_8011E778(id);
        if (obj == NULL) {
            return 0;
        }
    } else {
        if (obj == NULL) {
            return 0;
        }
    }

    switch ((u16)selector) {
    case 0x01: return fn_8011E760(obj);
    case 0x03: return (u32)(u16)fn_8011E734(obj);
    case 0x04: return (u32)(u16)fn_8011E708(obj);
    case 0x05: return (u32)(u16)fn_8011E6DC(obj);
    case 0x06: return (u32)(u16)fn_8011E6B0(obj);
    case 0x07: return (u32)(u16)fn_8011E684(obj);
    case 0x08: return (u32)(u16)fn_8011E658(obj);
    case 0x0A: return (u32)(u16)fn_8011E62C(obj);
    case 0x0B: return (u32)(u16)fn_8011E600(obj);
    case 0x0C: return (u32)(u16)fn_8011E5D4(obj);
    case 0x0D: return (u32)(u16)fn_8011E5A8(obj);
    case 0x0E: return (u32)(u16)fn_8011E57C(obj);
    case 0x0F: return (u32)(u16)fn_8011E550(obj);
    case 0x10: return (u32)(u16)fn_8011E538(obj);
    case 0x11: return (u32)(u8)fn_8011E520(obj);
    case 0x12: return (u32)(u8)fn_8011E508(obj);
    case 0x13: return (u32)(u8)fn_8011E4F0(obj);
    case 0x14: return (u32)(u16)fn_8011E4D8(obj);
    case 0x15: return (u32)(u16)fn_8011E4A4(obj, d);
    case 0x16: return (u32)(u8)fn_8011E474(obj, d);
    case 0x17: return (u32)(u8)fn_8011E444(obj, d);
    case 0x19: return (u32)(u8)fn_8011E3FC(obj, d);
    case 0x1A: return (u32)(u16)fn_8011E3B4(obj, d);
    case 0x1B: return (u32)(u16)fn_8011E36C(obj, d);
    case 0x1D: return (u32)(u8)fn_8011E324(obj, d);
    case 0x1E: return (u32)(u16)fn_8011E2DC(obj, d);
    case 0x20: return (u32)(u8)fn_8011E2AC(obj, 0x00);
    case 0x21: return (u32)(u8)fn_8011E2AC(obj, 0x01);
    case 0x22: return (u32)(u8)fn_8011E2AC(obj, 0x02);
    case 0x23: return (u32)(u8)fn_8011E2AC(obj, 0x03);
    case 0x24: return (u32)(u8)fn_8011E2AC(obj, 0x04);
    case 0x25: return (u32)(u8)fn_8011E2AC(obj, 0x05);
    case 0x26: return (u32)(u8)fn_8011E2AC(obj, 0x06);
    case 0x27: return (u32)(u8)fn_8011E2AC(obj, 0x07);
    case 0x28: return (u32)(u8)fn_8011E2AC(obj, 0x08);
    case 0x29: return (u32)(u8)fn_8011E2AC(obj, 0x09);
    case 0x2A: return (u32)(u8)fn_8011E2AC(obj, 0x0A);
    case 0x2B: return (u32)(u8)fn_8011E2AC(obj, 0x0B);
    case 0x2C: return (u32)(u8)fn_8011E2AC(obj, 0x0C);
    case 0x2D: return (u32)(u8)fn_8011E2AC(obj, 0x0D);
    case 0x2E: return (u32)(u8)fn_8011E2AC(obj, 0x0E);
    case 0x2F: return (u32)(u8)fn_8011E2AC(obj, 0x0F);
    case 0x30: return (u32)(u8)fn_8011E2AC(obj, 0x10);
    case 0x31: return (u32)(u8)fn_8011E2AC(obj, 0x11);
    case 0x32: return (u32)(u8)fn_8011E2AC(obj, 0x12);
    case 0x33: return (u32)(u8)fn_8011E2AC(obj, 0x13);
    case 0x34: return (u32)(u8)fn_8011E2AC(obj, 0x14);
    case 0x35: return (u32)(u8)fn_8011E2AC(obj, 0x15);
    case 0x36: return (u32)(u8)fn_8011E2AC(obj, 0x16);
    case 0x37: return (u32)(u8)fn_8011E2AC(obj, 0x17);
    case 0x38: return (u32)(u8)fn_8011E2AC(obj, 0x18);
    case 0x39: return (u32)(u8)fn_8011E2AC(obj, 0x1A); /* 0x39 maps to slot 0x1A, skips 0x19 */
    case 0x3A: return (u32)(u8)fn_8011E2AC(obj, 0x1B);
    case 0x3B: return (u32)(u8)fn_8011E2AC(obj, 0x1C);
    case 0x3C: return (u32)(u8)fn_8011E2AC(obj, 0x1D);
    case 0x3D: return (u32)(u8)fn_8011E2AC(obj, 0x1E);
    case 0x3E: return (u32)(u8)fn_8011E2AC(obj, 0x1F);
    case 0x3F: return (u32)(u8)fn_8011E2AC(obj, 0x20);
    case 0x40: return (u32)(u8)fn_8011E2AC(obj, 0x21);
    case 0x41: return (u32)(u8)fn_8011E2AC(obj, 0x22);
    case 0x42: return (u32)(u8)fn_8011E2AC(obj, 0x23);
    case 0x43: return (u32)(u8)fn_8011E2AC(obj, 0x24);
    case 0x44: return (u32)(u8)fn_8011E2AC(obj, 0x25);
    case 0x45: return (u32)(u8)fn_8011E2AC(obj, 0x26);
    case 0x46: return (u32)(u8)fn_8011E2AC(obj, 0x27);
    case 0x47: return (u32)(u8)fn_8011E2AC(obj, 0x28);
    case 0x48: return (u32)(u8)fn_8011E2AC(obj, 0x29);
    case 0x49: return (u32)(u8)fn_8011E2AC(obj, 0x2A);
    case 0x4A: return (u32)(u8)fn_8011E2AC(obj, 0x2B);
    case 0x4B: return (u32)(u8)fn_8011E2AC(obj, 0x2C);
    case 0x4C: return (u32)(u8)fn_8011E2AC(obj, 0x2D);
    case 0x4D: return (u32)(u8)fn_8011E2AC(obj, 0x2E);
    case 0x4E: return (u32)(u8)fn_8011E2AC(obj, 0x2F);
    case 0x4F: return (u32)(u8)fn_8011E2AC(obj, 0x30);
    case 0x50: return (u32)(u8)fn_8011E2AC(obj, 0x31);
    case 0x51: return (u32)(u8)fn_8011E2AC(obj, 0x32);
    case 0x52: return (u32)(u8)fn_8011E2AC(obj, 0x33);
    case 0x53: return (u32)(u8)fn_8011E2AC(obj, 0x34);
    case 0x54: return (u32)(u8)fn_8011E2AC(obj, 0x35);
    case 0x55: return (u32)(u8)fn_8011E2AC(obj, 0x36);
    case 0x56: return (u32)(u8)fn_8011E2AC(obj, 0x37);
    case 0x57: return (u32)(u8)fn_8011E2AC(obj, 0x38);
    case 0x58: return (u32)(u8)fn_8011E2AC(obj, 0x39);
    case 0x59: return (u32)(u8)fn_8011E2AC(obj, 0x3A);
    case 0x5A: return fn_8011E264(obj, d);
    case 0x5B: return fn_8011E21C(obj, d);
    case 0x5C: return (u32)(u8)fn_8011E1D4(obj, d);
    case 0x5D: return fn_8011E1BC(obj);
    case 0x5E: return (u32)(u16)fn_8011E1A4(obj);
    case 0x5F: return (u32)(u16)fn_8011E18C(obj);
    case 0x60: return fn_8011E174(obj);
    case 0x61: return (u32)(u16)fn_8011E15C(obj);
    case 0x62: return fn_8011E128(obj);
    case 0x63: return fn_8011E0F4(obj);
    case 0x64: return (u32)(u16)fn_8011E0DC(obj);
    case 0x65: return (u32)(u16)fn_8011E0C4(obj);
    case 0x66: return fn_8011E0AC(obj);
    case 0x68: return (u32)(u16)fn_8011E078(obj, d);
    case 0x69: return (u32)(u8)fn_8011E048(obj, d);
    case 0x6A: return (u32)(u16)fn_8011E030(obj);
    case 0x6B: return (u32)(u8)fn_8011E018(obj);
    case 0x6C: return (u32)(u8)fn_8011E000(obj);
    case 0x6E: return (u32)(u16)fn_8011F5C8(obj);
    case 0x6F: return fn_8011F5B0(obj);
    case 0x70: return fn_8011F598(obj);
    case 0x71: return (u32)(u16)fn_8011F580(obj);
    case 0x72: return (u32)(u8)fn_8011F568(obj);
    case 0x73: return (u32)(u8)fn_8011F550(obj);
    case 0x74: return (u32)(u8)fn_8011F538(obj);
    case 0x75: return fn_8011F520(obj);
    case 0x76: return fn_8011F508(obj);
    case 0x77: return fn_8011F4F0(obj);
    case 0x78: return fn_8011F4D8(obj);
    case 0x79: return fn_8011F4C0(obj);
    case 0x7A: return (u32)(u8)fn_8011F4A8(obj);

    case 0x7B: { /* is_species_zero: recursive self-call to check if species (0x83) is zero */
        u16 species = (u16)pokemonGetStatus(obj, 0, 0x83, 0);
        return (u32)(u8)(species == 0 ? 1 : 0);
    }

    case 0x7C: return fn_8011F474(obj, d);
    case 0x7D: return fn_8011F45C(obj);
    case 0x7F: return (u32)(u16)fn_8011F228(obj, d);
    case 0x80: return (u32)(u8)fn_8011F1F0(obj, d);
    case 0x81: return (u32)(u8)fn_8011F1B8(obj, d);
    case 0x82: return (u32)(u16)fn_8011F1A0(obj);
    case 0x83: return (u32)(u16)fn_8011F188(obj);
    case 0x87: return (u32)(u16)fn_8011F15C(obj);
    case 0x88: return (u32)(u16)fn_8011F130(obj);
    case 0x89: return (u32)(u16)fn_8011F104(obj);
    case 0x8A: return (u32)(u16)fn_8011F0D8(obj);
    case 0x8B: return (u32)(u16)fn_8011F0AC(obj);
    case 0x8C: return (u32)(u16)fn_8011F080(obj);
    case 0x8D: return (u32)(u16)fn_8011F054(obj);
    case 0x8E: return (u32)(u16)fn_8011F028(obj);
    case 0x8F: return (u32)(u16)fn_8011EFFC(obj);
    case 0x90: return (u32)(u16)fn_8011EFD0(obj);
    case 0x91: return (u32)(u16)fn_8011EFA4(obj);
    case 0x92: return (u32)(u16)fn_8011EF78(obj);
    case 0x93: return (u32)(u16)fn_8011EF4C(obj);
    case 0x94: return (u32)(u16)fn_8011EF20(obj);
    case 0x95: return (u32)(u16)fn_8011EEF4(obj);
    case 0x96: return (u32)(u16)fn_8011EEC8(obj);
    case 0x97: return (u32)(u16)fn_8011EE9C(obj);
    case 0x98: return (u32)(u16)fn_8011EE70(obj);
    case 0x99: return (u32)(u16)fn_8011EE58(obj);
    case 0x9C: return (u32)(u8)fn_8011ECEC(obj);
    case 0x9D: return (u32)(u8)fn_8011ECC0(obj);
    case 0x9E: return (u32)(u8)fn_8011EC94(obj);
    case 0x9F: return (u32)(u8)fn_8011EC68(obj);
    case 0xA0: return (u32)(u8)fn_8011EC3C(obj);
    case 0xA1: return (u32)(u8)fn_8011EB48(obj);
    case 0xA3: return (u32)(u8)fn_8011EB1C(obj);
    case 0xA4: return (u32)(u8)fn_8011EAF0(obj);
    case 0xA5: return (u32)(u8)fn_8011EAC4(obj);
    case 0xA6: return (u32)(u8)fn_8011EA98(obj);
    case 0xA7: return (u32)(u8)fn_8011EA6C(obj);
    case 0xA8: return (u32)(u8)fn_8011EA40(obj);
    case 0xA9: return (u32)(u8)fn_8011EA14(obj);
    case 0xAA: return (u32)(u8)fn_8011E9E8(obj);
    case 0xAB: return (u32)(u8)fn_8011E9BC(obj);
    case 0xAC: return (u32)(u8)fn_8011E990(obj);
    case 0xAD: return (u32)(u8)fn_8011E964(obj);
    case 0xAE: return (u32)(u8)fn_8011E938(obj);
    case 0xAF: return (u32)(u8)fn_8011E90C(obj);
    case 0xB0: return (u32)(u8)fn_8011EC10(obj);
    case 0xB1: return (u32)(u8)fn_8011EBE4(obj);
    case 0xB2: return (u32)(u8)fn_8011EBB8(obj);
    case 0xB3: return (u32)(u8)fn_8011EB8C(obj);
    case 0xB4: return (u32)(u8)fn_8011EB60(obj);
    case 0xB5: return (u32)(u8)fn_8011E8F4(obj);
    case 0xB6: return (u32)(u8)fn_8011E8DC(obj);
    case 0xB7: return (u32)(u8)fn_8011E868(obj);
    case 0xB8: return (u32)(u8)fn_8011E850(obj);
    case 0xB9: return (u32)(u8)fn_8011E838(obj);

    case 0xBA: { /* gender determination */
        u32 personality;
        u32 gender_ratio;
        s32 gender_val;
        s32 gender_class;
        u8 result;

        if (obj == NULL) {
            return (u32)(u8)2;
        }
        personality = pokemonGetStatus(obj, 0, 0x6F, 0);
        {
            u16 species_id = (u16)pokemonGetStatus(obj, 0, 0x6E, 0);
            gender_ratio = (u16)pokemonGetStatus(NULL, (u32)species_id, 0x13, 0);
        }
        if (obj == NULL) {
            return (u32)(u8)2;
        }
        {
            u16 species_id2 = (u16)pokemonGetStatus(obj, 0, 0x6E, 0);
            gender_val = (u16)pokemonGetStatus(NULL, (u32)species_id2, 0x13, 0);
            if (gender_val == (u16)fn_80131574(0)) {
                gender_class = 0;
            } else if (gender_val == (u16)fn_80131574(1)) {
                gender_class = 1;
            } else if (gender_val == (u16)fn_80131574(2)) {
                gender_class = 2;
            } else {
                gender_class = -1;
            }
        }
        if ((s8)gender_class < 0) {
            if (gender_ratio > (u8)personality) {
                gender_class = 1;
            } else {
                gender_class = 0;
            }
        }
        result = (u8)gender_class;
        return (u32)result;
    }

    case 0xBB: return (u32)(u8)fn_8011E820(obj);
    case 0xBC: return (u32)(u8)fn_8011E808(obj);
    case 0xBD: return (u32)(u16)fn_8011E7D8(obj);
    case 0xBE: return (u32)(u16)fn_8011E7F0(obj);

    case 0xBF: { /* nature = personality % 25 */
        u32 pv = pokemonGetStatus(obj, 0, 0x6F, 0);
        return (u32)(u8)(pv % 25);
    }

    case 0xC0: { /* level from exp */
        u16 species_id;
        u8 growth_type;
        u32 exp;
        void* growth_tbl;
        s32 level;
        u8 result;

        species_id = (u16)pokemonGetStatus(obj, 0, 0x6E, 0);
        growth_type = (u8)pokemonGetStatus(NULL, (u32)species_id, 0x11, 0);
        exp = pokemonGetStatus(obj, 0, 0x79, 0);
        growth_tbl = fn_8011CE74((u32)growth_type);
        if (growth_tbl == NULL) {
            return (u32)(u8)0;
        }
        for (level = 1; level < 0x65; level++) {
            if (fn_8011CE44(growth_tbl, (u8)level) > exp) {
                break;
            }
        }
        result = (u8)(level - 1);
        return (u32)result;
    }

    case 0xC1: { /* shiny check */
        u32 tid;
        u32 pid;
        u32 xv;
        u8 result;

        if (obj == NULL) {
            return (u32)(u8)0;
        }
        tid = pokemonGetStatus(obj, 0, 0x75, 0);
        pid = pokemonGetStatus(obj, 0, 0x6F, 0);
        xv = (tid >> 16) ^ (tid & 0xFFFF) ^ (pid >> 16) ^ (pid & 0xFFFF);
        result = (u8)(xv < 8 ? 1 : 0);
        return (u32)result;
    }

    case 0xC2: return (u32)(u16)fn_8011E7C0(obj);
    case 0xC3: return (u32)(u16)fn_8011EE40(obj);
    case 0xC4: return fn_8011ED18(obj);
    case 0xC5: return fn_8011EE28(obj);
    case 0xC6: return fn_8011EE10(obj);
    case 0xC7: return (u32)(u16)fn_8011EDF8(obj);
    case 0xC8: return (u32)(u8)fn_8011ED68(obj);
    case 0xC9: return fn_8011EDC4(obj, d);
    case 0xCB: return fn_801FDB60(obj);
    case 0xCC: return fn_801FDB48(obj);
    case 0xCD: return fn_801FDB14(obj, d);
    case 0xCE: return (u32)(s32)(s16)fn_801FDAFC(obj); /* extsh */
    case 0xCF: return (u32)(u8)fn_801FDAE4(obj);
    case 0xD0: return (u32)(u8)fn_801FDACC(obj);
    case 0xD1: return (u32)(u8)fn_801FDAB4(obj);
    case 0xD2: return (u32)(u8)fn_801FD8E0(obj);
    case 0xD5: return fn_801FDA9C(obj);
    case 0xD6: return fn_801FDA84(obj);
    case 0xD7: return fn_801FDA6C(obj);
    case 0xD8: return fn_801FDA38(obj, d);
    case 0xD9: return fn_801FDA20(obj);
    case 0xDA: return (u32)(u16)fn_80205184(obj);
    case 0xDB: return (u32)(u16)fn_80205224(obj);

    case 0xDC: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return fn_8011BEB4(ctx, 0, 0x2C, 0);
    }
    case 0xDD: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return fn_8011BEB4(ctx, 0, 0x2B, 0);
    }
    case 0xDE: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return (u32)(u8)fn_8020990C(ctx, d);
    }
    case 0xE0: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return (u32)(u8)fn_802096E8(ctx);
    }
    case 0xE1: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return fn_8011BEB4(ctx, 0, 0x2D, 0);
    }
    case 0xE2: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return fn_8011BEB4(ctx, 0, 0x2F, 0);
    }
    case 0xE3: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return fn_8011BEB4(ctx, 0, 0x29, 0);
    }
    case 0xE4: {
        u32 ctx = pokemonGetStatus(obj, 0, 0xD9, 0);
        return fn_8011BEB4(ctx, 0, 0x2E, 0);
    }

    case 0xE5: return fn_801FDA08(obj);
    case 0xE6: return (u32)(u8)fn_801FD9F0(obj);
    case 0xE7: return (u32)(u8)fn_801FD9D8(obj);
    case 0xE8: return (u32)(u8)fn_801FD9C0(obj);
    case 0xE9: return (u32)(u8)fn_801FD9A8(obj);
    case 0xEA: return (u32)(u8)fn_801FD990(obj);
    case 0xEB: return (u32)(u8)fn_801FD978(obj);
    case 0xEC: return (u32)(u8)fn_801FD960(obj);
    case 0xED: return (u32)(u16)fn_801FD948(obj);
    case 0xEE: return fn_801FD808(obj);
    case 0xEF: return (u32)(u16)fn_801FD064(obj);
    case 0xF0: return (u32)(u16)fn_801FD04C(obj);
    case 0xF1: return (u32)(u16)fn_801FD034(obj);
    case 0xF2: return (u32)(u16)fn_801FD01C(obj);
    case 0xF3: return (u32)(u16)fn_801FD004(obj);
    case 0xF4: return (u32)(u16)fn_801FCFEC(obj);
    case 0xF5: return (u32)(s32)(s16)fn_801FCFD4(obj); /* extsh */
    case 0xF6: return (u32)(u16)fn_801FCFBC(obj);
    case 0xF7: return (u32)(u16)fn_801FCFA4(obj);
    case 0xF8: return fn_801FCF8C(obj);
    case 0xF9: return (u32)(u8)fn_801FD7E0(obj);
    case 0xFA: return (u32)(u16)fn_801FD7C8(obj);
    case 0xFB: return (u32)(u16)fn_801FD7B0(obj);
    case 0xFC: return (u32)(u8)fn_801FD798(obj);
    case 0xFD: return (u32)(s32)(s16)fn_801FD684(obj, (u8)d); /* extsh */
    case 0xFE: return fn_801FD648(obj);
    case 0xFF: return (u32)(u16)fn_801FD614(obj, (u8)d);
    case 0x100: return (u32)(u16)fn_801FD5D8(obj);
    case 0x101: return fn_801FD5B0(obj);
    case 0x102: return (u32)(s32)(s16)fn_801FD780(obj); /* extsh */
    case 0x103: return (u32)(u16)fn_801FD768(obj);
    case 0x104: return (u32)(s32)(s16)fn_801FD750(obj); /* extsh */
    case 0x105: return (u32)(u16)fn_801FD738(obj);
    case 0x106: return (u32)(u8)fn_801FD598(obj);
    case 0x107: return (u32)(u8)fn_801FD580(obj);
    case 0x108: return (u32)(u8)fn_801FD568(obj);
    case 0x109: return (u32)(u8)fn_801FD550(obj);
    case 0x10A: return (u32)(u8)fn_801FD538(obj);
    case 0x10B: return (u32)(u8)fn_801FD520(obj);
    case 0x10C: return (u32)(u8)fn_801FD508(obj);
    case 0x10D: return (u32)(u8)fn_801FD4F0(obj);
    case 0x10E: return (u32)(u8)fn_801FD4D8(obj);
    case 0x10F: return (u32)(u8)fn_801FD4C0(obj);
    case 0x110: return (u32)(u8)fn_801FD4A8(obj);
    case 0x111: return (u32)(u8)fn_801FD490(obj);
    case 0x112: return (u32)(u8)fn_801FD478(obj);
    case 0x113: return (u32)(u8)fn_801FD460(obj);
    case 0x114: return (u32)(u8)fn_801FD448(obj);
    case 0x115: return (u32)(u8)fn_801FD430(obj);
    case 0x116: return (u32)(u8)fn_801FD418(obj);
    case 0x117: return (u32)(u8)fn_801FD400(obj);
    case 0x118: return (u32)(u8)fn_801FD3E8(obj);
    case 0x119: return (u32)(u8)fn_801FD3D0(obj);
    case 0x11A: return (u32)(u8)fn_801FD3B8(obj);
    case 0x11B: return fn_801FD3A0(obj);
    case 0x11C: return (u32)(s32)(s16)fn_801FD388(obj); /* extsh */
    case 0x11D: return (u32)(u16)fn_801FD370(obj);
    case 0x11E: return (u32)(s32)(s16)fn_801FD358(obj); /* extsh */
    case 0x11F: return (u32)(u16)fn_801FD340(obj);
    case 0x120: return (u32)(u8)fn_801FD188(obj);
    case 0x121: return (u32)(s32)(s16)fn_801FD160(obj); /* extsh */
    case 0x122: return fn_801FD11C(obj, d);
    case 0x123: return (u32)(u8)fn_802062FC(obj);

    default:
        return 0;
    }
}
#endif
