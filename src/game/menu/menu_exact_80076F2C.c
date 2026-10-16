/**
 * @file menu_exact_80076F2C.c
 * @brief Exact island 0x80076F2C - 0x800776E4 (fn_80076F2C, fn_800772AC,
 *        fn_800774D4): standalone copies, in address order, of the bodies in
 *        menu_range_8007109C.c, with the helpers, types and externs they use.
 */
#include "dolphin/types.h"
#include "game/hero.h"
#include "game/win_sprite.h"

typedef struct MenuDVDFileInfo {
    u8 pad[0x34];
    u32 length;
    void* callback;
} MenuDVDFileInfo;

typedef struct GbaBootContext {
    u8 alarm[0x28];
    u8 thread_and_work[0x318];
    u8 channel;
    u8 pad_341;
    u8 cancel;
    u8 reject_upload;
    u8 status_only;
    u8 pad_345;
    u8 state;
    u8 pad_347;
    u16 boot_type;
    u8 pad_34A[2];
    u32 device_code;
    u32 game_code;
    u32 crc;
    u32 challenge;
    u8 pad_35C[0x24];
    u8* upload_start;
    u8* upload_end;
    u8 pad_388[0x2000];
    u32 reply[0x410];
} GbaBootContext;

typedef struct MenuExDiscLoadData {
    u32 words[4];
} MenuExDiscLoadData;

typedef struct ExDiscCouponResult {
    u32 flagsA62C;
    u32 couponValue;
    u32 optionFlags;
    u16 partyCount;
    u16 itemId;
    u8 bag[0xC8];
} ExDiscCouponResult;

typedef struct MenuRankSprite {
    u8 pad_00[6];
    s16 id;
} MenuRankSprite;

#define GBA_BOOT_BSWAP(v)                                                    \
    ((((u32)(v) & 0xFF000000) >> 24) | (((u32)(v) & 0x00FF0000) >> 8)      \
     | (((u32)(v) & 0x0000FF00) << 8) | (((u32)(v) & 0x000000FF) << 24))

#define GBA_BOOT_FAIL(ctx, code) \
    do {                         \
        (ctx)->state = (code);   \
        return (code);           \
    } while (0)

#define GBA_BOOT_DELAY(ctx, handler, level)                  \
    do {                                                      \
        OSCreateAlarm((ctx));                                 \
        (level) = OSDisableInterrupts();                       \
        OSSetAlarm((ctx), 0x10, (void*)(handler));             \
        OSSuspendThread((u8*)(ctx) + 0x28);                    \
        OSRestoreInterrupts(level);                            \
    } while (0)

#define GBA_BOOT_CLASSIFY(ctx, response, gba_status, result)                 \
    do {                                                                     \
        u32 boot_delta;                                                      \
        if ((response) == (ctx)->game_code) {                                \
            (result) = 1;                                                    \
        } else if (((gba_status) & 0x30) != 0) {                             \
            (result) = 0;                                                    \
        } else {                                                             \
            boot_delta = (ctx)->device_code ^ (response);                    \
            if (boot_delta == 0x20000000) {                                  \
                (ctx)->boot_type = 0x100;                                    \
                (result) = 2;                                                \
            } else if (boot_delta == 0x00200000) {                           \
                (ctx)->boot_type = 0x200;                                    \
                (result) = 2;                                                \
            } else {                                                         \
                (ctx)->boot_type = 0;                                        \
                (result) = ((boot_delta & 0xDFDFDFDF) == 0) ? 2 : 0;         \
            }                                                                \
        }                                                                    \
    } while (0)

#define SET_EXDISC_VISIBILITY(id_, visible_)                       \
    do {                                                           \
        void* model_ = GSresGetResource(std093_getCurrentGroupID(), (id_));      \
        if (model_ != 0) {                                         \
            GSmodelSetVisibility(model_, (visible_));              \
        }                                                          \
    } while (0)

#define RESTORE_EXDISC_SCENE(handle_)                              \
    do {                                                           \
        fadeSet(3, lbl_8047C108);                                  \
        fadeCheck(1);                                              \
        std113_mapObjectUnk_8018db68((u32)(handle_));                               \
        SET_EXDISC_VISIBILITY(0x104E1000, 1);                      \
        SET_EXDISC_VISIBILITY(0x104E1001, 1);                      \
        SET_EXDISC_VISIBILITY(0x104E1002, 1);                      \
        cameraPlayAnime((s32)std093_getCurrentGroupID(), 0x10941800, 0, 0);    \
        fadeSet(2, lbl_8047C108);                                  \
        fadeCheck(1);                                              \
    } while (0)

/* GBA link timing: OS_TIMER_CLOCK / OSMillisecondsToTicks, see include/dolphin/si/SI.h */
#define OS_BUS_CLOCK   (*(u32*)0x800000F8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)
#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))

extern u8 lbl_803F7A30[];
extern u8 lbl_80268940[];
extern u8 lbl_803F6F18[];
extern u8 lbl_803F6E40[];
extern u8 lbl_803B6E40[];
extern u8 lbl_803D6E40[];
extern u8 lbl_80268780[];
extern u32 lbl_8047A604;
extern u32 lbl_8047A608;
extern u32 lbl_8047A620;
extern u32 lbl_8047A628;
extern u32 lbl_8047A62C;
extern u8 lbl_8047A630;
extern u8 lbl_8047A631;
extern u8 lbl_8047A632;
extern u8 lbl_8047A633;
extern u8 lbl_8047A634;
extern u8 lbl_8047A635;
extern u32 lbl_8047A638;
extern u32 lbl_80268AD0[];
extern u8 lbl_80268AE0[];
extern const f32 lbl_8047C0E0;
extern const f32 lbl_8047C0E4;
extern f32 lbl_8047C100;
extern const f32 lbl_8047C108;
extern u8 lbl_8047C10C;
extern u32 OSGetTick(void);
extern void _threadSwitch(void);

/* Retail inlines this wait loop into its callers rather than calling out to
 * it: framescan shows fn_80078390 and fn_800788BC each hold lbl_8047C0E0 /
 * lbl_8047C0E4 and the two int->float conversion biases in callee-saved FPRs
 * (f27-f31), which only happens when the loop is in the function body. Ours
 * emitted `bl MenuWaitMotionInterval` and saved no FPRs at all.
 * The timer externs are declared in-body because the file-scope ones come
 * later than the first call sites. */
static inline void MenuWaitMotionInterval(void)
{
    extern u32 GSgetFrameStep(void);
    extern s32 fn_800D37CC(void);
    f32 elapsed;

    elapsed = lbl_8047C0E0;
    while (elapsed < lbl_8047C0E4) {
        _threadSwitch();
        elapsed += (f32)GSgetFrameStep() / (f32)fn_800D37CC();
    }
}
extern void winMsgOpenField(s32, s32, s32);
extern void winMsgOpen(s32, s32, s32, s32);
extern void winMsgClose(s32);
extern s32 GBAReset(s32 chan, u8* status);
extern void OSCreateAlarm(void*);
extern u32 OSDisableInterrupts(void);
extern void OSSetAlarm(void*, s64, void*);
extern s32 OSSuspendThread(void*);
extern void OSRestoreInterrupts(u32);
extern void fn_8007C23C(u8*);
extern u32 lbl_803FAEF8[256];
extern void* heroBiosGetNamePtr(void*);
extern void* heroBiosGetPokemonPtr(void*, u16);
extern void* std093_getCurrentGroupID(void);
extern void* GSresGetResource(void*, u32);
extern void GSmodelSetVisibility(void*, u32);
extern void* std112_modelGetResourceID(u32);
extern void cameraPlayAnime(s32, u32, s32, s32);
extern void fadeSet(s32, f32);
extern s32 fadeCheck(s32);
extern void std113_mapObjectUnk_8018db68(u32);
extern void GSscene_SetMode(s32);
extern u8 fn_80079EF4(s32, u32);
extern void gbaCommandSetKeyState(s32 mode, s32 flag);
extern s32 fn_80073C38(s32 chan);
extern void fn_80072684(void);
extern s32 GBAWrite();
extern s32 GBARead();
extern s32 GBAGetStatus();

typedef struct GbaIdleCallback {
    void (*func)(s32 chan, void* arg);
    void* arg;
} GbaIdleCallback;

extern GbaIdleCallback lbl_803B6E18[5];
extern volatile s32 lbl_803B6E08[4];
extern u8 lbl_803B6D88[0x58];
extern void floorLink(s32, s32);

/* Same test as fn_80077A5C. */
static inline s32 menuRuleSlotIsEmpty(void* pokemon)
{
    extern s32 pokemonGetStatus(void*, s32, s32, s32);

    return pokemon == 0 || pokemonGetStatus(pokemon, 0, 0x6E, 0) == 0;
}

/* Shadow Lugia and the other event Pokemon may only enter once their
 * event flag is set. */
static inline u8 menuRuleEventPokemonAllowed(void* pokemon)
{
    extern u16 pokemonBiosGetPokemonDataId(void*);
    extern u8 pokemonBiosGetEventGetFlag(void*);

    switch (pokemonBiosGetPokemonDataId(pokemon)) {
    case 0x97:
    case 0x19A:
        if (pokemonBiosGetEventGetFlag(pokemon) == 0) {
            return 0;
        }
    }
    return 1;
}

typedef struct MenuRuleItemRestrictions {
    u8 pad_00[8];
    s32 mode;
    u8 pad_0C[0xC];
    u8 item_disabled[0x3C];
} MenuRuleItemRestrictions;
extern u8 lbl_80268A48[];
extern u8 lbl_80268A58[];
extern u16 lbl_802EE458[];
extern u32 lbl_80478928;



/* Check the whole party against the selected party rule. */
u8 fn_80076F2C(void* hero, const u8* rule, s32 mode)
{
    extern s32 pokemonGetStatus();
    extern u8 pokemonBiosGetTamagoFlag();
    extern u8 pokemonCheckValid();
    extern u8 pokemonBiosGetLevel();
    extern u16 pokemonBiosGetItemDataId();
    extern u16 pokemonBiosGetPokemonDataId();
    s32 level_total;
    u8 unique_species;
    u8 unique_items;
    u32 i;
    u32 j;
    s32 inner_rejected, inner_invalid, inner_present;
    s32 outer_rejected, outer_invalid, outer_present;
    void* pokemon;
    void* other;

    level_total = 0;
    unique_species = 1;
    unique_items = 1;
    for (i = 0; i < 6; i++) {
        u8 result;

        pokemon = heroBiosGetPokemonPtr(hero, (u16)i);

        outer_present = pokemon == 0;
        outer_invalid = outer_present != 0 ||
                        pokemonGetStatus(pokemon, 0, 0x6E, 0) == 0;
        if ((s32)outer_invalid != 0) {
            continue;
        }

        outer_rejected = 0;
        if ((s32)outer_present == 0) {
            if (pokemonGetStatus(pokemon, 0, 0x6E, 0) != 0) {
                goto pokemon_valid;
            }
        }
        outer_rejected = 1;
pokemon_valid:
        if ((s32)outer_rejected != 0) {
            result = 0;
        } else {
            outer_invalid = 0;
            if (pokemonBiosGetTamagoFlag(pokemon) == 0) {
                if (pokemonCheckValid(pokemon) != 0) {
                    goto pokemon_rejected;
                }
            }
            outer_invalid = 1;
pokemon_rejected:
            result = (u8)outer_invalid;
        }
        if ((u8)result != 0) {
            continue;
        }

        level_total += pokemonBiosGetLevel(pokemon);
        for (j = i + 1; j < 6; j++) {
            u8 other_result;

            other = heroBiosGetPokemonPtr(hero, (u16)j);

            inner_present = other == 0;
            inner_invalid = 0;
            if ((s32)inner_present == 0) {
                if (pokemonGetStatus(other, 0, 0x6E, 0) != 0) {
                    goto other_present;
                }
            }
            inner_invalid = 1;
other_present:
            if ((s32)inner_invalid != 0) {
                continue;
            }

            inner_rejected = 0;
            if ((s32)inner_present == 0) {
                if (pokemonGetStatus(other, 0, 0x6E, 0) != 0) {
                    goto other_valid;
                }
            }
            inner_rejected = 1;
other_valid:
            if ((s32)inner_rejected != 0) {
                other_result = 0;
            } else {
                inner_rejected = 0;
                if (pokemonBiosGetTamagoFlag(other) == 0) {
                    if (pokemonCheckValid(other) != 0) {
                        goto other_rejected;
                    }
                }
                inner_rejected = 1;
other_rejected:
                other_result = (u8)inner_rejected;
            }
            if ((u8)other_result != 0) {
                continue;
            }

            if (pokemonBiosGetItemDataId(pokemon) != 0) {
                unique_items &= pokemonBiosGetItemDataId(pokemon) !=
                                pokemonBiosGetItemDataId(other);
            }
            unique_species &= pokemonBiosGetPokemonDataId(pokemon) !=
                              pokemonBiosGetPokemonDataId(other);
        }
    }

    switch (mode) {
    case 0:
        return level_total <= *(const s16*)(rule + 4);
    case 1:
        return rule[0xC] != 0 || unique_species != 0;
    case 2:
        return rule[0xD] != 0 || unique_items != 0;
    case 3:
    {
        extern void* heroBiosGetPokemonPtr();
        s32 count;
        s32 index;
        s32 slot_invalid;

        count = 0;
        index = 0;
        while ((u16)index < 6) {
            pokemon = heroBiosGetPokemonPtr(hero, index);
            slot_invalid = 0;
            if (pokemon != 0) {
                if (pokemonGetStatus(pokemon, 0, 0x6E, 0) != 0) {
                    goto count_present;
                }
            }
            slot_invalid = 1;
count_present:
            if (slot_invalid == 0) {
                count++;
            }
            index++;
        }
        return *(const s16*)(rule + 6) <= (u16)count;
    }
    }

    return 0;
}

static inline u8 menuRuleCheckPresentPokemonMode(void* pokemon,
                                                 const s16* levels, s32 mode,
                                                 const char* file,
                                                 const char* cond)
{
    extern u8 pokemonBiosGetLevel(void*);
    extern u16 pokemonBiosGetItemDataId(void*);
    extern u8 fn_80142984(u16);
    extern u8* fn_8006B420(void);
    MenuRuleItemRestrictions* restrictions;
    u16 item;
    u32 i;
    u8 valid;

    switch (mode) {
    case 0:
        return pokemonBiosGetLevel(pokemon) >= levels[0];
    case 1:
        return pokemonBiosGetLevel(pokemon) <= levels[1];
    case 2:
        item = pokemonBiosGetItemDataId(pokemon);
        restrictions = (MenuRuleItemRestrictions*)fn_8006B420();
        switch (item) {
        case 0:
            valid = 1;
            break;
        case 0xAF:
            valid = 0;
            break;
        default:
            valid = fn_80142984(item);
            break;
        }
        if (valid == 0) {
            return 0;
        }

        switch (restrictions->mode) {
        case 0:
            return 1;
        case 1:
            return item == 0;
        case 2:
            for (i = 0; i < lbl_80478928; i++) {
                if (item == lbl_802EE458[i]) {
                    return restrictions->item_disabled[i] == 0;
                }
            }
            return 1;
        default:
            return 0;
        }
    default:
        __assert(file, 0xFB, cond);
        return 0;
    }
}

static inline u8 menuRuleCheckPokemonMode(void* pokemon, const s16* levels,
                                          s32 mode)
{
    extern s32 pokemonGetStatus(void*, s32, s32, s32);
    s32 is_null;
    s32 blank;

    is_null = pokemon == 0;
    blank = 0;
    if (is_null == 0) {
        if (pokemonGetStatus(pokemon, 0, 0x6E, 0) != 0) {
            goto pokemon_present;
        }
    }
    blank = 1;
pokemon_present:
    if (blank != 0) {
        return 1;
    }
    return menuRuleCheckPresentPokemonMode(pokemon, levels, mode,
                                           (const char*)lbl_80268A48,
                                           (const char*)lbl_80268A58);
}

u8 fn_800772AC(void* pokemon, const s16* levels)
{
    s32 mode;

    for (mode = 0; mode < 3; mode++) {
        if (menuRuleCheckPokemonMode(pokemon, levels, mode) == 0) {
            return 0;
        }
    }
    return 1;
}

static inline u8 menuRuleCheckSlotMode(void* pokemon, const s16* levels,
                                       s32 mode, const char* file,
                                       const char* cond)
{
    extern s32 pokemonGetStatus(void*, s32, s32, s32);
    s32 blank;

    blank = 0;
    if (pokemon == 0 || pokemonGetStatus(pokemon, 0, 0x6E, 0) == 0) {
        blank = 1;
    }
    if (blank != 0) {
        return 1;
    }
    return menuRuleCheckPresentPokemonMode(pokemon, levels, mode, file, cond);
}

u8 fn_800774D4(void* pokemon, const s16* levels, s32 mode)
{
    return menuRuleCheckSlotMode(pokemon, levels, mode,
                                 (const char*)lbl_80268A48,
                                 (const char*)lbl_80268A58);
}
