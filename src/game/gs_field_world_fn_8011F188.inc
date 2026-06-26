    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F198
    li r3, 0x0
    blr
@L_8011F198:
    lhz r3, 0x8a(r3)
    blr
