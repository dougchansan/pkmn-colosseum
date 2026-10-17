/**
 * .text 0x80179FA4 - 0x8017A5FC: fsysInitTOC. fn_80179F4C (0x80179F4C -
 * 0x80179FA4), the other function between camera.c (which ends at
 * 0x80179F4C with _cameraRestoreStateData) and gs_range_8017A5FC_prefix.c,
 * is exact at level 0 and carved into gs_range_80179F4C_exact_80179F4C.c.
 *
 * They are not camera.c code: both are built without optimization (the
 * parameter goes through the stack), they share no data with camera.c and
 * fsysInitTOC works on 0x80453FEC / 0x8047B1B8, which the fsys units after
 * it also use. The function sets up the archive's ARAM cache and asynchronous
 * DVD read, including the separate external-file path.
 *
 * Exact (2026-09-29) with GC/2.0 like the neighbouring fsys read units
 * (GC/1.3 colours the DMA-setup temporaries differently; 1.3.2 through 2.7
 * all give retail), under title-path exceptions: the statement-form
 * fsysCacheOldestHandle (see fsys_file_candidate_8017DB74_gc20.c), the
 * total-size loop as a first-level helper (stack numbering), and the unused
 * `sub` local that retail loads and homes at 0x9C. Function-level stack
 * locals are numbered in declaration order (archive 0xA4, cacheAddress
 * 0xA0, sub 0x9C).
 */

#include "game/fsys/fsys_entry.h"

extern FSYSManager lbl_80453FEC;
extern FSYSFileHandle* lbl_8047B1B8;
extern s32 lbl_8047B1BC;
extern void* lbl_8047B1C0[2];
extern u32 fn_8017FA5C(void);
extern void fn_8017F800(s32 handleID);
extern u32 fn_8017F928(u32 size, u32 fileHandle, u32 key1, u32 key2);
extern u8 fn_80167EF8(const char* path);
extern u32 GSdvdOpen(const char* path);
extern u32 GSdvdGetLength(u32 fileInfo);
extern u8 GSdvdReadAsync(u32 fileInfo, void* dst, u32 length, u32 offset,
                      void (*callback)(s32 result));
extern void fn_8017A5FC(s32 result);

/* The same cache table walk and LRU removal expand in the later FSYS stages. */
static inline s32 fsysCacheFindHandle(FSYSSlot* slot)
{
    FSYSFileHandle* table;
    s32 i;

    table = lbl_8047B1B8;
    for (i = 0; i < lbl_8047B1BC; i++) {
        if (table->handleID == (s32)slot->field_08) {
            return table->handleID;
        }
        table++;
    }
    return -1;
}

static inline s32 fsysCacheOldestHandle(void)
{
    s32 id;
    FSYSFileHandle* table;

    table = lbl_8047B1B8;
    id = table->handleID;
    /* RULE-EXCEPTION(title-path): constant condition used only to force statement inlining — see docs/RULE_EXCEPTIONS.md */
    if (((void)0, 1)) {
        return id;
    }
    return -1;
}

static inline s32 fsysCacheRemoveHandle(s32 handleID)
{
    FSYSFileHandle* table;
    s32 i;
    s32 found;
    s32 index;

    found = 0;
    table = lbl_8047B1B8;
    for (i = 0; i < lbl_8047B1BC; i++) {
        if (table->handleID == handleID) {
            table->handleID = -1;
            found = 1;
            index = i;
            break;
        }
        table++;
    }
    if (found) {
        table = lbl_8047B1B8;
        for (i = 0; i < lbl_8047B1BC - 1; i++) {
            if (i >= index) {
                table[i] = table[i + 1];
            }
        }
        lbl_8047B1BC--;
        table[lbl_8047B1BC].handleID = -1;
        return 0;
    }
    return -1;
}

static inline void fsysCacheMakeRoom(FSYSSlot* slot)
{
    u32 freeSize;
    s32 n;
    s32 handleID;

    freeSize = fn_8017FA5C();
    if (slot->totalDecompSize > freeSize && fsysCacheFindHandle(slot) < 0) {
        for (n = 0;; n++) {
            handleID = fsysCacheOldestHandle();
            if (handleID < 0) {
                break;
            }
            fn_8017F800(handleID);
            if (fsysCacheRemoveHandle(handleID) < 0) {
                break;
            }
            freeSize = fn_8017FA5C();
            if (slot->totalDecompSize <= freeSize) {
                break;
            }
        }
    }
}

/*
 * External-file archives: the total size is the sum of the entries' sizes
 * plus the archive trailer's word at +8. Retail numbers member/trailer
 * (0x7C/0x80) with the eviction helpers' locals and the fsysGetEntry
 * expansion (0x20..0x30) last, i.e. the loop is a first-level helper.
 * RULE-EXCEPTION(title-path): single-use inline helper evidenced only by stack numbering — see docs/RULE_EXCEPTIONS.md
 */
static inline void fsysSetTotalSize(FSYSSlot* slot)
{
    u32 total;
    u32 i;
    FSYSFileEntry* member;
    u32* trailer;

    total = 0;
    for (i = 0; i < slot->numEntries; i++) {
        member = fsysGetEntry(slot, i);
        total += member->decompressedSize;
    }
    trailer = (u32*)((u8*)slot->archiveData + slot->field_18);
    total += trailer[2];
    slot->totalDecompSize = total;
}

/* Address: 0x80179FA4 | size: 0x658. Archive cache and DVD read setup. */
void fsysInitTOC(FSYSSlot* slot, u32 offset, u32 length, void* callbackA,
                 u32 callbackB, u32 callbackC, const char* externalPath,
                 FSYSFileEntry* entry)
{
    FSYSArchiveHeader* archive;
    u32 cacheAddress;
    FSYSSubEntry* sub;
    void* dvdBuffer;
    u32 cacheSize;
    u32 lowSize;
    u32 fileSize;

    /* RULE-EXCEPTION(title-path): unused local kept for retail's dead load/store — see docs/RULE_EXCEPTIONS.md */
    sub = slot->currentSub;
    cacheSize = entry->decompressedSize >> 17;
    lowSize = entry->decompressedSize & 0x1FFFF;
    lowSize = (lowSize + 0x1F) & ~0x1F;
    cacheSize = lowSize + (cacheSize << 17);

    fsysCacheMakeRoom(slot);
    cacheAddress = fn_8017F928(cacheSize, slot->fileHandle, entry->groupID,
                                 entry->nameHash);
    fsysCacheMakeRoom(slot);

    if (length < 0x20000) {
        slot->dmaChunkSize = (length + 0x1F) & ~0x1F;
    } else {
        slot->dmaChunkSize = 0x20000;
    }
    slot->dmaBytesRemaining = length;
    slot->dmaSrcOffset = 0;
    slot->dmaDstOffset = offset;
    slot->callbackA = (u32)callbackA;
    slot->callbackB = callbackB;
    slot->callbackC = callbackC;
    slot->dmaCopyDst = (void*)cacheAddress;
    slot->dmaAsyncRequest = NULL;
    dvdBuffer = lbl_8047B1C0[lbl_80453FEC.field_24];
    lbl_80453FEC.activeSlot = slot;
    lbl_80453FEC.currentSlot = slot;

    archive = slot->archiveData;
    if (archive->flags & 1) {
        slot->tocBuffer = NULL;
        if (externalPath && fn_80167EF8(externalPath)) {
            slot->tocBuffer = (void*)GSdvdOpen(externalPath);
        }
        if (!slot->tocBuffer) {
            GSdvdReadAsync(slot->fileInfo0, dvdBuffer, slot->dmaChunkSize,
                         offset, fn_8017A5FC);
        } else {
            fileSize = GSdvdGetLength((u32)slot->tocBuffer);
            if (fileSize < 0x20000) {
                slot->dmaChunkSize = (fileSize + 0x1F) & ~0x1F;
            } else {
                slot->dmaChunkSize = 0x20000;
            }
            entry->decompressedSize = fileSize;
            slot->dmaBytesRemaining = fileSize;
            slot->dmaDstOffset = 0;
            fsysSetTotalSize(slot);
            GSdvdReadAsync((u32)slot->tocBuffer, dvdBuffer, slot->dmaChunkSize,
                         0, fn_8017A5FC);
        }
    } else {
        GSdvdReadAsync(slot->fileInfo0, dvdBuffer, slot->dmaChunkSize,
                     offset, fn_8017A5FC);
    }
}
