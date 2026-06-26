    nofralloc
    cmplwi r3, 0x0
    beqlr
    bne @L_8011C394
    li r3, 0x0
    b @L_8011C398
@L_8011C394:
    lhz r3, 0x1c(r3)
@L_8011C398:
    lwz r0, lbl_80478B50(r13)
    clrlwi r3, r3, 16
    cmplw r3, r0
    blt @L_8011C3B0
    li r3, 0x0
    b @L_8011C3C0
@L_8011C3B0:
    mulli r5, r3, 0x14
    lis r3, lbl_8035C430@ha
    addi r0, r3, lbl_8035C430@l
    add r3, r0, r5
@L_8011C3C0:
    cmplwi r3, 0x0
    beqlr
    stw r4, 0x4(r3)
    blr
