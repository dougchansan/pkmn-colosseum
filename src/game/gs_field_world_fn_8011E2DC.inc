    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E2EC
    li r4, 0x0
    b @L_8011E30C
@L_8011E2EC:
    clrlwi r0, r4, 16
    cmplwi r0, 0x14
    blt @L_8011E300
    li r4, 0x0
    b @L_8011E30C
@L_8011E300:
    clrlslwi r4, r4, 16, 2
    addi r4, r4, 0xba
    add r4, r3, r4
@L_8011E30C:
    cmplwi r4, 0x0
    bne @L_8011E31C
    li r3, 0x0
    blr
@L_8011E31C:
    lhz r3, 0x2(r4)
    blr
