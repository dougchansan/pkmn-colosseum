/**
 * @file script_callback.c
 * @brief Game script callback handlers for UI and Pokemon operations.
 *
 * Provides callback functions that the game's script interpreter
 * (psinterpret.c) invokes when script opcodes need to interact with
 * the Pokemon, party, and field systems. These callbacks bridge the
 * event scripting system with the game's UI subsystems.
 *
 * Key behaviors:
 *   - fn_80053110 (0x33C bytes) is a multi-case handler that reads script
 *     parameters and dispatches to appropriate subsystems
 *   - fn_80053778 references BSS lbl_803A95E8 (0x138 bytes) and
 *     lbl_803A9720 (0x48 bytes), and calls fn_80057270 (text system)
 *     and fn_80057E40 (dialog system)
 *   - Functions fn_80053A60 through fn_80053DD4 form a series of nearly
 *     identical wrappers (each 0x68 bytes) that:
 *     1. Call fn_80057270 to get the current text/message context
 *     2. Call fn_8011E820 to check a Pokemon status flag
 *     3. Test different bit positions (bits 28, 29, 30, 31)
 *     4. Pass result to fn_80109220 for model visibility
 *     These are "check Pokemon condition X" script callbacks:
 *     fn_80053A60: checks bit 28 (poisoned?)
 *     fn_80053AC8: checks bit 29 (paralyzed?)
 *     fn_80053B30: checks bit 30 (burned?)
 *     fn_80053B98: checks bit 31 (frozen?)
 *   - fn_80053C00 is different -- checks species presence via fn_801230E0,
 *     checks people state via fn_801440A0/fn_80144088, then triggers
 *     a sound effect (0xE7) and effect (0x37)
 *   - fn_80053C84 checks species count via fn_801230E0
 *   - fn_80053CE8 through fn_80053DD4 check different battle-related flags
 *   - fn_80053ED8 (0x548 bytes) is a larger handler that manages text
 *     window state via fn_80057DE8 and fn_80057F94
 *   - fn_80054420 and fn_800544A8 are standalone handlers
 *   - fn_8005464C through fn_8005471C are small accessor functions
 *   - fn_80054760 updates BSS lbl_803A95E8 and lbl_803A9720
 *
 * BSS usage:
 *   - lbl_803A95E8 (0x138 bytes): Script callback state A
 *   - lbl_803A9720 (0x48 bytes): Script callback state B
 *
 * Address range: 0x80053110 - 0x80054914 (25 functions)
 */

#include "dolphin/types.h"

typedef struct ScriptCallbackStateCopy {
    u32 data[78];
} ScriptCallbackStateCopy;

/* ===== GS Engine ===== */
extern void  fn_80109220(u32 obj, u8 visible);  /* Model visibility */
extern void  fn_800FB680(u32 a, u32 b, s32 c, u32 d);  /* Sound trigger */
extern void  fn_80132A38(u32 effectId, u32 param);      /* Effect trigger */
extern void* fn_800FA280(void);                /* Field utility */
extern u16   fn_801230E0(void);                /* Species count/presence */
extern void* fn_801440A0(void);                /* People state query A */
extern void* fn_80144088(void);                /* People state query B */
extern u8    fn_8011E820(void);                /* Pokemon status flag get */

/* ===== Text / Dialog system (internal) ===== */
extern void* fn_80057270(void);                /* Text context get (in msgbox.c) */
extern void  fn_80057DE8(void* ctx);           /* Dialog state set (in dialog.c) */
extern s32   fn_80057E40(void);                /* Dialog state get (in dialog.c) */
extern void  fn_80057F94(void* ctx);           /* Dialog update (in dialog.c) */
extern void  fn_80058F08(void);                /* Text utility (in dialog.c) */

/* ===== Rendering / model helpers ===== */
extern void  fn_800D88DC(u32 a);
extern void  fn_800D888C(u32 a);
extern void  fn_800D6A00(u32 a);
extern void  fn_800D7820(void* a);
extern void  fn_800D67BC(u32 a);
extern void  fn_800D61E4(s32 a, s32 b);
extern void  fn_800D5CB8(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void  fn_800D6728(void);
extern void  fn_800D85D4(u32 a, u32 b);
extern void  fn_800D59B8(u32 a, f32 b, f32 c);
extern u8    fn_80109B90(void* obj, u32 wait);
extern u8    fn_8010A210(void* obj, void* ctx);
extern u32   fn_80109934(void* obj);
extern void  fn_80109C88(void* obj, void* ctx);

/* ===== BSS data ===== */
extern ScriptCallbackStateCopy lbl_803A95E8[];   /* Script callback state A (0x138 bytes) */
extern u8    lbl_803A9720[];   /* Script callback state B (0x48 bytes) */
extern u8    lbl_80314E08[];
extern u8    lbl_80314F98[];
extern f64   lbl_8047BE58;
extern f32   lbl_8047BE60;
extern f32   lbl_8047BE64;
extern f32   lbl_8047BE68;
extern f32   lbl_8047BE6C;
extern f32   lbl_8047BE70;
extern f32   lbl_8047A540;

/*
 * Functions in this translation unit (25 total):
 *
 * fn_80053110  0x33C  Multi-case script handler
 * fn_8005344C  0x0D0  Script callback (calls fn_80057094, fn_80057A08, fn_80058F08)
 * fn_8005351C  0x20C  Script callback (calls fn_80057E40)
 * fn_80053728  0x050  Script callback (calls fn_80057E40)
 * fn_80053778  0x2E8  Script callback main (lbl_803A95E8, lbl_803A9720, calls fn_80057270)
 * fn_80053A60  0x068  Check Pokemon condition: bit 28 (poisoned?)
 * fn_80053AC8  0x068  Check Pokemon condition: bit 29 (paralyzed?)
 * fn_80053B30  0x068  Check Pokemon condition: bit 30 (burned?)
 * fn_80053B98  0x068  Check Pokemon condition: bit 31 (frozen?)
 * fn_80053C00  0x084  Script: species presence + trigger effect 0x37 + sound 0xE7
 * fn_80053C84  0x064  Script: species count check
 * fn_80053CE8  0x07C  Check battle flag variant A
 * fn_80053D64  0x070  Check battle flag variant B
 * fn_80053DD4  0x0A8  Check battle flag variant C
 * fn_80053E7C  0x05C  Script result accessor
 * fn_80053ED8  0x548  Script text window handler (calls fn_80057DE8, fn_80057F94)
 * fn_80054420  0x088  Script standalone handler A
 * fn_800544A8  0x1A4  Script standalone handler B
 * fn_8005464C  0x024  Script accessor stub A
 * fn_80054670  0x010  Script accessor stub B
 * fn_80054680  0x040  Script accessor stub C
 * fn_800546C0  0x030  Script accessor stub D
 * fn_800546F0  0x02C  Script accessor stub E
 * fn_8005471C  0x044  Script accessor (lbl_803A9720)
 * fn_80054760  0x1B4  Script state updater (lbl_803A95E8, lbl_803A9720)
 */


/* 0x80053110 | size: 0x33C */
asm void fn_80053110(void) { nofralloc
    #include "asm/GC6E01/nonmatching/script_callback/fn_80053110.s"
}

/* 0x80053778 | size: 0x2E8 */
#pragma push
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u32 fn_80053778(u32 unused, u8* p) {
    u32 model;
    s32 alpha;
    s32 y;
    ScriptCallbackStateCopy* msgCtx;
    f32 scale;
    f32 fade;

    model = 0;
    if (fn_80057E40() != 2) {
        fn_800D88DC(1);
        fn_800D888C(6);
        fn_800D6A00(6);
        fn_800D7820(lbl_80314E08);

        scale = lbl_8047BE64 * lbl_8047A540;
        if (scale > lbl_8047BE60) {
            scale = lbl_8047BE60;
        }
        y = (s32)((f32)(s32)*(s16*)(p + 0x56) * scale);

        fade = (*(volatile f32*)&lbl_8047BE64) * (lbl_8047A540 - lbl_8047BE6C);
        if (fade < lbl_8047BE68) {
            fade = lbl_8047BE68;
        }
        alpha = (s32)(lbl_8047BE70 * (*(volatile f32*)&lbl_8047BE60 - fade));

        fn_800D67BC(4);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0);
        fn_800D61E4(*(s16*)(p + 0x54), 0);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0);
        fn_800D61E4(*(s16*)(p + 0x54), y);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, alpha);
        fn_800D61E4(0, y);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, alpha);
        fn_800D6728();
        return 0;
    }

    msgCtx = fn_80057270();
    if (msgCtx == NULL) {
        return 0;
    }

    if (fn_80109B90(lbl_803A9720, 0) == 0) {
        if (fn_8010A210(lbl_803A9720, msgCtx) != 0) {
            model = fn_80109934(lbl_803A9720);
        } else {
            fn_80109C88(lbl_803A9720, msgCtx);
            lbl_803A95E8[0] = *msgCtx;
        }
    }

    if (model != 0) {
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D6A00(7);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, model);
        fn_800D67BC(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
        fn_800D59B8(0, lbl_8047BE68, lbl_8047BE68);
        fn_800D61E4(*(s16*)(p + 0x54), *(s16*)(p + 0x56));
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
        fn_800D59B8(0, lbl_8047BE60, lbl_8047BE60);
        fn_800D6728();
    }

    return 0;
}
#pragma pop
