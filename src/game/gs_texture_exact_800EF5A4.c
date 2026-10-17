#include "game/gs_texture.h"

extern void fn_800E24B0(u16 handle);
extern void GSmemFree(u16 handle);

void GStextureFree(GStextureHandle* texture)
{
    if (texture->inUse == 0) {
        return;
    }
    if (texture->memHandle == 0) {
        return;
    }
    texture->inUse = 0;
    fn_800E24B0(texture->memHandle);
    GSmemFree(texture->memHandle);
}
