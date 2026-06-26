    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F16C
    li r3, 0x0
    b @L_8011F170
@L_8011F16C:
    addi r3, r3, 0x8c
@L_8011F170:
    cmplwi r3, 0x0
    bne @L_8011F180
    li r3, 0x0
    blr
@L_8011F180:
    lhz r3, 0x0(r3)
    blr
