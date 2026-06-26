    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    mr r5, r3
    stw r0, 0x14(r1)
    lfs f1, 0x38(r3)
    mr r3, r4
    lfs f2, 0x3c(r5)
    lfs f3, 0x40(r5)
    bl fn_800E01F4
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
