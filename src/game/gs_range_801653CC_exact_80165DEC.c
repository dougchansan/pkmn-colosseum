/**
 * @file gs_range_801653CC_exact_80165DEC.c
 * @brief Byte-exact gs-engine range, 0x80165DEC - 0x80165FDC.
 */
#include "game/gs_range_801653CC_shared.h"

void soundReadStreamFile(const char* path, void* buffer, u32 capacity)
{
    const char* messages = lbl_80273548;
    u32 alignedSize;
    void* file = GSdvdOpen(path);
    u32 size;

    if (file == 0) {
        GSlogWrite(messages + 0xE0, path);
        return;
    }

    size = GSdvdGetLength(file);
    if (size == 0) {
        GSlogWrite(messages + 0xFC, path);
    } else {
        alignedSize = (size + 0x1F) & ~0x1FU;
        if (alignedSize < capacity) {
            if (buffer == 0) {
                GSlogWrite(messages + 0x11C);
            } else {
                memset(buffer, 0xE0, alignedSize);
                if (GSdvdRead(file, buffer, alignedSize, 0) <= 0) {
                    GSlogWrite(messages + 0x140, path);
                }
            }
        } else {
            GSlogWrite(messages + 0x168);
        }
    }

    GSdvdClose(file);
}

void soundLoadGroupBuffer(void* wave, void* buffer, u32 size)
{
    memset(buffer, 0xE0, size);
    lbl_8047B0B0 = size;
    lbl_8047B0B4 = buffer;
    fsysLoadEntryInto(0x99, wave);
}

void soundLoadStreamBuffer(void)
{
    lbl_8047B0B4 = GSresGetResource(0, 0x406);
    lbl_8047B0B0 = 0x10000;
    fsysStartLoadExclusive(0x99);
    soundWaitGroup(0x99);
}

void soundAllocBuffers(void)
{
    GSresRegisterResource(soundAllocLocked(0x10000), 0, 0x406, 0);
    GSresRegisterResource(soundAllocLocked(0x2000), 0, 0x407, 0);
    lbl_8047B0AC = 0;
    lbl_8047B0A8 = 0;
}
