    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xe(r3)
    blr
