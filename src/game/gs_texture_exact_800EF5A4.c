#include "game/gs_texture.h"

extern void GSmemUnlock(u16 handle);
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
    GSmemUnlock(texture->memHandle);
    GSmemFree(texture->memHandle);
}
