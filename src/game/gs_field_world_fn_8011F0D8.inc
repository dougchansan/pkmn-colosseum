    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F0E8
    li r3, 0x0
    b @L_8011F0EC
@L_8011F0E8:
    addi r3, r3, 0x8c
@L_8011F0EC:
    cmplwi r3, 0x0
    bne @L_8011F0FC
    li r3, 0x0
    blr
@L_8011F0FC:
    lhz r3, 0x6(r3)
    blr
