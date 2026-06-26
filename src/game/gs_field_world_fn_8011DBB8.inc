    nofralloc
    cmplwi r3, 0x0
    bne @L_8011DBC8
    li r3, 0x0
    b @L_8011DBCC
@L_8011DBC8:
    addi r3, r3, 0x8c
@L_8011DBCC:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x8(r3)
    blr
