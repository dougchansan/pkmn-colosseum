/**
 * pslist.c - Particle Script List Management
 *
 * Manages the linked lists of active script particles. The system uses
 * 16 priority-based linked lists (link 0 = highest priority). Each link
 * has a head pointer, an active flag, and a data bank pointer.
 *
 * A free list (singly linked) holds unused PSParticle structs. When a
 * new script is spawned, a particle is removed from the free list and
 * inserted into the appropriate link list. When a script terminates,
 * its particle is returned to the free list.
 *
 * Source file confirmed by rodata string: "pslist.c" at 0x802737B8
 *
 * Address range: 0x80168C64 - 0x80169104 (core list functions)
 * Additional functions extend through ~0x8016A000
 */

#include "game/script/script.h"

/* ======================================================================
 * BSS / SDA globals
 * ====================================================================== */

/** Free list head pointer (SDA: lbl_8047B108) */
static PSParticle* sFreeListHead;

/** Active particle count (SDA: lbl_8047B11A) */
static u16 sActiveCount;

/** Peak active count, for diagnostics (SDA: lbl_8047B114) */
static u16 sPeakActiveCount;

/** Per-link active flags: 1 if link has had activity (lbl_80452748, 16 entries) */
static u32 sLinkActiveFlags[PS_NUM_LINK];

/** Per-link head pointers for active particle lists (lbl_80452788, 16 entries) */
static PSParticle* sLinkHeads[PS_NUM_LINK];

/** Per-link data bank pointers (lbl_80452708, 16 entries)
 *  These hold script bytecode data for each link. */
static void* sLinkDataBanks[PS_NUM_LINK];

/* ======================================================================
 * _psListGetFirst | pslist_GetHead
 * Size: 0x6C
 *
 * Returns the head particle of the given link list.
 * Asserts that linkNo is in range [0, PS_NUM_LINK).
 * ====================================================================== */
PSParticle* pslist_GetHead(s32 linkNo) {
    s32 valid = 0;

    if (linkNo >= 0 && linkNo < PS_NUM_LINK) {
        valid = 1;
    }

    if (valid == 0) {
        /* Assert: "linkNo >= 0 && linkNo < PS_NUM_LINK" */
        /* File: "pslist.c", Line: 0x98 (152) */
        __assert("pslist.c", 0x98, "linkNo >= 0 && linkNo < PS_NUM_LINK");
    }

    return sLinkHeads[linkNo];
}

/* ======================================================================
 * _psListDelete | pslist_Unlink
 * Size: 0xDC
 *
 * Removes a particle from its link list and places it on the free list.
 *
 * If parent is NULL:
 *   - Removes pp from the head of its link's list
 *   - Asserts that sLinkHeads[pp->linkNo] == pp
 *   - Sets sLinkHeads[pp->linkNo] = pp->next
 *
 * If parent is non-NULL:
 *   - Asserts that parent->next == pp
 *   - Sets parent->next = pp->next
 *
 * In both cases:
 *   - Marks the link as active (sLinkActiveFlags[pp->linkNo] = 1)
 *   - Prepends pp to the free list
 *   - Decrements sActiveCount
 * ====================================================================== */
void pslist_Unlink(PSParticle* pp, PSParticle* parent) {
    /* Mark this link as active */
    sLinkActiveFlags[pp->linkNo] = 1;

    if (parent == NULL) {
        /* Removing from head of list */
        if (sLinkHeads[pp->linkNo] != pp) {
            /* Assert: "activeParticle[pp->linkNo] == pp" */
            __assert("pslist.c", 0x88,
                         "activeParticle[pp->linkNo] == pp");
        }
        sLinkHeads[pp->linkNo] = pp->next;
    } else {
        /* Removing from middle/end of list */
        if (parent->next != pp) {
            /* Assert: "parent->next == pp" */
            __assert("pslist.c", 0x8B,
                         "parent->next == pp");
        }
        parent->next = pp->next;
    }

    /* Prepend to free list */
    pp->next = sFreeListHead;
    sFreeListHead = pp;

    /* Decrement active count */
    sActiveCount--;
}

/* ======================================================================
 * fn_80168DAC | pslist_Alloc
 * Size: 0xDC
 *
 * Allocates a particle from the free list. If the free list is empty,
 * allocates a new PSParticle struct from the heap.
 *
 * The particle is inserted into the given link list:
 *   - If insertAfter is NULL, inserts at the head of sLinkHeads[linkNo]
 *   - If insertAfter is non-NULL, inserts after that particle
 *
 * Returns the new particle, or NULL if allocation failed.
 * ====================================================================== */
PSParticle* pslist_Alloc(PSParticle* insertAfter, u32 linkNo) {
    PSParticle* pp;

    /* Ensure free list has at least one entry */
    if (sFreeListHead == NULL) {
        pp = (PSParticle*)HSD_MemAlloc(PS_PARTICLE_SIZE);  /* fn_801A6928 */
        if (pp == NULL) {
            return NULL;
        }
        memset(pp, 0, PS_PARTICLE_SIZE);
    }

    if (sFreeListHead == NULL) {
        return NULL;
    }

    /* Track stats */
    sActiveCount++;
    if (sActiveCount > sPeakActiveCount) {
        sPeakActiveCount = sActiveCount;
    }

    /* Remove from free list */
    pp = sFreeListHead;
    sFreeListHead = pp->next;

    /* Insert into link list */
    if (insertAfter == NULL) {
        /* Insert at head */
        pp->next = sLinkHeads[linkNo];
        sLinkHeads[linkNo] = pp;
    } else {
        /* Insert after given particle */
        pp->next = insertAfter->next;
        insertAfter->next = pp;
    }

    /* Mark link as active */
    sLinkActiveFlags[linkNo] = 1;

    return pp;
}

/* ======================================================================
 * fn_80168E88 | pslist_DestroyAll
 * Size: 0xA0
 *
 * Frees all particles from all lists (free list + all 16 link lists).
 * Walks each list and calls HSD_MemFree on every particle.
 * Clears all head pointers, data bank pointers, and active flags.
 * ====================================================================== */
void pslist_DestroyAll(void) {
    PSParticle* cur;
    PSParticle* next;
    s32 i;

    /* Free all particles on the free list */
    cur = sFreeListHead;
    while (cur != NULL) {
        next = cur->next;
        HSD_MemFree(cur);  /* fn_801A6960 */
        cur = next;
    }
    sFreeListHead = NULL;

    /* Free all particles on all 16 link lists */
    for (i = 0; i < PS_NUM_LINK; i++) {
        cur = sLinkHeads[i];  /* lbl_80452708 + 0x80 offset = lbl_80452788 */
        while (cur != NULL) {
            next = cur->next;
            HSD_MemFree(cur);
            cur = next;
        }
        sLinkHeads[i] = NULL;
        sLinkActiveFlags[i] = 0;
        sLinkDataBanks[i] = NULL;
    }
}

/* ======================================================================
 * fn_80168F28 | pslist_Init
 * Size: 0x10C
 *
 * Initializes the particle system pool.
 *   1. Clears all 16 link list arrays (heads, flags, data banks)
 *   2. Clears the free list head
 *   3. Pre-allocates (count) particles and links them into the free list
 *
 * Returns -1 if any allocation fails, otherwise returns 0+.
 * ====================================================================== */
s32 pslist_Init(s32 count) {
    s32 i;
    PSParticle* pp;

    /* Clear all 16 link lists (done in groups of 8 per iteration, 2 iterations) */
    for (i = 0; i < PS_NUM_LINK; i++) {
        sLinkHeads[i] = NULL;
        sLinkActiveFlags[i] = 0;
        sLinkDataBanks[i] = NULL;
    }

    sFreeListHead = NULL;

    /* Pre-allocate particles into the free list */
    for (i = count - 1; i >= 0; i--) {
        pp = (PSParticle*)HSD_MemAlloc(PS_PARTICLE_SIZE);
        if (pp == NULL) {
            return -1;
        }
        memset(pp, 0, PS_PARTICLE_SIZE);

        /* Prepend to free list */
        pp->next = sFreeListHead;
        sFreeListHead = pp;
    }

    return i;  /* Returns final loop counter value */
}

/* ======================================================================
 * fn_80169034 | pslist_UpdateVisibility
 * Size: 0xD0
 *
 * Iterates through all particles in the same link as pp, and all
 * particles in the generator list. For each particle matching pp's
 * scriptId (field 0x18), sets or clears the NO_DETACH flag based
 * on the visible parameter.
 *
 * visible == 0: set NO_DETACH flag (oris 0x2000)
 * visible != 0: clear NO_DETACH flag (rlwinm clear bits 2-3)
 * ====================================================================== */
void pslist_UpdateVisibility(PSParticle* pp, u8 visible) {
    PSParticle* cur;

    /* Iterate particles in the same link list */
    cur = pslist_GetHead(pp->color1A);  /* field at 0x15 used as link ref */
    while (cur != NULL) {
        if (cur->scriptId == pp->scriptId) {
            if (visible) {
                cur->flags &= ~PS_FLAG_NO_DETACH;
            } else {
                cur->flags |= PS_FLAG_NO_DETACH;
            }
        }
        cur = cur->next;
    }

    /* Also iterate the generator list (separate global list) */
    /* cur = sGeneratorListHead; (lbl_8047B188) */
    /* ... same logic ... */
}
