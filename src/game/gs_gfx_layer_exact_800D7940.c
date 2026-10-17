#include "dolphin/types.h"

extern void gfxBegin(u16 count);
extern void gfxVtxPos1x16(u16 vertex);
extern void gfxVtxPos1x8(u8 vertex);
extern void gfxVtxNormal1x16(u16 vertex);
extern void gfxVtxNormal1x8(u8 vertex);
extern void gfxVtxColor1x16(u32 layer, u16 vertex);
extern void gfxVtxColor1x8(u32 layer, u8 vertex);
extern void gfxVtxTexCoord1x16(u32 layer, u16 vertex);
extern void gfxVtxTexCoord1x8(u32 layer, u8 vertex);
extern void gfxEnd(void);

void fn_800D7940(u8* object, u16 vertexCount)
{
    u16 vertex;
    s32 i;
    u32 layer;
    u8* entry;

    gfxBegin(vertexCount);
    for (vertex = 0; vertex < vertexCount; vertex++) {
        if (*(s32*)(object + 0x28) == 2) {
            gfxVtxPos1x16(vertex);
        } else {
            gfxVtxPos1x8(vertex);
        }

        if (*(u8*)(object + 0x40) == 1) {
            if (*(s32*)(object + 0x44) == 2) {
                gfxVtxNormal1x16(vertex);
            } else {
                gfxVtxNormal1x8(vertex);
            }
        }

        for (i = 4; i <= 5; i++) {
            entry = object + i * 0x1c;
            if (entry[0x8] == 1) {
                layer = i - 4;
                if (*(s32*)(entry + 0xc) == 2) {
                    gfxVtxColor1x16(layer, vertex);
                } else {
                    gfxVtxColor1x8(layer, vertex);
                }
            }
        }

        for (i = 6; i <= 13; i++) {
            entry = object + i * 0x1c;
            if (entry[0x8] == 1) {
                layer = i - 6;
                if (*(s32*)(entry + 0xc) == 2) {
                    gfxVtxTexCoord1x16(layer, vertex);
                } else {
                    gfxVtxTexCoord1x8(layer, vertex);
                }
            }
        }
    }
    gfxEnd();
}
