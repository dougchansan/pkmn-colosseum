    nofralloc
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x6(r3)
    blr
