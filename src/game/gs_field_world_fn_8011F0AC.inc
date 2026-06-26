    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F0BC
    li r3, 0x0
    b @L_8011F0C0
@L_8011F0BC:
    addi r3, r3, 0x8c
@L_8011F0C0:
    cmplwi r3, 0x0
    bne @L_8011F0D0
    li r3, 0x0
    blr
@L_8011F0D0:
    lhz r3, 0x8(r3)
    blr
