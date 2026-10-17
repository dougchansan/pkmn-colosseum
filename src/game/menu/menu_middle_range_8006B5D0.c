/**
 * @file menu_middle_range_8006B5D0.c
 * @brief Trainer setup, 0x8006B5D0 - 0x8006B6B4.
 */
#include "dolphin/types.h"
#include "game/menu/menu_middle.h"

extern void menuCBBiosCreateTrainerHero(void* destination, u16 trainerId);
extern void* savedataGetStatus(s32 index, s32 kind);
extern void menuCB_InitMenu(s32 menuId);
extern void* memcpy(void* destination, const void* source, u32 size);
extern void* lbl_8047A5A4;
extern u8 lbl_8047A5E0;
extern const u16 lbl_8047C038[4];
extern const u32 lbl_80267DD8[4];

void menuColosseumBattleSetup(MenuMiddleWork* work)
{
    u32 i;

    lbl_8047A5A4 = savedataGetStatus(0, 0xE);
    if (lbl_8047A5E0 == 0) {
        for (i = 0; i < 4; i++) {
            u32 trainerKind;
            u32 controllerId;

            menuCBBiosCreateTrainerHero(&work->battleSlots[i], lbl_8047C038[i]);
            trainerKind = lbl_80267DD8[i];
            controllerId = i & 0xFFFF;
            work->battleSlots[i].inputDevice = trainerKind;
            work->battleSlots[i].controllerId = controllerId;
            memcpy(&work->partySlots[i], &work->battleSlots[i], sizeof(MenuMiddleTrainerSlot));
        }

        switch (work->ruleMode) {
        case 3:
            menuCB_InitMenu(0xAF);
            work->randomTableIndex = 0;
            break;
        case 0:
        default:
            menuCB_InitMenu(0xA8);
            work->randomTableIndex = 4;
            break;
        }
    }
    lbl_8047A5E0 = 0;
}
