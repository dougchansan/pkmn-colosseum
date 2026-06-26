    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xa93(r3)
    blr
