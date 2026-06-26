    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E744
    li r3, 0x0
    b @L_8011E748
@L_8011E744:
    addi r3, r3, 0x84
@L_8011E748:
    cmplwi r3, 0x0
    bne @L_8011E758
    li r3, 0x0
    blr
@L_8011E758:
    lhz r3, 0x0(r3)
    blr
