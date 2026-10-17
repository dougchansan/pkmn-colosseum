#ifndef GAME_GS_RANGE_8017FA5C_SHARED_H
#define GAME_GS_RANGE_8017FA5C_SHARED_H

/*
 * Shared declarations for the gs small-block heap, ARQ transfer queue and
 * GSgapp job pool and its tasks, 0x8017FA5C - 0x801812C4. The range is one
 * retail translation unit built at `-opt level=0` with `-inline deferred`;
 * it is split into several objects only so the exact functions can link on
 * their own. The queue, the cache area and the arena are defined (and pooled) by
 * gs_range_8017FA5C_exact_801800F8.c.
 */

#include "dolphin/types.h"

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

/* Heap block descriptor: free-list link, size, data pointer, and the block
 * this one was split from. lbl_80455070 holds 0x1000 of them; a NULL data
 * pointer marks a free descriptor. */
typedef struct GsRangeMemNode {
    struct GsRangeMemNode* next;
    u32 size;
    void* data;
    struct GsRangeMemNode* previous;
} GsRangeMemNode;

/* Bump arena the heap grows from (lbl_80455048). */
typedef struct GsRangeArena {
    s32 cursorIndex;
    u8* cursor[8];
    u32 remaining;
} GsRangeArena;

typedef struct GsRangeQueue {
    u32 field_00;
    u32 field_04;
    u32 field_08;
    u32 field_0C;
    u32 field_10;
    u32 field_14;
    u32 field_18;
    u32 field_1C;
} GsRangeQueue;

/* Cache header lbl_80454038 plus the 0x1000 bytes that follow it up to
 * the arena (symbols.txt size 0x1010; no relocation addresses the tail). */
typedef struct GsRangeCacheArea {
    GsRangeCache cache;
    u8 unk10[0x1000];
} GsRangeCacheArea;

typedef struct GsRangeARQEntry {
    u8 request[0x20];
    s32 state;
    s32 mode;
    void* src;
    void* dst;
    u32 size;
    u32 flush;
    void (*callback)(void* arg0, void* arg1);
    void* callbackArg;
    u32 index;
} GsRangeARQEntry;

/* The fsys load slot (FSYSSlot in game/fsys/fsys.h) as the job pool sees it. */
typedef struct GsRangeSlotInfo {
    u8 pad00[0x40];
    struct FSYSArchiveHeader* archiveData;
    u8 pad44[0xB0];
    u32 entryIndex;
    void* taskParam;
} GsRangeSlotInfo;

typedef struct GsRangePoolElem {
    s32 active;
    s32 field_04;
    void (*callback)(void*, void*);
    s32 state;
    void (*task)(void); /* GSgapp task the job runs (fn_8018114C, ...) */
    s32 type;
    void* app;
    struct GsRangePoolElem* nextJob;
    GsRangeSlotInfo* slot;
    u32 index;
    struct FSYSSubEntry* subEntry;
    u8 _pad_2C[0x8];
    void* taskParam;
    void* compressed; /* LZSS source buffer, set by fsysLZSSJobSetup */
    u8 _pad_3C[0x4];
} GsRangePoolElem;

typedef struct GsRangePoolInfo {
    s32 count;
    GsRangePoolElem* base;
} GsRangePoolInfo;

extern GsRangeMemNode* lbl_8047B1D0;      /* free-list rover */
extern GsRangeArena lbl_80455048;
extern GsRangeMemNode lbl_80455070[0x1000];
extern GsRangeMemNode lbl_80465070;       /* free-list sentinel */
extern GsRangeARQEntry* lbl_8047B1D4;     /* ARQ entry array */
extern u32 lbl_8047B1D8;                  /* ARQ entry count */
extern GsRangePoolInfo lbl_8047B1E8;      /* job pool */
extern void* lbl_8047B1E0;                /* job pool file buffer */
extern GsRangePoolElem* lbl_8047B1E4;     /* current job */

extern void DCFlushRange(void* addr, u32 nBytes);
extern u16 GSmemAlloc(u32 size, u32 align);
extern void* GSmemLock(u16 handle);
extern void fsysFlushCachedHandles(void);

u32 fn_8017FA5C(void);
void fn_8017FB08(void* allocation);
void* fn_8017FDB0(u32 size);
void fn_801808E4(GsRangeARQEntry* entry);

/* Expanded three times: twice in fsysCacheInit, once in fn_80180B94. */
static inline void* memAlloc(u32 size)
{
    u16 h = GSmemAlloc(size, 0x20);
    if (h) {
        return GSmemLock(h);
    }
    return NULL;
}

#endif /* GAME_GS_RANGE_8017FA5C_SHARED_H */
