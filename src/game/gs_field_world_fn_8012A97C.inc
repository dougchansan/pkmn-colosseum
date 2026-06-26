    nofralloc
    cmplwi r3, 0x0
    bne @L_8012A98C
    li r3, 0x0
    blr
@L_8012A98C:
    lbz r3, 0xa91(r3)
    blr
