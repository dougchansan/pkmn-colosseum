    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C7B8
    li r3, 0x0
    blr
@L_8011C7B8:
    lbz r3, 0x14(r3)
    blr
