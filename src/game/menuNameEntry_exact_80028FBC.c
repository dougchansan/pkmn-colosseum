/* Name-entry entry point, 0x80028FBC-0x80029558.
 * RULE-EXCEPTION(user-approved): retained widened choice local;
 * see docs/RULE_EXCEPTIONS.md. Compiler controls are at unit scope.
 */
#include "dolphin/types.h"

typedef struct NameEntryChoiceList {
    u8 pad00[8];
    s32* choices;
    s32 count;
} NameEntryChoiceList;

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
