/* gs_field_world slot-accessor: given a slot-object pointer (or NULL to auto-resolve)
   and a selector 0x00-0x10, return the corresponding sub-pointer or constant.
   Selector >= 0x11 returns 0 unconditionally. */
u32 fn_80129280(u8* arg1, u16 arg2)
{
    extern u8* fn_80128E24(void);
    extern u32 fn_80128E04(u8* ptr);
    extern u32 fn_80128DEC(u8* ptr);
    extern u32 fn_80128DD4(u8* ptr);
    extern u32 fn_80128DB8(u8* ptr);
    extern u32 fn_80128D9C(u8* ptr);
    extern u32 fn_80128D80(u8* ptr);
    extern u32 fn_80128D68(u8* ptr);
    extern u32 fn_80128D4C(u8* ptr);
    extern u32 fn_80128D30(u8* ptr);
    extern u32 fn_80128D14(u8* ptr);
    extern u32 fn_80128CF8(u8* ptr);
    extern u32 fn_80128CDC(u8* ptr);
    extern u32 fn_80128CC0(u8* ptr);

    if ((u32)arg2 >= 0x11) {
        return 0;
    }
    if (arg1 == NULL) {
        arg1 = fn_80128E24();
        if (arg1 == NULL) {
            return 0;
        }
    }
    switch (arg2) {
        case 0x0:  return (u32)arg1;
        case 0x1:  return fn_80128E04(arg1);
        case 0x2:  return fn_80128DEC(arg1);
        case 0x3:  return fn_80128DD4(arg1);
        case 0x4:  return fn_80128DB8(arg1);
        case 0x5:  return fn_80128D9C(arg1);
        case 0x6:  return fn_80128D80(arg1);
        case 0x7:  return 0x8;
        case 0x8:  return 0x20;
        case 0x9:  return 0x180;
        case 0xa:  return fn_80128D68(arg1);
        case 0xb:  return fn_80128D4C(arg1);
        case 0xc:  return fn_80128D30(arg1);
        case 0xd:  return fn_80128D14(arg1);
        case 0xe:  return fn_80128CF8(arg1);
        case 0xf:  return fn_80128CDC(arg1);
        case 0x10: return fn_80128CC0(arg1);
        default:   return 0;
    }
}
