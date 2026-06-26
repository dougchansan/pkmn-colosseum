    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EE50
    li r3, 0x0
    blr
@L_8011EE50:
    lhz r3, 0xd8(r3)
    blr
