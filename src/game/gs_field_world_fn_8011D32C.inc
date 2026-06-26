    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D33C
    li r3, 0x0
    b @L_8011D340
@L_8011D33C:
    addi r3, r3, 0x90
@L_8011D340:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x4(r3)
    blr
