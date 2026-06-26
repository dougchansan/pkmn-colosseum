    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D19C
    li r4, 0x0
    b @L_8011D1BC
@L_8011D19C:
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x5
    blt @L_8011D1B0
    li r4, 0x0
    b @L_8011D1BC
@L_8011D1B0:
    mulli r4, r0, 0x6
    addi r4, r4, 0x9c
    add r4, r3, r4
@L_8011D1BC:
    cmplwi r4, 0x0
    beqlr
    sth r5, 0x2(r4)
    blr
