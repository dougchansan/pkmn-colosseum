    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F064
    li r3, 0x0
    b @L_8011F068
@L_8011F064:
    addi r3, r3, 0x98
@L_8011F068:
    cmplwi r3, 0x0
    bne @L_8011F078
    li r3, 0x0
    blr
@L_8011F078:
    lhz r3, 0x0(r3)
    blr
