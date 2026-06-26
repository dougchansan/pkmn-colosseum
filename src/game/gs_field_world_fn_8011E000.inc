    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E010
    li r3, 0x0
    blr
@L_8011E010:
    lbz r3, 0x4(r3)
    blr
