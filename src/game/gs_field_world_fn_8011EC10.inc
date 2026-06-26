    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EC20
    li r3, 0x0
    b @L_8011EC24
@L_8011EC20:
    addi r3, r3, 0xb7
@L_8011EC24:
    cmplwi r3, 0x0
    bne @L_8011EC34
    li r3, 0x0
    blr
@L_8011EC34:
    lbz r3, 0x0(r3)
    blr
