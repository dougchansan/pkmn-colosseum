    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr r31, r3
    stw r30, 0x18(r1)
    li r30, 0x0
    stw r29, 0x14(r1)
    mr r29, r4
@L_80118C44:
    lwz r3, 0x8(r31)
    cmplwi r3, 0x0
    beq @L_80118C5C
    lwz r3, 0x10(r3)
    mr r4, r29
    bl fn_80169034
@L_80118C5C:
    addi r30, r30, 0x1
    addi r31, r31, 0x4
    cmplwi r30, 0x40
    blt @L_80118C44
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
