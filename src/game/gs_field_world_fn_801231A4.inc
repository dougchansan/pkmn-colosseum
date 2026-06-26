    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    mr. r29, r3
    bne @L_801231CC
    li r3, 0x2
    b @L_801232C4
@L_801231CC:
    li r4, 0x0
    li r5, 0x6f
    li r6, 0x0
    bl fn_8012640C
    mr r31, r3
    mr r3, r29
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x13
    li r6, 0x0
    bl fn_8012640C
    cmplwi r29, 0x0
    clrlwi r30, r3, 16
    bne @L_8012321C
    li r3, 0x2
    b @L_801232A0
@L_8012321C:
    mr r3, r29
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x13
    li r6, 0x0
    bl fn_8012640C
    clrlwi r29, r3, 16
    li r3, 0x0
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r29, r0
    bne @L_80123264
    li r3, 0x0
    b @L_801232A0
@L_80123264:
    li r3, 0x1
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r29, r0
    bne @L_80123280
    li r3, 0x1
    b @L_801232A0
@L_80123280:
    li r3, 0x2
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r29, r0
    bne @L_8012329C
    li r3, 0x2
    b @L_801232A0
@L_8012329C:
    li r3, -0x1
@L_801232A0:
    extsb. r0, r3
    bge @L_801232C0
    clrlwi r0, r31, 24
    cmplw r30, r0
    ble @L_801232BC
    li r3, 0x1
    b @L_801232C0
@L_801232BC:
    li r3, 0x0
@L_801232C0:
    clrlwi r3, r3, 24
@L_801232C4:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
