/**
 * @file gs_scene.h
 * @brief GSscene -- Scene object lifecycle, XFB capture, and camera management.
 *
 * GSscene manages the scene-level object hierarchy:
 *   - Linked-list object pools (active/free lists)
 *   - Object spawn/despawn lifecycle
 *   - Parent-child attachment
 *   - Transform hierarchy (position, rotation, scale)
 *   - Scene camera with interpolation
 *   - XFB (framebuffer) capture for debug screenshots
 *   - Scene rendering dispatch
 *
 * Object lists stored in BSS:
 *   - Active list head: lbl_8047B188
 *   - Free list head:   lbl_8047B18C
 *   - Object count:     lbl_8047B118 (u16)
 *   - Camera state:     lbl_80478C40
 *
 * Debug strings:
 *   "gs%04d.xfb"
 *
 * Address range: 0x8017572C - 0x8017A5FC (22KB, 78 functions)
 *
 * Eleven addresses previously declared here under invented GSscene_*
 * names collided with real, differently-named symbols.txt entries
 * (psKillAllGenerator, psRemoveGenerator, psGetNewIDNum) or are still
 * unnamed fn_ scaffolds; those declarations have been corrected/removed
 * below (see gs_scene.c's header comment for the full mapping). The
 * genuinely-matched GSscene_* declarations (camera vectors, mode) are
 * real and unchanged.
 */
#ifndef GS_SCENE_H
#define GS_SCENE_H

#include "game/gs_scene_types.h"

/* ===================================================================
 * Public API
 * =================================================================== */

/** real name (symbols.txt) */ void   psKillAllGenerator(void);
/** real name (symbols.txt) */ void*  psRemoveGenerator(u32 type, u32 param);
/** real name (symbols.txt) */ u32    psGetNewIDNum(void);
/** unnamed scaffold */ void   fn_80177A64(u32 captureIndex);
/** unnamed scaffold */ void   fn_80178AA8(void* sceneObj);
/** unnamed scaffold */ void   fn_80179020(void* camera);
/** 0x801778B4 */ void   GSscene_GetCameraDirectionVector(GSSceneVec3* dst);
/** 0x801778DC */ void   GSscene_SetCameraDirectionVector(GSSceneVec3* src);
/** 0x80177830 */ void   GSscene_GetCameraRotationVector(GSSceneVec3* dst);
/** 0x80177858 */ void   GSscene_SetCameraRotationVector(GSSceneVec3* src);
/** 0x80177908 */ void   GSscene_GetCameraPositionVector(GSSceneVec3* dst);
/** 0x80177930 */ void   GSscene_SetCameraPositionVector(GSSceneVec3* src);
/** 0x8017795C */ void   GSscene_GetCameraViewVector(GSSceneVec3* dst);
/** 0x80177984 */ void   GSscene_SetCameraViewVector(GSSceneVec3* src);
/** 0x80177A38 */ u32    GSscene_GetMode(void);
/** 0x80177A44 */ u32    GSscene_SetMode(u32 mode);
/** unnamed scaffold */ void   fsysInitTOC(void);
/** 0x801766A8 */ void   cameraSetFov(f32 fov);
/** 0x80176758 */ void   cameraSetRotY(f32 angle);
/** 0x801767E0 */ void   cameraSetDistance(f32 distance);
/** 0x80176868 */ void   cameraSetHeight(f32 height);
/** 0x80176948 */ void   std040_cameraSetPosition(f32 x, f32 y, f32 z);
/** 0x8017697C */ void   cameraSetTargetPosXYZ(f32 x, f32 y, f32 z);
/** 0x801769B0 */ void   cameraSetTargetOfsXYZ(f32 x, f32 y, f32 z);
/** 0x80176B48 */ s32    cameraWaitSyncAnime(s32 sync);
/** 0x80176F98 */ u32    cameraMoveEndCheckSpecial(u8 wait);
/** 0x80177478 */ void   cameraMoveRotation(void* unused,
                                            GSSceneVec3* rotation,
                                            f32 duration);
/** 0x801774F0 */ void   cameraMovePositionXYZ(f32 x, f32 y, f32 z,
                                               f32 duration);
/** 0x80177574 */ void   cameraMovePosition(void* unused,
                                            GSSceneVec3* position,
                                            f32 duration);
/** 0x801775EC */ void   cameraMoveTargetXYZ(f32 x, f32 y, f32 z,
                                             f32 duration);
/** 0x80177670 */ void   cameraMoveTargetOfs(void* unused,
                                             GSSceneVec3* offset,
                                             f32 duration);
/** 0x801776E8 */ void   cameraMoveTargetPos(void* unused,
                                             GSSceneVec3* target,
                                             f32 duration);
/** 0x80177760 */ void   cameraMoveTarget(void* unused, u32 group, u32 id,
                                          f32 duration);

#endif /* GS_SCENE_H */
