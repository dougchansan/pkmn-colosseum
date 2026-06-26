    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EED8
    li r3, 0x0
    b @L_8011EEDC
@L_8011EED8:
    addi r3, r3, 0xa4
@L_8011EEDC:
    cmplwi r3, 0x0
    bne @L_8011EEEC
    li r3, 0x0
    blr
@L_8011EEEC:
    lhz r3, 0x6(r3)
    blr
