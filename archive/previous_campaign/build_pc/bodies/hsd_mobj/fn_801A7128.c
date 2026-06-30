#define PCPORT_MOBJ_TE_TEX ((u32) -1)
#define PCPORT_MOBJ_TE_RAS ((u32) -2)

static void PCPort_MObjCallTObjMakeTExp(HSD_TObj* tobj, u32 lightmap,
                                        u32 lightmap_done, HSD_TExp** c,
                                        HSD_TExp** a, HSD_TExp** list)
{
    HSD_ClassInfo* info;
    HSD_TObjInfo* method;

    if (tobj == NULL) {
        return;
    }
    info = tobj->parent.parent.class_info;
    method = info != NULL ? HSD_TOBJ_INFO(info) : NULL;
    if (method != NULL && method->make_texp != NULL) {
        method->make_texp(tobj, lightmap, lightmap_done, c, a, list);
        return;
    }
    PCPort_TObjMakeTExp(tobj, lightmap, lightmap_done, c, a, list);
}

HSD_TExp* fn_801A7128(HSD_MObj* mobj, HSD_TObj* tobj_top, HSD_TExp** list)
{
    typedef HSD_TExp* (*alloc_texp_t)(HSD_TExp**);
    typedef HSD_TExp* (*make_input_t)(void* src, u32 kind, u32 arg,
                                      HSD_TExp** list);
    typedef void (*setup_stage_t)(HSD_TExp* expr, u32 a, u32 b, u32 c,
                                  u32 d);
    typedef void (*combine_stage_t)(HSD_TExp* expr, u32 a, u32 b, u32 c,
                                    u32 d, u32 e, u32 f, u32 g, u32 h);
    typedef void (*bind_tex_t)(HSD_TExp* expr, HSD_TObj* tobj, u32 kind);
    typedef s32 (*is_simple_t)(HSD_TExp* expr);
    alloc_texp_t alloc_texp;
    make_input_t make_input;
    setup_stage_t setup_color;
    setup_stage_t setup_alpha;
    combine_stage_t color_in;
    combine_stage_t alpha_in;
    bind_tex_t bind_tex;
    is_simple_t is_simple;
    HSD_TExp* root;
    HSD_TExp* temp;
    HSD_TExp* color_expr;
    HSD_TExp* alpha_expr;
    HSD_TExp* spec_expr;
    HSD_TExp* final_expr;
    HSD_TObj* toon_tobj;
    HSD_TObj* tobj;
    u32 render_mode;
    u32 base_mode;
    u32 light_mode;
    u32 lightmap_done;

    if (list == NULL) {
        return NULL;
    }
    if (mobj == NULL || mobj->mat == NULL) {
        return PCPort_MObjMakeTExp(mobj, tobj_top, list);
    }

    alloc_texp = (alloc_texp_t) fn_801B707C;
    make_input = (make_input_t) fn_801B6F5C;
    setup_color = (setup_stage_t) fn_801B6E74;
    setup_alpha = (setup_stage_t) fn_801B6CD8;
    color_in = (combine_stage_t) fn_801B64EC;
    alpha_in = (combine_stage_t) fn_801B5F08;
    bind_tex = (bind_tex_t) fn_801B5E40;
    is_simple = (is_simple_t) fn_801B7C60;

    *list = NULL;
    toon_tobj = NULL;
    lightmap_done = 0;
    for (tobj = tobj_top; tobj != NULL; tobj = tobj->next) {
        if (tobj_coord(tobj) == TEX_COORD_TOON) {
            toon_tobj = tobj;
        }
    }

    render_mode = mobj->rendermode;
    base_mode = render_mode & (RENDER_CONSTANT | RENDER_VERTEX);
    if (base_mode == 0) {
        base_mode = RENDER_CONSTANT;
    }

    light_mode = render_mode & (0x6000);
    if (light_mode == 0) {
        light_mode = base_mode << 13;
    }

    root = alloc_texp(list);
    if (render_mode & RENDER_DIFFUSE) {
        if (base_mode == RENDER_VERTEX) {
            setup_color(root, 0, 0, 0, 1);
            color_in(root, 7, 0, 7, 0, 7, 0, 8, 0);
        } else {
            temp = make_input(&mobj->mat->diffuse, 1, 0, list);
            setup_color(root, 0, 0, 0, 1);
            color_in(root, 7, 0, 7, 0, 7, 0, 1, (u32) temp);
        }

        if (light_mode == 0x4000) {
            temp = make_input(lbl_80478C88, 6, 0, list);
            setup_alpha(root, 0, 0, 0, 1);
            alpha_in(root, 7, 0, 7, 0, 7, 0, 6, (u32) temp);
        } else {
            temp = make_input(&mobj->mat->alpha, 6, 3, list);
            setup_alpha(root, 0, 0, 0, 1);
            alpha_in(root, 7, 0, 7, 0, 7, 0, 6, (u32) temp);
        }
    } else {
        if (base_mode == RENDER_CONSTANT) {
            temp = make_input(&mobj->mat->diffuse, 1, 0, list);
            setup_color(root, 0, 0, 0, 1);
            color_in(root, 7, 0, 7, 0, 7, 0, 1, (u32) temp);
        } else if (base_mode == RENDER_VERTEX) {
            bind_tex(root, toon_tobj, TEX_COORD_TOON);
            setup_color(root, 0, 0, 0, 1);
            color_in(root, 7, 0, 7, 0, 7, 0, 1,
                     toon_tobj != NULL ? PCPORT_MOBJ_TE_TEX
                                       : PCPORT_MOBJ_TE_RAS);
        } else {
            temp = make_input(&mobj->mat->diffuse, 1, 0, list);
            bind_tex(root, toon_tobj, TEX_COORD_TOON);
            setup_color(root, 0, 0, 0, 1);
            color_in(root, 1,
                     toon_tobj != NULL ? PCPORT_MOBJ_TE_TEX
                                       : PCPORT_MOBJ_TE_RAS,
                     7, 0, 7, 0, 1, (u32) temp);
        }

        if (light_mode == 0x2000) {
            temp = make_input(&mobj->mat->alpha, 6, 3, list);
            setup_alpha(root, 0, 0, 0, 1);
            alpha_in(root, 7, 0, 7, 0, 7, 0, 6, (u32) temp);
        } else if (light_mode == 0x4000) {
            bind_tex(root, toon_tobj, TEX_COORD_TOON);
            setup_alpha(root, 0, 0, 0, 1);
            alpha_in(root, 7, 0, 7, 0, 7, 0, 5, PCPORT_MOBJ_TE_RAS);
        } else {
            temp = make_input(&mobj->mat->alpha, 6, 3, list);
            bind_tex(root, toon_tobj, TEX_COORD_TOON);
            setup_alpha(root, 0, 0, 0, 1);
            alpha_in(root, 7, 0, 5, PCPORT_MOBJ_TE_RAS, 6, (u32) temp, 7, 0);
        }
    }

    color_expr = root;
    alpha_expr = root;
    for (tobj = tobj_top; tobj != NULL; tobj = tobj->next) {
        if ((tobj->flags & (TEX_LIGHTMAP_DIFFUSE | TEX_LIGHTMAP_AMBIENT)) != 0 &&
            tobj->id != 0xFF)
        {
            PCPort_MObjCallTObjMakeTExp(tobj,
                                        TEX_LIGHTMAP_DIFFUSE |
                                            TEX_LIGHTMAP_AMBIENT,
                                        lightmap_done, &color_expr, &alpha_expr,
                                        list);
        }
    }
    lightmap_done |= TEX_LIGHTMAP_DIFFUSE | TEX_LIGHTMAP_AMBIENT;

    if (render_mode & RENDER_DIFFUSE) {
        if ((light_mode & 0x4000) != 0) {
            root = alloc_texp(list);
            bind_tex(root, NULL, 5);
            setup_color(root, 0, 0, 0, 1);
            color_in(root, 7, 0, 7, 0, 7, 0, 1, (u32) color_expr);
            setup_alpha(root, 0, 0, 0, 1);
            alpha_in(root, 5, (u32) alpha_expr, 7, 0, 5, PCPORT_MOBJ_TE_RAS,
                     7, 0);
            color_expr = root;
            alpha_expr = root;
        }

        root = alloc_texp(list);
        bind_tex(root, toon_tobj, TEX_COORD_TOON);
        setup_color(root, 0, 0, 0, 1);
        color_in(root, 7, 0, 1, (u32) color_expr, 1,
                 toon_tobj != NULL ? PCPORT_MOBJ_TE_TEX : PCPORT_MOBJ_TE_RAS,
                 7, 0);
        color_expr = root;

        setup_alpha(root, 0, 0, 0, 1);
        if ((light_mode & 0x4000) != 0) {
            alpha_in(root, 7, 0, 5, (u32) alpha_expr, 5,
                     PCPORT_MOBJ_TE_RAS, 7, 0);
        } else {
            alpha_in(root, 5, (u32) alpha_expr, 7, 0, 5,
                     PCPORT_MOBJ_TE_RAS, 7, 0);
        }
        alpha_expr = root;
    }

    if (render_mode & RENDER_SPECULAR) {
        temp = make_input(&mobj->mat->specular, 1, 0, list);
        root = alloc_texp(list);
        setup_color(root, 0, 0, 0, 1);
        color_in(root, 7, 0, 7, 0, 7, 0, 1, (u32) temp);
        spec_expr = root;

        for (tobj = tobj_top; tobj != NULL; tobj = tobj->next) {
            if ((tobj->flags & TEX_LIGHTMAP_SPECULAR) != 0 &&
                tobj->id != 0xFF)
            {
                PCPort_MObjCallTObjMakeTExp(tobj, TEX_LIGHTMAP_SPECULAR,
                                            lightmap_done, &spec_expr,
                                            &alpha_expr, list);
            }
        }
        lightmap_done |= TEX_LIGHTMAP_SPECULAR;

        root = alloc_texp(list);
        bind_tex(root, NULL, 5);
        setup_color(root, 0, 0, 0, 1);
        color_in(root, 7, 0, 1, (u32) spec_expr, 1, PCPORT_MOBJ_TE_RAS, 7, 0);
        spec_expr = root;

        root = alloc_texp(list);
        setup_color(root, 0, 0, 0, 1);
        color_in(root, 1, (u32) spec_expr, 7, 0, 7, 0, 1,
                 (u32) color_expr);
        color_expr = root;
    }

    final_expr = color_expr;
    for (tobj = tobj_top; tobj != NULL; tobj = tobj->next) {
        if ((tobj->flags & TEX_LIGHTMAP_EXT) != 0 && tobj->id != 0xFF) {
            PCPort_MObjCallTObjMakeTExp(tobj, TEX_LIGHTMAP_EXT, lightmap_done,
                                        &final_expr, &alpha_expr, list);
        }
    }

    if (final_expr == alpha_expr && is_simple(final_expr) == 1 &&
        is_simple(alpha_expr) == 1)
    {
        return final_expr;
    }

    root = alloc_texp(list);
    setup_color(root, 0, 0, 0, 1);
    color_in(root, 7, 0, 7, 0, 7, 0, 1, (u32) final_expr);
    setup_alpha(root, 0, 0, 0, 1);
    alpha_in(root, 7, 0, 7, 0, 7, 0, 5, (u32) alpha_expr);
    return root;
}

#undef PCPORT_MOBJ_TE_TEX
#undef PCPORT_MOBJ_TE_RAS
