    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F00C
    li r3, 0x0
    b @L_8011F010
@L_8011F00C:
    addi r3, r3, 0x98
@L_8011F010:
    cmplwi r3, 0x0
    bne @L_8011F020
    li r3, 0x0
    blr
@L_8011F020:
    lhz r3, 0x4(r3)
    blr
