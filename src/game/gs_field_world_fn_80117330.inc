    nofralloc
    stwu r1, -0x90(r1)
    mflr r0
    stw r0, 0x94(r1)
    stfd f31, 0x80(r1)
    psq_st f31, 0x88(r1), 0, 0
    fmr f31, f1
    lbz r0, lbl_8047AD71(r13)
    cmplwi r0, 0x0
    beq @L_801174AC
    bl GSscene_GetMode
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_801174AC
    lwz r0, lbl_8047AD68(r13)
    cmplwi r0, 0x0
    beq @L_801174AC
    li r3, 0x0
    li r4, 0x64
    bl fn_800F9318
    cmplwi r3, 0x0
    beq @L_80117390
    addi r4, r1, 0x44
    bl fn_800E3D98
    b @L_80117398
@L_80117390:
    addi r3, r1, 0x44
    bl GSscene_GetCameraPositionVector
@L_80117398:
    addi r3, r1, 0x38
    bl GSscene_GetCameraViewVector
    lwz r0, lbl_8047AD68(r13)
    cmplwi r0, 0x1
    bne @L_801173CC
    lwz r3, lbl_8047AD6C(r13)
    lfs f0, 0x10(r3)
    stfs f0, 0xc(r1)
    lfs f0, 0xc(r3)
    stfs f0, 0x10(r1)
    lfs f0, 0x14(r3)
    stfs f0, 0x8(r1)
    b @L_80117404
@L_801173CC:
    bl fn_8017669C
    stfs f1, 0xc(r1)
    bl fn_80176690
    stfs f1, 0x10(r1)
    bl fn_80176684
    stfs f1, 0x8(r1)
    addi r3, r1, 0x44
    addi r4, r1, 0xc
    addi r5, r1, 0x10
    addi r6, r1, 0x8
    bl floorUpdateFieldCamera
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    beq @L_801174AC
@L_80117404:
    lfs f1, -26320(r2)
    addi r3, r1, 0x14
    lfs f2, 0xc(r1)
    lfs f3, 0x10(r1)
    bl fn_800E01F4
    lfs f1, 0x8(r1)
    addi r3, r1, 0x50
    bl fn_800E0518
    addi r3, r1, 0x14
    addi r4, r1, 0x50
    mr r5, r3
    bl fn_800DFF98
    addi r3, r1, 0x2c
    addi r4, r1, 0x44
    addi r5, r1, 0x38
    bl fn_800E019C
    addi r3, r1, 0x2c
    addi r5, r1, 0x14
    mr r4, r3
    bl fn_800E019C
    lfs f0, 0x8(r1)
    lfs f1, 0xc(r1)
    stfs f0, 0x24(r1)
    lfs f2, 0x10(r1)
    bl fn_800CE2D8
    frsp f2, f1
    lfs f0, -26320(r2)
    fmr f1, f31
    addi r4, r1, 0x44
    stfs f0, 0x28(r1)
    li r3, 0x0
    fneg f0, f2
    stfs f0, 0x20(r1)
    bl fn_801776E8
    fmr f1, f31
    addi r4, r1, 0x2c
    li r3, 0x0
    bl fn_80177574
    fmr f1, f31
    addi r4, r1, 0x20
    li r3, 0x0
    bl fn_80177478
@L_801174AC:
    psq_l f31, 0x88(r1), 0, 0
    lwz r0, 0x94(r1)
    lfd f31, 0x80(r1)
    mtlr r0
    addi r1, r1, 0x90
    blr
