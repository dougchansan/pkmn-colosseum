    nofralloc
    cmplwi r3, 0x0
    bne @L_8011DBA4
    li r3, 0x0
    b @L_8011DBA8
@L_8011DBA4:
    addi r3, r3, 0x8c
@L_8011DBA8:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0xa(r3)
    blr
