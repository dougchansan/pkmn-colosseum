/**
 * @file hsd_fobj_host.c
 * @brief Faithful host decompilation of the HSD FObj keyframe interpreter.
 *
 * The PC port runs the game's OWN animation code. The FObj keyframe
 * interpreter is asm-only in the GC build (no decompiled C exists), so the
 * link previously auto-stubbed HSD_FObjInterpretAnim to a no-op and the host
 * HSD_FObjInterpretAnimAll only advanced the clock. This file is a literal,
 * faithful decompilation of the original PowerPC:
 *
 *   - HSD_FObjInterpretAnim   = fn_80199AF8  (src/hsd/hsd_state_fn_80199AF8.inc)
 *   - FObjLoadData            = fn_8019A24C  (src/hsd/hsd_state_fn_8019A24C.inc)
 *   - cubic-Hermite evaluator = fn_801B2560  (src/hsd/hsd_tev_fn_801B2560.inc)
 *
 * Constants were read verbatim from the DOL .sdata2 (r2 = _SDA2_BASE_
 * 0x804836A0): 0.0f, 0.0(d), 1.0(d), the 0x4330000000000000 unsigned and
 * 0x4330000080000000 signed int->double magics, and the Hermite basis
 * coefficients 3.0f / 2.0f / 1.0f.
 *
 * FObj state machine (low nibble of flags @0x10):
 *   0 = idle/done, 2 = load next packet, 3 = advance segment (decode fterm),
 *   4 = sample within segment, 5 = settled (re-enter 4 next frame),
 *   6 = steady per-frame output.  An FObj must be started in state 2 (the real
 *   HSD_FObjReqAnimAll sets nibble=2); PCPort_FObjStartAnim does that for the
 *   host wiring since the adapted src/hsd/hsd_fobj.c zeroes flags.
 *
 * Keyframe value encoding (frac byte): 0 => 32-bit float (LITTLE-endian in the
 * stream); else top 3 bits select s8 (0x60) / u8 (0x80) / s16 (0x20) /
 * u16 (0x40, both little-endian) and the low 5 bits are a shift: the decoded
 * integer is divided by (float)(1 << shift).
 *
 * PCPORT-only. Never compiled into the CodeWarrior byte-match build.
 */

#ifdef PCPORT

#include "dolphin/types.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_forward.h"

/* ------------------------------------------------------------------------- */
/*  cubic-Hermite evaluator (fn_801B2560), transcribed instruction-for-      */
/*  instruction. s = 1/fterm, t = time-within-segment; p0/p1 endpoints,      */
/*  d0/d1 tangents (value per frame). Equivalent to the standard Hermite     */
/*  basis with u = t/fterm:  p0*h00 + p1*h01 + d0*H10 + d1*H11.              */
/* ------------------------------------------------------------------------- */
static f32 pcport_fobj_spline(f32 s, f32 t, f32 p0, f32 p1, f32 d0, f32 d1)
{
    f32 f0, f1, f2, f7, f8, f9, f10, f11;

    f11 = t * t;
    f0  = 3.0f;
    f10 = s * s;
    f8  = 2.0f;
    f7  = 1.0f;
    f9  = f11 * t;
    f0  = f0 * f11;
    f11 = f11 * s;
    f9  = f10 * f9;
    f10 = f0 * f10;
    f8  = f8 * f9;
    f0  = f9 - f11;
    f9  = f9 - f11;
    f1  = f8 * s;
    f8  = f0 - f11;
    f0  = -f1;
    f1  = f1 - f10;
    f2  = t + f8;
    f0  = f0 + f10;
    f1  = f7 + f1;
    f0  = p1 * f0;
    f0  = p0 * f1 + f0;
    f1  = d0 * f2 + f0;
    f1  = d1 * f9 + f1;
    return f1;
}

/* ------------------------------------------------------------------------- */
/*  Decode one packed value from the keyframe stream, advancing fobj->ad.    */
/* ------------------------------------------------------------------------- */
static f32 fobj_read_value(HSD_FObj* fobj, u8 frac)
{
    if (frac == 0) {
        /* 32-bit float stored little-endian in the stream */
        union { u32 u; f32 fv; } cvt;
        u32 w;
        w  = (u32) fobj->ad[0];
        w |= (u32) fobj->ad[1] << 8;
        w |= (u32) fobj->ad[2] << 16;
        w |= (u32) fobj->ad[3] << 24;
        fobj->ad += 4;
        cvt.u = w;
        return cvt.fv;
    } else {
        u32 type = frac & 0xE0;
        u32 shift = frac & 0x1F;
        s32 iv;
        f32 divisor;

        switch (type) {
        case 0x60: { /* s8 */
            s8 b = (s8) fobj->ad[0];
            fobj->ad += 1;
            iv = (s32) b;
            break;
        }
        case 0x80: { /* u8 */
            u8 b = fobj->ad[0];
            fobj->ad += 1;
            iv = (s32) (u32) b;
            break;
        }
        case 0x20: { /* s16, little-endian */
            u32 lo = fobj->ad[0];
            s32 hi = (s8) fobj->ad[1];
            fobj->ad += 2;
            iv = (s32) (lo | ((u32) hi << 8));
            break;
        }
        case 0x40: { /* u16, little-endian */
            u32 lo = fobj->ad[0];
            u32 hi = fobj->ad[1];
            fobj->ad += 2;
            iv = (s32) (lo | (hi << 8));
            break;
        }
        default: /* shift with no encoding type -> 0.0 */
            return 0.0f;
        }

        divisor = (f32) (s32) (1u << shift);
        return (f32) iv / divisor;
    }
}

/* ------------------------------------------------------------------------- */
/*  Evaluate the current segment for the active interpolation kind           */
/*  (fobj->op_intrp). Returns 1 if a value was produced (call update_func),  */
/*  0 if no output should occur (op_intrp 6 with the "has value" bit clear). */
/*  Side effects (slope recompute for LIN, value-bit clear for CON) match    */
/*  the original interpreter exactly.                                        */
/* ------------------------------------------------------------------------- */
static int fobj_eval(HSD_FObj* fobj, f32* outVal)
{
    f32 fterm_f = (f32) fobj->fterm;

    switch (fobj->op_intrp) {
    case 1: /* hold value, step to p1 at the segment boundary */
        *outVal = (fobj->time >= fterm_f) ? fobj->p1 : fobj->p0;
        return 1;
    case 2: /* linear: slope = (p1-p0)/fterm, recomputed when flagged */
        if (fobj->flags & 0x20) {
            fobj->flags &= ~0x20;
            if (fobj->fterm != 0) {
                fobj->d0 = (fobj->p1 - fobj->p0) / fterm_f;
            } else {
                fobj->d0 = 0.0f;
                fobj->p0 = fobj->p1;
            }
        }
        *outVal = fobj->p0 + fobj->d0 * fobj->time;
        return 1;
    case 3:
    case 4:
    case 5: /* cubic-Hermite spline through p0..p1 with tangents d0,d1 */
        if (fobj->fterm != 0) {
            *outVal = pcport_fobj_spline((f32) (1.0 / (double) fterm_f),
                                         fobj->time, fobj->p0, fobj->p1,
                                         fobj->d0, fobj->d1);
        } else {
            *outVal = fobj->p1;
        }
        return 1;
    case 6: /* constant: emit p0 once, then suppress until reloaded */
        if (!(fobj->flags & 0x80)) {
            return 0;
        }
        *outVal = fobj->p0;
        fobj->flags &= ~0x80;
        return 1;
    default: /* 0 or >6: no defined interpolation, hold p0 */
        *outVal = fobj->p0;
        return 1;
    }
}

/* ------------------------------------------------------------------------- */
/*  FObjLoadData (fn_8019A24C): pull the next keyframe packet from the        */
/*  stream into p0/p1/d0/d1 and return the next state. Header byte yields     */
/*  op (low nibble) and a 7-bit varint run length (nb_pack).                  */
/* ------------------------------------------------------------------------- */
static u32 fobj_load_data(HSD_FObj* fobj)
{
    u32 oldstate;
    u32 newstate;

    if ((u32) (fobj->ad - fobj->ad_head) >= fobj->length) {
        return 6;
    }

    fobj->op_intrp = fobj->op;

    if (fobj->nb_pack == 0) {
        u8 b = *fobj->ad;
        u32 count;
        int sh;
        fobj->op = (u8) (b & 0x0F);
        b = *fobj->ad++;            /* consume header byte */
        count = ((u32) (b >> 4) & 7) + 1;
        sh = 3;
        while (b & 0x80) {
            b = *fobj->ad++;
            count += (u32) (b & 0x7F) << sh;
            sh += 7;
        }
        fobj->nb_pack = (u16) count;
    }
    fobj->nb_pack--;

    oldstate = fobj->flags & 0x0F;
    newstate = (oldstate == 1) ? 3 : 4;

    switch (fobj->op) {
    case 0:
        return 0;

    case 1: /* CON / LIN: one value into p1, shift tangent forward */
    case 2:
        fobj->p0 = fobj->p1;
        fobj->p1 = fobj_read_value(fobj, fobj->frac_value);
        if (fobj->op_intrp != 5) {
            fobj->d0 = fobj->d1;
            fobj->d1 = 0.0f;
        }
        fobj->flags = (u8) ((fobj->flags & 0xF0) | newstate);
        return newstate;

    case 3: /* SPL0: value into p1, carry old tangent into d0, zero d1 */
        fobj->p0 = fobj->p1;
        fobj->d0 = fobj->d1;
        fobj->p1 = fobj_read_value(fobj, fobj->frac_value);
        fobj->d1 = 0.0f;
        fobj->flags = (u8) ((fobj->flags & 0xF0) | newstate);
        return newstate;

    case 4: /* SPL: value into p1 and explicit slope into d1 */
        fobj->p0 = fobj->p1;
        fobj->p1 = fobj_read_value(fobj, fobj->frac_value);
        fobj->d0 = fobj->d1;
        fobj->d1 = fobj_read_value(fobj, fobj->frac_slope);
        fobj->flags = (u8) ((fobj->flags & 0xF0) | newstate);
        return newstate;

    case 5: /* SLP: only a new slope, state unchanged */
        fobj->d0 = fobj->d1;
        fobj->d1 = fobj_read_value(fobj, fobj->frac_slope);
        return fobj->flags & 0x0F;

    case 6: /* KEY/hold: latch op_intrp, mark "has value", reload p1 */
        if (fobj->flags & 0x40) {
            fobj->op_intrp = fobj->op;
            fobj->flags &= ~0x40;
            fobj->flags |= 0x80;
            fobj->p0 = fobj->p1;
        }
        fobj->p1 = fobj_read_value(fobj, fobj->frac_value);
        fobj->flags |= 0x40;
        fobj->flags = (u8) ((fobj->flags & 0xF0) | newstate);
        return newstate;

    default:
        return 0;
    }
}

/* ------------------------------------------------------------------------- */
/*  HSD_FObjInterpretAnim (fn_80199AF8): advance one FObj's clock by `rate`  */
/*  and run the state machine until it settles, calling update_func with the */
/*  interpolated value (obj, obj_type, &HSD_ObjData).                        */
/* ------------------------------------------------------------------------- */
void HSD_FObjInterpretAnim(HSD_FObj* fobj, void* obj,
                           HSD_ObjUpdateFunc update_func, f32 rate)
{
    u32 state;
    HSD_ObjData val;

    state = (fobj != NULL) ? (u32) (fobj->flags & 0x0F) : 0;
    if (state == 0) {
        return;
    }

    fobj->time += rate;
    if ((double) fobj->time < 0.0) {
        return;
    }

    for (;;) {
        switch (state) {
        case 1:
        case 2:
            state = fobj_load_data(fobj);
            continue;

        case 5: /* settled last frame; re-enter the sampling state */
            fobj->flags = (u8) ((fobj->flags & 0xF0) | 4);
            state = 4;
            continue;

        case 6: /* steady per-frame output */
            fobj->time += 0.0f;
            if (fobj->flags & 0x40) {
                fobj->op_intrp = fobj->op;
                fobj->flags &= ~0x40;
                fobj->flags |= 0x80;
                fobj->p0 = fobj->p1;
            }
            if (update_func == NULL) {
                return;
            }
            if (fobj_eval(fobj, &val.fv)) {
                update_func(obj, fobj->obj_type, &val);
            }
            return;

        case 3: /* advance to the next segment, decoding its length (fterm) */
            if ((fobj->flags & 0x80) && update_func != NULL) {
                if (fobj_eval(fobj, &val.fv)) {
                    update_func(obj, fobj->obj_type, &val);
                }
            }
            if ((u32) (fobj->ad - fobj->ad_head) >= fobj->length) {
                state = 6;
                continue;
            }
            {
                u32 acc = 0;
                int sh = 0;
                u8 b;
                do {
                    b = *fobj->ad++;
                    acc |= (u32) (b & 0x7F) << sh;
                    sh += 7;
                } while (b & 0x80);
                fobj->fterm = (u16) acc;
            }
            fobj->flags |= 0x20;
            fobj->flags = (u8) ((fobj->flags & 0xF0) | 2);
            state = 2;
            continue;

        case 4: { /* sample within the current segment */
            f32 fterm_f = (f32) fobj->fterm;
            if (fterm_f > fobj->time) {
                if (update_func != NULL) {
                    if (fobj_eval(fobj, &val.fv)) {
                        update_func(obj, fobj->obj_type, &val);
                    }
                }
                fobj->flags = (u8) ((fobj->flags & 0xF0) | 5);
                return;
            } else {
                fobj->time = fobj->time - fterm_f;
                fobj->flags = (u8) ((fobj->flags & 0xF0) | 3);
                state = 3;
                continue;
            }
        }

        default:
            return;
        }
    }
}

/* ------------------------------------------------------------------------- */
/*  HSD_FObjInterpretAnimAll (fn_80199A88) lives in hsd_host.c and walks the */
/*  FObj list calling the above for each node.                               */
/* ------------------------------------------------------------------------- */

/* Host kickoff: put an FObj list into the "load next packet" start state.
 * The real HSD_FObjReqAnimAll sets the low nibble to 2; the adapted
 * src/hsd/hsd_fobj.c zeroes flags, which would leave the machine idle, so the
 * host animation wiring calls this once after loading a scene's anims. */
void PCPort_FObjStartAnim(HSD_FObj* fobj, f32 startframe)
{
    HSD_FObj* f;
    for (f = fobj; f != NULL; f = f->next) {
        f->ad = f->ad_head;
        f->time = startframe;
        f->flags = (u8) ((f->flags & 0xF0) | 2);
        f->nb_pack = 0;
        /* Reset segment control state to match the initial-load condition
         * (HSD_FObjAlloc memsetz fterm/p0/p1/d0/d1 to 0).  Re-arming
         * without this leaves stale fterm from the previous cycle, causing
         * the state-4 "fterm_f > time" guard to skip over the stream-decode
         * in state-3 and sample the wrong (terminal) segment's control points
         * for the whole next cycle. */
        f->fterm = 0;
        f->p0    = 0.0f;
        f->p1    = 0.0f;
        f->d0    = 0.0f;
        f->d1    = 0.0f;
    }
}

#endif /* PCPORT */
