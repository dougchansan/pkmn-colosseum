    nofralloc
    cmplwi r3, 0x0
    beqlr
    sth r4, 0xd2(r3)
    blr
