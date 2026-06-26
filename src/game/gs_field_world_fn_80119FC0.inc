    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x5(r3)
    blr
