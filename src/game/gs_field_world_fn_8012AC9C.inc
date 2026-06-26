    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    lis r3, lbl_80426BD0@ha
    stw r0, 0x14(r1)
    addi r4, r3, lbl_80426BD0@l
    stw r31, 0xc(r1)
    stw r30, 0x8(r1)
    lwz r3, 0x184(r4)
    addi r0, r3, 0x1
    cmpwi r0, 0x100
    stw r0, 0x184(r4)
    blt @L_8012AD38
    li r0, 0x0
    li r31, 0x0
    stw r0, 0x184(r4)
@L_8012ACD8:
    clrlwi r5, r31, 16
    li r3, 0x0
    li r4, 0x3
    bl fn_8012A5B0
    mr. r30, r3
    beq @L_8012AD2C
    bl fn_80123FBC
    clrlwi. r0, r3, 24
    beq @L_8012AD2C
    mr r3, r30
    bl fn_8011F1A0
    bl fn_801440A0
    cmplwi r3, 0x0
    bne @L_8012AD18
    li r4, 0x0
    b @L_8012AD20
@L_8012AD18:
    bl fn_80143F6C
    mr r4, r3
@L_8012AD20:
    mr r3, r30
    li r5, 0x5
    bl fn_80122370
@L_8012AD2C:
    addi r31, r31, 0x1
    cmpwi r31, 0x6
    blt @L_8012ACD8
@L_8012AD38:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
