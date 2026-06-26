    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CDC8
    li r3, 0x0
    blr
@L_8011CDC8:
    lbz r3, 0x7(r3)
    blr
