#ifndef DOLPHIN_PAD_PAD_H
#define DOLPHIN_PAD_PAD_H

#include "dolphin/types.h"

#define PAD_CHAN0           0
#define PAD_CHAN1           1
#define PAD_CHAN2           2
#define PAD_CHAN3           3
#define PAD_MAX_CONTROLLERS 4

#define PAD_CHAN0_BIT 0x80000000
#define PAD_CHAN1_BIT 0x40000000
#define PAD_CHAN2_BIT 0x20000000
#define PAD_CHAN3_BIT 0x10000000

#define PAD_MOTOR_STOP      0
#define PAD_MOTOR_RUMBLE    1
#define PAD_MOTOR_STOP_HARD 2

#define PAD_BUTTON_LEFT   0x0001
#define PAD_BUTTON_RIGHT  0x0002
#define PAD_BUTTON_DOWN   0x0004
#define PAD_BUTTON_UP     0x0008
#define PAD_TRIGGER_Z     0x0010
#define PAD_TRIGGER_R     0x0020
#define PAD_TRIGGER_L     0x0040
#define PAD_BUTTON_A      0x0100
#define PAD_BUTTON_B      0x0200
#define PAD_BUTTON_X      0x0400
#define PAD_BUTTON_Y      0x0800
#define PAD_BUTTON_START  0x1000

#define PAD_ERR_NONE       0
#define PAD_ERR_NO_CONTROLLER -1
#define PAD_ERR_NOT_READY  -2
#define PAD_ERR_TRANSFER   -3

typedef struct PADStatus {
    u16 button;
    s8  stickX;
    s8  stickY;
    s8  substickX;
    s8  substickY;
    u8  triggerLeft;
    u8  triggerRight;
    u8  analogA;
    u8  analogB;
    s8  err;
    u8  _padding;
} PADStatus;

typedef void (*PADSamplingCallback)(void);

BOOL PADInit(void);
u32  PADRead(PADStatus* status);
BOOL PADReset(u32 mask);
BOOL PADRecalibrate(u32 mask);
void PADControlMotor(s32 chan, u32 command);
void PADClamp(PADStatus* status);
void PADClampCircle(PADStatus* status);
BOOL __PADDisableRecalibration(BOOL disable);
PADSamplingCallback PADSetSamplingCallback(PADSamplingCallback callback);

#endif /* DOLPHIN_PAD_PAD_H */
