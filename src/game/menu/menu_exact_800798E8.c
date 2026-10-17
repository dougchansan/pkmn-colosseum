/**
 * @file menu_exact_800798E8.c
 * @brief Commit an e-card shop transaction to savedata.
 */
#include "dolphin/types.h"

typedef struct MenuSaveSnapshot {
    u8 bytes[0x1DFD0];
} MenuSaveSnapshot;

extern u8 lbl_8047A630;
extern u8 lbl_8047A631;
extern u8 lbl_8047A632;
extern u8 lbl_8047A633;
extern u8 lbl_8047A634;
extern u8 lbl_8047A635;
extern u32 lbl_8047A638;
extern s32 std011_dialogDisplayYesNoMenu(void);
extern s32 fn_80075A9C(void);
extern u8 fn_80075AC0(void);
extern s32 fn_80075AE4(void);
extern u8 fn_80075B08(void);
extern s32 fn_80075B2C(void);
extern u8 fn_80075B50(void);
extern u8 fn_80079EF4(s32, u32);
extern s32 fn_801D0748(u32, u32, u32);
extern u32 exDiscCouponGive();
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
extern void winMsgOpenField(s32, s32, s32);
extern void winMsgOpen(s32, s32, s32, s32);
extern void winMsgClose(s32);
extern s32 menuClose(s32);

void exDiscCouponMenu(void* backup)
{
    u32 canBag;
    void* pcbox;
    s32 saveResult;
    u32 canParty;
    u8 i;
    void* hero;
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
    if ((u8)exDiscCouponGive(1, canBag, canParty, pokemon) == 0) {
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
    choice = (s8)std011_dialogDisplayYesNoMenu();
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
