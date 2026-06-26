    nofralloc
    stwu r1, -0xa0(r1)
    mflr r0
    stw r0, 0xa4(r1)
    stfd f31, 0x90(r1)
    psq_st f31, 0x98(r1), 0, 0
    stfd f30, 0x80(r1)
    psq_st f30, 0x88(r1), 0, 0
    stmw r26, 0x68(r1)
    mr. r26, r3
    fmr f31, f1
    mr r27, r4
    mr r28, r5
    blt @L_8012B1D8
    cmpwi r26, 0x2
    blt @L_8012B1E0
@L_8012B1D8:
    li r0, 0x0
    b @L_8012B208
@L_8012B1E0:
    lis r3, lbl_80426BD0@ha
    slwi r0, r26, 5
    addi r3, r3, lbl_80426BD0@l
    add r3, r3, r0
    lhz r0, 0x4(r3)
    clrlwi. r0, r0, 31
    bne @L_8012B204
    li r0, 0x0
    b @L_8012B208
@L_8012B204:
    li r0, 0x1
@L_8012B208:
    clrlwi. r0, r0, 24
    bne @L_8012B218
    li r3, 0x0
    b @L_8012B5C0
@L_8012B218:
    cmplwi r27, 0x0
    beq @L_8012B23C
    lfs f0, 0x0(r27)
    stfs f0, 0x2c(r1)
    lfs f0, 0x4(r27)
    stfs f0, 0x30(r1)
    lfs f0, 0x8(r27)
    stfs f0, 0x34(r1)
    b @L_8012B278
@L_8012B23C:
    lwz r3, -26224(r2)
    cmpwi r26, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x14(r1)
    stw r0, 0x18(r1)
    blt @L_8012B268
    cmpwi r26, 0x2
    bge @L_8012B268
    slwi r0, r26, 2
    addi r3, r1, 0x14
    lwzx r4, r3, r0
@L_8012B268:
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x2c
    bl fn_800E3D98
@L_8012B278:
    lfs f0, 0x30(r1)
    cmpwi r26, 0x0
    lfs f1, -26212(r2)
    lwz r3, -26224(r2)
    fadds f0, f0, f1
    lwz r0, -26220(r2)
    stw r3, 0x1c(r1)
    stfs f0, 0x30(r1)
    lfs f0, 0x0(r28)
    stw r0, 0x20(r1)
    stfs f0, 0x38(r1)
    lfs f0, 0x4(r28)
    fadds f0, f1, f0
    stfs f0, 0x3c(r1)
    lfs f0, 0x8(r28)
    stfs f0, 0x40(r1)
    blt @L_8012B2C4
    cmpwi r26, 0x2
    blt @L_8012B2CC
@L_8012B2C4:
    li r0, 0x0
    b @L_8012B2DC
@L_8012B2CC:
    slwi r0, r26, 2
    addi r3, r1, 0x1c
    lwzx r31, r3, r0
    li r0, 0x1
@L_8012B2DC:
    clrlwi. r0, r0, 24
    bne @L_8012B2EC
    li r3, -0x1
    b @L_8012B310
@L_8012B2EC:
    mr r4, r31
    li r3, 0x0
    bl fn_8018D998
    bl fn_8018D928
    cmplwi r3, 0x0
    bne @L_8012B30C
    li r3, -0x1
    b @L_8012B310
@L_8012B30C:
    lwz r3, 0x30(r3)
@L_8012B310:
    addis r0, r3, 0x1
    cmplwi r0, 0xffff
    bne @L_8012B324
    li r3, 0x0
    b @L_8012B5C0
@L_8012B324:
    bl fn_8018F6F4
    cmplwi r3, 0x0
    bne @L_8012B338
    li r3, 0x0
    b @L_8012B5C0
@L_8012B338:
    bl fn_8018F5E4
    lfs f0, -26208(r2)
    addi r3, r1, 0x2c
    addi r4, r1, 0x38
    li r5, 0x0
    fmuls f30, f0, f1
    fmr f1, f30
    bl fn_8010F320
    cmpwi r3, 0x0
    beq @L_8012B368
    li r3, 0x0
    b @L_8012B5C0
@L_8012B368:
    lwz r3, -26224(r2)
    cmpwi r26, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x24(r1)
    stw r0, 0x28(r1)
    blt @L_8012B394
    cmpwi r26, 0x2
    bge @L_8012B394
    slwi r0, r26, 2
    addi r3, r1, 0x24
    lwzx r29, r3, r0
@L_8012B394:
    mr r4, r29
    li r3, 0x0
    bl fn_8018D998
    bl fn_8018D928
    cmplwi r3, 0x0
    bne @L_8012B3B4
    li r3, 0x0
    b @L_8012B5C0
@L_8012B3B4:
    cmplwi r27, 0x0
    lwz r31, 0x50(r3)
    beq @L_8012B3DC
    lfs f0, 0x0(r27)
    stfs f0, 0x2c(r1)
    lfs f0, 0x4(r27)
    stfs f0, 0x30(r1)
    lfs f0, 0x8(r27)
    stfs f0, 0x34(r1)
    b @L_8012B41C
@L_8012B3DC:
    lwz r3, -26224(r2)
    cmpwi r26, 0x0
    lwz r0, -26220(r2)
    stw r3, 0xc(r1)
    stw r0, 0x10(r1)
    blt @L_8012B408
    cmpwi r26, 0x2
    bge @L_8012B408
    slwi r0, r26, 2
    addi r3, r1, 0xc
    lwzx r30, r3, r0
@L_8012B408:
    mr r4, r30
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x2c
    bl fn_800E3D98
@L_8012B41C:
    lfs f0, 0x0(r28)
    addi r3, r1, 0x38
    addi r4, r1, 0x2c
    addi r5, r1, 0x50
    stfs f0, 0x38(r1)
    lfs f0, 0x4(r28)
    stfs f0, 0x3c(r1)
    lfs f0, 0x8(r28)
    stfs f0, 0x40(r1)
    bl fn_800A3A9C
    lfs f3, 0x2c(r1)
    fadds f5, f30, f31
    lfs f0, 0x38(r1)
    lfs f2, 0x34(r1)
    lfs f1, 0x40(r1)
    fsubs f3, f3, f0
    lfs f0, -26216(r2)
    fsubs f1, f2, f1
    fmuls f2, f3, f3
    fmuls f1, f1, f1
    fadds f4, f2, f1
    fcmpo cr0, f4, f0
    ble @L_8012B4C0
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
    b @L_8012B544
@L_8012B4C0:
    lfd f0, -26184(r2)
    fcmpo cr0, f4, f0
    bge @L_8012B4D8
    lis r3, lbl_80478AC0@ha
    lfs f4, lbl_80478AC0@l(r3)
    b @L_8012B544
@L_8012B4D8:
    stfs f4, 0x8(r1)
    lis r0, 0x7f80
    lwz r4, 0x8(r1)
    rlwinm r3, r4, 0, 1, 8
    cmpw r3, r0
    beq @L_8012B500
    bge @L_8012B530
    cmpwi r3, 0x0
    beq @L_8012B518
    b @L_8012B530
@L_8012B500:
    clrlwi. r0, r4, 9
    beq @L_8012B510
    li r0, 0x1
    b @L_8012B534
@L_8012B510:
    li r0, 0x2
    b @L_8012B534
@L_8012B518:
    clrlwi. r0, r4, 9
    beq @L_8012B528
    li r0, 0x5
    b @L_8012B534
@L_8012B528:
    li r0, 0x3
    b @L_8012B534
@L_8012B530:
    li r0, 0x4
@L_8012B534:
    cmpwi r0, 0x1
    bne @L_8012B544
    lis r3, lbl_80478AC0@ha
    lfs f4, lbl_80478AC0@l(r3)
@L_8012B544:
    lfs f0, -26216(r2)
    fcmpo cr0, f4, f0
    cror eq, lt, eq
    bne @L_8012B55C
    li r3, 0x1
    b @L_8012B5C0
@L_8012B55C:
    fcmpo cr0, f4, f5
    cror eq, lt, eq
    bne @L_8012B570
    li r3, 0x1
    b @L_8012B5C0
@L_8012B570:
    fsubs f1, f4, f5
    lfs f0, -26176(r2)
    addi r3, r1, 0x50
    addi r4, r1, 0x44
    fsubs f0, f1, f0
    fdivs f1, f0, f4
    bl fn_800A3AC0
    addi r3, r1, 0x2c
    addi r4, r1, 0x44
    addi r5, r1, 0x38
    bl fn_800A3A78
    mr r3, r31
    addi r4, r1, 0x2c
    addi r5, r1, 0x38
    li r6, 0x0
    bl fn_8010FDF8
    subfic r4, r3, 0x6
    subi r0, r3, 0x6
    or r0, r4, r0
    srwi r3, r0, 31
@L_8012B5C0:
    psq_l f31, 0x98(r1), 0, 0
    lfd f31, 0x90(r1)
    psq_l f30, 0x88(r1), 0, 0
    lfd f30, 0x80(r1)
    lmw r26, 0x68(r1)
    lwz r0, 0xa4(r1)
    mtlr r0
    addi r1, r1, 0xa0
    blr
