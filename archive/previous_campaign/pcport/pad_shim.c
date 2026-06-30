/**
 * @file pad_shim.c
 * @brief PAD API replacement -- SDL2 GameController backend stubs.
 *
 * References:
 *   - docs/pc_port_design.md Section 10 (Input Replacement)
 *   - pad_shim.h for full API documentation
 *
 * Phase 3 PC port scaffolding -- skeleton only.
 */

#ifdef __MWERKS__
/* GCN build: pcport shim not applicable */
#else

#include "pad_shim.h"
#include "pcport_window.h"

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>

/* Keyboard input is read directly from GLFW (the build links GLFW, not SDL2).
 * glfwPollEvents() is pumped every frame in VIWaitForRetrace_PC (os_shim.c), so
 * glfwGetKey() returns fresh per-frame state with no extra polling here.
 * The g_keymap fields hold GLFW key codes (GLFW_KEY_*), set in PADShim_Init. */

/* =========================================================================
 * Internal state
 * ========================================================================= */

/** SDL GameController handles for up to 4 ports */
static void* g_controllers[4] = { 0, 0, 0, 0 };

/** Whether keyboard input is enabled */
static BOOL g_keyboardEnabled = 0;

/** Whether the pad system is initialized */
static BOOL g_padInitialized = 0;

/** Keyboard mapping (SDL scancodes) */
static struct {
    u32 keyA, keyB, keyX, keyY;
    u32 keyStart, keyZ, keyL, keyR;
    u32 keyUp, keyDown, keyLeft, keyRight;
    u32 keyStickUp, keyStickDown, keyStickLeft, keyStickRight;
    u32 keyCStickUp, keyCStickDown, keyCStickLeft, keyCStickRight;
} g_keymap;

/* =========================================================================
 * PAD SDK function implementations
 * ========================================================================= */

void PADInit(void) {
    PADShim_Init(1); /* Enable keyboard by default */
}

void PADRead(PADStatus status[4]) {
    s32 i;

    /* Clear all status entries */
    memset(status, 0, sizeof(PADStatus) * 4);

    /* TODO: Phase 3a -- Read controllers via SDL2
     *
     * SDL_GameControllerUpdate();
     *
     * for (int i = 0; i < 4; i++) {
     *     SDL_GameController* gc = (SDL_GameController*)g_controllers[i];
     *     if (gc == NULL || !SDL_GameControllerGetAttached(gc)) {
     *         status[i].err = -1; // not connected
     *         continue;
     *     }
     *     status[i].err = 0;
     *
     *     // Map buttons
     *     u16 buttons = 0;
     *     if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_A))
     *         buttons |= GCN_PAD_BUTTON_A;
     *     if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_B))
     *         buttons |= GCN_PAD_BUTTON_B;
     *     if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_X))
     *         buttons |= GCN_PAD_BUTTON_X;
     *     if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_Y))
     *         buttons |= GCN_PAD_BUTTON_Y;
     *     if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_START))
     *         buttons |= GCN_PAD_BUTTON_START;
     *     if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
     *         buttons |= GCN_PAD_TRIGGER_Z;
     *     if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_UP))
     *         buttons |= GCN_PAD_BUTTON_UP;
     *     if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
     *         buttons |= GCN_PAD_BUTTON_DOWN;
     *     if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
     *         buttons |= GCN_PAD_BUTTON_LEFT;
     *     if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
     *         buttons |= GCN_PAD_BUTTON_RIGHT;
     *
     *     // Analog triggers -> digital trigger buttons
     *     s16 trigL = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
     *     s16 trigR = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
     *     if (trigL > 16000) buttons |= GCN_PAD_TRIGGER_L;
     *     if (trigR > 16000) buttons |= GCN_PAD_TRIGGER_R;
     *
     *     status[i].button = buttons;
     *
     *     // Analog sticks (scale from [-32768,32767] to [-128,127])
     *     status[i].stickX = (s8)(SDL_GameControllerGetAxis(gc,
     *         SDL_CONTROLLER_AXIS_LEFTX) >> 8);
     *     status[i].stickY = (s8)(-(SDL_GameControllerGetAxis(gc,
     *         SDL_CONTROLLER_AXIS_LEFTY) >> 8)); // Y axis inverted
     *     status[i].substickX = (s8)(SDL_GameControllerGetAxis(gc,
     *         SDL_CONTROLLER_AXIS_RIGHTX) >> 8);
     *     status[i].substickY = (s8)(-(SDL_GameControllerGetAxis(gc,
     *         SDL_CONTROLLER_AXIS_RIGHTY) >> 8));
     *
     *     // Analog triggers (scale from [0,32767] to [0,255])
     *     status[i].triggerLeft = (u8)(trigL >> 7);
     *     status[i].triggerRight = (u8)(trigR >> 7);
     * }
     */

    /* Mark all controllers as not connected (stub behavior) */
    for (i = 0; i < 4; i++) {
        if (g_controllers[i] != 0) {
            status[i].err = 0;
        } else {
            status[i].err = -1;
        }
    }

    /* Apply keyboard input to controller 0 if enabled */
    if (g_keyboardEnabled) {
        PADShim_UpdateKeyboard(&status[0]);
    }
}

u32 PADReset(u32 mask) {
    (void)mask;

    /* TODO: Phase 3a -- Re-open SDL controllers if needed
     *
     * for (int i = 0; i < 4; i++) {
     *     if (mask & (1 << i)) {
     *         if (g_controllers[i]) {
     *             SDL_GameControllerClose((SDL_GameController*)g_controllers[i]);
     *             g_controllers[i] = NULL;
     *         }
     *         // Re-open if available
     *         if (SDL_IsGameController(i)) {
     *             g_controllers[i] = SDL_GameControllerOpen(i);
     *         }
     *     }
     * }
     */
    return 0;
}

u32 PADRecalibrate(u32 mask) {
    (void)mask;
    /* No-op on PC -- SDL handles calibration automatically. */
    return 0;
}

void PADControlMotor(u32 chan, u32 cmd) {
    if (chan >= 4) return;

    /* TODO: Phase 3a -- Control rumble via SDL
     *
     * SDL_GameController* gc = (SDL_GameController*)g_controllers[chan];
     * if (gc == NULL) return;
     *
     * switch (cmd) {
     *     case PAD_MOTOR_STOP:
     *         SDL_GameControllerRumble(gc, 0, 0, 0);
     *         break;
     *     case PAD_MOTOR_RUMBLE:
     *         SDL_GameControllerRumble(gc, 0xFFFF, 0xFFFF, 100);
     *         break;
     *     case PAD_MOTOR_BRAKE:
     *         SDL_GameControllerRumble(gc, 0x4000, 0x4000, 50);
     *         break;
     * }
     */

    (void)cmd;
}

u32 SIGetType(s32 chan) {
    if (chan < 0 || chan >= 4) return 0;

    /* TODO: Phase 3a -- Check controller type
     *
     * if (g_controllers[chan] != NULL) {
     *     return SI_TYPE_GC_CONTROLLER_MOTOR; // 0x09000000
     * }
     * return 0;
     */

    if (g_controllers[chan] != 0)
        return SI_TYPE_GC_CONTROLLER_MOTOR;
    return 0;
}

/* =========================================================================
 * Extended PC-specific API
 * ========================================================================= */

void PADShim_Init(BOOL enableKeyboard) {
    s32 i;

    memset(g_controllers, 0, sizeof(g_controllers));
    g_keyboardEnabled = enableKeyboard;

    /* Default keyboard mapping (from pc_port_design.md Section 10.4), expressed
     * as GLFW key codes. Face/trigger buttons: Z=A, X=B, A=X, S=Y, Enter=Start,
     * Space=Z, Q/E=L/R. D-pad on the arrow keys, main stick on WASD, C-stick on
     * IJKL. (W/A/S/D deliberately double as the stick per the GC default map; the
     * menu flow drives off the D-pad arrows so the overlap is harmless.) */
    memset(&g_keymap, 0, sizeof(g_keymap));
    g_keymap.keyA = GLFW_KEY_Z;
    g_keymap.keyB = GLFW_KEY_X;
    g_keymap.keyX = GLFW_KEY_A;
    g_keymap.keyY = GLFW_KEY_S;
    g_keymap.keyStart = GLFW_KEY_ENTER;
    g_keymap.keyZ = GLFW_KEY_SPACE;
    g_keymap.keyL = GLFW_KEY_Q;
    g_keymap.keyR = GLFW_KEY_E;
    g_keymap.keyUp = GLFW_KEY_UP;
    g_keymap.keyDown = GLFW_KEY_DOWN;
    g_keymap.keyLeft = GLFW_KEY_LEFT;
    g_keymap.keyRight = GLFW_KEY_RIGHT;
    g_keymap.keyStickUp = GLFW_KEY_W;
    g_keymap.keyStickDown = GLFW_KEY_S;
    g_keymap.keyStickLeft = GLFW_KEY_A;
    g_keymap.keyStickRight = GLFW_KEY_D;
    g_keymap.keyCStickUp = GLFW_KEY_I;
    g_keymap.keyCStickDown = GLFW_KEY_K;
    g_keymap.keyCStickLeft = GLFW_KEY_J;
    g_keymap.keyCStickRight = GLFW_KEY_L;

    /* TODO: Phase 3a -- Initialize SDL GameController subsystem
     *
     * if (SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0) {
     *     printf("[pad_shim] SDL_Init failed: %s\n", SDL_GetError());
     * }
     *
     * // Open all connected controllers (up to 4)
     * int numJoysticks = SDL_NumJoysticks();
     * int controllerIdx = 0;
     * for (int i = 0; i < numJoysticks && controllerIdx < 4; i++) {
     *     if (SDL_IsGameController(i)) {
     *         g_controllers[controllerIdx] = SDL_GameControllerOpen(i);
     *         if (g_controllers[controllerIdx]) {
     *             printf("[pad_shim] Controller %d: %s\n", controllerIdx,
     *                    SDL_GameControllerName(g_controllers[controllerIdx]));
     *             controllerIdx++;
     *         }
     *     }
     * }
     */

    g_padInitialized = 1;

    (void)i;
    printf("[pad_shim] PADShim_Init stub (keyboard=%d)\n", enableKeyboard);
}

void PADShim_Shutdown(void) {
    /* TODO: Phase 3a -- Close all controllers
     *
     * for (int i = 0; i < 4; i++) {
     *     if (g_controllers[i]) {
     *         SDL_GameControllerClose((SDL_GameController*)g_controllers[i]);
     *         g_controllers[i] = NULL;
     *     }
     * }
     * SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
     */

    memset(g_controllers, 0, sizeof(g_controllers));
    g_padInitialized = 0;
}

void PADShim_SetKeyboardMapping(u32 keyA, u32 keyB, u32 keyX, u32 keyY,
                                u32 keyStart, u32 keyZ, u32 keyL, u32 keyR) {
    g_keymap.keyA = keyA;
    g_keymap.keyB = keyB;
    g_keymap.keyX = keyX;
    g_keymap.keyY = keyY;
    g_keymap.keyStart = keyStart;
    g_keymap.keyZ = keyZ;
    g_keymap.keyL = keyL;
    g_keymap.keyR = keyR;
}

void PADShim_UpdateKeyboard(PADStatus* status) {
    GLFWwindow* window;
    u16 buttons;
    s8 sx;
    s8 sy;
    s8 cx;
    s8 cy;

    if (!status) return;

    /* Keyboard maps to player 1, which is treated as always "connected". */
    status->err = 0;

    window = PCPort_GetHostWindow();
    if (window == NULL) {
        return; /* headless: no host window, so no key state to read */
    }

    buttons = 0;
    if (glfwGetKey(window, (int)g_keymap.keyA) == GLFW_PRESS)     buttons |= GCN_PAD_BUTTON_A;
    if (glfwGetKey(window, (int)g_keymap.keyB) == GLFW_PRESS)     buttons |= GCN_PAD_BUTTON_B;
    if (glfwGetKey(window, (int)g_keymap.keyX) == GLFW_PRESS)     buttons |= GCN_PAD_BUTTON_X;
    if (glfwGetKey(window, (int)g_keymap.keyY) == GLFW_PRESS)     buttons |= GCN_PAD_BUTTON_Y;
    if (glfwGetKey(window, (int)g_keymap.keyStart) == GLFW_PRESS) buttons |= GCN_PAD_BUTTON_START;
    if (glfwGetKey(window, (int)g_keymap.keyZ) == GLFW_PRESS)     buttons |= GCN_PAD_TRIGGER_Z;
    if (glfwGetKey(window, (int)g_keymap.keyL) == GLFW_PRESS)     buttons |= GCN_PAD_TRIGGER_L;
    if (glfwGetKey(window, (int)g_keymap.keyR) == GLFW_PRESS)     buttons |= GCN_PAD_TRIGGER_R;

    /* D-pad from arrow keys */
    if (glfwGetKey(window, (int)g_keymap.keyUp) == GLFW_PRESS)    buttons |= GCN_PAD_BUTTON_UP;
    if (glfwGetKey(window, (int)g_keymap.keyDown) == GLFW_PRESS)  buttons |= GCN_PAD_BUTTON_DOWN;
    if (glfwGetKey(window, (int)g_keymap.keyLeft) == GLFW_PRESS)  buttons |= GCN_PAD_BUTTON_LEFT;
    if (glfwGetKey(window, (int)g_keymap.keyRight) == GLFW_PRESS) buttons |= GCN_PAD_BUTTON_RIGHT;

    status->button |= buttons;

    /* Main stick from WASD */
    sx = 0;
    sy = 0;
    if (glfwGetKey(window, (int)g_keymap.keyStickLeft) == GLFW_PRESS)  sx = (s8)(sx - 80);
    if (glfwGetKey(window, (int)g_keymap.keyStickRight) == GLFW_PRESS) sx = (s8)(sx + 80);
    if (glfwGetKey(window, (int)g_keymap.keyStickUp) == GLFW_PRESS)    sy = (s8)(sy + 80);
    if (glfwGetKey(window, (int)g_keymap.keyStickDown) == GLFW_PRESS)  sy = (s8)(sy - 80);
    if (sx != 0 || sy != 0) {
        status->stickX = sx;
        status->stickY = sy;
    }

    /* C-stick from IJKL */
    cx = 0;
    cy = 0;
    if (glfwGetKey(window, (int)g_keymap.keyCStickLeft) == GLFW_PRESS)  cx = (s8)(cx - 80);
    if (glfwGetKey(window, (int)g_keymap.keyCStickRight) == GLFW_PRESS) cx = (s8)(cx + 80);
    if (glfwGetKey(window, (int)g_keymap.keyCStickUp) == GLFW_PRESS)    cy = (s8)(cy + 80);
    if (glfwGetKey(window, (int)g_keymap.keyCStickDown) == GLFW_PRESS)  cy = (s8)(cy - 80);
    if (cx != 0 || cy != 0) {
        status->substickX = cx;
        status->substickY = cy;
    }

    /* Keyboard L/R -> full digital trigger press (255). */
    if (buttons & GCN_PAD_TRIGGER_L) status->triggerLeft = 255;
    if (buttons & GCN_PAD_TRIGGER_R) status->triggerRight = 255;
}

void PADShim_HandleControllerEvent(u32 eventType, s32 deviceId) {
    (void)eventType; (void)deviceId;

    /* TODO: Phase 3a -- Handle controller hotplug
     *
     * if (eventType == SDL_CONTROLLERDEVICEADDED) {
     *     // Find an empty slot and open the controller
     *     for (int i = 0; i < 4; i++) {
     *         if (g_controllers[i] == NULL) {
     *             g_controllers[i] = SDL_GameControllerOpen(deviceId);
     *             printf("[pad_shim] Controller added to port %d\n", i);
     *             break;
     *         }
     *     }
     * } else if (eventType == SDL_CONTROLLERDEVICEREMOVED) {
     *     // Find and close the removed controller
     *     for (int i = 0; i < 4; i++) {
     *         SDL_GameController* gc = (SDL_GameController*)g_controllers[i];
     *         if (gc && SDL_JoystickInstanceID(
     *                 SDL_GameControllerGetJoystick(gc)) == deviceId) {
     *             SDL_GameControllerClose(gc);
     *             g_controllers[i] = NULL;
     *             printf("[pad_shim] Controller removed from port %d\n", i);
     *             break;
     *         }
     *     }
     * }
     */
}


#endif /* __MWERKS__ */