    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    mr. r29, r3
    bne @L_80123E98
    li r3, 0x0
    b @L_80123ED4
@L_80123E98:
    clrlwi r5, r4, 16
    li r4, 0x0
    addi r31, r5, 0x4
    li r5, 0x7f
    clrlwi r6, r31, 16
    bl fn_8012640C
    clrlwi r30, r3, 16
    mr r3, r29
    clrlwi r6, r31, 16
    li r4, 0x0
    li r5, 0x81
    bl fn_8012640C
    clrlwi r4, r3, 24
    mr r3, r30
    bl fn_8011BB6C
@L_80123ED4:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
