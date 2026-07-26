/** Candidate-only owner for 0x800A839C - 0x800A880C. */
#include "src/dolphin/vi/VI_range_800A8178.c"

typedef struct {
    u8 diskID[0x20];
    u8 padding[0x18];
    void* fstLocation;
    u32 fstMaxLength;
} FSTBootInfo;

typedef struct {
    u32 bootFilePosition;
    u32 fstPosition;
    u32 fstLength;
    u32 fstMaxLength;
    void* fstAddress;
} FSTBootBlock;

extern u8 bb2Buf[];
extern DVDCommandBlock fstLoadBlock;
extern FSTBootBlock* bb2_8047A83C;
extern DVDDiskID* idTmp_8047A840;
extern void* OSGetArenaHi(void);
extern void OSSetArenaHi(void* arenaHi);
extern void OSReport(const char* format, ...);
extern void* memcpy(void* destination, const void* source, u32 length);

void __fstLoad(void)
{
    FSTBootInfo* bootInfo;
    DVDDiskID* id;
    u8 idTmpBuffer[sizeof(DVDDiskID) + 31];

    OSGetArenaHi();
    bootInfo = (FSTBootInfo*)0x80000000;
    idTmp_8047A840 =
        (DVDDiskID*)(((u32)idTmpBuffer + 31) & ~31);
    bb2_8047A83C = (FSTBootBlock*)(((u32)bb2Buf + 31) & ~31);
    DVDReset();
    DVDReadDiskID(&fstLoadBlock, idTmp_8047A840, cb);
    while (DVDGetDriveStatus() != 0) {
    }
    bootInfo->fstLocation = bb2_8047A83C->fstAddress;
    bootInfo->fstMaxLength = bb2_8047A83C->fstMaxLength;
    id = (DVDDiskID*)bootInfo;
    memcpy(id, idTmp_8047A840, sizeof(DVDDiskID));
    OSReport("\n");
    OSReport("  Game Name ... %c%c%c%c\n", id->gameName[0], id->gameName[1],
             id->gameName[2], id->gameName[3]);
    OSReport("  Company ..... %c%c\n", id->company[0], id->company[1]);
    OSReport("  Disk # ...... %d\n", id->diskNumber);
    OSReport("  Game ver .... %d\n", id->gameVersion);
    OSReport("  Streaming ... %s\n", id->streaming == 0 ? "OFF" : "ON");
    OSReport("\n");
    OSSetArenaHi(bb2_8047A83C->fstAddress);
}
