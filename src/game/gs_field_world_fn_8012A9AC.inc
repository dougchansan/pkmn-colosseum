    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xa97(r3)
    blr
