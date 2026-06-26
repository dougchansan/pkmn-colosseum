    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E500
    li r3, 0x0
    blr
@L_8011E500:
    lbz r3, 0x2(r3)
    blr
