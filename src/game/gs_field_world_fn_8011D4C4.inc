    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xd0(r3)
    blr
