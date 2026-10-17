/** Exact PDA-mail attachment callbacks, 0x8004E790 - 0x8004E9C0. */
#include "dolphin/types.h"

typedef struct PdaMailAttachState {
    s32 unused;
    s32 mailIndex;
    s32* selection;
} PdaMailAttachState;

typedef struct PdaMailAttachWindow {
    u8 pad00[0x60];
    PdaMailAttachState* state;
} PdaMailAttachWindow;

typedef struct PdaMailWindowA {
    u8 pad00;
    s8 phase;
    s8 guard;
    u8 pad03;
    s32 msgObj;
    u8 pad08[0x58];
    s32** field_0x60;
} PdaMailWindowA;

extern u8* windowGetKeyInfo(void);
extern u32 mailGetAttachFileGroup(s32 index);
extern s32 fn_8017B2CC(u32 fileHandle);
extern s32 fn_8017B448(u32 fileHandle);
extern u32 fsysFindLoadedArchive(u32 fileHandle, u32 index);
extern u32 fsysEntryGetType(void);
extern void menuButtonNormal(void* p);
extern void winSeqSetMenu(s32 ctx, s32 id);
extern f32 lbl_8047BE4C;
extern f32 lbl_8047BE50;

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 pdaMailAttachKey(PdaMailAttachWindow* window)
{
    u8* input;
    s32 total;
    s32 count;
    s32 index;
    u32 object;
    PdaMailAttachState* state;
    s32 loaded;
    s32 selection;

    state = window->state;
    input = windowGetKeyInfo();
    selection = *state->selection;
    index = state->mailIndex;
    count = mailGetAttachFileGroup(index);
    if (fn_8017B2CC(count) == 1) {
        loaded = 0;
    } else {
        loaded = 1;
    }
    if (loaded == 0) {
        count = -1;
    } else {
        object = mailGetAttachFileGroup(index);
        total = fn_8017B448(object);
        count = 0;
        index = count;
        while (index < total) {
            fsysFindLoadedArchive(object, index);
            if (fsysEntryGetType() == 9) {
                count++;
            }
            index++;
        }
    }
    if (count < 2) {
        return 0;
    }
    if ((*(u16*)(input + 6) & 8) != 0) {
        selection++;
        if (selection >= count) {
            selection = 0;
        }
    }
    if ((*(u16*)(input + 6) & 4) != 0) {
        selection--;
        if (selection < 0) {
            selection = count - 1;
        }
    }
    *state->selection = selection;
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
void fn_8004E89C(void* widget)
{
    u8* state = windowGetKeyInfo();
    if (!(*(u16*)state & 0x10)) {
        menuButtonNormal(widget);
    }
}
#pragma peephole reset

/* RULE-EXCEPTION(user-approved): local peephole control;
 * see docs/RULE_EXCEPTIONS.md. */
#pragma peephole off
s32 fn_8004E8E0(PdaMailWindowA* window)
{
    s32** field = window->field_0x60;
    switch (window->phase) {
    case 0:
        if (window->guard == 0) {
            winSeqSetMenu(0x77, 0x86);
            window->guard = 1;
        }
        break;
    case 2: {
        f32 result;
        f32 thresh = lbl_8047BE4C;
        f32 val = *(f32*)*field;
        /* RULE-EXCEPTION(user-approved): no-op copy sets FP web priority;
         * see docs/RULE_EXCEPTIONS.md. */
        val = val;
        result = val + lbl_8047BE50;
        *(f32*)*field = result;
        if (result >= thresh) {
            *(f32*)*field -= thresh;
        }
        break;
    }
    case 3:
        if (window->guard == 0) {
            winSeqSetMenu(0x77, 0x8a);
            window->guard = 1;
        }
        break;
    }
    return 0;
}
#pragma peephole reset
