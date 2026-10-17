/**
 * @file field_range_exact_80114CA8.c
 * @brief The floorRead* resource readers and _unload* release callbacks,
 *        0x80114CA8 - 0x80115250.
 *
 * Standalone carve of field_range_80114AE0.c. The readers are the FSYS
 * group callbacks lbl_8036C2A0 names for the map (pre only), script, font,
 * message and plain archive entries; each release callback is the GSres
 * release a reader registers. floorReadMapPostFunc (0x80114AE0) stays in
 * field_range_80114AE0.c.
 *
 * Functions are in address order. The unit is built with -opt nopeephole,
 * which replaces the local peephole/scheduling pragmas the candidate source
 * carried.
 */

#include "dolphin/types.h"

extern void GSlogWrite(const char* fmt, ...);
extern void* GSresAllocResourceAlign(u32 size, u32 alignment, u32 group, u32 id, void* release);
extern void* GSresGetResource(u32 group, u32 id);
extern u8 fn_800FF548(void); /* the current floor's fade-out flag */
extern u8 fn_800FF554(void); /* the current floor's fade-in flag */
extern void cms_RegisterScript(void* script);
extern s32 cms_UnregisterScript(void* script);
extern void floorStartSoundScript(void);
extern void* GSmsgFontOpen(void* font);
extern s32 GSmsgFontClose(void* font);
extern void GSmsgOpen(void* msg);
extern s32 GSmsgClose(void* msg);
extern void flareRelease(void);
extern void gsParticleKillAll(void* bank);
extern void GScameraFree(void* camera);
extern void GSlightFree(void* light);
extern s32 GScolsys2UnloadCCD(void);
extern void GStextureFree(void* texture);

extern const char lbl_802724E8[]; /* "floorReadMapPreFunc(): can't alloc %d bytes of memory" */
extern const char lbl_80272520[]; /* "floorReadScriptPreFunc(): can't alloc ..." */
extern const char lbl_8027255C[]; /* "floorReadFontPreFunc(): can't alloc ..." */
extern const char lbl_80272594[]; /* "floorReadMsgPreFunc(): can't alloc ..." */
extern const char lbl_802725CC[]; /* "floorReadNormalPreFunc(): can't alloc ..." */

u32 _unloadScript__FPvUlUl(void* data, u32 group, u32 id);
u32 _unloadFont__FPvUlUl(void* data, u32 group, u32 id);
u32 _unloadMsg__FPvUlUl(void* data, u32 group, u32 id);

/* A map archive: its 0x60-byte HSD_Archive header, then the file. */
void* floorReadMapPreFunc(u32 group, u32 id, u32 size) {
    u32 total = ((size + 0x1F) & ~0x1F) + 0x60;
    u8* archive = GSresAllocResourceAlign(total, 0x20, group, id, NULL);

    if (archive == NULL) {
        GSlogWrite(lbl_802724E8, total);
        return NULL;
    }
    return archive + 0x60;
}

void* floorReadScriptPostFunc(u32 group, u32 id) {
    void* script = GSresGetResource(group, id);

    if (fn_800FF548() == 0 && script != NULL) {
        cms_RegisterScript(script);
        floorStartSoundScript();
    }
    return script;
}

void* floorReadScriptPreFunc(u32 group, u32 id, u32 size) {
    void* buffer;

    if (fn_800FF548() != 0) {
        return NULL;
    }
    size = (size + 0x1F) & ~0x1F;
    buffer = GSresAllocResourceAlign(size, 0x20, group, id, _unloadScript__FPvUlUl);
    if (buffer == NULL) {
        GSlogWrite(lbl_80272520, size);
    }
    return buffer;
}

void* floorReadFontPostFunc(u32 group, u32 id) {
    void* font;

    if (fn_800FF548() != 0) {
        return NULL;
    }
    font = GSresGetResource(group, id);
    if (font != NULL) {
        GSmsgFontOpen(font);
    }
    return font;
}

void* floorReadFontPreFunc(u32 group, u32 id, u32 size) {
    void* buffer;

    if (fn_800FF548() != 0) {
        return NULL;
    }
    size = (size + 0x1F) & ~0x1F;
    buffer = GSresAllocResourceAlign(size, 0x20, group, id, _unloadFont__FPvUlUl);
    if (buffer == NULL) {
        GSlogWrite(lbl_8027255C, size);
    }
    return buffer;
}

void* floorReadMsgPostFunc(u32 group, u32 id) {
    void* msg;

    if (fn_800FF548() != 0) {
        return NULL;
    }
    msg = GSresGetResource(group, id);
    if (msg != NULL) {
        GSmsgOpen(msg);
    }
    return msg;
}

void* floorReadMsgPreFunc(u32 group, u32 id, u32 size) {
    void* buffer;

    if (fn_800FF548() != 0) {
        return NULL;
    }
    size = (size + 0x1F) & ~0x1F;
    buffer = GSresAllocResourceAlign(size, 0x20, group, id, _unloadMsg__FPvUlUl);
    if (buffer == NULL) {
        GSlogWrite(lbl_80272594, size);
    }
    return buffer;
}

/* A plain entry. The error reports the unrounded size. */
void* floorReadNormalPreFunc(u32 group, u32 id, u32 size) {
    void* buffer = GSresAllocResourceAlign((size + 0x1F) & ~0x1F, 0x20, group, id, NULL);

    if (buffer == NULL) {
        GSlogWrite(lbl_802725CC, size);
    }
    return buffer;
}

u32 _unloadFlare__FPvUlUl(void* data, u32 group, u32 id) {
    flareRelease();
    return 1;
}

u32 _unloadParticles__FPvUlUl(void* data, u32 group, u32 id) {
    gsParticleKillAll(data);
    return 1;
}

u32 _unloadCamera__FPvUlUl(void* data, u32 group, u32 id) {
    GScameraFree(data);
    return 1;
}

u32 _unloadLight__FPvUlUl(void* data, u32 group, u32 id) {
    GSlightFree(data);
    return 1;
}

/* A resource read while a floor fades in is kept (release vetoed). */
u32 _unloadScript__FPvUlUl(void* data, u32 group, u32 id) {
    if (fn_800FF554() != 0) {
        return 0;
    }
    cms_UnregisterScript(data);
    return 1;
}

u32 _unloadFont__FPvUlUl(void* data, u32 group, u32 id) {
    if (fn_800FF554() != 0) {
        return 0;
    }
    GSmsgFontClose(data);
    return 1;
}

u32 _unloadMsg__FPvUlUl(void* data, u32 group, u32 id) {
    if (fn_800FF554() != 0) {
        return 0;
    }
    GSmsgClose(data);
    return 1;
}

u32 _unloadColsys__FPvUlUl(void* data, u32 group, u32 id) {
    GScolsys2UnloadCCD();
    return 1;
}

u32 _unloadTexture__FPvUlUl(void* data, u32 group, u32 id) {
    GStextureFree(data);
    return 1;
}
