    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr r31, r3
    bl fn_80105624
    cmplwi r31, 0x0
    beq @L_80024368
    lwz r0, lbl_8047A370@sda21(r0)
    cmpwi r0, 0x1
    beq @L_8002434C
    lhz r0, 0x4(r3)
    rlwinm r0, r0, 0, 27, 27
    cmpwi r0, 0x0
    beq @L_8002434C
    li r0, 0x1
    stb r0, 0x98(r31)
@L_8002434C:
    lhz r0, 0x4(r3)
    rlwinm r0, r0, 0, 26, 26
    cmpwi r0, 0x0
    beq @L_80024368
    li r0, 0x1
    stb r0, 0x98(r31)
    stb r0, 0x99(r31)
@L_80024368:
    bl fn_800D3088
    lis r0, 0x4330
    stw r3, 0xc(r1)
    lfd f2, lbl_8047B8D0@sda21(r0)
    stw r0, 0x8(r1)
    lfs f3, lbl_8047A378@sda21(r0)
    lfd f1, 0x8(r1)
    lfs f0, lbl_8047A374@sda21(r0)
    fsubs f2, f1, f2
    lfs f1, lbl_8047B8C0@sda21(r0)
    fmadds f0, f3, f2, f0
    fcmpo cr0, f0, f1
    stfs f0, lbl_8047A374@sda21(r0)
    bge @L_800243B0
    fneg f0, f3
    stfs f1, lbl_8047A374@sda21(r0)
    stfs f0, lbl_8047A378@sda21(r0)
    b @L_800243C8
@L_800243B0:
    lfs f1, lbl_8047B8C4@sda21(r0)
    fcmpo cr0, f0, f1
    ble @L_800243C8
    fneg f0, f3
    stfs f1, lbl_8047A374@sda21(r0)
    stfs f0, lbl_8047A378@sda21(r0)
@L_800243C8:
    lwz r0, lbl_8047A370@sda21(r0)
    cmpwi r0, 0x1
    bne @L_80024420
    bl fn_800D3088
    lis r0, 0x4330
    stw r3, 0xc(r1)
    lfd f3, lbl_8047B8D0@sda21(r0)
    stw r0, 0x8(r1)
    lfs f2, lbl_8047B8C8@sda21(r0)
    lfd f0, 0x8(r1)
    lfs f1, lbl_80478898@sda21(r0)
    fsubs f3, f0, f3
    lfs f0, lbl_8047B8AC@sda21(r0)
    fnmsubs f1, f2, f3, f1
    fcmpo cr0, f1, f0
    stfs f1, lbl_80478898@sda21(r0)
    bge @L_80024420
    lwz r0, lbl_8047A36C@sda21(r0)
    li r3, 0x0
    stfs f0, lbl_80478898@sda21(r0)
    stw r3, lbl_8047A370@sda21(r0)
    stw r0, lbl_8047A368@sda21(r0)
@L_80024420:
    li r3, 0x0
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

