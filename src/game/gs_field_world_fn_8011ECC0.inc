    nofralloc
    cmplwi r3, 0x0
    bne @L_8011ECD0
    li r3, 0x0
    b @L_8011ECD4
@L_8011ECD0:
    addi r3, r3, 0xb2
@L_8011ECD4:
    cmplwi r3, 0x0
    bne @L_8011ECE4
    li r3, 0x0
    blr
@L_8011ECE4:
    lbz r3, 0x1(r3)
    blr
