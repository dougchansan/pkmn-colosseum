#include "game/ps_types.h"

extern PSAppSRT* lbl_8047B124;
extern u16 lbl_8047B120;
extern u16 lbl_8047B116;
extern u16 lbl_8047B110;
extern const f32 lbl_8047D5C0;
extern const f32 lbl_8047D5C4;
extern void* HSD_MemAlloc(s32 size);
extern void* memset(void* destination, s32 value, u32 size);

PSAppSRT* psAddGeneratorAppSRT(PSGeneratorState* generator, u8 type)
{
    PSAppSRT* appSRT;
    u16 familyId = generator->familyId;

    if (lbl_8047B124 == NULL) {
        lbl_8047B124 = HSD_MemAlloc(lbl_8047B120);
        if (lbl_8047B124 != NULL) {
            memset(lbl_8047B124, 0, lbl_8047B120);
        }
    }

    appSRT = lbl_8047B124;
    if (appSRT != NULL) {
        u16 count;

        lbl_8047B124 = appSRT->next;
        appSRT->next = NULL;
        appSRT->refCount = 1;
        appSRT->flags = 0;
        appSRT->type = type;
        appSRT->rotationX = appSRT->rotationY = appSRT->rotationZ = lbl_8047D5C0;
        appSRT->translationX = appSRT->translationY = appSRT->translationZ = lbl_8047D5C0;
        appSRT->scaleX = appSRT->scaleY = appSRT->scaleZ = lbl_8047D5C4;
        appSRT->destroy = NULL;
        appSRT->active = 0;
        appSRT->owner = NULL;
        appSRT->familyId = familyId;

        count = lbl_8047B116 + 1;
        lbl_8047B116 = count;
        if (count > lbl_8047B110) {
            lbl_8047B110 = count;
        }
    }

    generator->appSRT = appSRT;
    return appSRT;
}
