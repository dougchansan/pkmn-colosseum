    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E22C
    li r4, 0x0
    b @L_8011E24C
@L_8011E22C:
    clrlwi r0, r4, 16
    cmplwi r0, 0x2
    blt @L_8011E240
    li r4, 0x0
    b @L_8011E24C
@L_8011E240:
    clrlslwi r4, r4, 16, 3
    addi r4, r4, 0x10c
    add r4, r3, r4
@L_8011E24C:
    cmplwi r4, 0x0
    bne @L_8011E25C
    li r3, 0x0
    blr
@L_8011E25C:
    lhz r3, 0x2(r4)
    blr
