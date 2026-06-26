    nofralloc
    cmplwi r3, 0x0
    bne @L_8012A92C
    li r3, 0x0
    blr
@L_8012A92C:
    lbz r3, 0xa95(r3)
    blr
