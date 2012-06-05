// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#define AMPERSAND 0x26
#define CHARACTER_TABULATION 0x09
#define LINE_FEED 0x0A
#define FORM_FEED 0x0C
#define CARRIAGE_RETURN 0x0D
#define SPACE 0x20
#define SOLIDUS 0x2F
#define GREATER_THAN_SIGN 0x3E
#define LESS_THAN_SIGN 0x3C
#define NULL_CHARACTER 0x00
#define EXCLAMATION_MARK 0x21
#define QUESTION_MARK 0x3F
#define HYPHEN_MINUS 0x2D
#define QUOTATION_MARK 0x22
#define APOSTROPHE 0x27
#define EQUALS_SIGN 0x3D
#define GRAVE_ACCENT 0x60
#define NUMBER_SIGN 0x23
#define SEMICOLON 0x3B
#define CAPITAL_A 0x41
#define CAPITAL_Z 0x5A
#define SMALL_A 0x61
#define SMALL_Z 0x7A
#define CAPITAL_TO_SMALL 0x20
#define DIGIT_ZERO 0x30
#define DIGIT_NINE 0x39




/*if char_to_check is a named character, evalue code_point and return 1, otherwise return 0*/
int is_named_char_ref(unsigned char * char_to_check, unsigned long *code_point);

/*if char_to_check is a named character and has a second code point, evaluate code_point and return 1, otherwise return 0*/
int has_second_code_point(unsigned char *char_to_check, unsigned long *code_point);

//return 1 if code point is disallowed, otherwise return 0;
int code_point_disallowed(unsigned long code_point);