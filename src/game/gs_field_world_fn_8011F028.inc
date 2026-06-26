    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F038
    li r3, 0x0
    b @L_8011F03C
@L_8011F038:
    addi r3, r3, 0x98
@L_8011F03C:
    cmplwi r3, 0x0
    bne @L_8011F04C
    li r3, 0x0
    blr
@L_8011F04C:
    lhz r3, 0x2(r3)
    blr
