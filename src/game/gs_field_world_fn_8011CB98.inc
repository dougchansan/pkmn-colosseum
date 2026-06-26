    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CBA8
    li r3, 0x0
    blr
@L_8011CBA8:
    lbz r3, 0x1(r3)
    blr
