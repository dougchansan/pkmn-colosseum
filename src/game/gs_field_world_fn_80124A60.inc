    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    li r0, 0x0
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    mr. r30, r3
    sth r0, 0x8(r1)
    beq @L_80125220
    li r4, 0x0
    li r5, 0xc9
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xc3
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    cmplwi r30, 0x0
    beq @L_80124AD0
    mr r3, r30
    li r4, 0x0
    li r5, 0xc5
    li r6, 0x0
    li r7, -0x64
    bl fn_801254B4
@L_80124AD0:
    mr r3, r30
    li r4, 0x0
    li r5, 0xc6
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xc7
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xc8
    li r6, 0x0
    bl fn_8012640C
    li r4, 0x1
    bl fn_8011B950
    mr r3, r30
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x6f
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x70
    li r6, 0x0
    bl fn_8012640C
    bl gamedataAttestInit
    cmplwi r30, 0x0
    beq @L_80124BFC
    mr r3, r30
    li r4, 0x0
    li r5, 0x71
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x72
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x73
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x74
    li r6, 0x0
    li r7, 0x2
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x75
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    addi r7, r1, 0x8
    li r4, 0x0
    li r5, 0x76
    li r6, 0x0
    bl fn_801254B4
@L_80124BFC:
    mr r3, r30
    addi r7, r1, 0x8
    li r4, 0x0
    li r5, 0x77
    li r6, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x79
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x7a
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x7c
    li r6, 0x0
    bl fn_8012640C
    li r4, 0x1
    bl fn_8011B950
    mr r3, r30
    li r4, 0x0
    li r5, 0x7d
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    cmplwi r30, 0x0
    beq @L_80124CE8
    li r31, 0x0
    b @L_80124CDC
@L_80124C88:
    cmplwi r30, 0x0
    beq @L_80124CD8
    mr r3, r30
    mr r6, r31
    li r4, 0x0
    li r5, 0x7f
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    mr r6, r31
    li r4, 0x0
    li r5, 0x80
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    mr r6, r31
    li r4, 0x0
    li r5, 0x81
    li r7, 0x0
    bl fn_801254B4
@L_80124CD8:
    addi r31, r31, 0x1
@L_80124CDC:
    clrlwi r0, r31, 16
    cmplwi r0, 0x4
    blt @L_80124C88
@L_80124CE8:
    cmplwi r30, 0x0
    beq @L_80124D1C
    mr r3, r30
    li r4, 0x0
    li r5, 0x82
    li r6, 0x0
    bl fn_8012640C
    mr r3, r30
    li r4, 0x0
    li r5, 0x82
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
@L_80124D1C:
    mr r3, r30
    li r4, 0x0
    li r5, 0x83
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x87
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x88
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x89
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x8a
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x8b
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x8c
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x8d
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x8e
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x8f
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x90
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x91
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x92
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x93
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x94
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x95
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x96
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x97
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x98
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x99
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x9c
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x9d
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x9e
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x9f
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xa0
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xa1
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xa3
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xa4
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xa5
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xa6
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xa7
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xa8
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xa9
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xaa
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xab
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xac
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xad
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xae
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xaf
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xb0
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xb1
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xb2
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xb3
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xb4
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xb5
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xb6
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xb7
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xb8
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xb9
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xbb
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xbc
    li r6, 0x0
    li r7, 0xff
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xbd
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0xbe
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    bl fn_8011D480
@L_80125220:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
