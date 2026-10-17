/**
 * @file synthvoice.c
 * @brief MusyX voice IDs, priorities and allocation, 0x80157280 - 0x80158BB4.
 *
 * Follows the reference MusyX runtime's synthvoice.c (AxioDL/musyx) in the
 * 1.5.4 - 2.0.0 form (u16 allocId, voiceAllocate in one piece, vidMakeNew
 * without the 2.0.2 retry). The game never calls synthKillAllVoices or
 * synthKillVoicesByMacroReferences, so they are dead-stripped and omitted.
 *
 * The TU owns its statics: .bss 0x80445F50 - 0x80446F10 (vidList, the
 * priority sort lists, voiceList, synth_last_fxstarted/started) and .sbss
 * 0x8047AFD0 - 0x8047AFE8. Retail addresses the voice lists as offsets
 * from one .bss base (e.g. voiceFree 0x80157EEC: base + 0x800/0x900/0xA00),
 * which is how MWCC addresses file-local arrays, so they are static here.
 */
#include "dolphin/types.h"
#include "musyx/runtime/synth_voice.h"

typedef struct VID_LIST {
    struct VID_LIST* next;
    struct VID_LIST* prev;
    u32 vid;
    u32 root;
} VID_LIST;

typedef struct SYNTH_VOICELIST {
    u8 prev;
    u8 next;
    u16 user;
} SYNTH_VOICELIST;

typedef struct SYNTH_ROOTLIST {
    u16 next;
    u16 prev;
} SYNTH_ROOTLIST;

typedef struct SND_PLAYBACKINFO {
    u32 frq;
    u8 stereo;
    u8 bits;
    s8 deviceName[256];
    s8 versionText[256];
} SND_PLAYBACKINFO;

typedef struct SynthInfo {
    u32 mixFrq;
    u32 numSamples;
    SND_PLAYBACKINFO pbInfo;
    u8 voiceNum;
    u8 maxMusic;
    u8 maxSFX;
    u8 studioNum;
} SynthInfo;

extern SynthInfo lbl_80434C50;               /* synthInfo */
extern SYNTH_VOICE* lbl_8047AF48;            /* synthVoice */
extern u8 lbl_8047AF18;                      /* sndActive */
extern u8 lbl_8047AF50;                      /* synthIdleWaitActive */

extern void hwSetPriority(u32 voice, u32 prio);  /* hwSetPriority */
extern u32 hwIsActive(u32 voice);            /* hwIsActive */
extern void hwBreak(u32 voice);
extern void macMakeInactive(SYNTH_VOICE* svoice, s32 newState);
extern void streamKill(u32 voice);          /* streamKill */

void voiceResetLastStarted(SYNTH_VOICE* svoice);

static VID_LIST vidList[128];
static u8 synth_last_started[8][16];
static u8 synth_last_fxstarted[64];
static SYNTH_VOICELIST voiceList[64];
static SYNTH_ROOTLIST voicePrioSortRootList[256];
static u8 voicePrioSortVoicesRoot[256];
static SYNTH_VOICELIST voicePrioSortVoices[64];
static VID_LIST* vidFree = NULL;
static VID_LIST* vidRoot = NULL;
static u32 vidCurrentId = 0;
static u16 voicePrioSortRootListRoot = 0;
static u8 voiceMusicRunning = 0;
static u8 voiceFxRunning = 0;
static u8 voiceListInsert = 0;
static u8 voiceListRoot = 0;

void vidInit(void)
{
    int i;
    VID_LIST* lvl;

    vidCurrentId = 0;
    vidRoot = NULL;
    vidFree = vidList;
    for (lvl = NULL, i = 0; i < 128; lvl = &vidList[i], ++i) {
        vidList[i].prev = lvl;
        if (lvl != NULL) {
            lvl->next = &vidList[i];
        }
    }
    lvl->next = NULL;
}

static VID_LIST* get_vidlist(u32 vid)
{
    VID_LIST* vl = vidRoot;

    while (vl != NULL) {
        if (vl->vid == vid) {
            return vl;
        }
        if (vl->vid > vid) {
            break;
        }
        vl = vl->next;
    }

    return NULL;
}

static u32 get_newvid(void)
{
    u32 vid;

    do {
        vid = vidCurrentId++;
    } while (vid == 0xFFFFFFFF);

    return vid;
}

static void vidRemove(VID_LIST** vidList)
{
    if ((*vidList)->prev != NULL) {
        (*vidList)->prev->next = (*vidList)->next;
    } else {
        vidRoot = (*vidList)->next;
    }

    if ((*vidList)->next != NULL) {
        (*vidList)->next->prev = (*vidList)->prev;
    }

    (*vidList)->next = vidFree;

    if (vidFree != NULL) {
        vidFree->prev = *vidList;
    }

    (*vidList)->prev = NULL;
    vidFree = *vidList;
    *vidList = NULL;
}

void vidRemoveVoiceReferences(SYNTH_VOICE* svoice) /* vidRemoveVoiceReferences */
{
    if (svoice->id == 0xFFFFFFFF) {
        return;
    }

    voiceResetLastStarted(svoice);
    if (svoice->parent != 0xFFFFFFFF) {
        lbl_8047AF48[svoice->parent & 0xFF].child = svoice->child;
        if (svoice->child != 0xFFFFFFFF) {
            lbl_8047AF48[svoice->child & 0xFF].parent = svoice->parent;
        }

        vidRemove((VID_LIST**)&svoice->vidList);
    } else if (svoice->child != 0xFFFFFFFF) {
        ((VID_LIST*)svoice->vidList)->root = svoice->child;
        lbl_8047AF48[svoice->child & 0xFF].parent = 0xFFFFFFFF;
        lbl_8047AF48[svoice->child & 0xFF].vidMasterList = svoice->vidMasterList;
        if (svoice->vidList != svoice->vidMasterList) {
            vidRemove((VID_LIST**)&svoice->vidList);
        }

        svoice->vidMasterList = svoice->vidList = NULL;
    } else if (svoice->vidList != svoice->vidMasterList) {
        vidRemove((VID_LIST**)&svoice->vidList);
        vidRemove((VID_LIST**)&svoice->vidMasterList);
    } else {
        vidRemove((VID_LIST**)&svoice->vidList);
        svoice->vidMasterList = NULL;
    }
}

u32 vidMakeRoot(SYNTH_VOICE* svoice) /* vidMakeRoot */
{
    svoice->vidMasterList = svoice->vidList;
    return ((VID_LIST*)svoice->vidList)->vid;
}

u32 vidMakeNew(SYNTH_VOICE* svoice, u32 isMaster) /* vidMakeNew */
{
    u32 vid;
    VID_LIST* nvl;
    VID_LIST* lvl;
    VID_LIST* vl;

    vid = get_newvid();
    lvl = NULL;
    nvl = vidRoot;

    while (nvl != NULL) {
        if (nvl->vid > vid) {
            break;
        }

        if (nvl->vid == vid) {
            vid = get_newvid();
        }
        lvl = nvl;
        nvl = nvl->next;
    }

    if ((vl = vidFree) == NULL) {
        return 0xFFFFFFFF;
    }

    if ((vidFree = vidFree->next) != NULL) {
        vidFree->prev = NULL;
    }

    if (lvl == NULL) {
        vidRoot = vl;
    } else {
        lvl->next = vl;
    }

    vl->prev = lvl;
    vl->next = nvl;

    if (nvl != NULL) {
        nvl->prev = vl;
    }

    vl->vid = vid;
    vl->root = svoice->id;
    svoice->vidMasterList = isMaster ? vl : NULL;
    svoice->vidList = vl;

    return isMaster ? vid : svoice->id;
}

u32 vidGetInternalId(u32 vid)
{
    VID_LIST* vl;

    if (vid != 0xFFFFFFFF) {
        if ((vl = get_vidlist(vid)) != NULL) {
            return vl->root;
        }
    }

    return 0xFFFFFFFF;
}

static void voiceInitPrioSort(void)
{
    u32 i;

    for (i = 0; i < lbl_80434C50.voiceNum; ++i) {
        voicePrioSortVoices[i].user = 0;
    }

    for (i = 0; i < 256; ++i) {
        voicePrioSortVoicesRoot[i] = 0xFF;
    }

    voicePrioSortRootListRoot = 0xFFFF;
}

static void voiceRemovePriority(SYNTH_VOICE* svoice)
{
    SYNTH_VOICELIST* vps;
    SYNTH_ROOTLIST* rps;

    vps = &voicePrioSortVoices[svoice->id & 0xFF];
    if (vps->user != 1) {
        return;
    }

    if (vps->prev != 0xFF) {
        voicePrioSortVoices[vps->prev].next = vps->next;
    } else {
        voicePrioSortVoicesRoot[svoice->prio] = vps->next;
    }

    if (vps->next != 0xFF) {
        voicePrioSortVoices[vps->next].prev = vps->prev;
    } else if (vps->prev == 0xFF) {
        rps = &voicePrioSortRootList[svoice->prio];

        if (rps->prev != 0xFFFF) {
            voicePrioSortRootList[rps->prev].next = rps->next;
        } else {
            voicePrioSortRootListRoot = rps->next;
        }

        if (rps->next != 0xFFFF) {
            voicePrioSortRootList[rps->next].prev = rps->prev;
        }
    }

    vps->user = 0;
}

void voiceSetPriority(SYNTH_VOICE* svoice, u8 prio)
{
    u16 li;
    SYNTH_VOICELIST* vps;
    u16 i;
    u32 v;

    v = (u8)svoice->id;
    vps = &voicePrioSortVoices[v];
    if (vps->user == 1) {
        if (svoice->prio == prio) {
            return;
        }

        voiceRemovePriority(svoice);
    }

    vps->user = 1;
    vps->prev = 0xFF;
    if ((vps->next = voicePrioSortVoicesRoot[prio]) != 0xFF) {
        voicePrioSortVoices[voicePrioSortVoicesRoot[prio]].prev = v;
    } else if (voicePrioSortRootListRoot != 0xFFFF) {
        if (prio >= voicePrioSortRootListRoot) {
            for (i = voicePrioSortRootListRoot; i != 0xFFFF; i = voicePrioSortRootList[i].next) {
                if ((u16)i > prio) {
                    break;
                }
                li = i;
            }

            voicePrioSortRootList[li].next = (u16)prio;
            voicePrioSortRootList[prio].prev = li;
            voicePrioSortRootList[prio].next = i;
            if (i != 0xFFFF) {
                voicePrioSortRootList[i].prev = prio;
            }
        } else {
            voicePrioSortRootList[prio].next = voicePrioSortRootListRoot;
            voicePrioSortRootList[prio].prev = 0xFFFF;
            voicePrioSortRootList[voicePrioSortRootListRoot].prev = prio;
            voicePrioSortRootListRoot = prio;
        }
    } else {
        voicePrioSortRootList[prio].next = 0xFFFF;
        voicePrioSortRootList[prio].prev = 0xFFFF;
        voicePrioSortRootListRoot = prio;
    }

    voicePrioSortVoicesRoot[prio] = v;
    svoice->prio = prio;
    hwSetPriority(svoice->id & 0xFF, ((u32)prio << 24) | (svoice->age >> 15));
}

u32 voiceAllocate(u8 priority, u8 maxVoices, u16 allocId, u8 fxFlag) /* voiceAllocate */
{
    s32 i;
    s32 num;
    s32 voice;
    u16 p;
    u32 type_alloc;
    SYNTH_VOICELIST* sfv;

    if (!lbl_8047AF50) {
        if (fxFlag) {
            type_alloc = (voiceFxRunning >= lbl_80434C50.maxSFX && lbl_80434C50.voiceNum > lbl_80434C50.maxSFX);

            if (lbl_80434C50.maxSFX <= maxVoices) {
                goto _skip_alloc;
            }

            goto _do_alloc;
        } else {
            type_alloc =
                (voiceMusicRunning >= lbl_80434C50.maxMusic && lbl_80434C50.voiceNum > lbl_80434C50.maxMusic);

            if (lbl_80434C50.maxMusic <= maxVoices) {
                goto _skip_alloc;
            }

        _do_alloc:
            num = 0;
            voice = -1;

            p = voicePrioSortRootListRoot;
            while (p != 0xFFFF && priority >= p && voice == -1) {
                for (i = voicePrioSortVoicesRoot[p]; i != 0xFF; i = voicePrioSortVoices[i].next) {
                    if (allocId != lbl_8047AF48[i].allocId)
                        continue;
                    ++num;
                    if (lbl_8047AF48[i].block)
                        continue;

                    if (!type_alloc || fxFlag == lbl_8047AF48[i].fxFlag) {
                        if ((lbl_8047AF48[i].cFlags & 2))
                            continue;
                        if (voice != -1) {
                            if (lbl_8047AF48[i].age < lbl_8047AF48[voice].age)
                                voice = i;
                        } else
                            voice = i;
                    }
                }

                p = voicePrioSortRootList[p].next;
            }
        }

        if (num < maxVoices) {
            while (p != 0xFFFF && num < maxVoices) {
                i = voicePrioSortVoicesRoot[p];
                while (i != 0xFF) {
                    if (allocId == lbl_8047AF48[i].allocId) {
                        num++;
                    }

                    i = voicePrioSortVoices[i].next;
                }

                p = voicePrioSortRootList[p].next;
            }

            if (num < maxVoices) {
            _skip_alloc:
                if (voiceListRoot != 0xFF && type_alloc == 0) {
                    voice = voiceListRoot;
                    goto _update;
                }

                if (priority < voicePrioSortRootListRoot) {
                    return -1;
                }
                voice = -1;
                p = voicePrioSortRootListRoot;

                while (p != 0xFFFF && priority >= p && voice == -1) {
                    for (i = voicePrioSortVoicesRoot[p]; i != 0xFF; i = voicePrioSortVoices[i].next) {
                        if (lbl_8047AF48[i].block != 0)
                            continue;

                        if (!type_alloc || fxFlag == lbl_8047AF48[i].fxFlag) {
                            if ((lbl_8047AF48[i].cFlags & 2))
                                continue;
                            if (voice != -1) {
                                if (lbl_8047AF48[voice].age > lbl_8047AF48[i].age)
                                    voice = i;
                            } else
                                voice = i;
                        }
                    }
                    p = voicePrioSortRootList[p].next;
                }

                if (voice == -1) {
                    return 0xFFFFFFFF;
                }

                if (lbl_8047AF48[voice].prio > priority) {
                    goto _fail;
                }
            }
        }

    _update:
        if (voice == -1) {
            goto _fail;
        }

        if (voiceList[voice].user == 1) {
            sfv = voiceList + voice;
            i = sfv->prev;

            if (i != 0xFF) {
                voiceList[i].next = sfv->next;
            } else {
                voiceListRoot = sfv->next;
            }

            i = sfv->next;
            if (i != 0xFF) {
                voiceList[i].prev = sfv->prev;
            }

            if (voice == voiceListInsert) {
                voiceListInsert = sfv->prev;
            }

            sfv->user = 0;
        } else if (lbl_8047AF48[voice].fxFlag) {
            voiceFxRunning--;
        } else {
            voiceMusicRunning--;
        }
        if (fxFlag != FALSE) {
            ++voiceFxRunning;
        } else {
            ++voiceMusicRunning;
        }
        return voice;
    }

_fail:
    return -1;
}

void voiceFree(SYNTH_VOICE* svoice)
{
    u32 i;
    SYNTH_VOICELIST* sfv;

    macMakeInactive(svoice, 2);
    voiceRemovePriority(svoice);
    svoice->addr = NULL;
    svoice->prio = 0;
    sfv = &voiceList[(i = svoice->id & 0xFF)];
    if (sfv->user == 0) {
        sfv->user = 1;
        if (voiceListRoot != 0xFF) {
            sfv->next = 0xFF;
            sfv->prev = voiceListInsert;
            voiceList[voiceListInsert].next = i;
        } else {
            sfv->next = 0xFF;
            sfv->prev = 0xFF;
            voiceListRoot = i;
        }

        voiceListInsert = i;
        if (svoice->fxFlag != 0) {
            --voiceFxRunning;
        } else {
            --voiceMusicRunning;
        }
    }

    svoice->id = 0xFFFFFFFF;
}

static void voiceInitFreeList(void)
{
    u32 i;

    for (i = 0; i < lbl_80434C50.voiceNum; ++i) {
        voiceList[i].prev = i - 1;
        voiceList[i].next = i + 1;
        voiceList[i].user = 1;
    }

    voiceList[0].prev = 0xFF;
    voiceList[lbl_80434C50.voiceNum - 1].next = 0xFF;
    voiceListRoot = 0;
    voiceListInsert = lbl_80434C50.voiceNum - 1;
}

void synthInitAllocationAids(void)
{
    voiceInitFreeList();
    voiceInitPrioSort();
    voiceFxRunning = 0;
    voiceMusicRunning = 0;
}

u32 voiceBlock(u8 prio) /* voiceBlock */
{
    u32 voice;

    if ((voice = voiceAllocate(prio, 0xFF, 0xFFFF, 1)) != 0xFFFFFFFF) {
        lbl_8047AF48[voice].block = 1;
        lbl_8047AF48[voice].fxFlag = 1;
        lbl_8047AF48[voice].allocId = 0xFFFF;
        vidRemoveVoiceReferences(&lbl_8047AF48[voice]);
        lbl_8047AF48[voice].id = voice | 0xFFFFFF00;

        if (hwIsActive(voice)) {
            hwBreak(voice);
        }

        macMakeInactive(&lbl_8047AF48[voice], 2);
        lbl_8047AF48[voice].addr = NULL;
        voiceSetPriority(&lbl_8047AF48[voice], prio);
    }

    return voice;
}

void voiceUnblock(u32 voice)
{
    if (voice == 0xFFFFFFFF) {
        return;
    }

    if (hwIsActive(voice)) {
        hwBreak(voice);
    }

    lbl_8047AF48[voice].id = voice;
    voiceFree(&lbl_8047AF48[voice]);
    lbl_8047AF48[voice].block = 0;
}

void voiceKill(u32 vi)
{
    SYNTH_VOICE* sv = &lbl_8047AF48[vi];

    if (sv->addr != NULL) {
        vidRemoveVoiceReferences(sv);
        sv->cFlags &= ~3;
        sv->age = 0;
        voiceFree(sv);
    }

    if (sv->block != 0) {
        streamKill(vi);
    }

    hwBreak(vi);
}

s32 voiceKillSound(u32 voiceid)
{
    s32 ret = -1;
    u32 next_voiceid;
    u32 i;

    if (lbl_8047AF18 != FALSE) {
        for (voiceid = vidGetInternalId(voiceid); voiceid != -1; voiceid = next_voiceid) {
            i = voiceid & 0xFF;
            next_voiceid = lbl_8047AF48[i].child;
            if (voiceid == lbl_8047AF48[i].id) {
                voiceKill(i);
                ret = 0;
            }
        }
    }

    return ret;
}

u32 voiceIsLastStarted(SYNTH_VOICE* svoice)
{
    u32 i;

    if (svoice->id != 0xFFFFFFFF && svoice->midi != 0xFF) {
        i = svoice->id & 0xFF;
        if (svoice->midiSet == 0xFF) {
            if (synth_last_fxstarted[i] == i) {
                return TRUE;
            }
        } else if (synth_last_started[svoice->midiSet][svoice->midi] == i) {
            return TRUE;
        }
    }

    return FALSE;
}

void voiceSetLastStarted(SYNTH_VOICE* svoice)
{
    u32 i;

    if (svoice->id != 0xFFFFFFFF && svoice->midi != 0xFF) {
        i = svoice->id & 0xFF;
        if (svoice->midiSet == 0xFF) {
            synth_last_fxstarted[i] = i;
        } else {
            synth_last_started[svoice->midiSet][svoice->midi] = i;
        }
    }
}

void voiceResetLastStarted(SYNTH_VOICE* svoice)
{
    u32 i;

    if ((svoice->id != 0xFFFFFFFF) && (svoice->midi != 0xFF)) {
        i = svoice->id & 0xFF;
        if (svoice->midiSet == 0xFF) {
            if (synth_last_fxstarted[i] == i) {
                synth_last_fxstarted[i] = 0xFF;
            }
        } else if (i == synth_last_started[svoice->midiSet][svoice->midi]) {
            synth_last_started[svoice->midiSet][svoice->midi] = 0xFF;
        }
    }
}

void voiceInitLastStarted(void)
{
    u32 i;
    u32 j;

    for (i = 0; i < 8; ++i) {
        for (j = 0; j < 16; ++j) {
            synth_last_started[i][j] = 0xFF;
        }
    }

    for (j = 0; j < 64; ++j) {
        synth_last_fxstarted[j] = 0xFF;
    }
}
