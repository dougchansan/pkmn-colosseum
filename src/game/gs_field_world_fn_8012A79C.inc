    nofralloc
    cmplwi r3, 0x0
    bne @L_8012A7AC
    li r3, 0x0
    blr
@L_8012A7AC:
    lbz r3, 0xa98(r3)
    blr
