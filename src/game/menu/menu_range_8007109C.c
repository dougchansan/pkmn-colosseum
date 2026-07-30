/**
 * @file menu_range_8007109C.c
 * @brief menu (GBA-link/comm UI), 0x8007109C - 0x8007C260.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) -- mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 */
#include "dolphin/types.h"
#include "game/hero.h"
#include "game/win_sprite.h"

typedef struct MenuDVDFileInfo {
    u8 pad[0x34];
    u32 length;
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
        void* model_ = GSresGetResource(fn_80113F48(), (id_));      \
        if (model_ != 0) {                                         \
            GSmodelSetVisibility(model_, (visible_));              \
        }                                                          \
    } while (0)

#define RESTORE_EXDISC_SCENE(handle_)                              \
    do {                                                           \
        fadeSet(3, lbl_8047C108);                                  \
        fadeCheck(1);                                              \
        fn_801CB9D8((u32)(handle_));                               \
        SET_EXDISC_VISIBILITY(0x104E1000, 1);                      \
        SET_EXDISC_VISIBILITY(0x104E1001, 1);                      \
        SET_EXDISC_VISIBILITY(0x104E1002, 1);                      \
        cameraPlayAnime((s32)fn_80113F48(), 0x10941800, 0, 0);    \
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
extern f32 lbl_8047C0E0;
extern f32 lbl_8047C0E4;
extern f32 lbl_8047C100;
extern f32 lbl_8047C108;
extern u8 lbl_8047C10C;
extern u32 OSGetTick(void);
extern void _threadSwitch(void);
extern void winMsgOpenField(s32, s32, s32);
extern void winMsgOpen(s32, s32, s32, s32);
extern void winMsgClose(s32);
extern u32 GBAReset(s32, u32);
extern void OSCreateAlarm(void*);
extern u32 OSDisableInterrupts(void);
extern void OSSetAlarm(void*, s64, void*);
extern s32 OSSuspendThread(void*);
extern void OSRestoreInterrupts(u32);
extern void fn_8007C23C(u8*);
extern u32 lbl_803FAEF8[256];
extern void* heroBiosGetNamePtr(void*);
extern void* heroBiosGetPokemonPtr(void*, u16);
extern void* fn_80113F48(void);
extern void* GSresGetResource(void*, u32);
extern void GSmodelSetVisibility(void*, u32);
extern void* fn_801CBA0C(u32);
extern void cameraPlayAnime(s32, u32, s32, s32);
extern void fadeSet(s32, f32);
extern s32 fadeCheck(s32);
extern void fn_801CB9D8(u32);
extern void GSscene_SetMode(s32);
extern u8 fn_80079EF4(s32, u32);
extern void gbaCommandSetKeyState(s32 mode, s32 flag);
extern s32 fn_80073C38(s32 chan);
extern s32 GBAWrite();
extern s32 GBARead();
extern s32 GBAGetStatus();

typedef struct GbaIdleCallback {
    void (*func)(s32 chan, void* arg);
    void* arg;
} GbaIdleCallback;

extern GbaIdleCallback lbl_803B6E18[5];
extern s32 lbl_803B6E08[4];
extern u8 lbl_803B6D88[0x58];
extern void floorLink(s32, s32);

u8 fn_80076398(void* pokemon, s32 check)
{
    extern u32 pokemonGetStatus(void*, s32, s32, s32);
    extern u16 pokemonBiosGetPokemonDataId(void*);
    extern u8 pokemonBiosGetEventGetFlag(void*);
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
    u16 move;
    u16 base_stat;
    s32 i;

    if (pokemon != NULL && pokemonGetStatus(pokemon, 0, 0x6E, 0) == 0) {
        return 1;
    }

    switch (check) {
    case 0:
        if (fn_80076398(pokemon, 2) == 0) {
            return 1;
        }
        if ((pokemonBiosGetPokemonDataId(pokemon) == 0x97 ||
             pokemonBiosGetPokemonDataId(pokemon) == 0x19A) &&
            pokemonBiosGetEventGetFlag(pokemon) == 0) {
            return 0;
        }

        pokemonBiosCopy(copy, pokemon);
        pokemonResetBasisStatus(copy);
        base_stat = pokemonBiosGetMaxHp(pokemon);
        if (pokemonBiosGetMaxHp(copy) < base_stat) {
            return 0;
        }
        base_stat = pokemonBiosGetPhyAtk(pokemon);
        if (pokemonBiosGetPhyAtk(copy) < base_stat) {
            return 0;
        }
        base_stat = pokemonBiosGetPhyDef(pokemon);
        if (pokemonBiosGetPhyDef(copy) < base_stat) {
            return 0;
        }
        base_stat = pokemonBiosGetSpeAtk(pokemon);
        if (pokemonBiosGetSpeAtk(copy) < base_stat) {
            return 0;
        }
        base_stat = pokemonBiosGetSpeDef(pokemon);
        if (pokemonBiosGetSpeDef(copy) < base_stat) {
            return 0;
        }
        base_stat = pokemonBiosGetNimbleness(pokemon);
        if (pokemonBiosGetNimbleness(copy) < base_stat) {
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
        move = pokemonBiosGetItemDataId(pokemon);
        if (move == 0) {
            return 1;
        }
        if (move == 0xAF) {
            return 0;
        }
        return fn_80142984();

    case 4:
        for (i = 0; i < 4; i++) {
            move = pokemonGetStatus(pokemon, 0, 0x7F, (u16)i);
            if (move != 0 &&
                wazaDataBiosGetPtr(move) == wazaDataBiosGetPtr(0)) {
                return 0;
            }
        }
        return 1;

    case 5:
        if (pokemonGetStatus(pokemon, 0, 0x6E, 0) == 0) {
            __assert((const char*)lbl_80268940 + 0x108, 0x25E,
                     (const char*)lbl_80268940 + 0x14C);
        }
        return pokemonCheckValid(pokemon);

    default:
        __assert((const char*)lbl_80268940 + 0x108, 0x274,
                 (const char*)lbl_80268940 + 0x118);
        return 0;
    }
}


/*
 * Small helpers ported from the previous campaign's archive
 * (archive/previous_campaign/src/game/menu/menu_common_ext.c). Each is
 * a trivial, self-contained accessor/no-op operating on the GBA-link
 * FightSeq-style call stack at lbl_803B6D88; re-verified against this
 * unit's own compiler flags rather than copied wholesale.
 */

/* fn_8007162C (0x8007162C): peek the current call-stack depth slot. */
s32 fn_8007162C(void) {
    u32 depth;

    depth = *(u32*)(lbl_803B6D88 + 0x40);
    return *(s32*)(lbl_803B6D88 + depth * 8);
}

/* fn_800716C8 (0x800716C8): register a per-channel idle callback
 * (archive's "return 0" stub was incomplete -- disassembly shows it
 * actually populates lbl_803B6E18[chan]). */
s32 fn_800716C8(s32 chan, void* arg, void (*func)(s32 chan, void* arg)) {
    lbl_803B6E18[chan].func = func;
    lbl_803B6E18[chan].arg = arg;
    return 0;
}

/* fn_800716E8 (0x800716E8): store a per-channel value, always returns 0. */
s32 fn_800716E8(s32 channel, s32 value) {
    lbl_803B6E08[channel] = value;
    return 0;
}

/* fn_8007169C (0x8007169C): fixed diagnostic-log call, always returns 0. */
#pragma push
#pragma peephole off
s32 fn_8007169C(void) {
    floorLink(0x385, 0);
    return 0;
}
#pragma pop

/*
 * menuCB_Common.c block (0x8007109C-0x80071698): menu call-stack at
 * lbl_803B6D88 ({id, flag} pairs + depth word at +0x40) plus two heap
 * handle helpers. Assert strings: lbl_80268708 = "menuCB_Common.c",
 * lbl_80268718 = _menuPop underflow, lbl_80268750 = _menuPush overflow,
 * lbl_8047C090 = "handle".
 */
extern void __assert(const char* file, u32 line, const char* msg);
extern const u8 lbl_80268708[];
extern const u8 lbl_80268718[];
extern const u8 lbl_80268750[];
extern char lbl_8047C090;

/* fn_8007109C (0x8007109C): release the current heap handle, if any. */
#pragma push
#pragma peephole off
void fn_8007109C(void) {
    extern u16 fn_800E202C(void);
    extern void fn_800E24B0(u16 handle);
    extern void fn_800E209C(u16 handle);
    u16 handle;

    handle = fn_800E202C();
    if (!(handle != 0)) {
        __assert((const char*)lbl_80268708, 0xDE, (const char*)&lbl_8047C090);
    }
    if (handle != 0) {
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }
}
#pragma pop

/* fn_80071104 (0x80071104): allocate a 32-byte-aligned handle and lock it. */
#pragma push
#pragma peephole off
s32 fn_80071104(s32 size) {
    extern u16 fn_800E2C04(s32 size, s32 align);
    extern s32 fn_800E27B0(u16 handle);
    u16 handle;

    handle = fn_800E2C04((size + 0x1F) & ~0x1F, 0x20);
    if (handle != 0) {
        return fn_800E27B0(handle);
    }
    if (!(handle != 0)) {
        __assert((const char*)lbl_80268708, 0xD5, (const char*)&lbl_8047C090);
    }
    return 0;
}
#pragma pop

/* fn_80071160 (0x80071160): scan the 4 save slots for a live, valid entry. */
#pragma push
#pragma peephole off
s32 fn_80071160(void) {
    extern u8* savedataGetStatus(s32 side, s32 slotType);
    extern s32 fn_8006A7E8(u8* p);
    extern u8 fn_8008ABA0(s32 v);
    s32 off;
    u32 i;
    s32 v;

    for (i = 0, off = 0; i < 4; off += 0x1660, i++) {
        v = *(s32*)(savedataGetStatus(0, 0xE) + off + 0x59CC);
        if (v != 0) {
            if (fn_8006A7E8(savedataGetStatus(0, 0xE) + off + 0x59A8) != 0) {
                if (fn_8008ABA0(v) == 0) {
                    return v;
                }
            }
        }
    }
    return 0;
}
#pragma pop

/* fn_80071344 (0x80071344): reopen the menu on top of the call stack. */
#pragma push
#pragma peephole off
void fn_80071344(void) {
    extern s32 menuOpenCustom(s32 slot, ...);
    u32 depth;

    depth = *(u32*)(lbl_803B6D88 + 0x40);
    menuOpenCustom(*(s32*)(lbl_803B6D88 + depth * 8), 0,
                   lbl_803B6D88 + depth * 8 + 4, 0x10, 1, 0, 0);
}
#pragma pop

/* _menuPop (0x800714C8): close the top menu and pop the call stack. */
#pragma push
#pragma peephole off
s32 _menuPop(void) {
    extern s32 windowGetActiveID(void);
    extern u32 windowSearchID(s32 id);
    extern void menuCloseCustom(s32 slot, s32 p1, s32 p2);
    s32 top;
    s32 active;
    u32 d;

    top = *(s32*)(lbl_803B6D88 + *(u32*)(lbl_803B6D88 + 0x40) * 8);
    active = windowGetActiveID();
    if (active == top) {
        menuCloseCustom(*(s32*)(lbl_803B6D88 + *(u32*)(lbl_803B6D88 + 0x40) * 8), 0, 0);
    }
    if (windowSearchID(0xBE) != 0) {
        menuCloseCustom(0xBE, 0, 1);
    }
    *(s32*)(lbl_803B6D88 + *(u32*)(lbl_803B6D88 + 0x40) * 8 + 4) = 0;
    if (*(s32*)(lbl_803B6D88 + 0x40) == 0) {
        return -1;
    }
    if (!(0 < *(s32*)(lbl_803B6D88 + 0x40))) {
        __assert((const char*)lbl_80268708, 0x5C, (const char*)lbl_80268718);
    }
    d = *(u32*)(lbl_803B6D88 + 0x40) - 1;
    *(u32*)(lbl_803B6D88 + 0x40) = d;
    return *(s32*)(lbl_803B6D88 + d * 8);
}
#pragma pop

/* _menuPush (0x800715BC): push a menu id onto the call stack. */
#pragma push
#pragma peephole off
void _menuPush(s32 id) {
    u32 depth;

    depth = *(u32*)(lbl_803B6D88 + 0x40);
    if (depth >= 8) {
        __assert((const char*)lbl_80268708, 0x41, (const char*)lbl_80268750);
    } else {
        *(u32*)(lbl_803B6D88 + 0x40) = depth + 1;
        *(s32*)(lbl_803B6D88 + (depth + 1) * 8) = id;
        *(s32*)(lbl_803B6D88 + *(u32*)(lbl_803B6D88 + 0x40) * 8 + 4) = 0;
    }
}
#pragma pop

/* menuCB_InitMenu (0x80071644): clear the call stack and seed slot 0. */
#pragma push
#pragma peephole off
void menuCB_InitMenu(s32 id) {
    u32 i;

    for (i = 0; i < 8; i++) {
        *(u32*)(lbl_803B6D88 + i * 8) = 0;
        *(u32*)(lbl_803B6D88 + i * 8 + 4) = 0;
    }
    *(s32*)(lbl_803B6D88 + 0x0) = id;
    *(u32*)(lbl_803B6D88 + 0x40) = 0;
}
#pragma pop

static inline s32 fn_80071AE4_poll_read(s32 chan, u32* response, u8* length,
                                        u8* status) {
    extern s32 GBARead(s32 chan, void* destination, u8* status);
    extern s32 GBAGetStatus(s32 chan, u8* status);
    u32 timeout;
    u32 start;

    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return 1;
        }
        if (GBAGetStatus(chan, status) != 0) {
            return 2;
        }
        if ((*status & 0xA) == 8) {
            break;
        }
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            return 0x3E8;
        }
    }
    if (GBARead(chan, response, length) != 0) {
        return 3;
    }
    return 0;
}

static inline u32 fn_80071AE4_swap_word(u32 value) {
    u32 result;

    result = value >> 24;
    result |= (value >> 8) & 0x0000FF00;
    result |= (value << 8) & 0x00FF0000;
    result |= value << 24;
    return result;
}

s32 fn_80073A44(s32 chan, u16* buttons)
{
    u32 setupTimeout;
    u32 setupStart;
    u32 response;
    u32 command;
    u8 length;
    u8 status;
    s32 result;
    s32 setupExpired;

    setupTimeout = OSMillisecondsToTicks(5);
    setupStart = OSGetTick();
    setupExpired = OSGetTick() - setupStart > setupTimeout;
    result = fn_80073C38(chan);
    if (result == 1) {
        if (setupExpired) {
            return 1;
        }
    } else if (result != 0) {
        return result;
    }

    command = 0xAA;
    if (GBAWrite(chan, &command, &length) != 0) {
        return 0xB;
    }

    result = fn_80071AE4_poll_read(chan, &response, &length, &status);
    if (result != 0) {
        return result + 0xB;
    }
    if ((response >> 24) != 0xAA) {
        return 0xF;
    }

    *buttons = fn_80071AE4_swap_word(response) >> 16;
    return 0;
}

s32 fn_80073C38(s32 chan)
{
    extern u32 fn_800D0F44(s32 chan);
    extern u32 lbl_8047A60C;
    u32 timeout;
    u32 start;
    u32 response;
    u8 length;
    u8 status;
    s32 result;

    if (fn_800D0F44(chan) != 0x40000) {
        return 1;
    }
    if (GBAReset(chan, (u32)&length) != 0) {
        return 2;
    }

    timeout = OSMillisecondsToTicks(5);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            result = 1;
            break;
        }
        if (GBAGetStatus(chan, &status) != 0) {
            result = 2;
            break;
        }
        if ((status & 0xA) == 8) {
            if (GBARead(chan, &response, &length) != 0) {
                result = 3;
            } else {
                result = 0;
            }
            break;
        }
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            result = 0x3E8;
            break;
        }
    }
    if (result != 0) {
        return result + 2;
    }
    if (response != lbl_8047A60C) {
        return 6;
    }
    if (GBAGetStatus(chan, &length) != 0) {
        return 7;
    }
    if (GBAWrite(chan, &response, &length) != 0) {
        return 8;
    }

    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return 9;
        }
        if (GBAGetStatus(chan, &length) != 0) {
            return 10;
        }
        if ((length & 2) == 0) {
            return 0;
        }
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            return 0x3E8;
        }
    }
}

s32 fn_80072A00(s32 chan)
{
    u32 command;
    u32 response;
    u32 outerTimeout;
    u32 outerStart;
    u32 setupTimeout;
    u32 setupStart;
    u8 length;
    u8 status;
    s32 result;
    s32 innerResult;
    s32 setupExpired;

    gbaCommandSetKeyState(chan + 1, 2);
    result = fn_80073C38(chan);
    if (result != 0) {
        goto done;
    }

    command = 0x60;
    if (GBAWrite(chan, &command, &length) != 0) {
        result = 0xB;
        goto done;
    }

    outerTimeout = OS_TIMER_CLOCK * 3;
    outerStart = OSGetTick();
    do {
        if (OSGetTick() - outerStart > outerTimeout) {
            result = 0x10;
            break;
        }

        setupTimeout = OSMillisecondsToTicks(5);
        setupStart = OSGetTick();
        setupExpired = OSGetTick() - setupStart > setupTimeout;
        innerResult = fn_80073C38(chan);
        if (innerResult == 1) {
            if (!setupExpired) {
                innerResult = 0;
            }
        }

        if (innerResult == 0) {
            command = 0xAA;
            if (GBAWrite(chan, &command, &length) != 0) {
                innerResult = 0xB;
            } else {
                innerResult =
                    fn_80071AE4_poll_read(chan, &response, &length, &status);
                if (innerResult != 0) {
                    innerResult += 0xB;
                } else if ((response >> 24) != 0xAA) {
                    innerResult = 0xF;
                }
            }
        }
        _threadSwitch();
    } while (innerResult != 0);

done:
    gbaCommandSetKeyState(chan + 1, 1);
    return result;
}

s32 fn_800730F8(s32 chan, u32 value)
{
    u32 timeout;
    u32 start;
    u32 command;
    u32 response;
    u32 sendValue;
    u8 length;
    u8 status;
    s32 result;
    s32 expired;

    gbaCommandSetKeyState(chan + 1, 2);
    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    sendValue = fn_80071AE4_swap_word(value);

    do {
        expired = OSGetTick() - start > timeout;
        result = fn_80073C38(chan);
        if (result == 0) {
            command = 0x77;
            if (GBAWrite(chan, &command, &length) != 0) {
                result = 0xB;
            } else {
                result =
                    fn_80071AE4_poll_read(chan, &response, &length, &status);
                if (result != 0) {
                    result += 0xB;
                } else if ((response >> 24) != 0x77) {
                    result = 0xF;
                }
            }

            if (result == 0) {
                GBAWrite(chan, &sendValue, &status);
                timeout = OSMillisecondsToTicks(100);
                start = OSGetTick();
                for (;;) {
                    if (OSGetTick() - start > timeout) {
                        result = 0x10;
                        break;
                    }
                    if (GBAGetStatus(chan, &status) != 0) {
                        result = 0x11;
                        break;
                    }
                    if ((status & 2) == 0) {
                        result = 0;
                        break;
                    }
                    if (lbl_803B6E18[chan].func != NULL) {
                        lbl_803B6E18[chan].func(
                            chan, lbl_803B6E18[chan].arg);
                    }
                    if (lbl_803B6E08[chan] != 0) {
                        result = 0x3E8;
                        break;
                    }
                }
            }
        }
    } while (result == 1 && !expired);

    gbaCommandSetKeyState(chan + 1, result != 0 ? 1 : 3);
    return result;
}

s32 fn_800733D0(s32 chan, const u32* data)
{
    u32 setupTimeout;
    u32 setupStart;
    u32 timeout;
    u32 start;
    u32 command;
    u32 response;
    u32 word;
    s32 offset;
    u8 length;
    u8 status;
    s32 result;
    s32 setupExpired;

    gbaCommandSetKeyState(chan + 1, 2);
    setupTimeout = OSMillisecondsToTicks(5);
    setupStart = OSGetTick();
    setupExpired = OSGetTick() - setupStart > setupTimeout;
    result = fn_80073C38(chan);
    if (result == 1) {
        if (!setupExpired) {
            result = 0;
        }
    }
    if (result != 0) {
        goto done;
    }

    command = 0x88;
    if (GBAWrite(chan, &command, &length) != 0) {
        result = 0xB;
        goto done;
    }
    result = fn_80071AE4_poll_read(chan, &response, &length, &status);
    if (result != 0) {
        result += 0xB;
        goto done;
    }
    if ((response >> 24) != 0x88) {
        result = 0xF;
        goto done;
    }

    for (offset = 0; offset < 0x780; offset += 4, data++) {
        word = *data;
        if (GBAWrite(chan, &word, &status) != 0) {
            result = 0x10;
            goto done;
        }

        timeout = OSMillisecondsToTicks(100);
        start = OSGetTick();
        for (;;) {
            if (OSGetTick() - start > timeout) {
                result = 0x11;
                goto done;
            }
            if (GBAGetStatus(chan, &status) != 0) {
                result = 0x12;
                goto done;
            }
            if ((status & 2) == 0) {
                break;
            }
        }
        if ((offset & 0x3F) == 0) {
            _threadSwitch();
        }
    }
    result = 0;

done:
    gbaCommandSetKeyState(chan + 1, 1);
    return result;
}

s32 fn_80073700(s32 chan, u32* data)
{
    u32 command;
    u32 response;
    u32 timeout;
    u32 start;
    s32 offset;
    u8 length;
    u8 status;
    s32 result;

    result = fn_80073C38(chan);
    if (result != 0) {
        return result;
    }

    command = 0x99;
    if (GBAWrite(chan, &command, &length) != 0) {
        return 0xB;
    }

    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            result = 1;
            break;
        }
        if (GBAGetStatus(chan, &status) != 0) {
            result = 2;
            break;
        }
        if ((status & 0xA) == 8) {
            if (GBARead(chan, &response, &length) != 0) {
                result = 3;
            } else {
                result = 0;
            }
            break;
        }
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            result = 0x3E8;
            break;
        }
    }
    if (result != 0) {
        return result + 0xB;
    }
    if ((response >> 24) != 0x99) {
        return 0xF;
    }

    for (offset = 0; offset < 0x278; offset += 4, data++) {
        timeout = OSMillisecondsToTicks(100);
        start = OSGetTick();
        for (;;) {
            if (OSGetTick() - start > timeout) {
                return 0x10;
            }
            if (GBAGetStatus(chan, &status) != 0) {
                return 0x11;
            }
            if ((status & 8) != 0) {
                break;
            }
            if (lbl_803B6E18[chan].func != NULL) {
                lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
            }
            if (lbl_803B6E08[chan] != 0) {
                return 0x3E8;
            }
        }
        if (GBARead(chan, &response, &status) != 0) {
            return 0x12;
        }
        *data = response;
        if (lbl_803B6E08[chan] != 0) {
            return 0x3E8;
        }
    }
    return 0;
}

/* Send a variable-sized data block to one GBA channel. */
s32 fn_80071AE4(s32 chan, const u32* data) {
    u32 response;
    u32 command;
    u32 send_word;
    u8 length;
    u8 send_status;
    u8 status;
    s32 key_channel;
    s32 result;
    u32 outer_timeout;
    u32 outer_start;
    u32 timeout;
    u32 start;
    s32 offset;
    s32 total_size;
    s32 outer_expired;
    const u32* data_ptr;
    u32 packed;
    u16 extra_size;
    u16 word_count;

    key_channel = chan + 1;
    gbaCommandSetKeyState(key_channel, 2);
    outer_timeout = OSMillisecondsToTicks(100);
    outer_start = OSGetTick();
    packed = fn_80071AE4_swap_word(data[3]);
    extra_size = packed >> 16;
    word_count = packed;

    do {
        outer_expired = OSGetTick() - outer_start > outer_timeout;

        {
            s32 setup_result;

            setup_result = fn_80073C38(chan);
            if (setup_result != 0) {
                result = setup_result;
                goto transfer_done;
            }
        }

        command = 0x22;
        if (GBAWrite(chan, &command, &length) != 0) {
            result = 0xB;
            goto header_done;
        }

        {
            s32 poll_result;

            poll_result = fn_80071AE4_poll_read(chan, &response, &length,
                                                &status);
            result = poll_result != 0 ? poll_result + 0xB : 0;
        }

header_done:
        switch (result) {
        case 0:
            if ((response >> 24) != 0x22) {
                result = 0xF;
                goto transfer_done;
            }

            total_size = word_count;
            total_size = total_size * 4 + 0x10;
            if (extra_size != 0) {
                total_size += 0x70;
            }

            data_ptr = data;
            for (offset = 0; offset < total_size;
                 offset += 4, data_ptr++) {
                send_word = *data_ptr;
                if (GBAWrite(chan, &send_word, &send_status) != 0) {
                    result = 0x10;
                    goto data_done;
                }

                timeout = OSMillisecondsToTicks(100);
                start = OSGetTick();
                for (;;) {
                    if (OSGetTick() - start > timeout) {
                        result = 0x11;
                        goto data_done;
                    }
                    if (GBAGetStatus(chan, &send_status) != 0) {
                        result = 0x12;
                        goto data_done;
                    }
                    if ((send_status & 2) == 0) {
                        break;
                    }
                    if (lbl_803B6E18[chan].func != NULL) {
                        lbl_803B6E18[chan].func(
                            chan, lbl_803B6E18[chan].arg);
                    }
                    if (lbl_803B6E08[chan] == 0) {
                        continue;
                    }
                    result = 0x3E8;
                    goto data_done;
                }
                if (lbl_803B6E08[chan] != 0) {
                    result = 0x3E8;
                    goto data_done;
                }
            }
            result = 0;

data_done:
            switch (result) {
            case 0:
                result = 0;
                break;
            default:
                goto transfer_done;
            }
            break;
        default:
            goto transfer_done;
        }

transfer_done:
        ;
    } while (result == 1 && outer_expired == 0);

    gbaCommandSetKeyState(key_channel, result != 0 ? 1 : 3);
    return result;
}

#pragma push
#pragma peephole off
s32 fn_80071E34(s32 chan, void* data) {
    extern s32 fn_80071EA4(s32 chan, u32* data);
    s32 keyChannel;
    s32 result;

    keyChannel = chan + 1;
    gbaCommandSetKeyState(keyChannel, 2);
    result = fn_80071EA4(chan, data);
    gbaCommandSetKeyState(keyChannel, 1);
    return result;
}
#pragma pop

s32 fn_80071EA4(s32 chan, u32* data)
{
    u32 command;
    u32 response;
    u8 length;
    u8 status;
    s32 result;
    s32 offset;
    s32 size;

    result = fn_80073C38(chan);
    if (result != 0) {
        return result;
    }

    command = 0x33;
    if (GBAWrite(chan, &command, &length) != 0) {
        return 0xB;
    }
    result = fn_80071AE4_poll_read(chan, &response, &length, &status);
    if (result != 0) {
        return result + 0xB;
    }
    if ((response >> 24) != 0x33) {
        return 0xF;
    }

    for (offset = 0; offset < 0x10; offset += 4) {
        result = fn_80071AE4_poll_read(chan, data, &length, &status);
        if (result != 0) {
            return result + 0xF;
        }
        data++;
    }

    size = (fn_80071AE4_swap_word(data[-1]) & 0xFFFF) * 4;
    for (offset = 0; offset < size; offset += 4) {
        result = fn_80071AE4_poll_read(chan, data, &length, &status);
        if (result != 0) {
            return result + 0x12;
        }
        data++;
    }
    return 0;
}

/* fn_80072C74 (0x80072C74): poll one GBA channel and read its response. */
#pragma push
#pragma peephole off
s32 fn_80072C74(s32 chan, u32* response) {
    extern u32 fn_800D0F44(s32 chan);
    extern s32 GBAGetStatus(s32 chan, void* status);
    extern s32 GBAWrite(s32 chan, void* source, void* status);
    extern s32 GBARead(s32 chan, void* destination, void* status);
    u8 readBuf[4];
    u8 writeBuf[4];
    u8 status[4];
    s32 result;

    if (fn_800D0F44(chan) != 0x40000) {
        result = 1;
    } else if (GBAGetStatus(chan, status) != 0) {
        result = 2;
    } else if ((status[0] & 8) == 0) {
        *(u32*)writeBuf = 0x11;
        GBAWrite(chan, writeBuf, status);
        result = -1;
    } else if (GBARead(chan, readBuf, status) != 0) {
        result = 3;
    } else {
        result = 0;
        *response = *(u32*)readBuf;
    }
    if (result >= 0) {
        gbaCommandSetKeyState(chan + 1, 1);
    }
    return result;
}
#pragma pop

/* _AGB_EntryGetStatus (0x80073034): poll one GBA channel and read its status. */
#pragma push
#pragma peephole off
s32 _AGB_EntryGetStatus__FlPUl(s32 chan, u32* response) {
    extern u32 fn_800D0F44(s32 chan);
    extern s32 GBAGetStatus(s32 chan, void* status);
    extern s32 GBAWrite(s32 chan, void* source, void* status);
    extern s32 GBARead(s32 chan, void* destination, void* status);
    u8 readBuf[4];
    u8 writeBuf[4];
    u8 status[4];

    if (fn_800D0F44(chan) != 0x40000) {
        return 1;
    }
    if (GBAGetStatus(chan, status) != 0) {
        return 2;
    }
    if ((status[0] & 8) == 0) {
        *(u32*)writeBuf = 0x11;
        GBAWrite(chan, writeBuf, status);
        return -1;
    }
    if (GBARead(chan, readBuf, status) != 0) {
        return 3;
    }
    *response = *(u32*)readBuf;
    return 0;
}
#pragma pop

/* fn_80073690 (0x80073690): bracket a channel operation with key-state updates. */
#pragma push
#pragma peephole off
s32 fn_80073690(s32 chan, s32 data) {
    extern s32 fn_80073700(s32 chan, s32 data);
    s32 keyChannel;
    s32 result;

    keyChannel = chan + 1;
    gbaCommandSetKeyState(keyChannel, 2);
    result = fn_80073700(chan, data);
    gbaCommandSetKeyState(keyChannel, 1);
    return result;
}
#pragma pop

/* fn_80073990 (0x80073990): wait one millisecond, then send command 0x11. */
#pragma push
#pragma peephole off
s32 fn_80073990(s32 chan) {
    extern u32 fn_800D0F44(s32 chan);
    u32 command;
    u32 length;
    u32 delay;
    u32 start;
    s32 result;

    delay = OSMillisecondsToTicks(1);
    start = OSGetTick();
    while (OSGetTick() - start < delay) {
    }
    if (fn_800D0F44(chan) != 0x40000) {
        result = 1;
    } else {
        command = 0x11;
        if (GBAWrite(chan, (u32)&command, (u32)&length) != 0) {
            result = 2;
        } else {
            result = 0;
        }
    }
    return result;
}
#pragma pop

/*
 * Trivial single-call tail-wrappers ported from the previous campaign's
 * archive (archive/previous_campaign/src/game/menu/menu_tool2.c);
 * re-verified against this unit's own compiler flags.
 */
extern s32 fn_80190528(s32);
extern s32 fn_801902E0(s32);
extern s32 menuClose(s32);
extern s32 fn_801906A0(s32);
extern void _flagSet();

/* fn_80074324 (0x80074324): prep key-state then perform per-channel reset path. */
#pragma peephole off
s32 fn_80074360(s32 chan)
{
    extern u32 fn_800D0F44(s32 chan);
    u32 response;
    u32 timeout;
    u32 start;
    u8 status;
    s32 result;

    if (fn_800D0F44(chan) != 0x40000) {
        return 1;
    }
    if (GBAReset(chan, (u32)&status) != 0) {
        return 2;
    }

    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return 3;
        }
        if (GBAGetStatus(chan, &status) != 0) {
            return 4;
        }
        if (status == 0x18) {
            break;
        }
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            return 0x3E8;
        }
    }

    if (GBARead(chan, &response, &status) != 0) {
        return 5;
    }
    if (response != 0x41585645) {
        return 6;
    }
    if (GBAGetStatus(chan, &status) != 0 && status != 0x10) {
        return 7;
    }
    if (GBAWrite(chan, &response, &status) != 0) {
        return 8;
    }

    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return 9;
        }
        if (GBAGetStatus(chan, &status) != 0) {
            return 0xA;
        }
        if ((status & 0x30) != 0x10) {
            return 0xB;
        }
        if ((status & 2) == 0) {
            break;
        }
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            return 0x3E8;
        }
    }
    result = 0;
    return result;
}
#pragma peephole reset

extern s32 fn_8007480C();

s32 fn_800745B4(s32 chan)
{
    u32 command;
    u32 response;
    u32 timeout;
    u32 start;
    u8 length;
    u8 status;
    s32 result;
    s32 expired;

    result = fn_8007480C(chan);
    if (result != 0) {
        goto done;
    }

    timeout = OS_TIMER_CLOCK * 8;
    start = OSGetTick();
    while (OSGetTick() - start <= timeout) {
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            return 0x3E8;
        }
    }

    timeout = OSMillisecondsToTicks(5);
    start = OSGetTick();
    do {
        expired = OSGetTick() - start > timeout;
        result = fn_80073C38(chan);
    } while (result == 1 && !expired);
    if (result != 0) {
        goto done;
    }

    command = 0xAA;
    if (GBAWrite(chan, &command, &length) != 0) {
        result = 0xB;
        goto done;
    }
    result = fn_80071AE4_poll_read(chan, &response, &length, &status);
    if (result != 0) {
        result += 0xB;
        goto done;
    }
    if ((response >> 24) != 0xAA) {
        result = 0xF;
    }

done:
    gbaCommandSetKeyState(chan + 1, 1);
    return result;
}

#pragma push
#pragma peephole off
s32 fn_80074324(s32 arg0) {
    extern s32 fn_80074360(s32);

    gbaCommandSetKeyState(arg0 + 1, 0);
    return fn_80074360(arg0);
}
#pragma pop

void fn_8007581C(void)
{
    extern s32 _menuPop_80071398(s32);
    extern u8* savedataGetStatus(s32, s32);
    extern void fn_8005E750(void*);
    extern s32 fn_80063D14(void*);
    extern s32 menuCB_Battle(void*);
    extern s32 fn_80062948(void*);
    extern u8 fn_800FF52C(void);
    extern void fn_800FF660(void);
    extern void fn_800FF58C(s32);
    extern void floorSetFadeScript(s32, u32);
    extern char lbl_802688F8[];
    extern char lbl_8026890C[];
    extern void __assert(const char*, s32, const char*);
    u8* status;
    s32 running;
    s32 result;

    running = 1;
    result = 0;
    while (running != 0) {
        status = savedataGetStatus(0, 0xE);
        fn_8005E750(status);
        status = savedataGetStatus(0, 0xE);
        result = fn_80063D14(status);
        if (result == 0xB3) {
            _menuPop_80071398(result);
        } else {
            status = savedataGetStatus(0, 0xE);
            result = menuCB_Battle(status);
            *(s32*)(savedataGetStatus(0, 0xE) + 0x20) = result;
            status = savedataGetStatus(0, 0xE);
            result = fn_80062948(status);

            switch (result) {
            case 0xD1:
                status = savedataGetStatus(0, 0xE);
                if (*(s32*)status == 1) {
                    status = savedataGetStatus(0, 0xE);
                    if (*(s32*)(status + 0x20) == 2) {
                        running = 0;
                    }
                }
                break;
            case 0x105:
                _menuPush(0x105);
                running = 0;
                break;
            case 0xAC:
                status = savedataGetStatus(0, 0xE);
                if (*(s32*)status == 0) {
                    result = 0xAE;
                }
                _menuPop_80071398(result);
                running = 0;
                break;
            default:
                _menuPop_80071398(result);
                running = 0;
                break;
            }
        }
    }

    if (fn_800FF52C() != 0) {
        status = savedataGetStatus(0, 0xE);
        if (*(s32*)status != 1) {
            __assert(lbl_802688F8, 0xA7, lbl_8026890C);
        }
        fn_800FF660();
        status = savedataGetStatus(0, 0xE);
        if (*(s32*)status == 1 && result != 0xD1) {
            _flagSet(0x8AE, 0);
            floorSetFadeScript(0x5960009, 0);
            fadeCheck(1);
        }
    }
    fn_800FF58C(0x395);
}

/* fn_80075A34 (0x80075A34): load the battle scene and start its camera. */
void fn_80075A34(void) {
    extern u32 fn_80113F48(void);
    extern u32 fn_801CBA0C(u32 resourceId);
    extern void GSresGetResource(u32 archive, u32 resource);
    extern void cameraPlayAnime(s32 cameraId, u32 animationId, s32 frame, s32 loop);
    extern void GSscene_SetMode(u32 mode);
    extern u8 lbl_8047A5D0;
    u32 archive;

    archive = fn_80113F48();
    GSresGetResource(archive, *(u32*)&lbl_8047A5D0 = fn_801CBA0C(0x10801000));
    cameraPlayAnime(0x5E0, 0x10821800, 0, 1);
    GSscene_SetMode(4);
}

/* Check party-rule constraints for the selected Pokemon. */
#pragma peephole off
u8 fn_80076A8C(u32 hero, u32 pokemon, const u8* rule, s32 mode)
{
    extern u8 fn_80076F2C();
    extern u32 pokemonGetStatus();
    extern u8 pokemonBiosGetTamagoFlag();
    extern u8 pokemonCheckValid();
    extern u32 heroBiosGetPokemonPtr();
    extern u16 pokemonBiosGetPokemonDataId();
    extern u16 pokemonBiosGetItemDataId();
    u32 candidate;
    s32 present;
    s32 invalid;
    u32 i;

    switch (mode) {
    case 0:
        return fn_80076F2C(hero, rule, mode);

    case 1:
        if (rule[0xC] != 0) {
            return 1;
        }

        present = pokemon == 0;
        invalid = 0;
        if (present == 0) {
            if (pokemonGetStatus(pokemon, 0, 0x6E, 0) != 0) {
                goto selected_present_species;
            }
        }
        invalid = 1;
selected_present_species:
        if (invalid != 0) {
            return 1;
        }

        present = 0;
        if (present == 0) {
            if (pokemonGetStatus(pokemon, 0, 0x6E, 0) != 0) {
                goto selected_species_valid;
            }
        }
        present = 1;
selected_species_valid:
        if (present != 0) {
            invalid = 0;
        } else {
            present = 0;
            if (pokemonBiosGetTamagoFlag(pokemon) == 0) {
                if (pokemonCheckValid(pokemon) != 0) {
                    goto selected_species_rejected;
                }
            }
            present = 1;
selected_species_rejected:
            invalid = present;
        }
        if ((u8)invalid != 0) {
            return 1;
        }

        for (i = 0; i < 6; i++) {
            candidate = heroBiosGetPokemonPtr(hero, (u16)i);
            if (candidate == pokemon) {
                continue;
            }

            present = candidate == 0;
            invalid = 0;
            if (present == 0) {
                if (pokemonGetStatus(candidate, 0, 0x6E, 0) != 0) {
                    goto candidate_present_species;
                }
            }
            invalid = 1;
candidate_present_species:
            if (invalid != 0) {
                continue;
            }

            present = 0;
            if (present == 0) {
                if (pokemonGetStatus(candidate, 0, 0x6E, 0) != 0) {
                    goto candidate_species_valid;
                }
            }
            present = 1;
candidate_species_valid:
            if (present != 0) {
                invalid = 0;
            } else {
                present = 0;
                if (pokemonBiosGetTamagoFlag(candidate) == 0) {
                    if (pokemonCheckValid(candidate) != 0) {
                        goto candidate_species_rejected;
                    }
                }
                present = 1;
candidate_species_rejected:
                invalid = present;
            }
            if ((u8)invalid != 0) {
                continue;
            }

            present = pokemonBiosGetPokemonDataId(pokemon);
            if ((u16)pokemonBiosGetPokemonDataId(candidate) == (u16)present) {
                return 0;
            }
        }
        return 1;

    case 2:
        if (rule[0xD] != 0) {
            return 1;
        }

        present = pokemon == 0;
        invalid = 0;
        if (present == 0) {
            if (pokemonGetStatus(pokemon, 0, 0x6E, 0) != 0) {
                goto selected_present_item;
            }
        }
        invalid = 1;
selected_present_item:
        if (invalid != 0) {
            return 1;
        }

        present = 0;
        if (present == 0) {
            if (pokemonGetStatus(pokemon, 0, 0x6E, 0) != 0) {
                goto selected_item_valid;
            }
        }
        present = 1;
selected_item_valid:
        if (present != 0) {
            invalid = 0;
        } else {
            present = 0;
            if (pokemonBiosGetTamagoFlag(pokemon) == 0) {
                if (pokemonCheckValid(pokemon) != 0) {
                    goto selected_item_rejected;
                }
            }
            present = 1;
selected_item_rejected:
            invalid = present;
        }
        if ((u8)invalid != 0) {
            return 1;
        }
        if (pokemonBiosGetItemDataId(pokemon) == 0) {
            return 1;
        }

        for (i = 0; i < 6; i++) {
            candidate = heroBiosGetPokemonPtr(hero, (u16)i);
            if (candidate == pokemon) {
                continue;
            }

            present = candidate == 0;
            invalid = 0;
            if (present == 0) {
                if (pokemonGetStatus(candidate, 0, 0x6E, 0) != 0) {
                    goto candidate_present_item;
                }
            }
            invalid = 1;
candidate_present_item:
            if (invalid != 0) {
                continue;
            }

            present = 0;
            if (present == 0) {
                if (pokemonGetStatus(candidate, 0, 0x6E, 0) != 0) {
                    goto candidate_item_valid;
                }
            }
            present = 1;
candidate_item_valid:
            if (present != 0) {
                invalid = 0;
            } else {
                present = 0;
                if (pokemonBiosGetTamagoFlag(candidate) == 0) {
                    if (pokemonCheckValid(candidate) != 0) {
                        goto candidate_item_rejected;
                    }
                }
                present = 1;
candidate_item_rejected:
                invalid = present;
            }
            if ((u8)invalid != 0) {
                continue;
            }

            present = pokemonBiosGetItemDataId(pokemon);
            if ((u16)pokemonBiosGetItemDataId(candidate) == (u16)present) {
                return 0;
            }
        }
        return 1;

    case 3:
        invalid = 0;
        if (fn_80076F2C(hero, rule, 3) == 0) {
            present = 0;
            if (pokemon != 0 &&
                pokemonGetStatus(pokemon, 0, 0x6E, 0) != 0) {
                goto selected_valid_for_mode3;
            }
            present = 1;
selected_valid_for_mode3:
            if (present != 0) {
                return (u8)invalid;
            }
        }
        invalid = 1;
        return (u8)invalid;
    }

    return 0;
}
#pragma peephole reset

/* Check the whole party against the selected party rule. */
#pragma peephole off
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

        outer_invalid = 0;
        outer_present = pokemon == 0;
        if ((s32)outer_present == 0) {
            if (pokemonGetStatus(pokemon, 0, 0x6E, 0) != 0) {
                goto pokemon_present;
            }
        }
        outer_invalid = 1;
pokemon_present:
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
            outer_rejected = 0;
            if (pokemonBiosGetTamagoFlag(pokemon) == 0) {
                if (pokemonCheckValid(pokemon) != 0) {
                    goto pokemon_rejected;
                }
            }
            outer_rejected = 1;
pokemon_rejected:
            result = (u8)outer_rejected;
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
#pragma peephole reset

#pragma push
#pragma scheduling off
s32 fn_80075A9C(void) { return fn_80190528(0xab5); }
#pragma pop

#pragma push
#pragma scheduling off
s32 fn_80075AC0(void) { return fn_801902E0(0xab5); }
#pragma pop

#pragma push
#pragma scheduling off
s32 fn_80075AE4(void) { return fn_80190528(0xab4); }
#pragma pop

#pragma push
#pragma scheduling off
s32 fn_80075B08(void) { return fn_801902E0(0xab4); }
#pragma pop

#pragma push
#pragma scheduling off
s32 fn_80075B2C(void) { return fn_80190528(0xab3); }
#pragma pop

#pragma push
#pragma scheduling off
s32 fn_80075B50(void) { return fn_801902E0(0xab3); }
#pragma pop

#pragma push
#pragma scheduling off
s32 fn_80075BFC(void) { return fn_80190528(0xab1); }
#pragma pop

#pragma push
#pragma scheduling off
s32 fn_80075C20(void) { return fn_801902E0(0xab1); }
#pragma pop

#pragma push
#pragma scheduling off
s32 fn_80075C44(void) { return fn_801902E0(0xa14); }
#pragma pop

#pragma push
#pragma scheduling off
s32 fn_80075C68(void) {
    fadeCheck(1);
    return menuClose(0xe0);
}
#pragma pop

/* fn_80075C94 (0x80075C94): choose the next e-Reader menu flow. */
#pragma push
#pragma peephole off
void fn_80075C94(void) {
    extern void msgctrlSetValue(s32 id, u32 value);
    extern s32 menuOpenCustom(s32 slot, ...);
    extern s32 fn_801D0748(u32, u32, u32);
    extern void* gamedatasaveGetStatus(u32, u32);
    s32 result;

    for (;;) {
        msgctrlSetValue(0x37, 0);
        result = menuOpenCustom(0xE0, 0, 0, 0x10, 1, 0);
        fadeCheck(1);

        switch (result) {
        case 0:
            floorLink(0x322, 0);
            return;

        case 1:
            result = fn_801D0748(2, 2, 0);
            if (result != 3 || gamedatasaveGetStatus(0, 4) == 0) {
                if (result == -1) {
                    continue;
                }
                winMsgOpen(2, 0x44DB, 1, 0);
                winMsgClose(1);
                continue;
            }
            floorLink(0x323, 0);
            return;

        case -1:
            break;

        case 2:
            break;

        default:
            break;
        }

        floorLink(0x320, 0);
        return;
    }
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80075390(void) {
    extern u8 fn_80075638(void);
    extern void fn_8007565C(void);
    extern void fn_800756C8(s32);

    if (fn_80075638() != 0) {
        fn_8007565C();
    } else {
        fn_800756C8(3);
    }
    return 0;
}
#pragma pop

/* fn_8007565C (0x8007565C): close and release the active menu resource. */
#pragma push
#pragma peephole off
void fn_8007565C(void) {
    extern u8 lbl_8047A610;
    extern void fn_8010A420(u32);
    extern void menuCloseCustom(s32, s32, s32);
    extern u16 fn_800E202C(u32);
    extern void fn_800E24B0(void);
    extern void fn_800E209C(u16);
    u16 handle;

    fn_8010A420(*(u32*)&lbl_8047A610 + 0x144);
    menuCloseCustom(0xD8, 0, 1);
    handle = fn_800E202C(*(u32*)&lbl_8047A610);
    if (handle != 0) {
        fn_800E24B0();
        fn_800E209C(handle);
    }
    *(u32*)&lbl_8047A610 = 0;
}
#pragma pop

/* fn_80075BC4 (0x80075BC4): helper counter clamp from 0 to 0x30. */
#pragma push
#pragma peephole off
s32 fn_80075BC4(void) {
    u32 value;

    value = fn_801906A0(0xab2);
    if (value > 0x30) {
        return 0;
    } else {
        return 0x30 - value;
    }
}
#pragma pop

/* fn_80075B74 (0x80075B74): increment and clamp the helper counter. */
#pragma push
#pragma peephole off
s32 fn_80075B74(void) {
    s32 result;
    u32 value;

    value = fn_801906A0(0xab2) + 1;
    result = 1;
    if (value > 0x30) {
        value = 0x30;
        result = 0;
    }
    _flagSet(0xab2, value);
    return result;
}
#pragma pop

/* Address: 0x80071700 | Size: 0x2A8 */
#pragma peephole off
s32 fn_80071700(s32 chan) {
    s32 mode;
    s32 result;
    u32 timeout;
    u32 start;
    u32 cmdBuf;
    u32 respBuf;
    u8 statusA;
    u8 statusB;
    u8 lenA;
    u8 lenB;

    mode = chan + 1;
    gbaCommandSetKeyState(mode, 2);
    result = fn_80073C38(chan);
    if (result != 0) {
        goto done;
    }

    cmdBuf = 0x44;
    if (GBAWrite(chan, (u32)&cmdBuf, (u32)&lenA) != 0) {
        result = 0xB;
        goto done;
    }

    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if ((OSGetTick() - start) > timeout) {
            result = 1;
            break;
        }
        if (GBAGetStatus(chan, (u32)&statusA) != 0) {
            result = 2;
            break;
        }
        if ((statusA & 0xA) == 8) {
            result = 0;
            break;
        }
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            result = 0x3E8;
            break;
        }
    }

    if (result == 0) {
        if (GBARead(chan, (u32)&respBuf, (u32)&lenA) != 0) {
            result = 3;
        }
    }
    if (result != 0) {
        result = result + 0xB;
        goto done;
    }

    if ((respBuf >> 24) != 0x44) {
        result = 0xF;
        goto done;
    }

    timeout = OSMillisecondsToTicks(30000);
    start = OSGetTick();
    for (;;) {
        if ((OSGetTick() - start) > timeout) {
            result = 1;
            break;
        }
        if (GBAGetStatus(chan, (u32)&statusB) != 0) {
            result = 2;
            break;
        }
        if ((statusB & 0xA) == 8) {
            result = 0;
            break;
        }
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            result = 0x3E8;
            break;
        }
    }

    if (result == 0) {
        if (GBARead(chan, (u32)&respBuf, (u32)&lenB) != 0) {
            result = 3;
        }
    }
    if (result != 0) {
        result = result + 0xF;
        goto done;
    }

    result = (respBuf != 0) ? 0x13 : 0;

done:
    gbaCommandSetKeyState(mode, 1);
    return result;
}
#pragma peephole reset

/* Address: 0x800722A0 | Size: 0x2A8 -- byte-identical body to fn_80071700 */
#pragma peephole off
s32 fn_800722A0(s32 chan) {
    s32 mode;
    s32 result;
    u32 timeout;
    u32 start;
    u32 cmdBuf;
    u32 respBuf;
    u8 statusA;
    u8 statusB;
    u8 lenA;
    u8 lenB;

    mode = chan + 1;
    gbaCommandSetKeyState(mode, 2);
    result = fn_80073C38(chan);
    if (result != 0) {
        goto done;
    }

    cmdBuf = 0x44;
    if (GBAWrite(chan, (u32)&cmdBuf, (u32)&lenA) != 0) {
        result = 0xB;
        goto done;
    }

    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if ((OSGetTick() - start) > timeout) {
            result = 1;
            break;
        }
        if (GBAGetStatus(chan, (u32)&statusA) != 0) {
            result = 2;
            break;
        }
        if ((statusA & 0xA) == 8) {
            result = 0;
            break;
        }
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            result = 0x3E8;
            break;
        }
    }

    if (result == 0) {
        if (GBARead(chan, (u32)&respBuf, (u32)&lenA) != 0) {
            result = 3;
        }
    }
    if (result != 0) {
        result = result + 0xB;
        goto done;
    }

    if ((respBuf >> 24) != 0x44) {
        result = 0xF;
        goto done;
    }

    timeout = OSMillisecondsToTicks(30000);
    start = OSGetTick();
    for (;;) {
        if ((OSGetTick() - start) > timeout) {
            result = 1;
            break;
        }
        if (GBAGetStatus(chan, (u32)&statusB) != 0) {
            result = 2;
            break;
        }
        if ((statusB & 0xA) == 8) {
            result = 0;
            break;
        }
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            result = 0x3E8;
            break;
        }
    }

    if (result == 0) {
        if (GBARead(chan, (u32)&respBuf, (u32)&lenB) != 0) {
            result = 3;
        }
    }
    if (result != 0) {
        result = result + 0xF;
        goto done;
    }

    result = (respBuf != 0) ? 0x13 : 0;

done:
    gbaCommandSetKeyState(mode, 1);
    return result;
}
#pragma peephole reset

/*
 * More small helpers ported from the previous campaign's archive
 * (menu_common_ext.c, menu_tool.c, menu_tool2.c, menu_exdisc.c);
 * re-verified against this unit's own compiler flags.
 */
extern void OSResumeThread(u32);
extern u32 lbl_8047A600;

/* fn_80072684 (0x80072684): resume the thread stashed at lbl_8047A600. */
void fn_80072684(void) {
    OSResumeThread(lbl_8047A600);
}

static inline s32 fn_800726A8_poll_read(s32 chan, u32* response, u8* length,
                                        u8* status) {
    extern s32 GBARead(s32 chan, void* destination, u8* status);
    extern s32 GBAGetStatus(s32 chan, u8* status);
    u32 timeout;
    u32 start;

    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return 1;
        }
        if (GBAGetStatus(chan, status) != 0) {
            return 2;
        }
        if ((*status & 0xA) == 8) {
            break;
        }
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            return 0x3E8;
        }
    }
    if (GBARead(chan, response, length) != 0) {
        return 3;
    }
    return 0;
}

/* fn_800726A8 (0x800726A8): send a fixed-size data block to one GBA
 * channel. */
#pragma push
#pragma peephole off
s32 fn_800726A8(s32 chan, const u32* data) {
    extern u32 fn_800D0F44(s32 chan);
    extern s32 GBAWrite(s32 chan, void* source, u8* status);
    extern s32 GBARead(s32 chan, void* destination, u8* status);
    extern s32 GBAGetStatus(s32 chan, u8* status);
    u32 response;
    u32 command;
    u32 send_word;
    u8 length;
    u8 send_status;
    u8 status;
    s32 key_channel;
    s32 result;
    u32 outer_timeout;
    u32 outer_start;
    u32 timeout;
    u32 start;
    s32 offset;
    u32 send_timeout;
    s32 outer_expired;
    const u32* data_ptr;

    key_channel = chan + 1;
    gbaCommandSetKeyState(key_channel, 2);
    outer_timeout = OSMillisecondsToTicks(100);
    outer_start = OSGetTick();
    do {
        outer_expired = OSGetTick() - outer_start > outer_timeout;

        timeout = OSMillisecondsToTicks(100);
        start = OSGetTick();
        for (;;) {
            if (OSGetTick() - start > timeout) {
                result = 1;
                goto transfer_done;
            }
            if (fn_800D0F44(chan) == 0x40000) {
                break;
            }
            if (lbl_803B6E18[chan].func != NULL) {
                lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
            }
            if (lbl_803B6E08[chan] != 0) {
                result = 0x3E8;
                goto transfer_done;
            }
        }

        {
            s32 setup_result;

            setup_result = fn_80073C38(chan);
            if (setup_result != 0) {
                result = setup_result;
                goto transfer_done;
            }
        }

        command = 0x55;
        if (GBAWrite(chan, &command, &length) != 0) {
            result = 0xB;
            goto header_done;
        }

        {
            s32 poll_result;

            poll_result = fn_800726A8_poll_read(chan, &response, &length,
                                                &status);
            result = poll_result != 0 ? poll_result + 0xB : 0;
        }

header_done:
        switch (result) {
        case 0:
            if ((response >> 24) != 0x55) {
                result = 0xF;
                goto transfer_done;
            }

            data_ptr = data;
            for (offset = 0; offset < 0x78; data_ptr++, offset += 4) {
                send_word = *data_ptr;
                if (GBAWrite(chan, &send_word, &send_status) != 0) {
                    goto data_done;
                }

                send_timeout = OSMillisecondsToTicks(100);
                start = OSGetTick();
                for (;;) {
                    if (OSGetTick() - start > send_timeout) {
                        goto data_done;
                    }
                    if (GBAGetStatus(chan, &send_status) != 0) {
                        goto data_done;
                    }
                    if ((send_status & 2) == 0) {
                        break;
                    }
                    if (lbl_803B6E18[chan].func != NULL) {
                        lbl_803B6E18[chan].func(chan,
                                               lbl_803B6E18[chan].arg);
                    }
                    if (lbl_803B6E08[chan] == 0) {
                        continue;
                    }
                    goto data_done;
                }
                if (lbl_803B6E08[chan] != 0) {
                    goto data_done;
                }
            }

data_done:
            result = 0;
            break;
        default:
            goto transfer_done;
        }
transfer_done:
        ;
    } while (result == 1 && outer_expired == 0);

    gbaCommandSetKeyState(key_channel, result != 0 ? 1 : 3);
    return result;
}
#pragma pop

/* fn_80072D58 (0x80072D58): send a variable-size data block to one GBA
 * channel. */
#pragma peephole off
s32 fn_80072D58(s32 chan, const u32* data, s32 size) {
    extern s32 GBAWrite(s32 chan, void* source, u8* status);
    extern s32 GBARead(s32 chan, void* destination, u8* status);
    extern s32 GBAGetStatus(s32 chan, u8* status);
    u32 response;
    u32 command;
    u32 send_word;
    u8 length;
    u8 send_status;
    u8 status;
    s32 key_channel;
    s32 result;
    u32 outer_timeout;
    u32 outer_start;
    u32 start;
    s32 offset;
    u32 send_timeout;
    u32 timeout;
    s32 outer_expired;

    key_channel = chan + 1;
    gbaCommandSetKeyState(key_channel, 2);
    outer_timeout = OSMillisecondsToTicks(100);
    outer_start = OSGetTick();
    do {
        outer_expired = OSGetTick() - outer_start > outer_timeout;

        {
            s32 setup_result;

            setup_result = fn_80073C38(chan);
            if (setup_result != 0) {
                result = setup_result;
                goto transfer_done;
            }
        }

        command = 0x66;
        if (GBAWrite(chan, &command, &length) != 0) {
            result = 0xB;
            goto header_done;
        }

        {
            s32 poll_result;

            timeout = OSMillisecondsToTicks(100);
            start = OSGetTick();
            for (;;) {
                if (OSGetTick() - start > timeout) {
                    poll_result = 1;
                    goto poll_done;
                }
                if (GBAGetStatus(chan, &status) != 0) {
                    poll_result = 2;
                    goto poll_done;
                }
                if ((status & 0xA) == 8) {
                    break;
                }
                if (lbl_803B6E18[chan].func != NULL) {
                    lbl_803B6E18[chan].func(chan,
                                           lbl_803B6E18[chan].arg);
                }
                if (lbl_803B6E08[chan] != 0) {
                    poll_result = 0x3E8;
                    goto poll_done;
                }
            }
            if (GBARead(chan, &response, &length) != 0) {
                poll_result = 3;
            } else {
                poll_result = 0;
            }
poll_done:
            result = poll_result != 0 ? poll_result + 0xB : 0;
        }

header_done:
        switch (result) {
        case 0:
            if ((response >> 24) != 0x66) {
                result = 0xF;
                goto transfer_done;
            }

            {
                const u32* data_ptr = data;

                for (offset = 0; offset < size;
                     offset += 4, data_ptr++) {
                    send_word = *data_ptr;
                    if (GBAWrite(chan, &send_word, &send_status) != 0) {
                        result = 0x10;
                        goto transfer_done;
                    }

                    send_timeout = OSMillisecondsToTicks(100);
                    start = OSGetTick();
                    for (;;) {
                        if (OSGetTick() - start > send_timeout) {
                            result = 0x11;
                            goto transfer_done;
                        }
                        if (GBAGetStatus(chan, &send_status) != 0) {
                            result = 0x12;
                            goto transfer_done;
                        }
                        if ((send_status & 2) == 0) {
                            break;
                        }
                    }
                    if ((offset % 0x40) == 0) {
                        _threadSwitch();
                    }
                }
            }
            result = 0;
            break;
        default:
            goto transfer_done;
        }
transfer_done:
        ;
    } while (result == 1 && outer_expired == 0);

    gbaCommandSetKeyState(key_channel, result != 0 ? 1 : 3);
    return result;
}
#pragma peephole reset

/* fn_80073E84 (0x80073E84): constant-1 accessor. */
s32 fn_80073E84(void) {
    return 1;
}

extern s32 menuIsCheck(s32);

/* fn_80073E8C (0x80073E8C): load one or two menu binary blocks and refresh checksum bytes. */
s32 fn_80073E8C(void* pathA, void* pathB) {
    extern void* memcpy(void* dst, const void* src, u32 size);
    extern void* fn_800A7BCC(void);
    extern void GBAInit(void);
    extern s32 DVDOpen(void* path, MenuDVDFileInfo* info);
    extern s32 DVDRead(MenuDVDFileInfo* info, void* dst, s32 length, s32 offset, s32 prio);
    extern void DVDClose(MenuDVDFileInfo* info);
    extern void fn_800060F0(const char* file, s32 line, const char* fmt, ...);
    extern u8 lbl_80268780[];
    extern u32 lbl_8047A60C;
    extern u32 lbl_8047A608;
    extern u32 lbl_8047A604;
    extern u8 lbl_803D6E40[];
    extern u8 lbl_803B6E40[];
    MenuDVDFileInfo fileA;
    MenuDVDFileInfo fileB;
    u32 size;
    u8* buf;
    s32 i;
    s32 checksum;

    memcpy(&lbl_8047A60C, fn_800A7BCC(), 4);
    lbl_8047A60C |= 0x20002020;
    GBAInit();

    if (DVDOpen(pathA, &fileA) == 0) {
        fn_800060F0((const char*)lbl_80268780, 0x1d6, (const char*)lbl_80268780 + 0x10);
    }
    size = (fileA.length + 7) & ~7;
    lbl_8047A608 = size;
    if (size == 0 || size > 0x20000) {
        fn_800060F0((const char*)lbl_80268780, 0x1dc, (const char*)lbl_80268780 + 0x28);
    }
    size = (lbl_8047A608 + 0x1f) & ~0x1f;
    lbl_8047A608 = size;
    if (DVDRead(&fileA, lbl_803D6E40, size, 0, 2) < 0) {
        fn_800060F0((const char*)lbl_80268780, 0x1e1, (const char*)lbl_80268780 + 0x4c);
    }
    DVDClose(&fileA);

    buf = lbl_803D6E40;
    buf[0xac] = ((u8*)&lbl_8047A60C)[0];
    buf[0xad] = ((u8*)&lbl_8047A60C)[1];
    buf[0xae] = ((u8*)&lbl_8047A60C)[2];
    buf[0xaf] = ((u8*)&lbl_8047A60C)[3];
    checksum = 0xe7;
    for (i = 0xa0; i < 0xbd; i++) {
        checksum -= buf[i];
    }
    buf[i] = checksum;

    if (pathB != 0) {
        if (DVDOpen(pathB, &fileB) == 0) {
            fn_800060F0((const char*)lbl_80268780, 0x1d6, (const char*)lbl_80268780 + 0x10);
        }
        size = (fileB.length + 7) & ~7;
        lbl_8047A604 = size;
        if (size == 0 || size > 0x20000) {
            fn_800060F0((const char*)lbl_80268780, 0x1dc, (const char*)lbl_80268780 + 0x28);
        }
        size = (lbl_8047A604 + 0x1f) & ~0x1f;
        lbl_8047A604 = size;
        if (DVDRead(&fileB, lbl_803B6E40, size, 0, 2) < 0) {
            fn_800060F0((const char*)lbl_80268780, 0x1e1, (const char*)lbl_80268780 + 0x4c);
        }
        DVDClose(&fileB);

        buf = lbl_803B6E40;
        buf[0xac] = ((u8*)&lbl_8047A60C)[0];
        buf[0xad] = ((u8*)&lbl_8047A60C)[1];
        buf[0xae] = ((u8*)&lbl_8047A60C)[2];
        buf[0xaf] = ((u8*)&lbl_8047A60C)[3];
        checksum = 0xe7;
        for (i = 0xa0; i < 0xbd; i++) {
            checksum -= buf[i];
        }
        buf[i] = checksum;
    }

    return 0;
}

/* fn_80075638 (0x80075638): tail-call wrapper. */
#pragma push
#pragma scheduling off
s32 fn_80075638(void) { return menuIsCheck(0xd8); }
#pragma pop

extern u8 lbl_8047A5D0;
/* fn_800757F0 (0x800757F0): release and clear the handle at lbl_8047A5D0. */
void fn_800757F0(void) {
    fn_801CB9D8(*(u32*)&lbl_8047A5D0);
    *(u32*)&lbl_8047A5D0 = 0;
}

/* fn_80075D98 (0x80075D98): no-op. */
void fn_80075D98(void) {
}

/* fn_80075D9C (0x80075D9C): tail-call wrapper. */
#pragma push
#pragma scheduling off
s32 fn_80075D9C(void) {
    fadeCheck(1);
    return menuClose(0xe2);
}
#pragma pop

extern s32 fn_80165A20(s32, s32, s32);

/* fn_80075EE0 (0x80075EE0): start the menu worker and fade in. */
void fn_80075EE0(void) {
    extern s32 lbl_8047A618;
    extern f32 lbl_8047C0C8;
    extern u32 fn_800FF560(void);
    extern void* GSthreadCreate(s32, u32, u32, s32, s32, void*);
    extern void fadeSet(s32, f32);
    extern s32 fn_80075F4C(void);

    if (lbl_8047A618 == 0) {
        GSthreadCreate(1, fn_800FF560(), 0x4000, 1, 1, fn_80075F4C);
    } else {
        fn_80165A20(0x46a, 0, 0x7f);
    }
    fadeSet(2, lbl_8047C0C8);
}

/* fn_80075F4C (0x80075F4C): tail-call wrapper. */
#pragma push
#pragma scheduling off
s32 fn_80075F4C(void) { return fn_80165A20(0x46a, 0, 0x7f); }
#pragma pop

/* fn_80075F78 (0x80075F78): set the message value for the selected rule. */
void fn_80075F78(void* rule) {
    extern u32 GSmsgGetGSchar(s32 messageId);
    extern void msgctrlSetValue(s32 id, u32 value);
    s32 messageId;

    switch (*(s8*)((u8*)rule + 0x95)) {
    case 0:
        messageId = 0x43bc;
        break;
    case 1:
        messageId = 0x43ba;
        break;
    case 2:
        messageId = 0x43be;
        break;
    default:
        messageId = 1;
        break;
    }
    msgctrlSetValue(0x37, GSmsgGetGSchar(messageId));
}

/* menuCBRule_CheckPokemonEventFlag (0x80075FEC): require the event flag for
 * the two special Pokemon data IDs. */
#pragma push
#pragma peephole off
u8 menuCBRule_CheckPokemonEventFlag(u8* pokemon) {
    extern u16 pokemonBiosGetPokemonDataId(u8* pokemon);
    extern u8 pokemonBiosGetEventGetFlag(u8* pokemon);

    switch (pokemonBiosGetPokemonDataId(pokemon)) {
    case 0x97:
    case 0x19A:
        if (pokemonBiosGetEventGetFlag(pokemon) == 0) {
            return 0;
        }
        break;
    }
    return 1;
}
#pragma pop

typedef struct MenuRuleMessages {
    u8 pad[0xFC];
    u16 pokemonErrors[6];
} MenuRuleMessages;

#pragma peephole off
s32 fn_80076054(void* hero, const u8* battleRules)
{
    extern s32 pokemonGetStatus(void*, s32, s32, s32);
    extern u8 pokemonBiosGetLevel(void*);
    extern u32 pokemonBiosGetItemDataId(void*);
    extern u8 fn_80142984(u32);
    extern u8* fn_8006B420(void);
    extern u8 fn_80076F2C(void*, const u8*, u32);
    extern const u8 lbl_8047C0D0[7];
    extern const u16 lbl_8047C0D8[4];
    extern u32 lbl_80478928;
    extern u16 lbl_802EE458[];
    const MenuRuleMessages* ruleText = (const MenuRuleMessages*)lbl_80268940;
    u32 partyWork;
    u32 validationWork;
    u32 ruleCheck;
    s32 ruleSlot;
    u32 teamCheck;

    for (partyWork = 0; partyWork < 6; partyWork++) {
        for (validationWork = 0; (s32)validationWork < 6; validationWork++) {
            if (fn_80076398(heroBiosGetPokemonPtr(hero, (u16)validationWork),
                           partyWork) == 0) {
                return ruleText->pokemonErrors[partyWork];
            }
        }
    }

    if (battleRules == 0) {
        return 0;
    }

    for (ruleCheck = 0; ruleCheck < 3; ruleCheck++) {
        for (ruleSlot = 0; ruleSlot < 6; ruleSlot++) {
            u8 valid = 0;

            partyWork = (u32)heroBiosGetPokemonPtr(hero, (u16)ruleSlot);
            validationWork = 0;

            if (partyWork == 0 ||
                pokemonGetStatus((void*)partyWork, 0, 0x6E, 0) == 0) {
                validationWork = 1;
            }

            if ((s32)validationWork != 0) {
                valid = 1;
            } else {
                switch (ruleCheck) {
                case 0:
                    valid = pokemonBiosGetLevel((void*)partyWork) >=
                            *(s16*)(battleRules + 0);
                    break;
                case 1:
                    valid = *(s16*)(battleRules + 2) >=
                            pokemonBiosGetLevel((void*)partyWork);
                    break;
                case 2: {
                    u8 itemValid;

                    partyWork = pokemonBiosGetItemDataId((void*)partyWork);
                    validationWork = (u32)fn_8006B420();
                    switch ((u16)partyWork) {
                    case 0:
                        itemValid = 1;
                        break;
                    case 0xAF:
                        itemValid = 0;
                        break;
                    default:
                        itemValid = fn_80142984(partyWork);
                        break;
                    }

                    if (itemValid == 0) {
                        valid = 0;
                        break;
                    }

                    switch (*(s32*)(validationWork + 8)) {
                    case 0:
                        valid = 1;
                        break;
                    case 1:
                        valid = (u16)partyWork == 0;
                        break;
                    case 2: {
                        u8* itemRules = (u8*)validationWork;
                        s32 itemIndex;
                        const u32 itemCount = lbl_80478928;

                        for (itemIndex = 0; (u32)itemIndex < itemCount;
                             itemIndex++) {
                            if ((u16)partyWork == lbl_802EE458[itemIndex]) {
                                valid = itemRules[0x18 + itemIndex] == 0;
                                goto item_rule_checked;
                            }
                        }
                        valid = 1;
item_rule_checked:
                        break;
                    }
                    default:
                        valid = 0;
                        break;
                    }
                    break;
                }
                default:
                    __assert((const char*)ruleText + 0x108, 0xFB,
                             (const char*)ruleText + 0x118);
                    valid = 0;
                    break;
                }
            }

            if (valid == 0) {
                return ((const u16*)lbl_8047C0D0)[ruleCheck];
            }
        }
    }

    for (teamCheck = 0; teamCheck < 4; teamCheck++) {
        if (fn_80076F2C(hero, battleRules, teamCheck) == 0) {
            return lbl_8047C0D8[teamCheck];
        }
    }
    return 0;
}
#pragma peephole reset

/* menuCBRule_CheckPokemonErrorAll (0x80076334): require every party member to
 * pass the per-slot error check. */
#pragma push
#pragma scheduling off
#pragma peephole off
u8 menuCBRule_CheckPokemonErrorAll(void* pokemon) {
    extern u8 fn_80076398(void* pokemon, s32 index);
    s32 i;

    for (i = 0; i < 6; i++) {
        if (fn_80076398(pokemon, i) == 0) {
            return 0;
        }
    }
#pragma scheduling on
    return 1;
}
#pragma pop

/* Validate every party member against the active battle rule. */
u8 fn_800767B8(void* hero, const u8* rule)
{
    extern u8 fn_80076F2C(void* hero, const u8* rule, s32 mode);
    extern void* heroBiosGetPokemonPtr(void* hero, u16 slot);
    extern u8 pokemonCheckValid(void* pokemon);
    extern s32 pokemonGetStatus(void* pokemon, s32 index, s32 field,
                                s32 subindex);
    extern u8 pokemonBiosGetLevel(void* pokemon);
    extern u16 pokemonBiosGetItemDataId(void* pokemon);
    extern u8* fn_8006B420(void);
    extern u8 fn_80142984(u16 item);
    extern u16 lbl_802EE458[];
    extern u32 lbl_80478928;
    extern void __assert(const char* file, s32 line, const char* condition);
    extern const char lbl_80268A48[];
    extern const char lbl_80268A58[];
    void* pokemon;
    u8* itemRule;
    u16 item;
    s32 partyCount;
    s32 slot;
    s32 check;
    s32 mode;
    s32 i;
    u8 valid;

    for (mode = 0; mode < 4; mode++) {
        if (!fn_80076F2C(hero, rule, mode)) {
            return 0;
        }
    }

    partyCount = 0;
    for (slot = 0; slot < 6; slot++) {
        pokemon = heroBiosGetPokemonPtr(hero, (u16)slot);
        if (pokemon == NULL || !pokemonCheckValid(pokemon)) {
            continue;
        }

        for (check = 0; check < 3; check++) {
            if (pokemonGetStatus(pokemon, 0, 0x6E, 0) == 0) {
                valid = 1;
            } else {
                switch (check) {
                case 0:
                    valid = pokemonBiosGetLevel(pokemon) >=
                            *(const s16*)(rule + 0);
                    break;
                case 1:
                    valid = pokemonBiosGetLevel(pokemon) <=
                            *(const s16*)(rule + 2);
                    break;
                case 2:
                    item = pokemonBiosGetItemDataId(pokemon);
                    itemRule = fn_8006B420();
                    if (item == 0) {
                        valid = 1;
                    } else if (item == 0xAF) {
                        valid = 0;
                    } else {
                        valid = fn_80142984(item);
                    }
                    if (!valid) {
                        break;
                    }

                    switch (*(s32*)(itemRule + 8)) {
                    case 0:
                        valid = 1;
                        break;
                    case 1:
                        valid = item == 0;
                        break;
                    case 2:
                        valid = 1;
                        for (i = 0; (u32)i < lbl_80478928; i++) {
                            if (item == lbl_802EE458[i]) {
                                valid = itemRule[i + 0x18] == 0;
                                break;
                            }
                        }
                        break;
                    default:
                        valid = 0;
                        break;
                    }
                    break;
                default:
                    __assert(lbl_80268A48, 0xFB, lbl_80268A58);
                    valid = 0;
                    break;
                }
            }
            if (!valid) {
                return 0;
            }
        }
        partyCount++;
    }
    return partyCount != 0;
}

#pragma peephole off
u8 fn_800776E4(void* hero) {
    extern u8* fn_8006B420(void);
    extern u8 fn_80076F2C(void* hero, const u8* rule, s32 mode);
    extern void* heroBiosGetPokemonPtr();
    extern u8 pokemonCheckValid(void* pokemon);
    extern u8 pokemonBiosGetLevel(void* pokemon);
    extern u16 pokemonBiosGetItemDataId(void* pokemon);
    extern s32 pokemonGetStatus(void* pokemon, s32 index, s32 field, s32 subindex);
    extern u8 fn_80142984(u16 item);
    extern void __assert(const char* file, s32 line, const char* condition);
    extern u8 lbl_80268A48[];
    extern u8 lbl_80268A58[];
    extern u16 lbl_802EE458[];
    extern u32 lbl_80478928;
    s32 is_null;
    void* party_pokemon;
    s32 count;
    s32 party_slot;
    u8* rule;
    u8 result;
    s32 final_result;
    s32 mode;

    {
        void* pokemon;
        s32 slot;
        s32 check;

        slot = 0;
        while ((u16)slot < 6) {
            pokemon = heroBiosGetPokemonPtr(hero, slot);
            check = 0;
            do {
                if (fn_80076398(pokemon, check) == 0) {
                    result = 0;
                    goto pokemon_error_done;
                }
                check++;
            } while (check < 6);
            result = 1;

pokemon_error_done:
            if (result == 0) {
                result = 0;
                goto party_error_done;
            }
            slot++;
        }
        result = 1;
    }

party_error_done:
    final_result = result != 0;
    if (final_result == 0) {
        goto done;
    }

    rule = fn_8006B420();
    count = 0;
    mode = count;
    do {
        if (fn_80076F2C(hero, rule, mode) == 0) {
            result = 0;
            goto trainer_rule_done;
        }
        mode++;
    } while (mode < 4);
    result = 1;

trainer_rule_done:
    if (result == 0) {
        result = 0;
        goto regulation_done;
    } else {
        party_slot = 0;
        do {
            party_pokemon = heroBiosGetPokemonPtr(hero, (u16)party_slot);
            if (party_pokemon != 0) {
                if (pokemonCheckValid(party_pokemon) != 0) {
                    s32 check;

                    is_null = party_pokemon == 0;
                    check = 0;
                    do {
                        s32 blank;

                        blank = 0;
                        if (is_null == 0) {
                            if (pokemonGetStatus(party_pokemon, 0, 0x6E, 0) != 0) {
                                goto pokemon_present;
                            }
                        }
                        blank = 1;

pokemon_present:
                        if (blank != 0) {
                            result = 1;
                        } else {
                            switch (check) {
                            case 0:
                                result = pokemonBiosGetLevel(party_pokemon) >=
                                         *(s16*)(rule + 0);
                                break;
                            case 1:
                                result = pokemonBiosGetLevel(party_pokemon) <=
                                         *(s16*)(rule + 2);
                                break;
                            case 2:
                            {
                                u8* item_rule;
                                u16 item;
                                u8 valid_item;
                                s32 i;

                                item = pokemonBiosGetItemDataId(party_pokemon);
                                item_rule = fn_8006B420();
                                switch (item) {
                                case 0:
                                    valid_item = 1;
                                    break;
                                case 0xAF:
                                    valid_item = 0;
                                    break;
                                default:
                                    valid_item = fn_80142984(item);
                                    break;
                                }
                                if (valid_item == 0) {
                                    result = 0;
                                    break;
                                }

                                switch (*(s32*)(item_rule + 8)) {
                                case 0:
                                    result = 1;
                                    break;
                                case 1:
                                    result = item == 0;
                                    break;
                                case 2:
                                    for (i = 0; (u32)i < lbl_80478928; i++) {
                                        if (item == lbl_802EE458[i]) {
                                            result = item_rule[i + 0x18] == 0;
                                            goto item_rule_done;
                                        }
                                    }
                                    result = 1;
                                    break;
                                default:
                                    result = 0;
                                    break;
                                }
item_rule_done:
                                break;
                            }
                            default:
                                __assert((const char*)lbl_80268A48, 0xFB,
                                         (const char*)lbl_80268A58);
                                result = 0;
                                break;
                            }
                        }

                        if (result == 0) {
                            result = 0;
                            goto pokemon_rules_done;
                        }
                        check++;
                    } while (check < 3);
                    result = 1;

pokemon_rules_done:
                    if (result == 0) {
                        result = 0;
                        goto regulation_done;
                    }
                    count++;
                }
            }
            party_slot++;
        } while (party_slot < 6);
        result = count > 0;
    }

regulation_done:
    final_result = result != 0;
done:
    return (u8)final_result;
}
#pragma peephole reset

/* fn_80077A5C (0x80077A5C): accept an empty slot or a zero species value. */
#pragma push
#pragma peephole off
u8 fn_80077A5C(void* pokemon) {
    extern s32 pokemonGetStatus(void* pokemon, s32 index, s32 field, s32 subindex);
    s32 result;

    result = 0;
    if (pokemon == 0 || pokemonGetStatus(pokemon, 0, 0x6E, 0) == 0) {
        result = 1;
    }
    return result;
}
#pragma pop

extern u8* fn_8006B420(void);

/* fn_80077AAC..fn_80077B60 (0x80077AAC-0x80077B60): fixed-index byte
 * accessors into the fn_8006B420() record. */
#pragma push
#pragma scheduling off
u8 fn_80077AAC(void) { return fn_8006B420()[0x13]; }
#pragma pop

#pragma push
#pragma scheduling off
u8 fn_80077AD0(void) { return fn_8006B420()[0x12]; }
#pragma pop

#pragma push
#pragma scheduling off
u8 fn_80077AF4(void) { return fn_8006B420()[0x11]; }
#pragma pop

#pragma push
#pragma scheduling off
u8 fn_80077B18(void) { return fn_8006B420()[0x10]; }
#pragma pop

#pragma push
#pragma scheduling off
u8 fn_80077B3C(void) { return fn_8006B420()[0xf]; }
#pragma pop

#pragma push
#pragma scheduling off
u8 fn_80077B60(void) { return fn_8006B420()[0xe]; }
#pragma pop

/* fn_80077B84 (0x80077B84): fixed-index s16 accessor into the same record. */
#pragma push
#pragma scheduling off
s16 fn_80077B84(void) { return ((s16*)fn_8006B420())[0xb]; }
#pragma pop

/* menuCBRule_GetBattleTimeLimit (0x80077BA8): same shape, scaled by 0x3c. */
s32 menuCBRule_GetBattleTimeLimit(void) {
    return ((s16*)fn_8006B420())[0xa] * 0x3c;
}

/* fn_80077BD0 (0x80077BD0): accept initialized save-status values. */
#pragma push
#pragma scheduling off
u8 fn_80077BD0(void) {
    extern s32* savedataGetStatus(s32 side, s32 slotType);
    s32 value;

    value = savedataGetStatus(0, 0xE)[2];
    switch (value) {
    case 0:
    case 1:
    case 2:
        return 1;
    }
    return 0;
}
#pragma pop

/* menuCBRule_CheckValidItem (0x80077C1C): handle sentinel item ids locally. */
u8 menuCBRule_CheckValidItem(u16 item) {
    extern u8 fn_80142984(u16 item);

    switch (item) {
    case 0:
        return 1;
    case 0xAF:
        return 0;
    default:
        return fn_80142984(item);
    }
}

extern u32 lbl_80478928;
extern u16 lbl_802EE458[];

typedef struct MenuRuleItemRestrictions {
    u8 pad_00[8];
    s32 mode;
    u8 pad_0C[0xC];
    u8 item_disabled[0x3C];
} MenuRuleItemRestrictions;

/* fn_80077C68 (0x80077C68): apply the current rule's item restriction. */
#pragma push
#pragma peephole off
u8 fn_80077C68(u16 item) {
    MenuRuleItemRestrictions* rule;
    u32 i;

    rule = (MenuRuleItemRestrictions*)fn_8006B420();
    if (menuCBRule_CheckValidItem(item) == 0) {
        return 0;
    }

    switch (rule->mode) {
    case 0:
        return 1;
    case 1:
        return item == 0;
    case 2:
        for (i = 0; i < lbl_80478928; i++) {
            if (item == lbl_802EE458[i]) {
                return rule->item_disabled[i] == 0;
            }
        }
        return 1;
    default:
        return 0;
    }
}
#pragma pop

/* fn_80077D88 (0x80077D88): bounds-checked table lookup. */
u16 fn_80077D88(s32 index) {
    if (index < 0 || lbl_80478928 <= (u32)index) {
        return 0;
    }
    return lbl_802EE458[index];
}

/* fn_80077DB8 (0x80077DB8): map the current save state to a rule value. */
#pragma push
#pragma scheduling off
#pragma peephole off
s32 fn_80077DB8(void) {
    extern s32* savedataGetStatus(s32 side, s32 slotType);
    s32* entry;
    s32 state;

    entry = savedataGetStatus(0, 0xE);
    if (entry[0] == 2) {
        entry = savedataGetStatus(0, 0xE);
        if (entry[2] == 0) {
            return 6;
        }
    }

    entry = savedataGetStatus(0, 0xE);
    state = entry[1];
    switch (state) {
    case 0:
        return 3;
    case 1:
        return 4;
    case 2:
        break;
    default:
        break;
    }
    return 2;
}
#pragma pop

extern u8 lbl_80268940[];

/* menuCBRule_ConstantRule (0x80077E50): fixed-slot table lookup, NULL out of range. */
void* menuCBRule_ConstantRule(s32 index) {
    switch (index) {
    case 0:
    case 1:
    case 2:
        return lbl_80268940 + (index * 0x54);
    }
    return (void*)0;
}

extern void* memcpy(void* dst, const void* src, u32 size);

/* fn_80077E80 (0x80077E80): fixed-size record copy. */
#pragma scheduling off
void fn_80077E80(void* dst, void* src) {
    memcpy(dst, src, 0x54);
}
#pragma scheduling on

extern s32 memcmp(const void* s1, const void* s2, u32 size);

/* fn_80077EA4 (0x80077EA4): fixed-size record equality check. */
#pragma push
#pragma peephole off
u8 fn_80077EA4(u16* s1, u16* s2) {
    return memcmp(s1, s2, 0x54) == 0;
}
#pragma pop


#pragma push
#pragma peephole off
/* 0x80077ED4 | Celebi/e-Reader party transfer sequence. */
void fn_80077ED4(void)
{
    typedef struct MenuSaveSnapshot {
        u8 bytes[0x1DFD0];
    } MenuSaveSnapshot;
    extern void* fn_801D036C(void);
    extern void fn_801D0314(void*);
    extern void* savedataGetStatus(u32, u32);
    extern void fn_80075B74(void);
    extern s32 fn_801D0748(u32, u32, u32);
    extern void fn_80166A28(u32);
    extern void* heroBiosGetPokemonPtr(void*, u32);
    extern void pokemonInit(void*);
    extern void* floorDataBiosGetCurrentPtr(void);
    extern u32 floorDataBiosGetFloorID(void*);
    extern void heroPokemonGetCelebi(void*, u32);
    extern u32 pokemonCheckValid(void*);
    extern void __assert(const char*, u32, const char*);
    extern char lbl_80268AB8[];
    extern char lbl_8047C0E8[] __attribute__((section(".sdata2")));
    extern void fn_80093574(u32);
    extern void fn_80092C90(u32, void*, void*);
    extern s32 fn_80093610(u32);
    extern void fn_80093698(u32);

    void* pokemon;
    void* name;
    void* backup;
    u32 i;
    u32 resource = 0x104F1000;

    name = heroBiosGetNamePtr(lbl_803F6F18);
    backup = fn_801D036C();
    *(MenuSaveSnapshot*)backup =
        *(MenuSaveSnapshot*)savedataGetStatus(0, 0);
    fn_80075B74();

    if (fn_801D0748(4, 2, 0) != 4) {
        fn_80166A28(0x4C7);
        fn_801CB834(resource, 4, 0, 0);
        scriptWaitSyncMotion(resource, 1);
        MenuWaitMotionInterval();
        msgctrlSetValue(0x4D, (u32)name);
        menuSetEnablePort(1);
        winMsgOpenField(0x44B0, 1, 0);
        menuSetEnablePort(1);
        winMsgOpen(2, 0x44CF, 1, 0);
        winMsgClose(1);
    } else {
        menuSetEnablePort(1);
        winMsgOpen(2, 0x3D83, 0, 1);
        _threadSwitch();

        for (i = 0; (u16)i < 6; i++) {
            pokemonInit(heroBiosGetPokemonPtr(lbl_803F6F18, i));
        }

        heroPokemonGetCelebi(
            lbl_803F6F18,
            floorDataBiosGetFloorID(floorDataBiosGetCurrentPtr()));

        for (i = 0; (u16)i < 6; i++) {
            void* candidate = heroBiosGetPokemonPtr(lbl_803F6F18, i);
            u32 valid = (u8)pokemonCheckValid(candidate);
            if (valid != 0) {
                pokemon = heroBiosGetPokemonPtr(lbl_803F6F18, i);
                break;
            }
        }
        if (pokemon == NULL) {
            __assert(lbl_80268AB8, 0x42E, lbl_8047C0E8);
        }

        *(u32*)(lbl_803F6E40 + 8) |= 8;
        fn_80093574(1);
        fn_80092C90(1, lbl_803F6E40, pokemon);
        fn_80093574(1);

        if (fn_80093610(1) != 0xC) {
            fn_80093698(1);
            winMsgClose(1);
            menuSetEnablePort(1);
            winMsgOpen(2, 0x3D85, 1, 0);
            winMsgClose(1);

            *(MenuSaveSnapshot*)savedataGetStatus(0, 0) =
                *(MenuSaveSnapshot*)backup;
            fn_801D0748(4, 2, 0);
            fn_80166A28(0x4C7);
            fn_801CB834(resource, 4, 0, 0);
            scriptWaitSyncMotion(resource, 1);
            MenuWaitMotionInterval();
            msgctrlSetValue(0x4D, (u32)name);
            menuSetEnablePort(1);
            winMsgOpenField(0x44B0, 1, 0);
            winMsgClose(1);
            menuSetEnablePort(1);
            winMsgOpen(2, 0x44CF, 1, 0);
            winMsgClose(1);
        } else {
            fn_80093698(1);
            menuSetEnablePort(1);
            winMsgOpen(2, 0x3D84, 1, 0);
            winMsgClose(1);
            fn_80165668(0x3D2, 0, 0xFF);
            msgctrlSetValue(0x4D, (u32)name);
            menuSetEnablePort(1);
            winMsgOpenField(0x4435, 1, 0);
            winMsgClose(1);
            menuSetEnablePort(1);
            winMsgOpen(2, 0x3D55, 1, 0);
            winMsgClose(1);
        }
    }

    fn_801D0314(backup);
    lbl_8047A620 = 0;
}
#pragma pop
/* fn_80078D38 (0x80078D38): reset the menu fade timer probe. */
#pragma push
#pragma scheduling off
s32 fn_80078D38(void) {
    return fadeCheck(1);
}
#pragma pop

/* fn_8007926C (0x8007926C): initialize the menu scene object. */
#pragma push
#pragma peephole off
void fn_8007926C(void) {
    extern u32 lbl_8047A620;
    extern u32 fn_801CBA0C(u32 resourceId);
    extern void fn_801CB954(u32 object, s32 visible);
    extern void fn_801CB61C(u32 object, u32 resourceId, s32 animationId);
    extern void fn_801CB834(u32 object, s32 arg1, s32 arg2, s32 arg3);
    u32 object;

    lbl_8047A620 = 1;
    object = fn_801CBA0C(0x10BD1000);
    fn_801CB954(object, 1);
    fn_801CB61C(object, 0x104F1000, 0x207);
    fn_801CB834(object, 0, 0, 1);
}
#pragma pop

extern s32 fn_800D37CC(void);
extern u32 fn_800D3088(void);
extern s32 menuOpen(s32, s32);
extern s32 fn_8001E184(void);

extern u32 lbl_804788F0;
extern u32 lbl_802E61D8[];
extern u8 lbl_8047A630;
extern u8 lbl_8047A631;
extern u8 lbl_8047A632;
extern u8 lbl_8047A633;
extern u8 lbl_8047A634;
extern u8 lbl_8047A635;
extern u32 lbl_8047A638;
extern f32 lbl_8047C108;
extern f32 lbl_8047C114;
extern f64 lbl_8047C118;
extern f64 lbl_8047C120;
extern f32 lbl_8047C128;

#define WAIT_MENU_TIME(limit_)           \
    do {                                 \
        f32 elapsed_ = lbl_8047C114;     \
        while (elapsed_ < (limit_)) {    \
            s32 frames_;                 \
            u32 ticks_;                  \
            _threadSwitch();             \
            frames_ = fn_800D37CC();     \
            ticks_ = fn_800D3088();      \
            elapsed_ += (f32)ticks_ / (f32)frames_; \
        }                                \
    } while (0)

#define SHOW_CANCEL_MESSAGE()    \
    do {                         \
        if (arg0 == 0) {         \
            winMsgOpen(2, 0x44cf, 1, 0); \
            winMsgClose(1);      \
        }                        \
    } while (0)

#define CLOSE_AND_ABORT()        \
    do {                         \
        menuClose(0xef);         \
        WAIT_MENU_TIME(lbl_8047C108); \
        lbl_8047A638 = 1;        \
        return 0;                \
    } while (0)

#define SHOW_BLOCKING_MESSAGE(msg_) \
    do {                            \
        winMsgOpenField((msg_), 1, 0); \
        winMsgClose(1);             \
        SHOW_CANCEL_MESSAGE();      \
        CLOSE_AND_ABORT();          \
    } while (0)

/*
 * Send a GBA multiboot image.  The link protocol starts by exchanging two
 * challenge words, then sends the image in four-byte blocks.  Blocks after
 * the 0xA0-byte header are encrypted with the rolling key used by the GBA
 * boot ROM.  The last exchange validates the rolling CRC and releases the
 * remote CPU.
 */
s32 fn_8007480C(s32 channel, s32 use_primary_image) {
    u8 status;
    u32 send_word;
    u32 recv_word;
    u32* image;
    u32 image_size;
    u32 challenge;
    u32 response_key;
    u32 padded_size;
    u32 timeout;
    u32 start;
    u32 bit_count;
    u32 value;
    u32 i;
    u32 offset;
    u32 rolling_crc;
    u32 rolling_key;
    u32 encoded;
    u32 expected_status;
    u32 final_word;
    u32 candidate;
    u32 checksum;

    if (use_primary_image != 0) {
        image = (u32*)lbl_803B6E40;
        image_size = lbl_8047A604;
    } else {
        image = (u32*)lbl_803D6E40;
        image_size = lbl_8047A608;
    }

    i = 0;
    do {
        if (i > 0x20) {
            challenge = 0xDD654321;
        } else {
            challenge = (OSGetTick() & 0x00FFFFFF) | 0xDD000000;
        }

        bit_count = 0;
        value = challenge;
        for (offset = 0; offset < 32; offset++) {
            if ((value & 1) != 0) {
                bit_count++;
            }
            value >>= 1;
        }
        i++;
    } while (bit_count < 10 || bit_count > 24);

    send_word = MENU_BSWAP32(challenge);
    if (GBAWrite(channel, (u32)&send_word, (u32)&status) != 0) {
        return 1;
    }

    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return 2;
        }
        if (GBAGetStatus(channel, (u32)&status) != 0) {
            return 3;
        }
        if (status == 0x38) {
            break;
        }
        if (lbl_803B6E18[channel].func != NULL) {
            lbl_803B6E18[channel].func(channel,
                                          lbl_803B6E18[channel].arg);
        }
        if (lbl_803B6E08[channel] != 0) {
            return 0x3E8;
        }
    }

    if (GBARead(channel, (u32)&recv_word, (u32)&status) != 0) {
        return 4;
    }
    value = MENU_BSWAP32(recv_word);
    if ((value & 0xFF) != 0xEE) {
        OSReport((const char*)lbl_80268780 + 0x68);
        response_key = 0;
    } else {
        response_key = value & 0xFFFFFF00;
        bit_count = 0;
        for (i = 0; i < 24; i++) {
            if ((value & 0x80000000) != 0) {
                bit_count++;
            }
            value <<= 1;
        }
        if (bit_count < 7 || bit_count > 14) {
            OSReport((const char*)lbl_80268780 + 0x98, response_key);
            response_key = 0;
        }
    }
    if (response_key == 0) {
        return 5;
    }

    padded_size = (image_size + 7) & ~7;
    send_word = MENU_BSWAP32((padded_size >> 3) - 1);
    if (GBAWrite(channel, (u32)&send_word, (u32)&status) != 0) {
        return 6;
    }

    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return 7;
        }
        if (GBAGetStatus(channel, (u32)&status) != 0) {
            return 8;
        }
        if ((status & 0x30) != 0x30) {
            return 9;
        }
        if ((status & 2) == 0) {
            break;
        }
        if (lbl_803B6E18[channel].func != NULL) {
            lbl_803B6E18[channel].func(channel,
                                          lbl_803B6E18[channel].arg);
        }
        if (lbl_803B6E08[channel] != 0) {
            return 0x3E8;
        }
    }

    rolling_crc = challenge ^ response_key;
    rolling_key = rolling_crc * 0x6177614B + 1;
    expected_status = 0x30;

    for (offset = 0; offset < padded_size; offset += 4, image++) {
        value = MENU_BSWAP32(*image);
        encoded = value;
        if (offset >= 0xA0) {
            encoded = (value - rolling_crc) ^ rolling_key;
            rolling_crc ^= value;
            for (i = 0; i < 32; i++) {
                if ((rolling_crc & 1) != 0) {
                    rolling_crc = (rolling_crc >> 1) ^ 0xA1C1;
                } else {
                    rolling_crc >>= 1;
                }
            }
            rolling_key = rolling_key * 0x6177614B + 1;
        }

        send_word = MENU_BSWAP32(encoded);
        if (GBAWrite(channel, (u32)&send_word, (u32)&status) != 0) {
            return 0xA;
        }
        if ((status & 0x30) != expected_status) {
            return 0x12;
        }
        expected_status ^= 0x10;

        timeout = OSMillisecondsToTicks(100);
        start = OSGetTick();
        for (;;) {
            if (OSGetTick() - start > timeout) {
                return 0xB;
            }
            if (GBAGetStatus(channel, (u32)&status) != 0) {
                return 0xC;
            }
            if ((status & 0x20) == 0) {
                return 0xD;
            }
            if ((status & 2) == 0) {
                break;
            }
            if (lbl_803B6E18[channel].func != NULL) {
                lbl_803B6E18[channel].func(channel,
                                              lbl_803B6E18[channel].arg);
            }
            if (lbl_803B6E08[channel] != 0) {
                return 0x3E8;
            }
        }
        if (lbl_803B6E08[channel] != 0) {
            return 0x3E8;
        }
    }

    timeout = OS_TIMER_CLOCK * 10;
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return 0xE;
        }
        if (GBAGetStatus(channel, (u32)&status) != 0) {
            return 0xF;
        }
        if ((status & 0x30) != 0x30) {
            return 0x10;
        }
        if ((status & 0xA) == 8) {
            if (GBARead(channel, (u32)&recv_word, (u32)&status) != 0) {
                return 0x11;
            }

            value = recv_word >> 24;
            if (value == 0xFF) {
                final_word = MENU_BSWAP32(recv_word);
                OSReport((const char*)lbl_80268780 + 0xCC, final_word,
                         rolling_crc);
                checksum = final_word >> 8;
                candidate = 0xFFFFFFFF;
                for (offset = 0; offset < 0x100; offset++) {
                    value = candidate ^ rolling_crc;
                    for (i = 0; i < 32; i++) {
                        if ((value & 1) != 0) {
                            value = (value >> 1) ^ 0xA1C1;
                        } else {
                            value >>= 1;
                        }
                    }
                    if (checksum == value) {
                        value = candidate ^ 0xBB;
                        for (i = 0; i < 32; i++) {
                            if ((value & 1) != 0) {
                                value = (value >> 1) ^ 0xA1C1;
                            } else {
                                value >>= 1;
                            }
                        }
                        final_word = value | 0xBB000000;
                        OSReport((const char*)lbl_80268780 + 0x100,
                                 candidate, final_word);
                        break;
                    }
                    candidate += 0x01000000;
                }
                if (offset >= 0x100) {
                    OSReport((const char*)lbl_80268780 + 0x130, final_word,
                             rolling_crc);
                    final_word = 0;
                }
                if (final_word == 0) {
                    return 0x12;
                }
                send_word = MENU_BSWAP32(final_word);
                if (GBAWrite(channel, (u32)&send_word, (u32)&status) != 0) {
                    return 0x13;
                }
                break;
            }
            if (value != 0xCC) {
                return 0x14;
            }
            if (lbl_803B6E08[channel] != 0) {
                return 0x3E8;
            }
        } else {
            if (lbl_803B6E18[channel].func != NULL) {
                lbl_803B6E18[channel].func(channel,
                                              lbl_803B6E18[channel].arg);
            }
            if (lbl_803B6E08[channel] != 0) {
                return 0x3E8;
            }
        }
    }

    timeout = OS_TIMER_CLOCK;
    start = OSGetTick();
    do {
        if (OSGetTick() - start > timeout) {
            return 0x15;
        }
        if (fn_800D0F44(channel) != 0x40000) {
            return 0;
        }
        if (lbl_803B6E18[channel].func != NULL) {
            lbl_803B6E18[channel].func(channel,
                                          lbl_803B6E18[channel].arg);
        }
        if (lbl_803B6E08[channel] != 0) {
            return 0x3E8;
        }
    } while (1);
}


u32 fn_8007B6D8(GbaBootContext* context) {
    u8 gba_status;
    u8 crc_byte;
    u32 send_word;
    u32 recv_word;
    u32 channel;
    u32 transfer_size;
    u32 handshake;
    u32 offset;
    u32 retries;
    u32 interrupts;
    u32 reply_crc;
    u32 candidate;
    u32 decrypt_word;
    u32 expected_crc;
    u32 i;

    channel = context->channel;
    context->state = 0;

    if (GBAReset(channel, (u32)&gba_status) != 0) {
        return 0;
    }
    if (GBAGetStatus(channel, (u32)&gba_status) == 0 &&
        gba_status == 0x28) {
        if (GBARead(channel, (u32)&recv_word, (u32)&gba_status) != 0) {
            return 0;
        }
        GBA_BOOT_CLASSIFY(context, recv_word, gba_status, handshake);
        if (handshake != 1) {
            return 0;
        }
        if (context->reject_upload != 0) {
            return 3;
        }
        if (GBAGetStatus(channel, (u32)&gba_status) != 0 ||
            gba_status != 0x20) {
            return 0;
        }
        if (GBAWrite(channel, (u32)&context->game_code,
                     (u32)&gba_status) != 0) {
            return 0;
        }
        if (GBAGetStatus(channel, (u32)&gba_status) != 0 ||
            gba_status != 0x30) {
            return 0;
        }

        transfer_size = context->upload_end - context->upload_start;
        send_word = GBA_BOOT_BSWAP(transfer_size);
        if (GBAWrite(channel, (u32)&send_word, (u32)&gba_status) != 0) {
            return 0;
        }
        if (GBARead(channel, (u32)&recv_word, (u32)&gba_status) != 0 ||
            send_word != recv_word) {
            return 0;
        }

        context->state = 1;
        for (offset = 0; offset < transfer_size; offset += 4) {
            retries = 0;
            for (;;) {
                if (context->cancel != 0) {
                    GBA_BOOT_FAIL(context, 9);
                }
                if (GBAGetStatus(channel, (u32)&gba_status) != 0) {
                    GBA_BOOT_FAIL(context, 9);
                }
                if ((gba_status & 0x30) != 0x30) {
                    if ((gba_status & 8) != 0) {
                        GBARead(channel, (u32)&recv_word,
                                (u32)&gba_status);
                    }
                    GBA_BOOT_FAIL(context, 9);
                }
                if ((gba_status & 2) == 0) {
                    break;
                }
                if (++retries > 9) {
                    GBA_BOOT_FAIL(context, 9);
                }
                GBA_BOOT_DELAY(context, fn_8007C23C, interrupts);
            }
            send_word = *(u32*)(context->upload_start + offset);
            if (GBAWrite(channel, (u32)&send_word, (u32)&gba_status) != 0) {
                GBA_BOOT_FAIL(context, 9);
            }
        }

        context->state = 2;
        for (;;) {
            if (context->cancel != 0) {
                GBA_BOOT_FAIL(context, 9);
            }
            if (GBAGetStatus(channel, (u32)&gba_status) != 0 ||
                (gba_status & 8) != 0) {
                break;
            }
            GBA_BOOT_DELAY(context, fn_8007C23C, interrupts);
        }
        if (GBAReset(channel, (u32)&gba_status) != 0) {
            GBA_BOOT_FAIL(context, 9);
        }
        if (GBAGetStatus(channel, (u32)&gba_status) != 0 ||
            gba_status != 8) {
            GBA_BOOT_FAIL(context, 9);
        }
        if (GBARead(channel, (u32)&recv_word, (u32)&gba_status) != 0) {
            GBA_BOOT_FAIL(context, 9);
        }
        GBA_BOOT_CLASSIFY(context, recv_word, gba_status, handshake);
        if (handshake != 2) {
            GBA_BOOT_FAIL(context, 9);
        }
        if (GBAWrite(channel, (u32)&context->device_code,
                     (u32)&gba_status) != 0) {
            GBA_BOOT_FAIL(context, 9);
        }
        context->state = 3;
        return 1;
    }

    if (GBAGetStatus(channel, (u32)&gba_status) != 0 || gba_status != 8) {
        return 0;
    }
    if (GBARead(channel, (u32)&recv_word, (u32)&gba_status) != 0) {
        return 0;
    }
    GBA_BOOT_CLASSIFY(context, recv_word, gba_status, handshake);
    if (handshake != 2) {
        return 0;
    }

    if (context->status_only != 0) {
        send_word = context->device_code;
    } else {
        context->state = 4;
        send_word = context->device_code | 0x20202020;
    }
    if (GBAGetStatus(channel, (u32)&gba_status) != 0 || gba_status != 0) {
        GBA_BOOT_FAIL(context, 9);
    }
    if (GBAWrite(channel, (u32)&send_word, (u32)&gba_status) != 0) {
        GBA_BOOT_FAIL(context, 9);
    }

    if (context->status_only != 0) {
        context->state = 0;
        if ((context->boot_type & 1) != 0) {
            return 4;
        }
        return 2;
    }

    for (;;) {
        if (context->cancel != 0) {
            GBA_BOOT_FAIL(context, 6);
        }
        if (GBAGetStatus(channel, (u32)&gba_status) != 0) {
            break;
        }
        if ((gba_status & 0x30) == 0x30 ||
            (gba_status & 0x30) == 0) {
            break;
        }

        if ((gba_status & 0x32) == 0x20) {
            context->challenge = OSGetTick();
            send_word = context->challenge;
            if (GBAWrite(channel, (u32)&send_word, (u32)&gba_status) != 0 ||
                (gba_status & 0x30) != 0x20) {
                GBA_BOOT_FAIL(context, 9);
            }

            for (offset = 0; offset < 0x1040; offset += 4) {
                retries = 0;
                for (;;) {
                    if (GBAGetStatus(channel, (u32)&gba_status) != 0) {
                        GBA_BOOT_FAIL(context, 9);
                    }
                    if ((gba_status & 0x30) != 0x20) {
                        GBA_BOOT_FAIL(context, 9);
                    }
                    if ((gba_status & 0xA) == 8) {
                        break;
                    }
                    if (++retries > 9) {
                        GBA_BOOT_FAIL(context, 9);
                    }
                    GBA_BOOT_DELAY(context, fn_8007C23C, interrupts);
                }
                if (GBARead(channel, (u32)&recv_word,
                            (u32)&gba_status) != 0) {
                    GBA_BOOT_FAIL(context, 9);
                }
                context->reply[offset >> 2] =
                    recv_word ^ context->challenge;
            }

            expected_crc = GBA_BOOT_BSWAP(context->reply[0x40F]);
            for (candidate = 0; candidate < 0x100; candidate++) {
                reply_crc = context->crc;
                crc_byte = candidate;
                reply_crc = (reply_crc >> 8) ^
                            lbl_803FAEF8[(reply_crc ^ crc_byte) & 0xFF];
                if (reply_crc == expected_crc) {
                    break;
                }
            }
            if (candidate == 0x100) {
                GBA_BOOT_FAIL(context, 7);
            }

            decrypt_word = 0xAA478422;
            crc_byte = candidate;
            decrypt_word = (decrypt_word >> 8) ^
                           lbl_803FAEF8[(decrypt_word ^ crc_byte) & 0xFF];
            decrypt_word = GBA_BOOT_BSWAP(decrypt_word);
            for (i = 0; i < 0x40F; i++) {
                context->reply[i] ^= decrypt_word;
            }

            expected_crc = GBA_BOOT_BSWAP(context->reply[0x40E]);
            reply_crc = context->crc;
            for (i = 0; i < 0x1038; i++) {
                reply_crc = (reply_crc >> 8) ^
                            lbl_803FAEF8[(reply_crc ^
                                         ((u8*)context->reply)[i]) &
                                        0xFF];
            }
            if (reply_crc != expected_crc) {
                GBA_BOOT_FAIL(context, 7);
            }
            GBA_BOOT_FAIL(context, 5);
        }

        GBA_BOOT_DELAY(context, fn_8007C23C, interrupts);
    }

    if (GBAGetStatus(channel, (u32)&gba_status) != 0) {
        GBA_BOOT_FAIL(context, 9);
    }
    if ((gba_status & 0x30) == 0) {
        GBA_BOOT_FAIL(context, 6);
    }
    if ((gba_status & 0x30) == 0x30) {
        if ((gba_status & 8) != 0) {
            GBA_BOOT_FAIL(context, 7);
        }
        GBA_BOOT_FAIL(context, 8);
    }
    GBA_BOOT_FAIL(context, 9);
}


#pragma push
#pragma peephole off
void fn_80078390(void)
{
    void* name;
    u8 sex;
    s8 answer;
    u32 resource = 0x104F1000;

    sex = heroBiosGetSexDataId(lbl_803F6F18);
    name = heroBiosGetNamePtr(lbl_803F6F18);

    if ((*(u32*)(lbl_803F6E40 + 8) & 8) != 0) {
        MenuOpenHeroField(name, 0x43CB, 0x43CD, sex);
        menuSetEnablePort(1);
        winMsgOpen(2, 0x44CF, 1, 0);
        winMsgClose(1);
        lbl_8047A620 = 0;
        return;
    }

    MenuOpenHeroField(name, 0x43C7, 0x43C9, sex);
    fadeSet(3, lbl_8047C100);
    fadeCheck(1);
    fn_801CB834(resource, 2, 0, 0);
    fn_80166A28(0x4C7);
    fadeSet(2, lbl_8047C100);
    fadeCheck(1);
    scriptWaitSyncMotion(resource, 1);
    fn_801CB834(resource, 3, 0, 1);
    msgctrlSetValue(0x4D, (u32)name);
    menuSetEnablePort(1);
    winMsgOpenField(0x4434, 1, 0);
    winMsgClose(1);

    if (*(u16*)(lbl_803F6E40 + 0x0C) == 6) {
        menuSetEnablePort(1);
        winMsgOpenField(0x43A6, 1, 0);
        fn_80166A28(0x4C7);
        fn_801CB834(resource, 4, 0, 0);
        scriptWaitSyncMotion(resource, 1);
        MenuWaitMotionInterval();
        msgctrlSetValue(0x4D, (u32)name);
        menuSetEnablePort(1);
        winMsgOpenField(0x44B0, 1, 0);
        menuSetEnablePort(1);
        winMsgOpen(2, 0x44CF, 1, 0);
        winMsgClose(1);
        lbl_8047A620 = 0;
        return;
    }

    fn_80166A28(0x4C7);
    fn_801CB834(resource, 5, 0, 0);
    scriptWaitSyncMotion(resource, 1);
    MenuWaitMotionInterval();
    msgctrlSetValue(0x4D, (u32)name);
    menuSetEnablePort(1);
    winMsgOpen(2, 0x43B0, 1, 0);
    menuSetEnablePort(1);
    answer = menuSubOpenYesNo(0, -1, -1, 0);
    winMsgClose(1);

    switch (answer) {
    case -1:
    case 1:
        fn_80166A28(0x4C7);
        fn_801CB834(resource, 4, 0, 0);
        scriptWaitSyncMotion(resource, 1);
        MenuWaitMotionInterval();
        menuSetEnablePort(1);
        winMsgOpenField(0x44B0, 1, 0);
        winMsgClose(1);
        menuSetEnablePort(1);
        winMsgOpen(2, 0x44CF, 1, 0);
        winMsgClose(1);
        lbl_8047A620 = 0;
        break;
    case 0:
    default:
        lbl_8047A620 = 0x0B;
        break;
    }
}
#pragma pop


#pragma push
#pragma peephole off
void fn_800788BC(void* backup)
{
    typedef struct MenuSaveSnapshot {
        u8 bytes[0x1DFD0];
    } MenuSaveSnapshot;
    extern void* savedataGetStatus(u32, u32);
    extern s32 fn_80075BFC(void);
    extern void* floorDataBiosGetCurrentPtr(void);
    extern u32 floorDataBiosGetFloorID(void*);
    extern void heroPokemonGetCelebi(void*, u32);
    extern s32 fn_801D0748(u32, u32, u32);
    extern void fn_80165668(u32, u32, u32);

    MenuSaveSnapshot* savedataBackup = backup;
    void* hero;
    u32 resource = 0x104F1000;

    winMsgOpenField(0x43A1, 1, 0);
    winMsgClose(1);
    winMsgOpenField(0x43A2, 1, 0);
    winMsgClose(1);

    fadeSet(3, lbl_8047C100);
    fadeCheck(1);
    fn_801CB834(resource, 2, 0, 0);
    fn_80166A28(0x4C7);
    fadeSet(2, lbl_8047C100);
    fadeCheck(1);
    scriptWaitSyncMotion(resource, 1);
    fn_801CB834(resource, 3, 0, 1);

    winMsgOpenField(0x43A3, 1, 0);
    winMsgClose(1);
    winMsgOpenField(0x43A4, 1, 0);
    winMsgClose(1);

    hero = savedataGetStatus(0, 2);
    if (MenuHeroHasFreePartySlot(hero) == 0) {
        winMsgOpenField(0x43A6, 1, 0);
        fn_80166A28(0x4C7);
        fn_801CB834(resource, 4, 0, 0);
        scriptWaitSyncMotion(resource, 1);
        MenuWaitMotionInterval();
        winMsgOpenField(0x43AC, 1, 0);
        winMsgClose(1);
        lbl_8047A620 = 0;
        return;
    }

    fn_80166A28(0x4C7);
    fn_801CB834(resource, 5, 0, 0);
    scriptWaitSyncMotion(resource, 1);
    MenuWaitMotionInterval();

    *savedataBackup =
        *(MenuSaveSnapshot*)savedataGetStatus(0, 0);
    fn_80075BFC();
    heroPokemonGetCelebi(
        hero, floorDataBiosGetFloorID(floorDataBiosGetCurrentPtr()));

    if ((s8)fn_801D0748(4, 2, 0) != 4) {
        fn_80166A28(0x4C7);
        fn_801CB834(resource, 4, 0, 0);
        scriptWaitSyncMotion(resource, 1);
        MenuWaitMotionInterval();
        *(MenuSaveSnapshot*)savedataGetStatus(0, 0) =
            *savedataBackup;
        winMsgOpenField(0x43AC, 1, 0);
        lbl_8047A620 = 0;
        return;
    }

    fn_80165668(0x3D2, 0, 0xFF);
    winMsgOpenField(0x43A8, 1, 0);
    winMsgClose(1);
    winMsgOpenField(0x43AA, 1, 0);
    winMsgClose(1);
    lbl_8047A620 = 0;
}
#pragma pop


/* Drive the e-Reader/ex-disc scene state machine to completion. */
#pragma push
#pragma peephole off
void fn_80078D5C(void)
{
    extern void* fn_801D036C();
    extern void fn_801D0314();
    extern void* fn_80113F48();
    extern void cameraPlayAnime();
    extern u8 fn_80075C44();
    extern u8 fn_80075C20();
    extern u8 fn_801EE398();
    extern u32 fn_80075BC4();
    extern void winMsgOpenField();
    extern void winMsgClose();
    extern void fn_80166A28();
    extern void msgctrlSetValue();
    extern s8 fn_8001E184();
    extern void fn_800788BC();
    extern void fn_80077ED4();
    extern void* GSresGetResource();
    extern void GSmodelSetVisibility();
    extern u32 fn_801CBA0C();
    extern void GSscene_SetMode();
    extern s32 fn_800849B4();
    extern void fn_801CB9D8();
    extern u8 lbl_80268AA8[];
    extern u8 lbl_803F6E40[];
    extern u8 lbl_803F6F18[];
    extern const f32 lbl_8047C104;

    void* context;
    void* model;
    u32 scene;
    s8 answer;
    f32 elapsed;
    MenuExDiscLoadData loadData;

    context = fn_801D036C();
    while ((s32)lbl_8047A620 > 0) {
        switch (lbl_8047A620) {
        case 1:
            cameraPlayAnime(fn_80113F48(), 0x10951800, 0, 0);
            fn_801CB834(0x104F1000, 6, 0, 1);
            if (fn_80075C44() == 1) {
                lbl_8047A620 = 2;
            } else {
                lbl_8047A620 = 3;
            }
            break;

        case 2:
            fadeSet(3, lbl_8047C100);
            fadeCheck(1);
            fn_801CB834(0x104F1000, 0, 0, 1);
            fadeSet(2, lbl_8047C100);
            fadeCheck(1);
            fadeCheck(1);
            winMsgOpenField(0x43C3, 1, 0);
            winMsgClose(1);
            if (fn_80075C20() == 1) {
                lbl_8047A620 = 5;
            } else {
                lbl_8047A620 = 4;
            }
            break;

        case 3:
            fadeSet(3, lbl_8047C100);
            fadeCheck(1);
            fn_801CB834(0x104F1000, 1, 0, 1);
            fadeSet(2, lbl_8047C100);
            fadeCheck(1);
            fadeCheck(1);
            winMsgOpenField(0x43C0, 1, 0);
            fn_80166A28(0x3F9);
            elapsed = lbl_8047C0E0;
            while (elapsed < lbl_8047C0E4) {
                _threadSwitch();
                elapsed += (f32)fn_800D3088() / (f32)fn_800D37CC();
            }
            lbl_8047A620 = 0;
            break;

        case 4:
            if (fn_801EE398() == 1) {
                lbl_8047A620 = 7;
            } else {
                lbl_8047A620 = 6;
            }
            break;

        case 5:
            winMsgOpenField(0x43AF, 1, 0);
            winMsgClose(1);
            scene = fn_80075BC4();
            if (scene < 1) {
                winMsgOpenField(0x43B2, 1, 0);
                winMsgClose(1);
                lbl_8047A620 = 0;
                break;
            }
            msgctrlSetValue(0x2F, scene);
            winMsgOpenField(0x43BB, 1, 0);
            answer = fn_8001E184();
            winMsgClose(1);
            switch (answer) {
            case -1:
            case 1:
                winMsgOpenField(0x43C1, 1, 0);
                lbl_8047A620 = 0;
                break;
            case 0:
            default:
                winMsgOpenField(0x43C4, 1, 0);
                winMsgClose(1);
                lbl_8047A620 = 8;
                break;
            }
            break;

        case 6:
            winMsgOpenField(0x43C6, 1, 0);
            winMsgClose(1);
            lbl_8047A620 = 0;
            break;

        case 7:
            fn_800788BC(context);
            break;

        case 8:
            loadData = *(MenuExDiscLoadData*)lbl_80268AA8;
            context = fn_80113F48();
            fadeSet(3, lbl_8047C104);
            fadeCheck(1);
            model = GSresGetResource(context, 0x104F1000);
            if (model != 0) {
                GSmodelSetVisibility(model, 0);
            }
            scene = fn_801CBA0C(0x0FFE1000);
            GSresGetResource(context, scene);
            cameraPlayAnime(0x5D5, 0x0FFF1800, 0, 1);
            GSscene_SetMode(4);
            fadeSet(2, lbl_8047C104);
            fadeCheck(1);
            loadData.words[1] = (u32)lbl_803F6F18;
            if (fn_800849B4(0, 0x20, &loadData, lbl_803F6E40) < 0) {
                lbl_8047A620 = 0;
                break;
            }
            fadeSet(3, lbl_8047C104);
            fadeCheck(1);
            fn_801CB9D8(scene);
            if (model != 0) {
                GSmodelSetVisibility(model, 1);
                cameraPlayAnime(context, 0x10951800, 0, 0);
            }
            fadeSet(2, lbl_8047C104);
            fadeCheck(1);
            lbl_8047A620 = 10;
            break;

        case 10:
            fn_80078390();
            break;

        case 11:
            fn_80077ED4();
            break;

        default:
            break;
        }
    }

    fn_801D0314(context);
    floorLink(0x321, 0);
}
#pragma pop


#pragma push
#pragma peephole off
void fn_800792D8(void) {
    u32 command[4];
    ExDiscCouponResult result;
    u8 hero[0xB20];
    void* sceneHandle;
    void* giftPokemon;
    u32 flags;
    u32 maskGift;
    u32 maskParty;
    u32 maskItem;
    u32 canBag;
    u32 canParty;
    u16 i;
    s8 choice;

    command[0] = lbl_80268AD0[0];
    command[1] = lbl_80268AD0[1];
    command[2] = lbl_80268AD0[2];
    command[3] = lbl_80268AD0[3];
    giftPokemon = 0;

    fadeSet(3, lbl_8047C108);
    fadeCheck(1);
    menuClose(0xe1);
    SET_EXDISC_VISIBILITY(0x104E1000, 0);
    SET_EXDISC_VISIBILITY(0x104E1001, 0);
    SET_EXDISC_VISIBILITY(0x104E1002, 0);

    sceneHandle = fn_801CBA0C(0x0FFE1000);
    GSresGetResource(fn_80113F48(), (u32)sceneHandle);
    cameraPlayAnime(0x5d4, 0x0FFF1800, 0, 1);
    GSscene_SetMode(4);
    fadeSet(2, lbl_8047C108);
    fadeCheck(1);

    command[1] = (u32)hero;
    if (fn_800849B4(0, 0, command, &result) < 0) {
        lbl_8047A638 = 1;
        RESTORE_EXDISC_SCENE(sceneHandle);
        return;
    }

    RESTORE_EXDISC_SCENE(sceneHandle);

    flags = result.optionFlags;
    maskGift = flags & 4;
    maskParty = flags & 2;
    maskItem = flags & 1;
    lbl_8047A62C = result.flagsA62C;
    lbl_8047A628 = result.couponValue;
    lbl_8047A635 = maskGift != 0;
    lbl_8047A634 = maskParty != 0;
    lbl_8047A633 = maskItem != 0;

    if (fn_80079EF4(0, result.couponValue) == 0) {
        return;
    }

    canBag = 1;
    if (lbl_8047A632 != 0) {
        if (fn_80029850(result.bag, result.itemId, 0x47, 0x3e7) < 1) {
            canBag = 0;
        } else {
            fn_800298DC(result.bag, result.itemId, 0x47, 1, -1, 0x3e7);
            result.optionFlags |= 4;
        }
    }

    if (lbl_8047A630 != 0 && fn_80029850(result.bag, result.itemId, 1, 0x3e7) < 1) {
        canBag = 0;
    }

    canParty = 1;
    if (lbl_8047A631 != 0) {
        canParty = result.partyCount < 6;
    }

    if (fn_80079C1C(0, canBag, canParty, heroGetStatus(hero, 1, 0)) == 0) {
        return;
    }

    winMsgOpen(2, 0x3d3b, 0, 1);

    if (lbl_8047A631 != 0) {
        for (i = 0; i < 6; i++) {
            pokemonInit(heroBiosGetPokemonPtr(hero, i));
        }

        heroPokemonGetPikachu(hero, floorDataBiosGetFloorID(floorDataBiosGetCurrentPtr()));

        for (i = 0; i < 6; i++) {
            void* pokemon = heroBiosGetPokemonPtr(hero, i);
            if (pokemonCheckValid(pokemon) != 0) {
                giftPokemon = pokemon;
                break;
            }
        }

        if (giftPokemon == 0) {
            __assert((const char*)lbl_80268AE0, 0x460,
                     (const char*)&lbl_8047C10C);
        }
        result.optionFlags |= 2;
    }

    if (lbl_8047A630 != 0) {
        fn_800298DC(result.bag, result.itemId, 1, 1, -1, 0x3e7);
        result.optionFlags |= 1;
    }

    fn_80093574(1);
    fn_80092C90(1, &result, giftPokemon);
    fn_80093574(1);
    if (fn_80093610(1) != 0xc) {
        fn_80093698(1);
        winMsgOpen(2, 0x3d85, 1, 0);
        winMsgClose(1);
        winMsgOpen(2, 0x44cf, 1, 0);
        winMsgClose(1);
        menuClose(0xef);
        lbl_8047A638 = 1;
        return;
    }

    fn_80093698(1);
    winMsgClose(1);
    winMsgOpenField(0x43c5, 1, 0);
    choice = (s8)fn_8001E184();
    if (choice != 0 && choice >= -1 && choice < 2) {
        winMsgOpenField(0x43ca, 1, 0);
        winMsgClose(1);
        menuClose(0xef);
        lbl_8047A638 = 0;
    } else {
        winMsgOpen(2, 0x44cf, 1, 0);
        winMsgClose(1);
        menuClose(0xef);
        lbl_8047A638 = 1;
    }
}
#pragma pop

#pragma peephole off
void fn_800798E8(void* backup)
{
    typedef struct MenuSaveSnapshot {
        u8 bytes[0x1DFD0];
    } MenuSaveSnapshot;
    extern s32 fn_801D0748(u32, u32, u32);
    extern u32 fn_80079C1C();
    extern void* gamedatasaveGetStatus(u32, u32);
    extern void* savedataGetStatus(u32, u32);
    extern void* fn_80128DD4(void*);
    extern u16 pcboxGetItemCapacity(void*, u32);
    extern void pcboxDelItem(void*, u32, u32);
    extern void* heroGetStatus(void*, u32, u32);
    extern u8 pokemonCheckValid(void*);
    extern void* floorDataBiosGetCurrentPtr(void);
    extern u32 floorDataBiosGetFloorID(void*);
    extern void heroPokemonGetPikachu(void*, u32);
    void* hero;
    u32 canBag;
    void* pcbox;
    s32 saveResult;
    u32 canParty;
    u8 i;
    void* pokemon;
    u32 partyResult;
    u32 heroValue;
    s8 choice;

    saveResult = fn_801D0748(2, 2, 0);
    if (saveResult != 3 || gamedatasaveGetStatus(0, 4) == NULL) {
        if (saveResult != -1) {
            winMsgOpen(2, 0x44DB, 1, 0);
            winMsgClose(1);
        }
        menuClose(0xEF);
        lbl_8047A638 = 1;
        return;
    }

    lbl_8047A635 = fn_80075AC0();
    lbl_8047A634 = fn_80075B08();
    lbl_8047A633 = fn_80075B50();
    heroValue = (u32)heroGetStatus(0, 0xE, 0);
    if (fn_80079EF4(1, heroValue) == 0) {
        return;
    }

    canBag = 1;
    pcbox = fn_80128DD4(savedataGetStatus(0, 0));
    if (lbl_8047A632 != 0 && pcboxGetItemCapacity(pcbox, 0x47) < 1) {
        canBag = 0;
    }
    if (lbl_8047A630 != 0 && pcboxGetItemCapacity(pcbox, 1) < 1) {
        canBag = 0;
    }

    canParty = 1;
    hero = savedataGetStatus(0, 2);
    if (lbl_8047A631 != 0) {
        for (i = 0; i < 6; i++) {
            if (pokemonCheckValid(heroGetStatus(hero, 3, i)) != 1) {
                partyResult = 1;
                goto party_capacity_done;
            }
        }
        partyResult = 0;
party_capacity_done:
        canParty = partyResult;
    }

    pokemon = heroGetStatus(0, 1, 0);
    *(MenuSaveSnapshot*)backup =
        *(MenuSaveSnapshot*)savedataGetStatus(0, 0);
    if ((u8)fn_80079C1C(1, canBag, canParty, pokemon) == 0) {
        return;
    }

    if (lbl_8047A632 != 0) {
        pcboxDelItem(pcbox, 0x47, 1);
        fn_80075A9C();
    }
    if (lbl_8047A631 != 0) {
        heroPokemonGetPikachu(
            hero, floorDataBiosGetFloorID(floorDataBiosGetCurrentPtr()));
        fn_80075AE4();
    }
    if (lbl_8047A630 != 0) {
        pcboxDelItem(pcbox, 1, 1);
        fn_80075B2C();
    }

    if (fn_801D0748(4, 2, 0) != 4) {
        menuClose(0xEF);
        *(MenuSaveSnapshot*)savedataGetStatus(0, 0) =
            *(MenuSaveSnapshot*)backup;
        lbl_8047A638 = 1;
        return;
    }

    winMsgOpenField(0x43C5, 1, 0);
    choice = (s8)fn_8001E184();
    if (choice != 0) {
        if (choice < 0) {
            if (choice >= -1) {
                goto accepted;
            }
            goto cancelled;
        }
        if (choice >= 2) {
            goto cancelled;
        }
accepted:
        winMsgOpenField(0x43C8, 1, 0);
        menuClose(0xEF);
        lbl_8047A638 = 0;
        return;
    }
cancelled:
    menuClose(0xEF);
    lbl_8047A638 = 1;
}
#pragma peephole reset

#pragma push
#pragma peephole off
u8 fn_80079EF4(s32 arg0, u32 value) {
    s32 rank;
    s8 choice;

    lbl_8047A630 = 0;
    lbl_8047A631 = 0;
    lbl_8047A632 = 0;

    rank = lbl_804788F0 - 1;
    while (rank >= 0 && lbl_802E61D8[rank] > value) {
        rank--;
    }
    if (rank < 0) {
        rank = 0;
    }

    menuClose(0xe1);
    WAIT_MENU_TIME(lbl_8047C128);
    menuOpen(0xef, 0);
    WAIT_MENU_TIME(lbl_8047C108);

    if (rank < 1) {
        SHOW_BLOCKING_MESSAGE(0x43a7);
    }

    switch (rank) {
    case 1:
        if (lbl_8047A635 != 0) {
            SHOW_BLOCKING_MESSAGE(0x43ae);
        }
        winMsgOpenField(0x43b4, 1, 0);
        lbl_8047A632 = 1;
        break;
    case 2:
        if (lbl_8047A635 != 0 && lbl_8047A634 != 0) {
            SHOW_BLOCKING_MESSAGE(0x43ab);
        }
        if (lbl_8047A635 != 0) {
            winMsgOpenField(0x43b3, 1, 0);
            lbl_8047A631 = 1;
        } else {
            winMsgOpenField(0x43b6, 1, 0);
            lbl_8047A632 = 1;
            lbl_8047A631 = 1;
        }
        break;
    case 3:
        if (lbl_8047A635 != 0 && lbl_8047A634 != 0 && lbl_8047A633 != 0) {
            SHOW_BLOCKING_MESSAGE(0x43a9);
        }
        if (lbl_8047A634 != 0) {
            winMsgOpenField(0x43b1, 1, 0);
            lbl_8047A630 = 1;
        } else if (lbl_8047A635 != 0) {
            winMsgOpenField(0x43b5, 1, 0);
            lbl_8047A630 = 1;
            lbl_8047A631 = 1;
        } else {
            winMsgOpenField(0x43c2, 1, 0);
            lbl_8047A630 = 1;
            lbl_8047A631 = 1;
            lbl_8047A632 = 1;
        }
        break;
    default:
        menuClose(0xef);
        winMsgClose(1);
        WAIT_MENU_TIME(lbl_8047C108);
        lbl_8047A638 = 1;
        return 0;
    }

    winMsgOpenField(0x43d1, 1, 0);
    choice = (s8)fn_8001E184();
    winMsgClose(1);
    if (choice == 0 || choice < -1 || choice >= 2) {
        return 1;
    }

    SHOW_CANCEL_MESSAGE();
    CLOSE_AND_ABORT();
}
#pragma pop


s32 fn_8007AB10(u32 code, u32* state) {
    u32 event;
    u32 done;

    if (code == 0) {
        return 0;
    }

    if (state[0] == 0) {
        if (fn_800D0F44(1) != 0x40000) {
            return fn_8007AB10_initial_result(code);
        }
        state[0] = 1;
        OPEN_LINK_PROMPT(3);
    }

    if (lbl_803F7A30[0x345] != 0) {
        fn_800A1E54(lbl_803F7A30 + 0x28, &event);
        lbl_803F7A30[0x345] = 0;

        if (fn_800D0F44(1) != 0x40000) {
            event = 10;
        } else if (event == 0) {
            event = 0;
        }

        switch (event) {
        case 0:
            OPEN_LINK_PROMPT(3);
            return fn_8007AB10_initial_result(code);
        case 1:
            OPEN_LINK_PROMPT(3);
            if (code == 15) {
                return 16;
            }
            return 0;
        case 2:
            if (code == 18) {
                OPEN_LINK_PROMPT(2);
                return 0;
            }
            OPEN_LINK_PROMPT(3);
            return fn_8007AB10_event2_result(code);
        case 3:
            if (code == 3 || code == 10 || code == 12 || code == 14) {
                OPEN_LINK_PROMPT(1);
                if (code == 14) {
                    return 0;
                }
                return 14;
            }
            OPEN_LINK_PROMPT(3);
            if (code == 1) {
                return 2;
            }
            if (code == 6) {
                return 7;
            }
            if (code == 8) {
                return 4;
            }
            return 0;
        case 4:
            done = (code == 1 || code == 3 || code == 6 || code == 8 || code == 10 ||
                    code == 12 || code == 14 || code == 15 || code == 17 || code == 18);
            OPEN_LINK_PROMPT(done != 0 ? 0 : 3);
            return 0;
        case 5:
            OPEN_LINK_PROMPT(3);
            return 20;
        case 6:
            OPEN_LINK_PROMPT(3);
            if (code != 21) {
                return 21;
            }
            return 0;
        case 7:
            OPEN_LINK_PROMPT(3);
            if (code != 22) {
                return 22;
            }
            return 0;
        case 8:
            OPEN_LINK_PROMPT(3);
            if (code != 23) {
                return 23;
            }
            return 0;
        case 9:
        case 10:
            state[0] = 0;
            if ((code - 14) <= 1 || code == 17 || code == 18) {
                return 19;
            }
            return 0;
        default:
            return 0;
        }
    }

    if (lbl_803F7A30[0x346] == 2 && code == 14) {
        return 15;
    }
    return 0;
}

#undef SHOW_BLOCKING_MESSAGE
#undef CLOSE_AND_ABORT
#undef SHOW_CANCEL_MESSAGE
#undef WAIT_MENU_TIME

void fn_80071318(u8* dst, u8* src) {
    s16 upperX;
    s16 upperY;
    u32 entryWord;
    s16 lowerX;
    s16 lowerY;

    upperX = *(volatile s16*)(src + 0xC);
    upperY = *(s16*)(src + 0xE);
    *(s16*)(dst + 0x54) = upperX;
    entryWord = *(u32*)(src + 0x10);
    *(s16*)(dst + 0x56) = upperY;
    lowerX = *(s16*)(src + 0x8);
    *(u32*)(dst + 0x58) = entryWord;
    lowerY = *(s16*)(src + 0xA);
    *(s16*)(dst + 0x5C) = lowerX;
    *(s16*)(dst + 0x5E) = lowerY;
}

/* fn_8007A5E8 (0x8007A5E8): draw the current coupon total. */
void fn_8007A5E8(s32 unused, u8* window) {
    extern u32 lbl_8047A62C;
    extern void msgctrlSetValue(s32, u32);
    extern u32 GSmsgGetRect(s32);
    extern void fn_800FB680(s32, s32, s32, s32);
    u32 value;
    u32 rect;

    if ((s32)lbl_8047A638 == 4) {
        value = lbl_8047A62C;
    } else {
        value = heroGetStatus(0, 0xD, 0);
    }
    msgctrlSetValue(0x50, value);
    rect = GSmsgGetRect(0x153);
    fn_800FB680(*(s16*)(window + 0x54) - (rect >> 16), 0, -1, 0x153);
}

/* fn_8007A664 (0x8007A664): show the sprite selected by the current mode. */
void fn_8007A664(s32 unused, u8* sprite) {
    switch (lbl_8047A638) {
    case 4:
        if (*(s16*)(sprite + 6) == 0x10BF) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 3:
        if (*(s16*)(sprite + 6) == 0x10C0) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    }
}

/* fn_8007A6F0 (0x8007A6F0): show the coupon-rank sprite. */
#pragma push
#pragma peephole off
void fn_8007A6F0(s32 unused, MenuRankSprite* sprite) {
    u32 value;
    s32 rank;

    if ((s32)lbl_8047A638 == 4) {
        value = lbl_8047A628;
    } else {
        value = heroGetStatus(0, 0xE, 0);
    }

    for (rank = lbl_804788F0 - 1; rank >= 0; rank--) {
        if (lbl_802E61D8[rank] <= value) {
            break;
        }
    }
    if (rank < 0) {
        rank = 0;
    }

    winSpriteSetDisp(sprite, 0);
    switch (rank) {
    case 1:
        if (sprite->id == 0x10C3) {
            winSpriteSetDisp(sprite, 1);
        }
        break;
    case 2:
        if (sprite->id == 0x10C4) {
            winSpriteSetDisp(sprite, 1);
        }
        break;
    case 3:
        if (sprite->id == 0x10C5) {
            winSpriteSetDisp(sprite, 1);
        }
        break;
    default:
        if (sprite->id == 0x10C2) {
            winSpriteSetDisp(sprite, 1);
        }
        break;
    }
}
#pragma pop

#pragma push
#pragma scheduling off
s32 fn_8007A82C(void) {
    return fadeCheck(1);
}
#pragma pop

void fn_8007A850(void)
{
    extern void* fn_801D036C(void);
    extern void fn_801D0314(void*);
    void* backup;

    backup = fn_801D036C();
    fadeCheck(1);
    do {
        switch (lbl_8047A638) {
        case 1:
        {
            s32 choice;

            winMsgOpenField(0x43CF, 0, 1);
            choice = menuOpen(0xE1, 1);
            winMsgClose(1);
            switch (choice) {
            case 1:
                lbl_8047A638 = 3;
                break;
            case 2:
                lbl_8047A638 = 4;
                break;
            case 3:
                lbl_8047A638 = 2;
                break;
            default:
                menuClose(0xE1);
                lbl_8047A638 = 0;
                break;
            }
            break;
        }
        case 2:
        {
            f32 elapsed;

            winMsgOpenField(0x43A5, 1, 0);
            winMsgClose(1);
            elapsed = lbl_8047C114;
            while (elapsed < lbl_8047C108) {
                s32 frames;
                u32 ticks;

                _threadSwitch();
                frames = fn_800D37CC();
                ticks = fn_800D3088();
                elapsed += (f32)ticks / (f32)frames;
            }
            lbl_8047A638 = 1;
            break;
        }
        case 3:
            fn_800798E8(backup);
            break;
        case 4:
            fn_800792D8();
            break;
        }
    } while ((s32)lbl_8047A638 > 0);

    fn_801D0314(backup);
    floorLink(0x321, 0);
}

void fn_8007AAFC(void) {
    lbl_803F7A30[0x342] = 1;
}

/* fn_8007AA6C (0x8007AA6C): reset event flag + start menu animation. */
void fn_8007AA6C(void) {
    extern u32 fn_80113F48(void);
    extern void cameraPlayAnime(u32, u32, u32, u32);

    lbl_8047A638 = 1;
    cameraPlayAnime(fn_80113F48(), 0x10941800, 0, 0);
}

/* fn_8007AAA8 (0x8007AAA8): wait for the worker and release its state. */
#pragma push
#pragma peephole off
s32 fn_8007AAA8(void) {
    extern s32 fn_800A1E54(void* thread, void** result);
    extern void fn_8007B0D8(void);
    void* result;

    lbl_803F7A30[0x342] = 1;
    while (*(volatile u8*)(lbl_803F7A30 + 0x345) == 0) {
    }
    fn_800A1E54(lbl_803F7A30 + 0x28, &result);
    fn_8007B0D8();
    return 0;
}
#pragma pop

/* fn_8007B090 (0x8007B090): snapshot callback state, then start the request. */
void fn_8007B090(s32 request) {
    extern void* fn_800A7BCC(void);
    extern u32 lbl_8047A640;
    extern void fn_8007B114(s32 request);
    void* state;

    state = fn_800A7BCC();
    memcpy(&lbl_8047A640, state, 4);
    fn_8007B114(request);
}

/* fn_8007B0D8 (0x8007B0D8): free outstanding pending callback state. */
void fn_8007B0D8(void) {
    extern void* lbl_80478980;
    extern void* lbl_8047A648;
    extern void* lbl_8047A64C;
    extern void fn_8009AAD4(void*, void*);
    void* r4;

    r4 = lbl_8047A648;
    if (r4 != NULL) {
        fn_8009AAD4((void*)lbl_80478980, r4);
        lbl_8047A648 = NULL;
        lbl_8047A64C = NULL;
    }
}

void fn_8007B114(s32 request)
{
    extern u32 lbl_803FAEF8[256];
    extern char* lbl_802EE608[];
    extern char lbl_803FADF8[];
    extern void* lbl_80478980;
    extern u8* lbl_8047A648;
    extern u32 lbl_8047A64C;
    extern u32 lbl_8047A650;
    extern char* strcpy(char*, const char*);
    extern void* memset(void*, s32, u32);
    extern void* memmove(void*, const void*, u32);
    extern s32 DVDOpen(const char*, MenuDVDFileInfo*);
    extern s32 DVDRead(MenuDVDFileInfo*, void*, s32, s32, s32);
    extern void DVDClose(MenuDVDFileInfo*);
    extern void fn_8009AAD4(void*, void*);
    extern void* fn_8009A9D8(void*, u32);
    MenuDVDFileInfo file;
    MenuDVDFileInfo readFile;
    u32 value;
    u32 aligned;
    u32 length;
    s32 readResult;
    s32 i;
    s32 bit;

    for (i = 0; i < 256; i++) {
        value = i;
        for (bit = 0; bit < 8; bit++) {
            if ((value & 1) != 0) {
                value = (value >> 1) ^ 0xEDB88320;
            } else {
                value >>= 1;
            }
        }
        lbl_803FAEF8[i] = value;
    }

    lbl_8047A64C = 0;
    strcpy(lbl_803FADF8, lbl_802EE608[OSGetTick() % 3]);
    if (DVDOpen(lbl_803FADF8, &file) == 0 || file.length == 0) {
        return;
    }

    aligned = (file.length + 0x5B) & ~0x1F;
    lbl_8047A650 = aligned;
    if (lbl_8047A648 != NULL) {
        fn_8009AAD4(lbl_80478980, lbl_8047A648);
    }
    lbl_8047A648 = fn_8009A9D8(lbl_80478980, aligned);
    if (lbl_8047A648 == NULL) {
        return;
    }
    memset(lbl_8047A648, 0, aligned);
    DVDClose(&file);

    if (lbl_8047A64C != 0 ||
        DVDOpen(lbl_803FADF8, &readFile) == 0 ||
        readFile.length == 0) {
        return;
    }
    length = readFile.length;
    readResult = DVDRead(&readFile, lbl_8047A648,
                         (length + 0x1F) & ~0x1F, 0, 2);
    if (readResult < 0) {
        return;
    }
    if ((u32)readResult < length) {
        return;
    }
    DVDClose(&readFile);

    memmove(lbl_8047A648 + 0x34, lbl_8047A648, length);
    memset(lbl_8047A648, 0, 0x34);
    memset(lbl_8047A648 + length + 0x34, 0,
           lbl_8047A650 - (length + 0x34));
    lbl_8047A64C = 1;
}

/* fn_8007B350 (0x8007B350): prepare the GBA upload context and worker. */
#pragma push
#pragma peephole off
void fn_8007B350(GbaBootContext* context, u32 channel, const u8* device_code,
                 u32 region, const u8* name, u32 variant, u32 flags) {
    extern u8* lbl_8047A648;
    extern u32 lbl_8047A650;
    extern void fn_8009F1D0(void* queue, void* messages, u32 count);
    extern void fn_8007B6A4(u8* context);
    extern void OSCreateThread(void* thread, void* entry, void* arg,
                               void* stack, u32 stack_size, u32 priority,
                               u32 attributes);
    extern void OSResumeThread(void* thread);
    u8* upload_start;
    u32 upload_size;
    u8* upload_end;
    u32 payload_size;
    u32 crc;
    u32 crc_limit;
    u32 i;

    memset(context, 0, (u8*)context->reply - (u8*)context);
    context->channel = (u8)channel;
    context->device_code = ((u32)device_code[0] << 24) |
                           ((u32)device_code[1] << 16) |
                           ((u32)device_code[2] << 8) | device_code[3];
    context->status_only = flags & 1;
    context->reject_upload = flags & 2;
    context->crc = *(const u32*)device_code;
    if (region == 0x4A) {
        context->game_code = 0x5053414A;
    } else {
        context->game_code = 0x50534145;
    }

    upload_start = lbl_8047A648;
    upload_end = upload_start + lbl_8047A650;
    context->upload_start = upload_start;
    context->upload_end = upload_end;
    upload_size = upload_end - upload_start;
    payload_size = upload_size - 0x3C;
    upload_start[0x2C] = payload_size;
    upload_start[0x2D] = payload_size >> 8;
    upload_start[0x2E] = payload_size >> 16;
    upload_start[0x2F] = payload_size >> 24;

    for (i = 0; i < 0x20; i++) {
        if (name[i] == 0) {
            break;
        }
        upload_start[i + 4] = name[i];
    }
    for (; i < 0x20; i++) {
        upload_start[i + 4] = 0;
    }

    upload_start[0x28] = 2;
    upload_start[0x29] = (u8)variant;
    upload_start[0x30] = 8;
    upload_start[0x31] = 0;
    upload_start[0x32] = 0;
    upload_start[0x33] = 0;
    memcpy(upload_end - 8, device_code, 4);
    memcpy(upload_end - 4, &context->crc, 4);

    crc = 0xAA478422;
    crc_limit = upload_size - 4;
    i = 0;
    do {
        crc = (crc >> 8) ^
              lbl_803FAEF8[(crc ^ upload_start[i++ + 4]) & 0xFF];
    } while (i != crc_limit);
    upload_start[0] = crc;
    upload_start[1] = crc >> 8;
    upload_start[2] = crc >> 16;
    upload_start[3] = crc >> 24;

    fn_8009F1D0(context->pad_35C, context->pad_35C + 0x20, 1);
    OSCreateThread(context->thread_and_work, (void*)fn_8007B6A4, context,
                   context->reply, 0x2000, 8, 0);
    OSResumeThread(context->thread_and_work);
}
#pragma pop

void fn_8007B6A4(u8* r3) {
    extern void fn_8007B6D8(u8* p);
    u8* r31 = r3;

    fn_8007B6D8(r31);
    *(u8*)(r31 + 0x345) = 0x1;
}

#pragma push
#pragma scheduling off
void fn_8007C23C(u8* r3) {
    OSResumeThread((u32)(r3 + 0x28));
}
#pragma pop

#pragma peephole off
int fn_80079C1C(s32 arg0, int arg1, int arg2, s32 arg3) {
    if ((u8)arg1 == 0 && (u8)arg2 == 0) {
        fn_801067E8(0x43D2, 1, 0);
        fn_801069FC(1);
        fn_801067E8(0x43D3, 1, 0);
        fn_801069FC(1);
        if (arg0 == 0) {
            fn_80106D3C(2, 0x44CF, 1, 0);
            fn_801069FC(1);
        }
        fn_80102510(0xEF);
        *(u32*)&lbl_8047A638 = 1;
        return 0;
    }
    if ((u8)arg1 == 0) {
        fn_801067E8(0x43D2, 1, 0);
        fn_801069FC(1);
        if (arg0 == 0) {
            fn_80106D3C(2, 0x44CF, 1, 0);
            fn_801069FC(1);
        }
        fn_80102510(0xEF);
        *(u32*)&lbl_8047A638 = 1;
        return 0;
    }
    if ((u8)arg2 == 0) {
        fn_801067E8(0x43D3, 1, 0);
        fn_801069FC(1);
        if (arg0 == 0) {
            fn_80106D3C(2, 0x44CF, 1, 0);
            fn_801069FC(1);
        }
        fn_80102510(0xEF);
        *(u32*)&lbl_8047A638 = 1;
        return 0;
    }
    if (lbl_8047A632 != 0) {
        fn_80132A38(0x2D, 0x47);
        fn_80165668(0x3CA, 0, 0xFF);
        switch (arg0) {
        case 1:
            fn_801067E8(0x43AD, 1, 0);
            break;
        case 0:
            fn_80132A38(0x4D, arg3);
            fn_801067E8(0x4436, 1, 0);
            break;
        }
        fn_801069FC(1);
    }
    if (lbl_8047A631 != 0) {
        fn_80165668(0x3D2, 0, 0xFF);
        switch (arg0) {
        case 1:
            fn_801067E8(0x4437, 1, 0);
            break;
        case 0:
            fn_80132A38(0x4D, arg3);
            fn_801067E8(0x443B, 1, 0);
            break;
        }
        fn_801069FC(1);
    }
    if (lbl_8047A630 != 0) {
        fn_80132A38(0x2D, 1);
        fn_80165668(0x3CA, 0, 0xFF);
        switch (arg0) {
        case 1:
            fn_801067E8(0x43AD, 1, 0);
            break;
        case 0:
            fn_80132A38(0x4D, arg3);
            fn_801067E8(0x4436, 1, 0);
            break;
        }
        fn_801069FC(1);
    }
    return 1;
}
#pragma peephole reset

extern u32 fn_800F7AF0(s32);
extern u32 fn_800F7BC4(s32);
extern u8 fn_8008ABA0(s32);
extern s32 menuCBBios_ControlerIDtoPortID(s32);
extern s32 fn_80073A44(s32, u16*);
extern s32 windowGetActiveID(void);
extern void* windowSearchID(s32);
extern void menuCloseCustom(s32, s32, s32);
extern u32 fn_800A13F8(void);
extern u8 lbl_803B6DE0[];
extern int fn_800D0F44();
extern f64 sin(f64);

u32 fn_80071208(s32 controller)
{
    u16 buttons;
    u32 input;
    s32 port;

    input = fn_800F7AF0(controller);
    input &= fn_800F7BC4(controller);
    if (input == 0 && fn_8008ABA0(controller) != 0) {
        port = menuCBBios_ControlerIDtoPortID(controller);
        if (fn_80073A44(port, &buttons) == 0) {
            if (buttons & 0x001) {
                input |= 0x100;
            }
            if (buttons & 0x002) {
                input |= 0x200;
            }
            if (buttons & 0x008) {
                input |= 0x1000;
            }
            if (buttons & 0x010) {
                input |= 0x2;
            }
            if (buttons & 0x020) {
                input |= 0x1;
            }
            if (buttons & 0x040) {
                input |= 0x8;
            }
            if (buttons & 0x080) {
                input |= 0x4;
            }
            if (buttons & 0x100) {
                input |= 0x20;
            }
            if (buttons & 0x200) {
                input |= 0x40;
            }
        }
    }
    return input;
}

s32 _menuPop_80071398(s32 target)
{
    u32 depth;
    s32 top;

    depth = *(u32*)(lbl_803B6D88 + 0x40);
    top = *(s32*)(lbl_803B6D88 + depth * 8);
    if (windowGetActiveID() == top) {
        menuCloseCustom(top, 0, 0);
    }
    if (windowSearchID(0xBE) != 0) {
        menuCloseCustom(0xBE, 0, 1);
    }
    *(s32*)(lbl_803B6D88 + depth * 8 + 4) = 0;

    if (depth != 0) {
        if ((s32)depth <= 0) {
            __assert((const char*)lbl_80268708, 0x5C,
                     (const char*)lbl_80268718);
        }
        depth--;
        *(u32*)(lbl_803B6D88 + 0x40) = depth;
        while (depth != 0 &&
               *(s32*)(lbl_803B6D88 + depth * 8) != target) {
            depth--;
            *(u32*)(lbl_803B6D88 + 0x40) = depth;
        }
    }
    return *(s32*)(lbl_803B6D88 + depth * 8);
}

static inline s32 menuReadGbaZeroResponse(s32 channel)
{
    u8 status[4];
    u32 response;
    u32 interruptState;
    u32 thread;
    s32 result;

    thread = fn_800A13F8();
    lbl_8047A600 = thread;
    OSCreateAlarm(lbl_803B6DE0);
    interruptState = OSDisableInterrupts();
    OSSetAlarm(lbl_803B6DE0, OSMillisecondsToTicks(1),
               (void*)fn_80072684);
    OSSuspendThread((void*)lbl_8047A600);
    OSRestoreInterrupts(interruptState);

    if (fn_800D0F44(channel) != 0x40000) {
        result = 1;
    } else if (GBAGetStatus(channel, status) != 0) {
        result = 2;
    } else if ((status[0] & 8) == 0) {
        result = -1;
    } else if (GBARead(channel, &response, status) != 0) {
        result = 3;
    } else if (response != 0) {
        result = 4;
    } else {
        result = 0;
    }
    if (result == 0 || result >= 3) {
        gbaCommandSetKeyState(channel + 1, 1);
    }
    return result;
}

#pragma peephole off
s32 fn_800719A8(s32 channel)
{
    return menuReadGbaZeroResponse(channel);
}
#pragma peephole reset

#pragma peephole off
s32 fn_80072548(s32 channel)
{
    return menuReadGbaZeroResponse(channel);
}
#pragma peephole reset

extern u32 lbl_8047A610;
extern f32 lbl_8047C098;
extern f32 lbl_8047C09C;
extern f32 lbl_8047C0A0;
extern f32 lbl_8047C0A4;
extern f32 lbl_8047C0A8;
extern f32 lbl_8047C0AC;
extern f32 lbl_8047C0C0;
extern f32 lbl_8047C0C4;
extern u8 menuModelCheck(void*, s32);
extern void* menuModelRender(void*);
extern void* fn_801DAC3C(void*);
extern void fn_800E3DC4(void*, f32*);
extern void fn_800D88DC(s32);
extern void fn_800D888C(s32);
extern void fn_800D6A00(s32);
extern void fn_800D7820(void*);
extern void fn_800D85D4(s32, void*);
extern void fn_800D67BC(s32);
extern void fn_800D61E4(s32, s32);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void fn_800D59B8(s32, f32, f32);
extern void fn_800D6728(void);
extern u8 lbl_80314F98[];

void fn_800753D0(void)
{
    u8* work;
    void* object;
    f32 direction[3];
    f32 frames;

    work = (u8*)lbl_8047A610;
    frames = (f32)fn_800D37CC();
    *(f32*)work = (f32)fn_800D3088() / frames;
    *(f32*)(work + 0x18C) =
        lbl_8047C098 + lbl_8047C09C * (f32)sin(*(f32*)(work + 4));
    if (*(f32*)(work + 0x18C) > lbl_8047C0A0) {
        *(f32*)(work + 0x18C) = lbl_8047C0A0;
    }

    if (menuModelCheck(work + 0x144, 0) == 0) {
        object = fn_801DAC3C(*(void**)(work + 0x168));
        if (object != 0) {
            *(f32*)(work + 8) += lbl_8047C0A4 * *(f32*)work;
            direction[0] = lbl_8047C0A8;
            direction[1] = lbl_8047C0A4 * *(f32*)work;
            direction[2] = lbl_8047C0A8;
            fn_800E3DC4(object, direction);
        }
    }

    *(f32*)(work + 4) += *(f32*)work;
    if (*(f32*)(work + 4) >= lbl_8047C0AC) {
        *(f32*)(work + 4) = lbl_8047C0A8;
    }
}

#pragma peephole off
void fn_80075518(u8* context, u8* message)
{
    s32 alpha;
    void* model;

    switch (*(s16*)(message + 6)) {
    case 0xD3C:
        break;
    case 0xD3D:
    default:
        return;
    }
    alpha = (s32)*(f32*)(lbl_8047A610 + 0x18C);
    model = menuModelRender((void*)(lbl_8047A610 + 0x144));
    if (model == 0) {
        return;
    }

    fn_800D88DC(3);
    fn_800D888C(4);
    fn_800D6A00(7);
    fn_800D7820(lbl_80314F98);
    fn_800D85D4(0, model);
    fn_800D67BC(2);
    fn_800D61E4(0, 0);
    fn_800D5CB8(0, 0x28, 0x3E, 0xC8, alpha);
    fn_800D59B8(0, lbl_8047C0A8, lbl_8047C0A8);
    fn_800D61E4(*(s16*)(message + 0x54), *(s16*)(message + 0x56));
    fn_800D5CB8(0, 0x28, 0x3E, 0xC8, alpha);
    fn_800D59B8(0, lbl_8047C0AC, lbl_8047C0AC);
    fn_800D6728();
}
#pragma peephole reset

extern u32 fn_800FF56C(void);
extern u16 fn_800E2C04(s32, s32);
extern void* fn_800E27B0(u16);
extern void* gamedataGetStatus(s32, s32);
extern u8 lbl_802EF0A8[];
extern void pokemonCreate(void*, s32, s32, void*);
extern void menuModelInit(void*, s32, s32);
extern void fn_80109C88(void*, void*);
extern s32 menuOpenCustom(s32, ...);

void fn_800756C8(s32 pokemonId)
{
    u8* work;
    u16 handle;

    if (fn_800FF56C() != 0x43) {
        return;
    }
    handle = fn_800E2C04(0x1A0, 0x20);
    if (handle != 0) {
        work = fn_800E27B0(handle);
    } else {
        work = 0;
    }
    lbl_8047A610 = (u32)work;
    pokemonCreate(work + 0xC, pokemonId, 0xA, gamedataGetStatus(0, 1));
    *(f32*)(work + 4) = lbl_8047C0A8;
    *(f32*)(work + 8) = lbl_8047C0A8;
    *(f32*)(work + 0x18C) =
        lbl_8047C098 + lbl_8047C09C * (f32)sin(*(f32*)(work + 4));
    if (*(f32*)(work + 0x18C) > lbl_8047C0A0) {
        *(f32*)(work + 0x18C) = lbl_8047C0A0;
    }
    menuModelInit(work + 0x144,
                  *(s16*)(lbl_802EF0A8 + 0x17296),
                  *(s16*)(lbl_802EF0A8 + 0x17298));
    fn_80109C88(work + 0x144, work + 0xC);
    menuOpenCustom(0xD8, 0, 0, 0, 0, 0);
}

extern int fn_801CB834();
extern void floorSetFadeScript(s32, u32);

void fn_80075DC8(void)
{
    u32 waitTicks;
    u32 elapsed;
    s32 destination;

    cameraPlayAnime((s32)fn_80113F48(), 0x0B561800, 0, 0);
    waitTicks = 1;
    if (fn_800D37CC() == 0x32) {
        waitTicks = (u32)lbl_8047C0C0;
        if (waitTicks < 1) {
            waitTicks = 1;
        }
    }
    elapsed = 0;
    while (elapsed < waitTicks) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    fn_801CB834(0x0B541000, 2, 0, 1);
    destination =
        menuOpenCustom(0xE2, 0, 0, 0x10, 1, 0) == 0 ? 0x321 : 0x384;
    fadeCheck(1);
    fadeSet(3, lbl_8047C0C4);
    floorLink(destination, 0);
    floorSetFadeScript(0, 0x05960008);
}

extern u8 lbl_80268A48[];
extern u8 lbl_80268A58[];

static inline u8 menuRuleCheckPokemonMode(void* pokemon, const s16* levels,
                                          s32 mode)
{
    extern s32 pokemonGetStatus(void*, s32, s32, s32);
    extern u8 pokemonBiosGetLevel(void*);
    extern u16 pokemonBiosGetItemDataId(void*);
    extern u8 fn_80142984(u16);
    MenuRuleItemRestrictions* restrictions;
    u16 item;
    u32 i;

    if (pokemon == 0 || pokemonGetStatus(pokemon, 0, 0x6E, 0) == 0) {
        return 1;
    }

    switch (mode) {
    case 0:
        return pokemonBiosGetLevel(pokemon) >= levels[0];
    case 1:
        return pokemonBiosGetLevel(pokemon) <= levels[1];
    case 2:
        item = pokemonBiosGetItemDataId(pokemon);
        restrictions = (MenuRuleItemRestrictions*)fn_8006B420();
        if (item == 0) {
            return 1;
        }
        if (item == 0xAF || fn_80142984(item) == 0) {
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
        __assert((const char*)lbl_80268A48, 0xFB,
                 (const char*)lbl_80268A58);
        return 0;
    }
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

u8 fn_800774D4(void* pokemon, const s16* levels, s32 mode)
{
    return menuRuleCheckPokemonMode(pokemon, levels, mode);
}
