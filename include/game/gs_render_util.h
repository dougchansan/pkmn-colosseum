#ifndef GAME_GS_RENDER_UTIL_H
#define GAME_GS_RENDER_UTIL_H

#include "dolphin/types.h"

/* ===================================================================
 * gs_render_util layout structs
 *
 * These describe the render-object ("camera") array element used by
 * gs_render_util.c (lbl_8047AA6C, stride 0x128), the compact snapshot
 * buffer used to save/restore camera state (GScameraRestoreState/GScameraSaveState),
 * and the small global render-state block pointed to by lbl_8047AA80.
 * =================================================================== */

typedef struct GSRenderVec3 {
    f32 x;
    f32 y;
    f32 z;
} GSRenderVec3;

typedef f32 GSRenderMtx[3][4];

typedef struct GSRenderCameraDesc {
    void* cobjDesc;
    void** animations;
} GSRenderCameraDesc;

/* Same layout as HSD_WObjDesc. */
typedef struct GSRenderWObjDesc {
    /* 0x00 */ char* className;
    /* 0x04 */ GSRenderVec3 pos;
    /* 0x10 */ void* robjDesc;
} GSRenderWObjDesc; /* size 0x14 */

/* Same layout as HSD_CameraDescPerspective. */
typedef struct GSRenderCObjDesc {
    /* 0x00 */ char* className;
    /* 0x04 */ u16 flags;
    /* 0x06 */ u16 projectionType;
    /* 0x08 */ u16 viewportLeft;
    /* 0x0A */ u16 viewportRight;
    /* 0x0C */ u16 viewportTop;
    /* 0x0E */ u16 viewportBottom;
    /* 0x10 */ u16 scissorLeft;
    /* 0x12 */ u16 scissorRight;
    /* 0x14 */ u16 scissorTop;
    /* 0x16 */ u16 scissorBottom;
    /* 0x18 */ GSRenderWObjDesc* eyeDesc;
    /* 0x1C */ GSRenderWObjDesc* interestDesc;
    /* 0x20 */ f32 roll;
    /* 0x24 */ GSRenderVec3* upVector;
    /* 0x28 */ f32 nearZ;
    /* 0x2C */ f32 farZ;
    /* 0x30 */ f32 fov;
    /* 0x34 */ f32 aspect;
} GSRenderCObjDesc; /* size 0x38 */

typedef struct GSRenderCamera {
    /* 0x00 */ u8 active;
    /* 0x01 */ u8 useLookAt;
    /* 0x02 */ u8 dirty;
    /* 0x03 */ u8 hasAnimation;
    /* 0x04 */ u8 isAnimating;
    /* 0x05 */ u8 pad_05[3];
    /* 0x08 */ GSRenderCameraDesc* desc;
    /* 0x0C */ void* cobj;
    /* 0x10 */ GSRenderWObjDesc eyeDesc;      /* default camera descriptors */
    /* 0x24 */ GSRenderWObjDesc interestDesc;
    /* 0x38 */ GSRenderCObjDesc cobjDesc;
    /* 0x70 */ GSRenderVec3 eye;
    /* 0x7C */ GSRenderVec3 prevEye;
    /* 0x88 */ GSRenderVec3 rotation;
    /* 0x94 */ GSRenderMtx viewMtx;
    /* 0xC4 */ GSRenderMtx projectionMtx;
    /* 0xF4 */ GSRenderVec3 upVector;
    /* 0x100 */ GSRenderVec3 interest;
    /* 0x10C */ s32 animMode;
    /* 0x110 */ u32 animCount;
    /* 0x114 */ u32 animIndex;
    /* 0x118 */ f32 animRate;
    /* 0x11C */ f32 animFrame;
    /* 0x120 */ f32 animEndFrame;
    /* 0x124 */ s8 animEnded;
    /* 0x125 */ s8 animDirection;
    /* 0x126 */ u8 pad_126[2];
} GSRenderCamera; /* size 0x128 */

/* Compact save/restore snapshot for a GSRenderCamera (see GScameraRestoreState,
 * GScameraSaveState). Same field semantics as GSRenderCamera but packed
 * without the leading state-flag bytes / cobj pointers. */
typedef struct GSRenderCameraSnapshot {
    /* 0x00 */ u8 isAnimating;
    /* 0x01 */ u8 animEnded;
    /* 0x02 */ u8 pad_02[2];
    /* 0x04 */ GSRenderVec3 eye;
    /* 0x10 */ GSRenderVec3 prevEye;
    /* 0x1C */ GSRenderVec3 rotation;
    /* 0x28 */ GSRenderMtx viewMtx;
    /* 0x58 */ GSRenderMtx projectionMtx;
    /* 0x88 */ GSRenderVec3 upVector;
    /* 0x94 */ GSRenderVec3 interest;
    /* 0xA0 */ s32 animMode;
    /* 0xA4 */ u32 animIndex;
    /* 0xA8 */ f32 animRate;
    /* 0xAC */ f32 animFrame;
} GSRenderCameraSnapshot; /* size 0xB0 */

/* Global render-state block pointed to by lbl_8047AA80. Only the
 * fields actually touched by gs_render_util.c are named; everything
 * else is left as padding since this struct is never iterated/strided. */
typedef struct GSRenderState {
    /* 0x00 */ u8 unk_00[0x19];
    /* 0x19 */ u8 fogEnabled;
    /* 0x1A */ u8 pad_1A[2];
    /* 0x1C */ u8 fogColorR;
    /* 0x1D */ u8 fogColorG;
    /* 0x1E */ u8 fogColorB;
    /* 0x1F */ u8 fogColorA;
    /* 0x20 */ u8 unk_20[0x38];
    /* 0x58 */ u32 renderWidth;
    /* 0x5C */ u8 frameLevel;
} GSRenderState;

void GScameraGetDistanceVector(GSRenderCamera* camera, GSRenderVec3* dest);
void GScameraGetPerspective(GSRenderCamera* camera, f32* fov, f32* aspect,
                            f32* near, f32* far);
void GScameraSetPerspective(GSRenderCamera* camera, f32 fov, f32 aspect,
                            f32 near, f32 far);
void GScameraGetLookAt(GSRenderCamera* camera, GSRenderVec3* up,
                       GSRenderVec3* interest);
void GScameraLookAt(GSRenderCamera* camera, const GSRenderVec3* up,
                    const GSRenderVec3* interest);
void GScameraGetRotation(GSRenderCamera* camera, GSRenderVec3* rotation);
void GScameraGetPosition(GSRenderCamera* camera, GSRenderVec3* position);
void GScameraSetRotation(GSRenderCamera* camera,
                         const GSRenderVec3* rotation);

#endif /* GAME_GS_RENDER_UTIL_H */
