    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xf(r3)
    blr
