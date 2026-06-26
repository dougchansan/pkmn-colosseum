    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E848
    li r3, 0x0
    blr
@L_8011E848:
    lbz r3, 0xce(r3)
    blr
