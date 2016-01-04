//#define REPORT_MEM_USAGE
//Form control positioning constants for SiEd
#define SIED_VER_NUMBER 12
#define SIZE_X 113
#define SIZE_Y 26
#define MacPopupLeft 71
#define MacPopupRight 90
#define MacPopupTop 148
#define MacPopupBottom 158

#define REPLACE_BUTTON_LEFT 4
#define SIZE_RECT_X 110
#define SIZE_RECT_Y 24
#define FILENAME_WIDTH (MacPopupLeft-BORDER-11)
#define LINE_DISPLAY_POS MacPopupRight+3
#define MAX_LINE_DISPLAY_WIDTH 134 - (LINE_DISPLAY_POS)

#define MACRO_RECT_X (MacPopupLeft)
#define MACRO_RECT_Y SiUtility::CurrentScreenHeight-12
#define MACRO_RECT_HEIGHT 11
#define MACRO_RECT_WIDTH (MacPopupRight-MacPopupLeft)
#ifdef DEBUG
#define TEST_MENU_ITEM MENUITEM "Run Tests" ID 6785
#else
#define TEST_MENU_ITEM
#endif
#define MACRO_LIST_WIDTH 114
#define KEYBOARD_TOP 80
#define KEYBOARD_DONE_LEFT 6
#define KEYBOARD_DONE_TOP 65
#define CapsAlphaKeyboardDialog 5400
#define CapsIntlKeyboardDialog 5500
#define KeyboardDoneButton 5401
#define Alpha_select 5402
#define Num_select 5403
#define Intl_select 5404
#define CapsKey 5405
#define TabKey 5406
#define ReturnKey 5407
#define SpaceKey 5408
#define ShiftKey 5409
#define DeleteKey 5410
#define CursorKeyUp 5411
#define CursorKeyDown 5412
#define CursorKeyLeft 5413
#define CursorKeyRight 5414
#define RETURN_KEY_BMP 5211
#define SMALL_RETURN_BMP 5221

#define REPLACE_FORM_TOP 85
#define REPLACE_MATCH_CASE_X 0
#define REPLACE_MATCH_CASE_Y PrevBottom+2
#define REPLACE_WHOLE_WORD_X 0
#define REPLACE_WHOLE_WORD_Y PrevBottom+2
#define REPLACE_FROM_TOP_X PrevRight+2
#define REPLACE_FROM_TOP_Y PrevTop

#define REPLACE_FORM_BUTTON_WIDTH 36
#define REPLACE_FIND_BUTTON_TOP PrevBottom+5
#define REPLACE_FIND_BUTTON_LEFT 1
#define REPLACE_REPLACE_BUTTON_TOP (PrevTop)
#define REPLACE_REPLACE_BUTTON_LEFT PrevRight+2
#define REPLACE_ALL_BUTTON_TOP PrevTop
#define REPLACE_ALL_BUTTON_LEFT PrevRight+2
#define REPLACE_EXIT_BUTTON_TOP PrevTop
#define REPLACE_EXIT_BUTTON_LEFT PrevRight+2

#ifdef LANGUAGE_PORTUGUESE
#define FILE_LIST_WIDTH 99
#define FILE_BUTTON_WIDTH 48

#elif LANGUAGE_NORWEGIAN
#define FILE_LIST_WIDTH 94
#define FILE_BUTTON_WIDTH 48
#elif LANGUAGE_FRENCH

#define FILE_LIST_WIDTH 99
#define FILE_BUTTON_WIDTH 34

#elif LANGUAGE_CATALAN
#define FILE_LIST_WIDTH 99
#define FILE_BUTTON_WIDTH 48
#elif LANGUAGE_GERMAN
#define FILE_LIST_WIDTH 99
#define FILE_BUTTON_WIDTH 48
#define REPLACE_FORM_BUTTON_WIDTH  38
#elif LANGUAGE_SPANISH
#define FILE_LIST_WIDTH 99
#define FILE_BUTTON_WIDTH 48

#else
#define FILE_LIST_WIDTH 103
#define FILE_BUTTON_WIDTH 45

#endif

//fonts
#define tinyFont 3901
#define smallFont 3902
#define narrowFixedFont 3903
#define stdFixedFont 3904
#define tinyHiresFont 3905
#define smallHiresFont 3906
#define stdFixedHiresFont 3907
#define smallBoldHiresFont 3908
