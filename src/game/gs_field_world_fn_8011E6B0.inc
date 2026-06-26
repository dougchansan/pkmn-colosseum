    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E6C0
    li r3, 0x0
    b @L_8011E6C4
@L_8011E6C0:
    addi r3, r3, 0x84
@L_8011E6C4:
    cmplwi r3, 0x0
    bne @L_8011E6D4
    li r3, 0x0
    blr
@L_8011E6D4:
    lhz r3, 0x6(r3)
    blr
