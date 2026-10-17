/**
 * @file sound.c
 * @brief Sound -- two small SDA-getter accessors.
 *
 * This unit spans 0x801653BC-0x801653CC (2 functions, 0x10 bytes total),
 * squeezed between musyx_range_801652DC.c (0x801652DC-0x801653BC) and
 * gs_range_801653CC.c (0x801653CC-0x80167040) per splits.txt. It contains
 * only two trivial SDA getters, both matched at 100%:
 *   - soundGetCurrentSubSound: returns lbl_8047B0A8
 *   - fn_801653C4: returns lbl_8047B0AC
 *
 * A prior campaign-generation transplant (from
 * archive/previous_campaign/src/game/sound/{sound_bgm,sound_se}.c) had
 * attached a large orphan block to this file: a full invented JAudio2
 * sound-engine API (sndInit/sndShutdown/sndPlaySe/sndPlayBgm/sndStop/
 * sndCheckFileInfo/sndWaveOpen/... plus internal _snd* helpers), none of
 * whose names appear in config/GC6E01/symbols.txt. The claimed address
 * range for that block (0x801652DC-0x80167040) actually belongs to five
 * *other* units per splits.txt (musyx_range_801652DC.c, this file,
 * gs_range_801653CC.c, gs_dvd.c, ps_range_80168C64.c, gs_scene.c) --
 * a wrong-unit attribution. None of the 29 orphan names were referenced
 * anywhere else in src/ or include/ (including from any forbidden file),
 * so the whole block -- including two dead static helpers
 * (sndGetWaveData, _sndSeekOldBgm) that only called into the same
 * fiction -- has been deleted wholesale. include/game/sound/sound.h,
 * which existed solely to declare this fictional API and is included
 * nowhere else, has been trimmed to match.
 *
 * Address range: 0x801653BC - 0x801653CC
 */

#include "dolphin/types.h"

extern u32 lbl_8047B0A8;
extern u32 lbl_8047B0AC;

/* 0x801653BC | 0x8 | sda_getter */
u32 soundGetCurrentSubSound(void) {
    return lbl_8047B0A8;
}

/* 0x801653C4 | 0x8 | sda_getter */
u32 std228_soundGetCurrentBackgroundMusic(void) {
    return lbl_8047B0AC;
}
