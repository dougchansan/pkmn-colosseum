    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C2E0
    li r3, 0x0
    blr
@L_8011C2E0:
    bne @L_8011C2EC
    li r3, 0x0
    b @L_8011C2F0
@L_8011C2EC:
    lhz r3, 0x1c(r3)
@L_8011C2F0:
    lwz r0, lbl_80478B50(r13)
    clrlwi r3, r3, 16
    cmplw r3, r0
    blt @L_8011C308
    li r3, 0x0
    b @L_8011C318
@L_8011C308:
    mulli r4, r3, 0x14
    lis r3, lbl_8035C430@ha
    addi r0, r3, lbl_8035C430@l
    add r3, r0, r4
@L_8011C318:
    cmplwi r3, 0x0
    bne @L_8011C328
    li r3, 0x0
    blr
@L_8011C328:
    lwz r3, 0xc(r3)
    blr
