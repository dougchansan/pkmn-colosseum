    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EB70
    li r3, 0x0
    b @L_8011EB74
@L_8011EB70:
    addi r3, r3, 0xb7
@L_8011EB74:
    cmplwi r3, 0x0
    bne @L_8011EB84
    li r3, 0x0
    blr
@L_8011EB84:
    lbz r3, 0x4(r3)
    blr
