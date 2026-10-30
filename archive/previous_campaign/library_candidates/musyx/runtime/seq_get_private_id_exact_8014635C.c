#include "dolphin/types.h"

typedef struct SEQ_INSTANCE {
    struct SEQ_INSTANCE* next;
    struct SEQ_INSTANCE* prev;
    u8 state;
    u8 index;
    u16 groupID;
    u32 publicId;
} SEQ_INSTANCE;

extern SEQ_INSTANCE* lbl_8047AF10;
extern SEQ_INSTANCE* lbl_8047AF14;

#define SND_SEQ_ERROR_ID 0xFFFFFFFFU
#define SND_SEQ_CROSSFADE_ID 0x80000000U

u32 seqGetPrivateId(u32 seqId)
{
    SEQ_INSTANCE* si;

    for (si = lbl_8047AF14; si != NULL; si = si->next) {
        if (si->publicId == (seqId & ~SND_SEQ_CROSSFADE_ID)) {
            return si->index | (seqId & SND_SEQ_CROSSFADE_ID);
        }
    }
    for (si = lbl_8047AF10; si != NULL; si = si->next) {
        if (si->publicId == (seqId & ~SND_SEQ_CROSSFADE_ID)) {
            return si->index | (seqId & SND_SEQ_CROSSFADE_ID);
        }
    }
    return SND_SEQ_ERROR_ID;
}
