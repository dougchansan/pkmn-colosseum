    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    clrlwi. r0, r4, 16
    stw r31, 0xc(r1)
    mr r31, r5
    stw r30, 0x8(r1)
    mr r30, r4
    beq @L_8012A5DC
    cmplwi r0, 0x1a
    blt @L_8012A5E4
@L_8012A5DC:
    li r3, 0x0
    b @L_8012A75C
@L_8012A5E4:
    cmplwi r3, 0x0
    bne @L_8012A620
    li r3, 0x0
    li r4, 0x0
    bl fn_80129280
    cmplwi r3, 0x0
    bne @L_8012A608
    li r3, 0x0
    b @L_8012A75C
@L_8012A608:
    li r4, 0x2
    bl fn_80129280
    cmplwi r3, 0x0
    bne @L_8012A620
    li r3, 0x0
    b @L_8012A75C
@L_8012A620:
    clrlwi r0, r30, 16
    cmplwi r0, 0x19
    bgt @L_8012A758
    lis r4, jumptable_80363558@ha
    slwi r0, r0, 2
    addi r4, r4, jumptable_80363558@l
    lwzx r0, r4, r0
    mtctr r0
    bctr
    bl fn_8012AC54
    b @L_8012A75C
    bl fn_8012AC3C
    b @L_8012A75C
    mr r4, r31
    bl fn_8012AC08
    b @L_8012A75C
    mr r4, r31
    bl fn_8012ABD4
    b @L_8012A75C
    mr r4, r31
    bl fn_8012ABA0
    b @L_8012A75C
    mr r4, r31
    bl fn_8012AB6C
    b @L_8012A75C
    mr r4, r31
    bl fn_8012AB38
    b @L_8012A75C
    mr r4, r31
    bl fn_8012AB04
    b @L_8012A75C
    mr r4, r31
    bl fn_8012AAD0
    b @L_8012A75C
    mr r4, r31
    bl fn_8012AA9C
    b @L_8012A75C
    bl fn_8012AA2C
    clrlwi r3, r3, 24
    b @L_8012A75C
    bl fn_8012A854
    b @L_8012A75C
    bl fn_8012A80C
    b @L_8012A75C
    bl fn_8012A7C4
    b @L_8012A75C
    bl fn_8012A994
    clrlwi r3, r3, 24
    b @L_8012A75C
    bl fn_8012A97C
    clrlwi r3, r3, 24
    b @L_8012A75C
    bl fn_8012A964
    clrlwi r3, r3, 24
    b @L_8012A75C
    bl fn_8012A94C
    clrlwi r3, r3, 24
    b @L_8012A75C
    bl fn_8012A934
    clrlwi r3, r3, 24
    b @L_8012A75C
    bl fn_8012A91C
    clrlwi r3, r3, 24
    b @L_8012A75C
    bl fn_8012A904
    clrlwi r3, r3, 24
    b @L_8012A75C
    bl fn_8012A8EC
    clrlwi r3, r3, 24
    b @L_8012A75C
    bl fn_8012A8D4
    b @L_8012A75C
    bl fn_8012A79C
    clrlwi r3, r3, 24
    b @L_8012A75C
    bl fn_8012A784
    clrlwi r3, r3, 24
    b @L_8012A75C
@L_8012A758:
    li r3, 0x0
@L_8012A75C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
