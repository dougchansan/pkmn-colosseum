    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    li r5, 0x0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r4
    li r4, 0xc
    stw r30, 0x8(r1)
    mr r30, r3
    bl fn_8012A5B0
    subf r5, r31, r3
    mr r3, r30
    li r4, 0xc
    bl fn_8012A450
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
