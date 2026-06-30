/**
 * @file ui_core.c
 * @brief UI Core state machine and dispatch (0x80059BDC-0x80069A60)
 *
 * Address range: 0x80059BDC - 0x80069A60
 * Total functions: 101
 */

#include "dolphin/types.h"

/* ===== External function declarations ===== */
extern s32 fn_8001DA60();
extern void fn_8001E074();
extern void fn_8001E58C();
extern void fn_8002D91C();
extern void fn_800347B8();
extern void fn_800347C4();
extern void fn_800347E8();
extern void fn_8003480C();
extern void fn_80069C0C();
extern void fn_8006A76C();
extern void fn_8006A79C();
extern void fn_8006A7AC();
extern void fn_8006A7BC();
extern void fn_8006A7C8();
extern void fn_8006A7D0();
extern void fn_8006A7E0();
extern void fn_8006A7E8();
extern void fn_8006A7F0();
extern void fn_8006A814();
extern void fn_8006A81C();
extern void fn_8006A824();
extern void fn_8006AC28();
extern void fn_8006AC6C();
extern void fn_8006ACCC();
extern void fn_8006ADB4();
extern void fn_8006ADEC();
extern void fn_8006AF44();
extern void fn_8006AFC4();
extern void fn_8006AFE4();
extern void fn_800626CC();
extern void fn_8006B09C();
extern void fn_8006B0F8();
extern s32 fn_8006B1D4();
extern void fn_8006B1F4();
extern void fn_8006B2A4();
extern void fn_8006B354();
extern void fn_8006B3C8();
extern void fn_8006B420();
extern void fn_8006B4AC();
extern void fn_8006B51C();
extern void fn_8006B5D0();
extern void fn_8006B8E8();
extern void fn_8006B8F0();
extern void fn_8006B8FC();
extern void fn_8006E0CC();
extern void fn_80071160();
extern void fn_80071344();
extern void fn_80071398();
extern void fn_800714C8();
extern void fn_800715BC();
extern void fn_8007162C();
extern void fn_80076054();
extern void fn_800776E4();
extern void fn_80077E80();
extern void fn_80077EA4();
extern void fn_800849B4();
extern void fn_800886D0();
extern void fn_80088964();
extern void fn_800889A4();
extern void fn_800889E4();
extern void fn_80088C60();
extern void fn_80088D84();
extern void fn_80089028();
extern void fn_8008A9AC();
extern void fn_8008A9E4();
extern void fn_8008AB20();
extern void fn_8008AB4C();
extern void fn_8008ABA0();
extern void fn_80092C90();
extern void fn_80093574();
extern void fn_800CE2D8();
extern void fn_800D3088();
extern void fn_800D37CC();
extern void fn_800D5648();
extern void fn_800D59B8();
extern void fn_800D5BA0();
extern void fn_800D5CB8();
extern void fn_800D61E4();
extern void fn_800D6728();
extern void fn_800D67BC();
extern void fn_800D6A00();
extern void fn_800D7820();
extern void fn_800D85D4();
extern void fn_800D888C();
extern void fn_800D88DC();
extern void fn_800DA1E8();
extern void fn_800DD270();
extern void fn_800DD384();
extern void fn_800E202C();
extern void fn_800E209C();
extern void fn_800E24B0();
extern void fn_800E27B0();
extern void fn_800E2C04();
extern void _threadSwitch();
extern void fn_800F7A08();
extern void fn_800F7A7C();
extern void fn_800F7AF0();
extern void fn_800F7BC4();
extern void fn_800F7C28();
extern void fn_800F7EF8();
extern void fn_800F915C();
extern void fn_800F9210();
extern void fn_800F92D4();
extern void fn_800F9318();
extern void fn_800F9E70();
extern void fn_800F9EE4();
extern s32 fn_800FA280();
extern void fn_800FA444();
extern void fn_800FAEF8();
extern void fn_800FB680();
extern void fn_800FBB34();
extern void fn_800FE35C();
extern void fn_800FE38C();
extern void fn_800FE4D4();
extern void fn_800FE6D0();
extern void fn_800FF540();
extern void fn_800FF548();
extern void fn_800FF56C();
extern void fn_800FF58C();
extern void fn_801022B8();
extern void menuCloseSync();
extern void fn_80102428();
/* renamed symbols referenced by asm incs (symbolmap port) */
extern void floorChangePos();
extern void GScharCmp();
extern void menuSubOpenYesNo();
extern void fn_80102510();
extern void fn_80102568();
extern void fn_80102620();
extern s32 fn_8010264C();
extern void fn_801026A4();
extern void fn_80102868();
extern void fn_80102ED4();
extern void fn_80103CC0();
extern void fn_801040B8();
extern void fn_801040D0();
extern void fn_801040F0();
extern void fn_801043A4();
extern void fn_801045A8();
extern void fn_801046B8();
extern void fn_80104704();
extern u8* fn_80105624(void);
extern void fn_801069FC();
extern void fn_80106D3C();
extern void fn_801070F4();
extern void fn_801080CC();
extern void fn_80108518();
extern void fn_80109220();
extern void fn_80109934();
extern void fn_8010B01C();
extern void fn_8010B9E8();
extern void fn_8010BBB8();
extern void fn_80113778();
extern void fn_80113828();
extern void fn_80113F48();
extern void fn_801176C8();
extern void fn_80117AD4();
extern s32 fn_8011BEB4();
extern s32 fn_8011C9EC();
extern void fn_8011CA34();
extern void fn_8011F15C();
extern s32 fn_8011F188();
extern u32 fn_8011F4F0();
extern s32 fn_801230E0();
extern s32 fn_80123CD4();
extern void fn_80123FBC();
extern s32 fn_8012640C();
extern void* fn_80129280();
extern void fn_80129384();
extern void fn_801293FC();
extern void fn_80129A78();
extern void fn_8012A248();
extern void fn_8012A774();
extern void fn_8012A7C4();
extern void fn_8012A7DC();
extern void fn_8012A80C();
extern void fn_8012A824();
extern void fn_8012AC3C();
extern void fn_8012AC54();
extern void fn_8012AC64();
extern void fn_80130054();
extern void fn_80132A38();
extern void fn_80135168();
extern void fn_80142984();
extern void fn_80165A20();
extern void fn_801666BC();
extern void fn_80166A28();
extern void fn_80166A50();
extern void fn_80166AB8();
extern void fn_80176E0C();
extern void GSscene_SetMode();
extern void fn_8017B000();
extern void fn_8017B1CC();
extern void fn_801906A0();
extern void fn_8019075C();
extern void __assert();
extern void fn_801C40F0();
extern void fn_801CB9D8();
extern void fn_801CBA0C();
extern void fn_801D04E8();
extern void fn_801EE398();
extern void fn_801EF634();
extern void fn_801FBD28();
extern void fn_801FBD58();
extern void fn_801FCC64();
extern void fn_801FCCC4();
extern void fn_8025CD64();
extern void fn_8025CDB8();
extern void fn_8025D06C();
extern void fn_8025D164();
extern void fn_8025D28C();
extern void fn_8025D2B0();
extern void fn_8025D2D4();
extern void fn_8025D364();
extern void fn_8025D3F4();
extern void fn_8025D560();
extern void fn_8025D584();
extern void fn_8025D5E0();
extern void fn_8025D644();
extern void fn_8025D744();
extern void fn_8025D788();
extern void fn_8025D808();
extern s32 fn_8025D89C();
extern void fn_8025D914();
extern void fn_8025D938();
extern s32 fn_8025D970();
extern void fn_8025D9A8();
extern void fn_8025D9CC();
extern s32 fn_8025D9F0();
extern void fn_8025DA18();
extern void fn_8025DA3C();
extern s32 fn_8025DA88();
extern void fn_8025DAAC();
extern s32 fn_8025DAD0();
extern void fn_8025DAF4();
extern void fn_8025DB2C();
extern void fn_8025DB5C();
extern void fn_8025DB80();
extern void fn_8025DBB0();
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_80478848;
extern u8 lbl_80478900;
extern u8 lbl_80478908;
extern u8 lbl_80478910;
extern u8 lbl_80478918;
extern u8 lbl_8047891A;
extern u8 lbl_8047891C;
extern u8 lbl_80478920;
extern u8 lbl_80478922;
extern u8 lbl_80478968;
extern u8 lbl_80478BD8;
extern u8 lbl_80478E90;
extern u8 lbl_80478E94;
extern u8* lbl_8047A5A0;
extern u8 lbl_8047A5A8;
extern u8 lbl_8047A5B0;
extern u8 lbl_8047A5B4;
extern u8 lbl_8047A5B8;
extern u8 lbl_8047A5BC;
extern u8 lbl_8047A5C0;
extern u8 lbl_8047A5C4;
extern u8 lbl_8047A5C8;
extern u8 lbl_8047A5D0;
extern f32 lbl_8047BF18;
extern u8 lbl_8047BF1C;
extern u8 lbl_8047BF20;
extern u8 lbl_8047BF24;
extern u8 lbl_8047BF28;
extern u8 lbl_8047BF30;
extern u8 lbl_8047BF34;
extern u8 lbl_8047BF38;
extern u8 lbl_8047BF3C;
extern u8 lbl_8047BF40;
extern u8 lbl_8047BF48;
extern u8 lbl_8047BF50;
extern u8 lbl_8047BF54;
extern u8 lbl_8047BF58;
extern u8 lbl_8047BF5C;
extern u8 lbl_8047BF60;
extern u8 lbl_8047BF64;
extern u8 lbl_8047BF68;
extern u8 lbl_8047BF6C;
extern u8 lbl_8047BF70;
extern u8 lbl_8047BF74;
extern u8 lbl_8047BF78;
extern u8 lbl_8047BF7C;
extern u8 lbl_8047BF80;
extern u8 lbl_8047BF88;
extern u8 lbl_8047BF90;
extern u8 lbl_8047BF94;
extern u8 lbl_8047BF98;
extern u8 lbl_8047BF9C;
extern u8 lbl_8047BFA0;
extern u8 lbl_8047BFA4;
extern u8 lbl_8047BFA8;
extern u8 lbl_8047BFAC;
extern u8 lbl_8047BFB0;
extern u8 lbl_8047BFB4;
extern u8 lbl_8047BFB8;
extern u8 lbl_8047BFBC;
extern u8 lbl_8047BFC0;
extern u8 lbl_8047BFC4;
extern u8 lbl_8047BFC8;
extern u8 lbl_8047BFCC;
extern u8 lbl_8047BFD0;
extern u8 lbl_8047BFD4;
extern u8 lbl_8047BFD8;
extern u8 lbl_8047BFE0;
extern u8 lbl_8047BFE8;
extern u8 lbl_8047BFEC;
extern u8 lbl_8047BFF0;
extern u8 lbl_8047BFF8;
extern u8 lbl_8047BFFC;
extern u8 lbl_8047C000;
extern u8 lbl_8047C004;
extern u8 lbl_8047C008;
extern u8 lbl_8047C00C;
extern u8 lbl_8047C010;
extern u8 lbl_8047C014;
extern u8 lbl_8047C018;
extern u8 lbl_8047C020;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802E62B0[];
extern u8 jumptable_802ED9B8[];
extern u8 jumptable_802EDB7C[];
extern u8 lbl_80267840[];
extern u8 lbl_802678D8[];
extern u8 lbl_80267A20[];
extern u8 lbl_80267A80[];
extern u8 lbl_80267A98[];
extern u8 lbl_80267AB0[];
extern u8 lbl_80267AF8[];
extern u8 lbl_80267B88[];
extern u8 lbl_802E2DB8[];
extern u8 lbl_802E6428[];
extern u8 lbl_802E7CE8[];
extern u8 lbl_802ED958[];
extern u8 lbl_802ED978[];
extern u8 lbl_802ED9A0[];
extern u8 lbl_802ED9F0[];
extern u8 lbl_802ED9FC[];
extern u8 lbl_802EDA20[];
extern u8 lbl_802EDB40[];
extern u8 lbl_802EDB64[];
extern u8 lbl_802EF0A8[];
extern u8 lbl_80314E08[];
extern u8 lbl_80314F98[];
extern u8 lbl_803A9A60[];

typedef struct UICmdMsg {
    u8 _0[4];
    s8 flags4;  /* 0x4 */
    u8 _5;
    s16 cmd;    /* 0x6 */
    u8 _8[0x48];
    s16 s50;
    s16 s52;
    s16 s54;
    s16 s56;
} UICmdMsg;

extern u8 lbl_803A9E40[];
extern u8 lbl_803A9EA0[];
extern u8 lbl_803A9F08[];

/* ===== Forward declarations ===== */
void fn_80059BDC(void);
void fn_8005CCD0(void);
void fn_8005CD88(void);
void fn_8005CEE8(u32 arg);
void fn_8005CF2C(void);
s32 fn_8005D094(void);
s32 fn_8005D0B8(void);
s32 fn_8005D0DC(void);
s32 fn_8005D130(void);
void fn_8005D184(void);
s32  fn_8005D26C(void);
void fn_8005D2E8(void);
void fn_8005D3D0(void);
s32 fn_8005D4AC(void* a, void* b);
s32 fn_8005D4F4(void* a, void* b);
s32 fn_8005D53C(void* a, void* b);
s32 fn_8005D584(void* a, void* b);
void fn_8005D5CC(void);
void fn_8005D6A8(u8* ctx);
u8   fn_8005D738(u8 arg);
u16  fn_8005D798(u8 *base, u8 sel);
void* fn_8005D7F8(u32 idx);
void* fn_8005D830(u32 idx);
void* fn_8005D858(u32 idx);
void fn_8005D880(u32 idx, u16 val1, u16 val2);
s32 fn_8005D8B8(u32 idx);
void fn_8005D8F8(u32 idx, u32 val);
void* fn_8005D934(u32 idx);
void fn_8005D95C(s32 idx, u16* out1, u16* out2);
void fn_8005D9AC(s32 idx, u16 val1, u16 val2);
u8 fn_8005D9E4(s32 idx);
void* fn_8005DA18(s32 idx);
void fn_8005DA48(void);
s32  fn_8005DBC4(void);
void fn_8005DC24(void);
void fn_8005DCC4(void);
void fn_8005DEE8(void);
void fn_8005DFC8(void);
void fn_8005E690(void);
void fn_8005E730(void);
void fn_8005E750(void);
void fn_8005E7F0();
void fn_8005FFE4(void);
void fn_80060434();
s32 fn_800608C4();
void fn_800609B4();
void fn_80060A28(void);
void fn_80060D70();
void fn_80060EF4();
u32 fn_80061018(void);
void fn_80061028(u32 arg);
void fn_8006106C();
void fn_80061240(void);
void fn_80061454();
void fn_800615F4();
void fn_800617E0();
void fn_80061A2C();
void fn_80061B74();
void fn_80061BBC();
void fn_80061D34(void);
void fn_80061F6C(void);
void fn_80062284(void);
void fn_80062334(void);
void _menuCBBattleStartDispTrainerTexCallBack__FlPvl(void);
void fn_80062834(void);
void fn_80062948(void);
void fn_80062AB4(void);
void fn_80063060(void);
void fn_800637B0(void);
void fn_800638F4(void);
void fn_80063AD4(void);
void fn_80063D10(void);
void fn_80063D14(void);
void fn_80064378(u8* ctx, u8* arg1);
void fn_800643D4();
void fn_80065628(void);
void fn_80065730(void);
void fn_80065838(void);
void fn_80065940(void);
void fn_80065A48();
void fn_800676EC(void);
void fn_800679C0(void);
void fn_800681B4(void);
void fn_80068418(u8*, s32);
s32 fn_80068738(void);
void fn_80068794();
void fn_800688C4();
void fn_800689FC();
void fn_80068BB0();
void fn_80068DBC();
void fn_80068F84(void);
u8 fn_80069048(void);
void fn_8006905C(void);
void fn_80069220(void);
void fn_800693A4(void);
void fn_80069504(void);
void fn_80069664(void);
s32 fn_800697C4(void);
void fn_800697F4(void);
void fn_80069944(void);
s32 fn_80069A08(s32 a, s32 b, s32 c, s32 d);

/* ===== Function implementations ===== */


/* 0x80059BDC | size: 0x30F4 */
#if 1
asm void fn_80059BDC(void) {
#include "src/game/ui/ui_core_fn_80059BDC.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
void fn_80059BDC(void) {
    extern s32 fn_8007162C(void);
    extern s32 fn_80071344(void);
    extern s32 fn_80071398();
    extern s32 fn_800714C8(void);
    extern s32 fn_80071160(void);
    extern s32 fn_801026A4(s32, s32, void*, s32, s32, s32, ...);
    extern s32 fn_80102620(s32);
    extern s32 fn_800F7EF8(s32);
    extern s32 fn_8008ABA0(s32);
    extern u8* fn_8006AFC4(void*);
    extern s32 fn_8006A7E8(void*);
    extern void* fn_8006A7C8(void*);
    extern s32 fn_8006A7BC(void*);
    extern s32 fn_8006A76C(void*);
    extern u8* fn_8006B420(void);
    extern u8* fn_8006B51C(s32);
    extern u8* fn_8006B09C(s32);
    extern u8* fn_8006AFE4(s32);
    extern s32 fn_8006ADEC(void);
    extern u32 fn_800FF540(void);
    extern u32 fn_801906A0(s32);
    extern u8* fn_80104704(u32);
    extern s32 fn_801046B8(void);
    extern u8* fn_80105624(void);
    extern s32 fn_80076054(void*, void*);
    extern s32 GScharCmp(void*, void*);
    extern s32 fn_80089028(void);
    extern s32 fn_80093574(s32);
    extern s32 fn_800849B4(s32, s32, void*, void*);
    extern u32 fn_8012AC3C(void*);
    extern u8* fn_8012AC54(void*);
    extern s32 fn_801022B8(s32);
    extern u32 fn_80135168(s32, s32);
    extern s32 fn_80088C60(void);
    extern s32 fn_80088964(void);
    extern s32 fn_80088D84(void);
    extern s32 fn_800889A4(void);
    extern s32 fn_800776E4(void*);
    extern s32 fn_80077EA4(void*, void*);
    extern s32 fn_801D04E8(void);
    extern s8 menuSubOpenYesNo(s32, s32, s32, s32);
    extern void floorChangePos(u32, s32, f32, f32, f32);
    typedef struct SaveImage { u32 data[0x330B]; } SaveImage;
#define WORKP ((u8*)fn_80129280(0, 0xE))
    u8* dat = (u8*)lbl_80267840;
    s32 prevCmd;
    s32 first;
    u32 floorId;
    u8 posFlag;
    s32 warpArg;
    s32 cmd;
    f32 posX;
    f32 posY;
    f32 posZ;

    fn_80129280(0, 0xE);
    posFlag = 0;
    warpArg = 0;
    prevCmd = fn_8007162C();
    floorId = 0;
    if (fn_800FF540() != 0) {
        __assert((char*)(dat + 0x98), 0x267, (char*)(dat + 0xB0));
    }
    if (fn_801906A0(0x8AE) != 0) {
        __assert((char*)(dat + 0x98), 0x268, (char*)(dat + 0xD0));
    }
    while (fn_8007162C() > 0) {
        s32 mode;
        first = fn_8007162C();
        cmd = fn_8007162C();
        fn_801026A4(0xBE, 0, NULL, 0x10, 0, 1, first);
        switch (fn_8007162C()) {
        case 0xAA: {
            s32 v = fn_80071344();
            if (v < 0) {
                cmd = -1;
                break;
            }
            cmd = (*(s32(*)[6])(dat + 0))[v];
            switch (v) {
            case 1:
                floorId = 1;
                break;
            }
            break;
        }
        case 0xA8: {
            s32 t;
            switch (fn_80071344()) {
            case 0:
                cmd = 0xAC;
                t = 0;
                *(s32*)(WORKP + 4) = t;
                t = 0;
                *(s32*)(WORKP + 0x10) = t;
                t = 0;
                *(s32*)(WORKP + 0) = t;
                fn_8006A7E0(WORKP + 0x59A8, 0);
                break;
            case 1:
                cmd = 0xB3;
                t = 2;
                *(s32*)(WORKP + 4) = t;
                t = 4;
                *(s32*)(WORKP + 0x10) = t;
                t = 2;
                *(s32*)(WORKP + 0) = t;
                break;
            case 3:
            default:
                cmd = -1;
                break;
            }
            break;
        }
        case 0xAC: {
            fn_8006B4AC(0);
            {
                s32 v = fn_80071344();
                if (v < 0) {
                    cmd = -1;
                    break;
                }
                prevCmd = (*(s32(*)[6])(dat + 0x18))[v];
            }
            if (fn_8006AFC4(WORKP) == 0 && (prevCmd == 0xAE || prevCmd == 0xAF)) {
                fn_80106D3C(2, 0x3BFE, 1, 0);
                fn_801069FC(1);
                break;
            }
            switch (prevCmd) {
            case 0xAE: {
                s32 t = 0;
                *(s32*)(WORKP + 0) = t;
                cmd = prevCmd;
                break;
            }
            case 0xAF: {
                s32 t;
                t = 1;
                *(s32*)(WORKP + 0) = t;
                t = 6;
                *(s32*)(WORKP + 0xC) = t;
                t = 2;
                *(s32*)(WORKP + 8) = t;
                cmd = prevCmd;
                break;
            }
            case 0xAD:
                if ((u8)fn_8006A7BC(WORKP) == 0 || fn_8006AFC4(WORKP) == 0) {
                    fn_80106D3C(2, 0x4415, 1, 0);
                    fn_801069FC(1);
                    break;
                }
                cmd = prevCmd;
                break;
            default:
                cmd = prevCmd;
                break;
            }
            break;
        }
        case 0xAD: {
            s32 ok = 0;
            u8* st;
            s32 r;
            if ((u8)fn_8006A7BC(WORKP) != 0) {
                if (fn_8006AFC4(WORKP) != 0) {
                    ok = 1;
                }
            }
            if (ok == 0) {
                __assert((char*)(dat + 0x98), 0x30F, (char*)(dat + 0x10C));
            }
            cmd = (s32)(WORKP + 0xC98C);
            st = fn_8006AFC4(WORKP);
            if ((u8)fn_80102620(0xC8)) {
                fn_80102510(0xC8);
                while ((u8)fn_80102620(0xC8)) {
                    _threadSwitch();
                }
            }
            r = fn_8006A7E8(st);
            fn_801026A4(0xC8, 0, NULL, 0x10, 0, 4, fn_8006A7C8(st), r, 0, cmd);
            if ((u8)fn_80102620(0xD6)) {
                fn_80102510(0xD6);
                while ((u8)fn_80102620(0xD6)) {
                    _threadSwitch();
                }
            }
            {
                u32 modeAd = 1;
                r = fn_801026A4(0xD6, 0, &modeAd, 0x10, 1, 4, 0x3DB0, 0x3DB1, 0x3D89, 0);
            }
            fn_80102510(0xD6);
            if (r != 0) {
                fn_80102568(0xC8, 0, 0);
                fn_8006E0CC();
                cmd = -1;
                break;
            }
            fn_8006A7F0(fn_8006AFE4(1), fn_8006AFC4(WORKP));
            if ((u8)fn_8006A76C(WORKP) != 0) {
                fn_8006A79C(WORKP);
            } else {
                s32 wasOpen = fn_8006A7BC(WORKP);
                fn_8006A79C(WORKP);
                fn_80106D3C(2, 0x44D9, 1, 0);
                fn_801069FC(1);
                if (fn_80088C60() < 0) {
                    if ((u8)wasOpen != 0) {
                        fn_8006A7AC(WORKP);
                    }
                    fn_80102568(0xC8, 0, 0);
                    fn_8006E0CC();
                    cmd = -1;
                    break;
                }
            }
            if (*(s32*)(WORKP + 0xC98C) != 1) {
                u8* q = WORKP + 0x10000;
                s32 t;
                t = *(s32*)(q - 0x3674);
                *(s32*)(WORKP + 0) = t;
                t = *(s32*)(q - 0x3670);
                *(s32*)(WORKP + 4) = t;
                t = *(s32*)(q - 0x366C);
                *(s32*)(WORKP + 8) = t;
                t = *(s32*)(q - 0x3668);
                *(s32*)(WORKP + 0xC) = t;
                t = *(s32*)(q - 0x3664);
                *(s32*)(WORKP + 0x10) = t;
                t = *(s32*)(q - 0x3660);
                *(s32*)(WORKP + 0x14) = t;
                t = *(s32*)(q - 0x365C);
                *(s32*)(WORKP + 0x18) = t;
                if (*(u32*)(q - 0x3660) == 0) {
                    fn_8006ADB4(0);
                }
                if (*(s32*)(WORKP + 0) != 0) {
                    __assert((char*)(dat + 0x98), 0xAB, (char*)(dat + 0x158));
                }
                fn_8019075C(0x8AE, (*(s32*)(q - 0x3670) == 0) ? 1 : 2);
                fn_80069C0C(WORKP);
                cmd = 0xD1;
            } else {
                u8* q = WORKP + 0x10000;
                s32 t;
                t = *(s32*)(q - 0x3674);
                *(s32*)(WORKP + 0) = t;
                t = *(s32*)(q - 0x3670);
                *(s32*)(WORKP + 4) = t;
                t = *(s32*)(q - 0x366C);
                *(s32*)(WORKP + 8) = t;
                t = *(s32*)(q - 0x3668);
                *(s32*)(WORKP + 0xC) = t;
                t = *(s32*)(q - 0x3664);
                *(s32*)(WORKP + 0x10) = t;
                t = *(s32*)(q - 0x3660);
                *(s32*)(WORKP + 0x14) = t;
                t = *(s32*)(q - 0x365C);
                *(s32*)(WORKP + 0x18) = t;
                if (*(u32*)(q - 0x3660) == 0) {
                    fn_8006ADB4(0);
                }
                if (*(s32*)(WORKP + 0xC) != 6) {
                    __assert((char*)(dat + 0x98), 0x81, (char*)(dat + 0x184));
                }
                if (*(s32*)(WORKP + 0) != 1) {
                    __assert((char*)(dat + 0x98), 0x82, (char*)(dat + 0x1B0));
                }
                fn_8019075C(0x8AE, (*(s32*)(q - 0x3670) == 0) ? 1 : 2);
                fn_8019075C(0xB59, *(s32*)(WORKP + 0x14));
                fn_8019075C(0xAFC, 0);
                fn_8019075C(0xB11, 0);
                fn_8019075C(0xDE1, 0);
                fn_80130054();
                fn_8019075C(0xAFC, *(s32*)(q - 0x3634));
                fn_8019075C(0xB11, *(s32*)(q - 0x362C));
                fn_8019075C(0xDE1, *(s32*)(q - 0x3628));
                floorId = *(s32*)(q - 0x3654);
                posX = *(f32*)(WORKP + 0xC9B4);
                posY = *(f32*)(WORKP + 0xC9B8);
                posZ = *(f32*)(WORKP + 0xC9BC);
                warpArg = (s32)(lbl_8047BF18 * *(f32*)(WORKP + 0xC9C4));
                posFlag = 1;
                cmd = 0x105;
            }
            fn_80102568(0xC8, 0, 0);
            fn_8006E0CC();
            break;
        }
        case 0xAE: {
            s32 t;
            s32 v;
            t = 2;
            *(s32*)(WORKP + 8) = t;
            t = 0;
            *(s32*)(WORKP + 0) = t;
            v = fn_80071344();
            switch (v) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                *(s32*)(WORKP + 0xC) = v;
                fn_8006B4AC((*(s32(*)[7])(dat + 0x30))[v]);
                cmd = 0xAF;
                break;
            case 6:
                cmd = 0xC0;
                break;
            default:
                cmd = -1;
                break;
            }
            break;
        }
        case 0xAF: {
            s32 v = fn_80071344();
            if (v < 0) {
                cmd = -1;
                break;
            }
            if (v == 2) {
                cmd = -1;
                break;
            }
            *(s32*)(WORKP + 4) = v;
            if (*(s32*)(WORKP + 0) == 3) {
                u8* npc;
                u8* mem;
                s32 r;
                prevCmd = (u8)(v == 1);
                npc = fn_80104704((u16)fn_8007162C());
                if (npc != NULL) {
                    mem = *(u8**)(npc + 0x20);
                } else {
                    mem = NULL;
                }
                fn_80108518(mem + 0xC, 0x1CE);
                r = fn_801026A4(0xBC, 0, NULL, 0x10, 1, 1, prevCmd);
                switch (r) {
                case 0:
                case 1:
                case 2:
                case 3: {
                    s32 t;
                    *(s32*)(WORKP + 0x10) = r;
                    t = 0;
                    *(s32*)(WORKP + 8) = t;
                    t = 0;
                    *(s32*)(WORKP + 0xC) = t;
                    cmd = 0xCC;
                    fn_80102568(0xBC, 0, 0);
                    break;
                }
                default:
                    fn_80102568(0xBC, 0, 1);
                    fn_80108518(mem + 0xC, 0x1CA);
                    break;
                }
                break;
            }
            if (*(s32*)(WORKP + 0x10) == 4) {
                cmd = 0xBF;
            } else {
                cmd = 0xB1;
            }
            break;
        }
        case 0xEB:
            __assert((char*)(dat + 0x98), 0x3C4, (char*)&lbl_8047BF1C);
            break;
        case 0xB0:
            switch (fn_80071344()) {
            case 0:
                if ((u8)fn_801D04E8() == 0) {
                    fn_80106D3C(2, 0x44EB, 1, 0);
                    fn_801069FC(1);
                    break;
                }
                if (fn_80135168(0, 4) == 0) {
                    fn_80106D3C(2, 0x444D, 1, 0);
                    fn_801069FC(1);
                    break;
                }
                fn_8012AC64(lbl_8047A5A0 + 0x1660, fn_80129280(0, 2));
                cmd = 0xEE;
                break;
            case 1:
                cmd = 0xED;
                break;
            default:
                cmd = -1;
                break;
            }
            break;
        case 0xEE: {
            prevCmd = (s32)(lbl_8047A5A0 + 0x1660);
            fn_8006B4AC(2);
            if ((u8)fn_800776E4((u8*)prevCmd) == 0) {
                s32 m;
                fn_801026A4(0xBE, 0, NULL, 0x10, 0, 1, 0xF5);
                fn_801026A4(0xDA, 0, NULL, 0x10, 0, 4, prevCmd, 0, fn_8006B420(), 0);
                fn_80102868(0xDA, 0, -0x28);
                m = fn_80076054((u8*)prevCmd, fn_8006B420());
                if ((u16)m == 0) {
                    __assert((char*)(dat + 0x98), 0x1BB, (char*)(dat + 0x1E0));
                }
                fn_80166A28(0x26);
                fn_80106D3C(7, (u16)m, 1, 0);
                fn_80106D3C(7, 0x440A, 1, 0);
                fn_80102568(0xDA, 0, 0);
                fn_80102568(0xBE, 0, 1);
                fn_80102568(0xDA, 0, 1);
                fn_8006E0CC();
                fn_8006B4AC(0);
                fn_801069FC(1);
                fn_800714C8();
                cmd = -1;
                break;
            }
            fn_8006B4AC(0);
            fn_8012A774((u8*)prevCmd, 0);
            fn_8006AC28(lbl_8047A5A0, 0);
            fn_8006A824(lbl_8047A5A0, (u8*)prevCmd);
            fn_800714C8();
            cmd = 0xB2;
            break;
        }
        case 0xED: {
            u32 i = 0;
            s32 r;
            u8* arr[4];
            prevCmd = i;
            for (; i < 4; i++) {
                memset(lbl_8047A5A0 + 0x1660 + prevCmd, 0, 0xB18);
                fn_8012A248(lbl_8047A5A0 + 0x1660 + prevCmd);
                prevCmd += 0xB18;
            }
            arr[0] = NULL;
            arr[1] = lbl_8047A5A0 + 0x1660;
            arr[2] = NULL;
            arr[3] = NULL;
            fn_8006B4AC(2);
            r = fn_800849B4(0, 2, arr, NULL);
            fn_8006B4AC(0);
            if (r < 0) {
                fn_800714C8();
                cmd = -1;
                break;
            }
            fn_8006AC28(lbl_8047A5A0, 0);
            fn_8006A824(lbl_8047A5A0, lbl_8047A5A0 + 0x1660);
            fn_800714C8();
            cmd = 0xB2;
            break;
        }
        case 0xB2: {
            u8* sv = lbl_8047A5A0;
            s32 r;
            if ((u8)fn_80102620(0xDA)) {
                fn_80102510(0xDA);
                while ((u8)fn_80102620(0xDA)) {
                    _threadSwitch();
                }
            }
            r = fn_8006A7E8(sv);
            fn_801026A4(0xDA, 0, NULL, 0x10, 0, 4, fn_8006A7C8(sv), r, 0, 0);
            if ((u8)fn_80102620(0xD6)) {
                fn_80102510(0xD6);
                while ((u8)fn_80102620(0xD6)) {
                    _threadSwitch();
                }
            }
            {
                u32 modeB2 = 0;
                r = fn_801026A4(0xD6, 0, &modeB2, 0x10, 1, 4, 0x3D47, 0x3D49, 0, -0x2A);
            }
            fn_80102510(0xD6);
            if (r == 0) {
                if (fn_8006AFC4(WORKP) != 0) {
                    u8* p;
                    fn_80102568(0xDA, 0, 0);
                    fn_8006E0CC();
                    fn_80102568(0xBE, 0, 1);
                    fn_801026A4(0xBE, 0, NULL, 0x10, 0, 1, 0xEB);
                    p = fn_8006AFC4(WORKP);
                    if ((u8)fn_80102620(0xDA)) {
                        fn_80102510(0xDA);
                        while ((u8)fn_80102620(0xDA)) {
                            _threadSwitch();
                        }
                    }
                    r = fn_8006A7E8(p);
                    fn_801026A4(0xDA, 0, NULL, 0x10, 0, 4, fn_8006A7C8(p), r, 0, 0);
                    if ((u8)fn_80102620(0xD6)) {
                        fn_80102510(0xD6);
                        while ((u8)fn_80102620(0xD6)) {
                            _threadSwitch();
                        }
                    }
                    {
                        u32 modeB2b = 1;
                        r = fn_801026A4(0xD6, 0, &modeB2b, 0x10, 1, 4, 0x3D47, 0x3D49, 0x3C54, -0x28);
                    }
                    fn_80102510(0xD6);
                    if (r != 0) {
                        goto b2_cancel;
                    }
                    if ((u8)fn_8006A7BC(WORKP) != 0) {
                        fn_80106D3C(2, 0x44C2, 1, 0);
                        prevCmd = menuSubOpenYesNo(0, 0x3C, 0x9E, 1);
                        fn_801069FC(1);
                        if (prevCmd != 0) {
                            goto b2_cancel;
                        }
                    }
                    fn_80102568(0xDA, 0, 0);
                    fn_8006E0CC();
                    fn_80102568(0xBE, 0, 1);
                    fn_801026A4(0xBE, 0, NULL, 0x10, 0, 1, first);
                    {
                        u8* sv2 = lbl_8047A5A0;
                        if ((u8)fn_80102620(0xDA)) {
                            fn_80102510(0xDA);
                            while ((u8)fn_80102620(0xDA)) {
                                _threadSwitch();
                            }
                        }
                        r = fn_8006A7E8(sv2);
                        fn_801026A4(0xDA, 0, NULL, 0x10, 0, 4, fn_8006A7C8(sv2), r, 0, 0);
                    }
                }
                memcpy(lbl_8047A5A0 + 0x4318, fn_80129280(0, 0xE), 0xCC2C);
                fn_8006AF44(fn_80129280(0, 0xE), lbl_8047A5A0);
                if ((u8)fn_801D04E8() == 0) {
                    fn_80166A28(0x26);
                    fn_80106D3C(2, 0x3C60, 1, 0);
                    if (fn_8006A7E8(lbl_8047A5A0) != 0) {
                        fn_80106D3C(2, 0x3D55, 1, 0);
                    }
                } else if (fn_800889A4() < 0) {
                    memcpy(fn_80129280(0, 0xE), lbl_8047A5A0 + 0x4318, 0xCC2C);
                    if (fn_8006A7E8(lbl_8047A5A0) != 0) {
                        fn_80106D3C(2, 0x3D55, 1, 0);
                    }
                } else if (fn_8006A7E8(lbl_8047A5A0) == 0) {
                    fn_80106D3C(2, 0x3C5E, 1, 0);
                } else {
                    fn_80106D3C(2, 0x3D44, 1, 0);
                }
                fn_80102568(0xDA, 0, 0);
                fn_8006E0CC();
                fn_800714C8();
                fn_801069FC(1);
                cmd = -1;
                break;
            }
        b2_cancel:
            if (fn_8006A7E8(lbl_8047A5A0) != 0) {
                fn_80106D3C(2, 0x3D55, 1, 0);
                fn_80102568(0xDA, 0, 0);
                fn_8006E0CC();
                fn_801069FC(1);
            }
            cmd = -1;
            break;
        }
        case 0xCC: {
            u8* p;
            s32 r;
            fn_80069C0C(fn_80129280(0, 0xE));
            p = fn_8006B09C(0);
            if ((u8)fn_80102620(0xDA)) {
                fn_80102510(0xDA);
                while ((u8)fn_80102620(0xDA)) {
                    _threadSwitch();
                }
            }
            r = fn_8006A7E8(p);
            fn_801026A4(0xDA, 0, NULL, 0x10, 0, 4, fn_8006A7C8(p), r, 0, 0);
            if ((u8)fn_80102620(0xD6)) {
                fn_80102510(0xD6);
                while ((u8)fn_80102620(0xD6)) {
                    _threadSwitch();
                }
            }
            {
                u32 modeCc = 0;
                r = fn_801026A4(0xD6, 0, &modeCc, 0x10, 1, 4, 0x3D47, 0x3D49, 0, -0x2A);
            }
            fn_80102510(0xD6);
            fn_80102568(0xDA, 0, 0);
            fn_8006E0CC();
            if (r != 0) {
                cmd = -1;
                break;
            }
            cmd = 0xD1;
            break;
        }
        case 0xB1: {
            s32 r;
            s32 m;
            u8* wm;
            cmd = (s32)fn_8006AFC4(WORKP);
            if (*(s32*)(WORKP + 4) == 2) {
                __assert((char*)(dat + 0x98), 0x4B9, (char*)(dat + 0x1F4));
            }
            if ((u32)cmd == 0) {
                __assert((char*)(dat + 0x98), 0x4BA, (char*)&lbl_8047BF20);
            }
            wm = fn_8006B420();
            if ((u8)fn_80102620(0xDA)) {
                fn_80102510(0xDA);
                while ((u8)fn_80102620(0xDA)) {
                    _threadSwitch();
                }
            }
            r = fn_8006A7E8((void*)cmd);
            fn_801026A4(0xDA, 0, NULL, 0x10, 0, 4, fn_8006A7C8((void*)cmd), r, wm, 0);
            m = fn_80076054((u8*)cmd + 0xB44, fn_8006B420());
            if ((u16)m != 0) {
                fn_80106D3C(1, (u16)m, 1, 0);
                fn_801069FC(1);
                fn_80102568(0xDA, 0, 0);
                fn_8006E0CC();
                fn_800714C8();
                cmd = -1;
                break;
            }
            if ((u8)fn_80102620(0xD6)) {
                fn_80102510(0xD6);
                while ((u8)fn_80102620(0xD6)) {
                    _threadSwitch();
                }
            }
            {
                u32 modeB1 = 0;
                r = fn_801026A4(0xD6, 0, &modeB1, 0x10, 1, 4, 0x3D47, 0x3D49, 0, -0x2A);
            }
            fn_80102510(0xD6);
            if (r != 0) {
                fn_80102568(0xDA, 0, 0);
                fn_8006E0CC();
                cmd = -1;
                break;
            }
            {
                u8* dst = lbl_8047A5A0 + 0x4318;
                SaveImage* dstSaveImage = (SaveImage*)dst;
                SaveImage* srcSaveImage = (SaveImage*)fn_80129280(0, 0xE);

                *dstSaveImage = *srcSaveImage;
                prevCmd = (s32)fn_8006AFC4(dst);
                if ((u8)fn_8006A7BC(dst) != 0 && (u32)prevCmd != 0) {
                    fn_80102568(0xDA, 0, 0);
                    fn_8006E0CC();
                    fn_80102568(0xBE, 0, 1);
                    fn_801026A4(0xBE, 0, NULL, 0x10, 0, 1, 0xD7);
                    if ((u8)fn_80102620(0xC8)) {
                        fn_80102510(0xC8);
                        while ((u8)fn_80102620(0xC8)) {
                            _threadSwitch();
                        }
                    }
                    r = fn_8006A7E8((void*)prevCmd);
                    fn_801026A4(0xC8, 0, NULL, 0x10, 0, 4, fn_8006A7C8((void*)prevCmd), r, 0, dst + 0xC98C);
                    if ((u8)fn_80102620(0xD6)) {
                        fn_80102510(0xD6);
                        while ((u8)fn_80102620(0xD6)) {
                            _threadSwitch();
                        }
                    }
                    {
                        u32 modeB1b = 1;
                        r = fn_801026A4(0xD6, 0, &modeB1b, 0x10, 1, 4, 0x3D47, 0x3D49, 0x44C8, 0);
                    }
                    fn_80102510(0xD6);
                    fn_80102568(0xC8, 0, 0);
                    fn_8006E0CC();
                    fn_80102568(0xBE, 0, 1);
                    if (r != 0) {
                        cmd = -1;
                        break;
                    }
                    if ((u8)fn_8006A76C(WORKP) == 0) {
                        u8* wm2;
                        fn_801026A4(0xBE, 0, NULL, 0x10, 0, 1, first);
                        wm2 = fn_8006B420();
                        if ((u8)fn_80102620(0xDA)) {
                            fn_80102510(0xDA);
                            while ((u8)fn_80102620(0xDA)) {
                                _threadSwitch();
                            }
                        }
                        r = fn_8006A7E8((void*)cmd);
                        fn_801026A4(0xDA, 0, NULL, 0x10, 0, 4, fn_8006A7C8((void*)cmd), r, wm2, 0);
                    }
                }
            }
            if ((u8)fn_8006A76C(WORKP) != 0) {
                fn_8006A79C(WORKP);
            } else {
                s32 wasOpen = fn_8006A7BC(WORKP);
                fn_8006A79C(WORKP);
                if (fn_80088C60() < 0) {
                    if ((u8)wasOpen != 0) {
                        fn_8006A7AC(WORKP);
                    }
                    fn_80102568(0xDA, 0, 0);
                    fn_8006E0CC();
                    cmd = -1;
                    break;
                }
            }
            {
                s32 v3;
                s32 v2;
                s32 v1;
                s32 v0;
                s32 buf[20];
                s32 t;
                prevCmd = *(s32*)(WORKP + 0x10);
                v3 = *(s32*)(WORKP + 0xC);
                v2 = *(s32*)(WORKP + 8);
                v1 = *(s32*)(WORKP + 4);
                v0 = *(s32*)(WORKP + 0);
                memset(buf, 0, 0x50);
                buf[0] = v0;
                buf[1] = v1;
                buf[2] = v2;
                buf[3] = v3;
                buf[4] = prevCmd;
                buf[5] = 0;
                buf[6] = 5;
                if (*(s32*)(WORKP + 0) == 1) {
                    t = buf[0];
                    *(s32*)(WORKP + 0) = t;
                    t = buf[1];
                    *(s32*)(WORKP + 4) = t;
                    t = buf[2];
                    *(s32*)(WORKP + 8) = t;
                    t = buf[3];
                    *(s32*)(WORKP + 0xC) = t;
                    t = buf[4];
                    *(s32*)(WORKP + 0x10) = t;
                    t = buf[5];
                    *(s32*)(WORKP + 0x14) = t;
                    t = buf[6];
                    *(s32*)(WORKP + 0x18) = t;
                    if ((u32)buf[5] == 0) {
                        fn_8006ADB4(0);
                    }
                    if (*(s32*)(WORKP + 0xC) != 6) {
                        __assert((char*)(dat + 0x98), 0x81, (char*)(dat + 0x184));
                    }
                    if (*(s32*)(WORKP + 0) != 1) {
                        __assert((char*)(dat + 0x98), 0x82, (char*)(dat + 0x1B0));
                    }
                    fn_8019075C(0x8AE, (buf[1] == 0) ? 1 : 2);
                    fn_8019075C(0xB59, *(s32*)(WORKP + 0x14));
                    fn_8019075C(0xAFC, 0);
                    fn_8019075C(0xB11, 0);
                    fn_8019075C(0xDE1, 0);
                    fn_80130054();
                    floorId = 0x4C;
                    cmd = 0x105;
                } else {
                    t = buf[0];
                    *(s32*)(WORKP + 0) = t;
                    t = buf[1];
                    *(s32*)(WORKP + 4) = t;
                    t = buf[2];
                    *(s32*)(WORKP + 8) = t;
                    t = buf[3];
                    *(s32*)(WORKP + 0xC) = t;
                    t = buf[4];
                    *(s32*)(WORKP + 0x10) = t;
                    t = buf[5];
                    *(s32*)(WORKP + 0x14) = t;
                    t = buf[6];
                    *(s32*)(WORKP + 0x18) = t;
                    if ((u32)buf[5] == 0) {
                        fn_8006ADB4(0);
                    }
                    if (*(s32*)(WORKP + 0) != 0) {
                        __assert((char*)(dat + 0x98), 0xAB, (char*)(dat + 0x158));
                    }
                    fn_8019075C(0x8AE, (buf[1] == 0) ? 1 : 2);
                    fn_80069C0C(WORKP);
                    cmd = 0xD1;
                }
            }
            fn_80102568(0xDA, 0, 0);
            fn_8006E0CC();
            break;
        }
        case 0xB3: {
            s32 v = fn_80071344();
            if (v < 0) {
                cmd = -1;
                break;
            }
            if (v < 4) {
                u8* e = dat + 0x4C;
                s32* st;
                s32 t;
                e += v << 4;
                st = (s32*)(e + 8);
                if (*st == 0) {
                    if ((u8)fn_801D04E8() == 0) {
                        fn_80106D3C(2, 0x44EA, 1, 0);
                        fn_801069FC(1);
                        break;
                    }
                    if (fn_80135168(0, 4) == 0) {
                        fn_80106D3C(2, 0x44DB, 1, 0);
                        fn_801069FC(1);
                        break;
                    }
                }
                t = *(s32*)e;
                *(s32*)(WORKP + 4) = t;
                t = *(s32*)(e + 4);
                *(s32*)(WORKP + 0xC) = t;
                fn_8006A7E0(WORKP + 0x59A8, *st);
                fn_8006A81C(WORKP + 0x59A8, (s8)e[0xC]);
                {
                    u8* p = e + 1;
                    u32 i = 1;
                    prevCmd = 0x1660;
                    for (; i < 4; i++) {
                        fn_8006A7E0(WORKP + (prevCmd + 0x59A8), 1);
                        fn_8006A81C(WORKP + (prevCmd + 0x59A8), (s8)p[0xC]);
                        prevCmd += 0x1660;
                        p += 1;
                    }
                }
                if (*(s32*)(WORKP + 4) == 2) {
                    cmd = 0xBF;
                } else {
                    cmd = 0xAF;
                }
                break;
            }
            cmd = -1;
            break;
        }
        case 0xC0: {
            s32 r;
            mode = 0;
            if (prevCmd == 0xC1) {
                mode = 7;
            } else {
                fn_80077E80(lbl_8047A5A0 + 0x42C0, fn_8006B420());
            }
            for (;;) {
                r = fn_801026A4(fn_8007162C(), 0, &mode, 0x10, 1, 1, lbl_8047A5A0 + 0x42C0);
                if (r < 0) {
                    cmd = -1;
                    goto c0_end;
                }
                switch (r) {
                case 3:
                    if (*(s32*)(lbl_8047A5A0 + 0x42C8) == 2) {
                        u8 tmp[0x3C];
                        s32 r2;
                        memcpy(tmp, lbl_8047A5A0 + 0x42D8, 0x3C);
                        r2 = fn_801026A4(0xB4, 0, NULL, 0x10, 1, 1, tmp);
                        fn_80102510(0xB4);
                        if (r2 >= 0) {
                            memcpy(lbl_8047A5A0 + 0x42D8, tmp, 0x3C);
                        }
                        mode = 3;
                        continue;
                    }
                    goto c0_end;
                case 8:
                    fn_800714C8();
                    cmd = 0xC1;
                    goto c0_end;
                default:
                    goto c0_end;
                }
            }
        c0_end:
            break;
        }
        case 0xC1: {
            s32 r = fn_801026A4(fn_8007162C(), 0, NULL, 0x10, 1, 1, lbl_8047A5A0 + 0x42C0);
            if (r < 0) {
                cmd = -1;
                break;
            }
            switch (r) {
            case 5:
                fn_800714C8();
                cmd = 0xC0;
                break;
            case 6:
                if (fn_801022B8(fn_8007162C()) == 0x9FC) {
                    fn_80077E80(lbl_8047A5A0 + 0x42C0, fn_8006B51C(0));
                    *(s16*)(lbl_8047A5A0 + 0x42C6) = 6;
                    break;
                }
                goto c1_apply;
            case 7:
            c1_apply:
                if ((u8)fn_80077EA4(lbl_8047A5A0 + 0x42C0, fn_8006B420()) == 0) {
                    s32 ofs = *(s32*)(WORKP + 8) * 0x54;
                    fn_80077E80(WORKP + (ofs + 0xC9DC), lbl_8047A5A0 + 0x42C0);
                    for (;;) {
                        if ((u8)fn_801D04E8() == 0) {
                            break;
                        }
                        fn_80106D3C(2, 0x44B1, 1, 0);
                        if (menuSubOpenYesNo(0, 0x3C, 0x9E, 0) != 0) {
                            break;
                        }
                        if (fn_80088964() >= 0) {
                            break;
                        }
                    }
                    fn_801069FC(1);
                }
                cmd = -1;
                break;
            default:
                break;
            }
            break;
        }
        case 0xB4:
            __assert((char*)(dat + 0x98), 0x5F5, (char*)&lbl_8047BF1C);
            break;
        case 0xBF: {
            s32 v = fn_80071344();
            switch (v) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                *(s32*)(WORKP + 8) = v;
                if (*(u16*)(fn_80105624() + 4) & 0x400) {
                    cmd = 0xC0;
                } else {
                    cmd = 0xC2;
                }
                break;
            default:
                if (*(s32*)(WORKP + 4) == 2) {
                    cmd = fn_80071398(0xB3);
                } else {
                    cmd = fn_80071398(0xAF);
                }
                break;
            }
            break;
        }
        case 0xC2:
            switch (fn_80071344()) {
            case 0:
                fn_800714C8();
                if (*(s32*)(WORKP + 0x10) != 4) {
                    cmd = 0xB1;
                    break;
                }
                if ((u8)fn_80089028() != 0) {
                    cmd = 0xE4;
                    break;
                }
                cmd = 0xB6;
                break;
            case 1:
            default:
                cmd = fn_80071398(0xB3);
                break;
            }
            break;
        case 0xE4: {
            u32 off;
            s32 mode2;
            u8* arr[4];
            s32 r;
            prevCmd = 0;
            off = prevCmd;
            for (; (u32)prevCmd < 4; prevCmd++) {
                memset(lbl_8047A5A0 + 0x1660 + off, 0, 0xB18);
                fn_8012A248(lbl_8047A5A0 + 0x1660 + off);
                off += 0xB18;
            }
            switch (*(s32*)(WORKP + 4)) {
            case 0:
            case 1:
                if (*(s32*)(WORKP + 0x59AC) == 0) {
                    mode2 = 0;
                    arr[0] = lbl_8047A5A0 + 0x1660;
                    arr[1] = lbl_8047A5A0 + 0x2178;
                    arr[2] = NULL;
                    arr[3] = NULL;
                } else {
                    arr[0] = NULL;
                    arr[1] = lbl_8047A5A0 + 0x1660;
                    arr[2] = lbl_8047A5A0 + 0x2178;
                    arr[3] = NULL;
                    mode2 = 1;
                }
                break;
            case 2:
            default:
                if (*(s32*)(WORKP + 0x59AC) == 0) {
                    mode2 = 2;
                } else {
                    mode2 = 3;
                }
                arr[0] = lbl_8047A5A0 + 0x1660;
                arr[1] = lbl_8047A5A0 + 0x2178;
                arr[2] = lbl_8047A5A0 + 0x2C90;
                arr[3] = lbl_8047A5A0 + 0x37A8;
                break;
            }
            r = fn_800849B4(mode2, 0x1A, arr, NULL);
            if (r < 0) {
                cmd = fn_80071398(0xB3);
                break;
            }
            if (*(s32*)(WORKP + 0x59AC) == 0) {
                fn_8012A774(lbl_8047A5A0 + 0x1660, 0);
            }
            switch (*(s32*)(WORKP + 4)) {
            case 0:
            case 1:
                fn_8006A824(WORKP + 0x59A8, lbl_8047A5A0 + 0x1660);
                fn_8006A824(WORKP + 0x7008, lbl_8047A5A0 + 0x2178);
                if (*(s32*)(WORKP + 0x59AC) == 0) {
                    fn_8006A81C(WORKP + 0x59A8, 1);
                    fn_8006A81C(WORKP + 0x7008, 2);
                } else {
                    fn_8006A81C(WORKP + 0x59A8, 2);
                    fn_8006A81C(WORKP + 0x7008, 3);
                }
                fn_8006A81C(WORKP + 0x8668, 0);
                fn_8006A81C(WORKP + 0x9CC8, 0);
                break;
            case 2:
            default:
                fn_8006A824(WORKP + 0x59A8, lbl_8047A5A0 + 0x1660);
                fn_8006A824(WORKP + 0x7008, lbl_8047A5A0 + 0x2178);
                fn_8006A824(WORKP + 0x8668, lbl_8047A5A0 + 0x2C90);
                fn_8006A824(WORKP + 0x9CC8, lbl_8047A5A0 + 0x37A8);
                fn_8006A81C(WORKP + 0x59A8, 1);
                fn_8006A81C(WORKP + 0x7008, 2);
                fn_8006A81C(WORKP + 0x8668, 3);
                fn_8006A81C(WORKP + 0x9CC8, 4);
                break;
            }
            cmd = 0xB6;
            break;
        }
        case 0xB6: {
            s32 v;
            s32 r;
            {
                u32 i = 0;
                u32 zb;
                prevCmd = 0;
                zb = prevCmd;
                for (; i < 4; i++) {
                    *(u8*)(WORKP + (prevCmd + 0x7005)) = zb;
                    prevCmd += 0x1660;
                }
            }
            v = fn_80071344();
            if (v < 0) {
                s32 e = fn_80071160();
                switch (e) {
                case 0:
                    if ((u8)fn_8008ABA0(1) != 0) {
                        fn_80106D3C(2, 0x4445, 1, 0);
                        while ((u8)fn_800F7EF8(1) == 0) {
                            _threadSwitch();
                        }
                    } else {
                        fn_80106D3C(2, 0x3D55, 1, 0);
                    }
                    break;
                case 1:
                    fn_80106D3C(2, 0x44C0, 1, 0);
                    while ((u8)fn_800F7EF8(1) == 0) {
                        _threadSwitch();
                    }
                    break;
                default:
                    fn_80132A38(0x2F, e);
                    fn_80106D3C(2, 0x44B8, 1, 0);
                    break;
                }
                fn_801069FC(1);
                if ((u8)fn_800F7EF8(1) == 0) {
                    fn_80106D3C(2, (u8)fn_8008ABA0(1) ? 0x4445 : 0x3C4F, 1, 0);
                    while ((u8)fn_800F7EF8(1) == 0) {
                        _threadSwitch();
                    }
                    fn_801069FC(1);
                }
                cmd = fn_80071398(0xB3);
                break;
            }
            r = fn_8010264C(0xD0, 1);
            fn_80102510(0xD0);
            if (r < 0) {
                s32 e = fn_80071160();
                switch (e) {
                case 0:
                    if ((u8)fn_8008ABA0(1) != 0) {
                        fn_80106D3C(2, 0x4445, 1, 0);
                        while ((u8)fn_800F7EF8(1) == 0) {
                            _threadSwitch();
                        }
                    } else {
                        fn_80106D3C(2, 0x3D55, 1, 0);
                    }
                    break;
                case 1:
                    fn_80106D3C(2, 0x44C0, 1, 0);
                    while ((u8)fn_800F7EF8(1) == 0) {
                        _threadSwitch();
                    }
                    break;
                default:
                    fn_80132A38(0x2F, e);
                    fn_80106D3C(2, 0x44B8, 1, 0);
                    break;
                }
                fn_801069FC(1);
                if ((u8)fn_800F7EF8(1) == 0) {
                    fn_80106D3C(2, (u8)fn_8008ABA0(1) ? 0x4445 : 0x3C4F, 1, 0);
                    while ((u8)fn_800F7EF8(1) == 0) {
                        _threadSwitch();
                    }
                    fn_801069FC(1);
                }
                cmd = fn_80071398(0xB3);
                break;
            }
            if (*(s32*)(WORKP + 4) != 2) {
                fn_80069C0C(WORKP);
                cmd = 0xD1;
                break;
            }
            cmd = 0xB5;
            break;
        }
        case 0xB5: {
            s32 v = fn_80071344();
            cmd = 0xD1;
            switch (v) {
            case 0:
            case 1:
            case 2: {
                u8* e = dat + 0x8C;
                s32 t;
                e += v * 4;
                t = (s8)e[0];
                *(s32*)(WORKP + 0x59D0) = t;
                t = (s8)e[1];
                *(s32*)(WORKP + 0x7030) = t;
                t = (s8)e[2];
                *(s32*)(WORKP + 0x8690) = t;
                t = (s8)e[3];
                *(s32*)(WORKP + 0x9CF0) = t;
                fn_80069C0C(WORKP);
                goto b5_done;
            }
            case -1:
            case 3:
            default: {
                s32 e = fn_80071160();
                switch (e) {
                case 0:
                    if ((u8)fn_8008ABA0(1) != 0) {
                        fn_80106D3C(2, 0x4445, 1, 0);
                        while ((u8)fn_800F7EF8(1) == 0) {
                            _threadSwitch();
                        }
                    } else {
                        fn_80106D3C(2, 0x3D55, 1, 0);
                    }
                    break;
                case 1:
                    fn_80106D3C(2, 0x44C0, 1, 0);
                    while ((u8)fn_800F7EF8(1) == 0) {
                        _threadSwitch();
                    }
                    break;
                default:
                    fn_80132A38(0x2F, e);
                    fn_80106D3C(2, 0x44B8, 1, 0);
                    break;
                }
                fn_801069FC(1);
                if ((u8)fn_800F7EF8(1) == 0) {
                    fn_80106D3C(2, (u8)fn_8008ABA0(1) ? 0x4445 : 0x3C4F, 1, 0);
                    while ((u8)fn_800F7EF8(1) == 0) {
                        _threadSwitch();
                    }
                    fn_801069FC(1);
                }
                cmd = fn_80071398(0xB3);
                break;
            }
            }
        b5_done:
            break;
        }
        case 0xD1:
            fn_8006B8FC();
            floorId = 0x397;
            break;
        case 0xB8:
            fn_80129280(0, 0xE);
            fn_80062948();
            cmd = fn_80071398();
            break;
        case 0xB9: {
            s32 r = fn_8010264C(0xB9, 1);
            fn_80102568(0xB9, 0, 0);
            fn_80102568(0xBE, 0, 1);
            switch (r) {
            case 0:
                if ((u8)fn_801D04E8() == 0) {
                    fn_80106D3C(2, 0x44EA, 1, 0);
                    fn_801069FC(1);
                    break;
                }
                if (fn_80135168(0, 4) == 0) {
                    fn_80106D3C(2, 0x44DB, 1, 0);
                    fn_801069FC(1);
                    break;
                }
                fn_8002D91C(0xB);
                break;
            case 1:
                if ((u8)fn_8008ABA0(1) != 0) {
                    fn_80106D3C(2, 0x4445, 1, 0);
                    while ((u8)fn_800F7EF8(1) == 0) {
                        _threadSwitch();
                    }
                    fn_801069FC(1);
                }
                fn_8002D91C(0xC);
                break;
            case -1:
            default:
                cmd = -1;
                break;
            }
            break;
        }
        case 0x105: {
            u32 coins;
            s32 st;
            if ((u8)fn_8006A7BC(WORKP) == 0) {
                coins = fn_8006ADEC();
                cmd = (s32)fn_8006AFC4(WORKP);
                if ((u32)cmd == 0) {
                    __assert((char*)(dat + 0x98), 0x72C, (char*)&lbl_8047BF24);
                }
                if (coins == 0) {
                    goto quit105;
                }
                fn_8006ADB4(0);
                if (fn_8006A7E8((void*)cmd) == 0) {
                    if ((u8)fn_8006A76C(WORKP) != 0) {
                        goto quit105;
                    }
                    prevCmd = coins;
                    fn_801293FC(0, coins);
                    for (;;) {
                        fn_80106D3C(2, 0x3C03, 1, 0);
                        if (menuSubOpenYesNo(0, 0x3C, 0x9E, 0) == 0) {
                            if (fn_80088D84() >= 0) {
                                goto quit105;
                            }
                            continue;
                        }
                        fn_80106D3C(2, 0x3D54, 1, 0);
                        if (menuSubOpenYesNo(0, 0x3C, 0x9E, 1) != 0) {
                            continue;
                        }
                        fn_80129384(0, prevCmd);
                        goto quit105;
                    }
                }
                for (;;) {
                    fn_80106D3C(7, 0x3C23, 1, 0);
                    if (menuSubOpenYesNo(0, 0x3C, 0x9E, 0) != 0) {
                        goto ask105;
                    }
                    {
                        u8* arr2[4];
                        u32 money[0x36];
                        s32 r;
                        u32 h;
                        u8* nm;
                        arr2[0] = NULL;
                        arr2[1] = lbl_8047A5A0 + 0x1660;
                        arr2[2] = NULL;
                        arr2[3] = NULL;
                        r = fn_800849B4(0, 0x40, arr2, money);
                        if (r < 0) {
                            continue;
                        }
                        h = fn_8012AC3C((u8*)cmd + 0xB44);
                        if (h != fn_8012AC3C(lbl_8047A5A0 + 0x1660)) {
                            goto mismatch105;
                        }
                        nm = fn_8012AC54((u8*)cmd + 0xB44);
                        if (GScharCmp(fn_8012AC54(lbl_8047A5A0 + 0x1660), nm) != 0) {
                            goto mismatch105;
                        }
                        fn_80106D3C(7, 0x3D51, 0, 1);
                        money[0] += coins;
                        money[1] += coins;
                        if (money[0] > 9999999) {
                            money[0] = 9999999;
                        }
                        if (money[1] > 9999999) {
                            money[1] = 9999999;
                        }
                        fn_80093574(1);
                        fn_80092C90(1, money, 0);
                        if (fn_80093574(1) == 0xC) {
                            fn_80106D3C(7, 0x3D52, 1, 0);
                            break;
                        }
                        fn_80106D3C(7, 0x3D53, 1, 0);
                        continue;
                    }
                mismatch105:
                    fn_80106D3C(7, 0x44DA, 1, 0);
                    continue;
                ask105:
                    fn_80106D3C(7, 0x3D54, 1, 0);
                    if (menuSubOpenYesNo(0, 0x3C, 0x9E, 1) != 0) {
                        continue;
                    }
                    break;
                }
                if ((u8)fn_8006A76C(fn_80129280(0, 0xE)) == 0) {
                    do {
                        fn_80106D3C(2, 0x44EC, 1, 0);
                        if (menuSubOpenYesNo(0, 0x3C, 0x9E, 0) != 0) {
                            break;
                        }
                    } while (fn_80088C60() < 0);
                }
            quit105:
                fn_801069FC(1);
            }
            st = *(s32*)(WORKP + 0);
            switch (st) {
            case 0:
                cmd = fn_80071398(0xAE);
                break;
            case 1:
            default:
                cmd = fn_80071398(0xAC);
                break;
            }
            break;
        }
        default:
            break;
        }
        if ((u8)fn_800F7EF8(1) == 0 && (u8)fn_8008ABA0(1) == 0) {
            fn_80106D3C(2, 0x3C4F, 1, 0);
            while ((u8)fn_800F7EF8(1) == 0) {
                _threadSwitch();
            }
            fn_801069FC(1);
        }
        prevCmd = first;
        if (cmd < 0) {
            s32 r = fn_800714C8();
            fn_80102568(0xBE, 0, 1);
            if (r < 0) {
                goto done;
            }
        } else if (cmd != fn_8007162C()) {
            s32 r = fn_8007162C();
            if (r == fn_801046B8()) {
                fn_80102568(fn_8007162C(), 0, 0);
            }
            fn_80102568(0xBE, 0, 1);
            fn_800715BC(cmd);
        }
        if (floorId != 0) {
            s32 r = fn_8007162C();
            if (r != fn_801046B8()) {
                goto done;
            }
            fn_80102568(fn_8007162C(), 0, 0);
            fn_80102568(0xBE, 0, 1);
            goto done;
        }
    }
done:
    if (posFlag != 0) {
        fn_8006B8FC();
        floorChangePos(floorId, warpArg, posX, posY, posZ);
    } else {
        if (floorId == 0) {
            floorId = 0x3A1;
        }
        if (floorId == 0x3A1) {
            fn_8006B8F0();
            if ((u8)fn_800F7EF8(1) == 0) {
                fn_80106D3C(2, (u8)fn_8008ABA0(1) ? 0x4445 : 0x3C4F, 1, 0);
                while ((u8)fn_800F7EF8(1) == 0) {
                    _threadSwitch();
                }
                fn_801069FC(1);
            }
        } else {
            fn_8006B8FC();
        }
        fn_80113828(floorId, 0);
    }
#undef WORKP
}
#pragma pop
#endif


/* 0x8005CCD0 | size: 0xB8 */
#if 1
asm void fn_8005CCD0(void) {
#include "src/game/ui/ui_core_fn_8005CCD0.inc"
}
#else
void fn_8005CCD0(void) {
    extern void fn_800E202C();
    extern void fn_800E209C();
    extern void fn_800E24B0();
    extern void fn_80102510();
    extern void fn_80129280();
    extern void fn_8019075C();
    extern void __assert();
    extern void fn_801C40F0();
    extern void fn_801CB9D8();
    extern void fn_8025CD64();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r31 = 0;

    
    r3 = 0x1;
    fn_801C40F0();
    ((void(*)(void))fn_8006B8E8)();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        r31 = 0x0;
        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        *(u8*)((u8*)r3 + 0x1C) = r31;
        r3 = 0x8ae;
        r4 = 0x0;
        fn_8019075C();
    }
    r3 = 0xd3;
    fn_80102510();
    fn_8025CD64();
    r3 = *(u32*)&lbl_8047A5A0;
    r3 = *(u32*)((u8*)r3 + 0x4314);
    fn_801CB9D8();
    r3 = *(u32*)&lbl_8047A5A0;
    fn_800E202C();
    r31 = r3;
    r0 = r31 & 0xFFFF;
    if (r0 == (u32)0x0) {
        r3 = (u32)&lbl_802678D8;
        r4 = 0x252;
        r3 = (u32)&lbl_802678D8;
        r5 = (u32)&lbl_8047BF28;
        __assert();
    }
    r3 = r31;
    fn_800E24B0();
    r3 = r31;
    fn_800E209C();
    r0 = 0x0;
    *(u32*)&lbl_8047A5A0 = r0;
    return;
}
#endif


/* 0x8005CD88 | size: 0x160 */
#if 1
asm void fn_8005CD88(void) {
#include "src/game/ui/ui_core_fn_8005CD88.inc"
}
#else
void fn_8005CD88(void) {
    extern void fn_800E27B0();
    extern void fn_800E2C04();
    extern void fn_800F9318();
    extern void fn_800FF548();
    extern void fn_8010264C();
    extern void fn_80113F48();
    extern void fn_80129280();
    extern void fn_80165A20();
    extern void fn_80176E0C();
    extern void GSscene_SetMode();
    extern void fn_8019075C();
    extern void __assert();
    extern void fn_801CBA0C();
    extern void fn_8025CDB8();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r4 = (u32)&lbl_80267840;
    r3 = 0x1e;
    r31 = (u32)&lbl_80267840;
    r4 = 0x0;
    r5 = 0xff;
    fn_80165A20();
    r3 = 0x8ae;
    r4 = 0x0;
    fn_8019075C();
    r0 = *(u32*)&lbl_8047A5A0;
    if (r0 != (u32)0x0) {
        r3 = r31 + 0x98;
        r5 = r31 + 0x21c;
        r4 = 0x20f;
        __assert();
    }
    r3 = (0x1 << 16);
    r4 = 0x20;
    r3 = r3 + 0xf60;
    fn_800E2C04();
    r30 = r3;
    r0 = r30 & 0xFFFF;
    if (r0 == (u32)0x0) {
        r3 = r31 + 0x98;
        r4 = 0x212;
        r5 = (u32)&lbl_8047BF28;
        __assert();
    }
    r3 = r30;
    fn_800E27B0();
    *(u32*)&lbl_8047A5A0 = r3;
    if (r3 == (u32)0x0) {
        r3 = r31 + 0x98;
        r5 = r31 + 0x22c;
        r4 = 0x213;
        __assert();
    }
    fn_80113F48();
    r4 = (0xffe << 16);
    r30 = r3;
    r3 = r4 + 0x1000;
    fn_801CBA0C();
    r4 = *(u32*)&lbl_8047A5A0;
    *(u32*)((u8*)r4 + 0x4314) = r3;
    r3 = r30;
    r4 = *(u32*)&lbl_8047A5A0;
    r4 = *(u32*)((u8*)r4 + 0x4314);
    fn_800F9318();
    r4 = (0xfff << 16);
    r3 = 0x531;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x1;
    fn_80176E0C();
    r3 = 0x4;
    GSscene_SetMode();
    fn_800FF548();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        fn_8025CDB8();
        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        ((void(*)(void))fn_8006B5D0)();
    }
    ((void(*)(void))fn_8006B8E8)();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        r31 = 0x0;
        r3 = 0x0;
        r4 = 0xe;
        fn_80129280();
        *(u8*)((u8*)r3 + 0x1C) = r31;
    }
    r3 = 0xd3;
    r4 = 0x0;
    fn_8010264C();
    return;
}
#endif


/* 0x8005CEE8 | size: 0x44 */
#if 0
asm void fn_8005CEE8(u32 arg) {
#include "src/game/ui/ui_core_fn_8005CEE8.inc"
}
#else
#pragma optimization_level 4
void fn_8005CEE8(u32 arg) {
    u32* ptr;
    fn_8006B8F0();
    ptr = (u32*)fn_80129280(0, 0xE);
    *ptr = arg;
    fn_800FF58C(0x395);
}
#endif


/* 0x8005CF2C | size: 0x168 */
#if 1
asm void fn_8005CF2C(void) {
#include "src/game/ui/ui_core_fn_8005CF2C.inc"
}
#else
void fn_8005CF2C(void) {
    extern void fn_80102568();
    extern void fn_801026A4();
    extern void fn_80102868();
    extern void fn_80106D3C();
    extern void fn_80166A28();
    extern void __assert();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r30 = r4;
    r3 = 0xbe;
    r4 = 0x0;
    r5 = 0x0;
    r6 = 0x10;
    r7 = 0x0;
    r8 = 0x1;
    r9 = 0xf5;
    /* crclr cr1eq */;
    fn_801026A4();
    ((void(*)(void))fn_8006B420)();
    r0 = 0x0;
    r9 = r31;
    r10 = r30;
    *(u32*)(sp + 0xC) = r0;
    r3 = 0xda;
    r4 = 0x0;
    r5 = 0x0;
    r6 = 0x10;
    r7 = 0x0;
    r8 = 0x4;
    /* crclr cr1eq */;
    fn_801026A4();
    r3 = 0xda;
    r4 = 0x0;
    r5 = -0x28;
    fn_80102868();
    ((void(*)(void))fn_8006B420)();
    r4 = r3;
    r3 = r31;
    ((void(*)(void))fn_80076054)();
    r31 = r3;
    r0 = r31 & 0xFFFF;
    if (r0 == (u32)0x0) {
        r3 = (u32)&lbl_802678D8;
        r5 = (u32)&lbl_80267A20;
        r3 = (u32)&lbl_802678D8;
        r4 = 0x1bb;
        r5 = (u32)&lbl_80267A20;
        __assert();
    }
    r3 = 0x26;
    fn_80166A28();
    r4 = r31 & 0xFFFF;
    r3 = 0x7;
    r5 = 0x1;
    r6 = 0x0;
    fn_80106D3C();
    if ((s32)r30 == (s32)0x0) {
        r3 = 0x7;
        r4 = 0x440a;
        r5 = 0x1;
        r6 = 0x0;
        fn_80106D3C();
    } else {
        r3 = 0x7;
        r4 = 0x3c4e;
        r5 = 0x1;
        r6 = 0x0;
        fn_80106D3C();
    }
    r3 = 0xda;
    r4 = 0x0;
    r5 = 0x0;
    fn_80102568();
    r3 = 0xbe;
    r4 = 0x0;
    r5 = 0x1;
    fn_80102568();
    r3 = 0xda;
    r4 = 0x0;
    r5 = 0x1;
    fn_80102568();
    ((void(*)(void))fn_8006E0CC)();
    return;
}
#endif


/* 0x8005D094 | size: 0x24 | call_return_const */
#pragma push
#pragma scheduling off
#if 0
asm s32 fn_8005D094(void) {
#include "src/game/ui/ui_core_fn_8005D094.inc"
}
#else
#pragma optimization_level 4
s32 fn_8005D094(void) {
    fn_800347B8();
    return -1;
}
#endif

#pragma pop

/* 0x8005D0B8 | size: 0x24 | call_return_const */
#pragma push
#pragma scheduling off
#if 0
asm s32 fn_8005D0B8(void) {
#include "src/game/ui/ui_core_fn_8005D0B8.inc"
}
#else
#pragma optimization_level 4
s32 fn_8005D0B8(void) {
    fn_800347C4();
    return 0;
}
#endif

#pragma pop

/* 0x8005D0DC | size: 0x54 */
#if 0
asm s32 fn_8005D0DC(void) {
#include "src/game/ui/ui_core_fn_8005D0DC.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
s32 fn_8005D0DC(void) {
    s32 result;

    result = fn_8010264C(2, 1);
    fn_80102510(2);
    if (result >= 0) {
        fn_800347E8(result);
    }
    return 0;
}
#pragma pop
#endif



/* 0x8005D130 | size: 0x54 */
#if 0
asm s32 fn_8005D130(void) {
#include "src/game/ui/ui_core_fn_8005D130.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
s32 fn_8005D130(void) {
    s32 result;

    result = fn_8010264C(2, 1);
    fn_80102510(2);
    if (result >= 0) {
        fn_8003480C(result);
    }
    return 0;
}
#pragma pop
#endif



/* 0x8005D184 | size: 0xE8 */
#if 1
asm void fn_8005D184(void) {
#include "src/game/ui/ui_core_fn_8005D184.inc"
}
#else
void fn_8005D184(void) {
    extern void fn_80109220();
    u8 sp[0x20];
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
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r5 = (u32)&lbl_80267A80;
    r11 = (u32)sp + 0x8;
    r10 = (u32)&lbl_80267A80;
    r12 = 0x0;
    r9 = *(u32*)((u8*)r10 + 0x0);
    r8 = *(u32*)((u8*)r10 + 0x4);
    r7 = *(u32*)((u8*)r10 + 0x8);
    r6 = *(u32*)((u8*)r10 + 0xC);
    r5 = *(u32*)((u8*)r10 + 0x10);
    r0 = *(u32*)((u8*)r10 + 0x14);
    r0 = 0x2;
    ctr_fn = (void(*)(void))r0;
    do {
        r6 = 0x0;
        r5 = *(s16*)((u8*)r4 + 0x6);
        r0 = *(u32*)((u8*)r11 + 0x0);
        if ((s32)r5 != (s32)r0) {
            r6 = 0x1;
            r0 = *(u32*)((u8*)r11 + 0x4);
            if ((s32)r5 != (s32)r0) {
                r6 = 0x2;
                r0 = *(u32*)((u8*)r11 + 0x8);
                if ((s32)r5 != (s32)r0) {
                    r6 = 0x3;
                }
            }
        }
        if ((s32)r6 < (s32)0x3) break;
        r11 = r11 + 0xc;
        r12 = r12 + 0x1;
    } while (--ctr != 0);
    if ((s32)r12 >= (s32)0x2) {
        r3 = 0x0;
    } else {

        r0 = *(u8*)((u8*)r3 + 0x95);
        r3 = r4;
        r0 = (s8)r0;
        if ((s32)r0 == (s32)r12) {
            r4 = 0x1;
        } else {

            r4 = 0x0;
        }
        fn_80109220();
        r3 = 0x0;
    }
    return;
}
#endif


/* 0x8005D26C | size: 0x7C */
#if 0
asm void fn_8005D26C(void) {
#include "src/game/ui/ui_core_fn_8005D26C.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
s32 fn_8005D26C(void) {
    s32 r31;
    u32 arr[2];
    arr[0] = *(u32*)&lbl_8047BF30;
    arr[1] = *(u32*)&lbl_8047BF34;
    r31 = fn_8010264C(0x9e, 1);
    fn_80102510(0x9e);
    menuCloseSync(0x9e, 1);
    if (r31 < -1 || r31 >= 2) {
        return 1;
    }
    return arr[r31];
}
#pragma pop
#endif


/* 0x8005D2E8 | size: 0xE8 */
#if 1
asm void fn_8005D2E8(void) {
#include "src/game/ui/ui_core_fn_8005D2E8.inc"
}
#else
void fn_8005D2E8(void) {
    extern void fn_80109220();
    u8 sp[0x20];
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
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r5 = (u32)&lbl_80267A98;
    r11 = (u32)sp + 0x8;
    r10 = (u32)&lbl_80267A98;
    r12 = 0x0;
    r9 = *(u32*)((u8*)r10 + 0x0);
    r8 = *(u32*)((u8*)r10 + 0x4);
    r7 = *(u32*)((u8*)r10 + 0x8);
    r6 = *(u32*)((u8*)r10 + 0xC);
    r5 = *(u32*)((u8*)r10 + 0x10);
    r0 = *(u32*)((u8*)r10 + 0x14);
    r0 = 0x2;
    ctr_fn = (void(*)(void))r0;
    do {
        r6 = 0x0;
        r5 = *(s16*)((u8*)r4 + 0x6);
        r0 = *(u32*)((u8*)r11 + 0x0);
        if ((s32)r5 != (s32)r0) {
            r6 = 0x1;
            r0 = *(u32*)((u8*)r11 + 0x4);
            if ((s32)r5 != (s32)r0) {
                r6 = 0x2;
                r0 = *(u32*)((u8*)r11 + 0x8);
                if ((s32)r5 != (s32)r0) {
                    r6 = 0x3;
                }
            }
        }
        if ((s32)r6 < (s32)0x3) break;
        r11 = r11 + 0xc;
        r12 = r12 + 0x1;
    } while (--ctr != 0);
    if ((s32)r12 >= (s32)0x2) {
        r3 = 0x0;
    } else {

        r0 = *(u8*)((u8*)r3 + 0x95);
        r3 = r4;
        r0 = (s8)r0;
        if ((s32)r0 == (s32)r12) {
            r4 = 0x1;
        } else {

            r4 = 0x0;
        }
        fn_80109220();
        r3 = 0x0;
    }
    return;
}
#endif


/* 0x8005D3D0 | size: 0xDC */
#if 1
asm void fn_8005D3D0(void) {
#include "src/game/ui/ui_core_fn_8005D3D0.inc"
}
#else
void fn_8005D3D0(void) {
    extern void menuCloseSync();
    extern void fn_80102510();
    extern void fn_801026A4();
    extern void fn_801046B8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r31 = 0;

    
    r5 = *(u32*)&lbl_8047BF38;
    r0 = 0x0;
    r4 = *(u32*)&lbl_8047BF3C;
    r6 = (u32)sp + 0xc;
    *(u32*)(sp + 0x8) = r0;
    while ((s32)r4 < (s32)0x2) {

        r0 = *(u32*)((u8*)r6 + 0x0);
        if ((s32)r3 == (s32)r0) break;
        r6 = r6 + 0x4;
        r0 = r4 + 0x1;
        *(u32*)(sp + 0x8) = r0;

    }

    if ((s32)r4 >= (s32)0x2) {
        r0 = 0x0;
        *(u32*)(sp + 0x8) = r0;
    }
    fn_801046B8();
    r4 = r3;
    r5 = (u32)sp + 0x8;
    r3 = 0xa7;
    r6 = 0x0;
    r7 = 0x1;
    r8 = 0x0;
    /* crclr cr1eq */;
    fn_801026A4();
    r31 = r3;
    r3 = 0xa7;
    fn_80102510();
    r3 = 0xa7;
    r4 = 0x1;
    menuCloseSync();
    if ((s32)r31 <= (s32)-0x1) { r3 = 0x1; return; }
    if ((s32)r31 >= (s32)0x2) {

        r3 = 0x1;
        return;
    }
    r0 = r31 << 2;
    r3 = (u32)sp + 0xc;
    r3 = *(u32*)(r3 + r0);

    return;
}
#endif


/* 0x8005D4AC | size: 0x48 */
#if 0
asm s32 fn_8005D4AC(void* a, void* b) {
#include "src/game/ui/ui_core_fn_8005D4AC.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
s32 fn_8005D4AC(void* a, void* b) {
    u8 val = (lbl_8047A5A8 & 0x8) ? 1 : 0;
    fn_80109220(b, val);
    return 0;
}
#pragma pop
#endif


/* 0x8005D4F4 | size: 0x48 */
#if 0
asm s32 fn_8005D4F4(void* a, void* b) {
#include "src/game/ui/ui_core_fn_8005D4F4.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
s32 fn_8005D4F4(void* a, void* b) {
    u8 val = (lbl_8047A5A8 & 0x4) ? 1 : 0;
    fn_80109220(b, val);
    return 0;
}
#pragma pop
#endif


/* 0x8005D53C | size: 0x48 */
#if 0
asm s32 fn_8005D53C(void* a, void* b) {
#include "src/game/ui/ui_core_fn_8005D53C.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
s32 fn_8005D53C(void* a, void* b) {
    u8 val = (lbl_8047A5A8 & 0x2) ? 1 : 0;
    fn_80109220(b, val);
    return 0;
}
#pragma pop
#endif


/* 0x8005D584 | size: 0x48 */
#if 0
asm s32 fn_8005D584(void* a, void* b) {
#include "src/game/ui/ui_core_fn_8005D584.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
s32 fn_8005D584(void* a, void* b) {
    u8 val = (lbl_8047A5A8 & 0x1) ? 1 : 0;
    fn_80109220(b, val);
    return 0;
}
#pragma pop
#endif


/* 0x8005D5CC | size: 0xDC */
#if 1
asm void fn_8005D5CC(void) {
#include "src/game/ui/ui_core_fn_8005D5CC.inc"
}
#else
void fn_8005D5CC(void) {
    extern void fn_80109220();
    u8 sp[0x50];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r5 = (u32)&lbl_80267AB0;
    r0 = 0x9;
    r5 = (u32)&lbl_80267AB0;
    r7 = (u32)sp + 0x4;
    /* subi r6, r5, 0x4 */;
    ctr_fn = (void(*)(void))r0;
    do {
        r5 = *(u32*)((u8*)r6 + 0x4);
        r0 = *(u32*)((u8*)r6 + 0x8);
        *(u32*)((u8*)r7 + 0x4) = r5;
        r7 += 8; *(u32*)r7 = r0;
    } while (--ctr != 0);
    r6 = (u32)sp + 0x8;
    r8 = 0x0;
    r0 = 0x6;
    ctr_fn = (void(*)(void))r0;
    do {
        r7 = 0x0;
        r5 = *(s16*)((u8*)r4 + 0x6);
        r0 = *(u32*)((u8*)r6 + 0x0);
        if ((s32)r5 != (s32)r0) {
            r7 = 0x1;
            r0 = *(u32*)((u8*)r6 + 0x4);
            if ((s32)r5 != (s32)r0) {
                r7 = 0x2;
                r0 = *(u32*)((u8*)r6 + 0x8);
                if ((s32)r5 != (s32)r0) {
                    r7 = 0x3;
                }
            }
        }
        if ((s32)r7 < (s32)0x3) break;
        r6 = r6 + 0xc;
        r8 = r8 + 0x1;
    } while (--ctr != 0);
    if ((s32)r8 >= (s32)0x6) {
        r3 = 0x0;
    } else {

        r0 = *(u8*)((u8*)r3 + 0x95);
        r3 = r4;
        r0 = (s8)r0;
        if ((s32)r0 == (s32)r8) {
            r4 = 0x1;
        } else {

            r4 = 0x0;
        }
        fn_80109220();
        r3 = 0x0;
    }
    return;
}
#endif


/* 0x8005D6A8 | size: 0x90 */
#if 0
asm void fn_8005D6A8(void) {
#include "src/game/ui/ui_core_fn_8005D6A8.inc"
}
#else
void fn_8005D6A8(u8* ctx) {
#pragma peephole off
    u32 table;
    u8 bytes[4];
    u8* result;
    s32 idx;

    result = fn_80105624();
    idx = (s8)ctx[0x95];
    table = *(u32*)&lbl_8047BF40;
    *(u32*)bytes = table;
    if (idx >= 0 && idx <= 3 && (*(u16*)(result + 4) & 0x10) != 0) {
        lbl_8047A5A8 ^= bytes[idx];
        fn_80166A50(0x3C6, 0, 0xFF, 0);
    } else {
        fn_80102ED4(ctx);
    }
}
#endif


/* 0x8005D738 | size: 0x60 */
#if 0
asm void fn_8005D738(void) {
#include "src/game/ui/ui_core_fn_8005D738.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
u8 fn_8005D738(u8 arg) {
    s32 r31;
    lbl_8047A5A8 = arg;
    r31 = fn_8010264C(0x9d, 1);
    fn_80102510(0x9d);
    menuCloseSync(0x9d, 1);
    if (r31 == 0x73d) {
        return lbl_8047A5A8;
    }
    return 0xff;
}
#pragma pop
#endif


/* 0x8005D798 | size: 0x60 */
#if 0
asm void fn_8005D798(void) {
#include "src/game/ui/ui_core_fn_8005D798.inc"
}
#else
/* getter: select one of 5 u16 fields by (sel&0xFF) in 1..5 -> binary-search compare
 * tree; 0 / out-of-range returns 0. byte-match verified via objdiff. */
u16 fn_8005D798(u8 *base, u8 sel) {
    u32 i = sel & 0xFF;
    u16 r = 0;
    switch (i) {
    case 1: r = *(u16 *)(base + 0x4); break;
    case 2: r = *(u16 *)(base + 0x6); break;
    case 3: r = *(u16 *)(base + 0x8); break;
    case 4: r = *(u16 *)(base + 0x0); break;
    case 5: r = *(u16 *)(base + 0x2); break;
    }
    return r;
}
#endif


/* 0x8005D7F8 | size: 0x38 */
#if 0
asm void* fn_8005D7F8(u32 idx) {
#include "src/game/ui/ui_core_fn_8005D7F8.inc"
}
#else
#pragma optimization_level 4
void* fn_8005D7F8(u32 idx) {
    u8* base = *(u8**)&lbl_80478E94;
    if (base == (u8*)0) {
        return (void*)0;
    }
    if (idx >= *(*(u32**)&lbl_80478E90)) {
        return (void*)0;
    }
    return base + idx * 0xA;
}
#endif


/* 0x8005D830 | size: 0x28 */
#if 0
asm void* fn_8005D830(u32 idx) {
#include "src/game/ui/ui_core_fn_8005D830.inc"
}
#else
#pragma optimization_level 4
void* fn_8005D830(u32 idx) {
    if (idx >= *(u32*)&lbl_80478900) {
        return (void*)0;
    }
    return &lbl_802E6428[idx * 0xC];
}
#endif


/* 0x8005D858 | size: 0x28 */
#if 0
asm void* fn_8005D858(u32 idx) {
#include "src/game/ui/ui_core_fn_8005D858.inc"
}
#else
#pragma optimization_level 4
void* fn_8005D858(u32 idx) {
    if (idx >= *(u32*)&lbl_80478908) {
        return (void*)0;
    }
    return &lbl_802E7CE8[idx * 0x18];
}
#endif


/* 0x8005D880 | size: 0x38 */
#if 0
asm void fn_8005D880(u32 idx, u16 val1, u16 val2) {
#include "src/game/ui/ui_core_fn_8005D880.inc"
}
#else
#pragma optimization_level 4
void fn_8005D880(u32 idx, u16 val1, u16 val2) {
    u8* ptr;
    if (idx >= *(u32*)&lbl_80478968) {
        ptr = (u8*)0;
    } else {
        ptr = &lbl_802EF0A8[idx * 0x1C];
    }
    if (ptr == (u8*)0) return;
    *(u16*)(ptr + 0x2) = val1;
    *(u16*)(ptr + 0x4) = val2;
}
#endif


/* 0x8005D8B8 | size: 0x40 */
#if 0
asm s32 fn_8005D8B8(u32 idx) {
#include "src/game/ui/ui_core_fn_8005D8B8.inc"
}
#else
#pragma optimization_level 4
s32 fn_8005D8B8(u32 idx) {
    u8* ptr;
    if (idx >= *(u32*)&lbl_80478968) {
        ptr = (u8*)0;
    } else {
        ptr = &lbl_802EF0A8[idx * 0x1C];
    }
    if (ptr != (u8*)0) {
        return (ptr[0] >> 7) & 1;
    }
    return 0;
}
#endif


/* 0x8005D8F8 | size: 0x3C */
#if 0
asm void fn_8005D8F8(u32 idx, u32 val) {
#include "src/game/ui/ui_core_fn_8005D8F8.inc"
}
#else
#ifndef PCPORT
typedef struct { u8 b7:1; u8 rest:7; } UiBits_8005D8F8;
#endif
#pragma optimization_level 4
void fn_8005D8F8(u32 idx, u32 val) {
#ifdef PCPORT
    u8* ptr;
    if (idx >= *(u32*)&lbl_80478968) {
        ptr = (u8*)0;
    } else {
        ptr = &lbl_802EF0A8[idx * 0x1C];
    }
    if (ptr == (u8*)0) return;
    *ptr = (*ptr & 0x7F) | ((val & 1) << 7);
#else
    UiBits_8005D8F8* ptr;
    if (idx >= *(u32*)&lbl_80478968) {
        ptr = (UiBits_8005D8F8*)0;
    } else {
        ptr = (UiBits_8005D8F8*)&lbl_802EF0A8[idx * 0x1C];
    }
    if (ptr == (UiBits_8005D8F8*)0) return;
    ptr->b7 = (u8)val;
#endif
}
#endif


/* 0x8005D934 | size: 0x28 */
#if 0
asm void* fn_8005D934(u32 idx) {
#include "src/game/ui/ui_core_fn_8005D934.inc"
}
#else
#pragma optimization_level 4
void* fn_8005D934(u32 idx) {
    if (idx >= *(u32*)&lbl_80478968) {
        return (void*)0;
    }
    return &lbl_802EF0A8[idx * 0x1C];
}
#endif


/* 0x8005D95C | size: 0x50 */
#if 0
asm void fn_8005D95C(s32 idx, u16* out1, u16* out2) {
#include "src/game/ui/ui_core_fn_8005D95C.inc"
}
#else
#pragma optimization_level 4
void fn_8005D95C(s32 idx, u16* out1, u16* out2) {
    u8* ptr;
    if (idx < 0) {
        idx = 0;
    }
    if ((u32)idx >= *(u32*)&lbl_80478848) {
        idx = 1;
    }
    ptr = &lbl_802E2DB8[idx * 0x1C];
    if (out1 != (u16*)0) {
        *out1 = *(s16*)(ptr + 0x6);
    }
    if (out2 == (u16*)0) return;
    *out2 = *(s16*)(ptr + 0x8);
}
#endif


/* 0x8005D9AC | size: 0x38 */
#if 0
asm void fn_8005D9AC(s32 idx, u16 val1, u16 val2) {
#include "src/game/ui/ui_core_fn_8005D9AC.inc"
}
#else
#pragma optimization_level 4
void fn_8005D9AC(s32 idx, u16 val1, u16 val2) {
    u8* ptr;
    if (idx < 0) {
        idx = 0;
    }
    if ((u32)idx >= *(u32*)&lbl_80478848) {
        idx = 1;
    }
    ptr = &lbl_802E2DB8[idx * 0x1C];
    *(u16*)(ptr + 0x6) = val1;
    *(u16*)(ptr + 0x8) = val2;
}
#endif


/* 0x8005D9E4 | size: 0x34 */
#if 0
asm u8 fn_8005D9E4(s32 idx) {
#include "src/game/ui/ui_core_fn_8005D9E4.inc"
}
#else
#pragma optimization_level 4
u8 fn_8005D9E4(s32 idx) {
    u8* ptr;
    if (idx < 0) {
        idx = 0;
    }
    if ((u32)idx >= *(u32*)&lbl_80478848) {
        idx = 1;
    }
    ptr = &lbl_802E2DB8[idx * 0x1C];
    return ptr[2];
}
#endif


/* 0x8005DA18 | size: 0x30 */
#if 0
asm void* fn_8005DA18(s32 idx) {
#include "src/game/ui/ui_core_fn_8005DA18.inc"
}
#else
#pragma optimization_level 4
void* fn_8005DA18(s32 idx) {
    if (idx < 0) {
        idx = 0;
    }
    if ((u32)idx >= *(u32*)&lbl_80478848) {
        idx = 1;
    }
    return &lbl_802E2DB8[idx * 0x1C];
}
#endif


/* 0x8005DA48 | size: 0x17C */
#if 1
asm void fn_8005DA48(void) {
#include "src/game/ui/ui_core_fn_8005DA48.inc"
}
#else
void fn_8005DA48(void) {
    extern void fn_800DA1E8();
    extern void fn_800DD270();
    extern void fn_800DD384();
    extern void fn_800FAEF8();
    extern void fn_801040B8();
    extern void fn_801040D0();
    extern void fn_80105624();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r28 = r3;
    r0 = *(u32*)&lbl_8047BF48;
    *(u32*)(sp + 0xC) = r0;
    fn_80105624();
    r0 = *(u16*)((u8*)r3 + 0x4);
    r0 = r0 & 0x00000100;
    if ((s32)r0 != (s32)0x0) {
        r3 = r28;
        r4 = 0x0;
        fn_801040D0();
        r5 = r3 + 0x1;
        r3 = r28;
        r0 = r5 << 30;
        r4 = 0x0;
        r5 = (u32)r5 >> 31;
        r0 = r0 - r5;
        /* rotlwi r0, r0, 2 */;
        r5 = r0 + r5;
        fn_801040B8();
    }
    r3 = r28;
    r4 = 0x0;
    fn_801040D0();
    if ((s32)r3 == (s32)0x1) {
        r28 = 0xa;
        r29 = 0x82;
        r30 = 0x27;
    } else if ((s32)r3 == (s32)0x2) {
        r28 = 0x20;
        r29 = 0x1a0;
        r30 = 0x27;
    } else if ((s32)r3 == (s32)0x3) {
        r28 = 0x1;
        r29 = 0xd;
        r30 = 0x1ba;
    } else {
        r28 = 0xa;
        r29 = 0x82;
        r30 = 0x145;
    }
    r3 = 0x0;
    r4 = 0x7;
    r5 = 0x2;
    fn_800DA1E8();
    /* subi r4, r30, 0x5 */;
    r6 = r29 + 0x12;
    r7 = (u32)sp + 0x8;
    *(u32*)(sp + 0x8) = r0;
    r3 = 0xf;
    r5 = 0x25d;
    ((void(*)(void))fn_8001E58C)();
    r29 = 0x0;
    r31 = (0xc0c1 << 16);
    while ((s32)r29 < (s32)r28) {

        fn_800DD384();
        r0 = r29 + r3;
        r3 = r0 - r28;
        if ((s32)r3 >= (s32)0x0) {
            fn_800DD270();
            r6 = r3;
            r4 = r30;
            /* subi r5, r31, 0x3f01 */;
            r3 = 0x14;
            /* crclr cr1eq */;
            fn_800FAEF8();
        }
        r30 = r30 + 0xd;
        r29 = r29 + 0x1;

    }
    r3 = 0x0;
    return;
}
#endif


/* 0x8005DBC4 | size: 0x60 */
#if 0
asm void fn_8005DBC4(void) {
#include "src/game/ui/ui_core_fn_8005DBC4.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
s32 fn_8005DBC4(void) {
    extern u8 fn_80102620(s32);
    extern void fn_801026A4(s32, s32, s32, s32, s32, s32, ...);
    if (fn_80102620(0xbb)) {
        fn_80102510(0xbb);
    } else {
        fn_801026A4(0xbb, 0, 0, 0, 1, 0);
    }
    return 0;
}
#pragma pop
#endif


/* 0x8005DC24 | size: 0xA0 */
#if 1
asm void fn_8005DC24(void) {
#include "src/game/ui/ui_core_fn_8005DC24.inc"
}
#else
void fn_8005DC24(void) {
    extern void fn_800FF56C();
    extern void fn_80102620();
    extern void fn_801026A4();
    extern void fn_80102868();
    extern void fn_801176C8();
    extern void fn_80117AD4();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r31 = 0;

    
    r3 = 0xca;
    fn_80102620();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        fn_800FF56C();
        r31 = r3;
        fn_80117AD4();
        if (r31 != (u32)r3) {
            r3 = r31;
            fn_801176C8();
            r0 = 0x0;
            *(u32*)&lbl_8047A5B0 = r0;
            *(u32*)&lbl_8047A5B4 = r0;
            *(u32*)&lbl_8047A5B8 = r0;
        }
        r0 = 0x0;
        r3 = 0xca;
        *(u8*)&lbl_8047A5BC = r0;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x1;
        r8 = 0x0;
        /* crclr cr1eq */;
        fn_801026A4();
        r3 = 0xca;
        r4 = 0xc;
        r5 = 0xa;
        fn_80102868();
    }
    r3 = 0x0;
    return;
}
#endif


/* 0x8005DCC4 | size: 0x224 */
#if 1
asm void fn_8005DCC4(void) {
#include "src/game/ui/ui_core_fn_8005DCC4.inc"
}
#else
void fn_8005DCC4(void) {
    extern void fn_80105624();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    fn_80105624();
    r0 = *(u16*)((u8*)r3 + 0x6);
    r0 = r0 & 0x1;
    if ((s32)r0 != (s32)0x0) {
        r4 = *(u8*)((u8*)r31 + 0x95);
        /* subi r4, r4, 0x1 */;
        r0 = (s8)r4;
        *(u8*)((u8*)r31 + 0x95) = r4;
        if ((s32)r0 < (s32)0x0) {
            r0 = 0x0;
            *(u8*)((u8*)r31 + 0x95) = r0;
    }
    }
    r0 = *(u16*)((u8*)r3 + 0x6);
    r0 = r0 & 0x00000002;
    if ((s32)r0 != (s32)0x0) {
        r3 = *(u8*)((u8*)r31 + 0x95);
        r3 = r3 + 0x1;
        r0 = (s8)r3;
        *(u8*)((u8*)r31 + 0x95) = r3;
        if ((s32)r0 > (s32)0x1) {
            r0 = 0x1;
            *(u8*)((u8*)r31 + 0x95) = r0;
    }
    }
    r0 = *(u8*)((u8*)r31 + 0x95);
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 >= (s32)0x1) { r3 = 0x0; return; }
        if ((s32)r0 < (s32)0x0) {
            r3 = 0x0;
            return;
        }
        fn_80105624();
        r0 = *(u16*)((u8*)r3 + 0x6);
        r0 = r0 & 0x00000008;
        if ((s32)r0 != (s32)0x0) {
            r4 = *(u32*)&lbl_8047A5C8;
            r5 = *(u32*)&lbl_80478BD8;
            r0 = r4 + 0x1;
            *(u32*)&lbl_8047A5C8 = r0;
            if ((s32)r0 >= (s32)r5) {
                /* subi r0, r5, 0x1 */;
                *(u32*)&lbl_8047A5C8 = r0;
        }
        }
        r0 = *(u16*)((u8*)r3 + 0x6);
        r0 = r0 & 0x00000400;
        if ((s32)r0 != (s32)0x0) {
            r4 = *(u32*)&lbl_8047A5C8;
            r5 = *(u32*)&lbl_80478BD8;
            r0 = r4 + 0xa;
            *(u32*)&lbl_8047A5C8 = r0;
            if ((s32)r0 >= (s32)r5) {
                /* subi r0, r5, 0x1 */;
                *(u32*)&lbl_8047A5C8 = r0;
        }
        }
        r0 = *(u16*)((u8*)r3 + 0x6);
        r0 = r0 & 0x00000004;
        if ((s32)r0 != (s32)0x0) {
            r4 = *(u32*)&lbl_8047A5C8;
            /* subi r0, r4, 0x1 */;
            *(u32*)&lbl_8047A5C8 = r0;
            if ((s32)r0 < (s32)0x0) {
                r0 = 0x0;
                *(u32*)&lbl_8047A5C8 = r0;
        }
        }
        r0 = *(u16*)((u8*)r3 + 0x6);
        r0 = r0 & 0x00000200;
        if ((s32)r0 == (s32)0x0) { r3 = 0x0; return; }
        r3 = *(u32*)&lbl_8047A5C8;
        /* subi r0, r3, 0xa */;
        *(u32*)&lbl_8047A5C8 = r0;
        if ((s32)r0 >= (s32)0x0) { r3 = 0x0; return; }
        r0 = 0x0;
        *(u32*)&lbl_8047A5C8 = r0;
        r3 = 0x0;
        return;
    }
    fn_80105624();
    r0 = *(u16*)((u8*)r3 + 0x6);
    r0 = r0 & 0x00000008;
    if ((s32)r0 != (s32)0x0) {
        r4 = *(u32*)&lbl_8047A5C4;
        r0 = r4 + 0x1;
        *(u32*)&lbl_8047A5C4 = r0;
        if ((s32)r0 > (s32)0x3e7) {
            r0 = 0x3e7;
            *(u32*)&lbl_8047A5C4 = r0;
    }
    }
    r0 = *(u16*)((u8*)r3 + 0x6);
    r0 = r0 & 0x00000400;
    if ((s32)r0 != (s32)0x0) {
        r4 = *(u32*)&lbl_8047A5C4;
        r0 = r4 + 0xa;
        *(u32*)&lbl_8047A5C4 = r0;
        if ((s32)r0 > (s32)0x3e7) {
            r0 = 0x3e7;
            *(u32*)&lbl_8047A5C4 = r0;
    }
    }
    r0 = *(u16*)((u8*)r3 + 0x6);
    r0 = r0 & 0x00000004;
    if ((s32)r0 != (s32)0x0) {
        r4 = *(u32*)&lbl_8047A5C4;
        /* subi r0, r4, 0x1 */;
        *(u32*)&lbl_8047A5C4 = r0;
        if ((s32)r0 < (s32)0x0) {
            r0 = 0x0;
            *(u32*)&lbl_8047A5C4 = r0;
    }
    }
    r0 = *(u16*)((u8*)r3 + 0x6);
    r0 = r0 & 0x00000200;
    if ((s32)r0 == (s32)0x0) { r3 = 0x0; return; }
    r3 = *(u32*)&lbl_8047A5C4;
    /* subi r0, r3, 0xa */;
    *(u32*)&lbl_8047A5C4 = r0;
    if ((s32)r0 >= (s32)0x0) { r3 = 0x0; return; }
    r0 = 0x0;
    *(u32*)&lbl_8047A5C4 = r0;

    r3 = 0x0;
    return;
}
#endif


/* 0x8005DEE8 | size: 0xE0 */
#if 1
asm void fn_8005DEE8(void) {
#include "src/game/ui/ui_core_fn_8005DEE8.inc"
}
#else
void fn_8005DEE8(void) {
    extern void menuCloseSync();
    extern void fn_80102510();
    extern void fn_80102568();
    extern void fn_8010264C();
    extern void fn_80129A78();
    extern void fn_80142984();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r31 = 0;

    
    r0 = *(u32*)&lbl_8047A5C0;
    if ((s32)r0 == (s32)0x0) {
        r0 = 0x1;
        *(u32*)&lbl_8047A5C8 = r0;
        *(u32*)&lbl_8047A5C0 = r0;
    }
    r0 = 0x1;
    *(u32*)&lbl_8047A5C4 = r0;
    while (1) {
    r3 = 0xcb;
    r4 = 0x1;
    fn_8010264C();
    if ((s32)r3 == (s32)-0x1) break;
    r0 = *(u32*)&lbl_8047A5C8;
    r3 = r0 & 0xFFFF;
    fn_80142984();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) continue;
    r0 = *(u32*)&lbl_8047A5C4;
    if ((s32)r0 < (s32)0x1 || (s32)r0 > (s32)0x3e7) continue;

    r3 = 0x44;
    r4 = 0x1;
    fn_8010264C();
    r31 = r3;
    r3 = 0x44;
    r4 = 0x0;
    r5 = 0x1;
    fn_80102568();
    if ((s32)r31 != (s32)0x0) continue;
    r4 = *(u32*)&lbl_8047A5C8;
    r3 = 0x0;
    r0 = *(u32*)&lbl_8047A5C4;
    r6 = -0x1;
    r4 = r4 & 0xFFFF;
    r5 = r0 & 0xFFFF;
    fn_80129A78();
    }
    r3 = 0xcb;
    fn_80102510();
    r3 = 0xcb;
    r4 = 0x1;
    menuCloseSync();
    return;
}
#endif


/* 0x8005DFC8 | size: 0x6C8 */
#if 1
asm void fn_8005DFC8(void) {
#include "src/game/ui/ui_core_fn_8005DFC8.inc"
}
#else
void fn_8005DFC8(void) {
    extern void fn_80060A28();
    extern u8 fn_80069048(void);
    extern void fn_800D3088();
    extern void fn_800D37CC();
    extern void fn_801080CC();
    extern void fn_801666BC();
    extern void fn_8017B000();
    extern s32 fn_8025DA88();
    extern void _menuCBBattleStartDispTrainerTexCallBack__FlPvl();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    *(f64*)(sp + 0x20) = f31;
    /* psq_st f31, 0x28((u32)sp), 0, qr0 */;
    r30 = r3;
    fn_800D37CC();
    /* xoris r3, r3, 0x8000 */;
    r0 = (0x4330 << 16);
    f1 = *(f64*)&lbl_8047BF80;
    *(u32*)(sp + 0x8) = r0;
    f0 = *(f64*)(sp + 0x8);
    f31 = f0 - f1;
    fn_800D3088();
    r5 = (0x4330 << 16);
    r4 = (u32)&lbl_803A9A60;
    r31 = (u32)&lbl_803A9A60;
    r0 = *(u32*)((u8*)r31 + 0x38);
    f1 = *(f64*)&lbl_8047BF88;
    f0 = *(f64*)(sp + 0x10);
    f0 = f0 - f1;
    f2 = f0 / f31;
    *(f32*)((u8*)r31 + 0x3C) = f2;
    switch ((s32)r0) {
    case 0x0:
        fn_80069048();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x0) break;
        r3 = (u32)&lbl_803A9A60;
        r30 = (u32)&lbl_803A9A60;
        r0 = *(u8*)((u8*)r30 + 0x34);
        if (r0 == (u32)0x0) {
            r0 = 0x0;
            *(u8*)((u8*)r30 + 0x34) = r0;
            *(u32*)((u8*)r30 + 0x2C) = r0;
            fn_8025DA88();
            if ((s32)r3 == (s32)0x2) {
                r3 = (u32)&lbl_803A9A60;
                r0 = 0x4;
                r3 = (u32)&lbl_803A9A60;
                *(u32*)((u8*)r3 + 0x30) = r0;
            } else {

                r3 = (u32)&lbl_803A9A60;
                r0 = 0x2;
                r3 = (u32)&lbl_803A9A60;
                *(u32*)((u8*)r3 + 0x30) = r0;
            }
            r0 = 0x0;
            *(u32*)((u8*)r30 + 0x10) = r0;
            *(u32*)((u8*)r30 + 0xC) = r0;
            *(u32*)((u8*)r30 + 0x18) = r0;
            *(u32*)((u8*)r30 + 0x14) = r0;
            *(u32*)((u8*)r30 + 0x20) = r0;
            *(u32*)((u8*)r30 + 0x1C) = r0;
            *(u32*)((u8*)r30 + 0x28) = r0;
            *(u32*)((u8*)r30 + 0x24) = r0;
            fn_8025DA88();
            if ((s32)r3 == (s32)0x2) {
                r3 = 0x0;
            } else {

                r3 = 0x1;
            }
            r0 = 0x1;
            *(u8*)((u8*)r30 + 0x34) = r0;
            if ((s32)r3 == (s32)0x0) {
                r3 = 0x5c4;
            } else {

                r3 = 0x5c3;
            }
            r5 = (u32)_menuCBBattleStartDispTrainerTexCallBack__FlPvl;
            r4 = 0x0;
            r5 = (u32)_menuCBBattleStartDispTrainerTexCallBack__FlPvl;
            r6 = 0x0;
            r7 = 0x0;
            fn_8017B000();
            r0 = 0x1;
            *(u32*)((u8*)r31 + 0x38) = r0;
            break;
        }
        r0 = 0x2;
        *(u32*)((u8*)r31 + 0x38) = r0;
        break;
    case 0x1:
        fn_80069048();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x0) {
            r3 = (u32)&lbl_803A9A60;
            r3 = (u32)&lbl_803A9A60;
            r0 = *(u8*)((u8*)r3 + 0x34);
            if (r0 != (u32)0x0) {
                r0 = 0x1;
                goto L_8005E1B8;
        }
        }
        r0 = 0x0;
        L_8005E1B8: ;
        r0 = r0 & 0xFF;
        if (r0 != (u32)0x0) {
            r0 = 0x2;
            *(u32*)((u8*)r31 + 0x38) = r0;
        }
        break;
    case 0x2:
        fn_80060A28();
        r3 = (u32)&lbl_803A9A60;
        r3 = (u32)&lbl_803A9A60;
        r5 = r3;
        r0 = 0x4;
        ctr_fn = (void(*)(void))r0;
        do {
            r4 = r5 + 0x58;
            f1 = *(f32*)((u8*)r4 + 0x3C);
            f0 = *(f32*)((u8*)r4 + 0x54);
            if (f1 != f0) {
                r0 = 0x0;
                goto L_8005E358;
            }
            r6 = r4 + 0x4;
            f1 = *(f32*)((u8*)r6 + 0x3C);
            f0 = *(f32*)((u8*)r6 + 0x54);
            if (f1 != f0) {
                r0 = 0x0;
                goto L_8005E358;
            }
            r6 = r4 + 0x8;
            f1 = *(f32*)((u8*)r6 + 0x3C);
            f0 = *(f32*)((u8*)r6 + 0x54);
            if (f1 != f0) {
                r0 = 0x0;
                goto L_8005E358;
            }
            r6 = r4 + 0xc;
            f1 = *(f32*)((u8*)r6 + 0x3C);
            f0 = *(f32*)((u8*)r6 + 0x54);
            if (f1 != f0) {
                r0 = 0x0;
                goto L_8005E358;
            }
            r6 = r4 + 0x10;
            f1 = *(f32*)((u8*)r6 + 0x3C);
            f0 = *(f32*)((u8*)r6 + 0x54);
            if (f1 != f0) {
                r0 = 0x0;
                goto L_8005E358;
            }
            r6 = r4 + 0x14;
            f1 = *(f32*)((u8*)r6 + 0x3C);
            f0 = *(f32*)((u8*)r6 + 0x54);
            if (f1 != f0) {
                r0 = 0x0;
                goto L_8005E358;
            }
            r5 = r5 + 0xb4;
        } while (--ctr != 0);
        r4 = r3 + 0x328;
        f1 = *(f32*)((u8*)r4 + 0x4);
        f0 = *(f32*)((u8*)r4 + 0x8);
        if (f1 != f0) {
            r0 = 0x0;
            goto L_8005E358;
        }
        r5 = r3 + 0xc;
        r4 = r5 + 0x328;
        f1 = *(f32*)((u8*)r4 + 0x4);
        f0 = *(f32*)((u8*)r4 + 0x8);
        if (f1 != f0) {
            r0 = 0x0;
            goto L_8005E358;
        }
        r5 = r5 + 0xc;
        r4 = r5 + 0x328;
        f1 = *(f32*)((u8*)r4 + 0x4);
        f0 = *(f32*)((u8*)r4 + 0x8);
        if (f1 != f0) {
            r0 = 0x0;
            goto L_8005E358;
        }
        r5 = r5 + 0xc;
        r4 = r5 + 0x328;
        f1 = *(f32*)((u8*)r4 + 0x4);
        f0 = *(f32*)((u8*)r4 + 0x8);
        if (f1 != f0) {
            r0 = 0x0;
            goto L_8005E358;
        }
        f1 = *(f32*)((u8*)r3 + 0x48);
        f0 = *(f32*)((u8*)r3 + 0x50);
        if (f1 != f0) {
            r0 = 0x0;
            goto L_8005E358;
        }
        r3 = r3 + 0x4;
        f1 = *(f32*)((u8*)r3 + 0x48);
        f0 = *(f32*)((u8*)r3 + 0x50);
        if (f1 != f0) {
            r0 = 0x0;
            goto L_8005E358;
        }
        r3 = (u32)&lbl_803A9A60;
        r0 = 0x1;
        r3 = (u32)&lbl_803A9A60;
        *(u8*)((u8*)r3 + 0x368) = r0;
        L_8005E358: ;
        r0 = r0 & 0xFF;
        if (r0 == (u32)0x0) break;
        r3 = (u32)&lbl_803A9A60;
        r3 = (u32)&lbl_803A9A60;
        r0 = *(u32*)((u8*)r3 + 0x4);
        if ((s32)r0 == (s32)0x0) {
            f0 = *(f32*)&lbl_8047BF60;
            r0 = 0x3;
            *(u32*)((u8*)r31 + 0x38) = r0;
            *(f32*)((u8*)r3 + 0x3B8) = f0;
            break;
        }
        r0 = 0x4;
        *(u32*)((u8*)r31 + 0x38) = r0;
        break;
    case 0x3:
        f1 = *(f32*)((u8*)r31 + 0x3B8);
        f0 = *(f32*)&lbl_8047BF64;
        f1 = f1 + f2;
        *(f32*)((u8*)r31 + 0x3B8) = f1;
        /* cror eq, gt, eq */;
        if (f1 == f0) {
            r0 = 0xa;
            *(u32*)((u8*)r31 + 0x38) = r0;
        }
        break;
    case 0x4:
        r7 = r31;
        r3 = 0x1;
        r5 = 0x0;
        f1 = *(f32*)&lbl_8047BF68;
        do {
            r6 = r7 + 0x58;
            r4 = 0x0;
            r0 = 0x2;
            ctr_fn = (void(*)(void))r0;
            do {
                f0 = *(f32*)((u8*)r6 + 0x84);
                f3 = *(f32*)((u8*)r6 + 0x6C);
                if (f0 != f3) {
                    f2 = *(f32*)((u8*)r6 + 0x9C);
                    f0 = *(f32*)((u8*)r31 + 0x3C);
                    f0 = f2 * f0;
                    f0 = f0 * f1;
                    f0 = f3 - f0;
                    *(f32*)((u8*)r6 + 0x6C) = f0;
                    f0 = *(f32*)((u8*)r6 + 0x6C);
                    f2 = *(f32*)((u8*)r6 + 0x84);
                    if (f0 < f2) {
                        *(f32*)((u8*)r6 + 0x6C) = f2;
                    }
                    r3 = 0x0;
                }
                r6 = r6 + 0x4;
                f0 = *(f32*)((u8*)r6 + 0x84);
                f3 = *(f32*)((u8*)r6 + 0x6C);
                if (f0 != f3) {
                    f2 = *(f32*)((u8*)r6 + 0x9C);
                    f0 = *(f32*)((u8*)r31 + 0x3C);
                    f0 = f2 * f0;
                    f0 = f0 * f1;
                    f0 = f3 - f0;
                    *(f32*)((u8*)r6 + 0x6C) = f0;
                    f0 = *(f32*)((u8*)r6 + 0x6C);
                    f2 = *(f32*)((u8*)r6 + 0x84);
                    if (f0 < f2) {
                        *(f32*)((u8*)r6 + 0x6C) = f2;
                    }
                    r3 = 0x0;
                }
                r6 = r6 + 0x4;
                f0 = *(f32*)((u8*)r6 + 0x84);
                f3 = *(f32*)((u8*)r6 + 0x6C);
                if (f0 != f3) {
                    f2 = *(f32*)((u8*)r6 + 0x9C);
                    f0 = *(f32*)((u8*)r31 + 0x3C);
                    f0 = f2 * f0;
                    f0 = f0 * f1;
                    f0 = f3 - f0;
                    *(f32*)((u8*)r6 + 0x6C) = f0;
                    f0 = *(f32*)((u8*)r6 + 0x6C);
                    f2 = *(f32*)((u8*)r6 + 0x84);
                    if (f0 < f2) {
                        *(f32*)((u8*)r6 + 0x6C) = f2;
                    }
                    r3 = 0x0;
                }
                r6 = r6 + 0x4;
                r4 = r4 + 0x2;
            } while (--ctr != 0);
            r7 = r7 + 0xb4;
            r5 = r5 + 0x1;
        } while ((s32)r5 < (s32)0x4);
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x0) {
            r0 = 0x5;
            *(u32*)((u8*)r31 + 0x38) = r0;
        }
        break;
    case 0x5:
        f1 = *(f32*)((u8*)r31 + 0x3B4);
        f0 = *(f32*)&lbl_8047BF6C;
        f1 = f1 + f2;
        *(f32*)((u8*)r31 + 0x3B4) = f1;
        /* cror eq, gt, eq */;
        if (f1 == f0) {
            r0 = 0x6;
            *(u32*)((u8*)r31 + 0x38) = r0;
        }
        break;
    case 0x6:
        r3 = r31;
        r0 = 0x2;
        ctr_fn = (void(*)(void))r0;
        do {
            r4 = r3 + 0x358;
            f0 = *(f32*)((u8*)r4 + 0x0);
            f1 = *(f32*)((u8*)r4 + 0x4);
            if (f0 != f1) {
                f1 = f1 - f0;
                f2 = *(f32*)&lbl_8047BF70;
                f0 = *(f32*)((u8*)r31 + 0x3C);
                f1 = f2 * f1;
                f3 = f1 * f0;
                if (f3 > f2) {
                    f3 = f2;
                }
                f0 = *(f32*)&lbl_8047BF74;
                /* cror eq, lt, eq */;
                if (f3 == f0) {
                    f3 = f0;
                }
                f1 = *(f32*)((u8*)r4 + 0x0);
                f0 = *(f32*)&lbl_8047BF60;
                f1 = f1 + f3;
                *(f32*)((u8*)r4 + 0x0) = f1;
                f2 = *(f32*)((u8*)r4 + 0x4);
                f0 = *(f32*)((u8*)r4 + 0x0);
                f1 = f2 - f0;
                if (f3 > f0) {
                } else {

                    f3 = -f3;
                }
                f0 = *(f32*)&lbl_8047BF60;
                if (f1 > f0) {
                } else {

                    f1 = -f1;
                }
                /* cror eq, lt, eq */;
                if (f1 == f3) {
                    *(f32*)((u8*)r4 + 0x0) = f2;
            }
            }
            r3 = r3 + 0x8;
        } while (--ctr != 0);
        r3 = (u32)&lbl_803A9A60;
        f0 = *(f32*)&lbl_8047BF60;
        r3 = (u32)&lbl_803A9A60;
        f2 = *(f32*)((u8*)r3 + 0x358);
        f1 = *(f32*)((u8*)r3 + 0x35C);
        f1 = f2 - f1;
        if (f1 > f0) {
        } else {

            f1 = -f1;
        }
        f0 = *(f32*)&lbl_8047BF78;
        /* cror eq, lt, eq */;
        if (f1 == f0) {
            r3 = (u32)&lbl_803A9A60;
            f0 = *(f32*)&lbl_8047BF60;
            r3 = (u32)&lbl_803A9A60;
            r0 = 0x7;
            *(u32*)((u8*)r31 + 0x38) = r0;
            *(f32*)((u8*)r3 + 0x3B8) = f0;
        }
        break;
    case 0x7:
        f1 = *(f32*)((u8*)r31 + 0x3B8);
        f0 = *(f32*)&lbl_8047BF7C;
        f1 = f1 + f2;
        *(f32*)((u8*)r31 + 0x3B8) = f1;
        /* cror eq, gt, eq */;
        if (f1 == f0) {
            r0 = 0x8;
            *(u32*)((u8*)r31 + 0x38) = r0;
        }
        break;
    case 0x8:
        r0 = 0x9;
        *(u32*)((u8*)r31 + 0x38) = r0;
        break;
    case 0x9:
    case 0x64:
    default:
        break;
    case 0xa:
        r3 = *(u32*)((u8*)r31 + 0x3BC);
        fn_801666BC();
        if ((s32)r3 == (s32)0x0) {
            r0 = 0xb;
            *(u32*)((u8*)r31 + 0x38) = r0;
        }
        break;
    case 0xb:
        r0 = 0x64;
        r4 = 0x1c6;
        *(u32*)((u8*)r31 + 0x38) = r0;
        r3 = *(u32*)((u8*)r30 + 0x4);
        fn_801080CC();
        r0 = 0x1;
        *(u8*)((u8*)r30 + 0x2) = r0;
        break;
    }
    /* psq_l f31, 0x28((u32)sp), 0, qr0 */;
    f31 = *(f64*)(sp + 0x20);
    return;
}
#endif


/* 0x8005E690 | size: 0xA0 */
#if 1
asm void fn_8005E690(void) {
#include "src/game/ui/ui_core_fn_8005E690.inc"
}
#else
void fn_8005E690(void) {
    extern void fn_80102ED4();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r30 = r3;
    r4 = (u32)&lbl_803A9A60;
    r31 = (u32)&lbl_803A9A60;
    r0 = *(u32*)((u8*)r31 + 0x4);
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 >= (s32)0x1) return;
        if ((s32)r0 < (s32)0x0) {
            return;
        }
        r0 = *(u32*)((u8*)r31 + 0x38);
        if ((s32)r0 >= (s32)0x3) {
            fn_80102ED4();
        }
        r0 = *(u32*)((u8*)r31 + 0x38);
        if ((s32)r0 != (s32)0x64) return;
        r0 = 0x1;
        *(u8*)((u8*)r30 + 0x98) = r0;
        return;
    }
    r0 = *(u32*)((u8*)r31 + 0x38);
    if ((s32)r0 >= (s32)0x7) {
        fn_80102ED4();
    }
    r0 = *(u32*)((u8*)r31 + 0x38);
    if ((s32)r0 != (s32)0x9) return;
    r0 = 0x1;
    *(u8*)((u8*)r30 + 0x98) = r0;

    return;
}
#endif


/* 0x8005E730 | size: 0x20 */
#if 0
asm void fn_8005E730(void) {
#include "src/game/ui/ui_core_fn_8005E730.inc"
}
#else
#pragma optimization_level 4
void fn_8005E730(void) {
    fn_8005DFC8();
}
#endif

/* 0x8005E750 | size: 0xA0 */
#if 1
asm void fn_8005E750(void) {
#include "src/game/ui/ui_core_fn_8005E750.inc"
}
#else
void fn_8005E750(void) {
    extern void fn_80061F6C();
    extern void fn_80102568();
    extern void fn_8010264C();
    extern void fn_80103CC0();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r4 = 0x0;
    fn_80061F6C();
    r3 = (u32)&lbl_803A9A60;
    r0 = 0xba;
    r4 = (u32)&lbl_803A9A60;
    r3 = 0x0;
    *(u16*)((u8*)r4 + 0x8) = r0;
    fn_80103CC0();
    r3 = 0xdf;
    r4 = 0x0;
    fn_8010264C();
    r3 = 0xba;
    r4 = 0x1;
    fn_8010264C();
    r3 = 0x1;
    fn_80103CC0();
    r3 = 0xba;
    r4 = 0x0;
    r5 = 0x1;
    fn_80102568();
    r3 = (u32)&lbl_803A9A60;
    r0 = 0x0;
    r3 = (u32)&lbl_803A9A60;
    *(u32*)((u8*)r3 + 0x4) = r0;
    r0 = *(u32*)((u8*)r31 + 0x4);
    if ((s32)r0 != (s32)0x2) {
        r3 = 0xc4;
    } else {

        r3 = 0xc6;
    }
    return;
}
#endif


/* 0x8005E7F0 | size: 0x17F4 */
#if 0
asm void fn_8005E7F0(void) {
#include "src/game/ui/ui_core_fn_8005E7F0.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
void fn_8005E7F0(void* ctx, void* arg1)
{
    UICmdMsg* msg = (UICmdMsg*) arg1;
    u32* tbl = (u32*) lbl_803A9E40;

    switch (msg->cmd) {
    case 0x8A6:
        fn_800608C4(ctx, msg);
        break;
    case 0xC01:
        fn_8006106C(ctx, msg, 0, tbl[0], 2);
        break;
    case 0xC02:
        fn_8006106C(ctx, msg, 0, tbl[1], 2);
        break;
    case 0xC03:
        fn_8006106C(ctx, msg, 0, tbl[2], 2);
        break;
    case 0xC04:
        fn_8006106C(ctx, msg, 0, tbl[3], 2);
        break;
    case 0xC05:
        fn_8006106C(ctx, msg, 0, tbl[4], 2);
        break;
    case 0xC06:
        fn_8006106C(ctx, msg, 0, tbl[5], 2);
        break;
    case 0xC0D:
        msg->flags4 &= ~2;
        break;
    case 0xC0E:
        msg->flags4 &= ~2;
        break;
    case 0xC0F:
        msg->flags4 &= ~2;
        break;
    case 0xC10:
        msg->flags4 &= ~2;
        break;
    case 0xC11:
        msg->flags4 &= ~2;
        break;
    case 0xC12:
        msg->flags4 &= ~2;
        break;
    case 0xC13:
        fn_80061A2C(ctx, msg, 0, tbl[0], 2);
        break;
    case 0xC14:
        fn_80061A2C(ctx, msg, 0, tbl[1], 2);
        break;
    case 0xC15:
        fn_80061A2C(ctx, msg, 0, tbl[2], 2);
        break;
    case 0xC16:
        fn_80061A2C(ctx, msg, 0, tbl[3], 2);
        break;
    case 0xC17:
        fn_80061A2C(ctx, msg, 0, tbl[4], 2);
        break;
    case 0xC18:
        fn_80061A2C(ctx, msg, 0, tbl[5], 2);
        break;
    case 0xC19:
        fn_80061BBC(ctx, msg, 0, tbl[0], 2);
        break;
    case 0xC1A:
        fn_80061BBC(ctx, msg, 0, tbl[1], 2);
        break;
    case 0xC1B:
        fn_80061BBC(ctx, msg, 0, tbl[2], 2);
        break;
    case 0xC1C:
        fn_80061BBC(ctx, msg, 0, tbl[3], 2);
        break;
    case 0xC1D:
        fn_80061BBC(ctx, msg, 0, tbl[4], 2);
        break;
    case 0xC1E:
        fn_80061BBC(ctx, msg, 0, tbl[5], 2);
        break;
    case 0xDB5:
        fn_80061B74(ctx, msg, 0, tbl[0], 2);
        break;
    case 0xC1F:
        fn_80061B74(ctx, msg, 0, tbl[1], 2);
        break;
    case 0xC20:
        fn_80061B74(ctx, msg, 0, tbl[2], 2);
        break;
    case 0xC21:
        fn_80061B74(ctx, msg, 0, tbl[3], 2);
        break;
    case 0xC22:
        fn_80061B74(ctx, msg, 0, tbl[4], 2);
        break;
    case 0xC23:
        fn_80061B74(ctx, msg, 0, tbl[5], 2);
        break;
    case 0xC27:
        fn_8006106C(ctx, msg, 1, tbl[6], 2);
        break;
    case 0xC28:
        fn_8006106C(ctx, msg, 1, tbl[7], 2);
        break;
    case 0xC29:
        fn_8006106C(ctx, msg, 1, tbl[8], 2);
        break;
    case 0xC2A:
        fn_8006106C(ctx, msg, 1, tbl[9], 2);
        break;
    case 0xC2B:
        fn_8006106C(ctx, msg, 1, tbl[10], 2);
        break;
    case 0xC2C:
        fn_8006106C(ctx, msg, 1, tbl[11], 2);
        break;
    case 0xC33:
        msg->flags4 &= ~2;
        break;
    case 0xC34:
        msg->flags4 &= ~2;
        break;
    case 0xC35:
        msg->flags4 &= ~2;
        break;
    case 0xC36:
        msg->flags4 &= ~2;
        break;
    case 0xC37:
        msg->flags4 &= ~2;
        break;
    case 0xC38:
        msg->flags4 &= ~2;
        break;
    case 0xC39:
        fn_80061A2C(ctx, msg, 1, tbl[6], 2);
        break;
    case 0xC3A:
        fn_80061A2C(ctx, msg, 1, tbl[7], 2);
        break;
    case 0xC3B:
        fn_80061A2C(ctx, msg, 1, tbl[8], 2);
        break;
    case 0xC3C:
        fn_80061A2C(ctx, msg, 1, tbl[9], 2);
        break;
    case 0xC3D:
        fn_80061A2C(ctx, msg, 1, tbl[10], 2);
        break;
    case 0xC3E:
        fn_80061A2C(ctx, msg, 1, tbl[11], 2);
        break;
    case 0xC3F:
        fn_80061BBC(ctx, msg, 1, tbl[6], 2);
        break;
    case 0xC40:
        fn_80061BBC(ctx, msg, 1, tbl[7], 2);
        break;
    case 0xC41:
        fn_80061BBC(ctx, msg, 1, tbl[8], 2);
        break;
    case 0xC42:
        fn_80061BBC(ctx, msg, 1, tbl[9], 2);
        break;
    case 0xC43:
        fn_80061BBC(ctx, msg, 1, tbl[10], 2);
        break;
    case 0xC44:
        fn_80061BBC(ctx, msg, 1, tbl[11], 2);
        break;
    case 0xDB4:
        fn_80061B74(ctx, msg, 1, tbl[6], 2);
        break;
    case 0xC45:
        fn_80061B74(ctx, msg, 1, tbl[7], 2);
        break;
    case 0xC46:
        fn_80061B74(ctx, msg, 1, tbl[8], 2);
        break;
    case 0xC47:
        fn_80061B74(ctx, msg, 1, tbl[9], 2);
        break;
    case 0xC48:
        fn_80061B74(ctx, msg, 1, tbl[10], 2);
        break;
    case 0xC49:
        fn_80061B74(ctx, msg, 1, tbl[11], 2);
        break;
    case 0xC4D:
        fn_8006106C(ctx, msg, 2, tbl[12], 2);
        break;
    case 0xC4E:
        fn_8006106C(ctx, msg, 2, tbl[13], 2);
        break;
    case 0xC4F:
        fn_8006106C(ctx, msg, 2, tbl[14], 2);
        break;
    case 0xC50:
        fn_8006106C(ctx, msg, 2, tbl[15], 2);
        break;
    case 0xC51:
        fn_8006106C(ctx, msg, 2, tbl[16], 2);
        break;
    case 0xC52:
        fn_8006106C(ctx, msg, 2, tbl[17], 2);
        break;
    case 0xC59:
        msg->flags4 &= ~2;
        break;
    case 0xC5A:
        msg->flags4 &= ~2;
        break;
    case 0xC5B:
        msg->flags4 &= ~2;
        break;
    case 0xC5C:
        msg->flags4 &= ~2;
        break;
    case 0xC5D:
        msg->flags4 &= ~2;
        break;
    case 0xC5E:
        msg->flags4 &= ~2;
        break;
    case 0xC5F:
        fn_80061A2C(ctx, msg, 2, tbl[12], 2);
        break;
    case 0xC60:
        fn_80061A2C(ctx, msg, 2, tbl[13], 2);
        break;
    case 0xC61:
        fn_80061A2C(ctx, msg, 2, tbl[14], 2);
        break;
    case 0xC62:
        fn_80061A2C(ctx, msg, 2, tbl[15], 2);
        break;
    case 0xC63:
        fn_80061A2C(ctx, msg, 2, tbl[16], 2);
        break;
    case 0xC64:
        fn_80061A2C(ctx, msg, 2, tbl[17], 2);
        break;
    case 0xC65:
        fn_80061BBC(ctx, msg, 2, tbl[12], 2);
        break;
    case 0xC66:
        fn_80061BBC(ctx, msg, 2, tbl[13], 2);
        break;
    case 0xC67:
        fn_80061BBC(ctx, msg, 2, tbl[14], 2);
        break;
    case 0xC68:
        fn_80061BBC(ctx, msg, 2, tbl[15], 2);
        break;
    case 0xC69:
        fn_80061BBC(ctx, msg, 2, tbl[16], 2);
        break;
    case 0xC6A:
        fn_80061BBC(ctx, msg, 2, tbl[17], 2);
        break;
    case 0xDAF:
        fn_80061B74(ctx, msg, 2, tbl[12], 2);
        break;
    case 0xC6B:
        fn_80061B74(ctx, msg, 2, tbl[13], 2);
        break;
    case 0xC6C:
        fn_80061B74(ctx, msg, 2, tbl[14], 2);
        break;
    case 0xC6D:
        fn_80061B74(ctx, msg, 2, tbl[15], 2);
        break;
    case 0xC6E:
        fn_80061B74(ctx, msg, 2, tbl[16], 2);
        break;
    case 0xC6F:
        fn_80061B74(ctx, msg, 2, tbl[17], 2);
        break;
    case 0xC73:
        fn_8006106C(ctx, msg, 3, tbl[18], 2);
        break;
    case 0xC74:
        fn_8006106C(ctx, msg, 3, tbl[19], 2);
        break;
    case 0xC75:
        fn_8006106C(ctx, msg, 3, tbl[20], 2);
        break;
    case 0xC76:
        fn_8006106C(ctx, msg, 3, tbl[21], 2);
        break;
    case 0xC77:
        fn_8006106C(ctx, msg, 3, tbl[22], 2);
        break;
    case 0xC78:
        fn_8006106C(ctx, msg, 3, tbl[23], 2);
        break;
    case 0xC7F:
        msg->flags4 &= ~2;
        break;
    case 0xC80:
        msg->flags4 &= ~2;
        break;
    case 0xC81:
        msg->flags4 &= ~2;
        break;
    case 0xC82:
        msg->flags4 &= ~2;
        break;
    case 0xC83:
        msg->flags4 &= ~2;
        break;
    case 0xC84:
        msg->flags4 &= ~2;
        break;
    case 0xC85:
        fn_80061A2C(ctx, msg, 3, tbl[18], 2);
        break;
    case 0xC86:
        fn_80061A2C(ctx, msg, 3, tbl[19], 2);
        break;
    case 0xC87:
        fn_80061A2C(ctx, msg, 3, tbl[20], 2);
        break;
    case 0xC88:
        fn_80061A2C(ctx, msg, 3, tbl[21], 2);
        break;
    case 0xC89:
        fn_80061A2C(ctx, msg, 3, tbl[22], 2);
        break;
    case 0xC8A:
        fn_80061A2C(ctx, msg, 3, tbl[23], 2);
        break;
    case 0xC8B:
        fn_80061BBC(ctx, msg, 3, tbl[18], 2);
        break;
    case 0xC8C:
        fn_80061BBC(ctx, msg, 3, tbl[19], 2);
        break;
    case 0xC8D:
        fn_80061BBC(ctx, msg, 3, tbl[20], 2);
        break;
    case 0xC8E:
        fn_80061BBC(ctx, msg, 3, tbl[21], 2);
        break;
    case 0xC8F:
        fn_80061BBC(ctx, msg, 3, tbl[22], 2);
        break;
    case 0xC90:
        fn_80061BBC(ctx, msg, 3, tbl[23], 2);
        break;
    case 0xDB3:
        fn_80061B74(ctx, msg, 3, tbl[18], 2);
        break;
    case 0xC91:
        fn_80061B74(ctx, msg, 3, tbl[19], 2);
        break;
    case 0xC92:
        fn_80061B74(ctx, msg, 3, tbl[20], 2);
        break;
    case 0xC93:
        fn_80061B74(ctx, msg, 3, tbl[21], 2);
        break;
    case 0xC94:
        fn_80061B74(ctx, msg, 3, tbl[22], 2);
        break;
    case 0xC95:
        fn_80061B74(ctx, msg, 3, tbl[23], 2);
        break;
    case 0xDB6:
        fn_8006106C(ctx, msg, 0, tbl[0], 0);
        break;
    case 0xDB7:
        fn_8006106C(ctx, msg, 0, tbl[1], 0);
        break;
    case 0xDB8:
        fn_8006106C(ctx, msg, 0, tbl[2], 0);
        break;
    case 0xDB9:
        msg->flags4 &= ~2;
        break;
    case 0xDBA:
        msg->flags4 &= ~2;
        break;
    case 0xDBB:
        msg->flags4 &= ~2;
        break;
    case 0xDBC:
        fn_80061A2C(ctx, msg, 0, tbl[0], 0);
        break;
    case 0xDBD:
        fn_80061A2C(ctx, msg, 0, tbl[1], 0);
        break;
    case 0xDBE:
        fn_80061A2C(ctx, msg, 0, tbl[2], 0);
        break;
    case 0xDBF:
        fn_80061BBC(ctx, msg, 0, tbl[0], 0);
        break;
    case 0xDC0:
        fn_80061BBC(ctx, msg, 0, tbl[1], 0);
        break;
    case 0xDC1:
        fn_80061BBC(ctx, msg, 0, tbl[2], 0);
        break;
    case 0xDC2:
        fn_8006106C(ctx, msg, 1, tbl[6], 0);
        break;
    case 0xDC3:
        fn_8006106C(ctx, msg, 1, tbl[7], 0);
        break;
    case 0xDC4:
        fn_8006106C(ctx, msg, 1, tbl[8], 0);
        break;
    case 0xDC5:
        msg->flags4 &= ~2;
        break;
    case 0xDC6:
        msg->flags4 &= ~2;
        break;
    case 0xDC7:
        msg->flags4 &= ~2;
        break;
    case 0xDC8:
        fn_80061A2C(ctx, msg, 1, tbl[6], 0);
        break;
    case 0xDC9:
        fn_80061A2C(ctx, msg, 1, tbl[7], 0);
        break;
    case 0xDCA:
        fn_80061A2C(ctx, msg, 1, tbl[8], 0);
        break;
    case 0xDCB:
        fn_80061BBC(ctx, msg, 1, tbl[6], 0);
        break;
    case 0xDCC:
        fn_80061BBC(ctx, msg, 1, tbl[7], 0);
        break;
    case 0xDCD:
        fn_80061BBC(ctx, msg, 1, tbl[8], 0);
        break;
    case 0xDD8:
        fn_8006106C(ctx, msg, 0, tbl[0], 1);
        break;
    case 0xDD9:
        fn_8006106C(ctx, msg, 0, tbl[1], 1);
        break;
    case 0xDDA:
        fn_8006106C(ctx, msg, 0, tbl[2], 1);
        break;
    case 0xDE4:
        fn_8006106C(ctx, msg, 0, tbl[3], 1);
        break;
    case 0xDE5:
        fn_8006106C(ctx, msg, 0, tbl[4], 1);
        break;
    case 0xDE6:
        fn_8006106C(ctx, msg, 0, tbl[5], 1);
        break;
    case 0xDDB:
        msg->flags4 &= ~2;
        break;
    case 0xDDC:
        msg->flags4 &= ~2;
        break;
    case 0xDDD:
        msg->flags4 &= ~2;
        break;
    case 0xDE7:
        msg->flags4 &= ~2;
        break;
    case 0xDE8:
        msg->flags4 &= ~2;
        break;
    case 0xDE9:
        msg->flags4 &= ~2;
        break;
    case 0xDDE:
        fn_80061A2C(ctx, msg, 0, tbl[0], 1);
        break;
    case 0xDDF:
        fn_80061A2C(ctx, msg, 0, tbl[1], 1);
        break;
    case 0xDE0:
        fn_80061A2C(ctx, msg, 0, tbl[2], 1);
        break;
    case 0xDEA:
        fn_80061A2C(ctx, msg, 0, tbl[3], 1);
        break;
    case 0xDEB:
        fn_80061A2C(ctx, msg, 0, tbl[4], 1);
        break;
    case 0xDEC:
        fn_80061A2C(ctx, msg, 0, tbl[5], 1);
        break;
    case 0xDE1:
        fn_80061BBC(ctx, msg, 0, tbl[0], 1);
        break;
    case 0xDE2:
        fn_80061BBC(ctx, msg, 0, tbl[1], 1);
        break;
    case 0xDE3:
        fn_80061BBC(ctx, msg, 0, tbl[2], 1);
        break;
    case 0xDED:
        fn_80061BBC(ctx, msg, 0, tbl[3], 1);
        break;
    case 0xDEE:
        fn_80061BBC(ctx, msg, 0, tbl[4], 1);
        break;
    case 0xDEF:
        fn_80061BBC(ctx, msg, 0, tbl[5], 1);
        break;
    case 0xDF3:
        msg->flags4 &= ~2;
        break;
    case 0xDF4:
        msg->flags4 &= ~2;
        break;
    case 0xDF5:
        msg->flags4 &= ~2;
        break;
    case 0xDFF:
        msg->flags4 &= ~2;
        break;
    case 0xE00:
        msg->flags4 &= ~2;
        break;
    case 0xE01:
        msg->flags4 &= ~2;
        break;
    case 0xDF6:
        fn_80061A2C(ctx, msg, 1, tbl[6], 1);
        break;
    case 0xDF7:
        fn_80061A2C(ctx, msg, 1, tbl[7], 1);
        break;
    case 0xDF8:
        fn_80061A2C(ctx, msg, 1, tbl[8], 1);
        break;
    case 0xE02:
        fn_80061A2C(ctx, msg, 1, tbl[9], 1);
        break;
    case 0xE03:
        fn_80061A2C(ctx, msg, 1, tbl[10], 1);
        break;
    case 0xE04:
        fn_80061A2C(ctx, msg, 1, tbl[11], 1);
        break;
    case 0xDF0:
        fn_8006106C(ctx, msg, 1, tbl[6], 1);
        break;
    case 0xDF1:
        fn_8006106C(ctx, msg, 1, tbl[7], 1);
        break;
    case 0xDF2:
        fn_8006106C(ctx, msg, 1, tbl[8], 1);
        break;
    case 0xDFC:
        fn_8006106C(ctx, msg, 1, tbl[9], 1);
        break;
    case 0xDFD:
        fn_8006106C(ctx, msg, 1, tbl[10], 1);
        break;
    case 0xDFE:
        fn_8006106C(ctx, msg, 1, tbl[11], 1);
        break;
    case 0xDF9:
        fn_80061BBC(ctx, msg, 1, tbl[6], 1);
        break;
    case 0xDFA:
        fn_80061BBC(ctx, msg, 1, tbl[7], 1);
        break;
    case 0xDFB:
        fn_80061BBC(ctx, msg, 1, tbl[8], 1);
        break;
    case 0xE05:
        fn_80061BBC(ctx, msg, 1, tbl[9], 1);
        break;
    case 0xE06:
        fn_80061BBC(ctx, msg, 1, tbl[10], 1);
        break;
    case 0xE07:
        fn_80061BBC(ctx, msg, 1, tbl[11], 1);
        break;
    case 0xDD4:
        fn_80060D70(ctx, msg, 0, 0);
        break;
    case 0xDD5:
        fn_80060D70(ctx, msg, 0, 1);
        break;
    case 0xDD6:
        fn_80060D70(ctx, msg, 1, 0);
        break;
    case 0xDD7:
        fn_80060D70(ctx, msg, 1, 1);
        break;
    case 0x102C:
        fn_80060D70(ctx, msg, 1, 2);
        break;
    case 0x102D:
        fn_80060D70(ctx, msg, 1, 2);
        break;
    case 0xBF1:
    case 0xBF2:
        fn_80060EF4(ctx, msg, 6);
        break;
    case 0xBF3:
    case 0xBF4:
        fn_80060EF4(ctx, msg, 6);
        break;
    case 0xBF5:
        fn_80060EF4(ctx, msg, -1);
        break;
    case 0xBF6:
    case 0xBF7:
        fn_80060EF4(ctx, msg, 3);
        break;
    case 0xBF8:
    case 0xBF9:
        fn_80060EF4(ctx, msg, 4);
        break;
    case 0xBFA:
    case 0xBFB:
        fn_80060EF4(ctx, msg, 2);
        break;
    case 0xBFC:
    case 0xBFD:
        fn_80060EF4(ctx, msg, 1);
        break;
    case 0xBFE:
        fn_80060EF4(ctx, msg, 0);
        break;
    case 0xDD0:
        fn_800617E0(ctx, msg, 0, 0);
        break;
    case 0xDD1:
        fn_800617E0(ctx, msg, 1, 0);
        break;
    case 0xDCF:
        fn_800615F4(ctx, msg, 0, 0);
        break;
    case 0xDCE:
        fn_800615F4(ctx, msg, 1, 0);
        break;
    case 0xDD3:
        fn_80061454(ctx, msg, 0, 0);
        break;
    case 0xDD2:
        fn_80061454(ctx, msg, 1, 0);
        break;
    case 0xC26:
        fn_800617E0(ctx, msg, 0, 2);
        break;
    case 0xC4C:
        fn_800617E0(ctx, msg, 1, 2);
        break;
    case 0xC72:
        fn_800617E0(ctx, msg, 2, 2);
        break;
    case 0xC98:
        fn_800617E0(ctx, msg, 3, 2);
        break;
    case 0xC24:
        fn_80061454(ctx, msg, 0, 2);
        break;
    case 0xC4A:
        fn_80061454(ctx, msg, 1, 2);
        break;
    case 0xC70:
        fn_80061454(ctx, msg, 2, 2);
        break;
    case 0xC96:
        fn_80061454(ctx, msg, 3, 2);
        break;
    case 0xC25:
        fn_800615F4(ctx, msg, 0, 2);
        break;
    case 0xC4B:
        fn_800615F4(ctx, msg, 1, 2);
        break;
    case 0xC71:
        fn_800615F4(ctx, msg, 2, 2);
        break;
    case 0xC97:
        fn_800615F4(ctx, msg, 3, 2);
        break;
    case 0xBFF:
        fn_800609B4(ctx, msg, *(f32*) (lbl_803A9A60 + 0x48));
        break;
    case 0xC00:
        fn_800609B4(ctx, msg, *(f32*) (lbl_803A9A60 + 0x4c));
        break;
    case 0x1096:
        fn_80060434(ctx, msg);
        break;
    }
}
#pragma pop
#endif


/* 0x8005FFE4 | size: 0x450 */
#if 1
asm void fn_8005FFE4(void) {
#include "src/game/ui/ui_core_fn_8005FFE4.inc"
}
#else
void fn_8005FFE4(void) {
    extern void fn_8025D808();
    extern s32 fn_8025D89C();
    extern s32 fn_8025DA88();
    u8 sp[0x140];
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
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    fn_8025DA88();
    r5 = *(u32*)&lbl_8047BF58;
    r4 = (u32)&lbl_803A9E40;
    r0 = *(u32*)&lbl_8047BF5C;
    r31 = r3;
    r7 = (u32)&lbl_803A9E40;
    *(u32*)(sp + 0xC) = r0;
    r29 = 0x0;
    r6 = 0x1;
    r5 = 0x2;
    r4 = 0x3;
    r3 = 0x4;
    r0 = 0x5;
    *(u32*)((u8*)r7 + 0x0) = r29;
    r8 = r7 + 0x18;
    *(u32*)((u8*)r7 + 0x4) = r6;
    *(u32*)((u8*)r7 + 0x8) = r5;
    *(u32*)((u8*)r7 + 0xC) = r4;
    *(u32*)((u8*)r7 + 0x10) = r3;
    *(u32*)((u8*)r7 + 0x14) = r0;
    *(u32*)((u8*)r8 + 0x0) = r29;
    *(u32*)((u8*)r8 + 0x4) = r6;
    *(u32*)((u8*)r8 + 0x8) = r5;
    *(u32*)((u8*)r8 + 0xC) = r4;
    *(u32*)((u8*)r8 + 0x10) = r3;
    *(u32*)((u8*)r8 + 0x14) = r0;
    r8 = r8 + 0x18;
    *(u32*)((u8*)r8 + 0x0) = r29;
    *(u32*)((u8*)r8 + 0x4) = r6;
    *(u32*)((u8*)r8 + 0x8) = r5;
    *(u32*)((u8*)r8 + 0xC) = r4;
    *(u32*)((u8*)r8 + 0x10) = r3;
    *(u32*)((u8*)r8 + 0x14) = r0;
    r8 = r8 + 0x18;
    *(u32*)((u8*)r8 + 0x0) = r29;
    *(u32*)((u8*)r8 + 0x4) = r6;
    *(u32*)((u8*)r8 + 0x8) = r5;
    *(u32*)((u8*)r8 + 0xC) = r4;
    *(u32*)((u8*)r8 + 0x10) = r3;
    *(u32*)((u8*)r8 + 0x14) = r0;
    r3 = (u32)&lbl_803A9A60;
    r3 = (u32)&lbl_803A9A60;
    r0 = *(u32*)((u8*)r3 + 0x4);
    if ((s32)r0 == (s32)0x1) {
        r30 = (u32)sp + 0x8;
        do {
            r3 = r29;
            fn_8025D808();
            *(u16*)((u8*)r30 + 0x0) = r3;
            r30 = r30 + 0x2;
            r29 = r29 + 0x1;
        } while ((s32)r29 < (s32)0x4);
    } else if ((s32)r0 == (s32)0x0) {
        r30 = (u32)sp + 0x8;
        r29 = 0x0;
        do {
            r3 = r29;
            fn_8025D89C();
            *(u16*)((u8*)r30 + 0x0) = r3;
            r30 = r30 + 0x2;
            r29 = r29 + 0x1;
        } while ((s32)r29 < (s32)0x4);
    }
    if ((s32)r31 < (s32)0x0 || (s32)r31 >= (s32)0x3) return;
    if ((s32)r31 != (s32)0x1) {
        /* r31 == 0 or r31 == 2: handled at L_80060260 below */
    } else {
    r3 = (u32)&lbl_803A9E40;
    r8 = (u32)sp + 0x8;
    r7 = (u32)&lbl_803A9E40;
    r9 = 0x0;
    r3 = (u32)&lbl_80267B88;
    r4 = (u32)&lbl_80267B88;
    do {
        r0 = 0x12;
        r6 = (u32)sp + 0xc;
        /* subi r5, r4, 0x4 */;
        ctr_fn = (void(*)(void))r0;
        do {
            r3 = *(u32*)((u8*)r5 + 0x4);
            r0 = *(u32*)((u8*)r5 + 0x8);
            *(u32*)((u8*)r6 + 0x4) = r3;
            r6 += 8; *(u32*)r6 = r0;
        } while (--ctr != 0);
        r3 = *(u16*)((u8*)r8 + 0x0);
        r5 = (u32)sp + 0x10;
        r8 = r8 + 0x2;
        r9 = r9 + 0x1;
        /* subi r0, r3, 0x1 */;
        r0 = r0 * 0x18;
        r5 = r5 + r0;
        r3 = *(u32*)((u8*)r5 + 0x0);
        r0 = *(u32*)((u8*)r5 + 0x4);
        *(u32*)((u8*)r7 + 0x0) = r3;
        r3 = *(u32*)((u8*)r5 + 0x8);
        *(u32*)((u8*)r7 + 0x4) = r0;
        r0 = *(u32*)((u8*)r5 + 0xC);
        *(u32*)((u8*)r7 + 0x8) = r3;
        r3 = *(u32*)((u8*)r5 + 0x10);
        *(u32*)((u8*)r7 + 0xC) = r0;
        r0 = *(u32*)((u8*)r5 + 0x14);
        *(u32*)((u8*)r7 + 0x10) = r3;
        *(u32*)((u8*)r7 + 0x14) = r0;
        r7 = r7 + 0x18;
    } while ((s32)r9 < (s32)0x2);
    return;
    }
    r3 = (u32)&lbl_803A9E40;
    r4 = (u32)sp + 0x8;
    r5 = (u32)&lbl_803A9E40;
    r0 = 0x4;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = *(u16*)((u8*)r4 + 0x0);
        r12 = 0x0;
        if (r3 > (u32)0x0) {
            /* subi r0, r3, 0x8 */;
            if (r3 > (u32)0x8) {
                r0 = r0 & 0xFFFF;
                while (1) {
                    r6 = r12 & 0xFFFF;
                    if (r6 >= (u32)r0) break;
                    r9 = r12 & 0xFFFF;
                    r7 = r12 + 0x1;
                    r8 = r9 << 2;
                    r6 = r12 + 0x2;
                    r30 = r5 + r8;
                    r10 = r12 + 0x3;
                    *(u32*)((u8*)r30 + 0x0) = r9;
                    r7 = r7 & 0xFFFF;
                    r9 = r12 + 0x4;
                    r8 = r12 + 0x5;
                    *(u32*)((u8*)r30 + 0x4) = r7;
                    r11 = r6 & 0xFFFF;
                    r7 = r12 + 0x6;
                    r6 = r12 + 0x7;
                    *(u32*)((u8*)r30 + 0x8) = r11;
                    r10 = r10 & 0xFFFF;
                    r9 = r9 & 0xFFFF;
                    r8 = r8 & 0xFFFF;
                    *(u32*)((u8*)r30 + 0xC) = r10;
                    r7 = r7 & 0xFFFF;
                    r6 = r6 & 0xFFFF;
                    r12 = r12 + 0x8;
                    *(u32*)((u8*)r30 + 0x10) = r9;
                    *(u32*)((u8*)r30 + 0x14) = r8;
                    *(u32*)((u8*)r30 + 0x18) = r7;
                    *(u32*)((u8*)r30 + 0x1C) = r6;
                }
        }
            while (1) {
                r0 = r12 & 0xFFFF;
                if (r0 >= (u32)r3) break;
                r6 = r12 & 0xFFFF;
                r12 = r12 + 0x1;
                r0 = r6 << 2;
                *(u32*)(r5 + r0) = r6;
            }
        }
        r6 = r3;
        if (r3 < (u32)0x6) {
            r0 = 0x6 - r3;
            r0 = r0 & 0xFFFF;
            if (r0 > (u32)0x8) {
                while (1) {
                    r0 = r6 & 0xFFFF;
                    if (r0 >= (u32)0xfffe) break;
                    r0 = r6 & 0xFFFF;
                    r7 = r3;
                    r0 = r0 << 2;
                    r3 = r3 + 0x1;
                    r8 = r7 & 0xFFFF;
                    r6 = r6 + 0x8;
                    r7 = r3;
                    r9 = r5 + r0;
                    r3 = r3 + 0x1;
                    *(u32*)((u8*)r9 + 0x0) = r8;
                    r0 = r3;
                    r7 = r7 & 0xFFFF;
                    *(u32*)((u8*)r9 + 0x4) = r7;
                    r3 = r3 + 0x1;
                    r7 = r3;
                    r0 = r0 & 0xFFFF;
                    *(u32*)((u8*)r9 + 0x8) = r0;
                    r3 = r3 + 0x1;
                    r0 = r3;
                    r7 = r7 & 0xFFFF;
                    *(u32*)((u8*)r9 + 0xC) = r7;
                    r3 = r3 + 0x1;
                    r7 = r3;
                    r0 = r0 & 0xFFFF;
                    *(u32*)((u8*)r9 + 0x10) = r0;
                    r3 = r3 + 0x1;
                    r0 = r3;
                    r7 = r7 & 0xFFFF;
                    *(u32*)((u8*)r9 + 0x14) = r7;
                    r0 = r0 & 0xFFFF;
                    r3 = r3 + 0x1;
                    *(u32*)((u8*)r9 + 0x18) = r0;
                    r0 = r3 & 0xFFFF;
                    r3 = r3 + 0x1;
                    *(u32*)((u8*)r9 + 0x1C) = r0;
                }
        }
            while (1) {
                r0 = r6 & 0xFFFF;
                if (r0 >= (u32)0x6) break;
                r0 = r6 & 0xFFFF;
                r7 = r3 & 0xFFFF;
                r0 = r0 << 2;
                r3 = r3 + 0x1;
                *(u32*)(r5 + r0) = r7;
                r6 = r6 + 0x1;
            }
        }
        r4 = r4 + 0x2;
        r5 = r5 + 0x18;
    } while (--ctr != 0);

    return;
}
#endif


/* 0x80060434 | size: 0x490 */
#if 1
asm void fn_80060434(void) {
#include "src/game/ui/ui_core_fn_80060434.inc"
}
#else
void fn_80060434(void) {
    extern void fn_800FA444();
    extern void fn_800FB680();
    extern void fn_80132A38();
    extern void fn_8025D9A8();
    extern s32 fn_8025DA88();
    extern s32 fn_8025DAD0();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r29 = r3;
    fn_8025DA88();
    fn_8025D9A8();
    r27 = r3;
    fn_8025DAD0();
    r5 = (u32)&lbl_803A9A60;
    r4 = (u32)&lbl_802EF0A8;
    r5 = (u32)&lbl_803A9A60;
    r0 = *(u32*)((u8*)r5 + 0x4);
    r4 = (u32)&lbl_802EF0A8;
    r31 = *(u32*)((u8*)r5 + 0x3C0);
    r30 = r4 + (0x2 << 16);
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 >= (s32)0x1) return;
        if ((s32)r0 < (s32)0x0) {
            return;
        }
        if ((s32)r27 != (s32)0x2) {
            if ((s32)r27 >= (s32)0x2) return;
            if ((s32)r27 != (s32)0x0) {
                if ((s32)r27 < (s32)0x0) return;


                return;
            }
            if ((s32)r31 <= (s32)0x5) {
                r4 = r31 + 0x1;
                r3 = 0x2f;
                fn_80132A38();
                r3 = 0x3f39;
                fn_800FA444();
                r4 = *(s16*)((u8*)r30 + (-12178));
                r5 = (u32)r3 >> 16;
                r3 = *(u8*)((u8*)r29 + 0x8B);
                r0 = -0x100;
                r7 = r4 - r5;
                r4 = 0x0;
                r6 = (u32)r7 >> 31;
                r5 = r3 | r0;
                r0 = r6 + r7;
                r6 = 0x3f39;
                r3 = (s32)r0 >> 1;
                fn_800FB680();
                return;
            }
            if ((s32)r31 == (s32)0x6) {
                r3 = 0x3f3a;
                fn_800FA444();
                r4 = *(s16*)((u8*)r30 + (-12178));
                r5 = (u32)r3 >> 16;
                r3 = *(u8*)((u8*)r29 + 0x8B);
                r0 = -0x100;
                r7 = r4 - r5;
                r4 = 0x0;
                r6 = (u32)r7 >> 31;
                r5 = r3 | r0;
                r0 = r6 + r7;
                r6 = 0x3f3a;
                r3 = (s32)r0 >> 1;
                fn_800FB680();
                return;
            }
            if ((s32)r31 != (s32)0x7) return;
            r3 = 0x3f3b;
            fn_800FA444();
            r4 = *(s16*)((u8*)r30 + (-12178));
            r5 = (u32)r3 >> 16;
            r3 = *(u8*)((u8*)r29 + 0x8B);
            r0 = -0x100;
            r7 = r4 - r5;
            r4 = 0x0;
            r6 = (u32)r7 >> 31;
            r5 = r3 | r0;
            r0 = r6 + r7;
            r6 = 0x3f3b;
            r3 = (s32)r0 >> 1;
            fn_800FB680();
            return;

            r4 = r31 + 0x1;
            r3 = 0x2f;
            fn_80132A38();
            r3 = 0x3f3c;
            fn_800FA444();
            r4 = *(s16*)((u8*)r30 + (-12178));
            r5 = (u32)r3 >> 16;
            r3 = *(u8*)((u8*)r29 + 0x8B);
            r0 = -0x100;
            r7 = r4 - r5;
            r4 = 0x0;
            r6 = (u32)r7 >> 31;
            r5 = r3 | r0;
            r0 = r6 + r7;
            r6 = 0x3f3c;
            r3 = (s32)r0 >> 1;
            fn_800FB680();
            return;
        }
        r4 = (u32)&lbl_802ED9A0;
        r28 = r3 << 2;
        r27 = (u32)&lbl_802ED9A0;
        r3 = *(u32*)(r27 + r28);
        fn_800FA444();
        r4 = *(s16*)((u8*)r30 + (-12178));
        r5 = (u32)r3 >> 16;
        r3 = *(u8*)((u8*)r29 + 0x8B);
        r0 = -0x100;
        r7 = r4 - r5;
        r6 = *(u32*)(r27 + r28);
        r4 = (u32)r7 >> 31;
        r5 = r3 | r0;
        r0 = r4 + r7;
        r4 = 0x0;
        r3 = (s32)r0 >> 1;
        fn_800FB680();
        return;
    }
    if ((s32)r27 != (s32)0x2) {
        if ((s32)r27 >= (s32)0x2) return;
        if ((s32)r27 != (s32)0x0) {
            if ((s32)r27 < (s32)0x0) return;


            return;
        }
        if ((s32)r31 <= (s32)0x5) {
            r4 = r31 + 0x1;
            r3 = 0x2f;
            fn_80132A38();
            r3 = 0x3f39;
            fn_800FA444();
            r4 = (u32)r3 >> 16;
            r3 = 0x3f3d;
            r26 = r4 + 0xb;
            fn_800FA444();
            r3 = (u32)r3 >> 16;
            r0 = *(s16*)((u8*)r30 + (-12178));
            r4 = r26 + r3;
            r3 = *(u8*)((u8*)r29 + 0x8B);
            r6 = r0 - r4;
            r0 = -0x100;
            r5 = (u32)r6 >> 31;
            r4 = 0x0;
            r6 = r5 + r6;
            r5 = r3 | r0;
            r27 = (s32)r6 >> 1;
            r6 = 0x3f39;
            r3 = r27;
            fn_800FB680();
            r5 = *(u8*)((u8*)r29 + 0x8B);
            r0 = -0x100;
            r3 = r27 + r26;
            r4 = 0x0;
            r5 = r5 | r0;
            r6 = 0x3f3d;
            fn_800FB680();
            return;
        }
        if ((s32)r31 == (s32)0x6) {
            r3 = 0x3f3a;
            fn_800FA444();
            r4 = (u32)r3 >> 16;
            r3 = 0x3f3d;
            r26 = r4 + 0x9;
            fn_800FA444();
            r3 = (u32)r3 >> 16;
            r0 = *(s16*)((u8*)r30 + (-12178));
            r4 = r26 + r3;
            r3 = *(u8*)((u8*)r29 + 0x8B);
            r6 = r0 - r4;
            r0 = -0x100;
            r5 = (u32)r6 >> 31;
            r4 = 0x0;
            r6 = r5 + r6;
            r5 = r3 | r0;
            r27 = (s32)r6 >> 1;
            r6 = 0x3f3a;
            r3 = r27;
            fn_800FB680();
            r5 = *(u8*)((u8*)r29 + 0x8B);
            r0 = -0x100;
            r3 = r27 + r26;
            r4 = 0x0;
            r5 = r5 | r0;
            r6 = 0x3f3d;
            fn_800FB680();
            return;
        }
        if ((s32)r31 != (s32)0x7) return;
        r3 = 0x3f3b;
        fn_800FA444();
        r4 = (u32)r3 >> 16;
        r3 = 0x3f3d;
        r26 = r4 + 0x9;
        fn_800FA444();
        r3 = (u32)r3 >> 16;
        r0 = *(s16*)((u8*)r30 + (-12178));
        r4 = r26 + r3;
        r3 = *(u8*)((u8*)r29 + 0x8B);
        r6 = r0 - r4;
        r0 = -0x100;
        r5 = (u32)r6 >> 31;
        r4 = 0x0;
        r6 = r5 + r6;
        r5 = r3 | r0;
        r27 = (s32)r6 >> 1;
        r6 = 0x3f3b;
        r3 = r27;
        fn_800FB680();
        r5 = *(u8*)((u8*)r29 + 0x8B);
        r0 = -0x100;
        r3 = r26 + r27;
        r4 = 0x0;
        r5 = r5 | r0;
        r6 = 0x3f3d;
        fn_800FB680();
        return;

        r3 = 0x3f3c;
        fn_800FA444();
        r4 = (u32)r3 >> 16;
        r3 = 0x3f3d;
        r26 = r4 + 0xb;
        fn_800FA444();
        r3 = (u32)r3 >> 16;
        r0 = *(s16*)((u8*)r30 + (-12178));
        r3 = r26 + r3;
        r4 = r31 + 0x1;
        r5 = r0 - r3;
        r3 = 0x2f;
        r0 = (u32)r5 >> 31;
        r0 = r0 + r5;
        r27 = (s32)r0 >> 1;
        fn_80132A38();
        r5 = *(u8*)((u8*)r29 + 0x8B);
        r0 = -0x100;
        r3 = r27;
        r4 = 0x0;
        r5 = r5 | r0;
        r6 = 0x3f3c;
        fn_800FB680();
        r5 = *(u8*)((u8*)r29 + 0x8B);
        r0 = -0x100;
        r3 = r26 + r27;
        r4 = 0x0;
        r5 = r5 | r0;
        r6 = 0x3f3d;
        fn_800FB680();
        return;
    }
    r4 = (u32)&lbl_802ED9A0;
    r27 = r3 << 2;
    r28 = (u32)&lbl_802ED9A0;
    r3 = *(u32*)(r28 + r27);
    fn_800FA444();
    r4 = (u32)r3 >> 16;
    r3 = 0x3f3d;
    r26 = r4 + 0xb;
    fn_800FA444();
    r3 = (u32)r3 >> 16;
    r0 = *(s16*)((u8*)r30 + (-12178));
    r3 = r26 + r3;
    r4 = r31;
    r5 = r0 - r3;
    r3 = 0x2f;
    r0 = (u32)r5 >> 31;
    r0 = r0 + r5;
    r30 = (s32)r0 >> 1;
    fn_80132A38();
    r4 = *(u8*)((u8*)r29 + 0x8B);
    r0 = -0x100;
    r6 = *(u32*)(r28 + r27);
    r3 = r30;
    r5 = r4 | r0;
    r4 = 0x0;
    fn_800FB680();
    r5 = *(u8*)((u8*)r29 + 0x8B);
    r0 = -0x100;
    r3 = r26 + r30;
    r4 = 0x0;
    r5 = r5 | r0;
    r6 = 0x3f3d;
    fn_800FB680();

    return;
}
#endif


/* 0x800608C4 | size: 0xF0 */
#if 1
asm s32 fn_800608C4(void) {
#include "src/game/ui/ui_core_fn_800608C4.inc"
}
#else
s32 fn_800608C4(void) {
    extern void fn_800D59B8();
    extern void fn_800D5CB8();
    extern void fn_800D61E4();
    extern void fn_800D6728();
    extern void fn_800D67BC();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D85D4();
    extern void fn_800D888C();
    extern void fn_800D88DC();
    extern void fn_80109934();
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;

    r30 = r4;
    r3 = (u32)&lbl_803A9A60;
    r3 = r3 + 0x36c;
    fn_80109934();
    r31 = r3;
    if (r31 != 0) {
        r3 = 0x3;
        fn_800D88DC();
        r3 = 0x4;
        fn_800D888C();
        r3 = 0x7;
        fn_800D6A00();
        r3 = (u32)&lbl_80314F98;
        fn_800D7820();
        r4 = r31;
        r3 = 0x0;
        fn_800D85D4();
        r3 = 0x2;
        fn_800D67BC();
        r3 = 0x0;
        r4 = 0x0;
        fn_800D61E4();
        r3 = 0x0;
        r4 = 0xff;
        r5 = 0xff;
        r6 = 0xff;
        r7 = 0xff;
        fn_800D5CB8();
        f1 = *(f32*)&lbl_8047BF60;
        r3 = 0x0;
        f2 = f1;
        fn_800D59B8();
        r3 = *(s16*)((u8*)r30 + 0x54);
        r4 = *(s16*)((u8*)r30 + 0x56);
        fn_800D61E4();
        r3 = 0x0;
        r4 = 0xff;
        r5 = 0xff;
        r6 = 0xff;
        r7 = 0xff;
        fn_800D5CB8();
        f1 = *(f32*)&lbl_8047BF90;
        r3 = 0x0;
        f2 = f1;
        fn_800D59B8();
        fn_800D6728();
    }
    return 0;
}
#endif



/* 0x800609B4 | size: 0x74 */
#if 1
asm void fn_800609B4(void) {
#include "src/game/ui/ui_core_fn_800609B4.inc"
}
#else
void fn_800609B4(void) {
    extern void fn_800FE4D4();
    extern void fn_800FE6D0();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    
    r0 = *(s16*)((u8*)r4 + 0x6);
    f0 = (f64)(s32)f1;
    r5 = (u32)&lbl_802EF0A8;
    r6 = r0 * 0x1c;
    *(f64*)(sp + 0x8) = f0;
    r0 = (u32)&lbl_802EF0A8;
    r5 = r0 + r6;
    r5 = *(s16*)((u8*)r5 + 0x2);
    r0 = r5 + r0;
    r0 = (s16)r0;
    *(u16*)((u8*)r4 + 0x50) = r0;
    r7 = *(s16*)((u8*)r3 + 0x84);
    r6 = *(s16*)((u8*)r4 + 0x50);
    r5 = *(s16*)((u8*)r3 + 0x86);
    r0 = *(s16*)((u8*)r4 + 0x52);
    r3 = r7 + r6;
    r3 = (s16)r3;
    r0 = r5 + r0;
    r4 = (s16)r0;
    fn_800FE6D0();
    fn_800FE4D4();
    return;
}
#endif


/* 0x80060A28 | size: 0x348 */
#if 1
asm void fn_80060A28(void) {
#include "src/game/ui/ui_core_fn_80060A28.inc"
}
#else
void fn_80060A28(void) {
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r3 = (u32)&lbl_803A9A60;
    r5 = 0x0;
    r6 = (u32)&lbl_803A9A60;
    r4 = r6;
    f3 = *(f32*)&lbl_8047BF60;
    do {
        r3 = r4 + 0x58;
        r7 = 0x0;
        r0 = 0x6;
        ctr_fn = (void(*)(void))r0;
        do {
            f1 = *(f32*)((u8*)r3 + 0x24);
            if (f3 != f1) {
                f0 = *(f32*)((u8*)r6 + 0x3C);
                f0 = f1 - f0;
                *(f32*)((u8*)r3 + 0x24) = f0;
                f0 = *(f32*)((u8*)r3 + 0x24);
                if (f0 < f3) {
                    *(f32*)((u8*)r3 + 0x24) = f3;
        }
                continue;
            }
            f0 = *(f32*)((u8*)r3 + 0x3C);
            f1 = *(f32*)((u8*)r3 + 0x54);
            if (f0 == f1) continue;
            f4 = f1 - f0;
            f2 = *(f32*)&lbl_8047BF94;
            f1 = *(f32*)((u8*)r6 + 0x3C);
            f0 = *(f32*)&lbl_8047BF98;
            f2 = f2 * f4;
            f4 = f2 * f1;
            if (f4 > f0) {
                f4 = f0;
            }
            f0 = *(f32*)&lbl_8047BF9C;
            /* cror eq, lt, eq */;
            if (f4 == f0) {
                f4 = f0;
            }
            f1 = *(f32*)((u8*)r3 + 0x3C);
            f0 = *(f32*)&lbl_8047BF60;
            f1 = f1 + f4;
            *(f32*)((u8*)r3 + 0x3C) = f1;
            f2 = *(f32*)((u8*)r3 + 0x54);
            f0 = *(f32*)((u8*)r3 + 0x3C);
            f1 = f2 - f0;
            if (f4 > f0) {
            } else {

                f4 = -f4;
            }
            f0 = *(f32*)&lbl_8047BF60;
            if (f1 > f0) {
                f0 = f1;
            } else {

                f0 = -f1;
            }
            /* cror eq, lt, eq */;
            if (f0 != f4) {
                f0 = *(f32*)&lbl_8047BF60;
                if (f1 > f0) {
                } else {

                    f1 = -f1;
                }
                f0 = *(f32*)&lbl_8047BF90;
                if (f1 >= f0) continue;
            }
            *(f32*)((u8*)r3 + 0x3C) = f2;

            r3 = r3 + 0x4;
            r7 = r7 + 0x1;
        } while (--ctr != 0);
        r4 = r4 + 0xb4;
        r5 = r5 + 0x1;
    } while ((s32)r5 < (s32)0x4);
    r3 = r6;
    r0 = 0x4;
    f3 = *(f32*)&lbl_8047BF60;
    ctr_fn = (void(*)(void))r0;
    do {
        r4 = r3 + 0x328;
        f1 = *(f32*)((u8*)r4 + 0x0);
        if (f3 != f1) {
            f0 = *(f32*)((u8*)r6 + 0x3C);
            f0 = f1 - f0;
            *(f32*)((u8*)r4 + 0x0) = f0;
            f0 = *(f32*)((u8*)r4 + 0x0);
            if (f0 < f3) {
                *(f32*)((u8*)r4 + 0x0) = f3;
        }
            continue;
        }
        f0 = *(f32*)((u8*)r4 + 0x4);
        f1 = *(f32*)((u8*)r4 + 0x8);
        if (f0 == f1) continue;
        f4 = f1 - f0;
        f2 = *(f32*)&lbl_8047BF94;
        f1 = *(f32*)((u8*)r6 + 0x3C);
        f0 = *(f32*)&lbl_8047BF98;
        f2 = f2 * f4;
        f4 = f2 * f1;
        if (f4 > f0) {
            f4 = f0;
        }
        f0 = *(f32*)&lbl_8047BF9C;
        /* cror eq, lt, eq */;
        if (f4 == f0) {
            f4 = f0;
        }
        f1 = *(f32*)((u8*)r4 + 0x4);
        f0 = *(f32*)&lbl_8047BF60;
        f1 = f1 + f4;
        *(f32*)((u8*)r4 + 0x4) = f1;
        f2 = *(f32*)((u8*)r4 + 0x8);
        f0 = *(f32*)((u8*)r4 + 0x4);
        f1 = f2 - f0;
        if (f4 > f0) {
        } else {

            f4 = -f4;
        }
        f0 = *(f32*)&lbl_8047BF60;
        if (f1 > f0) {
            f0 = f1;
        } else {

            f0 = -f1;
        }
        /* cror eq, lt, eq */;
        if (f0 != f4) {
            f0 = *(f32*)&lbl_8047BF60;
            if (f1 > f0) {
            } else {

                f1 = -f1;
            }
            f0 = *(f32*)&lbl_8047BF90;
            if (f1 >= f0) continue;
        }
        *(f32*)((u8*)r4 + 0x4) = f2;

        r3 = r3 + 0xc;
    } while (--ctr != 0);
    r3 = (u32)&lbl_803A9A60;
    r4 = r7 << 2;
    r0 = (u32)&lbl_803A9A60;
    r5 = r6;
    r3 = r0 + r4;
    r3 = r3 + 0x40;
    r0 = 0x2;
    f3 = *(f32*)&lbl_8047BF60;
    ctr_fn = (void(*)(void))r0;
    do {
        f0 = *(f32*)((u8*)r5 + 0x40);
        if (f3 != f0) {
            f1 = *(f32*)((u8*)r3 + 0x0);
            f0 = *(f32*)((u8*)r6 + 0x3C);
            f0 = f1 - f0;
            *(f32*)((u8*)r3 + 0x0) = f0;
            f0 = *(f32*)((u8*)r3 + 0x0);
            if (f0 < f3) {
                *(f32*)((u8*)r3 + 0x0) = f3;
        }
            continue;
        }
        f0 = *(f32*)((u8*)r5 + 0x48);
        f1 = *(f32*)((u8*)r5 + 0x50);
        if (f0 == f1) continue;
        f4 = f1 - f0;
        f2 = *(f32*)&lbl_8047BF94;
        f1 = *(f32*)((u8*)r6 + 0x3C);
        f0 = *(f32*)&lbl_8047BF98;
        f2 = f2 * f4;
        f4 = f2 * f1;
        if (f4 > f0) {
            f4 = f0;
        }
        f0 = *(f32*)&lbl_8047BF9C;
        /* cror eq, lt, eq */;
        if (f4 == f0) {
            f4 = f0;
        }
        f1 = *(f32*)((u8*)r5 + 0x48);
        f0 = *(f32*)&lbl_8047BF60;
        f1 = f1 + f4;
        *(f32*)((u8*)r5 + 0x48) = f1;
        f2 = *(f32*)((u8*)r5 + 0x50);
        f0 = *(f32*)((u8*)r5 + 0x48);
        f1 = f2 - f0;
        if (f4 > f0) {
        } else {

            f4 = -f4;
        }
        f0 = *(f32*)&lbl_8047BF60;
        if (f1 > f0) {
            f0 = f1;
        } else {

            f0 = -f1;
        }
        /* cror eq, lt, eq */;
        if (f0 != f4) {
            f0 = *(f32*)&lbl_8047BF60;
            if (f1 > f0) {
            } else {

                f1 = -f1;
            }
            f0 = *(f32*)&lbl_8047BF90;
            if (f1 >= f0) continue;
        }
        *(f32*)((u8*)r5 + 0x48) = f2;

        r5 = r5 + 0x4;
    } while (--ctr != 0);
    return;
}
#endif


/* 0x80060D70 | size: 0x184 */
#if 1
asm void fn_80060D70(void) {
#include "src/game/ui/ui_core_fn_80060D70.inc"
}
#else
void fn_80060D70(void) {
    extern void fn_801EF634();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;

    
    r29 = r4;
    r30 = r5;
    r31 = r6;
    r3 = (u32)&lbl_803A9A60;
    r5 = *(u32*)&lbl_8047BF50;
    r3 = (u32)&lbl_803A9A60;
    r4 = *(u32*)&lbl_8047BF54;
    r0 = *(u32*)((u8*)r3 + 0x4);
    if ((s32)r0 == (s32)0x1) {
        fn_801EF634();
        r0 = r3 & 0xFFFF;
        if ((s32)r0 == (s32)0x2 || (s32)r0 == (s32)0x5) {
            r3 = 0x0;
            r0 = 0x1;
            *(u32*)(sp + 0xC) = r0;
        } else if ((s32)r0 == (s32)0x3 || (s32)r0 == (s32)0x4) {
            r3 = 0x1;
            r0 = 0x0;
            *(u32*)(sp + 0xC) = r0;
        } else {
            r0 = 0x2;
            *(u32*)(sp + 0x8) = r0;
            *(u32*)(sp + 0xC) = r0;
        }
        r3 = (u32)&lbl_803A9A60;
        r4 = (u32)&lbl_803A9A60;
        r0 = *(u32*)((u8*)r4 + 0x38);
        if ((s32)r0 >= (s32)0x6) {
            r0 = r30 << 2;
            r3 = (u32)sp + 0x8;
            r0 = *(u32*)(r3 + r0);
            if ((s32)r31 == (s32)r0) {
                r0 = r30 << 3;
                f2 = *(f32*)&lbl_8047BF90;
                r3 = r4 + r0;
                f0 = *(f32*)&lbl_8047BFA0;
                f3 = *(f32*)((u8*)r3 + 0x358);
                f1 = f3 - f2;
                f1 = f2 - f1;
                f0 = f0 * f1;
                f0 = (f64)(s32)f0;
                *(f64*)(sp + 0x10) = f0;
                *(u8*)((u8*)r29 + 0x67) = r0;
                *(f32*)((u8*)r29 + 0x68) = f3;
                *(f32*)((u8*)r29 + 0x6C) = f3;
                r0 = *(u8*)((u8*)r29 + 0x4);
                r0 = r0 | 0x2;
                r0 = (s8)r0;
                *(u8*)((u8*)r29 + 0x4) = r0;
                return;
            }
            r0 = *(u8*)((u8*)r29 + 0x4);
            r0 = r0 & 0xFFFFFFFD;
            r0 = (s8)r0;
            *(u8*)((u8*)r29 + 0x4) = r0;
            return;
        }
        r0 = *(u8*)((u8*)r29 + 0x4);
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r29 + 0x4) = r0;
        return;
    }
    r0 = *(u8*)((u8*)r29 + 0x4);
    r0 = r0 & 0xFFFFFFFD;
    r0 = (s8)r0;
    *(u8*)((u8*)r29 + 0x4) = r0;

    return;
}
#endif


/* 0x80060EF4 | size: 0x124 */
#if 1
asm void fn_80060EF4() {
#include "src/game/ui/ui_core_fn_80060EF4.inc"
}
#else
void fn_80060EF4(void) {
    extern void fn_8025D9A8();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = r4;
    r29 = r5;
    r3 = (u32)&lbl_803A9A60;
    r3 = (u32)&lbl_803A9A60;
    r3 = *(u32*)((u8*)r3 + 0x0);
    r30 = *(u32*)((u8*)r3 + 0xC);
    fn_8025D9A8();
    if ((s32)r29 < (s32)0x0) {
        if ((s32)r3 == (s32)0x1) {
            r0 = *(u8*)((u8*)r31 + 0x4);
            r0 = r0 | 0x2;
            r0 = (s8)r0;
            *(u8*)((u8*)r31 + 0x4) = r0;
            return;
        }
        if ((s32)r29 == (s32)r30) {
            r0 = *(u8*)((u8*)r31 + 0x4);
            r0 = r0 | 0x2;
            r0 = (s8)r0;
            *(u8*)((u8*)r31 + 0x4) = r0;
            return;
        }
        r0 = *(u8*)((u8*)r31 + 0x4);
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r31 + 0x4) = r0;
        return;
    }
    if ((s32)r3 == (s32)0x1) {
        r0 = *(u8*)((u8*)r31 + 0x4);
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r31 + 0x4) = r0;
        return;
    }
    if ((s32)r30 == (s32)0x5) {
        if ((s32)r29 == (s32)0x3) {
            r0 = *(u8*)((u8*)r31 + 0x4);
            r0 = r0 | 0x2;
            r0 = (s8)r0;
            *(u8*)((u8*)r31 + 0x4) = r0;
            return;
        }
        r0 = *(u8*)((u8*)r31 + 0x4);
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r31 + 0x4) = r0;
        return;
    }
    if ((s32)r29 == (s32)r30) {
        r0 = *(u8*)((u8*)r31 + 0x4);
        r0 = r0 | 0x2;
        r0 = (s8)r0;
        *(u8*)((u8*)r31 + 0x4) = r0;
        return;
    }
    r0 = *(u8*)((u8*)r31 + 0x4);
    r0 = r0 & 0xFFFFFFFD;
    r0 = (s8)r0;
    *(u8*)((u8*)r31 + 0x4) = r0;

    return;
}
#endif


/* 0x80061018 | size: 0x10 */
extern u8 lbl_803A9A60[];
u32 fn_80061018(void) {
    return *(u32*)(lbl_803A9A60 + 0x4);
}

/* 0x80061028 | size: 0x44 */
#if 0
asm void fn_80061028(void) {
#include "src/game/ui/ui_core_fn_80061028.inc"
}
#else
void fn_80061028(u32 arg) {
#pragma peephole off
    fn_80102568(0xBA, 0, 1);
    *(u32*)(lbl_803A9A60 + 4) = arg;
}
#endif


/* 0x8006106C | size: 0x1D4 */
#if 1
asm void fn_8006106C(void) {
#include "src/game/ui/ui_core_fn_8006106C.inc"
}
#else
void fn_8006106C(void) {
    extern void fn_80061D34();
    extern void fn_80069A08();
    extern void fn_800FE4D4();
    extern void fn_800FE6D0();
    extern void fn_801040F0();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
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

    
    r25 = r3;
    r26 = r4;
    r27 = r5;
    r28 = r6;
    r29 = r7;
    fn_80061D34();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
    r6 = r27 * 0xb4;
    r5 = (u32)&lbl_803A9A60;
    r4 = *(s16*)((u8*)r26 + 0x6);
    r3 = (u32)&lbl_802EF0A8;
    r0 = (u32)&lbl_803A9A60;
    r5 = r0 + r6;
    r31 = r5 + 0x58;
    r30 = r28 << 2;
    r5 = r31 + r30;
    r0 = (u32)&lbl_802EF0A8;
    f0 = *(f32*)((u8*)r5 + 0x3C);
    r3 = r4 * 0x1c;
    f0 = (f64)(s32)f0;
    r3 = r0 + r3;
    r3 = *(s16*)((u8*)r3 + 0x2);
    *(f64*)(sp + 0x8) = f0;
    r0 = r3 + r0;
    r0 = (s16)r0;
    *(u16*)((u8*)r26 + 0x50) = r0;
    r5 = *(s16*)((u8*)r25 + 0x84);
    r3 = *(s16*)((u8*)r26 + 0x50);
    r4 = *(s16*)((u8*)r25 + 0x86);
    r0 = *(s16*)((u8*)r26 + 0x52);
    r3 = r5 + r3;
    r3 = (s16)r3;
    r0 = r4 + r0;
    r4 = (s16)r0;
    fn_800FE6D0();
    fn_800FE4D4();
    r3 = r25;
    r4 = r26;
    r5 = r27;
    r6 = r28;
    fn_80069A08();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r24 = r28 << 1;
        r0 = *(s16*)(r31 + r24);
        if ((s32)r0 != (s32)0x0) {
            r0 = r0 << 1;
            r3 = (u32)&lbl_80478910;
            r6 = *(u16*)(r3 + r0);
            r5 = r25;
            r3 = 0x0;
            r4 = 0x0;
            r7 = 0x0;
            fn_801040F0();
            r3 = (u32)&lbl_803A9A60;
            r3 = (u32)&lbl_803A9A60;
            r0 = *(u32*)((u8*)r3 + 0x38);
            if ((s32)r0 == (s32)0x3) {
                r4 = r31 + r30;
                f1 = *(f32*)((u8*)r3 + 0x3C);
                r4 = r4 + 0xc;
                f0 = *(f32*)&lbl_8047BFA4;
                f2 = *(f32*)((u8*)r4 + 0x0);
                f1 = f2 + f1;
                *(f32*)((u8*)r4 + 0x0) = f1;
                f1 = *(f32*)((u8*)r4 + 0x0);
                /* cror eq, gt, eq */;
                if (f1 == f0) {
                    f0 = *(f32*)&lbl_8047BF60;
                    *(f32*)((u8*)r4 + 0x0) = f0;
                    r3 = *(s16*)(r31 + r24);
                    /* subi r0, r3, 0x1 */;
                    *(u16*)(r31 + r24) = r0;
        }
        }
        }
    } else {
    r0 = r28 << 1;
    r0 = *(s16*)(r31 + r0);
    if ((s32)r0 != (s32)0x0) {
        r0 = r0 << 1;
        r3 = (u32)&lbl_80478910;
        r6 = *(u16*)(r3 + r0);
        r5 = r25;
        r3 = 0x0;
        r4 = 0x0;
        r7 = 0x0;
        fn_801040F0();
    }
    }
    }
    r3 = r25;
    r4 = r26;
    r5 = r27;
    r6 = r28;
    r7 = r29;
    fn_80061D34();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r5 = r25;
        r3 = -0x8;
        r4 = -0x8;
        r6 = 0x40;
        r7 = 0x0;
        fn_801040F0();
    }
    return;
}
#endif


/* 0x80061240 | size: 0x214 */
#if 1
asm void fn_80061240(void) {
#include "src/game/ui/ui_core_fn_80061240.inc"
}
#else
void fn_80061240(void) {
    extern void fn_800D5BA0();
    extern void fn_800D61E4();
    extern void fn_800D6728();
    extern void fn_800D67BC();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D888C();
    extern void fn_800D88DC();
    u8 sp[0x60];
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f31 = 0.0f;

    
    *(f64*)(sp + 0x50) = f31;
    /* psq_st f31, 0x58((u32)sp), 0, qr0 */;
    r27 = r4;
    r5 = r5 * 0xb4;
    r4 = (u32)&lbl_803A9A60;
    r6 = r6 << 2;
    f0 = *(f32*)&lbl_8047BFA8;
    r0 = (u32)&lbl_803A9A60;
    r26 = *(u8*)((u8*)r3 + 0x8B);
    r3 = r0 + r5;
    r0 = r3 + 0x58;
    r3 = r0 + r6;
    f2 = *(f32*)((u8*)r3 + 0x6C);
    f1 = *(f32*)((u8*)r3 + 0x9C);
    f1 = f2 / f1;
    *(f32*)&lbl_8047891C = f1;
    /* cror eq, lt, eq */;
    if (f1 == f0) {
        r28 = 0xa7;
        r29 = 0x23;
        r25 = 0x13;
    } else {
        f0 = *(f32*)&lbl_8047BF68;
        /* cror eq, lt, eq */;
        if (f1 == f0) {
            r28 = 0xc1;
            r29 = 0xbd;
            r25 = 0x16;
        } else {
            r28 = 0x5;
            r29 = 0xb3;
            r25 = 0x11;
        }
    }
    f0 = *(f32*)&lbl_8047BF60;
    if (f0 != f1) {
        r3 = 0x1;
        fn_800D88DC();
        r3 = 0x6;
        fn_800D888C();
        r3 = (u32)&lbl_80314E08;
        r3 = (u32)&lbl_80314E08;
        fn_800D7820();
        r3 = 0x4;
        fn_800D6A00();
        r3 = 0x4;
        fn_800D67BC();
        /* lha r3, lbl_80478918(r13) */;
        /* lha r4, lbl_8047891A(r13) */;
        fn_800D61E4();
        r30 = r28 & 0xFF;
        r31 = r29 & 0xFF;
        r29 = r25 & 0xFF;
        r28 = r26 & 0xFF;
        /* subi r4, r30, 0x2 */;
        /* subi r0, r31, 0x2 */;
        /* subi r5, r29, 0x2 */;
        r3 = 0x0;
        r4 = r4 & 0xFF;
        r0 = r0 & 0xFF;
        r5 = r5 & 0xFF;
        r4 = r4 << 24;
        r0 = r0 << 16;
        r5 = r5 << 8;
        r0 = r4 | r0;
        r0 = r5 | r0;
        r26 = r28 | r0;
        r4 = r26;
        fn_800D5BA0();
        /* lha r5, lbl_80478918(r13) */;
        r3 = (0x4330 << 16);
        r4 = *(s16*)((u8*)r27 + 0x54);
        /* xoris r0, r5, 0x8000 */;
        r4 = r4 - r5;
        f2 = *(f64*)&lbl_8047BF80;
        /* xoris r4, r4, 0x8000 */;
        *(u32*)(sp + 0x14) = r0;
        f3 = *(f32*)&lbl_8047891C;
        /* lha r4, lbl_8047891A(r13) */;
        f1 = *(f64*)(sp + 0x8);
        f0 = *(f64*)(sp + 0x10);
        f1 = f1 - f2;
        f0 = f0 - f2;
        f31 = f3 * f1 + f0;
        f0 = (f64)(s32)f31;
        *(f64*)(sp + 0x18) = f0;
        fn_800D61E4();
        r4 = r26;
        r3 = 0x0;
        fn_800D5BA0();
        /* lha r4, lbl_8047891A(r13) */;
        r0 = *(s16*)((u8*)r27 + 0x56);
        /* lha r3, lbl_80478918(r13) */;
        r0 = r0 - r4;
        r4 = (s16)r0;
        fn_800D61E4();
        r4 = r30 << 24;
        r0 = r31 << 16;
        r5 = r29 << 8;
        r3 = 0x0;
        r0 = r4 | r0;
        r0 = r5 | r0;
        r26 = r28 | r0;
        r4 = r26;
        fn_800D5BA0();
        f0 = (f64)(s32)f31;
        /* lha r3, lbl_8047891A(r13) */;
        r0 = *(s16*)((u8*)r27 + 0x56);
        *(f64*)(sp + 0x20) = f0;
        r0 = r0 - r3;
        r4 = (s16)r0;
        fn_800D61E4();
        r4 = r26;
        r3 = 0x0;
        fn_800D5BA0();
        fn_800D6728();
    }
    /* psq_l f31, 0x58((u32)sp), 0, qr0 */;
    f31 = *(f64*)(sp + 0x50);
    return;
}
#endif


/* 0x80061454 | size: 0x1A0 */
#if 1
asm void fn_80061454(void) {
#include "src/game/ui/ui_core_fn_80061454.inc"
}
#else
void fn_80061454(void) {
    extern void fn_800D59B8();
    extern void fn_800D5CB8();
    extern void fn_800D61E4();
    extern void fn_800D6728();
    extern void fn_800D67BC();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D85D4();
    extern void fn_800D888C();
    extern void fn_800D88DC();
    extern void fn_800FE4D4();
    extern void fn_800FE6D0();
    extern s32 fn_8025DA88();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
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

    
    r28 = r3;
    r29 = r4;
    r25 = r5;
    r26 = r6;
    r3 = (u32)&lbl_803A9A60;
    r4 = r25 << 3;
    r0 = (u32)&lbl_803A9A60;
    r31 = *(u8*)((u8*)r28 + 0x8B);
    r3 = r0 + r4;
    r27 = 0x1;
    r30 = r3 + 0xc;
    fn_8025DA88();
    if ((s32)r26 == (s32)0x2) {
        if ((s32)r3 != (s32)0x2) {
            r27 = 0x0;
        }
    } else {
        if ((s32)r3 == (s32)0x2) {
            r27 = 0x0;
        }
    }
    r0 = r27 & 0xFF;
    if (r0 != (u32)0x0) {
        r4 = r25 * 0xc;
        r3 = (u32)&lbl_803A9A60;
        r6 = *(s16*)((u8*)r29 + 0x6);
        r5 = (u32)&lbl_802EF0A8;
        r0 = (u32)&lbl_803A9A60;
        r3 = r0 + r4;
        f0 = *(f32*)((u8*)r3 + 0x32C);
        r3 = r6 * 0x1c;
        r0 = (u32)&lbl_802EF0A8;
        f0 = (f64)(s32)f0;
        r3 = r0 + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        *(f64*)(sp + 0x8) = f0;
        r0 = r3 + r0;
        r0 = (s16)r0;
        *(u16*)((u8*)r29 + 0x50) = r0;
        r5 = *(s16*)((u8*)r28 + 0x84);
        r3 = *(s16*)((u8*)r29 + 0x50);
        r4 = *(s16*)((u8*)r28 + 0x86);
        r0 = *(s16*)((u8*)r29 + 0x52);
        r3 = r5 + r3;
        r3 = (s16)r3;
        r0 = r4 + r0;
        r4 = (s16)r0;
        fn_800FE6D0();
        fn_800FE4D4();
        r27 = *(u32*)((u8*)r30 + 0x0);
        if (r27 != (u32)0x0) {
            r3 = 0x3;
            fn_800D88DC();
            r3 = 0x4;
            fn_800D888C();
            r3 = 0x7;
            fn_800D6A00();
            r3 = (u32)&lbl_80314F98;
            r3 = (u32)&lbl_80314F98;
            fn_800D7820();
            r4 = r27;
            r3 = 0x0;
            fn_800D85D4();
            r3 = 0x2;
            fn_800D67BC();
            r3 = 0x0;
            r4 = 0x0;
            fn_800D61E4();
            r7 = r31;
            r3 = 0x0;
            r4 = 0xff;
            r5 = 0xff;
            r6 = 0xff;
            fn_800D5CB8();
            f1 = *(f32*)&lbl_8047BF60;
            r3 = 0x0;
            f2 = f1;
            fn_800D59B8();
            r3 = *(s16*)((u8*)r29 + 0x54);
            r4 = *(s16*)((u8*)r29 + 0x56);
            fn_800D61E4();
            r7 = r31;
            r3 = 0x0;
            r4 = 0xff;
            r5 = 0xff;
            r6 = 0xff;
            fn_800D5CB8();
            f1 = *(f32*)&lbl_8047BF90;
            r3 = 0x0;
            f2 = f1;
            fn_800D59B8();
            fn_800D6728();
    }
    }
    return;
}
#endif


/* 0x800615F4 | size: 0x1EC */
#if 1
asm void fn_800615F4(void) {
#include "src/game/ui/ui_core_fn_800615F4.inc"
}
#else
void fn_800615F4(void) {
    extern s32 fn_800FA280();
    extern void fn_800FB680();
    extern void fn_800FBB34();
    extern void fn_800FE4D4();
    extern void fn_800FE6D0();
    extern void fn_80132A38();
    extern void fn_801FBD28();
    extern void fn_801FBD58();
    extern void fn_801FCC64();
    extern void fn_801FCCC4();
    extern void fn_8025D28C();
    extern void fn_8025D9CC();
    extern void fn_8025DA18();
    extern s32 fn_8025DA88();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;

    
    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    fn_8025D9CC();
    r31 = r3;
    r26 = 0x1;
    fn_8025DA88();
    if ((s32)r30 == (s32)0x2) {
        if ((s32)r3 != (s32)0x2) {
            r26 = 0x0;
        }
    } else {
        if ((s32)r3 == (s32)0x2) {
            r26 = 0x0;
        }
    }
    r0 = r26 & 0xFF;
    if (r0 == (u32)0x0) return;
    r4 = r29 * 0xc;
    r3 = (u32)&lbl_803A9A60;
    r6 = *(s16*)((u8*)r28 + 0x6);
    r5 = (u32)&lbl_802EF0A8;
    r0 = (u32)&lbl_803A9A60;
    r3 = r0 + r4;
    f0 = *(f32*)((u8*)r3 + 0x32C);
    r3 = r6 * 0x1c;
    r0 = (u32)&lbl_802EF0A8;
    f0 = (f64)(s32)f0;
    r3 = r0 + r3;
    r3 = *(s16*)((u8*)r3 + 0x2);
    *(f64*)(sp + 0x8) = f0;
    r0 = r3 + r0;
    r0 = (s16)r0;
    *(u16*)((u8*)r28 + 0x50) = r0;
    r5 = *(s16*)((u8*)r27 + 0x84);
    r3 = *(s16*)((u8*)r28 + 0x50);
    r4 = *(s16*)((u8*)r27 + 0x86);
    r0 = *(s16*)((u8*)r28 + 0x52);
    r3 = r5 + r3;
    r3 = (s16)r3;
    r0 = r4 + r0;
    r4 = (s16)r0;
    fn_800FE6D0();
    fn_800FE4D4();
    if ((s32)r31 == (s32)0x4) {
        if ((s32)r30 != (s32)0x0) return;
        r3 = r29;
        fn_8025DA18();
        r4 = r3 & 0xFFFF;
        r3 = 0x34;
        r4 = r4 + 0x1;
        fn_80132A38();
        if ((s32)r29 == (s32)0x0) {
            r4 = *(u8*)((u8*)r27 + 0x8B);
            r0 = -0x100;
            r5 = *(s16*)((u8*)r28 + 0x54);
            r3 = 0x0;
            r6 = *(s16*)((u8*)r28 + 0x56);
            r7 = r4 | r0;
            r4 = 0x0;
            r8 = 0x30e9;
            fn_800FBB34();
            return;
        }
        r5 = *(u8*)((u8*)r27 + 0x8B);
        r0 = -0x100;
        r3 = 0x0;
        r4 = 0x0;
        r5 = r5 | r0;
        r6 = 0x30e5;
        fn_800FB680();
        return;
    }
    r3 = r29;
    fn_8025D28C();
    r28 = r3;
    r3 = r28 & 0xFFFF;
    fn_801FCCC4();
    fn_801FCC64();
    fn_801FBD58();
    fn_801FBD28();
    r0 = r28 & 0xFFFF;
    r3 = (u32)&lbl_803A9A60;
    r3 = (u32)&lbl_803A9A60;
    r3 = *(u32*)((u8*)r3 + 0x3DC);
    if (r0 == (u32)0x0) {
        r3 = 0x1;
        fn_800FA280();
        r28 = r3;
    } else {

        fn_800FA280();
        r28 = r3;
    }
    r4 = r28;
    r3 = 0x37;
    fn_80132A38();
    r4 = r28;
    r3 = 0x4d;
    fn_80132A38();
    if ((s32)r30 != (s32)0x0) return;
    if ((s32)r29 == (s32)0x0) return;
    r5 = *(u8*)((u8*)r27 + 0x8B);
    r0 = -0x100;
    r3 = 0x0;
    r4 = 0x0;
    r5 = r5 | r0;
    r6 = 0xcf;
    fn_800FB680();

    return;
}
#endif


/* 0x800617E0 | size: 0x24C */
#if 1
asm void fn_800617E0(void) {
#include "src/game/ui/ui_core_fn_800617E0.inc"
}
#else
void fn_800617E0(void) {
    extern s32 fn_800FA280();
    extern void fn_800FB680();
    extern void fn_800FBB34();
    extern void fn_800FE4D4();
    extern void fn_800FE6D0();
    extern void fn_8012AC54();
    extern void fn_80132A38();
    extern void fn_8025D914();
    extern void fn_8025D9CC();
    extern void fn_8025DA18();
    extern s32 fn_8025DA88();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;

    
    r28 = r3;
    r29 = r4;
    r30 = r5;
    r31 = r6;
    r27 = 0x1;
    fn_8025DA88();
    if ((s32)r31 == (s32)0x2) {
        if ((s32)r3 != (s32)0x2) {
            r27 = 0x0;
        }
    } else {
        if ((s32)r3 == (s32)0x2) {
            r27 = 0x0;
        }
    }
    r0 = r27 & 0xFF;
    if (r0 == (u32)0x0) return;
    r4 = r30 * 0xc;
    r3 = (u32)&lbl_803A9A60;
    r6 = *(s16*)((u8*)r29 + 0x6);
    r5 = (u32)&lbl_802EF0A8;
    r0 = (u32)&lbl_803A9A60;
    r3 = r0 + r4;
    f0 = *(f32*)((u8*)r3 + 0x32C);
    r3 = r6 * 0x1c;
    r0 = (u32)&lbl_802EF0A8;
    f0 = (f64)(s32)f0;
    r3 = r0 + r3;
    r3 = *(s16*)((u8*)r3 + 0x2);
    *(f64*)(sp + 0x8) = f0;
    r0 = r3 + r0;
    r0 = (s16)r0;
    *(u16*)((u8*)r29 + 0x50) = r0;
    r5 = *(s16*)((u8*)r28 + 0x84);
    r3 = *(s16*)((u8*)r29 + 0x50);
    r4 = *(s16*)((u8*)r28 + 0x86);
    r0 = *(s16*)((u8*)r29 + 0x52);
    r3 = r5 + r3;
    r3 = (s16)r3;
    r0 = r4 + r0;
    r4 = (s16)r0;
    fn_800FE6D0();
    fn_800FE4D4();
    r3 = r30;
    fn_8025D914();
    fn_8012AC54();
    r27 = r3;
    if (r27 == (u32)0x0) {
        r3 = 0x1;
        fn_800FA280();
        r27 = r3;
    }
    fn_8025D9CC();
    if ((s32)r3 == (s32)0x4) {
        r4 = r27;
        r3 = 0x37;
        fn_80132A38();
        r4 = r27;
        r3 = 0x4d;
        fn_80132A38();
    } else if ((s32)r30 == (s32)0x0) {
        r4 = r27;
        r3 = 0x37;
        fn_80132A38();
        r4 = r27;
        r3 = 0x4d;
        fn_80132A38();
    } else {
        r4 = (u32)&lbl_803A9A60;
        r3 = 0x37;
        r4 = (u32)&lbl_803A9A60;
        r27 = r4 + 0x3c4;
        r4 = r27;
        fn_80132A38();
        r4 = r27;
        r3 = 0x4d;
        fn_80132A38();
    }
    if ((s32)r31 == (s32)0x0) {
        if ((s32)r30 == (s32)0x0) {
            r4 = *(u8*)((u8*)r28 + 0x8B);
            r0 = -0x100;
            r5 = *(s16*)((u8*)r29 + 0x54);
            r3 = 0x0;
            r6 = *(s16*)((u8*)r29 + 0x56);
            r7 = r4 | r0;
            r4 = 0x0;
            r8 = 0x30e2;
            fn_800FBB34();
            return;
        }
        r5 = *(u8*)((u8*)r28 + 0x8B);
        r0 = -0x100;
        r3 = 0x0;
        r4 = 0x0;
        r5 = r5 | r0;
        r6 = 0xce;
        fn_800FB680();
        return;
    }
    r3 = r30;
    fn_8025DA18();
    r4 = r3 & 0xFFFF;
    r3 = 0x34;
    r4 = r4 + 0x1;
    fn_80132A38();
    if ((s32)r30 < (s32)0x2) {
        r4 = *(u8*)((u8*)r28 + 0x8B);
        r0 = -0x100;
        r5 = *(s16*)((u8*)r29 + 0x54);
        r3 = 0x0;
        r6 = *(s16*)((u8*)r29 + 0x56);
        r7 = r4 | r0;
        r4 = 0x0;
        r8 = 0x30e9;
        fn_800FBB34();
        r4 = *(u8*)((u8*)r28 + 0x8B);
        r0 = -0x100;
        r5 = *(s16*)((u8*)r29 + 0x54);
        r3 = 0x0;
        r6 = *(s16*)((u8*)r29 + 0x56);
        r7 = r4 | r0;
        r4 = 0x16;
        r8 = 0x30e8;
        fn_800FBB34();
        return;
    }
    r5 = *(u8*)((u8*)r28 + 0x8B);
    r0 = -0x100;
    r3 = 0x0;
    r4 = 0x0;
    r5 = r5 | r0;
    r6 = 0x30e7;
    fn_800FB680();

    return;
}
#endif


/* 0x80061A2C | size: 0x148 */
#if 1
asm void fn_80061A2C(void) {
#include "src/game/ui/ui_core_fn_80061A2C.inc"
}
#else
void fn_80061A2C(void) {
    extern void fn_80061240();
    extern void fn_80061D34();
    extern void fn_800FE4D4();
    extern void fn_800FE6D0();
    extern void fn_801040F0();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;

    
    r27 = r3;
    r28 = r4;
    r29 = r5;
    r30 = r6;
    r31 = r7;
    r8 = (u32)&lbl_803A9A60;
    r8 = (u32)&lbl_803A9A60;
    r0 = *(u32*)((u8*)r8 + 0x4);
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 >= (s32)0x1) return;
        if ((s32)r0 < (s32)0x0) {
            return;
        }
        r0 = *(u8*)((u8*)r28 + 0x4);
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r28 + 0x4) = r0;
        return;
    }
    fn_80061D34();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r4 = r29 * 0xb4;
        r3 = (u32)&lbl_803A9A60;
        r6 = *(s16*)((u8*)r28 + 0x6);
        r5 = (u32)&lbl_802EF0A8;
        r0 = (u32)&lbl_803A9A60;
        r3 = r0 + r4;
        r0 = r30 << 2;
        r4 = (u32)&lbl_802EF0A8;
        r3 = r3 + r0;
        f0 = *(f32*)((u8*)r3 + 0x94);
        r0 = r6 * 0x1c;
        f0 = (f64)(s32)f0;
        r3 = r4 + r0;
        r3 = *(s16*)((u8*)r3 + 0x2);
        *(f64*)(sp + 0x8) = f0;
        r0 = r3 + r0;
        r0 = (s16)r0;
        *(u16*)((u8*)r28 + 0x50) = r0;
        r5 = *(s16*)((u8*)r27 + 0x84);
        r3 = *(s16*)((u8*)r28 + 0x50);
        r4 = *(s16*)((u8*)r27 + 0x86);
        r0 = *(s16*)((u8*)r28 + 0x52);
        r3 = r5 + r3;
        r3 = (s16)r3;
        r0 = r4 + r0;
        r4 = (s16)r0;
        fn_800FE6D0();
        fn_800FE4D4();
        r0 = *(u8*)((u8*)r28 + 0x4);
        r5 = r27;
        r3 = 0x0;
        r4 = 0x0;
        r0 = r0 & 0xFFFFFFFD;
        r6 = 0x314;
        r0 = (s8)r0;
        r7 = 0x0;
        *(u8*)((u8*)r28 + 0x4) = r0;
        fn_801040F0();
        r3 = r27;
        r4 = r28;
        r5 = r29;
        r6 = r30;
        r7 = r31;
        fn_80061240();
        return;
    }
    r0 = *(u8*)((u8*)r28 + 0x4);
    r0 = r0 & 0xFFFFFFFD;
    r0 = (s8)r0;
    *(u8*)((u8*)r28 + 0x4) = r0;

    return;
}
#endif


/* 0x80061B74 | size: 0x48 */
#if 0
asm void fn_80061B74(void) {
#include "src/game/ui/ui_core_fn_80061B74.inc"
}
#else
void fn_80061B74(void* unused, s8* entry) {
#pragma peephole off
    s32 state = *(s32*)(lbl_803A9A60 + 4);
    if (state != 1) {
        if (state >= 1) {
            return;
        }
        if (state < 0) {
            return;
        }
        entry[4] = (s8)(entry[4] & ~2);
        return;
    }
    entry[4] = (s8)(entry[4] & ~2);
}
#endif


/* 0x80061BBC | size: 0x178 */
#if 1
asm void fn_80061BBC(void) {
#include "src/game/ui/ui_core_fn_80061BBC.inc"
}
#else
void fn_80061BBC(void) {
    extern void fn_80061D34();
    extern void fn_800FE4D4();
    extern void fn_800FE6D0();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    
    r25 = r3;
    r26 = r4;
    r27 = r5;
    r28 = r6;
    r29 = r7;
    r3 = (u32)&lbl_803A9A60;
    r5 = (u32)&lbl_803A9A60;
    r0 = *(u32*)((u8*)r5 + 0x4);
    if ((s32)r0 != (s32)0x1) {
        if ((s32)r0 >= (s32)0x1) return;
        if ((s32)r0 < (s32)0x0) {
            return;
        }
        r0 = *(u8*)((u8*)r26 + 0x4);
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r26 + 0x4) = r0;
        return;
    }
    r0 = r27 * 0xb4;
    r4 = *(s16*)((u8*)r26 + 0x6);
    r3 = (u32)&lbl_802EF0A8;
    r31 = r28 << 2;
    r5 = r5 + r0;
    r0 = (u32)&lbl_802EF0A8;
    r30 = r5 + 0x58;
    r3 = r30 + r31;
    f0 = *(f32*)((u8*)r3 + 0x3C);
    r3 = r4 * 0x1c;
    f0 = (f64)(s32)f0;
    r3 = r0 + r3;
    r3 = *(s16*)((u8*)r3 + 0x2);
    *(f64*)(sp + 0x8) = f0;
    r0 = r3 + r0;
    r0 = (s16)r0;
    *(u16*)((u8*)r26 + 0x50) = r0;
    r5 = *(s16*)((u8*)r25 + 0x84);
    r3 = *(s16*)((u8*)r26 + 0x50);
    r4 = *(s16*)((u8*)r25 + 0x86);
    r0 = *(s16*)((u8*)r26 + 0x52);
    r3 = r5 + r3;
    r3 = (s16)r3;
    r0 = r4 + r0;
    r4 = (s16)r0;
    fn_800FE6D0();
    fn_800FE4D4();
    r3 = (u32)&lbl_803A9A60;
    r3 = (u32)&lbl_803A9A60;
    r0 = *(u32*)((u8*)r3 + 0x38);
    if ((s32)r0 >= (s32)0x5) {
        r3 = r25;
        r4 = r26;
        r5 = r27;
        r6 = r28;
        r7 = r29;
        fn_80061D34();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x0) {
            r3 = r30 + r31;
            f1 = *(f32*)&lbl_8047BF60;
            f0 = *(f32*)((u8*)r3 + 0x84);
            if (f1 == f0) {
                r0 = *(u8*)((u8*)r26 + 0x4);
                r0 = r0 | 0x2;
                r0 = (s8)r0;
                *(u8*)((u8*)r26 + 0x4) = r0;
                return;
            }
            r0 = *(u8*)((u8*)r26 + 0x4);
            r0 = r0 & 0xFFFFFFFD;
            r0 = (s8)r0;
            *(u8*)((u8*)r26 + 0x4) = r0;
            return;
        }
        r0 = *(u8*)((u8*)r26 + 0x4);
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r26 + 0x4) = r0;
        return;
    }
    r0 = *(u8*)((u8*)r26 + 0x4);
    r0 = r0 & 0xFFFFFFFD;
    r0 = (s8)r0;
    *(u8*)((u8*)r26 + 0x4) = r0;

    return;
}
#endif


/* 0x80061D34 | size: 0x238 */
#if 1
asm void fn_80061D34(void) {
#include "src/game/ui/ui_core_fn_80061D34.inc"
}
#else
void fn_80061D34(void) {
    extern void fn_8025D808();
    extern s32 fn_8025D89C();
    extern s32 fn_8025DA88();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r30 = r4;
    r24 = r5;
    r25 = r6;
    r26 = r7;
    fn_8025DA88();
    r4 = (u32)&lbl_803A9A60;
    r28 = r3;
    r29 = (u32)&lbl_803A9A60;
    r31 = 0x1;
    r0 = *(u32*)((u8*)r29 + 0x4);
    if ((s32)r0 == (s32)0x1) {
        r3 = r24;
        fn_8025D808();
        r27 = r3 & 0xFFFF;
    } else if ((s32)r0 == (s32)0x0) {
        r3 = r24;
        fn_8025D89C();
        r27 = r3 & 0xFFFF;
    }
    r0 = *(u32*)((u8*)r29 + 0x4);
    if ((s32)r0 == (s32)0x0) {
        if ((s32)r26 == (s32)0x2) {
            if ((s32)r28 == (s32)0x2) { r3 = r31; return; }
            r0 = *(u8*)((u8*)r30 + 0x4);
            r31 = 0x0;
            r0 = r0 & 0xFFFFFFFD;
            r0 = (s8)r0;
            *(u8*)((u8*)r30 + 0x4) = r0;
            r3 = r31;
            return;
        }
        if ((s32)r26 == (s32)0x0) {
            if ((s32)r27 >= (s32)0x4) {
                r0 = *(u8*)((u8*)r30 + 0x4);
                r31 = 0x0;
                r0 = r0 & 0xFFFFFFFD;
                r0 = (s8)r0;
                *(u8*)((u8*)r30 + 0x4) = r0;
                r3 = r31;
                return;
            }
            if ((s32)r28 != (s32)0x2) { r3 = r31; return; }
            r0 = *(u8*)((u8*)r30 + 0x4);
            r31 = 0x0;
            r0 = r0 & 0xFFFFFFFD;
            r0 = (s8)r0;
            *(u8*)((u8*)r30 + 0x4) = r0;
            r3 = r31;
            return;
        }
        if ((s32)r27 < (s32)0x4) {
            r0 = *(u8*)((u8*)r30 + 0x4);
            r31 = 0x0;
            r0 = r0 & 0xFFFFFFFD;
            r0 = (s8)r0;
            *(u8*)((u8*)r30 + 0x4) = r0;
            r3 = r31;
            return;
        }
        if ((s32)r28 != (s32)0x2) { r3 = r31; return; }
        r0 = *(u8*)((u8*)r30 + 0x4);
        r31 = 0x0;
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r30 + 0x4) = r0;
        r3 = r31;
        return;
    }
    if ((s32)r26 == (s32)0x2) {
        if ((s32)r28 != (s32)0x2) {
            r0 = *(u8*)((u8*)r30 + 0x4);
            r31 = 0x0;
            r0 = r0 & 0xFFFFFFFD;
            r0 = (s8)r0;
            *(u8*)((u8*)r30 + 0x4) = r0;
        }
        if ((s32)r27 > (s32)r25) { r3 = r31; return; }
        r31 = 0x0;
        r3 = r31;
        return;
    }
    if ((s32)r26 == (s32)0x0) {
        if ((s32)r27 >= (s32)0x4) {
            r0 = *(u8*)((u8*)r30 + 0x4);
            r31 = 0x0;
            r0 = r0 & 0xFFFFFFFD;
            r0 = (s8)r0;
            *(u8*)((u8*)r30 + 0x4) = r0;
            r3 = r31;
            return;
        }
        if ((s32)r28 == (s32)0x2) {
            r0 = *(u8*)((u8*)r30 + 0x4);
            r31 = 0x0;
            r0 = r0 & 0xFFFFFFFD;
            r0 = (s8)r0;
            *(u8*)((u8*)r30 + 0x4) = r0;
            r3 = r31;
            return;
        }
        if ((s32)r27 > (s32)r25) { r3 = r31; return; }
        r0 = *(u8*)((u8*)r30 + 0x4);
        r31 = 0x0;
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r30 + 0x4) = r0;
        r3 = r31;
        return;
    }
    if ((s32)r27 < (s32)0x4) {
        r0 = *(u8*)((u8*)r30 + 0x4);
        r31 = 0x0;
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r30 + 0x4) = r0;
        r3 = r31;
        return;
    }
    if ((s32)r28 == (s32)0x2) {
        r0 = *(u8*)((u8*)r30 + 0x4);
        r31 = 0x0;
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r30 + 0x4) = r0;
        r3 = r31;
        return;
    }
    if ((s32)r27 > (s32)r25) { r3 = r31; return; }
    r0 = *(u8*)((u8*)r30 + 0x4);
    r31 = 0x0;
    r0 = r0 & 0xFFFFFFFD;
    r0 = (s8)r0;
    *(u8*)((u8*)r30 + 0x4) = r0;

    r3 = r31;
    return;
}
#endif


/* 0x80061F6C | size: 0x318 */
#if 1
asm void fn_80061F6C(void) {
#include "src/game/ui/ui_core_fn_80061F6C.inc"
}
#else
void fn_80061F6C(void) {
    extern void fn_8005FFE4();
    extern void fn_80062334();
    extern void fn_80068F84();
    extern void fn_800697C4();
    extern void fn_800F9E70();
    extern s32 fn_800FA280();
    extern void fn_8011F15C();
    extern s32 fn_8011F188();
    extern void fn_8012AC54();
    extern void fn_80165A20();
    extern void fn_801FBD28();
    extern void fn_801FBD58();
    extern void fn_801FCC64();
    extern void fn_801FCCC4();
    extern void fn_8025D28C();
    extern void fn_8025D914();
    extern void fn_8025D938();
    extern void fn_8025D9A8();
    extern s32 fn_8025DA88();
    extern void fn_8025DBB0();
    u8 sp[0x40];
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    
    r30 = r3;
    r28 = r4;
    fn_8025DA88();
    r31 = r3;
    r29 = 0x1;
    fn_8025DA88();
    if ((s32)r31 == (s32)0x2) {
        if ((s32)r3 != (s32)0x2) {
            r29 = 0x0;
        }

    } else {
    if ((s32)r3 == (s32)0x2) {
        r29 = 0x0;
    }
    }
    r0 = r29 & 0xFF;
    if (r0 != (u32)0x0) {
        r3 = 0x1;
        fn_8025D914();
        fn_8012AC54();
        r4 = (u32)&lbl_803A9A60;
        r4 = (u32)&lbl_803A9A60;
        r27 = r4 + 0x3c4;
        r4 = r3;
        if (r27 != (u32)0x0) {
            r3 = r27;
            fn_800F9E70();
        } else {

            r3 = 0x1;
            fn_800FA280();
            r4 = r3;
            r3 = r27;
            fn_800F9E70();
        }
        r3 = 0x1;
        fn_8025D28C();
        r3 = r3 & 0xFFFF;
        fn_801FCCC4();
        fn_801FCC64();
        fn_801FBD58();
        fn_801FBD28();
        r4 = (u32)&lbl_803A9A60;
        r4 = (u32)&lbl_803A9A60;
        *(u32*)((u8*)r4 + 0x3DC) = r3;
    }
    fn_8025DBB0();
    r4 = (u32)&lbl_803A9A60;
    r0 = 0x0;
    r4 = (u32)&lbl_803A9A60;
    *(u32*)((u8*)r4 + 0x3C0) = r3;
    *(u32*)((u8*)r4 + 0x0) = r30;
    *(u32*)((u8*)r4 + 0x4) = r28;
    *(u32*)((u8*)r4 + 0x38) = r0;
    fn_8005FFE4();
    fn_80062334();
    r3 = (u32)&lbl_803A9A60;
    f0 = *(f32*)&lbl_8047BF60;
    r3 = (u32)&lbl_803A9A60;
    *(f32*)((u8*)r3 + 0x3B4) = f0;
    if ((s32)r28 != (s32)0x1) {
        if ((s32)r28 >= (s32)0x1) return;
        if ((s32)r28 < (s32)0x0) {
            return;
        }
        fn_80068F84();
        fn_800697C4();
        fn_8025DBB0();
        r31 = r3;
        fn_8025D9A8();
        if ((s32)r3 == (s32)0x0) {
            r4 = (u32)&lbl_802ED958;
            r3 = (u32)&lbl_803A9A60;
            r0 = r31 << 2;
            r4 = (u32)&lbl_802ED958;
            r3 = (u32)&lbl_803A9A60;
            r0 = *(u32*)(r4 + r0);
            *(u32*)((u8*)r3 + 0x3BC) = r0;
        } else if ((s32)r3 == (s32)0x1) {
            r3 = (0x6666 << 16);
            r4 = (u32)&lbl_802ED978;
            r0 = r3 + 0x6667;
            r3 = (u32)&lbl_803A9A60;
            r0 = (s32)((s64)r0 * (s64)r31 >> 32);
            r4 = (u32)&lbl_802ED978;
            r3 = (u32)&lbl_803A9A60;
            r0 = (s32)r0 >> 2;
            r5 = (u32)r0 >> 31;
            r0 = r0 + r5;
            r0 = r0 * 0xa;
            r0 = r31 - r0;
            r0 = r0 << 2;
            r0 = *(u32*)(r4 + r0);
            *(u32*)((u8*)r3 + 0x3BC) = r0;
        } else {
            r3 = (u32)&lbl_803A9A60;
            r0 = 0x3cd;
            r3 = (u32)&lbl_803A9A60;
            *(u32*)((u8*)r3 + 0x3BC) = r0;
        }
        r3 = (u32)&lbl_803A9A60;
        r4 = 0x0;
        r3 = (u32)&lbl_803A9A60;
        r5 = 0xff;
        r3 = *(u32*)((u8*)r3 + 0x3BC);
        fn_80165A20();
        return;
    }
    fn_80068F84();
    fn_800697C4();
    r3 = (u32)&lbl_803A9A60;
    r29 = 0x0;
    r27 = (u32)&lbl_803A9A60;
    do {
        r28 = r27 + 0x58;
        r30 = 0x0;
        do {
            r3 = r29;
            r4 = r30;
            fn_8025D938();
            r31 = r3;
            if (r31 != (u32)0x0) {
                fn_8011F15C();
                r3 = r3 & 0xFFFF;
                r0 = (0x4330 << 16);
                r3 = r31;
                f1 = *(f64*)&lbl_8047BF88;
                *(u32*)(sp + 0x8) = r0;
                f0 = *(f64*)(sp + 0x8);
                f0 = f0 - f1;
                *(f32*)((u8*)r28 + 0x6C) = f0;
                fn_8011F188();
                r3 = r3 & 0xFFFF;
                r0 = (0x4330 << 16);
                r3 = r31;
                f1 = *(f64*)&lbl_8047BF88;
                *(u32*)(sp + 0x10) = r0;
                f0 = *(f64*)(sp + 0x10);
                f0 = f0 - f1;
                *(f32*)((u8*)r28 + 0x84) = f0;
                fn_8011F15C();
                r3 = r3 & 0xFFFF;
                r0 = (0x4330 << 16);
                f1 = *(f64*)&lbl_8047BF88;
                *(u32*)(sp + 0x18) = r0;
                f0 = *(f64*)(sp + 0x18);
                f0 = f0 - f1;
                *(f32*)((u8*)r28 + 0x9C) = f0;
            } else {

                f1 = *(f32*)&lbl_8047BF60;
                f0 = *(f32*)&lbl_8047BFAC;
                *(f32*)((u8*)r28 + 0x6C) = f1;
                *(f32*)((u8*)r28 + 0x84) = f1;
                *(f32*)((u8*)r28 + 0x9C) = f0;
            }
            r28 = r28 + 0x4;
            r30 = r30 + 0x1;
        } while ((s32)r30 < (s32)0x6);
        r27 = r27 + 0xb4;
        r29 = r29 + 0x1;
    } while ((s32)r29 < (s32)0x4);
    r3 = (u32)&lbl_803A9A60;
    r0 = 0x1e;
    r4 = (u32)&lbl_803A9A60;
    r3 = 0x1e;
    *(u32*)((u8*)r4 + 0x3BC) = r0;
    r4 = 0x0;
    r5 = 0xff;
    fn_80165A20();

    return;
}
#endif


/* 0x80062284 | size: 0xB0 */
#if 1
asm void fn_80062284(void) {
#include "src/game/ui/ui_core_fn_80062284.inc"
}
#else
void fn_80062284(void) {
    extern void fn_80123FBC();
    extern s32 fn_8012640C();
    extern void fn_8025D808();
    extern void fn_8025D938();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r28 = r3;
    fn_8025D808();
    r29 = r3 & 0xFFFF;
    r31 = 0x0;
    while ((s32)r31 < (s32)r29) {

        r3 = r28;
        r4 = r31;
        fn_8025D938();
        r30 = r3;
        if (r30 != (u32)0x0) {
            fn_80123FBC();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x0) {
                r3 = r30;
                r4 = 0x0;
                r5 = 0x7b;
                r6 = 0x0;
                fn_8012640C();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = 0x0;
                    return;
        }
        }
        }
        r31 = r31 + 0x1;

    }
    r3 = 0x1;

    return;
}
#endif


/* 0x80062334 | size: 0x398 */
#if 1
asm void fn_80062334(void) {
#include "src/game/ui/ui_core_fn_80062334.inc"
}
#else
void fn_80062334(void) {
    extern s32 fn_8025DA88();
    u8 sp[0x70];
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    fn_8025DA88();
    r4 = (u32)&lbl_803A9E40;
    f5 = *(f32*)&lbl_8047BFA4;
    r7 = (u32)&lbl_803A9E40;
    f4 = *(f32*)&lbl_8047BFA8;
    f3 = *(f32*)&lbl_8047BFB0;
    r6 = r7 + 0x18;
    f2 = *(f32*)&lbl_8047BFB4;
    r5 = r7 + 0x30;
    f1 = *(f32*)&lbl_8047BF68;
    r0 = r7 + 0x48;
    f0 = *(f32*)&lbl_8047BFB8;
    r4 = (u32)&lbl_803A9A60;
    r11 = (u32)&lbl_803A9A60;
    r8 = (u32)sp + 0x8;
    r9 = (u32)sp + 0x34;
    r7 = r11;
    r10 = (u32)sp + 0x18;
    r31 = 0x0;
    *(u32*)(sp + 0x14) = r0;
    *(f32*)(sp + 0x34) = f5;
    *(f32*)(sp + 0x38) = f4;
    *(f32*)(sp + 0x3C) = f3;
    *(f32*)(sp + 0x40) = f2;
    *(f32*)(sp + 0x44) = f1;
    *(f32*)(sp + 0x48) = f0;
    *(f32*)(sp + 0x18) = f0;
    *(f32*)(sp + 0x1C) = f1;
    *(f32*)(sp + 0x20) = f2;
    *(f32*)(sp + 0x24) = f3;
    *(f32*)(sp + 0x28) = f4;
    *(f32*)(sp + 0x2C) = f5;
    do {
        r30 = r7 + 0x58;
        if ((s32)r3 == (s32)0x2) {
            r4 = r30;
            r5 = r30;
            r6 = r9;
            r12 = r10;
            r0 = 0x6;
            ctr_fn = (void(*)(void))r0;
            do {
                r0 = *(u32*)((u8*)r11 + 0x4);
                if ((s32)r0 == (s32)0x1) {
                    r0 = 0x0;
                    f0 = *(f32*)&lbl_8047BF60;
                    *(u16*)((u8*)r4 + 0x0) = r0;
                    *(f32*)((u8*)r5 + 0xC) = f0;
                } else if ((s32)r0 == (s32)0x0) {
                    r0 = 0x3;
                    f0 = *(f32*)&lbl_8047BF60;
                    *(u16*)((u8*)r4 + 0x0) = r0;
                    *(f32*)((u8*)r5 + 0xC) = f0;
                }
                if ((s32)r31 < (s32)0x2) {
                    f0 = *(f32*)&lbl_8047BFBC;
                    f1 = *(f32*)&lbl_8047BF60;
                    *(f32*)((u8*)r5 + 0x3C) = f0;
                    f0 = *(f32*)((u8*)r6 + 0x0);
                    *(f32*)((u8*)r5 + 0x54) = f1;
                    *(f32*)((u8*)r5 + 0x24) = f0;
                } else {

                    f0 = *(f32*)&lbl_8047BFC0;
                    f1 = *(f32*)&lbl_8047BF60;
                    *(f32*)((u8*)r5 + 0x3C) = f0;
                    f0 = *(f32*)((u8*)r12 + 0x0);
                    *(f32*)((u8*)r5 + 0x54) = f1;
                    *(f32*)((u8*)r5 + 0x24) = f0;
                }
                r4 = r4 + 0x2;
                r5 = r5 + 0x4;
                r6 = r6 + 0x4;
                r12 = r12 + 0x4;
            } while (--ctr != 0);
        } else {
        r26 = (u32)r31 >> 31;
        r0 = r31 & 0x1;
        r0 = r0 ^ r26;
        r12 = 0x0;
        r4 = r12;
        r5 = r30;
        r6 = r30;
        r0 = r0 - r26;
        r29 = 0x6;
        ctr_fn = (void(*)(void))r29;
        do {
            r26 = *(u32*)((u8*)r11 + 0x4);
            r27 = *(u32*)((u8*)r8 + 0x0);
            r28 = *(u32*)(r27 + r4);
            if ((s32)r26 == (s32)0x1) {
                r26 = 0x0;
                f0 = *(f32*)&lbl_8047BF60;
                *(u16*)((u8*)r5 + 0x0) = r26;
                *(f32*)((u8*)r6 + 0xC) = f0;
            } else if ((s32)r26 == (s32)0x0) {
                r26 = 0x3;
                f0 = *(f32*)&lbl_8047BF60;
                *(u16*)((u8*)r5 + 0x0) = r26;
                *(f32*)((u8*)r6 + 0xC) = f0;
            }
            if ((s32)r0 != (s32)0x0) {
                r26 = (0x5555 << 16);
                f0 = *(f32*)&lbl_8047BFC0;
                r27 = r26 + 0x5556;
                r26 = r28 << 2;
                r29 = (s32)((s64)r27 * (s64)r12 >> 32);
                *(f32*)((u8*)r6 + 0x3C) = f0;
                f0 = *(f32*)&lbl_8047BF60;
                r28 = (u32)sp + 0x18;
                r26 = r26 + 0x24;
                *(f32*)((u8*)r6 + 0x54) = f0;
                r27 = (u32)r29 >> 31;
                r27 = r29 + r27;
                r27 = r27 * 0x3;
                r27 = r12 - r27;
                r27 = r27 << 2;
                r27 = r27 + 0xc;
                f0 = *(f32*)(r28 + r27);
                *(f32*)(r30 + r26) = f0;
            } else {

                r27 = (0x5555 << 16);
                f0 = *(f32*)&lbl_8047BFBC;
                r27 = r27 + 0x5556;
                r29 = r28 << 2;
                r26 = (s32)((s64)r27 * (s64)r12 >> 32);
                *(f32*)((u8*)r6 + 0x3C) = f0;
                f0 = *(f32*)&lbl_8047BF60;
                r28 = (u32)sp + 0x34;
                r29 = r29 + 0x24;
                *(f32*)((u8*)r6 + 0x54) = f0;
                r27 = (u32)r26 >> 31;
                r27 = r26 + r27;
                r27 = r27 * 0x3;
                r27 = r12 - r27;
                r27 = r27 << 2;
                f0 = *(f32*)(r28 + r27);
                *(f32*)(r30 + r29) = f0;
            }
            r4 = r4 + 0x4;
            r5 = r5 + 0x2;
            r6 = r6 + 0x4;
            r12 = r12 + 0x1;
        } while (--ctr != 0);
        }
        r7 = r7 + 0xb4;
        r8 = r8 + 0x4;
        r31 = r31 + 0x1;
    } while ((s32)r31 < (s32)0x4);
    r5 = r11;
    r6 = 0x0;
    r0 = 0x4;
    ctr_fn = (void(*)(void))r0;
    do {
        r7 = r5 + 0x328;
        if ((s32)r3 == (s32)0x2) {
            if ((s32)r6 < (s32)0x2) {
                f0 = *(f32*)&lbl_8047BFBC;
                f1 = *(f32*)&lbl_8047BF60;
                *(f32*)((u8*)r7 + 0x4) = f0;
                f0 = *(f32*)&lbl_8047BFC4;
                *(f32*)((u8*)r7 + 0x8) = f1;
                *(f32*)((u8*)r7 + 0x0) = f0;
            } else {
            f0 = *(f32*)&lbl_8047BFC0;
            f1 = *(f32*)&lbl_8047BF60;
            *(f32*)((u8*)r7 + 0x4) = f0;
            f0 = *(f32*)&lbl_8047BFC4;
            *(f32*)((u8*)r7 + 0x8) = f1;
            *(f32*)((u8*)r7 + 0x0) = f0;
            }
        } else {
        r4 = (u32)r6 >> 31;
        r0 = r6 & 0x1;
        r0 = r0 ^ r4;
        r0 = r0 - r4;
        if ((s32)r0 != (s32)0x0) {
            f0 = *(f32*)&lbl_8047BFC0;
            f1 = *(f32*)&lbl_8047BF60;
            *(f32*)((u8*)r7 + 0x4) = f0;
            f0 = *(f32*)&lbl_8047BFC4;
            *(f32*)((u8*)r7 + 0x8) = f1;
            *(f32*)((u8*)r7 + 0x0) = f0;
        } else {
        f0 = *(f32*)&lbl_8047BFBC;
        f1 = *(f32*)&lbl_8047BF60;
        *(f32*)((u8*)r7 + 0x4) = f0;
        f0 = *(f32*)&lbl_8047BFC4;
        *(f32*)((u8*)r7 + 0x8) = f1;
        *(f32*)((u8*)r7 + 0x0) = f0;
        }
        }
        r5 = r5 + 0xc;
        r6 = r6 + 0x1;
    } while (--ctr != 0);
    f4 = *(f32*)&lbl_8047BF70;
    r3 = (u32)&lbl_803A9A60;
    f3 = *(f32*)&lbl_8047BF90;
    r3 = (u32)&lbl_803A9A60;
    f2 = *(f32*)&lbl_8047BF60;
    r0 = 0x0;
    f1 = *(f32*)&lbl_8047BFBC;
    f0 = *(f32*)&lbl_8047BFC0;
    *(u8*)((u8*)r3 + 0x368) = r0;
    *(f32*)((u8*)r11 + 0x358) = f4;
    *(f32*)((u8*)r11 + 0x35C) = f3;
    *(f32*)((u8*)r3 + 0x360) = f4;
    *(f32*)((u8*)r3 + 0x364) = f3;
    *(f32*)((u8*)r3 + 0x54) = f2;
    *(f32*)((u8*)r3 + 0x4C) = f1;
    *(f32*)((u8*)r3 + 0x44) = f2;
    *(f32*)((u8*)r3 + 0x50) = f2;
    *(f32*)((u8*)r3 + 0x48) = f0;
    *(f32*)((u8*)r3 + 0x40) = f2;
    return;
}
#endif


/* 0x800626CC | size: 0x168 */
#if 1
asm void _menuCBBattleStartDispTrainerTexCallBack__FlPvl(void) {
#include "src/game/ui/ui_core_fn_800626CC.inc"
}
#else
void _menuCBBattleStartDispTrainerTexCallBack__FlPvl(void) {
    extern void fn_800F92D4();
    extern void fn_8017B000();
    extern void fn_8025D2D4();
    extern void fn_8025D364();
    extern s32 fn_8025DA88();
    extern void _menuCBBattleStartDispTrainerTexCallBack__FlPvl();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r27 = r4;
    r30 = 0x1;
    if (r27 != (u32)0x0) {
        r3 = *(u32*)((u8*)r27 + 0x4);
        fn_800F92D4();
        *(u32*)((u8*)r27 + 0x0) = r3;
    }
    r3 = (u32)&lbl_803A9A60;
    r31 = (u32)&lbl_803A9A60;
    do {
        r3 = *(u32*)((u8*)r31 + 0x2C);
        r0 = *(u32*)((u8*)r31 + 0x30);
        if ((s32)r3 == (s32)r0) {
            r30 = 0x0;
        } else {
        fn_8025DA88();
        if ((s32)r3 != (s32)0x2) {
            r0 = *(u32*)((u8*)r31 + 0x2C);
            r3 = (u32)r0 >> 31;
            r0 = r0 & 0x1;
            r0 = r0 ^ r3;
            r0 = r0 - r3;
            if ((s32)r0 != (s32)0x0) {
                r3 = 0x1;
                r4 = 0x0;
                fn_8025D364();
                r28 = r3;
            } else {
                r3 = 0x0;
                r4 = 0x1;
                fn_8025D364();
                r28 = r3;
            }
        } else {
            r3 = *(u32*)((u8*)r31 + 0x2C);
            if ((s32)r3 < (s32)0x2) {
                r4 = 0x1;
            } else {
                r4 = 0x0;
            }
            fn_8025D2D4();
            r28 = r3;
        }
        fn_8025DA88();
        if ((s32)r3 != (s32)0x2) {
            r27 = 0x5c3;
        } else {

            r27 = 0x5c4;
        }
        r4 = *(u32*)((u8*)r31 + 0x2C);
        r3 = (u32)&lbl_803A9A60;
        r0 = (u32)&lbl_803A9A60;
        r3 = r4 << 3;
        r3 = r0 + r3;
        r29 = r3 + 0xc;
        *(u32*)((u8*)r29 + 0x4) = r28;
        r3 = *(u32*)((u8*)r29 + 0x4);
        fn_800F92D4();
        *(u32*)((u8*)r29 + 0x0) = r3;
        r0 = *(u32*)((u8*)r29 + 0x0);
        if (r0 == (u32)0x0) {
        r4 = (u32)_menuCBBattleStartDispTrainerTexCallBack__FlPvl;
        r3 = r27;
        r5 = (u32)_menuCBBattleStartDispTrainerTexCallBack__FlPvl;
        r6 = r29;
        r4 = r28;
        r7 = r28;
        fn_8017B000();
        r30 = 0x0;
        }
        r3 = *(u32*)((u8*)r31 + 0x2C);
        r0 = r3 + 0x1;
        *(u32*)((u8*)r31 + 0x2C) = r0;
        }
    } while ((s32)r30 != (s32)0x0);
    return;
}
#endif


/* 0x80062834 | size: 0x114 */
#if 1
asm void fn_80062834(void) {
#include "src/game/ui/ui_core_fn_80062834.inc"
}
#else
void fn_80062834(void) {
    extern void fn_800F915C();
    extern void fn_800F9210();
    extern void fn_8017B1CC();
    extern s32 fn_8025DA88();
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

    
    fn_8025DA88();
    if ((s32)r3 != (s32)0x2) {
        r29 = 0x5c3;
    } else {

        r29 = 0x5c4;
    }
    r3 = r29;
    fn_8017B1CC();
    r3 = r29;
    fn_800F915C();
    r3 = (u32)&lbl_803A9A60;
    r3 = (u32)&lbl_803A9A60;
    r30 = r3 + 0xc;
    r28 = r30;
    fn_8025DA88();
    if ((s32)r3 == (s32)0x2) {
        r3 = (u32)&lbl_803A9A60;
        r0 = 0x4;
        r3 = (u32)&lbl_803A9A60;
        *(u32*)((u8*)r3 + 0x30) = r0;
    } else {

        r3 = (u32)&lbl_803A9A60;
        r0 = 0x2;
        r3 = (u32)&lbl_803A9A60;
        *(u32*)((u8*)r3 + 0x30) = r0;
    }
    r3 = (u32)&lbl_803A9A60;
    r27 = 0x0;
    r31 = (u32)&lbl_803A9A60;
    while (1) {
        r0 = *(u32*)((u8*)r31 + 0x30);
        if ((s32)r27 >= (s32)r0) break;
        r4 = *(u32*)((u8*)r28 + 0x4);
        r3 = r29;
        fn_800F9210();
        r28 = r28 + 0x8;
        r27 = r27 + 0x1;
    }
    r3 = (u32)&lbl_803A9A60;
    r0 = 0x0;
    r3 = (u32)&lbl_803A9A60;
    *(u8*)((u8*)r3 + 0x34) = r0;
    *(u32*)((u8*)r3 + 0x2C) = r0;
    fn_8025DA88();
    if ((s32)r3 == (s32)0x2) {
        r0 = 0x4;
        *(u32*)((u8*)r31 + 0x30) = r0;
    } else {

        r0 = 0x2;
        *(u32*)((u8*)r31 + 0x30) = r0;
    }
    r0 = 0x0;
    *(u32*)((u8*)r30 + 0x4) = r0;
    *(u32*)((u8*)r30 + 0x0) = r0;
    *(u32*)((u8*)r30 + 0xC) = r0;
    *(u32*)((u8*)r30 + 0x8) = r0;
    *(u32*)((u8*)r30 + 0x14) = r0;
    *(u32*)((u8*)r30 + 0x10) = r0;
    *(u32*)((u8*)r30 + 0x1C) = r0;
    *(u32*)((u8*)r30 + 0x18) = r0;
    return;
}
#endif


/* 0x80062948 | size: 0x16C */
#if 1
asm void fn_80062948(void) {
#include "src/game/ui/ui_core_fn_80062948.inc"
}
#else
void fn_80062948(void) {
    extern void fn_80061028();
    extern void fn_80061F6C();
    extern void fn_80062834();
    extern void fn_80062AB4();
    extern void fn_80063060();
    extern void fn_80069944();
    extern void _threadSwitch();
    extern void fn_80102568();
    extern void fn_8010264C();
    extern void fn_801070F4();
    extern void fn_801080CC();
    extern void fn_8025D788();
    extern void fn_8025D9A8();
    extern s32 fn_8025DA88();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    r4 = 0x1;
    fn_80061F6C();
    fn_8025D9A8();
    r31 = r3;
    fn_8025DA88();
    switch ((s32)r31) {
    case 0x0:
    case 0x1:
        r3 = r30;
        fn_80063060();
        r31 = r3;
        goto L_80062A28;
    case 0x3:
        r3 = 0xdf;
        r4 = 0x0;
        fn_8010264C();
        r3 = 0xba;
        r4 = 0x1;
        fn_8010264C();
        r3 = 0x106;
        r4 = 0x1;
        fn_8010264C();
        r0 = *(u32*)((u8*)r30 + 0x4);
        if (((s32)r0 != (s32)0x2) && ((s32)r3 > (s32)0x0)) {

            r3 = r3 + 0x1;
        }
        if ((s32)r3 != (s32)0x0) {
            r31 = -0x1;
        } else {
            fn_8025D788();
            r31 = 0xd1;
        }
        r3 = 0x106;
        r4 = 0x0;
        r5 = 0x1;
        fn_80102568();
        goto L_80062A28;
    case 0x2:
    default:
        r3 = r30;
        fn_80062AB4();
        r31 = r3;
        break;
    }
    L_80062A28: ;
    r3 = 0xdf;
    r4 = 0x1c6;
    fn_801080CC();
    r3 = 0xba;
    r4 = 0x1c6;
    fn_801080CC();
    while (r0 != (u32)0x0) {
            r3 = 0xdf;
            fn_801070F4();
            r0 = r3 & 0xFF;
            _threadSwitch();
    }
    while (r0 != (u32)0x0) {
            r3 = 0xba;
            fn_801070F4();
            r0 = r3 & 0xFF;
            _threadSwitch();
    }
    fn_80069944();
    fn_80062834();
    r3 = 0x1;
    fn_80061028();
    r3 = 0xdf;
    r4 = 0x0;
    r5 = 0x1;
    fn_80102568();
    r3 = r31;
    return;
}
#endif


/* 0x80062AB4 | size: 0x5AC */
#if 1
asm void fn_80062AB4(void) {
#include "src/game/ui/ui_core_fn_80062AB4.inc"
}
#else
void fn_80062AB4(void) {
    extern void fn_8008ABA0();
    extern void _threadSwitch();
    extern void fn_800F7C28();
    extern void fn_800F7EF8();
    extern void fn_80102568();
    extern void fn_8010264C();
    extern void fn_801026A4();
    extern void fn_80103CC0();
    extern void fn_801046B8();
    extern void fn_801069FC();
    extern void fn_80106D3C();
    extern void fn_80132A38();
    extern void fn_801EF634();
    extern void fn_8025D2B0();
    extern void fn_8025D788();
    extern void fn_8025D9A8();
    extern s32 fn_8025D9F0();
    extern void fn_8025DA3C();
    extern s32 fn_8025DA88();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
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

    
    r28 = r3;
    fn_801EF634();
    r27 = r3;
    r25 = 0x0;
    r3 = 0x0;
    fn_80103CC0();
    r3 = 0xdf;
    r4 = 0x0;
    fn_8010264C();
    r3 = 0xba;
    r4 = 0x1;
    fn_8010264C();
    r0 = r27 & 0xFFFF;
    if ((s32)r0 != (s32)0x1) {
        goto L_80062CC8;
    }
    r3 = 0x1;
    fn_80103CC0();
    fn_8025DA88();
    if ((s32)r3 == (s32)0x2) {
        r3 = 0x0;
        fn_8025D9F0();
        r0 = r3 & 0xFFFF;
        if ((s32)r0 != (s32)0x0) {
            r0 = 0x1;
            goto L_80062B40;
        }
        r0 = 0x0;

    } else {
    r0 = 0x0;
    }
    L_80062B40: ;
    r0 = r0 & 0xFFFF;
    if (r0 == (u32)0x0) {
        fn_8025DA3C();
        r29 = r3;
        fn_8025D9A8();
        r30 = 0x0;
        while ((s32)r30 < (s32)r29) {

            r3 = r30;
            fn_8025D9F0();
            r27 = r3;
            r3 = r30;
            fn_8025D2B0();
            r31 = r3;
            do {
                if ((s32)r31 == (s32)0x0) break;
                r0 = r27 & 0xFFFF;
                if (r0 != (u32)0x1) {
                    if (r0 != (u32)0x2) break;
                }
                r3 = r31;
                fn_8008ABA0();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x0) {
                    goto L_80062BBC;
            }
            } while (0);
            r30 = r30 + 0x1;

        }
        r31 = 0x2;
        L_80062BBC: ;
        r4 = r31;
        r3 = 0x30;
        fn_80132A38();
        r3 = 0x2;
        r4 = 0x44dc;
        r5 = 0x1;
        r6 = 0x1;
        fn_80106D3C();
        r3 = 0x1;
        fn_801069FC();
        r25 = 0x1;

    } else {
    fn_8025DA3C();
    r29 = r3;
    fn_8025D9A8();
    r30 = 0x0;
    while ((s32)r30 < (s32)r29) {

        r3 = r30;
        fn_8025D9F0();
        r27 = r3;
        r3 = r30;
        fn_8025D2B0();
        do {
            if ((s32)r3 == (s32)0x0) break;
            r0 = r27 & 0xFFFF;
            if (r0 != (u32)0x1) {
                if (r0 != (u32)0x2) break;
            }
            fn_8008ABA0();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x0) break;
        } while (0);
        r30 = r30 + 0x1;

    }

    r3 = 0x2;
    r4 = 0x44e7;
    r5 = 0x1;
    r6 = 0x1;
    fn_80106D3C();
    r27 = 0x1;
    do {
        r3 = 0x1;
        fn_800F7EF8();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x0) {
            r3 = 0x1;
            fn_800F7C28();
            if ((s32)r3 == (s32)0x0) {
                r0 = 0x1;
                goto L_80062C9C;
            }
            r0 = 0x0;

        } else {
        r0 = 0x0;
        }
        L_80062C9C: ;
        r0 = r0 & 0xFF;
        if (r0 != (u32)0x0) {
            r27 = 0x0;
        } else {

            _threadSwitch();
        }
    } while ((s32)r27 != (s32)0x0);
    r3 = 0x1;
    fn_801069FC();
    r25 = 0x1;
    }
    L_80062CC8: ;
    r0 = r25 & 0xFF;
    if (r0 != (u32)0x0) {
        r3 = 0xb3;
        return;
    }
    r0 = *(u32*)((u8*)r28 + 0x4);
    if ((s32)r0 != (s32)0x2) {
        r25 = 0xd4;
    } else {

        r25 = 0xd5;
    }
    r3 = 0x1;
    fn_80103CC0();
    r3 = 0x2;
    r4 = 0x3c20;
    r5 = 0x1;
    r6 = 0x1;
    fn_80106D3C();
    fn_801046B8();
    r31 = r25 & 0xFFFF;
    r4 = r3;
    r3 = r31;
    r5 = 0x0;
    r6 = 0x8;
    r7 = 0x1;
    r8 = 0x0;
    /* crclr cr1eq */;
    fn_801026A4();
    r0 = *(u32*)((u8*)r28 + 0x4);
    if (((s32)r0 != (s32)0x2) && ((s32)r3 > (s32)0x0)) {

        r3 = r3 + 0x1;
    }
    switch ((s32)r3) {
    case 0x0:
        fn_8025D788();
        r30 = 0xd1;
        break;
    case 0x1:
        r30 = 0xb5;
        break;
    case 0x2:
        r30 = 0xb3;
        break;
    case -0x1:
    default:
        r30 = -0x1;
        break;
    }
    r3 = 0x1;
    fn_801069FC();
    if ((s32)r30 != (s32)-0x1) {
        if ((s32)r30 == (s32)0xb3) {
        }
        r25 = 0x0;
        r29 = 0x1;
        fn_8025DA88();
        if ((s32)r3 == (s32)0x2) {
            r3 = 0x0;
            fn_8025D9F0();
            r0 = r3 & 0xFFFF;
            if ((s32)r0 != (s32)0x0) {
                r0 = 0x1;
                goto L_80062DEC;
            }
            r0 = 0x0;

        } else {
        r0 = 0x0;
        }
        L_80062DEC: ;
        r28 = r0 & 0xFFFF;
        do {
            switch ((s32)r25) {
            case 0x0:
                if ((s32)r28 == (s32)0x0) {
                    r25 = 0x1;
                    break;
                }
                r25 = 0x2;
                break;
            case 0x1:
                r3 = r31;
                r4 = 0x0;
                r5 = 0x1;
                fn_80102568();
                r3 = 0x2;
                r4 = 0x4446;
                r5 = 0x1;
                r6 = 0x1;
                fn_80106D3C();
                r25 = 0x1;
                do {
                    r26 = 0x0;
                    do {
                        r3 = r26;
                        fn_8025D9F0();
                        r27 = r3;
                        r3 = r26;
                        fn_8025D2B0();
                        do {
                            if ((s32)r3 == (s32)0x0) break;
                            r0 = r27 & 0xFFFF;
                            if (r0 != (u32)0x1) {
                                if (r0 != (u32)0x2) break;
                            }
                            fn_8008ABA0();
                            r0 = r3 & 0xFF;
                            if (r0 != (u32)0x0) {
                                r0 = 0x0;
                                goto L_80062EB8;
                    }
                        } while (0);
                        r26 = r26 + 0x1;
                    } while ((s32)r26 < (s32)0x4);
                    r0 = 0x1;
                    L_80062EB8: ;
                    r0 = r0 & 0xFF;
                    if (r0 != (u32)0x0) {
                        r25 = 0x0;
                    }
                    if ((s32)r25 != (s32)0x0) {
                        _threadSwitch();
                    }
                } while ((s32)r25 != (s32)0x0);
                r3 = 0x1;
                fn_801069FC();
                r25 = 0x4;
                break;
            case 0x2:
                r3 = r31;
                r4 = 0x0;
                r5 = 0x1;
                fn_80102568();
                r3 = 0x2;
                r4 = 0x4445;
                r5 = 0x1;
                r6 = 0x1;
                fn_80106D3C();
                r25 = 0x1;
                do {
                    r26 = 0x0;
                    do {
                        r3 = r26;
                        fn_8025D9F0();
                        r27 = r3;
                        r3 = r26;
                        fn_8025D2B0();
                        do {
                            if ((s32)r3 == (s32)0x0) break;
                            r0 = r27 & 0xFFFF;
                            if (r0 != (u32)0x1) {
                                if (r0 != (u32)0x2) break;
                            }
                            fn_8008ABA0();
                            r0 = r3 & 0xFF;
                            if (r0 != (u32)0x0) {
                                r0 = 0x0;
                                goto L_80062F70;
                }
                        } while (0);
                        r26 = r26 + 0x1;
                    } while ((s32)r26 < (s32)0x4);
                    r0 = 0x1;
                    L_80062F70: ;
                    r0 = r0 & 0xFF;
                    if (r0 != (u32)0x0) {
                        r25 = 0x0;
                    }
                    if ((s32)r25 != (s32)0x0) {
                        _threadSwitch();
                    }
                } while ((s32)r25 != (s32)0x0);
                r3 = 0x1;
                fn_801069FC();
                r25 = 0x3;
                break;
            case 0x3:
                r3 = r31;
                r4 = 0x0;
                r5 = 0x1;
                fn_80102568();
                r3 = 0x2;
                r4 = 0x44e2;
                r5 = 0x1;
                r6 = 0x1;
                fn_80106D3C();
                r26 = 0x1;
                do {
                    r3 = 0x1;
                    fn_800F7EF8();
                    r0 = r3 & 0xFF;
                    if (r0 != (u32)0x0) {
                        r3 = 0x1;
                        fn_800F7C28();
                        if ((s32)r3 == (s32)0x0) {
                            r0 = 0x1;
                            goto L_80063004;
                        }
                        r0 = 0x0;

                    } else {
                    r0 = 0x0;
                    }
                    L_80063004: ;
                    r0 = r0 & 0xFF;
                    if (r0 != (u32)0x0) {
                        r26 = 0x0;
                    } else {

                        _threadSwitch();
                    }
                } while ((s32)r26 != (s32)0x0);
                r3 = 0x1;
                fn_801069FC();
                r25 = 0x4;
                break;
            case 0x4:
                r29 = 0x0;
                break;
            default:
                break;
            }
        } while ((s32)r29 != (s32)0x0);
        }
    r0 = 0x0;
    r3 = r30;
    *(u32*)&lbl_8047A5D0 = r0;

    return;
}
#endif


/* 0x80063060 | size: 0x750 */
#if 1
asm void fn_80063060(void) {
#include "src/game/ui/ui_core_fn_80063060.inc"
}
#else
void fn_80063060(void) {
    extern void fn_80061028();
    extern void fn_80062284();
    extern void fn_80062834();
    extern void fn_800637B0();
    extern void fn_80069944();
    extern void fn_80088D84();
    extern void _threadSwitch();
    extern void fn_80102568();
    extern void fn_8010264C();
    extern void fn_801026A4();
    extern void fn_80102868();
    extern void fn_801043A4();
    extern void fn_801045A8();
    extern void fn_801046B8();
    extern void fn_801069FC();
    extern void fn_80106D3C();
    extern void fn_801070F4();
    extern void fn_801080CC();
    extern void fn_80129280();
    extern void fn_8012A7C4();
    extern void fn_8012A7DC();
    extern void fn_8012A80C();
    extern void fn_8012A824();
    extern void fn_80132A38();
    extern void fn_80166AB8();
    extern void fn_801906A0();
    extern void fn_801EE398();
    extern void fn_801EF634();
    extern void fn_8025D06C();
    extern void fn_8025D164();
    extern void fn_8025D9A8();
    extern void fn_8025DAF4();
    extern void fn_8025DB2C();
    extern void fn_8025DB5C();
    extern void fn_8025DB80();
    extern void fn_8025DBB0();
    extern u8 jumptable_802ED9B8[];
    u8 sp[0x50];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r16 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
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
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r31 = r3;
    r16 = 0x0;
    fn_801EF634();
    r18 = r3;
    r27 = -0x1;
    r26 = 0x1;
    fn_8025D9A8();
    r29 = r3;
    fn_8025DBB0();
    r30 = r3;
    r25 = 0x0;
    r24 = 0x0;
    r23 = 0x0;
    r22 = 0x1;
    r20 = 0x1;
    r21 = 0x0;
    r3 = 0x0;
    r4 = 0x2;
    fn_80129280();
    r17 = r3;
    fn_8012A80C();
    r0 = r3;
    r3 = r17;
    r17 = r0;
    fn_8012A7C4();
    r4 = (0x6666 << 16);
    r5 = r30 + 0x1;
    r0 = r4 + 0x6667;
    r19 = r3;
    r0 = (s32)((s64)r0 * (s64)r5 >> 32);
    r28 = r18 & 0xFFFF;
    r0 = (s32)r0 >> 2;
    r3 = (u32)r0 >> 31;
    r0 = r0 + r3;
    r0 = r0 * 0xa;
    r18 = r5 - r0;
    do {
        do {
            if (r16 > (u32)0xc) break;
            r3 = (u32)jumptable_802ED9B8;
            r0 = r16 << 2;
            r3 = (u32)jumptable_802ED9B8;
            r0 = *(u32*)(r3 + r0);
            ctr_fn = (void(*)(void))r0;
            /* indirect jump via ctr */;
            r3 = 0xdf;
            r4 = 0x0;
            fn_8010264C();
            r3 = 0xba;
            r4 = 0x1;
            fn_8010264C();
            do {
            if ((s32)r28 == (s32)0x5) break;
                if ((s32)r28 < (s32)0x5) {
                    if ((s32)r28 == (s32)0x2) break;
                    if ((s32)r28 >= (s32)0x2) goto L_80063260;
                    goto L_80063290;
                }
                if ((s32)r28 >= (s32)0x8) goto L_80063290;
                goto L_80063260;
            } while (0);
            if ((s32)r29 != (s32)0x1) {
                if ((s32)r29 >= (s32)0x1) goto L_800631C0;
                if ((s32)r29 < (s32)0x0) {
                    goto L_800631C0;
                }
                if ((s32)r30 == (s32)0x7) {
                    fn_8025D164();
                    ((void(*)(void))fn_8006ADB4)();
                    r25 = 0x1;
                    fn_800637B0();
                }

            } else {
            if ((s32)r18 == (s32)0x0) {
                fn_8025D164();
                r16 = r3;
                ((void(*)(void))fn_8006ADEC)();
                r3 = r16 + r3;
                ((void(*)(void))fn_8006ADB4)();
            }
            r0 = r30 + 0x1;
            if ((s32)r0 == (s32)0x64) {
                r25 = 0x1;
        }
            }
            L_800631C0: ;
            r0 = r25 & 0xFF;
            if (r0 != (u32)0x0) {
                if ((s32)r29 == (s32)0x1) {
                    r3 = 0xafd;
                    fn_801906A0();
                    if (r3 == (u32)0x0) {
                        fn_801EE398();
                        r21 = r3;
                }
                }
                r16 = 0x5;
                break;
            }
            r3 = 0x0;
            fn_80062284();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x0) {
                r3 = 0x2;
                r4 = 0x3c10;
                r5 = 0x1;
                r6 = 0x1;
                fn_80106D3C();
                r3 = 0x1;
                fn_801069FC();
                fn_8025DB2C();
                r3 = 0x2;
                r4 = 0x30dd;
                r5 = 0x0;
                r6 = 0x1;
                fn_80106D3C();
                r16 = 0x1;
                break;
            }
            r3 = 0x2;
            r4 = 0x30dd;
            r5 = 0x0;
            r6 = 0x1;
            fn_80106D3C();
            r16 = 0x1;
            break;
            L_80063260: ;
            fn_8025DB5C();
            if ((s32)r3 == (s32)0x0) {
                if ((s32)r29 == (s32)0x1) {
                    r27 = 0x105;
                } else {

                    r27 = 0xac;
                }
                r16 = 0x9;
                break;
            }
            r16 = 0x2;
            break;
            L_80063290: ;
            r3 = 0x2;
            r4 = 0x3da4;
            r5 = 0x0;
            r6 = 0x1;
            fn_80106D3C();
            r16 = 0x9;
            break;
            fn_801046B8();
            r4 = r3;
            r3 = 0xec;
            r5 = 0x0;
            r6 = 0x8;
            r7 = 0x0;
            r8 = 0x0;
            /* crclr cr1eq */;
            fn_801026A4();
            /* lha r4, lbl_80478920(r13) */;
            r3 = 0xec;
            /* lha r5, lbl_80478922(r13) */;
            fn_80102868();
            r3 = 0xec;
            r4 = 0x1;
            fn_801045A8();
            r3 = 0xec;
            fn_801043A4();
            r16 = (s8)r3;
            r3 = 0xec;
            r4 = 0x0;
            r5 = 0x1;
            fn_80102568();
            r3 = 0x1;
            fn_801069FC();
            if ((s32)r16 == (s32)0x0) {
                r0 = r22 & 0xFF;
                r24 = 0x1;
                if (r0 != (u32)0x0) {
                    fn_8025DB80();
                }
                r27 = 0xd1;
                r23 = 0x0;
                r16 = 0x9;
                break;
            }
            r0 = r22 & 0xFF;
            r24 = 0x1;
            if (r0 != (u32)0x0) {
                fn_8025DB80();
            }
            r16 = 0x9;
            r23 = 0x1;
            r27 = 0xac;
            break;
            r3 = 0x2;
            r4 = 0x44e3;
            r5 = 0x0;
            r6 = 0x1;
            fn_80106D3C();
            /* lha r4, lbl_80478920(r13) */;
            r3 = 0x0;
            /* lha r5, lbl_80478922(r13) */;
            r6 = 0x1;
            ((void(*)(void))fn_8001E074)();
            r16 = (s8)r3;
            r3 = 0x1;
            fn_801069FC();
            if ((s32)r16 == (s32)0x0) {
                r16 = 0xc;
                break;
            }
            r16 = 0x1;
            break;
            fn_8025DB5C();
            r4 = r3;
            r3 = 0x30;
            fn_80132A38();
            r3 = 0x2;
            r4 = 0x3c13;
            r5 = 0x0;
            r6 = 0x1;
            fn_80106D3C();
            /* lha r4, lbl_80478920(r13) */;
            r3 = 0x0;
            /* lha r5, lbl_80478922(r13) */;
            r6 = 0x0;
            ((void(*)(void))fn_8001E074)();
            r16 = (s8)r3;
            r3 = 0x1;
            fn_801069FC();
            if ((s32)r16 == (s32)0x0) {
                fn_8025DAF4();
                r16 = 0x9;
                r27 = 0xd1;
                break;
            }
            r16 = 0x3;
            break;
            fn_8025DB5C();
            r4 = r3;
            r3 = 0x30;
            fn_80132A38();
            r3 = 0x2;
            r4 = 0x44df;
            r5 = 0x0;
            r6 = 0x1;
            fn_80106D3C();
            /* lha r4, lbl_80478920(r13) */;
            r3 = 0x0;
            /* lha r5, lbl_80478922(r13) */;
            r6 = 0x1;
            ((void(*)(void))fn_8001E074)();
            r16 = (s8)r3;
            r3 = 0x1;
            fn_801069FC();
            if ((s32)r16 == (s32)0x0) {
                r16 = 0x9;
                r27 = 0xac;
                break;
            }
            r16 = 0x2;
            break;
            ((void(*)(void))fn_8006ADEC)();
            r0 = r3;
            r3 = 0x30;
            r4 = r0;
            fn_80132A38();
            r3 = 0x3cc;
            r4 = 0x0;
            r5 = 0x0;
            fn_80166AB8();
            r3 = 0x2;
            r4 = 0x3c11;
            r5 = 0x1;
            r6 = 0x1;
            fn_80106D3C();
            r3 = 0x0;
            ((void(*)(void))fn_8006B09C)();
            ((void(*)(void))fn_8006A7D0)();
            ((void(*)(void))fn_8006AC6C)();
            r0 = r21 & 0xFF;
            r3 = r3 & 0xFFFF;
            if (r0 != (u32)0x0) {
                r16 = 0x8;
                break;
            }
            if ((s32)r3 != (s32)0x0) {
                if ((s32)r3 >= (s32)0x0 && (s32)r3 < (s32)0x3) {


                } else {
                r16 = 0x6;
                break;
                }
                r27 = 0x105;
                r16 = 0xc;
                } else {

                r16 = 0x6;
                break;
                r3 = 0x2;
                r4 = 0x3c23;
                r5 = 0x0;
                r6 = 0x1;
                fn_80106D3C();
                /* lha r4, lbl_80478920(r13) */;
                r3 = 0x0;
                /* lha r5, lbl_80478922(r13) */;
                r6 = 0x0;
                ((void(*)(void))fn_8001E074)();
                r0 = (s8)r3;
                if ((s32)r0 == (s32)0x0) {
                    r27 = 0x105;
                    r16 = 0xc;
                    break;
                }
                r16 = 0xb;
                break;
                r3 = 0x2;
                r4 = 0x3c0f;
                r5 = 0x0;
                r6 = 0x1;
                fn_80106D3C();
                /* lha r4, lbl_80478920(r13) */;
                r3 = 0x0;
                /* lha r5, lbl_80478922(r13) */;
                r6 = 0x1;
                ((void(*)(void))fn_8001E074)();
                r0 = (s8)r3;
                if ((s32)r0 == (s32)0x0) {
                    r27 = 0xac;
                    r16 = 0xc;
                    break;
                }
                r16 = 0xa;
                break;
                r3 = 0x2;
                r4 = 0x3c03;
                r5 = 0x0;
                r6 = 0x1;
                fn_80106D3C();
                /* lha r4, lbl_80478920(r13) */;
                r3 = 0x0;
                /* lha r5, lbl_80478922(r13) */;
                r6 = 0x0;
                ((void(*)(void))fn_8001E074)();
                r16 = (s8)r3;
                r3 = 0x1;
                fn_801069FC();
                if ((s32)r16 == (s32)0x0) {
                    r0 = r20 & 0xFF;
                    r27 = 0xac;
                    r16 = 0x9;
                    if (r0 != (u32)0x0) {
                        fn_8025D06C();
                    }
                    r23 = 0x1;
                    break;
                }
                r16 = 0x7;
                break;
                r3 = 0x2;
                r4 = 0x3c41;
                r5 = 0x0;
                r6 = 0x1;
                fn_80106D3C();
                /* lha r4, lbl_80478920(r13) */;
                r3 = 0x0;
                /* lha r5, lbl_80478922(r13) */;
                r6 = 0x1;
                ((void(*)(void))fn_8001E074)();
                r0 = (s8)r3;
                if ((s32)r0 == (s32)0x0) {
                    r3 = 0x0;
                    r4 = 0x2;
                    fn_80129280();
                    r4 = r17;
                    r16 = r3;
                    fn_8012A824();
                    r3 = r16;
                    r4 = r19;
                    fn_8012A7DC();
                    r16 = 0xc;
                    break;
                }
                r16 = 0x6;
                break;
                r3 = 0x2;
                r4 = 0x3c12;
                r5 = 0x0;
                r6 = 0x1;
                fn_80106D3C();
                r3 = 0x1;
                fn_801069FC();
                r3 = 0xdf;
                r4 = 0x1c6;
                fn_801080CC();
                r3 = 0xba;
                r4 = 0x1c6;
                fn_801080CC();
                while (1) {

                    _threadSwitch();

                    r3 = 0xdf;
                    fn_801070F4();
                    r0 = r3 & 0xFF;
                        if (r0 == (u32)0x0) break;
            }
                while (1) {

                    _threadSwitch();

                    r3 = 0xba;
                    fn_801070F4();
                    r0 = r3 & 0xFF;
                        if (r0 == (u32)0x0) break;
            }
                fn_80069944();
                fn_80062834();
                r3 = 0x1;
                fn_80061028();
                r3 = 0xdf;
                r4 = 0x0;
                r5 = 0x1;
                fn_80102568();
                ((void(*)(void))fn_800886D0)();
                r27 = 0x105;
                r16 = 0x9;
                break;
                r0 = r23 & 0xFF;
                if (r0 != (u32)0x0) {
                    r0 = r24 & 0xFF;
                    if (r0 != (u32)0x0) {
                        r3 = 0x0;
                        ((void(*)(void))fn_800889E4)();
                        if ((s32)r3 < (s32)0x0) {
                            r16 = 0x4;
                            r22 = 0x0;
                            break;
                        }
                        r3 = r31;
                        r26 = 0x0;
                        ((void(*)(void))fn_80069C0C)();
                        break;
                    }
                    fn_80088D84();
                    if ((s32)r3 < (s32)0x0) {
                        r16 = 0x6;
                        r20 = 0x0;
                        break;
                    }
                    r3 = r31;
                    r26 = 0x0;
                    ((void(*)(void))fn_80069C0C)();
                    break;
                }
                r0 = r24 & 0xFF;
                r26 = 0x0;
                if (r0 != (u32)0x0) {
                    r3 = r31;
                    ((void(*)(void))fn_80069C0C)();
            }
                break;
                r26 = 0x0;
                }
        } while (0);
    } while ((s32)r26 != (s32)0x0);
    r3 = 0x1;
    fn_801069FC();
    r3 = r27;
    return;
}
#endif


/* 0x800637B0 | size: 0x144 */
#if 1
asm void fn_800637B0(void) {
#include "src/game/ui/ui_core_fn_800637B0.inc"
}
#else
void fn_800637B0(void) {
    extern s32 fn_8025DA88();
    extern void fn_8025DAAC();
    u8 sp[0x10];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    fn_8025DA88();
    r30 = r3;
    fn_8025DAAC();
    r0 = r3;
    r4 = r30;
    r31 = r0;
    ((void(*)(void))fn_8006B1F4)();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        r3 = r31;
        r4 = r30;
        ((void(*)(void))fn_8006B2A4)();
    }
    r3 = 0x3;
    ((void(*)(void))fn_8006B3C8)();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        r30 = 0x1;
        r31 = 0x0;
        do {
            r3 = r31;
            r4 = 0x0;
            ((void(*)(void))fn_8006B1F4)();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x0) {
                r30 = 0x0;
                break;
            }
            r3 = r31;
            r4 = 0x1;
            ((void(*)(void))fn_8006B1F4)();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x0) {
                r30 = 0x0;
                break;
            }
            r31 = r31 + 0x1;
        } while ((s32)r31 <= (s32)0x2);
        r0 = r30 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x3;
            ((void(*)(void))fn_8006B354)();
    }
    }
    r3 = 0x5;
    ((void(*)(void))fn_8006B3C8)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) return;
    r31 = 0x1;
    r3 = 0x4;
    r4 = 0x0;
    ((void(*)(void))fn_8006B1F4)();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        r31 = 0x0;
    } else {
        r3 = 0x4;
        r4 = 0x1;
        ((void(*)(void))fn_8006B1F4)();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x0) {
            r31 = 0x0;
        }
    }
    r0 = r31 & 0xFF;
    if (r0 != (u32)0x1) return;
    r3 = 0x5;
    ((void(*)(void))fn_8006B354)();

    return;
}
#endif


/* 0x800638F4 | size: 0x1E0 */
#if 1
asm void fn_800638F4(void) {
#include "src/game/ui/ui_core_fn_800638F4.inc"
}
#else
void fn_800638F4(void) {
    extern void fn_80063AD4();
    extern void fn_800FB680();
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;

    
    r0 = *(s16*)((u8*)r4 + 0x6);
    if ((s32)r0 == (s32)0xe08 || (s32)r0 == (s32)0xe17 || (s32)r0 == (s32)0x1264 || (s32)r0 == (s32)0x126f) {
        /* L_80063990 / L_80063998 equivalent */
        fn_80063AD4();
        return;
    }
    if ((s32)r0 == (s32)0x1123) {
        /* L_80063AC0 */
        fn_80063AD4();
        return;
    }
    if ((s32)r0 == (s32)0xe14 || (s32)r0 == (s32)0xe24) {
        /* L_800639A0 / L_80063A00: r6=0x3c21 */
        r5 = *(u8*)((u8*)r3 + 0x8B);
        r0 = -0x100;
        r3 = 0x0;
        r4 = 0x0;
        r5 = r5 | r0;
        r6 = 0x3c21;
        fn_800FB680();
        return;
    }
    if ((s32)r0 == (s32)0xe15 || (s32)r0 == (s32)0xe26) {
        /* L_800639C0: r6=0x3db2 */
        r5 = *(u8*)((u8*)r3 + 0x8B);
        r0 = -0x100;
        r3 = 0x0;
        r4 = 0x0;
        r5 = r5 | r0;
        r6 = 0x3db2;
        fn_800FB680();
        return;
    }
    if ((s32)r0 == (s32)0xe16 || (s32)r0 == (s32)0xe27 || (s32)r0 == (s32)0x1270) {
        /* L_800639E0 / L_80063A60 / L_80063AA0: r6=0x3db3 */
        r5 = *(u8*)((u8*)r3 + 0x8B);
        r0 = -0x100;
        r3 = 0x0;
        r4 = 0x0;
        r5 = r5 | r0;
        r6 = 0x3db3;
        fn_800FB680();
        return;
    }
    if ((s32)r0 == (s32)0xe25) {
        /* L_80063A20: r6=0x3dae */
        r5 = *(u8*)((u8*)r3 + 0x8B);
        r0 = -0x100;
        r3 = 0x0;
        r4 = 0x0;
        r5 = r5 | r0;
        r6 = 0x3dae;
        fn_800FB680();
        return;
    }

    return;
}
#endif


/* 0x80063AD4 | size: 0x23C */
#if 1
asm void fn_80063AD4(void) {
#include "src/game/ui/ui_core_fn_80063AD4.inc"
}
#else
void fn_80063AD4(void) {
    extern void fn_800D5648();
    extern void fn_800D5BA0();
    extern void fn_800D61E4();
    extern void fn_800D6728();
    extern void fn_800D67BC();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D888C();
    extern void fn_800D88DC();
    extern void fn_800FE35C();
    extern void fn_800FE38C();
    u8 sp[0x60];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f31 = 0.0f;

    
    *(f64*)(sp + 0x50) = f31;
    /* psq_st f31, 0x58((u32)sp), 0, qr0 */;
    r31 = r4;
    r4 = *(u8*)((u8*)r3 + 0x8B);
    r5 = (0x4330 << 16);
    r0 = *(u8*)((u8*)r31 + 0x67);
    r3 = (0x8102 << 16);
    r8 = *(u32*)&lbl_8047BFC8;
    /* subi r7, r3, 0x7dfd */;
    r6 = r4 * r0;
    r0 = *(u32*)&lbl_8047BFCC;
    r3 = 0x1;
    f4 = *(f64*)&lbl_8047BFD8;
    *(u32*)(sp + 0xC) = r0;
    r7 = (s32)((s64)r7 * (s64)r6 >> 32);
    r4 = *(u8*)(sp + 0xB);
    r0 = *(u8*)(sp + 0xF);
    f2 = *(f64*)&lbl_8047BFE0;
    r4 = r7 + r6;
    r4 = (s32)r4 >> 15;
    r6 = (u32)r4 >> 31;
    *(u32*)(sp + 0x2C) = r0;
    r0 = r4 + r6;
    f0 = *(f64*)(sp + 0x18);
    /* xoris r0, r0, 0x8000 */;
    f1 = f0 - f2;
    *(u32*)(sp + 0x14) = r0;
    f0 = *(f64*)(sp + 0x28);
    f3 = *(f64*)(sp + 0x10);
    f0 = f0 - f2;
    f31 = f3 - f4;
    f1 = f1 * f31;
    f0 = f0 * f31;
    f1 = (f64)(s32)f1;
    f0 = (f64)(s32)f0;
    *(f64*)(sp + 0x20) = f1;
    *(f64*)(sp + 0x30) = f0;
    *(u8*)(sp + 0xB) = r4;
    *(u8*)(sp + 0xF) = r0;
    fn_800D88DC();
    r3 = 0x6;
    fn_800D888C();
    r3 = 0x6;
    fn_800D6A00();
    r3 = (u32)&lbl_80314E08;
    r3 = (u32)&lbl_80314E08;
    fn_800D7820();
    r3 = 0x4;
    fn_800D67BC();
    r3 = 0x0;
    r4 = 0x0;
    fn_800D61E4();
    r3 = 0x0;
    fn_800D5BA0();
    r3 = *(s16*)((u8*)r31 + 0x54);
    r4 = 0x0;
    fn_800D61E4();
    r3 = 0x0;
    fn_800D5BA0();
    r3 = *(s16*)((u8*)r31 + 0x54);
    r4 = *(s16*)((u8*)r31 + 0x56);
    fn_800D61E4();
    r3 = 0x0;
    fn_800D5BA0();
    r4 = *(s16*)((u8*)r31 + 0x56);
    r3 = 0x0;
    fn_800D61E4();
    r3 = 0x0;
    fn_800D5BA0();
    fn_800D6728();
    r5 = *(s16*)((u8*)r31 + 0x54);
    r3 = 0x0;
    r6 = *(s16*)((u8*)r31 + 0x56);
    r4 = 0x0;
    fn_800FE38C();
    r3 = 0x1;
    fn_800D88DC();
    r3 = 0x6;
    fn_800D888C();
    f1 = *(f32*)&lbl_8047BFD0;
    fn_800D5648();
    r3 = 0x1;
    fn_800D6A00();
    r3 = (u32)&lbl_80314E08;
    r3 = (u32)&lbl_80314E08;
    fn_800D7820();
    f0 = *(f32*)&lbl_8047BFD4;
    r0 = 0xff;
    *(u8*)(sp + 0x8) = r0;
    r30 = 0x0;
    f0 = f0 * f31;
    *(u8*)(sp + 0x9) = r0;
    f0 = (f64)(s32)f0;
    *(u8*)(sp + 0xA) = r0;
    *(f64*)(sp + 0x38) = f0;
    *(u8*)(sp + 0xB) = r0;

    while ((s32)r3 < (s32)r0) {
        r3 = 0x2;
        fn_800D67BC();
        r4 = r30;
        r3 = 0x0;
        fn_800D61E4();
        r3 = 0x0;
        fn_800D5BA0();
        r3 = *(s16*)((u8*)r31 + 0x54);
        r4 = r30;
        fn_800D61E4();
        r3 = 0x0;
        fn_800D5BA0();
        fn_800D6728();
        r30 = r30 + 0x4;

    r0 = *(s16*)((u8*)r31 + 0x56);
    r3 = (s16)r30;
    }
    fn_800FE35C();
    /* psq_l f31, 0x58((u32)sp), 0, qr0 */;
    f31 = *(f64*)(sp + 0x50);
    return;
}
#endif


/* 0x80063D10 | size: 0x4 */
#if 0
asm void fn_80063D10(void) {
#include "src/game/ui/ui_core_fn_80063D10.inc"
}
#else
void fn_80063D10(void) {
}
#endif

/* 0x80063D14 | size: 0x664 */
#if 1
asm void fn_80063D14(void) {
#include "src/game/ui/ui_core_fn_80063D14.inc"
}
#else
void fn_80063D14(void) {
    extern void fn_80062834();
    extern void fn_8008AB20();
    extern void fn_8008AB4C();
    extern void _threadSwitch();
    extern void fn_800F7C28();
    extern void fn_800F7EF8();
    extern void fn_80102568();
    extern void fn_8010264C();
    extern void fn_80103CC0();
    extern void fn_801069FC();
    extern void fn_80106D3C();
    extern void fn_80132A38();
    extern void fn_80165A20();
    extern void fn_8025D3F4();
    extern void fn_8025D744();
    extern s32 fn_8025D89C();
    extern void fn_8025D9CC();
    extern s32 fn_8025D9F0();
    extern void fn_8025DA3C();
    extern s32 fn_8025DA88();
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
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
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r22 = r3;
    r25 = 0x1;
    r3 = 0x1e;
    r4 = 0x0;
    r5 = 0xff;
    fn_80165A20();
    r3 = (u32)&lbl_803A9F08;
    r5 = (0x1 << 16);
    r3 = (u32)&lbl_803A9F08;
    r4 = r22;
    r3 = r3 + 0x150;
    /* subi r5, r5, 0x33d4 */;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    fn_8025DA88();
    r30 = r3;
    fn_8025DA3C();
    r4 = (u32)&lbl_803A9F08;
    r0 = -0x1;
    r27 = (u32)&lbl_803A9F08;
    f0 = *(f32*)&lbl_8047BFE8;
    r4 = 0x1;
    r29 = 0x0;
    r26 = r27 + (0x1 << 16);
    r31 = r3;
    *(u8*)((u8*)r26 + (-12712)) = r4;
    r28 = r27;
    *(u32*)((u8*)r26 + (-12708)) = r0;
    *(f32*)((u8*)r26 + (-12724)) = f0;
    *(u32*)((u8*)r26 + (-12720)) = r29;
    *(u32*)((u8*)r27 + 0x2C) = r29;
    *(u32*)((u8*)r26 + (-12928)) = r29;
    *(u32*)((u8*)r27 + 0x0) = r29;
    *(u32*)((u8*)r27 + 0xC) = r29;
    while ((s32)r29 < (s32)r31) {

        r3 = r29;
        ((void(*)(void))fn_8006B09C)();
        r24 = r3;
        ((void(*)(void))fn_8006A814)();
        r22 = r3;
        r3 = r29;
        ((void(*)(void))fn_8006B0F8)();
        r0 = 0x0;
        r4 = r3;
        *(u8*)((u8*)r28 + 0x4) = r0;
        r0 = *(u32*)((u8*)r24 + 0x4);
        if ((s32)r0 != (s32)0x0) {
            r3 = r22;
            fn_8008AB4C();
            if ((s32)r30 == (s32)0x1) {
                r24 = 0x2;
            } else {

                r24 = 0x1;
            }
            ((void(*)(void))fn_8006B1D4)();
            r23 = r3 & 0xFFFF;
            r3 = r29;
            fn_8025D89C();
            r0 = r3 & 0xFFFF;
            r3 = r22;
            if (r0 < r23) {
            } else {

                r0 = r23;
            }
            r5 = r24;
            r4 = r0 & 0xFFFF;
            fn_8008AB20();
        }
        r28 = r28 + 0x1;
        r29 = r29 + 0x1;

    }
    r22 = 0x0;
    do {
        r3 = r22;
        fn_8025D744();
        r22 = r22 + 0x1;
    } while ((s32)r22 < (s32)0x4);
    r29 = r27;
    r24 = (u32)sp + 0x8;
    r23 = 0x0;
    r28 = -0x1;
    do {
        r3 = r23;
        ((void(*)(void))fn_8006ACCC)();
        *(u32*)((u8*)r24 + 0x0) = r3;
        r3 = *(u32*)((u8*)r24 + 0x0);
        if (r3 != (u32)0x0) {
            r0 = *(u32*)((u8*)r3 + 0x28);
            r0 = (s8)r0;
            *(u8*)((u8*)r29 + 0x8) = r0;
        } else {

            *(u8*)((u8*)r29 + 0x8) = r28;
        }
        r24 = r24 + 0x4;
        r29 = r29 + 0x1;
        r23 = r23 + 0x1;
    } while ((s32)r23 < (s32)0x4);
    fn_8025DA88();
    if ((s32)r3 != (s32)0x2) {
        if ((s32)r3 >= (s32)0x2) goto L_80063EDC;
        if ((s32)r3 < (s32)0x0) {
            goto L_80063EDC;
        }
        r0 = 0x136;
        *(u32*)((u8*)r26 + (-12928)) = r0;
        goto L_80063EE4;
    }
    r0 = 0x0;
    *(u32*)((u8*)r26 + (-12928)) = r0;
    goto L_80063EE4;
    L_80063EDC: ;
    r0 = 0x0;
    *(u32*)((u8*)r26 + (-12928)) = r0;
    L_80063EE4: ;
    r3 = (u32)&lbl_803A9F08;
    r31 = (u32)&lbl_803A9F08;
    do {
        r0 = *(u32*)((u8*)r31 + 0x0);
        switch ((s32)r0) {
        case 0x0:
            r3 = 0x0;
            fn_80103CC0();
            r3 = 0xc6;
            r4 = 0x1;
            fn_8010264C();
            r28 = r3;
            r3 = 0x1;
            fn_80103CC0();
            if ((s32)r28 == (s32)0x0) {
                r3 = (u32)&lbl_803A9F08;
                r0 = 0x2;
                r3 = (u32)&lbl_803A9F08;
                *(u32*)((u8*)r3 + 0x0) = r0;
                break;
            }
            if ((s32)r28 == (s32)0x1) {
                r3 = (u32)&lbl_803A9F08;
                r0 = 0x1;
                r3 = (u32)&lbl_803A9F08;
                *(u32*)((u8*)r3 + 0x0) = r0;
                break;
            }
            r3 = (u32)&lbl_803A9F08;
            r0 = 0x3;
            r3 = (u32)&lbl_803A9F08;
            *(u32*)((u8*)r3 + 0x0) = r0;
            break;
        case 0x1:
            r3 = 0xc5;
            r4 = 0x1;
            fn_8010264C();
            if ((s32)r3 == (s32)0x0) {
                r0 = 0x1;
                r3 = 0xc5;
                *(u8*)((u8*)r31 + 0x4) = r0;
                r4 = 0x0;
                r5 = 0x1;
                fn_80102568();
                r3 = (u32)&lbl_803A9F08;
                r0 = 0x0;
                r3 = (u32)&lbl_803A9F08;
                *(u32*)((u8*)r3 + 0x0) = r0;
                break;
            }
            r3 = 0x0;
            fn_8025D744();
            r3 = 0xc5;
            r4 = 0x0;
            r5 = 0x1;
            fn_80102568();
            r3 = (u32)&lbl_803A9F08;
            r0 = 0x0;
            r3 = (u32)&lbl_803A9F08;
            *(u32*)((u8*)r3 + 0x0) = r0;
            break;
        case 0x2:
            r3 = 0xc7;
            r4 = 0x1;
            fn_8010264C();
            if ((s32)r3 >= (s32)0x0) {
                r3 = 0xc7;
                r4 = 0x0;
                r5 = 0x1;
                fn_80102568();
                r3 = (u32)&lbl_803A9F08;
                r0 = 0x0;
                r3 = (u32)&lbl_803A9F08;
                *(u32*)((u8*)r3 + 0x0) = r0;
                break;
            }
            r3 = 0xc7;
            r4 = 0x0;
            r5 = 0x1;
            fn_80102568();
            r3 = (u32)&lbl_803A9F08;
            r0 = 0x0;
            r3 = (u32)&lbl_803A9F08;
            *(u32*)((u8*)r3 + 0x0) = r0;
            break;
        case 0x3:
            r25 = 0x0;
            r3 = 0xc6;
            r4 = 0x0;
            r5 = 0x1;
            fn_80102568();
            r3 = 0xdf;
            r4 = 0x0;
            r5 = 0x1;
            fn_80102568();
            break;
        default:
            break;
        }
    } while ((s32)r25 != (s32)0x0);
    r0 = *(u8*)((u8*)r26 + (-12712));
    if (r0 == (u32)0x0) {
        fn_8025DA88();
        if ((s32)r3 == (s32)0x2) {
            r3 = 0x0;
            fn_8025D9F0();
            r0 = r3 & 0xFFFF;
            if ((s32)r0 != (s32)0x0) {
                r0 = 0x1;
                goto L_800640B8;
            }
            r0 = 0x0;

        } else {
        r0 = 0x0;
        }
        L_800640B8: ;
        r0 = r0 & 0xFFFF;
        if (r0 == (u32)0x0) {
            r4 = *(u32*)((u8*)r26 + (-12708));
            r3 = 0x30;
            fn_80132A38();
            r3 = 0x2;
            r4 = 0x44dc;
            r5 = 0x1;
            r6 = 0x1;
            fn_80106D3C();

        } else {
        r3 = 0x2;
        r4 = 0x44e7;
        r5 = 0x1;
        r6 = 0x1;
        fn_80106D3C();
        r24 = 0x1;
        do {
            r3 = 0x1;
            fn_800F7EF8();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x0) {
                r3 = 0x1;
                fn_800F7C28();
                if ((s32)r3 == (s32)0x0) {
                    r0 = 0x1;
                    goto L_80064138;
                }
                r0 = 0x0;

            } else {
            r0 = 0x0;
            }
            L_80064138: ;
            r0 = r0 & 0xFF;
            if (r0 != (u32)0x0) {
                r24 = 0x0;
            } else {

                _threadSwitch();
            }
        } while ((s32)r24 != (s32)0x0);
        r3 = 0x1;
        fn_801069FC();
        }
        r0 = 0x0;
        r3 = (u32)&lbl_803A9F08;
        r3 = (u32)&lbl_803A9F08;
        *(u32*)((u8*)r27 + 0x2C) = r0;
        r3 = r3 + (0x1 << 16);
        *(u8*)((u8*)r3 + (-12924)) = r0;
        r0 = 0x2;
        ctr_fn = (void(*)(void))r0;
        do {
            r3 = r27 + 0x30;
            r0 = 0x0;
            *(u8*)((u8*)r3 + 0x0) = r0;
            r8 = r3 + 0xc;
            r7 = r3 + 0x18;
            r6 = r3 + 0x24;
            *(u32*)((u8*)r3 + 0x4) = r0;
            r5 = r3 + 0x30;
            r4 = r3 + 0x3c;
            r27 = r27 + 0x48;
            *(u8*)((u8*)r8 + 0x0) = r0;
            r3 = r27 + 0x30;
            r27 = r27 + 0x48;
            *(u32*)((u8*)r8 + 0x4) = r0;
            r8 = r3 + 0xc;
            *(u8*)((u8*)r7 + 0x0) = r0;
            *(u32*)((u8*)r7 + 0x4) = r0;
            r7 = r3 + 0x18;
            *(u8*)((u8*)r6 + 0x0) = r0;
            *(u32*)((u8*)r6 + 0x4) = r0;
            r6 = r3 + 0x24;
            *(u8*)((u8*)r5 + 0x0) = r0;
            *(u32*)((u8*)r5 + 0x4) = r0;
            r5 = r3 + 0x30;
            *(u8*)((u8*)r4 + 0x0) = r0;
            *(u32*)((u8*)r4 + 0x4) = r0;
            r4 = r3 + 0x3c;
            *(u8*)((u8*)r3 + 0x0) = r0;
            *(u32*)((u8*)r3 + 0x4) = r0;
            *(u8*)((u8*)r8 + 0x0) = r0;
            *(u32*)((u8*)r8 + 0x4) = r0;
            *(u8*)((u8*)r7 + 0x0) = r0;
            *(u32*)((u8*)r7 + 0x4) = r0;
            *(u8*)((u8*)r6 + 0x0) = r0;
            *(u32*)((u8*)r6 + 0x4) = r0;
            *(u8*)((u8*)r5 + 0x0) = r0;
            *(u32*)((u8*)r5 + 0x4) = r0;
            *(u8*)((u8*)r4 + 0x0) = r0;
            *(u32*)((u8*)r4 + 0x4) = r0;
        } while (--ctr != 0);
        fn_80062834();
        r3 = 0xb3;
        return;
    }
    fn_8025DA88();
    r25 = r3;
    fn_8025D9CC();
    if ((s32)r3 == (s32)0x4) {
        do {
            if ((s32)r25 >= (s32)0x2) break;
            if ((s32)r25 < (s32)0x0) {
                break;
            }
            r23 = 0x2;
            goto L_80064284;
        } while (0);
        r23 = 0x4;
        goto L_80064284;
    }
    do {
        if ((s32)r25 >= (s32)0x2) break;
        if ((s32)r25 < (s32)0x0) {
            break;
        }
        r23 = 0x2;
        goto L_80064284;
    } while (0);
    r23 = 0x1;
    L_80064284: ;
    r22 = 0x0;
    while ((s32)r22 < (s32)r23) {

        r3 = r22;
        fn_8025D3F4();
        r22 = r22 + 0x1;

    }
    r0 = 0x0;
    r3 = (u32)&lbl_803A9F08;
    r3 = (u32)&lbl_803A9F08;
    *(u32*)((u8*)r27 + 0x2C) = r0;
    r3 = r3 + (0x1 << 16);
    *(u8*)((u8*)r3 + (-12924)) = r0;
    r0 = 0x2;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = r27 + 0x30;
        r0 = 0x0;
        *(u8*)((u8*)r3 + 0x0) = r0;
        r8 = r3 + 0xc;
        r7 = r3 + 0x18;
        r6 = r3 + 0x24;
        *(u32*)((u8*)r3 + 0x4) = r0;
        r5 = r3 + 0x30;
        r4 = r3 + 0x3c;
        r27 = r27 + 0x48;
        *(u8*)((u8*)r8 + 0x0) = r0;
        r3 = r27 + 0x30;
        r27 = r27 + 0x48;
        *(u32*)((u8*)r8 + 0x4) = r0;
        r8 = r3 + 0xc;
        *(u8*)((u8*)r7 + 0x0) = r0;
        *(u32*)((u8*)r7 + 0x4) = r0;
        r7 = r3 + 0x18;
        *(u8*)((u8*)r6 + 0x0) = r0;
        *(u32*)((u8*)r6 + 0x4) = r0;
        r6 = r3 + 0x24;
        *(u8*)((u8*)r5 + 0x0) = r0;
        *(u32*)((u8*)r5 + 0x4) = r0;
        r5 = r3 + 0x30;
        *(u8*)((u8*)r4 + 0x0) = r0;
        *(u32*)((u8*)r4 + 0x4) = r0;
        r4 = r3 + 0x3c;
        *(u8*)((u8*)r3 + 0x0) = r0;
        *(u32*)((u8*)r3 + 0x4) = r0;
        *(u8*)((u8*)r8 + 0x0) = r0;
        *(u32*)((u8*)r8 + 0x4) = r0;
        *(u8*)((u8*)r7 + 0x0) = r0;
        *(u32*)((u8*)r7 + 0x4) = r0;
        *(u8*)((u8*)r6 + 0x0) = r0;
        *(u32*)((u8*)r6 + 0x4) = r0;
        *(u8*)((u8*)r5 + 0x0) = r0;
        *(u32*)((u8*)r5 + 0x4) = r0;
        *(u8*)((u8*)r4 + 0x0) = r0;
        *(u32*)((u8*)r4 + 0x4) = r0;
    } while (--ctr != 0);
    r3 = 0xb8;

    return;
}
#endif


/* 0x80064378 | size: 0x5C */
#if 0
asm void fn_80064378(void) {
#include "src/game/ui/ui_core_fn_80064378.inc"
}
#else
void fn_80064378(u8* ctx, u8* arg1) {
    unsigned long long new_var;
    s16 value;
    int new_var2;

    value = *((s16*)(arg1 + 6));
    switch (value) {
    case 0xA9E:
        new_var2 = ctx[0x8b] | (new_var = -0x100);
        fn_800FB680(0, 0, new_var2, 0x3C1A);
        break;

    case 0xA88:
        fn_80063AD4();
        break;
    }
}
#endif


/* 0x800643D4 | size: 0x1254 */
#if 1
asm void fn_800643D4(void) {
#include "src/game/ui/ui_core_fn_800643D4.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
void fn_800643D4(void* arg0, void* arg1)
{
    UICmdMsg* msg;
    u8* ctx = (u8*) arg0;
    s32 h;

    msg = (UICmdMsg*) arg1;

    switch (msg->cmd) {
    case 0xB38: {
        void* q;
        u32 t;
        u32 snd;
        s32 mask = -0x100;
        q = (void*) fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        t = fn_8011F4F0();
        if (t == 0) {
            t = fn_800FA280(1);
        }
        fn_80132A38(0x37, t);
        fn_800FB680(0, 0, (ctx[0x8b] | mask), 0xe7);
        switch ((u8) fn_8001DA60(q)) {
        case 0:
            snd = 0xd67;
            break;
        case 1:
            snd = 0xd68;
            break;
        default:
            snd = 0;
            break;
        }
        if (snd != 0) {
            fn_80132A38(0x37, fn_800FA280(snd));
            fn_800FB680(0x5a, 0, (ctx[0x8b] | mask), 0xcf);
        }
        break;
    }
    case 0xB39: {
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        if (h != 0) {
            fn_80132A38(0x34, (u8) fn_8012640C(h, 0, 0x7a, 0));
            fn_800FB680(0, 0, (ctx[0x8b] | mask), 0xd3);
        }
        break;
    }
    case 0xB3A: {
        s32 v;
        s32 mask = -0x100;
        fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        v = fn_8011F188();
        fn_80132A38(0x34, (u16) v);
        fn_800FB680(0, 0, (ctx[0x8b] | mask), 0xd3);
        break;
    }
    case 0xB2C: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        cnt = fn_8012640C(h, 0, 0x7f, 0) & 0xffff;
        if ((u8) fn_80123CD4(h, 0) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_0;
            }
        } else if (cnt < 0x10000) {
        set_zero_0:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xffff:
            snd = 0x933;
            break;
        case 0xfffe:
            snd = 0x934;
            break;
        case 0:
            break;
        default:
            snd = fn_8011BEB4(0, snd, 1, 0);
            break;
        }
        if (snd != 0) {
            fn_80132A38(0x37, fn_800FA280(snd));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xe9);
        }
        break;
    }
    case 0xB2D: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        cnt = fn_8012640C(h, 0, 0x7f, 1) & 0xffff;
        if ((u8) fn_80123CD4(h, 1) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_1;
            }
        } else if (cnt < 0x10000) {
        set_zero_1:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xffff:
            snd = 0x933;
            break;
        case 0xfffe:
            snd = 0x934;
            break;
        case 0:
            break;
        default:
            snd = fn_8011BEB4(0, snd, 1, 0);
            break;
        }
        if (snd != 0) {
            fn_80132A38(0x37, fn_800FA280(snd));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xe9);
        }
        break;
    }
    case 0xB2E: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        cnt = fn_8012640C(h, 0, 0x7f, 2) & 0xffff;
        if ((u8) fn_80123CD4(h, 2) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_2;
            }
        } else if (cnt < 0x10000) {
        set_zero_2:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xffff:
            snd = 0x933;
            break;
        case 0xfffe:
            snd = 0x934;
            break;
        case 0:
            break;
        default:
            snd = fn_8011BEB4(0, snd, 1, 0);
            break;
        }
        if (snd != 0) {
            fn_80132A38(0x37, fn_800FA280(snd));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xe9);
        }
        break;
    }
    case 0xB2F: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        cnt = fn_8012640C(h, 0, 0x7f, 3) & 0xffff;
        if ((u8) fn_80123CD4(h, 3) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_3;
            }
        } else if (cnt < 0x10000) {
        set_zero_3:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xffff:
            snd = 0x933;
            break;
        case 0xfffe:
            snd = 0x934;
            break;
        case 0:
            break;
        default:
            snd = fn_8011BEB4(0, snd, 1, 0);
            break;
        }
        if (snd != 0) {
            fn_80132A38(0x37, fn_800FA280(snd));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xe9);
        }
        break;
    }
    case 0xB30: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        cnt = fn_8012640C(h, 0, 0x7f, 0) & 0xffff;
        if ((u8) fn_80123CD4(h, 0) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_4;
            }
        } else if (cnt < 0x10000) {
        set_zero_4:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xfffe:
        case 0:
            break;
        default:
            fn_80132A38(0x34, fn_8012640C(h, 0, 0x80, 0));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xdf);
            break;
        }
        break;
    }
    case 0xB31: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        cnt = fn_8012640C(h, 0, 0x7f, 1) & 0xffff;
        if ((u8) fn_80123CD4(h, 1) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_5;
            }
        } else if (cnt < 0x10000) {
        set_zero_5:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xfffe:
        case 0:
            break;
        default:
            fn_80132A38(0x34, fn_8012640C(h, 0, 0x80, 1));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xdf);
            break;
        }
        break;
    }
    case 0xB32: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        cnt = fn_8012640C(h, 0, 0x7f, 2) & 0xffff;
        if ((u8) fn_80123CD4(h, 2) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_6;
            }
        } else if (cnt < 0x10000) {
        set_zero_6:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xfffe:
        case 0:
            break;
        default:
            fn_80132A38(0x34, fn_8012640C(h, 0, 0x80, 2));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xdf);
            break;
        }
        break;
    }
    case 0xB33: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        cnt = fn_8012640C(h, 0, 0x7f, 3) & 0xffff;
        if ((u8) fn_80123CD4(h, 3) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_7;
            }
        } else if (cnt < 0x10000) {
        set_zero_7:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xfffe:
        case 0:
            break;
        default:
            fn_80132A38(0x34, fn_8012640C(h, 0, 0x80, 3));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xdf);
            break;
        }
        break;
    }
    case 0xB34: {
        s32 cnt;
        u32 v;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        cnt = fn_8012640C(h, 0, 0x7f, 0) & 0xffff;
        if ((u8) fn_80123CD4(h, 0) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_8;
            }
        } else if (cnt < 0x10000) {
        set_zero_8:
            cnt = 0;
        }
        v = (u16) cnt;
        if (v == 0xffff) {
            v = 0xa5;
        }
        if (v != 0) {
            u32 w;
            fn_8011CA34((u16) v);
            w = (u8) fn_8011C9EC();
            if (w != 0xfffe) {
                fn_801040F0(0, 0, ctx, ((u16*) lbl_802EDB40)[w], 0);
            }
        }
        break;
    }
    case 0xB35: {
        s32 cnt;
        u32 v;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        cnt = fn_8012640C(h, 0, 0x7f, 1) & 0xffff;
        if ((u8) fn_80123CD4(h, 1) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_9;
            }
        } else if (cnt < 0x10000) {
        set_zero_9:
            cnt = 0;
        }
        v = (u16) cnt;
        if (v == 0xffff) {
            v = 0xa5;
        }
        if (v != 0) {
            u32 w;
            fn_8011CA34((u16) v);
            w = (u8) fn_8011C9EC();
            if (w != 0xfffe) {
                fn_801040F0(0, 0, ctx, ((u16*) lbl_802EDB40)[w], 0);
            }
        }
        break;
    }
    case 0xB36: {
        s32 cnt;
        u32 v;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        cnt = fn_8012640C(h, 0, 0x7f, 2) & 0xffff;
        if ((u8) fn_80123CD4(h, 2) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_10;
            }
        } else if (cnt < 0x10000) {
        set_zero_10:
            cnt = 0;
        }
        v = (u16) cnt;
        if (v == 0xffff) {
            v = 0xa5;
        }
        if (v != 0) {
            u32 w;
            fn_8011CA34((u16) v);
            w = (u8) fn_8011C9EC();
            if (w != 0xfffe) {
                fn_801040F0(0, 0, ctx, ((u16*) lbl_802EDB40)[w], 0);
            }
        }
        break;
    }
    case 0xB37: {
        s32 cnt;
        u32 v;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        cnt = fn_8012640C(h, 0, 0x7f, 3) & 0xffff;
        if ((u8) fn_80123CD4(h, 3) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_11;
            }
        } else if (cnt < 0x10000) {
        set_zero_11:
            cnt = 0;
        }
        v = (u16) cnt;
        if (v == 0xffff) {
            v = 0xa5;
        }
        if (v != 0) {
            u32 w;
            fn_8011CA34((u16) v);
            w = (u8) fn_8011C9EC();
            if (w != 0xfffe) {
                fn_801040F0(0, 0, ctx, ((u16*) lbl_802EDB40)[w], 0);
            }
        }
        break;
    }
    case 0xB1F:
        fn_801040F0(
            0, 0, ctx,
            ((u16*) lbl_802ED9FC)[(u16) fn_8012640C(
                0,
                (u16) fn_8012640C(fn_8025D970(0, ((u32*) lbl_803A9F08)[3]), 0,
                                  0x6e, 0),
                0x16, 0)],
            0);
        break;
    case 0xB20: {
        u32 v1;
        u32 v2;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        v1 = (u16) fn_8012640C(0, (u16) fn_8012640C(h, 0, 0x6e, 0), 0x16, 0);
        v2 = (u16) fn_8012640C(0, (u16) fn_8012640C(h, 0, 0x6e, 0), 0x16, 1);
        if (v1 != v2) {
            fn_801040F0(0, 0, ctx, ((u16*) lbl_802ED9FC)[v2], 0);
        }
        break;
    }
    case 0xB21: {
        s32 fl;
        s32 mask = -0x100;
        u32 byte;
        s32 cnt;
        u32 v;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        byte = ctx[0x8b];
        fl = byte | mask;
        v = (u16) fn_801230E0(h);
        if (v != 0) {
            fn_80132A38(0x2d, v);
            fn_800FBB34(0, 0, msg->s54, msg->s56, fl, 0x30da);
        }
        break;
    }
    case 0xB27: {
        s32 fl;
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        fl = ctx[0x8b] | mask;
        fn_80132A38(0x34, (s16) fn_8012640C(h, 0, 0x88, 0));
        fn_800FBB34(0, 0, msg->s54, msg->s56, fl, 0xdf);
        break;
    }
    case 0xB28: {
        s32 fl;
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        fl = ctx[0x8b] | mask;
        fn_80132A38(0x34, (s16) fn_8012640C(h, 0, 0x89, 0));
        fn_800FBB34(0, 0, msg->s54, msg->s56, fl, 0xdf);
        break;
    }
    case 0xB29: {
        s32 fl;
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        fl = ctx[0x8b] | mask;
        fn_80132A38(0x34, (s16) fn_8012640C(h, 0, 0x8a, 0));
        fn_800FBB34(0, 0, msg->s54, msg->s56, fl, 0xdf);
        break;
    }
    case 0xB2A: {
        s32 fl;
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        fl = ctx[0x8b] | mask;
        fn_80132A38(0x34, (s16) fn_8012640C(h, 0, 0x8b, 0));
        fn_800FBB34(0, 0, msg->s54, msg->s56, fl, 0xdf);
        break;
    }
    case 0xB2B: {
        s32 fl;
        s32 mask = -0x100;
        h = fn_8025D970(0, ((u32*) lbl_803A9F08)[3]);
        fl = ctx[0x8b] | mask;
        fn_80132A38(0x34, (s16) fn_8012640C(h, 0, 0x8c, 0));
        fn_800FBB34(0, 0, msg->s54, msg->s56, fl, 0xdf);
        break;
    }
    case 0xB1E: {
        s32 idx;
        u8* p;
        idx = ((u32*) lbl_803A9F08)[3];
        fn_8025DA88();
        p = (u8*) lbl_803A9F08 + idx * 0xc + 0x30;
        if (p[0] != 0) {
            fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
        }
        break;
    }
    case 0xE32: {
        s32 idx;
        idx = ((u32*) lbl_803A9F08)[3];
        fn_8025DA88();
        if ((u16) fn_801230E0(fn_8025D970(0, idx)) != 0) {
            msg->flags4 |= 2;
        } else {
            msg->flags4 &= ~2;
        }
        break;
    }
    case 0x1097:
    case 0x1098:
        fn_80060EF4(ctx, msg, 6);
        break;
    case 0x1099:
    case 0x109A:
        fn_80060EF4(ctx, msg, 6);
        break;
    case 0x109B:
        fn_80060EF4(ctx, msg, -1);
        break;
    case 0x109C:
    case 0x109D:
        fn_80060EF4(ctx, msg, 3);
        break;
    case 0x109E:
    case 0x109F:
        fn_80060EF4(ctx, msg, 4);
        break;
    case 0x10A0:
    case 0x10A1:
        fn_80060EF4(ctx, msg, 2);
        break;
    case 0x10A2:
    case 0x10A3:
        fn_80060EF4(ctx, msg, 1);
        break;
    case 0x10A4:
        fn_80060EF4(ctx, msg, 0);
        break;
    case 0x10A5: {
        u8* p = lbl_802EF0A8 + 0x20000;
        u32 t;
        s32 mask = -0x100;
        if (fn_8025DAD0() == 0) {
            t = fn_800FA280(0x3db4);
        } else {
            fn_80132A38(0x2f, fn_8006B1D4());
            t = fn_800FA280(0x3c1e);
        }
        fn_80132A38(0x37, t);
        fn_800FBB34(*(s16*) (p - 0x2df2) - msg->s50 - 0x12,
                    *(s16*) (p - 0x2df0) - msg->s52, *(s16*) (p - 0x2dee),
                    *(s16*) (p - 0x2dec), (ctx[0x8b] | mask), 0xcf);
        break;
    }
    }
}
#pragma pop
#endif


/* 0x80065628 | size: 0x108 */
#if 1
asm void fn_80065628(void) {
#include "src/game/ui/ui_core_fn_80065628.inc"
}
#else
void fn_80065628(void) {
    extern void fn_80065A48();
    extern void fn_8025DA18();
    extern s32 fn_8025DA88();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r29 = r3;
    r30 = r4;
    fn_8025DA88();
    r31 = 0x3;
    r3 = (u32)&lbl_803A9F08;
    r3 = (u32)&lbl_803A9F08;
    r0 = *(u32*)((u8*)r3 + 0x154);
    if ((s32)r0 != (s32)0x2) {
        r0 = *(u8*)((u8*)r30 + 0x4);
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r30 + 0x4) = r0;
    }
    r3 = r29;
    r4 = r30;
    r5 = 0x3;
    fn_80065A48();
    r3 = r31;
    fn_8025DA18();
    r3 = r3 & 0xFFFF;
    r0 = *(s16*)((u8*)r30 + 0x6);
    r4 = r3 * 0x3;
    r3 = (u32)&lbl_802ED9F0;
    r3 = (u32)&lbl_802ED9F0;
    r3 = r3 + r4;
    if ((s32)r0 == (s32)0xbb1 || (s32)r0 == (s32)0xb92 || (s32)r0 == (s32)0xb73 || (s32)r0 == (s32)0xbd0) {
        r0 = *(u8*)((u8*)r3 + 0x0);
        *(u8*)((u8*)r30 + 0x64) = r0;
        r0 = *(u8*)((u8*)r3 + 0x1);
        *(u8*)((u8*)r30 + 0x65) = r0;
        r0 = *(u8*)((u8*)r3 + 0x2);
        *(u8*)((u8*)r30 + 0x66) = r0;
    }

    return;
}
#endif


/* 0x80065730 | size: 0x108 */
#if 1
asm void fn_80065730(void) {
#include "src/game/ui/ui_core_fn_80065730.inc"
}
#else
void fn_80065730(void) {
    extern void fn_80065A48();
    extern void fn_8025DA18();
    extern s32 fn_8025DA88();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r29 = r3;
    r30 = r4;
    fn_8025DA88();
    if ((s32)r3 >= (s32)0x0 && (s32)r3 < (s32)0x2) {
        r31 = 0x1;
    } else {
        r31 = 0x2;
    }
    r3 = (u32)&lbl_803A9F08;
    r3 = (u32)&lbl_803A9F08;
    r0 = *(u32*)((u8*)r3 + 0x154);
    if ((s32)r0 != (s32)0x2) {
        r0 = *(u8*)((u8*)r30 + 0x4);
        r0 = r0 | 0x2;
        r0 = (s8)r0;
        *(u8*)((u8*)r30 + 0x4) = r0;
    }
    r3 = r29;
    r4 = r30;
    r5 = 0x2;
    fn_80065A48();
    r3 = r31;
    fn_8025DA18();
    r3 = r3 & 0xFFFF;
    r0 = *(s16*)((u8*)r30 + 0x6);
    r4 = r3 * 0x3;
    r3 = (u32)&lbl_802ED9F0;
    r3 = (u32)&lbl_802ED9F0;
    r3 = r3 + r4;
    if ((s32)r0 == (s32)0xbb1 || (s32)r0 == (s32)0xb92 || (s32)r0 == (s32)0xb73 || (s32)r0 == (s32)0xbd0) {
        r0 = *(u8*)((u8*)r3 + 0x0);
        *(u8*)((u8*)r30 + 0x64) = r0;
        r0 = *(u8*)((u8*)r3 + 0x1);
        *(u8*)((u8*)r30 + 0x65) = r0;
        r0 = *(u8*)((u8*)r3 + 0x2);
        *(u8*)((u8*)r30 + 0x66) = r0;
    }

    return;
}
#endif


/* 0x80065838 | size: 0x108 */
#if 1
asm void fn_80065838(void) {
#include "src/game/ui/ui_core_fn_80065838.inc"
}
#else
void fn_80065838(void) {
    extern void fn_80065A48();
    extern void fn_8025DA18();
    extern s32 fn_8025DA88();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r29 = r3;
    r30 = r4;
    fn_8025DA88();
    if ((s32)r3 >= (s32)0x0 && (s32)r3 < (s32)0x2) {
        r31 = 0x2;
    } else {
        r31 = 0x1;
    }
    r3 = (u32)&lbl_803A9F08;
    r3 = (u32)&lbl_803A9F08;
    r0 = *(u32*)((u8*)r3 + 0x154);
    if ((s32)r0 != (s32)0x2) {
        r0 = *(u8*)((u8*)r30 + 0x4);
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r30 + 0x4) = r0;
    }
    r3 = r29;
    r4 = r30;
    r5 = 0x1;
    fn_80065A48();
    r3 = r31;
    fn_8025DA18();
    r3 = r3 & 0xFFFF;
    r0 = *(s16*)((u8*)r30 + 0x6);
    r4 = r3 * 0x3;
    r3 = (u32)&lbl_802ED9F0;
    r3 = (u32)&lbl_802ED9F0;
    r3 = r3 + r4;
    if ((s32)r0 == (s32)0xbb1 || (s32)r0 == (s32)0xb92 || (s32)r0 == (s32)0xb73 || (s32)r0 == (s32)0xbd0) {
        r0 = *(u8*)((u8*)r3 + 0x0);
        *(u8*)((u8*)r30 + 0x64) = r0;
        r0 = *(u8*)((u8*)r3 + 0x1);
        *(u8*)((u8*)r30 + 0x65) = r0;
        r0 = *(u8*)((u8*)r3 + 0x2);
        *(u8*)((u8*)r30 + 0x66) = r0;
    }

    return;
}
#endif


/* 0x80065940 | size: 0x108 */
#if 1
asm void fn_80065940(void) {
#include "src/game/ui/ui_core_fn_80065940.inc"
}
#else
void fn_80065940(void) {
    extern void fn_80065A48();
    extern void fn_8025DA18();
    extern s32 fn_8025DA88();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r29 = r3;
    r30 = r4;
    fn_8025DA88();
    r31 = 0x0;
    r3 = (u32)&lbl_803A9F08;
    r3 = (u32)&lbl_803A9F08;
    r0 = *(u32*)((u8*)r3 + 0x154);
    if ((s32)r0 != (s32)0x2) {
        r0 = *(u8*)((u8*)r30 + 0x4);
        r0 = r0 | 0x2;
        r0 = (s8)r0;
        *(u8*)((u8*)r30 + 0x4) = r0;
    }
    r3 = r29;
    r4 = r30;
    r5 = 0x0;
    fn_80065A48();
    r3 = r31;
    fn_8025DA18();
    r3 = r3 & 0xFFFF;
    r0 = *(s16*)((u8*)r30 + 0x6);
    r4 = r3 * 0x3;
    r3 = (u32)&lbl_802ED9F0;
    r3 = (u32)&lbl_802ED9F0;
    r3 = r3 + r4;
    if ((s32)r0 == (s32)0xbb1 || (s32)r0 == (s32)0xb92 || (s32)r0 == (s32)0xb73 || (s32)r0 == (s32)0xbd0) {
        r0 = *(u8*)((u8*)r3 + 0x0);
        *(u8*)((u8*)r30 + 0x64) = r0;
        r0 = *(u8*)((u8*)r3 + 0x1);
        *(u8*)((u8*)r30 + 0x65) = r0;
        r0 = *(u8*)((u8*)r3 + 0x2);
        *(u8*)((u8*)r30 + 0x66) = r0;
    }

    return;
}
#endif


/* 0x80065A48 | size: 0x1CA4 */
#if 0
asm void fn_80065A48(void) {
#include "src/game/ui/ui_core_fn_80065A48.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
extern u8 lbl_803A9F08[];

void fn_80065A48(void* ctx, void* arg1, s32 arg2)
{
    UICmdMsg* msg = (UICmdMsg*) arg1;
    s32 i0;
    s32 i1;
    s32 i2;
    s32 i3;

    switch (fn_8025DA88()) {
    case 0:
    case 1:
        i0 = 0;
        i1 = 1;
        i2 = 2;
        i3 = 3;
        break;
    case 2:
        i0 = 0;
        i2 = 1;
        i1 = 2;
        i3 = 3;
        break;
    default:
        i0 = 0;
        i2 = 1;
        i1 = 2;
        i3 = 3;
        break;
    }
    switch (msg->cmd) {
    case 0xB74: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i0 * 0x48 + 0x30;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB75: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i0 * 0x48 + 0x3C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB76: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i0 * 0x48 + 0x48;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB77: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i0 * 0x48 + 0x54;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB78: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i0 * 0x48 + 0x60;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB79: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i0 * 0x48 + 0x6C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBB2: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i2 * 0x48 + 0x30;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBB3: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i2 * 0x48 + 0x3C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBB4: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i2 * 0x48 + 0x48;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBB5: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i2 * 0x48 + 0x54;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBB6: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i2 * 0x48 + 0x60;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBB7: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i2 * 0x48 + 0x6C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB93: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i1 * 0x48 + 0x30;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB94: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i1 * 0x48 + 0x3C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB95: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i1 * 0x48 + 0x48;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB96: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i1 * 0x48 + 0x54;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB97: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i1 * 0x48 + 0x60;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB98: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i1 * 0x48 + 0x6C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBD1: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i3 * 0x48 + 0x30;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBD2: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i3 * 0x48 + 0x3C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBD3: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i3 * 0x48 + 0x48;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBD4: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i3 * 0x48 + 0x54;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBD5: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i3 * 0x48 + 0x60;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBD6: {
        s32 ok = 1;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i3 * 0x48 + 0x6C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB86:
    case 0xB87:
    case 0xB88:
    case 0xB89:
    case 0xB8A:
    case 0xB8B:
        fn_80068DBC(ctx, msg, i0);
        break;
    case 0xB8C:
    case 0xB8D:
    case 0xB8E:
    case 0xB8F:
    case 0xB90:
    case 0xB91:
        fn_80068BB0(ctx, msg, i0, 0);
        break;
    case 0xB80:
    case 0xB81:
    case 0xB82:
    case 0xB83:
    case 0xB84:
    case 0xB85:
        fn_800689FC(ctx, msg, i0);
        break;
    case 0xB3D: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i0);
            m = (m < n) ? m : n;
            if ((s32) m > 0) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB3E: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i0);
            m = (m < n) ? m : n;
            if ((s32) m > 1) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB3F: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i0);
            m = (m < n) ? m : n;
            if ((s32) m > 2) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB40: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i0);
            m = (m < n) ? m : n;
            if ((s32) m > 3) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB41: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i0);
            m = (m < n) ? m : n;
            if ((s32) m > 4) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB42: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i0);
            m = (m < n) ? m : n;
            if ((s32) m > 5) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB43:
        fn_80068794(ctx, msg, i0, 0);
        break;
    case 0xB44:
        fn_80068794(ctx, msg, i0, 1);
        break;
    case 0xB45:
        fn_80068794(ctx, msg, i0, 2);
        break;
    case 0xB46:
        fn_80068794(ctx, msg, i0, 3);
        break;
    case 0xB47:
        fn_80068794(ctx, msg, i0, 4);
        break;
    case 0xB48:
        fn_80068794(ctx, msg, i0, 5);
        break;
    case 0xBC4:
    case 0xBC5:
    case 0xBC6:
    case 0xBC7:
    case 0xBC8:
    case 0xBC9:
        fn_80068DBC(ctx, msg, i2);
        break;
    case 0xBCA:
    case 0xBCB:
    case 0xBCC:
    case 0xBCD:
    case 0xBCE:
    case 0xBCF:
        fn_80068BB0(ctx, msg, i2, 0);
        break;
    case 0xBBE:
    case 0xBBF:
    case 0xBC0:
    case 0xBC1:
    case 0xBC2:
    case 0xBC3:
        fn_800689FC(ctx, msg, i2);
        break;
    case 0xB59: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i2);
            m = (m < n) ? m : n;
            if ((s32) m > 0) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB5A: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i2);
            m = (m < n) ? m : n;
            if ((s32) m > 1) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB5B: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i2);
            m = (m < n) ? m : n;
            if ((s32) m > 2) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB5C: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i2);
            m = (m < n) ? m : n;
            if ((s32) m > 3) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB5D: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i2);
            m = (m < n) ? m : n;
            if ((s32) m > 4) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB5E: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i2);
            m = (m < n) ? m : n;
            if ((s32) m > 5) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB5F:
        fn_80068794(ctx, msg, i2, 0);
        break;
    case 0xB60:
        fn_80068794(ctx, msg, i2, 1);
        break;
    case 0xB61:
        fn_80068794(ctx, msg, i2, 2);
        break;
    case 0xB62:
        fn_80068794(ctx, msg, i2, 3);
        break;
    case 0xB63:
        fn_80068794(ctx, msg, i2, 4);
        break;
    case 0xB64:
        fn_80068794(ctx, msg, i2, 5);
        break;
    case 0xBA5:
    case 0xBA6:
    case 0xBA7:
    case 0xBA8:
    case 0xBA9:
    case 0xBAA:
        fn_80068DBC(ctx, msg, i1);
        break;
    case 0xBAB:
    case 0xBAC:
    case 0xBAD:
    case 0xBAE:
    case 0xBAF:
    case 0xBB0:
        fn_80068BB0(ctx, msg, i1, 0);
        break;
    case 0xB9F:
    case 0xBA0:
    case 0xBA1:
    case 0xBA2:
    case 0xBA3:
    case 0xBA4:
        fn_800689FC(ctx, msg, i1);
        break;
    case 0xB4B: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i1);
            m = (m < n) ? m : n;
            if ((s32) m > 0) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB4C: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i1);
            m = (m < n) ? m : n;
            if ((s32) m > 1) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB4D: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i1);
            m = (m < n) ? m : n;
            if ((s32) m > 2) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB4E: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i1);
            m = (m < n) ? m : n;
            if ((s32) m > 3) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB4F: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i1);
            m = (m < n) ? m : n;
            if ((s32) m > 4) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB50: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i1);
            m = (m < n) ? m : n;
            if ((s32) m > 5) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB51:
        fn_80068794(ctx, msg, i1, 0);
        break;
    case 0xB52:
        fn_80068794(ctx, msg, i1, 1);
        break;
    case 0xB53:
        fn_80068794(ctx, msg, i1, 2);
        break;
    case 0xB54:
        fn_80068794(ctx, msg, i1, 3);
        break;
    case 0xB55:
        fn_80068794(ctx, msg, i1, 4);
        break;
    case 0xB56:
        fn_80068794(ctx, msg, i1, 5);
        break;
    case 0xBE3:
    case 0xBE4:
    case 0xBE5:
    case 0xBE6:
    case 0xBE7:
    case 0xBE8:
        fn_80068DBC(ctx, msg, i3);
        break;
    case 0xBE9:
    case 0xBEA:
    case 0xBEB:
    case 0xBEC:
    case 0xBED:
    case 0xBEE:
        fn_80068BB0(ctx, msg, i3, 0);
        break;
    case 0xBDD:
    case 0xBDE:
    case 0xBDF:
    case 0xBE0:
    case 0xBE1:
    case 0xBE2:
        fn_800689FC(ctx, msg, i3);
        break;
    case 0xB67: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i3);
            m = (m < n) ? m : n;
            if ((s32) m > 0) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB68: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i3);
            m = (m < n) ? m : n;
            if ((s32) m > 1) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB69: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i3);
            m = (m < n) ? m : n;
            if ((s32) m > 2) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB6A: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i3);
            m = (m < n) ? m : n;
            if ((s32) m > 3) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB6B: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i3);
            m = (m < n) ? m : n;
            if ((s32) m > 4) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB6C: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (fn_8025DA88()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = fn_8025D89C(i3);
            m = (m < n) ? m : n;
            if ((s32) m > 5) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB6D:
        fn_80068794(ctx, msg, i3, 0);
        break;
    case 0xB6E:
        fn_80068794(ctx, msg, i3, 1);
        break;
    case 0xB6F:
        fn_80068794(ctx, msg, i3, 2);
        break;
    case 0xB70:
        fn_80068794(ctx, msg, i3, 3);
        break;
    case 0xB71:
        fn_80068794(ctx, msg, i3, 4);
        break;
    case 0xB72:
        fn_80068794(ctx, msg, i3, 5);
        break;
    case 0xB3C:
        fn_800688C4(ctx, msg, i0, 0);
        break;
    case 0xB58:
        fn_800688C4(ctx, msg, i2, 1);
        break;
    case 0xB4A:
        fn_800688C4(ctx, msg, i1, 2);
        break;
    case 0xB66:
        fn_800688C4(ctx, msg, i3, 3);
        break;
    case 0xBF0:
        switch ((u16) fn_8025D9F0(0)) {
        case 0:
            msg->flags4 |= 2;
            break;
        default:
            msg->flags4 &= ~2;
            break;
        }
        break;
    case 0xB3B:
        switch ((u16) fn_8025D9F0(0)) {
        case 0:
            msg->flags4 |= 2;
            break;
        default:
            msg->flags4 &= ~2;
            break;
        }
        break;
    case 0xB7A:
    case 0xB7B:
    case 0xB7C:
    case 0xB7D:
    case 0xB7E:
    case 0xB7F:
    case 0xBEF:
        switch ((u16) fn_8025D9F0(0)) {
        case 0:
            msg->flags4 |= 2;
            break;
        default:
            msg->flags4 &= ~2;
            break;
        }
        break;
    }
}
#pragma pop
#endif


/* 0x800676EC | size: 0x2D4 */
#if 1
asm void fn_800676EC(void) {
#include "src/game/ui/ui_core_fn_800676EC.inc"
}
#else
void fn_800676EC(void) {
    extern void fn_800679C0();
    extern void fn_8006905C();
    extern void fn_800F7BC4();
    extern void fn_80102ED4();
    extern void fn_8025D2B0();
    extern s32 fn_8025D89C();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r3 = 0x1;
    fn_800F7BC4();
    r0 = r3 & 0x00000020;
    if (r0 != (u32)0x0) {
        r3 = 0x0;
        fn_8025D2B0();
        if ((s32)r3 != (s32)0x1) goto L_80067948;
        r3 = (u32)&lbl_803A9F08;
        r3 = (u32)&lbl_803A9F08;
        r0 = *(u32*)((u8*)r3 + 0x0);
        switch ((s32)r0) {
        case 0x0:
            r0 = *(u8*)((u8*)r3 + 0x4);
            if (r0 == (u32)0x0) {
                r3 = 0x1;
                fn_800F7BC4();
                r30 = r3;
                r29 = -0x1;
                r3 = 0x0;
                fn_8025D89C();
                r0 = r30 & 0x1;
                r3 = r3 & 0xFFFF;
                if (r0 != (u32)0x0) {
                    r29 = 0x0;
                }
                r0 = r30 & 0x00000008;
                if (r0 != (u32)0x0) {
                    r29 = 0x1;
                }
                r0 = r30 & 0x00000800;
                if (r0 != (u32)0x0) {
                    r29 = 0x2;
                }
                r0 = r30 & 0x00000004;
                if (r0 != (u32)0x0) {
                    r29 = 0x3;
                }
                r0 = r30 & 0x00000002;
                if (r0 != (u32)0x0) {
                    r29 = 0x4;
                }
                r0 = r30 & 0x00000400;
                if (r0 != (u32)0x0) {
                    r29 = 0x5;
                }
                if ((s32)r3 <= (s32)r29) {
                    r29 = -0x1;
                }
                if ((s32)r29 >= (s32)0x0) {
                    r3 = 0x0;
                    fn_8025D89C();
                    r0 = r3 & 0xFFFF;
                    if ((s32)r29 < (s32)r0) {
                        r0 = 0x0;
                        r3 = (u32)&lbl_803A9F08;
                        *(u8*)((u8*)r31 + 0x95) = r0;
                        r0 = 0x1;
                        r3 = (u32)&lbl_803A9F08;
                        *(u8*)((u8*)r31 + 0x98) = r0;
                        *(u32*)((u8*)r3 + 0xC) = r29;
            }
            }
            }
            r3 = r31;
            r4 = 0x1;
            fn_800679C0();
            goto L_80067948;
        case 0x1:
            r3 = r31;
            r4 = 0x1;
            fn_800679C0();
            goto L_80067948;
        case 0x2:
            break;
        default:
            goto L_80067948;
        }
        r3 = 0x1;
        fn_800F7BC4();
        r4 = (u32)&lbl_803A9F08;
        r5 = 0x0;
        r4 = (u32)&lbl_803A9F08;
        r0 = *(u32*)((u8*)r4 + 0xC);
        switch ((s32)r0) {
        case 0x0:
            r5 = 0x1;
            break;
        case 0x1:
            r5 = 0x8;
            break;
        case 0x2:
            r5 = 0x800;
            break;
        case 0x3:
            r5 = 0x4;
            break;
        case 0x4:
            r5 = 0x2;
            break;
        case 0x5:
            r5 = 0x400;
            break;
        default:
            break;
        }
        r0 = r3 & r5;
        if (r0 == (u32)0x0) {
            r0 = 0x1;
            *(u8*)((u8*)r31 + 0x98) = r0;
        }
        r3 = r31;
        r4 = 0x1;
        fn_800679C0();

    } else {
    r3 = (u32)&lbl_803A9F08;
    r3 = (u32)&lbl_803A9F08;
    r0 = *(u32*)((u8*)r3 + 0x0);
    switch ((s32)r0) {
    case 0x0:
        r3 = r31;
        r4 = 0x0;
        fn_800679C0();
        break;
    case 0x1:
        r3 = r31;
        r4 = 0x1;
        fn_800679C0();
        break;
    case 0x2:
        r3 = r31;
        r4 = 0x1;
        fn_800679C0();
        r0 = 0x1;
        *(u8*)((u8*)r31 + 0x98) = r0;
        break;
    default:
        break;
    }
    }
    L_80067948: ;
    fn_8006905C();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r0 = 0x1;
        *(u8*)((u8*)r31 + 0x98) = r0;
        *(u8*)((u8*)r31 + 0x99) = r0;
    }
    r3 = (u32)&lbl_803A9F08;
    r3 = (u32)&lbl_803A9F08;
    r3 = r3 + (0x1 << 16);
    r0 = *(u8*)((u8*)r3 + (-12712));
    if (r0 == (u32)0x0) {
        r0 = 0x1;
        *(u8*)((u8*)r31 + 0x98) = r0;
        *(u8*)((u8*)r31 + 0x99) = r0;
    }
    r3 = (u32)&lbl_803A9F08;
    r3 = (u32)&lbl_803A9F08;
    r0 = *(u32*)((u8*)r3 + 0x0);
    if ((s32)r0 == (s32)0x1) {
        r3 = r31;
        fn_80102ED4();
    }
    return;
}
#endif


/* 0x800679C0 | size: 0x7F4 */
#if 1
asm void fn_800679C0(void) {
#include "src/game/ui/ui_core_fn_800679C0.inc"
}
#else
void fn_800679C0(void) {
    extern void fn_800681B4();
    extern void fn_8008ABA0();
    extern void fn_800F7AF0();
    extern void fn_800F7BC4();
    extern void fn_80166AB8();
    extern void fn_8025D2B0();
    extern void fn_8025D560();
    extern void fn_8025D584();
    extern void fn_8025D644();
    extern s32 fn_8025D89C();
    extern void fn_8025D9CC();
    extern s32 fn_8025D9F0();
    extern void fn_8025DA18();
    extern void fn_8025DA3C();
    extern s32 fn_8025DA88();
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

    
    r27 = r3;
    r24 = r4;
    fn_8025DA3C();
    r30 = r3;
    fn_8025DA88();
    r3 = 0x0;
    fn_8025D9F0();
    r3 = 0x1;
    fn_8025D9F0();
    r3 = 0x2;
    fn_8025D9F0();
    r3 = 0x3;
    fn_8025D9F0();
    if ((s32)r24 != (s32)0x0) {
        r28 = 0x1;
    } else {

        r28 = 0x0;
    }
    r3 = (u32)&lbl_803A9F08;
    r31 = (u32)&lbl_803A9F08;
    r29 = r31 + (0x1 << 16);
    r26 = r28 * 0x30;
    while ((s32)r28 < (s32)r30) {

        fn_8025D9CC();
        if ((s32)r3 == (s32)0x4) {
            r3 = (u32)&lbl_803A9F08;
            r24 = r28;
            r0 = (u32)&lbl_803A9F08;
            r3 = r0 + r28;
            r0 = *(u8*)((u8*)r3 + 0x4);
        }
        do {
            if (r0 == (u32)0x0) break;
            r3 = r28;
            fn_8025D2B0();
            ((void(*)(void))fn_8006AFE4)();
            r0 = *(u32*)((u8*)r3 + 0x4);
            if ((s32)r0 != (s32)0x1) {
                if ((s32)r0 != (s32)0x2) break;
            }
            r3 = r24;
            fn_8025D2B0();
            r25 = r3;
            fn_8008ABA0();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x0) {
                r0 = 0x0;
                *(u8*)((u8*)r29 + (-12712)) = r0;
                r0 = *(u32*)((u8*)r29 + (-12708));
                if ((s32)r0 < (s32)0x0) {
                    *(u32*)((u8*)r29 + (-12708)) = r25;
        }
            }
        } while (0);
        r3 = r31 + r28;
        r0 = *(u8*)((u8*)r3 + 0x4);
        if (r0 == (u32)0x0) {
            fn_8025D9CC();
            if ((s32)r3 == (s32)0x4) {
                r3 = r28;
                fn_8025D2B0();
                ((void(*)(void))fn_8006AFE4)();
                r0 = *(u32*)((u8*)r3 + 0x4);
                if ((s32)r0 != (s32)0x1) {
                    if ((s32)r0 == (s32)0x2) {
                    }
                    r3 = r28;
                    fn_8025D2B0();
                    r4 = r28;
                    fn_800681B4();
                    } else {

                    r3 = r28;
                    fn_8025D2B0();
                    r24 = r3;
                    fn_800F7AF0();
                    r25 = r3;
                    r3 = r24;
                    fn_800F7BC4();
                    r25 = r3 & r25;
                    r3 = r28;
                    fn_8025DA18();
                    r0 = r25 & 0x00000040;
                    if (r0 != (u32)0x0) {
                        r3 = r28;
                        fn_8025D560();
                        r25 = r3;
                        r3 = r28;
                        fn_8025D584();
                        if ((s32)r25 != (s32)r3) {
                            r3 = 0x25;
                            r4 = 0x0;
                            r5 = 0x0;
                            fn_80166AB8();
                        }
                        goto L_800680EC;
                    }
                    r0 = r25 & 0xc0f;
                    if (r0 != (u32)0x0) {
                        r3 = r28;
                        r24 = -0x1;
                        fn_8025D89C();
                        r0 = r25 & 0x1;
                        r3 = r3 & 0xFFFF;
                        if (r0 != (u32)0x0) {
                            r24 = 0x0;
                        }
                        r0 = r25 & 0x00000008;
                        if (r0 != (u32)0x0) {
                            r24 = 0x1;
                        }
                        r0 = r25 & 0x00000800;
                        if (r0 != (u32)0x0) {
                            r24 = 0x2;
                        }
                        r0 = r25 & 0x00000004;
                        if (r0 != (u32)0x0) {
                            r24 = 0x3;
                        }
                        r0 = r25 & 0x00000002;
                        if (r0 != (u32)0x0) {
                            r24 = 0x4;
                        }
                        r0 = r25 & 0x00000400;
                        if (r0 != (u32)0x0) {
                            r24 = 0x5;
                        }
                        if ((s32)r3 <= (s32)r24) {
                            r24 = -0x1;
                        }
                        if ((s32)r24 >= (s32)0x0) {
                            r3 = r28;
                            r4 = r24;
                            fn_8025D644();
                            r25 = r3;
                            if ((s32)r25 >= (s32)0x0) {
                                r3 = 0x3c3;
                                r4 = 0x0;
                                r5 = 0x0;
                                fn_80166AB8();
                                r3 = 0x5 - r25;
                                r0 = (0x4330 << 16);
                                r4 = r3 * 0x18;
                                r3 = (u32)&lbl_803A9F08;
                                *(u32*)(sp + 0x8) = r0;
                                r0 = r25 << 2;
                                r3 = (u32)&lbl_803A9F08;
                                f2 = *(f64*)&lbl_8047BFF0;
                                /* xoris r4, r4, 0x8000 */;
                                f0 = *(f32*)&lbl_8047BFE8;
                                r4 = r28 * 0x30;
                                f1 = *(f64*)(sp + 0x8);
                                r3 = r3 + r4;
                                r3 = r3 + (0x1 << 16);
                                f1 = f1 - f2;
                                r3 = r3 + r0;
                                /* subi r3, r3, 0x3274 */;
                                *(f32*)((u8*)r3 + 0x0) = f1;
                                *(f32*)((u8*)r3 + 0x18) = f0;
                    }
                    }
                    }
                    goto L_800680EC;
                }
                if ((s32)r28 == (s32)0x1) {
                    r3 = r28;
                    fn_8025D2B0();
                    r3 = r28;
                    fn_8025DA18();
                    f2 = *(f32*)((u8*)r29 + (-12724));
                    f1 = *(f32*)((u8*)r29 + (-12920));
                    f0 = *(f32*)&lbl_8047BFEC;
                    f1 = f2 + f1;
                    *(f32*)((u8*)r29 + (-12724)) = f1;
                    f1 = *(f32*)((u8*)r29 + (-12724));
                    /* cror eq, gt, eq */;
                    if (f1 == f0) {
                        f0 = *(f32*)&lbl_8047BFE8;
                        r3 = (u32)&lbl_802EDB64;
                        r3 = (u32)&lbl_802EDB64;
                        *(f32*)((u8*)r29 + (-12724)) = f0;
                        r5 = *(u32*)((u8*)r29 + (-12720));
                        r4 = r5 << 2;
                        r0 = r5 + 0x1;
                        r24 = *(u32*)(r3 + r4);
                        *(u32*)((u8*)r29 + (-12720)) = r0;
                    } else {

                        r24 = 0x0;
                    }
                    r0 = r24 & 0xc0f;
                    if (r0 != (u32)0x0) {
                        r3 = r28;
                        r25 = -0x1;
                        fn_8025D89C();
                        r0 = r24 & 0x1;
                        r3 = r3 & 0xFFFF;
                        if (r0 != (u32)0x0) {
                            r25 = 0x0;
                        }
                        r0 = r24 & 0x00000008;
                        if (r0 != (u32)0x0) {
                            r25 = 0x1;
                        }
                        r0 = r24 & 0x00000800;
                        if (r0 != (u32)0x0) {
                            r25 = 0x2;
                        }
                        r0 = r24 & 0x00000004;
                        if (r0 != (u32)0x0) {
                            r25 = 0x3;
                        }
                        r0 = r24 & 0x00000002;
                        if (r0 != (u32)0x0) {
                            r25 = 0x4;
                        }
                        r0 = r24 & 0x00000400;
                        if (r0 != (u32)0x0) {
                            r25 = 0x5;
                        }
                        r3 = r28;
                        if ((s32)r3 <= (s32)r25) {
                            r4 = -0x1;
                        } else {

                            r4 = r25;
                        }
                        fn_8025D644();
                        r25 = r3;
                        if ((s32)r25 >= (s32)0x0) {
                            r3 = 0x3c3;
                            r4 = 0x0;
                            r5 = 0x0;
                            fn_80166AB8();
                            r3 = 0x5 - r25;
                            r0 = (0x4330 << 16);
                            r4 = r3 * 0x18;
                            r3 = (u32)&lbl_803A9F08;
                            *(u32*)(sp + 0x8) = r0;
                            r0 = r25 << 2;
                            r3 = (u32)&lbl_803A9F08;
                            f2 = *(f64*)&lbl_8047BFF0;
                            /* xoris r4, r4, 0x8000 */;
                            f0 = *(f32*)&lbl_8047BFE8;
                            r4 = r28 * 0x30;
                            f1 = *(f64*)(sp + 0x8);
                            r3 = r3 + r4;
                            r3 = r3 + (0x1 << 16);
                            f1 = f1 - f2;
                            r3 = r3 + r0;
                            /* subi r3, r3, 0x3274 */;
                            *(f32*)((u8*)r3 + 0x0) = f1;
                            *(f32*)((u8*)r3 + 0x18) = f0;
                    }
                    }
                    goto L_800680EC;
                }
                r3 = r28;
                fn_8025D9F0();
                r0 = r3 & 0xFFFF;
                if ((s32)r0 != (s32)0x0) {
                    if ((s32)r0 >= (s32)0x0 && (s32)r0 < (s32)0x3) {


                    } else {
                    r3 = r28;
                    fn_8025D2B0();
                    r24 = r3;
                    fn_800F7AF0();
                    r25 = r3;
                    r3 = r24;
                    fn_800F7BC4();
                    r24 = r3 & r25;
                    r3 = r28;
                    fn_8025DA18();
                    r0 = r24 & 0x00000040;
                    if (r0 != (u32)0x0) {
                        r3 = r28;
                        fn_8025D560();
                        r25 = r3;
                        r3 = r28;
                        fn_8025D584();
                        if ((s32)r25 != (s32)r3) {
                            r3 = 0x25;
                            r4 = 0x0;
                            r5 = 0x0;
                            fn_80166AB8();
                        }
                        goto L_800680EC;
                    }
                    r0 = r24 & 0xc0f;
                    if (r0 != (u32)0x0) {
                        r3 = r28;
                        r25 = -0x1;
                        fn_8025D89C();
                        r0 = r24 & 0x1;
                        r3 = r3 & 0xFFFF;
                        if (r0 != (u32)0x0) {
                            r25 = 0x0;
                        }
                        r0 = r24 & 0x00000008;
                        if (r0 != (u32)0x0) {
                            r25 = 0x1;
                        }
                        r0 = r24 & 0x00000800;
                        if (r0 != (u32)0x0) {
                            r25 = 0x2;
                        }
                        r0 = r24 & 0x00000004;
                        if (r0 != (u32)0x0) {
                            r25 = 0x3;
                        }
                        r0 = r24 & 0x00000002;
                        if (r0 != (u32)0x0) {
                            r25 = 0x4;
                        }
                        r0 = r24 & 0x00000400;
                        if (r0 != (u32)0x0) {
                            r25 = 0x5;
                        }
                        if ((s32)r3 <= (s32)r25) {
                            r25 = -0x1;
                        }
                        if ((s32)r25 >= (s32)0x0) {
                            r3 = r28;
                            r4 = r25;
                            fn_8025D644();
                            r25 = r3;
                            if ((s32)r25 >= (s32)0x0) {
                                r3 = 0x3c3;
                                r4 = 0x0;
                                r5 = 0x0;
                                fn_80166AB8();
                                r3 = 0x5 - r25;
                                r0 = (0x4330 << 16);
                                r4 = r3 * 0x18;
                                r3 = (u32)&lbl_803A9F08;
                                *(u32*)(sp + 0x8) = r0;
                                r0 = r25 << 2;
                                r3 = (u32)&lbl_803A9F08;
                                f2 = *(f64*)&lbl_8047BFF0;
                                /* xoris r4, r4, 0x8000 */;
                                f0 = *(f32*)&lbl_8047BFE8;
                                r4 = r28 * 0x30;
                                f1 = *(f64*)(sp + 0x8);
                                r3 = r3 + r4;
                                r3 = r3 + (0x1 << 16);
                                f1 = f1 - f2;
                                r3 = r3 + r0;
                                /* subi r3, r3, 0x3274 */;
                                *(f32*)((u8*)r3 + 0x0) = f1;
                                *(f32*)((u8*)r3 + 0x18) = f0;
                    }
                    }
                    }
                    goto L_800680EC;
                    }
                    r3 = r28;
                    fn_8025D2B0();
                    r4 = r28;
                    fn_800681B4();
                    } else {

                    r3 = r28;
                    fn_8025D2B0();
                    r24 = r3;
                    fn_800F7AF0();
                    r25 = r3;
                    r3 = r24;
                    fn_800F7BC4();
                    r24 = r3 & r25;
                    r3 = r28;
                    fn_8025DA18();
                    r0 = r24 & 0x00000040;
                    if (r0 != (u32)0x0) {
                        r3 = r28;
                        fn_8025D560();
                        r25 = r3;
                        r3 = r28;
                        fn_8025D584();
                        if ((s32)r25 != (s32)r3) {
                            r3 = 0x25;
                            r4 = 0x0;
                            r5 = 0x0;
                            fn_80166AB8();
                    }
                        goto L_800680EC;
                    }
                    r0 = r24 & 0xc0f;
                    if (r0 != (u32)0x0) {
                        r3 = r28;
                        r25 = -0x1;
                        fn_8025D89C();
                        r0 = r24 & 0x1;
                        r3 = r3 & 0xFFFF;
                        if (r0 != (u32)0x0) {
                            r25 = 0x0;
                        }
                        r0 = r24 & 0x00000008;
                        if (r0 != (u32)0x0) {
                            r25 = 0x1;
                        }
                        r0 = r24 & 0x00000800;
                        if (r0 != (u32)0x0) {
                            r25 = 0x2;
                        }
                        r0 = r24 & 0x00000004;
                        if (r0 != (u32)0x0) {
                            r25 = 0x3;
                        }
                        r0 = r24 & 0x00000002;
                        if (r0 != (u32)0x0) {
                            r25 = 0x4;
                        }
                        r0 = r24 & 0x00000400;
                        if (r0 != (u32)0x0) {
                            r25 = 0x5;
                        }
                        if ((s32)r3 <= (s32)r25) {
                            r25 = -0x1;
                        }
                        if ((s32)r25 >= (s32)0x0) {
                            r3 = r28;
                            r4 = r25;
                            fn_8025D644();
                            r25 = r3;
                            if ((s32)r25 >= (s32)0x0) {
                                r3 = 0x3c3;
                                r4 = 0x0;
                                r5 = 0x0;
                                fn_80166AB8();
                                r3 = 0x5 - r25;
                                r0 = (0x4330 << 16);
                                r4 = r3 * 0x18;
                                r3 = (u32)&lbl_803A9F08;
                                *(u32*)(sp + 0x8) = r0;
                                r0 = r25 << 2;
                                r3 = (u32)&lbl_803A9F08;
                                f2 = *(f64*)&lbl_8047BFF0;
                                /* xoris r4, r4, 0x8000 */;
                                f0 = *(f32*)&lbl_8047BFE8;
                                r4 = r28 * 0x30;
                                f1 = *(f64*)(sp + 0x8);
                                r3 = r3 + r4;
                                r3 = r3 + (0x1 << 16);
                                f1 = f1 - f2;
                                r3 = r3 + r0;
                                /* subi r3, r3, 0x3274 */;
                                *(f32*)((u8*)r3 + 0x0) = f1;
                                *(f32*)((u8*)r3 + 0x18) = f0;
            }
                }
                }
                    }
                    }
            L_800680EC: ;
            r3 = r28;
            fn_8025D2B0();
            if ((s32)r3 == (s32)0x1) {
                r3 = r28;
                fn_8025D9F0();
                r0 = r3 & 0xFFFF;
                if (r0 == (u32)0x0) {
                    r3 = r28;
                    fn_8025D560();
                    r25 = r3;
                    ((void(*)(void))fn_8006B1D4)();
                    r24 = r3 & 0xFFFF;
                    r3 = r28;
                    fn_8025D89C();
                    r0 = r3 & 0xFFFF;
                    if (r0 < r24) {
                    } else {

                        r0 = r24;
                    }
                    r0 = r0 & 0xFFFF;
                    if ((s32)r25 == (s32)r0) {
                        /* subi r0, r25, 0x1 */;
                        if ((s32)r0 < (s32)0x0) {
                            r0 = 0x0;
                        }
                        r3 = (u32)&lbl_803A9F08;
                        r0 = r0 << 2;
                        r3 = (u32)&lbl_803A9F08;
                        f1 = *(f32*)&lbl_8047BFE8;
                        r3 = r3 + r26;
                        r3 = r3 + r0;
                        r3 = r3 + (0x1 << 16);
                        f0 = *(f32*)((u8*)r3 + (-12916));
                        if (f1 == f0) {
                            r0 = 0x1;
                            *(u8*)((u8*)r27 + 0x95) = r0;
                            *(u8*)((u8*)r27 + 0x98) = r0;
    }
        }
        }
        }
        }
        r28 = r28 + 0x1;
        r26 = r26 + 0x30;

    }
    return;
}
#endif


/* 0x800681B4 | size: 0x264 */
#if 1
asm void fn_800681B4(void) {
#include "src/game/ui/ui_core_fn_800681B4.inc"
}
#else
void fn_800681B4(void) {
    extern void fn_8008A9AC();
    extern void fn_8008A9E4();
    extern void fn_80166AB8();
    extern void fn_8025D560();
    extern void fn_8025D584();
    extern void fn_8025D5E0();
    extern void fn_8025D644();
    extern void fn_8025D808();
    extern void fn_8025DA18();
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r31 = r3;
    r30 = r4;
    r3 = r30;
    fn_8025DA18();
    r3 = r31;
    r4 = (u32)sp + 0x8;
    fn_8008A9E4();
    r0 = (0x100 << 16);
    /* clrrwi r3, r3, 24 */;
    if ((s32)r3 != (s32)r0) {
        if ((s32)r3 < (s32)r0) {
            if ((s32)r3 != (s32)0x0) {
                if ((s32)r3 >= (s32)0x0) return;
                r0 = (0xff00 << 16);
                if ((s32)r3 != (s32)r0) return;

            }
            r0 = (0x300 << 16);
            if ((s32)r3 != (s32)r0) return;

        }
        r3 = r30;
        fn_8025D560();
        r0 = r3;
        r3 = r30;
        r4 = r0;
        fn_8025D644();
        r31 = r3;
        if ((s32)r31 < (s32)0x0) return;
        r3 = 0x3c3;
        r4 = 0x0;
        r5 = 0x0;
        fn_80166AB8();
        r3 = 0x5 - r31;
        r0 = (0x4330 << 16);
        r3 = r3 * 0x18;
        r4 = (u32)&lbl_803A9F08;
        *(u32*)(sp + 0x30) = r0;
        r5 = r31 << 2;
        f2 = *(f64*)&lbl_8047BFF0;
        r4 = (u32)&lbl_803A9F08;
        /* xoris r0, r3, 0x8000 */;
        f0 = *(f32*)&lbl_8047BFE8;
        *(u32*)(sp + 0x34) = r0;
        r0 = r30 * 0x30;
        f1 = *(f64*)(sp + 0x30);
        r3 = r4 + r0;
        r4 = r3 + (0x1 << 16);
        f1 = f1 - f2;
        /* subi r4, r4, 0x3274 */;
        r3 = r4 + r5;
        *(f32*)(r4 + r5) = f1;
        *(f32*)((u8*)r3 + 0x18) = f0;
        return;

        r3 = r30;
        fn_8025D560();
        r31 = r3;
        r3 = r30;
        fn_8025D584();
        if ((s32)r31 == (s32)r3) return;
        r3 = 0x25;
        r4 = 0x0;
        r5 = 0x0;
        fn_80166AB8();
        return;
            }
    r3 = r30;
    fn_8025D808();
    r31 = r3 & 0xFFFF;
    r4 = (u32)sp + 0xc;
    fn_8008A9AC();
    r6 = 0x0;
    if ((s32)r6 < (s32)r31) {
        /* subi r3, r31, 0x8 */;
        if ((s32)r31 > (s32)0x8) {
            r4 = (u32)sp + 0xc;
            r5 = (u32)sp + 0x14;
            r0 = r3 + 0x7;
            r0 = (u32)r0 >> 3;
            ctr_fn = (void(*)(void))r0;
            if ((s32)r3 > (s32)0x0) {
                do {
                    r0 = *(u8*)((u8*)r4 + 0x0);
                    r6 = r6 + 0x8;
                    *(u32*)((u8*)r5 + 0x0) = r0;
                    r0 = *(u8*)((u8*)r4 + 0x1);
                    *(u32*)((u8*)r5 + 0x4) = r0;
                    r0 = *(u8*)((u8*)r4 + 0x2);
                    *(u32*)((u8*)r5 + 0x8) = r0;
                    r0 = *(u8*)((u8*)r4 + 0x3);
                    *(u32*)((u8*)r5 + 0xC) = r0;
                    r0 = *(u8*)((u8*)r4 + 0x4);
                    *(u32*)((u8*)r5 + 0x10) = r0;
                    r0 = *(u8*)((u8*)r4 + 0x5);
                    *(u32*)((u8*)r5 + 0x14) = r0;
                    r0 = *(u8*)((u8*)r4 + 0x6);
                    *(u32*)((u8*)r5 + 0x18) = r0;
                    r0 = *(u8*)((u8*)r4 + 0x7);
                    r4 = r4 + 0x8;
                    *(u32*)((u8*)r5 + 0x1C) = r0;
                    r5 = r5 + 0x20;
                } while (--ctr != 0);
        }
        }
        r3 = (u32)sp + 0xc;
        r0 = r6 << 2;
        r4 = (u32)sp + 0x14;
        r3 = r3 + r6;
        r4 = r4 + r0;
        r0 = r31 - r6;
        ctr_fn = (void(*)(void))r0;
        if ((s32)r6 < (s32)r31) {
            do {
                r0 = *(u8*)((u8*)r3 + 0x0);
                r3 = r3 + 0x1;
                *(u32*)((u8*)r4 + 0x0) = r0;
                r4 = r4 + 0x4;
            } while (--ctr != 0);
    }
    }
    r3 = r30;
    r4 = r31;
    r5 = (u32)sp + 0x14;
    fn_8025D5E0();
    r3 = (u32)&lbl_803A9F08;
    r4 = 0x1;
    r0 = (u32)&lbl_803A9F08;
    r3 = r0 + r30;
    *(u8*)((u8*)r3 + 0x4) = r4;
    return;

    r3 = (u32)&lbl_803A9F08;
    r0 = 0x0;
    r3 = (u32)&lbl_803A9F08;
    r3 = r3 + (0x1 << 16);
    *(u8*)((u8*)r3 + (-12712)) = r0;
    r0 = *(u32*)((u8*)r3 + (-12708));
    if ((s32)r0 >= (s32)0x0) return;
    *(u32*)((u8*)r3 + (-12708)) = r31;

    return;
}
#endif


/* 0x80068418 | size: 0x320 */
#if 1
asm void fn_80068418(u8* p, s32 v) {
#include "src/game/ui/ui_core_fn_80068418.inc"
}
#else
void fn_80068418(u8* p, s32 v) {
    extern void fn_800CE2D8();
    extern void fn_800D3088();
    extern void fn_800F7A08();
    extern void fn_800F7A7C();
    extern void fn_800F7BC4();
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
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

    
    r26 = r3;
    r0 = *(u16*)((u8*)r26 + 0x0);
    r31 = r4;
    r3 = r31;
    r29 = 0x0;
    *(u16*)((u8*)r26 + 0x2) = r0;
    r4 = 0x0;
    fn_800F7A08();
    r27 = r3;
    r3 = r31;
    r4 = 0x0;
    fn_800F7A7C();
    r0 = (s8)r3;
    if ((s32)r0 < (s32)0x0) {
        r0 = -r0;
    }
    if ((s32)r0 <= (s32)0x20) {
        r0 = (s8)r27;
        if ((s32)r0 < (s32)0x0) {
            r0 = -r0;
        }
        if ((s32)r0 > (s32)0x20) {
            r0 = 0; /* flag: not skipped */
        }
    } else {
        r0 = 0; /* flag: not skipped */
    }
    if ((s32)r0 == (s32)0x0) {
    r4 = (s8)r3;
    r0 = (s8)r27;
    r3 = (0x4330 << 16);
    f2 = *(f64*)&lbl_8047BFF0;
    /* xoris r4, r4, 0x8000 */;
    /* xoris r0, r0, 0x8000 */;
    f0 = *(f64*)(sp + 0x8);
    *(u32*)(sp + 0x14) = r0;
    f1 = f0 - f2;
    f0 = *(f64*)(sp + 0x10);
    f2 = f0 - f2;
    fn_800CE2D8();
    f2 = (f32)f1;
    f0 = *(f32*)&lbl_8047BFE8;
    if (f2 > f0) {
        f1 = f2;
    } else {

        f1 = -f2;
    }
    f0 = *(f32*)&lbl_8047BFF8;
    if (f1 < f0) {
        r0 = r29 | 0x2;
        r29 = r0 & 0xFFFF;
    } else {
        f0 = *(f32*)&lbl_8047BFE8;
        if (f2 > f0) {
            f1 = f2;
        } else {

            f1 = -f2;
        }
        f0 = *(f32*)&lbl_8047BFFC;
        if (f1 > f0) {
            r0 = r29 | 0x1;
            r29 = r0 & 0xFFFF;
        }
    }
    f0 = *(f32*)&lbl_8047BFE8;
    f1 = *(f32*)&lbl_8047C000;
    if (f2 > f0) {
        f0 = f2;
    } else {

        f0 = -f2;
    }
    if (f1 < f0) {
        f0 = *(f32*)&lbl_8047BFE8;
        if (f2 > f0) {
            f1 = f2;
        } else {

            f1 = -f2;
        }
        f0 = *(f32*)&lbl_8047C004;
    }
    if ((f1 < f0)) {
    f0 = *(f32*)&lbl_8047BFE8;
    if (f2 < f0) {
        r0 = r29 | 0x4;
        r29 = r0 & 0xFFFF;
    } else {
    r0 = r29 | 0x8;
    r29 = r0 & 0xFFFF;
    }
    }
    }
    r3 = r31;
    fn_800F7BC4();
    r0 = r3 & 0x00000008;
    if (r0 != (u32)0x0) {
        r0 = r29 | 0x1;
        r29 = r0 & 0xFFFF;
    }
    r0 = r3 & 0x00000004;
    if (r0 != (u32)0x0) {
        r0 = r29 | 0x2;
        r29 = r0 & 0xFFFF;
    }
    r0 = r3 & 0x1;
    if (r0 != (u32)0x0) {
        r0 = r29 | 0x4;
        r29 = r0 & 0xFFFF;
    }
    r0 = r3 & 0x00000002;
    if (r0 != (u32)0x0) {
        r0 = r29 | 0x8;
        r29 = r0 & 0xFFFF;
    }
    r0 = r3 & 0x00000100;
    if (r0 != (u32)0x0) {
        r0 = r29 | 0x10;
        r29 = r0 & 0xFFFF;
    }
    r0 = r3 & 0x00000200;
    if (r0 != (u32)0x0) {
        r0 = r29 | 0x20;
        r29 = r0 & 0xFFFF;
    }
    r0 = r3 & 0x00000400;
    if (r0 != (u32)0x0) {
        r0 = r29 | 0x40;
        r29 = r0 & 0xFFFF;
    }
    r0 = r3 & 0x00000800;
    if (r0 != (u32)0x0) {
        r0 = r29 | 0x80;
        r29 = r0 & 0xFFFF;
    }
    r0 = r3 & 0x00000010;
    if (r0 != (u32)0x0) {
        r0 = r29 | 0x100;
        r29 = r0 & 0xFFFF;
    }
    r0 = r3 & 0x00000040;
    if (r0 != (u32)0x0) {
        r0 = r29 | 0x200;
        r29 = r0 & 0xFFFF;
    }
    r0 = r3 & 0x00000020;
    if (r0 != (u32)0x0) {
        r0 = r29 | 0x400;
        r29 = r0 & 0xFFFF;
    }
    r0 = r3 & 0x00001000;
    if (r0 != (u32)0x0) {
        r0 = r29 | 0x800;
        r29 = r0 & 0xFFFF;
    }
    r0 = *(u16*)((u8*)r26 + 0x2);
    r31 = r29 & 0xFFFF;
    r27 = 0x0;
    r30 = 0x0;
    r0 = r0 ^ 0xffff;
    r0 = r0 & r31;
    r28 = r0 & 0xFFFF;
    do {
        r0 = 0x1;
        r0 = r0 << r30;
        r25 = r0 & 0xFFFF;
        r0 = r28 & r25;
        if ((s32)r0 != (s32)0x0) {
            r0 = r30 + 0xa;
            r3 = 0xf;
            *(u8*)(r26 + r0) = r3;
            r27 = r27 | r25;
        } else {
        r0 = r31 & r25;
        if ((s32)r0 != (s32)0x0) {
            fn_800D3088();
            r4 = r30 + 0xa;
            r0 = *(u8*)(r26 + r4);
            r0 = r0 - r3;
            r0 = (s8)r0;
            *(u8*)(r26 + r4) = r0;
            r0 = *(u8*)(r26 + r4);
            r0 = (s8)r0;
            if ((s32)r0 <= (s32)0x0) {
                r0 = 0x5;
                r27 = r27 | r25;
                *(u8*)(r26 + r4) = r0;
    }
        }
        }
        r30 = r30 + 0x1;
    } while ((s32)r30 < (s32)0x10);
    *(u16*)((u8*)r26 + 0x0) = r29;
    *(u16*)((u8*)r26 + 0x4) = r28;
    *(u16*)((u8*)r26 + 0x6) = r27;
    return;
}
#endif


/* 0x80068738 | size: 0x5C */
#if 0
asm s32 fn_80068738(void) {
#include "src/game/ui/ui_core_fn_80068738.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma peephole off
s32 fn_80068738(void) {
    u8 *ptr;
    s32 i;

    fn_80105624();
    ptr = (u8 *)&lbl_803A9EA0;
    for (i = 0; i < 4; i++) {
        fn_80068418(ptr, i + 1);
        ptr += 0x1A;
    }
    return 0;
}
#pragma pop
#endif



/* 0x80068794 | size: 0x130 */
#if 1
asm void fn_80068794(void) {
#include "src/game/ui/ui_core_fn_80068794.inc"
}
#else
void fn_80068794(void) {
    extern void fn_8025D560();
    extern s32 fn_8025DA88();
    u8 sp[0x30];
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;

    
    r31 = r4;
    r27 = r5;
    r28 = r6;
    r3 = r27;
    fn_8025D560();
    r30 = r3;
    r29 = 0x1;
    fn_8025DA88();
    if ((s32)r3 < (s32)0x2 && (s32)r3 >= (s32)0x0) {
        if ((s32)r27 >= (s32)0x2) {
            r29 = 0x0;
        }
    }
    r0 = r29 & 0xFF;
    if (r0 == (u32)0x0) return;
    if ((s32)r30 > (s32)r28) {
        r6 = r27 * 0x30;
        r5 = (u32)&lbl_803A9F08;
        r4 = *(s16*)((u8*)r31 + 0x6);
        r3 = (u32)&lbl_802EF0A8;
        r0 = (u32)&lbl_803A9F08;
        f0 = *(f32*)&lbl_8047BFE8;
        r5 = r0 + r6;
        r6 = r28 << 2;
        r5 = r5 + (0x1 << 16);
        r0 = (u32)&lbl_802EF0A8;
        /* subi r5, r5, 0x3274 */;
        r5 = r5 + r6;
        f1 = *(f32*)((u8*)r5 + 0x0);
        r3 = r4 * 0x1c;
        f1 = (f64)(s32)f1;
        r3 = r0 + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        *(f64*)(sp + 0x8) = f1;
        r0 = r3 + r0;
        r0 = (s16)r0;
        *(u16*)((u8*)r31 + 0x50) = r0;
        f2 = *(f32*)((u8*)r5 + 0x18);
        f1 = *(f32*)((u8*)r5 + 0x0);
        f2 = f2 - f1;
        if (f2 > f0) {
        } else {

            f2 = -f2;
        }
        f1 = *(f32*)&lbl_8047C00C;
        f0 = *(f32*)&lbl_8047C008;
        f0 = -(f1 * f2 - f0);
        f0 = (f64)(s32)f0;
        *(f64*)(sp + 0x10) = f0;
        *(u8*)((u8*)r31 + 0x67) = r0;
        r0 = *(u8*)((u8*)r31 + 0x4);
        r0 = r0 | 0x2;
        r0 = (s8)r0;
        *(u8*)((u8*)r31 + 0x4) = r0;
        return;
    }
    r0 = *(u8*)((u8*)r31 + 0x4);
    r0 = r0 & 0xFFFFFFFD;
    r0 = (s8)r0;
    *(u8*)((u8*)r31 + 0x4) = r0;

    return;
}
#endif


/* 0x800688C4 | size: 0x138 */
#if 1
asm void fn_800688C4(void) {
#include "src/game/ui/ui_core_fn_800688C4.inc"
}
#else
void fn_800688C4(void) {
    extern s32 fn_800FA280();
    extern void fn_800FB680();
    extern void fn_8012AC54();
    extern void fn_80132A38();
    extern void fn_8025D914();
    extern void fn_8025D9CC();
    extern void fn_8025DA18();
    extern s32 fn_8025DA88();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r28 = r3;
    r29 = r5;
    r30 = r6;
    r31 = 0x1;
    fn_8025DA88();
    if ((s32)r3 < (s32)0x2 && (s32)r3 >= (s32)0x0) {
        if ((s32)r29 >= (s32)0x2) {
            r31 = 0x0;
        }
    }
    r0 = r31 & 0xFF;
    if (r0 == (u32)0x0) return;
    r3 = r29;
    fn_8025D914();
    fn_8012AC54();
    r31 = r3;
    if (r31 == (u32)0x0) {
        r3 = 0x1;
        fn_800FA280();
        r31 = r3;
    }
    r3 = r29;
    fn_8025DA18();
    r4 = r3 & 0xFFFF;
    r3 = 0x34;
    r4 = r4 + 0x1;
    fn_80132A38();
    r4 = r31;
    r3 = 0x37;
    fn_80132A38();
    fn_8025D9CC();
    if ((s32)r3 == (s32)0x4) {
        r5 = *(u8*)((u8*)r28 + 0x8B);
        r0 = -0x100;
        r3 = 0x0;
        r4 = 0x0;
        r5 = r5 | r0;
        r6 = 0x30dc;
        fn_800FB680();
        return;
    }
    if ((s32)r30 == (s32)0x2) {
        r5 = *(u8*)((u8*)r28 + 0x8B);
        r0 = -0x100;
        r3 = 0x0;
        r4 = 0x0;
        r5 = r5 | r0;
        r6 = 0x30e6;
        fn_800FB680();
        return;
    }
    r5 = *(u8*)((u8*)r28 + 0x8B);
    r0 = -0x100;
    r3 = 0x0;
    r4 = 0x0;
    r5 = r5 | r0;
    r6 = 0x30dc;
    fn_800FB680();

    return;
}
#endif


/* 0x800689FC | size: 0x1B4 */
#if 1
asm void fn_800689FC(void) {
#include "src/game/ui/ui_core_fn_800689FC.inc"
}
#else
void fn_800689FC(void) {
    extern s32 fn_801230E0();
    extern s32 fn_8025D970();
    extern s32 fn_8025DA88();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r31 = r4;
    r29 = r5;
    r30 = 0x1;
    fn_8025DA88();
    if ((s32)r3 < (s32)0x2 && (s32)r3 >= (s32)0x0) {
        if ((s32)r29 >= (s32)0x2) {
            r30 = 0x0;
        }
    }
    r0 = r30 & 0xFF;
    if (r0 == (u32)0x0) return;
    r0 = *(s16*)((u8*)r31 + 0x6);
    r3 = (u32)&lbl_802EDA20;
    r3 = (u32)&lbl_802EDA20;
    r4 = 0x0;
    r5 = r0 & 0xFFFF;
    r0 = 0x8;
    ctr_fn = (void(*)(void))r0;
    r0 = 0x0;
    do {
        { int _i;
        for (_i = 0; _i < 9; _i++) {
            r0 = *(u16*)((u8*)r3 + 0x0);
            if (r0 == (u32)r5) {
                r0 = *(u16*)((u8*)r3 + 0x2);
                break;
            }
            r3 = r3 + 0x4;
        }
        if (_i < 9) break;
        }
        r4 = r4 + 0x8;
    } while (--ctr != 0);
    r4 = r0 & 0xFFFF;
    r3 = r29;
    fn_8025D970();
    fn_801230E0();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x0) {
        r0 = *(u8*)((u8*)r31 + 0x4);
        r0 = r0 | 0x2;
        r0 = (s8)r0;
        *(u8*)((u8*)r31 + 0x4) = r0;
        return;
    }
    r0 = *(u8*)((u8*)r31 + 0x4);
    r0 = r0 & 0xFFFFFFFD;
    r0 = (s8)r0;
    *(u8*)((u8*)r31 + 0x4) = r0;

    return;
}
#endif


/* 0x80068BB0 | size: 0x20C */
#if 1
asm void fn_80068BB0(void) {
#include "src/game/ui/ui_core_fn_80068BB0.inc"
}
#else
void fn_80068BB0(void) {
    extern void fn_800FB680();
    extern void fn_80123FBC();
    extern s32 fn_8012640C();
    extern void fn_80132A38();
    extern s32 fn_8025D970();
    extern s32 fn_8025DA88();
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
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r29 = r3;
    r30 = r4;
    r27 = r5;
    r31 = r6;
    r28 = 0x1;
    fn_8025DA88();
    if ((s32)r3 < (s32)0x2 && (s32)r3 >= (s32)0x0) {
        if ((s32)r27 >= (s32)0x2) {
            r28 = 0x0;
        }
    }
    r0 = r28 & 0xFF;
    if (r0 == (u32)0x0) return;
    r0 = *(s16*)((u8*)r30 + 0x6);
    r3 = (u32)&lbl_802EDA20;
    r3 = (u32)&lbl_802EDA20;
    r4 = 0x0;
    r5 = r0 & 0xFFFF;
    r0 = 0x8;
    ctr_fn = (void(*)(void))r0;
    r0 = 0x0;
    do {
        { int _i;
        for (_i = 0; _i < 9; _i++) {
            r0 = *(u16*)((u8*)r3 + 0x0);
            if (r0 == (u32)r5) {
                r0 = *(u16*)((u8*)r3 + 0x2);
                break;
            }
            r3 = r3 + 0x4;
        }
        if (_i < 9) break;
        }
        r4 = r4 + 0x8;
    } while (--ctr != 0);
    r4 = r0 & 0xFFFF;
    r3 = r27;
    fn_8025D970();
    r28 = r3;
    if (r28 == (u32)0x0) return;
    fn_80123FBC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        r0 = *(u8*)((u8*)r30 + 0x4);
        r0 = r0 & 0xFFFFFFFD;
        r0 = (s8)r0;
        *(u8*)((u8*)r30 + 0x4) = r0;
        return;
    }
    r3 = r28;
    r4 = 0x0;
    r5 = 0x7a;
    r6 = 0x0;
    fn_8012640C();
    r4 = r3 & 0xFF;
    r3 = 0x34;
    fn_80132A38();
    if ((s32)r31 == (s32)0x0) {
        r5 = *(u8*)((u8*)r29 + 0x8B);
        r0 = -0x100;
        r3 = 0x0;
        r4 = 0x0;
        r5 = r5 | r0;
        r6 = 0x30d4;
        fn_800FB680();
        return;
    }
    r5 = *(u8*)((u8*)r29 + 0x8B);
    r0 = -0x100;
    r3 = 0x0;
    r4 = 0x0;
    r5 = r5 | r0;
    r6 = 0xd3;
    fn_800FB680();

    return;
}
#endif


/* 0x80068DBC | size: 0x1C8 */
#if 1
asm void fn_80068DBC(void) {
#include "src/game/ui/ui_core_fn_80068DBC.inc"
}
#else
void fn_80068DBC(void) {
    extern s32 fn_800FA280();
    extern void fn_800FB680();
    extern u32 fn_8011F4F0();
    extern void fn_80132A38();
    extern s32 fn_8025D970();
    extern s32 fn_8025DA88();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r31 = r3;
    r28 = r4;
    r29 = r5;
    r30 = 0x1;
    fn_8025DA88();
    if ((s32)r3 < (s32)0x2 && (s32)r3 >= (s32)0x0) {
        if ((s32)r29 >= (s32)0x2) {
            r30 = 0x0;
        }
    }
    r0 = r30 & 0xFF;
    if (r0 == (u32)0x0) return;
    r0 = *(s16*)((u8*)r28 + 0x6);
    r3 = (u32)&lbl_802EDA20;
    r3 = (u32)&lbl_802EDA20;
    r4 = 0x0;
    r5 = r0 & 0xFFFF;
    r0 = 0x8;
    ctr_fn = (void(*)(void))r0;
    r0 = 0x0;
    do {
        { int _i;
        for (_i = 0; _i < 9; _i++) {
            r0 = *(u16*)((u8*)r3 + 0x0);
            if (r0 == (u32)r5) {
                r0 = *(u16*)((u8*)r3 + 0x2);
                break;
            }
            r3 = r3 + 0x4;
        }
        if (_i < 9) break;
        }
        r4 = r4 + 0x8;
    } while (--ctr != 0);
    r4 = r0 & 0xFFFF;
    r3 = r29;
    fn_8025D970();
    fn_8011F4F0();
    if (r3 == (u32)0x0) {
        r3 = 0x1;
        fn_800FA280();
    }
    r4 = r3;
    r3 = 0x37;
    fn_80132A38();
    r5 = *(u8*)((u8*)r31 + 0x8B);
    r0 = -0x100;
    r3 = 0x0;
    r4 = 0x0;
    r5 = r5 | r0;
    r6 = 0xe9;
    fn_800FB680();

    return;
}
#endif


/* 0x80068F84 | size: 0xC4 */
#if 1
asm void fn_80068F84(void) {
#include "src/game/ui/ui_core_fn_80068F84.inc"
}
#else
void fn_80068F84(void) {
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 ctr = 0;

    r4 = (u32)&lbl_803A9F08;
    *(u32*)((u8*)r4 + 0x2C) = 0;
    r3 = r4 + (0x1 << 16);
    *(u8*)((u8*)r3 + (-12924)) = 0;
    ctr = 2;
    do {
        r9 = r4 + 0x30;
        r0 = 0x0;
        *(u8*)((u8*)r9 + 0x0) = r0;
        r8 = r9 + 0xc;
        r7 = r9 + 0x18;
        r6 = r9 + 0x24;
        *(u32*)((u8*)r9 + 0x4) = r0;
        r5 = r9 + 0x30;
        r3 = r9 + 0x3c;
        r4 = r4 + 0x48;
        *(u8*)((u8*)r8 + 0x0) = r0;
        r9 = r4 + 0x30;
        r4 = r4 + 0x48;
        *(u32*)((u8*)r8 + 0x4) = r0;
        r8 = r9 + 0xc;
        *(u8*)((u8*)r7 + 0x0) = r0;
        *(u32*)((u8*)r7 + 0x4) = r0;
        r7 = r9 + 0x18;
        *(u8*)((u8*)r6 + 0x0) = r0;
        *(u32*)((u8*)r6 + 0x4) = r0;
        r6 = r9 + 0x24;
        *(u8*)((u8*)r5 + 0x0) = r0;
        *(u32*)((u8*)r5 + 0x4) = r0;
        r5 = r9 + 0x30;
        *(u8*)((u8*)r3 + 0x0) = r0;
        *(u32*)((u8*)r3 + 0x4) = r0;
        r3 = r9 + 0x3c;
        *(u8*)((u8*)r9 + 0x0) = r0;
        *(u32*)((u8*)r9 + 0x4) = r0;
        *(u8*)((u8*)r8 + 0x0) = r0;
        *(u32*)((u8*)r8 + 0x4) = r0;
        *(u8*)((u8*)r7 + 0x0) = r0;
        *(u32*)((u8*)r7 + 0x4) = r0;
        *(u8*)((u8*)r6 + 0x0) = r0;
        *(u32*)((u8*)r6 + 0x4) = r0;
        *(u8*)((u8*)r5 + 0x0) = r0;
        *(u32*)((u8*)r5 + 0x4) = r0;
        *(u8*)((u8*)r3 + 0x0) = r0;
        *(u32*)((u8*)r3 + 0x4) = r0;
    } while (--ctr != 0);
}
#endif


/* 0x80069048 | size: 0x14 */
u8 fn_80069048(void) {
    return *(volatile u8*)((u32)&lbl_803A9F08 + 0x10000 - 0x327C);
}

/* 0x8006905C | size: 0x1C4 */
#if 1
asm void fn_8006905C(void) {
#include "src/game/ui/ui_core_fn_8006905C.inc"
}
#else
void fn_8006905C(void) {
    extern void fn_8025D560();
    extern s32 fn_8025D89C();
    extern void fn_8025D9CC();
    extern s32 fn_8025DA88();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r31 = 0x1;
    fn_8025D9CC();
    r30 = r3;
    fn_8025DA88();
    if ((s32)r3 == (s32)0x2) {
        r3 = (u32)&lbl_803A9F08;
        r0 = 0x4;
        r3 = (u32)&lbl_803A9F08;
        r31 = 0x4;
        r3 = r3 + (0x1 << 16);
        *(u32*)((u8*)r3 + (-12932)) = r0;
    } else if ((s32)r3 >= (s32)0x0 && (s32)r3 < (s32)0x2) {
        if ((s32)r30 == (s32)0x4) {
            r3 = (u32)&lbl_803A9F08;
            r0 = 0x4;
            r3 = (u32)&lbl_803A9F08;
            r31 = 0x2;
            r3 = r3 + (0x1 << 16);
            *(u32*)((u8*)r3 + (-12932)) = r0;
        } else {
            r3 = (u32)&lbl_803A9F08;
            r31 = 0x2;
            r3 = (u32)&lbl_803A9F08;
            r3 = r3 + (0x1 << 16);
            *(u32*)((u8*)r3 + (-12932)) = r30;
        }
    }
    r3 = (u32)&lbl_803A9F08;
    r4 = (u32)&lbl_803A9F08;
    r3 = r4 + (0x1 << 16);
    r0 = *(u32*)((u8*)r3 + (-12932));
    if ((s32)r0 != (s32)0x4) {
        r28 = r4 + 0x1;
        r27 = r4 + 0x30;
        r26 = 0x1;
        while ((s32)r26 < (s32)r31) {

            r0 = *(u8*)((u8*)r28 + 0x4);
            if (r0 == (u32)0x0) {
                ((void(*)(void))fn_8006B1D4)();
                r30 = r3 & 0xFFFF;
                r3 = r26;
                fn_8025D89C();
                r29 = r3 & 0xFFFF;
                if (r29 < r30) {
                } else {

                    r29 = r30;
                }
                r3 = r26;
                fn_8025D560();
                r0 = r29 & 0xFFFF;
                if ((s32)r3 == (s32)r0) {
                    r3 = r26;
                    fn_8025D560();
                    r30 = r3;
                    ((void(*)(void))fn_8006B1D4)();
                    r29 = r3 & 0xFFFF;
                    r3 = r26;
                    fn_8025D89C();
                    r0 = r3 & 0xFFFF;
                    if (r0 < r29) {
                    } else {

                        r0 = r29;
                    }
                    r0 = r0 & 0xFFFF;
                    if ((s32)r30 == (s32)r0) {
                        /* subi r0, r30, 0x1 */;
                        if ((s32)r0 < (s32)0x0) {
                            r0 = 0x0;
                        }
                        r3 = r0 << 2;
                        f1 = *(f32*)&lbl_8047BFE8;
                        r3 = r3 + (0x1 << 16);
                        /* subi r3, r3, 0x3274 */;
                        f0 = *(f32*)(r27 + r3);
                        if (f1 == f0) {
                            r0 = 0x1;
                            *(u8*)((u8*)r28 + 0x4) = r0;
            }
            }
            }
            }
            r28 = r28 + 0x1;
            r27 = r27 + 0x30;
            r26 = r26 + 0x1;

        }
    }
    r3 = (u32)&lbl_803A9F08;
    r3 = (u32)&lbl_803A9F08;
    ctr_fn = (void(*)(void))r31;
    if ((s32)r31 > (s32)0x0) {
        do {
            r0 = *(u8*)((u8*)r3 + 0x4);
            if (r0 == (u32)0x0) {
                r3 = 0x0;
                return;
            }
            r3 = r3 + 0x1;
        } while (--ctr != 0);
    }
    r3 = 0x1;

    return;
}
#endif


/* 0x80069220 | size: 0x184 */
#if 1
asm void fn_80069220(void) {
#include "src/game/ui/ui_core_fn_80069220.inc"
}
#else
void fn_80069220(void) {
    extern void fn_800D3088();
    extern void fn_800D37CC();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    *(f64*)(sp + 0x20) = f31;
    /* psq_st f31, 0x28((u32)sp), 0, qr0 */;
    r31 = r3;
    fn_800D37CC();
    /* xoris r3, r3, 0x8000 */;
    r0 = (0x4330 << 16);
    f1 = *(f64*)&lbl_8047BFF0;
    *(u32*)(sp + 0x8) = r0;
    f0 = *(f64*)(sp + 0x8);
    f31 = f0 - f1;
    fn_800D3088();
    r0 = (0x4330 << 16);
    r4 = (u32)&lbl_803A9F08;
    f1 = *(f64*)&lbl_8047C020;
    *(u32*)(sp + 0x10) = r0;
    r5 = (u32)&lbl_803A9F08;
    r6 = r5 + (0x1 << 16);
    r3 = 0x0;
    f0 = *(f64*)(sp + 0x10);
    f0 = f0 - f1;
    f0 = f0 / f31;
    *(f32*)((u8*)r6 + (-12920)) = f0;
    do {
        r4 = r5 + (0x1 << 16);
        /* subi r4, r4, 0x3274 */;
        r0 = 0x6;
        ctr_fn = (void(*)(void))r0;
        do {
            f0 = *(f32*)((u8*)r4 + 0x0);
            f1 = *(f32*)((u8*)r4 + 0x18);
            if (f0 != f1) {
            f1 = f1 - f0;
            f2 = *(f32*)&lbl_8047C010;
            f0 = *(f32*)((u8*)r6 + (-12920));
            f1 = f2 * f1;
            f3 = f1 * f0;
            if (f3 > f2) {
                f3 = f2;
            }
            f0 = *(f32*)&lbl_8047C014;
            /* cror eq, lt, eq */;
            if (f3 == f0) {
                f3 = f0;
            }
            f1 = *(f32*)((u8*)r4 + 0x0);
            f0 = *(f32*)&lbl_8047BFE8;
            f1 = f1 + f3;
            *(f32*)((u8*)r4 + 0x0) = f1;
            f2 = *(f32*)((u8*)r4 + 0x18);
            f0 = *(f32*)((u8*)r4 + 0x0);
            f1 = f2 - f0;
            if (f3 > f0) {
            } else {

                f3 = -f3;
            }
            f0 = *(f32*)&lbl_8047BFE8;
            if (f1 > f0) {
                f0 = f1;
            } else {

                f0 = -f1;
            }
            /* cror eq, lt, eq */;
            if (f0 != f3) {
                f0 = *(f32*)&lbl_8047BFE8;
                if (f1 > f0) {
                } else {

                    f1 = -f1;
                }
                f0 = *(f32*)&lbl_8047C018;
                if ((f1 < f0)) {
            *(f32*)((u8*)r4 + 0x0) = f2;
                }
            } else {
            *(f32*)((u8*)r4 + 0x0) = f2;
            }
            }
            r4 = r4 + 0x4;
        } while (--ctr != 0);
        r5 = r5 + 0x30;
        r3 = r3 + 0x1;
    } while ((s32)r3 < (s32)0x4);
    r3 = (u32)&lbl_803A9F08;
    r3 = (u32)&lbl_803A9F08;
    r3 = r3 + (0x1 << 16);
    r0 = *(u32*)((u8*)r3 + (-12928));
    r0 = (s16)r0;
    *(u16*)((u8*)r31 + 0x84) = r0;
    /* psq_l f31, 0x28((u32)sp), 0, qr0 */;
    f31 = *(f64*)(sp + 0x20);
    return;
}
#endif


/* 0x800693A4 | size: 0x160 */
#if 1
asm void fn_800693A4(void) {
#include "src/game/ui/ui_core_fn_800693A4.inc"
}
#else
void fn_800693A4(void) {
    extern void fn_800D3088();
    extern void fn_800D37CC();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    *(f64*)(sp + 0x20) = f31;
    /* psq_st f31, 0x28((u32)sp), 0, qr0 */;
    fn_800D37CC();
    /* xoris r3, r3, 0x8000 */;
    r0 = (0x4330 << 16);
    f1 = *(f64*)&lbl_8047BFF0;
    *(u32*)(sp + 0x8) = r0;
    f0 = *(f64*)(sp + 0x8);
    f31 = f0 - f1;
    fn_800D3088();
    r0 = (0x4330 << 16);
    r4 = (u32)&lbl_803A9F08;
    f1 = *(f64*)&lbl_8047C020;
    *(u32*)(sp + 0x10) = r0;
    r5 = (u32)&lbl_803A9F08;
    r6 = r5 + (0x1 << 16);
    r3 = 0x0;
    f0 = *(f64*)(sp + 0x10);
    f0 = f0 - f1;
    f0 = f0 / f31;
    *(f32*)((u8*)r6 + (-12920)) = f0;
    do {
        r4 = r5 + (0x1 << 16);
        /* subi r4, r4, 0x3274 */;
        r0 = 0x6;
        ctr_fn = (void(*)(void))r0;
        do {
            f0 = *(f32*)((u8*)r4 + 0x0);
            f1 = *(f32*)((u8*)r4 + 0x18);
            if (f0 != f1) {
            f1 = f1 - f0;
            f2 = *(f32*)&lbl_8047C010;
            f0 = *(f32*)((u8*)r6 + (-12920));
            f1 = f2 * f1;
            f3 = f1 * f0;
            if (f3 > f2) {
                f3 = f2;
            }
            f0 = *(f32*)&lbl_8047C014;
            /* cror eq, lt, eq */;
            if (f3 == f0) {
                f3 = f0;
            }
            f1 = *(f32*)((u8*)r4 + 0x0);
            f0 = *(f32*)&lbl_8047BFE8;
            f1 = f1 + f3;
            *(f32*)((u8*)r4 + 0x0) = f1;
            f2 = *(f32*)((u8*)r4 + 0x18);
            f0 = *(f32*)((u8*)r4 + 0x0);
            f1 = f2 - f0;
            if (f3 > f0) {
            } else {

                f3 = -f3;
            }
            f0 = *(f32*)&lbl_8047BFE8;
            if (f1 > f0) {
                f0 = f1;
            } else {

                f0 = -f1;
            }
            /* cror eq, lt, eq */;
            if (f0 != f3) {
                f0 = *(f32*)&lbl_8047BFE8;
                if (f1 > f0) {
                } else {

                    f1 = -f1;
                }
                f0 = *(f32*)&lbl_8047C018;
                if ((f1 < f0)) {
            *(f32*)((u8*)r4 + 0x0) = f2;
                }
            } else {
            *(f32*)((u8*)r4 + 0x0) = f2;
            }
            }
            r4 = r4 + 0x4;
        } while (--ctr != 0);
        r5 = r5 + 0x30;
        r3 = r3 + 0x1;
    } while ((s32)r3 < (s32)0x4);
    /* psq_l f31, 0x28((u32)sp), 0, qr0 */;
    f31 = *(f64*)(sp + 0x20);
    return;
}
#endif


/* 0x80069504 | size: 0x160 */
#if 1
asm void fn_80069504(void) {
#include "src/game/ui/ui_core_fn_80069504.inc"
}
#else
void fn_80069504(void) {
    extern void fn_800D3088();
    extern void fn_800D37CC();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    *(f64*)(sp + 0x20) = f31;
    /* psq_st f31, 0x28((u32)sp), 0, qr0 */;
    fn_800D37CC();
    /* xoris r3, r3, 0x8000 */;
    r0 = (0x4330 << 16);
    f1 = *(f64*)&lbl_8047BFF0;
    *(u32*)(sp + 0x8) = r0;
    f0 = *(f64*)(sp + 0x8);
    f31 = f0 - f1;
    fn_800D3088();
    r0 = (0x4330 << 16);
    r4 = (u32)&lbl_803A9F08;
    f1 = *(f64*)&lbl_8047C020;
    *(u32*)(sp + 0x10) = r0;
    r5 = (u32)&lbl_803A9F08;
    r6 = r5 + (0x1 << 16);
    r3 = 0x0;
    f0 = *(f64*)(sp + 0x10);
    f0 = f0 - f1;
    f0 = f0 / f31;
    *(f32*)((u8*)r6 + (-12920)) = f0;
    do {
        r4 = r5 + (0x1 << 16);
        /* subi r4, r4, 0x3274 */;
        r0 = 0x6;
        ctr_fn = (void(*)(void))r0;
        do {
            f0 = *(f32*)((u8*)r4 + 0x0);
            f1 = *(f32*)((u8*)r4 + 0x18);
            if (f0 != f1) {
            f1 = f1 - f0;
            f2 = *(f32*)&lbl_8047C010;
            f0 = *(f32*)((u8*)r6 + (-12920));
            f1 = f2 * f1;
            f3 = f1 * f0;
            if (f3 > f2) {
                f3 = f2;
            }
            f0 = *(f32*)&lbl_8047C014;
            /* cror eq, lt, eq */;
            if (f3 == f0) {
                f3 = f0;
            }
            f1 = *(f32*)((u8*)r4 + 0x0);
            f0 = *(f32*)&lbl_8047BFE8;
            f1 = f1 + f3;
            *(f32*)((u8*)r4 + 0x0) = f1;
            f2 = *(f32*)((u8*)r4 + 0x18);
            f0 = *(f32*)((u8*)r4 + 0x0);
            f1 = f2 - f0;
            if (f3 > f0) {
            } else {

                f3 = -f3;
            }
            f0 = *(f32*)&lbl_8047BFE8;
            if (f1 > f0) {
                f0 = f1;
            } else {

                f0 = -f1;
            }
            /* cror eq, lt, eq */;
            if (f0 != f3) {
                f0 = *(f32*)&lbl_8047BFE8;
                if (f1 > f0) {
                } else {

                    f1 = -f1;
                }
                f0 = *(f32*)&lbl_8047C018;
                if ((f1 < f0)) {
            *(f32*)((u8*)r4 + 0x0) = f2;
                }
            } else {
            *(f32*)((u8*)r4 + 0x0) = f2;
            }
            }
            r4 = r4 + 0x4;
        } while (--ctr != 0);
        r5 = r5 + 0x30;
        r3 = r3 + 0x1;
    } while ((s32)r3 < (s32)0x4);
    /* psq_l f31, 0x28((u32)sp), 0, qr0 */;
    f31 = *(f64*)(sp + 0x20);
    return;
}
#endif


/* 0x80069664 | size: 0x160 */
#if 1
asm void fn_80069664(void) {
#include "src/game/ui/ui_core_fn_80069664.inc"
}
#else
void fn_80069664(void) {
    extern void fn_800D3088();
    extern void fn_800D37CC();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    *(f64*)(sp + 0x20) = f31;
    /* psq_st f31, 0x28((u32)sp), 0, qr0 */;
    fn_800D37CC();
    /* xoris r3, r3, 0x8000 */;
    r0 = (0x4330 << 16);
    f1 = *(f64*)&lbl_8047BFF0;
    *(u32*)(sp + 0x8) = r0;
    f0 = *(f64*)(sp + 0x8);
    f31 = f0 - f1;
    fn_800D3088();
    r0 = (0x4330 << 16);
    r4 = (u32)&lbl_803A9F08;
    f1 = *(f64*)&lbl_8047C020;
    *(u32*)(sp + 0x10) = r0;
    r5 = (u32)&lbl_803A9F08;
    r6 = r5 + (0x1 << 16);
    r3 = 0x0;
    f0 = *(f64*)(sp + 0x10);
    f0 = f0 - f1;
    f0 = f0 / f31;
    *(f32*)((u8*)r6 + (-12920)) = f0;
    do {
        r4 = r5 + (0x1 << 16);
        /* subi r4, r4, 0x3274 */;
        r0 = 0x6;
        ctr_fn = (void(*)(void))r0;
        do {
            f0 = *(f32*)((u8*)r4 + 0x0);
            f1 = *(f32*)((u8*)r4 + 0x18);
            if (f0 != f1) {
            f1 = f1 - f0;
            f2 = *(f32*)&lbl_8047C010;
            f0 = *(f32*)((u8*)r6 + (-12920));
            f1 = f2 * f1;
            f3 = f1 * f0;
            if (f3 > f2) {
                f3 = f2;
            }
            f0 = *(f32*)&lbl_8047C014;
            /* cror eq, lt, eq */;
            if (f3 == f0) {
                f3 = f0;
            }
            f1 = *(f32*)((u8*)r4 + 0x0);
            f0 = *(f32*)&lbl_8047BFE8;
            f1 = f1 + f3;
            *(f32*)((u8*)r4 + 0x0) = f1;
            f2 = *(f32*)((u8*)r4 + 0x18);
            f0 = *(f32*)((u8*)r4 + 0x0);
            f1 = f2 - f0;
            if (f3 > f0) {
            } else {

                f3 = -f3;
            }
            f0 = *(f32*)&lbl_8047BFE8;
            if (f1 > f0) {
                f0 = f1;
            } else {

                f0 = -f1;
            }
            /* cror eq, lt, eq */;
            if (f0 != f3) {
                f0 = *(f32*)&lbl_8047BFE8;
                if (f1 > f0) {
                } else {

                    f1 = -f1;
                }
                f0 = *(f32*)&lbl_8047C018;
                if ((f1 < f0)) {
            *(f32*)((u8*)r4 + 0x0) = f2;
                }
            } else {
            *(f32*)((u8*)r4 + 0x0) = f2;
            }
            }
            r4 = r4 + 0x4;
        } while (--ctr != 0);
        r5 = r5 + 0x30;
        r3 = r3 + 0x1;
    } while ((s32)r3 < (s32)0x4);
    /* psq_l f31, 0x28((u32)sp), 0, qr0 */;
    f31 = *(f64*)(sp + 0x20);
    return;
}
#endif


/* 0x800697C4 | size: 0x30 */
#if 0
asm s32 fn_800697C4(void) {
#include "src/game/ui/ui_core_fn_800697C4.inc"
}
#else
s32 fn_800697C4(void) {
#pragma peephole off
    fn_8010B01C(0, fn_800697F4, 0);
}
#endif


/* 0x800697F4 | size: 0x150 */
#if 1
asm void fn_800697F4(void) {
#include "src/game/ui/ui_core_fn_800697F4.inc"
}
#else
void fn_800697F4(void) {
    extern u32 fn_80061018();
    extern void fn_8010BBB8();
    extern void fn_80123FBC();
    extern void fn_8025D808();
    extern void fn_8025D938();
    extern s32 fn_8025D970();
    extern void fn_8025DA3C();
    extern s32 fn_8025DA88();
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

    
    r26 = r3;
    r29 = 0x1;
    fn_8025DA3C();
    fn_8025DA88();
    if (r26 != (u32)0x0) {
        r0 = 0x1;
        *(u8*)((u8*)r26 + 0x0) = r0;
    }
    r3 = (u32)&lbl_803A9F08;
    r31 = (u32)&lbl_803A9F08;
    r30 = r31 + (0x1 << 16);
    do {
        r4 = *(u32*)((u8*)r31 + 0x2C);
        if ((s32)r4 == (s32)0x18) {
            r0 = 0x1;
            r29 = 0x0;
            *(u8*)((u8*)r30 + (-12924)) = r0;
            r27 = 0x0;
        } else {
        r3 = (0x2aab << 16);
        /* subi r0, r3, 0x5555 */;
        r3 = (s32)((s64)r0 * (s64)r4 >> 32);
        r0 = (u32)r3 >> 31;
        r28 = r3 + r0;
        r0 = r28 * 0x6;
        r26 = r4 - r0;
        fn_80061018();
        if ((s32)r3 == (s32)0x0) {
            r3 = r28;
            r4 = r26;
            fn_8025D970();
            r27 = r3;
        } else {
            r3 = r28;
            fn_8025D808();
            r0 = r3 & 0xFFFF;
            if ((s32)r0 <= (s32)r26) {
                r27 = 0x0;
            } else {
                r3 = r28;
                r4 = r26;
                fn_8025D938();
                r27 = r3;
            }
        }
        r5 = r28 * 0x48;
        r4 = (u32)&lbl_803A9F08;
        r3 = r27;
        r4 = (u32)&lbl_803A9F08;
        r0 = r26 * 0xc;
        r4 = r4 + r5;
        r28 = r4 + r0;
        r28 = r28 + 0x30;
        fn_80123FBC();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x0) {
            r3 = r27;
            fn_8010BBB8();
            r0 = 0x1;
            r29 = 0x0;
            *(u8*)((u8*)r28 + 0x0) = r0;
            *(u16*)((u8*)r28 + 0x2) = r3;
            r3 = *(u32*)((u8*)r31 + 0x2C);
            r0 = r3 + 0x1;
            *(u32*)((u8*)r31 + 0x2C) = r0;
        } else {
            r0 = 0x0;
            *(u8*)((u8*)r28 + 0x0) = r0;
            r3 = *(u32*)((u8*)r31 + 0x2C);
            r0 = r3 + 0x1;
            *(u32*)((u8*)r31 + 0x2C) = r0;
        }
        }
    } while ((s32)r29 != (s32)0x0);
    r3 = r27;
    return;
}
#endif


/* 0x80069944 | size: 0xC4 */
#if 1
asm void fn_80069944(void) {
#include "src/game/ui/ui_core_fn_80069944.inc"
}
#else
void fn_80069944(void) {
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 ctr = 0;

    r4 = (u32)&lbl_803A9F08;
    *(u32*)((u8*)r4 + 0x2C) = 0;
    r3 = r4 + (0x1 << 16);
    *(u8*)((u8*)r3 + (-12924)) = 0;
    ctr = 2;
    do {
        r3 = r4 + 0x30;
        r0 = 0x0;
        *(u8*)((u8*)r3 + 0x0) = r0;
        r9 = r3 + 0xc;
        r8 = r3 + 0x18;
        r7 = r3 + 0x24;
        *(u32*)((u8*)r3 + 0x4) = r0;
        r6 = r3 + 0x30;
        r5 = r3 + 0x3c;
        r4 = r4 + 0x48;
        *(u8*)((u8*)r9 + 0x0) = r0;
        r3 = r4 + 0x30;
        r4 = r4 + 0x48;
        *(u32*)((u8*)r9 + 0x4) = r0;
        r9 = r3 + 0xc;
        *(u8*)((u8*)r8 + 0x0) = r0;
        *(u32*)((u8*)r8 + 0x4) = r0;
        r8 = r3 + 0x18;
        *(u8*)((u8*)r7 + 0x0) = r0;
        *(u32*)((u8*)r7 + 0x4) = r0;
        r7 = r3 + 0x24;
        *(u8*)((u8*)r6 + 0x0) = r0;
        *(u32*)((u8*)r6 + 0x4) = r0;
        r6 = r3 + 0x30;
        *(u8*)((u8*)r5 + 0x0) = r0;
        *(u32*)((u8*)r5 + 0x4) = r0;
        r5 = r3 + 0x3c;
        *(u8*)((u8*)r3 + 0x0) = r0;
        *(u32*)((u8*)r3 + 0x4) = r0;
        *(u8*)((u8*)r9 + 0x0) = r0;
        *(u32*)((u8*)r9 + 0x4) = r0;
        *(u8*)((u8*)r8 + 0x0) = r0;
        *(u32*)((u8*)r8 + 0x4) = r0;
        *(u8*)((u8*)r7 + 0x0) = r0;
        *(u32*)((u8*)r7 + 0x4) = r0;
        *(u8*)((u8*)r6 + 0x0) = r0;
        *(u32*)((u8*)r6 + 0x4) = r0;
        *(u8*)((u8*)r5 + 0x0) = r0;
        *(u32*)((u8*)r5 + 0x4) = r0;
    } while (--ctr != 0);
}
#endif


/* 0x80069A08 | size: 0x58 */
#if 0
asm s32 fn_80069A08(s32 a, s32 b, s32 c, s32 d) {
#include "src/game/ui/ui_core_fn_80069A08.inc"
}
#else
s32 fn_80069A08(s32 a, s32 b, s32 c, s32 d) {
#pragma peephole off
    u8* entry = lbl_803A9F08 + c * 0x48;
    entry += d * 0xC;
    entry = (u8*)((u32)entry + 0x30);
    if (entry[0] != 0) {
        fn_8010B9E8(a, b, *(u16*)(entry + 2));
        return 1;
    }
    return 0;
}
#endif
