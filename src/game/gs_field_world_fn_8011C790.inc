    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C7A0
    li r3, 0x0
    blr
@L_8011C7A0:
    lbz r3, 0x15(r3)
    blr
