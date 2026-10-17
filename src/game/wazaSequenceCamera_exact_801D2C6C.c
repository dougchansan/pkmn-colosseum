/**
 * @file wazaSequenceCamera_exact_801D2C6C.c
 * @brief battleCameraWazaGetOwner / fn_801D2C74, 0x801D2C6C - 0x801D2D28.
 *
 * Function-boundary carve of the waza camera TU (see wazaSequenceCamera.c):
 * no jump table, no pooled constant; its data is the .sbss camera state
 * (lbl_8047B3EC/F0/F4), kept extern. GC/1.3 -O4,p like the TU, no pragmas.
 */
#include "game/battle/waza_camera_stop.h"

extern u8 lbl_8047B3F4; /* waza camera enabled */

extern void battleCameraStartWaza(void* owner, void* sequence);

/* The active waza camera sequence. */
void* battleCameraWazaGetOwner(void)
{
    return lbl_8047B3EC;
}

/* Start the waza camera for owner once no camera animation is running,
 * ending the previous sequence first. */
void fn_801D2C74(void* owner)
{
    if (lbl_8047B3F4 != 0) {
        if (lbl_8047B3F0 == NULL) {
            if (lbl_8047B3EC != NULL) {
                wazaCameraStop();
            }
            battleCameraStartWaza(owner, NULL);
        }
    }
}
