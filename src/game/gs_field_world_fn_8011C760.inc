    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x2(r3)
    blr
