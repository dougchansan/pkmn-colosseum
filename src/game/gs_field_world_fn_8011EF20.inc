    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EF30
    li r3, 0x0
    b @L_8011EF34
@L_8011EF30:
    addi r3, r3, 0xa4
@L_8011EF34:
    cmplwi r3, 0x0
    bne @L_8011EF44
    li r3, 0x0
    blr
@L_8011EF44:
    lhz r3, 0x2(r3)
    blr
