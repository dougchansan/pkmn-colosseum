/**
 * @file gs_res.c
 * @brief GSres -- GSAPI resource registry (allocation and registration half).
 *
 * Address range: 0x800F915C - 0x800F96E4 (8 functions).
 * XD class: game/pxdvs/GSAPI/GSres/GSres.cpp
 *
 * The unit starts at the three release functions (fn_800F915C, fn_800F9210,
 * GSresFindById): they walk this registry's table, and fn_800F9210 is the
 * resFindEntry lookup below inlined (its found-path `b` over the NULL
 * store is the inline's return), so they cannot belong to the pad manager
 * that precedes them.
 *
 * The registry is a flat table of GSresEntry records allocated from GSmem
 * by GSresInit.  An entry is live while its data pointer is non-NULL and is
 * looked up by the (group, id) key pair.  Entries created by
 * GSresAllocResource/GSresAllocResourceAlign own a GSmem block (memHandle);
 * entries added with GSresRegisterResource wrap caller-owned data
 * (memHandle == 0).  The release callback is consulted by the free routines
 * (fn_800F915C / fn_800F9210) before an entry is
 * dropped: it receives (data, group, id) and returns zero to veto the
 * release.
 */
#include "dolphin/types.h"

typedef u8 (*GSresReleaseFunc)(void* data, u32 group, u32 id);

typedef struct GSresEntry {
    /* 0x00 */ u16 memHandle;           /* GSmem handle, 0 if not owned */
    /* 0x04 */ void* data;              /* resource pointer, NULL = free slot */
    /* 0x08 */ u32 group;               /* first lookup key */
    /* 0x0C */ u32 id;                  /* second lookup key */
    /* 0x10 */ GSresReleaseFunc release;
} GSresEntry; /* size 0x14 */

/* GSmem.  The allocators are seen here as returning the handle in a full
 * word: GSresInit re-extends the result (clrlwi) and passes the extended
 * value on to GSmemGetPtr, which a u16-returning prototype does not produce. */
extern u32 _toolentryAlloc__FUl(u32 size);      /* GSmemAllocRaw */
extern u32 GSmemAlloc(u32 size, u32 align);    /* GSmemAlloc */
extern void* GSmemLock(u16 handle);           /* GSmemGetPtr */
extern void GSmemFree(u16 handle);            /* GSmemFree */
extern void* fn_800E24B0(u16 handle);           /* GSmemLock */

/* Registry state (.sbss, owned outside this split) */
extern u16 lbl_8047AC58;        /* GSmem handle of the entry table */
extern GSresEntry* lbl_8047AC5C; /* entry table */
extern u32 lbl_8047AC60;        /* entry count */

static inline GSresEntry* resFindEntry(u32 group, u32 id) {
    GSresEntry* entry = lbl_8047AC5C;
    u32 i;

    for (i = lbl_8047AC60; i != 0; i--) {
        if (entry->data != NULL && entry->group == group && entry->id == id) {
            return entry;
        }
        entry++;
    }
    return NULL;
}

static inline GSresEntry* resFindFreeEntry(void) {
    GSresEntry* entry = lbl_8047AC5C;
    u32 i;

    for (i = 0; i < lbl_8047AC60; i++) {
        if (entry->data == NULL) {
            return entry;
        }
        entry++;
    }
    return NULL;
}

/* 0x800F915C | 0xB4 -- release every entry of a group */
void fn_800F915C(u32 group) {
    u32 i;
    GSresEntry* entry;

    entry = lbl_8047AC5C;
    i = lbl_8047AC60;

    for (; i-- != 0; entry++) {
        if (entry->data == NULL || entry->group != group) {
            continue;
        }
        if (entry->release != NULL &&
            !entry->release(entry->data, entry->group, entry->id)) {
            continue;
        }
        if (entry->memHandle != 0) {
            fn_800E24B0(entry->memHandle);
            GSmemFree(entry->memHandle);
            entry->memHandle = 0;
        }
        entry->data = NULL;
    }
}

/* 0x800F9210 | 0xC4 -- release one entry */
void fn_800F9210(u32 group, u32 id) {
    GSresEntry* entry = resFindEntry(group, id);

    if (entry == NULL) {
        return;
    }
    if (entry->release != NULL &&
        !entry->release(entry->data, entry->group, entry->id)) {
        return;
    }
    if (entry->memHandle != 0) {
        fn_800E24B0(entry->memHandle);
        GSmemFree(entry->memHandle);
        entry->memHandle = 0;
    }
    entry->data = NULL;
}

/* 0x800F92D4 | 0x44 -- first live entry with this id, any group */
void* GSresFindById(u32 id) {
    GSresEntry* entry = lbl_8047AC5C;
    u32 i;

    for (i = lbl_8047AC60; i != 0; i--) {
        if (entry->data != NULL && entry->id == id) {
            return entry->data;
        }
        entry++;
    }
    return NULL;
}

/* 0x800F9318 | 0x60 */
void* GSresGetResource(u32 group, u32 id) {
    GSresEntry* entry = resFindEntry(group, id);

    if (entry == NULL) {
        return NULL;
    }
    return entry->data;
}

/* 0x800F9378 | 0xA0 */
void GSresRegisterResource(void* data, u32 group, u32 id,
                           GSresReleaseFunc release) {
    GSresEntry* entry;

    if (resFindEntry(group, id) != NULL) {
        return;
    }
    entry = resFindFreeEntry();
    if (entry == NULL) {
        return;
    }
    entry->memHandle = 0;
    entry->data = data;
    entry->group = group;
    entry->id = id;
    entry->release = release;
}

/* 0x800F9418 | 0x12C */
void* GSresAllocResourceAlign(u32 size, u32 align, u32 group, u32 id,
                              GSresReleaseFunc release) {
    GSresEntry* entry;

    if (resFindEntry(group, id) != NULL) {
        return NULL;
    }
    entry = resFindFreeEntry();
    if (entry == NULL) {
        return NULL;
    }
    entry->memHandle = GSmemAlloc(size, align);
    if (entry->memHandle == 0) {
        return NULL;
    }
    entry->data = GSmemLock(entry->memHandle);
    if (entry->data == NULL) {
        GSmemFree(entry->memHandle);
        return NULL;
    }
    entry->group = group;
    entry->id = id;
    entry->release = release;
    return entry->data;
}

/* 0x800F9544 | 0x12C */
void* GSresAllocResource(u32 size, u32 group, u32 id,
                         GSresReleaseFunc release) {
    GSresEntry* entry;

    if (resFindEntry(group, id) != NULL) {
        return NULL;
    }
    entry = resFindFreeEntry();
    if (entry == NULL) {
        return NULL;
    }
    entry->memHandle = _toolentryAlloc__FUl(size);
    if (entry->memHandle == 0) {
        return NULL;
    }
    entry->data = GSmemLock(entry->memHandle);
    if (entry->data == NULL) {
        GSmemFree(entry->memHandle);
        return NULL;
    }
    entry->group = group;
    entry->id = id;
    entry->release = release;
    return entry->data;
}

/* 0x800F9670 | 0x74 */
void GSresInit(u32 count) {
    u16 handle;
    u32 i;

    lbl_8047AC60 = count;
    handle = _toolentryAlloc__FUl(count * sizeof(GSresEntry));
    lbl_8047AC58 = handle;
    if (handle == 0) {
        return;
    }
    lbl_8047AC5C = GSmemLock(handle);
    for (i = 0; i < lbl_8047AC60; i++) {
        lbl_8047AC5C[i].data = NULL;
    }
}
