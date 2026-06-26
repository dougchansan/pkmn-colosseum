    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stmw r27, 0xc(r1)
    mr r27, r3
    mr r28, r4
    mr r29, r5
    li r30, 0x0
    li r3, 0x3ca
    li r4, 0x0
    li r5, 0xff
    bl fn_801653CC
    clrlwi r0, r27, 24
    cmpwi r0, 0x3
    beq @L_80115FFC
    bge @L_80115EBC
    cmpwi r0, 0x1
    beq @L_80115F1C
    bge @L_80115EC8
    b @L_8011614C
@L_80115EBC:
    cmpwi r0, 0x5
    bge @L_8011614C
    b @L_80115F1C
@L_80115EC8:
    mr r4, r28
    li r3, 0x4b
    bl fn_80132A38
    li r3, 0x3
    li r4, 0x3cb5
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
    mr r4, r28
    li r3, 0x0
    bl fn_801294C4
    li r3, 0x3
    li r4, 0x3cb7
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
    b @L_8011614C
@L_80115F1C:
    cmpwi r29, 0x0
    ble @L_8011614C
    mr r4, r28
    li r3, 0x2d
    bl fn_80132A38
    mr r4, r29
    li r3, 0x2f
    bl fn_80132A38
    clrlwi r0, r27, 24
    cmplwi r0, 0x1
    bne @L_80115F78
    cmpwi r29, 0x1
    bne @L_80115F58
    li r30, 0x3cb4
    b @L_80115F5C
@L_80115F58:
    li r30, 0x3cb9
@L_80115F5C:
    mr r4, r30
    li r3, 0x3
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
@L_80115F78:
    clrlwi r4, r28, 16
    clrlwi r5, r29, 16
    li r3, 0x0
    li r6, -0x1
    bl fn_80129A78
    mr r31, r3
    cmpwi r31, 0x0
    bne @L_80115FB0
    cmpwi r29, 0x1
    bne @L_80115FA8
    li r30, 0x3cb8
    b @L_80115FDC
@L_80115FA8:
    li r30, 0x3cbd
    b @L_80115FDC
@L_80115FB0:
    ble @L_80115FDC
    clrlwi r4, r28, 16
    clrlwi r5, r31, 16
    li r3, 0x0
    bl fn_8013467C
    cmpwi r29, 0x1
    clrlwi r31, r3, 16
    bne @L_80115FD8
    li r30, 0x3cba
    b @L_80115FDC
@L_80115FD8:
    li r30, 0x3cbb
@L_80115FDC:
    mr r4, r30
    li r3, 0x3
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
    b @L_8011614C
@L_80115FFC:
    mr r4, r28
    li r3, 0x2d
    bl fn_80132A38
    li r3, 0x3
    li r4, 0x3cbc
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
    clrlwi r4, r28, 16
    li r3, 0x0
    li r5, 0x1
    li r6, -0x1
    bl fn_80129A78
    li r3, 0x3
    li r4, 0x3cb6
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
    cmpwi r28, 0x21d
    beq @L_8011608C
    bge @L_80116078
    cmpwi r28, 0x21b
    beq @L_80116094
    bge @L_8011609C
    cmpwi r28, 0x21a
    bge @L_80116084
    b @L_801160A8
@L_80116078:
    cmpwi r28, 0x223
    beq @L_801160A4
    b @L_801160A8
@L_80116084:
    li r30, 0x3b33
    b @L_801160A8
@L_8011608C:
    li r30, 0x3b35
    b @L_801160A8
@L_80116094:
    li r30, 0x3b39
    b @L_801160A8
@L_8011609C:
    li r30, 0x3b37
    b @L_801160A8
@L_801160A4:
    li r30, 0x44c4
@L_801160A8:
    mr r4, r30
    li r3, 0x3
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    bl fn_8001E184
    extsb r31, r3
    li r3, 0x1
    bl fn_801069FC
    cmpwi r31, 0x0
    beq @L_801160DC
    li r3, 0x0
    b @L_80116150
@L_801160DC:
    cmpwi r28, 0x21d
    beq @L_80116114
    bge @L_80116100
    cmpwi r28, 0x21b
    beq @L_8011611C
    bge @L_80116124
    cmpwi r28, 0x21a
    bge @L_8011610C
    b @L_80116130
@L_80116100:
    cmpwi r28, 0x223
    beq @L_8011612C
    b @L_80116130
@L_8011610C:
    li r30, 0x3b34
    b @L_80116130
@L_80116114:
    li r30, 0x3b36
    b @L_80116130
@L_8011611C:
    li r30, 0x3b30
    b @L_80116130
@L_80116124:
    li r30, 0x3b38
    b @L_80116130
@L_8011612C:
    li r30, 0x44c5
@L_80116130:
    mr r4, r30
    li r3, 0x3
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
@L_8011614C:
    mr r3, r31
@L_80116150:
    lmw r27, 0xc(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
