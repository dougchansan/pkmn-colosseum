    nofralloc
    cmplwi r3, 0x0
    bne @L_8011ECA4
    li r3, 0x0
    b @L_8011ECA8
@L_8011ECA4:
    addi r3, r3, 0xb2
@L_8011ECA8:
    cmplwi r3, 0x0
    bne @L_8011ECB8
    li r3, 0x0
    blr
@L_8011ECB8:
    lbz r3, 0x2(r3)
    blr
