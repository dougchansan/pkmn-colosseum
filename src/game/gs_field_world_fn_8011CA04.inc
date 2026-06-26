    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CA14
    li r3, 0x0
    blr
@L_8011CA14:
    lbz r3, 0x1(r3)
    blr
