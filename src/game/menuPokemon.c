/**
 * @file menuPokemon.c
 * @brief menuPokemon -- Party Pokemon menu (PC box / party list) UI.
 *
 * Address range: 0x800181C4 - 0x8001D7E4 (42 functions)
 *
 * Split from game/gs_pcbox.c (originally mislabeled -- this range is
 * actually the party Pokemon menu module, not the PC box module).
 * Corresponds to XD game/menuPokemon.cpp.
 *
 * NOTE: fn_800181C4 (0x3D0 bytes) is declared extern and called from
 * fn_800188E0 but has no recovered C body anywhere in the tree yet; it
 * remains an unimplemented hole within this address range.
 */

#include "dolphin/types.h"

/* =========================================================================
 * External declarations (shared)
 * ========================================================================= */

/* Pokemon data */
extern void  heroItemGetItemKindToItemAryPtr(void* pokeData, u8 fieldId, u16* outCount,
                          s32 p4, s32 p5, s32 p6);
extern void  heroHizukiItemGetItemAryPtr(void* pokeData, u16* outCount, s32 p3, s32 p4, s32 p5);
extern u8    fn_801429E8(void* fieldData);
extern u16   itemBiosGetNum(void* fieldData);
extern u16   itemDataBiosGetPtr(u16 speciesId);
extern u16   itemDataBiosGetPrice(void);

/* Text formatting */
extern void  fn_8002A0B8(void* outBuf, void* fmt, s32 p3, s32 p4,
                          u16 p5, s32 p6, ...);
extern s32   heroGetStatus(void* partyData, s32 slot, s32 p3);

/* Dialog/rendering */
extern void  winMsgOpenWithSE(s32 p1, void* text, s32 p3, s32 p4, u8 p5);
extern void  winMsgClose(s32 slot);
extern void  winSeqSetMenu(void* ctx, s32 state);
extern void  menuDataBiosGetXY(s16 npcId, u16* outX, u16* outY);
extern void  menuDataBiosSetXY(s16 x, s16 y, s16 z);
extern void* menuDataBiosGetPtr(void* data);

/* 0x8001C064 | 0x754 */
extern u32 fn_801906A0();
extern void savedataGetStatus();
extern u32 fn_8006AEEC();
extern u32 fightFloorGetGcHeroFightTrainerPtr();
extern u32 fightTrainerGetStatus();
extern u32 heroBiosGetPokemonPtr();
extern u32 fightTrainerGetValidFightPokemonPtr();
extern u32 pokemonGetStatus();
extern u32 pokemonCheckValid();
extern void msgctrlSetValue();
extern void pokemonGetSoubiItemDataId(void);
extern void heroItemCheckAddItemDataId(void);
extern void winMsgOpen();
extern void heroItemAddItemDataId(void);
extern void pokemonDoItemSoubi(void);
extern void heroItemDecItemDataId(void);
extern void pokemonToMenuPokemonStatus();
extern u32 pokemonGetJoutaiMenuSpriteId();
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
    extern void winMsgOpen();
    extern void pokemonToMenuPokemonStatus();
    extern void pokemonGetJoutaiMenuSpriteId();
    extern void pokemonGetSoubiItemDataId();
    extern void pokemonDoItemSoubi();
    extern void pokemonCheckValid();
    extern void pokemonGetStatus();
    extern void savedataGetStatus();
    extern void heroItemCheckAddItemDataId();
    extern void heroItemDecItemDataId();
    extern void heroItemAddItemDataId();
    extern void heroBiosGetPokemonPtr();
    extern void msgctrlSetValue();
    extern void fn_801906A0();
    extern void fightFloorGetGcHeroFightTrainerPtr();
    extern void fightTrainerGetValidFightPokemonPtr();
    extern void fightTrainerGetStatus();
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
                savedataGetStatus();
                goto L_8001C14C;
            }
            fn_8006AEEC();
            goto L_8001C14C;
        }
        if (r4 == 0) {
            r3 = 0x0;
            fightFloorGetGcHeroFightTrainerPtr();
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
        fightTrainerGetStatus();
        goto L_8001C14C;
    L_8001C148:
        r3 = 0x0;
    L_8001C14C:
        if (r3 == 0) {
            r27 = 0x0;
            goto L_8001C204;
        }
        r4 = r25;
        heroBiosGetPokemonPtr();
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
        fightFloorGetGcHeroFightTrainerPtr();
        r4 = r3;
    }
    if (r4 == 0) {
        r27 = 0x0;
        goto L_8001C204;
    }
    r3 = r4;
    r4 = r25;
    fightTrainerGetValidFightPokemonPtr();
    if (r3 == 0) {
        r27 = 0x0;
        goto L_8001C204;
    }
    r4 = 0x0;
    r5 = 0xcc;
    r6 = 0x0;
    pokemonGetStatus();
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
    pokemonCheckValid();
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
    msgctrlSetValue();
    r3 = r27;
    pokemonGetSoubiItemDataId();
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
            savedataGetStatus();
            goto L_8001C2C0;
        }
        fn_8006AEEC();
        goto L_8001C2C0;
    }
    if (r4 == 0) {
        r3 = 0x0;
        fightFloorGetGcHeroFightTrainerPtr();
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
    fightTrainerGetStatus();
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
            heroItemCheckAddItemDataId();
            if ((s32)r3 <= 0) {
                r3 = 0x2;
                r4 = 0x2b6b;
                r5 = 0x1;
                r6 = 0x1;
                winMsgOpen();
                break;
            }
            r3 = r25;
            r4 = r26;
            r5 = 0x1;
            r6 = -0x1;
            heroItemAddItemDataId();
            r3 = r27;
            r4 = 0x0;
            r5 = 0x0;
            pokemonDoItemSoubi();
            if (r29 != 0) {
                *(u16*)((u8*)r29 + 0x0) = r3;
            }
            r4 = r3 & 0xFFFF;
            r3 = 0x2d;
            msgctrlSetValue();
            r3 = 0x2;
            r4 = 0x2b69;
            r5 = 0x1;
            r6 = 0x1;
            winMsgOpen();
            break;
        }
        r3 = 0x2;
        r4 = 0x2b6a;
        r5 = 0x1;
        r6 = 0x1;
        winMsgOpen();
        break;
    }
    r4 = r26 & 0xFFFF;
    if (r4 != 0) {
        r3 = 0x2d;
        msgctrlSetValue();
        r3 = 0x2;
        r4 = 0x2b66;
        r5 = 0x1;
        r6 = 0x0;
        winMsgOpen();
        r3 = 0x0;
        r4 = -0x1;
        r5 = -0x1;
        r6 = 0x0;
        r24 = fn_8001E074((u8)r3, (s16)r4, (s16)r5, (u32)r6);
        r3 = 0x1;
        ((void(*)(void))winMsgClose)();
        if ((s32)r24 != 0) {
            r3 = -0x1;
            return;
        }
        r3 = r25;
        r4 = r30;
        r6 = r28;
        r5 = 0x1;
        heroItemDecItemDataId();
        if ((s32)r3 != 0) {
            r3 = 0x2;
            r4 = 0x2b6b;
            r5 = 0x1;
            r6 = 0x1;
            winMsgOpen();
            break;
        }
        r3 = r25;
        r4 = r26;
        heroItemCheckAddItemDataId();
        if ((s32)r3 <= 0) {
            r4 = (u32)lbl_803A1D40;
            r3 = r25;
            r5 = (u32)lbl_803A1D40;
            r4 = r30;
            r6 = *(u8*)((u8*)r5 + 0x11);
            r5 = 0x1;
            heroItemAddItemDataId();
            r3 = 0x2;
            r4 = 0x2b6b;
            r5 = 0x1;
            r6 = 0x1;
            winMsgOpen();
            break;
        }
        r3 = r25;
        r4 = r26;
        r5 = 0x1;
        r6 = -0x1;
        heroItemAddItemDataId();
        r3 = r27;
        r4 = 0x0;
        r5 = 0x0;
        pokemonDoItemSoubi();
        r26 = r3;
        if (r29 != 0) {
            *(u16*)((u8*)r29 + 0x0) = r26;
        }
        r3 = r27;
        r4 = r30;
        r5 = 0x1;
        pokemonDoItemSoubi();
        r4 = r26 & 0xFFFF;
        r3 = 0x2d;
        msgctrlSetValue();
        r4 = r30 & 0xFFFF;
        r3 = 0x2e;
        msgctrlSetValue();
        r3 = 0x2;
        r4 = 0x2b67;
        r5 = 0x1;
        r6 = 0x0;
        winMsgOpen();
        break;
    }
    r3 = r25;
    r4 = r30;
    r6 = r28;
    r5 = 0x1;
    heroItemDecItemDataId();
    if ((s32)r3 == 0) {
        r3 = r27;
        r4 = r30;
        r5 = 0x1;
        pokemonDoItemSoubi();
        r4 = r30 & 0xFFFF;
        r3 = 0x2d;
        msgctrlSetValue();
        r3 = 0x2;
        r4 = 0x2b68;
        r5 = 0x1;
        r6 = 0x0;
        winMsgOpen();
    }
} while (0);
    r3 = 0x1;
    ((void(*)(void))winMsgClose)();
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
                    savedataGetStatus();
                    goto L_8001C604;
                }
                fn_8006AEEC();
                goto L_8001C604;
            }
            if (r3 == 0) {
                r3 = 0x0;
                fightFloorGetGcHeroFightTrainerPtr();
            }
            if (r3 == 0) {
                r3 = 0x0;
                goto L_8001C604;
            }
            r4 = 0x0;
            r5 = 0x44;
            r6 = 0x0;
            fightTrainerGetStatus();
            goto L_8001C604;
        L_8001C600:
            r3 = 0x0;
        L_8001C604:
            if (r3 == 0) {
                r25 = 0x0;
                break;
            }
            r4 = r27;
            heroBiosGetPokemonPtr();
            r25 = r3;
            break;
        }
        if (r4 >= 6) {
            break;
        }
        if (r3 == 0) {
            r3 = 0x0;
            fightFloorGetGcHeroFightTrainerPtr();
        }
        if (r3 == 0) {
            r25 = 0x0;
            break;
        }
        r4 = r27;
        fightTrainerGetValidFightPokemonPtr();
        if (r3 == 0) {
            r25 = 0x0;
            break;
        }
        r4 = 0x0;
        r5 = 0xcc;
        r6 = 0x0;
        pokemonGetStatus();
        r25 = r3;
        break;
    L_8001C680:
        if (r4 >= 0x1e) {
            break;
        }
    } while (0);
        r3 = r25;
        pokemonCheckValid();
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
            pokemonToMenuPokemonStatus();
            r3 = r25;
            r4 = 0x0;
            r5 = 0x7b;
            r6 = 0x0;
            pokemonGetStatus();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                tmp = 0x0;
                *(u16*)((u8*)r29 + 0x1A) = tmp;
            }
            r3 = r25;
            r4 = 0x0;
            r5 = 0x7b;
            r6 = 0x0;
            pokemonGetStatus();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                tmp = 0x1;
                break;
            }
            r3 = r25;
            pokemonGetJoutaiMenuSpriteId();
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
extern s32 menuOpen();
extern void menuClose();
extern void menuCloseCustom();
extern void fadeSet();
extern void fadeCheck();
extern void fn_80097F08(void);
extern s32 menuOpenCustom();
extern u8* windowSearchID();
extern s32 menuGetCursorItemID();
extern u32 _threadSwitch(void);
extern void winSeqCheckMove(void);
extern void pokemonReplace(void);
extern u32 fn_80019064(void);
extern void fn_80018F54();
extern void fn_8001D718(f32 target);
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
    extern void fn_8006AEEC();
    extern void fn_80097F08();
    extern void _threadSwitch();
    extern void menuGetCursorItemID();
    extern void menuClose();
    extern void menuCloseCustom();
    extern void menuOpen();
    extern void menuOpenCustom();
    extern void windowSearchID();
    extern void winSeqCheckMove();
    extern void pokemonToMenuPokemonStatus();
    extern void pokemonGetJoutaiMenuSpriteId();
    extern void pokemonReplace();
    extern void pokemonCheckValid();
    extern void pokemonGetStatus();
    extern void savedataGetStatus();
    extern void heroBiosGetPokemonPtr();
    extern void fn_801906A0();
    extern void fadeCheck();
    extern void fadeSet();
    extern void fightFloorGetGcHeroFightTrainerPtr();
    extern void fightTrainerGetValidFightPokemonPtr();
    extern void fightTrainerGetStatus();
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
            menuOpen();
            r23 = r3;
            r3 = 0x6b;
            menuClose();
        } else {

            r3 = 0x10f;
            r4 = 0x1;
            menuOpen();
            r23 = r3;
            r3 = 0x10f;
            menuClose();
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
                menuCloseCustom();
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
                            savedataGetStatus();
                            goto L_8001C940;
                        }
                        fn_8006AEEC();
                        goto L_8001C940;
                    }
                    if (r3 == 0) {
                        r3 = 0x0;
                        fightFloorGetGcHeroFightTrainerPtr();
                    }
                    if (r3 == 0) {
                        r3 = 0x0;
                        goto L_8001C940;
                    }
                    r4 = 0x0;
                    r5 = 0x44;
                    r6 = 0x0;
                    fightTrainerGetStatus();
                    goto L_8001C940;
                L_8001C93C:
                    r3 = 0x0;
                L_8001C940:
                    if (r3 == 0) {
                        r24 = 0x0;
                        break;
                    }
                    r4 = r23;
                    heroBiosGetPokemonPtr();
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
                    fightFloorGetGcHeroFightTrainerPtr();
                }
                if (r3 == 0) {
                    r24 = 0x0;
                    break;
                }
                r4 = r23;
                fightTrainerGetValidFightPokemonPtr();
                if (r3 == 0) {
                    r24 = 0x0;
                    break;
                }
                r4 = 0x0;
                r5 = 0xcc;
                r6 = 0x0;
                pokemonGetStatus();
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
                pokemonCheckValid();
                tmp = r3 & 0xFF;
                if (tmp == 0) {
                    r24 = 0x0;
                }
            } while (0);
                f1 = lbl_8047B7C0;
                r3 = 0x3;
                fadeSet();
                r3 = 0x1;
                fadeCheck();
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
                menuOpenCustom();
                r3 = 0x63;
                windowSearchID();
                if (r3 != 0) {
                    tmp = 0x1;
                    *(u8*)((u8*)r3 + 0x98) = tmp;
                }
                f1 = lbl_8047B7C0;
                r3 = 0x2;
                fadeSet();
                r3 = 0x1;
                fadeCheck();
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
                menuOpenCustom();
                tmp = *(u8*)((u8*)r31 + 0x1);
                r3 = (s8)r3;
                if (tmp == 0) {
                    r3 = -0x2;
                }
                tmp = -0x1;
                r28 = (s8)r3;
                *(u8*)((u8*)r31 + 0x7) = tmp;
                r3 = 0x63;
                menuGetCursorItemID();
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
                                savedataGetStatus();
                                goto L_8001CBB8;
                            }
                            fn_8006AEEC();
                            goto L_8001CBB8;
                        }
                        if (r3 == 0) {
                            r3 = 0x0;
                            fightFloorGetGcHeroFightTrainerPtr();
                        }
                        if (r3 == 0) {
                            r3 = 0x0;
                            goto L_8001CBB8;
                        }
                        r4 = 0x0;
                        r5 = 0x44;
                        r6 = 0x0;
                        fightTrainerGetStatus();
                        goto L_8001CBB8;
                    L_8001CBB4:
                        r3 = 0x0;
                    L_8001CBB8:
                        if (r3 == 0) {
                            r27 = 0x0;
                            break;
                        }
                        r4 = r23;
                        heroBiosGetPokemonPtr();
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
                        fightFloorGetGcHeroFightTrainerPtr();
                    }
                    if (r3 == 0) {
                        r27 = 0x0;
                        break;
                    }
                    r4 = r23;
                    fightTrainerGetValidFightPokemonPtr();
                    if (r3 == 0) {
                        r27 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    pokemonGetStatus();
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
                    pokemonCheckValid();
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
                                savedataGetStatus();
                                goto L_8001CD20;
                            }
                            fn_8006AEEC();
                            goto L_8001CD20;
                        }
                        if (r3 == 0) {
                            r3 = 0x0;
                            fightFloorGetGcHeroFightTrainerPtr();
                        }
                        if (r3 == 0) {
                            r3 = 0x0;
                            goto L_8001CD20;
                        }
                        r4 = 0x0;
                        r5 = 0x44;
                        r6 = 0x0;
                        fightTrainerGetStatus();
                        goto L_8001CD20;
                    L_8001CD1C:
                        r3 = 0x0;
                    L_8001CD20:
                        if (r3 == 0) {
                            r29 = 0x0;
                            break;
                        }
                        r4 = r28 & 0xFFFF;
                        heroBiosGetPokemonPtr();
                        r29 = r3;
                        break;
                    }
                    tmp = r28 & 0xFFFF;
                    if (tmp >= 6) {
                        break;
                    }
                    if (r3 == 0) {
                        r3 = 0x0;
                        fightFloorGetGcHeroFightTrainerPtr();
                    }
                    if (r3 == 0) {
                        r29 = 0x0;
                        break;
                    }
                    r4 = r28 & 0xFFFF;
                    fightTrainerGetValidFightPokemonPtr();
                    if (r3 == 0) {
                        r29 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0xcc;
                    r6 = 0x0;
                    pokemonGetStatus();
                    r29 = r3;
                    break;
                L_8001CDA0:
                    tmp = r28 & 0xFFFF;
                    if (tmp >= 0x1e) {
                        break;
                    }
                } while (0);
                    r3 = r29;
                    pokemonCheckValid();
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
                    ((void(*)(void))menuDataBiosGetXY)();
                    tmp = *(s16*)((u8*)(u32)sp + 0x1E);
                    r3 = r25;
                    if ((s32)tmp > 0xfa) {
                        r4 = 0x11a;
                    } else {

                        r4 = 0x122;
                    }
                    ((void(*)(void))winSeqSetMenu)();
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
                    ((void(*)(void))menuDataBiosGetXY)();
                    tmp = *(s16*)((u8*)(u32)sp + 0x1A);
                    r3 = r28;
                    if ((s32)tmp > 0xfa) {
                        r4 = 0x11a;
                    } else {

                        r4 = 0x122;
                    }
                    ((void(*)(void))winSeqSetMenu)();
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
                        ((void(*)(void))menuDataBiosGetXY)();
                        r3 = r28;
                        winSeqCheckMove();
                        tmp = r3 & 0xFF;
                        if (tmp == 0) break;
                        _threadSwitch();

                    }
                    r3 = r27;
                    r4 = r29;
                    pokemonReplace();
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
                                    savedataGetStatus();
                                    goto L_8001CFB4;
                                }
                                fn_8006AEEC();
                                goto L_8001CFB4;
                            }
                            if (r3 == 0) {
                                r3 = 0x0;
                                fightFloorGetGcHeroFightTrainerPtr();
                            }
                            if (r3 == 0) {
                                r3 = 0x0;
                                goto L_8001CFB4;
                            }
                            r4 = 0x0;
                            r5 = 0x44;
                            r6 = 0x0;
                            fightTrainerGetStatus();
                            goto L_8001CFB4;
                        L_8001CFB0:
                            r3 = 0x0;
                        L_8001CFB4:
                            if (r3 == 0) {
                                r23 = 0x0;
                                break;
                            }
                            r4 = r29;
                            heroBiosGetPokemonPtr();
                            r23 = r3;
                            break;
                        }
                        if (r4 >= 6) {
                            break;
                        }
                        if (r3 == 0) {
                            r3 = 0x0;
                            fightFloorGetGcHeroFightTrainerPtr();
                        }
                        if (r3 == 0) {
                            r23 = 0x0;
                            break;
                        }
                        r4 = r29;
                        fightTrainerGetValidFightPokemonPtr();
                        if (r3 == 0) {
                            r23 = 0x0;
                            break;
                        }
                        r4 = 0x0;
                        r5 = 0xcc;
                        r6 = 0x0;
                        pokemonGetStatus();
                        r23 = r3;
                        break;
                    L_8001D030:
                        if (r4 >= 0x1e) {
                            break;
                        }
                    } while (0);
                        r3 = r23;
                        pokemonCheckValid();
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
                            pokemonToMenuPokemonStatus();
                            r3 = r23;
                            r4 = 0x0;
                            r5 = 0x7b;
                            r6 = 0x0;
                            pokemonGetStatus();
                            tmp = r3 & 0xFF;
                            if (tmp == 1) {
                                tmp = 0x0;
                                *(u16*)((u8*)r28 + 0x1A) = tmp;
                            }
                            r3 = r23;
                            r4 = 0x0;
                            r5 = 0x7b;
                            r6 = 0x0;
                            pokemonGetStatus();
                            tmp = r3 & 0xFF;
                            if (tmp == 1) {
                                tmp = 0x1;
                                break;
                            }
                            r3 = r23;
                            pokemonGetJoutaiMenuSpriteId();
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
                    fn_8001D718(lbl_8047B7C0);
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
                    ((void(*)(void))menuDataBiosGetXY)();
                    tmp = *(s16*)((u8*)(u32)sp + 0x12);
                    r3 = r24;
                    if ((s32)tmp > 0xfa) {
                        r4 = 0x116;
                    } else {

                        r4 = 0x11e;
                    }
                    ((void(*)(void))winSeqSetMenu)();
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
                    ((void(*)(void))menuDataBiosGetXY)();
                    tmp = *(s16*)((u8*)(u32)sp + 0xE);
                    r3 = r24;
                    if ((s32)tmp > 0xfa) {
                        r4 = 0x116;
                    } else {

                        r4 = 0x11e;
                    }
                    ((void(*)(void))winSeqSetMenu)();
                    while (1) {
                        tmp = *(u8*)((u8*)r31 + 0x4);
                        r4 = (u32)sp + 0xa;
                        r5 = (u32)sp + 0xc;
                        tmp = (s8)tmp;
                        tmp = tmp * 0x30;
                        r24 = *(s16*)(r30 + tmp);
                        r3 = r24;
                        ((void(*)(void))menuDataBiosGetXY)();
                        r3 = r24;
                        winSeqCheckMove();
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
            menuOpen();
            r24 = r3;
            r3 = 0x6c;
            menuClose();
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
                menuClose();
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
                menuOpenCustom();
                r3 = 0x63;
                windowSearchID();
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
    extern void pokemonToMenuPokemonStatus();
    extern void pokemonGetJoutaiMenuSpriteId();
    extern void pokemonCheckValid();
    extern void pokemonGetStatus();
    extern void savedataGetStatus();
    extern void heroBiosGetPokemonPtr();
    extern void fn_801906A0();
    extern void fightFloorGetGcHeroFightTrainerPtr();
    extern void fightTrainerGetValidFightPokemonPtr();
    extern void fightTrainerGetStatus();
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
                    savedataGetStatus();
                    goto L_8001D474;
                }
                fn_8006AEEC();
                goto L_8001D474;
            }
            if (r3 == 0) {
                r3 = 0x0;
                fightFloorGetGcHeroFightTrainerPtr();
            }
            if (r3 == 0) {
                r3 = 0x0;
                goto L_8001D474;
            }
            r4 = 0x0;
            r5 = 0x44;
            r6 = 0x0;
            fightTrainerGetStatus();
            goto L_8001D474;
        L_8001D470:
            r3 = 0x0;
        L_8001D474:
            if (r3 == 0) {
                r27 = 0x0;
                break;
            }
            r4 = r29;
            heroBiosGetPokemonPtr();
            r27 = r3;
            break;
        }
        if (r4 >= 6) {
            break;
        }
        if (r3 == 0) {
            r3 = 0x0;
            fightFloorGetGcHeroFightTrainerPtr();
        }
        if (r3 == 0) {
            r27 = 0x0;
            break;
        }
        r4 = r29;
        fightTrainerGetValidFightPokemonPtr();
        if (r3 == 0) {
            r27 = 0x0;
            break;
        }
        r4 = 0x0;
        r5 = 0xcc;
        r6 = 0x0;
        pokemonGetStatus();
        r27 = r3;
        break;
    L_8001D4F0:
        if (r4 >= 0x1e) {
            break;
        }
    } while (0);
        r3 = r27;
        pokemonCheckValid();
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
            pokemonToMenuPokemonStatus();
            r3 = r27;
            r4 = 0x0;
            r5 = 0x7b;
            r6 = 0x0;
            pokemonGetStatus();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                tmp = 0x0;
                *(u16*)((u8*)r28 + 0x1A) = tmp;
            }
            r3 = r27;
            r4 = 0x0;
            r5 = 0x7b;
            r6 = 0x0;
            pokemonGetStatus();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                tmp = 0x1;
                break;
            }
            r3 = r27;
            pokemonGetJoutaiMenuSpriteId();
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
extern s32 fn_800D37CC(void);
extern u32 fn_800D3088(void);
extern f32 lbl_8047B7C8;
extern f64 lbl_8047B7D0;
extern f64 lbl_8047B7D8;
#if 0
asm void fn_8001D718(void) {
#include "src/game/gs_pcbox_fn_8001D718.inc"
}
#else
void fn_8001D718(f32 target) {
    extern void _threadSwitch();
    f32 progress = lbl_8047B7C8;

    while (progress < target) {
        _threadSwitch();
        progress += (f32)fn_800D3088() / (f32)fn_800D37CC();
    }
}
#endif

typedef struct MenuPokemonQuantityArgs {
    s32 columns;
    u8 color[4];
    s32 menuId;
    s32 maximum;
    s32 minimum;
    s32 unitPrice;
} MenuPokemonQuantityArgs;

s32 fn_800181C4(page, itemId, itemSlot)
    s32 page;
    u16 itemId;
    s16 itemSlot;
{
    extern void* itemDataBiosGetPtr(u16 itemId);
    extern u16 itemDataBiosGetPrice(void* itemData);
    extern u32 fn_8002A0B8(u8* color, u32 messageContext, s32 group,
                           s32 category, ...);
    extern void winMsgOpenWithSE(s32, u32, s32, s32, u8);
    extern void winMsgOpen(s32, u32, s32, s32);
    extern void winMsgClose(s32);
    extern u8* heroItemGetItemKindToItemAryPtr(void* hero, u8 kind,
                                               u16* count, s32, s32, s32);
    extern u8* heroHizukiItemGetItemAryPtr(void* hero, u16* count,
                                           s32, s32, s32);
    extern u8 fn_801429E8(void* item);
    extern u16 itemBiosGetNum(void* item);
    extern s32 windowGetActiveID(void);
    extern s32 menuOpenCustom(s32 menuId, s32 parent, void* args,
                              s32, s32, s32, void* color);
    extern void menuClose(s32 menuId);
    extern void menuCloseSync(s32 menuId, s32 wait);
    extern s8 menuSubOpenYesNo(s32, s32, s32, s32);
    extern void fn_8012959C(void* hero, u16 item, u16 count, s16 slot);
    extern void heroItemDecItemDataId(void* hero, u16 item, u16 count,
                                      s16 slot);
    extern void fn_80166AB8(s32 sound, s32, s32);
    extern void heroAddPokedoru(void* hero, u32 amount);
    extern u8 lbl_80266918[];
    extern u32 lbl_8047A2BC;
    extern u32 lbl_8047A2DC;
    extern void* lbl_8047A2F8;
    extern s32 lbl_8047A2FC;
    MenuPokemonQuantityArgs quantityArgs;
    u8 messageColor;
    u8* summary;
    u8* inventory;
    u16 inventoryCount;
    u16 quantity;
    u16 price;
    u32 message;
    s32 validIndex;
    s32 menuId;
    s32 choice;
    s32 selectedQuantity;
    s32 i;
    s32 kind;
    u32 saleValue;

    summary = lbl_80266918 + page * 0x4C;
    price = itemDataBiosGetPrice(itemDataBiosGetPtr(itemId));
    if (price == 0) {
        message = fn_8002A0B8(&messageColor, lbl_8047A2BC, 0xB, 0x2D,
                              itemId, -1);
        winMsgOpenWithSE(2, message, 1, 0, messageColor);
        winMsgClose(1);
        return 0;
    }

    message = fn_8002A0B8(&messageColor, lbl_8047A2BC, 0xD, 0x2D,
                          itemId, -1);
    lbl_8047A2DC = message;
    kind = *(s32*)(summary + 4);
    if (kind >= 0) {
        inventory = heroItemGetItemKindToItemAryPtr(
            lbl_8047A2F8, (u8)kind, &inventoryCount, 0, 0, 0);
    } else {
        inventory = heroHizukiItemGetItemAryPtr(
            lbl_8047A2F8, &inventoryCount, 0, 0, 0);
    }

    validIndex = -1;
    quantity = 0;
    for (i = 0; i < inventoryCount; i++, inventory += 4) {
        if (fn_801429E8(inventory)) {
            validIndex++;
            if (validIndex >= itemSlot) {
                quantity = itemBiosGetNum(inventory);
                break;
            }
        }
    }
    if (quantity < 1) {
        return 0;
    }

    saleValue = price / 2;
    if (inventoryCount > 100) {
        menuId = saleValue > 0 ? 0x5E : 0x5C;
        quantityArgs.columns = 2;
    } else {
        menuId = saleValue > 0 ? 0x5D : 0x5B;
        quantityArgs.columns = 1;
    }
    quantityArgs.color[0] = summary[0];
    quantityArgs.color[1] = summary[1];
    quantityArgs.color[2] = summary[2];
    quantityArgs.color[3] = 0;
    quantityArgs.menuId = menuId;
    quantityArgs.maximum = quantity;
    quantityArgs.minimum = 1;
    quantityArgs.unitPrice = saleValue;
    lbl_8047A2FC = 1;

    choice = menuOpenCustom(menuId, windowGetActiveID(), &quantityArgs,
                            0, 1, 1, quantityArgs.color);
    menuClose(menuId);
    menuCloseSync(menuId, 1);
    if (choice == -1) {
        return 0;
    }
    selectedQuantity = lbl_8047A2FC;
    if (selectedQuantity < 0) {
        return 0;
    }

    saleValue *= selectedQuantity;
    message = fn_8002A0B8(&messageColor, lbl_8047A2BC, 9, 0x4B,
                          saleValue, -1);
    winMsgOpenWithSE(2, message, 1, 0, messageColor);
    choice = menuSubOpenYesNo(0, -1, -1, 0);
    if (choice == 1 || choice == -1) {
        winMsgClose(1);
        return 0;
    }

    if (kind == -1) {
        fn_8012959C(lbl_8047A2F8, itemId, (u16)selectedQuantity, itemSlot);
    } else {
        heroItemDecItemDataId(lbl_8047A2F8, itemId,
                              (u16)selectedQuantity, itemSlot);
    }
    fn_80166AB8(0x3CB, 0, 0);
    heroAddPokedoru(lbl_8047A2F8, saleValue);
    message = fn_8002A0B8(&messageColor, lbl_8047A2BC, 0xA, 0x2D,
                          itemId, 0x4B, saleValue, -1);
    winMsgOpen(2, message, 1, 0);
    winMsgClose(1);
    return 1;
}

/* fn_80018594 - 0x80018594 | size: 0x34c */
extern u32 itemDataBiosGetFieldUseFunc();
extern u32 itemDataBiosGetBattleUseFunc();
extern s32 fn_80017CB8();
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
#if 0
asm s32 fn_80018594() {
#include "src/game/gs_pcbox_fn_80018594.inc"
}
#else
s32 fn_80018594(u32 boxIndex, u32 speciesId, u32 slotIndex, u16* outSpecies) {
    extern void* windowGetActiveID();
    extern s32 menuOpenCustom();
    extern void menuClose();
    struct {
        u8 r;
        u8 g;
        u8 b;
        u8 pad;
        void* entries;
        s32 count;
    } menuArg;
    u8 menuEntries[0x3C];
    u8* summaryEntry;
    s32 count;
    s32 choice;
    s32 callbackRet;
    s32 callbackValue;
    s32 (*menuCallback)();
    s32 (*speciesCallback)();
    u32 color;
    u32 selectedSpecies;

    *outSpecies = 0;
    if ((s32)lbl_8047A2E0 == 0 && (u16)speciesId == 0x21E) {
        summaryEntry = sSummaryPageEntries + (s32)lbl_8047A2D8 * 0x4C;
        lbl_8047A2DC = 0x2B41;
        color = (summaryEntry[0] << 24) | (summaryEntry[1] << 16) | (summaryEntry[2] << 8) | 0xFF;
        selectedSpecies = (u32)fn_80019754((void*)color);
        if ((u16)selectedSpecies == 0) {
            return 1;
        }

        *outSpecies = (u16)selectedSpecies;
        itemDataBiosGetPtr((u16)selectedSpecies);
        if ((s32)lbl_8047A2E0 == 0) {
            speciesCallback = (s32 (*)())itemDataBiosGetFieldUseFunc();
        } else {
            speciesCallback = (s32 (*)())itemDataBiosGetBattleUseFunc();
        }
        if (speciesCallback == 0) {
            winMsgOpen(2, 0x4261, 1, 0);
            winMsgClose(1);
            return 0;
        }

        callbackValue = 0;
        if (speciesCallback((u16)selectedSpecies, &callbackValue) == 2) {
            if (callbackValue > 0) {
                ((void (*)())heroItemDecItemDataId)(lbl_8047A2F8, selectedSpecies, (u16)callbackValue, -1);
            }
            return 4;
        }
        return 1;
    }

    count = fn_80017CB8(menuEntries, 5, boxIndex, slotIndex);
    summaryEntry = sSummaryPageEntries + (s32)lbl_8047A2D8 * 0x4C;
    lbl_8047A2DC = 0x135;
    menuArg.r = summaryEntry[0];
    menuArg.g = summaryEntry[1];
    menuArg.b = summaryEntry[2];
    menuArg.pad = 0;
    menuArg.entries = menuEntries;
    menuArg.count = count;

    choice = menuOpenCustom(0x5A, windowGetActiveID(), 0, 0, 1, 1, &menuArg);
    menuClose(0x5A);
    menuCloseSync(0x5A, 1);
    if (choice < 0 || choice >= count) {
        return 1;
    }

    menuCallback = *(s32 (**)())(menuEntries + 4 + choice * 0xC);
    if (menuCallback == 0) {
        return 1;
    }

    callbackValue = 0;
    callbackRet = menuCallback(boxIndex, slotIndex, &callbackValue);
    switch (callbackRet) {
    case 0:
        if (callbackValue > 0) {
            summaryEntry = sSummaryPageEntries + boxIndex * 0x4C;
            if (*(s32*)(summaryEntry + 4) == -1) {
                ((void (*)())fn_8012959C)(lbl_8047A2F8, speciesId, (u16)callbackValue, (s16)slotIndex);
            } else {
                ((void (*)())heroItemDecItemDataId)(lbl_8047A2F8, speciesId, (u16)callbackValue, (s16)slotIndex);
            }
        }
        lbl_8047A2B8 = slotIndex;
        return 0;
    case 1:
        return 2;
    case 2:
        return 3;
    case 3:
        return 1;
    case 4:
        if (callbackValue > 0) {
            summaryEntry = sSummaryPageEntries + boxIndex * 0x4C;
            if (*(s32*)(summaryEntry + 4) == -1) {
                ((void (*)())fn_8012959C)(lbl_8047A2F8, speciesId, (u16)callbackValue, (s16)slotIndex);
            } else {
                ((void (*)())heroItemDecItemDataId)(lbl_8047A2F8, speciesId, (u16)callbackValue, (s16)slotIndex);
            }
        }
        return 4;
    default:
        return 0;
    }
}
#endif

/* fn_800188E0 - 0x800188E0 | size: 0x188 */
extern u32 fn_80143FCC();
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
            extern void itemDataBiosGetPtr();
            itemDataBiosGetPtr(r5);
        }
        r0 = fn_80143FCC();
        if ((u8)r0 != 0) {
            msgctrlSetValue(0x2d, (u16)r5);
            winMsgOpen(2, 0x4262, 1, 0);
            winMsgClose(1);
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
extern u32 cursorBiosGetPos();
extern void cursorBiosSetPos();
extern u16 itemBiosGetItemDataId();
extern void fn_800FF660(void);
extern void floorSetFadeScript();
extern u32 lbl_80478860;
extern u8 lbl_802E4DB0[];
extern u8 lbl_802EF0A8[];
extern u32 lbl_8047A2E8;
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2D8;
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2E4;
#if 0
asm u16 fn_80018A68(void) {
#include "src/game/gs_pcbox_fn_80018A68.inc"
}
#else
u16 fn_80018A68(void) {
    extern void* windowGetActiveID();
    extern s32 menuOpenCustom();
    extern void menuCloseCustom();
    u32 filterValue;
    u32* filterPtr;
    u32 selectedOffset;
    u32 retrySelection;
    u32 done;
    u32 selectedSpecies;
    u16 encoded;
    u16 fieldCount;
    u16 adjusted;
    u16 replacementSpecies;
    u8* summaryEntry;
    u8* fieldData;
    u8* posEntry;
    s32 i;
    s32 occupied;
    s32 leftCount;
    s32 rightCount;
    s32 excess;
    s32 fieldId;
    s32 selection;
    s32 ret;
    s32 activeIndex;
    s32 targetIndex;

    filterPtr = 0;
    selectedOffset = 0;
    retrySelection = 0;
    done = 0;
    selectedSpecies = 0;

    if (*(u8*)&lbl_80478860 != 0) {
        for (i = 0; i < 8; i++) {
            summaryEntry = lbl_802E4DB0 + i * 0xC;
            posEntry = lbl_802EF0A8 + *(u32*)summaryEntry * 0x1C;
            *(s16*)(summaryEntry + 4) = *(s16*)(posEntry + 4);
            *(s16*)(summaryEntry + 6) = *(s16*)(posEntry + 8);
        }
        *(u8*)&lbl_80478860 = 0;
    }

    encoded = (u16)(cursorBiosGetPos(2) >> 16);
    if ((s8)encoded <= 0) {
        filterValue = 5;
        filterPtr = &filterValue;
    }

    while (done == 0) {
        if (retrySelection != 0) {
            lbl_8047A2E8 = selectedOffset;
        } else {
            lbl_8047A2E8 = -1;
        }

        summaryEntry = sSummaryPageEntries;
        for (i = 0; i < 6; i++, summaryEntry += 0x4C) {
            encoded = (u16)(cursorBiosGetPos((u16)*(u32*)(summaryEntry + 0x1C)) >> 16);
            leftCount = (s8)(encoded >> 8);
            rightCount = (s8)encoded;
            if (rightCount < 0) {
                rightCount = 0;
            }
            if (leftCount < 0) {
                leftCount = 0;
            }

            fieldId = *(s32*)(summaryEntry + 4);
            fieldCount = 0;
            if (fieldId >= 0) {
                fieldData = ((u8* (*)())heroItemGetItemKindToItemAryPtr)((void*)lbl_8047A2F8, (u8)fieldId, &fieldCount, 0, 0, 0);
            } else {
                fieldData = ((u8* (*)())heroHizukiItemGetItemAryPtr)((void*)lbl_8047A2F8, &fieldCount, 0, 0, 0);
            }

            occupied = 0;
            for (targetIndex = 0; targetIndex < (s32)fieldCount; targetIndex++, fieldData += 4) {
                if (fn_801429E8(fieldData) != 0) {
                    occupied++;
                }
            }

            excess = (leftCount + rightCount) - occupied;
            if (excess > 0) {
                rightCount -= (s8)excess;
                if (rightCount < 0) {
                    rightCount = 0;
                }
                excess = (leftCount + rightCount) - occupied;
                if (excess > 0) {
                    leftCount -= (s8)excess;
                    if (leftCount < 0) {
                        leftCount = 0;
                    }
                }
            }

            adjusted = ((u8)leftCount << 8) | (u8)rightCount;
            cursorBiosSetPos((u16)*(u32*)(summaryEntry + 0x1C), &adjusted);
        }

        lbl_8047A2D8 = -1;
        selection = menuOpenCustom(0x59, windowGetActiveID(), filterPtr, 0, 1, 0);
        lbl_8047A2D8 = selection;
        if (selection == -1) {
            selectedSpecies = 0;
        } else {
            summaryEntry = sSummaryPageEntries + selection * 0x4C;
            encoded = (u16)(cursorBiosGetPos((u16)*(u32*)(summaryEntry + 0x1C)) >> 16);
            targetIndex = (s8)(encoded >> 8) + (s8)encoded;
            fieldId = *(s32*)(summaryEntry + 4);
            fieldCount = 0;
            if (fieldId >= 0) {
                fieldData = ((u8* (*)())heroItemGetItemKindToItemAryPtr)((void*)lbl_8047A2F8, (u8)fieldId, &fieldCount, 0, 0, 0);
            } else {
                fieldData = ((u8* (*)())heroHizukiItemGetItemAryPtr)((void*)lbl_8047A2F8, &fieldCount, 0, 0, 0);
            }

            activeIndex = -1;
            selectedSpecies = 0;
            for (i = 0; i < (s32)fieldCount; i++, fieldData += 4) {
                if (fn_801429E8(fieldData) != 0) {
                    activeIndex++;
                    if (activeIndex >= targetIndex) {
                        selectedSpecies = itemBiosGetItemDataId(fieldData);
                        break;
                    }
                }
            }
            selectedOffset = targetIndex;
        }

        filterPtr = 0;
        retrySelection = 0;
        if ((u16)selectedSpecies == 0) {
            done = 1;
        } else {
            replacementSpecies = 0;
            ret = fn_800188E0(lbl_8047A2E0, lbl_8047A2D8, selectedSpecies, selectedOffset, &replacementSpecies);
            if (replacementSpecies != 0) {
                selectedSpecies = replacementSpecies;
            }
            switch (ret) {
            case 1:
                done = 1;
                break;
            case 2:
                retrySelection = 1;
                break;
            case 3:
                filterValue = 0;
                filterPtr = &filterValue;
                break;
            default:
                break;
            }
        }
    }

    menuCloseCustom(0x59, 0, 1);
    if ((s32)lbl_8047A2E4 != 0) {
        fn_800FF660();
        floorSetFadeScript(0, 0);
    }
    return (u16)selectedSpecies;
}
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
extern u32 fightTrainer_GetHeroPtr();
extern void _flagSet();
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
        lbl_8047A2F8 = fightTrainer_GetHeroPtr(val);
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
    _flagSet(1, 0);
    fn_800FF730(0x38f);
    floorSetFadeScript(0, 0);
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
typedef struct MenuPokemonSpeciesCacheEntry {
    u16 species;
    u16 pad;
    u32 data;
} MenuPokemonSpeciesCacheEntry;

typedef struct MenuPokemonSpeciesCache {
    MenuPokemonSpeciesCacheEntry entries[8];
    s32 count;
} MenuPokemonSpeciesCache;

extern u8 lbl_803A1B90[];
#if 0
asm void fn_80019070(void) {
#include "src/game/gs_pcbox_fn_80019070.inc"
}
#else
#pragma optimization_level 4
u32 fn_80019070(u32 species) {
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
#pragma peephole off
void fn_800190D8(u32 species, u32 data) {
    MenuPokemonSpeciesCache* cache;
    s32 count;

    cache = (MenuPokemonSpeciesCache*)lbl_803A1B90;
    count = cache->count;
    if (count >= 8) {
        return;
    }
    species = (u16)species;
    cache->entries[count].species = species;
    cache->entries[cache->count].data = data;
    cache->count = cache->count + 1;
}
#pragma pop
#endif

/* fn_80019118 - 0x80019118 | size: 0xec */
extern void winSpriteSetDisp();
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
    winSpriteSetDisp(b, r30);
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
    extern void winSpriteSetDisp();
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
    winSpriteSetDisp(b, r4);
    return 0;
}
#endif

/* fn_800192A8 - 0x800192A8 | size: 0x228 */
extern u32 itemDataBiosGetName(u32 a);
extern void fn_800FB680();
extern u32 itemDataBiosGetKind(u32 a);
extern u32 GSmsgGetRect(u32 a);
extern void fn_80142CF4(void);
extern u8 lbl_80266BF0[];
#if 0
asm void fn_800192A8(void) {
#include "src/game/gs_pcbox_fn_800192A8.inc"
}
#else
#pragma optimization_level 4
s32 fn_800192A8(u8* a, u8* b) {
    extern void heroItemGetItemKindToItemAryPtr();
    extern void msgctrlSetValue();
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
        r6 = (u32)itemDataBiosGetPtr(r3u);
        r6 = (u32)itemDataBiosGetName(r6);
    }
    ((void(*)(u32,u32,u32,u32))fn_800FB680)(0, 0, (u32)(s32)(-1), r6);
    r3u = *(u16*)((u8*)r31 + r28);
    if (r3u == 0) return 0;
    r4 = (u32)itemDataBiosGetPtr(r3u);
    r4 = (u32)itemDataBiosGetKind(r4);
    heroItemGetItemKindToItemAryPtr(0, r4, &sp_a, 0, 0, 0);
    ((void(*)(u32,u32))msgctrlSetValue)(0x34, (u32)(u16)sp_a);
    r29  = (s32)(s16)(u16)(((u32)((u32(*)(u32))GSmsgGetRect)(0xca)) >> 16);
    r29 += (s32)(s16)(u16)(((u32)((u32(*)(u32))GSmsgGetRect)(0x12e)) >> 16);
    r6 = (u32)(s32)(0xc3 - r29);
    ((void(*)(u32,u32,u32,u32))fn_800FB680)(r6, 0, (u32)(s32)(-1), 0x12e);
    r28b = (u32)*(u16*)((u8*)r31 + r28);
    r4 = (u32)itemDataBiosGetPtr((u16)r28b);
    r4 = (u32)itemDataBiosGetKind(r4);
    heroItemGetItemKindToItemAryPtr(0, r4, &sp_8, 0, 0, 0);
    r30 = 0;
    r29 = (s32)r4; /* r3 after heroItemGetItemKindToItemAryPtr = field array ptr */
    {
        u32 r31_acc;
        r31_acc = 0;
        while ((s32)r30 < (s32)(u16)sp_8) {
            if (fn_801429E8((void*)r29) != 0) {
                if (((u32(*)(u32,u32,u32,u32))fn_80142CF4)((u32)r29, 0, 0x1b, 0) == r28b) {
                    r31_acc += (u32)itemBiosGetNum((void*)r29);
                }
            }
            r30++;
            r29 += 4;
        }
        ((void(*)(u32,u32))msgctrlSetValue)(0x34, r31_acc);
    }
    r29 = (s32)(s16)(u16)(((u32)((u32(*)(u32))GSmsgGetRect)(0xca)) >> 16);
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
    extern u8* windowGetKeyInfo();
    u8* r3;
    u8* r30;
    u32 r31;
    s32 r5;
    s32 r0;
    s32 r4;
    r30 = a;
    r31 = *(u32*)(a + 0x60);
    r3 = windowGetKeyInfo();
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
            winSeqSetMenu((void*)0x5f, 0x66);
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
            winSeqSetMenu((void*)0x5f, 0x6a);
            arg[2] = 1;
        }
        break;
    }
    return 0;
}
#pragma peephole reset
#endif

/* fn_80019754 - 0x80019754 | size: 0x1e4 */
extern u32 heroItemCheckHaveItemDataId();
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
    extern s32 windowGetActiveID(void);
    extern s32 menuOpenCustom(s32, s32, s32, s32, s32, s32, void*, ...);
    extern void menuClose(s32);
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
        itemDataBiosGetPtr(i);
        if ((u8)itemDataBiosGetKind(0) == 6) {
            if ((u8)heroItemCheckHaveItemDataId(0, i)) {
                ids[count] = (u16)i;
                count++;
            }
        }
    }
    ids[count] = 0;
    count_plus_one = count + 1;

    r4 = windowGetActiveID();
    sel = menuOpenCustom(0x5f, r4, 0, 0, 1, 1, &arg);
    menuClose(0x5f);
    menuCloseSync(0x5f, 1);
    if (sel >= 0 && sel < count_plus_one) {
        return ids[sel];
    }
    return 0;
}
#endif

/* fn_80019938 - 0x80019938 | size: 0xbc */
extern u8* windowGetAllocPtr();
#if 0
asm void fn_80019938(void) {
#include "src/game/gs_pcbox_fn_80019938.inc"
}
#else
#pragma optimization_level 4
void fn_80019938(u8* a, u8* b) {
    extern void msgctrlSetValue(s32, u32);
    extern void fn_800FB680(s32, s32, s32, u32);
    u8* base;
    s16 r0;
    u32 r4;
    u32 r4v;
    base = windowGetAllocPtr();
    r0 = *(s16*)(b + 0x6);
    r4 = 0x0;
    switch (r0) {
    case (s16)0xe93:
        r4 = 0;
        break;
    case (s16)0xe94:
        r4 = 1;
        break;
    case (s16)0xe95:
        r4 = 2;
        break;
    case (s16)0xe96:
        r4 = 3;
        break;
    }
    base = base + r4 * 0xc;
    r4v = *(u32*)(base + 0x4);
    if (r4v == 0) return;
    msgctrlSetValue(0x37, r4v);
    fn_800FB680(0x0, 0x0, (s32)*(u8*)(a + 0x8b) | (s32)(-0x100), 0xe7);
}
#endif

/* fn_800199F4 - 0x800199F4 | size: 0x128 */
extern void* windowAllocMemory();
extern void menuItemBiosSetSelectFlag();
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
        dst = windowAllocMemory(arg, 0x48);
        if (dst != NULL) {
            memcpy(dst, *(void**)(arg + 0x60), 0x48);
        }
    }
    {
        u8* tmp;
        tmp = windowGetAllocPtr(arg);
        i = 0;
        ids = (u16*)&lbl_80478870;
        entry = tmp;
    }
    while (i < 4) {
        if (*(u32*)(entry + 4) != 0) {
            menuItemBiosSetSelectFlag(*ids, 1);
        } else {
            menuItemBiosSetSelectFlag(*ids, 0);
        }
        entry += 0xc;
        ids++;
        i++;
    }
    {
        s32 val;
        val = menuGetCursorItemID(*(u32*)(arg + 4));
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
    extern void menuCloseCustom();
    menuCloseCustom(0x42, 0x0, 0x1);
}
#pragma pop
#endif

/* fn_80019B48 - 0x80019B48 | size: 0x214 */
extern void pokemonToMenuWazaStatus(void);
extern u8 lbl_803A1BD8[];
#if 0
asm void fn_80019B48(void) {
#include "src/game/gs_pcbox_fn_80019B48.inc"
}
#else
#pragma optimization_level 4
s32 fn_80019B48(s32 a) {
    extern u8 lbl_803A1D40[];
    extern void menuOpenCustom();
    extern void menuCloseCustom();
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
                savedataGetStatus();
                r3 = 0;
            } else {
                fn_8006AEEC();
                r3 = 0;
            }
        } else if (r5 == 0x1) {
            if (r4 == 0) {
                r3 = 0x0;
                fightFloorGetGcHeroFightTrainerPtr();
                r4 = r3;
            }
            if (r4 == 0) {
                r3 = 0;
            } else {
                r3 = r4;
                r4 = 0x0; r5 = 0x44; r6 = 0x0;
                fightTrainerGetStatus();
                r3 = 0;
            }
        } else {
            r3 = 0;
        }
        if (r3 == 0) { r31 = 0; goto L_80019CE0; }
        r4 = (u32)r30;
        heroBiosGetPokemonPtr();
        r31 = r3;
        goto L_80019CC8;
    } else if ((s32)r5 == 0x1) {
        r30 = (s8)a;
        if ((u32)(s16)r30 >= 0x6) goto L_80019CE0;
        if (r4 == 0) { r3 = 0; fightFloorGetGcHeroFightTrainerPtr(); r4 = r3; }
        if (r4 == 0) { r31 = 0; goto L_80019CE0; }
        r3 = r4; r4 = (u32)r30;
        fightTrainerGetValidFightPokemonPtr();
        if (r3 == 0) { r31 = 0; goto L_80019CE0; }
        r4 = 0x0; r5 = 0xcc; r6 = 0x0;
        pokemonGetStatus();
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
    pokemonCheckValid();
    if ((r3 & 0xFF) != 0) goto L_80019CE0;
    r31 = 0;
    L_80019CE0:
    if (r31 == 0) return -1;
    ((void(*)(u32,u8*))pokemonToMenuWazaStatus)(r31, lbl_803A1BD8);
    menuOpenCustom(0x42, 0x0, 0x0, 0x0, 0x1, 0x1, lbl_803A1BD8);
    r30 = r3;
    menuCloseCustom(0x42, 0x0, 0x1);
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
    extern void savedataGetStatus();
    extern void fn_8006AEEC();
    extern void fightFloorGetGcHeroFightTrainerPtr();
    extern void fightTrainerGetStatus();
    extern void heroBiosGetPokemonPtr();
    extern void fightTrainerGetValidFightPokemonPtr();
    extern void pokemonGetStatus();
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
            savedataGetStatus();
        } else {
            fn_8006AEEC();
        }
        r3 = 0;
        goto L_80019E9C;
        L_80019E64:
        if (r3 != 0) goto L_80019E74;
        r3 = 0x0;
        fightFloorGetGcHeroFightTrainerPtr();
        L_80019E74:
        if (r3 == 0) { r3 = 0; goto L_80019E9C; }
        r4 = 0x0; r5 = 0x44; r6 = 0x0;
        fightTrainerGetStatus();
        goto L_80019E9C;
        L_80019E98:
        r3 = 0;
        L_80019E9C:
        if (r3 == 0) { r27 = 0; goto L_80019F44; }
        r4 = (u32)r28;
        heroBiosGetPokemonPtr();
        r27 = r3;
        goto L_80019F2C;
    } else if ((s32)r4 == 0x1) {
        if ((u16)(s16)r28 >= 0x6) goto L_80019F44;
        if (r3 == 0) { r3 = 0; fightFloorGetGcHeroFightTrainerPtr(); }
        if (r3 == 0) { r27 = 0; goto L_80019F44; }
        r4 = (u32)r28;
        fightTrainerGetValidFightPokemonPtr();
        if (r3 == 0) { r27 = 0; goto L_80019F44; }
        r4 = 0x0; r5 = 0xcc; r6 = 0x0;
        pokemonGetStatus();
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
    pokemonCheckValid();
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

/* menuPokemonDrawItem - 0x80019F6C | size: 0xa18 */
extern s32 windowGetParam(s32, s32);
extern u32 itemDataBiosGetWazaMachineNo();
extern u32 pokemonIsDarkPokemon();
extern u32 pokemonDataBiosGetWazaMcn();
extern void fn_800FBB34();
extern void fn_8010B9E8();
extern u32 fn_80107E78();
extern void fn_801081F8();
extern void windowDrawSprite2();
extern u32 pokemonDataBiosGetPtr();
extern void fn_8001DACC();
extern u32 GSmsgGetGSchar();
extern u32 lbl_8047A308;
extern u32 lbl_8047B7B8;
extern u32 lbl_8047B7B0;
extern u32 menuSubCalcColor();
#if 0
asm void menuPokemonDrawItem(void) {
#include "src/game/gs_pcbox_menuPokemonDrawItem.inc"
}
#else
void menuPokemonDrawItem(u8* ctx, u8* pane) {
    u8* entry;
    u8* mon;
    u8* base;
    s32 state;
    s32 paneId;
    s32 visible;
    s32 color;
    s32 msg;
    s32 tmp;
    s32 species;
    s32 count;
    s32 value;
    s32 x;
    s32 y;
    s32 grade;
    f32 ratio;

    entry = (u8*)windowGetParam((s32)ctx, 0);
    if (entry == NULL) {
        return;
    }

    state = 0;
    if (*(u16*)(entry + 0) == 0) {
        state = -1;
    } else if (*(s16*)(entry + 0x1A) == 0) {
        state = 1;
    }

    visible = 1;
    msg = -1;
    paneId = (s32)*(s16*)(pane + 6);
    if (state == -1) {
        if ((paneId >= 0x3BA && paneId < 0x3C2) || (paneId >= 0x3C4 && paneId < 0x3C9) ||
            (paneId >= 0x545 && paneId < 0x548) || (paneId >= 0x12A0 && paneId < 0x12A3)) {
            visible = 0;
        }
    } else if (state == 1) {
        if ((paneId >= 0x3B8 && paneId < 0x3BA) || (paneId >= 0x3C4 && paneId < 0x3C9) ||
            paneId == 0x129D || paneId == 0x129E || paneId == 0x12A0) {
            msg = 0x808080FF;
        }
    }

    winSpriteSetDisp(pane, visible);
    *(u32*)(pane + 0x64) = msg;
    if ((u8)visible == 0) {
        return;
    }

    color = (s32)menuSubCalcColor(ctx, pane);
    count = (u16)windowGetParam((s32)ctx, 1);

    if (lbl_803A1D40[0] == 3 || lbl_803A1D40[0] == 4) {
        species = itemDataBiosGetPtr(*(u16*)(lbl_803A1D40 + 0x12));
        if ((u8)itemDataBiosGetKind(species) == 4) {
            if (paneId == 0x3BC || paneId == 0x12A1) {
                value = itemDataBiosGetWazaMachineNo(species);
                mon = NULL;
                switch (*(s32*)(lbl_803A1D40 + 8)) {
                case 0:
                    if ((u32)count < 6) {
                        tmp = fn_801906A0(0x8AE);
                        if (tmp == 0) {
                            savedataGetStatus(0, 2);
                        } else {
                            fn_8006AEEC();
                        }
                        if (tmp != 0) {
                            mon = (u8*)heroBiosGetPokemonPtr(tmp, count);
                        }
                    }
                    break;
                case 1:
                    if ((u32)count < 6) {
                        mon = *(u8**)(lbl_803A1D40 + 0xC);
                        if (mon == NULL) {
                            mon = (u8*)fightFloorGetGcHeroFightTrainerPtr(0);
                        }
                        if (mon != NULL) {
                            mon = (u8*)fightTrainerGetValidFightPokemonPtr(mon, count);
                            if (mon != NULL) {
                                mon = (u8*)pokemonGetStatus(mon, 0, 0xCC, 0);
                            }
                        }
                    }
                    break;
                case 2:
                    if ((u32)count < 0x1E) {
                        mon = NULL;
                    }
                    break;
                }
                if (mon != NULL && (u8)pokemonCheckValid(mon) == 0) {
                    mon = NULL;
                }
                if ((u8)pokemonIsDarkPokemon(mon) != 0) {
                    msg = 0x2B65;
                } else if ((u8)pokemonDataBiosGetWazaMcn(pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(mon)), value) != 0) {
                    msg = 0x2B64;
                } else {
                    msg = 0x2B65;
                }
                fn_800FB680(0, -4, color, msg);
            }
            if (paneId == 0x3BE || paneId == 0x12A2) {
                winSpriteSetDisp(pane, 0);
            }
        }
    }

    switch (paneId) {
    case 0x3C2:
    case 0x3C6:
    case 0x129D:
    case 0x129E:
        if (entry[0x29] == 0) {
            winSpriteSetDisp(pane, 1);
        } else {
            winSpriteSetDisp(pane, 0);
        }
        break;
    case 0x3B8:
    case 0x3B9:
    case 0x3C3:
    case 0x3C7:
    case 0x3C8:
        if (entry[0x29] == 0) {
            winSpriteSetDisp(pane, 0);
        } else if (entry[0x29] == 1) {
            winSpriteSetDisp(pane, 1);
        } else if (entry[0x29] == 2) {
            ratio = (f32)*(s16*)&lbl_8047A308 / *(f32*)&lbl_8047B7B0;
            fn_8001DACC(ctx, pane, ratio);
            winSpriteSetDisp(pane, 0);
        }
        break;
    case 0x3BA:
    case 0x3CB:
        if (*(u16*)(entry + 0x24) != 0) {
            windowDrawSprite2(0, 0, *(s16*)(pane + 0x54), *(s16*)(pane + 0x56), *(u16*)(entry + 0x24), color, ctx, 0, 0);
        }
        break;
    case 0x3BB:
    case 0x3C9:
        if (*(u16*)(entry + 0x2A) != 0) {
            winSpriteSetDisp(pane, 1);
        } else {
            winSpriteSetDisp(pane, 0);
        }
        break;
    case 0x3BF:
        fn_800FB680(0x2E, 0, color, 0x2BD4);
        msgctrlSetValue(0x34, *(s16*)(entry + 0x1A));
        fn_800FBB34(0, 0, 0x2C, *(s16*)(pane + 0x56), color, 0xDE);
        msgctrlSetValue(0x34, *(s16*)(entry + 0x18));
        fn_800FBB34(0, 0, (s16)(*(s16*)(pane + 0x54) - 2), *(s16*)(pane + 0x56), color, 0xDE);
        break;
    case 0x545:
        fn_800FB680(0x1B, 0, color, 0x195);
        msgctrlSetValue(0x34, *(s16*)(entry + 0x1A));
        fn_800FBB34(0, 0, 0x1B, *(s16*)(pane + 0x56), color, 0xDF);
        msgctrlSetValue(0x34, *(s16*)(entry + 0x18));
        fn_800FBB34(0, 0, (s16)(*(s16*)(pane + 0x54) - 2), *(s16*)(pane + 0x56), color, 0xDF);
        break;
    case 0x3C0:
    case 0x546:
        msgctrlSetValue(0x34, entry[0x17]);
        fn_800FBB34(0, 0, *(s16*)(pane + 0x54), *(s16*)(pane + 0x56), color, 0xD3);
        break;
    case 0x3C1:
    case 0x547:
        msgctrlSetValue(0x37, entry);
        fn_800FB680(0, 0, color, 0xE7);
        x = (s16)(GSmsgGetRect(0xE7) >> 16);
        if (entry[0x28] == 0) {
            msg = 0xD67;
        } else if (entry[0x28] == 1) {
            msg = 0xD68;
        } else {
            msg = 0;
        }
        if (msg != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(msg));
            fn_800FB680(x - 2, 0, color, 0xCF);
        }
        break;
    case 0x3C4:
    case 0x12A0:
        fn_8010B9E8(ctx, pane, *(u16*)(entry + 0x26));
        break;
    case 0x3BE:
    case 0x12A2:
        x = *(s16*)(entry + 0x1A);
        y = *(s16*)(entry + 0x18);
        if (x <= 0) {
            grade = 0;
        } else if (x <= (y * 20) / 100) {
            grade = 0x66;
        } else if (x <= (y * 50) / 100) {
            grade = 0x65;
        } else {
            grade = 0x64;
        }
        if ((u16)grade != 0) {
            windowDrawSprite2(0, 0, (s16)((x * *(s16*)(pane + 0x54) + y - 1) / y), *(s16*)(pane + 0x56), grade, color, ctx, 0, 0);
        }
        break;
    case 0x543:
    case 0x12A4:
        if ((s8)lbl_803A1D40[7] >= 0 && (s8)lbl_803A1D40[7] < 6) {
            base = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30 + (s32)(s8)lbl_803A1D40[7] * 8;
            winSpriteSetDisp(pane, *(s16*)base == *(s32*)(ctx + 4));
        } else {
            winSpriteSetDisp(pane, 0);
        }
        break;
    case 0x542:
    case 0x12A3:
        if ((s8)lbl_803A1D40[6] >= 0 && (s8)lbl_803A1D40[6] < 6) {
            base = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30 + (s32)(s8)lbl_803A1D40[6] * 8;
            if (*(s16*)base == *(s32*)(ctx + 4)) {
                winSpriteSetDisp(pane, 1);
                if (lbl_803A1D40[0x14] == 0 || lbl_803A1D40[0x14] == 2) {
                    if ((u8)fn_80107E78(ctx, *(u16*)(pane + 6), 0x2D) == 0) {
                        fn_801081F8(ctx, *(u16*)(pane + 6), 0x2D);
                    }
                } else if (lbl_803A1D40[0x14] == 1 || lbl_803A1D40[0x14] == 3) {
                    if ((u8)fn_80107E78(ctx, *(u16*)(pane + 6), 0x20D) == 0) {
                        fn_801081F8(ctx, *(u16*)(pane + 6), 0x20D);
                    }
                }
            } else {
                winSpriteSetDisp(pane, 0);
            }
        } else {
            winSpriteSetDisp(pane, 0);
        }
        break;
    case 0x3B6:
    case 0x3B7:
        if (lbl_803A1D40[0x14] == 0 || lbl_803A1D40[0x14] == 2) {
            winSpriteSetDisp(pane, 1);
        } else if (lbl_803A1D40[0x14] == 1 || lbl_803A1D40[0x14] == 3) {
            winSpriteSetDisp(pane, 0);
        }
        break;
    }
}
#endif

/* menuPokemonDrawHelp - 0x8001A984 | size: 0x114 */
#if 0
asm void menuPokemonDrawHelp(void) {
#include "src/game/gs_pcbox_menuPokemonDrawHelp.inc"
}
#else
#pragma optimization_level 4
void menuPokemonDrawHelp(u8* a) {
    extern u8 lbl_803A1D40[];
    extern u8 lbl_803A1C20[];
    extern u32 itemDataBiosGetPtr();
    extern void msgctrlSetValue();
    extern void fn_800FB680();
    u32 r6;
    u32 r0;
    u8* r3;
    r6 = 0;
    r0 = *(u8*)(lbl_803A1D40 + 0x14);
    if ((s32)r0 == 0x0) {
        r0 = *(u8*)(lbl_803A1D40 + 0x0);
        if ((s32)r0 == 0x3 || (s32)r0 == 0x4) {
            r3 = (u8*)itemDataBiosGetPtr(*(u16*)(lbl_803A1D40 + 0x12));
            r0 = itemDataBiosGetKind((u32)r3);
            r6 = ((r0 & 0xFF) == 0x4) ? 0x2b63 : 0x2b61;
        } else if ((s32)r0 == 0x5) {
            r6 = 0x2b62;
        } else {
            r6 = 0x2b5d;
        }
    } else if ((s32)r0 == 0x1) {
        r3 = lbl_803A1C20 + (s32)((s8)*(u8*)(lbl_803A1D40 + 0x6)) * 0x30;
        msgctrlSetValue(0x32, r3);
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

/* menuPokemonButton - 0x8001AA98 | size: 0xd8 */
extern void fn_80166A28(void);
extern void menuButtonNormal(void);
#if 0
asm void menuPokemonButton(void) {
#include "src/game/gs_pcbox_menuPokemonButton.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void menuPokemonButton(u8* a) {
    extern u8 lbl_803A1D40[];
    extern u32 windowGetKeyInfo();
    extern u32 menuGetCursorItemID();
    extern void fn_80166A28(u32);
    extern void menuButtonNormal(u8*);
    s32 r3;
    u32 r31;
    u32 r3b;
    r31 = 0;
    if (*(u8*)(lbl_803A1D40 + 0x1) == 0) {
        *(u8*)(a + 0x98) = 0x1;
        return;
    }
    r3 = windowGetKeyInfo();
    r3 = *(u16*)((u8*)r3 + 0x4);
    if ((r3 & 0x10) != 0) {
        r3b = menuGetCursorItemID(*(u32*)(a + 0x4));
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
    menuButtonNormal(a);
}
#pragma pop
#endif

/* fn_8001AB70 - 0x8001AB70 | size: 0x3d4 */
extern u8 menuDataBiosGetType(void*);
#if 0
asm void fn_8001AB70(void) {
#include "src/game/gs_pcbox_fn_8001AB70.inc"
}
#else
void fn_8001AB70(u8* ctx) {
    extern u8* windowGetKeyInfo();
    u8 valid[8];
    u8* input;
    u8* mon;
    s32 mode;
    s32 index;
    s8 oldIndex;
    s8 newIndex;
    s8 step;
    u16 buttons;
    s32 count;

    oldIndex = *(s8*)(ctx + 0x95);
    newIndex = oldIndex;
    step = 0;

    for (index = 0; index < 6; index++) {
        mon = NULL;
        mode = *(s32*)(lbl_803A1D40 + 8);
        switch (mode) {
        case 0:
            if ((u32)(u16)index < 6) {
                mon = (u8*)fn_801906A0(0x8AE);
                if (mon == NULL) {
                    savedataGetStatus(0, 2);
                } else {
                    fn_8006AEEC();
                }
                if (mon != NULL) {
                    mon = (u8*)heroBiosGetPokemonPtr(mon, (u16)index);
                }
            }
            break;
        case 1:
            if ((u32)(u16)index < 6) {
                mon = *(u8**)(lbl_803A1D40 + 0xC);
                if (mon == NULL) {
                    mon = (u8*)fightFloorGetGcHeroFightTrainerPtr(0);
                }
                if (mon != NULL) {
                    mon = (u8*)fightTrainerGetValidFightPokemonPtr(mon, (u16)index);
                    if (mon != NULL) {
                        mon = (u8*)pokemonGetStatus(mon, 0, 0xCC, 0);
                    }
                }
            }
            break;
        case 2:
            if ((u32)(u16)index < 0x1E) {
                mon = (u8*)1;
            }
            break;
        }
        if (mon != NULL && (u8)pokemonCheckValid(mon) == 0) {
            mon = NULL;
        }
        valid[index] = (mon != NULL);
    }
    valid[6] = 1;

    input = windowGetKeyInfo();
    buttons = *(u16*)(input + 6);
    if ((buttons & 8) != 0) {
        step = 1;
        if ((s8)lbl_803A1D40[4] == 0) {
            if (oldIndex == 0) {
                if ((s8)lbl_803A1D40[5] == 3) {
                    newIndex = lbl_803A1D40[5];
                } else {
                    newIndex = 2;
                }
            }
        } else if (oldIndex == 0) {
            if ((s8)lbl_803A1D40[5] == 3) {
                newIndex = lbl_803A1D40[5];
            } else {
                newIndex = 2;
            }
        } else if (oldIndex == 1) {
            if ((s8)lbl_803A1D40[5] == 5) {
                newIndex = lbl_803A1D40[5];
            } else {
                newIndex = 4;
            }
        }
    } else if ((buttons & 4) != 0) {
        step = -1;
        if ((s8)lbl_803A1D40[4] == 0) {
            if (oldIndex >= 1 && oldIndex < 6) {
                newIndex = 0;
            }
        } else if ((s8)lbl_803A1D40[4] == 1) {
            if (oldIndex >= 2 && oldIndex < 4) {
                newIndex = 0;
            } else if (oldIndex >= 4 && oldIndex < 6) {
                newIndex = 1;
            }
        }
    }

    if ((buttons & 1) != 0) {
        newIndex--;
        step = -1;
        if (newIndex < 0) {
            newIndex = 6;
        }
    } else if ((buttons & 2) != 0) {
        count = menuDataBiosGetType(*(void**)(ctx + 4));
        step = 1;
        newIndex++;
        if (newIndex >= count) {
            newIndex = 0;
        }
    }

    if (step != 0) {
        while (valid[(s32)newIndex] == 0) {
            newIndex += step;
            if (newIndex < 0 || newIndex >= 7) {
                newIndex = 7;
                break;
            }
        }
    }

    if (oldIndex != newIndex) {
        lbl_803A1D40[5] = oldIndex;
        lbl_803A1D40[6] = newIndex;
        *(s8*)(ctx + 0x95) = newIndex;
    }
}
#endif

/* menuPokemonCtrl - 0x8001AF44 | size: 0x240 */
extern void menuItemBiosSetXY(s16 x, s16 y, s16 z);
extern u32 lbl_8047A308;
#if 0
asm void menuPokemonCtrl(void) {
#include "src/game/gs_pcbox_fn_8001AF44.inc"
}
#else
#pragma push
#pragma peephole off
s32 menuPokemonCtrl(s32 ctx) {
    extern u8 lbl_803A1D40[];
    extern u8 lbl_802E4E58[];
    extern s32 menuOpenCustom(s32, s32, s32, s32, s32, s32, void*, ...);
    u32 result;
    s32 byte_off;
    u8* iter;
    s32 i;

    result = windowGetParam(ctx, 0);
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
                menuItemBiosSetXY(*(s16*)(slot + 2), *(s16*)(slot + 4), *(s16*)(slot + 6));
                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30 + byte_off;
                menuDataBiosSetXY(*(s16*)(slot + 0), *(s16*)(slot + 4), *(s16*)(slot + 6));

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30;
                menuOpenCustom((s32)*(s16*)(slot + byte_off), 0x63, 0, 0, 0, 2, (void*)iter, i);

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30;
                npcId = *(s16*)(slot + byte_off);
                menuDataBiosGetXY(npcId, (u16*)&sx, (u16*)&sy);
                if (sx > 0xfa) state = 0x116;
                else state = 0x11e;
                winSeqSetMenu((void*)(s32)npcId, state);

                byte_off += 8;
                iter += 0x30;
            }
            winSeqSetMenu(*(void**)((u8*)ctx + 4), 1);
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
                menuDataBiosGetXY(npcId, (u16*)&sx, (u16*)&sy);
                if (sx > 0xfa) state = 0x11a;
                else state = 0x122;
                winSeqSetMenu((void*)(s32)npcId, state);

                i += 8;
            }
            winSeqSetMenu(*(void**)((u8*)ctx + 4), 7);
            *((s8*)ctx + 2) = 1;
        }
    }
    *(s16*)&lbl_8047A308 = (s16)(((s32)*(s16*)&lbl_8047A308 + 1) % 1000);
    return 0;
}
#pragma pop
#endif

/* menuPokemonClose - 0x8001B184 | size: 0x68 */
#if 0
asm void menuPokemonClose(void) {
#include "src/game/gs_pcbox_fn_8001B184.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling off
void menuPokemonClose(void) {
    extern u8 lbl_803A1D40[];
    extern void menuCloseCustom();
    extern void fn_800FF660();
    extern void floorSetFadeScript();
    extern void _threadSwitch();
    menuCloseCustom(0x63, 0x0, 0x1);
    if (*(u8*)(lbl_803A1D40 + 0x2) != 0) {
        fn_800FF660();
        if (*(u8*)(lbl_803A1D40 + 0x3) != 0x1) floorSetFadeScript(0x0, 0x0);
        _threadSwitch();
    }
}
#pragma pop
#endif

/* menuPokemonSub - 0x8001B1EC | size: 0x8d8 */
extern void fn_8010B01C();
extern u32 menuPokemonCheckPokemonChange();
extern void fn_80097E58();
extern f32 pokemonGetDp();
extern u32 fn_8010B560();
extern void fn_8001BEBC(void*);
extern u8 lbl_8047A30A[];
extern f32 lbl_8047B7C0;
extern u32 lbl_8047B7C4;
#if 0
asm void menuPokemonSub(void) {
#include "src/game/gs_pcbox_fn_8001B1EC.inc"
}
#else
s32 menuPokemonSub() {
    u8* ctx;
    u8* entries;
    u8* mon;
    s32 i;
    s32 mode;
    s32 result;
    s32 selection;
    s32 action;
    s32 tmp;
    u32 menuArg;
    f32 shadowGauge;

    ctx = lbl_803A1D40;
    entries = lbl_803A1C20;
    result = 0;

    ctx[6] = 0;
    ctx[5] = 0;
    ctx[7] = (u8)-1;
    if (ctx[1] == 0) {
        ctx[6] = (u8)-1;
    }

    memset(entries, 0, 0x120);

    for (i = 0; (u32)(u16)i < 6; i++) {
        mon = NULL;
        mode = *(s32*)(ctx + 8);
        switch (mode) {
        case 0:
            if ((u32)(u16)i < 6) {
                mon = (u8*)fn_801906A0(0x8AE);
                if (mon == NULL) {
                    savedataGetStatus(0, 2);
                } else {
                    fn_8006AEEC();
                }
                if (mon != NULL) {
                    mon = (u8*)heroBiosGetPokemonPtr(mon, i);
                }
            }
            break;
        case 1:
            if ((u32)(u16)i < 6) {
                mon = *(u8**)(ctx + 0xC);
                if (mon == NULL) {
                    mon = (u8*)fightFloorGetGcHeroFightTrainerPtr(0);
                }
                if (mon != NULL) {
                    mon = (u8*)fightTrainerGetValidFightPokemonPtr(mon, i);
                    if (mon != NULL) {
                        mon = (u8*)pokemonGetStatus(mon, 0, 0xCC, 0);
                    }
                }
            }
            break;
        case 2:
            if ((u32)(u16)i < 0x1E) {
                mon = NULL;
            }
            break;
        }
        if (mon != NULL && (u8)pokemonCheckValid(mon) == 0) {
            mon = NULL;
        }

        if (mon == NULL) {
            *(u16*)(entries + i * 0x30) = 0;
        } else {
            pokemonToMenuPokemonStatus(mon, entries + i * 0x30);
            if ((u8)pokemonGetStatus(mon, 0, 0x7B, 0) == 1) {
                *(u16*)(entries + i * 0x30 + 0x1A) = 0;
            }
            if ((u8)pokemonGetStatus(mon, 0, 0x7B, 0) == 1) {
                tmp = 1;
            } else {
                tmp = pokemonGetJoutaiMenuSpriteId(mon);
                switch ((u16)tmp) {
                case 0x3A:
                    tmp = 2;
                    break;
                case 0x3B:
                    tmp = 3;
                    break;
                case 0x3C:
                    tmp = 4;
                    break;
                case 0x3D:
                    tmp = 5;
                    break;
                case 0x3E:
                    tmp = 6;
                    break;
                default:
                    tmp = 0;
                    break;
                }
            }
            *(u16*)(entries + i * 0x30 + 0x24) = *(u16*)(lbl_802E4EB8 + (u16)tmp * 2);
        }
    }

    *(u16*)lbl_8047A30A = 0;
    fn_8010B01C(0, fn_8001BEBC, lbl_8047A30A);

    while (1) {
        ctx[0x14] = 0;
        menuArg = (s32)(s8)ctx[6];
        selection = (s8)menuOpenCustom(0x63, 0, &menuArg, 0, 1, 1, entries);
        if (ctx[1] == 0) {
            selection = -2;
        }
        ctx[6] = selection;
        if ((s8)selection == -2) {
            break;
        }

        if (menuGetCursorItemID(0x63) == 0x3B6 || (s8)selection == -1) {
            ctx[6] = (u8)-1;
            break;
        }

        result = 0;
        switch (ctx[0]) {
        case 1:
            result = ((s32 (*)(s32))fn_8001C7B8)(selection);
            break;
        case 2:
            ctx[0x14] = 1;
            while (1) {
                if (ctx[0x15] != 0) {
                    action = menuOpen(0x6D, 1);
                    menuClose(0x6D);
                } else {
                    action = menuOpen(0x108, 1);
                    menuClose(0x108);
                }

                if (action == -1 || action == 2) {
                    action = 1;
                    break;
                }
                if (action == 0) {
                    if ((u8)menuPokemonCheckPokemonChange(*(void**)(ctx + 0x18), *(u32*)(ctx + 0xC), (s8)selection) != 0) {
                        action = -1;
                    } else {
                        action = 1;
                    }
                    break;
                }
                if (action == 1) {
                    menuCloseCustom(0x63, 0, 1);
                    mon = NULL;
                    mode = *(s32*)(ctx + 8);
                    switch (mode) {
                    case 0:
                        if ((u32)(u16)(s8)selection < 6) {
                            mon = (u8*)fn_801906A0(0x8AE);
                            if (mon == NULL) {
                                savedataGetStatus(0, 2);
                            } else {
                                fn_8006AEEC();
                            }
                            if (mon != NULL) {
                                mon = (u8*)heroBiosGetPokemonPtr(mon, (s8)selection);
                            }
                        }
                        break;
                    case 1:
                        if ((u32)(u16)(s8)selection < 6) {
                            mon = *(u8**)(ctx + 0xC);
                            if (mon == NULL) {
                                mon = (u8*)fightFloorGetGcHeroFightTrainerPtr(0);
                            }
                            if (mon != NULL) {
                                mon = (u8*)fightTrainerGetValidFightPokemonPtr(mon, (s8)selection);
                                if (mon != NULL) {
                                    mon = (u8*)pokemonGetStatus(mon, 0, 0xCC, 0);
                                }
                            }
                        }
                        break;
                    case 2:
                        if ((u32)(u16)(s8)selection < 0x1E) {
                            mon = NULL;
                        }
                        break;
                    }
                    if (mon != NULL && (u8)pokemonCheckValid(mon) == 0) {
                        mon = NULL;
                    }
                    fadeSet(3, lbl_8047B7C0);
                    fadeCheck(1);
                    fn_80097E58(*(u32*)(ctx + 0xC), mon, entries + (s32)(s8)selection * 0x30, 0);
                    menuArg = (s32)(s8)ctx[6];
                    menuOpenCustom(0x63, 0, &menuArg, 0, 0, 1, entries + (s32)(s8)selection * 0x30);
                    mon = (u8*)windowSearchID(0x63);
                    if (mon != NULL) {
                        mon[0x98] = 1;
                    }
                    fadeSet(2, lbl_8047B7C0);
                    fadeCheck(1);
                    selection = (s8)ctx[6];
                    continue;
                }
                action = 0;
                break;
            }
            result = action;
            break;
        case 3:
            result = -1;
            break;
        case 5:
            result = ((s32 (*)(s32, u32, u32, u32*))fn_8001C064)(selection, ctx[0x11], *(u16*)(ctx + 0x12), &menuArg);
            break;
        case 6:
            result = -1;
            break;
        case 7:
            mon = NULL;
            mode = *(s32*)(ctx + 8);
            switch (mode) {
            case 0:
                if ((u32)(u16)i < 6) {
                    mon = (u8*)fn_801906A0(0x8AE);
                    if (mon == NULL) {
                        savedataGetStatus(0, 2);
                    } else {
                        fn_8006AEEC();
                    }
                    if (mon != NULL) {
                        mon = (u8*)heroBiosGetPokemonPtr(mon, i);
                    }
                }
                break;
            case 1:
                if ((u32)(u16)i < 6) {
                    mon = *(u8**)(ctx + 0xC);
                    if (mon == NULL) {
                        mon = (u8*)fightFloorGetGcHeroFightTrainerPtr(0);
                    }
                    if (mon != NULL) {
                        mon = (u8*)fightTrainerGetValidFightPokemonPtr(mon, i);
                        if (mon != NULL) {
                            mon = (u8*)pokemonGetStatus(mon, 0, 0xCC, 0);
                        }
                    }
                }
                break;
            case 2:
                if ((u32)(u16)i < 0x1E) {
                    mon = NULL;
                }
                break;
            }
            if (mon == NULL) {
                continue;
            }
            if ((u8)pokemonIsDarkPokemon(mon) == 0) {
                winMsgOpen(2, 0x44DD, 1, 0);
                winMsgClose(1);
                continue;
            }
            shadowGauge = pokemonGetDp(mon);
            if (*(f32*)&lbl_8047B7C4 != shadowGauge) {
                winMsgOpen(2, 0x44DE, 1, 0);
                winMsgClose(1);
                continue;
            }
            result = -1;
            break;
        default:
            break;
        }

        if (result < 0) {
            break;
        }
    }

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    *(s32*)(ctx + 0x1C) = (s32)(s8)ctx[6];
    return result;
}
#endif

/* menuPokemonOpenSub - 0x8001BAC4 | size: 0x228 */
extern void menuCreateOffScreen();
extern void menuReleaseOffScreen();
extern u32 lbl_8047B7B8;
extern u32 lbl_8047B7C4;
#if 0
asm void menuPokemonOpenSub(void) {
#include "src/game/gs_pcbox_fn_8001BAC4.inc"
}
#else
#pragma push
#pragma peephole off
u32 menuPokemonOpenSub(u32 a0, u8 a1, u8 a2, u16 a3, u32 a4, u8 a5) {
    extern u8 lbl_803A1D40[];
    extern s32 fn_800D37CC(void);
    extern void _flagSet();
    extern void fn_800FF730();
    extern void floorSetFadeScript();
    extern void _threadSwitch();
    extern void menuPokemonSub();
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
    menuCreateOffScreen(*(f32*)&lbl_8047B7C4 / (f32)fn_800D37CC());
    *(u8*)(lbl_803A1D40 + 0x2) = 1;
    *(u8*)(lbl_803A1D40 + 0x3) = 2;
BC20:
    if (*(u8*)(lbl_803A1D40 + 0x2) == 1) {
        _flagSet(1, 1);
        fn_800FF730(0x38f);
        if (*(u8*)(lbl_803A1D40 + 0x3) != 1) {
            floorSetFadeScript(0, 0);
        }
        _threadSwitch();
        if (*(u8*)(lbl_803A1D40 + 0x3) == 2) {
            menuReleaseOffScreen(*(f32*)&lbl_8047B7C4 / (f32)fn_800D37CC());
        }
    } else {
        menuPokemonSub(a0, (u16)a3, a4);
    }
    *(u8*)(lbl_803A1D40 + 0x4) = 1;
    return *(u32*)(lbl_803A1D40 + 0x1C);
}
#pragma pop
#endif

/* menuPokemonOpenItemGive - 0x8001BCEC | size: 0x50 */
#if 0
asm void menuPokemonOpenItemGive(void) {
#include "src/game/gs_pcbox_fn_8001BCEC.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void menuPokemonOpenItemGive(u32 a, u32 b, u32 c, u32 d) {
    extern u8 lbl_803A1D40[];
    extern void menuPokemonOpenSub(u32, u32, u32, u32, u32, u32);
    *(u8*)(lbl_803A1D40 + 0x4) = 0x1;
    menuPokemonOpenSub(0x5, a, b, c, d, 0x1);
}
#pragma pop
#endif

/* menuPokemonOpenItemUse - 0x8001BD3C | size: 0x44 */
#if 0
asm void menuPokemonOpenItemUse(void) {
#include "src/game/gs_pcbox_fn_8001BD3C.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void menuPokemonOpenItemUse(u32 a, u32 b, u32 c, u32 d) {
    extern u8 lbl_803A1D40[];
    extern void menuPokemonOpenSub();
    *(u8*)(lbl_803A1D40 + 0x4) = 0x1;
    menuPokemonOpenSub(a, 0x0, 0x0, c, d, b);
}
#pragma pop
#endif

/* menuPokemonOpenFight - 0x8001BD80 | size: 0x74 */
#if 0
asm void menuPokemonOpenFight(void) {
#include "src/game/gs_pcbox_fn_8001BD80.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void menuPokemonOpenFight(u8 a, u8 b, u32 c, u32 d) {
    extern u8 lbl_803A1D40[];
    extern void menuPokemonOpenSub(u32, u32, u32, u32, u32, u32);
    if (a == 0x1) {
        *(u8*)(lbl_803A1D40 + 0x4) = 0x0;
    } else {
        *(u8*)(lbl_803A1D40 + 0x4) = 0x1;
    }
    *(u32*)(lbl_803A1D40 + 0x18) = d;
    menuPokemonOpenSub(0x2, 0x0, 0x0, b, c, 0x1);
}
#pragma pop
#endif

/* menuPokemonOpen - 0x8001BDF4 | size: 0x44 */
#if 0
asm void menuPokemonOpen(void) {
#include "src/game/gs_pcbox_fn_8001BDF4.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void menuPokemonOpen(u32 a, u32 b, u32 c) {
    extern u8 lbl_803A1D40[];
    extern void menuPokemonOpenSub(u32, u32, u32, u32, u32, u32);
    *(u8*)(lbl_803A1D40 + 0x4) = 0x1;
    menuPokemonOpenSub(a, 0x0, 0x0, b, c, 0x1);
}
#pragma pop
#endif

/* menuPokemonMain - 0x8001BE38 | size: 0x84 */
#if 0
asm void menuPokemonMain(void) {
#include "src/game/gs_pcbox_fn_8001BE38.inc"
}
#else
#pragma push
#pragma scheduling off
#pragma optimization_level 4
s32 menuPokemonMain(void) {
    extern u8 lbl_803A1D40[];
    extern void menuPokemonSub();
    extern void menuCloseCustom();
    extern void fn_800FF660();
    extern void floorSetFadeScript();
    extern void _threadSwitch();
    u8 r3;
    u16 r4;
    u32 r5;
    r3 = *(u8*)(lbl_803A1D40 + 0x0);
    r4 = *(u16*)(lbl_803A1D40 + 0x12);
    r5 = *(u32*)(lbl_803A1D40 + 0xC);
    menuPokemonSub((u32)r3, (u32)r4, r5);
    menuCloseCustom(0x63, 0x0, 0x1);
    if (*(u8*)(lbl_803A1D40 + 0x2) != 0) {
        fn_800FF660();
        if (*(u8*)(lbl_803A1D40 + 0x3) != 0x1) floorSetFadeScript(0x0, 0x0);
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
    extern void savedataGetStatus();
    extern void fn_8006AEEC();
    extern void fightFloorGetGcHeroFightTrainerPtr();
    extern void fightTrainerGetStatus();
    extern void heroBiosGetPokemonPtr();
    extern void fightTrainerGetValidFightPokemonPtr();
    extern void pokemonGetStatus();
    extern void pokemonCheckValid();
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
        savedataGetStatus();
        goto L_8001BF98;
    }
    fn_8006AEEC();
    goto L_8001BF98;
    L_8001BF60:
    if (r4 != 0) goto L_8001BF70;
    r3 = 0x0;
    fightFloorGetGcHeroFightTrainerPtr();
    L_8001BF70:
    if (r3 == 0) {
        r3 = 0x0;
        goto L_8001BF98;
    }
    r4 = 0x0;
    r5 = 0x44;
    r6 = 0x0;
    fightTrainerGetStatus();
    goto L_8001BF98;
    L_8001BF94:
    r3 = 0x0;
    L_8001BF98:
    if (r3 != 0) {
        r4 = r29;
        heroBiosGetPokemonPtr();
        r30 = r3;
        goto L_8001C020;
    }
    r30 = 0x0;
    goto L_8001C038;
    L_8001BFB8:
    if ((u32)r29 >= 0x6) goto L_8001C038;
    if (r4 != 0) goto L_8001BFD4;
    r3 = 0x0;
    fightFloorGetGcHeroFightTrainerPtr();
    L_8001BFD4:
    if (r3 == 0) {
        r30 = 0x0;
        goto L_8001C038;
    }
    r4 = r29;
    fightTrainerGetValidFightPokemonPtr();
    if (r3 == 0) {
        r30 = 0x0;
        goto L_8001C038;
    }
    r4 = 0x0;
    r5 = 0xcc;
    r6 = 0x0;
    pokemonGetStatus();
    r30 = r3;
    goto L_8001C020;
    L_8001C014:
    if ((u32)r29 < 0x1e) goto L_8001C020;
    goto L_8001C038;
    L_8001C020:
    r3 = r30;
    pokemonCheckValid();
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
    extern u32 pokemonGetStatus();
    extern u32 pokemonGetJoutaiMenuSpriteId();
    u32 r3 = pokemonGetStatus(a, 0x0, 0x7b, 0x0);
    if ((u8)r3 == 0x1) {
        r3 = 0x1;
    } else {
        r3 = pokemonGetJoutaiMenuSpriteId(a);
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
