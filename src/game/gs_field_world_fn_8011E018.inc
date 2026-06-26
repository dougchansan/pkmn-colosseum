    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E028
    li r3, 0x0
    blr
@L_8011E028:
    lbz r3, 0x3(r3)
    blr
