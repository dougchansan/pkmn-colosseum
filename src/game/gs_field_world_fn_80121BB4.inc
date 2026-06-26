    nofralloc
    stwu r1, -0x150(r1)
    mflr r0
    stw r0, 0x154(r1)
    stw r31, 0x14c(r1)
    mr r31, r4
    stw r30, 0x148(r1)
    mr. r30, r3
    beq @L_80121C00
    cmplwi r31, 0x0
    beq @L_80121C00
    mr r4, r30
    addi r3, r1, 0x8
    bl fn_8011F5FC
    mr r3, r30
    mr r4, r31
    bl fn_8011F5FC
    mr r3, r31
    addi r4, r1, 0x8
    bl fn_8011F5FC
@L_80121C00:
    lwz r0, 0x154(r1)
    lwz r31, 0x14c(r1)
    lwz r30, 0x148(r1)
    mtlr r0
    addi r1, r1, 0x150
    blr
