/**
 * @file fsys_file_r52_8017D960_o4s_inline_noauto.c
 * @brief FSYS slot load completion (0x8017D960 - 0x8017DAB8).
 *
 * Marks the slot loaded (status 1000). A slot that was re-requested while
 * loading records the entry it was asked for (load mode 3: the entry whose
 * name hash is slot->requestID; otherwise the last entry) and drops to load
 * mode 2. Then the slot's open disc file is closed, the manager's active
 * slot cleared and the next pending load started (fsysStartPendingLoad), and the
 * slot's completion callback is told the load mode.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): unit-wide "-opt level=0", no
 * local pragmas. The second loop assigns the lookup to `entry` although
 * nothing reads it: retail builds fsysGetEntry's result (r22) in full,
 * which a discarded call expression does not do.
 *
 * Retail copies the first loop's lookup into `entry` (mr r29,r26) and
 * keeps the copy although the compare reads r26, and ranks `entry` above
 * the archive base registers. Without help the final peephole pass deletes
 * the dead copy and `entry` ranks lower (r27): 98.1%.
 *
 * RULE-EXCEPTION(title-path), user directive 2026-09-28: exact with two
 * register-only constructs, both tagged below: `entry = entry;` (ranking)
 * and the dead `i = (u32)entry;` after the first loop, which keeps `entry`
 * live out of the loop so the peephole pass keeps the mr and deletes only
 * the dead copy into `i`. (A dead copy into `callback` also keeps the mr
 * but raises `callback` above the lookup temporaries.) A clean fix needs
 * the real instruction-free second use of `entry`.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

extern FSYSManager lbl_80453FEC;

extern s32 GSdvdClose(u32 fileInfo);
extern void fsysStartPendingLoad(void);

/* Address: 0x8017D960 | size: 0x158 */
void fn_8017D960(FSYSSlot* slot)
{
    u32 i;
    FSYSFileEntry* entry;
    void (*callback)(s32 loadMode, u32 argB, u32 argC);

    slot->status = 1000;
    if (slot->reloadFlag == 1) {
        if (slot->loadMode == 3) {
            for (i = 0; i < slot->numEntries; i++) {
                entry = fsysGetEntry(slot, i);
                /* RULE-EXCEPTION(title-path): self-assignment whose only effect is register ranking (ranks `entry` above the archive base registers) - see docs/RULE_EXCEPTIONS.md */
                entry = entry;
                if (entry->nameHash == slot->requestID) {
                    slot->entryIndex = i;
                    break;
                }
            }
            /* RULE-EXCEPTION(title-path): dead copy that keeps retail's dead `mr r29,r26` (entry live out of the loop; the peephole pass then deletes only this copy) - see docs/RULE_EXCEPTIONS.md */
            i = (u32)entry;
        } else {
            for (i = 0; i < slot->numEntries; i++) {
                entry = fsysGetEntry(slot, i);
                slot->entryIndex = i;
            }
        }
        slot->loadMode = 2;
    }
    if (slot->fileInfo0) {
        GSdvdClose(slot->fileInfo0);
        slot->fileInfo0 = 0;
        lbl_80453FEC.activeSlot = NULL;
        fsysStartPendingLoad();
    }
    if (slot->callbackA) {
        callback = (void (*)(s32, u32, u32))slot->callbackA;
        callback(slot->loadMode, slot->callbackB, slot->callbackC);
    }
}
