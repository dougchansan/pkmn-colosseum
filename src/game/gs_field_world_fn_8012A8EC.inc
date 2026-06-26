    nofralloc
    cmplwi r3, 0x0
    bne @L_8012A8FC
    li r3, 0x0
    blr
@L_8012A8FC:
    lbz r3, 0xa97(r3)
    blr
