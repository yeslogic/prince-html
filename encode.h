// Copyright (C) 2011-2012 YesLogic Pty. Ltd.
// Released as Open Source (see COPYING.txt for details)


#define CAPITAL_M 0x4D
#define	SMALL_M 0x6D
#define CAPITAL_E 0x45
#define SMALL_E 0x65
#define CAPITAL_T 0x54
#define SMALL_T 0x74


/*file_name is the input file.
  returns a string representing the character encoding, or NULL if no character encoding can be found.
  the returned string should be freed after use.*/
unsigned char *get_file_encoding(unsigned char *file_name);

/*returns a string representing the character encoding, or NULL if no character encoding can be found.
  the returned string should be freed after use.*/
unsigned char *get_encoding(unsigned char *file_buffer, long buffer_length);