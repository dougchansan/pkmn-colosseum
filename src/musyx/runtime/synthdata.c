/**
 * @file synthdata.c
 * @brief MusyX runtime data tables (musyx/runtime/synthdata.c), 0x80150C78 - 0x801525E4.
 *
 * Split out of the misnamed people_field.c unit (2026-07-02). Reference:
 * AxioDL/musyx `musyx/runtime/synthdata.c`. Boundary evidence: simindex
 * identifies dataInsertKeymap (0x80150C78) through dataInit (0x801524E0)
 * as synthdata.c at seq=1.0 vs the matched MP4/Prime/Strikers copies
 * (including the maccmp/curvecmp/layercmp/fxcmp comparator cluster);
 * dataExit (0x20) is dataExit (reference synthdata.c's final one-call
 * wrapper), ending at mcmdWait (0x801525E4), synthmacros.c's first fn.
 *
 * Built as one translation unit that owns its small data (.sbss
 * 0x8047AF68 - 0x8047AFB0): the reference's u16 table counters and the
 * lookup functions' static key/result slots, which MWCC lays out exactly
 * as retail (reverse declaration order). The tables themselves stay
 * extern (.bss). The per-function carves that preceded this could not
 * link dataGetMacro: its statics sit at 0x8047AF8C, which only the whole
 * TU's 8-aligned .sbss can place.
 */

#include "dolphin/types.h"
#include "game/people/people.h"
#include "musyx/synthdata.h"

/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const char* fmt, ...);
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void  DCFlushRange(void* ptr, u32 size);
extern u32   OSDisableInterrupts(void);
extern void  OSRestoreInterrupts(u32 level);


/* ===== dataInsert*/ /* dataRemove* cluster (0x80150C78 - 0x8015210C) =====
 * Struct shapes below were reverse-engineered from
 * build/GC6E01/asm/musyx/runtime/synthdata.s (field offsets/sizes confirmed
 * by the stw/sth/lhz/stwx patterns of each function) since the reference
 * AxioDL synthdata.c declares these as opaque named structs we don't have
 * headers for. Field ORDER below reflects real memory layout, not
 * necessarily the reference source's declaration order. Locally-scoped
 * typedefs (distinct names from dataInit's own `DataMacMainEntry`) so as
 * not to require touching the already-matched dataInit block below. */
extern void hwDisableIrq(void);
extern void hwEnableIrq(void);

typedef struct { void* data; u16 id; u16 refCount; } DataTabT;               /* keymap/curve entry, 8 bytes */
typedef struct { void* data; u16 id; u16 num; u16 refCount; } LayerTabT;     /* layer entry, 12 bytes */
typedef struct { void* data; void* base; u16 numSmp; } SdirTabT;             /* sample-dir directory entry, 12 bytes */
typedef struct {
    u32 info;
    u32 length;
    u32 loopOffset;
    u32 loopLength;
    u32 extraData;
} SdirHeaderT;
typedef struct {
    u16 id;
    u16 refCount;
    u32 offset;
    void* addr;
    SdirHeaderT header;
} SdirDataT; /* sample-dir data entry, 0x20 bytes */
typedef struct {
    u32 info;
    void* addr;
    void* extraData;
    u32 offset;
    u32 length;
    u32 loop;
    u32 loopLength;
    u8 compType;
} SampleInfoT;
typedef struct { u8 pad[9]; u8 vGroup; } FxEntryT;                            /* individual FX_TAB entry, 0xA bytes (only vGroup@9 used here) */
typedef struct { u16 gid; u16 fxNum; void* fxTab; } FxGroupT;                 /* FX group cluster entry, 8 bytes */
typedef struct { u16 num; u16 subTabIndex; } MacMainEntryT;                   /* macro main-table entry, 4 bytes */
typedef struct { void* data; u16 id; u16 refCount; } MacSubEntryT;            /* macro sub-table entry, 8 bytes */

static u16 dataSmpSDirNum;
static u16 dataCurveNum;
static u16 dataKeymapNum;
static u16 dataLayerNum;
static u16 dataMacTotal;
static u16 dataFXGroupNum;

s32 maccmp(u16* a, u16* b);
s32 smpcmp(u16* a, u16* b);
s32 curvecmp(u16* a, u16* b);
s32 layercmp(u16* a, u16* b);
s32 fxcmp(u16* a, u16* b);


s32 dataInsertKeymap(u16 cid, void* keymapdata) {
    extern DataTabT lbl_804378F8[];
#define tab (lbl_804378F8)
    s32 i, j;

    hwDisableIrq();
    for (i = 0; i < dataKeymapNum && tab[i].id < cid; ++i) {}

    if (i < dataKeymapNum) {
        if (cid != tab[i].id) {
            if (dataKeymapNum < 0x100) {
                for (j = dataKeymapNum - 1; j >= i; --j) tab[j + 1] = tab[j];
                ++dataKeymapNum;
            } else {
                hwEnableIrq();
                return 0;
            }
        } else {
            tab[i].refCount++;
            hwEnableIrq();
            return 0;
        }
    } else if (dataKeymapNum < 0x100) {
        ++dataKeymapNum;
    } else {
        hwEnableIrq();
        return 0;
    }

    tab[i].id = cid;
    tab[i].data = keymapdata;
    tab[i].refCount = 1;
    hwEnableIrq();
    return 1;
#undef tab
}

s32 dataRemoveKeymap(u16 sid) {
    extern u8 lbl_804378F8[];
#define tab ((DataTabT*)lbl_804378F8)
    s32 i;
    u16 new_var;
    s32 j;

    hwDisableIrq();
    new_var = dataKeymapNum;
    for (i = 0; i < new_var && tab[i].id != sid; ++i) {}

    if (i != new_var && --tab[i].refCount == 0) {
        for (j = i + 1; j < new_var; j++) {
            tab[j - 1] = tab[j];
        }
        --dataKeymapNum;
        hwEnableIrq();
        return 1;
    }

    hwEnableIrq();
    return 0;
#undef tab
}

s32 dataInsertLayer(u16 cid, void* layerdata, u16 size) {
    extern LayerTabT lbl_804380F8[];
#define tab (lbl_804380F8)
    s32 i, j;

    hwDisableIrq();
    for (i = 0; i < dataLayerNum && tab[i].id < cid; ++i) {}

    if (i < dataLayerNum) {
        if (cid != tab[i].id) {
            if (dataLayerNum < 0x100) {
                for (j = dataLayerNum - 1; j >= i; --j) tab[j + 1] = tab[j];
                ++dataLayerNum;
            } else {
                hwEnableIrq();
                return 0;
            }
        } else {
            tab[i].refCount++;
            hwEnableIrq();
            return 0;
        }
    } else if (dataLayerNum < 0x100) {
        ++dataLayerNum;
    } else {
        hwEnableIrq();
        return 0;
    }

    tab[i].id = cid;
    tab[i].data = layerdata;
    tab[i].num = size;
    tab[i].refCount = 1;
    hwEnableIrq();
    return 1;
#undef tab
}

s32 dataRemoveLayer(u16 sid) {
    extern u8 lbl_804380F8[];
#define tab ((LayerTabT*)lbl_804380F8)
    u16 new_var;
    s32 i, j;

    hwDisableIrq();
    new_var = dataLayerNum;
    for (i = 0; i < dataLayerNum && tab[i].id != sid; ++i) {}

    if (i != dataLayerNum && --tab[i].refCount == 0) {
        for (j = i + 1; j < new_var; j++) {
            tab[j - 1] = tab[j];
        }
        --dataLayerNum;
        hwEnableIrq();
        return 1;
    }

    hwEnableIrq();
    return 0;
#undef tab
}

s32 dataInsertCurve(u16 cid, void* curvedata) {
    extern DataTabT lbl_80438CF8[];
#define tab (lbl_80438CF8)
    s32 i, j;

    hwDisableIrq();
    for (i = 0; i < dataCurveNum && tab[i].id < cid; ++i) {}

    if (i < dataCurveNum) {
        if (cid != tab[i].id) {
            if (dataCurveNum < 0x800) {
                for (j = dataCurveNum - 1; j >= i; --j) tab[j + 1] = tab[j];
                ++dataCurveNum;
            } else {
                hwEnableIrq();
                return 0;
            }
        } else {
            hwEnableIrq();
            tab[i].refCount++;
            return 0;
        }
    } else if (dataCurveNum < 0x800) {
        ++dataCurveNum;
    } else {
        hwEnableIrq();
        return 0;
    }

    tab[i].id = cid;
    tab[i].data = curvedata;
    tab[i].refCount = 1;
    hwEnableIrq();
    return 1;
#undef tab
}

s32 dataRemoveCurve(u16 sid) {
    extern u8 lbl_80438CF8[];
#define tab ((DataTabT*)lbl_80438CF8)
    s32 i, j;
    u16 new_var;

    hwDisableIrq();
    new_var = dataCurveNum;
    for (i = 0; i < dataCurveNum && tab[i].id != sid; ++i) {}

    if (i != dataCurveNum && --tab[i].refCount == 0) {
        for (j = i + 1; j < new_var; j++) {
            tab[j - 1] = tab[j];
        }
        --dataCurveNum;
        hwEnableIrq();
        return 1;
    }

    hwEnableIrq();
    return 0;
#undef tab
}

s32 dataInsertSDir(SdirDataT* sdir, void* smp_data) {
    extern u8 lbl_8043CCF8[];
#define tab ((SdirTabT*)lbl_8043CCF8)
    s32 i;
    SdirDataT* s;
    u16 n;
    u16 j;
    u16 k;
    u16 count;
    u32 offset;

    for (i = 0; i < dataSmpSDirNum && tab[i].data != sdir; ++i) {}

    if (i == dataSmpSDirNum) {
        if (dataSmpSDirNum < 0x80) {
            n = 0;
            for (s = sdir; s->id != 0xFFFF; ++s) {
                ++n;
            }

            hwDisableIrq();
            for (j = 0; j < n; ++j) {
                for (i = 0; i < dataSmpSDirNum; ++i) {
                    for (k = 0; k < tab[i].numSmp; ++k) {
                        if (sdir[j].id == ((SdirDataT*)tab[i].data)[k].id) goto found_id;
                    }
                }
            found_id:
                if (i != dataSmpSDirNum) {
                    sdir[j].refCount = 0xFFFF;
                } else {
                    sdir[j].refCount = 0;
                }
            }

            count = dataSmpSDirNum;
            offset = count * sizeof(SdirTabT);
            dataSmpSDirNum = count + 1;
            tab[count].data = sdir;
            ((SdirTabT*)((u8*)tab + offset))->numSmp = n;
            ((SdirTabT*)((u8*)tab + offset))->base = smp_data;
            hwEnableIrq();
            return 1;
        } else {
            return 0;
        }
    }

    return 1;
#undef tab
}

s32 dataAddSampleReference(u16 sid) {
    extern u8 lbl_8043CCF8[];
    extern void hwSaveSample(void* header, void* addr);
#define tab ((SdirTabT*)lbl_8043CCF8)
    SdirTabT* new_var;
    u32 i;
    SdirDataT* data;
    SdirDataT* sdir;
    void* header;

    sdir = NULL;
    for (i = 0; i < dataSmpSDirNum; ++i) {
        for (data = (SdirDataT*)tab[i].data; data->id != 0xFFFF; ++data) {
            if (data->id == sid && data->refCount != 0xFFFF) {
                sdir = data;
                goto done;
            }
        }
    }
done:
    if (sdir->refCount == 0) {
        new_var = tab;
        sdir->addr = (void*)((u32)new_var[i].base + sdir->offset);
        header = &sdir->header;
        hwSaveSample(&header, &sdir->addr);
    }
    ++sdir->refCount;
    return 1;
#undef tab
}

s32 dataRemoveSampleReference(u16 sid) {
    extern u8 lbl_8043CCF8[];
    extern void hwRemoveSample(void* header, void* addr);
    SdirTabT* tab = (SdirTabT*)lbl_8043CCF8;
    u32 i;
    SdirDataT* sdir;

    for (i = 0; i < dataSmpSDirNum; ++i) {
        for (sdir = (SdirDataT*)tab[i].data; sdir->id != 0xFFFF; ++sdir) {
            if (sdir->id == sid && sdir->refCount != 0xFFFF) {
                --sdir->refCount;
                if (sdir->refCount == 0) {
                    hwRemoveSample(&sdir->header, sdir->addr);
                }
                return 1;
            }
        }
    }
    return 0;
}

s32 dataInsertFX(u16 gid, FxEntryT* fx, u16 fxNum) {
    extern u8 lbl_8043D2F8[];
#define tab ((FxGroupT*)lbl_8043D2F8)
    s32 i;

    for (i = 0; i < dataFXGroupNum && gid != tab[i].gid; ++i) {}

    if (i == dataFXGroupNum) {
        if (dataFXGroupNum < 0x80) {
            hwDisableIrq();
            tab[dataFXGroupNum].gid = gid;
            tab[dataFXGroupNum].fxNum = fxNum;
            tab[dataFXGroupNum].fxTab = fx;

            for (i = 0; i < fxNum; ++i, ++fx) {
                fx->vGroup = 31;
            }

            dataFXGroupNum++;
            hwEnableIrq();
            return 1;
        }
    }
    return 0;
#undef tab
}

s32 dataInsertMacro(u16 mid, void* macroaddr) {
    extern MacMainEntryT lbl_8043D6F8[];
    extern MacSubEntryT lbl_8043DEF8[];
#define mainTab (lbl_8043D6F8)
#define subTab (lbl_8043DEF8)
    s32 main;
    s32 base;
    s32 pos;
    s32 i;

    hwDisableIrq();
    main = (mid >> 6) & 0x3FF;

    if (mainTab[main].num == 0) {
        pos = base = mainTab[main].subTabIndex = dataMacTotal;
    } else {
        base = mainTab[main].subTabIndex;
        for (i = 0; i < mainTab[main].num && subTab[base + i].id < mid; ++i) {}

        if (i < mainTab[main].num) {
            pos = base + i;
            if (mid == subTab[pos].id) {
                subTab[pos].refCount++;
                hwEnableIrq();
                return 0;
            }
        } else {
            pos = base + i;
        }
    }

    if (dataMacTotal < 0x1000) {
        for (i = 0; i < 512; ++i) {
            if (mainTab[i].subTabIndex > base) mainTab[i].subTabIndex++;
        }

        i = dataMacTotal - 1;
        for (; i >= pos; --i) subTab[i + 1] = subTab[i];

        subTab[pos].id = mid;
        subTab[pos].data = macroaddr;
        subTab[pos].refCount = 1;
        mainTab[main].num++;
        dataMacTotal++;
        hwEnableIrq();
        return 1;
    }
    hwEnableIrq();
    return 0;
#undef mainTab
#undef subTab
}

s32 dataRemoveMacro(u16 mid) {
    extern MacMainEntryT lbl_8043D6F8[];
    extern MacSubEntryT lbl_8043DEF8[];
#define mainTab (lbl_8043D6F8)
#define subTab (lbl_8043DEF8)
    s32 main;
    s32 base;
    s32 i;

    hwDisableIrq();
    main = (mid >> 6) & 0x3FF;

    if (mainTab[main].num != 0) {
        base = mainTab[main].subTabIndex;
        for (i = 0; i < mainTab[main].num && mid != subTab[base + i].id; ++i) {}

        if (i < mainTab[main].num) {
            if (--subTab[base + i].refCount == 0) {
                for (i = base + i + 1; i < dataMacTotal; ++i) {
                    subTab[i - 1] = subTab[i];
                }

                for (i = 0; i < 512; ++i) {
                    if (mainTab[i].subTabIndex > base) --mainTab[i].subTabIndex;
                }

                --mainTab[main].num;
                --dataMacTotal;
            }
        }
    }

    hwEnableIrq();
    return 0;
#undef mainTab
#undef subTab
}

s32 maccmp(u16* a, u16* b) {
    return (s32)(a[2]) - (s32)(b[2]);
}
typedef s32 (*PeopleCmpFn)(u8* a, u8* b);
extern void* sndBSearch(u8* key, u8* base, s32 count, u32 size, PeopleCmpFn cmp);
extern MacMainEntryT lbl_8043D6F8[512];   /* dataMacMainTab */
extern MacSubEntryT lbl_8043DEF8[2048];   /* dataMacSubTabmem */
/* Early asm includes predate the symbol-map rename at 0x80162118. */
#define fn_80162118 sndBSearch
void* dataGetMacro(u16 mid) {
    static s32 base;
    static s32 main;
    static MacSubEntryT key;
    static MacSubEntryT* result;

    main = (mid >> 6) & 0x3FFF;

    if (lbl_8043D6F8[main].num != 0) {
        base = lbl_8043D6F8[main].subTabIndex;
        key.id = mid;
        if ((result = (MacSubEntryT*)sndBSearch((u8*)&key, (u8*)&lbl_8043DEF8[base],
                                                lbl_8043D6F8[main].num, 8,
                                                (PeopleCmpFn)maccmp)) != NULL) {
            return result->data;
        }
    }

    return NULL;
}

s32 smpcmp(u16* a, u16* b) {
    return (s32)(a[0]) - (s32)(b[0]);
}

extern void _savegpr_20(void);
extern void _restgpr_20(void);
extern void _savegpr_23(void);
extern void _restgpr_23(void);
extern void _savegpr_24(void);
extern void _restgpr_24(void);
extern void _savegpr_25(void);
extern void _restgpr_25(void);
extern void _savegpr_27(void);
extern void _restgpr_27(void);
extern u8 lbl_80445EF8[];
extern u8 lbl_8043CCF8[];
/* Resolve a sample directory entry and copy its header fields into the
 * caller's SAMPLE_INFO-compatible output record. */
u32 dataGetSample(u16 sid, u32* out) {
#define key (*(SdirDataT*)lbl_80445EF8)
#define directories ((SdirTabT*)lbl_8043CCF8)
    static SdirDataT* result;
    static SdirHeaderT* sheader;
    SampleInfoT* newsmp = (SampleInfoT*)out;
    s32 i;

    key.id = sid;
    for (i = 0; i < dataSmpSDirNum; i++) {
        result = sndBSearch((u8*)&key, (u8*)directories[i].data,
                            directories[i].numSmp, sizeof(SdirDataT),
                            (PeopleCmpFn)smpcmp);
        if (result != NULL && result->refCount != 0xFFFF) {
            sheader = &result->header;
            newsmp->info = sheader->info;
            newsmp->addr = result->addr;
            newsmp->offset = 0;
            newsmp->loop = sheader->loopOffset;
            newsmp->length = sheader->length & 0xFFFFFF;
            newsmp->loopLength = sheader->loopLength;
            newsmp->compType = sheader->length >> 24;
            if (result->header.extraData != 0) {
                newsmp->extraData = (void*)(result->header.extraData +
                                             (u32)directories[i].data);
            }
            return 0;
        }
    }
#undef key
#undef directories
    return (u32)-1;
}

s32 curvecmp(u16* a, u16* b) {
    return (s32)(a[2]) - (s32)(b[2]);
}
extern u8 lbl_80438CF8[];
void* dataGetCurve(u16 cid) {
    static DataTabT key;
    static DataTabT* result;

    key.id = cid;
    if ((result = (DataTabT*)sndBSearch((u8*)&key, lbl_80438CF8, dataCurveNum, 8,
                                        (PeopleCmpFn)curvecmp)) != NULL) {
        return result->data;
    }
    return NULL;
}
extern u8 lbl_804378F8[];
void* dataGetKeymap(u16 cid) {
    static DataTabT key;
    static DataTabT* result;

    key.id = cid;
    if ((result = (DataTabT*)sndBSearch((u8*)&key, lbl_804378F8, dataKeymapNum, 8,
                                        (PeopleCmpFn)curvecmp)) != NULL) {
        return result->data;
    }
    return NULL;
}
s32 layercmp(u16* a, u16* b) {
    return (s32)(a[2]) - (s32)(b[2]);
}
extern u8 lbl_80445F18[];
extern u8 lbl_804380F8[];
void* dataGetLayer(u16 cid, u16* n) {
    static LayerTabT* result;

    ((LayerTabT*)lbl_80445F18)->id = cid;
    if ((result = (LayerTabT*)sndBSearch(lbl_80445F18, lbl_804380F8, dataLayerNum, 0xC,
                                         (PeopleCmpFn)layercmp)) != NULL) {
        *n = result->num;
        return result->data;
    }
    return NULL;
}
extern u8 lbl_8043D2F8[];
extern u8 lbl_80445F24[];
s32 fxcmp(u16* a, u16* b) {
    return (s32)(a[0]) - (s32)(b[0]);
}
u32 dataGetFX(u16 key) {
    extern void* sndBSearch(u8* a, u8* b, u16 c, u32 d, void* e);
    void* result;
    u8* table;
    s32 i;

    *(u16*)lbl_80445F24 = key;
    for (i = 0; i < dataFXGroupNum; i++) {
        table = lbl_8043D2F8 + i * 8;
        result = sndBSearch(lbl_80445F24, *(u8**)(table + 4), *(u16*)(table + 2), 0xA, fxcmp);
        if (result != NULL) { return (u32)result; }
    }
    return 0;
}
typedef struct { u16 num; u16 subTabIndex; } DataMacMainEntry;

void dataInit(u32 smpBase, u32 smpLength) {
    extern void hwInitSampleMem(u32 a, u32 b);
    s32 i;

    dataSmpSDirNum = 0;
    dataCurveNum = 0;
    dataKeymapNum = 0;
    dataLayerNum = 0;
    dataFXGroupNum = 0;
    dataMacTotal = 0;
    for (i = 0; i < 0x200; i++) {
        ((DataMacMainEntry*)lbl_8043D6F8)[i].num = 0;
        ((DataMacMainEntry*)lbl_8043D6F8)[i].subTabIndex = 0;
    }
    hwInitSampleMem(smpBase, smpLength);
}

#undef fn_80162118

void dataExit(void) {
    extern void hwExitSampleMem(void);
    hwExitSampleMem();
}
