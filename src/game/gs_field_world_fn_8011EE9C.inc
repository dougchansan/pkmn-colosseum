    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EEAC
    li r3, 0x0
    b @L_8011EEB0
@L_8011EEAC:
    addi r3, r3, 0xa4
@L_8011EEB0:
    cmplwi r3, 0x0
    bne @L_8011EEC0
    li r3, 0x0
    blr
@L_8011EEC0:
    lhz r3, 0x8(r3)
    blr
