    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E518
    li r3, 0x0
    blr
@L_8011E518:
    lbz r3, 0x1(r3)
    blr
