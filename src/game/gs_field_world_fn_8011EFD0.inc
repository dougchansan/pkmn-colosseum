    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EFE0
    li r3, 0x0
    b @L_8011EFE4
@L_8011EFE0:
    addi r3, r3, 0x98
@L_8011EFE4:
    cmplwi r3, 0x0
    bne @L_8011EFF4
    li r3, 0x0
    blr
@L_8011EFF4:
    lhz r3, 0x6(r3)
    blr
