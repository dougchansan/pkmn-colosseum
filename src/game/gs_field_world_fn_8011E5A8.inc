    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E5B8
    li r3, 0x0
    b @L_8011E5BC
@L_8011E5B8:
    addi r3, r3, 0x90
@L_8011E5BC:
    cmplwi r3, 0x0
    bne @L_8011E5CC
    li r3, 0x0
    blr
@L_8011E5CC:
    lhz r3, 0x6(r3)
    blr
