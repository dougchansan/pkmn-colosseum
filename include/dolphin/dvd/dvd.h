#ifndef DOLPHIN_DVD_DVD_H
#define DOLPHIN_DVD_DVD_H

#include "dolphin/types.h"

typedef struct DVDDiskID {
    char gameName[4];
    char company[2];
    u8 diskNumber;
    u8 gameVersion;
    u8 streaming;
    u8 streamingBufSize;
    u8 padding[22];
} DVDDiskID;

typedef struct DVDCommandBlock DVDCommandBlock;

typedef void (*DVDCBCallback)(s32 result, DVDCommandBlock* block);
typedef void (*DVDLowCallback)(u32 intType);

struct DVDCommandBlock {
    DVDCommandBlock* next;
    DVDCommandBlock* prev;
    u32 command;
    s32 state;
    u32 offset;
    u32 length;
    void* addr;
    u32 currTransferSize;
    u32 transferredSize;
    DVDDiskID* id;
    DVDCBCallback callback;
    void* userData;
    u8 _padding[0x30 - 0x30];
};

typedef struct DVDDriveInfo {
    u16 revisionLevel;
    u16 deviceCode;
    u32 releaseDate;
    u8 padding[24];
} DVDDriveInfo;

typedef struct DVDFileInfo {
    DVDCommandBlock cb;
    u32 startAddr;
    u32 length;
    DVDCBCallback callback;
} DVDFileInfo;

/* DVD functions */
void DVDInit(void);
BOOL DVDReadDiskID(DVDCommandBlock* block, DVDDiskID* diskID, DVDCBCallback callback);
BOOL DVDCancelStreamAsync(DVDCommandBlock* block, DVDCBCallback callback);
BOOL DVDInquiryAsync(DVDCommandBlock* block, DVDDriveInfo* info, DVDCBCallback callback);
void DVDReset(void);
s32 DVDGetDriveStatus(void);

/* DVD internal functions */
void __DVDFSInit(void);
void __DVDClearWaitingQueue(void);
BOOL __DVDPushWaitingQueue(s32 prio, DVDCommandBlock* block);
DVDCommandBlock* __DVDPopWaitingQueue(void);
BOOL __DVDCheckWaitingQueue(void);
void stateTimeout(void);
void fn_800A58F0(void);
void fn_800A5FC0(DVDCommandBlock* block);
void fn_800A5FF4(DVDCommandBlock* block);
void fn_800A60D4(u32 intType);

/* DVDLow functions */
void __DVDInitWA(void);
void DVDLowReset(void);
BOOL DVDLowStopMotor(DVDLowCallback callback);
BOOL DVDLowWaitCoverClose(DVDLowCallback callback);
void __DVDLowSetWAType(u32 type, u32 location);

/* DVD ID utilities */
BOOL DVDCompareDiskID(const DVDDiskID* id1, const DVDDiskID* id2);

/* DVDError functions */
void __DVDStoreErrorCode(u32 error);

/* DVD file operations */
s32  DVDConvertPathToEntrynum(const char* path);
BOOL DVDFastOpen(s32 entrynum, DVDFileInfo* fileInfo);
BOOL DVDOpen(const char* path, DVDFileInfo* fileInfo);
BOOL DVDClose(DVDFileInfo* fileInfo);
s32 DVDRead(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset, s32 prio);
BOOL DVDReadAbsAsyncPrio(DVDCommandBlock* block, void* addr, s32 length,
                         s32 offset, DVDCBCallback callback, s32 prio);
BOOL DVDCancel(DVDCommandBlock* block);
u32  DVDGetCurrentDir(void);

/* DVD state */
void DVDResume(void);
s32  DVDGetCommandBlockStatus(DVDCommandBlock* block);

/* FST globals - actual types defined in individual .c files */

/* __fstLoad - declared above as BOOL __fstLoad(void) via FST section */
/* __DVDPushWaitingQueue - declared above as BOOL __DVDPushWaitingQueue(s32, DVDCommandBlock*) */

#endif /* DOLPHIN_DVD_DVD_H */
