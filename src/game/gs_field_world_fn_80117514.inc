    nofralloc
    stwu r1, -0xa0(r1)
    mflr r0
    stw r0, 0xa4(r1)
    stfd f31, 0x90(r1)
    psq_st f31, 0x98(r1), 0, 0
    stfd f30, 0x80(r1)
    psq_st f30, 0x88(r1), 0, 0
    stfd f29, 0x70(r1)
    psq_st f29, 0x78(r1), 0, 0
    stfd f28, 0x60(r1)
    psq_st f28, 0x68(r1), 0, 0
    stfd f27, 0x50(r1)
    psq_st f27, 0x58(r1), 0, 0
    stfd f26, 0x40(r1)
    psq_st f26, 0x48(r1), 0, 0
    stmw r25, 0x24(r1)
    mr r25, r3
    mr r26, r4
    mr r27, r5
    mr r28, r6
    lwz r0, lbl_8047AD68(r13)
    cmplwi r0, 0x1
    bne @L_8011759C
    lwz r4, lbl_8047AD6C(r13)
    li r3, 0x1
    lfs f0, 0x10(r4)
    stfs f0, 0x0(r26)
    lwz r4, lbl_8047AD6C(r13)
    lfs f0, 0xc(r4)
    stfs f0, 0x0(r27)
    lwz r4, lbl_8047AD6C(r13)
    lfs f0, 0x14(r4)
    stfs f0, 0x0(r28)
    b @L_80117684
    @L_8011759C:
    lfs f29, lbl_8047CFD0(r2)
    li r30, 0x0
    li r31, 0x0
    fmr f28, f29
    fmr f27, f29
    fmr f26, f29
    lfs f30, lbl_8047CFDC(r2)
    lfs f31, lbl_8047CFE0(r2)
    b @L_80117630
    @L_801175C0:
    lwz r0, lbl_8047AD6C(r13)
    addi r3, r1, 0x8
    add r29, r0, r31
    lfs f1, 0x0(r29)
    lfs f2, 0x4(r29)
    lfs f3, 0x8(r29)
    bl fn_800E01F4
    mr r4, r25
    addi r3, r1, 0x8
    bl fn_800E0020
    fcmpo cr0, f1, f30
    ble @L_80117614
    fdivs f3, f31, f1
    lfs f2, 0xc(r29)
    lfs f1, 0x10(r29)
    lfs f0, 0x14(r29)
    fadds f29, f29, f3
    fmadds f28, f2, f3, f28
    fmadds f27, f1, f3, f27
    fmadds f26, f0, f3, f26
    b @L_80117628
    @L_80117614:
    fmr f29, f30
    lfs f27, 0x10(r29)
    lfs f28, 0xc(r29)
    lfs f26, 0x14(r29)
    b @L_8011763C
    @L_80117628:
    addi r31, r31, 0x18
    addi r30, r30, 0x1
    @L_80117630:
    lwz r0, lbl_8047AD68(r13)
    cmplw r30, r0
    blt @L_801175C0
    @L_8011763C:
    lfs f0, lbl_8047CFD0(r2)
    fcmpu cr0, f0, f29
    bne @L_80117660
    lis r3, lbl_80272770@ha
    addi r3, r3, lbl_80272770@l
    crxor 6, 6, 6
    bl fn_800DD970
    li r3, 0x0
    b @L_80117684
    @L_80117660:
    lfs f0, lbl_8047CFDC(r2)
    li r3, 0x1
    fdivs f0, f0, f29
    fmuls f2, f27, f0
    fmuls f1, f28, f0
    fmuls f0, f26, f0
    stfs f2, 0x0(r26)
    stfs f1, 0x0(r27)
    stfs f0, 0x0(r28)
    @L_80117684:
    psq_l f31, 0x98(r1), 0, 0
    lfd f31, 0x90(r1)
    psq_l f30, 0x88(r1), 0, 0
    lfd f30, 0x80(r1)
    psq_l f29, 0x78(r1), 0, 0
    lfd f29, 0x70(r1)
    psq_l f28, 0x68(r1), 0, 0
    lfd f28, 0x60(r1)
    psq_l f27, 0x58(r1), 0, 0
    lfd f27, 0x50(r1)
    psq_l f26, 0x48(r1), 0, 0
    lfd f26, 0x40(r1)
    lmw r25, 0x24(r1)
    lwz r0, 0xa4(r1)
    mtlr r0
    addi r1, r1, 0xa0
    blr
