    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D11C
    li r4, 0x0
    b @L_8011D13C
@L_8011D11C:
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x14
    blt @L_8011D130
    li r4, 0x0
    b @L_8011D13C
@L_8011D130:
    clrlslwi r4, r4, 16, 2
    addi r4, r4, 0xba
    add r4, r3, r4
@L_8011D13C:
    cmplwi r4, 0x0
    beqlr
    stb r5, 0x0(r4)
    blr
