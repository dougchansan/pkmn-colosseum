    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    addic. r0, r1, 0x8
    stw r31, 0x1c(r1)
    mr r31, r5
    stw r30, 0x18(r1)
    mr r30, r4
    beq @L_80129540
    li r0, 0xa
    sth r0, 0x8(r1)
@L_80129540:
    li r4, 0xa
    li r5, 0x0
    bl fn_8012A5B0
    mr. r4, r3
    beq @L_80129584
    lhz r3, 0x8(r1)
    clrlwi r0, r30, 16
    cmplw r0, r3
    bge @L_80129584
    clrlwi r0, r31, 16
    cmplw r0, r3
    bge @L_80129584
    clrlslwi r3, r30, 16, 2
    clrlslwi r0, r31, 16, 2
    add r3, r4, r3
    add r4, r4, r0
    bl fn_80140A9C
@L_80129584:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
