    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xa81(r3)
    blr
