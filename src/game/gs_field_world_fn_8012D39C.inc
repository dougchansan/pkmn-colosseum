    nofralloc
    stwu r1, -0x20(r1)
    lfs f2, 0x8(r4)
    lfs f0, 0x8(r3)
    lfs f5, 0x0(r4)
    fsubs f6, f2, f0
    lfs f2, 0x0(r3)
    lfs f0, -26104(r2)
    fsubs f8, f5, f2
    fmuls f2, f6, f6
    fmadds f5, f8, f8, f2
    fcmpo cr0, f5, f0
    bge @L_8012D3D4
    li r0, 0x0
    b @L_8012D4E0
@L_8012D3D4:
    lfs f0, -26216(r2)
    fcmpo cr0, f5, f0
    ble @L_8012D428
    frsqrte f2, f5
    lfd f4, -26200(r2)
    lfd f3, -26192(r2)
    fmul f0, f2, f2
    fmul f2, f4, f2
    fnmsub f0, f5, f0, f3
    fmul f2, f2, f0
    fmul f0, f2, f2
    fmul f2, f4, f2
    fnmsub f0, f5, f0, f3
    fmul f2, f2, f0
    fmul f0, f2, f2
    fmul f2, f4, f2
    fnmsub f0, f5, f0, f3
    fmul f0, f2, f0
    fmul f5, f5, f0
    frsp f5, f5
    b @L_8012D4AC
@L_8012D428:
    lfd f0, -26184(r2)
    fcmpo cr0, f5, f0
    bge @L_8012D440
    lis r8, lbl_80478AC0@ha
    lfs f5, lbl_80478AC0@l(r8)
    b @L_8012D4AC
@L_8012D440:
    stfs f5, 0x8(r1)
    lis r0, 0x7f80
    lwz r9, 0x8(r1)
    rlwinm r8, r9, 0, 1, 8
    cmpw r8, r0
    beq @L_8012D468
    bge @L_8012D498
    cmpwi r8, 0x0
    beq @L_8012D480
    b @L_8012D498
@L_8012D468:
    clrlwi. r0, r9, 9
    beq @L_8012D478
    li r0, 0x1
    b @L_8012D49C
@L_8012D478:
    li r0, 0x2
    b @L_8012D49C
@L_8012D480:
    clrlwi. r0, r9, 9
    beq @L_8012D490
    li r0, 0x5
    b @L_8012D49C
@L_8012D490:
    li r0, 0x3
    b @L_8012D49C
@L_8012D498:
    li r0, 0x4
@L_8012D49C:
    cmpwi r0, 0x1
    bne @L_8012D4AC
    lis r8, lbl_80478AC0@ha
    lfs f5, lbl_80478AC0@l(r8)
@L_8012D4AC:
    lfs f0, -26144(r2)
    fneg f3, f6
    lfs f2, 0x0(r4)
    li r0, 0x1
    fdivs f7, f0, f5
    lfs f0, 0x8(r3)
    lfs f6, 0x0(r3)
    lfs f5, 0x8(r4)
    fmuls f0, f2, f0
    fmuls f3, f3, f7
    fmuls f4, f8, f7
    fmsubs f0, f6, f5, f0
    fmuls f7, f7, f0
@L_8012D4E0:
    cmpwi r0, 0x0
    bne @L_8012D4F0
    li r3, -0x1
    b @L_8012D7E8
@L_8012D4F0:
    fmuls f5, f4, f4
    lfs f6, -26144(r2)
    fneg f2, f7
    lfs f0, -26216(r2)
    fmadds f5, f3, f3, f5
    fdivs f9, f6, f5
    fmuls f2, f2, f9
    fcmpo cr0, f9, f0
    fmuls f0, f3, f2
    fmuls f2, f4, f2
    ble @L_8012D564
    frsqrte f6, f9
    lfd f8, -26200(r2)
    lfd f7, -26192(r2)
    fmul f5, f6, f6
    fmul f6, f8, f6
    fnmsub f5, f9, f5, f7
    fmul f6, f6, f5
    fmul f5, f6, f6
    fmul f6, f8, f6
    fnmsub f5, f9, f5, f7
    fmul f6, f6, f5
    fmul f5, f6, f6
    fmul f6, f8, f6
    fnmsub f5, f9, f5, f7
    fmul f5, f6, f5
    fmul f6, f9, f5
    frsp f6, f6
    b @L_8012D5F0
@L_8012D564:
    lfd f5, -26184(r2)
    fcmpo cr0, f9, f5
    bge @L_8012D57C
    lis r3, lbl_80478AC0@ha
    lfs f6, lbl_80478AC0@l(r3)
    b @L_8012D5F0
@L_8012D57C:
    stfs f9, 0x10(r1)
    lis r0, 0x7f80
    lwz r4, 0x10(r1)
    rlwinm r3, r4, 0, 1, 8
    cmpw r3, r0
    beq @L_8012D5A4
    bge @L_8012D5D4
    cmpwi r3, 0x0
    beq @L_8012D5BC
    b @L_8012D5D4
@L_8012D5A4:
    clrlwi. r0, r4, 9
    beq @L_8012D5B4
    li r0, 0x1
    b @L_8012D5D8
@L_8012D5B4:
    li r0, 0x2
    b @L_8012D5D8
@L_8012D5BC:
    clrlwi. r0, r4, 9
    beq @L_8012D5CC
    li r0, 0x5
    b @L_8012D5D8
@L_8012D5CC:
    li r0, 0x3
    b @L_8012D5D8
@L_8012D5D4:
    li r0, 0x4
@L_8012D5D8:
    cmpwi r0, 0x1
    bne @L_8012D5EC
    lis r3, lbl_80478AC0@ha
    lfs f6, lbl_80478AC0@l(r3)
    b @L_8012D5F0
@L_8012D5EC:
    fmr f6, f9
@L_8012D5F0:
    fneg f5, f3
    lfs f9, -26104(r2)
    fmuls f3, f4, f6
    fmuls f4, f5, f6
    fmuls f6, f3, f3
    fmuls f5, f4, f4
    fadds f5, f6, f5
    fcmpo cr0, f5, f9
    bge @L_8012D61C
    li r3, 0x0
    b @L_8012D7E8
@L_8012D61C:
    lfs f8, 0x0(r5)
    fmuls f7, f1, f1
    lfs f6, 0x8(r5)
    fneg f1, f9
    fsubs f8, f8, f0
    fsubs f11, f6, f2
    fmuls f6, f4, f8
    fmsubs f6, f3, f11, f6
    fmuls f6, f6, f6
    fmsubs f10, f5, f7, f6
    fcmpo cr0, f10, f1
    bge @L_8012D654
    li r3, 0x0
    b @L_8012D7E8
@L_8012D654:
    fmuls f1, f4, f11
    fcmpo cr0, f10, f9
    fmadds f9, f3, f8, f1
    bge @L_8012D688
    fdivs f5, f9, f5
    lfs f1, -26216(r2)
    li r3, 0x1
    fmadds f3, f3, f5, f0
    fmadds f0, f4, f5, f2
    stfs f3, 0x0(r7)
    stfs f1, 0x4(r7)
    stfs f0, 0x8(r7)
    b @L_8012D7E8
@L_8012D688:
    lfs f1, -26216(r2)
    fcmpo cr0, f10, f1
    ble @L_8012D6DC
    frsqrte f6, f10
    lfd f8, -26200(r2)
    lfd f7, -26192(r2)
    fmul f1, f6, f6
    fmul f6, f8, f6
    fnmsub f1, f10, f1, f7
    fmul f6, f6, f1
    fmul f1, f6, f6
    fmul f6, f8, f6
    fnmsub f1, f10, f1, f7
    fmul f6, f6, f1
    fmul f1, f6, f6
    fmul f6, f8, f6
    fnmsub f1, f10, f1, f7
    fmul f1, f6, f1
    fmul f10, f10, f1
    frsp f10, f10
    b @L_8012D760
@L_8012D6DC:
    lfd f1, -26184(r2)
    fcmpo cr0, f10, f1
    bge @L_8012D6F4
    lis r3, lbl_80478AC0@ha
    lfs f10, lbl_80478AC0@l(r3)
    b @L_8012D760
@L_8012D6F4:
    stfs f10, 0xc(r1)
    lis r0, 0x7f80
    lwz r4, 0xc(r1)
    rlwinm r3, r4, 0, 1, 8
    cmpw r3, r0
    beq @L_8012D71C
    bge @L_8012D74C
    cmpwi r3, 0x0
    beq @L_8012D734
    b @L_8012D74C
@L_8012D71C:
    clrlwi. r0, r4, 9
    beq @L_8012D72C
    li r0, 0x1
    b @L_8012D750
@L_8012D72C:
    li r0, 0x2
    b @L_8012D750
@L_8012D734:
    clrlwi. r0, r4, 9
    beq @L_8012D744
    li r0, 0x5
    b @L_8012D750
@L_8012D744:
    li r0, 0x3
    b @L_8012D750
@L_8012D74C:
    li r0, 0x4
@L_8012D750:
    cmpwi r0, 0x1
    bne @L_8012D760
    lis r3, lbl_80478AC0@ha
    lfs f10, lbl_80478AC0@l(r3)
@L_8012D760:
    lfs f7, -26144(r2)
    fsubs f6, f9, f10
    fadds f1, f9, f10
    lfs f8, 0x8(r6)
    fdivs f7, f7, f5
    lfs f5, 0x0(r6)
    fmuls f6, f7, f6
    fmuls f1, f7, f1
    fmadds f7, f3, f6, f0
    fmadds f6, f4, f6, f2
    fmadds f9, f3, f1, f0
    fmadds f4, f4, f1, f2
    fsubs f3, f5, f7
    fsubs f2, f8, f6
    fsubs f1, f5, f9
    fsubs f0, f8, f4
    fmuls f3, f3, f3
    fmuls f2, f2, f2
    fmuls f1, f1, f1
    fmuls f0, f0, f0
    fadds f2, f3, f2
    fadds f0, f1, f0
    fcmpo cr0, f2, f0
    bge @L_8012D7D4
    stfs f7, 0x0(r7)
    lfs f0, -26216(r2)
    stfs f0, 0x4(r7)
    stfs f6, 0x8(r7)
    b @L_8012D7E4
@L_8012D7D4:
    stfs f9, 0x0(r7)
    lfs f0, -26216(r2)
    stfs f0, 0x4(r7)
    stfs f4, 0x8(r7)
@L_8012D7E4:
    li r3, 0x2
@L_8012D7E8:
    addi r1, r1, 0x20
    blr
