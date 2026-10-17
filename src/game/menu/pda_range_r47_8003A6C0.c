/**
 * PDA functions 0x8003A6C0 - 0x8003AFDC (fn_8003A6C0 .. fn_8003AEF0): the
 * digit display, the numeric spinner, menu setup and the event-group
 * visibility check. Carved from
 * pda_range_80037158.c, which keeps the same bodies for its other
 * candidate partitions.
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

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 fn_8003A6C0(PdaDrawWork* work, PdaSprite* sprite)
{
    extern const s32 lbl_80267130[3];
    extern s32 lbl_8047A4C8;
    extern void fn_800FB8C8(s32, s32, s16, s16, void*, s32);
    extern void msgctrlSetValue(s32 id, s32 value);
    s32 index;
    s32 i;
    s32 divisor;
    s32 ids[3];
    s32 digit;

    ids[0] = lbl_80267130[0];
    ids[1] = lbl_80267130[1];
    ids[2] = lbl_80267130[2];
    for (index = 0; index < 3; index++) {
        if (sprite->eventId == ids[index]) {
            break;
        }
    }
    if (index >= 3) {
        return 0;
    }

    divisor = 1;
    for (i = 0; i < index; i++) {
        divisor *= 10;
    }
    digit = lbl_8047A4C8 / divisor;
    digit %= 10;
    msgctrlSetValue(0x34, digit);
    fn_800FB8C8(0, 0, sprite->x, sprite->y, work->drawData, 0xC9);
    return 0;
}
#pragma peephole reset

#pragma peephole off
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
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 fn_8003A7F0(PdaNumberWork* work)
{
    extern PdaKeyInfo* windowGetKeyInfo(void);
    extern s32 lbl_8047A4C8;
    extern void soundStart(s32 id, s32 a, s32 b, s32 c);
    PdaKeyInfo* keyInfo;
    PdaNumberRange* range;
    s32 step;
    s32 i;
    s32 cur;
    s32 digit;
    s32 base;
    s32 limit;
    s32 value;

    range = work->range;
    keyInfo = windowGetKeyInfo();
    if ((keyInfo->buttons & 0xf) != 0) {
        step = 1;
        for (i = 0; i < work->digitIndex; i++) {
            step *= 10;
        }
        if ((keyInfo->buttons & 1) != 0 && range->max != range->min) {
            if (step < 10) {
                value = lbl_8047A4C8 + step;
                lbl_8047A4C8 = value;
                if (value > range->max) {
                    lbl_8047A4C8 = range->min;
                }
                soundStart(0x3c5, 0, 0xff, 0);
            } else {
                cur = lbl_8047A4C8;
                digit = (cur / step) % 10;
                base = cur - digit * step;
                for (limit = 9; limit >= 0; limit--) {
                    if (base + limit * step <= range->max) {
                        break;
                    }
                }
                digit++;
                if (digit > limit) {
                    digit = 0;
                }
                lbl_8047A4C8 = base + digit * step;
                if (lbl_8047A4C8 < range->min) {
                    lbl_8047A4C8 = range->min;
                } else {
                    soundStart(0x3c5, 0, 0xff, 0);
                }
            }
        }
        if ((keyInfo->buttons & 2) != 0 && range->max != range->min) {
            if (step < 10) {
                value = lbl_8047A4C8 - step;
                lbl_8047A4C8 = value;
                if (value < range->min) {
                    lbl_8047A4C8 = range->max;
                }
                soundStart(0x3c5, 0, 0xff, 0);
            } else {
                cur = lbl_8047A4C8;
                digit = (cur / step) % 10;
                base = cur - digit * step;
                for (limit = 9; limit >= 0; limit--) {
                    if (base + limit * step <= range->max) {
                        break;
                    }
                }
                digit--;
                if (digit < 0) {
                    digit = limit;
                }
                lbl_8047A4C8 = base + digit * step;
                if (lbl_8047A4C8 < range->min) {
                    lbl_8047A4C8 = range->min;
                } else {
                    soundStart(0x3c5, 0, 0xff, 0);
                }
            }
        }
        if ((keyInfo->buttons & 8) != 0) {
            if (--work->digitIndex < 0) {
                work->digitIndex = 0;
            }
        }
        if ((keyInfo->buttons & 4) != 0) {
            if (++work->digitIndex >= 3) {
                work->digitIndex = 2;
            }
        }
    }
    return 0;
}

s32 fn_8003AC50(PdaMenuState* state)
{

    switch (state->mode) {
    case 0:
        if (state->menuSet == 0) {
            winSeqSetMenu(0x26, 0xb4);
            state->menuSet = 1;
        }
        break;
    case 3:
        if (state->menuSet == 0) {
            winSeqSetMenu(0x26, 0xb8);
            state->menuSet = 1;
        }
        break;
    }
    return 0;
}

s32 fn_8003ACE8(s32 arg0, s32 arg1, s32 arg2)
{
    extern s32 lbl_8047A4C8;
    extern s32 windowGetActiveID(void);
    extern s32 menuOpenCustom(s32 menu, ...);
    extern void menuClose(s32 menu);
    extern void menuCloseSync(s32 menu, s32 sync);
    s32 args[2];
    s32 result;
    s32 value;

    lbl_8047A4C8 = arg0;
    args[0] = arg2;
    args[1] = arg1;
    result = menuOpenCustom(0x26, windowGetActiveID(), 0, 0, 1, 1, args);
    menuClose(0x26);
    menuCloseSync(0x26, 1);
    if (result == -1) {
        value = -1;
    } else {
        value = lbl_8047A4C8;
    }
    return value;
}

s32 fn_8003AD6C(PdaSelectionWork* work, PdaSprite* sprite)
{
    extern const s32 lbl_80267140[4];
    s32 groups[4];
    s32* q;
    s32* p;
    s32 group;
    s32 found;
    s32 i;
    u8 flag;
    s8 selectedIndex;

    groups[0] = lbl_80267140[0];
    groups[1] = lbl_80267140[1];
    groups[2] = lbl_80267140[2];
    groups[3] = lbl_80267140[3];
    p = groups;
    found = 0;
    for (group = 0; group < 2; p += 2, group++) {
        for (q = p, i = 0; i < 2 && !found; q++, i++) {
            if (sprite->eventId == *q) {
                found = 1;
            }
        }
        if (found) {
            break;
        }
    }
    if (!found) {
        return 0;
    }

    selectedIndex = work->selectedIndex;
    if (group == (s32)selectedIndex) {
        flag = 1;
    } else {
        flag = 0;
    }
    winSpriteSetDisp(sprite, flag);
    return 0;
}

s32 fn_8003AE84(void)
{
    extern s32 menuOpen(s32 menu, s32 mode);
    extern void menuClose(s32 menu);
    extern void menuCloseSync(s32 menu, s32 sync);
    s32 result;
    s32 value;

    result = menuOpen(0x27, 1);
    menuClose(0x27);
    menuCloseSync(0x27, 1);
    if (result == -1) {
        value = 0;
    } else if (result == 0) {
        value = 1;
    } else {
        value = 0;
    }
    return value;
}

void fn_8003AEF0(PdaSprite* sprite)
{
    extern PdaListEntry* lbl_8047A4D4;
    extern void fn_801EED88(u16 id);
    extern void* fn_801EE544(u16 id, u8* variant);
    extern s32 fn_801EEF40(u16 id);
    extern s32 fn_8011396C(s32 floor);
    extern void msgctrlSetValue(s32 id, s32 value);
    u16 id;
    void* message;
    u32 value;

    fn_801EED88(lbl_8047A4D4[(u16)*(u32*)&lbl_803A6748].battleId);
    id = lbl_8047A4D4[*(u32*)&lbl_803A6748].battleId;
    sprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
    message = fn_801EE544(id, (u8*)&lbl_803A6748 + 0x94);
    value = fn_8011396C(fn_801EEF40(id));
    if (value == 0) {
        msgctrlSetValue(0x31, 0x18d3);
    } else {
        msgctrlSetValue(0x31, value);
    }
    fn_800FB680(0, 0, (u32)sprite->alphaByte | -0x100LL, message);
}
#endif
