    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EB9C
    li r3, 0x0
    b @L_8011EBA0
@L_8011EB9C:
    addi r3, r3, 0xb7
@L_8011EBA0:
    cmplwi r3, 0x0
    bne @L_8011EBB0
    li r3, 0x0
    blr
@L_8011EBB0:
    lbz r3, 0x3(r3)
    blr
