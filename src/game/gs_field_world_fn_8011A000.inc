    nofralloc
    cmplwi r3, 0x0
    bne @L_8011A010
    li r3, 0x0
    blr
@L_8011A010:
    lhz r3, 0xc(r3)
    blr
