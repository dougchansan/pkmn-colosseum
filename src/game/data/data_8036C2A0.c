#include "dolphin/types.h"

#pragma section ".data"

extern void* lbl_8036C2A0[];
extern void* lbl_8036C3E0[];

extern u8 floorReadBGMPostFunc[];
extern u8 floorReadBGMPreFunc[];
extern u8 floorReadCameraPostFunc[];
extern u8 floorReadCameraPreFunc[];
extern u8 floorReadColPostFunc[];
extern u8 floorReadColPreFunc[];
extern u8 floorReadFontPostFunc[];
extern u8 floorReadFontPreFunc[];
extern u8 floorReadGFLPostFunc[];
extern u8 floorReadGFLPreFunc[];
extern u8 floorReadMapPostFunc[];
extern u8 floorReadMapPreFunc[];
extern u8 floorReadMsgPostFunc[];
extern u8 floorReadMsgPreFunc[];
extern u8 floorReadNormalPreFunc[];
extern u8 floorReadNotLinkedParticlePostFunc[];
extern u8 floorReadNotLinkedParticlePreFunc[];
extern u8 floorReadObjPostFunc[];
extern u8 floorReadObjPreFunc[];
extern u8 floorReadPKXPreFunc[];
extern u8 floorReadParticlePostFunc[];
extern u8 floorReadParticlePreFunc[];
extern u8 floorReadScriptPostFunc[];
extern u8 floorReadScriptPreFunc[];
extern u8 floorReadTexPostFunc[];
extern u8 floorReadTexPreFunc[];
extern u8 floorReadWZXPreFunc[];
extern u8 floorReadRelModulePostFunc[];
extern u8 floorReadRelModulePreFunc[];
extern u8 fsysStatus8000[];
extern u8 fsysStatus9999[];
extern u8 fsysStatus600[];
extern u8 fsysStatus500[];
extern u8 fsysAllocLZSSJob[];
extern u8 fsysStatus305[];
extern u8 fsysStatus304[];
extern u8 fsysSlotStatusFinishRead[];
extern u8 fsysStatus302[];
extern u8 fsysStatus301[];
extern u8 fsysStatus300[];
extern u8 fsysStatus162[];
extern u8 fsysStatus161[];
extern u8 fsysStatus160[];
extern u8 fsysStatus152[];
extern u8 fsysStatus151[];
extern u8 fsysSlotStatusStartEntries[];
extern u8 fsysStatus102[];
extern u8 fsysStatus101[];
extern u8 fsysStatus401[];
extern u8 fsysStatus400[];
extern u8 fsysStatus201[];
extern u8 fsysStatus200[];
extern u8 fsysStatus100[];
extern u8 fsysSlotStatus2000[];
extern u8 fsysStatus1000[];
extern u8 fsysStatus5[];
extern u8 fsysStatus4[];
extern u8 fsysStatus3[];
extern u8 fsysStatus2[];
extern u8 fsysStatus1[];
extern u8 fsysStatus0[];
extern u8 fn_8017F3F8[];
extern u8 fn_8017F484[];

/* Auto-carved .data unit 0x8036C2A0..0x8036C4E8 (2 objects). Non-relocated data as byte-exact u8[]; pointer/jump tables as void*[] for R_PPC_ADDR32 relocations. */

void* lbl_8036C2A0[80] = {
    (void*)0x00000320,
    (void*)0x00000000,
    (void*)((u8*)floorReadNormalPreFunc),
    (void*)0x00000000,
    (void*)0x00000258,
    (void*)0x00000001,
    (void*)((u8*)floorReadMapPreFunc),
    (void*)((u8*)floorReadMapPostFunc),
    (void*)0x000002BC,
    (void*)0x00000002,
    (void*)((u8*)floorReadObjPreFunc),
    (void*)((u8*)floorReadObjPostFunc),
    (void*)0x00000320,
    (void*)0x00000003,
    (void*)((u8*)floorReadColPreFunc),
    (void*)((u8*)floorReadColPostFunc),
    (void*)0x0000012C,
    (void*)0x00000004,
    (void*)((u8*)floorReadBGMPreFunc),
    (void*)((u8*)floorReadBGMPostFunc),
    (void*)0x00000190,
    (void*)0x00000005,
    (void*)((u8*)floorReadMsgPreFunc),
    (void*)((u8*)floorReadMsgPostFunc),
    (void*)0x000001F4,
    (void*)0x00000006,
    (void*)((u8*)floorReadFontPreFunc),
    (void*)((u8*)floorReadFontPostFunc),
    (void*)0x000000C8,
    (void*)0x00000007,
    (void*)((u8*)floorReadScriptPreFunc),
    (void*)((u8*)floorReadScriptPostFunc),
    (void*)0x00000320,
    (void*)0x00000008,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000384,
    (void*)0x00000009,
    (void*)((u8*)floorReadTexPreFunc),
    (void*)((u8*)floorReadTexPostFunc),
    (void*)0x00000320,
    (void*)0x0000000A,
    (void*)((u8*)floorReadParticlePreFunc),
    (void*)((u8*)floorReadParticlePostFunc),
    (void*)0x00000320,
    (void*)0x0000000B,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000320,
    (void*)0x0000000C,
    (void*)((u8*)floorReadCameraPreFunc),
    (void*)((u8*)floorReadCameraPostFunc),
    (void*)0x00000320,
    (void*)0x0000000D,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000064,
    (void*)0x0000000E,
    (void*)((u8*)fn_8017F3F8),
    (void*)((u8*)fn_8017F484),
    (void*)0x000002BC,
    (void*)0x0000000F,
    (void*)((u8*)floorReadPKXPreFunc),
    (void*)0x00000000,
    (void*)0x00000320,
    (void*)0x00000010,
    (void*)((u8*)floorReadWZXPreFunc),
    (void*)0x00000000,
    (void*)0x000003B6,
    (void*)0x00000011,
    (void*)((u8*)floorReadGFLPreFunc),
    (void*)((u8*)floorReadGFLPostFunc),
    (void*)0x00000320,
    (void*)0x00000012,
    (void*)((u8*)floorReadNotLinkedParticlePreFunc),
    (void*)((u8*)floorReadNotLinkedParticlePostFunc),
    (void*)0x00000064,
    (void*)0x00000013,
    (void*)((u8*)floorReadRelModulePreFunc),
    (void*)((u8*)floorReadRelModulePostFunc),
};

void* lbl_8036C3E0[66] = {
    (void*)0x00000000,
    (void*)((u8*)fsysStatus0),
    (void*)0x00000001,
    (void*)((u8*)fsysStatus1),
    (void*)0x00000002,
    (void*)((u8*)fsysStatus2),
    (void*)0x00000003,
    (void*)((u8*)fsysStatus3),
    (void*)0x00000004,
    (void*)((u8*)fsysStatus4),
    (void*)0x00000005,
    (void*)((u8*)fsysStatus5),
    (void*)0x000003E8,
    (void*)((u8*)fsysStatus1000),
    (void*)0x000007D0,
    (void*)((u8*)fsysSlotStatus2000),
    (void*)0x00000064,
    (void*)((u8*)fsysStatus100),
    (void*)0x00000065,
    (void*)((u8*)fsysStatus101),
    (void*)0x00000066,
    (void*)((u8*)fsysStatus102),
    (void*)0x00000067,
    (void*)((u8*)fsysAllocLZSSJob),
    (void*)0x00000096,
    (void*)((u8*)fsysSlotStatusStartEntries),
    (void*)0x00000097,
    (void*)((u8*)fsysStatus151),
    (void*)0x00000098,
    (void*)((u8*)fsysStatus152),
    (void*)0x000000A0,
    (void*)((u8*)fsysStatus160),
    (void*)0x000000A1,
    (void*)((u8*)fsysStatus161),
    (void*)0x000000A2,
    (void*)((u8*)fsysStatus162),
    (void*)0x0000012C,
    (void*)((u8*)fsysStatus300),
    (void*)0x0000012D,
    (void*)((u8*)fsysStatus301),
    (void*)0x0000012E,
    (void*)((u8*)fsysStatus302),
    (void*)0x00000130,
    (void*)((u8*)fsysStatus304),
    (void*)0x00000131,
    (void*)((u8*)fsysStatus305),
    (void*)0x0000012F,
    (void*)((u8*)fsysSlotStatusFinishRead),
    (void*)0x000001F4,
    (void*)((u8*)fsysStatus500),
    (void*)0x00000258,
    (void*)((u8*)fsysStatus600),
    (void*)0x000000C8,
    (void*)((u8*)fsysStatus200),
    (void*)0x000000C9,
    (void*)((u8*)fsysStatus201),
    (void*)0x00000190,
    (void*)((u8*)fsysStatus400),
    (void*)0x00000191,
    (void*)((u8*)fsysStatus401),
    (void*)0x0000270F,
    (void*)((u8*)fsysStatus9999),
    (void*)0x00001F40,
    (void*)((u8*)fsysStatus8000),
    (void*)0xFFFFFFFF,
    (void*)0x00000000,
};

