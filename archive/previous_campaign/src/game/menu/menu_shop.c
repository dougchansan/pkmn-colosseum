/**
 * @file menu_shop.c
 * @brief Shop/item purchase and sale menu system.
 *
 * Implements the in-game shop interface where the player can buy and sell
 * items. Manages the shop inventory display, price calculations, purchase
 * confirmation, and item transfer to/from the player's bag.
 *
 * Key behaviors:
 *   - Uses BSS lbl_803A6748 (0xA0 bytes) as the shop state structure,
 *     containing current item selection, scroll position, and price data
 *   - fn_8003AEF0 loads item/price data by reading from a table at
 *     lbl_8047A4D4 (sdata) and calling battle system accessors
 *     (fn_801EED88, fn_801EE544, fn_801EEF40) for item info
 *   - fn_8003AFDC contains a large switch statement dispatching on item
 *     message IDs (0x308, 0x30B, 0xD96, etc.) for shop event handling
 *   - fn_8003B85C (0x6F8 bytes) is the main shop update loop
 *   - fn_8003C2B8 (0x470 bytes) handles the buy/sell confirmation dialog
 *   - fn_8003C7C0 (0x65C bytes) processes the transaction and updates
 *     the player's wallet (also references lbl_803A6610)
 *   - fn_8003CE1C is an internal helper called by fn_8003B2D8 and fn_8003C2B8
 *   - Sound effects triggered through fn_800FB680
 *   - Effect/sprite rendering via fn_80132A38
 *
 * BSS usage:
 *   - lbl_803A6748 (0xA0 bytes): Shop state structure
 *   - lbl_803A6610 (0x138 bytes): Referenced by shop transaction (fn_8003C7C0)
 *
 * Address range: 0x8003AEF0 - 0x8003D1FC (18 functions)
 */

#include "dolphin/types.h"

/* ===== GS Engine ===== */
extern void  _threadSwitch(void);           /* GSthread yield */
extern void  fn_800E01D0(void* dst, void* src); /* material copy */
extern void  fn_800FB680(u32 a, u32 b, s32 c, u32 d); /* sound trigger */
extern void  fn_80132A38(u32 effectId, u32 param);     /* effect trigger */
extern void  fn_801240C4(void* obj, u16 species, u32 form); /* people set species */
extern void  fn_8011DFE0(void* obj, u32 pos);  /* people set position */
extern void  fn_8011DF90(void* obj, u32 rot);  /* people set rotation */
extern u32   fn_8011394C(void);            /* floor state query */
extern void* fn_8011396C(void);            /* floor model get */

/* ===== Battle / Item data ===== */
extern void  fn_801EED88(u16 itemId);      /* Item info lookup */
extern u32   fn_801EE544(u16 itemId);      /* Item price get */
extern void  fn_801EEF40(u16 itemId);      /* Item category get */

/* ===== BSS data ===== */
extern u8    lbl_803A6610[];   /* Shop extended data (0x138 bytes) */
extern u8    lbl_803A6748[];   /* Shop state structure (0xA0 bytes) */

/* ===== SDA data ===== */
extern u32   lbl_8047A4D4;     /* Shop item table pointer */

/*
 * Functions in this translation unit (18 total):
 *
 * fn_8003AEF0  0x0EC  Shop item info loader (price calc, model setup)
 * fn_8003AFDC  0x2FC  Shop event handler (switch on message IDs)
 * fn_8003B2D8  0x1A0  Shop display updater (calls fn_8003CE1C)
 * fn_8003B478  0x258  Shop inventory renderer
 * fn_8003B6D0  0x144  Shop scroll handler
 * fn_8003B814  0x024  Shop wrapper A (calls fn_8003B85C)
 * fn_8003B838  0x024  Shop wrapper B (calls fn_8003B85C)
 * fn_8003B85C  0x6F8  Shop main update loop
 * fn_8003BF54  0x0E8  Shop cursor handler
 * fn_8003C03C  0x100  Shop selection processor
 * fn_8003C13C  0x0E0  Shop item detail display
 * fn_8003C21C  0x030  Shop accessor
 * fn_8003C24C  0x06C  Shop sub-handler (calls fn_8003C2B8)
 * fn_8003C2B8  0x470  Buy/sell confirmation dialog (calls fn_8003CE1C)
 * fn_8003C728  0x098  Shop result handler
 * fn_8003C7C0  0x65C  Transaction processor (wallet update, lbl_803A6610)
 * fn_8003CE1C  0x11C  Internal shop helper
 * fn_8003CF38  0x2C4  Shop cleanup / finalize
 */


/* 0x8003AEF0 | size: 0xEC */
asm void fn_8003AEF0(void) { nofralloc
    #include "asm/GC6E01/nonmatching/menu_shop/fn_8003AEF0.s"
}

/* 0x8003B85C | size: 0x6F8 */
asm void fn_8003B85C(void) { nofralloc
    #include "asm/GC6E01/nonmatching/menu_shop/fn_8003B85C.s"
}

