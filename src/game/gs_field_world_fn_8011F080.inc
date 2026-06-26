    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F090
    li r3, 0x0
    b @L_8011F094
@L_8011F090:
    addi r3, r3, 0x8c
@L_8011F094:
    cmplwi r3, 0x0
    bne @L_8011F0A4
    li r3, 0x0
    blr
@L_8011F0A4:
    lhz r3, 0xa(r3)
    blr
