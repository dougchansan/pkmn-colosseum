    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr. r31, r3
    bne @L_80122DFC
    li r3, 0x0
    b @L_80122FE0
@L_80122DFC:
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122E2C
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122E2C
    li r3, 0x0
    b @L_80122E38
@L_80122E2C:
    mr r3, r31
    li r4, 0x3
    bl fn_8011B67C
@L_80122E38:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122E4C
    li r3, 0x0
    b @L_80122FE0
@L_80122E4C:
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122E7C
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122E7C
    li r3, 0x0
    b @L_80122E88
@L_80122E7C:
    mr r3, r31
    li r4, 0x4
    bl fn_8011B67C
@L_80122E88:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122E9C
    li r3, 0x0
    b @L_80122FE0
@L_80122E9C:
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122ECC
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122ECC
    li r3, 0x0
    b @L_80122ED8
@L_80122ECC:
    mr r3, r31
    li r4, 0x5
    bl fn_8011B67C
@L_80122ED8:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122EEC
    li r3, 0x0
    b @L_80122FE0
@L_80122EEC:
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122F1C
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122F1C
    li r3, 0x0
    b @L_80122F28
@L_80122F1C:
    mr r3, r31
    li r4, 0x6
    bl fn_8011B67C
@L_80122F28:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122F3C
    li r3, 0x0
    b @L_80122FE0
@L_80122F3C:
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122F6C
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122F6C
    li r3, 0x0
    b @L_80122F78
@L_80122F6C:
    mr r3, r31
    li r4, 0x7
    bl fn_8011B67C
@L_80122F78:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122F8C
    li r3, 0x0
    b @L_80122FE0
@L_80122F8C:
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122FBC
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122FBC
    li r3, 0x0
    b @L_80122FC8
@L_80122FBC:
    mr r3, r31
    li r4, 0x8
    bl fn_8011B67C
@L_80122FC8:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122FDC
    li r3, 0x0
    b @L_80122FE0
@L_80122FDC:
    li r3, 0x1
@L_80122FE0:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
