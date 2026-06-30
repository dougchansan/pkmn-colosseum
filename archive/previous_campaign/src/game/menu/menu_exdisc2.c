/**
 * @file menu_exdisc2.c
 * @brief Extra disc coupon and related menus (0x80078D38-0x8007C2C0)
 *
 * Address range: 0x80078D38 - 0x8007C2C0
 * Total functions: 25
 */

#include "dolphin/types.h"

/* ===== External function declarations ===== */
extern void fn_8001E184();
extern void fn_80029850();
extern void fn_800298DC();
extern void fn_8002DC6C();
extern void fn_80075A9C();
extern void fn_80075AC0();
extern void fn_80075AE4();
extern void fn_80075B08();
extern void fn_80075B2C();
extern void fn_80075B50();
extern void fn_80075BC4();
extern void fn_80075C20();
extern void fn_80075C44();
extern void fn_80077ED4();
extern void fn_80078390();
extern void fn_800788BC();
extern void fn_800849B4();
extern void fn_80092C90();
extern void fn_80093574();
extern void fn_80093610();
extern void fn_80093698();
extern void fn_8009A9D8();
extern void fn_8009AAD4();
extern void fn_8009F1D0();
extern void fn_800A19CC();
extern void fn_800A1E54();
extern void fn_800A1F94();
extern void fn_800A221C();
extern void fn_800A501C();
extern void fn_800A50E4();
extern void fn_800A541C();
extern void fn_800A7BCC();
extern void fn_800C8174();
extern void strcpy();
extern void fn_800D0F44();
extern void fn_800D3088();
extern void fn_800D37CC();
extern void fn_800E4014();
extern void _threadSwitch();
extern void fn_800F9318();
extern void fn_800FA444();
extern void fn_800FB680();
extern void fn_800FF58C();
extern void fn_80102510();
extern void fn_8010264C();
extern void fn_801067E8();
extern void fn_801069FC();
extern void fn_80106D3C();
extern void fn_80109220();
extern void fn_80113828();
extern void fn_80113F48();
extern void fn_801159F0();
extern void fn_80115BD8();
extern void fn_80123FBC();
extern void fn_80124A60();
extern void fn_80128DD4();
extern void fn_80129280();
extern void fn_8012A5B0();
extern void fn_8012AC08();
extern void fn_80130770();
/* ... and 24 more external functions */
extern void OSCreateAlarm();
extern void OSDisableInterrupts();
extern void OSGetTick();
extern void OSRestoreInterrupts();
extern void OSSetAlarm();
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_804788F0;
extern u8 lbl_80478930;
extern u8 lbl_80478940;
extern u8 lbl_80478980;
extern u8 lbl_8047A620;
extern u8 lbl_8047A628;
extern u8 lbl_8047A62C;
extern u8 lbl_8047A630;
extern u8 lbl_8047A631;
extern u8 lbl_8047A632;
extern u8 lbl_8047A633;
extern u8 lbl_8047A634;
extern u8 lbl_8047A635;
extern u8 lbl_8047A638;
extern u8 lbl_8047A640;
extern u8 lbl_8047A648;
extern u8 lbl_8047A64C;
extern u8 lbl_8047A650;
extern u8 lbl_8047C0E0;
extern u8 lbl_8047C0E4;
extern u8 lbl_8047C0F0;
extern u8 lbl_8047C0F8;
extern u8 lbl_8047C100;
extern u8 lbl_8047C104;
extern u8 lbl_8047C108;
extern u8 lbl_8047C10C;
extern u8 lbl_8047C114;
extern u8 lbl_8047C118;
extern u8 lbl_8047C120;
extern u8 lbl_8047C128;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EE4D8[];
extern u8 jumptable_802EE51C[];
extern u8 jumptable_802EE550[];
extern u8 jumptable_802EE594[];
extern u8 jumptable_802EE5C0[];
extern u8 lbl_80268AA8[];
extern u8 lbl_80268AD0[];
extern u8 lbl_80268AE0[];
extern u8 lbl_802E61D8[];
extern u8 lbl_802EE508[];
extern u8 lbl_802EE608[];
extern u8 lbl_803F6E40[];
extern u8 lbl_803F6F18[];
extern u8 lbl_803F7A30[];
extern u8 lbl_803FADF8[];
extern u8 lbl_803FAEF8[];

/* ===== Forward declarations ===== */
s32 fn_80078D38(void);
void fn_80078D5C(void);
void fn_8007926C(void);
void fn_800792D8(void);
void fn_800798E8(void);
u32 fn_80079C1C();
void fn_80079EF4(void);
void fn_8007A5E8(s32 r3, u8 *r4);
void fn_8007A664(s32 r3, u8 *r4);
void fn_8007A6F0(void);
s32 fn_8007A82C(void);
void fn_8007A850(void);
void fn_8007AA6C(void);
s32 fn_8007AAA8(void);
void fn_8007AAFC(void);
void fn_8007AB10(void);
void fn_8007B090(u8 *r3);
void fn_8007B0D8(void);
void fn_8007B114(void);
void fn_8007B350(void);
void fn_8007B6A4(u8 *r3);
void fn_8007B6D8(void);
void fn_8007C23C(u8 *r3);
void fn_8007C260(void);
void fn_8007C26C(void);

/* ===== Function implementations ===== */

/* 0x80078D38 | size: 0x24 */
#pragma push
#pragma scheduling off
s32 fn_80078D38(void) {
    extern s32 fn_801C40F0(s32);
    return fn_801C40F0(1);
}
#pragma pop

/* 0x80078D5C | size: 0x510 */
void fn_80078D5C(void) {
    extern void fn_80132A38();
    extern void fn_80166A28();
    extern void fn_80176E0C();
    extern void GSscene_SetMode();
    extern void fn_801C40F0();
    extern void fn_801C41C8();
    extern void fn_801CB834();
    extern void fn_801CB9D8();
    extern void fn_801CBA0C();
    extern void fn_801D0314();
    extern void fn_801D036C();
    extern void fn_801EE398();
    extern u8 jumptable_802EE4D8[];
    u8 sp[0x90];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f9 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;

    fn_801D036C();
    r31 = r3;
    while (1) {
        r0 = *(u32*)&lbl_8047A620;
        if ((s32)r0 <= 0x0) break;
        if (r0 <= 0xb) {
            r3 = (u32)jumptable_802EE4D8;
            r0 = r0 << 2;
            r3 = (u32)jumptable_802EE4D8;
            r0 = *(u32*)(r3 + r0);
            ctr_fn = (void(*)(void))r0;
            ((void(*)(void))fn_80113F48)();
            r4 = (0x1095 << 16);
            r5 = 0x0;
            r4 = r4 + 0x1800;
            r6 = 0x0;
            fn_80176E0C();
            r3 = (0x104f << 16);
            r4 = 0x6;
            r3 = r3 + 0x1000;
            r5 = 0x0;
            r6 = 0x1;
            fn_801CB834();
            ((void(*)(void))fn_80075C44)();
            r0 = r3 & 0xFF;
            if (r0 == 0x1) {
                r0 = 0x2;
                *(u32*)&lbl_8047A620 = r0;
                continue;
            }
            r0 = 0x3;
            *(u32*)&lbl_8047A620 = r0;
            continue;
            f1 = *(f32*)&lbl_8047C100;
            r3 = 0x3;
            fn_801C41C8();
            r3 = 0x1;
            fn_801C40F0();
            r3 = (0x104f << 16);
            r4 = 0x0;
            r3 = r3 + 0x1000;
            r5 = 0x0;
            r6 = 0x1;
            fn_801CB834();
            f1 = *(f32*)&lbl_8047C100;
            r3 = 0x2;
            fn_801C41C8();
            r3 = 0x1;
            fn_801C40F0();
            r3 = 0x1;
            fn_801C40F0();
            r3 = 0x43c3;
            r4 = 0x1;
            r5 = 0x0;
            ((void(*)(void))fn_801067E8)();
            r3 = 0x1;
            ((void(*)(void))fn_801069FC)();
            ((void(*)(void))fn_80075C20)();
            r0 = r3 & 0xFF;
            if (r0 == 0x1) {
                r0 = 0x5;
                *(u32*)&lbl_8047A620 = r0;
                continue;
            }
            r0 = 0x4;
            *(u32*)&lbl_8047A620 = r0;
            continue;
            f1 = *(f32*)&lbl_8047C100;
            r3 = 0x3;
            fn_801C41C8();
            r3 = 0x1;
            fn_801C40F0();
            r3 = (0x104f << 16);
            r4 = 0x1;
            r3 = r3 + 0x1000;
            r5 = 0x0;
            r6 = 0x1;
            fn_801CB834();
            f1 = *(f32*)&lbl_8047C100;
            r3 = 0x2;
            fn_801C41C8();
            r3 = 0x1;
            fn_801C40F0();
            r3 = 0x1;
            fn_801C40F0();
            r3 = 0x43c0;
            r4 = 0x1;
            r5 = 0x0;
            ((void(*)(void))fn_801067E8)();
            r3 = 0x3f9;
            fn_80166A28();
            f27 = *(f32*)&lbl_8047C0E0;
            f28 = *(f64*)&lbl_8047C0F0;
            r30 = (0x4330 << 16);
            f30 = *(f64*)&lbl_8047C0F8;
            f31 = *(f32*)&lbl_8047C0E4;
            while (f27 < f31) {

                ((void(*)(void))_threadSwitch)();
                ((void(*)(void))fn_800D37CC)();
                *(u32*)(sp + 0x1C) = r0;
                f29 = f0 - f28;
                ((void(*)(void))fn_800D3088)();
                f0 = f0 - f30;
                f0 = f0 / f29;
                f27 = f27 + f0;

            }
            r0 = 0x0;
            *(u32*)&lbl_8047A620 = r0;
            continue;
            fn_801EE398();
            r0 = r3 & 0xFF;
            if (r0 == 0x1) {
                r0 = 0x7;
                *(u32*)&lbl_8047A620 = r0;
                continue;
            }
            r0 = 0x6;
            *(u32*)&lbl_8047A620 = r0;
            continue;
            r3 = 0x43af;
            r4 = 0x1;
            r5 = 0x0;
            ((void(*)(void))fn_801067E8)();
            r3 = 0x1;
            ((void(*)(void))fn_801069FC)();
            ((void(*)(void))fn_80075BC4)();
            if (r3 < 0x1) {
                r3 = 0x43b2;
                r4 = 0x1;
                r5 = 0x0;
                ((void(*)(void))fn_801067E8)();
                r3 = 0x1;
                ((void(*)(void))fn_801069FC)();
                r0 = 0x0;
                *(u32*)&lbl_8047A620 = r0;
                continue;
            }
            r4 = r3;
            r3 = 0x2f;
            fn_80132A38();
            r3 = 0x43bb;
            r4 = 0x1;
            r5 = 0x0;
            ((void(*)(void))fn_801067E8)();
            ((void(*)(void))fn_8001E184)();
            r30 = r3;
            r3 = 0x1;
            ((void(*)(void))fn_801069FC)();
            r0 = (s8)r30;
            if ((s32)r0 != 0x0) {
                if ((s32)r0 < 0x0) {
                    if ((s32)r0 < (s32)-0x1) {
                        goto L_8007904C;
                    }
                    if ((s32)r0 >= 0x2) goto L_8007904C;
                    }
                r3 = 0x43c1;
                r4 = 0x1;
                r5 = 0x0;
                ((void(*)(void))fn_801067E8)();
                r0 = 0x0;
                *(u32*)&lbl_8047A620 = r0;
                continue;
            }
        L_8007904C: ;
            r3 = 0x43c4;
            r4 = 0x1;
            r5 = 0x0;
            ((void(*)(void))fn_801067E8)();
            r3 = 0x1;
            ((void(*)(void))fn_801069FC)();
            r0 = 0x8;
            *(u32*)&lbl_8047A620 = r0;
            continue;
            r3 = 0x43c6;
            r4 = 0x1;
            r5 = 0x0;
            ((void(*)(void))fn_801067E8)();
            r3 = 0x1;
            ((void(*)(void))fn_801069FC)();
            r0 = 0x0;
            *(u32*)&lbl_8047A620 = r0;
            continue;
            r3 = r31;
            ((void(*)(void))fn_800788BC)();
            continue;
            r3 = (u32)&lbl_80268AA8;
            r6 = (u32)&lbl_80268AA8;
            r5 = *(u32*)((u8*)r6 + 0x0);
            r4 = *(u32*)((u8*)r6 + 0x4);
            r3 = *(u32*)((u8*)r6 + 0x8);
            r0 = *(u32*)((u8*)r6 + 0xC);
            *(u32*)(sp + 0x14) = r0;
            ((void(*)(void))fn_80113F48)();
            f1 = *(f32*)&lbl_8047C104;
            r28 = r3;
            r3 = 0x3;
            fn_801C41C8();
            r3 = 0x1;
            fn_801C40F0();
            r4 = (0x104f << 16);
            r3 = r28;
            r4 = r4 + 0x1000;
            ((void(*)(void))fn_800F9318)();
            r30 = r3;
            if (r30 != 0x0) {
                r4 = 0x0;
                ((void(*)(void))fn_800E4014)();
            }
            r3 = (0xffe << 16);
            r3 = r3 + 0x1000;
            fn_801CBA0C();
            r29 = r3;
            r3 = r28;
            r4 = r29;
            ((void(*)(void))fn_800F9318)();
            r4 = (0xfff << 16);
            r3 = 0x5d5;
            r4 = r4 + 0x1800;
            r5 = 0x0;
            r6 = 0x1;
            fn_80176E0C();
            r3 = 0x4;
            GSscene_SetMode();
            f1 = *(f32*)&lbl_8047C104;
            r3 = 0x2;
            fn_801C41C8();
            r3 = 0x1;
            fn_801C40F0();
            r4 = (u32)&lbl_803F6F18;
            r3 = (u32)&lbl_803F6E40;
            r0 = (u32)&lbl_803F6F18;
            r5 = (u32)sp + 0x8;
            *(u32*)(sp + 0xC) = r0;
            r6 = (u32)&lbl_803F6E40;
            r3 = 0x0;
            r4 = 0x20;
            ((void(*)(void))fn_800849B4)();
            if ((s32)r3 < 0x0) {
                r0 = 0x0;
                *(u32*)&lbl_8047A620 = r0;
                continue;
            }
            f1 = *(f32*)&lbl_8047C104;
            r3 = 0x3;
            fn_801C41C8();
            r3 = 0x1;
            fn_801C40F0();
            r3 = r29;
            fn_801CB9D8();
            if (r30 != 0x0) {
                r3 = r30;
                r4 = 0x1;
                ((void(*)(void))fn_800E4014)();
                r4 = (0x1095 << 16);
                r3 = r28;
                r4 = r4 + 0x1800;
                r5 = 0x0;
                r6 = 0x0;
                fn_80176E0C();
            }
            f1 = *(f32*)&lbl_8047C104;
            r3 = 0x2;
            fn_801C41C8();
            r3 = 0x1;
            fn_801C40F0();
            r0 = 0xa;
            *(u32*)&lbl_8047A620 = r0;
            continue;
            ((void(*)(void))fn_80078390)();
            continue;
            ((void(*)(void))fn_80077ED4)();
        }
    }
    r3 = r31;
    fn_801D0314();
    r3 = 0x321;
    r4 = 0x0;
    ((void(*)(void))fn_80113828)();
    return;
}

/* 0x8007926C | size: 0x6C */
void fn_8007926C(void) {
    extern void fn_801CB61C();
    extern void fn_801CB834();
    extern void fn_801CB954();
    extern void fn_801CBA0C();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r31 = 0;

    tmp = 0x1;
    r3 = 0x10BD0000;
    *(u32*)&lbl_8047A620 = tmp;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    r4 = 0x1;
    r31 = r3;
    fn_801CB954();
    r4 = 0x104F0000;
    r3 = r31;
    r4 = r4 + 0x1000;
    r5 = 0x207;
    fn_801CB61C();
    r3 = r31;
    r4 = 0x0;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    return;
}

/* 0x800792D8 | size: 0x610 */
void fn_800792D8(void) {
    extern u32 fn_80079C1C();
    extern void fn_80079EF4();
    extern void fn_80176E0C();
    extern void GSscene_SetMode();
    extern void __assert();
    extern void fn_801C40F0();
    extern void fn_801C41C8();
    extern void fn_801CB9D8();
    extern void fn_801CBA0C();
    u8 sp[0xC20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r3 = (u32)&lbl_80268AD0;
    f1 = *(f32*)&lbl_8047C108;
    r7 = (u32)&lbl_80268AD0;
    r31 = 0x0;
    r6 = *(u32*)((u8*)r7 + 0x0);
    r3 = 0x3;
    r5 = *(u32*)((u8*)r7 + 0x4);
    r4 = *(u32*)((u8*)r7 + 0x8);
    tmp = *(u32*)((u8*)r7 + 0xC);
    *(u32*)(sp + 0x14) = tmp;
    fn_801C41C8();
    r3 = 0x1;
    fn_801C40F0();
    r3 = 0xe1;
    ((void(*)(void))fn_80102510)();
    ((void(*)(void))fn_80113F48)();
    r4 = 0x104E0000;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    if (r3 != 0) {
        r4 = 0x0;
        ((void(*)(void))fn_800E4014)();
    }
    ((void(*)(void))fn_80113F48)();
    r4 = 0x104E0000;
    r4 = r4 + 0x1001;
    ((void(*)(void))fn_800F9318)();
    if (r3 != 0) {
        r4 = 0x0;
        ((void(*)(void))fn_800E4014)();
    }
    ((void(*)(void))fn_80113F48)();
    r4 = 0x104E0000;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    if (r3 != 0) {
        r4 = 0x0;
        ((void(*)(void))fn_800E4014)();
    }
    r3 = 0xFFE0000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    r30 = r3;
    ((void(*)(void))fn_80113F48)();
    r4 = r30;
    ((void(*)(void))fn_800F9318)();
    r4 = 0xFFF0000;
    r3 = 0x5d4;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x1;
    fn_80176E0C();
    r3 = 0x4;
    GSscene_SetMode();
    f1 = *(f32*)&lbl_8047C108;
    r3 = 0x2;
    fn_801C41C8();
    r3 = 0x1;
    fn_801C40F0();
    tmp = (u32)sp + 0xf0;
    r5 = (u32)sp + 0x8;
    *(u32*)(sp + 0xC) = tmp;
    r6 = (u32)sp + 0x18;
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_800849B4)();
    if ((s32)r3 < 0) {
        tmp = 0x1;
        f1 = *(f32*)&lbl_8047C108;
        *(u32*)&lbl_8047A638 = tmp;
        r3 = 0x3;
        fn_801C41C8();
        r3 = 0x1;
        fn_801C40F0();
        r3 = r30;
        fn_801CB9D8();
        ((void(*)(void))fn_80113F48)();
        r4 = 0x104E0000;
        r4 = r4 + 0x1000;
        ((void(*)(void))fn_800F9318)();
        if (r3 != 0) {
            r4 = 0x1;
            ((void(*)(void))fn_800E4014)();
        }
        ((void(*)(void))fn_80113F48)();
        r4 = 0x104E0000;
        r4 = r4 + 0x1001;
        ((void(*)(void))fn_800F9318)();
        if (r3 != 0) {
            r4 = 0x1;
            ((void(*)(void))fn_800E4014)();
        }
        ((void(*)(void))fn_80113F48)();
        r4 = 0x104E0000;
        r4 = r4 + 0x1002;
        ((void(*)(void))fn_800F9318)();
        if (r3 != 0) {
            r4 = 0x1;
            ((void(*)(void))fn_800E4014)();
        }
        ((void(*)(void))fn_80113F48)();
        r4 = 0x10940000;
        r5 = 0x0;
        r4 = r4 + 0x1800;
        r6 = 0x0;
        fn_80176E0C();
        f1 = *(f32*)&lbl_8047C108;
        r3 = 0x2;
        fn_801C41C8();
        r3 = 0x1;
        fn_801C40F0();
        return;
    }
    f1 = *(f32*)&lbl_8047C108;
    r3 = 0x3;
    fn_801C41C8();
    r3 = 0x1;
    fn_801C40F0();
    r3 = r30;
    fn_801CB9D8();
    ((void(*)(void))fn_80113F48)();
    r4 = 0x104E0000;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    if (r3 != 0) {
        r4 = 0x1;
        ((void(*)(void))fn_800E4014)();
    }
    ((void(*)(void))fn_80113F48)();
    r4 = 0x104E0000;
    r4 = r4 + 0x1001;
    ((void(*)(void))fn_800F9318)();
    if (r3 != 0) {
        r4 = 0x1;
        ((void(*)(void))fn_800E4014)();
    }
    ((void(*)(void))fn_80113F48)();
    r4 = 0x104E0000;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    if (r3 != 0) {
        r4 = 0x1;
        ((void(*)(void))fn_800E4014)();
    }
    ((void(*)(void))fn_80113F48)();
    r4 = 0x10940000;
    r5 = 0x0;
    r4 = r4 + 0x1800;
    r6 = 0x0;
    fn_80176E0C();
    f1 = *(f32*)&lbl_8047C108;
    r3 = 0x2;
    fn_801C41C8();
    r3 = 0x1;
    fn_801C40F0();
    r3 = 0x0;
    r10 = tmp & 0x00000004;
    r8 = tmp & 0x00000002;
    r6 = tmp & 0x1;
    r9 = -r10;
    r7 = -r8;
    r5 = -r6;
    *(u32*)&lbl_8047A62C = tmp;
    tmp = r5 | r6;
    r9 = r9 | r10;
    r7 = r7 | r8;
    *(u32*)&lbl_8047A628 = r4;
    r6 = (u32)r9 >> 31;
    tmp = (u32)tmp >> 31;
    r5 = (u32)r7 >> 31;
    *(u8*)&lbl_8047A635 = r6;
    *(u8*)&lbl_8047A634 = r5;
    *(u8*)&lbl_8047A633 = tmp;
    fn_80079EF4();
    tmp = r3 & 0xFF;
    if (tmp == 0) return;
    tmp = *(u8*)&lbl_8047A632;
    r29 = 0x1;
    if (tmp != 0) {
        r30 = (u32)sp + 0x28;
        r4 = *(u16*)(sp + 0x26);
        r3 = r30;
        r5 = 0x47;
        r6 = 0x3e7;
        ((void(*)(void))fn_80029850)();
        if ((s32)r3 < 1) {
            r29 = 0x0;
            goto L_8007962C;
        }
        r4 = *(u16*)(sp + 0x26);
        r3 = r30;
        r5 = 0x47;
        r6 = 0x1;
        r7 = -0x1;
        r8 = 0x3e7;
        ((void(*)(void))fn_800298DC)();
        tmp = tmp | 0x4;
        *(u32*)(sp + 0x20) = tmp;
    }
L_8007962C:
    tmp = *(u8*)&lbl_8047A630;
    if (tmp != 0) {
        r4 = *(u16*)(sp + 0x26);
        r3 = (u32)sp + 0x28;
        r5 = 0x1;
        r6 = 0x3e7;
        ((void(*)(void))fn_80029850)();
        if ((s32)r3 < 1) {
            r29 = 0x0;
    }
    }
    tmp = *(u8*)&lbl_8047A631;
    r30 = 0x1;
    if (tmp != 0) {
        r3 = *(u16*)(sp + 0x24);
        tmp = (u32)tmp >> 31;
        r30 = tmp;
    }
    r3 = (u32)sp + 0xf0;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_8012A5B0)();
    r6 = r3;
    r4 = r29;
    r5 = r30;
    r3 = 0x0;
    fn_80079C1C();
    tmp = r3 & 0xFF;
    if (tmp == 0) return;
    r3 = 0x2;
    r4 = 0x3d3b;
    r5 = 0x0;
    r6 = 0x1;
    ((void(*)(void))fn_80106D3C)();
    tmp = *(u8*)&lbl_8047A631;
    if (tmp != 0) {
        r29 = 0x0;
        while (1) {
            tmp = r29 & 0xFFFF;
            if (tmp >= 6) break;
            r4 = r29;
            r3 = (u32)sp + 0xf0;
            ((void(*)(void))fn_8012AC08)();
            ((void(*)(void))fn_80124A60)();
            r29 = r29 + 0x1;

        }
        ((void(*)(void))fn_80115BD8)();
        ((void(*)(void))fn_801159F0)();
        r4 = r3;
        r3 = (u32)sp + 0xf0;
        ((void(*)(void))fn_80130770)();
        r30 = 0x0;
        while (1) {
            tmp = r30 & 0xFFFF;
            if (tmp >= 6) break;
            r4 = r30;
            r3 = (u32)sp + 0xf0;
            ((void(*)(void))fn_8012AC08)();
            ((void(*)(void))fn_80123FBC)();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                r4 = r30;
                r3 = (u32)sp + 0xf0;
                ((void(*)(void))fn_8012AC08)();
                r31 = r3;
                break;
            }
            r30 = r30 + 0x1;

        }

        if (r31 == 0) {
            r3 = (u32)&lbl_80268AE0;
            r4 = 0x460;
            r3 = (u32)&lbl_80268AE0;
            r5 = (u32)&lbl_8047C10C;
            __assert();
        }
        tmp = tmp | 0x2;
        *(u32*)(sp + 0x20) = tmp;
    }
    tmp = *(u8*)&lbl_8047A630;
    if (tmp != 0) {
        r4 = *(u16*)(sp + 0x26);
        r3 = (u32)sp + 0x28;
        r5 = 0x1;
        r6 = 0x1;
        r7 = -0x1;
        r8 = 0x3e7;
        ((void(*)(void))fn_800298DC)();
        tmp = tmp | 0x1;
        *(u32*)(sp + 0x20) = tmp;
    }
    r3 = 0x1;
    ((void(*)(void))fn_80093574)();
    r5 = r31;
    r4 = (u32)sp + 0x18;
    r3 = 0x1;
    ((void(*)(void))fn_80092C90)();
    r3 = 0x1;
    ((void(*)(void))fn_80093574)();
    r3 = 0x1;
    ((void(*)(void))fn_80093610)();
    if ((s32)r3 != 0xc) {
        r3 = 0x1;
        ((void(*)(void))fn_80093698)();
        r3 = 0x2;
        r4 = 0x3d85;
        r5 = 0x1;
        r6 = 0x0;
        ((void(*)(void))fn_80106D3C)();
        r3 = 0x1;
        ((void(*)(void))fn_801069FC)();
        r3 = 0x2;
        r4 = 0x44cf;
        r5 = 0x1;
        r6 = 0x0;
        ((void(*)(void))fn_80106D3C)();
        r3 = 0x1;
        ((void(*)(void))fn_801069FC)();
        r3 = 0xef;
        ((void(*)(void))fn_80102510)();
        tmp = 0x1;
        *(u32*)&lbl_8047A638 = tmp;
        return;
    }
    r3 = 0x1;
    ((void(*)(void))fn_80093698)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = 0x43c5;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    ((void(*)(void))fn_8001E184)();
    tmp = (s8)r3;
    if ((s32)tmp != 0) {
        if ((s32)tmp < 0) {
            if ((s32)tmp < (s32)-0x1) {
                goto L_800798A0;
            }
            if ((s32)tmp >= 2) goto L_800798A0;
            }
        r3 = 0x43ca;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
        r3 = 0x1;
        ((void(*)(void))fn_801069FC)();
        r3 = 0xef;
        ((void(*)(void))fn_80102510)();
        tmp = 0x0;
        *(u32*)&lbl_8047A638 = tmp;
        return;
    }
L_800798A0:
    r3 = 0x2;
    r4 = 0x44cf;
    r5 = 0x1;
    r6 = 0x0;
    ((void(*)(void))fn_80106D3C)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    r3 = 0xef;
    ((void(*)(void))fn_80102510)();
    tmp = 0x1;
    *(u32*)&lbl_8047A638 = tmp;

    return;
}

/* 0x800798E8 | size: 0x334 */
void fn_800798E8(void) {
    extern u32 fn_80079C1C();
    extern void fn_80079EF4();
    extern void fn_80134420();
    extern void fn_8013467C();
    extern void fn_80135168();
    extern void fn_801D0748();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r28 = r3;
    r3 = 0x2;
    r4 = 0x2;
    r5 = 0x0;
    fn_801D0748();
    r29 = r3;
    if ((s32)r29 == 3) {
        r3 = 0x0;
        r4 = 0x4;
        fn_80135168();
        if (r3 == 0) {
        }
        if ((s32)r29 != (s32)-0x1) {
            r3 = 0x2;
            r4 = 0x44db;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
            r3 = 0x1;
            ((void(*)(void))fn_801069FC)();
        }
        r3 = 0xef;
        ((void(*)(void))fn_80102510)();
        tmp = 0x1;
        *(u32*)&lbl_8047A638 = tmp;
        return;
        }
    ((void(*)(void))fn_80075AC0)();
    *(u8*)&lbl_8047A635 = r3;
    ((void(*)(void))fn_80075B08)();
    *(u8*)&lbl_8047A634 = r3;
    ((void(*)(void))fn_80075B50)();
    *(u8*)&lbl_8047A633 = r3;
    r3 = 0x0;
    r4 = 0xe;
    r5 = 0x0;
    ((void(*)(void))fn_8012A5B0)();
    tmp = r3;
    r3 = 0x1;
    r4 = tmp;
    fn_80079EF4();
    tmp = r3 & 0xFF;
    if (tmp == 0) return;
    r31 = 0x1;
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_80129280)();
    ((void(*)(void))fn_80128DD4)();
    tmp = *(u8*)&lbl_8047A632;
    r30 = r3;
    if (tmp != 0) {
        r4 = 0x47;
        fn_80134420();
        tmp = r3 & 0xFFFF;
        if (tmp < 1) {
            r31 = 0x0;
    }
    }
    tmp = *(u8*)&lbl_8047A630;
    if (tmp != 0) {
        r3 = r30;
        r4 = 0x1;
        fn_80134420();
        tmp = r3 & 0xFFFF;
        if (tmp < 1) {
            r31 = 0x0;
    }
    }
    r27 = 0x1;
    r3 = 0x0;
    r4 = 0x2;
    ((void(*)(void))fn_80129280)();
    tmp = *(u8*)&lbl_8047A631;
    r29 = r3;
    if (tmp != 0) {
        r27 = 0x0;
        while (1) {
            tmp = r27 & 0xFF;
            if (tmp >= 6) break;
            r3 = r29;
            r5 = r27 & 0xFF;
            r4 = 0x3;
            ((void(*)(void))fn_8012A5B0)();
            ((void(*)(void))fn_80123FBC)();
            tmp = r3 & 0xFF;
            if (tmp != 1) {
                tmp = 0x1;
                break;
            }
            r27 = r27 + 0x1;

        }
        tmp = 0x0;

        r27 = tmp;
    }
    r3 = 0x0;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_8012A5B0)();
    tmp = r3;
    r3 = 0x0;
    r26 = tmp;
    r4 = 0x0;
    ((void(*)(void))fn_80129280)();
    tmp = 0x3bfa;
    ctr_fn = (void(*)(void))tmp;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        tmp = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r5 + 0x4) = r3;
        r5 += 8; *(u32*)r5 = tmp;
    } while (--ctr != 0);
    r4 = r31;
    r5 = r27;
    r6 = r26;
    r3 = 0x1;
    fn_80079C1C();
    tmp = r3 & 0xFF;
    if (tmp == 0) return;
    tmp = *(u8*)&lbl_8047A632;
    if (tmp != 0) {
        r3 = r30;
        r4 = 0x47;
        r5 = 0x1;
        fn_8013467C();
        ((void(*)(void))fn_80075A9C)();
    }
    tmp = *(u8*)&lbl_8047A631;
    if (tmp != 0) {
        ((void(*)(void))fn_80115BD8)();
        ((void(*)(void))fn_801159F0)();
        r4 = r3;
        r3 = r29;
        ((void(*)(void))fn_80130770)();
        ((void(*)(void))fn_80075AE4)();
    }
    tmp = *(u8*)&lbl_8047A630;
    if (tmp != 0) {
        r3 = r30;
        r4 = 0x1;
        r5 = 0x1;
        fn_8013467C();
        ((void(*)(void))fn_80075B2C)();
    }
    r3 = 0x4;
    r4 = 0x2;
    r5 = 0x0;
    fn_801D0748();
    if ((s32)r3 != 4) {
        r3 = 0xef;
        ((void(*)(void))fn_80102510)();
        r3 = 0x0;
        r4 = 0x0;
        ((void(*)(void))fn_80129280)();
        tmp = 0x3bfa;
        ctr_fn = (void(*)(void))tmp;
        do {
            r3 = *(u32*)((u8*)r4 + 0x4);
            tmp = *(u32*)((u8*)r4 + 0x8);
            *(u32*)((u8*)r5 + 0x4) = r3;
            r5 += 8; *(u32*)r5 = tmp;
        } while (--ctr != 0);
        tmp = 0x1;
        *(u32*)&lbl_8047A638 = tmp;
        return;
    }
    r3 = 0x43c5;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    ((void(*)(void))fn_8001E184)();
    tmp = (s8)r3;
    if ((s32)tmp != 0) {
        if ((s32)tmp < 0) {
            if ((s32)tmp < (s32)-0x1) {
                goto L_80079BF8;
            }
            if ((s32)tmp >= 2) goto L_80079BF8;
            }
        r3 = 0x43c8;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
        r3 = 0xef;
        ((void(*)(void))fn_80102510)();
        tmp = 0x0;
        *(u32*)&lbl_8047A638 = tmp;
        return;
    }
L_80079BF8:
    r3 = 0xef;
    ((void(*)(void))fn_80102510)();
    tmp = 0x1;
    *(u32*)&lbl_8047A638 = tmp;

    return;
}

/* 0x80079C1C | size: 0x2D8 */
#pragma peephole off
u32 fn_80079C1C(s32 arg0, int arg1, int arg2, s32 arg3) {
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
#pragma peephole on

/* 0x80079EF4 | size: 0x6F4 */
void fn_80079EF4(void) {
    u8 sp[0x80];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r31 = r3;
    r5 = *(u32*)&lbl_804788F0;
    r6 = 0x0;
    r3 = (u32)&lbl_802E61D8;
    *(u8*)&lbl_8047A630 = r6;
    *(u8*)&lbl_8047A631 = r6;
    r5 = r29 << 2;
    tmp = (u32)&lbl_802E61D8;
    *(u8*)&lbl_8047A632 = r6;
    r3 = tmp + r5;
    tmp = r29 + 0x1;
    ctr_fn = (void(*)(void))tmp;
    if ((s32)r29 >= 0) {
        do {
            tmp = *(u32*)((u8*)r3 + 0x0);
            if (tmp <= r4) break;
        } while (--ctr != 0);
    }
    if ((s32)r29 < 0) {
        r29 = 0x0;
    }
    r3 = 0xe1;
    ((void(*)(void))fn_80102510)();
    f27 = *(f32*)&lbl_8047C114;
    f31 = *(f64*)&lbl_8047C118;
    r30 = 0x43300000;
    f29 = *(f64*)&lbl_8047C120;
    f28 = *(f32*)&lbl_8047C128;
    while (f27 < f28) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D37CC)();
        *(u32*)(sp + 0xC) = tmp;
        f30 = f0 - f31;
        ((void(*)(void))fn_800D3088)();
        f0 = f0 - f29;
        f0 = f0 / f30;
        f27 = f27 + f0;

    }
    r3 = 0xef;
    r4 = 0x0;
    ((void(*)(void))fn_8010264C)();
    f27 = *(f32*)&lbl_8047C114;
    f31 = *(f64*)&lbl_8047C118;
    r30 = 0x43300000;
    f29 = *(f64*)&lbl_8047C120;
    f28 = *(f32*)&lbl_8047C108;
    while (f27 < f28) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D37CC)();
        *(u32*)(sp + 0x14) = tmp;
        f30 = f0 - f31;
        ((void(*)(void))fn_800D3088)();
        f0 = f0 - f29;
        f0 = f0 / f30;
        f27 = f27 + f0;

    }
    if ((s32)r29 < 1) {
        r3 = 0x43a7;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
        r3 = 0x1;
        ((void(*)(void))fn_801069FC)();
        if ((s32)r31 == 0) {
            r3 = 0x2;
            r4 = 0x44cf;
            r5 = 0x1;
            r6 = 0x0;
            ((void(*)(void))fn_80106D3C)();
            r3 = 0x1;
            ((void(*)(void))fn_801069FC)();
        }
        r3 = 0xef;
        ((void(*)(void))fn_80102510)();
        f27 = *(f32*)&lbl_8047C114;
        f31 = *(f64*)&lbl_8047C118;
        r30 = 0x43300000;
        f29 = *(f64*)&lbl_8047C120;
        f28 = *(f32*)&lbl_8047C108;
        while (f27 < f28) {

            ((void(*)(void))_threadSwitch)();
            ((void(*)(void))fn_800D37CC)();
            *(u32*)(sp + 0x14) = tmp;
            f30 = f0 - f31;
            ((void(*)(void))fn_800D3088)();
            f0 = f0 - f29;
            f0 = f0 / f30;
            f27 = f27 + f0;

        }
        tmp = 0x1;
        r3 = 0x0;
        *(u32*)&lbl_8047A638 = tmp;
        return;
    }
    if ((s32)r29 != 2) {
        if ((s32)r29 < 2) {
            if ((s32)r29 < 1) {
                goto L_8007A450;
            }
            if ((s32)r29 >= 4) goto L_8007A450;
            goto L_8007A30C;
            }
        tmp = *(u8*)&lbl_8047A635;
        if (tmp != 0) {
            r3 = 0x43ae;
            r4 = 0x1;
            r5 = 0x0;
            ((void(*)(void))fn_801067E8)();
            r3 = 0x1;
            ((void(*)(void))fn_801069FC)();
            if ((s32)r31 == 0) {
                r3 = 0x2;
                r4 = 0x44cf;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
                r3 = 0x1;
                ((void(*)(void))fn_801069FC)();
            }
            r3 = 0xef;
            ((void(*)(void))fn_80102510)();
            f27 = *(f32*)&lbl_8047C114;
            f31 = *(f64*)&lbl_8047C118;
            r30 = 0x43300000;
            f29 = *(f64*)&lbl_8047C120;
            f28 = *(f32*)&lbl_8047C108;
            while (f27 < f28) {

                ((void(*)(void))_threadSwitch)();
                ((void(*)(void))fn_800D37CC)();
                *(u32*)(sp + 0x14) = tmp;
                f30 = f0 - f31;
                ((void(*)(void))fn_800D3088)();
                f0 = f0 - f29;
                f0 = f0 / f30;
                f27 = f27 + f0;

            }
            tmp = 0x1;
            r3 = 0x0;
            *(u32*)&lbl_8047A638 = tmp;
            return;
        }
        r3 = 0x43b4;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
        tmp = 0x1;
        *(u8*)&lbl_8047A632 = tmp;
        goto L_8007A4C8;
    }
    r3 = *(u8*)&lbl_8047A635;
    if (r3 != 0) {
        tmp = *(u8*)&lbl_8047A634;
        if (tmp != 0) {
            r3 = 0x43ab;
            r4 = 0x1;
            r5 = 0x0;
            ((void(*)(void))fn_801067E8)();
            r3 = 0x1;
            ((void(*)(void))fn_801069FC)();
            if ((s32)r31 == 0) {
                r3 = 0x2;
                r4 = 0x44cf;
                r5 = 0x1;
                r6 = 0x0;
                ((void(*)(void))fn_80106D3C)();
                r3 = 0x1;
                ((void(*)(void))fn_801069FC)();
            }
            r3 = 0xef;
            ((void(*)(void))fn_80102510)();
            f27 = *(f32*)&lbl_8047C114;
            f31 = *(f64*)&lbl_8047C118;
            r30 = 0x43300000;
            f29 = *(f64*)&lbl_8047C120;
            f28 = *(f32*)&lbl_8047C108;
            while (f27 < f28) {

                ((void(*)(void))_threadSwitch)();
                ((void(*)(void))fn_800D37CC)();
                *(u32*)(sp + 0x14) = tmp;
                f30 = f0 - f31;
                ((void(*)(void))fn_800D3088)();
                f0 = f0 - f29;
                f0 = f0 / f30;
                f27 = f27 + f0;

            }
            tmp = 0x1;
            r3 = 0x0;
            *(u32*)&lbl_8047A638 = tmp;
            return;
    }
    }
    if (r3 != 0) {
        r3 = 0x43b3;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
        tmp = 0x1;
        *(u8*)&lbl_8047A631 = tmp;
        goto L_8007A4C8;
    }
    r3 = 0x43b6;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    tmp = 0x1;
    *(u8*)&lbl_8047A632 = tmp;
    *(u8*)&lbl_8047A631 = tmp;
    goto L_8007A4C8;
L_8007A30C:
    r3 = *(u8*)&lbl_8047A635;
    if (r3 != 0) {
        tmp = *(u8*)&lbl_8047A634;
        if (tmp != 0) {
            tmp = *(u8*)&lbl_8047A633;
            if (tmp != 0) {
                r3 = 0x43a9;
                r4 = 0x1;
                r5 = 0x0;
                ((void(*)(void))fn_801067E8)();
                r3 = 0x1;
                ((void(*)(void))fn_801069FC)();
                if ((s32)r31 == 0) {
                    r3 = 0x2;
                    r4 = 0x44cf;
                    r5 = 0x1;
                    r6 = 0x0;
                    ((void(*)(void))fn_80106D3C)();
                    r3 = 0x1;
                    ((void(*)(void))fn_801069FC)();
                }
                r3 = 0xef;
                ((void(*)(void))fn_80102510)();
                f27 = *(f32*)&lbl_8047C114;
                f31 = *(f64*)&lbl_8047C118;
                r30 = 0x43300000;
                f29 = *(f64*)&lbl_8047C120;
                f28 = *(f32*)&lbl_8047C108;
                while (f27 < f28) {

                    ((void(*)(void))_threadSwitch)();
                    ((void(*)(void))fn_800D37CC)();
                    *(u32*)(sp + 0x14) = tmp;
                    f30 = f0 - f31;
                    ((void(*)(void))fn_800D3088)();
                    f0 = f0 - f29;
                    f0 = f0 / f30;
                    f27 = f27 + f0;

                }
                tmp = 0x1;
                r3 = 0x0;
                *(u32*)&lbl_8047A638 = tmp;
                return;
    }
    }
    }
    tmp = *(u8*)&lbl_8047A634;
    if (tmp != 0) {
        r3 = 0x43b1;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
        tmp = 0x1;
        *(u8*)&lbl_8047A630 = tmp;

    } else if (r3 != 0) {
        r3 = 0x43b5;
        r4 = 0x1;
        r5 = 0x0;
        ((void(*)(void))fn_801067E8)();
        tmp = 0x1;
        *(u8*)&lbl_8047A630 = tmp;
        *(u8*)&lbl_8047A631 = tmp;

    }
    r3 = 0x43c2;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    tmp = 0x1;
    *(u8*)&lbl_8047A630 = tmp;
    *(u8*)&lbl_8047A631 = tmp;
    *(u8*)&lbl_8047A632 = tmp;
    goto L_8007A4C8;
L_8007A450:
    r3 = 0xef;
    ((void(*)(void))fn_80102510)();
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    f27 = *(f32*)&lbl_8047C114;
    f31 = *(f64*)&lbl_8047C118;
    r30 = 0x43300000;
    f29 = *(f64*)&lbl_8047C120;
    f28 = *(f32*)&lbl_8047C108;
    while (f27 < f28) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D37CC)();
        *(u32*)(sp + 0x14) = tmp;
        f30 = f0 - f31;
        ((void(*)(void))fn_800D3088)();
        f0 = f0 - f29;
        f0 = f0 / f30;
        f27 = f27 + f0;

    }
    tmp = 0x1;
    r3 = 0x0;
    *(u32*)&lbl_8047A638 = tmp;
    return;
L_8007A4C8:
    r3 = 0x43d1;
    r4 = 0x1;
    r5 = 0x0;
    ((void(*)(void))fn_801067E8)();
    ((void(*)(void))fn_8001E184)();
    r30 = r3;
    r3 = 0x1;
    ((void(*)(void))fn_801069FC)();
    tmp = (s8)r30;
    if ((s32)tmp == 0) { r3 = 0x1; return; }
    if ((s32)tmp < 0) {
        if ((s32)tmp < (s32)-0x1) {
            r3 = 0x1;
            return;
        }
        if ((s32)tmp >= 2) { r3 = 0x1; return; }
        }
    if ((s32)r31 == 0) {
        r3 = 0x2;
        r4 = 0x44cf;
        r5 = 0x1;
        r6 = 0x0;
        ((void(*)(void))fn_80106D3C)();
        r3 = 0x1;
        ((void(*)(void))fn_801069FC)();
    }
    r3 = 0xef;
    ((void(*)(void))fn_80102510)();
    f27 = *(f32*)&lbl_8047C114;
    f28 = *(f64*)&lbl_8047C118;
    r31 = 0x43300000;
    f30 = *(f64*)&lbl_8047C120;
    f31 = *(f32*)&lbl_8047C108;
    while (f27 < f31) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D37CC)();
        *(u32*)(sp + 0x14) = tmp;
        f29 = f0 - f28;
        ((void(*)(void))fn_800D3088)();
        f0 = f0 - f30;
        f0 = f0 / f29;
        f27 = f27 + f0;

    }
    tmp = 0x1;
    r3 = 0x0;
    *(u32*)&lbl_8047A638 = tmp;
    return;

    r3 = 0x1;

    return;
}

/* 0x8007A5E8 | size: 0x7C */
void fn_8007A5E8(s32 r3, u8 *r4) {
    extern u32 fn_8012A5B0(s32, s32, s32);
    extern void fn_80132A38(s32, u32);
    extern u32 fn_800FA444(s32);
    extern void fn_800FB680(s32, s32, s32, s32);
    u8 *r31 = r4;
    u32 v;
    if ((s32)*(u32*)&lbl_8047A638 == 4) {
        v = *(u32*)&lbl_8047A62C;
    } else {
        v = fn_8012A5B0(0, 0xd, 0);
    }
    fn_80132A38(0x50, v);
    v = fn_800FA444(0x153);
    fn_800FB680(*(s16*)(r31 + 0x54) - (v >> 16), 0, -1, 0x153);
    return;
}

/* 0x8007A664 | size: 0x8C */
void fn_8007A664(s32 r3, u8 *r4) {
    extern void fn_80109220(u8 *, s32);
    switch ((s32)*(u32*)&lbl_8047A638) {
    case 4:
        if (*(s16*)(r4 + 0x6) == 0x10bf) {
            fn_80109220(r4, 1);
        } else {
            fn_80109220(r4, 0);
        }
        break;
    case 3:
        if (*(s16*)(r4 + 0x6) == 0x10c0) {
            fn_80109220(r4, 1);
        } else {
            fn_80109220(r4, 0);
        }
        break;
    }
}

/* 0x8007A6F0 | size: 0x13C */
void fn_8007A6F0(void) {
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r30 = r4;
    tmp = *(u32*)&lbl_8047A638;
    if ((s32)tmp == 4) {
        r3 = *(u32*)&lbl_8047A628;
    } else {

        r3 = 0x0;
        r4 = 0xe;
        r5 = 0x0;
        ((void(*)(void))fn_8012A5B0)();
    }
    r5 = *(u32*)&lbl_804788F0;
    r4 = (u32)&lbl_802E61D8;
    tmp = (u32)&lbl_802E61D8;
    r4 = r31 << 2;
    r4 = tmp + r4;
    tmp = r31 + 0x1;
    ctr_fn = (void(*)(void))tmp;
    if ((s32)r31 >= 0) {
        do {
            tmp = *(u32*)((u8*)r4 + 0x0);
            if (tmp <= r3) break;
        } while (--ctr != 0);
    }
    if ((s32)r31 < 0) {
        r31 = 0x0;
    }
    r3 = r30;
    r4 = 0x0;
    ((void(*)(void))fn_80109220)();
    if ((s32)r31 != 2) {
        if ((s32)r31 < 2) {
            if ((s32)r31 < 1) {
                goto L_8007A7FC;
            }
            if ((s32)r31 >= 4) goto L_8007A7FC;

            } else {
            tmp = *(s16*)((u8*)r30 + 0x6);
            if ((s32)tmp != 0x10c3) return;
            r3 = r30;
            r4 = 0x1;
            ((void(*)(void))fn_80109220)();
            return;
        }
        tmp = *(s16*)((u8*)r30 + 0x6);
        if ((s32)tmp != 0x10c4) return;
        r3 = r30;
        r4 = 0x1;
        ((void(*)(void))fn_80109220)();
        return;
            }
    tmp = *(s16*)((u8*)r30 + 0x6);
    if ((s32)tmp != 0x10c5) return;
    r3 = r30;
    r4 = 0x1;
    ((void(*)(void))fn_80109220)();
    return;
L_8007A7FC:
    tmp = *(s16*)((u8*)r30 + 0x6);
    if ((s32)tmp != 0x10c2) return;
    r3 = r30;
    r4 = 0x1;
    ((void(*)(void))fn_80109220)();

    return;
}

/* 0x8007A82C | size: 0x24 */
#pragma push
#pragma scheduling off
s32 fn_8007A82C(void) {
    extern s32 fn_801C40F0(s32);
    return fn_801C40F0(1);
}
#pragma pop

/* 0x8007A850 | size: 0x21C */
void fn_8007A850(void) {
    extern void fn_800792D8();
    extern void fn_800798E8();
    extern void fn_801C40F0();
    extern void fn_801D0314();
    extern void fn_801D036C();
    u8 sp[0x70];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;

    fn_801D036C();
    tmp = r3;
    r3 = 0x1;
    r30 = tmp;
    fn_801C40F0();
    while (1) {
        tmp = *(u32*)&lbl_8047A638;
        if ((s32)tmp <= 0) break;
        if ((s32)tmp != 3) {
            if ((s32)tmp < 3) {
                if ((s32)tmp != 1) {
                    if ((s32)tmp < 1) {
                        continue;
                    }
                    if ((s32)tmp >= 5) continue;
                    goto L_8007AA08;
                    }
                r3 = 0x43cf;
                r4 = 0x0;
                r5 = 0x1;
                ((void(*)(void))fn_801067E8)();
                r3 = 0xe1;
                r4 = 0x1;
                ((void(*)(void))fn_8010264C)();
                r31 = r3;
                r3 = 0x1;
                ((void(*)(void))fn_801069FC)();
                if ((s32)r31 != 1) {
                    if ((s32)r31 < 1) {
                        if ((s32)r31 != (s32)-0x1) {
                            if ((s32)r31 < (s32)-0x1) {
                                goto L_8007A96C;
                            }
                            if ((s32)r31 != 3) {
                                if ((s32)r31 >= 3) goto L_8007A96C;
                                goto L_8007A938;
                                }
                            tmp = 0x3;
                            *(u32*)&lbl_8047A638 = tmp;
                            continue;
                        }
                        tmp = 0x4;
                        *(u32*)&lbl_8047A638 = tmp;
                        continue;
                    L_8007A938:
                        tmp = 0x2;
                        *(u32*)&lbl_8047A638 = tmp;
                        continue;
                            }
                    r3 = 0xe1;
                    ((void(*)(void))fn_80102510)();
                    tmp = 0x0;
                    *(u32*)&lbl_8047A638 = tmp;
                    continue;
                        }
                r3 = 0xe1;
                ((void(*)(void))fn_80102510)();
                tmp = 0x0;
                *(u32*)&lbl_8047A638 = tmp;
                continue;
            L_8007A96C:
                r3 = 0xe1;
                ((void(*)(void))fn_80102510)();
                tmp = 0x0;
                *(u32*)&lbl_8047A638 = tmp;
                continue;
                    }
            r3 = 0x43a5;
            r4 = 0x1;
            r5 = 0x0;
            ((void(*)(void))fn_801067E8)();
            r3 = 0x1;
            ((void(*)(void))fn_801069FC)();
            f27 = *(f32*)&lbl_8047C114;
            f28 = *(f64*)&lbl_8047C118;
            r31 = 0x43300000;
            f30 = *(f64*)&lbl_8047C120;
            f31 = *(f32*)&lbl_8047C108;
            while (f27 < f31) {

                ((void(*)(void))_threadSwitch)();
                ((void(*)(void))fn_800D37CC)();
                *(u32*)(sp + 0xC) = tmp;
                f29 = f0 - f28;
                ((void(*)(void))fn_800D3088)();
                f0 = f0 - f30;
                f0 = f0 / f29;
                f27 = f27 + f0;

            }
            tmp = 0x1;
            *(u32*)&lbl_8047A638 = tmp;
            continue;
        }
        r3 = r30;
        fn_800798E8();
        continue;
    L_8007AA08:
        fn_800792D8();
    }
    r3 = r30;
    fn_801D0314();
    r3 = 0x321;
    r4 = 0x0;
    ((void(*)(void))fn_80113828)();
    return;
}

/* 0x8007AA6C | size: 0x3C */
void fn_8007AA6C(void) {
    extern u32 fn_80113F48(void);
    extern void fn_80176E0C(u32, u32, u32, u32);
    *(u32*)&lbl_8047A638 = 1;
    fn_80176E0C(fn_80113F48(), 0x10941800, 0, 0);
    return;
}

/* 0x8007AAA8 | size: 0x54 */
#pragma peephole off
s32 fn_8007AAA8(void) {
    extern void fn_8007B0D8(void);
    extern void fn_800A1E54(void*, void*);
    u32 sp;

    lbl_803F7A30[0x342] = 1;
    while (*(volatile u8*)&lbl_803F7A30[0x345] == 0) {
    }
    fn_800A1E54(&lbl_803F7A30[0x28], &sp);
    fn_8007B0D8();
    return 0;
}
#pragma peephole on

/* 0x8007AAFC | size: 0x14 */
void fn_8007AAFC(void) {
    lbl_803F7A30[0x342] = 1;
}

/* 0x8007AB10 | size: 0x580 */
void fn_8007AB10(void) {
    extern void fn_8007B350();
    extern u8 jumptable_802EE51C[];
    extern u8 jumptable_802EE550[];
    extern u8 jumptable_802EE594[];
    extern u8 jumptable_802EE5C0[];
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;

    r31 = r3;
    r30 = r4;
    if ((s32)r31 == 0) {
        r3 = 0x0;
        return;
    }
    tmp = *(u32*)((u8*)r30 + 0x0);
    if ((s32)tmp == 0) {
        r3 = 0x1;
        ((void(*)(void))fn_800D0F44)();
        /* subis tmp, r3, 0x4 */;
        if (tmp != 0) {
            if (r31 <= 0x10) {
                r3 = (u32)jumptable_802EE5C0;
                tmp = r31 << 2;
                r3 = (u32)jumptable_802EE5C0;
                tmp = *(u32*)(r3 + tmp);
                ctr_fn = (void(*)(void))tmp;
                r3 = 0x2;
                return;
                r3 = 0x4;
                return;
                r3 = 0x7;
                return;
                r3 = 0x4;
                return;
                r3 = 0xb;
                return;
                r3 = 0x4;
                return;
                r3 = 0x13;
                return;
            }
            r3 = 0x0;
            return;
        }
        tmp = 0x1;
        r3 = (u32)&lbl_803F7A30;
        *(u32*)((u8*)r30 + 0x0) = tmp;
        r5 = (u32)&lbl_802EE508;
        r7 = (u32)&lbl_802EE508;
        r3 = (u32)&lbl_803F7A30;
        r8 = *(u8*)&lbl_80478930;
        r4 = 0x1;
        r5 = (u32)&lbl_8047A640;
        r6 = 0x4a;
        r9 = 0x3;
        fn_8007B350();
    }
    r3 = (u32)&lbl_803F7A30;
    r29 = (u32)&lbl_803F7A30;
    tmp = *(u8*)((u8*)r29 + 0x345);
    if (tmp != 0) {
        r3 = r29 + 0x28;
        r4 = (u32)sp + 0x8;
        ((void(*)(void))fn_800A1E54)();
        tmp = 0x0;
        r3 = 0x1;
        *(u8*)((u8*)r29 + 0x345) = tmp;
        ((void(*)(void))fn_800D0F44)();
        /* subis tmp, r3, 0x4 */;
        if (tmp != 0) {
            tmp = 0xa;

        } else {
            if ((s32)tmp == 0) {
                tmp = 0x0;
            }
        }
        if (tmp > 0xa) { r3 = 0x0; return; }
        r3 = (u32)jumptable_802EE594;
        tmp = tmp << 2;
        r3 = (u32)jumptable_802EE594;
        tmp = *(u32*)(r3 + tmp);
        ctr_fn = (void(*)(void))tmp;
        r3 = (u32)&lbl_803F7A30;
        r4 = (u32)&lbl_802EE508;
        r7 = (u32)&lbl_802EE508;
        r8 = *(u8*)&lbl_80478930;
        r3 = (u32)&lbl_803F7A30;
        r4 = 0x1;
        r5 = (u32)&lbl_8047A640;
        r6 = 0x4a;
        r9 = 0x3;
        fn_8007B350();
        if (r31 > 0x10) { r3 = 0x0; return; }
        r3 = (u32)jumptable_802EE550;
        tmp = r31 << 2;
        r3 = (u32)jumptable_802EE550;
        tmp = *(u32*)(r3 + tmp);
        ctr_fn = (void(*)(void))tmp;
        r3 = 0x2;
        return;
        r3 = 0x4;
        return;
        r3 = 0x7;
        return;
        r3 = 0x4;
        return;
        r3 = 0xb;
        return;
        r3 = 0x4;
        return;
        r3 = 0x13;
        return;
        r3 = (u32)&lbl_803F7A30;
        r4 = (u32)&lbl_802EE508;
        r7 = (u32)&lbl_802EE508;
        r8 = *(u8*)&lbl_80478930;
        r3 = (u32)&lbl_803F7A30;
        r4 = 0x1;
        r5 = (u32)&lbl_8047A640;
        r6 = 0x4a;
        r9 = 0x3;
        fn_8007B350();
        if ((s32)r31 != 0xf) { r3 = 0x0; return; }
        r3 = 0x10;
        return;
        if ((s32)r31 == 0x12) {
            r3 = (u32)&lbl_803F7A30;
            r4 = (u32)&lbl_802EE508;
            r7 = (u32)&lbl_802EE508;
            r8 = *(u8*)&lbl_80478930;
            r3 = (u32)&lbl_803F7A30;
            r4 = 0x1;
            r5 = (u32)&lbl_8047A640;
            r6 = 0x4a;
            r9 = 0x2;
            fn_8007B350();
            if ((s32)r31 == 0x12) {
                r3 = 0x0;
                return;
            }
            r3 = 0x12;
            return;
        }
        r3 = (u32)&lbl_803F7A30;
        r4 = (u32)&lbl_802EE508;
        r7 = (u32)&lbl_802EE508;
        r8 = *(u8*)&lbl_80478930;
        r3 = (u32)&lbl_803F7A30;
        r4 = 0x1;
        r5 = (u32)&lbl_8047A640;
        r6 = 0x4a;
        r9 = 0x3;
        fn_8007B350();
        if (r31 > 0xc) { r3 = 0x0; return; }
        r3 = (u32)jumptable_802EE51C;
        tmp = r31 << 2;
        r3 = (u32)jumptable_802EE51C;
        tmp = *(u32*)(r3 + tmp);
        ctr_fn = (void(*)(void))tmp;
        r3 = 0x10;
        return;
        r3 = 0x11;
        return;
        r3 = 0xb;
        return;
        r3 = 0x4;
        return;
        if ((s32)r31 != 3 && (s32)r31 != 0xa && (s32)r31 != 0xc) {

            if ((s32)r31 == 0xe) {
            }
            r3 = (u32)&lbl_803F7A30;
            r4 = (u32)&lbl_802EE508;
            r7 = (u32)&lbl_802EE508;
            r8 = *(u8*)&lbl_80478930;
            r3 = (u32)&lbl_803F7A30;
            r4 = 0x1;
            r5 = (u32)&lbl_8047A640;
            r6 = 0x4a;
            r9 = 0x1;
            fn_8007B350();
            if ((s32)r31 == 0xe) {
                r3 = 0x0;
                return;
            }
            r3 = 0xe;
            return;
            }
        r3 = (u32)&lbl_803F7A30;
        r4 = (u32)&lbl_802EE508;
        r7 = (u32)&lbl_802EE508;
        r8 = *(u8*)&lbl_80478930;
        r3 = (u32)&lbl_803F7A30;
        r4 = 0x1;
        r5 = (u32)&lbl_8047A640;
        r6 = 0x4a;
        r9 = 0x3;
        fn_8007B350();
        if ((s32)r31 == 6) { r3 = 0x7; return; }
        if ((s32)r31 < 6) {
            if ((s32)r31 == 1) { r3 = 0x2; return; }
            r3 = 0x0;
            return;
        }
        if ((s32)r31 == 8) { r3 = 0x4; return; }
        r3 = 0x0;
        return;

        r3 = 0x2;
        return;

        r3 = 0x7;
        return;

        r3 = 0x4;
        return;
        r3 = (u32)&lbl_803F7A30;
        r4 = (u32)&lbl_802EE508;
        r8 = *(u8*)&lbl_80478930;
        r7 = (u32)&lbl_802EE508;
        r3 = (u32)&lbl_803F7A30;
        r4 = 0x1;
        r5 = (u32)&lbl_8047A640;
        r6 = 0x4a;
        tmp = 0x0;
        if ((s32)r31 != 1 && (s32)r31 != 3 && (s32)r31 != 6 && (s32)r31 != 8 && (s32)r31 != 0xa && (s32)r31 != 0xc && (s32)r31 != 0xe && (s32)r31 != 0xf && (s32)r31 != 0x11) {

            if ((s32)r31 == 0x12) {
            }
            tmp = 0x1;
            }
        if ((s32)tmp != 0) {
            r9 = 0x0;
        } else {

            r9 = 0x3;
        }
        fn_8007B350();
        r3 = 0x0;
        return;
        r3 = (u32)&lbl_803F7A30;
        r4 = (u32)&lbl_802EE508;
        r7 = (u32)&lbl_802EE508;
        r8 = *(u8*)&lbl_80478930;
        r3 = (u32)&lbl_803F7A30;
        r4 = 0x1;
        r5 = (u32)&lbl_8047A640;
        r6 = 0x4a;
        r9 = 0x3;
        fn_8007B350();
        r3 = 0x14;
        return;
        r3 = (u32)&lbl_803F7A30;
        r4 = (u32)&lbl_802EE508;
        r7 = (u32)&lbl_802EE508;
        r8 = *(u8*)&lbl_80478930;
        r3 = (u32)&lbl_803F7A30;
        r4 = 0x1;
        r5 = (u32)&lbl_8047A640;
        r6 = 0x4a;
        r9 = 0x3;
        fn_8007B350();
        if ((s32)r31 == 0x15) { r3 = 0x0; return; }
        r3 = 0x15;
        return;
        r3 = (u32)&lbl_803F7A30;
        r4 = (u32)&lbl_802EE508;
        r7 = (u32)&lbl_802EE508;
        r8 = *(u8*)&lbl_80478930;
        r3 = (u32)&lbl_803F7A30;
        r4 = 0x1;
        r5 = (u32)&lbl_8047A640;
        r6 = 0x4a;
        r9 = 0x3;
        fn_8007B350();
        if ((s32)r31 == 0x16) { r3 = 0x0; return; }
        r3 = 0x16;
        return;
        r3 = (u32)&lbl_803F7A30;
        r4 = (u32)&lbl_802EE508;
        r7 = (u32)&lbl_802EE508;
        r8 = *(u8*)&lbl_80478930;
        r3 = (u32)&lbl_803F7A30;
        r4 = 0x1;
        r5 = (u32)&lbl_8047A640;
        r6 = 0x4a;
        r9 = 0x3;
        fn_8007B350();
        if ((s32)r31 == 0x17) { r3 = 0x0; return; }
        r3 = 0x17;
        return;
        r3 = 0x0;
        *(u32*)((u8*)r30 + 0x0) = r3;
        if (tmp <= 1) { r3 = 0x13; return; }
        if ((s32)r31 == 0x11) { r3 = 0x13; return; }
        if ((s32)r31 != 0x12) { r3 = 0x0; return; }

        r3 = 0x13;
        return;
        r3 = 0x0;
        *(u32*)((u8*)r30 + 0x0) = r3;
        if (tmp <= 1) { r3 = 0x13; return; }
        if ((s32)r31 == 0x11) { r3 = 0x13; return; }
        if ((s32)r31 != 0x12) { r3 = 0x0; return; }

        r3 = 0x13;
        return;
    }
    tmp = *(u8*)((u8*)r29 + 0x346);
    if ((s32)tmp != 2) { r3 = 0x0; return; }
    if ((s32)r31 != 0xe) { r3 = 0x0; return; }
    r3 = 0xf;
    return;

    r3 = 0x0;

    return;
}

/* 0x8007B090 | size: 0x48 */
void fn_8007B090(u8 *r3) {
    extern void fn_8007B114(u8 *p);
    extern void *fn_800A7BCC(void);
    u8 *r31 = r3;
    void *tmp;

    tmp = fn_800A7BCC();
    memcpy((void*)&lbl_8047A640, (const void*)tmp, 0x4);
    fn_8007B114(r31);
    return;
}

/* 0x8007B0D8 | size: 0x3C */
void fn_8007B0D8(void) {
    extern void fn_8009AAD4(void *, void *);
    void *r4 = *(void **)&lbl_8047A648;

    if (r4 != 0) {
        fn_8009AAD4(*(void **)&lbl_80478980, r4);
        *(u32*)&lbl_8047A648 = 0;
        *(u32*)&lbl_8047A64C = 0;
    }
}

/* 0x8007B114 | size: 0x23C */
void fn_8007B114(void) {
    u8 sp[0x90];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r3 = (u32)&lbl_803FAEF8;
    r4 = 0x0;
    r6 = (u32)&lbl_803FAEF8;
    do {
        r5 = r4;
        r3 = 0x8;
        tmp = 0x2;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = r5 & 0x1;
            if (tmp != 0) {
                tmp = (u32)r5 >> 1;
                r5 = r5 ^ 0x8320;
            } else {

                r5 = (u32)r5 >> 1;
            }
            tmp = r5 & 0x1;
            if (tmp != 0) {
                tmp = (u32)r5 >> 1;
                r5 = r5 ^ 0x8320;
            } else {

                r5 = (u32)r5 >> 1;
            }
            tmp = r5 & 0x1;
            if (tmp != 0) {
                tmp = (u32)r5 >> 1;
                r5 = r5 ^ 0x8320;
            } else {

                r5 = (u32)r5 >> 1;
            }
            tmp = r5 & 0x1;
            if (tmp != 0) {
                tmp = (u32)r5 >> 1;
                r5 = r5 ^ 0x8320;
            } else {

                r5 = (u32)r5 >> 1;
            }
        } while (--ctr != 0);
        *(u32*)((u8*)r6 + 0x0) = r5;
        r6 = r6 + 0x4;
        r4 = r4 + 0x1;
    } while ((s32)r4 < 0x100);
    tmp = 0x0;
    *(u32*)&lbl_8047A64C = tmp;
    OSGetTick();
    r5 = 0xAAAB0000;
    r4 = (u32)&lbl_802EE608;
    r5 = (u32)&lbl_803FADF8;
    r6 = (u32)((u64)tmp * (u64)r3 >> 32);
    r4 = (u32)&lbl_802EE608;
    tmp = (u32)&lbl_803FADF8;
    r6 = (u32)r6 >> 1;
    r5 = r6 * 0x3;
    r5 = r3 - r5;
    r3 = tmp;
    tmp = r5 << 2;
    r4 = *(u32*)(r4 + tmp);
    ((void(*)(void))strcpy)();
    r3 = (u32)&lbl_803FADF8;
    r4 = (u32)sp + 0x44;
    r3 = (u32)&lbl_803FADF8;
    ((void(*)(void))fn_800A501C)();
    if ((s32)r3 != 0) {
        if (r3 != 0) {
            r4 = *(u32*)&lbl_8047A648;
            tmp = r3 + 0x5b;
            /* clrrwi tmp, tmp, 5 */;
            *(u32*)&lbl_8047A650 = tmp;
            if (r4 != 0) {
                r3 = *(u32*)&lbl_80478980;
                ((void(*)(void))fn_8009AAD4)();
            }
            r3 = *(u32*)&lbl_80478980;
            r4 = *(u32*)&lbl_8047A650;
            ((void(*)(void))fn_8009A9D8)();
            *(u32*)&lbl_8047A648 = r3;
            if (r3 != 0) {
                r5 = *(u32*)&lbl_8047A650;
                r4 = 0x0;
                memset((void*)r3, (int)r4, (u32)r5);
                r3 = (u32)sp + 0x44;
                ((void(*)(void))fn_800A50E4)();
                tmp = *(u32*)&lbl_8047A64C;
                if ((s32)tmp == 0) {
                    r3 = (u32)&lbl_803FADF8;
                    r4 = (u32)sp + 0x8;
                    r3 = (u32)&lbl_803FADF8;
                    ((void(*)(void))fn_800A501C)();
                    if ((s32)r3 != 0) {
                        if (r31 != 0) {
                            tmp = r31 + 0x1f;
                            r4 = *(u32*)&lbl_8047A648;
                            r3 = (u32)sp + 0x8;
                            r6 = 0x0;
                            /* clrrwi r5, tmp, 5 */;
                            r7 = 0x2;
                            ((void(*)(void))fn_800A541C)();
                            if ((s32)r3 >= 0) {
                                if (r3 >= r31) {
                                    r3 = (u32)sp + 0x8;
                                    ((void(*)(void))fn_800A50E4)();
                                    r4 = *(u32*)&lbl_8047A648;
                                    r5 = r31;
                                    r3 = r4 + 0x34;
                                    ((void(*)(void))fn_800C8174)();
                                    r3 = *(u32*)&lbl_8047A648;
                                    r4 = 0x0;
                                    r5 = 0x34;
                                    memset((void*)r3, (int)r4, (u32)r5);
                                    r3 = r31 + 0x34;
                                    r4 = *(u32*)&lbl_8047A648;
                                    r5 = r3;
                                    tmp = *(u32*)&lbl_8047A650;
                                    r3 = r4 + r3;
                                    r4 = 0x0;
                                    r5 = tmp - r5;
                                    memset((void*)r3, (int)r4, (u32)r5);
                                    tmp = 0x1;
                                    *(u32*)&lbl_8047A64C = tmp;
    }
    }
    }
    }
    }
    }
    }
    }
    return;
}

/* 0x8007B350 | size: 0x354 */
void fn_8007B350(void) {
    extern void fn_8007B6A4();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r25 = r3;
    r31 = r4;
    r26 = r5;
    r30 = r6;
    r27 = r7;
    r28 = r8;
    r29 = r9;
    tmp = r25 + 0x2388;
    r4 = 0x0;
    r5 = tmp - r25;
    memset((void*)r3, (int)r4, (u32)r5);
    tmp = r31 & 0xFF;
    r3 = r29 & 0x1;
    *(u8*)((u8*)r25 + 0x340) = tmp;
    tmp = r29 & 0x00000002;
    r5 = *(u8*)((u8*)r26 + 0x0);
    r4 = *(u8*)((u8*)r26 + 0x1);
    r6 = *(u8*)((u8*)r26 + 0x2);
    r5 = r5 << 24;
    r4 = r4 << 16;
    r7 = *(u8*)((u8*)r26 + 0x3);
    r6 = r6 << 8;
    r4 = r5 | r4;
    r4 = r6 | r4;
    r4 = r7 | r4;
    *(u32*)((u8*)r25 + 0x34C) = r4;
    *(u8*)((u8*)r25 + 0x344) = r3;
    *(u8*)((u8*)r25 + 0x343) = tmp;
    tmp = *(u32*)((u8*)r26 + 0x0);
    *(u32*)((u8*)r25 + 0x354) = tmp;
    if (r30 == 0x4a) {
        r3 = 0x50530000;
        tmp = r3 + 0x414a;
        *(u32*)((u8*)r25 + 0x350) = tmp;
    } else {

        r3 = 0x50530000;
        tmp = r3 + 0x4145;
        *(u32*)((u8*)r25 + 0x350) = tmp;
    }
    r31 = *(u32*)&lbl_8047A648;
    r5 = 0x0;
    tmp = *(u32*)&lbl_8047A650;
    r29 = r31 + tmp;
    *(u32*)((u8*)r25 + 0x380) = r31;
    r30 = r29 - r31;
    *(u32*)((u8*)r25 + 0x384) = r29;
    r3 = r4 & 0xFF;
    tmp = (u32)r4 >> 8;
    *(u8*)((u8*)r31 + 0x2C) = r3;
    r3 = tmp & 0xFF;
    tmp = (u32)r4 >> 16;
    *(u8*)((u8*)r31 + 0x2D) = r3;
    r3 = tmp & 0xFF;
    tmp = (u32)r4 >> 24;
    *(u8*)((u8*)r31 + 0x2E) = r3;
    *(u8*)((u8*)r31 + 0x2F) = tmp;
    tmp = 0x4;
    r3 = r27;
    ctr_fn = (void(*)(void))tmp;
    do {
        r4 = *(u8*)((u8*)r3 + 0x0);
        if (r4 == 0) break;
        tmp = r5 + 0x4;
        r5 = r5 + 0x1;
        *(u8*)(r31 + tmp) = r4;
        r4 = *(u8*)((u8*)r3 + 0x1);
        if (r4 == 0) break;
        tmp = r5 + 0x4;
        r5 = r5 + 0x1;
        *(u8*)(r31 + tmp) = r4;
        r4 = *(u8*)((u8*)r3 + 0x2);
        if (r4 == 0) break;
        tmp = r5 + 0x4;
        r5 = r5 + 0x1;
        *(u8*)(r31 + tmp) = r4;
        r4 = *(u8*)((u8*)r3 + 0x3);
        if (r4 == 0) break;
        tmp = r5 + 0x4;
        r5 = r5 + 0x1;
        *(u8*)(r31 + tmp) = r4;
        r4 = *(u8*)((u8*)r3 + 0x4);
        if (r4 == 0) break;
        tmp = r5 + 0x4;
        r5 = r5 + 0x1;
        *(u8*)(r31 + tmp) = r4;
        r4 = *(u8*)((u8*)r3 + 0x5);
        if (r4 == 0) break;
        tmp = r5 + 0x4;
        r5 = r5 + 0x1;
        *(u8*)(r31 + tmp) = r4;
        r4 = *(u8*)((u8*)r3 + 0x6);
        if (r4 == 0) break;
        tmp = r5 + 0x4;
        r5 = r5 + 0x1;
        *(u8*)(r31 + tmp) = r4;
        r4 = *(u8*)((u8*)r3 + 0x7);
        if (r4 == 0) break;
        tmp = r5 + 0x4;
        r5 = r5 + 0x1;
        *(u8*)(r31 + tmp) = r4;
        r3 = r3 + 0x8;
    } while (--ctr != 0);

    r3 = 0x20 - r5;
    r4 = 0x0;
    do {
        if (r5 >= 0x20) break;
        tmp = (u32)r3 >> 3;
        ctr_fn = (void(*)(void))tmp;
        if (tmp != 0) {
            do {
                tmp = r5 + 0x4;
                *(u8*)(r31 + tmp) = r4;
                tmp = r5 + 0x5;
                *(u8*)(r31 + tmp) = r4;
                tmp = r5 + 0x6;
                *(u8*)(r31 + tmp) = r4;
                tmp = r5 + 0x7;
                *(u8*)(r31 + tmp) = r4;
                tmp = r5 + 0x8;
                *(u8*)(r31 + tmp) = r4;
                tmp = r5 + 0x9;
                *(u8*)(r31 + tmp) = r4;
                tmp = r5 + 0xa;
                *(u8*)(r31 + tmp) = r4;
                tmp = r5 + 0xb;
                r5 = r5 + 0x8;
                *(u8*)(r31 + tmp) = r4;
            } while (--ctr != 0);
            r3 = r3 & 0x7;
            if (tmp == 0) break;
        }
        ctr_fn = (void(*)(void))r3;
        do {
            tmp = r5 + 0x4;
            r5 = r5 + 0x1;
            *(u8*)(r31 + tmp) = r4;
        } while (--ctr != 0);
    } while (0);

    tmp = 0x2;
    r3 = r28 & 0xFF;
    *(u8*)((u8*)r31 + 0x28) = tmp;
    r6 = 0x8;
    tmp = 0x0;
    r4 = r26;
    *(u8*)((u8*)r31 + 0x29) = r3;
    r5 = 0x4;
    *(u8*)((u8*)r31 + 0x30) = r6;
    *(u8*)((u8*)r31 + 0x31) = tmp;
    *(u8*)((u8*)r31 + 0x32) = tmp;
    *(u8*)((u8*)r31 + 0x33) = tmp;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r4 = r25 + 0x354;
    r5 = 0x4;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r3 = 0xAA480000;
    r5 = 0x0;
    r3 = (u32)&lbl_803FAEF8;
    r3 = (u32)&lbl_803FAEF8;
    do {
        r4 = r5;
        r5 = r5 + 0x1;
        tmp = r4 + 0x4;
        r4 = (u32)r6 >> 8;
        tmp = *(u8*)(r31 + tmp);
        tmp = r6 ^ tmp;
        tmp = tmp & 0xFF;
        tmp = tmp << 2;
        tmp = *(u32*)(r3 + tmp);
        r6 = r4 ^ tmp;
    } while (r5 != r7);
    r3 = r6 & 0xFF;
    tmp = (u32)r6 >> 8;
    *(u8*)((u8*)r31 + 0x0) = r3;
    r4 = tmp & 0xFF;
    r3 = (u32)r6 >> 16;
    tmp = (u32)r6 >> 24;
    *(u8*)((u8*)r31 + 0x1) = r4;
    r5 = r3 & 0xFF;
    r3 = r25 + 0x35c;
    r4 = r25 + 0x37c;
    *(u8*)((u8*)r31 + 0x2) = r5;
    r5 = 0x1;
    *(u8*)((u8*)r31 + 0x3) = tmp;
    ((void(*)(void))fn_8009F1D0)();
    r3 = (u32)fn_8007B6A4;
    r5 = r25;
    r4 = (u32)fn_8007B6A4;
    r6 = r25 + 0x2388;
    r3 = r25 + 0x28;
    r7 = 0x2000;
    r8 = 0x8;
    r9 = 0x0;
    ((void(*)(void))fn_800A19CC)();
    r3 = r25 + 0x28;
    ((void(*)(void))fn_800A1F94)();
    return;
}

/* 0x8007B6A4 | size: 0x34 */
void fn_8007B6A4(u8 *r3) {
    extern void fn_8007B6D8(u8 *p);
    u8 *r31 = r3;

    fn_8007B6D8(r31);
    *(u8*)(r31 + 0x345) = 0x1;
    return;
}

/* 0x8007B6D8 | size: 0xB64 */
void fn_8007B6D8(void) {
    extern void fn_8025F3F4();
    extern void fn_8025F484();
    extern void fn_8025F584();
    extern void fn_8025F648();
    extern void fn_8007C23C();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
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
    tmp = 0x0;
    r30 = *(u8*)((u8*)r31 + 0x340);
    r4 = (u32)sp + 0x9;
    *(u8*)((u8*)r31 + 0x346) = tmp;
    r3 = r30;
    fn_8025F484();
    if ((s32)r3 != 0) {
        r3 = 0x0;
        return;
    }
    r3 = r30;
    r4 = (u32)sp + 0x9;
    fn_8025F3F4();
    if ((s32)r3 == 0) {
        tmp = *(u8*)(sp + 0x9);
        if (tmp == 0x28) {
            r3 = r30;
            r4 = (u32)sp + 0x10;
            r5 = (u32)sp + 0x9;
            fn_8025F584();
            if ((s32)r3 != 0) {
                r3 = 0x0;
                return;
            }
            tmp = *(u32*)((u8*)r31 + 0x350);
            r3 = *(u8*)(sp + 0x9);
            do {
                if (r4 == tmp) {
                    r4 = 0x1;
                    break;
                }
                tmp = r3 & 0x00000030;
                if (tmp != 0) {
                    r4 = 0x0;
                    break;
                }
                tmp = *(u32*)((u8*)r31 + 0x34C);
                r5 = tmp ^ r4;
                /* subis tmp, r5, 0x2000 */;
                if (tmp == 0) {
                    tmp = 0x100;
                    *(u16*)((u8*)r31 + 0x348) = tmp;

                } else {
                    /* subis tmp, r5, 0x20 */;
                    if (tmp == 0) {
                        tmp = 0x200;
                        *(u16*)((u8*)r31 + 0x348) = tmp;

                    } else {
                        r3 = 0xDFE00000;
                        r4 = 0x0;
                        *(u16*)((u8*)r31 + 0x348) = r4;
                        tmp = r5 & tmp;
                        if (tmp != 0) {
                            break;
                        }
                    }
                }
                r4 = 0x2;
            } while (0);

            if (r4 != 1) {
                r3 = 0x0;
                return;
            }
            tmp = *(u8*)((u8*)r31 + 0x343);
            if (tmp != 0) {
                r3 = 0x3;
                return;
            }
            r3 = r30;
            r4 = (u32)sp + 0x9;
            fn_8025F3F4();
            if ((s32)r3 != 0) { r3 = 0x0; return; }
            tmp = *(u8*)(sp + 0x9);
            if (tmp != 0x20) {

                r3 = 0x0;
                return;
            }
            r3 = r30;
            r4 = r31 + 0x350;
            r5 = (u32)sp + 0x9;
            fn_8025F648();
            if ((s32)r3 != 0) {
                r3 = 0x0;
                return;
            }
            r3 = r30;
            r4 = (u32)sp + 0x9;
            fn_8025F3F4();
            if ((s32)r3 != 0) { r3 = 0x0; return; }
            tmp = *(u8*)(sp + 0x9);
            if (tmp != 0x30) {

                r3 = 0x0;
                return;
            }
            r6 = *(u32*)((u8*)r31 + 0x380);
            r3 = r30;
            tmp = *(u32*)((u8*)r31 + 0x384);
            r4 = (u32)sp + 0xc;
            r5 = (u32)sp + 0x9;
            r29 = tmp - r6;
            /* clrrwi r6, r29, 24 */;
            tmp = r29 & 0x00FF0000;
            r7 = r29 & 0x0000FF00;
            r8 = r29 & 0xFF;
            r6 = (u32)r6 >> 24;
            tmp = (u32)tmp >> 8;
            r7 = r7 << 8;
            tmp = r6 | tmp;
            r6 = r8 << 24;
            tmp = r7 | tmp;
            tmp = r6 | tmp;
            *(u32*)(sp + 0xC) = tmp;
            fn_8025F648();
            if ((s32)r3 != 0) {
                r3 = 0x0;
                return;
            }
            r3 = r30;
            r4 = (u32)sp + 0x10;
            r5 = (u32)sp + 0x9;
            fn_8025F584();
            if ((s32)r3 != 0) { r3 = 0x0; return; }
            if (r3 != tmp) {

                r3 = 0x0;
                return;
            }
            tmp = 0x1;
            r26 = 0x0;
            *(u8*)((u8*)r31 + 0x346) = tmp;
            r3 = (u32)fn_8007C23C;
            r28 = (u32)fn_8007C23C;
            while (1) {
                if (r26 >= r29) break;
                r25 = 0x0;
            while (1) {
                    tmp = *(u8*)((u8*)r31 + 0x342);
                    if (tmp != 0) {
                        tmp = 0x9;
                        r3 = 0x9;
                        *(u8*)((u8*)r31 + 0x346) = tmp;
                        return;
                    }
                    r3 = r30;
                    r4 = (u32)sp + 0x9;
                    fn_8025F3F4();
                    if ((s32)r3 != 0) {
                        tmp = 0x9;
                        r3 = 0x9;
                        *(u8*)((u8*)r31 + 0x346) = tmp;
                        return;
                    }
                    r3 = *(u8*)(sp + 0x9);
                    tmp = r3 & 0x00000030;
                    if ((s32)tmp != 0x30) {
                        tmp = r3 & 0x00000008;
                        if ((s32)tmp != 0) {
                            r3 = r30;
                            r4 = (u32)sp + 0x10;
                            r5 = (u32)sp + 0x9;
                            fn_8025F584();
                        }
                        tmp = 0x9;
                        r3 = 0x9;
                        *(u8*)((u8*)r31 + 0x346) = tmp;
                        return;
                    }
                    tmp = r3 & 0x00000002;
                    if ((s32)tmp != 0) {
                        r25 = r25 + 0x1;
                        if (r25 > 9) {
                            tmp = 0x9;
                            r3 = 0x9;
                            *(u8*)((u8*)r31 + 0x346) = tmp;
                            return;
                        }
                        r3 = r31;
                        OSCreateAlarm();
                        OSDisableInterrupts();
                        r27 = r3;
                        r3 = r31;
                        r7 = r28;
                        r6 = 0x10;
                        r5 = 0x0;
                        OSSetAlarm();
                        r3 = r31 + 0x28;
                        ((void(*)(void))fn_800A221C)();
                        r3 = r27;
                        OSRestoreInterrupts();
            }
                }
                r6 = *(u32*)((u8*)r31 + 0x380);
                r3 = r30;
                r4 = (u32)sp + 0xc;
                r5 = (u32)sp + 0x9;
                tmp = *(u32*)(r6 + r26);
                *(u32*)(sp + 0xC) = tmp;
                fn_8025F648();
                if ((s32)r3 != 0) {
                    tmp = 0x9;
                    r3 = 0x9;
                    *(u8*)((u8*)r31 + 0x346) = tmp;
                    return;
                }
                r26 = r26 + 0x4;

            }
            tmp = 0x2;
            *(u8*)((u8*)r31 + 0x346) = tmp;
            r3 = (u32)fn_8007C23C;
            r29 = (u32)fn_8007C23C;
            do {
                tmp = *(u8*)((u8*)r31 + 0x342);
                if (tmp != 0) {
                    tmp = 0x9;
                    r3 = 0x9;
                    *(u8*)((u8*)r31 + 0x346) = tmp;
                    return;
                }
                r3 = r30;
                r4 = (u32)sp + 0x9;
                fn_8025F3F4();
                if ((s32)r3 != 0) break;
                tmp = *(u8*)(sp + 0x9);
                tmp = tmp & 0x00000008;
                if ((s32)tmp != 0) break;
                r3 = r31;
                OSCreateAlarm();
                OSDisableInterrupts();
                r27 = r3;
                r3 = r31;
                r7 = r29;
                r6 = 0x10;
                r5 = 0x0;
                OSSetAlarm();
                r3 = r31 + 0x28;
                ((void(*)(void))fn_800A221C)();
                r3 = r27;
                OSRestoreInterrupts();
            } while (1);

            r3 = r30;
            r4 = (u32)sp + 0x9;
            fn_8025F484();
            if ((s32)r3 != 0) {
                tmp = 0x9;
                r3 = 0x9;
                *(u8*)((u8*)r31 + 0x346) = tmp;
                return;
            }
            r3 = r30;
            r4 = (u32)sp + 0x9;
            fn_8025F3F4();
            if ((s32)r3 == 0) {
                tmp = *(u8*)(sp + 0x9);
                if (tmp != 8) {
                }
                tmp = 0x9;
                r3 = 0x9;
                *(u8*)((u8*)r31 + 0x346) = tmp;
                return;
                }
            r3 = r30;
            r4 = (u32)sp + 0x10;
            r5 = (u32)sp + 0x9;
            fn_8025F584();
            if ((s32)r3 != 0) {
                tmp = 0x9;
                r3 = 0x9;
                *(u8*)((u8*)r31 + 0x346) = tmp;
                return;
            }
            tmp = *(u32*)((u8*)r31 + 0x350);
            r3 = *(u8*)(sp + 0x9);
            do {
                if (r4 == tmp) {
                    r4 = 0x1;
                    break;
                }
                tmp = r3 & 0x00000030;
                if (tmp != 0) {
                    r4 = 0x0;
                    break;
                }
                tmp = *(u32*)((u8*)r31 + 0x34C);
                r5 = tmp ^ r4;
                /* subis tmp, r5, 0x2000 */;
                if (tmp == 0) {
                    tmp = 0x100;
                    *(u16*)((u8*)r31 + 0x348) = tmp;

                } else {
                    /* subis tmp, r5, 0x20 */;
                    if (tmp == 0) {
                        tmp = 0x200;
                        *(u16*)((u8*)r31 + 0x348) = tmp;

                    } else {
                        r3 = 0xDFE00000;
                        r4 = 0x0;
                        *(u16*)((u8*)r31 + 0x348) = r4;
                        tmp = r5 & tmp;
                        if (tmp != 0) {
                            break;
                        }
                    }
                }
                r4 = 0x2;
            } while (0);

            if (r4 != 2) {
                tmp = 0x9;
                r3 = 0x9;
                *(u8*)((u8*)r31 + 0x346) = tmp;
                return;
            }
            r3 = r30;
            r4 = r31 + 0x34c;
            r5 = (u32)sp + 0x9;
            fn_8025F648();
            if ((s32)r3 != 0) {
                tmp = 0x9;
                r3 = 0x9;
                *(u8*)((u8*)r31 + 0x346) = tmp;
                return;
            }
            tmp = 0x3;
            r3 = 0x1;
            *(u8*)((u8*)r31 + 0x346) = tmp;
            return;
        }
        }
    if ((s32)r3 != 0) { r3 = 0x0; return; }
    tmp = *(u8*)(sp + 0x9);
    if (tmp != 8) { r3 = 0x0; return; }
    r3 = r30;
    r4 = (u32)sp + 0x10;
    r5 = (u32)sp + 0x9;
    fn_8025F584();
    if ((s32)r3 != 0) {
        r3 = 0x0;
        return;
    }
    tmp = *(u32*)((u8*)r31 + 0x350);
    r3 = *(u8*)(sp + 0x9);
    do {
        if (r4 == tmp) {
            r4 = 0x1;
            break;
        }
        tmp = r3 & 0x00000030;
        if (tmp != 0) {
            r4 = 0x0;
            break;
        }
        tmp = *(u32*)((u8*)r31 + 0x34C);
        r5 = tmp ^ r4;
        /* subis tmp, r5, 0x2000 */;
        if (tmp == 0) {
            tmp = 0x100;
            *(u16*)((u8*)r31 + 0x348) = tmp;

        } else {
            /* subis tmp, r5, 0x20 */;
            if (tmp == 0) {
                tmp = 0x200;
                *(u16*)((u8*)r31 + 0x348) = tmp;

            } else {
                r3 = 0xDFE00000;
                r4 = 0x0;
                *(u16*)((u8*)r31 + 0x348) = r4;
                tmp = r5 & tmp;
                if (tmp != 0) {
                    break;
                }
            }
        }
        r4 = 0x2;
    } while (0);

    if (r4 != 2) {
        r3 = 0x0;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x344);
    if (tmp != 0) {
        tmp = *(u32*)((u8*)r31 + 0x34C);
        *(u32*)(sp + 0xC) = tmp;
    } else {

        tmp = 0x4;
        *(u8*)((u8*)r31 + 0x346) = tmp;
        tmp = *(u32*)((u8*)r31 + 0x34C);
        tmp = tmp | (0x2020 << 16);
        tmp = tmp | 0x2020;
        *(u32*)(sp + 0xC) = tmp;
    }
    r3 = r30;
    r4 = (u32)sp + 0x9;
    fn_8025F3F4();
    if ((s32)r3 == 0) {
        tmp = *(u8*)(sp + 0x9);
        if (tmp != 0) {
        }
        tmp = 0x9;
        r3 = 0x9;
        *(u8*)((u8*)r31 + 0x346) = tmp;
        return;
        }
    r3 = r30;
    r4 = (u32)sp + 0xc;
    r5 = (u32)sp + 0x9;
    fn_8025F648();
    if ((s32)r3 != 0) {
        tmp = 0x9;
        r3 = 0x9;
        *(u8*)((u8*)r31 + 0x346) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x344);
    if (tmp != 0) {
        tmp = 0x0;
        *(u8*)((u8*)r31 + 0x346) = tmp;
        tmp = *(u16*)((u8*)r31 + 0x348);
        tmp = tmp & 0x1;
        if ((s32)tmp != 0) {
            r3 = 0x4;
            return;
        }
        r3 = 0x2;
        return;
    }
    r3 = (u32)fn_8007C23C;
    r29 = (u32)fn_8007C23C;
    do {
        tmp = *(u8*)((u8*)r31 + 0x342);
        if (tmp != 0) {
            tmp = 0x6;
            r3 = 0x6;
            *(u8*)((u8*)r31 + 0x346) = tmp;
            return;
        }
        r3 = r30;
        r4 = (u32)sp + 0x9;
        fn_8025F3F4();
        if ((s32)r3 != 0) break;
        r3 = *(u8*)(sp + 0x9);
        tmp = r3 & 0x00000030;
        if ((s32)tmp == 0x30 || (s32)tmp == 0) break;

        tmp = r3 & 0x32;
        if ((s32)tmp == 0x20) {
            OSGetTick();
            *(u32*)((u8*)r31 + 0x358) = r3;
            r3 = r30;
            r4 = (u32)sp + 0xc;
            r5 = (u32)sp + 0x9;
            tmp = *(u32*)((u8*)r31 + 0x358);
            *(u32*)(sp + 0xC) = tmp;
            fn_8025F648();
            if ((s32)r3 == 0) {
                tmp = *(u8*)(sp + 0x9);
                tmp = tmp & 0x00000030;
                if ((s32)tmp != 0x20) {
                }
                tmp = 0x9;
                r3 = 0x9;
                *(u8*)((u8*)r31 + 0x346) = tmp;
                return;
                }
            r28 = 0x0;
            r3 = (u32)fn_8007C23C;
            r29 = (u32)fn_8007C23C;
            do {
                r26 = 0x0;
            while (1) {
                    r3 = r30;
                    r4 = (u32)sp + 0x9;
                    fn_8025F3F4();
                    if ((s32)r3 != 0) {
                        tmp = 0x9;
                        r3 = 0x9;
                        *(u8*)((u8*)r31 + 0x346) = tmp;
                        return;
                    }
                    r3 = *(u8*)(sp + 0x9);
                    tmp = r3 & 0x00000030;
                    if ((s32)tmp != 0x20) {
                        tmp = 0x9;
                        r3 = 0x9;
                        *(u8*)((u8*)r31 + 0x346) = tmp;
                        return;
                    }
                    tmp = r3 & 0xa;
                    if ((s32)tmp != 8) {
                        r26 = r26 + 0x1;
                        if (r26 > 9) {
                            tmp = 0x9;
                            r3 = 0x9;
                            *(u8*)((u8*)r31 + 0x346) = tmp;
                            return;
                        }
                        r3 = r31;
                        OSCreateAlarm();
                        OSDisableInterrupts();
                        r27 = r3;
                        r3 = r31;
                        r7 = r29;
                        r6 = 0x10;
                        r5 = 0x0;
                        OSSetAlarm();
                        r3 = r31 + 0x28;
                        ((void(*)(void))fn_800A221C)();
                        r3 = r27;
                        OSRestoreInterrupts();
            }
                }
                r3 = r30;
                r4 = (u32)sp + 0x10;
                r5 = (u32)sp + 0x9;
                fn_8025F584();
                if ((s32)r3 != 0) {
                    tmp = 0x9;
                    r3 = 0x9;
                    *(u8*)((u8*)r31 + 0x346) = tmp;
                    return;
                }
                tmp = r28 + 0x2388;
                r3 = *(u32*)((u8*)r31 + 0x358);
                r28 = r28 + 0x4;
                r3 = r4 ^ r3;
                *(u32*)(r31 + tmp) = r3;
            } while (r28 < 0x1040);
            r6 = *(u32*)((u8*)r31 + 0x33C4);
            r3 = 0x0;
            /* clrrwi r4, r6, 24 */;
            tmp = r6 & 0x00FF0000;
            r5 = r6 & 0x0000FF00;
            r6 = r6 & 0xFF;
            r4 = (u32)r4 >> 24;
            tmp = (u32)tmp >> 8;
            r5 = r5 << 8;
            r6 = r6 << 24;
            tmp = r4 | tmp;
            tmp = r5 | tmp;
            r9 = r6 | tmp;
            r4 = (u32)&lbl_803FAEF8;
            tmp = 0x80;
            r5 = (u32)sp + 0x8;
            r4 = (u32)&lbl_803FAEF8;
            ctr_fn = (void(*)(void))tmp;
            do {
                tmp = r3 & 0xFF;
                r8 = *(u32*)((u8*)r31 + 0x354);
                *(u8*)(sp + 0x8) = tmp;
                r7 = 0x0;
                do {
                    tmp = r7;
                    r7 = r7 + 0x1;
                    tmp = *(u8*)(r5 + tmp);
                    r6 = (u32)r8 >> 8;
                    tmp = r8 ^ tmp;
                    tmp = tmp & 0xFF;
                    tmp = tmp << 2;
                    tmp = *(u32*)(r4 + tmp);
                    r8 = r6 ^ tmp;
                } while (r7 != 1);
                if (r9 == r8) break;
                r3 = r3 + 0x1;
                tmp = r3 & 0xFF;
                r8 = *(u32*)((u8*)r31 + 0x354);
                *(u8*)(sp + 0x8) = tmp;
                r7 = 0x0;
                do {
                    tmp = r7;
                    r7 = r7 + 0x1;
                    tmp = *(u8*)(r5 + tmp);
                    r6 = (u32)r8 >> 8;
                    tmp = r8 ^ tmp;
                    tmp = tmp & 0xFF;
                    tmp = tmp << 2;
                    tmp = *(u32*)(r4 + tmp);
                    r8 = r6 ^ tmp;
                } while (r7 != 1);
                if (r9 == r8) break;
                r3 = r3 + 0x1;
            } while (--ctr != 0);

            if (r3 == 0x100) {
                tmp = 0x7;
                r3 = 0x7;
                *(u8*)((u8*)r31 + 0x346) = tmp;
                return;
            }
            r3 = 0xAA480000;
            r6 = 0x0;
            r3 = (u32)&lbl_803FAEF8;
            r4 = (u32)sp + 0x8;
            r3 = (u32)&lbl_803FAEF8;
            do {
                tmp = r6;
                r6 = r6 + 0x1;
                tmp = *(u8*)(r4 + tmp);
                r5 = (u32)r7 >> 8;
                tmp = r7 ^ tmp;
                tmp = tmp & 0xFF;
                tmp = tmp << 2;
                tmp = *(u32*)(r3 + tmp);
                r7 = r5 ^ tmp;
            } while (r6 != 1);
            /* clrrwi r3, r7, 24 */;
            tmp = r7 & 0x00FF0000;
            r4 = r7 & 0x0000FF00;
            r5 = r7 & 0xFF;
            r3 = (u32)r3 >> 24;
            tmp = (u32)tmp >> 8;
            r4 = r4 << 8;
            r5 = r5 << 24;
            tmp = r3 | tmp;
            r6 = 0x0;
            tmp = r4 | tmp;
            r4 = r5 | tmp;
            tmp = 0x40f;
            ctr_fn = (void(*)(void))tmp;
            do {
                r3 = r6 + 0x2388;
                r6 = r6 + 0x4;
                tmp = *(u32*)(r31 + r3);
                tmp = tmp ^ r4;
                *(u32*)(r31 + r3) = tmp;
            } while (--ctr != 0);
            r5 = *(u32*)((u8*)r31 + 0x33C0);
            r6 = 0x0;
            r7 = *(u32*)((u8*)r31 + 0x354);
            /* clrrwi r3, r5, 24 */;
            tmp = r5 & 0x00FF0000;
            r4 = r5 & 0x0000FF00;
            r5 = r5 & 0xFF;
            r3 = (u32)r3 >> 24;
            tmp = (u32)tmp >> 8;
            r4 = r4 << 8;
            r5 = r5 << 24;
            tmp = r3 | tmp;
            tmp = r4 | tmp;
            r5 = r5 | tmp;
            r3 = (u32)&lbl_803FAEF8;
            r3 = (u32)&lbl_803FAEF8;
            do {
                r4 = r6;
                r6 = r6 + 0x1;
                tmp = r4 + 0x2388;
                r4 = (u32)r7 >> 8;
                tmp = *(u8*)(r31 + tmp);
                tmp = r7 ^ tmp;
                tmp = tmp & 0xFF;
                tmp = tmp << 2;
                tmp = *(u32*)(r3 + tmp);
                r7 = r4 ^ tmp;
            } while (r6 != 0x1038);
            if (r7 != r5) {
                tmp = 0x7;
                r3 = 0x7;
                *(u8*)((u8*)r31 + 0x346) = tmp;
                return;
            }
            tmp = 0x5;
            r3 = 0x5;
            *(u8*)((u8*)r31 + 0x346) = tmp;
            return;
        }
        r3 = r31;
        OSCreateAlarm();
        OSDisableInterrupts();
        r27 = r3;
        r3 = r31;
        r7 = r29;
        r6 = 0x10;
        r5 = 0x0;
        OSSetAlarm();
        r3 = r31 + 0x28;
        ((void(*)(void))fn_800A221C)();
        r3 = r27;
        OSRestoreInterrupts();
    } while (1);

    r3 = r30;
    r4 = (u32)sp + 0x9;
    fn_8025F3F4();
    if ((s32)r3 == 0) {
        tmp = 0x9;
        r3 = 0x9;
        *(u8*)((u8*)r31 + 0x346) = tmp;
        return;
    }
    r3 = *(u8*)(sp + 0x9);
    tmp = r3 & 0x00000030;
    if ((s32)tmp == 0) {
        tmp = 0x6;
        r3 = 0x6;
        *(u8*)((u8*)r31 + 0x346) = tmp;
        return;
    }
    if ((s32)tmp == 0x30) {
        tmp = r3 & 0x00000008;
        if ((s32)tmp != 0) {
            tmp = 0x7;
            r3 = 0x7;
            *(u8*)((u8*)r31 + 0x346) = tmp;
            return;
        }
        tmp = 0x8;
        r3 = 0x8;
        *(u8*)((u8*)r31 + 0x346) = tmp;
        return;
    }
    tmp = 0x9;
    r3 = 0x9;
    *(u8*)((u8*)r31 + 0x346) = tmp;
    return;

    r3 = 0x0;

    return;
}

/* 0x8007C23C | size: 0x24 */
#pragma push
#pragma scheduling off
void fn_8007C23C(u8 *r3) {
    extern void fn_800A1F94(u8 *p);
    fn_800A1F94(r3 + 0x28);
}
#pragma pop

/* 0x8007C260 | size: 0xC */
void fn_8007C260(void) {
    *(u32*)&lbl_80478940 = -1;
}

/* 0x8007C26C | size: 0x54 */
void fn_8007C26C(void) {
    extern void fn_8002DC6C(s32);
    extern void fn_800FF58C(s32);
    u32 tmp = 0;

    tmp = *(u32*)&lbl_80478940;
    switch ((s32)tmp) {
    case 0:
        fn_8002DC6C(0xb);
        break;
    case 1:
        fn_8002DC6C(0xc);
        break;
    }
    fn_800FF58C(0x395);
    return;
}
