    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr. r31, r3
    bne @L_80121000
    li r3, 0x0
    b @L_801211E4
@L_80121000:
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121030
    li r3, 0x3
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121030
    li r3, 0x0
    b @L_8012103C
@L_80121030:
    mr r3, r31
    li r4, 0x3
    bl fn_8011B67C
@L_8012103C:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121050
    li r3, 0x3a
    b @L_801211E4
@L_80121050:
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121080
    li r3, 0x4
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121080
    li r3, 0x0
    b @L_8012108C
@L_80121080:
    mr r3, r31
    li r4, 0x4
    bl fn_8011B67C
@L_8012108C:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801210A0
    li r3, 0x3a
    b @L_801211E4
@L_801210A0:
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_801210D0
    li r3, 0x5
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_801210D0
    li r3, 0x0
    b @L_801210DC
@L_801210D0:
    mr r3, r31
    li r4, 0x5
    bl fn_8011B67C
@L_801210DC:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801210F0
    li r3, 0x3b
    b @L_801211E4
@L_801210F0:
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121120
    li r3, 0x6
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121120
    li r3, 0x0
    b @L_8012112C
@L_80121120:
    mr r3, r31
    li r4, 0x6
    bl fn_8011B67C
@L_8012112C:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121140
    li r3, 0x3c
    b @L_801211E4
@L_80121140:
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_80121170
    li r3, 0x7
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_80121170
    li r3, 0x0
    b @L_8012117C
@L_80121170:
    mr r3, r31
    li r4, 0x7
    bl fn_8011B67C
@L_8012117C:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80121190
    li r3, 0x3d
    b @L_801211E4
@L_80121190:
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_801211C0
    li r3, 0x8
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_801211C0
    li r3, 0x0
    b @L_801211CC
@L_801211C0:
    mr r3, r31
    li r4, 0x8
    bl fn_8011B67C
@L_801211CC:
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801211E0
    li r3, 0x3e
    b @L_801211E4
@L_801211E0:
    li r3, 0x0
@L_801211E4:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
