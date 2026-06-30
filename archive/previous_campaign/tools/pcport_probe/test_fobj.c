/*
 * Standalone unit test for the faithful HSD FObj interpreter decompilation
 * (pcport/hsd_fobj_host.c). Compiled on the host with the project include
 * dir; includes the .c directly so the static helpers are exercisable.
 *
 *   gcc -DPCPORT -Iinclude tools/pcport_probe/test_fobj.c -o build_pc/test_fobj
 *
 * Validates:
 *   1. cubic-Hermite endpoints (u=0 -> p0, u=1 -> p1) + midpoint behaviour
 *   2. value decoder for float / s8 / u8 / s16 / u16 with shift scaling
 *   3. a full single-op LIN keyframe stream producing a linear ramp 0->100
 *      across a 30-frame segment (50 at the midpoint).
 */
#include <stdio.h>
#include <string.h>
#include <math.h>

#define PCPORT 1
#include "../../pcport/hsd_fobj_host.c"

static int g_fail = 0;
static void check(const char* what, double got, double want, double tol)
{
    double d = got - want;
    if (d < 0) d = -d;
    if (d > tol) {
        printf("  FAIL %-28s got=%.5f want=%.5f (tol %.5f)\n", what, got, want, tol);
        g_fail++;
    } else {
        printf("  ok   %-28s got=%.5f\n", what, got);
    }
}

/* update_func capturing the last interpolated value */
static f32 g_last;
static int g_calls;
static void capture(void* obj, u32 type, HSD_ObjData* v)
{
    (void) obj; (void) type;
    g_last = v->fv;
    g_calls++;
}

/* little-endian float into a byte buffer */
static void putf_le(u8* p, f32 v)
{
    union { u32 u; f32 f; } c; c.f = v;
    p[0] = (u8) (c.u);
    p[1] = (u8) (c.u >> 8);
    p[2] = (u8) (c.u >> 16);
    p[3] = (u8) (c.u >> 24);
}

int main(void)
{
    printf("== cubic-Hermite spline (s=1/fterm, t=time) ==\n");
    {
        f32 fterm = 30.0f, s = 1.0f / fterm;
        /* p0=10, p1=110, zero tangents: at u=0 -> 10, u=1 -> 110, u=0.5 -> 60 */
        check("spline t=0",      pcport_fobj_spline(s, 0.0f,  10.0f, 110.0f, 0.0f, 0.0f), 10.0,  1e-3);
        check("spline t=fterm",  pcport_fobj_spline(s, 30.0f, 10.0f, 110.0f, 0.0f, 0.0f), 110.0, 1e-3);
        check("spline t=mid",    pcport_fobj_spline(s, 15.0f, 10.0f, 110.0f, 0.0f, 0.0f), 60.0,  1e-3);
    }

    printf("== value decoder (frac encodings) ==\n");
    {
        HSD_FObj f; u8 buf[8];
        /* float */
        memset(&f, 0, sizeof f); putf_le(buf, -3.5f); f.ad = buf; f.ad_head = buf;
        check("float", fobj_read_value(&f, 0x00), -3.5, 1e-4);
        /* s8 shift 0 -> raw signed byte */
        memset(&f, 0, sizeof f); buf[0] = (u8) (s8) -7; f.ad = buf;
        check("s8 shift0", fobj_read_value(&f, 0x60), -7.0, 1e-4);
        /* u8 shift 4 -> 200/16 = 12.5 */
        memset(&f, 0, sizeof f); buf[0] = 200; f.ad = buf;
        check("u8 shift4", fobj_read_value(&f, 0x84), 200.0 / 16.0, 1e-4);
        /* s16 LE shift 0: 0x8001 -> -32767 */
        memset(&f, 0, sizeof f); buf[0] = 0x01; buf[1] = 0x80; f.ad = buf;
        check("s16 shift0", fobj_read_value(&f, 0x20), -32767.0, 1.0);
        /* u16 LE shift 8: 4096/256 = 16.0 */
        memset(&f, 0, sizeof f); buf[0] = 0x00; buf[1] = 0x10; f.ad = buf;
        check("u16 shift8", fobj_read_value(&f, 0x48), 16.0, 1e-4);
    }

    printf("== full LIN keyframe stream (two segments via state machine) ==\n");
    {
        /* Two LIN segments, each 30 frames:
         *   seg0: 0 -> 100   (slope 3.333/frame, spans frames [0,30))
         *   seg1: 100 -> 160 (slope 2.0/frame,   spans frames [30,60))
         * stream: [header op=2 count=3][v0=0][fterm0=30][v1=100][fterm1=30][v2=160]
         * Exercises header decode, varint fterm, value decode, the state
         * machine and the seg0->seg1 transition. */
        u8 stream[24];
        HSD_FObj f;
        int frame;
        u32 n = 0;
        stream[n++] = 0x22;                  /* op=2, count=3 */
        putf_le(&stream[n], 0.0f);   n += 4;  /* v0 */
        stream[n++] = 30;                     /* fterm0 */
        putf_le(&stream[n], 100.0f); n += 4;  /* v1 */
        stream[n++] = 30;                     /* fterm1 */
        putf_le(&stream[n], 160.0f); n += 4;  /* v2 */

        memset(&f, 0, sizeof f);
        f.ad_head = stream;
        f.length = n;
        f.frac_value = 0;   /* float */
        f.frac_slope = 0;
        f.obj_type = 1;
        PCPort_FObjStartAnim(&f, 0.0f);

        g_last = -999.0f;
        for (frame = 1; frame <= 50; frame++) {
            f32 prev = g_last;
            HSD_FObjInterpretAnim(&f, NULL, capture, 1.0f);
            if (frame == 15) check("seg0 @frame15", g_last, 50.0,  0.2);   /* mid seg0 */
            if (frame == 29) check("seg0 @frame29", g_last, 96.667, 0.2);  /* end seg0 */
            if (frame == 45) check("seg1 @frame45", g_last, 130.0, 0.2);   /* mid seg1 */
            if (frame > 1 && frame <= 45 && g_last < prev - 0.01) {
                printf("  FAIL monotonic break at frame %d: %.3f < %.3f\n",
                       frame, g_last, prev);
                g_fail++;
            }
        }
        printf("  (value after 50 frames = %.3f, %d callbacks)\n", g_last, g_calls);
    }

    printf(g_fail ? "\nRESULT: %d FAILURE(S)\n" : "\nRESULT: all checks passed\n", g_fail);
    return g_fail ? 1 : 0;
}
