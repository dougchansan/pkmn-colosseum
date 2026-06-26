    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r4
    stw r30, 0x8(r1)
    mr. r30, r3
    beq @L_8011DF3C
    cmplwi r31, 0x0
    bne @L_8011DF10
    b @L_8011DF3C
@L_8011DF10:
    addi r3, r30, 0x2e
    li r5, 0xb
    bl fn_800F9D24
    cmplwi r30, 0x0
    beq @L_8011DF3C
    cmplwi r31, 0x0
    beq @L_8011DF3C
    mr r4, r31
    addi r3, r30, 0x44
    li r5, 0xb
    bl fn_800F9D24
@L_8011DF3C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
