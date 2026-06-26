    nofralloc
    cmplwi r3, 0x0
    beqlr
    lbz r0, 0x0(r3)
    rlwimi r0, r4, 7, 24, 24
    stb r0, 0x0(r3)
    blr
