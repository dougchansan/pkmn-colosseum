/**
 * @file gs_range_801653CC_exact_80165DEC.c
 * @brief Byte-exact gs-engine range, 0x80165DEC - 0x80165FDC.
 */
#include "game/gs_range_801653CC_shared.h"

void fn_80165DEC(const char* path, void* buffer, u32 capacity)
{
    const char* messages = lbl_80273548;
    u32 alignedSize;
    void* file = fn_80167F28(path);
    u32 size;

    if (file == 0) {
        GSlogWrite(messages + 0xE0, path);
        return;
    }

    size = fn_80167E5C(file);
    if (size == 0) {
        GSlogWrite(messages + 0xFC, path);
    } else {
        alignedSize = (size + 0x1F) & ~0x1FU;
        if (alignedSize < capacity) {
            if (buffer == 0) {
                GSlogWrite(messages + 0x11C);
            } else {
                memset(buffer, 0xE0, alignedSize);
                if (fn_80167ED0(file, buffer, alignedSize, 0) <= 0) {
                    GSlogWrite(messages + 0x140, path);
                }
            }
        } else {
            GSlogWrite(messages + 0x168);
        }
    }

    fn_80167E64(file);
}

void fn_80165EE0(void* wave, void* buffer, u32 size)
{
    memset(buffer, 0xE0, size);
    lbl_8047B0B0 = size;
    lbl_8047B0B4 = buffer;
    fn_8017AF6C(0x99, wave);
}

void fn_80165F40(void)
{
    lbl_8047B0B4 = GSresGetResource(0, 0x406);
    lbl_8047B0B0 = 0x10000;
    fn_8017B370(0x99);
    fn_80165FDC(0x99);
}

void fn_80165F84(void)
{
    GSresRegisterResource(fn_8016604C(0x10000), 0, 0x406, 0);
    GSresRegisterResource(fn_8016604C(0x2000), 0, 0x407, 0);
    lbl_8047B0AC = 0;
    lbl_8047B0A8 = 0;
}
