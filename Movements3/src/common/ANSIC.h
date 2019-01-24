#pragma once

//https://misc.flogisoft.com/bash/tip_colors_and_formatting

//Stringyfier for c preproccessor
#define ANSIC_Str1(X) #X
#define ANSIC_Str(X) ANSIC_Str1 (X)


//ANSI text color escape string formatter
#define ANSIC(Text_Attribute, Foreground_Color, Background_Color) \
"\x1B[" ANSIC_Str (Text_Attribute) ";3" ANSIC_Str (Foreground_Color) ";4" ANSIC_Str (Background_Color) "m"


//Used for text attributes
#define ANSIC_NORMAL       0
#define ANSIC_BOLD         1
#define ANSIC_UNDERSCORE   4
#define ANSIC_BLINK        5
#define ANSIC_REVERSE      7
#define ANSIC_CONCEAL      8


//Used for foreground and background color
#define ANSIC_BLACK   0
#define ANSIC_RED     1
#define ANSIC_GREEN   2
#define ANSIC_YELLOW  3
#define ANSIC_BLUE    4
#define ANSIC_MAGENTA 5
#define ANSIC_CYAN    6
#define ANSIC_WHITE   7
#define ANSIC_DEFAULT 9


//Default text color used in terminals
//#define ANSIC_DEFAULT ANSIC (ANSIC_NORMAL, ANSIC_DEFAULT, ANSIC_DEFAULT)
#define ANSIC_RESET "\x1B[0m"
