/**
 * @file gba_misc.c
 * @brief GBA miscellaneous communication support (0x800895A4-0x80092C90)
 *
 * Address range: 0x800895A4 - 0x80092C90
 * Total functions: 69
 */

#include "dolphin/types.h"

typedef struct GbaMiscContext {
    u8 unk_0000[0x4000];
    u8 state_4000;       /* 0x4000 */
    u8 unk_4001[0x135];
    u8 tableKey_4136;    /* 0x4136 */
} GbaMiscContext;

/*
 * Source-level names only: the macros preserve the original fn_* linker
 * symbols for objdiff while documenting the recovered behavior.
 */
#define GbaMisc_GetMappedContextByte fn_80089B8C
#define GbaMisc_HasActiveContextState fn_80089C10
#define GbaMisc_PollEntryStatusA fn_80089CA8
#define GbaMisc_ResetEntryStatusA fn_80089D30
#define GbaMisc_GetEntryStatus fn_8008A9E4
#define GbaMisc_SendPackedEntryStatus fn_8008AB20
#define GbaMisc_SetEntryState fn_8008ABE4
#define GbaMisc_RunFlagDispatch fn_8008C700

#define GBA_MISC_ENTRY_WORD_OFFSET(idx) ((idx) << 2)
#define GBA_MISC_ENTRY_HALF_OFFSET(idx) ((idx) << 1)
#define GbaMisc_EntryStateAtWordOffset(offset) \
    (*(s32*)((u8*)&lbl_803FB318 + (offset) + (-4)))
#define GbaMisc_EntryCachedStatusAtWordOffset(offset) \
    (*(s32*)((u8*)&lbl_803FB308 + (offset) + (-4)))
#define GbaMisc_EntryCounterAAtHalfOffset(offset) \
    (*(u16*)((u8*)&lbl_8047A684 + (offset) + (-2)))
#define GbaMisc_EntryCounterBAtHalfOffset(offset) \
    (*(u16*)((u8*)&lbl_8047A67C + (offset) + (-2)))
#define GbaMisc_EntryState(idx) \
    GbaMisc_EntryStateAtWordOffset(GBA_MISC_ENTRY_WORD_OFFSET(idx))
#define GbaMisc_EntryCachedStatus(idx) \
    GbaMisc_EntryCachedStatusAtWordOffset(GBA_MISC_ENTRY_WORD_OFFSET(idx))
#define GbaMisc_EntryCounterA(idx) \
    GbaMisc_EntryCounterAAtHalfOffset(GBA_MISC_ENTRY_HALF_OFFSET(idx))
#define GbaMisc_EntryCounterB(idx) \
    GbaMisc_EntryCounterBAtHalfOffset(GBA_MISC_ENTRY_HALF_OFFSET(idx))

/* ===== External function declarations ===== */
extern void fn_8001E184();
extern void fn_80071700();
extern void fn_800719A8();
extern s32 fn_80071AE4();
extern void fn_800722A0();
extern void fn_80072548();
extern s32 fn_800726A8();
extern void fn_80072A00();
extern void fn_80072C74();
extern void fn_80072D58();
extern s32 _AGB_EntryGetStatus__FlPUl(s32, u32*);
extern void fn_800730F8();
extern void fn_800733D0();
extern void fn_80073990();
extern void fn_80073A44();
extern void fn_800830A4();
extern void fn_80083BF8();
extern GbaMiscContext* fn_80083CFC();
extern void fn_80083D30();
extern void fn_80083ECC();
extern void fn_800C46B0();
extern void fn_800C8174();
extern void fn_800D3088();
extern void fn_800D37CC();
extern void fn_800E3C00();
extern void fn_800E3C08();
extern void fn_800E8FA0();
extern void fn_800E8FE8();
extern void fn_800E900C();
extern void fn_800E9108();
extern void fn_800EC4D0();
extern void fn_800EC990();
extern void fn_800ECA78();
extern void fn_800ECB74();
extern void fn_800ECCA8();
extern void _threadSwitch();
extern void fn_800F9318();
extern void fn_800F9AEC();
extern void fn_800F9C04();
extern void fn_800FF58C();
extern void fn_8011288C();
extern void fn_80113F48();
extern void fn_80118874();
extern void fn_8011D480();
extern void fn_8011D494();
extern void fn_8011D4A4();
extern void fn_8011D4B4();
extern void fn_8011D4C4();
extern void fn_8011D4D4();
extern void fn_8011D4E4();
extern void fn_8011D4F4();
extern void fn_8011D504();
extern void fn_8011D56C();
extern void fn_8011D57C();
extern void fn_8011D58C();
extern void fn_8011D5B0();
extern void fn_8011D5D4();
extern void fn_8011D5F8();
extern void fn_8011D61C();
extern void fn_8011D640();
/* ... and 210 more external functions */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_80478960;
extern u8 lbl_8047A670;
extern u8 lbl_8047A674;
extern u8 lbl_8047A678;
extern u8 lbl_8047A67C;
extern u8 lbl_8047A684;
extern u8 lbl_8047A690;
extern u8 lbl_8047A694;
extern u8 lbl_8047C1D0;
extern u8 lbl_8047C1D4;
extern u8 lbl_8047C1D8;
extern u8 lbl_8047C1DC;
extern u8 lbl_8047C1E0;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EEBB8[];
extern u8 jumptable_802EEBE0[];
extern u8 jumptable_802EEC10[];
extern u8 jumptable_802EEC30[];
extern u8 lbl_802EEB98[];
extern u8 lbl_802EEC70[];
extern u8 lbl_803FB308[];
extern u8 lbl_803FB318[];

/* ===== Forward declarations ===== */
void fn_800895A4(void);
u32 fn_800896B8(void);
u32 fn_800896C0(void);
u32 fn_800896C8(void);
void fn_800896D0(u32 v);
void fn_800896D8(u32 v);
void fn_800896E0(u32 v);
void fn_800896E8(void);
void fn_80089978(void);
u8 GbaMisc_GetMappedContextByte(void);
s32 GbaMisc_HasActiveContextState(void);
s32 fn_80089C54(void);
void fn_80089C84(s32 param);
s32 GbaMisc_PollEntryStatusA(s32 r31);
s32 GbaMisc_ResetEntryStatusA(s32 param);
void fn_80089D74(s32 param);
s32 fn_80089D98(s32 r31);
s32 fn_80089E20(s32 idx, void* obj, u32 packedStatus, u32 highHalf);
u32 fn_80089F58(u32 v);
u32 fn_80089F60(u32 v);
u32 fn_80089F68(u32 v);
u32 fn_80089F70(u32 v);
void fn_80089F78(void);
s32 fn_8008A99C(void);
int fn_8008A9AC(u32 r3, u8* r4);
s32 GbaMisc_GetEntryStatus(s32 idx, u32* out);
void GbaMisc_SendPackedEntryStatus(s32 param0, u32 param1, u32 param2);
void fn_8008AB4C(s32 param0, s32 param1);
s32 fn_8008AB8C(s32 r3);
u8 fn_8008ABA0(s32 idx);
s32 GbaMisc_SetEntryState(s32 idx, s32 value);
void fn_8008AC34(void);
void fn_8008AE18(void);
void fn_8008BBDC(void);
void fn_8008C5D4(void);
void fn_8008C6FC(void);
void GbaMisc_RunFlagDispatch(void);
s32 fn_8008C78C(void);
void fn_8008C7B0(void);
void fn_8008CACC(void);
void fn_8008CDD8(void);
void fn_8008D0A0(void);
void fn_8008D348(void);
void fn_8008D938(void);
void fn_8008E320(void);
void fn_8008E7D4(void);
void fn_8008EC28(void);
void fn_8008EED0(void);
void fn_8008F190(void);
void fn_8008F524(void);
void fn_8008F91C(void);
void fn_8008FBF4(void);
void fn_8008FE94(void);
void fn_80090100(void);
void fn_80090720(void);
void fn_800909E4(void);
void fn_80090D34(void);
void fn_8009100C(void);
void fn_80091564(void);
void fn_80091774(void);
void fn_80091984(void);
void fn_80091B94(void);
void fn_80091DA4(void);
void fn_80091F48(void);
void fn_80092140(void);
void fn_80092498(void);
void fn_80092664(void);
void fn_800929BC(void);
void fn_80092B2C(void);

/* ===== Function implementations ===== */

/* 0x800895A4 | size: 0x114 */
void fn_800895A4(void) {
    extern void fn_8008BBDC();
    extern void fn_8012A774();
    extern void fn_8012AA44();
    extern void fn_8012AA54();
    extern void fn_8012AA64();
    extern void fn_8012AC08();
    extern void fn_80135938();
    extern void fn_80265EC4();
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

    r30 = r3;
    r31 = r4;
    r0 = *(u8*)((u8*)r31 + 0x0);
    r0 = r0 & 0x00000004;
    if ((s32)r0 != 0x0) {
        r0 = 0x2;
    } else {

        r0 = 0x1;
    }
    r4 = r0 & 0xFF;
    fn_8012A774();
    r3 = 0x0;
    r4 = 0x5;
    fn_80135938();
    r6 = r3;
    r3 = (u32)sp + 0x8;
    r4 = r31 + 0x4;
    r5 = 0x7;
    ((void(*)(void))fn_800F9C04)();
    r3 = r30;
    r4 = (u32)sp + 0x8;
    fn_8012AA64();
    r4 = *(u8*)((u8*)r31 + 0xC);
    r3 = r30;
    fn_8012AA44();
    r6 = *(u32*)((u8*)r31 + 0x10);
    r3 = r30;
    r0 = r6 & 0x0000FF00;
    r5 = r6 & 0x00FF0000;
    r4 = r6 << 24;
    r6 = (u32)r6 >> 24;
    r0 = r0 << 8;
    r5 = (u32)r5 >> 8;
    r0 = r4 | r0;
    r0 = r5 | r0;
    r4 = r6 | r0;
    fn_8012AA54();
    r29 = r31;
    r28 = 0x0;
    do {
        r3 = r30;
        r4 = r28 & 0xFFFF;
        fn_8012AC08();
        r4 = r29 + 0x14;
        r27 = r3;
        fn_8008BBDC();
        r3 = r27;
        r4 = r28 & 0xFFFF;
        ((void(*)(void))fn_8011D494)();
        r29 = r29 + 0x64;
        r28 = r28 + 0x1;
    } while ((s32)r28 < 0x6);
    r29 = 0x0;
    do {
        r0 = r29 + 0x26c;
        r3 = r29;
        r4 = *(u8*)(r31 + r0);
        fn_80265EC4();
        r29 = r29 + 0x1;
    } while ((s32)r29 < 0xb);
    return;
}

/* 0x800896B8 | size: 0x8 */
u32 fn_800896B8(void) {
    return *(u32*)&lbl_80478960;
}

/* 0x800896C0 | size: 0x8 */
u32 fn_800896C0(void) {
    return *(u32*)&lbl_8047A674;
}

/* 0x800896C8 | size: 0x8 */
u32 fn_800896C8(void) {
    return *(u32*)&lbl_8047A670;
}

/* 0x800896D0 | size: 0x8 */
void fn_800896D0(u32 v) {
    *(u32*)&lbl_80478960 = v;
}

/* 0x800896D8 | size: 0x8 */
void fn_800896D8(u32 v) {
    *(u32*)&lbl_8047A674 = v;
}

/* 0x800896E0 | size: 0x8 */
void fn_800896E0(u32 v) {
    *(u32*)&lbl_8047A670 = v;
}

/* 0x800896E8 | size: 0x290 */
void fn_800896E8(void) {
    extern void fn_80089978();
    extern void fn_80132A38();
    extern void fn_80189990();
    extern void fn_8018C1E8();
    extern void fn_801C40F0();
    extern void fn_801CA5C4();
    extern void fn_801EEAD0();
    extern void fn_801FC794();
    extern void fn_801FCA2C();
    extern void fn_801FCAFC();
    extern void fn_801FCB40();
    extern void fn_801FCB84();
    extern void fn_801FCB94();
    extern void fn_801FCC3C();
    extern void fn_801FCC54();
    extern void fn_801FCCC4();
    extern void fn_8020E0F8();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r30 = r3;
    r26 = r4;
    r3 = 0x0;
    ((void(*)(void))fn_80083CFC)();
    r31 = r3;
    if (r31 == 0) {
        r3 = 0x0;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4000);
    if (tmp == 1) {
        r4 = r31 + 0x4004;
        r3 = 0x4d;
        fn_80132A38();
        r3 = r30;
        r4 = r26;
        r5 = 0xe0;
        fn_80189990();

    } else {
        r4 = r31 + 0x4060;
        r3 = 0x4d;
        fn_80132A38();
        r3 = r30;
        r4 = r26;
        r5 = 0xe0;
        fn_80189990();
        ((void(*)(void))fn_8001E184)();
        tmp = (s8)r3;
        if ((s32)tmp != 0) {
            if ((s32)tmp < 0) {
            } else {

            }
            r3 = 0x0;
            return;
        }
    }
    tmp = 0x2;
    r3 = 0x231;
    *(u8*)((u8*)r31 + 0x4000) = tmp;
    fn_8020E0F8();
    r3 = 0x9;
    fn_801FCCC4();
    r4 = *(u8*)((u8*)r31 + 0x4124);
    r28 = r3;
    fn_801FCB94();
    r4 = *(u8*)((u8*)r31 + 0x4125);
    r3 = r28;
    fn_801FCC54();
    r4 = *(u16*)((u8*)r31 + 0x4134);
    r3 = r28;
    fn_801FCB84();
    r4 = *(u8*)((u8*)r31 + 0x4136);
    r3 = r28;
    fn_801FCAFC();
    r27 = r31;
    r29 = 0x0;
    do {
        r5 = *(u16*)((u8*)r27 + 0x4126);
        r3 = r28;
        r4 = r29 & 0xFF;
        fn_801FCB40();
        r27 = r27 + 0x2;
        r29 = r29 + 0x1;
    } while ((s32)r29 < 4);
    r3 = r28;
    fn_801FCC3C();
    fn_801FCA2C();
    r29 = 0x0;
    r26 = 0x0;
    tmp = r29 * 0x2a;
    r28 = r3;
    r27 = r31 + tmp;
    r27 = r27 + 0x4000;
    while ((s32)r26 < 4) {

        r3 = r28;
        r4 = r27 + 0x138;
        fn_80089978();
        tmp = r3 & 0xFF;
        if (tmp == 1) {
            r28 = r28 + 0x50;
            r29 = r29 + 0x1;
        }
        r27 = r27 + 0x2a;
        r26 = r26 + 0x1;

    }
    while ((s32)r29 < 6) {

        r3 = r28;
        r4 = 0x0;
        fn_801FC794();
        r28 = r28 + 0x50;
        r29 = r29 + 0x1;

    }
    r3 = r31 + 0x4118;
    tmp = r31 + 0x40bc;
    r5 = 0x9;
    *(u32*)&lbl_8047A670 = r3;
    r3 = 0x231;
    r4 = 0x1;
    *(u32*)&lbl_80478960 = r5;
    r5 = 0x0;
    *(u32*)&lbl_8047A674 = tmp;
    fn_801CA5C4();
    r29 = r3;
    do {
        if (r29 != 2) break;
        r3 = *(u8*)((u8*)r31 + 0x41E1);
        do {
            if (r3 == 0) break;
            fn_801EEAD0();
            tmp = r3 & 0xFF;
            if (tmp != 1) break;
            tmp = 0x1;
            break;
        } while (0);

        tmp = 0x0;

        tmp = tmp & 0xFF;
        if (tmp != 1) break;
        r3 = r31;
        ((void(*)(void))fn_800830A4)();
        r3 = 0x0;
        ((void(*)(void))fn_80083CFC)();
        if (r3 != 0) {
            r4 = *(u8*)((u8*)r3 + 0x4136);
        } else {

            r4 = 0x0;
        }
        r3 = (u32)&lbl_802EEB98;
        r5 = 0x0;
        r3 = (u32)&lbl_802EEB98;
        tmp = 0x10;
        ctr_fn = (void(*)(void))tmp;
        do {
            tmp = *(u8*)((u8*)r3 + 0x1);
            if (r4 == tmp) {
                r3 = (u32)&lbl_802EEB98;
                tmp = r5 << 1;
                r3 = (u32)&lbl_802EEB98;
                r4 = *(u8*)(r3 + tmp);
                break;
            }
            r3 = r3 + 0x2;
            r5 = r5 + 0x1;
        } while (--ctr != 0);
        r3 = (u32)&lbl_802EEB98;
        r3 = (u32)&lbl_802EEB98;
        r4 = *(u8*)((u8*)r3 + 0x0);

        r3 = r30;
        r5 = 0x0;
        fn_8018C1E8();
    } while (0);

    r3 = 0x1;
    fn_801C40F0();
    r3 = r29;

    return;
}

/* 0x80089978 | size: 0x214 */
void fn_80089978(void) {
    extern void fn_801EEAD0();
    extern void fn_801EEE6C();
    extern void fn_801FC684();
    extern void fn_801FC694();
    extern void fn_801FC6D4();
    extern void fn_801FC6E4();
    extern void fn_801FC6F4();
    extern void fn_801FC744();
    extern void fn_801FC784();
    extern void fn_801FC794();
    extern void fn_801FC7A4();
    extern void fn_801FC7B4();
    extern void fn_801FC7D4();
    extern void fn_801FC7E4();
    extern void fn_801FC808();
    extern void fn_801FC828();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r28 = r3;
    r29 = r4;
    r3 = *(u8*)((u8*)r29 + 0x2);
    do {
        if (r3 == 0) break;
        fn_801EEAD0();
        tmp = r3 & 0xFF;
        if (tmp != 1) break;
        tmp = 0x1;
        break;
    } while (0);

    tmp = 0x0;

    tmp = tmp & 0xFF;
    if (tmp == 1) {
        r3 = r28;
        r4 = 0x0;
        fn_801FC794();
        r3 = 0x0;
    } else {

        r4 = *(u16*)((u8*)r29 + 0x0);
        r3 = r28;
        fn_801FC794();
        r3 = r28;
        r4 = 0x0;
        fn_801FC828();
        r4 = *(u8*)((u8*)r29 + 0x2);
        r3 = r28;
        fn_801FC7B4();
        r3 = *(u8*)((u8*)r29 + 0x2);
        if (r3 != 0) {
            r4 = *(u8*)((u8*)r29 + 0x28);
            fn_801EEE6C();
        }
        r4 = *(u8*)((u8*)r29 + 0x3);
        r3 = r28;
        fn_801FC7D4();
        r31 = r29;
        r30 = 0x0;
        do {
            r5 = *(u16*)((u8*)r31 + 0x4);
            r3 = r28;
            r4 = r30 & 0xFF;
            fn_801FC744();
            r31 = r31 + 0x2;
            r30 = r30 + 0x1;
        } while ((s32)r30 < 4);
        r4 = *(u16*)((u8*)r29 + 0xC);
        r3 = r28;
        fn_801FC784();
        r4 = *(u8*)((u8*)r29 + 0xE);
        r3 = r28;
        fn_801FC7A4();
        r5 = *(u8*)((u8*)r29 + 0xF);
        r3 = r28;
        r4 = 0x0;
        fn_801FC808();
        r5 = *(u8*)((u8*)r29 + 0x10);
        r3 = r28;
        r4 = 0x1;
        fn_801FC808();
        r5 = *(u8*)((u8*)r29 + 0x11);
        r3 = r28;
        r4 = 0x2;
        fn_801FC808();
        r5 = *(u8*)((u8*)r29 + 0x12);
        r3 = r28;
        r4 = 0x3;
        fn_801FC808();
        r5 = *(u8*)((u8*)r29 + 0x13);
        r3 = r28;
        r4 = 0x4;
        fn_801FC808();
        r5 = *(u8*)((u8*)r29 + 0x14);
        r3 = r28;
        r4 = 0x5;
        fn_801FC808();
        r5 = *(s16*)((u8*)r29 + 0x16);
        r3 = r28;
        r4 = 0x0;
        fn_801FC7E4();
        r5 = *(s16*)((u8*)r29 + 0x18);
        r3 = r28;
        r4 = 0x1;
        fn_801FC7E4();
        r5 = *(s16*)((u8*)r29 + 0x1A);
        r3 = r28;
        r4 = 0x2;
        fn_801FC7E4();
        r5 = *(s16*)((u8*)r29 + 0x1C);
        r3 = r28;
        r4 = 0x3;
        fn_801FC7E4();
        r5 = *(s16*)((u8*)r29 + 0x1E);
        r3 = r28;
        r4 = 0x4;
        fn_801FC7E4();
        r5 = *(s16*)((u8*)r29 + 0x20);
        r3 = r28;
        r4 = 0x5;
        fn_801FC7E4();
        r4 = *(s16*)((u8*)r29 + 0x22);
        r3 = r28;
        fn_801FC6F4();
        tmp = *(u8*)((u8*)r29 + 0x24);
        r3 = r28;
        r4 = (s8)tmp;
        fn_801FC6E4();
        r4 = *(u8*)((u8*)r29 + 0x25);
        r3 = r28;
        fn_801FC6D4();
        r4 = *(u8*)((u8*)r29 + 0x26);
        r3 = r28;
        fn_801FC694();
        r4 = *(u8*)((u8*)r29 + 0x27);
        r3 = r28;
        fn_801FC684();
        r3 = 0x1;
    }
    return;
}

/* 0x80089B8C | size: 0x84 */
#pragma push
#pragma peephole off
u8 GbaMisc_GetMappedContextByte(void) {
    GbaMiscContext* ptr;
    u32 value;
    u8* table;
    u32 index;
    u32 count;

    ptr = fn_80083CFC(0);
    if (ptr != 0) {
        value = ptr->tableKey_4136;
    } else {
        value = 0;
    }
    for (table = lbl_802EEB98, index = 0, count = 0x10; count != 0; count--) {
        if (value == table[1]) {
            return lbl_802EEB98[index << 1];
        }
        table += 2;
        index++;
    }
    return lbl_802EEB98[0];
}
#pragma pop

/* 0x80089C10 | size: 0x44 */
#pragma push
#pragma scheduling off
s32 GbaMisc_HasActiveContextState(void) {
    GbaMiscContext* ptr;

    ptr = fn_80083CFC(0);
    if (ptr != 0) {
        if (ptr->state_4000 != 0) {
            return 1;
        }
    }
    return 0;
}
#pragma pop

/* 0x80089C54 | size: 0x30 */
#pragma push
#pragma scheduling off
s32 fn_80089C54(void) {
    extern s32 fn_80083BF8(s32);
    return fn_80083BF8(0) > 0;
}
#pragma pop

/* 0x80089C84 | size: 0x24 */
#pragma push
#pragma scheduling off
void fn_80089C84(s32 param) {
    extern void fn_80071700(s32);
    fn_80071700(param - 1);
}
#pragma pop

/* 0x80089CA8 | size: 0x88 */
s32 GbaMisc_PollEntryStatusA(s32 r31) {
    extern s32 fn_800719A8(s32);
    s32 n;

    n = fn_800719A8(r31 - 1);
    if (n < 0) {
        u16 *base = (u16*)&lbl_8047A684;
        base[r31 - 1] = 0;
    } else if (n == 1 || n == 2) {
        u16 *base = (u16*)&lbl_8047A684;
        u32 v = base[r31 - 1] + 1;
        base[r31 - 1] = v;
        if ((u16)v <= 0xa) {
            n = -1;
        }
    }
    return n;
}

/* 0x80089D30 | size: 0x44 */
#pragma push
#pragma peephole off
s32 GbaMisc_ResetEntryStatusA(s32 param) {
    extern s32 fn_80071AE4(s32);
    s32 ret;
    u32 tmp;
    u32 r4;

    ret = fn_80071AE4(param - 1);
    tmp = GBA_MISC_ENTRY_HALF_OFFSET(param);
    r4 = (u32)&lbl_8047A684;
    r4 = r4 + tmp;
    tmp = 0;
    *(u16*)((u8*)r4 + (-2)) = tmp;
    return ret;
}
#pragma pop

/* 0x80089D74 | size: 0x24 */
#pragma push
#pragma scheduling off
void fn_80089D74(s32 param) {
    extern void fn_800722A0(s32);
    fn_800722A0(param - 1);
}
#pragma pop

/* 0x80089D98 | size: 0x88 */
s32 fn_80089D98(s32 r31) {
    extern s32 fn_80072548(s32);
    s32 n;
    n = fn_80072548(r31 - 1);
    if (n < 0) {
        u16 *base = (u16*)&lbl_8047A684;
        base[r31 - 1] = 0;
    } else if (n == 1 || n == 2) {
        u16 *base = (u16*)&lbl_8047A684;
        u32 v = base[r31 - 1] + 1;
        base[r31 - 1] = v;
        if ((u16)v <= 0xa) {
            n = -1;
        }
    }
    return n;
}

/* 0x80089E20 | size: 0x138 */
#pragma push
#pragma peephole off
s32 fn_80089E20(s32 r30, void* r31, u32 r5, u32 r29) {
    extern void fn_8008AE18(void*, void*);
    extern u8 fn_8011E868(void*);
    extern u16 fn_8011F5C8(void*);
    extern u8 fn_80265F14(s32);
    u8 sp[0x78];
    u32 tmp;
    u32 r3;
    u32 r4;
    u32 r6;
    s32 ret;

    tmp = r5 & 0x0000FF00;
    r4 = r5 & 0x00FF0000;
    r3 = r5 << 24;
    r5 = (u32)r5 >> 24;
    tmp = tmp << 8;
    r4 = (u32)r4 >> 8;
    tmp = r3 | tmp;
    tmp = r4 | tmp;
    tmp = r5 | tmp;
    *(u32*)(sp + 0x0) = tmp;
    r3 = fn_8011F5C8(r31);
    r3 = r3 & 0xFFFF;
    tmp = r29 << 16;
    r6 = tmp | r3;
    tmp = r6 & 0x0000FF00;
    r4 = r6 & 0x00FF0000;
    r5 = r6 << 24;
    tmp = tmp << 8;
    r6 = (u32)r6 >> 24;
    r4 = (u32)r4 >> 8;
    tmp = r5 | tmp;
    tmp = r4 | tmp;
    tmp = r6 | tmp;
    *(u32*)(sp + 0x4) = tmp;
    if (fn_8011F5C8(r31) == 0x181) {
        tmp = fn_8011E868(r31);
        tmp = __cntlzw(tmp & 0xFF);
        tmp = (u32)tmp >> 5;
        r4 = tmp & 0xFF;
        fn_8011D504(r31, r4);
    }
    fn_8008AE18(r31, sp + 0x8);
    r31 = sp + 0x6C;
    memset(r31, 0, 0xc);
    for (r29 = 0; (s32)r29 < 0xb; r31 = (u8*)r31 + 1, r29++) {
        *(u8*)r31 = fn_80265F14(r29);
    }
    ret = fn_800726A8(r30 - 1, sp + 0x0);
    r3 = r30 << 1;
    r4 = (u32)&lbl_8047A684;
    r4 = r4 + r3;
    r3 = 0;
    *(u16*)((u8*)r4 + (-2)) = r3;
    return ret;
}
#pragma pop

/* 0x80089F58 | size: 0x8 */
u32 fn_80089F58(u32 v) {
    return v & 0xFFFF;
}

/* 0x80089F60 | size: 0x8 */
u32 fn_80089F60(u32 v) {
    return (v >> 8) & 0xFF;
}

/* 0x80089F68 | size: 0x8 */
u32 fn_80089F68(u32 v) {
    return v & 0xFF;
}

/* 0x80089F70 | size: 0x8 */
u32 fn_80089F70(u32 v) {
    return v >> 16;
}

/* 0x80089F78 | size: 0xA24 */
void fn_80089F78(void) {
    extern void fn_8008C5D4();
    extern void fn_8011E7C0();
    extern void fn_8011E808();
    extern void fn_8011E8F4();
    extern void fn_8011F188();
    extern void fn_8011F1F0();
    extern void fn_8011F228();
    extern void fn_8011F4A8();
    extern void fn_8011F4F0();
    extern void fn_8011F598();
    extern void fn_8011F5C8();
    extern void fn_801231A4();
    extern void fn_80135A70();
    extern void fn_801EF634();
    extern void fn_801F02AC();
    extern void fn_801F1700();
    extern void fn_801F2020();
    extern void fn_801F54A4();
    extern void fn_801F6B48();
    extern void fn_801F8C00();
    extern void fn_801F981C();
    extern void fn_801F986C();
    extern void fn_801FCDB4();
    extern void fn_801FD614();
    extern void fn_801FDB78();
    extern void fn_801FF1BC();
    extern void fn_801FFEC8();
    extern void fn_802042E0();
    extern void fn_80205B8C();
    extern void fn_80205BE8();
    extern void fn_80207BF4();
    extern void fn_8020E1A4();
    extern void fn_8020E1BC();
    extern void fn_8020E1D4();
    extern void fn_8020E204();
    extern void fn_8022B2CC();
    extern void fn_80265924();
    extern void fn_8008A99C();
    u8 sp[0x8D0];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r14 = 0;
    u32 r15 = 0;
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

    r15 = r4;
    r17 = r5;
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x14;
    r6 = 0x0;
    fn_801F54A4();
    r24 = r3 & 0xFFFF;
    r3 = r24;
    fn_8020E204();
    r18 = r3;
    fn_8020E1D4();
    r19 = r3;
    r3 = r18;
    fn_8020E1BC();
    r16 = r3;
    r3 = r18;
    fn_8020E1A4();
    r18 = r3;
    r3 = r19 & 0xFF;
    tmp = r18 & 0xFF;
    r4 = r15;
    tmp = r3 * tmp;
    r5 = r24;
    r3 = 0xb;
    tmp = tmp << 1;
    r23 = tmp & 0xFF;
    fn_801F02AC();
    r4 = 0x0;
    r20 = r3;
    fn_801F981C();
    tmp = r19 & 0xFF;
    r4 = 0x1;
    *(u8*)(sp + 0x24) = r4;
    if (tmp == 2) {
        r4 = r20;
        r5 = r24;
        r3 = 0x7;
        fn_801F02AC();
        r4 = 0x0;
        r19 = r3;
        fn_801F981C();
        r3 = r19;
        fn_801FCDB4();
        *(u8*)(sp + 0x25) = r3;
        r4 = r20;
        r5 = r24;
        r3 = 0x9;
        fn_801F02AC();
        r4 = 0x0;
        r19 = r3;
        fn_801F981C();
        r3 = r19;
        fn_801FCDB4();
        *(u8*)(sp + 0x26) = r3;
        r4 = r20;
        r5 = r24;
        r3 = 0xa;
        fn_801F02AC();
        r4 = 0x0;
        r19 = r3;
        fn_801F981C();
        r3 = r19;
        fn_801FCDB4();
        *(u8*)(sp + 0x27) = r3;

    } else {
        tmp = r18 & 0xFF;
        if (tmp == 2) {
            r3 = r20;
            r4 = 0x1;
            fn_801F981C();
            r4 = r20;
            r5 = r24;
            r3 = 0x9;
            fn_801F02AC();
            r4 = 0x0;
            r19 = r3;
            fn_801F981C();
            r3 = r19;
            r4 = 0x1;
            fn_801F981C();
            tmp = 0x2;
            r4 = 0x1;
            *(u8*)(sp + 0x25) = r4;
            *(u8*)(sp + 0x26) = tmp;
            *(u8*)(sp + 0x27) = tmp;

        } else {
            r4 = r20;
            r5 = r24;
            r3 = 0x9;
            fn_801F02AC();
            r4 = 0x0;
            fn_801F981C();
            tmp = 0x1;
            *(u8*)(sp + 0x25) = tmp;
        }
    }
    r3 = r15;
    r4 = r17 & 0xFFFF;
    fn_801F981C();
    tmp = r18 & 0xFF;
    r22 = r3;
    if (tmp == 2) {
        tmp = r17 + 0x1;
    } else {

        tmp = 0x0;
    }
    tmp = tmp & 0xFF;
    r4 = *(u8*)(sp + 0x3B);
    r4 = (r4 & ~0x0000007F) | (((tmp << 0) | (tmp >> 32)) & 0x0000007F);
    r25 = (u32)sp + 0x38;
    *(u8*)(sp + 0x3B) = r4;
    r3 = tmp & 0xFF;
    tmp = r4 & 0xFF;
    tmp = (tmp & ~0x00000080) | (((r3 << 7) | (r3 >> 25)) & 0x00000080);
    *(u8*)(sp + 0x3F) = r23;
    r27 = r25;
    r26 = r16 & 0xFF;
    *(u8*)(sp + 0x3B) = tmp;
    r19 = 0x0;
    r18 = 0x0;
    r21 = 0x0;
    r28 = 0x8;
    while (1) {
        if ((s32)r21 >= (s32)r26) break;
        r3 = r15;
        r4 = r21 & 0xFFFF;
        fn_801F986C();
        r29 = r3;
        if (r29 == 0) break;
        fn_80205BE8();
        r17 = r3;
        fn_8011E7C0();
        tmp = r3 & 0xFFFF;
        r3 = r15;
        tmp = tmp << r28;
        r4 = r29;
        r19 = r19 | tmp;
        fn_801F8C00();
        tmp = r3 & 0xFF;
        r3 = r17;
        tmp = tmp << r28;
        r18 = r18 | tmp;
        fn_8011E8F4();
        r16 = r3;
        r3 = r17;
        fn_8011F188();
        tmp = r3 & 0xFFFF;
        r3 = r17;
        r4 = tmp << 8;
        tmp = (s32)tmp >> 8;
        tmp = r4 | tmp;
        tmp = tmp & 0xFFFF;
        *(u16*)((u8*)r27 + 0x24) = tmp;
        fn_8008C5D4();
        r20 = r3 & 0xFFFF;
        r3 = r29;
        tmp = r20 & 0xFF;
        *(u8*)((u8*)r27 + 0x26) = tmp;
        fn_802042E0();
        r5 = r3 & 0xFFFF;
        tmp = *(u8*)((u8*)r27 + 0x27);
        r4 = -r5;
        r3 = r17;
        r4 = r4 | r5;
        r4 = (u32)r4 >> 31;
        tmp = (tmp & ~0x00000080) | (((r4 << 7) | (r4 >> 25)) & 0x00000080);
        *(u8*)((u8*)r27 + 0x27) = tmp;
        fn_8011E808();
        r4 = r3 & 0xFF;
        r7 = r16 & 0xFF;
        r3 = 0xff - r4;
        r6 = *(u8*)((u8*)r27 + 0x27);
        r5 = r7 & 0xF;
        tmp = r3 | tmp;
        r3 = r7 & 0x000000F0;
        tmp = (u32)tmp >> 31;
        r4 = -r5;
        r6 = (r6 & ~0x00000040) | (((tmp << 6) | (tmp >> 26)) & 0x00000040);
        tmp = -r3;
        *(u8*)((u8*)r27 + 0x27) = r6;
        r4 = r4 | r5;
        r5 = (u32)r4 >> 31;
        r3 = tmp | r3;
        r4 = *(u8*)((u8*)r27 + 0x27);
        r4 = (r4 & ~0x00000010) | (((r5 << 4) | (r5 >> 28)) & 0x00000010);
        tmp = (s32)r20 >> 8;
        r5 = (u32)r3 >> 31;
        *(u8*)((u8*)r27 + 0x27) = r4;
        r4 = tmp & 0xFF;
        r3 = r29;
        tmp = *(u8*)((u8*)r27 + 0x27);
        tmp = (tmp & ~0x00000020) | (((r5 << 5) | (r5 >> 27)) & 0x00000020);
        *(u8*)((u8*)r27 + 0x27) = tmp;
        tmp = *(u8*)((u8*)r27 + 0x27);
        tmp = (tmp & ~0x0000000F) | (((r4 << 0) | (r4 >> 32)) & 0x0000000F);
        *(u8*)((u8*)r27 + 0x27) = tmp;
        fn_802042E0();
        tmp = r3 & 0xFFFF;
        r31 = r27;
        r3 = tmp << 8;
        r30 = r27;
        tmp = (s32)tmp >> 8;
        r29 = r27;
        tmp = r3 | tmp;
        r20 = 0x0;
        tmp = tmp & 0xFFFF;
        *(u16*)((u8*)r27 + 0x28) = tmp;
        do {
            r3 = r17;
            r4 = r20 & 0xFFFF;
            fn_8011F228();
            tmp = r3;
            r3 = r17;
            r16 = tmp;
            r4 = r20 & 0xFFFF;
            tmp = r16 & 0xFFFF;
            r5 = tmp << 8;
            tmp = (s32)tmp >> 8;
            tmp = r5 | tmp;
            tmp = tmp & 0xFFFF;
            *(u16*)((u8*)r31 + 0x2C) = tmp;
            fn_8011F1F0();
            *(u8*)((u8*)r30 + 0x34) = r3;
            r4 = r16;
            r3 = r29 + 0x38;
            ((void(*)(void))fn_80083ECC)();
            r31 = r31 + 0x2;
            r30 = r30 + 0x1;
            r29 = r29 + 0x50;
            r20 = r20 + 0x1;
        } while ((s32)r20 < 4);
        r28 = r28 + 0x4;
        r27 = r27 + 0x154;
        r21 = r21 + 0x1;

    }

    r20 = r21 & 0xFF;
    tmp = r18 & 0x0000FF00;
    r3 = r20 << 2;
    r4 = r18 & 0x00FF0000;
    r3 = r3 + 0x8;
    r5 = 0x1;
    r5 = r5 << r3;
    r3 = r18 << 24;
    r5 = -r5;
    tmp = tmp << 8;
    r19 = r19 | r5;
    r4 = (u32)r4 >> 8;
    r5 = r19 & 0x0000FF00;
    tmp = r3 | tmp;
    r3 = r19 & 0x00FF0000;
    r6 = r19 << 24;
    r5 = r5 << 8;
    r8 = (u32)r19 >> 24;
    r7 = (u32)r3 >> 8;
    r3 = (u32)r18 >> 24;
    r5 = r6 | r5;
    tmp = r4 | tmp;
    r5 = r7 | r5;
    r5 = r8 | r5;
    r3 = r3 | tmp;
    r4 = (r4 & ~0xFFFFFF00) | (((r5 << 8) | (r5 >> 24)) & 0xFFFFFF00);
    tmp = (tmp & ~0xFFFFFF00) | (((r3 << 8) | (r3 >> 24)) & 0xFFFFFF00);
    r3 = r22;
    r26 = 0x0;
    *(u32*)(sp + 0x3C) = tmp;
    fn_80205B8C();
    tmp = r3;
    r3 = r22;
    r21 = tmp;
    r4 = (u32)sp + 0x20;
    r5 = (u32)sp + 0x1c;
    fn_801FDB78();
    r3 = r21;
    r4 = r9 & 0x0000FF00;
    r8 = r9 & 0x00FF0000;
    tmp = r10 & 0x0000FF00;
    r5 = r10 & 0x00FF0000;
    r7 = r9 << 24;
    r6 = r4 << 8;
    r4 = r10 << 24;
    tmp = tmp << 8;
    r8 = (u32)r8 >> 8;
    r6 = r7 | r6;
    r5 = (u32)r5 >> 8;
    tmp = r4 | tmp;
    r7 = (u32)r9 >> 24;
    r4 = r8 | r6;
    r6 = r7 | r4;
    r4 = (u32)r10 >> 24;
    tmp = r5 | tmp;
    tmp = r4 | tmp;
    *(u32*)(sp + 0x44) = tmp;
    fn_8011F5C8();
    r4 = r3 & 0xFFFF;
    r3 = r4 << 8;
    tmp = (s32)r4 >> 8;
    tmp = r3 | tmp;
    tmp = tmp & 0xFFFF;
    *(u16*)(sp + 0x48) = tmp;
    if (r4 == 0x181) {
        r3 = r22;
        r4 = 0x0;
        fn_801FD614();
        tmp = r3 & 0xFFFF;
        if (tmp == 0xa) {
            r4 = 0x1;

        } else if (tmp == 0xb) {
            r4 = 0x2;

        } else if (tmp == 0xf) {
            r4 = 0x3;

        } else {
            r4 = 0x0;
        }
        tmp = r3 & 0xFFFF;
        if (tmp == 0xa) {
            tmp = 0x1;

        } else if (tmp == 0xb) {
            tmp = 0x2;

        } else if (tmp == 0xf) {
            tmp = 0x3;

        } else {
            tmp = 0x0;
        }
        r3 = tmp & 0xFFFF;
        tmp = r4 & 0xFFFF;
        r3 = r3 << 8;
        tmp = (s32)tmp >> 8;
        tmp = r3 | tmp;
        tmp = tmp & 0xFFFF;
        *(u16*)(sp + 0x4A) = tmp;
    } else {

        tmp = 0x0;
        *(u16*)(sp + 0x4A) = tmp;
    }
    if ((s32)tmp != 0) {
        r3 = 0x0;
        r6 = 0x0;

    } else {
        fn_801F6B48();
        r4 = r22;
        r5 = (u32)sp + 0x18;
        fn_801F2020();
        tmp = r3 & 0xFF;
        if (tmp == 2) {
            fn_80207BF4();
            tmp = r3 & 0xFFFF;
            if (tmp == 0x17) {
                tmp = 0x2;

            } else if (tmp == 0x2a) {
                tmp = 0x3;

            } else if (tmp == 0x47) {
                tmp = 0x4;

            } else {
                tmp = 0x0;
            }
            r3 = tmp & 0xFF;
        }
        r4 = (u32)sp + 0x28;
        tmp = r6 << 2;
        r4 = r4 + tmp;
        ctr_fn = (void(*)(void))r6;
        if ((s32)r6 > 0) {
            do {
                tmp = *(u32*)((u8*)r4 + 0x0);
                if (r5 == tmp) break;
            } while (--ctr != 0);
        }
    }
    tmp = *(u8*)(sp + 0x5B);
    tmp = (tmp & ~0x0000000F) | (((r3 << 0) | (r3 >> 32)) & 0x0000000F);
    r4 = r6 & 0xFF;
    r3 = r22;
    *(u8*)(sp + 0x5B) = tmp;
    tmp = tmp & 0xFF;
    tmp = (tmp & ~0x000000F0) | (((r4 << 4) | (r4 >> 28)) & 0x000000F0);
    r4 = 0x0;
    *(u8*)(sp + 0x5B) = tmp;
    fn_801FF1BC();
    tmp = *(u8*)(sp + 0x5A);
    tmp = (tmp & ~0x0000000F) | (((r3 << 0) | (r3 >> 32)) & 0x0000000F);
    r19 = 0x0;
    r16 = r25;
    *(u8*)(sp + 0x5A) = tmp;
    r17 = r25;
    r18 = r19;
    r3 = (u32)fn_8008A99C;
    r15 = (u32)fn_8008A99C;
    do {
        tmp = 0x0;
        r3 = r21;
        *(u8*)&lbl_8047A678 = tmp;
        r4 = r19 & 0xFFFF;
        fn_8011F228();
        tmp = r3;
        r3 = r21;
        r27 = tmp;
        r4 = r19 & 0xFFFF;
        tmp = r27 & 0xFFFF;
        r5 = tmp << 8;
        tmp = (s32)tmp >> 8;
        tmp = r5 | tmp;
        tmp = tmp & 0xFFFF;
        *(u16*)((u8*)r16 + 0x14) = tmp;
        fn_8011F1F0();
        *(u8*)((u8*)r17 + 0x1C) = r3;
        r3 = r22;
        r4 = r27;
        r5 = r24;
        r6 = r15;
        r7 = 0x1;
        r8 = 0x0;
        r9 = -0x1;
        fn_8022B2CC();
        tmp = *(u8*)&lbl_8047A678;
        if (tmp != 0) {
            tmp = 0x8;
            tmp = tmp << r18;
            tmp = r26 | tmp;
            r26 = tmp & 0xFFFF;
        }
        r3 = r22;
        r4 = r19 & 0xFFFF;
        r6 = (u32)sp + 0x10;
        r5 = 0x1;
        fn_801FFEC8();
        r3 = r3 & 0xFF;
        tmp = r3 << r18;
        tmp = r26 | tmp;
        r26 = tmp & 0xFFFF;
        if (r3 == 5) {
            r14 = *(u16*)(sp + 0x10);
        }
        r16 = r16 + 0x2;
        r17 = r17 + 0x1;
        r18 = r18 + 0x4;
        r19 = r19 + 0x1;
    } while ((s32)r19 < 4);
    tmp = (s32)r26 >> 8;
    r3 = r26 << 8;
    tmp = r3 | tmp;
    r3 = r14 << 8;
    r4 = tmp & 0xFFFF;
    tmp = (s32)r14 >> 8;
    *(u16*)(sp + 0x58) = r4;
    tmp = r3 | tmp;
    r4 = 0x3;
    tmp = tmp & 0xFFFF;
    r3 = *(u16*)(sp + 0x52);
    if (r3 != tmp) {
        r4 = 0x2;
        r3 = *(u16*)(sp + 0x50);
        if (r3 != tmp) {
            r4 = 0x1;
            r3 = *(u16*)(sp + 0x4E);
            if (r3 != tmp) {
                r4 = 0x0;
    }
    }
    }
    r3 = r4 & 0xFF;
    tmp = *(u8*)(sp + 0x5A);
    tmp = (tmp & ~0x000000F0) | (((r3 << 4) | (r3 >> 28)) & 0x000000F0);
    r14 = (u32)sp + 0x28;
    *(u8*)(sp + 0x5A) = tmp;
    r15 = (u32)sp + 0x24;
    r16 = 0x0;
    while ((s32)r16 < (s32)r23) {

        r3 = r25 + 0x81c;
        r4 = 0x0;
        r5 = 0xc;
        memset((void*)r3, (int)r4, (u32)r5);
        r3 = *(u32*)((u8*)r14 + 0x0);
        if (r3 == r22) {
            tmp = 0xff;
            *(u8*)((u8*)r25 + 0x81C) = tmp;
        } else {

            fn_80205B8C();
            r18 = r3;
            fn_8011F598();
            r17 = r3;
            fn_80135A70();
            r19 = r3 & 0xFF;
            r3 = r18;
            fn_8011F4F0();
            r4 = r3;
            r5 = r19;
            r3 = r25 + 0x81c;
            ((void(*)(void))fn_800F9AEC)();
            if ((s32)r3 < 0xa) {
                tmp = r3 + 0x81c;
                r3 = 0xff;
                *(u8*)(r25 + tmp) = r3;
            }
            r3 = r18;
            fn_8011F5C8();
            r19 = r3;
            r3 = r18;
            fn_801231A4();
            r5 = r19 & 0xFFFF;
            r6 = *(u8*)((u8*)r25 + 0x826);
            r6 = (r6 & ~0x000000C0) | (((r3 << 6) | (r3 >> 26)) & 0x000000C0);
            r4 = *(u8*)((u8*)r15 + 0x0);
            tmp = 0x20 - r5;
            *(u8*)((u8*)r25 + 0x826) = r6;
            r3 = __cntlzw(tmp);
            tmp = 0x1d - r5;
            r3 = (u32)r3 >> 5;
            r5 = *(u8*)((u8*)r25 + 0x826);
            r3 = r3 & 0xFF;
            tmp = __cntlzw(tmp);
            r5 = (r5 & ~0x00000020) | (((r3 << 5) | (r3 >> 27)) & 0x00000020);
            r3 = r17;
            *(u8*)((u8*)r25 + 0x826) = r5;
            tmp = (u32)tmp >> 5;
            r5 = tmp & 0xFF;
            tmp = *(u8*)((u8*)r25 + 0x826);
            tmp = (tmp & ~0x00000010) | (((r5 << 4) | (r5 >> 28)) & 0x00000010);
            *(u8*)((u8*)r25 + 0x826) = tmp;
            tmp = *(u8*)((u8*)r25 + 0x826);
            tmp = (tmp & ~0x00000007) | (((r4 << 0) | (r4 >> 32)) & 0x00000007);
            *(u8*)((u8*)r25 + 0x826) = tmp;
            fn_80135A70();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                tmp = *(u8*)((u8*)r25 + 0x826);
                r3 = 0x1;
                tmp = (tmp & ~0x00000008) | (((r3 << 3) | (r3 >> 29)) & 0x00000008);
                *(u8*)((u8*)r25 + 0x826) = tmp;
            } else {

                tmp = *(u8*)((u8*)r25 + 0x826);
                r3 = 0x0;
                tmp = (tmp & ~0x00000008) | (((r3 << 3) | (r3 >> 29)) & 0x00000008);
                *(u8*)((u8*)r25 + 0x826) = tmp;
            }
            r3 = r18;
            fn_8011F4A8();
            *(u8*)((u8*)r25 + 0x827) = r3;
        }
        r25 = r25 + 0xc;
        r14 = r14 + 0x4;
        r15 = r15 + 0x1;
        r16 = r16 + 0x1;

    }
    r3 = r20 * 0x154;
    tmp = (u32)sp + 0x38;
    r4 = (u32)sp + 0x854;
    r14 = r23 * 0xc;
    r3 = r3 + 0x24;
    r5 = r14;
    r3 = tmp + r3;
    ((void(*)(void))fn_800C8174)();
    tmp = r20 * 0x154;
    r4 = (u32)sp + 0x38;
    r5 = tmp + r14;
    r3 = r15;
    r5 = r5 + 0x24;
    ((void(*)(void))fn_80072D58)();
    do {
        fn_801EF634();
        tmp = r3 & 0xFFFF;
        if (tmp == 1) {
            r3 = r15;
            ((void(*)(void))fn_80072A00)();
            r3 = 0x50000;
            return;
        }
        r3 = 0x0;
        fn_801F1700();
        tmp = r3 & 0xFF;
        if (tmp == 1) {
            fn_80265924();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                r3 = r15;
                ((void(*)(void))fn_80072A00)();
                r3 = 0x40000;
                return;
        }
        }
        r3 = r15;
        r4 = (u32)sp + 0x14;
        ((void(*)(void))fn_80072C74)();
        if ((s32)r3 > 0) {
            r3 = r3 | (0x5 << 16);
            return;
        }
        if ((s32)r3 == 0) {
            tmp = r5 & 0x0000FF00;
            r4 = r5 & 0x00FF0000;
            r3 = r5 << 24;
            r5 = (u32)r5 >> 24;
            tmp = tmp << 8;
            r4 = (u32)r4 >> 8;
            tmp = r3 | tmp;
            tmp = r4 | tmp;
            r3 = r5 | tmp;
            return;
        }
        ((void(*)(void))_threadSwitch)();
    } while (1);

    return;
}

/* 0x8008A99C | size: 0x10 */
s32 fn_8008A99C(void) {
    lbl_8047A678 = 1;
    return 0;
}

/* 0x8008A9AC | size: 0x38 */
int fn_8008A9AC(u32 r3, u8* r4) {
    r4[0] = r3 & 0xF;
    r4[1] = (r3 >> 4) & 0xF;
    r4[2] = (r3 >> 8) & 0xF;
    r4[3] = (r3 >> 12) & 0xF;
    r4[4] = (r3 >> 16) & 0xF;
    r4[5] = (r3 >> 20) & 0xF;
    return 0;
}

/* 0x8008A9E4 | size: 0x13C */
#pragma push
#pragma peephole off
s32 GbaMisc_GetEntryStatus(s32 idx, u32* out) {
    u32 status;
    u32 tmp;
    u32 r3;
    u32 r4;
    u32 r5;
    u32 offset32;
    u32 offset16;
    s32 ret;

    *out = 0x2000000;
    ret = _AGB_EntryGetStatus__FlPUl(idx - 1, &status);
    if (ret < 0) {
        status = 0x2000000;
        goto returnZero;
    }
    if (ret != 0) {
        *out = 0x3000000;
        offset32 = GBA_MISC_ENTRY_WORD_OFFSET(idx);
        offset16 = GBA_MISC_ENTRY_HALF_OFFSET(idx);
        GbaMisc_EntryStateAtWordOffset(offset32) = 1;
        GbaMisc_EntryCachedStatusAtWordOffset(offset32) = 0;
        GbaMisc_EntryCounterAAtHalfOffset(offset16) = 0;
        GbaMisc_EntryCounterBAtHalfOffset(offset16) = 0;
        return ret;
    }
    r5 = *(volatile u32*)&status;
    tmp = r5 & 0x0000FF00;
    r4 = r5 & 0x00FF0000;
    r3 = r5 << 24;
    r5 = (u32)r5 >> 24;
    tmp = tmp << 8;
    r4 = (u32)r4 >> 8;
    tmp = r3 | tmp;
    tmp = r4 | tmp;
    tmp = r5 | tmp;
    *out = tmp;
    tmp = *out >> 24;
    if (tmp != 0) {
        goto returnZero;
    }
    offset32 = GBA_MISC_ENTRY_WORD_OFFSET(idx);
    offset16 = GBA_MISC_ENTRY_HALF_OFFSET(idx);
    GbaMisc_EntryStateAtWordOffset(offset32) = 1;
    GbaMisc_EntryCachedStatusAtWordOffset(offset32) = 0;
    GbaMisc_EntryCounterAAtHalfOffset(offset16) = 0;
    GbaMisc_EntryCounterBAtHalfOffset(offset16) = 0;
returnZero:
    return 0;
}
#pragma pop

/* 0x8008AB20 | size: 0x2C */
#pragma push
#pragma peephole off
void GbaMisc_SendPackedEntryStatus(s32 param0, u32 param1, u32 param2) {
    u32 packed;

    packed = param2 << 24;
    param0--;
    fn_800730F8(param0, packed | param1);
}
#pragma pop

/* 0x8008AB4C | size: 0x40 */
void fn_8008AB4C(s32 param0, s32 param1) {
    extern void fn_80083D30(s32, void*);
    extern void fn_800733D0(s32, void*);
    u8 buf[0x780];
    fn_80083D30(param1, buf);
    fn_800733D0(param0 - 1, buf);
}

/* 0x8008AB8C | size: 0x14 */
s32 fn_8008AB8C(s32 r3) {
    u16 *base = (u16*)&lbl_8047A67C;
    return base[r3 - 1];
}

/* 0x8008ABA0 | size: 0x44 */
u8 fn_8008ABA0(s32 idx) {
    u32 ret = 0;
    if (GbaMisc_EntryState(idx) != 0) {
        if (GbaMisc_EntryCachedStatus(idx) == 0) {
            ret = 1;
        }
    }
    return (u8)ret;
}

/* 0x8008ABE4 | size: 0x50 */
s32 GbaMisc_SetEntryState(s32 idx, s32 value) {
    u32 r0;
    u32 r5;
    u32 r6;
    u32 r7;
    u32 r8;
    u32 r9;
    u32 r10;
    s32 old;

    r6 = (u32)&lbl_803FB318;
    r7 = idx << 2;
    r0 = r6;
    r5 = (u32)&lbl_803FB308;
    r9 = r0 + r7;
    r10 = idx << 1;
    idx = r10;
    r9 = r9 - 4;
    r6 = r5;
    old = *(s32*)r9;
    r5 = (u32)&lbl_8047A684;
    r7 = r6 + r7;
    r8 = 0;
    r6 = r5 + idx;
    r0 = (u32)&lbl_8047A67C;
    r5 = r0 + idx;
    *(s32*)r9 = value;
    *(u32*)((u8*)r7 + (-4)) = r8;
    *(u16*)((u8*)r6 + (-2)) = r8;
    *(u16*)((u8*)r5 + (-2)) = r8;
    return old;
}

/* 0x8008AC34 | size: 0x1E4 */
void fn_8008AC34(void) {
    u8 sp[0x20];
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

    r29 = r3;
    r3 = (u32)&lbl_803FB318;
    r30 = r29 << 2;
    tmp = (u32)&lbl_803FB318;
    r31 = tmp + r30;
    tmp = *(u32*)((u8*)r31 + 0x0);
    if ((s32)tmp != 2) {
        if ((s32)tmp < 2) {
            if ((s32)tmp < 1 || (s32)tmp >= 4) {
                tmp = r29 << 1;
                r3 = (u32)&lbl_8047A67C;
                r3 = r3 + tmp;
                tmp = 0x0;
                *(u16*)((u8*)r3 + (-2)) = tmp;
                r3 = 0x1;
            } else {

            r4 = (u32)sp + 0x8;
            ((void(*)(void))fn_80073A44)();
            if ((s32)r3 == 0) {
                r7 = r29 << 1;
                r6 = (u32)&lbl_8047A684;
                tmp = (u32)&lbl_8047A67C;
                r5 = *(u16*)(sp + 0x8);
                r4 = tmp + r7;
                r6 = r6 + r7;
                tmp = 0x0;
                *(u16*)((u8*)r4 + (-2)) = r5;
                *(u16*)((u8*)r6 + (-2)) = tmp;
            } else {
            do {
                if ((s32)r3 > 2) break;
                tmp = r29 << 1;
                r4 = (u32)&lbl_8047A684;
                r5 = r4 + tmp;
                r4 = *(u16*)((u8*)r5 + (-2));
                r4 = r4 + 0x1;
                tmp = r4 & 0xFFFF;
                *(u16*)((u8*)r5 + (-2)) = r4;
                if (tmp > 0xa) break;
                r3 = 0x0;
            } while (0);

            r7 = r29 << 1;
            tmp = (u32)&lbl_8047A67C;
            r8 = tmp + r7;
            r6 = 0x0;
            r4 = (u32)&lbl_803FB308;
            *(u16*)((u8*)r8 + 0x0) = r6;
            r4 = (u32)&lbl_803FB308;
            tmp = (u32)&lbl_8047A684;
            r5 = r4 + r30;
            *(u32*)((u8*)r31 + 0x0) = r6;
            r4 = tmp + r7;
            *(u32*)((u8*)r5 + (-4)) = r6;
            *(u16*)((u8*)r4 + (-2)) = r6;
            *(u16*)((u8*)r8 + 0x0) = r6;
            }
            }
        } else {
        ((void(*)(void))fn_80073990)();
        if ((s32)r3 == 0) {
            tmp = r29 << 1;
            r4 = (u32)&lbl_8047A684;
            r4 = r4 + tmp;
            tmp = 0x0;
            *(u16*)((u8*)r4 + (-2)) = tmp;

        } else {
            do {
            if ((s32)r3 > 2) break;
                tmp = r29 << 1;
                r4 = (u32)&lbl_8047A684;
                r5 = r4 + tmp;
                r4 = *(u16*)((u8*)r5 + (-2));
                r4 = r4 + 0x1;
                tmp = r4 & 0xFFFF;
                *(u16*)((u8*)r5 + (-2)) = r4;
                if (tmp > 0xa) break;
                r3 = 0x0;
                break;
            } while (0);

            tmp = 0x0;
            *(u32*)((u8*)r31 + 0x0) = tmp;
        }
        tmp = r29 << 1;
        r4 = (u32)&lbl_8047A67C;
        r4 = r4 + tmp;
        tmp = 0x0;
        *(u16*)((u8*)r4 + (-2)) = tmp;
        }
    } else {
    tmp = r29 << 1;
    r3 = (u32)&lbl_8047A67C;
    r3 = r3 + tmp;
    tmp = 0x0;
    *(u16*)((u8*)r3 + (-2)) = tmp;
    r3 = 0x0;
    }
    r4 = (u32)&lbl_803FB308;
    tmp = (u32)&lbl_803FB308;
    r4 = tmp + r30;
    *(u32*)((u8*)r4 + (-4)) = r3;
    return;
}

/* 0x8008AE18 | size: 0xDC4 */
void fn_8008AE18(void) {
    extern void fn_8011E7A4();
    extern void fn_8011E7D8();
    extern void fn_8011E7F0();
    extern void fn_8011E808();
    extern void fn_8011E820();
    extern void fn_8011E838();
    extern void fn_8011E850();
    extern void fn_8011E868();
    extern void fn_8011E8DC();
    extern void fn_8011E8F4();
    extern void fn_8011E90C();
    extern void fn_8011E938();
    extern void fn_8011E964();
    extern void fn_8011E990();
    extern void fn_8011E9BC();
    extern void fn_8011E9E8();
    extern void fn_8011EA14();
    extern void fn_8011EA40();
    extern void fn_8011EA6C();
    extern void fn_8011EA98();
    extern void fn_8011EAC4();
    extern void fn_8011EAF0();
    extern void fn_8011EB1C();
    extern void fn_8011EB48();
    extern void fn_8011EB60();
    extern void fn_8011EB8C();
    extern void fn_8011EBB8();
    extern void fn_8011EBE4();
    extern void fn_8011EC10();
    extern void fn_8011EC3C();
    extern void fn_8011EC68();
    extern void fn_8011EC94();
    extern void fn_8011ECC0();
    extern void fn_8011ECEC();
    extern void fn_8011EE58();
    extern void fn_8011EE70();
    extern void fn_8011EE9C();
    extern void fn_8011EEC8();
    extern void fn_8011EEF4();
    extern void fn_8011EF20();
    extern void fn_8011EF4C();
    extern void fn_8011EF78();
    extern void fn_8011EFA4();
    extern void fn_8011EFD0();
    extern void fn_8011EFFC();
    extern void fn_8011F028();
    extern void fn_8011F054();
    extern void fn_8011F080();
    extern void fn_8011F0AC();
    extern void fn_8011F0D8();
    extern void fn_8011F104();
    extern void fn_8011F130();
    extern void fn_8011F15C();
    extern void fn_8011F188();
    extern void fn_8011F1A0();
    extern void fn_8011F1B8();
    extern void fn_8011F1F0();
    extern void fn_8011F228();
    extern void fn_8011F45C();
    extern void fn_8011F4A8();
    extern void fn_8011F4C0();
    extern void fn_8011F4D8();
    extern void fn_8011F508();
    extern void fn_8011F520();
    extern void fn_8011F538();
    extern void fn_8011F550();
    extern void fn_8011F568();
    extern void fn_8011F580();
    extern void fn_8011F598();
    extern void fn_8011F5B0();
    extern void fn_8011F5C8();
    extern void fn_8012189C();
    extern void fn_80121984();
    extern void fn_80121ADC();
    extern void fn_80123FBC();
    extern void fn_80135A70();
    extern void fn_80135AB8();
    extern u8 jumptable_802EEBB8[];
    extern u8 jumptable_802EEBE0[];
    u8 sp[0x40];
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
    u32 r12 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r28 = r3;
    r31 = r4;
    r3 = r4;
    r29 = 0x0;
    r4 = 0x0;
    r5 = 0x64;
    memset((void*)r3, (int)r4, (u32)r5);
    r3 = r28;
    fn_80123FBC();
    tmp = r3 & 0xFF;
    if (tmp == 0) return;
    r3 = r28;
    fn_8011F5B0();
    tmp = r3 & 0x0000FF00;
    r5 = r3 & 0x00FF0000;
    r4 = r3 << 24;
    r6 = (u32)r3 >> 24;
    tmp = tmp << 8;
    r5 = (u32)r5 >> 8;
    tmp = r4 | tmp;
    r3 = r28;
    tmp = r5 | tmp;
    tmp = r6 | tmp;
    *(u32*)((u8*)r31 + 0x0) = tmp;
    fn_8011F520();
    tmp = r3 & 0x0000FF00;
    r5 = r3 & 0x00FF0000;
    r4 = r3 << 24;
    r6 = (u32)r3 >> 24;
    tmp = tmp << 8;
    r5 = (u32)r5 >> 8;
    tmp = r4 | tmp;
    r3 = r28;
    tmp = r5 | tmp;
    tmp = r6 | tmp;
    *(u32*)((u8*)r31 + 0x4) = tmp;
    fn_8011F598();
    r30 = r3;
    fn_80135AB8();
    tmp = r3 & 0xFF;
    do {
        if (tmp <= 0xb) {
            r3 = (u32)jumptable_802EEBE0;
            tmp = tmp << 2;
            r3 = (u32)jumptable_802EEBE0;
            tmp = *(u32*)(r3 + tmp);
            ctr_fn = (void(*)(void))tmp;
            tmp = *(u16*)(sp + 0x10);
            r3 = 0x1;
            tmp = (tmp & ~0x00000780) | (((r3 << 7) | (r3 >> 25)) & 0x00000780);
            *(u16*)(sp + 0x10) = tmp;
            break;
            tmp = *(u16*)(sp + 0x10);
            r3 = 0x2;
            tmp = (tmp & ~0x00000780) | (((r3 << 7) | (r3 >> 25)) & 0x00000780);
            *(u16*)(sp + 0x10) = tmp;
            break;
            tmp = *(u16*)(sp + 0x10);
            r3 = 0x3;
            tmp = (tmp & ~0x00000780) | (((r3 << 7) | (r3 >> 25)) & 0x00000780);
            *(u16*)(sp + 0x10) = tmp;
            break;
            tmp = *(u16*)(sp + 0x10);
            r3 = 0x4;
            tmp = (tmp & ~0x00000780) | (((r3 << 7) | (r3 >> 25)) & 0x00000780);
            *(u16*)(sp + 0x10) = tmp;
            break;
            tmp = *(u16*)(sp + 0x10);
            r3 = 0x5;
            tmp = (tmp & ~0x00000780) | (((r3 << 7) | (r3 >> 25)) & 0x00000780);
            *(u16*)(sp + 0x10) = tmp;
            break;
            tmp = *(u16*)(sp + 0x10);
            r3 = 0xf;
            tmp = (tmp & ~0x00000780) | (((r3 << 7) | (r3 >> 25)) & 0x00000780);
            *(u16*)(sp + 0x10) = tmp;
            break;
        }
        tmp = *(u16*)(sp + 0x10);
        r3 = 0x0;
        tmp = (tmp & ~0x00000780) | (((r3 << 7) | (r3 >> 25)) & 0x00000780);
        *(u16*)(sp + 0x10) = tmp;
    } while (0);

    r3 = r30;
    fn_80135A70();
    tmp = r3 & 0xFF;
    do {
        if (tmp <= 9) {
            r3 = (u32)jumptable_802EEBB8;
            tmp = tmp << 2;
            r3 = (u32)jumptable_802EEBB8;
            tmp = *(u32*)(r3 + tmp);
            ctr_fn = (void(*)(void))tmp;
            tmp = 0x1;
            *(u8*)((u8*)r31 + 0x12) = tmp;
            break;
            tmp = 0x2;
            *(u8*)((u8*)r31 + 0x12) = tmp;
            break;
            tmp = 0x5;
            *(u8*)((u8*)r31 + 0x12) = tmp;
            break;
            tmp = 0x3;
            *(u8*)((u8*)r31 + 0x12) = tmp;
            break;
            tmp = 0x4;
            *(u8*)((u8*)r31 + 0x12) = tmp;
            break;
            tmp = 0x7;
            *(u8*)((u8*)r31 + 0x12) = tmp;
            break;
            tmp = 0x2;
            *(u8*)((u8*)r31 + 0x12) = tmp;
            break;
            tmp = 0x2;
            *(u8*)((u8*)r31 + 0x12) = tmp;
            break;
        }
        tmp = 0x0;
        *(u8*)((u8*)r31 + 0x12) = tmp;
    } while (0);

    tmp = *(u8*)((u8*)r31 + 0x13);
    r3 = 0x1;
    tmp = (tmp & ~0x00000002) | (((r3 << 1) | (r3 >> 31)) & 0x00000002);
    r3 = r28;
    *(u8*)((u8*)r31 + 0x13) = tmp;
    fn_8011E8DC();
    r5 = r3 & 0xFF;
    tmp = *(u8*)((u8*)r31 + 0x13);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000004) | (((r4 << 2) | (r4 >> 30)) & 0x00000004);
    *(u8*)((u8*)r31 + 0x13) = tmp;
    fn_8011E838();
    tmp = *(u8*)((u8*)r31 + 0x13);
    tmp = (tmp & ~0x000000F8) | (((r3 << 3) | (r3 >> 29)) & 0x000000F8);
    r3 = r28;
    *(u8*)((u8*)r31 + 0x13) = tmp;
    fn_8011E850();
    r5 = r3 & 0xFF;
    tmp = *(u8*)((u8*)r31 + 0x13);
    r4 = -r5;
    r3 = r30;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000001) | (((r4 << 0) | (r4 >> 32)) & 0x00000001);
    *(u8*)((u8*)r31 + 0x13) = tmp;
    fn_80135A70();
    r27 = r3 & 0xFF;
    r3 = r28;
    fn_8011F4D8();
    r4 = r3;
    r5 = r27;
    r3 = r31 + 0x8;
    ((void(*)(void))fn_800F9AEC)();
    r5 = r3;
    if ((s32)r5 < 0xa) {
        r4 = r31 + r5;
        tmp = 0xff;
        r3 = r5 + 0x9;
        *(u8*)((u8*)r4 + 0x8) = tmp;
        r3 = r31 + r3;
        r5 = 0x9 - r5;
        r4 = 0x0;
        memset((void*)r3, (int)r4, (u32)r5);
    }
    r3 = r30;
    fn_80135A70();
    r30 = r3 & 0xFF;
    r3 = r28;
    fn_8011F508();
    r4 = r3;
    r5 = r30;
    r3 = r31 + 0x14;
    ((void(*)(void))fn_800F9AEC)();
    r5 = r3;
    if ((s32)r5 < 7) {
        r4 = r31 + r5;
        tmp = 0xff;
        r3 = r5 + 0x15;
        *(u8*)((u8*)r4 + 0x14) = tmp;
        r3 = r31 + r3;
        r5 = 0x6 - r5;
        r4 = 0x0;
        memset((void*)r3, (int)r4, (u32)r5);
    }
    r3 = r28;
    fn_8011E820();
    *(u8*)((u8*)r31 + 0x1B) = r3;
    r3 = r28;
    fn_8011E7F0();
    tmp = r3 & 0xFFFF;
    r3 = r28;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    tmp = tmp & 0xFFFF;
    *(u16*)((u8*)r31 + 0x1E) = tmp;
    fn_8011F5C8();
    tmp = r3 & 0xFFFF;
    r3 = r28;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    tmp = tmp & 0xFFFF;
    *(u16*)((u8*)r31 + 0x20) = tmp;
    fn_8011F1A0();
    tmp = r3 & 0xFFFF;
    r3 = r28;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    tmp = tmp & 0xFFFF;
    *(u16*)((u8*)r31 + 0x22) = tmp;
    fn_8011F4C0();
    tmp = r3 & 0x0000FF00;
    r5 = r3 & 0x00FF0000;
    r4 = r3 << 24;
    r6 = (u32)r3 >> 24;
    tmp = tmp << 8;
    r5 = (u32)r5 >> 8;
    tmp = r4 | tmp;
    r3 = r28;
    tmp = r5 | tmp;
    tmp = r6 | tmp;
    *(u32*)((u8*)r31 + 0x24) = tmp;
    fn_8011EE58();
    tmp = r3 & 0xFF;
    r3 = r28;
    *(u8*)((u8*)r31 + 0x29) = tmp;
    fn_8011E7D8();
    *(u16*)((u8*)r31 + 0x2A) = r3;
    r26 = 0x0;
    r27 = r31;
    r30 = 0x0;
    *(u8*)((u8*)r31 + 0x28) = r26;
    do {
        r3 = r28;
        r4 = r30 & 0xFFFF;
        fn_8011F228();
        tmp = r3 & 0xFFFF;
        r3 = r28;
        r5 = tmp << 8;
        r4 = r30 & 0xFFFF;
        tmp = (s32)tmp >> 8;
        tmp = r5 | tmp;
        tmp = tmp & 0xFFFF;
        *(u16*)((u8*)r27 + 0x2C) = tmp;
        fn_8011F1B8();
        r3 = r3 & 0xFF;
        tmp = *(u8*)((u8*)r31 + 0x28);
        r4 = r3 << r26;
        r3 = r28;
        tmp = tmp | r4;
        r4 = r30 & 0xFFFF;
        tmp = tmp & 0xFF;
        *(u8*)((u8*)r31 + 0x28) = tmp;
        fn_8011F1F0();
        tmp = r30 + 0x34;
        r27 = r27 + 0x2;
        *(u8*)(r31 + tmp) = r3;
        r26 = r26 + 0x2;
        r30 = r30 + 0x1;
    } while ((s32)r30 < 4);
    r3 = r28;
    fn_8011F054();
    tmp = r3 & 0xFF;
    r3 = r28;
    *(u8*)((u8*)r31 + 0x38) = tmp;
    fn_8011F028();
    tmp = r3 & 0xFF;
    r3 = r28;
    *(u8*)((u8*)r31 + 0x39) = tmp;
    fn_8011EFFC();
    tmp = r3 & 0xFF;
    r3 = r28;
    *(u8*)((u8*)r31 + 0x3A) = tmp;
    fn_8011EF78();
    tmp = r3 & 0xFF;
    r3 = r28;
    *(u8*)((u8*)r31 + 0x3B) = tmp;
    fn_8011EFD0();
    tmp = r3 & 0xFF;
    r3 = r28;
    *(u8*)((u8*)r31 + 0x3C) = tmp;
    fn_8011EFA4();
    tmp = r3 & 0xFF;
    r3 = r28;
    *(u8*)((u8*)r31 + 0x3D) = tmp;
    fn_8011ECEC();
    *(u8*)((u8*)r31 + 0x3E) = r3;
    r3 = r28;
    fn_8011ECC0();
    *(u8*)((u8*)r31 + 0x3F) = r3;
    r3 = r28;
    fn_8011EC94();
    *(u8*)((u8*)r31 + 0x40) = r3;
    r3 = r28;
    fn_8011EC68();
    *(u8*)((u8*)r31 + 0x41) = r3;
    r3 = r28;
    fn_8011EC3C();
    *(u8*)((u8*)r31 + 0x42) = r3;
    r3 = r28;
    fn_8011EB48();
    *(u8*)((u8*)r31 + 0x43) = r3;
    r3 = r28;
    fn_8011E8F4();
    *(u8*)(sp + 0x13) = r3;
    r3 = r28;
    fn_8011F580();
    tmp = r3 & 0xFF;
    r3 = r28;
    *(u8*)(sp + 0x12) = tmp;
    fn_8011F568();
    tmp = *(u8*)(sp + 0x11);
    tmp = (tmp & ~0x0000007F) | (((r3 << 0) | (r3 >> 32)) & 0x0000007F);
    r3 = r28;
    *(u8*)(sp + 0x11) = tmp;
    fn_8011F550();
    tmp = *(u8*)(sp + 0x10);
    tmp = (tmp & ~0x00000078) | (((r3 << 3) | (r3 >> 29)) & 0x00000078);
    r3 = r28;
    *(u8*)(sp + 0x10) = tmp;
    fn_8011F538();
    tmp = *(u8*)(sp + 0x10);
    tmp = (tmp & ~0x00000080) | (((r3 << 7) | (r3 >> 25)) & 0x00000080);
    r3 = r28;
    *(u8*)(sp + 0x10) = tmp;
    fn_8011EF4C();
    r3 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0xF);
    tmp = (tmp & ~0x0000001F) | (((r3 << 0) | (r3 >> 32)) & 0x0000001F);
    r3 = r28;
    *(u8*)(sp + 0xF) = tmp;
    fn_8011EF20();
    tmp = *(u16*)(sp + 0xE);
    tmp = (tmp & ~0x000003E0) | (((r3 << 5) | (r3 >> 27)) & 0x000003E0);
    r3 = r28;
    *(u16*)(sp + 0xE) = tmp;
    fn_8011EEF4();
    r3 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0xE);
    tmp = (tmp & ~0x0000007C) | (((r3 << 2) | (r3 >> 30)) & 0x0000007C);
    r3 = r28;
    *(u8*)(sp + 0xE) = tmp;
    fn_8011EE70();
    r3 = r3 & 0xFFFF;
    tmp = (tmp & ~0x000F8000) | (((r3 << 15) | (r3 >> 17)) & 0x000F8000);
    r3 = r28;
    *(u32*)(sp + 0xC) = tmp;
    fn_8011EEC8();
    tmp = *(u16*)(sp + 0xC);
    tmp = (tmp & ~0x000001F0) | (((r3 << 4) | (r3 >> 28)) & 0x000001F0);
    r3 = r28;
    *(u16*)(sp + 0xC) = tmp;
    fn_8011EE9C();
    r3 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0xC);
    tmp = (tmp & ~0x0000003E) | (((r3 << 1) | (r3 >> 31)) & 0x0000003E);
    r3 = r28;
    *(u8*)(sp + 0xC) = tmp;
    fn_8011E8DC();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0xC);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000040) | (((r4 << 6) | (r4 >> 26)) & 0x00000040);
    *(u8*)(sp + 0xC) = tmp;
    fn_8011E868();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0xC);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000080) | (((r4 << 7) | (r4 >> 25)) & 0x00000080);
    *(u8*)(sp + 0xC) = tmp;
    fn_8011EC10();
    tmp = *(u8*)(sp + 0xB);
    tmp = (tmp & ~0x00000007) | (((r3 << 0) | (r3 >> 32)) & 0x00000007);
    r3 = r28;
    *(u8*)(sp + 0xB) = tmp;
    fn_8011EBE4();
    tmp = *(u8*)(sp + 0xB);
    tmp = (tmp & ~0x00000038) | (((r3 << 3) | (r3 >> 29)) & 0x00000038);
    r3 = r28;
    *(u8*)(sp + 0xB) = tmp;
    fn_8011EBB8();
    r3 = r3 & 0xFF;
    tmp = *(u16*)(sp + 0xA);
    tmp = (tmp & ~0x000001C0) | (((r3 << 6) | (r3 >> 26)) & 0x000001C0);
    r3 = r28;
    *(u16*)(sp + 0xA) = tmp;
    fn_8011EB8C();
    tmp = *(u8*)(sp + 0xA);
    tmp = (tmp & ~0x0000000E) | (((r3 << 1) | (r3 >> 31)) & 0x0000000E);
    r3 = r28;
    *(u8*)(sp + 0xA) = tmp;
    fn_8011EB60();
    tmp = *(u8*)(sp + 0xA);
    tmp = (tmp & ~0x00000070) | (((r3 << 4) | (r3 >> 28)) & 0x00000070);
    r3 = r28;
    *(u8*)(sp + 0xA) = tmp;
    fn_8011EB1C();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0xA);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000080) | (((r4 << 7) | (r4 >> 25)) & 0x00000080);
    *(u8*)(sp + 0xA) = tmp;
    fn_8011EAF0();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0x9);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000001) | (((r4 << 0) | (r4 >> 32)) & 0x00000001);
    *(u8*)(sp + 0x9) = tmp;
    fn_8011EAC4();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0x9);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000002) | (((r4 << 1) | (r4 >> 31)) & 0x00000002);
    *(u8*)(sp + 0x9) = tmp;
    fn_8011EA98();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0x9);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000004) | (((r4 << 2) | (r4 >> 30)) & 0x00000004);
    *(u8*)(sp + 0x9) = tmp;
    fn_8011EA6C();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0x9);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000008) | (((r4 << 3) | (r4 >> 29)) & 0x00000008);
    *(u8*)(sp + 0x9) = tmp;
    fn_8011EA40();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0x9);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000010) | (((r4 << 4) | (r4 >> 28)) & 0x00000010);
    *(u8*)(sp + 0x9) = tmp;
    fn_8011EA14();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0x9);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000020) | (((r4 << 5) | (r4 >> 27)) & 0x00000020);
    *(u8*)(sp + 0x9) = tmp;
    fn_8011E9E8();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0x9);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000040) | (((r4 << 6) | (r4 >> 26)) & 0x00000040);
    *(u8*)(sp + 0x9) = tmp;
    fn_8011E9BC();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0x9);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000080) | (((r4 << 7) | (r4 >> 25)) & 0x00000080);
    *(u8*)(sp + 0x9) = tmp;
    fn_8011E990();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0x8);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000001) | (((r4 << 0) | (r4 >> 32)) & 0x00000001);
    *(u8*)(sp + 0x8) = tmp;
    fn_8011E964();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0x8);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000002) | (((r4 << 1) | (r4 >> 31)) & 0x00000002);
    *(u8*)(sp + 0x8) = tmp;
    fn_8011E938();
    r5 = r3 & 0xFF;
    tmp = *(u8*)(sp + 0x8);
    r4 = -r5;
    r3 = r28;
    r4 = r4 | r5;
    r4 = (u32)r4 >> 31;
    tmp = (tmp & ~0x00000004) | (((r4 << 2) | (r4 >> 30)) & 0x00000004);
    *(u8*)(sp + 0x8) = tmp;
    fn_8011E90C();
    tmp = *(u8*)(sp + 0x8);
    tmp = (tmp & ~0x00000078) | (((r3 << 3) | (r3 >> 29)) & 0x00000078);
    r3 = r28;
    *(u8*)(sp + 0x8) = tmp;
    fn_8011E7A4();
    tmp = *(u8*)(sp + 0x8);
    tmp = (tmp & ~0x00000080) | (((r3 << 7) | (r3 >> 25)) & 0x00000080);
    r3 = r28;
    r27 = 0x0;
    *(u8*)(sp + 0x8) = tmp;
    r4 = 0x4;
    fn_80121ADC();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r3 = r28;
        r4 = 0x4;
        fn_80121984();
        tmp = (s16)r3;
        tmp = tmp << 8;
        tmp = tmp | 0x80;
        r27 = tmp & 0xFFFF;

    } else {
        r3 = r28;
        r4 = 0x5;
        fn_80121ADC();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            tmp = r27 | 0x40;
            r27 = tmp & 0xFFFF;
        } else {
        r3 = r28;
        r4 = 0x7;
        fn_80121ADC();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            tmp = r27 | 0x20;
            r27 = tmp & 0xFFFF;
        } else {
        r3 = r28;
        r4 = 0x6;
        fn_80121ADC();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            tmp = r27 | 0x10;
            r27 = tmp & 0xFFFF;
        } else {
        r3 = r28;
        r4 = 0x3;
        fn_80121ADC();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            tmp = r27 | 0x8;
            r27 = tmp & 0xFFFF;

        } else {
            r3 = r28;
            r4 = 0x8;
            fn_80121ADC();
            tmp = r3 & 0xFF;
            if (tmp != 0) {
                r3 = r28;
                r4 = 0x8;
                fn_8012189C();
                tmp = (s8)r3;
                r27 = tmp & 0xFFFF;
            }
        }
        }
        }
        }
    }
    r3 = r28;
    r27 = r27 & 0xFFFF;
    fn_8011F45C();
    tmp = r27 & 0x0000FF00;
    r5 = r27 & 0x00FF0000;
    r4 = r27 << 24;
    /* clrrwi r6, r3, 12 */;
    tmp = tmp << 8;
    r3 = (u32)r5 >> 8;
    tmp = r4 | tmp;
    r4 = (u32)r27 >> 24;
    tmp = r3 | tmp;
    r3 = r28;
    tmp = r4 | tmp;
    tmp = tmp | r6;
    *(u32*)((u8*)r31 + 0x50) = tmp;
    fn_8011F4A8();
    *(u8*)((u8*)r31 + 0x54) = r3;
    r3 = r28;
    fn_8011E808();
    *(u8*)((u8*)r31 + 0x55) = r3;
    r3 = r28;
    fn_8011F15C();
    tmp = r3 & 0xFFFF;
    r3 = r28;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    tmp = tmp & 0xFFFF;
    *(u16*)((u8*)r31 + 0x58) = tmp;
    fn_8011F188();
    tmp = r3 & 0xFFFF;
    r3 = r28;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    tmp = tmp & 0xFFFF;
    *(u16*)((u8*)r31 + 0x56) = tmp;
    fn_8011F130();
    tmp = r3 & 0xFFFF;
    r3 = r28;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    tmp = tmp & 0xFFFF;
    *(u16*)((u8*)r31 + 0x5A) = tmp;
    fn_8011F104();
    tmp = r3 & 0xFFFF;
    r3 = r28;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    tmp = tmp & 0xFFFF;
    *(u16*)((u8*)r31 + 0x5C) = tmp;
    fn_8011F080();
    tmp = r3 & 0xFFFF;
    r3 = r28;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    tmp = tmp & 0xFFFF;
    *(u16*)((u8*)r31 + 0x5E) = tmp;
    fn_8011F0D8();
    tmp = r3 & 0xFFFF;
    r3 = r28;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    tmp = tmp & 0xFFFF;
    *(u16*)((u8*)r31 + 0x60) = tmp;
    fn_8011F0AC();
    tmp = r3 & 0xFFFF;
    r4 = r31 + 0x20;
    r7 = r11 & 0x0000FF00;
    r5 = tmp << 8;
    r3 = (s32)tmp >> 8;
    r10 = r11 & 0x00FF0000;
    r3 = r5 | r3;
    r27 = r3 & 0xFFFF;
    r5 = r12 & 0x0000FF00;
    r8 = r12 & 0x00FF0000;
    r3 = tmp & 0x0000FF00;
    r6 = tmp & 0x00FF0000;
    r9 = r11 << 24;
    r7 = r7 << 8;
    r10 = (u32)r10 >> 8;
    r9 = r9 | r7;
    *(u16*)((u8*)r31 + 0x62) = r27;
    r11 = (u32)r11 >> 24;
    r7 = r12 << 24;
    r9 = r10 | r9;
    r5 = r5 << 8;
    r9 = r11 | r9;
    r8 = (u32)r8 >> 8;
    r7 = r7 | r5;
    *(u32*)((u8*)r31 + 0x44) = r9;
    r9 = (u32)r12 >> 24;
    r5 = tmp << 24;
    r7 = r8 | r7;
    r3 = r3 << 8;
    r7 = r9 | r7;
    r6 = (u32)r6 >> 8;
    r5 = r5 | r3;
    *(u32*)((u8*)r31 + 0x48) = r7;
    r7 = (u32)tmp >> 24;
    r3 = r4;
    tmp = r6 | r5;
    tmp = r7 | tmp;
    *(u32*)((u8*)r31 + 0x4C) = tmp;
    tmp = 0x3;
    ctr_fn = (void(*)(void))tmp;
    do {
        r5 = *(u16*)((u8*)r3 + 0x0);
        r7 = *(u16*)((u8*)r3 + 0x2);
        tmp = (s32)r5 >> 8;
        r5 = r5 << 8;
        r8 = *(u16*)((u8*)r3 + 0x4);
        r6 = r5 | tmp;
        tmp = (s32)r7 >> 8;
        r5 = r7 << 8;
        r7 = *(u16*)((u8*)r3 + 0x6);
        r11 = r6 & 0xFFFF;
        r10 = r5 | tmp;
        tmp = (s32)r8 >> 8;
        r5 = r8 << 8;
        r8 = *(u16*)((u8*)r3 + 0x8);
        r6 = r5 | tmp;
        tmp = (s32)r7 >> 8;
        r5 = r7 << 8;
        r7 = *(u16*)((u8*)r3 + 0xA);
        r9 = r5 | tmp;
        tmp = (s32)r8 >> 8;
        r5 = r8 << 8;
        r26 = *(u16*)((u8*)r3 + 0xC);
        r12 = *(u16*)((u8*)r3 + 0xE);
        r8 = r5 | tmp;
        tmp = (s32)r7 >> 8;
        r5 = r7 << 8;
        r7 = r5 | tmp;
        tmp = (s32)r26 >> 8;
        r5 = r26 << 8;
        r29 = r29 + r11;
        r11 = r10 & 0xFFFF;
        r10 = r6 & 0xFFFF;
        r29 = r29 + r11;
        r6 = r5 | tmp;
        r29 = r29 + r10;
        tmp = r9 & 0xFFFF;
        r29 = r29 + tmp;
        tmp = r8 & 0xFFFF;
        r29 = r29 + tmp;
        r7 = r7 & 0xFFFF;
        r5 = r12 << 8;
        tmp = (s32)r12 >> 8;
        tmp = r5 | tmp;
        r29 = r29 + r7;
        r5 = r6 & 0xFFFF;
        r3 = r3 + 0x10;
        r29 = r29 + r5;
        tmp = tmp & 0xFFFF;
        r29 = r29 + tmp;
    } while (--ctr != 0);
    tmp = r29 & 0xFFFF;
    r3 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r3 | tmp;
    tmp = tmp & 0xFFFF;
    *(u16*)((u8*)r31 + 0x1C) = tmp;
    tmp = 0x2;
    ctr_fn = (void(*)(void))tmp;
    do {
        r3 = *(u32*)((u8*)r31 + 0x0);
        tmp = *(u32*)((u8*)r31 + 0x4);
        r5 = *(u32*)((u8*)r4 + 0x0);
        tmp = r3 ^ tmp;
        tmp = r5 ^ tmp;
        *(u32*)((u8*)r4 + 0x0) = tmp;
        r3 = *(u32*)((u8*)r31 + 0x0);
        tmp = *(u32*)((u8*)r31 + 0x4);
        r5 = *(u32*)((u8*)r4 + 0x4);
        tmp = r3 ^ tmp;
        tmp = r5 ^ tmp;
        *(u32*)((u8*)r4 + 0x4) = tmp;
        r3 = *(u32*)((u8*)r31 + 0x0);
        tmp = *(u32*)((u8*)r31 + 0x4);
        r5 = *(u32*)((u8*)r4 + 0x8);
        tmp = r3 ^ tmp;
        tmp = r5 ^ tmp;
        *(u32*)((u8*)r4 + 0x8) = tmp;
        r3 = *(u32*)((u8*)r31 + 0x0);
        tmp = *(u32*)((u8*)r31 + 0x4);
        r5 = *(u32*)((u8*)r4 + 0xC);
        tmp = r3 ^ tmp;
        tmp = r5 ^ tmp;
        *(u32*)((u8*)r4 + 0xC) = tmp;
        r3 = *(u32*)((u8*)r31 + 0x0);
        tmp = *(u32*)((u8*)r31 + 0x4);
        r5 = *(u32*)((u8*)r4 + 0x10);
        tmp = r3 ^ tmp;
        tmp = r5 ^ tmp;
        *(u32*)((u8*)r4 + 0x10) = tmp;
        r3 = *(u32*)((u8*)r31 + 0x0);
        tmp = *(u32*)((u8*)r31 + 0x4);
        r5 = *(u32*)((u8*)r4 + 0x14);
        tmp = r3 ^ tmp;
        tmp = r5 ^ tmp;
        *(u32*)((u8*)r4 + 0x14) = tmp;
        r4 = r4 + 0x18;
    } while (--ctr != 0);
    r6 = *(u32*)((u8*)r31 + 0x0);
    r3 = 0xAAAB0000;
    tmp = r6 & 0x0000FF00;
    r5 = r6 & 0x00FF0000;
    r4 = r6 << 24;
    r6 = (u32)r6 >> 24;
    tmp = tmp << 8;
    r5 = (u32)r5 >> 8;
    tmp = r4 | tmp;
    tmp = r5 | tmp;
    r27 = r6 | tmp;
    tmp = (u32)((u64)r3 * (u64)r27 >> 32);
    tmp = (u32)tmp >> 4;
    tmp = tmp * 0x18;
    r27 = r27 - tmp;
    tmp = (u32)((u64)r3 * (u64)r27 >> 32);
    tmp = (u32)tmp >> 2;
    if (tmp != 0) {
        r26 = tmp * 0xc;
        r3 = (u32)sp + 0x14;
        r5 = 0xc;
        r4 = r31 + r26;
        r4 = r4 + 0x20;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        r5 = r26;
        r3 = r31 + 0x2c;
        r4 = r31 + 0x20;
        ((void(*)(void))fn_800C8174)();
        r3 = r31 + 0x20;
        r4 = (u32)sp + 0x14;
        r5 = 0xc;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
    }
    r3 = 0xAAAB0000;
    tmp = (u32)((u64)tmp * (u64)r27 >> 32);
    tmp = (u32)tmp >> 2;
    tmp = tmp * 0x6;
    r27 = r27 - tmp;
    tmp = (u32)r27 >> 1;
    if (tmp != 0) {
        r26 = tmp * 0xc;
        r3 = (u32)sp + 0x14;
        r5 = 0xc;
        r4 = r31 + r26;
        r4 = r4 + 0x2c;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        r28 = r31 + 0x2c;
        r5 = r26;
        r4 = r28;
        r3 = r31 + 0x38;
        ((void(*)(void))fn_800C8174)();
        r3 = r28;
        r4 = (u32)sp + 0x14;
        r5 = 0xc;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
    }
    tmp = r27 & 0x1;
    if (tmp == 0) return;
    r26 = r31 + 0x44;
    r3 = (u32)sp + 0x14;
    r4 = r26;
    r5 = 0xc;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r27 = r31 + 0x38;
    r3 = r26;
    r4 = r27;
    r5 = 0xc;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r3 = r27;
    r4 = (u32)sp + 0x14;
    r5 = 0xc;
    memcpy((void*)r3, (const void*)r4, (u32)r5);

    return;
}

/* 0x8008BBDC | size: 0x9F8 */
void fn_8008BBDC(void) {
    extern void fn_8011D664();
    extern void fn_8011D688();
    extern void fn_8011D6AC();
    extern void fn_8011D6D0();
    extern void fn_8011D6F4();
    extern void fn_8011D718();
    extern void fn_8011D73C();
    extern void fn_8011D760();
    extern void fn_8011D770();
    extern void fn_8011D794();
    extern void fn_8011D7B8();
    extern void fn_8011D7DC();
    extern void fn_8011D800();
    extern void fn_8011D824();
    extern void fn_8011D848();
    extern void fn_8011D86C();
    extern void fn_8011D890();
    extern void fn_8011D8B4();
    extern void fn_8011D904();
    extern void fn_8011D924();
    extern void fn_8011D958();
    extern void fn_8011D98C();
    extern void fn_8011D9C0();
    extern void fn_8011D9F4();
    extern void fn_8011DA28();
    extern void fn_8011DA5C();
    extern void fn_8011DA90();
    extern void fn_8011DAC4();
    extern void fn_8011DAF8();
    extern void fn_8011DB2C();
    extern void fn_8011DB60();
    extern void fn_8011DB94();
    extern void fn_8011DBB8();
    extern void fn_8011DBDC();
    extern void fn_8011DC00();
    extern void fn_8011DC24();
    extern void fn_8011DC48();
    extern void fn_8011DC6C();
    extern void fn_8011DCB4();
    extern void fn_8011DCC4();
    extern void fn_8011DD80();
    extern void fn_8011DDFC();
    extern void fn_8011DE38();
    extern void fn_8011DE48();
    extern void fn_8011DE98();
    extern void fn_8011DEE4();
    extern void fn_8011DF90();
    extern void fn_8011DFA0();
    extern void fn_8011DFB0();
    extern void fn_8011DFC0();
    extern void fn_8011DFD0();
    extern void fn_8011DFE0();
    extern void fn_8011DFF0();
    extern void fn_8011F508();
    extern void fn_8011F598();
    extern void fn_8012173C();
    extern void fn_8012190C();
    extern void fn_801219F4();
    extern void fn_80124A60();
    extern void fn_801353C0();
    extern u8 jumptable_802EEC10[];
    extern u8 jumptable_802EEC30[];
    u8 sp[0x60];
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
    u32 r12 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r30 = r3;
    r31 = r4;
    fn_80124A60();
    r3 = *(u8*)((u8*)r31 + 0x13);
    /* extrwi tmp, r3, 1, 30 */;
    if (tmp == 0) {
        tmp = r3 & 0x1;
        if (tmp == 0) return;
    }
    r3 = r31 + 0x20;
    tmp = 0x2;
    ctr_fn = (void(*)(void))tmp;
    do {
        r4 = *(u32*)((u8*)r31 + 0x0);
        tmp = *(u32*)((u8*)r31 + 0x4);
        r5 = *(u32*)((u8*)r3 + 0x0);
        tmp = r4 ^ tmp;
        tmp = r5 ^ tmp;
        *(u32*)((u8*)r3 + 0x0) = tmp;
        r4 = *(u32*)((u8*)r31 + 0x0);
        tmp = *(u32*)((u8*)r31 + 0x4);
        r5 = *(u32*)((u8*)r3 + 0x4);
        tmp = r4 ^ tmp;
        tmp = r5 ^ tmp;
        *(u32*)((u8*)r3 + 0x4) = tmp;
        r4 = *(u32*)((u8*)r31 + 0x0);
        tmp = *(u32*)((u8*)r31 + 0x4);
        r5 = *(u32*)((u8*)r3 + 0x8);
        tmp = r4 ^ tmp;
        tmp = r5 ^ tmp;
        *(u32*)((u8*)r3 + 0x8) = tmp;
        r4 = *(u32*)((u8*)r31 + 0x0);
        tmp = *(u32*)((u8*)r31 + 0x4);
        r5 = *(u32*)((u8*)r3 + 0xC);
        tmp = r4 ^ tmp;
        tmp = r5 ^ tmp;
        *(u32*)((u8*)r3 + 0xC) = tmp;
        r4 = *(u32*)((u8*)r31 + 0x0);
        tmp = *(u32*)((u8*)r31 + 0x4);
        r5 = *(u32*)((u8*)r3 + 0x10);
        tmp = r4 ^ tmp;
        tmp = r5 ^ tmp;
        *(u32*)((u8*)r3 + 0x10) = tmp;
        r4 = *(u32*)((u8*)r31 + 0x0);
        tmp = *(u32*)((u8*)r31 + 0x4);
        r5 = *(u32*)((u8*)r3 + 0x14);
        tmp = r4 ^ tmp;
        tmp = r5 ^ tmp;
        *(u32*)((u8*)r3 + 0x14) = tmp;
        r3 = r3 + 0x18;
    } while (--ctr != 0);
    r5 = *(u32*)((u8*)r31 + 0x0);
    tmp = r5 & 0x0000FF00;
    r4 = r5 & 0x00FF0000;
    r3 = r5 << 24;
    r5 = (u32)r5 >> 24;
    tmp = tmp << 8;
    r4 = (u32)r4 >> 8;
    tmp = r3 | tmp;
    tmp = r4 | tmp;
    r25 = r5 | tmp;
    tmp = r25 & 0x1;
    if (tmp != 0) {
        r26 = r31 + 0x38;
        r3 = (u32)sp + 0x14;
        r4 = r26;
        r5 = 0xc;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        r27 = r31 + 0x44;
        r3 = r26;
        r4 = r27;
        r5 = 0xc;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        r3 = r27;
        r4 = (u32)sp + 0x14;
        r5 = 0xc;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
    }
    r3 = 0xAAAB0000;
    r4 = (u32)r25 >> 1;
    tmp = (u32)((u64)tmp * (u64)r4 >> 32);
    tmp = (u32)tmp >> 1;
    tmp = tmp * 0x3;
    r26 = r4 - tmp;
    if (r26 != 0) {
        r27 = r31 + 0x2c;
        r3 = (u32)sp + 0x14;
        r4 = r27;
        r5 = 0xc;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        r26 = r26 * 0xc;
        r3 = r27;
        r4 = r31 + 0x38;
        r5 = r26;
        ((void(*)(void))fn_800C8174)();
        r3 = r31 + r26;
        r4 = (u32)sp + 0x14;
        r3 = r3 + 0x2c;
        r5 = 0xc;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
    }
    r3 = 0xAAAB0000;
    tmp = (u32)((u64)tmp * (u64)r25 >> 32);
    tmp = (u32)tmp >> 2;
    r25 = tmp & 0x3;
    if (r25 != 0) {
        r3 = (u32)sp + 0x14;
        r4 = r31 + 0x20;
        r5 = 0xc;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        r25 = r25 * 0xc;
        r3 = r31 + 0x20;
        r4 = r31 + 0x2c;
        r5 = r25;
        ((void(*)(void))fn_800C8174)();
        r3 = r31 + r25;
        r4 = (u32)sp + 0x14;
        r3 = r3 + 0x20;
        r5 = 0xc;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
    }
    r25 = *(u32*)((u8*)r31 + 0x44);
    r3 = r30;
    tmp = *(u32*)((u8*)r31 + 0x48);
    r4 = *(u32*)((u8*)r31 + 0x4C);
    r11 = r25 & 0x0000FF00;
    r5 = *(u32*)((u8*)r31 + 0x0);
    r9 = tmp & 0x0000FF00;
    r8 = r4 & 0x0000FF00;
    r26 = r25 & 0x00FF0000;
    r29 = tmp & 0x00FF0000;
    r10 = r4 & 0x00FF0000;
    r6 = r5 & 0x0000FF00;
    r7 = r5 & 0x00FF0000;
    r27 = r25 << 24;
    r28 = r11 << 8;
    r12 = tmp << 24;
    r11 = r9 << 8;
    r9 = r4 << 24;
    r8 = r8 << 8;
    r26 = (u32)r26 >> 8;
    r28 = r27 | r28;
    r29 = (u32)r29 >> 8;
    r11 = r12 | r11;
    r10 = (u32)r10 >> 8;
    r8 = r9 | r8;
    r12 = (u32)r25 >> 24;
    r9 = r26 | r28;
    r28 = r12 | r9;
    r12 = (u32)tmp >> 24;
    r11 = r29 | r11;
    r9 = (u32)r4 >> 24;
    r8 = r10 | r8;
    r4 = r5 << 24;
    tmp = r6 << 8;
    r10 = r12 | r11;
    r8 = r9 | r8;
    r6 = (u32)r7 >> 8;
    tmp = r4 | tmp;
    r4 = (u32)r5 >> 24;
    tmp = r6 | tmp;
    r4 = r4 | tmp;
    fn_8011DFE0();
    r6 = *(u32*)((u8*)r31 + 0x4);
    r3 = r30;
    tmp = r6 & 0x0000FF00;
    r5 = r6 & 0x00FF0000;
    r4 = r6 << 24;
    r6 = (u32)r6 >> 24;
    tmp = tmp << 8;
    r5 = (u32)r5 >> 8;
    tmp = r4 | tmp;
    tmp = r5 | tmp;
    r4 = r6 | tmp;
    fn_8011DF90();
    tmp = *(u16*)(sp + 0x10);
    /* extrwi tmp, tmp, 4, 21 */;
    do {
        if (tmp <= 0xf) {
            r3 = (u32)jumptable_802EEC30;
            tmp = tmp << 2;
            r3 = (u32)jumptable_802EEC30;
            tmp = *(u32*)(r3 + tmp);
            ctr_fn = (void(*)(void))tmp;
            r25 = 0x8;
            break;
            r25 = 0x9;
            break;
            r25 = 0xa;
            break;
            r25 = 0x1;
            break;
            r25 = 0x2;
            break;
            r25 = 0xb;
            break;
        }
        r25 = 0x0;
    } while (0);

    tmp = *(u8*)((u8*)r31 + 0x12);
    do {
        if (tmp <= 7) {
            r3 = (u32)jumptable_802EEC10;
            tmp = tmp << 2;
            r3 = (u32)jumptable_802EEC10;
            tmp = *(u32*)(r3 + tmp);
            ctr_fn = (void(*)(void))tmp;
            r26 = 0x1;
            r27 = 0x1;
            break;
            r26 = 0x2;
            r27 = 0x2;
            break;
            r26 = 0x3;
            r27 = 0x4;
            break;
            r26 = 0x3;
            r27 = 0x5;
            break;
            r26 = 0x3;
            r27 = 0x3;
            break;
            r26 = 0x3;
            r27 = 0x6;
            break;
        }
        r26 = 0x0;
        r27 = 0x0;
    } while (0);

    r3 = r30;
    fn_8011F598();
    r4 = r25;
    r6 = r26;
    r7 = r27;
    r5 = 0x3;
    fn_801353C0();
    r25 = r27 & 0xFF;
    r3 = (u32)sp + 0x20;
    r6 = r25;
    r4 = r31 + 0x8;
    r5 = 0xa;
    ((void(*)(void))fn_800F9C04)();
    r3 = r30;
    r4 = (u32)sp + 0x20;
    fn_8011DEE4();
    tmp = *(u8*)((u8*)r31 + 0x13);
    r3 = r30;
    /* extrwi r4, tmp, 5, 24 */;
    ((void(*)(void))fn_8011D4E4)();
    tmp = *(u8*)((u8*)r31 + 0x13);
    r3 = r30;
    r4 = tmp & 0x1;
    ((void(*)(void))fn_8011D4F4)();
    r3 = r30;
    fn_8011F508();
    r6 = r25;
    r4 = r31 + 0x14;
    r5 = 0x7;
    ((void(*)(void))fn_800F9C04)();
    r4 = *(u8*)((u8*)r31 + 0x1B);
    r3 = r30;
    ((void(*)(void))fn_8011D4D4)();
    tmp = *(u16*)((u8*)r31 + 0x1E);
    r3 = r30;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    r4 = tmp & 0xFFFF;
    ((void(*)(void))fn_8011D4B4)();
    tmp = *(u16*)((u8*)r31 + 0x20);
    r3 = r30;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    r4 = tmp & 0xFFFF;
    fn_8011DFF0();
    tmp = *(u16*)((u8*)r31 + 0x22);
    r3 = r30;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    r4 = tmp & 0xFFFF;
    fn_8011DCB4();
    r6 = *(u32*)((u8*)r31 + 0x24);
    r3 = r30;
    tmp = r6 & 0x0000FF00;
    r5 = r6 & 0x00FF0000;
    r4 = r6 << 24;
    r6 = (u32)r6 >> 24;
    tmp = tmp << 8;
    r5 = (u32)r5 >> 8;
    tmp = r4 | tmp;
    tmp = r5 | tmp;
    r4 = r6 | tmp;
    fn_8011DE98();
    r4 = *(u8*)((u8*)r31 + 0x29);
    r3 = r30;
    fn_8011D904();
    r4 = *(u16*)((u8*)r31 + 0x2A);
    r3 = r30;
    ((void(*)(void))fn_8011D4A4)();
    r27 = 0x0;
    r25 = r31;
    r26 = r27;
    do {
        tmp = *(u16*)((u8*)r25 + 0x2C);
        r3 = r30;
        r4 = r27 & 0xFFFF;
        r5 = tmp << 8;
        tmp = (s32)tmp >> 8;
        tmp = r5 | tmp;
        r5 = tmp & 0xFFFF;
        fn_8011DDFC();
        tmp = *(u8*)((u8*)r31 + 0x28);
        r3 = r30;
        r4 = r27 & 0xFFFF;
        tmp = (s32)tmp >> r26;
        r5 = tmp & 0x3;
        fn_8011DCC4();
        tmp = r27 + 0x34;
        r3 = r30;
        r5 = *(u8*)(r31 + tmp);
        r4 = r27 & 0xFFFF;
        fn_8011DD80();
        r25 = r25 + 0x2;
        r26 = r26 + 0x2;
        r27 = r27 + 0x1;
    } while ((s32)r27 < 4);
    r4 = *(u8*)((u8*)r31 + 0x38);
    r3 = r30;
    fn_8011DB60();
    r4 = *(u8*)((u8*)r31 + 0x39);
    r3 = r30;
    fn_8011DB2C();
    r4 = *(u8*)((u8*)r31 + 0x3A);
    r3 = r30;
    fn_8011DAF8();
    r4 = *(u8*)((u8*)r31 + 0x3B);
    r3 = r30;
    fn_8011DA5C();
    r4 = *(u8*)((u8*)r31 + 0x3C);
    r3 = r30;
    fn_8011DAC4();
    r4 = *(u8*)((u8*)r31 + 0x3D);
    r3 = r30;
    fn_8011DA90();
    r4 = *(u8*)((u8*)r31 + 0x3E);
    r3 = r30;
    fn_8011D8B4();
    r4 = *(u8*)((u8*)r31 + 0x3F);
    r3 = r30;
    fn_8011D890();
    r4 = *(u8*)((u8*)r31 + 0x40);
    r3 = r30;
    fn_8011D86C();
    r4 = *(u8*)((u8*)r31 + 0x41);
    r3 = r30;
    fn_8011D848();
    r4 = *(u8*)((u8*)r31 + 0x42);
    r3 = r30;
    fn_8011D824();
    r4 = *(u8*)((u8*)r31 + 0x43);
    r3 = r30;
    fn_8011D760();
    r4 = *(u8*)(sp + 0x13);
    r3 = r30;
    ((void(*)(void))fn_8011D57C)();
    r4 = *(u8*)(sp + 0x12);
    r3 = r30;
    fn_8011DFD0();
    tmp = *(u8*)(sp + 0x11);
    r3 = r30;
    r4 = tmp & 0x7F;
    fn_8011DFC0();
    tmp = *(u8*)(sp + 0x10);
    r3 = r30;
    /* extrwi r4, tmp, 4, 25 */;
    fn_8011DFB0();
    tmp = *(u8*)(sp + 0x10);
    r3 = r30;
    /* extrwi r4, tmp, 1, 24 */;
    fn_8011DFA0();
    tmp = *(u8*)(sp + 0xF);
    r3 = r30;
    r4 = tmp & 0x1F;
    fn_8011DA28();
    tmp = *(u16*)(sp + 0xE);
    r3 = r30;
    /* extrwi r4, tmp, 5, 22 */;
    fn_8011D9F4();
    tmp = *(u8*)(sp + 0xE);
    r3 = r30;
    /* extrwi r4, tmp, 5, 25 */;
    fn_8011D9C0();
    r3 = r30;
    /* extrwi r4, tmp, 5, 12 */;
    fn_8011D924();
    tmp = *(u16*)(sp + 0xC);
    r3 = r30;
    /* extrwi r4, tmp, 5, 23 */;
    fn_8011D98C();
    tmp = *(u8*)(sp + 0xC);
    r3 = r30;
    /* extrwi r4, tmp, 5, 26 */;
    fn_8011D958();
    tmp = *(u8*)(sp + 0xC);
    r3 = r30;
    /* extrwi r4, tmp, 1, 25 */;
    ((void(*)(void))fn_8011D56C)();
    tmp = *(u8*)(sp + 0xC);
    r3 = r30;
    /* extrwi r4, tmp, 1, 24 */;
    ((void(*)(void))fn_8011D504)();
    tmp = *(u8*)(sp + 0xB);
    r3 = r30;
    r4 = tmp & 0x7;
    fn_8011D800();
    tmp = *(u8*)(sp + 0xB);
    r3 = r30;
    /* extrwi r4, tmp, 3, 26 */;
    fn_8011D7DC();
    tmp = *(u16*)(sp + 0xA);
    r3 = r30;
    /* extrwi r4, tmp, 3, 23 */;
    fn_8011D7B8();
    tmp = *(u8*)(sp + 0xA);
    r3 = r30;
    /* extrwi r4, tmp, 3, 28 */;
    fn_8011D794();
    tmp = *(u8*)(sp + 0xA);
    r3 = r30;
    /* extrwi r4, tmp, 3, 25 */;
    fn_8011D770();
    tmp = *(u8*)(sp + 0xA);
    r3 = r30;
    /* extrwi r4, tmp, 1, 24 */;
    fn_8011D73C();
    tmp = *(u8*)(sp + 0x9);
    r3 = r30;
    r4 = tmp & 0x1;
    fn_8011D718();
    tmp = *(u8*)(sp + 0x9);
    r3 = r30;
    /* extrwi r4, tmp, 1, 30 */;
    fn_8011D6F4();
    tmp = *(u8*)(sp + 0x9);
    r3 = r30;
    /* extrwi r4, tmp, 1, 29 */;
    fn_8011D6D0();
    tmp = *(u8*)(sp + 0x9);
    r3 = r30;
    /* extrwi r4, tmp, 1, 28 */;
    fn_8011D6AC();
    tmp = *(u8*)(sp + 0x9);
    r3 = r30;
    /* extrwi r4, tmp, 1, 27 */;
    fn_8011D688();
    tmp = *(u8*)(sp + 0x9);
    r3 = r30;
    /* extrwi r4, tmp, 1, 26 */;
    fn_8011D664();
    tmp = *(u8*)(sp + 0x9);
    r3 = r30;
    /* extrwi r4, tmp, 1, 25 */;
    ((void(*)(void))fn_8011D640)();
    tmp = *(u8*)(sp + 0x9);
    r3 = r30;
    /* extrwi r4, tmp, 1, 24 */;
    ((void(*)(void))fn_8011D61C)();
    tmp = *(u8*)(sp + 0x8);
    r3 = r30;
    r4 = tmp & 0x1;
    ((void(*)(void))fn_8011D5F8)();
    tmp = *(u8*)(sp + 0x8);
    r3 = r30;
    /* extrwi r4, tmp, 1, 30 */;
    ((void(*)(void))fn_8011D5D4)();
    tmp = *(u8*)(sp + 0x8);
    r3 = r30;
    /* extrwi r4, tmp, 1, 29 */;
    ((void(*)(void))fn_8011D5B0)();
    tmp = *(u8*)(sp + 0x8);
    r3 = r30;
    /* extrwi r4, tmp, 4, 25 */;
    ((void(*)(void))fn_8011D58C)();
    tmp = *(u8*)(sp + 0x8);
    r3 = r30;
    /* extrwi r4, tmp, 1, 24 */;
    ((void(*)(void))fn_8011D480)();
    r5 = *(u32*)((u8*)r31 + 0x50);
    tmp = r5 & 0x0000FF00;
    r4 = r5 & 0x00FF0000;
    r3 = r5 << 24;
    r5 = (u32)r5 >> 24;
    tmp = tmp << 8;
    r4 = (u32)r4 >> 8;
    tmp = r3 | tmp;
    tmp = r4 | tmp;
    tmp = r5 | tmp;
    r29 = tmp & 0xFFFF;
    tmp = r29 & 0x00000080;
    if ((s32)tmp != 0) {
        r3 = r30;
        r4 = 0x4;
        r5 = 0x0;
        fn_801219F4();
        tmp = r29 & 0x00000F00;
        r3 = r30;
        tmp = (s32)tmp >> 8;
        r4 = 0x4;
        r5 = (s16)tmp;
        fn_8012190C();

    } else {
        tmp = r29 & 0x00000040;
        if ((s32)tmp != 0) {
            r3 = r30;
            r4 = 0x5;
            r5 = 0x0;
            fn_801219F4();
        } else {
        tmp = r29 & 0x00000020;
        if ((s32)tmp != 0) {
            r3 = r30;
            r4 = 0x7;
            r5 = 0x0;
            fn_801219F4();
        } else {
        tmp = r29 & 0x00000010;
        if ((s32)tmp != 0) {
            r3 = r30;
            r4 = 0x6;
            r5 = 0x0;
            fn_801219F4();
        } else {
        tmp = r29 & 0x00000008;
        if ((s32)tmp != 0) {
            r3 = r30;
            r4 = 0x3;
            r5 = 0x0;
            fn_801219F4();

        } else {
            r25 = r29 & 0x7;
            if ((s32)r25 != 0) {
                r3 = r30;
                r4 = 0x8;
                r5 = 0x0;
                fn_801219F4();
                r3 = r30;
                r5 = (s8)r25;
                r4 = 0x8;
                fn_8012173C();
            }
        }
        }
        }
        }
    }
    r6 = *(u32*)((u8*)r31 + 0x50);
    r3 = r30;
    tmp = r6 & 0x0000FF00;
    r5 = r6 & 0x00FF0000;
    r4 = r6 << 24;
    r6 = (u32)r6 >> 24;
    tmp = tmp << 8;
    r5 = (u32)r5 >> 8;
    tmp = r4 | tmp;
    tmp = r5 | tmp;
    tmp = r6 | tmp;
    /* clrrwi r4, tmp, 12 */;
    fn_8011DE38();
    r4 = *(u8*)((u8*)r31 + 0x54);
    r3 = r30;
    fn_8011DE48();
    r4 = *(u8*)((u8*)r31 + 0x55);
    r3 = r30;
    ((void(*)(void))fn_8011D4C4)();
    tmp = *(u16*)((u8*)r31 + 0x58);
    r3 = r30;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    r4 = tmp & 0xFFFF;
    fn_8011DC48();
    tmp = *(u16*)((u8*)r31 + 0x56);
    r3 = r30;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    r4 = tmp & 0xFFFF;
    fn_8011DC6C();
    tmp = *(u16*)((u8*)r31 + 0x5A);
    r3 = r30;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    r4 = tmp & 0xFFFF;
    fn_8011DC24();
    tmp = *(u16*)((u8*)r31 + 0x5C);
    r3 = r30;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    r4 = tmp & 0xFFFF;
    fn_8011DC00();
    tmp = *(u16*)((u8*)r31 + 0x5E);
    r3 = r30;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    r4 = tmp & 0xFFFF;
    fn_8011DB94();
    tmp = *(u16*)((u8*)r31 + 0x60);
    r3 = r30;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    r4 = tmp & 0xFFFF;
    fn_8011DBDC();
    tmp = *(u16*)((u8*)r31 + 0x62);
    r3 = r30;
    r4 = tmp << 8;
    tmp = (s32)tmp >> 8;
    tmp = r4 | tmp;
    r4 = tmp & 0xFFFF;
    fn_8011DBB8();

    return;
}

/* 0x8008C5D4 | size: 0x128 */
void fn_8008C5D4(void) {
    extern void fn_8012189C();
    extern void fn_80121984();
    extern void fn_80121ADC();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    r31 = 0x0;
    r4 = 0x4;
    fn_80121ADC();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        r3 = r30;
        r4 = 0x4;
        fn_80121984();
        tmp = (s16)r3;
        tmp = tmp << 8;
        tmp = tmp | 0x80;
        r31 = tmp & 0xFFFF;
        r3 = r31;
        return;
    }
    r3 = r30;
    r4 = 0x5;
    fn_80121ADC();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        tmp = r31 | 0x40;
        r31 = tmp & 0xFFFF;
        r3 = r31;
        return;
    }
    r3 = r30;
    r4 = 0x7;
    fn_80121ADC();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        tmp = r31 | 0x20;
        r31 = tmp & 0xFFFF;
        r3 = r31;
        return;
    }
    r3 = r30;
    r4 = 0x6;
    fn_80121ADC();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        tmp = r31 | 0x10;
        r31 = tmp & 0xFFFF;
        r3 = r31;
        return;
    }
    r3 = r30;
    r4 = 0x3;
    fn_80121ADC();
    tmp = r3 & 0xFF;
    if (tmp != 0) {
        tmp = r31 | 0x8;
        r31 = tmp & 0xFFFF;
        r3 = r31;
        return;
    }
    r3 = r30;
    r4 = 0x8;
    fn_80121ADC();
    tmp = r3 & 0xFF;
    if (tmp == 0) { r3 = r31; return; }
    r3 = r30;
    r4 = 0x8;
    fn_8012189C();
    tmp = (s8)r3;
    r31 = tmp & 0xFFFF;

    r3 = r31;
    return;
}

/* 0x8008C6FC | size: 0x4 */
void fn_8008C6FC(void) {
}

/* 0x8008C700 | size: 0x8C */
#pragma push
#pragma peephole off
void GbaMisc_RunFlagDispatch(void) {
    extern s32 fn_80113F48(void);
    extern s32 fn_801906A0(s32);
    extern void fn_8019075C(s32, s32);
    s32 arg;
    u32 state;
    u32 offset;
    void (*handler)(s32);
    s32 nextState;

    *(u32*)&lbl_8047A694 = 0;
    *(u32*)((u8*)&lbl_8047A694 + 0x4) = 0;
    *(u32*)&lbl_8047A690 = 0;
    arg = fn_80113F48();
    state = fn_801906A0(0xb5d);
    offset = state << 2;
    handler = *(void (**)(s32))((u8*)lbl_802EEC70 + offset);
    handler(arg);
    nextState = state + 1;
    if ((u32)nextState >= 0x1f) {
        nextState = 0;
    }
    fn_8019075C(0xb5d, nextState);
}
#pragma pop

/* 0x8008C78C | size: 0x24 */
#pragma push
#pragma scheduling off
s32 fn_8008C78C(void) {
    extern s32 fn_801906A0(s32);
    return fn_801906A0(0xb5d);
}
#pragma pop

/* 0x8008C7B0 | size: 0x31C */
void fn_8008C7B0(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_80190528();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x30];
    u32 tmp = 0;
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))fn_800ECA78)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r29;
    ((void(*)(void))fn_800EC990)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0xB720000;
    r28 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r27 = tmp;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r4 = 0xD040000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xD0D0000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    r4 = 0xD0D0000;
    r29 = r3;
    r3 = r4 + 0x1001;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r30 = tmp;
    r4 = r28;
    r5 = r31;
    r6 = r29;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r30;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r27;
    r4 = 0x6;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x8d0;
    fn_80190528();
    r3 = 0x1;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008CACC | size: 0x30C */
void fn_8008CACC(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))fn_800ECA78)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r29;
    ((void(*)(void))fn_800EC990)();
    r4 = 0x11210000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0xCE60000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0xCE60000;
    r4 = 0x2;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r30 = 0x64;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1DC;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0x6BC0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xD020000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xD0C0000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008CDD8 | size: 0x2C8 */
void fn_8008CDD8(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB708();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))fn_800ECA78)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r29;
    ((void(*)(void))fn_800EC990)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r4 = 0xD010000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xD0B0000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r28;
    r4 = 0x1;
    fn_801CB708();
    r3 = r28;
    r4 = 0xc;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008D0A0 | size: 0x2A8 */
void fn_8008D0A0(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))fn_800ECA78)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r29;
    ((void(*)(void))fn_800EC990)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r4 = 0xD000000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xD0A0000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008D348 | size: 0x5F0 */
void fn_8008D348(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x50];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r22 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r22 = r3;
        if (r22 < 1) {
            r22 = 0x1;
    }
    }
    r21 = 0x0;
    while (r21 < r22) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r21 = r21 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r21 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r21 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r21 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r21 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r21;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r21;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r21;
    ((void(*)(void))fn_800ECA78)();
    r3 = r21;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r21;
    ((void(*)(void))fn_800EC990)();
    r4 = 0x11200000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0xCE60000;
    r21 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r21;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BC0000;
    r30 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r24 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r25 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r26 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r27 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r29 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r30;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r24;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r25;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r26;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r29;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r21 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r21;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r21;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r21 = r3;
    r3 = r31;
    r4 = r30;
    ((void(*)(void))fn_800F9318)();
    r4 = r21;
    ((void(*)(void))fn_800E3C08)();
    r3 = r31;
    r4 = r30;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r4 = 0xCFF0000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r22 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r22 = r3;
        if (r22 < 1) {
            r22 = 0x1;
    }
    }
    r21 = 0x0;
    while (r21 < r22) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r21 = r21 + r3;

    }
    r3 = 0xD090000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    r4 = 0xD090000;
    r23 = r3;
    r3 = r4 + 0x1001;
    fn_801CBA0C();
    r4 = 0xD090000;
    r22 = r3;
    r3 = r4 + 0x1006;
    fn_801CBA0C();
    r4 = 0xD090000;
    r21 = r3;
    r3 = r4 + 0x1002;
    fn_801CBA0C();
    r4 = 0xD090000;
    r20 = r3;
    r3 = r4 + 0x1003;
    fn_801CBA0C();
    r4 = 0xD090000;
    r19 = r3;
    r3 = r4 + 0x1004;
    fn_801CBA0C();
    r4 = 0xD090000;
    r18 = r3;
    r3 = r4 + 0x1005;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r17 = tmp;
    r4 = r30;
    r5 = r31;
    r6 = r23;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r24;
    r5 = r31;
    r6 = r22;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r25;
    r5 = r31;
    r6 = r21;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r26;
    r5 = r31;
    r6 = r20;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r19;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r29;
    r5 = r31;
    r6 = r18;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r28;
    r5 = r31;
    r6 = r17;
    r7 = 0x0;
    fn_801845E4();
    r3 = 0xCE60000;
    r4 = 0x1;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r30;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r24;
    r4 = 0x7;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r25;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r26;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r27;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r29;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r28;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008D938 | size: 0x9E8 */
void fn_8008D938(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB708();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r14 = 0;
    u32 r15 = 0;
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r15 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r15;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r16 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r16 = r3;
        if (r16 < 1) {
            r16 = 0x1;
    }
    }
    r14 = 0x0;
    while (r14 < r16) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r14 = r14 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r15;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r14 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r14 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r14 + 0x144) = tmp;
    r3 = r15;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r14 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r14;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r14;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r14;
    ((void(*)(void))fn_800ECA78)();
    r3 = r14;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r14;
    ((void(*)(void))fn_800EC990)();
    r4 = 0x111B0000;
    r3 = r15;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0xCE60000;
    r14 = r3;
    r3 = r15;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r14;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0xCE60000;
    r3 = r15;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r16 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))fn_800C46B0)();
        r16 = r3;
        if (r16 < 1) {
            r16 = 0x1;
    }
    }
    r14 = 0x0;
    while (r14 < r16) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r14 = r14 + r3;

    }
    r4 = 0x111F0000;
    r3 = r15;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0xCE60000;
    r14 = r3;
    r3 = r15;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r14;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0xCE60000;
    r3 = r15;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0x6BC0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r31 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD290000;
    r30 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r29 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r28 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD290000;
    r27 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r26 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r25 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r24 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r15;
    r23 = tmp;
    r4 = r31;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r30;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r29;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r27;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r26;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r25;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r24;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r23;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xCFE0000;
    r3 = r15;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r16 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r16 = r3;
        if (r16 < 1) {
            r16 = 0x1;
    }
    }
    r14 = 0x0;
    while (r14 < r16) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r14 = r14 + r3;

    }
    r3 = 0xD080000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    r4 = 0xD080000;
    r3 = r4 + 0x1008;
    fn_801CBA0C();
    r4 = 0xD080000;
    r14 = r3;
    r3 = r4 + 0x1001;
    fn_801CBA0C();
    r4 = 0xD080000;
    r22 = r3;
    r3 = r4 + 0x1002;
    fn_801CBA0C();
    r4 = 0xD080000;
    r21 = r3;
    r3 = r4 + 0x1003;
    fn_801CBA0C();
    r4 = 0xD080000;
    r20 = r3;
    r3 = r4 + 0x1004;
    fn_801CBA0C();
    r4 = 0xD080000;
    r19 = r3;
    r3 = r4 + 0x1005;
    fn_801CBA0C();
    r4 = 0xD080000;
    r18 = r3;
    r3 = r4 + 0x1006;
    fn_801CBA0C();
    r4 = 0xD080000;
    r17 = r3;
    r3 = r4 + 0x1007;
    fn_801CBA0C();
    tmp = r3;
    r3 = r15;
    r4 = r31;
    r16 = tmp;
    r5 = r15;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r30;
    r5 = r15;
    r6 = r14;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r29;
    r5 = r15;
    r6 = r22;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r28;
    r5 = r15;
    r6 = r21;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r27;
    r5 = r15;
    r6 = r20;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r26;
    r5 = r15;
    r6 = r19;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r25;
    r5 = r15;
    r6 = r18;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r24;
    r5 = r15;
    r6 = r17;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r23;
    r5 = r15;
    r6 = r16;
    r7 = 0x0;
    fn_801845E4();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r31;
    r4 = 0x6;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r30;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r29;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r28;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r27;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r26;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r25;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r24;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r24;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r23;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r31;
    r4 = 0x1;
    fn_801CB708();
    r3 = r30;
    r4 = 0x1;
    fn_801CB708();
    r3 = r29;
    r4 = 0x1;
    fn_801CB708();
    r3 = r28;
    r4 = 0x1;
    fn_801CB708();
    r3 = r27;
    r4 = 0x1;
    fn_801CB708();
    r3 = r26;
    r4 = 0x1;
    fn_801CB708();
    r3 = r25;
    r4 = 0x1;
    fn_801CB708();
    r3 = r24;
    r4 = 0x1;
    fn_801CB708();
    r3 = r24;
    r4 = 0x1;
    fn_801CB708();
    r3 = r23;
    r4 = 0x1;
    fn_801CB708();
    r3 = r31;
    r4 = 0x7;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r30;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r29;
    r4 = 0xc;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r28;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r26;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r27;
    r4 = 0xc;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r25;
    r4 = 0xc;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r24;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r24;
    r4 = 0xc;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r23;
    r4 = 0xc;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r31;
    r4 = 0x1;
    fn_801CB708();
    r3 = r30;
    r4 = 0x1;
    fn_801CB708();
    r3 = r29;
    r4 = 0x1;
    fn_801CB708();
    r3 = r28;
    r4 = 0x1;
    fn_801CB708();
    r3 = r27;
    r4 = 0x1;
    fn_801CB708();
    r3 = r26;
    r4 = 0x1;
    fn_801CB708();
    r3 = r25;
    r4 = 0x1;
    fn_801CB708();
    r3 = r24;
    r4 = 0x1;
    fn_801CB708();
    r3 = r24;
    r4 = 0x1;
    fn_801CB708();
    r3 = r23;
    r4 = 0x1;
    fn_801CB708();
    r3 = r31;
    r4 = 0x6;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r30;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r29;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r28;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r27;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r26;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r25;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r24;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r24;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r23;
    r4 = 0xb;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008E320 | size: 0x4B4 */
void fn_8008E320(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
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

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r27 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r27 = r3;
        if (r27 < 1) {
            r27 = 0x1;
    }
    }
    r26 = 0x0;
    while (r26 < r27) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r26 = r26 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r26 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r26 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r26 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r26 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r26;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r26;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r26;
    ((void(*)(void))fn_800ECA78)();
    r3 = r26;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r26;
    ((void(*)(void))fn_800EC990)();
    r4 = 0x111B0000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0xCE60000;
    r26 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r26;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r27 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))fn_800C46B0)();
        r27 = r3;
        if (r27 < 1) {
            r27 = 0x1;
    }
    }
    r26 = 0x0;
    while (r26 < r27) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r26 = r26 + r3;

    }
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BC0000;
    r30 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r29 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r28 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r27 = tmp;
    r4 = r30;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r26 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r26;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r26;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r29;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r26 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r26;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r26;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r26 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r26;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r26;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r26 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r26;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r26;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r26 = r3;
    r3 = r31;
    r4 = r30;
    ((void(*)(void))fn_800F9318)();
    r4 = r26;
    ((void(*)(void))fn_800E3C08)();
    r3 = r31;
    r4 = r30;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r4 = 0xCFD0000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r25 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r25 = r3;
        if (r25 < 1) {
            r25 = 0x1;
    }
    }
    r26 = 0x0;
    while (r26 < r25) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r26 = r26 + r3;

    }
    r3 = 0xD070000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    r4 = 0xD070000;
    r25 = r3;
    r3 = r4 + 0x1001;
    fn_801CBA0C();
    r4 = 0xD070000;
    r26 = r3;
    r3 = r4 + 0x1003;
    fn_801CBA0C();
    r4 = 0xD070000;
    r24 = r3;
    r3 = r4 + 0x1002;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r23 = tmp;
    r4 = r30;
    r5 = r31;
    r6 = r25;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r29;
    r5 = r31;
    r6 = r26;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r28;
    r5 = r31;
    r6 = r24;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r23;
    r7 = 0x0;
    fn_801845E4();
    r3 = r30;
    r4 = 0xa;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r29;
    r4 = 0x5;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r28;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r27;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008E7D4 | size: 0x454 */
void fn_8008E7D4(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB708();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x30];
    u32 tmp = 0;
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

    r28 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r28;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r27 = 0x0;
    while (r27 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r27 = r27 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r28;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r27 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r27 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r27 + 0x144) = tmp;
    r3 = r28;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r27 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r27;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r27;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r27;
    ((void(*)(void))fn_800ECA78)();
    r3 = r27;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r27;
    ((void(*)(void))fn_800EC990)();
    r4 = 0x111B0000;
    r3 = r28;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0xCE60000;
    r27 = r3;
    r3 = r28;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r27;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0xCE60000;
    r3 = r28;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r27 = 0x0;
    while (r27 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r27 = r27 + r3;

    }
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BC0000;
    r31 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r30 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r28;
    r29 = tmp;
    r4 = r31;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r27 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r27;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r27;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r28;
    r4 = r30;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r27 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r27;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r27;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r28;
    r4 = r29;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r27 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r27;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r27;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0x11510000;
    r3 = r28;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r27 = r3;
    r3 = r28;
    r4 = r31;
    ((void(*)(void))fn_800F9318)();
    r4 = r27;
    ((void(*)(void))fn_800E3C08)();
    r3 = r28;
    r4 = r31;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r4 = 0xCFC0000;
    r3 = r28;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r26 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r26 = r3;
        if (r26 < 1) {
            r26 = 0x1;
    }
    }
    r27 = 0x0;
    while (r27 < r26) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r27 = r27 + r3;

    }
    r3 = 0xD060000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    r4 = 0xD060000;
    r26 = r3;
    r3 = r4 + 0x1001;
    fn_801CBA0C();
    r4 = 0xD060000;
    r27 = r3;
    r3 = r4 + 0x1002;
    fn_801CBA0C();
    tmp = r3;
    r3 = r28;
    r25 = tmp;
    r4 = r31;
    r5 = r28;
    r6 = r26;
    r7 = 0x2;
    fn_801845E4();
    r3 = r28;
    r4 = r30;
    r5 = r28;
    r6 = r27;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = r29;
    r5 = r28;
    r6 = r25;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = 0x7;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r31;
    r4 = 0x1;
    fn_801CB708();
    r3 = r31;
    r4 = 0x8;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r30;
    r4 = 0x5;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r29;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008EC28 | size: 0x2A8 */
void fn_8008EC28(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))fn_800ECA78)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r29;
    ((void(*)(void))fn_800EC990)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r4 = 0xCFB0000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xD050000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = 0x7;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008EED0 | size: 0x2C0 */
void fn_8008EED0(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB708();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))fn_800ECA78)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r29;
    ((void(*)(void))fn_800EC990)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x4;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r4 = 0xCFA0000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xD030000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = 0x1;
    fn_801CB708();
    r3 = r28;
    r4 = 0x5;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008F190 | size: 0x394 */
void fn_8008F190(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB708();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f8 = 0.0f;
    f32 f9 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))fn_800ECA78)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r29;
    ((void(*)(void))fn_800EC990)();
    r4 = 0x111B0000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0xCE60000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r30 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x0;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r4 = 0xCF90000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xCF80000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = 0x1;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r28;
    r4 = 0x1;
    fn_801CB708();
    r3 = r28;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r28;
    r4 = 0x1;
    fn_801CB708();
    r3 = r28;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008F524 | size: 0x3F8 */
void fn_8008F524(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x30];
    u32 tmp = 0;
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
    f32 f7 = 0.0f;

    r28 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r28;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r28;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r29 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r29 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r29 + 0x144) = tmp;
    r3 = r28;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))fn_800ECA78)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r29;
    ((void(*)(void))fn_800EC990)();
    r4 = 0x111B0000;
    r3 = r28;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0xCE60000;
    r29 = r3;
    r3 = r28;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r29;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0xCE60000;
    r3 = r28;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r30 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0x6BC0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r31 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r30 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r28;
    r29 = tmp;
    r4 = r31;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r27 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r27;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r27;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r28;
    r4 = r30;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r27 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r27;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r27;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r28;
    r4 = r29;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r27 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r27;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r27;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xCF70000;
    r3 = r28;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r26 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r26 = r3;
        if (r26 < 1) {
            r26 = 0x1;
    }
    }
    r27 = 0x0;
    while (r27 < r26) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r27 = r27 + r3;

    }
    r3 = 0xCEE0000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    r4 = 0xCEE0000;
    r26 = r3;
    r3 = r4 + 0x1002;
    fn_801CBA0C();
    r4 = 0xCEE0000;
    r27 = r3;
    r3 = r4 + 0x1001;
    fn_801CBA0C();
    tmp = r3;
    r3 = r28;
    r25 = tmp;
    r4 = r31;
    r5 = r28;
    r6 = r26;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = r30;
    r5 = r28;
    r6 = r27;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = r29;
    r5 = r28;
    r6 = r25;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r30;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r29;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008F91C | size: 0x2D8 */
void fn_8008F91C(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x30];
    u32 tmp = 0;
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f6 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))fn_800ECA78)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r29;
    ((void(*)(void))fn_800EC990)();
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BA0000;
    r28 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r27 = tmp;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xCF60000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xCED0000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    r4 = 0xCED0000;
    r30 = r3;
    r3 = r4 + 0x1001;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r29 = tmp;
    r4 = r27;
    r5 = r31;
    r6 = r30;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r28;
    r5 = r31;
    r6 = r29;
    r7 = 0x0;
    fn_801845E4();
    r3 = r27;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r28;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008FBF4 | size: 0x2A0 */
void fn_8008FBF4(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f5 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))fn_800ECA78)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r29;
    ((void(*)(void))fn_800EC990)();
    r3 = 0x6AF0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x0;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0x11510000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r4 = 0xCF50000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xCEC0000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8008FE94 | size: 0x26C */
void fn_8008FE94(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f4 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r29 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r29;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r29;
    ((void(*)(void))fn_800ECA78)();
    r3 = r29;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r29;
    ((void(*)(void))fn_800EC990)();
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xCF40000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xCEB0000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80090100 | size: 0x620 */
void fn_80090100(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r15 = 0;
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f3 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r18 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r18 = r3;
        if (r18 < 1) {
            r18 = 0x1;
    }
    }
    r17 = 0x0;
    while (r17 < r18) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r17 = r17 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r18 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r18 = r3;
        if (r18 < 1) {
            r18 = 0x1;
    }
    }
    r17 = 0x0;
    while (r17 < r18) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r17 = r17 + r3;

    }
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r17 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r17 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0xCE60000;
    *(u32*)((u8*)r17 + 0x144) = tmp;
    r3 = r31;
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = r4 + 0x1004;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x0;
    r17 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r17;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r17;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x0;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r17;
    ((void(*)(void))fn_800ECA78)();
    r3 = r17;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r17;
    ((void(*)(void))fn_800EC990)();
    r3 = 0x6BC0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0xCEA0000;
    r23 = r3;
    r3 = r4 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r22 = tmp;
    r4 = r23;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = 0xD290000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r24 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r25 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r26 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r27 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD290000;
    r30 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r29 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r24;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r25;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r26;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r30;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r29;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r17 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r17;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r17;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xCF30000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r18 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r18 = r3;
        if (r18 < 1) {
            r18 = 0x1;
    }
    }
    r17 = 0x0;
    while (r17 < r18) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r17 = r17 + r3;

    }
    r3 = 0xCEA0000;
    r3 = r3 + 0x1006;
    fn_801CBA0C();
    r4 = 0xCEA0000;
    r21 = r3;
    r3 = r4 + 0x1007;
    fn_801CBA0C();
    r4 = 0xCEA0000;
    r20 = r3;
    r3 = r4 + 0x1001;
    fn_801CBA0C();
    r4 = 0xCEA0000;
    r19 = r3;
    r3 = r4 + 0x1002;
    fn_801CBA0C();
    r4 = 0xCEA0000;
    r18 = r3;
    r3 = r4 + 0x1003;
    fn_801CBA0C();
    r4 = 0xCEA0000;
    r17 = r3;
    r3 = r4 + 0x1004;
    fn_801CBA0C();
    r4 = 0xCEA0000;
    r16 = r3;
    r3 = r4 + 0x1005;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r15 = tmp;
    r4 = r23;
    r5 = r31;
    r6 = r22;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r24;
    r5 = r31;
    r6 = r21;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r25;
    r5 = r31;
    r6 = r20;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r26;
    r5 = r31;
    r6 = r19;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r18;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r30;
    r5 = r31;
    r6 = r17;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r29;
    r5 = r31;
    r6 = r16;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r28;
    r5 = r31;
    r6 = r15;
    r7 = 0x0;
    fn_801845E4();
    r3 = r23;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r24;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r25;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r26;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r27;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r30;
    r4 = 0x5;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r29;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r28;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80090720 | size: 0x2C4 */
void fn_80090720(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB708();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = 0xCE60000;
    r3 = r31;
    *(f32*)(sp + 0x8) = f0;
    r4 = r4 + 0x1004;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x1;
    r30 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r30;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r30;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x1;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r30;
    ((void(*)(void))fn_800ECA78)();
    r3 = r30;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r30;
    ((void(*)(void))fn_800EC990)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r3 = 0x6BC0000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r28 = tmp;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xCF20000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xCE90000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = 0x1;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1004;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r28;
    r4 = 0x1;
    fn_801CB708();
    r3 = r28;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r28;
    r4 = 0x1;
    fn_801CB708();
    r3 = r28;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x800909E4 | size: 0x350 */
void fn_800909E4(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x30];
    u32 tmp = 0;
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = 0xCE60000;
    r3 = r31;
    *(f32*)(sp + 0x8) = f0;
    r4 = r4 + 0x1004;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x1;
    r30 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r30;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r30;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x1;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r30;
    ((void(*)(void))fn_800ECA78)();
    r3 = r30;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r30;
    ((void(*)(void))fn_800EC990)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0x111B0000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0xCE60000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r30 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BA0000;
    r28 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r27 = tmp;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xCF10000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xCE80000;
    r3 = r3 + 0x1001;
    fn_801CBA0C();
    r4 = 0xCE80000;
    r29 = r3;
    r3 = r4 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r30 = tmp;
    r4 = r28;
    r5 = r31;
    r6 = r29;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r30;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r27;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80090D34 | size: 0x2D8 */
void fn_80090D34(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x30];
    u32 tmp = 0;
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
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    f0 = *(f32*)&lbl_8047C1D4;
    r4 = 0xCE60000;
    r3 = r31;
    *(f32*)(sp + 0x8) = f0;
    r4 = r4 + 0x1004;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x1;
    r30 = r3;
    ((void(*)(void))fn_800ECCA8)();
    r3 = r30;
    r4 = (u32)sp + 0x8;
    r5 = 0x0;
    ((void(*)(void))fn_800EC4D0)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r30;
    f0 = *(f32*)&lbl_8047C1D8;
    r4 = 0x1;
    f0 = f1 - f0;
    *(f32*)(sp + 0x8) = f0;
    ((void(*)(void))fn_800ECCA8)();
    f1 = *(f32*)(sp + 0x8);
    r3 = r30;
    ((void(*)(void))fn_800ECA78)();
    r3 = r30;
    r4 = 0x0;
    ((void(*)(void))fn_800ECB74)();
    r3 = r30;
    ((void(*)(void))fn_800EC990)();
    r3 = 0xCE60000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xCE60000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r30 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r30 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r3 = 0x6BD0000;
    *(u32*)((u8*)r30 + 0x144) = tmp;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BA0000;
    r28 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r27 = tmp;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xCF00000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xCE70000;
    r3 = r3 + 0x1001;
    fn_801CBA0C();
    r4 = 0xCE70000;
    r29 = r3;
    r3 = r4 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r30 = tmp;
    r4 = r28;
    r5 = r31;
    r6 = r29;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r30;
    r7 = 0x0;
    fn_801845E4();
    r3 = r28;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r27;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x8009100C | size: 0x558 */
void fn_8009100C(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r14 = 0;
    u32 r15 = 0;
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
    f32 f1 = 0.0f;

    r15 = r3;
    r4 = 0x6DB0000;
    r4 = r4 + 0x1604;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DB0000;
    r3 = r15;
    r4 = r4 + 0x1001;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0x6BC0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r24 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r23 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD290000;
    r22 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r21 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r20 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD290000;
    r19 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r18 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0x6BE0000;
    r17 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r15;
    r16 = tmp;
    r4 = r24;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r23;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r22;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r21;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r20;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r19;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r18;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r17;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r15;
    r4 = r16;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r14 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r14;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r14;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xC390000;
    r3 = r15;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r25 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r25 = r3;
        if (r25 < 1) {
            r25 = 0x1;
    }
    }
    r14 = 0x0;
    while (r14 < r25) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r14 = r14 + r3;

    }
    r3 = 0xC380000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    r4 = 0xC380000;
    r3 = r4 + 0x1008;
    fn_801CBA0C();
    r4 = 0xC380000;
    r14 = r3;
    r3 = r4 + 0x1001;
    fn_801CBA0C();
    r4 = 0xC380000;
    r31 = r3;
    r3 = r4 + 0x1002;
    fn_801CBA0C();
    r4 = 0xC380000;
    r30 = r3;
    r3 = r4 + 0x1003;
    fn_801CBA0C();
    r4 = 0xC380000;
    r29 = r3;
    r3 = r4 + 0x1004;
    fn_801CBA0C();
    r4 = 0xC380000;
    r28 = r3;
    r3 = r4 + 0x1005;
    fn_801CBA0C();
    r4 = 0xC380000;
    r27 = r3;
    r3 = r4 + 0x1006;
    fn_801CBA0C();
    r4 = 0xC380000;
    r26 = r3;
    r3 = r4 + 0x1007;
    fn_801CBA0C();
    tmp = r3;
    r3 = r15;
    r4 = r24;
    r25 = tmp;
    r5 = r15;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r23;
    r5 = r15;
    r6 = r14;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r22;
    r5 = r15;
    r6 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r21;
    r5 = r15;
    r6 = r30;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r20;
    r5 = r15;
    r6 = r29;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r19;
    r5 = r15;
    r6 = r28;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r18;
    r5 = r15;
    r6 = r27;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r17;
    r5 = r15;
    r6 = r26;
    r7 = 0x0;
    fn_801845E4();
    r3 = r15;
    r4 = r16;
    r5 = r15;
    r6 = r25;
    r7 = 0x0;
    fn_801845E4();
    r3 = r24;
    r4 = 0x3;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r23;
    r4 = 0x5;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r22;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r21;
    r4 = 0xf;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r20;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r19;
    r4 = 0x4;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r18;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r17;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r16;
    r4 = 0xe;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80091564 | size: 0x210 */
void fn_80091564(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0x6DC0000;
    r4 = r4 + 0x1605;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1001;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0x6DC0000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0x11260000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x6DC0000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0x6DC0000;
    r4 = 0x4;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x64;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1DC;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x7;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xC420000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xC3D0000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x81;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80091774 | size: 0x210 */
void fn_80091774(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0x6DC0000;
    r4 = r4 + 0x1605;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1001;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0x6DC0000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0x11250000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x6DC0000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0x6DC0000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x64;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1DC;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x6;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xC410000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xC3C0000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x82;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80091984 | size: 0x210 */
void fn_80091984(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0x6DC0000;
    r4 = r4 + 0x1605;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1001;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0x6DC0000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0x11240000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x6DC0000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0x6DC0000;
    r4 = 0x2;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x64;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1DC;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x5;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xC400000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xC3B0000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x82;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80091B94 | size: 0x210 */
void fn_80091B94(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0x6DC0000;
    r4 = r4 + 0x1605;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1001;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0x6DC0000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0x11220000;
    r3 = r31;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x6DC0000;
    r30 = r3;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r30;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0x6DC0000;
    r3 = r31;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0x6DC0000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r29 = 0x64;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1DC;
        ((void(*)(void))fn_800C46B0)();
        r29 = r3;
        if (r29 < 1) {
            r29 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r29) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0x6BD0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0x1;
    r28 = r3;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xC3E0000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0x10490000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r6 = tmp;
    r4 = r28;
    r5 = r31;
    r7 = 0x0;
    fn_801845E4();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x82;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80091DA4 | size: 0x1A4 */
void fn_80091DA4(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r29 = r3;
    r4 = 0xCE60000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xCE60000;
    r3 = r29;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xCE60000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r3 = 0xCE60000;
    r4 = 0x1;
    r3 = r3 + 0x1004;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r4 = 0x111B0000;
    r3 = r29;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0xCE60000;
    r31 = r3;
    r3 = r29;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r31;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0xCE60000;
    r3 = r29;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r3 = 0xCE60000;
    r4 = 0x3;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r30 = 0x32;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1E0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r31 = 0x0;
    while (r31 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r31 = r31 + r3;

    }
    r4 = 0xCEF0000;
    r3 = r29;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r31 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r31 = r3;
        if (r31 < 1) {
            r31 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r31) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x82;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80091F48 | size: 0x1F8 */
void fn_80091F48(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB708();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
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
    f32 f1 = 0.0f;

    r30 = r3;
    r4 = 0x6DD0000;
    r4 = r4 + 0x1604;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DD0000;
    r3 = r30;
    r4 = r4 + 0x1001;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0x6BB0000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r31 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r30;
    r27 = tmp;
    r4 = r31;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r30;
    r4 = r27;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r29 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r29;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r29;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xB890000;
    r3 = r30;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r28 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r28 = r3;
        if (r28 < 1) {
            r28 = 0x1;
    }
    }
    r29 = 0x0;
    while (r29 < r28) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r29 = r29 + r3;

    }
    r3 = 0xB860000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    r4 = 0xB860000;
    r29 = r3;
    r3 = r4 + 0x1001;
    fn_801CBA0C();
    tmp = r3;
    r3 = r30;
    r28 = tmp;
    r4 = r31;
    r5 = r30;
    r6 = r29;
    r7 = 0x0;
    fn_801845E4();
    r3 = r30;
    r4 = r27;
    r5 = r30;
    r6 = r28;
    r7 = 0x0;
    fn_801845E4();
    r3 = r27;
    r4 = 0x8;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r31;
    r4 = 0x8;
    r5 = 0x32;
    r6 = 0x0;
    fn_801CB834();
    r3 = r31;
    r4 = 0x0;
    fn_801CB708();
    r3 = r31;
    r4 = 0x9;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x89;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80092140 | size: 0x358 */
void fn_80092140(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
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
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0x6DD0000;
    r4 = r4 + 0x1604;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DD0000;
    r3 = r31;
    r4 = r4 + 0x1001;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xD240000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r30 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r29 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r28 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r27 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r26 = tmp;
    r4 = r30;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r29;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r26;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xB880000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r24 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r24 = r3;
        if (r24 < 1) {
            r24 = 0x1;
    }
    }
    r25 = 0x0;
    while (r25 < r24) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r25 = r25 + r3;

    }
    r3 = 0xB850000;
    r3 = r3 + 0x1004;
    fn_801CBA0C();
    r4 = 0xB850000;
    r24 = r3;
    r3 = r4 + 0x1003;
    fn_801CBA0C();
    r4 = 0xB850000;
    r25 = r3;
    r3 = r4 + 0x1001;
    fn_801CBA0C();
    r4 = 0xB850000;
    r23 = r3;
    r3 = r4 + 0x1002;
    fn_801CBA0C();
    r4 = 0xB850000;
    r22 = r3;
    r3 = r4 + 0x1003;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r21 = tmp;
    r4 = r30;
    r5 = r31;
    r6 = r24;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r29;
    r5 = r31;
    r6 = r25;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r28;
    r5 = r31;
    r6 = r23;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r22;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r26;
    r5 = r31;
    r6 = r21;
    r7 = 0x0;
    fn_801845E4();
    r3 = r30;
    r4 = 0x6;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r29;
    r4 = 0x6;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r28;
    r4 = 0x8;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r27;
    r4 = 0x8;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r26;
    r4 = 0x7;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x83;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80092498 | size: 0x1CC */
void fn_80092498(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r29 = r3;
    r4 = 0xB630000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xB630000;
    r3 = r29;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xB630000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r31 = 0x0;
    while (r31 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r31 = r31 + r3;

    }
    r3 = 0xB630000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xB630000;
    r3 = r29;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r31 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r31 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    r4 = 0x112B0000;
    *(u32*)((u8*)r31 + 0x144) = tmp;
    r3 = r29;
    r4 = r4 + 0x1400;
    ((void(*)(void))fn_800F9318)();
    r4 = 0xB630000;
    r31 = r3;
    r3 = r29;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = r31;
    ((void(*)(void))fn_800E3C08)();
    r4 = 0xB630000;
    r3 = r29;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x4;
    ((void(*)(void))fn_800E3C00)();
    r4 = 0xB830000;
    r3 = r29;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r31 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r31 = r3;
        if (r31 < 1) {
            r31 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r31) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0xB630000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x83;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80092664 | size: 0x358 */
void fn_80092664(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
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
    f32 f1 = 0.0f;

    r31 = r3;
    r4 = 0x6DD0000;
    r4 = r4 + 0x1604;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0x6DD0000;
    r3 = r31;
    r4 = r4 + 0x1001;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xD240000;
    r3 = r3 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r30 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r29 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r28 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    r4 = 0xD240000;
    r27 = r3;
    r3 = r4 + 0x400;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r26 = tmp;
    r4 = r30;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r29;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r28;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r27;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r3 = r31;
    r4 = r26;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r25 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r25;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r25;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    r4 = 0xB870000;
    r3 = r31;
    r4 = r4 + 0x1800;
    r5 = 0x0;
    r6 = 0x0;
    fn_80176E0C();
    r24 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r24 = r3;
        if (r24 < 1) {
            r24 = 0x1;
    }
    }
    r25 = 0x0;
    while (r25 < r24) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r25 = r25 + r3;

    }
    r3 = 0xB840000;
    r3 = r3 + 0x1004;
    fn_801CBA0C();
    r4 = 0xB840000;
    r24 = r3;
    r3 = r4 + 0x1000;
    fn_801CBA0C();
    r4 = 0xB840000;
    r25 = r3;
    r3 = r4 + 0x1001;
    fn_801CBA0C();
    r4 = 0xB840000;
    r23 = r3;
    r3 = r4 + 0x1002;
    fn_801CBA0C();
    r4 = 0xB840000;
    r22 = r3;
    r3 = r4 + 0x1003;
    fn_801CBA0C();
    tmp = r3;
    r3 = r31;
    r21 = tmp;
    r4 = r30;
    r5 = r31;
    r6 = r24;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r29;
    r5 = r31;
    r6 = r25;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r28;
    r5 = r31;
    r6 = r23;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r27;
    r5 = r31;
    r6 = r22;
    r7 = 0x0;
    fn_801845E4();
    r3 = r31;
    r4 = r26;
    r5 = r31;
    r6 = r21;
    r7 = 0x0;
    fn_801845E4();
    r3 = r30;
    r4 = 0x1;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r29;
    r4 = 0x1;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r28;
    r4 = 0x1;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r27;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r26;
    r4 = 0x2;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x87;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x800929BC | size: 0x170 */
void fn_800929BC(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801CB7C4();
    extern void fn_801CB834();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r29 = r3;
    r4 = 0xB630000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xB630000;
    r3 = r29;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xB630000;
    r4 = 0x0;
    r3 = r3 + 0x1000;
    r5 = 0x0;
    r6 = 0x0;
    fn_801CB834();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r31 = 0x0;
    while (r31 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r31 = r31 + r3;

    }
    r3 = 0xB630000;
    r3 = r3 + 0x1000;
    fn_801CB7C4();
    r4 = 0xB630000;
    r3 = r29;
    r4 = r4 + 0x1000;
    ((void(*)(void))fn_800F9318)();
    r31 = r3;
    r4 = 0x1;
    r3 = *(u32*)((u8*)r31 + 0x144);
    ((void(*)(void))fn_80118874)();
    tmp = 0x0;
    *(u32*)((u8*)r31 + 0x144) = tmp;
    ((void(*)(void))fn_80113F48)();
    r4 = 0xB660000;
    r5 = 0x0;
    r4 = r4 + 0x1800;
    r6 = 0x0;
    fn_80176E0C();
    r31 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r31 = r3;
        if (r31 < 1) {
            r31 = 0x1;
    }
    }
    r30 = 0x0;
    while (r30 < r31) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r30 = r30 + r3;

    }
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x83;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}

/* 0x80092B2C | size: 0x164 */
void fn_80092B2C(void) {
    extern void fn_80176B48();
    extern void fn_80176E0C();
    extern void fn_801845E4();
    extern void fn_801CB834();
    extern void fn_801CBA0C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r28 = r3;
    r4 = 0xB630000;
    r4 = r4 + 0x1602;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A690 = r3;
    r4 = 0xB630000;
    r3 = r28;
    r4 = r4 + 0x1002;
    ((void(*)(void))fn_800F9318)();
    *(u32*)&lbl_8047A694 = r3;
    r3 = 0x280;
    r4 = 0x1e0;
    ((void(*)(void))fn_800E8FA0)();
    r3 = 0xB720000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    r4 = 0x6;
    r29 = r3;
    r5 = 0x0;
    r6 = 0x1;
    fn_801CB834();
    r3 = r28;
    r4 = r29;
    ((void(*)(void))fn_800F9318)();
    r4 = 0x2;
    r31 = r3;
    ((void(*)(void))fn_800E9108)();
    r4 = *(u32*)&lbl_8047A690;
    r3 = r31;
    ((void(*)(void))fn_800E8FE8)();
    r3 = r31;
    r4 = 0x1;
    r5 = (u32)&lbl_8047A694;
    ((void(*)(void))fn_800E900C)();
    ((void(*)(void))fn_80113F48)();
    r4 = 0xB650000;
    r5 = 0x0;
    r4 = r4 + 0x1800;
    r6 = 0x0;
    fn_80176E0C();
    r30 = 0x1;
    ((void(*)(void))fn_800D37CC)();
    if ((s32)r3 == 0x32) {
        f1 = *(f32*)&lbl_8047C1D0;
        ((void(*)(void))fn_800C46B0)();
        r30 = r3;
        if (r30 < 1) {
            r30 = 0x1;
    }
    }
    r31 = 0x0;
    while (r31 < r30) {

        ((void(*)(void))_threadSwitch)();
        ((void(*)(void))fn_800D3088)();
        r31 = r31 + r3;

    }
    r3 = 0xB730000;
    r3 = r3 + 0x1000;
    fn_801CBA0C();
    tmp = r3;
    r3 = r28;
    r6 = tmp;
    r4 = r29;
    r5 = r28;
    r7 = 0x0;
    fn_801845E4();
    r3 = 0x1;
    fn_80176B48();
    r3 = 0x87;
    ((void(*)(void))fn_800FF58C)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_8011288C)();
    return;
}
