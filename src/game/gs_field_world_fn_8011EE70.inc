    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EE80
    li r3, 0x0
    b @L_8011EE84
@L_8011EE80:
    addi r3, r3, 0xa4
@L_8011EE84:
    cmplwi r3, 0x0
    bne @L_8011EE94
    li r3, 0x0
    blr
@L_8011EE94:
    lhz r3, 0xa(r3)
    blr
