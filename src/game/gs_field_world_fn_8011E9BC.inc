    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E9CC
    li r3, 0x0
    b @L_8011E9D0
@L_8011E9CC:
    addi r3, r3, 0xbd
@L_8011E9D0:
    cmplwi r3, 0x0
    bne @L_8011E9E0
    li r3, 0x0
    blr
@L_8011E9E0:
    lbz r3, 0x8(r3)
    blr
