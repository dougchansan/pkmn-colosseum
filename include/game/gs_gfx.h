/**
 * @file gs_gfx.h
 * @brief GSgfx -- Genius Sonority graphics subsystem for Pokemon Colosseum.
 *
 * The GSgfxState layout below documents the 0x5A0-byte state structure
 * referenced by the gs_gfx.c functions through lbl_8047AA80.
 *
 * This header previously also declared a set of friendly-named wrapper
 * functions (GSgfxInit, GSgfxSetVideoMode, GSgfxEnableRendering,
 * GSgfxSwapBuffers, GSgfxSetDrawMode, GSgfxGetFrameCount,
 * GSgfxGetTickCount) whose bodies lived in gs_gfx.c behind a dead
 * `#ifdef PCPORT` (PCPORT is never defined anywhere in this build). None
 * of those names appear in config/GC6E01/symbols.txt; the prototypes have
 * been removed along with the dead definitions.
 *
 * Address range: 0x800D3074 - 0x800D3E4C (approx.)
 */
#ifndef GS_GFX_H
#define GS_GFX_H

#include "dolphin/types.h"

/* -----------------------------------------------------------------------
 * GSgfx global state structure -- 0x5A0 bytes.
 * Allocated from GSmem, pointer stored at lbl_8047AA80.
 * ----------------------------------------------------------------------- */
typedef struct GSgfxState {
    /* 0x000 */ s32  mode;           /* rendering mode (0=off, 1=init, 2=active) */
    /* 0x004 */ s32  frameCounter;   /* current frame number (-1 = uninit) */
    /* 0x008 */ u32  fifoSize;       /* GX command FIFO entry count */
    /* 0x00C */ void* renderTarget;  /* texture receiving the current frame */
    /* 0x010 */ u32  field_10;
    /* 0x014 */ u32  drawFlags;      /* GX draw control flags */
    /* 0x018 */ u8   matrixDirty;    /* set when model-view needs reload */
    /* 0x019 */ u8   projDirty;
    /* 0x01A */ u8   viewportDirty;
    /* 0x01B */ u8   scissorDirty;
    /* 0x01C */ u32  pad_01C;
    /* 0x020 */ u32  lightMask;      /* bitmask of active lights */
    /* 0x024-0x044 */ u32 reserved[9]; /* matrix state, counters, etc. */
    /* 0x048 */ u32  xfbIndex;       /* current XFB being displayed */
    /* 0x04C */ u32  xfbCount;
    /* 0x050 */ u32  xfbAddr0;
    /* 0x054 */ u32  frameDelta;     /* retraces elapsed since the prior frame */
    /* 0x058 */ u32  renderEnabled;  /* 1 = rendering pipeline active */
    /* 0x05C */ u8   vsyncFlag;
    /* 0x05D-0x47D */ u8 pad[0x421];
    /* 0x47E */ u8   gammaMode;
    /* 0x47F */ u8   pad_47F;
    /* 0x480 */ u32  field_480;
    /* 0x484 */ u32  field_484;
    /* 0x488 */ s32  prevMode;       /* previous rendering mode before switch */
    /* 0x48C-0x49B */ u8 pad2[0x10];
    /* 0x49C */ u8   interlaceMode;
    /* 0x49D */ u8   progressiveFlag;
    /* 0x49E */ u8   pad_49E;
    /* 0x49F */ u8   field_49F;
    /* 0x4A0-0x59F */ u8 pad3[0x100];
} GSgfxState;

extern GSgfxState* lbl_8047AA80;

u32 GSgetFrameStep(void);

/* -----------------------------------------------------------------------
 * TV standards taken by GSgfxInit / fn_800D37D4; each selects one row of
 * render modes (the SDK GXNtsc*, GXPal*, GXEurgb60Hz* and GXMpal* objects).
 * ----------------------------------------------------------------------- */
#define GSGFX_VMODE_NTSC       1
#define GSGFX_VMODE_PAL50      2
#define GSGFX_VMODE_PAL60      3 /* EURGB60 */
#define GSGFX_VMODE_MPAL       4

#endif /* GS_GFX_H */
