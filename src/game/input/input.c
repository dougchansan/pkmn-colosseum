/**
 * @file input.c
 * @brief Game-level PAD input wrapper for Pokemon Colosseum.
 *
 * Wraps the Dolphin SDK PADRead/PADStatus system with a higher-level
 * input API that provides:
 *   - Edge-detected button presses and releases
 *   - Dead zone filtering for analog sticks
 *   - Digital stick mode (threshold conversion)
 *   - Per-controller rumble management
 *   - Controller connection state tracking
 *
 * The SDK PADRead function is called once per frame from the TaskPadRead
 * callback (fn_80005FFC in main.c), which iterates over controllers 1-4.
 * For each controller, PADInput_ReadButtons updates the per-pad state
 * by comparing the current button mask to the previous frame's.
 *
 * Address range: 0x800F7500 - 0x800F8268 (game-level PAD wrapper)
 *                0x8008A99C - 0x8008BBDC (lower-level PAD/motor)
 *
 * The global gRumbleEnabled (lbl_80478DC8) acts as a master switch:
 * when 0, all rumble commands are suppressed regardless of per-pad
 * settings. This flag is controlled by SetRumbleEnabled (fn_800056E4)
 * in main.c, which the player can toggle in the Options menu.
 *
 * PAD init sequence (from GameInit in main.c):
 *   1. PADInput_Init(16)           -- allocate state for 16 pads
 *   2. PADInput_SetMappingTable()  -- set controller mapping
 *   3. For each pad 1-4:
 *      a. PADInput_InitMotor(i)    -- check motor hardware
 *      b. PADInput_SetRumbleMode(i, 2) -- set to brake mode
 *      c. PADInput_SetDeadzone(i, 7)   -- 7-unit dead zone
 *      d. PADInput_SetStickMode(i, 1)  -- digital stick mode
 *
 * Per-frame update (from TaskPadRead / TaskPadRumble):
 *   1. For each pad 1-4:
 *      a. PADInput_IsConnected(i) -> if disconnected, recalibrate
 *      b. PADInput_ReadButtons(i)  -- update button state
 *   2. For each pad 1-4:
 *      a. PADInput_IsMotorAvailable(i) -> if not, reset rumble
 */

#include "game/input/input.h"

/* ===================================================================
 * External SDK functions
 * =================================================================== */

extern void* memset(void* dst, int val, u32 size);

/* GSmem allocator */
extern u16   fn_800E2C04(u32 size, u32 alignment);  /* GSmemAlloc */
extern void* fn_800E27B0(u16 handle);                /* GSmemGetPtr */

/* Dolphin SDK PAD functions */
extern void  PADRead(void* status);            /* Read all 4 controllers */
extern u32   PADReset(u32 mask);               /* Reset specified pads */
extern void  PADControlMotor(u32 chan, u32 cmd); /* Motor on/off/brake */
extern u32   PADRecalibrate(u32 mask);         /* Re-zero sticks */

/* Dolphin SDK SI functions (for type detection) */
extern u32   SIGetType(s32 chan);

/* Global rumble flag from main.c */
extern u8    lbl_80478DC8;  /* gRumbleEnabled */

/* ===================================================================
 * SDK PADStatus structure
 *
 * The SDK fills one PADStatus per controller during PADRead.
 * =================================================================== */

typedef struct PADStatus {
    u16  button;       /* 0x00: button mask */
    s8   stickX;       /* 0x02: main stick X */
    s8   stickY;       /* 0x03: main stick Y */
    s8   substickX;    /* 0x04: C-stick X */
    s8   substickY;    /* 0x05: C-stick Y */
    u8   triggerLeft;  /* 0x06: left trigger analog */
    u8   triggerRight; /* 0x07: right trigger analog */
    u8   analogA;      /* 0x08: A button analog (not used on GCN) */
    u8   analogB;      /* 0x09: B button analog */
    s8   err;          /* 0x0A: error code (0 = ok, -1 = not connected) */
    u8   padding;      /* 0x0B */
} PADStatus; /* size: 0x0C */

/* ===================================================================
 * Global state
 * =================================================================== */

/**
 * Per-controller state array.
 * Allocated during PADInput_Init with GSmem. The game uses 1-based
 * indexing (pad 1-4), so the array has room for index 0 as unused.
 */
static PADInputState* sPadStates = NULL;

/**
 * Number of pad slots allocated.
 */
static u32 sMaxPads = 0;

/**
 * Controller mapping table pointer.
 * Maps logical player indices to physical controller ports.
 */
static void* sPadMappingTable = NULL;

/**
 * SDK PADStatus buffer for all 4 controllers.
 * Filled by PADRead each frame.
 */
static PADStatus sPadStatus[PAD_MAX_CONTROLLERS];

/* ===================================================================
 * fn_800F7758: PADInput_Init
 *
 * Allocates the per-controller state array from GSmem.
 * Each PADInputState is approximately 0x1C bytes.
 *
 * @param maxPads  Number of controller slots (game uses 16)
 * =================================================================== */
void PADInput_Init(u32 maxPads) {
    u32 allocSize;
    u16 handle;

    sMaxPads = maxPads;

    /* Allocate state array, 32-byte aligned */
    allocSize = (maxPads * sizeof(PADInputState) + 0x1F) & ~0x1F;
    handle = fn_800E2C04(allocSize, 0x20);
    if (handle != 0) {
        sPadStates = (PADInputState*)fn_800E27B0(handle);
    } else {
        sPadStates = NULL;
    }

    /* Clear all state to zero */
    if (sPadStates != NULL) {
        memset(sPadStates, 0, allocSize);
    }
}

/* ===================================================================
 * fn_800F75FC: PADInput_SetMappingTable
 *
 * Stores the mapping table pointer. The table maps logical player
 * indices to physical controller port numbers.
 * =================================================================== */
void PADInput_SetMappingTable(void* table) {
    sPadMappingTable = table;
}

/* ===================================================================
 * fn_800F7AF0: PADInput_ReadButtons
 *
 * Core per-frame update for a single controller. Called from the
 * TaskPadRead callback for each of the 4 controllers.
 *
 * Flow:
 *   1. Read the raw SDK PADStatus for this pad
 *   2. Save current -> previous button state
 *   3. Store new held buttons from PADStatus
 *   4. Compute pressed = held & ~prev  (newly pressed)
 *   5. Compute released = prev & ~held (newly released)
 *   6. Copy analog stick values
 *   7. Update connection state
 *
 * Size: 0x6C in the symbol map -- a compact function.
 * =================================================================== */
void PADInput_ReadButtons(u32 padIdx) {
    PADInputState* state;
    PADStatus* status;
    u32 chan;

    if (sPadStates == NULL) {
        return;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return;
    }

    state = &sPadStates[padIdx];
    chan = padIdx - 1; /* convert 1-based to 0-based for SDK */
    status = &sPadStatus[chan];

    /* Read fresh PAD data from the SDK */
    PADRead(sPadStatus);

    /* Save previous state */
    state->buttonsPrev = state->buttonsHeld;

    /* Check connection */
    if (status->err == 0) {
        state->connected = 1;
        state->buttonsHeld = status->button;
        state->stickX = status->stickX;
        state->stickY = status->stickY;
        state->cStickX = status->substickX;
        state->cStickY = status->substickY;
        state->triggerL = status->triggerLeft;
        state->triggerR = status->triggerRight;
    } else {
        state->connected = 0;
        state->buttonsHeld = 0;
        state->stickX = 0;
        state->stickY = 0;
        state->cStickX = 0;
        state->cStickY = 0;
        state->triggerL = 0;
        state->triggerR = 0;
    }

    /* Compute edge detection */
    state->buttonsPressed  = state->buttonsHeld & ~state->buttonsPrev;
    state->buttonsReleased = state->buttonsPrev & ~state->buttonsHeld;
}

/* ===================================================================
 * fn_800F7BC4: PADInput_GetHeld
 *
 * Returns the current button mask. Simple accessor.
 * Size: 0x64 in the symbol map.
 * =================================================================== */
u16 PADInput_GetHeld(u32 padIdx) {
    if (sPadStates == NULL) {
        return 0;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return 0;
    }
    return sPadStates[padIdx].buttonsHeld;
}

/* ===================================================================
 * fn_800F7B5C: PADInput_GetPressed
 * =================================================================== */
u16 PADInput_GetPressed(u32 padIdx) {
    if (sPadStates == NULL) {
        return 0;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return 0;
    }
    return sPadStates[padIdx].buttonsPressed;
}

/* ===================================================================
 * fn_800F7C28: PADInput_GetReleased
 * =================================================================== */
u16 PADInput_GetReleased(u32 padIdx) {
    if (sPadStates == NULL) {
        return 0;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return 0;
    }
    return sPadStates[padIdx].buttonsReleased;
}

/* ===================================================================
 * fn_800F7C8C: PADInput_GetStickX
 *
 * Returns the main stick X value with dead zone filtering applied.
 * If the absolute value of the raw stick position is within the
 * dead zone threshold, returns 0 instead.
 *
 * In digital stick mode (stickMode == 1), the raw value is converted
 * to -128, 0, or +127 based on the dead zone threshold.
 *
 * Size: 0xAC in the symbol map -- moderate complexity.
 * =================================================================== */
s8 PADInput_GetStickX(u32 padIdx) {
    PADInputState* state;
    s8 raw;
    s8 dz;

    if (sPadStates == NULL) {
        return 0;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return 0;
    }

    state = &sPadStates[padIdx];
    raw = state->stickX;
    dz = (s8)state->deadzone;

    /* Dead zone filtering */
    if (raw > 0) {
        if (raw < dz) {
            return 0;
        }
        if (state->stickMode == PAD_STICK_MODE_DIGITAL) {
            return 127;
        }
    } else if (raw < 0) {
        if (raw > -dz) {
            return 0;
        }
        if (state->stickMode == PAD_STICK_MODE_DIGITAL) {
            return -128;
        }
    }

    return raw;
}

/* ===================================================================
 * fn_800F7D38: PADInput_GetStickY
 *
 * Same pattern as GetStickX but for the Y axis.
 * Size: 0xAC in the symbol map.
 * =================================================================== */
s8 PADInput_GetStickY(u32 padIdx) {
    PADInputState* state;
    s8 raw;
    s8 dz;

    if (sPadStates == NULL) {
        return 0;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return 0;
    }

    state = &sPadStates[padIdx];
    raw = state->stickY;
    dz = (s8)state->deadzone;

    /* Dead zone filtering */
    if (raw > 0) {
        if (raw < dz) {
            return 0;
        }
        if (state->stickMode == PAD_STICK_MODE_DIGITAL) {
            return 127;
        }
    } else if (raw < 0) {
        if (raw > -dz) {
            return 0;
        }
        if (state->stickMode == PAD_STICK_MODE_DIGITAL) {
            return -128;
        }
    }

    return raw;
}

/* ===================================================================
 * fn_800F7DE4: PADInput_SetStickMode
 *
 * Sets whether the analog stick operates in normal analog mode or
 * digital (threshold) mode. In digital mode, any stick deflection
 * past the dead zone is treated as full deflection.
 *
 * Size: 0x5C in the symbol map.
 * =================================================================== */
void PADInput_SetStickMode(u32 padIdx, u32 mode) {
    if (sPadStates == NULL) {
        return;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return;
    }
    sPadStates[padIdx].stickMode = mode;
}

/* ===================================================================
 * fn_800F7E40: PADInput_SetDeadzone
 *
 * Size: 0x5C in the symbol map.
 * =================================================================== */
void PADInput_SetDeadzone(u32 padIdx, u32 deadzone) {
    if (sPadStates == NULL) {
        return;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return;
    }
    sPadStates[padIdx].deadzone = (u8)deadzone;
}

/* ===================================================================
 * fn_800F7E9C: PADInput_SetRumbleMode
 *
 * Size: 0x5C in the symbol map.
 * =================================================================== */
void PADInput_SetRumbleMode(u32 padIdx, u32 mode) {
    if (sPadStates == NULL) {
        return;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return;
    }
    sPadStates[padIdx].rumbleMode = mode;
}

/* ===================================================================
 * fn_800F7EF8: PADInput_IsConnected
 *
 * Returns 1 if the controller is connected (last PADRead succeeded).
 * Size: 0x6C in the symbol map.
 * =================================================================== */
BOOL PADInput_IsConnected(u32 padIdx) {
    if (sPadStates == NULL) {
        return FALSE;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return FALSE;
    }
    return (sPadStates[padIdx].connected != 0) ? TRUE : FALSE;
}

/* ===================================================================
 * fn_800F7F64: PADInput_ResetRumble
 *
 * Stops the rumble motor and clears rumble state. Called when a
 * controller is disconnected to prevent orphaned motor activity.
 *
 * Size: 0x14C in the symbol map -- this is a larger function because
 * it handles multiple cleanup tasks.
 * =================================================================== */
void PADInput_ResetRumble(u32 padIdx) {
    PADInputState* state;
    u32 chan;

    if (sPadStates == NULL) {
        return;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return;
    }

    state = &sPadStates[padIdx];
    chan = padIdx - 1;

    /* Stop the motor */
    PADControlMotor(chan, 0); /* 0 = stop */

    /* Clear rumble state */
    state->rumbleActive = 0;
    state->rumbleTimer = 0;
    state->rumbleMode = PAD_RUMBLE_OFF;
}

/* ===================================================================
 * fn_800F80B0: PADInput_InitMotor
 *
 * Queries the SI controller type and checks if a rumble motor is
 * available. Standard GCN controllers have motor support; WaveBird
 * and third-party controllers may not.
 *
 * Size: 0x88 in the symbol map.
 * =================================================================== */
void PADInput_InitMotor(u32 padIdx) {
    PADInputState* state;
    u32 chan;
    u32 siType;

    if (sPadStates == NULL) {
        return;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return;
    }

    state = &sPadStates[padIdx];
    chan = padIdx - 1;

    /* Query SI hardware type */
    siType = SIGetType((s32)chan);

    /*
     * Standard GCN controller: SI type includes the motor bit.
     * SI type 0x09000000 = standard controller with motor.
     * If the type doesn't indicate motor support, mark as unavailable.
     */
    if ((siType & 0x20000000) != 0) {
        /* Has motor capability */
        state->motorInitialized = 1;
    } else {
        state->motorInitialized = 0;
    }
}

/* ===================================================================
 * fn_8008ABA0: PADInput_IsMotorAvailable
 *
 * Checks both the hardware detection (motorInitialized) and the
 * global rumble enable flag (lbl_80478DC8). Both must be true
 * for the motor to be considered available.
 *
 * This function is called from TaskPadRumble in main.c:
 *   for (i = 1; i <= 4; i++) {
 *       if (!PADInput_IsMotorAvailable(i)) {
 *           PADInput_ResetRumble(i);
 *       }
 *   }
 *
 * Size: 0x44 in the symbol map.
 * =================================================================== */
BOOL PADInput_IsMotorAvailable(u32 padIdx) {
    PADInputState* state;

    if (sPadStates == NULL) {
        return FALSE;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return FALSE;
    }

    /* Check global rumble enable first */
    if (lbl_80478DC8 == 0) {
        return FALSE;
    }

    state = &sPadStates[padIdx];
    return (state->motorInitialized != 0) ? TRUE : FALSE;
}

/* ===================================================================
 * fn_8008AC34: PADInput_Recalibrate
 *
 * Re-zeros the analog sticks and triggers for a controller. Called
 * when a controller is reconnected (detected by TaskPadRead in main.c
 * when PADInput_IsConnected returns FALSE).
 *
 * This calls the SDK PADRecalibrate which re-reads the neutral
 * position of the sticks and sets that as the new zero point.
 *
 * Size: 0x1E4 in the symbol map -- larger because it also resets
 * internal state and re-queries the controller type.
 * =================================================================== */
void PADInput_Recalibrate(u32 padIdx) {
    PADInputState* state;
    u32 chan;
    u32 mask;

    if (sPadStates == NULL) {
        return;
    }
    if (padIdx == 0 || padIdx > PAD_MAX_CONTROLLERS) {
        return;
    }

    state = &sPadStates[padIdx];
    chan = padIdx - 1;
    mask = (u32)(1 << chan); /* PAD channel bitmask */

    /* Call SDK recalibration */
    PADRecalibrate(mask);

    /* Reset the input state for this controller */
    state->buttonsHeld = 0;
    state->buttonsPrev = 0;
    state->buttonsPressed = 0;
    state->buttonsReleased = 0;
    state->stickX = 0;
    state->stickY = 0;
    state->cStickX = 0;
    state->cStickY = 0;
    state->triggerL = 0;
    state->triggerR = 0;
    state->connected = 0;

    /* Re-check motor availability */
    PADInput_InitMotor(padIdx);
}

/* ===================================================================
 * Stub functions for coverage -- TODO: decompile
 * 4 function(s)
 * =================================================================== */

/* fn_800F760C - 0x800F760C | size: 0xD8 */
/*
 * InputRemoveAndScan - Remove an input entry from the linked list
 * and scan pad entries for matching IDs.
 *
 * Walks the linked list at (state+0x08) to find and unlink the
 * target entry. Then iterates through pad slots to mark matching
 * entries with status 3.
 *
 * 0x800F760C | size: 0xCC
 */
s32 fn_800F760C(u8* target) {
    extern u8 lbl_802712B8[];
    extern u8* lbl_80478B00;
    extern void fn_800DD38C(const char* msg, ...);
    u8* state = lbl_80478B00;
    u8* prev;
    u8* cur;
    u32 offset;
    u8* padData;
    s32 i;
    u8 status;

    /* Try to unlink target from the linked list at state+0x08 */
    cur = *(u8**)(state + 0x08);
    if (cur == target) {
        /* Target is head: remove it */
        *(u32*)(state + 0x08) = *(u32*)(cur + 0x14);
    } else {
        /* Walk the list to find target */
        prev = cur;
        while ((cur = *(u8**)(prev + 0x14)) != NULL) {
            if (cur == target) {
                /* Unlink: prev->next = cur->next */
                *(u32*)(prev + 0x14) = *(u32*)(cur + 0x14);
                goto scan;
            }
            prev = cur;
        }
        if (cur == NULL) {
            /* Not found in list */
            fn_800DD38C((const char*)lbl_802712B8, target);
            return -1;
        }
    }

scan:
    /* Scan pad entries and mark matching ones */
    i = 0;
    offset = 0;
    status = 3;
    while (i < (s32)*(u16*)lbl_80478B00) {
        padData = (u8*)(*(u32*)(lbl_80478B00 + 0x0C) + offset);
        if (*(u8*)(padData + 0x04) != 0) {
            u16 entryId = (u16)(*(u32*)(padData + 0x08) >> 16);
            if (*(u16*)(target + 0x00) == entryId) {
                *(u8*)(padData + 0x04) = status;
            }
        }
        offset += 0x16C;
        i++;
    }
    return 0;
}

/* fn_800F78A4 - 0x800F78A4 | size: 0x7C */
extern u8 lbl_80401C10[];
/* Shared 4-slot pad lookup. Defined before all readers so CW 1.3 inlines it:
 * the inlined multi-`return pad` yields the target's un-inverted `bne;b` per
 * slot, and per-function optimization_level 2 keeps the `nul` sentinel in its
 * own register instead of folding to `li rPad,0`. */
static inline u8* WI_FindPad(s32 padId) {
    u8* pad = &lbl_80401C10[0];
    u8* nul = NULL;
    if (*(s32*)pad == padId) return pad;
    if (*(s32*)(pad += 0x6c) == padId) return pad;
    if (*(s32*)(pad += 0x6c) == padId) return pad;
    if (*(s32*)(pad += 0x6c) == padId) return pad;
    return nul;
}

/*
 * InputSetRumble - Find a pad entry by ID and configure rumble.
 *
 * Searches through up to 4 pad entries in lbl_80401C10 for a
 * matching pad ID. If found and non-null, sets rumble parameters.
 *
 * 0x800F78A4 | size: 0x7C
 */
void fn_800F78A4(s32 padId, u8 mode, u8 strength, u32 duration, u8 flags) {
#pragma optimization_level 2
    u8* pad;

    pad = WI_FindPad(padId);

    if (pad == NULL) { return; }
    if (mode != 0) { return; }

    /* Set rumble parameters */
    *(u32*)(pad + 0x5C) = 1;
    *(u32*)(pad + 0x60) = (u32)(strength & 0xFF) * 0xF;
    *(u32*)(pad + 0x64) = duration;
    *(u8*)(pad + 0x68) = flags;
}

/*
 * InputGetAnalogY - Get analog Y stick value for a pad by ID.
 *
 * Searches pad entries for a match and returns the Y axis value.
 * If mode == 1, reads from offset 0x5B (smoothed), else 0x29 (raw).
 *
 * 0x800F7920 | size: 0x74
 */
u8 fn_800F7920(s32 padId, s32 mode) {
#pragma optimization_level 2
    u8* pad;

    pad = WI_FindPad(padId);

    if (pad == NULL) { return 0; }

    if (mode == 1) {
        return *(u8*)(pad + 0x5B);
    }
    return *(u8*)(pad + 0x29);
}

/*
 * InputGetAnalogX - Get analog X stick value for a pad by ID.
 *
 * Same pattern as InputGetAnalogY but reads offsets 0x5A / 0x28.
 *
 * 0x800F7994 | size: 0x74
 */
u8 fn_800F7994(s32 padId, s32 mode) {
#pragma optimization_level 2
    u8* pad;

    pad = WI_FindPad(padId);

    if (pad == NULL) { return 0; }

    if (mode == 1) {
        return *(u8*)(pad + 0x5A);
    }
    return *(u8*)(pad + 0x28);
}

extern u8* lbl_80478B00;
s32 fn_800F75FC(u32 val) {
    *(u32*)(lbl_80478B00 + 0x10) = val;
    return 0;
}
extern u8* lbl_80478B00;
#if 0
asm void fn_800F76E4(void) {
#include "src/game/input/input_fn_800F76E4.inc"
}
#else
void fn_800F76E4(u8* arg) {
    extern u8* lbl_80478B00;
    u8* head;
    u32 off;
    u32* tbl;
    s32 i;

    *(u32*)(arg + 0x14) = 0;
    head = (u8*)*(u32*)(lbl_80478B00 + 0x8);
    if (head == NULL) {
        *(u32*)(lbl_80478B00 + 0x8) = (u32)arg;
    } else {
        *(u32*)(lbl_80478B00 + 0x8) = (u32)arg;
        *(u32*)(arg + 0x14) = (u32)head;
    }
    if (*(u8*)(arg + 0xa) != 0) return;
    tbl = (u32*)(arg + *(u32*)(arg + 0xc));
    i = 0;
    while (i < (s32)*(u16*)(arg + 0x6)) {
        off = *tbl;
        tbl += 1;
        i += 1;
        *(u32*)(arg + off) += (u32)arg;
    }
    *(u8*)(arg + 0xa) = 1;
}
#endif
extern void fn_800E3534(void);
extern void fn_800DD38C(void);
extern u8 lbl_80401BF8[];
extern u8* lbl_80478B00;
extern u8 lbl_802712E4[];
#if 1
s32 fn_800F7758(u32 count) {
    extern void* memset(void* dst, int val, u32 size);
    extern u16 fn_800E3534(u32 size);
    extern void* fn_800E27B0(u16 handle);
    extern void fn_800DD38C(const char* msg, ...);
    extern u8 lbl_80401BF8[];
    extern u8* lbl_80478B00;
    extern u8 lbl_802712E4[];
    s32 count16;
    u8* item;
    s32 i;

    memset(lbl_80401BF8, 0, 0x14);
    count16 = (u16)count;
    lbl_80478B00 = lbl_80401BF8;
    *(u16*)(lbl_80478B00 + 2) = fn_800E3534(count16 * 0x16c);
    if (*(u16*)(lbl_80478B00 + 2) == 0) {
        fn_800DD38C((const char*)lbl_802712E4);
        return -1;
    }

    *(u32*)(lbl_80478B00 + 0xc) = (u32)fn_800E27B0(*(u16*)(lbl_80478B00 + 2));
    item = *(u8**)(lbl_80478B00 + 0xc);
    for (i = 0; i < count16; i++) {
        *(u32*)(item + 0) = 0;
        *(u8*)(item + 4) = 0;
        item += 0x16c;
    }
    *(u16*)lbl_80478B00 = count;
    return 0;
}
#else
void fn_800F7758(void) { /* TODO */ }
#endif
#if 0
asm void fn_800F7A08(void) {
#include "src/game/input/input_fn_800F7A08.inc"
}
#else
#pragma push
#pragma optimization_level 2
u32 fn_800F7A08(s32 key, s32 sel) {
    u8* pad = WI_FindPad(key);
    if (pad == NULL) return 0;
    if (sel == 1) return *(u8*)(pad + 0x59);
    return *(u8*)(pad + 0x27);
}
#pragma pop
#endif
#if 0
asm void fn_800F7A7C(void) {
#include "src/game/input/input_fn_800F7A7C.inc"
}
#else
#pragma push
#pragma optimization_level 2
u32 fn_800F7A7C(s32 key, s32 sel) {
    u8* pad = WI_FindPad(key);
    if (pad == NULL) return 0;
    if (sel == 1) return *(u8*)(pad + 0x58);
    return *(u8*)(pad + 0x26);
}
#pragma pop
#endif
#if 0
asm void fn_800F7AF0(void) {
#include "src/game/input/input_fn_800F7AF0.inc"
}
#else
#pragma push
#pragma optimization_level 2
u32 fn_800F7AF0(s32 padId) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return 0;
    return (u32)*(u16*)(pad + 0x24) ^ *(u32*)(pad + 0x30);
}
#pragma pop
#endif
#if 0
asm void fn_800F7B5C(void) {
#include "src/game/input/input_fn_800F7B5C.inc"
}
#else
#pragma push
#pragma optimization_level 2
u32 fn_800F7B5C(s32 padId) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return 0;
    return ~(u32)*(u16*)(pad + 0x24);
}
#pragma pop
#endif
#if 0
asm void fn_800F7BC4(void) {
#include "src/game/input/input_fn_800F7BC4.inc"
}
#else
#pragma push
#pragma optimization_level 2
u16 fn_800F7BC4(s32 padId) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return 0;
    return *(u16*)(pad + 0x24);
}
#pragma pop
#endif
#if 0
asm void fn_800F7C28(void) {
#include "src/game/input/input_fn_800F7C28.inc"
}
#else
#pragma push
#pragma optimization_level 2
u32 fn_800F7C28(s32 padId) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return 2;
    return *(u32*)(pad + 0x4);
}
#pragma pop
#endif
extern u32 lbl_8047CCC8;
#if 1
asm void fn_800F7C8C(void) {
#include "src/game/input/input_fn_800F7C8C.inc"
}
#else
#pragma push
#pragma optimization_level 3
void fn_800F7C8C(s32 padId, s8 stickX, s8 stickY) {
    extern u8 lbl_80401C10[];
    u8* pad = &lbl_80401C10[0];
    u8* nul = NULL;
    if (*(s32*)pad != padId) goto try1;
    goto found;
try1:
    pad += 0x6c;
    if (*(s32*)pad != padId) goto try2;
    goto found;
try2:
    pad += 0x6c;
    if (*(s32*)pad != padId) goto try3;
    goto found;
try3:
    pad += 0x6c;
    if (*(s32*)pad != padId) goto miss;
    goto found;
miss:
    pad = nul;
found:
    if (pad == NULL) return;
    *(s8*)(pad + 0x36) = stickX;
    *(s8*)(pad + 0x37) = stickY;
    *(f32*)(pad + 0x50) = (f32)(s32)stickX;
    *(f32*)(pad + 0x54) = (f32)(s32)stickY;
}
#pragma pop
#endif
extern u32 lbl_8047CCC8;
#if 1
asm void fn_800F7D38(void) {
#include "src/game/input/input_fn_800F7D38.inc"
}
#else
#pragma push
#pragma optimization_level 3
void fn_800F7D38(s32 padId, s8 stickX, s8 stickY) {
    extern u8 lbl_80401C10[];
    u8* pad = &lbl_80401C10[0];
    u8* nul = NULL;
    if (*(s32*)pad != padId) goto try1;
    goto found;
try1:
    pad += 0x6c;
    if (*(s32*)pad != padId) goto try2;
    goto found;
try2:
    pad += 0x6c;
    if (*(s32*)pad != padId) goto try3;
    goto found;
try3:
    pad += 0x6c;
    if (*(s32*)pad != padId) goto miss;
    goto found;
miss:
    pad = nul;
found:
    if (pad == NULL) return;
    *(s8*)(pad + 0x34) = stickX;
    *(s8*)(pad + 0x35) = stickY;
    *(f32*)(pad + 0x48) = (f32)(s32)stickX;
    *(f32*)(pad + 0x4c) = (f32)(s32)stickY;
}
#pragma pop
#endif
static inline u8* PADInput_FindPad(s32 padId) {
    extern u8 lbl_80401C10[];
    u8* pad = &lbl_80401C10[0];
    u8* nul = NULL;
    if (*(s32*)pad == padId) return pad;
    if (*(s32*)(pad += 0x6c) == padId) return pad;
    if (*(s32*)(pad += 0x6c) == padId) return pad;
    if (*(s32*)(pad += 0x6c) == padId) return pad;
    return nul;
}
#if 0
asm void fn_800F7DE4(void) {
#include "src/game/input/input_fn_800F7DE4.inc"
}
#else
#pragma push
#pragma optimization_level 2
void fn_800F7DE4(s32 padId, u32 val) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return;
    *(u32*)(pad + 0x14) = val;
}
#pragma pop
#endif
#if 0
asm void fn_800F7E40(void) {
#include "src/game/input/input_fn_800F7E40.inc"
}
#else
#pragma push
#pragma optimization_level 2
void fn_800F7E40(s32 padId, u8 val) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return;
    *(u8*)(pad + 0x10) = val;
}
#pragma pop
#endif
#if 0
asm void fn_800F7E9C(void) {
#include "src/game/input/input_fn_800F7E9C.inc"
}
#else
#pragma push
#pragma optimization_level 2
void fn_800F7E9C(s32 padId, u32 val) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return;
    *(u32*)(pad + 0x8) = val;
}
#pragma pop
#endif
#if 0
asm void fn_800F7EF8(void) {
#include "src/game/input/input_fn_800F7EF8.inc"
}
#else
#pragma push
#pragma optimization_level 2
u8 fn_800F7EF8(s32 padId) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return 0;
    return *(u32*)(pad + 0xc) == 0;
}
#pragma pop
#endif
extern void fn_800D3094(void);
extern void fn_800AAD34(void);
extern void fn_800F8654(void);
extern void fn_800F8A54(void);
extern void OSDisableInterrupts();
extern void* memcpy(void* dst, const void* src, u32 n);
extern void OSRestoreInterrupts();
extern u32 lbl_8047AC4C;
extern u32 lbl_8047AC50;
#if 1
#pragma push
#pragma optimization_level 2
u32 fn_800F7F64(s32 padId) {
    extern u32 OSDisableInterrupts(void);
    extern void OSRestoreInterrupts(u32);
    extern u32 fn_800D3094(void);
    extern void fn_800AAD34(u32);
    extern void* memcpy(void* dst, const void* src, u32 n);
    extern void fn_800F8654(u8*, u8, u8, u8*, u8*, u8*, u8*, f32*, f32*);
    extern void fn_800F8A54(u8*);
    extern u32 lbl_8047AC4C;
    extern u32 lbl_8047AC50;
    u8* pad;
    u32 irq;
    u32 token;

    pad = WI_FindPad(padId);
    if (pad == NULL) {
        return 2;
    }

    *(u32*)(pad + 0x30) = *(u16*)(pad + 0x24);
    irq = OSDisableInterrupts();
    token = fn_800D3094();
    if (*(volatile u32*)&lbl_8047AC4C != 0 && token != lbl_8047AC50) {
        fn_800AAD34(*(volatile u32*)&lbl_8047AC4C);
        lbl_8047AC50 = token;
    }
    memcpy(pad + 0x24, pad + 0x18, 0xc);
    OSRestoreInterrupts(irq);
    fn_800F8654(pad, *(u8*)(pad + 0x26), *(u8*)(pad + 0x27),
                pad + 0x34, pad + 0x35, pad + 0x38, pad + 0x3c,
                (f32*)(pad + 0x48), (f32*)(pad + 0x4c));
    fn_800F8654(pad, *(u8*)(pad + 0x28), *(u8*)(pad + 0x29),
                pad + 0x36, pad + 0x37, pad + 0x40, pad + 0x44,
                (f32*)(pad + 0x50), (f32*)(pad + 0x54));
    fn_800F8A54(pad);
    return *(u32*)(pad + 0xc);
}
#pragma pop
#else
void fn_800F7F64(void) { /* TODO */ }
#endif
#if 0
asm void fn_800F80B0(void) {
#include "src/game/input/input_fn_800F80B0.inc"
}
#else
s32 fn_800F80B0(u32 padIdx) {
    extern u8 lbl_80401C10[];
    u8* pad = &lbl_80401C10[0];
    u8* nul = NULL;

    if (*(s32*)pad == 0) goto found;
    pad += 0x6C;
    if (*(s32*)pad == 0) goto found;
    pad += 0x6C;
    if (*(s32*)pad == 0) goto found;
    pad += 0x6C;
    if (*(s32*)pad == 0) goto found;
    pad = nul;
found:
    if (pad == NULL) {
        return 4;
    }
    if (padIdx >= 5) {
        return 1;
    }
    if (padIdx < 1) {
        return 1;
    }

    *(u32*)pad = padIdx;
    return 0;
}
#endif
extern void fn_800ABCF4(void);
extern void fn_800AAF38(void);
extern void fn_800AB4FC(void);
extern void fn_800AAE34(void);
extern void fn_800F8268(void);
extern void fn_800ABF5C(void);
extern void SISetSamplingRate();
extern u32 lbl_8047AC48;
extern u32 lbl_8047CCD0;
extern u32 lbl_8047AC4C;
extern u32 lbl_8047AC50;
#if 0
asm void fn_800F8138(void) {
#include "src/game/input/input_fn_800F8138.inc"
}
#else
#pragma push
#pragma scheduling on
void fn_800F8138(void) {
    extern u8 lbl_80401C10[];
    extern u32 lbl_8047AC48;
    extern u32 lbl_8047CCD0;
    extern u32 lbl_8047AC4C;
    extern u32 lbl_8047AC50;
    extern void fn_800ABCF4(s32);
    extern void fn_800AAF38(void);
    extern void fn_800AB4FC(void*);
    extern void fn_800AAE34(u32);
    extern void fn_800ABF5C(void*);
    extern void SISetSamplingRate(s32);
    u8* base;
    u8* pad;
    u32* arr1c0;
    u32* arr1b0;
    s32 i;
    f32 zero_f;

    lbl_8047AC48 = 0;
    base = lbl_80401C10;
    lbl_8047AC4C = 0xF0000000;
    pad = base;
    zero_f = *(f32*)&lbl_8047CCD0;
    arr1c0 = (u32*)(base + 0x1c0);
    arr1b0 = (u32*)(base + 0x1b0);
    lbl_8047AC50 = 0;

    for (i = 0; i < 4; i++) {
        *(u32*)(pad + 0x00) = 0;
        *(u32*)(pad + 0x08) = 0;
        *(u32*)(pad + 0x0C) = 3;
        *(u8*) (pad + 0x10) = 0;
        *(u32*)(pad + 0x14) = 0;
        *(u32*)(pad + 0x30) = 0;
        memset(pad + 0x18, 0, 12);
        memset(pad + 0x24, 0, 12);
        *(u8*) (pad + 0x34) = 0;
        *(u8*) (pad + 0x35) = 0;
        *(u8*) (pad + 0x36) = 0;
        *(u8*) (pad + 0x37) = 0;
        *(f32*)(pad + 0x48) = zero_f;
        *(f32*)(pad + 0x4C) = zero_f;
        *(f32*)(pad + 0x50) = zero_f;
        *(f32*)(pad + 0x54) = zero_f;
        *(u32*)(pad + 0x5C) = 3;
        *(u32*)(pad + 0x60) = 0;
        *(u32*)(pad + 0x64) = 0;
        *(u8*) (pad + 0x68) = 0;
        pad += 0x6c;
        *arr1c0++ = 2;
        *arr1b0++ = 0;
    }

    fn_800ABCF4(0);
    fn_800AAF38();
    fn_800AB4FC(base + 0x1c0);
    fn_800AAE34(lbl_8047AC4C);
    SISetSamplingRate(0xb);
    fn_800ABF5C((void*)fn_800F8268);
}
#pragma pop
#endif
