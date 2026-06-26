    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stmw r27, 0xc(r1)
    rlwinm r4, r3, 0, 1, 15
    li r30, 0x0
    subis r0, r4, 0x7fff
    li r28, 0x0
    cmplwi r0, 0x0
    beq @L_80115CE4
    li r3, 0x0
    b @L_80115D50
    @L_80115CE4:
    clrlwi r27, r3, 23
    li r29, 0x0
    li r31, 0x0
    b @L_80115D24
    @L_80115CF4:
    lwz r0, lbl_80478EBC(r13)
    add r30, r0, r31
    bl fn_800FF56C
    lhz r0, 0x4(r30)
    cmplw r0, r3
    bne @L_80115D1C
    mr r0, r28
    addi r28, r28, 0x1
    cmplw r27, r0
    beq @L_80115D34
    @L_80115D1C:
    addi r31, r31, 0x1c
    addi r29, r29, 0x1
    @L_80115D24:
    lwz r3, lbl_80478EB8(r13)
    lwz r0, 0x0(r3)
    cmplw r29, r0
    blt @L_80115CF4
    @L_80115D34:
    lwz r3, lbl_80478EB8(r13)
    lwz r0, 0x0(r3)
    cmplw r29, r0
    bne @L_80115D4C
    li r3, 0x0
    b @L_80115D50
    @L_80115D4C:
    mr r3, r30
    @L_80115D50:
    lmw r27, 0xc(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
