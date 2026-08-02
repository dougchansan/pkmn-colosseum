#include "dolphin/types.h"
#include "hsd/hsd_pobj.h"

extern char lbl_8047DCB8;
extern char lbl_8027503C[];
extern char lbl_8027506C[];
extern char lbl_80275084[];
extern char lbl_802750B4[];

void get_shape_normal_xyz(HSD_ShapeSet* shape_set, s32 shape_id, s32 arrayidx,
                          f32 dst[3])
{
    extern void __assert(const char*, s32, const char*);
    extern void HSD_Panic(const char*, s32, const char*);
    extern void* memcpy(void* dst, const void* src, u32 size);

    u8* index_array = shape_set->normal_idx_list[shape_id];
    s32 idx;
    void* src_base;

    if ((s32) shape_set->normal_desc->attr_type == 3) {
        idx = index_array[arrayidx * 2];
        idx = (idx << 8) + index_array[arrayidx * 2 + 1];
    } else {
        idx = index_array[arrayidx];
    }

    if ((s32) shape_set->normal_desc->comp_cnt != 0) {
        __assert(&lbl_8047DCB8, 1145, lbl_8027503C);
    }

    src_base = ((u8*) shape_set->normal_desc->vertex) +
               idx * shape_set->normal_desc->stride;

    if ((s32) shape_set->normal_desc->comp_type == 4) {
        memcpy(dst, src_base, sizeof(f32[3]));
    } else {
        s32 decimal_point = 1 << shape_set->normal_desc->frac;
        switch ((s32) shape_set->normal_desc->comp_type) {
        case 0: {
            u8* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        case 1: {
            s8* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        case 2: {
            u16* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        case 3: {
            s16* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        default:
            HSD_Panic(&lbl_8047DCB8, 1188, lbl_8027506C);
        }
    }
}

void get_shape_vertex_xyz(HSD_ShapeSet* shape_set, s32 shape_id, s32 arrayidx,
                          f32 dst[3])
{
    extern void __assert(const char*, s32, const char*);
    extern void HSD_Panic(const char*, s32, const char*);
    extern void* memcpy(void* dst, const void* src, u32 size);

    u8* index_array = shape_set->vertex_idx_list[shape_id];
    s32 idx;
    void* src_base;

    if ((s32) shape_set->vertex_desc->attr_type == 3) {
        idx = index_array[arrayidx * 2];
        idx = (idx << 8) + index_array[arrayidx * 2 + 1];
    } else {
        idx = index_array[arrayidx];
    }

    if ((s32) shape_set->vertex_desc->comp_cnt != 1) {
        __assert(&lbl_8047DCB8, 1082, lbl_80275084);
    }

    src_base = ((u8*) shape_set->vertex_desc->vertex) +
               idx * shape_set->vertex_desc->stride;

    if ((s32) shape_set->vertex_desc->comp_type == 4) {
        memcpy(dst, src_base, sizeof(f32[3]));
    } else {
        s32 decimal_point = 1 << shape_set->vertex_desc->frac;
        switch ((s32) shape_set->vertex_desc->comp_type) {
        case 0: {
            u8* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        case 1: {
            s8* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        case 2: {
            u16* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        case 3: {
            s16* src = src_base;
            dst[0] = (f32) src[0] / decimal_point;
            dst[1] = (f32) src[1] / decimal_point;
            dst[2] = (f32) src[2] / decimal_point;
            break;
        }
        default:
            HSD_Panic(&lbl_8047DCB8, 1125, lbl_802750B4);
        }
    }
}
