/**
 * PDA fn_8003B6D0 (.text 0x8003B6D0 - 0x8003B814): caption for the
 * highlighted battle. Carved from pda_range_80037158.c, which keeps the same
 * body for its other candidate partitions; here fn_801EE248 is declared with
 * the u32 return retail's callers imply (no 16-bit mask on its result).
 */
#include "dolphin/types.h"

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

#if !defined(PDA_RANGE_EXACT_80037174_ONLY)

#endif

/* Retail places this callback immediately after fn_80037174. */

#if !defined(PDA_RANGE_EXACT_80037174_ONLY)
#endif

#if !defined(PDA_RANGE_EXACT_80037174_ONLY)
/* Four copies of the same fade-in step, one per 0x18-byte record in
   lbl_803A654C. Two shapes here are load-bearing: 0.0f as a literal, because
   retail reloads the constant instead of reusing the compare's copy, and the
   embedded `velocity =`, which orders the record load ahead of the velocity
   load. */

/* |v|. Retail's shape -- `> 0` first -- is what emits the two-way
   branch rather than an inverted single one. */
static inline f32 pdaOrbitAbs(f32 v)
{
    if (v > 0.0f) {
        return v;
    }
    return -v;
}

static inline void pdaUpdateOrbitSprite(PdaSprite* sprite, f32 baseAngle,
                                        s32 updateTarget)
{
    extern f32 lbl_802E52A8[4];
    extern PdaKeyInfo* windowGetKeyInfo(void);
    extern f64 cos(f64 angle);
    extern f64 sin(f64 angle);
    f32 angle;
    f32 step;
    f32 distance;
    f32 magnitude;
    s8 direction;

    if (updateTarget) {
        lbl_8047A488 = lbl_802E52A8[lbl_8047A47C];
    }
    angle = lbl_8047A484 - baseAngle;
    if (angle < 0.0f) {
        angle += 6.28318548f;
    }
    if (angle >= 6.28318548f) {
        angle -= 6.28318548f;
    }

    windowGetKeyInfo();
    step = lbl_8047BA64 * lbl_8047A494;
    if (lbl_8047A484 != lbl_8047A488) {
        if (lbl_8047A488 - lbl_8047A484 < 0.0f) {
            step = -step;
        }
        lbl_8047A48C = step;
        lbl_8047A484 += step;
        if (lbl_8047A484 >= 6.28318548f) {
            lbl_8047A484 -= 6.28318548f;
        }
        if (lbl_8047A484 < 0.0f) {
            lbl_8047A484 += 6.28318548f;
        }

        distance = pdaOrbitAbs(lbl_8047A484 - lbl_8047A488);
        magnitude = pdaOrbitAbs(step);
        if (distance < magnitude) {
            lbl_8047A484 = lbl_8047A488;
            lbl_8047A48C = 0.0f;
        }
    } else if (lbl_8047A47D != lbl_8047A47C) {
        direction = lbl_8047A47D - lbl_8047A47C;
        if (direction > 0) {
            lbl_8047A48C = -step;
        } else if (direction < 0) {
            lbl_8047A48C = step;
        }
        lbl_8047A47D = lbl_8047A47C;
    }

    sprite->field_50 =
        (s16)(lbl_8047BA6C * (f32)sin(angle) + lbl_8047BA68);
    sprite->field_52 =
        (s16)(lbl_8047BA6C * (f32)cos(angle) + lbl_8047BA70);
}

/* The redundant expressions preserve MWCC's exact register/scheduling shape. */

static inline u8 pdaAllStopped(void)
{
    extern u8 lbl_803A654C[];
    f32* p = (f32*)lbl_803A654C;
    s32 i;

    for (i = 0; i < 4; i++, p += 6) {
        if (lbl_8047BA58 != p[4]) {
            return 0;
        }
    }
    return 1;
}

/* Re-seed the People screen's two 4-entry widget tables and reset the
   carousel angle back to the table's rest value. */
static inline void pdaResetPeopleTables(u8* base, u8* tbl)
{
    extern void* memcpy(void* dst, const void* src, u32 size);
    u8* dst;
    u8* src;
    s32 i;

    lbl_8047A480 = (void*)0x1b5a;
    src = tbl + 0;
    dst = base + 0xb4;
    for (i = 0; i < 4; dst += 0x18, src += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    src = tbl + 0x60;
    dst = base + 0x54;
    for (i = 0; i < 4; dst += 0x18, src += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    lbl_8047A47C = 0;
    lbl_8047A488 = *(f32*)(tbl + 0xe0);
    lbl_8047A484 = *(f32*)(tbl + 0xe0);
    lbl_8047A47D = 0;
}

/* People screen driver: run the top-level menu, dispatch the four entries,
   and tear the menu down on exit. */

/* Prime the save-overwrite prompt with the current play time and the two
   hero-name message slots. */
static inline void pdaFillSavePrompt(void)
{
    extern u32 fn_80005748(void);
    extern u32 fn_801EF214(void);
    extern u32 fn_801EF274(void);
    extern void msgctrlSetValue(s32 id, u32 value);
    u32 playTime;

    playTime = fn_80005748();
    if (lbl_8047A498 != 0) {
        playTime = lbl_8047A498;
    }
    lbl_8047A49C = fn_801EF214();
    lbl_8047A4A0 = fn_801EF274();
    msgctrlSetValue(0x4c, playTime);
    msgctrlSetValue(0x2f, fn_801EF274());
    msgctrlSetValue(0x30, fn_801EF214());
}

typedef struct PdaSaveImage {
    u8 bytes[0x1dfd0];
} PdaSaveImage;

/* Save-and-quit flow reached from the People screen's model preview. */

/* The data id / count of the target-th listed item in PC box 0. */
static inline s32 pdaBoxItemId(s32 target)
{
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern s32 itemBiosGetItemDataId(void* item);
    extern s32 itemBiosGetNum(void* item);
    void* item;
    s32 i;
    s32 found;
    s32 slots;

    slots = pcboxGetNbItemSlot(0);
    found = -1;
    for (i = 0; i < slots; i++) {
        item = pcboxGetItem(0, (s16)i);
        if ((u8)fn_801429E8(item) != 0) {
            found++;
            if (found >= target) {
                return itemBiosGetItemDataId(item);
            }
        }
    }
    return 0;
}

static inline s32 pdaBoxItemNum(s32 target)
{
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern s32 itemBiosGetItemDataId(void* item);
    extern s32 itemBiosGetNum(void* item);
    void* item;
    s32 i;
    s32 found;
    s32 slots;

    slots = pcboxGetNbItemSlot(0);
    found = -1;
    for (i = 0; i < slots; i++) {
        item = pcboxGetItem(0, (s16)i);
        if ((u8)fn_801429E8(item) != 0) {
            found++;
            if (found >= target) {
                return itemBiosGetNum(item);
            }
        }
    }
    return 0;
}

/* PC item list: cursor/page input, item swapping, and caption refresh. */

/* PC item transfer loop: run the box list and move the highlighted stack
   between the bag (mode 0) and the PC (mode 1) until the list is closed. */

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

/* Digit-wise spinner for the PDA's numeric entry field: left/right step the
   selected digit with wraparound inside the field's range, up/down move
   between digits. */


extern u8 lbl_802EF0A8[];
extern u8 lbl_80314F98[];
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
extern f32 lbl_8047BAC0;
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
extern u32 fn_801EE248();
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

static inline void pdaDrawModelTexture(PdaSprite* sprite, u8* model)
{
    u32 texture = menuModelRender(model);
    if (texture != 0) {
        gfxSetFlags(3);
        gfxClearFlags(4);
        gfxSetPrimitive(7);
        gfxSetVtxDesc(lbl_80314F98);
        gfxLoadTexture(0, texture);
        gfxBegin(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxTexCoord(0, lbl_8047BAC4, lbl_8047BAC4);
        fn_800D61E4(sprite->x, sprite->y);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxTexCoord(0, lbl_8047BAC8, lbl_8047BAC8);
        gfxEnd();
    }
}

/* Load the memo row at byte `offset` into the shared PDA work Pokemon. */
static inline u32 pdaLoadRowPokemon(s32 offset)
{
    extern u32 gamedataGetStatus(s32 a, s32 b);
    extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
    extern u32 memoDataGetPokemonRndFromID(s32 a, u32 id);
    extern u32 memoDataGetPokemonTrainerRndFromID(s32 a, u32 id);
    extern void pokemonBiosSetRnd(u32 work, u32 rnd);
    extern void pokemonBiosSetCatchTrainerRnd(u32 work, u32 rnd);
    u32 work = lbl_8047A4E0;
    u32 rnd;
    u32 species;
    u32 trainerRnd;

    if (work != 0) {
        species = *(u16*)((u8*)lbl_8047A4E4 + offset);
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

/* Species-name message for the memo row at byte `offset`, 0 when
   unavailable. */
static inline u32 pdaRowNameMsg(s32 offset)
{
    extern u32 pokemonBiosGetPokemonDataId(u32 work);
    extern void* pokemonDataBiosGetPtr(u32 id);
    extern void* pokemonDataBiosGetName(void* data);
    extern u32 GSmsgGetGSchar(u32 msg);
    u32 work = lbl_8047A4E0;

    if (work != 0) {
        work = pdaLoadRowPokemon(offset);
        if (work != 0) {
            return GSmsgGetGSchar((u32)pokemonDataBiosGetName(
                pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work))));
        }
        return 0;
    }
    return 0;
}

/* Draw the visible slice of the species-name column. */
static inline void pdaDrawNameRows(u8* context, s32 first, f32 y)
{
    extern void msgctrlSetValue(s32 id, u32 value);
    extern u32 GSmsgGetGSchar(u32 msg);
    s32 offset;
    s32 i;
    u32 name;

    for (i = first, offset = first; i < lbl_8047A4E8; i++) {
        if (i >= *(s32*)((u8*)&lbl_803A6818 + 8) - 1 &&
            i <= *(s32*)((u8*)&lbl_803A6818 + 0xC) + 1) {
            name = pdaRowNameMsg(offset);
            if (name == 0) {
                name = GSmsgGetGSchar(1);
            }
            msgctrlSetValue(0x37, name);
            fn_800FB680(0, (s32)y - 2, (u32)context[0x8B] | -0x100LL,
                        (void*)0xE7);
        }
        y += lbl_8047BCF4;
        offset += 2;
    }
}

/* Species-name column of the memo list. */

static inline void* pdaAlloc(s32 size, s32 align)
{
    u32 h = fn_800E2C04(size, align);

    if ((u16)h != 0) {
        return (void*)fn_800E27B0(h);
    }
    return NULL;
}

extern void* pokemonDataBiosGetPtr(u32 id);
extern void* pokemonDataBiosGetName(void* data);
extern u32 GSmsgGetGSchar(u32 msg);
extern void msgctrlSetValue(s32 id, u32 value);
extern void* fightTrainerPokemonDataBiosGetPtr(void* data);
extern void* fightTrainerPokemonDataBiosGetNickname(void* entry);
extern u8 fightTrainerPokemonDataBiosGetDarkPokemonFlag(void* entry);

/* Caption for the highlighted battle: the Pokemon's species name, or the
   matching dark Pokemon's nickname from the trainer's party. */
static inline u32 pdaBattleCaption(u16 id)
{
    u16 trainerId;
    void* entry;
    void* nick;
    u32 name;

    trainerId = fn_801EEFAC(id, 0);
    if (trainerId == 9) {
        entry = pokemonDataBiosGetPtr(fn_801EE248(id));
        if (entry != NULL) {
            name = GSmsgGetGSchar((u32)pokemonDataBiosGetName(entry));
        }
    } else {
        entry = fightTrainerPokemonDataBiosGetPtr(
            (void*)fn_801FCC3C(fightTrainerDataBiosGetPtr(trainerId)));
        name = GSmsgGetGSchar(
            (u32)fightTrainerPokemonDataBiosGetNickname(entry));
        for (;;) {
            if (id == fightTrainerPokemonDataBiosGetDarkPokemonFlag(entry)) {
                nick = fightTrainerPokemonDataBiosGetNickname(entry);
                if (nick != NULL) {
                    name = GSmsgGetGSchar((u32)nick);
                }
                break;
            }
            entry = (u8*)entry + 0x50;
        }
    }
    return name;
}

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
/* RULE-EXCEPTION(user-approved): local opt_lifetimes pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma opt_lifetimes off
void fn_8003B6D0(u8* ctx)
{
    u32 name;
    u32 msg;

    ctx[0x8b] = (u8)(lbl_8047BAC0 * *(f32*)((u8*)&lbl_803A6748 + 0x44));
    name = pdaBattleCaption(lbl_8047A4D4[*(u32*)&lbl_803A6748].battleId);
    msg = name;
    if (name == 0) {
        msg = GSmsgGetGSchar(1);
    }
    msgctrlSetValue(0x37, msg);
    fn_800FB680(0, 0, ctx[0x8b] | -0x100, (void*)0xcf);
}
#pragma pop
#pragma peephole reset
#endif
