    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    stw r0, 0x34(r1)
    stmw r27, 0x1c(r1)
    mr. r29, r3
    mr r30, r4
    mr r31, r5
    mr r27, r6
    beq @L_801243FC
    clrlwi. r0, r30, 16
    bne @L_801240F8
    li r0, 0x0
    b @L_80124138
@L_801240F8:
    li r3, 0x0
    li r5, 0x1
    li r6, 0x0
    bl fn_8012640C
    cmplwi r3, 0x0
    bne @L_80124118
    li r0, 0x0
    b @L_80124138
@L_80124118:
    lwz r3, lbl_80478F90(r13)
    clrlwi r4, r30, 16
    lwz r0, 0x0(r3)
    cmplw r4, r0
    blt @L_80124134
    li r0, 0x0
    b @L_80124138
@L_80124134:
    li r0, 0x1
@L_80124138:
    clrlwi. r0, r0, 24
    beq @L_801243FC
    cmplwi r27, 0x0
    beq @L_801243FC
    mr r3, r29
    bl fn_80124A60
    mr r3, r29
    clrlwi r7, r30, 16
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_801254B4
    bl fn_800E0C54
    slwi r28, r3, 16
    bl fn_800E0C54
    clrlwi r0, r3, 16
    mr r3, r29
    or r7, r0, r28
    li r4, 0x0
    li r5, 0x6f
    li r6, 0x0
    bl fn_801254B4
    mr r3, r29
    li r4, 0x0
    li r5, 0x70
    li r6, 0x0
    bl fn_8012640C
    mr r4, r27
    bl fn_80135AD0
    mr r4, r30
    li r3, 0x0
    li r5, 0x1
    li r6, 0x0
    bl fn_8012640C
    bl fn_800FA280
    mr r7, r3
    mr r3, r29
    li r4, 0x0
    li r5, 0x77
    li r6, 0x0
    bl fn_801254B4
    mr r3, r29
    clrlwi r7, r31, 24
    li r4, 0x0
    li r5, 0x7a
    li r6, 0x0
    bl fn_801254B4
    bl fn_800E0C54
    clrlwi r7, r3, 27
    clrlwi r27, r3, 16
    mr r3, r29
    li r4, 0x0
    li r5, 0x93
    li r6, 0x0
    bl fn_801254B4
    extrwi r7, r27, 5, 22
    mr r3, r29
    li r4, 0x0
    li r5, 0x94
    li r6, 0x0
    bl fn_801254B4
    extrwi r7, r27, 5, 17
    mr r3, r29
    li r4, 0x0
    li r5, 0x95
    li r6, 0x0
    bl fn_801254B4
    bl fn_800E0C54
    clrlwi r7, r3, 27
    clrlwi r27, r3, 16
    mr r3, r29
    li r4, 0x0
    li r5, 0x98
    li r6, 0x0
    bl fn_801254B4
    mr r3, r29
    extrwi r7, r27, 5, 22
    li r4, 0x0
    li r5, 0x96
    li r6, 0x0
    bl fn_801254B4
    mr r3, r29
    extrwi r7, r27, 5, 17
    li r4, 0x0
    li r5, 0x97
    li r6, 0x0
    bl fn_801254B4
    mr r3, r29
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x11
    li r6, 0x0
    bl fn_8012640C
    clrlwi r27, r3, 24
    mr r3, r29
    li r4, 0x0
    li r5, 0x7a
    li r6, 0x0
    bl fn_8012640C
    clrlwi r28, r3, 24
    mr r3, r27
    bl fn_8011CE74
    cmplwi r3, 0x0
    bne @L_801242F0
    li r7, 0x0
    b @L_801242FC
@L_801242F0:
    mr r4, r28
    bl fn_8011CE44
    mr r7, r3
@L_801242FC:
    mr r3, r29
    li r4, 0x0
    li r5, 0x79
    li r6, 0x0
    bl fn_801254B4
    mr r4, r30
    li r3, 0x0
    li r5, 0x14
    li r6, 0x0
    bl fn_8012640C
    mr r0, r3
    mr r3, r29
    mr r7, r0
    li r4, 0x0
    li r5, 0x99
    li r6, 0x0
    bl fn_801254B4
    bl fn_800E0C54
    clrlwi. r0, r3, 31
    beq @L_801243A4
    cmplwi r29, 0x0
    li r27, 0x1
    beq @L_801243A4
    mr r3, r29
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x17
    li r6, 0x1
    bl fn_8012640C
    cmpwi r3, 0x0
    bne @L_8012438C
    li r27, 0x0
@L_8012438C:
    mr r3, r29
    clrlwi r7, r27, 24
    li r4, 0x0
    li r5, 0xb7
    li r6, 0x0
    bl fn_801254B4
@L_801243A4:
    clrlwi r27, r31, 24
    li r28, 0x1
    li r30, 0x0
    b @L_801243EC
@L_801243B4:
    stb r30, 0x8(r1)
@L_801243B8:
    mr r3, r29
    clrlwi r4, r28, 24
    addi r6, r1, 0x8
    li r5, 0x1
    bl fn_8012361C
    extsb r0, r3
    cmpwi r0, -0x3
    beq @L_801243E8
    lbz r3, 0x8(r1)
    addi r0, r3, 0x1
    stb r0, 0x8(r1)
    b @L_801243B8
@L_801243E8:
    addi r28, r28, 0x1
@L_801243EC:
    cmplw r28, r27
    ble @L_801243B4
    mr r3, r29
    bl fn_8012546C
@L_801243FC:
    lmw r27, 0x1c(r1)
    lwz r0, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr
