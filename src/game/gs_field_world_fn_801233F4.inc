    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    stw r30, 0x8(r1)
    mr. r30, r3
    bne @L_80123418
    li r0, 0x0
    b @L_801234E8
@L_80123418:
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r31, r3, 16
    bne @L_80123438
    li r0, 0x0
    b @L_801234E8
@L_80123438:
    bne @L_80123444
    li r0, 0x0
    b @L_80123484
@L_80123444:
    mr r4, r31
    li r3, 0x0
    li r5, 0x1
    li r6, 0x0
    bl fn_8012640C
    cmplwi r3, 0x0
    bne @L_80123468
    li r0, 0x0
    b @L_80123484
@L_80123468:
    lwz r3, lbl_80478F90(r13)
    lwz r0, 0x0(r3)
    cmplw r31, r0
    blt @L_80123480
    li r0, 0x0
    b @L_80123484
@L_80123480:
    li r0, 0x1
@L_80123484:
    clrlwi. r0, r0, 24
    bne @L_80123494
    li r0, 0x0
    b @L_801234E8
@L_80123494:
    mr r3, r30
    li r4, 0x0
    li r5, 0x70
    li r6, 0x0
    bl fn_8012640C
    bl gamedataAttestCheckValid
    clrlwi. r0, r3, 24
    bne @L_801234BC
    li r0, 0x0
    b @L_801234E8
@L_801234BC:
    mr r3, r30
    li r4, 0x0
    li r5, 0xb8
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801234E4
    li r0, 0x0
    b @L_801234E8
@L_801234E4:
    li r0, 0x1
@L_801234E8:
    clrlwi. r0, r0, 24
    bne @L_801234F8
    li r3, 0x0
    b @L_8012356C
@L_801234F8:
    mr r3, r30
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    cmpwi r3, 0x19c
    bne @L_8012351C
    li r3, 0x0
    b @L_8012356C
@L_8012351C:
    mr r3, r30
    li r4, 0x0
    li r5, 0xb6
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80123544
    li r3, 0x0
    b @L_8012356C
@L_80123544:
    mr r3, r30
    li r4, 0x0
    li r5, 0x7b
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 24
    subfic r3, r4, 0x1
    subi r0, r4, 0x1
    or r0, r3, r0
    srwi r3, r0, 31
@L_8012356C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
