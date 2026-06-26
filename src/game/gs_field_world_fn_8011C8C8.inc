    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C8D8
    li r3, 0x0
    blr
@L_8011C8D8:
    lbz r3, 0x8(r3)
    blr
