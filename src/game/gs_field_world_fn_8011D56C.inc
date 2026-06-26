    nofralloc
    cmplwi r3, 0x0
    beqlr
    stb r4, 0xcb(r3)
    blr
