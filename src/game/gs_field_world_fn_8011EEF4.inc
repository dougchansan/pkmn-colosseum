    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EF04
    li r3, 0x0
    b @L_8011EF08
@L_8011EF04:
    addi r3, r3, 0xa4
@L_8011EF08:
    cmplwi r3, 0x0
    bne @L_8011EF18
    li r3, 0x0
    blr
@L_8011EF18:
    lhz r3, 0x4(r3)
    blr
