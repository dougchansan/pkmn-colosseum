#ifndef GAME_BATTLE_BATTLE_WAZA_TYPES_H
#define GAME_BATTLE_BATTLE_WAZA_TYPES_H

/**
 * @file battle_waza_types.h
 * @brief Shared types and cross-TU declarations for the waza (move animation)
 *        system, split across game/mail.c, game/mailMain.c,
 *        game/wazaSequenceCamera.c, game/wazaViewer.c,
 *        game/wazaSequenceEntry.c, game/wazaSequenceSys.c,
 *        game/wazaSequenceSys_tail.c,
 *        game/wazaSequence.c and game/sequence.c (originally one
 *        CodeCandidate bucket: game/battle/battle_waza.c,
 *        0x801D1338 - 0x801DE698).
 *
 * These declarations span matching and partially decompiled units; this
 * header lets each split TU see the others' signatures without
 * redeclaration mismatches.
 */

#include "game/battle/battle.h"

/* =========================================================================
 * Waza Entry Structure (temporary - based on 0x2C byte size)
 * ========================================================================= */

typedef struct WazaEntry {
    /* 0x00 */ u8  sendCondType;
    /* 0x01 */ u8  pad_01[3];
    /* 0x04 */ u16 field_04;
    /* 0x06 */ u16 sendRate;
    /* 0x08 */ f32 duration;       /* Duration of the entry */
    /* 0x0C */ u32 field_0C;       /* Unknown field */
    /* 0x10 */ u32 field_10;       /* Unknown field */
    /* 0x14 */ u32 field_14;
    /* 0x18 */ u32 field_18;
    /* 0x1C */ s32 sendCondition;
    /* 0x20 */ s32 field_20;
    /* 0x24 */ s32 field_24;      /* Field being accessed by fn_801D14C0 */
    /* 0x28 */ s32 resourceID;    /* Resource ID */
    /* 0x2C */ /* end of struct */
} WazaEntry;

typedef struct WazaSequenceNode WazaSequenceNode;
typedef struct WazaSequence WazaSequence;
typedef struct WazaEffect WazaEffect;
typedef WazaEffect WazaSequenceOwner;
typedef struct GSpart GSpart;
typedef struct GSvec GSvec;

struct WazaSequenceNode {
    /* 0x00 */ s32 linkKey;
    /* 0x04 */ s32 kind;
    /* 0x08 */ s32 linkedEntryKey;
    /* 0x0C */ s32 sourceIndex;
    /* 0x10 */ s32 targetIndex;
    /* 0x14 */ s32 timingIndex;
    /* 0x18 */ s32 state;
    /* 0x1C */ u32 flags;
    /* 0x20 */ s32 attachment;
    /* 0x24 */ s32 partIndex;
    /* 0x28 */ s32 positionType;
    /* 0x2C */ s32 timing[0x10];
    /* 0x6C */ u32 runtimeState;
    /* 0x70 */ s32 startTime;
    /* 0x74 */ s32 currentTime;
    /* 0x78 */ s32 resourceId;
    /* 0x7C */ u32 runtimeFlags;
    /* 0x80 */ s32 field_80;
    /* 0x84 */ s32 animationMode;
    /* 0x88 */ void* resource;
    /* 0x8C */ s32 textureAnimationMode;
    /* 0x90 */ s32 textureAnimation;
    /* 0x94 */ s32 restoreTransform;
    /* 0x98 */ u8 pad_98[8];
    /* 0xA0 */ s32 attached;
    /* 0xA4 */ void* model;
    /* 0xA8 */ WazaSequenceNode* next;
    /* 0xAC */ WazaSequenceNode* previous;
    /* 0xB0 */ WazaSequence* sequence;
};

struct WazaSequence {
    /* 0x00 */ u32 state;
    /* 0x04 */ u8 pad_04[4];
    /* 0x08 */ u32 flags;
    /* 0x0C */ s32 kind;
    /* 0x10 */ s32 field_10;
    /* 0x14 */ u8 active;
    /* 0x15 */ u8 stopping;
    /* 0x16 */ u8 cameraActive;
    /* 0x17 */ u8 pad_17[0x0D];
    /* 0x24 */ WazaSequenceNode* firstNode;
    /* 0x28 */ u8 pad_28[2];
    /* 0x2A */ u16 handle;
    /* 0x2C */ u8 pad_2C[2];
    /* 0x2E */ u16 animationMode;
    /* 0x30 */ u8 pad_30[0x0C];
    /* 0x3C */ WazaSequenceOwner* owner;
};

typedef struct WazaPartyScratch {
    u16 seqIds[0x200];
    u16 count;
    u8 receivedFlags[0x40];
    u8 selectedHandle;
    u8 currentHandle;
} WazaPartyScratch;

/* Waza effect data table entry (stride 0xD4) */
typedef struct WazaEffectTblEntry {
    /* 0x00 */ u8  pad_00[0x90];
    /* 0x90 */ s32 field_90;                 /* set by fn_801DD100 */
    /* 0x94 */ u8  pad_94[0xD4 - 0x94];
} WazaEffectTblEntry;

/* Waza battle effect object (size 0x8C) */
struct WazaEffect {
    /* 0x00 */ u8 pad_00[0x10];
    /* 0x10 */ s32 scale_selector;          /* selects scale value */
    /* 0x14 */ u8 pad_14[0x02];
    /* 0x16 */ u8 motionBusy;
    /* 0x17 */ u8 sequenceEnabled;
    /* 0x18 */ u8 flags;                     /* trajectory/flags byte */
    /* 0x19 */ u8 pad_19[0x0B];
    /* 0x24 */ struct GSmodel* model;
    /* 0x28 */ void* particleBank;
    /* 0x2C */ WazaEffectTblEntry* table;    /* effect data table */
    /* 0x30 */ u8 pad_30[0x02];
    /* 0x32 */ u16 index;                    /* table index */
    /* 0x34 */ u16 field_34;                 /* set by fn_801DD100 */
    /* 0x36 */ u8 pad_36[0x32];
    /* 0x68 */ WazaSequence* sequenceList;
    /* 0x6C */ WazaSequence* currentSequence;
    /* 0x70 */ u8 pad_70[0x04];
    /* 0x74 */ u8 active;
    /* 0x75 */ u8 animationActive;
    /* 0x76 */ u8 pad_76[0x0A];
    /* 0x80 */ u8* field_80;                 /* secondary particle node */
    /* 0x84 */ u8* effect_handle;            /* particle node */
    /* 0x88 */ u8 pad_88[0x04];
};

typedef struct WazaFxNode {
    u8 pad_00[0x2C];
    u16 field_2C;
    u16 field_2E;
    u8 pad_30[0x04];
    struct WazaFxNode* next;
} WazaFxNode;

typedef struct WazaFxOwner {
    u8 pad_00[0x68];
    WazaFxNode* first_child;
} WazaFxOwner;

/* =========================================================================
 * External function declarations (other, already-split, translation units)
 * ========================================================================= */

/* CRT */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* Engine core */
extern void  GSlogWrite(const char* fmt, ...);           /* GSlog_Print */
extern s32   fn_800D37CC(void);                            /* GSrandom_Get */
extern s32   fn_800D3088(void);                            /* GSgfx tick */
extern void* fn_800DB940(u32 size);                        /* GSmem_Alloc */
extern void  fn_800DB9A4(void* ptr);                       /* GSmem_Free */

/* Scene management */
extern void  menuCloseCustom(s32 objID, s32 arg1, s32 arg2);  /* release scene object */
extern u8    menuIsCheck(s32 objID);                       /* check scene object active */
extern void* fn_801025C0(s32 objID);                       /* get scene object pointer */

/* HSD model/animation */
extern void  fn_80362D0C(void* jobj);                      /* HSD_JObjAnimAll */
extern void  fn_80362E40(void* jobj, f32 frame);           /* HSD_JObjReqAnimAll */
extern void* fn_80363B8C(void* data, s32 idx);             /* HSD_JObjLoadJoint */
extern void  fn_80363CF4(void* jobj);                      /* HSD_JObjRemoveAll */
extern void  fn_8036A384(void* jobj, f32 x, f32 y, f32 z); /* HSD_JObjSetTranslate */

/* Sound */
extern void  fn_801659FC(s32 sndID, s32 fadeTime, s32 volume); /* sndPlay */
extern void  soundStop(s32 sndID, s32 volume);              /* soundStop */
extern void  fn_801669E4(s32 handle, s32 fadeTime, s32 volume);
extern void  heroMoveAddStepCallback(void* callback, s32 arg);

/* Particle system */
extern void GSthreadTerminateGroup(u32 priority);          /* stop particle system */
extern void* fn_80121E24(s32 effectID, f32 x, f32 y, f32 z); /* spawn particle effect */
extern void  fn_80121F3C(void* particle);                  /* destroy particle */
extern void  fn_80122048(void* particle, f32 x, f32 y, f32 z); /* set particle pos */

/* Battle grid/scene */
extern void* savedataGetStatus(s32 side, s32 slotType);
extern void* fadeEffectDokuStop(s32 slot);                 /* get grid slot model (was fn_801C4078) */
extern f32   fadeEffectHookFunction_Doku_Init(s32 slot);   /* get slot X (was fn_801C4814) */
extern f32   fadeEffectHookFunction_fadein_Init(s32 slot);                        /* get slot Y */
extern f32   fadeEffectHookFunction_trainer_Init(s32 slot);                        /* get slot Z */

/* Waza effect functions (engine callbacks) */
extern u8*   fn_801190DC(u8* texture, u32 selector, u32 subid);
extern void  fn_80118C88(void* particleNode, u32 visible);
extern GSpart* GSmodelGetPart(struct GSmodel* model, s32 partIndex);
void  set__5GSvecFfff(f32* vec, f32 x, f32 y, f32 z);
extern void  fn_80118FB0(u8* particleNode, GSpart* part, u32 state,
                         u32 byte5, u32 initFromZero, u32 attachModel);
extern void  fn_80118D18(void* particleNode, u8 enabled);
extern void  fn_80118DE0(u8* particleNode, f32* scale, u32 applyToGenerator,
                         u32 angleRadiusScale);
extern void  GSpartFree(GSpart* part);
extern u8    GSmodelCenterNull(struct GSmodel* model);
extern s32   fn_800EE0E8(struct GSmodel* model);
extern void  fn_80118CAC(void* particleNode, u8 enabled);

/* Additional externs (other TUs) referenced from scattered file-scope
 * declarations in the original battle_waza.c; consolidated here so every
 * split TU sees them regardless of which one the declaration used to sit
 * physically next to. */
extern void  GSscene_SetMode(s32 arg);
extern void  cameraStopAnime(void* arg);
extern void  fn_801765F4(s32 arg);
extern s32   fn_800057A8(void);
extern void  _threadSwitch(void);
extern s32   fn_8017B2CC(s32 id);
extern void  fn_800F915C(s32 id);
extern void  fn_8017B1CC(s32 id);
extern u32   GSmodelIsRootNullAdded(struct GSmodel* model);
extern void  GSmodelGetRootPosition(struct GSmodel* model, GSvec* position);
extern void  GSmodelAddNull(struct GSmodel* model, const GSvec* position,
                            const GSvec* rotation, const GSvec* scale);
extern void  GSmodelRemoveNull(struct GSmodel* model);
extern void  GSmodelStopAnimation(struct GSmodel* model);
extern void  GSmodelSetVisibility(struct GSmodel* model, u8 visible);
extern void  GSmodelForceAnimTransformUpdate(struct GSmodel* model);
extern u32   fn_800E3CBC(struct GSmodel* model);
extern void  GSmodelDrawModel(struct GSmodel* model, u32 flags);
extern void  GSmodelLinkToGSparticleBank(struct GSmodel* model, void* bank);
extern void  fn_800E3CC8(struct GSmodel* model, u8 enable);
extern void  fn_80118874(u8* resource, u32 notify);

/* Data symbols (lbl_*, in .sdata/.sdata2/.bss of neighboring TUs) referenced
 * from scattered file-scope declarations in the original battle_waza.c. */
extern u32*  lbl_80478E98;              /* waza context pointer */
extern WazaEntry* lbl_80478E9C;          /* waza entry array pointer */
extern s32   lbl_80478CB8;
extern u32   lbl_8036E0E0[20];
extern s32   lbl_80467390[];
extern void* lbl_8047B3EC;
extern void* lbl_8047B3F0;
extern s32   lbl_8047B410;
extern u8    lbl_80467CC0[];
extern u8    lbl_8047B3F4;
extern u8    lbl_804673F8[];
extern s32   lbl_8047B418;
extern s32   lbl_8047B414;
extern u8    lbl_80467C80[];

/* =========================================================================
 * Cross-TU forward declarations for all waza-system functions.
 *
 * These are the functions physically defined across the 8 split TUs
 * (plus 12 defined in the neighboring gs_range_801DE698.c /
 * field_range_801DF790.c units). Declaring them all here -- with the
 * exact signature used at each definition site -- lets any TU call any
 * other without per-file extern bookkeeping, and avoids the
 * MWCC redeclaration-mismatch errors that ad hoc function-local externs
 * can cause.
 * ========================================================================= */

extern u16 fn_801D1338(void);
extern void* fn_801D1364(u16 handle, s32 idx);
extern u32 fn_801D139C(s32 idx);
extern u32 fn_801D13E4(s32 idx);
extern u32 fn_801D142C(s32 idx);
extern u32 mailGetNbMailData(void);
extern s32 fn_801D147C(s32 idx);
extern s32 fn_801D14C0(s32 idx);
extern s32 fn_801D1504(s32 idx);
extern s32 mailGetSendCondition(s32 idx);
extern u32 mailGetSendCondType(s32 idx);
extern u32 mailGetSendRate(s32 idx);
extern u32 fn_801D1618(void);
extern u32 fn_801D1620(u32 idx);
extern u32 fn_801D1650(u32 idx);
extern void fn_801D167C(u8 handle);
extern u8 fn_801D16C4(void);
extern u32 mailGetAttachFileGroup(s32 idx);
extern u8 fn_801D1734(s32 sequence, s32 index);
extern u16 fn_801D1864(s32 sequence, s32 index);
extern u32 fn_801D19A4(s32 idx);
extern u32 mailGetContents(s32 idx);
extern u32 mailGetSenderName(s32 idx);
extern u32 mailGetSubject(s32 idx);
extern void fn_801D1B10(s32 handle);
extern u8 mailGetSortMode(void);
extern BOOL fn_801D1B78(s32 mailId);
extern BOOL fn_801D1C20(s32 mailId);
extern BOOL mailChkReceiveMail(s32 idx);
extern BOOL mailAddMailbox(s32 mailId);
extern s32 mailGetReceiveNumber(s32 mailId);
extern BOOL fn_801D1E50(s32 seqHandle);
extern s32 mailGetMailIDInMailbox(s32 idx);
extern u16 mailGetNbMailInMailbox(void);
extern void mailInitMailbox(WazaPartyScratch* mailbox);
extern void mailMainSendByScrpt(s32 seqHandle, f32 alpha, f32 speed);
extern void mailMainSendAllMail(s32 seqHandle, f32 targetScale, f32 speed);
extern void fn_801D228C(s32 seqHandle);
extern void mailMainReceiveTerminate(void);
extern void fn_801D2404(void);
extern void mailMainReceiveStart(s32 seqHandle, s32 slot, s32 boneIdx);
extern s32 chkMailSend(s32 seqHandle, f32 step);
extern void cbStep(s32 moveID, s32 hitCount);
extern void mailMainInit(void);
extern void* wazaSequenceCameraGetPattern__Fbi(u8 shortTable, s32 flags);
extern void* fn_801D2C6C(void);
extern void fn_801D2C74(void* owner);
extern void fn_801D2D28(void);
extern void fn_801D2F94(void);
extern void fn_801D301C(void);
extern void fn_801D3034(void* state);
extern void battleCameraStartWaza(void* owner, void* sequence);
extern void _wazaSequenceCameraDoPosition__FP13ModelSequenceP24wazaSequenceCameraParamsfb(void);
extern void _wazaSequenceCameraDoDollyPosition__FP21TemplateExpFileHeaderP24wazaSequenceCameraParamsfb(void);
extern void _wazaSequenceCameraDoFOV__FP13ModelSequenceP24wazaSequenceCameraParamsif(void);
extern s32 _wazaSequenceCameraSelectDuration__FUcPff(
    u8 mode, f32* thresholds, s32 duration);
extern void _wazaSequenceCameraSelectMotion__FP13ModelSequenceP12WazaSequenceP24wazaSequenceCameraParams(
    void* modelSequence, void* wazaSequence, void* cameraParams);
extern void _wazaSequenceCameraCalculateParams__FP13ModelSequenceiP24wazaSequenceCameraParams(
    void* modelSequence, s32 flags, void* cameraParams);
extern void _wazaViewerFinalize(u8 r, u8 g, u8 b, f32 duration);
extern void _wazaViewerUpdate(void);
extern void _wazaViewerInitialize(s32 slot, f32 zoom, f32 speed);
extern void wazaViewerThread(void);
extern void fn_801D56B0(void);
extern void fn_801D58E4(void);
extern void fn_801D5A94(s32 moveID);
extern void fn_801D5DA0(void);
extern void fn_801D603C(void); /* GSgapp per-frame callback; takes no arguments */
extern void fn_801D624C(void);
extern void fn_801D6A64(void);
extern void fn_801D6E58(void* effect);
extern void fn_801D7230(void);
extern void fn_801D744C(u32 bits);
extern void fn_801D7464(void);
extern void fn_801D7B94(void* effect);
extern u8 wazaSequenceEntryStop(void* entry, BOOL immediate);
extern u8 wazaSequenceEntryUpdate(void* entry, s32 elapsed);
extern u8 wazaSequenceEntryStart(void* entry);
extern u8 _wazaSequenceEffectEntryStart(void* entry);
extern u8 _wazaSequenceParticleEntryStart(void* entry);
extern u8 _wazaSequenceModelEntryStart(void* entry);
extern void* fn_801D97F0(void* entry);
extern void fn_801D9950(void* owner, f32* scale, s32 selector);
extern u8 wazaSequencePokemonMotionStart(void* owner, BOOL enabled);
extern u8 fn_801D9E1C(void* obj);
extern void fn_801D9E34(void* obj);
extern void fn_801D9E8C(void* effect);
extern void fn_801DA014(void* effect);
extern void fn_801DA070(void* effect);
extern void fn_801DA224(void* effect, s32 flags);
extern void fn_801DA2C4(void* effect, f32 radius, f32 t);
extern u8 fn_801DA354(void* effect);
extern void fn_801DA36C(void* effect, s32 trajType);
extern void fn_801DA3CC(void* effect, s32 trajType);
extern u32 fn_801DA42C(void* effect);
extern void fn_801DA448(void* effect, u32 visible);
extern void fn_801DA4E8(void* effect, u32 visible);
extern void fn_801DA5AC(void* effect, u8 val);
extern u8 fn_801DA5C4(s32 timeType);
extern u8 fn_801DA698(void* sequence, s32 moveID, s32 variant, s32 timeType);
extern s32 fn_801DA74C(void* sequence, s32 moveID, s32 variant, s32 timeType);
extern void fn_801DA7AC(void);
extern void fn_801DA83C(void* effect);
extern void fn_801DA8C4(void* obj, s32 search_key1, s32 search_key2);
extern void fn_801DA914(void* obj, s32 search_key1, s32 search_key2);
extern s32 fn_801DA94C(void* obj, s32 search_key1, s32 search_key2);
extern void fn_801DA9B4(void* obj, s32 search_key1, s32 search_key2);
extern void fn_801DA9E8(void* sequence, s32 moveID, s32 variant);
extern void fn_801DAAAC(void* effect);
extern f32 fn_801DABAC(void* obj);
extern u32 fn_801DAC24(void* obj);
extern u32 fn_801DAC3C(void* obj);
extern u32 fn_801DAC54(void* obj);
extern u16 fn_801DAC78(void* obj);
extern void wazaSequenceSysRelease(void);
extern void fn_801DADC0(void* context);
extern void fn_801DAEF8(s32 count);
extern int wazaSequenceSysGetResID(void);
extern void fn_801DB088(void);
extern void fn_801DB100(void* obj);
extern void* fn_801DB154(void);
extern void wazaSequenceSysFreeSequenceResource(void* obj);
extern void wazaSequenceSysFreeWazaResource(void* seqData, ...);
extern s32 wazaSequenceSysGetWazaTime(void* owner, void* sequence, s32 timeType);
extern s32 wazaSequenceSysGetModelShadowLight__Fv(void);
extern s32 wazaSequenceSysGetModelShadowCount__Fv(void);
extern void* wazaSequenceSysGetModelShadowList__Fv(void);
extern void wazaSequenceSysResetAnimationExcept(void* except);
extern void fn_801DB8FC(void* entry, u32 drawFlags, u8 modelID);
extern s8 wazaSequenceUpdate(void* sequence);
extern void wazaSequenceApplyStop(void* obj);
extern void fn_801DBC30(void* obj);
extern void wazaSequenceStart(void* sequence);
extern void wazaSequenceFree(void* obj);
extern WazaSequence* fn_801DBFB0(void);
extern u8 wazaSequenceLoadData(void* sequence, void* resource);
extern void wazaSequenceEntryLink(void* sequence, void* entry);
extern void* fn_801DC46C(void* entry, void* data);
extern void* fn_801DC5F0(void* sequence, void* data);
extern void* _wazaSequenceEffectEntryLoad(void* entry, void* data);
extern void* _wazaSequenceParticleEntryLoad(void* sequence, void* entry,
                                            void* data);
extern void* _wazaSequenceModelEntryLoad(void* sequence, void* entry,
                                         void* data);
extern void* fn_801DCDA8(void* obj, s32 fieldEffect);
extern s32 fn_801DCDCC(void* obj);
extern void fn_801DCE0C(void* obj);
extern void fn_801DCEA8(void* obj);
extern void fn_801DCF00(WazaEffect* effect);
extern void fn_801DCF84(void* obj);
extern void fn_801DCFD8(void* obj);
extern void fn_801DD028(void* obj);
extern void fn_801DD078(void* obj);
extern void* GetWaza__12NullSequenceCFUsUs(void* obj, s32 search_key1, s32 search_key2);
extern void fn_801DD100(WazaSequenceOwner* owner, WazaSequence* sequence);
extern void fn_801DD158(void* obj);
extern void fn_801DD23C(void* obj);
extern void fn_801DD3E4(void* obj);
extern BOOL sequenceLoad(void* effect, void* data);
extern u8 fn_801DD5E8(void* effect, void* resource);
extern BOOL fn_801DDB4C(void* owner, void* resource);
extern s32 fn_801DDC10(u16 index, u16 type);
extern BOOL fn_801DDD28(void* owner, u16 group, u16 index, u32 variant);
extern void fn_801DDEE4(void* owner, u16 group, u16 type, u8 variant,
                        u32* resourceGroup, u32* resourceId);
extern BOOL fn_801DE164(s32 slot);
extern void* fn_801DE190(u16 index, void* model, u8 variant);
extern void* fn_801DE418(u16 index);
extern void fn_801DE598(u32 group, u32 resource);
extern void sequenceAnimEndCallback(s32 arg0, s32 arg1);
extern void fn_801DE698(s32 arg0, s32 arg1);
extern void _eyeTexAnimEnded(s32 arg0, s32 arg1);
extern void fn_801DEE14(s32 slot, u32 status);
extern void fn_801DEF0C(void* obj, s32 arg1, s32 arg2);
extern void fn_801DF070(s32 weatherType);
extern s32 fn_801DF160(u8* obj);
extern void fn_801DF1D0(void* obj);
extern void fn_801DF33C(void* obj);
extern void fn_801DF3D4(void* obj);
extern void fn_801DF474(s32 slot, s32 abilityID);
extern void fn_801DF790(s32 slot, s32 itemID);
extern void fn_801DFC30(void);

#endif /* GAME_BATTLE_BATTLE_WAZA_TYPES_H */
