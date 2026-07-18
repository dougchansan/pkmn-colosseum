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
    /* 0x00 */ s32 type;           /* WAZA_ENTRY_* type */
    /* 0x04 */ f32 startFrame;     /* Frame when entry should start */
    /* 0x08 */ f32 duration;       /* Duration of the entry */
    /* 0x0C */ u32 field_0C;       /* Unknown field */
    /* 0x10 */ u32 field_10;       /* Unknown field */
    /* 0x14 */ f32 posY;          /* Position Y */
    /* 0x18 */ f32 posZ;          /* Position Z */
    /* 0x1C */ f32 scale;         /* Scale */
    /* 0x20 */ f32 rotation;      /* Rotation */
    /* 0x24 */ s32 field_24;      /* Field being accessed by fn_801D14C0 */
    /* 0x28 */ s32 resourceID;    /* Resource ID */
    /* 0x2C */ /* end of struct */
} WazaEntry;

typedef struct WazaPartyScratch {
    u16 seqIds[0x200];
    u16 count;
    u8 pad_402[0x40];
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
typedef struct WazaEffect {
    /* 0x00 */ u8 pad_00[0x10];
    /* 0x10 */ s32 scale_selector;          /* selects scale value */
    /* 0x14 */ u8 pad_14[0x04];
    /* 0x18 */ u8 flags;                     /* trajectory/flags byte */
    /* 0x19 */ u8 pad_19[0x0B];
    /* 0x24 */ s32 handle;                   /* owner handle */
    /* 0x28 */ u8 pad_28[0x04];
    /* 0x2C */ union {
        WazaEffectTblEntry* table;           /* effect data table */
        char* table_bytes;
    };
    /* 0x30 */ u8 pad_30[0x02];
    /* 0x32 */ u16 index;                    /* table index */
    /* 0x34 */ u16 field_34;                 /* set by fn_801DD100 */
    /* 0x36 */ u8 pad_36[0x3E];
    /* 0x74 */ u8 active;
    /* 0x75 */ u8 pad_75[0x0B];
    /* 0x80 */ u32 field_80;                 /* secondary motion/effect handle */
    /* 0x84 */ u32 effect_handle;            /* effect handle */
    /* 0x88 */ u8 pad_88[0x04];
} WazaEffect;

/* Sequence-entry ("obj") fields read by fn_801DD100 when linking a waza
 * blend effect to its owner; see wazaSequence.c's wazaSequenceStart for
 * the many other offsets accessed on this same pointer (obj/blendType,
 * not yet consolidated into a full struct). */
typedef struct WazaBlendEntry {
    /* 0x00 */ u8  pad_00[0x0C];
    /* 0x0C */ u32 field_0C;                 /* WazaEffect::table index */
    /* 0x10 */ s32 field_10;                 /* value stored into WazaEffect::table[index].field_90 */
} WazaBlendEntry;

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
extern void  fn_800D3088(void);                            /* GSgfx tick */
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
extern s32   fn_801190DC(s32, s32, s32);
extern void  fn_80118C88(s32, s32);
extern void* GSmodelGetPart(s32, s32);
void  set__5GSvecFfff(void*, f32, f32, f32);
extern void  fn_80118FB0(s32, s32, s32, s32, s32, s32);
extern void  fn_80118D18(s32, s32);
extern void  fn_80118DE0(s32, void*, s32, s32);
extern void  GSpartFree(s32);
extern u8    GSmodelCenterNull(s32);
extern s32   fn_800EE0E8(s32);
extern void  fn_80118CAC(s32, s32);

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
extern void  GSmodelRemoveNull(void* obj);
extern void  GSmodelStopAnimation(void* obj);

/* Data symbols (lbl_*, in .sdata/.sdata2/.bss of neighboring TUs) referenced
 * from scattered file-scope declarations in the original battle_waza.c. */
extern u32*  lbl_80478E98;              /* waza context pointer */
extern u32   lbl_80478E9C;              /* waza entry array pointer */
extern s32   lbl_80478CB8;
extern u32   lbl_8036E0E0[];
extern s32   lbl_80467390[];
extern u32   lbl_8047B3EC;
extern u32   lbl_8047B3F0;
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

extern s32 fn_801D1338(void* wazaCtx);
extern void* fn_801D1364(u16 handle, s32 idx);
extern s32 fn_801D139C(void* entry);
extern u32 fn_801D13E4(void* entry);
extern f32 fn_801D142C(void* entry);
extern u32 mailGetNbMailData(void* entry);
extern s32 fn_801D147C(void* entry);
extern s32 fn_801D14C0(s32 idx);
extern s32 fn_801D1504(void* entry);
extern s32 mailGetSendCondition(void* entry);
extern u32 mailGetSendCondType(void* entry);
extern u32 mailGetSendRate(void* entry);
extern u32 fn_801D1618(void);
extern u32 fn_801D1620(u32 idx);
extern u32 fn_801D1650(u32 idx);
extern void fn_801D167C(u8 handle);
extern u8 fn_801D16C4(void);
extern u32 mailGetAttachFileGroup(s32 idx);
extern void fn_801D1734(s32 attackerSlot, s32 targetSlot, s32 moveID);
extern void fn_801D1864(s32 attackerSlot, s32 targetSlot, s32 moveID, s32 cameraMode);
extern void fn_801D19A4(s32 seqHandle, f32 speed);
extern u32 mailGetContents(s32 idx);
extern u32 mailGetSenderName(s32 idx);
extern u32 mailGetSubject(s32 idx);
extern void fn_801D1B10(s32 handle);
extern u8 mailGetSortMode(void);
extern void fn_801D1B78(s32 seqHandle);
extern void fn_801D1C20(s32 seqHandle);
extern BOOL mailChkReceiveMail(s32 idx);
extern void mailAddMailbox(s32 seqHandle);
extern BOOL fn_801D1E50(s32 seqHandle);
extern s32 mailGetMailIDInMailbox(s32 idx);
extern u16 mailGetNbMailInMailbox(void);
extern void mailInitMailbox(s32 seqHandle, u32 color);
extern void mailMainSendByScrpt(s32 seqHandle, f32 alpha, f32 speed);
extern void mailMainSendAllMail(s32 seqHandle, f32 targetScale, f32 speed);
extern void fn_801D228C(s32 seqHandle, f32 targetRot, f32 speed);
extern void mailMainReceiveTerminate(void);
extern void fn_801D2404(s32 seqHandle, f32 x, f32 y, f32 z, f32 scale, f32 rot);
extern void mailMainReceiveStart(s32 seqHandle, s32 slot, s32 boneIdx);
extern void chkMailSend(s32 seqHandle, f32 velX, f32 velY, f32 velZ);
extern void cbStep(s32 moveID, s32 hitCount);
extern void mailMainInit(void);
extern void wazaSequenceCameraGetPattern__Fbi(void);
extern u32 fn_801D2C6C(void);
extern void fn_801D2C74(s32 moveID);
extern void fn_801D2D28();
extern void fn_801D2F94(void);
extern void fn_801D301C(void);
extern void fn_801D3034(u32 state);
extern void battleCameraStartWaza(void);
extern void _wazaSequenceCameraDoPosition__FP13ModelSequenceP24wazaSequenceCameraParamsfb(void);
extern void _wazaSequenceCameraDoDollyPosition__FP21TemplateExpFileHeaderP24wazaSequenceCameraParamsfb(void);
extern void _wazaSequenceCameraDoFOV__FP13ModelSequenceP24wazaSequenceCameraParamsif(void);
extern void _wazaSequenceCameraSelectDuration__FUcPff(void);
extern void _wazaSequenceCameraSelectMotion__FP13ModelSequenceP12WazaSequenceP24wazaSequenceCameraParams(s32 effectID, s32 slot);
extern void _wazaSequenceCameraCalculateParams__FP13ModelSequenceiP24wazaSequenceCameraParams(s32 modelID, s32 attackerSlot, s32 targetSlot);
extern void _wazaViewerFinalize(u8 r, u8 g, u8 b, f32 duration);
extern void _wazaViewerUpdate(void);
extern void _wazaViewerInitialize(s32 slot, f32 zoom, f32 speed);
extern void wazaViewerThread(s32 slot, s32 motionType);
extern void fn_801D56B0(s32 slot, s32 reactionType);
extern void fn_801D58E4(s32 effectType);
extern void fn_801D5A94(s32 moveID);
extern void fn_801D5DA0(s32 elementType);
extern void fn_801D603C(s32 slot, s32 hitEffectType);
extern void fn_801D624C(void);
extern void fn_801D6A64(void);
extern void fn_801D6E58(void);
extern void fn_801D7230(void);
extern void fn_801D744C(u32 bits);
extern void fn_801D7464(void);
extern void fn_801D7B94(void);
extern void wazaSequenceEntryStop(void* entry);
extern void wazaSequenceEntryUpdate(void* entry);
extern void wazaSequenceEntryStart(void);
extern void _wazaSequenceEffectEntryStart(void* entry, s32 type);
extern void _wazaSequenceParticleEntryStart(void* entry);
extern void _wazaSequenceModelEntryStart(void* entry);
extern void fn_801D97F0(void* entry);
extern void fn_801D9950(s32 slot, s32 motionType);
extern void wazaSequencePokemonMotionStart(s32 slot);
extern u8 fn_801D9E1C(void* obj);
extern void fn_801D9E34(void* obj);
extern void fn_801D9E8C(void* effect);
extern void fn_801DA014(void* effect);
extern void fn_801DA070(void* effect);
extern void fn_801DA224(void* effect, f32 height, f32 t);
extern void fn_801DA2C4(void* effect, f32 radius, f32 t);
extern u8 fn_801DA354(void* effect);
extern void fn_801DA36C(void* effect, s32 trajType);
extern void fn_801DA3CC(void* effect, s32 trajType);
extern u32 fn_801DA42C(void* effect);
extern void fn_801DA448(void* effect, f32 gravity);
extern void fn_801DA4E8(void* effect, f32 drag);
extern void fn_801DA5AC(void* effect, u8 val);
extern void fn_801DA5C4(void* effect, f32 lifetime);
extern BOOL fn_801DA698(void* effect);
extern void fn_801DA74C(void* effect, f32 fadeSpeed);
extern void* fn_801DA7AC(s32 effectType);
extern void fn_801DA83C(void* effect);
extern void fn_801DA8C4(void* obj);
extern void fn_801DA914(void* obj);
extern s32 fn_801DA94C(void* obj);
extern void fn_801DA9B4(void* obj);
extern void fn_801DA9E8(s32 sndID, s32 slot);
extern void fn_801DAAAC(s32 sndID, f32 x, f32 y, f32 z);
extern void fn_801DABAC(s32 sndHandle);
extern u32 fn_801DAC24(void* obj);
extern u32 fn_801DAC3C(void* obj);
extern u32 fn_801DAC54(void* obj);
extern u16 fn_801DAC78(void* obj);
extern void wazaSequenceSysRelease(void);
extern void fn_801DADC0(void);
extern void fn_801DAEF8(s32 count);
extern BOOL wazaSequenceSysGetResID(void);
extern void fn_801DB088(void);
extern void fn_801DB100(void* obj);
extern void* fn_801DB154(void);
extern void wazaSequenceSysFreeSequenceResource(void* obj);
extern void wazaSequenceSysFreeWazaResource(void* seqData, s32 moveID);
extern void wazaSequenceSysGetWazaTime(void* seqData);
extern s32 wazaSequenceSysGetModelShadowLight__Fv(void);
extern s32 wazaSequenceSysGetModelShadowCount__Fv(s32 moveID);
extern void* wazaSequenceSysGetModelShadowList__Fv(s32 moveID);
extern void* wazaSequenceSysResetAnimationExcept(s32 moveID, s32 entryType, s32 idx);
extern BOOL fn_801DB8FC(void* entry);
extern void wazaSequenceUpdate(void);
extern void wazaSequenceApplyStop(void* obj);
extern void fn_801DBC30(void* obj);
extern void wazaSequenceStart(s32 blendType);
extern void wazaSequenceFree(void* obj);
extern s32 fn_801DBFB0(void);
extern void wazaSequenceLoadData(s32 distortType, f32 intensity);
extern void wazaSequenceEntryLink(void);
extern void fn_801DC46C(s32 overlayType, u32 color);
extern void fn_801DC5F0(void);
extern void _wazaSequenceEffectEntryLoad(void);
extern void _wazaSequenceParticleEntryLoad(void);
extern void _wazaSequenceModelEntryLoad(s32 fieldEffect);
extern void* fn_801DCDA8(void* obj, s32 fieldEffect);
extern s32 fn_801DCDCC(void* obj);
extern void fn_801DCE0C(void* obj);
extern void fn_801DCEA8(void* obj);
extern void fn_801DCF00(u32 color, f32 intensity);
extern void fn_801DCF84(void* obj);
extern void fn_801DCFD8(void* obj);
extern void fn_801DD028(void* obj);
extern void fn_801DD078(void* obj);
extern void* GetWaza__12NullSequenceCFUsUs(void* obj, s32 search_key1, s32 search_key2);
extern void fn_801DD100(u8* p, u8* q);
extern void fn_801DD158(void* obj);
extern void fn_801DD23C(void* obj);
extern void fn_801DD3E4(void* obj);
extern void sequenceLoad(void);
extern void fn_801DD5E8(void);
extern void fn_801DDB4C(void);
extern void fn_801DDC10(void);
extern void fn_801DDD28(void);
extern void fn_801DDEE4(s32 slot, s32 flashType);
extern BOOL fn_801DE164(s32 slot);
extern void fn_801DE190(void);
extern void fn_801DE418(s32 attackerSlot, s32 targetSlot);
extern void fn_801DE598(void);
extern void sequenceAnimEndCallback(s32 arg0, s32 arg1);
extern void fn_801DE698(s32 slot, s32 statID, s32 direction);
extern void _eyeTexAnimEnded(void);
extern void fn_801DEE14(s32 slot, u32 status);
extern void fn_801DEF0C(void* obj, s32 arg1, s32 arg2);
extern void fn_801DF070(s32 weatherType);
extern void fn_801DF160(void);
extern void fn_801DF1D0(void* obj);
extern void fn_801DF33C(void* obj);
extern void fn_801DF3D4(void* obj);
extern void fn_801DF474(s32 slot, s32 abilityID);
extern void fn_801DF790(s32 slot, s32 itemID);
extern void fn_801DFC30(void);

#endif /* GAME_BATTLE_BATTLE_WAZA_TYPES_H */
