Final C body

```c
#pragma optimization_level 4
#pragma scheduling off
void fn_800259B0(void) {
    u32 result;

    lbl_8047A368 = 0;
    lbl_8047A36C = 0;
    lbl_8047A370 = 0;
    lbl_80478898 = lbl_8047B8A8;
    *(f32*)(&lbl_803A2058[0]) = lbl_8047B8AC;
    *(f32*)(&lbl_803A2058[4]) = lbl_8047B8AC;
    *(f32*)(&lbl_803A2058[8]) = lbl_8047B8AC;
    *(f32*)(&lbl_803A204C[0]) = lbl_8047B8AC;
    *(f32*)(&lbl_803A204C[4]) = lbl_8047B8AC;
    *(f32*)(&lbl_803A204C[8]) = lbl_8047B8AC;
    lbl_8047A374 = lbl_8047B8AC;
    lbl_8047A378 = lbl_8047B8E8;
    lbl_8047A37C = lbl_8047B8EC;
    lbl_8047A380 = 0;
    lbl_8047A384 = 0;
    lbl_8047A388 = 0;
    lbl_8047A390 = 0;
    fn_801CB954(0x0C6A1000, 0);
    fn_801CB954(0x0C6A1001, 0);
    fn_801CB954(0x0C6A1002, 0);
    result = fn_8011394C();
    lbl_8047A3AC = result;
    if (result == 0x7B) {
        lbl_8047A3AC = 0x66;
    }
}
```

What was wrong

- The active C uses `#pragma peephole off`, which is not suggested by the target asm and can perturb this small init function unnecessarily.
- The `vec1` / `vec2` pointer locals are semantically fine, but they change the store shape away from the direct `stfs` sequence the asm uses for `lbl_803A2058` and `lbl_803A204C`.
- This function is mostly about exact SDA/global store order. A direct typed-write version is the right shape for CW here.

Required pragmas

```c
#pragma optimization_level 4
#pragma scheduling off
```
