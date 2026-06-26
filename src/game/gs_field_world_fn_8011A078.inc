    nofralloc
    cmplwi r3, 0x0
    bne @L_8011A088
    li r3, 0x0
    blr
@L_8011A088:
    lhz r3, 0x2(r3)
    blr
