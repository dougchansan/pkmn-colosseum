/**
 * @file fsys_file_r51_8017C5B8_prefix.c
 * @brief FSYS slot status handler fsysSlotStatusStartEntries (0x8017C5B8 - 0x8017C6E0).
 *
 * Puts the entry being loaded into the ARAM cache: when fn_8017F794 finds
 * no cached image of it, fn_8017F928 reserves one of the entry's size and
 * fn_80180694 copies the entry's buffer there (completion callback
 * fsysARQCallback, status 151); a failed lookup-and-reserve leaves status 152.
 * For load mode 3 the slot's handle is refreshed first (fsysSlotRefreshHandle) and the
 * handle table is scanned for it; the scan's result is not used.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code; exact (74/74 instructions and relocations) with the
 * unit-wide `-opt level=0` and no local pragmas.
 *
 * - The entry lookup is fsysGetEntry (fsys_entry.h): the index argument is
 *   materialised in r18, the return value built in r23 and copied to
 *   entry's r30, and r20 is the helper's folded header view, as in its
 *   other expansions.
 * - The body is the inline helper fsysCacheEntry. Its inline fingerprint:
 *   retail copies the ARAM address from its home r29 into r19 and only then
 *   stores it to the caller's stack local (mr r19,r29; stw r19,0x8(r1)),
 *   the helper's return value routed through a temporary to its home.
 *   Controlled compiles of the in-place body (GC/1.3, these flags) store
 *   r29 straight to the stack (73 instructions) for every tested form:
 *   `saved = cached;` with the local declared first or last, with a cast,
 *   and with a zero initialiser.
 * - `result` is the caller's stack local: retail stores the value to 0x8
 *   and never reads it.
 * - The helper's locals are declared size, cached, sub: level 0 ranks an
 *   inline's equal-weight locals in reverse declaration order, which gives
 *   retail's sub r25 above size r24.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

extern FSYSFileHandle* lbl_8047B1B8; /* handle table */
extern s32 lbl_8047B1BC;             /* handle table fill count */

extern u32 fn_8017F794(u32 fileHandle, u32 groupID, u32 nameHash);
extern u32 fn_8017F928(u32 size, u32 fileHandle, u32 groupID, u32 nameHash);
extern FSYSFileHandle* fsysSlotRefreshHandle(FSYSSlot* slot);
extern void fn_80180694(void* src, u32 aram, u32 size,
                        void (*callback)(s32 result, void* userData), FSYSSlot* slot);
extern void fsysARQCallback(s32 result, void* userData);

/* ARAM address of the slot's current entry, caching the entry if needed. */
static inline u32 fsysCacheEntry(FSYSSlot* slot)
{
    FSYSFileEntry* entry;
    u32 size;
    u32 cached;
    FSYSSubEntry* sub;
    FSYSFileHandle* handle;
    s32 i;

    entry = fsysGetEntry(slot, slot->entryIndex);
    sub = slot->currentSub;
    cached = fn_8017F794(slot->fileHandle, entry->groupID, entry->nameHash);
    if (cached == 0) {
        size = entry->decompressedSize;
        cached = fn_8017F928(size, slot->fileHandle, entry->groupID, entry->nameHash);
        if (cached == 0) {
            slot->status = 0x98;
        } else {
            if (slot->loadMode == 3) {
                fsysSlotRefreshHandle(slot);
                handle = lbl_8047B1B8;
                for (i = 0; i < lbl_8047B1BC; i++) {
                    if (handle->handleID == (s32)slot->field_08) {
                        break;
                    }
                    handle++;
                }
            }
            slot->status = 0x97;
            fn_80180694(sub->buffer, cached, size, fsysARQCallback, slot);
        }
    } else {
        slot->status = 0x98;
    }
    return cached;
}

/* Address: 0x8017C5B8 | size: 0x128 */
s32 fsysSlotStatusStartEntries(FSYSSlot* slot)
{
    u32 result;

    result = fsysCacheEntry(slot);
    return 1;
}
