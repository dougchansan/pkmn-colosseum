    nofralloc
    stwu r1, -0x160(r1)
    mflr r0
    stw r0, 0x164(r1)
    stmw r24, 0x140(r1)
    mr. r28, r4
    mr r24, r3
    mr r25, r5
    mr r26, r6
    mr r27, r7
    bne @L_80129F50
    li r3, 0x6
    b @L_8012A078
@L_80129F50:
    mr r3, r28
    li r4, 0x0
    li r5, 0x7a
    li r6, 0x0
    bl fn_8012640C
    clrlwi r30, r3, 24
    mr r3, r24
    li r4, 0xb
    li r5, 0x0
    bl fn_8012A5B0
    clrlwi r31, r3, 24
    mr r3, r24
    li r4, 0x2
    li r5, 0x0
    bl fn_8012A5B0
    mr r0, r3
    mr r3, r24
    mr r29, r0
    li r4, 0x1
    li r5, 0x0
    bl fn_8012A5B0
    mr r4, r28
    mr r28, r3
    addi r3, r1, 0x8
    bl fn_8011F5FC
    mr r4, r25
    mr r5, r30
    mr r6, r26
    mr r7, r31
    mr r8, r29
    mr r9, r28
    addi r3, r1, 0x8
    bl fn_80123EF0
    addic. r0, r1, 0x8
    bne @L_80129FE4
    li r31, 0x6
    b @L_8012A034
@L_80129FE4:
    li r31, 0x0
    b @L_8012A024
@L_80129FEC:
    mr r3, r24
    clrlwi r5, r31, 24
    li r4, 0x3
    bl fn_8012A5B0
    mr r30, r3
    bl fn_80123FBC
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    beq @L_8012A020
    mr r3, r30
    addi r4, r1, 0x8
    bl fn_8011F5FC
    b @L_8012A034
@L_8012A020:
    addi r31, r31, 0x1
@L_8012A024:
    clrlwi r0, r31, 24
    cmplwi r0, 0x6
    blt @L_80129FEC
    li r31, 0x6
@L_8012A034:
    clrlwi r0, r31, 24
    cmplwi r0, 0x6
    blt @L_8012A074
    clrlwi. r0, r27, 24
    bne @L_8012A050
    li r3, -0x2
    b @L_8012A078
@L_8012A050:
    addi r4, r1, 0x8
    li r3, 0x0
    li r5, -0x1
    bl fn_80134BC0
    subfic r0, r3, 0x1
    cntlzw r0, r0
    srwi r3, r0, 5
    subi r3, r3, 0x2
    b @L_8012A078
@L_8012A074:
    extsh r3, r0
@L_8012A078:
    lmw r24, 0x140(r1)
    lwz r0, 0x164(r1)
    mtlr r0
    addi r1, r1, 0x160
    blr
