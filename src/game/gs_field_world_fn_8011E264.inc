    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E274
    li r4, 0x0
    b @L_8011E294
@L_8011E274:
    clrlwi r0, r4, 16
    cmplwi r0, 0x2
    blt @L_8011E288
    li r4, 0x0
    b @L_8011E294
@L_8011E288:
    clrlslwi r4, r4, 16, 3
    addi r4, r4, 0x10c
    add r4, r3, r4
@L_8011E294:
    cmplwi r4, 0x0
    bne @L_8011E2A4
    li r3, 0x0
    blr
@L_8011E2A4:
    lwz r3, 0x4(r4)
    blr
