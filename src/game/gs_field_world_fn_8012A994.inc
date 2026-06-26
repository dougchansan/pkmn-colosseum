    nofralloc
    cmplwi r3, 0x0
    bne @L_8012A9A4
    li r3, 0x0
    blr
@L_8012A9A4:
    lbz r3, 0xa90(r3)
    blr
