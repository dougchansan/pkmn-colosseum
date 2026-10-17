/**
 * @file gs_range_exact_8010A420.c
 * @brief menuModelTerm, 0x8010A420 - 0x8010A5BC.
 *
 * Function-boundary carve of gs_range_80109C88.c. Text only; state and
 * resource globals remain extern-owned by the range data objects.
 */
#include "dolphin/types.h"

/* 0x8010A420 | 0x19C */
/* RULE-EXCEPTION(user-approved): local peephole control -- see docs/RULE_EXCEPTIONS.md. */
s32 menuModelTerm(void* obj)
{
    typedef struct Obj {
        u8 state;
        u8 flag1;
        u8 pad2[0x12];
        u8 flag14;
        u8 pad15[0xF];
        u32 handle24;
        u32 taskHandle;
        u8 pad2C[8];
        void* texture;
        void* camera;
        void* lights[3];
    } Obj;
    extern s32 lbl_8047AD40;
    extern u8 lbl_8047AD44;
    extern char lbl_80271F80[];
    extern char lbl_8035B458[];
    extern void GSlogWrite(const char* fmt, ...);
    extern u8 GSthreadIsRunning(u32 task);
    extern void GSthreadClose(u32 task);
    extern void _threadSwitch(void);
    extern void wazaSequenceOwnerFree(u32 handle);
    extern void GSmodelFree(void* a);
    extern void GSlightFree(void* a);
    extern void GScameraFree(void* camera);
    extern void GStextureFree(void* a);
    extern int wazaSequenceSysRelease(void);

    s32 i;
    u32 temp;
    Obj* o = (Obj*)obj;

    if (o == NULL) {
        return 0;
    }
    if (lbl_8047AD40 <= 0) {
        GSlogWrite(lbl_80271F80, lbl_8035B458);
        return 0;
    }

    o->flag1 = 0;
    for (;;) {
        if (o->taskHandle == 0) {
            break;
        }
        temp = GSthreadIsRunning(o->taskHandle);
        if (temp == 0) {
            GSthreadClose(o->taskHandle);
            break;
        }
        _threadSwitch();
    }

    if (o != NULL) {
        o->flag1 = 0;
        if (o->flag14) {
            if (o->handle24 != 0) {
                wazaSequenceOwnerFree(o->handle24);
                o->handle24 = 0;
            }
        } else if (o->handle24 != 0) {
            GSmodelFree((void*)o->handle24);
            o->handle24 = 0;
        }
        o->state = 0;
    }

    for (i = 0; i < 3; i++) {
        if (o->lights[i] != NULL) {
            GSlightFree(o->lights[i]);
            o->lights[i] = NULL;
        }
    }

    if (o->camera != NULL) {
        GScameraFree(o->camera);
        o->camera = NULL;
    }

    if (o->texture != NULL) {
        GStextureFree(o->texture);
        o->texture = NULL;
    }
    lbl_8047AD40 = lbl_8047AD40 - 1;
    if (lbl_8047AD40 == 0 && lbl_8047AD44) {
        wazaSequenceSysRelease();
        lbl_8047AD44 = 0;
    }

    return 1;
}
