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
    mr r28, r3
    bl fn_8012A248
    bl fn_800E0C54
    clrlwi r31, r3, 16
    bl fn_800E0C54
    slwi r0, r3, 16
    mr r3, r28
    or r5, r0, r31
    li r4, 0x2
    bl fn_8012A450
    mr r3, r28
    mr r5, r29
    li r4, 0x1
    bl fn_8012A450
    mr r3, r28
    clrlwi r5, r30, 24
    li r4, 0xb
    bl fn_8012A450
    li r3, 0xfa2
    bl fn_800FA280
    mr r5, r3
    mr r3, r28
    li r4, 0x17
    bl fn_8012A450
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
