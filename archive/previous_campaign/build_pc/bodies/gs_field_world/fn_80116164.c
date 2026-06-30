static u8* pcport_field_find_anim_record(u32 key)
{
    extern u32 lbl_80478EB8;
    extern u32 lbl_80478EBC;
    extern void* fn_800FF56C(void);

    u32 i;
    u32 seen;
    u32 wanted;
    u32 count;
    u8* base;

    if ((key & 0x7FFF0000u) != 0x7FFF0000u) {
        return NULL;
    }

    wanted = key & 0x1FFu;
    count = *(u32*)lbl_80478EB8;
    base = (u8*)lbl_80478EBC;
    seen = 0;

    for (i = 0; i < count; i++) {
        u8* rec = base + i * 0x1C;
        if (*(u16*)(rec + 0x04) == (u32)fn_800FF56C()) {
            if (wanted == seen) {
                return rec;
            }
            seen++;
        }
    }
    return NULL;
}

/* Drive field animation/flag side effects for the selected animation record. */
s32 fn_80116164(u32 scene, u32 key, u32 mode)
{
    u8* rec;
    u8 kind;

    rec = pcport_field_find_anim_record(key);
    if (rec == NULL) {
        return -1;
    }

    kind = rec[0] >> 5;
    switch ((u8)mode) {
    case 0:
        if (kind == 1) {
            ((void (*)(u32, u32, u32, u32, u32))fn_8018B76C)(scene, key, 2, 0, 1);
            ((void (*)(u32, u32, u32))fn_8018C7C8)(scene, key, 1);
        } else if (kind >= 2 && kind < 4) {
            ((void (*)(u32, u32, u32))fn_8018C1E8)(scene, key, 0);
        }
        break;

    case 1:
        if (kind == 1) {
            ((void (*)(u32, u32, u32, u32, u32))fn_8018B76C)(scene, key, 0, 0, 1);
        }
        if (kind >= 1 && kind < 4) {
            ((void (*)(u32, u32, u32, u32, u32))fn_8018B76C)(scene, key, 0, 0, 1);
        }
        break;

    case 2:
        if (*(u16*)(rec + 0x06) != 0 &&
            (u8)((u32 (*)(u32))fn_801902E0)(*(u16*)(rec + 0x06)) != 0) {
            return 0;
        }

        if (kind == 1) {
            ((void (*)(u32, u32, u32, u32, u32))fn_8018B76C)(scene, key, 1, 0, 0);
            ((void (*)(u32))fn_80166A28)(0x3C2);
            ((void (*)(u32, u32, u32))fn_8018B07C)(scene, key, 1);
        }

        {
            u8* rec2 = pcport_field_find_anim_record(key);
            if (rec2 != NULL) {
                u8 kind2 = rec2[0] >> 5;
                if (kind2 == 1) {
                    ((void (*)(u32, u32, u32, u32, u32))fn_8018B76C)(scene, key, 2, 0, 1);
                    ((void (*)(u32, u32, u32))fn_8018C7C8)(scene, key, 1);
                } else if (kind2 >= 2 && kind2 < 4) {
                    ((void (*)(u32, u32, u32))fn_8018C1E8)(scene, key, 0);
                }
            }
        }

        ((void (*)(u32))fn_80190528)(*(u16*)(rec + 0x06));
        ((void (*)(u32, u32, u32))fn_80115E6C)((rec[0] >> 2) & 7,
                                                 *(u32*)(rec + 0x0C),
                                                 rec[1]);
        break;

    default:
        break;
    }

    return 0;
}
