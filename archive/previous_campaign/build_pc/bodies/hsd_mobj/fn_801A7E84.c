static f32 PCPort_MObjClamp01(f32 value)
{
    if (value <= 0.0f) {
        return 0.0f;
    }
    if (value >= 1.0f) {
        return 1.0f;
    }
    return value;
}

static u8 PCPort_MObjAnimByte(f32 value)
{
    return (u8) (PCPort_MObjClamp01(value) * 255.0f);
}

static void PCPort_MObjSetPackedRGB(u32* color, u32 shift, u8 component)
{
    u32 mask;

    if (color == NULL) {
        return;
    }
    mask = 0xFFu << shift;
    *color = (*color & ~mask) | ((u32) component << shift);
}

void fn_801A7E84(HSD_MObj* mobj, u32 type, HSD_ObjData* value)
{
    u8 component;

    if (mobj == NULL || value == NULL || type > 13u) {
        return;
    }

    component = PCPort_MObjAnimByte(value->fv);
    switch (type) {
    case 1:
        if (mobj->mat != NULL) {
            PCPort_MObjSetPackedRGB(&mobj->mat->ambient, 24u, component);
        }
        break;
    case 2:
        if (mobj->mat != NULL) {
            PCPort_MObjSetPackedRGB(&mobj->mat->ambient, 16u, component);
        }
        break;
    case 3:
        if (mobj->mat != NULL) {
            PCPort_MObjSetPackedRGB(&mobj->mat->ambient, 8u, component);
        }
        break;
    case 4:
        if (mobj->mat != NULL) {
            PCPort_MObjSetPackedRGB(&mobj->mat->diffuse, 24u, component);
        }
        break;
    case 5:
        if (mobj->mat != NULL) {
            PCPort_MObjSetPackedRGB(&mobj->mat->diffuse, 16u, component);
        }
        break;
    case 6:
        if (mobj->mat != NULL) {
            PCPort_MObjSetPackedRGB(&mobj->mat->diffuse, 8u, component);
        }
        break;
    case 7:
        if (mobj->mat != NULL) {
            mobj->mat->alpha = PCPort_MObjClamp01(1.0f - value->fv);
        }
        break;
    case 8:
        if (mobj->mat != NULL) {
            PCPort_MObjSetPackedRGB(&mobj->mat->specular, 24u, component);
        }
        break;
    case 9:
        if (mobj->mat != NULL) {
            PCPort_MObjSetPackedRGB(&mobj->mat->specular, 16u, component);
        }
        break;
    case 10:
        if (mobj->mat != NULL) {
            PCPort_MObjSetPackedRGB(&mobj->mat->specular, 8u, component);
        }
        break;
    case 11:
        if (mobj->pe != NULL) {
            mobj->pe->ref0 = component;
        }
        break;
    case 12:
        if (mobj->pe != NULL) {
            mobj->pe->ref1 = component;
        }
        break;
    case 13:
        if (mobj->pe != NULL) {
            mobj->pe->dst_alpha = component;
        }
        break;
    default:
        break;
    }
}
