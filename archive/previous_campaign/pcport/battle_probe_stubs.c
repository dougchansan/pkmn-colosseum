#include "dolphin/types.h"

#include <stdarg.h>

void OSReport(const char* fmt, ...) {
    (void)fmt;
}

void* HSD_DObjLoadDesc(void* desc) {
    (void)desc;
    return NULL;
}

void HSD_DObjAnimAll(void* dobj) {
    (void)dobj;
}

void HSD_DObjReqAnimAll(void* dobj, f32 frame) {
    (void)dobj;
    (void)frame;
}

void HSD_DObjAddAnimAll(void* dobj, void* matanim, void* shapeanim) {
    (void)dobj;
    (void)matanim;
    (void)shapeanim;
}

void HSD_DObjAnim(void* dobj) {
    (void)dobj;
}

void HSD_DObjRemoveAll(void* dobj) {
    (void)dobj;
}
