    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    li r31, 0x0
    stw r30, 0x8(r1)
    mr. r30, r3
    bne @L_80120DF8
    li r3, 0x0
    b @L_80120FC8
@L_80120DF8:
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120E28
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80120E28
    li r3, 0x0
    b @L_80120E34
@L_80120E28:
    mr r3, r30
    li r4, 0x3
    bl fn_8011B67C
@L_80120E34:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80120E44
    li r31, 0x3
@L_80120E44:
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120E74
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80120E74
    li r3, 0x0
    b @L_80120E80
@L_80120E74:
    mr r3, r30
    li r4, 0x4
    bl fn_8011B67C
@L_80120E80:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80120E90
    li r31, 0x4
@L_80120E90:
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120EC0
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80120EC0
    li r3, 0x0
    b @L_80120ECC
@L_80120EC0:
    mr r3, r30
    li r4, 0x8
    bl fn_8011B67C
@L_80120ECC:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80120EDC
    li r31, 0x8
@L_80120EDC:
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120F0C
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80120F0C
    li r3, 0x0
    b @L_80120F18
@L_80120F0C:
    mr r3, r30
    li r4, 0x5
    bl fn_8011B67C
@L_80120F18:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80120F28
    li r31, 0x5
@L_80120F28:
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120F58
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80120F58
    li r3, 0x0
    b @L_80120F64
@L_80120F58:
    mr r3, r30
    li r4, 0x6
    bl fn_8011B67C
@L_80120F64:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80120F74
    li r31, 0x6
@L_80120F74:
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80120FA4
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80120FA4
    li r3, 0x0
    b @L_80120FB0
@L_80120FA4:
    mr r3, r30
    li r4, 0x7
    bl fn_8011B67C
@L_80120FB0:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80120FC0
    li r31, 0x7
@L_80120FC0:
    mr r3, r31
    bl fn_80119F50
@L_80120FC8:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
