    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    li r0, 0x0
    stw r31, 0x1c(r1)
    mr r31, r6
    li r6, 0x0
    stw r30, 0x18(r1)
    mr r30, r5
    li r5, 0x2
    stw r29, 0x14(r1)
    mr r29, r4
    stw r28, 0x10(r1)
    mr r28, r3
    li r3, 0x0
    sth r0, 0xc(r1)
    sth r0, 0xa(r1)
    bl itemGetStatus
    clrlwi r4, r3, 24
    mr r3, r28
    addi r5, r1, 0xc
    addi r6, r1, 0xa
    addi r7, r1, 0x9
    addi r8, r1, 0x8
    bl fn_80129BC8
    cmplwi r3, 0x0
    bne @L_80129AEC
    li r3, -0x1
    b @L_80129B0C
@L_80129AEC:
    lhz r4, 0xc(r1)
    mr r5, r29
    lhz r8, 0xa(r1)
    mr r6, r30
    lbz r9, 0x9(r1)
    mr r7, r31
    lbz r10, 0x8(r1)
    bl fn_80141308
@L_80129B0C:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
