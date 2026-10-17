#include "game/battle/battle_waza_types.h"

typedef struct WazaPoolHeader {
    void* entries;
    u16 count;
    u8 active;
    u8 pad07;
    void* resourceA;
    void* resourceB;
    u16 handle;
} WazaPoolHeader;

u8 lbl_80467C80[0x40];
u8 lbl_80467CC0[0x14];
u8 lbl_80467CD4[0x24];

/* RULE-EXCEPTION(title-path): unreferenced BSS-order helper, dead-stripped
 * at link time; the same pattern is recorded for GSmsg in RULE_EXCEPTIONS.md. */
static void wazaResetBssLayout(void) {
    lbl_80467C80[0] = 0;
    lbl_80467CC0[0] = 0;
    lbl_80467CD4[0] = 0;
}

void fn_801DADC0(void* context) {
    extern void* floorDataBiosGetCurrentPtr(void);
    extern u32 floorDataBiosGetGroupID(void*);
    extern u32 floorDataBiosGetShadowReciveNum(void*);
    extern void* floorDataBiosGetShadowReciveID(void*, u32);
    extern void* floorDataBiosGetShadowLightID(void*);
    extern void* GSresGetResource(u32, u32);
    extern void wazaSequenceSysClearWork(void);
    extern u16 _toolentryAlloc__FUl(u32);
    extern void* GSmemLock(u16);
    u32 groupId;
    void* floor;
    void* resource;
    s32 receiverCount;
    u16 handle;
    s32 count = (s32)context;
    s32 i;

    memset(lbl_80467CC0, 0, sizeof(WazaPoolHeader));
    memset(lbl_80467CD4, 0, 0x20);
    floor = floorDataBiosGetCurrentPtr();
    groupId = floorDataBiosGetGroupID(floor);
    receiverCount = floorDataBiosGetShadowReciveNum(floor);
    lbl_8047B414 = 0;
    for (i = 0; i < receiverCount; i++) {
        resource = GSresGetResource(groupId, (u32)floorDataBiosGetShadowReciveID(floor, i));
        if (resource != NULL) {
            ((void**)lbl_80467C80)[lbl_8047B414++] = resource;
        }
    }
    resource = floorDataBiosGetShadowLightID(floor);
    if (resource != NULL) {
        lbl_8047B418 = (s32)GSresGetResource(groupId, (u32)resource);
    }
    if (count != 0) {
        wazaSequenceSysClearWork();
        receiverCount = count * 0x8C;
        handle = _toolentryAlloc__FUl(receiverCount);
        if (handle != 0) {
            ((WazaPoolHeader*)lbl_80467CC0)->handle = handle;
            ((WazaPoolHeader*)lbl_80467CC0)->count = count;
            ((WazaPoolHeader*)lbl_80467CC0)->entries = GSmemLock(handle);
            memset(((WazaPoolHeader*)lbl_80467CC0)->entries, 0, receiverCount);
            battleCameraWazaInit();
            ((WazaPoolHeader*)lbl_80467CC0)->resourceA = NULL;
            ((WazaPoolHeader*)lbl_80467CC0)->resourceB = NULL;
            ((WazaPoolHeader*)lbl_80467CC0)->active = 0;
        }
    }
}

void wazaSequenceSysInit(s32 count) {
    extern void* floorDataBiosGetCurrentPtr(void);
    extern u32 floorDataBiosGetGroupID(void*);
    extern u32 floorDataBiosGetShadowReciveNum(void*);
    extern void* floorDataBiosGetShadowReciveID(void*, u32);
    extern void* floorDataBiosGetShadowLightID(void*);
    extern void* GSresGetResource(u32, u32);
    extern void wazaSequenceSysClearWork(void);
    extern u16 _toolentryAlloc__FUl(u32);
    extern void* GSmemLock(u16);
    u32 groupId;
    void* floor;
    void* resource;
    s32 receiverCount;
    u16 handle;
    s32 i;

    memset(lbl_80467CC0, 0, sizeof(WazaPoolHeader));
    memset(lbl_80467CD4, 0, 0x20);
    floor = floorDataBiosGetCurrentPtr();
    groupId = floorDataBiosGetGroupID(floor);
    receiverCount = floorDataBiosGetShadowReciveNum(floor);
    lbl_8047B414 = 0;
    for (i = 0; i < receiverCount; i++) {
        resource = GSresGetResource(groupId, (u32)floorDataBiosGetShadowReciveID(floor, i));
        if (resource != NULL) {
            ((void**)lbl_80467C80)[lbl_8047B414++] = resource;
        }
    }
    resource = floorDataBiosGetShadowLightID(floor);
    if (resource != NULL) {
        lbl_8047B418 = (s32)GSresGetResource(groupId, (u32)resource);
    }
    if (count != 0) {
        wazaSequenceSysClearWork();
        receiverCount = count * 0x8C;
        handle = _toolentryAlloc__FUl(receiverCount);
        if (handle != 0) {
            ((WazaPoolHeader*)lbl_80467CC0)->handle = handle;
            ((WazaPoolHeader*)lbl_80467CC0)->count = count;
            ((WazaPoolHeader*)lbl_80467CC0)->entries = GSmemLock(handle);
            memset(((WazaPoolHeader*)lbl_80467CC0)->entries, 0, receiverCount);
            battleCameraWazaInit();
            fn_801DE598(0x6F7, 0);
            ((WazaPoolHeader*)lbl_80467CC0)->resourceA = GSresGetResource(0x6F7, 0x11EF2400);
            ((WazaPoolHeader*)lbl_80467CC0)->resourceB = GSresGetResource(0x6F7, 0x11EE2400);
            ((WazaPoolHeader*)lbl_80467CC0)->active = 0;
        }
    }
}
