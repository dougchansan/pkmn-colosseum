    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F5C0
    li r3, 0x0
    blr
@L_8011F5C0:
    lwz r3, 0x4(r3)
    blr
