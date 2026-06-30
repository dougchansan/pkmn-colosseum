    nofralloc
    stwu r1, -0x40(r1)
    mflr r0
    stw r0, 0x44(r1)
    stmw r24, 0x20(r1)
    mr r30, r3
    mr r29, r4
    clrlwi r3, r30, 16
    bl fn_801440A0
    mr r26, r3
    bl fn_80143F24
    clrlwi r24, r3, 24
    mr r3, r24
    bl fn_80143EF0
    clrlwi r25, r3, 16
    li r3, 0x4cb
    li r4, 0x0
    li r5, 0xff
    li r6, 0x0
    bl fn_80166A50
    mr r3, r26
    bl fn_80143E88
    mr r31, r3
    clrlwi r0, r31, 24
    cmplwi r0, 0xff
    beq @L_800228A0
    li r26, 0x4260
    b @L_800228A4
@L_800228A0:
    li r26, 0x4265
@L_800228A4:
    mr r4, r25
    li r3, 0x39
    bl fn_80132A38
    mr r4, r26
    li r3, 0x2
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
    li r3, 0x2
    li r4, 0x426b
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x0
    li r4, -0x1
    li r5, -0x1
    li r6, 0x0
    bl fn_8001E074
    extsb r28, r3
    li r3, 0x1
    bl fn_801069FC
    cmpwi r28, 0x1
    beq @L_80022920
    bge @L_80022928
    cmpwi r28, 0x0
    bge @L_80022918
    b @L_80022928
@L_80022918:
    li r0, 0x0
    b @L_8002292C
@L_80022920:
    li r0, 0x1
    b @L_8002292C
@L_80022928:
    li r0, 0x2
@L_8002292C:
    cmpwi r0, 0x1
    beq @L_8002293C
    cmpwi r0, 0x2
    bne @L_80022944
@L_8002293C:
    li r3, 0x1
    b @L_80022B28
@L_80022944:
    mr r3, r30
    li r4, 0x1
    bl fn_800141BC
    mr r30, r3
    cmpwi r30, 0x0
    blt @L_80022AE4
    addi r4, r1, 0x10
    addi r5, r1, 0xc
    bl fn_80014118
    lwz r25, 0x10(r1)
    mr r3, r25
    bl fn_8011FC74
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    beq @L_800229A4
    li r3, 0x2
    li r4, 0x424c
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
    li r28, 0x0
    b @L_80022AE0
@L_800229A4:
    mr r3, r24
    bl fn_80143EF0
    li r26, 0x0
    mr r27, r3
    clrlwi r28, r27, 16
@L_800229B8:
    mr r3, r25
    clrlwi r4, r26, 16
    bl fn_8011F228
    clrlwi r0, r3, 16
    cmplw r28, r0
    beq @L_800229DC
    addi r26, r26, 0x1
    cmpwi r26, 0x4
    blt @L_800229B8
@L_800229DC:
    cmpwi r26, 0x4
    bge @L_80022A28
    mr r3, r25
    bl fn_8011F4F0
    mr r4, r3
    li r3, 0x32
    bl fn_80132A38
    clrlwi r4, r27, 16
    li r3, 0x39
    bl fn_80132A38
    li r3, 0x2
    li r4, 0x4244
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
    li r28, 0x0
    b @L_80022AE0
@L_80022A28:
    mr r3, r25
    bl fn_8011F5C8
    bl fn_8011E778
    mr r4, r24
    bl fn_8011E2AC
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_80022A8C
    mr r3, r25
    bl fn_8011F4F0
    mr r4, r3
    li r3, 0x32
    bl fn_80132A38
    clrlwi r4, r27, 16
    li r3, 0x39
    bl fn_80132A38
    li r3, 0x2
    li r4, 0x423f
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
    li r28, 0x0
    b @L_80022AE0
@L_80022A8C:
    lis r4, fn_80023274@ha
    mr r3, r25
    addi r7, r4, fn_80023274@l
    addi r5, r1, 0x8
    mr r4, r27
    li r6, 0x1
    li r8, 0x0
    bl fn_802600E4
    mr r28, r3
    cmpwi r28, 0x0
    beq @L_80022AE0
    lbz r4, 0x8(r1)
    mr r3, r25
    clrlwi r5, r27, 16
    bl fn_80123D58
    mr r3, r25
    bl fn_80123090
    mr r4, r3
    mr r3, r25
    li r5, 0x4
    bl fn_80122370
@L_80022AE0:
    mr r27, r28
@L_80022AE4:
    mr r3, r30
    bl fn_80014198
    cmpwi r30, 0x0
    blt @L_80022B24
    cmpwi r27, 0x0
    beq @L_80022B24
    clrlwi r0, r31, 24
    cmplwi r0, 0xff
    beq @L_80022B14
    li r0, 0x0
    stw r0, 0x0(r29)
    b @L_80022B1C
@L_80022B14:
    li r0, 0x1
    stw r0, 0x0(r29)
@L_80022B1C:
    li r3, 0x0
    b @L_80022B28
@L_80022B24:
    li r3, 0x1
@L_80022B28:
    lmw r24, 0x20(r1)
    lwz r0, 0x44(r1)
    mtlr r0
    addi r1, r1, 0x40
    blr

