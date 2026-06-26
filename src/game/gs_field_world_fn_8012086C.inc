    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr. r31, r3
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    stw r28, 0x10(r1)
    beq @L_80120AE0
    li r4, 0x0
    li r5, 0x87
    li r6, 0x0
    bl fn_8012640C
    clrlwi r7, r3, 16
    mr r3, r31
    li r4, 0x0
    li r5, 0x83
    li r6, 0x0
    bl fn_801254B4
    li r28, 0x0
    b @L_8012099C
@L_801208C0:
    cmplwi r31, 0x0
    bne @L_801208D0
    li r0, 0x0
    b @L_80120920
@L_801208D0:
    clrlwi r30, r28, 24
    mr r3, r31
    mr r6, r30
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    cmpwi r3, 0x0
    bne @L_801208F8
    li r0, 0x0
    b @L_80120920
@L_801208F8:
    mr r3, r31
    mr r6, r30
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    cmpwi r3, 0x163
    bne @L_8012091C
    li r0, 0x0
    b @L_80120920
@L_8012091C:
    li r0, 0x1
@L_80120920:
    clrlwi r0, r0, 24
    cmplwi r0, 0x1
    bne @L_80120998
    cmplwi r31, 0x0
    bne @L_8012093C
    li r0, 0x0
    b @L_80120980
@L_8012093C:
    clrlwi r4, r28, 24
    mr r3, r31
    addi r29, r4, 0x4
    li r4, 0x0
    clrlwi r6, r29, 16
    li r5, 0x7f
    bl fn_8012640C
    clrlwi r30, r3, 16
    mr r3, r31
    clrlwi r6, r29, 16
    li r4, 0x0
    li r5, 0x81
    bl fn_8012640C
    clrlwi r4, r3, 24
    mr r3, r30
    bl fn_8011BB6C
    mr r0, r3
@L_80120980:
    mr r3, r31
    clrlwi r6, r28, 24
    clrlwi r7, r0, 24
    li r4, 0x0
    li r5, 0x80
    bl fn_801254B4
@L_80120998:
    addi r28, r28, 0x1
@L_8012099C:
    clrlwi r0, r28, 24
    cmplwi r0, 0x4
    blt @L_801208C0
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_801209D0
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    bne @L_801209DC
@L_801209D0:
    mr r3, r31
    li r4, 0x3
    bl fn_8011B788
@L_801209DC:
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120A04
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    bne @L_80120A10
@L_80120A04:
    mr r3, r31
    li r4, 0x4
    bl fn_8011B788
@L_80120A10:
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120A38
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    bne @L_80120A44
@L_80120A38:
    mr r3, r31
    li r4, 0x5
    bl fn_8011B788
@L_80120A44:
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120A6C
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    bne @L_80120A78
@L_80120A6C:
    mr r3, r31
    li r4, 0x6
    bl fn_8011B788
@L_80120A78:
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120AA0
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    bne @L_80120AAC
@L_80120AA0:
    mr r3, r31
    li r4, 0x7
    bl fn_8011B788
@L_80120AAC:
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120AD4
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    bne @L_80120AE0
@L_80120AD4:
    mr r3, r31
    li r4, 0x8
    bl fn_8011B788
@L_80120AE0:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
