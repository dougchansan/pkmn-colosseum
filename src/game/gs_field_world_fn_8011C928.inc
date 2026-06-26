    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C938
    li r3, 0x0
    blr
@L_8011C938:
    lwz r3, 0x28(r3)
    blr
