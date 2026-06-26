    nofralloc
    cmplwi r3, 0x0
    beqlr
    bne @L_8011C234
    li r3, 0x0
    b @L_8011C238
@L_8011C234:
    lhz r3, 0x1c(r3)
@L_8011C238:
    lwz r0, lbl_80478B50(r13)
    clrlwi r3, r3, 16
    cmplw r3, r0
    blt @L_8011C250
    li r3, 0x0
    b @L_8011C260
@L_8011C250:
    mulli r5, r3, 0x14
    lis r3, lbl_8035C430@ha
    addi r0, r3, lbl_8035C430@l
    add r3, r0, r5
@L_8011C260:
    cmplwi r3, 0x0
    beqlr
    stw r4, 0xc(r3)
    blr
