    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    lbz r0, 0x1(r3)
    cmplwi r0, 0x1
    bne @L_80118DC8
    li r3, -0x1
    b @L_80118DD0
@L_80118DC8:
    lwz r3, 0x10(r3)
    bl fn_801694E0
@L_80118DD0:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
