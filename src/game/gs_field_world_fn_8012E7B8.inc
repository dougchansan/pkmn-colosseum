    nofralloc
    stwu r1, -0xb0(r1)
    mflr r0
    stw r0, 0xb4(r1)
    stfd f31, 0xa0(r1)
    psq_st f31, 0xa8(r1), 0, 0
    stmw r25, 0x84(r1)
    mr r28, r3
    li r3, 0x1
    bl fn_800F7AF0
    mr r27, r3
    li r3, 0x1
    bl fn_800F7BC4
    and r0, r3, r27
    rlwinm. r0, r0, 0, 23, 23
    beq @L_8012E80C
    mr r3, r28
    bl fn_8012C0B4
    clrlwi. r0, r3, 24
    beq @L_8012E80C
    lfs f1, -26216(r2)
    b @L_8012EBB8
@L_8012E80C:
    lwz r3, -26224(r2)
    cmpwi r28, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x28(r1)
    stw r0, 0x2c(r1)
    blt @L_8012E838
    cmpwi r28, 0x2
    bge @L_8012E838
    slwi r0, r28, 2
    addi r3, r1, 0x28
    lwzx r26, r3, r0
@L_8012E838:
    mr r4, r26
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x68
    bl fn_800E3D98
    mr r3, r28
    addi r4, r1, 0xc
    bl fn_8012E388
    lwz r3, -26224(r2)
    cmpwi r28, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x20(r1)
    stw r0, 0x24(r1)
    blt @L_8012E884
    cmpwi r28, 0x2
    bge @L_8012E884
    slwi r0, r28, 2
    addi r3, r1, 0x20
    lwzx r25, r3, r0
@L_8012E884:
    mr r4, r25
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x5c
    bl fn_800E3D98
    addi r3, r1, 0x5c
    addi r4, r1, 0x68
    addi r5, r1, 0x50
    bl fn_800A3A9C
    bl fn_800D3088
    lis r0, 0x4330
    stw r3, 0x7c(r1)
    addi r3, r1, 0x50
    lfd f2, -26168(r2)
    stw r0, 0x78(r1)
    mr r4, r3
    lfs f0, -26144(r2)
    lfd f1, 0x78(r1)
    fsubs f1, f1, f2
    fdivs f1, f0, f1
    bl fn_800A3AC0
    lwz r3, -26224(r2)
    cmpwi r28, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x30(r1)
    stw r0, 0x34(r1)
    blt @L_8012E904
    cmpwi r28, 0x2
    bge @L_8012E904
    slwi r0, r28, 2
    addi r3, r1, 0x30
    lwzx r29, r3, r0
@L_8012E904:
    mr r4, r29
    addi r5, r1, 0x50
    li r3, 0x0
    bl fn_801887D8
    fmr f31, f1
    lfs f0, -26064(r2)
    fcmpo cr0, f31, f0
    bge @L_8012E940
    lfs f1, 0xc(r1)
    fcmpo cr0, f1, f0
    cror eq, gt, eq
    bne @L_8012E93C
    fmr f31, f0
    b @L_8012E940
@L_8012E93C:
    fmr f31, f1
@L_8012E940:
    lwz r3, -26224(r2)
    cmpwi r28, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x18(r1)
    stw r0, 0x1c(r1)
    blt @L_8012E96C
    cmpwi r28, 0x2
    bge @L_8012E96C
    slwi r0, r28, 2
    addi r3, r1, 0x18
    lwzx r31, r3, r0
@L_8012E96C:
    mr r4, r31
    li r3, 0x0
    bl fn_800F9318
    fmr f1, f31
    mr r4, r28
    bl fn_8012C660
    lis r4, lbl_80426BD0@ha
    lwz r3, -26224(r2)
    addi r4, r4, lbl_80426BD0@l
    lwz r0, -26220(r2)
    lwz r4, 0x0(r4)
    li r31, 0x0
    stw r3, 0x10(r1)
    cmpwi r4, 0x0
    stw r0, 0x14(r1)
    blt @L_8012E9C0
    cmpwi r4, 0x2
    bge @L_8012E9C0
    slwi r0, r4, 2
    addi r3, r1, 0x10
    lwzx r30, r3, r0
@L_8012E9C0:
    mr r4, r30
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x44
    bl fn_800E3D98
    lis r3, lbl_80426BD0@ha
    addi r3, r3, lbl_80426BD0@l
    lwz r0, 0x48(r3)
    cmpwi r0, 0x0
    bgt @L_8012E9F0
    li r7, 0x0
    b @L_8012EA2C
@L_8012E9F0:
    lwz r4, 0x44(r3)
    subic. r4, r4, 0x1
    bge @L_8012EA00
    addi r4, r4, 0x14
@L_8012EA00:
    mulli r0, r4, 0xc
    lis r4, lbl_80426BD0@ha
    li r7, 0x1
    addi r4, r4, lbl_80426BD0@l
    add r6, r4, r0
    lwz r5, 0x4c(r6)
    lwz r4, 0x50(r6)
    lwz r0, 0x54(r6)
    stw r5, 0x38(r1)
    stw r4, 0x3c(r1)
    stw r0, 0x40(r1)
@L_8012EA2C:
    clrlwi. r0, r7, 24
    beq @L_8012EB44
    lfs f3, 0x38(r1)
    lfs f0, 0x44(r1)
    lfs f2, 0x40(r1)
    lfs f1, 0x4c(r1)
    fsubs f3, f3, f0
    lfs f0, -26216(r2)
    fsubs f1, f2, f1
    fmuls f2, f3, f3
    fmuls f1, f1, f1
    fadds f4, f2, f1
    fcmpo cr0, f4, f0
    ble @L_8012EAAC
    frsqrte f1, f4
    lfd f3, -26200(r2)
    lfd f2, -26192(r2)
    fmul f0, f1, f1
    fmul f1, f3, f1
    fnmsub f0, f4, f0, f2
    fmul f1, f1, f0
    fmul f0, f1, f1
    fmul f1, f3, f1
    fnmsub f0, f4, f0, f2
    fmul f1, f1, f0
    fmul f0, f1, f1
    fmul f1, f3, f1
    fnmsub f0, f4, f0, f2
    fmul f0, f1, f0
    fmul f4, f4, f0
    frsp f4, f4
    b @L_8012EB30
@L_8012EAAC:
    lfd f0, -26184(r2)
    fcmpo cr0, f4, f0
    bge @L_8012EAC4
    lis r4, lbl_80478AC0@ha
    lfs f4, lbl_80478AC0@l(r4)
    b @L_8012EB30
@L_8012EAC4:
    stfs f4, 0x8(r1)
    lis r0, 0x7f80
    lwz r5, 0x8(r1)
    rlwinm r4, r5, 0, 1, 8
    cmpw r4, r0
    beq @L_8012EAEC
    bge @L_8012EB1C
    cmpwi r4, 0x0
    beq @L_8012EB04
    b @L_8012EB1C
@L_8012EAEC:
    clrlwi. r0, r5, 9
    beq @L_8012EAFC
    li r0, 0x1
    b @L_8012EB20
@L_8012EAFC:
    li r0, 0x2
    b @L_8012EB20
@L_8012EB04:
    clrlwi. r0, r5, 9
    beq @L_8012EB14
    li r0, 0x5
    b @L_8012EB20
@L_8012EB14:
    li r0, 0x3
    b @L_8012EB20
@L_8012EB1C:
    li r0, 0x4
@L_8012EB20:
    cmpwi r0, 0x1
    bne @L_8012EB30
    lis r4, lbl_80478AC0@ha
    lfs f4, lbl_80478AC0@l(r4)
@L_8012EB30:
    lfs f0, -26060(r2)
    fcmpo cr0, f4, f0
    ble @L_8012EB48
    li r31, 0x1
    b @L_8012EB48
@L_8012EB44:
    li r31, 0x1
@L_8012EB48:
    clrlwi. r0, r31, 24
    beq @L_8012EBAC
    lis r4, lbl_80426BD0@ha
    lwz r5, 0x44(r1)
    addi r7, r4, lbl_80426BD0@l
    lwz r4, 0x48(r1)
    lwz r6, 0x44(r7)
    lwz r0, 0x4c(r1)
    mulli r6, r6, 0xc
    add r6, r7, r6
    stw r5, 0x4c(r6)
    stw r4, 0x50(r6)
    stw r0, 0x54(r6)
    lwz r4, 0x44(r7)
    addi r0, r4, 0x1
    cmpwi r0, 0x14
    stw r0, 0x44(r7)
    blt @L_8012EB98
    li r0, 0x0
    stw r0, 0x44(r7)
@L_8012EB98:
    lwz r4, 0x48(r3)
    cmpwi r4, 0x14
    bge @L_8012EBAC
    addi r0, r4, 0x1
    stw r0, 0x48(r3)
@L_8012EBAC:
    addi r3, r1, 0x68
    addi r4, r1, 0x5c
    bl fn_800A3C00
@L_8012EBB8:
    psq_l f31, 0xa8(r1), 0, 0
    lfd f31, 0xa0(r1)
    lmw r25, 0x84(r1)
    lwz r0, 0xb4(r1)
    mtlr r0
    addi r1, r1, 0xb0
    blr
