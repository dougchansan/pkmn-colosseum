    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D15C
    li r4, 0x0
    b @L_8011D17C
@L_8011D15C:
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x5
    blt @L_8011D170
    li r4, 0x0
    b @L_8011D17C
@L_8011D170:
    mulli r4, r0, 0x6
    addi r4, r4, 0x9c
    add r4, r3, r4
@L_8011D17C:
    cmplwi r4, 0x0
    beqlr
    sth r5, 0x4(r4)
    blr
