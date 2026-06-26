    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    clrlwi. r0, r5, 16
    stmw r27, 0xc(r1)
    mr r30, r5
    mr r27, r3
    mr r29, r4
    beq @L_8011A26C
    mr r3, r30
    bl fn_80119E90
    clrlwi r28, r3, 24
    mr r3, r30
    bl fn_80119F10
    mr r31, r3
    mr r3, r30
    bl fn_80119ED0
    mr r6, r3
    mr r3, r31
    mr r4, r27
    li r5, 0x0
    li r7, 0x0
    bl statusGetStatus
    cmplwi r3, 0x0
    bne @L_8011A114
    li r31, 0x0
    b @L_8011A11C
@L_8011A114:
    clrlslwi r0, r28, 16, 4
    add r31, r3, r0
@L_8011A11C:
    cmplwi r31, 0x0
    beq @L_8011A26C
    clrlwi. r0, r30, 16
    bne @L_8011A134
    li r0, 0x0
    b @L_8011A1EC
@L_8011A134:
    mr r3, r30
    bl fn_80119E90
    clrlwi r28, r3, 24
    mr r3, r30
    bl fn_80119F10
    mr r27, r3
    mr r3, r30
    bl fn_80119ED0
    mr r6, r3
    mr r3, r27
    mr r4, r29
    li r5, 0x0
    li r7, 0x0
    bl statusGetStatus
    cmplwi r3, 0x0
    bne @L_8011A17C
    li r27, 0x0
    b @L_8011A184
@L_8011A17C:
    clrlslwi r0, r28, 16, 4
    add r27, r3, r0
@L_8011A184:
    cmplwi r27, 0x0
    bne @L_8011A194
    li r0, 0x0
    b @L_8011A1EC
@L_8011A194:
    bne @L_8011A1A0
    li r0, 0x0
    b @L_8011A1BC
@L_8011A1A0:
    mr r3, r27
    bl fn_8011A090
    clrlwi. r0, r3, 16
    bne @L_8011A1B8
    li r0, 0x0
    b @L_8011A1BC
@L_8011A1B8:
    li r0, 0x1
@L_8011A1BC:
    clrlwi r0, r0, 24
    cmplwi r0, 0x1
    bne @L_8011A1E8
    mr r3, r27
    bl fn_8011A090
    clrlwi r3, r3, 16
    clrlwi r0, r30, 16
    cmplw r0, r3
    bne @L_8011A1E8
    li r0, 0x1
    b @L_8011A1EC
@L_8011A1E8:
    li r0, 0x0
@L_8011A1EC:
    clrlwi. r0, r0, 24
    beq @L_8011A26C
    mr r3, r30
    bl fn_80119E90
    clrlwi r27, r3, 24
    mr r3, r30
    bl fn_80119F10
    mr r28, r3
    mr r3, r30
    bl fn_80119ED0
    mr r6, r3
    mr r3, r28
    mr r4, r29
    li r5, 0x0
    li r7, 0x0
    bl statusGetStatus
    cmplwi r3, 0x0
    bne @L_8011A23C
    li r4, 0x0
    b @L_8011A244
@L_8011A23C:
    clrlslwi r0, r27, 16, 4
    add r4, r3, r0
@L_8011A244:
    cmplwi r4, 0x0
    beq @L_8011A26C
    lwz r3, 0x0(r4)
    lwz r0, 0x4(r4)
    stw r3, 0x0(r31)
    stw r0, 0x4(r31)
    lwz r3, 0x8(r4)
    lwz r0, 0xc(r4)
    stw r3, 0x8(r31)
    stw r0, 0xc(r31)
@L_8011A26C:
    lmw r27, 0xc(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
