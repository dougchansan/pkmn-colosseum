/**
 * @file menu_exact_80076334.c
 * @brief Exact island 0x80076334 - 0x80076A8C (menuCBRule_CheckPokemonErrorAll,
 *        fn_80076398, fn_800767B8): standalone copies, in address order, of
 *        the bodies in menu_range_8007109C.c (candidate
 *        menu_candidate_80076054 keeps fn_80076054 and fn_80076A8C).
 */
#include "dolphin/types.h"
#include "game/hero.h"
#include "game/win_sprite.h"


typedef struct MenuRuleItemRestrictions {
    u8 pad_00[8];
    s32 mode;
    u8 pad_0C[0xC];
    u8 item_disabled[0x3C];
} MenuRuleItemRestrictions;

extern u32 lbl_80478928;
extern u16 lbl_802EE458[];
extern u8 lbl_80268A48[];
extern u8 lbl_80268A58[];
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

/* menuCBRule_CheckPokemonErrorAll (0x80076334): require every party member to
 * pass the per-slot error check. */
u8 menuCBRule_CheckPokemonErrorAll(void* pokemon) {
    extern u8 fn_80076398(void* pokemon, s32 index);
    s32 i;

    for (i = 0; i < 6; i++) {
        if (fn_80076398(pokemon, i) == 0) {
            return 0;
        }
    }
    return 1;
}

u8 fn_80076398(void* pokemon, s32 check)
{
    extern s32 pokemonGetStatus(void*, s32, s32, u16);
    extern void pokemonBiosCopy(void*, void*);
    extern void pokemonResetBasisStatus(void*);
    extern u16 pokemonBiosGetMaxHp(void*);
    extern u16 pokemonBiosGetPhyAtk(void*);
    extern u16 pokemonBiosGetPhyDef(void*);
    extern u16 pokemonBiosGetSpeAtk(void*);
    extern u16 pokemonBiosGetSpeDef(void*);
    extern u16 pokemonBiosGetNimbleness(void*);
    extern u16 pokemonBiosGetMaxHpEffort(void*);
    extern u16 pokemonBiosGetPhyAtkEffort(void*);
    extern u16 pokemonBiosGetPhyDefEffort(void*);
    extern u16 pokemonBiosGetSpeAtkEffort(void*);
    extern u16 pokemonBiosGetSpeDefEffort(void*);
    extern u16 pokemonBiosGetNimblenessEffort(void*);
    extern u8 pokemonIsDarkPokemon(void*);
    extern u8 pokemonBiosGetTamagoFlag(void*);
    extern u16 pokemonBiosGetItemDataId(void*);
    extern u8 fn_80142984(void);
    extern void* wazaDataBiosGetPtr(u16);
    extern u8 pokemonCheckValid(void*);
    extern void __assert(const char*, s32, const char*);
    u8 copy[0x130];
    u32 total;
    s32 i;
    u16 move;
    const char* text = (const char*)lbl_80268940;

    if (menuRuleSlotIsEmpty(pokemon)) {
        return 1;
    }

    switch (check) {
    case 0:
        if (fn_80076398(pokemon, 2) == 0) {
            return 1;
        }
        if (menuRuleEventPokemonAllowed(pokemon) == 0) {
            return 0;
        }

        pokemonBiosCopy(copy, pokemon);
        pokemonResetBasisStatus(copy);
        if (pokemonBiosGetMaxHp(copy) < pokemonBiosGetMaxHp(pokemon) ||
            pokemonBiosGetPhyAtk(copy) < pokemonBiosGetPhyAtk(pokemon) ||
            pokemonBiosGetPhyDef(copy) < pokemonBiosGetPhyDef(pokemon) ||
            pokemonBiosGetSpeAtk(copy) < pokemonBiosGetSpeAtk(pokemon) ||
            pokemonBiosGetSpeDef(copy) < pokemonBiosGetSpeDef(pokemon) ||
            pokemonBiosGetNimbleness(copy) < pokemonBiosGetNimbleness(pokemon)) {
            return 0;
        }

        total = pokemonBiosGetMaxHpEffort(pokemon);
        total += pokemonBiosGetPhyAtkEffort(pokemon);
        total += pokemonBiosGetPhyDefEffort(pokemon);
        total += pokemonBiosGetSpeAtkEffort(pokemon);
        total += pokemonBiosGetSpeDefEffort(pokemon);
        total += pokemonBiosGetNimblenessEffort(pokemon);
        if (total > 0x1FE) {
            return 0;
        }
        if (pokemonBiosGetMaxHpEffort(pokemon) > 0xFF ||
            pokemonBiosGetPhyAtkEffort(pokemon) > 0xFF ||
            pokemonBiosGetPhyDefEffort(pokemon) > 0xFF ||
            pokemonBiosGetSpeAtkEffort(pokemon) > 0xFF ||
            pokemonBiosGetSpeDefEffort(pokemon) > 0xFF ||
            pokemonBiosGetNimblenessEffort(pokemon) > 0xFF) {
            return 0;
        }
        return 1;

    case 1:
        return pokemonIsDarkPokemon(pokemon) == 0;

    case 2:
        return pokemonBiosGetTamagoFlag(pokemon) == 0;

    case 3:
        switch (pokemonBiosGetItemDataId(pokemon)) {
        case 0:
            return 1;
        case 0xAF:
            return 0;
        default:
            return fn_80142984();
        }

    case 4:
        for (i = 0; i < 4; i++) {
            move = pokemonGetStatus(pokemon, 0, 0x7F, i);
            if (move != 0 &&
                wazaDataBiosGetPtr(move) == wazaDataBiosGetPtr(0)) {
                return 0;
            }
        }
        return 1;

    case 5:
        if ((u16)pokemonGetStatus(pokemon, 0, 0x6E, 0) == 0) {
            __assert(text + 0x108, 0x25E, text + 0x14C);
        }
        return pokemonCheckValid(pokemon);

    default:
        __assert(text + 0x108, 0x274, text + 0x118);
        return 0;
    }
}

/* Check every whole-party rule. */
static inline u8 menuRuleCheckPartyAllModes(void* hero, const u8* rule)
{
    extern u8 fn_80076F2C(void* hero, const u8* rule, s32 mode);
    s32 mode;

    for (mode = 0; mode < 4; mode++) {
        if (fn_80076F2C(hero, rule, mode) == 0) {
            return 0;
        }
    }
    return 1;
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

/* Validate every party member against the active battle rule. */
u8 fn_800767B8(void* hero, const u8* rule)
{
    extern void* heroBiosGetPokemonPtr(void* hero, u16 slot);
    extern u8 pokemonCheckValid(void* pokemon);
    s32 mode;
    s32 slot;
    s32 partyCount;
    void* pokemon;
    u8 valid;

    partyCount = 0;
    if (menuRuleCheckPartyAllModes(hero, rule) == 0) {
        return 0;
    }

    for (slot = 0; slot < 6; slot++) {
        pokemon = heroBiosGetPokemonPtr(hero, (u16)slot);
        if (pokemon != 0 && pokemonCheckValid(pokemon) != 0) {
            for (mode = 0; mode < 3; mode++) {
                if (menuRuleCheckPokemonMode(pokemon, (const s16*)rule,
                                             mode) == 0) {
                    valid = 0;
                    goto pokemon_checked;
                }
            }
            valid = 1;
pokemon_checked:
            if (valid == 0) {
                return 0;
            }
            partyCount++;
        }
    }
    return partyCount > 0;
}
