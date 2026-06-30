/* fn_8012C0B4 @ 0x8012C0B4 | gs_field_world.c
 * Field "turn-zone" / cut-scene camera setup for a player.
 * Looks up the per-player object-id table {100,101} (lbl_8047D030/34, stored as
 * 32-bit integer ids -- loaded with lwz in the target, NOT floats), resolves the
 * scene/camera handle for the requested player, then either (a) builds and stores
 * a 0xD0-byte camera transform into the global field-world buffer (lbl_80426BD0)
 * when no live scene exists, or (b) drives the full per-player object enable /
 * turn-path / re-init cascade when a scene IS already live.
 * Returns 1 when a scene was (re)started, else 0.
 * ENDIAN-QA: lbl_80426BD0+0x4 is read as a u16 occupancy/active flag of a wider field. */
u32 fn_8012C0B4(s32 playerIdx) {
    extern u8 lbl_80426BD0[];
    /* object-id table {100,101}; sdata2 stores these as plain 32-bit ints */
    extern f32 lbl_8047D030;  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047D034;  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047D038; /* 0.0f */
    extern f32 lbl_8047D070; /* 1.2f */
    extern f32 lbl_8047D074; /* 40.0f */
    extern f32 lbl_8047D078; /* 5.0f  */
    extern f32 lbl_8047D07C; /* 10.0f */
    extern f32 lbl_8047D080; /* 1.0f  */

    extern u32  fn_8018CD08(u32 a, u32 obj, f32 f1, f32 f2);
    extern u32  fn_8018D998(u32 a, u32 obj);
    extern u32  fn_8018D928(void);
    extern void* fn_8018FCBC(void);
    extern u32  fn_8018FC50(u32 obj);
    extern void fn_800CE148(void); /* sin */
    extern void fn_800CDBE0(void); /* cos */
    extern void fn_800A3A78(void* dst, const void* a, const void* b);
    extern void* fn_80111B9C(void* a, void* b, void* dst);
    extern void fn_8018790C(u32 a, u32 b);
    extern void fn_800F7D38(u32 a, u32 b, u32 c);
    extern void fn_800F7C8C(u32 a, u32 b, u32 c);
    extern void* fn_800F9318(u32 a, u32 obj);
    extern void fn_8012C660(void* obj, s32 playerIdx, f32 turnAmount);
    extern void fn_8018C7C8(u32 a, u32 obj, u32 mask);
    extern void fn_8018C69C(u32 a, u32 obj, u32 mask);
    extern void fn_8018CA20(u32 a, u32 obj, u32 mode);
    extern void fn_8018BA04(u32 a, u32 b, void* outVec);
    extern void fn_80187D48(u32 a, u32 obj, f32 x, f32 y, f32 z, f32 w);
    extern u32  fn_8018D7D0(u32 a, u32 b);
    extern void fn_80116164(u32 a, u32 b, u32 c);
    extern void fn_80183730(void);
    extern u32  fn_8018397C(u32 a, u32 b);
    extern void fn_801812E8(u32 a, u32 b, u32 c);
    extern void fn_800F7434(void* a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g);
    extern void fn_80189490(u32 a, u32 b);
    extern void fn_80183688(void);

    extern float sinf(float);
    extern float cosf(float);

    u32 ids[2];
    u32 selObj;
    u32 scene;
    s32 i;
    s32 offset;
    u8 active;

    ids[0] = lbl_8047D030;
    ids[1] = lbl_8047D034;

    selObj = 0;
    if (playerIdx >= 0 && playerIdx < 2) {
        selObj = ids[playerIdx];
    }

    scene = fn_8018CD08(0, selObj, lbl_8047D070, lbl_8047D074);

    if (scene == 0) {
        /* ---- no live scene: build and store a camera transform ---- */
        u32 ids2[2];
        s32 idx;
        u32 obj2;
        u32 srcObj;
        void* nodeA;
        void* nodeB;
        u32 transform = 0;

        struct Vec3 { f32 x, y, z; };
        struct Mtx { f32 m[3][4]; }; /* 0xD0 = 0x34 (vec+flag block) ... matched to copy size below */

        f32 posSrc[3];   /* sp+0x38 : translation/source vec */
        f32 sc[3];       /* sp+0x44 : {sin-scaled, cos-scaled, 0} build vec */
        u8  result[0xD0];/* sp+0x5c : composed 0xD0-byte transform */

        ids2[0] = lbl_8047D030;
        ids2[1] = lbl_8047D034;

        /* index = field-world buffer's stored player/zone index */
        idx = *(s32*)(lbl_80426BD0 + 0x1A0);
        obj2 = 0;
        if (idx >= 0 && idx < 2) {
            obj2 = ids2[idx];
        }

        fn_8018D998(0, obj2);
        srcObj = fn_8018D928();

        if (srcObj != 0) {
            f32 angle;
            f32 s, c;

            nodeA = fn_8018FCBC();                 /* yields a vec3 source (nodeA[0..2]) */
            srcObj = fn_8018FC50(srcObj);
            nodeB = (void*)srcObj;                 /* second vec3 source */

            posSrc[0] = ((f32*)nodeA)[0];          /* r25+0x0 -> sp+0x38 */
            /* r25+0x4 is reinterpreted as f32 and offset by 5.0f */
            angle = *(f32*)((u8*)nodeA + 0x4) + lbl_8047D078;
            posSrc[1] = angle;                     /* stored back at sp+0x3c */
            posSrc[2] = *(f32*)((u8*)nodeA + 0x8); /* r25+0x8 -> sp+0x40 */

            /* sin of nodeB[0x4]; scaled by 10.0f */
            s = sinf(*(f32*)((u8*)nodeB + 0x4));
            sc[0] = lbl_8047D07C * s;
            sc[2] = lbl_8047D038;                  /* 0.0f at sp+0x48 (note store order) */

            /* cos of nodeA[0x4]; scaled by 10.0f */
            c = cosf(*(f32*)((u8*)nodeB + 0x4));    /* r24 (=fn_8018FC50 result) +0x4 */
            sc[1] = lbl_8047D07C * c;              /* sp+0x4c */
            /* FUNCTIONAL-TODO: sin uses nodeB+0x4, cos uses nodeB+0x4 as well per
               the lfs 0x4(r24) -> both trig args come from the fn_8018FC50 object;
               posSrc/sc field offsets (0x44/0x48/0x4c) preserved from target stores. */

            fn_800A3A78((void*)posSrc, (const void*)sc, (const void*)sc);
            transform = (u32)fn_80111B9C((void*)posSrc, (void*)sc, (void*)result);

            memcpy(lbl_80426BD0 + 0x1A0, result, 0xD0);
        } else {
            /* no source: only the cleared/previous result block (sp+0x5c) is copied */
            memcpy(lbl_80426BD0 + 0x1A0, &result[0], 0xD0);
        }

        *(u32*)(lbl_80426BD0 + 0x410) = transform;

        if (transform != 0) {
            fn_8018790C(0, 0x64);
            return 1;
        }
        return 0;
    }

    /* ---- live scene exists: run the per-player enable / turn cascade ---- */
    fn_800F7D38(1, 0, 0);
    fn_800F7C8C(1, 0, 0);

    for (i = 0, offset = 0; i < 2; i++, offset += 0x20) {
        u32 pObj;
        u32 pSel;
        void* fieldObj;
        active = (*(u16*)(lbl_80426BD0 + offset + 0x4) & 0x1) ? 1 : 0;
        if (!active) {
            continue;
        }

        ids[0] = lbl_8047D030;
        ids[1] = lbl_8047D034;
        pObj = (i >= 0 && i < 2) ? ids[i] : 0;
        pSel = (i >= 0 && i < 2) ? ids[i] : 0;

        fn_800F9318(0, pSel);
        fieldObj = fn_800F9318(0, pSel); /* result lives in r3 for next call's selector path */
        (void)fieldObj;

        fn_8012C660((void*)0, i, lbl_8047D038);

        fn_8018C7C8(0, pObj, 0x80000008);
        fn_8018C69C(0, pObj, 0x100);
        fn_8018C69C(0, pObj, 0x400);
        fn_8018CA20(0, pObj, 0);
    }

    {
        u32 sceneA = *(u32*)(lbl_80426BD0 + 0x28); /* scene handle is the global, not 'scene' local: target reads r31 = the live handle */
        u32 sceneB = *(u32*)(lbl_80426BD0 + 0x2C);
        f32 vec[3];
        u32 hit;

        /* NOTE: target indexes r31 (the live scene returned by fn_8018CD08) at +0x28/+0x2C */
        sceneA = *(u32*)((u8*)scene + 0x28);
        sceneB = *(u32*)((u8*)scene + 0x2C);

        fn_8018BA04(sceneA, sceneB, vec);
        fn_80187D48(0, selObj, vec[0], vec[1], vec[2], lbl_8047D080);

        sceneA = *(u32*)((u8*)scene + 0x28);
        sceneB = *(u32*)((u8*)scene + 0x2C);
        hit = fn_8018D7D0(sceneA, sceneB) & 0xFF;

        if (hit != 0) {
            sceneA = *(u32*)((u8*)scene + 0x28);
            sceneB = *(u32*)((u8*)scene + 0x2C);
            fn_80116164(sceneA, sceneB, 2);
        } else {
            u32 npc;
            sceneA = *(u32*)((u8*)scene + 0x28);
            sceneB = *(u32*)((u8*)scene + 0x2C);
            fn_8018D998(sceneA, sceneB);
            fn_80183730();

            sceneA = *(u32*)((u8*)scene + 0x28);
            sceneB = *(u32*)((u8*)scene + 0x2C);
            npc = fn_8018397C(sceneA, sceneB);

            if (npc != 0) {
                sceneA = *(u32*)((u8*)scene + 0x28);
                sceneB = *(u32*)((u8*)scene + 0x2C);
                fn_801812E8(sceneA, sceneB, 1);

                sceneA = *(u32*)((u8*)scene + 0x28);
                sceneB = *(u32*)((u8*)scene + 0x2C);
                fn_800F7434((void*)npc, 0x4, sceneA, sceneB, 0, 0, 0);

                sceneA = *(u32*)((u8*)scene + 0x28);
                sceneB = *(u32*)((u8*)scene + 0x2C);
                fn_801812E8(sceneA, sceneB, 0);

                sceneA = *(u32*)((u8*)scene + 0x28);
                sceneB = *(u32*)((u8*)scene + 0x2C);
                fn_80189490(sceneA, sceneB);
            }

            sceneA = *(u32*)((u8*)scene + 0x28);
            sceneB = *(u32*)((u8*)scene + 0x2C);
            fn_8018D998(sceneA, sceneB);
            fn_80183688();
        }
    }

    for (i = 0, offset = 0; i < 2; i++, offset += 0x20) {
        u32 pObj;
        active = (*(u16*)(lbl_80426BD0 + offset + 0x4) & 0x1) ? 1 : 0;
        if (!active) {
            continue;
        }
        ids[0] = lbl_8047D030;
        ids[1] = lbl_8047D034;
        pObj = (i >= 0 && i < 2) ? ids[i] : 0;

        fn_8018CA20(0, pObj, 1);
        fn_8018C7C8(0, pObj, 0x700);
        fn_8018C69C(0, pObj, 0x80000008);
    }

    fn_800F7D38(1, 0, 0);
    fn_800F7C8C(1, 0, 0);
    return 1;
}
