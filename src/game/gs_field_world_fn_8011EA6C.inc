    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EA7C
    li r3, 0x0
    b @L_8011EA80
@L_8011EA7C:
    addi r3, r3, 0xbd
@L_8011EA80:
    cmplwi r3, 0x0
    bne @L_8011EA90
    li r3, 0x0
    blr
@L_8011EA90:
    lbz r3, 0x4(r3)
    blr
