    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E1E4
    li r4, 0x0
    b @L_8011E204
@L_8011E1E4:
    clrlwi r0, r4, 16
    cmplwi r0, 0x2
    blt @L_8011E1F8
    li r4, 0x0
    b @L_8011E204
@L_8011E1F8:
    clrlslwi r4, r4, 16, 3
    addi r4, r4, 0x10c
    add r4, r3, r4
@L_8011E204:
    cmplwi r4, 0x0
    bne @L_8011E214
    li r3, 0x0
    blr
@L_8011E214:
    lbz r3, 0x0(r4)
    blr
