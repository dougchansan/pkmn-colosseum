    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xca(r3)
    blr
