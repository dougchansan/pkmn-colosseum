    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    clrlwi r31, r4, 16
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    li r29, 0x0
    stw r28, 0x10(r1)
    mr r28, r3
    b @L_8011B9E0
@L_8011B97C:
    clrlslwi r0, r29, 16, 4
    add. r30, r28, r0
    beq @L_8011B9DC
    mr r3, r30
    li r4, 0x0
    bl fn_80119FF0
    mr r3, r30
    li r4, 0x0
    bl fn_80119FE0
    mr r3, r30
    li r4, 0x0
    bl fn_80119FD0
    mr r3, r30
    li r4, 0x0
    bl fn_80119FC0
    mr r3, r30
    li r4, 0x0
    bl fn_80119FB0
    mr r3, r30
    li r4, 0x0
    bl fn_80119FA0
    mr r3, r30
    li r4, 0x0
    bl fn_80119F90
@L_8011B9DC:
    addi r29, r29, 0x1
@L_8011B9E0:
    clrlwi r0, r29, 16
    cmplw r0, r31
    blt @L_8011B97C
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
