/**
 * @file menu_middle.c
 * @brief Menu middle code between battle and common (0x80069C0C-0x8007109C)
 *
 * Address range: 0x80069C0C - 0x8007109C
 * Total functions: 100
 */

#include "dolphin/types.h"
#include "game/menu/menu_middle.h"

#if !defined(MENU_MIDDLE_RESIDUAL_80069C0C_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006A65C_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006A824_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006A990_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006AC28_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006ACCC_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006ADB4_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006AE18_ONLY)
#define MENU_MIDDLE_ALL
#endif

/* ===== External function declarations ===== */
extern void menuSubGetPokemonSexForDisp();
extern void menuSeqBiosGetPtr();
extern void menuSpriteBiosGetPtr();
extern void menuItemBiosSetSelectFlag();
extern void fn_80071160();
extern void fn_80071208();
extern void fn_80071318();
extern void fn_8007162C();
extern void menuCB_InitMenu();
extern void menuCBRule_CheckPokemonErrorAll();
extern void fn_80076398();
extern void fn_800767B8();
extern void fn_80076A8C();
extern void fn_80076F2C();
extern void fn_800772AC();
extern void fn_800774D4();
extern void fn_80077A5C();
extern u8 fn_80077BD0(void);
extern void menuCBRule_CheckValidItem();
extern void fn_80077C68();
extern void fn_80077D88();
extern void fn_80077DB8();
extern void menuCBRule_ConstantRule();
extern void fn_80088EA8();
extern void sprintf();
extern void fmod();
extern void fn_800D5648();
extern void fn_800D5BA0();
extern void fn_800D61E4();
extern void fn_800D6728();
extern void fn_800D67BC();
extern void fn_800D6A00();
extern void fn_800D7820();
extern void fn_800D888C();
extern void fn_800D88DC();
extern void fn_800E0C54();
extern void _threadSwitch();
extern void GScharMakeFromSJIS();
extern u32 GSmsgGetGSchar();
extern void GSmsgGetRect();
extern void fn_800FB680();
extern void fn_800FE35C();
extern void fn_800FE38C();
extern void fn_800FF730();
extern s32 menuGetCursorFromItemID(s32 id, s32 itemId);
extern s32 menuGetCursorItemID(s32 id);
extern void menuSetCursor(s32 id, s32 cursor);
extern void menuSetPosition();
extern void menuButtonNormal();
extern void menuCursorNormal(void* menu);
extern void* windowGetParam(void* menu, s32 idx);
extern void fn_801044D0(s32 id, void* arg);
extern void windowGetActiveID();
extern void windowSearchItemID();
extern void windowSearchID();
extern void windowCreateCursorSprite();
extern void* windowGetKeyInfo(void);
extern void fn_80107F38();
extern void fn_801081F8();
extern void winSetSequence();
/* ... and 50 more external functions */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_80478938;
extern u8 lbl_80478F20;
extern u8 lbl_8047A5A4;
extern u8 lbl_8047A5D8;
extern u8 lbl_8047A5E0;
extern u8 lbl_8047A5E8;
extern u8 lbl_8047A5EC;
extern u8 lbl_8047A5F0;
extern u8 lbl_8047A5F4;
extern u8 lbl_8047A5F8;
extern u8 lbl_8047A5FC;
extern u8 lbl_8047C028;
extern u8 lbl_8047C030;
extern u8 lbl_8047C038;
extern u8 lbl_8047C040;
extern u8 lbl_8047C048;
extern u8 lbl_8047C050;
extern u8 lbl_8047C058;
extern u8 lbl_8047C060;
extern u8 lbl_8047C064;
extern u8 lbl_8047C068;
extern u8 lbl_8047C070;
extern u8 lbl_8047C078;
extern u8 lbl_8047C080;
extern u8 lbl_8047C088;
extern u8 lbl_8047C08C;
extern u8 lbl_8047E708;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EDE78[];
extern u8 jumptable_802EDEFC[];
extern u8 jumptable_802EDF20[];
extern u8 jumptable_802EDFB0[];
extern u8 jumptable_802EDFCC[];
extern u8 jumptable_802EE06C[];
extern u8 jumptable_802EE0F0[];
extern u8 jumptable_802EE20C[];
extern u8 jumptable_802EE31C[];
extern u8 lbl_80267C18[];
extern u8 lbl_80267DD8[];
extern u8 lbl_80267DE8[];
extern u8 lbl_80267E70[];
extern u8 lbl_80267EA8[];
extern u8 lbl_80267F68[];
extern u8 lbl_80267FE8[];
extern u8 lbl_80268184[];
extern u8 lbl_802681B4[];
extern u8 lbl_80268234[];
extern u8 lbl_80268424[];
extern u8 lbl_80268560[];
extern u8 lbl_80268574[];
extern u8 lbl_8026858C[];
extern u8 lbl_8026860C[];
extern u8 lbl_8026864C[];
extern u8 lbl_80268674[];
extern u8 lbl_80268680[];
extern u8 lbl_802686D0[];
extern u8 lbl_802EDE58[];
extern u8 lbl_802EE618[];
extern u8 lbl_80314E08[];
extern u8 lbl_803B6D68[];

/* ===== Forward declarations ===== */
void fn_80069C0C(void* arg0);
u16 fn_8006A65C(void);
u16 fn_8006A718(s32 idx);
u8 fn_8006A76C(void);
void fn_8006A79C(u8* p);
void fn_8006A7AC(u8* p);
u8 fn_8006A7BC(u8* p);
u32 fn_8006A7C8(u32 r3);
u16 fn_8006A7D0(u32 r3);
u16 fn_8006A7D8(u32 r3);
void fn_8006A7E0(u32 r3, u32 r4);
u32 fn_8006A7E8(u32 r3);
void fn_8006A7F0(void* dst, const void* src);
u32 fn_8006A814(u32 r3);
void fn_8006A81C(u32 r3, u32 r4);
void fn_8006A824(u32 r3, u32 r4);
void fn_8006A990(void);
void fn_8006AABC(void);
void menuCBBios_InitTrainer(void* p, u16 value);
s32 fn_8006AC6C(u32 id);
u8* fn_8006ACCC(s32 id);
void fn_8006ADB4(s32 value);
s32 fn_8006ADEC(void);
s32 fn_8006AE18(void);
u8* fn_8006AEEC(void);
void fn_8006AF44(u8* base, void* src);
u8* fn_8006AFC4(u8* p);
u8* fn_8006AFE4(s32 id);
u8* fn_8006B09C(s32 index);
u8* fn_8006B0F8(s32 index);
s32 menuCBBios_ControlerIDtoPortID(s32 id);
u32 fn_8006B1C0(s32 i);
void fn_8006B1D4(void);
u32 fn_8006B1F4(s32 index, s32 slot);
void fn_8006B2A4(s32 idx, s32 sub);
void fn_8006B354(s32 index);
u32 fn_8006B3C8(s32 index);
void* fn_8006B420(void);
void fn_8006B4AC(s32 value);
u8* fn_8006B51C(s32 index);
s32 fn_8006B57C(void);
s32 fn_8006B5A8(void);
void fn_8006B5D0(MenuMiddleWork* work);
void fn_8006B6B4(void);
u8 fn_8006B8E8(void);
void fn_8006B8F0(void);
void fn_8006B8FC(void);
void fn_8006B908(u32 r3);
void fn_8006B930(void* menu);
void fn_8006B9B8(void);
void fn_8006BB34(void* menu);
void fn_8006C018(void* menu);
void fn_8006C0DC(void* menu);
void fn_8006C164(void);
void fn_8006C5D8(void);
void fn_8006C7D4(void);
void fn_8006CCC0(void* arg0, void* arg1);
void fn_8006D550(void);
void fn_8006D940(void* menu);
void fn_8006D98C(void);
void fn_8006DAE4(void* arg0);
void fn_8006DC28(void);
void fn_8006E0CC(void);
u32 fn_8006E128(u8* p);
void fn_8006E160(u32 r3);
void fn_8006E188(void);
void fn_8006E18C(void* menu);
void fn_8006E258(void* menu);
void fn_8006E338(void);
void fn_8006E798(void);
void fn_8006E9A4(void);
void fn_8006EE7C(void* menu);
void fn_8006EF24(void* menu);
void fn_8006EFF8(void);
void fn_8006F284(void);
void fn_8006F720(void);
void fn_8006FBFC(void);
void fn_8006FCF8(u32 r3);
void fn_8006FD24(u32 r3);
void fn_8006FD4C(u32 r3);
void fn_8006FD74(u32 r3);
void fn_8006FD9C(u32 r3);
void fn_8006FDC4(u32 r3);
void fn_8006FDEC(u32 r3);
void fn_8006FE14(u32 r3);
void fn_8006FE3C(u32 r3);
void fn_8006FE64(void* menu);
void fn_8006FEE4(void);
void fn_80070274(u32 r3);
void fn_8007029C(u32 r3);
void fn_800702C8(u32 r3);
void fn_800702F0(u32 r3);
void fn_80070318(void);
void fn_80070428(void* arg0, void* menu);
void fn_800704A4(void);
void fn_800704A8(void);
void fn_800704AC(void);
void fn_800706C4(void);
void fn_80070A9C(void);
void fn_80070D84(void);

/* ===== Function implementations ===== */


#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_80069C0C_ONLY)
/* 0x80069C0C | size: 0xA50 */
void fn_80069C0C(void* arg0) {
    extern void fn_8006A7E0();
    extern void fn_8006A7F0();
    extern void fn_8006A81C();
    extern void fn_8006A990();
    extern void fn_8006AABC();
    extern void fn_8006AFC4();
    extern void fn_8006B1C0();
    extern void pokemonGrowDataBiosGetExp();
    extern void pokemonGrowDataBiosGetPtr();
    extern void pokemonBiosSetItemDataId();
    extern void pokemonBiosSetExp();
    extern void pokemonDataBiosGetGrowDataId();
    extern void pokemonDataBiosGetPtr();
    extern void pokemonBiosGetLevel();
    extern void pokemonBiosGetPokemonDataId();
    extern void pokemonCheckValid();
    extern void pokemonResetBasisStatus();
    extern void heroBiosSetNamePtr();
    extern void heroBiosGetPokemonPtr();
    extern void heroBiosCopy();
    extern void __assert();
    extern void fightTrainerCreateFightTrainerDataIdToHero();
    extern void fightEncountDataBiosSetSyoukaiWzxDataId();
    extern void fightEncountDataBiosSetBgmSndId();
    extern void fightEncountDataBiosSetGSInputDevice();
    extern void fightEncountDataBiosSetFightTrainerDataId();
    extern void fightEncountDataBiosSetFightFloorDataId();
    extern void fightEncountDataBiosSetTrainer();
    extern void fightEncountDataBiosSetFightKind();
    extern void fightEncountDataBiosGetPtr();
    u8 sp[0xB90];
    u32 r0;
    u32 r1 = (u32)sp;
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r25;
    u32 r26;
    u32 r27;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 r31;

    
    r31 = (u32)arg0;
    r0 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
    r3 = (u32)&lbl_80267C18;
    r29 = (u32)&lbl_80267C18;
    r25 = 0x0;
    if ((s32)r0 == (s32)0x1) {
        r25 = 0x20b;
    } else if ((s32)r0 < (s32)0x1) {
        if ((s32)r0 >= (s32)0x0) {
            r25 = 0x20a;
        }
    } else if ((s32)r0 < (s32)0x3) {
        r25 = 0x20c;
    }
    L_80069C68: ;
    r0 = r25 & 0xFFFF;
    if (r0 == (u32)0x0) {
        r3 = r29 + 0x7c;
        r5 = r29 + 0x8c;
        r4 = 0xf8;
        __assert();
    }
    r3 = r25;
    fightEncountDataBiosGetPtr();
    r0 = MENU_MIDDLE_U32_0000(r31)->unk_0000;
    r30 = r3;
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 >= (s32)0x1) goto L_80069D48;
        if ((s32)r0 < (s32)0x0) {
            goto L_80069D48;
        }
        r0 = MENU_MIDDLE_U32_0014(r31)->unk_0014;
        do {
            if ((s32)r0 == (s32)0x7 || (s32)r0 >= (s32)0x7) break;

            if ((s32)r0 < (s32)0x0) {
                break;
            }
            r4 = 0x1f;
            goto L_80069D7C;
        } while (0);
        r0 = MENU_MIDDLE_U32_000C(r31)->unk_000C;
        do {
            if ((s32)r0 == (s32)0x4) break;
            if ((s32)r0 < (s32)0x4) {
                if ((s32)r0 >= (s32)0x3) break;
                if ((s32)r0 >= (s32)0x0) break;
                break;
            }
            break;
        } while (0);

        r4 = 0x20;
        goto L_80069D7C;


    }
    r0 = MENU_MIDDLE_U32_0014(r31)->unk_0014;
    if (r0 < (u32)0x1e) {
        r4 = 0x1a;

    } else if (r0 < (u32)0x3c) {
        r4 = 0x17;

    } else if (r0 < (u32)0x63) {
        r4 = 0x18;

    } else {
        r4 = 0x3d5;
    }
    goto L_80069D7C;
    L_80069D48: ;
    r5 = *(u32*)&lbl_8047A5D8;
    r3 = (0xaaab << 16);
    /* subi r0, r3, 0x5555 */;
    r4 = r29 + 0x0;
    r3 = r5 + 0x1;
    r5 = r5 << 2;
    r0 = (u32)((u64)r0 * (u64)r3 >> 32);
    *(u32*)&lbl_8047A5D8 = r3;
    r4 = *(u32*)(r4 + r5);
    r0 = (u32)r0 >> 1;
    r0 = r0 * 0x3;
    r0 = r3 - r0;
    *(u32*)&lbl_8047A5D8 = r0;
    L_80069D7C: ;
    r3 = r30;
    fightEncountDataBiosSetBgmSndId();
    r0 = MENU_MIDDLE_U32_0000(r31)->unk_0000;
    do {
        if ((s32)r0 != (s32)0x1) break;
        r0 = MENU_MIDDLE_U32_000C(r31)->unk_000C;
        if ((s32)r0 != (s32)0x6) break;
        r0 = MENU_MIDDLE_U32_0014(r31)->unk_0014;
        if (r0 < (u32)0x1e) {
            r4 = 0x28;

        } else if (r0 < (u32)0x3c) {
            r4 = 0x29;

        } else if (r0 < (u32)0x63) {
            r4 = 0x2a;

        } else {
            r4 = 0x2e;
        }
        goto L_80069E08;
    } while (0);

    r0 = MENU_MIDDLE_U32_000C(r31)->unk_000C;
    if (r0 >= (u32)0x7) {
        r3 = r29 + 0x7c;
        r5 = r29 + 0xb8;
        r4 = 0x166;
        __assert();
    }
    r0 = MENU_MIDDLE_U32_000C(r31)->unk_000C;
    r3 = r29 + 0xc;
    r0 = r0 << 1;
    r0 = *(u16*)(r3 + r0);
    r4 = r0;
    L_80069E08: ;
    r3 = r30;
    fightEncountDataBiosSetFightFloorDataId();
    r0 = MENU_MIDDLE_U32_0000(r31)->unk_0000;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 != (s32)0x0) {
                if ((s32)r0 < (s32)0x0) {
                    goto L_80069EEC;
                }
                goto L_80069EEC;
                }
            r0 = MENU_MIDDLE_U32_0014(r31)->unk_0014;
            if (r0 >= (u32)0x8) {
                r3 = r29 + 0x7c;
                r5 = r29 + 0xe0;
                r4 = 0x17f;
                __assert();
            }
            r0 = MENU_MIDDLE_U32_0014(r31)->unk_0014;
            if (r0 < (u32)0x7) {
                r3 = r30;
                r4 = 0xd;
                fightEncountDataBiosSetFightKind();
            } else {

                r3 = r30;
                r4 = 0xe;
                fightEncountDataBiosSetFightKind();
            }
            r0 = MENU_MIDDLE_U32_0014(r31)->unk_0014;
            r4 = r29 + 0x1c;
            r3 = r30;
            r0 = r0 << 2;
            r4 = *(u32*)(r4 + r0);
            fightEncountDataBiosSetSyoukaiWzxDataId();
            goto L_80069F04;
                }
        r0 = MENU_MIDDLE_U32_0014(r31)->unk_0014;
        if (r0 < (u32)0x63) {
            r3 = r30;
            r4 = 0xf;
            fightEncountDataBiosSetFightKind();
        } else {

            r3 = r30;
            r4 = 0x12;
            fightEncountDataBiosSetFightKind();
        }
        r3 = r30;
        r4 = 0x0;
        fightEncountDataBiosSetSyoukaiWzxDataId();
        goto L_80069F04;
    }
    r3 = r30;
    r4 = 0x10;
    fightEncountDataBiosSetFightKind();
    r3 = r30;
    r4 = 0x0;
    fightEncountDataBiosSetSyoukaiWzxDataId();
    goto L_80069F04;
    L_80069EEC: ;
    r3 = r30;
    r4 = 0xc;
    fightEncountDataBiosSetFightKind();
    r3 = r30;
    r4 = 0x0;
    fightEncountDataBiosSetSyoukaiWzxDataId();
    L_80069F04: ;
    ((void(*)(void))fn_80077DB8)();
    if ((s32)r3 == (s32)0x6) {
        r0 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
        if ((s32)r0 != (s32)0x1) {
            if ((s32)r0 < (s32)0x1) {
                goto L_80069F54;
            }
            if ((s32)r0 >= (s32)0x3) goto L_80069F54;

        } else {
        r3 = r30;
        r4 = 0x1;
        fightEncountDataBiosSetTrainer();
        goto L_80069FFC;
        }
        r3 = r30;
        r4 = 0x2;
        fightEncountDataBiosSetTrainer();
        goto L_80069FFC;
        L_80069F54: ;
        r3 = r30;
        r4 = 0x0;
        fightEncountDataBiosSetTrainer();
        goto L_80069FFC;
    }
    r0 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 < (s32)0x1) {
            goto L_80069FD8;
        }
        if ((s32)r0 >= (s32)0x3) goto L_80069FD8;

    } else {
    if ((s32)r3 != (s32)0x4) {
        r3 = r29 + 0x7c;
        r5 = r29 + 0x108;
        r4 = 0x1c0;
        __assert();
    }
    r3 = r30;
    r4 = 0x5;
    fightEncountDataBiosSetTrainer();
    goto L_80069FFC;
    }
    if ((s32)r3 != (s32)0x2) {
        r3 = r29 + 0x7c;
        r5 = r29 + 0x114;
        r4 = 0x1c5;
        __assert();
    }
    r3 = r30;
    r4 = 0x6;
    fightEncountDataBiosSetTrainer();
    goto L_80069FFC;
    L_80069FD8: ;
    if ((s32)r3 != (s32)0x3) {
        r3 = r29 + 0x7c;
        r5 = r29 + 0x120;
        r4 = 0x1cb;
        __assert();
    }
    r3 = r30;
    r4 = 0x4;
    fightEncountDataBiosSetTrainer();
    L_80069FFC: ;
    r0 = MENU_MIDDLE_U32_0000(r31)->unk_0000;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 < (s32)0x0) {
                goto L_8006A5A0;
            }
            if ((s32)r0 < (s32)0x4) {
                r0 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
                if (r0 >= (u32)0x2) {
                    r3 = r29 + 0x7c;
                    r5 = r29 + 0x12c;
                    r4 = 0x221;
                    __assert();
                }
                r0 = MENU_MIDDLE_U32_0000(r31)->unk_0000;
                if (r0 >= (u32)0x4) {
                    r3 = r29 + 0x7c;
                    r5 = r29 + 0x150;
                    r4 = 0x222;
                    __assert();
                }
                r4 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
                r3 = r29 + 0x3c;
                r0 = MENU_MIDDLE_U32_0010(r31)->unk_0010;
                r4 = r4 << 5;
                r0 = r0 << 3;
                r27 = r4 + r0;
                r27 = r3 + r27;
                ((void(*)(void))fn_800E0C54)();
                r0 = r3 & 0xFFFF;
                r4 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
                r0 = r0 & 0x7;
                r3 = *(u8*)(r27 + r0);
                /* subi r5, r3, 0x1 */;
                if ((s32)r4 == (s32)0x0 || (s32)r4 == (s32)0x1) {


                if (r5 < (u32)0x64) {


                    r3 = r4 * 0x64;
                    r0 = *(u32*)&lbl_80478938;
                    r25 = r3 + r5;
                    r25 = r25 + 0x60;
                    if (r0 <= (u32)r25) {
                    r3 = r29 + 0x7c;
                    r5 = r29 + 0x174;
                    r4 = 0xca;
                    __assert();
                    }
                    r3 = (u32)&lbl_802EE618;
                    r0 = r25 << 1;
                    r3 = (u32)&lbl_802EE618;
                    r25 = *(u16*)(r3 + r0);
                }
            }
                r26 = r25 & 0xFFFF;
                do {
                    ((void(*)(void))fn_800E0C54)();
                    r0 = r3 & 0xFFFF;
                    r4 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
                    r0 = r0 & 0x7;
                    r3 = *(u8*)(r27 + r0);
                    /* subi r5, r3, 0x1 */;
                    if ((s32)r4 == (s32)0x0 || (s32)r4 == (s32)0x1) {


                    if (r5 < (u32)0x64) {


                        r3 = r4 * 0x64;
                        r0 = *(u32*)&lbl_80478938;
                        r28 = r3 + r5;
                        r28 = r28 + 0x60;
                        if (r0 <= (u32)r28) {
                        r3 = r29 + 0x7c;
                        r5 = r29 + 0x174;
                        r4 = 0xca;
                        __assert();
                        }
                        r3 = (u32)&lbl_802EE618;
                        r0 = r28 << 1;
                        r3 = (u32)&lbl_802EE618;
                        r28 = *(u16*)(r3 + r0);
                    }
            }
                    r0 = r28 & 0xFFFF;
                } while (r26 == (u32)r0);
                r4 = r25;
                r3 = r31 + 0x1684;
                fn_8006AABC();
                r3 = r31 + 0x1684;
                r4 = 0x0;
                fn_8006A81C();
                r25 = 0x0;
                while (1) {
                    r0 = r25 & 0xFFFF;
                    if (r0 >= (u32)0x6) break;
                    r4 = r25;
                    r3 = r31 + 0x16b0;
                    heroBiosGetPokemonPtr();
                    r4 = 0x0;
                    pokemonBiosSetItemDataId();
                    r25 = r25 + 0x1;


                }
                r25 = 0x0;
                while (1) {
                    r0 = r25 & 0xFFFF;
                    if (r0 >= (u32)0x6) break;
                    r4 = r25;
                    r3 = r31 + 0x21c8;
                    heroBiosGetPokemonPtr();
                    r4 = 0x0;
                    pokemonBiosSetItemDataId();
                    r25 = r25 + 0x1;


                }
                ((void(*)(void))fn_800E0C54)();
                r4 = (0xaaab << 16);
                r5 = r3 & 0xFFFF;
                /* subi r0, r4, 0x5555 */;
                r3 = (u32)&lbl_8047C028;
                r0 = (u32)((u64)r0 * (u64)r5 >> 32);
                r0 = (u32)r0 >> 1;
                r0 = r0 * 0x3;
                r0 = r5 - r0;
                r0 = r0 << 1;
                r3 = *(u16*)(r3 + r0);
                ((void(*)(void))GSmsgGetGSchar)();
                r4 = (u32)sp + 0x8;
                while (1) {
                    r0 = MENU_MIDDLE_U16_0000(r3)->unk_0000;
                    if (r0 == (u32)0x0) break;
                    MENU_MIDDLE_U16_0000(r4)->unk_0000 = r0;
                    r3 = r3 + 0x2;
                    r4 = r4 + 0x2;


                }
                r0 = 0x0;
                r3 = 0x0;
                MENU_MIDDLE_U16_0000(r4)->unk_0000 = r0;
                fn_8006B1C0();
                r4 = r3;
                r3 = r28;
                r5 = (u32)sp + 0x88;
                fightTrainerCreateFightTrainerDataIdToHero();
                r3 = (u32)sp + 0x88;
                r4 = (u32)sp + 0x8;
                heroBiosSetNamePtr();
                r25 = 0x0;
                while (1) {
                    r0 = r25 & 0xFFFF;
                    if (r0 >= (u32)0x6) break;
                    r4 = r25;
                    r3 = (u32)sp + 0x88;
                    heroBiosGetPokemonPtr();
                    r4 = 0x0;
                    pokemonBiosSetItemDataId();
                    r25 = r25 + 0x1;


                }
                r3 = r31 + 0x24;
                r4 = (u32)sp + 0x88;
                r5 = 0x1;
                fn_8006A990();
                r3 = 0x0;
                fn_8006B1C0();
                r4 = r3;
                r3 = r31 + 0x24;
                fn_8006A81C();
                r3 = r31 + 0x24;
                r4 = 0x0;
                fn_8006A7E0();
            }
            goto L_8006A5A0;
            }
        r0 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
        r4 = MENU_MIDDLE_U32_0014(r31)->unk_0014;
        r3 = MENU_MIDDLE_U32_000C(r31)->unk_000C;
        if ((s32)r0 == (s32)0x0 || (s32)r0 == (s32)0x1) {


        if ((s32)r3 != (s32)0x6) {
            if ((s32)r3 >= (s32)0x6) goto L_8006A33C;
            if ((s32)r3 < (s32)0x0) {
                goto L_8006A33C;
            }
            if (r4 < (u32)0x8) {


                r0 = r0 << 3;
                r3 = r3 << 4;
                r28 = r0 + r4;
                r28 = r3 + r28;
                goto L_8006A33C;
        }
        if (r4 < (u32)0x64) {


                r0 = r0 * 0x64;
                r28 = r0 + r4;
                r28 = r28 + 0x60;
                L_8006A33C: ;
                r0 = *(u32*)&lbl_80478938;
                if (r0 <= (u32)r28) {
                r3 = r29 + 0x7c;
                r5 = r29 + 0x174;
                r4 = 0xca;
                __assert();
                }
                r3 = (u32)&lbl_802EE618;
                r0 = r28 << 1;
                r3 = (u32)&lbl_802EE618;
                r4 = *(u16*)(r3 + r0);
        }
        }
        }
        r3 = r31 + 0x7008;
        fn_8006AABC();
        r3 = r31;
        fn_8006AFC4();
        r4 = r3;
        r3 = r31 + 0x59a8;
        fn_8006A7F0();
        r0 = MENU_MIDDLE_U32_0000(r31)->unk_0000;
        if ((s32)r0 == (s32)0x1) {
            r27 = 0x0;
            r26 = r27;
            do {
                r3 = r31 + 0x64ec;
                r4 = r26 & 0xFFFF;
                heroBiosGetPokemonPtr();
                r25 = r3;
                pokemonCheckValid();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x0) {
                    r3 = r25;
                    pokemonBiosGetLevel();
                    r0 = r3 & 0xFF;
                    if ((s32)r27 < (s32)r0) {
                        r3 = r25;
                        pokemonBiosGetLevel();
                        r27 = r3 & 0xFF;
                }
                }
                r26 = r26 + 0x1;
            } while ((s32)r26 < (s32)0x6);
            if (r27 > (u32)0x64) {
                r27 = 0x64;
            }
            r28 = r31 + 0x7b4c;
            r26 = 0x0;
            do {
                r3 = r28;
                r4 = r26 & 0xFFFF;
                heroBiosGetPokemonPtr();
                r25 = r3;
                pokemonCheckValid();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x0) {
                    r3 = r25;
                    pokemonBiosGetLevel();
                    r0 = r3 & 0xFF;
                    if (r27 > r0) {
                        r3 = r25;
                        pokemonBiosGetPokemonDataId();
                        pokemonDataBiosGetPtr();
                        pokemonDataBiosGetGrowDataId();
                        pokemonGrowDataBiosGetPtr();
                        r4 = r27 & 0xFF;
                        pokemonGrowDataBiosGetExp();
                        r4 = r3;
                        r3 = r25;
                        pokemonBiosSetExp();
                        r3 = r25;
                        pokemonResetBasisStatus();
                }
                }
                r26 = r26 + 0x1;
            } while ((s32)r26 < (s32)0x6);
            r4 = r28;
            r3 = r31 + 0x7034;
            heroBiosCopy();
        }
        r3 = r31 + 0x24;
        r4 = r31 + 0x59a8;
        fn_8006A7F0();
        r3 = r31 + 0x1684;
        r4 = r31 + 0x7008;
        fn_8006A7F0();
        r3 = 0x0;
        fn_8006B1C0();
        r4 = r3;
        r3 = r31 + 0x24;
        fn_8006A81C();
        r3 = r31 + 0x24;
        r4 = 0x0;
        fn_8006A7E0();
        r3 = 0x0;
        r0 = 0x1;
        MENU_MIDDLE_U16_59AA(r31)->unk_59AA = r3;
        MENU_MIDDLE_U16_0026(r31)->unk_0026 = r3;
        MENU_MIDDLE_U16_700A(r31)->unk_700A = r0;
        MENU_MIDDLE_U16_1686(r31)->unk_1686 = r0;
        goto L_8006A5A0;
    }
    r0 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 >= (s32)0x2) goto L_8006A590;
        if ((s32)r0 < (s32)0x0) {
            goto L_8006A590;
        }
        r0 = 0x0;
        r3 = 0x1;
        MENU_MIDDLE_U32_59D0(r31)->unk_59D0 = r0;
        r5 = r31 + (0x1 << 16);
        r6 = 0x2;
        r0 = 0x3;
        MENU_MIDDLE_U32_7030(r31)->unk_7030 = r3;
        r3 = r31 + 0x24;
        r4 = r31 + 0x59a8;
        MENU_MIDDLE_NEG_U32_8690(r5)->unk_8690 = r6;
        MENU_MIDDLE_NEG_U32_9CF0(r5)->unk_9CF0 = r0;
        fn_8006A7F0();
        r3 = r31 + 0x1684;
        r4 = r31 + 0x7008;
        fn_8006A7F0();
        goto L_8006A5A0;
    }
    r0 = MENU_MIDDLE_U32_59D0(r31)->unk_59D0;
    r3 = r31 + 0x24;
    r4 = r0 * 0x1660;
    r4 = r4 + 0x59a8;
    r4 = r31 + r4;
    fn_8006A7F0();
    r0 = MENU_MIDDLE_U32_7030(r31)->unk_7030;
    r3 = r31 + 0x1684;
    r4 = r0 * 0x1660;
    r4 = r4 + 0x59a8;
    r4 = r31 + r4;
    fn_8006A7F0();
    r4 = r31 + (0x1 << 16);
    r3 = r31 + 0x2ce4;
    r0 = MENU_MIDDLE_NEG_U32_8690(r4)->unk_8690;
    r4 = r0 * 0x1660;
    r4 = r4 + 0x59a8;
    r4 = r31 + r4;
    fn_8006A7F0();
    r4 = r31 + (0x1 << 16);
    r3 = r31 + 0x4344;
    r0 = MENU_MIDDLE_NEG_U32_9CF0(r4)->unk_9CF0;
    r4 = r0 * 0x1660;
    r4 = r4 + 0x59a8;
    r4 = r31 + r4;
    fn_8006A7F0();
    goto L_8006A5A0;
    L_8006A590: ;
    r3 = r29 + 0x7c;
    r4 = 0x291;
    r5 = (u32)&lbl_8047C030;
    __assert();
    L_8006A5A0: ;
    r0 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 >= (s32)0x2) goto L_8006A63C;
        if ((s32)r0 < (s32)0x0) {
            goto L_8006A63C;
        }
        r5 = MENU_MIDDLE_U16_2CE4(r31)->unk_2CE4;
        r3 = r30;
        r4 = 0x2;
        fightEncountDataBiosSetFightTrainerDataId();
        r5 = MENU_MIDDLE_U16_4344(r31)->unk_4344;
        r3 = r30;
        r4 = 0x3;
        fightEncountDataBiosSetFightTrainerDataId();
        r5 = MENU_MIDDLE_U32_2D08(r31)->unk_2D08;
        r3 = r30;
        r4 = 0x2;
        fightEncountDataBiosSetGSInputDevice();
        r5 = MENU_MIDDLE_U32_4368(r31)->unk_4368;
        r3 = r30;
        r4 = 0x3;
        fightEncountDataBiosSetGSInputDevice();
        }
    r5 = MENU_MIDDLE_U16_0024(r31)->unk_0024;
    r3 = r30;
    r4 = 0x0;
    fightEncountDataBiosSetFightTrainerDataId();
    r5 = MENU_MIDDLE_U16_1684(r31)->unk_1684;
    r3 = r30;
    r4 = 0x1;
    fightEncountDataBiosSetFightTrainerDataId();
    r5 = MENU_MIDDLE_U32_0048(r31)->unk_0048;
    r3 = r30;
    r4 = 0x0;
    fightEncountDataBiosSetGSInputDevice();
    r5 = MENU_MIDDLE_U32_16A8(r31)->unk_16A8;
    r3 = r30;
    r4 = 0x1;
    fightEncountDataBiosSetGSInputDevice();
    L_8006A63C: ;
    r0 = 0x1;
    r3 = 0x0;
    MENU_MIDDLE_U8_001C(r31)->unk_001C = r0;
    return;
}
#endif


#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006A65C_ONLY)
/* 0x8006A65C | size: 0xBC */
#pragma push
#pragma peephole off
u16 fn_8006A65C(void) {
    extern void* savedataGetStatus(int, int);
    extern void scriptSoundStop(int);
    extern void fn_80088EA8(void*);
    extern u32 fn_801906A0(int);
    extern void fn_80069C0C(void*);
    extern void fn_800FF730(int);
    extern void _threadSwitch(void);
    void* menuPtr;
    u32 x;

    menuPtr = (u8*)savedataGetStatus(0, 0xe) + 0xC9A8;
    scriptSoundStop(0x3e8);
    fn_80088EA8(menuPtr);
    x = fn_801906A0(0xb59);
    MENU_MIDDLE_U32_0014(savedataGetStatus(0, 0xe))->unk_0014 = x;
    MENU_MIDDLE_U32_000C(savedataGetStatus(0, 0xe))->unk_000C = 6;
    MENU_MIDDLE_U32_0000(savedataGetStatus(0, 0xe))->unk_0000 = 1;
    fn_80069C0C(savedataGetStatus(0, 0xe));
    fn_800FF730(0x397);
    _threadSwitch();
    return (u16) * (u32*)((u8*)savedataGetStatus(0, 0xe) + 0x20);
}
#pragma pop


/* 0x8006A718 | size: 0x54 */
#pragma push
#pragma scheduling off
u16 fn_8006A718(s32 idx) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    u8* p;
    u16 value;

    p = savedataGetStatus(idx, 0xE) + 0x10000;
    if (MENU_MIDDLE_NEG_U8_C988(p)->unk_C988 != 0) {
        p -= 0x4cd8;
    } else {
        p = 0;
    }
    if (p != 0) {
        value = MENU_MIDDLE_U16_0000(p)->unk_0000;
    } else {
        value = 0;
    }
    return value;
}
#pragma pop


/* 0x8006A76C | size: 0x30 */
#pragma push
#pragma peephole off
u8 fn_8006A76C(void) {
    extern u8 fn_801D04E8(void);
    return (u8)((fn_801D04E8() & 0xFF) == 0);
}
#pragma pop

/* 0x8006A79C | size: 0x10 */
void fn_8006A79C(u8* p) {
    p[0xC98B] = 0;
}

/* 0x8006A7AC | size: 0x10 */
void fn_8006A7AC(u8* p) {
    p[0xC98B] = 1;
}

/* 0x8006A7BC | size: 0xC */
u8 fn_8006A7BC(u8* p) {
    return p[0xC98B];
}

/* 0x8006A7C8 | size: 0x8 */
u32 fn_8006A7C8(u32 r3) {
    return r3 + 0xb44;
}

/* 0x8006A7D0 | size: 0x8 */
u16 fn_8006A7D0(u32 r3) {
    return MENU_MIDDLE_U16_0000(r3)->unk_0000;
}

/* 0x8006A7D8 | size: 0x8 */
u16 fn_8006A7D8(u32 r3) {
    return MENU_MIDDLE_U16_0002(r3)->unk_0002;
}

/* 0x8006A7E0 | size: 0x8 */
void fn_8006A7E0(u32 r3, u32 r4) {
    MENU_MIDDLE_U32_0004(r3)->unk_0004 = r4;
}

/* 0x8006A7E8 | size: 0x8 */
u32 fn_8006A7E8(u32 r3) {
    return MENU_MIDDLE_U32_0004(r3)->unk_0004;
}

/* 0x8006A7F0 | size: 0x24 */
#pragma push
#pragma scheduling off
void fn_8006A7F0(void* dst, const void* src) {
    memcpy(dst, src, 0x1660);
}
#pragma pop

/* 0x8006A814 | size: 0x8 */
u32 fn_8006A814(u32 r3) {
    return MENU_MIDDLE_U32_0024(r3)->unk_0024;
}

/* 0x8006A81C | size: 0x8 */
void fn_8006A81C(u32 r3, u32 r4) {
    MENU_MIDDLE_U32_0024(r3)->unk_0024 = r4;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006A824_ONLY)
/* 0x8006A824 | size: 0x16C */
#pragma push
#pragma peephole off
void fn_8006A824(u32 r28, u32 r29) {
    extern void fn_8006A990();
    extern u8 heroBiosGetHomePlace(u32);
    extern u8 heroBiosGetSexDataId(u32);
    extern void __assert(u32, u32, u32);
    u32 r0 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r31 = (u32)&lbl_80267DD8;
    r30 = 0x1;
    r0 = heroBiosGetHomePlace(r29) & 0xFF;
    switch ((s32)r0) {
        case 0:
            r30 = 0x1;
            r0 = heroBiosGetSexDataId(r29) & 0xFF;
            if (r0 != (u32)0x0) {
                __assert(r31 + 0x10, 0x281, r31 + 0x20);
            }
            break;
        case 1:
            r0 = heroBiosGetSexDataId(r29) & 0xFF;
            switch ((s32)r0) {
                case 0: r30 = 0x2; break;
                case 1: r30 = 0x3; break;
                case 2:
                default:
                    __assert(r31 + 0x10, 0x28a, r31 + 0x4c);
                    break;
            }
            break;
        case 2:
            r0 = heroBiosGetSexDataId(r29) & 0xFF;
            switch ((s32)r0) {
                case 0: r30 = 0x309; break;
                case 1: r30 = 0x308; break;
                case 2:
                default:
                    __assert(r31 + 0x10, 0x294, r31 + 0x4c);
                    break;
            }
            break;
        default:
            __assert(r31 + 0x10, 0x299, r31 + 0x60);
            break;
    }
    fn_8006A990(r28, r29, r30);
    return;
}
#pragma pop
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006A990_ONLY)
/* 0x8006A990 | size: 0x12C */
void fn_8006A990(void) {
    extern void pokemonAllKaihuku();
    extern void heroBiosSetHizukiFlag();
    extern void heroBiosGetPokemonPtr();
    extern void heroBiosCopy();
    u8 sp[0xB30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r28 = r3;
    r29 = r5;
    r3 = (u32)sp + 0x8;
    heroBiosCopy();
    r3 = (u32)sp + 0x8;
    r4 = 0x0;
    heroBiosSetHizukiFlag();
    r30 = 0x0;
    while (1) {
        r0 = r30 & 0xFFFF;
        if (r0 >= (u32)0x6) break;
        r4 = r30;
        r3 = (u32)sp + 0x8;
        heroBiosGetPokemonPtr();
        r31 = r3;
        ((void(*)(void))fn_80077A5C)();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x0) {
            r3 = r31;
            pokemonAllKaihuku();
        }
        r30 = r30 + 0x1;


    }
    r31 = MENU_MIDDLE_U16_0002(r28)->unk_0002;
    r3 = r28;
    r4 = 0x0;
    r5 = 0x1660;
    memset((void*)r3, (int)r4, (u32)r5);
    MENU_MIDDLE_U16_0002(r28)->unk_0002 = r31;
    r3 = r28 + 0x2c;
    r4 = (u32)sp + 0x8;
    heroBiosCopy();
    r3 = r28 + 0xb44;
    r4 = (u32)sp + 0x8;
    heroBiosCopy();
    MENU_MIDDLE_U16_0000(r28)->unk_0000 = r29;
    r4 = r29 & 0xFFFF;
    r3 = *(u32*)&lbl_80478F20;
    r0 = MENU_MIDDLE_U32_0000(r3)->unk_0000;
    if (r0 <= (u32)r4) {
        r0 = -0x1;
    } else if ((s32)r4 >= (s32)0x9) {
        r0 = 0x2;
    } else if ((s32)r4 == (s32)0x1 || (s32)r4 < (s32)0x1 || (s32)r4 >= (s32)0x30a || (s32)r4 < (s32)0x308) {
        r0 = 0x0;
    } else {
        r0 = 0x1;
    }
    MENU_MIDDLE_U32_0004(r28)->unk_0004 = r0;
    return;
}


/* 0x8006AABC | size: 0x16C */
void fn_8006AABC(void) {
    extern void pokemonAllKaihuku();
    extern void heroCheckValid();
    extern void heroBiosSetHizukiFlag();
    extern void heroBiosGetPokemonPtr();
    extern void heroBiosCopy();
    extern void __assert();
    extern void fightTrainerCreateFightTrainerDataIdToHero();
    u8 sp[0xB30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r28 = r3;
    r29 = r4;
    r4 = (u32)&lbl_80267DD8;
    r3 = (u32)sp + 0x8;
    r31 = (u32)&lbl_80267DD8;
    r4 = r28 + 0xb44;
    heroBiosCopy();
    r4 = MENU_MIDDLE_U32_0000(r31)->unk_0000;
    r3 = r29;
    r5 = (u32)sp + 0x8;
    fightTrainerCreateFightTrainerDataIdToHero();
    r3 = (u32)sp + 0x8;
    heroCheckValid();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        r3 = r31 + 0x10;
        r5 = r31 + 0x7c;
        r4 = 0x258;
        __assert();
    }
    r3 = (u32)sp + 0x8;
    r4 = 0x0;
    heroBiosSetHizukiFlag();
    r30 = 0x0;
    while (1) {
        r0 = r30 & 0xFFFF;
        if (r0 >= (u32)0x6) break;
        r4 = r30;
        r3 = (u32)sp + 0x8;
        heroBiosGetPokemonPtr();
        r31 = r3;
        ((void(*)(void))fn_80077A5C)();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x0) {
            r3 = r31;
            pokemonAllKaihuku();
        }
        r30 = r30 + 0x1;


    }
    r31 = MENU_MIDDLE_U16_0002(r28)->unk_0002;
    r3 = r28;
    r4 = 0x0;
    r5 = 0x1660;
    memset((void*)r3, (int)r4, (u32)r5);
    MENU_MIDDLE_U16_0002(r28)->unk_0002 = r31;
    r3 = r28 + 0x2c;
    r4 = (u32)sp + 0x8;
    heroBiosCopy();
    r3 = r28 + 0xb44;
    r4 = (u32)sp + 0x8;
    heroBiosCopy();
    MENU_MIDDLE_U16_0000(r28)->unk_0000 = r29;
    r4 = r29 & 0xFFFF;
    r3 = *(u32*)&lbl_80478F20;
    r0 = MENU_MIDDLE_U32_0000(r3)->unk_0000;
    if (r0 <= (u32)r4) {
        r0 = -0x1;
    } else if ((s32)r4 >= (s32)0x9) {
        r0 = 0x2;
    } else if ((s32)r4 == (s32)0x1 || (s32)r4 < (s32)0x1 || (s32)r4 >= (s32)0x30a || (s32)r4 < (s32)0x308) {
        r0 = 0x0;
    } else {
        r0 = 0x1;
    }
    MENU_MIDDLE_U32_0004(r28)->unk_0004 = r0;
    return;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006AC28_ONLY)
/* 0x8006AC28 | size: 0x44 */
#pragma push
#pragma peephole off
void menuCBBios_InitTrainer(void* p, u16 value) {
    memset(p, 0, 0x1660);
    MENU_MIDDLE_U16_0002(p)->unk_0002 = value;
}
#pragma pop


/* 0x8006AC6C | size: 0x60 */
s32 fn_8006AC6C(u32 r3) {
    u32 r0;
    u32 r4;

    r4 = *(u32*)&lbl_80478F20;
    r3 = r3 & 0xffff;
    r0 = MENU_MIDDLE_U32_0000(r4)->unk_0000;
    if (r0 <= r3) {
        return -1;
    }
    if ((s32)r3 < 9) {
        if ((s32)r3 == 1) {
            goto ret0;
        }
        if ((s32)r3 >= 1) {
            goto ret1;
        }
        goto ret0;
    }
    if ((s32)r3 >= 0x30a) {
        goto ret0;
    }
    if ((s32)r3 >= 0x308) {
        goto ret2;
    }
    goto ret0;
ret1:
    return 1;
ret2:
    return 2;
ret0:
    return 0;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006ACCC_ONLY)
/* 0x8006ACCC | size: 0xE8 */
u8* fn_8006ACCC(s32 id) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    s32 ruleType;
    u32 offset;
    s32 i;
    u8* status;

    ruleType = MENU_MIDDLE_U32_0004(savedataGetStatus(0, 0xE))->unk_0004;
    if (ruleType == 2 || ruleType >= 2) {
        goto search;
    }
    if (ruleType < 0) {
        goto search;
    }

    if (id < 0) {
        return 0;
    }
    if (id > 1) {
        return 0;
    }
    status = savedataGetStatus(0, 0xE);
    return status + id * 0x1660 + 0x24;

search:
    i = 0;
    offset = 0;
    do {
        s32 trainerId = *(s32*)(savedataGetStatus(0, 0xE) + offset + 0x4c);
        if (id == trainerId) {
            status = savedataGetStatus(0, 0xE);
            return status + i * 0x1660 + 0x24;
        }
        i++;
        offset += 0x1660;
    } while (i < 4);

    return 0;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006ADB4_ONLY)
/* 0x8006ADB4 | size: 0x38 */
#pragma push
#pragma peephole off
void fn_8006ADB4(s32 value) {
    extern u8* savedataGetStatus(s32 idx, s32 type);

    *(s32*)(savedataGetStatus(0, 0xe) + 0x59a4) = value;
}
#pragma pop


/* 0x8006ADEC | size: 0x2C */
#pragma push
#pragma scheduling off
s32 fn_8006ADEC(void) {
    extern u8 *savedataGetStatus(s32 idx, s32 type);
    return *(s32*)(savedataGetStatus(0x0, 0xe) + 0x59a4);
}
#pragma pop

#endif
#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006AE18_ONLY)
/* 0x8006AE18 | size: 0xD4 */
#pragma push
#pragma scheduling off
s32 fn_8006AE18(void) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    extern u32 fn_801906A0(u32 flag);
    extern void __assert(const char* file, s32 line, const char* expr);
    u8* p;
    s32 state;

    if (fn_801906A0(0x8AE) == 0) {
        goto ret_zero;
    }

    p = savedataGetStatus(0, 0xE) + 0x10000;
    if (MENU_MIDDLE_NEG_U8_C988(p)->unk_C988 != 0) {
        p -= 0x4cd8;
    } else {
        p = 0;
    }

    if (p == 0) {
        goto ret_zero;
    }

    state = MENU_MIDDLE_U16_0000(p)->unk_0000;
    switch (state) {
    case 1:
        return 0;
    case 2:
        return 1;
    case 3:
        return 2;
    case 0x309:
        return 3;
    case 0x308:
        return 4;
    default:
        __assert((const char*)&lbl_80267DE8, 0x1c2, (const char*)&lbl_8047C040);
        break;
    }

ret_zero:
    return 0;
}
#pragma pop


/* 0x8006AEEC | size: 0x58 */
#pragma push
#pragma scheduling off
u8* fn_8006AEEC(void) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    u8* p;

    p = savedataGetStatus(0, 0xe) + 0x10000;
    if (*(u8*)(p - 0x3678) != 0) {
        p -= 0x4cd8;
    } else {
        p = NULL;
    }
    if (p == NULL) {
        return NULL;
    }
    return p + 0xb44;
}
#pragma pop


/* 0x8006AF44 | size: 0x80 */
void fn_8006AF44(u8* base, void* src) {
    if (src != 0) {
        memcpy(base + 0x10000 - 0x4cd8, src, 0x1660);
        MENU_MIDDLE_NEG_U32_B34C(base + 0x10000)->unk_B34C = MENU_MIDDLE_U32_0000(&lbl_80267DD8)->unk_0000;
        MENU_MIDDLE_NEG_U16_B32A(base + 0x10000)->unk_B32A = 0;
        MENU_MIDDLE_NEG_U8_C988(base + 0x10000)->unk_C988 = 1;
    } else {
        MENU_MIDDLE_NEG_U8_C988(base + 0x10000)->unk_C988 = 0;
    }
    MENU_MIDDLE_NEG_U8_C98B(base + 0x10000)->unk_C98B = 0;
}


/* 0x8006AFC4 | size: 0x20 */
u8* fn_8006AFC4(u8* p) {
    p += 0x10000;
    if (*(u8*)(p - 0x3678) != 0) {
        return p - 0x4cd8;
    }
    return 0;
}

/* 0x8006AFE4 | size: 0xB8 */
#pragma peephole off
u8* fn_8006AFE4(s32 id) {
    extern u8* savedataGetStatus(s32 side, s32 type);
    s32 index;
    s32* cursor = (s32*)lbl_80267DD8;
    u32 status;
    u32 offset;

    if (id == *cursor++) {
        index = 0;
    } else if (id == *cursor++) {
        index = 1;
    } else if (id == *cursor++) {
        index = 2;
    } else if (id == *cursor) {
        index = 3;
    } else {
        index = -1;
    }

    if (index < 0) {
        return NULL;
    }
    status = (u32)savedataGetStatus(0, 0xe);
    offset = index * 0x1660;
    return (u8*)(status + offset + 0x24);
}
#pragma peephole reset


/* 0x8006B09C | size: 0x5C */
#pragma push
#pragma peephole off
u8* fn_8006B09C(s32 index) {
    extern u8* savedataGetStatus(s32 idx, s32 type);

    if (index < 0 || index >= 4) {
        return 0;
    }

    return savedataGetStatus(0, 0xE) + index * 0x1660 + 0x24;
}
#pragma pop


/* 0x8006B0F8 | size: 0x5C */
#pragma push
#pragma peephole off
u8* fn_8006B0F8(s32 index) {
    extern u8* savedataGetStatus(s32 idx, s32 type);

    if (index < 0 || (u32)index >= 4) {
        return 0;
    }

    return savedataGetStatus(0, 0xE) + index * 0x1660 + 0x50;
}
#pragma pop


/* 0x8006B154 | size: 0x6C */
#pragma push
#pragma peephole off
s32 menuCBBios_ControlerIDtoPortID(s32 id) {
    s32* cursor = (s32*)lbl_80267DD8;

    if (id == *cursor) {
        return 0;
    }
    cursor++;
    if (id == *cursor) {
        return 1;
    }
    cursor++;
    if (id == *cursor) {
        return 2;
    }
    cursor++;
    if (id == *cursor) {
        return 3;
    }
    return -1;
}
#pragma pop


/* 0x8006B1C0 | size: 0x14 */
u32 fn_8006B1C0(s32 i) {
    return ((u32*)lbl_80267DD8)[i];
}

/* 0x8006B1D4 | size: 0x20 */
void fn_8006B1D4(void) {
    fn_80077DB8();
}

/* 0x8006B1F4 | size: 0xB0 */
#pragma push
#pragma peephole off
u32 fn_8006B1F4(s32 index, s32 slot) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    s32 r30;
    s32 r31;
    u32 r0;
    u8* r3;

    r30 = index;
    r31 = slot;
    if (r30 < 0) {
        goto invalid_index;
    }
    if (r30 < 7) {
        goto valid_index;
    }
invalid_index:
    r0 = 0;
    goto check_enabled;
valid_index:
    r3 = savedataGetStatus(0, 0xe);
    r0 = *(u8*)(r3 + (r30 + (1 << 16)) - 0x342c);
check_enabled:
    r0 = (u8)r0;
    if (r0 != 0) {
        goto enabled;
    }
    return 0;
enabled:
    if (r31 < 0) {
        goto ret0;
    }
    if ((u32)r31 < 2) {
        goto valid_slot;
    }
ret0:
    return 0;
valid_slot:
    r3 = savedataGetStatus(0, 0xe);
    return *(u8*)(r3 + (r31 + (1 << 16)) + r30 * 2 - 0x3425);
}
#pragma pop


/* 0x8006B2A4 | size: 0xB0 */
#pragma push
#pragma peephole off
void fn_8006B2A4(s32 idx, s32 sub) {
    extern u8* savedataGetStatus(s32 side, s32 type);
    u8 flag;

    if (idx >= 0 && idx < 7) {
        flag = MENU_MIDDLE_NEG_U8_CBD4(savedataGetStatus(0, 0xE) + idx + 0x10000)->unk_CBD4;
    } else {
        flag = 0;
    }

    if (flag == 0) {
        return;
    }
    if (sub < 0 || (u32)sub >= 2) {
        return;
    }

    MENU_MIDDLE_NEG_U8_CBDB(savedataGetStatus(0, 0xE) + sub + 0x10000 + idx * 2)->unk_CBDB = 1;
}
#pragma pop


/* 0x8006B354 | size: 0x74 */
#pragma push
#pragma peephole off
void fn_8006B354(s32 index) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    extern void __assert(char* file, s32 line, char* expr);
    s32 r30;
    u32 r31;

    r30 = index;
    if (r30 >= 0) {
        if (r30 < 7) {
            goto valid_index;
        }
    }
    __assert((char*)&lbl_80267DE8, 0xe4, (char*)&lbl_8047C040);
    return;
valid_index:
    r31 = 1;
    *(u8*)(savedataGetStatus(0, 0xe) + (r30 + (1 << 16)) - 0x342c) = r31;
}
#pragma pop


/* 0x8006B3C8 | size: 0x58 */
#pragma push
#pragma peephole off
u32 fn_8006B3C8(s32 index) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    s32 r31;

    r31 = index;
    if (r31 < 0) {
        goto ret0;
    }
    if (r31 < 7) {
        goto valid_index;
    }
ret0:
    return 0;
valid_index:
    return *(u8*)(savedataGetStatus(0, 0xe) + (r31 + (1 << 16)) - 0x342c);
}
#pragma pop


/* 0x8006B420 | size: 0x8C */
#pragma push
#pragma scheduling off
void* fn_8006B420(void) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    extern void* menuCBRule_ConstantRule(s32 index);
    s32 ruleId;
    s32 index;
    void* value;

    ruleId = MENU_MIDDLE_U32_0008(savedataGetStatus(0, 0xE))->unk_0008;
    value = menuCBRule_ConstantRule(ruleId);
    if (value != 0) {
        return value;
    }

    index = MENU_MIDDLE_U32_0008(savedataGetStatus(0, 0xE))->unk_0008;
    if (index < 0 || (u32)index >= 6) {
        return 0;
    }

    {
        u32 offset = index * 0x54;
        offset += 0x10000;
        offset -= 0x3624;
        return savedataGetStatus(0, 0xE) + offset;
    }
}
#pragma pop


/* 0x8006B4AC | size: 0x70 */
#pragma push
#pragma peephole off
void fn_8006B4AC(s32 value) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    extern void __assert(const char* file, s32 line, const char* expr);
    s32 valid;

    valid = 0;
    if (value >= 0 && (u32)value < 6) {
        valid = 1;
    }
    if (valid == 0) {
        __assert((const char*)&lbl_80267DE8, 0xB9, (const char*)&lbl_80267E70);
    }
    MENU_MIDDLE_U32_0008(savedataGetStatus(0, 0xE))->unk_0008 = value;
}
#pragma pop


/* 0x8006B51C | size: 0x60 */
#pragma push
#pragma peephole off
u8* fn_8006B51C(s32 index) {
    extern u8* savedataGetStatus(s32 idx, s32 type);

    if (index < 0 || (u32)index >= 6) {
        return 0;
    }

    return savedataGetStatus(0, 0xE) + index * 0x54 + 0xC9DC;
}
#pragma pop


/* 0x8006B57C | size: 0x2C */
#pragma push
#pragma scheduling off
s32 fn_8006B57C(void) {
    extern u8 *savedataGetStatus(s32 idx, s32 type);
    return savedataGetStatus(0x0, 0xe)[0x1c];
}
#pragma pop

/* 0x8006B5A8 | size: 0x28 */
#pragma push
#pragma scheduling off
s32 fn_8006B5A8(void) {
    extern s32 savedataGetStatus(s32 idx, s32 type);
    return savedataGetStatus(0x0, 0xe);
}
#pragma pop

/* 0x8006B5D0 | size: 0xE4 */
void fn_8006B5D0(MenuMiddleWork* work) {
    extern void fn_8006AABC();
    extern void* savedataGetStatus(s32 index, s32 kind);
    u16* trainerIds;
    u8* slot;
    u32* trainerKinds;
    u32 i;

    *(void**)&lbl_8047A5A4 = savedataGetStatus(0, 0xE);
    if (lbl_8047A5E0 == 0) {
        slot = (u8*)work;
        trainerKinds = (u32*)lbl_80267DD8;
        i = 0;
        trainerIds = (u16*)&lbl_8047C038;
        do {
            u32 trainerKind;
            u32 controllerId;

            fn_8006AABC(slot + 0x24, *trainerIds);
            trainerKind = *trainerKinds;
            controllerId = i & 0xFFFF;
            *(u32*)(slot + 0x48) = trainerKind;
            *(u16*)(slot + 0x26) = controllerId;
            memcpy(slot + 0x59A8, slot + 0x24, 0x1660);
            trainerIds++;
            slot += 0x1660;
            trainerKinds++;
            i++;
        } while (i < 4);

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


/* 0x8006B6B4 | size: 0x234 */
void fn_8006B6B4(void) {
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r31 = r3;
    r0 = 0x0;
    r4 = (0x1 << 16);
    *(u8*)&lbl_8047A5E0 = r0;
    /* subi r5, r4, 0x33d4 */;
    r4 = 0x0;
    memset((void*)r3, (int)r4, (u32)r5);
    r0 = 0x0;
    r3 = 0x0;
    MENU_MIDDLE_U8_001C(r31)->unk_001C = r0;
    ((void(*)(void))menuCBRule_ConstantRule)();
    r4 = r31 + (0x1 << 16);
    r0 = 0xa;
    /* subi r5, r4, 0x3628 */;
    /* subi r4, r3, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = MENU_MIDDLE_U32_0004(r4)->unk_0004;
        r0 = MENU_MIDDLE_U32_0008(r4)->unk_0008;
        MENU_MIDDLE_U32_0004(r5)->unk_0004 = r3;
        r5 += 8; *(u32*)r5 = r0;
    } while (--ctr != 0);
    r0 = MENU_MIDDLE_U32_0004(r4)->unk_0004;
    r3 = 0x1;
    MENU_MIDDLE_U32_0004(r5)->unk_0004 = r0;
    ((void(*)(void))menuCBRule_ConstantRule)();
    r4 = r31 + (0x1 << 16);
    r0 = 0xa;
    /* subi r5, r4, 0x35d4 */;
    /* subi r4, r3, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = MENU_MIDDLE_U32_0004(r4)->unk_0004;
        r0 = MENU_MIDDLE_U32_0008(r4)->unk_0008;
        MENU_MIDDLE_U32_0004(r5)->unk_0004 = r3;
        r5 += 8; *(u32*)r5 = r0;
    } while (--ctr != 0);
    r0 = MENU_MIDDLE_U32_0004(r4)->unk_0004;
    r3 = 0x2;
    MENU_MIDDLE_U32_0004(r5)->unk_0004 = r0;
    ((void(*)(void))menuCBRule_ConstantRule)();
    r4 = r31 + (0x1 << 16);
    r0 = 0xa;
    /* subi r5, r4, 0x3580 */;
    /* subi r4, r3, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = MENU_MIDDLE_U32_0004(r4)->unk_0004;
        r0 = MENU_MIDDLE_U32_0008(r4)->unk_0008;
        MENU_MIDDLE_U32_0004(r5)->unk_0004 = r3;
        r5 += 8; *(u32*)r5 = r0;
    } while (--ctr != 0);
    r0 = MENU_MIDDLE_U32_0004(r4)->unk_0004;
    r3 = 0x0;
    MENU_MIDDLE_U32_0004(r5)->unk_0004 = r0;
    ((void(*)(void))menuCBRule_ConstantRule)();
    r4 = r31 + (0x1 << 16);
    r0 = 0xa;
    /* subi r5, r4, 0x352c */;
    /* subi r4, r3, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = MENU_MIDDLE_U32_0004(r4)->unk_0004;
        r0 = MENU_MIDDLE_U32_0008(r4)->unk_0008;
        MENU_MIDDLE_U32_0004(r5)->unk_0004 = r3;
        r5 += 8; *(u32*)r5 = r0;
    } while (--ctr != 0);
    r0 = MENU_MIDDLE_U32_0004(r4)->unk_0004;
    r3 = 0x0;
    MENU_MIDDLE_U32_0004(r5)->unk_0004 = r0;
    ((void(*)(void))menuCBRule_ConstantRule)();
    r4 = r31 + (0x1 << 16);
    r0 = 0xa;
    /* subi r5, r4, 0x34d8 */;
    /* subi r4, r3, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = MENU_MIDDLE_U32_0004(r4)->unk_0004;
        r0 = MENU_MIDDLE_U32_0008(r4)->unk_0008;
        MENU_MIDDLE_U32_0004(r5)->unk_0004 = r3;
        r5 += 8; *(u32*)r5 = r0;
    } while (--ctr != 0);
    r0 = MENU_MIDDLE_U32_0004(r4)->unk_0004;
    r3 = 0x0;
    MENU_MIDDLE_U32_0004(r5)->unk_0004 = r0;
    ((void(*)(void))menuCBRule_ConstantRule)();
    r4 = r31 + (0x1 << 16);
    r0 = 0xa;
    /* subi r6, r4, 0x3484 */;
    /* subi r4, r3, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = MENU_MIDDLE_U32_0004(r4)->unk_0004;
        r0 = MENU_MIDDLE_U32_0008(r4)->unk_0008;
        MENU_MIDDLE_U32_0004(r6)->unk_0004 = r3;
        r6 += 8; *(u32*)r6 = r0;
    } while (--ctr != 0);
    r0 = MENU_MIDDLE_U32_0004(r4)->unk_0004;
    r4 = r31 + (0x1 << 16);
    r5 = 0x6;
    r3 = 0x1;
    MENU_MIDDLE_U32_0004(r6)->unk_0004 = r0;
    r0 = 0x0;
    MENU_MIDDLE_NEG_U16_CB86(r4)->unk_CB86 = r5;
    MENU_MIDDLE_NEG_U16_CB32(r4)->unk_CB32 = r5;
    MENU_MIDDLE_NEG_U16_CADE(r4)->unk_CADE = r5;
    MENU_MIDDLE_NEG_U8_CBD4(r4)->unk_CBD4 = r3;
    MENU_MIDDLE_NEG_U8_CBD5(r4)->unk_CBD5 = r3;
    MENU_MIDDLE_NEG_U8_CBD6(r4)->unk_CBD6 = r3;
    MENU_MIDDLE_NEG_U8_CBD7(r4)->unk_CBD7 = r0;
    MENU_MIDDLE_NEG_U8_CBD8(r4)->unk_CBD8 = r3;
    MENU_MIDDLE_NEG_U8_CBD9(r4)->unk_CBD9 = r0;
    MENU_MIDDLE_NEG_U8_CBDB(r4)->unk_CBDB = r0;
    r5 = r31 + 0x2;
    r3 = r5 + (0x1 << 16);
    MENU_MIDDLE_NEG_U8_CBDC(r4)->unk_CBDC = r0;
    r5 = r5 + 0x2;
    MENU_MIDDLE_NEG_U8_CBDB(r3)->unk_CBDB = r0;
    MENU_MIDDLE_NEG_U8_CBDC(r3)->unk_CBDC = r0;
    r3 = r5 + (0x1 << 16);
    r5 = r5 + 0x2;
    MENU_MIDDLE_NEG_U8_CBDB(r3)->unk_CBDB = r0;
    MENU_MIDDLE_NEG_U8_CBDC(r3)->unk_CBDC = r0;
    r3 = r5 + (0x1 << 16);
    r5 = r5 + 0x2;
    MENU_MIDDLE_NEG_U8_CBDB(r3)->unk_CBDB = r0;
    MENU_MIDDLE_NEG_U8_CBDC(r3)->unk_CBDC = r0;
    r3 = r5 + (0x1 << 16);
    r5 = r5 + 0x2;
    MENU_MIDDLE_NEG_U8_CBDB(r3)->unk_CBDB = r0;
    MENU_MIDDLE_NEG_U8_CBDC(r3)->unk_CBDC = r0;
    r3 = r5 + (0x1 << 16);
    r5 = r5 + 0x2;
    MENU_MIDDLE_NEG_U8_CBDB(r3)->unk_CBDB = r0;
    MENU_MIDDLE_NEG_U8_CBDC(r3)->unk_CBDC = r0;
    r3 = r5 + (0x1 << 16);
    MENU_MIDDLE_NEG_U8_CBDB(r3)->unk_CBDB = r0;
    MENU_MIDDLE_NEG_U8_CBDC(r3)->unk_CBDC = r0;
    return;
}


/* 0x8006B8E8 | size: 0x8 */
u8 fn_8006B8E8(void) {
    return lbl_8047A5E0;
}

/* 0x8006B8F0 | size: 0xC */
void fn_8006B8F0(void) {
    lbl_8047A5E0 = 0;
}

/* 0x8006B8FC | size: 0xC */
void fn_8006B8FC(void) {
    lbl_8047A5E0 = 1;
}

/* 0x8006B908 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006B908(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006B930 | size: 0x88 */
#pragma peephole off
void fn_8006B930(void* menu) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    extern s32 fn_80071160(void);
    extern u32 fn_80071208(u32 flags);
    u32 flags;

    if (fn_80071160() != 0) {
        MENU_MIDDLE_U8_0098(menu)->unk_0098 = 1;
        MENU_MIDDLE_U8_0099(menu)->unk_0099 = 1;
        return;
    }

    flags = MENU_MIDDLE_U32_59CC(savedataGetStatus(0, 0xE))->unk_59CC;
    flags = fn_80071208(flags);
    if ((flags & 0x1000) != 0) {
        MENU_MIDDLE_U8_0098(menu)->unk_0098 = 1;
        return;
    }
    if ((flags & 0x200) == 0) {
        return;
    }
    MENU_MIDDLE_U8_0098(menu)->unk_0098 = 1;
    MENU_MIDDLE_U8_0099(menu)->unk_0099 = 1;
}
#pragma peephole reset


/* 0x8006B9B8 | size: 0x17C */
void fn_8006B9B8(void) {
    extern void fn_8006A814();
    extern void savedataGetStatus();
    extern void fn_80166A28();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r3 = 0x0;
    r4 = 0xe;
    savedataGetStatus();
    r0 = MENU_MIDDLE_U32_0004(r3)->unk_0004;
    if ((s32)r0 == (s32)0x2 || (s32)r0 >= (s32)0x2 || (s32)r0 < (s32)0x0) {
        r27 = 0x4;
    } else {
        r27 = 0x2;
    }
    r0 = MENU_MIDDLE_U8_0001(r31)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            return;
        }
        return;
    }
    ((void(*)(void))fn_80071160)();
    if ((s32)r3 != (s32)0x0) {
        r0 = 0x1;
        MENU_MIDDLE_U8_0098(r31)->unk_0098 = r0;
        MENU_MIDDLE_U8_0099(r31)->unk_0099 = r0;
        return;
    }
    r28 = 0x0;
    r29 = 0x0;
    while (1) {
        if ((s32)r28 >= (s32)r27) break;
        r3 = 0x0;
        r4 = 0xe;
        savedataGetStatus();
        r0 = r29 + 0x59a8;
        r3 = r3 + r0;
        fn_8006A814();
        ((void(*)(void))fn_80071208)();
        r0 = r3 & 0x00000100;
        if (r0 != (u32)0x0) {
            r3 = 0x0;
            r4 = 0xe;
            savedataGetStatus();
            r0 = r29 + 0x7005;
            r0 = *(u8*)(r3 + r0);
            if (r0 == (u32)0x0) {
                r30 = 0x1;
                r3 = 0x0;
                r4 = 0xe;
                savedataGetStatus();
                r0 = r29 + 0x7005;
                *(u8*)(r3 + r0) = r30;
                r3 = 0x24;
                fn_80166A28();
            }
        } else {
            r0 = r3 & 0x00000200;
            if (r0 != (u32)0x0) {
                r3 = 0x0;
                r4 = 0xe;
                savedataGetStatus();
                r0 = r29 + 0x7005;
                r0 = *(u8*)(r3 + r0);
                if (r0 != (u32)0x0) {
                    r30 = 0x0;
                    r3 = 0x0;
                    r4 = 0xe;
                    savedataGetStatus();
                    r0 = r29 + 0x7005;
                    *(u8*)(r3 + r0) = r30;
                    r3 = 0x25;
                    fn_80166A28();
                } else {
                    r0 = 0x1;
                    MENU_MIDDLE_U8_0098(r31)->unk_0098 = r0;
                    MENU_MIDDLE_U8_0099(r31)->unk_0099 = r0;
                    return;
                }
            }
        }
        r29 = r29 + 0x1660;
        r28 = r28 + 0x1;

    }

    return;
}


/* 0x8006BB34 | size: 0x4E4 */
extern void fn_80166A28(s32 sndId);
extern void __assert(const void* file, s32 line, const void* expr);

typedef struct MenuState_8006BB34 {
    u8 pad0[4];
    s32 menuId;   /* 0x04 */
    u8 pad8[2];
    u8 disabled;  /* 0x0A */
    u8 padB[0x89];
    u8 pad_val;   /* 0x94 */
    s8 pad_cur;   /* 0x95 */
} MenuState_8006BB34;

typedef struct KeyInfo_8006BB34 {
    u8 pad0[4];
    u16 flags4;
    u16 flags6;
    u8 padA[2];
} KeyInfo_8006BB34;

typedef struct Param_8006BB34 {
    u8 pad0[0x11];
    u8 sel11;
    u8 sel12;
    u8 sel13;
    s16 val14;
    s16 val16;
} Param_8006BB34;

typedef struct CursorArg_8006BB34 {
    u8 pad;
    s8 cursor;
} CursorArg_8006BB34;

void fn_8006BB34(void* menu) {
    MenuState_8006BB34* m = (MenuState_8006BB34*)menu;
    KeyInfo_8006BB34* ki;
    Param_8006BB34* p;
    u8* asrt = lbl_80267EA8;
    s32 cur;
    int inc, confirm, dec;
    int pressed;
    int dir;
    s32 delta;
    s32 signOld;
    s32 absVal;
    s16 v;
    CursorArg_8006BB34 argA;
    CursorArg_8006BB34 argB;
    CursorArg_8006BB34 srcA;
    CursorArg_8006BB34 srcB;

    ki = (KeyInfo_8006BB34*)windowGetKeyInfo();
    if (m->disabled != 0) return;

    {
        u32 _f = ki->flags6;
        u32 inc_bit = _f & 4;
        u32 confirm_bit = _f & 1;
        u32 dec_bit = _f & 8;
        u32 inc_b = ((u32)(-(s32)inc_bit) | inc_bit) >> 31;
        u32 confirm_b = ((u32)(-(s32)confirm_bit) | confirm_bit) >> 31;
        u32 dec_b = ((u32)(-(s32)dec_bit) | dec_bit) >> 31;
        inc = (int)inc_b;
        confirm = (int)confirm_b;
        dec = (int)dec_b;

        pressed = 0;
        if (inc_b != 0 || dec_b != 0) pressed = 1;

        dir = 0;
        if (inc_b != 0 || dec_b == 0) dir = 1;
    }

    cur = menuGetCursorItemID(m->menuId);
    if (fn_80077BD0()) {
        s32 c;
        if (!confirm) return;
        c = menuGetCursorFromItemID(m->menuId, 0xE35);
        srcA.cursor = (s8)c;
        srcA.pad = 0;
        argA = srcA;
        fn_801044D0(m->menuId, &argA);
        return;
    }

    p = (Param_8006BB34*)windowGetParam(menu, 0);

    delta = 0;
    if (ki->flags6 & 2) delta = -1;
    else if (ki->flags6 & 1) delta = 1;

    switch (cur) {
    case 0xA0C:
        delta = delta * 10;
        /* fallthrough */
    case 0xA0D:
        v = p->val14;
        if (v >= 0) {
            s32 nv = v + delta;
            if (nv < 1) nv = 1;
            else if (nv > 99) nv = 99;
            p->val14 = (s16)nv;
        }
        break;
    case 0xE34:
        delta = delta * 10;
        /* fallthrough */
    case 0xE33:
        v = p->val16;
        if (v >= 0) {
            s32 nv = v + delta;
            if (nv < 1) nv = 1;
            else if (nv > 99) nv = 99;
            p->val16 = (s16)nv;
        }
        break;
    default:
        delta = 0;
        break;
    }
    if (delta != 0) return;

    srcB.pad = m->pad_val;
    srcB.cursor = m->pad_cur;

    switch (cur) {
    case 0x9F7:
        if (!pressed) break;
        if (p->sel11 != dir) fn_80166A28(0x24);
        p->sel11 = dir;
        return;
    case 0x9F8:
        if (!pressed) break;
        if (p->sel12 != dir) fn_80166A28(0x24);
        p->sel12 = dir;
        return;
    case 0x9F9:
        if (!pressed) break;
        if (p->sel13 != dir) fn_80166A28(0x24);
        p->sel13 = dir;
        return;
    case 0x9FA:
        if (pressed) {
            s16 vv = p->val14;
            signOld = (u32)vv >> 31;
            absVal = ((s32)vv >> 31) ^ vv;
            absVal = absVal - ((s32)vv >> 31);
            if ((u32)signOld == dir) fn_80166A28(0x24);
            p->val14 = (s16)(dir ? absVal : -absVal);
            return;
        }
        if (p->val14 < 0) break;
        if (!(ki->flags4 & 0x10)) break;
        {
            s32 c = menuGetCursorFromItemID(m->menuId, 0xA0C);
            menuSetCursor(m->menuId, c);
        }
        return;
    case 0x9FB:
        if (pressed) {
            s16 vv = p->val16;
            signOld = (u32)vv >> 31;
            absVal = ((s32)vv >> 31) ^ vv;
            absVal = absVal - ((s32)vv >> 31);
            if ((u32)signOld == dir) fn_80166A28(0x24);
            p->val16 = (s16)(dir ? absVal : -absVal);
            return;
        }
        if (p->val16 < 0) break;
        if (!(ki->flags4 & 0x10)) break;
        {
            s32 c = menuGetCursorFromItemID(m->menuId, 0xE34);
            menuSetCursor(m->menuId, c);
        }
        return;
    case 0xA0C:
        if (inc) return;
        /* fallthrough */
    case 0xA0D:
        if (p->val14 < 0) {
            __assert(asrt + 0x7d8, 0xE73, asrt + 0x7e8);
        }
        if (!(ki->flags4 & 0x30)) break;
        {
            s32 c = menuGetCursorFromItemID(m->menuId, 0x9FA);
            menuSetCursor(m->menuId, c);
        }
        return;
    case 0xE34:
        if (inc) return;
        /* fallthrough */
    case 0xE33:
        if (p->val16 < 0) {
            __assert(asrt + 0x7d8, 0xE7F, asrt + 0x808);
        }
        if (!(ki->flags4 & 0x30)) break;
        {
            s32 c = menuGetCursorFromItemID(m->menuId, 0x9FB);
            menuSetCursor(m->menuId, c);
        }
        return;
    case 0x9FD:
        if (confirm) {
            s32 c = menuGetCursorFromItemID(m->menuId, 0x9FB);
            srcB.cursor = (s8)c;
            argB = srcB;
            fn_801044D0(m->menuId, &argB);
            return;
        }
        if (dec) return;
        break;
    default:
        break;
    }

    menuCursorNormal(menu);
}

#if 0
static void fn_8006BB34_deadcode(void) {
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r22 = 0;
    r5 = MENU_MIDDLE_U16_0006(r26)->unk_0006;
    r3 = 0x0;
    r4 = r5 & 0x00000004;
    r7 = r5 & 0x1;
    r0 = -r4;
    r5 = r5 & 0x00000008;
    r0 = r0 | r4;
    r6 = -r7;
    r28 = (u32)r0 >> 31;
    r4 = -r5;
    r0 = r28 & 0xFF;
    r6 = r6 | r7;
    r0 = r4 | r5;
    r27 = (u32)r6 >> 31;
    r29 = (u32)r0 >> 31;
    if (r0 == (u32)0x0) {
        r0 = r29 & 0xFF;
        if (r0 != (u32)0x0) {
        }
        r3 = 0x1;
        }
    r0 = r28 & 0xFF;
    r24 = r3 & 0xFF;
    r4 = 0x0;
    if (r0 == (u32)0x0) {
        r0 = r29 & 0xFF;
        if (r0 == (u32)0x0) {
        }
        r4 = 0x1;
        }
    r3 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
    r23 = r4 & 0xFF;
    ((void(*)(void))menuGetCursorItemID)();
    r30 = r3;
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r0 = r27 & 0xFF;
        if (r0 == (u32)0x0) return;
        r3 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
        r4 = 0xe35;
        ((void(*)(void))menuGetCursorFromItemID)();
        r3 = (s8)r3;
        r0 = 0x0;
        *(u8*)(sp + 0x11) = r3;
        r4 = (u32)sp + 0xc;
        *(u8*)(sp + 0x10) = r0;
        r0 = *(u16*)(sp + 0x10);
        *(u16*)(sp + 0xC) = r0;
        r3 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
        ((void(*)(void))fn_801044D0)();
        return;
    }
    r3 = r22;
    r4 = 0x0;
    ((void(*)(void))windowGetParam)();
    r4 = MENU_MIDDLE_U16_0006(r26)->unk_0006;
    r31 = r3;
    r3 = 0x0;
    r0 = r4 & 0x00000002;
    if ((s32)r0 != (s32)0x0) {
        r3 = -0x1;

    } else {
    r0 = r4 & 0x1;
    if ((s32)r0 != (s32)0x0) {
        r3 = 0x1;
    }
    }
    if ((s32)r30 != (s32)0xe33) {
        if ((s32)r30 < (s32)0xe33) {
            if ((s32)r30 != (s32)0xa0d) {
                if ((s32)r30 >= (s32)0xa0d) goto L_8006BD20;
                if ((s32)r30 < (s32)0xa0c) {
                    goto L_8006BD20;
                }
                if ((s32)r30 >= (s32)0xe35) goto L_8006BD20;
                goto L_8006BCE4;
                }
            r3 = r3 * 0xa;
            }
        r0 = MENU_MIDDLE_S16_0014(r31)->unk_0014;
        if ((s32)r0 >= (s32)0x0) {
            r0 = r0 + r3;
            if ((s32)r0 < (s32)0x1) {
                r0 = 0x1;

            } else {
            if ((s32)r0 > (s32)0x63) {
                r0 = 0x63;
        }
            }
            r0 = (s16)r0;
            MENU_MIDDLE_U16_0014(r31)->unk_0014 = r0;
        }
        goto L_8006BD24;
        L_8006BCE4: ;
        r3 = r3 * 0xa;
    }
    r0 = MENU_MIDDLE_S16_0016(r31)->unk_0016;
    if ((s32)r0 >= (s32)0x0) {
        r0 = r0 + r3;
        if ((s32)r0 < (s32)0x1) {
            r0 = 0x1;

        } else {
        if ((s32)r0 > (s32)0x63) {
            r0 = 0x63;
    }
        }
        r0 = (s16)r0;
        MENU_MIDDLE_U16_0016(r31)->unk_0016 = r0;
    }
    goto L_8006BD24;
    L_8006BD20: ;
    r3 = 0x0;
    L_8006BD24: ;
    if ((s32)r3 != (s32)0x0) return;
    r0 = MENU_MIDDLE_U16_0094(r22)->unk_0094;
    *(u16*)(sp + 0x14) = r0;
    if ((s32)r30 != (s32)0x9fd) {
        if ((s32)r30 < (s32)0x9fd) {
            if ((s32)r30 != (s32)0x9f9) {
                if ((s32)r30 < (s32)0x9f9) {
                    if ((s32)r30 != (s32)0x9f7) {
                        if ((s32)r30 < (s32)0x9f7) {
                            goto L_8006BFFC;
                        }
                        if ((s32)r30 != (s32)0x9fb) {
                            if ((s32)r30 < (s32)0x9fb) {
                                goto L_8006BE08;
                            }
                            if ((s32)r30 == (s32)0xe33) goto L_8006BF70;
                            if ((s32)r30 < (s32)0xe33) {
                                if ((s32)r30 == (s32)0xa0d) goto L_8006BF1C;
                                if ((s32)r30 < (s32)0xa0d) {
                                    if ((s32)r30 >= (s32)0xa0c) goto L_8006BF10;
                            }
                            }
                            goto L_8006BFFC;
                        }
                        if ((s32)r30 >= (s32)0xe35) goto L_8006BFFC;
                        goto L_8006BF64;
                            }
                    if (r24 == (u32)0x0) goto L_8006BFFC;
                    r0 = MENU_MIDDLE_U8_0011(r31)->unk_0011;
                    if (r0 != (u32)r23) {
                        r3 = 0x24;
                        fn_80166A28();
                    }
                    MENU_MIDDLE_U8_0011(r31)->unk_0011 = r23;
                    return;
                            }
                if (r24 == (u32)0x0) goto L_8006BFFC;
                r0 = MENU_MIDDLE_U8_0012(r31)->unk_0012;
                if (r0 != (u32)r23) {
                    r3 = 0x24;
                    fn_80166A28();
                }
                MENU_MIDDLE_U8_0012(r31)->unk_0012 = r23;
                return;
                }
            if (r24 == (u32)0x0) goto L_8006BFFC;
            r0 = MENU_MIDDLE_U8_0013(r31)->unk_0013;
            if (r0 != (u32)r23) {
                r3 = 0x24;
                fn_80166A28();
            }
            MENU_MIDDLE_U8_0013(r31)->unk_0013 = r23;
            return;
            L_8006BE08: ;
            if (r24 != (u32)0x0) {
                r4 = MENU_MIDDLE_S16_0014(r31)->unk_0014;
                r0 = (u32)r4 >> 31;
                r3 = (s32)r4 >> 31;
                r22 = r3 ^ r4;
                r22 = r22 - r3;
                if ((s32)r0 == (s32)r23) {
                    r3 = 0x24;
                    fn_80166A28();
                }
                if (r23 != (u32)0x0) {
                    r0 = r22;
                } else {

                    r0 = -r22;
                }
                r0 = (s16)r0;
                MENU_MIDDLE_U16_0014(r31)->unk_0014 = r0;
                return;
            }
            r0 = MENU_MIDDLE_S16_0014(r31)->unk_0014;
            if ((s32)r0 >= (s32)0x0) {
                r0 = MENU_MIDDLE_U16_0004(r26)->unk_0004;
                r0 = r0 & 0x00000010;
            }
            if ((s32)r0 == (s32)0x0) goto L_8006BFFC;
            r3 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
            r4 = 0xa0c;
            ((void(*)(void))menuGetCursorFromItemID)();
            r4 = r3;
            r3 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
            ((void(*)(void))menuSetCursor)();
            return;
                        }
        if (r24 != (u32)0x0) {
            r4 = MENU_MIDDLE_S16_0016(r31)->unk_0016;
            r0 = (u32)r4 >> 31;
            r3 = (s32)r4 >> 31;
            r22 = r3 ^ r4;
            r22 = r22 - r3;
            if ((s32)r0 == (s32)r23) {
                r3 = 0x24;
                fn_80166A28();
            }
            if (r23 != (u32)0x0) {
                r0 = r22;
            } else {

                r0 = -r22;
            }
            r0 = (s16)r0;
            MENU_MIDDLE_U16_0016(r31)->unk_0016 = r0;
            return;
        }
        r0 = MENU_MIDDLE_S16_0016(r31)->unk_0016;
        if ((s32)r0 >= (s32)0x0) {
            r0 = MENU_MIDDLE_U16_0004(r26)->unk_0004;
            r0 = r0 & 0x00000010;
        }
        if ((s32)r0 == (s32)0x0) goto L_8006BFFC;
        r3 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
        r4 = 0xe34;
        ((void(*)(void))menuGetCursorFromItemID)();
        r4 = r3;
        r3 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
        ((void(*)(void))menuSetCursor)();
        return;
        L_8006BF10: ;
        r0 = r28 & 0xFF;
        if (r0 != (u32)0x0) return;
        L_8006BF1C: ;
        r0 = MENU_MIDDLE_S16_0014(r31)->unk_0014;
        if ((s32)r0 < (s32)0x0) {
            r3 = r25 + 0x7d8;
            r5 = r25 + 0x7e8;
            r4 = 0xe73;
            __assert();
        }
        r0 = MENU_MIDDLE_U16_0004(r26)->unk_0004;
        r0 = r0 & 0x00000030;
        if ((s32)r0 == (s32)0x0) goto L_8006BFFC;
        r3 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
        r4 = 0x9fa;
        ((void(*)(void))menuGetCursorFromItemID)();
        r4 = r3;
        r3 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
        ((void(*)(void))menuSetCursor)();
        return;
        L_8006BF64: ;
        r0 = r28 & 0xFF;
        if (r0 != (u32)0x0) return;
        L_8006BF70: ;
        r0 = MENU_MIDDLE_S16_0016(r31)->unk_0016;
        if ((s32)r0 < (s32)0x0) {
            r3 = r25 + 0x7d8;
            r5 = r25 + 0x808;
            r4 = 0xe7f;
            __assert();
        }
        r0 = MENU_MIDDLE_U16_0004(r26)->unk_0004;
        r0 = r0 & 0x00000030;
        if ((s32)r0 == (s32)0x0) goto L_8006BFFC;
        r3 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
        r4 = 0x9fb;
        ((void(*)(void))menuGetCursorFromItemID)();
        r4 = r3;
        r3 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
        ((void(*)(void))menuSetCursor)();
        return;
    }
    r0 = r27 & 0xFF;
    if (r0 != (u32)0x0) {
        r3 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
        r4 = 0x9fb;
        ((void(*)(void))menuGetCursorFromItemID)();
        r0 = (s8)r3;
        r4 = (u32)sp + 0x8;
        *(u8*)(sp + 0x15) = r0;
        r0 = *(u16*)(sp + 0x14);
        *(u16*)(sp + 0x8) = r0;
        r3 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
        ((void(*)(void))fn_801044D0)();
        return;
    }
    r0 = r29 & 0xFF;
    if (r0 != (u32)0x0) return;
    L_8006BFFC: ;
    r3 = r22;
    ((void(*)(void))menuCursorNormal)();

    return;
}
#endif


/* 0x8006C018 | size: 0xC4 */
#pragma peephole off
void fn_8006C018(void* menu) {
    typedef struct MenuButton_8006C018 {
        u8 pad0;
        u8 state;
        u8 pad2[2];
        s32 menuId;
    } MenuButton_8006C018;

    MenuButton_8006C018* button = menu;
    KeyInfo_8006BB34* keyInfo;
    s32 itemId;
    u32 value;

    value = button->state;
    value = (s8)value;
    if ((s32)value != 2) {
        return;
    }

    keyInfo = windowGetKeyInfo();
    itemId = menuGetCursorItemID(button->menuId);

    switch (itemId) {
    case 0x9F7:
    case 0x9F8:
    case 0x9F9:
    case 0x9FA:
    case 0x9FB:
        value = keyInfo->flags4 & 0x10;
        if ((s32)value == 0) {
            break;
        }
        return;
    case 0xA0C:
    case 0xA0D:
    case 0xE33:
    case 0xE34:
        value = keyInfo->flags4 & 0x30;
        if ((s32)value == 0) {
            break;
        }
        return;
    }

    menuButtonNormal(button);
}
#pragma peephole reset


/* 0x8006C0DC | size: 0x88 */
#pragma peephole off
void fn_8006C0DC(void* menu) {
    typedef struct MenuButton_8006C0DC {
        u8 pad0;
        u8 state;
        u8 pad2[2];
        s32 menuId;
    } MenuButton_8006C0DC;

    MenuButton_8006C0DC* button = menu;
    KeyInfo_8006BB34* keyInfo;
    s32 itemId;
    u32 value;

    value = button->state;
    value = (s8)value;
    if ((s32)value != 2) {
        return;
    }

    keyInfo = windowGetKeyInfo();
    itemId = menuGetCursorItemID(button->menuId);

    do {
        if (itemId >= 0x9D2) {
            break;
        }
        if (itemId < 0x9CA) {
            break;
        }
        value = keyInfo->flags4 & 0x10;
        if ((s32)value == 0) {
            break;
        }
        return;
    } while (0);

    menuButtonNormal(button);
}
#pragma peephole reset


/* 0x8006C164 | size: 0x474 */
void fn_8006C164(void) {
    extern void fn_80166A28();
    extern u8 jumptable_802EDE78[];
    extern u8 jumptable_802EDEFC[];
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r24 = r3;
    r0 = MENU_MIDDLE_U8_000A(r24)->unk_000A;
    if (r0 != (u32)0x0) return;
    ((void(*)(void))windowGetKeyInfo)();
    r0 = MENU_MIDDLE_U16_0006(r3)->unk_0006;
    r3 = r0 & 0x1;
    r0 = -r3;
    r0 = r0 | r3;
    r27 = (u32)r0 >> 31;
    ((void(*)(void))windowGetKeyInfo)();
    r0 = MENU_MIDDLE_U16_0006(r3)->unk_0006;
    r3 = r0 & 0x00000002;
    r0 = -r3;
    r0 = r0 | r3;
    r28 = (u32)r0 >> 31;
    ((void(*)(void))windowGetKeyInfo)();
    r0 = MENU_MIDDLE_U16_0006(r3)->unk_0006;
    r3 = r0 & 0x00000004;
    r0 = -r3;
    r0 = r0 | r3;
    r29 = (u32)r0 >> 31;
    ((void(*)(void))windowGetKeyInfo)();
    r4 = MENU_MIDDLE_U16_0006(r3)->unk_0006;
    r0 = r29 & 0xFF;
    r3 = 0x0;
    r4 = r4 & 0x00000008;
    r0 = -r4;
    r0 = r0 | r4;
    r30 = (u32)r0 >> 31;
    if (r0 == (u32)0x0) {
        r0 = r30 & 0xFF;
        if (r0 != (u32)0x0) {
        }
        r3 = 0x1;
        }
    r0 = r29 & 0xFF;
    r26 = r3 & 0xFF;
    r3 = 0x0;
    if (r0 == (u32)0x0) {
        r0 = r30 & 0xFF;
        if (r0 == (u32)0x0) {
        }
        r3 = 0x1;
        }
    r25 = r3 & 0xFF;
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r0 = r28 & 0xFF;
        if (r0 == (u32)0x0) return;
        r3 = MENU_MIDDLE_U32_0004(r24)->unk_0004;
        r4 = 0x9d2;
        ((void(*)(void))menuGetCursorFromItemID)();
        r3 = (s8)r3;
        r0 = 0x0;
        *(u8*)(sp + 0x11) = r3;
        r4 = (u32)sp + 0xc;
        *(u8*)(sp + 0x10) = r0;
        r0 = *(u16*)(sp + 0x10);
        *(u16*)(sp + 0xC) = r0;
        r3 = MENU_MIDDLE_U32_0004(r24)->unk_0004;
        ((void(*)(void))fn_801044D0)();
        return;
    }
    r3 = r24;
    r4 = 0x0;
    ((void(*)(void))windowGetParam)();
    r31 = r3;
    r3 = MENU_MIDDLE_U32_0004(r24)->unk_0004;
    ((void(*)(void))menuGetCursorItemID)();
    r0 = r28 & 0xFF;
    r5 = 0x0;
    if (r0 != (u32)0x0) {
        r5 = -0x1;
    } else {
        r0 = r27 & 0xFF;
        if (r0 != (u32)0x0) {
            r5 = 0x1;
        }
    }
    /* subi r0, r3, 0x9e2 */;
    if (r0 <= (u32)0x8) {
        r4 = (u32)jumptable_802EDEFC;
        r0 = r0 << 2;
        r4 = (u32)jumptable_802EDEFC;
        r0 = *(u32*)(r4 + r0);
        ctr_fn = (void(*)(void))r0;
        /* indirect jump via ctr */;
        r5 = r5 * 0xa;
        r5 = r5 * 0xa;
        r0 = MENU_MIDDLE_S16_0000(r31)->unk_0000;
        r0 = r0 + r5;
        r0 = (s16)r0;
        MENU_MIDDLE_U16_0000(r31)->unk_0000 = r0;
        r0 = MENU_MIDDLE_S16_0002(r31)->unk_0002;
        r4 = MENU_MIDDLE_S16_0000(r31)->unk_0000;
        if ((s32)r0 < (s32)r4) {
            MENU_MIDDLE_U16_0002(r31)->unk_0002 = r4;
        }
        /* case 1: handled above */
        r5 = r5 * 0xa;
        r5 = r5 * 0xa;
        r0 = MENU_MIDDLE_S16_0002(r31)->unk_0002;
        r0 = r0 + r5;
        r0 = (s16)r0;
        MENU_MIDDLE_U16_0002(r31)->unk_0002 = r0;
        r4 = MENU_MIDDLE_S16_0002(r31)->unk_0002;
        r0 = MENU_MIDDLE_S16_0000(r31)->unk_0000;
        if ((s32)r4 < (s32)r0) {
            MENU_MIDDLE_U16_0000(r31)->unk_0000 = r4;
        }
        /* case 2: handled above */
        r5 = r5 * 0xa;
        r5 = r5 * 0xa;
        r0 = MENU_MIDDLE_S16_0004(r31)->unk_0004;
        r0 = r0 + r5;
        r0 = (s16)r0;
        MENU_MIDDLE_U16_0004(r31)->unk_0004 = r0;
        /* case 3: handled above */
    }
    r5 = 0x0;
    if ((s32)r5 != (s32)0x0) {
        r0 = MENU_MIDDLE_S16_0000(r31)->unk_0000;
        if ((s32)r0 < (s32)0x1) {
            r0 = 0x1;
        } else if ((s32)r0 > (s32)0x64) {
            r0 = 0x64;
        }
        r0 = (s16)r0;
        MENU_MIDDLE_U16_0000(r31)->unk_0000 = r0;
        r0 = MENU_MIDDLE_S16_0002(r31)->unk_0002;
        if ((s32)r0 < (s32)0x1) {
            r0 = 0x1;
        } else if ((s32)r0 > (s32)0x64) {
            r0 = 0x64;
        }
        r0 = (s16)r0;
        MENU_MIDDLE_U16_0002(r31)->unk_0002 = r0;
        r0 = MENU_MIDDLE_S16_0000(r31)->unk_0000;
        r3 = MENU_MIDDLE_S16_0004(r31)->unk_0004;
        r0 = r0 * 0x6;
        if ((s32)r0 <= (s32)r3) {
            r0 = MENU_MIDDLE_S16_0002(r31)->unk_0002;
            r0 = r0 * 0x6;
            if ((s32)r0 >= (s32)r3) {
                r0 = r3;
            }
        }
        r0 = (s16)r0;
        MENU_MIDDLE_U16_0004(r31)->unk_0004 = r0;
        return;
    }
    /* subi r0, r3, 0x9ca */;
    r3 = MENU_MIDDLE_U16_0094(r24)->unk_0094;
    *(u16*)(sp + 0x14) = r3;
    if (r0 <= (u32)0x20) {
        r3 = (u32)jumptable_802EDE78;
        r0 = r0 << 2;
        r3 = (u32)jumptable_802EDE78;
        r0 = *(u32*)(r3 + r0);
        ctr_fn = (void(*)(void))r0;
        /* indirect jump via ctr */;
        r0 = r27 & 0xFF;
        if (r0 != (u32)0x0) {
            return;

            if (r0 != (u32)0x0) return;
            r0 = r29 & 0xFF;
            if (r0 == (u32)0x0) {
                r0 = r29 & 0xFF;
                if (r0 != (u32)0x0) {
                    r3 = *(u8*)(sp + 0x15);
                    /* subi r0, r3, 0x1 */;
                    *(u8*)(sp + 0x15) = r0;
                }
                r0 = r30 & 0xFF;
                if (r0 != (u32)0x0) {
                    r3 = *(u8*)(sp + 0x15);
                    r0 = r3 + 0x1;
                    *(u8*)(sp + 0x15) = r0;
                }
                r0 = *(u16*)(sp + 0x14);
                r4 = (u32)sp + 0x8;
                *(u16*)(sp + 0x8) = r0;
                r3 = MENU_MIDDLE_U32_0004(r24)->unk_0004;
                ((void(*)(void))fn_801044D0)();
                return;
                if (r26 != (u32)0x0) {
                    r0 = MENU_MIDDLE_U8_000C(r31)->unk_000C;
                    if (r0 != (u32)r25) {
                        r3 = 0x24;
                        fn_80166A28();
                    }
                    MENU_MIDDLE_U8_000C(r31)->unk_000C = r25;
                    return;

                    if (r0 != (u32)0x0) {
                        r3 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
                        /* subi r0, r3, 0x1 */;
                        MENU_MIDDLE_U32_0008(r31)->unk_0008 = r0;
                        r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
                        if ((s32)r0 < (s32)0x0) {
                            r0 = 0x0;
                            MENU_MIDDLE_U32_0008(r31)->unk_0008 = r0;
                            return;
                        }
                        r3 = 0x24;
                        fn_80166A28();
                        return;
                    }
                    r0 = r30 & 0xFF;
                    if (r0 != (u32)0x0) {
                        r3 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
                        r0 = r3 + 0x1;
                        MENU_MIDDLE_U32_0008(r31)->unk_0008 = r0;
                        r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
                        if ((s32)r0 >= (s32)0x3) {
                            r0 = 0x2;
                            MENU_MIDDLE_U32_0008(r31)->unk_0008 = r0;
                            return;
                        }
                        r3 = 0x24;
                        fn_80166A28();
                        return;
                        if (r26 != (u32)0x0) {
                            r0 = MENU_MIDDLE_U8_000D(r31)->unk_000D;
                            if (r0 != (u32)r25) {
                                r3 = 0x24;
                                fn_80166A28();
                            }
                            MENU_MIDDLE_U8_000D(r31)->unk_000D = r25;
                            return;
                            if (r26 != (u32)0x0) {
                                r0 = MENU_MIDDLE_U8_000E(r31)->unk_000E;
                                if (r0 != (u32)r25) {
                                    r3 = 0x24;
                                    fn_80166A28();
                                }
                                MENU_MIDDLE_U8_000E(r31)->unk_000E = r25;
                                return;
                                if (r26 != (u32)0x0) {
                                    r0 = MENU_MIDDLE_U8_000F(r31)->unk_000F;
                                    if (r0 != (u32)r25) {
                                        r3 = 0x24;
                                        fn_80166A28();
                                    }
                                    MENU_MIDDLE_U8_000F(r31)->unk_000F = r25;
                                    return;
                                    if (r26 != (u32)0x0) {
                                        r0 = MENU_MIDDLE_U8_0010(r31)->unk_0010;
                                        if (r0 != (u32)r25) {
                                            r3 = 0x24;
                                            fn_80166A28();
                                        }
                                        MENU_MIDDLE_U8_0010(r31)->unk_0010 = r25;
                                        return;
    }
    }
    }
    }
    }
    }
    }
    }
    }
    r3 = r24;
    ((void(*)(void))menuCursorNormal)();

    return;
}


/* 0x8006C5D8 | size: 0x1FC */
void fn_8006C5D8(void) {
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    
    r31 = r4;
    r3 = (u32)&lbl_80268674;
    r5 = MENU_MIDDLE_S16_0054(r31)->unk_0054;
    r9 = (u32)&lbl_80268674;
    r6 = MENU_MIDDLE_S16_0056(r31)->unk_0056;
    r8 = MENU_MIDDLE_U32_0000(r9)->unk_0000;
    r3 = 0x0;
    r7 = MENU_MIDDLE_U32_0004(r9)->unk_0004;
    r4 = 0x0;
    r0 = MENU_MIDDLE_U32_0008(r9)->unk_0008;
    *(u32*)(sp + 0x10) = r0;
    ((void(*)(void))fn_800FE38C)();
    r3 = 0x1;
    ((void(*)(void))fn_800D88DC)();
    r3 = 0x6;
    ((void(*)(void))fn_800D888C)();
    r5 = *(u8*)(sp + 0xF);
    r3 = (0x8081 << 16);
    r4 = MENU_MIDDLE_U8_0067(r31)->unk_0067;
    /* subi r6, r3, 0x7f7f */;
    r0 = *(u8*)(sp + 0x13);
    r3 = 0x6;
    r5 = r5 * r4;
    r0 = r0 * r4;
    r7 = (s32)((s64)r6 * (s64)r5 >> 32);
    r4 = (s32)((s64)r6 * (s64)r0 >> 32);
    r5 = r7 + r5;
    r5 = (s32)r5 >> 7;
    r0 = r4 + r0;
    r6 = (u32)r5 >> 31;
    r0 = (s32)r0 >> 7;
    r4 = (u32)r0 >> 31;
    r5 = r5 + r6;
    r0 = r0 + r4;
    r4 = r5 & 0xFF;
    r0 = r0 & 0xFF;
    *(u8*)(sp + 0xF) = r4;
    *(u8*)(sp + 0x13) = r0;
    ((void(*)(void))fn_800D6A00)();
    r3 = (u32)&lbl_80314E08;
    r3 = (u32)&lbl_80314E08;
    ((void(*)(void))fn_800D7820)();
    r3 = 0x4;
    ((void(*)(void))fn_800D67BC)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_800D61E4)();
    r3 = 0x0;
    ((void(*)(void))fn_800D5BA0)();
    r3 = MENU_MIDDLE_S16_0054(r31)->unk_0054;
    r4 = 0x0;
    ((void(*)(void))fn_800D61E4)();
    r3 = 0x0;
    ((void(*)(void))fn_800D5BA0)();
    r3 = MENU_MIDDLE_S16_0054(r31)->unk_0054;
    r4 = MENU_MIDDLE_S16_0056(r31)->unk_0056;
    ((void(*)(void))fn_800D61E4)();
    r3 = 0x0;
    ((void(*)(void))fn_800D5BA0)();
    r4 = MENU_MIDDLE_S16_0056(r31)->unk_0056;
    r3 = 0x0;
    ((void(*)(void))fn_800D61E4)();
    r3 = 0x0;
    ((void(*)(void))fn_800D5BA0)();
    ((void(*)(void))fn_800D6728)();
    f1 = *(f32*)&lbl_8047C060;
    ((void(*)(void))fn_800D5648)();
    r3 = 0x1;
    ((void(*)(void))fn_800D6A00)();
    r3 = (u32)&lbl_80314E08;
    r3 = (u32)&lbl_80314E08;
    ((void(*)(void))fn_800D7820)();
    r0 = MENU_MIDDLE_U8_0067(r31)->unk_0067;
    r4 = 0xff;
    r3 = (0x8081 << 16);
    *(u8*)(sp + 0x8) = r4;
    r0 = r0 * 0x38;
    r30 = 0x0;
    /* subi r3, r3, 0x7f7f */;
    *(u8*)(sp + 0x9) = r4;
    r3 = (s32)((s64)r3 * (s64)r0 >> 32);
    *(u8*)(sp + 0xA) = r4;
    r0 = r3 + r0;
    r0 = (s32)r0 >> 7;
    r3 = (u32)r0 >> 31;
    r0 = r0 + r3;
    r0 = r0 & 0xFF;
    *(u8*)(sp + 0xB) = r0;

    while ((s32)r3 < (s32)r0) {
        r3 = 0x2;
        ((void(*)(void))fn_800D67BC)();
        r4 = r30;
        r3 = 0x0;
        ((void(*)(void))fn_800D61E4)();
        r3 = 0x0;
        ((void(*)(void))fn_800D5BA0)();
        r3 = MENU_MIDDLE_S16_0054(r31)->unk_0054;
        r4 = r30;
        ((void(*)(void))fn_800D61E4)();
        r3 = 0x0;
        ((void(*)(void))fn_800D5BA0)();
        ((void(*)(void))fn_800D6728)();
        r30 = r30 + 0x4;

    r0 = MENU_MIDDLE_S16_0056(r31)->unk_0056;
    r3 = (s16)r30;
    }
    ((void(*)(void))fn_800FE35C)();
    return;
}


/* 0x8006C7D4 | size: 0x4EC */
void fn_8006C7D4(void) {
    extern void fn_8006A7E8();
    extern void savedataGetStatus();
    extern void heroBiosGetSexDataId();
    extern void heroBiosGetRnd();
    extern void heroBiosGetNamePtr();
    extern void msgctrlSetValue();
    extern u8 jumptable_802EDF20[];
    u8 sp[0x1A0];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r28 = r4;
    r3 = MENU_MIDDLE_S16_0006(r28)->unk_0006;
    /* subi r0, r3, 0xec2 */;
    if (r0 > (u32)0x23) return;
    r3 = (u32)jumptable_802EDF20;
    r0 = r0 << 2;
    r3 = (u32)jumptable_802EDF20;
    r0 = *(u32*)(r3 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    r0 = 0x2;
    r30 = 0x0;
    do {
    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;

    } while (0);
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 == (s32)0x0) return;
            if ((s32)r0 < (s32)0x0) {
                return;
            }
            if ((s32)r0 >= (s32)0x4) return;
            goto L_8006CB80;
            }
        r29 = MENU_MIDDLE_U32_0064(r28)->unk_0064;
        r3 = 0x0;
        r4 = 0xe;
        savedataGetStatus();
        r4 = r30 * 0x1660;
        r0 = r4 + 0x64ec;
        r0 = r3 + r0;
        r3 = r0;
        heroBiosGetNamePtr();
        r4 = r3;
        r3 = 0x37;
        msgctrlSetValue();
        r5 = r29;
        r3 = 0x0;
        r4 = 0x0;
        r6 = 0xd0;
        ((void(*)(void))fn_800FB680)();
        return;
    }
    r31 = 0x0;
    r3 = 0x0;
    r4 = 0xe;
    savedataGetStatus();
    r4 = r30 * 0x1660;
    r29 = r4 + 0x59a8;
    r29 = r3 + r29;
    r3 = r29;
    fn_8006A7E8();
    if ((s32)r3 != (s32)0x1) {
        if ((s32)r3 < (s32)0x1) {
            if ((s32)r3 < (s32)0x0) {
                goto L_8006CB08;
            }
            if ((s32)r3 >= (s32)0x3) goto L_8006CB08;
            goto L_8006CACC;
            }
        r0 = 0x0;
        goto L_8006CB0C;
    }
    r3 = r29 + 0xb44;
    heroBiosGetSexDataId();
    r0 = r3 & 0xFF;
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 < (s32)0x1) {
            if ((s32)r0 < (s32)0x0) {
                goto L_8006CB08;
            }
            goto L_8006CB08;
            }
        r0 = 0x1;
        goto L_8006CB0C;
    }
    r0 = 0x2;
    goto L_8006CB0C;
    L_8006CACC: ;
    r3 = r29 + 0xb44;
    heroBiosGetSexDataId();
    r0 = r3 & 0xFF;
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 < (s32)0x1) {
            if ((s32)r0 < (s32)0x0) {
                goto L_8006CB08;
            }
            goto L_8006CB08;
            }
        r0 = 0x3;
        goto L_8006CB0C;
    }
    r0 = 0x4;
    goto L_8006CB0C;
    L_8006CB08: ;
    r0 = 0x1;
    L_8006CB0C: ;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 != (s32)0x0) {
                if ((s32)r0 < (s32)0x0) {
                    goto L_8006CB5C;
                }
                if ((s32)r0 != (s32)0x4) {
                    if ((s32)r0 >= (s32)0x4) goto L_8006CB5C;
                    goto L_8006CB50;
                    }
                r31 = 0x29f;
                goto L_8006CB5C;
                    }
            r31 = 0x2a1;
            goto L_8006CB5C;
        }
        r31 = 0x2a2;
        goto L_8006CB5C;
        L_8006CB50: ;
        r31 = 0x2a3;

                } else {
    r31 = 0x2a0;
                }
    L_8006CB5C: ;
    if (r31 == (u32)0x0) return;
    r3 = r31;
    ((void(*)(void))menuSpriteBiosGetPtr)();
    r0 = r3;
    r3 = r28;
    r4 = r0;
    ((void(*)(void))fn_80071318)();
    return;
    L_8006CB80: ;
    r31 = MENU_MIDDLE_U32_0064(r28)->unk_0064;
    r3 = 0x0;
    r4 = 0xe;
    savedataGetStatus();
    r4 = r30 * 0x1660;
    r0 = r4 + 0x64ec;
    r0 = r3 + r0;
    r3 = r0;
    heroBiosGetRnd();
    r7 = (0xcccd << 16);
    r6 = (0x51ec << 16);
    r0 = r3 & 0xFFFF;
    r5 = (0x1062 << 16);
    /* subi r9, r7, 0x3333 */;
    /* subi r7, r6, 0x7ae1 */;
    r8 = (u32)((u64)r9 * (u64)r0 >> 32);
    r4 = (0xd1b7 << 16);
    r6 = r5 + 0x4dd3;
    r5 = r4 + 0x1759;
    r4 = (u32)&lbl_802686D0;
    r3 = (u32)sp + 0x10;
    r7 = (u32)((u64)r7 * (u64)r0 >> 32);
    r29 = r8;
    r30 = (u32)r8 >> 3;
    r29 = (u32)r29 >> 3;
    r4 = (u32)&lbl_802686D0;
    r6 = (u32)((u64)r6 * (u64)r0 >> 32);
    r12 = (u32)r7 >> 5;
    r5 = (u32)((u64)r5 * (u64)r0 >> 32);
    r11 = (u32)r6 >> 6;
    r8 = (u32)((u64)r9 * (u64)r30 >> 32);
    r10 = (u32)r5 >> 13;
    r7 = (u32)((u64)r9 * (u64)r12 >> 32);
    r8 = (u32)r8 >> 3;
    r6 = (u32)((u64)r9 * (u64)r11 >> 32);
    r7 = (u32)r7 >> 3;
    r5 = (u32)((u64)r9 * (u64)r10 >> 32);
    r6 = (u32)r6 >> 3;
    r9 = r29 * 0xa;
    r5 = (u32)r5 >> 3;
    r0 = r0 - r9;
    r9 = r0 & 0xFF;
    r0 = r7 * 0xa;
    *(u8*)(sp + 0x8) = r9;
    r8 = r8 * 0xa;
    r0 = r12 - r0;
    r7 = r30 - r8;
    r8 = r7 & 0xFF;
    r7 = r0 & 0xFF;
    *(u8*)(sp + 0x9) = r8;
    r0 = r5 * 0xa;
    *(u8*)(sp + 0xA) = r7;
    r6 = r6 * 0xa;
    r0 = r10 - r0;
    r5 = r11 - r6;
    r6 = r5 & 0xFF;
    r5 = r0 & 0xFF;
    *(u8*)(sp + 0xB) = r6;
    *(u8*)(sp + 0xC) = r5;
    /* crclr cr1eq */;
    ((void(*)(void))sprintf)();
    r3 = (u32)sp + 0x90;
    r4 = (u32)sp + 0x10;
    ((void(*)(void))GScharMakeFromSJIS)();
    r4 = (u32)sp + 0x90;
    r3 = 0x37;
    msgctrlSetValue();
    r5 = r31;
    r3 = 0x0;
    r4 = 0x0;
    r6 = 0xd0;
    ((void(*)(void))fn_800FB680)();

    return;
}


/* 0x8006CCC0 | size: 0x890 */
void fn_8006CCC0(void* arg0, void* arg1) {
    extern void fn_8010B718();
    extern void pokemonBiosGetTamagoFlag();
    extern void pokemonBiosGetNicknamePtr();
    extern void pokemonCheckValid();
    extern void pokemonGetStatus();
    extern void heroBiosGetPokemonPtr();
    extern void heroBiosGetNamePtr();
    extern void msgctrlSetValue();
    extern void __assert();
    extern u8 jumptable_802EDFB0[];
    u8 sp[0x40];
    u32 r0;
    u32 r1 = (u32)sp;
    u32 r3 = (u32)arg0;
    u32 r4 = (u32)arg1;
    u32 r5;
    u32 r6;
    u32 r21;
    u32 r22;
    u32 r23;
    u32 r24;
    u32 r25;
    u32 r26;
    u32 r27;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 r31;
    void (*ctr_fn)(void);
    u32 ctr;

    
    r23 = (u32)arg0;
    r24 = (u32)arg1;
    r31 = 0x0;
    r30 = (u32)windowGetParam(arg0, 0);
    r21 = (u32)windowGetParam(arg0, 1);
    r29 = (u32)windowGetParam(arg0, 2);
    r22 = (u32)windowGetParam(arg0, 3);
    r0 = MENU_MIDDLE_U8_0001(r23)->unk_0001;
    r28 = 0x0;
    r27 = 0x0;
    r0 = (s8)r0;
    r26 = 0x0;
    r25 = 0x0;
    if ((s32)r0 >= (s32)0x4) return;
    if ((s32)r0 == (s32)0x0) return;
    if ((s32)r0 < (s32)0x0) {
        return;


    }
    r0 = MENU_MIDDLE_S16_0006(r24)->unk_0006;
    if ((s32)r0 != (s32)0xf2c) {
        if ((s32)r0 < (s32)0xf2c) {
            if ((s32)r0 != (s32)0xe79) {
                if ((s32)r0 < (s32)0xe79) {
                    if ((s32)r0 != (s32)0xe61) {
                        if ((s32)r0 < (s32)0xe61) {
                            if ((s32)r0 != (s32)0xe5e) {
                                if ((s32)r0 < (s32)0xe5e) {
                                    if ((s32)r0 != (s32)0xe5c) {
                                        if ((s32)r0 < (s32)0xe5c) {
                                            goto L_8006D2C8;
                                        }
                                        if ((s32)r0 < (s32)0xe60) {
                                            goto L_8006D054;
                                        }
                                        if ((s32)r0 == (s32)0xe76) goto L_8006D090;
                                        if ((s32)r0 < (s32)0xe76) {
                                            if ((s32)r0 == (s32)0xe74) goto L_8006D078;
                                            if ((s32)r0 >= (s32)0xe74) goto L_8006D084;
                                            goto L_8006D2C8;
                                        }
                                        if ((s32)r0 >= (s32)0xe78) goto L_8006D0A8;
                                        goto L_8006D09C;
                                    }
                                    if ((s32)r0 != (s32)0xe80) {
                                        if ((s32)r0 < (s32)0xe80) {
                                            if ((s32)r0 == (s32)0xe7d) goto L_8006D0E4;
                                            if ((s32)r0 < (s32)0xe7d) {
                                                if ((s32)r0 == (s32)0xe7b) goto L_8006D0CC;
                                                if ((s32)r0 >= (s32)0xe7b) goto L_8006D0D8;
                                                goto L_8006D0C0;
                                            }
                                            if ((s32)r0 >= (s32)0xe7f) goto L_8006D0FC;
                                            goto L_8006D0F0;
                                        }
                                        if ((s32)r0 != (s32)0xe84) {
                                            if ((s32)r0 < (s32)0xe84) {
                                                if ((s32)r0 != (s32)0xe82) {
                                                    if ((s32)r0 < (s32)0xe82) {
                                                        goto L_8006CFA4;
                                                    }
                                                }
                                                if ((s32)r0 >= (s32)0xe86) goto L_8006D2C8;
                                                goto L_8006D024;
                                            }
                                            if ((s32)r0 == (s32)0xf4a) goto L_8006D198;
                                            if ((s32)r0 < (s32)0xf4a) {
                                                if ((s32)r0 == (s32)0xf44) goto L_8006D150;
                                                if ((s32)r0 < (s32)0xf44) {
                                                    if ((s32)r0 == (s32)0xf30) goto L_8006D138;
                                                    if ((s32)r0 < (s32)0xf30) {
                                                        if ((s32)r0 == (s32)0xf2e) goto L_8006D120;
                                                        if ((s32)r0 >= (s32)0xf2e) goto L_8006D12C;
                                                        goto L_8006D114;
                                                    }
                                                    if ((s32)r0 < (s32)0xf32) {
                                                        goto L_8006D144;
                                                    }
                                                    if ((s32)r0 == (s32)0xf47) goto L_8006D174;
                                                    if ((s32)r0 < (s32)0xf47) {
                                                        if ((s32)r0 >= (s32)0xf46) goto L_8006D168;
                                                        goto L_8006D15C;
                                                    }
                                                    if ((s32)r0 >= (s32)0xf49) goto L_8006D18C;
                                                    goto L_8006D180;
                                                }
                                                if ((s32)r0 != (s32)0xfc9) {
                                                    if ((s32)r0 < (s32)0xfc9) {
                                                        if ((s32)r0 == (s32)0xf4e) goto L_8006D1C8;
                                                        if ((s32)r0 < (s32)0xf4e) {
                                                            if ((s32)r0 == (s32)0xf4c) goto L_8006D1B0;
                                                            if ((s32)r0 >= (s32)0xf4c) goto L_8006D1BC;
                                                            goto L_8006D1A4;
                                                        }
                                                    }
                                                    if ((s32)r0 >= (s32)0xf50) goto L_8006D2C8;
                                                    goto L_8006D1D4;
                                                }
                                                if ((s32)r0 == (s32)0x12bc) goto L_8006D1FC;
                                                if ((s32)r0 < (s32)0x12bc) {
                                                    if ((s32)r0 == (s32)0xfcb) goto L_8006D024;
                                                }
                                                if ((s32)r0 >= (s32)0xfcb) goto L_8006D2C8;
                                            }
                                            goto L_8006D1E0;
                                        }
                                                }
                                    if ((s32)r21 != (s32)0x0) {
                                        if ((s32)r21 < (s32)0x0) {
                                            goto L_8006CF18;
                                        }
                                        if ((s32)r21 >= (s32)0x3) goto L_8006CF18;

                                    } else {
                                    r0 = 0x3d8d;
                                    MENU_MIDDLE_U32_004C(r24)->unk_004C = r0;
                                    goto L_8006D2C8;
                                    }
                                    r0 = 0x3d8f;
                                    MENU_MIDDLE_U32_004C(r24)->unk_004C = r0;
                                    goto L_8006D2C8;
                                    L_8006CF18: ;
                                    r0 = 0x0;
                                    MENU_MIDDLE_U32_004C(r24)->unk_004C = r0;
                                    goto L_8006D2C8;
                                    }
                                if (r22 != (u32)0x0) {
                                    r0 = MENU_MIDDLE_U32_000C(r22)->unk_000C;
                                    if (r0 <= (u32)0x6) {
                                        r3 = (u32)jumptable_802EDFB0;
                                        r0 = r0 << 2;
                                        r3 = (u32)jumptable_802EDFB0;
                                        r0 = *(u32*)(r3 + r0);
                                        ctr_fn = (void(*)(void))r0;
                                        /* indirect jump via ctr */;
                                        r0 = 0x3d91;
                                        MENU_MIDDLE_U32_004C(r24)->unk_004C = r0;
                                }
                                }
                                goto L_8006D2C8;

                                MENU_MIDDLE_U32_004C(r24)->unk_004C = r0;
                                goto L_8006D2C8;

                                MENU_MIDDLE_U32_004C(r24)->unk_004C = r0;
                                goto L_8006D2C8;

                                MENU_MIDDLE_U32_004C(r24)->unk_004C = r0;
                                goto L_8006D2C8;

                                MENU_MIDDLE_U32_004C(r24)->unk_004C = r0;
                                goto L_8006D2C8;

                                MENU_MIDDLE_U32_004C(r24)->unk_004C = r0;
                                goto L_8006D2C8;

                                MENU_MIDDLE_U32_004C(r24)->unk_004C = r0;
                                goto L_8006D2C8;
                                L_8006CFA4: ;
                                if (r22 != (u32)0x0) {
                                    r0 = MENU_MIDDLE_U32_0004(r22)->unk_0004;
                                    if ((s32)r0 != (s32)0x1) {
                                        if ((s32)r0 >= (s32)0x1) goto L_8006CFE0;
                                        if ((s32)r0 < (s32)0x0) {
                                            goto L_8006CFE0;
                                        }
                                        r0 = 0x3d9d;
                                        MENU_MIDDLE_U32_004C(r24)->unk_004C = r0;
                                        goto L_8006CFF4;
                                    }
                                    r0 = 0x3d9e;
                                    MENU_MIDDLE_U32_004C(r24)->unk_004C = r0;
                                    goto L_8006CFF4;
                                    L_8006CFE0: ;
                                    r3 = (u32)&lbl_80268680;
                                    r4 = 0xb47;
                                    r3 = (u32)&lbl_80268680;
                                    r5 = (u32)&lbl_8047C064;
                                    __assert();
                                    L_8006CFF4: ;
                                    r3 = r23;
                                    r4 = 0xe80;
                                    ((void(*)(void))windowSearchItemID)();
                                    r22 = r3;
                                    r3 = MENU_MIDDLE_U32_004C(r22)->unk_004C;
                                    ((void(*)(void))GSmsgGetRect)();
                                    r0 = MENU_MIDDLE_S16_0050(r22)->unk_0050;
                                    r3 = (u32)r3 >> 16;
                                    r0 = r0 + r3;
                                    r0 = (s16)r0;
                                    MENU_MIDDLE_U16_0050(r24)->unk_0050 = r0;
                                }
                                goto L_8006D2C8;
                                L_8006D024: ;
                                r0 = 0x3d8b;
                                MENU_MIDDLE_U32_004C(r24)->unk_004C = r0;
                                goto L_8006D2C8;
                                            }
                            r31 = 0x1;
                            r28 = 0x0;
                            goto L_8006D2C8;
                                            }
                        r31 = 0x1;
                        r28 = 0x1;
                        goto L_8006D2C8;
                                }
                    r31 = 0x1;
                    r28 = 0x2;
                    goto L_8006D2C8;
                    L_8006D054: ;
                    r31 = 0x1;
                    r28 = 0x3;
                    goto L_8006D2C8;
                                        }
                r31 = 0x1;
                r28 = 0x4;
                goto L_8006D2C8;
                    }
            r31 = 0x1;
            r28 = 0x5;
            goto L_8006D2C8;
            L_8006D078: ;
            r31 = 0x2;
            r28 = 0x0;
            goto L_8006D2C8;
            L_8006D084: ;
            r31 = 0x2;
            r28 = 0x1;
            goto L_8006D2C8;
            L_8006D090: ;
            r31 = 0x2;
            r28 = 0x2;
            goto L_8006D2C8;
            L_8006D09C: ;
            r31 = 0x2;
            r28 = 0x3;
            goto L_8006D2C8;
            L_8006D0A8: ;
            r31 = 0x2;
            r28 = 0x4;
            goto L_8006D2C8;
            }
        r31 = 0x2;
        r28 = 0x5;
        goto L_8006D2C8;
        L_8006D0C0: ;
        r31 = 0x3;
        r28 = 0x0;
        goto L_8006D2C8;
        L_8006D0CC: ;
        r31 = 0x3;
        r28 = 0x1;
        goto L_8006D2C8;
        L_8006D0D8: ;
        r31 = 0x3;
        r28 = 0x2;
        goto L_8006D2C8;
        L_8006D0E4: ;
        r31 = 0x3;
        r28 = 0x3;
        goto L_8006D2C8;
        L_8006D0F0: ;
        r31 = 0x3;
        r28 = 0x4;
        goto L_8006D2C8;
        L_8006D0FC: ;
        r31 = 0x3;
        r28 = 0x5;
        goto L_8006D2C8;
    }
    r31 = 0x1;
    r28 = 0x0;
    goto L_8006D2C8;
    L_8006D114: ;
    r31 = 0x1;
    r28 = 0x1;
    goto L_8006D2C8;
    L_8006D120: ;
    r31 = 0x1;
    r28 = 0x2;
    goto L_8006D2C8;
    L_8006D12C: ;
    r31 = 0x1;
    r28 = 0x3;
    goto L_8006D2C8;
    L_8006D138: ;
    r31 = 0x1;
    r28 = 0x4;
    goto L_8006D2C8;
    L_8006D144: ;
    r31 = 0x1;
    r28 = 0x5;
    goto L_8006D2C8;
    L_8006D150: ;
    r31 = 0x2;
    r28 = 0x0;
    goto L_8006D2C8;
    L_8006D15C: ;
    r31 = 0x2;
    r28 = 0x1;
    goto L_8006D2C8;
    L_8006D168: ;
    r31 = 0x2;
    r28 = 0x2;
    goto L_8006D2C8;
    L_8006D174: ;
    r31 = 0x2;
    r28 = 0x3;
    goto L_8006D2C8;
    L_8006D180: ;
    r31 = 0x2;
    r28 = 0x4;
    goto L_8006D2C8;
    L_8006D18C: ;
    r31 = 0x2;
    r28 = 0x5;
    goto L_8006D2C8;
    L_8006D198: ;
    r31 = 0x3;
    r28 = 0x0;
    goto L_8006D2C8;
    L_8006D1A4: ;
    r31 = 0x3;
    r28 = 0x1;
    goto L_8006D2C8;
    L_8006D1B0: ;
    r31 = 0x3;
    r28 = 0x2;
    goto L_8006D2C8;
    L_8006D1BC: ;
    r31 = 0x3;
    r28 = 0x3;
    goto L_8006D2C8;
    L_8006D1C8: ;
    r31 = 0x3;
    r28 = 0x4;
    goto L_8006D2C8;
    L_8006D1D4: ;
    r31 = 0x3;
    r28 = 0x5;
    goto L_8006D2C8;
    L_8006D1E0: ;
    if (r30 != (u32)0x0) {
        r3 = r30;
        heroBiosGetNamePtr();
        r31 = 0x4;
        r25 = r3;
    }
    goto L_8006D2C8;
    L_8006D1FC: ;
    if (r22 != (u32)0x0) {
        r0 = MENU_MIDDLE_U32_0000(r22)->unk_0000;
        r21 = 0x0;
        if ((s32)r0 != (s32)0x1) {
            if ((s32)r0 >= (s32)0x1) goto L_8006D280;
            if ((s32)r0 < (s32)0x0) {
                goto L_8006D280;
            }
            r3 = MENU_MIDDLE_U32_0014(r22)->unk_0014;
            if ((s32)r3 != (s32)0x6) {
                if ((s32)r3 < (s32)0x6) {
                    if ((s32)r3 < (s32)0x0) {
                        goto L_8006D280;
                    }
                    if ((s32)r3 >= (s32)0x8) goto L_8006D280;
                    goto L_8006D268;
                    }
                r4 = r3 + 0x1;
                r3 = 0x2f;
                msgctrlSetValue();
                r21 = 0x3d9f;
                goto L_8006D280;
            }
            r21 = 0x3da0;
            goto L_8006D280;
            L_8006D268: ;
            r21 = 0x3da1;
            goto L_8006D280;
        }
        r4 = MENU_MIDDLE_U32_0014(r22)->unk_0014;
        r3 = 0x2f;
        msgctrlSetValue();
        r21 = 0x3da2;
        L_8006D280: ;
        r5 = MENU_MIDDLE_U32_0064(r24)->unk_0064;
        r6 = r21;
        r3 = 0x0;
        r4 = 0x0;
        ((void(*)(void))fn_800FB680)();
        r3 = r21;
        ((void(*)(void))GSmsgGetRect)();
        r3 = (u32)r3 >> 16;
        r4 = MENU_MIDDLE_U32_0018(r22)->unk_0018;
        r0 = r3 + 0x24;
        r3 = 0x2f;
        r21 = (s16)r0;
        msgctrlSetValue();
        r5 = MENU_MIDDLE_U32_0064(r24)->unk_0064;
        r3 = r21;
        r4 = 0x0;
        r6 = 0x3da4;
        ((void(*)(void))fn_800FB680)();
    }
    L_8006D2C8: ;
    if ((s32)r31 == (s32)0x0) return;
    r3 = r30;
    r4 = r28;
    heroBiosGetPokemonPtr();
    r28 = r3;
    ((void(*)(void))fn_80077A5C)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r28 = 0x0;

    } else {
    r3 = r28;
    pokemonCheckValid();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        r28 = 0x0;
        r26 = 0x1;
        r27 = 0x1;

    } else {
    r3 = r28;
    pokemonBiosGetTamagoFlag();
    r27 = r3;
    }
    }
    if ((s32)r31 != (s32)0x3) {
        if ((s32)r31 < (s32)0x3) {
            if ((s32)r31 != (s32)0x1) {
                if ((s32)r31 < (s32)0x1) {
                    return;
                }
                if ((s32)r31 >= (s32)0x5) return;
                goto L_8006D514;
                }
            if (r28 == (u32)0x0) return;
            r3 = r23;
            r4 = r24;
            r5 = r28;
            fn_8010B718();
            return;
                }
        r0 = r26 & 0xFF;
        if (r0 != (u32)0x0) {
            r5 = MENU_MIDDLE_U32_0064(r24)->unk_0064;
            r3 = 0x0;
            r4 = 0x0;
            r6 = 0x56c;
            ((void(*)(void))fn_800FB680)();
            return;
        }
        r0 = r27 & 0xFF;
        if (r0 != (u32)0x0) {
            r5 = MENU_MIDDLE_U32_0064(r24)->unk_0064;
            r3 = 0x0;
            r4 = 0x0;
            r6 = 0x56b;
            ((void(*)(void))fn_800FB680)();
            return;
        }
        if (r28 == (u32)0x0) return;
        r3 = r28;
        pokemonBiosGetNicknamePtr();
        if (r3 != (u32)0x0) {
            r0 = MENU_MIDDLE_U16_0000(r3)->unk_0000;
            if (r0 != (u32)0x0) {
                r4 = r3;
                r3 = 0x37;
                msgctrlSetValue();
                r5 = MENU_MIDDLE_U32_0064(r24)->unk_0064;
                r3 = 0x0;
                r4 = 0x0;
                r6 = 0xe7;
                ((void(*)(void))fn_800FB680)();
        }
        }
        r3 = r28;
        ((void(*)(void))menuSubGetPokemonSexForDisp)();
        r0 = r3 & 0xFF;
        if ((s32)r0 != (s32)0x1) {
            if ((s32)r0 < (s32)0x1) {
                if ((s32)r0 < (s32)0x0) {
                    goto L_8006D434;
                }
                goto L_8006D434;
                }
            r6 = 0xd67;
            goto L_8006D438;
        }
        r6 = 0xd68;
        goto L_8006D438;
        L_8006D434: ;
        r6 = 0x0;
        L_8006D438: ;
        if (r6 == (u32)0x0) return;
        r0 = MENU_MIDDLE_U8_0067(r24)->unk_0067;
        if (r0 != (u32)0xff) return;
        r5 = MENU_MIDDLE_U32_0064(r24)->unk_0064;
        r3 = 0x5c;
        r4 = 0x0;
        ((void(*)(void))fn_800FB680)();
        return;
    }
    if (r28 == (u32)0x0) return;
    r0 = r27 & 0xFF;
    if (r0 != (u32)0x0) return;
    r21 = MENU_MIDDLE_U32_0064(r24)->unk_0064;
    r3 = r28;
    r4 = 0x0;
    r5 = 0x7a;
    r6 = 0x0;
    pokemonGetStatus();
    r4 = r3;
    r3 = 0x2f;
    msgctrlSetValue();
    do {
    if (r29 == (u32)0x0) break;

    r3 = r28;
    r4 = r29;
    r5 = 0x0;
    ((void(*)(void))fn_800774D4)();
    r0 = r3 & 0xFF;
    do {
    if (r0 == (u32)0x0) break;

    r3 = r28;
    r4 = r29;
    r5 = 0x1;
    ((void(*)(void))fn_800774D4)();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) break;

    r3 = r30;
    r4 = r29;
    r5 = 0x0;
    ((void(*)(void))fn_80076F2C)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) break;

    } while (0);

    r21 = r21 | (0xff00 << 16);
    r21 = r21 & 0xFF0000FF;
    } while (0);

    r5 = r21;
    r3 = 0x0;
    r4 = 0x0;
    r6 = 0x41fa;
    ((void(*)(void))fn_800FB680)();
    return;
    L_8006D514: ;
    if (r25 == (u32)0x0) return;
    r4 = r25;
    r3 = 0x37;
    msgctrlSetValue();
    r5 = MENU_MIDDLE_U32_0064(r24)->unk_0064;
    r3 = 0x0;
    r4 = 0x0;
    r6 = 0xcf;
    ((void(*)(void))fn_800FB680)();

    return;
}


/* 0x8006D550 | size: 0x3F0 */
void fn_8006D550(void) {
    extern void fn_8006A7E8();
    extern void menuCBBios_ControlerIDtoPortID();
    extern void winSpriteGetDisp();
    extern void savedataGetStatus();
    extern void heroBiosGetSexDataId();
    extern void heroBiosGetRnd();
    extern void heroBiosGetNamePtr();
    extern void msgctrlSetValue();
    extern void __assert();
    extern u8 jumptable_802EDFCC[];
    u8 sp[0x1B0];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r31 = r4;
    r3 = r31;
    winSpriteGetDisp();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) return;
    r3 = MENU_MIDDLE_S16_0006(r31)->unk_0006;
    r28 = 0x0;
    /* subi r0, r3, 0xa4f */;
    if (r0 > (u32)0x27) return;
    r3 = (u32)jumptable_802EDFCC;
    r0 = r0 << 2;
    r3 = (u32)jumptable_802EDFCC;
    r0 = *(u32*)(r3 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    r30 = 0x2;
    r26 = 0x0;
    do {
    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;


    break;

    } while (0);
    r27 = 0x0;
    r29 = 0x0;
    do {
        r3 = 0x0;
        r4 = 0xe;
        savedataGetStatus();
        r0 = r29 + 0x59cc;
        r3 = *(u32*)(r3 + r0);
        menuCBBios_ControlerIDtoPortID();
        if ((s32)r26 == (s32)r3) {
            r3 = 0x0;
            r4 = 0xe;
            savedataGetStatus();
            r4 = r27 * 0x1660;
            r28 = r4 + 0x59a8;
            r28 = r3 + r28;
            break;
        }
        r29 = r29 + 0x1660;
        r27 = r27 + 0x1;
    } while (r27 < (u32)0x4);

    if (r28 == (u32)0x0) return;
    if ((s32)r30 != (s32)0x2) {
        if ((s32)r30 < (s32)0x2) {
            if ((s32)r30 < (s32)0x1) {
                goto L_8006D918;
            }
            if ((s32)r30 >= (s32)0x4) goto L_8006D918;
            goto L_8006D7F4;
            }
        r29 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
        r3 = r28 + 0xb44;
        heroBiosGetNamePtr();
        r4 = r3;
        r3 = 0x37;
        msgctrlSetValue();
        r5 = r29;
        r3 = 0x0;
        r4 = 0x0;
        r6 = 0xcf;
        ((void(*)(void))fn_800FB680)();
        return;
    }
    r31 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
    r3 = r28 + 0xb44;
    heroBiosGetRnd();
    r7 = (0xcccd << 16);
    r6 = (0x51ec << 16);
    r0 = r3 & 0xFFFF;
    r5 = (0x1062 << 16);
    /* subi r9, r7, 0x3333 */;
    /* subi r7, r6, 0x7ae1 */;
    r8 = (u32)((u64)r9 * (u64)r0 >> 32);
    r4 = (0xd1b7 << 16);
    r6 = r5 + 0x4dd3;
    r5 = r4 + 0x1759;
    r4 = (u32)&lbl_802686D0;
    r3 = (u32)sp + 0x10;
    r7 = (u32)((u64)r7 * (u64)r0 >> 32);
    r29 = r8;
    r30 = (u32)r8 >> 3;
    r29 = (u32)r29 >> 3;
    r4 = (u32)&lbl_802686D0;
    r6 = (u32)((u64)r6 * (u64)r0 >> 32);
    r12 = (u32)r7 >> 5;
    r5 = (u32)((u64)r5 * (u64)r0 >> 32);
    r11 = (u32)r6 >> 6;
    r8 = (u32)((u64)r9 * (u64)r30 >> 32);
    r10 = (u32)r5 >> 13;
    r7 = (u32)((u64)r9 * (u64)r12 >> 32);
    r8 = (u32)r8 >> 3;
    r6 = (u32)((u64)r9 * (u64)r11 >> 32);
    r7 = (u32)r7 >> 3;
    r5 = (u32)((u64)r9 * (u64)r10 >> 32);
    r6 = (u32)r6 >> 3;
    r9 = r29 * 0xa;
    r5 = (u32)r5 >> 3;
    r0 = r0 - r9;
    r9 = r0 & 0xFF;
    r0 = r7 * 0xa;
    *(u8*)(sp + 0x8) = r9;
    r8 = r8 * 0xa;
    r0 = r12 - r0;
    r7 = r30 - r8;
    r8 = r7 & 0xFF;
    r7 = r0 & 0xFF;
    *(u8*)(sp + 0x9) = r8;
    r0 = r5 * 0xa;
    *(u8*)(sp + 0xA) = r7;
    r6 = r6 * 0xa;
    r0 = r10 - r0;
    r5 = r11 - r6;
    r6 = r5 & 0xFF;
    r5 = r0 & 0xFF;
    *(u8*)(sp + 0xB) = r6;
    *(u8*)(sp + 0xC) = r5;
    /* crclr cr1eq */;
    ((void(*)(void))sprintf)();
    r3 = (u32)sp + 0x90;
    r4 = (u32)sp + 0x10;
    ((void(*)(void))GScharMakeFromSJIS)();
    r4 = (u32)sp + 0x90;
    r3 = 0x37;
    msgctrlSetValue();
    r5 = r31;
    r3 = 0x0;
    r4 = 0x0;
    r6 = 0xcf;
    ((void(*)(void))fn_800FB680)();
    return;
    L_8006D7F4: ;
    r3 = r28;
    r26 = 0x0;
    fn_8006A7E8();
    if ((s32)r3 != (s32)0x1) {
        if ((s32)r3 < (s32)0x1) {
            if ((s32)r3 < (s32)0x0) {
                goto L_8006D8A4;
            }
            if ((s32)r3 >= (s32)0x3) goto L_8006D8A4;
            goto L_8006D868;
            }
        r0 = 0x0;
        goto L_8006D8A8;
    }
    r3 = r28 + 0xb44;
    heroBiosGetSexDataId();
    r0 = r3 & 0xFF;
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 < (s32)0x1) {
            if ((s32)r0 < (s32)0x0) {
                goto L_8006D8A4;
            }
            goto L_8006D8A4;
            }
        r0 = 0x1;
        goto L_8006D8A8;
    }
    r0 = 0x2;
    goto L_8006D8A8;
    L_8006D868: ;
    r3 = r28 + 0xb44;
    heroBiosGetSexDataId();
    r0 = r3 & 0xFF;
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 < (s32)0x1) {
            if ((s32)r0 < (s32)0x0) {
                goto L_8006D8A4;
            }
            goto L_8006D8A4;
            }
        r0 = 0x3;
        goto L_8006D8A8;
    }
    r0 = 0x4;
    goto L_8006D8A8;
    L_8006D8A4: ;
    r0 = 0x1;
    L_8006D8A8: ;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 != (s32)0x0) {
                if ((s32)r0 < (s32)0x0) {
                    goto L_8006D8F8;
                }
                if ((s32)r0 != (s32)0x4) {
                    if ((s32)r0 >= (s32)0x4) goto L_8006D8F8;
                    goto L_8006D8EC;
                    }
                r26 = 0x2ba;
                goto L_8006D8F8;
                    }
            r26 = 0x2bc;
            goto L_8006D8F8;
        }
        r26 = 0x2b5;
        goto L_8006D8F8;
        L_8006D8EC: ;
        r26 = 0x2bb;

                } else {
    r26 = 0x2b4;
                }
    L_8006D8F8: ;
    if (r26 == (u32)0x0) return;
    r3 = r26;
    ((void(*)(void))menuSpriteBiosGetPtr)();
    r4 = r3;
    r3 = r31;
    ((void(*)(void))fn_80071318)();
    return;
    L_8006D918: ;
    r3 = (u32)&lbl_80268680;
    r4 = 0xae6;
    r3 = (u32)&lbl_80268680;
    r5 = (u32)&lbl_8047C064;
    __assert();

    return;
}


/* 0x8006D940 | size: 0x4C */
#pragma push
#pragma peephole off
void fn_8006D940(void* menu) {
    extern void* windowSearchItemID(void* menu, s32 itemId);
    void* value;
    void* target;

    value = windowGetParam(menu, 0);
    target = windowSearchItemID(menu, 0xE8E);
    MENU_MIDDLE_U32_004C(target)->unk_004C = (u32)value;
}
#pragma pop


/* 0x8006D98C | size: 0x158 */
void fn_8006D98C(void) {
    extern void fn_80070D84();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r4 = 0x0;
    ((void(*)(void))windowGetParam)();
    r3 = r3 & 0xFF;
    if (r3 != (u32)0x0) {
        r28 = 0x6;
    } else {

        r28 = 0x3;
    }
    r0 = MENU_MIDDLE_U8_0001(r31)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) goto L_8006DAC0;
        if ((s32)r0 != (s32)0x0) {
            goto L_8006DAC0;
        }
        r0 = MENU_MIDDLE_U8_0002(r31)->unk_0002;
        r0 = (s8)r0;
        if ((s32)r0 == (s32)0x0) {
            if (r3 != (u32)0x0) {
                r0 = 0x152;
            } else {

                r0 = 0x0;
            }
            r0 = (s16)r0;
            r3 = (u32)&lbl_8026864C;
            MENU_MIDDLE_U16_0084(r31)->unk_0084 = r0;
            r30 = (u32)&lbl_8026864C;
            r27 = 0x0;
            do {
                r4 = MENU_MIDDLE_U16_0000(r30)->unk_0000;
                r3 = r31;
                r29 = MENU_MIDDLE_U32_0004(r30)->unk_0004;
                ((void(*)(void))windowSearchItemID)();
                MENU_MIDDLE_U32_004C(r3)->unk_004C = r29;
                r30 = r30 + 0x8;
                r27 = r27 + 0x1;
            } while (r27 < (u32)0x5);
            r3 = (u32)&lbl_80267EA8;
            r27 = MENU_MIDDLE_U32_001C(r31)->unk_001C;
            r29 = r28 << 2;
            r30 = (u32)&lbl_80267EA8;
            while (r27 != (u32)0x0) {

                r0 = MENU_MIDDLE_S16_0006(r27)->unk_0006;
                r3 = r31;
                r5 = *(u16*)(r30 + r29);
                r4 = r0 & 0xFFFF;
                ((void(*)(void))fn_801081F8)();
                r27 = MENU_MIDDLE_U32_0000(r27)->unk_0000;

            }
        }

    } else {
    r0 = MENU_MIDDLE_U8_0002(r31)->unk_0002;
    r0 = (s8)r0;
    if ((s32)r0 == (s32)0x0) {
        r3 = (u32)&lbl_80267EA8;
        r4 = r28 << 2;
        r0 = (u32)&lbl_80267EA8;
        r27 = MENU_MIDDLE_U32_001C(r31)->unk_001C;
        r3 = r0 + r4;
        r29 = r3 + 0x2;
        while (r27 != (u32)0x0) {

            r0 = MENU_MIDDLE_S16_0006(r27)->unk_0006;
            r3 = r31;
            r5 = MENU_MIDDLE_U16_0000(r29)->unk_0000;
            r4 = r0 & 0xFFFF;
            ((void(*)(void))fn_801081F8)();
            r27 = MENU_MIDDLE_U32_0000(r27)->unk_0000;

        }
    }
    }
    L_8006DAC0: ;
    r3 = r31;
    r4 = 0x0;
    r5 = 0x0;
    fn_80070D84();
    return;
}


/* 0x8006DAE4 | size: 0x144 */
#pragma peephole off
void fn_8006DAE4(void* arg0) {
    extern void* windowGetParam(void*, int);
    extern void* windowSearchItemID(void*, int);
    extern void menuSetPosition(int, int, s16);
    extern u32 GSmsgGetRect(void*);
    extern void winSpriteSetDisp(void*, int);
    extern void fn_80070D84(void*, void*, int);

    typedef struct {
        u8 unk0;
        s8 unk1;
        s8 unk2;
    } arg_state_DAE4;

    typedef struct {
        u8 unk0[0x4c];
        u32 field_4c;
        u8 pad50[2];
        s16 field_52;
    } slot_obj_DAE4;

    void* a;
    void* b;
    void* c;
    slot_obj_DAE4* slot;
    s32 d29;
    s32 d27;
    s16 cmp;

    if (((arg_state_DAE4*)arg0)->unk2 == 0) {
        switch (((arg_state_DAE4*)arg0)->unk1) {
        case 0:
        {
            a = windowGetParam(arg0, 0);
            b = windowGetParam(arg0, 1);
            c = windowGetParam(arg0, 2);
            menuSetPosition(0xd6, 0, (s16)(s32)windowGetParam(arg0, 3));
            slot = (slot_obj_DAE4*)windowSearchItemID(arg0, 0xe8c);
            slot->field_4c = (u32)a;
            slot = (slot_obj_DAE4*)windowSearchItemID(arg0, 0xe8d);
            slot->field_4c = (u32)b;
            cmp = (s16)(u16)GSmsgGetRect(c);
            d29 = 0;
            d27 = 0;
            if (cmp > 0x32) {
                d29 = -0x14;
                d27 = -0xa;
            }
            slot = (slot_obj_DAE4*)windowSearchItemID(arg0, 0xe8a);
            slot->field_4c = (u32)c;
            slot->field_52 = slot->field_52 + d29;
            slot = (slot_obj_DAE4*)windowSearchItemID(arg0, 0xe87);
            winSpriteSetDisp(slot, c != 0);
            slot->field_52 = slot->field_52 + d27;
            break;
        }
        }
    }
    fn_80070D84(arg0, lbl_8026860C, 8);
}
#pragma peephole reset


/* 0x8006DC28 | size: 0x4A4 */
void fn_8006DC28(void) {
    extern void fn_80070D84();
    extern void winSpriteSetDisp();
    extern void fn_8010B01C();
    extern void pokemonBiosGetItemDataId();
    extern void pokemonIsDarkPokemon();
    extern void pokemonCheckValid();
    extern void heroBiosGetPokemonPtr();
    extern u32 fn_8006E128(u8* p);
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
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

    
    r29 = r3;
    r4 = 0x0;
    ((void(*)(void))windowGetParam)();
    r0 = r3;
    r3 = r29;
    r30 = r0;
    r4 = 0x1;
    ((void(*)(void))windowGetParam)();
    r3 = r29;
    r4 = 0x2;
    ((void(*)(void))windowGetParam)();
    r0 = MENU_MIDDLE_U8_0002(r29)->unk_0002;
    r31 = r3;
    r0 = (s8)r0;
    do {
    if ((s32)r0 != (s32)0x0) break;
    r0 = MENU_MIDDLE_U8_0001(r29)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) break;
        if ((s32)r0 != (s32)0x0) {
            if ((s32)r0 < (s32)0x0) {
                break;
            }
            r25 = 0x0;
            r26 = r25;
            r3 = (u32)&lbl_803B6D68;
            r23 = (u32)&lbl_803B6D68;
            do {
                r3 = r30;
                r4 = r26 & 0xFFFF;
                heroBiosGetPokemonPtr();
                r24 = r3;
                pokemonCheckValid();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x0) {
                    r0 = r25;
                    r25 = r25 + 0x1;
                    r0 = r0 << 2;
                    *(u32*)(r23 + r0) = r24;
                }
                r26 = r26 + 0x1;
            } while ((s32)r26 < (s32)0x6);
            r4 = (u32)&lbl_803B6D68;
            r0 = r25 << 2;
            r5 = (u32)&lbl_803B6D68;
            r6 = 0x0;
            *(u32*)(r5 + r0) = r6;
            r3 = (u32)fn_8006E128;
            r4 = (u32)fn_8006E128;
            MENU_MIDDLE_U32_001C(r5)->unk_001C = r6;
            r3 = 0x0;
            fn_8010B01C();
            r3 = (u32)&lbl_8026858C;
            r26 = 0x0;
            r27 = (u32)&lbl_8026858C;
            r3 = (0x2aab << 16);
            /* subi r28, r3, 0x5555 */;
            do {
                r4 = (s32)((s64)r28 * (s64)r26 >> 32);
                r3 = r30;
                r0 = (u32)r4 >> 31;
                r0 = r4 + r0;
                r0 = r0 * 0x6;
                r0 = r26 - r0;
                r4 = r0 & 0xFFFF;
                heroBiosGetPokemonPtr();
                r4 = 0x0;
                r25 = r3;
                ((void(*)(void))fn_80076398)();
                r0 = r3 & 0xFF;
                r23 = 0x0;
                r0 = __cntlzw(r0);
                r0 = (u32)r0 >> 5;
                r24 = r0 & 0xFF;
                if (r24 == (u32)0x0) {
                    r3 = r25;
                    ((void(*)(void))menuCBRule_CheckPokemonErrorAll)();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x0) {
                    }
                    r23 = 0x1;
                    }
                r23 = r23 & 0xFF;
                if (r31 != (u32)0x0) {
                    if (r25 != (u32)0x0) {
                        r3 = r25;
                        pokemonCheckValid();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x0) {
                            r3 = r25;
                            r4 = r31;
                            ((void(*)(void))fn_800772AC)();
                            r0 = r3 & 0xFF;
                            r0 = __cntlzw(r0);
                            r0 = (u32)r0 >> 5;
                            r0 = r23 | r0;
                            r23 = r0 & 0xFF;
                    }
                    }
                    r3 = r30;
                    r4 = r25;
                    r5 = r31;
                    r6 = 0x1;
                    ((void(*)(void))fn_80076A8C)();
                    r0 = r3 & 0xFF;
                    r3 = r30;
                    r0 = __cntlzw(r0);
                    r4 = r25;
                    r0 = (u32)r0 >> 5;
                    r5 = r31;
                    r0 = r23 | r0;
                    r6 = 0x2;
                    r23 = r0 & 0xFF;
                    ((void(*)(void))fn_80076A8C)();
                    r0 = r3 & 0xFF;
                    r3 = r30;
                    r0 = __cntlzw(r0);
                    r4 = r25;
                    r0 = (u32)r0 >> 5;
                    r5 = r31;
                    r0 = r23 | r0;
                    r6 = 0x3;
                    r23 = r0 & 0xFF;
                    ((void(*)(void))fn_80076A8C)();
                    r0 = r3 & 0xFF;
                    r0 = __cntlzw(r0);
                    r0 = (u32)r0 >> 5;
                    r0 = r23 | r0;
                    r23 = r0 & 0xFF;
                }
                r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
                r3 = r29;
                ((void(*)(void))windowSearchItemID)();
                r22 = r3;
                do {
                if (r22 == (u32)0x0) break;
                if (r24 != (u32)0x0) {
                    r3 = 0x375;
                    ((void(*)(void))menuSpriteBiosGetPtr)();
                    r4 = r3;
                    r3 = r22;
                    ((void(*)(void))fn_80071318)();
                    break;
                }
                r0 = r23 & 0xFF;
                if (r0 == (u32)0x0) break;
                r3 = 0x25b;
                ((void(*)(void))menuSpriteBiosGetPtr)();
                r4 = r3;
                r3 = r22;
                ((void(*)(void))fn_80071318)();
                } while (0);
                r4 = MENU_MIDDLE_U16_0002(r27)->unk_0002;
                r3 = r29;
                ((void(*)(void))windowSearchItemID)();
                r24 = r3;
                if (r24 != (u32)0x0) {
                    r0 = r23 & 0xFF;
                    r22 = 0x274;
                    if (r0 != (u32)0x0) {
                        r22 = 0x25c;
                    }
                    r3 = r25;
                    pokemonIsDarkPokemon();
                    r0 = r3 & 0xFF;
                    if (r0 != (u32)0x0) {
                        r22 = 0x341;
                    }
                    r3 = r22;
                    ((void(*)(void))menuSpriteBiosGetPtr)();
                    r4 = r3;
                    r3 = r24;
                    ((void(*)(void))fn_80071318)();
                }
                r4 = MENU_MIDDLE_U16_0004(r27)->unk_0004;
                r3 = r29;
                ((void(*)(void))windowSearchItemID)();
                if (r3 != (u32)0x0) {
                    r4 = r23;
                    winSpriteSetDisp();
                }
                r27 = r27 + 0x8;
                r26 = r26 + 0x1;
            } while (r26 < (u32)0xc);
            r3 = (u32)&lbl_80267EA8;
            r24 = MENU_MIDDLE_U32_001C(r29)->unk_001C;
            r3 = (u32)&lbl_80267EA8;
            r23 = MENU_MIDDLE_U16_000C(r3)->unk_000C;
            while (r24 != (u32)0x0) {

                r4 = r23;
                r3 = r24 + 0xc;
                ((void(*)(void))winSetSequence)();
                r24 = MENU_MIDDLE_U32_0000(r24)->unk_0000;

            }
            break;
            }
        r3 = (u32)&lbl_8026858C;
        r23 = 0x0;
        r27 = (u32)&lbl_8026858C;
        do {
            r4 = MENU_MIDDLE_U16_0006(r27)->unk_0006;
            r3 = r29;
            ((void(*)(void))windowSearchItemID)();
            r28 = r3;
            do {
            if (r28 == (u32)0x0) break;

            r4 = (0x2aab << 16);
            r3 = r30;
            /* subi r0, r4, 0x5555 */;
            r4 = (s32)((s64)r0 * (s64)r23 >> 32);
            r0 = (u32)r4 >> 31;
            r0 = r4 + r0;
            r0 = r0 * 0x6;
            r0 = r23 - r0;
            r4 = r0 & 0xFFFF;
            heroBiosGetPokemonPtr();
            r22 = r3;
            do {
            if (r22 == (u32)0x0) break;

            pokemonCheckValid();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x0) break;

            r3 = r22;
            pokemonBiosGetItemDataId();
            r24 = 0x0;
            r25 = r3;
            ((void(*)(void))menuCBRule_CheckValidItem)();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x0) {
                r3 = r25;
                ((void(*)(void))fn_80077C68)();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x0) {
                    r24 = 0x1;
            }
            }
            r24 = r24 & 0xFF;
            if (r31 != (u32)0x0) {
                r3 = r30;
                r4 = r22;
                r5 = r31;
                r6 = 0x2;
                ((void(*)(void))fn_80076A8C)();
                r0 = r24 & r3;
                r24 = r0 & 0xFF;
            }
            r4 = r25 & 0xFFFF;
            r3 = r28;
            r0 = -r4;
            r0 = r0 | r4;
            r4 = (u32)r0 >> 31;
            winSpriteSetDisp();
            r0 = r24 & 0xFF;
            r3 = MENU_MIDDLE_U32_0064(r28)->unk_0064;
            r3 = r3 & 0xFF;
            if (r0 != (u32)0x0) {
                r0 = -0x100;
            } else {

                r0 = (0xff00 << 16);
            }
            r0 = r3 | r0;
            MENU_MIDDLE_U32_0064(r28)->unk_0064 = r0;
            break;

            } while (0);

            r3 = r28;
            r4 = 0x0;
            winSpriteSetDisp();
            } while (0);

            r27 = r27 + 0x8;
            r23 = r23 + 0x1;
        } while (r23 < (u32)0xc);
        break;
    }
    r3 = (u32)&lbl_80267EA8;
    r22 = MENU_MIDDLE_U32_001C(r29)->unk_001C;
    r3 = (u32)&lbl_80267EA8;
    r23 = MENU_MIDDLE_U16_001A(r3)->unk_001A;
    while (r22 != (u32)0x0) {

        r4 = r23;
        r3 = r22 + 0xc;
        ((void(*)(void))winSetSequence)();
        r22 = MENU_MIDDLE_U32_0000(r22)->unk_0000;

    }
    } while (0);
    r3 = r29;
    r4 = 0x0;
    r5 = 0x0;
    fn_80070D84();
    return;
}


/* 0x8006E0CC | size: 0x5C */
#pragma peephole off
void fn_8006E0CC(void) {
    extern void fn_8010BBB8(void);
    extern s8 fn_8010BCE4(void);
    extern void _threadSwitch(void);
    u32* entry = (u32*)&lbl_803B6D68;

    goto check;
    do {
        fn_8010BBB8();
        if (fn_8010BCE4() == 0) {
            _threadSwitch();
        } else {
            entry = (u32*)((u8*)entry + 4);
        }
    check:
        ;
    } while (MENU_MIDDLE_U32_0000(entry)->unk_0000 != 0);
}
#pragma peephole reset


/* 0x8006E128 | size: 0x38 */
u32 fn_8006E128(u8* p) {
    u32 index;

    if (p == NULL) {
        return 0;
    }
    index = *(u32*)(p + 0x1C);
    if (index >= 7) {
        return 0;
    }
    *(u32*)(p + 0x1C) = index + 1;
    return *(u32*)(p + index * 4);
}


/* 0x8006E160 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006E160(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006E188 | size: 0x4 */
void fn_8006E188(void) {
}

/* 0x8006E18C | size: 0xCC */
#pragma push
#pragma peephole off
void fn_8006E18C(void* menu) {
    extern void fn_80070D84(void* menu, s32 arg1, s32 arg2);
    extern void winSpriteSetDisp(void* widget, s32 flag);
    extern void* windowSearchItemID(void* menu, s32 itemId);
    extern s32 fn_80071160(void);
    extern void fn_80107F38(s32 param, u32 key);
    typedef struct Entry_8006E18C {
        u16 itemId;
        u8 threshold;
        u8 pad3;
    } Entry_8006E18C;

    Entry_8006E18C* entry = (Entry_8006E18C*)&lbl_80268574;
    void* widget;
    s32 flags;
    u32 i;

    i = 0;
    do {
        s32 diff;
        s32 threshold;
        u32 active;

        widget = windowSearchItemID(menu, entry->itemId);
        threshold = MENU_MIDDLE_U8_0095(menu)->unk_0095;
        threshold = (s8)threshold;
        diff = entry->threshold - threshold;
        active = (u32)__cntlzw(diff) >> 5;
        active = active & 0xFF;
        winSpriteSetDisp(widget, active);
        entry++;
        i++;
    } while (i < 6);

    flags = MENU_MIDDLE_U8_0001(menu)->unk_0001;
    flags = (s8)flags;
    if (flags == 2) {
        if (fn_80071160() != 0) {
            fn_80107F38(MENU_MIDDLE_U32_0004(menu)->unk_0004, 0x1CE);
            MENU_MIDDLE_U8_0098(menu)->unk_0098 = 1;
            MENU_MIDDLE_U8_0099(menu)->unk_0099 = 1;
            return;
        }
    }

    fn_80070D84(menu, 0, 0);
}
#pragma pop


/* 0x8006E258 | size: 0xE0 */
#pragma peephole off
void fn_8006E258(void* menu) {
    extern s32 menuCBBios_ControlerIDtoPortID(u32 flags);
    extern void* windowSearchItemID(void* menu, s32 itemId);
    extern void winSpriteSetDisp(void* widget, s32 flag);
    extern u8* savedataGetStatus(s32 idx, s32 type);
    void* widget[5];
    u16* itemIds;
    s32 portId;
    u32 slot;
    s32 active;
    u32 i;

    portId = menuCBBios_ControlerIDtoPortID(MENU_MIDDLE_U32_59CC(savedataGetStatus(0, 0xE))->unk_59CC);
    itemIds = (u16*)&lbl_80268560;
    slot = 0;
    do {
        void* w;

        active = (u32)__cntlzw(portId - slot) >> 5;

        i = 0;
        do {
            w = windowSearchItemID(menu, itemIds[i]);
            winSpriteSetDisp(w, active);
            widget[i] = w;
            i++;
        } while (i < 5);

        {
            u32 val;
            if (active != 0) {
                val = 0x424B;
            } else {
                val = 0;
            }
            MENU_MIDDLE_U32_004C(widget[4])->unk_004C = val;

            if (active != 0) {
                val = 0x3F40;
            } else {
                val = 0;
            }
            MENU_MIDDLE_U32_004C(widget[3])->unk_004C = val;
        }

        itemIds += 5;
        slot++;
    } while (slot < 2);
}
#pragma peephole reset


/* 0x8006E338 | size: 0x460 */
void fn_8006E338(void) {
    extern void fn_8006A7E8();
    extern void menuCBBios_ControlerIDtoPortID();
    extern void fn_80070D84();
    extern void winSpriteSetDisp();
    extern void savedataGetStatus();
    extern void __assert();
    u8 sp[0xB0];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
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

    
    r30 = r3;
    r0 = MENU_MIDDLE_U8_0002(r30)->unk_0002;
    r4 = (u32)&lbl_80267EA8;
    r3 = *(u32*)&lbl_8047E708;
    r28 = (u32)&lbl_80267EA8;
    r0 = (s8)r0;
    r31 = 0x1;
    do {
    if ((s32)r0 != (s32)0x0) break;
    r0 = MENU_MIDDLE_U8_0001(r30)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) break;
        if ((s32)r0 != (s32)0x0) {
            break;
        }
        r26 = r28 + 0x698;
        r27 = r28 + 0x628;
        r21 = 0x0;
        r25 = r28 + 0x0;
        do {
            r0 = MENU_MIDDLE_U32_0000(r26)->unk_0000;
            r24 = r27;
            r22 = 0x0;
            r23 = r0 << 2;
            do {
                r4 = MENU_MIDDLE_U16_0000(r24)->unk_0000;
                r3 = r30;
                ((void(*)(void))windowSearchItemID)();
                r4 = *(u16*)(r25 + r23);
                r3 = r3 + 0xc;
                ((void(*)(void))winSetSequence)();
                r24 = r24 + 0x2;
                r22 = r22 + 0x1;
            } while (r22 < (u32)0xe);
            r26 = r26 + 0x4;
            r27 = r27 + 0x1c;
            r21 = r21 + 0x1;
        } while (r21 < (u32)0x4);
        break;
    }
    r26 = r28 + 0x698;
    r27 = r28 + 0x628;
    r22 = 0x0;
    r23 = r28 + 0x0;
    do {
        r0 = MENU_MIDDLE_U32_0000(r26)->unk_0000;
        r24 = r27;
        r21 = 0x0;
        r3 = r0 << 2;
        r25 = r3 + 0x2;
        do {
            r4 = MENU_MIDDLE_U16_0000(r24)->unk_0000;
            r3 = r30;
            ((void(*)(void))windowSearchItemID)();
            r4 = *(u16*)(r23 + r25);
            r3 = r3 + 0xc;
            ((void(*)(void))winSetSequence)();
            r24 = r24 + 0x2;
            r21 = r21 + 0x1;
        } while (r21 < (u32)0xe);
        r26 = r26 + 0x4;
        r27 = r27 + 0x1c;
        r22 = r22 + 0x1;
    } while (r22 < (u32)0x4);
    } while (0);
    r24 = 0x0;
    r26 = r28 + 0x6a8;
    r29 = r24;
    r27 = (u32)&lbl_8047C058;
    do {
        r3 = 0x0;
        r4 = 0xe;
        savedataGetStatus();
        r0 = r29 + 0x59cc;
        r3 = *(u32*)(r3 + r0);
        menuCBBios_ControlerIDtoPortID();
        if ((s32)r3 >= (s32)0x0) {
            r0 = r3 * 0x1c;
            r23 = r28 + 0x628;
            r4 = (u32)sp + 0x8;
            r5 = 0x1;
            *(u8*)(r4 + r3) = r5;
            r23 = r23 + r0;
            r25 = (u32)sp + 0x44;
            r21 = 0x0;
            do {
                r4 = MENU_MIDDLE_U16_0000(r23)->unk_0000;
                r3 = r30;
                ((void(*)(void))windowSearchItemID)();
                MENU_MIDDLE_U32_0000(r25)->unk_0000 = r3;
                r23 = r23 + 0x2;
                r25 = r25 + 0x4;
                r21 = r21 + 0x1;
            } while (r21 < (u32)0xe);
            r3 = 0x0;
            r4 = 0xe;
            savedataGetStatus();
            r0 = r29 + 0x59a8;
            r3 = r3 + r0;
            fn_8006A7E8();
            if ((s32)r3 != (s32)0x0) {
                r3 = 0x2b2;
            } else {

                r3 = 0x2ae;
            }
            ((void(*)(void))menuSpriteBiosGetPtr)();
            r0 = r3;
            r4 = r0;
            ((void(*)(void))fn_80071318)();
            r3 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
            ((void(*)(void))menuSpriteBiosGetPtr)();
            r4 = r3;
            ((void(*)(void))fn_80071318)();
            r3 = 0x0;
            r5 = MENU_MIDDLE_U32_0000(r26)->unk_0000;
            r4 = 0xe;
            r0 = MENU_MIDDLE_U32_0064(r6)->unk_0064;
            r0 = r0 & 0xFF;
            r0 = r0 | r5;
            MENU_MIDDLE_U32_0064(r6)->unk_0064 = r0;
            r0 = MENU_MIDDLE_U32_0064(r8)->unk_0064;
            r0 = r0 & 0xFF;
            r0 = r0 | r5;
            MENU_MIDDLE_U32_0064(r8)->unk_0064 = r0;
            r0 = MENU_MIDDLE_U32_0064(r7)->unk_0064;
            r0 = r0 & 0xFF;
            r0 = r0 | r5;
            MENU_MIDDLE_U32_0064(r7)->unk_0064 = r0;
            savedataGetStatus();
            r0 = r29 + 0x7005;
            r0 = *(u8*)(r3 + r0);
            r31 = r31 & r0;
        }
        r29 = r29 + 0x1660;
        r27 = r27 + 0x2;
        r26 = r26 + 0x4;
        r24 = r24 + 0x1;
    } while ((s32)r24 < (s32)0x4);
    r0 = r31 & 0xFF;
    r26 = r28 + 0x628;
    r0 = __cntlzw(r0);
    r27 = (u32)sp + 0x8;
    r29 = (u32)r0 >> 5;
    r24 = 0x0;
    r25 = (u32)sp + 0xc;
    do {
        r23 = r26;
        r22 = r25;
        r21 = 0x0;
        do {
            r4 = MENU_MIDDLE_U16_0000(r23)->unk_0000;
            r3 = r30;
            ((void(*)(void))windowSearchItemID)();
            MENU_MIDDLE_U32_0000(r22)->unk_0000 = r3;
            r23 = r23 + 0x2;
            r22 = r22 + 0x4;
            r21 = r21 + 0x1;
        } while (r21 < (u32)0xe);
        r0 = MENU_MIDDLE_U8_0000(r27)->unk_0000;
        if (r0 != (u32)0x0) {
            r23 = 0x0;
            r22 = r23;
            L_8006E5E8: ;
            r3 = 0x0;
            r4 = 0xe;
            savedataGetStatus();
            r0 = r22 + 0x59cc;
            r3 = *(u32*)(r3 + r0);
            menuCBBios_ControlerIDtoPortID();
            if ((s32)r24 != (s32)r3) {
                r22 = r22 + 0x1660;
                r23 = r23 + 0x1;
                if ((s32)r23 < (s32)0x4) goto L_8006E5E8;
            }
            if (r23 >= (u32)0x4) {
                r3 = r28 + 0x7d8;
                r5 = r28 + 0x83c;
                r4 = 0x8a1;
                __assert();
            }
            r3 = 0x0;
            r4 = 0xe;
            savedataGetStatus();
            r4 = r23 * 0x1660;
            r0 = r4 + 0x7005;
            r21 = *(u8*)(r3 + r0);
            if (r21 != (u32)0x0) {
                r0 = 0x0;
            } else {

                r0 = 0x3f3f;
            }
            MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
            if (r21 != (u32)0x0) {
                r0 = 0x0;
            } else {

                r0 = 0x3f40;
            }
            r4 = r29 & 0xFF;
            MENU_MIDDLE_U32_004C(r5)->unk_004C = r0;
            winSpriteSetDisp();
            r0 = __cntlzw(r21);
            r23 = (u32)r0 >> 5;
            r4 = r23 & 0xFF;
            winSpriteSetDisp();
            r4 = r23 & 0xFF;
            winSpriteSetDisp();
            r0 = 0x0;
            if ((r21 != (u32)0x0) && ((s32)r29 != (s32)0x0)) {

                r0 = 0x1;
            }
            r4 = r0 & 0xFF;
            winSpriteSetDisp();

        } else {
        r22 = r25;
        r23 = 0x0;
        do {
            r3 = MENU_MIDDLE_U32_0000(r22)->unk_0000;
            r4 = 0x0;
            winSpriteSetDisp();
            r22 = r22 + 0x4;
            r23 = r23 + 0x1;
        } while (r23 < (u32)0xe);
        }
        r26 = r26 + 0x1c;
        r27 = r27 + 0x1;
        r24 = r24 + 0x1;
    } while (r24 < (u32)0x4);
    r3 = 0x0;
    r4 = 0xe;
    savedataGetStatus();
    r0 = MENU_MIDDLE_U32_59CC(r3)->unk_59CC;
    if ((s32)r0 != (s32)0x1) {
        r3 = 0x2ae;
        ((void(*)(void))menuSpriteBiosGetPtr)();
        r4 = r28 + 0x628;
        r21 = r3;
        r4 = MENU_MIDDLE_U16_0008(r4)->unk_0008;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r4 = r21;
        r21 = r3;
        ((void(*)(void))fn_80071318)();
        r3 = r21;
        r4 = 0x1;
        winSpriteSetDisp();
        r4 = r28 + 0x628;
        r3 = r30;
        r4 = MENU_MIDDLE_U16_000C(r4)->unk_000C;
        ((void(*)(void))windowSearchItemID)();
        r4 = 0x1;
        winSpriteSetDisp();
    }
    r3 = r30;
    r4 = 0x0;
    r5 = 0x0;
    fn_80070D84();
    MENU_MIDDLE_U8_0098(r30)->unk_0098 = r31;
    return;
}


/* 0x8006E798 | size: 0x20C */
void fn_8006E798(void) {
    extern void fn_8006A7E8();
    extern void fn_80070D84();
    extern void winSpriteSetDisp();
    extern void savedataGetStatus();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r30 = r3;
    r0 = MENU_MIDDLE_U8_0002(r30)->unk_0002;
    r3 = (u32)&lbl_80267EA8;
    r31 = (u32)&lbl_80267EA8;
    r0 = (s8)r0;
    do {
    if ((s32)r0 != (s32)0x0) break;
    r0 = MENU_MIDDLE_U8_0001(r30)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) break;
        if ((s32)r0 != (s32)0x0) {
            break;
        }
        r28 = r31 + 0x5f4;
        r3 = r31 + 0x0;
        r27 = 0x0;
        r29 = MENU_MIDDLE_U16_0018(r3)->unk_0018;
        do {
            r4 = MENU_MIDDLE_U16_0000(r28)->unk_0000;
            r3 = r30;
            ((void(*)(void))windowSearchItemID)();
            r4 = r29;
            r3 = r3 + 0xc;
            ((void(*)(void))winSetSequence)();
            r28 = r28 + 0x2;
            r27 = r27 + 0x1;
        } while (r27 < (u32)0x7);
        break;
    }
    r28 = r31 + 0x5f4;
    r3 = r31 + 0x0;
    r27 = 0x0;
    r29 = MENU_MIDDLE_U16_001A(r3)->unk_001A;
    do {
        r4 = MENU_MIDDLE_U16_0000(r28)->unk_0000;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r4 = r29;
        r3 = r3 + 0xc;
        ((void(*)(void))winSetSequence)();
        r28 = r28 + 0x2;
        r27 = r27 + 0x1;
    } while (r27 < (u32)0x7);
    } while (0);
    r3 = 0x0;
    r4 = 0xe;
    savedataGetStatus();
    r0 = MENU_MIDDLE_U32_0004(r3)->unk_0004;
    do {
        if ((s32)r0 == (s32)0x2 || (s32)r0 >= (s32)0x2) break;

        if ((s32)r0 < (s32)0x0) {
            break;
        }
        r27 = 0x0;
        goto L_8006E88C;
    } while (0);
    r27 = 0x2;
    L_8006E88C: ;
    r3 = 0x0;
    r4 = 0xe;
    savedataGetStatus();
    r3 = r3 + 0x59a8;
    fn_8006A7E8();
    if ((s32)r3 != (s32)0x0) {
        r0 = 0x1;
    } else {

        r0 = 0x0;
    }
    r3 = r30;
    r27 = r27 + r0;
    r4 = 0x99b;
    ((void(*)(void))windowSearchItemID)();
    r0 = r27 << 3;
    r28 = r31 + 0x5d4;
    r29 = 0x0;
    r27 = r3;
    r28 = r28 + r0;
    do {
        r4 = MENU_MIDDLE_U8_0000(r28)->unk_0000;
        r3 = r27;
        winSpriteSetDisp();
        r27 = MENU_MIDDLE_U32_0000(r27)->unk_0000;
        r28 = r28 + 0x1;
        r29 = r29 + 0x1;
    } while ((s32)r29 < (s32)0x8);
    r3 = r30;
    r4 = 0x9a7;
    ((void(*)(void))windowSearchItemID)();
    r0 = 0x3d2c;
    r4 = 0x9a9;
    MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
    r3 = r30;
    ((void(*)(void))windowSearchItemID)();
    r0 = 0x3d26;
    r4 = 0x9a6;
    MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
    r3 = r30;
    ((void(*)(void))windowSearchItemID)();
    r29 = r3;
    r3 = 0x0;
    r4 = 0xe;
    savedataGetStatus();
    r0 = MENU_MIDDLE_U32_0008(r3)->unk_0008;
    r5 = r31 + 0x604;
    r3 = r30;
    r4 = 0x9a8;
    r0 = r0 << 2;
    r0 = *(u32*)(r5 + r0);
    MENU_MIDDLE_U32_004C(r29)->unk_004C = r0;
    ((void(*)(void))windowSearchItemID)();
    r29 = r3;
    r3 = 0x0;
    r4 = 0xe;
    savedataGetStatus();
    r0 = MENU_MIDDLE_U32_0004(r3)->unk_0004;
    r6 = r31 + 0x61c;
    r3 = r30;
    r4 = 0x0;
    r0 = r0 << 2;
    r5 = 0x0;
    r0 = *(u32*)(r6 + r0);
    MENU_MIDDLE_U32_004C(r29)->unk_004C = r0;
    fn_80070D84();
    return;
}


/* 0x8006E9A4 | size: 0x4D8 */
void fn_8006E9A4(void) {
    extern void fn_8006B3C8();
    extern void winSpriteGetDisp();
    extern void msgctrlSetValue();
    u8 sp[0x910];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r30 = r3;
    r31 = r4;
    r0 = MENU_MIDDLE_U8_0001(r30)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 == (s32)0x0) return;


    r3 = r31;
    winSpriteGetDisp();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) return;
    r3 = r30;
    r4 = 0x0;
    ((void(*)(void))windowGetParam)();
    r0 = MENU_MIDDLE_S16_0006(r31)->unk_0006;
    r5 = 0x0;
    if ((s32)r0 != (s32)0xd8e) {
        if ((s32)r0 < (s32)0xd8e) {
            if ((s32)r0 != (s32)0x969) {
                if ((s32)r0 < (s32)0x969) {
                    if ((s32)r0 != (s32)0x966) {
                        if ((s32)r0 < (s32)0x966) {
                            if ((s32)r0 != (s32)0x964) {
                                if ((s32)r0 < (s32)0x964) {
                                    goto L_8006EE30;
                                }
                                if ((s32)r0 < (s32)0x968) {
                                    goto L_8006ECA0;
                                }
                                if ((s32)r0 != (s32)0xa0f) {
                                    if ((s32)r0 < (s32)0xa0f) {
                                        if ((s32)r0 < (s32)0xa0e) {
                                            goto L_8006EE30;
                                        }
                                        if ((s32)r0 < (s32)0xd8d) {
                                            goto L_8006EE30;
                                        }
                                        if ((s32)r0 != (s32)0xd94) {
                                            if ((s32)r0 < (s32)0xd94) {
                                                if ((s32)r0 != (s32)0xd91) {
                                                    if ((s32)r0 < (s32)0xd91) {
                                                        if ((s32)r0 < (s32)0xd90) {
                                                            goto L_8006EAF8;
                                                        }
                                                        if ((s32)r0 >= (s32)0xd93) goto L_8006EB48;
                                                        goto L_8006EB40;
                                                    }
                                                    if ((s32)r0 == (s32)0xda0) goto L_8006EBFC;
                                                    if ((s32)r0 < (s32)0xda0) {
                                                        if ((s32)r0 >= (s32)0xd96) goto L_8006EE30;
                                                        goto L_8006EB88;
                                                    }
                                                    if ((s32)r0 >= (s32)0xda2) goto L_8006EE30;
                                                    goto L_8006EBD0;
                                                    }
                                                r4 = (0x51ec << 16);
                                                r0 = MENU_MIDDLE_S16_0000(r3)->unk_0000;
                                                /* subi r3, r4, 0x7ae1 */;
                                                r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                                r0 = (s32)r0 >> 5;
                                                r3 = (u32)r0 >> 31;
                                                r5 = r0 + r3;
                                                goto L_8006EE30;
                                            }
                                            r4 = (0x6666 << 16);
                                            r0 = MENU_MIDDLE_S16_0000(r3)->unk_0000;
                                            r3 = r4 + 0x6667;
                                            r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                            r0 = (s32)r0 >> 2;
                                            r3 = (u32)r0 >> 31;
                                            r5 = r0 + r3;
                                            goto L_8006EE30;
                                            L_8006EAF8: ;
                                            r5 = MENU_MIDDLE_S16_0000(r3)->unk_0000;
                                            goto L_8006EE30;
                                                        }
                                        r4 = (0x51ec << 16);
                                        r0 = MENU_MIDDLE_S16_0002(r3)->unk_0002;
                                        /* subi r3, r4, 0x7ae1 */;
                                        r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                        r0 = (s32)r0 >> 5;
                                        r3 = (u32)r0 >> 31;
                                        r5 = r0 + r3;
                                        goto L_8006EE30;
                                                }
                                    r4 = (0x6666 << 16);
                                    r0 = MENU_MIDDLE_S16_0002(r3)->unk_0002;
                                    r3 = r4 + 0x6667;
                                    r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                    r0 = (s32)r0 >> 2;
                                    r3 = (u32)r0 >> 31;
                                    r5 = r0 + r3;
                                    goto L_8006EE30;
                                    L_8006EB40: ;
                                    r5 = MENU_MIDDLE_S16_0002(r3)->unk_0002;
                                    goto L_8006EE30;
                                    L_8006EB48: ;
                                    r4 = (0x51ec << 16);
                                    r0 = MENU_MIDDLE_S16_0004(r3)->unk_0004;
                                    /* subi r3, r4, 0x7ae1 */;
                                    r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                    r0 = (s32)r0 >> 5;
                                    r3 = (u32)r0 >> 31;
                                    r5 = r0 + r3;
                                    goto L_8006EE30;
                                        }
                                r4 = (0x6666 << 16);
                                r0 = MENU_MIDDLE_S16_0004(r3)->unk_0004;
                                r3 = r4 + 0x6667;
                                r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                r0 = (s32)r0 >> 2;
                                r3 = (u32)r0 >> 31;
                                r5 = r0 + r3;
                                goto L_8006EE30;
                                L_8006EB88: ;
                                r5 = MENU_MIDDLE_S16_0004(r3)->unk_0004;
                                goto L_8006EE30;
                                    }
                            r0 = MENU_MIDDLE_S16_0014(r3)->unk_0014;
                            r3 = (0x6666 << 16);
                            r4 = r3 + 0x6667;
                            r3 = (s32)r0 >> 31;
                            r0 = r3 ^ r0;
                            r0 = r0 - r3;
                            r0 = (s32)((s64)r4 * (s64)r0 >> 32);
                            r0 = (s32)r0 >> 2;
                            r3 = (u32)r0 >> 31;
                            r5 = r0 + r3;
                            goto L_8006EE30;
                                        }
                        r3 = MENU_MIDDLE_S16_0014(r3)->unk_0014;
                        r0 = (s32)r3 >> 31;
                        r5 = r0 ^ r3;
                        r5 = r5 - r0;
                        goto L_8006EE30;
                        L_8006EBD0: ;
                        r0 = MENU_MIDDLE_S16_0016(r3)->unk_0016;
                        r3 = (0x6666 << 16);
                        r4 = r3 + 0x6667;
                        r3 = (s32)r0 >> 31;
                        r0 = r3 ^ r0;
                        r0 = r0 - r3;
                        r0 = (s32)((s64)r4 * (s64)r0 >> 32);
                        r0 = (s32)r0 >> 2;
                        r3 = (u32)r0 >> 31;
                        r5 = r0 + r3;
                        goto L_8006EE30;
                        L_8006EBFC: ;
                        r3 = MENU_MIDDLE_S16_0016(r3)->unk_0016;
                        r0 = (s32)r3 >> 31;
                        r5 = r0 ^ r3;
                        r5 = r5 - r0;
                        goto L_8006EE30;
                            }
                    r31 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
                    r3 = (u32)sp + 0x288;
                    r4 = (u32)&lbl_8047C068;
                    r5 = 0x32;
                    /* crclr cr1eq */;
                    ((void(*)(void))sprintf)();
                    r3 = (u32)sp + 0x808;
                    r4 = (u32)sp + 0x288;
                    ((void(*)(void))GScharMakeFromSJIS)();
                    r4 = (u32)sp + 0x808;
                    r3 = 0x37;
                    msgctrlSetValue();
                    r5 = r31;
                    r3 = 0xa;
                    r4 = 0x0;
                    r6 = 0xcf;
                    ((void(*)(void))fn_800FB680)();
                    return;
                                }
                r31 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
                r3 = (u32)sp + 0x208;
                r4 = (u32)&lbl_8047C068;
                r5 = 0x32;
                /* crclr cr1eq */;
                ((void(*)(void))sprintf)();
                r3 = (u32)sp + 0x708;
                r4 = (u32)sp + 0x208;
                ((void(*)(void))GScharMakeFromSJIS)();
                r4 = (u32)sp + 0x708;
                r3 = 0x37;
                msgctrlSetValue();
                r5 = r31;
                r3 = 0xa;
                r4 = 0x0;
                r6 = 0xcf;
                ((void(*)(void))fn_800FB680)();
                return;
                L_8006ECA0: ;
                r31 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
                r3 = (u32)sp + 0x188;
                r4 = (u32)&lbl_8047C068;
                r5 = 0x32;
                /* crclr cr1eq */;
                ((void(*)(void))sprintf)();
                r3 = (u32)sp + 0x608;
                r4 = (u32)sp + 0x188;
                ((void(*)(void))GScharMakeFromSJIS)();
                r4 = (u32)sp + 0x608;
                r3 = 0x37;
                msgctrlSetValue();
                r5 = r31;
                r3 = 0xa;
                r4 = 0x0;
                r6 = 0xcf;
                ((void(*)(void))fn_800FB680)();
                return;
                            }
            r31 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
            r3 = 0x3;
            fn_8006B3C8();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x0) {
                r5 = 0x32;
            } else {

                r5 = -0x1;
            }
            if ((s32)r5 >= (s32)0x0) {
                r3 = (u32)sp + 0x108;
                r4 = (u32)&lbl_8047C068;
                /* crclr cr1eq */;
                ((void(*)(void))sprintf)();
            } else {

                r3 = (u32)sp + 0x108;
                r4 = (u32)&lbl_8047C070;
                /* crclr cr1eq */;
                ((void(*)(void))sprintf)();
            }
            r3 = (u32)sp + 0x508;
            r4 = (u32)sp + 0x108;
            ((void(*)(void))GScharMakeFromSJIS)();
            r4 = (u32)sp + 0x508;
            r3 = 0x37;
            msgctrlSetValue();
            r5 = r31;
            r3 = 0xa;
            r4 = 0x0;
            r6 = 0xcf;
            ((void(*)(void))fn_800FB680)();
            return;
                                }
        r31 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
        r3 = (u32)sp + 0x88;
        r4 = (u32)&lbl_8047C068;
        r5 = 0x64;
        /* crclr cr1eq */;
        ((void(*)(void))sprintf)();
        r3 = (u32)sp + 0x408;
        r4 = (u32)sp + 0x88;
        ((void(*)(void))GScharMakeFromSJIS)();
        r4 = (u32)sp + 0x408;
        r3 = 0x37;
        msgctrlSetValue();
        r5 = r31;
        r3 = 0xa;
        r4 = 0x0;
        r6 = 0xcf;
        ((void(*)(void))fn_800FB680)();
        return;
                            }
    r31 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
    r3 = 0x5;
    fn_8006B3C8();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r5 = 0x64;
    } else {

        r5 = -0x1;
    }
    if ((s32)r5 >= (s32)0x0) {
        r3 = (u32)sp + 0x8;
        r4 = (u32)&lbl_8047C068;
        /* crclr cr1eq */;
        ((void(*)(void))sprintf)();
    } else {

        r3 = (u32)sp + 0x8;
        r4 = (u32)&lbl_8047C070;
        /* crclr cr1eq */;
        ((void(*)(void))sprintf)();
    }
    r3 = (u32)sp + 0x308;
    r4 = (u32)sp + 0x8;
    ((void(*)(void))GScharMakeFromSJIS)();
    r4 = (u32)sp + 0x308;
    r3 = 0x37;
    msgctrlSetValue();
    r5 = r31;
    r3 = 0xa;
    r4 = 0x0;
    r6 = 0xcf;
    ((void(*)(void))fn_800FB680)();
    return;
    L_8006EE30: ;
    r4 = (0xcccd << 16);
    r3 = 0x34;
    /* subi r0, r4, 0x3333 */;
    r0 = (u32)((u64)r0 * (u64)r5 >> 32);
    r0 = (u32)r0 >> 3;
    r0 = r0 * 0xa;
    r4 = r5 - r0;
    msgctrlSetValue();
    r5 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
    r3 = 0x0;
    r4 = 0x0;
    r6 = 0xc9;
    ((void(*)(void))fn_800FB680)();

    return;
}


/* 0x8006EE7C | size: 0xA8 */
#pragma peephole off
void fn_8006EE7C(void* menu) {
    typedef struct MenuState_8006EE7C {
        u8 pad0;
        u8 state;
        u8 pad2[0x92];
        s8 cursor;
        s8 offset;
        u8 pad96[2];
        u8 dirty;
    } MenuState_8006EE7C;

    MenuState_8006EE7C* state;
    KeyInfo_8006BB34* keyInfo;
    u8* params;
    u8 toggled;
    s32 index;
    u32 value;

    state = menu;
    value = state->state;
    value = (s8)value;
    if ((s32)value != 2) {
        if (((!menu) && (!menu)) && (!menu)) {
            /* Preserve MWCC register allocation. */
        }
    } else {
        keyInfo = windowGetKeyInfo();
        value = keyInfo->flags4;
        value = value & 0x10;
        toggled = value == 0;
        if ((s32)value != 0) {
            index = state->offset + state->cursor;
            if (index < 60) {
                params = windowGetParam(menu, 0);
                value = params[index];
                toggled = value == 0;
                params[index] = toggled;
                state->dirty = 0;
                return;
            }
        }
    }

    menuButtonNormal(menu);
}
#pragma peephole reset


/* 0x8006EF24 | size: 0xD4 */
typedef struct MenuState_8006EF24 {
    u8 pad0;
    s8 state;
    u8 pad2[0x92];
    s8 cursor;
    s8 mode;
} MenuState_8006EF24;

#pragma peephole off
void fn_8006EF24(void* menu) {
    MenuState_8006EF24* state = (MenuState_8006EF24*)menu;
    KeyInfo_8006BB34* keyInfo;
    s32 currentState;

    currentState = state->state;
    if (currentState != 2) {
        if ((state && state) && state) {
            /* Preserve MWCC register allocation. */
        }
        goto normal;
    }

    if (state->mode == 0) {
        keyInfo = (KeyInfo_8006BB34*)windowGetKeyInfo();
        if (keyInfo->flags6 & 1) {
            state->cursor--;
            if (state->cursor < 0) {
                state->cursor = 0;
            }
            return;
        }
    } else if (state->mode == 10) {
        keyInfo = (KeyInfo_8006BB34*)windowGetKeyInfo();
        if (keyInfo->flags6 & 2) {
            state->cursor++;
            if (state->cursor > 50) {
                state->cursor = 50;
            }
            return;
        }
    }

normal:
    menuCursorNormal(menu);
}
#pragma peephole reset


/* 0x8006EFF8 | size: 0x28C */
void fn_8006EFF8(void) {
    extern void winSpriteSetDisp();
    extern void fn_80142984();
    extern void itemDataBiosGetName();
    extern void itemDataBiosGetPtr();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
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

    
    r31 = r3;
    r4 = 0x0;
    ((void(*)(void))windowGetParam)();
    r28 = r3;
    r3 = r31;
    r4 = 0xa40;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_0094(r31)->unk_0094;
    r4 = (s8)r0;
    r0 = -r4;
    r0 = r0 & ~r4;
    r4 = (u32)r0 >> 31;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0xa3f;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_0094(r31)->unk_0094;
    r5 = 0x32;
    r0 = (s8)r0;
    r0 = r5 ^ r0;
    r4 = (s32)r0 >> 1;
    r0 = r0 & r5;
    r0 = r4 - r0;
    r4 = (u32)r0 >> 31;
    winSpriteSetDisp();
    r3 = (u32)&lbl_80268424;
    r25 = 0x0;
    r27 = (u32)&lbl_80268424;
    do {
        r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
        r3 = r31;
        ((void(*)(void))windowSearchItemID)();
        r4 = MENU_MIDDLE_U16_0002(r27)->unk_0002;
        r29 = r3;
        r3 = r31;
        ((void(*)(void))windowSearchItemID)();
        r4 = MENU_MIDDLE_U16_0004(r27)->unk_0004;
        r30 = r3;
        r3 = r31;
        ((void(*)(void))windowSearchItemID)();
        r4 = MENU_MIDDLE_U16_0006(r27)->unk_0006;
        r24 = r3;
        r3 = r31;
        ((void(*)(void))windowSearchItemID)();
        r0 = MENU_MIDDLE_U8_0094(r31)->unk_0094;
        r23 = r3;
        r0 = (s8)r0;
        r26 = r25 + r0;
        if ((s32)r26 == (s32)0x3c) {
            r3 = 0x43e6;
            r0 = 0x0;
            MENU_MIDDLE_U32_004C(r29)->unk_004C = r3;
            MENU_MIDDLE_U32_004C(r30)->unk_004C = r0;

        } else {
        r3 = r26;
        ((void(*)(void))fn_80077D88)();
        r22 = r3;
        fn_80142984();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x0) {
            r3 = r22;
            itemDataBiosGetPtr();
            itemDataBiosGetName();
            MENU_MIDDLE_U32_004C(r29)->unk_004C = r3;
        } else {

            r0 = 0x12e;
            MENU_MIDDLE_U32_004C(r29)->unk_004C = r0;
        }
        r0 = *(u8*)(r28 + r26);
        if (r0 != (u32)0x0) {
            if (r0 != (u32)0x0) {
                r0 = 0x3d6f;
            } else {

                r0 = 0x3d68;
            }
            r3 = (0xff00 << 16);
            MENU_MIDDLE_U32_004C(r30)->unk_004C = r0;
            r0 = r3 + 0xff;
            MENU_MIDDLE_U32_0064(r30)->unk_0064 = r0;

        } else {
        if (r0 != (u32)0x0) {
            r0 = 0x3d6f;
        } else {

            r0 = 0x3d68;
        }
        MENU_MIDDLE_U32_004C(r30)->unk_004C = r0;
        r0 = -0x1;
        MENU_MIDDLE_U32_0064(r30)->unk_0064 = r0;
        }
        }
        r4 = *(u8*)(r28 + r26);
        r3 = r24;
        winSpriteSetDisp();
        r0 = MENU_MIDDLE_U8_0095(r31)->unk_0095;
        r3 = r23;
        r0 = (s8)r0;
        r0 = r0 - r25;
        r0 = __cntlzw(r0);
        r0 = (u32)r0 >> 5;
        r4 = r0 & 0xFF;
        winSpriteSetDisp();
        r27 = r27 + 0x8;
        r25 = r25 + 0x1;
    } while (r25 < (u32)0xb);
    r0 = MENU_MIDDLE_U8_0001(r31)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) return;
        if ((s32)r0 != (s32)0x0) {
            return;


        }
        r0 = MENU_MIDDLE_U8_0002(r31)->unk_0002;
        r0 = (s8)r0;
        if ((s32)r0 != (s32)0x0) return;
        r3 = (u32)&lbl_80267EA8;
        r24 = MENU_MIDDLE_U32_001C(r31)->unk_001C;
        r3 = (u32)&lbl_80267EA8;
        r23 = MENU_MIDDLE_U16_0018(r3)->unk_0018;
        while (r24 != (u32)0x0) {

            r0 = MENU_MIDDLE_S16_0006(r24)->unk_0006;
            r3 = r31;
            r5 = r23;
            r4 = r0 & 0xFFFF;
            ((void(*)(void))fn_801081F8)();
            r24 = MENU_MIDDLE_U32_0000(r24)->unk_0000;

        }
        r3 = r31;
        r23 = 0x424a;
        r4 = 0xe4c;
        ((void(*)(void))windowSearchItemID)();
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r23;
        return;
    }
    r0 = MENU_MIDDLE_U8_0002(r31)->unk_0002;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x0) return;
    r3 = (u32)&lbl_80267EA8;
    r24 = MENU_MIDDLE_U32_001C(r31)->unk_001C;
    r3 = (u32)&lbl_80267EA8;
    r23 = MENU_MIDDLE_U16_001A(r3)->unk_001A;
    while (r24 != (u32)0x0) {

        r0 = MENU_MIDDLE_S16_0006(r24)->unk_0006;
        r3 = r31;
        r5 = r23;
        r4 = r0 & 0xFFFF;
        ((void(*)(void))fn_801081F8)();
        r24 = MENU_MIDDLE_U32_0000(r24)->unk_0000;

    }
    r0 = 0x1;
    MENU_MIDDLE_U8_0002(r31)->unk_0002 = r0;

    return;
}


/* 0x8006F284 | size: 0x49C */
void fn_8006F284(void) {
    extern void fn_80070D84();
    extern void winSpriteSetDisp();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r0 = MENU_MIDDLE_U8_0001(r31)->unk_0001;
    r3 = (u32)&lbl_80267EA8;
    r30 = (u32)&lbl_80267EA8;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 != (s32)0x0) {
                goto L_8006F370;
            }
            goto L_8006F370;
            }
        r26 = r30 + 0x4ec;
        r25 = 0x0;
        do {
            r4 = MENU_MIDDLE_U16_0000(r26)->unk_0000;
            r3 = r31;
            ((void(*)(void))windowSearchItemID)();
            r0 = MENU_MIDDLE_U32_0004(r26)->unk_0004;
            r26 = r26 + 0x8;
            r25 = r25 + 0x1;
            MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
        } while (r25 < (u32)0x10);
        ((void(*)(void))fn_80077BD0)();
        r0 = r3 & 0xFF;
        r3 = 0x9fc;
        r0 = __cntlzw(r0);
        r0 = (u32)r0 >> 5;
        r25 = r0 & 0xFF;
        r4 = r25;
        ((void(*)(void))menuItemBiosSetSelectFlag)();
        if (r25 == (u32)0x0) {
            r3 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
            r4 = 0x9fd;
            ((void(*)(void))menuGetCursorFromItemID)();
            r3 = (s8)r3;
            r0 = 0x0;
            *(u8*)(sp + 0xD) = r3;
            r4 = (u32)sp + 0x8;
            *(u8*)(sp + 0xC) = r0;
            r0 = *(u16*)(sp + 0xC);
            *(u16*)(sp + 0x8) = r0;
            r3 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
            ((void(*)(void))fn_801044D0)();
            r3 = r31;
            ((void(*)(void))windowCreateCursorSprite)();
        }

    } else {
    r3 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
    ((void(*)(void))menuGetCursorItemID)();
    if ((s32)r3 == (s32)0xe35) {
        r0 = 0x1;
        MENU_MIDDLE_U8_0098(r31)->unk_0098 = r0;
    }
    }
    L_8006F370: ;
    r3 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
    ((void(*)(void))menuGetCursorItemID)();
    if ((s32)r3 < (s32)0x9fe) {
        if ((s32)r3 != (s32)0x9f9) {
            if ((s32)r3 < (s32)0x9f9) {
                if ((s32)r3 != (s32)0x9f7) {
                    if ((s32)r3 < (s32)0x9f7) {
                        goto L_8006F400;
                    }
                    if ((s32)r3 == (s32)0x9fb) goto L_8006F3F8;
                    if ((s32)r3 < (s32)0x9fb) {
                        goto L_8006F3F0;
                    }
                    if ((s32)r3 < (s32)0xe33) {
                        if ((s32)r3 < (s32)0xa0e) {
                            if ((s32)r3 >= (s32)0xa0c) goto L_8006F3F0;
                    }
                    }
                    goto L_8006F400;
                }
                if ((s32)r3 == (s32)0xe35 || (s32)r3 >= (s32)0xe35) goto L_8006F400;

                goto L_8006F3F8;
                    }
            r25 = 0x0;
            goto L_8006F404;
                    }
        r25 = 0x1;
        goto L_8006F404;
        }
    r25 = 0x2;
    goto L_8006F404;
    L_8006F3F0: ;
    r25 = 0x3;
    goto L_8006F404;
    L_8006F3F8: ;
    r25 = 0x4;
    goto L_8006F404;
    L_8006F400: ;
    r25 = 0x5;
    L_8006F404: ;
    r27 = r30 + 0x4e0;
    r28 = 0x0;
    do {
        r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
        r3 = r31;
        ((void(*)(void))windowSearchItemID)();
        r29 = r3;
        r26 = 0x0;
        if ((s32)r25 == (s32)r28) {
            ((void(*)(void))fn_80077BD0)();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x0) {
                r26 = 0x1;
        }
        }
        r4 = r26 & 0xFF;
        r3 = r29;
        winSpriteSetDisp();
        r3 = 0x191;
        ((void(*)(void))menuSeqBiosGetPtr)();
        r0 = MENU_MIDDLE_U32_000C(r29)->unk_000C;
        if (r0 != (u32)r3) {
            r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
            r3 = r31;
            r5 = 0x191;
            ((void(*)(void))fn_801081F8)();
        }
        r27 = r27 + 0x2;
        r28 = r28 + 0x1;
    } while (r28 < (u32)0x5);
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    r3 = r31;
    r0 = __cntlzw(r0);
    r4 = 0x0;
    r0 = (u32)r0 >> 5;
    r29 = r0 & 0xFF;
    ((void(*)(void))windowGetParam)();
    r5 = MENU_MIDDLE_S16_0014(r3)->unk_0014;
    r4 = 0xa06;
    r0 = MENU_MIDDLE_S16_0016(r3)->unk_0016;
    r5 = (u32)r5 >> 31;
    r25 = MENU_MIDDLE_U8_0011(r3)->unk_0011;
    r0 = (u32)r0 >> 31;
    r26 = MENU_MIDDLE_U8_0012(r3)->unk_0012;
    r5 = r5 ^ 0x1;
    r24 = MENU_MIDDLE_U8_0013(r3)->unk_0013;
    r0 = r0 ^ 0x1;
    r3 = r31;
    r28 = r5 & 0xFF;
    r27 = r0 & 0xFF;
    ((void(*)(void))windowSearchItemID)();
    r4 = r25;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0xa07;
    ((void(*)(void))windowSearchItemID)();
    r0 = __cntlzw(r25);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0xa08;
    ((void(*)(void))windowSearchItemID)();
    r4 = r26;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0xa09;
    ((void(*)(void))windowSearchItemID)();
    r0 = __cntlzw(r26);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0xa0a;
    ((void(*)(void))windowSearchItemID)();
    r4 = r24;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0xa0b;
    ((void(*)(void))windowSearchItemID)();
    r0 = __cntlzw(r24);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0x11a2;
    ((void(*)(void))windowSearchItemID)();
    r25 = r3;
    r26 = 0x0;
    if (r28 != (u32)0x0) {
        r3 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
        ((void(*)(void))menuGetCursorItemID)();
        if ((s32)r3 != (s32)0xa0c) {
            r3 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
            ((void(*)(void))menuGetCursorItemID)();
            if ((s32)r3 != (s32)0xa0d) {
                r26 = 0x1;
    }
    }
    }
    r4 = r26 & 0xFF;
    r3 = r25;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0x11a3;
    ((void(*)(void))windowSearchItemID)();
    r0 = __cntlzw(r28);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0x11a4;
    ((void(*)(void))windowSearchItemID)();
    r25 = r3;
    r26 = 0x0;
    if (r27 != (u32)0x0) {
        r3 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
        ((void(*)(void))menuGetCursorItemID)();
        if ((s32)r3 != (s32)0xe34) {
            r3 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
            ((void(*)(void))menuGetCursorItemID)();
            if ((s32)r3 != (s32)0xe33) {
                r26 = 0x1;
    }
    }
    }
    r4 = r26 & 0xFF;
    r3 = r25;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0x11a5;
    ((void(*)(void))windowSearchItemID)();
    r0 = __cntlzw(r27);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0xa16;
    ((void(*)(void))windowSearchItemID)();
    r4 = r29;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0xd9f;
    ((void(*)(void))windowSearchItemID)();
    r4 = r29;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0xa15;
    ((void(*)(void))windowSearchItemID)();
    if (r29 != (u32)0x0) {
        r0 = 0x3d7a;
    } else {

        r0 = 0x3d79;
    }
    MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
    r3 = r31;
    r4 = 0x11a6;
    ((void(*)(void))windowSearchItemID)();
    r25 = r3;
    r26 = 0x0;
    if (r29 != (u32)0x0) {
        if (r28 != (u32)0x0) {
            r3 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
            ((void(*)(void))menuGetCursorItemID)();
            if ((s32)r3 == (s32)0x9fa) {
                r26 = 0x1;
    }
    }
    }
    r4 = r26 & 0xFF;
    r3 = r25;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0x11a7;
    ((void(*)(void))windowSearchItemID)();
    r26 = r3;
    r28 = 0x0;
    if (r29 != (u32)0x0) {
        if (r27 != (u32)0x0) {
            r3 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
            ((void(*)(void))menuGetCursorItemID)();
            if ((s32)r3 == (s32)0x9fb) {
                r28 = 0x1;
    }
    }
    }
    r4 = r28 & 0xFF;
    r3 = r26;
    winSpriteSetDisp();
    r3 = r31;
    r4 = r30 + 0x56c;
    r5 = 0x2;
    fn_80070D84();
    return;
}


/* 0x8006F720 | size: 0x4DC */
void fn_8006F720(void) {
    extern void fn_80070D84();
    extern void winSpriteSetDisp();
    extern void savedataGetStatus();
    extern u8 jumptable_802EE06C[];
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r29 = r3;
    r5 = (u32)&lbl_80267EA8;
    r4 = 0x0;
    r30 = (u32)&lbl_80267EA8;
    ((void(*)(void))windowGetParam)();
    r31 = r3;
    r28 = r30 + 0x3dc;
    r26 = 0x0;
    do {
        r4 = MENU_MIDDLE_U16_0000(r28)->unk_0000;
        r3 = r29;
        ((void(*)(void))windowSearchItemID)();
        r0 = MENU_MIDDLE_U32_0004(r28)->unk_0004;
        r28 = r28 + 0x8;
        r26 = r26 + 0x1;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
    } while (r26 < (u32)0x1a);
    r3 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
    ((void(*)(void))menuGetCursorItemID)();
    /* subi r0, r3, 0x9ca */;
    if (r0 <= (u32)0x20) {
        r3 = (u32)jumptable_802EE06C;
        r0 = r0 << 2;
        r3 = (u32)jumptable_802EE06C;
        r0 = *(u32*)(r3 + r0);
        ctr_fn = (void(*)(void))r0;
        /* indirect jump via ctr */;
        r25 = 0x0;


    } else {
    r25 = 0x8;
    }
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r28 = r30 + 0x3cc;
        r26 = 0x0;
        do {
            r4 = MENU_MIDDLE_U16_0000(r28)->unk_0000;
            r3 = r29;
            ((void(*)(void))windowSearchItemID)();
            r4 = 0x0;
            winSpriteSetDisp();
            r28 = r28 + 0x2;
            r26 = r26 + 0x1;
        } while (r26 < (u32)0x8);
    } else {

        r27 = r30 + 0x3cc;
        r26 = 0x0;
        do {
            r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
            r3 = r29;
            ((void(*)(void))windowSearchItemID)();
            r0 = r26 - r25;
            r28 = r3;
            r0 = __cntlzw(r0);
            r0 = (u32)r0 >> 5;
            r4 = r0 & 0xFF;
            winSpriteSetDisp();
            r3 = 0x191;
            ((void(*)(void))menuSeqBiosGetPtr)();
            r0 = MENU_MIDDLE_U32_000C(r28)->unk_000C;
            if (r0 != (u32)r3) {
                r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
                r3 = r29;
                r5 = 0x191;
                ((void(*)(void))fn_801081F8)();
            }
            r27 = r27 + 0x2;
            r26 = r26 + 0x1;
        } while (r26 < (u32)0x8);
    }
    r3 = r29;
    r4 = 0xd80;
    ((void(*)(void))windowSearchItemID)();
    r4 = MENU_MIDDLE_U8_000C(r31)->unk_000C;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9eb;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_000C(r31)->unk_000C;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9ec;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9ed;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
    r0 = 0x1 - r0;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9ee;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
    r0 = 0x2 - r0;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9ef;
    ((void(*)(void))windowSearchItemID)();
    r4 = MENU_MIDDLE_U8_000D(r31)->unk_000D;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9f0;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_000D(r31)->unk_000D;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9f1;
    ((void(*)(void))windowSearchItemID)();
    r4 = MENU_MIDDLE_U8_000E(r31)->unk_000E;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9f2;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_000E(r31)->unk_000E;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9f3;
    ((void(*)(void))windowSearchItemID)();
    r4 = MENU_MIDDLE_U8_000F(r31)->unk_000F;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9f4;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_000F(r31)->unk_000F;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9f5;
    ((void(*)(void))windowSearchItemID)();
    r4 = MENU_MIDDLE_U8_0010(r31)->unk_0010;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x8a2;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_0010(r31)->unk_0010;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9d4;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_S16_0002(r31)->unk_0002;
    r6 = MENU_MIDDLE_S16_0004(r31)->unk_0004;
    r0 = r0 * 0x6;
    r5 = (s32)r6 >> 31;
    r4 = (u32)r0 >> 31;
    r0 = r6 - r0;
    r0 = r5 + r4; /* +carry */;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0xfb0;
    ((void(*)(void))windowSearchItemID)();
    r28 = 0x0;
    r26 = r3;
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        r3 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
        ((void(*)(void))menuGetCursorItemID)();
        if ((s32)r3 == (s32)0x9cd) {
            r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
            if ((s32)r0 == (s32)0x2) {
                r28 = 0x1;
    }
    }
    }
    r4 = r28 & 0xFF;
    r3 = r26;
    winSpriteSetDisp();
    r3 = 0x0;
    r4 = 0xe;
    savedataGetStatus();
    r0 = MENU_MIDDLE_U32_0000(r3)->unk_0000;
    if ((s32)r0 == (s32)0x0) {
        r27 = r30 + 0x4ac;
        r28 = 0x0;
        do {
            r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
            r3 = r29;
            ((void(*)(void))windowSearchItemID)();
            r4 = 0x0;
            winSpriteSetDisp();
            r27 = r27 + 0x2;
            r28 = r28 + 0x1;
        } while (r28 < (u32)0x12);
        r3 = r29;
        r4 = 0xd9b;
        ((void(*)(void))windowSearchItemID)();
        r0 = 0x4238;
        r4 = 0xd9c;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
        r3 = r29;
        ((void(*)(void))windowSearchItemID)();
        r0 = 0x0;
        r4 = 0x9d3;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
        r3 = r29;
        ((void(*)(void))windowSearchItemID)();
        r0 = 0x4238;
        r4 = 0xd9a;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
        r3 = r29;
        ((void(*)(void))windowSearchItemID)();
        r0 = 0x0;
        r4 = 0x9d4;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
        r3 = r29;
        ((void(*)(void))windowSearchItemID)();
        r0 = 0x0;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
    }
    r0 = MENU_MIDDLE_U8_0001(r29)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            goto L_8006FBD8;
        }
        goto L_8006FBD8;
    }
    r3 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
    ((void(*)(void))menuGetCursorItemID)();
    if ((s32)r3 == (s32)0x9d2) {
        r0 = 0x1;
        MENU_MIDDLE_U8_0098(r29)->unk_0098 = r0;
    }
    ((void(*)(void))windowGetKeyInfo)();
    r0 = MENU_MIDDLE_U16_0004(r3)->unk_0004;
    r0 = r0 & 0x00000010;
    if ((s32)r0 == (s32)0x0) goto L_8006FBD8;
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r3 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
        r4 = 0x9d2;
        ((void(*)(void))menuGetCursorFromItemID)();
        r3 = (s8)r3;
        r0 = 0x0;
        *(u8*)(sp + 0xD) = r3;
        r4 = (u32)sp + 0x8;
        *(u8*)(sp + 0xC) = r0;
        r0 = *(u16*)(sp + 0xC);
        *(u16*)(sp + 0x8) = r0;
        r3 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
        ((void(*)(void))fn_801044D0)();

    } else {
    r0 = MENU_MIDDLE_U8_0095(r29)->unk_0095;
    r3 = 0x0;
    r0 = (s8)r0;
    if ((s32)r0 == (s32)0x3) {
        r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
        if ((s32)r0 == (s32)0x2) {
            r3 = 0x1;
    }
    }
    r0 = r3 & 0xFF;
    MENU_MIDDLE_U8_0098(r29)->unk_0098 = r0;
    }
    L_8006FBD8: ;
    r3 = r29;
    r4 = r30 + 0x4d0;
    r5 = 0x2;
    fn_80070D84();
    return;
}


/* 0x8006FBFC | size: 0xFC */
void fn_8006FBFC(void) {
    extern void fn_80070D84();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;


    r29 = r3;
    r4 = 0x9bb;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_0095(r29)->unk_0095;
    r0 = (s8)r0;
    if ((s32)r0 < (s32)0x3) {
        r0 = 0x3dc0;
    } else {

        r0 = 0x3dc1;
    }
    MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
    r0 = MENU_MIDDLE_U8_0001(r29)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 != (s32)0x0) {
                goto L_8006FCC8;
            }
            goto L_8006FCC8;
            }
        r3 = (u32)&lbl_80268234;
        r30 = 0x0;
        r31 = (u32)&lbl_80268234;
        do {
            r4 = MENU_MIDDLE_U16_0000(r31)->unk_0000;
            r3 = r29;
            ((void(*)(void))windowSearchItemID)();
            r0 = MENU_MIDDLE_U32_0004(r31)->unk_0004;
            r31 = r31 + 0x8;
            r30 = r30 + 0x1;
            MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
        } while (r30 < (u32)0x8);

    } else {
    ((void(*)(void))windowGetKeyInfo)();
    r0 = MENU_MIDDLE_U16_0004(r3)->unk_0004;
    r0 = r0 & 0x00000400;
    if ((s32)r0 != (s32)0x0) {
        r0 = MENU_MIDDLE_U8_0095(r29)->unk_0095;
        r0 = (s8)r0;
        if ((s32)r0 < (s32)0x6) {
            r0 = 0x1;
            MENU_MIDDLE_U8_0098(r29)->unk_0098 = r0;
    }
    }
    }
    L_8006FCC8: ;
    r4 = (u32)&lbl_802681B4;
    r3 = r29;
    r4 = (u32)&lbl_802681B4;
    r5 = 0x10;
    fn_80070D84();
    return;
}


/* 0x8006FCF8 | size: 0x2C */
#pragma push
#pragma peephole off
void fn_8006FCF8(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, (u32)lbl_80268184, 0x6);
}
#pragma pop

/* 0x8006FD24 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FD24(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FD4C | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FD4C(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FD74 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FD74(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FD9C | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FD9C(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FDC4 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FDC4(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FDEC | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FDEC(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FE14 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FE14(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FE3C | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FE3C(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FE64 | size: 0x80 */
#pragma peephole off
void fn_8006FE64(void* menu) {
    extern u32 fn_8006B3C8(s32 index);
    extern void fn_80166A28(s32 sndId);
    KeyInfo_8006BB34* keyInfo;
    s32 state;
    s32 flag;
    int stateIndex;
    u32 value;

    state = MENU_MIDDLE_U8_0095(menu)->unk_0095;
    state = (s8)state;
    if (state < 6) {
        stateIndex = state;
        keyInfo = windowGetKeyInfo();
        flag = keyInfo->flags4 & 0x10;
        if (flag != 0) {
            value = fn_8006B3C8(stateIndex);
            if ((u8)value == 0) {
                fn_80166A28(0x26);
                return;
            }
        }
    }
    menuButtonNormal(menu);
}
#pragma peephole reset


/* 0x8006FEE4 | size: 0x390 */
void fn_8006FEE4(void) {
    extern void fn_8006B1F4();
    extern void fn_8006B3C8();
    extern void fn_80070D84();
    extern void winSpriteSetDisp();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
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

    
    r30 = r3;
    r0 = MENU_MIDDLE_U8_0095(r30)->unk_0095;
    r3 = (u32)&lbl_80267EA8;
    r31 = (u32)&lbl_80267EA8;
    r0 = (s8)r0;
    if ((s32)r0 < (s32)0x6) {
        *(u32*)&lbl_8047A5FC = r0;
    }
    r3 = *(u32*)&lbl_8047A5FC;
    fn_8006B3C8();
    r28 = r3;
    r3 = r30;
    r4 = 0x957;
    ((void(*)(void))windowSearchItemID)();
    r25 = r3;
    if (r25 != (u32)0x0) {
        r0 = r28 & 0xFF;
        if (r0 != (u32)0x0) {
            r0 = *(u32*)&lbl_8047A5FC;
            r3 = r31 + 0x218;
            r0 = r0 << 2;
            r3 = *(u32*)(r3 + r0);
        } else {

            r3 = 0x26c;
        }
        ((void(*)(void))menuSpriteBiosGetPtr)();
        r0 = r3;
        r3 = r25;
        r4 = r0;
        ((void(*)(void))fn_80071318)();
    }
    r27 = r31 + 0x1c8;
    r25 = 0x0;
    do {
        r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r29 = r3;
        r3 = r25;
        fn_8006B3C8();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x0) {
            r0 = MENU_MIDDLE_U32_0004(r27)->unk_0004;
        } else {

            r0 = 0x3daa;
        }
        MENU_MIDDLE_U32_004C(r29)->unk_004C = r0;
        r27 = r27 + 0x8;
        r25 = r25 + 0x1;
    } while ((s32)r25 < (s32)0x6);
    r0 = r25 << 3;
    r27 = r31 + 0x1c8;
    r27 = r27 + r0;
    while (r25 < (u32)0xa) {

        r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r0 = MENU_MIDDLE_U32_0004(r27)->unk_0004;
        r27 = r27 + 0x8;
        r25 = r25 + 0x1;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;

    }
    r0 = r28 & 0xFF;
    r27 = r31 + 0x230;
    r0 = __cntlzw(r0);
    r25 = 0x0;
    r29 = (u32)r0 >> 5;
    do {
        r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r4 = r29 & 0xFF;
        winSpriteSetDisp();
        r27 = r27 + 0x2;
        r25 = r25 + 0x1;
    } while (r25 < (u32)0x5);
    r0 = MENU_MIDDLE_U8_0001(r30)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) goto L_8007011C;
        if ((s32)r0 != (s32)0x0) {
            goto L_8007011C;
        }
        r26 = r31 + 0x23c;
        r25 = 0x0;
        r29 = r31 + 0x0;
        do {
            r0 = (u32)r25 >> 31;
            r27 = r26;
            r0 = r0 + r25;
            r24 = 0x0;
            r0 = (s32)r0 >> 1;
            r3 = r0 << 2;
            r28 = r3 + 0xc;
            do {
                r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
                r3 = r30;
                ((void(*)(void))windowSearchItemID)();
                r4 = *(u16*)(r29 + r28);
                r3 = r3 + 0xc;
                ((void(*)(void))winSetSequence)();
                r27 = r27 + 0x2;
                r24 = r24 + 0x1;
            } while (r24 < (u32)0x5);
            r26 = r26 + 0xa;
            r25 = r25 + 0x1;
        } while (r25 < (u32)0x6);

    } else {
    r0 = MENU_MIDDLE_U8_0002(r30)->unk_0002;
    r0 = (s8)r0;
    if ((s32)r0 == (s32)0x0) {
        r28 = r31 + 0x23c;
        r25 = 0x0;
        r29 = r31 + 0x0;
        do {
            r0 = (u32)r25 >> 31;
            r26 = r28;
            r0 = r0 + r25;
            r24 = 0x0;
            r0 = (s32)r0 >> 1;
            r3 = r0 << 2;
            r27 = r3 + 0xe;
            do {
                r4 = MENU_MIDDLE_U16_0000(r26)->unk_0000;
                r3 = r30;
                ((void(*)(void))windowSearchItemID)();
                r4 = *(u16*)(r29 + r27);
                r3 = r3 + 0xc;
                ((void(*)(void))winSetSequence)();
                r26 = r26 + 0x2;
                r24 = r24 + 0x1;
            } while (r24 < (u32)0x5);
            r28 = r28 + 0xa;
            r25 = r25 + 0x1;
        } while (r25 < (u32)0x6);
    }
    }
    L_8007011C: ;
    r3 = r30;
    r4 = r31 + 0x278;
    r5 = 0xb;
    fn_80070D84();
    r26 = r31 + 0x2d0;
    r27 = 0x0;
    do {
        r3 = *(u32*)&lbl_8047A5FC;
        if ((s32)r27 == (s32)0x0) {
            r4 = 0x0;
        } else {

            r4 = 0x1;
        }
        fn_8006B1F4();
        r4 = MENU_MIDDLE_U16_0000(r26)->unk_0000;
        r24 = r3;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r4 = r24;
        winSpriteSetDisp();
        r4 = MENU_MIDDLE_U16_0002(r26)->unk_0002;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r4 = r24;
        winSpriteSetDisp();
        r4 = MENU_MIDDLE_U16_0004(r26)->unk_0004;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r0 = r24 & 0xFF;
        if (r0 != (u32)0x0) {
            r0 = -0x1;
        } else {

            r4 = (0x6060 << 16);
            r0 = r4 + 0x60ff;
        }
        MENU_MIDDLE_U32_0064(r3)->unk_0064 = r0;
        r26 = r26 + 0x6;
        r27 = r27 + 0x1;
    } while (r27 < (u32)0x2);
    r27 = 0x0;
    r26 = (u32)&lbl_8047C050;
    do {
        r4 = MENU_MIDDLE_U16_0000(r26)->unk_0000;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r4 = (u32)r27 >> 31;
        r0 = r27 & 0x1;
        r0 = r0 ^ r4;
        r31 = r3;
        r0 = r0 - r4;
        if ((s32)r0 != (s32)0x0) {
            f1 = MENU_MIDDLE_F32_0070(r31)->unk_0070;
            f0 = *(f32*)&lbl_8047C078;
            f2 = *(f64*)&lbl_8047C080;
            f0 = f1 + f0;
            MENU_MIDDLE_F32_0070(r31)->unk_0070 = f0;
            f1 = MENU_MIDDLE_F32_0070(r31)->unk_0070;
            ((void(*)(void))fmod)();
            f0 = (f32)f1;
            MENU_MIDDLE_F32_0070(r31)->unk_0070 = f0;
        } else {

            f2 = MENU_MIDDLE_F32_0070(r31)->unk_0070;
            f1 = *(f32*)&lbl_8047C078;
            f0 = *(f32*)&lbl_8047C088;
            f1 = f2 - f1;
            MENU_MIDDLE_F32_0070(r31)->unk_0070 = f1;
            f1 = MENU_MIDDLE_F32_0070(r31)->unk_0070;
            if (f0 > f1) {
                f0 = *(f32*)&lbl_8047C08C;
                f0 = f1 + f0;
                MENU_MIDDLE_F32_0070(r31)->unk_0070 = f0;
            }
            f1 = MENU_MIDDLE_F32_0070(r31)->unk_0070;
            f2 = *(f64*)&lbl_8047C080;
            ((void(*)(void))fmod)();
            f0 = (f32)f1;
            MENU_MIDDLE_F32_0070(r31)->unk_0070 = f0;
        }
        r26 = r26 + 0x2;
        r27 = r27 + 0x1;
    } while (r27 < (u32)0x4);
    return;
}


/* 0x80070274 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_80070274(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8007029C | size: 0x2C */
#pragma push
#pragma peephole off
void fn_8007029C(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, (u32)lbl_80267FE8, 0x11);
}
#pragma pop

/* 0x800702C8 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_800702C8(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x800702F0 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_800702F0(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x80070318 | size: 0x110 */
void fn_80070318(void) {
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r0 = MENU_MIDDLE_U8_0001(r31)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) return;
        if ((s32)r0 != (s32)0x0) {
            return;


        }
        r0 = MENU_MIDDLE_U8_0002(r31)->unk_0002;
        r0 = (s8)r0;
        if ((s32)r0 != (s32)0x0) return;
        r3 = (u32)&lbl_80267F68;
        r28 = 0x0;
        r29 = (u32)&lbl_80267F68;
        r3 = (u32)&lbl_80267EA8;
        r30 = (u32)&lbl_80267EA8;
        do {
            r0 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
            r3 = r31;
            r4 = MENU_MIDDLE_U16_0000(r29)->unk_0000;
            r0 = r0 << 2;
            r5 = *(u16*)(r30 + r0);
            ((void(*)(void))fn_801081F8)();
            r29 = r29 + 0x8;
            r28 = r28 + 0x1;
        } while (r28 < (u32)0x10);
        return;
    }
    r0 = MENU_MIDDLE_U8_0002(r31)->unk_0002;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x0) return;
    r3 = (u32)&lbl_80267F68;
    r28 = 0x0;
    r29 = (u32)&lbl_80267F68;
    r3 = (u32)&lbl_80267EA8;
    r30 = (u32)&lbl_80267EA8;
    do {
        r0 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
        r3 = r31;
        r4 = MENU_MIDDLE_U16_0000(r29)->unk_0000;
        r0 = r0 << 2;
        r5 = r30 + r0;
        r5 = MENU_MIDDLE_U16_0002(r5)->unk_0002;
        ((void(*)(void))fn_801081F8)();
        r29 = r29 + 0x8;
        r28 = r28 + 0x1;
    } while (r28 < (u32)0x10);
    r0 = 0x1;
    MENU_MIDDLE_U8_0002(r31)->unk_0002 = r0;

    return;
}


/* 0x80070428 | size: 0x7C */
#pragma peephole off
void fn_80070428(void* arg0, void* menu) {
    extern void msgctrlSetValue();
    u32 value;
    void* context = menu;
    s16 state = MENU_MIDDLE_S16_0006(context)->unk_0006;
    u32 message;

    if (state < 0xA28) {
        if (state >= 0xA1D) {
            message = MENU_MIDDLE_U32_004C(context)->unk_004C;
            if (message != 0) {
                value = GSmsgGetGSchar(message);
                msgctrlSetValue(0x37, value);
                fn_800FB680(0, 0, MENU_MIDDLE_U32_0064(context)->unk_0064, 0xE7);
                MENU_MIDDLE_U32_004C(context)->unk_004C = 0;
            }
        }
    }
}
#pragma peephole reset


/* 0x800704A4 | size: 0x4 */
void fn_800704A4(void) {
}

/* 0x800704A8 | size: 0x4 */
void fn_800704A8(void) {
}

/* 0x800704AC | size: 0x218 */
void fn_800704AC(void) {
    extern void fn_8006B3C8();
    extern void winSpriteSetDisp();
    extern u8 jumptable_802EE0F0[];
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r31 = r4;
    r0 = MENU_MIDDLE_U8_0001(r3)->unk_0001;
    r3 = (u32)&lbl_80267EA8;
    r30 = (u32)&lbl_80267EA8;
    r0 = (s8)r0;
    if ((s32)r0 < (s32)0x3) {
        if ((s32)r0 < (s32)0x0) {
            goto L_80070624;
        }
        goto L_80070624;
        }
    ((void(*)(void))fn_8007162C)();
    /* subi r0, r3, 0xa8 */;
    if (r0 <= (u32)0x46) {
        r3 = (u32)jumptable_802EE0F0;
        r0 = r0 << 2;
        r3 = (u32)jumptable_802EE0F0;
        r0 = *(u32*)(r3 + r0);
        ctr_fn = (void(*)(void))r0;
        /* indirect jump via ctr */;
        r0 = r30 + 0x28;
        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;

        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;

        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;

        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;


        do {
            r3 = r28;
            fn_8006B3C8();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x0) {
                r0 = MENU_MIDDLE_U32_0000(r30)->unk_0000;
            } else {

                r0 = 0x43fe;
            }
            MENU_MIDDLE_U32_0000(r29)->unk_0000 = r0;
            r30 = r30 + 0x4;
            r29 = r29 + 0x4;
            r28 = r28 + 0x1;
        } while (r28 < (u32)0x6);
        r3 = (u32)&lbl_802EDE58;
        r0 = (u32)&lbl_802EDE58;
        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;


        if (r3 != (u32)0x0) {
            r0 = r30 + 0x90;
            *(u32*)&lbl_8047A5F8 = r0;
            goto L_80070624;
        }
        r0 = r30 + 0x9c;
        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;

        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;

        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;


        if ((s32)r3 == (s32)r30) {
            r0 = 0x0;
        } else {

            r0 = 0x1;
        }
        r3 = r0 << 2;
        r0 = (u32)&lbl_8047C048;
        r0 = r0 + r3;
        *(u32*)&lbl_8047A5F8 = r0;
        goto L_80070624;
    }
    r0 = 0x0;
    *(u32*)&lbl_8047A5F8 = r0;
    L_80070624: ;
    r0 = *(u32*)&lbl_8047A5F8;
    if (r0 != (u32)0x0) {
        r3 = r31;
        r4 = 0x1;
        winSpriteSetDisp();
        r0 = MENU_MIDDLE_S16_0006(r31)->unk_0006;
        if ((s32)r0 != (s32)0x93d) {
            if ((s32)r0 >= (s32)0x93d) return;
            return;
        }
        ((void(*)(void))fn_8007162C)();
        ((void(*)(void))windowSearchID)();
        if (r3 == (u32)0x0) {
            ((void(*)(void))windowGetActiveID)();
            ((void(*)(void))windowSearchID)();
        }
        if (r3 == (u32)0x0) return;
        r0 = MENU_MIDDLE_U8_0095(r3)->unk_0095;
        r3 = *(u32*)&lbl_8047A5F8;
        r0 = (s8)r0;
        r0 = r0 << 2;
        r0 = *(u32*)(r3 + r0);
        MENU_MIDDLE_U32_004C(r31)->unk_004C = r0;
        return;
    }
    r0 = 0x0;
    r3 = r31;
    MENU_MIDDLE_U32_004C(r31)->unk_004C = r0;
    r4 = 0x0;
    winSpriteSetDisp();

    return;
}


/* 0x800706C4 | size: 0x3D8 */
void fn_800706C4(void) {
    extern void fn_8006AFC4();
    extern void fn_8006B420();
    extern void winSpriteSetDisp();
    extern void savedataGetStatus();
    extern u8 jumptable_802EE20C[];
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r29 = r3;
    r30 = r4;
    r0 = MENU_MIDDLE_U8_0001(r29)->unk_0001;
    r0 = (s8)r0;
    do {
    if ((s32)r0 >= (s32)0x3) break;
    if ((s32)r0 < (s32)0x0) {
        break;
    }
    r0 = MENU_MIDDLE_U32_004C(r30)->unk_004C;
    r4 = 0x0;
    *(u32*)&lbl_8047A5F4 = r0;
    ((void(*)(void))windowGetParam)();
    /* subi r0, r3, 0xa8 */;
    do {
    if (r0 > (u32)0x43) break;
    r3 = (u32)jumptable_802EE20C;
    r0 = r0 << 2;
    r3 = (u32)jumptable_802EE20C;
    r0 = *(u32*)(r3 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    r3 = 0x3be5;
    r0 = 0x1;
    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;


    if (r3 == (u32)0x0) break;
    fn_8006B420();
    r31 = r3;
    r3 = 0x0;
    r4 = 0xe;
    savedataGetStatus();
    fn_8006AFC4();
    r4 = r31;
    r3 = r3 + 0xb44;
    ((void(*)(void))fn_800767B8)();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) break;
    r3 = 0x3bfc;
    r0 = 0x2;
    *(u32*)&lbl_8047A5F4 = r3;
    *(u32*)&lbl_8047A5F0 = r0;
    break;
    } while (0);
    r0 = 0x0;
    *(u32*)&lbl_8047A5F4 = r0;
    *(u32*)&lbl_8047A5F0 = r0;
    } while (0);
    r3 = *(u32*)&lbl_8047A5F4;
    ((void(*)(void))GSmsgGetRect)();
    r0 = MENU_MIDDLE_S16_0006(r30)->unk_0006;
    r3 = (u32)r3 >> 16;
    /* subi r3, r3, 0x20 */;
    r31 = (s16)r3;
    if ((s32)r0 != (s32)0x939) {
        if ((s32)r0 < (s32)0x939) {
            if ((s32)r0 != (s32)0x809) {
                if ((s32)r0 < (s32)0x809) {
                    if ((s32)r0 < (s32)0x808) {
                        return;
                    }
                    if ((s32)r0 != (s32)0x937) {
                        if ((s32)r0 < (s32)0x937) {
                            return;
                        }
                        if ((s32)r0 != (s32)0xef8) {
                            if ((s32)r0 < (s32)0xef8) {
                                if ((s32)r0 != (s32)0xef6) {
                                    if ((s32)r0 < (s32)0xef6) {
                                        return;
                                    }
                                    if ((s32)r0 >= (s32)0xefa) return;
                                    r0 = *(u32*)&lbl_8047A5F0;
                                    r3 = r30;
                                    r0 = 0x2 - r0;
                                    r0 = __cntlzw(r0);
                                    r0 = (u32)r0 >> 5;
                                    r4 = r0 & 0xFF;
                                    winSpriteSetDisp();
                                    r0 = *(u32*)&lbl_8047A5F0;
                                    if ((s32)r0 == (s32)0x2) {
                                        r0 = *(u32*)&lbl_8047A5F4;
                                    } else {

                                        r0 = 0x0;
                                    }
                                    MENU_MIDDLE_U32_004C(r30)->unk_004C = r0;
                                    return;
                                        }
                                r0 = *(u32*)&lbl_8047A5F0;
                                r3 = r30;
                                r0 = 0x1 - r0;
                                r0 = __cntlzw(r0);
                                r0 = (u32)r0 >> 5;
                                r4 = r0 & 0xFF;
                                winSpriteSetDisp();
                                r0 = *(u32*)&lbl_8047A5F0;
                                if ((s32)r0 == (s32)0x1) {
                                    r0 = *(u32*)&lbl_8047A5F4;
                                } else {

                                    r0 = 0x0;
                                }
                                MENU_MIDDLE_U32_004C(r30)->unk_004C = r0;
                                return;
                                }
                            r3 = r30;
                            r4 = 0x0;
                            winSpriteSetDisp();
                            return;
                                    }
                        r0 = *(u32*)&lbl_8047A5F0;
                        r3 = r30;
                        r0 = 0x2 - r0;
                        r0 = __cntlzw(r0);
                        r0 = (u32)r0 >> 5;
                        r4 = r0 & 0xFF;
                        winSpriteSetDisp();
                        return;
                                    }
                    r0 = *(u32*)&lbl_8047A5F0;
                    r3 = r30;
                    r0 = 0x2 - r0;
                    r0 = __cntlzw(r0);
                    r0 = (u32)r0 >> 5;
                    r4 = r0 & 0xFF;
                    winSpriteSetDisp();
                    MENU_MIDDLE_U16_0054(r30)->unk_0054 = r31;
                    return;
                }
                r0 = *(u32*)&lbl_8047A5F0;
                r3 = r30;
                r0 = 0x2 - r0;
                r0 = __cntlzw(r0);
                r0 = (u32)r0 >> 5;
                r4 = r0 & 0xFF;
                winSpriteSetDisp();
                r3 = r29;
                r4 = 0x938;
                ((void(*)(void))windowSearchItemID)();
                r0 = MENU_MIDDLE_S16_0050(r3)->unk_0050;
                r0 = r31 + r0;
                r0 = (s16)r0;
                MENU_MIDDLE_U16_0050(r30)->unk_0050 = r0;
                return;
                                    }
            r0 = *(u32*)&lbl_8047A5F0;
            r3 = r30;
            r0 = 0x1 - r0;
            r0 = __cntlzw(r0);
            r0 = (u32)r0 >> 5;
            r4 = r0 & 0xFF;
            winSpriteSetDisp();
            return;
                                    }
        r0 = *(u32*)&lbl_8047A5F0;
        r3 = r30;
        r0 = 0x1 - r0;
        r0 = __cntlzw(r0);
        r0 = (u32)r0 >> 5;
        r4 = r0 & 0xFF;
        winSpriteSetDisp();
        MENU_MIDDLE_U16_0054(r30)->unk_0054 = r31;
        return;
                        }
    r0 = *(u32*)&lbl_8047A5F0;
    r3 = r30;
    r0 = 0x1 - r0;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0xef7;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_S16_0050(r3)->unk_0050;
    r0 = r31 + r0;
    r0 = (s16)r0;
    MENU_MIDDLE_U16_0050(r30)->unk_0050 = r0;

    return;
}


/* 0x80070A9C | size: 0x2E8 */
void fn_80070A9C(void) {
    extern void winSpriteSetDisp();
    extern void savedataGetStatus();
    extern u8 jumptable_802EE31C[];
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r31 = r4;
    r0 = MENU_MIDDLE_U8_0001(r3)->unk_0001;
    r0 = (s8)r0;
    do {
    if ((s32)r0 >= (s32)0x3) break;
    if ((s32)r0 < (s32)0x0) {
        break;
    }
    r0 = MENU_MIDDLE_U32_004C(r31)->unk_004C;
    r4 = 0x0;
    *(u32*)&lbl_8047A5E8 = r0;
    r0 = MENU_MIDDLE_U32_004C(r31)->unk_004C;
    *(u32*)&lbl_8047A5EC = r0;
    ((void(*)(void))windowGetParam)();
    /* subi r0, r3, 0xa8 */;
    if (r0 > (u32)0x4d) break;
    r3 = (u32)jumptable_802EE31C;
    r0 = r0 << 2;
    r3 = (u32)jumptable_802EE31C;
    r0 = *(u32*)(r3 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    r0 = 0x0;
    *(u32*)&lbl_8047A5E8 = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    if ((s32)r0 == (s32)0x0) {
        r3 = 0x4237;
        r0 = 0x0;
        *(u32*)&lbl_8047A5E8 = r3;
        *(u32*)&lbl_8047A5EC = r0;
        break;
    }
    r3 = 0x0;
    r4 = 0xe;
    savedataGetStatus();
    r0 = MENU_MIDDLE_U32_0008(r3)->unk_0008;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 < (s32)0x3) {
            if ((s32)r0 != (s32)0x1) {
                if ((s32)r0 < (s32)0x1) {
                    if ((s32)r0 < (s32)0x0) {
                        break;
                    }
                    if ((s32)r0 != (s32)0x5) {
                        if ((s32)r0 >= (s32)0x5) break;
                        goto L_80070C74;
                        }
                    r3 = 0x3d7c;
                    r0 = 0x0;
                    *(u32*)&lbl_8047A5E8 = r3;
                    *(u32*)&lbl_8047A5EC = r0;
                    break;
                    }
                r3 = 0x3d7d;
                r0 = 0x0;
                *(u32*)&lbl_8047A5E8 = r3;
                *(u32*)&lbl_8047A5EC = r0;
                break;
                    }
            r3 = 0x3d7e;
            r0 = 0x0;
            *(u32*)&lbl_8047A5E8 = r3;
            *(u32*)&lbl_8047A5EC = r0;
            break;
        }
        r3 = 0x3d7f;
        r0 = 0x0;
        *(u32*)&lbl_8047A5E8 = r3;
        *(u32*)&lbl_8047A5EC = r0;
        break;
        L_80070C74: ;
        r3 = 0x3d80;
        r0 = 0x0;
        *(u32*)&lbl_8047A5E8 = r3;
        *(u32*)&lbl_8047A5EC = r0;
        break;
                    }
    r3 = 0x3d81;
    r0 = 0x0;
    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    break;


    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            if ((s32)r0 != (s32)0x0) {
                if ((s32)r0 < (s32)0x0) {
                    break;
                }
                if ((s32)r0 >= (s32)0x4) break;
                r3 = 0x3d6e;
                r0 = 0x0;
                *(u32*)&lbl_8047A5E8 = r3;
                *(u32*)&lbl_8047A5EC = r0;
                break;
                }
            r3 = 0x3d3a;
            r0 = 0x3dab;
            *(u32*)&lbl_8047A5E8 = r3;
            *(u32*)&lbl_8047A5EC = r0;
            break;
                }
        r3 = 0x3d3a;
        r0 = 0x423c;
        *(u32*)&lbl_8047A5E8 = r3;
        *(u32*)&lbl_8047A5EC = r0;
        break;
    }
    r3 = 0x3d3a;
    r0 = 0x3d2d;
    *(u32*)&lbl_8047A5E8 = r3;
    *(u32*)&lbl_8047A5EC = r0;
    } while (0);
    r4 = *(u32*)&lbl_8047A5E8;
    r3 = r31;
    r0 = -r4;
    r0 = r0 | r4;
    r4 = (u32)r0 >> 31;
    winSpriteSetDisp();
    r0 = MENU_MIDDLE_S16_0006(r31)->unk_0006;
    if ((s32)r0 != (s32)0x89b) {
        if ((s32)r0 < (s32)0x89b) {
            if ((s32)r0 == (s32)0x80a) return;
            return;
        }
        if ((s32)r0 != (s32)0x93e) {
            return;
        }
        r0 = *(u32*)&lbl_8047A5E8;
        MENU_MIDDLE_U32_004C(r31)->unk_004C = r0;
        return;
        }
    r0 = *(u32*)&lbl_8047A5EC;
    MENU_MIDDLE_U32_004C(r31)->unk_004C = r0;

    return;
}


/* 0x80070D84 | size: 0x318 */
void fn_80070D84(void) {
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r29 = r3;
    r30 = r5;
    r0 = MENU_MIDDLE_U8_0002(r29)->unk_0002;
    r31 = MENU_MIDDLE_U32_001C(r29)->unk_001C;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r0 = MENU_MIDDLE_U8_0001(r29)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) { r3 = 0x1; return; }
        if ((s32)r0 != (s32)0x0) {
            r3 = 0x1;
            return;
        }
        if (r4 != (u32)0x0) {
            r27 = r4;
            r26 = 0x0;
            r3 = (u32)&lbl_80267EA8;
            r28 = (u32)&lbl_80267EA8;
            while (r26 < r30) {

                r0 = MENU_MIDDLE_U32_0004(r27)->unk_0004;
                r3 = r29;
                r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
                r0 = r0 << 2;
                r5 = *(u16*)(r28 + r0);
                ((void(*)(void))fn_801081F8)();
                r27 = r27 + 0x8;
                r26 = r26 + 0x1;

            }
        }
        while (r31 != (u32)0x0) {

            r0 = MENU_MIDDLE_S16_0050(r31)->unk_0050;
            if ((s32)r0 < (s32)0x12c) {
                r0 = MENU_MIDDLE_S16_0052(r31)->unk_0052;
                if ((s32)r0 < (s32)0x64) {
                    r4 = 0x3;

                } else if ((s32)r0 < (s32)0xc8) {
                    r4 = 0x4;

                }
                r4 = 0x5;

            }

            r0 = MENU_MIDDLE_S16_0052(r31)->unk_0052;
            if ((s32)r0 < (s32)0x64) {
                r4 = 0x6;

            } else if ((s32)r0 < (s32)0xc8) {
                r4 = 0x7;

            } else {
                r4 = 0x8;
            }
            r3 = 0x0;
            if (r31 != (u32)0x0) {
                r0 = MENU_MIDDLE_U32_000C(r31)->unk_000C;
                if (r0 != (u32)0x0) {
                    r0 = MENU_MIDDLE_U8_0046(r31)->unk_0046;
                    if (r0 == (u32)0x0) {
                        r3 = 0x1;
            }
            }
            }
            if ((s32)r3 == (s32)0x0) {
                r3 = (u32)&lbl_80267EA8;
                r0 = r4 << 2;
                r4 = (u32)&lbl_80267EA8;
                r4 = *(u16*)(r4 + r0);
                r3 = r31 + 0xc;
                ((void(*)(void))winSetSequence)();
            }
            r31 = MENU_MIDDLE_U32_0000(r31)->unk_0000;

        }
        r27 = MENU_MIDDLE_U32_0020(r29)->unk_0020;
        while (r27 != (u32)0x0) {

            r3 = 0x0;
            if (r27 != (u32)0x0) {
                r0 = MENU_MIDDLE_U32_000C(r27)->unk_000C;
                if (r0 != (u32)0x0) {
                    r0 = MENU_MIDDLE_U8_0046(r27)->unk_0046;
                    if (r0 == (u32)0x0) {
                        r3 = 0x1;
            }
            }
            }
            if ((s32)r3 == (s32)0x0) {
                r3 = r27 + 0xc;
                r4 = 0x1ca;
                ((void(*)(void))winSetSequence)();
            }
            r27 = MENU_MIDDLE_U32_0000(r27)->unk_0000;

        }
        r3 = 0x1;
        return;
    }
    if (r4 != (u32)0x0) {
        r27 = r4;
        r26 = 0x0;
        r3 = (u32)&lbl_80267EA8;
        r28 = (u32)&lbl_80267EA8;
        while (r26 < r30) {

            r0 = MENU_MIDDLE_U32_0004(r27)->unk_0004;
            r3 = r29;
            r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
            r0 = r0 << 2;
            r5 = r28 + r0;
            r5 = MENU_MIDDLE_U16_0002(r5)->unk_0002;
            ((void(*)(void))fn_801081F8)();
            r27 = r27 + 0x8;
            r26 = r26 + 0x1;

        }
    }
    while (r31 != (u32)0x0) {

        r0 = MENU_MIDDLE_S16_0050(r31)->unk_0050;
        if ((s32)r0 < (s32)0x12c) {
            r0 = MENU_MIDDLE_S16_0052(r31)->unk_0052;
            if ((s32)r0 < (s32)0x64) {
                r4 = 0x3;

            } else if ((s32)r0 < (s32)0xc8) {
                r4 = 0x4;

            }
            r4 = 0x5;

        }

        r0 = MENU_MIDDLE_S16_0052(r31)->unk_0052;
        if ((s32)r0 < (s32)0x64) {
            r4 = 0x6;

        } else if ((s32)r0 < (s32)0xc8) {
            r4 = 0x7;

        } else {
            r4 = 0x8;
        }
        r3 = 0x0;
        if (r31 != (u32)0x0) {
            r0 = MENU_MIDDLE_U32_000C(r31)->unk_000C;
            if (r0 != (u32)0x0) {
                r0 = MENU_MIDDLE_U8_0046(r31)->unk_0046;
                if (r0 == (u32)0x0) {
                    r3 = 0x1;
        }
        }
        }
        if ((s32)r3 == (s32)0x0) {
            r3 = (u32)&lbl_80267EA8;
            r4 = r4 << 2;
            r0 = (u32)&lbl_80267EA8;
            r4 = r0 + r4;
            r3 = r31 + 0xc;
            r4 = MENU_MIDDLE_U16_0002(r4)->unk_0002;
            ((void(*)(void))winSetSequence)();
        }
        r31 = MENU_MIDDLE_U32_0000(r31)->unk_0000;

    }
    r27 = MENU_MIDDLE_U32_0020(r29)->unk_0020;
    while (r27 != (u32)0x0) {

        r3 = 0x0;
        if (r27 != (u32)0x0) {
            r0 = MENU_MIDDLE_U32_000C(r27)->unk_000C;
            if (r0 != (u32)0x0) {
                r0 = MENU_MIDDLE_U8_0046(r27)->unk_0046;
                if (r0 == (u32)0x0) {
                    r3 = 0x1;
        }
        }
        }
        if ((s32)r3 == (s32)0x0) {
            r3 = r27 + 0xc;
            r4 = 0x1ce;
            ((void(*)(void))winSetSequence)();
        }
        r27 = MENU_MIDDLE_U32_0000(r27)->unk_0000;

    }
    r0 = 0x1;
    MENU_MIDDLE_U8_0002(r29)->unk_0002 = r0;

    r3 = 0x1;

    return;
}
#endif
