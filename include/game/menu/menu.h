/**
 * @file menu.h
 * @brief Menu system structures, state machine, and common API.
 *
 * The menu system in Pokemon Colosseum uses a callback-driven architecture
 * with a central menu stack managed by menuCB_Common. Each menu screen is
 * identified by an integer menu ID and has associated callback functions
 * for initialization, per-frame update, input handling, and cleanup.
 *
 * The naming conventions are taken directly from assert strings in the
 * binary (rodata):
 *   - _CBC  = "Common Battle Callbacks" context (menuCB_Common work area)
 *   - _CB   = "Colosseum Battle" context (menuCB_Battle work area)
 *   - _CARDE = Card e-Reader work area
 *
 * Key assert strings and their source files:
 *   menuCB_Common.c: "0 < _CBC.m_nMenuStackDepth"
 *   menuCB_Common.c: "_menuPush(int eMenuID):stack over."
 *   menuCB_Common.c: "_menuPop():stack under."
 *   menuCB_Battle.c: "FIGHT_ENCOUNTER_DATA_null != null"
 *   menuCB_Battle.c: "0 <= eRuleType && _LENGTH(_CB.m_aRule) > eRuleType"
 *   menuToolBattle.c: "BATTLEMODE_BATTLEYAMA100 == _CB.m_eBattleMode"
 *   menuPokeCoupon.c: "POKECOUPONREFER_INVALID != _menuPokeCouponWork.m_eRefer"
 *   menuCardE.c: "_CARDE.card_type == CARDE_CARDTYPE_TRAINER"
 *   menuCardE_Matrix.c: "i < cem->m_seriesN", "!cem->m_isAnimating"
 *
 * Code ranges:
 *   menuCB_Common:      0x8007109C - 0x8007162C (core) + helpers to ~0x80072A00
 *   menuCB_Battle:      0x80069A60 - 0x80069C0C
 *   menuToolBattle:     0x8007581C - 0x80075A34
 *   menuCB_Rule:        0x800767B8 - 0x80077A5C
 *   menuExDiscShrine:   0x80077ED4 - 0x80078390
 *   menuExDiscCoupon:   0x800792D8 - 0x800798E8
 *   menuPokeCoupon:     0x8007C2C0 - 0x8007C300
 *   menuCardE:          0x80033278 - 0x80034280
 *   menuCardE_Matrix:   0x8007C300 - 0x8007FD64
 */

#ifndef GAME_MENU_H
#define GAME_MENU_H

#include "dolphin/types.h"

/* =========================================================================
 * Constants
 * ========================================================================= */

/** Maximum depth of the menu stack.
 *  Inferred from the push/pop assert strings and typical GCN menu systems. */
#define MENU_STACK_MAX 16

/** Menu ID for "no menu" / invalid */
#define MENU_ID_NONE (-1)

/* -------------------------------------------------------------------------
 * Battle mode constants
 * From assert: "BATTLEMODE_BATTLEYAMA100 == _CB.m_eBattleMode"
 * "YAMA100" = "Mountain 100" = Mt. Battle 100 Trainer Challenge
 * ------------------------------------------------------------------------- */
#define BATTLEMODE_BATTLEYAMA100  0  /* Mt. Battle 100-trainer challenge */
#define BATTLEMODE_COLOSSEUM      1  /* Standard Colosseum battle */
#define BATTLEMODE_STORY          2  /* Story mode battle */
#define BATTLEMODE_COUNT          3

/* -------------------------------------------------------------------------
 * Poke Coupon reference types
 * From assert: "POKECOUPONREFER_INVALID != _menuPokeCouponWork.m_eRefer"
 * ------------------------------------------------------------------------- */
#define POKECOUPONREFER_INVALID   (-1)
#define POKECOUPONREFER_SHOP       0  /* Poke Coupon shop */
#define POKECOUPONREFER_EXCHANGE   1  /* Coupon exchange counter */

/* -------------------------------------------------------------------------
 * Card e-Reader card types
 * From assert: "_CARDE.card_type == CARDE_CARDTYPE_TRAINER"
 * ------------------------------------------------------------------------- */
#define CARDE_CARDTYPE_NONE       0
#define CARDE_CARDTYPE_TRAINER    1  /* Virtual trainer card */
#define CARDE_CARDTYPE_POKEMON    2  /* Shadow Pokemon card */
#define CARDE_CARDTYPE_ITEM       3  /* Item card */

/* -------------------------------------------------------------------------
 * Rule types for battle rules menu
 * From assert: "0 <= eRuleType && _LENGTH(_CB.m_aRule) > eRuleType"
 * ------------------------------------------------------------------------- */
#define RULE_TYPE_LEVEL      0  /* Level restriction rule */
#define RULE_TYPE_ITEMS      1  /* Item usage rule */
#define RULE_TYPE_SPECIES    2  /* Species restriction rule */
#define RULE_TYPE_COUNT      3

/* =========================================================================
 * Forward declarations
 * ========================================================================= */

struct MenuCallback;
struct MenuCommonWork;
struct MenuBattleWork;
struct MenuPokeCouponWork;
struct MenuCardEWork;
struct MenuCardEEntry;
struct MenuCardEMatrixContext;
struct MenuCardEMatrixWork;

/* Shared battle-menu helpers implemented by the middle/common menu units. */
u8* savedataBattleGetRulePtr(void);
s32 fn_80076054(void* hero, void* battleRules);

/* =========================================================================
 * Callback function pointer types
 * ========================================================================= */

/** Menu callback function: called each frame while the menu is active.
 *  Returns 0 to continue, nonzero to signal completion. */
typedef s32 (*MenuCallbackFunc)(void);

/** Menu init function: called once when the menu is pushed. */
typedef void (*MenuInitFunc)(void);

/** Menu exit function: called once when the menu is popped. */
typedef void (*MenuExitFunc)(void);

/* =========================================================================
 * Menu callback entry
 * Each menu ID maps to a set of callbacks for its lifecycle.
 * ========================================================================= */
typedef struct MenuCallback {
    MenuInitFunc     pfnInit;       /* 0x00: Called on menu push */
    MenuCallbackFunc pfnUpdate;     /* 0x04: Called each frame */
    MenuCallbackFunc pfnDraw;       /* 0x08: Called each frame for rendering */
    MenuExitFunc     pfnExit;       /* 0x0C: Called on menu pop */
} MenuCallback;

/* =========================================================================
 * Menu stack entry
 * Stored in _CBC.m_aMenuStack[].
 * ========================================================================= */
typedef struct MenuStackEntry {
    s32  eMenuID;       /* 0x00: Menu identifier */
    s32  nParam;        /* 0x04: Parameter passed to this menu */
} MenuStackEntry;

/* =========================================================================
 * menuCB_Common work area (_CBC)
 *
 * Central state for the menu state machine. The assert string
 * "0 < _CBC.m_nMenuStackDepth" confirms the struct prefix "_CBC" and
 * the member name "m_nMenuStackDepth".
 *
 * The push/pop error strings reveal the stack-based architecture:
 *   "_menuPush(int eMenuID):stack over."
 *   "_menuPop():stack under."
 * ========================================================================= */
typedef struct MenuCommonWork {
    /* 0x00 */ s32            m_nMenuStackDepth;  /* Current stack depth (>= 0) */
    /* 0x04 */ MenuStackEntry m_aMenuStack[MENU_STACK_MAX]; /* Menu stack entries */
    /* 0x84 */ s32            m_eCurrentMenuID;   /* ID of the currently active menu */
    /* 0x88 */ s32            m_eNextMenuID;       /* Pending menu transition (-1 if none) */
    /* 0x8C */ s32            m_nTransitionState;  /* Transition animation state */
    /* 0x90 */ s32            m_nTransitionTimer;  /* Frame counter for transitions */
    /* 0x94 */ u32            m_nInputFlags;       /* Processed controller input */
    /* 0x98 */ u32            m_nInputRepeat;      /* Auto-repeat input state */
    /* 0x9C */ s32            m_nCursorPos;        /* Current cursor position */
    /* 0xA0 */ s32            m_nCursorMax;        /* Maximum cursor position */
    /* 0xA4 */ u32            m_bIsActive;         /* TRUE if menu system is running */
    /* 0xA8 */ void*          m_pMenuData;         /* Pointer to loaded menu resource data */
    /* 0xAC */ void*          m_pFontData;         /* Pointer to font resource */
} MenuCommonWork;

/* =========================================================================
 * menuCB_Battle work area (_CB)
 *
 * State for the Colosseum/Battle menu system. Assert strings reveal:
 *   "_CB.m_eBattleMode" - battle mode enum
 *   "_CB.m_aRule" - array of battle rules
 *
 * "FIGHT_ENCOUNTER_DATA_null != null" - encounter data validation
 * "0 <= eRuleType && _LENGTH(_CB.m_aRule) > eRuleType" - rule bounds check
 * ========================================================================= */
typedef struct MenuBattleWork {
    /* 0x00 */ s32   m_eBattleMode;       /* BATTLEMODE_* constant */
    /* 0x04 */ s32   m_eSelectedRule;      /* Currently selected rule index */
    /* 0x08 */ s32   m_aRule[RULE_TYPE_COUNT]; /* Rule settings array */
    /* 0x14 */ s32   m_nCursorPos;         /* Cursor position in battle menu */
    /* 0x18 */ s32   m_nNumEntries;        /* Number of visible menu entries */
    /* 0x1C */ void* m_pEncounterData;     /* Fight encounter data pointer (assert: != null) */
    /* 0x20 */ s32   m_eColosseumID;       /* Which Colosseum is selected */
    /* 0x24 */ s32   m_nRound;             /* Current round number */
    /* 0x28 */ s32   m_nTrainerIndex;      /* Index of the opponent trainer */
    /* 0x2C */ u32   m_bIsDoubleBattle;    /* TRUE for double battle */
    /* 0x30 */ s32   m_nPartySize;         /* Player's party size */
    /* 0x34 */ u32   m_nStatusFlags;       /* Status/state flags */
} MenuBattleWork;

/* =========================================================================
 * menuPokeCoupon work area (_menuPokeCouponWork)
 *
 * From assert: "POKECOUPONREFER_INVALID != _menuPokeCouponWork.m_eRefer"
 * The struct is named exactly "_menuPokeCouponWork" and member "m_eRefer".
 * ========================================================================= */
typedef struct MenuPokeCouponWork {
    /* 0x00 */ s32   m_eRefer;             /* POKECOUPONREFER_* constant */
    /* 0x04 */ s32   m_nCursorPos;         /* Selected item in the shop list */
    /* 0x08 */ s32   m_nScrollOffset;      /* Scroll position */
    /* 0x0C */ s32   m_nNumItems;          /* Total items available */
    /* 0x10 */ s32   m_nSelectedItem;      /* Item ID of current selection */
    /* 0x14 */ s32   m_nCouponBalance;     /* Player's current Poke Coupon count */
    /* 0x18 */ s32   m_nItemPrice;         /* Cost of the selected item */
    /* 0x1C */ u32   m_bConfirmDialog;     /* TRUE if confirmation dialog is shown */
} MenuPokeCouponWork;

/* =========================================================================
 * menuCardE work area (_CARDE)
 *
 * From assert: "_CARDE.card_type == CARDE_CARDTYPE_TRAINER"
 * ========================================================================= */
typedef struct MenuCardEWork {
    /* 0x00 */ s32   card_type;            /* CARDE_CARDTYPE_* constant */
    /* 0x04 */ s32   m_nState;             /* Processing state */
    /* 0x08 */ s32   m_nResult;            /* Processing result code */
    /* 0x0C */ void* m_pCardData;          /* Pointer to scanned card data */
    /* 0x10 */ s32   m_nDataSize;          /* Size of card data */
    /* 0x14 */ u32   m_bIsProcessing;      /* TRUE while e-Reader is scanning */
    /* 0x18 */ s32   m_nErrorCode;         /* Error code from e-Reader */
} MenuCardEWork;

/* =========================================================================
 * Card-E matrix raw matching overlays
 *
 * These partial layouts are anchored by menuCardEMatrixSelectCard, menuCardEMatrixSetTarget,
 * fn_8007C7EC, menuCardEMatrixWindowProc, menuCardE_CompareEntryPtrs, and fn_8007FDBC.
 * They intentionally stop at supported offsets and leave weak fields as
 * unk_XX. The older
 * MenuCardEMatrixWork below remains a high-level sketch used by exploratory
 * code; use MenuCardEMatrixContext when working from raw matching offsets.
 * ========================================================================= */

typedef struct MenuCardEEntry {
    /* 0x00 */ u8  unk_00[0x1A];
    /* 0x1A */ u8  cardId;                 /* Compared after sortGroup by menuCardE_CompareEntryPtrs */
    /* 0x1B */ u8  unk_1B;
    /* 0x1C */ s8  sortGroup;              /* Primary signed sort key in menuCardE_CompareEntryPtrs */
    /* 0x1D */ u8  unk_1D;                 /* Used as sub-entry count in matrix loops */
} MenuCardEEntry;

typedef struct MenuCardEMatrixContext {
    /* 0x000 */ u8                unk_000[0xA0];
    /* 0x0A0 */ s32               prevEntryIndex;
    /* 0x0A4 */ s32               currentEntryIndex;
    /* 0x0A8 */ u8                unk_A8[0x04]; /* includes 0xAA lookup key */
    /* 0x0AC */ s32               entryCount;
    /* 0x0B0 */ MenuCardEEntry**  entries;
    /* 0x0B4 */ u8                prevSubIndex;
    /* 0x0B5 */ u8                currentSubIndex;
    /* 0x0B6 */ u8                transitionActive;
    /* 0x0B7 */ u8                unk_B7;
    /* 0x0B8 */ s32               transitionFrame;
    /* 0x0BC */ s32               unk_BC;  /* Switch selector for jumptable_802EE868 */
} MenuCardEMatrixContext;

/* =========================================================================
 * menuCardE_Matrix work area (cem)
 *
 * From asserts: "i < cem->m_seriesN", "!cem->m_isAnimating", "s[ANIM_cur]"
 * The variable name "cem" and member names are from the asserts. This is a
 * high-level sketch; see MenuCardEMatrixContext for raw offset evidence.
 * ========================================================================= */

/** Animation indices for the card matrix display */
#define ANIM_cur    0
#define ANIM_prev   1
#define ANIM_COUNT  2

typedef struct MenuCardEMatrixWork {
    /* 0x00 */ s32   m_seriesN;            /* Number of card series */
    /* 0x04 */ s32   m_nCurrentSeries;     /* Currently displayed series */
    /* 0x08 */ s32   m_nCursorX;           /* Grid cursor X position */
    /* 0x0C */ s32   m_nCursorY;           /* Grid cursor Y position */
    /* 0x10 */ s32   m_nGridCols;          /* Grid columns */
    /* 0x14 */ s32   m_nGridRows;          /* Grid rows */
    /* 0x18 */ BOOL  m_isAnimating;        /* TRUE during transition animation */
    /* 0x1C */ s32   m_nAnimTimer;         /* Animation frame counter */
    /* 0x20 */ void* m_pSeriesData;        /* Pointer to series data array */
    /* 0x24 */ void* m_pDisplayList;       /* HSD display list for the matrix grid */
} MenuCardEMatrixWork;

/* =========================================================================
 * Global work area instances (BSS)
 *
 * These are the static work areas referenced by the assert strings.
 * The exact BSS addresses are inferred from SDA references in the
 * disassembly.
 * ========================================================================= */

extern MenuCommonWork     _CBC;                 /* menuCB_Common work area */
extern MenuBattleWork     _CB;                  /* menuCB_Battle work area */
extern MenuPokeCouponWork _menuPokeCouponWork;  /* menuPokeCoupon work area */
extern MenuCardEWork      _CARDE;               /* menuCardE work area */

/* =========================================================================
 * Core menu/window API (menu.c)
 * ========================================================================= */

u32 windowGetActiveID(void);
s32 menuOpenCustom(void* menuId, u32 parentId, s32* cursorOut,
                   s32 closeFlags, void* checkCursor, s32 openParam, ...);

/* menuOpenCustom's pointer-typed ABI carries integer menu IDs and flags. */
#define MENU_ID(value) ((void*)(value))
#define MENU_CURSOR_CHECK(value) ((void*)(value))

/* =========================================================================
 * menuCB_Common API (0x8007109C - 0x8007162C)
 *
 * These are the core menu framework functions. All other menus use
 * _menuPush/_menuPop to navigate.
 * ========================================================================= */

/**
 * menuCommonFree: Validates that the menu stack depth is positive.
 * Assert: "0 < _CBC.m_nMenuStackDepth" (file: "menuCB_Common.c")
 */
void menuCB_Common_ValidateStackNotEmpty(void);

/**
 * menuCommonAlloc: Validates the current menu ID is valid.
 * Assert: uses menuCB_Common.c source reference
 */
void menuCB_Common_ValidateMenuID(void);

/**
 * fn_80071160: Retrieves the current menu ID from the top of stack.
 * Size: 0xA8
 */
s32 menuCB_Common_GetCurrentMenuID(void);

/**
 * fn_80071208: Main per-frame update dispatcher for the menu system.
 * Calls the appropriate callback based on current menu state.
 * Size: 0x110
 */
void menuCB_Common_Update(void);

/**
 * fn_80071318: Returns the current menu stack depth.
 * Size: 0x2C
 */
s32 menuCB_Common_GetStackDepth(void);

/**
 * fn_80071344: Checks whether a menu transition is in progress.
 * Size: 0x54
 */
BOOL menuCB_Common_IsTransitioning(void);

/**
 * _menuPop_80071398: Pushes a new menu onto the stack.
 * Assert: "_menuPush(int eMenuID):stack over." (file: "menuCB_Common.c")
 * Size: 0x130
 */
void _menuPush(s32 eMenuID);

/**
 * _menuPop: Pops the current menu from the stack.
 * Assert: "_menuPop():stack under." (file: "menuCB_Common.c")
 * Size: 0xF4
 */
void _menuPop(void);

/**
 * _menuPush: Validates menu state post-transition.
 * Assert: uses "menuCB_Common.c" source reference
 * Size: 0x70
 */
void menuCB_Common_ValidateTransition(void);

/**
 * fn_8007162C: Returns TRUE if the menu system is active.
 * Size: 0x18
 */
BOOL menuCB_Common_IsActive(void);

/**
 * menuCB_InitMenu: Initializes the common menu work area.
 * Size: 0x58
 */
void menuCB_Common_Init(void);

/**
 * fn_8007169C: Shuts down the menu system.
 * Size: 0x2C
 */
void menuCB_Common_Shutdown(void);

/**
 * fn_800716C8: Sets the cursor bounds for the current menu.
 * Size: 0x20
 */
void menuCB_Common_SetCursorBounds(s32 nMax);

/**
 * fn_800716E8: Gets the current cursor position.
 * Size: 0x18
 */
s32 menuCB_Common_GetCursorPos(void);

/* =========================================================================
 * menuCB_Battle API (0x80069A60 - 0x80069C0C + surrounding functions)
 * ========================================================================= */

/**
 * menuCB_Battle: Battle menu callback with rule validation.
 * Assert: "FIGHT_ENCOUNTER_DATA_null != null"
 * Assert: "0 <= eRuleType && _LENGTH(_CB.m_aRule) > eRuleType"
 * File: "menuCB_Battle.c"
 * Size: 0x1AC
 */
void menuCB_Battle_ValidateEncounter(void);

/**
 * menuCBBattleSetupFight: Main battle menu update handler.
 * Size: 0xA50 (very large - likely contains switch/case state machine)
 */
s32 menuCB_Battle_Update(void);

/**
 * menuCBPokemonEntryDispPokemonFace: Battle menu initialization.
 * Size: 0x58
 */
void menuCB_Battle_Init(void);

/**
 * menuCBPokemonEntryTexWorkInit: Battle menu input handler.
 * Size: 0xC4
 */
s32 menuCB_Battle_HandleInput(void);

/* =========================================================================
 * menuToolBattle API (0x8007581C - 0x80075A34)
 * ========================================================================= */

/**
 * menuToolBattleRun: Tool battle menu with battle mode validation.
 * Assert: "BATTLEMODE_BATTLEYAMA100 == _CB.m_eBattleMode"
 * File: "menuToolBattle.c"
 * Size: 0x218
 */
void menuToolBattle_ValidateBattleMode(void);

/* =========================================================================
 * menuCB_Rule API (0x800767B8 - 0x80077A5C)
 * ========================================================================= */

/**
 * menuRuleCheckPartyByRule: Rule menu handler 1 - initialization / first page.
 * File: "menuCB_Rule.c"
 * Size: 0x2D4
 */
void menuCB_Rule_Init(void);

/**
 * menuRuleCheckPokemonAllModes: Rule menu handler 2 - level rules.
 * Size: 0x228
 */
void menuCB_Rule_HandleLevel(void);

/**
 * fn_800774D4: Rule menu handler 3 - item rules.
 * Size: 0x210
 */
void menuCB_Rule_HandleItems(void);

/**
 * menuRuleCheckPartyAndRule: Rule menu handler 4 - species rules / confirmation.
 * Size: 0x378
 */
void menuCB_Rule_HandleSpecies(void);

/* =========================================================================
 * menuExDiscShrine API (0x80077ED4 - 0x80078390)
 * ========================================================================= */

/**
 * menuExDiscShrineRun: Mt. Battle exchange (shrine) menu main handler.
 * File: "menuExDiscShrine.c"
 * Size: 0x4BC
 */
void menuExDiscShrine_Main(void);

/* =========================================================================
 * menuExDiscCoupon API (0x800792D8 - 0x800798E8)
 * ========================================================================= */

/**
 * fn_800792D8: Coupon exchange menu main handler.
 * References GBA thumb code: bg0thumbcode.bin, bg1thumbcode.bin, bg2thumbcode.bin
 * File: "menuExDiscCoupon.c"
 * Size: 0x610
 */
void menuExDiscCoupon_Main(void);

/* =========================================================================
 * menuPokeCoupon API (0x8007C2C0 - 0x8007C300)
 * ========================================================================= */

/**
 * menuPokeCouponInit: Poke Coupon shop validation.
 * Assert: "POKECOUPONREFER_INVALID != _menuPokeCouponWork.m_eRefer"
 * File: "menuPokeCoupon.c"
 * Size: 0x40
 */
void menuPokeCoupon_ValidateRefer(void);

/* =========================================================================
 * menuCardE API (0x80033278 - 0x80034280)
 * ========================================================================= */

/**
 * _sysvarsProcessData__FP16sysvarsFuncEntryPc: Card e-Reader menu main handler.
 * Assert: "_CARDE.card_type == CARDE_CARDTYPE_TRAINER"
 * File: "menuCardE.c"
 * Size: 0x1008 (4104 bytes - large state machine)
 */
s32 menuCardE_Main(void);

/* =========================================================================
 * menuCardE_Matrix API (0x8007C300 - 0x8007FD64)
 *
 * The prototype names in this legacy exploratory section predate the raw
 * offset overlay above. Prefer MenuCardEMatrixContext and symbols.txt
 * Proposed names when working directly on the matching decomp files.
 * ========================================================================= */

/**
 * menuCardEMatrixSelectCard: Sets the current/previous Card-E matrix selection by card id.
 * File: "menuCardE_Matrix.c"
 * Size: 0x114
 */
void menuCardE_Matrix_Init(MenuCardEMatrixWork* cem);

/**
 * menuCardEMatrixSetTarget: Sets a target selection and starts transition state if needed.
 * Size: 0x1E4
 */
void menuCardE_Matrix_ValidateSeries(MenuCardEMatrixWork* cem, s32 i);

/**
 * fn_8007C7EC: Reloads and sorts the Card-E entry pointer array.
 * Size: 0x2C4
 */
void menuCardE_Matrix_ValidateAnim(MenuCardEMatrixWork* cem);

/**
 * menuCardEMatrixWindowProc: Card e matrix main update loop.
 * Size: 0x23EC (9196 bytes - very large state machine)
 */
s32 menuCardE_Matrix_Main(MenuCardEMatrixWork* cem);

/* =========================================================================
 * HSD Assert function (called by all menu assert macros)
 * ========================================================================= */

/**
 * __assert: HSD assertion failure handler.
 * Prints file, line, and condition string then halts/crashes.
 * This is the "assertion \"%s\" failed" handler from HSD.
 */
extern void __assert(const char* file, s32 line, const char* expr);

#endif /* GAME_MENU_H */
