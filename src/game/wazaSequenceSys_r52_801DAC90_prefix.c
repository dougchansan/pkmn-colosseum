/* Waza system teardown, 0x801DAC90-0x801DADC0. No owned data or literals. */
#include "game/battle/battle_waza_types.h"

extern u16 lbl_80467CD4[16];
extern void fn_800E24B0(u16 handle);
extern void GSmemFree(u16 handle);
extern void fn_801D2F94(void);

/* Retail expands wazaSequenceOwnerFree's 0x54-byte entry-clear body in this loop. */
static inline void wazaSequenceClearEntry(WazaEffect* entry)
{
    if (entry != NULL) {
        if (entry->active != 0) {
            fn_801DD3E4(entry);
            wazaSequenceOwnerRelease(entry);
        }
        memset(entry, 0, sizeof(*entry));
    }
}

/* RULE-EXCEPTION(title-path): single-use inline helper retains the resource
 * loop's zero-copy/register layout; see docs/RULE_EXCEPTIONS.md. */
static inline void wazaSequenceReleaseResources(void)
{
    s32 i = 0;
    u16* resource = lbl_80467CD4;
    do {
        if (*resource != 0) {
            fn_800F915C(*resource);
            GSresReleaseById(*resource);
            *resource = 0;
        }
        i++;
        resource++;
    } while (i < 16);
}

void wazaSequenceSysRelease(void)
{
    u8* pool = lbl_80467CC0;
    s32 handle = *(u16*)(pool + 0x10);
    WazaEffect* entry = *(WazaEffect**)pool;
    s32 i = 0;
    s32 count = *(u16*)(pool + 4);

    for (; i < count; i++, entry++) {
        wazaSequenceClearEntry(entry);
    }
    fn_800E24B0(handle);
    GSmemFree(handle);
    memset(lbl_80467CC0, 0, 0x14);
    wazaSequenceReleaseResources();
    fn_801D2F94();
    lbl_8047B414 = 0;
    lbl_8047B418 = 0;
    if (lbl_80467CC0[6] == 0) {
        fn_800F915C(0x6F7);
        GSresReleaseById(0x6F7);
    }
}
