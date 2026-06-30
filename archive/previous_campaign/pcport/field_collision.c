/* Host-side WZX field-collision parser + floor query for the PC port.
 * See field_collision.h for the on-disc WZX layout. Pure host C (no engine
 * dependency): the game's own GScolsys2 (src/game/gs_colsys.c) reads the same
 * data, but for the port's free-fly/walk path a flat triangle list + a vertical
 * ray query is simpler and avoids dragging in the GSmem allocator + layer BSS. */

#include "field_collision.h"
#include "real_content_host.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    f32 v[3][3];   /* three triangle vertices */
    f32 n[3];      /* face normal (from the WZX record) */
    int cat;       /* PCPortColCategory */
} ColTri;

static ColTri* s_tris = NULL;
static int s_triCount = 0;
static int s_triCap = 0;
static f32 s_min[3], s_max[3];

static u32 ReadBE32(const u8* p) {
    return ((u32)p[0] << 24) | ((u32)p[1] << 16) | ((u32)p[2] << 8) | (u32)p[3];
}

static f32 ReadBEf32(const u8* p) {
    union { u32 u; f32 f; } v;
    v.u = ReadBE32(p);
    return v.f;
}

static BOOL FiniteBounded(f32 a) {
    /* a == a rejects NaN; the magnitude clamp rejects inf / garbage. */
    return (a == a) && a < 1.0e6f && a > -1.0e6f;
}

static void PushTri(const u8* base, u32 triOff, int cat) {
    ColTri* t;
    int i, j;
    f32 vals[9], nrm[3];

    for (i = 0; i < 9; ++i) {
        vals[i] = ReadBEf32(base + triOff + (u32)i * 4u);
        if (!FiniteBounded(vals[i])) {
            return;
        }
    }
    for (i = 0; i < 3; ++i) {
        nrm[i] = ReadBEf32(base + triOff + 0x24u + (u32)i * 4u);
        if (!FiniteBounded(nrm[i])) {
            nrm[i] = 0.0f;
        }
    }

    if (s_triCount == s_triCap) {
        int newCap = s_triCap ? s_triCap * 2 : 256;
        ColTri* grown = (ColTri*)realloc(s_tris, (size_t)newCap * sizeof(ColTri));
        if (grown == NULL) {
            return;
        }
        s_tris = grown;
        s_triCap = newCap;
    }

    t = &s_tris[s_triCount++];
    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
            t->v[i][j] = vals[i * 3 + j];
            if (t->v[i][j] < s_min[j]) s_min[j] = t->v[i][j];
            if (t->v[i][j] > s_max[j]) s_max[j] = t->v[i][j];
        }
    }
    t->n[0] = nrm[0]; t->n[1] = nrm[1]; t->n[2] = nrm[2];
    t->cat = cat;
}

void PCPort_FieldColUnload(void) {
    free(s_tris);
    s_tris = NULL;
    s_triCount = 0;
    s_triCap = 0;
}

int PCPort_FieldColLoad(const char* fsysPath) {
    u8* wzx = NULL;
    u32 wzxSize = 0;
    u32 vertOff, groupCount, g;

    PCPort_FieldColUnload();

    if (!PCPort_LoadFsysWZXMember(fsysPath, &wzx, &wzxSize) || wzx == NULL) {
        return 0;
    }
    if (wzxSize < 0x48u) {
        PCPort_FreeBuffer(wzx);
        return 0;
    }

    s_min[0] = s_min[1] = s_min[2] = 1.0e30f;
    s_max[0] = s_max[1] = s_max[2] = -1.0e30f;

    vertOff = ReadBE32(wzx + 0x00);
    groupCount = ReadBE32(wzx + 0x04);
    if (vertOff < 8u || vertOff >= wzxSize || groupCount == 0u || groupCount > 256u) {
        PCPort_FreeBuffer(wzx);
        return 0;
    }

    for (g = 0; g < groupCount; ++g) {
        u32 grpBase = vertOff + g * 0x40u;
        u32 slot;
        if (grpBase + 0x40u > wzxSize) {
            break;
        }
        for (slot = 0; slot < 6u; ++slot) {
            u32 so = ReadBE32(wzx + grpBase + 0x24u + slot * 4u);
            u32 smVtx, smCnt, t;
            if (so == 0u || so + 0x10u > wzxSize) {
                continue;
            }
            smVtx = ReadBE32(wzx + so + 0x00u);
            smCnt = ReadBE32(wzx + so + 0x04u);
            if (smVtx == 0u || smCnt == 0u || smCnt > 100000u) {
                continue;
            }
            if (smVtx + smCnt * 0x34u > wzxSize) {
                continue;
            }
            for (t = 0; t < smCnt; ++t) {
                PushTri(wzx, smVtx + t * 0x34u, (int)slot);
            }
        }
    }

    PCPort_FreeBuffer(wzx);
    return s_triCount;
}

int PCPort_FieldColTriCount(void) {
    return s_triCount;
}

BOOL PCPort_FieldColGetTri(int i, f32 out9[9], int* outCat) {
    int a, b;
    if (i < 0 || i >= s_triCount || out9 == NULL) {
        return FALSE;
    }
    for (a = 0; a < 3; ++a) {
        for (b = 0; b < 3; ++b) {
            out9[a * 3 + b] = s_tris[i].v[a][b];
        }
    }
    if (outCat != NULL) {
        *outCat = s_tris[i].cat;
    }
    return TRUE;
}

BOOL PCPort_FieldColBounds(f32 outMin[3], f32 outMax[3]) {
    int j;
    if (s_triCount == 0) {
        return FALSE;
    }
    for (j = 0; j < 3; ++j) {
        if (outMin) outMin[j] = s_min[j];
        if (outMax) outMax[j] = s_max[j];
    }
    return TRUE;
}

/* True if (px,pz) is inside triangle t's XZ projection. Uses the sign of the
 * three edge cross-products; accepts either winding (collision tris are not
 * consistently wound). A small epsilon includes shared edges. */
static BOOL PointInTriXZ(const ColTri* t, f32 px, f32 pz) {
    f32 d1, d2, d3;
    BOOL hasNeg, hasPos;
    const f32 eps = 1.0e-3f;

    d1 = (px - t->v[1][0]) * (t->v[0][2] - t->v[1][2]) -
         (t->v[0][0] - t->v[1][0]) * (pz - t->v[1][2]);
    d2 = (px - t->v[2][0]) * (t->v[1][2] - t->v[2][2]) -
         (t->v[1][0] - t->v[2][0]) * (pz - t->v[2][2]);
    d3 = (px - t->v[0][0]) * (t->v[2][2] - t->v[0][2]) -
         (t->v[2][0] - t->v[0][0]) * (pz - t->v[0][2]);

    hasNeg = (d1 < -eps) || (d2 < -eps) || (d3 < -eps);
    hasPos = (d1 > eps) || (d2 > eps) || (d3 > eps);
    return !(hasNeg && hasPos);
}

/* Solve the triangle's plane for Y at (px,pz). Returns FALSE for near-vertical
 * surfaces (no well-defined floor height). */
static BOOL TriPlaneY(const ColTri* t, f32 px, f32 pz, f32* outY) {
    /* Plane normal via edge cross product (don't trust the stored normal's
     * sign/scale for this). */
    f32 e1[3], e2[3], nx, ny, nz;
    e1[0] = t->v[1][0] - t->v[0][0];
    e1[1] = t->v[1][1] - t->v[0][1];
    e1[2] = t->v[1][2] - t->v[0][2];
    e2[0] = t->v[2][0] - t->v[0][0];
    e2[1] = t->v[2][1] - t->v[0][1];
    e2[2] = t->v[2][2] - t->v[0][2];
    nx = e1[1] * e2[2] - e1[2] * e2[1];
    ny = e1[2] * e2[0] - e1[0] * e2[2];
    nz = e1[0] * e2[1] - e1[1] * e2[0];

    if (ny < 1.0e-4f && ny > -1.0e-4f) {
        return FALSE;
    }
    /* n . (P - v0) = 0  ->  Py = v0y - (nx*(Px-v0x) + nz*(Pz-v0z)) / ny */
    *outY = t->v[0][1] -
            (nx * (px - t->v[0][0]) + nz * (pz - t->v[0][2])) / ny;
    return TRUE;
}

static BOOL Walkable(int cat) {
    return cat == PCPORT_COLCAT_FLOOR ||
           cat == PCPORT_COLCAT_SLOPE ||
           cat == PCPORT_COLCAT_EXTFLOOR;
}

static BOOL Blocking(int cat) {
    return cat == PCPORT_COLCAT_WALL || cat == PCPORT_COLCAT_BOUND;
}

/* Closest point on segment (ax,az)-(bx,bz) to (px,pz), in XZ. */
static void ClosestOnSeg(f32 ax, f32 az, f32 bx, f32 bz,
                         f32 px, f32 pz, f32* cx, f32* cz) {
    f32 dx = bx - ax, dz = bz - az;
    f32 len2 = dx * dx + dz * dz;
    f32 t;
    if (len2 < 1.0e-8f) {
        *cx = ax; *cz = az;
        return;
    }
    t = ((px - ax) * dx + (pz - az) * dz) / len2;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    *cx = ax + dx * t;
    *cz = az + dz * t;
}

BOOL PCPort_FieldColResolveXZ(f32* x, f32* z, f32 yLo, f32 yHi, f32 radius) {
    int pass, i, e;
    BOOL pushedAny = FALSE;
    const f32 r2 = radius * radius;

    if (x == NULL || z == NULL || radius <= 0.0f) {
        return FALSE;
    }

    /* A few relaxation passes so a corner (two walls) settles. */
    for (pass = 0; pass < 4; ++pass) {
        BOOL pushedThisPass = FALSE;
        for (i = 0; i < s_triCount; ++i) {
            const ColTri* t = &s_tris[i];
            f32 triLo, triHi;
            int k;
            if (!Blocking(t->cat)) {
                continue;
            }
            /* Vertical-span overlap test with the player's body. */
            triLo = triHi = t->v[0][1];
            for (k = 1; k < 3; ++k) {
                if (t->v[k][1] < triLo) triLo = t->v[k][1];
                if (t->v[k][1] > triHi) triHi = t->v[k][1];
            }
            if (triHi < yLo || triLo > yHi) {
                continue;
            }
            /* Push out of the nearest of the triangle's 3 XZ edges. */
            for (e = 0; e < 3; ++e) {
                int a0 = e, a1 = (e + 1) % 3;
                f32 cx, cz, dx, dz, d2;
                ClosestOnSeg(t->v[a0][0], t->v[a0][2],
                             t->v[a1][0], t->v[a1][2], *x, *z, &cx, &cz);
                dx = *x - cx; dz = *z - cz;
                d2 = dx * dx + dz * dz;
                if (d2 < r2 && d2 > 1.0e-8f) {
                    f32 d = (f32)sqrt((double)d2);
                    f32 push = (radius - d) / d;
                    *x += dx * push;
                    *z += dz * push;
                    pushedThisPass = TRUE;
                    pushedAny = TRUE;
                }
            }
        }
        if (!pushedThisPass) {
            break;
        }
    }
    return pushedAny;
}

/* ------------------------------------------------------------------------- *
 *  Field exit / door triggers (map warps)
 * ------------------------------------------------------------------------- */

static PCPortFieldExit* s_exits = NULL;
static int s_exitCount = 0;

void PCPort_FieldExitUnload(void) {
    free(s_exits);
    s_exits = NULL;
    s_exitCount = 0;
}

void PCPort_FieldExitSet(const PCPortFieldExit* exits, int n) {
    PCPort_FieldExitUnload();
    if (exits == NULL || n <= 0) {
        return;
    }
    s_exits = (PCPortFieldExit*)malloc((size_t)n * sizeof(PCPortFieldExit));
    if (s_exits == NULL) {
        return;
    }
    memcpy(s_exits, exits, (size_t)n * sizeof(PCPortFieldExit));
    s_exitCount = n;
}

int PCPort_FieldExitCount(void) {
    return s_exitCount;
}

BOOL PCPort_FieldExitGet(int i, PCPortFieldExit* out) {
    if (i < 0 || i >= s_exitCount || out == NULL) {
        return FALSE;
    }
    *out = s_exits[i];
    return TRUE;
}

int PCPort_FieldExitCheck(f32 px, f32 py, f32 pz, f32 mvx, f32 mvz) {
    int i;
    f32 mlen2 = mvx * mvx + mvz * mvz;
    (void)py;
    for (i = 0; i < s_exitCount; ++i) {
        const PCPortFieldExit* e = &s_exits[i];
        f32 dx = e->pos[0] - px;
        f32 dz = e->pos[2] - pz;
        f32 d2 = dx * dx + dz * dz;
        if (e->radius <= 0.0f || d2 > e->radius * e->radius) {
            continue;
        }
        /* Approach-cone gate: only fire if the player is moving roughly toward
         * the door (within halfAngle of the exit facing). halfAngle<=0 = any. */
        if (e->halfAngle > 0.0f && mlen2 > 1.0e-6f) {
            f32 fdx = sinf(e->facing);
            f32 fdz = -cosf(e->facing);
            f32 mlen = (f32)sqrt((double)mlen2);
            f32 dot = (mvx * fdx + mvz * fdz) / mlen;
            if (dot < cosf(e->halfAngle)) {
                continue;
            }
        }
        return i;
    }
    return -1;
}

BOOL PCPort_FieldColFloorAt(f32 x, f32 z, f32 queryY, f32 climb, f32* outY) {
    int i;
    BOOL found = FALSE;
    f32 best = -1.0e30f;

    if (outY == NULL) {
        return FALSE;
    }
    for (i = 0; i < s_triCount; ++i) {
        const ColTri* t = &s_tris[i];
        f32 y;
        if (!Walkable(t->cat)) {
            continue;
        }
        if (!PointInTriXZ(t, x, z)) {
            continue;
        }
        if (!TriPlaneY(t, x, z, &y)) {
            continue;
        }
        /* Floors at or below the query point (allow a small upward step). */
        if (y <= queryY + climb && y > best) {
            best = y;
            found = TRUE;
        }
    }
    if (found) {
        *outY = best;
    }
    return found;
}
