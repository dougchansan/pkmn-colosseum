    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    li r5, 0x2
    li r6, 0x0
    stw r0, 0x24(r1)
    li r0, 0x0
    stw r31, 0x1c(r1)
    mr r31, r4
    stw r30, 0x18(r1)
    mr r30, r3
    li r3, 0x0
    sth r0, 0xc(r1)
    sth r0, 0xa(r1)
    bl itemGetStatus
    clrlwi r4, r3, 24
    mr r3, r30
    addi r5, r1, 0xc
    addi r6, r1, 0xa
    addi r8, r1, 0x8
    li r7, 0x0
    bl fn_80129BC8
    cmplwi r3, 0x0
    bne @L_8012991C
    li r3, -0x1
    b @L_80129930
@L_8012991C:
    lhz r4, 0xc(r1)
    mr r5, r31
    lhz r6, 0xa(r1)
    lbz r7, 0x8(r1)
    bl fn_80140588
@L_80129930:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
