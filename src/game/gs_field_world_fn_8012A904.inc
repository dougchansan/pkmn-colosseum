    nofralloc
    cmplwi r3, 0x0
    bne @L_8012A914
    li r3, 0x0
    blr
@L_8012A914:
    lbz r3, 0xa96(r3)
    blr
