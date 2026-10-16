/**
 * @file gs_range_8017F3F8_middle.c
 * @brief gs-engine code, 0x8017F3F8 - 0x8017FA5C (7 fns): the REL floor
 *        resource callbacks and the file-resource cache list.
 *
 * Exact island carved out of the 0x8017F2C4 - 0x80180C78 range. The
 * neighbouring LZSS decoder (gs_range_8017F2C4.c) and heap/ARQ code
 * (gs_range_8017FA5C_suffix.c) stay CodeCandidate residuals.
 *
 * The whole range is optimisation-level-0 code (peephole and scheduling
 * still on): leaf functions keep dead induction counters in r30/r31, and
 * single-use parameters and locals get stack homes. configure.py builds
 * this unit with a unit-wide `-opt level=0`; every function here matches
 * with that one setting and no local pragmas.
 *
 * fn_8017F3F8 / fn_8017F484 are the pre/post read callbacks of floor
 * resource type 0x0E in the floor read table (lbl_8036C2A0); the post
 * callback relocates and links the loaded REL (common_rel) and runs its
 * prolog. fn_8017F6B4 is the release callback that runs the epilog and
 * unlinks it.
 */
#include "dolphin/types.h"
#include "dolphin/os/OSModule.h"

extern void* memcpy(void* dst, const void* src, u32 n);
extern void DCFlushRange(void* addr, u32 nBytes);
extern void* GSresGetResource(u32 group, u32 handle);
extern void* GSresAllocResourceAlign(u32 size, u32 alignment, u32 group,
                                      u32 handle, void* releaseCallback);
extern void fn_800F9210(u32 group, u32 handle);
extern u16 fn_800E2B00(u32 size, u32 alignment);
extern void* fn_800E27B0(u16 handle);
extern u16 fn_800E202C(void* ptr);
extern void fn_800E24B0(u16 handle);
extern void fn_800E209C(u16 handle);
extern BOOL OSLink(OSModuleInfo* module, void* bss);
extern BOOL fn_8009EFE4(OSModuleInfo* module);
extern void fn_8017FB08(void* allocation);
extern void* fn_8017FDB0(u32 size);

s32 fn_8017F6B4(void* unused, u32 group, u32 handle);

/* Pre-read callback: reserve 32-byte aligned room for the REL image. */
void* fn_8017F3F8(u32 group, u32 handle, u32 size)
{
    u32 alignedSize = (size + 0x1F) & ~0x1F;
    /* Fetched and never used; retail keeps the store (stw r0,0xc(r1)). */
    void* existing = GSresGetResource(group, handle);
    void* buffer = GSresAllocResourceAlign(alignedSize, 0x20, group, handle, NULL);

    if (!buffer) {
        return NULL;
    }
    return buffer;
}

/*
 * Temporary block from the tail of the GS heap (handle alloc + pointer).
 * Recovered helper: fn_8017F484 expands it twice with the same call
 * sequence, each time routing the result through its own return temp
 * (r22 -> r29, r20 -> r29).
 */
static inline void* memAllocTail(u32 size)
{
    u32 alignedSize = (size + 0x1F) & ~0x1F;
    u16 handle = fn_800E2B00(alignedSize, 0x20);

    if (handle) {
        return fn_800E27B0(handle);
    }
    return NULL;
}

/*
 * Post-read callback: move the REL image into a resource block with room
 * for its bss (or its fixed-link tail), link it and run its prolog.
 */
void* fn_8017F484(u32 group, u32 handle, u32 size)
{
    OSModuleHeader* resource;
    OSModuleHeader* module;
    void* bss;
    void* image;
    u32 alignedSize;
    u32 bssSize;
    /*
     * Retail stores to both of these and never reads them: `unused` is
     * zeroed on entry (stw r0,0xc(r1)) and `copy` receives the temporary
     * image in the bss path (stw r29,0x10(r1)).
     */
    void* copy;
    void* unused = NULL;
    u16 imageHandle;

    bss = NULL;
    image = NULL;
    resource = GSresGetResource(group, handle);
    module = resource;
    if (module->info.version >= 3) {
        if (module->bssSize != 0) {
            alignedSize = (size + 0x1F) & ~0x1F;
            bssSize = (module->bssSize + 0x1F) & ~0x1F;
            image = memAllocTail(alignedSize);
            memcpy(image, resource, size);
            copy = image;
            fn_800F9210(group, handle);
            resource = GSresAllocResourceAlign(alignedSize + bssSize, 0x20, group,
                                               handle, fn_8017F6B4);
            memcpy(resource, image, alignedSize);
            module = resource;
            bss = (u8*)module + alignedSize;
            bss = (void*)(((u32)bss + 0x1F) & ~0x1F);
            OSLink(&module->info, bss);
        } else {
            alignedSize = (size + 0x1F) & ~0x1F;
            image = memAllocTail(alignedSize);
            memcpy(image, resource, size);
            fn_800F9210(group, handle);
            resource = GSresAllocResourceAlign(alignedSize, 0x20, group, handle,
                                               fn_8017F6B4);
            DCFlushRange(image, alignedSize);
            memcpy(resource, image, alignedSize);
            module = resource;
            bss = (u8*)module + module->fixSize;
            bss = (void*)(((u32)bss + 0x1F) & ~0x1F);
            OSLink(&module->info, bss);
        }
    } else {
        OSLink(&module->info, bss);
    }
    if (module->prolog) {
        ((void (*)(void))module->prolog)();
    }
    if (image) {
        imageHandle = fn_800E202C(image);
        if (imageHandle) {
            fn_800E24B0(imageHandle);
            fn_800E209C(imageHandle);
        }
    }
    return resource;
}

/* Release callback: run the REL epilog and unlink it. */
s32 fn_8017F6B4(void* unused, u32 group, u32 handle)
{
    OSModuleHeader* resource;
    OSModuleHeader* module;
    /* Zeroed on entry and never read (stw r0,0x10(r1)). */
    void* result = NULL;

    resource = GSresGetResource(group, handle);
    module = resource;
    if (module->epilog) {
        ((void (*)(void))module->epilog)();
    }
    fn_8009EFE4(&module->info);
    return 1;
}

typedef struct GsRangeCacheNode {
    void* data;
    struct GsRangeCacheNode* prev;
    struct GsRangeCacheNode* next;
    s32 size;
    u32 fileHandle;
    u32 key1;
    u32 key2;
    s32 active;
} GsRangeCacheNode;

typedef struct GsRangeCache {
    GsRangeCacheNode* nodes;
    GsRangeCacheNode* last;
    u32 capacity;
    s32 count;
} GsRangeCache;

extern GsRangeCache lbl_80454038;

s32 fn_8017F728(u32 fileHandle, u32 key1, u32 key2)
{
    GsRangeCacheNode* node;
    s32 i;

    node = lbl_80454038.nodes;
    i = 0;
    while (node) {
        if (node->fileHandle == fileHandle && node->key1 == key1 &&
            node->key2 == key2)
        {
            return node->size;
        }
        node = node->next;
        i++;
    }
    return 0;
}

void* fn_8017F794(u32 fileHandle, u32 key1, u32 key2)
{
    GsRangeCacheNode* node;
    s32 i;

    node = lbl_80454038.nodes;
    i = 0;
    while (node) {
        if (node->fileHandle == fileHandle && node->key1 == key1 &&
            node->key2 == key2)
        {
            return node->data;
        }
        node = node->next;
        i++;
    }
    return NULL;
}

void fn_8017F800(u32 fileHandle)
{
    GsRangeCacheNode* node;
    GsRangeCacheNode* prev;
    GsRangeCacheNode* next;
    s32 i;

    node = lbl_80454038.nodes;
    i = 0;
    while (node) {
        if (node->active != 0 && node->fileHandle == fileHandle) {
            next = node->next;
            prev = node->prev;
            if (next) {
                next->prev = node->prev;
            }
            if (prev) {
                prev->next = node->next;
            }
            lbl_80454038.count--;
            node->size = 0;
            node->fileHandle = 0;
            node->key1 = 0;
            node->key2 = 0;
            if (node->data) {
                fn_8017FB08(node->data);
                node->data = NULL;
            }
            node->active = 0;
            if (lbl_80454038.last == node) {
                node->next = NULL;
                lbl_80454038.last = node->prev;
            }
        }
        node = node->next;
        i++;
    }
}

void* fn_8017F928(s32 size, u32 fileHandle, u32 key1, u32 key2)
{
    GsRangeCacheNode* node;
    u32 i;
    u32 alignedSize;

    node = &lbl_80454038.nodes[1];
    for (i = 1; i < lbl_80454038.capacity; node++, i++) {
        if (node->active == 0) {
            node->next = NULL;
            alignedSize = (size + 0x1F) & ~0x1F;
            node->prev = lbl_80454038.last;
            node->data = fn_8017FDB0(alignedSize);
            if (node->data) {
                node->size = size;
                node->fileHandle = fileHandle;
                node->key1 = key1;
                node->key2 = key2;
                if (lbl_80454038.last) {
                    lbl_80454038.last->next = node;
                }
                lbl_80454038.last = node;
                lbl_80454038.count++;
                node->active = 1;
            }
            return node->data;
        }
    }
    return NULL;
}
