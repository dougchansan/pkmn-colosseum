    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F46C
    li r3, 0x0
    blr
@L_8011F46C:
    lwz r3, 0x74(r3)
    blr
