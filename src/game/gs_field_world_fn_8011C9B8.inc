    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C9C8
    li r3, 0x0
    blr
@L_8011C9C8:
    lbz r3, 0x3(r3)
    blr
