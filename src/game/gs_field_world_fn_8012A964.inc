    nofralloc
    cmplwi r3, 0x0
    bne @L_8012A974
    li r3, 0x0
    blr
@L_8012A974:
    lbz r3, 0xa92(r3)
    blr
