    nofralloc
    cmplwi r3, 0x0
    bne @L_8012A864
    li r3, 0x0
    blr
@L_8012A864:
    lwz r3, 0xa84(r3)
    blr
