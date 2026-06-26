    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E3C4
    li r4, 0x0
    b @L_8011E3E4
@L_8011E3C4:
    clrlwi r0, r4, 16
    cmplwi r0, 0x5
    blt @L_8011E3D8
    li r4, 0x0
    b @L_8011E3E4
@L_8011E3D8:
    mulli r4, r0, 0x6
    addi r4, r4, 0x9c
    add r4, r3, r4
@L_8011E3E4:
    cmplwi r4, 0x0
    bne @L_8011E3F4
    li r3, 0x0
    blr
@L_8011E3F4:
    lhz r3, 0x2(r4)
    blr
