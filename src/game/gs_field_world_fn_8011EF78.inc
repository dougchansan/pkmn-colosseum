    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EF88
    li r3, 0x0
    b @L_8011EF8C
@L_8011EF88:
    addi r3, r3, 0x98
@L_8011EF8C:
    cmplwi r3, 0x0
    bne @L_8011EF9C
    li r3, 0x0
    blr
@L_8011EF9C:
    lhz r3, 0xa(r3)
    blr
