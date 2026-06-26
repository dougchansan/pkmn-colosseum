    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CCF4
    li r3, 0x0
    blr
@L_8011CCF4:
    lbz r3, 0xb(r3)
    blr
