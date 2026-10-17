/**
 * @file wazaViewer_exact_801D5A94.c
 * @brief wazaViewerLoadSequenceFiles, 0x801D5A94 - 0x801D5DA0.
 *
 * Function-boundary carve of the wazaViewer TU (see wazaViewer.c): load the
 * viewer's effect model and optional animation from the host file system.
 * Built with the TU's GC/1.3.2 profile.
 */

#include "game/battle/battle_waza_types.h"

/** Bare 3-float vector, as passed to the GSmodel setters. */
typedef struct WazaViewerVec {
    /* 0x0 */ f32 x;
    /* 0x4 */ f32 y;
    /* 0x8 */ f32 z;
} WazaViewerVec;

extern u8 lbl_804673F8[];
extern u8 lbl_80279338[];

/* Host-file (DVD) wrappers: open returns the file object, 0 on failure. */
extern void* GSdvdOpen(const char* path);
extern u32 GSdvdGetLength(void* file);
extern void GSdvdClose(void* file);
extern s32 GSdvdRead(void* file, void* buf, u32 size, s32 offset);
extern s32 fn_80167E54(void* file, void* buf, u32 size, s32 offset);

/**
 * wazaViewerGetFilesize - size of a host file, or 0 if it cannot be opened.
 * Named by the diagnostic it prints. Inlined into every caller by -inline auto,
 * so it has no symbol of its own in the shipped image.
 */
static inline u32 wazaViewerGetFilesize(const char* messages, const char* path) {
    u32 size;
    void* file;

    file = GSdvdOpen(path);
    if (file == NULL) {
        GSlogWrite(messages + 0x6C, path);
        return 0;
    }
    size = GSdvdGetLength(file);
    GSdvdClose(file);
    return size;
}

/**
 * wazaViewerLoadFile - read a host file into buf, or write buf back out.
 * All three of this function's diagnostics name it "wazaViewerLoadFile()",
 * including the write one, so retail had a single helper with a direction
 * flag; constant propagation at each inline site keeps only one arm.
 */
static inline void wazaViewerLoadFile(const char* messages, const char* path, void* buf,
                                      u32 size, int writing) {
    void* file;

    file = GSdvdOpen(path);
    if (writing) {
        if (file != NULL) {
            if (fn_80167E54(file, buf, size, 0) < 0) {
                GSlogWrite(messages + 0xF8, path);
            }
            GSdvdClose(file);
        } else {
            GSlogWrite(messages + 0x9C, path);
        }
        return;
    }
    if (file == NULL) {
        GSlogWrite(messages + 0x9C, path);
        return;
    }
    if (GSdvdRead(file, buf, (size + 0x1F) & ~0x1F, 0) < 0) {
        GSlogWrite(messages + 0xC8, path);
        GSdvdClose(file);
        return;
    }
    GSdvdClose(file);
}

void wazaViewerLoadSequenceFiles(s32 slot) {
    extern u32 strlen(const char* s);
    extern u16 GSmemAlloc(u32 size, u32 align);
    extern void* GSmemLock(u16 block);
    extern void GSmodelSetRotation(void* model, WazaViewerVec* rotation);
    extern void GSmodelSetPosition(void* model, WazaViewerVec* position);

    void* modelBuf;
    const u8* viewerData;
    char* modelPath;
    s32 modelSize;
    s32 animSize;
    void* effect;
    char* animPath;
    u16 block;
    void* animBuf;
    void* buf;
    WazaViewerVec rotation;
    WazaViewerVec position;

    viewerData = lbl_80279338;
    modelPath = (char*)(lbl_804673F8 + 0x1C);
    modelSize = 0;
    animSize = 0;
    if (strlen(modelPath) != 0) {
        modelSize = wazaViewerGetFilesize((const char*)viewerData, modelPath);
        animPath = (char*)(lbl_804673F8 + 0x41C);
        if (strlen(animPath) != 0) {
            animSize = wazaViewerGetFilesize((const char*)viewerData, animPath);
        }
    }

    effect = fn_801DB154();
    if (effect == NULL) {
        return;
    }
    if (modelSize == 0) {
        return;
    }
    block = GSmemAlloc((modelSize + 0x1F) & ~0x1F, 0x20);
    if (block == 0) {
        return;
    }
    *(u16*)(lbl_804673F8 + 0x660) = block;
    modelBuf = GSmemLock(block);
    buf = *(void**)(lbl_804673F8 + 0x65C) = modelBuf;
    wazaViewerLoadFile((const char*)viewerData, modelPath, buf, modelSize, FALSE);
    sequenceLoad(effect, *(void**)(lbl_804673F8 + 0x65C));
    *(s32*)(lbl_804673F8 + 0x04) = 1;
    wazaSequenceOwnerSetVisibility(effect, 1);
    rotation = *(WazaViewerVec*)(viewerData + 0x0C);
    position = *(WazaViewerVec*)(viewerData + 0x18);
    position.x = battleGridGetDistance(0);
    GSmodelSetRotation(*(void**)((u8*)effect + 0x24), &rotation);
    GSmodelSetPosition(*(void**)((u8*)effect + 0x24), &position);
    *(s8*)((u8*)effect + 0x76) = -1;
    if (animSize != 0) {
        block = GSmemAlloc((animSize + 0x1F) & ~0x1F, 0x20);
        if (block != 0) {
            *(u16*)(lbl_804673F8 + 0x668) = block;
            animBuf = *(void**)(lbl_804673F8 + 0x664) = GSmemLock(block);
            buf = animBuf;
            animPath = (char*)(lbl_804673F8 + 0x41C);
            wazaViewerLoadFile((const char*)viewerData, animPath, buf, animSize, FALSE);
            if ((u8)fn_801DDB4C(effect, *(void**)(lbl_804673F8 + 0x664))) {
                *(s32*)(lbl_804673F8 + 0x04) = 2;
            } else {
                wazaSequenceSysStopWaza(effect, 0, 0);
            }
        }
    }
    ((void**)(lbl_804673F8 + 0x61C))[slot] = effect;
}