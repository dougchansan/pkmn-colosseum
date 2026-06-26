    nofralloc
    stwu r1, -0x70(r1)
    mflr r0
    stw r0, 0x74(r1)
    stfd f31, 0x60(r1)
    psq_st f31, 0x68(r1), 0, 0
    stmw r26, 0x48(r1)
    mr r29, r3
    mr r26, r4
    li r3, 0x0
    li r4, 0x2
    bl fn_800F9318
    lwz r3, -26224(r2)
    cmpwi r29, 0x0
    lwz r0, -26220(r2)
    stw r3, 0xc(r1)
    stw r0, 0x10(r1)
    stw r3, 0x1c(r1)
    stw r0, 0x20(r1)
    blt @L_8012E3E8
    cmpwi r29, 0x2
    bge @L_8012E3E8
    slwi r0, r29, 2
    addi r3, r1, 0x1c
    lwzx r27, r3, r0
@L_8012E3E8:
    lwz r3, 0xc(r1)
    cmpwi r29, 0x0
    lwz r0, 0x10(r1)
    stw r3, 0x14(r1)
    stw r0, 0x18(r1)
    blt @L_8012E414
    cmpwi r29, 0x2
    bge @L_8012E414
    slwi r0, r29, 2
    addi r3, r1, 0x14
    lwzx r28, r3, r0
@L_8012E414:
    mr r4, r28
    li r3, 0x0
    bl fn_800F9318
    li r3, 0x1
    li r4, 0x1
    bl fn_800F7A7C
    mr r0, r3
    li r3, 0x1
    mr r31, r0
    li r4, 0x1
    bl fn_800F7A08
    mr r0, r3
    li r3, 0x1
    mr r30, r0
    li r4, 0x0
    bl fn_800F7A7C
    mr r0, r3
    li r3, 0x1
    mr r29, r0
    li r4, 0x0
    bl fn_800F7A08
    extsb. r0, r31
    mr r28, r3
    bne @L_8012E4E8
    extsb. r0, r30
    bne @L_8012E4E8
    li r3, 0x1
    bl fn_800F7BC4
    rlwinm. r0, r3, 0, 28, 28
    beq @L_8012E490
    li r30, -0x38
@L_8012E490:
    li r3, 0x1
    bl fn_800F7BC4
    rlwinm. r0, r3, 0, 29, 29
    beq @L_8012E4A4
    li r30, 0x38
@L_8012E4A4:
    li r3, 0x1
    bl fn_800F7BC4
    clrlwi. r0, r3, 31
    beq @L_8012E4B8
    li r31, -0x38
@L_8012E4B8:
    li r3, 0x1
    bl fn_800F7BC4
    rlwinm. r0, r3, 0, 30, 30
    beq @L_8012E4CC
    li r31, 0x38
@L_8012E4CC:
    extsb. r0, r31
    mr r29, r31
    mr r28, r30
    bne @L_8012E4E8
    extsb. r0, r30
    bne @L_8012E4E8
    bl GSscene_GetMode
@L_8012E4E8:
    extsb. r0, r31
    bne @L_8012E4F8
    extsb. r0, r30
    beq @L_8012E788
@L_8012E4F8:
    extsb r0, r31
    cmpwi r0, 0x38
    ble @L_8012E50C
    li r31, 0x38
    b @L_8012E518
@L_8012E50C:
    cmpwi r0, -0x38
    bge @L_8012E518
    li r31, -0x38
@L_8012E518:
    extsb r0, r30
    cmpwi r0, 0x38
    ble @L_8012E52C
    li r30, 0x38
    b @L_8012E538
@L_8012E52C:
    cmpwi r0, -0x38
    bge @L_8012E538
    li r30, -0x38
@L_8012E538:
    extsb. r0, r31
    extsb r0, r31
    neg r3, r0
    ble @L_8012E54C
    mr r3, r0
@L_8012E54C:
    xoris r0, r3, 0x8000
    lis r4, 0x4330
    stw r0, 0x34(r1)
    extsb r0, r30
    lfd f2, -26072(r2)
    extsb. r3, r30
    stw r4, 0x30(r1)
    neg r3, r0
    lfs f0, -26092(r2)
    lfd f1, 0x30(r1)
    fsubs f1, f1, f2
    fdivs f5, f1, f0
    ble @L_8012E584
    mr r3, r0
@L_8012E584:
    xoris r3, r3, 0x8000
    lis r0, 0x4330
    stw r3, 0x3c(r1)
    lfd f3, -26072(r2)
    stw r0, 0x38(r1)
    lfs f1, -26092(r2)
    lfd f2, 0x38(r1)
    lfs f0, -26216(r2)
    fsubs f2, f2, f3
    fdivs f6, f2, f1
    fmuls f1, f6, f6
    fmadds f4, f5, f5, f1
    fcmpo cr0, f4, f0
    ble @L_8012E604
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
    b @L_8012E688
@L_8012E604:
    lfd f0, -26184(r2)
    fcmpo cr0, f4, f0
    bge @L_8012E61C
    lis r3, lbl_80478AC0@ha
    lfs f4, lbl_80478AC0@l(r3)
    b @L_8012E688
@L_8012E61C:
    stfs f4, 0x8(r1)
    lis r0, 0x7f80
    lwz r4, 0x8(r1)
    rlwinm r3, r4, 0, 1, 8
    cmpw r3, r0
    beq @L_8012E644
    bge @L_8012E674
    cmpwi r3, 0x0
    beq @L_8012E65C
    b @L_8012E674
@L_8012E644:
    clrlwi. r0, r4, 9
    beq @L_8012E654
    li r0, 0x1
    b @L_8012E678
@L_8012E654:
    li r0, 0x2
    b @L_8012E678
@L_8012E65C:
    clrlwi. r0, r4, 9
    beq @L_8012E66C
    li r0, 0x5
    b @L_8012E678
@L_8012E66C:
    li r0, 0x3
    b @L_8012E678
@L_8012E674:
    li r0, 0x4
@L_8012E678:
    cmpwi r0, 0x1
    bne @L_8012E688
    lis r3, lbl_80478AC0@ha
    lfs f4, lbl_80478AC0@l(r3)
@L_8012E688:
    stfs f4, 0x0(r26)
    lfs f0, -26140(r2)
    lfs f1, 0x0(r26)
    fcmpo cr0, f1, f0
    ble @L_8012E6A0
    stfs f0, 0x0(r26)
@L_8012E6A0:
    extsb r0, r29
    cmpwi r0, -0x2
    ble @L_8012E6C8
    cmpwi r0, 0x2
    bge @L_8012E6C8
    extsb r0, r28
    cmpwi r0, -0x2
    ble @L_8012E6C8
    cmpwi r0, 0x2
    blt @L_8012E760
@L_8012E6C8:
    lfs f0, -26088(r2)
    fcmpo cr0, f6, f0
    bge @L_8012E6DC
    lfs f2, -26084(r2)
    b @L_8012E708
@L_8012E6DC:
    fdivs f1, f5, f6
    lfs f0, -26152(r2)
    fcmpo cr0, f1, f0
    ble @L_8012E6F0
    fmr f1, f0
@L_8012E6F0:
    lfs f0, -26080(r2)
    fdivs f1, f1, f0
    bl sin
    frsp f1, f1
    lfs f0, -26084(r2)
    fmuls f2, f0, f1
@L_8012E708:
    extsb. r0, r30
    blt @L_8012E718
    fmr f31, f2
    b @L_8012E720
@L_8012E718:
    lfs f0, -26076(r2)
    fsubs f31, f0, f2
@L_8012E720:
    extsb. r0, r31
    bge @L_8012E748
    extsb. r0, r30
    blt @L_8012E740
    lfs f1, -26076(r2)
    fsubs f0, f1, f2
    fadds f31, f1, f0
    b @L_8012E748
@L_8012E740:
    lfs f0, -26076(r2)
    fadds f31, f0, f2
@L_8012E748:
    bl fn_80176684
    fadds f1, f31, f1
    lfs f2, 0x0(r26)
    mr r4, r27
    li r3, 0x0
    bl fn_8018805C
@L_8012E760:
    lfs f1, 0x0(r26)
    mr r4, r27
    li r3, 0x0
    bl fn_80188214
    li r3, 0x0
    li r4, 0x7d0
    bl fn_800F9318
    addi r4, r1, 0x24
    bl fn_80166458
    b @L_8012E79C
@L_8012E788:
    lfs f0, -26216(r2)
    mr r4, r27
    li r3, 0x0
    stfs f0, 0x0(r26)
    bl fn_8018790C
@L_8012E79C:
    psq_l f31, 0x68(r1), 0, 0
    lfd f31, 0x60(r1)
    lmw r26, 0x48(r1)
    lwz r0, 0x74(r1)
    mtlr r0
    addi r1, r1, 0x70
    blr
