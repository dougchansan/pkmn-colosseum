/*
 * Score-driven instrumentation partition before C_MTXLookAt.
 * This is not evidence of an original translation-unit boundary.
 */
#define SDK_MTXLOOKAT_EXCLUDE
#define SDK_MTXLOOKAT_AFTER_EXCLUDE
#include "src/dolphin/sdk_range_800A2D38.c"
