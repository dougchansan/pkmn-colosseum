/**
 * @file movie.h
 * @brief THP movie playback system for Pokemon Colosseum.
 *
 * The movie system handles playback of THP (Nintendo's video format) files
 * for opening cinematics, logo screens, credits, auto-demo sequences, etc.
 *
 * THP files referenced in rodata:
 *   "movie/openingdemo.thp"  -- Opening cinematic
 *   "movie/staffroll.thp"    -- Credits / staff roll
 *   "movie/autodemo01.thp"   -- Auto-demo / attract mode
 *   "movie/gs_logo.thp"      -- Genius Sonority logo
 *   "movie/tpc.thp"          -- The Pokemon Company logo
 *
 * The movie translation unit (src/game/movie.c) spans 0x80035DD4 -
 * 0x80037158: the movie scene callbacks, the boot entry bootMain and
 * the boot loader fn_8003686C with its two threads. They call into the THP
 * player library (fn_801E1874 for THPPlayerGetState, fn_801E189C for THPPlayerOpen, etc.)
 * and coordinate with the sound system (fn_80165A20) and flag system
 * (fn_801902E0, fn_80190528) for proper sequencing.
 *
 * openingDemoPlay, fn_80035EE4, staffrollFinish, and staffrollPlay are the real,
 * address/size-confirmed movie stop/cleanup, opening-demo, auto-demo, and
 * staff-roll functions (defined directly in movie.c under those fn_ names,
 * matching config/GC6E01/symbols.txt); nothing else in the tree calls them
 * yet, so no prototypes are declared here. A prior campaign transplant's
 * invented movieWaitForFinish/moviePlayGSLogo/moviePlayTPCLogo helpers had
 * no real callers and have been removed.
 *
 * Address range: 0x80035DD4 - 0x80037158
 */
#ifndef GAME_MOVIE_H
#define GAME_MOVIE_H

#include "dolphin/types.h"

/* ===================================================================
 * Movie ID constants
 *
 * These are derived from the order in which the movie paths appear
 * in the rodata and from the calling conventions in the game code.
 * =================================================================== */
#define MOVIE_ID_OPENING_DEMO   0   /* movie/openingdemo.thp */
#define MOVIE_ID_STAFF_ROLL     1   /* movie/staffroll.thp */
#define MOVIE_ID_AUTO_DEMO      2   /* movie/autodemo01.thp */
#define MOVIE_ID_GS_LOGO        3   /* movie/gs_logo.thp */
#define MOVIE_ID_TPC_LOGO       4   /* movie/tpc.thp */

/* ===================================================================
 * THP player state constants (returned by THPPlayerGetState)
 * =================================================================== */
#define THP_STATE_IDLE       0   /* player not active */
#define THP_STATE_PLAYING    1   /* currently playing a movie */
#define THP_STATE_STOPPED    2   /* playback finished or stopped */
#define THP_STATE_ERROR      3   /* an error occurred */

#endif /* GAME_MOVIE_H */
