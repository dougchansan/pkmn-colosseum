    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E694
    li r3, 0x0
    b @L_8011E698
@L_8011E694:
    addi r3, r3, 0x84
@L_8011E698:
    cmplwi r3, 0x0
    bne @L_8011E6A8
    li r3, 0x0
    blr
@L_8011E6A8:
    lhz r3, 0x8(r3)
    blr
