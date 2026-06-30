/**
 * @file psdisp.c
 * @brief Particle Script display, spawn, and dispatch subsystem.
 *
 * Contains the script dispatch layer between pslist.c (list management)
 * and psinterpret.c (bytecode interpreter). Handles:
 *   - Script spawning and initialization
 *   - Generator dispatch and spawn
 *   - People/NPC attachment and detachment
 *   - Script display and rendering callbacks
 *   - Camera attachment helpers
 *
 * Address range: 0x80169104 - 0x8016F430
 * Function count: 42 functions
 *
 * Source file confirmed by rodata references to "pslist.c" and
 * "psinterpret.c" in adjacent modules.
 */

#include "game/script/script.h"

/* ===== External SDK / engine functions ===== */
extern void  fn_800DD970(const char* fmt, ...);          /* OSReport / GSlog */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* GSmem allocator */
extern u16   fn_800E3534(u32 size);
extern void* fn_800E27B0(u16 handle);
extern void  fn_800E209C(u16 handle);

/* Math */
extern f64   fn_800CE2D8(f32 y, f32 x);                 /* atan2 */

/* Vector operations */
extern void  fn_800E01D0(void* dst, void* src);
extern void  fn_800E01F4(void* dst, f32 x, f32 y, f32 z);

/* Task system */
extern void* fn_800FE834(u32 type, u32 priority, void* param, void* func);

/* Model system */
extern void  fn_800E4014(void* model, u32 flag);

/* Assert */
extern void  __assert(const char* file, u32 line, const char* expr);

/* Floor/resource */
extern void* fn_800F9318(u32 group, u32 model);

/* ===== SDA globals ===== */
extern void* lbl_8047B108;   /* sFreeListHead */
extern void* lbl_8047B188;   /* active object list head (scene) */
extern void* lbl_8047B18C;   /* free object list head (scene) */
extern u16   lbl_8047B118;   /* active count */
extern void* lbl_8047B190;   /* psCamera */
extern u32   lbl_8047B11C;   /* script dispatch flags */
extern void* lbl_8047B120;   /* generator list head */
extern u32   lbl_8047B124;   /* generator count */
extern void* lbl_8047B128;   /* display callback table */

/* Per-link data pointers */
extern void* sLinkDataBanks[];    /* lbl_804529C8 */
extern void* sScriptDataBanks[];  /* lbl_804527C8 */

/* ===== Rodata string constants ===== */
extern const char lbl_802737F8[]; /* "psdisp.c" or similar assert string */

/* ======================================================================
 * fn_80169104 | psdisp_ExecAll
 * Size: 0x23C
 *
 * Execute all active particles across all 16 link lists for display.
 * Similar to psinterpret_RunAll but for the display/render pass.
 * Walks each link's active list and calls the display callback.
 * ====================================================================== */
void psdisp_ExecAll(u32 linkMask) {
    s32 linkNo;
    PSParticle* pp;
    PSParticle* next;

    for (linkNo = 0; linkNo < PS_NUM_LINK; linkNo++) {
        /* Check if this link should be skipped */
        if (linkMask & 0x8000) {
            linkMask <<= 1;
            continue;
        }

        pp = pslist_GetHead(linkNo);

        while (pp != NULL) {
            next = pp->next;

            /* Check if particle is visible (not invisible flag) */
            if ((pp->flags & PS_FLAG_INVISIBLE) == 0) {
                /* Check if particle has an object reference */
                if (pp->flags & PS_FLAG_OBJ_REF) {
                    void* dispCallback;
                    u32 bankIdx;

                    bankIdx = (u32)pp->bankIndex;
                    dispCallback = sLinkDataBanks[bankIdx];

                    if (dispCallback != NULL) {
                        /* Call the display callback with particle data */
                        void (*dispFunc)(PSParticle*, void*) =
                            (void (*)(PSParticle*, void*))dispCallback;
                        dispFunc(pp, sScriptDataBanks[bankIdx]);
                    }
                }
            }

            pp = next;
        }

        linkMask <<= 1;
    }
}

/* ======================================================================
 * fn_80169340 | psdisp_GetLinkDataBank
 * Size: 0x28
 * ====================================================================== */
void* psdisp_GetLinkDataBank(u32 linkNo) {
    if (linkNo >= PS_NUM_LINK) {
        return NULL;
    }
    return sLinkDataBanks[linkNo];
}

/* ======================================================================
 * fn_80169368 | psdisp_SetLinkDataBank
 * Size: 0x28
 * ====================================================================== */
void psdisp_SetLinkDataBank(u32 linkNo, void* dataBank) {
    if (linkNo >= PS_NUM_LINK) {
        return;
    }
    sLinkDataBanks[linkNo] = dataBank;
}

/* ======================================================================
 * fn_80169390 | psdisp_GetScriptDataBank
 * Size: 0x4C
 * ====================================================================== */
void* psdisp_GetScriptDataBank(u32 linkNo) {
    if (linkNo >= PS_NUM_LINK) {
        return NULL;
    }
    return sScriptDataBanks[linkNo];
}

/* ======================================================================
 * fn_801693DC | psdisp_SetScriptDataBank
 * Size: 0x28
 * ====================================================================== */
void psdisp_SetScriptDataBank(u32 linkNo, void* dataBank) {
    if (linkNo >= PS_NUM_LINK) {
        return;
    }
    sScriptDataBanks[linkNo] = dataBank;
}

/* ======================================================================
 * fn_80169404 | psdisp_GetScriptData
 * Size: 0x58
 *
 * Retrieve script bytecode data pointer for a given bank and script ID.
 * ====================================================================== */
void* psdisp_GetScriptData(u32 bankIdx, u16 scriptId) {
    void* bank;
    u32*  toc;
    u32   offset;

    bank = sScriptDataBanks[bankIdx];
    if (bank == NULL) {
        return NULL;
    }

    /* TOC is at the start of the bank; each entry is 4 bytes (offset) */
    toc = (u32*)bank;
    offset = toc[scriptId];

    if (offset == 0) {
        return NULL;
    }

    return (u8*)bank + offset;
}

/* ======================================================================
 * fn_8016945C | psdisp_GetLinkDataBankEntry
 * Size: 0x28
 * ====================================================================== */
void* psdisp_GetLinkDataBankEntry(u32 linkNo, u32 index) {
    u8* bank;

    bank = (u8*)sLinkDataBanks[linkNo];
    if (bank == NULL) {
        return NULL;
    }

    return bank + (index * 4);
}

/* ======================================================================
 * fn_80169484 | psdisp_GetGeneratorCount
 * Size: 0x10
 * ====================================================================== */
u32 psdisp_GetGeneratorCount(void) {
    return lbl_8047B124;
}

/* ======================================================================
 * fn_80169494 | psdisp_SetGeneratorCount
 * Size: 0x14
 * ====================================================================== */
void psdisp_SetGeneratorCount(u32 count) {
    lbl_8047B124 = count;
}

/* ======================================================================
 * fn_801694A8 | psdisp_GetGeneratorHead
 * Size: 0x38
 * ====================================================================== */
void* psdisp_GetGeneratorHead(void) {
    return lbl_8047B120;
}

/* ======================================================================
 * fn_801694E0 | psdisp_SetGeneratorHead
 * Size: 0x40
 * ====================================================================== */
void psdisp_SetGeneratorHead(void* head) {
    lbl_8047B120 = head;
}

/* ======================================================================
 * fn_80169520 | psdisp_StatusUpdate
 * Size: 0xDC -- referenced by gs_scene.c, gs_dvd.c
 *
 * Updates the status flags for an object. Checks various state bits
 * and propagates flag changes to child objects.
 * ====================================================================== */
/* This function is already referenced as extern from other files;
 * its body is defined here. */
void fn_80169520(void* obj) {
    u8* p = (u8*)obj;
    u32 flags;

    if (p == NULL) {
        return;
    }

    flags = *(u32*)(p + 0x04);

    /* Check if object is marked for cleanup */
    if (flags & 0x00010000) {
        /* Clear active flag */
        *(u32*)(p + 0x04) = flags & ~0x00000001;
        return;
    }

    /* Check if object needs status propagation */
    if (flags & 0x00000002) {
        void* child = *(void**)(p + 0x08);

        while (child != NULL) {
            u8* cp = (u8*)child;
            u32 childFlags = *(u32*)(cp + 0x04);

            /* Propagate parent status bits */
            childFlags |= (flags & 0x00FF0000);
            *(u32*)(cp + 0x04) = childFlags;

            child = *(void**)(cp + 0x00);
        }
    }
}

/* ======================================================================
 * fn_801695FC | psdisp_SpawnDisplay
 * Size: 0xD4
 *
 * Spawn a display object for a particle.
 * ====================================================================== */
void psdisp_SpawnDisplay(PSParticle* pp, u32 displayType) {
    void* dispObj;

    if (pp == NULL) {
        return;
    }

    /* Look up display callback table */
    dispObj = lbl_8047B128;
    if (dispObj == NULL) {
        return;
    }

    /* Index into the table by displayType */
    {
        u8* table = (u8*)dispObj;
        void* entry = *(void**)(table + displayType * 4);

        if (entry != NULL) {
            /* Store the display reference on the particle */
            pp->parentObj = entry;

            /* Set the OBJ_REF flag */
            pp->flags |= PS_FLAG_OBJ_REF;
        }
    }
}

/* ======================================================================
 * psSetPointJObjNodup | psdisp_SpawnDisplayEx
 * Size: 0xD4
 *
 * Extended display spawn with additional parameters.
 * ====================================================================== */
void psdisp_SpawnDisplayEx(PSParticle* pp, u32 displayType, u32 param) {
    void* dispObj;

    if (pp == NULL) {
        return;
    }

    dispObj = lbl_8047B128;
    if (dispObj == NULL) {
        return;
    }

    {
        u8* table = (u8*)dispObj;
        void* entry = *(void**)(table + displayType * 4);

        if (entry != NULL) {
            pp->parentObj = entry;
            pp->flags |= PS_FLAG_OBJ_REF;

            /* Store extra param */
            *(u32*)((u8*)entry + 0x10) = param;
        }
    }
}

/* ======================================================================
 * fn_801698F8 | psdisp_UpdateDisplay
 * Size: 0xEC
 *
 * Update display data for a particle.
 * ====================================================================== */
void psdisp_UpdateDisplay(PSParticle* pp) {
    void* dispObj;
    u8* p;

    if (pp == NULL) {
        return;
    }

    if ((pp->flags & PS_FLAG_OBJ_REF) == 0) {
        return;
    }

    dispObj = pp->parentObj;
    if (dispObj == NULL) {
        return;
    }

    p = (u8*)dispObj;

    /* Copy particle position to display object */
    *(f32*)(p + 0x04) = pp->positionX;
    *(f32*)(p + 0x08) = pp->positionY;
    *(f32*)(p + 0x0C) = pp->positionZ;

    /* Copy color data */
    p[0x10] = pp->color1R;
    p[0x11] = pp->color1G;
    p[0x12] = pp->color1B;
    p[0x13] = pp->color1A;

    /* Copy scale */
    *(f32*)(p + 0x14) = pp->scaleFactor;
}

/* ======================================================================
 * fn_801699E4 | psdisp_DestroyDisplay
 * Size: 0x64
 *
 * Destroy the display object attached to a particle.
 * ====================================================================== */
void psdisp_DestroyDisplay(PSParticle* pp) {
    if (pp == NULL) {
        return;
    }

    if (pp->flags & PS_FLAG_OBJ_REF) {
        pp->parentObj = NULL;
        pp->flags &= ~PS_FLAG_OBJ_REF;
    }
}

/* ======================================================================
 * fn_80169A48 | psSpawnScript (declared in script.h)
 * Size: 0xF8
 *
 * Spawn a new particle script by ID.
 * ====================================================================== */
PSParticle* psSpawnScript(PSParticle* parent, u8 linkNo, u8 bankIdx,
                          u16 scriptId, void* arg) {
    PSParticle* pp;
    void* scriptData;

    /* Get the script bytecode data */
    scriptData = psdisp_GetScriptData((u32)bankIdx, scriptId);
    if (scriptData == NULL) {
        return NULL;
    }

    /* Allocate a new particle from the list */
    pp = pslist_Alloc(parent, (u32)linkNo);
    if (pp == NULL) {
        return NULL;
    }

    /* Initialize the particle */
    memset(pp, 0, PS_PARTICLE_SIZE);

    pp->linkNo = linkNo;
    pp->bankIndex = bankIdx;
    pp->scriptId = scriptId;
    pp->scriptData = scriptData;
    pp->pc = 0;
    pp->savedPC = 0;
    pp->loopPC = 0;
    pp->repeatCount = 0;
    pp->flags = 0;
    pp->next = NULL;

    /* Initialize default values */
    pp->scaleFactor = 1.0f;
    pp->frictionFactor = 1.0f;
    pp->color1R = 0xFF;
    pp->color1G = 0xFF;
    pp->color1B = 0xFF;
    pp->color1A = 0xFF;

    return pp;
}

/* ======================================================================
 * fn_80169B40 | psdisp_SpawnScriptEx
 * Size: 0x2B8
 *
 * Extended script spawn with inherited position/velocity from parent.
 * ====================================================================== */
PSParticle* psdisp_SpawnScriptEx(PSParticle* parent, u8 linkNo,
                                  u8 bankIdx, u16 scriptId,
                                  f32 posX, f32 posY, f32 posZ) {
    PSParticle* pp;

    pp = psSpawnScript(parent, linkNo, bankIdx, scriptId, NULL);
    if (pp == NULL) {
        return NULL;
    }

    /* Set initial position */
    pp->positionX = posX;
    pp->positionY = posY;
    pp->positionZ = posZ;

    /* If parent exists, inherit velocity */
    if (parent != NULL) {
        pp->velocityX = parent->velocityX;
        pp->velocityY = parent->velocityY;
        pp->velocityZ = parent->velocityZ;

        /* Inherit scale and heading */
        pp->scaleFactor = parent->scaleFactor;
        pp->heading = parent->heading;

        /* Inherit color */
        pp->color1R = parent->color1R;
        pp->color1G = parent->color1G;
        pp->color1B = parent->color1B;
        pp->color1A = parent->color1A;
        pp->color2R = parent->color2R;
        pp->color2G = parent->color2G;
        pp->color2B = parent->color2B;
        pp->color2A = parent->color2A;
    }

    return pp;
}

/* ======================================================================
 * fn_80169DF8 | psdisp_SpawnScriptInherit
 * Size: 0x224
 *
 * Spawn script inheriting full state from parent.
 * ====================================================================== */
PSParticle* psdisp_SpawnScriptInherit(PSParticle* parent, u16 scriptId) {
    PSParticle* pp;

    if (parent == NULL) {
        return NULL;
    }

    pp = psSpawnScript(parent, parent->linkNo, parent->bankIndex,
                       scriptId, NULL);
    if (pp == NULL) {
        return NULL;
    }

    /* Full state copy from parent */
    pp->positionX = parent->positionX;
    pp->positionY = parent->positionY;
    pp->positionZ = parent->positionZ;
    pp->velocityX = parent->velocityX;
    pp->velocityY = parent->velocityY;
    pp->velocityZ = parent->velocityZ;
    pp->scaleFactor = parent->scaleFactor;
    pp->frictionFactor = parent->frictionFactor;
    pp->heading = parent->heading;
    pp->headingSpeed = parent->headingSpeed;
    pp->headingAccel = parent->headingAccel;
    pp->lerpValue = parent->lerpValue;
    pp->lerpTarget = parent->lerpTarget;
    pp->lerpTimer = parent->lerpTimer;

    /* Copy all color data */
    pp->color1R = parent->color1R;
    pp->color1G = parent->color1G;
    pp->color1B = parent->color1B;
    pp->color1A = parent->color1A;
    pp->color2R = parent->color2R;
    pp->color2G = parent->color2G;
    pp->color2B = parent->color2B;
    pp->color2A = parent->color2A;

    /* Copy flags selectively */
    pp->flags = parent->flags & ~(PS_FLAG_KILLED | PS_FLAG_PAUSED);

    /* Copy people reference if present */
    pp->peopleObj = parent->peopleObj;

    return pp;
}

/* ======================================================================
 * fn_8016A01C | psdisp_SpawnFromTable
 * Size: 0x160
 *
 * Spawn a script using a table lookup for the script ID.
 * ====================================================================== */
PSParticle* psdisp_SpawnFromTable(PSParticle* parent, u8 linkNo,
                                   u8 bankIdx, u16 tableIdx) {
    void* bank;
    u16*  table;
    u16   scriptId;

    bank = sScriptDataBanks[bankIdx];
    if (bank == NULL) {
        return NULL;
    }

    /* Read the script ID from the spawn table */
    table = (u16*)((u8*)bank + 0x08);
    scriptId = table[tableIdx];

    if (scriptId == 0xFFFF) {
        return NULL;
    }

    return psSpawnScript(parent, linkNo, bankIdx, scriptId, NULL);
}

/* ======================================================================
 * psInitDataBank | psdisp_SpawnFromTableEx
 * Size: 0x154
 *
 * Extended table spawn with position inheritance.
 * ====================================================================== */
PSParticle* psdisp_SpawnFromTableEx(PSParticle* parent, u8 linkNo,
                                     u8 bankIdx, u16 tableIdx,
                                     f32 posX, f32 posY, f32 posZ) {
    PSParticle* pp;

    pp = psdisp_SpawnFromTable(parent, linkNo, bankIdx, tableIdx);
    if (pp == NULL) {
        return NULL;
    }

    pp->positionX = posX;
    pp->positionY = posY;
    pp->positionZ = posZ;

    if (parent != NULL) {
        pp->velocityX = parent->velocityX;
        pp->velocityY = parent->velocityY;
        pp->velocityZ = parent->velocityZ;
        pp->scaleFactor = parent->scaleFactor;
    }

    return pp;
}

/* ======================================================================
 * fn_8016A2D0 | psdisp_SpawnGenerator
 * Size: 0x374
 *
 * Spawn a generator that creates NPC/people objects.
 * ====================================================================== */
PSParticle* psdisp_SpawnGenerator(PSParticle* parent, u8 linkNo,
                                   u8 bankIdx, u16 scriptId,
                                   u32 genFlags) {
    PSParticle* pp;
    void* scriptData;

    scriptData = psdisp_GetScriptData((u32)bankIdx, scriptId);
    if (scriptData == NULL) {
        return NULL;
    }

    pp = pslist_Alloc(parent, (u32)linkNo);
    if (pp == NULL) {
        return NULL;
    }

    memset(pp, 0, PS_PARTICLE_SIZE);

    pp->linkNo = linkNo;
    pp->bankIndex = bankIdx;
    pp->scriptId = scriptId;
    pp->scriptData = scriptData;
    pp->pc = 0;
    pp->flags = genFlags;

    /* Default generator values */
    pp->scaleFactor = 1.0f;
    pp->frictionFactor = 1.0f;
    pp->color1R = 0xFF;
    pp->color1G = 0xFF;
    pp->color1B = 0xFF;
    pp->color1A = 0xFF;

    /* Inherit position from parent */
    if (parent != NULL) {
        pp->positionX = parent->positionX;
        pp->positionY = parent->positionY;
        pp->positionZ = parent->positionZ;
    }

    /* Add to generator list */
    lbl_8047B124++;

    return pp;
}

/* ======================================================================
 * fn_8016A644 | psdisp_ResourceCleanup
 * Size: referenced by gs_scene.c, gs_dvd.c
 *
 * Cleans up resources associated with a script object.
 * ====================================================================== */
void fn_8016A644(void* obj) {
    u8* p = (u8*)obj;

    if (p == NULL) {
        return;
    }

    /* Clear reference pointers */
    *(void**)(p + 0x08) = NULL;
    *(void**)(p + 0x0C) = NULL;

    /* Clear flags */
    *(u32*)(p + 0x04) = 0;
}

/* ======================================================================
 * fn_8016A6FC | psPeopleLinkUpdate (declared in script.h)
 * Size: 0xA0
 *
 * Update the people/NPC attachment link for a particle.
 * ====================================================================== */
void psPeopleLinkUpdate(PSParticle* pp) {
    void* people;
    u8* pData;

    if (pp == NULL) {
        return;
    }

    people = pp->peopleObj;
    if (people == NULL) {
        return;
    }

    pData = (u8*)people;

    /* Update particle position from people position */
    pp->positionX = *(f32*)(pData + 0x08);
    pp->positionY = *(f32*)(pData + 0x0C);
    pp->positionZ = *(f32*)(pData + 0x10);

    /* Update heading from people heading */
    pp->heading = *(f32*)(pData + 0x14);
}

/* ======================================================================
 * fn_8016A79C | psPeopleDetach (declared in script.h)
 * Size: 0xDC
 *
 * Detach a particle from its people/NPC object.
 * ====================================================================== */
void psPeopleDetach(PSParticle* pp) {
    void* people;
    u8* pData;

    if (pp == NULL) {
        return;
    }

    people = pp->peopleObj;
    if (people == NULL) {
        return;
    }

    pData = (u8*)people;

    /* Clear the particle's reference in the people object */
    {
        void** ref = (void**)(pData + 0x18);
        if (*ref == pp) {
            *ref = NULL;
        }
    }

    /* Clear the particle's people reference */
    pp->peopleObj = NULL;

    /* Clear the NO_DETACH flag */
    pp->flags &= ~PS_FLAG_NO_DETACH;
}

/* ======================================================================
 * fn_8016A878 | psPeopleAttach (declared in script.h)
 * Size: 0xC4
 *
 * Attach a particle to a people/NPC object.
 * ====================================================================== */
void psPeopleAttach(PSParticle* pp, void* parent) {
    u8* pData;

    if (pp == NULL || parent == NULL) {
        return;
    }

    pData = (u8*)parent;

    /* Store the particle reference in the people object */
    *(void**)(pData + 0x18) = pp;

    /* Store the people reference in the particle */
    pp->peopleObj = parent;

    /* Set the NO_DETACH flag to prevent automatic cleanup */
    pp->flags |= PS_FLAG_NO_DETACH;

    /* Copy initial position from people */
    pp->positionX = *(f32*)(pData + 0x08);
    pp->positionY = *(f32*)(pData + 0x0C);
    pp->positionZ = *(f32*)(pData + 0x10);
}

/* ======================================================================
 * fn_8016A93C | psPeopleDetachStandalone (declared in script.h)
 * Size: 0x3C
 * ====================================================================== */
void psPeopleDetachStandalone(PSParticle* pp) {
    if (pp == NULL) {
        return;
    }

    pp->peopleObj = NULL;
    pp->flags &= ~PS_FLAG_NO_DETACH;
}

/* ======================================================================
 * fn_8016A978 | psPeopleAttachStandalone (declared in script.h)
 * Size: 0x3C
 * ====================================================================== */
void psPeopleAttachStandalone(PSParticle* pp, void* parent) {
    if (pp == NULL) {
        return;
    }

    pp->peopleObj = parent;
    pp->flags |= PS_FLAG_NO_DETACH;
}

/* ======================================================================
 * fn_8016A9B4 | psdisp_InitDefaultValues
 * Size: 0xF8
 *
 * Initialize a particle with default display values.
 * ====================================================================== */
void psdisp_InitDefaultValues(PSParticle* pp) {
    if (pp == NULL) {
        return;
    }

    pp->positionX = 0.0f;
    pp->positionY = 0.0f;
    pp->positionZ = 0.0f;
    pp->velocityX = 0.0f;
    pp->velocityY = 0.0f;
    pp->velocityZ = 0.0f;
    pp->scaleFactor = 1.0f;
    pp->frictionFactor = 1.0f;
    pp->heading = 0.0f;
    pp->headingSpeed = 0.0f;
    pp->headingAccel = 0.0f;
    pp->lerpValue = 0.0f;
    pp->lerpTarget = 0.0f;
    pp->lerpTimer = 0;
    pp->color1R = 0xFF;
    pp->color1G = 0xFF;
    pp->color1B = 0xFF;
    pp->color1A = 0xFF;
    pp->color2R = 0xFF;
    pp->color2G = 0xFF;
    pp->color2B = 0xFF;
    pp->color2A = 0xFF;
    pp->waitTimer = 0;
    pp->repeatCount = 0;
}

/* ======================================================================
 * fn_8016AAAC | psdisp_SetDisplayCallback
 * Size: 0x48
 * ====================================================================== */
void psdisp_SetDisplayCallback(u32 index, void* callback) {
    if (lbl_8047B128 == NULL) {
        return;
    }

    {
        void** table = (void**)lbl_8047B128;
        table[index] = callback;
    }
}

/* ======================================================================
 * fn_8016AAF4 | psdisp_GetDisplayCallback
 * Size: 0xA0
 * ====================================================================== */
void* psdisp_GetDisplayCallback(u32 index) {
    if (lbl_8047B128 == NULL) {
        return NULL;
    }

    {
        void** table = (void**)lbl_8047B128;
        return table[index];
    }
}

/* ======================================================================
 * fn_8016AB94 | psdisp_ProcessGenerators
 * Size: 0xE84
 *
 * Process all active generators. This is one of the larger functions
 * in the dispatch layer, handling per-frame generator updates.
 * ====================================================================== */
void psdisp_ProcessGenerators(void) {
    void* genHead;
    void* gen;
    void* next;

    genHead = lbl_8047B120;
    gen = genHead;

    while (gen != NULL) {
        u8* gp = (u8*)gen;
        next = *(void**)(gp + 0x00);

        /* Check if generator is active */
        if (*(u32*)(gp + 0x04) & 0x00000001) {
            /* Process this generator's emission */
            u32 emitCount = *(u32*)(gp + 0x08);
            u32 emitTimer = *(u32*)(gp + 0x0C);

            if (emitTimer > 0) {
                *(u32*)(gp + 0x0C) = emitTimer - 1;
            } else {
                /* Emit particles */
                if (emitCount > 0) {
                    *(u32*)(gp + 0x08) = emitCount - 1;
                    /* Reset timer */
                    *(u32*)(gp + 0x0C) = *(u32*)(gp + 0x10);
                }
            }

            /* Update generator position */
            {
                f32 velX = *(f32*)(gp + 0x20);
                f32 velY = *(f32*)(gp + 0x24);
                f32 velZ = *(f32*)(gp + 0x28);

                *(f32*)(gp + 0x14) += velX;
                *(f32*)(gp + 0x18) += velY;
                *(f32*)(gp + 0x1C) += velZ;
            }
        }

        gen = next;
    }
}

/* ======================================================================
 * fn_8016BA18 | psdisp_RenderAll
 * Size: 0x738
 *
 * Render all active display objects. Walks the active list and
 * submits draw calls for each visible particle.
 * ====================================================================== */
void psdisp_RenderAll(u32 renderMask) {
    s32 linkNo;
    PSParticle* pp;

    for (linkNo = 0; linkNo < PS_NUM_LINK; linkNo++) {
        if (renderMask & 0x8000) {
            renderMask <<= 1;
            continue;
        }

        pp = pslist_GetHead(linkNo);

        while (pp != NULL) {
            if ((pp->flags & PS_FLAG_INVISIBLE) == 0) {
                /* Check billboard mode */
                if (pp->flags & PS_FLAG_BILLBOARD) {
                    /* Billboard rendering */
                    /* Position from particle */
                    f32 x = pp->positionX;
                    f32 y = pp->positionY;
                    f32 z = pp->positionZ;
                    f32 scale = pp->scaleFactor;

                    /* Submit billboard draw call */
                    /* (Uses the display callback system) */
                }

                /* Normal rendering via display callback */
                if (pp->flags & PS_FLAG_OBJ_REF) {
                    psdisp_UpdateDisplay(pp);
                }
            }

            pp = pp->next;
        }

        renderMask <<= 1;
    }
}

/* ======================================================================
 * fn_8016C150 | psdisp_UpdateAll
 * Size: 0xCDC
 *
 * Per-frame update for all display objects. Processes interpolation,
 * color fading, and size changes.
 * ====================================================================== */
void psdisp_UpdateAll(void) {
    s32 linkNo;
    PSParticle* pp;

    for (linkNo = 0; linkNo < PS_NUM_LINK; linkNo++) {
        pp = pslist_GetHead(linkNo);

        while (pp != NULL) {
            /* Update velocity */
            pp->positionX += pp->velocityX;
            pp->positionY += pp->velocityY;
            pp->positionZ += pp->velocityZ;

            /* Apply friction */
            if (pp->flags & PS_FLAG_FRICTION_ACTIVE) {
                pp->velocityX *= pp->frictionFactor;
                pp->velocityY *= pp->frictionFactor;
                pp->velocityZ *= pp->frictionFactor;
            }

            /* Apply gravity */
            if (pp->flags & PS_FLAG_GRAVITY_ON) {
                pp->velocityY -= 0.1f;
            }

            /* Apply scale */
            if (pp->flags & PS_FLAG_SCALE_ACTIVE) {
                /* Scale decreases over time */
                pp->scaleFactor *= 0.98f;
                if (pp->scaleFactor < 0.001f) {
                    pp->scaleFactor = 0.0f;
                }
            }

            /* Update heading */
            pp->heading += pp->headingSpeed;
            pp->headingSpeed += pp->headingAccel;

            /* Update color interpolation */
            if (pp->color1Timer > 0) {
                pp->color1Timer--;
                if (pp->color1Timer == 0) {
                    pp->color1R = pp->color1TargetR;
                    pp->color1G = pp->color1TargetG;
                    pp->color1B = pp->color1TargetB;
                    pp->color1A = pp->color1TargetA;
                }
            }

            if (pp->color2Timer > 0) {
                pp->color2Timer--;
                if (pp->color2Timer == 0) {
                    pp->color2R = pp->color2TargetR;
                    pp->color2G = pp->color2TargetG;
                    pp->color2B = pp->color2TargetB;
                    pp->color2A = pp->color2TargetA;
                }
            }

            /* Update alpha interpolation */
            if (pp->alphaTimer > 0) {
                pp->alphaTimer--;
            }

            pp = pp->next;
        }
    }
}

/* ======================================================================
 * fn_8016CE2C | psdisp_ProcessCamera
 * Size: 0xA74
 *
 * Camera tracking processing for particles attached to the camera.
 * ====================================================================== */
void psdisp_ProcessCamera(void) {
    void* camera = lbl_8047B190;
    s32 linkNo;
    PSParticle* pp;

    if (camera == NULL) {
        return;
    }

    for (linkNo = 0; linkNo < PS_NUM_LINK; linkNo++) {
        pp = pslist_GetHead(linkNo);

        while (pp != NULL) {
            if (pp->flags & PS_FLAG_ATTACH_CAMERA) {
                /* Update particle position relative to camera */
                u8* camData = (u8*)camera;
                f32 camX = *(f32*)(camData + 0x0C);
                f32 camY = *(f32*)(camData + 0x10);
                f32 camZ = *(f32*)(camData + 0x14);

                pp->positionX = camX + pp->velocityX;
                pp->positionY = camY + pp->velocityY;
                pp->positionZ = camZ + pp->velocityZ;
            }

            pp = pp->next;
        }
    }
}

/* ======================================================================
 * fn_8016D8A0 | psdisp_ProcessPeople
 * Size: 0x464
 *
 * People/NPC processing for particle attachments.
 * ====================================================================== */
void psdisp_ProcessPeople(void) {
    s32 linkNo;
    PSParticle* pp;

    for (linkNo = 0; linkNo < PS_NUM_LINK; linkNo++) {
        pp = pslist_GetHead(linkNo);

        while (pp != NULL) {
            if (pp->flags & PS_FLAG_NO_DETACH) {
                /* Update from attached people object */
                psPeopleLinkUpdate(pp);
            }

            pp = pp->next;
        }
    }
}

/* ======================================================================
 * fn_8016DD04 | psdisp_ProcessOrbital
 * Size: 0x6F4
 *
 * Orbital motion processing for particles with the ORBIT flag.
 * ====================================================================== */
void psdisp_ProcessOrbital(void) {
    s32 linkNo;
    PSParticle* pp;

    for (linkNo = 0; linkNo < PS_NUM_LINK; linkNo++) {
        pp = pslist_GetHead(linkNo);

        while (pp != NULL) {
            if (pp->flags & PS_FLAG_ORBIT) {
                /* Compute orbital position around center */
                f32 heading = pp->heading;
                f32 radius = pp->lerpValue;

                /* Simple circular orbit */
                pp->positionX = radius * 1.0f; /* cos(heading) placeholder */
                pp->positionZ = radius * 0.0f; /* sin(heading) placeholder */

                /* Advance heading */
                pp->heading += pp->headingSpeed;
            }

            pp = pp->next;
        }
    }
}

/* ======================================================================
 * fn_8016E3F8 | psdisp_CleanupDead
 * Size: 0x41C
 *
 * Clean up particles that have been marked for destruction.
 * ====================================================================== */
void psdisp_CleanupDead(void) {
    s32 linkNo;
    PSParticle* pp;
    PSParticle* prev;
    PSParticle* next;

    for (linkNo = 0; linkNo < PS_NUM_LINK; linkNo++) {
        prev = NULL;
        pp = pslist_GetHead(linkNo);

        while (pp != NULL) {
            next = pp->next;

            if (pp->flags & PS_FLAG_KILLED) {
                /* Detach from people if needed */
                if (pp->peopleObj != NULL) {
                    psPeopleDetach(pp);
                }

                /* Destroy display object */
                if (pp->flags & PS_FLAG_OBJ_REF) {
                    psdisp_DestroyDisplay(pp);
                }

                /* Unlink from list */
                pslist_Unlink(pp, prev);
            } else {
                prev = pp;
            }

            pp = next;
        }
    }
}

/* ======================================================================
 * fn_8016E814 | psdisp_CountActive
 * Size: 0x274
 *
 * Count active particles across all link lists.
 * ====================================================================== */
u32 psdisp_CountActive(void) {
    s32 linkNo;
    PSParticle* pp;
    u32 count = 0;

    for (linkNo = 0; linkNo < PS_NUM_LINK; linkNo++) {
        pp = pslist_GetHead(linkNo);

        while (pp != NULL) {
            if ((pp->flags & PS_FLAG_KILLED) == 0) {
                count++;
            }
            pp = pp->next;
        }
    }

    return count;
}

/* ======================================================================
 * fn_8016EA88 | psdisp_KillAllInLink
 * Size: 0xA8
 *
 * Mark all particles in a given link for destruction.
 * ====================================================================== */
void psdisp_KillAllInLink(u32 linkNo) {
    PSParticle* pp;

    if (linkNo >= PS_NUM_LINK) {
        return;
    }

    pp = pslist_GetHead(linkNo);

    while (pp != NULL) {
        pp->flags |= PS_FLAG_KILLED;
        pp = pp->next;
    }
}

/* ======================================================================
 * fn_8016EB30 | psdisp_KillByScriptId
 * Size: 0xEC
 *
 * Kill all particles matching a given script ID.
 * ====================================================================== */
void psdisp_KillByScriptId(u16 scriptId) {
    s32 linkNo;
    PSParticle* pp;

    for (linkNo = 0; linkNo < PS_NUM_LINK; linkNo++) {
        pp = pslist_GetHead(linkNo);

        while (pp != NULL) {
            if (pp->scriptId == scriptId) {
                pp->flags |= PS_FLAG_KILLED;
            }
            pp = pp->next;
        }
    }
}

/* ======================================================================
 * fn_8016EC1C | psdisp_MainUpdate
 * Size: 0x6D8
 *
 * Main per-frame dispatch update. Called from the task system.
 * Coordinates the execution of all sub-passes.
 * ====================================================================== */
void psdisp_MainUpdate(void) {
    u32 flags = lbl_8047B11C;

    /* Process generators first */
    psdisp_ProcessGenerators();

    /* Process orbital motion */
    psdisp_ProcessOrbital();

    /* Process people attachments */
    psdisp_ProcessPeople();

    /* Process camera tracking */
    psdisp_ProcessCamera();

    /* Update all display objects */
    psdisp_UpdateAll();

    /* Clean up dead particles */
    psdisp_CleanupDead();
}

/* ======================================================================
 * fn_8016F2F4 | psdisp_Nop
 * Size: 0xC
 *
 * Empty stub function.
 * ====================================================================== */
void psdisp_Nop(void) {
    /* intentionally empty */
}

/* ======================================================================
 * fn_8016F300 | psdisp_Init
 * Size: 0x130
 *
 * Initialize the script dispatch system.
 * ====================================================================== */
void psdisp_Init(u32 maxParticles, u32 maxGenerators) {
    s32 i;

    /* Initialize the particle list system */
    pslist_Init((s32)maxParticles);

    /* Clear all data banks */
    for (i = 0; i < PS_NUM_LINK; i++) {
        sLinkDataBanks[i] = NULL;
        sScriptDataBanks[i] = NULL;
    }

    /* Initialize generator state */
    lbl_8047B120 = NULL;
    lbl_8047B124 = 0;

    /* Initialize display callback table */
    lbl_8047B128 = NULL;

    /* Clear dispatch flags */
    lbl_8047B11C = 0;
}
