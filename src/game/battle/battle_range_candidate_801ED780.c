/**
 * @file battle_range_candidate_801ED780.c
 * @brief Residual battle resource runtime candidate.
 *
 * Address range: 0x801ED780 - 0x801EE034.
 */

#include "dolphin/types.h"

#define FIELD_U8(base, off) (*(u8*)((u8*)(base) + (off)))
#define FIELD_U16(base, off) (*(u16*)((u8*)(base) + (off)))
#define FIELD_U32(base, off) (*(u32*)((u8*)(base) + (off)))
#define FIELD_F32(base, off) (*(f32*)((u8*)(base) + (off)))

extern u8 lbl_80314958[];
extern u8 lbl_80314C78[];
extern u8 lbl_80375230[];
extern u8 lbl_8046D630[];
extern u8 lbl_8047B5C0;
extern u8 lbl_8047B5C1;
extern u32 lbl_8047B5C4;
extern u32 lbl_8047B5C8;
extern f32 lbl_8047E4D0;
extern f32 lbl_8047E4D4;
extern f32 lbl_8047E4D8;
extern f32 lbl_8047E4DC;
extern f32 lbl_8047E4E0;
extern f32 lbl_8047E4E4;
extern f32 lbl_8047E4E8;
extern f32 lbl_8047E4EC;
extern f32 lbl_8047E4F0;
extern f64 lbl_8047E4F8;
extern f64 lbl_8047E500;

extern void* GScameraGetActiveCamera(void);
extern void GScameraGetPosition(void*, void*);
extern s32 GScolsys2Sun(void*, void*);
extern void* fn_800D2F34(void*, void*);
extern f32 fn_800E008C(void*);
extern void fn_800D88DC(u32);
extern void fn_800D888C(u32);
extern void fn_800D9B58(f32, f32, f32, f32);
extern void fn_800DA4C4(u32, u32, u32);
extern void fn_800DA2BC(u32, u32, u32);
extern void fn_800DA1E8(u32, u32, u32);
extern void fn_800DA028(u32);
extern void fn_800D9ED8(u32);
extern void fn_800E00AC(void*, void*, f32);
extern u32 fn_800EF4FC(void*);
extern u32 fn_800EF4F4(void*);
extern void fn_800E013C(void*, void*, f32);
extern void fn_800D85D4(u32, void*);
extern void fn_800D67BC(u32);
extern void fn_800D6680(f32, f32, f32);
extern void fn_800D5C18(u32, u32, u32, u32);
extern void fn_800D59B8(f32, f32);
extern void fn_800D6728(void);
extern void fn_800D6A00(u32);
extern void fn_800D7820(void*);

void fn_801ED780(void)
{
    f32 camera_pos[3];
    f32 col_pos[3];
    f32 delta[3];
    f32 span_ratio;
    f32 distance;
    f32 x0;
    f32 y0;
    f32 x1;
    f32 y1;
    u32 alpha;
    u32 task;
    u8* entry_list;
    u32* tex_table;
    u16 count;
    s32 mode;

    if (lbl_8047B5C0 == 0 || lbl_8047B5C1 == 0) {
        return;
    }

    task = (u32)GScameraGetActiveCamera();
    if (task == 0) {
        return;
    }

    GScameraGetPosition((void*)task, camera_pos);
    mode = GScolsys2Sun(lbl_80375230, camera_pos);
    if (mode == 1) {
        return;
    }

    mode = (s32)fn_800D2F34(lbl_80375230, col_pos);
    if (mode == 0) {
        return;
    }

    delta[0] = lbl_8047E4D0 - col_pos[0];
    delta[1] = lbl_8047E4D4 - col_pos[1];
    delta[2] = lbl_8047E4D8;
    distance = fn_800E008C(delta);
    if ((distance > lbl_8047E4D8 ? distance : -distance) < lbl_8047E4DC) {
        return;
    }

    if (distance > (f32)FIELD_U32((void*)lbl_8047B5C4, 0x08)) {
        return;
    }

    fn_800D88DC(3);
    fn_800D888C(4);
    fn_800D9B58(lbl_8047E4D8, lbl_8047E4D8, lbl_8047E4E0, lbl_8047E4E4);
    fn_800DA4C4(1, 1, 1);
    fn_800DA2BC(1, 1, 0);
    fn_800DA1E8(0, 1, 1);
    fn_800DA028(0);
    fn_800D9ED8(1);

    if (mode == 2) {
        fn_800E00AC(delta, delta, distance);

        if (distance < (f32)FIELD_U32((void*)lbl_8047B5C4, 0x0C)) {
            span_ratio = lbl_8047E4E8;
        } else {
            f32 near_z = (f32)FIELD_U32((void*)lbl_8047B5C4, 0x0C);
            f32 far_z = (f32)FIELD_U32((void*)lbl_8047B5C4, 0x08);
            f32 range = far_z - near_z;

            span_ratio = (range - (distance - near_z)) / range;
            if (span_ratio < lbl_8047E4D8) {
                span_ratio = lbl_8047E4D8;
            } else if (span_ratio > lbl_8047E4E8) {
                span_ratio = lbl_8047E4E8;
            }
        }

        alpha = (u32)(lbl_8047E4EC * span_ratio);
        fn_800D6A00(4);
        fn_800D7820(lbl_80314C78);

        entry_list = (u8*)lbl_8047B5C8;
        tex_table = (u32*)lbl_8046D630;
        count = FIELD_U16((void*)lbl_8047B5C4, 0x06);

        while (count-- != 0) {
            void* tex = (void*)tex_table[(u32)FIELD_U16(entry_list, 0x02)];

            if (FIELD_U8(entry_list, 0x00) == 1) {
                x0 = col_pos[0] - FIELD_F32(entry_list, 0x08) * (f32)fn_800EF4FC(tex);
                y0 = col_pos[1] - FIELD_F32(entry_list, 0x08) * (f32)fn_800EF4F4(tex);
                x1 = col_pos[0] + FIELD_F32(entry_list, 0x08) * (f32)fn_800EF4FC(tex);
                y1 = col_pos[1] + FIELD_F32(entry_list, 0x08) * (f32)fn_800EF4F4(tex);
            } else {
                fn_800E013C(&delta[0], &delta[0], lbl_8047E4F0 * FIELD_F32(entry_list, 0x04) * distance);
                x0 = lbl_8047E4D0 + delta[0] - FIELD_F32(entry_list, 0x08) * (f32)fn_800EF4FC(tex);
                y0 = lbl_8047E4D4 + delta[1] - FIELD_F32(entry_list, 0x08) * (f32)fn_800EF4F4(tex);
                x1 = lbl_8047E4D0 + delta[0] + FIELD_F32(entry_list, 0x08) * (f32)fn_800EF4FC(tex);
                y1 = lbl_8047E4D4 + delta[1] + FIELD_F32(entry_list, 0x08) * (f32)fn_800EF4F4(tex);
            }

            fn_800D85D4(0, tex);
            fn_800D67BC(4);

            fn_800D6680(x0, y0, lbl_8047E4D8);
            fn_800D5C18(0, alpha & 0xFF, alpha & 0xFF, alpha & 0xFF);
            fn_800D59B8(lbl_8047E4D8, lbl_8047E4D8);

            fn_800D6680(x1, y0, lbl_8047E4D8);
            fn_800D5C18(0, alpha & 0xFF, alpha & 0xFF, alpha & 0xFF);
            fn_800D59B8(lbl_8047E4E8, lbl_8047E4D8);

            fn_800D6680(x0, y1, lbl_8047E4D8);
            fn_800D5C18(0, alpha & 0xFF, alpha & 0xFF, alpha & 0xFF);
            fn_800D59B8(lbl_8047E4D8, lbl_8047E4E8);

            fn_800D6680(x1, y1, lbl_8047E4D8);
            fn_800D5C18(0, alpha & 0xFF, alpha & 0xFF, alpha & 0xFF);
            fn_800D59B8(lbl_8047E4E8, lbl_8047E4E8);
            fn_800D6728();

            entry_list += 0x0C;
        }

        if (distance >= (f32)FIELD_U32((void*)lbl_8047B5C4, 0x0C)) {
            return;
        }

        if (distance < (f32)FIELD_U32((void*)lbl_8047B5C4, 0x10)) {
            span_ratio = FIELD_F32((void*)lbl_8047B5C4, 0x14);
        } else {
            f32 near_z = (f32)FIELD_U32((void*)lbl_8047B5C4, 0x10);
            f32 far_z = (f32)FIELD_U32((void*)lbl_8047B5C4, 0x0C);
            f32 range = far_z - near_z;

            span_ratio = FIELD_F32((void*)lbl_8047B5C4, 0x14) * ((range - (distance - near_z)) / range);
            if (span_ratio < lbl_8047E4D8) {
                span_ratio = lbl_8047E4D8;
            } else if (span_ratio > lbl_8047E4E8) {
                span_ratio = lbl_8047E4E8;
            }
        }

        alpha = (u32)(lbl_8047E4EC * span_ratio);
        fn_800D888C(2);
        fn_800D6A00(4);
        fn_800D7820(lbl_80314958);
        fn_800D67BC(4);

        fn_800D6680(lbl_8047E4D8, lbl_8047E4D8, lbl_8047E4D8);
        fn_800D5C18(0, alpha & 0xFF, alpha & 0xFF, alpha & 0xFF);
        fn_800D6680(lbl_8047E4E0, lbl_8047E4D8, lbl_8047E4D8);
        fn_800D5C18(0, alpha & 0xFF, alpha & 0xFF, alpha & 0xFF);
        fn_800D6680(lbl_8047E4D8, lbl_8047E4E4, lbl_8047E4D8);
        fn_800D5C18(0, alpha & 0xFF, alpha & 0xFF, alpha & 0xFF);
        fn_800D6680(lbl_8047E4E0, lbl_8047E4E4, lbl_8047E4D8);
        fn_800D5C18(0, alpha & 0xFF, alpha & 0xFF, alpha & 0xFF);
        fn_800D6728();
        return;
    }

    fn_800DA4C4(2, 1, 1);

    if (distance >= (f32)FIELD_U32((void*)lbl_8047B5C4, 0x0C)) {
        return;
    }

    if (distance < (f32)FIELD_U32((void*)lbl_8047B5C4, 0x10)) {
        span_ratio = FIELD_F32((void*)lbl_8047B5C4, 0x18);
    } else {
        f32 near_z = (f32)FIELD_U32((void*)lbl_8047B5C4, 0x10);
        f32 far_z = (f32)FIELD_U32((void*)lbl_8047B5C4, 0x0C);
        f32 range = far_z - near_z;

        span_ratio = FIELD_F32((void*)lbl_8047B5C4, 0x18) * ((range - (distance - near_z)) / range);
        if (span_ratio < lbl_8047E4D8) {
            span_ratio = lbl_8047E4D8;
        } else if (span_ratio > lbl_8047E4E8) {
            span_ratio = lbl_8047E4E8;
        }
    }

    alpha = (u32)(lbl_8047E4EC * span_ratio);
    fn_800D888C(2);
    fn_800D6A00(4);
    fn_800D7820(lbl_80314958);
    fn_800D67BC(4);

    fn_800D6680(lbl_8047E4D8, lbl_8047E4D8, lbl_8047E4D8);
    fn_800D5C18(0, alpha & 0xFF, alpha & 0xFF, alpha & 0xFF);
    fn_800D6680(lbl_8047E4E0, lbl_8047E4D8, lbl_8047E4D8);
    fn_800D5C18(0, alpha & 0xFF, alpha & 0xFF, alpha & 0xFF);
    fn_800D6680(lbl_8047E4D8, lbl_8047E4E4, lbl_8047E4D8);
    fn_800D5C18(0, alpha & 0xFF, alpha & 0xFF, alpha & 0xFF);
    fn_800D6680(lbl_8047E4E0, lbl_8047E4E4, lbl_8047E4D8);
    fn_800D5C18(0, alpha & 0xFF, alpha & 0xFF, alpha & 0xFF);
    fn_800D6728();
}
