    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    clrlwi. r0, r5, 16
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    mr r30, r5
    stw r29, 0x14(r1)
    mr r29, r6
    stw r28, 0x10(r1)
    mr r28, r3
    beq @L_80126444
    cmplwi r0, 0x124
    blt @L_8012644C
@L_80126444:
    li r3, 0x0
    b @L_8012793C
@L_8012644C:
    cmplwi r0, 0x6d
    bge @L_8012646C
    mr r3, r4
    bl fn_8011E778
    mr. r28, r3
    bne @L_8012647C
    li r3, 0x0
    b @L_8012793C
@L_8012646C:
    cmplwi r28, 0x0
    bne @L_8012647C
    li r3, 0x0
    b @L_8012793C
@L_8012647C:
    clrlwi r0, r30, 16
    cmplwi r0, 0x123
    bgt @L_80127938
    lis r3, jumptable_8035E4B0@ha
    slwi r0, r0, 2
    addi r3, r3, jumptable_8035E4B0@l
    lwzx r0, r3, r0
    mtctr r0
    bctr
    mr r3, r28
    bl fn_8011E760
    b @L_8012793C
    mr r3, r28
    bl fn_8011E734
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E708
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E6DC
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E6B0
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E684
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E658
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E62C
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E600
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E5D4
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E5A8
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E57C
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E550
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E538
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E520
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E508
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E4F0
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E4D8
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011E4A4
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011E474
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011E444
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011E3FC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011E3B4
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011E36C
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011E324
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011E2DC
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    li r4, 0x0
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x1
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x2
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x3
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x4
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x5
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x6
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x7
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x8
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x9
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0xa
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0xb
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0xc
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0xd
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0xe
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0xf
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x10
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x11
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x12
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x13
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x14
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x15
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x16
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x17
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x18
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x1a
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x1b
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x1c
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x1d
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x1e
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x1f
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x20
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x21
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x22
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x23
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x24
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x25
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x26
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x27
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x28
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x29
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x2a
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x2b
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x2c
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x2d
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x2e
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x2f
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x30
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x31
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x32
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x33
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x34
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x35
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x36
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x37
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x38
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x39
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x3a
    bl fn_8011E2AC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011E264
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011E21C
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011E1D4
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E1BC
    b @L_8012793C
    mr r3, r28
    bl fn_8011E1A4
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E18C
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E174
    b @L_8012793C
    mr r3, r28
    bl fn_8011E15C
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E128
    b @L_8012793C
    mr r3, r28
    bl fn_8011E0F4
    b @L_8012793C
    mr r3, r28
    bl fn_8011E0DC
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E0C4
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E0AC
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011E078
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011E048
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E030
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E018
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E000
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011F5C8
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011F5B0
    b @L_8012793C
    mr r3, r28
    bl fn_8011F598
    b @L_8012793C
    mr r3, r28
    bl fn_8011F580
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011F568
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011F550
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011F538
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011F520
    b @L_8012793C
    mr r3, r28
    bl fn_8011F508
    b @L_8012793C
    mr r3, r28
    bl fn_8011F4F0
    b @L_8012793C
    mr r3, r28
    bl fn_8011F4D8
    b @L_8012793C
    mr r3, r28
    bl fn_8011F4C0
    b @L_8012793C
    mr r3, r28
    bl fn_8011F4A8
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x0
    li r5, 0x83
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 16
    cntlzw r0, r0
    extrwi r3, r0, 8, 19
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011F474
    b @L_8012793C
    mr r3, r28
    bl fn_8011F45C
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011F228
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011F1F0
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011F1B8
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011F1A0
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011F188
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011F15C
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011F130
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011F104
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011F0D8
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011F0AC
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011F080
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011F054
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011F028
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011EFFC
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011EFD0
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011EFA4
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011EF78
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011EF4C
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011EF20
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011EEF4
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011EEC8
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011EE9C
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011EE70
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011EE58
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011ECEC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011ECC0
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EC94
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EC68
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EC3C
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EB48
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EB1C
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EAF0
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EAC4
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EA98
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EA6C
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EA40
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EA14
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E9E8
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E9BC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E990
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E964
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E938
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E90C
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EC10
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EBE4
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EBB8
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EB8C
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011EB60
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E8F4
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E8DC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E868
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E850
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E838
    clrlwi r3, r3, 24
    b @L_8012793C
    cmplwi r28, 0x0
    bne @L_8012705C
    li r0, 0x2
    b @L_80127158
@L_8012705C:
    mr r3, r28
    li r4, 0x0
    li r5, 0x6f
    li r6, 0x0
    bl fn_8012640C
    mr r31, r3
    mr r3, r28
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x13
    li r6, 0x0
    bl fn_8012640C
    cmplwi r28, 0x0
    clrlwi r30, r3, 16
    bne @L_801270B0
    li r3, 0x2
    b @L_80127134
@L_801270B0:
    mr r3, r28
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x13
    li r6, 0x0
    bl fn_8012640C
    clrlwi r29, r3, 16
    li r3, 0x0
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r29, r0
    bne @L_801270F8
    li r3, 0x0
    b @L_80127134
@L_801270F8:
    li r3, 0x1
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r29, r0
    bne @L_80127114
    li r3, 0x1
    b @L_80127134
@L_80127114:
    li r3, 0x2
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r29, r0
    bne @L_80127130
    li r3, 0x2
    b @L_80127134
@L_80127130:
    li r3, -0x1
@L_80127134:
    extsb. r0, r3
    bge @L_80127154
    clrlwi r0, r31, 24
    cmplw r30, r0
    ble @L_80127150
    li r3, 0x1
    b @L_80127154
@L_80127150:
    li r3, 0x0
@L_80127154:
    clrlwi r0, r3, 24
@L_80127158:
    clrlwi r3, r0, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E820
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E808
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E7D8
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011E7F0
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    li r4, 0x0
    li r5, 0x6f
    li r6, 0x0
    bl fn_8012640C
    lis r4, 0x51ec
    subi r0, r4, 0x7ae1
    mulhwu r0, r0, r3
    srwi r0, r0, 3
    mulli r0, r0, 0x19
    subf r0, r0, r3
    clrlwi r3, r0, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x11
    li r6, 0x0
    bl fn_8012640C
    clrlwi r30, r3, 24
    mr r3, r28
    li r4, 0x0
    li r5, 0x79
    li r6, 0x0
    bl fn_8012640C
    mr r31, r3
    mr r3, r30
    bl fn_8011CE74
    mr. r30, r3
    bne @L_80127230
    li r0, 0x0
    b @L_8012725C
@L_80127230:
    li r29, 0x1
@L_80127234:
    mr r3, r30
    clrlwi r4, r29, 24
    bl fn_8011CE44
    cmplw r3, r31
    bgt @L_80127254
    addi r29, r29, 0x1
    cmpwi r29, 0x65
    blt @L_80127234
@L_80127254:
    subi r0, r29, 0x1
    clrlwi r0, r0, 24
@L_8012725C:
    clrlwi r3, r0, 24
    b @L_8012793C
    cmplwi r28, 0x0
    bne @L_80127274
    li r0, 0x0
    b @L_801272D0
@L_80127274:
    mr r3, r28
    li r4, 0x0
    li r5, 0x75
    li r6, 0x0
    bl fn_8012640C
    mr r31, r3
    mr r3, r28
    li r4, 0x0
    li r5, 0x6f
    li r6, 0x0
    bl fn_8012640C
    srwi r4, r31, 16
    clrlwi r0, r31, 16
    srwi r5, r3, 16
    clrlwi r6, r3, 16
    xor r0, r4, r0
    li r3, 0x8
    xor r0, r5, r0
    xor r0, r6, r0
    xor r0, r3, r0
    cntlzw r0, r0
    slw r0, r3, r0
    srwi r0, r0, 31
@L_801272D0:
    clrlwi r3, r0, 24
    b @L_8012793C
    mr r3, r28
    bl fn_8011E7C0
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011EE40
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011ED18
    b @L_8012793C
    mr r3, r28
    bl fn_8011EE28
    b @L_8012793C
    mr r3, r28
    bl fn_8011EE10
    b @L_8012793C
    mr r3, r28
    bl fn_8011EDF8
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_8011ED68
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_8011EDC4
    b @L_8012793C
    mr r3, r28
    bl fn_801FDB60
    b @L_8012793C
    mr r3, r28
    bl fn_801FDB48
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_801FDB14
    b @L_8012793C
    mr r3, r28
    bl fn_801FDAFC
    extsh r3, r3
    b @L_8012793C
    mr r3, r28
    bl fn_801FDAE4
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FDACC
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FDAB4
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD8E0
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FDA9C
    b @L_8012793C
    mr r3, r28
    bl fn_801FDA84
    b @L_8012793C
    mr r3, r28
    bl fn_801FDA6C
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_801FDA38
    b @L_8012793C
    mr r3, r28
    bl fn_801FDA20
    b @L_8012793C
    mr r3, r28
    bl fn_80205184
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_80205224
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    li r4, 0x0
    li r5, 0x2c
    li r6, 0x0
    bl fn_8011BEB4
    b @L_8012793C
    mr r3, r28
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    li r4, 0x0
    li r5, 0x2b
    li r6, 0x0
    bl fn_8011BEB4
    b @L_8012793C
    mr r3, r28
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    mr r4, r29
    bl fn_8020990C
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    bl fn_802096E8
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    li r4, 0x0
    li r5, 0x2d
    li r6, 0x0
    bl fn_8011BEB4
    b @L_8012793C
    mr r3, r28
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    li r4, 0x0
    li r5, 0x2f
    li r6, 0x0
    bl fn_8011BEB4
    b @L_8012793C
    mr r3, r28
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    li r4, 0x0
    li r5, 0x29
    li r6, 0x0
    bl fn_8011BEB4
    b @L_8012793C
    mr r3, r28
    li r4, 0x0
    li r5, 0xd9
    li r6, 0x0
    bl fn_8012640C
    li r4, 0x0
    li r5, 0x2e
    li r6, 0x0
    bl fn_8011BEB4
    b @L_8012793C
    mr r3, r28
    bl fn_801FDA08
    b @L_8012793C
    mr r3, r28
    bl fn_801FD9F0
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD9D8
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD9C0
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD9A8
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD990
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD978
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD960
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD948
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FD808
    b @L_8012793C
    mr r3, r28
    bl fn_801FD064
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FD04C
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FD034
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FD01C
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FD004
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FCFEC
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FCFD4
    extsh r3, r3
    b @L_8012793C
    mr r3, r28
    bl fn_801FCFBC
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FCFA4
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FCF8C
    b @L_8012793C
    mr r3, r28
    bl fn_801FD7E0
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD7C8
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FD7B0
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FD798
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    clrlwi r4, r29, 24
    bl fn_801FD684
    extsh r3, r3
    b @L_8012793C
    mr r3, r28
    bl fn_801FD648
    b @L_8012793C
    mr r3, r28
    clrlwi r4, r29, 24
    bl fn_801FD614
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FD5D8
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FD5B0
    b @L_8012793C
    mr r3, r28
    bl fn_801FD780
    extsh r3, r3
    b @L_8012793C
    mr r3, r28
    bl fn_801FD768
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FD750
    extsh r3, r3
    b @L_8012793C
    mr r3, r28
    bl fn_801FD738
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FD598
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD580
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD568
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD550
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD538
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD520
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD508
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD4F0
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD4D8
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD4C0
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD4A8
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD490
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD478
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD460
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD448
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD430
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD418
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD400
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD3E8
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD3D0
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD3B8
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD3A0
    b @L_8012793C
    mr r3, r28
    bl fn_801FD388
    extsh r3, r3
    b @L_8012793C
    mr r3, r28
    bl fn_801FD370
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FD358
    extsh r3, r3
    b @L_8012793C
    mr r3, r28
    bl fn_801FD340
    clrlwi r3, r3, 16
    b @L_8012793C
    mr r3, r28
    bl fn_801FD188
    clrlwi r3, r3, 24
    b @L_8012793C
    mr r3, r28
    bl fn_801FD160
    extsh r3, r3
    b @L_8012793C
    mr r3, r28
    mr r4, r29
    bl fn_801FD11C
    b @L_8012793C
    mr r3, r28
    bl fn_802062FC
    clrlwi r3, r3, 24
    b @L_8012793C
@L_80127938:
    li r3, 0x0
@L_8012793C:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
