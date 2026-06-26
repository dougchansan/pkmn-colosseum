    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r3
    bl fn_800FF554
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    beq @L_801151E8
    li r3, 0x0
    b @L_801151F4
@L_801151E8:
    mr r3, r31
    bl fn_800FC1D0
    li r3, 0x1
@L_801151F4:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
