    nofralloc
    cmplwi r3, 0x0
    bne @L_8011ED78
    li r3, 0x0
    blr
@L_8011ED78:
    bne @L_8011ED84
    li r0, 0x0
    b @L_8011ED88
@L_8011ED84:
    lhz r0, 0xd8(r3)
@L_8011ED88:
    clrlwi. r0, r0, 16
    bne @L_8011ED98
    li r3, 0x0
    blr
@L_8011ED98:
    cmplwi r3, 0x0
    bne @L_8011EDA8
    li r0, 0x0
    b @L_8011EDAC
@L_8011EDA8:
    lwz r0, 0xdc(r3)
@L_8011EDAC:
    cmpwi r0, 0x0
    bge @L_8011EDBC
    li r3, 0x0
    blr
@L_8011EDBC:
    li r3, 0x1
    blr
