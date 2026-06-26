    nofralloc
    stwu r1, -0xc0(r1)
    mflr r0
    stw r0, 0xc4(r1)
    stfd f31, 0xb0(r1)
    psq_st f31, 0xb8(r1), 0, 0
    stfd f30, 0xa0(r1)
    psq_st f30, 0xa8(r1), 0, 0
    stmw r24, 0x80(r1)
    mr. r24, r3
    blt @L_8012B614
    cmpwi r24, 0x2
    blt @L_8012B61C
@L_8012B614:
    li r0, 0x0
    b @L_8012B644
@L_8012B61C:
    lis r3, lbl_80426BD0@ha
    slwi r0, r24, 5
    addi r3, r3, lbl_80426BD0@l
    add r3, r3, r0
    lhz r0, 0x4(r3)
    clrlwi. r0, r0, 31
    bne @L_8012B640
    li r0, 0x0
    b @L_8012B644
@L_8012B640:
    li r0, 0x1
@L_8012B644:
    clrlwi. r0, r0, 24
    bne @L_8012B654
    li r3, 0x0
    b @L_8012BAAC
@L_8012B654:
    lwz r3, -26224(r2)
    lis r5, lbl_80426BD0@ha
    lwz r0, -26220(r2)
    cmpwi r24, 0x0
    stw r3, 0x24(r1)
    lwz r26, lbl_80426BD0@l(r5)
    stw r0, 0x28(r1)
    blt @L_8012B688
    cmpwi r24, 0x2
    bge @L_8012B688
    slwi r0, r24, 2
    addi r3, r1, 0x24
    lwzx r4, r3, r0
@L_8012B688:
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x68
    bl fn_800E3D98
    lfs f1, 0x6c(r1)
    cmpwi r26, 0x0
    lfs f0, -26212(r2)
    lwz r3, -26224(r2)
    fadds f0, f1, f0
    lwz r0, -26220(r2)
    stw r3, 0x1c(r1)
    stfs f0, 0x6c(r1)
    stw r0, 0x20(r1)
    blt @L_8012B6D4
    cmpwi r26, 0x2
    bge @L_8012B6D4
    slwi r0, r26, 2
    addi r3, r1, 0x1c
    lwzx r29, r3, r0
@L_8012B6D4:
    mr r4, r29
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x5c
    bl fn_800E3D98
    lfs f1, 0x60(r1)
    cmpwi r24, 0x0
    lfs f0, -26212(r2)
    lwz r3, -26224(r2)
    fadds f0, f1, f0
    lwz r0, -26220(r2)
    stw r3, 0x34(r1)
    stfs f0, 0x60(r1)
    stw r0, 0x38(r1)
    blt @L_8012B718
    cmpwi r24, 0x2
    blt @L_8012B720
@L_8012B718:
    li r0, 0x0
    b @L_8012B730
@L_8012B720:
    slwi r0, r24, 2
    addi r3, r1, 0x34
    lwzx r31, r3, r0
    li r0, 0x1
@L_8012B730:
    clrlwi. r0, r0, 24
    bne @L_8012B740
    li r3, -0x1
    b @L_8012B764
@L_8012B740:
    mr r4, r31
    li r3, 0x0
    bl fn_8018D998
    bl fn_8018D928
    cmplwi r3, 0x0
    bne @L_8012B760
    li r3, -0x1
    b @L_8012B764
@L_8012B760:
    lwz r3, 0x30(r3)
@L_8012B764:
    addis r0, r3, 0x1
    cmplwi r0, 0xffff
    bne @L_8012B778
    li r3, 0x0
    b @L_8012BAAC
@L_8012B778:
    bl fn_8018F6F4
    cmplwi r3, 0x0
    bne @L_8012B78C
    li r3, 0x0
    b @L_8012BAAC
@L_8012B78C:
    bl fn_8018F5E4
    fmr f30, f1
    addi r3, r1, 0x68
    addi r4, r1, 0x5c
    li r5, 0x0
    bl fn_8010F320
    cmpwi r3, 0x0
    beq @L_8012B7B4
    li r3, 0x0
    b @L_8012BAAC
@L_8012B7B4:
    lwz r3, -26224(r2)
    cmpwi r26, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x2c(r1)
    stw r0, 0x30(r1)
    blt @L_8012B7D4
    cmpwi r26, 0x2
    blt @L_8012B7DC
@L_8012B7D4:
    li r0, 0x0
    b @L_8012B7EC
@L_8012B7DC:
    slwi r0, r26, 2
    addi r3, r1, 0x2c
    lwzx r30, r3, r0
    li r0, 0x1
@L_8012B7EC:
    clrlwi. r0, r0, 24
    bne @L_8012B7FC
    li r3, -0x1
    b @L_8012B820
@L_8012B7FC:
    mr r4, r30
    li r3, 0x0
    bl fn_8018D998
    bl fn_8018D928
    cmplwi r3, 0x0
    bne @L_8012B81C
    li r3, -0x1
    b @L_8012B820
@L_8012B81C:
    lwz r3, 0x30(r3)
@L_8012B820:
    addis r0, r3, 0x1
    cmplwi r0, 0xffff
    bne @L_8012B834
    li r3, 0x0
    b @L_8012BAAC
@L_8012B834:
    bl fn_8018F6F4
    cmplwi r3, 0x0
    bne @L_8012B848
    li r3, 0x0
    b @L_8012BAAC
@L_8012B848:
    bl fn_8018F5E4
    lwz r3, -26224(r2)
    fmr f31, f1
    lwz r0, -26220(r2)
    cmpwi r24, 0x0
    stw r3, 0x3c(r1)
    stw r0, 0x40(r1)
    blt @L_8012B87C
    cmpwi r24, 0x2
    bge @L_8012B87C
    slwi r0, r24, 2
    addi r3, r1, 0x3c
    lwzx r25, r3, r0
@L_8012B87C:
    mr r4, r25
    li r3, 0x0
    bl fn_8018D998
    bl fn_8018D928
    cmplwi r3, 0x0
    bne @L_8012B89C
    li r3, 0x0
    b @L_8012BAAC
@L_8012B89C:
    lwz r4, -26224(r2)
    cmpwi r24, 0x0
    lwz r0, -26220(r2)
    stw r4, 0x14(r1)
    lwz r29, 0x50(r3)
    stw r0, 0x18(r1)
    blt @L_8012B8CC
    cmpwi r24, 0x2
    bge @L_8012B8CC
    slwi r0, r24, 2
    addi r3, r1, 0x14
    lwzx r28, r3, r0
@L_8012B8CC:
    mr r4, r28
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x68
    bl fn_800E3D98
    lwz r3, -26224(r2)
    cmpwi r26, 0x0
    lwz r0, -26220(r2)
    stw r3, 0xc(r1)
    stw r0, 0x10(r1)
    blt @L_8012B90C
    cmpwi r26, 0x2
    bge @L_8012B90C
    slwi r0, r26, 2
    addi r3, r1, 0xc
    lwzx r27, r3, r0
@L_8012B90C:
    mr r4, r27
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x5c
    bl fn_800E3D98
    addi r3, r1, 0x5c
    addi r4, r1, 0x68
    addi r5, r1, 0x50
    bl fn_800A3A9C
    lfs f3, 0x68(r1)
    fadds f5, f30, f31
    lfs f0, 0x5c(r1)
    lfs f2, 0x70(r1)
    lfs f1, 0x64(r1)
    fsubs f3, f3, f0
    lfs f0, -26216(r2)
    fsubs f1, f2, f1
    fmuls f2, f3, f3
    fmuls f1, f1, f1
    fadds f4, f2, f1
    fcmpo cr0, f4, f0
    ble @L_8012B9AC
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
    b @L_8012BA30
@L_8012B9AC:
    lfd f0, -26184(r2)
    fcmpo cr0, f4, f0
    bge @L_8012B9C4
    lis r3, lbl_80478AC0@ha
    lfs f4, lbl_80478AC0@l(r3)
    b @L_8012BA30
@L_8012B9C4:
    stfs f4, 0x8(r1)
    lis r0, 0x7f80
    lwz r4, 0x8(r1)
    rlwinm r3, r4, 0, 1, 8
    cmpw r3, r0
    beq @L_8012B9EC
    bge @L_8012BA1C
    cmpwi r3, 0x0
    beq @L_8012BA04
    b @L_8012BA1C
@L_8012B9EC:
    clrlwi. r0, r4, 9
    beq @L_8012B9FC
    li r0, 0x1
    b @L_8012BA20
@L_8012B9FC:
    li r0, 0x2
    b @L_8012BA20
@L_8012BA04:
    clrlwi. r0, r4, 9
    beq @L_8012BA14
    li r0, 0x5
    b @L_8012BA20
@L_8012BA14:
    li r0, 0x3
    b @L_8012BA20
@L_8012BA1C:
    li r0, 0x4
@L_8012BA20:
    cmpwi r0, 0x1
    bne @L_8012BA30
    lis r3, lbl_80478AC0@ha
    lfs f4, lbl_80478AC0@l(r3)
@L_8012BA30:
    lfs f0, -26216(r2)
    fcmpo cr0, f4, f0
    cror eq, lt, eq
    bne @L_8012BA48
    li r3, 0x1
    b @L_8012BAAC
@L_8012BA48:
    fcmpo cr0, f4, f5
    cror eq, lt, eq
    bne @L_8012BA5C
    li r3, 0x1
    b @L_8012BAAC
@L_8012BA5C:
    fsubs f1, f4, f5
    lfs f0, -26176(r2)
    addi r3, r1, 0x50
    addi r4, r1, 0x44
    fsubs f0, f1, f0
    fdivs f1, f0, f4
    bl fn_800A3AC0
    addi r3, r1, 0x68
    addi r4, r1, 0x44
    addi r5, r1, 0x5c
    bl fn_800A3A78
    mr r3, r29
    addi r4, r1, 0x68
    addi r5, r1, 0x5c
    li r6, 0x0
    bl fn_8010FDF8
    subfic r4, r3, 0x6
    subi r0, r3, 0x6
    or r0, r4, r0
    srwi r3, r0, 31
@L_8012BAAC:
    psq_l f31, 0xb8(r1), 0, 0
    lfd f31, 0xb0(r1)
    psq_l f30, 0xa8(r1), 0, 0
    lfd f30, 0xa0(r1)
    lmw r24, 0x80(r1)
    lwz r0, 0xc4(r1)
    mtlr r0
    addi r1, r1, 0xc0
    blr
