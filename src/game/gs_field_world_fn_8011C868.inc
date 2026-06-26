    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C878
    li r3, 0x0
    blr
@L_8011C878:
    lbz r3, 0xc(r3)
    blr
