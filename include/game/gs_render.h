/**
 * @file gs_render.h
 * @brief GSrender -- Extended GSgfx rendering pipeline, materials, and GX wrapper.
 *
 * This header covers the rendering subsystems between the SDK code and GSmem:
 *   - GSgfx VBlank/retrace callbacks
 *   - GSlog debug printing (varargs printf with hex)
 *   - GSgfx matrix stack operations
 *   - GSgfx viewport/projection/scissor setup
 *   - GSgfx blend/TEV/fog configuration
 *   - GSgfx lighting system
 *   - GSmaterial system (MObj-based materials for HSD)
 *   - GSgfx draw command dispatch
 *
 * Debug strings:
 *   "GSgfx: invalid matrix index"
 *   "GSgfx: matrix stack underflow!"
 *   "GSgfx: matrix stack overflow!"
 *   "GSmaterialSetPEdescr: Warning: already using a custom description!"
 *   "GSmaterialCreate: Run out of materials..."
 *   "GSmaterial MObj"
 *   "GSmaterial: Unsupported texture format for environment map!"
 *   "GSmaterial: Error creating environment map: no texture defined!"
 *
 * Address range: 0x800D3E4C - 0x800E202C (56KB, 278 functions)
 */
#ifndef GS_RENDER_H
#define GS_RENDER_H

#include "dolphin/types.h"

/* ===================================================================
 * Constants
 * =================================================================== */

/** GSgfx state structure size (saved/restored on VBlank) */
#define GSGFX_STATE_SIZE        0x5A0

/** Maximum matrix stack depth */
#define GSGFX_MAX_MTX_DEPTH     32

/** Maximum number of GX lights */
#define GSGFX_MAX_LIGHTS        8

/* ===================================================================
 * GSmaterial system constants
 * =================================================================== */

/** Maximum number of active materials (capacity checked by GSmaterial_Create) */
#define GSMATERIAL_MAX_COUNT    128

/* ===================================================================
 * Sub-module address ranges
 * =================================================================== */

/*
 * 0x800D3E4C - 0x800D4610:  GSgfx callbacks (VBlank, retrace, draw-done)
 * 0x800D461C - 0x800D5504:  GSlog debug output
 * 0x800D5504 - 0x800D7900:  GSgfx matrix/transform (~80 functions)
 * 0x800D7900 - 0x800D9F40:  GSgfx viewport/projection
 * 0x800D9F40 - 0x800DA2BC:  GSgfx blend/TEV/fog
 * 0x800DB890 - 0x800DC224:  GSgfx lighting
 * 0x800DC224 - 0x800DE680:  GSmaterial system
 * 0x800DE680 - 0x800E202C:  GSgfx draw commands
 */

/* ===================================================================
 * Public API -- GSgfx callbacks
 * =================================================================== */

/** _gfxScratchNotify__F15GSscratchNotifyPvUc */ void GSgfx_VBlankCallback(s32 mode, void* buffer);
/** GSgfxSetViewportJitter */ void GSgfx_PreRetraceCallback(s32 flag, f32 p1, f32 p2,
                                                   f32 p3, f32 p4, f32 p5, f32 p6);
/** fn_800D3F50 */ void GSgfx_DrawDoneCallback(void);
/** fn_800D3F5C */ void GSgfx_FrameEndCallback(void);
/** GSgfxDrawScene */ void GSgfx_BeginFrame(void);

/* ===================================================================
 * Public API -- GSlog
 * =================================================================== */

/** fn_800D461C */ void GSlog_PrintFormatted(u32 channel, u32 paramCount, ...);
/** fn_800D4F98 */ void GSlog_QueueCommand(u32 opcode, u32 paramCount, ...);

/* ===================================================================
 * Public API -- GSgfx pipeline
 * =================================================================== */

/** fn_800D892C */ void GSgfx_ConfigurePipeline(void);

/* ===================================================================
 * Public API -- GS math
 * =================================================================== */

struct GSvec;

/** GSvecAdd */ void GSvecAdd(struct GSvec* dst, const struct GSvec* lhs,
                             const struct GSvec* rhs);
void GSlerpGetLinearInterpolationVector(struct GSvec* out,
                                        const struct GSvec* start,
                                        const struct GSvec* end, f32 t);

/* ===================================================================
 * Public API -- GSmaterial
 * =================================================================== */

/** logVsnprintf_float */ void GSmaterial_Create(void* params);

/* ===================================================================
 * Public API -- Draw
 * =================================================================== */

/** GSmemOptimize */ void GSgfx_DrawDispatch(void* drawList);

#endif /* GS_RENDER_H */
