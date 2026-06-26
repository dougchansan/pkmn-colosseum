    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EA24
    li r3, 0x0
    b @L_8011EA28
@L_8011EA24:
    addi r3, r3, 0xbd
@L_8011EA28:
    cmplwi r3, 0x0
    bne @L_8011EA38
    li r3, 0x0
    blr
@L_8011EA38:
    lbz r3, 0x6(r3)
    blr
