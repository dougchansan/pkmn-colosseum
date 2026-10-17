/**
 * @file pslist.c
 * @brief Particle link lists: per-link particle lists, the particle free
 *        list and the draw-order sort (0x80168934 - 0x80169034).
 *
 * The file carries the name retail's asserts give it: its .rodata
 * (0x802737B8 - 0x80273820) is the string pool "pslist.c",
 * "linkNo >= 0 && linkNo < PS_NUM_LINK", "activeParticle[pp->linkNo] == pp"
 * and "parent->next == pp", produced here by HSD_ASSERT through __FILE__
 * and #cond.
 *
 * It owns the three per-link arrays in .bss (0x80452708 - 0x804527C8):
 *   0x80452708  second-pass head found by the last sort
 *   0x80452748  set when a link's list changes, cleared by the sort
 *   0x80452788  activeParticle[] (the name is the assert's): list heads
 * MWCC pools data defined in the unit, so functions that touch more than
 * one array load the unit's .bss base once and reach each array with an
 * add of its offset, as retail does (particleSort, _psListClear,
 * _psLinkInit); functions that touch one array address it directly.
 * The symbols keep their address names.
 *
 * Compiler: GC/1.3.2 -O4,p with -inline deferred and -str readonly, no
 * local pragmas.
 *   - GC/1.3 does not pool the arrays in particleSort (it folds base+index
 *     into one register and uses lwzu); 1.3.2 and 2.0 do.
 *   - Retail lays the arrays out as 0x80452708, 0x80452748, 0x80452788.
 *     Without deferred inlining MWCC orders pooled .bss by first reference,
 *     and particleSort, the first function, reaches activeParticle first.
 *     Under -inline deferred the unit is generated in reverse definition
 *     order and its .bss in reverse definition order too, so the functions
 *     and arrays are written in HAL's order (the reverse of their addresses).
 *   - -str readonly puts the assert strings in .rodata, as retail has them.
 * No function here calls another, so auto-inlining does not arise.
 */
#include "dolphin/types.h"
#include "game/script/script.h"
#include "hsd/hsd_debug.h"

#define PS_NUM_LINK 16

typedef struct PSSortBucket {
    PSParticle* head;
    PSParticle* tail;
} PSSortBucket;

/* Sort key: blend group (flags bits 25-27); particles without flag bit 3
 * go to the second pass (keys 8-15). */
#define PS_SORT_KEY(flags) \
    ((((flags) >> 25) & 7) + (((flags) & 8) ? 0 : 1) * 8)

extern void* memset(void* dst, int val, u32 size);
extern void* HSD_MemAlloc(u32 size); /* HSD_MemAlloc */
extern void HSD_Free(void* p);   /* HSD_Free */

extern PSParticle* lbl_8047B108; /* particle free list */
extern u16 lbl_8047B114;         /* peak live particles */
extern u16 lbl_8047B11A;         /* live particles */

PSParticle* lbl_80452788[PS_NUM_LINK]; /* activeParticle */
s32 lbl_80452748[PS_NUM_LINK];         /* list changed since last sort */
PSParticle* lbl_80452708[PS_NUM_LINK]; /* second-pass head of last sort */

#define activeParticle lbl_80452788

s32 _psLinkInit(s32 count)
{
    u32 link;
    s32 i;

    for (link = 0; link < PS_NUM_LINK; link++) {
        activeParticle[link] = NULL;
        lbl_80452748[link] = 0;
        lbl_80452708[link] = NULL;
    }

    lbl_8047B108 = NULL;
    for (i = count - 1; i >= 0; i--) {
        PSParticle* pp = HSD_MemAlloc(sizeof(PSParticle));

        memset(pp, 0, sizeof(PSParticle));
        if (pp == NULL) {
            return -1;
        }
        pp->next = lbl_8047B108;
        lbl_8047B108 = pp;
    }
    return i;
}

void _psListClear(void)
{
    PSParticle* pp;
    PSParticle* next;
    s32 i;

    pp = lbl_8047B108;
    while (pp != NULL) {
        next = pp->next;
        HSD_Free(pp);
        pp = next;
    }
    lbl_8047B108 = NULL;

    for (i = 0; i < PS_NUM_LINK; i++) {
        pp = activeParticle[i];
        while (pp != NULL) {
            next = pp->next;
            HSD_Free(pp);
            pp = next;
        }
        activeParticle[i] = NULL;
        lbl_80452748[i] = 0;
        lbl_80452708[i] = NULL;
    }
}

PSParticle* _psListNew(PSParticle* parent, u32 linkNo)
{
    PSParticle* pp;

    if (lbl_8047B108 == NULL) {
        lbl_8047B108 = HSD_MemAlloc(sizeof(PSParticle));
        memset(lbl_8047B108, 0, sizeof(PSParticle));
    }

    pp = lbl_8047B108;
    if (pp == NULL) {
        return NULL;
    }

    lbl_8047B11A++;
    if (lbl_8047B11A > lbl_8047B114) {
        lbl_8047B114 = lbl_8047B11A;
    }

    lbl_8047B108 = pp->next;
    if (parent == NULL) {
        pp->next = activeParticle[linkNo];
        activeParticle[linkNo] = pp;
    } else {
        pp->next = parent->next;
        parent->next = pp;
    }

    lbl_80452748[linkNo] = 1;
    return pp;
}

void _psListDelete(PSParticle* pp, PSParticle* parent)
{
    lbl_80452748[pp->linkNo] = 1;
    if (parent == NULL) {
        HSD_ASSERT(136, activeParticle[pp->linkNo] == pp);
        activeParticle[pp->linkNo] = pp->next;
    } else {
        HSD_ASSERT(139, parent->next == pp);
        parent->next = pp->next;
    }
    pp->next = lbl_8047B108;
    lbl_8047B108 = pp;
    lbl_8047B11A--;
}

PSParticle* _psListGetFirst(s32 linkNo)
{
    HSD_ASSERT(152, linkNo >= 0 && linkNo < PS_NUM_LINK);
    return activeParticle[linkNo];
}

PSParticle* particleSort(s32 linkNo, PSParticle** firstPass, PSParticle** secondPass)
{
    PSSortBucket bucket[16];
    PSParticle** head;
    PSParticle* pp;
    PSParticle* next;
    PSParticle* firstHead;
    PSParticle* secondHead;
    PSParticle** firstLink;
    PSParticle** secondLink;
    PSParticle* result;
    u32 key;
    s32 i;

    head = &activeParticle[linkNo];
    pp = *head;
    if (lbl_80452748[linkNo] == 0) {
        if (pp == NULL) {
            *firstPass = NULL;
            *secondPass = NULL;
            return NULL;
        }
        if (pp->flags & 8) {
            *firstPass = pp;
        } else {
            *firstPass = NULL;
        }
        *secondPass = lbl_80452708[linkNo];
        return pp;
    }

    lbl_80452748[linkNo] = 0;
    if (pp == NULL) {
        lbl_80452708[linkNo] = NULL;
        *firstPass = NULL;
        *secondPass = NULL;
        return NULL;
    }

    /* Bucket runs of equal key, keeping each run's own links. */
    memset(bucket, 0, sizeof(bucket));
    key = PS_SORT_KEY(pp->flags);
    bucket[key].head = pp;
    for (next = pp->next; next != NULL; next = next->next) {
        if ((pp->flags ^ next->flags) & 0x0E000008) {
            bucket[key].tail = pp;
            key = PS_SORT_KEY(next->flags);
            if (bucket[key].head == NULL) {
                bucket[key].head = next;
            } else {
                bucket[key].tail->next = next;
            }
        }
        pp = next;
    }
    bucket[key].tail = pp;

    firstLink = NULL;
    firstHead = NULL;
    secondLink = NULL;
    secondHead = NULL;
    for (i = 0; i < 8; i++) {
        if (bucket[i].head != NULL) {
            if (firstHead == NULL) {
                firstHead = bucket[i].head;
            } else {
                *firstLink = bucket[i].head;
            }
            firstLink = &bucket[i].tail->next;
        }
    }
    for (i = 8; i < 16; i++) {
        if (bucket[i].head != NULL) {
            if (secondHead == NULL) {
                secondHead = bucket[i].head;
            } else {
                *secondLink = bucket[i].head;
            }
            secondLink = &bucket[i].tail->next;
        }
    }

    result = NULL;
    if (firstLink != NULL) {
        result = firstHead;
        *firstLink = secondHead;
    }
    if (secondLink != NULL) {
        if (result == NULL) {
            result = secondHead;
        }
        *secondLink = NULL;
    }

    *head = result;
    lbl_80452708[linkNo] = secondHead;
    *firstPass = firstHead;
    *secondPass = secondHead;
    return result;
}
