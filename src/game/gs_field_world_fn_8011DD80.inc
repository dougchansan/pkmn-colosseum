    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    mr r30, r5
    li r5, 0x0
    stw r29, 0x14(r1)
    mr r29, r4
    stw r28, 0x10(r1)
    mr r28, r3
    bl fn_8011F260
    mr. r31, r3
    beq @L_8011DDDC
    mr r3, r28
    mr r4, r29
    bl fn_80123E70
    rlwinm r0, r30, 0, 24, 31
    rlwinm r4, r3, 0, 24, 31
    cmplw r4, r0
    bge @L_8011DDD8
    mr r30, r3
@L_8011DDD8:
    stb r30, 0x2(r31)
@L_8011DDDC:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
