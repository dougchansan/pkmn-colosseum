    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E830
    li r3, 0x0
    blr
@L_8011E830:
    lbz r3, 0xcf(r3)
    blr
