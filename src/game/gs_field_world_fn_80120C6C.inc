    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    clrlwi. r0, r4, 16
    stw r31, 0xc(r1)
    mr r31, r4
    bne @L_80120C90
    li r3, 0x0
    b @L_80120CC8
@L_80120C90:
    li r5, 0x1
    li r6, 0x0
    bl fn_8012640C
    cmplwi r3, 0x0
    bne @L_80120CAC
    li r3, 0x0
    b @L_80120CC8
@L_80120CAC:
    lwz r3, lbl_80478F90(r13)
    clrlwi r0, r31, 16
    lwz r3, 0x0(r3)
    xor r0, r3, r0
    cntlzw r0, r0
    slw r0, r3, r0
    srwi r3, r0, 31
@L_80120CC8:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
