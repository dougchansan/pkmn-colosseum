    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C8A8
    li r3, 0x0
    blr
@L_8011C8A8:
    lbz r3, 0xa(r3)
    blr
