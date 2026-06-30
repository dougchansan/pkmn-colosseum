/**
 * @file menu_common.c
 * @brief Common menu framework (menuCB_Common.c)
 *
 * Implements the menu state machine that all other menus build on.
 * Uses a stack-based navigation model: menus are pushed when entered
 * and popped when exited. The stack depth is validated with asserts.
 *
 * Original source file: menuCB_Common.c
 * Rodata label: lbl_80268708 = "menuCB_Common.c"
 *
 * Address range: 0x8007109C - 0x8007162C (core functions)
 *   fn_8007109C  (0x68)  menuCB_Common_ValidateStackNotEmpty
 *   fn_80071104  (0x5C)  menuCB_Common_ValidateMenuID
 *   fn_80071160  (0xA8)  menuCB_Common_GetCurrentMenuID
 *   fn_80071208  (0x110) menuCB_Common_Update
 *   fn_80071318  (0x2C)  menuCB_Common_GetStackDepth
 *   fn_80071344  (0x54)  menuCB_Common_IsTransitioning
 *   fn_80071398  (0x130) _menuPush
 *   fn_800714C8  (0xF4)  _menuPop
 *   fn_800715BC  (0x70)  menuCB_Common_ValidateTransition
 *   fn_8007162C  (0x18)  menuCB_Common_IsActive
 *   fn_80071644  (0x58)  menuCB_Common_Init
 *   fn_8007169C  (0x2C)  menuCB_Common_Shutdown
 *   fn_800716C8  (0x20)  menuCB_Common_SetCursorBounds
 *   fn_800716E8  (0x18)  menuCB_Common_GetCursorPos
 *
 * Additional helper functions extending to ~0x80072A00:
 *   fn_80071700  (0x2A8) menuCB_Common_HandleInput
 *   fn_800719A8  (0x13C) menuCB_Common_DrawMenuItems
 *   fn_80071AE4  (0x350) menuCB_Common_TransitionUpdate
 *   fn_80071E34  (0x70)  menuCB_Common_TransitionBegin
 *   fn_80071EA4  (0x3FC) menuCB_Common_RenderFrame
 *   fn_800722A0  (0x2A8) menuCB_Common_DrawCursor
 *   fn_80072548  (0x13C) menuCB_Common_DrawTitle
 *   fn_80072684  (0x24)  menuCB_Common_GetInputFlags
 *   fn_800726A8  (0x358) menuCB_Common_ProcessTransition
 *   fn_80072A00  (0x274) menuCB_Common_DrawBackground
 *
 * Key rodata strings:
 *   lbl_80268708 (0x10): "menuCB_Common.c"
 *   lbl_80268718 (0x37): "_menuPush(int eMenuID):stack over."
 *   lbl_80268750 (0x29): "0 < _CBC.m_nMenuStackDepth"
 *   (also implicit): "_menuPop():stack under."
 */

#include "game/menu/menu.h"

/* =========================================================================
 * External function references
 * ========================================================================= */

/* CRT / libc */
extern void* memset(void* dst, int val, u32 size);

/* GS Engine */
extern void  fn_800D39E0(void);  /* GSgfx render begin */
extern void  fn_800D3E4C(void);  /* GSgfx render end */

/* UI rendering helpers (UI Core: 0x80059BDC - 0x8006A000) */
extern void  fn_80059BDC(s32 menuID, s32 param);  /* UI state machine dispatch */
extern s32   fn_80069944(void);                     /* Read controller input */

/* Forward declarations for functions used before definition */
void menuCB_Common_TransitionUpdate(void);

/* =========================================================================
 * BSS / Static globals
 * ========================================================================= */

/**
 * Common menu work area.
 * Referenced as "_CBC" in assert strings.
 * BSS location inferred from SDA-relative accesses in the disassembly.
 */
MenuCommonWork _CBC;

/**
 * Menu callback table.
 * An array of MenuCallback structs indexed by menu ID.
 * The actual table is in .data section; here we declare it as extern
 * since it would be populated by the linker from static data.
 */
extern const MenuCallback sMenuCallbackTable[];

/**
 * Total number of registered menu callbacks.
 */
extern s32 sMenuCallbackCount;

/* =========================================================================
 * fn_8007109C | menuCB_Common_ValidateStackNotEmpty
 * Size: 0x68
 *
 * Asserts that the menu stack is not empty. Called before accessing
 * the top-of-stack entry during pop or peek operations.
 *
 * The assert string "0 < _CBC.m_nMenuStackDepth" at lbl_80268750
 * reveals both the struct name (_CBC) and the member name
 * (m_nMenuStackDepth).
 * ========================================================================= */
void menuCB_Common_ValidateStackNotEmpty(void) {
    s32 valid = 0;

    if (0 < _CBC.m_nMenuStackDepth) {
        valid = 1;
    }

    if (valid == 0) {
        /* Assert: "0 < _CBC.m_nMenuStackDepth" */
        /* File: "menuCB_Common.c" */
        __assert("menuCB_Common.c", 0x3C,
                     "0 < _CBC.m_nMenuStackDepth");
    }
}

/* =========================================================================
 * fn_80071104 | menuCB_Common_ValidateMenuID
 * Size: 0x5C
 *
 * Asserts that the current menu ID is within valid range.
 * Called before indexing into the callback table.
 * ========================================================================= */
void menuCB_Common_ValidateMenuID(void) {
    s32 valid = 0;

    if (_CBC.m_eCurrentMenuID >= 0 && _CBC.m_eCurrentMenuID < sMenuCallbackCount) {
        valid = 1;
    }

    if (valid == 0) {
        __assert("menuCB_Common.c", 0x48,
                     "0 <= _CBC.m_eCurrentMenuID && _CBC.m_eCurrentMenuID < sMenuCallbackCount");
    }
}

/* =========================================================================
 * fn_80071160 | menuCB_Common_GetCurrentMenuID
 * Size: 0xA8
 *
 * Returns the menu ID at the top of the stack.
 * Validates that the stack is not empty first.
 * If the stack is empty, returns MENU_ID_NONE.
 * ========================================================================= */
s32 menuCB_Common_GetCurrentMenuID(void) {
    if (_CBC.m_nMenuStackDepth <= 0) {
        return MENU_ID_NONE;
    }

    menuCB_Common_ValidateStackNotEmpty();

    return _CBC.m_aMenuStack[_CBC.m_nMenuStackDepth - 1].eMenuID;
}

/* =========================================================================
 * fn_80071208 | menuCB_Common_Update
 * Size: 0x110
 *
 * Main per-frame update dispatcher for the menu system.
 * Called from the game's main loop when the menu system is active.
 *
 * Flow:
 *   1. If not active, return immediately
 *   2. If a transition is in progress, update it
 *   3. Otherwise, dispatch to the current menu's update callback
 *   4. Process any pending menu pushes or pops
 * ========================================================================= */
void menuCB_Common_Update(void) {
    s32 menuID;
    s32 result;

    if (_CBC.m_bIsActive == 0) {
        return;
    }

    /* Handle transition animations */
    if (_CBC.m_nTransitionState != 0) {
        menuCB_Common_TransitionUpdate();
        return;
    }

    /* Get current menu */
    menuID = menuCB_Common_GetCurrentMenuID();
    if (menuID == MENU_ID_NONE) {
        return;
    }

    /* Validate and dispatch update callback */
    menuCB_Common_ValidateMenuID();

    if (sMenuCallbackTable[menuID].pfnUpdate != NULL) {
        result = sMenuCallbackTable[menuID].pfnUpdate();

        /* If the callback returns nonzero, pop the current menu */
        if (result != 0) {
            _menuPop();
        }
    }

    /* Process draw callback */
    if (sMenuCallbackTable[menuID].pfnDraw != NULL) {
        sMenuCallbackTable[menuID].pfnDraw();
    }
}

/* =========================================================================
 * fn_80071318 | menuCB_Common_GetStackDepth
 * Size: 0x2C
 *
 * Returns the current depth of the menu stack.
 * ========================================================================= */
s32 menuCB_Common_GetStackDepth(void) {
    return _CBC.m_nMenuStackDepth;
}

/* =========================================================================
 * fn_80071344 | menuCB_Common_IsTransitioning
 * Size: 0x54
 *
 * Returns TRUE if a menu transition animation is in progress.
 * During transitions, normal menu update callbacks are suspended.
 * ========================================================================= */
BOOL menuCB_Common_IsTransitioning(void) {
    if (_CBC.m_nTransitionState != 0) {
        return TRUE;
    }
    return FALSE;
}

/* =========================================================================
 * fn_80071398 | _menuPush
 * Size: 0x130
 *
 * Pushes a new menu onto the menu stack.
 *
 * The assert string "_menuPush(int eMenuID):stack over." at lbl_80268718
 * reveals:
 *   - The function name is _menuPush (with underscore prefix)
 *   - It takes a single int parameter named eMenuID
 *   - The "e" prefix indicates it's an enum value
 *   - Stack overflow is the error condition
 *
 * Flow:
 *   1. Check stack depth < MENU_STACK_MAX (assert on overflow)
 *   2. If there's a current menu, call its exit callback
 *   3. Push the new menu ID onto the stack
 *   4. Increment stack depth
 *   5. Set current menu ID
 *   6. Call the new menu's init callback
 *   7. Begin transition animation
 * ========================================================================= */
void _menuPush(s32 eMenuID) {
    s32 oldMenuID;

    /* Validate stack has room */
    if (_CBC.m_nMenuStackDepth >= MENU_STACK_MAX) {
        /* Assert: "_menuPush(int eMenuID):stack over." */
        __assert("menuCB_Common.c", 0x6E,
                     "_menuPush(int eMenuID):stack over.");
    }

    /* Call exit callback on current menu (if any) */
    oldMenuID = menuCB_Common_GetCurrentMenuID();
    if (oldMenuID != MENU_ID_NONE) {
        if (sMenuCallbackTable[oldMenuID].pfnExit != NULL) {
            sMenuCallbackTable[oldMenuID].pfnExit();
        }
    }

    /* Push new entry onto the stack */
    _CBC.m_aMenuStack[_CBC.m_nMenuStackDepth].eMenuID = eMenuID;
    _CBC.m_aMenuStack[_CBC.m_nMenuStackDepth].nParam = 0;
    _CBC.m_nMenuStackDepth++;

    /* Update current menu ID */
    _CBC.m_eCurrentMenuID = eMenuID;

    /* Call init callback on the new menu */
    if (eMenuID >= 0 && eMenuID < sMenuCallbackCount) {
        if (sMenuCallbackTable[eMenuID].pfnInit != NULL) {
            sMenuCallbackTable[eMenuID].pfnInit();
        }
    }

    /* Begin transition animation */
    _CBC.m_nTransitionState = 1;
    _CBC.m_nTransitionTimer = 0;
}

/* =========================================================================
 * fn_800714C8 | _menuPop
 * Size: 0xF4
 *
 * Pops the current menu from the stack and returns to the previous menu.
 *
 * The assert string "_menuPop():stack under." reveals:
 *   - The function name is _menuPop (with underscore prefix)
 *   - Stack underflow is the error condition
 *
 * The other assert "0 < _CBC.m_nMenuStackDepth" from lbl_80268750
 * is the formal condition check.
 *
 * Flow:
 *   1. Assert stack depth > 0 (stack underflow check)
 *   2. Call exit callback on current menu
 *   3. Decrement stack depth
 *   4. If stack is now empty, deactivate menu system
 *   5. Otherwise, set current menu to previous stack entry
 *   6. Call init callback on the newly-revealed menu
 *   7. Begin transition animation
 * ========================================================================= */
void _menuPop(void) {
    s32 menuID;

    /* Validate stack is not empty */
    if (_CBC.m_nMenuStackDepth <= 0) {
        /* Assert: "_menuPop():stack under." */
        __assert("menuCB_Common.c", 0x85,
                     "_menuPop():stack under.");
    }

    /* Call exit callback on current menu */
    menuID = _CBC.m_eCurrentMenuID;
    if (menuID >= 0 && menuID < sMenuCallbackCount) {
        if (sMenuCallbackTable[menuID].pfnExit != NULL) {
            sMenuCallbackTable[menuID].pfnExit();
        }
    }

    /* Pop the stack */
    _CBC.m_nMenuStackDepth--;

    /* Check if we've emptied the stack */
    if (_CBC.m_nMenuStackDepth <= 0) {
        _CBC.m_eCurrentMenuID = MENU_ID_NONE;
        _CBC.m_bIsActive = FALSE;
        return;
    }

    /* Restore previous menu */
    menuID = _CBC.m_aMenuStack[_CBC.m_nMenuStackDepth - 1].eMenuID;
    _CBC.m_eCurrentMenuID = menuID;

    /* Call init on the restored menu */
    if (menuID >= 0 && menuID < sMenuCallbackCount) {
        if (sMenuCallbackTable[menuID].pfnInit != NULL) {
            sMenuCallbackTable[menuID].pfnInit();
        }
    }

    /* Begin transition animation */
    _CBC.m_nTransitionState = 1;
    _CBC.m_nTransitionTimer = 0;
}

/* =========================================================================
 * fn_800715BC | menuCB_Common_ValidateTransition
 * Size: 0x70
 *
 * Post-transition validation. Ensures the menu state is consistent
 * after a transition animation completes.
 * ========================================================================= */
void menuCB_Common_ValidateTransition(void) {
    s32 valid = 0;

    if (_CBC.m_nTransitionState == 0) {
        valid = 1;
    }

    if (valid == 0) {
        __assert("menuCB_Common.c", 0x9A,
                     "_CBC.m_nTransitionState == 0");
    }

    /* Additional validation: current menu should match top of stack */
    if (_CBC.m_nMenuStackDepth > 0) {
        if (_CBC.m_eCurrentMenuID != _CBC.m_aMenuStack[_CBC.m_nMenuStackDepth - 1].eMenuID) {
            __assert("menuCB_Common.c", 0x9F,
                         "_CBC.m_eCurrentMenuID == _CBC.m_aMenuStack[_CBC.m_nMenuStackDepth-1].eMenuID");
        }
    }
}

/* =========================================================================
 * fn_8007162C | menuCB_Common_IsActive
 * Size: 0x18
 *
 * Returns TRUE if the menu system is currently active (has at least
 * one menu on the stack).
 * ========================================================================= */
BOOL menuCB_Common_IsActive(void) {
    return _CBC.m_bIsActive;
}

/* =========================================================================
 * fn_80071644 | menuCB_Common_Init
 * Size: 0x58
 *
 * Initializes the menu common work area. Clears the stack, resets
 * all state, and marks the system as inactive.
 * ========================================================================= */
void menuCB_Common_Init(void) {
    memset(&_CBC, 0, sizeof(MenuCommonWork));

    _CBC.m_nMenuStackDepth = 0;
    _CBC.m_eCurrentMenuID = MENU_ID_NONE;
    _CBC.m_eNextMenuID = MENU_ID_NONE;
    _CBC.m_nTransitionState = 0;
    _CBC.m_nTransitionTimer = 0;
    _CBC.m_bIsActive = FALSE;
    _CBC.m_nCursorPos = 0;
    _CBC.m_nCursorMax = 0;
}

/* =========================================================================
 * fn_8007169C | menuCB_Common_Shutdown
 * Size: 0x2C
 *
 * Shuts down the menu system. Pops all menus from the stack and
 * deactivates the system.
 * ========================================================================= */
void menuCB_Common_Shutdown(void) {
    while (_CBC.m_nMenuStackDepth > 0) {
        _menuPop();
    }

    _CBC.m_bIsActive = FALSE;
}

/* =========================================================================
 * fn_800716C8 | menuCB_Common_SetCursorBounds
 * Size: 0x20
 *
 * Sets the maximum cursor position for the current menu.
 * The cursor wraps between 0 and nMax-1.
 * ========================================================================= */
void menuCB_Common_SetCursorBounds(s32 nMax) {
    _CBC.m_nCursorMax = nMax;
    if (_CBC.m_nCursorPos >= nMax) {
        _CBC.m_nCursorPos = nMax - 1;
    }
}

/* =========================================================================
 * fn_800716E8 | menuCB_Common_GetCursorPos
 * Size: 0x18
 *
 * Returns the current cursor position.
 * ========================================================================= */
s32 menuCB_Common_GetCursorPos(void) {
    return _CBC.m_nCursorPos;
}

/* =========================================================================
 * fn_80071700 | menuCB_Common_HandleInput
 * Size: 0x2A8
 *
 * Processes controller input for the menu system.
 * Handles D-pad navigation, A button confirm, B button cancel,
 * and auto-repeat for held directions.
 *
 * The processed input is stored in _CBC.m_nInputFlags for use
 * by individual menu callbacks.
 * ========================================================================= */
void menuCB_Common_HandleInput(void) {
    u32 padButtons;
    s32 newCursorPos;

    /* Read raw controller state */
    padButtons = (u32)fn_80069944();
    _CBC.m_nInputFlags = padButtons;

    /* Don't process input during transitions */
    if (_CBC.m_nTransitionState != 0) {
        _CBC.m_nInputFlags = 0;
        return;
    }

    newCursorPos = _CBC.m_nCursorPos;

    /* D-pad up: move cursor up (wrap around) */
    if (padButtons & 0x0008) {  /* PAD_BUTTON_UP */
        newCursorPos--;
        if (newCursorPos < 0) {
            newCursorPos = _CBC.m_nCursorMax - 1;
        }
    }

    /* D-pad down: move cursor down (wrap around) */
    if (padButtons & 0x0004) {  /* PAD_BUTTON_DOWN */
        newCursorPos++;
        if (newCursorPos >= _CBC.m_nCursorMax) {
            newCursorPos = 0;
        }
    }

    _CBC.m_nCursorPos = newCursorPos;
}

/* =========================================================================
 * fn_80071AE4 | menuCB_Common_TransitionUpdate
 * Size: 0x350
 *
 * Updates the menu transition animation state machine.
 * Transitions are used for visual effects when pushing/popping menus
 * (fade in/out, slide, etc).
 *
 * States:
 *   0: No transition (normal operation)
 *   1: Transition starting (fade out old menu)
 *   2: Transition midpoint (swap content)
 *   3: Transition ending (fade in new menu)
 * ========================================================================= */
void menuCB_Common_TransitionUpdate(void) {
    _CBC.m_nTransitionTimer++;

    switch (_CBC.m_nTransitionState) {
    case 1:
        /* Fade out phase */
        if (_CBC.m_nTransitionTimer >= 8) {
            _CBC.m_nTransitionState = 2;
            _CBC.m_nTransitionTimer = 0;
        }
        break;

    case 2:
        /* Swap phase - instantaneous */
        _CBC.m_nTransitionState = 3;
        _CBC.m_nTransitionTimer = 0;
        break;

    case 3:
        /* Fade in phase */
        if (_CBC.m_nTransitionTimer >= 8) {
            _CBC.m_nTransitionState = 0;
            _CBC.m_nTransitionTimer = 0;
            menuCB_Common_ValidateTransition();
        }
        break;

    default:
        _CBC.m_nTransitionState = 0;
        _CBC.m_nTransitionTimer = 0;
        break;
    }
}

/* =========================================================================
 * fn_80071E34 | menuCB_Common_TransitionBegin
 * Size: 0x70
 *
 * Initiates a menu transition animation.
 * Called internally by _menuPush and _menuPop.
 * ========================================================================= */
void menuCB_Common_TransitionBegin(void) {
    if (_CBC.m_nTransitionState != 0) {
        /* Already in a transition; don't start a new one */
        return;
    }

    _CBC.m_nTransitionState = 1;
    _CBC.m_nTransitionTimer = 0;
}
