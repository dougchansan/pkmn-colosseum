    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr. r31, r3
    bne @L_80122C84
    li r3, 0x0
    b @L_80122DC8
@L_80122C84:
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122CB4
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122CB4
    li r3, 0x0
    b @L_80122CC0
@L_80122CB4:
    mr r3, r31
    li r4, 0x3
    bl fn_8011B67C
@L_80122CC0:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122CD4
    li r3, 0x1
    b @L_80122DC8
@L_80122CD4:
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122D04
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122D04
    li r3, 0x0
    b @L_80122D10
@L_80122D04:
    mr r3, r31
    li r4, 0x5
    bl fn_8011B67C
@L_80122D10:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122D24
    li r3, 0x1
    b @L_80122DC8
@L_80122D24:
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122D54
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122D54
    li r3, 0x0
    b @L_80122D60
@L_80122D54:
    mr r3, r31
    li r4, 0x6
    bl fn_8011B67C
@L_80122D60:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122D74
    li r3, 0x1
    b @L_80122DC8
@L_80122D74:
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80122DA4
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80122DA4
    li r3, 0x0
    b @L_80122DB0
@L_80122DA4:
    mr r3, r31
    li r4, 0x4
    bl fn_8011B67C
@L_80122DB0:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122DC4
    li r3, 0x1
    b @L_80122DC8
@L_80122DC4:
    li r3, 0x0
@L_80122DC8:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
