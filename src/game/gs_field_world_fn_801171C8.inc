    nofralloc
    stwu r1, -0x40(r1)
    mflr r0
    stw r0, 0x44(r1)
    stfd f31, 0x30(r1)
    psq_st f31, 0x38(r1), 0, 0
    stfd f30, 0x20(r1)
    psq_st f30, 0x28(r1), 0, 0
    lbz r0, lbl_8047AD71(r13)
    cmplwi r0, 0x0
    beq @L_80117310
    bl GSscene_GetMode
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_80117310
    lwz r0, lbl_8047AD68(r13)
    cmplwi r0, 0x0
    beq @L_80117310
    cmplwi r0, 0x1
    bne @L_8011723C
    lwz r3, lbl_8047AD6C(r13)
    lfs f30, 0x14(r3)
    lfs f31, 0xc(r3)
    lfs f1, 0x10(r3)
    bl fn_80176868
    fmr f1, f31
    bl fn_801767E0
    fmr f1, f30
    bl fn_80176758
    b @L_80117310
@L_8011723C:
    addi r3, r1, 0x14
    bl GSscene_GetCameraPositionVector
    lfs f2, lbl_8047AD74(r13)
    addi r3, r1, 0x14
    lfs f1, lbl_8047AD78(r13)
    addi r4, r1, 0xc
    lfs f0, lbl_8047AD7C(r13)
    addi r5, r1, 0x10
    stfs f2, 0xc(r1)
    addi r6, r1, 0x8
    stfs f1, 0x10(r1)
    stfs f0, 0x8(r1)
    bl floorUpdateFieldCamera
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    beq @L_80117310
    lbz r0, lbl_8047AD70(r13)
    cmplwi r0, 0x0
    beq @L_801172D0
    lfs f3, -26312(r2)
    lfs f2, 0xc(r1)
    lfs f1, 0x10(r1)
    lfs f0, 0x8(r1)
    fmuls f4, f3, f2
    fmuls f2, f3, f1
    lfs f6, -26316(r2)
    lfs f5, lbl_8047AD74(r13)
    fmuls f0, f3, f0
    lfs f3, lbl_8047AD78(r13)
    lfs f1, lbl_8047AD7C(r13)
    fmadds f4, f6, f5, f4
    fmadds f2, f6, f3, f2
    fmadds f0, f6, f1, f0
    stfs f4, lbl_8047AD74(r13)
    stfs f2, lbl_8047AD78(r13)
    stfs f0, lbl_8047AD7C(r13)
    b @L_801172F0
@L_801172D0:
    lfs f2, 0xc(r1)
    li r0, 0x1
    lfs f1, 0x10(r1)
    lfs f0, 0x8(r1)
    stfs f2, lbl_8047AD74(r13)
    stfs f1, lbl_8047AD78(r13)
    stfs f0, lbl_8047AD7C(r13)
    stb r0, lbl_8047AD70(r13)
@L_801172F0:
    lfs f31, lbl_8047AD7C(r13)
    lfs f30, lbl_8047AD78(r13)
    lfs f1, lbl_8047AD74(r13)
    bl fn_80176868
    fmr f1, f30
    bl fn_801767E0
    fmr f1, f31
    bl fn_80176758
@L_80117310:
    psq_l f31, 0x38(r1), 0, 0
    lfd f31, 0x30(r1)
    psq_l f30, 0x28(r1), 0, 0
    lwz r0, 0x44(r1)
    lfd f30, 0x20(r1)
    mtlr r0
    addi r1, r1, 0x40
    blr
