    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xa91(r3)
    blr
