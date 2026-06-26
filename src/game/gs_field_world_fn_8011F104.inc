    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F114
    li r3, 0x0
    b @L_8011F118
@L_8011F114:
    addi r3, r3, 0x8c
@L_8011F118:
    cmplwi r3, 0x0
    bne @L_8011F128
    li r3, 0x0
    blr
@L_8011F128:
    lhz r3, 0x4(r3)
    blr
