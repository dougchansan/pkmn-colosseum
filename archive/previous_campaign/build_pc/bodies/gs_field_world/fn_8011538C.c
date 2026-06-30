/* fn_8011538C - indexed slot accessor: walks ptr->+0x10->*->array[idx]+8
 * ptr  : container object
 * idx  : slot index (0..7); returns NULL for out-of-range or any NULL link */
void* fn_8011538C(void* ptr, u32 idx) {
    extern void fn_800DD970(const char* fmt, void* tag, ...);
    extern const char lbl_80272608[];
    extern const char lbl_8027262C[];
    extern u8 lbl_8035BB30[];

    void* p1;

    if (ptr == NULL) {
        fn_800DD970(lbl_80272608, lbl_8035BB30);
        return NULL;
    }
    p1 = *(void**)((u8*)ptr + 0x10);
    if (p1 == NULL) { return NULL; }
    if (idx >= 8) { return NULL; }
    p1 = *(void**)p1;
    if (p1 == NULL) {
        fn_800DD970(lbl_8027262C, lbl_8035BB30);
        return NULL;
    }
    /* advance byte-pointer by idx*4 then read the void* at +8 within that slot */
    return *(void**)((u8*)p1 + idx * 4 + 8);
}
