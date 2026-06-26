    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr. r31, r3
    stw r30, 0x18(r1)
    mr r30, r5
    beq @L_80123CBC
    li r5, 0x0
    bl fn_8011F260
    mr r0, r3
    mr r3, r31
    mr r31, r0
    mr r4, r30
    li r5, 0x0
    bl fn_8011F260
    mr r30, r3
    mr r4, r31
    addi r3, r1, 0x8
    bl fn_8011F5E0
    mr r3, r31
    mr r4, r30
    bl fn_8011F5E0
    mr r3, r30
    addi r4, r1, 0x8
    bl fn_8011F5E0
@L_80123CBC:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
