/**
 * @file wazaViewer.c
 * @brief wazaViewer: move-animation viewer thread/init/update/finalize helpers.
 *
 * Split from the former game/battle/battle_waza.c CodeCandidate bucket
 * (0x801D1470-0x801DE698); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit. Shared typedefs and cross-TU
 * forward declarations live in include/game/battle/battle_waza_types.h.
 */

#include "game/battle/battle_waza_types.h"

/** Bare 3-float vector, as passed to the GSscene camera setters. */
typedef struct WazaViewerVec {
    /* 0x0 */ f32 x;
    /* 0x4 */ f32 y;
    /* 0x8 */ f32 z;
} WazaViewerVec;

/**
 * One entry of the viewer's camera-preset table (rodata lbl_80279338, stride
 * 0x24). Entry 0 is the rotation preset; entries 1 and 2 are the two identical
 * "look at the model from the front" presets used by the 2-player and
 * single-player paths.
 */
typedef struct WazaViewerCamPreset {
    /* 0x00 */ WazaViewerVec direction;
    /* 0x0C */ WazaViewerVec position;
    /* 0x18 */ WazaViewerVec view;
} WazaViewerCamPreset;

/**
 * Byte 0x8 of the viewer work area: the two playback control fields the
 * viewer's per-frame callback tests. Both are read for their sign only.
 */
typedef struct WazaViewerCtrl {
    /* 0x80 */ s8 advance : 1;
    /* 0x40 */ s8 paused : 1;
    /* 0x20 */ s8 overlay : 1;
    /* 0x10 */ s8 bounds : 1;
    /* 0x0F */ s8 unused_0F : 4;
} WazaViewerCtrl;

#if !defined(PR409_WAZA_VIEWER_SPLIT) || defined(PR409_WAZA_VIEWER_5328_53D4)

/**
 * _wazaViewerFinalize - Move animation helper: screen flash.
 * Address: 0x801D5328 | Size: 0xAC
 */
void _wazaViewerFinalize(u8 r, u8 g, u8 b, f32 duration) {
    extern u8 lbl_804673F8[];
    extern u32 lbl_8047B3F8;
    extern void fn_801024E8(s32);
    extern void fn_801684F0(u32);
    extern void fn_801E11F0(void);

    GSthreadTerminateGroup(0x58);
    *(u32*)(lbl_804673F8 + 0x66C) = 0;
    fn_801024E8(0);
    *(u32*)(lbl_804673F8 + 0x0C) = 0;
    fn_801684F0(*(u32*)(lbl_804673F8 + 0x10));
    if (*(void**)(lbl_804673F8 + 0x878) != NULL) {
        GStextureFree(*(void**)(lbl_804673F8 + 0x878));
    }
    memset(lbl_804673F8, 0, 0x884);
    if (lbl_804673F8[0x87D] == 0) {
        fn_801E11F0();
    }
    GSgappTerminate((void*)lbl_8047B3F8);
    lbl_8047B3F8 = 0;
    wazaSequenceSysRelease();
}

#endif

#if !defined(PR409_WAZA_VIEWER_SPLIT) || defined(PR409_WAZA_VIEWER_53D4_53D8)

/**
 * _wazaViewerUpdate - Move animation no-op.
 * Address: 0x801D53D4 | Size: 0x4
 */
void _wazaViewerUpdate(void) {
    /* No-op */
}


#endif

#if !defined(PR409_WAZA_VIEWER_SPLIT) || defined(PR409_WAZA_VIEWER_53D8_7E58)

extern u8 lbl_80279338[];

/**
 * wazaViewerGetFilesize - size of a host file, or 0 if it cannot be opened.
 * Named by the diagnostic it prints. Inlined into every caller by -inline auto,
 * so it has no symbol of its own in the shipped image.
 */
static inline s32 wazaViewerGetFilesize(const char* messages, const char* path) {
    extern s32 fn_80167F28(const char* path);
    extern s32 fn_80167E5C(void);
    extern void fn_80167E64(s32 handle);

    s32 handle;
    s32 size;

    handle = fn_80167F28(path);
    if (handle == 0) {
        GSlogWrite(messages + 0x6C, path);
        return 0;
    }
    size = fn_80167E5C();
    fn_80167E64(handle);
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
    extern s32 fn_80167F28(const char* path);
    extern void fn_80167E64(s32 handle);
    extern s32 fn_80167ED0(s32 handle, void* buf, u32 size, s32 offset);
    extern s32 fn_80167E54(s32 handle, void* buf, u32 size, s32 offset);

    s32 handle;

    handle = fn_80167F28(path);
    if (writing) {
        if (handle != 0) {
            if (fn_80167E54(handle, buf, size, 0) < 0) {
                GSlogWrite(messages + 0xF8, path);
            }
            fn_80167E64(handle);
        } else {
            GSlogWrite(messages + 0x9C, path);
        }
        return;
    }
    if (handle == 0) {
        GSlogWrite(messages + 0x9C, path);
        return;
    }
    if (fn_80167ED0(handle, buf, size, 0) < 0) {
        GSlogWrite(messages + 0xC8, path);
        fn_80167E64(handle);
        return;
    }
    fn_80167E64(handle);
}

/**
 * _wazaViewerInitialize - Move animation helper: camera zoom.
 * Address: 0x801D53D8 | Size: 0x8C
 */
void _wazaViewerInitialize(s32 slot, f32 zoom, f32 speed) {
    extern u8 lbl_804673F8[];
    extern struct GSmodel* GSresGetResource(u32 group, u32 handle);
    extern s32 GSthreadCreate(s32, s32, s32, s32, s32, void*);
    extern s32 fn_800057A8(void);

    struct GSmodel* result;
    s32 value;

    fn_801DAEF8(8);
    fn_801D58E4();
    result = GSresGetResource(0, 0x64);
    if (result != 0) {
        GSmodelSetVisibility(result, 0);
    }
    *(s32*)(lbl_804673F8 + 0x66C) = GSthreadCreate(0x14, 0x58, 0x2000, 1, 0, wazaViewerThread);
    value = fn_800057A8();
    *(u32*)lbl_804673F8 = (u32)((u32)((2 - value) | (value - 2)) >> 31);
}

/**
 * wazaViewerThread - The viewer's own GSthread body. Once a second it polls
 * the host filesystem for wazaViewer.wvd; when the file carries command 1 it
 * copies the three 0x200-byte name buffers out of it, re-arms the viewer, and
 * writes the record back with the command cleared.
 * Address: 0x801D5464 | Size: 0x24C
 */
void wazaViewerThread(void) {
    extern u8 lbl_80279338[];
    extern u8 lbl_804673F8[];
    extern s32 fn_80167F28(const char* path);
    extern u32 fn_80167E5C(void);
    extern void fn_80167E64(s32 handle);
    extern s32 fn_80167ED0(s32 handle, void* buf, u32 size, s32 offset);
    extern s32 fn_80167E54(s32 handle, void* buf, u32 size, s32 offset);
    extern u16 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u16 block);
    extern void fn_800E24B0(u16 block);
    extern void fn_800E209C(u16 block);
    extern void cameraUpdate(void);

    const char* messages;
    u32 size;
    char* path;
    void* attackerName;
    void* targetName;
    void* moveName;
    s32* record;
    u16 block;
    void* sequence;

    messages = (const char*)lbl_80279338;
    path = (char*)(lbl_804673F8 + 0x670);
    attackerName = lbl_804673F8 + 0x1C;
    targetName = lbl_804673F8 + 0x21C;
    moveName = lbl_804673F8 + 0x41C;

    while (TRUE) {
        fn_801DB088();
        switch (*(s32*)(lbl_804673F8 + 0x00)) {
        case 1:
            fn_801D624C();
            break;
        case 0:
            sequence = *(void**)(lbl_804673F8 + 0x61C + *(s32*)(lbl_804673F8 + 0x624) * 4);
            if (sequence != NULL) {
                if ((u8)fn_801DA94C(sequence, 0, 0)) {
                    *(s32*)(lbl_804673F8 + 0x870) = 0;
                    break;
                }
            }
            if (*(s32*)(lbl_804673F8 + 0x870) <= 0) {
                size = wazaViewerGetFilesize(messages, path);
                if (size != 0) {
                    block = fn_800E2C04((size + 0x1F) & ~0x1F, 0x20);
                    if (block != 0) {
                        record = (s32*)fn_800E27B0(block);
                        wazaViewerLoadFile(messages, path, record, (size + 0x1F) & ~0x1F, FALSE);
                        if (record[0] == 1) {
                            memcpy(attackerName, (u8*)record + 0x208, 0x200);
                            memcpy(targetName, (u8*)record + 0x408, 0x200);
                            memcpy(moveName, (u8*)record + 0x008, 0x200);
                            lbl_804673F8[0x87C] = record[0x608 / 4];
                            record[1] = 0;
                            record[0] = 0;
                            fn_801D56B0();
                            wazaViewerLoadFile(messages, path, record, size, TRUE);
                        }
                        fn_800E24B0(block);
                        fn_800E209C(block);
                    }
                }
                *(s32*)(lbl_804673F8 + 0x870) = 0x3C;
            } else {
                *(s32*)(lbl_804673F8 + 0x870) -= fn_800D3088();
            }
            break;
        }
        cameraUpdate();
        _threadSwitch();
    }
}

/**
 * fn_801D56B0 - Re-arm the viewer after a sequence file has been loaded:
 * reset the camera, drop the previous sequence, and re-read the new one.
 * Address: 0x801D56B0 | Size: 0x234
 */
void fn_801D56B0(void) {
    extern u8 lbl_80279338[];
    extern u8 lbl_804673F8[];
    extern void GSscene_SetCameraDirectionVector(WazaViewerVec* src);
    extern void GSscene_SetCameraPositionVector(WazaViewerVec* src);
    extern void GSscene_SetCameraViewVector(WazaViewerVec* src);
    extern void GSscene_SetMode(s32 mode);
    extern s32 fn_800057A8(void);
    extern void GSmodelDisableColorSwap(void* model);
    extern void DVDInit(void);
    extern void fn_801DB100(void* sequence);
    extern void fn_800E24B0(u16 handle);
    extern void fn_800E209C(u16 handle);
    extern u32 strlen(const char* s);

    const WazaViewerCamPreset* presets;
    WazaViewerVec direction;
    WazaViewerVec position;
    WazaViewerVec view;
    void* sequence;

    presets = (const WazaViewerCamPreset*)lbl_80279338;
    if (fn_800057A8() == 2) {
        direction = presets[2].direction;
        position = presets[2].position;
        view = presets[2].view;
        GSscene_SetCameraDirectionVector(&direction);
        GSscene_SetCameraPositionVector(&position);
        GSscene_SetCameraViewVector(&view);
        GSscene_SetMode(2);
        *(s32*)(lbl_804673F8 + 0x00) = 0;
    } else {
        GSscene_SetMode(7);
        *(s32*)(lbl_804673F8 + 0x00) = 1;
    }

    memset(lbl_804673F8 + 0x08, 0, 4);
    *(s32*)(lbl_804673F8 + 0x14) = 0;
    *(s32*)(lbl_804673F8 + 0x18) = -1;
    sequence = *(void**)(lbl_804673F8 + 0x61C + *(s32*)(lbl_804673F8 + 0x624) * 4);
    if (sequence != NULL) {
        sequence = *(void**)((u8*)sequence + 0x24);
        if (sequence != NULL) {
            GSmodelDisableColorSwap(sequence);
        }
    }
    *(s32*)(lbl_804673F8 + 0x65C) = 0;
    *(s32*)(lbl_804673F8 + 0x664) = 0;
    *(s32*)(lbl_804673F8 + 0x0C) = 0;
    DVDInit();
    *(s32*)(lbl_804673F8 + 0x63C) = 0;
    switch (*(s32*)(lbl_804673F8 + 0x00)) {
    case 0:
        fn_801DB100(*(void**)(lbl_804673F8 + 0x61C));
        if (*(u16*)(lbl_804673F8 + 0x660) != 0) {
            fn_800E24B0(*(u16*)(lbl_804673F8 + 0x660));
            fn_800E209C(*(u16*)(lbl_804673F8 + 0x660));
            *(u16*)(lbl_804673F8 + 0x660) = 0;
            if (*(u16*)(lbl_804673F8 + 0x668) != 0) {
                fn_800E24B0(*(u16*)(lbl_804673F8 + 0x668));
                fn_800E209C(*(u16*)(lbl_804673F8 + 0x668));
                *(u16*)(lbl_804673F8 + 0x668) = 0;
            }
        }
        memset(lbl_804673F8 + 0x61C, 0, 8);
        fn_801D5A94(0);
        break;
    case 1:
        memset(lbl_804673F8 + 0x61C, 0, 8);
        break;
    }

    if (strlen((char*)(lbl_804673F8 + 0x21C)) != 0) {
        fn_801D5DA0();
    }
    if (fn_800057A8() == 2) {
        GSscene_SetMode(2);
    } else {
        GSscene_SetMode(7);
    }
}

/**
 * fn_801D58E4 - Reset the viewer work area and (re)create its GS resources.
 * Address: 0x801D58E4 | Size: 0x1B0
 */
void fn_801D58E4(void) {
    extern u8 lbl_80279338[];
    extern u8 lbl_804673F8[];
    extern u32 lbl_8047B3F8;
    extern void GSscene_SetCameraDirectionVector(WazaViewerVec* src);
    extern void GSscene_SetCameraPositionVector(WazaViewerVec* src);
    extern void GSscene_SetCameraViewVector(WazaViewerVec* src);
    extern void GSscene_SetMode(s32 mode);
    extern s32 fn_800057A8(void);
    extern void* GSfilterCreate(u8* color);
    extern u8 fn_801E11E8(void);
    extern void fn_801E1258(void);
    extern void fn_801C31EC(void);
    extern u32 GSgappCreate(s32 group, s32 priority, s32 flags, void* callback);

    const WazaViewerCamPreset* presets;
    WazaViewerVec direction;
    WazaViewerVec position;
    WazaViewerVec view;
    u8 color[4];

    presets = (const WazaViewerCamPreset*)lbl_80279338;
    if (fn_800057A8() == 2) {
        direction = presets[1].direction;
        position = presets[1].position;
        view = presets[1].view;
        GSscene_SetCameraDirectionVector(&direction);
        GSscene_SetCameraPositionVector(&position);
        GSscene_SetCameraViewVector(&view);
        GSscene_SetMode(2);
        *(s32*)(lbl_804673F8 + 0x00) = 0;
    } else {
        GSscene_SetMode(7);
        *(s32*)(lbl_804673F8 + 0x00) = 1;
    }

    *(s32*)(lbl_804673F8 + 0x04) = 0;
    memset(lbl_804673F8 + 0x08, 0, 4);
    *(s32*)(lbl_804673F8 + 0x14) = 0;
    *(s32*)(lbl_804673F8 + 0x18) = -1;
    color[3] = 0;
    color[2] = 0;
    color[1] = 0;
    color[0] = 0;
    *(void**)(lbl_804673F8 + 0x10) = GSfilterCreate(color);
    *(s32*)(lbl_804673F8 + 0x65C) = 0;
    *(s32*)(lbl_804673F8 + 0x664) = 0;
    *(u16*)(lbl_804673F8 + 0x660) = 0;
    *(u16*)(lbl_804673F8 + 0x668) = 0;
    memset(lbl_804673F8 + 0x61C, 0, 8);
    *(s32*)(lbl_804673F8 + 0x624) = 0;
    *(s32*)(lbl_804673F8 + 0x870) = 0;
    *(s32*)(lbl_804673F8 + 0x0C) = 0;
    *(s32*)(lbl_804673F8 + 0x880) = 0;
    *(s32*)(lbl_804673F8 + 0x878) = 0;
    *(s32*)(lbl_804673F8 + 0x874) = 1;
    lbl_804673F8[0x87D] = fn_801E11E8();
    fn_801E1258();
    fn_801C31EC();
    lbl_8047B3F8 = GSgappCreate(1, 0x7F, 0, fn_801D603C);
    *(s32*)(lbl_804673F8 + 0x63C) = 0;
}

/**
 * fn_801D5A94 - Move animation helper: combined effect sequence.
 * Address: 0x801D5A94 | Size: 0x30C
 */
void fn_801D5A94(s32 slot) {
    extern u8 lbl_80279338[];
    extern u8 lbl_804673F8[];
    extern u32 strlen(const char* s);
    extern s32 fn_80167F28(const char* path);
    extern s32 fn_80167E5C(void);
    extern void fn_80167E64(s32 handle);
    extern s32 fn_80167ED0(s32 handle, void* buf, u32 size, s32 offset);
    extern u16 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u16 block);
    extern void GSmodelSetRotation(void* model, WazaViewerVec* rotation);
    extern void GSmodelSetPosition(void* model, WazaViewerVec* position);

    const u8* viewerData;
    char* modelPath;
    char* animPath;
    void* effect;
    s32 modelSize;
    s32 animSize;
    u16 block;
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
    block = fn_800E2C04((modelSize + 0x1F) & ~0x1F, 0x20);
    if (block == 0) {
        return;
    }
    *(u16*)(lbl_804673F8 + 0x660) = block;
    buf = fn_800E27B0(block);
    *(void**)(lbl_804673F8 + 0x65C) = buf;
    wazaViewerLoadFile((const char*)viewerData, modelPath, buf, (modelSize + 0x1F) & ~0x1F, FALSE);
    sequenceLoad(effect, *(void**)(lbl_804673F8 + 0x65C));
    *(s32*)(lbl_804673F8 + 0x04) = 1;
    fn_801DA4E8(effect, 1);
    rotation = *(WazaViewerVec*)(viewerData + 0x0C);
    position = *(WazaViewerVec*)(viewerData + 0x18);
    position.x = battleGridGetDistance(0);
    GSmodelSetRotation(*(void**)((u8*)effect + 0x24), &rotation);
    GSmodelSetPosition(*(void**)((u8*)effect + 0x24), &position);
    *(s8*)((u8*)effect + 0x76) = -1;
    if (animSize != 0) {
        block = fn_800E2C04((animSize + 0x1F) & ~0x1F, 0x20);
        if (block != 0) {
            *(u16*)(lbl_804673F8 + 0x668) = block;
            buf = fn_800E27B0(block);
            *(void**)(lbl_804673F8 + 0x664) = buf;
            animPath = (char*)(lbl_804673F8 + 0x41C);
            wazaViewerLoadFile((const char*)viewerData, animPath, buf, (animSize + 0x1F) & ~0x1F, FALSE);
            if ((u8)fn_801DDB4C(effect, *(void**)(lbl_804673F8 + 0x664))) {
                *(s32*)(lbl_804673F8 + 0x04) = 2;
            } else {
                fn_801DA8C4(effect, 0, 0);
            }
        }
    }
    *(void**)(lbl_804673F8 + 0x61C + slot * 4) = effect;
}

/**
 * fn_801D5DA0 - Move animation helper: element-specific rendering.
 * Address: 0x801D5DA0 | Size: 0x29C
 */
void fn_801D5DA0(void) {
    extern u8 lbl_80279338[];
    extern u8 lbl_804673F8[];
    extern s32 fn_80167F28(const char* path);
    extern s32 fn_80167E5C(void);
    extern void fn_80167E64(s32 handle);
    extern s32 fn_80167ED0(s32 handle, void* buf, u32 size, s32 offset);
    extern u16 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u16 block);
    extern void fn_800E24B0(u16 block);
    extern void fn_800E209C(u16 block);
    extern void GSmodelEnableColorSwap(void* model, s32 a, s32 b, s32 c, s32 d);
    extern void GSmodelEnableModulation(void* model, u8* color);

    const char* messages;
    char* path;
    void* sequence;
    s32 size;
    u16 block;
    s32* record;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 value;
    u8 color[4];

    messages = (const char*)lbl_80279338;
    sequence = *(void**)(lbl_804673F8 + 0x61C + *(s32*)(lbl_804673F8 + 0x624) * 4);
    if (sequence == NULL) {
        return;
    }
    if (*(void**)((u8*)sequence + 0x24) == NULL) {
        return;
    }

    path = (char*)(lbl_804673F8 + 0x21C);
    size = wazaViewerGetFilesize(messages, path);
    if (size == 0) {
        return;
    }

    block = fn_800E2C04((size + 0x1F) & ~0x1F, 0x20);
    if (block == 0) {
        return;
    }
    record = (s32*)fn_800E27B0(block);
    wazaViewerLoadFile(messages, path, record, (size + 0x1F) & ~0x1F, FALSE);

    switch (record[0]) {
    case 0:
        a = 0;
        break;
    case 1:
        a = 1;
        break;
    case 2:
        a = 2;
        break;
    case 3:
        a = 3;
        break;
    }
    switch (record[1]) {
    case 0:
        b = 0;
        break;
    case 1:
        b = 1;
        break;
    case 2:
        b = 2;
        break;
    case 3:
        b = 3;
        break;
    }
    switch (record[2]) {
    case 0:
        c = 0;
        break;
    case 1:
        c = 1;
        break;
    case 2:
        c = 2;
        break;
    case 3:
        c = 3;
        break;
    }
    switch (record[3]) {
    case 0:
        d = 0;
        break;
    case 1:
        d = 1;
        break;
    case 2:
        d = 2;
        break;
    case 3:
        d = 3;
        break;
    }
    value = record[4];
    fn_800E24B0(block);
    fn_800E209C(block);
    color[3] = value >> 24;
    color[2] = value >> 16;
    color[1] = value >> 8;
    color[0] = value;
    GSmodelEnableColorSwap(*(void**)((u8*)sequence + 0x24), a, b, c, d);
    GSmodelEnableModulation(*(void**)((u8*)sequence + 0x24), color);
}

/**
 * fn_801D603C - Move animation helper: hit effect rendering.
 * Address: 0x801D603C | Size: 0x210
 */
void fn_801D603C(void) {
    extern u8 lbl_804673F8[];
    extern u8 lbl_80279470[];  /* "SLOW MOTION" */
    extern f32 lbl_8047E2F0;
    extern const char lbl_8047E2F4;  /* "STEP" */
    extern const char lbl_8047E2FC;  /* "PAUSED" */
    extern const char lbl_8047E304;  /* "PLAYING" */
    extern f32 lbl_8047E30C;
    extern void fn_80168408(void* filter, u8* color);
    extern u8 dbgMenuIsOpen(void);
    extern s32 fn_801E11E0(void);
    extern void fn_800FE6A0(f32 width, f32 height);
    extern void fn_800FAEF8(s32 x, s32 y, u32 color, const char* fmt, ...);

    void** models;
    void* sequence;
    void* node;
    s32 state;
    s32 arg1;
    s32 arg2;
    u8 color[4];

    switch (*(s32*)(lbl_804673F8 + 0x00)) {
    case 1:
        models = (void**)(lbl_804673F8 + 0x61C);
        sequence = models[*(s32*)(lbl_804673F8 + 0x624)];
        color[3] = 0;
        color[2] = 0;
        color[1] = 0;
        color[0] = 0;
        fn_80168408(*(void**)(lbl_804673F8 + 0x10), color);
        if (sequence != NULL) {
            if (((WazaViewerCtrl*)(lbl_804673F8 + 0x08))->advance) {
                fn_801D7B94(sequence);
            }
            *(s32*)(lbl_804673F8 + 0x14) = *(u16*)((u8*)sequence + 0x32);
            if (!dbgMenuIsOpen() && !menuIsCheck(0x78)) {
                fn_801D7464();
            }
        }
        fn_801D7230();
        state = fn_801E11E0();
        arg2 = arg1 = 0;
        if (((WazaViewerCtrl*)(lbl_804673F8 + 0x08))->overlay) {
            sequence = models[*(s32*)(lbl_804673F8 + 0x624)];
            if (sequence != NULL) {
                node = *(void**)((u8*)sequence + 0x68);
                if (node != NULL) {
                    arg1 = *(u16*)((u8*)node + 0x2C);
                    arg2 = *(u16*)((u8*)node + 0x2E);
                }
                if ((u8)fn_801DA94C(sequence, arg1, arg2)) {
                    fn_800FE6A0(lbl_8047E2F0, lbl_8047E2F0);
                    switch (state) {
                    case 3:
                        fn_800FAEF8(0xC8, 0x10, 0xFF0000A0, &lbl_8047E2F4);
                        break;
                    case 4:
                        fn_800FAEF8(0xC8, 0x10, 0xFF0000A0, (const char*)lbl_80279470);
                        break;
                    case 1:
                        fn_800FAEF8(0xC8, 0x10, 0xFF0000A0, &lbl_8047E2FC);
                        break;
                    case 2:
                        fn_800FAEF8(0xC8, 0x10, 0xFF0000A0, &lbl_8047E304);
                        break;
                    }
                    fn_800FE6A0(lbl_8047E30C, lbl_8047E30C);
                }
            }
        }
        break;
    case 0:
        fn_801D6A64();
        break;
    }
}

/**
 * wazaViewerSpawnAttacker - respawn the attacker-side effect from the work
 * area's staged parameters, seating it at -distance for its grid slot.
 * mode is the fn_801DDD28 animation mode; passing 0 folds that call away,
 * which is how the camera-reset path spawns without an animation.
 */
static inline void wazaViewerSpawnAttacker(s32 mode) {
    extern u8 lbl_804673F8[];
    extern f32 lbl_8047E2E4;
    extern f32 lbl_8047E2EC;
    extern s32 _fadeEffectGetRandom__FUl(s32 range);
    extern void* fn_801DE190(u16 id, s32 random, u32 useSeed);
    extern void* fn_801DE418(u16 id);
    extern void GSmodelSetPosition(void* model, f32* position);
    extern void GSmodelSetRotation(void* model, f32* rotation);
    extern void fn_801DDD28(void* effect, u16 anim, s32 mode, u8 flag);

    WazaViewerVec position;
    WazaViewerVec rotation;
    s32 slot;
    s32 anim;
    s32 flag;
    s32 seed;
    s32 id;
    void* effect;
    f32 distance;

    slot = *(s32*)(lbl_804673F8 + 0x630);
    flag = *(s32*)(lbl_804673F8 + 0x654);
    anim = *(s32*)(lbl_804673F8 + 0x650);
    seed = *(s32*)(lbl_804673F8 + 0x640);
    id = *(s32*)(lbl_804673F8 + 0x634);
    distance = battleGridGetDistance((u8)slot);
    set__5GSvecFfff((f32*)&position, -distance, lbl_8047E2E4, lbl_8047E2E4);
    set__5GSvecFfff((f32*)&rotation, lbl_8047E2E4, lbl_8047E2EC, lbl_8047E2E4);
    *(void**)(lbl_804673F8 + 0x61C) = NULL;
    if (id == 0) {
        return;
    }
    if (slot != 0) {
        effect = fn_801DE190((u16)id, _fadeEffectGetRandom__FUl(-1),
                             (u32)(-seed | seed) >> 31);
    } else {
        effect = fn_801DE418((u16)id);
    }
    if (effect == NULL) {
        return;
    }
    GSmodelSetPosition(*(void**)((u8*)effect + 0x24), (f32*)&position);
    GSmodelSetRotation(*(void**)((u8*)effect + 0x24), (f32*)&rotation);
    fn_801DA4E8(effect, 1);
    *(s8*)((u8*)effect + 0x76) = -1;
    if (mode != 0 && anim != 0) {
        fn_801DDD28(effect, (u16)anim, mode, (u8)flag);
    }
    *(void**)(lbl_804673F8 + 0x61C) = effect;
}

/** wazaViewerSpawnTarget - the defender-side counterpart, seated at +distance. */
static inline void wazaViewerSpawnTarget(s32 mode) {
    extern u8 lbl_804673F8[];
    extern f32 lbl_8047E2E4;
    extern f32 lbl_8047E2E8;
    extern s32 _fadeEffectGetRandom__FUl(s32 range);
    extern void* fn_801DE190(u16 id, s32 random, u32 useSeed);
    extern void* fn_801DE418(u16 id);
    extern void GSmodelSetPosition(void* model, f32* position);
    extern void GSmodelSetRotation(void* model, f32* rotation);
    extern void fn_801DDD28(void* effect, u16 anim, s32 mode, u8 flag);

    WazaViewerVec position;
    WazaViewerVec rotation;
    s32 slot;
    s32 anim;
    s32 flag;
    s32 seed;
    s32 id;
    void* effect;
    f32 distance;

    slot = *(s32*)(lbl_804673F8 + 0x644);
    flag = *(s32*)(lbl_804673F8 + 0x654);
    anim = *(s32*)(lbl_804673F8 + 0x650);
    seed = *(s32*)(lbl_804673F8 + 0x64C);
    id = *(s32*)(lbl_804673F8 + 0x648);
    distance = battleGridGetDistance((u8)slot);
    set__5GSvecFfff((f32*)&position, distance, lbl_8047E2E4, lbl_8047E2E4);
    set__5GSvecFfff((f32*)&rotation, lbl_8047E2E4, lbl_8047E2E8, lbl_8047E2E4);
    *(void**)(lbl_804673F8 + 0x620) = NULL;
    if (id == 0) {
        return;
    }
    if (slot != 0) {
        effect = fn_801DE190((u16)id, _fadeEffectGetRandom__FUl(-1),
                             (u32)(-seed | seed) >> 31);
    } else {
        effect = fn_801DE418((u16)id);
    }
    if (effect == NULL) {
        return;
    }
    GSmodelSetPosition(*(void**)((u8*)effect + 0x24), (f32*)&position);
    GSmodelSetRotation(*(void**)((u8*)effect + 0x24), (f32*)&rotation);
    fn_801DA4E8(effect, 1);
    *(s8*)((u8*)effect + 0x76) = 1;
    if (mode != 0 && anim != 0) {
        fn_801DDD28(effect, (u16)anim, mode, (u8)flag);
    }
    *(void**)(lbl_804673F8 + 0x620) = effect;
}

/**
 * fn_801D624C - Move animation mega-function.
 * Address: 0x801D624C | Size: 0x818
 * Very large function (~2KB) that orchestrates a complete move
 * animation from start to finish, coordinating particle effects,
 * model animations, camera movements, and sound effects.
 */
void fn_801D624C(void) {
    extern u8 lbl_804673F8[];
    extern f32 lbl_8047E2E4;
    extern f32 lbl_8047E310;
    extern f32 lbl_8047E314;
    extern void GSscene_SetMode(s32 mode);
    extern void cameraSetTarget(s32 a, s32 b);
    extern void cameraSetDistance(f32 distance);
    extern void cameraSetHeight(f32 height);
    extern void cameraSetRotY(f32 angle);

    void** models;
    void* effect;
    s32 i;

    models = (void**)(lbl_804673F8 + 0x61C);
    effect = models[*(s32*)(lbl_804673F8 + 0x624)];
    if (*(s32*)(lbl_804673F8 + 0x628) != 0) {
        for (i = 0; i < 2; i++) {
            if (*(void**)(lbl_804673F8 + 0x61C + i * 4) != NULL) {
                fn_801DB100(*(void**)(lbl_804673F8 + 0x61C + i * 4));
                *(void**)(lbl_804673F8 + 0x61C + i * 4) = NULL;
            }
        }
        if (*(s32*)(lbl_804673F8 + 0x63C) != 0) {
            wazaViewerSpawnAttacker(0);
            *(s32*)(lbl_804673F8 + 0x628) = 0;
            *(s32*)(lbl_804673F8 + 0x62C) = 0;
            *(s32*)(lbl_804673F8 + 0x624) = 0;
            effect = models[*(s32*)(lbl_804673F8 + 0x624)];
            if (effect != NULL) {
                cameraSetTarget(*(s32*)effect, *(s32*)((u8*)effect + 0x04));
                GSscene_SetMode(5);
                cameraSetDistance(lbl_8047E310);
                cameraSetHeight(lbl_8047E314);
                cameraSetRotY(lbl_8047E2E4);
            }
            return;
        }
        switch (*(s32*)(lbl_804673F8 + 0x658)) {
        case 2:
            if (*(s32*)(lbl_804673F8 + 0x634) != 0) {
                wazaViewerSpawnAttacker(4);
            }
            if (*(s32*)(lbl_804673F8 + 0x648) != 0) {
                wazaViewerSpawnTarget(0);
            }
            break;
        case 1:
            if (*(s32*)(lbl_804673F8 + 0x634) != 0) {
                wazaViewerSpawnAttacker(3);
            }
            if (*(s32*)(lbl_804673F8 + 0x648) != 0) {
                wazaViewerSpawnTarget(0);
            }
            break;
        case 0:
        default:
            if (*(s32*)(lbl_804673F8 + 0x634) != 0) {
                wazaViewerSpawnAttacker(1);
            }
            if (*(s32*)(lbl_804673F8 + 0x648) != 0) {
                wazaViewerSpawnTarget(2);
            }
            break;
        }
        *(s32*)(lbl_804673F8 + 0x628) = 0;
        *(s32*)(lbl_804673F8 + 0x62C) = 0;
        *(s32*)(lbl_804673F8 + 0x624) = 0;
        if (models[0] != NULL) {
            *(s32*)(lbl_804673F8 + 0x624) = 0;
        } else if (models[1] != NULL) {
            *(s32*)(lbl_804673F8 + 0x624) = 1;
        }
        return;
    }

    if (*(s32*)(lbl_804673F8 + 0x62C) == 0) {
        return;
    }
    if ((u8)fn_801DAC54(effect)) {
        return;
    }
    *(s32*)(lbl_804673F8 + 0x624) = *(s32*)(lbl_804673F8 + 0x624) + 1;
    while (*(s32*)(lbl_804673F8 + 0x624) < 2) {
        effect = models[*(s32*)(lbl_804673F8 + 0x624)];
        if (effect == NULL) {
            *(s32*)(lbl_804673F8 + 0x624) = *(s32*)(lbl_804673F8 + 0x624) + 1;
            continue;
        }
        if (*(void**)((u8*)effect + 0x68) == NULL) {
            break;
        }
        fn_801DA4E8(effect, 1);
        fn_801DA9E8(effect, *(u16*)((u8*)(*(void**)((u8*)effect + 0x68)) + 0x2C),
                    *(u16*)((u8*)(*(void**)((u8*)effect + 0x68)) + 0x2E));
        break;
    }
    if (*(s32*)(lbl_804673F8 + 0x624) >= 2) {
        *(s32*)(lbl_804673F8 + 0x624) = 0;
        *(s32*)(lbl_804673F8 + 0x62C) = 0;
        if (models[0] != NULL) {
            *(s32*)(lbl_804673F8 + 0x624) = 0;
        } else if (models[1] != NULL) {
            *(s32*)(lbl_804673F8 + 0x624) = 1;
        }
    }
}

/**
 * fn_801D6A64 - Move animation secondary mega-function.
 * Address: 0x801D6A64 | Size: 0x3F4
 */
void fn_801D6A64(void) {
    extern u8 lbl_80279338[];
    extern u8 lbl_804673F8[];
    extern f32 lbl_8047E2F0;
    extern const char lbl_8047E2F4;  /* "STEP" */
    extern const char lbl_8047E2FC;  /* "PAUSED" */
    extern const char lbl_8047E304;  /* "PLAYING" */
    extern f32 lbl_8047E30C;
    extern void fn_80168408(void* filter, u8* color);
    extern u8 dbgMenuIsOpen(void);
    extern s32 fn_801E11E0(void);
    extern void fn_800FE6A0(f32 width, f32 height);
    extern void fn_800FAEF8(s32 x, s32 y, u32 color, const char* fmt, ...);
    extern u16 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u16 block);
    extern void fn_800E24B0(u16 block);
    extern void fn_800E209C(u16 block);

    const char* messages;
    void** models;
    void* effect;
    void* node;
    s32 state;
    s32 arg1;
    s32 arg2;
    char* path;
    u32 size;
    u16 block;
    s32* record;
    u8 color[4];

    messages = (const char*)lbl_80279338;
    models = (void**)(lbl_804673F8 + 0x61C);
    effect = models[*(s32*)(lbl_804673F8 + 0x624)];
    color[3] = 0;
    color[2] = 0;
    color[1] = 0;
    color[0] = 0;
    fn_80168408(*(void**)(lbl_804673F8 + 0x10), color);
    if (effect != NULL) {
        if (((WazaViewerCtrl*)(lbl_804673F8 + 0x08))->advance) {
            fn_801D7B94(effect);
        }
        node = *(void**)((u8*)effect + 0x68);
        if (node != NULL) {
            if (((u8*)node)[0x14] == 0) {
                ((u8*)node)[0x16] = ((WazaViewerCtrl*)(lbl_804673F8 + 0x08))->paused;
            }
        }
        fn_801D6E58(effect);
        *(s32*)(lbl_804673F8 + 0x14) = *(u16*)((u8*)effect + 0x32);
        if (!dbgMenuIsOpen() && !menuIsCheck(0x78)) {
            fn_801D7464();
        }
    }
    fn_801D7230();
    state = fn_801E11E0();
    arg2 = arg1 = 0;
    if (((WazaViewerCtrl*)(lbl_804673F8 + 0x08))->overlay) {
        effect = models[*(s32*)(lbl_804673F8 + 0x624)];
        if (effect != NULL) {
            node = *(void**)((u8*)effect + 0x68);
            if (node != NULL) {
                arg1 = *(u16*)((u8*)node + 0x2C);
                arg2 = *(u16*)((u8*)node + 0x2E);
            }
            if ((u8)fn_801DA94C(effect, arg1, arg2)) {
                fn_800FE6A0(lbl_8047E2F0, lbl_8047E2F0);
                switch (state) {
                case 3:
                    fn_800FAEF8(0xC8, 0x10, 0xFF0000A0, &lbl_8047E2F4);
                    break;
                case 4:
                    fn_800FAEF8(0xC8, 0x10, 0xFF0000A0, messages + 0x138);
                    break;
                case 1:
                    fn_800FAEF8(0xC8, 0x10, 0xFF0000A0, &lbl_8047E2FC);
                    break;
                case 2:
                    fn_800FAEF8(0xC8, 0x10, 0xFF0000A0, &lbl_8047E304);
                    break;
                }
                fn_800FE6A0(lbl_8047E30C, lbl_8047E30C);
            }
        }
    }
    if (fn_801E11E0() == 1) {
        if (*(s32*)(lbl_804673F8 + 0x870) <= 0) {
            path = (char*)(lbl_804673F8 + 0x670);
            size = wazaViewerGetFilesize(messages, path);
            if (size != 0) {
                block = fn_800E2C04((size + 0x1F) & ~0x1F, 0x20);
                if (block != 0) {
                    record = (s32*)fn_800E27B0(block);
                    wazaViewerLoadFile(messages, path, record, (size + 0x1F) & ~0x1F, FALSE);
                    if (record[0] == 1) {
                        memcpy(lbl_804673F8 + 0x1C, (u8*)record + 0x208, 0x200);
                        memcpy(lbl_804673F8 + 0x21C, (u8*)record + 0x408, 0x200);
                        memcpy(lbl_804673F8 + 0x41C, (u8*)record + 0x008, 0x200);
                        lbl_804673F8[0x87C] = record[0x608 / 4];
                        record[1] = 0;
                        record[0] = 0;
                        fn_801D56B0();
                        wazaViewerLoadFile(messages, path, record, size, TRUE);
                    }
                    fn_800E24B0(block);
                    fn_800E209C(block);
                }
            }
            *(s32*)(lbl_804673F8 + 0x870) = 0x3C;
        } else {
            *(s32*)(lbl_804673F8 + 0x870) -= fn_800D3088();
        }
    }
}

/**
 * fn_801D6E58 - Move animation tertiary mega-function.
 * Address: 0x801D6E58 | Size: 0x3D8
 */
void fn_801D6E58(void* effect) {
    extern u8 lbl_804673F8[];
    extern u8 lbl_8027947C[];  /* "scene_data" */
    extern s32 lbl_8047B400;
    extern u16 lbl_8047E2E0;
    extern u8 lbl_8047E2E2;
    extern f32 lbl_8047E2F0;
    extern u8 lbl_80314958[];
    extern void* floorDataBiosGetCurrentPtr(void);
    extern void* fn_80113F48(void);
    extern void* GSresGetResource(u32 archive, u32 resId);
    extern void* HSD_ArchiveGetPublicAddress(void* archive, const char* symbol);
    extern u32 floorReadMakeModelResID(u32 id);
    extern void* GScameraGetActiveCamera(void);
    extern void* GSmodelGetBound(void* model);
    extern void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void);
    extern void GScameraGetPosition(void* camera, WazaViewerVec* position);
    extern void GScameraGetLookAt(void* camera, WazaViewerVec* up, WazaViewerVec* at);
    extern void fn_800E0168(WazaViewerVec* dst, const WazaViewerVec* a, const WazaViewerVec* b);
    extern void ObjInfoInit(void* bound, WazaViewerVec* extent);
    extern f32 fn_800E008C(const WazaViewerVec* v);
    extern void fn_800E0060(WazaViewerVec* dst, const WazaViewerVec* src);
    extern void fn_800E013C(WazaViewerVec* dst, const WazaViewerVec* src, f32 scale);
    extern void GSvecAdd(WazaViewerVec* dst, const WazaViewerVec* a, const WazaViewerVec* b);
    extern void fn_800DFFCC(WazaViewerVec* dst, const WazaViewerVec* a, const WazaViewerVec* b);
    extern void fn_800DA4C4(u32 a, u32 b, u32 c);
    extern void fn_800DA2BC(u32 a, u32 b, u32 c);
    extern void fn_800DA1E8(u32 a, u32 b, u32 c);
    extern void fn_800D9ED8(u32 a);
    extern void fn_800DA028(u32 a);
    extern void fn_800D88DC(u32 a);
    extern void fn_800D888C(u32 a);
    extern void fn_800D7820(u32 a);
    extern void fn_800D6A00(u32 a);
    extern void fn_800D67BC(u32 a);
    extern void fn_800D6680(f32 x, f32 y, f32 z);
    extern void fn_800D5C18(u32 a, u32 r, u32 g, u32 b);
    extern void fn_800D6728(void);

    void* floorData;
    void* archive;
    void* sceneData;
    void* camera;
    void* bound;
    s32 i;
    s32 index;
    u8 green;
    u8 blue;
    f32 extentLength;
    u8 color[4];
    WazaViewerVec eye;
    WazaViewerVec axisX;
    WazaViewerVec forward;
    WazaViewerVec at;
    WazaViewerVec centre;
    WazaViewerVec axisY;
    WazaViewerVec extent;

    floorData = floorDataBiosGetCurrentPtr();
    archive = fn_80113F48();
    index = 0;
    if (lbl_8047B400 != ((WazaViewerCtrl*)(lbl_804673F8 + 0x08))->bounds) {
        sceneData = HSD_ArchiveGetPublicAddress(
            GSresGetResource((u32)fn_80113F48(), *(u32*)((u8*)floorData + 0x08)),
            (const char*)lbl_8027947C);
        if (sceneData == NULL) {
            return;
        }
        if (*(u32*)sceneData != 0) {
            i = floorReadMakeModelResID(*(u32*)((u8*)floorData + 0x08));
            for (; (*(u32**)sceneData)[index] != 0; index++) {
                void* model = GSresGetResource((u32)archive, i | index);
                if (model != NULL) {
                    GSmodelSetVisibility(model, (u8)lbl_8047B400);
                }
            }
        }
        lbl_8047B400 = ((WazaViewerCtrl*)(lbl_804673F8 + 0x08))->bounds;
    }
    if (lbl_8047B400 == 0) {
        return;
    }
    camera = GScameraGetActiveCamera();
    *(u16*)&color[0] = lbl_8047E2E0;
    color[2] = lbl_8047E2E2;
    if (camera == NULL) {
        return;
    }
    if (*(void**)((u8*)effect + 0x24) == NULL) {
        return;
    }
    bound = GSmodelGetBound(*(void**)((u8*)effect + 0x24));
    if (bound == NULL) {
        return;
    }
    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
    GScameraGetPosition(camera, &eye);
    GScameraGetLookAt(camera, &forward, &at);
    fn_800E0168(&axisX, &at, &eye);
    ObjInfoInit(bound, &extent);
    extentLength = fn_800E008C(&axisX);
    {
        f32 length = fn_800E008C(&extent);
        extentLength = length + extentLength;
    }
    fn_800E0060(&axisX, &axisX);
    fn_800E013C(&centre, &axisX, extentLength);
    GSvecAdd(&centre, &centre, &at);
    fn_800E0060(&forward, &forward);
    fn_800DFFCC(&axisY, &axisX, &forward);
    fn_800E0060(&axisY, &axisY);
    fn_800DFFCC(&forward, &axisY, &axisX);
    fn_800E013C(&axisY, &axisY, lbl_8047E2F0 * extentLength);
    fn_800E013C(&forward, &forward, extentLength);
    fn_800DA4C4(0, 1, 0);
    fn_800DA2BC(1, 0, 1);
    fn_800DA1E8(1, 2, 1);
    fn_800D9ED8(0);
    fn_800DA028(0);
    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D7820((u32)lbl_80314958);
    fn_800D6A00(6);
    fn_800D67BC(4);
    green = color[1];
    blue = color[2];
    fn_800D6680(forward.x + (centre.x + axisY.x), forward.y + (centre.y + axisY.y),
                forward.z + (centre.z + axisY.z));
    fn_800D5C18(0, color[0], green, blue);
    fn_800D6680(forward.x + (centre.x - axisY.x), forward.y + (centre.y - axisY.y),
                forward.z + (centre.z - axisY.z));
    fn_800D5C18(0, color[0], green, blue);
    fn_800D6680((centre.x - axisY.x) - forward.x, (centre.y - axisY.y) - forward.y,
                (centre.z - axisY.z) - forward.z);
    fn_800D5C18(0, color[0], green, blue);
    fn_800D6680((centre.x + axisY.x) - forward.x, (centre.y + axisY.y) - forward.y,
                (centre.z + axisY.z) - forward.z);
    fn_800D5C18(0, color[0], green, blue);
    fn_800D6728();
}

/**
 * fn_801D7230 - Move animation finalize.
 * Address: 0x801D7230 | Size: 0x21C
 */
void fn_801D7230(void) {
    extern u8 lbl_80279338[];
    extern u8 lbl_804673F8[];
    extern f32 lbl_8047E2F0;
    extern f32 lbl_8047E30C;
    extern void fn_800FE6A0(f32 width, f32 height);
    extern void fn_800FAEF8(s32 x, s32 y, u32 color, const char* fmt, ...);

    const char* messages;
    s32 flags;
    s32 line;

    messages = (const char*)lbl_80279338;
    line = 0;
    flags = *(s32*)(lbl_804673F8 + 0x0C);
    if (flags != 0) {
        fn_800FE6A0(lbl_8047E2F0, lbl_8047E2F0);
        if (flags & 0x001) {
            fn_800FAEF8(0xA, 0x10 + line * 0xD, -1, messages + 0x150);
            line++;
        }
        if (flags & 0x002) {
            fn_800FAEF8(0xA, 0x10 + line * 0xD, -1, messages + 0x150);
            line++;
        }
        if (flags & 0x004) {
            fn_800FAEF8(0xA, 0x10 + line * 0xD, -1, messages + 0x16C);
            line++;
        }
        if (flags & 0x008) {
            fn_800FAEF8(0xA, 0x10 + line * 0xD, -1, messages + 0x184);
            line++;
        }
        if (flags & 0x010) {
            fn_800FAEF8(0xA, 0x10 + line * 0xD, -1, messages + 0x19C);
            line++;
        }
        if (flags & 0x020) {
            fn_800FAEF8(0xA, 0x10 + line * 0xD, -1, messages + 0x1B0);
            line++;
        }
        if (flags & 0x040) {
            fn_800FAEF8(0xA, 0x10 + line * 0xD, -1, messages + 0x1C8);
            line++;
        }
        if (flags & 0x080) {
            fn_800FAEF8(0xA, 0x10 + line * 0xD, -1, messages + 0x1DC);
            line++;
        }
        if (flags & 0x100) {
            fn_800FAEF8(0xA, 0x10 + line * 0xD, -1, messages + 0x1F4);
            line++;
        }
        if (flags & 0x200) {
            fn_800FAEF8(0xA, 0x10 + line * 0xD, -1, messages + 0x208);
            line++;
        }
        if (flags & 0x400) {
            fn_800FAEF8(0xA, 0x10 + line * 0xD, -1, messages + 0x228);
            line++;
        }
        fn_800FE6A0(lbl_8047E30C, lbl_8047E30C);
    }
}

/**
 * fn_801D744C - Move animation OR bits into lbl_804673F8+0xC.
 * Address: 0x801D744C | Size: 0x18
 */
extern u8 lbl_804673F8[];
void fn_801D744C(u32 bits) {
    *(u32*)(lbl_804673F8 + 0x0C) |= bits;
}

/* =========================================================================
 * CORE WAZA SEQUENCE FUNCTIONS (0x801D7464 - 0x801D9E34)
 *
 * The main waza sequence API: load, update, start/stop entries.
 * These are the functions called from the battle state machine.
 * ========================================================================= */

/**
 * fn_801D7464 / wazaSequenceLoad - Load a complete waza sequence.
 * Address: 0x801D7464 | Size: 0x730
 * Proposed name from symbols: wazaSequenceLoad.
 * Loads all entries (particle, model, camera, sound) for a move's animation.
 * Referenced by battle_logic.c.
 */
void fn_801D7464(void) {
    extern u8 lbl_804673F8[];
    extern u8 lbl_803725A0[];
    extern u8 lbl_8047B3FC;
    extern u8 lbl_8047B3FD;
    extern u8 lbl_8047B3FE;
    extern u8 lbl_8047B3FF;
    extern const f32 lbl_8047E318;
    extern const f32 lbl_8047E31C;
    extern const f32 lbl_8047E320;
    extern const f32 lbl_8047E328;
    extern void GSscene_GetCameraDirectionVector(WazaViewerVec* src);
    extern void GSscene_GetCameraPositionVector(WazaViewerVec* src);
    extern void GSscene_GetCameraViewVector(WazaViewerVec* src);
    extern void GSscene_SetCameraDirectionVector(WazaViewerVec* src);
    extern void GSscene_SetCameraPositionVector(WazaViewerVec* src);
    extern void GSscene_SetCameraViewVector(WazaViewerVec* src);
    extern u8 GSscene_GetMode(void);
    extern void fn_800E0168(WazaViewerVec* dst, const WazaViewerVec* a,
                            const WazaViewerVec* b);
    extern f32 fn_800E008C(const WazaViewerVec* v);
    extern void fn_800E0060(WazaViewerVec* dst, const WazaViewerVec* src);
    extern void fn_800DFFCC(WazaViewerVec* dst, const WazaViewerVec* a,
                            const WazaViewerVec* b);
    extern void fn_800E013C(WazaViewerVec* dst, const WazaViewerVec* src,
                            f32 scale);
    extern void fn_800E0718(void* out, const void* axis, f32 angle);
    extern void GSvecTransformQuat(void* out, const void* quat, const void* vec);
    extern void GSvecAdd(WazaViewerVec* dst, const WazaViewerVec* a,
                         const WazaViewerVec* b);
    extern void set__5GSvecFfff(f32* vec, f32 x, f32 y, f32 z);
    extern u8 fn_800F7AF0(s32 pad);
    extern u32 fn_800F7BC4(s32 pad);
    extern s32 fn_800F7A7C(s32 pad, s32 mode);
    extern s32 fn_800F7A08(s32 pad, s32 mode);
    extern s32 fn_800F7994(s32 pad, s32 mode);
    extern s32 fn_800F7920(s32 pad, s32 mode);
    extern u32 fn_800F7B5C(s32 pad);
    extern void* GScameraGetActiveCamera(void);
    extern void GScameraSetPosition(void* cam, WazaViewerVec* pos);
    extern void GScameraLookAt(void* cam, void* up, WazaViewerVec* at);
    extern void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void);
    extern u8 wazaSequencePokemonMotionStart(void* owner, BOOL enabled);
    extern s32 fn_801DA94C(void* obj, s32 search_key1, s32 search_key2);
    extern void fn_801DA4E8(void* effect, u32 visible);
    extern void fn_801DA9E8(void* sequence, s32 moveID, s32 variant);
    extern void fn_801DF070(void* effect, s32 weatherType, s32 enabled);
    WazaEffect* effect;
    void* node;
    void* camera;
    WazaViewerVec direction;
    WazaViewerVec position;
    WazaViewerVec view;
    WazaViewerVec target;
    WazaViewerVec axisX;
    WazaViewerVec axisY;
    WazaViewerVec forward;
    WazaViewerVec temp;
    f32 angleStep;
    s32 currentIdx;
    s32 motionA;
    s32 motionB;
    s32 moveX;
    s32 moveY;
    s32 rotateX;
    s32 rotateY;

    currentIdx = *(s32*)(lbl_804673F8 + 0x624);
    effect = ((WazaEffect**)(lbl_804673F8 + 0x61C))[currentIdx];
    if (effect == NULL) {
        return;
    }

    node = *(void**)((u8*)effect + 0x68);
    motionA = 0;
    motionB = 0;
    if (node != NULL) {
        motionA = *(u16*)((u8*)node + 0x2C);
        motionB = *(u16*)((u8*)node + 0x2E);
    }

    if (*(s32*)lbl_804673F8 == 0 && GSscene_GetMode() == 2) {
        GSscene_GetCameraDirectionVector(&direction);
        GSscene_GetCameraPositionVector(&position);
        GSscene_GetCameraViewVector(&view);
        GSvecAdd(&target, &position, &view);
        fn_800E0168(&target, &target, &direction);
        fn_800E0060(&target, &target);
        fn_800DFFCC(&axisX, &position, &target);
        fn_800E0060(&axisX, &axisX);
        fn_800DFFCC(&forward, &target, &axisX);
        fn_800E0060(&forward, &forward);

        moveX = fn_800F7A7C(1, 1);
        moveY = fn_800F7A08(1, 1);
        rotateX = fn_800F7994(1, 1);
        rotateY = fn_800F7920(1, 1);

        if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x08) != 0) {
            lbl_8047B3FC = 1;
        }
        if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x04) != 0) {
            lbl_8047B3FD = 1;
        }
        if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x01) != 0) {
            lbl_8047B3FE = 1;
        }
        if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x02) != 0) {
            lbl_8047B3FF = 1;
        }

        angleStep = lbl_8047E318 * (f32)fn_800D3088();
        if (lbl_8047B3FC != 0) {
            fn_800E013C(&temp, &target, angleStep);
            GSvecAdd(&position, &position, &target);
            if ((fn_800F7B5C(1) & fn_800F7AF0(1) & 0x08) != 0) {
                lbl_8047B3FC = 0;
            }
        }
        if (lbl_8047B3FD != 0) {
            fn_800E013C(&temp, &target, angleStep);
            fn_800E0168(&position, &position, &temp);
            if ((fn_800F7B5C(1) & fn_800F7AF0(1) & 0x04) != 0) {
                lbl_8047B3FD = 0;
            }
        }
        if (lbl_8047B3FE != 0) {
            fn_800E013C(&temp, &axisX, angleStep);
            GSvecAdd(&position, &position, &temp);
            if ((fn_800F7B5C(1) & fn_800F7AF0(1) & 0x01) != 0) {
                lbl_8047B3FE = 0;
            }
        }
        if (lbl_8047B3FF != 0) {
            fn_800E013C(&temp, &axisX, angleStep);
            fn_800E0168(&position, &position, &temp);
            if ((fn_800F7B5C(1) & fn_800F7AF0(1) & 0x02) != 0) {
                lbl_8047B3FF = 0;
            }
        }

        if (moveX != 0 || moveY != 0 || rotateX != 0 || rotateY != 0) {
            set__5GSvecFfff((f32*)&temp, lbl_8047E320 * (f32)moveX,
                            lbl_8047E320 * (f32)moveY, 0.0f);
            fn_800E0060(&temp, &temp);
            fn_800DFFCC(&target, &target, &temp);
            if (moveY > 0) {
                if (fn_800E008C(&target) > lbl_8047E320) {
                    fn_800E0060(&target, &target);
                    fn_800E0718(&axisY, &target, lbl_8047E31C * angleStep);
                    GSvecTransformQuat(&target, &axisY, &target);
                }
            }
            if (rotateY != 0) {
                GSvecCopy(&temp, &target);
                fn_800E0060(&temp, &temp);
                if (rotateY > 0) {
                    fn_800E013C(&temp, &temp, angleStep);
                } else {
                    fn_800E013C(&temp, &temp, -angleStep);
                }
                GSvecAdd(&target, &target, &temp);
            }

            GSvecAdd(&target, &target, &view);
            GSscene_SetCameraDirectionVector(&target);
            GSscene_SetCameraPositionVector(&position);
            GSscene_SetCameraViewVector(&view);
            camera = GScameraGetActiveCamera();
            if (camera != NULL) {
                GSvecAdd(&temp, &position, &view);
                GScameraSetPosition(camera, &target);
                GScameraLookAt(camera, lbl_803725A0, &temp);
                _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
            }
        }
    }

    if (*(s32*)(lbl_804673F8 + 0x4) == 1 && *(void**)((u8*)effect + 0x68) == NULL) {
        if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x40) != 0) {
            (*(u16*)((u8*)effect + 0x32))--;
        }
        if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x20) != 0) {
            (*(u16*)((u8*)effect + 0x32))++;
            if (*(u16*)((u8*)effect + 0x32) > *(u16*)((u8*)effect + 0x14) - 1) {
                *(u16*)((u8*)effect + 0x32) = 0;
            }
        }
    }

    if (((WazaViewerCtrl*)(lbl_804673F8 + 0x08))->advance &&
        (fn_800F7BC4(1) & fn_800F7AF0(1) & 0x100) != 0) {
        (*(s32*)(lbl_804673F8 + 0x18))++;
        if (*(s32*)(lbl_804673F8 + 0x18) >= 0x10) {
            *(s32*)(lbl_804673F8 + 0x18) = -1;
        }
    }

    if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x100) != 0 &&
        *(s32*)lbl_804673F8 == 1 && *(void**)((u8*)effect + 0x68) == NULL) {
        wazaSequencePokemonMotionStart(effect, 0);
    }

    if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x100) != 0) {
        if (*(s32*)lbl_804673F8 == 1 && *(s32*)(lbl_804673F8 + 0x63C) != 0) {
            fn_801DF070(effect, *(s32*)(lbl_804673F8 + 0x638), 0);
        } else if (fn_801DA94C(effect, motionA, motionB) == 0) {
            fn_801DA4E8(effect, 1);
            fn_801DA9E8(effect, motionA, motionB);
            *(s32*)(lbl_804673F8 + 0x62C) = 1;
        }
    }
}

/**
 * fn_801D7B94 / wazaSequenceUpdate - Per-frame waza sequence update.
 * Address: 0x801D7B94 | Size: 0x2C4
 * Proposed name from symbols: wazaSequenceUpdate.
 * Called every frame while a move animation is playing.
 * Referenced by battle_logic.c.
 */
void fn_801D7B94(void* effect) {
    extern u8 lbl_804673F8[];
    extern u8 lbl_80314638[];
    extern const f32 lbl_8047E318;
    extern const f32 lbl_8047E30C;
    extern const f32 lbl_8047E330;
    extern const f32 lbl_8047E334;
    extern const f32 lbl_8047E338;
    extern const f32 lbl_8047E33C;
    extern const f32 lbl_8047E340;
    extern const f32 lbl_8047E344;
    extern void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
    extern void* GScameraGetActiveCamera(void);
    extern void GScameraGetPosition(void*, WazaViewerVec*);
    extern void GScameraGetPerspective(
        void*, f32*, f32*, f32*, f32*);
    extern void fn_800DA4C4(s32, s32, s32);
    extern void fn_800DA2BC(s32, s32, s32);
    extern void fn_800DA1E8(s32, s32, s32);
    extern void fn_800DA028(s32);
    extern f32 fn_800E008C(WazaViewerVec*);
    extern void fn_800B944C(s32, s32);
    extern void fn_800D88DC(s32);
    extern void fn_800D888C(s32);
    extern void fn_800D7820(void*);
    extern void fn_800D6A00(s32);
    extern void fn_800D67BC(s32);
    extern void GSpartGetTransform(
        GSpart*, WazaViewerVec*, void*, void*);
    extern void clear__5GSvecFv(WazaViewerVec*);
    extern void fn_800D6680(f32, f32, f32);
    extern void fn_800D5CB8(s32, s32, s32, s32, s32);
    extern void fn_800D6728(void);
    u8* data;
    u8* entry;
    struct GSmodel* model;
    GSpart* part;
    void* camera;
    WazaViewerVec transform;
    WazaViewerVec cameraPosition;
    f32 aspect;
    f32 fov;
    f32 scale;
    s32 partIndex;
    s32 i;

    data = effect;
    model = *(struct GSmodel**)(data + 0x24);
    entry = *(u8**)(data + 0x2C) + *(u16*)(data + 0x32) * 0xD4;
    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(
        *(void**)(data + 0x2C));
    camera = GScameraGetActiveCamera();
    GScameraGetPosition(camera, &cameraPosition);
    GScameraGetPerspective(camera, &fov, &aspect, &aspect, &aspect);

    switch (*(s32*)(data + 0x10)) {
    case -2:
        aspect = lbl_8047E318;
        break;
    case -1:
        aspect = lbl_8047E330;
        break;
    case 0:
        aspect = lbl_8047E30C;
        break;
    case 1:
        aspect = lbl_8047E334;
        break;
    case 2:
        aspect = lbl_8047E338;
        break;
    case 3:
        aspect = lbl_8047E33C;
        break;
    }

    fn_800DA4C4(1, 6, 1);
    fn_800DA2BC(1, 1, 1);
    fn_800DA1E8(0, 7, 0);
    fn_800DA028(0);
    scale = lbl_8047E340 * aspect /
            (fov * fn_800E008C(&cameraPosition));
    if (scale > lbl_8047E30C) {
        scale = lbl_8047E30C;
    }
    fn_800B944C((s32)(lbl_8047E344 * scale), 5);
    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D7820(lbl_80314638);
    fn_800D6A00(0);

    partIndex = *(s32*)(lbl_804673F8 + 0x18);
    if (partIndex < 0) {
        fn_800D67BC(0x10);
        for (i = 0; i < 0x10; i++) {
            part = GSmodelGetPart(
                model, *(s32*)(entry + 0x4C + i * 4));
            if (part != NULL) {
                GSpartGetTransform(part, &transform, NULL, NULL);
                GSpartFree(part);
            } else {
                clear__5GSvecFv(&transform);
                *(u32*)(lbl_804673F8 + 0x0C) |= 2;
            }
            fn_800D6680(transform.x, transform.y, transform.z);
            fn_800D5CB8(0, 0xFF, 0, 0, 0xDC);
        }
        fn_800D6728();
    } else {
        fn_800D67BC(1);
        part = GSmodelGetPart(
            model, *(s32*)(entry + 0x4C + partIndex * 4));
        if (part != NULL) {
            GSpartGetTransform(part, &transform, NULL, NULL);
            GSpartFree(part);
        } else {
            clear__5GSvecFv(&transform);
            *(u32*)(lbl_804673F8 + 0x0C) |= 2;
        }
        fn_800D6680(transform.x, transform.y, transform.z);
        fn_800D5CB8(0, 0xFF, 0, 0, 0xDC);
        fn_800D6728();
    }
}

#endif
