/**
 * @file gs_dvd.h
 * @brief Header for src/game/gs_dvd.c (a mostly-MusyX audio-runtime unit;
 * see the 2026-07-02 note in that file). This header is not included by
 * gs_dvd.c or any other file in the tree.
 *
 * 2026-07-02 reconciliation: removed the GSDVD_CheckAndClose,
 * GSDVD_CloseHandle, GSDVD_Open, GSDVD_EmptyFunc, GSDVD_ErrorStateMachine,
 * GSDVD_ErrorCoverOpenMain and GSDVD_Init prototypes (and the
 * GSDVDHandle/GSDVDEntry typedefs and GSDVD_HANDLE_SIZE/GSDVD_ENTRY_SIZE
 * constants they relied on) - their definitions in gs_dvd.c were orphan
 * fiction and have been removed; none of these names are present in
 * symbols.txt.
 */
#ifndef GS_DVD_H
#define GS_DVD_H

#include "dolphin/types.h"

/* DVD error states */
#define GSDVD_ERROR_NONE            0
#define GSDVD_ERROR_COVEROPEN_WAIT  1
#define GSDVD_ERROR_COVER_CLOSED    2
#define GSDVD_ERROR_DISC_CHANGE     3
#define GSDVD_ERROR_FATAL           4
#define GSDVD_ERROR_RETRY           5

/* ===================================================================
 * Public API
 * =================================================================== */

/** GSfilterDraw */ void _sndCheckSndWorkALL(void);

#endif /* GS_DVD_H */
