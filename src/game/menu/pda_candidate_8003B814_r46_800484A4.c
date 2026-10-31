/* RULE-EXCEPTION(user-approved): constant_import (temporary) - remove when this file is merged back into one unit - see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_range_80037158.c
 * @brief PDA subsystem body, 0x80037158 - 0x8004B7EC.
 *
 * Colosseum's PDA: the People/party-select 3D screen (5 private
 * widget tables), a shared PDA scene/camera block, and the mail-fetch
 * helpers. A reduced/reordered subset of XD's PDA (ReliveHall and
 * PdaSearcher clusters were cut). Internal boundaries are fuzzy --
 * split further only with evidence. All functions asm-only until
 * matched.
 */
#include "dolphin/types.h"

extern void* memcpy(void* dst, const void* src, u32 len);
extern u16 pdaMemoLoadPokemon(void);

extern s8 lbl_8047A470;
extern void* lbl_8047A480;
extern s8 lbl_8047A490;
extern f32 lbl_8047A478;
extern f32 lbl_8047A484;
extern f32 lbl_8047A488;
extern f32 lbl_8047A48C;
extern f32 lbl_8047A494;
extern s8 lbl_8047A47C;
extern s8 lbl_8047A47D;
extern s32 lbl_8047A4A8;
extern s32 lbl_8047A4B8;
extern s32 lbl_8047A4B4;
extern s32 lbl_8047A4B0;
extern u32 lbl_8047A498;
extern u32 lbl_8047A49C;
extern u32 lbl_8047A4A0;
extern s32 lbl_8047A4AC;
extern s32 lbl_8047A4BC;
extern f32 lbl_8047A4C0;
extern u8 lbl_803A6498[];
extern u8 lbl_80314F98[];
extern void* pcboxGetItem(void*, s16);

extern f32 lbl_8047BA58;
extern f32 lbl_8047BA5C;
extern f32 lbl_8047BAB0;
extern f32 lbl_8047BABC;
extern f32 lbl_8047BA60;
extern f32 lbl_8047BA64;
extern f32 lbl_8047BA68;
extern f32 lbl_8047BA6C;
extern f32 lbl_8047BA70;
extern f32 lbl_8047BA74;
extern f32 lbl_8047BA78;
extern f32 lbl_8047BAC0;

typedef struct PdaModelWindow {
    u8 pad00[0x8];
    s32 field_08;
    s32 field_0C;
    s32 field_10;
    u8 pad14[0x4];
    f32 field_18;
    u8 pad1C[0xC];
    f32 field_28;
    u8 pad2C[0x18];
    f32 alphaScale;
    u8 pad48[0x4C];
    s8 variant;
    u8 pad95[0xB];
} PdaModelWindow; /* lbl_803A6748, size 0xA0 */

typedef struct PdaSprite {
    u8 pad00[0x4];
    s8 flags;
    u8 pad05;
    s16 eventId;
    u8 pad08[0x44];
    s32 messageId;
    s16 field_50;
    s16 field_52;
    s16 x;
    s16 y;
    u8 pad58[0xc];
    u8 colorR;
    u8 colorG;
    u8 colorB;
    u8 alpha;
    u8 pad68[0x8];
    f32 value;
    u8 pad74[0x17];
    u8 alphaByte;
    u8 pad8c[9];
    s8 selectedIndex;
} PdaSprite;

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0xC];
    s32 field_10;
    u8 pad14[0x14];
    s32 field_28;
    u8 pad2C[0x14];
    f32 angle;
    u8 pad44[8];
    f32 alphaScale;
    u8 pad50[0x1F8];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */

typedef struct PdaEvent {
    u8 pad00[0x6];
    s16 messageId;
} PdaEvent;

typedef struct PdaMenuState {
    u8 pad00;
    s8 mode;
    s8 menuSet;
} PdaMenuState;

typedef struct PdaSelectionWork {
    u8 pad00[4];
    void* menu;
    u8 pad08[0x8d];
    s8 selectedIndex;
} PdaSelectionWork;

typedef struct PdaKeyInfo {
    u8 pad00[4];
    u16 trigger;
    u16 buttons;
} PdaKeyInfo;

typedef struct PdaListEntry {
    u16 field_00;
    u16 battleId;
} PdaListEntry;

typedef struct PdaOrbitPoint {
    f32 angle;
    f32 pad04[6];
    f32 alpha;
    f32 pad20;
} PdaOrbitPoint;

typedef struct PdaDrawWork {
    u8 pad00[0x88];
    void* drawData;
    u8 pad8C[9];
    s8 selectedPage;
} PdaDrawWork;

extern PdaModelWindow lbl_803A6748;
extern u8 lbl_803A67FC[];
extern PdaSceneWork lbl_803A6818;

extern s32 pdaSnagListDetailKey(void* window, s32 enabled);
extern void pdaSnagListDrawDetail(PdaSprite* sprite, PdaEvent* event);
extern s32 pdaMemoUpdateScroll(void* work, s32 mode);
extern s32 pdaModelViewerControl(void* work, s32 mode);
extern void pdaReadPadInput(void* scene);
extern void GSscene_SetMode(s32 mode);
extern void menuButtonNormal(void* button);
extern void winSpriteSetDisp(void* sprite, s32 disp);
extern void GSmsgPrintColor(s32 arg0, s32 arg1, s32 arg2, void* data);

typedef struct PdaSaveImage {
    u8 bytes[0x1dfd0];
} PdaSaveImage;
typedef struct PdaNumberRange {
    s32 max;
    s32 min;
} PdaNumberRange;
typedef struct PdaNumberWork {
    u8 pad00[0x60];
    PdaNumberRange* range;
    u8 pad64[0x31];
    s8 digitIndex;
} PdaNumberWork;
extern u8 lbl_802EF0A8[];
extern u8 lbl_80267190[];
extern u8 lbl_803A6A60[];
extern PdaListEntry* lbl_8047A4D4;
extern u32 lbl_8047A4D0;
extern u32 lbl_8047A4D8;
extern u32 lbl_8047A4DC;
extern u32 lbl_8047A4E0;
extern u16* lbl_8047A4E4;
extern u16 lbl_8047A4E8;
extern s32 lbl_804788C0;
extern s16 lbl_804788C8[2];
extern s16 lbl_804788CC[2];
extern s16 lbl_804788D0[2];
extern f32 lbl_8047BC94;
extern f32 lbl_8047BC98;
extern f32 lbl_8047BCA0;
extern f32 lbl_8047BAC4;
extern f32 lbl_8047BAC8;
extern f32 lbl_8047BCB8;
extern f32 lbl_8047BCBC;
extern f32 lbl_8047BCC0;
extern f32 lbl_8047BCC4;
extern f32 lbl_8047BCC8;
extern const f32 lbl_8047BCF4;
extern f32 lbl_8047BCF8;
extern f32 lbl_8047BDAC;
extern f32 lbl_8047BDF0;
extern u32 gfxVtxTexCoord();
extern u32 gfxVtxColor4u8();
extern u32 gfxVtxPos2s16();
extern u32 gfxEnd();
extern u32 gfxBegin();
extern u32 gfxSetPrimitive();
extern u32 gfxSetVtxDesc();
extern u32 gfxLoadTexture();
extern u32 gfxClearFlags();
extern u32 gfxSetFlags();
extern u32 GSmemGetHandleByPtr();
extern u32 GSmemFree();
extern u32 fn_800E24B0();
extern u32 GSmemLock();
extern u32 GSmemAlloc();
extern u32 fn_800FA280();
extern void* menuDataBiosGetPtr(s32);
extern u32 fn_80102510();
extern u32 fn_8010264C();
extern u32 fn_801040F0();
extern void* windowGetKeyInfo(void*);
extern u32 menuModelRender();
extern u32 fn_80109B90();
extern u32 fn_8011D8D8();
extern u32 fn_8011D8F4();
extern u32 fn_8011DFE0();
extern u32 pokemonGetSex();
extern u32 fn_801240C4();
extern u32 fn_80132A38();
extern u32 soundStartRaw(u32, u32, u32);
extern u32 GSresReleaseById();
extern s32 GSresGroupGetState();
extern u32 fsysStartLoad();
extern u32 darkPokemonSaveGet4A4();
extern u16 darkPokemonSaveGet4A0();
extern u32 darkPokemonSaveGetName();
extern u16 darkPokemonGetFlag1C();
extern u32 darkPokemonSaveGet8();
extern u8 darkPokemonCheckFlag16();
extern u8 darkPokemonCheckFlag12();
extern u8 darkPokemonCheckFlag14();
extern u8 darkPokemonCheckFlag0C(u16);
extern u16 darkPokemonGetSpecies();
extern u32 fightTrainerDataBiosGetStatus03();
extern u32 fightTrainerDataBiosGetPtr();
extern u32 gamedataGetStatus();
extern void menuCloseSync(s32, s32);
extern u32 bagMenuOpen();
extern u32 GSresReleaseOwner();
extern void windowDrawSprite(s16 x, s16 y, PdaSprite* sprite, u16 id, s32 arg4);
extern u8 menuModelCheck(void* work, s32 index);
extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetDarkpokemonDataId(u32 work, s32 id);
extern void pokemonBiosSetDp(u32 work, s32 dp);
extern u32 GSmsgGetGSchar(u32 msg);
extern void msgctrlSetValue(s32 id, u32 value);
extern void* pokemonDataBiosGetPtr(u16 id);
extern void* pokemonDataBiosGetName(void* data);
extern void* fightTrainerPokemonDataBiosGetPtr(void* data);
extern void* fightTrainerPokemonDataBiosGetNickname(void* entry);
extern u8 fightTrainerPokemonDataBiosGetDarkPokemonFlag(void* entry);
extern u32 gamedataGetStatus(s32 a, s32 b);
extern u32 memoDataGetPokemonRndFromID(s32 a, u32 id);
extern u32 memoDataGetPokemonTrainerRndFromID(s32 a, u32 id);
extern void pokemonBiosSetCatchTrainerRnd(u32 work, u32 rnd);
extern u32 pokemonGetStatus(u32 a, u32 b, s32 id, s32 index);
extern u16 pokemonBiosGetPokemonDataId(u32 work);
extern u8 pokemonDataBiosGetColor(void* data, s32 index);
typedef struct PdaVec3 {
    f32 x;
    f32 y;
    f32 z;
} PdaVec3;
extern void GSvecCopy(void* dst, void* src);
extern void* GSmodelGetBound(void* model);
extern void ObjInfoInit(void* bound, void* out);
extern void GScameraGetPerspective(void* cam, f32* a, f32* b, f32* c, f32* d);
extern void GScameraSetPerspective(void* cam, f32 a, f32 b, f32 c, f32 d);
extern void GScameraSetPosition(void* cam, void* pos);
extern void GScameraSetRotation(void* cam, void* rot);
extern void GScameraLookAt(void* cam, void* a, void* b);
extern void set__5GSvecFfff(void* vec, f32 x, f32 y, f32 z);
extern void GSlightSetType(void* light, s32 type);
extern void GSlightSetColor(void* light, void* color);
extern void GSlightSetPosition(void* light, void* pos);
extern void GSlightSetTarget(void* light, void* target);
extern void GSlightSetActive(void* light, s32 active);
extern u8 lbl_802E543C[];
extern u8 lbl_802E5448[];
extern f32 lbl_8047BC9C;
extern u8 pdaModelFrameSingle(u8* work);
extern u8 pdaModelFrameCamera(void* work, void* sub);
typedef struct PdaNameEntry {
    u8 used;
    u8 pad01;
    u16 text[8];
} PdaNameEntry;
typedef struct PdaKanaGroup {
    u32 words[15];
    s32 count;
} PdaKanaGroup;
extern PdaKanaGroup* lbl_802E60B0[];
extern void GScharMakeFromSJIS(u16* dst, u32 sjis);
extern f32 lbl_8047BCFC;
extern f32 lbl_8047BD00;
extern f32 lbl_8047BD04;
extern f32 lbl_8047BD08;
extern u8 lbl_804788C4;
extern f32 lbl_8047BCA4;
extern f32 lbl_8047BCA8;
extern f32 lbl_8047BD10;
extern f32 lbl_8047BD14;
extern u32 GSmsgGetRect(s32 id);
extern f32 lbl_8047BD18;
extern f32 lbl_8047BD30;
extern f32 lbl_8047BD34;
extern u8 lbl_802E5424[];
extern u8 lbl_802E5430[];
extern void* wazaSequenceOwnerGetModel(void* h);
extern s32 wazaSequenceOwnerGetSizeClass(void* h);
extern void GSmodelGetPosition(void* model, void* out);
extern void GSmodelSetPosition(void* model, void* pos);
extern void GSmodelSetMatrix(void* model, void* mtx);
extern void GSmtxIdentity(f32* mtx);
extern void GSmtxTransApply(f32* mtx, void* vec);
extern f64 __frsqrte(f64 value);
extern f32 lbl_80478AC0[];
extern f32 lbl_8047BD38;
extern f32 lbl_8047BD3C;
extern f32 lbl_8047BD40;
extern f32 lbl_8047BD44;
extern f32 lbl_8047BD48;
extern f32 lbl_8047BD4C;
extern volatile f32 lbl_8047BD68;
extern const f64 lbl_8047BD50;
extern const f64 lbl_8047BD58;
extern u8 lbl_802E540C[];
extern u8 lbl_802E5418[];
extern void memoGetScaleAngle(u16 id, f32* scale, f32* angle);
extern void GSmodelCenterNull(void* model);
extern void modelRemoveCenterNull(void* model);
extern s32 GSmodelWalkJObjs(void* model);
extern void* GSmodelGetPart(void* model, s32 index);
extern void GSpartGetTransform(void* part, void* out, s32 a, s32 b);
extern void GSpartFree(void* part);
extern void GSmtxRotateY(f32* mtx, f32 angle);
extern f64 lbl_8047BD60;
extern f32 lbl_8047BD20;
extern f32 lbl_8047BD24;
extern f32 lbl_8047BD28;
extern f32 lbl_8047BD2C;
extern void* GSsplineCreate(s32 a, s32 b, s32 count);
extern void GSsplineAddControlVectorValue(void* spline, void* vec, f32 t);
extern void GSsplineFree(void* spline);
extern u8 lbl_802E53F4[];
extern u8 lbl_802E5400[];
extern void GSmtxMakeYRotation(f32* mtx, f32 angle);
extern void GSmtxRotateX(f32* mtx, f32 angle);
extern void GSmtxMakeTranslate(f32* mtx, void* vec);
extern void GSvecTransform(void* dst, f32* mtx, void* src);
typedef struct PdaSortPair {
    u16 index;
    u16 key;
} PdaSortPair;
extern u16 pokemonDataBiosGetWeight(void* data);
extern u16 pokemonDataBiosGetHeight(void* data);
extern PdaOrbitPoint lbl_802E52C8[8];
extern f32 lbl_8047BCAC;
extern s32 spriteSetOrigin(s16 x, s16 y);
extern void spriteSetEnv(s32 env);
extern u32 lbl_8047A4F0;
extern u16 lbl_803A67E8[];
extern u8 pokemonDataBiosGetTokuseiDataId(void* data, s32 slot);
extern void* pokemonTokuseiDataBiosGetPtr(u32 id);
extern void* pokemonTokuseiDataBiosGetName(void* data);
extern u32 pokemonDataBiosGetTypeName(void* data);
extern f32 lbl_8047BCD0;
extern f32 lbl_8047BCD4;
extern f32 lbl_8047BCE0;
extern f32 lbl_8047BCE4;
extern f32 lbl_8047BCE8;
extern f32 lbl_8047BCEC;
extern f32 lbl_8047BCF0;
extern s8 GSinputGetRightStickXData(s32 chan, s32 index);
extern s8 GSinputGetRightStickYData(s32 chan, s32 index);
extern void GSvecAdd(void* dst, void* a, void* b);
extern u8 menuPokemonModelIsLoaded(void* work, u32 pokemon);
extern void menuModelFree(void* work);
extern void menuPokemonModelRequest(void* work, u32 pokemon);
extern void GSmodelDestroyLinkedParticles(void* model);
typedef struct PdaCamBlock {
    PdaVec3 target;
    PdaVec3 pos;
} PdaCamBlock;

typedef struct PdaCamPresets {
    PdaCamBlock cams[2];
    PdaVec3 lightColor;
} PdaCamPresets;

extern const PdaCamPresets lbl_80267150;
extern f32 lbl_8047BCCC;
extern void menuOffScreenSetDisp(s32 disp);
extern void std114_mapObjectSetVisibility(void* h, s32 b);
extern void std113_mapObjectUnk_8018db68(void* h);
extern void* std112_modelGetResourceID(u32 id);
extern void* std093_getCurrentGroupID(void);
extern void* GSresGetResource(u32 group, u32 id);
extern void GSmodelSetVisibility(void* model, s32 vis);
extern void GSmodelSetAnimRate(void* model, f32 rate);
extern void GScameraGetPosition(void* cam, void* out);
extern void GScameraGetRotation(void* cam, void* out);
extern void cameraPlayAnime(u32 group, u32 id, s32 c, s32 d);
extern void menuModelInit(void* work, s32 w, s32 h);
extern void* peopleInfoBiosGetPtr(u32 id);
extern void peopleInfoGetMotion(void* info, s32 index, s32* motion, s32* out);
extern void menuModelSetMotion(void* work, s32 motion);
extern void menuPokemonModelRequestById(void* work, u32 id);

/* RULE-EXCEPTION(user-approved): single-use inline helper; the camera reset is a shared block of the original file (fn_80044630 repeats it), and inlining it numbers its locals after the outer function's, which gives retail's registers for the anim and camera handles - see docs/RULE_EXCEPTIONS.md */
static inline void pda_80048918_ResetCamera(const PdaCamPresets* presets)
{
    void* h;
    void* anim;
    void* cam;
    u8* saved;
    void* res;
    f32 persp0;
    f32 persp1;
    f32 persp2;
    f32 persp3;
    PdaVec3 target;
    PdaVec3 pos;
    PdaVec3 rot;

    h = *(void**)((u8*)&lbl_803A6818 + 0x10c);
    std114_mapObjectSetVisibility(h, 0);
    res = GSresGetResource((u32)std093_getCurrentGroupID(), (u32)h);
    if (res != NULL) {
        GSmodelSetVisibility(res, 0);
    }
    std113_mapObjectUnk_8018db68(h);
    *(f32*)((u8*)&lbl_803A6818 + 0x6c) = lbl_8047BC94;
    *(f32*)((u8*)&lbl_803A6818 + 0x70) = lbl_8047BC94;
    *(f32*)((u8*)&lbl_803A6818 + 0x74) = lbl_8047BC94;
    *(f32*)((u8*)&lbl_803A6818 + 0x64) = lbl_8047BCF0;
    anim = *(void**)((u8*)&lbl_803A6818 + 0x10c) = std112_modelGetResourceID(0xd171000);
    res = GSresGetResource((u32)std093_getCurrentGroupID(), (u32)anim);
    *(void**)((u8*)&lbl_803A6818 + 0x110) = res;
    GSmodelSetAnimRate(res, lbl_8047BD18);
    target = presets->cams[0].target;
    pos = presets->cams[0].pos;
    cam = GSresGetResource(0x17, 0xd731800);
    *(void**)((u8*)&lbl_803A6818 + 0x114) = cam;
    GScameraGetPosition(cam, &pos);
    GScameraGetRotation(cam, &rot);
    target.x = *(f32*)((u8*)cam + 0x100);
    target.y = *(f32*)((u8*)cam + 0x104);
    target.z = *(f32*)((u8*)cam + 0x108);
    GScameraGetPerspective(cam, &persp0, &persp1, &persp2, &persp3);
    saved = (u8*)&lbl_803A6818 + 0x218;
    GSvecCopy(saved, &pos);
    GSvecCopy(saved + 0xc, &target);
    *(f32*)(saved + 0x18) = persp0;
    *(f32*)(saved + 0x1c) = persp1;
    *(f32*)(saved + 0x20) = persp2;
    *(f32*)(saved + 0x24) = persp3;
    GScameraSetPerspective(cam, persp0, persp1, persp2, persp3);
    GSvecCopy((u8*)&lbl_803A6818 + 0x118, &pos);
    GSvecCopy((u8*)&lbl_803A6818 + 0x124, &rot);
    GSvecCopy((u8*)&lbl_803A6818 + 0x130, &target);
    *(f32*)((u8*)&lbl_803A6818 + 0x13c) = persp0;
    *(f32*)((u8*)&lbl_803A6818 + 0x140) = persp1;
    *(f32*)((u8*)&lbl_803A6818 + 0x144) = persp2;
    *(f32*)((u8*)&lbl_803A6818 + 0x148) = persp3;
    cameraPlayAnime(0x17, 0xd731800, 0, 1);
    GSscene_SetMode(4);
}

static inline void* pdaAlloc(s32 size, s32 align)
{
    u32 h = GSmemAlloc(size, align);

    if ((u16)h != 0) {
        return (void*)GSmemLock(h);
    }
    return NULL;
}

static inline u32 pdaLoadPokemon(s32 index)
{
    u32 work = lbl_8047A4E0;
    u32 rnd;
    u32 species;
    u32 trainerRnd;

    if (work != 0) {
        species = lbl_8047A4E4[index];
        if (species >= 0x8000) {
            species = species & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = memoDataGetPokemonRndFromID(0, species);
        trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

/* Point the rig's light from beside the camera at the model. */
/* RULE-EXCEPTION(user-approved): single-use inline helper; its locals are numbered and placed at the second inlining depth, which gives retail's stack layout for the light colour and position - see docs/RULE_EXCEPTIONS.md */
static inline void pda_80048918_SetLight(PdaVec3* pos, PdaVec3* target, const PdaCamPresets* presets)
{
    PdaVec3 color;
    PdaVec3 lightPos;

    color = presets->lightColor;
    memcpy(&lightPos, pos, 12);
    lightPos.x = lightPos.x - lbl_8047BC98;
    lightPos.y = lightPos.y + lbl_8047BC9C;
    GSlightSetType(*(void**)((u8*)&lbl_803A6818 + 0x108), 2);
    GSlightSetColor(*(void**)((u8*)&lbl_803A6818 + 0x108), &color);
    GSlightSetPosition(*(void**)((u8*)&lbl_803A6818 + 0x108), &lightPos);
    GSlightSetTarget(*(void**)((u8*)&lbl_803A6818 + 0x108), target);
    GSlightSetActive(*(void**)((u8*)&lbl_803A6818 + 0x108), 1);
}

static inline u8 fn_8003D1FC_setup(u8* sub, const PdaCamPresets* presets)
{
    void* model;
    void* cam;
    f32 persp0;
    f32 persp1;
    f32 persp2;
    f32 persp3;
    PdaVec3 lightTarget;
    PdaVec3 pos;
    PdaVec3 bound;
    f32 dist;

    if (sub == NULL) {
        return 0;
    }
    model = *(void**)((u8*)&lbl_803A6818 + 0xe8);
    if (model == NULL) {
        *sub = 0;
        return 0;
    }
    GSvecCopy(&pos, (u8*)&lbl_803A6818 + 0x1e8);
    GSvecCopy(lbl_802E5448, (u8*)&lbl_803A6818 + 0x1f4);
    persp0 = *(f32*)((u8*)&lbl_803A6818 + 0x200);
    persp1 = *(f32*)((u8*)&lbl_803A6818 + 0x204);
    persp2 = *(f32*)((u8*)&lbl_803A6818 + 0x208);
    persp3 = *(f32*)((u8*)&lbl_803A6818 + 0x20c);
    dist = *(f32*)((u8*)&lbl_803A6818 + 0x210);
    ObjInfoInit(GSmodelGetBound(model), &bound);
    GSscene_SetMode(3);
    GScameraGetPerspective(*(void**)((u8*)&lbl_803A6818 + 0xfc), &persp0, &persp1, &persp2,
                           &persp3);
    set__5GSvecFfff(&pos, lbl_8047BC94, lbl_8047BC94, dist);
    GScameraSetPosition(*(void**)((u8*)&lbl_803A6818 + 0xfc), &pos);
    GScameraSetPerspective(*(void**)((u8*)&lbl_803A6818 + 0xfc), persp0, persp1, persp2,
                           persp3);
    GScameraLookAt(*(void**)((u8*)&lbl_803A6818 + 0xfc), lbl_802E543C, lbl_802E5448);
    pda_80048918_SetLight(&pos, &lightTarget, presets);
    cam = *(void**)((u8*)&lbl_803A6818 + 0x114);
    if (cam != NULL) {
        GScameraSetPosition(cam, (u8*)&lbl_803A6818 + 0x118);
        GScameraSetRotation(cam, (u8*)&lbl_803A6818 + 0x124);
        GScameraSetPerspective(cam, *(f32*)((u8*)&lbl_803A6818 + 0x13c), *(f32*)((u8*)&lbl_803A6818 + 0x140),
                               *(f32*)((u8*)&lbl_803A6818 + 0x144), *(f32*)((u8*)&lbl_803A6818 + 0x148));
    }
    GSscene_SetMode(3);
    GSscene_SetMode(4);
    return 1;
}

/* RULE-EXCEPTION(user-approved): single-use inline helper; its scratch out-slot is placed below the other inline locals as in retail - see docs/RULE_EXCEPTIONS.md */
static inline void pda_80048918_GetMotion(u32 id, s32* motion)
{
    s32 out8;

    peopleInfoGetMotion(peopleInfoBiosGetPtr(id), 1, motion, &out8);
}

void pdaModelViewerSetup(void)
{
    extern u16 memoDataGetCount(s32 a);
    const PdaCamPresets* presets = &lbl_80267150;
    s32 motion;
    u16 total;
    u32 pokemon;

    menuOffScreenSetDisp(0);
    *(s32*)((u8*)&lbl_803A6818 + 0x110) = 0;
    *(s32*)((u8*)&lbl_803A6818 + 0x114) = 0;
    *(s32*)((u8*)&lbl_803A6818 + 0x10c) = 0;
    pda_80048918_ResetCamera(presets);
    *(s32*)((u8*)&lbl_803A6818 + 0x78) = 0;
    menuModelInit((u8*)&lbl_803A6818 + 0x7c, 0x280, 0x1e0);
    menuModelInit((u8*)&lbl_803A6818 + 0xc4, 0x280, 0x1e0);
    lbl_8047A4E0 = 0;
    lbl_8047A4E0 = (u32)pdaAlloc(0x140, 0x20);
    lbl_804788C0 = memoDataGetCount(0);
    lbl_8047A4E4 = (u16*)pdaAlloc(0x400, 0x20);
    *(f32*)((u8*)&lbl_803A6818 + 0x2c) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x30) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x34) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x38) = 0.0f;
    /* One zero for the five word resets; written separately, the front end's
     * per-statement copies move retail's basic-block break past the 0x15a store. */
    *(s32*)((u8*)&lbl_803A6818 + 0x20) = *(s32*)((u8*)&lbl_803A6818 + 0x4) =
        *(s32*)((u8*)&lbl_803A6818 + 0x14) = *(s32*)((u8*)&lbl_803A6818 + 0x28) =
        *(s32*)((u8*)&lbl_803A6818 + 0x0) = 0;
    *(f32*)((u8*)&lbl_803A6818 + 0x44) = 0.0f;
    *(s8*)((u8*)&lbl_803A6818 + 0x48) = 0;
    *(f32*)((u8*)&lbl_803A6818 + 0x64) = 1.5f;
    *(f32*)((u8*)&lbl_803A6818 + 0x68) = 1.5f;
    *(f32*)((u8*)&lbl_803A6818 + 0x6c) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x70) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x74) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x64) = 1.5f;
    *(s32*)((u8*)&lbl_803A6818 + 0x1c) = 0;
    *(f32*)((u8*)&lbl_803A6818 + 0x4c) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x50) = 1.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x58) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x54) = 1.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x5c) = 1.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x60) = 1.0f;
    lbl_804788C4 = 5;
    *(s8*)((u8*)&lbl_803A6818 + 0x158) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x159) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x15b) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x15a) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x15c) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x15d) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x15f) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x15e) = 0;
    *(s32*)((u8*)&lbl_803A6818 + 0x154) = 0;
    *(f32*)((u8*)&lbl_803A6818 + 0x1e0) = 300.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x1dc) = 300.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x1cc) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x1d0) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x1d4) = 0.0f;
    *(f32*)((u8*)&lbl_803A6818 + 0x1d8) = 0.0f;
    *(s8*)((u8*)&lbl_803A6818 + 0x164) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x178) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x18c) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x1a0) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x1b4) = 0;
    *(s32*)((u8*)&lbl_803A6818 + 0x1e4) = 0;
    pdaMemoLoadPokemon();
    if (lbl_804788C0 != 0) {
        pokemon = pdaLoadPokemon(lbl_803A6818.currentIndex);
        if (menuPokemonModelIsLoaded((u8*)&lbl_803A6818 + 0x7c, pokemon) == 0) {
            menuModelFree((u8*)&lbl_803A6818 + 0x7c);
            menuPokemonModelRequest((u8*)&lbl_803A6818 + 0x7c, pokemon);
        }
        *((u8*)&lbl_803A6818 + 0x214) = 0;
        switch (*(s32*)((u8*)&lbl_803A6818 + 0x1c)) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6:
        case 7:
        case 8:
        case 0xb:
        case 0xc:
            *((u8*)&lbl_803A6818 + 0x214) = pdaModelFrameSingle((u8*)&lbl_803A6818 + 0x7c);
            break;
        case 5:
            *((u8*)&lbl_803A6818 + 0x214) = pdaModelFrameCamera((u8*)&lbl_803A6818 + 0x7c, (u8*)&lbl_803A6818 + 0xc4);
            *((u8*)&lbl_803A6818 + 0x214) = fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4, presets);
            break;
        default:
            break;
        }
    }
    menuPokemonModelRequestById((u8*)&lbl_803A6818 + 0xc4, 0xf70400);
    switch (*(s32*)((u8*)&lbl_803A6818 + 0x1c)) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 6:
    case 7:
    case 8:
    case 0xb:
    case 0xc:
        *((u8*)&lbl_803A6818 + 0x214) = pdaModelFrameSingle((u8*)&lbl_803A6818 + 0x7c);
        break;
    case 5:
        *((u8*)&lbl_803A6818 + 0x214) = pdaModelFrameCamera((u8*)&lbl_803A6818 + 0x7c, (u8*)&lbl_803A6818 + 0xc4);
        *((u8*)&lbl_803A6818 + 0x214) = fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4, presets);
        break;
    default:
        break;
    }
    pda_80048918_GetMotion(0xf70400, &motion);
    menuModelSetMotion((u8*)&lbl_803A6818 + 0xc4, motion);
    total = lbl_8047A4E8;
    *(f32*)((u8*)&lbl_803A6818 + 0x40) = lbl_8047BC94;
    *(s32*)((u8*)&lbl_803A6818 + 0x8) = -5;
    if (total >= 5) {
        *(s32*)((u8*)&lbl_803A6818 + 0xc) = 5;
    } else {
        *(s32*)((u8*)&lbl_803A6818 + 0xc) = total;
    }
    *(s32*)((u8*)&lbl_803A6818 + 0x10) = total;
    *(s32*)((u8*)&lbl_803A6818 + 0x14c) = 0;
}
