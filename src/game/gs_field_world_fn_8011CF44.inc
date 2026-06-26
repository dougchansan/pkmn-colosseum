    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    cmplwi r3, 0x0
    stw r0, 0x14(r1)
    beq @L_8011CF60
    lwz r3, 0x28(r3)
    bl fn_8019075C
@L_8011CF60:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
