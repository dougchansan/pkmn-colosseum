/**
 * @file pad_shim.h
 * @brief PAD API replacement -- SDL2 GameController backend.
 *
 * Replaces the Dolphin SDK PAD functions (PADRead, PADReset, PADControlMotor,
 * PADRecalibrate) and SI functions (SIGetType) with SDL2 GameController
 * equivalents.
 *
 * The game's input wrapper (input.c, 20 functions) calls the PAD SDK
 * functions to read controller state. This shim intercepts at the SDK
 * boundary, translating SDL2 GameController events into the PADStatus
 * structure that the game expects.
 *
 * Also provides an optional keyboard fallback mapping for players
 * without a gamepad.
 *
 * References:
 *   - docs/pc_port_design.md Section 10 (Input Replacement)
 *   - include/game/input/input.h (PADInputState, button masks)
 *   - include/dolphin/si/SI.h (SIGetType)
 *
 * Phase 3 PC port scaffolding -- skeleton only.
 */
#ifndef PCPORT_PAD_SHIM_H
#define PCPORT_PAD_SHIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard types */
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long    u32;
typedef signed char    s8;
typedef signed short   s16;
typedef signed long      s32;
typedef int            BOOL;

/* =========================================================================
 * PADStatus structure (matching Dolphin SDK)
 *
 * PADRead fills an array of 4 of these, one per controller port.
 * The game's PADInput_ReadButtons function reads from this.
 * ========================================================================= */

typedef struct PADStatus {
    u16 button;         /* Held button bitmask (PAD_BUTTON_* from input.h) */
    s8  stickX;         /* Main stick X: -128 to 127 */
    s8  stickY;         /* Main stick Y: -128 to 127 */
    s8  substickX;      /* C-stick X: -128 to 127 */
    s8  substickY;      /* C-stick Y: -128 to 127 */
    u8  triggerLeft;    /* Left analog trigger: 0-255 */
    u8  triggerRight;   /* Right analog trigger: 0-255 */
    u8  analogA;        /* Analog A button (unused by Colosseum) */
    u8  analogB;        /* Analog B button (unused by Colosseum) */
    s8  err;            /* Error code: 0=ok, -1=not connected */
} PADStatus;

/* =========================================================================
 * GCN button masks (matching input.h for reference)
 * ========================================================================= */

#define GCN_PAD_BUTTON_LEFT   0x0001
#define GCN_PAD_BUTTON_RIGHT  0x0002
#define GCN_PAD_BUTTON_DOWN   0x0004
#define GCN_PAD_BUTTON_UP     0x0008
#define GCN_PAD_TRIGGER_Z     0x0010
#define GCN_PAD_TRIGGER_R     0x0020
#define GCN_PAD_TRIGGER_L     0x0040
#define GCN_PAD_BUTTON_A      0x0100
#define GCN_PAD_BUTTON_B      0x0200
#define GCN_PAD_BUTTON_X      0x0400
#define GCN_PAD_BUTTON_Y      0x0800
#define GCN_PAD_BUTTON_START  0x1000

/* =========================================================================
 * Motor commands (matching PAD SDK)
 * ========================================================================= */

#define PAD_MOTOR_STOP    0
#define PAD_MOTOR_RUMBLE  1
#define PAD_MOTOR_BRAKE   2

/* =========================================================================
 * Controller type return value (matching SI SDK)
 * ========================================================================= */

/** Standard GCN controller with motor support */
#define SI_TYPE_GC_CONTROLLER_MOTOR  0x09000000

/* =========================================================================
 * Public API -- PAD SDK function replacements
 * ========================================================================= */

/**
 * PADInit -- Initialize the controller subsystem.
 *
 * Called during game startup. On PC, opens SDL2 GameController subsystem
 * and enumerates connected controllers.
 *
 * SDL2 equivalent:
 *   SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
 *   for each joystick: SDL_GameControllerOpen(i);
 */
void PADInit(void);

/**
 * PADRead -- Read the current state of all 4 controllers.
 *
 * @param status  Array of 4 PADStatus structures to fill.
 *
 * Called every frame by the game's TaskPadRead.
 *
 * SDL2 equivalent:
 *   SDL_GameControllerUpdate();
 *   For each controller i:
 *     Map SDL_GameControllerGetAxis/Button to PADStatus fields.
 *     Scale stick axes from [-32768,32767] to [-128,127].
 *     Map triggers from [0,32767] to [0,255].
 */
void PADRead(PADStatus status[4]);

/**
 * PADReset -- Reset specified controllers.
 *
 * @param mask  Bitmask of controller ports to reset (bit 0 = port 1, etc.)
 * @return      0 on success.
 *
 * PC port: Re-open SDL GameControllers if needed.
 */
u32 PADReset(u32 mask);

/**
 * PADRecalibrate -- Recalibrate analog sticks.
 *
 * @param mask  Bitmask of controller ports to recalibrate.
 * @return      0 on success.
 *
 * PC port: No-op -- SDL handles stick calibration.
 */
u32 PADRecalibrate(u32 mask);

/**
 * PADControlMotor -- Control the rumble motor.
 *
 * @param chan  Controller port (0-3).
 * @param cmd   Motor command (PAD_MOTOR_STOP/RUMBLE/BRAKE).
 *
 * SDL2 equivalent:
 *   cmd=0: SDL_GameControllerRumble(gc, 0, 0, 0) -- stop
 *   cmd=1: SDL_GameControllerRumble(gc, 0xFFFF, 0xFFFF, 100) -- on
 *   cmd=2: SDL_GameControllerRumble(gc, 0x4000, 0x4000, 50) -- brake
 */
void PADControlMotor(u32 chan, u32 cmd);

/**
 * SIGetType -- Get the controller type for a port.
 *
 * @param chan  Controller port (0-3).
 * @return     Controller type bitmask, or 0 if not connected.
 *
 * PC port: Returns SI_TYPE_GC_CONTROLLER_MOTOR if an SDL GameController
 * is connected to that port, 0 otherwise.
 * The game uses this to check for motor (rumble) support.
 */
u32 SIGetType(s32 chan);

/* =========================================================================
 * Extended API -- PC-specific features
 * ========================================================================= */

/**
 * PADShim_Init -- Initialize the pad shim with keyboard + controller support.
 *
 * Call this instead of PADInit to get keyboard fallback in addition to
 * gamepad support.
 *
 * @param enableKeyboard  If non-zero, enable keyboard input mapping.
 */
void PADShim_Init(BOOL enableKeyboard);

/**
 * PADShim_Shutdown -- Clean up the pad shim.
 *
 * Closes all SDL GameControllers and frees resources.
 */
void PADShim_Shutdown(void);

/**
 * PADShim_SetKeyboardMapping -- Override the default keyboard mapping.
 *
 * Default mapping (from pc_port_design.md Section 10.4):
 *   Arrow keys  -> D-Pad
 *   Z           -> A button
 *   X           -> B button
 *   A           -> X button
 *   S           -> Y button
 *   Enter       -> Start
 *   Space       -> Z trigger
 *   Q / E       -> L / R triggers
 *   WASD        -> Main stick
 *   IJKL        -> C-stick
 *
 * @param keyA      SDL scancode for A button.
 * @param keyB      SDL scancode for B button.
 * @param keyX      SDL scancode for X button.
 * @param keyY      SDL scancode for Y button.
 * @param keyStart  SDL scancode for Start.
 * @param keyZ      SDL scancode for Z trigger.
 * @param keyL      SDL scancode for L trigger.
 * @param keyR      SDL scancode for R trigger.
 */
void PADShim_SetKeyboardMapping(u32 keyA, u32 keyB, u32 keyX, u32 keyY,
                                u32 keyStart, u32 keyZ, u32 keyL, u32 keyR);

/**
 * PADShim_UpdateKeyboard -- Update keyboard input state.
 *
 * Called internally by PADRead when keyboard mode is enabled.
 * Reads the current keyboard state via SDL_GetKeyboardState and
 * populates PADStatus[0] with the mapped button/stick values.
 *
 * @param status  PADStatus for controller 0 (keyboard maps to player 1).
 */
void PADShim_UpdateKeyboard(PADStatus* status);

/**
 * PADShim_HandleControllerEvent -- Handle SDL controller connect/disconnect.
 *
 * Call from the main event loop when receiving SDL_CONTROLLERDEVICEADDED
 * or SDL_CONTROLLERDEVICEREMOVED events.
 *
 * @param eventType  SDL event type.
 * @param deviceId   SDL joystick device ID.
 */
void PADShim_HandleControllerEvent(u32 eventType, s32 deviceId);

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_PAD_SHIM_H */
