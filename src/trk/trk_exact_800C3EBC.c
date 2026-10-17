#include "dolphin/types.h"

typedef struct CircleBuffer {
    u8* readPtr;
    u8* writePtr;
    u8* buffer;
    u32 size;
    u32 used;
    u32 free;
    u32 state;
} CircleBuffer;

extern const char lbl_8026FD4C[];
extern const char lbl_8026FD60[];
extern u8 lbl_803FED78[];
extern CircleBuffer lbl_803FF578;
extern void MWTRACE(s32 level, const char* format, ...);
extern void EXI2_Init(u8** communication, void (*callback)(s32));
extern void CircleBufferInitialize(CircleBuffer* circle, u8* buffer, u32 size);

s32 ddh_cc_initialize(u8** communication, void (*callback)(s32))
{
    MWTRACE(1, lbl_8026FD4C);
    EXI2_Init(communication, callback);
    MWTRACE(1, lbl_8026FD60);
    CircleBufferInitialize(&lbl_803FF578, lbl_803FED78, 0x800);
    return 0;
}
