    nofralloc
    stwu r1, -0x60(r1)
    mflr r0
    stw r0, 0x64(r1)
    stfd f31, 0x50(r1)
    psq_st f31, 0x58(r1), 0, 0
    stfd f30, 0x40(r1)
    psq_st f30, 0x48(r1), 0, 0
    stw r31, 0x3c(r1)
    stw r30, 0x38(r1)
    lwz r5, -26224(r2)
    fmr f31, f1
    lwz r0, -26220(r2)
    cmpwi r4, 0x0
    stw r5, 0x2c(r1)
    mr r31, r3
    stw r0, 0x30(r1)
    blt @L_8012C6AC
    cmpwi r4, 0x2
    blt @L_8012C6B4
@L_8012C6AC:
    li r0, 0x0
    b @L_8012C6C4
@L_8012C6B4:
    slwi r0, r4, 2
    addi r3, r1, 0x2c
    lwzx r4, r3, r0
    li r0, 0x1
@L_8012C6C4:
    clrlwi. r0, r0, 24
    bne @L_8012C6D4
    li r3, -0x1
    b @L_8012C6F4
@L_8012C6D4:
    li r3, 0x0
    bl fn_8018D998
    bl fn_8018D928
    cmplwi r3, 0x0
    bne @L_8012C6F0
    li r3, -0x1
    b @L_8012C6F4
@L_8012C6F0:
    lwz r3, 0x30(r3)
@L_8012C6F4:
    bl fn_8018F6F4
    addi r5, r1, 0x20
    mr r30, r3
    addi r6, r1, 0x8
    li r4, 0x1
    bl fn_8018F4C8
    mr r3, r30
    addi r5, r1, 0x1c
    addi r6, r1, 0x8
    li r4, 0x2
    bl fn_8018F4C8
    mr r3, r30
    addi r5, r1, 0x18
    addi r6, r1, 0x8
    li r4, 0x3
    bl fn_8018F4C8
    mr r3, r30
    addi r5, r1, 0x14
    addi r6, r1, 0x8
    li r4, 0x4
    bl fn_8018F4C8
    lfs f0, -26140(r2)
    fcmpo cr0, f31, f0
    ble @L_8012C758
    fmr f31, f0
@L_8012C758:
    mr r3, r31
    addi r4, r1, 0x28
    addi r5, r1, 0x24
    bl fn_800EC578
    lfs f0, -26136(r2)
    fcmpo cr0, f31, f0
    bge @L_8012C7F0
    lwz r3, 0x28(r1)
    lwz r0, 0x14(r1)
    cmpw r3, r0
    bne @L_8012C790
    lwz r0, 0x24(r1)
    cmpwi r0, -0x1
    beq @L_8012C7E0
@L_8012C790:
    lwz r0, 0x24(r1)
    lfs f30, -26216(r2)
    cmpwi r0, -0x1
    beq @L_8012C7C8
    mr r3, r31
    addi r4, r1, 0x10
    addi r5, r1, 0xc
    bl fn_800EC4D0
    mr r3, r31
    bl GSmodelGetAnimFrame
    lfs f2, 0x10(r1)
    lfs f0, 0xc(r1)
    fdivs f0, f2, f0
    fmuls f30, f0, f1
@L_8012C7C8:
    lwz r4, 0x14(r1)
    mr r3, r31
    bl fn_800ECCA8
    fmr f1, f30
    mr r3, r31
    bl fn_800ECA78
@L_8012C7E0:
    lfs f1, -26144(r2)
    mr r3, r31
    bl fn_800EC9DC
    b @L_8012CA5C
@L_8012C7F0:
    lfs f0, -26216(r2)
    fcmpo cr0, f31, f0
    bge @L_8012C89C
    lwz r3, 0x28(r1)
    lwz r0, 0x14(r1)
    cmpw r3, r0
    bne @L_8012C81C
    lwz r3, 0x24(r1)
    lwz r0, 0x20(r1)
    cmpw r3, r0
    beq @L_8012C870
@L_8012C81C:
    lwz r0, 0x24(r1)
    lfs f30, -26216(r2)
    cmpwi r0, -0x1
    bne @L_8012C838
    mr r3, r31
    bl GSmodelGetAnimFrame
    fmr f30, f1
@L_8012C838:
    lwz r4, 0x14(r1)
    mr r3, r31
    lwz r5, 0x20(r1)
    bl fn_800EC5FC
    mr r3, r31
    addi r4, r1, 0x10
    addi r5, r1, 0xc
    bl fn_800EC4D0
    lfs f1, 0xc(r1)
    mr r3, r31
    lfs f0, 0x10(r1)
    fdivs f0, f1, f0
    fmuls f1, f30, f0
    bl fn_800ECA78
@L_8012C870:
    lfs f1, -26136(r2)
    mr r3, r31
    lfs f0, -26132(r2)
    fsubs f1, f31, f1
    fdivs f31, f1, f0
    fmr f1, f31
    bl fn_800EC5B8
    lfs f1, -26208(r2)
    mr r3, r31
    bl fn_800EC9DC
    b @L_8012CA5C
@L_8012C89C:
    lfs f0, -26132(r2)
    fcmpo cr0, f31, f0
    bge @L_8012C940
    lwz r3, 0x28(r1)
    lwz r0, 0x1c(r1)
    cmpw r3, r0
    bne @L_8012C8C8
    lwz r3, 0x24(r1)
    lwz r0, 0x20(r1)
    cmpw r3, r0
    beq @L_8012C91C
@L_8012C8C8:
    lwz r0, 0x24(r1)
    lfs f30, -26216(r2)
    cmpwi r0, -0x1
    bne @L_8012C8E4
    mr r3, r31
    bl GSmodelGetAnimFrame
    fmr f30, f1
@L_8012C8E4:
    lwz r4, 0x1c(r1)
    mr r3, r31
    lwz r5, 0x20(r1)
    bl fn_800EC5FC
    mr r3, r31
    addi r4, r1, 0x10
    addi r5, r1, 0xc
    bl fn_800EC4D0
    lfs f1, 0xc(r1)
    mr r3, r31
    lfs f0, 0x10(r1)
    fdivs f0, f1, f0
    fmuls f1, f30, f0
    bl fn_800ECA78
@L_8012C91C:
    lfs f1, -26128(r2)
    mr r3, r31
    lfs f0, -26144(r2)
    fnmsubs f1, f1, f31, f0
    bl fn_800EC5B8
    lfs f1, -26208(r2)
    mr r3, r31
    bl fn_800EC9DC
    b @L_8012CA5C
@L_8012C940:
    lfs f0, -26144(r2)
    fcmpo cr0, f31, f0
    bge @L_8012C9C8
    lwz r3, 0x28(r1)
    lwz r0, 0x1c(r1)
    cmpw r3, r0
    bne @L_8012C968
    lwz r0, 0x24(r1)
    cmpwi r0, -0x1
    beq @L_8012C9B8
@L_8012C968:
    lwz r0, 0x24(r1)
    lfs f30, -26216(r2)
    cmpwi r0, -0x1
    beq @L_8012C9A0
    mr r3, r31
    addi r4, r1, 0x10
    addi r5, r1, 0xc
    bl fn_800EC4D0
    mr r3, r31
    bl GSmodelGetAnimFrame
    lfs f2, 0x10(r1)
    lfs f0, 0xc(r1)
    fdivs f0, f2, f0
    fmuls f30, f0, f1
@L_8012C9A0:
    lwz r4, 0x1c(r1)
    mr r3, r31
    bl fn_800ECCA8
    fmr f1, f30
    mr r3, r31
    bl fn_800ECA78
@L_8012C9B8:
    lfs f1, -26208(r2)
    mr r3, r31
    bl fn_800EC9DC
    b @L_8012CA5C
@L_8012C9C8:
    lwz r3, 0x28(r1)
    lwz r0, 0x1c(r1)
    cmpw r3, r0
    bne @L_8012C9E8
    lwz r3, 0x24(r1)
    lwz r0, 0x18(r1)
    cmpw r3, r0
    beq @L_8012CA3C
@L_8012C9E8:
    lwz r0, 0x24(r1)
    lfs f30, -26216(r2)
    cmpwi r0, -0x1
    bne @L_8012CA04
    mr r3, r31
    bl GSmodelGetAnimFrame
    fmr f30, f1
@L_8012CA04:
    lwz r4, 0x1c(r1)
    mr r3, r31
    lwz r5, 0x18(r1)
    bl fn_800EC5FC
    mr r3, r31
    addi r4, r1, 0x10
    addi r5, r1, 0xc
    bl fn_800EC4D0
    lfs f1, 0xc(r1)
    mr r3, r31
    lfs f0, 0x10(r1)
    fdivs f0, f1, f0
    fmuls f1, f30, f0
    bl fn_800ECA78
@L_8012CA3C:
    lfs f0, -26144(r2)
    mr r3, r31
    fsubs f31, f31, f0
    fmr f1, f31
    bl fn_800EC5B8
    lfs f1, -26208(r2)
    mr r3, r31
    bl fn_800EC9DC
@L_8012CA5C:
    psq_l f31, 0x58(r1), 0, 0
    lfd f31, 0x50(r1)
    psq_l f30, 0x48(r1), 0, 0
    lfd f30, 0x40(r1)
    lwz r31, 0x3c(r1)
    lwz r0, 0x64(r1)
    lwz r30, 0x38(r1)
    mtlr r0
    addi r1, r1, 0x60
    blr
