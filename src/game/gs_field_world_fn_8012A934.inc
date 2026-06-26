    nofralloc
    cmplwi r3, 0x0
    bne @L_8012A944
    li r3, 0x0
    blr
@L_8012A944:
    lbz r3, 0xa94(r3)
    blr
