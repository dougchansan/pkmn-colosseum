    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    mr r30, r5
    stw r29, 0x14(r1)
    mr r29, r4
    stw r28, 0x10(r1)
    mr. r28, r3
    beq @L_80123E50
    beq @L_80123DCC
    mr r6, r29
    li r4, 0x0
    li r5, 0x7f
    li r7, 0x0
    bl fn_801254B4
    mr r3, r28
    mr r6, r29
    li r4, 0x0
    li r5, 0x80
    li r7, 0x0
    bl fn_801254B4
    mr r3, r28
    mr r6, r29
    li r4, 0x0
    li r5, 0x81
    li r7, 0x0
    bl fn_801254B4
@L_80123DCC:
    mr r3, r28
    mr r6, r29
    clrlwi r7, r30, 16
    li r4, 0x0
    li r5, 0x7f
    bl fn_801254B4
    cmplwi r28, 0x0
    bne @L_80123DF4
    li r0, 0x0
    b @L_80123E38
@L_80123DF4:
    clrlwi r4, r29, 16
    mr r3, r28
    addi r30, r4, 0x4
    li r4, 0x0
    clrlwi r6, r30, 16
    li r5, 0x7f
    bl fn_8012640C
    clrlwi r31, r3, 16
    mr r3, r28
    clrlwi r6, r30, 16
    li r4, 0x0
    li r5, 0x81
    bl fn_8012640C
    clrlwi r4, r3, 24
    mr r3, r31
    bl fn_8011BB6C
    mr r0, r3
@L_80123E38:
    mr r3, r28
    mr r6, r29
    clrlwi r7, r0, 24
    li r4, 0x0
    li r5, 0x80
    bl fn_801254B4
@L_80123E50:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
