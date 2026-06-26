    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x12(r3)
    blr
