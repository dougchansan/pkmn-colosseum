    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EA50
    li r3, 0x0
    b @L_8011EA54
@L_8011EA50:
    addi r3, r3, 0xbd
@L_8011EA54:
    cmplwi r3, 0x0
    bne @L_8011EA64
    li r3, 0x0
    blr
@L_8011EA64:
    lbz r3, 0x5(r3)
    blr
