    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xce(r3)
    blr
