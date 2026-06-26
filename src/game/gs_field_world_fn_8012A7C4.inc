    nofralloc
    cmplwi r3, 0x0
    bne @L_8012A7D4
    li r3, 0x0
    blr
@L_8012A7D4:
    lwz r3, 0xa8c(r3)
    blr
