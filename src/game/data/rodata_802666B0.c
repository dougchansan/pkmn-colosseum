#include "dolphin/types.h"

typedef void (*FuncPtr)(void);

extern void fn_80013744(void);
extern void fn_8001374C(void);
extern void fn_800138B4(void);
extern void fn_80013A18(void);
extern void fn_80013DFC(void);
extern void fn_80013F80(void);
extern void fn_8001501C(void);
extern void fn_800150E4(void);
extern void menuPocketDrawNameList(void);
extern void fn_800155B0(void);
extern void fn_800159BC(void);
extern void menuPocketDrawItemList(void);

/*
 * .rodata table continuing after game/gs_range_800096B4.c's two stat-id
 * tables (0x80266698 - 0x802666B0), ending at the existing rodata_80266BD8
 * split. Together with rodata_802663A0.c it contains an OS-error string
 * table (OSError.c "OS_ERROR_*" names plus the associated panic-dump format
 * strings), a run of small id/index tables, two Shift-JIS diagnostic
 * strings, and a menu/UI dispatch table: SubEntry arrays of
 * {id, handler function, flags} feeding into a 6-entry BigEntry table that
 * also carries colors and secondary id lists. Values are reproduced
 * verbatim from the shipped binary; the exact producing structs for the
 * untyped tables are not yet identified.
 */
const u8 lbl_802666B0[48] = {
    0x82, 0xB1, 0x82, 0xCC, 0x83, 0x74, 0x83, 0x8D, 0x83, 0x41, 0x82, 0xA9, 0x82, 0xE7, 0x83, 0x74,
    0x83, 0x8D, 0x83, 0x41, 0x83, 0x60, 0x83, 0x46, 0x83, 0x93, 0x83, 0x57, 0x82, 0xCD, 0x82, 0xA8,
    0x82, 0xB1, 0x82, 0xC8, 0x82, 0xA6, 0x82, 0xDC, 0x82, 0xB9, 0x82, 0xF1, 0x00, 0x00, 0x00, 0x00,
};
const u32 lbl_802666E0[8] = {
    0x000000FE, 0x00000000, 0x000000FF, 0x00000001,
    0x00000100, 0x00000002, 0x00000101, 0x00000003,
};
const u32 lbl_80266700[28] = {
    0x00000102, 0x00000000, 0x00000103, 0x00000001, 0x00000104, 0x00000002, 0x00000105, 0x00000003,
    0x00000106, 0x00000004, 0x00000107, 0x00000005, 0x00000108, 0x00000006, 0x00000109, 0x00000007,
    0x0000010A, 0x00000008, 0x0000010B, 0x00000009, 0x0000010C, 0x0000000A, 0x0000010D, 0x0000000B,
    0x0000010E, 0x0000000C, 0x0000010F, 0x0000000D,
};
const u32 lbl_80266770[6] = {
    0x00000112, 0x00000000, 0x00000113, 0x00000001, 0x00000114, 0x00000002,
};
const u8 lbl_80266788[40] = {
    0x25, 0x73, 0x3A, 0x20, 0x97, 0x5C, 0x92, 0xE8, 0x8A, 0x4F, 0x82, 0xCC, 0x8F, 0xF0, 0x8C, 0x8F,
    0x82, 0xAA, 0x8B, 0x41, 0x82, 0xC1, 0x82, 0xC4, 0x82, 0xAB, 0x82, 0xC4, 0x82, 0xA2, 0x82, 0xDC,
    0x82, 0xB7, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00,
};

typedef struct SubEntry {
    u32 a;
    FuncPtr func;
    u32 c;
} SubEntry;

const SubEntry lbl_802667B0[5] = {
    { 0x00000131, fn_80013F80, 0x00000000 },
    { 0x00000132, fn_80013DFC, 0x00080000 },
    { 0x00000133, fn_80013A18, 0x00000000 },
    { 0x00002B01, fn_800138B4, 0x00050000 },
    { 0x00000134, NULL, 0x00000000 },
};
const SubEntry lbl_802667EC[3] = {
    { 0x00000132, fn_80013DFC, 0x00000000 },
    { 0x00000133, fn_80013A18, 0x00000000 },
    { 0x00000134, NULL, 0x00000000 },
};
const SubEntry lbl_80266810[3] = {
    { 0x00000131, fn_80013F80, 0x00000000 },
    { 0x00000132, fn_80013DFC, 0x00000000 },
    { 0x00000134, NULL, 0x00000000 },
};
const SubEntry lbl_80266834[4] = {
    { 0x00000131, fn_80013F80, 0x00000000 },
    { 0x00000132, fn_80013DFC, 0x00000000 },
    { 0x00000133, fn_80013A18, 0x00000000 },
    { 0x00000134, NULL, 0x00000000 },
};
const SubEntry lbl_80266864[2] = {
    { 0x00000131, fn_80013F80, 0x00000000 },
    { 0x00000134, NULL, 0x00000000 },
};
const SubEntry lbl_8026687C[4] = {
    { 0x00002B12, fn_8001374C, 0x00000000 },
    { 0x00002B3F, fn_80013744, 0x00000000 },
    { 0x00000131, fn_80013F80, 0x00000000 },
    { 0x00000134, NULL, 0x00000000 },
};
const SubEntry lbl_802668AC[2] = {
    { 0x00000131, fn_80013F80, 0x00020000 },
    { 0x00000134, NULL, 0x00000000 },
};
const SubEntry lbl_802668C4[2] = {
    { 0x00000131, fn_80013F80, 0x00000000 },
    { 0x00000134, NULL, 0x00000000 },
};
const SubEntry lbl_802668DC[1] = {
    { 0x00000134, NULL, 0x00000000 },
};
const SubEntry lbl_802668E8[2] = {
    { 0x00000131, fn_80013F80, 0x00000000 },
    { 0x00000134, NULL, 0x00000000 },
};
const SubEntry lbl_80266900[1] = {
    { 0x00000134, NULL, 0x00000000 },
};
const SubEntry lbl_8026690C[1] = {
    { 0x00000134, NULL, 0x00000000 },
};

typedef struct BigEntry {
    u32 color;
    u32 field1;
    u32 field2;
    u32 field3;
    u32 field4;
    FuncPtr func1;
    FuncPtr func2;
    u32 count1;
    u32 id;
    const SubEntry *arr1;
    u32 count2;
    const SubEntry *arr2;
    u32 count3;
    u32 zero[6];
} BigEntry;

const BigEntry lbl_80266918[6] = {
    { 0x96969600, 0xFFFFFFFF, 0x00000001, 0x00000203, 0x0000020B, fn_800150E4, fn_8001501C,
      0x00000008, 0x00002B01, lbl_8026687C, 0x00000004, lbl_8026690C, 0x00000001,
      { 0, 0, 0, 0, 0, 0 } },
    { 0xB4AED300, 0x00000005, 0x00000001, 0x00000204, 0x0000020C, menuPocketDrawNameList, fn_8001501C,
      0x00000007, 0x00002AFD, lbl_80266864, 0x00000002, lbl_80266900, 0x00000001,
      { 0, 0, 0, 0, 0, 0 } },
    { 0xAECCD300, 0x00000003, 0x00000000, 0x00000205, 0x0000020D, fn_800155B0, fn_8001501C,
      0x00000006, 0x00002AFC, lbl_80266834, 0x00000004, lbl_802668E8, 0x00000002,
      { 0, 0, 0, 0, 0, 0 } },
    { 0xAED3B200, 0x00000004, 0x00000000, 0x00000206, 0x0000020E, fn_800159BC, fn_8001501C,
      0x00000005, 0x00002AFB, lbl_80266810, 0x00000003, lbl_802668DC, 0x00000001,
      { 0, 0, 0, 0, 0, 0 } },
    { 0xD3C4AE00, 0x00000001, 0x00000001, 0x00000207, 0x0000020F, menuPocketDrawItemList, fn_8001501C,
      0x00000004, 0x00002AFA, lbl_802667EC, 0x00000003, lbl_802668C4, 0x00000002,
      { 0, 0, 0, 0, 0, 0 } },
    { 0xC0C0C000, 0x00000002, 0x00000001, 0x00000208, 0x00000210, menuPocketDrawItemList, fn_8001501C,
      0x00000003, 0x00002AF9, lbl_802667B0, 0x00000005, lbl_802668AC, 0x00000002,
      { 0, 0, 0, 0, 0, 0 } },
};
const u32 lbl_80266AE0[6] = {
    0x0000023E, 0x00000241, 0x00000242, 0x0000023D, 0x0000023F, 0x00000240,
};
const u32 lbl_80266AF8[9] = {
    0x0000024B, 0x00000247, 0x0000024A, 0x0000024C, 0x00000246,
    0x00000249, 0x0000024D, 0x00000245, 0x00000248,
};
const u32 lbl_80266B1C[6] = {
    0x0000025A, 0x00000257, 0x00000255, 0x00000259, 0x00000256, 0x00000254,
};
const u32 lbl_80266B34[9] = {
    0x0000026B, 0x00000267, 0x00000264, 0x0000026A, 0x00000266,
    0x00000263, 0x00000269, 0x00000265, 0x00000262,
};

typedef struct IdArrEntry {
    u32 id;
    const void *arr;
    u32 count;
} IdArrEntry;

const IdArrEntry lbl_80266B58[4] = {
    { 0x0000005B, lbl_80266AE0, 0x00000002 },
    { 0x0000005C, lbl_80266AF8, 0x00000003 },
    { 0x0000005D, lbl_80266B1C, 0x00000002 },
    { 0x0000005E, lbl_80266B34, 0x00000003 },
};
const u32 lbl_80266B88[5] = {
    0x00002B07, 0x00002B08, 0x00002B16, 0x00002B17, 0x00002B40,
};
const u32 lbl_80266B9C[5] = {
    0x0000023A, 0x00000239, 0x00000238, 0x00000237, 0x00000236,
};
const u32 lbl_80266BB0[5] = {
    0x00000231, 0x0000022F, 0x0000022D, 0x0000022B, 0x00000229,
};
const u32 lbl_80266BC4[5] = {
    0x00000232, 0x00000230, 0x0000022E, 0x0000022C, 0x0000022A,
};
