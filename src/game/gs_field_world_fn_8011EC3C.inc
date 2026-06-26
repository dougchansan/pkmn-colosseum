    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EC4C
    li r3, 0x0
    b @L_8011EC50
@L_8011EC4C:
    addi r3, r3, 0xb2
@L_8011EC50:
    cmplwi r3, 0x0
    bne @L_8011EC60
    li r3, 0x0
    blr
@L_8011EC60:
    lbz r3, 0x4(r3)
    blr
