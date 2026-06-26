    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EBF4
    li r3, 0x0
    b @L_8011EBF8
@L_8011EBF4:
    addi r3, r3, 0xb7
@L_8011EBF8:
    cmplwi r3, 0x0
    bne @L_8011EC08
    li r3, 0x0
    blr
@L_8011EC08:
    lbz r3, 0x1(r3)
    blr
