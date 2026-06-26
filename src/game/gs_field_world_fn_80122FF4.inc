    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    li r4, 0x0
    li r5, 0x82
    stw r0, 0x14(r1)
    li r6, 0x0
    stw r31, 0xc(r1)
    bl fn_8012640C
    clrlwi. r31, r3, 16
    bne @L_80123024
    li r3, 0x0
    b @L_8012307C
@L_80123024:
    bl fn_80008154
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80123064
    mr r4, r31
    li r3, 0x0
    li r5, 0x7
    li r6, 0x0
    bl itemGetStatus
    clrlwi r0, r3, 16
    cmplwi r0, 0x1a
    beq @L_8012305C
    cmplwi r0, 0x1e
    bne @L_80123064
@L_8012305C:
    li r3, 0x63
    b @L_8012307C
@L_80123064:
    mr r4, r31
    li r3, 0x0
    li r5, 0xa
    li r6, 0x0
    bl itemGetStatus
    clrlwi r3, r3, 16
@L_8012307C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
