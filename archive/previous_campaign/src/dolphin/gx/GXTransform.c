#include "dolphin/gx/GX.h"
#include "dolphin/os/OS.h"

/*
 * GXTransform.c - GX matrix and transform functions.
 *
 * Contains functions for loading position/normal/texture matrices
 * into the XF (Transform) unit, and projection matrix setup.
 *
 * Part of the 0x800B5E8C - 0x800BE348 GX block.
 */

/* WGPIPE for direct command writes */
#define WGPIPE_U8  (*(volatile u8*)0xCC008000)
#define WGPIPE_U32 (*(volatile u32*)0xCC008000)
#define WGPIPE_F32 (*(volatile f32*)0xCC008000)

/*
 * GXLoadPosMtxImm - Load a position matrix into the XF.
 */
void GXLoadPosMtxImm(f32 mtx[3][4], u32 id) {
    u32 addr;
    u32 i, j;

    addr = id * 4;

    /* Write XF load command: 0x10, count-1, addr */
    WGPIPE_U8 = 0x10;
    WGPIPE_U32 = 0x000B;  /* 12-1 values */
    WGPIPE_U32 = addr;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 4; j++) {
            WGPIPE_F32 = mtx[i][j];
        }
    }
}

/*
 * GXLoadNrmMtxImm - Load a normal matrix into the XF.
 */
void GXLoadNrmMtxImm(f32 mtx[3][4], u32 id) {
    u32 addr;
    u32 i;

    addr = 0x400 + id * 3;

    /* Write XF load command */
    WGPIPE_U8 = 0x10;
    WGPIPE_U32 = 0x0008;  /* 9-1 values */
    WGPIPE_U32 = addr;

    for (i = 0; i < 3; i++) {
        WGPIPE_F32 = mtx[i][0];
        WGPIPE_F32 = mtx[i][1];
        WGPIPE_F32 = mtx[i][2];
    }
}

/*
 * GXSetCurrentMtx - Set the current position/normal matrix.
 */
void GXSetCurrentMtx(u32 id) {
    /* Write CP matrix index register */
}

/*
 * GXLoadTexMtxImm - Load a texture matrix into the XF.
 */
void GXLoadTexMtxImm(f32 mtx[][4], u32 id, u8 type) {
    u32 addr;
    u32 rows;
    u32 i, j;

    if (id >= 0x40) {
        addr = 0x500 + (id - 0x40) * 4;
    } else {
        addr = id * 4;
    }

    rows = (type == 0) ? 2 : 3;

    /* Write XF load command */
    WGPIPE_U8 = 0x10;
    WGPIPE_U32 = (rows * 4) - 1;
    WGPIPE_U32 = addr;

    for (i = 0; i < rows; i++) {
        for (j = 0; j < 4; j++) {
            WGPIPE_F32 = mtx[i][j];
        }
    }
}

/*
 * GXSetProjection - Set the projection matrix.
 */
void GXSetProjection(f32 mtx[4][4], u8 type) {
    u32 i;

    WGPIPE_U8 = 0x10;
    WGPIPE_U32 = 0x0006;  /* 7-1 values */
    WGPIPE_U32 = 0x1020;

    if (type == 0) {
        /* Perspective */
        WGPIPE_F32 = mtx[0][0];
        WGPIPE_F32 = mtx[0][2];
        WGPIPE_F32 = mtx[1][1];
        WGPIPE_F32 = mtx[1][2];
        WGPIPE_F32 = mtx[2][2];
        WGPIPE_F32 = mtx[2][3];
        WGPIPE_U32 = 0;  /* perspective type */
    } else {
        /* Orthographic */
        WGPIPE_F32 = mtx[0][0];
        WGPIPE_F32 = mtx[0][3];
        WGPIPE_F32 = mtx[1][1];
        WGPIPE_F32 = mtx[1][3];
        WGPIPE_F32 = mtx[2][2];
        WGPIPE_F32 = mtx[2][3];
        WGPIPE_U32 = 1;  /* ortho type */
    }
}

/*
 * GXLoadProjectionMtx - Load a pre-formatted projection.
 */
void GXLoadProjectionMtx(f32 vals[7]) {
    u32 i;

    WGPIPE_U8 = 0x10;
    WGPIPE_U32 = 0x0006;
    WGPIPE_U32 = 0x1020;

    for (i = 0; i < 7; i++) {
        WGPIPE_F32 = vals[i];
    }
}
