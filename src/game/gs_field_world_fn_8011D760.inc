    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xbc(r3)
    blr
