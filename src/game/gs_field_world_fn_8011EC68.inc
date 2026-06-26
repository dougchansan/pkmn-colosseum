    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EC78
    li r3, 0x0
    b @L_8011EC7C
@L_8011EC78:
    addi r3, r3, 0xb2
@L_8011EC7C:
    cmplwi r3, 0x0
    bne @L_8011EC8C
    li r3, 0x0
    blr
@L_8011EC8C:
    lbz r3, 0x3(r3)
    blr
