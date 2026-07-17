#ifndef DOLPHIN_SI_SI_H
#define DOLPHIN_SI_SI_H

#include "dolphin/types.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSInterrupt.h"

typedef void (*SICallback)(s32 chan, u32 sr, OSContext* context);
typedef void (*SITypeAndStatusCallback)(s32 chan, u32 type);

/* Timing macros - bus clock is at 0x800000F8 */
#define OS_BUS_CLOCK         (*(u32*)0x800000F8)
#define OS_TIMER_CLOCK       (OS_BUS_CLOCK / 4)
#define OSMicrosecondsToTicks(usec) ((usec) * (OS_TIMER_CLOCK / 1000000))
#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))

void SIInit(void);
u32 SIGetType(s32 chan);
u32 SISetXY(u32 x, u32 y);
u32 SISetSamplingRate(u32 msec);
BOOL SITransfer(s32 chan, void* output, u32 outputBytes,
                void* input, u32 inputBytes,
                SICallback callback, s64 time);
BOOL SIBusy(void);
BOOL SIIsChanBusy(s32 chan);
u32 SISync(void);
u32 SIGetStatus(s32 chan);
void SISetCommand(s32 chan, u32 command);
u32 SIGetCommand(s32 chan);
void SITransferCommands(void);
u32 SIEnablePolling(u32 poll);
u32 SIDisablePolling(u32 poll);
BOOL SIGetResponse(s32 chan, void* data);
BOOL SIRegisterPollingHandler(__OSInterruptHandler handler);
BOOL SIUnregisterPollingHandler(__OSInterruptHandler handler);

/* Wireless controller functions (from OSRtc) */
extern void OSSetWirelessID(s32 chan, u16 id);
extern u16 OSGetWirelessID(s32 chan);

#endif /* DOLPHIN_SI_SI_H */
