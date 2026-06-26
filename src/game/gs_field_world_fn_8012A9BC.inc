    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xa96(r3)
    blr
