/**
 * @file s_data.c
 * @brief MusyX sound-group stack, 0x80159C48 - 0x8015A484.
 *
 * Follows the reference MusyX runtime's s_data.c (AxioDL/musyx) in its
 * 2.0.0-and-earlier form: a single static group stack (gs[128], sp), no
 * ARAM stack instances. Only the functions retail keeps are here; the
 * rest of the reference file (sndPopGroup, the stack-instance API) is
 * dead-stripped from the game. The stack itself stays extern under its
 * symbol-map names.
 *
 * The pool lookups and ID-list scanners are the reference's static
 * helpers; retail inlines ScanIDList five times into sndPushGroup
 * (samples, macros, curves, keymaps, layers) and the pool lookups into
 * InsertData. sndSeqPlayEx expands seqPlaySong with irq_call == 0.
 */
#include "dolphin/types.h"
#include "musyx/synthdata.h"

typedef struct GROUP_DATA {
    u32 nextOff;
    u16 id;
    u16 type;
    u32 macroOff;
    u32 sampleOff;
    u32 curveOff;
    u32 keymapOff;
    u32 layerOff;
    union {
        struct {
            u32 normpageOff;
            u32 drumpageOff;
            u32 midiSetupOff;
        } song;
        struct {
            u32 tableOff;
        } fx;
    } data;
} GROUP_DATA;

typedef struct GSTACK {
    GROUP_DATA* gAddr;
    void* sdirAddr;
    void* prjAddr;
} GSTACK;

typedef struct FX_DATA {
    u16 num;
    u16 reserved;
    u8 fx[1];
} FX_DATA;

typedef struct MIDISETUP {
    u16 songId;
    u16 reserved;
    u8 channel[16][5];
} MIDISETUP;

extern u8 lbl_8047AF18;                     /* sndActive */
extern GSTACK lbl_80447860[128];            /* gs */
extern s16 lbl_8047AFE8;                    /* sp */

extern void* hwTransAddr(void* samples);    /* hwTransAddr */
extern void hwSyncSampleMem(void);              /* hwSyncSampleMem */
extern void hwSetSaveSampleCallback(void* (*callback)(u32, u32), u32 chunckSize); /* hwSetSaveSampleCallback */
extern u32 dataInsertSDir(void* sdir, void* samples);
extern void dataInsertFX(u16 gid, void* fx, u16 num);
extern void hwDisableIrq(void);
extern void hwEnableIrq(void);
extern u32 seqStartPlay(void* norm, void* drum, MIDISETUP* midiSetup, void* arrfile, void* para,
                       u8 studio, u16 sgid); /* seqStartPlay */

void dataInitStack(void) /* dataInitStack */
{
    lbl_8047AFE8 = 0;
}

static MusyxPoolEntry* GetPoolAddr(u16 id, MusyxPoolEntry* m)
{
    while (m->nextOffset != 0xFFFFFFFF) {
        if (m->id == id) {
            return m;
        }

        m = (MusyxPoolEntry*)((u8*)m + m->nextOffset);
    }
    return NULL;
}

static MusyxPoolEntry* GetMacroAddr(u16 id, MusyxPoolData* pool)
{
    return pool == NULL ? NULL : GetPoolAddr(id, (MusyxPoolEntry*)((u8*)pool + pool->macroOffset));
}

static MusyxPoolEntry* GetCurveAddr(u16 id, MusyxPoolData* pool)
{
    return pool == NULL ? NULL : GetPoolAddr(id, (MusyxPoolEntry*)((u8*)pool + pool->curveOffset));
}

static MusyxPoolEntry* GetKeymapAddr(u16 id, MusyxPoolData* pool)
{
    return pool == NULL ? NULL : GetPoolAddr(id, (MusyxPoolEntry*)((u8*)pool + pool->keymapOffset));
}

static MusyxPoolEntry* GetLayerAddr(u16 id, MusyxPoolData* pool)
{
    return pool == NULL ? NULL : GetPoolAddr(id, (MusyxPoolEntry*)((u8*)pool + pool->layerOffset));
}

void InsertData(u16 id, void* data, u8 dataType, u32 remove) /* InsertData */
{
    MusyxPoolEntry* m;

    switch (dataType) {
    case 0:
        if (!remove) {
            if ((m = GetMacroAddr(id, data)) != NULL) {
                dataInsertMacro(id, m->data.macros);
            } else {
                dataInsertMacro(id, NULL);
            }
        } else {
            dataRemoveMacro(id);
        }
        break;
    case 2: {
        id |= 0x4000;
        if (!remove) {
            if ((m = GetKeymapAddr(id, data)) != NULL) {
                dataInsertKeymap(id, m->data.keymaps);
            } else {
                dataInsertKeymap(id, NULL);
            }
        } else {
            dataRemoveKeymap(id);
        }
    } break;
    case 3: {
        id |= 0x8000;
        if (!remove) {
            if ((m = GetLayerAddr(id, data)) != NULL) {
                dataInsertLayer(id, m->data.layer.entries, m->data.layer.count);
            } else {
                dataInsertLayer(id, NULL, 0);
            }
        } else {
            dataRemoveLayer(id);
        }
    } break;
    case 4:
        if (!remove) {
            if ((m = GetCurveAddr(id, data)) != NULL) {
                dataInsertCurve(id, m->data.curve);
            } else {
                dataInsertCurve(id, NULL);
            }
        } else {
            dataRemoveCurve(id);
        }
        break;
    case 1:
        if (!remove) {
            dataAddSampleReference(id);
        } else {
            dataRemoveSampleReference(id);
        }
        break;
    }
}

static void ScanIDList(u16* ref, void* data, u8 dataType, u32 remove)
{
    u16 id;

    while (*ref != 0xFFFF) {
        if ((*ref & 0x8000)) {
            id = *ref & 0x3FFF;
            while (id <= ref[1]) {
                InsertData(id, data, dataType, remove);
                ++id;
            }
            ref += 2;
        } else {
            InsertData(*ref++, data, dataType, remove);
        }
    }
}

static void InsertMacros(u16* ref, void* pool)
{
    ScanIDList(ref, pool, 0, 0);
}

static void InsertCurves(u16* ref, void* pool)
{
    ScanIDList(ref, pool, 4, 0);
}

static void InsertKeymaps(u16* ref, void* pool)
{
    ScanIDList(ref, pool, 2, 0);
}

static void InsertLayers(u16* ref, void* pool)
{
    ScanIDList(ref, pool, 3, 0);
}

static void InsertSamples(u16* ref, void* samples, void* sdir)
{
    samples = hwTransAddr(samples);
    if (dataInsertSDir(sdir, samples)) {
        ScanIDList(ref, sdir, 1, 0);
    }
}

static void InsertFXTab(u16 gid, FX_DATA* fd)
{
    dataInsertFX(gid, fd->fx, fd->num);
}

void sndSetSampleDataUploadCallback(void* (*callback)(u32, u32), u32 chunckSize) /* sndSetSampleDataUploadCallback */
{
    hwSetSaveSampleCallback(callback, chunckSize);
}

u32 sndPushGroup(void* prj_data, u16 gid, void* samples, void* sdir, void* pool) /* sndPushGroup */
{
    GROUP_DATA* g;

    if (lbl_8047AF18 && lbl_8047AFE8 < 128) {
        g = prj_data;

        while (g->nextOff != 0xFFFFFFFF) {
            if (g->id == gid) {
                lbl_80447860[lbl_8047AFE8].gAddr = g;
                lbl_80447860[lbl_8047AFE8].prjAddr = prj_data;
                lbl_80447860[lbl_8047AFE8].sdirAddr = sdir;
                InsertSamples((u16*)((u8*)prj_data + g->sampleOff), samples, sdir);
                InsertMacros((u16*)((u8*)prj_data + g->macroOff), pool);
                InsertCurves((u16*)((u8*)prj_data + g->curveOff), pool);
                InsertKeymaps((u16*)((u8*)prj_data + g->keymapOff), pool);
                InsertLayers((u16*)((u8*)prj_data + g->layerOff), pool);
                if (g->type == 1) {
                    InsertFXTab(gid, (FX_DATA*)((u8*)prj_data + g->data.song.normpageOff));
                }
                hwSyncSampleMem();
                ++lbl_8047AFE8;
                return TRUE;
            }

            g = (GROUP_DATA*)((u8*)prj_data + g->nextOff);
        }
    }

    return FALSE;
}

u32 seqPlaySong(u16 sgid, u16 sid, void* arrfile, void* para, u8 irq_call, u8 studio) /* seqPlaySong */
{
    int i;
    GROUP_DATA* g;
    void* norm;
    void* drum;
    MIDISETUP* midiSetup;
    u32 seqId;
    void* prj;

    for (i = 0; i < lbl_8047AFE8; ++i) {
        if (lbl_80447860[i].gAddr->id != sgid) {
            continue;
        }

        if (lbl_80447860[i].gAddr->type == 0) {
            g = lbl_80447860[i].gAddr;
            prj = lbl_80447860[i].prjAddr;
            norm = (void*)((u32)prj + g->data.song.normpageOff);
            drum = (void*)((u32)prj + g->data.song.drumpageOff);
            midiSetup = (MIDISETUP*)((u32)prj + g->data.song.midiSetupOff);
            while (midiSetup->songId != 0xFFFF) {
                if (midiSetup->songId == sid) {
                    if (irq_call != 0) {
                        seqId = seqStartPlay(norm, drum, midiSetup, arrfile, para, studio, sgid);
                    } else {
                        hwDisableIrq();
                        seqId = seqStartPlay(norm, drum, midiSetup, arrfile, para, studio, sgid);
                        hwEnableIrq();
                    }
                    return seqId;
                }

                ++midiSetup;
            }

            return 0xFFFFFFFF;
        } else {
            return 0xFFFFFFFF;
        }
    }

    return 0xFFFFFFFF;
}

u32 sndSeqPlayEx(u16 sgid, u16 sid, void* arrfile, void* para, u8 studio) /* sndSeqPlayEx */
{
    return seqPlaySong(sgid, sid, arrfile, para, 0, studio);
}
