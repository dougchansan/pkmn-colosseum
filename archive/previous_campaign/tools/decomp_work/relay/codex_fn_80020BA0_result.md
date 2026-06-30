#pragma scheduling on
#pragma optimization_level 4
void fn_80020BA0(void* arg0, u8* arg1) {
    s16* ptr;

    if (fn_80166C74() == 0) {
        ptr = (s16*)(lbl_802EF0A8 + 0x8FF0);
    } else {
        ptr = (s16*)(lbl_802EF0A8 + 0x900C);
    }
    fn_801040F0((s16)(ptr[1] - *(s16*)(arg1 + 0x50)), (s16)(ptr[2] - *(s16*)(arg1 + 0x52)), arg0, 0x192, 0);

    if (fn_80135168(0, 9) == 1) {
        ptr = (s16*)(lbl_802EF0A8 + 0x8FD4);
    } else {
        ptr = (s16*)(lbl_802EF0A8 + 0x8FB8);
    }
    fn_801040F0((s16)(ptr[1] - *(s16*)(arg1 + 0x50)), (s16)(ptr[2] - *(s16*)(arg1 + 0x52)), arg0, 0x192, 0);
}

What was wrong:
- The current C is semantically close, but it is shaped unlike the asm: extra locals (`base`, `flag`, `objx`, `objy`, `tblx`, `tbly`) encourage different register allocation and spills.
- The asm branches directly on `fn_80166C74()` and `fn_80135168(0, 9)`, then uses a single table pointer for each call to `fn_801040F0`.
- Express the table as an `s16*` and read `ptr[1]` / `ptr[2]`; that matches the `lha 0x2(r9)` and `lha 0x4(r9)` access pattern better than byte-pointer temporaries.
- Use `#pragma scheduling on` with `#pragma optimization_level 4`; this function is small and scheduling affects compare/branch placement.
