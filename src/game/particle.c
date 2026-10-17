/**
 * @file particle.c
 * @brief HAL's particle module (sysdolphin particle.c) in the Genius
 *        Sonority fork, 0x80169034 - 0x8016A644.
 *
 * The whole translation unit, linked as one object:
 *   .text   0x80169034 - 0x8016A644  pslist.c ends at 0x80169034; psappsrt.c
 *           starts with psRemoveGeneratorAppSRT at 0x8016A644
 *   .rodata 0x80273820 - 0x802738B8  object.h's ref_INC assert ("object.h",
 *           "HSD_OBJ(o)->ref_count != HSD_OBJ_NOREF"), __FILE__
 *           "particle.c" and psInitDataBank's two panic messages; pslist.c's
 *           pool ends at 0x80273820 and psdisp.c's constant block starts at
 *           0x802738B8
 *   .data   0x8036BF80 - 0x8036BFA4  psSetGeneratorAngleRadiusScale's jump
 *           table
 *   .bss    0x804527C8 - 0x80452DE8  the per-bank tables and point JObjs
 *   .sdata2 0x8047D5B0 - 0x8047D5C0  3.0f, 0.0f, 1.0f (+ padding); psappsrt.c's
 *           pool starts at 0x8047D5C0
 * The live/peak counters and free lists in .sbss are shared with pslist.c,
 * psappsrt.c and generator.c and stay extern.
 *
 * Reference: Melee's sysdolphin/baselib/particle.c (doldecomp/melee). The
 * fork moved the link lists to pslist.c, the interpreter to psinterpret.c
 * and the application SRTs to psappsrt.c; it has 64 banks, adds the
 * generator scaling setters (psSet*Scaling, psSetGeneratorAngleRadiusScale)
 * and point-JObj helpers, and passes the particle kind as a u64 (retail
 * psGenerateParticle forwards its r5:r6 pair into psGenerateParticle0's
 * r7:r8, skipping r6, and psGenerateParticleID0 builds the pair as 0:kind).
 *
 * Compiler: the particle library flags (pslist.c, generator.c,
 * psdisptev.c): GC/1.3.2 -O4,p -inline auto,deferred -use_lmw_stmw on
 * -sdata 8 -sdata2 8 -str reuse,readonly, unit-wide, no local pragmas.
 * Deferred inlining emits the unit in reverse definition order, so the
 * functions are written from psInitDataBankLocate (highest address) to
 * psSetParticleVisibility (lowest). psKillParticle and psClearPointJObj
 * (Melee's particle API) are only ever inlined and are dead-stripped;
 * psInitDataBankLoad is psInitDataBank's static inline half, as in Melee.
 *
 * Status: every function reproduces retail and the unit links as one
 * object (replacing the former function carves and their named .sdata2
 * stand-ins). psKillParticle and psClearPointJObj are emitted but never
 * referenced, so the linker strips them as retail's did.
 *
 * psRemoveParticle (psKillAllParticle, psClearPointJObj and the bank clear
 * inlined) was the last wall: its inlined list walk coloured next/prev/p
 * r28/r26/r27 where retail has r26/r27/r28. The MWCC register replay
 * (GC/2.6, same code as GC/1.3.2) shows why: MWCC numbers inlined locals
 * breadth-first (psKillAllParticle's i/next/pp, then psClearPointJObj's i,
 * then psKillParticle's p/prev), and within one colouring level a variable
 * created earlier is coloured earlier. Retail needs next coloured after
 * p/prev and psClearPointJObj's counter before them, which no declaration,
 * loop or scope form of a psKillAllParticle-local next can give (lane U3
 * and the particle lane swept those). Reading the successor inside
 * psKillParticle and returning it makes next psKillParticle's own local,
 * created after p/prev; psRemoveParticle and the standalone
 * psKillAllParticle then both match. HAL's psKillParticle returns void
 * (Melee's psstructs.h; GNT4 keeps a live copy), so this is a title-path
 * rule exception (see docs/RULE_EXCEPTIONS.md).
 *
 * psInitDataBankLocate: an unknown bank version skips straight to the kind
 * fix-up with num, num2 and base unset. Retail does the same (the default
 * edge of its switch reaches the loop with no initialising instruction).
 */
#include "dolphin/types.h"
#include "sysdolphin/baselib/object.h"
#include "sysdolphin/baselib/psstructs.h"

extern void* memset(void* dst, int val, u32 size);
extern void fn_800060F0(const char* file, int line, const char* msg, ...); /* OSPanic */
extern void HSD_JObjUnref(HSD_JObj* jobj);                                    /* HSD_JObjUnref */

extern HSD_Particle* _psListNew(HSD_Particle* parent, u32 linkNo);
extern void _psListDelete(HSD_Particle* pp, HSD_Particle* parent);
extern HSD_Particle* _psListGetFirst(s32 linkNo);
extern s32 _psLinkInit(s32 count);
extern void _psListClear(void);
extern u16 psGetNewIDNum(void);
extern void psKillGeneratorID(s32 idnum);
extern void psKillAllGenerator(void);
extern s32 psRemoveParticleAppSRT(HSD_Particle* pp);
extern s32 psAttachParticleAppSRT(HSD_Particle* pp, HSD_psAppSRT* appsrt);
extern HSD_Particle* psInterpretParticle0(HSD_Particle* pp, HSD_Particle* prev);

HSD_Particle* psKillParticle(HSD_Particle* pp);
void psKillAllParticle(void);
void psDeletePntJObjwithParticle(HSD_Particle* pp);
void psClearPointJObj(void);

extern u16 lbl_8047B114; /* peak live particles */
extern u16 lbl_8047B11A; /* live particles */
extern HSD_Generator* lbl_8047B188; /* active generators (generator.c) */

/* Defined in reverse address order (see the file header). */
HSD_JObj* lbl_80452DC8[8];                /* point JObjs */
s32 lbl_80452CC8[PS_NUM_BANK];            /* psCmdListArray: cmd list count */
s32 lbl_80452BC8[PS_NUM_BANK];            /* texture group count */
HSD_PSCmdList** lbl_80452AC8[PS_NUM_BANK]; /* cmd lists */
HSD_PSTexGroup** lbl_804529C8[PS_NUM_BANK]; /* texture groups */
HSD_PSFormGroup** lbl_804528C8[PS_NUM_BANK]; /* form groups */
u32* lbl_804527C8[PS_NUM_BANK];           /* bank references */

#define psPointJObj lbl_80452DC8
#define psCmdListArray lbl_80452CC8
#define psTexGroupNum lbl_80452BC8
#define psCmdList lbl_80452AC8
#define psTexGroupArray lbl_804529C8
#define psFormGroupArray lbl_804528C8
#define psBankRef lbl_804527C8

void psInitDataBankLocate(s32* cmdBank, s32* texBank, s32* formBank)
{
    s32 i;
    s32 num;
    s32 num2;
    HSD_PSCmdList** base;
    s32* ptr;
    s32* group;
    s32* groups;
    u32 fi;
    HSD_PSFormGroup* fg;
    s32 j;
    s32 k;
    s32 num_groups;

    /* An unknown version skips straight to the kind fix-up below with num,
     * num2 and base unset: retail sets none of them on that path. */
    switch (*(u16*) cmdBank) {
    case 0:
        num2 = cmdBank[1];
        base = (HSD_PSCmdList**) (cmdBank + 2);
        num = 0;
        for (i = 0; i < num2; i++) {
            cmdBank[i + 2] += (s32) cmdBank;
        }
        break;
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
        num = cmdBank[1];
        num2 = cmdBank[2] + num;
        base = (HSD_PSCmdList**) (cmdBank + 3 - num);
        ptr = cmdBank;
        for (j = 0; j < cmdBank[2]; j++) {
            if (ptr[3] != 0) {
                ptr[3] += (s32) cmdBank;
            }
            ptr++;
        }
        break;
    }

    for (i = num; i < num2; i++) {
        if (base[i] != NULL) {
            base[i]->kind &= 0xF1FFFFFF;
            base[i]->kind |= 0x08000000;
        }
    }

    num_groups = texBank[0];
    group = groups = texBank + 1;
    for (k = 1; k <= num_groups; k++) {
        if (group[0] != 0) {
            group[0] += (s32) texBank;
        }
        group++;
    }

    for (k = 0; k < num_groups; k++) {
        HSD_PSTexGroup* tg = (HSD_PSTexGroup*) groups[k];

        if (tg != NULL) {
            u32 ti;

            for (ti = 0; ti < ((HSD_PSTexGroup*) groups[k])->num; ti++) {
                if (((HSD_PSTexGroup*) groups[k])->texTable[ti] != NULL) {
                    ((HSD_PSTexGroup*) groups[k])->texTable[ti] += (u32) texBank;
                }
            }
            tg = (HSD_PSTexGroup*) groups[k];
            if (tg->fmt == 8 || tg->fmt == 9 || tg->fmt == 10) {
                if (tg->palflag & 1) {
                    if (tg->texTable[tg->num] != NULL) {
                        tg->texTable[tg->num] += (u32) texBank;
                    }
                } else if (tg->palnum != 0) {
                    for (ti = tg->num;
                         ti < ((HSD_PSTexGroup*) groups[k])->num +
                                  ((HSD_PSTexGroup*) groups[k])->palnum;
                         ti++)
                    {
                        if (((HSD_PSTexGroup*) groups[k])->texTable[ti] != NULL) {
                            ((HSD_PSTexGroup*) groups[k])->texTable[ti] +=
                                (u32) texBank;
                        }
                    }
                } else {
                    for (ti = tg->num; ti < ((HSD_PSTexGroup*) groups[k])->num * 2;
                         ti++)
                    {
                        if (((HSD_PSTexGroup*) groups[k])->texTable[ti] != NULL) {
                            ((HSD_PSTexGroup*) groups[k])->texTable[ti] +=
                                (u32) texBank;
                        }
                    }
                }
            }
        }
    }

    if (formBank == NULL) {
        return;
    }
    for (i = 1; i <= num_groups; i++) {
        if (formBank[i] != 0) {

            formBank[i] += (s32) formBank;
            fg = (HSD_PSFormGroup*) formBank[i];
            for (fi = 0; fi < fg->num; fi++) {
                if (fg->formTable[fi] != NULL) {
                    fg->formTable[fi] += (u32) formBank;
                }
            }
        }
    }
}

static inline void psInitDataBankLoad(s32 bank, s32* cmdBank, s32* texBank,
                                      u32* ref, s32* formBank)
{
    if (formBank != NULL && *formBank != *texBank) {
        fn_800060F0(__FILE__, 95, "illigal form data (strange number of group)\n");
    }

    psBankRef[bank] = ref;
    psTexGroupNum[bank] = *texBank;
    psTexGroupArray[bank] = (HSD_PSTexGroup**) (texBank + 1);
    if (formBank != NULL) {
        psFormGroupArray[bank] = (HSD_PSFormGroup**) (formBank + 1);
    } else {
        psFormGroupArray[bank] = NULL;
    }

    switch (*(u16*) cmdBank) {
    case 0:
        psCmdListArray[bank] = cmdBank[1];
        psCmdList[bank] = (HSD_PSCmdList**) (cmdBank + 2);
        break;
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43: {
        s32 count = cmdBank[1];

        psCmdListArray[bank] = cmdBank[2] + count;
        psCmdList[bank] = (HSD_PSCmdList**) (cmdBank + 3 - count);
        break;
    }
    default:
        fn_800060F0(__FILE__, 125, "psInitDataBanks: unknown version\n");
    }
}

void psInitDataBank(s32 bank, s32* cmdBank, s32* texBank, u32* ref,
                    s32* formBank)
{
    if (bank < PS_NUM_BANK) {
        psInitDataBankLocate(cmdBank, texBank, formBank);
        psInitDataBankLoad(bank, cmdBank, texBank, ref, formBank);
    }
}

void psInitParticle(s32 count)
{
    u32 bank;
    s32 i;

    _psLinkInit(count);
    lbl_8047B11A = 0;
    lbl_8047B114 = 0;

    for (bank = 0; bank < PS_NUM_BANK; bank++) {
        psCmdListArray[bank] = 0;
        psTexGroupNum[bank] = 0;
        psCmdList[bank] = NULL;
        psTexGroupArray[bank] = NULL;
        psFormGroupArray[bank] = NULL;
        psBankRef[bank] = NULL;
    }
    for (i = 0; i < 8; i++) {
        psPointJObj[i] = NULL;
    }
}

void psRemoveParticle(void)
{
    s32 i;
    u32 bank;

    psKillAllParticle();
    psKillAllGenerator();
    _psListClear();

    for (bank = 0; bank < PS_NUM_BANK; bank++) {
        psCmdListArray[bank] = 0;
        psTexGroupNum[bank] = 0;
        psCmdList[bank] = NULL;
        psTexGroupArray[bank] = NULL;
        psFormGroupArray[bank] = NULL;
        psBankRef[bank] = NULL;
    }
    psClearPointJObj();
}

HSD_Particle* psGenerateParticle0(HSD_Particle* parent, s32 linkNo, s32 bank,
                                  u64 kind, u16 texGroup, u8* list, s32 life,
                                  f32 x, f32 y, f32 z, f32 vx, f32 vy, f32 vz,
                                  f32 size, f32 grav, f32 fric, s32 palflag,
                                  HSD_Generator* gp, s32 flgInterpret)
{
    HSD_Particle* pp;

    pp = _psListNew(parent, linkNo);
    if (pp == NULL) {
        return NULL;
    }

    if (gp != NULL) {
        pp->idnum = gp->idnum;
    } else {
        pp->idnum = psGetNewIDNum();
    }

    pp->appsrt = NULL;
    if (gp != NULL && gp->appsrt != NULL) {
        psAttachParticleAppSRT(pp, gp->appsrt);
    }

    pp->bank = bank;
    pp->linkNo = linkNo;
    pp->kind = kind;
    pp->texGroup = texGroup;
    pp->pos.x = x;
    pp->pos.y = y;
    pp->pos.z = z;
    pp->vel.x = vx;
    pp->vel.y = vy;
    pp->vel.z = vz;
    pp->size = size;
    pp->grav = grav;
    pp->fric = fric;
    pp->life = life + 1;
    pp->cmdList = list;
    pp->cmdMarkPtr = 0;
    pp->cmdPtr = 0;

    if (palflag != 0) {
        pp->kind |= 0x10;
    }

    pp->cmdWait = list != NULL;
    pp->poseNum = 0;
    pp->palNum = 0xFF;
    pp->primCol.a = 0xFF;
    pp->primCol.b = 0xFF;
    pp->primCol.g = 0xFF;
    pp->primCol.r = 0xFF;
    pp->envCol.a = 0;
    pp->envCol.b = 0;
    pp->envCol.g = 0;
    pp->envCol.r = 0;
    pp->envColCount = 0;
    pp->primColCount = 0;
    pp->sizeCount = 0;
    pp->envColRemain = 0;
    pp->primColRemain = 0;
    pp->aCmpMode = 0x33;
    if (((pp->kind >> 22) & 3) >= 2) {
        pp->aCmpParam1 = 0;
    } else {
        pp->aCmpParam1 = 1;
    }
    pp->aCmpParam2 = 0xFF;
    pp->aCmpRemain = 0;
    pp->aCmpCount = 0;
    pp->rotateCount = 0;
    pp->x68 = 0.0F;
    pp->rotateTarget = 0.0F;
    pp->rotate = 0.0F;
    pp->gen = gp;
    if (gp != NULL) {
        gp->numChild++;
    }

    pp->pJObjOfs = 0;
    pp->matColRemain = 0;
    pp->matColCount = 0;
    pp->matRGB = 0xFF;
    pp->matA = 0xFF;
    pp->ambColRemain = 0;
    pp->ambColCount = 0;
    pp->ambRGB = 0xFF;
    pp->ambA = 0xFF;
    pp->trail = 1.0F;

    if (flgInterpret != 0) {
        psInterpretParticle0(pp, NULL);
    }
    return pp;
}

HSD_Particle* psGenerateParticleID0(HSD_Particle* p, s32 linkNo, s32 bank,
                                    s32 id, s32 flgInterpret)
{
    HSD_PSCmdList* cl;
    HSD_PSTexGroup* tg;
    s32 palflag;

    if (linkNo >= 8) {
        return NULL;
    }
    if (bank >= PS_NUM_BANK) {
        return NULL;
    }
    if (id >= psCmdListArray[bank]) {
        return NULL;
    }
    cl = psCmdList[bank][id];
    if (cl == NULL) {
        return NULL;
    }
    tg = psTexGroupArray[bank][cl->texGroup];
    if (tg != NULL) {
        palflag = tg->palflag;
    } else {
        palflag = 0;
    }
    return psGenerateParticle0(p, linkNo, bank, cl->kind, cl->texGroup,
                               cl->cmdList, cl->life, 0.0F, 0.0F, 0.0F, cl->vx,
                               cl->vy, cl->vz, cl->size, cl->grav, cl->fric,
                               palflag, NULL, flgInterpret);
}

HSD_Particle* psGenerateParticle(s32 linkNo, s32 bank, u64 kind, u16 texGroup,
                                 u8* list, s32 life, s32 palflag, f32 x, f32 y,
                                 f32 z, f32 vx, f32 vy, f32 vz, f32 size,
                                 f32 grav, f32 fric, HSD_Generator* gp)
{
    return psGenerateParticle0(NULL, linkNo, bank, kind, texGroup, list, life, x,
                               y, z, vx, vy, vz, size, grav, fric, palflag, gp,
                               1);
}

/* RULE-EXCEPTION(title-path): changed signature of a stripped function whose
 * only evidence is register allocation - see docs/RULE_EXCEPTIONS.md.
 * HAL's psKillParticle (Melee's psstructs.h, GNT4's live copy) returns void
 * and psKillAllParticle reads pp->next itself. Here psKillParticle reads the
 * successor first and returns it. The emitted code is identical, but MWCC
 * numbers inlined locals breadth-first: a `next` local of psKillAllParticle
 * is created before psKillParticle's p/prev and outranks them in
 * psRemoveParticle's colouring (r28/r26/r27 for next/prev/p where retail has
 * r26/r27/r28). As psKillParticle's own local it is created after p and
 * prev, which gives retail's order both there and in the standalone
 * psKillAllParticle. */
HSD_Particle* psKillParticle(HSD_Particle* pp)
{
    HSD_Particle* next;
    HSD_Particle* prev;
    HSD_Particle* p;

    next = pp->next;
    prev = NULL;
    p = _psListGetFirst(pp->linkNo);
    while (p != NULL) {
        if (p == pp) {
            if (pp->gen != NULL) {
                pp->gen->numChild--;
            }
            if (pp->appsrt != NULL) {
                psRemoveParticleAppSRT(pp);
            }
            psDeletePntJObjwithParticle(p);
            _psListDelete(p, prev);
            break;
        }
        prev = p;
        p = p->next;
    }
    return next;
}

void psKillAllParticle(void)
{
    HSD_Particle* pp;
    s32 i;

    for (i = 0; i < PS_NUM_LINK; i++) {
        pp = _psListGetFirst(i);
        while (pp != NULL) {
            pp = psKillParticle(pp);
        }
    }
}

void psSetPointJObj(s32 no, HSD_JObj* jobj)
{
    if (no < 0 || no > 8) {
        return;
    }

    if (no != 0) {
        HSD_JObj** p = psPointJObj;
        HSD_JObj* old;

        p += no;
        old = *--p;
        if (old == jobj) {
            return;
        }
        if (old != NULL) {
            HSD_JObjUnref(old);
        }
        *p = jobj;
        ref_INC(jobj);
    } else {
        s32 i;

        for (i = 0; i < 8; i++) {
            if (psPointJObj[i] == jobj) {
                HSD_JObjUnref(psPointJObj[i]);
                psPointJObj[i] = NULL;
            }
        }
    }
}

void psSetPointJObjNodup(HSD_JObj* jobj, s32 no)
{
    HSD_JObj** p;
    HSD_JObj* old;
    s32 i;

    if (no < 0 || no > 8) {
        return;
    }

    i = 0;
    do {
        if (psPointJObj[i] == jobj) {
            HSD_JObjUnref(psPointJObj[i]);
            psPointJObj[i] = NULL;
        }
        i++;
    } while (i < 8);

    if (no != 0) {
        p = psPointJObj;
        p += no;
        old = *--p;
        if (old != NULL) {
            HSD_JObjUnref(old);
        }
        *p = jobj;
        ref_INC(jobj);
    }
}

void psClearPointJObj(void)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        if (psPointJObj[i] != NULL) {
            HSD_JObjUnref(psPointJObj[i]);
            psPointJObj[i] = NULL;
        }
    }
}

void psDeletePntJObjwithParticle(HSD_Particle* pp)
{
    if (pp->kind & 0x8000) {
        u32 no = (pp->kind >> 12) & 7;

        if (psPointJObj[no] != NULL) {
            HSD_JObjUnref(psPointJObj[no]);
            psPointJObj[no] = NULL;
        }
    }
}

void psKillFamily(s32 idnum, s32 linkNo)
{
    HSD_Particle* next;
    HSD_Particle* prev = NULL;
    HSD_Particle* pp;

    pp = _psListGetFirst(linkNo);
    while (pp != NULL) {
        next = pp->next;
        if (pp->idnum == (u16) idnum) {
            if (pp->gen != NULL) {
                pp->gen->numChild--;
            }
            if (pp->appsrt != NULL) {
                psRemoveParticleAppSRT(pp);
            }
            psDeletePntJObjwithParticle(pp);
            _psListDelete(pp, prev);
        } else {
            prev = pp;
        }
        pp = next;
    }
    psKillGeneratorID(idnum);
}

void psKillGeneratorChild(HSD_Generator* gp)
{
    HSD_Particle* next;
    HSD_Particle* prev = NULL;
    HSD_Particle* pp;
    u16 idnum = gp->idnum;

    pp = _psListGetFirst(gp->linkNo);
    while (pp != NULL) {
        next = pp->next;
        if (pp->idnum == idnum && pp->gen != NULL && pp->gen == gp) {
            if (pp->gen != NULL) {
                pp->gen->numChild--;
            }
            if (pp->appsrt != NULL) {
                psRemoveParticleAppSRT(pp);
            }
            psDeletePntJObjwithParticle(pp);
            _psListDelete(pp, prev);
        } else {
            prev = pp;
        }
        pp = next;
    }
}

u32 psGetGeneratorChildMaxLife(HSD_Generator* gp)
{
    u32 maxLife = gp->genLife;
    HSD_Generator* g;

    for (g = lbl_8047B188; g != NULL; g = g->next) {
        if (g->idnum == gp->idnum && g->genLife > maxLife) {
            maxLife = g->genLife;
        }
    }
    return maxLife;
}

u32 psGetParticleChildCount(HSD_Generator* gp)
{
    u32 count = 0;
    HSD_Generator* g;

    for (g = lbl_8047B188; g != NULL; g = g->next) {
        if (g->idnum == gp->idnum) {
            count += g->numChild;
        }
    }
    return count;
}

void psLinkChildGensToJObj(HSD_Generator* gp, HSD_JObj* jobj)
{
    gp->posFlags |= 1;
    gp->jobj = jobj;
}

void psUnlinkChildGensFromJObj(HSD_Generator* gp)
{
    gp->posFlags &= ~1;
}

void psSetVelocityRotationInLocal(HSD_Generator* gp, u8 enable)
{
    if (enable) {
        gp->posFlags |= 4;
    } else {
        gp->posFlags &= ~4;
    }
}

void psSetOffsetRotationInLocal(HSD_Generator* gp, u8 enable, u8 local)
{
    if (enable) {
        gp->posFlags |= 8;
        if (local) {
            gp->posFlags |= 0x10;
        } else {
            gp->posFlags &= ~0x10;
        }
    } else {
        gp->posFlags &= ~8;
        gp->posFlags &= ~0x10;
    }
}

void psSetParticleTexScaling(HSD_Generator* gp, u8 enable)
{
    if (enable) {
        gp->posFlags |= 0x20;
    } else {
        gp->posFlags &= ~0x20;
    }
}

void psSetTornadoScaling(HSD_Generator* gp, u8 radius, u8 height)
{
    if (radius) {
        gp->posFlags |= 0x40;
    } else {
        gp->posFlags &= ~0x40;
    }
    if (height) {
        gp->posFlags |= 0x80;
    } else {
        gp->posFlags &= ~0x80;
    }
}

void psSetNodeScaling(HSD_Generator* gp, u8 enable)
{
    if (enable) {
        gp->posFlags |= 0x100;
    } else {
        gp->posFlags &= ~0x100;
    }
}

void psSetRandomVelocityScaling(HSD_Generator* gp, u8 enable)
{
    if (enable) {
        gp->posFlags |= 0x200;
    } else {
        gp->posFlags &= ~0x200;
    }
}

#define GEN_CMD(gp) psCmdList[bank][idx]

void psSetGeneratorAngleRadiusScale(HSD_Generator* gp, f32* scale, u8 motion)
{
    u16 type = gp->type & 0xF;
    s32 bank = gp->bank;
    s32 idx = gp->gfxIdx;
    f32 avg = (scale[0] + scale[1] + scale[2]) / 3.0F;

    switch (type) {
    case 0:
    case 3:
    case 4:
        gp->aux.cone.minAngle = avg * GEN_CMD(gp)->param1;
        gp->aux.cone.maxAngle = avg * GEN_CMD(gp)->param2;
        break;
    case 1:
        gp->aux.line.x2 = scale[0] * GEN_CMD(gp)->param1;
        gp->aux.line.y2 = scale[1] * GEN_CMD(gp)->param2;
        gp->aux.line.z2 = scale[2] * GEN_CMD(gp)->param3;
        break;
    case 6:
    case 7:
        gp->aux.cone.minAngle = avg * GEN_CMD(gp)->param1;
        gp->aux.cone.maxAngle = avg * GEN_CMD(gp)->param2;
        gp->aux.cone.height = avg * GEN_CMD(gp)->param3;
        break;
    case 5:
        gp->aux.rect.xx = gp->aux.rect.x = scale[0] * GEN_CMD(gp)->param1;
        gp->aux.rect.yy = gp->aux.rect.y = scale[1] * GEN_CMD(gp)->param2;
        gp->aux.rect.zz = gp->aux.rect.z = scale[2] * GEN_CMD(gp)->param3;
        break;
    case 8:
        gp->aux.sphere.latRange = avg * GEN_CMD(gp)->param1;
        gp->aux.sphere.lonRange = avg * GEN_CMD(gp)->param2;
        break;
    }

    gp->radius = avg * GEN_CMD(gp)->radius;
    if (motion == 1) {
        gp->grav *= avg;
        gp->fric *= avg;
        gp->posFlags |= 0x1000;
    }
    gp->scale2.x = scale[0];
    gp->scale2.y = scale[1];
    gp->scale2.z = scale[2];
}

void psSetParticleVisibility(HSD_Generator* gp, u8 visible)
{
    HSD_Particle* pp;
    HSD_Generator* g;

    for (pp = _psListGetFirst(gp->linkNo); pp != NULL; pp = pp->next) {
        if (pp->idnum == gp->idnum) {
            if (visible) {
                pp->kind &= ~0x20000000;
            } else {
                pp->kind |= 0x20000000;
            }
        }
    }
    for (g = lbl_8047B188; g != NULL; g = g->next) {
        if (g->idnum == gp->idnum) {
            if (visible) {
                g->kind &= ~0x20000000;
            } else {
                g->kind |= 0x20000000;
            }
        }
    }
}
