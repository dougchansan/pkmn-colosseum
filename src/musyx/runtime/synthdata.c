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
 * The dataInsert/dataRemove half (0x80150C78 - 0x8015210C) is asm-only.
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

/* renamed symbols referenced by asm incs (symbolmap port) */
extern void ARQPostRequest();
extern void InitStreamBuffers();
extern void aramQueueCallback();
extern void aramUploadData();
extern u32 inpGetMidiCtrl(u32 ctrl, u32 bank, u32 channel);
extern void salCalcVolume(u32 volumeArg, f32* out, u32 pan, u32 surroundPan,
                          f32 a, f32 b, f32 c, u32 narrowPan,
                          u32 studioMode);
extern void salCallback();
extern u8 jumptable_80369CB0[];
extern u8 jumptable_80369CD4[];
extern u8 jumptable_80369CF8[];
extern u8 lbl_80273448[];
extern u8 lbl_8036944C[];
extern u8 lbl_8036BF00[];
extern u8 lbl_80434C50[];
extern f32 lbl_8047D4D8;
extern f32 lbl_8047D4DC;
extern f32 lbl_8047D4E0;
extern f64 lbl_8047D4E8;
extern f32 lbl_8047D4F0;
extern f32 lbl_8047D4F4;
extern f32 lbl_8047D4F8;
extern f32 lbl_8047D4FC;
extern f32 lbl_8047D500;
extern f32 lbl_8047D504;
extern f64 lbl_8047D508;
extern f32 lbl_8047D510;
extern f32 lbl_8047D514;
extern f64 lbl_8047D518;
extern f32 lbl_8047D520;
extern f64 lbl_8047D528;
extern f32 lbl_8047D530;
extern f32 lbl_8047D534;
extern u32 lbl_8047B070;
extern u32 lbl_8047B078;
extern u32 lbl_8047B07C;

/* GSmem allocator */
extern u16   _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u16 handle);

/* External functions referenced from asm wrappers */
extern u32 sndAuxCallbackUpdateSettingsReverbHI(u8* ptr);

/* Model system */
extern void  GSmodelGetPart(void* model, u32 param);
extern void  GSpartFree(void* model, u32 param);
extern void  fn_800E24B0(void* model, u32 param);
extern void  fn_800E209C(void* model, u32 param);
extern void  set__5GSvecFfff(void* dst, void* src);
extern void  GSvecCopy(void* dst, void* src);
extern void  GSvecAdd(void* model, void* param);
extern void  fn_800E0BA0(void* param);
extern void  fn_800E0BE4(void* param);
extern void  fn_800E013C(void* param);
extern u32   __cvt_fp2unsigned(f64 val);

/* Floor/field system */
extern void* GSresGetResource(u16 group, u16 model, u16 param);

/* GX rendering */
extern void  GSmodelSetVisibility(void* param);

/* People data layer (people_data.c) */
extern void* itemDataBiosGetPtr(u16 index);   /* peopleFieldGetByIndex */
extern void* fn_80142CF4(u32 a, u32 b, u32 c, u32 d);  /* peopleFieldAlloc */
extern void  fn_801429E8(void* entry);  /* peopleFieldGetEntry */
extern void  fn_80142984(u32 id);       /* peopleFieldGetByID */

/* Script system */

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
typedef struct { u16 id; u16 refCount; u32 offset; void* addr; u8 header[0x14]; } SdirDataT; /* sample-dir data entry, 0x20 bytes */
typedef struct { u8 pad[9]; u8 vGroup; } FxEntryT;                            /* individual FX_TAB entry, 0xA bytes (only vGroup@9 used here) */
typedef struct { u16 gid; u16 fxNum; void* fxTab; } FxGroupT;                 /* FX group cluster entry, 8 bytes */
typedef struct { u16 num; u16 subTabIndex; } MacMainEntryT;                   /* macro main-table entry, 4 bytes */
typedef struct { void* data; u16 id; u16 refCount; } MacSubEntryT;            /* macro sub-table entry, 8 bytes */

s32 maccmp(u16* a, u16* b);
s32 smpcmp(u16* a, u16* b);
s32 curvecmp(u16* a, u16* b);
s32 layercmp(u16* a, u16* b);
s32 fxcmp(u16* a, u16* b);

#if !defined(MUSYX_SYNTHDATA_CANDIDATE_80150C78_80150E68) && \
    !defined(MUSYX_SYNTHDATA_EXACT_80150E68_80150FE4) && \
    !defined(MUSYX_SYNTHDATA_CANDIDATE_80150FE4_8015122C) && \
    !defined(MUSYX_SYNTHDATA_EXACT_8015122C_80151B84) && \
    !defined(MUSYX_SYNTHDATA_CANDIDATE_80151B84_8015210C) && \
    !defined(MUSYX_SYNTHDATA_EXACT_8015210C_8015211C) && \
    !defined(MUSYX_SYNTHDATA_CANDIDATE_8015211C_801521A8) && \
    !defined(MUSYX_SYNTHDATA_EXACT_801521A8_801521B8) && \
    !defined(MUSYX_SYNTHDATA_CANDIDATE_801521B8_801522E0) && \
    !defined(MUSYX_SYNTHDATA_EXACT_801522E0_801525E4)
#define MUSYX_SYNTHDATA_ALL
#endif

#if defined(MUSYX_SYNTHDATA_ALL) || \
    defined(MUSYX_SYNTHDATA_CANDIDATE_80150C78_80150E68)
s32 dataInsertKeymap(u16 cid, void* keymapdata) {
    extern DataTabT lbl_804378F8[];
    extern u16 lbl_8047AFA6;
#define tab (lbl_804378F8)
    s32 i, j;

    hwDisableIrq();
    for (i = 0; i < lbl_8047AFA6 && tab[i].id < cid; ++i) {}

    if (i < lbl_8047AFA6) {
        if (cid != tab[i].id) {
            if (lbl_8047AFA6 < 0x100) {
                for (j = lbl_8047AFA6 - 1; j >= i; --j) tab[j + 1] = tab[j];
                ++lbl_8047AFA6;
            } else {
                hwEnableIrq();
                return 0;
            }
        } else {
            tab[i].refCount++;
            hwEnableIrq();
            return 0;
        }
    } else if (lbl_8047AFA6 < 0x100) {
        ++lbl_8047AFA6;
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
#endif

#if defined(MUSYX_SYNTHDATA_ALL) || \
    defined(MUSYX_SYNTHDATA_EXACT_80150E68_80150FE4)
s32 dataRemoveKeymap(u16 sid) {
    extern u8 lbl_804378F8[];
    extern u16 lbl_8047AFA6;
#define tab ((DataTabT*)lbl_804378F8)
    s32 i;
    u16 new_var;
    s32 j;

    hwDisableIrq();
    new_var = lbl_8047AFA6;
    for (i = 0; i < new_var && tab[i].id != sid; ++i) {}

    if (i != new_var && --tab[i].refCount == 0) {
        for (j = i + 1; j < new_var; j++) {
            tab[j - 1] = tab[j];
        }
        --lbl_8047AFA6;
        hwEnableIrq();
        return 1;
    }

    hwEnableIrq();
    return 0;
#undef tab
}
#endif

#if defined(MUSYX_SYNTHDATA_ALL) || \
    defined(MUSYX_SYNTHDATA_CANDIDATE_80150FE4_8015122C)
s32 dataInsertLayer(u16 cid, void* layerdata, u16 size) {
    extern LayerTabT lbl_804380F8[];
    extern u16 lbl_8047AFA4;
#define tab (lbl_804380F8)
    s32 i, j;

    hwDisableIrq();
    for (i = 0; i < lbl_8047AFA4 && tab[i].id < cid; ++i) {}

    if (i < lbl_8047AFA4) {
        if (cid != tab[i].id) {
            if (lbl_8047AFA4 < 0x100) {
                for (j = lbl_8047AFA4 - 1; j >= i; --j) tab[j + 1] = tab[j];
                ++lbl_8047AFA4;
            } else {
                hwEnableIrq();
                return 0;
            }
        } else {
            tab[i].refCount++;
            hwEnableIrq();
            return 0;
        }
    } else if (lbl_8047AFA4 < 0x100) {
        ++lbl_8047AFA4;
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
#endif

#if defined(MUSYX_SYNTHDATA_ALL) || \
    defined(MUSYX_SYNTHDATA_EXACT_8015122C_80151B84)
s32 dataRemoveLayer(u16 sid) {
    extern u8 lbl_804380F8[];
    extern u16 lbl_8047AFA4;
#define tab ((LayerTabT*)lbl_804380F8)
    u16 new_var;
    s32 i, j;

    hwDisableIrq();
    new_var = lbl_8047AFA4;
    for (i = 0; i < lbl_8047AFA4 && tab[i].id != sid; ++i) {}

    if (i != lbl_8047AFA4 && --tab[i].refCount == 0) {
        for (j = i + 1; j < new_var; j++) {
            tab[j - 1] = tab[j];
        }
        --lbl_8047AFA4;
        hwEnableIrq();
        return 1;
    }

    hwEnableIrq();
    return 0;
#undef tab
}

s32 dataInsertCurve(u16 cid, void* curvedata) {
    extern DataTabT lbl_80438CF8[];
    extern u16 lbl_8047AFA8;
#define tab (lbl_80438CF8)
    s32 i, j;

    hwDisableIrq();
    for (i = 0; i < lbl_8047AFA8 && tab[i].id < cid; ++i) {}

    if (i < lbl_8047AFA8) {
        if (cid != tab[i].id) {
            if (lbl_8047AFA8 < 0x800) {
                for (j = lbl_8047AFA8 - 1; j >= i; --j) tab[j + 1] = tab[j];
                ++lbl_8047AFA8;
            } else {
                hwEnableIrq();
                return 0;
            }
        } else {
            hwEnableIrq();
            tab[i].refCount++;
            return 0;
        }
    } else if (lbl_8047AFA8 < 0x800) {
        ++lbl_8047AFA8;
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
    extern u16 lbl_8047AFA8;
#define tab ((DataTabT*)lbl_80438CF8)
    s32 i, j;
    u16 new_var;

    hwDisableIrq();
    new_var = lbl_8047AFA8;
    for (i = 0; i < lbl_8047AFA8 && tab[i].id != sid; ++i) {}

    if (i != lbl_8047AFA8 && --tab[i].refCount == 0) {
        for (j = i + 1; j < new_var; j++) {
            tab[j - 1] = tab[j];
        }
        --lbl_8047AFA8;
        hwEnableIrq();
        return 1;
    }

    hwEnableIrq();
    return 0;
#undef tab
}

s32 dataInsertSDir(SdirDataT* sdir, void* smp_data) {
    extern u8 lbl_8043CCF8[];
    extern u16 lbl_8047AFAA;
#define tab ((SdirTabT*)lbl_8043CCF8)
    s32 i;
    SdirDataT* s;
    u16 n;
    u16 j;
    u16 k;
    u16 count;
    u32 offset;

    for (i = 0; i < lbl_8047AFAA && tab[i].data != sdir; ++i) {}

    if (i == lbl_8047AFAA) {
        if (lbl_8047AFAA < 0x80) {
            n = 0;
            for (s = sdir; s->id != 0xFFFF; ++s) {
                ++n;
            }

            hwDisableIrq();
            for (j = 0; j < n; ++j) {
                for (i = 0; i < lbl_8047AFAA; ++i) {
                    for (k = 0; k < tab[i].numSmp; ++k) {
                        if (sdir[j].id == ((SdirDataT*)tab[i].data)[k].id) goto found_id;
                    }
                }
            found_id:
                if (i != lbl_8047AFAA) {
                    sdir[j].refCount = 0xFFFF;
                } else {
                    sdir[j].refCount = 0;
                }
            }

            count = lbl_8047AFAA;
            offset = count * sizeof(SdirTabT);
            lbl_8047AFAA = count + 1;
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
    extern u16 lbl_8047AFAA;
    extern void fn_80163050(void* header, void* addr);
#define tab ((SdirTabT*)lbl_8043CCF8)
    SdirTabT* new_var;
    u32 i;
    SdirDataT* data;
    SdirDataT* sdir;
    void* header;

    sdir = NULL;
    for (i = 0; i < lbl_8047AFAA; ++i) {
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
        fn_80163050(&header, &sdir->addr);
    }
    ++sdir->refCount;
    return 1;
#undef tab
}

s32 dataRemoveSampleReference(u16 sid) {
    extern u8 lbl_8043CCF8[];
    extern u16 lbl_8047AFAA;
    extern void fn_80163104(void* header, void* addr);
    SdirTabT* tab = (SdirTabT*)lbl_8043CCF8;
    u32 i;
    SdirDataT* sdir;

    for (i = 0; i < lbl_8047AFAA; ++i) {
        for (sdir = (SdirDataT*)tab[i].data; sdir->id != 0xFFFF; ++sdir) {
            if (sdir->id == sid && sdir->refCount != 0xFFFF) {
                --sdir->refCount;
                if (sdir->refCount == 0) {
                    fn_80163104(&sdir->header, sdir->addr);
                }
                return 1;
            }
        }
    }
    return 0;
}

s32 dataInsertFX(u16 gid, FxEntryT* fx, u16 fxNum) {
    extern u8 lbl_8043D2F8[];
    extern u16 lbl_8047AFA0;
#define tab ((FxGroupT*)lbl_8043D2F8)
    s32 i;

    for (i = 0; i < lbl_8047AFA0 && gid != tab[i].gid; ++i) {}

    if (i == lbl_8047AFA0) {
        if (lbl_8047AFA0 < 0x80) {
            hwDisableIrq();
            tab[lbl_8047AFA0].gid = gid;
            tab[lbl_8047AFA0].fxNum = fxNum;
            tab[lbl_8047AFA0].fxTab = fx;

            for (i = 0; i < fxNum; ++i, ++fx) {
                fx->vGroup = 31;
            }

            lbl_8047AFA0++;
            hwEnableIrq();
            return 1;
        }
    }
    return 0;
#undef tab
}
#endif

#if defined(MUSYX_SYNTHDATA_ALL) || \
    defined(MUSYX_SYNTHDATA_CANDIDATE_80151B84_8015210C)
s32 dataInsertMacro(u16 mid, void* macroaddr) {
    extern u8 lbl_8043D6F8[];
    extern u8 lbl_8043DEF8[];
    extern u16 lbl_8047AFA2;
#define mainTab ((MacMainEntryT*)lbl_8043D6F8)
#define subTab ((MacSubEntryT*)lbl_8043DEF8)
    s32 main;
    s32 base;
    s32 pos;
    s32 i;

    hwDisableIrq();
    main = (mid >> 6) & 0x3FF;

    if (mainTab[main].num == 0) {
        pos = base = mainTab[main].subTabIndex = lbl_8047AFA2;
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

    if (lbl_8047AFA2 < 0x1000) {
        for (i = 0; i < 512; ++i) {
            if (mainTab[i].subTabIndex > base) mainTab[i].subTabIndex++;
        }

        i = lbl_8047AFA2 - 1;
        for (; i >= pos; --i) subTab[i + 1] = subTab[i];

        subTab[pos].id = mid;
        subTab[pos].data = macroaddr;
        subTab[pos].refCount = 1;
        mainTab[main].num++;
        lbl_8047AFA2++;
        hwEnableIrq();
        return 1;
    }
    hwEnableIrq();
    return 0;
#undef mainTab
#undef subTab
}

s32 dataRemoveMacro(u16 mid) {
    extern u8 lbl_8043D6F8[];
    extern u8 lbl_8043DEF8[];
    extern u16 lbl_8047AFA2;
#define mainTab ((MacMainEntryT*)lbl_8043D6F8)
#define subTab ((MacSubEntryT*)lbl_8043DEF8)
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
                for (i = base + i + 1; i < lbl_8047AFA2; ++i) {
                    subTab[i - 1] = subTab[i];
                }

                for (i = 0; i < 512; ++i) {
                    if (mainTab[i].subTabIndex > base) --mainTab[i].subTabIndex;
                }

                --mainTab[main].num;
                --lbl_8047AFA2;
            }
        }
    }

    hwEnableIrq();
    return 0;
#undef mainTab
#undef subTab
}
#endif

#if defined(MUSYX_SYNTHDATA_ALL) || \
    defined(MUSYX_SYNTHDATA_EXACT_8015210C_8015211C)
s32 maccmp(u16* a, u16* b) {
    return (s32)(a[2]) - (s32)(b[2]);
}
#endif
typedef s32 (*PeopleCmpFn)(u8* a, u8* b);
extern void* sndBSearch(u8* key, u8* base, s32 count, u32 size, PeopleCmpFn cmp);
extern u8 lbl_8043D6F8[];
extern u32 lbl_8047AF98;
extern u8 lbl_8047AF90[8];  /* true .sbss size 0x8 -> @sda21 (was unsized [] => band mis-measured 96.57%) */
extern u8 lbl_8043DEF8[];
extern u32 lbl_8047AF9C;
extern u32 lbl_8047AF8C;
/* Early asm includes predate the symbol-map rename at 0x80162118. */
#define fn_80162118 sndBSearch
#if defined(MUSYX_SYNTHDATA_ALL) || \
    defined(MUSYX_SYNTHDATA_CANDIDATE_8015211C_801521A8)
#if 0
asm void dataGetMacro(void) {
#include "src/game/people/people_field_fn_8015211C.inc"
}
#else
/* The target loads `count` with a separate base and index. CW CSEs the table
 * entry address and emits `add` plus a plain `lhz`; pointer, typed-array, and
 * named-offset forms select the same addressing mode. */
#pragma peephole off
u32 dataGetMacro(u32 key) {
    extern void* sndBSearch(u8* a, u8* b, u16 c, u32 d, void* e);
    void* result;
    u16 count;
    u32 sub;
    u32 main;

    main = (key >> 6) & 0x3FF;
    count = ((MacMainEntryT*)lbl_8043D6F8)[main].num;
    lbl_8047AF98 = main;
    if (count != 0) {
        sub = ((MacMainEntryT*)lbl_8043D6F8)[main].subTabIndex;
        *(u16*)(lbl_8047AF90 + 4) = (u16)key;
        lbl_8047AF9C = sub;
        result = sndBSearch(lbl_8047AF90, lbl_8043DEF8 + sub * 8, count, 8, maccmp);
        lbl_8047AF8C = (u32)result;
        if (result != NULL) { return *(u32*)result; }
    }
    return 0;
}
#pragma peephole reset
#endif
#endif

#if defined(MUSYX_SYNTHDATA_ALL) || \
    defined(MUSYX_SYNTHDATA_EXACT_801521A8_801521B8)
#if 0
asm void smpcmp(void) {
#include "src/game/people/people_field_fn_801521A8.inc"
}
#else
s32 smpcmp(u16* a, u16* b) {
    return (s32)(a[0]) - (s32)(b[0]);
}
#endif
#endif

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
extern u32 lbl_8047AF88;
extern u32 lbl_8047AF84;
extern u16 lbl_8047AFAA;
#if defined(MUSYX_SYNTHDATA_ALL) || \
    defined(MUSYX_SYNTHDATA_CANDIDATE_801521B8_801522E0)
#if 0
asm void dataGetSample(void) {
#include "src/game/people/people_field_fn_801521B8.inc"
}
#else
/* Resolve a sample directory entry and copy its header fields into the
 * caller's SAMPLE_INFO-compatible output record. */
u32 dataGetSample(u16 key, u32* out) {
    void* result;
    u8* header;
    u8* table;
    u32 i;

    *(u16*)lbl_80445EF8 = key;
    for (i = 0; i < lbl_8047AFAA; i++) {
        table = lbl_8043CCF8 + i * 0xC;
        result = sndBSearch(lbl_80445EF8, *(u8**)table, *(u16*)(table + 8), 0x20, (PeopleCmpFn)smpcmp);
        lbl_8047AF88 = (u32)result;
        if (result != NULL && *(u16*)((u8*)result + 2) != 0xFFFF) {
            header = (u8*)result + 0xC;
            lbl_8047AF84 = (u32)header;
            out[0] = *(u32*)header;
            out[1] = *(u32*)((u8*)result + 8);
            out[3] = 0;
            out[5] = *(u32*)(header + 8);
            out[4] = *(u32*)(header + 4) & 0x00FFFFFF;
            out[6] = *(u32*)(header + 0xC);
            *(u8*)((u8*)out + 0x1C) = (u8)(*(u32*)(header + 4) >> 24);
            if (*(u32*)(header + 0x10) != 0) {
                out[2] = *(u32*)table + *(u32*)(header + 0x10);
            }
            return 0;
        }
    }
    return (u32)-1;
}
#endif
#endif

#if defined(MUSYX_SYNTHDATA_ALL) || \
    defined(MUSYX_SYNTHDATA_EXACT_801522E0_801525E4)
#if 0
asm void curvecmp(void) {
#include "src/game/people/people_field_curvecmp.inc"
}
#else
s32 curvecmp(u16* a, u16* b) {
    return (s32)(a[2]) - (s32)(b[2]);
}
#endif
extern u8 lbl_80438CF8[];
extern u8 lbl_8047AF7C[8];
extern u16 lbl_8047AFA8;
extern u32 lbl_8047AF78;
#if 0
asm void dataGetCurve(void) {
#include "src/game/people/people_field_dataGetCurve.inc"
}
#else
u32 dataGetCurve(u16 arg) {
    extern void* sndBSearch(u8* a, u8* b, u16 c, u32 d, void* e);
    void* result;
    *(u16*)(lbl_8047AF7C + 4) = arg;
    result = sndBSearch(lbl_8047AF7C, lbl_80438CF8, lbl_8047AFA8, 8, curvecmp);
    lbl_8047AF78 = (u32)result;
    if (result != NULL) { return *(u32*)result; }
    return 0;
}
#endif
extern u8 lbl_804378F8[];
extern u8 lbl_8047AF70[8];
extern u16 lbl_8047AFA6;
extern u32 lbl_8047AF6C;
#if 0
asm void dataGetKeymap(void) {
#include "src/game/people/people_field_dataGetKeymap.inc"
}
#else
u32 dataGetKeymap(u16 arg) {
    extern void* sndBSearch(u8* a, u8* b, u16 c, u32 d, void* e);
    void* result;
    *(u16*)(lbl_8047AF70 + 4) = arg;
    result = sndBSearch(lbl_8047AF70, lbl_804378F8, lbl_8047AFA6, 8, curvecmp);
    lbl_8047AF6C = (u32)result;
    if (result != NULL) { return *(u32*)result; }
    return 0;
}
#endif
#if 0
asm void layercmp(void) {
#include "src/game/people/people_field_layercmp.inc"
}
#else
s32 layercmp(u16* a, u16* b) {
    return (s32)(a[2]) - (s32)(b[2]);
}
#endif
extern u8 lbl_80445F18[];
extern u8 lbl_804380F8[];
extern u16 lbl_8047AFA4;
extern u32 lbl_8047AF68;
#if 0
asm void dataGetLayer(void) {
#include "src/game/people/people_field_fn_801523B8.inc"
}
#else
u32 dataGetLayer(u16 arg, u16* out) {
    extern void* sndBSearch(u8* a, u8* b, u16 c, u32 d, void* e);
    void* result;
    *(u16*)(lbl_80445F18 + 4) = arg;
    result = sndBSearch(lbl_80445F18, lbl_804380F8, lbl_8047AFA4, 0xc, layercmp);
    lbl_8047AF68 = (u32)result;
    if (result != NULL) {
        *out = *(u16*)((u8*)result + 6);
        return *(u32*)(u32)lbl_8047AF68;
    }
    return 0;
}
#endif

extern u16 lbl_8047AFA0;
extern u16 lbl_8047AFA2;
extern u8 lbl_8043D2F8[];
extern u8 lbl_80445F24[];
#if 0
asm void fxcmp(void) {
#include "src/game/people/people_field_fn_80152434.inc"
}
#else
s32 fxcmp(u16* a, u16* b) {
    return (s32)(a[0]) - (s32)(b[0]);
}
#endif
#if 0
asm void dataGetFX(void) {
#include "src/game/people/people_field_fn_80152444.inc"
}
#else
u32 dataGetFX(u16 key) {
    extern void* sndBSearch(u8* a, u8* b, u16 c, u32 d, void* e);
    void* result;
    u8* table;
    s32 i;

    *(u16*)lbl_80445F24 = key;
    for (i = 0; i < lbl_8047AFA0; i++) {
        table = lbl_8043D2F8 + i * 8;
        result = sndBSearch(lbl_80445F24, *(u8**)(table + 4), *(u16*)(table + 2), 0xA, fxcmp);
        if (result != NULL) { return (u32)result; }
    }
    return 0;
}
#endif
#if 0
asm void dataInit(void) {
#include "src/game/people/people_field_dataInit.inc"
}
#else
typedef struct { u16 num; u16 subTabIndex; } DataMacMainEntry;

void dataInit(u32 smpBase, u32 smpLength) {
    extern void fn_8016300C(u32 a, u32 b);
    s32 i;

    lbl_8047AFAA = 0;
    lbl_8047AFA8 = 0;
    lbl_8047AFA6 = 0;
    lbl_8047AFA4 = 0;
    lbl_8047AFA0 = 0;
    lbl_8047AFA2 = 0;
    for (i = 0; i < 0x200; i++) {
        ((DataMacMainEntry*)lbl_8043D6F8)[i].num = 0;
        ((DataMacMainEntry*)lbl_8043D6F8)[i].subTabIndex = 0;
    }
    fn_8016300C(smpBase, smpLength);
}
#endif

#undef fn_80162118

void dataExit(void) {
    extern void fn_80163030(void);
    fn_80163030();
}
#endif
