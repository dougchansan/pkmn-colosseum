    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E9A0
    li r3, 0x0
    b @L_8011E9A4
@L_8011E9A0:
    addi r3, r3, 0xbd
@L_8011E9A4:
    cmplwi r3, 0x0
    bne @L_8011E9B4
    li r3, 0x0
    blr
@L_8011E9B4:
    lbz r3, 0x9(r3)
    blr
