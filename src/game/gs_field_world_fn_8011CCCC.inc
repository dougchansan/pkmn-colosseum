    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CCDC
    li r3, 0x0
    blr
@L_8011CCDC:
    lbz r3, 0xc(r3)
    blr
