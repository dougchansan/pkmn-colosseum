/**
 * @file gs_range_8017FA5C_residual_8018094C.c
 * @brief GSgapp job pool update fn_8018094C (0x8018094C - 0x80180B94).
 *
 * Carve of the 0x8017FA5C - 0x80180C78 retail unit, built at
 * `-opt level=0` with `-inline deferred` like the rest of the unit.
 *
 * RULE-EXCEPTION(title-path), user directive 2026-09-28: linked exact with
 * the two register-only constructs described at the end of this comment
 * (the single-use jobStartLoad inline and the `callback` local). A clean
 * fix needs evidence for the default job type's start helper (e.g. an
 * XD sibling) and for what really occupies the second folded register.
 *
 * The default job type sizes "s1_out.fsys" (open / length / close) and
 * allocates a 32-byte-aligned buffer of that size. Retail copies the
 * length into a fresh register before aligning it (mr r20,r22; addi
 * r0,r20,31; clrrwi r21,r0,5). At optimisation level 0 an inline
 * parameter is only materialised as its own variable when the argument is
 * a local of another inline expansion; an argument that is a local of the
 * caller is substituted and gives no copy (compare memAlloc(cacheSize) in
 * fsysCacheInit, which passes r18 straight to r3). So the length is the
 * result of an inline file-size helper whose value feeds memAllocAligned,
 * which fileGetSize reproduces with the same 147 instructions.
 *
 * Open difference: register numbering only. Retail saves r20-r31 and
 * leaves r23/r24 unused (variables whose copies the peephole pass folded
 * after allocation; memFree's pointer parameter accounts for one of them),
 * ranking the length (r22) above the aligned size (r21) and the parameter
 * copy (r20); this body allocates length r23, copy r22, aligned r21 and
 * saves r21-r31.
 *
 * What is known (G2 lane): at -opt level=0 every named value gets its own
 * non-volatile register, ranked by reference weight and then by list order
 * (the function's own locals in declaration order, then each inline
 * expansion: return temporary, locals in reverse declaration order,
 * parameter copies). Retail's "copy below aligned" order is what a start
 * inline for the default job type gives (a jobStartLoad(job) sibling of
 * jobStartDecode / jobStartGroup holding `file` and `length` as its own
 * locals and calling memAllocAligned(length), whose parameter copy then
 * ranks last). That alone is 99.71%: it still lacks the second folded
 * register above the length. Adding any own-scope two-reference pointer
 * whose copy the peephole folds (for example a local holding
 * entry->callback for the indirect call) makes the function 100%, but
 * nothing in the instruction stream distinguishes such a local from the
 * direct call, so it would be a register-only construct and is not used.
 */
#include "game/gs_range_8017FA5C_shared.h"

extern u16 GSmemGetHandleByPtr(void* ptr);
extern void fn_800E24B0(u16 handle);
extern void GSmemFree(u16 handle);
extern void fsysLZSSJobFinish(void*, void*, u32, void*);
extern void fsysLZSSJobSetup(void*, void*, u32, void*);
extern s32 fn_8017AC30(void);
extern void* GSgappCreate(s32, s32, void*, void*);
extern void fn_8018114C(void);
extern void fn_80181224(void);
extern void* GSdvdOpen(const char*);
extern s32 GSdvdGetLength(void*);
extern void GSdvdClose(void*);
extern const char lbl_80273F80[];

static inline void* memAllocAligned(u32 size)
{
    u32 alignedSize = (size + 0x1F) & ~0x1F;
    u16 h = GSmemAlloc(alignedSize, 0x20);
    if (h) {
        return GSmemLock(h);
    }
    return NULL;
}

/*
 * Same free sequence as fsysFree in fsys_slot_8017B1CC.c. Its pointer
 * parameter takes a register that no instruction uses once the peephole
 * pass folds the copy (r24 in retail).
 */
static inline void memFree(void* ptr)
{
    u16 h;

    h = GSmemGetHandleByPtr(ptr);
    if (h) {
        fn_800E24B0(h);
        GSmemFree(h);
    }
}

/*
 * Start of the default job type: size "s1_out.fsys" and allocate its
 * buffer.
 * RULE-EXCEPTION(title-path): single-use static inline helper backed only by register ranking (its locals and memAllocAligned's parameter copy rank as retail) - see docs/RULE_EXCEPTIONS.md
 */
static inline void jobStartLoad(GsRangePoolElem* job)
{
    void* file;
    u32 length;

    job->app = GSgappCreate(2, 0x1E, NULL, fn_80181224);
    if (job->app) {
        job->active = 1;
        job->state = 1;
        file = GSdvdOpen(lbl_80273F80);
        length = GSdvdGetLength(file);
        GSdvdClose(file);
        lbl_8047B1E0 = memAllocAligned(length);
        lbl_8047B1E4 = job;
    }
}

void fn_8018094C(void)
{
    GsRangePoolElem* entry;
    GsRangePoolElem* job;
    s32 i;
    /* RULE-EXCEPTION(title-path): local copy whose only effect is register ranking (the folded copy above the length) - see docs/RULE_EXCEPTIONS.md */
    void (*callback)(void*, void*);

    entry = lbl_8047B1E8.base;
    for (i = 0; i < lbl_8047B1E8.count; i++) {
        if (entry->active == 1) {
            if (entry->callback) {
                callback = entry->callback;
                callback(entry->slot, entry->subEntry);
                return;
            }
            if (entry->app) {
                if (entry->state == 1) {
                    return;
                }
                if (entry->state == 2) {
                    if (entry->type == 0) {
                        fsysLZSSJobFinish(entry->slot, entry->subEntry, entry->index, entry);
                    }
                    entry->state = 0;
                    return;
                }
                if (lbl_8047B1E0) {
                    memFree(lbl_8047B1E0);
                    lbl_8047B1E0 = NULL;
                }
                entry->active = 0;
                entry->app = NULL;
                entry->state = 0;
                if (entry->nextJob) {
                    job = entry->nextJob;
                    switch (job->type) {
                    case 0:
                        lbl_8047B1E4 = job;
                        fsysLZSSJobSetup(lbl_8047B1E4->slot, lbl_8047B1E4->subEntry,
                                    lbl_8047B1E4->index, lbl_8047B1E4);
                        job->app = GSgappCreate(fn_8017AC30(), 0xC8,
                                                job->slot->taskParam, fn_8018114C);
                        if (job->app) {
                            job->active = 1;
                            job->state = 1;
                            lbl_8047B1E4 = job;
                        }
                        break;
                    default:
                        jobStartLoad(job);
                        break;
                    }
                } else {
                    lbl_8047B1E4 = NULL;
                }
                return;
            }
        }
        entry++;
    }
}
