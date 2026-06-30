/**
 * @file gs_pcbox.c
 * @brief GSpcbox -- PC Box Pokemon storage system UI.
 *
 * Address range: 0x800181C4 - 0x80020328 (~60 functions)
 *
 * This module implements the PC Box storage system where players can
 * deposit, withdraw, and organize their Pokemon collection. It handles:
 *   - Box list display with Pokemon icons
 *   - Deposit/withdraw operations
 *   - Move between boxes
 *   - Box wallpaper/name customization
 *   - Multi-select operations
 *   - Party <==> Box transfers
 *
 * Key functions:
 *   fn_800181C4  GSpcbox_SelectPokemon       -- 0x3D0 bytes, Pokemon selection in box
 *   fn_80018594  GSpcbox_DrawBoxGrid          -- 0x34C bytes, render box grid icons
 *   fn_800188E0  GSpcbox_ProcessInput         -- 0x188 bytes, D-pad/stick navigation
 *   fn_80018A68  GSpcbox_OperationDispatch    -- 0x4C8 bytes, deposit/withdraw/move
 *   fn_80018F30  GSpcbox_GetCurrentBox        -- 0x24 bytes, return active box index
 *   fn_80018F54  GSpcbox_SetCurrentBox        -- 0x34 bytes, switch active box
 *   fn_80018F88  GSpcbox_GetBoxPokemonCount   -- 0xDC bytes, count Pokemon in box
 *   fn_80019064  GSpcbox_IsBoxFull            -- 0x0C bytes, check if box at capacity
 *   fn_80019070  GSpcbox_GetEmptySlot         -- 0x68 bytes, find first empty slot
 *   fn_800190D8  GSpcbox_IsSlotOccupied       -- 0x40 bytes, check if slot has Pokemon
 *   fn_80019118  GSpcbox_Deposit              -- 0xEC bytes, move party -> box
 *   fn_80019204  GSpcbox_Withdraw             -- 0xA4 bytes, move box -> party
 *   fn_800192A8  GSpcbox_Move                 -- 0x228 bytes, move between boxes
 *   fn_800194D0  GSpcbox_GetSlotSpecies       -- 0x14 bytes, get species at box slot
 *   fn_800194E4  GSpcbox_GetSlotData          -- 0xFC bytes, get full data for slot
 *   fn_800195E0  GSpcbox_DrawSlotIcon         -- 0xA0 bytes, render Pokemon mini icon
 *   fn_80019680  GSpcbox_DrawCursor           -- 0xD4 bytes, render selection cursor
 *   fn_80019754  GSpcbox_AnimateCursor        -- 0x1E4 bytes, cursor movement animation
 *   fn_80019938  GSpcbox_DrawBoxLabel         -- 0xBC bytes, render box name/number
 *   fn_800199F4  GSpcbox_DrawOperationMenu    -- 0x128 bytes, "Deposit/Withdraw/Move" menu
 *   fn_80019B1C  GSpcbox_GetOperation         -- 0x2C bytes, return selected operation
 *   fn_80019B48  GSpcbox_DrawItemInfo         -- 0x214 bytes, show held item info
 *   fn_80019D5C  GSpcbox_DrawMarkings         -- 0x210 bytes, show/edit markings
 *   fn_80019F6C  GSpcbox_BoxSwitchAnimation   -- 0xA18 bytes, box change animation
 *   fn_8001A984  GSpcbox_DrawWallpaper        -- 0x114 bytes, render box wallpaper
 *   fn_8001AA98  GSpcbox_ChangeWallpaper      -- 0xD8 bytes, wallpaper selection
 *   fn_8001AB70  GSpcbox_RenameBox            -- 0x3D4 bytes, box name entry
 *   fn_8001AF44  GSpcbox_DrawPokemonList      -- 0x240 bytes, party list sidebar
 *   fn_8001B184  GSpcbox_GetPartySlotState    -- 0x68 bytes, get party slot info
 *   fn_8001B1EC  GSpcbox_MainStateMachine     -- 0x8D8 bytes, main PC box state machine
 *   fn_8001BAC4  GSpcbox_TransitionState      -- 0x228 bytes, state transition handler
 *   fn_8001BCEC  GSpcbox_CheckCanDeposit      -- 0x50 bytes, validate deposit allowed
 *   fn_8001BD3C  GSpcbox_CheckCanWithdraw     -- 0x44 bytes, validate withdraw allowed
 *   fn_8001BD80  GSpcbox_CheckPartySpace      -- 0x74 bytes, check party has room
 *   fn_8001BDF4  GSpcbox_CheckLastPokemon     -- 0x44 bytes, prevent depositing last mon
 *   fn_8001BE38  GSpcbox_ShowConfirmDialog    -- 0x84 bytes, confirmation prompt
 *   fn_8001BEBC  GSpcbox_Init                 -- 0x1A8 bytes, full initialization
 *
 * fn_800181C4 (GSpcbox_SelectPokemon) implements the core selection logic:
 *   - Takes party slot index, species ID, and box position as parameters
 *   - Looks up the species data via fn_801440A0 / fn_80143FFC
 *   - If the count (from fn_80143FFC) is 0, shows "no Pokemon here" message
 *     via fn_8002A0B8 (format text) and fn_80106ADC (display)
 *   - Otherwise, opens the detail view with move list, stats, etc.
 *   - References sSummaryPageEntries event/page table with 0x4C stride per entry
 *
 * fn_8001B1EC (GSpcbox_MainStateMachine) is the largest function (0x8D8 bytes):
 *   - Manages box browse, select, deposit, withdraw, and move states
 *   - Uses lbl_803A1D40 as the box data array base address
 *   - Uses lbl_802E4E58 as the Pokemon slot layout table
 *   - Iterates over 6 party slots with a 0x30-byte stride per slot
 *   - Calls fn_801080CC to set dialog/UI display states
 *
 * SDA globals:
 *   lbl_8047A2A8-A2F8: PC box state variables
 *   lbl_8047A308: Current cursor position (s16)
 *   lbl_80478898: Camera angle for box view (float)
 *
 * BSS globals:
 *   lbl_803A1D40: Box data array (30 Pokemon per box, 14 boxes)
 *   lbl_803A2688: Party data working copy
 *
 * Rodata:
 *   lbl_802E4E58: Pokemon slot position table for box grid rendering
 */

#include "dolphin/types.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

/* Pokemon data */
extern void  fn_80129BC8(void* pokeData, u8 fieldId, u16* outCount,
                          s32 p4, s32 p5, s32 p6);
extern void  fn_801297D8(void* pokeData, u16* outCount, s32 p3, s32 p4, s32 p5);
extern u8    fn_801429E8(void* fieldData);
extern u16   fn_80143C50(void* fieldData);
extern u16   fn_801440A0(u16 speciesId);
extern u16   fn_80143FFC(void);

/* Text formatting */
extern void  fn_8002A0B8(void* outBuf, void* fmt, s32 p3, s32 p4,
                          u16 p5, s32 p6, ...);
extern s32   fn_8012A5B0(void* partyData, s32 slot, s32 p3);

/* Dialog/rendering */
extern void  fn_80106ADC(s32 p1, void* text, s32 p3, s32 p4, u8 p5);
extern void  fn_801069FC(s32 slot);
extern void  fn_801080CC(void* ctx, s32 state);
extern void  fn_8005D95C(s16 npcId, u16* outX, u16* outY);
extern void  fn_8005D9AC(s16 x, s16 y, s16 z);
extern void* fn_8005DA18(void* data);

/* =========================================================================
 * BSS data references
 * ========================================================================= */

/* lbl_803A1D40: PC box storage array
 * 14 boxes * 30 slots = 420 Pokemon slots
 * Each slot is a Pokemon data structure */

/* lbl_803A2688: Working party data copy
 * Used during box operations to avoid corrupting the party until commit */

/* =========================================================================
 * Stubs for remaining GSpcbox functions (0x8001C064-0x8001FD48)
 * ========================================================================= */

/* 0x8001C064 | 0x754 */
extern void fn_801906A0(void);
extern void fn_80129280(void);
extern void fn_8006AEEC(void);
extern void fn_801F2A7C(void);
extern void fn_801FB1C0(void);
extern void fn_8012AC08(void);
extern void fn_801F986C(void);
extern void fn_8012640C(void);
extern void fn_80123FBC(void);
extern void fn_80132A38();
extern void fn_801230E0(void);
extern void fn_801298B8(void);
extern void fn_80106D3C();
extern void fn_80129A78(void);
extern void fn_80123110(void);
extern void fn_801299C8(void);
extern void fn_8011FDC8(void);
extern void fn_80120FE0(void);
extern void* memset(void* dst, int val, u32 n);
extern u8 lbl_803A1D40[];
extern u8 lbl_803A1C20[];
extern u8 lbl_802E4EB8[];
extern s8 fn_8001E074(u8, s16, s16, u32);
#if 0
asm void fn_8001C064(void) {
#include "src/game/gs_pcbox_fn_8001C064.inc"
}
#else
void fn_8001C064(void) {
    extern u8 lbl_802E4EB8[];
    extern u8 lbl_803A1C20[];
    extern u8 lbl_803A1D40[];
    extern s8 fn_8001E074(u8, s16, s16, u32);
    extern void fn_8006AEEC();
    extern void fn_80106D3C();
    extern void fn_8011FDC8();
    extern void fn_80120FE0();
    extern void fn_801230E0();
    extern void fn_80123110();
    extern void fn_80123FBC();
    extern void fn_8012640C();
    extern void fn_80129280();
    extern void fn_801298B8();
    extern void fn_801299C8();
    extern void fn_80129A78();
    extern void fn_8012AC08();
    extern void fn_80132A38();
    extern void fn_801906A0();
    extern void fn_801F2A7C();
    extern void fn_801F986C();
    extern void fn_801FB1C0();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r28 = r4;
    r30 = r5;
    r29 = r6;
    r4 = (u32)lbl_803A1D40;
    r27 = 0x0;
    r31 = (u32)lbl_803A1D40;
    r5 = *(u32*)((u8*)r31 + 0x8);
    r4 = *(u32*)((u8*)r31 + 0xC);
    if ((s32)r5 != 1) {
        if ((s32)r5 < 1) {
            if ((s32)r5 < 0) {
                goto L_8001C1EC;
            }
            if ((s32)r5 >= 3) goto L_8001C1EC;
            goto L_8001C1D8;
            }
        r25 = (s8)r3;
        tmp = r25 & 0xFFFF;
        if (tmp >= 6) {
            goto L_8001C204;
        }
        if ((s32)r5 != 1) {
            if ((s32)r5 >= 1) goto L_8001C148;
            if ((s32)r5 < 0) {
                goto L_8001C148;
            }
            r3 = 0x8ae;
            fn_801906A0();
            if (r3 == 0) {
                r3 = 0x0;
                r4 = 0x2;
                fn_80129280();
                goto L_8001C14C;
            }
            fn_8006AEEC();
            goto L_8001C14C;
        }
        if (r4 == 0) {
            r3 = 0x0;
            fn_801F2A7C();
            r4 = r3;
        }
        if (r4 == 0) {
            r3 = 0x0;
            goto L_8001C14C;
        }
        r3 = r4;
        r4 = 0x0;
        r5 = 0x44;
        r6 = 0x0;
        fn_801FB1C0();
        goto L_8001C14C;
    L_8001C148:
        r3 = 0x0;
    L_8001C14C:
        if (r3 == 0) {
            r27 = 0x0;
            goto L_8001C204;
        }
        r4 = r25;
        fn_8012AC08();
        r27 = r3;
        goto L_8001C1EC;
    }
    r25 = (s8)r3;
    tmp = r25 & 0xFFFF;
    if (tmp >= 6) {
        goto L_8001C204;
    }
    if (r4 == 0) {
        r3 = 0x0;
        fn_801F2A7C();
        r4 = r3;
    }
    if (r4 == 0) {
        r27 = 0x0;
        goto L_8001C204;
    }
    r3 = r4;
    r4 = r25;
    fn_801F986C();
    if (r3 == 0) {
        r27 = 0x0;
        goto L_8001C204;
    }
    r4 = 0x0;
    r5 = 0xcc;
    r6 = 0x0;
    fn_8012640C();
    r27 = r3;
    goto L_8001C1EC;
L_8001C1D8:
    tmp = (s8)r3;
    tmp = tmp & 0xFFFF;
    if (tmp >= 0x1e) {
        goto L_8001C204;
    }
L_8001C1EC:
    r3 = r27;
    fn_80123FBC();
    tmp = r3 & 0xFF;
    if (tmp == 0) {
        r27 = 0x0;
    }
L_8001C204:
    r3 = (u32)lbl_803A1D40;
    r4 = (u32)lbl_803A1C20;
    r5 = (u32)lbl_803A1D40;
    r3 = 0x32;
    r5 = *(u8*)((u8*)r5 + 0x6);
    tmp = (u32)lbl_803A1C20;
    r4 = (s8)r5;
    r4 = r4 * 0x30;
    r4 = tmp + r4;
    fn_80132A38();
    r3 = r27;
    fn_801230E0();
    tmp = *(u32*)((u8*)r31 + 0x8);
    r26 = r3;
    r4 = *(u32*)((u8*)r31 + 0xC);
    if ((s32)tmp != 1) {
        if ((s32)tmp >= 1) goto L_8001C2BC;
        if ((s32)tmp < 0) {
            goto L_8001C2BC;
        }
        r3 = 0x8ae;
        fn_801906A0();
        if (r3 == 0) {
            r3 = 0x0;
            r4 = 0x2;
            fn_80129280();
            goto L_8001C2C0;
        }
        fn_8006AEEC();
        goto L_8001C2C0;
    }
    if (r4 == 0) {
        r3 = 0x0;
        fn_801F2A7C();
        r4 = r3;
    }
    if (r4 == 0) {
        r25 = 0x0;
        goto L_8001C2C4;
    }
    r3 = r4;
    r4 = 0x0;
    r5 = 0x44;
    r6 = 0x0;
    fn_801FB1C0();
    goto L_8001C2C0;
L_8001C2BC:
    r3 = 0x0;
L_8001C2C0:
    r25 = r3;
L_8001C2C4:
do {
    if (r25 == 0) {
        r3 = -0x1;
        return;
    }
    tmp = r30 & 0xFFFF;
    if (tmp == 0) {
        tmp = r26 & 0xFFFF;
        if (tmp != 0) {
            r3 = r25;
            r4 = r26;
            fn_801298B8();
            if ((s32)r3 <= 0) {
                r3 = 0x2;
                r4 = 0x2b6b;
                r5 = 0x1;
                r6 = 0x1;
                fn_80106D3C();
                break;
            }
            r3 = r25;
            r4 = r26;
            r5 = 0x1;
            r6 = -0x1;
            fn_80129A78();
            r3 = r27;
            r4 = 0x0;
            r5 = 0x0;
            fn_80123110();
            if (r29 != 0) {
                *(u16*)((u8*)r29 + 0x0) = r3;
            }
            r4 = r3 & 0xFFFF;
            r3 = 0x2d;
            fn_80132A38();
            r3 = 0x2;
            r4 = 0x2b69;
            r5 = 0x1;
            r6 = 0x1;
            fn_80106D3C();
            break;
        }
        r3 = 0x2;
        r4 = 0x2b6a;
        r5 = 0x1;
        r6 = 0x1;
        fn_80106D3C();
        break;
    }
    r4 = r26 & 0xFFFF;
    if (r4 != 0) {
        r3 = 0x2d;
        fn_80132A38();
        r3 = 0x2;
        r4 = 0x2b66;
        r5 = 0x1;
        r6 = 0x0;
        fn_80106D3C();
        r3 = 0x0;
        r4 = -0x1;
        r5 = -0x1;
        r6 = 0x0;
        r24 = fn_8001E074((u8)r3, (s16)r4, (s16)r5, (u32)r6);
        r3 = 0x1;
        ((void(*)(void))fn_801069FC)();
        if ((s32)r24 != 0) {
            r3 = -0x1;
            return;
        }
        r3 = r25;
        r4 = r30;
        r6 = r28;
        r5 = 0x1;
        fn_801299C8();
        if ((s32)r3 != 0) {
            r3 = 0x2;
            r4 = 0x2b6b;
            r5 = 0x1;
            r6 = 0x1;
            fn_80106D3C();
            break;
        }
        r3 = r25;
        r4 = r26;
        fn_801298B8();
        if ((s32)r3 <= 0) {
            r4 = (u32)lbl_803A1D40;
            r3 = r25;
            r5 = (u32)lbl_803A1D40;
            r4 = r30;
            r6 = *(u8*)((u8*)r5 + 0x11);
            r5 = 0x1;
            fn_80129A78();
            r3 = 0x2;
            r4 = 0x2b6b;
            r5 = 0x1;
            r6 = 0x1;
            fn_80106D3C();
            break;
        }
        r3 = r25;
        r4 = r26;
        r5 = 0x1;
        r6 = -0x1;
        fn_80129A78();
        r3 = r27;
        r4 = 0x0;
        r5 = 0x0;
        fn_80123110();
        r26 = r3;
        if (r29 != 0) {
            *(u16*)((u8*)r29 + 0x0) = r26;
        }
        r3 = r27;
        r4 = r30;
        r5 = 0x1;
        fn_80123110();
        r4 = r26 & 0xFFFF;
        r3 = 0x2d;
        fn_80132A38();
        r4 = r30 & 0xFFFF;
        r3 = 0x2e;
        fn_80132A38();
        r3 = 0x2;
        r4 = 0x2b67;
        r5 = 0x1;
        r6 = 0x0;
        fn_80106D3C();
        break;
    }
    r3 = r25;
    r4 = r30;
    r6 = r28;
    r5 = 0x1;
    fn_801299C8();
    if ((s32)r3 == 0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x1;
        fn_80123110();
        r4 = r30 & 0xFFFF;
        r3 = 0x2d;
        fn_80132A38();
        r3 = 0x2;
        r4 = 0x2b68;
        r5 = 0x1;
        r6 = 0x0;
        fn_80106D3C();
    }
} while (0);
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = (u32)lbl_803A1C20;
    r4 = 0x0;
    r3 = (u32)lbl_803A1C20;
    r5 = 0x120;
    memset((void*)r3, (int)r4, (u32)r5);
    r27 = 0x0;
    r3 = (u32)lbl_803A1C20;
    r28 = (u32)lbl_803A1C20;
    while (1) {
    do {
        tmp = r27 & 0xFFFF;
    do {
        if (tmp >= 6) break;
        r4 = r27 & 0xFFFF;
        r5 = *(u32*)((u8*)r31 + 0x8);
        tmp = r4 * 0x30;
        r3 = *(u32*)((u8*)r31 + 0xC);
        r25 = 0x0;
        r29 = r28 + tmp;
        if ((s32)r5 != 1) {
            if ((s32)r5 < 1) {
                if ((s32)r5 < 0) {
                    break;
                }
                if ((s32)r5 >= 3) break;
                goto L_8001C680;
                }
            if (r4 >= 6) {
                break;
            }
            if ((s32)r5 != 1) {
                if ((s32)r5 >= 1) goto L_8001C600;
                if ((s32)r5 < 0) {
                    goto L_8001C600;
                }
                r3 = 0x8ae;
                fn_801906A0();
                if (r3 == 0) {
                    r3 = 0x0;
                    r4 = 0x2;
                    fn_80129280();
                    goto L_8001C604;
                }
                fn_8006AEEC();
                goto L_8001C604;
            }
            if (r3 == 0) {
                r3 = 0x0;
                fn_801F2A7C();
            }
            if (r3 == 0) {
                r3 = 0x0;
                goto L_8001C604;
            }
            r4 = 0x0;
            r5 = 0x44;
            r6 = 0x0;
            fn_801FB1C0();
            goto L_8001C604;
        L_8001C600:
            r3 = 0x0;
        L_8001C604:
            if (r3 == 0) {
                r25 = 0x0;
                break;
            }
            r4 = r27;
            fn_8012AC08();
            r25 = r3;
            break;
        }
        if (r4 >= 6) {
            break;
        }
        if (r3 == 0) {
            r3 = 0x0;
            fn_801F2A7C();
        }
        if (r3 == 0) {
            r25 = 0x0;
            break;
        }
        r4 = r27;
        fn_801F986C();
        if (r3 == 0) {
            r25 = 0x0;
            break;
        }
        r4 = 0x0;
        r5 = 0xcc;
        r6 = 0x0;
        fn_8012640C();
        r25 = r3;
        break;
    L_8001C680:
        if (r4 >= 0x1e) {
            break;
        }
    } while (0);
        r3 = r25;
        fn_80123FBC();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r25 = 0x0;
        }
    } while (0);
        if (r25 == 0) {
            tmp = 0x0;
            *(u16*)((u8*)r29 + 0x0) = tmp;

        } else {
        do {
            r3 = r25;
            r4 = r29;
            fn_8011FDC8();
            r3 = r25;
            r4 = 0x0;
            r5 = 0x7b;
            r6 = 0x0;
            fn_8012640C();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                tmp = 0x0;
                *(u16*)((u8*)r29 + 0x1A) = tmp;
            }
            r3 = r25;
            r4 = 0x0;
            r5 = 0x7b;
            r6 = 0x0;
            fn_8012640C();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                tmp = 0x1;
                break;
            }
            r3 = r25;
            fn_80120FE0();
            tmp = r3 & 0xFFFF;
            if ((s32)tmp != 0x3c) {
                if ((s32)tmp < 0x3c) {
                    if ((s32)tmp != 0x3a) {
                        if ((s32)tmp < 0x3a) {
                            tmp = 0x0; break;
                        }
                        if ((s32)tmp != 0x3e) {
                            if ((s32)tmp >= 0x3e) { tmp = 0x0; break; }
                            goto L_8001C764;
                            }
                        tmp = 0x2;
                        break;
                            }
                    tmp = 0x3;
                    break;
                }
                tmp = 0x4;
                break;
            L_8001C764:
                tmp = 0x5;
                break;
                        }
            tmp = 0x6;
            break;


        } while (0);
            tmp = tmp & 0xFFFF;
            r3 = (u32)lbl_802E4EB8;
            tmp = tmp << 1;
            r3 = (u32)lbl_802E4EB8;
            tmp = *(u16*)(r3 + tmp);
            *(u16*)((u8*)r29 + 0x24) = tmp;
        }
        r27 = r27 + 0x1;

    }
    r3 = -0x1;

    return;
}
#endif

/* 0x8001C7B8 | 0xBC0 */
extern void fn_8010264C(void);
extern void fn_80102510();
extern void fn_80102568(void);
extern void fn_801C41C8(void);
extern void fn_801C40F0(void);
extern void fn_80097F08(void);
extern void fn_801026A4();
extern void fn_80104704(void);
extern s32 fn_801022B8();
extern u32 _threadSwitch(void);
extern void fn_801070F4(void);
extern void fn_80121BB4(void);
extern s8 fn_8001E074(u8, s16, s16, u32);
extern u32 fn_80019064(void);
extern void fn_80018F54();
extern void fn_8001D718(void);
extern f32 lbl_8047B7C0;
extern u8 lbl_802E4E58[];
#if 0
asm void fn_8001C7B8(void) {
#include "src/game/gs_pcbox_fn_8001C7B8.inc"
}
#else
void fn_8001C7B8(void) {
    extern u8 lbl_802E4E58[];
    extern u8 lbl_802E4EB8[];
    extern u8 lbl_803A1C20[];
    extern u8 lbl_803A1D40[];
    extern f32 lbl_8047B7C0;
    extern void fn_80018F54();
    extern u32 fn_80019064();
    extern void fn_8001C064();
    extern void fn_8001D718();
    extern void fn_8006AEEC();
    extern void fn_80097F08();
    extern void _threadSwitch();
    extern void fn_801022B8();
    extern void fn_80102510();
    extern void fn_80102568();
    extern void fn_8010264C();
    extern void fn_801026A4();
    extern void fn_80104704();
    extern void fn_801070F4();
    extern void fn_8011FDC8();
    extern void fn_80120FE0();
    extern void fn_80121BB4();
    extern void fn_80123FBC();
    extern void fn_8012640C();
    extern void fn_80129280();
    extern void fn_8012AC08();
    extern void fn_801906A0();
    extern void fn_801C40F0();
    extern void fn_801C41C8();
    extern void fn_801F2A7C();
    extern void fn_801F986C();
    extern void fn_801FB1C0();
    extern void fn_80019D5C();
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r26 = r3;
    r3 = (u32)lbl_803A1D40;
    r31 = (u32)lbl_803A1D40;
    do {
    do {
        tmp = 0x1;
        r3 = 0x8ae;
        *(u8*)((u8*)r31 + 0x14) = tmp;
        fn_801906A0();
        if (r3 == 0) {
            r3 = 0x6b;
            r4 = 0x1;
            fn_8010264C();
            r23 = r3;
            r3 = 0x6b;
            fn_80102510();
        } else {

            r3 = 0x10f;
            r4 = 0x1;
            fn_8010264C();
            r23 = r3;
            r3 = 0x10f;
            fn_80102510();
        }
        do {
            if ((s32)r23 == 0x3d2) break;
            if ((s32)r23 < 0x3d2) {
                if ((s32)r23 != 0x3cf) {
                    if ((s32)r23 < 0x3cf) {
                        if ((s32)r23 == (s32)-0x1) break;
                        break;
                    }
                    if ((s32)r23 < 0x3d1) {
                        goto L_8001CA84;
                    }
                    if ((s32)r23 == 0x52b) goto L_8001CA84;
                    if ((s32)r23 < 0x52b) {
                    }
                    if ((s32)r23 < 0x52a) {
                        break;
                    }
                    if ((s32)r23 >= 0x52d) break;
                    break;
                    }
            do {
                r3 = 0x63;
            do {
                r4 = 0x0;
                r5 = 0x1;
                fn_80102568();
                r4 = *(u32*)((u8*)r31 + 0x8);
                r24 = 0x0;
                r3 = *(u32*)((u8*)r31 + 0xC);
                if ((s32)r4 != 1) {
                    if ((s32)r4 < 1) {
                        if ((s32)r4 < 0) {
                            break;
                        }
                        if ((s32)r4 >= 3) break;
                        goto L_8001C9C4;
                        }
                    r23 = (s8)r26;
                    tmp = r23 & 0xFFFF;
                    if (tmp >= 6) {
                        break;
                    }
                    if ((s32)r4 != 1) {
                        if ((s32)r4 >= 1) goto L_8001C93C;
                        if ((s32)r4 < 0) {
                            goto L_8001C93C;
                        }
                        r3 = 0x8ae;
                        fn_801906A0();
                        if (r3 == 0) {
                            r3 = 0x0;
                            r4 = 0x2;
                            fn_80129280();
                            goto L_8001C940;
                        }
                        fn_8006AEEC();
                        goto L_8001C940;
                    }
                    if (r3 == 0) {
                        r3 = 0x0;
                        fn_801F2A7C();
                    }
                    if (r3 == 0) {
                        r3 = 0x0;
                        goto L_8001C940;
                    }
                    r4 = 0x0;
                    r5 = 0x44;
                    r6 = 0x0;
                    fn_801FB1C0();
                    goto L_8001C940;
                L_8001C93C:
                    r3 = 0x0;
                L_8001C940:
                    if (r3 == 0) {
                        r24 = 0x0;
                        break;
                    }
                    r4 = r23;
                    fn_8012AC08();
                    r24 = r3;
                    break;
                }
                r23 = (s8)r26;
                tmp = r23 & 0xFFFF;
                if (tmp >= 6) {
                    break;
                }
                if (r3 == 0) {
                    r3 = 0x0;
                    fn_801F2A7C();
                }
                if (r3 == 0) {
                    r24 = 0x0;
                    break;
                }
                r4 = r23;
                fn_801F986C();
                if (r3 == 0) {
                    r24 = 0x0;
                    break;
                }
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                fn_8012640C();
                r24 = r3;
                break;
            L_8001C9C4:
                tmp = (s8)r26;
                tmp = tmp & 0xFFFF;
                if (tmp >= 0x1e) {
                    break;
                }
            } while (0);
                r3 = r24;
                fn_80123FBC();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    r24 = 0x0;
                }
            } while (0);
                f1 = lbl_8047B7C0;
                r3 = 0x3;
                fn_801C41C8();
                r3 = 0x1;
                fn_801C40F0();
                r4 = (u32)fn_80019D5C;
                r3 = r24;
                r4 = (u32)fn_80019D5C;
                r5 = 0x0;
                fn_80097F08();
                tmp = *(u8*)((u8*)r31 + 0x6);
                r3 = (u32)lbl_803A1C20;
                r9 = (u32)lbl_803A1C20;
                r5 = (u32)sp + 0x28;
                tmp = (s8)tmp;
                r3 = 0x63;
                *(u32*)(sp + 0x28) = tmp;
                r4 = 0x0;
                r6 = 0x0;
                r7 = 0x0;
                r8 = 0x1;
                fn_801026A4();
                r3 = 0x63;
                fn_80104704();
                if (r3 != 0) {
                    tmp = 0x1;
                    *(u8*)((u8*)r3 + 0x98) = tmp;
                }
                f1 = lbl_8047B7C0;
                r3 = 0x2;
                fn_801C41C8();
                r3 = 0x1;
                fn_801C40F0();
                r26 = *(u8*)((u8*)r31 + 0x6);
                r23 = 0x0;
                break;
            L_8001CA84:
                tmp = *(u8*)((u8*)r31 + 0x6);
                r4 = 0x2;
                r3 = (u32)lbl_803A1C20;
                *(u8*)((u8*)r31 + 0x14) = r4;
                tmp = (s8)tmp;
                r5 = (u32)sp + 0x2c;
                *(u8*)((u8*)r31 + 0x7) = r26;
                r9 = (u32)lbl_803A1C20;
                r3 = 0x63;
                r4 = 0x0;
                *(u32*)(sp + 0x2C) = tmp;
                r6 = 0x0;
                r7 = 0x1;
                r8 = 0x1;
                fn_801026A4();
                tmp = *(u8*)((u8*)r31 + 0x1);
                r3 = (s8)r3;
                if (tmp == 0) {
                    r3 = -0x2;
                }
                tmp = -0x1;
                r28 = (s8)r3;
                *(u8*)((u8*)r31 + 0x7) = tmp;
                r3 = 0x63;
                fn_801022B8();
                do {
                if ((s32)r3 == 0x3b6 || (s32)r28 == (s32)-0x1) break;
                do {

                do {
                    r4 = *(u32*)((u8*)r31 + 0x8);
                    r27 = 0x0;
                    r3 = *(u32*)((u8*)r31 + 0xC);
                    if ((s32)r4 != 1) {
                        if ((s32)r4 < 1) {
                            if ((s32)r4 < 0) {
                                break;
                            }
                            if ((s32)r4 >= 3) break;
                            goto L_8001CC3C;
                            }
                        r23 = (s8)r26;
                        tmp = r23 & 0xFFFF;
                        if (tmp >= 6) {
                            break;
                        }
                        if ((s32)r4 != 1) {
                            if ((s32)r4 >= 1) goto L_8001CBB4;
                            if ((s32)r4 < 0) {
                                goto L_8001CBB4;
                            }
                            r3 = 0x8ae;
                            fn_801906A0();
                            if (r3 == 0) {
                                r3 = 0x0;
                                r4 = 0x2;
                                fn_80129280();
                                goto L_8001CBB8;
                            }
                            fn_8006AEEC();
                            goto L_8001CBB8;
                        }
                        if (r3 == 0) {
                            r3 = 0x0;
                            fn_801F2A7C();
                        }
                        if (r3 == 0) {
                            r3 = 0x0;
                            goto L_8001CBB8;
                        }
                        r4 = 0x0;
                        r5 = 0x44;
                        r6 = 0x0;
                        fn_801FB1C0();
                        goto L_8001CBB8;
                    L_8001CBB4:
                        r3 = 0x0;
                    L_8001CBB8:
                        if (r3 == 0) {
                            r27 = 0x0;
                            break;
                        }
                        r4 = r23;
                        fn_8012AC08();
                        r27 = r3;
                        break;
                    }
                    r23 = (s8)r26;
                    tmp = r23 & 0xFFFF;
                    if (tmp >= 6) {
                        break;
                    }
                    if (r3 == 0) {
                        r3 = 0x0;
                        fn_801F2A7C();
                    }
                    if (r3 == 0) {
                        r27 = 0x0;
                        break;
                    }
                    r4 = r23;
                    fn_801F986C();
                    if (r3 == 0) {
                        r27 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    fn_8012640C();
                    r27 = r3;
                    break;
                L_8001CC3C:
                    tmp = (s8)r26;
                    tmp = tmp & 0xFFFF;
                    if (tmp >= 0x1e) {
                        break;
                    }
                } while (0);
                    r3 = r27;
                    fn_80123FBC();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        r27 = 0x0;
                    }
                } while (0);
                do {
                    r4 = *(u32*)((u8*)r31 + 0x8);
                do {
                    r29 = 0x0;
                    r3 = *(u32*)((u8*)r31 + 0xC);
                    if ((s32)r4 != 1) {
                        if ((s32)r4 < 1) {
                            if ((s32)r4 < 0) {
                                break;
                            }
                            if ((s32)r4 >= 3) break;
                            goto L_8001CDA0;
                            }
                        tmp = r28 & 0xFFFF;
                        if (tmp >= 6) {
                            break;
                        }
                        if ((s32)r4 != 1) {
                            if ((s32)r4 >= 1) goto L_8001CD1C;
                            if ((s32)r4 < 0) {
                                goto L_8001CD1C;
                            }
                            r3 = 0x8ae;
                            fn_801906A0();
                            if (r3 == 0) {
                                r3 = 0x0;
                                r4 = 0x2;
                                fn_80129280();
                                goto L_8001CD20;
                            }
                            fn_8006AEEC();
                            goto L_8001CD20;
                        }
                        if (r3 == 0) {
                            r3 = 0x0;
                            fn_801F2A7C();
                        }
                        if (r3 == 0) {
                            r3 = 0x0;
                            goto L_8001CD20;
                        }
                        r4 = 0x0;
                        r5 = 0x44;
                        r6 = 0x0;
                        fn_801FB1C0();
                        goto L_8001CD20;
                    L_8001CD1C:
                        r3 = 0x0;
                    L_8001CD20:
                        if (r3 == 0) {
                            r29 = 0x0;
                            break;
                        }
                        r4 = r28 & 0xFFFF;
                        fn_8012AC08();
                        r29 = r3;
                        break;
                    }
                    tmp = r28 & 0xFFFF;
                    if (tmp >= 6) {
                        break;
                    }
                    if (r3 == 0) {
                        r3 = 0x0;
                        fn_801F2A7C();
                    }
                    if (r3 == 0) {
                        r29 = 0x0;
                        break;
                    }
                    r4 = r28 & 0xFFFF;
                    fn_801F986C();
                    if (r3 == 0) {
                        r29 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    fn_8012640C();
                    r29 = r3;
                    break;
                L_8001CDA0:
                    tmp = r28 & 0xFFFF;
                    if (tmp >= 0x1e) {
                        break;
                    }
                } while (0);
                    r3 = r29;
                    fn_80123FBC();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        r29 = 0x0;
                    }
                } while (0);
                    if (r27 == 0 || r27 == 0) break;

                    tmp = *(u8*)((u8*)r31 + 0x4);
                    r3 = (u32)lbl_802E4E58;
                    r30 = (s8)r26;
                    r4 = (u32)sp + 0x1e;
                    r5 = (s8)tmp;
                    tmp = (u32)lbl_802E4E58;
                    r3 = r5 * 0x30;
                    r24 = r30 << 3;
                    r5 = (u32)sp + 0x20;
                    r3 = tmp + r3;
                    r25 = *(s16*)(r3 + r24);
                    r3 = r25;
                    ((void(*)(void))fn_8005D95C)();
                    tmp = *(s16*)((u8*)(u32)sp + 0x1E);
                    r3 = r25;
                    if ((s32)tmp > 0xfa) {
                        r4 = 0x11a;
                    } else {

                        r4 = 0x122;
                    }
                    ((void(*)(void))fn_801080CC)();
                    r4 = *(u8*)((u8*)r31 + 0x4);
                    r3 = (u32)lbl_802E4E58;
                    tmp = (u32)lbl_802E4E58;
                    r25 = r28 << 3;
                    r3 = (s8)r4;
                    r4 = (u32)sp + 0x1a;
                    r3 = r3 * 0x30;
                    r5 = (u32)sp + 0x1c;
                    r3 = tmp + r3;
                    r28 = *(s16*)(r3 + r25);
                    r3 = r28;
                    ((void(*)(void))fn_8005D95C)();
                    tmp = *(s16*)((u8*)(u32)sp + 0x1A);
                    r3 = r28;
                    if ((s32)tmp > 0xfa) {
                        r4 = 0x11a;
                    } else {

                        r4 = 0x122;
                    }
                    ((void(*)(void))fn_801080CC)();
                    r3 = (u32)lbl_802E4E58;
                    r4 = r30 << 3;
                    tmp = (u32)lbl_802E4E58;
                    r30 = tmp + r4;
                    while (1) {
                        tmp = *(u8*)((u8*)r31 + 0x4);
                        r4 = (u32)sp + 0x16;
                        r5 = (u32)sp + 0x18;
                        tmp = (s8)tmp;
                        tmp = tmp * 0x30;
                        r28 = *(s16*)(r30 + tmp);
                        r3 = r28;
                        ((void(*)(void))fn_8005D95C)();
                        r3 = r28;
                        fn_801070F4();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) break;
                        _threadSwitch();

                    }
                    r3 = r27;
                    r4 = r29;
                    fn_80121BB4();
                    r3 = (u32)lbl_803A1C20;
                    r4 = 0x0;
                    r3 = (u32)lbl_803A1C20;
                    r5 = 0x120;
                    memset((void*)r3, (int)r4, (u32)r5);
                    r29 = 0x0;
                    r3 = (u32)lbl_803A1C20;
                    r27 = (u32)lbl_803A1C20;
                    while (1) {
                    do {
                        tmp = r29 & 0xFFFF;
                    do {
                        if (tmp >= 6) break;
                        r4 = r29 & 0xFFFF;
                        r5 = *(u32*)((u8*)r31 + 0x8);
                        tmp = r4 * 0x30;
                        r3 = *(u32*)((u8*)r31 + 0xC);
                        r23 = 0x0;
                        r28 = r27 + tmp;
                        if ((s32)r5 != 1) {
                            if ((s32)r5 < 1) {
                                if ((s32)r5 < 0) {
                                    break;
                                }
                                if ((s32)r5 >= 3) break;
                                goto L_8001D030;
                                }
                            if (r4 >= 6) {
                                break;
                            }
                            if ((s32)r5 != 1) {
                                if ((s32)r5 >= 1) goto L_8001CFB0;
                                if ((s32)r5 < 0) {
                                    goto L_8001CFB0;
                                }
                                r3 = 0x8ae;
                                fn_801906A0();
                                if (r3 == 0) {
                                    r3 = 0x0;
                                    r4 = 0x2;
                                    fn_80129280();
                                    goto L_8001CFB4;
                                }
                                fn_8006AEEC();
                                goto L_8001CFB4;
                            }
                            if (r3 == 0) {
                                r3 = 0x0;
                                fn_801F2A7C();
                            }
                            if (r3 == 0) {
                                r3 = 0x0;
                                goto L_8001CFB4;
                            }
                            r4 = 0x0;
                            r5 = 0x44;
                            r6 = 0x0;
                            fn_801FB1C0();
                            goto L_8001CFB4;
                        L_8001CFB0:
                            r3 = 0x0;
                        L_8001CFB4:
                            if (r3 == 0) {
                                r23 = 0x0;
                                break;
                            }
                            r4 = r29;
                            fn_8012AC08();
                            r23 = r3;
                            break;
                        }
                        if (r4 >= 6) {
                            break;
                        }
                        if (r3 == 0) {
                            r3 = 0x0;
                            fn_801F2A7C();
                        }
                        if (r3 == 0) {
                            r23 = 0x0;
                            break;
                        }
                        r4 = r29;
                        fn_801F986C();
                        if (r3 == 0) {
                            r23 = 0x0;
                            break;
                        }
                        r4 = 0x0;
                        r5 = 0xcc;
                        r6 = 0x0;
                        fn_8012640C();
                        r23 = r3;
                        break;
                    L_8001D030:
                        if (r4 >= 0x1e) {
                            break;
                        }
                    } while (0);
                        r3 = r23;
                        fn_80123FBC();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) {
                            r23 = 0x0;
                        }
                    } while (0);
                        if (r23 == 0) {
                            tmp = 0x0;
                            *(u16*)((u8*)r28 + 0x0) = tmp;

                        } else {
                        do {
                            r3 = r23;
                            r4 = r28;
                            fn_8011FDC8();
                            r3 = r23;
                            r4 = 0x0;
                            r5 = 0x7b;
                            r6 = 0x0;
                            fn_8012640C();
                            tmp = r3 & 0xFF;
                            if (tmp == 1) {
                                tmp = 0x0;
                                *(u16*)((u8*)r28 + 0x1A) = tmp;
                            }
                            r3 = r23;
                            r4 = 0x0;
                            r5 = 0x7b;
                            r6 = 0x0;
                            fn_8012640C();
                            tmp = r3 & 0xFF;
                            if (tmp == 1) {
                                tmp = 0x1;
                                break;
                            }
                            r3 = r23;
                            fn_80120FE0();
                            tmp = r3 & 0xFFFF;
                            if ((s32)tmp != 0x3c) {
                                if ((s32)tmp < 0x3c) {
                                    if ((s32)tmp != 0x3a) {
                                        if ((s32)tmp < 0x3a) {
                                            tmp = 0x0; break;
                                        }
                                        if ((s32)tmp != 0x3e) {
                                            if ((s32)tmp >= 0x3e) { tmp = 0x0; break; }
                                            goto L_8001D114;
                                            }
                                        tmp = 0x2;
                                        break;
                                            }
                                    tmp = 0x3;
                                    break;
                                }
                                tmp = 0x4;
                                break;
                            L_8001D114:
                                tmp = 0x5;
                                break;
                                        }
                            tmp = 0x6;
                            break;


                        } while (0);
                            tmp = tmp & 0xFFFF;
                            r3 = (u32)lbl_802E4EB8;
                            tmp = tmp << 1;
                            r3 = (u32)lbl_802E4EB8;
                            tmp = *(u16*)(r3 + tmp);
                            *(u16*)((u8*)r28 + 0x24) = tmp;
                        }
                        r29 = r29 + 0x1;

                    }
                    f1 = lbl_8047B7C0;
                    fn_8001D718();
                    r5 = *(u8*)((u8*)r31 + 0x4);
                    r3 = (u32)lbl_802E4E58;
                    tmp = (u32)lbl_802E4E58;
                    r4 = (u32)sp + 0x12;
                    r3 = (s8)r5;
                    r5 = (u32)sp + 0x14;
                    r3 = r3 * 0x30;
                    r3 = tmp + r3;
                    r24 = *(s16*)(r3 + r24);
                    r3 = r24;
                    ((void(*)(void))fn_8005D95C)();
                    tmp = *(s16*)((u8*)(u32)sp + 0x12);
                    r3 = r24;
                    if ((s32)tmp > 0xfa) {
                        r4 = 0x116;
                    } else {

                        r4 = 0x11e;
                    }
                    ((void(*)(void))fn_801080CC)();
                    r5 = *(u8*)((u8*)r31 + 0x4);
                    r3 = (u32)lbl_802E4E58;
                    tmp = (u32)lbl_802E4E58;
                    r4 = (u32)sp + 0xe;
                    r3 = (s8)r5;
                    r5 = (u32)sp + 0x10;
                    r3 = r3 * 0x30;
                    r3 = tmp + r3;
                    r24 = *(s16*)(r3 + r25);
                    r3 = r24;
                    ((void(*)(void))fn_8005D95C)();
                    tmp = *(s16*)((u8*)(u32)sp + 0xE);
                    r3 = r24;
                    if ((s32)tmp > 0xfa) {
                        r4 = 0x116;
                    } else {

                        r4 = 0x11e;
                    }
                    ((void(*)(void))fn_801080CC)();
                    while (1) {
                        tmp = *(u8*)((u8*)r31 + 0x4);
                        r4 = (u32)sp + 0xa;
                        r5 = (u32)sp + 0xc;
                        tmp = (s8)tmp;
                        tmp = tmp * 0x30;
                        r24 = *(s16*)(r30 + tmp);
                        r3 = r24;
                        ((void(*)(void))fn_8005D95C)();
                        r3 = r24;
                        fn_801070F4();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) break;
                        _threadSwitch();

                    }
                } while (0);
                r23 = -0x1;
                break;
                    }
        do {
            tmp = 0x3;
            r3 = 0x6c;
            *(u8*)((u8*)r31 + 0x14) = tmp;
            r4 = 0x1;
            fn_8010264C();
            r24 = r3;
            r3 = 0x6c;
            fn_80102510();
            if ((s32)r24 != 1) {
                if ((s32)r24 < 1) {
                    if ((s32)r24 != (s32)-0x1) {
                        if ((s32)r24 < (s32)-0x1) {
                            break;
                        }
                        if ((s32)r24 >= 3) break;
                    }
                    goto L_8001D338;
                    }
                r3 = 0x63;
                fn_80102510();
                r3 = 0x2;
                r4 = 0x0;
                r5 = 0x0;
                fn_80018F54();
                r25 = r3;
                fn_80019064();
                tmp = *(u8*)((u8*)r31 + 0x6);
                r4 = (u32)lbl_803A1C20;
                r9 = (u32)lbl_803A1C20;
                r24 = r3;
                tmp = (s8)tmp;
                r5 = (u32)sp + 0x24;
                *(u32*)(sp + 0x24) = tmp;
                r3 = 0x63;
                r4 = 0x0;
                r6 = 0x0;
                r7 = 0x0;
                r8 = 0x1;
                fn_801026A4();
                r3 = 0x63;
                fn_80104704();
                if (r3 != 0) {
                    tmp = 0x1;
                    *(u8*)((u8*)r3 + 0x98) = tmp;
                }
                tmp = r25 & 0xFFFF;
                if (tmp == 0) {
                    r24 = -0x1;
                    break;
                }
                r3 = r26;
                r5 = r25;
                r4 = r24 & 0xFF;
                r6 = (u32)sp + 0x8;
                fn_8001C064();
                r24 = 0x0;
                break;
            }
            r3 = r26;
            r6 = (u32)sp + 0x8;
            r4 = -0x1;
            r5 = 0x0;
            fn_8001C064();
            r24 = 0x0;
            break;
        L_8001D338:
            r24 = -0x1;
        } while (0);
            if ((s32)r24 == (s32)-0x1) {
                r23 = 0x0;
                break;
            }
            r23 = -0x1;
            break;
        } while (0);
        r23 = -0x1;
    } while (0);
        ;
    } while ((s32)r23 != (s32)(-0x1));
    r3 = 0x0;
    return;
}
#endif

/* 0x8001D378 | 0x2AC */
#if 0
asm void fn_8001D378(void) {
#include "src/game/gs_pcbox_fn_8001D378.inc"
}
#else
void fn_8001D378(void) {
    extern u8 lbl_802E4EB8[];
    extern u8 lbl_803A1C20[];
    extern u8 lbl_803A1D40[];
    extern void fn_8006AEEC();
    extern void fn_8011FDC8();
    extern void fn_80120FE0();
    extern void fn_80123FBC();
    extern void fn_8012640C();
    extern void fn_80129280();
    extern void fn_8012AC08();
    extern void fn_801906A0();
    extern void fn_801F2A7C();
    extern void fn_801F986C();
    extern void fn_801FB1C0();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r3 = (u32)lbl_803A1C20;
    r4 = 0x0;
    r3 = (u32)lbl_803A1C20;
    r5 = 0x120;
    memset((void*)r3, (int)r4, (u32)r5);
    r3 = (u32)lbl_803A1D40;
    r29 = 0x0;
    r30 = (u32)lbl_803A1D40;
    r3 = (u32)lbl_803A1C20;
    r31 = (u32)lbl_803A1C20;
    while (1) {
    do {
        tmp = r29 & 0xFFFF;
    do {
        if (tmp >= 6) break;
        r4 = r29 & 0xFFFF;
        r5 = *(u32*)((u8*)r30 + 0x8);
        tmp = r4 * 0x30;
        r3 = *(u32*)((u8*)r30 + 0xC);
        r27 = 0x0;
        r28 = r31 + tmp;
        if ((s32)r5 != 1) {
            if ((s32)r5 < 1) {
                if ((s32)r5 < 0) {
                    break;
                }
                if ((s32)r5 >= 3) break;
                goto L_8001D4F0;
                }
            if (r4 >= 6) {
                break;
            }
            if ((s32)r5 != 1) {
                if ((s32)r5 >= 1) goto L_8001D470;
                if ((s32)r5 < 0) {
                    goto L_8001D470;
                }
                r3 = 0x8ae;
                fn_801906A0();
                if (r3 == 0) {
                    r3 = 0x0;
                    r4 = 0x2;
                    fn_80129280();
                    goto L_8001D474;
                }
                fn_8006AEEC();
                goto L_8001D474;
            }
            if (r3 == 0) {
                r3 = 0x0;
                fn_801F2A7C();
            }
            if (r3 == 0) {
                r3 = 0x0;
                goto L_8001D474;
            }
            r4 = 0x0;
            r5 = 0x44;
            r6 = 0x0;
            fn_801FB1C0();
            goto L_8001D474;
        L_8001D470:
            r3 = 0x0;
        L_8001D474:
            if (r3 == 0) {
                r27 = 0x0;
                break;
            }
            r4 = r29;
            fn_8012AC08();
            r27 = r3;
            break;
        }
        if (r4 >= 6) {
            break;
        }
        if (r3 == 0) {
            r3 = 0x0;
            fn_801F2A7C();
        }
        if (r3 == 0) {
            r27 = 0x0;
            break;
        }
        r4 = r29;
        fn_801F986C();
        if (r3 == 0) {
            r27 = 0x0;
            break;
        }
        r4 = 0x0;
        r5 = 0xcc;
        r6 = 0x0;
        fn_8012640C();
        r27 = r3;
        break;
    L_8001D4F0:
        if (r4 >= 0x1e) {
            break;
        }
    } while (0);
        r3 = r27;
        fn_80123FBC();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r27 = 0x0;
        }
    } while (0);
        if (r27 == 0) {
            tmp = 0x0;
            *(u16*)((u8*)r28 + 0x0) = tmp;

        } else {
        do {
            r3 = r27;
            r4 = r28;
            fn_8011FDC8();
            r3 = r27;
            r4 = 0x0;
            r5 = 0x7b;
            r6 = 0x0;
            fn_8012640C();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                tmp = 0x0;
                *(u16*)((u8*)r28 + 0x1A) = tmp;
            }
            r3 = r27;
            r4 = 0x0;
            r5 = 0x7b;
            r6 = 0x0;
            fn_8012640C();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                tmp = 0x1;
                break;
            }
            r3 = r27;
            fn_80120FE0();
            tmp = r3 & 0xFFFF;
            if ((s32)tmp != 0x3c) {
                if ((s32)tmp < 0x3c) {
                    if ((s32)tmp != 0x3a) {
                        if ((s32)tmp < 0x3a) {
                            tmp = 0x0; break;
                        }
                        if ((s32)tmp != 0x3e) {
                            if ((s32)tmp >= 0x3e) { tmp = 0x0; break; }
                            goto L_8001D5D4;
                            }
                        tmp = 0x2;
                        break;
                            }
                    tmp = 0x3;
                    break;
                }
                tmp = 0x4;
                break;
            L_8001D5D4:
                tmp = 0x5;
                break;
                        }
            tmp = 0x6;
            break;


        } while (0);
            tmp = tmp & 0xFFFF;
            r3 = (u32)lbl_802E4EB8;
            tmp = tmp << 1;
            r3 = (u32)lbl_802E4EB8;
            tmp = *(u16*)(r3 + tmp);
            *(u16*)((u8*)r28 + 0x24) = tmp;
        }
        r29 = r29 + 0x1;

    }
    return;
}
#endif

/* 0x8001D718 | 0xCC */
extern s32 fn_800D37CC();
extern void fn_800D3088(void);
extern f32 lbl_8047B7C8;
extern f64 lbl_8047B7D0;
extern f64 lbl_8047B7D8;
#if 0
asm void fn_8001D718(void) {
#include "src/game/gs_pcbox_fn_8001D718.inc"
}
#else
void fn_8001D718(void) {
    extern f32 lbl_8047B7C8;
    extern f64 lbl_8047B7D0;
    extern f64 lbl_8047B7D8;
    extern void fn_800D3088();
    extern void fn_800D37CC();
    extern void _threadSwitch();
    u8 sp[0x70];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;

    f27 = f1;
    f28 = lbl_8047B7C8;
    f29 = lbl_8047B7D0;
    r31 = 0x43300000;
    f31 = lbl_8047B7D8;
    while (f28 < f27) {

        _threadSwitch();
        fn_800D37CC();
        *(u32*)(sp + 0xC) = tmp;
        f30 = f0 - f29;
        fn_800D3088();
        f0 = f0 - f31;
        f0 = f0 / f30;
        f28 = f28 + f0;

    }
    return;
}
#endif

/* 0x50 | fn_8001D7E4 | multi_call_cond */
extern u32 fn_800F7AF0(s32);
extern u32 fn_800F7BC4(s32);
#pragma peephole off
u32 fn_8001D7E4(void) {
    u32 a;
    u32 b;
    u32 m;
    goto _test;
    do {
        _threadSwitch();
    _test:
        a = fn_800F7AF0(1);
        b = fn_800F7BC4(1);
        m = (b & a) & 0x300;
    } while (m == 0);
    return b;
}
#pragma peephole reset

/* 0x8001D834 | 0xB4 */
#if 0
asm void fn_8001D834(void) {
#include "src/game/gs_pcbox_fn_8001D834.inc"
}
#else
void fn_8001D834(void) {
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    f32 f7 = 0.0f;

    r6 = *(u8*)((u8*)r4 + 0x67);
    r5 = 0x80810000;
    tmp = *(u8*)((u8*)r3 + 0x8B);
    r8 = *(u8*)((u8*)r4 + 0x66);
    r7 = *(u8*)((u8*)r3 + 0x8A);
    r9 = r6 * tmp;
    r6 = *(u8*)((u8*)r4 + 0x64);
    r5 = *(u8*)((u8*)r3 + 0x88);
    r4 = *(u8*)((u8*)r4 + 0x65);
    tmp = *(u8*)((u8*)r3 + 0x89);
    r3 = r8 * r7;
    r5 = r6 * r5;
    tmp = r4 * tmp;
    r4 = (s32)((s64)r10 * (s64)r9 >> 32);
    r6 = (s32)((s64)r10 * (s64)r3 >> 32);
    r4 = r4 + r9;
    r7 = (s32)r4 >> 7;
    r4 = (s32)((s64)r10 * (s64)r5 >> 32);
    r8 = (u32)r7 >> 31;
    r3 = r6 + r3;
    r7 = r7 + r8;
    r6 = (s32)r3 >> 7;
    r7 = r7 & 0xFF;
    r3 = (s32)((s64)r10 * (s64)tmp >> 32);
    r4 = r4 + r5;
    r5 = (u32)r6 >> 31;
    r4 = (s32)r4 >> 7;
    r6 = r6 + r5;
    r5 = (u32)r4 >> 31;
    tmp = r3 + tmp;
    r4 = r4 + r5;
    tmp = (s32)tmp >> 7;
    r5 = r6 & 0xFF;
    r3 = (u32)tmp >> 31;
    r4 = r4 & 0xFF;
    tmp = tmp + r3;
    r5 = r5 << 8;
    tmp = tmp & 0xFF;
    r3 = r4 << 24;
    tmp = tmp << 16;
    tmp = r3 | tmp;
    tmp = r5 | tmp;
    r3 = r7 | tmp;
    return;
}
#endif

/* 0x8001D8E8 | 0xAC */
extern void fn_801046B8();
extern void fn_80102868(void);
extern void fn_801045A8(void);
extern void fn_801043A4(void);
#if 0
asm void fn_8001D8E8(void) {
#include "src/game/gs_pcbox_fn_8001D8E8.inc"
}
#else
void fn_8001D8E8(void) {
    extern void fn_80102568();
    extern void fn_801026A4();
    extern void fn_80102868();
    extern void fn_801043A4();
    extern void fn_801045A8();
    extern void fn_801046B8();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r31 = r7;
    fn_801046B8();
    r4 = r3;
    r10 = r28;
    r5 = (u32)sp + 0x10;
    r9 = r27 & 0xFF;
    r3 = 0xe7;
    r6 = 0x0;
    r7 = 0x0;
    r8 = 0x3;
    fn_801026A4();
    r4 = (s16)r30;
    r5 = (s16)r31;
    r3 = 0xe7;
    fn_80102868();
    r3 = 0xe7;
    r4 = 0x1;
    fn_801045A8();
    r3 = 0xe7;
    fn_801043A4();
    tmp = r3;
    r3 = 0xe7;
    r31 = tmp;
    r4 = 0x0;
    r5 = 0x1;
    fn_80102568();
    r3 = r31;
    return;
}
#endif

/* 0x8001D994 | 0xCC */
extern void fn_8011F5C8(void);
extern void fn_8011E778(void);
extern void fn_8011E760(void);
extern void fn_800FA280(void);
extern void fn_800F9EE4(void);
extern void fn_801231A4(void);
#if 0
asm void fn_8001D994(void) {
#include "src/game/gs_pcbox_fn_8001D994.inc"
}
#else
void fn_8001D994(void) {
    extern void fn_800F9EE4();
    extern void fn_800FA280();
    extern void fn_8011E760();
    extern void fn_8011E778();
    extern void fn_8011F5C8();
    extern void fn_801231A4();
    extern void fn_80123FBC();
    extern void fn_8012640C();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    fn_80123FBC();
    tmp = r3 & 0xFF;
    if (tmp == 0) {
        r3 = 0xff;
        return;
    }
    r3 = r30;
    fn_8011F5C8();
    tmp = r3 & 0xFFFF;

    do {
        if (tmp != 0x1d && tmp != 0x20) break;

        r3 = r30;
        r4 = 0x0;
        r5 = 0x6e;
        r6 = 0x0;
        fn_8012640C();
        r3 = r3 & 0xFFFF;
        fn_8011E778();
        if (r3 == 0) break;
        fn_8011E760();
        fn_800FA280();
        tmp = r3;
        r3 = r30;
        r31 = tmp;
        r4 = 0x0;
        r5 = 0x77;
        r6 = 0x0;
        fn_8012640C();
        r4 = r31;
        fn_800F9EE4();
        if ((s32)r3 != 0) break;
        r3 = 0x2;
        return;
    } while (0);
    r3 = r30;
    fn_801231A4();

    return;
}
#endif

/* 0x8001DACC | 0x4DC */
extern void fn_800F92D4(void);
extern void fn_800EF4FC(void);
extern void fn_800EF4F4(void);
extern void fn_800D88DC(void);
extern void fn_800D888C(void);
extern void fn_800EF590(void);
extern void fn_800D848C(void);
extern void fn_800DC1D4(void);
extern void fn_800D85D4(void);
extern void fn_800DC224(void);
extern void fn_800DC14C(void);
extern void fn_800DC0D4(void);
extern void fn_800DC04C(void);
extern void fn_800DBFD4(void);
extern void fn_800D7820(void);
extern void fn_800D6A00(void);
extern void fn_800D67BC(void);
extern void fn_800D61E4(void);
extern void fn_800D5CB8(void);
extern void fn_800D59B8(void);
extern void fn_800D6728(void);
extern f64 lbl_8047B7D8;
extern f64 lbl_8047B7D0;
extern u32 lbl_8047AD00;
extern f32 lbl_8047B7E0;
#if 0
asm void fn_8001DACC(void) {
#include "src/game/gs_pcbox_fn_8001DACC.inc"
}
#else
void fn_8001DACC(void) {
    extern u32 lbl_8047AD00;
    extern f64 lbl_8047B7D0;
    extern f64 lbl_8047B7D8;
    extern f32 lbl_8047B7E0;
    extern void fn_800D59B8();
    extern void fn_800D5CB8();
    extern void fn_800D61E4();
    extern void fn_800D6728();
    extern void fn_800D67BC();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D848C();
    extern void fn_800D85D4();
    extern void fn_800D888C();
    extern void fn_800D88DC();
    extern void fn_800DBFD4();
    extern void fn_800DC04C();
    extern void fn_800DC0D4();
    extern void fn_800DC14C();
    extern void fn_800DC1D4();
    extern void fn_800DC224();
    extern void fn_800EF4F4();
    extern void fn_800EF4FC();
    extern void fn_800EF590();
    extern void fn_800F92D4();
    u8 sp[0xF0];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f4 = 0.0f;
    f32 f26 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;

    r29 = r4;
    f29 = f1;
    r3 = *(u32*)((u8*)r29 + 0x58);
    fn_800F92D4();
    r4 = 0xEF0000;
    r31 = r3;
    r3 = r4 + 0x1200;
    fn_800F92D4();
    r30 = r3;
    if ((r31 != 0) && (r30 != 0)) {

        r3 = r31;
        fn_800EF4FC();
        tmp = *(s16*)((u8*)r29 + 0x5C);
        r3 = r3 & 0xFFFF;
        r4 = 0x43300000;
        f2 = lbl_8047B7D8;
        r3 = r31;
        f1 = lbl_8047B7D0;
        *(u32*)(sp + 0x44) = tmp;
        f2 = f0 - f2;
        f0 = f0 - f1;
        f31 = f0 / f2;
        fn_800EF4FC();
        r4 = *(s16*)((u8*)r29 + 0x5C);
        r3 = r3 & 0xFFFF;
        tmp = *(s16*)((u8*)r29 + 0x60);
        r5 = 0x43300000;
        r3 = r31;
        tmp = r4 + tmp;
        f2 = lbl_8047B7D8;
        f1 = lbl_8047B7D0;
        *(u32*)(sp + 0x54) = tmp;
        f2 = f0 - f2;
        f0 = f0 - f1;
        f30 = f0 / f2;
        fn_800EF4F4();
        tmp = *(s16*)((u8*)r29 + 0x5E);
        r3 = r3 & 0xFFFF;
        r4 = 0x43300000;
        f2 = lbl_8047B7D8;
        r3 = r31;
        f1 = lbl_8047B7D0;
        *(u32*)(sp + 0x64) = tmp;
        f2 = f0 - f2;
        f0 = f0 - f1;
        f27 = f0 / f2;
        fn_800EF4F4();
        r4 = *(s16*)((u8*)r29 + 0x5E);
        r5 = 0x43300000;
        tmp = *(s16*)((u8*)r29 + 0x62);
        r6 = r3 & 0xFFFF;
        r3 = 0x80000000;
        tmp = r4 + tmp;
        f2 = lbl_8047B7D8;
        f1 = lbl_8047B7D0;
        r3 = r3 + 0x3;
        *(u32*)(sp + 0x74) = tmp;
        f2 = f0 - f2;
        f0 = f0 - f1;
        f26 = f0 / f2;
        fn_800D88DC();
        r3 = 0x4;
        fn_800D888C();
        r3 = r30;
        r4 = 0x1;
        r5 = 0x1;
        fn_800EF590();
        r6 = (u32)sp + 0x8;
        r3 = 0x0;
        r4 = 0x0;
        r5 = 0x4;
        fn_800D848C();
        r6 = (u32)sp + 0x8;
        r3 = 0x1;
        r4 = 0x0;
        r5 = 0x5;
        fn_800D848C();
        r6 = (u32)sp + 0x8;
        r3 = 0x2;
        r4 = 0x0;
        r5 = 0x6;
        fn_800D848C();
        r3 = 0x3;
        fn_800DC1D4();
        r4 = r31;
        r3 = 0x0;
        fn_800D85D4();
        r3 = 0x0;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x0;
        fn_800DC224();
        r3 = 0x0;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x1;
        fn_800DC14C();
        r3 = 0x0;
        r4 = 0xf;
        r5 = 0x8;
        r6 = 0xa;
        r7 = 0xf;
        fn_800DC0D4();
        r3 = 0x0;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x1;
        fn_800DC04C();
        r3 = 0x0;
        r4 = 0x7;
        r5 = 0x4;
        r6 = 0x5;
        r7 = 0x7;
        fn_800DBFD4();
        r4 = r30;
        r3 = 0x1;
        fn_800D85D4();
        r3 = 0x1;
        r4 = 0x0;
        r5 = 0x1;
        r6 = 0x1;
        r7 = 0x0;
        fn_800DC224();
        r3 = 0x1;
        r4 = 0x1;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x1;
        r8 = 0x0;
        fn_800DC14C();
        r3 = 0x1;
        r4 = 0xf;
        r5 = 0x8;
        r6 = 0xd;
        r7 = 0x2;
        fn_800DC0D4();
        r3 = 0x1;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x1;
        r8 = 0x0;
        fn_800DC04C();
        r3 = 0x1;
        r4 = 0x7;
        r5 = 0x7;
        r6 = 0x7;
        r7 = 0x1;
        fn_800DBFD4();
        r4 = r30;
        r3 = 0x2;
        fn_800D85D4();
        r3 = 0x2;
        r4 = 0x0;
        r5 = 0x2;
        r6 = 0x2;
        r7 = 0x0;
        fn_800DC224();
        r3 = 0x2;
        r4 = 0x1;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x1;
        r8 = 0x0;
        fn_800DC14C();
        r3 = 0x2;
        r4 = 0xf;
        r5 = 0x8;
        r6 = 0xd;
        r7 = 0x2;
        fn_800DC0D4();
        r3 = 0x2;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x1;
        r8 = 0x0;
        fn_800DC04C();
        r3 = 0x2;
        r4 = 0x7;
        r5 = 0x7;
        r6 = 0x7;
        r7 = 0x1;
        fn_800DBFD4();
        r3 = lbl_8047AD00;
        fn_800D7820();
        r3 = 0x7;
        fn_800D6A00();
        r3 = 0x2;
        fn_800D67BC();
        r3 = 0x0;
        r4 = 0x0;
        fn_800D61E4();
        r3 = 0x0;
        r4 = 0xff;
        r5 = 0x40;
        r6 = 0x40;
        r7 = 0xff;
        fn_800D5CB8();
        f1 = f31;
        r3 = 0x0;
        f2 = f27;
        fn_800D59B8();
        f28 = f29 + f27;
        r3 = 0x1;
        f1 = f29 + f31;
        f2 = f28;
        fn_800D59B8();
        f0 = lbl_8047B7E0;
        f2 = f28;
        r3 = 0x2;
        f27 = f0 - f29;
        f1 = f27 + f31;
        fn_800D59B8();
        r3 = *(s16*)((u8*)r29 + 0x54);
        r4 = *(s16*)((u8*)r29 + 0x56);
        fn_800D61E4();
        r3 = 0x0;
        r4 = 0xff;
        r5 = 0x40;
        r6 = 0x40;
        r7 = 0xff;
        fn_800D5CB8();
        f1 = f30;
        r3 = 0x0;
        f2 = f26;
        fn_800D59B8();
        f28 = f29 + f26;
        r3 = 0x1;
        f1 = f29 + f30;
        f2 = f28;
        fn_800D59B8();
        f2 = f28;
        r3 = 0x2;
        f1 = f27 + f30;
        fn_800D59B8();
        fn_800D6728();
        r3 = 0x1;
        fn_800DC1D4();
        r3 = 0x80000000;
        fn_800D888C();
    }
    r3 = 0x0;
    return;
}
#endif

/* 0x64 | fn_8001DFA8 | generic_call_check_store */
/* fn_8001DFA8 - 0x8001DFA8 | size: 0x64 */
#pragma push
#pragma peephole off
s32 fn_8001DFA8(u32 arg1, u8* arg2) {
    extern u8 fn_8005D8B8(s16);
    if (fn_8005D8B8(*(s16*)(arg2 + 0x6)) != 0) {
        *(u8*)(arg2 + 0x66) = 0xff;
        *(u8*)(arg2 + 0x65) = 0xff;
        *(u8*)(arg2 + 0x64) = 0xff;
    } else {
        *(u8*)(arg2 + 0x66) = 0x80;
        *(u8*)(arg2 + 0x65) = 0x80;
        *(u8*)(arg2 + 0x64) = 0x80;
    }
    return 0;
}
#pragma pop

/* 0x68 | fn_8001E00C | call_sequence */
/* fn_8001E00C - 0x8001E00C | size: 0x68 */
#if 0
asm void fn_8001E00C(void) {
#include "src/game/gs_pcbox_fn_8001E00C.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_8001E00C(u32 sp8) {
    extern void* fn_801046B8();
    extern s32 fn_801026A4(s32, ...);
    extern void fn_80102568(s32, s32, s32);
    s32 r31;
    r31 = fn_801026A4(0x43, fn_801046B8(), &sp8, 0, 1, 0);
    fn_80102568(0x43, 0, 1);
    return r31;
}
#pragma pop
#endif

/* 0x8001E3E0 | 0xD4 */
#if 0
asm void fn_8001E3E0(void) {
#include "src/game/gs_pcbox_fn_8001E3E0.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_8001E3E0(void* a, u32* b) {
    extern void* fn_801046B8();
    extern void fn_801026A4(s32, ...);
    extern void fn_80102868();
    extern void fn_801043A4();
    extern void fn_801045A8();
    extern u8* fn_80104704();
    extern void fn_80102510();
    void* r4_tmp;
    u8* r3;
    s32 r31;
    void* a_save;
    r31 = 0;
    r4_tmp = fn_801046B8();
    a_save = a;
    fn_801026A4(0x2, r4_tmp, 0, 0, 0, 0x3, a_save, 0x1, 0);
    fn_80102868(0x2, 0x32, 0x3c);
    fn_801045A8(0x2, 0x1);
    fn_801043A4(0x2);
    r3 = fn_80104704(0x2);
    if (r3 != 0) {
        if (b != 0) *b = *(u32*)(r3 + 0x80);
        if (*(u8*)(r3 + 0x99) == 0) r31 = 1;
        fn_80102510(0x2);
    }
    return r31;
}
#pragma peephole reset
#endif

/* 0x8001E4B4 | 0xD8 */
extern u8 lbl_80314E08[];
#if 0
asm void fn_8001E4B4(void) {
#include "src/game/gs_pcbox_fn_8001E4B4.inc"
}
#else
#pragma optimization_level 4
void fn_8001E4B4(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f) {
    extern void fn_800D5CB8(s32, s32, s32, s32, s32);
    extern void fn_800D61E4(s32, s32);
    extern void fn_800D6728();
    extern void fn_800D67BC(s32);
    extern void fn_800D6A00(s32);
    extern void fn_800D7820(u8*);
    extern void fn_800D888C(s32);
    extern void fn_800D88DC(s32);
    fn_800D88DC(0x1);
    fn_800D888C(0x6);
    fn_800D6A00(0x3);
    fn_800D7820(lbl_80314E08);
    fn_800D67BC(0x3);
    fn_800D61E4((s16)a, (s16)b);
    fn_800D5CB8(0x0, 0xff, 0xff, 0xff, 0xff);
    fn_800D61E4((s16)c, (s16)d);
    fn_800D5CB8(0x0, 0xff, 0xff, 0xff, 0xff);
    fn_800D61E4((s16)e, (s16)f);
    fn_800D5CB8(0x0, 0xff, 0xff, 0xff, 0xff);
    fn_800D6728();
}
#endif

/* 0x8001E644 | 0x454 */
extern void fn_800D5BA0(void);
extern void fn_800D5648(void);
extern void fn_8005D858(void);
extern void fn_801040F0(void);
extern void fn_8005D934(void);
extern void fn_80104160(void);
extern u8 lbl_80266C20[];
extern f64 lbl_8047B7D8;
extern f32 lbl_8047B7E4;
extern f32 lbl_8047B7E0;
#if 0
asm void fn_8001E644(void) {
#include "src/game/gs_pcbox_fn_8001E644.inc"
}
#else
void fn_8001E644(void) {
    extern u8 lbl_80266C20[];
    extern u8 lbl_80314E08[];
    extern f64 lbl_8047B7D8;
    extern f32 lbl_8047B7E0;
    extern f32 lbl_8047B7E4;
    extern void fn_8005D858();
    extern void fn_8005D934();
    extern void fn_800D5648();
    extern void fn_800D5BA0();
    extern void fn_800D61E4();
    extern void fn_800D6728();
    extern void fn_800D67BC();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D888C();
    extern void fn_800D88DC();
    extern void fn_801040F0();
    extern void fn_80104160();
    u8 sp[0x80];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;

    r28 = r3;
    r25 = r4;
    r27 = r5;
    r26 = r6;
    r6 = 0x43300000;
    tmp = r7 & 0xFF;
    *(u32*)(sp + 0x1C) = tmp;
    r3 = (u32)lbl_80266C20;
    r7 = (u32)lbl_80266C20;
    f3 = lbl_8047B7D8;
    r3 = 0x1;
    r5 = *(u32*)((u8*)r7 + 0x0);
    f0 = lbl_8047B7E4;
    f1 = f1 - f3;
    r4 = *(u32*)((u8*)r7 + 0x4);
    tmp = *(u32*)((u8*)r7 + 0x8);
    f4 = f1 / f0;
    r5 = *(u8*)(sp + 0xB);
    *(u32*)(sp + 0x10) = tmp;
    r4 = *(u8*)(sp + 0xF);
    tmp = *(u8*)(sp + 0x13);
    f1 = f0 - f3;
    f2 = f1 * f4;
    *(u32*)(sp + 0x44) = tmp;
    f1 = f0 - f3;
    f2 = (f64)(s32)f2;
    f1 = f1 * f4;
    f0 = f0 - f3;
    f1 = (f64)(s32)f1;
    f0 = f0 * f4;
    *(u8*)(sp + 0xB) = tmp;
    f0 = (f64)(s32)f0;
    *(u8*)(sp + 0xF) = r4;
    *(u8*)(sp + 0x13) = tmp;
    fn_800D88DC();
    r3 = 0x6;
    fn_800D888C();
    r3 = 0x6;
    fn_800D6A00();
    r3 = (u32)lbl_80314E08;
    r3 = (u32)lbl_80314E08;
    fn_800D7820();
    r31 = (s16)r3;
    r3 = 0x4;
    r23 = (s16)tmp;
    fn_800D67BC();
    r3 = r31;
    r4 = r23;
    fn_800D61E4();
    r3 = 0x0;
    fn_800D5BA0();
    tmp = r27 + 0x14;
    r4 = r23;
    tmp = (s16)tmp;
    r30 = r31 + tmp;
    r3 = (s16)r30;
    fn_800D61E4();
    r3 = 0x0;
    fn_800D5BA0();
    tmp = r26 + 0x14;
    r3 = (s16)r30;
    r24 = (s16)tmp;
    r29 = r23 + r24;
    r4 = (s16)r29;
    fn_800D61E4();
    r3 = 0x0;
    fn_800D5BA0();
    r3 = r31;
    r4 = (s16)r29;
    fn_800D61E4();
    r3 = 0x0;
    fn_800D5BA0();
    fn_800D6728();
    r3 = 0x1;
    fn_800D6A00();
    r3 = (u32)lbl_80314E08;
    r3 = (u32)lbl_80314E08;
    fn_800D7820();
    f1 = lbl_8047B7E0;
    fn_800D5648();
    r29 = r23;
    r23 = 0x0;
    while ((s32)r23 < (s32)r24) {

        r3 = 0x2;
        fn_800D67BC();
        r3 = r31;
        r4 = (s16)r29;
        fn_800D61E4();
        r3 = 0x0;
        fn_800D5BA0();
        r3 = (s16)r30;
        r4 = (s16)r29;
        fn_800D61E4();
        r3 = 0x0;
        fn_800D5BA0();
        fn_800D6728();
        r29 = r29 + 0x4;
        r23 = r23 + 0x4;

    }
    r3 = 0xbb;
    fn_8005D858();
    r6 = *(s16*)((u8*)r3 + 0xC);
    r5 = r28 + r27;
    r4 = *(s16*)((u8*)r3 + 0xE);
    r3 = 0xb8;
    tmp = (u32)r6 >> 31;
    r6 = tmp + r6;
    tmp = (u32)r4 >> 31;
    r8 = (s32)r6 >> 1;
    tmp = tmp + r4;
    r9 = (s32)tmp >> 1;
    r7 = r28 - r8;
    r4 = r25 - r9;
    r6 = r28 + r8;
    r5 = r5 - r8;
    r31 = (s16)r7;
    tmp = r25 + tmp;
    r30 = (s16)r6;
    r29 = (s16)r5;
    r28 = (s16)r4;
    r27 = (s16)tmp;
    fn_8005D858();
    r7 = *(s16*)((u8*)r3 + 0xE);
    tmp = r25 + r26;
    r3 = r31;
    r4 = r28;
    r6 = (u32)r7 >> 31;
    r5 = 0x0;
    r7 = r6 + r7;
    r6 = 0xbb;
    r8 = (s32)r7 >> 1;
    r7 = 0x0;
    r8 = tmp - r8;
    tmp = r8 + 0xa;
    r26 = (s16)tmp;
    fn_801040F0();
    r3 = r29;
    r4 = r28;
    r5 = 0x0;
    r6 = 0xbb;
    r7 = 0x1;
    fn_801040F0();
    r3 = r31;
    r4 = r26;
    r5 = 0x0;
    r6 = 0xb8;
    r7 = 0x0;
    fn_801040F0();
    r3 = r29;
    r4 = r26;
    r5 = 0x0;
    r6 = 0xb8;
    r7 = 0x1;
    fn_801040F0();
    r3 = 0xba;
    fn_8005D858();
    r24 = r3;
    r3 = 0x84;
    fn_8005D934();
    r25 = r3;
    r3 = 0x87;
    fn_8005D934();
    r7 = *(s16*)((u8*)r25 + 0x2);
    r23 = r26 - r27;
    tmp = *(s16*)((u8*)r3 + 0x2);
    r4 = r27;
    r5 = *(s16*)((u8*)r24 + 0xC);
    r6 = (s16)r23;
    tmp = tmp - r7;
    r7 = -0x1;
    tmp = (s16)tmp;
    r8 = 0x0;
    tmp = r31 + tmp;
    r9 = 0xba;
    r3 = (s16)tmp;
    r10 = 0x0;
    fn_80104160();
    r3 = 0x85;
    fn_8005D934();
    r25 = r3;
    r3 = 0x86;
    fn_8005D934();
    r8 = *(s16*)((u8*)r25 + 0x2);
    r4 = r27;
    tmp = *(s16*)((u8*)r3 + 0x2);
    r6 = (s16)r23;
    r5 = *(s16*)((u8*)r24 + 0xC);
    r7 = -0x1;
    tmp = tmp - r8;
    r8 = 0x0;
    tmp = (s16)tmp;
    r9 = 0xba;
    tmp = r29 + tmp;
    r10 = 0x0;
    r3 = (s16)tmp;
    fn_80104160();
    r3 = 0xb7;
    fn_8005D858();
    r27 = r3;
    r3 = 0x84;
    fn_8005D934();
    r25 = r3;
    r3 = 0x8b;
    fn_8005D934();
    r5 = r3;
    r23 = r29 - r30;
    r4 = *(s16*)((u8*)r25 + 0x4);
    r3 = r30;
    tmp = *(s16*)((u8*)r5 + 0x4);
    r5 = (s16)r23;
    r6 = *(s16*)((u8*)r27 + 0xE);
    r7 = -0x1;
    tmp = tmp - r4;
    r8 = 0x0;
    tmp = (s16)tmp;
    r9 = 0xb7;
    tmp = r28 + tmp;
    r10 = 0x0;
    r4 = (s16)tmp;
    fn_80104160();
    r3 = 0x88;
    fn_8005D934();
    r25 = r3;
    r3 = 0x8a;
    fn_8005D934();
    tmp = *(s16*)((u8*)r3 + 0x4);
    r3 = r30;
    r4 = *(s16*)((u8*)r25 + 0x4);
    r5 = (s16)r23;
    r6 = *(s16*)((u8*)r27 + 0xE);
    r7 = -0x1;
    tmp = tmp - r4;
    r8 = 0x0;
    tmp = (s16)tmp;
    r9 = 0xb7;
    tmp = r26 + tmp;
    r10 = 0x0;
    r4 = (s16)tmp;
    fn_80104160();
    return;
}
#endif

/* 0x8001EA98 | 0x170 */
extern f32 lbl_8047B7E0;
#if 0
asm void fn_8001EA98(void) {
#include "src/game/gs_pcbox_fn_8001EA98.inc"
}
#else
void fn_8001EA98(void) {
    extern u8 lbl_80314E08[];
    extern f32 lbl_8047B7E0;
    extern void fn_800D5648();
    extern void fn_800D5BA0();
    extern void fn_800D61E4();
    extern void fn_800D6728();
    extern void fn_800D67BC();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D888C();
    extern void fn_800D88DC();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r28 = r3;
    r29 = r4;
    r31 = r5;
    r30 = r6;
    r3 = 0x1;
    fn_800D88DC();
    r3 = 0x6;
    fn_800D888C();
    r3 = 0x7;
    fn_800D6A00();
    r3 = (u32)lbl_80314E08;
    r3 = (u32)lbl_80314E08;
    fn_800D7820();
    r3 = 0x2;
    fn_800D67BC();
    r3 = (s16)r3;
    r4 = (s16)tmp;
    fn_800D61E4();
    r3 = 0x0;
    r4 = 0xc0;
    fn_800D5BA0();
    r31 = r31 + 0xa;
    r30 = r30 + 0xa;
    r31 = r28 + r31;
    r30 = r29 + r30;
    r3 = (s16)r31;
    r4 = (s16)r30;
    fn_800D61E4();
    r3 = 0x0;
    r4 = 0xc0;
    fn_800D5BA0();
    fn_800D6728();
    f1 = lbl_8047B7E0;
    fn_800D5648();
    r3 = 0x2;
    fn_800D6A00();
    r3 = 0x5;
    fn_800D67BC();
    r3 = (s16)r3;
    r4 = (s16)tmp;
    fn_800D61E4();
    r3 = 0x0;
    r4 = -0x1;
    fn_800D5BA0();
    r3 = (s16)r31;
    r4 = (s16)tmp;
    fn_800D61E4();
    r3 = 0x0;
    r4 = -0x1;
    fn_800D5BA0();
    r3 = (s16)r31;
    r4 = (s16)r30;
    fn_800D61E4();
    r3 = 0x0;
    r4 = -0x1;
    fn_800D5BA0();
    r4 = (s16)r30;
    r3 = (s16)tmp;
    fn_800D61E4();
    r3 = 0x0;
    r4 = -0x1;
    fn_800D5BA0();
    r3 = (s16)r3;
    r4 = (s16)tmp;
    fn_800D61E4();
    r3 = 0x0;
    r4 = -0x1;
    fn_800D5BA0();
    fn_800D6728();
    return;
}
#endif

/* 0x8001EC08 | 0x370 */
extern void fn_800CE148(void);
extern void fn_800CDBE0(void);
extern void _savefpr_26(void);
extern void _restfpr_26(void);
extern u8 lbl_803A1D60[];
extern f64 lbl_8047B7F0;
extern f64 lbl_8047B7D0;
extern f64 lbl_8047B7F8;
extern f64 lbl_8047B7E8;
extern f64 lbl_8047B800;
#if 0
asm void fn_8001EC08(void) {
#include "src/game/gs_pcbox_fn_8001EC08.inc"
}
#else
void fn_8001EC08(void) {
    extern u8 lbl_80314E08[];
    extern u8 lbl_803A1D60[];
    extern f64 lbl_8047B7D0;
    extern f64 lbl_8047B7E8;
    extern f64 lbl_8047B7F0;
    extern f64 lbl_8047B7F8;
    extern f64 lbl_8047B800;
    extern void fn_800CDBE0();
    extern void fn_800CE148();
    extern void fn_800D5BA0();
    extern void fn_800D61E4();
    extern void fn_800D6728();
    extern void fn_800D67BC();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D888C();
    extern void fn_800D88DC();
    u8 sp[0xB0];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f26 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;

    r11 = (u32)sp + 0xb0;
    tmp = r8 & 0xFF;
    r8 = (u32)lbl_803A1D60;
    r27 = (u32)lbl_803A1D60;
    if (tmp != 0) {
        r7 = r7 & 0xFF;
        tmp = (u32)r7 >> 31;
        tmp = tmp + r7;
        tmp = (s32)tmp >> 1;
        r7 = tmp & 0xFF;
    }
    r8 = (u32)r5 >> 31;
    tmp = (u32)r6 >> 31;
    r8 = r8 + r5;
    r9 = r6 + r4;
    r8 = (s32)r8 >> 1;
    tmp = tmp + r6;
    tmp = (s32)tmp >> 1;
    r6 = r5 + r3;
    r9 = r9 + 0x2;
    r5 = r8 + r3;
    tmp = tmp + r4;
    r10 = (s16)r6;
    r28 = r27 + 0x0;
    r9 = (s16)r9;
    r8 = (s16)r4;
    r6 = (s16)r3;
    r5 = (s16)r5;
    tmp = (s16)tmp;
    r24 = r7 & 0xFF;
    r23 = r27 + 0x128;
    r22 = r27 + 0x28;
    *(u16*)((u8*)r27 + 0x0) = r10;
    r26 = r23;
    r21 = 0x0;
    *(u16*)((u8*)r28 + 0x2) = r9;
    r25 = r22;
    *(u16*)((u8*)r28 + 0x8) = r10;
    *(u16*)((u8*)r28 + 0xA) = r8;
    *(u16*)((u8*)r28 + 0x10) = r6;
    *(u16*)((u8*)r28 + 0x12) = r8;
    *(u16*)((u8*)r28 + 0x18) = r6;
    *(u16*)((u8*)r28 + 0x1A) = r9;
    *(u16*)((u8*)r28 + 0x20) = r5;
    *(u16*)((u8*)r28 + 0x22) = tmp;
    *(u32*)((u8*)r28 + 0x24) = r24;
    f28 = lbl_8047B7F0;
    f29 = lbl_8047B7D0;
    f30 = lbl_8047B7F8;
    r29 = 0x43300000;
    f31 = lbl_8047B7E8;
    f27 = lbl_8047B800;
    do {
        *(u32*)(sp + 0xC) = tmp;
        f0 = f0 - f29;
        f0 = f28 * f0;
        f26 = f0 * f30;
        f1 = f26;
        fn_800CE148();
        f0 = f0 - f29;
        f0 = f31 * f1 + f0;
        f1 = f26;
        f0 = (f64)(s32)f0;
        *(u16*)((u8*)r26 + 0x0) = tmp;
        fn_800CDBE0();
        tmp = 0x0;
        f0 = f0 - f29;
        f0 = f31 * f1 + f0;
        f1 = f26;
        f0 = (f64)(s32)f0;
        *(u16*)((u8*)r26 + 0x2) = r3;
        *(u32*)((u8*)r26 + 0x4) = tmp;
        fn_800CE148();
        f0 = f0 - f29;
        f0 = f27 * f1 + f0;
        f1 = f26;
        f0 = (f64)(s32)f0;
        *(u16*)((u8*)r25 + 0x0) = tmp;
        fn_800CDBE0();
        tmp = (s32)r21 >> 3;
        r3 = r27 + 0x0;
        tmp = tmp << 3;
        r3 = r3 + tmp;
        tmp = *(s16*)((u8*)r26 + 0x0);
        r5 = r3 + 0x2;
        r4 = *(s16*)((u8*)r3 + 0x0);
        r21 = r21 + 0x1;
        f0 = f0 - f29;
        r3 = *(s16*)((u8*)r5 + 0x0);
        tmp = tmp + r4;
        *(u16*)((u8*)r26 + 0x0) = tmp;
        f0 = f27 * f1 + f0;
        tmp = *(s16*)((u8*)r26 + 0x2);
        tmp = tmp + r3;
        f0 = (f64)(s32)f0;
        *(u16*)((u8*)r26 + 0x2) = tmp;
        r26 = r26 + 0x8;
        *(u16*)((u8*)r25 + 0x2) = tmp;
        *(u32*)((u8*)r25 + 0x4) = r24;
        tmp = *(s16*)((u8*)r25 + 0x0);
        tmp = tmp + r4;
        *(u16*)((u8*)r25 + 0x0) = tmp;
        tmp = *(s16*)((u8*)r25 + 0x2);
        tmp = tmp + r3;
        *(u16*)((u8*)r25 + 0x2) = tmp;
        r25 = r25 + 0x8;
    } while ((s32)r21 < 0x20);
    r3 = 0x1;
    fn_800D88DC();
    r3 = 0x6;
    fn_800D888C();
    r3 = 0x4;
    fn_800D6A00();
    r3 = (u32)lbl_80314E08;
    r3 = (u32)lbl_80314E08;
    fn_800D7820();
    r3 = 0x42;
    fn_800D67BC();
    r21 = r22;
    r24 = 0x0;
    do {
        r3 = *(s16*)((u8*)r23 + 0x0);
        r4 = *(s16*)((u8*)r23 + 0x2);
        fn_800D61E4();
        r4 = *(u32*)((u8*)r23 + 0x4);
        r3 = 0x0;
        fn_800D5BA0();
        r3 = *(s16*)((u8*)r21 + 0x0);
        r4 = *(s16*)((u8*)r21 + 0x2);
        fn_800D61E4();
        r4 = *(u32*)((u8*)r21 + 0x4);
        r3 = 0x0;
        fn_800D5BA0();
        r23 = r23 + 0x8;
        r21 = r21 + 0x8;
        r24 = r24 + 0x1;
    } while ((s32)r24 < 0x20);
    r4 = r27 + 0x128;
    r3 = *(s16*)((u8*)r27 + 0x128);
    r4 = *(s16*)((u8*)r4 + 0x2);
    fn_800D61E4();
    r4 = r27 + 0x128;
    r3 = 0x0;
    r4 = *(u32*)((u8*)r4 + 0x4);
    fn_800D5BA0();
    r21 = r27 + 0x28;
    r3 = *(s16*)((u8*)r27 + 0x28);
    r4 = *(s16*)((u8*)r21 + 0x2);
    fn_800D61E4();
    r23 = r21;
    r3 = 0x0;
    r4 = *(u32*)((u8*)r23 + 0x4);
    fn_800D5BA0();
    fn_800D6728();
    r3 = 0x5;
    fn_800D6A00();
    r3 = 0x22;
    fn_800D67BC();
    r3 = *(s16*)((u8*)r28 + 0x20);
    r4 = *(s16*)((u8*)r28 + 0x22);
    fn_800D61E4();
    r4 = *(u32*)((u8*)r28 + 0x24);
    r3 = 0x0;
    fn_800D5BA0();
    r24 = 0x0;
    do {
        r3 = *(s16*)((u8*)r22 + 0x0);
        r4 = *(s16*)((u8*)r22 + 0x2);
        fn_800D61E4();
        r4 = *(u32*)((u8*)r22 + 0x4);
        r3 = 0x0;
        fn_800D5BA0();
        r22 = r22 + 0x8;
        r24 = r24 + 0x1;
    } while ((s32)r24 < 0x20);
    r3 = *(s16*)((u8*)r27 + 0x28);
    r4 = *(s16*)((u8*)r21 + 0x2);
    fn_800D61E4();
    r4 = *(u32*)((u8*)r23 + 0x4);
    r3 = 0x0;
    fn_800D5BA0();
    fn_800D6728();
    r11 = (u32)sp + 0xb0;
    return;
}
#endif

/* 0x8001EF78 | 0x270 */
extern void fn_801666BC(void);
extern f64 lbl_8047B828;
extern f64 lbl_8047B830;
extern f32 lbl_8047B810;
extern f32 lbl_8047A338;
extern f32 lbl_8047B818;
extern f32 lbl_8047A344;
extern f32 lbl_8047B814;
extern u32 lbl_8047A31C;
extern f32 lbl_8047A334;
extern u8 lbl_803A1F88[];
extern f32 lbl_8047B81C;
extern f32 lbl_8047B820;
extern f32 lbl_8047B824;
#if 0
asm void fn_8001EF78(void) {
#include "src/game/gs_pcbox_fn_8001EF78.inc"
}
#else
void fn_8001EF78(void) {
    extern u8 lbl_803A1F88[];
    extern u32 lbl_8047A31C;
    extern f32 lbl_8047A334;
    extern f32 lbl_8047A338;
    extern f32 lbl_8047A344;
    extern f32 lbl_8047B810;
    extern f32 lbl_8047B814;
    extern f32 lbl_8047B818;
    extern f32 lbl_8047B81C;
    extern f32 lbl_8047B820;
    extern f32 lbl_8047B824;
    extern f64 lbl_8047B828;
    extern f64 lbl_8047B830;
    extern void fn_800D3088();
    extern void fn_800D37CC();
    extern void fn_801666BC();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    fn_800D37CC();
    tmp = 0x43300000;
    f1 = lbl_8047B828;
    *(u32*)(sp + 0x8) = tmp;
    f31 = f0 - f1;
    fn_800D3088();
    tmp = 0x43300000;
    f3 = lbl_8047B830;
    *(u32*)(sp + 0x10) = tmp;
    f2 = lbl_8047B810;
    f1 = lbl_8047A338;
    f3 = f0 - f3;
    f0 = lbl_8047B818;
    f3 = f3 / f31;
    f1 = f3 * f2 + f1;
    lbl_8047A344 = f3;
    lbl_8047A338 = f1;
    /* cror eq, gt, eq */;
    if (f1 == f0) {
        f0 = lbl_8047B814;
        lbl_8047A338 = f0;
    }
    tmp = lbl_8047A31C;
    if ((s32)tmp < 0x1e) return;
    if ((s32)tmp == 0xc8) return;
    f0 = lbl_8047A334;
    r3 = 0x46a;
    f0 = f0 + f3;
    lbl_8047A334 = f0;
    fn_801666BC();
    if ((s32)r3 == 0) {
        tmp = lbl_8047A31C;
        f0 = lbl_8047B814;
        lbl_8047A334 = f0;
        if ((s32)tmp != 0x3e8) {
            tmp = 0x28;
            lbl_8047A31C = tmp;
    }
    }
    r3 = (u32)lbl_803A1F88;
    f0 = lbl_8047A344;
    r3 = (u32)lbl_803A1F88;
    tmp = 0x3;
    ctr_fn = (void(*)(void))tmp;
    do {
        f1 = *(f32*)((u8*)r3 + 0x10);
        f2 = *(f32*)((u8*)r3 + 0x28);
        do {
            if (f1 == f2) break;
            f3 = f2 - f1;
            f2 = lbl_8047B81C;
            f1 = lbl_8047B820;
            f2 = f2 * f3;
            f4 = f2 * f0;
            if (f4 > f1) {
                f4 = f1;
            }
            f1 = lbl_8047B824;
            /* cror eq, lt, eq */;
            if (f4 == f1) {
                f4 = f1;
            }
            f2 = *(f32*)((u8*)r3 + 0x10);
            f1 = lbl_8047B814;
            f2 = f2 + f4;
            *(f32*)((u8*)r3 + 0x10) = f2;
            f3 = *(f32*)((u8*)r3 + 0x28);
            f1 = *(f32*)((u8*)r3 + 0x10);
            f2 = f3 - f1;
            if (f4 > f1) {
            } else {

                f4 = -f4;
            }
            f1 = lbl_8047B814;
            if (f2 > f1) {
                f1 = f2;
            } else {

                f1 = -f2;
            }
            /* cror eq, lt, eq */;
            if (f1 != f4) {
                f1 = lbl_8047B814;
                if (f2 > f1) {
                } else {

                    f2 = -f2;
                }
                f1 = lbl_8047B818;
                if (f2 >= f1) break;
            }
            *(f32*)((u8*)r3 + 0x10) = f3;
        } while (0);
        f1 = *(f32*)((u8*)r3 + 0x1C);
        f2 = *(f32*)((u8*)r3 + 0x34);
        do {
            if (f1 == f2) break;
            f3 = f2 - f1;
            f2 = lbl_8047B81C;
            f1 = lbl_8047B820;
            f2 = f2 * f3;
            f4 = f2 * f0;
            if (f4 > f1) {
                f4 = f1;
            }
            f1 = lbl_8047B824;
            /* cror eq, lt, eq */;
            if (f4 == f1) {
                f4 = f1;
            }
            f2 = *(f32*)((u8*)r3 + 0x1C);
            f1 = lbl_8047B814;
            f2 = f2 + f4;
            *(f32*)((u8*)r3 + 0x1C) = f2;
            f3 = *(f32*)((u8*)r3 + 0x34);
            f1 = *(f32*)((u8*)r3 + 0x1C);
            f2 = f3 - f1;
            if (f4 > f1) {
            } else {

                f4 = -f4;
            }
            f1 = lbl_8047B814;
            if (f2 > f1) {
                f1 = f2;
            } else {

                f1 = -f2;
            }
            /* cror eq, lt, eq */;
            if (f1 != f4) {
                f1 = lbl_8047B814;
                if (f2 > f1) {
                } else {

                    f2 = -f2;
                }
                f1 = lbl_8047B818;
                if (f2 >= f1) break;
            }
            *(f32*)((u8*)r3 + 0x1C) = f3;
        } while (0);
        r3 = r3 + 0x4;
    } while (--ctr != 0);

    return;
}
#endif

/* 0x8001F1E8 | 0x11C */
extern void fn_801337A8(void);
extern void fn_801669E4(void);
extern void fn_80105624(void);
extern void fn_80166AB8(void);
extern u32 lbl_8047A31C;
extern u32 lbl_8047A328;
#pragma peephole off
void fn_8001F1E8(u8* arg) {
    extern u32 lbl_8047A31C;
    extern u32 lbl_8047A328;
    extern u32 fn_800F7AF0(s32);
    extern u32 fn_800F7BC4(s32);
    extern u8* fn_80105624(void);
    extern void fn_801337A8(s32);
    extern void fn_801669E4(s32, s32, s32);
    extern void fn_80166AB8(s32, s32, s32);
    extern void* fn_8005DA18(u32);
    u32 a;
    u32 b;
    u8* obj;

    if ((s32)lbl_8047A31C < 4) {
        fn_801337A8(0);
        if (arg == 0) return;
        fn_8005DA18(*(u32*)(arg + 0x4));
        a = fn_800F7AF0(1);
        b = fn_800F7BC4(1);
        if ((b & a) & 0x1100) {
            lbl_8047A31C = 4;
            fn_801669E4(0x46e, 0, 0);
            lbl_8047A328 = 1;
        }
        a = fn_800F7AF0(1);
        b = fn_800F7BC4(1);
        if (((b & a) & 0x200) == 0) return;
        lbl_8047A31C = 4;
        fn_801669E4(0x46e, 0, 0);
        lbl_8047A328 = 1;
        return;
    }
    fn_801337A8(1);
    if (arg == 0) return;
    fn_8005DA18(*(u32*)(arg + 0x4));
    obj = fn_80105624();
    if ((*(u16*)(obj + 0x4) & 0x810) == 0) return;
    *(u8*)(arg + 0x98) = 1;
    fn_80166AB8(0x4c2, 0, 0);
}
#pragma peephole reset

/* 0x8001F304 | 0xA44 */
extern u32 lbl_8047A31C;
extern f32 lbl_8047B838;
extern f32 lbl_8047A338;
extern f32 lbl_8047B81C;
extern f32 lbl_8047B83C;
extern f32 lbl_8047B844;
extern f32 lbl_8047B840;
extern f32 lbl_8047B848;
extern f32 lbl_8047B814;
extern u32 lbl_80478878;
extern u8 lbl_802EF0A8[];
extern u8 lbl_8047A34C;
#if 0
asm void fn_8001F304(void) {
#include "src/game/gs_pcbox_fn_8001F304.inc"
}
#else
void fn_8001F304(void) {
    extern u8 lbl_802EF0A8[];
    extern u8 lbl_803A1F88[];
    extern u32 lbl_80478878;
    extern u32 lbl_8047A31C;
    extern f32 lbl_8047A338;
    extern u8 lbl_8047A34C;
    extern f32 lbl_8047B814;
    extern f32 lbl_8047B81C;
    extern f32 lbl_8047B838;
    extern f32 lbl_8047B83C;
    extern f32 lbl_8047B840;
    extern f32 lbl_8047B844;
    extern f32 lbl_8047B848;
    extern void fn_800CE148();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f8 = 0.0f;
    f32 f9 = 0.0f;

    r31 = r4;
    tmp = *(s16*)((u8*)r31 + 0x6);
    if ((s32)tmp != 0xf03) {
        if ((s32)tmp < 0xf03) {
            if ((s32)tmp != 0xefd) {
                if ((s32)tmp < 0xefd) {
                    if ((s32)tmp != 0xefa) {
                        if ((s32)tmp < 0xefa) {
                            if ((s32)tmp != 0x2c4) {
                                goto L_8001FD18;
                            }
                            if ((s32)tmp < 0xefc) {
                                goto L_8001F4C8;
                            }
                            if ((s32)tmp == 0xf00) goto L_8001F6F8;
                            if ((s32)tmp < 0xf00) {
                                if ((s32)tmp >= 0xeff) goto L_8001F688;
                                goto L_8001F618;
                            }
                            if ((s32)tmp >= 0xf02) goto L_8001F7D8;
                            goto L_8001F768;
                        }
                        if ((s32)tmp == 0xf09) goto L_8001FAE8;
                        if ((s32)tmp < 0xf09) {
                            if ((s32)tmp == 0xf06) goto L_8001F998;
                            if ((s32)tmp < 0xf06) {
                                if ((s32)tmp >= 0xf05) goto L_8001F928;
                                goto L_8001F8B8;
                            }
                            if ((s32)tmp >= 0xf08) goto L_8001FA78;
                            goto L_8001FA08;
                        }
                        if ((s32)tmp == 0xf0c) goto L_8001FC38;
                        if ((s32)tmp < 0xf0c) {
                            if ((s32)tmp >= 0xf0b) goto L_8001FBC8;
                            goto L_8001FB58;
                        }
                        if ((s32)tmp >= 0xf0e) goto L_8001FD18;
                        goto L_8001FCA8;
                            }
                    tmp = lbl_8047A31C;
                    if ((s32)tmp >= 0x1e) {
                        if ((s32)tmp > 0x20) {
                        }
                        tmp = *(u8*)((u8*)r31 + 0x4);
                        r3 = 0x0;
                        tmp = tmp & 0xFFFFFFFD;
                        tmp = (s8)tmp;
                        *(u8*)((u8*)r31 + 0x4) = tmp;

                        } else {
                        tmp = *(u8*)((u8*)r31 + 0x4);
                        r3 = 0x1;
                        tmp = tmp | 0x2;
                        tmp = (s8)tmp;
                        *(u8*)((u8*)r31 + 0x4) = tmp;
                        }
                    tmp = r3 & 0xFF;
                    if (tmp == 0) return;
                    tmp = lbl_8047A31C;
                    f2 = lbl_8047B838;
                    f1 = lbl_8047A338;
                    if ((s32)tmp == 0x1f) {
                        f0 = lbl_8047B81C;
                    } else {

                        f0 = lbl_8047B83C;
                    }
                    f0 = f1 * f0;
                    f1 = f2 * f0;
                    fn_800CE148();
                    f3 = (f32)f1;
                    f2 = lbl_8047B844;
                    f1 = lbl_8047B840;
                    f0 = lbl_8047B848;
                    f1 = f2 * f3 + f1;
                    if (f1 > f0) {
                        f1 = f0;
                    }
                    f0 = lbl_8047B814;
                    if (f1 < f0) {
                        f1 = f0;
                    }
                    f0 = (f64)(s32)f1;
                    *(u8*)((u8*)r31 + 0x67) = tmp;
                    return;
                                }
                tmp = lbl_8047A31C;
                if ((s32)tmp >= 0x1e) {
                    if ((s32)tmp > 0x20) {
                    }
                    tmp = *(u8*)((u8*)r31 + 0x4);
                    tmp = tmp & 0xFFFFFFFD;
                    tmp = (s8)tmp;
                    *(u8*)((u8*)r31 + 0x4) = tmp;
                    return;
                    }
                tmp = *(u8*)((u8*)r31 + 0x4);
                tmp = tmp | 0x2;
                tmp = (s8)tmp;
                *(u8*)((u8*)r31 + 0x4) = tmp;
                return;
            L_8001F4C8:
                tmp = lbl_80478878;
                if ((s32)tmp == 0) {
                    tmp = *(u8*)((u8*)r31 + 0x4);
                    r3 = (u32)lbl_803A1F88;
                    r4 = (u32)lbl_802EF0A8;
                    tmp = tmp | 0x2;
                    r3 = (u32)lbl_803A1F88;
                    r5 = (s8)tmp;
                    tmp = (u32)lbl_802EF0A8;
                    *(u8*)((u8*)r31 + 0x4) = r5;
                    f0 = *(f32*)((u8*)r3 + 0x1C);
                    r3 = *(s16*)((u8*)r31 + 0x6);
                    f0 = (f64)(s32)f0;
                    r3 = r3 * 0x1c;
                    r3 = tmp + r3;
                    r3 = *(s16*)((u8*)r3 + 0x2);
                    tmp = r3 + tmp;
                    tmp = (s16)tmp;
                    *(u16*)((u8*)r31 + 0x50) = tmp;
                    return;
                }
                tmp = *(u8*)((u8*)r31 + 0x4);
                tmp = tmp & 0xFFFFFFFD;
                tmp = (s8)tmp;
                *(u8*)((u8*)r31 + 0x4) = tmp;
                return;
                            }
            tmp = lbl_80478878;
            if ((s32)tmp == 0) {
                tmp = *(u8*)((u8*)r31 + 0x4);
                r3 = (u32)lbl_803A1F88;
                r4 = (u32)lbl_802EF0A8;
                tmp = tmp | 0x2;
                r3 = (u32)lbl_803A1F88;
                r5 = (s8)tmp;
                tmp = (u32)lbl_802EF0A8;
                *(u8*)((u8*)r31 + 0x4) = r5;
                f0 = *(f32*)((u8*)r3 + 0x20);
                r3 = *(s16*)((u8*)r31 + 0x6);
                f0 = (f64)(s32)f0;
                r3 = r3 * 0x1c;
                r3 = tmp + r3;
                r3 = *(s16*)((u8*)r3 + 0x2);
                tmp = r3 + tmp;
                tmp = (s16)tmp;
                *(u16*)((u8*)r31 + 0x50) = tmp;
                return;
            }
            tmp = *(u8*)((u8*)r31 + 0x4);
            tmp = tmp & 0xFFFFFFFD;
            tmp = (s8)tmp;
            *(u8*)((u8*)r31 + 0x4) = tmp;
            return;
            }
        tmp = lbl_80478878;
        if ((s32)tmp == 0) {
            tmp = *(u8*)((u8*)r31 + 0x4);
            r3 = (u32)lbl_803A1F88;
            r4 = (u32)lbl_802EF0A8;
            tmp = tmp | 0x2;
            r3 = (u32)lbl_803A1F88;
            r5 = (s8)tmp;
            tmp = (u32)lbl_802EF0A8;
            *(u8*)((u8*)r31 + 0x4) = r5;
            f0 = *(f32*)((u8*)r3 + 0x10);
            r3 = *(s16*)((u8*)r31 + 0x6);
            f0 = (f64)(s32)f0;
            r3 = r3 * 0x1c;
            r3 = tmp + r3;
            r3 = *(s16*)((u8*)r3 + 0x2);
            tmp = r3 + tmp;
            tmp = (s16)tmp;
            *(u16*)((u8*)r31 + 0x50) = tmp;
            return;
        }
        tmp = *(u8*)((u8*)r31 + 0x4);
        tmp = tmp & 0xFFFFFFFD;
        tmp = (s8)tmp;
        *(u8*)((u8*)r31 + 0x4) = tmp;
        return;
    L_8001F618:
        tmp = lbl_80478878;
        if ((s32)tmp == 0) {
            tmp = *(u8*)((u8*)r31 + 0x4);
            r3 = (u32)lbl_803A1F88;
            r4 = (u32)lbl_802EF0A8;
            tmp = tmp | 0x2;
            r3 = (u32)lbl_803A1F88;
            r5 = (s8)tmp;
            tmp = (u32)lbl_802EF0A8;
            *(u8*)((u8*)r31 + 0x4) = r5;
            f0 = *(f32*)((u8*)r3 + 0x14);
            r3 = *(s16*)((u8*)r31 + 0x6);
            f0 = (f64)(s32)f0;
            r3 = r3 * 0x1c;
            r3 = tmp + r3;
            r3 = *(s16*)((u8*)r3 + 0x2);
            tmp = r3 + tmp;
            tmp = (s16)tmp;
            *(u16*)((u8*)r31 + 0x50) = tmp;
            return;
        }
        tmp = *(u8*)((u8*)r31 + 0x4);
        tmp = tmp & 0xFFFFFFFD;
        tmp = (s8)tmp;
        *(u8*)((u8*)r31 + 0x4) = tmp;
        return;
    L_8001F688:
        tmp = lbl_80478878;
        if ((s32)tmp == 1) {
            tmp = *(u8*)((u8*)r31 + 0x4);
            r3 = (u32)lbl_803A1F88;
            r4 = (u32)lbl_802EF0A8;
            tmp = tmp | 0x2;
            r3 = (u32)lbl_803A1F88;
            r5 = (s8)tmp;
            tmp = (u32)lbl_802EF0A8;
            *(u8*)((u8*)r31 + 0x4) = r5;
            f0 = *(f32*)((u8*)r3 + 0x1C);
            r3 = *(s16*)((u8*)r31 + 0x6);
            f0 = (f64)(s32)f0;
            r3 = r3 * 0x1c;
            r3 = tmp + r3;
            r3 = *(s16*)((u8*)r3 + 0x2);
            tmp = r3 + tmp;
            tmp = (s16)tmp;
            *(u16*)((u8*)r31 + 0x50) = tmp;
            return;
        }
        tmp = *(u8*)((u8*)r31 + 0x4);
        tmp = tmp & 0xFFFFFFFD;
        tmp = (s8)tmp;
        *(u8*)((u8*)r31 + 0x4) = tmp;
        return;
    L_8001F6F8:
        tmp = lbl_80478878;
        if ((s32)tmp == 1) {
            tmp = *(u8*)((u8*)r31 + 0x4);
            r3 = (u32)lbl_803A1F88;
            r4 = (u32)lbl_802EF0A8;
            tmp = tmp | 0x2;
            r3 = (u32)lbl_803A1F88;
            r5 = (s8)tmp;
            tmp = (u32)lbl_802EF0A8;
            *(u8*)((u8*)r31 + 0x4) = r5;
            f0 = *(f32*)((u8*)r3 + 0x20);
            r3 = *(s16*)((u8*)r31 + 0x6);
            f0 = (f64)(s32)f0;
            r3 = r3 * 0x1c;
            r3 = tmp + r3;
            r3 = *(s16*)((u8*)r3 + 0x2);
            tmp = r3 + tmp;
            tmp = (s16)tmp;
            *(u16*)((u8*)r31 + 0x50) = tmp;
            return;
        }
        tmp = *(u8*)((u8*)r31 + 0x4);
        tmp = tmp & 0xFFFFFFFD;
        tmp = (s8)tmp;
        *(u8*)((u8*)r31 + 0x4) = tmp;
        return;
    L_8001F768:
        tmp = lbl_80478878;
        if ((s32)tmp == 1) {
            tmp = *(u8*)((u8*)r31 + 0x4);
            r3 = (u32)lbl_803A1F88;
            r4 = (u32)lbl_802EF0A8;
            tmp = tmp | 0x2;
            r3 = (u32)lbl_803A1F88;
            r5 = (s8)tmp;
            tmp = (u32)lbl_802EF0A8;
            *(u8*)((u8*)r31 + 0x4) = r5;
            f0 = *(f32*)((u8*)r3 + 0x24);
            r3 = *(s16*)((u8*)r31 + 0x6);
            f0 = (f64)(s32)f0;
            r3 = r3 * 0x1c;
            r3 = tmp + r3;
            r3 = *(s16*)((u8*)r3 + 0x2);
            tmp = r3 + tmp;
            tmp = (s16)tmp;
            *(u16*)((u8*)r31 + 0x50) = tmp;
            return;
        }
        tmp = *(u8*)((u8*)r31 + 0x4);
        tmp = tmp & 0xFFFFFFFD;
        tmp = (s8)tmp;
        *(u8*)((u8*)r31 + 0x4) = tmp;
        return;
    L_8001F7D8:
        tmp = lbl_80478878;
        if ((s32)tmp == 1) {
            tmp = *(u8*)((u8*)r31 + 0x4);
            r3 = (u32)lbl_803A1F88;
            r4 = (u32)lbl_802EF0A8;
            tmp = tmp | 0x2;
            r3 = (u32)lbl_803A1F88;
            r5 = (s8)tmp;
            tmp = (u32)lbl_802EF0A8;
            *(u8*)((u8*)r31 + 0x4) = r5;
            f0 = *(f32*)((u8*)r3 + 0x10);
            r3 = *(s16*)((u8*)r31 + 0x6);
            f0 = (f64)(s32)f0;
            r3 = r3 * 0x1c;
            r3 = tmp + r3;
            r3 = *(s16*)((u8*)r3 + 0x2);
            tmp = r3 + tmp;
            tmp = (s16)tmp;
            *(u16*)((u8*)r31 + 0x50) = tmp;
            return;
        }
        tmp = *(u8*)((u8*)r31 + 0x4);
        tmp = tmp & 0xFFFFFFFD;
        tmp = (s8)tmp;
        *(u8*)((u8*)r31 + 0x4) = tmp;
        return;
    }
    tmp = lbl_80478878;
    if ((s32)tmp == 1) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x14);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001F8B8:
    tmp = lbl_80478878;
    if ((s32)tmp == 1) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x18);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001F928:
    tmp = lbl_80478878;
    if ((s32)tmp == 2) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x1C);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001F998:
    tmp = lbl_80478878;
    if ((s32)tmp == 2) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x20);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FA08:
    tmp = lbl_80478878;
    if ((s32)tmp == 2) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x10);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FA78:
    tmp = lbl_80478878;
    if ((s32)tmp == 2) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x14);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FAE8:
    tmp = lbl_80478878;
    if ((s32)tmp == 2) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x18);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FB58:
    tmp = lbl_80478878;
    if ((s32)tmp == 3) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x10);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FBC8:
    tmp = lbl_80478878;
    if ((s32)tmp == 3) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x14);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FC38:
    tmp = lbl_80478878;
    if ((s32)tmp == 3) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x1C);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FCA8:
    tmp = lbl_80478878;
    if ((s32)tmp == 3) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x20);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FD18:
    tmp = lbl_8047A34C;
    if (tmp != 0) return;
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;

    return;
}
#endif

/* 0x8001FD48 | 0x5E0 */
extern void fn_801662E8(void);
extern void fn_80165A20(void);
extern void fn_8017B1AC(void);
extern void fn_800EC918(void);
extern void fn_80176A94(void);
extern void fn_8016557C(void);
extern void fn_80176A44(void);
extern void fn_80165548(void);
extern void fn_800EC8DC(void);
extern void fn_800ECCA8(void);
extern void fn_800ECA78(void);
extern void fn_800EC9DC(void);
extern void fn_800EC990(void);
extern void fn_800ECB74(void);
extern void fn_80176E0C(void);
extern void fn_800EC960(void);
extern void fn_801D0748(void);
extern void fn_80128E38(void);
extern void fn_80135168(void);
extern void fn_800216E0(void);
extern void fn_80113828(void);
extern f32 lbl_8047B814;
extern u32 lbl_8047A31C;
extern f32 lbl_8047A348;
extern f32 lbl_8047A340;
extern u32 lbl_8047A324;
extern u32 lbl_8047A33C;
extern u32 lbl_8047A328;
extern u32 lbl_8047A310;
extern u32 lbl_8047A320;
extern u32 lbl_8047A318;
extern u32 lbl_8047A314;
extern f32 lbl_8047B810;
extern f32 lbl_8047A344;
extern f32 lbl_8047B818;
#if 0
asm void fn_8001FD48(void) {
#include "src/game/gs_pcbox_fn_8001FD48.inc"
}
#else
void fn_8001FD48(void) {
    extern u32 lbl_8047A310;
    extern u32 lbl_8047A314;
    extern u32 lbl_8047A318;
    extern u32 lbl_8047A31C;
    extern u32 lbl_8047A320;
    extern u32 lbl_8047A324;
    extern u32 lbl_8047A328;
    extern u32 lbl_8047A33C;
    extern f32 lbl_8047A340;
    extern f32 lbl_8047A344;
    extern f32 lbl_8047A348;
    extern f32 lbl_8047B810;
    extern f32 lbl_8047B814;
    extern f32 lbl_8047B818;
    extern void fn_800216E0();
    extern void fn_800EC8DC();
    extern void fn_800EC918();
    extern void fn_800EC960();
    extern void fn_800EC990();
    extern void fn_800EC9DC();
    extern void fn_800ECA78();
    extern void fn_800ECB74();
    extern void fn_800ECCA8();
    extern void _threadSwitch();
    extern void fn_800F92D4();
    extern void fn_8010264C();
    extern void fn_801026A4();
    extern void fn_801046B8();
    extern void fn_80113828();
    extern void fn_80128E38();
    extern void fn_80135168();
    extern void fn_80165548();
    extern void fn_8016557C();
    extern void fn_80165A20();
    extern void fn_801662E8();
    extern void fn_80176A44();
    extern void fn_80176A94();
    extern void fn_80176E0C();
    extern void fn_8017B1AC();
    extern void fn_801C40F0();
    extern void fn_801C41C8();
    extern void fn_801D0748();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f31 = 0.0f;

    f0 = lbl_8047B814;
    tmp = 0x0;
    r3 = 0xB540000;
    lbl_8047A31C = tmp;
    r3 = r3 + 0x1000;
    r31 = 0x1;
    lbl_8047A348 = f0;
    fn_800F92D4();
    r4 = r3;
    f0 = lbl_8047B814;
    tmp = 0x0;
    r3 = 0x0;
    r30 = r4;
    lbl_8047A340 = f0;
    r4 = 0x406;
    lbl_8047A324 = r30;
    lbl_8047A33C = tmp;
    lbl_8047A328 = tmp;
    fn_801662E8();
    r3 = 0x46a;
    r4 = 0x0;
    r5 = 0xff;
    fn_80165A20();
    tmp = lbl_8047A310;
    if ((s32)tmp == 1) {
        r4 = 0x7;
        r3 = 0x0;
        tmp = 0x1;
        lbl_8047A31C = r4;
        lbl_8047A310 = r3;
        lbl_8047A320 = tmp;
    } else {

        tmp = 0x0;
        lbl_8047A320 = tmp;
    }
    r3 = 0x15;
    r4 = 0x0;
    fn_8010264C();
    do {
    do {
        fn_8017B1AC();
        r29 = r3;

        if ((s32)r29 == 0xb || (s32)r29 == 4) {

            tmp = lbl_8047A318;
            if ((s32)tmp == 0) {
                fn_800EC918();
                fn_80176A94();
                fn_8016557C();
                tmp = 0x1;
                lbl_8047A318 = tmp;
            }
            _threadSwitch();
            break;
        }
        tmp = lbl_8047A318;
        if ((s32)tmp == 1) {
            fn_80176A44();
            r3 = 0x0;
            r4 = 0x406;
            fn_801662E8();
            fn_80165548();
            fn_800EC8DC();
            tmp = 0x0;
            lbl_8047A318 = tmp;
        }
        if ((s32)r29 == 5) {
            tmp = lbl_8047A314;
            if ((s32)tmp == 0) {
                fn_800EC918();
                fn_80176A94();
                fn_8016557C();
                tmp = 0x1;
                lbl_8047A314 = tmp;
            }
            _threadSwitch();
            break;
        }
        tmp = lbl_8047A314;
        if ((s32)tmp == 1) {
            fn_80176A44();
            r3 = 0x0;
            r4 = 0x406;
            fn_801662E8();
            fn_80165548();
            fn_800EC8DC();
            tmp = 0x0;
            lbl_8047A314 = tmp;
        }
        tmp = lbl_8047A31C;
        if ((s32)tmp != 0x1e) {
            if ((s32)tmp < 0x1e) {
                if ((s32)tmp != 4) {
                    if ((s32)tmp < 4) {
                        if ((s32)tmp != 1) {
                            if ((s32)tmp < 1) {
                                if ((s32)tmp < 0) {
                                    break;
                                }
                                if ((s32)tmp < 3) {
                                    goto L_80020018;
                                }
                                if ((s32)tmp == 7) goto L_8002010C;
                                if ((s32)tmp < 7) {
                                    if ((s32)tmp >= 6) goto L_800200D8;
                                    goto L_800200B0;
                                }
                                if ((s32)tmp >= 9) break;
                                goto L_80020184;
                            }
                            if ((s32)tmp == 0xc8) goto L_80020274;
                            if ((s32)tmp < 0xc8) {
                                if ((s32)tmp == 0x28) goto L_800201E8;
                                if ((s32)tmp < 0x28) {
                                    if ((s32)tmp == 0x20) goto L_80020200;
                                    if ((s32)tmp >= 0x20) break;
                                    goto L_80020240;
                                }
                                if ((s32)tmp >= 0x2a) break;
                                goto L_800202E0;
                            }
                            if ((s32)tmp == 0x3e8) goto L_800202F8;
                            if ((s32)tmp >= 0x3e8) break;
                            break;
                                    }
                        if (r30 != 0) {
                            r3 = r30;
                            r4 = 0x0;
                            fn_800ECCA8();
                            f1 = lbl_8047B814;
                            r3 = r30;
                            fn_800ECA78();
                            f1 = lbl_8047B810;
                            r3 = r30;
                            fn_800EC9DC();
                            r3 = r30;
                            fn_800EC990();
                        }
                        r3 = r30;
                        r4 = 0x0;
                        fn_800ECB74();
                        r4 = 0xB550000;
                        r3 = 0x12;
                        r4 = r4 + 0x1800;
                        r5 = 0x0;
                        r6 = 0x0;
                        fn_80176E0C();
                        f1 = lbl_8047B810;
                        r3 = 0x4;
                        fn_801C41C8();
                        tmp = lbl_8047A31C;
                        if ((s32)tmp != 0) break;
                        tmp = 0x1;
                        lbl_8047A31C = tmp;
                        break;
                            }
                    r3 = r30;
                    fn_800EC960();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        tmp = 0x2;
                        lbl_8047A31C = tmp;
                        break;
                    }
                    _threadSwitch();
                    break;
                L_80020018:
                    if (r30 != 0) {
                        r3 = r30;
                        r4 = 0x1;
                        fn_800ECCA8();
                        f1 = lbl_8047B814;
                        r3 = r30;
                        fn_800ECA78();
                        f1 = lbl_8047B810;
                        r3 = r30;
                        fn_800EC9DC();
                        r3 = r30;
                        fn_800EC990();
                    }
                    r3 = r30;
                    r4 = 0x0;
                    fn_800ECB74();
                    tmp = 0x3;
                    lbl_8047A31C = tmp;
                    break;
                                }
                if (r30 == 0) {

                } else {
                    goto L_80020078;
                do {
                        _threadSwitch();
                    L_80020078:
                        r3 = r30;
                        fn_800EC960();
                        tmp = r3 & 0xFF;
                } while (tmp != 0);
                }
                tmp = 0x8;
                lbl_8047A31C = tmp;
                break;
                }
            f1 = lbl_8047B810;
            r3 = 0x5;
            fn_801C41C8();
            tmp = 0x6;
            lbl_8047A31C = tmp;
            break;
        L_800200B0:
            r3 = 0x0;
            fn_801C40F0();
            tmp = (s8)r3;
            if ((s32)tmp == 0) {
                tmp = 0x6;
                lbl_8047A31C = tmp;
                break;
            }
            _threadSwitch();
            break;
        L_800200D8:
            f31 = lbl_8047B810;
            while (1) {
                f0 = lbl_8047A348;
                if (f0 >= f31) break;
                f1 = lbl_8047A348;
                f0 = lbl_8047A344;
                f0 = f1 + f0;
                lbl_8047A348 = f0;
                _threadSwitch();

            }
            tmp = 0x7;
            lbl_8047A31C = tmp;
            break;
        L_8002010C:
            r4 = 0xB560000;
            r3 = 0x12;
            r4 = r4 + 0x1800;
            r5 = 0x0;
            r6 = 0x0;
            fn_80176E0C();
            if (r30 != 0) {
                r3 = r30;
                r4 = 0x2;
                fn_800ECCA8();
                f1 = lbl_8047B814;
                r3 = r30;
                fn_800ECA78();
                f1 = lbl_8047B810;
                r3 = r30;
                fn_800EC9DC();
                r3 = r30;
                fn_800EC990();
            }
            r3 = r30;
            r4 = 0x1;
            fn_800ECB74();
            f1 = lbl_8047B810;
            r3 = 0x4;
            fn_801C41C8();
            r3 = 0x1;
            fn_801C40F0();
            tmp = 0x1e;
            lbl_8047A31C = tmp;
            break;
        L_80020184:
            r4 = 0xB560000;
            r3 = 0x12;
            r4 = r4 + 0x1800;
            r5 = 0x0;
            r6 = 0x0;
            fn_80176E0C();
            if (r30 != 0) {
                r3 = r30;
                r4 = 0x2;
                fn_800ECCA8();
                f1 = lbl_8047B814;
                r3 = r30;
                fn_800ECA78();
                f1 = lbl_8047B810;
                r3 = r30;
                fn_800EC9DC();
                r3 = r30;
                fn_800EC990();
            }
            r3 = r30;
            r4 = 0x1;
            fn_800ECB74();
            tmp = 0x1e;
            lbl_8047A31C = tmp;
            break;
        L_800201E8:
            tmp = 0x3e8;
            lbl_8047A31C = tmp;
            break;
        }
        tmp = 0x20;
        lbl_8047A31C = tmp;
        break;
    L_80020200:
        fn_801046B8();
        r4 = r3;
        r3 = 0x13;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x1;
        r8 = 0x0;
        fn_801026A4();
        if ((s32)r3 < 0) break;
        f0 = lbl_8047B814;
        tmp = 0x1f;
        lbl_8047A31C = tmp;
        lbl_8047A348 = f0;
        break;
    L_80020240:
        f31 = lbl_8047B818;
        while (1) {
            f0 = lbl_8047A348;
            if (f0 >= f31) break;
            f1 = lbl_8047A348;
            f0 = lbl_8047A344;
            f0 = f1 + f0;
            lbl_8047A348 = f0;
            _threadSwitch();

        }
        tmp = 0xc8;
        lbl_8047A31C = tmp;
        break;
    L_80020274:
        r3 = 0x1;
        r4 = 0x2;
        r5 = 0x0;
        fn_801D0748();
        if ((s32)r3 == (s32)-0x1) {
            tmp = 0x20;
            lbl_8047A31C = tmp;
            break;
        }
        if ((s32)r3 != 3) {
            r3 = 0x0;
            r4 = 0x0;
            fn_80128E38();
        }
        r3 = 0x0;
        r4 = 0x4;
        fn_80135168();
        if (r3 != 0) {
            r3 = 0x1;
            fn_800216E0();
        } else {

            r3 = 0x0;
            fn_800216E0();
        }
        tmp = 0x29;
        lbl_8047A31C = tmp;
        break;
    L_800202E0:
        r3 = 0x3a1;
        r4 = 0x0;
        fn_80113828();
        tmp = 0x3e8;
        lbl_8047A31C = tmp;
        break;
    L_800202F8:
        r31 = 0x0;
    } while (0);
        ;
    } while ((s32)r31 != 0);
    return;
}
#endif

/* ===== Phase 2 recovery stubs ===== */

/* fn_80018594 - 0x80018594 | size: 0x34c */
extern void fn_80143DE4(void);
extern void fn_80143DCC(void);
extern void fn_80017CB8(void);
extern void fn_8012959C(void);
extern void menuCloseSync(); /* referenced by asm incs */
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2D8;
extern u8 lbl_80266918[];
#define sSummaryPageEntries lbl_80266918
extern u32 lbl_8047A2DC;
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2B8;
extern s32 fn_80019754(void* arg);
#if 1
asm s32 fn_80018594() {
#include "src/game/gs_pcbox_fn_80018594.inc"
}
#else
s32 fn_80018594() { /* TODO */ }
#endif

/* fn_800188E0 - 0x800188E0 | size: 0x188 */
extern u32 fn_80143FCC();
extern void fn_800181C4();
extern void fn_80017E8C();
extern u32 lbl_8047A2B8;
#if 0
asm void fn_800188E0(void) {
#include "src/game/gs_pcbox_fn_800188E0.inc"
}
#else
#pragma peephole off
s32 fn_800188E0(s32 mode, u32 ptr, u32 r5, u32 r6, u16* out) {
    u16 tmp;
    s32 ret;

    tmp = 0;
    switch (mode) {
    case 0:
    case 1: {
        ret = fn_80018594(ptr, r5, r6, &tmp);
        if (ret == 2) {
            ret = 2;
        } else if (ret == 3) {
            ret = 3;
        } else if (ret == 4) {
            ret = 1;
        } else if (mode == 0) {
            ret = 0;
        } else if (ret == 0) {
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    }
    case 2: {
        s32 r0;
        {
            extern void fn_801440A0();
            fn_801440A0(r5);
        }
        r0 = fn_80143FCC();
        if ((u8)r0 != 0) {
            fn_80132A38(0x2d, (u16)r5);
            fn_80106D3C(2, 0x4262, 1, 0);
            fn_801069FC(1);
            r0 = 0;
        } else {
            lbl_8047A2B8 = r6;
            r0 = 1;
        }
        if (r0 != 0) {
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    }
    case 3:
        fn_800181C4(ptr, r5, r6);
        ret = 0;
        break;
    case 4:
        fn_80017E8C(ptr, r5, r6);
        ret = 0;
        break;
    default:
        break;
    }
    *out = tmp;
    return ret;
}
#pragma peephole reset
#endif

/* fn_80018A68 - 0x80018A68 | size: 0x4c8 */
extern void fn_80103E68(void);
extern void fn_80103EAC(void);
extern void fn_80143C68(void);
extern void fn_800FF660(void);
extern void fn_8011288C();
extern u32 lbl_80478860;
extern u8 lbl_802E4DB0[];
extern u32 lbl_8047A2E8;
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2D8;
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2E4;
#if 1
asm u16 fn_80018A68(void) {
#include "src/game/gs_pcbox_fn_80018A68.inc"
}
#else
u16 fn_80018A68(void) { /* TODO */ }
#endif

/* fn_80018F30 - 0x80018F30 | size: 0x24 */
extern u32 lbl_8047A2F0;
#if 0
asm void fn_80018F30(void) {
#include "src/game/gs_pcbox_fn_80018F30.inc"
}
#else
void fn_80018F30(void) {
    *(u16*)&lbl_8047A2F0 = fn_80018A68();
}
#endif

/* fn_80018F54 - 0x80018F54 | size: 0x34 */
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2E4;
extern u32 lbl_8047A2BC;
#if 0
asm void fn_80018F54(void) {
#include "src/game/gs_pcbox_fn_80018F54.inc"
}
#else
void fn_80018F54(u32 a, u32 b, u32 c) {
    lbl_8047A2E0 = a;
    lbl_8047A2F8 = c;
    lbl_8047A2E4 = 0;
    lbl_8047A2BC = b;
    fn_80018A68();
}
#endif

/* fn_80018F88 - 0x80018F88 | size: 0xdc */
extern u32 fn_801FCEAC();
extern void fn_8019075C();
extern void fn_800FF730();
extern u32 lbl_8047A2F4;
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2EC;
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2E4;
extern u32 lbl_8047A2F0;
#if 0
asm void fn_80018F88(void) {
#include "src/game/gs_pcbox_fn_80018F88.inc"
}
#else
#pragma peephole off
u32 fn_80018F88(s32 mode, s32* ptr, u32 val) {
    if (mode == 1) {
        lbl_8047A2F4 = val;
        lbl_8047A2F8 = fn_801FCEAC(val);
        {
            s32 v = ptr[0];
            lbl_8047A2EC = v;
            if (v < 0 || v >= 5) {
                lbl_8047A2EC = 0;
            }
        }
    } else {
        lbl_8047A2F4 = 0;
        lbl_8047A2F8 = val;
        lbl_8047A2EC = (u32)-1;
    }
    lbl_8047A2E0 = mode;
    lbl_8047A2E4 = 1;
    fn_8019075C(1, 0);
    fn_800FF730(0x38f);
    fn_8011288C(0, 0);
    _threadSwitch();
    if (*(u16*)&lbl_8047A2F0 == 0) {
        return 0;
    }
    if (ptr != NULL) {
        ptr[0] = lbl_8047A2EC;
    }
    return *(u16*)&lbl_8047A2F0;
}
#pragma peephole reset
#endif

/* fn_80019064 - 0x80019064 | size: 0xc */
extern u32 lbl_8047A2B8;
#if 0
asm void fn_80019064(void) {
#include "src/game/gs_pcbox_fn_80019064.inc"
}
#else
u32 fn_80019064(void) {
    return (u8)lbl_8047A2B8;
}
#endif

/* fn_80019070 - 0x80019070 | size: 0x68 */
extern u8 lbl_803A1B90[];
#if 0
asm void fn_80019070(void) {
#include "src/game/gs_pcbox_fn_80019070.inc"
}
#else
#pragma optimization_level 4
u32 fn_80019070(u16 species) {
    u8* r4;
    u32 r7;
    u32 r6;
    u8* r5;
    u32 ctr;
    r4 = lbl_803A1B90;
    r7 = (u32)-1;
    r6 = 0;
    ctr = *(u32*)(r4 + 0x40);
    r5 = r4;
    species = (u16)species;
    if ((s32)ctr > 0) {
        do {
            if (species == *(u16*)r5) {
                r7 = *(u32*)(lbl_803A1B90 + r6 * 8 + 4);
                break;
            }
            r5 += 8;
            r6 += 1;
        } while (--ctr);
    }
    *(u32*)(r4 + 0x40) = 0;
    return r7;
}
#endif

/* fn_800190D8 - 0x800190D8 | size: 0x40 */
#if 0
asm void fn_800190D8(void) {
#include "src/game/gs_pcbox_fn_800190D8.inc"
}
#else
#pragma push
#pragma optimization_level 4
void fn_800190D8(u16 species, u32 data) {
    s32 count;
    count = *(u32*)(lbl_803A1B90 + 0x40);
    if (count >= 0x8) return;
    *(u16*)(lbl_803A1B90 + *(s32*)(lbl_803A1B90 + 0x40) * 8) = species;
    *(u32*)(lbl_803A1B90 + *(s32*)(lbl_803A1B90 + 0x40) * 8 + 4) = data;
    *(u32*)(lbl_803A1B90 + 0x40) = *(s32*)(lbl_803A1B90 + 0x40) + 1;
}
#pragma pop
#endif

/* fn_80019118 - 0x80019118 | size: 0xec */
extern void fn_80109220();
extern u8 lbl_80266C10[];
extern u32 lbl_8047B7A4;
extern u32 lbl_8047A300;
extern u32 lbl_8047B7A0;
#if 0
asm void fn_80019118(void) {
#include "src/game/gs_pcbox_fn_80019118.inc"
}
#else
/* Matching trick: use if/else (not ternary) for cmpw+bne branch pattern;
   declare r30 as u8 to get clrlwi r0,r30,24 + cmplwi (not record form) */
#pragma peephole off
#pragma optimization_level 4
s32 fn_80019118(u8* a, u8* b) {
    u32 sp[4];
    u8* r4;
    s32 r7;
    u32 r0;
    u32 r30;
    s32 r4_val;

    r4 = *(u8**)(a + 0x60);
    sp[0] = *(u32*)(lbl_80266C10 + 0x0);
    sp[1] = *(u32*)(lbl_80266C10 + 0x4);
    sp[2] = *(u32*)(lbl_80266C10 + 0x8);
    sp[3] = *(u32*)(lbl_80266C10 + 0xC);
    r7 = (s32)(s8)a[0x95] + (s32)(4 - *(u32*)(r4 + 0xC));
    if (r7 < 0 || r7 >= 4) return 0;
    r0 = sp[r7];
    r4_val = (s32)(s16)*(s16*)(b + 0x6);
    if (r4_val == (s32)r0) {
        r30 = 1;
    } else {
        r30 = 0;
    }
    fn_80109220(b, r30);
    if ((u8)r30 != 0) {
        *(u8*)(b + 0x67) = *(f32*)&lbl_8047B7A0 * (*(f32*)&lbl_8047B7A4 - *(f32*)&lbl_8047A300);
    }
    return 0;
}
#pragma peephole reset
#endif

/* fn_80019204 - 0x80019204 | size: 0xa4 */
extern u8 lbl_80266C00[];
#if 0
asm void fn_80019204(void) {
#include "src/game/gs_pcbox_fn_80019204.inc"
}
#else
#pragma optimization_level 4
s32 fn_80019204(u8* a, u8* b) {
#pragma peephole off
    extern void fn_80109220();
    s32 table[4];
    u8* r5;
    s32 r8;
    s32 r0;
    s32 r6;
    u32 r4;
    r5 = *(u8**)((u8*)a + 0x60);
    table[0] = *(u32*)(lbl_80266C00 + 0x0);
    table[1] = *(u32*)(lbl_80266C00 + 0x4);
    table[2] = *(u32*)(lbl_80266C00 + 0x8);
    table[3] = *(u32*)(lbl_80266C00 + 0xC);
    r8 = (s32)((s8)*(u8*)(a + 0x95)) + (s32)(4 - *(u32*)(r5 + 0xC));
    if (r8 < 0 || r8 >= 4) return 0;
    r0 = table[r8];
    r6 = (u32)(s32)(s16)*(s16*)(b + 0x6);
    if (r6 == r0) {
        r4 = 1;
    } else {
        r4 = 0;
    }
    fn_80109220(b, r4);
    return 0;
}
#endif

/* fn_800192A8 - 0x800192A8 | size: 0x228 */
extern u32 fn_80144088(u32 a);
extern void fn_800FB680(void);
extern u32 fn_80144014(u32 a);
extern u32 fn_800FA444(u32 a);
extern void fn_80142CF4(void);
extern u8 lbl_80266BF0[];
#if 0
asm void fn_800192A8(void) {
#include "src/game/gs_pcbox_fn_800192A8.inc"
}
#else
#pragma optimization_level 4
s32 fn_800192A8(u8* a, u8* b) {
    extern void fn_80129BC8();
    extern void fn_80132A38();
    u32 sp8_tbl;
    u32 sp12_tbl;
    u32 sp16_tbl;
    u32 sp20_tbl;
    u16 sp_a;
    u16 sp_8;
    u8* r31;
    s32 r8;
    u32 r28;
    u32 r28b;
    u16 r3u;
    u32 r6;
    s32 r29;
    u32 r30;
    s32 r0s;
    u32 r3;
    u32 r4;
    r31 = *(u8**)((u8*)a + 0x60);
    sp8_tbl  = *(u32*)(lbl_80266BF0 + 0x0);
    sp12_tbl = *(u32*)(lbl_80266BF0 + 0x4);
    sp16_tbl = *(u32*)(lbl_80266BF0 + 0x8);
    sp20_tbl = *(u32*)(lbl_80266BF0 + 0xC);
    r0s = (s32)(s16)*(s16*)(b + 0x6);
    if      (r0s == (s32)sp8_tbl)  r8 = 0;
    else if (r0s == (s32)sp12_tbl) r8 = 1;
    else if (r0s == (s32)sp16_tbl) r8 = 2;
    else if (r0s == (s32)sp20_tbl) r8 = 3;
    else                            r8 = 4;
    r3 = *(u32*)((u8*)r31 + 0xC);
    r8 = r8 - (s32)(4 - r3);
    if (r8 < 0 || r8 >= (s32)r3) return 0;
    r28 = (u32)r8 * 2 + 4;
    r3u = *(u16*)((u8*)r31 + r28);
    if (r3u == 0) {
        r6 = 0x134;
    } else {
        r6 = (u32)fn_801440A0(r3u);
        r6 = (u32)fn_80144088(r6);
    }
    ((void(*)(u32,u32,u32,u32))fn_800FB680)(0, 0, (u32)(s32)(-1), r6);
    r3u = *(u16*)((u8*)r31 + r28);
    if (r3u == 0) return 0;
    r4 = (u32)fn_801440A0(r3u);
    r4 = (u32)fn_80144014(r4);
    fn_80129BC8(0, r4, &sp_a, 0, 0, 0);
    ((void(*)(u32,u32))fn_80132A38)(0x34, (u32)(u16)sp_a);
    r29  = (s32)(s16)(u16)(((u32)((u32(*)(u32))fn_800FA444)(0xca)) >> 16);
    r29 += (s32)(s16)(u16)(((u32)((u32(*)(u32))fn_800FA444)(0x12e)) >> 16);
    r6 = (u32)(s32)(0xc3 - r29);
    ((void(*)(u32,u32,u32,u32))fn_800FB680)(r6, 0, (u32)(s32)(-1), 0x12e);
    r28b = (u32)*(u16*)((u8*)r31 + r28);
    r4 = (u32)fn_801440A0((u16)r28b);
    r4 = (u32)fn_80144014(r4);
    fn_80129BC8(0, r4, &sp_8, 0, 0, 0);
    r30 = 0;
    r29 = (s32)r4; /* r3 after fn_80129BC8 = field array ptr */
    {
        u32 r31_acc;
        r31_acc = 0;
        while ((s32)r30 < (s32)(u16)sp_8) {
            if (fn_801429E8((void*)r29) != 0) {
                if (((u32(*)(u32,u32,u32,u32))fn_80142CF4)((u32)r29, 0, 0x1b, 0) == r28b) {
                    r31_acc += (u32)fn_80143C50((void*)r29);
                }
            }
            r30++;
            r29 += 4;
        }
        ((void(*)(u32,u32))fn_80132A38)(0x34, r31_acc);
    }
    r29 = (s32)(s16)(u16)(((u32)((u32(*)(u32))fn_800FA444)(0xca)) >> 16);
    r6 = (u32)(s32)(0xc3 - r29);
    ((void(*)(u32,u32,u32,u32))fn_800FB680)(r6, 0, (u32)(s32)(-1), 0xca);
    return 0;
}
#endif

/* fn_800194D0 - 0x800194D0 | size: 0x14 */
#if 0
asm void fn_800194D0(void) {
#include "src/game/gs_pcbox_fn_800194D0.inc"
}
#else
#pragma optimization_level 4
s32 fn_800194D0(u8* a, u8* b) {
    u8* r5;
    s32 ret = 0;
    r5 = *(u8**)(a + 0x60);
    *(u32*)(b + 0x64) = *(u32*)r5;
    return ret;
}
#endif

/* fn_800194E4 - 0x800194E4 | size: 0xfc */
extern u8 lbl_802E4E10[];
#if 0
asm void fn_800194E4(void) {
#include "src/game/gs_pcbox_fn_800194E4.inc"
}
#else
#pragma optimization_level 4
s32 fn_800194E4(u8* a, u8* b) {
    u8* r5;
    s32 r7;
    u32 r6;
    s32 r5_idx;
    u8* r3tbl;
    r5 = *(u8**)((u8*)a + 0x60);
    if ((s16)*(s16*)(b + 0x6) != (s16)0x26c) {
        *(u32*)(b + 0x64) = *(u32*)(r5 + 0x0);
    }
    r7 = (s32)*(u32*)(r5 + 0xC);
    if (r7 >= 4) return 0;
    {
        s16 species;
        u8* tptr;
        species = *(s16*)(b + 0x6);
        r6 = 0x6;
        tptr = lbl_802E4E10;
        if (species == *(s32*)tptr) r6 = 0;
        else if (species == *(s32*)(tptr + 0xc)) r6 = 1;
        else if (species == *(s32*)(tptr + 0x18)) r6 = 2;
        else if (species == *(s32*)(tptr + 0x24)) r6 = 3;
        else if (species == *(s32*)(tptr + 0x30)) r6 = 4;
        else if (species == *(s32*)(tptr + 0x3c)) r6 = 5;
        if (r6 >= 6) return 0;
        r3tbl = lbl_802E4E10 + r6 * 0xC;
        r5_idx = 4 - r7;
        *(s16*)(b + 0x52) = (s16)((s32)(s16)*(s16*)(r3tbl + 0x4) + r5_idx * 0x1f);
        if (*(s32*)(r3tbl + 0x8) != 0) {
            *(s16*)(b + 0x56) = (s16)((s32)(s16)*(s16*)(r3tbl + 0x6) - r5_idx);
        }
    }
    return 0;
}
#endif

/* fn_800195E0 - 0x800195E0 | size: 0xa0 */
#if 0
asm void fn_800195E0(void) {
#include "src/game/gs_pcbox_fn_800195E0.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_800195E0(u8* a) {
    extern u8* fn_80105624();
    u8* r3;
    u8* r30;
    u32 r31;
    s32 r5;
    s32 r0;
    s32 r4;
    r30 = a;
    r31 = *(u32*)(a + 0x60);
    r3 = fn_80105624();
    r5 = *(s32*)((u8*)r31 + 0xC);
    r0 = *(u16*)(r3 + 0x6) & 0x2;
    if (r0 != 0) {
        r4 = (s32)(s8)*(u8*)(r30 + 0x95);
        r0 = r4 + 1;
        if (r0 >= r5) r0 = r5 - 1;
        *(s8*)(r30 + 0x95) = (s8)r0;
    }
    r0 = *(u16*)(r3 + 0x6) & 0x1;
    if (r0 != 0) {
        r4 = (s32)(s8)*(u8*)(r30 + 0x95);
        r0 = r4 - 1;
        if (r0 < 0) r0 = 0;
        *(s8*)(r30 + 0x95) = (s8)r0;
    }
    return 0;
}
#pragma peephole reset
#endif

/* fn_80019680 - 0x80019680 | size: 0xd4 */
extern u32 lbl_8047B7A8;
extern u32 lbl_8047A300;
extern u32 lbl_8047B7AC;
extern u32 lbl_8047B7A4;
#if 0
asm void fn_80019680(void) {
#include "src/game/gs_pcbox_fn_80019680.inc"
}
#else
#pragma peephole off
s32 fn_80019680(u8* arg) {
    s32 val;
    val = (s8)arg[1];
    switch (val) {
    case 0:
        if ((s8)arg[2] == 0) {
            fn_801080CC((void*)0x5f, 0x66);
            *(f32*)&lbl_8047A300 = *(f32*)&lbl_8047B7A8;
            arg[2] = 1;
        }
        break;
    case 2: {
        f32 f1;
        f1 = *(f32*)&lbl_8047A300 + *(f32*)&lbl_8047B7AC;
        *(f32*)&lbl_8047A300 = f1;
        if (f1 > *(f32*)&lbl_8047B7A4) {
            *(f32*)&lbl_8047A300 = *(f32*)&lbl_8047B7A8;
        }
        break;
    }
    case 3:
        if ((s8)arg[2] == 0) {
            fn_801080CC((void*)0x5f, 0x6a);
            arg[2] = 1;
        }
        break;
    }
    return 0;
}
#pragma peephole reset
#endif

/* fn_80019754 - 0x80019754 | size: 0x1e4 */
extern u32 fn_80129B2C();
extern u32 lbl_80478868;
extern u32 lbl_80478BD8;
extern u8  lbl_802E4E10[];
extern u8  lbl_802EF0A8[];
#if 0
asm void fn_80019754(void) {
#include "src/game/gs_pcbox_fn_80019754.inc"
}
#else
s32 fn_80019754(void* arg) {
    extern s32 fn_801046B8(void);
    extern s32 fn_801026A4(s32, s32, s32, s32, s32, s32, void*, ...);
    extern void fn_80102510(s32);
    extern void menuCloseSync(s32, s32);
    u8* e;
    s16* p4;
    s16* p8;
    u16 ids[3];
    s32 count_plus_one;
    s32 count;
    s32 i;
    s32 sel;
    s32 r4;

    if (lbl_80478868 != 0) {
        e = lbl_802E4E10;
        p4 = (s16*)(lbl_802EF0A8 + 0x4);
        p8 = (s16*)(lbl_802EF0A8 + 0x8);
        *(s16*)(e + 0x4)  = p4[*(u32*)(e + 0x0)  * 0xe];
        *(s16*)(e + 0x6)  = p8[*(u32*)(e + 0x0)  * 0xe];
        *(s16*)(e + 0x10) = p4[*(u32*)(e + 0xc)  * 0xe];
        *(s16*)(e + 0x12) = p8[*(u32*)(e + 0xc)  * 0xe];
        *(s16*)(e + 0x1c) = p4[*(u32*)(e + 0x18) * 0xe];
        *(s16*)(e + 0x1e) = p8[*(u32*)(e + 0x18) * 0xe];
        *(s16*)(e + 0x28) = p4[*(u32*)(e + 0x24) * 0xe];
        *(s16*)(e + 0x2a) = p8[*(u32*)(e + 0x24) * 0xe];
        *(s16*)(e + 0x34) = p4[*(u32*)(e + 0x30) * 0xe];
        *(s16*)(e + 0x36) = p8[*(u32*)(e + 0x30) * 0xe];
        *(s16*)(e + 0x40) = p4[*(u32*)(e + 0x3c) * 0xe];
        *(s16*)(e + 0x42) = p8[*(u32*)(e + 0x3c) * 0xe];
        lbl_80478868 = 0;
    }

    count = 0;
    for (i = 0; (u32)(u16)i < lbl_80478BD8 && count < 3; i++) {
        fn_801440A0(i);
        if ((u8)fn_80144014(0) == 6) {
            if ((u8)fn_80129B2C(0, i)) {
                ids[count] = (u16)i;
                count++;
            }
        }
    }
    ids[count] = 0;
    count_plus_one = count + 1;

    r4 = fn_801046B8();
    sel = fn_801026A4(0x5f, r4, 0, 0, 1, 1, &arg);
    fn_80102510(0x5f);
    menuCloseSync(0x5f, 1);
    if (sel >= 0 && sel < count_plus_one) {
        return ids[sel];
    }
    return 0;
}
#endif

/* fn_80019938 - 0x80019938 | size: 0xbc */
extern u8* fn_80103FE4();
#if 0
asm void fn_80019938(void) {
#include "src/game/gs_pcbox_fn_80019938.inc"
}
#else
#pragma optimization_level 4
void fn_80019938(u8* a, u8* b) {
    extern void fn_80132A38(s32, u32);
    extern void fn_800FB680(s32, s32, s32, u32);
    u8* base;
    s16 r0;
    u32 r4;
    u32 r4v;
    base = fn_80103FE4();
    r0 = *(s16*)(b + 0x6);
    r4 = 0x0;
    if (r0 == (s16)0xe93) r4 = 0x0;
    else if (r0 == (s16)0xe94) r4 = 0x1;
    else if (r0 == (s16)0xe95) r4 = 0x2;
    else if (r0 == (s16)0xe96) r4 = 0x3;
    base = base + r4 * 0xc;
    r4v = *(u32*)(base + 0x4);
    if (r4v == 0) return;
    fn_80132A38(0x37, r4v);
    fn_800FB680(0x0, 0x0, (s32)*(u8*)(a + 0x8b) | (s32)(-0x100), 0xe7);
}
#endif

/* fn_800199F4 - 0x800199F4 | size: 0x128 */
extern void* fn_80103FFC();
extern void fn_8005D8F8();
extern void* memcpy(void* dst, const void* src, u32 n);
extern u32 lbl_80478870;
#if 0
asm void fn_800199F4(void) {
#include "src/game/gs_pcbox_fn_800199F4.inc"
}
#else
#pragma peephole off
s32 fn_800199F4(u8* arg) {
    u8* entry;
    u16* ids;
    s32 i;
    void* dst;

    if ((s8)arg[1] == 0) {
        dst = fn_80103FFC(arg, 0x48);
        if (dst != NULL) {
            memcpy(dst, *(void**)(arg + 0x60), 0x48);
        }
    }
    {
        u8* tmp;
        tmp = fn_80103FE4(arg);
        i = 0;
        ids = (u16*)&lbl_80478870;
        entry = tmp;
    }
    while (i < 4) {
        if (*(u32*)(entry + 4) != 0) {
            fn_8005D8F8(*ids, 1);
        } else {
            fn_8005D8F8(*ids, 0);
        }
        entry += 0xc;
        ids++;
        i++;
    }
    {
        s32 val;
        val = fn_801022B8(*(u32*)(arg + 4));
        switch (val) {
        case 0xe93:
            *(u32*)(arg + 0x80) = 0;
            break;
        case 0xe94:
            *(u32*)(arg + 0x80) = 1;
            break;
        case 0xe95:
            *(u32*)(arg + 0x80) = 2;
            break;
        case 0xe96:
            *(u32*)(arg + 0x80) = 3;
            break;
        default:
            *(s32*)(arg + 0x80) = -1;
            break;
        }
    }
    return 0;
}
#pragma peephole reset
#endif

/* fn_80019B1C - 0x80019B1C | size: 0x2c */
#if 0
asm void fn_80019B1C(void) {
#include "src/game/gs_pcbox_fn_80019B1C.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling off
void fn_80019B1C(void) {
    extern void fn_80102568();
    fn_80102568(0x42, 0x0, 0x1);
}
#pragma pop
#endif

/* fn_80019B48 - 0x80019B48 | size: 0x214 */
extern void fn_801204A8(void);
extern u8 lbl_803A1BD8[];
#if 0
asm void fn_80019B48(void) {
#include "src/game/gs_pcbox_fn_80019B48.inc"
}
#else
#pragma optimization_level 4
s32 fn_80019B48(s32 a) {
    extern u8 lbl_803A1D40[];
    extern void fn_801026A4();
    extern void fn_80102568();
    u32 r5;
    u32 r4;
    u32 r3;
    u32 r6;
    s32 r30;
    s32 r31;
    r31 = 0;
    r5 = *(u32*)(lbl_803A1D40 + 0x8);
    r4 = *(u32*)(lbl_803A1D40 + 0xC);
    if ((s32)r5 == 0x0) {
        r30 = (s8)a;
        if ((u32)(s16)r30 >= 0x6) goto L_80019CE0;
        if (r5 == 0x0) {
            r3 = 0x8ae;
            fn_801906A0();
            if (r3 == 0) {
                r3 = 0x0; r4 = 0x2;
                fn_80129280();
                r3 = 0;
            } else {
                fn_8006AEEC();
                r3 = 0;
            }
        } else if (r5 == 0x1) {
            if (r4 == 0) {
                r3 = 0x0;
                fn_801F2A7C();
                r4 = r3;
            }
            if (r4 == 0) {
                r3 = 0;
            } else {
                r3 = r4;
                r4 = 0x0; r5 = 0x44; r6 = 0x0;
                fn_801FB1C0();
                r3 = 0;
            }
        } else {
            r3 = 0;
        }
        if (r3 == 0) { r31 = 0; goto L_80019CE0; }
        r4 = (u32)r30;
        fn_8012AC08();
        r31 = r3;
        goto L_80019CC8;
    } else if ((s32)r5 == 0x1) {
        r30 = (s8)a;
        if ((u32)(s16)r30 >= 0x6) goto L_80019CE0;
        if (r4 == 0) { r3 = 0; fn_801F2A7C(); r4 = r3; }
        if (r4 == 0) { r31 = 0; goto L_80019CE0; }
        r3 = r4; r4 = (u32)r30;
        fn_801F986C();
        if (r3 == 0) { r31 = 0; goto L_80019CE0; }
        r4 = 0x0; r5 = 0xcc; r6 = 0x0;
        fn_8012640C();
        r31 = r3;
        goto L_80019CC8;
    } else if ((s32)r5 == 0x2) {
        if ((u32)(s16)(s8)a < 0x1e) goto L_80019CC8;
        goto L_80019CE0;
    } else {
        goto L_80019CE0;
    }
    L_80019CC8:
    r3 = r31;
    fn_80123FBC();
    if ((r3 & 0xFF) != 0) goto L_80019CE0;
    r31 = 0;
    L_80019CE0:
    if (r31 == 0) return -1;
    ((void(*)(u32,u8*))fn_801204A8)(r31, lbl_803A1BD8);
    fn_801026A4(0x42, 0x0, 0x0, 0x0, 0x1, 0x1, lbl_803A1BD8);
    r30 = r3;
    fn_80102568(0x42, 0x0, 0x1);
    return r30;
}
#endif

/* fn_80019D5C - 0x80019D5C | size: 0x210 */
#if 0
asm void fn_80019D5C(void) {
#include "src/game/gs_pcbox_fn_80019D5C.inc"
}
#else
#pragma optimization_level 4
u32 fn_80019D5C(u32 a, u32 b) {
    extern u8 lbl_803A1D40[];
    extern void fn_801906A0();
    extern void fn_80129280();
    extern void fn_8006AEEC();
    extern void fn_801F2A7C();
    extern void fn_801FB1C0();
    extern void fn_8012AC08();
    extern void fn_801F986C();
    extern void fn_8012640C();
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u8* r29;
    s32 r28;
    s32 r27;
    s32 r31;
    u32 r30;
    r29 = lbl_803A1D40;
    r30 = a;
    r31 = (s32)(s8)*(u8*)(r29 + 0x6);
    if ((s32)b == 0x1) {
        r31 = r31 - 1;
    } else if ((s32)b == 0x2) {
        r31 = r31 + 1;
    } else {
        return a;
    }
    if ((s8)r31 >= 6) r31 = 5;
    if ((s8)r31 < 0) r31 = 0;
    r28 = (s8)r31;
    if (r28 == (s32)(s8)*(u8*)(r29 + 0x6)) return a;
    r27 = 0;
    r4 = *(u32*)(r29 + 0x8);
    r3 = *(u32*)(r29 + 0xC);
    if ((s32)r4 == 0x0) {
        if ((u16)(s16)r28 >= 0x6) goto L_80019F44;
        if ((s32)r4 == 0x1) goto L_80019E64;
        if ((s32)r4 >= 0x2) goto L_80019E98;
        /* r4==0 */
        r3 = 0x8ae;
        fn_801906A0();
        if (r3 == 0) {
            r3 = 0x0; r4 = 0x2;
            fn_80129280();
        } else {
            fn_8006AEEC();
        }
        r3 = 0;
        goto L_80019E9C;
        L_80019E64:
        if (r3 != 0) goto L_80019E74;
        r3 = 0x0;
        fn_801F2A7C();
        L_80019E74:
        if (r3 == 0) { r3 = 0; goto L_80019E9C; }
        r4 = 0x0; r5 = 0x44; r6 = 0x0;
        fn_801FB1C0();
        goto L_80019E9C;
        L_80019E98:
        r3 = 0;
        L_80019E9C:
        if (r3 == 0) { r27 = 0; goto L_80019F44; }
        r4 = (u32)r28;
        fn_8012AC08();
        r27 = r3;
        goto L_80019F2C;
    } else if ((s32)r4 == 0x1) {
        if ((u16)(s16)r28 >= 0x6) goto L_80019F44;
        if (r3 == 0) { r3 = 0; fn_801F2A7C(); }
        if (r3 == 0) { r27 = 0; goto L_80019F44; }
        r4 = (u32)r28;
        fn_801F986C();
        if (r3 == 0) { r27 = 0; goto L_80019F44; }
        r4 = 0x0; r5 = 0xcc; r6 = 0x0;
        fn_8012640C();
        r27 = r3;
        goto L_80019F2C;
    } else if ((s32)r4 >= 0x3) {
        goto L_80019F2C;
    } else { /* r4==2 */
        if ((u16)(s16)r28 < 0x1e) goto L_80019F2C;
        goto L_80019F44;
    }
    L_80019F2C:
    r3 = r27;
    fn_80123FBC();
    if ((r3 & 0xFF) != 0) goto L_80019F44;
    r27 = 0;
    L_80019F44:
    if (r27 != 0) {
        r30 = (u32)r27;
        *(u8*)(r29 + 0x6) = (u8)r31;
    }
    return r30;
}
#endif

/* fn_80019F6C - 0x80019F6C | size: 0xa18 */
extern s32 fn_801040D0(s32, s32);
extern void fn_80143F24(void);
extern void fn_8011FC74(void);
extern void fn_8011E2AC(void);
extern void fn_800FBB34(void);
extern void fn_8010B9E8(void);
extern void fn_80107E78(void);
extern void fn_801081F8(void);
extern u32 lbl_8047A308;
extern u32 lbl_8047B7B8;
extern u32 lbl_8047B7B0;
extern void menuSubCalcColor();
#if 1
asm void fn_80019F6C(void) {
#include "src/game/gs_pcbox_fn_80019F6C.inc"
}
#else
void fn_80019F6C(void) { /* TODO */ }
#endif

/* fn_8001A984 - 0x8001A984 | size: 0x114 */
#if 0
asm void fn_8001A984(void) {
#include "src/game/gs_pcbox_fn_8001A984.inc"
}
#else
#pragma optimization_level 4
void fn_8001A984(u8* a) {
    extern u8 lbl_803A1D40[];
    extern u8 lbl_803A1C20[];
    extern u32 fn_801440A0();
    extern void fn_80132A38();
    extern void fn_800FB680();
    u32 r6;
    u32 r0;
    u8* r3;
    r6 = 0;
    r0 = *(u8*)(lbl_803A1D40 + 0x14);
    if ((s32)r0 == 0x0) {
        r0 = *(u8*)(lbl_803A1D40 + 0x0);
        if ((s32)r0 == 0x3 || (s32)r0 == 0x4) {
            r3 = (u8*)fn_801440A0(*(u16*)(lbl_803A1D40 + 0x12));
            r0 = fn_80144014((u32)r3);
            r6 = ((r0 & 0xFF) == 0x4) ? 0x2b63 : 0x2b61;
        } else if ((s32)r0 == 0x5) {
            r6 = 0x2b62;
        } else {
            r6 = 0x2b5d;
        }
    } else if ((s32)r0 == 0x1) {
        r3 = lbl_803A1C20 + (s32)((s8)*(u8*)(lbl_803A1D40 + 0x6)) * 0x30;
        fn_80132A38(0x32, r3);
        r6 = 0x2b5e;
    } else if ((s32)r0 == 0x2) {
        r6 = 0x2b5f;
    } else if ((s32)r0 == 0x3) {
        r6 = 0x2b60;
    }
    if (r6 != 0) {
        fn_800FB680(0x0, 0x0, (s32)*(u8*)(a + 0x8b) | (s32)(-0x100), r6);
    }
}
#endif

/* fn_8001AA98 - 0x8001AA98 | size: 0xd8 */
extern void fn_80166A28(void);
extern void fn_80102ED4(void);
#if 0
asm void fn_8001AA98(void) {
#include "src/game/gs_pcbox_fn_8001AA98.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void fn_8001AA98(u8* a) {
    extern u8 lbl_803A1D40[];
    extern u32 fn_80105624();
    extern u32 fn_801022B8();
    extern void fn_80166A28(u32);
    extern void fn_80102ED4(u8*);
    s32 r3;
    u32 r31;
    u32 r3b;
    r31 = 0;
    if (*(u8*)(lbl_803A1D40 + 0x1) == 0) {
        *(u8*)(a + 0x98) = 0x1;
        return;
    }
    r3 = fn_80105624();
    r3 = *(u16*)((u8*)r3 + 0x4);
    if ((r3 & 0x10) != 0) {
        r3b = fn_801022B8(*(u32*)(a + 0x4));
        if ((r3b & 0xFFFF) == 0x3b6) r31 = 1;
    } else if ((r3 & 0x20) != 0) {
        r31 = 1;
    }
    if ((u8)r31 != 0) {
        if (*(u8*)(lbl_803A1D40 + 0x15) == 0) {
            fn_80166A28(0x26);
            return;
        }
        *(u8*)(a + 0x98) = 0x1;
        *(u8*)(a + 0x99) = 0x1;
        return;
    }
    fn_80102ED4(a);
}
#pragma pop
#endif

/* fn_8001AB70 - 0x8001AB70 | size: 0x3d4 */
extern void fn_8005D9E4(void);
#if 1
asm void fn_8001AB70(void) {
#include "src/game/gs_pcbox_fn_8001AB70.inc"
}
#else
void fn_8001AB70(void) { /* TODO */ }
#endif

/* fn_8001AF44 - 0x8001AF44 | size: 0x240 */
extern void fn_8005D880(s16 x, s16 y, s16 z);
extern u32 lbl_8047A308;
#if 0
asm void fn_8001AF44(void) {
#include "src/game/gs_pcbox_fn_8001AF44.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_8001AF44(s32 ctx) {
    extern u8 lbl_803A1D40[];
    extern u8 lbl_802E4E58[];
    extern s32 fn_801026A4(s32, s32, s32, s32, s32, s32, void*, ...);
    u32 result;
    s32 byte_off;
    u8* iter;
    s32 i;

    result = fn_801040D0(ctx, 0);
    if (result == 0) return 0;

    if ((s8)*((u8*)ctx + 1) == 0) {
        *((s8*)ctx + 0x97) = -1;
        if ((s8)*((u8*)ctx + 2) == 0) {
            i = 0;
            byte_off = 0;
            iter = (u8*)result;
            for (; i < 6; i++) {
                s16 sy, sx;
                s16 npcId;
                u8* slot;
                s32 state;

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30 + byte_off;
                fn_8005D880(*(s16*)(slot + 2), *(s16*)(slot + 4), *(s16*)(slot + 6));
                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30 + byte_off;
                fn_8005D9AC(*(s16*)(slot + 0), *(s16*)(slot + 4), *(s16*)(slot + 6));

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30;
                fn_801026A4((s32)*(s16*)(slot + byte_off), 0x63, 0, 0, 0, 2, (void*)iter, i);

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30;
                npcId = *(s16*)(slot + byte_off);
                fn_8005D95C(npcId, (u16*)&sx, (u16*)&sy);
                if (sx > 0xfa) state = 0x116;
                else state = 0x11e;
                fn_801080CC((void*)(s32)npcId, state);

                byte_off += 8;
                iter += 0x30;
            }
            fn_801080CC(*(void**)((u8*)ctx + 4), 1);
            *((s8*)ctx + 2) = 1;
        }
    } else if ((s8)*((u8*)ctx + 1) == 3) {
        if ((s8)*((u8*)ctx + 2) == 0) {
            byte_off = 0;
            for (i = byte_off; byte_off < 6; byte_off++) {
                s16 sy, sx;
                s16 npcId;
                u8* slot;
                s32 state;

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30;
                npcId = *(s16*)(slot + i);
                fn_8005D95C(npcId, (u16*)&sx, (u16*)&sy);
                if (sx > 0xfa) state = 0x11a;
                else state = 0x122;
                fn_801080CC((void*)(s32)npcId, state);

                i += 8;
            }
            fn_801080CC(*(void**)((u8*)ctx + 4), 7);
            *((s8*)ctx + 2) = 1;
        }
    }
    *(s16*)&lbl_8047A308 = (s16)(((s32)*(s16*)&lbl_8047A308 + 1) % 1000);
    return 0;
}
#pragma pop
#endif

/* fn_8001B184 - 0x8001B184 | size: 0x68 */
#if 0
asm void fn_8001B184(void) {
#include "src/game/gs_pcbox_fn_8001B184.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling off
void fn_8001B184(void) {
    extern u8 lbl_803A1D40[];
    extern void fn_80102568();
    extern void fn_800FF660();
    extern void fn_8011288C();
    extern void _threadSwitch();
    fn_80102568(0x63, 0x0, 0x1);
    if (*(u8*)(lbl_803A1D40 + 0x2) != 0) {
        fn_800FF660();
        if (*(u8*)(lbl_803A1D40 + 0x3) != 0x1) fn_8011288C(0x0, 0x0);
        _threadSwitch();
    }
}
#pragma pop
#endif

/* fn_8001B1EC - 0x8001B1EC | size: 0x8d8 */
extern void fn_8010B01C(void);
extern void fn_80010C98(void);
extern void fn_80097E58(void);
extern void fn_8011FC14(void);
extern void fn_8010B560(void);
extern u8 lbl_8047A30A[];
extern f32 lbl_8047B7C0;
extern u32 lbl_8047B7C4;
#if 1
asm void fn_8001B1EC(void) {
#include "src/game/gs_pcbox_fn_8001B1EC.inc"
}
#else
void fn_8001B1EC(void) { /* TODO */ }
#endif

/* fn_8001BAC4 - 0x8001BAC4 | size: 0x228 */
extern void fn_8010206C();
extern void fn_80102038();
extern u32 lbl_8047B7B8;
extern u32 lbl_8047B7C4;
#if 0
asm void fn_8001BAC4(void) {
#include "src/game/gs_pcbox_fn_8001BAC4.inc"
}
#else
#pragma push
#pragma peephole off
u32 fn_8001BAC4(u32 a0, u8 a1, u8 a2, u16 a3, u32 a4, u8 a5) {
    extern u8 lbl_803A1D40[];
    extern u32 fn_800D37CC(void);
    extern void fn_8019075C();
    extern void fn_800FF730();
    extern void fn_8011288C();
    extern void _threadSwitch();
    extern void fn_8001B1EC();
    s32 v;

    *(u8*)(lbl_803A1D40 + 0x0) = a0;
    *(u8*)(lbl_803A1D40 + 0x10) = a1;
    *(u8*)(lbl_803A1D40 + 0x11) = a2;
    *(u16*)(lbl_803A1D40 + 0x12) = a3;
    *(u32*)(lbl_803A1D40 + 0xC) = a4;
    *(u8*)(lbl_803A1D40 + 0x1) = a5;
    *(u32*)(lbl_803A1D40 + 0x1C) = 0;
    *(u8*)(lbl_803A1D40 + 0x15) = 1;

    switch ((s32)(u8)a0) {
    case 2:
    case 4:
        *(u32*)(lbl_803A1D40 + 0x8) = 1;
        break;
    case 3:
    default:
        *(u32*)(lbl_803A1D40 + 0x8) = 0;
        break;
    }

    v = (s32)*(u8*)(lbl_803A1D40 + 0x0);
    switch (v) {
    case 2:
        if ((u16)a3 != 0) {
            *(u8*)(lbl_803A1D40 + 0x2) = 1;
            *(u8*)(lbl_803A1D40 + 0x3) = 1;
            goto BBDC;
        }
        *(u8*)(lbl_803A1D40 + 0x2) = 1;
        *(u8*)(lbl_803A1D40 + 0x3) = 1;
        *(u8*)(lbl_803A1D40 + 0x15) = 0;
        goto BBDC;
    case 1:
        *(u8*)(lbl_803A1D40 + 0x2) = 1;
        *(u8*)(lbl_803A1D40 + 0x3) = 0;
        goto BC20;
    case 3:
    case 4:
    case 5:
        *(u8*)(lbl_803A1D40 + 0x2) = 0;
        *(u8*)(lbl_803A1D40 + 0x3) = 0;
        goto BC20;
    default:
        break;
    }
BBDC:
    fn_8010206C(*(f32*)&lbl_8047B7C4 / (f32)(u32)fn_800D37CC());
    *(u8*)(lbl_803A1D40 + 0x2) = 1;
    *(u8*)(lbl_803A1D40 + 0x3) = 2;
BC20:
    if (*(u8*)(lbl_803A1D40 + 0x2) == 1) {
        fn_8019075C(1, 1);
        fn_800FF730(0x38f);
        if (*(u8*)(lbl_803A1D40 + 0x3) != 1) {
            fn_8011288C(0, 0);
        }
        _threadSwitch();
        if (*(u8*)(lbl_803A1D40 + 0x3) == 2) {
            fn_80102038(*(f32*)&lbl_8047B7C4 / (f32)(u32)fn_800D37CC());
        }
    } else {
        fn_8001B1EC(a0, (u16)a3, a4);
    }
    *(u8*)(lbl_803A1D40 + 0x4) = 1;
    return *(u32*)(lbl_803A1D40 + 0x1C);
}
#pragma pop
#endif

/* fn_8001BCEC - 0x8001BCEC | size: 0x50 */
#if 0
asm void fn_8001BCEC(void) {
#include "src/game/gs_pcbox_fn_8001BCEC.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void fn_8001BCEC(u32 a, u32 b, u32 c, u32 d) {
    extern u8 lbl_803A1D40[];
    extern void fn_8001BAC4(u32, u32, u32, u32, u32, u32);
    *(u8*)(lbl_803A1D40 + 0x4) = 0x1;
    fn_8001BAC4(0x5, a, b, c, d, 0x1);
}
#pragma pop
#endif

/* fn_8001BD3C - 0x8001BD3C | size: 0x44 */
#if 0
asm void fn_8001BD3C(void) {
#include "src/game/gs_pcbox_fn_8001BD3C.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void fn_8001BD3C(u32 a, u32 b, u32 c, u32 d) {
    extern u8 lbl_803A1D40[];
    extern void fn_8001BAC4();
    *(u8*)(lbl_803A1D40 + 0x4) = 0x1;
    fn_8001BAC4(a, 0x0, 0x0, c, d, b);
}
#pragma pop
#endif

/* fn_8001BD80 - 0x8001BD80 | size: 0x74 */
#if 0
asm void fn_8001BD80(void) {
#include "src/game/gs_pcbox_fn_8001BD80.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void fn_8001BD80(u8 a, u8 b, u32 c, u32 d) {
    extern u8 lbl_803A1D40[];
    extern void fn_8001BAC4(u32, u32, u32, u32, u32, u32);
    if (a == 0x1) {
        *(u8*)(lbl_803A1D40 + 0x4) = 0x0;
    } else {
        *(u8*)(lbl_803A1D40 + 0x4) = 0x1;
    }
    *(u32*)(lbl_803A1D40 + 0x18) = d;
    fn_8001BAC4(0x2, 0x0, 0x0, b, c, 0x1);
}
#pragma pop
#endif

/* fn_8001BDF4 - 0x8001BDF4 | size: 0x44 */
#if 0
asm void fn_8001BDF4(void) {
#include "src/game/gs_pcbox_fn_8001BDF4.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void fn_8001BDF4(u32 a, u32 b, u32 c) {
    extern u8 lbl_803A1D40[];
    extern void fn_8001BAC4(u32, u32, u32, u32, u32, u32);
    *(u8*)(lbl_803A1D40 + 0x4) = 0x1;
    fn_8001BAC4(a, 0x0, 0x0, b, c, 0x1);
}
#pragma pop
#endif

/* fn_8001BE38 - 0x8001BE38 | size: 0x84 */
#if 0
asm void fn_8001BE38(void) {
#include "src/game/gs_pcbox_fn_8001BE38.inc"
}
#else
#pragma push
#pragma scheduling off
#pragma optimization_level 4
s32 fn_8001BE38(void) {
    extern u8 lbl_803A1D40[];
    extern void fn_8001B1EC();
    extern void fn_80102568();
    extern void fn_800FF660();
    extern void fn_8011288C();
    extern void _threadSwitch();
    u8 r3;
    u16 r4;
    u32 r5;
    r3 = *(u8*)(lbl_803A1D40 + 0x0);
    r4 = *(u16*)(lbl_803A1D40 + 0x12);
    r5 = *(u32*)(lbl_803A1D40 + 0xC);
    fn_8001B1EC((u32)r3, (u32)r4, r5);
    fn_80102568(0x63, 0x0, 0x1);
    if (*(u8*)(lbl_803A1D40 + 0x2) != 0) {
        fn_800FF660();
        if (*(u8*)(lbl_803A1D40 + 0x3) != 0x1) fn_8011288C(0x0, 0x0);
        _threadSwitch();
    }
    return 0;
}
#pragma pop
#endif

/* fn_8001BEBC - 0x8001BEBC | size: 0x1a8 */
#if 0
asm void fn_8001BEBC(void) {
#include "src/game/gs_pcbox_fn_8001BEBC.inc"
}
#else
#pragma optimization_level 4
void fn_8001BEBC(void* a) {
    extern u8 lbl_803A1D40[];
    extern void fn_801906A0();
    extern void fn_80129280();
    extern void fn_8006AEEC();
    extern void fn_801F2A7C();
    extern void fn_801FB1C0();
    extern void fn_8012AC08();
    extern void fn_801F986C();
    extern void fn_8012640C();
    extern void fn_80123FBC();
    u32 tmp;
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r29;
    u32 r30;
    u32 r31;
    r31 = (u32)a;
    r29 = *(u16*)((u8*)r31 + 0x0);
    r30 = 0x0;
    tmp = *(u32*)(lbl_803A1D40 + 0x8);
    r4 = *(u32*)(lbl_803A1D40 + 0xC);
    if ((s32)tmp == 0x1) goto L_8001BFB8;
    if ((s32)tmp >= 0x2) {
        if ((s32)tmp >= 0x3) goto L_8001C020;
        goto L_8001C014;
    }
    if ((s32)tmp < 0x0) goto L_8001C020;
    /* tmp == 0 */
    if ((u32)r29 >= 0x6) goto L_8001C038;
    if ((s32)tmp == 0x1) goto L_8001BF60;
    if ((s32)tmp >= 0x2) goto L_8001BF94;
    if ((s32)tmp < 0x0) goto L_8001BF94;
    /* tmp == 0, r29 < 6 */
    r3 = 0x8ae;
    fn_801906A0();
    if (r3 == 0) {
        r3 = 0x0;
        r4 = 0x2;
        fn_80129280();
        goto L_8001BF98;
    }
    fn_8006AEEC();
    goto L_8001BF98;
    L_8001BF60:
    if (r4 != 0) goto L_8001BF70;
    r3 = 0x0;
    fn_801F2A7C();
    L_8001BF70:
    if (r3 == 0) {
        r3 = 0x0;
        goto L_8001BF98;
    }
    r4 = 0x0;
    r5 = 0x44;
    r6 = 0x0;
    fn_801FB1C0();
    goto L_8001BF98;
    L_8001BF94:
    r3 = 0x0;
    L_8001BF98:
    if (r3 != 0) {
        r4 = r29;
        fn_8012AC08();
        r30 = r3;
        goto L_8001C020;
    }
    r30 = 0x0;
    goto L_8001C038;
    L_8001BFB8:
    if ((u32)r29 >= 0x6) goto L_8001C038;
    if (r4 != 0) goto L_8001BFD4;
    r3 = 0x0;
    fn_801F2A7C();
    L_8001BFD4:
    if (r3 == 0) {
        r30 = 0x0;
        goto L_8001C038;
    }
    r4 = r29;
    fn_801F986C();
    if (r3 == 0) {
        r30 = 0x0;
        goto L_8001C038;
    }
    r4 = 0x0;
    r5 = 0xcc;
    r6 = 0x0;
    fn_8012640C();
    r30 = r3;
    goto L_8001C020;
    L_8001C014:
    if ((u32)r29 < 0x1e) goto L_8001C020;
    goto L_8001C038;
    L_8001C020:
    r3 = r30;
    fn_80123FBC();
    if ((r3 & 0xFF) != 0) goto L_8001C038;
    r30 = 0x0;
    L_8001C038:
    r4 = *(u16*)((u8*)r31 + 0x0);
    r3 = r30;
    *(u16*)((u8*)r31 + 0x0) = (u16)(r4 + 0x1);
    return;
}
#endif

/* fn_8001D624 - 0x8001D624 | size: 0xf4 */
extern u8 lbl_802E4EC8[];
#if 0
asm void fn_8001D624(void) {
#include "src/game/gs_pcbox_fn_8001D624.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
u16 fn_8001D624(void* a, u8 b) {
    extern u32 fn_8012640C();
    extern u32 fn_80120FE0();
    u32 r3 = fn_8012640C(a, 0x0, 0x7b, 0x0);
    if ((u8)r3 == 0x1) {
        r3 = 0x1;
    } else {
        r3 = fn_80120FE0(a);
        switch ((u16)r3) {
        case 0x3a: r3 = 0x2; break;
        case 0x3b: r3 = 0x3; break;
        case 0x3c: r3 = 0x4; break;
        case 0x3d: r3 = 0x5; break;
        case 0x3e: r3 = 0x6; break;
        default: r3 = 0x0; break;
        }
    }
    if ((u8)b == 0) {
        return *(u16*)(lbl_802E4EB8 + (u16)r3 * 2);
    } else {
        return *(u16*)(lbl_802E4EC8 + (u16)r3 * 2);
    }
}
#pragma pop
#endif

/* fn_8001D7E4 - 0x8001D7E4 | size: 0x50 -- already decompiled above */

/* fn_8001DA60 - 0x8001DA60 | size: 0x6c */
#if 0
asm void fn_8001DA60(void) {
#include "src/game/gs_pcbox_fn_8001DA60.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_8001DA60(void* a) {
    extern u8 fn_80123FBC();
    extern u32 fn_8011F5C8();
    extern u32 fn_801231A4();
    u32 r3;
    r3 = fn_80123FBC();
    if ((r3 & 0xFF) == 0) return (s32)0xFF;
    r3 = fn_8011F5C8(a);
    if ((r3 & 0xFFFF) == 0x1d || (r3 & 0xFFFF) == 0x20) return 0x2;
    return (s32)fn_801231A4(a);
}
#pragma peephole reset
#endif

/* fn_8001E074 - 0x8001E074 | size: 0x110 */
#if 0
asm void fn_8001E074(void) {
#include "src/game/gs_pcbox_fn_8001E074.inc"
}
#else
#pragma optimization_level 4
s8 fn_8001E074(u8 arg1, s16 arg2, s16 arg3, u32 arg4) {
    extern void* fn_801046B8();
    extern s32 fn_801026A4(s32, ...);
    extern void fn_80102868(s32, s16, s16);
    extern void fn_801045A8(s32, s32);
    extern u32 fn_801043A4(s32);
    extern void fn_80102568(s32, s32, s32);
    u32 sp8;
    s16 r30;
    s16 r31;
    sp8 = arg4;
    if (arg4 != 0) sp8 = 1;
    if ((u8)arg1 == 0x0) r30 = 0x11;
    else if ((u8)arg1 == 0x1) r30 = 0x12;
    else r30 = 0x44;
    r31 = r30;
    fn_801026A4((s32)r31, fn_801046B8(), &sp8, 0, 0, 0);
    if (arg2 >= 0 && arg3 >= 0) fn_80102868((s32)r31, arg2, arg3);
    fn_801045A8((s32)r31, 0x1);
    r30 = (s8)fn_801043A4((s32)r31);
    fn_80102568((s32)r31, 0x0, 0x1);
    return (s8)r30;
}
#endif

/* fn_8001E184 - 0x8001E184 | size: 0x7c */
#if 0
asm void fn_8001E184(void) {
#include "src/game/gs_pcbox_fn_8001E184.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_8001E184(void) {
    extern void* fn_801046B8();
    extern void fn_801026A4(s32, ...);
    extern void fn_801045A8();
    extern u32 fn_801043A4();
    extern void fn_80102568();
    u32 sp8;
    s8 r31;
    sp8 = 0;
    fn_801026A4(0x12, fn_801046B8(), &sp8, 0, 0, 0);
    fn_801045A8(0x12, 0x1);
    r31 = (s8)fn_801043A4(0x12);
    fn_80102568(0x12, 0x0, 0x1);
    return r31;
}
#pragma pop
#endif

/* fn_8001E200 - 0x8001E200 | size: 0x24 */
#if 0
asm void fn_8001E200(void) {
#include "src/game/gs_pcbox_fn_8001E200.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling off
void fn_8001E200(void) {
    extern void fn_80102510();
    fn_80102510(0x2);
}
#pragma pop
#endif

/* fn_8001E224 - 0x8001E224 | size: 0xe0 */
#if 0
asm void fn_8001E224(void) {
#include "src/game/gs_pcbox_fn_8001E224.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_8001E224(void* a, u32* b, u8 c, void* d, void* e, u8 f) {
    extern void* fn_801046B8();
    extern void fn_801026A4(s32, ...);
    extern void fn_80102868();
    extern void fn_801045A8();
    extern void fn_801043A4();
    extern u8* fn_80104704();
    extern void fn_80102510();
    void* r4;
    u8* r3;
    s32 r31;
    u8 c_val;
    r31 = 0;
    r4 = fn_801046B8();
    c_val = c;
    fn_801026A4(0x2, r4, 0, 0, 0, 0x3, a, c_val, 0);
    fn_80102868(0x2, d, e);
    fn_801045A8(0x2, 0x1);
    fn_801043A4(0x2);
    r3 = fn_80104704(0x2);
    if (r3 != 0) {
        if (b != 0) *b = *(u32*)(r3 + 0x80);
        if (*(u8*)(r3 + 0x99) == 0) r31 = 1;
        if (f != 0) fn_80102510(0x2);
    }
    return r31;
}
#pragma peephole reset
#endif

/* fn_8001E304 - 0x8001E304 | size: 0xdc */
#if 0
asm void fn_8001E304(void) {
#include "src/game/gs_pcbox_fn_8001E304.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_8001E304(void* a, u32* b, void* c) {
    extern void* fn_801046B8();
    extern void fn_801026A4(s32, ...);
    extern void fn_80102868();
    extern void fn_801045A8();
    extern void fn_801043A4();
    extern u8* fn_80104704();
    extern void fn_80102510();
    void* r4_tmp;
    u8* r3;
    s32 r31;
    r31 = 0;
    r4_tmp = fn_801046B8();
    fn_801026A4(0x2, r4_tmp, 0, 0, 0, 0x3, a, 0x1, c);
    fn_80102868(0x2, 0x32, 0x3c);
    fn_801045A8(0x2, 0x1);
    fn_801043A4(0x2);
    r3 = fn_80104704(0x2);
    if (r3 != 0) {
        if (b != 0) *b = *(u32*)(r3 + 0x80);
        if (*(u8*)(r3 + 0x99) == 0) r31 = 1;
        fn_80102510(0x2);
    }
    return r31;
}
#pragma peephole reset
#endif

/* fn_8001E58C - 0x8001E58C | size: 0xb8 */
#if 0
asm void fn_8001E58C(void) {
#include "src/game/gs_pcbox_fn_8001E58C.inc"
}
#else
#pragma optimization_level 4
void fn_8001E58C(s16 x1, s16 y1, s16 x2, s16 y2, u8* color) {
    extern void fn_800D88DC();
    extern void fn_800D888C();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D67BC();
    extern void fn_800D61E4();
    extern void fn_800D5CB8();
    extern void fn_800D6728();
    extern u8 lbl_80314E08[];
    fn_800D88DC(0x1);
    fn_800D888C(0x6);
    fn_800D6A00(0x7);
    fn_800D7820(lbl_80314E08);
    x2 += x1;
    y2 += y1;
    fn_800D67BC(0x2);
    fn_800D61E4(x1, y1);
    fn_800D5CB8(0, color[0], color[1], color[2], color[3]);
    fn_800D61E4(x2, y2);
    fn_800D5CB8(0, color[0], color[1], color[2], color[3]);
    fn_800D6728();
}
#endif
