    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr r31, r5
    stw r30, 0x18(r1)
    mr r30, r4
    stw r29, 0x14(r1)
    mr r29, r3
    mr r3, r30
    bl fn_80119ED0
    rlwinm r0, r3, 0, 16, 31
    cmplwi r0, 0x7c
    beq @L_80121630
    mr r3, r30
    bl fn_80119ED0
    rlwinm r0, r3, 0, 16, 31
    cmplwi r0, 0xc8
    bne @L_80121640
@L_80121630:
    mr r3, r29
    mr r4, r30
    mr r5, r31
    bl fn_8011A570
@L_80121640:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
