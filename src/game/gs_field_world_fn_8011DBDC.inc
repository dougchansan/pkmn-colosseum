    nofralloc
    cmplwi r3, 0x0
    bne @L_8011DBEC
    li r3, 0x0
    b @L_8011DBF0
@L_8011DBEC:
    addi r3, r3, 0x8c
@L_8011DBF0:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x6(r3)
    blr
