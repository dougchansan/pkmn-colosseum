/**
 * @file sound.h
 * @brief Sound -- placeholder header for game/sound/sound.c.
 *
 * sound.c's real unit is two trivial SDA-getter accessors at
 * 0x801653BC-0x801653CC (soundGetCurrentSubSound, fn_801653C4); they need no
 * declarations here and are defined directly in the .c file.
 *
 * This header previously declared a large invented JAudio2 sound-engine
 * API (sndInit/sndShutdown/sndPlaySe/sndPlayBgm/sndStop/sndCheckFileInfo/
 * sndWaveOpen/... plus SndWork/SndResData/SndVec/SndListener structs and
 * internal _snd* helpers) carried over from a prior campaign-generation
 * transplant. None of those names appear in config/GC6E01/symbols.txt,
 * and this header was included nowhere but sound.c itself, so the
 * fictional declarations have been removed; see sound.c for the full
 * reconciliation note.
 */
#ifndef GAME_SOUND_H
#define GAME_SOUND_H

#include "dolphin/types.h"

#endif /* GAME_SOUND_H */
