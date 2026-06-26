    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    rlwinm. r0, r4, 0, 16, 31
    stw r31, 0x1c(r1)
    mr r31, r4
    stw r30, 0x18(r1)
    mr r30, r3
    stw r29, 0x14(r1)
    stw r28, 0x10(r1)
    bne @L_8011B164
    li r3, -0x1
    b @L_8011B2A0
@L_8011B164:
    bne @L_8011B170
    li r0, 0x0
    b @L_8011B228
@L_8011B170:
    mr r3, r31
    bl fn_80119E90
    rlwinm r29, r3, 0, 24, 31
    mr r3, r31
    bl fn_80119F10
    mr r28, r3
    mr r3, r31
    bl fn_80119ED0
    mr r6, r3
    mr r3, r28
    mr r4, r30
    li r5, 0x0
    li r7, 0x0
    bl fn_80135E44
    cmplwi r3, 0x0
    bne @L_8011B1B8
    li r28, 0x0
    b @L_8011B1C0
@L_8011B1B8:
    clrlslwi r0, r29, 16, 4
    add r28, r3, r0
@L_8011B1C0:
    cmplwi r28, 0x0
    bne @L_8011B1D0
    li r0, 0x0
    b @L_8011B228
@L_8011B1D0:
    bne @L_8011B1DC
    li r0, 0x0
    b @L_8011B1F8
@L_8011B1DC:
    mr r3, r28
    bl fn_8011A090
    rlwinm. r0, r3, 0, 16, 31
    bne @L_8011B1F4
    li r0, 0x0
    b @L_8011B1F8
@L_8011B1F4:
    li r0, 0x1
@L_8011B1F8:
    rlwinm r0, r0, 0, 24, 31
    cmplwi r0, 0x1
    bne @L_8011B224
    mr r3, r28
    bl fn_8011A090
    rlwinm r3, r3, 0, 16, 31
    rlwinm r0, r31, 0, 16, 31
    cmplw r0, r3
    bne @L_8011B224
    li r0, 0x1
    b @L_8011B228
@L_8011B224:
    li r0, 0x0
@L_8011B228:
    rlwinm. r0, r0, 0, 24, 31
    bne @L_8011B238
    li r3, -0x1
    b @L_8011B2A0
@L_8011B238:
    mr r3, r31
    bl fn_80119E90
    rlwinm r28, r3, 0, 24, 31
    mr r3, r31
    bl fn_80119F10
    mr r29, r3
    mr r3, r31
    bl fn_80119ED0
    mr r6, r3
    mr r3, r29
    mr r4, r30
    li r5, 0x0
    li r7, 0x0
    bl fn_80135E44
    cmplwi r3, 0x0
    bne @L_8011B280
    li r3, 0x0
    b @L_8011B288
@L_8011B280:
    clrlslwi r0, r28, 16, 4
    add r3, r3, r0
@L_8011B288:
    cmplwi r3, 0x0
    bne @L_8011B298
    li r3, -0x1
    b @L_8011B2A0
@L_8011B298:
    bl fn_8011A030
    rlwinm r3, r3, 0, 24, 31
@L_8011B2A0:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
