    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    stw r0, 0x34(r1)
    stmw r25, 0x14(r1)
    mr. r30, r3
    mr r31, r4
    mr r25, r5
    bne @L_801237E0
    li r3, -0x2
    b @L_80123B48
@L_801237E0:
    bne @L_801237EC
    li r29, -0x1
    b @L_80123894
@L_801237EC:
    clrlwi r28, r31, 16
    li r29, 0x0
    b @L_80123884
@L_801237F8:
    cmplwi r30, 0x0
    extsb r27, r29
    bne @L_8012380C
    li r0, 0x0
    b @L_80123858
@L_8012380C:
    mr r3, r30
    mr r6, r27
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    cmpwi r3, 0x0
    bne @L_80123830
    li r0, 0x0
    b @L_80123858
@L_80123830:
    mr r3, r30
    mr r6, r27
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    cmpwi r3, 0x163
    bne @L_80123854
    li r0, 0x0
    b @L_80123858
@L_80123854:
    li r0, 0x1
@L_80123858:
    clrlwi. r0, r0, 24
    beq @L_80123880
    mr r3, r30
    mr r6, r27
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    cmpw r28, r3
    bne @L_80123880
    b @L_80123894
@L_80123880:
    addi r29, r29, 0x1
@L_80123884:
    extsb r0, r29
    cmpwi r0, 0x4
    blt @L_801237F8
    li r29, -0x1
@L_80123894:
    extsb. r0, r29
    blt @L_801238A4
    li r3, -0x2
    b @L_80123B48
@L_801238A4:
    li r26, 0x0
    b @L_80123A00
@L_801238AC:
    cmplwi r30, 0x0
    bne @L_801238BC
    li r0, 0x0
    b @L_8012390C
@L_801238BC:
    extsb r27, r26
    mr r3, r30
    mr r6, r27
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    cmpwi r3, 0x0
    bne @L_801238E4
    li r0, 0x0
    b @L_8012390C
@L_801238E4:
    mr r3, r30
    mr r6, r27
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    cmpwi r3, 0x163
    bne @L_80123908
    li r0, 0x0
    b @L_8012390C
@L_80123908:
    li r0, 0x1
@L_8012390C:
    clrlwi. r0, r0, 24
    bne @L_801239FC
    cmplwi r30, 0x0
    beq @L_801239F4
    beq @L_8012396C
    extsb r27, r26
    mr r3, r30
    mr r6, r27
    li r4, 0x0
    li r5, 0x7f
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    mr r6, r27
    li r4, 0x0
    li r5, 0x80
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    mr r6, r27
    li r4, 0x0
    li r5, 0x81
    li r7, 0x0
    bl fn_801254B4
@L_8012396C:
    extsb r27, r26
    mr r3, r30
    mr r6, r27
    clrlwi r7, r31, 16
    li r4, 0x0
    li r5, 0x7f
    bl fn_801254B4
    cmplwi r30, 0x0
    bne @L_80123998
    li r0, 0x0
    b @L_801239DC
@L_80123998:
    clrlwi r4, r27, 16
    mr r3, r30
    addi r28, r4, 0x4
    li r4, 0x0
    clrlwi r6, r28, 16
    li r5, 0x7f
    bl fn_8012640C
    clrlwi r29, r3, 16
    mr r3, r30
    clrlwi r6, r28, 16
    li r4, 0x0
    li r5, 0x81
    bl fn_8012640C
    clrlwi r4, r3, 24
    mr r3, r29
    bl fn_8011BB6C
    mr r0, r3
@L_801239DC:
    mr r3, r30
    mr r6, r27
    clrlwi r7, r0, 24
    li r4, 0x0
    li r5, 0x80
    bl fn_801254B4
@L_801239F4:
    mr r3, r26
    b @L_80123B48
@L_801239FC:
    addi r26, r26, 0x1
@L_80123A00:
    extsb r0, r26
    cmpwi r0, 0x4
    blt @L_801238AC
    clrlwi. r0, r25, 24
    bne @L_80123A1C
    li r3, -0x1
    b @L_80123B48
@L_80123A1C:
    li r27, 0x1
    b @L_80123A68
@L_80123A24:
    cmplwi r30, 0x0
    beq @L_80123A64
    extsb r4, r27
    mr r3, r30
    subi r0, r4, 0x1
    li r5, 0x0
    clrlwi r4, r0, 16
    bl fn_8011F260
    mr r29, r3
    mr r3, r30
    extsb r4, r27
    li r5, 0x0
    bl fn_8011F260
    mr r4, r3
    mr r3, r29
    bl fn_8011F5E0
@L_80123A64:
    addi r27, r27, 0x1
@L_80123A68:
    extsb r0, r27
    cmpwi r0, 0x4
    blt @L_80123A24
    cmplwi r30, 0x0
    beq @L_80123B44
    beq @L_80123AC8
    mr r3, r30
    li r4, 0x0
    li r5, 0x7f
    li r6, 0x3
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x80
    li r6, 0x3
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x81
    li r6, 0x3
    li r7, 0x0
    bl fn_801254B4
@L_80123AC8:
    mr r3, r30
    clrlwi r7, r31, 16
    li r4, 0x0
    li r5, 0x7f
    li r6, 0x3
    bl fn_801254B4
    cmplwi r30, 0x0
    bne @L_80123AF0
    li r0, 0x0
    b @L_80123B2C
@L_80123AF0:
    mr r3, r30
    li r4, 0x0
    li r5, 0x7f
    li r6, 0x7
    bl fn_8012640C
    clrlwi r29, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x81
    li r6, 0x7
    bl fn_8012640C
    clrlwi r4, r3, 24
    mr r3, r29
    bl fn_8011BB6C
    mr r0, r3
@L_80123B2C:
    mr r3, r30
    clrlwi r7, r0, 24
    li r4, 0x0
    li r5, 0x80
    li r6, 0x3
    bl fn_801254B4
@L_80123B44:
    li r3, 0x3
@L_80123B48:
    lmw r25, 0x14(r1)
    lwz r0, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr
