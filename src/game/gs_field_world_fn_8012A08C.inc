    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    mr. r29, r4
    stw r28, 0x10(r1)
    mr r28, r3
    bne @L_8012A0BC
    li r3, 0x6
    b @L_8012A110
@L_8012A0BC:
    li r30, 0x0
    b @L_8012A100
@L_8012A0C4:
    mr r3, r28
    clrlwi r5, r30, 24
    li r4, 0x3
    bl fn_8012A5B0
    mr r31, r3
    bl fn_80123FBC
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    beq @L_8012A0FC
    mr r3, r31
    mr r4, r29
    bl fn_8011F5FC
    mr r3, r30
    b @L_8012A110
@L_8012A0FC:
    addi r30, r30, 0x1
@L_8012A100:
    clrlwi r0, r30, 24
    cmplwi r0, 0x6
    blt @L_8012A0C4
    li r3, 0x6
@L_8012A110:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
