    nofralloc
    stwu r1, -0x60(r1)
    mflr r0
    stw r0, 0x64(r1)
    stfd f31, 0x50(r1)
    psq_st f31, 0x58(r1), 0, 0
    stmw r23, 0x2c(r1)
    mr r24, r3
    mr r25, r4
    mr r23, r5
    mr r26, r6
    mr r27, r7
    lfs f0, -26368(r2)
    li r28, -0x1
    lwz r4, lbl_80478EC8(r13)
    stfs f0, 0x8(r1)
    lwz r0, 0x0(r4)
    cmplw r25, r0
    blt @L_801164C0
    li r3, -0x1
    b @L_8011693C
@L_801164C0:
    mulli r0, r25, 0x18
    lwz r4, lbl_80478ECC(r13)
    add r30, r4, r0
    lwz r4, 0x14(r30)
    cmplwi r4, 0x0
    bne @L_801164E0
    li r3, -0x1
    b @L_8011693C
@L_801164E0:
    bl fn_80113F6C
    mr r29, r3
    cmplwi r29, 0x0
    bne @L_801164F8
    li r3, -0x1
    b @L_8011693C
@L_801164F8:
    mr r3, r24
    bl fn_80115C48
    bl fn_80115A80
    clrlwi r31, r23, 16
    mr r23, r3
    cmpwi r31, 0x83
    bge @L_80116534
    cmpwi r31, 0x3
    bge @L_80116528
    cmpwi r31, 0x1
    bge @L_80116540
    b @L_80116798
@L_80116528:
    cmpwi r31, 0x81
    bge @L_80116540
    b @L_80116798
@L_80116534:
    cmpwi r31, 0xc0
    beq @L_80116750
    b @L_80116798
@L_80116540:
    lbz r0, 0x0(r30)
    extsb r4, r0
    cmpwi r4, 0x0
    blt @L_80116590
    cmplwi r29, 0x0
    lfs f31, 0x8(r1)
    beq @L_80116590
    mr r3, r29
    bl fn_800ECCA8
    fmr f1, f31
    mr r3, r29
    bl fn_800ECA78
    lfs f1, -26364(r2)
    mr r3, r29
    bl fn_800EC9DC
    mr r3, r29
    li r4, 0x0
    bl fn_800ECB74
    mr r3, r29
    bl fn_800EC990
@L_80116590:
    rlwinm r24, r31, 0, 24, 24
    cmpwi r24, 0x0
    beq @L_801165A4
    li r3, 0x44
    bl fn_80166A28
@L_801165A4:
    cmplwi r29, 0x0
    bne @L_801165B8
    b @L_801165CC
    b @L_801165B8
@L_801165B4:
    bl _threadSwitch
@L_801165B8:
    mr r3, r29
    bl fn_800EC960
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_801165B4
@L_801165CC:
    lbz r4, 0x8(r30)
    mr r3, r29
    bl fn_800EE150
    addi r4, r1, 0xc
    mr r25, r3
    li r5, 0x0
    li r6, 0x0
    bl fn_800EE3BC
    mr r3, r25
    bl fn_800EE828
    clrlwi r0, r31, 31
    cmpwi r0, 0x0
    beq @L_8011664C
    mr r3, r26
    mr r4, r27
    addi r6, r1, 0xc
    li r5, 0x1
    bl fn_8018AACC
    mr r3, r26
    mr r4, r27
    li r5, 0x1
    bl fn_8018A280
    lfs f1, -26368(r2)
    mr r3, r26
    lfs f2, -26364(r2)
    mr r4, r27
    bl fn_8018805C
    mr r3, r26
    mr r4, r27
    li r5, 0x1
    bl fn_8018A280
    b @L_801166C4
@L_8011664C:
    mr r3, r26
    mr r4, r27
    bl fn_80184470
    lbz r4, 0x8(r30)
    mr r3, r29
    bl fn_800EE150
    addi r4, r1, 0xc
    mr r25, r3
    li r5, 0x0
    li r6, 0x0
    bl fn_800EE3BC
    mr r3, r25
    bl fn_800EE828
    mr r3, r26
    mr r4, r27
    addi r5, r1, 0xc
    bl fn_8018C0A8
    lfs f1, 0x14(r1)
    mr r3, r26
    lfs f0, -26360(r2)
    mr r4, r27
    addi r6, r1, 0xc
    li r5, 0x1
    fadds f0, f1, f0
    stfs f0, 0x14(r1)
    bl fn_8018AACC
    mr r3, r26
    mr r4, r27
    li r5, 0x1
    bl fn_8018A280
@L_801166C4:
    lbz r0, 0x1(r30)
    extsb r4, r0
    cmpwi r4, 0x0
    blt @L_80116714
    cmplwi r29, 0x0
    lfs f31, 0x8(r1)
    beq @L_80116714
    mr r3, r29
    bl fn_800ECCA8
    fmr f1, f31
    mr r3, r29
    bl fn_800ECA78
    lfs f1, -26364(r2)
    mr r3, r29
    bl fn_800EC9DC
    mr r3, r29
    li r4, 0x0
    bl fn_800ECB74
    mr r3, r29
    bl fn_800EC990
@L_80116714:
    cmpwi r24, 0x0
    beq @L_80116724
    li r3, 0x44
    bl fn_80166A28
@L_80116724:
    cmplwi r29, 0x0
    bne @L_80116738
    b @L_801168E8
    b @L_80116738
@L_80116734:
    bl _threadSwitch
@L_80116738:
    mr r3, r29
    bl fn_800EC960
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_80116734
    b @L_801168E8
@L_80116750:
    cmplwi r29, 0x0
    bne @L_80116764
    b @L_80116778
    b @L_80116764
@L_80116760:
    bl _threadSwitch
@L_80116764:
    mr r3, r29
    bl fn_800EC960
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_80116760
@L_80116778:
    rlwinm r0, r31, 0, 24, 24
    cmpwi r0, 0x0
    beq @L_801168E8
    li r3, 0x45
    bl fn_801669BC
    li r3, 0x46
    bl fn_80166A28
    b @L_801168E8
@L_80116798:
    rlwinm r0, r31, 0, 29, 29
    cmpwi r0, 0x0
    beq @L_801167B0
    lbz r28, 0x5(r30)
    extsb r28, r28
    b @L_801167C4
@L_801167B0:
    rlwinm r0, r31, 0, 28, 28
    cmpwi r0, 0x0
    beq @L_801167C4
    lbz r28, 0x6(r30)
    extsb r28, r28
@L_801167C4:
    extsh r0, r28
    cmpwi r0, 0x0
    bge @L_801167D8
    li r3, -0x1
    b @L_8011693C
@L_801167D8:
    mr r3, r29
    bl fn_800EC1BC
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_801167F4
    li r3, -0x1
    b @L_8011693C
@L_801167F4:
    rlwinm r0, r31, 0, 26, 26
    cmpwi r0, 0x0
    beq @L_80116830
    lwz r6, 0x14(r30)
    mr r3, r26
    lbz r7, 0x8(r30)
    mr r4, r27
    mr r5, r23
    bl fn_801845E4
    lfs f1, -26368(r2)
    mr r3, r26
    mr r4, r27
    fmr f2, f1
    fmr f3, f1
    bl fn_801860F8
@L_80116830:
    rlwinm r0, r31, 0, 27, 27
    cmpwi r0, 0x0
    beq @L_80116868
    mr r3, r29
    extsh r4, r28
    bl fn_800ECCA8
    mr r3, r29
    addi r4, r1, 0x8
    li r5, 0x0
    bl fn_800EC4D0
    lfs f1, 0x8(r1)
    lfs f0, -26356(r2)
    fsubs f0, f1, f0
    stfs f0, 0x8(r1)
@L_80116868:
    cmplwi r29, 0x0
    lfs f31, 0x8(r1)
    beq @L_801168AC
    mr r3, r29
    extsh r4, r28
    bl fn_800ECCA8
    fmr f1, f31
    mr r3, r29
    bl fn_800ECA78
    lfs f1, -26364(r2)
    mr r3, r29
    bl fn_800EC9DC
    mr r3, r29
    li r4, 0x0
    bl fn_800ECB74
    mr r3, r29
    bl fn_800EC990
@L_801168AC:
    rlwinm r0, r31, 0, 25, 25
    cmpwi r0, 0x0
    beq @L_801168D4
    mr r3, r24
    mr r4, r25
    mr r6, r26
    mr r7, r27
    li r5, 0xc0
    bl fn_80116470
    b @L_801168E8
@L_801168D4:
    rlwinm r0, r31, 0, 24, 24
    cmpwi r0, 0x0
    beq @L_801168E8
    li r3, 0x45
    bl fn_80166A28
@L_801168E8:
    mr r3, r29
    addi r4, r1, 0x8
    li r5, 0x0
    bl fn_800EC4D0
    bl fn_800D37CC
    xoris r3, r3, 0x8000
    lis r0, 0x4330
    stw r3, 0x1c(r1)
    lfd f3, -26344(r2)
    stw r0, 0x18(r1)
    lfs f2, -26348(r2)
    lfd f0, 0x18(r1)
    lfs f1, 0x8(r1)
    fsubs f3, f0, f3
    lfs f0, -26352(r2)
    fmuls f1, f2, f1
    fdivs f1, f1, f3
    fmuls f0, f0, f1
    fctiwz f0, f0
    stfd f0, 0x20(r1)
    lwz r3, 0x24(r1)
@L_8011693C:
    psq_l f31, 0x58(r1), 0, 0
    lfd f31, 0x50(r1)
    lmw r23, 0x2c(r1)
    lwz r0, 0x64(r1)
    mtlr r0
    addi r1, r1, 0x60
    blr
