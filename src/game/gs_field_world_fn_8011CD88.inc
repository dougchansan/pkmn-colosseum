    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CD98
    li r3, 0x0
    blr
@L_8011CD98:
    lbz r3, 0x9(r3)
    blr
