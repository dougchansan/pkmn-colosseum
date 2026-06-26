    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    cmplwi r3, 0x0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r3
    bne @L_801290C4
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_801290C4
    li r3, 0x0
    b @L_801290C8
@L_801290C4:
    bl fn_80128E04
@L_801290C8:
    bl fn_80135338
    cmplwi r31, 0x0
    mr r3, r31
    bne @L_801290EC
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_801290EC
    li r3, 0x0
    b @L_801290F0
@L_801290EC:
    bl fn_80128DEC
@L_801290F0:
    bl fn_8012A248
    cmplwi r31, 0x0
    mr r3, r31
    bne @L_80129114
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_80129114
    li r3, 0x0
    b @L_80129118
@L_80129114:
    bl fn_80128DD4
@L_80129118:
    bl fn_80134F88
    cmplwi r31, 0x0
    beq @L_8012913C
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_80129134
    li r3, 0x0
@L_80129134:
    cmplw r31, r3
    bne @L_80129154
@L_8012913C:
    li r3, 0x1
    bl GSflagClear
    li r3, 0x2
    bl GSflagClear
    li r3, 0x3
    bl GSflagClear
@L_80129154:
    cmplwi r31, 0x0
    mr r3, r31
    bne @L_80129174
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_80129174
    li r3, 0x0
    b @L_80129178
@L_80129174:
    bl fn_80128D68
@L_80129178:
    bl fn_801D1FA8
    cmplwi r31, 0x0
    mr r3, r31
    bne @L_8012919C
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_8012919C
    li r3, 0x0
    b @L_801291A0
@L_8012919C:
    bl fn_80128D4C
@L_801291A0:
    bl fn_801ED310
    cmplwi r31, 0x0
    mr r3, r31
    bne @L_801291C4
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_801291C4
    li r3, 0x0
    b @L_801291C8
@L_801291C4:
    bl fn_80128CF8
@L_801291C8:
    bl fn_8006B6B4
    cmplwi r31, 0x0
    mr r3, r31
    bne @L_801291EC
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_801291EC
    li r3, 0x0
    b @L_801291F0
@L_801291EC:
    bl fn_80128D30
@L_801291F0:
    bl fn_80260070
    cmplwi r31, 0x0
    mr r3, r31
    bne @L_80129214
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_80129214
    li r3, 0x0
    b @L_80129218
@L_80129214:
    bl fn_80128D14
@L_80129218:
    bl fn_80083CBC
    cmplwi r31, 0x0
    mr r3, r31
    bne @L_8012923C
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_8012923C
    li r3, 0x0
    b @L_80129240
@L_8012923C:
    bl fn_80128CDC
@L_80129240:
    bl fn_801EF128
    cmplwi r31, 0x0
    mr r3, r31
    bne @L_80129264
    bl fn_80128E24
    cmplwi r3, 0x0
    bne @L_80129264
    li r3, 0x0
    b @L_80129268
@L_80129264:
    bl fn_80128CC0
@L_80129268:
    bl fn_80265F4C
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
