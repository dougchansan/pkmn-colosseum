    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F560
    li r3, 0x0
    blr
@L_8011F560:
    lbz r3, 0xf(r3)
    blr
