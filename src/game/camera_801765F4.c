/**
 * Linked camera unit 0x801765F4 - 0x80179DFC (battleCameraSetFlag3 .. cameraInit)
 * with its .rodata initialiser image (0x80273D98-0x80273DC8), its .data
 * (the up vector and cameraUpdate's two switch tables, 0x8036C248-0x8036C29C)
 * and its whole .sdata2 pool (0x8047D720-0x8047D790), compiled from the
 * whole-TU source camera.c. The save-state handlers after it link from
 * camera_exact_80179DFC.c and camera_candidate_80179E04.c.
 */
#define CAMERA_801765F4_ONLY
#include "src/game/camera.c"
