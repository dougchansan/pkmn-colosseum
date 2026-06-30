/* GSfield_ObjectBatchUpdate (0x8012CA84 region; twin of fn_80124A60 @0x80124A60)
 *
 * Resets/initialises a field object's full property block to default
 * values via the property-setter dispatcher (fn_801254B4) and a few
 * getters (fn_8012640C). `obj` is the field-object handle (PowerPC r30).
 *
 * The pseudo-register entry guard `r0=0; if((s32)r0!=0)` is the asm
 * lowering of an inverted NULL check on the object pointer: the whole
 * body runs only when obj != NULL. `local` is the u16 stack scratch
 * (sp+0x8) whose address is handed to selectors 0x76/0x77.
 */
void GSfield_ObjectBatchUpdate(u8* obj) {
    extern void fn_801254B4(u8* obj, u32 p, u16 sel, u32 a6, u32 v);
    extern u32  fn_8012640C(u8* obj, u32 id, u32 selector, u32 d);
    extern void fn_8011B950(u8* base, u16 count);
    extern void fn_80135708(void* ptr);
    extern void fn_8011D480(u8* ptr, u8 val);

    u16 local = 0;
    s32 i;

    if (obj == NULL) {
        return;
    }

    fn_801254B4(obj, 0, 0xc9, 0, 0);
    fn_801254B4(obj, 0, 0xc3, 0, 0);
    if (obj != NULL) {
        fn_801254B4(obj, 0, 0xc5, 0, (u32)(s32)-0x64);
    }
    fn_801254B4(obj, 0, 0xc6, 0, 0);
    fn_801254B4(obj, 0, 0xc7, 0, 0);
    fn_8011B950((u8*)fn_8012640C(obj, 0, 0xc8, 0), 1);

    fn_801254B4(obj, 0, 0x6e, 0, 0);
    fn_801254B4(obj, 0, 0x6f, 0, 0);
    fn_80135708((void*)fn_8012640C(obj, 0, 0x70, 0));

    if (obj != NULL) {
        fn_801254B4(obj, 0, 0x71, 0, 0);
        fn_801254B4(obj, 0, 0x72, 0, 0);
        fn_801254B4(obj, 0, 0x73, 0, 0);
        fn_801254B4(obj, 0, 0x74, 0, 2);
        fn_801254B4(obj, 0, 0x75, 0, 0);
        fn_801254B4(obj, 0, 0x76, 0, (u32)&local);
    }

    fn_801254B4(obj, 0, 0x77, 0, (u32)&local);
    fn_801254B4(obj, 0, 0x79, 0, 0);
    fn_801254B4(obj, 0, 0x7a, 0, 0);
    fn_8011B950((u8*)fn_8012640C(obj, 0, 0x7c, 0), 1);
    fn_801254B4(obj, 0, 0x7d, 0, 0);

    if (obj != NULL) {
        for (i = 0; (u16)i < 4; i++) {
            if (obj != NULL) {
                fn_801254B4(obj, 0, 0x7f, (u32)i, 0);
                fn_801254B4(obj, 0, 0x80, (u32)i, 0);
                fn_801254B4(obj, 0, 0x81, (u32)i, 0);
            }
        }
    }

    if (obj != NULL) {
        fn_8012640C(obj, 0, 0x82, 0);
        fn_801254B4(obj, 0, 0x82, 0, 0);
    }

    fn_801254B4(obj, 0, 0x83, 0, 0);
    fn_801254B4(obj, 0, 0x87, 0, 0);
    fn_801254B4(obj, 0, 0x88, 0, 0);
    fn_801254B4(obj, 0, 0x89, 0, 0);
    fn_801254B4(obj, 0, 0x8a, 0, 0);
    fn_801254B4(obj, 0, 0x8b, 0, 0);
    fn_801254B4(obj, 0, 0x8c, 0, 0);
    fn_801254B4(obj, 0, 0x8d, 0, 0);
    fn_801254B4(obj, 0, 0x8e, 0, 0);
    fn_801254B4(obj, 0, 0x8f, 0, 0);
    fn_801254B4(obj, 0, 0x90, 0, 0);
    fn_801254B4(obj, 0, 0x91, 0, 0);
    fn_801254B4(obj, 0, 0x92, 0, 0);
    fn_801254B4(obj, 0, 0x93, 0, 0);
    fn_801254B4(obj, 0, 0x94, 0, 0);
    fn_801254B4(obj, 0, 0x95, 0, 0);
    fn_801254B4(obj, 0, 0x96, 0, 0);
    fn_801254B4(obj, 0, 0x97, 0, 0);
    fn_801254B4(obj, 0, 0x98, 0, 0);
    fn_801254B4(obj, 0, 0x99, 0, 0);
    fn_801254B4(obj, 0, 0x9c, 0, 0);
    fn_801254B4(obj, 0, 0x9d, 0, 0);
    fn_801254B4(obj, 0, 0x9e, 0, 0);
    fn_801254B4(obj, 0, 0x9f, 0, 0);
    fn_801254B4(obj, 0, 0xa0, 0, 0);
    fn_801254B4(obj, 0, 0xa1, 0, 0);
    fn_801254B4(obj, 0, 0xa3, 0, 0);
    fn_801254B4(obj, 0, 0xa4, 0, 0);
    fn_801254B4(obj, 0, 0xa5, 0, 0);
    fn_801254B4(obj, 0, 0xa6, 0, 0);
    fn_801254B4(obj, 0, 0xa7, 0, 0);
    fn_801254B4(obj, 0, 0xa8, 0, 0);
    fn_801254B4(obj, 0, 0xa9, 0, 0);
    fn_801254B4(obj, 0, 0xaa, 0, 0);
    fn_801254B4(obj, 0, 0xab, 0, 0);
    fn_801254B4(obj, 0, 0xac, 0, 0);
    fn_801254B4(obj, 0, 0xad, 0, 0);
    fn_801254B4(obj, 0, 0xae, 0, 0);
    fn_801254B4(obj, 0, 0xaf, 0, 0);
    fn_801254B4(obj, 0, 0xb0, 0, 0);
    fn_801254B4(obj, 0, 0xb1, 0, 0);
    fn_801254B4(obj, 0, 0xb2, 0, 0);
    fn_801254B4(obj, 0, 0xb3, 0, 0);
    fn_801254B4(obj, 0, 0xb4, 0, 0);
    fn_801254B4(obj, 0, 0xb5, 0, 0);
    fn_801254B4(obj, 0, 0xb6, 0, 0);
    fn_801254B4(obj, 0, 0xb7, 0, 0);
    fn_801254B4(obj, 0, 0xb8, 0, 0);
    fn_801254B4(obj, 0, 0xb9, 0, 0);
    fn_801254B4(obj, 0, 0xbb, 0, 0);
    fn_801254B4(obj, 0, 0xbc, 0, (u32)0xff);
    fn_801254B4(obj, 0, 0xbd, 0, 0);
    fn_801254B4(obj, 0, 0xbe, 0, 0);

    fn_8011D480(obj, 0);
}
