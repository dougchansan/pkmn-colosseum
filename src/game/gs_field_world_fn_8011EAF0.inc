    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EB00
    li r3, 0x0
    b @L_8011EB04
@L_8011EB00:
    addi r3, r3, 0xbd
@L_8011EB04:
    cmplwi r3, 0x0
    bne @L_8011EB14
    li r3, 0x0
    blr
@L_8011EB14:
    lbz r3, 0x1(r3)
    blr
