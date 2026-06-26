    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xcd(r3)
    blr
