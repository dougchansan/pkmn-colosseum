    nofralloc
    cmplwi r3, 0x0
    bne @L_8012A95C
    li r3, 0x0
    blr
@L_8012A95C:
    lbz r3, 0xa93(r3)
    blr
