/* Swap two slot entries at indices idx and arg within the data structure at ptr.
   fn_8011F260 returns a pointer to the slot record for a given index (selector=0).
   fn_8011F5E0 copies one u32 word (*dst = *src), used here for a three-step swap. */
void fn_80123C54(void* ptr, u32 idx, u32 arg) {
    extern u32* fn_8011F260(void* a, u32 b, u32 c);
    extern void fn_8011F5E0(u32* dst, u32* src);
    u32 tmp;
    u32* val1;
    u32* val2;

    if (ptr == NULL) {
        return;
    }

    val1 = fn_8011F260(ptr, idx, 0);
    val2 = fn_8011F260(ptr, arg, 0);

    fn_8011F5E0(&tmp, val1);   /* tmp   = *val1 */
    fn_8011F5E0(val1, val2);   /* *val1 = *val2 */
    fn_8011F5E0(val2, &tmp);   /* *val2 = tmp   */
}
