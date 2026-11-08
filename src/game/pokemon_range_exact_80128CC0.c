/**
 * @file pokemon_range_exact_80128CC0.c
 * @brief Exact Pokemon pointer-accessor tail, 0x80128CC0 - 0x80128E38.
 */
#include "dolphin/types.h"

extern u32 lbl_8047ADB8;
extern u8 lbl_80408400[];

void* savedataGetSection1C45C(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1C45C;
}

void* savedataGetSection1B014(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1B014;
}

void* savedataBiosGetE3E8Ptr(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0xE3E8;
}

void* savedataGetSection9A1C(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x9A1C;
}

void* savedataGetSection82A8(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x82A8;
}

void* savedataGetSection8168(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x8168;
}

void* savedataGetSection7D20(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x7D20;
}

void* savedataGetSection1BE5C(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1BE5C;
}

void* savedataGetSection1BDDC(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1BDDC;
}

void* savedataGetSection1BDBC(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1BDBC;
}

void* savedataGetSectionB88(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0xB88;
}

void* savedataGetSection70(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x70;
}

u32 savedataGetSection0(void* ptr) {
    if (ptr != NULL) { return (u32)ptr; }
    return 0;
}

void randSetSeed(void* ptr) {
    if (ptr == NULL) { return; }
    lbl_8047ADB8 = (u32)ptr;
}

u32 savedataGetWorkPtr(void) {
    return lbl_8047ADB8;
}

void* savedataGetBuffer(void) {
    return (void*)lbl_80408400;
}
