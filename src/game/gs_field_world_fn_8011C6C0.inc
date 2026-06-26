    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x7(r3)
    blr
