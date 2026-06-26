    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    rlwinm. r0, r4, 0, 16, 31
    stw r31, 0x1c(r1)
    mr r31, r4
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    stw r28, 0x10(r1)
    mr r28, r3
    bne @L_8011B478
    li r3, 0x1
    b @L_8011B65C
@L_8011B478:
    mr r3, r31
    bl fn_80119E90
    rlwinm r29, r3, 0, 24, 31
    mr r3, r31
    bl fn_80119F10
    mr r30, r3
    mr r3, r31
    bl fn_80119ED0
    mr r6, r3
    mr r3, r30
    mr r4, r28
    li r5, 0x0
    li r7, 0x0
    bl fn_80135E44
    cmplwi r3, 0x0
    bne @L_8011B4C0
    li r30, 0x0
    b @L_8011B4C8
@L_8011B4C0:
    clrlslwi r0, r29, 16, 4
    add r30, r3, r0
@L_8011B4C8:
    cmplwi r30, 0x0
    bne @L_8011B4D8
    li r3, 0x0
    b @L_8011B65C
@L_8011B4D8:
    mr r3, r30
    bl fn_8011A090
    mr r0, r3
    mr r3, r31
    mr r29, r0
    bl fn_80119E50
    rlwinm r0, r3, 0, 24, 31
    cmpwi r0, 0x2
    beq @L_8011B530
    bge @L_8011B510
    cmpwi r0, 0x0
    beq @L_8011B520
    bge @L_8011B528
    b @L_8011B658
@L_8011B510:
    cmpwi r0, 0x4
    beq @L_8011B5E4
    bge @L_8011B658
    b @L_8011B570
@L_8011B520:
    li r3, 0x2
    b @L_8011B65C
@L_8011B528:
    li r3, 0x2
    b @L_8011B65C
@L_8011B530:
    cmplwi r30, 0x0
    bne @L_8011B540
    li r0, 0x0
    b @L_8011B55C
@L_8011B540:
    mr r3, r30
    bl fn_8011A090
    rlwinm. r0, r3, 0, 16, 31
    bne @L_8011B558
    li r0, 0x0
    b @L_8011B55C
@L_8011B558:
    li r0, 0x1
@L_8011B55C:
    rlwinm r0, r0, 0, 24, 31
    cntlzw r0, r0
    rlwinm r3, r0, 27, 5, 31
    addi r3, r3, 0x1
    b @L_8011B65C
@L_8011B570:
    cmplwi r30, 0x0
    bne @L_8011B580
    li r0, 0x0
    b @L_8011B59C
@L_8011B580:
    mr r3, r30
    bl fn_8011A090
    rlwinm. r0, r3, 0, 16, 31
    bne @L_8011B598
    li r0, 0x0
    b @L_8011B59C
@L_8011B598:
    li r0, 0x1
@L_8011B59C:
    rlwinm. r0, r0, 0, 24, 31
    bne @L_8011B5AC
    li r3, 0x2
    b @L_8011B65C
@L_8011B5AC:
    rlwinm r3, r29, 0, 16, 31
    rlwinm r0, r31, 0, 16, 31
    cmplw r3, r0
    beq @L_8011B5DC
    mr r3, r29
    bl fn_80119E10
    rlwinm r3, r3, 0, 16, 31
    rlwinm r0, r31, 0, 16, 31
    cmplw r0, r3
    beq @L_8011B5DC
    li r3, 0x2
    b @L_8011B65C
@L_8011B5DC:
    li r3, 0x1
    b @L_8011B65C
@L_8011B5E4:
    cmplwi r30, 0x0
    bne @L_8011B5F4
    li r0, 0x0
    b @L_8011B610
@L_8011B5F4:
    mr r3, r30
    bl fn_8011A090
    rlwinm. r0, r3, 0, 16, 31
    bne @L_8011B60C
    li r0, 0x0
    b @L_8011B610
@L_8011B60C:
    li r0, 0x1
@L_8011B610:
    rlwinm. r0, r0, 0, 24, 31
    bne @L_8011B620
    li r3, 0x2
    b @L_8011B65C
@L_8011B620:
    rlwinm r3, r29, 0, 16, 31
    rlwinm r0, r31, 0, 16, 31
    cmplw r3, r0
    beq @L_8011B648
    mr r3, r29
    bl fn_80119E10
    rlwinm r3, r3, 0, 16, 31
    rlwinm r0, r31, 0, 16, 31
    cmplw r0, r3
    bne @L_8011B650
@L_8011B648:
    li r3, 0x2
    b @L_8011B65C
@L_8011B650:
    li r3, 0x1
    b @L_8011B65C
@L_8011B658:
    li r3, 0x0
@L_8011B65C:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
