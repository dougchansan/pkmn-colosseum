    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xc(r3)
    blr
