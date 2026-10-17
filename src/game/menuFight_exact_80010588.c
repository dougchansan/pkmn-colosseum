/** Exact menuFight move-control block, 0x80010588 - 0x80010C98. */
#include "dolphin/types.h"

extern void* menuSubCalcColor(void*, void*);
extern void windowDrawSprite();
extern void msgctrlSetValue();
extern void menuItemBiosSetSelectFlag();
extern void* memcpy(void*, const void*, u32);
extern u16 lbl_80478850[4];

void menuFightDrawWaza(u8* arg1, u8* arg2)
{
    typedef struct {
        u32 unk_00;
        u32 value;
        u32 unk_08;
    } NpcInteractEntry;
    extern NpcInteractEntry* windowGetAllocPtr(u8* a);
    extern void* windowGetFreeWork(u8* a);
    extern void GSmsgPrintColor(s32 a, s32 b, s32 c, u32 d);
    NpcInteractEntry* participant;
    void* npc_data;
    s32 idx;
    s16 npc_id;
    s32 offset;
    u32 result;
    participant = windowGetAllocPtr(arg1);
    npc_data = windowGetFreeWork(arg1);
    npc_id = *(s16*)(arg2 + 6);
    idx = 0;
    switch (npc_id) {
    case 0xC4:
        idx = 0;
        break;
    case 0xC5:
        idx = 1;
        break;
    case 0xC6:
        idx = 2;
        break;
    case 0xC7:
        idx = 3;
        break;
    }
    if ((s8)*((u8*)npc_data + 2) == idx) {
        windowDrawSprite(0, 0, arg1, 0x49, 0);
        windowDrawSprite(0, 0, arg1, 0x4A, 0);
    }
    offset = idx * 0xc;
    participant = (NpcInteractEntry*)((u32)participant + offset);
    result = participant->value;
    if (result != 0) {
        msgctrlSetValue(0x37, result);
        GSmsgPrintColor(0, 0, (s32)menuSubCalcColor(arg1, arg2), 0xE7);
    }
}

void menuFightDrawCmdMsg(u8* arg1, u8* arg2)
{
    extern u32 windowGetAllocPtr(u8* a);
    extern u32 windowGetFreeWork(u8* a);
    extern u32 windowGetParam(u8* a, s32 b);
    extern void msgctrlSetValue(s32 p1, s32 val);
    extern void GSmsgPrintColor(s32 a, s32 b, s32 c, u32 d);
    extern void* fightOutPokemonGetNicknamePtr(void* a);
    void* participant;
    void* npc_data;
    u32 r30;
    s16 npc_id;
    s32 sub;
    s32 val;
    u8* p1;
    u32 t1;
    u32 t2;
    npc_id = *(s16*)(arg2 + 6);
    r30 = 0;
    switch (npc_id) {
    case 0xb4:
        t1 = windowGetAllocPtr(arg1);
        msgctrlSetValue(0x37, (s32)t1);
        r30 = 0xcf;
        break;
    case 0x11cd:
        t2 = windowGetAllocPtr(arg1);
        msgctrlSetValue(0x36, (s32)t2);
        r30 = 0x196;
        break;
    case 0xc1:
        participant = (void*)windowGetAllocPtr(arg1);
        npc_data = (void*)windowGetFreeWork(arg1);
        sub = (s32)*(u8*)npc_data;
        switch (sub) {
        case 0:
            msgctrlSetValue(0x36, *(s32*)participant);
            r30 = 0x196;
            break;
        case 1:
            r30 = 0x199;
            break;
        }
        break;
    case 0x11d8:
        r30 = 0x196;
        val = (s32)*(u32*)(arg1 + 4);
        if (val == 0xf7) {
            msgctrlSetValue(0x36, *(s32*)windowGetAllocPtr(arg1));
        } else if (val == 0xf8) {
            p1 = (u8*)windowGetParam(arg1, 1);
            if ((u8)windowGetParam(arg1, 2) != 0) {
                msgctrlSetValue(0x36, (s32)fightOutPokemonGetNicknamePtr(p1));
            } else {
                r30 = 0x1a9;
            }
        } else {
            return;
        }
        break;
    }
    if (r30 != 0) {
        GSmsgPrintColor(0, -2, (s32)menuSubCalcColor(arg1, arg2), r30);
    }
}

u32 menuFightWazaButton(u8* ctx)
{
    typedef struct MenuWindow {
        u8 pad_00[0x95];
        s8 selectedWaza;
        u8 pad_96[2];
        u8 done;
        u8 canceled;
    } MenuWindow;
    typedef struct MenuFightWazaStatus {
        u8 pad_00[0x40];
        void* outPokemon;
    } MenuFightWazaStatus;
    typedef struct MenuFightWazaWork {
        u8 mode;
        u8 unk_01;
        s8 selectedSlot;
    } MenuFightWazaWork;
    typedef struct MenuKeyInfo {
        u8 pad_00[4];
        u16 buttons;
    } MenuKeyInfo;
    extern MenuFightWazaStatus* windowGetAllocPtr(u8* a);
    extern MenuFightWazaWork* windowGetFreeWork(u8* a);
    extern MenuKeyInfo* windowGetKeyInfo(void);
    extern u32 fightOutPokemonGetPokemonPtr(void*);
    extern u32 pokemonGetStatus();
    extern void menuButtonNormal(u8* a);
    extern void pokemonWazaReplace(void*, s32, s32);
    extern void pokemonToMenuWazaStatus(void*, MenuFightWazaStatus*);
    MenuWindow* window;
    MenuFightWazaStatus* participant;
    MenuFightWazaWork* state;
    MenuKeyInfo* flagsObj;
    u32 model;
    u16 flags;
    void* saved;

    window = (MenuWindow*)ctx;
    participant = windowGetAllocPtr(ctx);
    state = windowGetFreeWork(ctx);
    model = fightOutPokemonGetPokemonPtr(participant->outPokemon);
    flagsObj = windowGetKeyInfo();
    flags = flagsObj->buttons;

    switch (state->mode) {
    case 0:
        if (model != 0 && ((u16)pokemonGetStatus(model, 0, 0x7F, window->selectedWaza) == 0x165)) {
            if ((windowGetKeyInfo()->buttons & 0x20) != 0) {
                window->done = 1;
                window->canceled = 1;
            }
        } else {
            menuButtonNormal(ctx);
        }
        break;
    case 1: {
        s32 flagBits = flags & 0xFFFF;
        if ((flagBits & 0xD0) != 0) {
            if (model != 0) {
                pokemonWazaReplace((void*)model, state->selectedSlot, window->selectedWaza);
                saved = participant->outPokemon;
                pokemonToMenuWazaStatus((void*)model, participant);
                participant->outPokemon = saved;
            }
            state->selectedSlot = -1;
            state->mode = 0;
        } else if ((flagBits & 0x20) != 0) {
            state->selectedSlot = -1;
            state->mode = 0;
        }
        break;
    }
    }
    return 0;
}

u32 menuFightWazaCtrl(u8* ctx)
{
    typedef struct MenuWindow {
        u8 pad_00;
        s8 state;
        u8 pad_02[2];
        u32 menuId;
        u8 pad_08[0x78];
        s32 selected;
    } MenuWindow;
    typedef struct MenuFightWazaRow {
        u32 unk_00;
        u32 wazaId;
        u32 unk_08;
    } MenuFightWazaRow;
    typedef struct MenuFightWazaWork {
        u8 mode;
        u8 unk_01;
        s8 selectedSlot;
    } MenuFightWazaWork;
    extern void* windowAllocMemory(u8* a, u32 size);
    extern u32 windowGetParam(u8* a, s32 b);
    extern MenuFightWazaWork* windowGetFreeWork(u8* a);
    extern MenuFightWazaRow* windowGetAllocPtr(u8* a);
    extern s32 menuGetCursorItemID(u32 val);
    MenuWindow* window;
    void* dst;
    MenuFightWazaWork* state;
    MenuFightWazaRow* party;
    s32 i;
    s32 id;

    window = (MenuWindow*)ctx;
    if (window->state == 0) {
        dst = windowAllocMemory(ctx, 0x48);
        if (dst != NULL) {
            memcpy(dst, (void*)windowGetParam(ctx, 0), 0x48);
        }
        state = windowGetFreeWork(ctx);
        state->mode = 0;
        state->unk_01 = 0;
        state->selectedSlot = -1;
    }

    party = windowGetAllocPtr(ctx);
    if (window->state == 5) {
        for (i = 0; i < 4; i++) {
            menuItemBiosSetSelectFlag(lbl_80478850[i], 1);
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (party[i].wazaId != 0) {
                menuItemBiosSetSelectFlag(lbl_80478850[i], 1);
            } else {
                menuItemBiosSetSelectFlag(lbl_80478850[i], 0);
            }
        }
    }

    id = menuGetCursorItemID(window->menuId);
    switch (id) {
    case 0xC4:
        window->selected = 0;
        break;
    case 0xC5:
        window->selected = 1;
        break;
    case 0xC6:
        window->selected = 2;
        break;
    case 0xC7:
        window->selected = 3;
        break;
    default:
        window->selected = -1;
        break;
    }
    return 0;
}

u32 menuFightMainCtrl(u8* arg)
{
    extern void* windowAllocMemory(u8* a, u32 size);
    extern void* windowGetAllocPtr(u8* a);
    extern s32 menuGetCursorItemID(u32 val);
    extern u8* windowSearchItemID(u8* a, s32 id);
    extern void menuItemBiosSetSelectFlag(s32 id, s32 flag);
    void* entry;
    void* participant;
    s32 trainer_id;
    u8* r;
    if ((s8)arg[1] == 0) {
        entry = windowAllocMemory(arg, 0x18);
        if (entry != NULL) {
            memcpy(entry, *(void**)(arg + 0x60), 0x18);
        }
        if (*(u8*)((u8*)entry + 0x16) != 0) {
            r = windowSearchItemID(arg, 0xB6);
            *(s32*)(r + 0x4C) = 0x13D;
            r = windowSearchItemID(arg, 0xB8);
            *(s32*)(r + 0x4C) = 0x140;
            menuItemBiosSetSelectFlag(0xB8, 1);
        } else {
            r = windowSearchItemID(arg, 0xB6);
            *(s32*)(r + 0x4C) = 0x13F;
            r = windowSearchItemID(arg, 0xB8);
            *(s32*)(r + 0x4C) = 0;
            menuItemBiosSetSelectFlag(0xB8, 0);
        }
    }
    participant = windowGetAllocPtr(arg);
    trainer_id = menuGetCursorItemID(*(u32*)(arg + 4));
    switch (trainer_id) {
    case 0xB5:
        *(s32*)(arg + 0x80) = 0;
        break;
    case 0xB6:
        if (*(u8*)((u8*)participant + 0x16) != 0) {
            *(s32*)(arg + 0x80) = 1;
        } else {
            *(s32*)(arg + 0x80) = 3;
        }
        break;
    case 0xB7:
        *(s32*)(arg + 0x80) = 2;
        break;
    case 0xB8:
        *(s32*)(arg + 0x80) = 3;
        break;
    default:
        *(s32*)(arg + 0x80) = -1;
        break;
    }
    return 0;
}
