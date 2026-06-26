    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EFB4
    li r3, 0x0
    b @L_8011EFB8
@L_8011EFB4:
    addi r3, r3, 0x98
@L_8011EFB8:
    cmplwi r3, 0x0
    bne @L_8011EFC8
    li r3, 0x0
    blr
@L_8011EFC8:
    lhz r3, 0x8(r3)
    blr
