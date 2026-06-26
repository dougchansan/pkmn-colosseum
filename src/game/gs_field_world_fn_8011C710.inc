    nofralloc
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x18(r3)
    blr
