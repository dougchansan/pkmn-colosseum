    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CCC4
    li r3, 0x0
    blr
@L_8011CCC4:
    lbz r3, 0xd(r3)
    blr
