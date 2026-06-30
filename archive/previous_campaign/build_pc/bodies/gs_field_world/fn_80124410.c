static u32 pcport_field_rand32(void)
{
    u32 hi = ((u32 (*)(void))fn_800E0C54)() << 16;
    u32 lo = ((u32 (*)(void))fn_800E0C54)() & 0xFFFFu;
    return hi | lo;
}

static s32 pcport_field_current_group(u8* obj)
{
    u16 species;
    u16 wanted;

    if (obj == NULL) {
        return 2;
    }

    species = (u16)fn_8012640C(obj, 0, 0x6E, 0);
    wanted = (u16)fn_8012640C(NULL, species, 0x13, 0);

    if ((s32)wanted == (s32)(u8)((u32 (*)(u32))fn_80131574)(0)) {
        return 0;
    }
    if ((s32)wanted == (s32)(u8)((u32 (*)(u32))fn_80131574)(1)) {
        return 1;
    }
    if ((s32)wanted == (s32)(u8)((u32 (*)(u32))fn_80131574)(2)) {
        return 2;
    }
    return -1;
}

static u16 pcport_field_group_threshold(u8* obj)
{
    u16 species = (u16)fn_8012640C(obj, 0, 0x6E, 0);
    return (u16)fn_8012640C(NULL, species, 0x13, 0);
}

static u8 pcport_field_group_or_random(u8* obj, u32 value)
{
    s32 group = pcport_field_current_group(obj);
    if (group >= 0) {
        return (u8)group;
    }
    return (pcport_field_group_threshold(obj) > (u32)(value & 0xFFu)) ? 1 : 0;
}

static u8 pcport_field_seed_test(u32 value, u32 seed_xor)
{
    u32 x = (value >> 16) ^ (value & 0xFFFFu) ^ seed_xor ^ 8u;
    return (x >= 8u && x < 16u) ? 1 : 0;
}

/* Draw a pseudo-random field/event id, optionally constrained by group,
   modulo-25 residue, and the 16-bit XOR fingerprint carried in seed. */
u32 fn_80124410(u8* obj, s32 arg_group, s32 arg_mod, s32 arg_seed_mode, u32 seed)
{
    s8 group_filter;
    s8 mod_filter;
    s8 seed_filter;
    u32 seed_xor;

    if (obj == NULL) {
        return pcport_field_rand32();
    }

    group_filter = (s8)arg_group;
    mod_filter = (s8)arg_mod;
    seed_filter = (s8)arg_seed_mode;
    seed_xor = (seed >> 16) ^ (seed & 0xFFFFu);

    for (;;) {
        u32 value = pcport_field_rand32();

        if (group_filter >= 0) {
            s32 group = pcport_field_current_group(obj);
            if (group < 0) {
                if (group_filter != 2) {
                    if ((u8)group_filter != pcport_field_group_or_random(obj, value)) {
                        continue;
                    }
                } else {
                    u32 current = fn_8012640C(obj, 0, 0x6F, 0);
                    u8 lhs = pcport_field_group_or_random(obj, current);
                    u8 rhs = pcport_field_group_or_random(obj, value);
                    if (lhs != rhs) {
                        continue;
                    }
                }
            }
        }

        if (mod_filter >= 0 && (u8)mod_filter != (u8)(value % 25u)) {
            continue;
        }

        if (seed_filter >= 0) {
            u8 hit = pcport_field_seed_test(value, seed_xor);
            if (seed_filter != 0) {
                if (hit == 0) {
                    continue;
                }
            } else if (hit == 1) {
                continue;
            }
        }

        return value;
    }
}
