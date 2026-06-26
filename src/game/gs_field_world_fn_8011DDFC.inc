    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r5
    li r5, 0x0
    bl fn_8011F260
    cmplwi r3, 0x0
    beq @L_8011DE24
    sth r31, 0x0(r3)
@L_8011DE24:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
