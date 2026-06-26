    nofralloc
    cmplwi r3, 0x0
    bne @L_8011A070
    li r3, 0x0
    blr
@L_8011A070:
    lbz r3, 0x4(r3)
    blr
