    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D03C
    li r4, 0x0
    b @L_8011D05C
@L_8011D03C:
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x2
    blt @L_8011D050
    li r4, 0x0
    b @L_8011D05C
@L_8011D050:
    clrlslwi r4, r4, 16, 3
    addi r4, r4, 0x10c
    add r4, r3, r4
@L_8011D05C:
    cmplwi r4, 0x0
    beqlr
    sth r5, 0x2(r4)
    blr
