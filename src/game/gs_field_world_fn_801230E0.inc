    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    li r4, 0x0
    li r5, 0x82
    stw r0, 0x14(r1)
    li r6, 0x0
    bl fn_8012640C
    lwz r0, 0x14(r1)
    rlwinm r3, r3, 0, 16, 31
    mtlr r0
    addi r1, r1, 0x10
    blr
