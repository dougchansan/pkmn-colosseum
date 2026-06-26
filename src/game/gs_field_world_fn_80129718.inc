    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    li r5, 0x0
    stw r0, 0x24(r1)
    addic. r0, r1, 0xa
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    mr r30, r4
    sth r5, 0xa(r1)
    sth r5, 0x8(r1)
    beq @L_8012974C
    li r0, 0xa
    sth r0, 0xa(r1)
@L_8012974C:
    addic. r0, r1, 0x8
    beq @L_8012975C
    li r0, 0x1
    sth r0, 0x8(r1)
@L_8012975C:
    li r4, 0xa
    li r5, 0x0
    bl fn_8012A5B0
    mr. r31, r3
    bne @L_80129778
    li r3, 0x0
    b @L_801297C0
@L_80129778:
    lhz r4, 0xa(r1)
    mr r5, r30
    lhz r7, 0x8(r1)
    li r6, 0x1
    bl fn_80142368
    cmplwi r3, 0x0
    beq @L_8012979C
    li r3, 0x1
    b @L_801297C0
@L_8012979C:
    lhz r4, 0xa(r1)
    mr r3, r31
    lhz r7, 0x8(r1)
    mr r5, r30
    li r6, 0x2
    bl fn_80142368
    neg r0, r3
    or r0, r0, r3
    srwi r3, r0, 31
@L_801297C0:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
