    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E904
    li r3, 0x0
    blr
@L_8011E904:
    lbz r3, 0xca(r3)
    blr
