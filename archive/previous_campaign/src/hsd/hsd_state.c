
/**
 * @file hsd_state.c
 * @brief HSD internal functions (0x80199A84-0x8019B490).
 *
 * Stub coverage for 4 functions.
 */

#include "dolphin/types.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_fobj.h"

/* Forward declarations for converted functions */
u32 fn_8019A24C(HSD_FObj* fobj);
void fn_80199AF8(HSD_FObj* fobj, void* obj, HSD_ObjUpdateFunc obj_update, f32 rate);

extern f32 lbl_8047DA3C; /* 0.0f */
extern f64 lbl_8047DA40; /* 0.0  */
extern f64 lbl_8047DA48; /* spline numerator */
extern f32 splGetHelmite(f32 step, f32 time, f32 p0, f32 p1, f32 d0, f32 d1);

/* 0x80199A84 | 0x4 */
void fn_80199A84(void) {
}

/* 0x70 | fn_80199A88 | HSD_FObjInterpretAnimAll */
void fn_80199A88(HSD_FObj* fobj, void* obj, HSD_ObjUpdateFunc obj_update, f32 rate) {
    HSD_FObj* node;
    for (node = fobj; node != NULL; node = node->next) {
        fn_80199AF8(node, obj, obj_update, rate);
    }
}

/* 0x80199AF8 | 0x754 | HSD_FObjInterpretAnim */
static inline u32 FObjGetState(HSD_FObj* fobj) {
    return (fobj != NULL) ? (fobj->flags & 0xF) : 0;
}

static inline void FObjSetState(HSD_FObj* fobj, u32 state) {
    if (fobj != NULL) {
        fobj->flags = (fobj->flags & 0xF0) | (state & 0xF);
    }
}

static inline void FObjLaunchKeyData(HSD_FObj* fobj) {
    if (fobj->flags & 0x40) {
        fobj->op_intrp = fobj->op;
        fobj->flags &= ~0x40;
        fobj->flags |= 0x80;
        fobj->p0 = fobj->p1;
    }
}

/* FObjUpdateAnim, expanded as a macro so each of the three call sites gets its
 * own stack slot for `value` (matches the original's per-site frame layout). */
#define FOBJ_UPDATE_ANIM(skip)                                                 \
    {                                                                          \
        HSD_ObjData value;                                                     \
        if (obj_update != NULL) {                                              \
            switch (fobj->op_intrp) {                                          \
            case HSD_A_OP_KEY:                                                 \
                if (!(fobj->flags & 0x80)) {                                   \
                    goto skip;                                                 \
                }                                                              \
                value.fv = fobj->p0;                                           \
                fobj->flags &= 0x7F;                                           \
                break;                                                         \
            case HSD_A_OP_CON:                                                 \
                value.fv =                                                     \
                    (fobj->time >= (f32)fobj->fterm) ? fobj->p1 : fobj->p0;    \
                break;                                                         \
            case HSD_A_OP_LIN:                                                 \
                if (fobj->flags & 0x20) {                                      \
                    fobj->flags &= ~0x20;                                      \
                    if (fobj->fterm != 0) {                                    \
                        fobj->d0 = (fobj->p1 - fobj->p0) / (f32)fobj->fterm;   \
                    } else {                                                   \
                        fobj->d0 = lbl_8047DA3C;                               \
                        fobj->p0 = fobj->p1;                                   \
                    }                                                          \
                }                                                              \
                value.fv = fobj->d0 * fobj->time + fobj->p0;                   \
                break;                                                         \
            case HSD_A_OP_SPL0:                                                \
            case HSD_A_OP_SPL:                                                 \
            case HSD_A_OP_SLP:                                                 \
                if (fobj->fterm != 0) {                                        \
                    value.fv = splGetHelmite(                                  \
                        (f32)(lbl_8047DA48 / (f32)fobj->fterm), fobj->time,    \
                        fobj->p0, fobj->p1, fobj->d0, fobj->d1);               \
                } else {                                                       \
                    value.fv = fobj->p1;                                       \
                }                                                              \
                break;                                                         \
            }                                                                  \
            obj_update(obj, fobj->obj_type, &value);                          \
        }                                                                      \
    }                                                                          \
    skip:

static inline u32 FObjLoadWait(HSD_FObj* fobj) {
    u32 val;
    s32 shift;
    u8 b;

    HSD_ASSERT(0x16C, FObjGetState(fobj) == FOBJ_LOAD_WAIT);
    if ((u32)(fobj->ad - fobj->ad_head) >= fobj->length) {
        return 6;
    }
    val = 0;
    shift = 0;
    do {
        b = *fobj->ad++;
        val |= (u32)(b & 0x7F) << shift;
        shift += 7;
    } while (b & 0x80);
    fobj->fterm = val;
    fobj->flags |= 0x20;
    FObjSetState(fobj, FOBJ_LOAD_DATA);
    return 2;
}

void fn_80199AF8(HSD_FObj* fobj, void* obj, HSD_ObjUpdateFunc obj_update,
                 f32 rate) {
#pragma fp_contract on
    f32 fterm;
    u32 state;

    fterm = lbl_8047DA3C;
    state = (fobj != NULL) ? FObjGetState(fobj) : 0;
    if (state != 0 && !(fobj->time += rate, fobj->time < lbl_8047DA40)) {
    for (;;) {
        switch (state) {
        case 6:
            fobj->time += fterm;
            FObjLaunchKeyData(fobj);
            FOBJ_UPDATE_ANIM(skip6);
            return;
        case 1:
        case 2:
            state = fn_8019A24C(fobj);
            break;
        case 3:
            if (fobj->flags & 0x80) {
                FOBJ_UPDATE_ANIM(skip3);
            }
            state = FObjLoadWait(fobj);
            break;
        case 4:
            if ((f32)fobj->fterm <= fobj->time) {
                fterm = (f32)fobj->fterm;
                fobj->time -= (f32)fobj->fterm;
                state = 3;
                FObjSetState(fobj, state);
                break;
            }
            FOBJ_UPDATE_ANIM(skip4);
            state = 5;
            FObjSetState(fobj, state);
            return;
        case 5:
            state = 4;
            FObjSetState(fobj, state);
            break;
        case 0:
            return;
        }
    }
    }
}

/* 0x8019A24C | 0x1244 */
u32 fn_8019A24C(HSD_FObj* fobj) {
    extern u8 lbl_8027477C[];
    extern u8 lbl_8047DA30[];
    extern f32 lbl_8047DA3C;
    extern f64 lbl_8047DA50;
    extern f64 lbl_8047DA58;
    extern void __assert();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;

    r31 = r3;
    r4 = *(u32*)((u8*)r3 + 0x8);
    r3 = *(u32*)((u8*)r3 + 0x4);
    tmp = *(u32*)((u8*)r31 + 0xC);
    r3 = r3 - r4;
    if (r3 >= tmp) {
        r3 = 0x6;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x11);
    *(u8*)((u8*)r31 + 0x12) = tmp;
    tmp = *(u16*)((u8*)r31 + 0x16);
    if (tmp == 0) {
        r3 = *(u32*)((u8*)r31 + 0x4);
        r4 = 0x3;
        tmp = *(u8*)((u8*)r3 + 0x0);
        tmp = tmp & 0xF;
        *(u8*)((u8*)r31 + 0x11) = tmp;
        r3 = *(u32*)((u8*)r31 + 0x4);
        tmp = r3 + 0x1;
        *(u32*)((u8*)r31 + 0x4) = tmp;
        r3 = *(u8*)((u8*)r3 + 0x0);
        tmp = r3 & 0x00000080;
        /* extrwi r3, r3, 3, 25 */;
        r5 = r3 + 0x1;
        if (tmp == 0) {

        } else {
        do {
                r3 = *(u32*)((u8*)r31 + 0x4);
                tmp = r3 + 0x1;
                *(u32*)((u8*)r31 + 0x4) = tmp;
                r3 = *(u8*)((u8*)r3 + 0x0);
                tmp = r3 & 0x00000080;
                r3 = r3 & 0x7F;
                r3 = r3 << r4;
                r4 = r4 + 0x7;
                r5 = r5 + r3;
        } while (tmp != 0);
        }
        *(u16*)((u8*)r31 + 0x16) = r5;
    }
    r3 = *(u16*)((u8*)r31 + 0x16);
    *(u16*)((u8*)r31 + 0x16) = tmp;
    tmp = *(u8*)((u8*)r31 + 0x11);
    if ((s32)tmp != 4) {
        if ((s32)tmp < 4) {
            if ((s32)tmp != 2) {
                if ((s32)tmp < 2) {
                    if ((s32)tmp < 1) {
                        r3 = 0x0;
                        return;
                    }
                    if ((s32)tmp != 6) {
                        if ((s32)tmp >= 6) { r3 = 0x0; return; }
                        goto L_8019AF60;
                        }
                    if (r31 == 0) {
                        r30 = 0x0;
                    } else {

                        tmp = *(u8*)((u8*)r31 + 0x10);
                        r30 = tmp & 0xF;
                    }
                    r3 = 0x1;
                    tmp = r3 - r4;
                    r5 = 0x1;
                    r3 = r3 | ~r4;
                    tmp = (u32)tmp >> 1;
                    tmp = r3 - tmp;
                    /* srwi. tmp, tmp, 31 */;
                    if (r31 == 0) {
                        r5 = 0x0;
                    }
                    if ((s32)r5 == 0) {
                        r4 = (u32)lbl_8027477C;
                        r3 = (u32)lbl_8047DA30;
                        r5 = (u32)lbl_8027477C;
                        r4 = 0x17f;
                        __assert();
                    }
                    f0 = *(f32*)((u8*)r31 + 0x24);
                    *(f32*)((u8*)r31 + 0x20) = f0;
                    r5 = *(u8*)((u8*)r31 + 0x14);
                    tmp = r5 & 0xFF;
                    if ((s32)r5 == 0) {
                        r4 = *(u32*)((u8*)r31 + 0x4);
                        tmp = r4 + 0x1;
                        *(u32*)((u8*)r31 + 0x4) = tmp;
                        r3 = *(u32*)((u8*)r31 + 0x4);
                        r5 = *(u8*)((u8*)r4 + 0x0);
                        tmp = r3 + 0x1;
                        *(u32*)((u8*)r31 + 0x4) = tmp;
                        r4 = *(u32*)((u8*)r31 + 0x4);
                        r3 = *(u8*)((u8*)r3 + 0x0);
                        tmp = r4 + 0x1;
                        *(u32*)((u8*)r31 + 0x4) = tmp;
                        tmp = r3 << 8;
                        tmp = r5 | tmp;
                        *(u32*)(sp + 0x20) = tmp;
                        r3 = *(u32*)((u8*)r31 + 0x4);
                        r4 = *(u8*)((u8*)r4 + 0x0);
                        tmp = r3 + 0x1;
                        r4 = r4 << 16;
                        *(u32*)((u8*)r31 + 0x4) = tmp;
                        tmp = r5 | r4;
                        *(u32*)(sp + 0x20) = tmp;
                        tmp = *(u8*)((u8*)r3 + 0x0);
                        tmp = tmp << 24;
                        tmp = r3 | tmp;
                        *(u32*)(sp + 0x20) = tmp;
                        f0 = *(f32*)(sp + 0x20);

                    } else {
                        r3 = r5 & 0x000000E0;
                        r4 = 0x1;
                        tmp = r5 & 0x1F;
                        tmp = r4 << tmp;
                        do {
                        if ((s32)r3 == 0x20) {
                                    r5 = *(u32*)((u8*)r31 + 0x4);
                                    r4 = 0x43300000;
                                    r3 = *(u32*)((u8*)r31 + 0x4);
                                    r5 = *(u8*)((u8*)r5 + 0x0);
                                    r3 = r3 + 0x1;
                                    r4 = (s8)r5;
                                    f1 = lbl_8047DA58;
                                    *(u32*)((u8*)r31 + 0x4) = r3;
                                    f2 = f0 - f1;
                        } else if ((s32)r3 == 0x40) {
                                r5 = *(u32*)((u8*)r31 + 0x4);
                                r4 = 0x43300000;
                                r3 = *(u32*)((u8*)r31 + 0x4);
                                r5 = *(u8*)((u8*)r5 + 0x0);
                                r3 = r3 + 0x1;
                                f1 = lbl_8047DA50;
                                *(u32*)((u8*)r31 + 0x4) = r3;
                                f2 = f0 - f1;
                        } else if ((s32)r3 == 0x60) {
                            r3 = *(u32*)((u8*)r31 + 0x4);
                            r4 = 0x43300000;
                            r5 = *(u32*)((u8*)r31 + 0x4);
                            r6 = *(u8*)((u8*)r3 + 0x1);
                            r3 = *(u32*)((u8*)r31 + 0x4);
                            r6 = (s8)r6;
                            r5 = *(u8*)((u8*)r5 + 0x0);
                            r5 = (r5 & ~0xFFFFFF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0xFFFFFF00);
                            r3 = r3 + 0x2;
                            f1 = lbl_8047DA58;
                            *(u32*)((u8*)r31 + 0x4) = r3;
                            f2 = f0 - f1;
                        } else if ((s32)r3 == 0x80) {
                        r5 = *(u32*)((u8*)r31 + 0x4);
                        r4 = 0x43300000;
                        r3 = *(u32*)((u8*)r31 + 0x4);
                        r6 = *(u8*)((u8*)r5 + 0x1);
                        r5 = *(u8*)((u8*)r3 + 0x0);
                        r5 = (r5 & ~0x0000FF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0x0000FF00);
                        r3 = *(u32*)((u8*)r31 + 0x4);
                        r3 = r3 + 0x2;
                        f1 = lbl_8047DA58;
                        *(u32*)((u8*)r31 + 0x4) = r3;
                        f2 = f0 - f1;
                        } else {
                            f0 = lbl_8047DA3C;
                            break;
                        }
                        tmp = 0x43300000;
                        f1 = lbl_8047DA58;
                        *(u32*)(sp + 0x28) = tmp;
                        f0 = f0 - f1;
                        f0 = f2 / f0;
                        } while (0);
                    }
                    *(f32*)((u8*)r31 + 0x24) = f0;
                    tmp = *(u8*)((u8*)r31 + 0x12);
                    if (tmp != 5) {
                        f1 = *(f32*)((u8*)r31 + 0x2C);
                        f0 = lbl_8047DA3C;
                        *(f32*)((u8*)r31 + 0x28) = f1;
                        *(f32*)((u8*)r31 + 0x2C) = f0;
                    }
                    tmp = 0x1;
                    r4 = r30 - tmp;
                    r3 = 0x4;
                    tmp = tmp - r30;
                    tmp = ~(r4 | tmp);
                    tmp = (s32)tmp >> 31;
                    r3 = tmp + r3;
                    if (r31 == 0) return;
                    tmp = *(u8*)((u8*)r31 + 0x10);
                    tmp = tmp & 0x000000F0;
                    tmp = (tmp & ~0x0000000F) | (((r3 << 0) | ((u32)r3 >> 32)) & 0x0000000F);
                    *(u8*)((u8*)r31 + 0x10) = tmp;
                    return;
                    }
                if (r31 == 0) {
                    r30 = 0x0;
                } else {

                    tmp = *(u8*)((u8*)r31 + 0x10);
                    r30 = tmp & 0xF;
                }
                r3 = 0x1;
                tmp = r3 - r4;
                r5 = 0x1;
                r3 = r3 | ~r4;
                tmp = (u32)tmp >> 1;
                tmp = r3 - tmp;
                /* srwi. tmp, tmp, 31 */;
                if (r31 == 0) {
                    r5 = 0x0;
                }
                if ((s32)r5 == 0) {
                    r4 = (u32)lbl_8027477C;
                    r3 = (u32)lbl_8047DA30;
                    r5 = (u32)lbl_8027477C;
                    r4 = 0x193;
                    __assert();
                }
                f0 = *(f32*)((u8*)r31 + 0x24);
                *(f32*)((u8*)r31 + 0x20) = f0;
                r5 = *(u8*)((u8*)r31 + 0x14);
                tmp = r5 & 0xFF;
                if ((s32)r5 == 0) {
                    r4 = *(u32*)((u8*)r31 + 0x4);
                    tmp = r4 + 0x1;
                    *(u32*)((u8*)r31 + 0x4) = tmp;
                    r3 = *(u32*)((u8*)r31 + 0x4);
                    r5 = *(u8*)((u8*)r4 + 0x0);
                    tmp = r3 + 0x1;
                    *(u32*)((u8*)r31 + 0x4) = tmp;
                    r4 = *(u32*)((u8*)r31 + 0x4);
                    r3 = *(u8*)((u8*)r3 + 0x0);
                    tmp = r4 + 0x1;
                    *(u32*)((u8*)r31 + 0x4) = tmp;
                    tmp = r3 << 8;
                    tmp = r5 | tmp;
                    *(u32*)(sp + 0x1C) = tmp;
                    r3 = *(u32*)((u8*)r31 + 0x4);
                    r4 = *(u8*)((u8*)r4 + 0x0);
                    tmp = r3 + 0x1;
                    r4 = r4 << 16;
                    *(u32*)((u8*)r31 + 0x4) = tmp;
                    tmp = r5 | r4;
                    *(u32*)(sp + 0x1C) = tmp;
                    tmp = *(u8*)((u8*)r3 + 0x0);
                    tmp = tmp << 24;
                    tmp = r3 | tmp;
                    *(u32*)(sp + 0x1C) = tmp;
                    f0 = *(f32*)(sp + 0x1C);

                } else {
                    r3 = r5 & 0x000000E0;
                    r4 = 0x1;
                    tmp = r5 & 0x1F;
                    tmp = r4 << tmp;
                                                        do {

                                                        if ((s32)r3 == 0x20) {

                                r5 = *(u32*)((u8*)r31 + 0x4);
                                r4 = 0x43300000;
                                r3 = *(u32*)((u8*)r31 + 0x4);
                                r5 = *(u8*)((u8*)r5 + 0x0);
                                r3 = r3 + 0x1;
                                r4 = (s8)r5;
                                f1 = lbl_8047DA58;
                                *(u32*)((u8*)r31 + 0x4) = r3;
                                f2 = f0 - f1;
                                                        } else if ((s32)r3 == 0x40) {

                            r5 = *(u32*)((u8*)r31 + 0x4);
                            r4 = 0x43300000;
                            r3 = *(u32*)((u8*)r31 + 0x4);
                            r5 = *(u8*)((u8*)r5 + 0x0);
                            r3 = r3 + 0x1;
                            f1 = lbl_8047DA50;
                            *(u32*)((u8*)r31 + 0x4) = r3;
                            f2 = f0 - f1;
                                                        } else if ((s32)r3 == 0x60) {

                        r3 = *(u32*)((u8*)r31 + 0x4);
                        r4 = 0x43300000;
                        r5 = *(u32*)((u8*)r31 + 0x4);
                        r6 = *(u8*)((u8*)r3 + 0x1);
                        r3 = *(u32*)((u8*)r31 + 0x4);
                        r6 = (s8)r6;
                        r5 = *(u8*)((u8*)r5 + 0x0);
                        r5 = (r5 & ~0xFFFFFF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0xFFFFFF00);
                        r3 = r3 + 0x2;
                        f1 = lbl_8047DA58;
                        *(u32*)((u8*)r31 + 0x4) = r3;
                        f2 = f0 - f1;
                                                        } else if ((s32)r3 == 0x80) {

                    r5 = *(u32*)((u8*)r31 + 0x4);
                    r4 = 0x43300000;
                    r3 = *(u32*)((u8*)r31 + 0x4);
                    r6 = *(u8*)((u8*)r5 + 0x1);
                    r5 = *(u8*)((u8*)r3 + 0x0);
                    r5 = (r5 & ~0x0000FF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0x0000FF00);
                    r3 = *(u32*)((u8*)r31 + 0x4);
                    r3 = r3 + 0x2;
                    f1 = lbl_8047DA58;
                    *(u32*)((u8*)r31 + 0x4) = r3;
                    f2 = f0 - f1;
                                                        } else {

                    f0 = lbl_8047DA3C;

                                                            break;

                                                        }

                    tmp = 0x43300000;
                    f1 = lbl_8047DA58;
                    *(u32*)(sp + 0x28) = tmp;
                    f0 = f0 - f1;
                    f0 = f2 / f0;
                                                        } while (0);

                                                    }

                *(f32*)((u8*)r31 + 0x24) = f0;
                tmp = *(u8*)((u8*)r31 + 0x12);
                if (tmp != 5) {
                    f1 = *(f32*)((u8*)r31 + 0x2C);
                    f0 = lbl_8047DA3C;
                    *(f32*)((u8*)r31 + 0x28) = f1;
                    *(f32*)((u8*)r31 + 0x2C) = f0;
                }
                tmp = 0x1;
                r4 = r30 - tmp;
                r3 = 0x4;
                tmp = tmp - r30;
                tmp = ~(r4 | tmp);
                tmp = (s32)tmp >> 31;
                r3 = tmp + r3;
                if (r31 == 0) return;
                tmp = *(u8*)((u8*)r31 + 0x10);
                tmp = tmp & 0x000000F0;
                tmp = (tmp & ~0x0000000F) | (((r3 << 0) | ((u32)r3 >> 32)) & 0x0000000F);
                *(u8*)((u8*)r31 + 0x10) = tmp;
                return;
                    }
            if (r31 == 0) {
                r30 = 0x0;
            } else {

                tmp = *(u8*)((u8*)r31 + 0x10);
                r30 = tmp & 0xF;
            }
            r3 = 0x1;
            tmp = r3 - r4;
            r5 = 0x1;
            r3 = r3 | ~r4;
            tmp = (u32)tmp >> 1;
            tmp = r3 - tmp;
            /* srwi. tmp, tmp, 31 */;
            if (r31 == 0) {
                r5 = 0x0;
            }
            if ((s32)r5 == 0) {
                r4 = (u32)lbl_8027477C;
                r3 = (u32)lbl_8047DA30;
                r5 = (u32)lbl_8027477C;
                r4 = 0x1a7;
                __assert();
            }
            f0 = *(f32*)((u8*)r31 + 0x24);
            *(f32*)((u8*)r31 + 0x20) = f0;
            f0 = *(f32*)((u8*)r31 + 0x2C);
            *(f32*)((u8*)r31 + 0x28) = f0;
            r5 = *(u8*)((u8*)r31 + 0x14);
            tmp = r5 & 0xFF;
            if ((s32)r5 == 0) {
                r4 = *(u32*)((u8*)r31 + 0x4);
                tmp = r4 + 0x1;
                *(u32*)((u8*)r31 + 0x4) = tmp;
                r3 = *(u32*)((u8*)r31 + 0x4);
                r5 = *(u8*)((u8*)r4 + 0x0);
                tmp = r3 + 0x1;
                *(u32*)((u8*)r31 + 0x4) = tmp;
                r4 = *(u32*)((u8*)r31 + 0x4);
                r3 = *(u8*)((u8*)r3 + 0x0);
                tmp = r4 + 0x1;
                *(u32*)((u8*)r31 + 0x4) = tmp;
                tmp = r3 << 8;
                tmp = r5 | tmp;
                *(u32*)(sp + 0x18) = tmp;
                r3 = *(u32*)((u8*)r31 + 0x4);
                r4 = *(u8*)((u8*)r4 + 0x0);
                tmp = r3 + 0x1;
                r4 = r4 << 16;
                *(u32*)((u8*)r31 + 0x4) = tmp;
                tmp = r5 | r4;
                *(u32*)(sp + 0x18) = tmp;
                tmp = *(u8*)((u8*)r3 + 0x0);
                tmp = tmp << 24;
                tmp = r3 | tmp;
                *(u32*)(sp + 0x18) = tmp;
                f0 = *(f32*)(sp + 0x18);

            } else {
                r3 = r5 & 0x000000E0;
                r4 = 0x1;
                tmp = r5 & 0x1F;
                tmp = r4 << tmp;
                                                    do {

                                                    if ((s32)r3 == 0x20) {

                            r5 = *(u32*)((u8*)r31 + 0x4);
                            r4 = 0x43300000;
                            r3 = *(u32*)((u8*)r31 + 0x4);
                            r5 = *(u8*)((u8*)r5 + 0x0);
                            r3 = r3 + 0x1;
                            r4 = (s8)r5;
                            f1 = lbl_8047DA58;
                            *(u32*)((u8*)r31 + 0x4) = r3;
                            f2 = f0 - f1;
                                                    } else if ((s32)r3 == 0x40) {

                        r5 = *(u32*)((u8*)r31 + 0x4);
                        r4 = 0x43300000;
                        r3 = *(u32*)((u8*)r31 + 0x4);
                        r5 = *(u8*)((u8*)r5 + 0x0);
                        r3 = r3 + 0x1;
                        f1 = lbl_8047DA50;
                        *(u32*)((u8*)r31 + 0x4) = r3;
                        f2 = f0 - f1;
                                                    } else if ((s32)r3 == 0x60) {

                    r3 = *(u32*)((u8*)r31 + 0x4);
                    r4 = 0x43300000;
                    r5 = *(u32*)((u8*)r31 + 0x4);
                    r6 = *(u8*)((u8*)r3 + 0x1);
                    r3 = *(u32*)((u8*)r31 + 0x4);
                    r6 = (s8)r6;
                    r5 = *(u8*)((u8*)r5 + 0x0);
                    r5 = (r5 & ~0xFFFFFF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0xFFFFFF00);
                    r3 = r3 + 0x2;
                    f1 = lbl_8047DA58;
                    *(u32*)((u8*)r31 + 0x4) = r3;
                    f2 = f0 - f1;
                                                    } else if ((s32)r3 == 0x80) {

                r5 = *(u32*)((u8*)r31 + 0x4);
                r4 = 0x43300000;
                r3 = *(u32*)((u8*)r31 + 0x4);
                r6 = *(u8*)((u8*)r5 + 0x1);
                r5 = *(u8*)((u8*)r3 + 0x0);
                r5 = (r5 & ~0x0000FF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0x0000FF00);
                r3 = *(u32*)((u8*)r31 + 0x4);
                r3 = r3 + 0x2;
                f1 = lbl_8047DA58;
                *(u32*)((u8*)r31 + 0x4) = r3;
                f2 = f0 - f1;
                                                    } else {

                f0 = lbl_8047DA3C;

                                                        break;

                                                    }

                tmp = 0x43300000;
                f1 = lbl_8047DA58;
                *(u32*)(sp + 0x28) = tmp;
                f0 = f0 - f1;
                f0 = f2 / f0;
                                                    } while (0);

                                                }

            tmp = 0x1;
            *(f32*)((u8*)r31 + 0x24) = f0;
            r3 = r30 - tmp;
            f0 = lbl_8047DA3C;
            tmp = tmp - r30;
            tmp = ~(r3 | tmp);
            r3 = 0x4;
            tmp = (s32)tmp >> 31;
            *(f32*)((u8*)r31 + 0x2C) = f0;
            r3 = tmp + r3;
            if (r31 == 0) return;
            tmp = *(u8*)((u8*)r31 + 0x10);
            tmp = tmp & 0x000000F0;
            tmp = (tmp & ~0x0000000F) | (((r3 << 0) | ((u32)r3 >> 32)) & 0x0000000F);
            *(u8*)((u8*)r31 + 0x10) = tmp;
            return;
        }
        if (r31 == 0) {
            r30 = 0x0;
        } else {

            tmp = *(u8*)((u8*)r31 + 0x10);
            r30 = tmp & 0xF;
        }
        r3 = 0x1;
        tmp = r3 - r4;
        r5 = 0x1;
        r3 = r3 | ~r4;
        tmp = (u32)tmp >> 1;
        tmp = r3 - tmp;
        /* srwi. tmp, tmp, 31 */;
        if (r31 == 0) {
            r5 = 0x0;
        }
        if ((s32)r5 == 0) {
            r4 = (u32)lbl_8027477C;
            r3 = (u32)lbl_8047DA30;
            r5 = (u32)lbl_8027477C;
            r4 = 0x1b9;
            __assert();
        }
        f0 = *(f32*)((u8*)r31 + 0x24);
        *(f32*)((u8*)r31 + 0x20) = f0;
        r5 = *(u8*)((u8*)r31 + 0x14);
        tmp = r5 & 0xFF;
        if ((s32)r5 == 0) {
            r4 = *(u32*)((u8*)r31 + 0x4);
            tmp = r4 + 0x1;
            *(u32*)((u8*)r31 + 0x4) = tmp;
            r3 = *(u32*)((u8*)r31 + 0x4);
            r5 = *(u8*)((u8*)r4 + 0x0);
            tmp = r3 + 0x1;
            *(u32*)((u8*)r31 + 0x4) = tmp;
            r4 = *(u32*)((u8*)r31 + 0x4);
            r3 = *(u8*)((u8*)r3 + 0x0);
            tmp = r4 + 0x1;
            *(u32*)((u8*)r31 + 0x4) = tmp;
            tmp = r3 << 8;
            tmp = r5 | tmp;
            *(u32*)(sp + 0x14) = tmp;
            r3 = *(u32*)((u8*)r31 + 0x4);
            r4 = *(u8*)((u8*)r4 + 0x0);
            tmp = r3 + 0x1;
            r4 = r4 << 16;
            *(u32*)((u8*)r31 + 0x4) = tmp;
            tmp = r5 | r4;
            *(u32*)(sp + 0x14) = tmp;
            tmp = *(u8*)((u8*)r3 + 0x0);
            tmp = tmp << 24;
            tmp = r3 | tmp;
            *(u32*)(sp + 0x14) = tmp;
            f0 = *(f32*)(sp + 0x14);

        } else {
            r3 = r5 & 0x000000E0;
            r4 = 0x1;
            tmp = r5 & 0x1F;
            tmp = r4 << tmp;
                                                do {

                                                if ((s32)r3 == 0x20) {

                        r5 = *(u32*)((u8*)r31 + 0x4);
                        r4 = 0x43300000;
                        r3 = *(u32*)((u8*)r31 + 0x4);
                        r5 = *(u8*)((u8*)r5 + 0x0);
                        r3 = r3 + 0x1;
                        r4 = (s8)r5;
                        f1 = lbl_8047DA58;
                        *(u32*)((u8*)r31 + 0x4) = r3;
                        f2 = f0 - f1;
                                                } else if ((s32)r3 == 0x40) {

                    r5 = *(u32*)((u8*)r31 + 0x4);
                    r4 = 0x43300000;
                    r3 = *(u32*)((u8*)r31 + 0x4);
                    r5 = *(u8*)((u8*)r5 + 0x0);
                    r3 = r3 + 0x1;
                    f1 = lbl_8047DA50;
                    *(u32*)((u8*)r31 + 0x4) = r3;
                    f2 = f0 - f1;
                                                } else if ((s32)r3 == 0x60) {

                r3 = *(u32*)((u8*)r31 + 0x4);
                r4 = 0x43300000;
                r5 = *(u32*)((u8*)r31 + 0x4);
                r6 = *(u8*)((u8*)r3 + 0x1);
                r3 = *(u32*)((u8*)r31 + 0x4);
                r6 = (s8)r6;
                r5 = *(u8*)((u8*)r5 + 0x0);
                r5 = (r5 & ~0xFFFFFF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0xFFFFFF00);
                r3 = r3 + 0x2;
                f1 = lbl_8047DA58;
                *(u32*)((u8*)r31 + 0x4) = r3;
                f2 = f0 - f1;
                                                } else if ((s32)r3 == 0x80) {

            r5 = *(u32*)((u8*)r31 + 0x4);
            r4 = 0x43300000;
            r3 = *(u32*)((u8*)r31 + 0x4);
            r6 = *(u8*)((u8*)r5 + 0x1);
            r5 = *(u8*)((u8*)r3 + 0x0);
            r5 = (r5 & ~0x0000FF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0x0000FF00);
            r3 = *(u32*)((u8*)r31 + 0x4);
            r3 = r3 + 0x2;
            f1 = lbl_8047DA58;
            *(u32*)((u8*)r31 + 0x4) = r3;
            f2 = f0 - f1;
                                                } else {

            f0 = lbl_8047DA3C;

                                                    break;

                                                }

            tmp = 0x43300000;
            f1 = lbl_8047DA58;
            *(u32*)(sp + 0x28) = tmp;
            f0 = f0 - f1;
            f0 = f2 / f0;
                                                } while (0);

                                            }

        *(f32*)((u8*)r31 + 0x24) = f0;
        f0 = *(f32*)((u8*)r31 + 0x2C);
        *(f32*)((u8*)r31 + 0x28) = f0;
        r5 = *(u8*)((u8*)r31 + 0x15);
        tmp = r5 & 0xFF;
        if ((s32)r3 == 0x80) {
            r4 = *(u32*)((u8*)r31 + 0x4);
            tmp = r4 + 0x1;
            *(u32*)((u8*)r31 + 0x4) = tmp;
            r3 = *(u32*)((u8*)r31 + 0x4);
            r5 = *(u8*)((u8*)r4 + 0x0);
            tmp = r3 + 0x1;
            *(u32*)((u8*)r31 + 0x4) = tmp;
            r4 = *(u32*)((u8*)r31 + 0x4);
            r3 = *(u8*)((u8*)r3 + 0x0);
            tmp = r4 + 0x1;
            *(u32*)((u8*)r31 + 0x4) = tmp;
            tmp = r3 << 8;
            tmp = r5 | tmp;
            *(u32*)(sp + 0x10) = tmp;
            r3 = *(u32*)((u8*)r31 + 0x4);
            r4 = *(u8*)((u8*)r4 + 0x0);
            tmp = r3 + 0x1;
            r4 = r4 << 16;
            *(u32*)((u8*)r31 + 0x4) = tmp;
            tmp = r5 | r4;
            *(u32*)(sp + 0x10) = tmp;
            tmp = *(u8*)((u8*)r3 + 0x0);
            tmp = tmp << 24;
            tmp = r3 | tmp;
            *(u32*)(sp + 0x10) = tmp;
            f0 = *(f32*)(sp + 0x10);

        } else {
            r3 = r5 & 0x000000E0;
            r4 = 0x1;
            tmp = r5 & 0x1F;
            tmp = r4 << tmp;
                                                do {

                                                if ((s32)r3 == 0x20) {

                        r5 = *(u32*)((u8*)r31 + 0x4);
                        r4 = 0x43300000;
                        r3 = *(u32*)((u8*)r31 + 0x4);
                        r5 = *(u8*)((u8*)r5 + 0x0);
                        r3 = r3 + 0x1;
                        r4 = (s8)r5;
                        f1 = lbl_8047DA58;
                        *(u32*)((u8*)r31 + 0x4) = r3;
                        f2 = f0 - f1;
                                                } else if ((s32)r3 == 0x40) {

                    r5 = *(u32*)((u8*)r31 + 0x4);
                    r4 = 0x43300000;
                    r3 = *(u32*)((u8*)r31 + 0x4);
                    r5 = *(u8*)((u8*)r5 + 0x0);
                    r3 = r3 + 0x1;
                    f1 = lbl_8047DA50;
                    *(u32*)((u8*)r31 + 0x4) = r3;
                    f2 = f0 - f1;
                                                } else if ((s32)r3 == 0x60) {

                r3 = *(u32*)((u8*)r31 + 0x4);
                r4 = 0x43300000;
                r5 = *(u32*)((u8*)r31 + 0x4);
                r6 = *(u8*)((u8*)r3 + 0x1);
                r3 = *(u32*)((u8*)r31 + 0x4);
                r6 = (s8)r6;
                r5 = *(u8*)((u8*)r5 + 0x0);
                r5 = (r5 & ~0xFFFFFF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0xFFFFFF00);
                r3 = r3 + 0x2;
                f1 = lbl_8047DA58;
                *(u32*)((u8*)r31 + 0x4) = r3;
                f2 = f0 - f1;
                                                } else if ((s32)r3 == 0x80) {

            r5 = *(u32*)((u8*)r31 + 0x4);
            r4 = 0x43300000;
            r3 = *(u32*)((u8*)r31 + 0x4);
            r6 = *(u8*)((u8*)r5 + 0x1);
            r5 = *(u8*)((u8*)r3 + 0x0);
            r5 = (r5 & ~0x0000FF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0x0000FF00);
            r3 = *(u32*)((u8*)r31 + 0x4);
            r3 = r3 + 0x2;
            f1 = lbl_8047DA58;
            *(u32*)((u8*)r31 + 0x4) = r3;
            f2 = f0 - f1;
                                                } else {

            f0 = lbl_8047DA3C;

                                                    break;

                                                }

            tmp = 0x43300000;
            f1 = lbl_8047DA58;
            *(u32*)(sp + 0x28) = tmp;
            f0 = f0 - f1;
            f0 = f2 / f0;
                                                } while (0);

                                            }

        tmp = 0x1;
        r4 = r30 - tmp;
        r3 = 0x4;
        tmp = tmp - r30;
        *(f32*)((u8*)r31 + 0x2C) = f0;
        tmp = ~(r4 | tmp);
        tmp = (s32)tmp >> 31;
        r3 = tmp + r3;
        if (r31 == 0) return;
        tmp = *(u8*)((u8*)r31 + 0x10);
        tmp = tmp & 0x000000F0;
        tmp = (tmp & ~0x0000000F) | (((r3 << 0) | ((u32)r3 >> 32)) & 0x0000000F);
        *(u8*)((u8*)r31 + 0x10) = tmp;
        return;
    L_8019AF60:
        if (r31 == 0) {
            r3 = 0x0;
        } else {

            tmp = *(u8*)((u8*)r31 + 0x10);
            r3 = tmp & 0xF;
        }
        r3 = 0x1;
        tmp = r3 - r4;
        r5 = 0x1;
        r3 = r3 | ~r4;
        tmp = (u32)tmp >> 1;
        tmp = r3 - tmp;
        /* srwi. tmp, tmp, 31 */;
        if (r31 == 0) {
            r5 = 0x0;
        }
        if ((s32)r5 == 0) {
            r4 = (u32)lbl_8027477C;
            r3 = (u32)lbl_8047DA30;
            r5 = (u32)lbl_8027477C;
            r4 = 0x1cc;
            __assert();
        }
        f0 = *(f32*)((u8*)r31 + 0x2C);
        *(f32*)((u8*)r31 + 0x28) = f0;
        r5 = *(u8*)((u8*)r31 + 0x15);
        tmp = r5 & 0xFF;
        if ((s32)r5 == 0) {
            r4 = *(u32*)((u8*)r31 + 0x4);
            tmp = r4 + 0x1;
            *(u32*)((u8*)r31 + 0x4) = tmp;
            r3 = *(u32*)((u8*)r31 + 0x4);
            r5 = *(u8*)((u8*)r4 + 0x0);
            tmp = r3 + 0x1;
            *(u32*)((u8*)r31 + 0x4) = tmp;
            r4 = *(u32*)((u8*)r31 + 0x4);
            r3 = *(u8*)((u8*)r3 + 0x0);
            tmp = r4 + 0x1;
            *(u32*)((u8*)r31 + 0x4) = tmp;
            tmp = r3 << 8;
            tmp = r5 | tmp;
            *(u32*)(sp + 0xC) = tmp;
            r3 = *(u32*)((u8*)r31 + 0x4);
            r4 = *(u8*)((u8*)r4 + 0x0);
            tmp = r3 + 0x1;
            r4 = r4 << 16;
            *(u32*)((u8*)r31 + 0x4) = tmp;
            tmp = r5 | r4;
            *(u32*)(sp + 0xC) = tmp;
            tmp = *(u8*)((u8*)r3 + 0x0);
            tmp = tmp << 24;
            tmp = r3 | tmp;
            *(u32*)(sp + 0xC) = tmp;
            f0 = *(f32*)(sp + 0xC);

        } else {
            r3 = r5 & 0x000000E0;
            r4 = 0x1;
            tmp = r5 & 0x1F;
            tmp = r4 << tmp;
                                                do {

                                                if ((s32)r3 == 0x20) {

                        r5 = *(u32*)((u8*)r31 + 0x4);
                        r4 = 0x43300000;
                        r3 = *(u32*)((u8*)r31 + 0x4);
                        r5 = *(u8*)((u8*)r5 + 0x0);
                        r3 = r3 + 0x1;
                        r4 = (s8)r5;
                        f1 = lbl_8047DA58;
                        *(u32*)((u8*)r31 + 0x4) = r3;
                        f2 = f0 - f1;
                                                } else if ((s32)r3 == 0x40) {

                    r5 = *(u32*)((u8*)r31 + 0x4);
                    r4 = 0x43300000;
                    r3 = *(u32*)((u8*)r31 + 0x4);
                    r5 = *(u8*)((u8*)r5 + 0x0);
                    r3 = r3 + 0x1;
                    f1 = lbl_8047DA50;
                    *(u32*)((u8*)r31 + 0x4) = r3;
                    f2 = f0 - f1;
                                                } else if ((s32)r3 == 0x60) {

                r3 = *(u32*)((u8*)r31 + 0x4);
                r4 = 0x43300000;
                r5 = *(u32*)((u8*)r31 + 0x4);
                r6 = *(u8*)((u8*)r3 + 0x1);
                r3 = *(u32*)((u8*)r31 + 0x4);
                r6 = (s8)r6;
                r5 = *(u8*)((u8*)r5 + 0x0);
                r5 = (r5 & ~0xFFFFFF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0xFFFFFF00);
                r3 = r3 + 0x2;
                f1 = lbl_8047DA58;
                *(u32*)((u8*)r31 + 0x4) = r3;
                f2 = f0 - f1;
                                                } else if ((s32)r3 == 0x80) {

            r5 = *(u32*)((u8*)r31 + 0x4);
            r4 = 0x43300000;
            r3 = *(u32*)((u8*)r31 + 0x4);
            r6 = *(u8*)((u8*)r5 + 0x1);
            r5 = *(u8*)((u8*)r3 + 0x0);
            r5 = (r5 & ~0x0000FF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0x0000FF00);
            r3 = *(u32*)((u8*)r31 + 0x4);
            r3 = r3 + 0x2;
            f1 = lbl_8047DA58;
            *(u32*)((u8*)r31 + 0x4) = r3;
            f2 = f0 - f1;
                                                } else {

            f0 = lbl_8047DA3C;

                                                    break;

                                                }

            tmp = 0x43300000;
            f1 = lbl_8047DA58;
            *(u32*)(sp + 0x28) = tmp;
            f0 = f0 - f1;
            f0 = f2 / f0;
                                                } while (0);

                                            }

        *(f32*)((u8*)r31 + 0x2C) = f0;
        if (r31 == 0) {
            r3 = 0x0;
            return;
        }
        tmp = *(u8*)((u8*)r31 + 0x10);
        r3 = tmp & 0xF;
        return;
                    }
    if (r31 == 0) {
        r30 = 0x0;
    } else {

        tmp = *(u8*)((u8*)r31 + 0x10);
        r30 = tmp & 0xF;
    }
    r3 = 0x1;
    tmp = r3 - r4;
    r5 = 0x1;
    r3 = r3 | ~r4;
    tmp = (u32)tmp >> 1;
    tmp = r3 - tmp;
    /* srwi. tmp, tmp, 31 */;
    if (r31 == 0) {
        r5 = 0x0;
    }
    if ((s32)r5 == 0) {
        r4 = (u32)lbl_8027477C;
        r3 = (u32)lbl_8047DA30;
        r5 = (u32)lbl_8027477C;
        r4 = 0x1e9;
        __assert();
    }
    tmp = *(u8*)((u8*)r31 + 0x10);
    tmp = tmp & 0x00000040;
    if ((s32)r5 != 0) {
        tmp = *(u8*)((u8*)r31 + 0x11);
        *(u8*)((u8*)r31 + 0x12) = tmp;
        tmp = *(u8*)((u8*)r31 + 0x10);
        tmp = tmp & 0xFFFFFFBF;
        *(u8*)((u8*)r31 + 0x10) = tmp;
        tmp = *(u8*)((u8*)r31 + 0x10);
        tmp = tmp | 0x80;
        *(u8*)((u8*)r31 + 0x10) = tmp;
        f0 = *(f32*)((u8*)r31 + 0x24);
        *(f32*)((u8*)r31 + 0x20) = f0;
    }
    r5 = *(u8*)((u8*)r31 + 0x14);
    tmp = r5 & 0xFF;
    if ((s32)r5 == 0) {
        r4 = *(u32*)((u8*)r31 + 0x4);
        tmp = r4 + 0x1;
        *(u32*)((u8*)r31 + 0x4) = tmp;
        r3 = *(u32*)((u8*)r31 + 0x4);
        r5 = *(u8*)((u8*)r4 + 0x0);
        tmp = r3 + 0x1;
        *(u32*)((u8*)r31 + 0x4) = tmp;
        r4 = *(u32*)((u8*)r31 + 0x4);
        r3 = *(u8*)((u8*)r3 + 0x0);
        tmp = r4 + 0x1;
        *(u32*)((u8*)r31 + 0x4) = tmp;
        tmp = r3 << 8;
        tmp = r5 | tmp;
        *(u32*)(sp + 0x8) = tmp;
        r3 = *(u32*)((u8*)r31 + 0x4);
        r4 = *(u8*)((u8*)r4 + 0x0);
        tmp = r3 + 0x1;
        r4 = r4 << 16;
        *(u32*)((u8*)r31 + 0x4) = tmp;
        tmp = r5 | r4;
        *(u32*)(sp + 0x8) = tmp;
        tmp = *(u8*)((u8*)r3 + 0x0);
        tmp = tmp << 24;
        tmp = r3 | tmp;
        *(u32*)(sp + 0x8) = tmp;
        f0 = *(f32*)(sp + 0x8);

    } else {
        r3 = r5 & 0x000000E0;
        r4 = 0x1;
        tmp = r5 & 0x1F;
        tmp = r4 << tmp;
                                            do {

                                            if ((s32)r3 == 0x20) {

                    r5 = *(u32*)((u8*)r31 + 0x4);
                    r4 = 0x43300000;
                    r3 = *(u32*)((u8*)r31 + 0x4);
                    r5 = *(u8*)((u8*)r5 + 0x0);
                    r3 = r3 + 0x1;
                    r4 = (s8)r5;
                    f1 = lbl_8047DA58;
                    *(u32*)((u8*)r31 + 0x4) = r3;
                    f2 = f0 - f1;
                                            } else if ((s32)r3 == 0x40) {

                r5 = *(u32*)((u8*)r31 + 0x4);
                r4 = 0x43300000;
                r3 = *(u32*)((u8*)r31 + 0x4);
                r5 = *(u8*)((u8*)r5 + 0x0);
                r3 = r3 + 0x1;
                f1 = lbl_8047DA50;
                *(u32*)((u8*)r31 + 0x4) = r3;
                f2 = f0 - f1;
                                            } else if ((s32)r3 == 0x60) {

            r3 = *(u32*)((u8*)r31 + 0x4);
            r4 = 0x43300000;
            r5 = *(u32*)((u8*)r31 + 0x4);
            r6 = *(u8*)((u8*)r3 + 0x1);
            r3 = *(u32*)((u8*)r31 + 0x4);
            r6 = (s8)r6;
            r5 = *(u8*)((u8*)r5 + 0x0);
            r5 = (r5 & ~0xFFFFFF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0xFFFFFF00);
            r3 = r3 + 0x2;
            f1 = lbl_8047DA58;
            *(u32*)((u8*)r31 + 0x4) = r3;
            f2 = f0 - f1;
                                            } else if ((s32)r3 == 0x80) {

        r5 = *(u32*)((u8*)r31 + 0x4);
        r4 = 0x43300000;
        r3 = *(u32*)((u8*)r31 + 0x4);
        r6 = *(u8*)((u8*)r5 + 0x1);
        r5 = *(u8*)((u8*)r3 + 0x0);
        r5 = (r5 & ~0x0000FF00) | (((r6 << 8) | ((u32)r6 >> 24)) & 0x0000FF00);
        r3 = *(u32*)((u8*)r31 + 0x4);
        r3 = r3 + 0x2;
        f1 = lbl_8047DA58;
        *(u32*)((u8*)r31 + 0x4) = r3;
        f2 = f0 - f1;
                                            } else {

        f0 = lbl_8047DA3C;

                                                break;

                                            }

        tmp = 0x43300000;
        f1 = lbl_8047DA58;
        *(u32*)(sp + 0x28) = tmp;
        f0 = f0 - f1;
        f0 = f2 / f0;
                                            } while (0);

                                        }

    *(f32*)((u8*)r31 + 0x24) = f0;
    tmp = 0x1;
    r3 = r30 - tmp;
    r4 = *(u8*)((u8*)r31 + 0x10);
    tmp = tmp - r30;
    tmp = ~(r3 | tmp);
    r3 = 0x4;
    r4 = r4 | 0x40;
    tmp = (s32)tmp >> 31;
    *(u8*)((u8*)r31 + 0x10) = r4;
    r3 = tmp + r3;
    if (r31 == 0) return;
    tmp = *(u8*)((u8*)r31 + 0x10);
    tmp = tmp & 0x000000F0;
    tmp = (tmp & ~0x0000000F) | (((r3 << 0) | ((u32)r3 >> 32)) & 0x0000000F);
    *(u8*)((u8*)r31 + 0x10) = tmp;
    return;

    r3 = 0x0;

    return;
}
