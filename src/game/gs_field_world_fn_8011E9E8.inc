    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E9F8
    li r3, 0x0
    b @L_8011E9FC
@L_8011E9F8:
    addi r3, r3, 0xbd
@L_8011E9FC:
    cmplwi r3, 0x0
    bne @L_8011EA0C
    li r3, 0x0
    blr
@L_8011EA0C:
    lbz r3, 0x7(r3)
    blr
