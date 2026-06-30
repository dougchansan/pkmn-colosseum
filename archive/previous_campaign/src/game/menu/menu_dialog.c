/**
 * @file menu_dialog.c
 * @brief Dialog window and text window management system.
 *
 * Manages the higher-level dialog/window system that wraps the message box
 * text renderer. Handles window open/close animations, choice selection
 * prompts (Yes/No, item selection), and multi-window coordination.
 *
 * Key behaviors:
 *   - Uses BSS lbl_803A9A08 (5 functions) for dialog window state
 *   - Uses BSS lbl_803A9A18 (2 functions: fn_80058754, fn_80058804) for
 *     secondary dialog state
 *   - fn_80057B34 (0x168 bytes) manages dialog window open/transition
 *   - fn_80057C9C (0x14C bytes) manages dialog window close/transition
 *   - fn_80057DE8 and fn_80057F94 are the most-called dialog functions
 *     from other modules (used by message box, script callbacks, and
 *     status screens)
 *   - fn_80058150 (0x604 = 1540 bytes) is the main dialog update loop,
 *     coordinating with fn_80055B98 (status aggregator)
 *   - fn_80058804 (called 23x externally) is a dialog utility for setting
 *     up text rendering within an active dialog window
 *   - fn_80058880 handles text rendering within dialog windows, calling
 *     fn_800566E8, fn_800573C0, fn_80057694 from msgbox.c
 *   - fn_80058AF0 and fn_80058DCC manage dialog text layout and scrolling
 *   - fn_80058F08 and fn_80058F40 are small utility functions for dialog
 *     state queries
 *   - fn_80059034 (0x978 = 2424 bytes) is a large choice-selection handler
 *     that coordinates multi-option dialogs, calling fn_8005464C through
 *     fn_8005471C (script accessor stubs)
 *   - fn_800599AC (0x230 bytes) wraps fn_80059034 as a convenience entry point
 *
 * BSS usage:
 *   - lbl_803A9A08: Dialog window state A
 *   - lbl_803A9A18: Dialog window state B
 *
 * Address range: 0x80057B34 - 0x80059BDC (19 functions)
 */

#include "dolphin/types.h"

/* ===== GS Engine ===== */
extern void  _threadSwitch(void);           /* GSthread yield */
extern void  fn_800E01D0(void* dst, void* src);
extern void  fn_80109220(u32 obj, u8 visible);
extern void  fn_800FB680(u32 a, u32 b, s32 c, u32 d);
extern void  fn_800D61E4(void* obj);
extern void  GScameraSetPerspective(void* obj);
extern void  fn_80166AB8(u32 seId);        /* Sound SE play */

/* ===== Text / Messages ===== */
extern void* fn_8001E224(u32 msgBank, u32 msgId);
extern u32   fn_8001E200(u32 msgBank, u32 msgId);

/* ===== Msgbox system (internal) ===== */
extern void  fn_800566E8(void);
extern void  fn_800573C0(void);
extern void  fn_80057094(void);
extern void  fn_80057400(u32 a);
extern void  fn_800574A8(u32 a);
extern void  fn_800574E0(u32 a);
extern void  fn_80057694(void);
extern void  fn_80057830(void);

/* ===== Status / script (internal) ===== */
extern void  fn_80055B98(void);
extern void  fn_8005464C(void);
extern void  fn_80054670(void);
extern void  fn_8005471C(u32 a);
extern void  fn_80054760(void);
extern void  fn_80055194(void);

/* ===== Sound ===== */
extern void  fn_800CE148(u32 sndId);       /* Sound play (SDK) */

/* ===== BSS data ===== */
extern u8    lbl_803A9A08[];   /* Dialog window state A */
extern u8    lbl_803A9A18[];   /* Dialog window state B */

/*
 * Functions in this translation unit (19 total):
 *
 * fn_80057B34  0x168  Dialog window open/transition (lbl_803A9A08)
 * fn_80057C9C  0x14C  Dialog window close/transition (lbl_803A9A08)
 * fn_80057DE8  0x058  Dialog state set (called from many modules, no BSS)
 * fn_80057E40  0x030  Dialog show (lbl_803A9A08, called from many modules)
 * fn_80057E70  0x124  Dialog display update (lbl_803A9A08)
 * fn_80057F94  0x1BC  Dialog update main (no BSS, called from many modules)
 * fn_80058150  0x604  Dialog main loop (lbl_803A9A08, calls fn_80055B98)
 * fn_80058754  0x044  Dialog utility A (lbl_803A9A18)
 * fn_80058798  0x040  Dialog utility B
 * fn_800587D8  0x02C  Dialog utility C
 * fn_80058804  0x07C  Dialog text setup (lbl_803A9A18, called 23x)
 * fn_80058880  0x230  Dialog text renderer (calls fn_800566E8, fn_800573C0, fn_80057694)
 * fn_80058AB0  0x040  Dialog text helper (calls fn_800573C0)
 * fn_80058AF0  0x2DC  Dialog text layout (calls fn_80057400, fn_800574A8, fn_800574E0)
 * fn_80058DCC  0x13C  Dialog scroll handler (calls fn_80057830, fn_80058AF0)
 * fn_80058F08  0x038  Dialog state query A
 * fn_80058F40  0x0F4  Dialog state query B
 * fn_80059034  0x978  Choice selection handler (2424 bytes, calls fn_8005464C-fn_8005471C)
 * fn_800599AC  0x230  Choice selection wrapper (calls fn_80059034)
 */


/* 0x80057B34 | size: 0x168 */
#pragma peephole off
u32 fn_80057B34(u32 arg0, u32 arg1) {
    u32 result;
    u8 flag;
    s32* state;

    flag = 0;
    state = (s32*)lbl_803A9A08;

    if ((state[0] != 0 ? (state[1] != 0 ? 1 : 0) : 2) != 2) {
        state = (s32*)lbl_803A9A08;
        flag = 1;
        state[2] = 1;
        while ((state[0] != 0 ? (state[1] != 0 ? 1 : 0) : 2) != 1) {
            _threadSwitch();
        }
    }

    fn_8017B1CC(0x70B);
    fn_800F915C(0x70B);
    fn_8017B3E4(0x670);
    while (fn_8017B2CC(0x670) == 1) {
        _threadSwitch();
    }

    result = fn_80029558(arg0, arg1);
    fn_8017B1CC(0x670);
    fn_800F915C(0x670);
    fn_8017B3E4(0x70B);
    while (fn_8017B2CC(0x70B) == 1) {
        _threadSwitch();
    }

    if (flag != 0) {
        ((s32*)lbl_803A9A08)[2] = 0;
    }
    return result;
}
#pragma peephole on

/* 0x80059034 | size: 0x978 */
asm void fn_80059034(void) { nofralloc
    #include "asm/GC6E01/nonmatching/menu_dialog/fn_80059034.s"
}

