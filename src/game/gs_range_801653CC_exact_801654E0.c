/**
 * @file gs_range_801653CC_exact_801654E0.c
 * @brief Byte-exact gs-engine range, 0x801654E0 - 0x80165D0C.
 */
#include "game/gs_range_801653CC_shared.h"

u32 fn_801654E0(u32 id, void* buffer, u32 size)
{
    void* wave;
    u32 waveIndex = lbl_80478FAC[id].waveIndex;

    if (waveIndex < *lbl_80478E30) {
        wave = lbl_80478E34[waveIndex].data;
    } else {
        wave = 0;
    }

    if (wave == 0) {
        return 0;
    }
    fn_80165EE0(wave, buffer, size);
    return 1;
}

#pragma dont_inline on
void fn_80165548(u32 id)
{
    if (id != (u32)-1) {
        fn_801667D8(id, 0x3E8, 0);
    }
}

u32 fn_8016557C(void)
{
    u32 id = fn_801662E8(0, 0x406);

    if (id != (u32)-1) {
        fn_801668DC(id, 0x32, 0);
        fn_80165D0C(0x32);
    }
    return id;
}
#pragma dont_inline reset

void fn_801655D4(u32 index)
{
    GSsndDvdWave* wave;

    if (*lbl_80478FB0 <= index) {
        GSlogWrite(lbl_80273548, index);
    }

    wave = &((GSsndDvdWave*)lbl_80478FB4)[index];
    if (!GSsndOpenWaveDVD(index, wave->fileId, 0, wave->unk4, wave->unk8, wave->unkC, wave->unk10)) {
        GSlogWrite(lbl_80273568, wave->fileId);
    }
}

u32 fn_80165668(u32 id, void* buffer, u32 size)
{
    u32 savedId = fn_8016557C();
    u32 result = fn_801653CC(id, buffer, size);

    fn_80165548(savedId);
    return result;
}

u32 fn_801656D8(u32 id)
{
    return fn_80166098(id);
}

u32 fn_801656F8(u32 id, u32 arg1, u32 arg2)
{
    return fn_80166670(id, arg1, arg2);
}

u32 fn_80165718(u32 volume, u32 fade)
{
    fn_80166D48(volume, fade, 1, 0);
    return 1;
}

u32 fn_80165744(void)
{
    GSvec position;
    u32 result;

    set__5GSvecFfff(&position, lbl_8047D540, lbl_8047D540, lbl_8047D540);
    result = fn_801664F0(&position);
    return (result | -result) >> 31;
}

u32 fn_80165788(u32 id, f32 x, f32 y, f32 z)
{
    GSvec position;
    u32 result;

    set__5GSvecFfff(&position, x, y, z);
    result = fn_80166308(id, &position);
    result = (result | -result) >> 31;
    return result;
}

void scriptSoundStop(u32 fade)
{
    soundStop(lbl_8047B0A8, fade);
}

u32 soundStop(u32 id, u32 fade)
{
    s32 restartSe;

    if (id == 0 || id >= *lbl_80478FA8) {
        return 0;
    }

    restartSe = 0;
    if (lbl_8047B0AC == id) {
        lbl_8047B0AC = 0;
        restartSe = 1;
    }

    if (lbl_8047B0A8 == id) {
        lbl_8047B0A8 = 0;
        restartSe = 0;
    }

    if (!(u8)fn_80166084(id)) {
        fn_80165C70(fade, restartSe, 1);
        if (restartSe != 0) {
            GSlogWrite(lbl_8027358C, id);
            return fn_80165A44(1, 0, 0xFF, restartSe);
        }
        fn_80166B18(id);
    } else {
        fn_801669E4(id, fade, 0);
        fn_80166B18(id);
    }
}

u32 fn_801658FC(u32 id, u32 fade, u32 volume, u32 priority)
{
    if (id == 0 || id >= *lbl_80478FA8) {
        return 0;
    }

    fn_80165C70(fade, 0, 1);
    return fn_80165A44(id, volume, priority, 0);
}

u32 fn_8016597C(u32 id, u32 fade, u32 volume, u32 priority)
{
    if (id == 0 || id >= *lbl_80478FA8) {
        return 0;
    }

    fn_80165C70(fade, 1, 1);
    return fn_80165A44(id, volume, priority, 1);
}

u32 fn_801659FC(u32 id, u32 volume, u32 priority)
{
    return fn_80165A44(id, volume, priority, 0);
}

u32 fn_80165A20(u32 id, u32 volume, u32 priority)
{
    return fn_80165A44(id, volume, priority, 1);
}

u32 fn_80165A44(u32 id, u32 volume, u32 priority, s32 kind)
{
    u32 flags;
    u32 waveIndex;
    u32 currentId;
    void* wave;

    if (id == 0 || id >= *lbl_80478FA8) {
        return 0;
    }

    flags = lbl_80478FAC[id].flags;
    if (((flags >> 7) & 1U) == 1U) {
        if (kind == 0) {
            s32 status = fn_801666BC(id);

            if (status != 0 && status >= 0 && status < 4) {
                fn_80166670(id, 0x32, 0);
                fn_80165D0C(0x32);
                fn_80166B18(id);
            }
            lbl_8047B0A8 = id;
        }
        return fn_80166A50(id, volume, priority, 0);
    }

    {
        u32 resourceId;
        u32 resourceSize;

        if (kind != 0) {
            resourceId = 0x406;
            resourceSize = 0x10000;
        } else {
            resourceId = 0x407;
            resourceSize = 0x2000;
        }

        waveIndex = lbl_80478FAC[id].waveIndex;
        if (waveIndex >= *lbl_80478E30) {
            GSlogWrite(lbl_802735C4, id);
            return 0;
        }

        currentId = fn_801662E8(0, resourceId);
        if (currentId != id) {
            if (currentId != (u32)-1) {
                s32 status = fn_801666BC(currentId);

                if (status != 0 && status >= 0 && status < 4) {
                    fn_80166670(currentId, 0x32, 0);
                    fn_80165D0C(0x32);
                    fn_80166B18(currentId);
                }
            }

            wave = lbl_80478E34[waveIndex].data;
            if (wave != 0) {
                fn_80165EE0(wave, GSresGetResource(0, resourceId), resourceSize);
            } else {
                wave = lbl_80478E34[waveIndex].archive;
                if (wave != 0) {
                    fn_80165DEC(wave, GSresGetResource(0, resourceId), resourceSize);
                } else {
                    GSlogWrite(lbl_802735F8, id);
                    return 0;
                }
            }
            fn_80166B3C(id, 0, resourceId);
        }

        if (kind != 0) {
            lbl_8047B0AC = id;
        }
        if (kind == 0) {
            lbl_8047B0A8 = id;
        }
        return fn_80166A50(id, volume, priority, 0);
    }
}

void fn_80165C70(u32 volume, u32 isSe, s32 wait)
{
    u32 resourceId = isSe != 0 ? 0x406 : 0x407;
    u32 id = fn_801662E8(0, resourceId);

    if (id != (u32)-1) {
        s32 status = fn_801666BC(id);

        if (status != 0 && status >= 0 && status < 4) {
            fn_80166670(id, volume, 0);
            if (wait != 0) {
                fn_80165D0C(volume);
            }
        }
    }
}
