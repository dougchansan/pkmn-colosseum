    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EF5C
    li r3, 0x0
    b @L_8011EF60
@L_8011EF5C:
    addi r3, r3, 0xa4
@L_8011EF60:
    cmplwi r3, 0x0
    bne @L_8011EF70
    li r3, 0x0
    blr
@L_8011EF70:
    lhz r3, 0x0(r3)
    blr
