    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E63C
    li r3, 0x0
    b @L_8011E640
@L_8011E63C:
    addi r3, r3, 0x90
@L_8011E640:
    cmplwi r3, 0x0
    bne @L_8011E650
    li r3, 0x0
    blr
@L_8011E650:
    lhz r3, 0x0(r3)
    blr
