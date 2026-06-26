    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F530
    li r3, 0x0
    blr
@L_8011F530:
    lwz r3, 0x14(r3)
    blr
