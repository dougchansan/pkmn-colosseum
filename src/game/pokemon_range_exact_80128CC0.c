/**
 * @file pokemon_range_exact_80128CC0.c
 * @brief Exact Pokemon pointer-accessor tail, 0x80128CC0 - 0x80128E38.
 */
#include "dolphin/types.h"

extern u32 lbl_8047ADB8;
extern u8 lbl_80408400[];

void* fn_80128CC0(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1C45C;
}

void* fn_80128CDC(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1B014;
}

void* savedataBiosGetE3E8Ptr(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0xE3E8;
}

void* fn_80128D14(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x9A1C;
}

void* fn_80128D30(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x82A8;
}

void* fn_80128D4C(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x8168;
}

void* fn_80128D68(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x7D20;
}

void* fn_80128D80(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1BE5C;
}

void* fn_80128D9C(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1BDDC;
}

void* fn_80128DB8(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x1BDBC;
}

void* fn_80128DD4(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0xB88;
}

void* fn_80128DEC(void* ptr) {
    if (ptr == NULL) { return NULL; }
    return (u8*)ptr + 0x70;
}

u32 fn_80128E04(void* ptr) {
    if (ptr != NULL) { return (u32)ptr; }
    return 0;
}

void fn_80128E14(void* ptr) {
    if (ptr == NULL) { return; }
    lbl_8047ADB8 = (u32)ptr;
}

u32 savedataGetWorkPtr(void) {
    return lbl_8047ADB8;
}

void* fn_80128E2C(void) {
    return (void*)lbl_80408400;
}
