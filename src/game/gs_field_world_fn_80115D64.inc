    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    stw r0, 0x34(r1)
    stmw r25, 0x14(r1)
    mr r25, r3
    mr r26, r4
    li r28, 0x0
    li r29, 0x0
    li r31, 0x0
    b @L_80115DC0
    @L_80115D8C:
    lwz r0, lbl_80478EBC(r13)
    add r30, r0, r31
    bl fn_800FF56C
    lhz r0, 0x4(r30)
    cmplw r0, r3
    bne @L_80115DB8
    cmplw r29, r25
    bne @L_80115DB4
    oris r27, r28, 0x7fff
    b @L_80115DD0
    @L_80115DB4:
    addi r28, r28, 0x1
    @L_80115DB8:
    addi r31, r31, 0x1c
    addi r29, r29, 0x1
    @L_80115DC0:
    lwz r3, lbl_80478EB8(r13)
    lwz r0, 0x0(r3)
    cmplw r29, r0
    blt @L_80115D8C
    @L_80115DD0:
    lwz r3, lbl_80478EB8(r13)
    lwz r0, 0x0(r3)
    cmplw r29, r0
    beq @L_80115DF0
    bl fn_80113F48
    mr r4, r27
    mr r5, r26
    bl fn_8018C1E8
    @L_80115DF0:
    lmw r25, 0x14(r1)
    lwz r0, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr
