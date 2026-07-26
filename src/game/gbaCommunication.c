/**
 * @file gbaCommunication.c
 * @brief Candidate gbaCommunication suffix, 0x80091DA4 - 0x800980E0
 *        (60 target functions).
 */
#include "dolphin/types.h"
#include "game/gs_material.h"

#define GBA_DATA_OFFSET 0x20
#define GBA_STATE_PORT 0x4338
#define GBA_STATE_TIMEOUT 0x433C
#define GBA_STATE_PHASE 0x4340
#define GBA_THREAD_PRIORITY 8

extern u32 lbl_8047A690;
extern u32 lbl_8047A694;
extern f32 lbl_8047C1D0; /* 0.833333313f -- PAL-adjusted 1-unit wait */
extern f32 lbl_8047C1D4; /* 0.0f */
extern f32 lbl_8047C1D8; /* 1.0f */
extern f32 lbl_8047C1DC; /* 83.3333282f -- PAL-adjusted 100-unit wait */
extern f32 lbl_8047C1E0; /* {41.6666641f, 0.0f} -- PAL-adjusted 50-unit wait */

/* Additional data labels referenced by the ported gba_comm_ext.c /
 * late_game.c bodies below (GBA link-cable state machine + battle-status
 * window helpers living in this same address range per the current
 * object map). */
extern u8 lbl_803FB328[];
extern u8 lbl_803FB338[];
extern u8 lbl_803FB380[];
extern u8 lbl_8047C1E8;
extern u8 lbl_8026F5A8[];
extern u8 lbl_8026F5C0[];
extern u8 lbl_80314F98[];
extern u16 lbl_802EED28[];

/* Common callees needed by the ported bodies below that are not already
 * declared with a full prototype at the point of use. */
extern void _threadSwitch(void);
extern void* windowSearchID(u32 id);
extern void fn_8009F7B4(void *p);
extern void fn_8009F890(void *p);
extern void fn_800A257C(void *p, u32 b);
extern void fn_800716E8(u32 port, u32 val);
extern void fn_8009FABC(void *p);
extern void fn_800A1E54(void *p, u32 v);
extern void fn_800716C8(u32 port, void *a, void *b);
extern u32 fn_800E202C(void *p);
extern void __assert(const u8 *file, u32 line, const u8 *msg);
extern void fn_800E24B0(u32 status);
extern void fn_800E209C(u32 status);
extern u32 fn_800A13F8(void);
extern void OSYieldThread(void);
extern void fn_800FF730(u32 id);
extern void floorSetFadeScript(u32 a, u32 b);
extern u32 GSresGetResource(u32 ctx, u32 id);

/* Storage used by the battle-status window renderer below. */
extern u8 lbl_8047C200;
extern u8 lbl_8047C204;
extern f32 lbl_8047C208;
extern u8 lbl_8047C20C;
extern u8 lbl_8047C210;
extern u8 lbl_8047C214;
extern u8 lbl_8047C218;
extern u8 lbl_8047C21C;
extern u8 lbl_8047C220;
extern u8 lbl_8047C228;
extern f32 lbl_8047C230;
extern f32 lbl_8047C234;
extern f32 lbl_8047C238;
extern void fn_801040F0();
extern void winSpriteSetDisp();
extern void fn_8001E58C();
extern void fn_800FA280();
extern void fn_800FA444();
extern void fn_800FB680();
extern void fn_800FB8C8();
extern void fn_800FBB34();

extern u16* windowGetKeyInfo(void);
extern void* pokemonDataBiosGetPtr(u32 id);
extern u8 pokemonBiosGetCatchBallId(void* pokemon);
extern u16 pokemonGetSoubiItemDataId(void* pokemon);
extern u32 pokemonDataBiosGetName(void* bios);
extern u32 GSmsgGetGSchar(u32 id);
extern u32 GSmsgGetRect(u32 id);
extern void msgctrlSetValue(u32 id, u32 value);
extern void windowDrawSprite(s32 x, s32 y, void* win, u16 sprite, u32 data);
extern void windowDrawSprite2(s32 x, s32 y, s16 w, s16 h, s32 color, s32 data, s32 sprite, s32 arg7);
extern void* menuModelRender(void* data);
extern void fn_800D88DC(u32 arg);
extern void fn_800D888C(u32 arg);
extern void fn_800D6A00(u32 arg);
extern void fn_800D7820(void* arg);
extern void fn_800D85D4(u32 arg0, void* arg1);
extern void fn_800D67BC(u32 arg);
extern void fn_800D61E4(s32 x, s32 y);
extern void fn_800D5CB8(u32 arg0, u32 r, u32 g, u32 b, u32 a);
extern void fn_800D59B8(u32 arg0, f32 s, f32 t);
extern void fn_800D6728(void);
extern u32 fn_8001D624(void* pokemon, u32 arg);
extern u8 menuSubGetPokemonSexForDisp(void* pokemon);

#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)

void fn_80094650(u32 r3, u32 r4) {
    extern void fn_8010C46C();
    extern void fn_8011BEB4();
    extern void fn_80123CD4();
    extern void fn_80123E70();
    extern void fn_8012640C();
    extern void fn_80132A38();
    extern void fn_801EE034();
    extern void fn_801EE04C();
    extern void fn_801EE064();
    extern void fn_801EE07C();
    extern void fn_801EE0A8();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;

    r27 = r3;
    r31 = r4;
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r29 = *(u32*)((u8*)r3 + 0xC);
    if (r29 == 0) return;
    tmp = *(s16*)((u8*)r31 + 0x6);
    r30 = 0x1;
    if ((s32)tmp < 0x1b8) {
        if ((s32)tmp < 0x18b) {
            if ((s32)tmp >= 0x182) goto L_8009473C;
            if ((s32)tmp < 0x170) {
                goto L_8009473C;
            }
            if ((s32)tmp < 0x191) {
            }
            goto L_800946D0;
        }
        if ((s32)tmp < 0x1d3) {
        }
        if ((s32)tmp < 0x1ca) {

        } else {
        }
        if ((s32)tmp >= 0x1d9) goto L_8009473C;
    }
L_800946D0:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x1);
    if ((s32)tmp != 7) {
        if ((s32)tmp >= 7 || (s32)tmp >= 5) goto L_8009472C;

        if ((s32)tmp < 3) {
            goto L_8009472C;
        }
        }
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x2);
    tmp = (s8)tmp;
    if ((s32)tmp >= 0 || (s32)tmp > 4) {

        r30 = 0x1;
        goto L_80094730;
    }
    r30 = 0x0;
    goto L_80094730;
L_8009472C:
    r30 = 0x0;
L_80094730:
    r3 = r31;
    r4 = r30;
    winSpriteSetDisp(r3, r4);
L_8009473C:
    tmp = r30 & 0xFF;
    if (tmp == 0) return;
    r4 = *(s16*)((u8*)r31 + 0x6);
    tmp = -0x100;
    r3 = *(u8*)((u8*)r27 + 0x8B);
    r30 = r3 | tmp;
    if ((s32)r4 < 0x1c1) {
        if ((s32)r4 != 0x18d) {
            if ((s32)r4 < 0x18d) {
                if ((s32)r4 == 0x181) return;
                if ((s32)r4 < 0x181) {
                    if ((s32)r4 == 0x170) goto L_80094F3C;
                    if ((s32)r4 < 0x170) return;
                    if ((s32)r4 >= 0x179) goto L_800952B4;
                    goto L_80095490;
                }
                if ((s32)r4 == 0x18b) goto L_80095010;
                if ((s32)r4 >= 0x18b) goto L_800950A8;
                if ((s32)r4 >= 0x187) return;
                goto L_800949F4;
            }
            if ((s32)r4 < 0x196) {
                if ((s32)r4 == 0x190) return;
                if ((s32)r4 >= 0x190) goto L_80094B58;
                if ((s32)r4 < 0x18f) return;

            }
            if ((s32)r4 == 0x1b8) goto L_80094F3C;
            if ((s32)r4 >= 0x1b8) goto L_80095490;
            if ((s32)r4 >= 0x19b) return;
            goto L_80094CB8;
        }
        if ((s32)r4 == 0x1d7) goto L_800951F4;
        if ((s32)r4 < 0x1d7) {
            if ((s32)r4 == 0x1d3) goto L_80095010;
            if ((s32)r4 < 0x1d3) {
                if ((s32)r4 == 0x1c9) return;
                if ((s32)r4 < 0x1c9) goto L_800952B4;
                if ((s32)r4 >= 0x1ce) return;
                goto L_800949F4;
            }
        }
        if ((s32)r4 == 0x1d5) goto L_80095134;
        if ((s32)r4 >= 0x1d5) return;
        goto L_800950A8;
    }
    if ((s32)r4 < 0x59b) {
        if ((s32)r4 < 0x1dd) {
            if ((s32)r4 < 0x1d9) return;

        }
        if ((s32)r4 >= 0x1e1) return;
        goto L_80094CB8;
    }
    if ((s32)r4 < 0x12b3) {
        if ((s32)r4 >= 0x59f) return;
    } else {

        if ((s32)r4 >= 0x12b8) return;
    }
    r3 = *(u32*)&lbl_8047C200;
    tmp = *(u32*)&lbl_8047C204;
    *(u32*)(sp + 0x14) = tmp;
    if ((s32)r4 != 0x12b3) {
        if ((s32)r4 < 0x12b3) {
            if ((s32)r4 != 0x59d) {
                if ((s32)r4 < 0x59d) {
                    if ((s32)r4 != 0x59b) {
                        if ((s32)r4 < 0x59b) {
                            goto L_80094910;
                        }
                        if ((s32)r4 >= 0x59f) goto L_80094910;
                        goto L_800948E4;
                    }
                    if ((s32)r4 == 0x12b6) goto L_8009490C;
                    if ((s32)r4 < 0x12b6) {
                        if ((s32)r4 >= 0x12b5) goto L_80094904;
                        goto L_800948FC;
                    }
                    if ((s32)r4 >= 0x12b8) goto L_80094910;
                    goto L_800948EC;
                        }
                r28 = 0x0;
                goto L_80094910;
                        }
            r28 = 0x1;
            goto L_80094910;
            }
        r28 = 0x2;
        goto L_80094910;
    L_800948E4:
        r28 = 0x3;
        goto L_80094910;
    L_800948EC:
        r28 = 0x0;
        goto L_80094910;
    }
    r28 = 0x1;
    goto L_80094910;
L_800948FC:
    r28 = 0x2;
    goto L_80094910;
L_80094904:
    r28 = 0x3;
    goto L_80094910;
L_8009490C:
    r28 = 0x4;
L_80094910:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    tmp = *(u8*)((u8*)r3 + 0x1);
    if ((s32)tmp != 4) {
        if ((s32)tmp < 4) {
            if ((s32)tmp < 3) return;

        }
        if ((s32)tmp != 7) return;

    }
    r3 = *(u8*)((u8*)r3 + 0x3);
    tmp = (s8)r28;
    r3 = (s8)r3;
    if ((s32)r3 == (s32)tmp) {
        r7 = (u32)sp + 0x10;
        r3 = 0x0;
        r4 = 0x0;
        *(u32*)(sp + 0x10) = tmp;
        r5 = *(s16*)((u8*)r31 + 0x54);
        r6 = *(s16*)((u8*)r31 + 0x56);
        ((void(*)(void))fn_8001E58C)();
    }
    r3 = (u32)&lbl_803FB380;
    tmp = (s8)r28;
    r3 = (u32)&lbl_803FB380;
    r3 = *(u8*)((u8*)r3 + 0x2);
    r3 = (s8)r3;
    if ((s32)r3 != (s32)tmp) return;
    r7 = (u32)sp + 0xc;
    r3 = 0x0;
    r4 = 0x0;
    *(u32*)(sp + 0xC) = tmp;
    r5 = *(s16*)((u8*)r31 + 0x54);
    r6 = *(s16*)((u8*)r31 + 0x56);
    ((void(*)(void))fn_8001E58C)();
    return;

    r3 = (u32)&lbl_803FB380;
    tmp = (s8)r28;
    r3 = (u32)&lbl_803FB380;
    r3 = *(u8*)((u8*)r3 + 0x2);
    r3 = (s8)r3;
    if ((s32)r3 != (s32)tmp) return;
    r7 = (u32)sp + 0x8;
    r3 = 0x0;
    r4 = 0x0;
    *(u32*)(sp + 0x8) = tmp;
    r5 = *(s16*)((u8*)r31 + 0x54);
    r6 = *(s16*)((u8*)r31 + 0x56);
    ((void(*)(void))fn_8001E58C)();
    return;
L_800949F4:
do {
    if ((s32)r4 != 0x186) {
        if ((s32)r4 < 0x186) {
            if ((s32)r4 != 0x183) {
                if ((s32)r4 < 0x183) {
                    if ((s32)r4 < 0x182) {
                        break;
                    }
                    if ((s32)r4 < 0x185) {
                        goto L_80094A78;
                    }
                    if ((s32)r4 != 0x1cc) {
                        if ((s32)r4 < 0x1cc) {
                            if ((s32)r4 != 0x1ca) {
                                if ((s32)r4 < 0x1ca) {
                                    break;
                                }
                                if ((s32)r4 >= 0x1ce) break;
                                r28 = 0x0;
                                break;
                            }
                            r28 = 0x1;
                            break;
                                }
                        r28 = 0x2;
                        break;
                            }
                    r28 = 0x3;
                    break;
                }
                r28 = 0x0;
                break;
                    }
            r28 = 0x1;
            break;
        L_80094A78:
            r28 = 0x2;
            break;
                }
        r28 = 0x3;
        break;
                    }
    r28 = 0x4;
} while (0);
    r30 = r28 & 0xFFFF;
    if (r30 == 4) {
        r3 = (u32)&lbl_803FB380;
        r3 = (u32)&lbl_803FB380;
        r28 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r30;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r28 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r30;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r28 = 0x0;
        }
    }
    tmp = r28 & 0xFFFF;
    if ((s32)tmp != 0x164) {
        if ((s32)tmp < 0x164) {
            if ((s32)tmp != 0) {
                goto L_80094B14;
            }
            if ((s32)tmp >= 0x166) goto L_80094B14;
            goto L_80094B0C;
        }
            }
    tmp = 0x0;
    goto L_80094B34;
L_80094B0C:
    tmp = 0x5d;
    goto L_80094B34;
L_80094B14:
    r4 = r28;
    r3 = 0x0;
    r5 = 0x3;
    r6 = 0x0;
    fn_8011BEB4();
    r3 = r3 & 0xFFFF;
    fn_8010C46C();
    tmp = r3 & 0xFFFF;
L_80094B34:
    if (tmp == 0) return;
    r5 = r27;
    r6 = tmp & 0xFFFF;
    r3 = 0x0;
    r4 = 0x0;
    r7 = 0x0;
    ((void(*)(void))fn_801040F0)();
    return;
L_80094B58:
do {
    if ((s32)r4 != 0x195) {
        if ((s32)r4 < 0x195) {
            if ((s32)r4 != 0x192) {
                if ((s32)r4 < 0x192) {
                    if ((s32)r4 < 0x191) {
                        break;
                    }
                    if ((s32)r4 < 0x194) {
                        goto L_80094BDC;
                    }
                    if ((s32)r4 != 0x1db) {
                        if ((s32)r4 < 0x1db) {
                            if ((s32)r4 != 0x1d9) {
                                if ((s32)r4 < 0x1d9) {
                                    break;
                                }
                                if ((s32)r4 >= 0x1dd) break;
                                r28 = 0x0;
                                break;
                            }
                            r28 = 0x1;
                            break;
                                }
                        r28 = 0x2;
                        break;
                            }
                    r28 = 0x3;
                    break;
                }
                r28 = 0x0;
                break;
                    }
            r28 = 0x1;
            break;
        L_80094BDC:
            r28 = 0x2;
            break;
                }
        r28 = 0x3;
        break;
                    }
    r28 = 0x4;
} while (0);
    r28 = r28 & 0xFFFF;
    if (r28 == 4) {
        r3 = (u32)&lbl_803FB380;
        r3 = (u32)&lbl_803FB380;
        r27 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r28;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r27 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r28;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r27 = 0x0;
        }
    }
    tmp = r27 & 0xFFFF;
    if (tmp == 0) {
        r5 = *(s16*)((u8*)r31 + 0x54);
        r7 = r30;
        r6 = *(s16*)((u8*)r31 + 0x56);
        r3 = 0x0;
        r4 = 0x0;
        r8 = 0x2be0;
        ((void(*)(void))fn_800FBB34)();
        return;
    }
    r4 = r27;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x0;
    fn_8011BEB4();
    if (r3 == 0) return;
    ((void(*)(void))fn_800FA280)();
    r4 = r3;
    r3 = 0x37;
    fn_80132A38();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r7 = r30;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xe7;
    ((void(*)(void))fn_800FBB34)();
    return;
L_80094CB8:
do {
    if ((s32)r4 != 0x19a) {
        if ((s32)r4 < 0x19a) {
            if ((s32)r4 != 0x197) {
                if ((s32)r4 < 0x197) {
                    if ((s32)r4 < 0x196) {
                        break;
                    }
                    if ((s32)r4 < 0x199) {
                        goto L_80094D3C;
                    }
                    if ((s32)r4 != 0x1df) {
                        if ((s32)r4 < 0x1df) {
                            if ((s32)r4 != 0x1dd) {
                                if ((s32)r4 < 0x1dd) {
                                    break;
                                }
                                if ((s32)r4 >= 0x1e1) break;
                                r28 = 0x0;
                                break;
                            }
                            r28 = 0x1;
                            break;
                                }
                        r28 = 0x2;
                        break;
                            }
                    r28 = 0x3;
                    break;
                }
                r28 = 0x0;
                break;
                    }
            r28 = 0x1;
            break;
        L_80094D3C:
            r28 = 0x2;
            break;
                }
        r28 = 0x3;
        break;
                    }
    r28 = 0x4;
} while (0);
    r26 = r28 & 0xFFFF;
    if (r26 == 4) {
        r3 = (u32)&lbl_803FB380;
        r3 = (u32)&lbl_803FB380;
        r27 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r26;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r27 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r26;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r27 = 0x0;
        }
    }
    r3 = 0x2bd4;
    ((void(*)(void))fn_800FA444)();
    r3 = (u32)r3 >> 16;
    tmp = *(s16*)((u8*)r31 + 0x54);
    r3 = (s16)r3;
    r5 = r30;
    r3 = tmp - r3;
    r4 = 0x0;
    tmp = (u32)r3 >> 31;
    r6 = 0x2bd4;
    tmp = tmp + r3;
    tmp = (s32)tmp >> 1;
    r25 = (s16)tmp;
    r3 = r25;
    ((void(*)(void))fn_800FB680)();
    tmp = r27 & 0xFFFF;
    if ((s32)tmp != 0x164) {
        if ((s32)tmp < 0x164) {
            if ((s32)tmp != 0) {
                goto L_80094E7C;
            }
            if ((s32)tmp >= 0x166) goto L_80094E7C;
            goto L_80094E40;
        }
            }
    r6 = *(s16*)((u8*)r31 + 0x56);
    r5 = r25;
    r7 = r30;
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0x2be1;
    ((void(*)(void))fn_800FB8C8)();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r7 = r30;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0x2be1;
    ((void(*)(void))fn_800FB8C8)();
    return;
L_80094E40:
    r6 = *(s16*)((u8*)r31 + 0x56);
    r5 = r25;
    r7 = r30;
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0x2b6d;
    ((void(*)(void))fn_800FB8C8)();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r7 = r30;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0x2b6d;
    ((void(*)(void))fn_800FB8C8)();
    return;
L_80094E7C:
    tmp = r28 & 0xFFFF;
    if (tmp == 4) {
        r4 = r27;
        r3 = 0x0;
        r5 = 0x2;
        r6 = 0x0;
        fn_8011BEB4();
    } else {

        r3 = r29;
        r6 = r26;
        r4 = 0x0;
        r5 = 0x80;
        fn_8012640C();
    }
    r4 = r3;
    r3 = 0x34;
    fn_80132A38();
    r6 = *(s16*)((u8*)r31 + 0x56);
    r5 = r25;
    r7 = r30;
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xd2;
    ((void(*)(void))fn_800FB8C8)();
    tmp = r28 & 0xFFFF;
    if (tmp == 4) {
        r4 = r27;
        r3 = 0x0;
        r5 = 0x2;
        r6 = 0x0;
        fn_8011BEB4();
    } else {

        r3 = r29;
        r4 = r28;
        fn_80123E70();
        r3 = r3 & 0xFF;
    }
    r4 = r3;
    r3 = 0x34;
    fn_80132A38();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r7 = r30;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xd2;
    ((void(*)(void))fn_800FB8C8)();
    return;
L_80094F3C:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r30 = *(u8*)((u8*)r3 + 0x2);
    r30 = (s8)r30;
    tmp = r30 & 0xFFFF;
    if (tmp == 4) {
        r28 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r30;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r28 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r30;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r28 = 0x0;
        }
    }
    tmp = r28 & 0xFFFF;
    if ((s32)tmp != 0x164) {
        if ((s32)tmp < 0x164) {
            if ((s32)tmp != 0) {
                goto L_80094FCC;
            }
            if ((s32)tmp >= 0x166) goto L_80094FCC;
            goto L_80094FC4;
        }
            }
    tmp = 0x0;
    goto L_80094FEC;
L_80094FC4:
    tmp = 0x5d;
    goto L_80094FEC;
L_80094FCC:
    r4 = r28;
    r3 = 0x0;
    r5 = 0x24;
    r6 = 0x0;
    fn_8011BEB4();
    r3 = r3 & 0xFF;
    fn_801EE0A8();
    tmp = r3 & 0xFFFF;
L_80094FEC:
    if (tmp == 0) return;
    r5 = r27;
    r6 = tmp & 0xFFFF;
    r3 = 0x0;
    r4 = 0x0;
    r7 = 0x0;
    ((void(*)(void))fn_801040F0)();
    return;
L_80095010:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r28 = *(u8*)((u8*)r3 + 0x2);
    r28 = (s8)r28;
    tmp = r28 & 0xFFFF;
    if (tmp == 4) {
        r27 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r28;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r27 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r28;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r27 = 0x0;
        }
    }
    r4 = r27;
    r3 = 0x0;
    r5 = 0x23;
    r6 = 0x0;
    fn_8011BEB4();
    r3 = r3 & 0xFFFF;
    fn_801EE07C();
    fn_801EE034();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r8 = r3;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r7 = r30;
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_800FBB34)();
    return;
L_800950A8:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r28 = *(u8*)((u8*)r3 + 0x2);
    r28 = (s8)r28;
    tmp = r28 & 0xFFFF;
    if (tmp == 4) {
        r27 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r28;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r27 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r28;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r27 = 0x0;
        }
    }
    r4 = r27;
    r3 = 0x0;
    r5 = 0x22;
    r6 = 0x0;
    fn_8011BEB4();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r8 = r3;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r7 = r30;
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_800FBB34)();
    return;
L_80095134:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r28 = *(u8*)((u8*)r3 + 0x2);
    r28 = (s8)r28;
    tmp = r28 & 0xFFFF;
    if (tmp == 4) {
        r27 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r28;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r27 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r28;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r27 = 0x0;
        }
    }
    r4 = r27;
    r3 = 0x0;
    r5 = 0x6;
    r6 = 0x0;
    fn_8011BEB4();
    if (r3 <= 1) {
        r5 = *(s16*)((u8*)r31 + 0x54);
        r7 = r30;
        r6 = *(s16*)((u8*)r31 + 0x56);
        r3 = 0x0;
        r4 = 0x0;
        r8 = 0x2be2;
        ((void(*)(void))fn_800FB8C8)();
        return;
    }
    r4 = r3;
    r3 = 0x34;
    fn_80132A38();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r7 = r30;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xd2;
    ((void(*)(void))fn_800FB8C8)();
    return;
L_800951F4:
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r28 = *(u8*)((u8*)r3 + 0x2);
    r28 = (s8)r28;
    tmp = r28 & 0xFFFF;
    if (tmp == 4) {
        r27 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r28;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r27 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r28;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r27 = 0x0;
        }
    }
    r4 = r27;
    r3 = 0x0;
    r5 = 0x7;
    r6 = 0x0;
    fn_8011BEB4();
    if (r3 <= 1) {
        r5 = *(s16*)((u8*)r31 + 0x54);
        r7 = r30;
        r6 = *(s16*)((u8*)r31 + 0x56);
        r3 = 0x0;
        r4 = 0x0;
        r8 = 0x2be2;
        ((void(*)(void))fn_800FB8C8)();
        return;
    }
    r4 = r3;
    r3 = 0x34;
    fn_80132A38();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r7 = r30;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r8 = 0xd2;
    ((void(*)(void))fn_800FB8C8)();
    return;
L_800952B4:
do {
    if ((s32)r4 != 0x1c1) {
        if ((s32)r4 < 0x1c1) {
            if ((s32)r4 != 0x17d) {
                if ((s32)r4 < 0x17d) {
                    if ((s32)r4 != 0x17a) {
                        if ((s32)r4 < 0x17a) {
                            if ((s32)r4 < 0x179) {
                                break;
                            }
                            if ((s32)r4 < 0x17c) {
                                goto L_800953A4;
                            }
                            if ((s32)r4 != 0x180) {
                                if ((s32)r4 >= 0x180) break;
                                if ((s32)r4 < 0x17f) {
                                    goto L_8009538C;
                                }
                                if ((s32)r4 != 0x1c6) {
                                    if ((s32)r4 < 0x1c6) {
                                        if ((s32)r4 != 0x1c4) {
                                            if ((s32)r4 < 0x1c4) {
                                                if ((s32)r4 < 0x1c3) {
                                                    goto L_8009536C;
                                                }
                                                if ((s32)r4 != 0x1c8) {
                                                    if ((s32)r4 >= 0x1c8) break;

                                                } else {
                                                    r28 = 0x1;
                                                    break;
                                                }
                                                r28 = 0x2;
                                                break;
                                            }
                                            r28 = 0x3;
                                            break;
                                                }
                                        r28 = 0x4;
                                        break;
                                            }
                                    r28 = 0x5;
                                    break;
                                                }
                                r28 = 0x6;
                                break;
                            L_8009536C:
                                r28 = 0x7;
                                break;
                            }
                            r28 = 0x8;
                            break;
                                }
                        r28 = 0x1;
                        break;
                                }
                    r28 = 0x2;
                    break;
                L_8009538C:
                    r28 = 0x3;
                    break;
                        }
                r28 = 0x4;
                break;
                            }
            r28 = 0x5;
            break;
        L_800953A4:
            r28 = 0x6;
            break;
                        }
        r28 = 0x7;
        break;
                            }
    r28 = 0x8;
} while (0);
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r31 = *(u8*)((u8*)r3 + 0x2);
    r31 = (s8)r31;
    tmp = r31 & 0xFFFF;
    if (tmp == 4) {
        r30 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r31;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r30 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r31;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r30 = 0x0;
        }
    }
    tmp = r30 & 0xFFFF;
    if (tmp != 0) {
        r4 = r30;
        r3 = 0x0;
        r5 = 0x23;
        r6 = 0x0;
        fn_8011BEB4();
        r3 = r3 & 0xFFFF;
        fn_801EE07C();
        fn_801EE064();
        r4 = r3 & 0xFF;
    } else {

        r4 = 0x0;
    }
    r3 = 0x66660000;
    tmp = r28 & 0xFFFF;
    r3 = r3 + 0x6667;
    r5 = r27;
    r6 = (s32)((s64)r3 * (s64)r4 >> 32);
    r3 = 0x0;
    r4 = 0x0;
    r6 = (s32)r6 >> 2;
    r7 = (u32)r6 >> 31;
    r6 = r6 + r7;
    if ((s32)r6 >= (s32)tmp) {
        r6 = 0xf6;
    } else {

        r6 = 0xf5;
    }
    r7 = 0x0;
    ((void(*)(void))fn_801040F0)();
    return;
L_80095490:
do {
    if ((s32)r4 != 0x1b9) {
        if ((s32)r4 < 0x1b9) {
            if ((s32)r4 != 0x175) {
                if ((s32)r4 < 0x175) {
                    if ((s32)r4 != 0x172) {
                        if ((s32)r4 < 0x172) {
                            if ((s32)r4 < 0x171) {
                                break;
                            }
                            if ((s32)r4 < 0x174) {
                                goto L_80095580;
                            }
                            if ((s32)r4 != 0x178) {
                                if ((s32)r4 >= 0x178) break;
                                if ((s32)r4 < 0x177) {
                                    goto L_80095568;
                                }
                                if ((s32)r4 != 0x1be) {
                                    if ((s32)r4 < 0x1be) {
                                        if ((s32)r4 != 0x1bc) {
                                            if ((s32)r4 < 0x1bc) {
                                                if ((s32)r4 < 0x1bb) {
                                                    goto L_80095548;
                                                }
                                                if ((s32)r4 != 0x1c0) {
                                                    if ((s32)r4 >= 0x1c0) break;

                                                } else {
                                                    r28 = 0x1;
                                                    break;
                                                }
                                                r28 = 0x2;
                                                break;
                                            }
                                            r28 = 0x3;
                                            break;
                                                }
                                        r28 = 0x4;
                                        break;
                                            }
                                    r28 = 0x5;
                                    break;
                                                }
                                r28 = 0x6;
                                break;
                            L_80095548:
                                r28 = 0x7;
                                break;
                            }
                            r28 = 0x8;
                            break;
                                }
                        r28 = 0x1;
                        break;
                                }
                    r28 = 0x2;
                    break;
                L_80095568:
                    r28 = 0x3;
                    break;
                        }
                r28 = 0x4;
                break;
                            }
            r28 = 0x5;
            break;
        L_80095580:
            r28 = 0x6;
            break;
                        }
        r28 = 0x7;
        break;
                            }
    r28 = 0x8;
} while (0);
    r3 = (u32)&lbl_803FB380;
    r3 = (u32)&lbl_803FB380;
    r31 = *(u8*)((u8*)r3 + 0x2);
    r31 = (s8)r31;
    tmp = r31 & 0xFFFF;
    if (tmp == 4) {
        r30 = *(u16*)((u8*)r3 + 0x18);

    } else {
        r3 = r29;
        r6 = r31;
        r4 = 0x0;
        r5 = 0x7f;
        fn_8012640C();
        r30 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r31;
        fn_80123CD4();
        tmp = r3 & 0xFF;
        if (tmp == 0) {
            r30 = 0x0;
        }
    }
    tmp = r30 & 0xFFFF;
    if (tmp != 0) {
        r4 = r30;
        r3 = 0x0;
        r5 = 0x23;
        r6 = 0x0;
        fn_8011BEB4();
        r3 = r3 & 0xFFFF;
        fn_801EE07C();
        fn_801EE04C();
        r4 = r3 & 0xFF;
    } else {

        r4 = 0x0;
    }
    r3 = 0x66660000;
    tmp = r28 & 0xFFFF;
    r3 = r3 + 0x6667;
    r5 = r27;
    r6 = (s32)((s64)r3 * (s64)r4 >> 32);
    r3 = 0x0;
    r4 = 0x0;
    r6 = (s32)r6 >> 2;
    r7 = (u32)r6 >> 31;
    r6 = r6 + r7;
    if ((s32)r6 >= (s32)tmp) {
        r6 = 0xf7;
    } else {

        r6 = 0xf5;
    }
    r7 = 0x0;
    ((void(*)(void))fn_801040F0)();

    return;
}



/* 0x80091564 | size: 0x210 */
#pragma push
/* Battle-status detail window renderer. */
void fn_8009567C(u8* context, u8* sprite)
{
    extern u32 pokemonGetStatus();
    extern void* pokemonDataBiosGetPtr();
    extern u8 pokemonDataBiosGetZokuseiDataId();
    extern u32 fn_8010C46C();
    extern u8 pokemonGetSex();
    extern u8 pokemonGetNowLevel();
    extern u8 pokemonIsDarkPokemon();
    extern u32 pokemonGetLevelToExp();
    extern u32 pokemonGetNowLevelToExp();
    extern u32 fn_8011CE00();
    extern u32 fn_8011CE18();
    extern u32 fn_8011396C();
    extern u32 fn_801248C4();
    extern u32 fn_8011CB6C();
    extern void fn_8011CB3C();
    extern void fn_8011CB54();
    extern u32 fn_801229F4();
    extern u8 fn_8011FC14();
    extern u8 fn_8011FC74();
    extern u32 fn_8011F77C();
    extern u32 fn_80129280();
    extern u32 fn_8012AC3C();
    extern u32 fn_8012AC54();
    extern u32 fn_80135938();
    extern u32 fn_801906A0();
    extern u32 fn_801F2A7C();
    extern u32 fn_801FCEAC();
    extern u32 fn_8006AEEC();
    extern u32 fn_800F9EE4();
    extern u32 fn_800FA280();
    extern u32 fn_800FA444();
    extern void fn_80132A38();
    extern void fn_801040F0();
    extern void fn_80104160();
    extern void fn_800FB680();
    extern void fn_800FBB34();
    extern void winSpriteSetDisp();
    u8* pokemon;
    u8* pokemon_data;
    s16 window_id;
    u32 color;
    u32 value;
    u32 message;
    u32 x;
    u32 y;
    u32 level;
    u32 next_exp;
    u32 current_exp;
    u32 trainer_name;
    u32 trainer_id;
    u32 valid;

    pokemon = *(u8**)(lbl_803FB380 + 0x0C);
    if (pokemon == NULL) {
        return;
    }

    pokemon_data = pokemonDataBiosGetPtr((u16)pokemonGetStatus(pokemon, 0, 0x6E, 0));
    if (pokemon_data == NULL) {
        return;
    }

    window_id = *(s16*)(sprite + 6);
    valid = 1;
    if ((window_id >= 0x170 && window_id < 0x182) ||
        (window_id >= 0x18B && window_id < 0x191) ||
        (window_id >= 0x1B8 && window_id < 0x1CA) ||
        (window_id >= 0x1D3 && window_id < 0x1D9)) {
        s32 page = (s8)lbl_803FB380[2];
        u32 menu = lbl_803FB380[1];
        valid = ((menu >= 3 && menu < 5) || menu == 7) && page >= 0 && page <= 4;
        winSpriteSetDisp(sprite, valid);
    }
    if (!valid) {
        return;
    }

    color = 0xFFFFFF00 | context[0x8B];
    x = *(s16*)(sprite + 0x54);
    y = *(s16*)(sprite + 0x56);

    switch (window_id) {
    case 0x13B:
        value = (u16)fn_8010C46C(
            (u8)pokemonDataBiosGetZokuseiDataId(pokemon_data, 0));
        fn_801040F0(0, 0, context, value, 0);
        break;
    case 0x13C:
        value = (u8)pokemonDataBiosGetZokuseiDataId(pokemon_data, 0);
        message = (u8)pokemonDataBiosGetZokuseiDataId(pokemon_data, 1);
        if (value != message) {
            message = (u16)fn_8010C46C(message);
            fn_801040F0(0, 0, context, message, 0);
        }
        break;
    case 0x142:
        if ((u8)fn_8011F77C(pokemon) < 3) {
            value = 0x934;
        } else {
            value = fn_8011CE00(fn_8011CE18((u8)pokemonGetStatus(pokemon, 0, 0xBF, 0)));
        }
        fn_80132A38(0x55, value);
        fn_80132A38(0x56, value == 0xC86 || value == 0xC96 ? 1 : 0x2BD8);
        value = (u8)pokemonGetStatus(pokemon, 0, 0x72, 0);
        fn_80132A38(0x34, value == 0 ? 5 : value);
        trainer_id = pokemonGetStatus(pokemon, 0, 0x75, 0);
        trainer_name = pokemonGetStatus(pokemon, 0, 0x76, 0);
        message = 0x2BCD;
        if (trainer_id == fn_8012AC3C(lbl_803FB380[8]) &&
            fn_800F9EE4(fn_8012AC54(lbl_803FB380[8]), trainer_name) == 0) {
            value = (u16)pokemonGetStatus(pokemon, 0, 0x6E, 0);
            message = (value >= 0xC4 && value < 0xC6) ? 0x2BE3 : 0x2BCD;
        }
        fn_800FBB34(0, 0, x, y, color, message);
        break;
    case 0x147:
        value = (s16)pokemonGetStatus(pokemon, 0, 0x8B, 0);
        fn_80132A38(0x34, value);
        fn_800FBB34(0, 0, x, y, color, 0xDE);
        break;
    case 0x148:
    case 0x54D:
        value = (s16)pokemonGetStatus(pokemon, 0, 0x8C, 0);
        fn_80132A38(0x34, value);
        fn_800FBB34(0, 0, x, y, color, 0xDE);
        break;
    case 0x149:
        value = (s16)pokemonGetStatus(pokemon, 0, 0x8A, 0);
        fn_80132A38(0x34, value);
        fn_800FBB34(0, 0, x, y, color, 0xDE);
        break;
    case 0x14A:
        value = (s16)pokemonGetStatus(pokemon, 0, 0x89, 0);
        fn_80132A38(0x34, value);
        fn_800FBB34(0, 0, x, y, color, 0xDE);
        break;
    case 0x14B:
    case 0x587:
        value = (s16)pokemonGetStatus(pokemon, 0, 0x88, 0);
        fn_80132A38(0x34, value);
        fn_800FBB34(0, 0, x, y, color, 0xDE);
        break;
    case 0x14C:
    case 0x581:
        value = (u16)pokemonGetStatus(pokemon, 0, 0x83, 0);
        fn_80132A38(0x34, value);
        fn_800FBB34(0, 0, 0x37, y, color, 0xDE);
        fn_800FB680(0x37, 0, color, 0x2BD4);
        value = (u16)pokemonGetStatus(pokemon, 0, 0x87, 0);
        fn_80132A38(0x34, value);
        fn_800FBB34(0, 0, x, y, color, 0xDE);
        break;
    case 0x153:
    case 0x58F:
        value = fn_8011CB6C((u16)fn_801248C4(pokemon));
        fn_8011CB3C();
        value = fn_800FA280();
        fn_80132A38(0x37, value);
        fn_800FBB34(0, 0, x, y, color, 0xCF);
        break;
    case 0x154:
    case 0x591:
        trainer_id = (s16)(fn_800FA444(*(u32*)(sprite + 0x4C)) >> 16);
        if (fn_8011FC74(pokemon) == 1) {
            fn_800FB680(trainer_id, 0, color, 0x2B70);
            break;
        }
        value = (u16)pokemonGetStatus(pokemon, 0, 0x75, 0);
        for (level = 0, next_exp = 10000; level < 5; level++) {
            fn_80132A38(0x34, value / next_exp);
            value %= next_exp;
            next_exp /= 10;
            fn_800FB680(trainer_id, 0, color, 0xCA);
            trainer_id += 13;
        }
        break;
    case 0x155:
    case 0x592:
        if (fn_8011FC74(pokemon) == 1) {
            fn_80132A38(0x37, fn_800FA280(0x2B70));
        } else {
            fn_80132A38(0x37, pokemonGetStatus(pokemon, 0, 0x76, 0));
        }
        fn_800FB680((s16)(fn_800FA444(*(u32*)(sprite + 0x4C)) >> 16), 0, color, 0xCF);
        break;
    case 0x158:
        level = (u8)pokemonGetStatus(pokemon, 0, 0x7A, 0);
        next_exp = fn_801229F4(pokemon, level + 1);
        if (next_exp != 0) {
            current_exp = fn_801229F4(pokemon, level);
            value = pokemonGetStatus(pokemon, 0, 0x79, 0) - current_exp;
            fn_80104160(0, 0, (s16)(x + value * *(s16*)(sprite + 0x54) / (next_exp - current_exp)), y,
                         color, context, 0x117, 0);
        }
        break;
    case 0x54E:
    case 0x57B:
        value = pokemonGetStatus(pokemon, 0, 0x79, 0);
        fn_80132A38(0x34, value);
        fn_800FBB34(0, 0, x, y, color, 0xDE);
        break;
    case 0x57D:
    case 0x582:
        value = fn_8011F77C(pokemon);
        message = 0x2BD9 + (value < 7 ? value : 0);
        fn_800FB680(0, 0, color, message);
        break;
    case 0x584:
    case 0x585:
        level = (u8)pokemonGetStatus(pokemon, 0, 0x7A, 0);
        next_exp = fn_801229F4(pokemon, level + 1);
        current_exp = pokemonGetStatus(pokemon, 0, 0x79, 0);
        fn_80132A38(0x34, next_exp != 0 ? next_exp - current_exp : 0);
        fn_800FBB34(0, 0, x, y, color, 0xDE);
        break;
    case 0x595:
    case 0x599:
    case 0x12B8:
    case 0x12B9:
    case 0x12BA:
    case 0x12BB:
        value = (u16)pokemonGetStatus(pokemon, 0, 0xC4, 0);
        level = value == 0 ? 0 : pokemonGetNowLevel(pokemon);
        if (level >= (u16)(window_id - 0x12B7)) {
            fn_80104160(0, 0, x, y, color, context, 0x116, 0);
        }
        break;
    default:
        break;
    }
}



#pragma pop
/* 0x80091DA4 | size: 0x1A4 */
void fn_80091DA4(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void fn_801CB7C4(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void fn_801CB834(u32 id, u32 slot, u32 x, u32 y);
    extern void scriptWaitSyncMotion(u32 id, u32 val);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 fn_800D3088(void);
    extern u32 fn_801CBA0C(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void fn_801845E4(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void fn_800FF58C(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);
    extern void fn_80118874(void *texture, u32 flag);
    extern u32 fn_80113F48(void);

    u32 elapsed;
    u32 waitFrames;
    u32 cameraElapsed;
    u32 cameraWaitFrames;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    fn_801CB7C4(0x0CE61000);
    fn_801CB834(0x0CE61004, 1, 0, 0);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x0CE61000), GSresGetResource(ctx, 0x111B1400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x0CE61000), 4);

    fn_801CB834(0x0CE61000, 3, 0, 0);
    waitFrames = 0x32;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1E0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    cameraPlayAnime(ctx, 0x0CEF1800, 0, 0);
    cameraWaitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        cameraWaitFrames = (u32)lbl_8047C1D0;
        if (cameraWaitFrames < 1) {
            cameraWaitFrames = 1;
        }
    }
    for (cameraElapsed = 0; cameraElapsed < cameraWaitFrames; ) {
        _threadSwitch();
        cameraElapsed += fn_800D3088();
    }

    cameraWaitSyncAnime(1);
    fn_800FF58C(0x82);
    floorSetFadeScript(0, 0);
}

/* 0x80091F48 | size: 0x1F8 */
void fn_80091F48(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void fn_801CB7C4(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void fn_801CB834(u32 id, u32 slot, u32 x, u32 y);
    extern void scriptWaitSyncMotion(u32 id, u32 val);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 fn_800D3088(void);
    extern u32 fn_801CBA0C(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void fn_801845E4(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void fn_800FF58C(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);
    extern void fn_80118874(void *texture, u32 flag);
    extern u32 fn_80113F48(void);

    u32 elapsed;
    u32 waitFrames;
    u32 modelA;
    u32 modelB;
    u32 shadowTarget;
    u32 animA;
    u32 animB;

    lbl_8047A690 = GSresGetResource(ctx, 0x06DD1604);
    lbl_8047A694 = GSresGetResource(ctx, 0x06DD1001);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);

    modelA = fn_801CBA0C(0x06BB0400);
    modelB = fn_801CBA0C(0x0D240400);

    shadowTarget = GSresGetResource(ctx, modelA);
    GSmodelSetShadowFlags(shadowTarget, 2);
    GSmodelSetShadowLight(shadowTarget, lbl_8047A690);
    GSmodelSetShadowSurface(shadowTarget, 1, &lbl_8047A694);

    shadowTarget = GSresGetResource(ctx, modelB);
    GSmodelSetShadowFlags(shadowTarget, 2);
    GSmodelSetShadowLight(shadowTarget, lbl_8047A690);
    GSmodelSetShadowSurface(shadowTarget, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0B891800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    animA = fn_801CBA0C(0x0B861000);
    animB = fn_801CBA0C(0x0B861001);

    fn_801845E4(ctx, modelA, ctx, animA, 0);
    fn_801845E4(ctx, modelB, ctx, animB, 0);

    fn_801CB834(modelB, 8, 0, 1);
    fn_801CB834(modelA, 8, 0x32, 0);
    scriptWaitSyncMotion(modelA, 0);
    fn_801CB834(modelA, 9, 0, 1);

    cameraWaitSyncAnime(1);
    fn_800FF58C(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x80092140 | size: 0x358 */
void fn_80092140(u32 ctx) {
    #pragma peephole off
    extern void GSmodelSetShadowTextureSize(u32, u32);
    extern u32 fn_801CBA0C(u32);
    extern u32 GSmodelSetShadowFlags(u32, u32);
    extern void GSmodelSetShadowLight(u32, u32);
    extern void GSmodelSetShadowSurface(u32, u32, u32*);
    extern void cameraPlayAnime(u32, u32, u32, u32);
    extern s32 fn_800D37CC(void);
    extern u32 fn_800D3088(void);
    extern void fn_801845E4(u32, u32, u32, u32, u32);
    extern void fn_801CB834(u32, u32, u32, u32);
    extern void cameraWaitSyncAnime(s32);
    extern void fn_800FF58C(u32);
    u32 configured, elapsed, waitFrames;
    u32 animation0, animation1, animation2, animation3, animation4;
    u32 model0, model1, model2, model3, model4;

    lbl_8047A690 = GSresGetResource(ctx, 0x06DD1604);
    lbl_8047A694 = GSresGetResource(ctx, 0x06DD1001);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    model0 = fn_801CBA0C(0x0D240400);
    model1 = fn_801CBA0C(0x0D240400);
    model2 = fn_801CBA0C(0x0D240400);
    model3 = fn_801CBA0C(0x0D240400);
    model4 = fn_801CBA0C(0x0D240400);

    configured = GSresGetResource(ctx, model0);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model1);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model2);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model3);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model4);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0B881800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames;) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    animation0 = fn_801CBA0C(0x0B851004);
    animation1 = fn_801CBA0C(0x0B851003);
    animation2 = fn_801CBA0C(0x0B851001);
    animation3 = fn_801CBA0C(0x0B851002);
    animation4 = fn_801CBA0C(0x0B851003);
    fn_801845E4(ctx, model0, ctx, animation0, 0);
    fn_801845E4(ctx, model1, ctx, animation1, 0);
    fn_801845E4(ctx, model2, ctx, animation2, 0);
    fn_801845E4(ctx, model3, ctx, animation3, 0);
    fn_801845E4(ctx, model4, ctx, animation4, 0);
    fn_801CB834(model0, 6, 0, 1);
    fn_801CB834(model1, 6, 0, 1);
    fn_801CB834(model2, 8, 0, 1);
    fn_801CB834(model3, 8, 0, 1);
    fn_801CB834(model4, 7, 0, 1);
    cameraWaitSyncAnime(1);
    fn_800FF58C(0x83);
    floorSetFadeScript(0, 0);
}

/* 0x80092498 | size: 0x1CC */
void fn_80092498(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void fn_801CB7C4(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void fn_801CB834(u32 id, u32 slot, u32 x, u32 y);
    extern void scriptWaitSyncMotion(u32 id, u32 val);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 fn_800D3088(void);
    extern u32 fn_801CBA0C(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void fn_801845E4(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void fn_800FF58C(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);
    extern void fn_80118874(void *texture, u32 flag);
    extern u32 fn_80113F48(void);

    u32 elapsed;
    u32 waitFrames;
    u32 cameraElapsed;
    u32 cameraWaitFrames;
    GSmaterialEntry *material;
    u32 particleBank;

    lbl_8047A690 = GSresGetResource(ctx, 0x0B631602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0B631002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    fn_801CB834(0x0B631000, 0, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    fn_801CB7C4(0x0B631000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0B631000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    particleBank = GSresGetResource(ctx, 0x112B1400);
    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x0B631000), particleBank);
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x0B631000), 4);

    cameraPlayAnime(ctx, 0x0B831800, 0, 0);
    cameraWaitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        cameraWaitFrames = (u32)lbl_8047C1D0;
        if (cameraWaitFrames < 1) {
            cameraWaitFrames = 1;
        }
    }
    for (cameraElapsed = 0; cameraElapsed < cameraWaitFrames; ) {
        _threadSwitch();
        cameraElapsed += fn_800D3088();
    }

    fn_801CB834(0x0B631000, 0, 0, 0);
    cameraWaitSyncAnime(1);
    fn_800FF58C(0x83);
    floorSetFadeScript(0, 0);
}

/* 0x80092664 | size: 0x358 */
void fn_80092664(u32 ctx) {
    #pragma peephole off
    extern void GSmodelSetShadowTextureSize(u32, u32);
    extern u32 fn_801CBA0C(u32);
    extern u32 GSmodelSetShadowFlags(u32, u32);
    extern void GSmodelSetShadowLight(u32, u32);
    extern void GSmodelSetShadowSurface(u32, u32, u32*);
    extern void cameraPlayAnime(u32, u32, u32, u32);
    extern s32 fn_800D37CC(void);
    extern u32 fn_800D3088(void);
    extern void fn_801845E4(u32, u32, u32, u32, u32);
    extern void fn_801CB834(u32, u32, u32, u32);
    extern void cameraWaitSyncAnime(s32);
    extern void fn_800FF58C(u32);
    u32 configured, elapsed, waitFrames;
    u32 animation0, animation1, animation2, animation3, animation4;
    u32 model0, model1, model2, model3, model4;

    lbl_8047A690 = GSresGetResource(ctx, 0x06DD1604);
    lbl_8047A694 = GSresGetResource(ctx, 0x06DD1001);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    model0 = fn_801CBA0C(0x0D240400);
    model1 = fn_801CBA0C(0x0D240400);
    model2 = fn_801CBA0C(0x0D240400);
    model3 = fn_801CBA0C(0x0D240400);
    model4 = fn_801CBA0C(0x0D240400);

    configured = GSresGetResource(ctx, model0);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model1);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model2);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model3);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model4);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0B871800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames;) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    animation0 = fn_801CBA0C(0x0B841004);
    animation1 = fn_801CBA0C(0x0B841000);
    animation2 = fn_801CBA0C(0x0B841001);
    animation3 = fn_801CBA0C(0x0B841002);
    animation4 = fn_801CBA0C(0x0B841003);
    fn_801845E4(ctx, model0, ctx, animation0, 0);
    fn_801845E4(ctx, model1, ctx, animation1, 0);
    fn_801845E4(ctx, model2, ctx, animation2, 0);
    fn_801845E4(ctx, model3, ctx, animation3, 0);
    fn_801845E4(ctx, model4, ctx, animation4, 0);
    fn_801CB834(model0, 1, 0, 1);
    fn_801CB834(model1, 1, 0, 1);
    fn_801CB834(model2, 1, 0, 1);
    fn_801CB834(model3, 2, 0, 1);
    fn_801CB834(model4, 2, 0, 1);
    cameraWaitSyncAnime(1);
    fn_800FF58C(0x87);
    floorSetFadeScript(0, 0);
}

/* 0x800929BC | size: 0x170 */
void fn_800929BC(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void fn_801CB7C4(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void fn_801CB834(u32 id, u32 slot, u32 x, u32 y);
    extern void scriptWaitSyncMotion(u32 id, u32 val);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 fn_800D3088(void);
    extern u32 fn_801CBA0C(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void fn_801845E4(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void fn_800FF58C(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);
    extern void fn_80118874(void *texture, u32 flag);
    extern u32 fn_80113F48(void);

    u32 elapsed;
    u32 waitFrames;
    u32 cameraElapsed;
    u32 cameraWaitFrames;
    GSmaterialEntry *material;

    lbl_8047A690 = GSresGetResource(ctx, 0x0B631602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0B631002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    fn_801CB834(0x0B631000, 0, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    fn_801CB7C4(0x0B631000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0B631000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    cameraPlayAnime(fn_80113F48(), 0x0B661800, 0, 0);
    cameraWaitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        cameraWaitFrames = (u32)lbl_8047C1D0;
        if (cameraWaitFrames < 1) {
            cameraWaitFrames = 1;
        }
    }
    for (cameraElapsed = 0; cameraElapsed < cameraWaitFrames; ) {
        _threadSwitch();
        cameraElapsed += fn_800D3088();
    }

    cameraWaitSyncAnime(1);
    fn_800FF58C(0x83);
    floorSetFadeScript(0, 0);
}

/* 0x80092B2C | size: 0x164 */
void fn_80092B2C(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void fn_801CB7C4(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void fn_801CB834(u32 id, u32 slot, u32 x, u32 y);
    extern void scriptWaitSyncMotion(u32 id, u32 val);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 fn_800D3088(void);
    extern u32 fn_801CBA0C(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void fn_801845E4(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void fn_800FF58C(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);
    extern void fn_80118874(void *texture, u32 flag);
    extern u32 fn_80113F48(void);

    u32 elapsed;
    u32 waitFrames;
    u32 model;
    u32 shadowTarget;
    u32 anim;

    lbl_8047A690 = GSresGetResource(ctx, 0x0B631602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0B631002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);

    model = fn_801CBA0C(0x0B721000);
    fn_801CB834(model, 6, 0, 1);

    shadowTarget = GSresGetResource(ctx, model);
    GSmodelSetShadowFlags(shadowTarget, 2);
    GSmodelSetShadowLight(shadowTarget, lbl_8047A690);
    GSmodelSetShadowSurface(shadowTarget, 1, &lbl_8047A694);

    cameraPlayAnime(fn_80113F48(), 0x0B651800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    anim = fn_801CBA0C(0x0B731000);
    fn_801845E4(ctx, model, ctx, anim, 0);

    cameraWaitSyncAnime(1);
    fn_800FF58C(0x87);
    floorSetFadeScript(0, 0);
}

/* 0x80092C90 | size: 0x1A8 */
s32 fn_80092C90(s32 channel, void* arg1, void* arg2)
{
    extern u32 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u32 handle);
    extern void fn_8009F77C(void* work);
    extern void fn_8009F9C8(void* callback);
    extern s32 fn_800937F4(void* arg);
    extern void fn_80093B04(u32 a, u32 b);
    extern s32 fn_80089048(void* out, void* arg1, void* arg2);
    extern void OSCreateThread(void* thread, void* entry, void* arg,
                               void* stack, u32 stackSize, s32 priority,
                               u16 attributes);
    extern void OSResumeThread(void* thread);
    extern void* memset(void* dst, int value, u32 size);

    u32 slot;
    u32 handle;
    u8* allocated;
    u8* work;
    s32 started;
    s32 requestStarted;
    u8* requestWork;

    if (channel < 0 || channel > 3) {
        started = 0;
    } else {
        slot = (u32)channel << 2;
        if (*(u8**)(lbl_803FB328 + slot) != NULL) {
            started = 1;
        } else {
            handle = fn_800E2C04(0x44A0, 0x20);
            if ((handle & 0xFFFF) == 0) {
                __assert(lbl_8026F5A8, 0x1DD, &lbl_8047C1E8);
            }
            allocated = fn_800E27B0(handle);
            memset(allocated, 0, 0x4490);
            *(u8**)(lbl_803FB328 + slot) = allocated;

            work = *(u8**)(lbl_803FB328 + slot);
            *(u32*)(work + GBA_STATE_PHASE) = 0;
            *(s32*)(work + GBA_STATE_PORT) = channel;
            fn_800716C8(channel, work + GBA_DATA_OFFSET, fn_80093B04);
            fn_8009F77C(work);
            fn_8009F9C8(work + 0x18);
            OSCreateThread(work + GBA_DATA_OFFSET, fn_800937F4, work,
                           work + GBA_STATE_PORT, 0x4000,
                           GBA_THREAD_PRIORITY, 0);
            OSResumeThread(work + GBA_DATA_OFFSET);
            started = 1;
        }
    }

    if (started == 0) {
        return 0;
    }

    requestWork = *(u8**)(lbl_803FB328 + ((u32)channel << 2));
    requestStarted = 0;
    fn_8009F7B4(requestWork);
    if (*(s32*)(requestWork + GBA_STATE_PHASE) == 0) {
        requestStarted = fn_80089048(requestWork + 0x4344, arg1, arg2);
        if (requestStarted != 0) {
            *(s32*)(requestWork + GBA_STATE_PHASE) = 12;
            *(u32*)(requestWork + GBA_STATE_TIMEOUT) = 0x3000C;
        }
    }
    fn_8009F890(requestWork);
    fn_800A257C(requestWork + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    if (requestStarted != 0) {
        fn_8009FABC(requestWork + 0x18);
    }
    return requestStarted;
}

#endif

#if !defined(GBA_COMMUNICATION_REMAINDER_80092FC8)

/* 0x80092FC8 | size: 0x198 */
#pragma push
#pragma peephole off
s32 fn_80092FC8(s32 channel, void* requestValue, void* requestContext)
{
    extern u32 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u32 handle);
    extern void fn_8009F77C(void* work);
    extern void fn_8009F9C8(void* callback);
    extern s32 fn_800937F4(void* arg);
    extern void fn_80093B04(u32 a, u32 b);
    extern void OSCreateThread(void* thread, void* entry, void* arg,
                               void* stack, u32 stackSize, s32 priority,
                               u16 attributes);
    extern void OSResumeThread(void* thread);
    extern void* memset(void* dst, int value, u32 size);

    u32 slot;
    u32 handle;
    u8* allocated;
    u8* work;
    s32 started;
    s32 requestStarted;
    u8* requestWork;

    if (channel < 0 || channel > 3) {
        started = 0;
    } else {
        slot = (u32)channel << 2;
        if (*(u8**)(lbl_803FB328 + slot) != NULL) {
            started = 1;
        } else {
            handle = fn_800E2C04(0x44A0, 0x20);
            if ((handle & 0xFFFF) == 0) {
                __assert(lbl_8026F5A8, 0x1DD, &lbl_8047C1E8);
            }
            allocated = fn_800E27B0(handle);
            memset(allocated, 0, 0x4490);
            *(u8**)(lbl_803FB328 + slot) = allocated;

            work = *(u8**)(lbl_803FB328 + slot);
            *(u32*)(work + GBA_STATE_PHASE) = 0;
            *(s32*)(work + GBA_STATE_PORT) = channel;
            fn_800716C8(channel, work + GBA_DATA_OFFSET, fn_80093B04);
            fn_8009F77C(work);
            fn_8009F9C8(work + 0x18);
            OSCreateThread(work + GBA_DATA_OFFSET, fn_800937F4, work,
                           work + GBA_STATE_PORT, 0x4000,
                           GBA_THREAD_PRIORITY, 0);
            OSResumeThread(work + GBA_DATA_OFFSET);
            started = 1;
        }
    }

    if (started == 0) {
        return 0;
    }

    requestWork = *(u8**)(lbl_803FB328 + ((u32)channel << 2));
    requestStarted = 0;
    fn_8009F7B4(requestWork);
    if (*(s32*)(requestWork + GBA_STATE_PHASE) == 0) {
        *(s32*)(requestWork + GBA_STATE_PHASE) = 4;
        *(u32*)(requestWork + GBA_STATE_TIMEOUT) = 0x30004;
        requestStarted = 1;
        *(void**)(requestWork + 0x4344) = requestValue;
        *(void**)(requestWork + 0x4348) = requestContext;
    }
    fn_8009F890(requestWork);
    fn_800A257C(requestWork + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    if (requestStarted != 0) {
        fn_8009FABC(requestWork + 0x18);
    }
    return requestStarted;
}
#pragma pop

/* 0x80093160 | size: 0x190 */
#pragma push
#pragma peephole off
s32 fn_80093160(s32 channel, void* requestValue)
{
    extern u32 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u32 handle);
    extern void fn_8009F77C(void* work);
    extern void fn_8009F9C8(void* callback);
    extern s32 fn_800937F4(void* arg);
    extern void fn_80093B04(u32 a, u32 b);
    extern void OSCreateThread(void* thread, void* entry, void* arg,
                               void* stack, u32 stackSize, s32 priority,
                               u16 attributes);
    extern void OSResumeThread(void* thread);
    extern void* memset(void* dst, int value, u32 size);

    u32 slot;
    u32 handle;
    u8* allocated;
    u8* work;
    s32 started;
    s32 requestStarted;
    u8* requestWork;

    if (channel < 0 || channel > 3) {
        started = 0;
    } else {
        slot = (u32)channel << 2;
        if (*(u8**)(lbl_803FB328 + slot) != NULL) {
            started = 1;
        } else {
            handle = fn_800E2C04(0x44A0, 0x20);
            if ((handle & 0xFFFF) == 0) {
                __assert(lbl_8026F5A8, 0x1DD, &lbl_8047C1E8);
            }
            allocated = fn_800E27B0(handle);
            memset(allocated, 0, 0x4490);
            *(u8**)(lbl_803FB328 + slot) = allocated;

            work = *(u8**)(lbl_803FB328 + slot);
            *(u32*)(work + GBA_STATE_PHASE) = 0;
            *(s32*)(work + GBA_STATE_PORT) = channel;
            fn_800716C8(channel, work + GBA_DATA_OFFSET, fn_80093B04);
            fn_8009F77C(work);
            fn_8009F9C8(work + 0x18);
            OSCreateThread(work + GBA_DATA_OFFSET, fn_800937F4, work,
                           work + GBA_STATE_PORT, 0x4000,
                           GBA_THREAD_PRIORITY, 0);
            OSResumeThread(work + GBA_DATA_OFFSET);
            started = 1;
        }
    }

    if (started == 0) {
        return 0;
    }

    requestWork = *(u8**)(lbl_803FB328 + ((u32)channel << 2));
    requestStarted = 0;
    fn_8009F7B4(requestWork);
    if (*(s32*)(requestWork + GBA_STATE_PHASE) == 0) {
        *(s32*)(requestWork + GBA_STATE_PHASE) = 2;
        *(u32*)(requestWork + GBA_STATE_TIMEOUT) = 0x30002;
        requestStarted = 1;
        *(void**)(requestWork + 0x4344) = requestValue;
    }
    fn_8009F890(requestWork);
    fn_800A257C(requestWork + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    if (requestStarted != 0) {
        fn_8009FABC(requestWork + 0x18);
    }
    return requestStarted;
}
#pragma pop

/* 0x800932F0 | size: 0x1F4 */
#pragma push
#pragma peephole off
s32 fn_800932F0(s32 channel, const char* primary, const char* secondary)
{
    extern u32 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u32 handle);
    extern void fn_8009F77C(void* work);
    extern void fn_8009F9C8(void* callback);
    extern s32 fn_800937F4(void* arg);
    extern void fn_80093B04(u32 a, u32 b);
    extern void OSCreateThread(void* thread, void* entry, void* arg,
                               void* stack, u32 stackSize, s32 priority,
                               u16 attributes);
    extern void OSResumeThread(void* thread);
    extern u32 strlen(const char* string);
    extern char* strcpy(char* dst, const char* src);
    extern void* memset(void* dst, int value, u32 size);

    u32 slot;
    u32 handle;
    u32 primaryLength;
    u32 secondaryLength;
    u8* allocated;
    u8* work;
    s32 started;
    s32 commandStarted;
    u8* commandWork;

    if (channel < 0 || channel > 3) {
        started = 0;
    } else {
        slot = (u32)channel << 2;
        if (*(u8**)(lbl_803FB328 + slot) != NULL) {
            started = 1;
        } else {
            handle = fn_800E2C04(0x44A0, 0x20);
            if ((handle & 0xFFFF) == 0) {
                __assert(lbl_8026F5A8, 0x1DD, &lbl_8047C1E8);
            }
            allocated = fn_800E27B0(handle);
            memset(allocated, 0, 0x4490);
            *(u8**)(lbl_803FB328 + slot) = allocated;

            work = *(u8**)(lbl_803FB328 + slot);
            *(u32*)(work + GBA_STATE_PHASE) = 0;
            *(s32*)(work + GBA_STATE_PORT) = channel;
            fn_800716C8(channel, work + GBA_DATA_OFFSET, fn_80093B04);
            fn_8009F77C(work);
            fn_8009F9C8(work + 0x18);
            OSCreateThread(work + GBA_DATA_OFFSET, fn_800937F4, work,
                           work + GBA_STATE_PORT, 0x4000,
                           GBA_THREAD_PRIORITY, 0);
            OSResumeThread(work + GBA_DATA_OFFSET);
            started = 1;
        }
    }

    if (started == 0) {
        return 0;
    }

    commandWork = *(u8**)(lbl_803FB328 + ((u32)channel << 2));
    commandStarted = 0;
    primaryLength = strlen(primary);
    if (secondary != NULL) {
        secondaryLength = strlen(secondary);
    } else {
        secondaryLength = 0;
    }

    if (primaryLength >= 0x7F || secondaryLength >= 0x7F) {
        commandStarted = 0;
        goto done;
    }

    fn_8009F7B4(commandWork);
    if (*(s32*)(commandWork + GBA_STATE_PHASE) == 0) {
        commandStarted++;
        *(s32*)(commandWork + GBA_STATE_PHASE) = commandStarted;
        *(u32*)(commandWork + GBA_STATE_TIMEOUT) = 0x30001;
        strcpy((char*)(commandWork + 0x4344), primary);
        if (secondary != NULL) {
            strcpy((char*)(commandWork + 0x43C4), secondary);
        } else {
            commandWork[0x43C4] = 0;
        }
    }
    fn_8009F890(commandWork);
    fn_800A257C(commandWork + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    if (commandStarted != 0) {
        fn_8009FABC(commandWork + 0x18);
    }

done:
    return commandStarted;
}
#pragma pop

/* 0x800934E4 | size: 0x90 */
s32 fn_800934E4(s32 channel)
{
#pragma peephole off
    s32 idle;
    u8* work;
    u32 slot;

    if (channel < 0 || channel > 3) {
        return 0;
    }

    slot = (u32)channel << 2;
    work = *(u8**)((u8*)lbl_803FB328 + slot);
    if (work != NULL) {
        fn_8009F7B4(work);
        idle = (*(u32*)(work + GBA_STATE_PHASE) == 0);
        fn_8009F890(work);
        fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    } else {
        idle = 1;
    }

    return idle;
}

/* 0x80093574 | size: 0x9C */
u32 fn_80093574(s32 channel)
{
#pragma peephole off
    u32 status;
    u8* work;
    u32 slot;

    if (channel < 0 || channel > 3) {
        return 0x10000;
    }

    slot = (u32)channel << 2;
    work = *(u8**)((u8*)lbl_803FB328 + slot);
    if (work == NULL) {
        return 0;
    }

    while (1) {
        fn_8009F7B4(work);
        status = *(u32*)(work + GBA_STATE_TIMEOUT);
        fn_8009F890(work);
        fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
        if ((s32)(status >> 16) == 3) {
            _threadSwitch();
        } else {
            return status;
        }
    }
}

/* 0x80093610 | size: 0x88 */
u32 fn_80093610(s32 channel)
{
#pragma peephole off
    u32 status;
    u8* work;
    u32 slot;

    if (channel < 0 || channel > 3) {
        return 0x10000;
    }

    slot = (u32)channel << 2;
    work = *(u8**)((u8*)lbl_803FB328 + slot);
    if (work == NULL) {
        return 0;
    }

    fn_8009F7B4(work);
    status = *(u32*)(work + GBA_STATE_TIMEOUT);
    fn_8009F890(work);
    fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);

    return status;
}

/* 0x80093698 | size: 0x15C */
s32 fn_80093698(s32 channel)
{
#pragma peephole off
    u32 slot;
    u32 status;
    u8* work;

    if (channel < 0 || channel > 3) {
        return 0;
    }

    slot = (u32)channel << 2;
    work = *(u8**)((u8*)lbl_803FB328 + slot);
    if (work == NULL) {
        return 1;
    }

    fn_800716E8(*(s32*)(work + GBA_STATE_PORT), 1);
    while (1) {
        fn_8009F7B4(work);
        status = *(u32*)(work + GBA_STATE_TIMEOUT);
        fn_8009F890(work);
        fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
        if ((s32)(status >> 16) == 3) {
            _threadSwitch();
        } else {
            break;
        }
    }

    fn_8009F7B4(work);
    *(u32*)(work + GBA_STATE_PHASE) = 0xD;
    *(u32*)(work + GBA_STATE_TIMEOUT) = 0x3000D;
    fn_8009F890(work);
    fn_800A257C(work + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    fn_8009FABC(work + 0x18);
    fn_800A1E54(work + GBA_DATA_OFFSET, 0);
    fn_800716C8(*(s32*)(work + GBA_STATE_PORT), NULL, NULL);
    fn_800716E8(*(s32*)(work + GBA_STATE_PORT), 0);

    status = fn_800E202C(*(u8**)((u8*)lbl_803FB328 + slot));
    if ((status & 0xFFFF) == 0) {
        __assert(lbl_8026F5A8, 0x1E6, &lbl_8047C1E8);
    }
    fn_800E24B0(status);
    fn_800E209C(status);
    *(u8**)((u8*)lbl_803FB328 + slot) = NULL;

    return 1;
}

#endif

#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)

/* 0x800937F4 | size: 0x310 */
#pragma push
#pragma peephole off
s32 fn_800937F4(void* arg0)
{
    extern void fn_8009F9E8();
    extern s32 fn_80073E8C();
    extern s32 fn_80073E84();
    extern s32 fn_80074324();
    extern s32 fn_800745B4();
    extern s32 fn_80073690();
    extern void fn_800895A4();
    extern s32 fn_80071E34();
    extern void fn_80089380();
    extern s32 fn_80089D30();
    extern s32 fn_80089CA8();
    extern s32 fn_80089C84();
    extern u64 OSGetTime(void);
    extern f32 lbl_8047C1F0;

    u8* p;
    s32 result;
    s32 status;
    s32 state;
    void* arg;
    u64 start;
    u64 now;
    f32 rate;
    u8 readBuffer[0xD8];
    u8 statusBuffer[0x278];

    p = arg0;
    result = 0;
    for (;;) {
        fn_8009F7B4(p);
        if (*(s32*)(p + GBA_STATE_PHASE) != 0xD) {
            *(s32*)(p + GBA_STATE_TIMEOUT) = result;
            *(s32*)(p + GBA_STATE_PHASE) = 0;
            while (*(s32*)(p + GBA_STATE_PHASE) == 0) {
                fn_8009F9E8(p + 0x18, p);
            }
        }

        state = *(s32*)(p + GBA_STATE_PHASE);
        fn_8009F890(p);
        status = 0;

        switch (state) {
        case 0:
        case 3:
            break;
        case 1:
            if ((s8)p[0x43C4] != 0) {
                arg = p + 0x43C4;
            } else {
                arg = NULL;
            }
            status = fn_80073E8C(p + 0x4344, arg);
            if (status == 0) {
                while (fn_80073E84() == 0) {
                    fn_800A257C((void*)fn_800A13F8(), 0x10);
                    OSYieldThread();
                }
                result = 1;
            }
            break;
        case 2:
            start = OSGetTime();
            if (*(s32*)(p + 0x4344) == 0) {
                rate = lbl_8047C1F0;
            } else {
                rate = lbl_8047C1F0;
            }
            while ((status = fn_80074324(*(s32*)(p + GBA_STATE_PORT))) != 0) {
                if (status == 0x3E8) {
                    goto case2Done;
                }
                now = OSGetTime() - start;
                if ((s32)(u32)(u64)(
                        rate * (f32)(*(u32*)0x800000F8 >> 2))
                    <= (s32)(u32)now) {
                    result = 0x20002;
                    goto case2Done;
                }
                fn_800A257C((void*)fn_800A13F8(), 0x10);
                OSYieldThread();
            }
            status = fn_800745B4(*(s32*)(p + GBA_STATE_PORT),
                                 *(s32*)(p + 0x4344));
            if (status == 0) {
                result = 2;
            }
        case2Done:
            break;
        case 4:
            status = fn_80073690(*(s32*)(p + GBA_STATE_PORT), statusBuffer);
            if (status == 0) {
                fn_800895A4(*(s32*)(p + 0x4344), statusBuffer);
                result = 4;
                **(u32**)(p + 0x4348) =
                    (statusBuffer[3] << 24) | (statusBuffer[2] << 16)
                    | (statusBuffer[1] << 8) | statusBuffer[0];
            }
            break;
        case 5:
            result = 5;
            break;
        case 6:
            result = 6;
            break;
        case 7:
            result = 7;
            break;
        case 8:
            result = 8;
            break;
        case 9:
            result = 9;
            break;
        case 10:
            result = 10;
            break;
        case 11:
            status = fn_80071E34(*(s32*)(p + GBA_STATE_PORT), readBuffer);
            if (status == 0) {
                fn_80089380(*(s32*)(p + 0x4344), readBuffer);
                result = 11;
            }
            break;
        case 12:
            status = fn_80089D30(*(s32*)(p + GBA_STATE_PORT) + 1,
                                  p + 0x4344);
            if (status == 0) {
                for (;;) {
                    status = fn_80089CA8(*(s32*)(p + GBA_STATE_PORT) + 1);
                    if (status == 0) {
                        status = fn_80089C84(*(s32*)(p + GBA_STATE_PORT) + 1);
                    }
                    if (status >= 0) {
                        break;
                    }
                    fn_800A257C((void*)fn_800A13F8(), 0x10);
                    OSYieldThread();
                }
                if (status == 0) {
                    result = 12;
                }
            }
            break;
        case 13:
            return 0;
        }

        if (status != 0) {
            result = (state & 0xFFFF) | 0x10000;
        }
    }
}
#pragma pop

/* 0x80093B04 | size: 0x48 */
void fn_80093B04(u32 a, u32 b) {
    u32 r31;
    u32 result;
    r31 = b;
    result = fn_800A13F8();
    if (r31 != 0) {
        if (r31 != result) return;
    }
    fn_800A257C((void*)result, 0x10);
    OSYieldThread();
    return;
}

/* 0x80093F2C | size: 0x38 */
#pragma push
#pragma scheduling off
void menuPokemonStatusCtrlRibbon(void) {
    extern void fn_80093F64();
    u8 *r4 = (u8*)&lbl_803FB380;
    u32 r3 = *(u32*)(r4 + 0xC);

    if (r3 != 0) {
        fn_80093F64(r3, r4 + 0x1c);
    }
    return;
}
#pragma pop

typedef struct RibbonGroupDescriptor {
    u16 selector;
    s8 firstRibbon;
    u8 maximum;
} RibbonGroupDescriptor;

typedef struct PokemonRibbonGrid {
    u32 count;
    s8 ribbon[9][4];
} PokemonRibbonGrid;

typedef struct PokemonStatusMenuWork {
    u8 flags;
    u8 state;
    s8 selection;
    s8 previousSelection;
    s32 result;
    u32 entityId;
    void* pokemon;
    void* callback;
    s32 callbackArg;
    u16 hasExtraMove;
    u16 padding;
    PokemonRibbonGrid ribbons;
} PokemonStatusMenuWork;

extern RibbonGroupDescriptor lbl_802EEFD8[10];
extern RibbonGroupDescriptor lbl_802EF000[7];
extern u32 pokemonGetStatus();

/* Build the four-column ribbon grid used by the Pokemon status window. */
void fn_80093F64(u8* pokemon, PokemonRibbonGrid* grid)
{
    RibbonGroupDescriptor* group;
    s32 available;
    s32 ribbon;
    u32 groupIndex;
    u32 outputIndex;
    u32 row;
    u32 column;
    u32 count;

    for (column = 0; column < 4; column++) {
        for (row = 0; row < 9; row++) {
            grid->ribbon[row][column] = -1;
        }
    }

    outputIndex = 0;
    group = lbl_802EEFD8;
    for (groupIndex = 0; groupIndex < 10; groupIndex++, group++) {
        available = pokemonGetStatus(pokemon, 0, group->selector, 0);
        if (available > group->maximum) {
            available = group->maximum;
        }
        for (ribbon = 0; ribbon < available; ribbon++, outputIndex++) {
            grid->ribbon[outputIndex % 9][outputIndex / 9] =
                group->firstRibbon + ribbon;
        }
    }

    outputIndex = 0;
    group = lbl_802EF000;
    for (groupIndex = 0; groupIndex < 7; groupIndex++, group++) {
        available = pokemonGetStatus(pokemon, 0, group->selector, 0);
        if (available > group->maximum) {
            available = group->maximum;
        }
        for (ribbon = 0; ribbon < available; ribbon++, outputIndex++) {
            grid->ribbon[outputIndex][3] = group->firstRibbon + ribbon;
        }
    }

    count = 0;
    for (column = 0; column < 4; column++) {
        for (row = 0; row < 9; row++) {
            if (grid->ribbon[row][column] >= 0) {
                count++;
            }
        }
    }
    grid->count = count;
}

/* 0x800965C8 | size: 0x680 */
void fn_800965C8(void* window, u8* sprite) {
    register s32 color;
    register void* pokemon;
    void* bios;
    s16 id;
    s32 state;
    s32 value;
    u32 msg;

    pokemon = *(void**)(lbl_803FB380 + 0x0C);
    if (pokemon == NULL) {
        return;
    }

    bios = pokemonDataBiosGetPtr((u16)pokemonGetStatus(pokemon, 0, 0x6E, 0));
    if (bios == NULL) {
        return;
    }

    id = *(s16*)(sprite + 0x06);
    color = (s32)((u8*)window)[0x8B] | -0x100;
    state = (s8)((u8*)window)[0x95];

    switch (id) {
    case 0xE7: {
        void* texture = menuModelRender(lbl_803FB338);
        if (texture == NULL) {
            return;
        }
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D6A00(7);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, texture);
        fn_800D67BC(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047C230, lbl_8047C230);
        fn_800D61E4(*(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56));
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047C208, lbl_8047C208);
        fn_800D6728();
        break;
    }
    case 0x107:
    case 0x108:
    case 0x109:
    case 0x10A: {
        u32 mask = 0;
        value = (u8)pokemonGetStatus(pokemon, 0, 0xBB, 0);
        switch (id) {
        case 0x107:
            mask = 8;
            break;
        case 0x108:
            mask = 4;
            break;
        case 0x109:
            mask = 2;
            break;
        case 0x10A:
            mask = 1;
            break;
        }
        winSpriteSetDisp(sprite, value & mask);
        break;
    }
    case 0x10B:
        value = pokemonBiosGetCatchBallId(pokemon);
        if (value < 13) {
            windowDrawSprite(0, 0, window, lbl_802EED28[value], 0);
        }
        break;
    case 0x10C:
        value = (u8)pokemonGetStatus(pokemon, 0, 0xB5, 0);
        if ((value & 0xF) != 0) {
            msg = 0xE8;
        } else if (value != 0) {
            msg = 0xE7;
        } else {
            msg = 0;
        }
        windowDrawSprite(0, 0, window, (u16)msg, 0);
        break;
    case 0x10D:
        windowDrawSprite(0, 0, window, (u16)fn_8001D624(pokemon, 1), 0);
        break;
    case 0x10E:
        winSpriteSetDisp(sprite, pokemonGetSoubiItemDataId(pokemon) != 0);
        break;
    case 0x551:
        value = pokemonGetSoubiItemDataId(pokemon);
        if (value != 0) {
            msgctrlSetValue(0x2D, (u16)value);
            fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0x2BD3);
        }
        break;
    case 0x552: {
        s32 x = (s16)(GSmsgGetRect(*(u32*)(sprite + 0x4C)) >> 16);
        msgctrlSetValue(0x34, (u8)pokemonGetStatus(pokemon, 0, 0x7A, 0));
        fn_800FBB34(x, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xD2);
        break;
    }
    case 0x554: {
        u32 name = pokemonDataBiosGetName(bios);
        s32 x = (s16)(GSmsgGetRect(0x2BD4) >> 16);
        fn_800FB680(0, 0, color, 0x2BD4);
        if (name != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(name));
            fn_800FB680(x, 0, color, 0xE7);
        }
        break;
    }
    case 0x555: {
        s32 x;
        u32 sexMsg;

        msgctrlSetValue(0x37, pokemonGetStatus(pokemon, 0, 0x77, 0));
        fn_800FBB34(0, 0, *(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56), color, 0xE7);
        x = (s16)(GSmsgGetRect(0xE7) >> 16);
        switch ((u8)menuSubGetPokemonSexForDisp(pokemon)) {
        case 0:
            sexMsg = 0xD67;
            break;
        case 1:
            sexMsg = 0xD68;
            break;
        default:
            sexMsg = 0;
            break;
        }
        if (sexMsg != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(sexMsg));
            fn_800FB680(x, 0, color, 0xCF);
        }
        break;
    }
    case 0xF3:
    case 0x110:
        winSpriteSetDisp(sprite, state == 0);
        break;
    case 0xF5:
    case 0x112:
        winSpriteSetDisp(sprite, state == 1);
        break;
    case 0xF4:
    case 0x111:
        winSpriteSetDisp(sprite, state == 2);
        break;
    case 0x106: {
        u32 disp = 0;
        switch (lbl_803FB380[1]) {
        case 2:
        case 4:
            disp = 1;
            break;
        case 3:
            if (lbl_803FB380[0] & 2) {
                disp = 1;
            }
            break;
        case 5:
            if (*(s32*)(lbl_803FB380 + 0x1C) > 0) {
                disp = 1;
            }
            break;
        }
        winSpriteSetDisp(sprite, disp);
        break;
    }
    case 0x598:
        msg = 0;
        switch (lbl_803FB380[1]) {
        case 2:
            msg = 0x2BCF;
            break;
        case 3:
            if (lbl_803FB380[0] & 2) {
                msg = 0x2BD0;
            }
            break;
        case 4:
            msg = 0x2BD0;
            break;
        case 5:
            if (*(s32*)(lbl_803FB380 + 0x1C) > 0) {
                msg = 0x2BD2;
            }
            break;
        }
        if (msg != 0) {
            fn_800FB680(0, 0, color, msg);
        }
        break;
    default:
        break;
    }
}


/* 0x80096C48 | size: 0x10C */
#pragma peephole off
void fn_80096C48(u32 unused, u8* dst) {
    typedef struct {
        f32 x;
        f32 y;
        f32 z;
    } ColorTriple;

    ColorTriple state0;
    ColorTriple state1;
    ColorTriple state2;
    register u8* out;
    register ColorTriple* triple;
    u8* obj;
    s32 state;

    out = dst;
    state0 = *(ColorTriple*)(lbl_8026F5C0 + 0x00);
    state1 = *(ColorTriple*)(lbl_8026F5C0 + 0x0C);
    state2 = *(ColorTriple*)(lbl_8026F5C0 + 0x18);

    obj = windowSearchID(0x53);
    if (obj == NULL) {
        return;
    }

    state = (s8)obj[0x95];
    switch (state) {
    case 0:
        triple = &state0;
        break;
    case 1:
        triple = &state1;
        break;
    case 2:
        triple = &state2;
        break;
    }

    out[0x64] = triple->x;
    out[0x65] = triple->y;
    out[0x66] = triple->z;
}
#pragma peephole on

/* Handle confirm/cancel input for the linked-Pokemon status menu. */
#pragma push
#pragma peephole off
void fn_80096D54(u8* menu)
{
    extern void fn_80166A28(s32 id);
    extern void pokemonWazaReplace();

    u16* keys;
    void* pokemon;
    PokemonStatusMenuWork* work;
    register s8* ribbonRow;
    register s32 ribbonColumn;
    register s32 ribbonIndex;
    s8 ribbon;

    keys = windowGetKeyInfo();
    if (keys[2] & 0x10) {
        work = (PokemonStatusMenuWork*)lbl_803FB380;
        switch (work->state) {
        case 0:
        case 1:
        case 6:
        case 8:
            break;
        case 2:
            work->selection = 0;
            work->state = 3;
            break;
        case 3:
            pokemon = work->pokemon;
            if (pokemon != NULL) {
                if ((s32)pokemonGetStatus(pokemon, 0, 0xC2, 0) != 0) {
                    fn_80166A28(0x26);
                } else if (lbl_803FB380[0] & 2) {
                    work->state = 4;
                    lbl_803FB380[3] = lbl_803FB380[2];
                }
            }
            break;
        case 4:
            pokemon = work->pokemon;
            if (pokemon != NULL) {
                pokemonWazaReplace(pokemon, work->previousSelection,
                                   work->selection);
            }
            *(s8*)(lbl_803FB380 + 3) = -1;
            work->state = 3;
            break;
        case 5:
            if ((s32)work->ribbons.count > 0) {
                for (ribbon = 0; ribbon < 36; ribbon++) {
                    ribbonIndex = ribbon % 9;
                    ribbonColumn = ribbon % 4;
                    ribbonRow = (s8*)work + ribbonIndex * 4;
                    ribbonRow += ribbonColumn;
                    if (ribbonRow[0x20] >= 0) {
                        break;
                    }
                }
                work->state = 6;
                *(s8*)(lbl_803FB380 + 0x1A) = ribbon;
            }
            break;
        case 7:
            if (work->flags & 0x10) {
                menu[0x98] = 1;
                work->result = work->selection;
            }
            break;
        }
    } else if (keys[2] & 0x20) {
        switch (lbl_803FB380[1]) {
        case 0:
        case 8:
            break;
        case 1:
        case 2:
        case 5:
        case 7:
            menu[0x98] = 1;
            menu[0x99] = 1;
            lbl_803FB380[1] = 8;
            break;
        case 3:
            lbl_803FB380[1] = 2;
            break;
        case 4:
            lbl_803FB380[1] = 3;
            break;
        case 6:
            *(s8*)(lbl_803FB380 + 0x1A) = -1;
            lbl_803FB380[1] = 5;
            break;
        }
    }
}
#pragma pop

/* Handle input for the linked-Pokemon status submenus. */
#pragma push
#pragma peephole off
void fn_80096FA0(u8* menu)
{
    typedef u32 (*StatusChangeCallback)(u32, s32, s32);
    extern void* fn_80105624(void);
    extern void fn_80103484(s32, s32);
    extern void fn_80109C88(void*, u32);
    extern u8 fn_80123CD4(u32, s32);
    extern u16 fn_8012640C(u32, u32, u32, s32);

    void* keyObject;
    u16 input;
    s32 action;
    s32 limit;
    s32 selection;
    u32 pokemon;
    u16 valid;
    u32 result;
    s32 row;
    s32 column;
    s32 scan;
    s32 inner;
    s32 previous;
    u8* cell;

    action = 0;
    if (*(u16*)(lbl_803FB380 + 0x18) != 0) {
        limit = 5;
    } else {
        limit = 4;
    }
    keyObject = fn_80105624();
    input = *(u16*)((u8*)keyObject + 6);

    if (lbl_803FB380[1] <= 8) {
        switch (lbl_803FB380[1]) {
        case 1:
        case 2:
        case 5:
            selection = menu[0x95];
            if (input & 8) {
                selection++;
            } else if (input & 4) {
                selection--;
            }
            if ((s8)selection > 2) {
                selection = 2;
            }
            if ((s8)selection < 0) {
                selection = 0;
            }
            menu[0x95] = selection;
            switch ((s8)selection) {
            case 0:
                lbl_803FB380[1] = 1;
                break;
            case 1:
                lbl_803FB380[1] = 2;
                break;
            case 2:
                lbl_803FB380[1] = 5;
                break;
            }
            if (input & 1) {
                action = 1;
            } else if (input & 2) {
                action = 2;
            }
            if (action != 0 && *(u32*)(lbl_803FB380 + 0x10) != 0) {
                result = (*(StatusChangeCallback*)(lbl_803FB380 + 0x10))(
                    *(u32*)(lbl_803FB380 + 0x0C), action,
                    *(s32*)(lbl_803FB380 + 0x14));
                if (*(u32*)(lbl_803FB380 + 0x0C) != result) {
                    if (result != 0) {
                        fn_80103484(*(s32*)(menu + 4), 1);
                        fn_80109C88(lbl_803FB338, result);
                    }
                    *(u32*)(lbl_803FB380 + 0x0C) = result;
                    return;
                }
            }
            return;

        case 3:
        case 4:
        case 7:
            selection = lbl_803FB380[2];
            if (input & 1) {
                selection--;
            } else if (input & 2) {
                selection++;
            }
            if ((s8)selection >= limit) {
                selection = (s8)(limit - 1);
            }
            if ((s8)selection < 0) {
                selection = 0;
            }
            pokemon = *(u32*)(lbl_803FB380 + 0x0C);
            if ((u16)(s8)selection == 4) {
                valid = *(u16*)(lbl_803FB380 + 0x18);
            } else {
                valid = fn_8012640C(pokemon, 0, 0x7F, (s8)selection);
                if (fn_80123CD4(pokemon, (s8)selection) == 0) {
                    valid = 0;
                }
            }
            if (valid != 0 && (s8)selection != (s8)lbl_803FB380[2]) {
                fn_80103484(*(s32*)(menu + 4), 1);
                lbl_803FB380[2] = selection;
                return;
            }
            break;

        case 6:
            row = (s8)lbl_803FB380[0x1A] / 9;
            column = (s8)lbl_803FB380[0x1A] % 9;
            if (input & 1) {
                scan = row;
            scan_up:
                previous = scan > 0;
                scan--;
                if (previous) {
                    inner = column;
                    cell = lbl_803FB380 + scan + column * 4;
                scan_up_row:
                    if ((s8)cell[0x20] >= 0) {
                        row = scan;
                        column = inner;
                        scan = -1;
                    } else {
                        previous = inner > 0;
                        cell -= 4;
                        inner--;
                        if (previous) {
                            goto scan_up_row;
                        }
                    }
                    goto scan_up;
                }
            } else if (input & 2) {
                scan = row;
            scan_down:
                scan++;
                if (scan < 4) {
                    inner = column;
                    cell = lbl_803FB380 + scan + column * 4;
                scan_down_row:
                    if ((s8)cell[0x20] >= 0) {
                        row = scan;
                        column = inner;
                        scan = 5;
                    } else {
                        previous = inner > 0;
                        cell -= 4;
                        inner--;
                        if (previous) {
                            goto scan_down_row;
                        }
                    }
                    goto scan_down;
                }
            } else if (input & 8) {
                previous = column;
                column++;
                if (column >= 9) {
                    column = 8;
                }
                if ((s8)lbl_803FB380[0x20 + column * 4 + row] < 0) {
                    column = previous;
                }
            } else if (input & 4) {
                previous = column;
                column--;
                if (column < 0) {
                    column = 0;
                }
                if ((s8)lbl_803FB380[0x20 + column * 4 + row] < 0) {
                    column = previous;
                }
            }
            selection = column + row * 9;
            if ((s8)selection != (s8)lbl_803FB380[0x1A]) {
                fn_80103484(*(s32*)(menu + 4), 1);
                lbl_803FB380[0x1A] = selection;
            }
            break;

        case 0:
        case 8:
            break;
        }
    }
}
#pragma pop


/* 0x80097BBC | size: 0x114 */
#pragma peephole off
s32 fn_80097BBC(u8 chan) {
    extern void* savedataGetStatus();
    extern void* heroBiosGetPokemonPtr();
    extern int pokemonCheckValid();
    extern int fn_8010B560();
    void* entity;
    void* mgr;

    entity = NULL;
    if (chan < 6) {
        mgr = savedataGetStatus(0, 2);
        if (mgr != 0) {
            entity = heroBiosGetPokemonPtr(mgr, chan);
            if ((u8)pokemonCheckValid() == 0) {
                entity = NULL;
            }
        }
    }
    if (entity == 0) {
        return -1;
    }
    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x11;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = (u32)entity;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = 0;
    *(u32*)(lbl_803FB380 + 0x14) = 0;
    *(u32*)(lbl_803FB380 + 0x4) = -1;
    fn_800FF730(0x39d);
    if (lbl_803FB380[0] & 8) {
        floorSetFadeScript(0, 0);
    }
    _threadSwitch();
    return *(s32*)(lbl_803FB380 + 0x4);
}
#pragma peephole on
#pragma peephole reset

/* 0x8009769C | size: 0x350 */
u32 fn_8009769C(u8 flags, u32 arg1, s32 pokemon, u16 arg3, u32 arg4, u32 arg5) {
    extern void menuModelInit(void* model, s32 width, s32 height);
    extern void fn_80109C88(void* model, u32 pokemon);
    extern void fadeCheck(s32 wait);
    extern void fadeSet(s32 mode, f32 value);
    extern s32 menuOpenCustom(s32 menu, ...);
    extern u32 pokemonGetStatus(u32 pokemon, u32 index, u32 status, s32 slot);
    extern u8 pokemonWazaCheckValid(u32 pokemon, s32 slot);
    extern s32 wazaGetStatus(u32 data, u16 index, u32 status, u32 arg3);
    extern void winMsgOpen(s32 slot, s32 message, s32 arg2, s32 arg3);
    extern void winMsgClose(s32 slot);
    extern s32 menuIsCheck(s32 menu);
    extern void menuCloseCustom(s32 menu, s32 mode, s32 wait);
    extern void fn_800FF660(void);
    extern void fn_8010A420(void* model);

    u32 cursor;
    s32 result;
    u16 move;
    u32 selectedPokemon;
    u8 currentFlags;
    u8* state;

    menuModelInit(lbl_803FB338, 0xC8, 0xB4);
    fn_80109C88(lbl_803FB338, pokemon);

    if (lbl_803FB380[0] & 8) {
        fadeCheck(1);
        if (lbl_803FB380[0] & 0x80) {
            fadeSet(2, lbl_8047C234);
        } else {
            fadeSet(2, lbl_8047C238);
        }
    }

    cursor = 0;
    state = lbl_803FB380;
    for (;;) {
        result = menuOpenCustom(0x53, 0, &cursor, 0, 1, 0);
        if (result == -1) {
            *(s32*)(state + 4) = result;
            break;
        }

        pokemon = *(s8*)(state + 2);
        *(u32*)(state + 4) = *(s8*)(state + 2);
        if (!(state[0] & 0x40)) {
            break;
        }

        selectedPokemon = *(u32*)(state + 0xC);
        if ((u16)pokemon == 4) {
            move = *(u16*)(state + 0x18);
        } else {
            move = (u16)pokemonGetStatus(selectedPokemon, 0, 0x7F, pokemon);
            if ((u8)pokemonWazaCheckValid(selectedPokemon, pokemon) == 0) {
                move = 0;
            }
        }

        if (wazaGetStatus(0, move, 0x19, 0) == 0) {
            break;
        }
        winMsgOpen(2, 0x2BE9, 1, 0);
        winMsgClose(1);
    }

    if (lbl_803FB380[0] & 8) {
        fadeCheck(1);
        if (lbl_803FB380[0] & 0x80) {
            fadeSet(3, lbl_8047C234);
        } else {
            fadeSet(3, lbl_8047C238);
        }
        fadeCheck(1);
    }

    currentFlags = lbl_803FB380[0];
    if ((currentFlags & 1) && !(currentFlags & 8)) {
        fadeCheck(1);
        fadeSet(3, lbl_8047C238);
        fadeCheck(1);
    }

    if ((u8)menuIsCheck(0x54) != 0) {
        menuCloseCustom(0x54, 0, 0);
    }
    if ((u8)menuIsCheck(0x55) != 0) {
        menuCloseCustom(0x55, 0, 0);
    }
    if ((u8)menuIsCheck(0x57) != 0) {
        menuCloseCustom(0x57, 0, 0);
    }
    if ((u8)menuIsCheck(0x56) != 0) {
        menuCloseCustom(0x56, 0, 0);
    }
    if ((u8)menuIsCheck(0x58) != 0) {
        menuCloseCustom(0x58, 0, 0);
    }
    menuCloseCustom(0x53, 0, 1);

    if (lbl_803FB380[0] & 1) {
        fn_800FF660();
        if (lbl_803FB380[0] & 8) {
            floorSetFadeScript(0, 0);
        }
    }

    fn_8010A420(lbl_803FB338);
    _threadSwitch();
    return *(u32*)(state + 4);
}

/* 0x800979EC | size: 0x4C */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void menuPokemonStatus(void) {
    *(u32*)(lbl_803FB380 + 4) = fn_8009769C(
        lbl_803FB380[0],
        *(u32*)(lbl_803FB380 + 8),
        *(u32*)(lbl_803FB380 + 0xC),
        *(u16*)(lbl_803FB380 + 0x18),
        *(u32*)(lbl_803FB380 + 0x10),
        *(u32*)(lbl_803FB380 + 0x14));
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/* 0x80097FCC | size: 0x4 */
void fn_80097FCC(void) {
}

/* 0x80097FD0 | size: 0x28 */
void fn_80097FD0(void) {
    extern int fn_80113F48();
    GSresGetResource(fn_80113F48(), 0x12670000);
}

/* 0x80097FF8 | size: 0x4 */
void fn_80097FF8(void) {
}

/* 0x80097A38 | size: 0xCC */
s32 fn_80097A38(u32 arg0, u16 arg1) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x59;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = arg1;
    *(u32*)(lbl_803FB380 + 0x10) = 0;
    *(u32*)(lbl_803FB380 + 0x14) = 0;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_800FF730(0x39d);
    if (lbl_803FB380[0] & 8) {
        floorSetFadeScript(0, 0);
    }
    _threadSwitch();
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097B04 | size: 0xB8 */
s32 fn_80097B04(u32 arg0, u16 arg1) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x58;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = arg1;
    *(u32*)(lbl_803FB380 + 0x10) = 0;
    *(u32*)(lbl_803FB380 + 0x14) = 0;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                arg1, *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097CD0 | size: 0xC4 */
s32 fn_80097CD0(u32 arg0, u32 arg1, u32 arg2) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0xc;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg1;
    *(u32*)(lbl_803FB380 + 0x14) = arg2;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097D94 | size: 0xC4 */
s32 fn_80097D94(u32 arg0, u32 arg1, u32 arg2) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0xe;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg1;
    *(u32*)(lbl_803FB380 + 0x14) = arg2;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097E58 | size: 0xB0 */
s32 fn_80097E58(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0xac;
    *(u32*)(lbl_803FB380 + 0x8) = arg0;
    *(u32*)(lbl_803FB380 + 0xC) = arg1;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg2;
    *(u32*)(lbl_803FB380 + 0x14) = arg3;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097F08 | size: 0xC4 */
s32 fn_80097F08(u32 arg0, u32 arg1, u32 arg2) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x8e;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg1;
    *(u32*)(lbl_803FB380 + 0x14) = arg2;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

asm u32 PPCMfmsr(void) {
    nofralloc
    mfmsr r3
    blr
}

asm void PPCMtmsr(register u32 val) {
    nofralloc
    mtmsr r3
    blr
}

asm u32 PPCMfhid0(void) {
    nofralloc
    mfspr r3, HID0
    blr
}

asm void PPCMthid0(register u32 val) {
    nofralloc
    mtspr HID0, r3
    blr
}

asm u32 PPCMfl2cr(void) {
    nofralloc
    mfspr r3, L2CR
    blr
}

asm void PPCMtl2cr(register u32 val) {
    nofralloc
    mtspr L2CR, r3
    blr
}

asm void PPCMtdec(register u32 val) {
    nofralloc
    mtdec r3
    blr
}

asm void PPCSync(void) {
    nofralloc
    sc
    blr
}

asm void PPCHalt(void) {
    nofralloc
    sync
_ppc_halt_loop:
    nop
    li r3, 0
    nop
    b _ppc_halt_loop
}

asm void PPCMtmmcr0(register u32 val) {
    nofralloc
    mtspr MMCR0, r3
    blr
}

asm void PPCMtmmcr1(register u32 val) {
    nofralloc
    mtspr MMCR1, r3
    blr
}

asm void PPCMtpmc1(register u32 val) {
    nofralloc
    mtspr PMC1, r3
    blr
}

asm void PPCMtpmc2(register u32 val) {
    nofralloc
    mtspr PMC2, r3
    blr
}

asm void PPCMtpmc3(register u32 val) {
    nofralloc
    mtspr PMC3, r3
    blr
}

asm void PPCMtpmc4(register u32 val) {
    nofralloc
    mtspr PMC4, r3
    blr
}

u32 PPCMffpscr(void) {
    union {
        f64 value;
        u32 words[2];
    } fpscr;

    fpscr.value = __mffs();
    return fpscr.words[1];
}

void PPCMtfpscr(u32 val) {
    volatile union {
        f64 value;
        struct {
            u32 hi;
            u32 lo;
        } words;
    } fpscr;

    fpscr.words.hi = 0;
    fpscr.words.lo = val;
    __setflm(fpscr.value);
}


asm u32 PPCMfhid2(void) {
    nofralloc
    mfspr r3, 920
    blr
}

asm void PPCMthid2(register u32 val) {
    nofralloc
    mtspr 920, r3
    blr
}

asm void PPCMtwpar(register u32 val) {
    nofralloc
    mtspr WPAR, r3
    blr
}

#endif

#if !defined(GBA_COMMUNICATION_EXACT_80092FC8_ONLY)

/* 0x80092E38 | size: 0x190 */
s32 fn_80092E38(s32 channel, void* requestValue)
{
    extern u32 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u32 handle);
    extern void fn_8009F77C(void* work);
    extern void fn_8009F9C8(void* callback);
    extern s32 fn_800937F4(void* arg);
    extern void fn_80093B04(u32 a, u32 b);
    extern void OSCreateThread(void* thread, void* entry, void* arg,
                               void* stack, u32 stackSize, s32 priority,
                               u16 attributes);
    extern void OSResumeThread(void* thread);
    extern void* memset(void* dst, int value, u32 size);

    u32 slot;
    u32 handle;
    u8* allocated;
    u8* work;
    s32 started;
    s32 requestStarted;
    u8* requestWork;

    if (channel < 0 || channel > 3) {
        started = 0;
    } else {
        slot = (u32)channel << 2;
        if (*(u8**)(lbl_803FB328 + slot) != NULL) {
            started = 1;
        } else {
            handle = fn_800E2C04(0x44A0, 0x20);
            if ((handle & 0xFFFF) == 0) {
                __assert(lbl_8026F5A8, 0x1DD, &lbl_8047C1E8);
            }
            allocated = fn_800E27B0(handle);
            memset(allocated, 0, 0x4490);
            *(u8**)(lbl_803FB328 + slot) = allocated;

            work = *(u8**)(lbl_803FB328 + slot);
            *(u32*)(work + GBA_STATE_PHASE) = 0;
            *(s32*)(work + GBA_STATE_PORT) = channel;
            fn_800716C8(channel, work + GBA_DATA_OFFSET, fn_80093B04);
            fn_8009F77C(work);
            fn_8009F9C8(work + 0x18);
            OSCreateThread(work + GBA_DATA_OFFSET, fn_800937F4, work,
                           work + GBA_STATE_PORT, 0x4000,
                           GBA_THREAD_PRIORITY, 0);
            OSResumeThread(work + GBA_DATA_OFFSET);
            started = 1;
        }
    }

    if (started == 0) {
        return 0;
    }

    requestWork = *(u8**)(lbl_803FB328 + ((u32)channel << 2));
    requestStarted = 0;
    fn_8009F7B4(requestWork);
    if (*(s32*)(requestWork + GBA_STATE_PHASE) == 0) {
        *(s32*)(requestWork + GBA_STATE_PHASE) = 11;
        *(u32*)(requestWork + GBA_STATE_TIMEOUT) = 0x3000B;
        requestStarted = 1;
        *(void**)(requestWork + 0x4344) = requestValue;
    }
    fn_8009F890(requestWork);
    fn_800A257C(requestWork + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    if (requestStarted != 0) {
        fn_8009FABC(requestWork + 0x18);
    }
    return requestStarted;
}

#endif
