    nofralloc
    cmplwi r3, 0x0
    bne @L_8012A794
    li r3, 0x0
    blr
@L_8012A794:
    lbz r3, 0xa81(r3)
    blr
