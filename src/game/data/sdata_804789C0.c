#include "dolphin/types.h"

extern void defaultOptionalCommandChecker();
extern u8 lbl_80311A80[];
extern u8 lbl_80311C48[];
extern u8 lbl_80311CB8[];

char lbl_804789C0[0x8] = "dvdfs.c";
void* __DVDVersion = lbl_80311A80;
u32 autoInvalidation_804789CC = 0x00000001;
void* lbl_804789D0 = defaultOptionalCommandChecker;
char lbl_804789D4[0x6] = "dvd.c";
u32 lbl_804789DC = 0xFFFFFFFF;
void* lbl_804789E0 = lbl_80311C48;
void* lbl_804789E4 = lbl_80311CB8;
