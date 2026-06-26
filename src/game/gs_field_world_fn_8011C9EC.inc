    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C9FC
    li r3, 0x0
    blr
@L_8011C9FC:
    lbz r3, 0x2(r3)
    blr
