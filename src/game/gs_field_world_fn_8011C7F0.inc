    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C800
    li r3, 0x0
    blr
@L_8011C800:
    lbz r3, 0x11(r3)
    blr
