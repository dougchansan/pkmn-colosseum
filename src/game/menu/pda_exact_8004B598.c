/**
 * @file pda_exact_8004B598.c
 * @brief PDA selection-wheel sprite callback, 0x8004B598 - 0x8004B7EC.
 *
 * The body is exact (copied from pda_range_80037158.c with the bank's
 * declarations).
 */
#include "dolphin/types.h"

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
} PdaModelWindow;

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
} PdaSceneWork;

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

typedef struct PdaVec3 {
    f32 x;
    f32 y;
    f32 z;
} PdaVec3;

typedef struct PdaNameEntry {
    u8 used;
    u8 pad01;
    u16 text[8];
} PdaNameEntry;

typedef struct PdaKanaGroup {
    u32 words[15];
    s32 count;
} PdaKanaGroup;

typedef struct PdaSortPair {
    u16 index;
    u16 key;
} PdaSortPair;

typedef struct PdaCamBlock {
    f32 v[6];
} PdaCamBlock;

typedef struct PdaCryTrail {
    void* data;
    f32 speed;
    u32 voice;
    f32 field_0c;
    s32 offset;
    f32 field_14;
    s32 active;
    f32 alpha;
    f32 field_20;
} PdaCryTrail;

typedef struct PdaPairS16 {
    s16 x;
    s16 y;
} PdaPairS16;

typedef struct PdaSpan12 {
    s16 v[12];
} PdaSpan12;

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
extern PdaModelWindow lbl_803A6748;
extern u8 lbl_803A67FC[];
extern PdaSceneWork lbl_803A6818;
extern s32 fn_8003B85C(void* window, s32 enabled);
extern void fn_8003C2B8(PdaSprite* sprite, PdaEvent* event);
extern s32 fn_80041E48(void* work, s32 mode);
extern s32 fn_80042658(void* work, s32 mode);
extern void pdaReadPadInput(void* scene);
extern void GSscene_SetMode(s32 mode);
extern void menuButtonNormal(void* button);
extern void winSpriteSetDisp(void* sprite, s32 disp);
extern void fn_800FB680(s32 arg0, s32 arg1, s32 arg2, void* data);
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
extern u32 fn_800D5CB8();
extern u32 fn_800D61E4();
extern u32 gfxEnd();
extern u32 gfxBegin();
extern u32 gfxSetPrimitive();
extern u32 gfxSetVtxDesc();
extern u32 gfxLoadTexture();
extern u32 gfxClearFlags();
extern u32 gfxSetFlags();
extern u32 fn_800E202C();
extern u32 fn_800E209C();
extern u32 fn_800E24B0();
extern u32 fn_800E27B0();
extern u32 fn_800E2C04();
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
extern u32 fn_8017B1CC();
extern s32 fn_8017B2CC();
extern u32 fn_8017B3E4();
extern u32 fn_801EE0BC();
extern u16 fn_801EE248();
extern u32 fn_801EE328();
extern u16 fn_801EE614();
extern u32 fn_801EE750();
extern u8 fn_801EE8F4();
extern u8 fn_801EEAD0();
extern u8 fn_801EEC74();
extern u8 fn_801EED88(u16);
extern u16 fn_801EEFAC();
extern u32 fn_801FCC7C();
extern u32 fightTrainerDataBiosGetPtr();
extern u32 gamedataGetStatus();
extern void menuCloseSync(s32, s32);
extern u32 fn_80018F54();
extern u32 fn_800F915C();
extern void windowDrawSprite(s16 x, s16 y, PdaSprite* sprite, u16 id, s32 arg4);
extern u8 menuModelCheck(void* work, s32 index);
extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetDarkpokemonDataId(u32 work, s32 id);
extern void pokemonBiosSetDp(u32 work, s32 dp);
extern u32 GSmsgGetGSchar(u32 msg);
extern void msgctrlSetValue(s32 id, u32 value);
extern void* pokemonDataBiosGetPtr(u32 id);
extern void* pokemonDataBiosGetName(void* data);
extern void* fightTrainerPokemonDataBiosGetPtr(void* data);
extern void* fightTrainerPokemonDataBiosGetNickname(void* entry);
extern u8 fightTrainerPokemonDataBiosGetDarkPokemonFlag(void* entry);
extern u32 gamedataGetStatus(s32 a, s32 b);
extern u32 memoDataGetPokemonRndFromID(s32 a, u32 id);
extern u32 memoDataGetPokemonTrainerRndFromID(s32 a, u32 id);
extern void pokemonBiosSetCatchTrainerRnd(u32 work, u32 rnd);
extern u32 pokemonGetStatus(u32 a, u32 b, s32 id, s32 index);
extern u32 pokemonBiosGetPokemonDataId(u32 work);
extern u8 pokemonDataBiosGetColor(void* data, s32 index);
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
extern u8 lbl_80267180[];
extern f32 lbl_8047BC9C;
extern u8 fn_80047CC0(u8* work);
extern u8 fn_800478B4(void* work, void* sub);
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
extern void* fn_801DAC3C(void* h);
extern s32 fn_801DAC24(void* h);
extern void GSmodelGetPosition(void* model, void* out);
extern void GSmodelSetPosition(void* model, void* pos);
extern void GSmodelSetMatrix(void* model, void* mtx);
extern void fn_800E064C(f32* mtx);
extern void fn_800E03B4(f32* mtx, void* vec);
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
extern void memoGetScaleAngle(u32 id, f32* scale, f32* angle);
extern void GSmodelCenterNull(void* model);
extern void modelRemoveCenterNull(void* model);
extern s32 fn_800EE0E8(void* model);
extern void* GSmodelGetPart(void* model, s32 index);
extern void GSpartGetTransform(void* part, void* out, s32 a, s32 b);
extern void GSpartFree(void* part);
extern void fn_800E032C(f32* mtx, f32 angle);
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
extern void fn_800E0370(f32* mtx, f32 angle);
extern void fn_800E0560(f32* mtx, void* vec);
extern void GSvecTransform(void* dst, f32* mtx, void* src);
extern u16 pokemonDataBiosGetWeight(void* data);
extern u16 pokemonDataBiosGetHeight(void* data);
extern PdaOrbitPoint lbl_802E52C8[8];
extern f32 lbl_8047BCAC;
extern s32 fn_800FE6D0(s16 x, s16 y);
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
extern s8 fn_800F7994(s32 chan, s32 index);
extern s8 fn_800F7920(s32 chan, s32 index);
extern void GSvecAdd(void* dst, void* a, void* b);
extern u8 fn_8010A210(void* work, u32 pokemon);
extern void menuModelFree(void* work);
extern void fn_80109C88(void* work, u32 pokemon);
extern void GSmodelDestroyLinkedParticles(void* model);
extern f32 lbl_8047BCCC;
extern const f32 lbl_80267150[];
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
extern void fn_8018F4C8(void* info, s32 index, s32* motion, s32* out);
extern void menuModelSetMotion(void* work, s32 motion);
extern void fn_8010A010(void* work, u32 id);
extern s8 lbl_804788D4[];
extern void* menuDataBiosGetPtr(s32 id);
extern void* windowGetKeyInfo(void* window);
extern void menuPlaySe(s32 id, s32 se);
extern u32 lbl_8047A4EC;
extern f32 lbl_8047BD1C;
extern u16 pokemonDataBiosGetVoice(void* data);
extern void* pokemonNakigoeDataBiosGetDataAddress(void* data);
extern s32 GSsndGetStatus(u32 voice);
extern u32 fn_800F7AF0(s32 chan);
extern s32 menuOpen(s32 id, s32 flag);
extern f32 lbl_8047BDA0;
extern f32 lbl_8047BDA4;
extern f32 lbl_8047BDA8;
extern f32 lbl_8047BDB0;
extern f32 lbl_8047BDB4;
extern f32 lbl_8047BDB8;
extern f32 lbl_8047BDD8;
extern f32 lbl_8047BDDC;
extern f32 lbl_8047BDE0;
extern f32 lbl_8047BDE4;
extern u8 std081_flagCheckTrue(s32 id);
extern void menuItemBiosSetSelectFlag(s32 id, s32 flag);
extern PdaPairS16 lbl_8047BD78;
extern PdaPairS16 lbl_8047BD7C;
extern PdaPairS16 lbl_8047BD80;
extern PdaPairS16 lbl_8047BD84;
extern PdaPairS16 lbl_8047BD88;
extern PdaPairS16 lbl_8047BD8C;
extern PdaPairS16 lbl_8047BD90;
extern PdaPairS16 lbl_8047BD94;
extern PdaSpan12 lbl_8026719C;
extern PdaSpan12 lbl_802671B4;
extern PdaPairS16 lbl_8047BD70;
extern PdaPairS16 lbl_8047BD74;
extern PdaPairS16 lbl_8047BD98;
extern PdaPairS16 lbl_8047BD9C;
extern f32 lbl_8047BDC8;
extern f32 lbl_8047BDCC;
extern s32 lbl_804788F0;
extern u32 lbl_802E61D8[];
extern u32 heroGetStatus(s32 a, s32 b, s32 c);
extern void menuSpriteBiosGetPtr(s32 id);
extern u32 menuGetCursorItemID(s32 menu);
extern void fn_8004A7A8(void* work, PdaSprite* sprite);
extern f32 lbl_8047BDD0;
extern f32 lbl_8047BDD4;
extern f32 lbl_804788D8;
extern f32 lbl_8047BACC;
extern f32 lbl_8047BAD0;
extern f32 lbl_8047BAD4;
extern f32 lbl_8047BAD8;
extern void fn_801EEDEC(u16 id, s32 flag);
extern u8 lbl_803A6610[];
extern u32 pokemonCreateRndFit(void* work, s32 a, s32 b, s32 c, s32 d);
extern void winMsgClose(s32 id);
extern void fn_8010A420(void* work);
extern f32 lbl_8047BAF8;
extern f32 lbl_8047BAFC;
extern f32 lbl_8047BB00;
extern u8 lbl_80267060[];
extern u8 lbl_803A65B0[];
extern u16 pcboxGetNbItemSlot(s32 box);
extern u16 itemBiosGetItemDataId(void* item);
extern void* itemDataBiosGetPtr(u16 id);
extern void* itemDataBiosGetName(void* data);
extern u8 itemDataBiosGetKind(void* data);
extern u16 itemBiosGetNum(void* item);
extern f32 lbl_8047BA90;
extern f32 lbl_8047BA94;
extern f32 lbl_8047BA98;
extern f32 lbl_8047BAA8;

void fn_8004B598(s32 unused, PdaSprite* sprite, s32 messageId)
{
    u32 periods[3];
    u8 raw;
    u8 sel;
    s8 selection;
    f32* angles;
    s16* layout;
    f32 step;

    periods[0] = ((u32*)lbl_80267190)[0];
    periods[1] = ((u32*)lbl_80267190)[1];
    periods[2] = ((u32*)lbl_80267190)[2];
    switch (messageId) {
    case 0x6E7:
    case 0x75D:
    case 0x760:
    case 0x763:
        *(u32*)(lbl_803A6A60 + 0x44) = 0;
        break;
    case 0x6E8:
    case 0x75E:
    case 0x761:
    case 0x764:
        *(u32*)(lbl_803A6A60 + 0x44) = 1;
        break;
    case 0x6E9:
    case 0x75F:
    case 0x762:
    case 0x765:
        *(u32*)(lbl_803A6A60 + 0x44) = 2;
        break;
    }

    raw = lbl_803A6A60[0];
    if ((s8)raw < 0) {
        sel = lbl_803A6A60[0x48];
    } else {
        sel = raw;
        lbl_803A6A60[0x48] = raw;
    }
    selection = (s8)sel;
    switch (selection) {
    case 0:
        layout = (s16*)(lbl_802EF0A8 +
                        (*(u32*)(lbl_803A6A60 + 0x44) + 0x6E7) * 0x1C);
        break;
    case 1:
        layout = (s16*)(lbl_802EF0A8 +
                        (*(u32*)(lbl_803A6A60 + 0x44) + 0x75D) * 0x1C);
        break;
    case 2:
        layout = (s16*)(lbl_802EF0A8 +
                        (*(u32*)(lbl_803A6A60 + 0x44) + 0x760) * 0x1C);
        break;
    case 3:
        layout = (s16*)(lbl_802EF0A8 +
                        (*(u32*)(lbl_803A6A60 + 0x44) + 0x763) * 0x1C);
        break;
    default:
        layout = (s16*)(lbl_802EF0A8 + 0xC144);
        break;
    }

    angles = (f32*)(lbl_803A6A60 + 0xC);
    step = *(f32*)(lbl_803A6A60 + 8) *
           (lbl_8047BDF0 / *(f32*)&periods[*(u32*)(lbl_803A6A60 + 0x44)]);
    angles[*(u32*)(lbl_803A6A60 + 0x44)] += step;
    if (angles[*(u32*)(lbl_803A6A60 + 0x44)] > lbl_8047BDF0) {
        angles[*(u32*)(lbl_803A6A60 + 0x44)] -= lbl_8047BDF0;
    }
    if (angles[*(u32*)(lbl_803A6A60 + 0x44)] < lbl_8047BDAC) {
        angles[*(u32*)(lbl_803A6A60 + 0x44)] += lbl_8047BDF0;
    }
    sprite->value = angles[*(u32*)(lbl_803A6A60 + 0x44)];
    sprite->field_50 = layout[1];
    sprite->field_52 = layout[2];
}
