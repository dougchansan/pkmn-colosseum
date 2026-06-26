    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C908
    li r3, 0x0
    blr
@L_8011C908:
    lbz r3, 0x6(r3)
    blr
