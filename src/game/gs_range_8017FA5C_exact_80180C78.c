/**
 * @file gs_range_8017FA5C_exact_80180C78.c
 * @brief GSgapp job pool: request entry and tasks (0x80180C78 - 0x801812C4).
 *
 * fn_80180C78 queues a job for an archive entry (fsys calls it from
 * fsysAllocLZSSJob for "LZSS" images, mode 0): take a free job record (pool
 * exhausted -> fn_80179F4C(1)), fill it in, and either append it to the
 * running job's chain or start it at once. Mode 0 jobs decode the entry
 * (fsysLZSSJobSetup sets them up, task fn_8018114C); mode 1 jobs run the
 * entry's group callback (task fn_80181094).
 *
 * The tasks: fn_80181094 runs an archive entry's group callback through
 * fsysFlushLoadedEntry, fn_8018114C decodes an entry's LZSS image through
 * fsysLZSSDecodeJob, and fn_80181224 reads "s1_out.fsys" (lbl_80273F80) into the
 * pool's file buffer 30 times. Each marks the current job (lbl_8047B1E4)
 * done and ends its task.
 *
 * TU evidence: this code belongs to the 0x8017FA5C retail unit rather than
 * to people.c (whose code starts at 0x801812C4): it owns no data of its
 * own and works on that unit's job pool (.sbss lbl_8047B1E0/E4/E8, set up
 * by fn_80180B94), and fn_8018094C in that unit starts the same tasks
 * (jobStartDecode below is expanded there too). It is built like the rest
 * of the unit at `-opt level=0` with `-inline deferred` and no local
 * pragmas; every function here is exact with those flags. Deferred
 * inlining emits a unit's functions in reverse source order, so they are
 * written from the highest address down.
 */
#include "game/gs_range_8017FA5C_shared.h"
#include "game/fsys/fsys.h"

extern const char lbl_80273F80[]; /* "s1_out.fsys" */

extern void GSgappTerminate(void* app);
extern u32 GSdvdOpen(const char* path);   /* DVDOpen wrapper */
extern u32 GSdvdGetLength(u32 file);           /* file length */
extern s32 GSdvdClose(u32 file);           /* DVDClose wrapper */
extern void GSdvdRead(u32 file, void* dst, u32 size, u32 offset);
extern s32 fsysFlushLoadedEntry(GsRangeSlotInfo* slot, FSYSFileEntry* entry);
extern void* fsysLZSSDecodeJob(GsRangeSlotInfo* slot, u32 nameHash,
                         void* compressed, FSYSSubEntry* sub);
extern void fn_80179F4C(s32 error);
extern void fsysLZSSJobSetup(GsRangeSlotInfo* slot, FSYSSubEntry* sub, u32 index,
                        GsRangePoolElem* job);
extern s32 fn_8017AC30(void);
extern void* GSgappCreate(s32 a, s32 b, void* param, void (*task)(void));

void fn_8018114C(void);
void fn_80181094(void);

/* Address: 0x80181224 | size: 0xA0 */
void fn_80181224(void)
{
    u32 file;
    s32 i;
    u32 size;
    void* buffer;

    buffer = lbl_8047B1E0;
    file = GSdvdOpen(lbl_80273F80);
    size = GSdvdGetLength(file);
    for (i = 0; i < 30; i++) {
        GSdvdRead(file, buffer, size, 0);
    }
    GSdvdClose(file);
    lbl_8047B1E4->state = 0;
    GSgappTerminate(lbl_8047B1E4->app);
}

/* Address: 0x8018114C | size: 0xD8 */
void fn_8018114C(void)
{
    FSYSFileEntry* entry;
    u32* offsets;
    u32* tables;
    FSYSArchiveHeader* archive;

    archive = lbl_8047B1E4->slot->archiveData;
    tables = (u32*)((u8*)lbl_8047B1E4->slot->archiveData + archive->tableOffset);
    offsets = (u32*)((u8*)lbl_8047B1E4->slot->archiveData + tables[0]);
    entry = (FSYSFileEntry*)((u8*)lbl_8047B1E4->slot->archiveData +
                             offsets[lbl_8047B1E4->index]);
    fsysLZSSDecodeJob(lbl_8047B1E4->slot, entry->nameHash, lbl_8047B1E4->compressed,
                lbl_8047B1E4->subEntry);
    lbl_8047B1E4->subEntry->state = 0;
    lbl_8047B1E4->state = 2;
    GSgappTerminate(lbl_8047B1E4->app);
}

/* Address: 0x80181094 | size: 0xB8 */
void fn_80181094(void)
{
    u32* offsets;
    u32* tables;
    FSYSArchiveHeader* archive;
    FSYSFileEntry* entry;

    archive = lbl_8047B1E4->slot->archiveData;
    tables = (u32*)((u8*)lbl_8047B1E4->slot->archiveData + archive->tableOffset);
    offsets = (u32*)((u8*)lbl_8047B1E4->slot->archiveData + tables[0]);
    entry = (FSYSFileEntry*)((u8*)lbl_8047B1E4->slot->archiveData +
                             offsets[lbl_8047B1E4->index]);
    fsysFlushLoadedEntry(lbl_8047B1E4->slot, entry);
    lbl_8047B1E4->state = 2;
    GSgappTerminate(lbl_8047B1E4->app);
}

/*
 * Job pool helpers. fn_80180C78 expands each of jobFindFree, jobCountActive
 * and jobFindTail once per request mode (identical sequences at 0x80180CB4
 * and 0x80180EA4, 0x80180CFC and 0x80180EFC, 0x80180DB0 and 0x80180FB0),
 * and fn_8018094C expands jobStartDecode as well. At level 0 their return
 * values travel through their own temporaries, which is what puts the
 * results in the stack slots retail uses.
 */
static inline GsRangePoolElem* jobFindFree(void)
{
    GsRangePoolElem* job;
    s32 i;

    job = lbl_8047B1E8.base;
    for (i = 0; i < lbl_8047B1E8.count; i++) {
        if (job->active == 0) {
            return job;
        }
        job++;
    }
    fn_80179F4C(1);
    return NULL;
}

static inline s32 jobCountActive(void)
{
    GsRangePoolElem* job;
    s32 i;
    s32 count;

    job = lbl_8047B1E8.base;
    count = 0;
    for (i = 0; i < lbl_8047B1E8.count; i++) {
        if (job->active == 1) {
            count++;
        }
        job++;
    }
    return count;
}

static inline GsRangePoolElem* jobFindTail(void)
{
    GsRangePoolElem* job;
    s32 i;

    job = lbl_8047B1E4;
    for (i = 0; i < lbl_8047B1E8.count; i++) {
        if (job->nextJob) {
            job = job->nextJob;
        } else {
            return job;
        }
    }
    return NULL;
}

static inline void jobStartDecode(GsRangePoolElem* job)
{
    lbl_8047B1E4 = job;
    fsysLZSSJobSetup(lbl_8047B1E4->slot, lbl_8047B1E4->subEntry, lbl_8047B1E4->index,
                lbl_8047B1E4);
    job->app = GSgappCreate(fn_8017AC30(), 0xC8, job->slot->taskParam, fn_8018114C);
    if (job->app) {
        job->active = 1;
        job->state = 1;
        lbl_8047B1E4 = job;
    }
}

static inline void jobStartGroup(GsRangePoolElem* job)
{
    job->app = GSgappCreate(1, 2, job->slot->taskParam, fn_80181094);
    if (job->app) {
        job->active = 1;
        job->state = 1;
        lbl_8047B1E4 = job;
    }
}

/*
 * The two request paths differ only in the task and in how a job that
 * does not have to wait is started. Retail counts the active jobs into a
 * local that nothing reads (stored at 0x4c(r1) / 0x44(r1) and never
 * loaded); level-0 code keeps that store.
 */
static inline s32 jobRequestDecode(GsRangeSlotInfo* slot, FSYSSubEntry* sub, s32 mode)
{
    GsRangePoolElem* job;
    s32 active;
    GsRangePoolElem* tail;

    job = jobFindFree();
    active = jobCountActive();
    if (job) {
        job->taskParam = slot->taskParam;
        job->type = mode;
        job->task = fn_8018114C;
        job->slot = slot;
        job->subEntry = sub;
        job->nextJob = NULL;
        job->index = slot->entryIndex;
        if (lbl_8047B1E4) {
            tail = jobFindTail();
            tail->nextJob = job;
            job->active = 2;
            job->state = 0;
            return 1;
        }
        jobStartDecode(job);
        return 1;
    }
    return 0;
}

static inline s32 jobRequestGroup(GsRangeSlotInfo* slot, FSYSSubEntry* sub, s32 mode)
{
    GsRangePoolElem* job;
    s32 active;
    GsRangePoolElem* tail;

    job = jobFindFree();
    active = jobCountActive();
    if (job) {
        job->taskParam = slot->taskParam;
        job->type = mode;
        job->task = fn_80181094;
        job->slot = slot;
        job->subEntry = sub;
        job->nextJob = NULL;
        job->index = slot->entryIndex;
        if (lbl_8047B1E4) {
            tail = jobFindTail();
            tail->nextJob = job;
            job->active = 2;
            job->state = 0;
            return 1;
        }
        jobStartGroup(job);
        return 1;
    }
    return 0;
}

/* Address: 0x80180C78 | size: 0x41C */
s32 fn_80180C78(GsRangeSlotInfo* slot, FSYSSubEntry* sub, s32 mode)
{
    s32 result;

    result = 0;
    switch (mode) {
    case 0:
        result = jobRequestDecode(slot, sub, mode);
        break;
    case 1:
        result = jobRequestGroup(slot, sub, mode);
        break;
    }
    return result;
}
