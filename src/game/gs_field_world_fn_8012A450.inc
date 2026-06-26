    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    clrlwi. r0, r4, 16
    stw r31, 0xc(r1)
    mr r31, r5
    stw r30, 0x8(r1)
    mr r30, r4
    beq @L_8012A598
    cmplwi r0, 0x1a
    blt @L_8012A480
    b @L_8012A598
@L_8012A480:
    cmplwi r3, 0x0
    bne @L_8012A4AC
    li r3, 0x0
    li r4, 0x0
    bl fn_80129280
    cmplwi r3, 0x0
    beq @L_8012A598
    li r4, 0x2
    bl fn_80129280
    cmplwi r3, 0x0
    beq @L_8012A598
@L_8012A4AC:
    clrlwi r0, r30, 16
    cmplwi r0, 0x19
    bgt @L_8012A598
    lis r4, jumptable_803634F0@ha
    slwi r0, r0, 2
    addi r4, r4, jumptable_803634F0@l
    lwzx r0, r4, r0
    mtctr r0
    bctr
    mr r4, r31
    bl fn_8012AA64
    b @L_8012A598
    mr r4, r31
    bl fn_8012AA54
    b @L_8012A598
    clrlwi r4, r31, 24
    bl fn_8012AA44
    b @L_8012A598
    mr r4, r31
    bl fn_8012A86C
    b @L_8012A598
    mr r4, r31
    bl fn_8012A824
    b @L_8012A598
    mr r4, r31
    bl fn_8012A7DC
    b @L_8012A598
    clrlwi r4, r31, 24
    bl fn_8012AA1C
    b @L_8012A598
    clrlwi r4, r31, 24
    bl fn_8012AA0C
    b @L_8012A598
    clrlwi r4, r31, 24
    bl fn_8012A9FC
    b @L_8012A598
    clrlwi r4, r31, 24
    bl fn_8012A9EC
    b @L_8012A598
    clrlwi r4, r31, 24
    bl fn_8012A9DC
    b @L_8012A598
    clrlwi r4, r31, 24
    bl fn_8012A9CC
    b @L_8012A598
    clrlwi r4, r31, 24
    bl fn_8012A9BC
    b @L_8012A598
    clrlwi r4, r31, 24
    bl fn_8012A9AC
    b @L_8012A598
    mr r4, r31
    bl fn_8012A89C
    b @L_8012A598
    clrlwi r4, r31, 24
    bl fn_8012A7B4
    b @L_8012A598
    clrlwi r4, r31, 24
    bl fn_8012A774
@L_8012A598:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
