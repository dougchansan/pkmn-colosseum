    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    clrlwi. r0, r5, 16
    stw r31, 0x1c(r1)
    mr r31, r7
    stw r30, 0x18(r1)
    mr r30, r6
    stw r29, 0x14(r1)
    mr r29, r5
    beq @L_801263F0
    cmplwi r0, 0x124
    blt @L_801254EC
    b @L_801263F0
@L_801254EC:
    cmplwi r0, 0x6d
    bge @L_80125508
    mr r3, r4
    bl fn_8011E778
    cmplwi r3, 0x0
    bne @L_80125510
    b @L_801263F0
@L_80125508:
    cmplwi r3, 0x0
    beq @L_801263F0
@L_80125510:
    clrlwi r0, r29, 16
    cmplwi r0, 0x121
    bgt @L_801263F0
    lis r4, jumptable_8035E028@ha
    slwi r0, r0, 2
    addi r4, r4, jumptable_8035E028@l
    lwzx r0, r4, r0
    mtctr r0
    bctr
    mr r4, r31
    bl fn_8011D470
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D44C
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D428
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D404
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D3E0
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D3BC
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D398
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D374
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D350
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D32C
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D308
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D2E4
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D2C0
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D2B0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D2A0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D290
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D280
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D270
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 16
    bl fn_8011D24C
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 24
    bl fn_8011D22C
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 24
    bl fn_8011D20C
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 24
    bl fn_8011D1CC
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 16
    bl fn_8011D18C
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 16
    bl fn_8011D14C
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 24
    bl fn_8011D10C
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 16
    bl fn_8011D0CC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x0
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x1
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x2
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x3
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x4
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x5
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x6
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x7
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x8
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x9
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0xa
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0xb
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0xc
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0xd
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0xe
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0xf
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x10
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x11
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x12
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x13
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x14
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x15
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x16
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x17
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x18
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x1a
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x1b
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x1c
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x1d
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x1e
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x1f
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x20
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x21
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x22
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x23
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x24
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x25
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x26
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x27
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x28
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x29
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x2a
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x2b
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x2c
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x2d
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x2e
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x2f
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x30
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x31
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x32
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x33
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x34
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x35
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x36
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x37
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x38
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x39
    bl fn_8011D0AC
    b @L_801263F0
    clrlwi r5, r31, 24
    li r4, 0x3a
    bl fn_8011D0AC
    b @L_801263F0
    mr r4, r30
    mr r5, r31
    bl fn_8011D06C
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 16
    bl fn_8011D02C
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 24
    bl fn_8011CFEC
    b @L_801263F0
    mr r4, r31
    bl fn_8011CFDC
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011CFCC
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011CFBC
    b @L_801263F0
    mr r4, r31
    bl fn_8011CFAC
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011CF9C
    b @L_801263F0
    mr r4, r31
    bl fn_8011CF70
    b @L_801263F0
    mr r4, r31
    bl fn_8011CF44
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011CF34
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011CF24
    b @L_801263F0
    mr r4, r31
    bl fn_8011CF14
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 16
    bl fn_8011CEF0
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 24
    bl fn_8011CED0
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011CEC0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011CEB0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011CEA0
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DFF0
    b @L_801263F0
    mr r4, r31
    bl fn_8011DFE0
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DFD0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011DFC0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011DFB0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011DFA0
    b @L_801263F0
    mr r4, r31
    bl fn_8011DF90
    b @L_801263F0
    mr r4, r31
    bl fn_8011DF54
    b @L_801263F0
    mr r4, r31
    bl fn_8011DEE4
    b @L_801263F0
    mr r4, r31
    bl fn_8011DEA8
    b @L_801263F0
    mr r4, r31
    bl fn_8011DE98
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011DE48
    b @L_801263F0
    li r4, 0x0
    li r5, 0x7c
    li r6, 0x0
    bl fn_8012640C
    cmplwi r3, 0x0
    beq @L_801263F0
    lwz r4, 0x0(r31)
    lwz r0, 0x4(r31)
    stw r4, 0x0(r3)
    stw r0, 0x4(r3)
    lwz r4, 0x8(r31)
    lwz r0, 0xc(r31)
    stw r4, 0x8(r3)
    stw r0, 0xc(r3)
    b @L_801263F0
    mr r4, r31
    bl fn_8011DE38
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 16
    bl fn_8011DDFC
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 24
    bl fn_8011DD80
    b @L_801263F0
    mr r4, r30
    clrlwi r5, r31, 24
    bl fn_8011DCC4
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DCB4
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DC6C
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DC48
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DC24
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DC00
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DBDC
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DBB8
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DB94
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DB60
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DB2C
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DAF8
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DAC4
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DA90
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DA5C
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DA28
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D9F4
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D9C0
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D98C
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D958
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D924
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D904
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D8B4
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D890
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D86C
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D848
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D824
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D760
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D73C
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D718
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D6F4
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D6D0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D6AC
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D688
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D664
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D640
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D61C
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D5F8
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D5D4
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D5B0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D58C
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D800
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D7DC
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D7B8
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D794
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D770
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D57C
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D56C
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D504
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D4F4
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D4E4
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D4D4
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_8011D4C4
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D4A4
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D4B4
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D494
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011D8F4
    b @L_801263F0
    mr r4, r31
    bl fn_8011D8D8
    b @L_801263F0
    mr r4, r31
    bl fn_8011DE88
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_8011DE68
    b @L_801263F0
    mr r4, r31
    bl fn_801FD938
    b @L_801263F0
    li r4, 0x0
    li r5, 0xcd
    li r6, 0x0
    bl fn_8012640C
    cmplwi r3, 0x0
    beq @L_801263F0
    lwz r4, 0x0(r31)
    lwz r0, 0x4(r31)
    stw r4, 0x0(r3)
    stw r0, 0x4(r3)
    lwz r4, 0x8(r31)
    lwz r0, 0xc(r31)
    stw r4, 0x8(r3)
    stw r0, 0xc(r3)
    b @L_801263F0
    extsh r4, r31
    bl fn_801FD928
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD918
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD908
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD8F8
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD8D0
    b @L_801263F0
    mr r4, r31
    bl fn_801FD8C0
    b @L_801263F0
    mr r4, r31
    bl fn_801FD8B0
    b @L_801263F0
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    mr r7, r31
    li r4, 0x0
    li r5, 0x28
    li r6, 0x0
    bl fn_8011BBD8
    b @L_801263F0
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    mr r7, r31
    li r4, 0x0
    li r5, 0x27
    li r6, 0x0
    bl fn_8011BBD8
    b @L_801263F0
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    mr r7, r31
    li r4, 0x0
    li r5, 0x2c
    li r6, 0x0
    bl fn_8011BBD8
    b @L_801263F0
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    mr r7, r31
    li r4, 0x0
    li r5, 0x2b
    li r6, 0x0
    bl fn_8011BBD8
    b @L_801263F0
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r31, 16
    mr r30, r3
    bl fn_8020981C
    clrlwi r0, r3, 24
    cmplwi r0, 0x2
    bne @L_801263F0
    mr r3, r30
    clrlwi r4, r31, 16
    li r5, 0x0
    bl fn_802097C8
    b @L_801263F0
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r4, r31, 16
    mr r30, r3
    beq @L_80126098
    bl fn_8020990C
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801263F0
    mr r3, r30
    clrlwi r4, r31, 16
    bl fn_80209960
    b @L_801263F0
@L_80126098:
    bl fn_80209FAC
    b @L_801263F0
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    mr r7, r31
    li r4, 0x0
    li r5, 0x2d
    li r6, 0x0
    bl fn_8011BBD8
    b @L_801263F0
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    clrlwi r7, r31, 16
    li r4, 0x0
    li r5, 0x2f
    li r6, 0x0
    bl fn_8011BBD8
    b @L_801263F0
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    mr r7, r31
    li r4, 0x0
    li r5, 0x29
    li r6, 0x0
    bl fn_8011BBD8
    b @L_801263F0
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    mr r7, r31
    li r4, 0x0
    li r5, 0x2e
    li r6, 0x0
    bl fn_8011BBD8
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD8A0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD890
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD880
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD870
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD860
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD850
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD840
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FD820
    b @L_801263F0
    mr r4, r31
    bl fn_801FD7F8
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FCF7C
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FCF6C
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FCF5C
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FCF4C
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FCF3C
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FCF2C
    b @L_801263F0
    extsh r4, r31
    bl fn_801FCF1C
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FCF0C
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FCEFC
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD728
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FD718
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FD708
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD6F8
    b @L_801263F0
    extsb r5, r31
    clrlwi r4, r30, 24
    bl fn_801FD660
    b @L_801263F0
    clrlwi r4, r30, 24
    clrlwi r5, r31, 16
    bl fn_801FD5F0
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FD5C8
    b @L_801263F0
    extsh r4, r31
    bl fn_801FD6E8
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FD6D8
    b @L_801263F0
    extsh r4, r31
    bl fn_801FD6C8
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FD6B8
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD330
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD320
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD310
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD300
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD2F0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD2E0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD2D0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD2C0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD2B0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD2A0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD290
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD280
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD270
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD260
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD250
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD240
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD230
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD220
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD210
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD200
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD1F0
    b @L_801263F0
    mr r4, r31
    bl fn_801FD1E0
    b @L_801263F0
    extsh r4, r31
    bl fn_801FD1D0
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FD1C0
    b @L_801263F0
    extsh r4, r31
    bl fn_801FD1B0
    b @L_801263F0
    clrlwi r4, r31, 16
    bl fn_801FD1A0
    b @L_801263F0
    clrlwi r4, r31, 24
    bl fn_801FD178
    b @L_801263F0
    extsh r4, r31
    bl fn_801FD150
@L_801263F0:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
