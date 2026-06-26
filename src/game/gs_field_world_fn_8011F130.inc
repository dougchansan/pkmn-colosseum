    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F140
    li r3, 0x0
    b @L_8011F144
@L_8011F140:
    addi r3, r3, 0x8c
@L_8011F144:
    cmplwi r3, 0x0
    bne @L_8011F154
    li r3, 0x0
    blr
@L_8011F154:
    lhz r3, 0x2(r3)
    blr
