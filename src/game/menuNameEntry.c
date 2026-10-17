/**
 * @file menuNameEntry.c
 * @brief Name-entry menu: player/Pokemon name input keyboard, back-panel
 *        model/ball preview, and the shop-adjacent draw helpers that share
 *        this address range.
 *
 * Split from the former game/gs_worldmap.c CodeCandidate bucket
 * (0x80026370-0x80030170); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit (0x80026370-0x80029850). This
 * range was originally mislabeled as world-map code; it is actually the
 * head of the XD-era menuNameEntry.cpp translation unit.
 */

#include "dolphin/types.h"
#include "game/menu/menu_name_entry.h"

/* ===== Phase 2 recovery stubs ===== */

typedef struct MenuNameEntryObject {
    u8 pad_00[0x8b];
    u8 alpha;
} MenuNameEntryObject;

typedef struct NameEntryRgb {
    u8 r;
    u8 g;
    u8 b;
} NameEntryRgb;

/* Name-entry row table: colour, X-button label and the four letter lists. */
typedef struct NameEntryModeEntry {
    NameEntryRgb color;
    u8 pad03;
    u32 xButtonMessage;
    u32 messages[4];
} NameEntryModeEntry;
extern NameEntryModeEntry lbl_80266E18[];

typedef struct NameEntryChoiceList {
    u8 pad00[8];
    s32* choices;
    s32 count;
} NameEntryChoiceList;

#if !(defined(MENU_NAME_ENTRY_80027740_ONLY) || defined(MENU_NAME_ENTRY_80027AA4_ONLY))
/* fn_80026370 - 0x80026370 | size: 0x20 */
s32 fn_80026370(void* r3, u8* r4) {
    r4[0x64] = 0;
    r4[0x65] = 0x35;
    r4[0x66] = 0x3c;
    return 0;
}


/* fn_80026390 - 0x80026390 | size: 0x20 */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (optimization_level 4) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
s32 fn_80026390(void* r3, u8* r4) {
    r4[0x64] = 0;
    r4[0x65] = 0x35;
    r4[0x66] = 0x3c;
    return 0;
}

/* fn_800263B0 - 0x800263B0 | size: 0x6c */
s32 fn_800263B0(void* r3, u8* r4) {
    void* ctx;
    s32 idx;
    NameEntryRgb c;
    ctx = *(void**)((u8*)(*(void**)((u8*)r3 + 0x60)) + 0x24);
    idx = *(s32*)ctx + 1;
    if (idx >= 2) idx -= 2;
    if (idx < 0 || idx >= 2) {
        c.r = 0xff; c.g = 0xff; c.b = 0xff;
    } else {
        c.r = lbl_80266E18[idx].color.r;
        c.g = lbl_80266E18[idx].color.g;
        c.b = lbl_80266E18[idx].color.b;
    }
    r4[0x64] = c.r;
    r4[0x65] = c.g;
    r4[0x66] = c.b;
    return 0;
}

/* fn_8002641C - 0x8002641C | size: 0x5c */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (optimization_level 4) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
s32 fn_8002641C(void* r3, u8* r4) {
    void* ctx;
    s32 idx;
    NameEntryRgb c;
    ctx = *(void**)((u8*)(*(void**)((u8*)r3 + 0x60)) + 0x24);
    idx = *(s32*)ctx;
    if (idx < 0 || idx >= 2) {
        c.r = 0xff; c.g = 0xff; c.b = 0xff;
    } else {
        c.r = lbl_80266E18[idx].color.r;
        c.g = lbl_80266E18[idx].color.g;
        c.b = lbl_80266E18[idx].color.b;
    }
    r4[0x64] = c.r;
    r4[0x65] = c.g;
    r4[0x66] = c.b;
    return 0;
}
#endif /* not a single-range build */

#if !defined(MENU_NAME_ENTRY_80026370_ONLY)

/* fn_80026478 - 0x80026478 | size: 0xa4 */
extern void* heroGetStatus(s32, s32, u32);
extern u8 pokemonCheckValid(void);
extern u8 pokemonGetSex(void*);
extern u8 lbl_80266DD8[];

#if !(defined(MENU_NAME_ENTRY_80027740_ONLY) || defined(MENU_NAME_ENTRY_80027AA4_ONLY))
s32 fn_80026478(void* r3, u8* r4) {
    void* ctx;
    void* r31;
    u8 r30;
    ctx = *(void**)((u8*)r3 + 0x60);
    r30 = 0;
    if (*(s32*)((u8*)ctx + 0x1c) != 2) {
        r4[0x67] = 0;
        return 0;
    }
    r31 = heroGetStatus(0, 3, (u16)*(u32*)((u8*)ctx + 0x20));
    if ((u8)pokemonCheckValid() == 0) goto L_done;
    if ((u32)(pokemonGetSex(r31) & 0xff) != 1) goto L_done;
    r30 = 0xff;
L_done:
    r4[0x67] = r30;
    return 0;
}

/* fn_8002651C - 0x8002651C | size: 0xa4 */
s32 fn_8002651C(void* r3, u8* r4) {
    void* ctx;
    void* r31;
    u8 r30;
    ctx = *(void**)((u8*)r3 + 0x60);
    r30 = 0;
    if (*(s32*)((u8*)ctx + 0x1c) != 2) {
        r4[0x67] = 0;
        return 0;
    }
    r31 = heroGetStatus(0, 3, (u16)*(u32*)((u8*)ctx + 0x20));
    if ((u8)pokemonCheckValid() == 0) goto L_done2;
    if ((u32)(pokemonGetSex(r31) & 0xff) != 0) goto L_done2;
    r30 = 0xff;
L_done2:
    r4[0x67] = r30;
    return 0;
}

/* fn_800265C0 - 0x800265C0 | size: 0x40 */
s32 fn_800265C0(void* r3, u8* r4) {
    void* ctx;
    u8* new_var;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    new_var = lbl_80266DD8;
    new_var = new_var + (*(s32*)((u8*)ctx + 0x1c) << 4);
    entry = (s32*)new_var;
    if (entry[1] != 7) r4[0x67] = 0;
    else r4[0x67] = 0xff;
    return 0;
}

/* fn_80026600 - 0x80026600 | size: 0x40 */
s32 fn_80026600(void* r3, u8* r4) {
    void* ctx;
    u8* new_var;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    new_var = lbl_80266DD8;
    new_var = new_var + (*(s32*)((u8*)ctx + 0x1c) << 4);
    entry = (s32*)new_var;
    if (entry[1] != 8) r4[0x67] = 0;
    else r4[0x67] = 0xff;
    return 0;
}

/* fn_80026640 - 0x80026640 | size: 0x40 */
s32 fn_80026640(void* r3, u8* r4) {
    void* ctx;
    u8* new_var;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    new_var = lbl_80266DD8;
    new_var = new_var + (*(s32*)((u8*)ctx + 0x1c) << 4);
    entry = (s32*)new_var;
    if (entry[1] == 0xa) goto L_40_then;
    r4[0x67] = 0;
    goto L_40_end;
L_40_then:
    r4[0x67] = 0xff;
L_40_end:
    return 0;
}

/* fn_80026680 - 0x80026680 | size: 0x40 */
s32 fn_80026680(void* r3, u8* r4) {
    void* ctx;
    u8* new_var;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    new_var = lbl_80266DD8;
    new_var = new_var + (*(s32*)((u8*)ctx + 0x1c) << 4);
    entry = (s32*)new_var;
    if (entry[1] == 7) goto L_80_then;
    r4[0x67] = 0;
    goto L_80_end;
L_80_then:
    r4[0x67] = 0xff;
L_80_end:
    return 0;
}

/* fn_800266C0 - 0x800266C0 | size: 0x40 */
s32 fn_800266C0(void* r3, u8* r4) {
    void* ctx;
    s32* entry;
    u8* base;
    ctx = *(void**)((u8*)r3 + 0x60);
    base = lbl_80266DD8;
    base = base + (*(s32*)((u8*)ctx + 0x1c) << 4);
    entry = (s32*)base;
    if (entry[1] == 8) goto L_C0_then;
    r4[0x67] = 0;
    goto L_C0_end;
L_C0_then:
    r4[0x67] = 0xff;
L_C0_end:
    return 0;
}

/* fn_80026700 - 0x80026700 | size: 0x40 */
s32 fn_80026700(void* r3, u8* r4) {
    void* ctx;
    u8* new_var;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    new_var = lbl_80266DD8;
    new_var = new_var + (*(s32*)((u8*)ctx + 0x1c) << 4);
    entry = (s32*)new_var;
    if (entry[1] == 0xa) goto L_700_then;
    r4[0x67] = 0;
    goto L_700_end;
L_700_then:
    r4[0x67] = 0xff;
L_700_end:
    return 0;
}

/* fn_80026740 - 0x80026740 | size: 0x90 | active C 65.8% (2026-06-29)
 *
 * The active C below is FAITHFUL, byte-correct logic and measures 65.8% after
 * activation. The inactive asm wrapper measured 96.67% under
 * objdiff (the 96.67 is a pure numeric-vs-named float-reloc disassembler
 * artifact: the .inc emits `lfs f2,-0x7d68(r2)` while the target object carries
 * `lbl_8047B938@sda21`; both resolve to the same sdata2 address).
 * Current active C still needs a permuter attack for byte matching.
 *
 * Levers that DID land (took the draft 72% -> 85.69%):
 *   - `#pragma fp_contract on`  -> `1.0 - 255.0*x` fuses to a single fnmsubs
 *   - declaring `state`/`index` BEFORE `ctx` reserves r5 for `state`, pinning
 *     ctx to r6 to match the target's whole-function allocation (+11%)
 *   - if(state!=7){0}else{...} single trailing `return 0` -> one epilogue li r3,0
 *   - dropping the (u8)/(s32) cast on the r4[0x67] store -> stb truncates with
 *     no redundant clrlwi (peephole off keeps the (s16) extsh on the X store)
 *   - `#pragma scheduling on` (off regresses to 71%)
 *
 * Residual WALL (3 CW reg-alloc / scheduler ties, not source-controllable):
 *   1. table base/index scratch rotation: target keeps base in r3 + idx in r0
 *      (`add r3,r3,r0`); CW emits base->r0 + idx->r3 (`add r3,r0,r3`).
 *   2. X multiply result: target reuses freed r5 (`mulli r5,r0,0x1a`); CW keeps
 *      it in r0 (`mulli r0,r0,0x1a`).
 *   3. float const-load schedule: target interleaves lfs f2/f0 into the X-mul
 *      load-delay slots; CW won't hoist them across the sth store.
 * -> permuter territory. Same wall applies to siblings fn_800267D0/fn_80026860.
 */
extern f32 lbl_8047B938;
extern f32 lbl_8047B934;
s32 fn_80026740(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];   /* state-machine entry table: each entry 16 bytes */
    s32  state;
    s32  index;
    s32  x;
    f32  one;
    f32  new_var;
    void* ctx;
    f32  scale;
    u8* entry;

    ctx = *(void**)((u8*)r3 + 0x60);
    entry = lbl_80266DD8;
    index = *(s32*)((u8*)ctx + 0x1c);
    entry += index << 4;
    state = *(s32*)(entry + 4);
    if (state != 7) {
        r4[0x67] = 0;
    } else {
        index = *(s32*)(*(s32**)((u8*)ctx + 0x34));
        if (index >= state) {
            index = state - 1;
        }
        scale = lbl_8047B938;
        one = lbl_8047B934;
        x = index * 0x1a;
        x += *(s32*)(*(s32**)((u8*)ctx + 0x48));
        *(s16*)(r4 + 0x50) = (s16)x;
        new_var = *(f32*)(*(f32**)((u8*)ctx + 0x30));
        new_var = one - (scale * new_var);
        r4[0x67] = new_var;
    }
    return 0;
}

/* fn_800267D0 - 0x800267D0 | size: 0x90 | WALL ~85.7% - sibling of fn_80026740.
 * selector=8, base ptr ctx+0x44. The inactive asm measured 96.67% (mostly
 * the numeric-vs-named float-reloc artifact). See fn_80026740 for the full lever
 * analysis + residual reg-alloc/scheduler ties. The C is active for honest
 * decomp progress. */
extern f32 lbl_8047B938;
extern f32 lbl_8047B934;
s32 fn_800267D0(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];
    s32  state;
    s32  index;
    s32  x;
    f32  one;
    f32  new_var;
    void* ctx;
    f32  scale;
    u8* entry;

    ctx = *(void**)((u8*)r3 + 0x60);
    entry = lbl_80266DD8;
    index = *(s32*)((u8*)ctx + 0x1c);
    entry += index << 4;
    state = *(s32*)(entry + 4);
    if (state != 8) {
        r4[0x67] = 0;
    } else {
        index = *(s32*)(*(s32**)((u8*)ctx + 0x34));
        if (index >= state) {
            index = state - 1;
        }
        scale = lbl_8047B938;
        one = lbl_8047B934;
        x = index * 0x1a;
        x += *(s32*)(*(s32**)((u8*)ctx + 0x44));
        *(s16*)(r4 + 0x50) = (s16)x;
        new_var = *(f32*)(*(f32**)((u8*)ctx + 0x30));
        new_var = one - (scale * new_var);
        r4[0x67] = new_var;
    }
    return 0;
}

/* fn_80026860 - 0x80026860 | size: 0x90 */
/* fn_80026860 - 0x80026860 | size: 0x90 | WALL ~85.7% - sibling of fn_80026740.
 * selector=0xa, base ptr ctx+0x40. The inactive asm measured 96.67% (mostly
 * the numeric-vs-named float-reloc artifact). See fn_80026740 for the full lever
 * analysis + residual reg-alloc/scheduler ties. The C is active for honest
 * decomp progress. */
extern f32 lbl_8047B938;
extern f32 lbl_8047B934;
s32 fn_80026860(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];
    s32  state;
    s32  index;
    s32  x;
    f32  one;
    f32  new_var;
    void* ctx;
    f32  scale;
    u8* entry;

    ctx = *(void**)((u8*)r3 + 0x60);
    entry = lbl_80266DD8;
    index = *(s32*)((u8*)ctx + 0x1c);
    entry += index << 4;
    state = *(s32*)(entry + 4);
    if (state != 0xa) {
        r4[0x67] = 0;
    } else {
        index = *(s32*)(*(s32**)((u8*)ctx + 0x34));
        if (index >= state) {
            index = state - 1;
        }
        scale = lbl_8047B938;
        one = lbl_8047B934;
        x = index * 0x1a;
        x += *(s32*)(*(s32**)((u8*)ctx + 0x40));
        *(s16*)(r4 + 0x50) = (s16)x;
        new_var = *(f32*)(*(f32**)((u8*)ctx + 0x30));
        new_var = one - (scale * new_var);
        r4[0x67] = new_var;
    }
    return 0;
}
#endif /* not a single-range build */

/* fn_800268F0 - 0x800268F0 | size: 0x254 */
extern void msgctrlSetValue(s32, void*);
extern u32 GSmsgGetRect(u32);
extern void GSmsgPrintColor(s32, s32, s32, u32);
extern s32 GSmsgGetLength(void*);
extern void* GSmsgGetGSchar(u32);
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;

/* Name-entry session block; the menu windows reach it through +0x60. */
typedef struct NAME_ENTRY_ARG {
    u16 buffer[12];
    u16* name;
    s32 kind;
    s32 index;
    s32* row;
    s32* letter;
    s32* column;
    f32* fade;
    s32* state;
    u32* work0;
    u32* work1;
    u32* work2;
    u32* work3;
    u32* work4;
} NAME_ENTRY_ARG;

/* Removes the last letter of the name; FALSE when the name is already empty. */
static inline u8 menuNameEntryDeleteLetter(NAME_ENTRY_ARG* arg)
{
    s32 pos;
    u8 deleted;

    pos = *arg->state;
    if (pos <= 0) {
        deleted = 0;
    } else {
        u16* name = arg->name;
        pos--;
        name += pos;
        *name = 0;
        deleted = 1;
        *arg->state = pos;
    }
    return deleted;
}

/* Letter `index` of the message list for (row, column), or 0 when out of range. */
static inline u16 menuNameEntryGetLetter(s32 row, s32 index, s32 column)
{
    u32 message;
    s32 length;

    if (row < 0 || row >= 2) {
        return 0;
    }
    if (column < 0 || column >= 4) {
        return 0;
    }
    message = lbl_80266E18[row].messages[column];
    length = GSmsgGetLength((void*)message);
    if (index < 0 || index >= length) {
        return 0;
    }
    return ((u16*)GSmsgGetGSchar(message))[index];
}

/* 6 when the letter differs from the current one, 0 otherwise. */
static inline s32 menuNameEntryGetLetterKind(u16 letter)
{
    u16* current = (u16*)GSmsgGetGSchar(0x2efc);
    if (letter == *current) {
        return 0;
    }
    return 6;
}
#if !(defined(MENU_NAME_ENTRY_80027740_ONLY) || defined(MENU_NAME_ENTRY_80027AA4_ONLY))

/* Draws the entered name and, while there is room, the blinking candidate letter,
 * when the session's name kind uses layout `type`. */
static inline void menuNameEntryDrawName(u8* self, u8* draw, s32 type)
{
    s32* types;
    u16* bufp;
    s32 x;
    u8* ctx;
    s32 count;
    s32 color;
    s32 width;
    s32 row;
    s32 column;
    u16 letter;
    u8 alpha;
    u16 next[2];
    u16 buf[2];
    u16* letters;
    s32 index;

    ctx = *(u8**)(self + 0x60);
    types = (s32*)(lbl_80266DD8 + 4);
    if (types[*(s32*)(ctx + 0x1c) * 4] != type) {
        draw[0x67] = 0;
    } else {
        letters = *(u16**)(ctx + 0x18);
        bufp = buf;
        count = 0;
        x = 0;
        while (*letters != 0) {
            color = self[0x8b] | -0x100;
            bufp[0] = *letters;
            bufp[1] = 0;
            msgctrlSetValue(0x37, bufp);
            width = 0x1b - (s16)(GSmsgGetRect(0xce) >> 16);
            GSmsgPrintColor(x + width / 2, 0, color, 0xce);
            x += 0x1a;
            count++;
            letters++;
        }
        if (count < types[*(s32*)(ctx + 0x1c) * 4] && self[0x98] == 0) {
            row = **(s32**)(ctx + 0x24);
            index = **(s32**)(ctx + 0x28);
            column = **(s32**)(ctx + 0x2c);
            letter = menuNameEntryGetLetter(row, index, column);
            if (letter != 0 && menuNameEntryGetLetterKind(letter) == 6) {
                alpha = (lbl_8047B934 - **(f32**)(ctx + 0x30)) * lbl_8047B938;
                color = alpha | 0xff0000;
                next[0] = letter;
                next[1] = 0;
                msgctrlSetValue(0x37, next);
                GSmsgPrintColor(count * 0x1a + (0x1b - (s16)(GSmsgGetRect(0xce) >> 16)) / 2, 0, color, 0xce);
            }
        }
        draw[0x67] = 0xff;
    }
}

s32 fn_800268F0(void* window, u8* draw)
{
    menuNameEntryDrawName(window, draw, 7);
    return 0;
}

/* fn_80026B44 - 0x80026B44 | size: 0x254 */
s32 fn_80026B44(void* window, u8* draw)
{
    menuNameEntryDrawName(window, draw, 8);
    return 0;
}

/* fn_80026D98 - 0x80026D98 | size: 0x254 */
s32 fn_80026D98(void* window, u8* draw)
{
    menuNameEntryDrawName(window, draw, 0xa);
    return 0;
}

/* Character-set ids the cursor callbacks look for (0x040A..0x040C). */
typedef struct NameEntryCharSetIds {
    u16 id[4];
} NameEntryCharSetIds;
extern const NameEntryCharSetIds lbl_8047B928;
extern u8 lbl_802EF0A8[];
extern f64 lbl_8047B948;
extern f32 lbl_8047B93C;
extern f32 lbl_8047B940;
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;

/* fn_80026FEC - 0x80026FEC | size: 0x190 */
s32 fn_80026FEC(void* window, u8* draw)
{
    u8* ctx;
    s16* rect;
    f32 width;
    f32 dw;
    f32 height;
    f32 dh;
    f32 scale;
    s16 left;
    s16 top;
    s16 right;
    s16 bottom;
    u8 alpha;
    NameEntryCharSetIds ids;
    s32 page;
    s32 set;
    u16 id;

    ctx = *(u8**)((u8*)window + 0x60);
    ids = lbl_8047B928;
    page = **(s32**)(ctx + 0x28);
    set = **(s32**)(ctx + 0x2c);
    if (page < 0xf) {
        id = 0xffff;
    } else if (set < 0 || set >= 4) {
        id = 0xffff;
    } else {
        id = ids.id[set];
    }
    if (id == *(s16*)(draw + 6)) {
        rect = (s16*)(lbl_802EF0A8 + *(s16*)(draw + 6) * 0x1c);
        scale = **(f32**)(ctx + 0x30);
        width = rect[3];
        height = rect[4];
        dw = lbl_8047B93C * (width * scale);
        dh = lbl_8047B93C * (height * scale);
        alpha = lbl_8047B938 * (lbl_8047B934 - scale);
        left = rect[1] - lbl_8047B940 * dw;
        top = rect[2] - lbl_8047B940 * dh;
        right = width + dw;
        bottom = height + dh;
        *(s16*)(draw + 0x50) = left;
        *(s16*)(draw + 0x52) = top;
        *(s16*)(draw + 0x54) = right;
        *(s16*)(draw + 0x56) = bottom;
    } else {
        alpha = 0;
    }
    draw[0x67] = alpha;
    return 0;
}

/* fn_8002717C - 0x8002717C | size: 0x190 */
s32 fn_8002717C(void* window, u8* draw)
{
    u8* ctx;
    s16* rect;
    f32 width;
    f32 dw;
    f32 height;
    f32 dh;
    f32 scale;
    s16 left;
    s16 top;
    s16 right;
    s16 bottom;
    u8 alpha;
    NameEntryCharSetIds ids;
    s32 page;
    s32 set;
    u16 id;

    ctx = *(u8**)((u8*)window + 0x60);
    ids = lbl_8047B928;
    page = **(s32**)(ctx + 0x28);
    set = **(s32**)(ctx + 0x2c);
    if (page < 0xf) {
        id = 0xffff;
    } else if (set < 0 || set >= 4) {
        id = 0xffff;
    } else {
        id = ids.id[set];
    }
    if (id == *(s16*)(draw + 6)) {
        rect = (s16*)(lbl_802EF0A8 + *(s16*)(draw + 6) * 0x1c);
        scale = **(f32**)(ctx + 0x30);
        width = rect[3];
        height = rect[4];
        dw = lbl_8047B93C * (width * scale);
        dh = lbl_8047B93C * (height * scale);
        alpha = lbl_8047B938 * (lbl_8047B934 - scale);
        left = rect[1] - lbl_8047B940 * dw;
        top = rect[2] - lbl_8047B940 * dh;
        right = width + dw;
        bottom = height + dh;
        *(s16*)(draw + 0x50) = left;
        *(s16*)(draw + 0x52) = top;
        *(s16*)(draw + 0x54) = right;
        *(s16*)(draw + 0x56) = bottom;
    } else {
        alpha = 0;
    }
    draw[0x67] = alpha;
    return 0;
}

/* fn_8002730C - 0x8002730C | size: 0x190 */
s32 fn_8002730C(void* window, u8* draw)
{
    u8* ctx;
    s16* rect;
    f32 width;
    f32 dw;
    f32 height;
    f32 dh;
    f32 scale;
    s16 left;
    s16 top;
    s16 right;
    s16 bottom;
    u8 alpha;
    NameEntryCharSetIds ids;
    s32 page;
    s32 set;
    u16 id;

    ctx = *(u8**)((u8*)window + 0x60);
    ids = lbl_8047B928;
    page = **(s32**)(ctx + 0x28);
    set = **(s32**)(ctx + 0x2c);
    if (page < 0xf) {
        id = 0xffff;
    } else if (set < 0 || set >= 4) {
        id = 0xffff;
    } else {
        id = ids.id[set];
    }
    if (id == *(s16*)(draw + 6)) {
        rect = (s16*)(lbl_802EF0A8 + *(s16*)(draw + 6) * 0x1c);
        scale = **(f32**)(ctx + 0x30);
        width = rect[3];
        height = rect[4];
        dw = lbl_8047B93C * (width * scale);
        dh = lbl_8047B93C * (height * scale);
        alpha = lbl_8047B938 * (lbl_8047B934 - scale);
        left = rect[1] - lbl_8047B940 * dw;
        top = rect[2] - lbl_8047B940 * dh;
        right = width + dw;
        bottom = height + dh;
        *(s16*)(draw + 0x50) = left;
        *(s16*)(draw + 0x52) = top;
        *(s16*)(draw + 0x54) = right;
        *(s16*)(draw + 0x56) = bottom;
    } else {
        alpha = 0;
    }
    draw[0x67] = alpha;
    return 0;
}

/* menuNameEntryDraw50Cursor - 0x8002749C | size: 0x158 */
extern f64 lbl_8047B948;
extern f32 lbl_8047B93C;
extern f32 lbl_8047B934;
extern f32 lbl_8047B940;
extern f32 lbl_8047B938;
s32 menuNameEntryDraw50Cursor(void* window, u8* draw)
{
    u8* ctx;
    s16* rect;
    f32 scale;
    f32 width;
    f32 height;
    f32 dh;
    s32 column;
    s32 row;
    s16 left;
    f32 dw;
    s16 top;
    s16 right;
    s16 bottom;
    u8 alpha;

    ctx = *(u8**)((u8*)window + 0x60);
    rect = (s16*)(lbl_802EF0A8 + *(s16*)(draw + 6) * 0x1c);
    column = **(s32**)(ctx + 0x28);
    scale = **(f32**)(ctx + 0x30);
    width = rect[3];
    height = rect[4];
    row = **(s32**)(ctx + 0x2c);
    dw = lbl_8047B93C * (width * scale);
    dh = lbl_8047B93C * (height * scale);
    if (column < 0xf) {
        left = (f32)(**(s32**)(ctx + 0x38) + column * 0x1b) - dw * lbl_8047B940;
        top = (f32)(**(s32**)(ctx + 0x3c) + row * 0x23) - dh * lbl_8047B940;
        right = width + dw;
        bottom = height + dh;
        alpha = lbl_8047B938 * (lbl_8047B934 - scale);
        *(s16*)(draw + 0x50) = left;
        *(s16*)(draw + 0x52) = top;
        *(s16*)(draw + 0x54) = right;
        *(s16*)(draw + 0x56) = bottom;
    } else {
        alpha = 0;
    }
    draw[0x67] = alpha;
    return 0;
}

/* menuNameEntryDraw50Text - 0x800275F4 | size: 0x14c */
/* Draws the 50-sound grid: four columns of letters for the current row. */
s32 menuNameEntryDraw50Text(void* window) {
    u8* self;
    u8* ctx;
    u16* bufp;
    s32 index;
    s32 column;
    s32 row;
    u16 buf[2];
    s32 color;
    u16 letter;

    self = window;
    ctx = *(u8**)(self + 0x60);
    row = **(s32**)(ctx + 0x24);
    bufp = buf;
    for (column = 0; column < 4; column++) {
        index = 0;
        while ((letter = menuNameEntryGetLetter(row, index, column)) != 0) {
            color = self[0x8b] | -0x100;
            bufp[0] = letter;
            bufp[1] = 0;
            msgctrlSetValue(0x37, bufp);
            GSmsgPrintColor(index * 0x1b + (0x1b - (s16)(GSmsgGetRect(0xce) >> 16)) / 2, column * 0x23, color, 0xce);
            index++;
        }
    }
    return 0;
}
#endif /* not a single-range build */

#if !defined(MENU_NAME_ENTRY_80027AA4_ONLY)

/* fn_80027740 - 0x80027740 | size: 0x3c */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off; optimization_level 1) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma optimization_level 1
s32 fn_80027740(void* r3) {
    u32 alpha = ((MenuNameEntryObject*)r3)->alpha;
    GSmsgPrintColor(0, 0, alpha | 0x509100, 0x2ef5);
    return 0;
}
#pragma pop

/* fn_8002777C - 0x8002777C | size: 0x3c */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off; optimization_level 1) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma optimization_level 1
s32 fn_8002777C(void* r3) {
    s32 mask = -0x100;
    u32 alpha = ((MenuNameEntryObject*)r3)->alpha;
    GSmsgPrintColor(0, 0, alpha | mask, 0x2ef3);
    return 0;
}
#pragma pop

/* fn_800277B8 - 0x800277B8 | size: 0x3c */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off; optimization_level 1) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma optimization_level 1
s32 fn_800277B8(void* r3) {
    s32 mask = -0x100;
    u32 alpha = ((MenuNameEntryObject*)r3)->alpha;
    GSmsgPrintColor(0, 0, alpha | mask, 0x2ef4);
    return 0;
}
#pragma pop

/* menuNameEntryDrawXButtonText - 0x800277F4 | size: 0xb0 */
/* RULE-EXCEPTION(user-approved): local peephole/optimization-level pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma optimization_level 2
s32 menuNameEntryDrawXButtonText(void* r3, u8* r4) {
    u8* r29;
    u8* r30;
    u32* r31;
    s32 index;
    u16 width;
    s32 x;

    r29 = r3;
    r30 = r4;
    index = *(s32*)(*(u8**)(*(u8**)(r29 + 0x60) + 0x24));
    index++;
    if (index >= 2) {
        index -= 2;
    }
    if (index >= 0 && index < 2) {
        r31 = (u32*)lbl_80266E18;
        r31 = (u32*)((NameEntryModeEntry*)r31 + index);
        r31 = (u32*)((u8*)r31 + 4);
        width = (u16)GSmsgGetRect(*r31);
        x = (s16)width;
        x = *(s16*)(r30 + 0x56) - x;
        GSmsgPrintColor(0, x, r29[0x8b] | -0x100, *r31);
    }
    return 0;
}
#pragma pop

/* menuNameEntryDrawTitle - 0x800278A4 | size: 0xbc */
extern s32 pokemonBiosGetPokemonDataId(void*);
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off; optimization_level 4) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 menuNameEntryDrawTitle(void* r3) {
    u8* r29;
    void* r31;
    void* r30;
    s32 r4;
    r29 = (u8*)r3;
    r31 = *(void**)(r29 + 0x60);
    if (*(s32*)r31 == 2) {
        r30 = heroGetStatus(0, 3, (u16)*(u32*)((u8*)r31 + 0x4));
        if (pokemonCheckValid() != 0) {
            r4 = pokemonBiosGetPokemonDataId(r30);
        } else {
            r4 = 1;
        }
        msgctrlSetValue(0x4e, (void*)(u32)(u16)r4);
    }
    GSmsgPrintColor(0, 0, r29[0x8b] | -0x100, *(u32*)(lbl_80266DD8 + (*(s32*)r31 << 4)));
    return 0;
}
#pragma pop

/* exchangeDakuon__FUs11DAKUON_MODE - 0x80027960 | size: 0x144 */
extern const u32 lbl_8047B920[2];
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma optimization_level 4
u16 exchangeDakuon__FUs11DAKUON_MODE(u16 letter, s32 mode) {
    u32* tables;
    u16* chars;
    s32 kind;
    s32 length;
    s32 i;
    u32 message;

    kind = 0;
    do {
        tables = (u32*)lbl_8047B920;
        message = tables[kind];
        if (message == 0) {
            continue;
        }
        length = GSmsgGetLength((void*)message);
        chars = (u16*)GSmsgGetGSchar(tables[kind]) + 1;
        for (i = 1; i < length; i += 2, chars += 2) {
            if (*chars == letter) {
                break;
            }
        }
        if (i < length) {
            break;
        }
    } while (++kind < 2);
    if (kind >= 2) {
        kind = 0;
    }
    if (kind == mode) {
        return letter;
    }
    /* The repeated test reproduces retail's beq/bne pair on one compare. */
    /* RULE-EXCEPTION(user-approved): repeated no-op test gives retail's beq/bne pair on one compare — see docs/RULE_EXCEPTIONS.md */
    if (kind == 1 || kind == 1) {
        letter = chars[-1];
    }
    if (mode == 0) {
        return letter;
    }
    tables = (u32*)lbl_8047B920;
    length = GSmsgGetLength((void*)tables[mode]);
    chars = (u16*)GSmsgGetGSchar(tables[mode]);
    for (i = 0; i < length; i += 2, chars += 2) {
        if (*chars == letter) {
            break;
        }
    }
    if (i >= length) {
        return 0;
    }
    return chars[1];
}
#pragma pop
#endif /* !MENU_NAME_ENTRY_80027AA4_ONLY */

#if !defined(MENU_NAME_ENTRY_80027740_ONLY)

/* selectLetter__FP14NAME_ENTRY_ARG - 0x80027AA4 | size: 0x2b4 */
extern void soundStartSE(void);
u16 exchangeDakuon__FUs11DAKUON_MODE(u16 letter, s32 mode);
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 selectLetter__FP14NAME_ENTRY_ARG(NAME_ENTRY_ARG* arg)
{
    extern void soundStartSE(u32 se);
    u16 letter;
    s32 done;
    u32 se;
    s32 index;
    s32 column;
    s32 action;
    s32 mode;
    s32 pos;
    s32 max;
    s32 row;
    s32* rowp;
    u16* name;
    u16 converted;

    done = 0;
    se = 0;
    index = *arg->letter;
    column = *arg->column;
    if (index < 0xf) {
        letter = menuNameEntryGetLetter(*arg->row, index, column);
        if (letter == 0) {
            letter = *(u16*)GSmsgGetGSchar(0x2ef9);
        }
        action = menuNameEntryGetLetterKind(letter);
    } else {
        switch (column) {
        case 0:
            action = 3;
            break;
        case 3:
            action = 5;
            break;
        default:
            action = 4;
            break;
        }
    }

    switch (action) {
    case 1:
    case 2:
        if (action == 1) {
            mode = 1;
        } else {
            mode = 1;
        }
        pos = *arg->state - 1;
        if (pos >= 0) {
            name = arg->name;
            converted = exchangeDakuon__FUs11DAKUON_MODE(name[pos], mode);
            if (converted != 0) {
                name[pos] = converted;
            }
        }
        se = 0x24;
        break;
    case 4:
        if (menuNameEntryDeleteLetter(arg)) {
            se = 0x25;
        }
        break;
    case 5:
        done = 1;
        break;
    case 3:
        rowp = arg->row;
        row = *rowp;
        row++;
        if (row >= 2) {
            row = 0;
        }
        *rowp = row;
        se = 0x27;
        break;
    case 0:
        letter = *(u16*)GSmsgGetGSchar(0x2ef9);
    default:
        max = ((s32*)(lbl_80266DD8 + 4))[arg->kind * 4];
        pos = *arg->state;
        if (pos >= max) {
            pos = max - 1;
        }
        name = &arg->name[pos];
        name[0] = letter;
        name[1] = 0;
        pos++;
        if (pos >= max + 1) {
            pos = max;
        }
        *arg->state = pos;
        se = 0x24;
        break;
    }
    if (se != 0) {
        soundStartSE(se);
    }
    return done;
}
#pragma pop
#endif /* !MENU_NAME_ENTRY_80027740_ONLY */

#if !(defined(MENU_NAME_ENTRY_80027740_ONLY) || defined(MENU_NAME_ENTRY_80027AA4_ONLY))

#if !defined(MENU_NAME_ENTRY_SUFFIX_ONLY)

/* menuNameEntryCursor - 0x80027D58 | size: 0x3a4 */
extern u16* windowGetKeyInfo(void);
static inline s32 menuNameEntryGetDakuonMode(u16 letter)
{
    u32* tables;
    s32 kind;
    s32 count;
    s32 i;
    u16* chars;

    kind = 0;
    do {
        tables = (u32*)lbl_8047B920;
        if (tables[kind] != 0) {
            count = GSmsgGetLength((void*)tables[kind]);
            chars = (u16*)GSmsgGetGSchar(tables[kind]) + 1;
            for (i = 1; i < count; i += 2, chars += 2) {
                if (*chars == letter) {
                    break;
                }
            }
            if (i < count) {
                break;
            }
        }
    } while (++kind < 2);
    if (kind >= 2) {
        kind = 0;
    }
    return kind;
}
#if 1
/* RULE-EXCEPTION(user-approved): local opt_strength_reduction off, so the inlined table walk's byte index is made by the backend (retail's register order) — see docs/RULE_EXCEPTIONS.md */
s32 menuNameEntryCursor(void* window)
{
    extern void soundStartSE(u32 se);
    u16* keys;
    u16* letters;
    s32 row;
    u16 converted;
    s32 value;
    u16* name;
    s32* rowp;
    s32 mode;
    s32 length;
    NAME_ENTRY_ARG* arg;
    s32 pos;
    s32 offset;
    s32 kind;

    keys = windowGetKeyInfo();
    arg = *(NAME_ENTRY_ARG**)((u8*)window + 0x60);
    if (keys[2] & 0x40) {
        rowp = arg->row;
        row = *rowp;
        row++;
        if (row >= 2) {
            row = 0;
        }
        *rowp = row;
        soundStartSE(0x27);
        return 0;
    }
    if (keys[2] & 0x10) {
        if (selectLetter__FP14NAME_ENTRY_ARG(arg)) {
            ((u8*)window)[0x98] = 1;
            return 0;
        }
        letters = arg->name;
        length = 0;
        while (*letters != 0) {
            letters++;
            length++;
        }
        if (length >= ((s32*)(lbl_80266DD8 + 4))[arg->kind * 4]) {
            *arg->letter = 0xf;
            *arg->column = 3;
        }
        return 0;
    }
    if (keys[2] & 0x20) {
        if (menuNameEntryDeleteLetter(arg)) {
            soundStartSE(0x25);
        }
        return 0;
    }
    if (keys[2] & 0x400) {
        pos = *arg->state - 1;
        if (pos >= 0) {
            name = arg->name;
            offset = pos;
            kind = menuNameEntryGetDakuonMode(name[offset]);
            mode = kind;
            do {
                mode++;
                if (mode >= 2) {
                    mode = 0;
                }
                converted = exchangeDakuon__FUs11DAKUON_MODE(name[offset], mode);
            } while (converted == 0);
            name[offset] = converted;
        }
        soundStartSE(0x24);
        return 0;
    }
    if (keys[2] & 0x800) {
        ((u8*)window)[0x98] = 1;
        return 0;
    }
    if (keys[3] & 8) {
        value = *arg->letter + 1;
        if (value >= 0x10) {
            value = 0xf;
        } else {
            soundStartSE(0x23);
        }
        *arg->letter = value;
    }
    if (keys[3] & 4) {
        value = *arg->letter - 1;
        if (value < 0) {
            value = 0;
        } else {
            soundStartSE(0x23);
        }
        *arg->letter = value;
    }
    if (keys[3] & 2) {
        value = *arg->column;
        if (*arg->letter >= 0xf) {
            if (value == 1) {
                value += 2;
            } else {
                value += 1;
            }
        } else {
            value += 1;
        }
        if (value >= 4) {
            value = 3;
        } else {
            soundStartSE(0x23);
        }
        *arg->column = value;
    }
    if (keys[3] & 1) {
        value = *arg->column;
        if (*arg->letter >= 0xf) {
            if (value == 2) {
                value -= 2;
            } else {
                value -= 1;
            }
        } else {
            value -= 1;
        }
        if (value < 0) {
            value = 0;
        } else {
            soundStartSE(0x23);
        }
        *arg->column = value;
    }
    return 0;
}
#endif

/* menuNameEntryCtrl - 0x800280FC | size: 0xf4 */
extern void winSeqSetMenu(void*, s32);
extern f32 lbl_8047B930;
extern f32 lbl_8047B950;
extern f32 lbl_8047B934;
s32 menuNameEntryCtrl(void* r3) {
    u8* r30;
    u8* r31;
    f32* fptr;
    f32 f0;
    f32 f1;
    f32 f2;
    s32 state;
    s32 flag;
    u8 one;

    r30 = (u8*)r3;
    state = (s32)(s8)*(volatile u8*)(r30 + 1);
    r31 = *(u8**)(r30 + 0x60);
    switch (state) {
    case 0:
        flag = (s32)(s8)*(volatile u8*)(r30 + 2);
        if (flag == 0) {
            winSeqSetMenu(*(void**)(r30 + 4), 0x56);
            one = 1;
            **(f32**)(r31 + 0x30) = lbl_8047B930;
            r30[2] = one;
        }
        break;
    case 2:
        fptr = *(f32**)(r31 + 0x30);
        f0 = lbl_8047B950;
        f2 = *(volatile f32*)fptr;
        f1 = lbl_8047B934;
        f0 = f2 + f0;
        *fptr = f0;
        if (f0 >= f1) {
            fptr = *(f32**)(r31 + 0x30);
            *fptr = *fptr - f1;
        }
        break;
    case 3:
        flag = (s32)(s8)*(volatile u8*)(r30 + 2);
        if (flag == 0) {
            winSeqSetMenu(*(void**)(r30 + 4), 0x5a);
            r30[2] = 1;
        }
        break;
    }
    return 0;
}

/* menuNameEntryButton - 0x800281F0 | size: 0x4 */
void menuNameEntryButton(void) { }

/* inputName__FPUsPUsiii - 0x800281F4 | size: 0x250 */
extern void GScharCpy(void*, u8*);
extern void dbgMenuSetEnable(void);
extern void windowGetActiveID(void);
extern void menuOpenCustom(void);
extern void winMsgOpen(void);
extern void fn_8001E074(void);
extern void winMsgClose(s32);
extern void menuClose(void);
extern void menuCloseSync(void);
extern void menuSubOpenYesNo(void);
extern void pcboxSetPokemonBoxName(void);
extern void GScharCmp(void);
extern void menuModelSetMotion(void);
extern void cameraWaitSyncAnime(void);
extern f32 sin(f32);
extern f32 cos(f32);
extern f32 lbl_8047B930;
extern u8 lbl_8047A3D4[4];
extern u8 lbl_8047A3D0[4];
extern u8 lbl_8047A3CC[4];
extern u8 lbl_8047A3C8[4];
extern u8 lbl_8047A3C4[4];
extern u32 lbl_8047A3C0;
extern u32 lbl_8047A3BC;
extern u32 lbl_8047A3B8;
extern u32 lbl_8047A3B4;
extern u32 lbl_8047A3B0;
extern u8 lbl_803A2068[];
s32 inputName__FPUsPUsiii(u16* name, u16* defaultName, s32 kind, s32 index, s32 canCancel)
{
    extern void dbgMenuSetEnable(s32 enable);
    extern u32 windowGetActiveID(void);
    extern void menuOpenCustom(s32 menuId, u32 parentId, s32, s32, s32, s32, ...);
    extern void soundStartSE(u32 se);
    extern void winMsgOpen(s32 slot, s32 msgId, s32, s32);
    extern s8 menuSubOpenYesNo(s32, s32, s32, s32);
    extern void winMsgClose(s32 slot);
    extern void menuClose(s32 menuId);
    extern void menuCloseSync(s32 menuId, s32 sync);

    NAME_ENTRY_ARG arg;
    s32 blanks;
    u16* letters;
    s32 length;
    u16* result;
    s32 valid;
    u16* blank;
    s32 accepted;
    s32 done;
    s32 yes;
    NAME_ENTRY_ARG* argp;
    s32 answer;

    done = 0;
    GScharCpy(arg.buffer, (u8*)defaultName);
    name[0] = 0;
    arg.name = name;
    arg.kind = kind;
    arg.index = index;
    *(s32*)lbl_8047A3D4 = 0;
    arg.row = (s32*)lbl_8047A3D4;
    *(s32*)lbl_8047A3D0 = 0;
    arg.letter = (s32*)lbl_8047A3D0;
    *(s32*)lbl_8047A3CC = 0;
    arg.column = (s32*)lbl_8047A3CC;
    *(f32*)lbl_8047A3C8 = 0.0f;
    arg.fade = (f32*)lbl_8047A3C8;
    *(s32*)lbl_8047A3C4 = 0;
    arg.state = (s32*)lbl_8047A3C4;
    arg.work0 = &lbl_8047A3C0;
    arg.work1 = &lbl_8047A3BC;
    arg.work2 = &lbl_8047A3B8;
    arg.work3 = &lbl_8047A3B4;
    arg.work4 = &lbl_8047A3B0;
    dbgMenuSetEnable(0);
    argp = &arg;
    while (!done) {
        menuOpenCustom(0x6e, windowGetActiveID(), 0, 0, 1, 1, argp);

        letters = arg.name;
        for (length = 0; letters[length] != 0; length++) {
        }
        if (length <= 0) {
            valid = 0;
        } else {
            blank = (u16*)GSmsgGetGSchar(0x2ef9);
            for (blanks = 0; blanks < length; blanks++, letters++) {
                if (*letters != *blank) {
                    break;
                }
            }
            if (blanks >= length) {
                valid = 0;
            } else {
                valid = 1;
            }
        }
        if (valid) {
            result = arg.name;
        } else {
            result = arg.buffer;
        }

        soundStartSE(0x440);
        msgctrlSetValue(0x4d, result);
        winMsgOpen(2, 0x2ef6, 1, 0);
        answer = menuSubOpenYesNo(0, -1, -1, 0);
        winMsgClose(1);
        if (answer == 1 || answer == -1) {
            yes = 0;
        } else {
            yes = 1;
        }
        accepted = yes;
        if (canCancel == 0) {
            done = 1;
        } else if (yes) {
            done = 1;
        }
    }
    dbgMenuSetEnable(1);
    menuClose(0x6e);
    menuCloseSync(0x6e, 1);
    if (accepted) {
        GScharCpy(lbl_803A2068, (u8*)result);
        return 1;
    }
    return 0;
}

#endif

/* menuNameEntryBackDrawHumanModel - 0x80028620 | size: 0x108 */
extern void* menuModelRender(void*);
extern void gfxClearFlags(s32);
extern void gfxSetFlags(s32);
extern void gfxSetVtxDesc(void*);
extern void gfxLoadTexture(s32, void*);
extern void gfxSetPrimitive(s32);
extern void gfxBegin(s32);
extern void gfxVtxPos2s16(s32, s32);
extern void gfxVtxColor4u8(s32, s32, s32, s32, s32);
extern void gfxVtxTexCoord(s32, f32, f32);
extern void gfxEnd(void);
extern u8 lbl_803A2094[];
extern u8 lbl_80314F98[];
extern f32 lbl_8047B930;
extern f32 lbl_8047B934;

/* menuNameEntryBackDrawPokemonModel - 0x80028728 | size: 0x108 */
extern f32 lbl_8047B930;
extern f32 lbl_8047B934;

#if !defined(MENU_NAME_ENTRY_PREFIX_ONLY)

/* menuNameEntryBackDrawBall - 0x80028830 | size: 0x118 */
extern void* menuSpriteBiosGetPtr(s32);
extern void windowDrawSprite2(s32, s32, s32, s32, u32, void*, s32, s32);
extern f64 lbl_8047B948;
extern f32 lbl_8047B940;
typedef struct WorldMapOverlay {
    s32 active;
    f32 x;
    f32 y;
    f32 scale;
    f32 unused10;
    u32 color;
    u8 alpha;
    u8 pad19[3];
    f32 timer;
    f32 lifetime;
} WorldMapOverlay;
extern WorldMapOverlay lbl_803A20DC[];
s32 menuNameEntryBackDrawBall(void* r3) {
    void* r27;
    WorldMapOverlay* r31;
    register s32 r30;
    register s32 r29;
    register s32 r28;
    f32 scale;
    f32 sx;
    f32 sy;
    s32 x0;
    s32 y0;
    s32 x1;
    s32 y1;

    r27 = r3;
    r29 = *(s16*)((u8*)menuSpriteBiosGetPtr(0x98) + 0xc);
    r28 = *(s16*)((u8*)menuSpriteBiosGetPtr(0x98) + 0xe);
    r30 = 0;
    while (r30 < 0x1e) {
        r31 = &lbl_803A20DC[r30];
        if (r31->active != 0) {
            scale = r31->scale;
            sx = (f32)r29 * scale;
            sy = (f32)r28 * scale;
            x1 = (s32)(lbl_8047B940 + sx);
            x0 = (s32)(lbl_8047B940 + (r31->x - sx * lbl_8047B940));
            y1 = (s32)(lbl_8047B940 + sy);
            y0 = (s32)(lbl_8047B940 + (r31->y - sy * lbl_8047B940));
            windowDrawSprite2(x0, y0, x1, y1, r31->color | r31->alpha, r27, 0x98, 0);
        }
        r30++;
    }
    return 0;
}

/* menuNameEntryBackCtrl - 0x80028948 | size: 0x674 */
extern f32 GSrandf(void);
extern f32 lbl_8047B958;
extern f32 lbl_8047B95C;
extern f32 lbl_8047B960;
extern f32 lbl_8047B964;
extern f32 lbl_8047B968;
extern f32 lbl_8047B930;
extern f32 lbl_8047B970;
extern f32 lbl_8047B96C;
extern f32 lbl_8047B934;
extern f32 lbl_8047B954;
/*
 * menuNameEntryBackCtrl - GSmap_MainRenderFrame (0x80028948, size 0x674)
 *
 * Particle/overlay state machine that drives the 30-entry overlay table
 * (lbl_803A20DC, WorldMapOverlay[30]) consumed by the renderer menuNameEntryBackDrawBall.
 *
 * Dispatches on the 1-byte mode field at offset 0x01 of the controller
 * object (signed):
 *   mode 0 : prime - clear all 30 overlays, then run 600 spawn/update ticks,
 *            finally latch the "primed" flag at offset 0x02.
 *   mode 2 : per-frame - probabilistically spawn one overlay, then update all.
 *   mode 3 : latch the flag at offset 0x02 (one-shot) and return.
 *   else   : no-op.
 *
 * Each tick: GSrandf() returns a random f32; if it is <= the spawn
 * threshold (lbl_8047B958) a new overlay is allocated in the first inactive
 * slot (if any of the 30 are free). The update pass ages every active
 * overlay, retires it when its timer reaches its lifetime, and recomputes
 * its per-frame scale and alpha.
 *
 * Big-endian note: the original advanced its timer / treated lbl_8047B934 as
 * both the per-tick increment and the constant 1.0 used in the alpha fade;
 * the same symbol is reused here so the value stays identical to the ROM.
 * The fctiwz->stb on the alpha is a truncate-to-int then low-byte store.
 */
/* RULE-EXCEPTION(user-approved): single-use inline helpers (Prime, Step) shape register numbering — see docs/RULE_EXCEPTIONS.md */
static inline void menuNameEntryBackPrime(void)
{
    u32 clearSlot;
    s32 i;
    f32 threshold;
    f32 ratio;
    WorldMapOverlay* base;
    WorldMapOverlay* ov;
    s32 slot;
    WorldMapOverlay* ov2;
    s32 slot2;

    base = lbl_803A20DC;
    for (ov = base, clearSlot = 1; clearSlot <= 30; clearSlot++) {
        ov->active = 0;
        ov++;
    }
    i = 0;
    threshold = lbl_8047B958;
    do {
        if (!(GSrandf() > threshold)) {
            for (slot = 0, ov = base; slot < 30; ov++, slot++) {
                if (ov->active == 0) {
                    break;
                }
            }
            if (slot < 30) {
                lbl_803A20DC[slot].active = 1;
                ov = &lbl_803A20DC[slot];
                ov->x = lbl_8047B95C * GSrandf();
                ov->y = lbl_8047B960 * GSrandf();
                ov->scale = lbl_8047B964;
                ov->unused10 = lbl_8047B968 * GSrandf() + lbl_8047B964;
                ov->color = -0x100;
                ov->alpha = 0x80;
                ov->timer = lbl_8047B930;
                ov->lifetime = lbl_8047B970 * GSrandf() + lbl_8047B96C;
            }
        }
        for (slot2 = 0, ov2 = base; slot2 < 30; ov2++, slot2++) {
            /* RULE-EXCEPTION(user-approved): repeated no-op test gives retail's doubled beq — see docs/RULE_EXCEPTIONS.md */
            if (ov2->active != 0 && ov2->active != 0) {
                if ((ov2->timer += lbl_8047B934) >= ov2->lifetime) {
                    ov2->active = 0;
                }
                ratio = ov2->timer / ov2->lifetime;
                ov2->scale = ov2->unused10 * ratio;
                ov2->alpha = lbl_8047B954 * (lbl_8047B934 - ratio);
            }
        }
    } while (++i < 0x258);
}

static inline void menuNameEntryBackStep(void)
{
    s32 slot;
    f32 ratio;
    WorldMapOverlay* ov;

    if (!(GSrandf() > lbl_8047B958)) {
        for (slot = 0, ov = lbl_803A20DC; slot < 30; ov++, slot++) {
            if (ov->active == 0) {
                break;
            }
        }
        if (slot < 30) {
            lbl_803A20DC[slot].active = 1;
            lbl_803A20DC[slot].x = lbl_8047B95C * GSrandf();
            lbl_803A20DC[slot].y = lbl_8047B960 * GSrandf();
            lbl_803A20DC[slot].scale = lbl_8047B964;
            lbl_803A20DC[slot].unused10 = lbl_8047B968 * GSrandf() + lbl_8047B964;
            lbl_803A20DC[slot].color = -0x100;
            lbl_803A20DC[slot].alpha = 0x80;
            lbl_803A20DC[slot].timer = lbl_8047B930;
            lbl_803A20DC[slot].lifetime = lbl_8047B970 * GSrandf() + lbl_8047B96C;
        }
    }
    for (slot = 0, ov = lbl_803A20DC; slot < 30; ov++, slot++) {
        /* RULE-EXCEPTION(user-approved): repeated no-op test gives retail's doubled beq — see docs/RULE_EXCEPTIONS.md */
        if (ov->active != 0 && ov->active != 0) {
            if ((ov->timer += lbl_8047B934) >= ov->lifetime) {
                ov->active = 0;
            }
            ratio = ov->timer / ov->lifetime;
            ov->scale = ov->unused10 * ratio;
            ov->alpha = lbl_8047B954 * (lbl_8047B934 - ratio);
        }
    }
}

s32 menuNameEntryBackCtrl(void* r3)
{
    extern f32 GSrandf(void);            /* random f32 source (-> f1) */
    extern f32 lbl_8047B958;                 /* spawn threshold           */
    extern f32 lbl_8047B95C;                 /* x scale                   */
    extern f32 lbl_8047B960;                 /* y scale                   */
    extern f32 lbl_8047B964;                 /* scale base / init scale   */
    extern f32 lbl_8047B968;                 /* scale rand coeff          */
    extern f32 lbl_8047B96C;                 /* lifetime base             */
    extern f32 lbl_8047B970;                 /* lifetime rand coeff       */
    extern f32 lbl_8047B930;                 /* initial timer             */
    extern f32 lbl_8047B934;                 /* timer increment / 1.0     */
    extern f32 lbl_8047B954;                 /* alpha scale               */

    u8* ctl;

    ctl = (u8*)r3;
    switch ((s8)ctl[1]) {
    case 0:
        if ((s8)ctl[2] != 0) {
            break;
        }
        menuNameEntryBackPrime();
        ctl[2] = 1;
        break;

    case 2:
        menuNameEntryBackStep();
        break;

    case 3:
        if ((s8)ctl[2] == 0) {
            ctl[2] = 1;
        }
        break;
    }
    return 0;
}

/* menuNameEntry - 0x80028FBC | size: 0x59c */
extern void pokemonBiosGetNicknamePtr(void);
extern void pcboxGetPokemonBoxName(void);
extern void menuItemBiosGetPtr(void);
extern void menuModelInit(void);
extern void menuPokemonModelRequestById(void);
extern void peopleInfoBiosGetPtr(void);
extern void fn_8018F4C8(void);
extern void menuPokemonModelRequest(void);
extern void menuModelCheck(void);
extern void fadeSet(void);
extern void fadeCheck(void);
extern void menuModelTerm(void);
extern void heroSetStatus(void);
extern void pokemonBiosSetNicknamePtr(void);
extern void fn_801349DC(void);
extern void fn_800F9EE4(void);
extern void floorBattleEndTerminateScripts(void);
extern void floorSetFadeScript(s32, u32);
extern s32 lbl_804788A0;
extern u8 lbl_80266DC0[];
extern f32 lbl_8047B940;
/* menuNameEntry - GSmap_MainUpdate (0x80028FBC, size 0x59C)
 *
 * World-map main update loop. Operates entirely on module globals:
 *   - lbl_803A2068 : the GSmap context block. Layout used here:
 *         +0x00 u16   (cleared by callers)
 *         +0x18 s32   mode      (selector category, range 0..3)
 *         +0x1c s32   subIndex
 *         +0x20 s32   result    (written here, read by callers)
 *         +0x24 s32   flag24
 *         +0x28 s32   asyncMode (1 => render/finalize path active)
 *   - lbl_80266DC0 : map data blob. Header field +0x00 (ptr) and +0x0C (ptr),
 *         followed at +0x18 by an array of 0x10-byte entries; each entry has
 *         a payload pointer at +0x08 and an element count at +0x0C.
 *   - lbl_802EF0A8 : large read-only data blob (cross-TU); five s16 fields are
 *         latched into the lbl_8047A3xx scratch globals on the first frame.
 *   - lbl_804788A0 : "first frame / needs-latch" flag.
 *   - lbl_803A2094 : the world-map menu-model handle.
 *
 * Byte-match is irrelevant; this reproduces the x86 semantics of the loop.
 */
/* RULE-EXCEPTION(user-approved): local optimizer controls and widened choice local; see docs/RULE_EXCEPTIONS.md */
void menuNameEntry(void) {
    /* --- module globals (block-scope typed externs, TU convention) --- */
    extern u8  lbl_803A2068[];     /* GSmap context block            */
    extern u8  lbl_80266DC0[];     /* map data blob                  */
    extern u8  lbl_802EF0A8[];     /* far read-only data blob        */
    extern u8  lbl_803A2094[];     /* menu-model handle              */
    extern s32 lbl_804788A0;       /* first-frame latch flag         */
    extern u32 lbl_8047A3C0;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3BC;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3B8;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3B4;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3B0;  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047B940;       /* 0.0f constant                  */

    /* --- callees (minimal real signatures inferred from each bl site) --- */
    extern u32  GSmsgGetGSchar(u32 id);                       /* id -> resource ptr        */
    extern u32  heroGetStatus(u8* ptr, u32 selector, u32 idx);
    extern u32  pokemonCheckValid(void);                         /* returns u8 status         */
    extern u32  pokemonBiosGetNicknamePtr(u32 a);
    extern u32  pcboxGetPokemonBoxName(s32 a, s32 b);
    extern void GScharCpy(void* dst, u8* src);           /* copy/build name struct    */
    extern void* menuItemBiosGetPtr(s32 id);                      /* returns struct ptr        */
    extern void menuModelInit(void* handle, s16 a, s16 b);
    extern void menuPokemonModelRequestById(void* handle, s32 v);
    extern void* peopleInfoBiosGetPtr(s32 v);
    extern void fn_8018F4C8(void* info, s32 a, s32* outA, s32* outB);
    extern void menuModelSetMotion(void* handle, s32 motion);
    extern void menuPokemonModelRequest(void* handle, u32 v);
    extern void menuModelCheck(void* handle, s32 v);
    extern void menuModelTerm(void* handle);
    extern void fadeSet(s32 mode, f32 v);
    extern void fadeCheck(s32 v);
    extern u32  windowGetActiveID(void);                         /* returns context handle    */
    extern s32  menuOpenCustom(s32 id, u32 ctx, s32 a, s32 b, s32 c, s32 d, ...);
    extern u32  soundStartSE(s32 size);
    extern void msgctrlSetValue(s32 id, u32 name);
    extern void winMsgOpen(s32 a, s32 b, s32 c, s32 d);
    extern s8   menuSubOpenYesNo(s32 a, s32 b, s32 c, s32 d);
    extern void winMsgClose(s32 a);
    extern void menuClose(s32 id);
    extern void menuCloseSync(s32 id, s32 a);
    extern s32  inputName__FPUsPUsiii(u8* ctx, void* nameBuf, s32 mode, s32 subIndex, s32 last);
    extern void heroSetStatus(s32 a, s32 b, u8* ctx);
    extern void pokemonBiosSetNicknamePtr(u32 v, u8* ctx);
    extern void pcboxSetPokemonBoxName(s32 a, s32 name, u8* ctx);
    extern s32  GScharCmp(u8* ctx, void* nameBuf);
    extern void floorBattleEndTerminateScripts(void);
    extern void floorSetFadeScript(s32 a, u32 b);

    u8* ctx;        /* lbl_803A2068 context block          */
    u8* data;       /* lbl_80266DC0 map data blob          */
    NameEntryChoiceList* entries;
    u8* ctx2;
    s32 mode;       /* ctx +0x18                           */
    s32 subIndex;   /* ctx +0x1c                           */
    s32 r0;         /* generic selection result            */
    u32 sel;        /* selection / pokemon handle          */
    s32 ok;
    s32* listPtr;
    s32 accepted;
    s32 count;
    u32 pkm;
    s32* listp;
    void* mdl;      /* struct ptr from menuItemBiosGetPtr          */
    s32 entryBuf[4];
    u16 nameBuf[12];
    s32 menuArg[2];
    s32 listArg[2];
    s32 motOut;     /* fn_8018F4C8 out word @ sp+0xc        */
    s32 motTmp;     /* fn_8018F4C8 out word @ sp+0x8        */
    s32 ans;

    data = lbl_80266DC0;

    /* First-frame latch: copy fixed display params out of the big data blob. */
    if (lbl_804788A0 != 0) {
        lbl_804788A0 = 0;
        (*(s32*)&lbl_8047A3C0) = (s32)*(s16*)(lbl_802EF0A8 + 0x6fca);
        (*(s32*)&lbl_8047A3BC) = (s32)*(s16*)(lbl_802EF0A8 + 0x6fcc);
        (*(s32*)&lbl_8047A3B8) = (s32)*(s16*)(lbl_802EF0A8 + 0x20ec2);
        (*(s32*)&lbl_8047A3B4) = (s32)*(s16*)(lbl_802EF0A8 + 0x73ba);
        (*(s32*)&lbl_8047A3B0) = (s32)*(s16*)(lbl_802EF0A8 + 0x70fe);
    }

    ctx = lbl_803A2068;
    mode = *(s32*)(ctx + 0x18);
    subIndex = *(s32*)(ctx + 0x1c);

    /* --- Resolve the current selection (r3) from the mode switch. --- */
    sel = 0;
    switch (mode) {
    case 0:
        sel = GSmsgGetGSchar(*(u32*)(data + 0x0));
        break;
    case 1:
        sel = GSmsgGetGSchar(*(u32*)(data + 0xc));
        break;
    case 2:
        r0 = (s32)heroGetStatus(0, 3, (u16)subIndex);
        if ((pokemonCheckValid() & 0xff) == 0) {
            sel = 0;
        } else {
            sel = pokemonBiosGetNicknamePtr((u32)r0);
        }
        break;
    case 3:
        sel = pcboxGetPokemonBoxName(0, (s32)(s8)subIndex);
        break;
    default:
        sel = 0;
        break;
    }

    /* Build the name buffer from the selection; if none, zero the head. */
    if (sel == 0) {
        ok = 0;
    } else {
        GScharCpy(nameBuf, (u8*)sel);
        ok = 1;
    }
    if (ok == 0) {
        *(u16*)nameBuf = 0;
    }

    /* Snapshot four words of the data header into a scratch frame array
       (these are indexed by 'mode' below). */
    mode = *(s32*)(ctx + 0x18);
    entryBuf[0] = *(s32*)(data + 0x88);
    entryBuf[1] = *(s32*)(data + 0x8c);
    entryBuf[2] = *(s32*)(data + 0x90);
    entryBuf[3] = *(s32*)(data + 0x94);
    subIndex = *(s32*)(ctx + 0x1c);

    /* Fetch the model descriptor (0xd3a for mode 2, else 0xd39) and pose it. */
    mdl = menuItemBiosGetPtr((mode == 2) ? 0xd3a : 0xd39);
    menuModelInit(lbl_803A2094, *(s16*)((u8*)mdl + 0x6), *(s16*)((u8*)mdl + 0x8));

    /* --- Per-mode model setup. --- */
    switch (mode) {
    case 0:
    case 1:
        sel = entryBuf[mode];
        menuPokemonModelRequestById(lbl_803A2094, sel);
        fn_8018F4C8(peopleInfoBiosGetPtr(sel), 1, &motOut, &motTmp);
        menuModelSetMotion(lbl_803A2094, motOut);
        break;
    case 2:
        sel = heroGetStatus(0, 3, (u16)subIndex);
        if ((pokemonCheckValid() & 0xff) != 0) {
            menuPokemonModelRequest(lbl_803A2094, sel);
        }
        break;
    case 3:
        menuPokemonModelRequestById(lbl_803A2094, entryBuf[mode]);
        break;
    default:
        break;
    }
    menuModelCheck(lbl_803A2094, 1);

    /* --- Open the primary menu window (id 0x6f) seeded with the context. --- */
    menuArg[0] = *(s32*)(ctx + 0x18);
    menuArg[1] = *(s32*)(ctx + 0x1c);
    menuOpenCustom(0x6f, windowGetActiveID(), 0, 0, 1, 1, menuArg);

    /* On async/render frames, prime the transition. */
    ctx2 = lbl_803A2068;
    if (*(s32*)(ctx2 + 0x28) == 0) {
        fadeSet(2, lbl_8047B940);
        fadeCheck(1);
    }

    /* --- List/confirm loop over the current mode's entry array. --- */
    entries = (NameEntryChoiceList*)(data + 0x18);
    if (entries[*(s32*)(ctx + 0x18)].count > 0) {
        listp = listArg;
        for (;;) {
            NameEntryChoiceList* entry = (NameEntryChoiceList*)(data + 0x18);
            s64 choiceName;
            entry += *(s32*)(ctx + 0x18);
            count = entry->count;
            listPtr = entry->choices;
            for (;;) {
                s32 pick;
                listArg[0] = (s32)listPtr;
                listArg[1] = count;
                pick = menuOpenCustom(0x70, windowGetActiveID(), 0, 0, 1, 1, listp);
                if (pick == 0) {
                    soundStartSE(0x24);
                    accepted = 0;
                    break;
                }
                if (pick == -1) {
                    continue;
                }
                choiceName = (u32)GSmsgGetGSchar((u32)listPtr[pick - 1]);
                {
                    s32 yes;
                    soundStartSE(0x440);
                    msgctrlSetValue(0x4d, choiceName);
                    winMsgOpen(2, 0x2ef6, 1, 0);
                    ans = menuSubOpenYesNo(0, -1, -1, 0);
                    winMsgClose(1);
                    if (ans == 1 || ans == -1) {
                        yes = 0;
                    } else {
                        yes = 1;
                    }
                    if (yes == 0) {
                        continue;
                    }
                }
                accepted = 1;
                break;
            }
            menuClose(0x70);
            menuCloseSync(0x70, 1);
            {
                s32 copied;
                if (accepted) {
                    GScharCpy(lbl_803A2068, (u8*)choiceName);
                    copied = 1;
                } else {
                    copied = 0;
                }
                if (copied) {
                    break;
                }
            }
            if (inputName__FPUsPUsiii(lbl_803A2068, nameBuf, *(s32*)(ctx + 0x18), *(s32*)(ctx + 0x1c), 0) != 0) {
                break;
            }
        }
    } else {
        inputName__FPUsPUsiii(lbl_803A2068, nameBuf, *(s32*)(ctx + 0x18), *(s32*)(ctx + 0x1c), 1);
    }

    /* --- Tear down the menu model and finalize the selection by mode. --- */
    fadeSet(3, lbl_8047B940);
    fadeCheck(1);
    menuModelTerm(lbl_803A2094);
    menuClose(0x6f);
    menuCloseSync(0x6f, 1);

    mode = *(s32*)(ctx + 0x18);
    subIndex = *(s32*)(ctx + 0x1c);
    switch (mode) {
    case 0:
        break;
    case 1:
        heroSetStatus(0, 0x17, lbl_803A2068);
        break;
    case 2:
        pkm = heroGetStatus(0, 3, (u16)subIndex);
        if ((pokemonCheckValid() & 0xff) != 0) {
            pokemonBiosSetNicknamePtr(pkm, lbl_803A2068);
        }
        break;
    case 3:
        pcboxSetPokemonBoxName(0, (s32)(s8)subIndex, lbl_803A2068);
        break;
    default:
        break;
    }

    /* result = (name changed) ? 1 : 0 */
    if (GScharCmp(lbl_803A2068, nameBuf) == 0) {
        *(s32*)(lbl_803A2068 + 0x20) = 0;
    } else {
        *(s32*)(lbl_803A2068 + 0x20) = 1;
    }

    /* Async finalize: post the appropriate completion event. */
    if (*(s32*)(ctx2 + 0x28) != 0) {
        floorBattleEndTerminateScripts();
        if (*(s32*)(lbl_803A2068 + 0x24) != 0) {
            floorSetFadeScript(0, 0x05960008);
        } else {
            floorSetFadeScript(0, 0);
        }
    }
}

#endif
#endif /* not a single-range build */

#endif /* !MENU_NAME_ENTRY_80026370_ONLY */
